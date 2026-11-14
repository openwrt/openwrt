// SPDX-License-Identifier: GPL-2.0
/*
 * AN7552 xPON - PHY transmit side: preamble, delimiter/guard, bit delay,
 * transceiver pin setting and transmitter power.
 *
 * Registers of the SerDes that shape an upstream burst, plus the transmitter
 * line itself.  What the optical front end is *told* - the mode it runs in and
 * whether user traffic flows - is not a register of ours and is in
 * an7552_laser.c with the rest of the front end.
 */

#include <linux/bitfield.h>

#include <linux/pon_laser.h>

#include "an7552_phy.h"

/* pon_phy_rogueonu_int_en() */
static void an7552_rogue_int_en(struct an7552_phy *priv, bool on)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	if (on) {
		pma_rmw(priv, PMA_PON_INTERRUPT_SET_0, 0, RG_ROGUE_EN);
		pma_rmw(priv, PMA_PON_INTERRUPT_EN_0, 0, PMA_INT_RO_TX_EN);
	} else {
		pma_rmw(priv, PMA_PON_INTERRUPT_SET_0, RG_ROGUE_EN, 0);
	}
	spin_unlock_irqrestore(&priv->lock, flags);
}

/*
 * debugfs phy/rogue_en.
 *
 * The vendor keeps this in gpPhyPriv+0xbc and nothing in the SDK ever
 * writes it, so there is no "the driver enables it" default to copy: it is
 * a setting here, off unless it is asked for, and an7552_trans_power_switch()
 * arms it with the transmitter.  On a rogue event the PHY raises
 * AN7552_PHYEV_ROGUE and the transmitter is switched off again.
 */
int an7552_phy_rogue_en(struct an7552_phy *priv, bool on)
{
	WRITE_ONCE(priv->rogue_enabled, on);
	an7552_rogue_int_en(priv, on);

	return 0;
}

/* phy_tx_power_config() (phy.c:2685-2688): sets txPowerEnFlag, no register */
void an7552_phy_tx_power(struct an7552_phy *priv, bool on)
{
	WRITE_ONCE(priv->tx_power_en, on);
}

/* phy_gpon_delimiter_guard() */
int an7552_phy_gpon_delimiter_guard(struct an7552_phy *priv, u32 delimiter,
					 u8 guard_time)
{
	unsigned long flags;

	if (delimiter & ~DELIMITER_PAT)
		return -EINVAL;

	spin_lock_irqsave(&priv->lock, flags);
	priv->guard_delimiter = delimiter |
				FIELD_PREP(GUARD_PAT, guard_time);
	csr_write(priv, PHY_CSR_GPON_DELIMITER_GUARD, priv->guard_delimiter);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* phy_gpon_preamble() */
int an7552_phy_gpon_preamble(struct an7552_phy *priv,
				  const struct an7552_phy_preamble *p)
{
	unsigned long flags;
	u32 val;

	spin_lock_irqsave(&priv->lock, flags);
	val = csr_read(priv, PHY_CSR_GPON_PREAMBLE);
	if (p->mask & PHY_GUARD_BIT_NUM_EN)
		FIELD_MODIFY(PREAMBLE_GUARD_BIT_NUM, &val, p->guard_bit_num);
	if (p->mask & PHY_PRE_T1_NUM_EN)
		FIELD_MODIFY(PREAMBLE_T1_NUM, &val, p->preamble_t1_num);
	if (p->mask & PHY_PRE_T2_NUM_EN)
		FIELD_MODIFY(PREAMBLE_T2_NUM, &val, p->preamble_t2_num);
	if (p->mask & PHY_PRE_T3_PAT_EN)
		FIELD_MODIFY(PREAMBLE_T3_PAT, &val, p->preamble_t3_pat);
	priv->preamble = val;
	priv->preamble_valid = true;
	csr_write(priv, PHY_CSR_GPON_PREAMBLE, val);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* phy_gpon_extend_preamble() (phy_tx.c:145-202, CONFIG_USE_A60928 build) */
int an7552_phy_gpon_extend_preamble(struct an7552_phy *priv,
					 const struct an7552_phy_preamble *p)
{
	unsigned long flags;
	u32 val;

	/* range checks: phy_tx.c:158, 167, 177, 187 */
	if (((p->mask & PHY_EXT_BUR_MODE_EN) && p->extend_burst_mode > 1) ||
	    ((p->mask & PHY_OPER_RANG_EN) && p->oper_ranged_st > 3) ||
	    ((p->mask & PHY_EXTB_LENG_SEL_EN) && p->extb_length_sel > 1) ||
	    ((p->mask & PHY_DIS_SCRAM_EN) && p->dis_scramble > 1))
		return -EINVAL;

	spin_lock_irqsave(&priv->lock, flags);
	val = csr_read(priv, PHY_CSR_GPON_EXTEND_PREAMBLE);
	if (p->mask & PHY_T3_O4_PRE_EN)
		FIELD_MODIFY(EXTPRE_T3_O4, &val, p->t3_o4_preamble);
	if (p->mask & PHY_T3_O5_PRE_EN)
		FIELD_MODIFY(EXTPRE_T3_O5, &val, p->t3_o5_preamble);
	if (p->mask & PHY_EXT_BUR_MODE_EN)
		FIELD_MODIFY(EXTPRE_EXT_BURST, &val, p->extend_burst_mode);
	if (p->mask & PHY_OPER_RANG_EN)
		FIELD_MODIFY(EXTPRE_OPER_RANGED, &val, p->oper_ranged_st);
	if (p->mask & PHY_EXTB_LENG_SEL_EN)
		FIELD_MODIFY(EXTPRE_EXTB_LENG_SEL, &val, p->extb_length_sel);
	if (p->mask & PHY_DIS_SCRAM_EN)
		FIELD_MODIFY(EXTPRE_DIS_SCRAM, &val, p->dis_scramble);
	priv->ext_preamble = val;				/* phy_tx.c:198 */
	priv->ext_preamble_valid = true;
	csr_write(priv, PHY_CSR_GPON_EXTEND_PREAMBLE, val);	/* phy_tx.c:199 */
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* phy_bit_delay(): TX_CTRL_0[7:0] = delay, delay < 8 */
int an7552_phy_set_bit_delay(struct an7552_phy *priv, u32 delay)
{
	unsigned long flags;

	if (delay > AN7552_PHY_BIT_DELAY_MAX)
		return -EINVAL;

	spin_lock_irqsave(&priv->lock, flags);
	pma_rmw(priv, PMA_TX_CTRL_0, RG_TX_DLY_DATA_FTUNE_LOW, delay);
	priv->digital_bit_delay = delay;
	priv->digital_bit_delay_valid = true;
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* ---- transceiver pin polarity (PHY_CSR_XPON_SETTING) ---- */

/* The vendor PHY_TransConfig_T bits and their XPON_SETTING bits */
static const struct {
	u32 flag;
	u32 bit;
} an7552_trans_inv_map[] = {
	{ AN7552_TRANS_TX_SD_INV,	XPON_SET_TX_SD_INV },
	{ AN7552_TRANS_TX_FAULT_INV,	XPON_SET_TX_FAULT_INV },
	{ AN7552_TRANS_RX_SD_INV,	XPON_SET_RX_SD_INV },
	{ AN7552_TRANS_BURST_EN_INV,	XPON_SET_BURST_EN_INV },
};

/*
 * Change transceiver pin polarities at run time.
 *
 * phy_trans_rx_setting() and phy_trans_tx_setting()
 * only rewrite XPON_SETTING bits 6 and 4/7/5.  The SerDes
 * keeps its own copies of two of them, which Mode_Config_7552 derives from
 * phy_xpon_trans_val: rx_sd_inv -> rg_tdc_los_inv and burst_en_inv ->
 * rg_ben_inv (see an7552_mode_config_7552()).  The
 * vendor "phy_trans" debug command (phy_write_proc) does the
 * complete change - shadow, XPON_SETTING and both SerDes bits - and is
 * what is done here, so the setting also survives the next SCU reset or
 * mode switch.
 */
int an7552_phy_set_trans_inv(struct an7552_phy *priv, u32 mask, u32 val)
{
	unsigned long flags;
	u32 clr = 0, set = 0;
	int i;

	if (mask & ~AN7552_TRANS_INV_ALL)
		return -EINVAL;

	for (i = 0; i < ARRAY_SIZE(an7552_trans_inv_map); i++) {
		if (!(mask & an7552_trans_inv_map[i].flag))
			continue;
		clr |= an7552_trans_inv_map[i].bit;
		if (val & an7552_trans_inv_map[i].flag)
			set |= an7552_trans_inv_map[i].bit;
	}

	spin_lock_irqsave(&priv->lock, flags);
	if (!priv->xpon_trans_val_valid)
		priv->xpon_trans_val = csr_read(priv, PHY_CSR_XPON_SETTING);
	priv->xpon_trans_val = (priv->xpon_trans_val & ~clr) | set;
	priv->xpon_trans_val_valid = true;
	csr_write(priv, PHY_CSR_XPON_SETTING, priv->xpon_trans_val);
	pma_rmw(priv, PMA_SS_LCPLL_PWCTL_SETTING_0, RG_TDC_LOS_INV,
		(priv->xpon_trans_val & XPON_SET_RX_SD_INV) ?
		RG_TDC_LOS_INV : 0);
	pma_rmw(priv, PMA_PON_SERDES_CTRL_0, RG_BEN_INV,
		(priv->xpon_trans_val & XPON_SET_BURST_EN_INV) ?
		RG_BEN_INV : 0);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/*
 * phy_trans_rx_getting() and phy_trans_tx_getting()
 */
u32 an7552_phy_get_trans_inv(struct an7552_phy *priv)
{
	u32 reg = csr_read(priv, PHY_CSR_XPON_SETTING);
	u32 val = 0;
	int i;

	for (i = 0; i < ARRAY_SIZE(an7552_trans_inv_map); i++)
		if (reg & an7552_trans_inv_map[i].bit)
			val |= an7552_trans_inv_map[i].flag;

	return val;
}

/* ---- burst / continuous transmission ---- */

/*
 * PON_SET_PHY 0xd: phy_tx_burst_config(): PHYSET3 bits 7
 * and 5 both set for continuous mode, both clear for burst mode.  With the
 * receiver in LOS the PHY is given a PLL/count reset pulse.  Going back to
 * burst mode also sets txPowerEnFlag and switches the transmitter on.
 */
int an7552_phy_set_tx_cont(struct an7552_phy *priv, bool cont)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	csr_rmw(priv, PHY_CSR_PHYSET3, PHY_TX_CONT_MODE,
		cont ? PHY_TX_CONT_MODE : 0);
	if (an7552_phy_is_los(priv))
		an7552_physet3_pulse_locked(priv);
	spin_unlock_irqrestore(&priv->lock, flags);

	if (!cont) {
		WRITE_ONCE(priv->tx_power_en, true);
		an7552_trans_power_switch(priv, true);
	}

	return 0;
}

/*
 * phy_ben_switch(), PON_SERDES_CTRL_15: on forces burst
 * enable high (rg_ben_force_mode | rg_ben_force_value); off pulses
 * force_mode with value 0, then clears bits [31:28] (BEN from the MAC).
 */
static void an7552_ben_switch_locked(struct an7552_phy *priv, bool on)
{
	u32 val = pma_read(priv, PMA_PON_SERDES_CTRL_15);

	if (on) {
		pma_write(priv, PMA_PON_SERDES_CTRL_15, val | RG_BEN_FORCE_MODE |
			  RG_BEN_FORCE_VALUE);
		return;
	}
	val &= ~RG_BEN_FORCE_FIELD;
	pma_write(priv, PMA_PON_SERDES_CTRL_15, val | RG_BEN_FORCE_MODE);
	pma_write(priv, PMA_PON_SERDES_CTRL_15, val);
}

int an7552_phy_ben_force(struct an7552_phy *priv, bool on)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	an7552_ben_switch_locked(priv, on);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* No vendor getter; reads back the two bits phy_ben_switch(1) sets */
bool an7552_phy_ben_forced(struct an7552_phy *priv)
{
	u32 val = pma_read(priv, PMA_PON_SERDES_CTRL_15);

	return (val & (RG_BEN_FORCE_MODE | RG_BEN_FORCE_VALUE)) ==
	       (RG_BEN_FORCE_MODE | RG_BEN_FORCE_VALUE);
}

/*
 * PON_SET_PHY 0x24: phy_tx_Mode().  Only the modes that
 * send normal data are supported: the PRBS modes 0, 2, 3 are test-only and
 * mode 1 is rejected by the vendor too ("NOT support!").  Each mode is
 * phy_ben_switch(), phy_trans_power_switch() and phy_tx_test_pattern(0),
 * which is BIST idle.
 */
int an7552_phy_set_tx_mode(struct an7552_phy *priv, enum an7552_xpon_tx_mode mode)
{
	unsigned long flags;
	bool ben, power;

	switch (mode) {
	case AN7552_TX_MODE_NORMAL:
		ben = false;
		power = true;
		break;
	case AN7552_TX_MODE_CONT:
		ben = true;
		power = true;
		break;
	case AN7552_TX_MODE_OFF:
		ben = false;
		power = false;
		break;
	default:
		return -EOPNOTSUPP;
	}

	guard(mutex)(&priv->op_lock);
	WRITE_ONCE(priv->tx_mode, mode);
	spin_lock_irqsave(&priv->lock, flags);
	an7552_ben_switch_locked(priv, ben);
	spin_unlock_irqrestore(&priv->lock, flags);
	an7552_trans_power_switch(priv, power);
	spin_lock_irqsave(&priv->lock, flags);
	csr_write(priv, PHY_CSR_BISTCTL_PRBS_TX_EN, 0);
	csr_write(priv, PHY_CSR_BISTCTL_LOOPBACK_SEL, 0);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* ---- transmitter line, swing, fault ---- */

/*
 * The transmitter line is held with one regulator reference of ours, taken
 * and dropped only when the state changes: the callers repeat themselves
 * (ifup, the no-ready handler, every LASER and ADMIN_DOWN request), and a
 * second regulator_disable() for a single enable is an "unbalanced disables"
 * warning, a second enable a line that one disable no longer drops.
 */
static int an7552_tx_reg_set(struct an7552_phy *priv, bool on)
{
	int ret = 0;

	guard(mutex)(&priv->tx_reg_lock);
	if (!priv->tx_rdev || priv->tx_reg_on == on)
		return 0;

	ret = on ? regulator_enable(priv->tx_rdev) :
		   regulator_disable(priv->tx_rdev);
	if (!ret)
		priv->tx_reg_on = on;

	return ret;
}

/*
 * Give back the reference this module holds on the front end's transmitter
 * regulator, taken in an7552_laser_setup().  An enabled line is disabled
 * first: a put with the enable still counted only warns in the regulator
 * core and leaves the transmitter on with nobody to switch it off.
 *
 * Not devres on purpose (see there): the reference has to go when the front
 * end detaches, because it is what keeps the front end's module loaded - the
 * regulator core takes a module reference on its owner for as long as a
 * consumer holds one.  An7552_laser_detached() calls this, so a front end that
 * unbinds is a module that can then be unloaded.
 *
 * Under the same lock as the enable, so a concurrent an7552_phy_tx_ctl()
 * cannot take the reference again between the disable here and the put.
 */
void an7552_tx_reg_put(struct an7552_phy *priv)
{
	struct regulator *rdev;

	mutex_lock(&priv->tx_reg_lock);
	rdev = priv->tx_rdev;
	if (rdev && priv->tx_reg_on && regulator_disable(rdev))
		dev_err(priv->dev, "transmitter off failed\n");
	priv->tx_rdev = NULL;
	priv->tx_reg_on = false;
	mutex_unlock(&priv->tx_reg_lock);

	if (rdev)
		regulator_put(rdev);
}

/*
 * phy_trans_power_switch(): phy_tx_ctl() drives LED 42 "TX power disable",
 * which on this board is the EN7571 tx-disable GPIO, reached through the
 * regulator the EN7571 registers for it.  Switching on needs the
 * txPowerEnFlag and is followed by the EN7571 safe circuit reset.
 *
 * The switch returns void like the vendor and its callers, so a failure to
 * enable is reported and not propagated.
 */
void an7552_trans_power_switch(struct an7552_phy *priv, bool on)
{
	int ret;

	WRITE_ONCE(priv->trans_power_on, on);

	if (!on) {
		if (priv->rogue_enabled)
			an7552_rogue_int_en(priv, false);
		an7552_tx_reg_set(priv, false);
		return;
	}

	if (priv->rogue_enabled)
		an7552_rogue_int_en(priv, true);
	if (!READ_ONCE(priv->tx_power_en))
		return;
	if (!priv->tx_rdev)
		return;
	ret = an7552_tx_reg_set(priv, true);
	if (ret)
		dev_err(priv->dev, "transmitter on: %d\n", ret);
	else
		schedule_work(&priv->laser_work);
}

/*
 * phy_tx_ctl(): drive the TX-disable line (LED 0x2a) only,
 * without the txPowerEnFlag check, the rogue interrupt and the state that
 * an7552_trans_power_switch() keeps.  It shares the one reference with it, so
 * the two cannot leave the count apart.
 */
int an7552_phy_tx_ctl(struct an7552_phy *priv, bool on)
{
	if (!priv->tx_rdev)
		return -ENODEV;

	return an7552_tx_reg_set(priv, on);
}

/* State of the transmitter (TX-disable) line as last driven */
bool an7552_phy_tx_ctl_get(struct an7552_phy *priv)
{
	bool on;

	/* the reference can go with the front end meanwhile */
	guard(mutex)(&priv->tx_reg_lock);
	on = priv->tx_rdev && regulator_is_enabled(priv->tx_rdev);

	return on;
}

/*
 * phy_tx_amp_setting() (phy_tx.c:411-426): TX output swing,
 * ANATXREG1[19:16].  Levels above 15 are rejected.
 */
int an7552_phy_set_tx_amp(struct an7552_phy *priv, u8 level)
{
	unsigned long flags;

	if (level > FIELD_MAX(PHY_TX_AMP_LEVEL))
		return -EINVAL;

	spin_lock_irqsave(&priv->lock, flags);
	csr_rmw(priv, PHY_CSR_ANATXREG1, PHY_TX_AMP_LEVEL,
		FIELD_PREP(PHY_TX_AMP_LEVEL, level));
	/* kept across SCU resets, like a DT airoha,tx-amplitude */
	priv->tx_amp = level;
	priv->tx_amp_valid = true;
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/*
 * phy_tx_fault_reset(): the optical front end's safe circuit
 * reset; only the EN7571 branch exists here.  The stock gpio_tx_dis_reset()
 * is this after phy_tx_ctl(0) and only runs at EN7571 init,
 * which the en7571 driver does itself (TX line taken asserted at probe,
 * safe circuit reset in its hardware init).
 */
int an7552_phy_tx_fault_reset(struct an7552_phy *priv)
{
	pon_laser_safe_circuit_reset_fn fn =
		READ_ONCE(priv->laser_safe_circuit_reset);

	if (!fn)
		return -ENODEV;

	return fn();
}
