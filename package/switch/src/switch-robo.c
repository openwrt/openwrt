/*
 * Broadcom BCM5325E/536x switch configuration module
 *
 * Copyright (C) 2005 Felix Fietkau <nbd@nbd.name>
 * Copyright (C) 2008 Michael Buesch <mb@bu3sch.de>
 * Based on 'robocfg' by Oleg I. Vdovikin
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
 * 02110-1301, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/delay.h>
#include <asm/uaccess.h>

#include "switch-core.h"
#include "etc53xx.h"

#ifdef CONFIG_BCM47XX
#include <bcm47xx_nvram.h>
#endif

#define DRIVER_NAME		"bcm53xx"
#define DRIVER_VERSION		"0.02"
#define PFX			"roboswitch: "

#define ROBO_PHY_ADDR		0x1E	/* robo switch phy address */
#define ROBO_PHY_ADDR_TG3	0x01	/* Tigon3 PHY address */
#define ROBO_PHY_ADDR_BCM63XX	0x00	/* BCM63XX PHY address */

/* MII registers */
#define REG_MII_PAGE	0x10	/* MII Page register */
#define REG_MII_ADDR	0x11	/* MII Address register */
#define REG_MII_DATA0	0x18	/* MII Data register 0 */

#define REG_MII_PAGE_ENABLE	1
#define REG_MII_ADDR_WRITE	1
#define REG_MII_ADDR_READ	2

/* Robo device ID register (in ROBO_MGMT_PAGE) */
#define ROBO_DEVICE_ID		0x30
#define  ROBO_DEVICE_ID_5325	0x25 /* Faked */
#define  ROBO_DEVICE_ID_5395	0x95
#define  ROBO_DEVICE_ID_5397	0x97
#define  ROBO_DEVICE_ID_5398	0x98
#define  ROBO_DEVICE_ID_53115	0x3115

/* Private et.o ioctls */
#define SIOCGETCPHYRD           (SIOCDEVPRIVATE + 9)
#define SIOCSETCPHYWR           (SIOCDEVPRIVATE + 10)

/* Data structure for a Roboswitch device. */
struct robo_switch {
	char *device;			/* The device name string (ethX) */
	u16 devid;			/* ROBO_DEVICE_ID_53xx */
	bool is_5350;
	struct ifreq ifr;
	struct net_device *dev;
	unsigned char port[6];
};

/* Currently we can only have one device in the system. */
static struct robo_switch robo;


static int do_ioctl(int cmd)
{
	mm_segment_t old_fs = get_fs();
	int ret;

	set_fs(KERNEL_DS);
	ret = robo.dev->netdev_ops->ndo_do_ioctl(robo.dev, &robo.ifr, cmd);
	set_fs(old_fs);

	return ret;
}

static u16 mdio_read(__u16 phy_id, __u8 reg)
{
	struct mii_ioctl_data *mii = if_mii(&robo.ifr);
	int err;

	mii->phy_id = phy_id;
	mii->reg_num = reg;

	err = do_ioctl(SIOCGMIIREG);
	if (err < 0) {
		printk(KERN_ERR PFX "failed to read mdio reg %i with err %i.\n", reg, err);

		return 0xffff;
	}

	return mii->val_out;
}

static void mdio_write(__u16 phy_id, __u8 reg, __u16 val)
{
	struct mii_ioctl_data *mii = if_mii(&robo.ifr);
	int err;

	mii->phy_id = phy_id;
	mii->reg_num = reg;
	mii->val_in = val;

	err = do_ioctl(SIOCSMIIREG);
	if (err < 0) {
		printk(KERN_ERR PFX "failed to write mdio reg: %i with err %i.\n", reg, err);
		return;
	}
}

static int robo_reg(__u8 page, __u8 reg, __u8 op)
{
	int i = 3;

	/* set page number */
	mdio_write(ROBO_PHY_ADDR, REG_MII_PAGE,
		(page << 8) | REG_MII_PAGE_ENABLE);

	/* set register address */
	mdio_write(ROBO_PHY_ADDR, REG_MII_ADDR,
		(reg << 8) | op);

	/* check if operation completed */
	while (i--) {
		if ((mdio_read(ROBO_PHY_ADDR, REG_MII_ADDR) & 3) == 0)
			return 0;
	}

	printk(KERN_ERR PFX "timeout in robo_reg on page %i and reg %i with op %i.\n", page, reg, op);

	return 0;
}

/*
static void robo_read(__u8 page, __u8 reg, __u16 *val, int count)
{
	int i;

	robo_reg(page, reg, REG_MII_ADDR_READ);

	for (i = 0; i < count; i++)
		val[i] = mdio_read(ROBO_PHY_ADDR, REG_MII_DATA0 + i);
}
*/

static __u16 robo_read16(__u8 page, __u8 reg)
{
	robo_reg(page, reg, REG_MII_ADDR_READ);

	return mdio_read(ROBO_PHY_ADDR, REG_MII_DATA0);
}

static __u32 robo_read32(__u8 page, __u8 reg)
{
	robo_reg(page, reg, REG_MII_ADDR_READ);

	return mdio_read(ROBO_PHY_ADDR, REG_MII_DATA0) +
		(mdio_read(ROBO_PHY_ADDR, REG_MII_DATA0 + 1) << 16);
}

static void robo_write16(__u8 page, __u8 reg, __u16 val16)
{
	/* write data */
	mdio_write(ROBO_PHY_ADDR, REG_MII_DATA0, val16);

	robo_reg(page, reg, REG_MII_ADDR_WRITE);
}

static void robo_write32(__u8 page, __u8 reg, __u32 val32)
{
	/* write data */
	mdio_write(ROBO_PHY_ADDR, REG_MII_DATA0, val32 & 65535);
	mdio_write(ROBO_PHY_ADDR, REG_MII_DATA0 + 1, val32 >> 16);

	robo_reg(page, reg, REG_MII_ADDR_WRITE);
}

/* checks that attached switch is 5325E/5350 */
static int robo_vlan5350(void)
{
	/* set vlan access id to 15 and read it back */
	__u16 val16 = 15;
	robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS_5350, val16);

	/* 5365 will refuse this as it does not have this reg */
	return (robo_read16(ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS_5350) == val16);
}

static int robo_switch_enable(void)
{
	unsigned int i, last_port;
	u16 val;
#ifdef CONFIG_BCM47XX
	char buf[20];
#endif

	val = robo_read16(ROBO_CTRL_PAGE, ROBO_SWITCH_MODE);
	if (!(val & (1 << 1))) {
		/* Unmanaged mode */
		val &= ~(1 << 0);
		/* With forwarding */
		val |= (1 << 1);
		robo_write16(ROBO_CTRL_PAGE, ROBO_SWITCH_MODE, val);
		val = robo_read16(ROBO_CTRL_PAGE, ROBO_SWITCH_MODE);
		if (!(val & (1 << 1))) {
			printk(KERN_ERR PFX "Failed to enable switch\n");
			return -EBUSY;
		}

		last_port = (robo.devid == ROBO_DEVICE_ID_5398) ?
				ROBO_PORT6_CTRL : ROBO_PORT3_CTRL;
		for (i = ROBO_PORT0_CTRL; i < last_port + 1; i++)
			robo_write16(ROBO_CTRL_PAGE, i, 0);
	}

#ifdef CONFIG_BCM47XX
	/* WAN port LED, except for Netgear WGT634U */
	if (bcm47xx_nvram_getenv("nvram_type", buf, sizeof(buf)) >= 0) {
		if (strcmp(buf, "cfe") != 0)
			robo_write16(ROBO_CTRL_PAGE, 0x16, 0x1F);
	}
#endif
	return 0;
}

static void robo_switch_reset(void)
{
	if ((robo.devid == ROBO_DEVICE_ID_5395) ||
	    (robo.devid == ROBO_DEVICE_ID_5397) ||
	    (robo.devid == ROBO_DEVICE_ID_5398)) {
		/* Trigger a software reset. */
		robo_write16(ROBO_CTRL_PAGE, 0x79, 0x83);
		mdelay(500);
		robo_write16(ROBO_CTRL_PAGE, 0x79, 0);
	}
}

static int robo_probe(char *devname)
{
	__u32 phyid;
	unsigned int i;
	int err = 1;

	printk(KERN_INFO PFX "Probing device '%s'\n", devname);
	strcpy(robo.ifr.ifr_name, devname);

	if ((robo.dev = dev_get_by_name(&init_net, devname)) == NULL) {
		printk(KERN_ERR PFX "No such device\n");
		return 1;
	}
	if (!robo.dev->netdev_ops || !robo.dev->netdev_ops->ndo_do_ioctl) {
		printk(KERN_ERR PFX "ndo_do_ioctl not implemented in ethernet driver\n");
		return 1;
	}

	robo.device = devname;
	for (i = 0; i < 5; i++)
		robo.port[i] = i;
	robo.port[5] = 8;

	/* try access using MII ioctls - get phy address */
	err = do_ioctl(SIOCGMIIPHY);
	if (err < 0) {
		printk(KERN_ERR PFX "error (%i) while accessing MII phy registers with ioctls\n", err);
		goto done;
	}

	/* got phy address check for robo address */
	struct mii_ioctl_data *mii = if_mii(&robo.ifr);
	if ((mii->phy_id != ROBO_PHY_ADDR) &&
	    (mii->phy_id != ROBO_PHY_ADDR_BCM63XX) &&
	    (mii->phy_id != ROBO_PHY_ADDR_TG3)) {
		printk(KERN_ERR PFX "Invalid phy address (%d)\n", mii->phy_id);
		goto done;
	}

	phyid = mdio_read(ROBO_PHY_ADDR, 0x2) | 
		(mdio_read(ROBO_PHY_ADDR, 0x3) << 16);

	if (phyid == 0xffffffff || phyid == 0x55210022) {
		printk(KERN_ERR PFX "No Robo switch in managed mode found, phy_id = 0x%08x\n", phyid);
		goto done;
	}

	/* Get the device ID */
	for (i = 0; i < 10; i++) {
		robo.devid = robo_read16(ROBO_MGMT_PAGE, ROBO_DEVICE_ID);
		if (robo.devid)
			break;
		udelay(10);
	}
	if (!robo.devid)
		robo.devid = ROBO_DEVICE_ID_5325; /* Fake it */
	robo.is_5350 = robo_vlan5350();

	robo_switch_reset();
	err = robo_switch_enable();
	if (err)
		goto done;
	err = 0;

	printk(KERN_INFO PFX "found a 5%s%x!%s at %s\n", robo.devid & 0xff00 ? "" : "3", robo.devid,
		robo.is_5350 ? " It's a 5350." : "", devname);

done:
	if (err) {
		dev_put(robo.dev);
		robo.dev = NULL;
	}
	return err;
}


static int handle_vlan_port_read(void *driver, char *buf, int nr)
{
	__u16 val16;
	int len = 0;
	int j;

	val16 = (nr) /* vlan */ | (0 << 12) /* read */ | (1 << 13) /* enable */;

	if (robo.is_5350) {
		u32 val32;
		robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS_5350, val16);
		/* actual read */
		val32 = robo_read32(ROBO_VLAN_PAGE, ROBO_VLAN_READ);
		if ((val32 & (1 << 20)) /* valid */) {
			for (j = 0; j < 6; j++) {
				if (val32 & (1 << j)) {
					len += sprintf(buf + len, "%d", j);
					if (val32 & (1 << (j + 6))) {
						if (j == 5) buf[len++] = 'u';
					} else {
						buf[len++] = 't';
						if (robo_read16(ROBO_VLAN_PAGE, ROBO_VLAN_PORT0_DEF_TAG + (j << 1)) == nr)
							buf[len++] = '*';
					}
					buf[len++] = '\t';
				}
			}
			len += sprintf(buf + len, "\n");
		}
	} else {
		robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS, val16);
		/* actual read */
		val16 = robo_read16(ROBO_VLAN_PAGE, ROBO_VLAN_READ);
		if ((val16 & (1 << 14)) /* valid */) {
			for (j = 0; j < 6; j++) {
				if (val16 & (1 << j)) {
					len += sprintf(buf + len, "%d", j);
					if (val16 & (1 << (j + 7))) {
						if (j == 5) buf[len++] = 'u';
					} else {
						buf[len++] = 't';
						if (robo_read16(ROBO_VLAN_PAGE, ROBO_VLAN_PORT0_DEF_TAG + (j << 1)) == nr)
							buf[len++] = '*';
					}
					buf[len++] = '\t';
				}
			}
			len += sprintf(buf + len, "\n");
		}
	}

	buf[len] = '\0';

	return len;
}

static int handle_vlan_port_write(void *driver, char *buf, int nr)
{
	switch_driver *d = (switch_driver *) driver;
	switch_vlan_config *c = switch_parse_vlan(d, buf);
	int j;
	__u16 val16;

	if (c == NULL)
		return -EINVAL;

	for (j = 0; j < d->ports; j++) {
		if ((c->untag | c->pvid) & (1 << j))
			/* change default vlan tag */
			robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_PORT0_DEF_TAG + (j << 1), nr);
	}

	/* write config now */

	if (robo.devid != ROBO_DEVICE_ID_5325) {
		__u8 regoff = ((robo.devid == ROBO_DEVICE_ID_5395) ||
			(robo.devid == ROBO_DEVICE_ID_53115)) ? 0x20 : 0;

		robo_write32(ROBO_ARLIO_PAGE, 0x63 + regoff, (c->untag << 9) | c->port);
		robo_write16(ROBO_ARLIO_PAGE, 0x61 + regoff, nr);
		robo_write16(ROBO_ARLIO_PAGE, 0x60 + regoff, 1 << 7);
		kfree(c);
		return 0;
	}

	val16 = (nr) /* vlan */ | (1 << 12) /* write */ | (1 << 13) /* enable */;
	if (robo.is_5350) {
		robo_write32(ROBO_VLAN_PAGE, ROBO_VLAN_WRITE_5350,
			(1 << 20) /* valid */ | (c->untag << 6) | c->port);
		robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS_5350, val16);
	} else {
		robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_WRITE,
			(1 << 14)  /* valid */ | (c->untag << 7) | c->port);
		robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_TABLE_ACCESS, val16);
	}

	kfree(c);
	return 0;
}

#define set_switch(state) \
	robo_write16(ROBO_CTRL_PAGE, ROBO_SWITCH_MODE, (robo_read16(ROBO_CTRL_PAGE, ROBO_SWITCH_MODE) & ~2) | (state ? 2 : 0));

static int handle_enable_read(void *driver, char *buf, int nr)
{
	return sprintf(buf, "%d\n", (((robo_read16(ROBO_CTRL_PAGE, ROBO_SWITCH_MODE) & 2) == 2) ? 1 : 0));
}

static int handle_enable_write(void *driver, char *buf, int nr)
{
	set_switch(buf[0] == '1');

	return 0;
}

static int handle_enable_vlan_read(void *driver, char *buf, int nr)
{
	return sprintf(buf, "%d\n", (((robo_read16(ROBO_VLAN_PAGE, ROBO_VLAN_CTRL0) & (1 << 7)) == (1 << 7)) ? 1 : 0));
}

static int handle_enable_vlan_write(void *driver, char *buf, int nr)
{
	int disable = ((buf[0] != '1') ? 1 : 0);

	robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_CTRL0, disable ? 0 :
		(1 << 7) /* 802.1Q VLAN */ | (3 << 5) /* mac check and hash */);
	robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_CTRL1, disable ? 0 :
		(robo.devid == ROBO_DEVICE_ID_5325 ? (1 << 1) :
		0) | (1 << 2) | (1 << 3)); /* RSV multicast */

	if (robo.devid != ROBO_DEVICE_ID_5325)
		return 0;

	robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_CTRL4, disable ? 0 :
		(1 << 6) /* drop invalid VID frames */);
	robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_CTRL5, disable ? 0 :
		(1 << 3) /* drop miss V table frames */);

	return 0;
}

static int handle_reset(void *driver, char *buf, int nr)
{
	switch_driver *d = (switch_driver *) driver;
	int j;
	__u16 val16;

	/* disable switching */
	set_switch(0);

	/* reset vlans */
	for (j = 0; j <= ((robo.is_5350) ? VLAN_ID_MAX5350 : VLAN_ID_MAX); j++) {
		/* write config now */
		val16 = (j) /* vlan */ | (1 << 12) /* write */ | (1 << 13) /* enable */;
		if (robo.is_5350)
			robo_write32(ROBO_VLAN_PAGE, ROBO_VLAN_WRITE_5350, 0);
		else
			robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_WRITE, 0);
		robo_write16(ROBO_VLAN_PAGE, robo.is_5350 ? ROBO_VLAN_TABLE_ACCESS_5350 :
							    ROBO_VLAN_TABLE_ACCESS,
			     val16);
	}

	/* reset ports to a known good state */
	for (j = 0; j < d->ports; j++) {
		robo_write16(ROBO_CTRL_PAGE, robo.port[j], 0x0000);
		robo_write16(ROBO_VLAN_PAGE, ROBO_VLAN_PORT0_DEF_TAG + (j << 1), 0);
	}

	/* enable switching */
	set_switch(1);

	/* enable vlans */
	handle_enable_vlan_write(driver, "1", 0);

	return 0;
}

static int __init robo_init(void)
{
	int notfound = 1;
	char *device;

	device = strdup("ethX");
	for (device[3] = '0'; (device[3] <= '3') && notfound; device[3]++) {
		if (! switch_device_registered (device))
			notfound = robo_probe(device);
	}
	device[3]--;

	if (notfound) {
		kfree(device);
		return -ENODEV;
	} else {
		static const switch_config cfg[] = {
			{
				.name	= "enable",
				.read	= handle_enable_read,
				.write	= handle_enable_write
			}, {
				.name	= "enable_vlan",
				.read	= handle_enable_vlan_read,
				.write	= handle_enable_vlan_write
			}, {
				.name	= "reset",
				.read	= NULL,
				.write	= handle_reset
			}, { NULL, },
		};
		static const switch_config vlan[] = {
			{
				.name	= "ports",
				.read	= handle_vlan_port_read,
				.write	= handle_vlan_port_write
			}, { NULL, },
		};
		switch_driver driver = {
			.name			= DRIVER_NAME,
			.version		= DRIVER_VERSION,
			.interface		= device,
			.cpuport		= 5,
			.ports			= 6,
			.vlans			= 16,
			.driver_handlers	= cfg,
			.port_handlers		= NULL,
			.vlan_handlers		= vlan,
		};
		if (robo.devid != ROBO_DEVICE_ID_5325) {
			driver.ports = 9;
			driver.cpuport = 8;
		}

		return switch_register_driver(&driver);
	}
}

static void __exit robo_exit(void)
{
	switch_unregister_driver(DRIVER_NAME);
	if (robo.dev)
		dev_put(robo.dev);
	kfree(robo.device);
}


MODULE_AUTHOR("Felix Fietkau <openwrt@nbd.name>");
MODULE_LICENSE("GPL");

module_init(robo_init);
module_exit(robo_exit);
