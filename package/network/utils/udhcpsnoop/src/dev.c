// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Felix Fietkau <nbd@nbd.name>
 */
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <libubox/vlist.h>
#include <libubox/avl-cmp.h>

#include "dhcpsnoop.h"

#define APPEND(_buf, _ofs, _format, ...) _ofs += snprintf(_buf + _ofs, sizeof(_buf) - _ofs, _format, ##__VA_ARGS__)

struct vlan_hdr {
	uint16_t tci;
	uint16_t proto;
};

struct gre_hdr {
	uint16_t flags;
	uint16_t proto;
};

struct packet {
	void *buffer;
	unsigned int len;
};


struct device {
	struct vlist_node node;
	char ifname[IFNAMSIZ + 1];

	int ifindex;
	bool ingress;
	bool egress;

	bool changed;
	bool active;
};

static void dev_update_cb(struct vlist_tree *tree, struct vlist_node *node_new,
			  struct vlist_node *node_old);

static struct uloop_fd ufd;
static VLIST_TREE(devices, avl_strcmp, dev_update_cb, true, false);

static void *pkt_peek(struct packet *pkt, unsigned int len)
{
	if (len > pkt->len)
		return NULL;

	return pkt->buffer;
}


static void *pkt_pull(struct packet *pkt, unsigned int len)
{
	void *ret = pkt_peek(pkt, len);

	if (!ret)
		return NULL;

	pkt->buffer += len;
	pkt->len -= len;

	return ret;
}

static bool
proto_is_vlan(uint16_t proto)
{
	return proto == ETH_P_8021Q || proto == ETH_P_8021AD;
}

static void
dhcpsnoop_packet_cb(struct packet *pkt)
{
	struct ethhdr *eth;
	struct ip6_hdr *ip6;
	struct ip *ip;
	struct udphdr *udp;
	uint16_t proto, port;
	const char *type;
	bool ipv6 = false;
	uint32_t rebind = 0;

inside_tunnel:
	eth = pkt_pull(pkt, sizeof(*eth));
	if (!eth)
		return;

	proto = be16_to_cpu(eth->h_proto);
	if (proto_is_vlan(proto)) {
		struct vlan_hdr *vlan;

		vlan = pkt_pull(pkt, sizeof(*vlan));
		if (!vlan)
			return;

		proto = be16_to_cpu(vlan->proto);
	}

	switch (proto) {
	case ETH_P_IP:
		ip = pkt_peek(pkt, sizeof(struct ip));
		if (!ip)
			return;

		if (!pkt_pull(pkt, ip->ip_hl * 4))
			return;

		proto = ip->ip_p;
		break;
	case ETH_P_IPV6:
		ip6 = pkt_pull(pkt, sizeof(*ip6));
		if (!ip6)
			return;

		proto = ip6->ip6_nxt;
		ipv6 = true;
		break;
	default:
		return;
	}

	if (proto == IPPROTO_GRE) {
		struct gre_hdr *gre;
		gre = pkt_pull(pkt, sizeof(*gre));
		if (!gre) return;
		proto = be16_to_cpu(gre->proto);
		if (proto != 0x6558) return;
		goto inside_tunnel;
	}

	if (proto != IPPROTO_UDP)
		return;

	udp = pkt_pull(pkt, sizeof(struct udphdr));
	if (!udp)
		return;

	port = ntohs(udp->uh_sport);

	if (!ipv6)
		type = dhcpsnoop_parse_ipv4(pkt->buffer, pkt->len, port, &rebind);
	else
		type = dhcpsnoop_parse_ipv6(pkt->buffer, pkt->len, port);

	if (!type)
		return;

	dhcpsnoop_ubus_notify(type, pkt->buffer, pkt->len);
	if (!ipv6 && !strcmp(type, "ack") && rebind)
		cache_entry(pkt->buffer, rebind);
}

static void
dhcpsnoop_socket_cb(struct uloop_fd *fd, unsigned int events)
{
	static uint8_t buf[8192];
	struct packet pkt = {
		.buffer = buf,
	};
	int len;

retry:
	len = recvfrom(fd->fd, buf, sizeof(buf), MSG_DONTWAIT, NULL, NULL);
	if (len < 0) {
		if (errno == EINTR)
			goto retry;
		return;
	}

	if (!len)
		return;

	pkt.len = len;
	dhcpsnoop_packet_cb(&pkt);
}

static int
dhcpsnoop_open_socket(void)
{
	struct sockaddr_ll sll = {
		.sll_family = AF_PACKET,
		.sll_protocol = htons(ETH_P_ALL),
	};
	int sock;

	sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock == -1) {
		ULOG_ERR("failed to create raw socket: %s\n", strerror(errno));
		return -1;
	}

	sll.sll_ifindex = if_nametoindex(DHCPSNOOP_IFB_NAME);
	if (bind(sock, (struct sockaddr *)&sll, sizeof(sll))) {
		ULOG_ERR("failed to bind socket to "DHCPSNOOP_IFB_NAME": %s\n",
			 strerror(errno));
		goto error;
	}

	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL) | O_NONBLOCK);

	ufd.fd = sock;
	ufd.cb = dhcpsnoop_socket_cb;
	uloop_fd_add(&ufd, ULOOP_READ);

	return 0;

error:
	close(sock);
	return -1;
}

static int
prepare_filter_cmd(char *buf, int len, const char *dev, int prio, bool add, bool egress)
{
	return snprintf(buf, len, "tc filter %s dev '%s' %sgress prio %d",
			add ? "add" : "del", dev, egress ? "e" : "in", prio);
}

#define MATCH_GRE_ETH_IP_UDP_DHCP_67 \
	" match u16 0x6558 0xffff at 22 " \
	" match u16 0x0800 0xffff at 36 " \
	" match u8 17 0xff at 47 " \
	" match u16 67 0xffff at 58 "

#define MATCH_GRE_ETH_VLAN_IP_UDP_DHCP_67 \
	" match u16 0x6558 0xffff at 22 " \
	" match u16 0x8100 0xffff at 36 " \
	" match u16 0x0800 0xffff at 40 " \
	" match u8 17 0xff at 51 " \
	" match u16 67 0xffff at 62 "

#define MATCH_GRE_ETH_IP_UDP_DHCP_68 \
	" match u16 0x6558 0xffff at 22 " \
	" match u16 0x0800 0xffff at 36 " \
	" match u8 17 0xff at 47 " \
	" match u16 68 0xffff at 58 "

#define MATCH_GRE_ETH_VLAN_IP_UDP_DHCP_68 \
	" match u16 0x6558 0xffff at 22 " \
	" match u16 0x8100 0xffff at 36 " \
	" match u16 0x0800 0xffff at 40 " \
	" match u8 17 0xff at 51 " \
	" match u16 68 0xffff at 62 "

static void
dhcpsnoop_dev_attach_filters(struct device *dev, bool egress)
{
	int prio = DHCPSNOOP_PRIO_BASE;
	char buf[350];
	int ofs;

	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol ip u32 match ip sport 67 0xffff"
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);

	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol 802.1Q u32 offset plus 4 match ip sport 67 0xffff"
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);

	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol ip u32 match ip sport 68 0xffff"
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);

	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol 802.1Q u32 offset plus 4 match ip sport 68 0xffff"
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);

	/* GRE */
	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol ip u32 match ip protocol 47 0xff"
			 MATCH_GRE_ETH_IP_UDP_DHCP_67
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);

	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol ip u32 match ip protocol 47 0xff"
			 MATCH_GRE_ETH_IP_UDP_DHCP_68
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);

	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol ip u32 match ip protocol 47 0xff "
			 MATCH_GRE_ETH_VLAN_IP_UDP_DHCP_67
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);

	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol ip u32 match ip protocol 47 0xff"
			 MATCH_GRE_ETH_VLAN_IP_UDP_DHCP_68
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);

	/* IPv6 */
	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol ipv6 u32 match ip6 sport 546 0xfffe"
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);

	ofs = prepare_filter_cmd(buf, sizeof(buf), dev->ifname, prio++, true, egress);
	APPEND(buf, ofs, " protocol 802.1Q u32 offset plus 4 match ip6 sport 546 0xfffe"
			 " flowid 1:1 action mirred ingress mirror dev " DHCPSNOOP_IFB_NAME " continue");
	dhcpsnoop_run_cmd(buf, false);
}

static void
dhcpsnoop_dev_cleanup_filters(struct device *dev, bool egress)
{
	char buf[128];
	int i;

	for (i = DHCPSNOOP_PRIO_BASE; i < DHCPSNOOP_PRIO_BASE + 10; i++) {
		prepare_filter_cmd(buf, sizeof(buf), dev->ifname, i, false, egress);
		dhcpsnoop_run_cmd(buf, true);
	}
}

static void
dhcpsnoop_dev_attach(struct device *dev)
{
	char buf[64];

	dev->active = true;
	snprintf(buf, sizeof(buf), "tc qdisc add dev '%s' clsact", dev->ifname);
	dhcpsnoop_run_cmd(buf, true);

	if (dev->ingress)
		dhcpsnoop_dev_attach_filters(dev, false);
	if (dev->egress)
		dhcpsnoop_dev_attach_filters(dev, true);
}

static void
dhcpsnoop_dev_cleanup(struct device *dev)
{
	dev->active = false;
	dhcpsnoop_dev_cleanup_filters(dev, true);
	dhcpsnoop_dev_cleanup_filters(dev, false);
}

static void
__dhcpsnoop_dev_check(struct device *dev)
{
	int ifindex;

	ifindex = if_nametoindex(dev->ifname);
	if (ifindex != dev->ifindex) {
		dev->ifindex = ifindex;
		dev->changed = true;
	}

	if (!dev->changed)
		return;

	dev->changed = false;
	dhcpsnoop_dev_cleanup(dev);
	if (ifindex)
		dhcpsnoop_dev_attach(dev);
}

static void dev_update_cb(struct vlist_tree *tree, struct vlist_node *node_new,
			  struct vlist_node *node_old)
{
	struct device *dev = NULL, *dev_free = NULL;

	if (node_old && node_new) {
		dev = container_of(node_old, struct device, node);
		dev_free = container_of(node_new, struct device, node);

		if (dev->ingress != dev_free->ingress ||
			dev->egress != dev_free->egress)
			dev->changed = true;

		dev->ingress = dev_free->ingress;
		dev->egress = dev_free->egress;
	} else if (node_old) {
		dev_free = container_of(node_old, struct device, node);
		if (dev_free->active)
			dhcpsnoop_dev_cleanup(dev_free);
	} else if (node_new) {
		dev = container_of(node_new, struct device, node);
	}

	if (dev)
		__dhcpsnoop_dev_check(dev);
	if (dev_free)
		free(dev_free);
}

static void
dhcpsnoop_dev_config_add(struct blob_attr *data)
{
	enum {
		DEV_ATTR_INGRESS,
		DEV_ATTR_EGRESS,
		__DEV_ATTR_MAX
	};
	static const struct blobmsg_policy policy[__DEV_ATTR_MAX] = {
		[DEV_ATTR_INGRESS] = { "ingress", BLOBMSG_TYPE_BOOL },
		[DEV_ATTR_EGRESS] = { "egress", BLOBMSG_TYPE_BOOL },
	};
	struct blob_attr *tb[__DEV_ATTR_MAX];
	struct blob_attr *cur;
	struct device *dev;
	int len;

	if (blobmsg_type(data) != BLOBMSG_TYPE_TABLE)
		return;

	dev = calloc(1, sizeof(*dev));
	len = snprintf(dev->ifname, sizeof(dev->ifname), "%s", blobmsg_name(data));
	if (!len || len > IFNAMSIZ)
		goto free;

	blobmsg_parse(policy, ARRAY_SIZE(tb), tb, blobmsg_data(data), blobmsg_len(data));

	if ((cur = tb[DEV_ATTR_INGRESS]) != NULL)
		dev->ingress = blobmsg_get_bool(cur);
	if ((cur = tb[DEV_ATTR_EGRESS]) != NULL)
		dev->egress = blobmsg_get_bool(cur);

	if (!dev->ingress && !dev->egress)
		goto free;

	vlist_add(&devices, &dev->node, dev->ifname);
	return;

free:
	free(dev);
	return;
}

void dhcpsnoop_dev_config_update(struct blob_attr *data, bool add_only)
{
	struct blob_attr *cur;
	int rem;

	if (!add_only)
		vlist_update(&devices);

	blobmsg_for_each_attr(cur, data, rem)
		dhcpsnoop_dev_config_add(cur);

	if (!add_only)
		vlist_flush(&devices);
}

void dhcpsnoop_dev_check(void)
{
	struct device *dev;

	vlist_for_each_element(&devices, dev, node)
		__dhcpsnoop_dev_check(dev);
}

int dhcpsnoop_dev_init(void)
{
	dhcpsnoop_dev_done();

	if (dhcpsnoop_run_cmd("ip link add "DHCPSNOOP_IFB_NAME" type ifb", false) ||
	    dhcpsnoop_run_cmd("ip link set dev "DHCPSNOOP_IFB_NAME" up", false) ||
	    dhcpsnoop_open_socket())
		return -1;

	return 0;
}

void dhcpsnoop_dev_done(void)
{
	if (ufd.registered) {
		uloop_fd_delete(&ufd);
		close(ufd.fd);
	}

	dhcpsnoop_run_cmd("ip link del "DHCPSNOOP_IFB_NAME, true);
	vlist_flush_all(&devices);
}
