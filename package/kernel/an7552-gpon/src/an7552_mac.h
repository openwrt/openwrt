/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Airoha AN7552/AN7563 xPON - GPON MAC layer
 *
 * Half of the an7552-gpon driver: the G.984.3 activation (O1..O7), the
 * downstream PLOAM handlers, the T-CONT and GEM port tables, the pon%d
 * netdev and the generic netlink ABI the userspace daemon (gpond) drives.
 * The SerDes half is an7552_phy.h; the hardware map both build on is
 * an7552_hw.h.
 *
 * Register map: the AN7552 SDK xpon_1g gpon_mac_reg_c_header_en7521.h, built
 * with TCSUPPORT_CPU_EN7521 + TCSUPPORT_CPU_EN7523 + TCSUPPORT_CPU_AN7552
 * (the branch set of the stock firmware).  Sequences and default values
 * follow the stock xPON driver.
 *
 * Locking: x->lock serialises the activation state machine, the tables and
 * every register sequence; it is held across the whole of a PLOAM handler.
 * x->reg_lock is the short register lock for the atomic paths (IRQ, the EN7571
 * MBI hook).  Functions marked [lock] must be called with x->lock held.
 */

#ifndef _AN7552_MAC_H
#define _AN7552_MAC_H

#include <linux/atomic.h>
#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/leds.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/workqueue.h>

#include "an7552_hw.h"
#include "uapi/gpon_genl.h"
#include "an7552_phy.h"

struct seq_file;

/* DSCP extraction from the packet head */
#define IPV4_TOS_DSCP			GENMASK(7, 2)	/* TOS bits 7:2 */
#define IPV6_TCLASS_DSCP		GENMASK(11, 6)	/* traffic class bits 7:2 */

/* an7552_led.c: link LED blink period */
#define XPON_LED_BLINK_MS	500

/* ---- Defaults from gpon_init_private_data() (HIR 0xf, ASIC) ---- */
#define GPON_RSP_TIME_DEFAULT		0x577
#define GPON_RSP_TIME_SYM		0xaee
#define GPON_FINE_INT_DLY_DEFAULT	0x0d
#define GPON_IDLE_GEM_THLD_DEFAULT	0x5a
/*
 * pon0 MTU limit: 1540 is what ISP CPEs run on the GPON side so that PPPoE
 * gets a 1500 MTU (RFC 4638).  Well inside the two hard limits of the path:
 * GDM2 takes frames up to 2000 bytes (REG_GDM_LEN_CFG, stock 0x1fb51514),
 * and an AN7563 RX frame has to fit one 2 KiB page_pool buffer (no RX
 * scatter): 1982 bytes after the headroom, about 1720 after skb_shared_info.
 */
#define GPON_MAX_MTU			1540
/* pon0 TX offloads, the GDM set without RX checksum (AIROHA_HW_FEATURES) */
#define AN7552_TX_FEATURES		(NETIF_F_SG | NETIF_F_IP_CSUM |	\
					 NETIF_F_IPV6_CSUM | NETIF_F_TSO | \
					 NETIF_F_TSO6)
#define GPON_GUARD_BIT_NUM_DEFAULT	20
#define GPON_RDI_SEND_NUM_DEFAULT	10000
#define GPON_RDI_ERR_EN_DEFAULT		0xf8
#define GPON_RDI_ERR_THLD_DEFAULT	2
#define GPON_SLEEP_CNT_DEFAULT		0xffffffff
#define GPON_DBA_BLOCK_SIZE		48	/* gpon_dev_init(): 0x30 */
#define GPON_TOD_CLK_PERIOD		10	/* gpon_dev_init() */
#define GPON_TO1_MS			10000
#define GPON_TO2_MS			100
#define GPON_TO1_MAX_CNT		20	/* gpon_act.c TO1_TIMEOUT_MAX_CNT */
/* gpon_dev_init() GPON_SYM extras */
#define GPON_SYM_MAX_RDM_DLY		0x1d2
#define GPON_SYM_DT_PREF_OFS		0x800
#define GPON_SYM_PHY_TX_DLY		0x40
/* gpon_init_qdma_tx_buff() */
#define GPON_TXBUF_CHAN_THR		4
#define GPON_TXBUF_TOTAL_THR		0xe0
/* ploam_recv_upstream_overhead(): PHY_TX_EN_PATTERN */
#define GPON_PHY_TX_EN_PATTERN		0xaa
/* gpon_wan.c */
#define OMCI_CRC_LEN			4
#define OMCI_BASIC_MSG_FIX_LEN		48
#define OMCI_BASIC_MSG_DEV_ID		0x0a
#define OMCI_EXTENDED_MSG_DEV_ID	0x0b
/* xpon_netif.h enum QueInfo: queue7 is used for OAM/OMCI */
#define GPON_OMCI_TX_QUEUE		7

#define GPON_MAX_GEM_ID			4096
#define GPON_MAX_TCONT			32
/* gwan_create_new_gemport(): channel 0x20 multicast, 0x21 T-CONT unknown */
#define GPON_MULTICAST_CHANNEL		0x20
#define GPON_UNKNOWN_CHANNEL		0x21
#define GPON_MAX_US_RULES		32
#define GPON_US_VID_ANY			4096
#define GPON_MAX_VID_MAP		16
#define GPON_PCP_KEEP			8
#define GPON_DSCP_NUM			64	/* 6 bit DSCP */
#define GPON_MAX_PCP			7
/* gpon_init.c / xmcs_gpon.c */
#define GPON_SILENCE_MS			70000
#define GPON_SD_DEFAULT			9
#define GPON_SF_DEFAULT			5
#define GPON_SD_MIN			4
#define GPON_SD_MAX			9
#define GPON_SF_MIN			3
#define GPON_SF_MAX			8
#define GPON_SD_SF_ALARM_BASE		2488320000u
#define GPON_UNASSIGN_ONU_ID		0xff
#define GPON_UNASSIGN_ALLOC_ID		0xff	/* gponDevDisableAllTCont() */
#define GPON_UNASSIGN_GEM_ID		0xffff
#define GPON_PLOAM_BROADCAST		0xff
#define GPON_PLOAM_SYM_BROADCAST	0xfe
#define GPON_PLOAM_LEN			12
#define GPON_SN_LEN			8
#define GPON_PASSWD_LEN			10
#define GPON_KEY_LEN			16

/* ---- PLOAM message ids (gpon_ploam_raw.h) ---- */
enum {
	PLOAMD_UPSTREAM_OVERHEAD	= 0x01,
	PLOAMD_ASSIGN_ONU_ID		= 0x03,
	PLOAMD_RANGING_TIME		= 0x04,
	PLOAMD_DEACTIVATE_ONU_ID	= 0x05,
	PLOAMD_DISABLE_SERIAL_NUM	= 0x06,
	PLOAMD_ENCRYPTED_PORT_ID	= 0x08,
	PLOAMD_REQUEST_PASSWORD		= 0x09,
	PLOAMD_ASSIGN_ALLOC_ID		= 0x0a,
	PLOAMD_POPUP			= 0x0c,
	PLOAMD_REQUEST_KEY		= 0x0d,
	PLOAMD_CONFIG_PORT_ID		= 0x0e,
	PLOAMD_PEE			= 0x0f,
	PLOAMD_CPL			= 0x10,
	PLOAMD_PST			= 0x11,
	PLOAMD_BER_INTERVAL		= 0x12,
	PLOAMD_KEY_SWITCHING_TIME	= 0x13,
	PLOAMD_EXT_BURST_LENGTH		= 0x14,
	PLOAMD_PON_ID			= 0x15,
	PLOAMD_SWIFT_POPUP		= 0x16,
	PLOAMD_RANGING_ADJUSTMENT	= 0x17,
	PLOAMD_SLEEP_ALLOW		= 0x18,
	PLOAMD_MAX			= 0x19,
};

enum {
	PLOAMU_SERIAL_NUMBER		= 0x01,
	PLOAMU_PASSWORD			= 0x02,
	PLOAMU_DYING_GASP		= 0x03,
	PLOAMU_NO_MESSAGE		= 0x04,
	PLOAMU_ENCRYPTION_KEY		= 0x05,
	PLOAMU_PEE			= 0x06,
	PLOAMU_PST			= 0x07,
	PLOAMU_REI			= 0x08,
	PLOAMU_ACKNOWLEDGE		= 0x09,
	PLOAMU_SLEEP_REQUEST		= 0x0a,
};

/* gpon_ploam.c: retransmission counts */
#define PLOAMU_PASSWD_TIMES		3
#define PLOAMU_KEY_TIMES		3
#define PLOAMU_DYING_GASP_TIMES		3
#define PLOAMU_ACK_TIMES		3	/* CONFIG_SEND_ACK_PLOAMU=3 */

/* G.984.3 activation states, written to G_ACTIVATION_ST as is */
enum an7552_gpon_state {
	GPON_O1 = 1,
	GPON_O2,
	GPON_O3,
	GPON_O4,
	GPON_O5,
	GPON_O6,
	GPON_O7,
};

/* gponDevGetGemPortCounter() GPON_GEMPORT_STATS_TYPE_t */
enum {
	GPON_GEM_CNT_RX_FRAMES,
	GPON_GEM_CNT_RX_BYTES,
	GPON_GEM_CNT_TX_FRAMES,
	GPON_GEM_CNT_TX_BYTES,
	GPON_GEM_CNT_MAX,
};

/* struct an7552_mac.flags, see the field */
#define AN7552_MAC_DOWN	BIT(0)	/* an7552_mac_down() running or done */
#define AN7552_MAC_GONE	BIT(1)	/* an7552_mac_shutdown() has run */

struct an7552_gpon_gem {
	bool		valid;
	bool		encrypt;
	u8		channel;	/* T-CONT index, 0..31 */
	u16		alloc_id;
	/*
	 * Software counters: NAPI adds to rx, every TX queue of pon0 to tx,
	 * on any CPU at once, so they are atomic (a 64 bit add is two words
	 * on this 32 bit SoC).
	 */
	atomic64_t	rx_packets;
	atomic64_t	rx_bytes;
	atomic64_t	tx_packets;
	atomic64_t	tx_bytes;
};

enum an7552_led_mode {
	XPON_LED_OFF,
	XPON_LED_ON,
	XPON_LED_BLINK,
};

struct an7552_led {
	struct led_trigger	trig;
	enum an7552_led_mode	mode;
	bool			registered;
};

struct an7552_us_rule {
	bool		valid;
	u16		vid;		/* GPON_US_VID_ANY: any/untagged */
	u8		pcp_mask;	/* bit per PCP, 0 = any */
	u16		gem;
};

/*
 * VID rewrite of the router side: the UNI VID the router sends on pon0
 * (with one of the P-bits of @pcp_mask, 0 = any) leaves on the fibre as
 * @ani_vid, with P-bit @pcp (GPON_PCP_KEEP: unchanged).  The CPU path does
 * it with gpond's tc rules on pon0 egress; the PPE never sees them and
 * takes the rewrite from here for the upstream flows it forwards.
 */
struct an7552_vid_map {
	bool		valid;
	u16		uni_vid;
	u8		pcp_mask;
	u16		ani_vid;
	u8		pcp;
};

struct an7552_mac {
	struct device		*dev;
	void __iomem		*gpon;
	struct regmap		*scu;
	struct reset_control	*mac_rst;
	struct an7552_phy	*phy;	/* the SerDes half, same module */
	enum an7552_xpon_mode	mode;

	struct net_device	*wan_dev;	/* airoha-eth GDM (client handle) */
	struct net_device	*ndev;		/* pon0 */

	int			irq;
	int			dg_irq;

	/* serializes the activation state machine and table updates */
	struct mutex		lock;
	/*
	 * A mode change and a start exclude each other.  Separate from
	 * state_lock because an7552_mac_open() calls an7552_mac_start() under
	 * rtnl_lock(), while an7552_mac_up()/teardown() take rtnl_lock()
	 * inside state_lock.
	 */
	struct mutex		mode_lock;
	/*
	 * serializes RMW of MAC registers shared with atomic paths, and the
	 * three words of an upstream PLOAM
	 */
	spinlock_t		reg_lock;
	/* MBI_TX_STOP as the MAC set it; the front end's burst_ctrl keeps it */
	bool			mbi_tx_stopped;
	/* a Dying_Gasp went out: no other upstream PLOAM after it */
	bool			dying;
	struct workqueue_struct	*wq;
	struct delayed_work	to1_work;
	struct delayed_work	to2_work;
	struct delayed_work	ber_work;
	struct delayed_work	mib_work;
	struct delayed_work	silence_work;
	struct work_struct	reset_work;
	struct work_struct	phy_work;
	struct work_struct	carrier_work;
	struct work_struct	ppe_flush_work;	/* airoha_xpon_ppe_flush() */
	struct delayed_work	rogue_work;
	unsigned long		phy_events;	/* bit per an7552_xpon_event */

	bool			started;	/* WAN started by userspace */
	/*
	 * The activation gate.  Closed, an interface that comes up parks in O1
	 * with the receiver running and the transmitter off, so nothing is
	 * announced: no Serial_Number_ONU means no emission on the shared
	 * splitter.  Opened by an7552_mac_activate(), from the debugfs node
	 * or from CMD_ONU_CTRL.
	 *
	 * It survives an7552_mac_stop() and gpon_reset_ids() on purpose - a
	 * front end that detaches and comes back re-announces without asking
	 * again - and is cleared by an7552_mac_deactivate(), the explicit
	 * "stop announcing", and in an7552_mac_shutdown(), where a new device
	 * starts over.  Not a bool to test against the state: it is a
	 * decision, and it is made explicitly.
	 */
	bool			armed;
	bool			phy_ready;
	bool			emergency;	/* O7 by Disable_SN */
	bool			reset_from_o7;
	bool			silence;
	bool			mib_synced;
	bool			up_omci;
	bool			up_data;
	bool			admin_down;
	bool			traffic_up;	/* last value sent to the PHY */
	/*
	 * Rogue ONU: the PHY saw the transmitter enabled outside a grant and
	 * switched it off.  rogue is the latch, rogue_retry the restarts
	 * already attempted, and rogue_off keeps the transmitter down once
	 * those are used up (Upstream_Overhead would otherwise switch it on
	 * again at the next discovery window).  All three are cleared by a
	 * LOS or by an explicit activation.
	 */
	bool			rogue;
	bool			rogue_off;
	u8			rogue_retry;
	/*
	 * x->lock serialises the activation state machine, the tables and the
	 * register sequences.  It is never held across a call that takes an
	 * en7571 lock: the front end takes its own lock and calls back up into
	 * this MAC from its timers, so the two orders would close a cycle.
	 * state_lock serialises the down/up/shutdown transitions themselves: the
	 * flag bit below says which way the transition is going, and this one
	 * makes sure only one of them runs at a time and that a caller which
	 * arrives mid-transition waits for it instead of racing it.  Lock order
	 * is state_lock before x->lock.
	 */
	struct mutex		state_lock;
	/*
	 * AN7552_MAC_DOWN is set the moment an7552_mac_down() begins, so a
	 * second one does not start a teardown that is already running, and is
	 * cleared only once an7552_mac_up() has finished putting everything
	 * back.  It is a bit rather than a bool so that claiming it is a single
	 * atomic test_and_set_bit() with no window between the test and the set.
	 *
	 * While it is set pon0 has no optical path behind it: an7552_mac_down()
	 * has taken the interface administratively down and unregistered the
	 * generic netlink family, so nothing is driven from userspace.
	 *
	 * AN7552_MAC_GONE means the MAC is finished for good -
	 * an7552_mac_shutdown() has given back the workqueue, the interrupts and
	 * the netdev allocation, so nothing can be started here again however
	 * often a front end appears.
	 */
	unsigned long		flags;
	u8			sd_thld;
	u8			sf_thld;
	u32			to1_ms;
	u32			to2_ms;
	u32			activations;
	enum an7552_gpon_state	state;
	unsigned int		to1_left;

	u8			sn[GPON_SN_LEN];
	u8			passwd[GPON_PASSWD_LEN];
	bool			password_warned;	/* reported the missing password once */
	u8			key[GPON_KEY_LEN];
	u8			key_idx;
	bool			key_requested;
	/* Type B protection (gpon_detect_los_lof() in O5) */
	u32			typeb_key[4];	/* G_AES_ACTIVE_KEY0..3 */
	bool			typeb_popup;	/* gpGponPriv+0x29c bit 0 */
	u8			onu_id;
	u16			omcc;
	bool			omcc_encrypt;
	u32			eqd;
	u32			byte_delay;
	u32			bit_delay;
	u32			ber_interval_ms;
	u8			rei_seq;
	u32			bip_total;
	/* BIP the PHY read-to-clear counter gave to GET_COUNTERS, not yet
	 * seen by the BER interval (SD/SF, REI) */
	u32			bip_pending;
	u8			guard_bit_num;
	u16			rsp_time;
	u32			alarms;		/* BIT(gpon_alarm) */
	u8			prev_ploam[GPON_PLOAM_LEN];
	unsigned int		same_ploam;

	u16			tcont_alloc[GPON_MAX_TCONT];
	u32			tcont_valid;	/* BIT(channel) */
	unsigned int		tcont_cnt;	/* gpWanPriv tcont count, >= 1 */
	bool			cfg_done;	/* monitor_done() */
	/* gponCfg.eqdO4Offset / eqdO5Offset / flags.eqdOffsetFlag */
	u8			eqd_o4_offset;
	u8			eqd_o5_offset;
	bool			eqd_offset_sub;
	bool			tod_update;	/* flags.isTodUpdate */
	struct an7552_us_rule	us_rule[GPON_MAX_US_RULES];
	struct an7552_vid_map	vid_map[GPON_MAX_VID_MAP];
	bool			ppe_upstream;	/* upstream flows in the PPE */
	u8			pcp_queue[8];
	u8			dscp_pcp[GPON_DSCP_NUM]; /* untagged IP: DSCP -> PCP */
	bool			dscp_en;
	bool			dei_mark;	/* VLAN DEI -> TX descriptor DEI */
	struct an7552_gpon_gem	*gem;		/* GPON_MAX_GEM_ID entries */
	DECLARE_BITMAP(gem_encrypt_req, GPON_MAX_GEM_ID);
	/*
	 * GPON_CMD_MCAST_RX 0: the multicast GEM ports stay valid in the
	 * software table but not in the hardware one, so the MAC drops
	 * the multicast nobody listens to (gpond decides).  Written under
	 * lock, read by the PLOAM path too.
	 */
	bool			mcast_rx_off;
	u16			us_gem;		/* default upstream data GEM */

	struct {
		u64		ploam_rx;
		u64		ploam_tx;
		u64		ploam_rx_drop;
		u64		omci_rx;
		u64		omci_tx;
		u64		rx_drop_gem;
		/* data frames by the GDM's checksum verdict */
		u64		rx_csum_ok;
		u64		rx_csum_bad;
		u64		rx_csum_none;
		u64		irq_err;
		u64		rogue;
		u32		last_err_status;
	} stats;

	struct an7552_led	led_pon;	/* LED_XPON_STATUS */
	struct an7552_led	led_los;	/* LED_XPON_LOS_ON_STATUS */

	struct dentry		*debugfs;
};

static inline u32 mac_rr(struct an7552_mac *x, u32 off)
{
	return readl(x->gpon + off);
}

static inline void mac_wr(struct an7552_mac *x, u32 off, u32 val)
{
	writel(val, x->gpon + off);
}

static inline void mac_rmw(struct an7552_mac *x, u32 off, u32 clr, u32 set)
{
	unsigned long flags;

	spin_lock_irqsave(&x->reg_lock, flags);
	writel((readl(x->gpon + off) & ~clr) | set, x->gpon + off);
	spin_unlock_irqrestore(&x->reg_lock, flags);
}

/* module parameter "debug": what to print to the kernel log */
#define AN7552_DBG_ACT		AN7552_DBG_MAC
#define an7552_mac_dbg(_x, _bit, _fmt, ...)				\
	do {								\
		if (an7552_phy_debug & (_bit))				\
			dev_info((_x)->dev, "mac: " _fmt, ##__VA_ARGS__);	\
	} while (0)

/* an7552_mac_gpon.c */
int an7552_mac_init(struct an7552_mac *x);
irqreturn_t an7552_mac_irq_thread(int irq, void *data);
/* the phy_work handler itself, [lock] free */
void an7552_mac_handle_phy_event(struct an7552_mac *x, enum an7552_xpon_event ev);
void an7552_mac_start(struct an7552_mac *x);
void an7552_mac_stop(struct an7552_mac *x);
/* True while the MAC is out of service: no optical path, nothing driven */
static inline bool an7552_mac_is_down(struct an7552_mac *x)
{
	return test_bit(AN7552_MAC_DOWN, &x->flags);
}

/*
 * True once an7552_mac_shutdown() has run.  The only caller is the laser
 * registry's xpon_alarm()/xpon_burst_ctrl(), which hold the registry lock
 * across the call into the MAC, so reading the flag is safe against the
 * teardown that would otherwise free the memory it is read from.
 */
static inline bool an7552_mac_is_gone(struct an7552_mac *x)
{
	return test_bit(AN7552_MAC_GONE, &x->flags);
}
void an7552_mac_dying_gasp(struct an7552_mac *x);

/* Optical front end faults (an7552_mac_gpon.c) */
void an7552_mac_laser_fault(struct an7552_mac *x, u32 alarm);
void an7552_mac_burst_ctrl(struct an7552_mac *x, bool enable);
int an7552_mac_gem_add(struct an7552_mac *x, u16 gem, u16 alloc_id,
			bool mcast);
int an7552_mac_gem_del(struct an7552_mac *x, u16 gem);
int an7552_mac_gem_flush(struct an7552_mac *x);
int an7552_mac_gem_counters(struct an7552_mac *x, u16 gem, bool clear,
			     u64 cnt[GPON_GEM_CNT_MAX]);
int an7552_mac_tcont_add(struct an7552_mac *x, u16 alloc_id);
int an7552_mac_tcont_del(struct an7552_mac *x, u16 alloc_id);
int an7552_mac_tcont_counters(struct an7552_mac *x, u16 alloc_id,
			       bool clear, u64 cnt[GPON_GEM_CNT_MAX]);
bool an7552_mac_traffic_up(struct an7552_mac *x);
void an7552_mac_sw_resync(struct an7552_mac *x);
void an7552_mac_set_eqd_o5_offset(struct an7552_mac *x, u8 offset);
int an7552_mac_cfg_done(struct an7552_mac *x);
int an7552_mac_set_tod(struct an7552_mac *x, u32 superframe, u32 sec,
			u32 nsec);
void an7552_mac_get_tod(struct an7552_mac *x, u32 *sec, u32 *nsec);
/* gponDevGetSuperframe(): the downstream superframe counter, free running.
 * Two reads a known time apart give the number of superframes in between and
 * with that the grants per second the OLT grants this ONU */
u32 an7552_mac_get_superframe(struct an7552_mac *x);
int an7552_mac_us_rule_set(struct an7552_mac *x, unsigned int idx,
			    u16 vid, u8 pcp_mask, u16 gem);
int an7552_mac_us_rule_del(struct an7552_mac *x, u16 idx);
int an7552_mac_vid_map_set(struct an7552_mac *x, unsigned int idx,
			    u16 uni_vid, u8 pcp_mask, u16 ani_vid, u8 pcp);
int an7552_mac_vid_map_del(struct an7552_mac *x, u16 idx);
int an7552_mac_mcast_rx(struct an7552_mac *x, u16 on);
void an7552_mac_ppe_flush(struct an7552_mac *x);

/* an7552_mac_gpon.c: what userspace asks of the queues, the shapers and the
 * MAC itself (all of it reached from the genl doits above) */
int an7552_mac_us_default(struct an7552_mac *x, u16 gem);
int an7552_mac_pcp_queue(struct an7552_mac *x, const u8 map[8]);
int an7552_mac_dscp_map(struct an7552_mac *x, const u8 *map, int enable);
int an7552_mac_tcont_sched(struct an7552_mac *x, u16 alloc_id, u8 mode,
			    const u16 weights[8]);
int an7552_mac_tcont_rate(struct an7552_mac *x, u16 alloc_id, u32 kbps,
			   u32 bucket);
int an7552_mac_set_emergency(struct an7552_mac *x, bool on);
int an7552_mac_set_silence(struct an7552_mac *x, bool on);
int an7552_mac_mib_reset(struct an7552_mac *x);
int an7552_mac_set_sd_sf(struct an7552_mac *x, u8 sd, u8 sf);
int an7552_mac_set_timers(struct an7552_mac *x, u32 to1_ms, u32 to2_ms);
int an7552_mac_set_mode(struct an7552_mac *x, enum an7552_xpon_mode mode);
int an7552_mac_set_admin(struct an7552_mac *x, bool down);
void an7552_mac_clear_counters(struct an7552_mac *x);
void an7552_mac_dump_regs(struct an7552_mac *x, struct seq_file *m);

/*
 * The activation gate, and the identity it announces.  An identity arrives
 * already binary - GPON_SN_LEN or GPON_PASSWD_LEN bytes - because that is
 * what the netlink attributes carry; the text a human types is parsed by the
 * debugfs node into the same bytes, so both end up here.
 */
int an7552_mac_set_identity(struct an7552_mac *x, const void *buf, bool is_pwd);
int an7552_mac_set_identity_text(struct an7552_mac *x, const char *str,
				 bool is_pwd);
void an7552_mac_get_identity(struct an7552_mac *x, u8 *buf, bool is_pwd);
int an7552_mac_activate(struct an7552_mac *x);
int an7552_mac_deactivate(struct an7552_mac *x);

/* an7552_mac_main.c: the platform device, the netdev and the datapath */
/* traffic_status_refresh(): O5 means user traffic flows */
void an7552_mac_carrier(struct an7552_mac *x);
/* Take the MAC out of service, reversibly; what the front end going away does */
void an7552_mac_down(struct an7552_mac *x);
/* Put it back; what the front end registering again does */
int an7552_mac_up(struct an7552_mac *x);
/* an7552_mac_phy_event(): entry point of the PHY half (no PHY lock held);
 * filters and queues phy_work. */
void an7552_mac_phy_event(struct an7552_mac *x, enum an7552_xpon_event ev);
/* gwan_prepare_tx_message(): the OMCC GEM port, from genl OMCI requests */
int an7552_mac_omci_tx(struct an7552_mac *x, const void *data, size_t len);

/* an7552_mac_genl.c */
int an7552_mac_genl_init(struct an7552_mac *x);
void an7552_mac_genl_exit(void);
void an7552_mac_genl_omci_rx(struct an7552_mac *x, struct sk_buff *skb);
void an7552_mac_genl_state(struct an7552_mac *x);
void an7552_mac_genl_ploam_unhandled(struct an7552_mac *x, const u8 *msg);
void an7552_mac_genl_tod(struct an7552_mac *x, u32 sec, u32 nsec);

/* an7552_led.c */
int an7552_led_init(struct an7552_mac *x);
void an7552_led_exit(struct an7552_mac *x);
void an7552_led_set(struct an7552_led *l, enum an7552_led_mode mode);
void an7552_led_gpon_state(struct an7552_mac *x);

/* the private data of pon0 is the driver it belongs to */
#define ndev_to_xpon(_ndev)	(*(struct an7552_mac **)netdev_priv(_ndev))

/* an7552_gpon_debugfs.c */
void an7552_mac_debugfs_init(struct an7552_mac *x);
void an7552_mac_debugfs_exit(struct an7552_mac *x);

#endif /* _AN7552_XPON_H */
