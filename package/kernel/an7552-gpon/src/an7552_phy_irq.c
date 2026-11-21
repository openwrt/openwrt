// SPDX-License-Identifier: GPL-2.0
/*
 * AN7552 xPON PHY - hard interrupt, event classification and the work that
 * runs the event handlers.
 *
 * Port of the vendor PHY interrupt handling: phy_isr(),
 * phy_event_handler(), phy_event_poll(), phy_los_handler(),
 * phy_ready_handler() and phy_ready_recover_expires().
 * Event numbers are the PHY_Event ids:
 *   0 LOS, 1 LOF, 2 rogue, 3 transceiver, 4 TX_SD fail, 5 ready,
 *   6 illegal, 7 I2C master, 8 LOS with a false ready, 0x107 no-LOS/no-ready
 * The hard interrupt only acknowledges and classifies; every handler that
 * sleeps or busy-waits runs from the event work, the vendor runs them in
 * interrupt context with mdelay().  The MAC event callback registration
 * lives in an7552_phy_ops.c, the work scheduling here with the handlers it
 * feeds.
 */

#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>

#include "an7552_mac.h"
#include "an7552_phy.h"

/* order in which phy_isr() raises the events of one interrupt */
static const u8 an7552_ev_order[] = {
	AN7552_PHYEV_LOS, AN7552_PHYEV_ILLG, AN7552_PHYEV_LOS_FALSE_READY,
	AN7552_PHYEV_LOF, AN7552_PHYEV_TRANS, AN7552_PHYEV_SD_FAIL,
	AN7552_PHYEV_READY, AN7552_PHYEV_I2C, AN7552_PHYEV_ROGUE,
};

/* ---- timers ---- */

/* both timers, the same rule: armed only while the PHY runs ([lock]) */
static void an7552_start_ms_locked(struct an7552_phy *priv,
				   struct delayed_work *work, unsigned int ms)
{
	if (priv->recover_enabled)
		mod_delayed_work(system_wq, work, msecs_to_jiffies(ms));
}

void an7552_phy_recover_start_locked(struct an7552_phy *priv)
{
	/*
	 * gpPhyPriv[0] is both the recover period and the LOS debounce, in
	 * units of 10 ms: phy_isr writes 50 on a LOS (phy.ann:0x22458) and
	 * every pass of phy_ready_recover_expires writes 5000 back
	 * (phy.ann:0x21434), then re-arms with priv[0]/10 jiffies
	 * (phy.ann:0x2148c-0x214b8).  So the period is 50 ms while a LOS is
	 * fresh and 5 s once a recover pass has run - not a fixed 5 s, which is
	 * what this used to do on the two paths that are not the LOS path
	 * itself: a SIGDET on a link that was up, and the poller noticing a
	 * ready that never came back.
	 */
	an7552_start_ms_locked(priv, &priv->recover_work,
			       priv->recover_ms ? priv->recover_ms :
						  AN7552_PHY_RECOVER_MS);
}

void an7552_phy_recover_start(struct an7552_phy *priv)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	an7552_phy_recover_start_locked(priv);
	spin_unlock_irqrestore(&priv->lock, flags);
}

void an7552_phy_poll_start_locked(struct an7552_phy *priv, unsigned int ms)
{
	an7552_start_ms_locked(priv, &priv->poll_work, ms);
}

void an7552_phy_poll_start(struct an7552_phy *priv, unsigned int ms)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	an7552_phy_poll_start_locked(priv, ms);
	spin_unlock_irqrestore(&priv->lock, flags);
}

/* ---- MAC event delivery ---- */

static void an7552_mac_notify(struct an7552_phy *priv,
			      enum an7552_xpon_event ev)
{
	an7552_phy_dbg(priv, AN7552_DBG_EVENTS, "notify MAC event %d%s\n", ev,
		   priv->mac ? "" : " (no MAC)");
	if (priv->mac)
		an7552_mac_phy_event(priv->mac, ev);
}

/* [lock] caller holds priv->lock */
static void an7552_raise(struct an7552_phy *priv, unsigned int id)
{
	priv->pending_events |= BIT(id);
	schedule_work(&priv->event_work);
}

/* ---- status helpers (phy_ready_status / phy_los_status) ---- */

/* both are called with priv->lock held */
static bool an7552_ready_no_los(struct an7552_phy *priv)
{
	return an7552_phy_is_ready(priv) && !an7552_phy_is_los(priv);
}

static bool an7552_no_ready_no_los(struct an7552_phy *priv)
{
	return !an7552_phy_is_ready(priv) && !an7552_phy_is_los(priv);
}

/* ---- handlers ---- */

/*
 * phy_los_handler(): the read of XPON_INT_STA below does not see the
 * interrupt that brought us here.
 *
 * XPON_INT_STA is W1C through XPON_INT_STA_CLR, and an7552_phy_irq_handler()
 * already cleared it - with the status it had read - before it raised the
 * event.  This handler runs from an7552_phy_event_work, so by now the
 * register reads zero and lof_to_los is never set.  The stock module has
 * the same shape: phy_isr clears 0x5f8 through 0x5f4 at xpon.ann:0x2201c-
 * 0x2203c, and all eleven of its calls into phy_event_handler - which is
 * where phy_los_handler is called from - sit above that, at 0x223a4 and
 * later.  It reaches the handler synchronously from the interrupt, where
 * this driver deliberately does not, because an7552_ready_handler() below
 * has nine milliseconds of delays in it.
 *
 * What is left of the test is a late interrupt: anything arriving between
 * the clear in the handler and the read here.  A LOF that arrived first and
 * is followed by a LOS will therefore be reported as the LOS.  Kept as it
 * is because the vendor behaves this way and the MAC side already has the
 * two flags to tell the cases apart; kept honest here because the comment
 * it replaced claimed the opposite, that a LOF turning out to be a LOS is
 * reported as a LOS, which nothing here does.
 */
static void an7552_los_handler(struct an7552_phy *priv)
{
	unsigned long flags;
	u32 status;

	spin_lock_irqsave(&priv->lock, flags);
	status = csr_read(priv, PHY_CSR_XPON_INT_STA);
	if (priv->lof_pending && (status & PHY_INT_TRANS_LOS))
		priv->lof_to_los = true;
	csr_write(priv, PHY_CSR_XPON_INT_STA_CLR, status);
	pma_rmw(priv, PMA_TX_CTRL_0, RG_TX_DLY_DATA_FTUNE_LOW, 0); /* bit_delay(0) */
	an7552_reset_counter_locked(priv);
	spin_unlock_irqrestore(&priv->lock, flags);
}

/* phy_ready_handler(): restart the LCPLL TDC, then the TX path */
static void an7552_ready_handler(struct an7552_phy *priv)
{
	u32 st;
	int i;

	/*
	 * About 9 ms of delays: no spinlock across them (the vendor takes none,
	 * and the interrupts would be off for the whole of it), so they sleep.
	 * Each register change is atomic on its own; the caller holds op_lock
	 * against a reset or a mode change in between.
	 */
	for (i = 1; ; i++) {
		pma_rmw_irq(priv, PMA_SS_LCPLL_TDC_PW_0, RG_LCPLL_TDC_DIG_PWDB, 0);
		udelay(1);
		pma_rmw_irq(priv, PMA_SS_LCPLL_TDC_PW_0, 0, RG_LCPLL_TDC_DIG_PWDB);
		fsleep(1000);
		st = FIELD_GET(RO_LCPLL_TDC_TDCPW_ST,
			       pma_read(priv, PMA_SS_LCPLL_TDC_RO_1));
		if (i > 6) {
			dev_warn_ratelimited(priv->dev, "TDC reset failed\n");
			break;
		}
		if (st > 10 || READ_ONCE(priv->xpon_trans_val) == 0xff)
			break;
	}
	fsleep(3000);

	pma_rmw_irq(priv, PMA_PON_SYS_CTRL_1, RG_TX_RESET_N, 0);
	udelay(1);
	pma_rmw_irq(priv, PMA_PON_SYS_CTRL_1, 0, RG_TX_RESET_N);
	fsleep(1000);
}

/*
 * no-LOS/no-ready seen by polling: after ten in a row the vendor runs a
 * full pon_phy_scu_reset_init() and polls again 3.5 s later.
 */
static void an7552_no_ready_handler(struct an7552_phy *priv)
{
	if (++priv->no_los_no_ready_cnt != AN7552_PHY_NO_READY_RESET_CNT)
		return;

	an7552_phy_scu_reset(priv);
	an7552_phy_fw_ready(priv, true);
	an7552_phy_poll_start(priv, AN7552_PHY_POLL_SCU_RESET_MS);
}

/* phy_event_handler() + notification of the MAC */
static void an7552_handle_event(struct an7552_phy *priv, unsigned int id)
{
	enum an7552_xpon_event mac_ev;

	switch (id) {
	case AN7552_PHYEV_LOS:
	case AN7552_PHYEV_LOF:
	case AN7552_PHYEV_ILLG:
	case AN7552_PHYEV_LOS_FALSE_READY:
		priv->no_los_no_ready_cnt = 0;
		priv->lof_pending = id == AN7552_PHYEV_LOF;
		priv->lof_to_los = false;
		an7552_los_handler(priv);
		if (id == AN7552_PHYEV_LOF && !priv->lof_to_los)
			mac_ev = AN7552_EVENT_LOF;
		else if (id == AN7552_PHYEV_ILLG)
			mac_ev = AN7552_EVENT_ILLG;
		else
			mac_ev = AN7552_EVENT_LOS;
		priv->lof_pending = false;
		break;
	case AN7552_PHYEV_READY:
		priv->no_los_no_ready_cnt = 0;
		priv->phy_ready_cnt++;
		mutex_lock(&priv->op_lock);
		an7552_ready_handler(priv);
		mutex_unlock(&priv->op_lock);
		mac_ev = AN7552_EVENT_PHY_RDY;
		break;
	case AN7552_PHYEV_TRANS:
		mac_ev = AN7552_EVENT_TRANS_INT;
		break;
	case AN7552_PHYEV_SD_FAIL:
		mac_ev = AN7552_EVENT_SD_FAIL;
		break;
	case AN7552_PHYEV_I2C:
		mac_ev = AN7552_EVENT_I2C_ERR;
		break;
	case AN7552_PHYEV_ROGUE:
		/* against set_tx_mode() and a reset switching it back on */
		mutex_lock(&priv->op_lock);
		an7552_trans_power_switch(priv, false);
		mutex_unlock(&priv->op_lock);
		mac_ev = AN7552_EVENT_ROGUE;
		break;
	case AN7552_PHYEV_NO_READY:
		an7552_no_ready_handler(priv);
		return;
	default:
		return;
	}

	an7552_mac_notify(priv, mac_ev);
}

void an7552_phy_event_work(struct work_struct *work)
{
	struct an7552_phy *priv = container_of(work, struct an7552_phy,
						    event_work);
	unsigned long flags;
	u32 events;
	int i;

	spin_lock_irqsave(&priv->lock, flags);
	events = priv->pending_events;
	priv->pending_events = 0;
	spin_unlock_irqrestore(&priv->lock, flags);

	an7552_phy_dbg(priv, AN7552_DBG_EVENTS, "events 0x%x\n", events);
	for (i = 0; i < ARRAY_SIZE(an7552_ev_order); i++)
		if (events & BIT(an7552_ev_order[i])) {
			an7552_phy_dbg(priv, AN7552_DBG_EVENTS, "event %d\n",
				   an7552_ev_order[i]);
			an7552_handle_event(priv, an7552_ev_order[i]);
		}
	if (events & BIT(AN7552_PHYEV_NO_READY)) {
		an7552_phy_dbg(priv, AN7552_DBG_EVENTS, "event %d (no ready)\n",
			   AN7552_PHYEV_NO_READY);
		an7552_handle_event(priv, AN7552_PHYEV_NO_READY);
	}
	if (an7552_phy_debug & AN7552_DBG_REGS)
		an7552_phy_dump(priv, NULL);
}

/* ---- phy_ready_recover_expires() ---- */

void an7552_phy_recover_work(struct work_struct *work)
{
	struct an7552_phy *priv = container_of(to_delayed_work(work),
						    struct an7552_phy,
						    recover_work);
	bool rearm, kband = false;
	unsigned long flags;

	/* against an7552_phy_scu_reset(), which runs Mode_Config meanwhile */
	guard(mutex)(&priv->op_lock);
	spin_lock_irqsave(&priv->lock, flags);
	if (an7552_no_ready_no_los(priv)) {
		priv->recover_resets++;
		/* phy_ready_recover_expires (0x21204-0x21258): the bare PHYSET3
		 * pulse, not xPON_Pll_Reset with its TDC_HOLD_CTRL_0 writes */
		an7552_physet3_pulse_locked(priv);
		if (!priv->phy_ready_cnt ||
		    priv->recover_resets > AN7552_PHY_KBAND_RESET_CNT) {
			priv->recover_resets = 0;
			kband = true;
		}
	} else {
		priv->recover_resets = 0;
	}
	rearm = !an7552_ready_no_los(priv);
	/* gpPhyPriv[0] = 5000, re-armed by the pass that just ran */
	priv->recover_ms = AN7552_PHY_RECOVER_MS;
	spin_unlock_irqrestore(&priv->lock, flags);
	an7552_phy_dbg(priv, AN7552_DBG_EVENTS, "recover: resets %u kband %d rearm %d\n",
		   priv->recover_resets, kband, rearm);

	if (kband)
		an7552_k_band(priv);
	if (rearm)
		an7552_phy_recover_start(priv);
}

/* ---- phy_event_poll() ---- */

void an7552_phy_poll_work(struct work_struct *work)
{
	struct an7552_phy *priv = container_of(to_delayed_work(work),
						    struct an7552_phy,
						    poll_work);
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	if (priv->ready_flag) {
		if (!an7552_ready_no_los(priv)) {
			priv->ready_flag = false;
			an7552_raise(priv, AN7552_PHYEV_LOS);
			an7552_phy_recover_start_locked(priv);
		}
	} else if (an7552_ready_no_los(priv)) {
		priv->ready_flag = true;
		an7552_raise(priv, AN7552_PHYEV_READY);
		cancel_delayed_work(&priv->recover_work);
	} else if (an7552_no_ready_no_los(priv)) {
		an7552_raise(priv, AN7552_PHYEV_NO_READY);
	}
	an7552_phy_poll_start_locked(priv, AN7552_PHY_POLL_MS);
	spin_unlock_irqrestore(&priv->lock, flags);
}

/* ---- phy_isr() ---- */

irqreturn_t an7552_phy_irq_handler(int irq, void *dev_id)
{
	struct an7552_phy *priv = dev_id;
	u32 status, pma_status;

	spin_lock(&priv->lock);
	status = csr_read(priv, PHY_CSR_XPON_INT_STA);
	csr_write(priv, PHY_CSR_XPON_INT_STA_CLR, status);
	/* re-arm the LOS/LOF/ready time recorder */
	pma_rmw(priv, PMA_PON_DEBUG_REGISTER_15, 0, RG_RECORD_START_DBG_CLR);
	pma_write(priv, PMA_PON_DEBUG_REGISTER_14, 1);
	pma_status = pma_read(priv, PMA_PON_INTERRUPT_STS_0);
	pma_write(priv, PMA_PON_INTERRUPT_CLR_0, PMA_INT_CLR_ALL);

	if (status & (PHY_INT_TRANS_LOS | PHY_INT_PHY_ILLG)) {
		priv->ready_flag = false;
		/* gpPhyPriv[0] = 50, the recover period while the LOS is fresh */
		priv->recover_ms = AN7552_PHY_RECOVER_LOS_MS;
		if ((status & PHY_INT_TRANS_LOS) && (status & PHY_INT_PHY_ILLG))
			an7552_raise(priv, AN7552_PHYEV_LOS_FALSE_READY);
		else if (status & PHY_INT_TRANS_LOS)
			an7552_raise(priv, AN7552_PHYEV_LOS);
		else
			an7552_raise(priv, AN7552_PHYEV_ILLG);
		an7552_start_ms_locked(priv, &priv->recover_work,
				       AN7552_PHY_RECOVER_LOS_MS);
	}

	if (status & PHY_INT_PHY_LOF) {
		priv->ready_flag = false;
		an7552_raise(priv, AN7552_PHYEV_LOF);
		an7552_phy_poll_start_locked(priv, AN7552_PHY_POLL_LOF_MS);
	}

	if (status & PHY_INT_TRANS)
		an7552_raise(priv, AN7552_PHYEV_TRANS);
	if (status & PHY_INT_TRANS_SD_FAIL)
		an7552_raise(priv, AN7552_PHYEV_SD_FAIL);

	if (status & PHY_INT_PHYRDY) {
		if (priv->ready_flag && (status & PHY_INT_SIGDET)) {
			priv->ready_flag = false;
			an7552_raise(priv, AN7552_PHYEV_LOS);
			an7552_phy_recover_start_locked(priv);
		}
		/* a ready together with LOS or illegal is not authentic */
		if (!(status & (PHY_INT_TRANS_LOS | PHY_INT_PHY_ILLG))) {
			priv->ready_flag = true;
			an7552_raise(priv, AN7552_PHYEV_READY);
			cancel_delayed_work(&priv->recover_work);
		}
	}

	if (status & PHY_INT_I2CM)
		an7552_raise(priv, AN7552_PHYEV_I2C);

	/* rogue ONU: transmitter enabled outside of a grant */
	if (priv->rogue_enabled &&
	    (pma_read(priv, PMA_PON_INTERRUPT_SET_0) & RG_ROGUE_EN) &&
	    (pma_status & PMA_INT_RO_TX_EN)) {
		pma_rmw(priv, PMA_PON_INTERRUPT_SET_0, RG_ROGUE_EN, 0);
		pma_rmw(priv, PMA_PON_INTERRUPT_CLR_0, 0, RG_TX_EN_INT_CLR);
		an7552_raise(priv, AN7552_PHYEV_ROGUE);
	}
	spin_unlock(&priv->lock);

	return (status || pma_status) ? IRQ_HANDLED : IRQ_NONE;
}
