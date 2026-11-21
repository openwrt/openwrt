// SPDX-License-Identifier: GPL-2.0
/*
 * AN7552 xPON PHY - SerDes mode sequence and the vendor reset/init helpers
 *
 * A vendor write of the form "r & AND | OR" is written as *_rmw(off, ~AND, OR).
 */

#include <linux/bitfield.h>
#include <linux/delay.h>

#include "an7552_phy.h"

static u32 an7552_wan_conf(int mode)
{
	switch (mode) {
	case AN7552_XPON_GPON_SYM:
		return WAN_CONF_GPON_SYM;
	default:
		return WAN_CONF_GPON;		/* modes 1, 11 */
	}
}

/*
 * Mode_Config_7552().  Modes 1 and 11 take identical branches,
 * mode 10 differs from them in WAN_CONF, PON_OSR_SEL_CTRL and
 * PON_SERDES_CTRL_17.  Mode 0 (EPON) is not supported: its branches
 * are left out.
 *
 * Called with priv->lock held and interrupts off, as phy_mode_config does.
 * Busy-waits for about 2 ms in total.
 */
void an7552_mode_config_7552(struct an7552_phy *priv, int mode)
{
	bool sym = mode == AN7552_XPON_GPON_SYM;
	u32 trans = priv->xpon_trans_val;

	regmap_update_bits(priv->np_scu, NP_SCU_WAN_CONF, WAN_CONF_MODE,
			   an7552_wan_conf(mode));
	udelay(AN7552_PHY_STEP_DELAY_US);
	regmap_set_bits(priv->chip_scu, CHIP_SCU_IOMUX_CTRL_3,
			IOMUX_CTRL_3_PON);
	csr_rmw(priv, PHY_CSR_PHYSET10, PHY_GPON_MODE, PHY_GPON_MODE);
	udelay(AN7552_PHY_STEP_DELAY_US);

	/* PLL */
	ana_rmw(priv, ANA_RG_PCIE_CLKDRV_OFFSET, RG_SSUSB_XTAL_TOP_RESERVE,
		0x02400000);
	pma_rmw(priv, PMA_PON_RXFEDIG_CTRL_12, RG_SSUSB_EQ_REV,
		0x00380000);
	pma_rmw(priv, PMA_PON_RXFEDIG_CTRL_0, RG_SSUSB_EQ_RX500M_CK_SEL, 0);
	/* PLL_IR[23:20] = 5; PLL_BR[26:24] |= 7 (OR without clear, as vendor) */
	pma_rmw(priv, PMA_PON_DA_CTRL_0, RG_SSUSB_PLL_IR, 0x07500000);
	pma_rmw(priv, PMA_PON_DA_CTRL_19, RG_SSUSB_PLL_BPA, 0x4);
	pma_rmw(priv, PMA_PON_DA_CTRL_1, RG_SSUSB_PLL_BC, 0x20);
	ana_rmw(priv, ANA_RG_SSUSB_PLL_KBAND_KFC, RG_SSUSB_PLL_LDO_VSEL,
		0x00010000);
	pma_rmw(priv, PMA_PON_DA_CTRL_4, RG_SSUSB_PLL_FBKDIV, 0x31);
	pma_rmw(priv, PMA_PON_DA_CTRL_0, RG_SSUSB_PLL_FBKSEL, 0);
	ana_rmw(priv, ANA_RG_SSUSB_PLL_SDM_HREN, 0, RG_SSUSB_PLL_RESERVE_B24);

	/* Mode_Config_7552 records the NCPO target for en7523_k_band() */
	if (!sym)
		priv->ncpo_online = 0x63886595;

	/* NCPO: keep bit 31, load the value, pulse ncpo_chg */
	pma_rmw(priv, PMA_SS_LCPLL_TDC_FLT_2, RG_LCPLL_NCPO_VALUE,
		0x63886595);
	pma_rmw(priv, PMA_SS_LCPLL_TDC_PCW_1, RG_LCPLL_PON_HRDDS_PCW_NCPO,
		0x63886595);
	pma_rmw(priv, PMA_SS_LCPLL_TDC_PCW_2, RG_LCPLL_PON_HRDDS_PCW_NCPO,
		0x64000000);
	pma_rmw(priv, PMA_SS_LCPLL_TDC_FLT_5, RG_LCPLL_NCPO_CHG, 0);
	pma_rmw(priv, PMA_SS_LCPLL_TDC_FLT_3, 0, RG_LCPLL_NCPO_LOAD);
	udelay(AN7552_PHY_STEP_DELAY_US);
	pma_rmw(priv, PMA_SS_LCPLL_TDC_FLT_5, 0, RG_LCPLL_NCPO_CHG);
	pma_rmw(priv, PMA_SS_LCPLL_TDC_FLT_1, RG_LCPLL_GPON_SEL,
		RG_LCPLL_GPON_SEL);

	/* TX rate */
	pma_rmw(priv, PMA_PON_OSR_SEL_CTRL, RG_TX_OSR_SEL,
		sym ? 1 : 0);
	pma_rmw(priv, PMA_PON_SERDES_CTRL_2, RG_TX_DIV_MODE, 0);
	/* CDR_RESERVE[7:0] = 0x0c, then RXAFE_RESERVE[15:8] = 0x11 */
	ana_rmw(priv, ANA_RG_SSUSB_LN0_CDR_RESERVE,
		RG_SSUSB_LN0_CDR_RESERVE | RG_SSUSB_LN0_RXAFE_RESERVE,
		0x0c | 0x1100);
	pma_rmw(priv, PMA_SS_LCPLL_TDC_FLT_0, RG_LCPLL_PON_RX_CDR_DIVTDC, 0x4);

	/*
	 * CDR loop parameters.  The vendor ANDs with 0xffffe0e0 / 0xfff0ffe0 /
	 * 0xffffe0ff, i.e. clears mode_0[4:0], mode_1[12:8] and bic_mode_0
	 * [19:16]; 0xff100c also ORs ones into [23:16] without clearing.
	 */
	pma_rmw(priv, PMA_CDR_CTRL_0, CDR_MODE_0 | CDR_MODE_1, 0x00ff100c);
	pma_rmw(priv, PMA_CDR_CTRL_1, CDR_MODE_0 | CDR_MODE_1, 0x1e1e);
	pma_rmw(priv, PMA_CDR_CTRL_2, CDR_MODE_0 | CDR_BIC_MODE_0 | CDR_MODE_1,
		0x000c0008 | 0x800);
	pma_rmw(priv, PMA_CDR_CTRL_3, CDR_MODE_0 | CDR_MODE_1, 0x1e1e);
	pma_rmw(priv, PMA_CDR_CTRL_4, CDR_MODE_0 | CDR_BIC_MODE_0 | CDR_MODE_1,
		0x00030004 | 0x200);
	pma_rmw(priv, PMA_CDR_CTRL_5, CDR_MODE_0 | CDR_MODE_1, 0x1e1e);

	pma_rmw(priv, PMA_PON_DA_CTRL_2, 0, RG_SSUSB_CDR_PI_PWD);
	ana_rmw(priv, ANA_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS,
		RG_SSUSB_LN0_RX_DAC_PWD, 0);
	pma_rmw(priv, PMA_PON_DA_CTRL_6, 0, RG_FORCE_RX_EQ_EN);

	pma_rmw(priv, PMA_PON_SERDES_CTRL_0, 0, RG_PHYA_AUTO_INIT);
	mdelay(AN7552_PHYA_INIT_DELAY_MS);

	/* Frequency detector, RX side */
	pma_rmw(priv, PMA_FRQ_CTRL_2, RG_HG_RX_FREQ_DET_MUX, 0);
	pma_rmw(priv, PMA_FRQ_CTRL_4, RG_HG_RX_FREQ_CNT, 0x06180000);
	pma_rmw(priv, PMA_PON_SERDES_CTRL_5, RG_SIG_DET_WAIT_DLY,
		0x03f00000);
	pma_rmw(priv, PMA_SS_LCPLL_PWCTL_SETTING_0, RG_TDC_LOS_INV,
		(trans & XPON_SET_RX_SD_INV) ? RG_TDC_LOS_INV : 0);

	/* RX power-down pulse */
	pma_rmw(priv, PMA_PON_SERDES_CTRL_1, 0, RG_RX_PWD | RG_RX_PWD_MUX);
	udelay(AN7552_PHY_STEP_DELAY_US);
	pma_rmw(priv, PMA_PON_SERDES_CTRL_1, RG_RX_PWD, 0);
	udelay(AN7552_PHY_STEP_DELAY_US);

	/* Frequency detector, TX side */
	pma_rmw(priv, PMA_FRQ_CTRL_2, RG_HG_TX_FREQ_DET_MUX, 0);
	pma_rmw(priv, PMA_FRQ_CTRL_4, RG_HG_TX_FREQ_CNT, 0x618);
	pma_rmw(priv, PMA_PON_SERDES_CTRL_10, RG_TX_BIT_POLARITY, 0);

	/*
	 * tx_bit_end_dly[15:8] = 0x05 (mode 10) / 0x0b, and
	 * tx_bit_start_dly[7:0] = 0x00 / 0x02.
	 */
	pma_rmw(priv, PMA_PON_SERDES_CTRL_17, GENMASK(15, 0),
		sym ? 0x0500 : 0x0b02);
	pma_write(priv, PMA_PON_SERDES_CTRL_16, 0x2);
	pma_rmw(priv, PMA_SS_LCPLL_PWCTL_SETTING_0, 0, RG_INT_LOS_INV);
	pma_rmw(priv, PMA_TX_CTRL_2, RG_OUTBEN_DATA_MODE, 0x1);
	pma_rmw(priv, PMA_PON_SERDES_CTRL_0, RG_BEN_INV,
		(trans & XPON_SET_BURST_EN_INV) ? RG_BEN_INV : 0);
	pma_rmw(priv, PMA_SS_LCPLL_TDC_FLT_3, RG_LCPLL_NCPO_LOAD, 0);
	pma_rmw(priv, PMA_SS_LCPLL_TDC_PW_0, 0, RG_LCPLL_TDC_DIG_PWDB);

	/* Interrupt recorder */
	pma_write(priv, PMA_PON_DEBUG_REGISTER_20, 0xff);
	pma_write(priv, PMA_PON_DEBUG_REGISTER_15, 0x7001);
	pma_rmw(priv, PMA_PON_DEBUG_REGISTER_15, 0, RG_RECORD_START_DBG_CLR);
	pma_write(priv, PMA_PON_DEBUG_REGISTER_14, 0x1);
	pma_rmw(priv, PMA_TDC_HOLD_CTRL_0, 0, RG_TDC_LOS_HOLD_MUX);
}

/*
 * PHY_PLL_RST | PHY_COUNT_RST pulse for 1 ms, restoring the saved PHYSET3.
 * The same sequence is open-coded in xPON_Pll_Reset,
 * pon_phy_scu_reset_init, phy_tx_burst_config.  Caller holds
 * priv->lock.
 */
void an7552_physet3_pulse_locked(struct an7552_phy *priv)
{
	u32 saved = csr_read(priv, PHY_CSR_PHYSET3);

	csr_write(priv, PHY_CSR_PHYSET3, saved | PHY_PLL_RST | PHY_COUNT_RST);
	mdelay(AN7552_PLL_RESET_DELAY_MS);
	csr_write(priv, PHY_CSR_PHYSET3, saved);
}

/* xPON_Pll_Reset() body, caller holds priv->lock */
static void an7552_pll_reset_locked(struct an7552_phy *priv)
{
	pma_rmw(priv, PMA_TDC_HOLD_CTRL_0, RG_TDC_LOS_HOLD_MUX, 0);
	an7552_physet3_pulse_locked(priv);
	pma_rmw(priv, PMA_TDC_HOLD_CTRL_0, 0, RG_TDC_LOS_HOLD_MUX);
}

/* xPON_Pll_Reset() */
int an7552_pll_reset(struct an7552_phy *priv)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	an7552_pll_reset_locked(priv);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* en7523_k_band(): reband the RX (CDR) and TX (PLL) VCOs.  Driven by the
 * recover work after AN7552_PHY_KBAND_RESET_CNT resets, not by the ISR. */
void an7552_k_band(struct an7552_phy *priv)
{
	u32 xtal = 0, ncpo;
	int i, ret;

	/* a failed read leaves xtal 0, which reads as "not the RGS_ECC_SEL
	 * variant" and would reband an SoC that does not need it */
	ret = regmap_read(priv->chip_scu, CHIP_SCU_RGS_ECC_SEL, &xtal);
	if (ret) {
		dev_err(priv->dev, "k band: RGS_ECC_SEL read failed: %d\n", ret);
		return;
	}
	if (xtal & RGS_XTAL_FREQ)
		return;

	for (i = 0; i < 9; i++) {
		fsleep(1000);
		ncpo = pma_read(priv, PMA_SS_LCPLL_TDC_RO_4);
		if (ncpo > priv->ncpo_online - 0x10000 &&
		    ncpo < priv->ncpo_online + 0x10000)
			break;
	}

	/*
	 * No spinlock across the sequence: it is ~20 ms of delays, which the
	 * vendor runs unlocked, and the interrupts stay on, so it sleeps.  Each
	 * step is atomic on its own; the callers hold op_lock.
	 */
	/* RX (CDR) VCO band */
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_5, 0, RG_FORCE_CDR_AUTOK_VCO);
	udelay(1);
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_8, RG_SSUSB_CDR_AUTOK_VCO, 0);
	udelay(1);
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_8, 0, RG_SSUSB_CDR_AUTOK_VCO);
	fsleep(9000);
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_8, RG_SSUSB_CDR_AUTOK_VCO, 0);
	udelay(1);
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_5, RG_FORCE_CDR_AUTOK_VCO, 0);
	fsleep(5000);

	/* TX (PLL) VCO band */
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_5, 0, RG_FORCE_PLL_AUTOK_VCO);
	udelay(1);
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_5, 0, RG_FORCE_PLL_PWD);
	udelay(1);
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_7, 0, RG_SSUSB_PLL_AUTOK_VCO);
	udelay(1);
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_7, 0, RG_SSUSB_PLL_PWD);
	fsleep(5000);
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_7, RG_SSUSB_PLL_PWD, 0);
	fsleep(1000);
	pma_rmw_irq(priv, PMA_PON_DA_CTRL_7, RG_SSUSB_PLL_AUTOK_VCO, 0);

	/* restart TX and RX */
	pma_rmw_irq(priv, PMA_PON_SYS_CTRL_1, RG_ALL_TX_RST_N | RG_ALL_RX_RST_N, 0);
	udelay(1);
	pma_rmw_irq(priv, PMA_PON_SYS_CTRL_1, 0, RG_ALL_TX_RST_N | RG_ALL_RX_RST_N);

	dev_dbg(priv->dev, "k band: TDC_RO_2 %08x state %08x NCPO %08x\n",
		pma_read(priv, PMA_SS_LCPLL_TDC_RO_2),
		pma_read(priv, PMA_PON_DEBUG_REGISTER_2),
		pma_read(priv, PMA_SS_LCPLL_TDC_RO_4));
}

/* xPON_Logic_Reset(), caller holds priv->lock */
void an7552_logic_reset_locked(struct an7552_phy *priv)
{
	u32 saved;

	pma_rmw(priv, PMA_TDC_HOLD_CTRL_0, RG_TDC_LOS_HOLD_MUX, 0);
	saved = csr_read(priv, PHY_CSR_PHYSET3);
	csr_write(priv, PHY_CSR_PHYSET3, saved | PHY_PLL_RST | PHY_COUNT_RST);
	udelay(AN7552_PHY_STEP_DELAY_US);
	csr_write(priv, PHY_CSR_PHYSET3, saved);
	udelay(AN7552_PHY_STEP_DELAY_US);
	pma_rmw(priv, PMA_TDC_HOLD_CTRL_0, 0, RG_TDC_LOS_HOLD_MUX);
}

/*
 * xPON_Phy_Reset(): pulse SCU RST_CTRL2 bit 0 (the "phy" reset
 * line, EN7581_XPON_PHY_RST) for 1 us, then re-run Mode_Config_7552.
 */
int an7552_phy_reset(struct an7552_phy *priv)
{
	unsigned long flags;
	int ret;

	ret = reset_control_assert(priv->reset);
	if (ret)
		return ret;
	udelay(AN7552_PHY_RESET_DELAY_US);
	ret = reset_control_deassert(priv->reset);
	if (ret)
		return ret;

	spin_lock_irqsave(&priv->lock, flags);
	an7552_mode_config_7552(priv, priv->mode);
	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

/* phy_int_config(): replace bits [7:0] of both enables */
void an7552_int_config_locked(struct an7552_phy *priv, u32 int_en,
			      u32 pma_int_en)
{
	/* the rogue enable armed with the transmitter stays armed */
	if (priv->rogue_enabled && priv->trans_power_on)
		pma_int_en |= PMA_INT_RO_TX_EN;
	csr_rmw(priv, PHY_CSR_XPON_INT_EN, PHY_INT_FIELD,
		int_en & PHY_INT_FIELD);
	pma_rmw(priv, PMA_PON_INTERRUPT_EN_0, PMA_INT_FIELD,
		pma_int_en & PMA_INT_FIELD);
}

/* phy_fw_ready() */
void an7552_fw_ready_locked(struct an7552_phy *priv, bool ready)
{
	csr_rmw(priv, PHY_CSR_PHYSET2, PHY_FW_RDY, ready ? PHY_FW_RDY : 0);
}

/*
 * phy_counter_clear(), types are AN7552_PHY_*_CNT_CLR.  The
 * EPON error frame clear (only outside GPON mode) is left out.
 */
void an7552_counter_clear_locked(struct an7552_phy *priv, u32 types)
{
	if (types & AN7552_PHY_ERR_CNT_CLR)
		csr_write(priv, PHY_CSR_XP_ERRCNT_CTL, PHY_ERR_CLR);
	if (types & AN7552_PHY_BIP_CNT_CLR)
		csr_write(priv, PHY_CSR_XP_ERRCNT_CTL, PHY_BIP_CLR);
	if (types & AN7552_PHY_RXFRAME_CNT_CLR)
		csr_write(priv, PHY_CSR_XP_ERRCNT_CTL, PHY_RXFRAME_CLR);
	if (types & AN7552_PHY_TXFRAME_CNT_CLR)
		csr_rmw(priv, PHY_CSR_TX_TEST_TRIG, 0, PHY_TX_CNT_CLR);
}

/* phy_reset_counter() */
void an7552_reset_counter_locked(struct an7552_phy *priv)
{
	an7552_counter_clear_locked(priv, AN7552_PHY_ERR_CNT_CLR |
				    AN7552_PHY_BIP_CNT_CLR |
				    AN7552_PHY_RXFRAME_CNT_CLR |
				    AN7552_PHY_TXFRAME_CNT_CLR);
}

/* phy_cnt_enable() */
static void an7552_cnt_enable_locked(struct an7552_phy *priv, bool err, bool bip,
			      bool rxfrm)
{
	u32 val = csr_read(priv, PHY_CSR_XP_ERRCNT_EN) & ~ERRCNT_EN_MASK;

	if (err)
		val |= ERRCNT_EN_ERR;
	if (bip)
		val |= ERRCNT_EN_BIP;
	if (rxfrm)
		val |= ERRCNT_EN_RXFRAME;
	udelay(AN7552_CNT_ENABLE_DELAY_US);
	csr_write(priv, PHY_CSR_XP_ERRCNT_EN, val);
	an7552_counter_clear_locked(priv, AN7552_PHY_ERR_CNT_CLR |
				    AN7552_PHY_BIP_CNT_CLR |
				    AN7552_PHY_RXFRAME_CNT_CLR);
}

/*
 * phy_dev_init().  The vendor does the PHYSET3 write before
 * opening the PBUS gate; either order works for it, csr 0x100-0x1ff answers
 * with the gate shut (Mode_Config_7552() writes there in the probe before
 * the gate opens).  The accesses past it, the guard (+0x404) and the int
 * enable (+0x5f0), come after the gate, as in the vendor.  int_enable
 * corresponds to the "phy started" condition.
 */
void an7552_phy_dev_init(struct an7552_phy *priv, bool int_enable)
{
	unsigned long flags;

	regmap_set_bits(priv->chip_scu, CHIP_SCU_IOMUX_CTRL_3,
			IOMUX_CTRL_3_PON);
	regmap_clear_bits(priv->np_scu, NP_SCU_RST_ACC_CHK_BMAP,
			  RST_ACC_CHK_PBUS_DISABLE);
	regmap_clear_bits(priv->np_scu, NP_SCU_GPIO_SSR,
			  GPIO_SSR_TOP_LED1_MODE);

	spin_lock_irqsave(&priv->lock, flags);
	csr_rmw(priv, PHY_CSR_PHYSET3, PHY_SQUELCH_DEGLITCH, 0);
	if (int_enable)
		an7552_int_config_locked(priv, PHY_INT_VENDOR_MASK, 0);
	an7552_cnt_enable_locked(priv, true, true, true);
	csr_write(priv, PHY_CSR_GPON_DELIMITER_GUARD,
		  priv->guard_delimiter);
	an7552_reset_counter_locked(priv);
	spin_unlock_irqrestore(&priv->lock, flags);
}
