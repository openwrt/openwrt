/*
 * drivers/net/phy/gw16083.c
 *
 * Driver for GW16083 Ventana Ethernet Expansion Mezzanine
 *
 * Author: Tim Harvey
 *
 * Copyright (c) 2014 Tim Harvey <tharvey@gateworks.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

/*
 * The GW16083 interfaces with a Ventana baseboard via the PCIe bus, an i2c
 * bus (i2c2), and a couple of GPIO's. On the PCIe bus is an i210 GigE with
 * its MAC connected to Port4 of a Marvell MV88E6176 7-port GigE switch via
 * MDIO and RGMII. Ports 0-3 are standard copper RJ45 but Ports 5 and 6
 * connect to Marvell MV88E1111 dual-mode Copper/Fiber PHY's over SGMII and
 * MDIO. The PHY's have both an RG45 for copper and an SFP module.
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/marvell_phy.h>
#include <linux/of_platform.h>

#include <linux/io.h>
#include <asm/irq.h>
#include <linux/uaccess.h>

#include "gw16083.h"

#undef FAIL_ON_CHECKSUM_ERR	/* fail to configure SFP if checksum bad */
#define PORT_POWER_CONTROL	/* ports can be enabled/disabled via sysfs */
#define PORT_MODE_CONTROL	/* ports 5/6 can have SFP/RJ45 mode forced */

MODULE_DESCRIPTION("GW16083 driver");
MODULE_AUTHOR("Tim Harvey");
MODULE_LICENSE("GPL");

struct mv88e1111_port_state {
	int port;
	bool present;
	bool serdes;
	bool sfp_signal;
	bool sfp_present;
	bool sfp_compat;
	bool sfp_enabled;
	char sfp_id[64];
};

struct mv88e1111_priv {
	struct phy_device		*phydev;
	struct i2c_client		*client;
	struct mv88e1111_port_state	port5;
	struct mv88e1111_port_state	port6;
	struct kobject			*sysfs_kobj;
};

enum {
	mode_copper = 0,
	mode_serdes = 1,
};

static struct i2c_client *gw16083_client = NULL;

static int gw16083_read_port_sfp(struct i2c_client *client,
				 struct mv88e1111_port_state *state);

/* read switch port register from port0-6 */
u16 read_switch_port(struct phy_device *pdev, int port, u8 regaddr)
{
	return pdev->bus->read(pdev->bus, MV_BASE + port, regaddr);
}

/* write switch port register to port0-6 */
int write_switch_port(struct phy_device *pdev, int port, u8 regaddr, u16 val)
{
	return pdev->bus->write(pdev->bus, MV_BASE + port, regaddr, val);
}

/*
 * read_switch_port_phy - write a register for a specific port on 88E6176
 * The 88E6176 PHY registers must be accessed thorugh the Global2 address
 * using the SMI_PHY_COMMAND_REG and SMI_PHY_DATA_REG.
 */
int read_switch_port_phy(struct phy_device *pdev, int port, u8 regaddr)
{
	u16 reg;
	int i;

	dev_dbg(&pdev->dev, "read_phy: port%d reg=0x%02x\n", port, regaddr);
	reg = SMIBUSY | SMIMODE22 | SMIOP_READ;
	reg |= port << DEVADDR;
	reg |= regaddr << REGADDR;
	pdev->bus->write(pdev->bus, MV_GLOBAL2, MV_SMI_PHY_COMMAND, reg);
	for (i = 0; i < 10; i++) {
		reg = pdev->bus->read(pdev->bus, MV_GLOBAL2,
				      MV_SMI_PHY_COMMAND);
		if (!(reg & (1<<15)))
			break;
		mdelay(1);
	}
	/* timeout */
	if (i == 10)
		return 0xffff;
	reg = pdev->bus->read(pdev->bus, MV_GLOBAL2, MV_SMI_PHY_DATA);
	return reg;
}

/*
 * write_switch_port_phy - write a register for a specific port on 88E6176
 * The 88E6176 PHY registers must be accessed thorugh the Global2 address
 * using the SMI_PHY_COMMAND_REG and SMI_PHY_DATA_REG.
 */
int write_switch_port_phy(struct phy_device *pdev, int port, u8 addr, u16 reg)
{
	int i;

	dev_dbg(&pdev->dev, "write_phy: port%d reg=0x%02x val=0x%04x\n", port,
		addr, reg);
	pdev->bus->write(pdev->bus, MV_GLOBAL2, MV_SMI_PHY_DATA, reg);
	reg = SMIBUSY | SMIMODE22 | SMIOP_WRITE;
	reg |= port << DEVADDR;
	reg |= addr << REGADDR;
	pdev->bus->write(pdev->bus, MV_GLOBAL2, MV_SMI_PHY_COMMAND, reg);
	for (i = 0; i < 10; i++) {
		reg = pdev->bus->read(pdev->bus, MV_GLOBAL2,
				      MV_SMI_PHY_COMMAND);
		if (!(reg & (1<<15)))
			break;
		mdelay(1);
	}
	/* timeout */
	if (i == 10)
		return -ETIMEDOUT;

	return 0;
}

/* read a scratch register from switch */
inline u8 read_switch_scratch(struct phy_device *pdev, u8 reg)
{
	pdev->bus->write(pdev->bus, MV_GLOBAL2, MV_SCRATCH_MISC, (reg << 8));
	return pdev->bus->read(pdev->bus, MV_GLOBAL2, MV_SCRATCH_MISC) & 0xff;
}

/* write a scratch register to switch */
inline void write_switch_scratch(struct phy_device *pdev, u8 reg, u8 val)
{
	pdev->bus->write(pdev->bus, MV_GLOBAL2, MV_SCRATCH_MISC,
			 (1 << 15) | (reg << 8) | val);
}

/* enable or disable an SFP's TXEN signal */
static int enable_sfp_txen(struct phy_device *pdev, int port, bool enable)
{
	u8 gpio;
	int bit;

	if (port != 5 && port != 6)
		return -EINVAL;

	/* GPIO[2:1] output low to enable TXEN */
	bit = (port == 5) ? 1 : 2;
	gpio = read_switch_scratch(pdev, MV_GPIO_DATA);
	if (enable)
		gpio |= (1 << bit);
	else
		gpio &= (1 << bit);
	write_switch_scratch(pdev, MV_GPIO_DATA, gpio);
	dev_info(&pdev->dev, "Port%d: SFP TX %s\n", port, enable ?
		 "enabled" : "disabled");
	return 0;
}

/* configure mv88e1111 port for copper or serdes
 *  For Copper we set auto link/duplex/speed detection
 *  For SerDes/Fiber we force 1000mbps link up and auto-neg duplex
 */
static int config_mv88e1111_port_sfp(struct phy_device *pdev, int port,
				     bool sfp)
{
	u16 reg;

	if (port != 5 && port != 6)
		return -EINVAL;

	dev_dbg(&pdev->dev, "%s: Port%d %s\n", __func__, port,
		sfp ? "SFP" : "copper");
	if (sfp) {
		enable_sfp_txen(pdev, port, 1);

		/* configure MV88E6176 Physical Control Port Register */
		dev_info(&pdev->dev,
			 "Port%d: SFP: force 1000mbps link up "
			 "(auto-negotiate duplex)\n",
			 port);
		reg = read_switch_port(pdev, port, MV_PORT_PHYS_CONTROL);
		reg &= ~0x3f; /* clear 5-0 */
		reg |= (1 << 4) | (1 << 5); /* force link up */
		reg |= 2; /* force 1000mbps */
		write_switch_port(pdev, port, MV_PORT_PHYS_CONTROL, reg);
		reg = read_switch_port(pdev, port, MV_PORT_PHYS_CONTROL);
	}

	/* copper */
	else {
		enable_sfp_txen(pdev, port, 0);

		/* configure MV88E6176 Physical Control Port Register */
		dev_info(&pdev->dev,
			 "Port%d: Copper: set auto-neg link/duplex/speed\n",
			 port);
		reg = read_switch_port(pdev, port, MV_PORT_PHYS_CONTROL);
		reg &= ~0x3f; /* clear 5-0 */
		reg |= 3; /* speed not forced */
		write_switch_port(pdev, port, MV_PORT_PHYS_CONTROL, reg);
		reg = read_switch_port(pdev, port, MV_PORT_PHYS_CONTROL);
	}
	dev_dbg(&pdev->dev, "%s: Port%d %s PORT_PHYS_CONTROL=0x%04x\n",
		__func__, port, sfp ? "SFP" : "copper",
		read_switch_port(pdev, port, MV_PORT_PHYS_CONTROL));

	return 0;
}

#if defined(PORT_POWER_CONTROL)
static int enable_switch_port(struct phy_device *pdev, int port, bool enable)
{
	struct mv88e1111_priv *priv = dev_get_drvdata(&pdev->dev);
	u16 reg;

	/* power up port */
	dev_info(&priv->client->dev, "Port%d: %s\n", port,
		 enable ? "normal operation" : "power down");
	reg = read_switch_port_phy(pdev, port, MV_PHY_CONTROL);
	if (enable)
		reg &= ~(1 << 11);	/* Normal Operation */
	else
		reg |= (1 << 11);	/* power down */
	write_switch_port_phy(pdev, port, MV_PHY_CONTROL, reg);

	reg = read_switch_port_phy(pdev, port, MV_PHY_CONTROL1);
	if (enable)
		reg &= ~(1 << 2);	/* Normal Operation */
	else
		reg |= (1 << 2);	/* power down */
	write_switch_port_phy(pdev, port, MV_PHY_CONTROL1, reg);

	return 0;
}
#endif

/*
 * Sysfs API
 */

struct mv88e1111_port_state *get_port_state(struct mv88e1111_priv *priv,
					    int port)
{
	if (port == 5)
		return &priv->port5;
	if (port == 6)
		return &priv->port6;
	return NULL;
}

/*
 * get MV88E6176 port number for a specific GW16083 port name
 *  The GW16083 ports as shown on the silkscreen are not mapped according to
 *  the MV88E6176 ports numbers.
 */
static int gw16083_get_port(const char* name)
{
	int i;
	int map[] = { 3, 2, 1, 0, 5, 6 };

	if (strncasecmp(name, "ETHERNET", 8) != 0 || strlen(name) != 9)
		return -1;
	i = name[8] - '0';
	if (i < 1 || i > 6)
		return -1;
	return map[i-1];
}

static ssize_t port_show(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct mv88e1111_priv *priv = dev_get_drvdata(dev);
	int port = -1;
	u16 reg;

	if (sscanf(attr->attr.name, "port%d", &port) != 1)
		return 0;
	if (port < 0 || port > 6)
		return 0;
	reg = read_switch_port_phy(priv->phydev, port, MV_PHY_CONTROL);
	return sprintf(buf, "%s\n", (reg & (1 << 11)) ? "disabled" : "enabled");
}

#if defined(PORT_POWER_CONTROL)
static ssize_t port_store(struct device *dev, struct device_attribute *attr,
			  const char *buf, size_t count)
{
	struct mv88e1111_priv *priv = dev_get_drvdata(dev);
	int port = -1;
	int val;

	port = gw16083_get_port(attr->attr.name);
	if (port < 0)
		return 0;
	if (sscanf(buf, "%d", &val) != 1)
		return 0;
	enable_switch_port(priv->phydev, port, val ? 1 : 0);
	return count;
}

static DEVICE_ATTR(ethernet1, S_IWUSR | S_IRUGO, port_show, port_store);
static DEVICE_ATTR(ethernet2, S_IWUSR | S_IRUGO, port_show, port_store);
static DEVICE_ATTR(ethernet3, S_IWUSR | S_IRUGO, port_show, port_store);
static DEVICE_ATTR(ethernet4, S_IWUSR | S_IRUGO, port_show, port_store);
static DEVICE_ATTR(ethernet5, S_IWUSR | S_IRUGO, port_show, port_store);
static DEVICE_ATTR(ethernet6, S_IWUSR | S_IRUGO, port_show, port_store);
#else
static DEVICE_ATTR(ethernet1, S_IRUGO, port_show, NULL);
static DEVICE_ATTR(ethernet2, S_IRUGO, port_show, NULL);
static DEVICE_ATTR(ethernet3, S_IRUGO, port_show, NULL);
static DEVICE_ATTR(ethernet4, S_IRUGO, port_show, NULL);
static DEVICE_ATTR(ethernet5, S_IRUGO, port_show, NULL);
static DEVICE_ATTR(ethernet6, S_IRUGO, port_show, NULL);
#endif

static ssize_t portsfp_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	struct mv88e1111_priv *priv = dev_get_drvdata(dev);
	struct mv88e1111_port_state *state;

	state = get_port_state(priv, gw16083_get_port(attr->attr.name));
	if (!state)
		return 0;

	if (!state->sfp_present)
		return 0;

	return sprintf(buf, "%s\n", state->sfp_id);
}

static ssize_t portmode_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	struct mv88e1111_priv *priv = dev_get_drvdata(dev);
	struct mv88e1111_port_state *state;

	state = get_port_state(priv, gw16083_get_port(attr->attr.name));
	if (!state)
		return 0;

	return sprintf(buf, "%s\n", state->serdes ? "SFP" : "RJ45");
}
static DEVICE_ATTR(ethernet5_sfp, S_IRUGO, portsfp_show, NULL);
static DEVICE_ATTR(ethernet6_sfp, S_IRUGO, portsfp_show, NULL);

#ifdef PORT_MODE_CONTROL
static ssize_t portmode_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	struct mv88e1111_priv *priv = dev_get_drvdata(dev);
	struct mv88e1111_port_state *state;
	u16 reg;
	int port;

	port = gw16083_get_port(attr->attr.name);
	state = get_port_state(priv, port);
	if (!state)
		return 0;

	reg = read_switch_port_phy(priv->phydev, port, MII_M1111_PHY_EXT_SR);
	if (strcasecmp(buf, "auto") == 0) {
		reg &= ~(1<<15); /* enable auto-selection */
		dev_info(&priv->client->dev, "Port%d: enable auto-selection\n",
			 port);
	} else if (strcasecmp(buf, "RJ45") == 0) {
		reg |= (1<<15); /* disable auto-selection */
		reg |= 0xb; /* RGMII to Copper */
		config_mv88e1111_port_sfp(priv->phydev, port, 0);
		dev_info(&priv->client->dev, "Port%d: select RJ45\n", port);
	} else if (strcasecmp(buf, "SFP") == 0) {
		reg |= (1<<15); /* disable auto-selection */
		reg |= 0x3; /* RGMII to Fiber */
		config_mv88e1111_port_sfp(priv->phydev, port, 1);
		dev_info(&priv->client->dev, "Port%d: select SFP\n", port);
	}
	write_switch_port_phy(priv->phydev, port, MII_M1111_PHY_EXT_SR, reg);

	return count;
}

static DEVICE_ATTR(ethernet5_mode, S_IWUSR | S_IRUGO, portmode_show,
		   portmode_store);
static DEVICE_ATTR(ethernet6_mode, S_IWUSR | S_IRUGO, portmode_show,
		   portmode_store);
#else
static DEVICE_ATTR(ethernet5_mode, S_IRUGO, portmode_show, NULL);
static DEVICE_ATTR(ethernet6_mode, S_IRUGO, portmode_show, NULL);
#endif


/*
 * PHY driver
 */

static int
mv88e6176_config_init(struct phy_device *pdev)
{
	dev_dbg(&pdev->dev, "%s\n", __func__);
	pdev->state = PHY_RUNNING;

	return 0;
}

/* check MV88E1111 PHY status and MV88E6176 GPIO */
static int
mv88e6176_read_status(struct phy_device *pdev)
{
	struct mv88e1111_priv *priv = dev_get_drvdata(&pdev->dev);
	struct mv88e1111_port_state *state;
	bool serdes, sfp_present, sfp_signal;
	int port;
	int ret = 0;
	u16 gpio;

	dev_dbg(&pdev->dev, "%s", __func__);
	gpio = read_switch_scratch(pdev, MV_GPIO_DATA);
	for (port = 5; port < 7; port++) {
		serdes = (read_switch_port_phy(pdev, port, MII_M1111_PHY_EXT_SR)
			 & (1<<13)) ? 1 : 0;
		dev_dbg(&pdev->dev, "%s: Port%d GPIO:0x%02x SerDes:%d\n",
			__func__, port, gpio, serdes);
		switch(port) {
		case 5:
			state = &priv->port5;
			sfp_present = !((gpio >> 5) & 1);
			sfp_signal = !((gpio >> 6) & 1);
			break;
		case 6:
			state = &priv->port6;
			sfp_present = !((gpio >> 3) & 1);
			sfp_signal = !((gpio >> 4) & 1);
			break;
		}

		/*
		 * on sfp_detect read/verify SFP MSA and set sfp_compat
		 * on sfp_signal issue link down?
		 * on serdes auto-select
		 */
		if (state->sfp_present != sfp_present) {
			state->sfp_present = sfp_present;
			dev_info(&pdev->dev, "Port%d: SFP %s\n",
				 port, sfp_present ? "inserted" : "removed");
			if (state->sfp_present) {
				if (gw16083_read_port_sfp(priv->client, state))
					state->sfp_compat = false;
				else
					state->sfp_compat = true;
			} else {
				state->sfp_compat = false;
				state->sfp_enabled = false;
			}
		}
		if (state->sfp_signal != sfp_signal) {
			state->sfp_signal = sfp_signal;
			dev_info(&pdev->dev, "Port%d: SFP signal %s\n",
				 port, sfp_signal ? "detected" : "lost");
		}
		if (state->serdes != serdes) {
			state->serdes = serdes;
			dev_info(&pdev->dev, "Port%d: %s auto-selected\n",
				 port, serdes ? "SERDES" : "copper");

			/*
			 * if auto-selection has switched to copper
			 * disable serdes
			 */
			if (!serdes) {
				config_mv88e1111_port_sfp(pdev, port, 0);
				state->sfp_enabled = false;
			}
		}

		/*
		 * if serdes and compatible SFP module and not yet enabled
		 * then enable for serdes
		 */
		if (serdes && state->sfp_compat && state->sfp_signal &&
		    !state->sfp_enabled)
		{
			if (!config_mv88e1111_port_sfp(pdev, port, 1))
				state->sfp_enabled = true;
		}
	}

	return ret;
}

static int
mv88e6176_config_aneg(struct phy_device *pdev)
{
	dev_dbg(&pdev->dev, "%s", __func__);
	return 0;
}

static void
mv88e6176_remove(struct phy_device *pdev)
{
	dev_dbg(&pdev->dev, "%s", __func__);

	device_remove_file(&pdev->dev, &dev_attr_ethernet1);
	device_remove_file(&pdev->dev, &dev_attr_ethernet2);
	device_remove_file(&pdev->dev, &dev_attr_ethernet3);
	device_remove_file(&pdev->dev, &dev_attr_ethernet4);
	device_remove_file(&pdev->dev, &dev_attr_ethernet5);
	device_remove_file(&pdev->dev, &dev_attr_ethernet6);
	device_remove_file(&pdev->dev, &dev_attr_ethernet5_sfp);
	device_remove_file(&pdev->dev, &dev_attr_ethernet6_sfp);
	device_remove_file(&pdev->dev, &dev_attr_ethernet5_mode);
	device_remove_file(&pdev->dev, &dev_attr_ethernet6_mode);
	sysfs_remove_link(kernel_kobj, "gw16083");
}

static int
mv88e6176_probe(struct phy_device *pdev)
{
	int port;
	int ret = 0;
	u32 id, reg;
	struct mv88e1111_priv *priv;

	dev_dbg(&pdev->dev, "%s: addr=0x%02x bus=%s:%s gw16083_client=%p\n",
		__func__, pdev->addr, pdev->bus->name, pdev->bus->id,
		gw16083_client);

	/* In single-chip addressing mode the MV88E6176 shows up on 0x10-0x16 */
	if (pdev->addr != MV_BASE)
		return 0;

	/* i2c driver needs to be loaded first */
	if (!gw16083_client)
		return 0;

	/* gw16083 has MV88E1676 hanging off of i210 mdio bus */
	if (strcmp(pdev->bus->name, "igb_enet_mii_bus") != 0)
		return 0;

	//dev_info(&pdev->dev, "Detected");
	dev_info(&gw16083_client->dev, "%s: MV88E6176 7-port switch detected",
		 pdev->bus->id);

	/*
	 * port5/6 config: MV88E1111 PHY
	 * Register 20: PHY Control Register
	 *   R20_7: add delay to RX_CLK for RXD
	 *   R20_1: add delay to TX_CLK for TXD
	 * Register 24: LED Control Register
	 *   0x4111:
	 *     Pulse stretch 170 to 340 ms
	 * Register 0: Control Register
	 *   R0_15: phy reset
	 */
	for (port = 5; port < 7; port++) {
#ifndef RGMII_DELAY_ON_PHY
		write_switch_port(pdev, port, MV_PORT_PHYS_CONTROL, 0xC003);
#endif

		id = read_switch_port_phy(pdev, port,
					  MII_M1111_PHY_IDENT0) << 16;
		id |= read_switch_port_phy(pdev, port, MII_M1111_PHY_IDENT1);
		if ((id & MII_M1111_PHY_ID_MASK) != MII_M1111_PHY_ID) {
			dev_err(&gw16083_client->dev,
				"Port%d: No MV88E1111 PHY detected", port);
			return 0;
			//continue;
		}

#ifdef RGMII_DELAY_ON_PHY
		/* phy rx/tx delay */
		reg = read_switch_port_phy(pdev, port, MII_M1111_PHY_EXT_CR);
		reg |= (1<<1) | (1<<7);
		write_switch_port_phy(pdev, port, MII_M1111_PHY_EXT_CR, reg);
#endif
		/* led config */
		write_switch_port_phy(pdev, port, MII_M1111_PHY_LED_CONTROL,
				      MII_M1111_PHY_LED_PULSE_STR);
		/* reset phy */
		reg = read_switch_port_phy(pdev, port, MII_M1111_PHY_CONTROL);
		reg |= MII_M1111_PHY_CONTROL_RESET;
		write_switch_port_phy(pdev, port, MII_M1111_PHY_CONTROL, reg);
		dev_info(&gw16083_client->dev,
			 "Port%d MV88E111 PHY configured\n", port);
	}

	/*
	 * GPIO Configuration:
	 *  GPIO1: FIB5_TXEN# (output)
	 *  GPIO2: FIB6_TXEN# (output)
	 *  GPIO3: FIB6_PRES# (input)
	 *  GPIO4: FIB6_LOS   (input)
	 *  GPIO5: FIB5_PRES# (input)
	 *  GPIO6: FIB5_LOS   (input)
	 */
	write_switch_scratch(pdev, MV_GPIO_DATA, 0x06); /* GPIO[2:1] out hi */
	write_switch_scratch(pdev, MV_GPIO_DIR,  0x78); /* GPIO[6:3] inp */

	pdev->irq = PHY_POLL;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
	memset(priv, 0, sizeof(*priv));
	priv->phydev = pdev;
	priv->client = gw16083_client;
	priv->port5.port = 5;
	priv->port6.port = 6;
	dev_set_drvdata(&pdev->dev, priv);

	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet1);
	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet2);
	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet3);
	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet4);
	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet5);
	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet6);
	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet5_sfp);
	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet6_sfp);
	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet5_mode);
	ret |= device_create_file(&pdev->dev, &dev_attr_ethernet6_mode);

	if (unlikely(ret))
		dev_err(&pdev->dev, "Failed creating attrs\n");

	/* Add a nice symlink to the real device */
	ret = sysfs_create_link(kernel_kobj, &pdev->dev.kobj, "gw16083");

	dev_dbg(&pdev->dev, "initial state: GPIO=0x%02x "
		"Port5_serdes=%d Port6_serdes=%d\n",
		read_switch_scratch(pdev, MV_GPIO_DATA),
		(read_switch_port_phy(pdev, 5, MII_M1111_PHY_EXT_SR)
		 & (1<<13) ? 1:0),
		(read_switch_port_phy(pdev, 6, MII_M1111_PHY_EXT_SR)
		 & (1<<13) ? 1:0));

	return ret;
}

static struct phy_driver mv88e6176_phy_driver = {
	.name		= "MV88E6176",
	.phy_id		= MV_IDENT_VALUE,
	.phy_id_mask	= MV_IDENT_MASK,
	.features	= PHY_BASIC_FEATURES,
	.probe		= &mv88e6176_probe,
	.remove		= &mv88e6176_remove,
	.config_init	= &mv88e6176_config_init,
	.config_aneg	= &mv88e6176_config_aneg,
	.read_status	= &mv88e6176_read_status,
	.driver		= { .owner = THIS_MODULE },
};

/*
 * I2C driver
 */

/* See SFF-8472 */
struct sfp_msa {
	/* Basic ID fields */
	u8	identifier;
	u8	ext_identifier;
	u8	connector;
	u8	transceiver[8];
	u8	encoding;
	u8	br_nominal;
	u8	rate_identifier;
	u8	length_smf_km;
	u8	length_smf;
	u8	length_om2;
	u8	length_om1;
	u8	length_om4;
	u8	length_om3;
	u8	vendor_name[16];
	u8	transceiver2;
	u8	vendor_oui[3];
	u8	vendor_pn[16];
	u8	vendor_rev[4];
	u8	wavelength[2];
	u8	resv1;
	u8	cc_base;

	/* extended id fields */
	u8	options[2];
	u8	br_max;
	u8	br_min;
	u8	vendor_sn[16];
	u8	date_code[8];
	u8	diags_type;
	u8	enhanced_options;
	u8	sff8472_compliance;
	u8	cc_ext;

	/* Vendor specific ID fields */
	u8	vendor_data[32];
	u8	sff8079[128];
};

enum identifier {
	UNKNOWN,
	GBIC,
	SFF,
	SFP,
	XBI,
	XENPACK,
	XFP,
	XFF,
	XFP_E,
	XPAK,
	X2,
	DWDM_SFP,
	QSFP,
	MAX_ID,
};

const char* id_names[] = {
	"UNKONWN",
	"GBIC",
	"SFF",
	"SFP",
	NULL,
};

/* Flags for SFP modules compatible with ETH up to 1Gb */
struct sfp_flags {
	u8 e1000_base_sx:1;
	u8 e1000_base_lx:1;
	u8 e1000_base_cx:1;
	u8 e1000_base_t:1;
	u8 e100_base_lx:1;
	u8 e100_base_fx:1;
	u8 e10_base_bx10:1;
	u8 e10_base_px:1;
};

#define STRING_APPEND(str, src)				\
	strncat(str, src, sizeof(src));			\
	for (i = 1; i < sizeof(str); i++)		\
		if (str[i-1] == ' ' && str[i] == ' ')	\
			str[i] = 0;

static int gw16083_read_port_sfp(struct i2c_client *client,
				 struct mv88e1111_port_state *state)
{
	int ret = 0;
	u8 data[256];
	struct sfp_flags *eth_flags;
	u8 crc;
	int i;
	u8 *str;
	struct sfp_msa *sfp_msa = (struct sfp_msa *)data;
	int port = state->port;
	union i2c_smbus_data d;

	dev_dbg(&client->dev, "%s Port%d\n", __func__, port);
	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_READ_I2C_BLOCK))
		return -ENODEV;
	d.byte = (port == 5) ? 1 : 2;
	if (i2c_smbus_xfer(client->adapter, GW16083_I2C_ADDR_PCA9543,
			   client->flags, I2C_SMBUS_WRITE, 0,
			   I2C_SMBUS_BYTE_DATA, &d) < 0)
	{
		dev_err(&client->dev,
			"Port%d: failed writing PCA9543 register\n", port);
		return ret;
	}

	/* read all 256 bytes of SFP EEPROM */
	for (i = 0; i < sizeof(data); i += I2C_SMBUS_BLOCK_MAX) {
		d.block[0] = I2C_SMBUS_BLOCK_MAX;
		if (i2c_smbus_xfer(client->adapter, GW16083_I2C_ADDR_SFP1,
				   client->flags, I2C_SMBUS_READ, i,
				   I2C_SMBUS_I2C_BLOCK_DATA, &d) < 0)
		{
			dev_err(&client->dev,
				"Port%d: failed reading SFP data\n", port);
			return ret;
		}
		memcpy(data + i, d.block + 1, I2C_SMBUS_BLOCK_MAX);
	}

	/* Validate checksums */
	for (crc = 0, i = 0; i < 63; i++)
		crc += data[i];
	if (crc != sfp_msa->cc_base) {
		dev_err(&client->dev, "Port%d: "
			"Checksum failure for Base ID fields: 0x%02x\n", port,
			crc);
#ifdef FAIL_ON_CHECKSUM_ERR
		return -EINVAL;
#endif
	}
	for (crc = 0, i = 64; i < 95; i++)
		crc += data[i];
	if (crc != sfp_msa->cc_ext) {
		dev_err(&client->dev, "Port%d: "
			"Checksum failure for Extended ID fields: 0x%02x\n",
			port, crc);
#ifdef FAIL_ON_CHECKSUM_ERR
		return -EINVAL;
#endif
	}
	state->sfp_id[0] = 0;
	for (i = 0; id_names[i]; i++) {
		if (sfp_msa->identifier == i) {
			sprintf(state->sfp_id, "%s: ", id_names[i]);
			break;
		}
	}
	STRING_APPEND(state->sfp_id, sfp_msa->vendor_oui);
	STRING_APPEND(state->sfp_id, sfp_msa->vendor_name);
	STRING_APPEND(state->sfp_id, sfp_msa->vendor_pn);
	STRING_APPEND(state->sfp_id, sfp_msa->vendor_rev);
	STRING_APPEND(state->sfp_id, sfp_msa->vendor_sn);
	dev_info(&client->dev, "Port%d: %s\n", port, state->sfp_id);

	if ((sfp_msa->identifier != GBIC) &&
	    (sfp_msa->identifier != SFF) &&
	    (sfp_msa->identifier != SFP))
	{
		dev_err(&client->dev, "Port%d: Unknown module identifier: %d\n",
			port, sfp_msa->identifier);
		return -EINVAL;
	}

	str = "";
	eth_flags = (struct sfp_flags *)(sfp_msa->transceiver + 3);
	if (eth_flags->e1000_base_sx) {
		str = "1000Base-SX (Fiber)";
	} else if (eth_flags->e1000_base_lx) {
		str = "1000Base-LX (Fiber)";
	} else if (eth_flags->e1000_base_t) {
		str = "1000Base-T (Copper)";
	} else if (eth_flags->e100_base_fx) {
		str = "100Base-FX (Fiber) - not supported";
		ret = -EINVAL;
	} else {
		str = "Unknown/Unsupported media type";
		ret = -EINVAL;
	}
	if (ret)
		dev_err(&client->dev, "Port%d: %s (0x%02x)\n", port, str,
			sfp_msa->transceiver[3]);
	else
		dev_info(&client->dev, "Port%d: %s (0x%02x)\n", port, str,
			sfp_msa->transceiver[3]);

	return ret;
}

static int gw16083_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	int ret;

	dev_info(&client->dev, "GW16083 Ethernet Expansion Mezzanine\n");
	if (gw16083_client) {
		dev_err(&client->dev, "client already registered\n");
		return -EINVAL;
	}
	gw16083_client = client;

	ret = phy_driver_register(&mv88e6176_phy_driver);
	if (ret)
		dev_err(&client->dev,
			"failed to register mv88e6176 phy driver: %d\n", ret);
	return ret;
}

static int gw16083_remove(struct i2c_client *client)
{
	dev_dbg(&client->dev, "%s\n", __func__);

	phy_driver_unregister(&mv88e6176_phy_driver);
	gw16083_client = NULL;
	return 0;
}

static const struct of_device_id gw16083_dt_ids[] = {
	{ .compatible = "gateworks,gw16083", },
	{ }
};

MODULE_DEVICE_TABLE(of, gw16083_dt_ids);

static const struct i2c_device_id gw16083_id[] = {
	{ "gw16083", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, gw16083_id);

static struct i2c_driver gw16083_driver = {
	.driver = {
		.name	= "gw16083",
		.of_match_table = gw16083_dt_ids,
	},
	.probe		= gw16083_probe,
	.remove		= gw16083_remove,
	.id_table	= gw16083_id,
};

static int __init mv88e6176_init(void)
{
	return i2c_add_driver(&gw16083_driver);
}

static void __exit mv88e6176_exit(void)
{
	i2c_del_driver(&gw16083_driver);
}

module_init(mv88e6176_init);
module_exit(mv88e6176_exit);
