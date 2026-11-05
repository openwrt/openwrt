// SPDX-License-Identifier: GPL-2.0
/*
 * Statistics of a netdev through ethtool generic netlink and the ethtool
 * ioctl.
 *
 * Source 1, ETHTOOL_MSG_STATS_GET (net/ethtool/stats.c of the kernel, the
 * request is stats_parse_request(), the reply stats_fill_reply()):
 *   request  header nest {DEV_NAME}, GROUPS bitset (NOMASK form, bits by
 *            name: "eth-mac", "rmon" = stats_std_names[]);
 *   reply    one ETHTOOL_A_STATS_GRP nest per group {GRP_ID, GRP_SS_ID,
 *            GRP_STAT nests, HIST_RX / HIST_TX nests}.  A GRP_STAT nest
 *            holds one u64 attribute whose type is the counter number
 *            (stat_put(): "start stats attr types from 0"); counters the
 *            driver did not set (ETHTOOL_STAT_NOT_SET) are omitted.  The
 *            RMON histogram is one HIST_RX / HIST_TX nest per size range
 *            {BKT_LOW, BKT_HI, VAL} (stats_put_rmon_hist()), the ranges
 *            are the driver's (mt7530: 0-64, 65-127, 128-255, 256-511,
 *            512-1023, 1024-MAX: mt7530.c mt7530_rmon_ranges[]).
 *   The eth-ctrl group (pause frames) is not requested: no PM attribute
 *   counts MAC control frames.
 *
 * Source 2, ETHTOOL_GSTATS ("ethtool -S"), only for what no standard group
 * has: the mt7530 driver strings RxDrop, RxCtrlDrop, RxIngressDrop,
 * TxCrcErr (mt7530.c mt7530_mib[]) and RxCrcErr (the FCS error counter,
 * which mt7530_get_eth_mac_stats() does not report in the eth-mac group).
 * Strings are looked up by name, so the DSA core's four leading
 * tx_packets / tx_bytes / rx_packets / rx_bytes entries
 * (net/dsa/user.c dsa_user_get_strings()) do not matter.
 *
 * The ethtool uapi numbers are written down here (include/uapi/linux/
 * ethtool_netlink{,_generated}.h of the 6.18 kernel the board runs) so
 * that the build does not depend on the version of the toolchain's kernel
 * headers.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>

#include <unl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>

#include "ethtool_stats.h"

#define ETHTOOL_GENL_NAME	"ethtool"

/* maps hold the ETS_* id + 1, 0: not a counter of ours */
static const uint8_t mac_map[__MAC_MAX] = {
	[MAC_TX_PKT] = ETS_TX_PKTS + 1,
	[MAC_SINGLE_COL] = ETS_SINGLE_COL + 1,
	[MAC_MULTI_COL] = ETS_MULTI_COL + 1,
	[MAC_RX_PKT] = ETS_RX_PKTS + 1,
	[MAC_FCS_ERR] = ETS_FCS + 1,
	[MAC_ALIGN_ERR] = ETS_ALIGN + 1,
	[MAC_TX_BYTES] = ETS_TX_BYTES + 1,
	[MAC_TX_DEFER] = ETS_TX_DEFER + 1,
	[MAC_LATE_COL] = ETS_LATE_COL + 1,
	[MAC_XS_COL] = ETS_XS_COL + 1,
	[MAC_RX_BYTES] = ETS_RX_BYTES + 1,
	[MAC_TX_MCAST] = ETS_TX_MCAST + 1,
	[MAC_TX_BCAST] = ETS_TX_BCAST + 1,
	[MAC_RX_MCAST] = ETS_RX_MCAST + 1,
	[MAC_RX_BCAST] = ETS_RX_BCAST + 1,
};

static const uint8_t rmon_map[__RMON_MAX] = {
	[RMON_UNDERSIZE] = ETS_UNDERSIZE + 1,
	[RMON_OVERSIZE] = ETS_OVERSIZE + 1,
	[RMON_FRAG] = ETS_FRAGMENTS + 1,
	[RMON_JABBER] = ETS_JABBERS + 1,
};

static void put(struct ets_stats *s, unsigned int id, uint64_t v)
{
	s->v[id] = v;
	s->have |= 1ull << id;
}

/*
 * The histogram bin of a size range: by its upper edge (the PM bins end at
 * 64, 127, 255, 511, 1023), the last one by its lower edge (1024 up to the
 * driver's maximum, the PM MEs count it as "1024-1518").  Ranges of other
 * shapes are not PM bins.
 */
static int hist_bin(uint32_t low, uint32_t hi)
{
	switch (hi) {
	case 64:
		return 0;
	case 127:
		return 1;
	case 255:
		return 2;
	case 511:
		return 3;
	case 1023:
		return 4;
	default:
		return low == 1024 ? 5 : -1;
	}
}

static bool a_u32(const struct nlattr *a, uint32_t *v)
{
	if (nla_len(a) != sizeof(*v))
		return false;
	*v = nla_get_u32(a);

	return true;
}

static void parse_hist(const struct nlattr *nest, bool tx, struct ets_stats *s)
{
	bool have_low = false, have_hi = false, have_val = false;
	uint32_t low = 0, hi = 0;
	uint64_t val = 0;
	struct nlattr *pos;
	int rem, bin;

	nla_for_each_nested(pos, nest, rem) {
		switch (nla_type(pos)) {
		case A_GRP_HIST_BKT_LOW:
			have_low = a_u32(pos, &low);
			break;
		case A_GRP_HIST_BKT_HI:
			have_hi = a_u32(pos, &hi);
			break;
		case A_GRP_HIST_VAL:
			if (nla_len(pos) == sizeof(uint64_t)) {
				val = nla_get_u64(pos);
				have_val = true;
			}
			break;
		default:
			break;		/* the 64 bit padding */
		}
	}
	if (!have_low || !have_hi || !have_val)
		return;
	bin = hist_bin(low, hi);
	if (bin >= 0)
		put(s, (tx ? ETS_TX_BIN0 : ETS_RX_BIN0) + bin, val);
}

static void parse_stat(const struct nlattr *nest, int grp, struct ets_stats *s)
{
	struct nlattr *pos;
	int rem;

	nla_for_each_nested(pos, nest, rem) {
		unsigned int t = nla_type(pos);
		unsigned int id = 0;

		if (nla_len(pos) != sizeof(uint64_t))
			continue;
		if (grp == GRP_ETH_MAC && t < __MAC_MAX)
			id = mac_map[t];
		else if (grp == GRP_RMON && t < __RMON_MAX)
			id = rmon_map[t];
		if (id)
			put(s, id - 1, nla_get_u64(pos));
	}
}

static void parse_grp(const struct nlattr *nest, struct ets_stats *s)
{
	int grp = -1, rem;
	struct nlattr *pos;

	nla_for_each_nested(pos, nest, rem) {
		uint32_t v;

		switch (nla_type(pos)) {
		case A_GRP_ID:
			grp = a_u32(pos, &v) ? (int)v : -1;
			break;
		case A_GRP_STAT:
			parse_stat(pos, grp, s);
			break;
		case A_GRP_HIST_RX:
		case A_GRP_HIST_TX:
			if (grp == GRP_RMON)
				parse_hist(pos, nla_type(pos) == A_GRP_HIST_TX,
					   s);
			break;
		default:
			break;
		}
	}
}

int ets_parse_reply(const void *attrs, int len, struct ets_stats *s)
{
	struct nlattr *pos;
	int rem;

	if (len < 0)
		return -EINVAL;
	nla_for_each_attr(pos, (struct nlattr *)attrs, len, rem)
		if (nla_type(pos) == A_STATS_GRP)
			parse_grp(pos, s);

	return 0;
}

/* ---- generic netlink ---- */

static struct unl nl;
static bool nl_up;
static time_t nl_failed;

#define NL_RETRY_S	30

static int nl_open(void)
{
	struct timespec t;

	if (nl_up)
		return 0;
	clock_gettime(CLOCK_MONOTONIC, &t);
	if (nl_failed && t.tv_sec - nl_failed < NL_RETRY_S)
		return -ENOENT;
	if (unl_genl_init(&nl, ETHTOOL_GENL_NAME)) {
		nl_failed = t.tv_sec ? t.tv_sec : 1;
		return -ENOENT;
	}
	nl_failed = 0;
	nl_up = true;

	return 0;
}

static int reply_cb(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gh = nlmsg_data(nlmsg_hdr(msg));

	ets_parse_reply(genlmsg_attrdata(gh, 0), genlmsg_attrlen(gh, 0), arg);

	return NL_SKIP;
}

static bool put_group(struct nl_msg *m, const char *name)
{
	struct nlattr *bit = nla_nest_start(m, A_BITSET_BITS_BIT);

	if (!bit || nla_put_string(m, A_BITSET_BIT_NAME, name))
		return false;
	nla_nest_end(m, bit);

	return true;
}

int ets_read_netlink(const char *dev, struct ets_stats *s)
{
	struct nlattr *hdr, *grps, *bits;
	struct nl_msg *m;
	int ret;

	if (!dev || !*dev || strlen(dev) >= IFNAMSIZ)
		return -EINVAL;
	ret = nl_open();
	if (ret)
		return ret;
	m = unl_genl_msg(&nl, MSG_STATS_GET, false);
	if (!m)
		return -ENOMEM;
	hdr = nla_nest_start(m, A_STATS_HEADER);
	if (!hdr || nla_put_string(m, A_HEADER_DEV_NAME, dev))
		goto nomem;
	nla_nest_end(m, hdr);
	grps = nla_nest_start(m, A_STATS_GROUPS);
	if (!grps || nla_put_flag(m, A_BITSET_NOMASK))
		goto nomem;
	bits = nla_nest_start(m, A_BITSET_BITS);
	if (!bits || !put_group(m, "eth-mac") || !put_group(m, "rmon"))
		goto nomem;
	nla_nest_end(m, bits);
	nla_nest_end(m, grps);

	/* unl_request() frees the message */
	return unl_request(&nl, m, reply_cb, s);
nomem:
	nlmsg_free(m);

	return -ENOMEM;
}

/* ---- ethtool ioctl ---- */

#define IOC_MAX_STATS	512	/* more strings than any switch driver has */

static const struct {
	const char *name;
	uint8_t id;
} ioc_names[] = {
	{ "RxDrop", ETS_RX_DROP },
	{ "RxCtrlDrop", ETS_RX_CTRL_DROP },
	{ "RxIngressDrop", ETS_RX_ING_DROP },
	{ "TxCrcErr", ETS_TX_CRC_ERR },
	{ "RxCrcErr", ETS_FCS },
};

void ets_parse_strings(const char *names, const uint64_t *vals, unsigned int n,
		       struct ets_stats *s)
{
	unsigned int i, j;

	for (i = 0; i < n; i++) {
		const char *nm = names + (size_t)i * ETH_GSTRING_LEN;

		for (j = 0; j < sizeof(ioc_names) / sizeof(ioc_names[0]); j++) {
			if (strncmp(nm, ioc_names[j].name, ETH_GSTRING_LEN))
				continue;
			/* RxCrcErr stands in for the FCS error counter only
			 * where the eth-mac group has none */
			if (ioc_names[j].id == ETS_FCS && ets_have(s, ETS_FCS))
				break;
			put(s, ioc_names[j].id, vals[i]);
			break;
		}
	}
}

static int ioc(int fd, struct ifreq *ifr, void *data)
{
	ifr->ifr_data = data;

	return ioctl(fd, SIOCETHTOOL, ifr) ? -errno : 0;
}

int ets_read_ioctl(const char *dev, struct ets_stats *s)
{
	struct ethtool_sset_info *info = NULL;
	struct ethtool_gstrings *gs = NULL;
	struct ethtool_stats *st = NULL;
	struct ifreq ifr;
	unsigned int n;
	int fd, ret;

	if (!dev || !*dev || strlen(dev) >= IFNAMSIZ)
		return -EINVAL;
	fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
	if (fd < 0)
		return -errno;
	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", dev);

	info = calloc(1, sizeof(*info) + sizeof(uint32_t));
	if (!info) {
		ret = -ENOMEM;
		goto out;
	}
	info->cmd = ETHTOOL_GSSET_INFO;
	info->sset_mask = 1ull << ETH_SS_STATS;
	ret = ioc(fd, &ifr, info);
	if (ret)
		goto out;
	/* sset_mask has no bit when the driver has no such set */
	n = (info->sset_mask & (1ull << ETH_SS_STATS)) ? info->data[0] : 0;
	if (!n || n > IOC_MAX_STATS) {
		ret = -EOPNOTSUPP;
		goto out;
	}

	gs = calloc(1, sizeof(*gs) + (size_t)n * ETH_GSTRING_LEN);
	/* the driver writes what it has, whatever n_stats says */
	st = calloc(1, sizeof(*st) + (size_t)IOC_MAX_STATS * sizeof(uint64_t));
	if (!gs || !st) {
		ret = -ENOMEM;
		goto out;
	}
	gs->cmd = ETHTOOL_GSTRINGS;
	gs->string_set = ETH_SS_STATS;
	gs->len = n;
	ret = ioc(fd, &ifr, gs);
	if (ret)
		goto out;
	st->cmd = ETHTOOL_GSTATS;
	st->n_stats = n;
	ret = ioc(fd, &ifr, st);
	if (ret)
		goto out;
	/* the set changed between the calls: names and values do not match */
	if (st->n_stats != n || gs->len != n) {
		ret = -EAGAIN;
		goto out;
	}
	ets_parse_strings((const char *)gs->data, (const uint64_t *)st->data, n,
			  s);
	ret = 0;
out:
	free(info);
	free(gs);
	free(st);
	close(fd);

	return ret;
}

int ets_read(const char *dev, struct ets_stats *s)
{
	int nlr, ior;

	memset(s, 0, sizeof(*s));
	nlr = ets_read_netlink(dev, s);
	if (nlr)		/* a failed request: drop what it left */
		memset(s, 0, sizeof(*s));
	ior = ets_read_ioctl(dev, s);
	if (ior && !s->have)
		return nlr ? nlr : ior;

	return 0;
}
