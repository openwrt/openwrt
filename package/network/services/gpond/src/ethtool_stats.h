/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Statistics of a netdev through ethtool generic netlink
 * (ETHTOOL_MSG_STATS_GET: eth-mac and rmon groups) plus the driver specific
 * counters of the ETHTOOL_GSTATS ioctl ("ethtool -S").
 */
#ifndef _GPOND_ETHTOOL_STATS_H
#define _GPOND_ETHTOOL_STATS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* the RMON histogram of the PM MEs: 64, 65-127, 128-255, 256-511, 512-1023,
 * 1024-1518 octets */
#define ETS_BINS	6

enum ets_id {
	/* eth-mac group (ETHTOOL_A_STATS_ETH_MAC_*) */
	ETS_TX_PKTS,
	ETS_RX_PKTS,
	ETS_TX_BYTES,
	ETS_RX_BYTES,
	ETS_SINGLE_COL,
	ETS_MULTI_COL,
	ETS_LATE_COL,
	ETS_XS_COL,
	ETS_TX_DEFER,
	ETS_FCS,		/* also filled from the driver's RxCrcErr */
	ETS_ALIGN,
	ETS_TX_MCAST,
	ETS_TX_BCAST,
	ETS_RX_MCAST,
	ETS_RX_BCAST,
	/* rmon group (ETHTOOL_A_STATS_RMON_*) */
	ETS_UNDERSIZE,
	ETS_OVERSIZE,
	ETS_FRAGMENTS,
	ETS_JABBERS,
	ETS_RX_BIN0,		/* ... ETS_RX_BIN0 + ETS_BINS - 1 */
	ETS_TX_BIN0 = ETS_RX_BIN0 + ETS_BINS,
	/* driver specific (ethtool -S) strings, no standard group has them */
	ETS_RX_DROP = ETS_TX_BIN0 + ETS_BINS,	/* "RxDrop" */
	ETS_RX_CTRL_DROP,			/* "RxCtrlDrop" */
	ETS_RX_ING_DROP,			/* "RxIngressDrop" */
	ETS_TX_CRC_ERR,				/* "TxCrcErr" */
	ETS__MAX,
};

struct ets_stats {
	uint64_t v[ETS__MAX];
	uint64_t have;		/* bit (1ull << id): v[id] was reported */
};

/* ---- the attribute numbers of the ethtool netlink ABI ---- */
/* the kernel does not export these numbers in a uapi header, so they are
 * mirrored here (ethtool_netlink.h of the kernel tree) */
enum {
	MSG_STATS_GET = 32,		/* ETHTOOL_MSG_STATS_GET */
};

enum {					/* ETHTOOL_A_HEADER_* */
	A_HEADER_DEV_NAME = 2,
};

enum {					/* ETHTOOL_A_BITSET_* */
	A_BITSET_NOMASK = 1,
	A_BITSET_BITS = 3,
};

enum {					/* ETHTOOL_A_BITSET_BITS_*, _BIT_* */
	A_BITSET_BITS_BIT = 1,
	A_BITSET_BIT_NAME = 2,
};

enum {					/* ETHTOOL_A_STATS_* */
	A_STATS_HEADER = 2,
	A_STATS_GROUPS = 3,
	A_STATS_GRP = 4,
};

enum {					/* ETHTOOL_A_STATS_GRP_* */
	A_GRP_ID = 2,
	A_GRP_STAT = 4,
	A_GRP_HIST_RX = 5,
	A_GRP_HIST_TX = 6,
	A_GRP_HIST_BKT_LOW = 7,
	A_GRP_HIST_BKT_HI = 8,
	A_GRP_HIST_VAL = 9,
};

enum {					/* ETHTOOL_STATS_* group numbers */
	GRP_ETH_MAC = 1,
	GRP_RMON = 3,
};

enum {					/* ETHTOOL_A_STATS_ETH_MAC_* */
	MAC_TX_PKT = 0,
	MAC_SINGLE_COL = 1,
	MAC_MULTI_COL = 2,
	MAC_RX_PKT = 3,
	MAC_FCS_ERR = 4,
	MAC_ALIGN_ERR = 5,
	MAC_TX_BYTES = 6,
	MAC_TX_DEFER = 7,
	MAC_LATE_COL = 8,
	MAC_XS_COL = 9,
	MAC_RX_BYTES = 12,
	MAC_TX_MCAST = 14,
	MAC_TX_BCAST = 15,
	MAC_RX_MCAST = 17,
	MAC_RX_BCAST = 18,
	__MAC_MAX = 32,
};

enum {					/* ETHTOOL_A_STATS_RMON_* */
	RMON_UNDERSIZE = 0,
	RMON_OVERSIZE = 1,
	RMON_FRAG = 2,
	RMON_JABBER = 3,
	__RMON_MAX = 8,
};

static inline bool ets_have(const struct ets_stats *s, unsigned int id)
{
	return s->have & (1ull << id);
}

/*
 * Read the statistics of @dev: STATS_GET first, the ioctl strings after it.
 * 0 when at least one source answered (ets_have() tells what is in it),
 * -errno when none did (no ethtool family, not supported by the device,
 * no such device).
 */
int ets_read(const char *dev, struct ets_stats *s);

/* the two halves of ets_read(), and the parsers (host tests feed them) */
int ets_read_netlink(const char *dev, struct ets_stats *s);
int ets_read_ioctl(const char *dev, struct ets_stats *s);
/* attributes of an ETHTOOL_MSG_STATS_GET_REPLY, 0 or -EINVAL */
int ets_parse_reply(const void *attrs, int len, struct ets_stats *s);
/* @n names (32 bytes each) and values of ETH_SS_STATS */
void ets_parse_strings(const char *names, const uint64_t *vals, unsigned int n,
		       struct ets_stats *s);

#endif /* _GPOND_ETHTOOL_STATS_H */
