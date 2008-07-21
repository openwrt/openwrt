/*
 *  Atheros AR71xx built-in ethernet mac driver
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Based on Atheros' AG7100 driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "ag71xx.h"

#define AG71XX_MII_RETRY	1000
#define AG71XX_MII_DELAY	5

static inline void ag71xx_mii_ctrl_wr(struct ag71xx *ag, u32 value)
{
	__raw_writel(value, ag->mii_ctrl);
}

static inline u32 ag71xx_mii_ctrl_rr(struct ag71xx *ag)
{
	return __raw_readl(ag->mii_ctrl);
}

void ag71xx_mii_ctrl_set_if(struct ag71xx *ag, unsigned int mii_if)
{
	u32 t;

	t = ag71xx_mii_ctrl_rr(ag);
	t &= ~(0x3);
	t |= (mii_if & 0x3);
	ag71xx_mii_ctrl_wr(ag, t);
}

void ag71xx_mii_ctrl_set_speed(struct ag71xx *ag, unsigned int speed)
{
	u32 t;

	t = ag71xx_mii_ctrl_rr(ag);
	t &= ~(0x3 << 4);
	t |= (speed & 0x3) << 4;
	ag71xx_mii_ctrl_wr(ag, t);
}

static int ag71xx_mii_read(struct ag71xx *ag, int addr, int reg)
{
	int ret;
	int i;

	ag71xx_wr(ag, AG71XX_REG_MII_CMD, MII_CMD_WRITE);
	ag71xx_wr(ag, AG71XX_REG_MII_ADDR,
			((addr & 0xff) << MII_ADDR_S) | (reg & 0xff));
	ag71xx_wr(ag, AG71XX_REG_MII_CMD, MII_CMD_READ);

	i = AG71XX_MII_RETRY;
	while (ag71xx_rr(ag, AG71XX_REG_MII_IND) & MII_IND_BUSY) {
		if (i-- == 0) {
			printk(KERN_ERR "%s: mii_read timed out\n",
				ag->dev->name);
			ret = 0xffff;
			goto out;
		}
		udelay(AG71XX_MII_DELAY);
	}

	ret = ag71xx_rr(ag, AG71XX_REG_MII_STATUS) & 0xffff;
	ag71xx_wr(ag, AG71XX_REG_MII_CMD, MII_CMD_WRITE);

	DBG("mii_read: addr=%04x, reg=%04x, value=%04x\n", addr, reg, ret);

out:
	return ret;
}

static void ag71xx_mii_write(struct ag71xx *ag, int addr, int reg, u16 val)
{
	int i;

	DBG("mii_write: addr=%04x, reg=%04x, value=%04x\n", addr, reg, val);

	ag71xx_wr(ag, AG71XX_REG_MII_ADDR,
			((addr & 0xff) << MII_ADDR_S) | (reg & 0xff));
	ag71xx_wr(ag, AG71XX_REG_MII_CTRL, val);

	i = AG71XX_MII_RETRY;
	while (ag71xx_rr(ag, AG71XX_REG_MII_IND) & MII_IND_BUSY) {
		if (i-- == 0) {
			printk(KERN_ERR "%s: mii_write timed out\n",
				ag->dev->name);
			break;
		}
		udelay(AG71XX_MII_DELAY);
	}
}

int ag71xx_mii_peek(struct ag71xx *ag)
{
	int cnt;
	int i;

	cnt = 0;
	for (i = 0; i < PHY_MAX_ADDR; i++) {
		u16 bmsr, id1, id2, bmcr, advert, lpa;

		bmsr = ag71xx_mii_read(ag, i, MII_BMSR);
		bmcr = ag71xx_mii_read(ag, i, MII_BMCR);
		id1 = ag71xx_mii_read(ag, i, MII_PHYSID1);
		id2 = ag71xx_mii_read(ag, i, MII_PHYSID2);
		advert = ag71xx_mii_read(ag, i, MII_ADVERTISE);
		lpa = ag71xx_mii_read(ag, i, MII_LPA);
		DBG("%s: phy%02d bmsr=%04x, bmcr=%04x, "
			"id=%04x.%04x, advertise=%04x, lpa=%04x\n",
			ag->dev->name, i,
			bmsr, bmcr, id1, id2, advert, lpa);

		if ((bmsr | bmcr | id1 | id2 | advert | lpa) != 0)
			cnt++;
	}

	return cnt;
}

#define PLL_SEC_CONFIG		0x18050004
#define PLL_ETH0_INT_CLOCK	0x18050010
#define PLL_ETH1_INT_CLOCK	0x18050014
#define PLL_ETH_EXT_CLOCK	0x18050018

#define ag7100_pll_shift(_ag)   (((_ag)->pdev->id) ? 19 : 17)
#define ag7100_pll_offset(_ag)	(((_ag)->pdev->id) ? PLL_ETH1_INT_CLOCK \
						   : PLL_ETH0_INT_CLOCK)

static void ag71xx_set_pll(struct ag71xx *ag, u32 pll_val)
{
	void __iomem *pll_reg = ioremap_nocache(ag7100_pll_offset(ag), 4);
	void __iomem *pll_cfg = ioremap_nocache(PLL_SEC_CONFIG, 4);
	u32 s;
	u32 t;

	s = ag7100_pll_shift(ag);

	t = __raw_readl(pll_cfg);
	t &= ~(3 << s);
	t |=  (2 << s);
	__raw_writel(t, pll_cfg);
	udelay(100);

	__raw_writel(pll_val, pll_reg);

	t |= (3 << s);
	__raw_writel(t, pll_cfg);
	udelay(100);

	t &= ~(3 << s);
	__raw_writel(t, pll_cfg);
	udelay(100);
	DBG("%s: pll_reg %#x: %#x\n", ag->dev->name,
			(unsigned int)pll_reg, __raw_readl(pll_reg));

	iounmap(pll_cfg);
	iounmap(pll_reg);
}

static unsigned char *ag71xx_speed_str(struct ag71xx *ag)
{
	switch (ag->speed) {
	case SPEED_1000:
		return "1000";
	case SPEED_100:
		return "100";
	case SPEED_10:
		return "10";
	}

	return "?";
}

#if 1
#define PLL_VAL_1000	0x00110000
#define PLL_VAL_100	0x00001099
#define PLL_VAL_10	0x00991099
#else
#define PLL_VAL_1000	0x01111000
#define PLL_VAL_100	0x09991000
#define PLL_VAL_10	0x09991999
#endif

void ag71xx_link_update(struct ag71xx *ag)
{
	u32 cfg2;
	u32 ifctl;
	u32 pll;
	u32 fifo5;
	u32 mii_speed;

	if (!ag->link) {
		netif_carrier_off(ag->dev);
		printk(KERN_INFO "%s: link down\n", ag->dev->name);
		return;
	}

	cfg2 = ag71xx_rr(ag, AG71XX_REG_MAC_CFG2);
	cfg2 &= ~(MAC_CFG2_IF_1000 | MAC_CFG2_IF_10_100 | MAC_CFG2_FDX);
	cfg2 |= (ag->duplex) ? MAC_CFG2_FDX : 0;

	ifctl = ag71xx_rr(ag, AG71XX_REG_MAC_IFCTL);
	ifctl &= ~(MAC_IFCTL_SPEED);

	fifo5 = ag71xx_rr(ag, AG71XX_REG_FIFO_CFG5);
	fifo5 &= ~FIFO_CFG5_BYTE_PER_CLK;

	switch (ag->speed) {
	case SPEED_1000:
		mii_speed =  MII_CTRL_SPEED_1000;
		cfg2 |= MAC_CFG2_IF_1000;
		pll = PLL_VAL_1000;
		fifo5 |= FIFO_CFG5_BYTE_PER_CLK;
		break;
	case SPEED_100:
		mii_speed = MII_CTRL_SPEED_100;
		cfg2 |= MAC_CFG2_IF_10_100;
		ifctl |= MAC_IFCTL_SPEED;
		pll = PLL_VAL_100;
		break;
	case SPEED_10:
		mii_speed = MII_CTRL_SPEED_10;
		cfg2 |= MAC_CFG2_IF_10_100;
		pll = PLL_VAL_10;
		break;
	default:
		BUG();
		return;
	}

	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG3, 0x008001ff);
	ag71xx_set_pll(ag, pll);
	ag71xx_mii_ctrl_set_speed(ag, mii_speed);

	ag71xx_wr(ag, AG71XX_REG_MAC_CFG2, cfg2);
	ag71xx_wr(ag, AG71XX_REG_FIFO_CFG5, fifo5);
	ag71xx_wr(ag, AG71XX_REG_MAC_IFCTL, ifctl);

	netif_carrier_on(ag->dev);
	printk(KERN_INFO "%s: link up (%sMbps/%s duplex)\n",
		ag->dev->name,
		ag71xx_speed_str(ag),
		(DUPLEX_FULL == ag->duplex) ? "Full" : "Half");

	DBG("%s: fifo1=%#x, fifo2=%#x, fifo3=%#x, fifo4=%#x, fifo5=%#x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG1),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG2),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG3),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG4),
		ag71xx_rr(ag, AG71XX_REG_FIFO_CFG5));

	DBG("%s: mac_cfg2=%#x, ifctl=%#x, mii_ctrl=%#x\n",
		ag->dev->name,
		ag71xx_rr(ag, AG71XX_REG_MAC_CFG2),
		ag71xx_rr(ag, AG71XX_REG_MAC_IFCTL),
		ag71xx_mii_ctrl_rr(ag));
}

static void ag71xx_link_adjust(struct net_device *dev)
{
	struct ag71xx *ag = netdev_priv(dev);
	struct phy_device *phydev = ag->phy_dev;
	unsigned long flags;
	int status_change = 0;

	spin_lock_irqsave(&ag->lock, flags);

	if (phydev->link) {
		if (ag->duplex != phydev->duplex
		    || ag->speed != phydev->speed) {
			status_change = 1;
		}
	}

	if (phydev->link != ag->link) {
		if (phydev->link)
			netif_schedule(dev);

		status_change = 1;
	}

	ag->link = phydev->link;
	ag->duplex = phydev->duplex;
	ag->speed = phydev->speed;

	if (status_change)
		ag71xx_link_update(ag);

	spin_unlock_irqrestore(&ag->lock, flags);
}

static int ag71xx_mdio_read(struct mii_bus *bus, int addr, int reg)
{
	struct ag71xx *ag = bus->priv;

	return ag71xx_mii_read(ag, addr, reg);
}

static int ag71xx_mdio_write(struct mii_bus *bus, int addr, int reg, u16 val)
{
	struct ag71xx *ag = bus->priv;

	ag71xx_mii_write(ag, addr, reg, val);
	return 0;
}

static int ag71xx_mdio_reset(struct mii_bus *bus)
{
	/* TODO */
	return 0;
}

static int ag71xx_mdio_probe(struct ag71xx *ag)
{
	struct net_device *dev = ag->dev;
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);
	struct phy_device *phydev = NULL;
	int phy_count = 0;
	int phy_addr;

	for (phy_addr = 0; phy_addr < PHY_MAX_ADDR; phy_addr++) {
		if (!(pdata->phy_mask & (1 << phy_addr)))
			continue;

		if (ag->mii_bus.phy_map[phy_addr] == NULL)
			continue;

		DBG("%s: PHY found at %s, uid=%08x\n",
			dev->name,
			ag->mii_bus.phy_map[phy_addr]->dev.bus_id,
			ag->mii_bus.phy_map[phy_addr]->phy_id);

		if (phydev == NULL)
			phydev = ag->mii_bus.phy_map[phy_addr];

		phy_count++;
	}

	switch (phy_count) {
	case 0:
		printk(KERN_ERR "%s: no PHY found\n", dev->name);
		return -ENODEV;
	case 1:
		ag->phy_dev = phy_connect(dev, phydev->dev.bus_id,
			&ag71xx_link_adjust, 0, pdata->phy_if_mode);

		if (IS_ERR(ag->phy_dev)) {
			printk(KERN_ERR "%s: could not connect to PHY at %s\n",
				dev->name, phydev->dev.bus_id);
			return PTR_ERR(ag->phy_dev);
		}

		/* mask with MAC supported features */
		phydev->supported &= (SUPPORTED_10baseT_Half
			| SUPPORTED_10baseT_Full
			| SUPPORTED_100baseT_Half
			| SUPPORTED_100baseT_Full
			| SUPPORTED_Autoneg
			| SUPPORTED_MII
			| SUPPORTED_TP);

		phydev->advertising = phydev->supported;

		printk(KERN_DEBUG "%s: connected to PHY at %s "
			"[uid=%08x, driver=%s]\n",
			dev->name, phydev->dev.bus_id,
			phydev->phy_id, phydev->drv->name);

		ag->link = 0;
		ag->speed = 0;
		ag->duplex = -1;
		break;
	default:
		ag->phy_dev = NULL;
		printk(KERN_DEBUG "%s: connected to multiple PHYs (%d)\n",
			dev->name, phy_count);
		break;
	}

	return 0;
}

int ag71xx_mdio_init(struct ag71xx *ag, int id)
{
	int err;
	int i;

	ag->mii_bus.name = "ag71xx_mii";
	ag->mii_bus.read = ag71xx_mdio_read;
	ag->mii_bus.write = ag71xx_mdio_write;
	ag->mii_bus.reset = ag71xx_mdio_reset;
	ag->mii_bus.id = id;
	ag->mii_bus.priv = ag;
	ag->mii_bus.dev = &ag->dev->dev;

	ag->mii_bus.irq = kmalloc(sizeof(*ag->mii_bus.irq) * PHY_MAX_ADDR,
				  GFP_KERNEL);
	if (!ag->mii_bus.irq) {
		err = -ENOMEM;
		goto err_out;
	}

	for (i = 0; i < PHY_MAX_ADDR; i++)
		ag->mii_bus.irq[i] = PHY_POLL;

	err = mdiobus_register(&ag->mii_bus);
	if (err)
		goto err_free_irqs;

	err = ag71xx_mdio_probe(ag);
	if (err)
		goto err_unregister_bus;

	return 0;

err_unregister_bus:
	mdiobus_unregister(&ag->mii_bus);
err_free_irqs:
	kfree(ag->mii_bus.irq);
err_out:
	return err;
}

void ag71xx_mdio_cleanup(struct ag71xx *ag)
{
	mdiobus_unregister(&ag->mii_bus);
	kfree(ag->mii_bus.irq);
}
