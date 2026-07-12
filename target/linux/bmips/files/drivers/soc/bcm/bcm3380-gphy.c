// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BCM3380 GPHY Driver
 *
 * Copyright (C) 2026 Hang Zhou <929513338@qq.com>
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/slab.h>

#include <soc/bcm/bcm3380-gphy.h>

// DsRelTunerRef REF00..REF03 registers used by the bootloader to bring up the shared GPHY reference clock.
#define GPHY_REF00				0x0000
#define GPHY_REF00_POWERUP			0x10780000
#define GPHY_REF00_CLEAR_FIRST			0x00500000
#define GPHY_REF00_CLEAR_SECOND			0x00080000
#define GPHY_REF00_CLEAR_THIRD			0x00200000
#define GPHY_REF01				0x0004
#define GPHY_REF01_RESET_PULSE			0x0009c000
#define GPHY_REF02				0x0008
#define GPHY_REF02_CLK_ENABLE			0x00800000
#define GPHY_REF02_CLK_SELECT			0x00000200
#define GPHY_REF02_OUTDIV_MASK			0x00000007
#define GPHY_REF03				0x000c
#define GPHY_REF03_ENABLE			0x00000016

struct bcm3380_gphy {
	struct device *dev;
	struct clk_bulk_data *clocks;
	int num_clocks;
	void __iomem *ref;
	bool ready;
};

static void gphy_disable_clocks(void *data)
{
	struct bcm3380_gphy *gphy = data;

	clk_bulk_disable_unprepare(gphy->num_clocks, gphy->clocks);
}

static bool gphy_ready(struct bcm3380_gphy *gphy)
{
	return gphy && gphy->ready;
}

static int gphy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct bcm3380_gphy *gphy = devm_kzalloc(dev, sizeof(*gphy), GFP_KERNEL);
	if (!gphy)
		return -ENOMEM;

	gphy->dev = dev;
	platform_set_drvdata(pdev, gphy);

	gphy->ref = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(gphy->ref))
		return PTR_ERR(gphy->ref);

	gphy->num_clocks = devm_clk_bulk_get_all(dev, &gphy->clocks);
	if (gphy->num_clocks < 0)
		return dev_err_probe(dev, gphy->num_clocks,
				     "failed to get clocks\n");

	int ret = clk_bulk_prepare_enable(gphy->num_clocks, gphy->clocks);
	if (ret)
		return dev_err_probe(dev, ret, "failed to enable clocks\n");

	ret = devm_add_action_or_reset(dev, gphy_disable_clocks, gphy);
	if (ret)
		return ret;

	struct reset_control *reset =
		devm_reset_control_get_optional_exclusive(dev, "gphy");
	if (IS_ERR(reset))
		return dev_err_probe(dev, PTR_ERR(reset), "failed to get reset\n");

	if (reset) {
		ret = reset_control_assert(reset);
		if (ret)
			return dev_err_probe(dev, ret, "failed to assert reset\n");
	}

	mdelay(1);

	u32 val = readl_be(gphy->ref + GPHY_REF00);

	writel_be(val | GPHY_REF00_POWERUP, gphy->ref + GPHY_REF00);
	mdelay(1);

	val = readl_be(gphy->ref + GPHY_REF00);
	writel_be(val & ~GPHY_REF00_CLEAR_FIRST, gphy->ref + GPHY_REF00);
	mdelay(1);

	val = readl_be(gphy->ref + GPHY_REF00);
	writel_be(val & ~GPHY_REF00_CLEAR_SECOND, gphy->ref + GPHY_REF00);
	mdelay(6);

	val = readl_be(gphy->ref + GPHY_REF00);
	writel_be(val & ~GPHY_REF00_CLEAR_THIRD, gphy->ref + GPHY_REF00);

	val = readl_be(gphy->ref + GPHY_REF02);
	writel_be(val | GPHY_REF02_CLK_ENABLE, gphy->ref + GPHY_REF02);

	val = readl_be(gphy->ref + GPHY_REF03);
	writel_be(val | GPHY_REF03_ENABLE, gphy->ref + GPHY_REF03);

	val = readl_be(gphy->ref + GPHY_REF01);
	writel_be(val | GPHY_REF01_RESET_PULSE, gphy->ref + GPHY_REF01);
	mdelay(1);

	val = readl_be(gphy->ref + GPHY_REF01);
	writel_be(val & ~GPHY_REF01_RESET_PULSE, gphy->ref + GPHY_REF01);

	val = readl_be(gphy->ref + GPHY_REF02);
	val |= GPHY_REF02_CLK_SELECT;
	val &= ~GPHY_REF02_OUTDIV_MASK;
	writel_be(val, gphy->ref + GPHY_REF02);

	dev_info(dev,
		 "initialized internal GPHY ref: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
		 readl_be(gphy->ref + GPHY_REF00),
		 readl_be(gphy->ref + GPHY_REF01),
		 readl_be(gphy->ref + GPHY_REF02),
		 readl_be(gphy->ref + GPHY_REF03));

	mdelay(1);

	if (reset) {
		ret = reset_control_deassert(reset);
		if (ret)
			return dev_err_probe(dev, ret, "failed to deassert reset\n");
	}

	usleep_range(10000, 20000);
	gphy->ready = true;

	return 0;
}

int gphy_get(struct device *consumer, struct bcm3380_gphy **gphy)
{
	if (!consumer || !consumer->of_node || !gphy)
		return -EINVAL;

	struct device_node *np = of_parse_phandle(consumer->of_node, "brcm,gphy", 0);
	if (!np)
		return -ENODEV;

	struct platform_device *pdev = of_find_device_by_node(np);
	of_node_put(np);
	if (!pdev)
		return -EPROBE_DEFER;

	struct bcm3380_gphy *provider = platform_get_drvdata(pdev);
	if (!gphy_ready(provider)) {
		put_device(&pdev->dev);
		return -EPROBE_DEFER;
	}

	*gphy = provider;

	return 0;
}
EXPORT_SYMBOL_GPL(gphy_get);

void gphy_put(void *data)
{
	struct bcm3380_gphy *gphy = data;

	if (gphy && gphy->dev)
		put_device(gphy->dev);
}
EXPORT_SYMBOL_GPL(gphy_put);

static const struct of_device_id gphy_of_match[] = {
	{ .compatible = "brcm,bcm3380-gphy", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, gphy_of_match);

static struct platform_driver gphy_driver = {
	.probe = gphy_probe,
	.driver = {
		.name = "bcm3380-gphy",
		.of_match_table = gphy_of_match,
	},
};
module_platform_driver(gphy_driver);

MODULE_DESCRIPTION("BCM3380 internal GPHY bring-up driver");
MODULE_LICENSE("GPL");
