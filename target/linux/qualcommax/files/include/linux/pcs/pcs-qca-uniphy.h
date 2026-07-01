/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __QCA_UNIPHY_H
#define __QCA_UNIPHY_H

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/phylink.h>
#include <linux/reset.h>

#define QCA_UNIPHY_CHANNELS		5

#define IPQ5018_UNIPHY_REFCLK		0x74
#define  IPQ5018_UNIPHY_REFCLK_EN	BIT(0)
#define  IPQ5018_UNIPHY_REFCLK_DIV	BIT(1)	/* 0 (default): 50MHz, 1: 25MHz */
#define  IPQ5018_UNIPHY_REFCLK_DS	GENMASK(3, 2) /* 0: 2.8V, 1 (default): 1.5V */

#define UNIPHY_OFFSET_CALIB_4		0x1e0
#define   UNIPHY_CALIBRATION_DONE	BIT(7)

#define UNIPHY_MISC2_PHY_MODE		0x218
#define   UNIPHY_MISC2_PHY_MODE_MASK	GENMASK(6, 4)
#define     UNIPHY_MISC2_SGMII		FIELD_PREP_CONST(UNIPHY_MISC2_PHY_MODE_MASK, 0x3)
#define     UNIPHY_MISC2_SGMIIPLUS	FIELD_PREP_CONST(UNIPHY_MISC2_PHY_MODE_MASK, 0x5)
#define     UNIPHY_MISC2_USXGMII	FIELD_PREP_CONST(UNIPHY_MISC2_PHY_MODE_MASK, 0x7)

#define UNIPHY_MODE_CTRL		0x46c
#define   UNIPHY_MODE_SEL_MASK		GENMASK(12, 8)
#define   UNIPHY_XPCS_MODE		BIT(12)
#define   UNIPHY_SGPLUS_MODE		BIT(11)
#define   UNIPHY_SGMII_MODE		BIT(10)
#define   UNIPHY_CH0_PSGMII_QSGMII	BIT(9)
#define   UNIPHY_CH0_QSGMII_SGMII	BIT(8)
#define   UNIPHY_CH0_MODE_CTRL_25M	GENMASK(6, 4)
#define   UNIPHY_CH0_MODE_1000BASEX	0
#define   UNIPHY_CH0_MODE_MAC		2
#define   UNIPHY_AUTONEG_MODE_ATH	BIT(0)

#define UNIPHY_PLL_POWER_ON_AND_RESET	0x780
#define   UNIPHY_PLL_RESET_ANALOG	BIT(6)

#define UNIPHY_CH_BASE(ch)		(0x480 + (ch) * 0x18)

#define UNIPHY_CH_CTRL(ch)		(UNIPHY_CH_BASE(ch) + 0x0)
#define   UNIPHY_CH_SPEED_MODE		GENMASK(2, 1)
#define   UNIPHY_CH_FORCE_MODE		BIT(3)
#define   UNIPHY_CH_AN_ENABLE		BIT(6)
#define   UNIPHY_CH_AN_RESTART		BIT(7)
#define   UNIPHY_CH_ADP_SW_RSTN		BIT(11)

#define UNIPHY_CH_STS(ch)		(UNIPHY_CH_BASE(ch) + 0x8)
#define   UNIPHY_CH_STS_RX_PAUSE	BIT(0)
#define   UNIPHY_CH_STS_TX_PAUSE	BIT(1)
#define   UNIPHY_CH_STS_SPEED_MODE	GENMASK(5, 4)
#define   UNIPHY_CH_STS_DUPLEX		BIT(6)
#define   UNIPHY_CH_STS_LINK		BIT(7)

#define XPCS_INDIRECT_ADDR		0x8000
#define XPCS_INDIRECT_AHB_ADDR		0x83fc
#define XPCS_INDIRECT_ADDR_H		GENMASK(20, 8)
#define XPCS_INDIRECT_ADDR_L		GENMASK(7, 0)
#define XPCS_INDIRECT_DATA_ADDR(reg)	(FIELD_PREP(GENMASK(15, 10), 0x20) | \
					 FIELD_PREP(GENMASK(9, 2), \
					 FIELD_GET(XPCS_INDIRECT_ADDR_L, reg)))

#define XPCS_KR_STS1			0x30020
#define XPCS_KR_STS1_PLU		BIT(12)

#define XPCS_DIG_CTRL			0x38000
#define XPCS_SOFT_RESET			BIT(15)
#define XPCS_USXG_ADPT_RESET		BIT(10)
#define XPCS_USXG_EN			BIT(9)

#define XPCS_MII_CTRL			0x1f0000
#define XPCS_MII1_CTRL(x)		(0x1a0000 + 0x10000 * ((x) - 1))
#define XPCS_MII_AN_EN			BIT(12)
#define XPCS_DUPLEX_FULL		BIT(8)
#define XPCS_SPEED_MASK			(BIT(13) | BIT(6) | BIT(5))
#define XPCS_SPEED_10000		(BIT(13) | BIT(6))
#define XPCS_SPEED_5000			(BIT(13) | BIT(5))
#define XPCS_SPEED_2500			BIT(5)
#define XPCS_SPEED_1000			BIT(6)
#define XPCS_SPEED_100			BIT(13)
#define XPCS_SPEED_10			0

#define XPCS_MII_AN_CTRL		0x1f8001
#define XPCS_MII1_AN_CTRL(x)		(0x1a8001 + 0x10000 * ((x) - 1))
#define XPCS_MII_AN_8BIT		BIT(8)

#define XPCS_MII_AN_INTR_STS		0x1f8002
#define XPCS_MII1_AN_INTR_STS(x)	(0x1a8002 + 0x10000 * ((x) - 1))
#define XPCS_USXG_AN_LINK_STS		BIT(14)
#define XPCS_USXG_AN_SPEED_MASK		GENMASK(12, 10)
#define XPCS_USXG_AN_SPEED_10		0
#define XPCS_USXG_AN_SPEED_100		1
#define XPCS_USXG_AN_SPEED_1000		2
#define XPCS_USXG_AN_SPEED_2500		4
#define XPCS_USXG_AN_SPEED_5000		5
#define XPCS_USXG_AN_SPEED_10000	3

#define UNIPHY_CALIBRATION_TIMEOUT_US	100000
#define UNIPHY_CALIBRATION_POLL_US	1000

enum qca_uniphy_type {
	UNIPHY_IPQ5018,
	UNIPHY_IPQ6018,
	UNIPHY_IPQ8074,
};

struct qca_uniphy;

struct qca_uniphy_clk {
	struct clk_hw hw;
	struct qca_uniphy *uniphy;
};

struct qca_uniphy_pcs {
	struct phylink_pcs pcs;
	struct qca_uniphy *uniphy;
	int channel;
};

struct qca_uniphy_match_data {
	enum qca_uniphy_type uniphy_type;
	bool ref_clk_enable;
};

#define to_qca_uniphy_pcs(n) container_of(pcs, struct qca_uniphy_pcs, pcs);

struct qca_uniphy {
	struct device *dev;
	void __iomem *base;
	struct regmap *regmap;
	struct reset_control *rst_ahb;
	struct reset_control *rst_soft;
	struct reset_control *rst_xpcs;
	struct clk_bulk_data *clks;
	int num_clks;
	struct qca_uniphy_pcs port_pcs[QCA_UNIPHY_CHANNELS];
	struct qca_uniphy_clk rx_clk;
	struct qca_uniphy_clk tx_clk;
	struct qca_uniphy_clk ref_clk;
	const struct qca_uniphy_match_data *data;
	phy_interface_t interface;
};

#define port_rx_clk_idx(upcs)	((upcs)->channel * 2) + 2
#define port_tx_clk_idx(upcs)	(((upcs)->channel * 2) + 1) + 2

#endif
