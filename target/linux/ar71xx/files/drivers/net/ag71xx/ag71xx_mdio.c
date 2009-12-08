/*
 *  Atheros AR71xx built-in ethernet mac driver
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Based on Atheros' AG7100 driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "ag71xx.h"

#define AG71XX_MDIO_RETRY	1000
#define AG71XX_MDIO_DELAY	5

static inline void ag71xx_mdio_wr(struct ag71xx_mdio *am, unsigned reg,
				  u32 value)
{
	void __iomem *r;

	r = am->mdio_base + reg;
	__raw_writel(value, r);

	/* flush write */
	(void) __raw_readl(r);
}

static inline u32 ag71xx_mdio_rr(struct ag71xx_mdio *am, unsigned reg)
{
	return __raw_readl(am->mdio_base + reg);
}

static void ag71xx_mdio_dump_regs(struct ag71xx_mdio *am)
{
	DBG("%s: mii_cfg=%08x, mii_cmd=%08x, mii_addr=%08x\n",
		am->mii_bus->name,
		ag71xx_mdio_rr(am, AG71XX_REG_MII_CFG),
		ag71xx_mdio_rr(am, AG71XX_REG_MII_CMD),
		ag71xx_mdio_rr(am, AG71XX_REG_MII_ADDR));
	DBG("%s: mii_ctrl=%08x, mii_status=%08x, mii_ind=%08x\n",
		am->mii_bus->name,
		ag71xx_mdio_rr(am, AG71XX_REG_MII_CTRL),
		ag71xx_mdio_rr(am, AG71XX_REG_MII_STATUS),
		ag71xx_mdio_rr(am, AG71XX_REG_MII_IND));
}

static int ag71xx_mdio_mii_read(struct ag71xx_mdio *am, int addr, int reg)
{
	int ret;
	int i;

	ag71xx_mdio_wr(am, AG71XX_REG_MII_CMD, MII_CMD_WRITE);
	ag71xx_mdio_wr(am, AG71XX_REG_MII_ADDR,
			((addr & 0xff) << MII_ADDR_SHIFT) | (reg & 0xff));
	ag71xx_mdio_wr(am, AG71XX_REG_MII_CMD, MII_CMD_READ);

	i = AG71XX_MDIO_RETRY;
	while (ag71xx_mdio_rr(am, AG71XX_REG_MII_IND) & MII_IND_BUSY) {
		if (i-- == 0) {
			printk(KERN_ERR "%s: mii_read timed out\n",
				am->mii_bus->name);
			ret = 0xffff;
			goto out;
		}
		udelay(AG71XX_MDIO_DELAY);
	}

	ret = ag71xx_mdio_rr(am, AG71XX_REG_MII_STATUS) & 0xffff;
	ag71xx_mdio_wr(am, AG71XX_REG_MII_CMD, MII_CMD_WRITE);

	DBG("mii_read: addr=%04x, reg=%04x, value=%04x\n", addr, reg, ret);

 out:
	return ret;
}

static void ag71xx_mdio_mii_write(struct ag71xx_mdio *am,
				  int addr, int reg, u16 val)
{
	int i;

	DBG("mii_write: addr=%04x, reg=%04x, value=%04x\n", addr, reg, val);

	ag71xx_mdio_wr(am, AG71XX_REG_MII_ADDR,
			((addr & 0xff) << MII_ADDR_SHIFT) | (reg & 0xff));
	ag71xx_mdio_wr(am, AG71XX_REG_MII_CTRL, val);

	i = AG71XX_MDIO_RETRY;
	while (ag71xx_mdio_rr(am, AG71XX_REG_MII_IND) & MII_IND_BUSY) {
		if (i-- == 0) {
			printk(KERN_ERR "%s: mii_write timed out\n",
				am->mii_bus->name);
			break;
		}
		udelay(AG71XX_MDIO_DELAY);
	}
}

static int ag71xx_mdio_reset(struct mii_bus *bus)
{
	struct ag71xx_mdio *am = bus->priv;
	u32 t;

	if (am->pdata->is_ar7240)
		t = MII_CFG_CLK_DIV_6;
	else
		t = MII_CFG_CLK_DIV_28;

	ag71xx_mdio_wr(am, AG71XX_REG_MII_CFG, t | MII_CFG_RESET);
	udelay(100);

	ag71xx_mdio_wr(am, AG71XX_REG_MII_CFG, t);
	udelay(100);

	return 0;
}

static int ag71xx_mdio_read(struct mii_bus *bus, int addr, int reg)
{
	struct ag71xx_mdio *am = bus->priv;

	return ag71xx_mdio_mii_read(am, addr, reg);
}

static int ag71xx_mdio_write(struct mii_bus *bus, int addr, int reg, u16 val)
{
	struct ag71xx_mdio *am = bus->priv;

	ag71xx_mdio_mii_write(am, addr, reg, val);
	return 0;
}

static int __init ag71xx_mdio_probe(struct platform_device *pdev)
{
	struct ag71xx_mdio_platform_data *pdata;
	struct ag71xx_mdio *am;
	struct resource *res;
	int i;
	int err;

	pdata = pdev->dev.platform_data;
	if (!pdata) {
		dev_err(&pdev->dev, "no platform data specified\n");
		return -EINVAL;
	}

	am = kzalloc(sizeof(*am), GFP_KERNEL);
	if (!am) {
		err = -ENOMEM;
		goto err_out;
	}

	am->pdata = pdata;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "no iomem resource found\n");
		err = -ENXIO;
		goto err_out;
	}

	am->mdio_base = ioremap_nocache(res->start, res->end - res->start + 1);
	if (!am->mdio_base) {
		dev_err(&pdev->dev, "unable to ioremap registers\n");
		err = -ENOMEM;
		goto err_free_mdio;
	}

	am->mii_bus = mdiobus_alloc();
	if (am->mii_bus == NULL) {
		err = -ENOMEM;
		goto err_iounmap;
	}

	am->mii_bus->name = "ag71xx_mdio";
	am->mii_bus->read = ag71xx_mdio_read;
	am->mii_bus->write = ag71xx_mdio_write;
	am->mii_bus->reset = ag71xx_mdio_reset;
	am->mii_bus->irq = am->mii_irq;
	am->mii_bus->priv = am;
	am->mii_bus->parent = &pdev->dev;
	snprintf(am->mii_bus->id, MII_BUS_ID_SIZE, "%s", dev_name(&pdev->dev));
	am->mii_bus->phy_mask = pdata->phy_mask;

	for (i = 0; i < PHY_MAX_ADDR; i++)
		am->mii_irq[i] = PHY_POLL;

	ag71xx_mdio_wr(am, AG71XX_REG_MAC_CFG1, 0);

	err = mdiobus_register(am->mii_bus);
	if (err)
		goto err_free_bus;

	ag71xx_mdio_dump_regs(am);

	platform_set_drvdata(pdev, am);
	return 0;

 err_free_bus:
	mdiobus_free(am->mii_bus);
 err_iounmap:
	iounmap(am->mdio_base);
 err_free_mdio:
	kfree(am);
 err_out:
	return err;
}

static int __exit ag71xx_mdio_remove(struct platform_device *pdev)
{
	struct ag71xx_mdio *am = platform_get_drvdata(pdev);

	if (am) {
		mdiobus_unregister(am->mii_bus);
		mdiobus_free(am->mii_bus);
		iounmap(am->mdio_base);
		kfree(am);
		platform_set_drvdata(pdev, NULL);
	}

	return 0;
}

static struct platform_driver ag71xx_mdio_driver = {
	.probe		= ag71xx_mdio_probe,
	.remove		= __exit_p(ag71xx_mdio_remove),
	.driver = {
		.name	= "ag71xx-mdio",
	}
};

int ag71xx_mdio_driver_init(void)
{
	return platform_driver_register(&ag71xx_mdio_driver);
}

void ag71xx_mdio_driver_exit(void)
{
	platform_driver_unregister(&ag71xx_mdio_driver);
}
