/*
 *  Realtek RTL865X built-in switch driver
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  Based on Realtek RE865X asic driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/if_vlan.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/sort.h>
#include <linux/bsearch.h>

#include <asm/mach-realtek/realtek.h>

#include "rtl865x.h"

#define RTL865X_MII_PHY_MASK		(~(BIT(RTL865X_MAX_PHYS) - 1))

/* milliseconds */
#define RTL865X_MDIO_WAIT_TIMEOUT	50

/* microseconds */
#define RTL865X_MDIO_WAIT_DELAY		10

#define RTL865X_LUT_RETRY_COUNT		1000

#define swdev_to_rtl865x(_dev) container_of(_dev, struct rtl865x, swdev)


static inline u32 rtl865x_reg_read(struct rtl865x *rsw, unsigned reg)
{
	return __raw_readl(rsw->swcore_base + reg);
}

static inline void rtl865x_reg_write(struct rtl865x *rsw, unsigned reg, u32 value)
{
	__raw_writel(value, rsw->swcore_base + reg);
}

static inline void rtl865x_reg_rmw(struct rtl865x *rsw, unsigned reg, u32 clear, u32 set)
{
	u32 value;

	value = __raw_readl(rsw->swcore_base + reg);
	value &= ~clear;
	value |= set;
	__raw_writel(value, rsw->swcore_base + reg);
}

static int rtl865x_mdio_wait(struct rtl865x *rsw)
{
	unsigned long timeout = msecs_to_jiffies(RTL865X_MDIO_WAIT_TIMEOUT);

	do {
		u32 status;

		status = rtl865x_reg_read(rsw, RTL865X_SW_REG_MDIO_STATUS);
		if (!(status & RTL865X_SW_MDIO_STATUS_BUSY))
			return 0;

		udelay(RTL865X_MDIO_WAIT_DELAY);
	} while (time_before(jiffies, timeout));

	pr_err("%s: MDIO operation timed out\n", rsw->mii_sw->name);

	return -ETIMEDOUT;
}

static int rtl865x_mdio_read(struct mii_bus *bus, int addr, int reg)
{
	struct rtl865x *rsw = (struct rtl865x *) bus->priv;
	int status;

	if (rtl865x_mdio_wait(rsw))
		return 0xffff;

	rtl865x_reg_write(rsw, RTL865X_SW_REG_MDIO_CMD,
		((addr << RTL865X_SW_MDIO_PHY_ADDR_SHIFT) & RTL865X_SW_MDIO_PHY_ADDR_MASK) |
		((reg << RTL865X_SW_MDIO_REG_ADDR_SHIFT) & RTL865X_SW_MDIO_REG_ADDR_MASK));

	status = rtl865x_reg_read(rsw, RTL865X_SW_REG_MDIO_STATUS);

	return (status & RTL865X_SW_MDIO_READ_DATA_MASK) >> RTL865X_SW_MDIO_READ_DATA_SHIFT;
}

static int rtl865x_mdio_write(struct mii_bus *bus, int addr, int reg, u16 val)
{
	struct rtl865x *rsw = (struct rtl865x *) bus->priv;

	rtl865x_mdio_wait(rsw);

	rtl865x_reg_write(rsw, RTL865X_SW_REG_MDIO_CMD,
		RTL865X_SW_MDIO_CMD_WRITE |
		((addr << RTL865X_SW_MDIO_PHY_ADDR_SHIFT) & RTL865X_SW_MDIO_PHY_ADDR_MASK) |
		((reg << RTL865X_SW_MDIO_REG_ADDR_SHIFT) & RTL865X_SW_MDIO_REG_ADDR_MASK) |
		((((u32) val) << RTL865X_SW_MDIO_READ_DATA_SHIFT) & RTL865X_SW_MDIO_READ_DATA_MASK));

	return 0;
}

static int rtl865x_register_mdio(struct rtl865x *rsw)
{
	int i;
	int err;

	rsw->mii_sw = mdiobus_alloc();
	if (!rsw->mii_sw)
		return -ENOMEM;

	rsw->mii_sw->name = "rtl865x_mdio";
	rsw->mii_sw->read = rtl865x_mdio_read;
	rsw->mii_sw->write = rtl865x_mdio_write;
	rsw->mii_sw->phy_mask = RTL865X_MII_PHY_MASK;
	memcpy(rsw->mii_sw->irq, rsw->mii_irq, sizeof(rsw->mii_irq));
	rsw->mii_sw->priv = rsw;
	rsw->mii_sw->parent = rsw->parent;
	snprintf(rsw->mii_sw->id, MII_BUS_ID_SIZE, "rtl865x-mdio");

	for (i = 0; i < PHY_MAX_ADDR; i++)
		rsw->mii_irq[i] = PHY_POLL;

	err = mdiobus_register(rsw->mii_sw);
	if (err)
		goto err_free_bus;

	return 0;

err_free_bus:
	mdiobus_free(rsw->mii_sw);
	return err;
}

static void rtl865x_unregister_mdio(struct rtl865x *rsw)
{
	mdiobus_unregister(rsw->mii_sw);
	mdiobus_free(rsw->mii_sw);
}

static void rtl865x_phy_page_reg_rmw(struct rtl865x *rsw, int addr, int page, int reg, u16 clear, u16 set)
{
	u16 orig_data = 0;

	if (page >= 31) {
		mdiobus_write(rsw->mii_sw, addr, 31, 7);
		mdiobus_write(rsw->mii_sw, addr, 30, page);
	} else {
		mdiobus_write(rsw->mii_sw, addr, 31, page);
	}

	if (clear != 0xffff)
	{
		orig_data = mdiobus_read(rsw->mii_sw, addr, reg);
		orig_data &= ~clear;
	}

	mdiobus_write(rsw->mii_sw, addr, reg, orig_data | set);

	mdiobus_write(rsw->mii_sw, addr, 31, 0);
}

static void rtl8196c_revb_fixup(struct rtl865x *rsw)
{
	int i;
	u16 val;

	for (i = 0; i < RTL865X_NUM_PHY_PORTS; i++)
		rtl865x_reg_rmw(rsw, RTL865X_SW_REG_PORT_CONFIG(i), 0, RTL865X_SW_96C_98_PORT_FORCED_MODE);

	for (i = 0; i < RTL865X_MAX_PHYS; i++) {
		rtl865x_phy_page_reg_rmw(rsw, i, 1, 17, 7 << 10, 7 << 10);
		rtl865x_phy_page_reg_rmw(rsw, i, 4, 24, 0xff, 0xf3);
		rtl865x_phy_page_reg_rmw(rsw, i, 4, 16, 1 << 3, 1 << 3);
		rtl865x_phy_page_reg_rmw(rsw, i, 1, 19, 7 << 11, 2 << 11);
		rtl865x_phy_page_reg_rmw(rsw, i, 1, 23, 7 << 6, 4 << 6);
		rtl865x_phy_page_reg_rmw(rsw, i, 1, 18, 7 << 3, 6 << 3);
	}

	/* 100M half duplex enhancement */
	rtl865x_reg_rmw(rsw, RTL865X_SW_REG_MAC_CONFIG, RTL865X_SW_CF_RXIPG_MASK, 0x5);

	/* fix the link down / link up issue when RTL8196c set to Auto-negotiation
	    and SmartBit force to 100M Full-duplex */
	rtl865x_reg_rmw(rsw, RTL865X_SW_REG_MAC_CONFIG,
		RTL865X_SW_SELIPG_MASK << RTL865X_SW_SELIPG_SHIFT,
		RTL865X_SW_SELIPG_11 << RTL865X_SW_SELIPG_SHIFT);

	for (i = 0; i < RTL865X_MAX_PHYS; i++) {
		rtl865x_phy_page_reg_rmw(rsw, i, 0, 21, 0xff, 0x32);
		rtl865x_phy_page_reg_rmw(rsw, i, 0, 22, 7 << 4, 5 << 4);
		rtl865x_phy_page_reg_rmw(rsw, i, 0, 0, 1 << 9, 1 << 9); /* Auto Neg. */
		rtl865x_phy_page_reg_rmw(rsw, i, 1, 17, 3 << 1, 3 << 1);
		rtl865x_phy_page_reg_rmw(rsw, i, 1, 18, 0xffff, 0x9004);
		rtl865x_phy_page_reg_rmw(rsw, i, 4, 26, 0xfff0, 0xff80);
		rtl865x_phy_page_reg_rmw(rsw, i, 0, 21, 1 << 14, 1 << 14);
	}

	for (i = 0; i < RTL865X_NUM_PHY_PORTS; i++)
		rtl865x_reg_rmw(rsw, RTL865X_SW_REG_PORT_CONFIG(i), RTL865X_SW_96C_98_PORT_FORCED_MODE, 0);

	/* disable 10M power saving */
	for (i = 0; i < RTL865X_MAX_PHYS; i++) {
		val = mdiobus_read(rsw->mii_sw, i, 0x18);
		if (val == 0xffff)
			continue;
		val &= ~(1 << 15);
		mdiobus_write(rsw->mii_sw, i, 0x18, val);
	}
}

static void rtl8196c_port_setup(struct rtl865x *rsw)
{
	int i;

	if (soc_is_rtl8196c_rev_b())
		rtl8196c_revb_fixup(rsw);

	for (i = 0; i < rsw->num_ports; i++)
	{
		rtl865x_reg_rmw(rsw, RTL865X_SW_REG_PORT_CONFIG(i), RTL865X_SW_96C_98_PORT_MAC_RESET_L, 0);
		rtl865x_reg_rmw(rsw, RTL865X_SW_REG_PORT_CONFIG(i), 0,
			(i << RTL865X_SW_96C_98_PORT_EXT_PHY_ID_SHIFT) |
			(RTL865X_SW_PORT_PACKET_LENGTH_1536 << RTL865X_SW_PORT_PACKET_LENGTH_SHIFT) |
			RTL865X_SW_PORT_ENABLE_PHY_IF |
			RTL865X_SW_96C_98_PORT_MAC_RESET_L);
	}
}

static void rtl865x_lut_start(struct rtl865x *rsw)
{
	rtl865x_reg_rmw(rsw, RTL865X_SW_REG_TABLE_CONTROL_0, RTL865X_SW_TLU_STOP, 0);
}

static void rtl865x_lut_stop(struct rtl865x *rsw)
{
	int retry = RTL865X_LUT_RETRY_COUNT;

	rtl865x_reg_rmw(rsw, RTL865X_SW_REG_TABLE_CONTROL_0, 0, RTL865X_SW_TLU_STOP);

	do {
		u32 status = rtl865x_reg_read(rsw, RTL865X_SW_REG_TABLE_CONTROL_0);

		if (status & RTL865X_SW_TLU_STOPPED)
			return;
	} while (--retry);

	dev_err(rsw->parent, "timed out stopping switch LUT");
}

static void rtl865x_lut_data_write(struct rtl865x *rsw, const u32 data[8])
{
	int i;

	for (i = 0; i < RTL865X_LUT_TABLE_NUM_DATA_REGS; i++)
		rtl865x_reg_write(rsw, RTL865X_SW_REG_TABLE_ACC_DATA(i), data[i]);
}

static void rtl865x_lut_write_vlan_entry(struct rtl865x *rsw, int vid, const union rtl865x_table_vlan *vt)
{
	int retry = RTL865X_LUT_RETRY_COUNT;

	/* write entry data to registers */
	rtl865x_lut_data_write(rsw, vt->data);

	/* table address of vid */
	rtl865x_reg_write(rsw, RTL865X_SW_REG_TABLE_ACC_ADDRESS, rtl865x_vlan_table_address(rsw, vid));

	/* force write operation */
	rtl865x_reg_rmw(rsw, RTL865X_SW_REG_TABLE_ACC_CONTROL, 0, RTL865X_SW_TABLE_CMD_FORCE | RTL865X_SW_TABLE_ACTION);

	do {
		u32 status = rtl865x_reg_read(rsw, RTL865X_SW_REG_TABLE_ACC_CONTROL);

		if (!(status & RTL865X_SW_TABLE_ACTION))
			return;
	} while (--retry);

	dev_err(rsw->parent, "timed out writting vlan entry");
}

static int __vlan_cache_cmp(const void *p1, const void *p2)
{
	struct rtl865x_vlan_cache *pvc1 = (struct rtl865x_vlan_cache *) p1;
	struct rtl865x_vlan_cache *pvc2 = (struct rtl865x_vlan_cache *) p2;

	return (int) pvc1->vid - (int) pvc2->vid;
}

static void __vlan_cache_swap_func(void *p1, void *p2, int size)
{
	struct rtl865x_vlan_cache *pvc1 = (struct rtl865x_vlan_cache *) p1;
	struct rtl865x_vlan_cache *pvc2 = (struct rtl865x_vlan_cache *) p2;
	struct rtl865x_vlan_cache t;

	t = *pvc1;
	*pvc1 = *pvc2;
	*pvc2 = t;
}

static void rtl865x_setup_default_vlan(struct rtl865x *rsw)
{
	int i;
	u16 vid;

	/* reset vlan configurations */
	memset(rsw->vlan_members, 0, sizeof (rsw->vlan_members));
	memset(rsw->vlan_untag_members, 0, sizeof (rsw->vlan_untag_members));
	memset(rsw->pvid, 0, sizeof (rsw->pvid));

	for (i = 0; i < rsw->num_ports; i++) {
		vid = i + 1;
		rsw->pvid[i] = vid;
		rsw->vlan_members[vid] = BIT(i);
		rsw->vlan_untag_members[vid] = BIT(i);
	}
}

static void rtl865x_setup_vlan(struct rtl865x *rsw)
{
	union rtl865x_table_vlan vt;
	u8 port_mask = BIT(rsw->num_ports) - 1;
	int i;
	bool restart_xmit = false;

	if (!netif_queue_stopped(rsw->dev)) {
		restart_xmit = true;
		netif_stop_queue(rsw->dev);
	}

	memset(&vt, 0, sizeof (vt));

	if (!rsw->vlan_enabled)
		rtl865x_setup_default_vlan(rsw);

	rtl865x_lut_stop(rsw);

	/* delete all vlan entries */
	for (i = 0; i < RTL865X_MAX_VLANS; i++) {
		/* empty entry means deletion */
		rtl865x_lut_write_vlan_entry(rsw, i, &vt);
	}

	/* write new vlan entries */
	for (i = 0; i < RTL865X_MAX_VLANS; i++) {
		if (!rsw->vlan_members[i])
			continue;

		/* exclude dummy cpu port bit */
		vt.member_port = rsw->vlan_members[i] & port_mask;
		vt.egress_untag = rsw->vlan_untag_members[i] & port_mask;

		/* write entry */
		rtl865x_lut_write_vlan_entry(rsw, rsw->vlan_id[i], &vt);
	}

	/* write port vid */
	for (i = 0; i < rsw->num_ports; i++) {
		if (i % 2)
			rtl865x_reg_rmw(rsw,
					RTL865X_SW_REG_PORT_VLAN_CONFIG(i / 2),
					RTL865X_SW_PORT_VLAN_ODD_MASK << RTL865X_SW_PORT_VLAN_ODD_SHIFT,
					rsw->pvid[i] << RTL865X_SW_PORT_VLAN_ODD_SHIFT);
		else
			rtl865x_reg_rmw(rsw,
					RTL865X_SW_REG_PORT_VLAN_CONFIG(i / 2),
					RTL865X_SW_PORT_VLAN_EVEN_MASK << RTL865X_SW_PORT_VLAN_EVEN_SHIFT,
					rsw->pvid[i] << RTL865X_SW_PORT_VLAN_EVEN_SHIFT);
	}

	if (rsw->vlan_enabled) {
		rtl865x_reg_rmw(rsw, RTL865X_SW_REG_TABLE_CONTROL_0,
			RTL865X_SW_UNKNOWN_VLAN_FRAME_TO_CPU,
			RTL865X_SW_UNKNOWN_NAPT_FRAME_TO_CPU);
	} else {
		rtl865x_reg_rmw(rsw, RTL865X_SW_REG_TABLE_CONTROL_0, 0,
			RTL865X_SW_UNKNOWN_VLAN_FRAME_TO_CPU |
			RTL865X_SW_UNKNOWN_NAPT_FRAME_TO_CPU);
	}

	rtl865x_lut_start(rsw);

	/* setup vlan cache */
	rsw->num_vlan_caches = 0;

	for (i = 0; i < RTL865X_MAX_VLANS; i++) {
		if (!rsw->vlan_members[i])
			continue;

		rsw->vlan_caches[rsw->num_vlan_caches].vid = rsw->vlan_id[i];
		rsw->vlan_caches[rsw->num_vlan_caches].members = rsw->vlan_members[i];
		rsw->vlan_caches[rsw->num_vlan_caches].untag_members = rsw->vlan_untag_members[i];

		rsw->num_vlan_caches++;
	}

	sort(rsw->vlan_caches, rsw->num_vlan_caches, sizeof (rsw->vlan_caches[0]),
	     __vlan_cache_cmp, __vlan_cache_swap_func);

	if (restart_xmit)
		/* TODO: delay 3s */
		netif_wake_queue(rsw->dev);
}

static int rtl865x_ephy_poll_reset(struct mii_bus *bus)
{
	const unsigned int sleep_msecs = 20;
	int ret, elapsed, i;

	for (elapsed = sleep_msecs; elapsed <= 600;
	     elapsed += sleep_msecs) {
		msleep(sleep_msecs);
		for (i = 0; i < RTL865X_MAX_PHYS; i++) {
			ret = mdiobus_read(bus, i, MII_BMCR);
			if (ret < 0)
				return ret;
			if (ret & BMCR_RESET)
				break;
			if (i == RTL865X_MAX_PHYS - 1) {
				usleep_range(1000, 2000);
				return 0;
			}
		}
	}
	return -ETIMEDOUT;
}

static void rtl865x_reset(struct rtl865x *rsw)
{
	int i;

	rtl865x_reg_write(rsw, RTL865X_SW_REG_RESET, RTL865X_SW_RESET_FULL);
	mdelay(50);

	/* reset mib counters */
	rtl865x_reg_write(rsw, RTL865X_SW_REG_MIB_CONTROL, RTL865X_SW_ALL_PORT_COUNTER_RESTART);

	if (soc_is_rtl8196c())
		rtl8196c_port_setup(rsw);

	/* enable broadcast packets to cpu port */
	rtl865x_reg_rmw(rsw, RTL865X_SW_REG_FRAME_FORWARDING_CONFIG, 0,
		RTL865X_SW_UNICAST_TO_CPU | RTL865X_SW_MULTICAST_TO_CPU);

	/* enable vlan ingress filtering */
	rtl865x_reg_write(rsw, RTL865X_SW_REG_VLAN_CONTROL_0, RTL965X_SW_VLAN_INGRESS_FILTER_ALL_EN_MASK);

	/* port security */
	rtl865x_reg_rmw(rsw, RTL865X_SW_REG_TABLE_CONTROL_0,
		RTL865X_SW_LAN_DECISION_POLICY_MASK,
		RTL865X_SW_LAN_DECISION_POLICY_BY_VLAN << RTL865X_SW_LAN_DECISION_POLICY_SHIFT);

	/* enable L2 lookup engine */
	rtl865x_reg_write(rsw, RTL865X_SW_REG_MODULE_SWITCH_CONTROL, RTL865X_SW_L2_ENGINE_ENABLE);

	/* restore vlan configurations */
	rtl865x_setup_vlan(rsw);

	/* setup PHYs */
	for (i = 0; i < RTL865X_MAX_PHYS; i++) {
		mdiobus_write(rsw->mii_sw, i, MII_ADVERTISE,
				   ADVERTISE_ALL | ADVERTISE_PAUSE_CAP |
				   ADVERTISE_PAUSE_ASYM);
		mdiobus_write(rsw->mii_sw, i, MII_BMCR,
				   BMCR_RESET | BMCR_ANENABLE);
	}
	rtl865x_ephy_poll_reset(rsw->mii_sw);

	/* enable switch functionality */
	rtl865x_reg_rmw(rsw, RTL865X_SW_REG_RESET, 0, RTL865X_SW_TRX_READY);
}

bool rtl865x_get_vlan_port_members(struct rtl865x *rsw, u16 vid, u8 *pmembers, u8 *puntag_members)
{
	struct rtl865x_vlan_cache *vlc, vlb;

	if (!rsw->vlan_enabled) {
		/* packets sent to all ports */

		if (pmembers)
			*pmembers = BIT(rsw->num_ports) - 1;

		if (puntag_members)
			*puntag_members = 0;

		return true;
	}

	vlb.vid = vid;

	vlc = (struct rtl865x_vlan_cache *) bsearch(&vlb, rsw->vlan_caches, rsw->num_vlan_caches,
		sizeof (rsw->vlan_caches[0]), __vlan_cache_cmp);

	if (vlc)
		vlb = *vlc;

	if (vlc) {
		if (pmembers)
			*pmembers = vlb.members;

		if (puntag_members)
			*puntag_members = vlb.untag_members;

		return true;
	}

	return false;
}

bool rtl865x_cpu_port_is_tagged(struct rtl865x *rsw, u16 vid)
{
	u8 members;
	u8 untag_members;
	u8 cpu_port_mask;

	if (!rsw->vlan_enabled)
		return false;

	if (!rtl865x_get_vlan_port_members(rsw, vid, &members, &untag_members))
		return false;

	cpu_port_mask = BIT(rsw->num_ports);

	if (members & (~untag_members) & cpu_port_mask)
		return true;

	return false;
}

static int rtl865x_get_vlan(struct switch_dev *dev, const struct switch_attr *attr,
			    struct switch_val *val)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	val->value.i = rsw->vlan_enabled;
	return 0;
}

static int rtl865x_set_vlan(struct switch_dev *dev, const struct switch_attr *attr,
			    struct switch_val *val)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	rsw->vlan_enabled = !!val->value.i;
	return 0;
}

static int rtl865x_get_vid(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	val->value.i = rsw->vlan_id[val->port_vlan];
	return 0;
}

static int rtl865x_set_vid(struct switch_dev *dev, const struct switch_attr *attr,
			   struct switch_val *val)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	rsw->vlan_id[val->port_vlan] = val->value.i;
	return 0;
}

static int rtl865x_get_pvid(struct switch_dev *dev, int port, int *vlan)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	*vlan = rsw->pvid[port];
	return 0;
}

static int rtl865x_set_pvid(struct switch_dev *dev, int port, int vlan)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	rsw->pvid[port] = vlan;
	return 0;
}

static int rtl865x_get_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	u8 port_mask = rsw->vlan_members[val->port_vlan];
	u8 untag_mask = rsw->vlan_untag_members[val->port_vlan];
	int i;

	val->len = 0;
	for (i = 0; i < rsw->swdev.ports; i++) {
		struct switch_port *p;

		if (!(port_mask & (1 << i)))
			continue;

		p = &val->value.ports[val->len++];
		p->id = i;
		if ((untag_mask & (1 << i)) == 0)
			p->flags = (1 << SWITCH_PORT_FLAG_TAGGED);
		else
			p->flags = 0;
	}

	return 0;
}

static int rtl865x_set_vlan_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	int i, j;

	u8 port_mask = 0;
	u8 untag_mask = 0;

	for (i = 0; i < val->len; i++) {
		struct switch_port *p = &val->value.ports[i];

		if ((p->flags & (1 << SWITCH_PORT_FLAG_TAGGED)) == 0) {
			untag_mask |= 1 << p->id;
			rsw->pvid[p->id] = val->port_vlan;

			/* make sure that an untagged port does not
			 * appear in other vlans which is also untagged.
			 */
			for (j = 0; j < RTL865X_MAX_VLANS; j++) {
				if (j == val->port_vlan)
					continue;

				if (rsw->vlan_members[j] & rsw->vlan_untag_members[j] & (1 << p->id)) {
					rsw->vlan_members[j] &= ~(1 << p->id);
					rsw->vlan_untag_members[j] &= ~(1 << p->id);
				}
			}
		}

		port_mask |= 1 << p->id;
	}

	if (port_mask) {
		rsw->vlan_members[val->port_vlan] = port_mask;
		rsw->vlan_untag_members[val->port_vlan] = untag_mask;
	}

	return 0;
}

static int rtl865x_apply_config(struct switch_dev *dev)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	rtl865x_setup_vlan(rsw);
	return 0;
}

static int rtl865x_reset_switch(struct switch_dev *dev)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	rtl865x_reset(rsw);
	return 0;
}

static int rtl865x_get_port_link(struct switch_dev *dev, int port,
				 struct switch_port_link *link)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);
	u32 status;

	if (port >= rsw->num_ports) {
		if (port == rsw->num_ports) {
			/* fixed link for cpu port */
			link->aneg = false;
			link->link = true;
			link->duplex = true;
			link->tx_flow = false;
			link->rx_flow = false;
			link->speed = SWITCH_PORT_SPEED_1000;
			return 0;
		}
		return -EINVAL;
	}

	status = rtl865x_reg_read(rsw, RTL865X_SW_REG_PORT_STATUS(port));

	link->aneg = !!(status & RTL865X_SW_PORT_ANEG_ENABLED);
	if (link->aneg) {
		link->link = !!(status & RTL865X_SW_PORT_LINK_UP);
		if (!link->link)
			return 0;
	} else {
		link->link = true;
	}

	link->tx_flow = false;
	link->rx_flow = false;
	link->duplex = !!(status & RTL865X_SW_PORT_DUPLEX);

	switch (status & RTL865X_SW_PORT_SPEED_MASK) {
	case RTL865X_SW_PORT_SPEED_10M:
		link->speed = SWITCH_PORT_SPEED_10;
		break;
	case RTL865X_SW_PORT_SPEED_100M:
		link->speed = SWITCH_PORT_SPEED_100;
		break;
	case RTL865X_SW_PORT_SPEED_1000M:
		link->speed = SWITCH_PORT_SPEED_1000;
		break;
	}

	return 0;
}

static int rtl865x_get_port_stats(struct switch_dev *dev, int port,
				  struct switch_port_stats *stats)
{
	struct rtl865x *rsw = swdev_to_rtl865x(dev);

	/* no statistics for cpu port */
	if (port > rsw->num_ports)
		return -EINVAL;

	stats->rx_bytes = rtl865x_reg_read(rsw, RTL865X_SW_REG_PORT_MIB(port, RX_BYTES_LO));
	stats->tx_bytes = rtl865x_reg_read(rsw, RTL865X_SW_REG_PORT_MIB(port, TX_BYTES_HI));

	return 0;
}

static struct switch_attr rtl865x_globals[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.get = rtl865x_get_vlan,
		.set = rtl865x_set_vlan,
		.max = 1
	},
};

static struct switch_attr rtl865x_port[] = {
};

static struct switch_attr rtl865x_vlan[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "vid",
		.description = "VLAN ID",
		.get = rtl865x_get_vid,
		.set = rtl865x_set_vid,
		.max = RTL865X_MAX_VLANS - 1,
	},
};

static const struct switch_dev_ops rtl865x_ops = {
	.attr_global = {
		.attr = rtl865x_globals,
		.n_attr = ARRAY_SIZE(rtl865x_globals),
	},
	.attr_port = {
		.attr = rtl865x_port,
		.n_attr = ARRAY_SIZE(rtl865x_port),
	},
	.attr_vlan = {
		.attr = rtl865x_vlan,
		.n_attr = ARRAY_SIZE(rtl865x_vlan),
	},
	.get_port_pvid = rtl865x_get_pvid,
	.set_port_pvid = rtl865x_set_pvid,
	.get_vlan_ports = rtl865x_get_vlan_ports,
	.set_vlan_ports = rtl865x_set_vlan_ports,
	.apply_config = rtl865x_apply_config,
	.reset_switch = rtl865x_reset_switch,
	.get_port_link = rtl865x_get_port_link,
	.get_port_stats = rtl865x_get_port_stats,
};

int rtl865x_switch_probe(struct rtl865x *rsw)
{
	int err;
	int i;

	if (soc_is_rtl8196c()) {
		rsw->num_ports = RTL865X_NUM_PHY_PORTS;
	} else {
		dev_err(rsw->parent, "unsupported soc for built-in switch\n");
		err = -EINVAL;
		goto err_out;
	}

	err = rtl865x_register_mdio(rsw);
	if (err) {
		dev_err(rsw->parent, "failed to register mdio bus of built-in switch\n");
		goto err_out;
	}

	rsw->swdev.name = "RTL865X built-in switch";
	rsw->swdev.ports = rsw->num_ports + 1; /* add one for cpu port */
	rsw->swdev.vlans = RTL865X_MAX_VLANS;
	rsw->swdev.ops = &rtl865x_ops;

	if (register_switch(&rsw->swdev, rsw->dev) < 0)
		goto err_remove_mdio;

	/* initialize vlan configurations */
	memset(rsw->vlan_members, 0, sizeof (rsw->vlan_members));
	memset(rsw->vlan_untag_members, 0, sizeof (rsw->vlan_untag_members));
	memset(rsw->pvid, 0, sizeof (rsw->pvid));

	for (i = 0; i < RTL865X_MAX_VLANS; i++)
		rsw->vlan_id[i] = i;

	rtl865x_reset(rsw);

	return 0;

err_remove_mdio:
	rtl865x_unregister_mdio(rsw);

err_out:
	return err;
}

int rtl865x_switch_remove(struct rtl865x *rsw)
{
	unregister_switch(&rsw->swdev);
	return 0;
}

