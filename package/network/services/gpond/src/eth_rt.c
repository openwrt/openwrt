// SPDX-License-Identifier: GPL-2.0
/*
 * global.own_config_only: the Linux devices of the OMCI model that netifd
 * would otherwise be told about in /etc/config/network, created at run time.
 *
 * With own_config_only nothing is written to /etc/config/network,
 * /etc/config/omcproxy or /etc/config/easycwmp; the part of the model that
 * the data path needs - the MAC bridges of ME 45 and the 802.1Q/802.1ad
 * devices - is made here instead, straight through rtnetlink and sysfs, the
 * way netifd does it, so that the link comes up whatever the admin has in
 * the configuration:
 *
 *   - a device that exists and is not ours is never touched: the admin's
 *     approved configuration wins (netifd made it), what is missing from it
 *     is added here and is not saved anywhere;
 *   - a device made here carries "gpond ..." in its ifalias, so that a
 *     restarted gpond (or a switch to own_config_only 0) knows which
 *     devices are its own and removes the ones the OMCI model no longer
 *     has;
 *   - a port that already belongs to another bridge stays there (an admin
 *     LAN bridge is never taken over);
 *   - IP interfaces (ME 134/329), omcproxy and easycwmp are only reported
 *     (eth.c own_config_note()), they need netifd/procd.
 */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_link.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "gpond.h"
#include "eth.h"

#define OWNER_TAG		"gpond"
#define SYSNET			"/sys/class/net"

/* ---- rtnetlink ------------------------------------------------------ */

struct nlb {
	char buf[2048];
	struct nlmsghdr *nh;
	bool full;
};

static void nlb_init(struct nlb *b, uint16_t type, uint16_t flags, int ifindex,
		     unsigned int ifi_flags, unsigned int ifi_change)
{
	struct ifinfomsg *ifm;

	memset(b, 0, sizeof(*b));
	b->nh = (struct nlmsghdr *)b->buf;
	b->nh->nlmsg_len = NLMSG_LENGTH(sizeof(*ifm));
	b->nh->nlmsg_type = type;
	b->nh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | flags;
	ifm = NLMSG_DATA(b->nh);
	ifm->ifi_family = AF_UNSPEC;
	ifm->ifi_index = ifindex;
	ifm->ifi_flags = ifi_flags;
	ifm->ifi_change = ifi_change;
}

static struct rtattr *nlb_put(struct nlb *b, uint16_t type, const void *data,
			      size_t len)
{
	size_t at = NLMSG_ALIGN(b->nh->nlmsg_len);
	struct rtattr *rta;

	if (at + RTA_SPACE(len) > sizeof(b->buf)) {
		b->full = true;
		return NULL;
	}

	rta = (struct rtattr *)(b->buf + at);
	rta->rta_type = type;
	rta->rta_len = RTA_LENGTH(len);
	if (len)
		memcpy(RTA_DATA(rta), data, len);
	b->nh->nlmsg_len = at + RTA_ALIGN(rta->rta_len);

	return rta;
}

static size_t nlb_nest(struct nlb *b, uint16_t type)
{
	size_t off = NLMSG_ALIGN(b->nh->nlmsg_len);

	nlb_put(b, type | NLA_F_NESTED, NULL, 0);

	return off;
}

static void nlb_nest_end(struct nlb *b, size_t off)
{
	struct rtattr *rta = (struct rtattr *)(b->buf + off);

	rta->rta_len = b->nh->nlmsg_len - off;
}

static void nlb_str(struct nlb *b, uint16_t type, const char *s)
{
	nlb_put(b, type, s, strlen(s) + 1);
}

static void nlb_u32(struct nlb *b, uint16_t type, uint32_t v)
{
	nlb_put(b, type, &v, sizeof(v));
}

/* one request, one acknowledgement; 0 or -errno */
static int rt_talk(struct nlb *b)
{
	struct sockaddr_nl sa = { .nl_family = AF_NETLINK };
	struct timeval tv = { .tv_sec = 3 };
	char rsp[2048];
	struct nlmsghdr *nh;
	int fd, ret = -EIO;
	ssize_t n;

	if (b->full)
		return -EMSGSIZE;

	fd = socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE);
	if (fd < 0)
		return -errno;
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	if (sendto(fd, b->buf, b->nh->nlmsg_len, 0, (struct sockaddr *)&sa,
		   sizeof(sa)) < 0) {
		ret = -errno;
		goto out;
	}

	n = recv(fd, rsp, sizeof(rsp), 0);
	if (n < 0) {
		ret = -errno;
		goto out;
	}

	nh = (struct nlmsghdr *)rsp;
	if (NLMSG_OK(nh, (size_t)n) && nh->nlmsg_type == NLMSG_ERROR &&
	    nh->nlmsg_len >= NLMSG_LENGTH(sizeof(struct nlmsgerr)))
		ret = ((struct nlmsgerr *)NLMSG_DATA(nh))->error;
out:
	close(fd);

	return ret;
}

static int link_up(int ifindex)
{
	struct nlb b;

	nlb_init(&b, RTM_NEWLINK, 0, ifindex, IFF_UP, IFF_UP);

	return rt_talk(&b);
}

static int link_del(int ifindex)
{
	struct nlb b;

	nlb_init(&b, RTM_DELLINK, 0, ifindex, 0, 0);

	return rt_talk(&b);
}

/* master 0: release the port */
static int link_master(int ifindex, int master)
{
	struct nlb b;

	nlb_init(&b, RTM_NEWLINK, 0, ifindex, 0, 0);
	nlb_u32(&b, IFLA_MASTER, master);

	return rt_talk(&b);
}

static int bridge_new(const char *name, const char *alias)
{
	struct nlb b;
	size_t li;

	nlb_init(&b, RTM_NEWLINK, NLM_F_CREATE | NLM_F_EXCL, 0, 0, 0);
	nlb_str(&b, IFLA_IFNAME, name);
	nlb_str(&b, IFLA_IFALIAS, alias);
	li = nlb_nest(&b, IFLA_LINKINFO);
	nlb_str(&b, IFLA_INFO_KIND, "bridge");
	nlb_nest_end(&b, li);

	return rt_talk(&b);
}

static void qos_map(struct nlb *b, uint16_t type, int to_fixed)
{
	struct ifla_vlan_qos_mapping m;
	size_t nest;
	int i;

	nest = nlb_nest(b, type);
	for (i = 0; i < 8; i++) {
		m.from = i;
		m.to = to_fixed >= 0 ? (uint32_t)to_fixed : (uint32_t)i;
		nlb_put(b, IFLA_VLAN_QOS_MAPPING, &m, sizeof(m));
	}
	nlb_nest_end(b, nest);
}

static int vlan_new(const struct eth_vlan *v, int parent, const char *alias)
{
	uint16_t id = v->vid, proto = htons(v->tpid);
	struct nlb b;
	size_t li, data;

	nlb_init(&b, RTM_NEWLINK, NLM_F_CREATE | NLM_F_EXCL, 0, 0, 0);
	nlb_u32(&b, IFLA_LINK, parent);
	nlb_str(&b, IFLA_IFNAME, v->name);
	nlb_str(&b, IFLA_IFALIAS, alias);
	li = nlb_nest(&b, IFLA_LINKINFO);
	nlb_str(&b, IFLA_INFO_KIND, "vlan");
	data = nlb_nest(&b, IFLA_INFO_DATA);
	nlb_put(&b, IFLA_VLAN_ID, &id, sizeof(id));
	nlb_put(&b, IFLA_VLAN_PROTOCOL, &proto, sizeof(proto));
	/* netifd vlandev.c: ingress_qos_mapping n:n, egress_qos_mapping */
	if (v->ingress == ING_IDENT)
		qos_map(&b, IFLA_VLAN_INGRESS_QOS, -1);
	if (v->egress == EGR_IDENT)
		qos_map(&b, IFLA_VLAN_EGRESS_QOS, -1);
	else if (v->egress != EGR_NONE)
		qos_map(&b, IFLA_VLAN_EGRESS_QOS, v->egress);
	nlb_nest_end(&b, data);
	nlb_nest_end(&b, li);

	return rt_talk(&b);
}

/* ---- sysfs ---------------------------------------------------------- */

static void sysfs_set(const char *dev, const char *dir, const char *attr,
		      const char *val)
{
	char path[160];
	int fd;

	snprintf(path, sizeof(path), SYSNET "/%s/%s/%s", dev, dir, attr);
	fd = open(path, O_WRONLY | O_CLOEXEC);
	if (fd < 0) {
		DBG("eth: runtime: %s: %s", path, strerror(errno));
		return;
	}
	if (write(fd, val, strlen(val)) < 0)
		DBG("eth: runtime: %s = %s: %s", path, val, strerror(errno));
	close(fd);
}

static void sysfs_set_u(const char *dev, const char *dir, const char *attr,
			unsigned int v)
{
	char val[16];

	snprintf(val, sizeof(val), "%u", v);
	sysfs_set(dev, dir, attr, val);
}

static bool sysfs_read(const char *dev, const char *attr, char *out, size_t len)
{
	char path[160];
	FILE *f;
	size_t n;

	snprintf(path, sizeof(path), SYSNET "/%s/%s", dev, attr);
	f = fopen(path, "r");
	if (!f)
		return false;
	n = fread(out, 1, len - 1, f);
	fclose(f);
	out[n] = '\0';
	while (n && (out[n - 1] == '\n' || out[n - 1] == '\r'))
		out[--n] = '\0';

	return true;
}

static bool dev_exists(const char *name)
{
	return if_nametoindex(name) != 0;
}

/* made by gpond (own_config_only) */
static bool dev_ours(const char *name)
{
	char alias[64];

	return sysfs_read(name, "ifalias", alias, sizeof(alias)) &&
	       !strncmp(alias, OWNER_TAG, strlen(OWNER_TAG));
}

/* the bridge a device is a port of, "" when none */
static void dev_master(const char *name, char *out, size_t len)
{
	char path[160], link[160];
	ssize_t n;
	char *base;

	out[0] = '\0';
	snprintf(path, sizeof(path), SYSNET "/%s/master", name);
	n = readlink(path, link, sizeof(link) - 1);
	if (n <= 0)
		return;
	link[n] = '\0';
	base = strrchr(link, '/');
	snprintf(out, len, "%s", base ? base + 1 : link);
}

/* ---- model lookups -------------------------------------------------- */

static const struct eth_vlan *find_vlan(const struct eth_model *m,
					const char *name)
{
	int i;

	for (i = 0; m && i < m->nvlan; i++)
		if (!strcmp(m->vlan[i].name, name))
			return &m->vlan[i];

	return NULL;
}

static const struct eth_bridge *find_bridge(const struct eth_model *m,
					    const char *name)
{
	int i;

	for (i = 0; m && i < m->nbr; i++)
		if (!strcmp(m->br[i].name, name))
			return &m->br[i];

	return NULL;
}

static bool vlan_same(const struct eth_vlan *a, const struct eth_vlan *b)
{
	return a && b && !strcmp(a->parent, b->parent) && a->vid == b->vid &&
	       a->tpid == b->tpid && a->egress == b->egress &&
	       a->ingress == b->ingress;
}

static bool in_ports(const struct eth_bridge *b, const char *dev)
{
	int i;

	for (i = 0; b && i < b->nport; i++)
		if (!strcmp(b->port[i].dev, dev))
			return true;

	return false;
}

/* ---- options -------------------------------------------------------- */

/* netifd bridge.c: the ME 45 options, seconds and centiseconds as there */
static void bridge_opts(const struct eth_bridge *b)
{
	const char *n = b->name;

	sysfs_set_u(n, "bridge", "stp_state", b->stp);
	if (b->priority)
		sysfs_set_u(n, "bridge", "priority", b->priority);
	if (b->max_age)
		sysfs_set_u(n, "bridge", "max_age", b->max_age * 100);
	if (b->hello_time)
		sysfs_set_u(n, "bridge", "hello_time", b->hello_time * 100);
	if (b->forward_delay)
		sysfs_set_u(n, "bridge", "forward_delay", b->forward_delay * 100);
	if (b->ageing_time)
		sysfs_set_u(n, "bridge", "ageing_time", b->ageing_time * 100);
	sysfs_set_u(n, "bridge", "multicast_snooping", b->igmp);
	if (b->igmp)
		sysfs_set_u(n, "bridge", "multicast_querier", 0);
	if (b->robustness) {
		sysfs_set_u(n, "bridge", "multicast_startup_query_count",
			    b->robustness);
		sysfs_set_u(n, "bridge", "multicast_last_member_count",
			    b->robustness);
	}
	if (b->query_interval)
		sysfs_set_u(n, "bridge", "multicast_query_interval",
			    b->query_interval);
	if (b->query_response)
		sysfs_set_u(n, "bridge", "multicast_query_response_interval",
			    b->query_response);
	if (b->last_member)
		sysfs_set_u(n, "bridge", "multicast_last_member_interval",
			    b->last_member);
}

/* the model's code is netifd's minus one: 0 never, 1 learn, 2 always */
static void port_mcast(const char *dev, bool fast_leave, uint8_t mrouter)
{
	if (fast_leave)
		sysfs_set(dev, "brport", "multicast_fast_leave", "1");
	if (mrouter)
		sysfs_set_u(dev, "brport", "multicast_router", mrouter - 1);
}

static void port_opts(const struct eth_model *m, const struct eth_bridge *b,
		      const struct eth_port *p)
{
	const struct eth_vlan *v = find_vlan(m, p->dev);

	if (v) {
		/* a VLAN device carries its own multicast options */
		port_mcast(p->dev, v->fast_leave, v->mrouter);
		return;
	}
	if (!b->learning)
		sysfs_set(p->dev, "brport", "learning", "0");
	if (b->unknown_discard)
		sysfs_set(p->dev, "brport", "unicast_flood", "0");
	port_mcast(p->dev, p->fast_leave, p->mrouter);
}

/* ---- sync ----------------------------------------------------------- */

static void drop_dev(const char *name, const char *why)
{
	int idx = if_nametoindex(name), ret;

	if (!idx)
		return;
	ret = link_del(idx);
	if (ret)
		WARN("eth: runtime: cannot remove %s: %s", name, strerror(-ret));
	else
		INFO("eth: runtime: %s removed (%s)", name, why);
}

/* the devices of ours that the model does not name (or names differently) */
static void collect(const struct eth_model *cur, const struct eth_model *next)
{
	struct dirent *de;
	DIR *d = opendir(SYSNET);

	if (!d)
		return;

	while ((de = readdir(d))) {
		const struct eth_vlan *nv, *cv;

		if (de->d_name[0] == '.' || !dev_ours(de->d_name))
			continue;

		nv = find_vlan(next, de->d_name);
		cv = find_vlan(cur, de->d_name);
		if (find_bridge(next, de->d_name))
			continue;
		if (next && nv && vlan_same(nv, cv))
			continue;
		/* a stale device of a former run, or no longer wanted */
		drop_dev(de->d_name, nv ? "changed" : "not in the OMCI model");
	}
	closedir(d);
}

static void make_bridge(const struct eth_bridge *b)
{
	char alias[48];
	int ret, idx;

	if (dev_exists(b->name)) {
		if (!dev_ours(b->name))
			INFO("eth: runtime: bridge %s exists and is not ours, "
			     "left alone", b->name);
		return;
	}

	snprintf(alias, sizeof(alias), OWNER_TAG " me 45 0x%04x", b->id);
	ret = bridge_new(b->name, alias);
	if (ret) {
		WARN("eth: runtime: cannot create bridge %s: %s", b->name,
		     strerror(-ret));
		return;
	}
	idx = if_nametoindex(b->name);
	if (idx && (ret = link_up(idx)))
		WARN("eth: runtime: cannot bring %s up: %s", b->name,
		     strerror(-ret));
	INFO("eth: runtime: bridge %s created (ME 45 0x%04x)", b->name, b->id);
}

static bool make_vlan(const struct eth_vlan *v)
{
	int parent, ret, idx;

	if (dev_exists(v->name)) {
		if (!dev_ours(v->name))
			INFO("eth: runtime: %s exists and is not ours, left "
			     "alone", v->name);
		return true;
	}

	parent = if_nametoindex(v->parent);
	if (!parent)
		return false;		/* its parent is made later */

	ret = vlan_new(v, parent, OWNER_TAG " vlan");
	if (ret) {
		WARN("eth: runtime: cannot create %s on %s (vid %u): %s",
		     v->name, v->parent, v->vid, strerror(-ret));
		return true;
	}
	idx = if_nametoindex(v->name);
	if (idx && (ret = link_up(idx)))
		WARN("eth: runtime: cannot bring %s up: %s", v->name,
		     strerror(-ret));
	INFO("eth: runtime: %s created on %s (vid %u, tpid 0x%04x)", v->name,
	     v->parent, v->vid, v->tpid);

	return true;
}

/* the ports of a bridge of ours: the missing ones in, the dropped ones out */
static void sync_ports(const struct eth_model *cur, const struct eth_model *next,
		       const struct eth_bridge *b)
{
	const struct eth_bridge *old = find_bridge(cur, b->name);
	int i, idx = if_nametoindex(b->name), ret;

	if (!idx || !dev_ours(b->name))
		return;

	for (i = 0; old && i < old->nport; i++) {
		const char *dev = old->port[i].dev;
		char master[IFNAMSIZ];

		if (in_ports(b, dev) || !dev_exists(dev))
			continue;
		dev_master(dev, master, sizeof(master));
		if (!strcmp(master, b->name) &&
		    !(ret = link_master(if_nametoindex(dev), 0)))
			INFO("eth: runtime: %s left bridge %s", dev, b->name);
	}

	for (i = 0; i < b->nport; i++) {
		const struct eth_port *p = &b->port[i];
		char master[IFNAMSIZ];
		int pidx = if_nametoindex(p->dev);

		if (!pidx) {
			DBG("eth: runtime: port %s of %s does not exist", p->dev,
			    b->name);
			continue;
		}
		dev_master(p->dev, master, sizeof(master));
		if (!strcmp(master, b->name)) {
			port_opts(next, b, p);
			continue;
		}
		if (master[0]) {
			INFO("eth: runtime: %s is a port of %s, not moved to "
			     "%s", p->dev, master, b->name);
			continue;
		}
		ret = link_master(pidx, idx);
		if (ret) {
			WARN("eth: runtime: cannot add %s to %s: %s", p->dev,
			     b->name, strerror(-ret));
			continue;
		}
		link_up(pidx);
		port_opts(next, b, p);
		INFO("eth: runtime: %s joined bridge %s", p->dev, b->name);
	}
}

void eth_rt_sync(const struct eth_model *cur, const struct eth_model *next)
{
	int pass, i, left;
	bool made[ETH_MAX_VLANS] = { false };

	collect(cur, next);

	for (i = 0; i < next->nbr; i++)
		make_bridge(&next->br[i]);

	/* a VLAN device may sit on another one or on a bridge: parents first */
	for (pass = 0, left = next->nvlan; left && pass <= next->nvlan; pass++) {
		for (i = 0; i < next->nvlan; i++) {
			if (made[i] || !make_vlan(&next->vlan[i]))
				continue;
			made[i] = true;
			left--;
		}
	}
	for (i = 0; i < next->nvlan; i++)
		if (!made[i])
			WARN("eth: runtime: %s: its parent %s does not exist",
			     next->vlan[i].name, next->vlan[i].parent);

	/* a bridge may take a VLAN device as a port: ports after the devices */
	for (i = 0; i < next->nbr; i++) {
		if (dev_exists(next->br[i].name) && dev_ours(next->br[i].name))
			bridge_opts(&next->br[i]);
		sync_ports(cur, next, &next->br[i]);
	}
}

/* own_config_only 0: netifd has the devices, none of ours is left over */
void eth_rt_flush(void)
{
	collect(NULL, NULL);
}
