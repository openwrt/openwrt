/*
 * ADM6996 switch driver
 *
 * swconfig interface based on ar8216.c
 *
 * Copyright (c) 2008 Felix Fietkau <nbd@openwrt.org>
 * VLAN support Copyright (c) 2010, 2011 Peter Lebbing <peter@digitalbrains.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of the GNU General Public License v2 as published by the
 * Free Software Foundation
 */

/*#define DEBUG 1*/
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
#include <linux/switch.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include "adm6996.h"

MODULE_DESCRIPTION("Infineon ADM6996 Switch");
MODULE_AUTHOR("Felix Fietkau, Peter Lebbing <peter@digitalbrains.com>");
MODULE_LICENSE("GPL");

enum adm6996_model {
	ADM6996FC,
	ADM6996M
};

static const char * const adm6996_model_name[] =
{
	"ADM6996FC",
	"ADM6996M"
};

struct adm6996_priv {
	struct switch_dev dev;
	struct phy_device *phydev;

	enum adm6996_model model;

	bool enable_vlan;
	bool vlan_enabled;	/* Current hardware state */

#ifdef DEBUG
	u16 addr;		/* Debugging: register address to operate on */
#endif

	u16 pvid[ADM_NUM_PORTS];	/* Primary VLAN ID */

	u16 vlan_id[ADM_NUM_VLANS];
	u8 vlan_table[ADM_NUM_VLANS];	/* bitmap, 1 = port is member */
	u8 vlan_tagged[ADM_NUM_VLANS];	/* bitmap, 1 = tagged member */

	struct mutex reg_mutex;

	/* use abstraction for regops, we want to add gpio support in the future */
	u16 (*read)(struct phy_device *phydev, enum admreg reg);
	void (*write)(struct phy_device *phydev, enum admreg reg, u16 val);
};

#define to_adm(_dev) container_of(_dev, struct adm6996_priv, dev)
#define phy_to_adm(_phy) ((struct adm6996_priv *) (_phy)->priv)

static inline u16
r16(struct phy_device *pdev, enum admreg reg)
{
	return phy_to_adm(pdev)->read(pdev, reg);
}

static inline void
w16(struct phy_device *pdev, enum admreg reg, u16 val)
{
	phy_to_adm(pdev)->write(pdev, reg, val);
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

static int
adm6996_set_enable_vlan(struct switch_dev *dev, const struct switch_attr *attr,
			struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);

	if (val->value.i > 1)
		return -EINVAL;

	priv->enable_vlan = val->value.i;

	return 0;
};

static int
adm6996_get_enable_vlan(struct switch_dev *dev, const struct switch_attr *attr,
			struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);

	val->value.i = priv->enable_vlan;

	return 0;
};

#ifdef DEBUG

static int
adm6996_set_addr(struct switch_dev *dev, const struct switch_attr *attr,
		 struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);

	if (val->value.i > 1023)
		return -EINVAL;

	priv->addr = val->value.i;

	return 0;
};

static int
adm6996_get_addr(struct switch_dev *dev, const struct switch_attr *attr,
		 struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);

	val->value.i = priv->addr;

	return 0;
};

static int
adm6996_set_data(struct switch_dev *dev, const struct switch_attr *attr,
		 struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);

	if (val->value.i > 65535)
		return -EINVAL;

	w16(priv->phydev, priv->addr, val->value.i);

	return 0;
};

static int
adm6996_get_data(struct switch_dev *dev, const struct switch_attr *attr,
		 struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);

	val->value.i = r16(priv->phydev, priv->addr);

	return 0;
};

#endif /* def DEBUG */

static int
adm6996_set_pvid(struct switch_dev *dev, int port, int vlan)
{
	struct adm6996_priv *priv = to_adm(dev);

	dev_dbg (&priv->phydev->dev, "set_pvid port %d vlan %d\n", port
			, vlan);

	if (vlan > ADM_VLAN_MAX_ID)
		return -EINVAL;

	priv->pvid[port] = vlan;

	return 0;
}

static int
adm6996_get_pvid(struct switch_dev *dev, int port, int *vlan)
{
	struct adm6996_priv *priv = to_adm(dev);

	dev_dbg (&priv->phydev->dev, "get_pvid port %d\n", port);
	*vlan = priv->pvid[port];

	return 0;
}

static int
adm6996_set_vid(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);

	dev_dbg (&priv->phydev->dev, "set_vid port %d vid %d\n", val->port_vlan,
			val->value.i);

	if (val->value.i > ADM_VLAN_MAX_ID)
		return -EINVAL;

	priv->vlan_id[val->port_vlan] = val->value.i;

	return 0;
};

static int
adm6996_get_vid(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);

	dev_dbg (&priv->phydev->dev, "get_vid port %d\n", val->port_vlan);

	val->value.i = priv->vlan_id[val->port_vlan];

	return 0;
};

static int
adm6996_get_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);
	u8 ports = priv->vlan_table[val->port_vlan];
	u8 tagged = priv->vlan_tagged[val->port_vlan];
	int i;

	dev_dbg (&priv->phydev->dev, "get_ports port_vlan %d\n",
			val->port_vlan);

	val->len = 0;

	for (i = 0; i < ADM_NUM_PORTS; i++) {
		struct switch_port *p;

		if (!(ports & (1 << i)))
			continue;

		p = &val->value.ports[val->len++];
		p->id = i;
		if (tagged & (1 << i))
			p->flags = (1 << SWITCH_PORT_FLAG_TAGGED);
		else
			p->flags = 0;
	}

	return 0;
};

static int
adm6996_set_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct adm6996_priv *priv = to_adm(dev);
	u8 *ports = &priv->vlan_table[val->port_vlan];
	u8 *tagged = &priv->vlan_tagged[val->port_vlan];
	int i;

	dev_dbg (&priv->phydev->dev, "set_ports port_vlan %d ports",
			val->port_vlan);

	*ports = 0;
	*tagged = 0;

	for (i = 0; i < val->len; i++) {
		struct switch_port *p = &val->value.ports[i];

#ifdef DEBUG
		pr_cont(" %d%s", p->id,
		       ((p->flags & (1 << SWITCH_PORT_FLAG_TAGGED)) ? "T" :
			""));
#endif

		if (p->flags & (1 << SWITCH_PORT_FLAG_TAGGED))
			*tagged |= (1 << p->id);

		*ports |= (1 << p->id);
	}

#ifdef DEBUG
	pr_cont("\n");
#endif

	return 0;
};

/*
 * Precondition: reg_mutex must be held
 */
static void
adm6996_enable_vlan(struct adm6996_priv *priv)
{
	u16 reg;

	reg = r16(priv->phydev, ADM_OTBE_P2_PVID);
	reg &= ~(ADM_OTBE_MASK);
	w16(priv->phydev, ADM_OTBE_P2_PVID, reg);
	reg = r16(priv->phydev, ADM_IFNTE);
	reg &= ~(ADM_IFNTE_MASK);
	w16(priv->phydev, ADM_IFNTE, reg);
	reg = r16(priv->phydev, ADM_VID_CHECK);
	reg |= ADM_VID_CHECK_MASK;
	w16(priv->phydev, ADM_VID_CHECK, reg);
	reg = r16(priv->phydev, ADM_SYSC0);
	reg |= ADM_NTTE;
	reg &= ~(ADM_RVID1);
	w16(priv->phydev, ADM_SYSC0, reg);
	reg = r16(priv->phydev, ADM_SYSC3);
	reg |= ADM_TBV;
	w16(priv->phydev, ADM_SYSC3, reg);

};

/*
 * Disable VLANs
 *
 * Sets VLAN mapping for port-based VLAN with all ports connected to
 * eachother (this is also the power-on default).
 *
 * Precondition: reg_mutex must be held
 */
static void
adm6996_disable_vlan(struct adm6996_priv *priv)
{
	u16 reg;
	int i;

	for (i = 0; i < ADM_NUM_PORTS; i++) {
		reg = ADM_VLAN_FILT_MEMBER_MASK;
		w16(priv->phydev, ADM_VLAN_FILT_L(i), reg);
		reg = ADM_VLAN_FILT_VALID | ADM_VLAN_FILT_VID(1);
		w16(priv->phydev, ADM_VLAN_FILT_H(i), reg);
	}

	reg = r16(priv->phydev, ADM_OTBE_P2_PVID);
	reg |= ADM_OTBE_MASK;
	w16(priv->phydev, ADM_OTBE_P2_PVID, reg);
	reg = r16(priv->phydev, ADM_IFNTE);
	reg |= ADM_IFNTE_MASK;
	w16(priv->phydev, ADM_IFNTE, reg);
	reg = r16(priv->phydev, ADM_VID_CHECK);
	reg &= ~(ADM_VID_CHECK_MASK);
	w16(priv->phydev, ADM_VID_CHECK, reg);
	reg = r16(priv->phydev, ADM_SYSC0);
	reg &= ~(ADM_NTTE);
	reg |= ADM_RVID1;
	w16(priv->phydev, ADM_SYSC0, reg);
	reg = r16(priv->phydev, ADM_SYSC3);
	reg &= ~(ADM_TBV);
	w16(priv->phydev, ADM_SYSC3, reg);
}

/*
 * Precondition: reg_mutex must be held
 */
static void
adm6996_apply_port_pvids(struct adm6996_priv *priv)
{
	u16 reg;
	int i;

	for (i = 0; i < ADM_NUM_PORTS; i++) {
		reg = r16(priv->phydev, adm_portcfg[i]);
		reg &= ~(ADM_PORTCFG_PVID_MASK);
		reg |= ADM_PORTCFG_PVID(priv->pvid[i]);
		w16(priv->phydev, adm_portcfg[i], reg);
	}

	w16(priv->phydev, ADM_P0_PVID, ADM_P0_PVID_VAL(priv->pvid[0]));
	w16(priv->phydev, ADM_P1_PVID, ADM_P1_PVID_VAL(priv->pvid[1]));
	reg = r16(priv->phydev, ADM_OTBE_P2_PVID);
	reg &= ~(ADM_P2_PVID_MASK);
	reg |= ADM_P2_PVID_VAL(priv->pvid[2]);
	w16(priv->phydev, ADM_OTBE_P2_PVID, reg);
	reg = ADM_P3_PVID_VAL(priv->pvid[3]);
	reg |= ADM_P4_PVID_VAL(priv->pvid[4]);
	w16(priv->phydev, ADM_P3_P4_PVID, reg);
	w16(priv->phydev, ADM_P5_PVID, ADM_P5_PVID_VAL(priv->pvid[5]));
}

/*
 * Precondition: reg_mutex must be held
 */
static void
adm6996_apply_vlan_filters(struct adm6996_priv *priv)
{
	u8 ports, tagged;
	u16 vid, reg;
	int i;

	for (i = 0; i < ADM_NUM_VLANS; i++) {
		vid = priv->vlan_id[i];
		ports = priv->vlan_table[i];
		tagged = priv->vlan_tagged[i];

		if (ports == 0) {
			/* Disable VLAN entry */
			w16(priv->phydev, ADM_VLAN_FILT_H(i), 0);
			w16(priv->phydev, ADM_VLAN_FILT_L(i), 0);
			continue;
		}

		reg = ADM_VLAN_FILT_MEMBER(ports);
		reg |= ADM_VLAN_FILT_TAGGED(tagged);
		w16(priv->phydev, ADM_VLAN_FILT_L(i), reg);
		reg = ADM_VLAN_FILT_VALID | ADM_VLAN_FILT_VID(vid);
		w16(priv->phydev, ADM_VLAN_FILT_H(i), reg);
	}
}

static int
adm6996_hw_apply(struct switch_dev *dev)
{
	struct adm6996_priv *priv = to_adm(dev);

	dev_dbg(&priv->phydev->dev, "hw_apply\n");

	mutex_lock(&priv->reg_mutex);

	if (!priv->enable_vlan) {
		if (priv->vlan_enabled) {
			adm6996_disable_vlan(priv);
			priv->vlan_enabled = 0;
		}
		goto out;
	}

	if (!priv->vlan_enabled) {
		adm6996_enable_vlan(priv);
		priv->vlan_enabled = 1;
	}

	adm6996_apply_port_pvids(priv);
	adm6996_apply_vlan_filters(priv);

out:
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

/*
 * Reset the switch
 *
 * The ADM6996 can't do a software-initiated reset, so we just initialise the
 * registers we support in this driver.
 *
 * Precondition: reg_mutex must be held
 */
static void
adm6996_perform_reset (struct adm6996_priv *priv)
{
	int i;

	/* initialize port and vlan settings */
	for (i = 0; i < ADM_NUM_PORTS - 1; i++) {
		w16(priv->phydev, adm_portcfg[i], ADM_PORTCFG_INIT |
			ADM_PORTCFG_PVID(0));
	}
	w16(priv->phydev, adm_portcfg[5], ADM_PORTCFG_CPU);

	/* reset all PHY ports */
	for (i = 0; i < ADM_PHY_PORTS; i++) {
		w16(priv->phydev, ADM_PHY_PORT(i), ADM_PHYCFG_INIT);
	}

	priv->enable_vlan = 0;
	priv->vlan_enabled = 0;

	for (i = 0; i < ADM_NUM_PORTS; i++) {
		priv->pvid[i] = 0;
	}

	for (i = 0; i < ADM_NUM_VLANS; i++) {
		priv->vlan_id[i] = i;
		priv->vlan_table[i] = 0;
		priv->vlan_tagged[i] = 0;
	}

	if (priv->model == ADM6996M) {
		/* Clear VLAN priority map so prio's are unused */
		w16 (priv->phydev, ADM_VLAN_PRIOMAP, 0);

		adm6996_disable_vlan(priv);
		adm6996_apply_port_pvids(priv);
	}
}

static int
adm6996_reset_switch(struct switch_dev *dev)
{
	struct adm6996_priv *priv = to_adm(dev);

	dev_dbg (&priv->phydev->dev, "reset\n");
	mutex_lock(&priv->reg_mutex);
	adm6996_perform_reset (priv);
	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static struct switch_attr adm6996_globals[] = {
	{
	 .type = SWITCH_TYPE_INT,
	 .name = "enable_vlan",
	 .description = "Enable VLANs",
	 .set = adm6996_set_enable_vlan,
	 .get = adm6996_get_enable_vlan,
	},
#ifdef DEBUG
	{
	 .type = SWITCH_TYPE_INT,
	 .name = "addr",
	 .description =
	 "Direct register access: set register address (0 - 1023)",
	 .set = adm6996_set_addr,
	 .get = adm6996_get_addr,
	 },
	{
	 .type = SWITCH_TYPE_INT,
	 .name = "data",
	 .description =
	 "Direct register access: read/write to register (0 - 65535)",
	 .set = adm6996_set_data,
	 .get = adm6996_get_data,
	 },
#endif /* def DEBUG */
};

static struct switch_attr adm6996_port[] = {
};

static struct switch_attr adm6996_vlan[] = {
	{
	 .type = SWITCH_TYPE_INT,
	 .name = "vid",
	 .description = "VLAN ID",
	 .set = adm6996_set_vid,
	 .get = adm6996_get_vid,
	 },
};

static const struct switch_dev_ops adm6996_ops = {
	.attr_global = {
			.attr = adm6996_globals,
			.n_attr = ARRAY_SIZE(adm6996_globals),
			},
	.attr_port = {
		      .attr = adm6996_port,
		      .n_attr = ARRAY_SIZE(adm6996_port),
		      },
	.attr_vlan = {
		      .attr = adm6996_vlan,
		      .n_attr = ARRAY_SIZE(adm6996_vlan),
		      },
	.get_port_pvid = adm6996_get_pvid,
	.set_port_pvid = adm6996_set_pvid,
	.get_vlan_ports = adm6996_get_ports,
	.set_vlan_ports = adm6996_set_ports,
	.apply_config = adm6996_hw_apply,
	.reset_switch = adm6996_reset_switch,
};

static int adm6996_config_init(struct phy_device *pdev)
{
	struct adm6996_priv *priv;
	struct switch_dev *swdev;

	int ret;
	u16 test, old;

	pdev->supported = ADVERTISED_100baseT_Full;
	pdev->advertising = ADVERTISED_100baseT_Full;

	if (pdev->addr != 0) {
		pr_info ("%s: PHY overlaps ADM6996, providing fixed PHY 0x%x.\n"
				, pdev->attached_dev->name, pdev->addr);
		return 0;
	}

	priv = kzalloc(sizeof(struct adm6996_priv), GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	mutex_init(&priv->reg_mutex);
	priv->phydev = pdev;
	priv->read = adm6996_read_mii_reg;
	priv->write = adm6996_write_mii_reg;
	pdev->priv = priv;

	/* Detect type of chip */
	old = r16(pdev, ADM_VID_CHECK);
	test = old ^ (1 << 12);
	w16(pdev, ADM_VID_CHECK, test);
	test ^= r16(pdev, ADM_VID_CHECK);
	if (test & (1 << 12)) {
		/* 
		 * Bit 12 of this register is read-only. 
		 * This is the FC model. 
		 */
		priv->model = ADM6996FC;
	} else {
		/* Bit 12 is read-write. This is the M model. */
		priv->model = ADM6996M;
		w16(pdev, ADM_VID_CHECK, old);
	}

	swdev = &priv->dev;
	swdev->name = (adm6996_model_name[priv->model]);
	swdev->cpu_port = ADM_CPU_PORT;
	swdev->ports = ADM_NUM_PORTS;
	swdev->vlans = ADM_NUM_VLANS;
	swdev->ops = &adm6996_ops;

	pr_info ("%s: %s model PHY found.\n", pdev->attached_dev->name,
			swdev->name);

	mutex_lock(&priv->reg_mutex);
	adm6996_perform_reset (priv);
	mutex_unlock(&priv->reg_mutex);

	if (priv->model == ADM6996M) {
		if ((ret = register_switch(swdev, pdev->attached_dev)) < 0) {
			kfree(priv);
			return ret;
		}
	}

	return 0;
}

/*
 * Warning: phydev->priv is NULL if phydev->addr != 0
 */
static int adm6996_read_status(struct phy_device *phydev)
{
	phydev->speed = SPEED_100;
	phydev->duplex = DUPLEX_FULL;
	phydev->link = 1;
	return 0;
}

/*
 * Warning: phydev->priv is NULL if phydev->addr != 0
 */
static int adm6996_config_aneg(struct phy_device *phydev)
{
	return 0;
}

static int adm6996_fixup(struct phy_device *dev)
{
	struct mii_bus *bus = dev->bus;
	u16 reg;

	/* Our custom registers are at PHY addresses 0-10. Claim those. */
	if (dev->addr > 10)
		return 0;

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
	return 0;
}

static void adm6996_remove(struct phy_device *pdev)
{
	struct adm6996_priv *priv = phy_to_adm(pdev);

	if (priv != NULL && priv->model == ADM6996M)
		unregister_switch(&priv->dev);

	kfree(priv);
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
