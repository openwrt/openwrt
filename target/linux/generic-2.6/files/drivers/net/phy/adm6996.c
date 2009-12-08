/*
 * ADM6996 switch driver
 *
 * Copyright (c) 2008 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of the GNU General Public License v2 as published by the
 * Free Software Foundation
 */
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include "adm6996.h"

MODULE_DESCRIPTION("Infineon ADM6996 Switch");
MODULE_AUTHOR("Felix Fietkau");
MODULE_LICENSE("GPL");

struct adm6996_priv {
	/* use abstraction for regops, we want to add gpio support in the future */
	u16 (*read)(struct phy_device *phydev, enum admreg reg);
	void (*write)(struct phy_device *phydev, enum admreg reg, u16 val);
};

#define to_adm(_phy) ((struct adm6996_priv *) (_phy)->priv)


static inline u16
r16(struct phy_device *pdev, enum admreg reg)
{
	return to_adm(pdev)->read(pdev, reg);
}

static inline void
w16(struct phy_device *pdev, enum admreg reg, u16 val)
{
	to_adm(pdev)->write(pdev, reg, val);
}


static u16
adm6996_read_mii_reg(struct phy_device *phydev, enum admreg reg)
{
	return phydev->bus->read(phydev->bus, PHYADDR(reg));
}

static void
adm6996_write_mii_reg(struct phy_device *phydev, enum admreg reg, u16 val)
{
	phydev->bus->write(phydev->bus, PHYADDR(reg), val);
}


static int adm6996_config_init(struct phy_device *pdev)
{
	int i;

	printk("%s: ADM6996 PHY driver attached.\n", pdev->attached_dev->name);
	pdev->supported = ADVERTISED_100baseT_Full;
	pdev->advertising = ADVERTISED_100baseT_Full;

	/* initialize port and vlan settings */
	for (i = 0; i < ADM_PHY_PORTS; i++) {
		w16(pdev, adm_portcfg[i], ADM_PORTCFG_INIT |
			ADM_PORTCFG_PVID((i == ADM_WAN_PORT) ? 1 : 0));
	}
	w16(pdev, adm_portcfg[5], ADM_PORTCFG_CPU);

	/* reset all ports */
	for (i = 0; i < ADM_PHY_PORTS; i++) {
		w16(pdev, ADM_PHY_PORT(i), ADM_PHYCFG_INIT);
	}

	return 0;
}

static int adm6996_read_status(struct phy_device *phydev)
{
	phydev->speed = SPEED_100;
	phydev->duplex = DUPLEX_FULL;
	phydev->link = 1;
	return 0;
}

static int adm6996_config_aneg(struct phy_device *phydev)
{
	return 0;
}

static int adm6996_fixup(struct phy_device *dev)
{
	struct mii_bus *bus = dev->bus;
	u16 reg;

	/* look for the switch on the bus */
	reg = bus->read(bus, PHYADDR(ADM_SIG0)) & ADM_SIG0_MASK;
	if (reg != ADM_SIG0_VAL)
		return 0;

	reg = bus->read(bus, PHYADDR(ADM_SIG1)) & ADM_SIG1_MASK;
	if (reg != ADM_SIG1_VAL)
		return 0;

	dev->phy_id = (ADM_SIG0_VAL << 16) | ADM_SIG1_VAL;
	return 0;
}

static int adm6996_probe(struct phy_device *pdev)
{
	struct adm6996_priv *priv;

	priv = kzalloc(sizeof(struct adm6996_priv), GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	priv->read = adm6996_read_mii_reg;
	priv->write = adm6996_write_mii_reg;
	pdev->priv = priv;
	return 0;
}

static void adm6996_remove(struct phy_device *pdev)
{
	kfree(pdev->priv);
}


static struct phy_driver adm6996_driver = {
	.name		= "Infineon ADM6996",
	.phy_id		= (ADM_SIG0_VAL << 16) | ADM_SIG1_VAL,
	.phy_id_mask	= 0xffffffff,
	.features	= PHY_BASIC_FEATURES,
	.probe		= adm6996_probe,
	.remove		= adm6996_remove,
	.config_init	= &adm6996_config_init,
	.config_aneg	= &adm6996_config_aneg,
	.read_status	= &adm6996_read_status,
	.driver		= { .owner = THIS_MODULE,},
};

static int __init adm6996_init(void)
{
	phy_register_fixup_for_id(PHY_ANY_ID, adm6996_fixup);
	return phy_driver_register(&adm6996_driver);
}

static void __exit adm6996_exit(void)
{
	phy_driver_unregister(&adm6996_driver);
}

module_init(adm6996_init);
module_exit(adm6996_exit);
