// SPDX-License-Identifier: GPL-2.0-only
/* CS7542 Snow Bush SerDes, PCIe mode with a 24 MHz reference clock. */
#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#define GLOBAL_JTAG_ID	0x00
#define GLOBAL_PHY_CONTROL 0x14
#define PHY_REFCLK_MASK(p) (3U << ((p) * 2))
#define PHY_REFCLK_24MHZ(p) (2U << ((p) * 2))
#define PHY_PD(p)	BIT(8 + (p))
#define PHY_CMU_RESETN(p) BIT(16 + (p) * 4)
#define PHY_LN0_RESETN(p) BIT(17 + (p) * 4)
#define PHY_POR_N(p)	BIT(19 + (p) * 4)
#define PHY_CMU_OK	BIT(18)
#define PHY_STATUS_STRIDE 0x80
#define SNOW_COM_REG0	0x00
#define SNOW_REG0_CFG	0x00090006
#define SNOW_REG0_AHB_RESETN BIT(0)
#define SNOW_REV_REG	96
#define SNOW_REV_REG_A0	0x5e082e00
#define SNOW_REV_REG_A1	0x5e080600
#define SNOW_COM_LANE_REG0 0xc8
#define SNOW_LANE_REG0_CFG 0x400290c0
#define SNOW_LANE_REG0_MASTER_RESETN BIT(1)
#define CS7542_PORTS	3

struct cs7542_serdes;
struct cs7542_lane {
	struct cs7542_serdes *serdes;
	void __iomem *base;
	struct phy *phy;
	unsigned int id;
};

struct cs7542_serdes {
	struct device *dev;
	struct regmap *global;
	void __iomem *status;
	struct clk *ref;
	struct cs7542_lane lanes[CS7542_PORTS];
	u32 revision_value;
};

struct snow_reg {
	u16 off;
	u32 val;
};

static const struct snow_reg snow_phy_24mhz[] = {
	{ 0,   SNOW_REG0_CFG }, { 4,   0x000e0960 }, { 32,  0x68a00000 },
	{ 44,  0x50040000 }, { 48,  0x40250270 }, { 52,  0x00004001 },
	{ 100, 0xf0914200 }, { 104, 0x4c0c9048 }, { 108, 0x00000373 },
	{ 124, 0x04841000 }, { 128, 0x000000e0 }, { 132, 0x04000023 },
	{ 136, 0x68001038 }, { 140, 0x0d181ea2 }, { 144, 0x0000000c },
	{ 196, 0x0f600000 }, { 200, SNOW_LANE_REG0_CFG }, { 204, 0x0000003c },
	{ 208, 0xc68b8300 }, { 212, 0x98280301 }, { 216, 0xe1782819 },
	{ 220, 0x00f410f0 }, { 232, 0xa0a0a000 }, { 236, 0xa0a0a0a0 },
	{ 240, 0x9fc00068 }, { 244, 0x00000001 }, { 248, 0x00000000 },
	{ 252, 0xd07e4130 }, { 256, 0x935285cc }, { 260, 0xb0dd49e0 },
	{ 264, 0x0000020b }, { 300, 0xd8000000 }, { 304, 0x0001ff1a },
	{ 308, 0xf0000000 }, { 312, 0xffffffff }, { 316, 0x3fc3c21c },
	{ 320, 0x0000000a }, { 324, 0x00f80000 },
};

static int cs7542_phy_power_off(struct phy *phy)
{
	struct cs7542_lane *lane = phy_get_drvdata(phy);
	unsigned int p = lane->id;
	u32 mask = PHY_PD(p) | PHY_CMU_RESETN(p) | PHY_LN0_RESETN(p) | PHY_POR_N(p);
	int ret;

	ret = regmap_update_bits(lane->serdes->global, GLOBAL_PHY_CONTROL, mask, PHY_PD(p));
	if (ret)
		return ret;
	clk_disable_unprepare(lane->serdes->ref);
	return ret;
}

static int cs7542_phy_power_on(struct phy *phy)
{
	struct cs7542_lane *lane = phy_get_drvdata(phy);
	struct cs7542_serdes *sd = lane->serdes;
	unsigned int p = lane->id;
	u32 mask, value;
	int ret, i;

	ret = clk_prepare_enable(sd->ref);
	if (ret)
		return ret;
	mask = PHY_PD(p) | PHY_CMU_RESETN(p) | PHY_LN0_RESETN(p) | PHY_POR_N(p) |
	       PHY_REFCLK_MASK(p);
	ret = regmap_update_bits(sd->global, GLOBAL_PHY_CONTROL, mask,
				 PHY_PD(p) | PHY_REFCLK_24MHZ(p));
	if (ret)
		goto power_off;
	msleep(100);
	ret = regmap_update_bits(sd->global, GLOBAL_PHY_CONTROL, PHY_PD(p) | PHY_POR_N(p),
				 PHY_POR_N(p));
	if (ret)
		goto power_off;
	usleep_range(10, 20);
	for (i = 0; i < ARRAY_SIZE(snow_phy_24mhz); i++) {
		if (snow_phy_24mhz[i].off == SNOW_REV_REG + 4)
			writel(sd->revision_value, lane->base + SNOW_REV_REG);
		writel(snow_phy_24mhz[i].val, lane->base + snow_phy_24mhz[i].off);
	}
	writel(SNOW_REG0_CFG | SNOW_REG0_AHB_RESETN, lane->base + SNOW_COM_REG0);
	ret = regmap_update_bits(sd->global, GLOBAL_PHY_CONTROL, PHY_CMU_RESETN(p),
				 PHY_CMU_RESETN(p));
	if (ret)
		goto power_off;
	ret = readl_poll_timeout(sd->status + p * PHY_STATUS_STRIDE, value,
				 value & PHY_CMU_OK, 100, 100000);
	if (ret) {
		dev_err(sd->dev, "lane %u CMU did not lock\n", p);
		goto power_off;
	}
	writel(SNOW_LANE_REG0_CFG | SNOW_LANE_REG0_MASTER_RESETN,
	       lane->base + SNOW_COM_LANE_REG0);
	ret = regmap_update_bits(sd->global, GLOBAL_PHY_CONTROL, PHY_LN0_RESETN(p),
				 PHY_LN0_RESETN(p));
	if (ret)
		goto power_off;
	msleep(100);
	return 0;

power_off:
	/* Failed power_on has no generic PHY power reference to retain. */
	if (cs7542_phy_power_off(phy))
		clk_disable_unprepare(sd->ref);
	return ret;
}

static int cs7542_phy_set_mode(struct phy *phy, enum phy_mode mode, int submode)
{
	return mode == PHY_MODE_PCIE && !submode ? 0 : -EINVAL;
}

static const struct phy_ops cs7542_phy_ops = {
	.power_on = cs7542_phy_power_on,
	.power_off = cs7542_phy_power_off,
	.set_mode = cs7542_phy_set_mode,
	.owner = THIS_MODULE,
};

static struct phy *cs7542_phy_xlate(struct device *dev, const struct of_phandle_args *args)
{
	struct cs7542_serdes *sd = dev_get_drvdata(dev);

	if (args->args_count != 1 || args->args[0] >= CS7542_PORTS)
		return ERR_PTR(-EINVAL);
	return sd->lanes[args->args[0]].phy;
}

static int cs7542_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct phy_provider *provider;
	struct cs7542_serdes *sd;
	unsigned int id;
	int ret, i;

	sd = devm_kzalloc(dev, sizeof(*sd), GFP_KERNEL);
	if (!sd)
		return -ENOMEM;
	sd->dev = dev;
	sd->global = syscon_regmap_lookup_by_phandle(dev->of_node, "cortina,global-syscon");
	if (IS_ERR(sd->global))
		return dev_err_probe(dev, PTR_ERR(sd->global), "global register map\n");
	sd->ref = devm_clk_get(dev, NULL);
	if (IS_ERR(sd->ref))
		return dev_err_probe(dev, PTR_ERR(sd->ref), "reference clock\n");
	if (clk_get_rate(sd->ref) != 24000000)
		return dev_err_probe(dev, -EINVAL, "only 24 MHz reference supported\n");
	ret = regmap_read(sd->global, GLOBAL_JTAG_ID, &id);
	if (ret)
		return ret;
	switch (FIELD_GET(GENMASK(15, 12), id)) {
	case 0:
	case 1:
		sd->revision_value = SNOW_REV_REG_A0;
		break;
	case 8:
	case 9:
		sd->revision_value = SNOW_REV_REG_A1;
		break;
	default:
		return dev_err_probe(dev, -ENODEV, "unsupported silicon revision %#x\n", id);
	}
	sd->status = devm_platform_ioremap_resource(pdev, CS7542_PORTS);
	if (IS_ERR(sd->status))
		return PTR_ERR(sd->status);
	for (i = 0; i < CS7542_PORTS; i++) {
		struct cs7542_lane *lane = &sd->lanes[i];

		lane->id = i;
		lane->serdes = sd;
		lane->base = devm_platform_ioremap_resource(pdev, i);
		if (IS_ERR(lane->base))
			return PTR_ERR(lane->base);
		lane->phy = devm_phy_create(dev, NULL, &cs7542_phy_ops);
		if (IS_ERR(lane->phy))
			return PTR_ERR(lane->phy);
		phy_set_drvdata(lane->phy, lane);
	}
	platform_set_drvdata(pdev, sd);
	provider = devm_of_phy_provider_register(dev, cs7542_phy_xlate);
	return PTR_ERR_OR_ZERO(provider);
}

static const struct of_device_id cs7542_phy_match[] = {
	{ .compatible = "cortina,cs7542-pcie-phy" },
	{ }
};
MODULE_DEVICE_TABLE(of, cs7542_phy_match);

static struct platform_driver cs7542_phy_driver = {
	.probe = cs7542_phy_probe,
	.driver = {
		.name = "cs7542-pcie-phy",
		.of_match_table = cs7542_phy_match,
	},
};
module_platform_driver(cs7542_phy_driver);

MODULE_DESCRIPTION("Cortina Systems CS7542 PCIe SerDes");
MODULE_LICENSE("GPL");
