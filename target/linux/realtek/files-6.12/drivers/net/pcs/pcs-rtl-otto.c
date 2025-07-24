// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/mdio.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/phylink.h>
#include <linux/regmap.h>

#define RTPCS_PORT_CNT				57

#define RTPCS_SPEED_10				0
#define RTPCS_SPEED_100				1
#define RTPCS_SPEED_1000			2
#define RTPCS_SPEED_10000_LEGACY		3
#define RTPCS_SPEED_10000			4
#define RTPCS_SPEED_2500			5
#define RTPCS_SPEED_5000			6

#define RTPCS_838X_CPU_PORT			28
#define RTPCS_838X_MAC_LINK_DUP_STS		0xa19c
#define RTPCS_838X_MAC_LINK_SPD_STS		0xa190
#define RTPCS_838X_MAC_LINK_STS			0xa188
#define RTPCS_838X_MAC_RX_PAUSE_STS		0xa1a4
#define RTPCS_838X_MAC_TX_PAUSE_STS		0xa1a0

#define RTPCS_839X_CPU_PORT			52
#define RTPCS_839X_MAC_LINK_DUP_STS		0x03b0
#define RTPCS_839X_MAC_LINK_SPD_STS		0x03a0
#define RTPCS_839X_MAC_LINK_STS			0x0390
#define RTPCS_839X_MAC_RX_PAUSE_STS		0x03c0
#define RTPCS_839X_MAC_TX_PAUSE_STS		0x03b8

#define RTPCS_83XX_MAC_LINK_SPD_BITS		2

#define RTPCS_930X_CPU_PORT			28
#define RTPCS_930X_MAC_LINK_DUP_STS		0xcb28
#define RTPCS_930X_MAC_LINK_SPD_STS		0xcb18
#define RTPCS_930X_MAC_LINK_STS			0xcb10
#define RTPCS_930X_MAC_RX_PAUSE_STS		0xcb30
#define RTPCS_930X_MAC_TX_PAUSE_STS		0xcb2c

#define RTPCS_931X_CPU_PORT			56
#define RTPCS_931X_MAC_LINK_DUP_STS		0x0ef0
#define RTPCS_931X_MAC_LINK_SPD_STS		0x0ed0
#define RTPCS_931X_MAC_LINK_STS			0x0ec0
#define RTPCS_931X_MAC_RX_PAUSE_STS		0x0f00
#define RTPCS_931X_MAC_TX_PAUSE_STS		0x0ef8

#define RTPCS_93XX_MAC_LINK_SPD_BITS		4

struct rtpcs_ctrl {
	struct device *dev;
	struct regmap *map;
	struct mii_bus *bus;
	const struct rtpcs_config *cfg;
	struct rtpcs_link *link[RTPCS_PORT_CNT];
	struct mutex lock;
};

struct rtpcs_link {
	struct rtpcs_ctrl *ctrl;
	struct phylink_pcs pcs;
	int sds;
	int port;
};

struct rtpcs_config {
	int cpu_port;
	int mac_link_dup_sts;
	int mac_link_spd_bits;
	int mac_link_spd_sts;
	int mac_link_sts;
	int mac_rx_pause_sts;
	int mac_tx_pause_sts;
	const struct phylink_pcs_ops *pcs_ops;
	int (*set_autoneg)(struct rtpcs_ctrl *ctrl, int sds, unsigned int neg_mode);
};

static int rtpcs_sds_to_mmd(int sds_page, int sds_regnum)
{
	return (sds_page << 8) + sds_regnum;
}

static int rtpcs_sds_read(struct rtpcs_ctrl *ctrl, int sds, int page, int regnum)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);

	return mdiobus_c45_read(ctrl->bus, sds, MDIO_MMD_VEND1, mmd_regnum);
}

/*
 * For later use, when the SerDes registers need to be written ...
 *
 * static int rtpcs_sds_write(struct rtpcs_ctrl *ctrl, int sds, int page, int regnum, u16 value)
 * {
 *	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);
 *
 *	return mdiobus_c45_write(ctrl->bus, sds, MDIO_MMD_VEND1, mmd_regnum, value);
 * }
 */

static int rtpcs_sds_modify(struct rtpcs_ctrl *ctrl, int sds, int page, int regnum,
			    u16 mask, u16 set)
{
	int mmd_regnum = rtpcs_sds_to_mmd(page, regnum);

	return mdiobus_c45_modify(ctrl->bus, sds, MDIO_MMD_VEND1, mmd_regnum,
				  mask, set);
}

static int rtpcs_regmap_read_bits(struct rtpcs_ctrl *ctrl, int base, int bithigh, int bitlow)
{
	int offset = base + (bitlow / 32) * 4;
	int bits = bithigh + 1 - bitlow;
	int shift = bitlow % 32;
	int value;

	regmap_read(ctrl->map, offset, &value);
	value = (value >> shift) & (BIT(bits) - 1);

	return value;
}

static struct rtpcs_link *rtpcs_phylink_pcs_to_link(struct phylink_pcs *pcs)
{
	return container_of(pcs, struct rtpcs_link, pcs);
}

static void rtpcs_pcs_get_state(struct phylink_pcs *pcs, struct phylink_link_state *state)
{
	struct rtpcs_link *link = rtpcs_phylink_pcs_to_link(pcs);
	struct rtpcs_ctrl *ctrl = link->ctrl;
	int port = link->port;
	int linkup, speed;

	state->link = 0;
	state->speed = SPEED_UNKNOWN;
	state->duplex = DUPLEX_UNKNOWN;
	state->pause &= ~(MLO_PAUSE_RX | MLO_PAUSE_TX);

	/* Read MAC side link twice */
	for (int i = 0; i < 2; i++)
		linkup = rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_link_sts, port, port);

	if (!linkup)
		return;

	state->link = 1;
	state->duplex = rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_link_dup_sts, port, port);

	speed = rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_link_spd_sts,
				       ctrl->cfg->mac_link_spd_bits * (port + 1) - 1,
				       ctrl->cfg->mac_link_spd_bits * port);
	switch (speed) {
	case RTPCS_SPEED_10:
		state->speed = SPEED_10;
		break;
	case RTPCS_SPEED_100:
		state->speed = SPEED_100;
		break;
	case RTPCS_SPEED_1000:
		state->speed = SPEED_1000;
		break;
	case RTPCS_SPEED_10000:
	case RTPCS_SPEED_10000_LEGACY:
		/*
		 * The legacy mode is ok so far with minor inconsistencies. On RTL838x this flag
		 * is either 500M or 2G. It might be that MAC_GLITE_STS register tells more. On
		 * RTL839x this is either 500M or 10G. More info might be in MAC_LINK_500M_STS.
		 * Without support for the 500M modes simply resolve to 10G.
		 */
		state->speed = SPEED_10000;
		break;
	case RTPCS_SPEED_2500:
		state->speed = SPEED_2500;
		break;
	case RTPCS_SPEED_5000:
		state->speed = SPEED_5000;
		break;
	default:
		dev_err(ctrl->dev, "unknown speed %d\n", speed);
	}

	if (rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_rx_pause_sts, port, port))
		state->pause |= MLO_PAUSE_RX;
	if (rtpcs_regmap_read_bits(ctrl, ctrl->cfg->mac_tx_pause_sts, port, port))
		state->pause |= MLO_PAUSE_TX;
}

static void rtpcs_pcs_an_restart(struct phylink_pcs *pcs)
{
	struct rtpcs_link *link = rtpcs_phylink_pcs_to_link(pcs);
	struct rtpcs_ctrl *ctrl = link->ctrl;

	dev_warn(ctrl->dev, "an_restart() for port %d and sds %d not yet implemented\n",
		 link->port, link->sds);
}

static int rtpcs_pcs_config(struct phylink_pcs *pcs, unsigned int neg_mode,
			    phy_interface_t interface, const unsigned long *advertising,
			    bool permit_pause_to_mac)
{
	struct rtpcs_link *link = rtpcs_phylink_pcs_to_link(pcs);
	struct rtpcs_ctrl *ctrl = link->ctrl;
	int ret = 0;

	if (link->sds < 0)
		return 0;

	/*
	 * TODO: This (or copies of this) will be the central function for configuring the
	 * link between PHY and SerDes. As of now a lot of the code is scattered throughout
	 * all the other Realtek drivers. Maybe some day this will live up to the expectations.
	 */

	dev_warn(ctrl->dev, "pcs_config(%s) for port %d and sds %d not yet fully implemented\n",
		 phy_modes(interface), link->port, link->sds);

	mutex_lock(&ctrl->lock);

	if (ctrl->cfg->set_autoneg) {
		ret = ctrl->cfg->set_autoneg(ctrl, link->sds, neg_mode);
		if (ret < 0)
			goto out;
	}

out:
	mutex_unlock(&ctrl->lock);

	return ret;
}

struct phylink_pcs *rtpcs_create(struct device *dev, struct device_node *np, int port);
struct phylink_pcs *rtpcs_create(struct device *dev, struct device_node *np, int port)
{
	struct platform_device *pdev;
	struct device_node *pcs_np;
	struct rtpcs_ctrl *ctrl;
	struct rtpcs_link *link;
	int sds;

	/*
	 * RTL838x devices have a built-in octa port RTL8218B PHY that is not attached via
	 * a SerDes. Allow to be called with an empty SerDes device node. In this case lookup
	 * the parent/driver node directly.
	 */
	if (np) {
		if (!of_device_is_available(np))
			return ERR_PTR(-ENODEV);

		if (of_property_read_u32(np, "reg", &sds))
			return ERR_PTR(-EINVAL);

		pcs_np = of_get_parent(np);
	} else {
		pcs_np = of_find_compatible_node(NULL, NULL, "realtek,otto-pcs");
		sds = -1;
	}

	if (!pcs_np)
		return ERR_PTR(-ENODEV);

	if (!of_device_is_available(pcs_np)) {
		of_node_put(pcs_np);
		return ERR_PTR(-ENODEV);
	}

	pdev = of_find_device_by_node(pcs_np);
	of_node_put(pcs_np);
	if (!pdev)
		return ERR_PTR(-EPROBE_DEFER);

	ctrl = platform_get_drvdata(pdev);
	if (!ctrl) {
		put_device(&pdev->dev);
		return ERR_PTR(-EPROBE_DEFER);
	}

	if (port < 0 || port > ctrl->cfg->cpu_port)
		return ERR_PTR(-EINVAL);

	if (sds !=-1 && rtpcs_sds_read(ctrl, sds, 0 , 0) < 0)
		return ERR_PTR(-EINVAL);

	link = kzalloc(sizeof(*link), GFP_KERNEL);
	if (!link) {
		put_device(&pdev->dev);
		return ERR_PTR(-ENOMEM);
	}

	device_link_add(dev, ctrl->dev, DL_FLAG_AUTOREMOVE_CONSUMER);

	link->ctrl = ctrl;
	link->port = port;
	link->sds = sds;
	link->pcs.ops = ctrl->cfg->pcs_ops;
	link->pcs.neg_mode = true;

	ctrl->link[port] = link;

	dev_dbg(ctrl->dev, "phylink_pcs created, port %d, sds %d\n", port, sds);

	return &link->pcs;
}
EXPORT_SYMBOL(rtpcs_create);

static struct mii_bus *rtpcs_probe_serdes_bus(struct rtpcs_ctrl *ctrl)
{
	struct device_node *np;
	struct mii_bus *bus;

	np = of_find_compatible_node(NULL, NULL, "realtek,otto-serdes-mdio");
	if (!np) {
		dev_err(ctrl->dev, "SerDes mdio bus not found in DT");
		return ERR_PTR(-ENODEV);
	}

	bus = of_mdio_find_bus(np);
	of_node_put(np);
	if (!bus) {
		dev_warn(ctrl->dev, "SerDes mdio bus not (yet) active");
		return ERR_PTR(-EPROBE_DEFER);
	}

	if (!of_device_is_available(np)) {
		dev_err(ctrl->dev, "SerDes mdio bus not usable");
		return ERR_PTR(-ENODEV);
	}

	return bus;
}

static int rtpcs_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct rtpcs_ctrl *ctrl;

	ctrl = devm_kzalloc(dev, sizeof(*ctrl), GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	mutex_init(&ctrl->lock);

	ctrl->dev = dev;
	ctrl->cfg = (const struct rtpcs_config *)device_get_match_data(ctrl->dev);
	ctrl->map = syscon_node_to_regmap(np->parent);
	if (IS_ERR(ctrl->map))
		return PTR_ERR(ctrl->map);

	ctrl->bus = rtpcs_probe_serdes_bus(ctrl);
	if (IS_ERR(ctrl->bus))
		return PTR_ERR(ctrl->bus);
	/*
	 * rtpcs_create() relies on that fact that data is attached to the platform device to
	 * determine if the driver is ready. Do this after everything is initialized properly.
	 */
	platform_set_drvdata(pdev, ctrl);

	dev_info(dev, "Realtek PCS driver initialized\n");

	return 0;
}

static int rtpcs_93xx_set_autoneg(struct rtpcs_ctrl *ctrl, int sds,
				  unsigned int neg_mode)
{
	u16 bmcr = neg_mode == PHYLINK_PCS_NEG_INBAND_ENABLED ? BMCR_ANENABLE : 0;

	return rtpcs_sds_modify(ctrl, sds, 2, MII_BMCR, BMCR_ANENABLE, bmcr);
}

static const struct phylink_pcs_ops rtpcs_838x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_config rtpcs_838x_cfg = {
	.cpu_port		= RTPCS_838X_CPU_PORT,
	.mac_link_dup_sts	= RTPCS_838X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts	= RTPCS_838X_MAC_LINK_SPD_STS,
	.mac_link_spd_bits	= RTPCS_83XX_MAC_LINK_SPD_BITS,
	.mac_link_sts		= RTPCS_838X_MAC_LINK_STS,
	.mac_rx_pause_sts	= RTPCS_838X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts	= RTPCS_838X_MAC_TX_PAUSE_STS,
	.pcs_ops		= &rtpcs_838x_pcs_ops,
};

static const struct phylink_pcs_ops rtpcs_839x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_config rtpcs_839x_cfg = {
	.cpu_port		= RTPCS_839X_CPU_PORT,
	.mac_link_dup_sts	= RTPCS_839X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts	= RTPCS_839X_MAC_LINK_SPD_STS,
	.mac_link_spd_bits	= RTPCS_83XX_MAC_LINK_SPD_BITS,
	.mac_link_sts		= RTPCS_839X_MAC_LINK_STS,
	.mac_rx_pause_sts	= RTPCS_839X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts	= RTPCS_839X_MAC_TX_PAUSE_STS,
	.pcs_ops		= &rtpcs_839x_pcs_ops,
};

static const struct phylink_pcs_ops rtpcs_930x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_config rtpcs_930x_cfg = {
	.cpu_port		= RTPCS_930X_CPU_PORT,
	.mac_link_dup_sts	= RTPCS_930X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts	= RTPCS_930X_MAC_LINK_SPD_STS,
	.mac_link_spd_bits	= RTPCS_93XX_MAC_LINK_SPD_BITS,
	.mac_link_sts		= RTPCS_930X_MAC_LINK_STS,
	.mac_rx_pause_sts	= RTPCS_930X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts	= RTPCS_930X_MAC_TX_PAUSE_STS,
	.pcs_ops		= &rtpcs_930x_pcs_ops,
	.set_autoneg		= rtpcs_93xx_set_autoneg,
};

static const struct phylink_pcs_ops rtpcs_931x_pcs_ops = {
	.pcs_an_restart		= rtpcs_pcs_an_restart,
	.pcs_config		= rtpcs_pcs_config,
	.pcs_get_state		= rtpcs_pcs_get_state,
};

static const struct rtpcs_config rtpcs_931x_cfg = {
	.cpu_port		= RTPCS_931X_CPU_PORT,
	.mac_link_dup_sts	= RTPCS_931X_MAC_LINK_DUP_STS,
	.mac_link_spd_sts	= RTPCS_931X_MAC_LINK_SPD_STS,
	.mac_link_spd_bits	= RTPCS_93XX_MAC_LINK_SPD_BITS,
	.mac_link_sts		= RTPCS_931X_MAC_LINK_STS,
	.mac_rx_pause_sts	= RTPCS_931X_MAC_RX_PAUSE_STS,
	.mac_tx_pause_sts	= RTPCS_931X_MAC_TX_PAUSE_STS,
	.pcs_ops		= &rtpcs_931x_pcs_ops,
	.set_autoneg		= rtpcs_93xx_set_autoneg,
};

static const struct of_device_id rtpcs_of_match[] = {
	{
		.compatible = "realtek,rtl8380-pcs",
		.data = &rtpcs_838x_cfg,
	},
	{
		.compatible = "realtek,rtl8392-pcs",
		.data = &rtpcs_839x_cfg,
	},
	{
		.compatible = "realtek,rtl9301-pcs",
		.data = &rtpcs_930x_cfg,
	},
	{
		.compatible = "realtek,rtl9311-pcs",
		.data = &rtpcs_931x_cfg,
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtpcs_of_match);

static struct platform_driver rtpcs_driver = {
	.driver = {
		.name = "realtek-otto-pcs",
		.of_match_table = rtpcs_of_match
	},
	.probe = rtpcs_probe,
};
module_platform_driver(rtpcs_driver);

MODULE_AUTHOR("Markus Stockhausen <markus.stockhausen@gmx.de>");
MODULE_DESCRIPTION("Realtek Otto SerDes PCS driver");
MODULE_LICENSE("GPL v2");