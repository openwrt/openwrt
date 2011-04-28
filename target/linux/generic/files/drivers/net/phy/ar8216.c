/*
 * ar8216.c: AR8216 switch driver
 *
 * Copyright (C) 2009 Felix Fietkau <nbd@openwrt.org>
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
 */

#include <linux/if.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/bitops.h>
#include <net/genetlink.h>
#include <linux/switch.h>
#include <linux/delay.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include "ar8216.h"

/* size of the vlan table */
#define AR8X16_MAX_VLANS	128
#define AR8X16_PROBE_RETRIES	10

struct ar8216_priv {
	struct switch_dev dev;
	struct phy_device *phy;
	u32 (*read)(struct ar8216_priv *priv, int reg);
	void (*write)(struct ar8216_priv *priv, int reg, u32 val);
	const struct net_device_ops *ndo_old;
	struct net_device_ops ndo;
	struct mutex reg_mutex;
	int chip;
	bool initialized;
	bool port4_phy;

	/* all fields below are cleared on reset */
	bool vlan;
	u16 vlan_id[AR8X16_MAX_VLANS];
	u8 vlan_table[AR8X16_MAX_VLANS];
	u8 vlan_tagged;
	u16 pvid[AR8216_NUM_PORTS];
};

#define to_ar8216(_dev) container_of(_dev, struct ar8216_priv, dev)

static inline void
split_addr(u32 regaddr, u16 *r1, u16 *r2, u16 *page)
{
	regaddr >>= 1;
	*r1 = regaddr & 0x1e;

	regaddr >>= 5;
	*r2 = regaddr & 0x7;

	regaddr >>= 3;
	*page = regaddr & 0x1ff;
}

static u32
ar8216_mii_read(struct ar8216_priv *priv, int reg)
{
	struct phy_device *phy = priv->phy;
	u16 r1, r2, page;
	u16 lo, hi;

	split_addr((u32) reg, &r1, &r2, &page);
	phy->bus->write(phy->bus, 0x18, 0, page);
	msleep(1); /* wait for the page switch to propagate */
	lo = phy->bus->read(phy->bus, 0x10 | r2, r1);
	hi = phy->bus->read(phy->bus, 0x10 | r2, r1 + 1);

	return (hi << 16) | lo;
}

static void
ar8216_mii_write(struct ar8216_priv *priv, int reg, u32 val)
{
	struct phy_device *phy = priv->phy;
	u16 r1, r2, r3;
	u16 lo, hi;

	split_addr((u32) reg, &r1, &r2, &r3);
	phy->bus->write(phy->bus, 0x18, 0, r3);
	msleep(1); /* wait for the page switch to propagate */

	lo = val & 0xffff;
	hi = (u16) (val >> 16);
	phy->bus->write(phy->bus, 0x10 | r2, r1 + 1, hi);
	phy->bus->write(phy->bus, 0x10 | r2, r1, lo);
}

static u32
ar8216_rmw(struct ar8216_priv *priv, int reg, u32 mask, u32 val)
{
	u32 v;

	v = priv->read(priv, reg);
	v &= ~mask;
	v |= val;
	priv->write(priv, reg, v);

	return v;
}

static inline int
ar8216_id_chip(struct ar8216_priv *priv)
{
	u32 val;
	u16 id;
	int i;

	val = ar8216_mii_read(priv, AR8216_REG_CTRL);
	if (val == ~0)
		return UNKNOWN;

	id = val & (AR8216_CTRL_REVISION | AR8216_CTRL_VERSION);
	for (i = 0; i < AR8X16_PROBE_RETRIES; i++) {
		u16 t;

		val = ar8216_mii_read(priv, AR8216_REG_CTRL);
		if (val == ~0)
			return UNKNOWN;

		t = val & (AR8216_CTRL_REVISION | AR8216_CTRL_VERSION);
		if (t != id)
			return UNKNOWN;
	}

	switch (id) {
	case 0x0101:
		return AR8216;
	case 0x1000:
	case 0x1001:
		return AR8316;
	default:
		printk(KERN_DEBUG
			"ar8216: Unknown Atheros device [ver=%d, rev=%d, phy_id=%04x%04x]\n",
			(int)(id >> AR8216_CTRL_VERSION_S),
			(int)(id & AR8216_CTRL_REVISION),
			priv->phy->bus->read(priv->phy->bus, priv->phy->addr, 2),
			priv->phy->bus->read(priv->phy->bus, priv->phy->addr, 3));

		return UNKNOWN;
	}
}

static int
ar8216_set_vlan(struct switch_dev *dev, const struct switch_attr *attr,
                struct switch_val *val)
{
	struct ar8216_priv *priv = to_ar8216(dev);
	priv->vlan = !!val->value.i;
	return 0;
}

static int
ar8216_get_vlan(struct switch_dev *dev, const struct switch_attr *attr,
                struct switch_val *val)
{
	struct ar8216_priv *priv = to_ar8216(dev);
	val->value.i = priv->vlan;
	return 0;
}


static int
ar8216_set_pvid(struct switch_dev *dev, int port, int vlan)
{
	struct ar8216_priv *priv = to_ar8216(dev);

	/* make sure no invalid PVIDs get set */

	if (vlan >= dev->vlans)
		return -EINVAL;

	priv->pvid[port] = vlan;
	return 0;
}

static int
ar8216_get_pvid(struct switch_dev *dev, int port, int *vlan)
{
	struct ar8216_priv *priv = to_ar8216(dev);
	*vlan = priv->pvid[port];
	return 0;
}

static int
ar8216_set_vid(struct switch_dev *dev, const struct switch_attr *attr,
                struct switch_val *val)
{
	struct ar8216_priv *priv = to_ar8216(dev);
	priv->vlan_id[val->port_vlan] = val->value.i;
	return 0;
}

static int
ar8216_get_vid(struct switch_dev *dev, const struct switch_attr *attr,
                struct switch_val *val)
{
	struct ar8216_priv *priv = to_ar8216(dev);
	val->value.i = priv->vlan_id[val->port_vlan];
	return 0;
}


static int
ar8216_mangle_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct ar8216_priv *priv = dev->phy_ptr;
	unsigned char *buf;

    if (unlikely(!priv))
        goto error;

	if (!priv->vlan)
		goto send;

	if (unlikely(skb_headroom(skb) < 2)) {
		if (pskb_expand_head(skb, 2, 0, GFP_ATOMIC) < 0)
			goto error;
	}

	buf = skb_push(skb, 2);
	buf[0] = 0x10;
	buf[1] = 0x80;

send:
	return priv->ndo_old->ndo_start_xmit(skb, dev);

error:
	dev_kfree_skb_any(skb);
	return 0;
}

static int
ar8216_mangle_rx(struct sk_buff *skb, int napi)
{
	struct ar8216_priv *priv;
	struct net_device *dev;
	unsigned char *buf;
	int port, vlan;

	dev = skb->dev;
	if (!dev)
		goto error;

	priv = dev->phy_ptr;
	if (!priv)
		goto error;

	/* don't strip the header if vlan mode is disabled */
	if (!priv->vlan)
		goto recv;

	/* strip header, get vlan id */
	buf = skb->data;
	skb_pull(skb, 2);

	/* check for vlan header presence */
	if ((buf[12 + 2] != 0x81) || (buf[13 + 2] != 0x00))
		goto recv;

	port = buf[0] & 0xf;

	/* no need to fix up packets coming from a tagged source */
	if (priv->vlan_tagged & (1 << port))
		goto recv;

	/* lookup port vid from local table, the switch passes an invalid vlan id */
	vlan = priv->vlan_id[priv->pvid[port]];

	buf[14 + 2] &= 0xf0;
	buf[14 + 2] |= vlan >> 8;
	buf[15 + 2] = vlan & 0xff;

recv:
	skb->protocol = eth_type_trans(skb, skb->dev);

	if (napi)
		return netif_receive_skb(skb);
	else
		return netif_rx(skb);

error:
	/* no vlan? eat the packet! */
	dev_kfree_skb_any(skb);
	return NET_RX_DROP;
}

static int
ar8216_netif_rx(struct sk_buff *skb)
{
	return ar8216_mangle_rx(skb, 0);
}

static int
ar8216_netif_receive_skb(struct sk_buff *skb)
{
	return ar8216_mangle_rx(skb, 1);
}


static struct switch_attr ar8216_globals[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.set = ar8216_set_vlan,
		.get = ar8216_get_vlan,
		.max = 1
	},
};

static struct switch_attr ar8216_port[] = {
};

static struct switch_attr ar8216_vlan[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "vid",
		.description = "VLAN ID (0-4094)",
		.set = ar8216_set_vid,
		.get = ar8216_get_vid,
		.max = 4094,
	},
};


static int
ar8216_get_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct ar8216_priv *priv = to_ar8216(dev);
	u8 ports = priv->vlan_table[val->port_vlan];
	int i;

	val->len = 0;
	for (i = 0; i < AR8216_NUM_PORTS; i++) {
		struct switch_port *p;

		if (!(ports & (1 << i)))
			continue;

		p = &val->value.ports[val->len++];
		p->id = i;
		if (priv->vlan_tagged & (1 << i))
			p->flags = (1 << SWITCH_PORT_FLAG_TAGGED);
		else
			p->flags = 0;
	}
	return 0;
}

static int
ar8216_set_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct ar8216_priv *priv = to_ar8216(dev);
	u8 *vt = &priv->vlan_table[val->port_vlan];
	int i, j;

	*vt = 0;
	for (i = 0; i < val->len; i++) {
		struct switch_port *p = &val->value.ports[i];

		if (p->flags & (1 << SWITCH_PORT_FLAG_TAGGED))
			priv->vlan_tagged |= (1 << p->id);
		else {
			priv->vlan_tagged &= ~(1 << p->id);
			priv->pvid[p->id] = val->port_vlan;

			/* make sure that an untagged port does not
			 * appear in other vlans */
			for (j = 0; j < AR8X16_MAX_VLANS; j++) {
				if (j == val->port_vlan)
					continue;
				priv->vlan_table[j] &= ~(1 << p->id);
			}
		}

		*vt |= 1 << p->id;
	}
	return 0;
}

static int
ar8216_wait_bit(struct ar8216_priv *priv, int reg, u32 mask, u32 val)
{
	int timeout = 20;

	while ((priv->read(priv, reg) & mask) != val) {
		if (timeout-- <= 0) {
			printk(KERN_ERR "ar8216: timeout waiting for operation to complete\n");
			return 1;
		}
	}
	return 0;
}

static void
ar8216_vtu_op(struct ar8216_priv *priv, u32 op, u32 val)
{
	if (ar8216_wait_bit(priv, AR8216_REG_VTU, AR8216_VTU_ACTIVE, 0))
		return;
	if ((op & AR8216_VTU_OP) == AR8216_VTU_OP_LOAD) {
		val &= AR8216_VTUDATA_MEMBER;
		val |= AR8216_VTUDATA_VALID;
		priv->write(priv, AR8216_REG_VTU_DATA, val);
	}
	op |= AR8216_VTU_ACTIVE;
	priv->write(priv, AR8216_REG_VTU, op);
}

static int
ar8216_hw_apply(struct switch_dev *dev)
{
	struct ar8216_priv *priv = to_ar8216(dev);
	u8 portmask[AR8216_NUM_PORTS];
	int i, j;

	mutex_lock(&priv->reg_mutex);
	/* flush all vlan translation unit entries */
	ar8216_vtu_op(priv, AR8216_VTU_OP_FLUSH, 0);

	memset(portmask, 0, sizeof(portmask));
	if (priv->vlan) {
		/* calculate the port destination masks and load vlans
		 * into the vlan translation unit */
		for (j = 0; j < AR8X16_MAX_VLANS; j++) {
			u8 vp = priv->vlan_table[j];

			if (!vp)
				continue;

			for (i = 0; i < AR8216_NUM_PORTS; i++) {
				u8 mask = (1 << i);
				if (vp & mask)
					portmask[i] |= vp & ~mask;
			}

			ar8216_vtu_op(priv,
				AR8216_VTU_OP_LOAD |
				(priv->vlan_id[j] << AR8216_VTU_VID_S),
				priv->vlan_table[j]);
		}
	} else {
		/* vlan disabled:
		 * isolate all ports, but connect them to the cpu port */
		for (i = 0; i < AR8216_NUM_PORTS; i++) {
			if (i == AR8216_PORT_CPU)
				continue;

			portmask[i] = 1 << AR8216_PORT_CPU;
			portmask[AR8216_PORT_CPU] |= (1 << i);
		}
	}

	/* update the port destination mask registers and tag settings */
	for (i = 0; i < AR8216_NUM_PORTS; i++) {
		int egress, ingress;
		int pvid;

		if (priv->vlan) {
			pvid = priv->vlan_id[priv->pvid[i]];
		} else {
			pvid = i;
		}

		if (priv->vlan && (priv->vlan_tagged & (1 << i))) {
			egress = AR8216_OUT_ADD_VLAN;
		} else {
			egress = AR8216_OUT_STRIP_VLAN;
		}
		if (priv->vlan) {
			ingress = AR8216_IN_SECURE;
		} else {
			ingress = AR8216_IN_PORT_ONLY;
		}

		ar8216_rmw(priv, AR8216_REG_PORT_CTRL(i),
			AR8216_PORT_CTRL_LEARN | AR8216_PORT_CTRL_VLAN_MODE |
			AR8216_PORT_CTRL_SINGLE_VLAN | AR8216_PORT_CTRL_STATE |
			AR8216_PORT_CTRL_HEADER | AR8216_PORT_CTRL_LEARN_LOCK,
			AR8216_PORT_CTRL_LEARN |
			  (priv->vlan && i == AR8216_PORT_CPU && (priv->chip == AR8216) ?
			   AR8216_PORT_CTRL_HEADER : 0) |
			  (egress << AR8216_PORT_CTRL_VLAN_MODE_S) |
			  (AR8216_PORT_STATE_FORWARD << AR8216_PORT_CTRL_STATE_S));

		ar8216_rmw(priv, AR8216_REG_PORT_VLAN(i),
			AR8216_PORT_VLAN_DEST_PORTS | AR8216_PORT_VLAN_MODE |
			  AR8216_PORT_VLAN_DEFAULT_ID,
			(portmask[i] << AR8216_PORT_VLAN_DEST_PORTS_S) |
			  (ingress << AR8216_PORT_VLAN_MODE_S) |
			  (pvid << AR8216_PORT_VLAN_DEFAULT_ID_S));
	}
	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static int
ar8316_hw_init(struct ar8216_priv *priv) {
	int i;
	u32 val, newval;
	struct mii_bus *bus;

	val = priv->read(priv, 0x8);

	if (priv->phy->interface == PHY_INTERFACE_MODE_RGMII) {
		if (priv->port4_phy) {
			/* value taken from Ubiquiti RouterStation Pro */
			newval = 0x81461bea;
			printk(KERN_INFO "ar8316: Using port 4 as PHY\n");
		} else {
			newval = 0x01261be2;
			printk(KERN_INFO "ar8316: Using port 4 as switch port\n");
		}
	} else if (priv->phy->interface == PHY_INTERFACE_MODE_GMII) {
		/* value taken from AVM Fritz!Box 7390 sources */
		newval = 0x010e5b71;
	} else {
		/* no known value for phy interface */
		printk(KERN_ERR "ar8316: unsupported mii mode: %d.\n",
			priv->phy->interface);
		return -EINVAL;
	}

	if (val == newval)
		goto out;

	priv->write(priv, 0x8, newval);

	/* standard atheros magic */
	priv->write(priv, 0x38, 0xc000050e);

	/* Initialize the ports */
	bus = priv->phy->bus;
	for (i = 0; i < 5; i++) {
		if ((i == 4) && priv->port4_phy &&
		    priv->phy->interface == PHY_INTERFACE_MODE_RGMII) {
			/* work around for phy4 rgmii mode */
			bus->write(bus, i, MII_ATH_DBG_ADDR, 0x12);
			bus->write(bus, i, MII_ATH_DBG_DATA, 0x480c);
			/* rx delay */
			bus->write(bus, i, MII_ATH_DBG_ADDR, 0x0);
			bus->write(bus, i, MII_ATH_DBG_DATA, 0x824e);
			/* tx delay */
			bus->write(bus, i, MII_ATH_DBG_ADDR, 0x5);
			bus->write(bus, i, MII_ATH_DBG_DATA, 0x3d47);
			msleep(1000);
		}

		/* initialize the port itself */
		bus->write(bus, i, MII_ADVERTISE,
			ADVERTISE_ALL | ADVERTISE_PAUSE_CAP | ADVERTISE_PAUSE_ASYM);
		bus->write(bus, i, MII_CTRL1000, ADVERTISE_1000FULL);
		bus->write(bus, i, MII_BMCR, BMCR_RESET | BMCR_ANENABLE);
		msleep(1000);
	}

out:
	priv->initialized = true;
	return 0;
}

static int
ar8216_reset_switch(struct switch_dev *dev)
{
	struct ar8216_priv *priv = to_ar8216(dev);
	int i;

	mutex_lock(&priv->reg_mutex);
	memset(&priv->vlan, 0, sizeof(struct ar8216_priv) -
		offsetof(struct ar8216_priv, vlan));
	for (i = 0; i < AR8X16_MAX_VLANS; i++) {
		priv->vlan_id[i] = i;
	}
	for (i = 0; i < AR8216_NUM_PORTS; i++) {
		/* Enable port learning and tx */
		priv->write(priv, AR8216_REG_PORT_CTRL(i),
			AR8216_PORT_CTRL_LEARN |
			(4 << AR8216_PORT_CTRL_STATE_S));

		priv->write(priv, AR8216_REG_PORT_VLAN(i), 0);

		/* Configure all PHYs */
		if (i == AR8216_PORT_CPU) {
			priv->write(priv, AR8216_REG_PORT_STATUS(i),
				AR8216_PORT_STATUS_LINK_UP |
				((priv->chip == AR8316) ?
					AR8216_PORT_SPEED_1000M : AR8216_PORT_SPEED_100M) |
				AR8216_PORT_STATUS_TXMAC |
				AR8216_PORT_STATUS_RXMAC |
				((priv->chip == AR8316) ? AR8216_PORT_STATUS_RXFLOW : 0) |
				((priv->chip == AR8316) ? AR8216_PORT_STATUS_TXFLOW : 0) |
				AR8216_PORT_STATUS_DUPLEX);
		} else {
			priv->write(priv, AR8216_REG_PORT_STATUS(i),
				AR8216_PORT_STATUS_LINK_AUTO);
		}
	}
	/* XXX: undocumented magic from atheros, required! */
	priv->write(priv, 0x38, 0xc000050e);

	if (priv->chip == AR8216) {
		ar8216_rmw(priv, AR8216_REG_GLOBAL_CTRL,
			AR8216_GCTRL_MTU, 1518 + 8 + 2);
	} else if (priv->chip == AR8316) {
		/* enable jumbo frames */
		ar8216_rmw(priv, AR8216_REG_GLOBAL_CTRL,
			AR8316_GCTRL_MTU, 9018 + 8 + 2);
	}

	if (priv->chip == AR8316) {
		/* enable cpu port to receive multicast and broadcast frames */
		priv->write(priv, AR8216_REG_FLOOD_MASK, 0x003f003f);
	}
	mutex_unlock(&priv->reg_mutex);
	return ar8216_hw_apply(dev);
}


static const struct switch_dev_ops ar8216_ops = {
	.attr_global = {
		.attr = ar8216_globals,
		.n_attr = ARRAY_SIZE(ar8216_globals),
	},
	.attr_port = {
		.attr = ar8216_port,
		.n_attr = ARRAY_SIZE(ar8216_port),
	},
	.attr_vlan = {
		.attr = ar8216_vlan,
		.n_attr = ARRAY_SIZE(ar8216_vlan),
	},
	.get_port_pvid = ar8216_get_pvid,
	.set_port_pvid = ar8216_set_pvid,
	.get_vlan_ports = ar8216_get_ports,
	.set_vlan_ports = ar8216_set_ports,
	.apply_config = ar8216_hw_apply,
	.reset_switch = ar8216_reset_switch,
};

static int
ar8216_config_init(struct phy_device *pdev)
{
	struct ar8216_priv *priv = pdev->priv;
	struct net_device *dev = pdev->attached_dev;
	struct switch_dev *swdev;
	int ret;

	if (!priv) {
		priv = kzalloc(sizeof(struct ar8216_priv), GFP_KERNEL);
		if (priv == NULL)
			return -ENOMEM;
	}

	priv->phy = pdev;

	priv->chip = ar8216_id_chip(priv);

	if (pdev->addr != 0) {
		if (priv->chip == AR8316) {
			pdev->supported |= SUPPORTED_1000baseT_Full;
			pdev->advertising |= ADVERTISED_1000baseT_Full;

			/* check if we're attaching to the switch twice */
			pdev = pdev->bus->phy_map[0];
			if (!pdev) {
				kfree(priv);
				return 0;
			}

			/* switch device has not been initialized, reuse priv */
			if (!pdev->priv) {
				priv->port4_phy = true;
				pdev->priv = priv;
				return 0;
			}

			kfree(priv);

			/* switch device has been initialized, reinit */
			priv = pdev->priv;
			priv->dev.ports = (AR8216_NUM_PORTS - 1);
			priv->initialized = false;
			priv->port4_phy = true;
			ar8316_hw_init(priv);
			return 0;
		}

		kfree(priv);
		return 0;
	}

	printk(KERN_INFO "%s: AR%d switch driver attached.\n",
		pdev->attached_dev->name, priv->chip);

	pdev->supported = priv->chip == AR8316 ?
		SUPPORTED_1000baseT_Full : SUPPORTED_100baseT_Full;
	pdev->advertising = pdev->supported;

	mutex_init(&priv->reg_mutex);
	priv->read = ar8216_mii_read;
	priv->write = ar8216_mii_write;

	pdev->priv = priv;

	swdev = &priv->dev;
	swdev->cpu_port = AR8216_PORT_CPU;
	swdev->ops = &ar8216_ops;
	swdev->ports = AR8216_NUM_PORTS;

	if (priv->chip == AR8316) {
		swdev->name = "Atheros AR8316";
		swdev->vlans = AR8X16_MAX_VLANS;

		if (priv->port4_phy) {
			/* port 5 connected to the other mac, therefore unusable */
			swdev->ports = (AR8216_NUM_PORTS - 1);
		}
	} else {
		swdev->name = "Atheros AR8216";
		swdev->vlans = AR8216_NUM_VLANS;
	}

	if ((ret = register_switch(&priv->dev, pdev->attached_dev)) < 0) {
		kfree(priv);
		goto done;
	}

	if (priv->chip == AR8316) {
		ret = ar8316_hw_init(priv);
		if (ret) {
			kfree(priv);
			goto done;
		}
	}

	ret = ar8216_reset_switch(&priv->dev);
	if (ret) {
		kfree(priv);
		goto done;
	}

	dev->phy_ptr = priv;

	/* VID fixup only needed on ar8216 */
	if (pdev->addr == 0 && priv->chip == AR8216) {
		pdev->pkt_align = 2;
		pdev->netif_receive_skb = ar8216_netif_receive_skb;
		pdev->netif_rx = ar8216_netif_rx;
		priv->ndo_old = dev->netdev_ops;
		memcpy(&priv->ndo, priv->ndo_old, sizeof(struct net_device_ops));
		priv->ndo.ndo_start_xmit = ar8216_mangle_tx;
		dev->netdev_ops = &priv->ndo;
	}

done:
	return ret;
}

static int
ar8216_read_status(struct phy_device *phydev)
{
	struct ar8216_priv *priv = phydev->priv;
	int ret;
	if (phydev->addr != 0) {
		return genphy_read_status(phydev);
	}

	phydev->speed = priv->chip == AR8316 ? SPEED_1000 : SPEED_100;
	phydev->duplex = DUPLEX_FULL;
	phydev->link = 1;

	/* flush the address translation unit */
	mutex_lock(&priv->reg_mutex);
	ret = ar8216_wait_bit(priv, AR8216_REG_ATU, AR8216_ATU_ACTIVE, 0);

	if (!ret)
		priv->write(priv, AR8216_REG_ATU, AR8216_ATU_OP_FLUSH);
	else
		ret = -ETIMEDOUT;
	mutex_unlock(&priv->reg_mutex);

	phydev->state = PHY_RUNNING;
	netif_carrier_on(phydev->attached_dev);
	phydev->adjust_link(phydev->attached_dev);

	return ret;
}

static int
ar8216_config_aneg(struct phy_device *phydev)
{
	if (phydev->addr == 0)
		return 0;

	return genphy_config_aneg(phydev);
}

static int
ar8216_probe(struct phy_device *pdev)
{
	struct ar8216_priv priv;
	u16 chip;

	priv.phy = pdev;
	chip = ar8216_id_chip(&priv);
	if (chip == UNKNOWN)
		return -ENODEV;

	return 0;
}

static void
ar8216_remove(struct phy_device *pdev)
{
	struct ar8216_priv *priv = pdev->priv;
	struct net_device *dev = pdev->attached_dev;

	if (!priv)
		return;

	if (priv->ndo_old && dev)
		dev->netdev_ops = priv->ndo_old;
	if (pdev->addr == 0)
		unregister_switch(&priv->dev);
	kfree(priv);
}

static struct phy_driver ar8216_driver = {
	.phy_id		= 0x004d0000,
	.name		= "Atheros AR8216/AR8316",
	.phy_id_mask	= 0xffff0000,
	.features	= PHY_BASIC_FEATURES,
	.probe		= ar8216_probe,
	.remove		= ar8216_remove,
	.config_init	= &ar8216_config_init,
	.config_aneg	= &ar8216_config_aneg,
	.read_status	= &ar8216_read_status,
	.driver		= { .owner = THIS_MODULE },
};

int __init
ar8216_init(void)
{
	return phy_driver_register(&ar8216_driver);
}

void __exit
ar8216_exit(void)
{
	phy_driver_unregister(&ar8216_driver);
}

module_init(ar8216_init);
module_exit(ar8216_exit);
MODULE_LICENSE("GPL");

