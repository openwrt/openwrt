// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s PCIe PHY driver
 *
 * Controls the analog SerDes and PLL for the two PCIe ports inside the
 * Top CRM block.
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#define CRM1_PHY_CTRL		0x008
#define CRM1_MISC		0x024
#define CRM1_MISC_BIT26		BIT(26)
#define CRM1_PHY1_CAL0		0x1b0
#define CRM1_PHY1_CAL1		0x1b4

struct zx_pcie_phy_port {
	u32 phy_on[3];
	bool has_cal;
};

static const struct zx_pcie_phy_port zx_phy_ports[] = {
	[0] = {
		.phy_on = { BIT(15), BIT(13), BIT(14) },
	},
	[1] = {
		.phy_on = { BIT(27), BIT(25), BIT(26) },
		.has_cal = true,
	},
};

struct zx_pcie_phy {
	struct regmap *regmap;
	const struct zx_pcie_phy_port *port;
	unsigned int port_id;
};

static int zx_pcie_phy_init(struct phy *phy)
{
	struct zx_pcie_phy *zphy = phy_get_drvdata(phy);
	const struct zx_pcie_phy_port *port = zphy->port;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(port->phy_on); i++) {
		ret = regmap_write_bits(zphy->regmap, CRM1_PHY_CTRL,
					port->phy_on[i], port->phy_on[i]);
		if (ret)
			return ret;
		fsleep(50);

		if (i == 0 && port->has_cal) {
			ret = regmap_write(zphy->regmap, CRM1_PHY1_CAL1, 0x00202d5a);
			if (ret)
				return ret;
			ret = regmap_write(zphy->regmap, CRM1_PHY1_CAL0, 0x0046c24a);
			if (ret)
				return ret;
		}
	}

	ret = regmap_write_bits(zphy->regmap, CRM1_MISC, CRM1_MISC_BIT26, 0);
	if (ret)
		return ret;
	fsleep(50);

	return 0;
}

static int zx_pcie_phy_exit(struct phy *phy)
{
	struct zx_pcie_phy *zphy = phy_get_drvdata(phy);
	const struct zx_pcie_phy_port *port = zphy->port;
	int i, ret;

	for (i = ARRAY_SIZE(port->phy_on) - 1; i >= 0; i--) {
		ret = regmap_write_bits(zphy->regmap, CRM1_PHY_CTRL,
					port->phy_on[i], 0);
		if (ret)
			return ret;
	}

	return 0;
}

static const struct phy_ops zx_pcie_phy_ops = {
	.init = zx_pcie_phy_init,
	.exit = zx_pcie_phy_exit,
	.owner = THIS_MODULE,
};

static int zx_pcie_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct phy_provider *provider;
	struct zx_pcie_phy *zphy;
	struct phy *phy;
	u32 port_id;
	int ret;

	zphy = devm_kzalloc(dev, sizeof(*zphy), GFP_KERNEL);
	if (!zphy)
		return -ENOMEM;

	ret = of_property_read_u32(dev->of_node, "reg", &port_id);
	if (ret)
		return dev_err_probe(dev, ret, "missing reg property\n");

	if (port_id >= ARRAY_SIZE(zx_phy_ports))
		return dev_err_probe(dev, -EINVAL, "invalid port id %u\n", port_id);

	zphy->port_id = port_id;
	zphy->port = &zx_phy_ports[port_id];

	zphy->regmap = syscon_node_to_regmap(dev->parent->of_node);
	if (IS_ERR(zphy->regmap))
		return dev_err_probe(dev, PTR_ERR(zphy->regmap),
				     "failed to get parent topcrm regmap\n");

	phy = devm_phy_create(dev, dev->of_node, &zx_pcie_phy_ops);
	if (IS_ERR(phy))
		return dev_err_probe(dev, PTR_ERR(phy), "failed to create phy\n");

	phy_set_drvdata(phy, zphy);

	provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	return PTR_ERR_OR_ZERO(provider);
}

static const struct of_device_id zx_pcie_phy_of_match[] = {
	{ .compatible = "zte,zx279128s-pcie-phy" },
	{ }
};
MODULE_DEVICE_TABLE(of, zx_pcie_phy_of_match);

static struct platform_driver zx_pcie_phy_driver = {
	.probe = zx_pcie_phy_probe,
	.driver = {
		.name = "zx279128s-pcie-phy",
		.of_match_table = zx_pcie_phy_of_match,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(zx_pcie_phy_driver);

MODULE_AUTHOR("Navid Ghahremani <ghahramani.navid@gmail.com>");
MODULE_DESCRIPTION("ZTE ZX279128S PCIe PHY driver");
MODULE_LICENSE("GPL");
