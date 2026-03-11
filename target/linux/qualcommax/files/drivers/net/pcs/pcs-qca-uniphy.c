// SPDX-License-Identifier: GPL-2.0-only
/*
 * Qualcomm IPQ6018 UNIPHY PCS driver
 *
 * Copyright (c) 2025 The OpenWrt Project
 *
 * Standalone PCS driver for the UNIPHY SerDes blocks in IPQ6018 SoCs.
 * Each UNIPHY instance provides up to 5 SGMII channels (PSGMII mode)
 * or a single channel for SGMII/USXGMII.
 */

#include <linux/bitfield.h>
#include <linux/clk/clk-conf.h>
#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/pcs/pcs-provider.h>
#include <linux/pcs/pcs-qca-uniphy.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#include <dt-bindings/net/qca-uniphy.h>

static const struct qca_uniphy_match_data ipq5018_data = {
	.uniphy_type	= UNIPHY_IPQ5018,
	.ref_clk_enable	= true,
};

static const struct qca_uniphy_match_data ipq6018_data = {
	.uniphy_type	= UNIPHY_IPQ6018,
	.ref_clk_enable	= false,
};

static const struct qca_uniphy_match_data ipq8074_data = {
	.uniphy_type	= UNIPHY_IPQ8074,
	.ref_clk_enable	= false,
};

static unsigned long
qca_uniphy_clk_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct qca_uniphy_clk *uclk =
		container_of(hw, struct qca_uniphy_clk, hw);
	struct qca_uniphy *uniphy = uclk->uniphy;
	u32 val;

	/*
	 * UNIPHY switch reference clock based on the configured
	 * PHY mode.
	 */
	regmap_read(uniphy->regmap, UNIPHY_MODE_CTRL, &val);
	if (val & UNIPHY_SGMII_MODE ||
	    val & UNIPHY_CH0_PSGMII_QSGMII ||
	    val & UNIPHY_CH0_QSGMII_SGMII)
		return 125000000;
	else if (val & UNIPHY_SGPLUS_MODE ||
		 val & UNIPHY_XPCS_MODE)
		return 312500000;

	return 0;
}

static const struct clk_ops qca_uniphy_clk_ops = {
	.recalc_rate = qca_uniphy_clk_recalc_rate,
};

static int qca_uniphy_clk_register(struct qca_uniphy *uniphy,
				   struct qca_uniphy_clk *uclk,
				   const char *name)
{
	struct clk_init_data init = {
		.name = name,
		.ops = &qca_uniphy_clk_ops,
		/* always derive rate from the UNIPHY register */
		.flags = CLK_GET_RATE_NOCACHE,
	};

	uclk->hw.init = &init;
	uclk->uniphy = uniphy;

	return devm_clk_hw_register(uniphy->dev, &uclk->hw);
}

static int
qca_uniphy_refclk_enable(struct clk_hw *hw)
{
	struct qca_uniphy_clk *uclk =
		container_of(hw, struct qca_uniphy_clk, hw);
	struct qca_uniphy *uniphy = uclk->uniphy;

	switch (uniphy->data->uniphy_type) {
	case UNIPHY_IPQ5018:
		if (!clk_hw_is_enabled(&uclk->hw)) {
			regmap_update_bits(uniphy->regmap, IPQ5018_UNIPHY_REFCLK,
					IPQ5018_UNIPHY_REFCLK_EN |
					IPQ5018_UNIPHY_REFCLK_DS,
					IPQ5018_UNIPHY_REFCLK_EN |
					FIELD_PREP(IPQ5018_UNIPHY_REFCLK_DS, 1));
		}
		break;
	default:
		break;
	}

	return 0;
}

static int
qca_uniphy_refclk_is_enabled(struct clk_hw *hw)
{
	struct qca_uniphy_clk *uclk =
		container_of(hw, struct qca_uniphy_clk, hw);
	struct qca_uniphy *uniphy = uclk->uniphy;
	u32 val;

	switch (uniphy->data->uniphy_type) {
	case UNIPHY_IPQ5018:
		regmap_read(uniphy->regmap, IPQ5018_UNIPHY_REFCLK, &val);
		return !!(val & IPQ5018_UNIPHY_REFCLK_EN);
	default:
		break;
	}

	return 0;
}

static int
qca_uniphy_refclk_determine_rate(struct clk_hw *hw,
				 struct clk_rate_request *req)
{
	struct qca_uniphy_clk *uclk =
		container_of(hw, struct qca_uniphy_clk, hw);
	struct qca_uniphy *uniphy = uclk->uniphy;

	switch (uniphy->data->uniphy_type) {
	case UNIPHY_IPQ5018:
		switch (req->rate) {
		case UNIPHY_REFCLK_25MHZ:
		case UNIPHY_REFCLK_50MHZ:
			return 0;
		default:
			return -EINVAL;
		}
	default:
		return 0;
	}
}

static unsigned long
qca_uniphy_refclk_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct qca_uniphy_clk *uclk =
		container_of(hw, struct qca_uniphy_clk, hw);
	struct qca_uniphy *uniphy = uclk->uniphy;
	u32 val;

	switch (uniphy->data->uniphy_type) {
	case UNIPHY_IPQ5018:
		regmap_read(uniphy->regmap, IPQ5018_UNIPHY_REFCLK, &val);
		if (val & IPQ5018_UNIPHY_REFCLK_DIV)
			return UNIPHY_REFCLK_25MHZ;
		else
			return UNIPHY_REFCLK_50MHZ;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int
qca_uniphy_refclk_set_rate(struct clk_hw *hw, unsigned long rate,
			   unsigned long parent_rate)
{
	struct qca_uniphy_clk *refclk =
		container_of(hw, struct qca_uniphy_clk, hw);
	struct qca_uniphy *uniphy = refclk->uniphy;
	u32 val = 0;

	switch (uniphy->data->uniphy_type) {
	case UNIPHY_IPQ5018:
		if (parent_rate != UNIPHY_REFCLK_50MHZ)
			return -EINVAL;

		if (rate == UNIPHY_REFCLK_50MHZ)
			val = FIELD_PREP(IPQ5018_UNIPHY_REFCLK_DIV, 0x0);
		else if (rate == UNIPHY_REFCLK_25MHZ)
			val = FIELD_PREP(IPQ5018_UNIPHY_REFCLK_DIV, 0x1);

		regmap_update_bits(uniphy->regmap, IPQ5018_UNIPHY_REFCLK,
				   IPQ5018_UNIPHY_REFCLK_DIV, val);
		break;
	default:
		return 0;
	}

	return 0;
}

static const struct clk_ops qca_uniphy_refclk_ops = {
	.enable = qca_uniphy_refclk_enable,
	.determine_rate = qca_uniphy_refclk_determine_rate,
	.recalc_rate = qca_uniphy_refclk_recalc_rate,
	.set_rate = qca_uniphy_refclk_set_rate,
};

static int qca_uniphy_refclk_register(struct qca_uniphy *uniphy,
				      const char *name) {
	struct clk_parent_data pdata = { .fw_name = "ref" };
	struct clk_init_data init = {};

	init.name = name,
	init.parent_data = &pdata;
	init.num_parents = 1;
	init.ops = &qca_uniphy_refclk_ops,
	/* always derive rate from the UNIPHY register */
	init.flags = CLK_GET_RATE_NOCACHE,

	uniphy->ref_clk.hw.init = &init;
	uniphy->ref_clk.uniphy = uniphy;

	return devm_clk_hw_register(uniphy->dev, &uniphy->ref_clk.hw);
}

static int qca_uniphy_register_clks(struct qca_uniphy *uniphy) {
	struct clk_hw_onecell_data *hw_data;
	struct device *dev = uniphy->dev;
	const char *name;
	int num_clks;
	int ret;

	num_clks = (uniphy->data->ref_clk_enable) ? 3 : 2;
	hw_data = devm_kzalloc(dev, struct_size(hw_data, hws, num_clks),
			       GFP_KERNEL);
	if (!hw_data)
		return -ENOMEM;

	if (of_property_read_string_index(dev->of_node, "clock-output-names",
					  UNIPHY_CLK_RX, &name))
		return -ENODEV;

	ret = qca_uniphy_clk_register(uniphy, &uniphy->rx_clk, name);
	if (ret)
		return ret;
	hw_data->hws[UNIPHY_CLK_RX] = &uniphy->rx_clk.hw;

	if (of_property_read_string_index(dev->of_node, "clock-output-names",
					  UNIPHY_CLK_TX, &name))
		return -ENODEV;

	ret = qca_uniphy_clk_register(uniphy, &uniphy->tx_clk, name);
	if (ret)
		return ret;
	hw_data->hws[UNIPHY_CLK_TX] = &uniphy->tx_clk.hw;

	if (uniphy->data->ref_clk_enable) {
		if (of_property_read_string_index(dev->of_node,
						  "clock-output-names",
						  UNIPHY_CLK_REF, &name))
			return -ENODEV;

		ret = qca_uniphy_refclk_register(uniphy, name);
		if (ret)
			return ret;
		hw_data->hws[UNIPHY_CLK_REF] = &uniphy->ref_clk.hw;
	}

	hw_data->num = num_clks;

	return devm_of_clk_add_hw_provider(dev, of_clk_hw_onecell_get, hw_data);
}

static unsigned int
qca_uniphy_pcs_inband_caps(struct phylink_pcs *pcs,
			   phy_interface_t interface)
{
	return LINK_INBAND_DISABLE | LINK_INBAND_ENABLE;
}

static void qca_uniphy_pcs_get_state_sgmii(struct qca_uniphy *uniphy,
					   int channel,
					   struct phylink_link_state *state)
{
	u32 val;

	regmap_read(uniphy->regmap,
		    UNIPHY_CH_STS(channel),
		    &val);

	state->link = !!(val & UNIPHY_CH_STS_LINK);
	if (!state->link)
		return;

	state->duplex = (val & UNIPHY_CH_STS_DUPLEX) ? DUPLEX_FULL : DUPLEX_HALF;

	switch (FIELD_GET(UNIPHY_CH_STS_SPEED_MODE, val)) {
	case 0:
		state->speed = SPEED_10;
		break;
	case 1:
		state->speed = SPEED_100;
		break;
	case 2:
		state->speed = SPEED_1000;
		break;
	default:
		state->link = false;
		return;
	}

	state->pause = 0;
	if (val & UNIPHY_CH_STS_RX_PAUSE)
		state->pause |= MLO_PAUSE_RX;
	if (val & UNIPHY_CH_STS_TX_PAUSE)
		state->pause |= MLO_PAUSE_TX;

	state->an_complete = state->link;
}

static void qca_uniphy_pcs_get_state_hsgmii(struct qca_uniphy *uniphy,
					    int channel,
					    struct phylink_link_state *state)
{
	u32 val;

	regmap_read(uniphy->regmap,
		    UNIPHY_CH_STS(channel),
		    &val);

	state->link = !!(val & UNIPHY_CH_STS_LINK);
	if (!state->link)
		return;

	state->speed = SPEED_2500;
	state->duplex = DUPLEX_FULL;

	state->pause = 0;
	if (val & UNIPHY_CH_STS_RX_PAUSE)
		state->pause |= MLO_PAUSE_RX;
	if (val & UNIPHY_CH_STS_TX_PAUSE)
		state->pause |= MLO_PAUSE_TX;

	state->an_complete = state->link;
}

static void qca_uniphy_pcs_get_state_usxgmii(struct qca_uniphy *uniphy,
					     struct phylink_link_state *state)
{
	unsigned int val;
	int ret;

	ret = regmap_read(uniphy->regmap, XPCS_MII_AN_INTR_STS, &val);
	if (ret) {
		state->link = 0;
		return;
	}

	state->link = !!(val & XPCS_USXG_AN_LINK_STS);

	if (!state->link)
		return;

	switch (FIELD_GET(XPCS_USXG_AN_SPEED_MASK, val)) {
	case XPCS_USXG_AN_SPEED_10000:
		state->speed = SPEED_10000;
		break;
	case XPCS_USXG_AN_SPEED_5000:
		state->speed = SPEED_5000;
		break;
	case XPCS_USXG_AN_SPEED_2500:
		state->speed = SPEED_2500;
		break;
	case XPCS_USXG_AN_SPEED_1000:
		state->speed = SPEED_1000;
		break;
	case XPCS_USXG_AN_SPEED_100:
		state->speed = SPEED_100;
		break;
	case XPCS_USXG_AN_SPEED_10:
		state->speed = SPEED_10;
		break;
	default:
		state->link = false;
		return;
	}

	state->duplex = DUPLEX_FULL;
}

static void qca_uniphy_pcs_get_state_10base_r(struct qca_uniphy *uniphy,
					      struct phylink_link_state *state)
{
	unsigned int val;
	int ret;

	ret = regmap_read(uniphy->regmap, XPCS_KR_STS1, &val);
	if (ret) {
		state->link = 0;
		return;
	}

	state->link = !!(val & XPCS_KR_STS1_PLU);

	if (!state->link)
		return;

	state->speed = SPEED_10000;
	state->duplex = DUPLEX_FULL;
}

static void qca_uniphy_pcs_get_state(struct phylink_pcs *pcs,
				     struct phylink_link_state *state)
{
	struct qca_uniphy_pcs *upcs = to_qca_uniphy_pcs(pcs);
	struct qca_uniphy *uniphy = upcs->uniphy;

	switch (state->interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_PSGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
		qca_uniphy_pcs_get_state_sgmii(uniphy, upcs->channel,
					       state);
		break;
	case PHY_INTERFACE_MODE_2500BASEX:
		qca_uniphy_pcs_get_state_hsgmii(uniphy, upcs->channel,
						state);
		break;
	case PHY_INTERFACE_MODE_USXGMII:
		qca_uniphy_pcs_get_state_usxgmii(uniphy, state);
		break;
	case PHY_INTERFACE_MODE_10GBASER:
		qca_uniphy_pcs_get_state_10base_r(uniphy, state);
		break;
	default:
		break;
	}
}

static int qca_uniphy_pcs_config_mode(struct phylink_pcs *pcs,
				      unsigned int neg_mode,
				      phy_interface_t interface,
				      const unsigned long *advertising,
				      bool permit_pause_to_mac)
{
	struct qca_uniphy_pcs *upcs = to_qca_uniphy_pcs(pcs);
	struct qca_uniphy *uniphy = upcs->uniphy;
	u32 misc2_phy_mode, mode_ctrl, val;
	int ret;

	dev_dbg(uniphy->dev, "Configuring PCS: chan=%d, interface=%s, neg_mode=0x%x\n",
		upcs->channel, phy_modes(interface), neg_mode);

	switch (interface) {
	case PHY_INTERFACE_MODE_1000BASEX:
		misc2_phy_mode = UNIPHY_MISC2_SGMII;
		mode_ctrl = UNIPHY_SGMII_MODE;
		break;
	case PHY_INTERFACE_MODE_SGMII:
		misc2_phy_mode = UNIPHY_MISC2_SGMII;
		mode_ctrl = UNIPHY_SGMII_MODE;
		mode_ctrl |= FIELD_PREP(UNIPHY_CH0_MODE_CTRL_25M, UNIPHY_CH0_MODE_MAC);
		mode_ctrl |= UNIPHY_AUTONEG_MODE_ATH;
		break;
	case PHY_INTERFACE_MODE_QSGMII:
		mode_ctrl = UNIPHY_CH0_QSGMII_SGMII;
		misc2_phy_mode = UNIPHY_MISC2_SGMII;
		break;
	case PHY_INTERFACE_MODE_PSGMII:
		mode_ctrl = UNIPHY_CH0_PSGMII_QSGMII;
		misc2_phy_mode = 0;
		break;
	case PHY_INTERFACE_MODE_USXGMII:
		mode_ctrl = UNIPHY_XPCS_MODE;
		misc2_phy_mode = UNIPHY_MISC2_USXGMII;
		break;
	case PHY_INTERFACE_MODE_10GBASER:
		mode_ctrl = UNIPHY_XPCS_MODE;
		misc2_phy_mode = 0;
		break;
	case PHY_INTERFACE_MODE_2500BASEX:
		misc2_phy_mode = UNIPHY_MISC2_SGMIIPLUS;
		mode_ctrl = UNIPHY_SGPLUS_MODE;
		mode_ctrl |= FIELD_PREP(UNIPHY_CH0_MODE_CTRL_25M, UNIPHY_CH0_MODE_MAC);
		mode_ctrl |= UNIPHY_AUTONEG_MODE_ATH;
		break;
	default:
		return -EINVAL;
	}

	/*
	* The clock could have been enabled by the bootloader.
	* Increase enable count (>0) CCF can properly account for clock state.
	*/
	if (uniphy->data->ref_clk_enable) {
		if (!clk_hw_is_enabled(&uniphy->ref_clk.hw) &&
		    qca_uniphy_refclk_is_enabled(&uniphy->ref_clk.hw))
			clk_prepare_enable(uniphy->ref_clk.hw.clk);
	}

	/* First update misc2 PHY mode... */
	regmap_update_bits(uniphy->regmap, UNIPHY_MISC2_PHY_MODE,
			   UNIPHY_MISC2_PHY_MODE_MASK, misc2_phy_mode);

	/* ...and reset Analog */
	regmap_clear_bits(uniphy->regmap, UNIPHY_PLL_POWER_ON_AND_RESET,
			  UNIPHY_PLL_RESET_ANALOG);
	msleep(100);
	regmap_set_bits(uniphy->regmap, UNIPHY_PLL_POWER_ON_AND_RESET,
			UNIPHY_PLL_RESET_ANALOG);
	msleep(100);

	/* Second assert XPCS... */
	if (uniphy->rst_xpcs)
		reset_control_assert(uniphy->rst_xpcs);

	/* ...and disable PHY clock */
	clk_disable(uniphy->clks[port_rx_clk_idx(upcs)].clk);
	clk_disable(uniphy->clks[port_tx_clk_idx(upcs)].clk);

	/* TODO: Fix for IPQ6018 and IPQ8074 */
	if (uniphy->data->uniphy_type == UNIPHY_IPQ5018) {
		//set force mode for fixed link
		if (neg_mode == PHYLINK_PCS_NEG_OUTBAND && !phylink_expects_phy(pcs->phylink)) {
			regmap_set_bits(uniphy->regmap,
					UNIPHY_CH_CTRL(upcs->channel),
					UNIPHY_CH_FORCE_MODE);
		}
	}

	/* Third update the mode ctrl... */
	regmap_update_bits(uniphy->regmap, UNIPHY_MODE_CTRL,
			   UNIPHY_MODE_SEL_MASK | UNIPHY_CH0_MODE_CTRL_25M |
			   UNIPHY_AUTONEG_MODE_ATH,
			   mode_ctrl);

	/*
	 * ...and execute soft reset...
	 *
	 * (soft reset is XPCS + all the UNIPHY PHY port reset,
	 * XPCS gets indirectly deassert by the soft reset deassert.
	 * It seems that resetting all at once is mandatory as from
	 * lots of testing it has been verified that operating
	 * on the single reset is problematic)
	 */
	reset_control_assert(uniphy->rst_soft);
	msleep(100);
	reset_control_deassert(uniphy->rst_soft);

	/* ...and wait for calibration */
	ret = regmap_read_poll_timeout(uniphy->regmap, UNIPHY_OFFSET_CALIB_4,
				       val, val & UNIPHY_CALIBRATION_DONE,
				       UNIPHY_CALIBRATION_POLL_US,
				       UNIPHY_CALIBRATION_TIMEOUT_US);
	if (ret) {
		dev_err(uniphy->dev, "PCS calibration timeout\n");
		return -EINVAL;
	}

	/* Trigger UNIPHY ref clock to recal rate */
	clk_hw_recalc_rate(&uniphy->rx_clk.hw);
	clk_hw_recalc_rate(&uniphy->tx_clk.hw);

	/* As last step enable PHY clock... */
	clk_enable(uniphy->clks[port_rx_clk_idx(upcs)].clk);
	clk_enable(uniphy->clks[port_tx_clk_idx(upcs)].clk);

	if (interface == PHY_INTERFACE_MODE_USXGMII ||
	    interface == PHY_INTERFACE_MODE_10GBASER)
		reset_control_deassert(uniphy->rst_xpcs);

	if (!uniphy->data->ref_clk_enable)
		return 0;

	/* Trigger UNIPHY ref output clock to recalc rate */
	clk_hw_recalc_rate(&uniphy->ref_clk.hw);
	/* Trigger assigned-clock-rates in DT to be applied */
	of_clk_set_defaults(uniphy->dev->of_node, true);
	if (!qca_uniphy_refclk_is_enabled(&uniphy->ref_clk.hw))
		qca_uniphy_refclk_enable(&uniphy->ref_clk.hw);

	return 0;
}

static int qca_uniphy_pcs_config_usxgmii(struct phylink_pcs *pcs,
					 unsigned int neg_mode,
					 phy_interface_t interface,
					 const unsigned long *advertising,
					 bool permit_pause_to_mac)
{
	struct qca_uniphy_pcs *upcs = to_qca_uniphy_pcs(pcs);
	struct qca_uniphy *uniphy = upcs->uniphy;
	int ret;

	ret = qca_uniphy_pcs_config_mode(pcs, neg_mode, interface, advertising, permit_pause_to_mac);
	if (ret)
		return ret;

	ret = regmap_update_bits(uniphy->regmap, XPCS_DIG_CTRL,
				 XPCS_USXG_EN,
				 interface == PHY_INTERFACE_MODE_USXGMII ? XPCS_USXG_EN : 0);
	if (ret)
		return ret;

	ret = regmap_update_bits(uniphy->regmap, XPCS_MII_AN_CTRL,
				 XPCS_MII_AN_8BIT,
				 interface == PHY_INTERFACE_MODE_USXGMII ? XPCS_MII_AN_8BIT : 0);
	if (ret)
		return ret;

	return regmap_update_bits(uniphy->regmap, XPCS_MII_CTRL,
				  XPCS_MII_AN_EN,
				  interface == PHY_INTERFACE_MODE_USXGMII ? XPCS_MII_AN_EN : 0);
}

static int qca_uniphy_pcs_config(struct phylink_pcs *pcs,
				 unsigned int neg_mode,
				 phy_interface_t interface,
				 const unsigned long *advertising,
				 bool permit_pause_to_mac)
{
	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_PSGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
		return qca_uniphy_pcs_config_mode(pcs, neg_mode, interface, advertising, permit_pause_to_mac);
	case PHY_INTERFACE_MODE_USXGMII:
	case PHY_INTERFACE_MODE_10GBASER:
		return qca_uniphy_pcs_config_usxgmii(pcs, neg_mode, interface, advertising, permit_pause_to_mac);
	default:
		return -EOPNOTSUPP;
	}
}

static int uniphy_link_up_sgmii(struct phylink_pcs *pcs,
				phy_interface_t interface,
				int speed)
{
	struct qca_uniphy_pcs *upcs = to_qca_uniphy_pcs(pcs);
	struct qca_uniphy *uniphy = upcs->uniphy;
	unsigned long uniphy_rate;
	int ret;

	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_PSGMII:
		switch (speed) {
		case SPEED_10:
			uniphy_rate = 2500000;
			break;
		case SPEED_100:
			uniphy_rate = 25000000;
			break;
		case SPEED_1000:
			uniphy_rate = 125000000;
			break;
		default:
			dev_err(uniphy->dev, "Invalid SGMII speed %d\n", speed);
			return -EINVAL;
		}
		break;
	case PHY_INTERFACE_MODE_1000BASEX:
		switch (speed) {
		case SPEED_1000:
			uniphy_rate = 125000000;
			break;
		default:
			dev_err(uniphy->dev, "Invalid 1000BaseX speed %d\n", speed);
			return -EINVAL;
		}
		break;
	case PHY_INTERFACE_MODE_2500BASEX:
		switch (speed) {
		case SPEED_2500:
			uniphy_rate = 312500000;
			break;
		default:
			dev_err(uniphy->dev, "Invalid 2500BaseX speed %d\n", speed);
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	clk_set_rate(uniphy->clks[port_rx_clk_idx(upcs)].clk, uniphy_rate);
	clk_set_rate(uniphy->clks[port_tx_clk_idx(upcs)].clk, uniphy_rate);

	ret = regmap_clear_bits(uniphy->regmap, UNIPHY_CH_CTRL(upcs->channel),
				UNIPHY_CH_ADP_SW_RSTN);
	if (ret)
		return ret;

	usleep_range(1000, 2000);

	return regmap_set_bits(uniphy->regmap, UNIPHY_CH_CTRL(upcs->channel),
			       UNIPHY_CH_ADP_SW_RSTN);
}

static int uniphy_link_up_usxgmii(struct phylink_pcs *pcs, int speed)
{
	struct qca_uniphy_pcs *upcs = to_qca_uniphy_pcs(pcs);
	struct qca_uniphy *uniphy = upcs->uniphy;
	unsigned int val, uniphy_rate;
	int ret;

	switch (speed) {
	case SPEED_10000:
		val = XPCS_SPEED_10000;
		uniphy_rate = 312500000;
		break;
	case SPEED_5000:
		val = XPCS_SPEED_5000;
		uniphy_rate = 156250000;
		break;
	case SPEED_2500:
		val = XPCS_SPEED_2500;
		uniphy_rate = 78125000;
		break;
	case SPEED_1000:
		val = XPCS_SPEED_1000;
		uniphy_rate = 125000000;
		break;
	case SPEED_100:
		val = XPCS_SPEED_100;
		uniphy_rate = 12500000;
		break;
	case SPEED_10:
		val = XPCS_SPEED_10;
		uniphy_rate = 1250000;
		break;
	default:
		dev_err(uniphy->dev, "Invalid USXGMII speed %d\n", speed);
		return -EINVAL;
	}

	clk_set_rate(uniphy->clks[port_rx_clk_idx(upcs)].clk, uniphy_rate);
	clk_set_rate(uniphy->clks[port_tx_clk_idx(upcs)].clk, uniphy_rate);

	/* Configure XPCS speed */
	ret = regmap_update_bits(uniphy->regmap, XPCS_MII_CTRL,
				 XPCS_SPEED_MASK, val | XPCS_DUPLEX_FULL);
	if (ret)
		return ret;

	/* XPCS adapter reset */
	return regmap_set_bits(uniphy->regmap, XPCS_DIG_CTRL, XPCS_USXG_ADPT_RESET);
}

static void qca_uniphy_pcs_link_up(struct phylink_pcs *pcs,
				   unsigned int neg_mode,
				   phy_interface_t interface,
				   int speed, int duplex)
{
	struct qca_uniphy_pcs *upcs = to_qca_uniphy_pcs(pcs);
	struct qca_uniphy *uniphy = upcs->uniphy;
	int ret = 0;

	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_PSGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
		ret = uniphy_link_up_sgmii(pcs, interface, speed);
		break;
	case PHY_INTERFACE_MODE_USXGMII:
	case PHY_INTERFACE_MODE_10GBASER:
		ret = uniphy_link_up_usxgmii(pcs, speed);
		break;
	default:
		return;
	}

	if (ret)
		dev_err(uniphy->dev, "PCS link up fail for interface %s\n",
			phy_modes(interface));
}

static int qca_uniphy_pcs_validate(struct phylink_pcs *pcs, unsigned long *supported,
			    const struct phylink_link_state *state)
{
	switch (state->interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_QSGMII:
	case PHY_INTERFACE_MODE_PSGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
	case PHY_INTERFACE_MODE_USXGMII:
	case PHY_INTERFACE_MODE_10GBASER:
		return 0;
	default:
		return -EINVAL;
	}
}

static void qca_uniphy_pcs_disable(struct phylink_pcs *pcs)
{
	struct qca_uniphy_pcs *upcs = to_qca_uniphy_pcs(pcs);
	struct qca_uniphy *uniphy = upcs->uniphy;

	if (uniphy->data->uniphy_type == UNIPHY_IPQ5018) {
		clk_disable(uniphy->clks[port_rx_clk_idx(upcs)].clk);
		clk_disable(uniphy->clks[port_tx_clk_idx(upcs)].clk);
	}
}

static int qca_uniphy_pcs_enable(struct phylink_pcs *pcs)
{
	struct qca_uniphy_pcs *upcs = to_qca_uniphy_pcs(pcs);
	struct qca_uniphy *uniphy = upcs->uniphy;
	int ret;

	if (uniphy->data->uniphy_type == UNIPHY_IPQ5018) {
		ret = clk_enable(uniphy->clks[port_rx_clk_idx(upcs)].clk);
		if (ret) {
			dev_err(uniphy->dev, "Failed to enable RX clock for channel %d\n",
				upcs->channel);
			return ret;
		}
		ret = clk_enable(uniphy->clks[port_tx_clk_idx(upcs)].clk);
		if (ret) {
			dev_err(uniphy->dev, "Failed to enable TX clock for channel %d\n",
				upcs->channel);
			return ret;
		}
	}

	return 0;
}

static void qca_uniphy_pcs_an_restart(struct phylink_pcs *pcs) { }

static const struct phylink_pcs_ops qca_uniphy_pcs_ops = {
	.pcs_validate = qca_uniphy_pcs_validate,
	.pcs_inband_caps = qca_uniphy_pcs_inband_caps,
	.pcs_get_state = qca_uniphy_pcs_get_state,
	.pcs_config = qca_uniphy_pcs_config,
	.pcs_link_up = qca_uniphy_pcs_link_up,
	.pcs_disable = qca_uniphy_pcs_disable,
	.pcs_enable = qca_uniphy_pcs_enable,
	.pcs_an_restart = qca_uniphy_pcs_an_restart,
};

static const struct of_device_id qca_uniphy_of_match[] = {
	{ .compatible = "qualcomm,ipq5018-uniphy", .data = &ipq5018_data },
	{ .compatible = "qualcomm,ipq6018-uniphy", .data = &ipq6018_data },
	{ .compatible = "qualcomm,ipq8074-uniphy", .data = &ipq8074_data },
	{ },
};
MODULE_DEVICE_TABLE(of, qca_uniphy_of_match);

static int uniphy_pcs_regmap_read(void *context, unsigned int reg,
				  unsigned int *val)
{
	struct qca_uniphy *uniphy = context;

	/* PCS uses direct AHB access while XPCS uses indirect AHB access */
	if (reg >= XPCS_INDIRECT_ADDR) {
		writel(FIELD_GET(XPCS_INDIRECT_ADDR_H, reg),
		       uniphy->base + XPCS_INDIRECT_AHB_ADDR);
		*val = readl(uniphy->base + XPCS_INDIRECT_DATA_ADDR(reg));
	} else {
		*val = readl(uniphy->base + reg);
	}

	return 0;
}

static int uniphy_pcs_regmap_write(void *context, unsigned int reg,
				   unsigned int val)
{
	struct qca_uniphy *uniphy = context;

	/* PCS uses direct AHB access while XPCS uses indirect AHB access */
	if (reg >= XPCS_INDIRECT_ADDR) {
		writel(FIELD_GET(XPCS_INDIRECT_ADDR_H, reg),
		       uniphy->base + XPCS_INDIRECT_AHB_ADDR);
		writel(val, uniphy->base + XPCS_INDIRECT_DATA_ADDR(reg));
	} else {
		writel(val, uniphy->base + reg);
	}

	return 0;
}

static struct phylink_pcs *qca_uniphy_get(struct fwnode_reference_args *pcsspec,
					  void *data)
{
	struct qca_uniphy *uniphy = data;
	struct device *dev = uniphy->dev;
	int channel = 0;

	if (pcsspec->nargs > 1) {
		dev_err(dev, "Invalid number of cells in 'pcs-handle' property\n");
		return ERR_PTR(-EINVAL);
	}

	if (pcsspec->nargs)
		channel = pcsspec->args[0];

	if (channel >= QCA_UNIPHY_CHANNELS) {
		dev_err(dev, "Invalid channel in 'pcs-handle' property\n");
		return ERR_PTR(-EINVAL);
	}

	return &uniphy->port_pcs[channel].pcs;
}

static const struct regmap_config uniphy_regmap_cfg = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_read = uniphy_pcs_regmap_read,
	.reg_write = uniphy_pcs_regmap_write,
	.fast_io = true,
};

static int qca_uniphy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct clk_bulk_data *clks;
	struct qca_uniphy *uniphy;
	int ret, i, num_clks;

	uniphy = devm_kzalloc(dev, sizeof(*uniphy), GFP_KERNEL);
	if (!uniphy)
		return -ENOMEM;

	uniphy->dev = dev;

	uniphy->data = device_get_match_data(dev);
	if (!uniphy->data)
		return -EINVAL;

	uniphy->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(uniphy->base))
		return dev_err_probe(dev, PTR_ERR(uniphy->base),
				     "Failed to ioremap resource");

	uniphy->regmap = devm_regmap_init(dev, NULL, uniphy, &uniphy_regmap_cfg);
	if (IS_ERR(uniphy->regmap))
		return dev_err_probe(dev, PTR_ERR(uniphy->regmap),
				     "Failed to init regmap");

	uniphy->rst_soft = devm_reset_control_get_exclusive(dev, "soft");
	if (IS_ERR(uniphy->rst_soft))
		return dev_err_probe(dev, PTR_ERR(uniphy->rst_soft),
				     "Failed to get soft reset\n");

	uniphy->rst_xpcs = devm_reset_control_get_optional_exclusive(dev, "xpcs");
	if (IS_ERR(uniphy->rst_xpcs))
		return dev_err_probe(dev, PTR_ERR(uniphy->rst_xpcs),
				     "Failed to get xpcs reset\n");

	num_clks = devm_clk_bulk_get_all_enabled(dev, &clks);
	if (num_clks < 0)
		return dev_err_probe(dev, num_clks, "Failed to get clocks\n");

	uniphy->clks = clks;
	uniphy->num_clks = num_clks;

	ret = qca_uniphy_register_clks(uniphy);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to register output clocks\n");

	for (i = 0; i < QCA_UNIPHY_CHANNELS; i++) {
		uniphy->port_pcs[i].pcs.ops = &qca_uniphy_pcs_ops;
		uniphy->port_pcs[i].pcs.neg_mode = true;
		uniphy->port_pcs[i].pcs.poll = true;
		uniphy->port_pcs[i].uniphy = uniphy;
		uniphy->port_pcs[i].channel = i;
	}

	platform_set_drvdata(pdev, uniphy);

	return fwnode_pcs_add_provider(dev_fwnode(dev), qca_uniphy_get,
				       uniphy);
}

static struct platform_driver qca_uniphy_driver = {
	.driver = {
		.name			= "qca-uniphy",
		.suppress_bind_attrs	= true,
		.of_match_table		= qca_uniphy_of_match,
	},
	.probe = qca_uniphy_probe,
};
module_platform_driver(qca_uniphy_driver);

MODULE_DESCRIPTION("Qualcomm IPQ6018 UNIPHY PCS driver");
MODULE_LICENSE("GPL");
