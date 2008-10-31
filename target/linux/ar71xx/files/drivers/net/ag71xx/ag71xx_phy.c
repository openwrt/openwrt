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

#define PLL_SEC_CONFIG		0x18050004
#define PLL_ETH0_INT_CLOCK	0x18050010
#define PLL_ETH1_INT_CLOCK	0x18050014
#define PLL_ETH_EXT_CLOCK	0x18050018

#define ag71xx_pll_shift(_ag)   (((_ag)->pdev->id) ? 19 : 17)
#define ag71xx_pll_offset(_ag)	(((_ag)->pdev->id) ? PLL_ETH1_INT_CLOCK \
						   : PLL_ETH0_INT_CLOCK)

static void ag71xx_set_pll(struct ag71xx *ag, u32 pll_val)
{
	void __iomem *pll_reg = ioremap_nocache(ag71xx_pll_offset(ag), 4);
	void __iomem *pll_cfg = ioremap_nocache(PLL_SEC_CONFIG, 4);
	u32 s;
	u32 t;

	s = ag71xx_pll_shift(ag);

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

static void ag71xx_phy_link_update(struct ag71xx *ag)
{
	u32 cfg2;
	u32 ifctl;
	u32 pll;
	u32 fifo5;
	u32 mii_speed;

	if (!ag->link) {
		netif_carrier_off(ag->dev);
		if (netif_msg_link(ag))
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
	if (netif_msg_link(ag))
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

static void ag71xx_phy_link_adjust(struct net_device *dev)
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
		ag71xx_phy_link_update(ag);

	spin_unlock_irqrestore(&ag->lock, flags);
}

void ag71xx_phy_start(struct ag71xx *ag)
{
	if (ag->phy_dev) {
		phy_start(ag->phy_dev);
	} else {
		struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);

		ag->duplex = pdata->duplex;
		ag->speed = pdata->speed;
		ag->link = 1;
		ag71xx_phy_link_update(ag);
	}
}

void ag71xx_phy_stop(struct ag71xx *ag)
{
	if (ag->phy_dev) {
		phy_stop(ag->phy_dev);
	} else {
		ag->duplex = -1;
		ag->link = 0;
		ag->speed = 0;
		ag71xx_phy_link_update(ag);
	}
}

int ag71xx_phy_connect(struct ag71xx *ag)
{
	struct net_device *dev = ag->dev;
	struct ag71xx_platform_data *pdata = ag71xx_get_pdata(ag);
	struct phy_device *phydev = NULL;
	int phy_count = 0;
	int phy_addr;

	if (ag->mii_bus && pdata->phy_mask) {
		/* TODO: use mutex of the mdio bus? */
		for (phy_addr = 0; phy_addr < PHY_MAX_ADDR; phy_addr++) {
			if (!(pdata->phy_mask & (1 << phy_addr)))
				continue;

			if (ag->mii_bus->phy_map[phy_addr] == NULL)
				continue;

			DBG("%s: PHY found at %s, uid=%08x\n",
				dev->name,
				ag->mii_bus->phy_map[phy_addr]->dev.bus_id,
				ag->mii_bus->phy_map[phy_addr]->phy_id);

			if (phydev == NULL)
				phydev = ag->mii_bus->phy_map[phy_addr];

			phy_count++;
		}
	}

	switch (phy_count) {
	case 1:
		ag->phy_dev = phy_connect(dev, phydev->dev.bus_id,
			&ag71xx_phy_link_adjust, 0, pdata->phy_if_mode);

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
		switch (pdata->speed) {
		case SPEED_10:
		case SPEED_100:
		case SPEED_1000:
			break;
		default:
			printk(KERN_ERR "%s: invalid speed specified\n",
				dev->name);
			return -EINVAL;
		}

		ag->phy_dev = NULL;
		printk(KERN_DEBUG "%s: connected to %d PHYs\n",
			dev->name, phy_count);
		break;
	}

	return 0;
}

void ag71xx_phy_disconnect(struct ag71xx *ag)
{
	if (ag->phy_dev)
		phy_disconnect(ag->phy_dev);
}
