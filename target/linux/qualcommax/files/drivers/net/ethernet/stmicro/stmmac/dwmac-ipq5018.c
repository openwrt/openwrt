// SPDX-License-Identifier: GPL-2.0-only
/*
 * Qualcomm IPQ5018 DWMAC glue layer
 *
 * Copyright (C) 2026 The OpenWrt Project
 */

#include <linux/clk.h>
#include <linux/of_mdio.h>
#include <linux/pcs/pcs.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/stmmac.h>

#include "stmmac_platform.h"

#define MAX_FRAME_SIZE	16383	/* 14 bits */
/* MAX_MTU = (MAX_FRAME_SIZE - ETH_HLEN - ETH_FCS_LEN - (2 * VLAN_HLEN)) */
#define MAX_MTU		16357

static struct clk_bulk_data ipq5018_gmac_clks[] = {
	{ .id = "stmmaceth" },
	{ .id = "pclk" },
	{ .id = "ptp_ref" },
	{ .id = "ahb" },
	{ .id = "axi" },
	{ .id = "rx" },
	{ .id = "tx" },
};

struct ipq5018_gmac {
	struct device *dev;
	struct clk *rx_clk;
	struct clk *tx_clk;
};

static void ipq5018_gmac_fix_speed(void *priv, unsigned int speed, unsigned int mode)
{
	struct ipq5018_gmac *gmac = priv;
	unsigned long rate;

	switch(speed) {
		case SPEED_10:
			rate = 2500000;
			break;
		case SPEED_100:
			rate = 25000000;
			break;
		case SPEED_1000:
			rate = 125000000;
			break;
		case SPEED_2500:
			rate = 312500000;
			break;
		default:
			dev_err(gmac->dev, "Unsupported speed: %d\n", speed);
			rate = 125000000;
			break;
	}

	clk_set_rate(gmac->rx_clk, rate);
	clk_set_rate(gmac->tx_clk, rate);
}

static int ipq5018_gmac_pcs_init(struct stmmac_priv *priv)
{
	struct phylink_pcs *pcs;
	int ret;

	pcs = fwnode_pcs_get(dev_fwnode(priv->device), 0);
	if (!pcs)
		return 0;
	else if (IS_ERR(pcs)) {
		ret = PTR_ERR(pcs);
		if (ret == -ENOENT)
			return 0;
		dev_err(priv->device, "failed to parse PCS from fwnode\n");
		return ret;
	}

	priv->hw->phylink_pcs = pcs;

	return 0;
}

static struct phylink_pcs *ipq5018_gmac_select_pcs(struct stmmac_priv *priv,
						   phy_interface_t interface)
{
	switch (interface) {
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_2500BASEX:
		if (priv->hw->phylink_pcs)
			return priv->hw->phylink_pcs;
	default:
		break;
	}

	return NULL;
}

static void ipq5018_gmac_get_interfaces(struct stmmac_priv *priv, void *bsp_priv,
				    unsigned long *interfaces)
{
	struct mac_device_info *mac = priv->hw;

	/*
	 * IPQ5018 has two GMACs:
	 * - GMAC0 supports SGMII and is wired to the SoC's internal GE PHY
	 * - GMAC1 supports SGMII and 2500BaseX, configurable by the UNIPHY PCS
	 */
	__set_bit(PHY_INTERFACE_MODE_SGMII, interfaces);
	if (priv->hw->phylink_pcs) {
		__set_bit(PHY_INTERFACE_MODE_2500BASEX, interfaces);

		/* 
		 * Synopsys DWMAC IP version 3.7 is limited to 1 Gpbs.
		 * This vendor specific implementation supports 2.5 Gbps, so override
	 	 * the default mac link capabilities.
	 	 */
		mac->link.caps |= MAC_2500FD;
	}
}

static int ipq5018_gmac_probe(struct platform_device *pdev)
{
	struct plat_stmmacenet_data *plat_dat;
	struct stmmac_resources stmmac_res;
	struct device *dev = &pdev->dev;
	struct ipq5018_gmac *gmac;
	int ret;

	ret = stmmac_get_platform_resources(pdev, &stmmac_res);
	if (ret)
		return dev_err_probe(dev, ret,
				     "failed to get stmmac platform resources\n");

	plat_dat = devm_stmmac_probe_config_dt(pdev, stmmac_res.mac);
	if (IS_ERR_OR_NULL(plat_dat))
		return dev_err_probe(dev, PTR_ERR(plat_dat),
				     "failed to parse stmmac dt parameters\n");

	gmac = devm_kzalloc(dev, sizeof(*gmac), GFP_KERNEL);
	if (!gmac)
		return dev_err_probe(dev, -ENOMEM,
				     "failed to allocate priv\n");

	gmac->dev = dev;

	gmac->rx_clk = devm_clk_get(dev, "rx");
	if (IS_ERR(gmac->rx_clk))
		return dev_err_probe(dev, PTR_ERR(gmac->rx_clk),
				     "failed to get RX clock\n");

	gmac->tx_clk = devm_clk_get(dev, "tx");
	if (IS_ERR(gmac->tx_clk))
		return dev_err_probe(dev, PTR_ERR(gmac->tx_clk),
				     "failed to get TX clock\n");
	
	ret = devm_clk_bulk_get(dev, ARRAY_SIZE(ipq5018_gmac_clks),
				ipq5018_gmac_clks);
	if (ret)
		return dev_err_probe(dev, ret, "failed to get clocks\n");

	ret = clk_bulk_prepare_enable(ARRAY_SIZE(ipq5018_gmac_clks),
						 ipq5018_gmac_clks);
	if (ret)
		return dev_err_probe(dev, ret, "failed to enable clocks\n");

	plat_dat->bsp_priv = gmac;
	plat_dat->max_speed = 2500;
	plat_dat->maxmtu = MAX_MTU;
	plat_dat->rx_fifo_size = MAX_FRAME_SIZE;
	plat_dat->tx_fifo_size = MAX_FRAME_SIZE;
	plat_dat->get_interfaces = ipq5018_gmac_get_interfaces;
	plat_dat->fix_mac_speed = ipq5018_gmac_fix_speed;
	plat_dat->pcs_init = ipq5018_gmac_pcs_init;
	plat_dat->select_pcs = ipq5018_gmac_select_pcs;

	return stmmac_dvr_probe(dev, plat_dat, &stmmac_res);
}

static const struct of_device_id ipq5018_gmac_dwmac_match[] = {
	{ .compatible = "qcom,ipq5018-gmac" },
	{ }
};
MODULE_DEVICE_TABLE(of, ipq5018_gmac_dwmac_match);

static struct platform_driver ipq5018_gmac_dwmac_driver = {
	.probe = ipq5018_gmac_probe,
	.driver = {
		.name		= "ipq5018-gmac-dwmac",
		.of_match_table	= ipq5018_gmac_dwmac_match,
	},
};
module_platform_driver(ipq5018_gmac_dwmac_driver);

MODULE_DESCRIPTION("Qualcomm IPQ5018 DWMAC glue layer");
MODULE_LICENSE("GPL");