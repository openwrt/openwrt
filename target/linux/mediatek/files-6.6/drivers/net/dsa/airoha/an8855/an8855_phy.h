// SPDX-License-Identifier:     GPL-2.0+
/*
 * Common part for Airoha AN8855 gigabit switch
 *
 * Copyright (C) 2023 Airoha Inc. All Rights Reserved.
 *
 * Author: Min Yao <min.yao@airoha.com>
 */

#ifndef _AN8855_PHY_H_
#define _AN8855_PHY_H_

#include <linux/bitops.h>

#define AN8855_NUM_PHYS	5

/*phy calibration use*/
#define DEV_1E				0x1E
/*global device 0x1f, always set P0*/
#define DEV_1F				0x1F

/************IEXT/REXT CAL***************/
/* bits range: for example BITS(16,23) = 0xFF0000*/
#define BITS(m, n)	 (~(BIT(m) - 1) & ((BIT(n) - 1) | BIT(n)))
#define ANACAL_INIT			0x01
#define ANACAL_ERROR			0xFD
#define ANACAL_SATURATION		0xFE
#define	ANACAL_FINISH			0xFF
#define ANACAL_PAIR_A			0
#define ANACAL_PAIR_B			1
#define ANACAL_PAIR_C			2
#define ANACAL_PAIR_D			3
#define DAC_IN_0V			0x00
#define DAC_IN_2V			0xf0
#define TX_AMP_OFFSET_0MV		0x20
#define TX_AMP_OFFSET_VALID_BITS	6

#define R0				0
#define PHY0				0
#define PHY1				1
#define PHY2				2
#define PHY3				3
#define PHY4				4
#define ANA_TEST_MODE			BITS(8, 15)
#define TST_TCLK_SEL			BITs(6, 7)
#define ANA_TEST_VGA_RG			0x100

#define FORCE_MDI_CROSS_OVER		BITS(3, 4)
#define T10_TEST_CTL_RG			0x145
#define RG_185				0x185
#define RG_TX_SLEW			BIT(0)
#define ANA_CAL_0			0xdb
#define RG_CAL_CKINV			BIT(12)
#define RG_ANA_CALEN			BIT(8)
#define RG_REXT_CALEN			BIT(4)
#define RG_ZCALEN_A			BIT(0)
#define ANA_CAL_1			0xdc
#define RG_ZCALEN_B			BIT(12)
#define RG_ZCALEN_C			BIT(8)
#define RG_ZCALEN_D			BIT(4)
#define RG_TXVOS_CALEN			BIT(0)
#define ANA_CAL_6			0xe1
#define RG_CAL_REFSEL			BIT(4)
#define RG_CAL_COMP_PWD			BIT(0)
#define ANA_CAL_5			0xe0
#define RG_REXT_TRIM			BITs(8, 13)
#define RG_ZCAL_CTRL			BITs(0, 5)
#define RG_17A				0x17a
#define AD_CAL_COMP_OUT			BIT(8)
#define RG_17B				0x17b
#define AD_CAL_CLK			bit(0)
#define RG_17C				0x17c
#define DA_CALIN_FLAG			bit(0)
/************R50 CAL****************************/
#define RG_174				0x174
#define RG_R50OHM_RSEL_TX_A_EN		BIT[15]
#define CR_R50OHM_RSEL_TX_A		BITS[8:14]
#define RG_R50OHM_RSEL_TX_B_EN		BIT[7]
#define CR_R50OHM_RSEL_TX_B		BITS[6:0]
#define RG_175				0x175
#define RG_R50OHM_RSEL_TX_C_EN		BITS[15]
#define CR_R50OHM_RSEL_TX_C		BITS[8:14]
#define RG_R50OHM_RSEL_TX_D_EN		BIT[7]
#define CR_R50OHM_RSEL_TX_D		BITS[0:6]
/**********TX offset Calibration***************************/
#define RG_95				0x96
#define BYPASS_TX_OFFSET_CAL		BIT(15)
#define RG_3E				0x3e
#define BYPASS_PD_TXVLD_A		BIT(15)
#define BYPASS_PD_TXVLD_B		BIT(14)
#define BYPASS_PD_TXVLD_C		BIT(13)
#define BYPASS_PD_TXVLD_D		BIT(12)
#define BYPASS_PD_TX_10M		BIT(11)
#define POWER_DOWN_TXVLD_A		BIT(7)
#define POWER_DOWN_TXVLD_B		BIT(6)
#define POWER_DOWN_TXVLD_C		BIT(5)
#define POWER_DOWN_TXVLD_D		BIT(4)
#define POWER_DOWN_TX_10M		BIT(3)
#define RG_DD				0xdd
#define RG_TXG_CALEN_A			BIT(12)
#define RG_TXG_CALEN_B			BIT(8)
#define RG_TXG_CALEN_C			BIT(4)
#define RG_TXG_CALEN_D			BIT(0)
#define RG_17D				0x17D
#define FORCE_DASN_DAC_IN0_A		BIT(15)
#define DASN_DAC_IN0_A			BITS(0, 9)
#define RG_17E				0x17E
#define FORCE_DASN_DAC_IN0_B		BIT(15)
#define DASN_DAC_IN0_B			BITS(0, 9)
#define RG_17F				0x17F

#define FORCE_DASN_DAC_IN0_C		BIT(15)
#define DASN_DAC_IN0_C			BITS(0, 9)
#define RG_180				0x180
#define FORCE_DASN_DAC_IN0_D		BIT(15)
#define DASN_DAC_IN0_D			BITS(0, 9)

#define RG_181				0x181
#define FORCE_DASN_DAC_IN1_A		BIT(15)
#define DASN_DAC_IN1_A			BITS(0, 9)
#define RG_182				0x182
#define FORCE_DASN_DAC_IN1_B		BIT(15)
#define DASN_DAC_IN1_B			BITS(0, 9)
#define RG_183				0x183
#define FORCE_DASN_DAC_IN1_C		BIT(15)
#define DASN_DAC_IN1_C			BITS(0, 9)
#define RG_184				0x184
#define FORCE_DASN_DAC_IN1_D		BIT(15)
#define DASN_DAC_IN1_D			BITS(0, 9)
#define RG_172				0x172
#define CR_TX_AMP_OFFSET_A		BITS(8, 13)
#define CR_TX_AMP_OFFSET_B		BITS(0, 5)
#define RG_173				0x173
#define CR_TX_AMP_OFFSET_C		BITS(8, 13)
#define CR_TX_AMP_OFFSET_D		BITS(0, 5)
/**********TX Amp Calibration ***************************/
#define RG_12				0x12
#define DA_TX_I2MPB_A_GBE		BITS(10, 15)
#define RG_17				0x17
#define DA_TX_I2MPB_B_GBE		BITS(8, 13)
#define RG_19				0x19
#define DA_TX_I2MPB_C_GBE		BITS(8, 13)
#define RG_21				0x21
#define DA_TX_I2MPB_D_GBE		BITS(8, 13)
#define TX_AMP_MAX			0x3f
#define TX_AMP_MAX_OFFSET		0xb
#define TX_AMP_HIGHEST_TS		((TX_AMP_MAX) + 3)
#define TX_AMP_LOWEST_TS		(0 - 3)
#define TX_AMP_HIGH_TS			(TX_AMP_MAX)
#define TX_AMP_LOW_TS			0

/* PHY Extend Register 0x14 bitmap of define */
#define PHY_EXT_REG_14			0x14

/* Fields of PHY_EXT_REG_14 */
#define PHY_EN_DOWN_SHFIT		BIT(4)

/* PHY Extend Register 0x17 bitmap of define */
#define PHY_EXT_REG_17			0x17

/* Fields of PHY_EXT_REG_17 */
#define PHY_LINKDOWN_POWER_SAVING_EN	BIT(4)

/* PHY PMA Register 0x17 bitmap of define */
#define SLV_DSP_READY_TIME_S		15
#define SLV_DSP_READY_TIME_M		(0xff << SLV_DSP_READY_TIME_S)

/* PHY PMA Register 0x18 bitmap of define */
#define ENABLE_RANDOM_UPDATE_TRIGGER	BIT(8)

/* PHY EEE Register bitmap of define */
#define PHY_DEV07			0x07
#define PHY_DEV07_REG_03C		0x3c
#define ADV_EEE_100			0x2
#define ADV_EEE_1000		0x4

/* PHY DEV 0x1e Register bitmap of define */
#define PHY_DEV1E			0x1e
/* PHY TX PAIR DELAY SELECT Register */
#define PHY_TX_PAIR_DLY_SEL_GBE		0x013
/* PHY ADC Register */
#define PHY_RXADC_CTRL				0x0d8
#define PHY_RXADC_REV_0				0x0d9
#define PHY_RXADC_REV_1				0x0da

/* PHY LED Register bitmap of define */
#define PHY_LED_CTRL_SELECT		0x3e8
#define PHY_SINGLE_LED_ON_CTRL(i)	(0x3e0 + ((i) * 2))
#define PHY_SINGLE_LED_BLK_CTRL(i)	(0x3e1 + ((i) * 2))
#define PHY_SINGLE_LED_ON_DUR(i)	(0x3e9 + ((i) * 2))
#define PHY_SINGLE_LED_BLK_DUR(i)	(0x3ea + ((i) * 2))

#define PHY_PMA_CTRL	(0x340)

#define PHY_DEV1F			0x1f

#define PHY_LED_ON_CTRL(i)		(0x24 + ((i) * 2))
#define LED_ON_EN				(1 << 15)
#define LED_ON_POL				(1 << 14)
#define LED_ON_EVT_MASK			(0x7f)

/* LED ON Event */
#define LED_ON_EVT_FORCE		(1 << 6)
#define LED_ON_EVT_LINK_HD		(1 << 5)
#define LED_ON_EVT_LINK_FD		(1 << 4)
#define LED_ON_EVT_LINK_DOWN	(1 << 3)
#define LED_ON_EVT_LINK_10M		(1 << 2)
#define LED_ON_EVT_LINK_100M	(1 << 1)
#define LED_ON_EVT_LINK_1000M	(1 << 0)

#define PHY_LED_BLK_CTRL(i)		(0x25 + ((i) * 2))
#define LED_BLK_EVT_MASK		(0x3ff)
/* LED Blinking Event */
#define LED_BLK_EVT_FORCE			(1 << 9)
#define LED_BLK_EVT_10M_RX_ACT		(1 << 5)
#define LED_BLK_EVT_10M_TX_ACT		(1 << 4)
#define LED_BLK_EVT_100M_RX_ACT		(1 << 3)
#define LED_BLK_EVT_100M_TX_ACT		(1 << 2)
#define LED_BLK_EVT_1000M_RX_ACT	(1 << 1)
#define LED_BLK_EVT_1000M_TX_ACT	(1 << 0)

#define PHY_LED_BCR				(0x21)
#define LED_BCR_EXT_CTRL		(1 << 15)
#define LED_BCR_CLK_EN			(1 << 3)
#define LED_BCR_TIME_TEST		(1 << 2)
#define LED_BCR_MODE_MASK		(3)
#define LED_BCR_MODE_DISABLE	(0)

#define PHY_LED_ON_DUR			(0x22)
#define LED_ON_DUR_MASK			(0xffff)

#define PHY_LED_BLK_DUR			(0x23)
#define LED_BLK_DUR_MASK		(0xffff)

#define PHY_LED_BLINK_DUR_CTRL	(0x720)

/* Proprietory Control Register of Internal Phy device 0x1e */
#define PHY_TX_MLT3_BASE		0x0
#define PHY_DEV1E_REG_13		0x13
#define PHY_DEV1E_REG_14		0x14
#define PHY_DEV1E_REG_41		0x41
#define PHY_DEV1E_REG_A6		0xa6
#define RXADC_CONTROL_3			0xc2
#define PHY_DEV1E_REG_0C6		0xc6
#define RXADC_LDO_CONTROL_2		0xd3
#define PHY_DEV1E_REG_0FE		0xfe
#define PHY_DEV1E_REG_123		0x123
#define PHY_DEV1E_REG_189		0x189
#define PHY_DEV1E_REG_234		0x234

/* Proprietory Control Register of Internal Phy device 0x1f */
#define PHY_DEV1F_REG_44		0x44
#define PHY_DEV1F_REG_268		0x268
#define PHY_DEV1F_REG_269		0x269
#define PHY_DEV1F_REG_26A		0x26A
#define TXVLD_DA_271			0x271
#define TXVLD_DA_272			0x272
#define TXVLD_DA_273			0x273

/* Fields of PHY_DEV1E_REG_0C6 */
#define PHY_POWER_SAVING_S		8
#define PHY_POWER_SAVING_M		0x300
#define PHY_POWER_SAVING_TX		0x0

/* Fields of PHY_DEV1E_REG_189 */
#define DESCRAMBLER_CLEAR_EN		0x1

/* Fields of PHY_DEV1E_REG_234 */
#define TR_OPEN_LOOP_EN			BIT(0)

/* Internal GPHY Page Control Register */
#define PHY_CL22_PAGE_CTRL		0x1f
#define PHY_TR_PAGE			0x52b5

/* Internal GPHY Token Ring Access Registers */
#define PHY_TR_CTRL			0x10
#define PHY_TR_LOW_DATA			0x11
#define PHY_TR_HIGH_DATA		0x12

/* Fields of PHY_TR_CTRL */
#define PHY_TR_PKT_XMT_STA		BIT(15)
#define PHY_TR_WR_S			13
#define PHY_TR_CH_ADDR_S		11
#define PHY_TR_NODE_ADDR_S		7
#define PHY_TR_DATA_ADDR_S		1

enum phy_tr_wr {
	PHY_TR_WRITE = 0,
	PHY_TR_READ = 1,
};

/* Helper macro for GPHY Token Ring Access */
#define PHY_TR_LOW_VAL(x)		((x) & 0xffff)
#define PHY_TR_HIGH_VAL(x)		(((x) & 0xff0000) >> 16)

/* Token Ring Channels */
#define PMA_CH				0x1
#define DSP_CH				0x2

/* Token Ring Nodes */
#define PMA_NOD				0xf
#define DSP_NOD				0xd

/* Token Ring register range */
enum tr_pma_reg_addr {
	PMA_MIN = 0x0,
	PMA_01 = 0x1,
	PMA_17 = 0x17,
	PMA_18 = 0x18,
	PMA_MAX = 0x3d,
};

enum tr_dsp_reg_addr {
	DSP_MIN = 0x0,
	DSP_06 = 0x6,
	DSP_08 = 0x8,
	DSP_0f = 0xf,
	DSP_10 = 0x10,
	DSP_MAX = 0x3e,
};
#endif /* _AN8855_REGS_H_ */
