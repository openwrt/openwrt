/* SPDX-License-Identifier: GPL-2.0 */
/*
 * OMCI performance monitoring: PM history data MEs, 15 minute intervals,
 * Get current data, threshold crossing alerts.
 */
#ifndef _GPOND_OMCI_PM_H
#define _GPOND_OMCI_PM_H

#include "omci.h"
#include "ethtool_stats.h"

#include <gpon_genl.h>

/* the interval length (SDK pmmgr_define.h DEFAULT_CYCLE_TIME, 900 s) */
#define OMCI_PM_INTERVAL_MS	900000
/* how often the counters are read; the kernel / PHY counters that can
 * wrap (32 bit FEC counters, at least ~220 s at the worst case) are read
 * far more often than that */
#define OMCI_PM_POLL_MS		5000

/*
 * Counter slots of the Ethernet PM MEs (what the omci_pm.c tables name).
 * omci_pm.c resolves each one from ethtool netlink / ioctl, or from
 * /sys/class/net/<dev>/statistics/<name> (Documentation/ABI/testing/
 * sysfs-class-net-statistics) when the device does not answer; a slot no
 * source has reads 0.
 */
enum {
	PM_NS_RX_PACKETS,
	PM_NS_TX_PACKETS,
	PM_NS_RX_BYTES,
	PM_NS_TX_BYTES,
	PM_NS_RX_DROPPED,	/* the switch's receive drop counter */
	PM_NS_MULTICAST,	/* received */
	PM_NS_RX_CRC,		/* FCS errors */
	PM_NS_RX_FRAME,		/* alignment errors */
	PM_NS_RX_OVER,		/* frames too long (oversize) */
	PM_NS_RX_BCAST,
	PM_NS_TX_MCAST,
	PM_NS_TX_BCAST,
	PM_NS_RX_UNDER,
	PM_NS_RX_FRAG,
	PM_NS_RX_JABBER,
	PM_NS_TX_CRC,
	PM_NS_COL_SINGLE,
	PM_NS_COL_MULTI,
	PM_NS_COL_LATE,
	PM_NS_COL_EXCESS,
	PM_NS_DEFER,
	PM_NS_RX_LACKDROP,	/* SDK "drop events": drop + control + ingress */
	PM_NS_RX_DISCARD,	/* SDK "received and discarded" */
	PM_NS_RX_BIN0,		/* 64, 65-127, ... 1024-1518 octets received */
	PM_NS_TX_BIN0 = PM_NS_RX_BIN0 + 6,
	__PM_NS_MAX = PM_NS_TX_BIN0 + 6,
};

/* threshold values 1..PM_MAX_THR, 0: no TCA */
#define PM_MAX_THR		14

/* the kind of a PM ME, i.e. where its counters come from */
enum {
	PM_NONE,
	PM_FEC,
	PM_GEM,
	PM_NET,
};

/* source slots of PM_FEC (GPON_CNT_FEC_*) */
enum { S_FEC_BYTES, S_FEC_CW, S_FEC_UNCORR, S_FEC_TOTAL, S_FEC_SEC };
/* source slots of PM_GEM: the order of the genl GEM counters */
enum { S_GEM_RXF, S_GEM_RXB, S_GEM_TXF, S_GEM_TXB };

struct pm_ctr {
	uint8_t attr;		/* ME attribute index */
	uint8_t slot;		/* source slot, NOSRC: stays 0 */
	uint8_t thr;		/* threshold value 1..14, 0: no TCA */
	uint8_t alarm;		/* TCA alarm number */
	uint16_t div;		/* divisor of the accumulated source */
};

struct pm_me {
	uint16_t class_id;
	uint8_t kind;
	bool ext;		/* extended PM: control block, history adds up */
	uint8_t nctr, nctr_down;
	const struct pm_ctr *ctr;
	const struct pm_ctr *ctr_down;	/* extended PM: downstream */
	uint8_t nalarm;
	uint8_t alarm[PM_MAX_THR];
};

/* the counters of one PM ME instance */
struct pm_inst {
	uint64_t cur[OMCI_MAX_ATTR + 1];	/* running interval */
	uint64_t hist[OMCI_MAX_ATTR + 1];	/* interval ended last */
	uint64_t last[OMCI_MAX_ATTR + 1];	/* last source reading */
	bool base[OMCI_MAX_ATTR + 1];
	uint16_t tca;				/* raised: bit threshold - 1 */
	bool down;				/* extended PM: downstream */
	bool stopped;				/* extended PM: accumulation off */
	char key[24];				/* source, a change re-bases */
};

struct omci_pm_state {
	uint64_t start;		/* start of the running interval, ms */
	uint8_t counter;	/* interval end time */
	bool synced;
	uint32_t polls, intervals;
};

/* one counter slot of a device: ethtool id + sysfs fallback */
struct ns_src {
	uint8_t ets;		/* ETS_* + 1, 0: none */
	const char *sys;	/* statistics file, NULL: none */
};

/* the readings of one device in the running tick */
struct ns_snap {
	char dev[24];
	unsigned int tick;
	bool valid;
	int ets_err;			/* ets_read() result */
	struct ets_stats ets;
	bool sys_done[__PM_NS_MAX];
	int sys_err[__PM_NS_MAX];
	uint64_t sys[__PM_NS_MAX];
};

/*
 * Where the counters come from.  The defaults read the kernel (generic
 * netlink) and sysfs; the host tests replace them with a fake clock and
 * fake counters.
 */
struct omci_pm_ops {
	uint64_t (*now_ms)(void);
	/* GPON_CNT_* of the global counters */
	int (*global)(uint64_t cnt[__GPON_CNT_MAX]);
	/* per GEM port: rx frames, rx bytes, tx frames, tx bytes */
	int (*gem)(uint16_t gem, uint64_t cnt[4]);
	/* one PM_NS_* counter of a netdev (0 when no source has it) */
	int (*netstat)(const char *dev, unsigned int stat, uint64_t *v);
	/* ethtool statistics of a netdev (ethtool_stats.c ets_read()) */
	int (*ethstats)(const char *dev, struct ets_stats *s);
};
extern struct omci_pm_ops omci_pm_ops;

void omci_pm_init(void);
bool omci_pm_is_pm(const struct omci_me_def *d);
void omci_pm_inst_create(struct omci_inst *in);
void omci_pm_inst_free(struct omci_inst *in);
/* attribute values of the history (Get) or running (Get current data) bank */
void omci_pm_refresh(struct omci_inst *in, bool current);
/* Set / Create value check: the threshold data the ME points to exists */
bool omci_pm_check(struct omci_inst *in, uint8_t idx, const uint8_t *v);
/* attributes of @in were set by the OLT */
void omci_pm_set(struct omci_inst *in, uint16_t mask);
void omci_pm_sync_time(void);
void omci_pm_mib_reset(void);
/* read the counters, end the interval when it is due (uloop timer) */
void omci_pm_tick(void);
/* alarm numbers (TCA) of a PM ME */
unsigned int omci_pm_alarms(const struct omci_me_def *d, const uint8_t **l);
/* the OLT unlocked @parent: TCAs held back while it was locked go out */
void omci_pm_parent_unlocked(struct omci_inst *parent);
void omci_pm_dump(struct blob_buf *b);
void omci_pm_inst_dump(struct blob_buf *b, struct omci_inst *in);

/* nl.c: counters over generic netlink */
int kx_counters_global(uint64_t cnt[__GPON_CNT_MAX]);
int kx_counters_gem(uint16_t gem, uint64_t cnt[4]);

#endif /* _GPOND_OMCI_PM_H */
