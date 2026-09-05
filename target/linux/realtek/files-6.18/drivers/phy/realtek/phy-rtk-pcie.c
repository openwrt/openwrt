// SPDX-License-Identifier: GPL-2.0-or-later

#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>

#define PCIE_MDIO_CTRL_PHY_REG		0x0
#define PCIE_MDIO_CTRL_PHY_DATA_MASK	GENMASK(31, 16)
#define PCIE_MDIO_CTRL_PHY_PAGE_MASK	GENMASK(15, 13)
#define PCIE_MDIO_CTRL_PHY_ADDR_MASK	GENMASK(12, 8)
#define PCIE_MDIO_CTRL_PHY_STATUS_MASK	GENMASK(6, 5)
#define PCIE_MDIO_CTRL_PHY_STATUS_DONE	FIELD_PREP(PCIE_MDIO_CTRL_PHY_STATUS_MASK, 1)
#define PCIE_MDIO_CTRL_PHY_READY	BIT(4)
#define PCIE_MDIO_CTRL_PHY_WRITE	BIT(0)
#define PCIE_MDIO_CTRL_PHY_READ		0

#define PCIE_PHY_POWER_CTRL_REG		0x08
#define PHY_RESET_BIT			BIT(7)
#define ENABLE_LTSSM_BIT		BIT(0)

#define PCIE_PHY_CTRL_BIT30		BIT(30)
#define PCIE_PHY_CTRL_MDRST0		BIT(24)
#define PCIE_PHY_CTRL_MDRST1		BIT(21)
#define PCIE_PHY_CTRL_DIS0		BIT(15)
#define PCIE_PHY_CTRL_DIS1		BIT(14)

#define MAX_PCIE_PHY_DATA_SIZE		0x30

#define PHY_GEN1_PAGE			0x0
#define PHY_GEN2_PAGE			0x2

#define PHY_ADDR_0X09			0x09
#define REG_0X09_FORCE_CALIBRATION	BIT(9)

struct phy_data {
	u8 page;
	u8 addr;
	u16 data;
};

struct phy_cfg {
	struct phy_data gen1_param[MAX_PCIE_PHY_DATA_SIZE];
	struct phy_data gen2_param[MAX_PCIE_PHY_DATA_SIZE];
	bool do_toggle;
	u32 mdio_reset_bit;
	u32 disable_bit;
};

struct rtk_phy {
	struct device *dev;
	struct phy *phy;
	const struct phy_cfg *phy_cfg;
	struct regmap *regmap;
	struct regmap *pcie_misc_map;
	struct reset_control *phy_rst;
};

static int rtk_phy_wait_done_and_ready(struct regmap *regmap)
{
	u32 val, cond;

	cond = PCIE_MDIO_CTRL_PHY_STATUS_DONE | PCIE_MDIO_CTRL_PHY_READY;

	return regmap_read_poll_timeout(regmap, PCIE_MDIO_CTRL_PHY_REG, val,
					(val & cond) == cond, 10, 2000);
}

static int rtk_phy_write(struct rtk_phy *rtk_phy, u8 page, u8 addr, u16 data)
{
	int ret;

	ret = regmap_write(rtk_phy->regmap, PCIE_MDIO_CTRL_PHY_REG,
			   FIELD_PREP(PCIE_MDIO_CTRL_PHY_PAGE_MASK, page) |
			   FIELD_PREP(PCIE_MDIO_CTRL_PHY_DATA_MASK, data) |
			   FIELD_PREP(PCIE_MDIO_CTRL_PHY_ADDR_MASK, addr) |
			   PCIE_MDIO_CTRL_PHY_WRITE);
	if (ret)
		return ret;

	return rtk_phy_wait_done_and_ready(rtk_phy->regmap);
}

static int rtk_phy_read(struct rtk_phy *rtk_phy, u8 page, u8 addr, u16 *data)
{
	u32 val;
	int ret;

	ret = regmap_write(rtk_phy->regmap, PCIE_MDIO_CTRL_PHY_REG,
			   FIELD_PREP(PCIE_MDIO_CTRL_PHY_PAGE_MASK, page) |
			   FIELD_PREP(PCIE_MDIO_CTRL_PHY_ADDR_MASK, addr) |
			   PCIE_MDIO_CTRL_PHY_READ);
	if (ret)
		return ret;

	ret = rtk_phy_wait_done_and_ready(rtk_phy->regmap);
	if (ret)
		return ret;

	ret = regmap_read(rtk_phy->regmap, PCIE_MDIO_CTRL_PHY_REG, &val);
	if (ret)
		return ret;

	*data = FIELD_GET(PCIE_MDIO_CTRL_PHY_DATA_MASK, val);

	return 0;
}

static int rtk_phy_toggle_calibration(struct rtk_phy *rtk_phy, u8 page, u8 addr)
{
	const struct phy_cfg *phy_cfg = rtk_phy->phy_cfg;
	u16 data;
	int ret;

	if (!phy_cfg->do_toggle)
		return 0;

	ret = rtk_phy_read(rtk_phy, page, addr, &data);
	if (ret)
		return ret;

	ret = rtk_phy_write(rtk_phy, page, addr, data & (~REG_0X09_FORCE_CALIBRATION));
	if (ret)
		return ret;

	return rtk_phy_write(rtk_phy, page, addr, data | REG_0X09_FORCE_CALIBRATION);
}

static int rtk_phy_init(struct phy *phy)
{
	struct rtk_phy *rtk_phy = phy_get_drvdata(phy);
	const struct phy_cfg *phy_cfg = rtk_phy->phy_cfg;
	int ret, i = 0;

	/* PCIE phy mdio reset */
	ret = regmap_clear_bits(rtk_phy->pcie_misc_map, 0, phy_cfg->disable_bit);
	if (ret)
		return ret;

	ret = regmap_clear_bits(rtk_phy->pcie_misc_map, 0, phy_cfg->mdio_reset_bit);
	if (ret)
		return ret;

	ret = regmap_set_bits(rtk_phy->pcie_misc_map, 0, phy_cfg->mdio_reset_bit);
	if (ret)
		return ret;

	/* PCIE IP Reset */
	ret = reset_control_deassert(rtk_phy->phy_rst);
	if (ret)
		return ret;

	usleep_range(10000, 11000);

	/*  Clear the PCIe PHY reset bit to activate the software reset */
	ret = regmap_clear_bits(rtk_phy->regmap, PCIE_PHY_POWER_CTRL_REG, PHY_RESET_BIT);
	if (ret)
		return ret;

	/* TODO: Check if ENABLE_LTSSM_BIT need to set here */
	ret = regmap_set_bits(rtk_phy->regmap, PCIE_PHY_POWER_CTRL_REG, PHY_RESET_BIT);
	if (ret)
		return ret;

	msleep(50);

	/* Set gen 1 parameters */
	for (i = 0; i < ARRAY_SIZE(phy_cfg->gen1_param); i++) {
		if (phy_cfg->gen1_param[i].page == 0 &&
		    phy_cfg->gen1_param[i].addr == 0 &&
		    phy_cfg->gen1_param[i].data == 0)
			break;

		u8 page = phy_cfg->gen1_param[i].page;
		u8 addr = phy_cfg->gen1_param[i].addr;
		u16 data = phy_cfg->gen1_param[i].data;

		ret = rtk_phy_write(rtk_phy, page, addr, data);
		if (ret)
			return ret;
	}

	/* toggle calibration for gen 1 parameters */
	ret = rtk_phy_toggle_calibration(rtk_phy, PHY_GEN1_PAGE, PHY_ADDR_0X09);
	if (ret)
		return ret;

	/* Set gen 2 parameters */
	for (i = 0; i < ARRAY_SIZE(phy_cfg->gen2_param); i++) {
		if (phy_cfg->gen2_param[i].page == 0 &&
		    phy_cfg->gen2_param[i].addr == 0 &&
		    phy_cfg->gen2_param[i].data == 0)
			break;

		u8 page = phy_cfg->gen2_param[i].page;
		u8 addr = phy_cfg->gen2_param[i].addr;
		u16 data = phy_cfg->gen2_param[i].data;

		ret = rtk_phy_write(rtk_phy, page, addr, data);
		if (ret)
			return ret;
	}

	/* toggle calibration for gen 2 parameters */
	if (i != 0) {
		ret = rtk_phy_toggle_calibration(rtk_phy, PHY_GEN2_PAGE, PHY_ADDR_0X09);
		if (ret)
			return ret;
	}

	msleep(20);

	return 0;
}

static int rtk_phy_exit(struct phy *phy)
{
	struct rtk_phy *rtk_phy = phy_get_drvdata(phy);

	return reset_control_assert(rtk_phy->phy_rst);
}

static const struct phy_ops ops = {
	.init		= rtk_phy_init,
	.exit		= rtk_phy_exit,
	.owner		= THIS_MODULE,
};

static int rtk_pcie_phy_probe(struct platform_device *pdev)
{
	static const struct regmap_config regmap_config = {
		.reg_bits = 32,
		.val_bits = 32,
		.reg_stride = 4,
	};
	struct rtk_phy *rtk_phy;
	struct device *dev = &pdev->dev;
	struct phy_provider *phy_provider;
	const struct phy_cfg *phy_cfg;
	void __iomem *base;

	rtk_phy = devm_kzalloc(dev, sizeof(*rtk_phy), GFP_KERNEL);
	if (!rtk_phy)
		return -ENOMEM;

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base))
		return dev_err_probe(dev, PTR_ERR(base),
				     "Failed to map phy-reg-mdio base\n");

	rtk_phy->regmap = devm_regmap_init_mmio(dev, base, &regmap_config);
	if (IS_ERR(rtk_phy->regmap))
		return PTR_ERR(rtk_phy->regmap);

	rtk_phy->pcie_misc_map = syscon_regmap_lookup_by_phandle(dev->of_node, "realtek,pcie-misc");
	if (IS_ERR(rtk_phy->pcie_misc_map))
		return dev_err_probe(dev, PTR_ERR(rtk_phy->pcie_misc_map),
				     "Failed to map pcie-misc registers\n");

	rtk_phy->phy_rst = devm_reset_control_array_get_optional_exclusive(dev);
	if (IS_ERR(rtk_phy->phy_rst))
		return dev_err_probe(dev, PTR_ERR(rtk_phy->phy_rst),
				     "Failed to get pcie phy resets\n");

	rtk_phy->dev = dev;
	phy_cfg = of_device_get_match_data(dev);
	if (!phy_cfg)
		return dev_err_probe(dev, -EINVAL, "phy config are not assigned!\n");

	rtk_phy->phy_cfg = phy_cfg;

	platform_set_drvdata(pdev, rtk_phy);

	rtk_phy->phy = devm_phy_create(dev, NULL, &ops);
	if (IS_ERR(rtk_phy->phy))
		return dev_err_probe(dev, PTR_ERR(rtk_phy->phy),
				     "Failed to create PCIe phy\n");

	phy_set_drvdata(rtk_phy->phy, rtk_phy);

	phy_provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	if (IS_ERR(phy_provider))
		return PTR_ERR(phy_provider);

	return 0;
}

static const struct phy_cfg rtl9607c_revB_gen2x1_phy_cfg = {
	.gen1_param = {
		{0x0, 0x01, 0xa852}, {0x0, 0x06, 0x0017}, {0x0, 0x08, 0x3591},
		{0x0, 0x09, 0x520c}, {0x0, 0x0a, 0xf670}, {0x0, 0x0b, 0xa90d},
		{0x0, 0x0d, 0xe720}, {0x0, 0x0e, 0x1010}, {0x0, 0x1c, 0x2001},
		{0x0, 0x1e, 0x66eb}, {0x1, 0x00, 0xd4a4}, {0x1, 0x01, 0x485a},
		{0x1, 0x03, 0x0b66}, {0x1, 0x04, 0x4f0c}, {0x1, 0x09, 0xf0f3},
		{0x1, 0x0b, 0xa0a1},
	},
	.gen2_param = {
		{0x2, 0x01, 0xa849}, {0x2, 0x06, 0x0017}, {0x2, 0x08, 0x3591},
		{0x2, 0x09, 0x520c}, {0x2, 0x0a, 0xf650}, {0x2, 0x0b, 0xa90d},
		{0x2, 0x0d, 0xe720}, {0x2, 0x0e, 0x1010}, {0x2, 0x1c, 0x2001},
		{0x3, 0x00, 0xd4a6}, {0x3, 0x01, 0x586a}, {0x3, 0x03, 0x0b66},
		{0x3, 0x09, 0xf0f3}, {0x3, 0x0b, 0xa0a1}, {0x3, 0x0f, 0x5046},
	},
	.do_toggle = true,
	.mdio_reset_bit = PCIE_PHY_CTRL_MDRST0 | PCIE_PHY_CTRL_BIT30,
	.disable_bit = PCIE_PHY_CTRL_DIS0,
};

static const struct phy_cfg rtl9607c_revB_gen1x1_phy_cfg = {
	.gen1_param = {
		{0x0, 0x01, 0xa852}, {0x0, 0x06, 0x0017}, {0x0, 0x08, 0x3591},
		{0x0, 0x09, 0x520c}, {0x0, 0x0a, 0xf670}, {0x0, 0x0b, 0xa90d},
		{0x0, 0x0d, 0xe720}, {0x0, 0x0e, 0x1010}, {0x0, 0x1c, 0x2001},
		{0x0, 0x1e, 0x66eb}, {0x1, 0x00, 0xd4a4}, {0x1, 0x01, 0x485a},
		{0x1, 0x03, 0x0b66}, {0x1, 0x04, 0x4f0c}, {0x1, 0x09, 0xf0f3},
		{0x1, 0x0b, 0xa0a1},
	},
	.gen2_param = { /* no parameter */ },
	.do_toggle = true,
	.mdio_reset_bit = PCIE_PHY_CTRL_MDRST1,
	.disable_bit = PCIE_PHY_CTRL_DIS1,
};

static const struct phy_cfg rtl9607c_revA_gen2x1_phy_cfg = {
	.gen1_param = {
		{0x0, 0x00, 0x4008}, {0x0, 0x01, 0xa812}, {0x0, 0x02, 0x6042},
		{0x0, 0x04, 0x5000}, {0x0, 0x05, 0x230a}, {0x0, 0x06, 0x0011},
		{0x0, 0x09, 0x520c}, {0x0, 0x0a, 0xc670}, {0x0, 0x0b, 0xb905},
		{0x0, 0x0d, 0xef16}, {0x0, 0x0e, 0x0000}, {0x1, 0x00, 0x9499},
		{0x1, 0x01, 0x66aa}, {0x1, 0x07, 0x011a},
	},
	.gen2_param = {
		{0x2, 0x00, 0x4008}, {0x2, 0x01, 0xa811}, {0x2, 0x02, 0x6042},
		{0x2, 0x04, 0x5000}, {0x2, 0x05, 0x230a}, {0x2, 0x06, 0x0011},
		{0x2, 0x09, 0x520c}, {0x2, 0x0a, 0xc670}, {0x2, 0x0b, 0xb905},
		{0x2, 0x0d, 0xef16}, {0x2, 0x0e, 0x0000}, {0x2, 0x0f, 0x000c},
		{0x2, 0x1b, 0xaea1}, {0x2, 0x1e, 0x28eb}, {0x3, 0x00, 0x94aa},
		{0x3, 0x01, 0x88ff}, {0x3, 0x02, 0x0093}, {0x3, 0x07, 0x011a},
		{0x3, 0x0f, 0x65bd},
	},
	.do_toggle = true,
	.mdio_reset_bit = PCIE_PHY_CTRL_MDRST0 | PCIE_PHY_CTRL_BIT30,
	.disable_bit = PCIE_PHY_CTRL_DIS0,
};

static const struct phy_cfg rtl9607c_revA_gen1x1_phy_cfg = {
	.gen1_param = {
		{0x0, 0x00, 0x8a50}, {0x0, 0x02, 0x26f9}, {0x0, 0x03, 0x6bcd},
		{0x0, 0x04, 0x8049}, {0x0, 0x06, 0x1088}, {0x0, 0x07, 0x52b3},
		{0x0, 0x08, 0x5285}, {0x0, 0x09, 0x6300}, {0x0, 0x0b, 0x0009},
		{0x0, 0x0c, 0x0800}, {0x0, 0x0e, 0x0093}, {0x1, 0x00, 0x0105},
		{0x1, 0x01, 0x1000},
	},
	.gen2_param = { /* no parameter */ },
	.do_toggle = false,
	.mdio_reset_bit = PCIE_PHY_CTRL_MDRST1,
	.disable_bit = PCIE_PHY_CTRL_DIS1,
};

static const struct of_device_id rtk_pcie_phy_dt_match[] = {
	{ .compatible = "realtek,rtl9607c-revA-gen1x1-pcie-phy", .data = &rtl9607c_revA_gen1x1_phy_cfg },
	{ .compatible = "realtek,rtl9607c-revA-gen2x1-pcie-phy", .data = &rtl9607c_revA_gen2x1_phy_cfg },
	{ .compatible = "realtek,rtl9607c-revB-gen1x1-pcie-phy", .data = &rtl9607c_revB_gen1x1_phy_cfg },
	{ .compatible = "realtek,rtl9607c-revB-gen2x1-pcie-phy", .data = &rtl9607c_revB_gen2x1_phy_cfg },
	{},
};
MODULE_DEVICE_TABLE(of, rtk_pcie_phy_dt_match);

static struct platform_driver rtk_pcie_phy_driver = {
	.probe		= rtk_pcie_phy_probe,
	.driver		= {
		.name	= "rtk-pcie-phy",
		.of_match_table = rtk_pcie_phy_dt_match,
	},
};

module_platform_driver(rtk_pcie_phy_driver);

MODULE_DESCRIPTION("Realtek PCIe PHY driver");
MODULE_LICENSE("GPL");
