// SPDX-License-Identifier: GPL-2.0-only

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/slab.h>

// DsRelTunerRef registers used by the BCM3380 bootloader to bring up the internal GPHY reference clock.
#define GPHY_REF_OFFSET				0x1000
#define GPHY_REF_SIZE				0x0010
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

struct gphy {
	struct clk_bulk_data *clocks;
	int num_clocks;
	void __iomem *ref;
};

static void gphy_disable_clocks(void *data)
{
	struct gphy *gphy = data;

	clk_bulk_disable_unprepare(gphy->num_clocks, gphy->clocks);
}

static int gphy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct gphy *gphy = devm_kzalloc(dev, sizeof(*gphy), GFP_KERNEL);
	if (!gphy)
		return -ENOMEM;

	platform_set_drvdata(pdev, gphy);

	u32 tuner_base;
	int ret = of_property_read_u32(dev->of_node, "brcm,dt_tunner",
				       &tuner_base);
	if (ret)
		return dev_err_probe(dev, ret, "failed to read brcm,dt_tunner\n");

	gphy->ref = devm_ioremap(dev,
				 (phys_addr_t)tuner_base + GPHY_REF_OFFSET,
				 GPHY_REF_SIZE);
	if (!gphy->ref)
		return dev_err_probe(dev, -ENOMEM,
				     "failed to map brcm,dt_tunner\n");

	gphy->num_clocks = devm_clk_bulk_get_all(dev, &gphy->clocks);
	if (gphy->num_clocks < 0)
		return dev_err_probe(dev, gphy->num_clocks,
				     "failed to get clocks\n");

	ret = clk_bulk_prepare_enable(gphy->num_clocks, gphy->clocks);
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

	return 0;
}

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
