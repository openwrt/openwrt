/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Airoha AN7552/AN7563 xPON - PON SerDes PHY layer
 *
 * Half of the an7552-gpon driver: the xPON SerDes, the optical front end
 * line and the PHY events.  The MAC half (an7552_mac.h) drives it through
 * the functions below; both live in one module, so there is no exported
 * API and no "struct phy" indirection in between - the MAC holds a
 * struct an7552_phy * directly.
 *
 * Locking, in one place so it does not have to be repeated at every call:
 *
 *   priv->lock       register lock (the vendor gpPhyPriv+0x7c spinlock).
 *                    Taken with interrupts disabled around every
 *                    read-modify-write of the csr/ana/pma windows.
 *   priv->op_lock    serialises the sleeping entry points that reprogram
 *                    the PHY (reset, mode change, power).
 *
 * Functions marked [lock] must be called with priv->lock held; the rest
 * take it themselves.  Functions marked [sleep] take priv->op_lock as well
 * and must be called from process context.
 */

#ifndef _AN7552_PHY_H
#define _AN7552_PHY_H

#include <linux/bitfield.h>
#include <linux/dcache.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/reset.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/workqueue.h>

#include "an7552_hw.h"
#include <linux/pon_laser.h>

#include "uapi/gpon_genl.h"

struct an7552_mac;
struct en7571;

/* Driver name: the bus, the reset line and the log */
#define AN7552_DRV_NAME			"an7552-gpon"

/*
 * The driver carries its own numbering, which is not the vendor's:
 * GPON_MODE_GPON is 0 here, while in Mode_Config_7552() - where the
 * SerDes mode is read as (gpPhyPriv[0x58] & 0x7f) >> 3 - 0 is EPON and GPON
 * is 1 (phy.ann:0x1bb0, 0x1cf4).  So a property written in the vendor's numbers
 * lands on the wrong mode: 1 is not a mode this enum has, and probe rejects
 * the platform device with -EOPNOTSUPP.
 *
 * Two modes, and no third: this is a GPON driver.  The vendor's EPON (14) has
 * no arm here - an7552_phy_main.c answers it with -EOPNOTSUPP before any of it
 * could run - so it is not an enum value to carry around.  GPON_MODE_EPON
 * still exists in the uapi header, because that header is shared with the
 * optical front end, which knows the mode as a number and maps it on its own
 * (an7552_laser_mode_of()).  Keeping the value there and dropping it here is
 * what keeps the two sides from implying that either of them supports it.
 *
 * The vendor's Mode_Config_7552() also distinguishes a mode 11, grouping it
 * with 1 (it compares against 1, 10, 11 and 0, four times over:
 * phy.ann:0x1b6c-0x1bb4 and again at 0x1cb4, 0x1ef0, 0x1fe8).  Nothing
 * reaches here: probe admits only GPON and GPON_SYM and answers anything else
 * with -EOPNOTSUPP (an7552_phy_main.c), the same way it answers the vendor's
 * own 1.  So no constant for it - it is a fourth SerDes configuration with no
 * driver behind it, not an omission.
 */
enum an7552_xpon_mode {
	AN7552_XPON_GPON		= GPON_MODE_GPON,
	AN7552_XPON_GPON_SYM		= GPON_MODE_GPON_SYM,
};

/* Events reported to the MAC, decoded from PHY_CSR_XPON_INT_STA. */
enum an7552_xpon_event {
	AN7552_EVENT_LOS		= 0,	/* TRANS_LOS (bit 0) */
	AN7552_EVENT_LOF		= 1,	/* PHY_LOF (bit 1) */
	AN7552_EVENT_PHY_RDY		= 2,	/* PHYRDY (bit 5) without LOS/ILLG */
	AN7552_EVENT_TX_FAULT		= 3,	/* TRANS_TF (bit 2), tx_fault not inverted */
	AN7552_EVENT_SD_FAIL		= 4,	/* TRANS_SD_FAIL (bit 4) */
	AN7552_EVENT_I2C_ERR		= 5,	/* PHY_I2CM (bit 7) */
	AN7552_EVENT_ILLG		= 6,	/* PHY_ILLG (bit 6) */
	AN7552_EVENT_TRANS_INT		= 7,	/* TRANS_INT (bit 3) */
	AN7552_EVENT_ROGUE		= 8,	/* rogue ONU detected, TX off */
};

/* Vendor phy_tx_Mode() values; the PRBS modes 0-3 are not supported */
enum an7552_xpon_tx_mode {
	AN7552_TX_MODE_PRBS7		= 0,
	AN7552_TX_MODE_PRBS15		= 1,
	AN7552_TX_MODE_PRBS23		= 2,
	AN7552_TX_MODE_PRBS31		= 3,
	AN7552_TX_MODE_NORMAL		= 4,	/* BEN from the MAC, TX on */
	AN7552_TX_MODE_CONT		= 5,	/* BEN forced high, TX on */
	AN7552_TX_MODE_OFF		= 6,	/* BEN from the MAC, TX off */
};

/* PHY_Event ids (phy_isr -> phy_event_handler) */
enum {
	AN7552_PHYEV_LOS		= 0,
	AN7552_PHYEV_LOF		= 1,
	AN7552_PHYEV_ROGUE		= 2,
	AN7552_PHYEV_TRANS		= 3,
	AN7552_PHYEV_SD_FAIL		= 4,
	AN7552_PHYEV_READY		= 5,
	AN7552_PHYEV_ILLG		= 6,
	AN7552_PHYEV_I2C		= 7,
	AN7552_PHYEV_LOS_FALSE_READY	= 8,
	AN7552_PHYEV_NO_READY		= 9,	/* 0x107 in vendor PHY driver */
};

/* initial value of ncpo_online */
#define AN7552_NCPO_ONLINE_INIT		0x17e21965

/*
 * PHY timers: recover 5000 ms (50 ms after LOS), poll 1000 ms (20 ms
 * after LOF), plus the reset counters the ISR compares against.
 */
#define AN7552_PHY_RECOVER_MS		5000
#define AN7552_PHY_RECOVER_LOS_MS	50
#define AN7552_PHY_POLL_MS		1000
#define AN7552_PHY_POLL_LOF_MS		20
#define AN7552_PHY_POLL_SCU_RESET_MS	3500	/* 0xdac */
#define AN7552_PHY_NO_READY_RESET_CNT	10
#define AN7552_PHY_KBAND_RESET_CNT	9	/* reset count > 9 */

struct an7552_phy {
	struct device *dev;
	void __iomem *csr;		/* "csr", 0x1faf0000 */
	void __iomem *ana;		/* "ana", 0x1faf3000 */
	void __iomem *pma;		/* "pma", 0x1faf4000 */
	struct regmap *chip_scu;
	struct regmap *np_scu;
	struct reset_control *reset;

	spinlock_t lock;			/* register lock */
	struct mutex op_lock;		/* sleeping entry points */

	int mode;			/* Mode_Config_7552 mode number */
	bool configured;		/* pon_phy_scu_reset_init done */
	bool started;			/* xpon_phy_start, gpPhyPriv+0x5e */
	bool tx_power_en;		/* phy_tx_power_config flag */
	bool rogue_enabled;		/* debugfs phy/rogue_en, gpPhyPriv+0xbc */
	bool traffic_up;		/* gpPhyPriv+0x58 bit 2 */
	/*
	 * The optical front end, as the two addresses the MAC reaches it by.
	 * There is one front end and one MAC, so there is nothing to look up and
	 * no handle to carry: NULL means there is none, and that is all the MAC
	 * has to check.  Set together by an7552_laser_attached() and cleared
	 * together by an7552_laser_detached().
	 */
	pon_laser_set_traffic_fn laser_set_traffic;
	pon_laser_safe_circuit_reset_fn laser_safe_circuit_reset;
	/*
	 * The same front end as a regulator: the transmitter line, with the
	 * usual regulator reference counting.  Set when a front end attaches.
	 */
	struct regulator *tx_rdev;
	bool trans_power_on;		/* phy_trans_power_switch state, +0xb4 */
	struct mutex tx_reg_lock;	/* tx_reg_on and the regulator calls */
	bool tx_reg_on;			/* our one reference on tx_rdev is taken */
	struct work_struct laser_work;	/* EN7571 safe circuit reset */
	u32 ncpo_online;		/* Mode_Config_7552 NCPO target */

	/* phy_isr / phy_event_handler state */
	bool ready_flag;		/* gpPhyPriv[0x20] */
	bool lof_pending;		/* gpPhyPriv+200 */
	bool lof_to_los;
	unsigned int phy_ready_cnt;
	unsigned int recover_resets;
	unsigned int no_los_no_ready_cnt;
	/*
	 * Recover re-arm period, gpPhyPriv[0]: 50 ms after a LOS, 5 s once a
	 * recover pass has run.  Zero means nothing has been seen yet and the
	 * default applies.
	 */
	unsigned int recover_ms;
	struct delayed_work poll_work;	/* phy_event_poll */

	/* Shadows written back by restore_preamble_guard */
	u32 preamble;
	u32 guard_delimiter;
	u32 ext_preamble;
	bool preamble_valid;
	bool ext_preamble_valid;

	/*
	 * gpPhyPriv+0xb0 "phy_xpon_trans_val": the PHY_CSR_XPON_SETTING image
	 * set by trans_model_setting.  Mode_Config_7552 derives rg_tdc_los_inv
	 * (bit 6, RX_SD_INV) and rg_ben_inv (bit 7, BURST_EN_INV) from it.
	 *
	 * Board wiring of this node, read at probe from airoha,rx-sd-inverted
	 * and its three siblings, not something the optical front end brings
	 * with it - the SerDes needs it before a front end is anywhere in sight.
	 * Valid is therefore set unconditionally there and only the netlink
	 * polarity setter can find it false.
	 */
	u32 xpon_trans_val;
	bool xpon_trans_val_valid;

	u8 digital_bit_delay;
	bool digital_bit_delay_valid;	/* set explicitly: restore it, 0 too */

	/* phy_tx_Mode() value, gpPhyPriv+0xb8 (4 = normal) */
	enum an7552_xpon_tx_mode tx_mode;
	/* DT airoha,tx-amplitude: ANATXREG1[19:16] written at each SCU reset */
	u8 tx_amp;
	bool tx_amp_valid;

	struct dentry *debugfs;

	/* Interrupts and events (pending_events/recover_* under priv->lock) */
	int irq;
	u32 pending_events;
	struct work_struct event_work;

	/* phy_ready_recover_expires */
	struct delayed_work recover_work;
	bool recover_enabled;

	/* The MAC half, notified directly from the event work */
	struct an7552_mac *mac;
};

/* ---- register accessors ---- */

/*
 * One read/write/read-modify-write triple per mapped window, from one macro:
 * the three windows differ only in the member they read through.
 */
#define AN7552_PHY_WINDOW(_p, _base)						\
static inline u32 _p##_read(struct an7552_phy *priv, u32 off)			\
{										\
	return readl(priv->_base + off);					\
}										\
										\
static inline void _p##_write(struct an7552_phy *priv, u32 off, u32 val)	\
{										\
	writel(val, priv->_base + off);					\
}										\
										\
static inline void _p##_rmw(struct an7552_phy *priv, u32 off, u32 clr,	\
			     u32 set)						\
{										\
	_p##_write(priv, off, (_p##_read(priv, off) & ~clr) | set);		\
}

AN7552_PHY_WINDOW(csr, csr)
AN7552_PHY_WINDOW(ana, ana)
AN7552_PHY_WINDOW(pma, pma)

/*
 * A read-modify-write that takes priv->lock for itself: for the sequences with
 * long delays in them (k band, the ready handler), which the vendor runs with
 * no lock at all and which must not keep the interrupts off across them.
 */
static inline void pma_rmw_irq(struct an7552_phy *priv, u32 off, u32 clr,
			       u32 set)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	pma_rmw(priv, off, clr, set);
	spin_unlock_irqrestore(&priv->lock, flags);
}

/* PHY ready / LOS; plain register reads, callers that already hold
 * priv->lock may call them from there. */
static inline bool an7552_phy_is_ready(struct an7552_phy *priv)
{
	return FIELD_GET(PHYSTA1_PHYRDY, csr_read(priv, PHY_CSR_PHYSTA1)) ==
	       PHYRDY_STATUS;
}

static inline bool an7552_phy_is_los(struct an7552_phy *priv)
{
	return csr_read(priv, PHY_CSR_XPON_STA) & XPON_STA_LOS;
}

static inline bool an7552_phy_is_gpon_hw(struct an7552_phy *priv)
{
	return csr_read(priv, PHY_CSR_PHYSET10) & PHY_GPON_MODE;
}

/* ---- software state, no register ---- */

static inline enum an7552_xpon_mode an7552_phy_get_mode(struct an7552_phy *priv)
{
	return READ_ONCE(priv->mode);
}

static inline enum an7552_xpon_tx_mode an7552_phy_get_tx_mode(struct an7552_phy *priv)
{
	return READ_ONCE(priv->tx_mode);
}

/* ---- an7552_phy_main.c: probe/remove of the PHY half of the device ---- */

int an7552_phy_probe(struct platform_device *pdev, struct an7552_phy *priv);
void an7552_phy_remove(struct an7552_phy *priv);

/* ---- an7552_phy_serdes.c: [lock] unless stated ---- */

void an7552_mode_config_7552(struct an7552_phy *priv, int mode);
void an7552_physet3_pulse_locked(struct an7552_phy *priv);
void an7552_logic_reset_locked(struct an7552_phy *priv);
void an7552_int_config_locked(struct an7552_phy *priv, u32 int_en, u32 pma_int_en);
void an7552_fw_ready_locked(struct an7552_phy *priv, bool ready);
void an7552_counter_clear_locked(struct an7552_phy *priv, u32 types);
void an7552_reset_counter_locked(struct an7552_phy *priv);

/* xPON_Pll_Reset(), also used by phy_write_proc */
int an7552_pll_reset(struct an7552_phy *priv);

int an7552_phy_reset(struct an7552_phy *priv);
void an7552_phy_dev_init(struct an7552_phy *priv, bool int_enable);
void an7552_k_band(struct an7552_phy *priv);

/* ---- an7552_phy_ops.c: power, mode, status, counters ---- */

/* PON_SET_PHY_SCU_RST: pon_phy_scu_reset_init() [sleep] */
int an7552_phy_scu_reset(struct an7552_phy *priv);
/* PON_SET_PHY_START / STOP: xpon_phy_start() / xpon_phy_stop() [sleep] */
void an7552_phy_start(struct an7552_phy *priv);
void an7552_phy_stop(struct an7552_phy *priv);
/* PON_SET_PHY_LOGIC_RESET: xPON_Logic_Reset(), called with the MAC reset held */
void an7552_phy_logic_reset(struct an7552_phy *priv);

/*
 * XPON_PHY_SET_MODE -> phy_mode_config(mode, tx_enable = 0): fw_ready 0,
 * Mode_Config_7552, fw_ready 1.  Before the first scu_reset the mode is
 * only stored.  The TX-disable GPIO switching of the vendor wrapper is not
 * done here (en7571 owns that line).  [sleep]
 */
int an7552_phy_set_mode(struct an7552_phy *priv, enum an7552_xpon_mode mode);

/* XPON_PHY_FW_READY_ENABLE/DISABLE: PHYSET2[0] */
void an7552_phy_fw_ready(struct an7552_phy *priv, bool enable);

/* Counters */
struct an7552_phy_stats {
	u64 rx_frames;			/* FRAME_CNT_H:FRAME_CNT_L */
	u32 lof_count;			/* read with PHYSET10 GPON_MODE set */
	u32 bip_errors;			/* read-to-clear, see bip_counter */
	u32 fec_corrected_bytes;
	u32 fec_corrected_codewords;
	u32 fec_uncorrectable_codewords;
	u32 fec_total_codewords;
	u32 fec_seconds;
	u32 tx_frames;
	u32 tx_bursts;
};
int an7552_phy_get_stats(struct an7552_phy *priv, struct an7552_phy_stats *stats);
/* PON_GET_PHY_BIP_COUNTER: latch, read, clear (read-to-clear) */
u32 an7552_phy_bip_counter(struct an7552_phy *priv);
/*
 * PON_GET_PHY_ROUND_TRIP_DELAY: the psync-to-SOF delay the SerDes measures,
 * i.e. how long the PON is.  The register only settles once the link is up,
 * so the read is repeated until three reads in a row agree (gpon.c:281-297,
 * mdelay(1) there, usleep_range() here); -EAGAIN when ten tries did not
 * settle, which is what an unaligned or dark link looks like.  [sleep]
 */
int an7552_phy_rtd(struct an7552_phy *priv, u32 *rtd);

/* Counter clear types (vendor ENUM_PhyCounterClr), may be OR'ed. */
#define AN7552_PHY_ERR_CNT_CLR		BIT(0)
#define AN7552_PHY_BIP_CNT_CLR		BIT(1)
#define AN7552_PHY_RXFRAME_CNT_CLR	BIT(2)
#define AN7552_PHY_TXFRAME_CNT_CLR	BIT(3)
/* XPON_PHY_COUNTER_CLEAR */
int an7552_phy_counter_clear(struct an7552_phy *priv, u32 counter_type);

/* ---- an7552_phy_tx.c: transmit side, preamble and transceiver line ---- */

/* Vendor PHY_GponPreb_T and ENUM_GponPreb_T mask bits */
#define PHY_GUARD_BIT_NUM_EN		BIT(0)
#define PHY_PRE_T1_NUM_EN		BIT(1)
#define PHY_PRE_T2_NUM_EN		BIT(2)
#define PHY_PRE_T3_PAT_EN		BIT(3)
#define PHY_T3_O4_PRE_EN		BIT(4)
#define PHY_T3_O5_PRE_EN		BIT(5)
#define PHY_EXT_BUR_MODE_EN		BIT(6)
#define PHY_OPER_RANG_EN		BIT(7)
#define PHY_EXTB_LENG_SEL_EN		BIT(8)
#define PHY_DIS_SCRAM_EN		BIT(9)

struct an7552_phy_preamble {
	u32 mask;			/* PHY_*_EN above */
	u8 guard_bit_num;
	u8 preamble_t1_num;
	u8 preamble_t2_num;
	u8 preamble_t3_pat;
	u8 t3_o4_preamble;
	u8 t3_o5_preamble;
	u8 extend_burst_mode;		/* 0/1 */
	u8 oper_ranged_st;		/* 2 bits: 2 = O3/O4, 3 = O5 */
	u8 extb_length_sel;		/* 0/1 */
	u8 dis_scramble;		/* 0/1 */
};

/* PON_SET_PHY_GPON_PREAMBLE: mask bits 0-3, GPON_PREAMBLE */
int an7552_phy_gpon_preamble(struct an7552_phy *priv,
			     const struct an7552_phy_preamble *p);
/* PON_SET_PHY_GPON_EXTEND_PREAMBLE: mask bits 4-9 */
int an7552_phy_gpon_extend_preamble(struct an7552_phy *priv,
				    const struct an7552_phy_preamble *p);
/*
 * PON_SET_PHY_GPON_DELIMITER_GUARD: delimiter[23:0], guard[31:24].
 * -EINVAL when delimiter has bits above 23 set.
 */
int an7552_phy_gpon_delimiter_guard(struct an7552_phy *priv, u32 delimiter,
				    u8 guard_time);

/* XPON_PHY_SET_BIT_DELAY: TX_CTRL_0[7:0], delay 0..7 */
int an7552_phy_set_bit_delay(struct an7552_phy *priv, u32 delay);

/*
 * Transceiver pin polarity, vendor PHY_TransConfig_T plus rx_sd_inv
 * (PON_SET_PHY_TRANS_TX/RX_SETTING, PON_GET 0x800a).  Flags are "inverted"
 * (low active).  The SerDes copies of RX_SD_INV and BURST_EN_INV follow.
 */
#define AN7552_TRANS_TX_SD_INV		BIT(0)
#define AN7552_TRANS_BURST_EN_INV	BIT(1)
#define AN7552_TRANS_TX_FAULT_INV	BIT(2)
#define AN7552_TRANS_RX_SD_INV		BIT(3)
#define AN7552_TRANS_INV_ALL		GENMASK(3, 0)
/* change the flags in @mask to the ones in @val; -EINVAL on unknown bits */
int an7552_phy_set_trans_inv(struct an7552_phy *priv, u32 mask, u32 val);
u32 an7552_phy_get_trans_inv(struct an7552_phy *priv);

/*
 * PON_SET_PHY 0xd / PON_GET 0x8009, phy_tx_burst_config(): continuous
 * (true) or burst (false) transmission, PHYSET3 bits 7 and 5.  Back to
 * burst mode also sets the tx_power flag and switches the transmitter on.
 */
int an7552_phy_set_tx_cont(struct an7552_phy *priv, bool cont);
/* PON_GET_PHY 0x8009: phy_tx_burst_getting() */
static inline bool an7552_phy_get_tx_cont(struct an7552_phy *priv)
{
	return csr_read(priv, PHY_CSR_PHYSET3) & PHY_TX_CONT_MODE;
}

/* phy_ben_switch(): force burst enable high in the SerDes */
int an7552_phy_ben_force(struct an7552_phy *priv, bool on);
bool an7552_phy_ben_forced(struct an7552_phy *priv);

/* PON_SET_PHY 0x24 / PON_GET 0x8025; -EOPNOTSUPP for the PRBS modes */
int an7552_phy_set_tx_mode(struct an7552_phy *priv, enum an7552_xpon_tx_mode mode);

/*
 * phy_tx_ctl(): drive the EN7571 TX-disable line directly, without the
 * bookkeeping of an7552_trans_power_switch().  -ENODEV without EN7571.
 */
int an7552_phy_tx_ctl(struct an7552_phy *priv, bool on);
bool an7552_phy_tx_ctl_get(struct an7552_phy *priv);

/*
 * Give back the transmitter regulator reference taken in an7552_laser_setup().
 * Called from an7552_laser_detached(), and it is what makes the front end
 * module unloadable once it has unbound.
 */
void an7552_tx_reg_put(struct an7552_phy *priv);

/* phy_tx_amp_setting(): TX swing 0..15, ANATXREG1[19:16] */
int an7552_phy_set_tx_amp(struct an7552_phy *priv, u8 level);
/* no vendor getter; reads back the field the setter writes */
static inline u8 an7552_phy_get_tx_amp(struct an7552_phy *priv)
{
	return FIELD_GET(PHY_TX_AMP_LEVEL,
			 csr_read(priv, PHY_CSR_ANATXREG1));
}

/* phy_tx_fault_reset(): EN7571 safe circuit reset; -ENODEV without EN7571 */
int an7552_phy_tx_fault_reset(struct an7552_phy *priv);

/*
 * phy_trans_power_switch(): switches the transmitter of the optical front
 * end (EN7571 tx-disable line); "on" only takes effect while the tx_power
 * flag is set.
 */
void an7552_trans_power_switch(struct an7552_phy *priv, bool on);
/*
 * pon_phy_rogueonu_int_en(): rogue ONU detection.  A setting, off by
 * default: debugfs phy/rogue_en.
 * While it is on, a rogue event switches the transmitter off.
 */
int an7552_phy_rogue_en(struct an7552_phy *priv, bool on);
static inline bool an7552_phy_rogue_get(struct an7552_phy *priv)
{
	return READ_ONCE(priv->rogue_enabled);
}
/* XPON_PHY_TX_POWER_CONFIG -> phy_tx_power_config(): software flag only */
void an7552_phy_tx_power(struct an7552_phy *priv, bool on);

/* ---- an7552_laser.c: the optical front end, registry and traffic channel --
 *
 * an7552_laser_attached()/an7552_laser_detached() are the binding and are
 * private to that file; nothing outside it has a use for them.  What the MAC
 * and the PHY call from there is this:
 */

/* Announce this core to the registry; binds a front end that got here first */
void an7552_laser_core_ready(struct an7552_mac *mac);
/* Take the core back out of the registry as the platform device goes away */
void an7552_laser_core_gone(struct an7552_mac *mac);
/*
 * PON_SET_PHY traffic status (0x1b, phy_set_traffic_status()): the MAC
 * reports whether user traffic flows; the optical front end loops
 * (EN7571 SWKT/OPKT/Pav) depend on it.
 */
int an7552_phy_set_traffic(struct an7552_phy *priv, bool up);
/* Hand the front end a mode on its own, with no traffic; see an7552_laser.c */
int an7552_phy_laser_mode(struct an7552_phy *priv, enum an7552_xpon_mode mode);
/* work_mt7571_safe_circuit_reset, queued by an7552_trans_power_switch(1) */
void an7552_laser_work(struct work_struct *work);

/* ---- an7552_phy_irq.c ---- */

irqreturn_t an7552_phy_irq_handler(int irq, void *dev_id);
void an7552_phy_event_work(struct work_struct *work);
void an7552_phy_recover_work(struct work_struct *work);
void an7552_phy_poll_work(struct work_struct *work);
void an7552_phy_recover_start_locked(struct an7552_phy *priv);
void an7552_phy_recover_start(struct an7552_phy *priv);
void an7552_phy_poll_start_locked(struct an7552_phy *priv, unsigned int ms);
void an7552_phy_poll_start(struct an7552_phy *priv, unsigned int ms);

/* ---- an7552_phy_debugfs.c ---- */

void an7552_phy_debugfs_create(struct dentry *dir, struct an7552_phy *priv);
/* the debugfs "status" values; @m NULL: one kernel log line each */
void an7552_phy_dump(struct an7552_phy *priv, struct seq_file *m);

/* module parameter "debug": bit 0 PHY events, bit 1 + register dump */
extern unsigned int an7552_phy_debug;
#define AN7552_DBG_EVENTS	BIT(0)	/* PHY events, recover work, MAC notify */
#define AN7552_DBG_REGS		BIT(1)	/* + the register dump at every event */
#define AN7552_DBG_MAC		BIT(2)	/* MAC: activation, PLOAM, counters */
#define an7552_phy_dbg(_p, _bit, _fmt, ...)				\
	do {								\
		if (an7552_phy_debug & (_bit))			\
			dev_info((_p)->dev, "phy: " _fmt, ##__VA_ARGS__);\
	} while (0)

#endif /* _AN7552_PHY_H */
