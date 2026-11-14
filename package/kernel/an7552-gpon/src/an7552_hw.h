/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Airoha AN7552/AN7563 xPON - hardware register map and timing constants
 *
 * Everything that describes the silicon, for both halves of the driver:
 * the PON SerDes PHY and the GPON MAC.  Nothing here knows about driver
 * state; the PHY layer (an7552_phy.h) and the MAC layer (an7552_mac.h)
 * build on it.
 *
 * PHY - three MMIO windows, mapped by name from the DT:
 *
 *   "csr"  0x1faf0000  digital PHY CSR (vendor phy_reg.h REG_BASE_1..5)
 *   "ana"  0x1faf3000  SerDes analog  (pon_hsgmii_ana_csr_c_header.h)
 *   "pma"  0x1faf4000  SerDes digital (pon_hsgmii_dig_csr_c_header.h)
 *
 * MAC - one window:
 *
 *   "gpon" 0x1fb64000  GPON MAC (xpon_1g gpon_mac_reg_c_header_en7521.h)
 *
 * PHY offsets are relative to the window base.  The comment after each
 * register gives the absolute address used by the vendor PHY driver.
 * Field names are the ones from the two SerDes headers or from phy_def.h.
 *
 * The GPON MAC offsets are the vendor ones relative to 0x1fb60000 minus
 * 0x4000, so that the "gpon" window above lands on them directly.
 * Sequences and default values follow the stock xPON driver.
 */

#ifndef _AN7552_HW_H
#define _AN7552_HW_H

#include <linux/bits.h>

/* ---- NP SCU ("airoha,scu", 0x1fb00000) - ecnt_scu.c ------------------- */

#define NP_SCU_WAN_CONF			0x070	/* ecnt_scu.c:69 */
#define   WAN_CONF_MODE			GENMASK(7, 0)
#define   WAN_CONF_GPON			0x00	/* modes 1 and 11 */
#define   WAN_CONF_GPON_SYM		0x14	/* mode 10 */
#define NP_SCU_GPIO_SSR			0x860	/* ecnt_scu.c:88 */
#define   GPIO_SSR_TOP_LED1_MODE	BIT(10)	/* phy_def.h:60 */
#define NP_SCU_RST_ACC_CHK_BMAP		0x92c	/* ecnt_scu.c:92 */
#define   RST_ACC_CHK_PBUS_DISABLE	BIT(2)	/* phy_def.h:49 */

/* ---- chip SCU ("airoha,chip-scu", 0x1fa20000) ----------------------- */

/* ecnt_scu.c:116-117 (EN7581/AN7552): RGS_ECC_SEL bit 19 = XTAL frequency */
#define CHIP_SCU_RGS_ECC_SEL		0x258
#define   RGS_XTAL_FREQ			BIT(19)
#define CHIP_SCU_IOMUX_CTRL_3		0x21c	/* ecnt_scu.h:82, ecnt_scu.c:137 */
#define   IOMUX_CTRL_3_PON		BIT(0)

/* ---- CSR window, bank 1 -------------------------------------------- */

#define PHY_CSR_PHYSET2			0x104
#define   PHY_FW_RDY			BIT(0)	/* phy_def.h:102 */
#define PHY_CSR_PHYSET3			0x108
#define   PHY_SQUELCH_DEGLITCH		BIT(2)	/* phy_def.h:70 */
#define   PHY_TX_CONT_MODE		(BIT(7) | BIT(5)) /* phy_def.h:125-126 */
#define   PHY_COUNT_RST			BIT(27)	/* phy_def.h:164 */
#define   PHY_PLL_RST			BIT(31)	/* phy_def.h:157 */
#define PHY_CSR_PHYSET10		0x124
#define   PHY_GPON_MODE			BIT(31)	/* phy_def.h:106 */
#define PHY_CSR_PHYSAP			0x12c	/* phy_reg.h:62; sense-amp cal */
#define PHY_CSR_PHYSTA1			0x130
#define   PHYSTA1_PHYRDY		GENMASK(20, 18)	/* phy_def.h:210,216 */
#define   PHYRDY_STATUS			6	/* phy_def.h:212 (A60928) */
#define   PHYRDY_STATUS_UNLOCKED	2	/* phy_init.c:184 recover test */
#define PHY_CSR_XPON_SETTING		0x138	/* phy_reg.h:67 */
#define   XPON_SET_TX_SD_INV		BIT(4)	/* phy_def.h:148-149 */
#define   XPON_SET_TX_FAULT_INV		BIT(5)	/* phy_def.h:152-153 */
#define   XPON_SET_RX_SD_INV		BIT(6)	/* phy_def.h:144 */
#define   XPON_SET_BURST_EN_INV		BIT(7)	/* phy_def.h:150-151 */
#define PHY_CSR_ANACAL1			0x140	/* phy_reg.h:70 (read only) */
#define PHY_CSR_ANACAL2			0x144	/* phy_reg.h:71 (read only) */
#define PHY_CSR_ANATXREG1		0x1a0	/* phy_reg.h:94 */
#define   PHY_TX_AMP_LEVEL		GENMASK(19, 16)	/* phy_def.h:130-132 */
#define PHY_CSR_TDCSTA1			0x1f4	/* phy_reg.h:113 (read only) */

/* ---- CSR window, bank 2: receive side counters --------------------- */

#define PHY_CSR_RX_STATUS		0x21c	/* phy_reg.h:124 */
#define   PHY_RX_FEC			BIT(15)	/* phy_def.h:202 */
#define PHY_CSR_XP_ERRCNT_EN		0x230	/* phy_reg.h:131 */
#define   ERRCNT_EN_ERR			BIT(0)	/* phy_def.h:196-198 */
#define   ERRCNT_EN_BIP			BIT(1)
#define   ERRCNT_EN_RXFRAME		BIT(2)
#define   ERRCNT_EN_MASK		GENMASK(2, 0)
#define PHY_CSR_XP_ERRCNT_CTL		0x234	/* phy_reg.h:132 */
#define   PHY_ERR_LATCH			BIT(0)	/* phy_def.h:190 */
#define   PHY_ERR_CLR			BIT(1)	/* phy_def.h:191 */
#define   PHY_BIP_LATCH			BIT(2)	/* phy_def.h:192 */
#define   PHY_BIP_CLR			BIT(3)	/* phy_def.h:193 */
#define   PHY_RXFRAME_LATCH		BIT(4)	/* phy_def.h:194 */
#define   PHY_RXFRAME_CLR		BIT(5)	/* phy_def.h:195 */
#define PHY_CSR_ERR_BYTE_CNT		0x238	/* phy_reg.h:133 */
#define PHY_CSR_ERR_CODE_CNT		0x23c	/* phy_reg.h:134 */
#define PHY_CSR_NOSOL_CODE_CNT		0x240	/* phy_reg.h:135 */
#define PHY_CSR_RX_CODE_CNT		0x244	/* phy_reg.h:136 */
#define PHY_CSR_FEC_SECONDS		0x248	/* phy_reg.h:137 */
#define PHY_CSR_BIP_CNT			0x24c	/* phy_reg.h:138 */
#define PHY_CSR_FRAME_CNT_L		0x250	/* phy_reg.h:139 */
#define PHY_CSR_FRAME_CNT_H		0x254	/* phy_reg.h:140 */
#define PHY_CSR_LOF_CNT			0x258	/* phy_reg.h:141 */
#define PHY_CSR_FECDEC_CTL		0x268	/* phy_reg.h:144 */
/* phy_round_trip_delay_sof(): the delay the SerDes measures between the
 * word sync and the start of the downstream frame, i.e. the length of the
 * PON it is attached to.  The SDK stores it once at O5 (gpon_act.c:332) and
 * reads it again on every PHY ready (gpon.c:286), but skips the EqD fine tune
 * it feeds on every chip but AN7552 (gpon.c:331); read-only here for the same
 * reason: it is a measurement, nothing is programmed from it.  Bits 15:0 */
#define PHY_CSR_ROUND_TRIP_DELAY	0x2d4	/* REG_BASE_2 + 0x00d4, phy_reg.h:160 */
#define   RTD_MASK			GENMASK(15, 0)

/* ---- CSR window, bank 4: transmit side ----------------------------- */

#define PHY_CSR_GPON_PREAMBLE		0x400	/* phy_reg.h:166 */
#define   PREAMBLE_GUARD_BIT_NUM	GENMASK(7, 0)	/* phy_def.h:297 */
#define   PREAMBLE_T1_NUM		GENMASK(15, 8)	/* phy_def.h:298-299 */
#define   PREAMBLE_T2_NUM		GENMASK(23, 16)	/* phy_def.h:300-301 */
#define   PREAMBLE_T3_PAT		GENMASK(31, 24)	/* phy_def.h:302-303 */
#define PHY_CSR_GPON_DELIMITER_GUARD	0x404	/* phy_reg.h:167 */
#define   DELIMITER_PAT			GENMASK(23, 0)	/* phy_def.h:206 */
#define   GUARD_PAT			GENMASK(31, 24)	/* phy_def.h:207 */
#define   GPON_DELIMITER_GUARD_DEFAULT	0xaaab5983
#define PHY_CSR_GPON_EXTEND_PREAMBLE	0x408	/* phy_reg.h:168 */
#define   EXTPRE_T3_O4			GENMASK(7, 0)	/* phy_def.h:304 */
#define   EXTPRE_T3_O5			GENMASK(15, 8)	/* phy_def.h:305-306 */
#define   EXTPRE_EXT_BURST		BIT(16)	/* phy_def.h:307-308 */
#define   EXTPRE_OPER_RANGED		GENMASK(18, 17)	/* phy_def.h:309-311 */
#define   EXTPRE_EXTB_LENG_SEL		BIT(19)	/* phy_def.h:291-295 (A60928) */
#define   EXTPRE_DIS_SCRAM		BIT(24)	/* phy_def.h:312-313 */
#define PHY_CSR_TX_STATUS		0x40c	/* phy_reg.h:169 */
#define   PHY_TX_FEC			BIT(15)	/* phy_def.h:203, phy_tx.c:91-92 */
#define PHY_CSR_TX_TEST_TRIG		0x424	/* phy_reg.h:172 */
#define   PHY_TX_CNT_CLR		BIT(2)	/* phy_def.h:136 */
#define   PHY_TX_CNT_LCH		BIT(3)	/* phy_def.h:137, phy_tx.c:566 */
#define PHY_CSR_TX_FRAME_COUNTER	0x434	/* phy_reg.h:177 */
#define PHY_CSR_TX_BURST_COUNTER	0x438	/* phy_reg.h:178 */
/* phy_tx_test_pattern(0), "BIST idle": both written 0 */
#define PHY_CSR_BISTCTL_LOOPBACK_SEL	0x4a0	/* phy_reg.h:187 */
#define PHY_CSR_BISTCTL_PRBS_TX_EN	0x4a4	/* phy_reg.h:188 */

/* ---- CSR window, bank 5: status and interrupts --------------------- */

#define PHY_CSR_XPON_STA		0x5e0	/* phy_reg.h:216 */
#define   XPON_STA_LOS			BIT(0)	/* phy_def.h:219 */
/* phy_reg.h:114 TDCSET2 = REG_BASE_1 + 0xf8; pon_phy_scu_reset_init()
 * writes a_tdc_setting, 2 in the stock PHY driver */
#define PHY_CSR_TDCSET2			0x1f8
#define   TDCSET2_DEFAULT		2
#define PHY_CSR_XPON_INT_EN		0x5f0	/* phy_reg.h:222 */
#define PHY_CSR_XPON_INT_STA_CLR	0x5f4	/* phy_reg.h:223 (W1C) */
#define PHY_CSR_XPON_INT_STA		0x5f8	/* phy_reg.h:224 */
#define   PHY_INT_TRANS_LOS		BIT(0)	/* phy_def.h:112 */
#define   PHY_INT_PHY_LOF		BIT(1)	/* phy_def.h:113 */
#define   PHY_INT_TRANS_TF		BIT(2)	/* phy_def.h:114 */
#define   PHY_INT_TRANS			BIT(3)	/* phy_def.h:115 */
#define   PHY_INT_TRANS_SD_FAIL		BIT(4)	/* phy_def.h:116 */
#define   PHY_INT_PHYRDY		BIT(5)	/* phy_def.h:117 */
#define   PHY_INT_PHY_ILLG		BIT(6)	/* phy_def.h:118-119 (A60928) */
#define   PHY_INT_I2CM			BIT(7)	/* phy_def.h:121 */
#define   PHY_INT_SIGDET		BIT(9)	/* phy_isr "transceiver SIGDET" */
#define   PHY_INT_FIELD			GENMASK(7, 0)	/* phy_def.h:122 */
/* LOS | LOF | PHYRDY | ILLG: phy_int_config(99, 0) */
#define   PHY_INT_VENDOR_MASK		0x63

/* ---- ANA window (0x1faf3000) --------------------------------------- */

#define ANA_RG_PCIE_CLKDRV_OFFSET	0x008	/* 0x1faf3008 */
#define   RG_SSUSB_XTAL_TOP_RESERVE	GENMASK(31, 16)
#define ANA_RG_SSUSB_PLL_KBAND_KFC	0x010	/* 0x1faf3010 */
#define   RG_SSUSB_PLL_LDO_VSEL		GENMASK(19, 12)	/* LDOVCO/LPF/CHP/PFD */
#define ANA_RG_SSUSB_PLL_SDM_HREN	0x014	/* 0x1faf3014 */
#define   RG_SSUSB_PLL_RESERVE_B24	BIT(24)	/* RG_SSUSB_PLL_RESERVE[30:23] */
#define ANA_RG_SSUSB_LN0_CDR_RESERVE	0x018	/* 0x1faf3018 */
#define   RG_SSUSB_LN0_CDR_RESERVE	GENMASK(7, 0)
#define   RG_SSUSB_LN0_RXAFE_RESERVE	GENMASK(15, 8)
#define ANA_RG_SSUSB_LN0_CDR_PD_DIV_BYPASS 0x024	/* 0x1faf3024 */
#define   RG_SSUSB_LN0_RX_DAC_PWD	BIT(27)

/* ---- PMA window (0x1faf4000) --------------------------------------- */

#define PMA_SS_LCPLL_PWCTL_SETTING_0	0x100	/* 0x1faf4100 */
#define   RG_TDC_LOS_INV		BIT(29)
#define   RG_INT_LOS_INV		BIT(30)
#define PMA_SS_LCPLL_TDC_PW_0		0x110	/* 0x1faf4110 */
#define   RG_LCPLL_TDC_DIG_PWDB		BIT(0)
#define PMA_SS_LCPLL_TDC_FLT_0		0x128	/* 0x1faf4128 */
#define   RG_LCPLL_PON_RX_CDR_DIVTDC	GENMASK(2, 0)
#define PMA_SS_LCPLL_TDC_FLT_1		0x12c	/* 0x1faf412c */
#define   RG_LCPLL_GPON_SEL		BIT(0)
#define PMA_SS_LCPLL_TDC_FLT_2		0x130	/* 0x1faf4130 */
#define   RG_LCPLL_NCPO_VALUE		GENMASK(30, 0)
#define PMA_SS_LCPLL_TDC_FLT_3		0x134	/* 0x1faf4134 */
#define   RG_LCPLL_NCPO_LOAD		BIT(8)
#define PMA_SS_LCPLL_TDC_FLT_5		0x13c	/* 0x1faf413c */
#define   RG_LCPLL_NCPO_CHG		BIT(24)
#define PMA_SS_LCPLL_TDC_PCW_1		0x148	/* 0x1faf4148 */
#define PMA_SS_LCPLL_TDC_PCW_2		0x14c	/* 0x1faf414c */
#define   RG_LCPLL_PON_HRDDS_PCW_NCPO	GENMASK(30, 0)
#define PMA_SS_LCPLL_TDC_RO_4		0x15c	/* 0x1faf415c, read only */
#define PMA_TDC_HOLD_CTRL_0		0x17c	/* 0x1faf417c */
#define   RG_TDC_LOS_HOLD_MUX		BIT(16)
#define PMA_PON_SERDES_CTRL_0		0x200	/* 0x1faf4200 */
#define   RG_PHYA_AUTO_INIT		BIT(0)
#define   RG_BEN_INV			BIT(24)
#define PMA_PON_SERDES_CTRL_1		0x204	/* 0x1faf4204 */
#define   RG_RX_PWD_MUX			BIT(8)
#define   RG_RX_PWD			BIT(9)
#define PMA_PON_SERDES_CTRL_2		0x208	/* 0x1faf4208 */
#define   RG_TX_DIV_MODE		BIT(8)
#define PMA_PON_SERDES_CTRL_5		0x214	/* 0x1faf4214 */
#define   RG_SIG_DET_WAIT_DLY		GENMASK(31, 16)
#define PMA_PON_SERDES_CTRL_10		0x228	/* 0x1faf4228 */
#define   RG_TX_BIT_POLARITY		BIT(8)
#define PMA_PON_SERDES_CTRL_15		0x244	/* 0x1faf4244, phy_ben_switch */
#define   RG_BEN_FORCE_MODE		BIT(28)
#define   RG_BEN_FORCE_VALUE		BIT(29)
#define   RG_AD_BEN_OUT_CLR		BIT(30)
#define   RG_BEN_FORCE_FIELD		GENMASK(31, 28)
#define PMA_PON_SERDES_CTRL_16		0x248	/* 0x1faf4248 */
#define PMA_PON_SERDES_CTRL_17		0x24c	/* 0x1faf424c */
#define   RG_TX_BIT_START_DLY		GENMASK(7, 0)
#define   RG_TX_BIT_END_DLY		GENMASK(15, 8)
#define PMA_PON_OSR_SEL_CTRL		0x320	/* 0x1faf4320 */
#define   RG_TX_OSR_SEL			GENMASK(1, 0)
#define PMA_CDR_CTRL_0			0x324	/* 0x1faf4324 */
#define PMA_CDR_CTRL_1			0x328	/* 0x1faf4328 */
#define PMA_CDR_CTRL_2			0x32c	/* 0x1faf432c */
#define PMA_CDR_CTRL_3			0x330	/* 0x1faf4330 */
#define PMA_CDR_CTRL_4			0x334	/* 0x1faf4334 */
#define PMA_CDR_CTRL_5			0x338	/* 0x1faf4338 */
#define   CDR_MODE_0			GENMASK(4, 0)	/* rg_b{ir,c}_lt*_mode_0 */
#define   CDR_MODE_1			GENMASK(12, 8)	/* rg_b{ir,c}_lt*_mode_1 */
#define   CDR_BIC_MODE_0		GENMASK(19, 16)	/* rg_bic_lt*_mode_0 */
#define PMA_TX_CTRL_0			0x33c	/* 0x1faf433c */
#define   RG_TX_DLY_DATA_FTUNE_LOW	GENMASK(7, 0)
#define PMA_TX_CTRL_2			0x344	/* 0x1faf4344 */
#define   RG_OUTBEN_DATA_MODE		GENMASK(2, 0)
#define PMA_FRQ_CTRL_2			0x364	/* 0x1faf4364 */
#define   RG_HG_TX_FREQ_DET_MUX		BIT(10)
#define   RG_HG_RX_FREQ_DET_MUX		BIT(11)
#define PMA_FRQ_CTRL_4			0x36c	/* 0x1faf436c */
#define   RG_HG_TX_FREQ_CNT		GENMASK(11, 0)
#define   RG_HG_RX_FREQ_CNT		GENMASK(27, 16)
#define PMA_PON_RXFEDIG_CTRL_0		0x500	/* 0x1faf4500 */
#define   RG_SSUSB_EQ_RX500M_CK_SEL	BIT(12)
#define PMA_PON_RXFEDIG_CTRL_12		0x530	/* 0x1faf4530 */
#define   RG_SSUSB_EQ_REV		GENMASK(31, 16)
#define PMA_PON_DA_CTRL_0		0x600	/* 0x1faf4600 */
#define   RG_SSUSB_PLL_IR		GENMASK(23, 20)
#define   RG_SSUSB_PLL_FBKSEL		GENMASK(13, 12)
#define PMA_PON_DA_CTRL_1		0x604	/* 0x1faf4604 */
#define   RG_SSUSB_PLL_BC		GENMASK(5, 4)
#define PMA_PON_DA_CTRL_2		0x608	/* 0x1faf4608 */
#define   RG_SSUSB_CDR_PI_PWD		BIT(25)
#define PMA_PON_DA_CTRL_4		0x610	/* 0x1faf4610 */
#define   RG_SSUSB_PLL_FBKDIV		GENMASK(8, 0)
#define PMA_PON_DA_CTRL_6		0x618	/* 0x1faf4618 */
#define   RG_FORCE_RX_EQ_EN		BIT(23)
#define PMA_PON_DA_CTRL_19		0x64c	/* 0x1faf464c */
#define   RG_SSUSB_PLL_BPA		GENMASK(3, 0)
#define PMA_PON_DEBUG_REGISTER_14	0x73c	/* 0x1faf473c */
#define PMA_PON_DEBUG_REGISTER_15	0x740	/* 0x1faf4740 */
#define   RG_RECORD_START_DBG_CLR	BIT(8)
#define PMA_PON_DEBUG_REGISTER_20	0x754	/* 0x1faf4754 */
/* phy_ready_handler() / en7523_k_band(); names: pon_hsgmii_dig_csr */
#define PMA_SS_LCPLL_TDC_RO_1		0x150	/* 0x1faf4150 */
#define   RO_LCPLL_TDC_TDCPW_ST		GENMASK(3, 0)
#define PMA_SS_LCPLL_TDC_RO_2		0x154	/* 0x1faf4154 */
#define PMA_SS_LCPLL_TDC_RO_4		0x15c	/* 0x1faf415c, NCPO_dump() */
#define PMA_PON_SYS_CTRL_1		0x304	/* 0x1faf4304 */
#define   RG_TX_RESET_N			BIT(8)
#define   RG_ALL_TX_RST_N		BIT(26)
#define   RG_ALL_RX_RST_N		BIT(27)
#define PMA_PON_DA_CTRL_5		0x614	/* 0x1faf4614 */
#define   RG_FORCE_PLL_AUTOK_VCO	BIT(5)
#define   RG_FORCE_PLL_PWD		BIT(6)
#define   RG_FORCE_CDR_AUTOK_VCO	BIT(27)
#define PMA_PON_DA_CTRL_7		0x61c	/* 0x1faf461c */
#define   RG_SSUSB_PLL_AUTOK_VCO	BIT(26)
#define   RG_SSUSB_PLL_PWD		BIT(27)
#define PMA_PON_DA_CTRL_8		0x620	/* 0x1faf4620 */
#define   RG_SSUSB_CDR_AUTOK_VCO	BIT(21)
#define PMA_PON_DEBUG_REGISTER_2	0x708	/* 0x1faf4708 */
#define PMA_PON_INTERRUPT_CLR_0		0x808	/* 0x1faf4808 */
#define   PMA_INT_CLR_ALL		0xfe	/* phy_isr / xpon_phy_start */
#define   RG_TX_EN_INT_CLR		BIT(7)
#define PMA_PON_INTERRUPT_SET_0		0x80c	/* 0x1faf480c */
#define   RG_ROGUE_EN			BIT(12)
#define PMA_PON_INTERRUPT_SET_1		0x810	/* 0x1faf4810 */
#define   PMA_INT_RO_TX_EN		BIT(7)	/* PON_INTERRUPT_STS_0 ro_tx_en_int */
#define PMA_PON_INTERRUPT_STS_0		0x800	/* 0x1faf4800 */
#define PMA_PON_INTERRUPT_EN_0		0x804	/* 0x1faf4804 */
#define   PMA_INT_FIELD			GENMASK(7, 0)

/* ---- PHY timing constants ---- */

/* Mode_Config_7552 step delay */
#define AN7552_PHY_STEP_DELAY_US	1
/* Mode_Config_7552 after rg_phya_auto_init */
#define AN7552_PHYA_INIT_DELAY_MS	2
/* xPON_Pll_Reset / phy_ready_recover_expires */
#define AN7552_PLL_RESET_DELAY_MS	1
/* xPON_Phy_Reset SCU pulse */
#define AN7552_PHY_RESET_DELAY_US	1
/* pon_phy_scu_reset_init settle after the SCU pulse */
#define AN7552_SCU_RESET_SETTLE_MS	1
/* phy_cnt_enable */
#define AN7552_CNT_ENABLE_DELAY_US	1

/* phy_bit_delay accepts values with (delay & 0xf8) == 0 */
#define AN7552_PHY_BIT_DELAY_MAX	7

/* ---- GPON MAC window (0x1fb64000) ------------------------------------ */

/* ---- GPON MAC registers (offset from 0x1fb64000) ---- */
#define G_ONU_ID			0x000
#define   ONU_ID_MASK			GENMASK(7, 0)
#define   ONU_ID_VLD			BIT(15)
#define G_GBL_CFG			0x004
#define   GBL_SR_BLK_SIZE_MASK		GENMASK(7, 0)
#define   GBL_US_FEC_EN			BIT(16)
#define G_INT_STATUS			0x008
#define G_INT_ENABLE			0x00c
#define   INT_PLOAMD_RECV		BIT(0)
#define   INT_PLOAMU_SEND		BIT(1)
#define   INT_SN_REQ_RECV		BIT(2)
#define   INT_SN_ONU_SEND_O3		BIT(3)
#define   INT_RANGING_REQ_RECV		BIT(4)
#define   INT_SN_ONU_SEND_O4		BIT(5)
#define   INT_SN_REQ_CRS		BIT(6)
#define   INT_LOS_GEM_DEL		BIT(7)
#define   INT_AES_KEY_SWITCH_DONE	BIT(8)
#define   INT_TOD_UPDATE_DONE		BIT(9)
#define   INT_TOD_1PPS			BIT(10)
#define   INT_DYING_GASP_SEND		BIT(11)
#define   INT_SN_REQ_RECV_O5		BIT(12)
#define   INT_CAL_GNT_ZERO		BIT(15)
#define   INT_RX_ERR			BIT(16)
#define   INT_FIFO_ERR			BIT(17)
#define   INT_BST_SGL_DIFF		BIT(18)
#define   INT_TX_LATE_START		BIT(19)
#define   INT_RX_EOF_ERR		BIT(20)
#define   INT_RX_GEM_INTLV_ERR		BIT(21)
#define   INT_BFIFO_FULL		BIT(22)
#define   INT_SFIFO_FULL		BIT(23)
#define   INT_O5_EQD_ADJ_DONE		BIT(24)
#define   INT_OLT_DS_FEC_CHG		BIT(25)
#define   INT_ONU_US_FEC_CHG		BIT(26)
#define   INT_POP_UP_RECV_O6		BIT(27)
#define   INT_FWI			BIT(28)
#define   INT_LWI			BIT(29)
#define   INT_BWM_STOP_TIME_ERR		BIT(30)
#define   INT_BWM_US_FEC_ERR		BIT(31)
/*
 * The GPON INT enable mask, as the vendor's gpon_INT_init() builds it.
 *
 * It is gpon_INT_init() *with* the TCSUPPORT_CPU_EN7521 block, which is this
 * SDK's header for the die.  Three bits differ from the SDK source, two of
 * them knowingly:
 *
 *   9   tod_update_done_int_en   gpon_INT_init() sets it, this does not.
 *       Deliberate: the driver owns the bit and uses it as a one-shot around
 *       a ToD write - an7552_mac_set_tod() arms it, gpon_tod_irq() clears it
 *       once the hardware says the new time is live - so it is not wanted in
 *       a steady-state mask.
 *   15  rsv_12[3]                reserved in gpon_mac_reg_c_header_en7521.h
 *       and set here but not by gpon_INT_init().  Left as found.
 *   31  bwm_us_fec_err_int_en    not set by gpon_INT_init() at all, not even
 *       inside the EN7521 block.  Left as found.
 *
 * Bits 9 and 15 are the two the binary and the source genuinely disagree on, so
 * they are not a transcription of the source either way; the rest is a match.
 * The mask is a value for this SDK generation, and nothing in the stock
 * firmware writes it on this board - in the vendor stack every writer of
 * +0x400c takes the mask as an argument, because it comes from userspace.
 */
#define GPON_INT_ENABLE_DEFAULT		0xf7ff89fd
/* gpon_reg.h: GPON_INT_* groups for TCSUPPORT_CPU_EN7521 */
#define GPON_INT_PLOAM			0x00000003
#define GPON_INT_TOD			0x00000600
#define GPON_INT_ERROR			0xc0ff00c0
#define GPON_INT_INDICATION		0x3f00993c

/* T-CONT 0..15: two per register, 16..31: indirect */
#define G_TCONT_ID(_i)			(0x020 + (((_i) >> 1) << 2))
#define   TCONT_ID_LO_MASK		GENMASK(11, 0)
#define   TCONT_VLD_LO			BIT(15)
#define   TCONT_ID_HI_MASK		GENMASK(27, 16)
#define   TCONT_VLD_HI			BIT(31)
#define G_TCONT_ID_16_31_CFG		0x180
#define   TCONT_WR_ID_MASK		GENMASK(11, 0)
#define   TCONT_ID_INDEX_MASK		GENMASK(19, 16)
#define   TCONT_WR_VLD			BIT(27)
#define   TCONT_CMD_WRITE		BIT(31)
#define G_TCONT_ID_16_31_STS		0x184
#define   TCONT_RD_ID_MASK		GENMASK(11, 0)
#define   TCONT_RD_VLD			BIT(16)
#define   TCONT_CMD_DONE		BIT(31)

#define G_GEM_PORT_CFG			0x040
#define   GEM_PORT_ID_MASK		GENMASK(11, 0)
#define   GEM_PORT_VLD			BIT(16)
#define   GEM_PORT_ENCRYPT		BIT(17)
#define   GEM_PORT_CMD_WRITE		BIT(31)
#define G_GEM_PORT_STS			0x044
#define   GEM_PORT_STS_VLD		BIT(0)
#define   GEM_PORT_STS_ENCRYPT		BIT(1)
#define   GEM_PORT_CMD_DONE		BIT(31)
#define G_OMCI_ID			0x048
#define   OMCI_GPID_MASK		GENMASK(11, 0)
#define   OMCI_PORT_ID_VLD		BIT(16)

#define G_PLOAMU_FIFO_STS		0x050
#define   PLOAMU_FIFO_AVAIL_MASK	GENMASK(7, 0)
#define G_PLOAMU_WDATA			0x054
#define G_PLOAMD_FIFO_STS		0x058
#define   PLOAMD_FIFO_USED_MASK		GENMASK(7, 0)
#define G_PLOAMD_RDATA			0x05c

#define G_AES_CFG			0x060
#define   AES_SPF_CNT_MASK		GENMASK(29, 0)
#define G_AES_ACTIVE_KEY(_n)		(0x064 + ((_n) << 2))
/* shadow key: key[12..15] at 0x74 ... key[0..3] at 0x80 (gponDevSetEncryptKey) */
#define G_AES_SHADOW_KEY(_n)		(0x074 + ((_n) << 2))
#define G_AES_KEY_SWITCH_BY_SW		0x084
#define   AES_KEY_SWITCH_SW_CMD		BIT(0)
#define   AES_KEY_SWITCH_SW_DONE	BIT(31)

#define G_PLOU_GUARD_BIT		0x094
#define   PLOU_GUARD_BIT_MASK		GENMASK(7, 0)
#define G_PLOU_PRMBL_TYPE1_2		0x098
#define   PLOU_PRMB1_MASK		GENMASK(7, 0)
#define   PLOU_PRMB2_MASK		GENMASK(15, 8)
#define G_PLOU_PRMBL_TYPE3		0x09c
#define   PLOU_PRMB3_O3_O4_MASK		GENMASK(7, 0)
#define   PLOU_PRMB3_O5_MASK		GENMASK(15, 8)
#define   PLOU_EBL_EN			BIT(24)
#define G_PLOU_DELM_BIT			0x0a0
#define   PLOU_DELM_BIT_MASK		GENMASK(7, 0)
#define G_PRE_ASSIGNED_DLY		0x0a4
#define   PRE_DLY_MASK			GENMASK(15, 0)
#define   PRE_DLY_EN			BIT(31)
#define G_EQD				0x0a8
#define G_RSP_TIME			0x0ac
#define   RSP_TIME_MASK			GENMASK(15, 0)
#define G_VENDOR_ID			0x0b0
#define G_VS_SN				0x0b4
#define G_ACTIVATION_ST			0x0bc
#define   ACT_ST_MASK			GENMASK(2, 0)
#define G_TOD_CFG			0x0d0
#define   TOD_SPF_CNT_MASK		GENMASK(29, 0)
#define G_NEW_TOD_SEC_L32		0x0d4
#define G_NEW_TOD_NANO_SEC		0x0d8
#define G_CUR_TOD_SEC_L32		0x0dc
#define G_CUR_TOD_NANO_SEC		0x0e0
#define G_TOD_CLK_PERIOD		0x0e4
#define   TOD_PERIOD_MASK		GENMASK(7, 0)

#define G_MIB_CTRL_STS			0x120
#define   MIB_GPID_MASK			GENMASK(11, 0)
#define   MIB_TYPE_MASK			GENMASK(18, 16)
#define   MIB_READ_CLR			BIT(24)
#define   MIB_CMD_DONE			BIT(30)
#define G_MIB_RDATA_L32			0x124
#define G_MIB_RDATA_H32			0x128
#define G_MIB_TBL_INIT			0x134
#define G_GPIDX_TBL_CTRL		0x140
#define   GPIDX_WDATA_MASK		GENMASK(8, 0)
#define   GPIDX_ADDR_MASK		GENMASK(23, 16)
#define   GPIDX_CMD_WRITE		BIT(31)
#define G_GPIDX_TBL_STS			0x144
#define   GPIDX_CMD_DONE		BIT(31)
#define G_GPIDX_TBL_INIT		0x148
#define   TBL_INIT_START		BIT(0)
#define   TBL_INIT_DONE			BIT(8)

#define G_MBI_MPI_STOP			0x160
#define   MBI_RX_STOP			BIT(0)
#define   MBI_RX_STOP_DONE		BIT(1)
#define   DEL_RX_STOP			BIT(4)
#define   MBI_TX_STOP			BIT(8)
#define   MBI_TX_STOP_DONE		BIT(9)
#define   MPI_RX_STOP			BIT(16)
#define   MPI_TX_STOP			BIT(24)
#define   MPI_RX_STOP_DONE		BIT(30)
#define   MPI_TX_STOP_DONE		BIT(31)

#define DBG_CAP_SETTING			0x200
#define   CAP_MAX_RDM_DLY_MASK		GENMASK(11, 0)
#define   CAP_MIB_FRAME_TYPE		BIT(26)
#define DBG_US_PREF_OFS			0x204
#define   US_DT_PREF_OFS_MASK		GENMASK(15, 0)
#define DBG_DLY				0x208
#define   DLY_PHY_TX_MASK		GENMASK(7, 0)
#define   DLY_FINE_INT_MASK		GENMASK(15, 8)
#define DBG_IDLE_GEM_THLD		0x20c
#define   IDLE_GEM_THLD_MASK		GENMASK(15, 0)
#define DBG_US_DYING_GASP_CTRL		0x21c
#define   DG_HW_EN			BIT(8)
#define   DG_NUM_MASK			GENMASK(27, 24)
#define DBG_GRP_0			0x228
#define DBG_GRP_1			0x22c
#define DBG_SW_RDI_CTRL			0x264
#define   RDI_NUM_MASK			GENMASK(31, 16)
#define DBG_RDI_ERR_CTRL		0x268
#define   RDI_ERR_THLD_MASK		GENMASK(26, 0)
#define   RDI_ERR_EN_MASK		GENMASK(31, 27)
#define DBG_RX_GEM_CNT			0x300
#define DBG_RX_CRC_ERR_CNT		0x304
#define DBG_RX_GTC_CNT			0x308
#define DBG_TX_GEM_CNT			0x30c
#define DBG_TX_BST_CNT			0x310
#define DBG_GEM_HEC_ONE_ERR_CNT		0x330
#define DBG_GEM_HEC_TWO_ERR_CNT		0x334
#define DBG_GEM_HEC_UC_ERR_CNT		0x338
/* DBA backdoor (gponDevSetDba*(), REG_DBG_DBA_BACK_DOOR) */
#define DBG_DBA_BACK_DOOR		0x340
#define   DBA_SHIFT_MODIFY_EN		BIT(23)
#define   DBA_SLIGHT_MODIFY_EN		BIT(27)
#define   DBA_SW_RPT_EN			BIT(31)
#define DBG_DBA_TOTAL_BUF_SIZE		0x344
#define DBG_DBA_GREEN_SIZE		0x348
#define DBG_DBA_YELLOW_SIZE		0x34c
/* slight: size; shift: bit 15 direction, [14:0] size */
#define DBG_SLIGHT_MODIFY_SIZE_1	0x350
#define   DBA_MOD_GREEN_MASK		GENMASK(15, 0)
#define   DBA_MOD_YELLOW_MASK		GENMASK(31, 16)
#define DBG_SLIGHT_MODIFY_SIZE_2	0x354
#define   DBA_MOD_TOTAL_MASK		GENMASK(15, 0)
/* gponDevGetSuperframe() on TCSUPPORT_CPU_EN7521; 31 bit of downstream
 * superframes.  The older chips read the same counter through the debug
 * probe (DBG_PROBE_CTRL/HIGH32/LOW32), which this MAC has no need for */
#define DBG_DS_SPF_CNT			0x358
#define   DS_SPF_CNT_MASK		GENMASK(30, 0)
#define DBG_TX_SYNC_OFFSET		0x35c
#define   TX_SYNC_OFFSET_MASK		GENMASK(1, 0)
#define DBG_PLOAMD_FILTER_IN_O5		0x360
#define   FILTER_US_OVERHEAD_O5		BIT(0)
#define   FILTER_EXT_BST_LEN_O5		BIT(8)
#define DBG_SW_RESYNC			0x364
#define   SW_RESYNC			BIT(0)
#define   SW_RESYNC_EN			BIT(31)
#define DBG_GPON_MAC_SET		0x3a0
#define   MAC_SW_RST_N			BIT(0)
#define   MAC_TX_ETH_4BYTE_ALIGN	BIT(31)
#define SLEEP_GLB_CFG			0x3a4
#define   SLEEP_RELOAD_CNT		BIT(30)
#define SLEEP_CNT			0x3a8
#define O3_O4_PLOAMU_CTRL		0x3c4
#define   PLOAMU_IND_CTRL		BIT(8)
#define TOD_1PPS_WD_CTRL		0x3c8	/* in ToD clock periods */
#define CONSID_TX_RX_PH			0x3cc
#define CAL_GNT_SIZE_CTRL_STS		0x3d0
/* ---- NP SCU, MAC view ---- */
#define SCU_WAN_CONF			0x70
#define   SCU_WAN_SEL_MASK		GENMASK(7, 0)
#define   SCU_WAN_SEL_GPON		0x00
#define   SCU_WAN_SEL_GPON_SYM		0x14

/* gpon_wan.c: DISABLE_SN modes for ploam_disable_sn() */
#define DISABLE_SN_DENIED		0xff
#define DISABLE_SN_PARTICIPATE		0x00
#define DISABLE_SN_PARTICIPATE_ALL	0x0f
#define DISABLE_SN_DENIED_ALL		0xf0

/* dying gasp: write 1 to clear (ecnt_scu.c:72) */
#define SCU_DMTC			0x84
#define   SCU_DYING_GASP_STATUS		BIT(16)

#endif /* _AN7552_HW_H */
