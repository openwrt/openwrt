/*
 *  Atheros AR71xx built-in ethernet mac driver
 *
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Based on Atheros' AG7100 driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/clk.h>
#include <linux/of_mdio.h>
#include "ag71xx.h"

#define AG71XX_MDIO_RETRY	1000
#define AG71XX_MDIO_DELAY	5

static int bus_count;

static int ag71xx_mdio_wait_busy(struct ag71xx_mdio *am)
{
	int i;

	for (i = 0; i < AG71XX_MDIO_RETRY; i++) {
		u32 busy;

		udelay(AG71XX_MDIO_DELAY);

		regmap_read(am->mii_regmap, AG71XX_REG_MII_IND, &busy);
		if (!busy)
			return 0;

		udelay(AG71XX_MDIO_DELAY);
	}

	pr_err("%s: MDIO operation timed out\n", am->mii_bus->name);

	return -ETIMEDOUT;
}

static int ag71xx_mdio_mii_read(struct mii_bus *bus, int addr, int reg)
{
	struct ag71xx_mdio *am = bus->priv;
	int err;
	int ret;

	err = ag71xx_mdio_wait_busy(am);
	if (err)
		return 0xffff;

	regmap_write(am->mii_regmap, AG71XX_REG_MII_CMD, MII_CMD_WRITE);
	regmap_write(am->mii_regmap, AG71XX_REG_MII_ADDR,
			((addr & 0xff) << MII_ADDR_SHIFT) | (reg & 0xff));
	regmap_write(am->mii_regmap, AG71XX_REG_MII_CMD, MII_CMD_READ);

	err = ag71xx_mdio_wait_busy(am);
	if (err)
		return 0xffff;

	regmap_read(am->mii_regmap, AG71XX_REG_MII_STATUS, &ret);
	ret &= 0xffff;
	regmap_write(am->mii_regmap, AG71XX_REG_MII_CMD, MII_CMD_WRITE);

	DBG("mii_read: addr=%04x, reg=%04x, value=%04x\n", addr, reg, ret);

	return ret;
}

static int ag71xx_mdio_mii_write(struct mii_bus *bus, int addr, int reg, u16 val)
{
	struct ag71xx_mdio *am = bus->priv;

	DBG("mii_write: addr=%04x, reg=%04x, value=%04x\n", addr, reg, val);

	regmap_write(am->mii_regmap, AG71XX_REG_MII_ADDR,
			((addr & 0xff) << MII_ADDR_SHIFT) | (reg & 0xff));
	regmap_write(am->mii_regmap, AG71XX_REG_MII_CTRL, val);

	ag71xx_mdio_wait_busy(am);

	return 0;
}

static int ar934x_mdio_clock_div(unsigned int rate)
{
	if (rate == 100 * 1000 * 1000)
		return 6; /* 100 MHz clock divided by 20 => 5 MHz */
	else if (rate == 25 * 1000 * 1000)
		return 0; /* 25 MHz clock divided by 4 => 6.25 MHz */
	else
		return 3; /* 40 MHz clock divided by 8 => 5 MHz */
}

static int ag71xx_mdio_reset(struct mii_bus *bus)
{
	struct device_node *np = bus->dev.of_node;
	struct ag71xx_mdio *am = bus->priv;
	bool builtin_switch;
	u32 t;

	builtin_switch = of_property_read_bool(np, "builtin-switch");

	if (of_device_is_compatible(np, "qca,ar7240-mdio"))
		t = MII_CFG_CLK_DIV_6;
	else if (of_device_is_compatible(np, "qca,ar9340-mdio"))
		t = MII_CFG_CLK_DIV_58;
	else if (builtin_switch)
		t = MII_CFG_CLK_DIV_10;
	else
		t = MII_CFG_CLK_DIV_28;

	if (builtin_switch && of_device_is_compatible(np, "qca,ar9340-mdio")) {
		struct clk *ref_clk = of_clk_get(np, 0);
		int clock_rate;

		if (WARN_ON_ONCE(!ref_clk))
			clock_rate = 40 * 1000 * 1000;
		else
			clock_rate = clk_get_rate(ref_clk);

		t = ar934x_mdio_clock_div(clock_rate);
		clk_put(ref_clk);
	}

	regmap_write(am->mii_regmap, AG71XX_REG_MII_CFG, t | MII_CFG_RESET);
	udelay(100);

	regmap_write(am->mii_regmap, AG71XX_REG_MII_CFG, t);
	udelay(100);

	return 0;
}

static int ag71xx_mdio_probe(struct platform_device *pdev)
{
	struct device *amdev = &pdev->dev;
	struct device_node *np = pdev->dev.of_node;
	struct ag71xx_mdio *am;
	struct mii_bus *mii_bus;
	bool builtin_switch;
	int i, err;

	am = devm_kzalloc(amdev, sizeof(*am), GFP_KERNEL);
	if (!am)
		return -ENOMEM;

	am->mii_regmap = syscon_regmap_lookup_by_phandle(np, "regmap");
	if (!am->mii_regmap)
		return -ENOENT;

	mii_bus = devm_mdiobus_alloc(amdev);
	if (!mii_bus)
		return -ENOMEM;

	am->mdio_reset = of_reset_control_get_exclusive(np, "mdio");
	builtin_switch = of_property_read_bool(np, "builtin-switch");

	mii_bus->name = "ag71xx_mdio";
	mii_bus->read = ag71xx_mdio_mii_read;
	mii_bus->write = ag71xx_mdio_mii_write;
	mii_bus->reset = ag71xx_mdio_reset;
	mii_bus->priv = am;
	mii_bus->parent = amdev;
	snprintf(mii_bus->id, MII_BUS_ID_SIZE, "%s.%d", np->name, bus_count++);

	if (!builtin_switch &&
	    of_property_read_u32(np, "phy-mask", &mii_bus->phy_mask))
		mii_bus->phy_mask = 0;

	for (i = 0; i < PHY_MAX_ADDR; i++)
		mii_bus->irq[i] = PHY_POLL;

	if (!IS_ERR(am->mdio_reset)) {
		reset_control_assert(am->mdio_reset);
		msleep(100);
		reset_control_deassert(am->mdio_reset);
		msleep(200);
	}

	err = of_mdiobus_register(mii_bus, np);
	if (err)
		return err;

	am->mii_bus = mii_bus;
	platform_set_drvdata(pdev, am);

	return 0;
}

static int ag71xx_mdio_remove(struct platform_device *pdev)
{
	struct ag71xx_mdio *am = platform_get_drvdata(pdev);

	mdiobus_unregister(am->mii_bus);
	return 0;
}

static const struct of_device_id ag71xx_mdio_match[] = {
	{ .compatible = "qca,ar7240-mdio" },
	{ .compatible = "qca,ar9340-mdio" },
	{ .compatible = "qca,ath79-mdio" },
	{}
};

static struct platform_driver ag71xx_mdio_driver = {
	.probe		= ag71xx_mdio_probe,
	.remove		= ag71xx_mdio_remove,
	.driver = {
		.name	 = "ag71xx-mdio",
		.of_match_table = ag71xx_mdio_match,
	}
};

module_platform_driver(ag71xx_mdio_driver);
MODULE_LICENSE("GPL");
