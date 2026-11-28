// SPDX-License-Identifier: GPL-2.0
/*
 * AN7552 xPON - PHY bring-up, mode switching, status and counters
 *
 * The MAC half of this module drives the PHY through these functions
 * directly; there is no exported API and no phy_ops, so each entry point
 * takes the op_lock itself and the *_locked helpers below assume it.
 */

#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/string.h>

#include "an7552_phy.h"

/*
 * PON_SET_PHY_SCU_RST: pon_phy_scu_reset_init().
 *
 *   fw_ready(0), remember and switch off the transmitter, stop polling,
 *   TDC_HOLD_CTRL_0[16] = 0, PHY reset pulse (SCU RST_CTRL2 bit 0),
 *   Mode_Config_7552, PHYSET3 PLL/count reset pulse, phy_dev_init steps,
 *   TDCSET2 = a_tdc_setting (2), XPON_SETTING = trans_val, k band,
 *   restore the transmitter, restart polling when started.
 */
int an7552_phy_scu_reset(struct an7552_phy *priv)
{
	bool tx_on = priv->trans_power_on;
	unsigned long flags;
	int ret;

	/*
	 * pon_phy_scu_reset_init() stops the poll event with del_timer_sync()
	 * when a foreign context resets the PHY, and with plain del_timer()
	 * when its own 1 s poll runs the reset - a sync wait on our own
	 * running work would wait for itself.  The handlers
	 * run in event_work here, so the sync branch is the normal one; the
	 * current_work() test keeps a future straight-from-the-poll-body call
	 * deadlock-free.  The flush sits BEFORE op_lock: an in-flight poll
	 * instance can be on its way into the no-LOS/no-ready reset, which
	 * waits for exactly this mutex.
	 */
	if (current_work() == &priv->poll_work.work)
		cancel_delayed_work(&priv->poll_work);
	else
		cancel_delayed_work_sync(&priv->poll_work);
	/*
	 * The recover pass (PHYSET3 pulse, k band) must not run into the
	 * reset either; it takes op_lock, so it is flushed before it as well
	 * and re-armed at the end.
	 */
	if (current_work() == &priv->recover_work.work)
		cancel_delayed_work(&priv->recover_work);
	else
		cancel_delayed_work_sync(&priv->recover_work);

	/* [sleep]: serialises against set_mode() and set_tx_mode() */
	guard(mutex)(&priv->op_lock);

	spin_lock_irqsave(&priv->lock, flags);
	an7552_fw_ready_locked(priv, false);
	spin_unlock_irqrestore(&priv->lock, flags);
	an7552_trans_power_switch(priv, false);

	spin_lock_irqsave(&priv->lock, flags);
	pma_rmw(priv, PMA_TDC_HOLD_CTRL_0, RG_TDC_LOS_HOLD_MUX, 0);
	spin_unlock_irqrestore(&priv->lock, flags);

	/*
	 * SCU RST_CTRL2 bit 0 pulse, 1 ms settle, then Mode_Config_7552.
	 * This is xPON_Phy_Reset plus the 1 ms settle, which only
	 * pon_phy_scu_reset_init has.
	 */
	ret = reset_control_assert(priv->reset);
	if (!ret) {
		udelay(AN7552_PHY_RESET_DELAY_US);
		ret = reset_control_deassert(priv->reset);
	}
	if (ret) {
		dev_err(priv->dev, "PHY reset failed: %d\n", ret);
		return ret;
	}
	fsleep(AN7552_SCU_RESET_SETTLE_MS * USEC_PER_MSEC);
	spin_lock_irqsave(&priv->lock, flags);
	an7552_mode_config_7552(priv, priv->mode);
	/*
	 * A bare PHYSET3 PLL/count pulse.  Unlike xPON_Pll_Reset it leaves
	 * rg_tdc_los_hold_mux as Mode_Config_7552 set it.
	 */
	an7552_physet3_pulse_locked(priv);
	spin_unlock_irqrestore(&priv->lock, flags);
	an7552_phy_dev_init(priv, priv->started);

	spin_lock_irqsave(&priv->lock, flags);
	csr_write(priv, PHY_CSR_TDCSET2, TDCSET2_DEFAULT);
	if (priv->xpon_trans_val_valid)
		csr_write(priv, PHY_CSR_XPON_SETTING, priv->xpon_trans_val);
	/* Board overrides from DT, written only when supplied */
	if (priv->tx_amp_valid)
		csr_rmw(priv, PHY_CSR_ANATXREG1, PHY_TX_AMP_LEVEL,
			FIELD_PREP(PHY_TX_AMP_LEVEL, priv->tx_amp));
	if (priv->preamble_valid)
		csr_write(priv, PHY_CSR_GPON_PREAMBLE, priv->preamble);
	if (priv->ext_preamble_valid)
		csr_write(priv, PHY_CSR_GPON_EXTEND_PREAMBLE, priv->ext_preamble);
	if (priv->digital_bit_delay_valid)
		pma_rmw(priv, PMA_TX_CTRL_0, RG_TX_DLY_DATA_FTUNE_LOW,
			priv->digital_bit_delay);
	priv->phy_ready_cnt = 0;
	spin_unlock_irqrestore(&priv->lock, flags);

	an7552_k_band(priv);
	an7552_trans_power_switch(priv, tx_on);

	priv->configured = true;
	if (priv->started)
		an7552_phy_poll_start(priv, 1000);
	/* only while recover_enabled, i.e. the PHY runs */
	an7552_phy_recover_start(priv);

	return 0;
}

/*
 * PON_SET_PHY_START: xpon_phy_start() - clear pending status, clear the PMA
 * interrupts, phy_int_config(0x63, 0), start polling and recovery.  FW
 * ready is left to the MAC.
 */
void an7552_phy_start(struct an7552_phy *priv)
{
	unsigned long flags;

	guard(mutex)(&priv->op_lock);
	if (!priv->configured)
		return;

	priv->started = true;
	spin_lock_irqsave(&priv->lock, flags);
	csr_write(priv, PHY_CSR_XPON_INT_STA_CLR, PHY_INT_FIELD);
	pma_write(priv, PMA_PON_INTERRUPT_CLR_0, PMA_INT_CLR_ALL);
	an7552_int_config_locked(priv, PHY_INT_VENDOR_MASK, 0);
	priv->recover_enabled = true;
	/* no LOS seen yet, so the default period; a stop below clears it */
	priv->recover_ms = 0;
	priv->recover_resets = 0;
	an7552_phy_poll_start_locked(priv, 1000);
	an7552_phy_recover_start_locked(priv);
	spin_unlock_irqrestore(&priv->lock, flags);
}

/* PON_SET_PHY_STOP: xpon_phy_stop() - mask, fw_ready(0), stop the timers */
void an7552_phy_stop(struct an7552_phy *priv)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	priv->recover_enabled = false;
	spin_unlock_irqrestore(&priv->lock, flags);
	/* the recover pass takes op_lock: flushed before it, it cannot re-arm */
	cancel_delayed_work_sync(&priv->recover_work);

	guard(mutex)(&priv->op_lock);
	cancel_delayed_work_sync(&priv->poll_work);

	spin_lock_irqsave(&priv->lock, flags);
	priv->recover_ms = 0;
	an7552_int_config_locked(priv, 0, 0);
	an7552_fw_ready_locked(priv, false);
	spin_unlock_irqrestore(&priv->lock, flags);

	priv->started = false;
}


/*
 * phy_mode_config(mode, tx_enable = 0), ASIC branch:
 * fw_ready(0); Mode_Config_7552 under the lock; fw_ready(1).  The
 * phy_trans_power_switch(0/1) calls only drive the TX
 * disable GPIO and are left to the MAC / en7571 driver.
 */
int an7552_phy_set_mode(struct an7552_phy *priv, enum an7552_xpon_mode mode)
{
	unsigned long flags;

	/*
	 * phy_mode_config / phy_mode_config_for_mac also take
	 * mode 0 (EPON), which this driver does not support and does not name.
	 * -EOPNOTSUPP rather than -EINVAL: the mode is one this driver cannot
	 * run, the same answer an7552_mac_set_mode() and airoha,pon-mode give.
	 */
	if (mode != AN7552_XPON_GPON && mode != AN7552_XPON_GPON_SYM)
		return -EOPNOTSUPP;

	mutex_lock(&priv->op_lock);
	priv->mode = mode;
	if (!priv->configured)
		goto out;

	spin_lock_irqsave(&priv->lock, flags);
	an7552_fw_ready_locked(priv, false);
	an7552_mode_config_7552(priv, mode);
	an7552_fw_ready_locked(priv, true);
	spin_unlock_irqrestore(&priv->lock, flags);
out:
	mutex_unlock(&priv->op_lock);

	/*
	 * The front end is told by the caller, outside its own lock: a mode that
	 * changes while traffic flows has to reach it as well, but making that
	 * call from here would reach the front end under whatever lock the
	 * caller holds.  an7552_mac_start() re-says it after it drops x->lock,
	 * and an7552_mac_set_mode() hands the mode over outside it too.
	 */
	return 0;
}

void an7552_phy_fw_ready(struct an7552_phy *priv, bool enable)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	an7552_fw_ready_locked(priv, enable);
	spin_unlock_irqrestore(&priv->lock, flags);
}

/*
 * PON_SET_PHY_LOGIC_RESET: xPON_Logic_Reset().  The MAC calls it
 * while it holds its own reset (EN7581_XPON_MAC_RST, RST_CTRL1 bit 31,
 * gpon_dev.c:3005-3016).
 */
void an7552_phy_logic_reset(struct an7552_phy *priv)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	an7552_logic_reset_locked(priv);
	spin_unlock_irqrestore(&priv->lock, flags);
}

/* ---- counters and statistics ---- */
/*
 * Counter read sequences are from the vendor phy_rx.c / phy_tx.c (A60928
 * build); clear sequences follow the vendor PHY driver.
 */
int an7552_phy_counter_clear(struct an7552_phy *priv, u32 counter_type)
{
	unsigned long flags;

	if (counter_type & ~(AN7552_PHY_ERR_CNT_CLR | AN7552_PHY_BIP_CNT_CLR |
			     AN7552_PHY_RXFRAME_CNT_CLR | AN7552_PHY_TXFRAME_CNT_CLR))
		return -EINVAL;

	spin_lock_irqsave(&priv->lock, flags);
	an7552_counter_clear_locked(priv, counter_type);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* phy_bip_counter() (phy_rx.c:57-68): latch, read, clear */
static u32 an7552_phy_bip_counter_locked(struct an7552_phy *priv)
{
	u32 val;

	csr_write(priv, PHY_CSR_XP_ERRCNT_CTL, PHY_BIP_LATCH);
	val = csr_read(priv, PHY_CSR_BIP_CNT);
	csr_write(priv, PHY_CSR_XP_ERRCNT_CTL, PHY_BIP_CLR);

	return val;
}

u32 an7552_phy_bip_counter(struct an7552_phy *priv)
{
	unsigned long flags;
	u32 val;

	spin_lock_irqsave(&priv->lock, flags);
	val = an7552_phy_bip_counter_locked(priv);
	spin_unlock_irqrestore(&priv->lock, flags);

	return val;
}

int an7552_phy_get_stats(struct an7552_phy *priv, struct an7552_phy_stats *stats)
{
	unsigned long flags;
	u32 hi, lo;

	if (!stats)
		return -EINVAL;

	memset(stats, 0, sizeof(*stats));

	spin_lock_irqsave(&priv->lock, flags);

	/* phy_rx_fec_counter() (phy_rx.c:28-35) */
	csr_write(priv, PHY_CSR_XP_ERRCNT_CTL, PHY_ERR_LATCH);
	stats->fec_corrected_bytes = csr_read(priv, PHY_CSR_ERR_BYTE_CNT);
	stats->fec_corrected_codewords = csr_read(priv, PHY_CSR_ERR_CODE_CNT);
	stats->fec_uncorrectable_codewords = csr_read(priv, PHY_CSR_NOSOL_CODE_CNT);
	stats->fec_total_codewords = csr_read(priv, PHY_CSR_RX_CODE_CNT);
	stats->fec_seconds = csr_read(priv, PHY_CSR_FEC_SECONDS);

	stats->bip_errors = an7552_phy_bip_counter_locked(priv);

	/* phy_rx_frame_counter() (phy_rx.c:83-89) */
	csr_write(priv, PHY_CSR_XP_ERRCNT_CTL, PHY_RXFRAME_LATCH);
	hi = csr_read(priv, PHY_CSR_FRAME_CNT_H);
	lo = csr_read(priv, PHY_CSR_FRAME_CNT_L);
	stats->rx_frames = ((u64)hi << 32) | lo;
	if (an7552_phy_is_gpon_hw(priv))
		stats->lof_count = csr_read(priv, PHY_CSR_LOF_CNT);

	/* phy_tx_frame_counter() / phy_tx_burst_counter() (phy_tx.c:560-599) */
	csr_rmw(priv, PHY_CSR_TX_TEST_TRIG, 0, PHY_TX_CNT_LCH);
	stats->tx_frames = csr_read(priv, PHY_CSR_TX_FRAME_COUNTER);
	csr_rmw(priv, PHY_CSR_TX_TEST_TRIG, 0, PHY_TX_CNT_LCH);
	stats->tx_bursts = csr_read(priv, PHY_CSR_TX_BURST_COUNTER);

	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

int an7552_phy_rtd(struct an7552_phy *priv, u32 *rtd)
{
	unsigned long flags;

	if (!rtd)
		return -EINVAL;

	/*
	 * One read, under the lock, which is what both references do:
	 * phy_round_trip_delay_sof() in the stock module is eighteen
	 * instructions with no branch at all (xpon.ann:0x3be4), and the SDK
	 * source it came from is a plain read of PHY_CSR_ROUND_TRIP_DELAY
	 * masked to 0xffff, with no caller that retries
	 * (xpon_phy/src/phy.c:446, called once from phy_init.c:737).
	 *
	 * An earlier version read the register ten times a millisecond apart
	 * and wanted three equal readings in a row.  It was dropped, for two
	 * reasons, the second of which is the one that matters: the claim it
	 * rested on cited gpon.c:281-297, which is gpon_detect_los_lof() and
	 * says nothing about this measurement; and the three-equal test was
	 * weaker than what it replaced, because it had to drop priv->lock
	 * between the readings it compared, so agreement between them said
	 * nothing about the register having settled.  A single read under the
	 * lock is the stronger guarantee, not the weaker one.
	 */
	spin_lock_irqsave(&priv->lock, flags);
	*rtd = csr_read(priv, PHY_CSR_ROUND_TRIP_DELAY) & RTD_MASK;
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}
