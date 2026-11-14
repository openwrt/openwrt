// SPDX-License-Identifier: GPL-2.0
/*
 * Multicast gate: the multicast GEM port reaches every ONU on the optical
 * tree, so the OLT sends this ONU the IPTV groups that the other ONUs
 * joined.  With nobody listening here, all of it went through the QDMA,
 * NAPI and the stack only to be dropped (~25 Mbit/s, ~2400 frames/s seen
 * on Orange PL with no set-top box at all).  Gate it in the MAC instead:
 * GPON_CMD_MCAST_RX 0 takes the multicast GEM ports out of the hardware
 * GEM table (the software table, OMCI and the counters are untouched).
 *
 * "Somebody listens" means a group a stream can be sent to (IPv4 outside
 * 224.0.0.0/24 and SSDP's 239.255.255.250; IPv6 transient, flag T, with a
 * scope above link-local: ff1x::/ff3x:: - the permanent ff0x:: groups such
 * as ff05::2, which the kernel joins on every interface of a router, are
 * not) on a device stacked on pon0
 * (pon0 itself, its VLAN devices, the bridges they are ports of and
 * their VLAN devices, followed through /sys/class/net/<dev>/lower_*):
 *   - a bridge MDB entry of such a bridge (RTM_GETMDB dump): a set-top
 *     box behind a UNI port (IGMP snooping), or a permanent ME 309
 *     static group (gpon-mcast.init);
 *   - a socket membership on such a device (/proc/net/igmp, igmp6): the
 *     router side, e.g. omcproxy with its uplink on the IPTV VLAN.
 *
 * Polled every MCG_POLL_MS: the gate opens at once (a join that arrives
 * while it is closed costs at most one poll before the OLT's stream gets
 * in) and closes MCG_HOLD_S after the last group went away, so a channel
 * change (leave + join) does not flap it.  The state is sent again every
 * MCG_REFRESH_S, which also covers a reload of the driver (it starts with
 * the gate open).  onu.mcast_gate '0' keeps the multicast open for good.
 */
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <linux/if_bridge.h>
#include <linux/if_ether.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

#include "gpond.h"

#define MCG_POLL_MS	1000
#define MCG_HOLD_S	30
#define MCG_REFRESH_S	60
#define MCG_MAX_DEVS	32
#define MCG_DEPTH	4	/* pon0 -> VLAN -> bridge -> VLAN */

static struct uloop_timeout mcg_tmo;
static int mcg_sent = -1;		/* last state sent, -1: none yet */
static time_t mcg_sent_at;
static time_t mcg_last_seen;		/* a listener was there */
static time_t mcg_retry_at;		/* after a refused GPON_CMD_MCAST_RX */

struct mcg_devs {
	int	n;
	int	ifindex[MCG_MAX_DEVS];
	char	name[MCG_MAX_DEVS][IFNAMSIZ];
	bool	bridge[MCG_MAX_DEVS];
};

static time_t mcg_now(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return ts.tv_sec;
}

/* @dev is GPON_NETDEV or reaches it through its lower_* links */
static bool mcg_on_pon(const char *dev, int depth)
{
	char path[PATH_MAX];
	struct dirent *de;
	bool found = false;
	DIR *d;

	if (!strcmp(dev, GPON_NETDEV))
		return true;
	if (depth >= MCG_DEPTH)
		return false;

	snprintf(path, sizeof(path), "/sys/class/net/%s", dev);
	d = opendir(path);
	if (!d)
		return false;
	while (!found && (de = readdir(d)))
		if (!strncmp(de->d_name, "lower_", 6))
			found = mcg_on_pon(de->d_name + 6, depth + 1);
	closedir(d);

	return found;
}

static void mcg_collect(struct mcg_devs *s)
{
	char path[PATH_MAX];
	struct dirent *de;
	DIR *d;

	s->n = 0;
	d = opendir("/sys/class/net");
	if (!d)
		return;
	while ((de = readdir(d)) && s->n < MCG_MAX_DEVS) {
		int idx;

		if (de->d_name[0] == '.' || strlen(de->d_name) >= IFNAMSIZ)
			continue;
		if (!mcg_on_pon(de->d_name, 0))
			continue;
		idx = if_nametoindex(de->d_name);
		if (!idx)
			continue;
		snprintf(path, sizeof(path), "/sys/class/net/%s/bridge",
			 de->d_name);
		s->ifindex[s->n] = idx;
		strcpy(s->name[s->n], de->d_name);
		s->bridge[s->n] = !access(path, F_OK);
		s->n++;
	}
	closedir(d);
}

static bool mcg_has_dev(const struct mcg_devs *s, const char *name)
{
	int i;

	for (i = 0; i < s->n; i++)
		if (!strcmp(s->name[i], name))
			return true;

	return false;
}

static bool mcg_bridge_idx(const struct mcg_devs *s, int ifindex)
{
	int i;

	for (i = 0; i < s->n; i++)
		if (s->bridge[i] && s->ifindex[i] == ifindex)
			return true;

	return false;
}

static bool mcg_v4_wanted(uint32_t grp_be)
{
	uint32_t g = ntohl(grp_be);

	/* 224.0.0.0/24: link-local control (all hosts, IGMPv3 reports...) */
	if ((g & 0xffffff00) == 0xe0000000)
		return false;

	return g != 0xeffffffa;		/* 239.255.255.250, SSDP */
}

static bool mcg_v6_wanted(const uint8_t *a)
{
	/*
	 * RFC 4291: flag T (0x10) set is a transient group (ff3x:: SSM
	 * included), what a stream is sent to; T clear is a permanent,
	 * well-known one (all-nodes, all-routers, DHCP, MLDv2 reports...),
	 * joined by the stack itself.  Scope 1 and 2 never leave the link.
	 */
	return a[0] == 0xff && (a[1] & 0x10) && (a[1] & 0x0f) > 2;
}

/*
 * /proc/net/igmp: a device line "<idx>\t<name>   :  <count> <querier>"
 * followed by "\t\t\t\t<group> <users> <timer> <reporter>" lines; the
 * group is the raw __be32 printed with %08X.
 */
static bool mcg_igmp4(const struct mcg_devs *s)
{
	char line[256], dev[IFNAMSIZ + 1] = "";
	bool ours = false, found = false;
	FILE *f = fopen("/proc/net/igmp", "r");

	if (!f)
		return false;
	while (!found && fgets(line, sizeof(line), f)) {
		unsigned int grp;
		int idx;

		if (line[0] != '\t') {
			if (sscanf(line, "%d %16s", &idx, dev) == 2)
				ours = mcg_has_dev(s, dev);
			else
				ours = false;
			continue;
		}
		if (ours && sscanf(line, " %x", &grp) == 1 &&
		    mcg_v4_wanted(grp))
			found = true;
	}
	fclose(f);

	return found;
}

/* /proc/net/igmp6: "<idx> <dev> <32 hex digits> <users> <flags> <timer>" */
static bool mcg_igmp6(const struct mcg_devs *s)
{
	char line[256], dev[IFNAMSIZ + 1], hex[33];
	bool found = false;
	FILE *f = fopen("/proc/net/igmp6", "r");

	if (!f)
		return false;
	while (!found && fgets(line, sizeof(line), f)) {
		uint8_t a[16];
		int idx, i;

		if (sscanf(line, "%d %16s %32s", &idx, dev, hex) != 3 ||
		    strlen(hex) != 32 || !mcg_has_dev(s, dev))
			continue;
		for (i = 0; i < 16; i++) {
			unsigned int b;

			if (sscanf(hex + 2 * i, "%2x", &b) != 1)
				break;
			a[i] = b;
		}
		if (i == 16 && mcg_v6_wanted(a))
			found = true;
	}
	fclose(f);

	return found;
}

/* one MDBA_MDB_ENTRY_INFO of a bridge we look at */
static bool mcg_mdb_entry(const struct br_mdb_entry *e)
{
	if (e->addr.proto == htons(ETH_P_IP))
		return mcg_v4_wanted(e->addr.u.ip4);
	if (e->addr.proto == htons(ETH_P_IPV6))
		return mcg_v6_wanted(e->addr.u.ip6.s6_addr);

	return false;		/* L2 (MAC) entries */
}

static bool mcg_mdb_msg(const struct mcg_devs *s, struct nlmsghdr *nh)
{
	struct br_port_msg *bpm = NLMSG_DATA(nh);
	int len = nh->nlmsg_len - NLMSG_LENGTH(sizeof(*bpm));
	struct rtattr *a;

	if (len < 0 || !mcg_bridge_idx(s, bpm->ifindex))
		return false;

	for (a = (struct rtattr *)((char *)bpm + NLMSG_ALIGN(sizeof(*bpm)));
	     RTA_OK(a, len); a = RTA_NEXT(a, len)) {
		struct rtattr *ent;
		int elen;

		if (a->rta_type != MDBA_MDB)
			continue;
		elen = RTA_PAYLOAD(a);
		for (ent = RTA_DATA(a); RTA_OK(ent, elen);
		     ent = RTA_NEXT(ent, elen)) {
			struct rtattr *info;
			int ilen;

			if (ent->rta_type != MDBA_MDB_ENTRY)
				continue;
			ilen = RTA_PAYLOAD(ent);
			for (info = RTA_DATA(ent); RTA_OK(info, ilen);
			     info = RTA_NEXT(info, ilen))
				if (info->rta_type == MDBA_MDB_ENTRY_INFO &&
				    RTA_PAYLOAD(info) >=
				    sizeof(struct br_mdb_entry) &&
				    mcg_mdb_entry(RTA_DATA(info)))
					return true;
		}
	}

	return false;
}

/* RTM_GETMDB dump of every bridge; true: a wanted group on one of ours */
static bool mcg_mdb(const struct mcg_devs *s)
{
	struct {
		struct nlmsghdr nh;
		struct br_port_msg bpm;
	} req = {
		.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct br_port_msg)),
		.nh.nlmsg_type = RTM_GETMDB,
		.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP,
		.nh.nlmsg_seq = 1,
		.bpm.family = PF_BRIDGE,
	};
	struct sockaddr_nl sa = { .nl_family = AF_NETLINK };
	bool found = false, done = false;
	static char buf[16384];
	int fd, i;

	for (i = 0; i < s->n && !s->bridge[i]; i++)
		;
	if (i == s->n)
		return false;	/* no bridge on pon0: nothing to dump */

	fd = socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE);
	if (fd < 0)
		return false;
	if (sendto(fd, &req, req.nh.nlmsg_len, 0, (struct sockaddr *)&sa,
		   sizeof(sa)) < 0)
		goto out;

	while (!done) {
		ssize_t n = recv(fd, buf, sizeof(buf), 0);
		struct nlmsghdr *nh;

		if (n <= 0)
			break;
		for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, (size_t)n);
		     nh = NLMSG_NEXT(nh, n)) {
			if (nh->nlmsg_type == NLMSG_DONE ||
			    nh->nlmsg_type == NLMSG_ERROR) {
				done = true;
				break;
			}
			/* read the dump to its end even after a hit */
			if (nh->nlmsg_type == RTM_NEWMDB && !found)
				found = mcg_mdb_msg(s, nh);
		}
	}
out:
	close(fd);

	return found;
}

static void mcg_send(bool on, time_t now)
{
	int ret = kx_mcast_rx(on);

	if (ret) {
		/* a driver without the command, or not ready: not every poll */
		if (!mcg_retry_at)
			WARN("multicast gate: GPON_CMD_MCAST_RX refused (%d)",
			     ret);
		mcg_retry_at = now + MCG_REFRESH_S;
		return;
	}
	mcg_retry_at = 0;
	if (mcg_sent != on)
		INFO("multicast GEM %s", on ? "open: a group has a listener"
					    : "gated: no listener");
	mcg_sent = on;
	mcg_sent_at = now;
}

static void mcg_poll(struct uloop_timeout *t)
{
	time_t now = mcg_now();
	struct mcg_devs s;
	bool on;

	uloop_timeout_set(t, MCG_POLL_MS);

	if (!cfg.mcast_gate) {
		on = true;
	} else {
		mcg_collect(&s);
		if (mcg_mdb(&s) || mcg_igmp4(&s) || mcg_igmp6(&s))
			mcg_last_seen = now;
		on = mcg_last_seen && now - mcg_last_seen < MCG_HOLD_S;
	}

	if (mcg_retry_at && now < mcg_retry_at)
		return;
	if (mcg_sent != on || now - mcg_sent_at >= MCG_REFRESH_S)
		mcg_send(on, now);
}

void mcast_gate_init(void)
{
	mcg_tmo.cb = mcg_poll;
	mcg_sent = -1;
	uloop_timeout_set(&mcg_tmo, MCG_POLL_MS);
}
