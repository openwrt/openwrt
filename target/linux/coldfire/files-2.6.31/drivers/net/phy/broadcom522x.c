/*
 *Copyright (C) 2009 Freescale Semiconductor, Inc. All rights reserved.
 *	Chenghu Wu <b16972@freescale.com>
 *
 * Driver for broadcom PHYs 522x
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/netdevice.h>

/* DP83865 phy identifier values */
#define BCM5222_PHY_ID	0x00406320

/* PHY Register */
#define BCM5222_TIMEOUT                 0x100

/* MII Registers */
#define BCM5222_CTRL                    0x00
#define BCM5222_STATUS                  0x01
#define BCM5222_ID_HIGH                 0x02
#define BCM5222_ID_LOW                  0x03
#define BCM5222_AN_ADV                  0x04
#define BCM5222_AN_LP                   0x05
#define BCM5222_AN_EXP                  0x06
#define BCM5222_AN_NEXTPG               0x07
#define BCM5222_AN_LP_NPTX              0x08
#define BCM5222_AUX_CS                  0x18
#define BCM5222_AUX_STATUS              0x19

/* CONTROL Bits */
#define BCM5222_CTRL_RESET              0x8000
#define BCM5222_CTRL_LOOPBACK           0x4000
#define BCM5222_CTRL_FORCE              0x2000
#define BCM5222_CTRL_AUTOEN             0x1000
#define BCM5222_CTRL_PWRDN              0x0800
#define BCM5222_CTRL_ISOLATE            0x0400
#define BCM5222_CTRL_RESTART            0x0200
#define BCM5222_CTRL_DUPLEX             0x0100
#define BCM5222_CTRL_COLLEN             0x0080

/* STATUS Bits */
#define BCM5222_STATUS_100T4            0x8000
#define BCM5222_STATUS_100TXFDX         0x4000
#define BCM5222_STATUS_100TX            0x2000
#define BCM5222_STATUS_10FDX            0x1000
#define BCM5222_STATUS_10               0x0800
#define BCM5222_STATUS_MF_PREAMBLE      0x0040
#define BCM5222_STATUS_AN_COMPLETE      0x0020
#define BCM5222_STATUS_REMOTE_FAULT     0x0010
#define BCM5222_STATUS_AN_CAPABLE       0x0008
#define BCM5222_STATUS_LINK             0x0004
#define BCM5222_STATUS_JABBER           0x0002
#define BCM5222_STATUS_EXT_CAP          0x0001

/* ID Values */
#define BCM5222_ID_HIGH_VAL             0x0040
#define BCM5222_ID_LOW_VAL              0x6320

/* Advertise Bits */
#define BCM5222_AN_ADV_NEXTPG           0x8000
#define BCM5222_AN_ADV_REMOTE_FAULT     0x2000
#define BCM5222_AN_ADV_PAUSE            0x0400
#define BCM5222_AN_ADV_100T4            0x0200
#define BCM5222_AN_ADV_100TXFDX         0x0100
#define BCM5222_AN_ADV_100TX            0x0080
#define BCM5222_AN_ADV_10FDX            0x0040
#define BCM5222_AN_ADV_10               0x0020
#define BCM5222_AN_ADV_8023             0x0001
#define BCM5222_AN_ADV_ALL              \
	(BCM5222_AN_ADV_100TXFDX | \
	BCM5222_AN_ADV_100TXFDX | \
	BCM5222_AN_ADV_100TX | \
	BCM5222_AN_ADV_10FDX | \
	BCM5222_AN_ADV_10 |    \
	BCM5222_AN_ADV_8023)

/* AUX CTRL/STATUS Bits */
#define BCM5222_AUX_CS_JABBER_DIS       0x8000
#define BCM5222_AUX_CS_FORCE_LINK       0x4000
#define BCM5222_AUX_CS_10M_TX_PWR       0x0100
#define BCM5222_AUX_CS_HSQ_LSQ_MASK     0x00c0
#define BCM5222_AUX_CS_EDGE_RATE_MASK   0x0030
#define BCM5222_AUX_CS_AN_IND           0x0008
#define BCM5222_AUX_CS_SPEED_FORCE      0x0004
#define BCM5222_AUX_CS_SPEED            0x0002
#define BCM5222_AUX_CS_DUPLEX           0x0001

/* AUX STATUS Bits */
#define BCM5222_AUX_STATUS_AN_COMP      0x8000
#define BCM5222_AUX_STATUS_AN_COMPACK   0x4000
#define BCM5222_AUX_STATUS_AN_ACKDET    0x2000
#define BCM5222_AUX_STATUS_AN_ABDET     0x1000
#define BCM5222_AUX_STATUS_AN_PAUSE     0x0800
#define BCM5222_AUX_STATUS_AN_HCDMASK   0x0700
#define BCM5222_AUX_STATUS_AN_PDFAULT   0x0080
#define BCM5222_AUX_STATUS_LP_RMTFAULT  0x0040
#define BCM5222_AUX_STATUS_LP_PGRX      0x0020
#define BCM5222_AUX_STATUS_LP_NEGABLE   0x0010
#define BCM5222_AUX_STATUS_SPEED        0x0008
#define BCM5222_AUX_STATUS_LINK         0x0004
#define BCM5222_AUX_STATUS_AN_EN        0x0002
#define BCM5222_AUX_STATUS_JABBER       0x0001

static int bcm5222_config_intr(struct phy_device *phydev)
{
	int err = 0;
	printk(KERN_INFO "%s PHY_INTERRUPT %x\n",
			__func__, phydev->interrupts);

	return err;
}

static int bcm5222_ack_interrupt(struct phy_device *phydev)
{
	return 0;
}

static int bcm5222_config_init(struct phy_device *phydev)
{
	return  bcm5222_ack_interrupt(phydev);
}

static int bcm5222_config_init_old(struct phy_device *phydev)
{
	int timeout;
	int flag = 1;
	int ret = phy_read(phydev, BCM5222_AUX_STATUS);
	if (ret < 0) {
		printk(KERN_INFO "%s MII_BCM5222_ISR %x\n",
			__func__, ret);
	}
	/*
	* reset
	*/
	phy_write(phydev, BCM5222_CTRL, BCM5222_CTRL_RESET);

	/* check that it cleared */
	ret = phy_read(phydev, BCM5222_CTRL);
	printk(KERN_INFO "%s BCM5222_CTRL %x\n",
		__func__, ret);
	/*if reset bit is set, return */
	if (ret & BCM5222_CTRL_RESET) {
		printk(KERN_ERR "%s %x = BCM5222_CTRL_RESET(%x)\n",
			__func__, ret, BCM5222_CTRL_RESET);
		return -ETIME;
	}

	/*
	* setup auto-negotiation
	*/

	/* disable */
	phy_write(phydev, BCM5222_CTRL, 0);
	ret = phy_read(phydev, BCM5222_CTRL);
	printk(KERN_INFO "%s BCM5222_CTRL %x\n",
		__func__, ret);
	/* set the auto-negotiation advertisement register */
	phy_write(phydev, BCM5222_AN_ADV, BCM5222_AN_ADV_ALL);
	ret = phy_read(phydev, BCM5222_AN_ADV);
	printk(KERN_INFO "%s BCM5222_AN_ADV %x, BCM5222_AN_ADV_ALL %x\n",
		__func__, ret, BCM5222_AN_ADV_ALL);
	/* enable */
	phy_write(phydev, BCM5222_CTRL, BCM5222_CTRL_AUTOEN);
	ret = phy_read(phydev, BCM5222_CTRL);
	printk(KERN_INFO "%s BCM5222_CTRL %x\n",
			__func__, ret);
	printk(KERN_INFO "** wait for complete\n");

	/* read aux status reg */
	ret = phy_read(phydev, BCM5222_AUX_STATUS);
	/* Wait for the auto-negotiation completion */
	timeout = BCM5222_TIMEOUT;
	while (!(ret & BCM5222_AUX_STATUS_AN_COMP)) {
		if (!timeout--) {
			flag = 0;
			printk(KERN_INFO "BCM5222: TIMEOUT\n");
			break;
		}

		mdelay(10);
		/* Read PHY status register */
		ret = phy_read(phydev, BCM5222_AUX_STATUS);
	}

	ret = phy_read(phydev, BCM5222_AUX_STATUS);
	ret = phy_read(phydev, BCM5222_AN_ADV);
	return 0;
}

static int bcm5222_read_status(struct phy_device *phydev)
{
	int ret;
	ret = phy_read(phydev, BCM5222_AUX_STATUS);
	printk(KERN_INFO "%s ret %x\n", __func__, ret);

	if (ret & BCM5222_AUX_STATUS_LINK)
		phydev->link = 1;
	else
		phydev->link = 0;

	if (ret & BCM5222_AUX_STATUS_SPEED)
		phydev->speed = SPEED_100;
	else
		phydev->speed = SPEED_10;

	ret = phy_read(phydev, BCM5222_AUX_CS);
	printk(KERN_INFO "%s ret %x\n", __func__, ret);
	if (ret & BCM5222_AUX_CS_DUPLEX)
		phydev->duplex = DUPLEX_FULL;
	else
		phydev->duplex = DUPLEX_HALF;
	return 0;
}

static int bcm5222_config_aneg(struct phy_device *phydev)
{
	phy_read(phydev, BCM5222_AUX_STATUS);
	phy_read(phydev, BCM5222_AN_ADV);
	return 0;
}

static struct phy_driver bcm5222_driver = {
	.phy_id = BCM5222_PHY_ID,
	.phy_id_mask = 0xfffffff0,
	.name = "Broadcom BCM5222",
	.features = PHY_BASIC_FEATURES,
	.flags = PHY_HAS_INTERRUPT,
	.config_init = bcm5222_config_init,
	.config_aneg = genphy_config_aneg,
	.read_status = genphy_read_status,
	.ack_interrupt = bcm5222_ack_interrupt,
	.config_intr = bcm5222_config_intr,
	.driver = {.owner = THIS_MODULE,}
};

static int __init bcm5222_init(void)
{
	int ret;

	ret = phy_driver_register(&bcm5222_driver);
	if (ret)
		goto err1;

	return 0;
err1:
	printk(KERN_INFO "register bcm5222 PHY driver fail\n");
	return ret;
}

static void __exit bcm5222_exit(void)
{
	phy_driver_unregister(&bcm5222_driver);
}

MODULE_DESCRIPTION("Broadcom PHY driver");
MODULE_LICENSE("GPL v2");

module_init(bcm5222_init);
module_exit(bcm5222_exit);
