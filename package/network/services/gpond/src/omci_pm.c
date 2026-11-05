// SPDX-License-Identifier: GPL-2.0
/*
 * OMCI performance monitoring.
 *
 * The PM history data MEs the SDK implements (SDK = gpon-en757x/xpon,
 * src/pmmgr/pmmgr.c:pmmgrPmMeTable 61-88, PM ME flag IS_CLASSICAL_PM_ME /
 * IS_EXTENDED_PM_ME in src/omci/omci_me.c) are kept here: the OLT creates
 * them (omci_me.c access OMCI_ME_ACCESS_OLT; the SDK has no
 * omciInternalCreateFor* for any PM ME, so the ONU creates none).  Each
 * instance has a running bank (Get current data) and the bank of the
 * interval that ended last (Get).
 *
 * Intervals (pmmgr.c:gponPMCurrentTmout() 1026-1380, updataHistoryData()
 * 976-1020, pmmgr_api.c:pmmgrSynchronizeTime() 71-98):
 *   - one 900 s interval for every instance, from the start of the ONU
 *     (not from the instance's creation); an instance made in the middle
 *     counts from its creation;
 *   - at its end the running bank becomes the history bank and the running
 *     one starts from 0; the extended PMs (334, 425) add the running bank
 *     to the history instead (updataHistoryData(), isEthernetFrame-
 *     ExtendedPMMe());
 *   - Synchronize time restarts the interval at once (times = 0, the
 *     start values are cleared) and the omci core re-arms its 900 s timer
 *     (omci_general_func.o omciSyncTimeAction() 4a0c-4a3c: RemoveTimer,
 *     AddTimer(0xdbba0 ms, periodic));
 *   - the "interval end time" attribute is a counter of the intervals
 *     ended since the last synchronisation, 8 bit, wrapping (stock
 *     libapi_omci_adpt.so omci_lib_adpt_update_pm_history_data(): the
 *     counter is cleared by Synchronize time, incremented at every end).
 *     The history of a MIB-reset ONU starts at 0 as well.
 *
 * Threshold crossing alerts (pmmgr.c:gponPMCurrentTmout(), :updataHistory-
 * Data()): every counter that has a threshold value (threshold data 1 / 2
 * attribute, 0 = off) raises its alarm once per interval when the running
 * counter reaches the threshold (current >= threshold); the alarms are
 * cleared when the interval ends.  Both are ordinary alarms (alarm number
 * bit of the PM instance, Alarm message MT 16, Get all alarms) through
 * omci_alarm.c.  Differences of the stock adapter, not followed here: it
 * compares "current > threshold" and treats a 0 threshold as a valid one
 * (omci_pm_report_alarm_by_tca_info 0x38608), and it only ever clears the
 * flags at the end of an interval without sending the clear.
 *
 * Counters: 64 bit accumulations of the deltas of the kernel counters
 * between polls, so a 32 bit hardware counter wraps unnoticed and a
 * counter reset (CLEAR_COUNTERS, GEM port re-created) only costs the
 * counts since the last poll.  Counters of an ME are saturated at the
 * size of the attribute (the SDK stores them in 32 bit and wraps).
 *
 * Sources: PHY FEC counters (global genl counters) for ME 312, the per-GEM
 * MIB for 341 and 267, /sys/class/net/<uni>/statistics of the device behind
 * a PPTP Ethernet UNI for the Ethernet MEs (the SDK reads the MT7530 MIB:
 * pmmgr_pm.c getAllDataFromApiForPptpEthernetUni*PM(), ...EthernetFrame-
 * UpPM() 2158-2450); what neither has stays 0, as in the SDK where IP
 * host / TCP-UDP / GAL / MAC bridge / energy PM are 0 or placeholders
 * (pmmgr_pm.c 1814-1860, 1870-1890, 2841-2880).
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "gpond.h"
#include "omci.h"
#include "omci_pm.h"
#include "eth.h"
#include "ethtool_stats.h"

#define NOSRC			0xff
#define GEM_BLOCK_BYTES		48	/* ANI-G GEM block length (omci_mib.c) */
#define TCA_DISABLE_ALL		0x4000	/* stock omci_me_eth_frame_extend_pm_
					 * history_notify: tca disable bit 14 */

#define C(_a, _s, _t, _al, _d) { _a, _s, _t, _al, _d }
#define N(_a, _t, _al) C(_a, NOSRC, _t, _al, 1)
#define S(_a, _s) C(_a, _s, 0, 0, 1)
#define SA(_a, _s, _t, _al) C(_a, _s, _t, _al, 1)

/*
 * threshold value n of an ME is attribute n + 2 of the counters for the
 * Ethernet PM history data (ethernet_services_func.c setEthernetPMHistory-
 * DataThresholdData() 1253-1316: values 1-5 -> 1-5, 7 -> 6 ...), the
 * alarm number n - 1 (stock omci_me_eth_pm_history_notify 0x46a70: alarms
 * 0..13 in threshold order).
 */
/*
 * Counter sources, ME 24: SDK pmmgr_pm.c getAllDataFromApiForPptpEthernet-
 * UniPM() 2450-2515 -> vendor ethcmd/mtkswitch_api.c (MIB ids of
 * eth_global_def.h:366-433, MT7530_MIBCNT_VER 2, offsets = the mainline
 * driver's MT7530_PORT_MIB_*) -> Linux:
 *   FCS            RX_FCS_ERR (api.c:1147)       eth-mac FCS / ethtool -S RxCrcErr
 *   excessive col  TX_XCOL (api.c:1152)          eth-mac FramesAbortedDueToXSColls
 *   late col       TX_LCOL (api.c:1157)          eth-mac LateCollisions
 *   too long       RX_OVERSIZE (api.c:1162)      rmon etherStatsOversizePkts
 *   buffer ovf rx  RX_DROP (api.c:1167)          ethtool -S RxDrop / stats64 rx_dropped
 *   single col     TX_SCOL (api.c:1179)          eth-mac SingleCollisionFrames
 *   multiple col   TX_MCOL (api.c:1184)          eth-mac MultipleCollisionFrames
 *   deferred       TX_DEFER (api.c:1194)         eth-mac FramesWithDeferredXmissions
 *   alignment      RX_ALIGN_ERR (api.c:1211)     eth-mac AlignmentErrors
 * No source (0): buffer overflows on transmit (the SDK struct has no field
 * for it, pmmgr_struct.h:101-113), SQE (api.c:1189 "not supported,
 * obsolete"), internal MAC transmit / receive error and carrier sense
 * (api.c:1199-1210, 1216-1220: constant 0, and the SDK does not read them;
 * the mt7530 MIB has no such counter).
 */
static const struct pm_ctr ctr_eth[] = {
	SA(3, PM_NS_RX_CRC, 1, 0),		/* FCS errors */
	SA(4, PM_NS_COL_EXCESS, 2, 1),		/* excessive collision */
	SA(5, PM_NS_COL_LATE, 3, 2),		/* late collision */
	SA(6, PM_NS_RX_OVER, 4, 3),		/* frames too long */
	SA(7, PM_NS_RX_DROPPED, 5, 4),		/* buffer overflows on receive */
	N(8, 6, 5),				/* ... on transmit */
	SA(9, PM_NS_COL_SINGLE, 7, 6),		/* single collision */
	SA(10, PM_NS_COL_MULTI, 8, 7),		/* multiple collisions */
	N(11, 9, 8),				/* SQE */
	SA(12, PM_NS_DEFER, 10, 9),		/* deferred transmission */
	N(13, 11, 10),				/* internal MAC transmit error */
	N(14, 12, 11),				/* carrier sense error */
	SA(15, PM_NS_RX_FRAME, 13, 12),		/* alignment error */
	N(16, 14, 13),				/* internal MAC receive error */
};

static const struct pm_ctr ctr_eth2[] = {
	N(3, 1, 0),				/* PPPoE filtered frames */
};

/* thresholds: drop events 1, undersize 2, fragments 3, jabbers 4
 * (pmmgr_pm.c 504-520), alarms 0..3 (stock omci_me_eth3_pm_history_notify) */
/* every counter is the receive side (SDK getAllDataFromApiForPptpEthernet-
 * Uni3PM() 2531-2600): drop events = RX_DROP + RX_CTRL_DROP + RX_ING_DROP
 * (api.c:612-622), octets RX_OCT, packets = RX ucast + mcast + bcast,
 * undersize RX_UNDERSIZE, fragments RX_FRAG_ERR, jabbers RX_JABB_ERR, the
 * size bins RX_PKT_* (api.c:726-847) */
#define BINS(_s) \
	S(11, _s), S(12, (_s) + 1), S(13, (_s) + 2), \
	S(14, (_s) + 3), S(15, (_s) + 4), S(16, (_s) + 5)

static const struct pm_ctr ctr_eth3[] = {
	SA(3, PM_NS_RX_LACKDROP, 1, 0),
	S(4, PM_NS_RX_BYTES),
	S(5, PM_NS_RX_PACKETS),
	S(6, PM_NS_RX_BCAST),
	S(7, PM_NS_MULTICAST),
	SA(8, PM_NS_RX_UNDER, 2, 1),
	SA(9, PM_NS_RX_FRAG, 3, 2),
	SA(10, PM_NS_RX_JABBER, 4, 3),
	BINS(PM_NS_RX_BIN0),
};

/*
 * Ethernet frame PM upstream (322) / downstream (321): drop events, CRC
 * errored, undersize, oversize are thresholds 1-4 (pmmgr_pm.c 2130-2180,
 * 2270-2290), alarms 0..3 (stock omci_me_eth_frame_up_pm_history_notify
 * 0x62628).  The downstream SDK read has no drop / undersize / oversize
 * (pmmgr_pm.c:2338-2356).  Extended PM (334; 425 has no threshold ME in
 * the SDK table): the same four, alarms 1..4 (stock omci_me_eth_frame_
 * extend_pm_history_notify 0x4dd24).
 */
static const struct pm_ctr ctr_frame_up[] = {
	SA(3, PM_NS_RX_LACKDROP, 1, 0),
	S(4, PM_NS_RX_BYTES),
	S(5, PM_NS_RX_PACKETS),
	S(6, PM_NS_RX_BCAST),
	S(7, PM_NS_MULTICAST),
	SA(8, PM_NS_RX_CRC, 2, 1),
	SA(9, PM_NS_RX_UNDER, 3, 2),
	SA(10, PM_NS_RX_OVER, 4, 3),
	BINS(PM_NS_RX_BIN0),
};

/* the transmit side: TX_CRC_CNT exists only in the driver's ethtool -S
 * strings (TxCrcErr); drop, undersize, oversize are 0 in the SDK
 * (pmmgr_pm.c:2367, 2384-2385 / api.c:709, 720: "always return 0") */
static const struct pm_ctr ctr_frame_down[] = {
	N(3, 1, 0),
	S(4, PM_NS_TX_BYTES),
	S(5, PM_NS_TX_PACKETS),
	S(6, PM_NS_TX_BCAST),
	S(7, PM_NS_TX_MCAST),
	SA(8, PM_NS_TX_CRC, 2, 1),
	N(9, 3, 2),
	N(10, 4, 3),
	BINS(PM_NS_TX_BIN0),
};

/* ME 334: the SDK reads the receive side whatever the direction
 * (pmmgr_pm.c:2624-2668 has no downstream variant); here the downstream
 * instance reads the transmit side like ME 425 does (:2748-2795) */
static const struct pm_ctr ctr_ext_up[] = {
	SA(3, PM_NS_RX_LACKDROP, 1, 1),
	S(4, PM_NS_RX_BYTES),
	S(5, PM_NS_RX_PACKETS),
	S(6, PM_NS_RX_BCAST),
	S(7, PM_NS_MULTICAST),
	SA(8, PM_NS_RX_CRC, 2, 2),
	SA(9, PM_NS_RX_UNDER, 3, 3),
	SA(10, PM_NS_RX_OVER, 4, 4),
	BINS(PM_NS_RX_BIN0),
};

static const struct pm_ctr ctr_ext_down[] = {
	N(3, 1, 1),
	S(4, PM_NS_TX_BYTES),
	S(5, PM_NS_TX_PACKETS),
	S(6, PM_NS_TX_BCAST),
	S(7, PM_NS_TX_MCAST),
	SA(8, PM_NS_TX_CRC, 2, 2),
	N(9, 3, 3),
	N(10, 4, 4),
	BINS(PM_NS_TX_BIN0),
};

static const struct pm_ctr ctr_ext64_up[] = {
	S(3, PM_NS_RX_LACKDROP),
	S(4, PM_NS_RX_BYTES),
	S(5, PM_NS_RX_PACKETS),
	S(6, PM_NS_RX_BCAST),
	S(7, PM_NS_MULTICAST),
	S(8, PM_NS_RX_CRC),
	S(9, PM_NS_RX_UNDER),
	S(10, PM_NS_RX_OVER),
	BINS(PM_NS_RX_BIN0),
};

static const struct pm_ctr ctr_ext64_down[] = {
	S(4, PM_NS_TX_BYTES),
	S(5, PM_NS_TX_PACKETS),
	S(6, PM_NS_TX_BCAST),
	S(7, PM_NS_TX_MCAST),
	S(8, PM_NS_TX_CRC),
	BINS(PM_NS_TX_BIN0),
};

/* MAC bridge port PM: thresholds delay exceeded 1, MTU exceeded 2, received
 * and discarded 3 (pmmgr_pm.c 1905-1925, ethernet_services...), alarms
 * 1, 2, 4 (stock omci_me_mac_br_port_pm_history_notify 0x5ed84).
 * Sources (pmmgr_pm.c getAllDataFromApiForMACBridgePortPM() 1951-2030):
 * forwarded = TxSuccFrames (tx ucast + mcast + bcast, api.c:545-556),
 * received = RxPkts, received and discarded = RxDropFrames (alignment +
 * FCS + undersize + oversize + fragments + jabbers + RX_DROP, api.c:
 * 583-596); delay / MTU exceeded are 0 in the SDK (:2005-2006). */
static const struct pm_ctr ctr_bport[] = {
	S(3, PM_NS_TX_PACKETS),
	N(4, 1, 1),
	N(5, 2, 2),
	S(6, PM_NS_RX_PACKETS),
	SA(7, PM_NS_RX_DISCARD, 3, 4),
};

/* FEC PM: corrected bytes 1, corrected code words 2, uncorrectable code
 * words 3, FEC seconds 4 (pmmgr_define.h:86-89); alarms 0, 1, 2, 4 (stock
 * omci_me_fec_pm_history_notify 0x6d2a8) */
static const struct pm_ctr ctr_fec[] = {
	SA(3, S_FEC_BYTES, 1, 0),
	SA(4, S_FEC_CW, 2, 1),
	SA(5, S_FEC_UNCORR, 3, 2),
	S(6, S_FEC_TOTAL),
	SA(7, S_FEC_SEC, 4, 4),
};

/* GEM port network CTP PM: the SDK has no threshold (ani_management_func.c
 * setGemPortCtpPmThrdDataValue() 7366-7399 is disabled); threshold 1 is the
 * encryption key error counter (stock adapter mapping, attr 7, alarm 0) */
static const struct pm_ctr ctr_gem_ctp[] = {
	S(3, S_GEM_TXF),
	S(4, S_GEM_RXF),
	S(5, S_GEM_RXB),
	S(6, S_GEM_TXB),
	N(7, 1, 0),
};

/* MAC bridge PM (51): the SDK reports no entry discard count (pmmgr_pm.c
 * 1836-1840 has NO_TCA and the API returns 0) */
static const struct pm_ctr ctr_bridge[] = {
	N(3, 1, 0),
};

/* GEM port PM (267): received packets = GEM frames, blocks = payload / 48
 * (pmmgr_pm.c getAllDataFromApiForGemPortPM() 1324-1365); lost, misinserted
 * and impaired are 0 there, thresholds 1-3, alarms 0-2 (stock
 * omci_me_gem_port_pm_history_notify 0x40cfc) */
static const struct pm_ctr ctr_gem_port[] = {
	N(3, 1, 0),
	N(4, 2, 1),
	S(5, S_GEM_RXF),
	C(6, S_GEM_RXB, 0, 0, GEM_BLOCK_BYTES),
	C(7, S_GEM_TXB, 0, 0, GEM_BLOCK_BYTES),
	N(8, 3, 2),
};

/* GAL ethernet PM (276): discarded frames, no source (pmmgr_pm.c 1703-1707) */
static const struct pm_ctr ctr_gal[] = {
	N(3, 1, 0),
};

/* IP host PM (135): icmp errors, dns errors, dhcp timeouts, address
 * conflicts, out of memory, internal errors (pmmgr_pm.c 1127-1139); the
 * SDK has no source for any of them */
static const struct pm_ctr ctr_iphost[] = {
	N(3, 1, 1),
	N(4, 2, 2),
	N(5, 3, 3),
	N(6, 4, 4),
	N(7, 5, 5),
	N(8, 6, 6),
};

/* TCP/UDP PM (342) and energy consumption PM (343): no source and no TCA in
 * the SDK (pmmgr_pm.c 1192-1204, 351-356) */
static const struct pm_ctr ctr_tcpudp[] = {
	N(3, 0, 0),
	N(4, 0, 0),
	N(5, 0, 0),
	N(6, 0, 0),
	N(7, 0, 0),
};

static const struct pm_ctr ctr_energy[] = {
	N(3, 0, 0),
	N(4, 0, 0),
	N(5, 0, 0),
};

#define ARRSZ(_a)	(sizeof(_a) / sizeof((_a)[0]))
#define ME(_c, _k, _t, _t2) \
	{ _c, _k, true, ARRSZ(_t), ARRSZ(_t2), _t, _t2 }
#define ME1(_c, _k, _t) { _c, _k, false, ARRSZ(_t), 0, _t, NULL }

static struct pm_me pm_me[] = {
	ME1(24, PM_NET, ctr_eth),
	ME1(89, PM_NET, ctr_eth2),
	ME1(296, PM_NET, ctr_eth3),
	ME1(322, PM_NET, ctr_frame_up),
	ME1(321, PM_NET, ctr_frame_down),
	ME(334, PM_NET, ctr_ext_up, ctr_ext_down),
	ME(425, PM_NET, ctr_ext64_up, ctr_ext64_down),
	ME1(52, PM_NET, ctr_bport),
	ME1(51, PM_NONE, ctr_bridge),
	ME1(135, PM_NONE, ctr_iphost),
	ME1(267, PM_GEM, ctr_gem_port),
	ME1(276, PM_NONE, ctr_gal),
	ME1(312, PM_FEC, ctr_fec),
	ME1(341, PM_GEM, ctr_gem_ctp),
	ME1(342, PM_NONE, ctr_tcpudp),
	ME1(343, PM_NONE, ctr_energy),
};

#define PM_NME	(sizeof(pm_me) / sizeof(pm_me[0]))

/* ---- state ---- */

static struct omci_pm_state st;
static struct uloop_timeout tick_tmo;
static unsigned int tick_no;

/* ---- default sources ---- */

static uint64_t def_now(void)
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);

	return (uint64_t)t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

/*
 * Where each counter slot comes from.  The ethtool id is the primary source
 * (one STATS_GET + one ETHTOOL_GSTATS per device and tick), the sysfs file
 * (the rtnl stats64 of the device) the fallback for a device that does not
 * answer ethtool or does not report that counter.  A slot with neither
 * stays 0.  The mt7530 driver (the board's switch) fills, in 6.18.52 (mt7530.c
 * mt7530_get_stats64() ~891), only rx/tx packets and bytes, multicast,
 * rx_dropped (RX_DROP only), tx_dropped and rx_crc_errors: every other
 * stats64 field (rx_frame_errors, rx_over_errors, collisions ...) is never
 * set there, so sysfs offers nothing beyond those.
 */
static struct ns_src ns_src_of(unsigned int slot)
{
#define NS(_e, _s) return (struct ns_src){ (_e) + 1, _s }
	switch (slot) {
	case PM_NS_RX_PACKETS:	NS(ETS_RX_PKTS, "rx_packets");
	case PM_NS_TX_PACKETS:	NS(ETS_TX_PKTS, "tx_packets");
	case PM_NS_RX_BYTES:	NS(ETS_RX_BYTES, "rx_bytes");
	case PM_NS_TX_BYTES:	NS(ETS_TX_BYTES, "tx_bytes");
	case PM_NS_RX_DROPPED:	NS(ETS_RX_DROP, "rx_dropped");
	case PM_NS_MULTICAST:	NS(ETS_RX_MCAST, "multicast");
	case PM_NS_RX_CRC:	NS(ETS_FCS, "rx_crc_errors");
	case PM_NS_RX_FRAME:	NS(ETS_ALIGN, "rx_frame_errors");
	case PM_NS_RX_OVER:	NS(ETS_OVERSIZE, "rx_over_errors");
	case PM_NS_RX_BCAST:	NS(ETS_RX_BCAST, NULL);
	case PM_NS_TX_MCAST:	NS(ETS_TX_MCAST, NULL);
	case PM_NS_TX_BCAST:	NS(ETS_TX_BCAST, NULL);
	case PM_NS_RX_UNDER:	NS(ETS_UNDERSIZE, NULL);
	case PM_NS_RX_FRAG:	NS(ETS_FRAGMENTS, NULL);
	case PM_NS_RX_JABBER:	NS(ETS_JABBERS, NULL);
	case PM_NS_TX_CRC:	NS(ETS_TX_CRC_ERR, NULL);
	case PM_NS_COL_SINGLE:	NS(ETS_SINGLE_COL, NULL);
	case PM_NS_COL_MULTI:	NS(ETS_MULTI_COL, NULL);
	case PM_NS_COL_LATE:	NS(ETS_LATE_COL, NULL);
	case PM_NS_COL_EXCESS:	NS(ETS_XS_COL, NULL);
	case PM_NS_DEFER:	NS(ETS_TX_DEFER, NULL);
	default:
		break;
	}
#undef NS
	/* the RX and TX histogram bins are consecutive in both enums */
	if (slot >= PM_NS_RX_BIN0 && slot < __PM_NS_MAX)
		return (struct ns_src){ ETS_RX_BIN0 + (slot - PM_NS_RX_BIN0) + 1,
					NULL };

	return (struct ns_src){ 0, NULL };
}

/* the readings of one device in the running tick */
static struct ns_snap ns_cache[4];
static unsigned int ns_next;

static int sysfs_read(const char *dev, const char *name, uint64_t *v)
{
	char path[96], buf[32];
	FILE *f;
	int n;

	if (strchr(dev, '/'))
		return -EINVAL;
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", dev,
		 name);
	f = fopen(path, "r");
	if (!f)
		return -errno;
	n = fgets(buf, sizeof(buf), f) ? 0 : -EIO;
	fclose(f);
	if (!n)
		*v = strtoull(buf, NULL, 10);

	return n;
}

static struct ns_snap *ns_snap(const char *dev)
{
	struct ns_snap *sn = NULL;
	unsigned int i;

	for (i = 0; i < ARRSZ(ns_cache); i++)
		if (ns_cache[i].valid && !strcmp(ns_cache[i].dev, dev))
			sn = &ns_cache[i];
	if (sn && sn->tick == tick_no)
		return sn;
	if (!sn) {
		sn = &ns_cache[ns_next++ % ARRSZ(ns_cache)];
		memset(sn, 0, sizeof(*sn));
		snprintf(sn->dev, sizeof(sn->dev), "%s", dev);
	}
	sn->valid = true;
	sn->tick = tick_no;
	memset(sn->sys_done, 0, sizeof(sn->sys_done));
	sn->ets_err = omci_pm_ops.ethstats(dev, &sn->ets);
	if (sn->ets_err)
		memset(&sn->ets, 0, sizeof(sn->ets));

	return sn;
}

/* a slot with a single source: ethtool, else sysfs, else 0 */
static int ns_simple(struct ns_snap *sn, unsigned int slot, uint64_t *v)
{
	struct ns_src src = ns_src_of(slot);

	*v = 0;
	if (src.ets && ets_have(&sn->ets, src.ets - 1)) {
		*v = sn->ets.v[src.ets - 1];
		return 0;
	}
	if (!src.sys)
		return 0;
	if (!sn->sys_done[slot]) {
		sn->sys_done[slot] = true;
		sn->sys_err[slot] = sysfs_read(sn->dev, src.sys,
					       &sn->sys[slot]);
	}
	if (sn->sys_err[slot])
		return sn->sys_err[slot];
	*v = sn->sys[slot];

	return 0;
}

/* the sum of slots (SDK macMT7530GetPortRxDropFramesCnt() and friends) */
static int ns_sum(struct ns_snap *sn, const uint8_t *slots, unsigned int n,
		  uint64_t *v)
{
	unsigned int i;
	int ret = 0;

	*v = 0;
	for (i = 0; i < n; i++) {
		uint64_t x;
		int r = ns_simple(sn, slots[i], &x);

		if (r && !ret)
			ret = r;
		*v += x;
	}

	return ret;
}

static int def_netstat(const char *dev, unsigned int stat, uint64_t *v)
{
	/* SDK macMT7530GetPortRxDropFramesCnt() (ethcmd/mtkswitch_api.c:
	 * 583-596): alignment + FCS + undersize + oversize + fragments +
	 * jabbers + RX_DROP */
	static const uint8_t discard[] = {
		PM_NS_RX_FRAME, PM_NS_RX_CRC, PM_NS_RX_UNDER, PM_NS_RX_OVER,
		PM_NS_RX_FRAG, PM_NS_RX_JABBER, PM_NS_RX_DROPPED,
	};
	struct ns_snap *sn;
	int ret;

	if (stat >= __PM_NS_MAX || strlen(dev) >= sizeof(sn->dev))
		return -EINVAL;
	sn = ns_snap(dev);
	switch (stat) {
	case PM_NS_RX_LACKDROP:
		/* SDK macMT7530GetPortRxLackDropPktCnt() (mtkswitch_api.c:
		 * 612-622): RX_DROP + RX_CTRL_DROP + RX_ING_DROP; the last
		 * two only exist in the driver's ethtool -S strings */
		ret = ns_simple(sn, PM_NS_RX_DROPPED, v);
		if (ets_have(&sn->ets, ETS_RX_CTRL_DROP))
			*v += sn->ets.v[ETS_RX_CTRL_DROP];
		if (ets_have(&sn->ets, ETS_RX_ING_DROP))
			*v += sn->ets.v[ETS_RX_ING_DROP];
		return ret;
	case PM_NS_RX_DISCARD:
		return ns_sum(sn, discard, ARRSZ(discard), v);
	default:
		return ns_simple(sn, stat, v);
	}
}

struct omci_pm_ops omci_pm_ops = {
	.now_ms = def_now,
	.global = kx_counters_global,
	.gem = kx_counters_gem,
	.netstat = def_netstat,
	.ethstats = ets_read,
};

/* ---- helpers ---- */

/* the alarm numbers of an ME: those its counters have a threshold for */
static void build_alarms(void)
{
	static bool done;
	unsigned int i, j, k, t;

	if (done)
		return;
	done = true;
	for (i = 0; i < PM_NME; i++) {
		struct pm_me *m = &pm_me[i];

		for (t = 0; t < 2; t++) {
			const struct pm_ctr *c = t ? m->ctr_down : m->ctr;
			unsigned int n = t ? m->nctr_down : m->nctr;

			for (j = 0; j < n; j++) {
				if (!c[j].thr)
					continue;
				for (k = 0; k < m->nalarm; k++)
					if (m->alarm[k] == c[j].alarm)
						break;
				if (k == m->nalarm && k < PM_MAX_THR)
					m->alarm[m->nalarm++] = c[j].alarm;
			}
		}
	}
}

static const struct pm_me *pm_find(uint16_t class_id)
{
	unsigned int i;

	build_alarms();
	for (i = 0; i < PM_NME; i++)
		if (pm_me[i].class_id == class_id)
			return &pm_me[i];

	return NULL;
}

bool omci_pm_is_pm(const struct omci_me_def *d)
{
	return pm_find(d->class_id) != NULL;
}

unsigned int omci_pm_alarms(const struct omci_me_def *d, const uint8_t **l)
{
	const struct pm_me *m = pm_find(d->class_id);

	if (!m)
		return 0;
	*l = m->alarm;

	return m->nalarm;
}

/* the control block of an extended PM: SDK EthExtndPM64CtrlBlk_t, 16 bytes
 * (omci_me_layer2_data_services.h:88-102): threshold id, parent class,
 * parent instance, accumulation disable, TCA disable, control fields, TCI */
static const uint8_t *ctl_block(struct omci_inst *in)
{
	const uint8_t *c = mib_val(in, 2);

	return c && in->len[2] >= 16 ? c : NULL;
}

static const struct pm_ctr *pm_ctrs(const struct pm_me *m,
				    const struct pm_inst *p, unsigned int *n)
{
	if (p->down) {
		*n = m->nctr_down;
		return m->ctr_down;
	}
	*n = m->nctr;

	return m->ctr;
}

/* the Linux device behind a PPTP Ethernet UNI instance, NULL: none */
static const char *uni_of_pptp(uint16_t id)
{
	return mib_find(11, id) ? eth_uni_dev(id) : NULL;
}

/* ME 47 whose TP is a PPTP Ethernet UNI */
static const char *uni_of_bport(uint16_t id)
{
	struct omci_inst *bp = mib_find(47, id);

	if (!bp || mib_get_uint(bp, 3) != TP_PPTP_ETH_UNI)
		return NULL;

	return uni_of_pptp(mib_get_uint(bp, 4));
}

/*
 * Where the counters of one instance come from; @key names it, a change
 * starts the counters from the new source's current value.
 */
static const char *pm_source(struct omci_inst *in, const struct pm_me *m,
			     struct pm_inst *p, char *key, size_t ks,
			     int *gem)
{
	const char *dev = NULL;
	const uint8_t *c;

	*gem = -1;
	p->down = false;
	switch (m->class_id) {
	case 24:
	case 89:
	case 296:
		dev = uni_of_pptp(in->id);
		break;
	case 52:
	case 321:
	case 322:
		dev = uni_of_bport(in->id);
		break;
	case 334:
	case 425:
		c = ctl_block(in);
		if (!c)
			break;
		/* control fields: bit 14 / 15 of the SDK bit-field filter by
		 * VID / P-bit, which device counters cannot do */
		if (rd_be16(c + 10) & 0xc000)
			break;
		p->down = rd_be16(c + 10) & 0x0002;	/* direction */
		if (rd_be16(c + 2) == 11)
			dev = uni_of_pptp(rd_be16(c + 4));
		else if (rd_be16(c + 2) == 47)
			dev = uni_of_bport(rd_be16(c + 4));
		break;
	case 341:
	case 267: {
		struct omci_inst *ctp = mib_find(268, in->id);

		if (ctp)
			*gem = mib_get_uint(ctp, 1);
		break;
	}
	case 312:
		snprintf(key, ks, "phy");
		return "phy";
	default:
		break;
	}

	if (m->kind == PM_GEM) {
		if (*gem < 0)
			snprintf(key, ks, "-");
		else
			snprintf(key, ks, "gem%d", *gem);
		return NULL;
	}
	snprintf(key, ks, "%s%s", dev ? dev : "-", p->down ? ":dn" : "");

	return dev;
}

/* ---- threshold data (ME 273 values 1-7, ME 274 values 8-14) ---- */

static bool thr_id(uint16_t id)
{
	return id && id != 0xffff;	/* SDK: 0 and 0xffff: none */
}

static uint32_t pm_threshold(struct omci_inst *in, unsigned int thr)
{
	struct omci_inst *t;
	uint16_t id = mib_get_uint(in, 2);

	if (!thr_id(id) || thr < 1 || thr > PM_MAX_THR)
		return 0;
	t = thr <= 7 ? mib_find(273, id) : mib_find(274, id);
	if (!t)
		return 0;

	return mib_get_uint(t, thr <= 7 ? thr : thr - 7);
}

bool omci_pm_check(struct omci_inst *in, uint8_t idx, const uint8_t *v)
{
	const struct pm_me *m = pm_find(in->def->class_id);
	uint16_t id;

	if (!m || idx != 2 || (m->ext && m->class_id == 425))
		return true;
	/* threshold data 1/2 id; ME 334 carries it in the control block,
	 * which the SDK table does not let the OLT write */
	id = rd_be16(v);
	if (!thr_id(id))
		return true;
	/* SDK set...ThresholdData(): getThresholdDataValue() fails -> the
	 * Set fails; ME 24 reads threshold data 2 as well (:1281-1316) */
	if (!mib_find(273, id))
		return false;

	return m->class_id != 24 || mib_find(274, id);
}

/* ---- counters ---- */

static uint64_t delta(uint64_t last, uint64_t v, bool w32)
{
	if (v >= last)
		return v - last;
	if (w32 && last - v > 0x80000000ull)
		return v + 0x100000000ull - last;

	return v;		/* the counter was cleared */
}

static uint64_t clamp_size(uint64_t v, unsigned int size)
{
	uint64_t max = size >= 8 ? ~0ull : (1ull << (8 * size)) - 1;

	return v > max ? max : v;
}

/* the global PHY counters: one read per tick */
static struct {
	unsigned int tick;
	bool valid, ok;
	uint64_t cnt[__GPON_CNT_MAX];
} gsnap;

static bool fetch(struct omci_inst *in, const struct pm_me *m,
		  struct pm_inst *p, const char *dev, int gem, uint64_t *raw,
		  bool *w32)
{
	const struct pm_ctr *ctr;
	unsigned int i, n;
	uint64_t g[4];

	*w32 = false;
	switch (m->kind) {
	case PM_FEC:
		if (!gsnap.valid || gsnap.tick != tick_no) {
			gsnap.valid = true;
			gsnap.tick = tick_no;
			gsnap.ok = !omci_pm_ops.global(gsnap.cnt);
		}
		if (!gsnap.ok)
			return false;
		raw[S_FEC_BYTES] = gsnap.cnt[GPON_CNT_FEC_CORR_BYTES];
		raw[S_FEC_CW] = gsnap.cnt[GPON_CNT_FEC_CORR_CW];
		raw[S_FEC_UNCORR] = gsnap.cnt[GPON_CNT_FEC_UNCORR_CW];
		raw[S_FEC_TOTAL] = gsnap.cnt[GPON_CNT_FEC_TOTAL_CW];
		raw[S_FEC_SEC] = gsnap.cnt[GPON_CNT_FEC_SECONDS];
		*w32 = true;	/* PHY counters are 32 bit */
		return true;
	case PM_GEM:
		if (gem < 0 || omci_pm_ops.gem(gem, g))
			return false;
		for (i = 0; i < 4; i++)
			raw[i] = g[i];
		return true;
	case PM_NET:
		if (!dev)
			return false;
		ctr = pm_ctrs(m, p, &n);
		for (i = 0; i < n; i++) {
			if (ctr[i].slot == NOSRC)
				continue;
			if (omci_pm_ops.netstat(dev, ctr[i].slot,
						&raw[ctr[i].slot]))
				return false;
		}
		return true;
	default:
		return false;
	}
}

static bool locked(struct omci_inst *in)
{
	struct omci_inst *par = NULL;
	const struct omci_me_def *d;

	switch (in->def->class_id) {
	case 24:
	case 89:
	case 296:
		par = mib_find(11, in->id);
		break;
	case 312:
		par = mib_find(263, in->id);
		break;
	default:
		break;
	}
	if (!par)
		return false;
	d = par->def;

	return d->admin && mib_get_uint(par, d->admin) == 1;
}

/* is TCA @thr switched off by the control block of an extended PM */
static bool tca_off(struct omci_inst *in, const struct pm_me *m,
		    unsigned int thr)
{
	const uint8_t *c;
	uint16_t dis;

	if (!m->ext || !(c = ctl_block(in)))
		return false;
	dis = rd_be16(c + 8);

	return (dis & TCA_DISABLE_ALL) || (dis & (1u << (thr - 1)));
}

static uint64_t ctr_val(const struct pm_ctr *c, const uint64_t *bank,
			unsigned int size)
{
	return clamp_size(bank[c->attr] / (c->div ? c->div : 1), size);
}

static void tca_eval(struct omci_inst *in, const struct pm_me *m,
		     struct pm_inst *p)
{
	const struct pm_ctr *ctr;
	unsigned int i, n;

	ctr = pm_ctrs(m, p, &n);
	for (i = 0; i < n; i++) {
		const struct pm_ctr *c = &ctr[i];
		uint32_t th;

		if (!c->thr || (p->tca & (1u << (c->thr - 1))) ||
		    tca_off(in, m, c->thr))
			continue;
		th = pm_threshold(in, c->thr);
		if (!th || ctr_val(c, p->cur, in->len[c->attr]) < th)
			continue;
		p->tca |= 1u << (c->thr - 1);
		omci_alarm_set(m->class_id, in->id, c->alarm, true,
			       !locked(in));
	}
}

static void tca_clear(struct omci_inst *in, const struct pm_me *m,
		      struct pm_inst *p)
{
	unsigned int i, n;
	const struct pm_ctr *ctr;

	if (!p->tca)
		return;
	ctr = pm_ctrs(m, p, &n);
	for (i = 0; i < n; i++)
		if (ctr[i].thr && (p->tca & (1u << (ctr[i].thr - 1))))
			omci_alarm_set(m->class_id, in->id, ctr[i].alarm,
				       false, !locked(in));
	p->tca = 0;
}

static void poll_inst(struct omci_inst *in, const struct pm_me *m,
		      struct pm_inst *p)
{
	uint64_t raw[__PM_NS_MAX > 8 ? __PM_NS_MAX : 8] = { 0 };
	const struct pm_ctr *ctr;
	char key[sizeof(p->key)];
	unsigned int i, n;
	const char *dev;
	bool w32;
	int gem;
	const uint8_t *cb;

	dev = pm_source(in, m, p, key, sizeof(key), &gem);
	if (strcmp(key, p->key)) {
		snprintf(p->key, sizeof(p->key), "%s", key);
		memset(p->base, 0, sizeof(p->base));
	}
	cb = m->ext ? ctl_block(in) : NULL;
	p->stopped = cb && (rd_be16(cb + 6) & 0x8000);	/* accumulation off */

	if (!fetch(in, m, p, dev, gem, raw, &w32)) {
		memset(p->base, 0, sizeof(p->base));
		return;
	}
	ctr = pm_ctrs(m, p, &n);
	for (i = 0; i < n; i++) {
		const struct pm_ctr *c = &ctr[i];
		uint64_t v;

		if (c->slot == NOSRC)
			continue;
		v = raw[c->slot];
		if (p->base[c->attr] && !p->stopped)
			p->cur[c->attr] += delta(p->last[c->attr], v, w32);
		p->last[c->attr] = v;
		p->base[c->attr] = true;
	}
	if (p->stopped)
		return;
	tca_eval(in, m, p);
}

static void for_each_pm(void (*fn)(struct omci_inst *, const struct pm_me *,
				   struct pm_inst *))
{
	struct omci_inst *in;

	list_for_each_entry(in, &omci_mib, list) {
		const struct pm_me *m;

		if (!in->pm || !(m = pm_find(in->def->class_id)))
			continue;
		fn(in, m, in->pm);
	}
}

static void poll_cb(struct omci_inst *in, const struct pm_me *m,
		    struct pm_inst *p)
{
	poll_inst(in, m, p);
}

/* @keep: the running bank is the finished interval */
static bool finish_keep;

static void finish_cb(struct omci_inst *in, const struct pm_me *m,
		      struct pm_inst *p)
{
	unsigned int i;

	for (i = 1; i <= OMCI_MAX_ATTR; i++) {
		if (!finish_keep)
			p->hist[i] = 0;
		else if (m->ext)
			p->hist[i] += p->cur[i];
		else
			p->hist[i] = p->cur[i];
		p->cur[i] = 0;
	}
	tca_clear(in, m, p);
}

static void poll_all(void)
{
	tick_no++;
	st.polls++;
	for_each_pm(poll_cb);
}

static void schedule(void)
{
	uint64_t now = omci_pm_ops.now_ms(), end = st.start + OMCI_PM_INTERVAL_MS;
	uint64_t d = now >= end ? 1 : end - now;

	uloop_timeout_set(&tick_tmo, d < OMCI_PM_POLL_MS ? d : OMCI_PM_POLL_MS);
}

void omci_pm_tick(void)
{
	uint64_t now = omci_pm_ops.now_ms();
	unsigned int n = 0;

	poll_all();
	/* the final read of an interval is the one just made */
	finish_keep = true;
	while (now >= st.start + OMCI_PM_INTERVAL_MS) {
		for_each_pm(finish_cb);
		st.counter++;
		st.intervals++;
		st.start += OMCI_PM_INTERVAL_MS;
		if (++n >= 4) {		/* the clock jumped */
			st.start = now;
			break;
		}
		/* intervals nobody counted in */
		finish_keep = false;
	}
}

static void tick_cb(struct uloop_timeout *t)
{
	omci_pm_tick();
	schedule();
}

void omci_pm_sync_time(void)
{
	poll_all();
	finish_keep = st.synced;
	for_each_pm(finish_cb);
	st.counter = 0;
	st.synced = true;
	st.start = omci_pm_ops.now_ms();
	INFO("OMCI: PM interval synchronised");
	schedule();
}

void omci_pm_mib_reset(void)
{
	/* the instances are gone with the MIB (omci_pm_inst_free) */
	st.counter = 0;
	st.synced = false;
	st.start = omci_pm_ops.now_ms();
	gsnap.valid = false;
	schedule();
}

void omci_pm_init(void)
{
	memset(&st, 0, sizeof(st));
	gsnap.valid = false;
	st.start = omci_pm_ops.now_ms();
	tick_tmo.cb = tick_cb;
	schedule();
}

void omci_pm_inst_create(struct omci_inst *in)
{
	const struct pm_me *m = pm_find(in->def->class_id);
	struct pm_inst *p;

	if (!m || in->pm)
		return;
	p = calloc(1, sizeof(*p));
	if (!p)
		return;
	in->pm = p;
	poll_inst(in, m, p);	/* the baseline of the counters */
}

void omci_pm_inst_free(struct omci_inst *in)
{
	free(in->pm);
	in->pm = NULL;
}

void omci_pm_set(struct omci_inst *in, uint16_t mask)
{
	const struct pm_me *m = pm_find(in->def->class_id);
	struct pm_inst *p = in->pm;
	const uint8_t *c;

	if (!m || !p || !m->ext || !(mask & omci_mask(2)) ||
	    !(c = ctl_block(in)))
		return;
	/* accumulation disable bit 14 (G.988, not in the SDK sources):
	 * clear the counters */
	if (rd_be16(c + 6) & 0x4000) {
		poll_inst(in, m, p);
		memset(p->cur, 0, sizeof(p->cur));
		memset(p->hist, 0, sizeof(p->hist));
		tca_clear(in, m, p);
	}
}

void omci_pm_refresh(struct omci_inst *in, bool current)
{
	const struct pm_me *m = pm_find(in->def->class_id);
	struct pm_inst *p = in->pm;
	const struct pm_ctr *ctr;
	unsigned int i, n;

	if (!m || !p)
		return;
	mib_set_uint(in, 1, st.counter);
	ctr = pm_ctrs(m, p, &n);
	for (i = 0; i < n; i++) {
		const struct pm_ctr *c = &ctr[i];
		unsigned int size = in->len[c->attr];
		uint64_t v = ctr_val(c, current ? p->cur : p->hist, size);
		uint8_t *d = mib_val(in, c->attr);
		int k;

		if (!d)
			continue;
		for (k = size - 1; k >= 0; k--, v >>= 8)
			d[k] = v & 0xff;
	}
}

void omci_pm_parent_unlocked(struct omci_inst *parent)
{
	struct omci_inst *in;

	list_for_each_entry(in, &omci_mib, list) {
		uint16_t c = in->def->class_id;

		if (!in->pm || in->id != parent->id)
			continue;
		if (((c == 24 || c == 89 || c == 296) &&
		     parent->def->class_id == 11) ||
		    (c == 312 && parent->def->class_id == 263))
			omci_alarm_unlocked(in);
	}
}

/* ---- ubus ---- */

static void bank_dump(struct blob_buf *b, const char *name,
		      struct omci_inst *in, const struct pm_me *m,
		      struct pm_inst *p, const uint64_t *bank)
{
	const struct pm_ctr *ctr;
	unsigned int i, n;
	void *t = blobmsg_open_table(b, name);

	ctr = pm_ctrs(m, p, &n);
	for (i = 0; i < n; i++) {
		const struct omci_attr_def *a = omci_attr(in->def, ctr[i].attr);

		if (a)
			blobmsg_add_u64(b, a->name,
					ctr_val(&ctr[i], bank, in->len[ctr[i].attr]));
	}
	blobmsg_close_table(b, t);
}

void omci_pm_inst_dump(struct blob_buf *b, struct omci_inst *in)
{
	const struct pm_me *m = pm_find(in->def->class_id);
	struct pm_inst *p = in->pm;
	void *t, *a;
	unsigned int i, n;
	const struct pm_ctr *ctr;

	if (!m || !p)
		return;
	t = blobmsg_open_table(b, "pm");
	blobmsg_add_string(b, "source", p->key);
	bank_dump(b, "current", in, m, p, p->cur);
	bank_dump(b, "history", in, m, p, p->hist);
	a = blobmsg_open_table(b, "threshold");
	ctr = pm_ctrs(m, p, &n);
	for (i = 0; i < n; i++) {
		const struct omci_attr_def *ad = omci_attr(in->def, ctr[i].attr);

		if (ctr[i].thr && ad)
			blobmsg_add_u32(b, ad->name, pm_threshold(in, ctr[i].thr));
	}
	blobmsg_close_table(b, a);
	a = blobmsg_open_array(b, "tca");
	for (i = 0; i < n; i++)
		if (ctr[i].thr && (p->tca & (1u << (ctr[i].thr - 1))))
			blobmsg_add_u32(b, NULL, ctr[i].alarm);
	blobmsg_close_array(b, a);
	blobmsg_close_table(b, t);
}

void omci_pm_dump(struct blob_buf *b)
{
	uint64_t now = omci_pm_ops.now_ms();
	struct omci_inst *in;
	void *a, *t;

	blobmsg_add_u32(b, "interval_end_time", st.counter);
	blobmsg_add_u8(b, "synchronized", st.synced);
	blobmsg_add_u32(b, "interval_s", OMCI_PM_INTERVAL_MS / 1000);
	blobmsg_add_u32(b, "elapsed_s", (now - st.start) / 1000);
	blobmsg_add_u32(b, "polls", st.polls);
	blobmsg_add_u32(b, "intervals", st.intervals);
	a = blobmsg_open_array(b, "instances");
	list_for_each_entry(in, &omci_mib, list) {
		if (!in->pm)
			continue;
		t = blobmsg_open_table(b, NULL);
		blobmsg_add_u32(b, "class", in->def->class_id);
		blobmsg_add_string(b, "name", in->def->name);
		blobmsg_add_u32(b, "instance", in->id);
		omci_pm_inst_dump(b, in);
		blobmsg_close_table(b, t);
	}
	blobmsg_close_array(b, a);
}
