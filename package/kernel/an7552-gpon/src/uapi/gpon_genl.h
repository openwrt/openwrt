/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 * GPON MAC generic netlink ABI ("gpon").
 * Shared by the kernel driver and the userspace daemon (gpond).
 *
 * Integer attributes are host order.  The kernel keeps only dynamic
 * state (lost on deactivation or reboot); persistent configuration is the
 * daemon's.
 */
#ifndef _UAPI_GPON_GENL_H
#define _UAPI_GPON_GENL_H

#include <linux/types.h>

#define GPON_GENL_NAME		"gpon"
/*
 * Bumped to 12 when CMD_TUNE, CMD_PHY_CTRL and CMD_QDMA_CNGST moved to
 * debugfs.  Commands are dispatched by value, so every number after the ones
 * that were removed shifted: a daemon built against a different version would
 * send a number this side reads as a different command, and be refused by
 * nothing.  gpond checks this at startup and refuses to run on a mismatch -
 * that check, not the number, is what keeps the two builds apart.
 *
 * 13 adds ATTR_DEACTIVATE.  Appending an attribute shifts nothing, but a
 * daemon that sends it to a module that does not know it gets -EINVAL; the
 * version check is what turns that into "mismatched packages" at startup.
 *
 * 14 adds CMD_VID_MAP_SET/DEL with ATTR_ANI_VID and ATTR_PCP.
 */
#define GPON_GENL_VERSION	14

enum {
	GPON_CMD_UNSPEC,
	GPON_CMD_OMCI,			/* tx request / rx event */
	GPON_CMD_GEM_ADD,		/* GEM, ALLOC_ID, [MCAST] */
	GPON_CMD_GEM_DEL,		/* GEM */
	GPON_CMD_SET_US_GEM,		/* GEM: default upstream port */
	GPON_CMD_GET_STATE,		/* request and "event" */
	GPON_CMD_GEM_FLUSH,		/* every GEM but the OMCC */
	GPON_CMD_TCONT_ADD,		/* ALLOC_ID */
	GPON_CMD_TCONT_DEL,		/* ALLOC_ID */
	GPON_CMD_US_RULE_SET,		/* RULE, VID, PCP_MASK, GEM */
	GPON_CMD_US_RULE_DEL,		/* RULE */
	GPON_CMD_PCP_QUEUE,		/* PCP_MAP (8 bytes) */
	GPON_CMD_QOS_SCHED,		/* ALLOC_ID, SCHED_MODE, WEIGHTS */
	GPON_CMD_QOS_RATE,		/* ALLOC_ID, RATE, BUCKET */
	GPON_CMD_GET_COUNTERS,		/* [GEM | ALLOC_ID] [CLEAR] */
	GPON_CMD_CLEAR_COUNTERS,
	GPON_CMD_ONU_CTRL,		/* EMERGENCY SILENCE MIB_RESET ... */
	GPON_CMD_GET_TABLES,		/* dump GEM and T-CONT tables */
	GPON_CMD_PLOAM,			/* event: a PLOAM the kernel does not act on */
	GPON_CMD_TOD,			/* GPON: [TOD_SF TOD_SEC TOD_NSEC] set, reply current;
					 * event: ToD at the first 1PPS after the update */
	GPON_CMD_DSCP_MAP,		/* [DSCP_MAP] [DSCP_EN]: the P-bit of
					 * untagged IP frames from the DSCP */
	GPON_CMD_VID_MAP_SET,		/* RULE, VID, [PCP_MASK], ANI_VID,
					 * [PCP]: the tc VID rewrite of the
					 * router side, for the PPE */
	GPON_CMD_VID_MAP_DEL,		/* RULE */
	GPON_CMD_MCAST_RX,		/* MCAST_RX: gate the multicast GEM
					 * ports in the hardware GEM table */
	__GPON_CMD_MAX,
};

enum {
	GPON_ATTR_UNSPEC,
	GPON_ATTR_OMCI,			/* binary */
	GPON_ATTR_SN,			/* binary, 8 */
	GPON_ATTR_PASSWORD,		/* binary, 10 */
	GPON_ATTR_GEM,			/* u16 */
	GPON_ATTR_ALLOC_ID,		/* u16 */
	GPON_ATTR_STATE,		/* u8, 1..7 = O1..O7 */
	GPON_ATTR_ONU_ID,		/* u8 */
	GPON_ATTR_OMCC,			/* u16 */
	GPON_ATTR_ALARMS,		/* u32, BIT(gpon_alarm) */
	GPON_ATTR_EQD,			/* u32 */
	GPON_ATTR_MCAST,		/* flag */
	GPON_ATTR_RULE,			/* u8 */
	GPON_ATTR_VID,			/* u16, 4096 = any */
	GPON_ATTR_PCP_MASK,		/* u8 */
	GPON_ATTR_PCP_MAP,		/* binary, 8 queue numbers */
	GPON_ATTR_SCHED_MODE,		/* u8, enum airoha_xpon_sched */
	GPON_ATTR_WEIGHTS,		/* binary, 8 x u16 */
	GPON_ATTR_RATE,			/* u32, kbit/s */
	GPON_ATTR_BUCKET,		/* u32, bytes */
	GPON_ATTR_CLEAR,		/* flag: read-clear */
	GPON_ATTR_CNT,			/* nest of u64 counters */
	GPON_ATTR_EMERGENCY,		/* u8 0/1 */
	GPON_ATTR_SILENCE,		/* u8 0/1 */
	GPON_ATTR_MIB_RESET,		/* flag */
	GPON_ATTR_MIB_SYNCED,		/* u8 0/1 */
	GPON_ATTR_UP_OMCI,		/* u8 0/1 */
	GPON_ATTR_UP_DATA,		/* u8 0/1 */
	GPON_ATTR_SD,			/* u8 */
	GPON_ATTR_SF,			/* u8 */
	GPON_ATTR_TO1,			/* u32, ms */
	GPON_ATTR_TO2,			/* u32, ms */
	GPON_ATTR_MODE,			/* u8, GPON_MODE_* */
	GPON_ATTR_ADMIN_DOWN,		/* u8 0/1 */
	GPON_ATTR_PLOAM,		/* binary, 12 */
	GPON_ATTR_CHANNEL,		/* u8 */
	GPON_ATTR_ENCRYPT,		/* u8 0/1 */
	GPON_ATTR_CFG_DONE,		/* flag: OMCI configuration done */
	GPON_ATTR_TOD_SF,		/* u32, superframe counter */
	GPON_ATTR_TOD_SEC,		/* u32 */
	GPON_ATTR_TOD_NSEC,		/* u32 */
	GPON_ATTR_TRAFFIC_UP,		/* u8 0/1: config done + a data GEM */
	/* CMD_DSCP_MAP */
	GPON_ATTR_DSCP_MAP,		/* binary, 64: P-bit (0..7) per DSCP */
	GPON_ATTR_DSCP_EN,		/* u8 0/1: untagged IPv4/IPv6 frames get
					 * the P-bit of their DSCP for the
					 * upstream classification */
	/* measurements, replies only; read only in the hardware */
	GPON_ATTR_SPF_CNT,		/* u32: downstream superframe counter
					 * (DBG_DS_SPF_CNT), free running */
	GPON_ATTR_ACTIVATE,		/* flag: open the activation gate, let
					 * this ONU be announced.  Refused with
					 * -ENODEV while no optical front end
					 * is attached, and a no-op once past
					 * O2.  When it arrives together with
					 * ATTR_SN / ATTR_PASSWORD the identity
					 * is stored first, so the gate is
					 * never left closed by its own
					 * message. */
	GPON_ATTR_DEACTIVATE,		/* flag: close the activation gate again
					 * and take a running link back to O1,
					 * so this ONU stops being announced.
					 * The counterpart of ATTR_ACTIVATE;
					 * a later start stays silent until
					 * ACTIVATE arrives again. */
	/* CMD_VID_MAP_SET */
	GPON_ATTR_ANI_VID,		/* u16: VID on the fibre */
	GPON_ATTR_PCP,			/* u8: P-bit on the fibre, 8 = keep */
	/* CMD_MCAST_RX */
	GPON_ATTR_MCAST_RX,		/* u8 0/1: 0 takes the multicast GEM
					 * ports out of the hardware GEM table
					 * (the MAC drops their frames before
					 * the QDMA), 1 puts them back.  It
					 * stays for ports created later, until
					 * the driver is loaded again; the
					 * software tables are not touched. */
	__GPON_ATTR_MAX,
};
#define GPON_ATTR_MAX	(__GPON_ATTR_MAX - 1)

/* GPON_ATTR_CNT nest */
enum {
	GPON_CNT_UNSPEC,
	GPON_CNT_PAD,
	GPON_CNT_RX_FRAMES,		/* per GEM: HW MIB */
	GPON_CNT_RX_BYTES,
	GPON_CNT_TX_FRAMES,
	GPON_CNT_TX_BYTES,
	GPON_CNT_PLOAM_RX,		/* global */
	GPON_CNT_PLOAM_TX,
	GPON_CNT_PLOAM_DROP,
	GPON_CNT_OMCI_RX,
	GPON_CNT_OMCI_TX,
	GPON_CNT_ACTIVATIONS,
	GPON_CNT_BIP,
	GPON_CNT_RX_GEM,		/* MAC DBG counters */
	GPON_CNT_RX_CRC_ERR,
	GPON_CNT_RX_GTC,
	GPON_CNT_TX_GEM,
	GPON_CNT_TX_BURST,
	GPON_CNT_HEC_ONE,
	GPON_CNT_HEC_TWO,
	GPON_CNT_HEC_UC,
	GPON_CNT_FEC_CORR_BYTES,	/* PHY */
	GPON_CNT_FEC_CORR_CW,
	GPON_CNT_FEC_UNCORR_CW,
	GPON_CNT_FEC_TOTAL_CW,
	GPON_CNT_FEC_SECONDS,
	GPON_CNT_LOF,
	GPON_CNT_RX_DROP_GEM,
	GPON_CNT_ROGUE,			/* rogue ONU events seen */
	__GPON_CNT_MAX,
};

/* multicast groups, in this order; names for genl_ctrl_resolve_grp() */
#define GPON_MCGRP_OMCI_NAME	"omci"
#define GPON_MCGRP_EVENT_NAME	"event"
enum {
	GPON_MCGRP_OMCI,
	GPON_MCGRP_EVENT,
};

/*
 * GPON_ATTR_MODE values.  These are the PON mode codes of the optical front
 * end: the MAC passes them to the laser driver as they are, so both drivers
 * and the daemon have to agree on them.  They live here because the netlink
 * ABI is what carries the mode between the daemon and the MAC, which is the
 * same value the laser is switched to.
 */
#define GPON_MODE_GPON		0
#define GPON_MODE_GPON_SYM	10
#define GPON_MODE_EPON		14

/* GPON_ATTR_ALARMS bits (subset of the SDK ALARM_INDEX_T) */
enum gpon_alarm {
	GPON_ALM_LOS,
	GPON_ALM_LOF,
	GPON_ALM_LCDG,
	GPON_ALM_SUF,
	GPON_ALM_DACT,
	GPON_ALM_DIS,
	GPON_ALM_MEM,
	GPON_ALM_PEE,
	GPON_ALM_MIS,
	GPON_ALM_SD,
	GPON_ALM_SF,
	GPON_ALM_RDI,
	GPON_ALM_NOPWD,
	__GPON_ALM_MAX,
};

#endif /* _UAPI_GPON_GENL_H */
