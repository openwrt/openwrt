/*
 *  Driver for Micrel/Kendin PHYs
 *
 *  Copyright (c) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/delay.h>
#include <linux/skbuff.h>
#include <linux/phy.h>

#define KSZ_REG_INT_CTRL	0x1b

#define KSZ_INT_LU_EN	(1 << 8)	/* enable Link Up interrupt */
#define KSZ_INT_RF_EN	(1 << 9)	/* enable Remote Fault interrupt */
#define KSZ_INT_LD_EN	(1 << 10)	/* enable Link Down interrupt */

#define KSZ_INT_INIT	(KSZ_INT_LU_EN | KSZ_INT_LD_EN)

static int ksz8041_ack_interrupt(struct phy_device *phydev)
{
	int err;

	err = phy_read(phydev, KSZ_REG_INT_CTRL);

	return (err < 0) ? err : 0;
}

static int ksz8041_config_intr(struct phy_device *phydev)
{
	int err;

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED)
		err = phy_write(phydev, KSZ_REG_INT_CTRL,
				KSZ_INT_INIT);
	else
		err = phy_write(phydev, KSZ_REG_INT_CTRL, 0);

	return err;
}

static struct phy_driver ksz8041_phy_driver = {
	.phy_id		= 0x00221512,
	.name		= "Micrel KSZ8041",
	.phy_id_mask	= 0x001fffff,
	.features	= PHY_BASIC_FEATURES,
	.flags		= PHY_HAS_INTERRUPT,
	.config_aneg	= genphy_config_aneg,
	.read_status	= genphy_read_status,
	.ack_interrupt	= ksz8041_ack_interrupt,
	.config_intr	= ksz8041_config_intr,
	.driver	= {
		.owner	= THIS_MODULE,
	},
};

static int __init micrel_phy_init(void)
{
	return phy_driver_register(&ksz8041_phy_driver);
}

static void __exit micrel_phy_exit(void)
{
	phy_driver_unregister(&ksz8041_phy_driver);
}

#ifdef MODULE
module_init(micrel_phy_init);
module_exit(micrel_phy_exit);
#else
subsys_initcall(micrel_phy_init);
#endif

MODULE_DESCRIPTION("Micrel/Kendin PHY driver");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Imre Kaloz <kaloz@openwrt.org>");
MODULE_LICENSE("GPL v2");
