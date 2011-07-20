/*
 *  Driver for the built-in ethernet switch of the Atheros AR7240 SoC
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (c) 2010 Felix Fietkau <nbd@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 */

#include <linux/etherdevice.h>
#include <linux/list.h>
#include <linux/netdevice.h>
#include <linux/phy.h>
#include <linux/mii.h>
#include <linux/bitops.h>
#include <linux/switch.h>
#include "ag71xx.h"

#define BITM(_count)	(BIT(_count) - 1)
#define BITS(_shift, _count)	(BITM(_count) << _shift)

#define AR7240_REG_MASK_CTRL		0x00
#define AR7240_MASK_CTRL_REVISION_M	BITM(8)
#define AR7240_MASK_CTRL_VERSION_M	BITM(8)
#define AR7240_MASK_CTRL_VERSION_S	8
#define AR7240_MASK_CTRL_SOFT_RESET	BIT(31)

#define AR7240_REG_MAC_ADDR0		0x20
#define AR7240_REG_MAC_ADDR1		0x24

#define AR7240_REG_FLOOD_MASK		0x2c
#define AR7240_FLOOD_MASK_BROAD_TO_CPU	BIT(26)

#define AR7240_REG_GLOBAL_CTRL		0x30
#define AR7240_GLOBAL_CTRL_MTU_M	BITM(12)

#define AR7240_REG_VTU			0x0040
#define   AR7240_VTU_OP			BITM(3)
#define   AR7240_VTU_OP_NOOP		0x0
#define   AR7240_VTU_OP_FLUSH		0x1
#define   AR7240_VTU_OP_LOAD		0x2
#define   AR7240_VTU_OP_PURGE		0x3
#define   AR7240_VTU_OP_REMOVE_PORT	0x4
#define   AR7240_VTU_ACTIVE		BIT(3)
#define   AR7240_VTU_FULL		BIT(4)
#define   AR7240_VTU_PORT		BITS(8, 4)
#define   AR7240_VTU_PORT_S		8
#define   AR7240_VTU_VID		BITS(16, 12)
#define   AR7240_VTU_VID_S		16
#define   AR7240_VTU_PRIO		BITS(28, 3)
#define   AR7240_VTU_PRIO_S		28
#define   AR7240_VTU_PRIO_EN		BIT(31)

#define AR7240_REG_VTU_DATA		0x0044
#define   AR7240_VTUDATA_MEMBER		BITS(0, 10)
#define   AR7240_VTUDATA_VALID		BIT(11)

#define AR7240_REG_ATU			0x50
#define AR7240_ATU_FLUSH_ALL		0x1

#define AR7240_REG_AT_CTRL		0x5c
#define AR7240_AT_CTRL_AGE_TIME		BITS(0, 15)
#define AR7240_AT_CTRL_AGE_EN		BIT(17)
#define AR7240_AT_CTRL_LEARN_CHANGE	BIT(18)
#define AR7240_AT_CTRL_ARP_EN		BIT(20)

#define AR7240_REG_TAG_PRIORITY		0x70

#define AR7240_REG_SERVICE_TAG		0x74
#define AR7240_SERVICE_TAG_M		BITM(16)

#define AR7240_REG_CPU_PORT		0x78
#define AR7240_MIRROR_PORT_S		4
#define AR7240_CPU_PORT_EN		BIT(8)

#define AR7240_REG_MIB_FUNCTION0	0x80
#define AR7240_MIB_TIMER_M		BITM(16)
#define AR7240_MIB_AT_HALF_EN		BIT(16)
#define AR7240_MIB_BUSY			BIT(17)
#define AR7240_MIB_FUNC_S		24
#define AR7240_MIB_FUNC_NO_OP		0x0
#define AR7240_MIB_FUNC_FLUSH		0x1
#define AR7240_MIB_FUNC_CAPTURE		0x3

#define AR7240_REG_MDIO_CTRL		0x98
#define AR7240_MDIO_CTRL_DATA_M		BITM(16)
#define AR7240_MDIO_CTRL_REG_ADDR_S	16
#define AR7240_MDIO_CTRL_PHY_ADDR_S	21
#define AR7240_MDIO_CTRL_CMD_WRITE	0
#define AR7240_MDIO_CTRL_CMD_READ	BIT(27)
#define AR7240_MDIO_CTRL_MASTER_EN	BIT(30)
#define AR7240_MDIO_CTRL_BUSY		BIT(31)

#define AR7240_REG_PORT_BASE(_port)	(0x100 + (_port) * 0x100)

#define AR7240_REG_PORT_STATUS(_port)	(AR7240_REG_PORT_BASE((_port)) + 0x00)
#define AR7240_PORT_STATUS_SPEED_M	BITM(2)
#define AR7240_PORT_STATUS_SPEED_10	0
#define AR7240_PORT_STATUS_SPEED_100	1
#define AR7240_PORT_STATUS_SPEED_1000	2
#define AR7240_PORT_STATUS_TXMAC	BIT(2)
#define AR7240_PORT_STATUS_RXMAC	BIT(3)
#define AR7240_PORT_STATUS_TXFLOW	BIT(4)
#define AR7240_PORT_STATUS_RXFLOW	BIT(5)
#define AR7240_PORT_STATUS_DUPLEX	BIT(6)
#define AR7240_PORT_STATUS_LINK_UP	BIT(8)
#define AR7240_PORT_STATUS_LINK_AUTO	BIT(9)
#define AR7240_PORT_STATUS_LINK_PAUSE	BIT(10)

#define AR7240_REG_PORT_CTRL(_port)	(AR7240_REG_PORT_BASE((_port)) + 0x04)
#define AR7240_PORT_CTRL_STATE_M	BITM(3)
#define	AR7240_PORT_CTRL_STATE_DISABLED	0
#define AR7240_PORT_CTRL_STATE_BLOCK	1
#define AR7240_PORT_CTRL_STATE_LISTEN	2
#define AR7240_PORT_CTRL_STATE_LEARN	3
#define AR7240_PORT_CTRL_STATE_FORWARD	4
#define AR7240_PORT_CTRL_LEARN_LOCK	BIT(7)
#define AR7240_PORT_CTRL_VLAN_MODE_S	8
#define AR7240_PORT_CTRL_VLAN_MODE_KEEP	0
#define AR7240_PORT_CTRL_VLAN_MODE_STRIP 1
#define AR7240_PORT_CTRL_VLAN_MODE_ADD	2
#define AR7240_PORT_CTRL_VLAN_MODE_DOUBLE_TAG 3
#define AR7240_PORT_CTRL_IGMP_SNOOP	BIT(10)
#define AR7240_PORT_CTRL_HEADER		BIT(11)
#define AR7240_PORT_CTRL_MAC_LOOP	BIT(12)
#define AR7240_PORT_CTRL_SINGLE_VLAN	BIT(13)
#define AR7240_PORT_CTRL_LEARN		BIT(14)
#define AR7240_PORT_CTRL_DOUBLE_TAG	BIT(15)
#define AR7240_PORT_CTRL_MIRROR_TX	BIT(16)
#define AR7240_PORT_CTRL_MIRROR_RX	BIT(17)

#define AR7240_REG_PORT_VLAN(_port)	(AR7240_REG_PORT_BASE((_port)) + 0x08)

#define AR7240_PORT_VLAN_DEFAULT_ID_S	0
#define AR7240_PORT_VLAN_DEST_PORTS_S	16
#define AR7240_PORT_VLAN_MODE_S		30
#define AR7240_PORT_VLAN_MODE_PORT_ONLY	0
#define AR7240_PORT_VLAN_MODE_PORT_FALLBACK	1
#define AR7240_PORT_VLAN_MODE_VLAN_ONLY	2
#define AR7240_PORT_VLAN_MODE_SECURE	3


#define AR7240_REG_STATS_BASE(_port)	(0x20000 + (_port) * 0x100)

#define AR7240_STATS_RXBROAD		0x00
#define AR7240_STATS_RXPAUSE		0x04
#define AR7240_STATS_RXMULTI		0x08
#define AR7240_STATS_RXFCSERR		0x0c
#define AR7240_STATS_RXALIGNERR		0x10
#define AR7240_STATS_RXRUNT		0x14
#define AR7240_STATS_RXFRAGMENT		0x18
#define AR7240_STATS_RX64BYTE		0x1c
#define AR7240_STATS_RX128BYTE		0x20
#define AR7240_STATS_RX256BYTE		0x24
#define AR7240_STATS_RX512BYTE		0x28
#define AR7240_STATS_RX1024BYTE		0x2c
#define AR7240_STATS_RX1518BYTE		0x30
#define AR7240_STATS_RXMAXBYTE		0x34
#define AR7240_STATS_RXTOOLONG		0x38
#define AR7240_STATS_RXGOODBYTE		0x3c
#define AR7240_STATS_RXBADBYTE		0x44
#define AR7240_STATS_RXOVERFLOW		0x4c
#define AR7240_STATS_FILTERED		0x50
#define AR7240_STATS_TXBROAD		0x54
#define AR7240_STATS_TXPAUSE		0x58
#define AR7240_STATS_TXMULTI		0x5c
#define AR7240_STATS_TXUNDERRUN		0x60
#define AR7240_STATS_TX64BYTE		0x64
#define AR7240_STATS_TX128BYTE		0x68
#define AR7240_STATS_TX256BYTE		0x6c
#define AR7240_STATS_TX512BYTE		0x70
#define AR7240_STATS_TX1024BYTE		0x74
#define AR7240_STATS_TX1518BYTE		0x78
#define AR7240_STATS_TXMAXBYTE		0x7c
#define AR7240_STATS_TXOVERSIZE		0x80
#define AR7240_STATS_TXBYTE		0x84
#define AR7240_STATS_TXCOLLISION	0x8c
#define AR7240_STATS_TXABORTCOL		0x90
#define AR7240_STATS_TXMULTICOL		0x94
#define AR7240_STATS_TXSINGLECOL	0x98
#define AR7240_STATS_TXEXCDEFER		0x9c
#define AR7240_STATS_TXDEFER		0xa0
#define AR7240_STATS_TXLATECOL		0xa4

#define AR7240_PORT_CPU		0
#define AR7240_NUM_PORTS	6
#define AR7240_NUM_PHYS		5

#define AR7240_PHY_ID1		0x004d
#define AR7240_PHY_ID2		0xd041

#define AR7240_PORT_MASK(_port)		BIT((_port))
#define AR7240_PORT_MASK_ALL		BITM(AR7240_NUM_PORTS)
#define AR7240_PORT_MASK_BUT(_port)	(AR7240_PORT_MASK_ALL & ~BIT((_port)))

#define AR7240_MAX_VLANS	16

#define sw_to_ar7240(_dev) container_of(_dev, struct ar7240sw, swdev)

struct ar7240sw {
	struct mii_bus	*mii_bus;
	struct switch_dev swdev;
	bool vlan;
	u16 vlan_id[AR7240_MAX_VLANS];
	u8 vlan_table[AR7240_MAX_VLANS];
	u8 vlan_tagged;
	u16 pvid[AR7240_NUM_PORTS];
};

struct ar7240sw_hw_stat {
	char string[ETH_GSTRING_LEN];
	int sizeof_stat;
	int reg;
};

static DEFINE_MUTEX(reg_mutex);

static inline void ar7240sw_init(struct ar7240sw *as, struct mii_bus *mii)
{
	as->mii_bus = mii;
}

static inline u16 mk_phy_addr(u32 reg)
{
	return 0x17 & ((reg >> 4) | 0x10);
}

static inline u16 mk_phy_reg(u32 reg)
{
	return (reg << 1) & 0x1e;
}

static inline u16 mk_high_addr(u32 reg)
{
	return (reg >> 7) & 0x1ff;
}

static u32 __ar7240sw_reg_read(struct mii_bus *mii, u32 reg)
{
	unsigned long flags;
	u16 phy_addr;
	u16 phy_reg;
	u32 hi, lo;

	reg = (reg & 0xfffffffc) >> 2;
	phy_addr = mk_phy_addr(reg);
	phy_reg = mk_phy_reg(reg);

	local_irq_save(flags);
	ag71xx_mdio_mii_write(mii->priv, 0x1f, 0x10, mk_high_addr(reg));
	lo = (u32) ag71xx_mdio_mii_read(mii->priv, phy_addr, phy_reg);
	hi = (u32) ag71xx_mdio_mii_read(mii->priv, phy_addr, phy_reg + 1);
	local_irq_restore(flags);

	return (hi << 16) | lo;
}

static void __ar7240sw_reg_write(struct mii_bus *mii, u32 reg, u32 val)
{
	unsigned long flags;
	u16 phy_addr;
	u16 phy_reg;

	reg = (reg & 0xfffffffc) >> 2;
	phy_addr = mk_phy_addr(reg);
	phy_reg = mk_phy_reg(reg);

	local_irq_save(flags);
	ag71xx_mdio_mii_write(mii->priv, 0x1f, 0x10, mk_high_addr(reg));
	ag71xx_mdio_mii_write(mii->priv, phy_addr, phy_reg + 1, (val >> 16));
	ag71xx_mdio_mii_write(mii->priv, phy_addr, phy_reg, (val & 0xffff));
	local_irq_restore(flags);
}

static u32 ar7240sw_reg_read(struct mii_bus *mii, u32 reg_addr)
{
	u32 ret;

	mutex_lock(&reg_mutex);
	ret = __ar7240sw_reg_read(mii, reg_addr);
	mutex_unlock(&reg_mutex);

	return ret;
}

static void ar7240sw_reg_write(struct mii_bus *mii, u32 reg_addr, u32 reg_val)
{
	mutex_lock(&reg_mutex);
	__ar7240sw_reg_write(mii, reg_addr, reg_val);
	mutex_unlock(&reg_mutex);
}

static u32 ar7240sw_reg_rmw(struct mii_bus *mii, u32 reg, u32 mask, u32 val)
{
	u32 t;

	mutex_lock(&reg_mutex);
	t = __ar7240sw_reg_read(mii, reg);
	t &= ~mask;
	t |= val;
	__ar7240sw_reg_write(mii, reg, t);
	mutex_unlock(&reg_mutex);

	return t;
}

static void ar7240sw_reg_set(struct mii_bus *mii, u32 reg, u32 val)
{
	u32 t;

	mutex_lock(&reg_mutex);
	t = __ar7240sw_reg_read(mii, reg);
	t |= val;
	__ar7240sw_reg_write(mii, reg, t);
	mutex_unlock(&reg_mutex);
}

static int __ar7240sw_reg_wait(struct mii_bus *mii, u32 reg, u32 mask, u32 val,
			       unsigned timeout)
{
	int i;

	for (i = 0; i < timeout; i++) {
		u32 t;

		t = __ar7240sw_reg_read(mii, reg);
		if ((t & mask) == val)
			return 0;

		msleep(1);
	}

	return -ETIMEDOUT;
}

static int ar7240sw_reg_wait(struct mii_bus *mii, u32 reg, u32 mask, u32 val,
			     unsigned timeout)
{
	int ret;

	mutex_lock(&reg_mutex);
	ret = __ar7240sw_reg_wait(mii, reg, mask, val, timeout);
	mutex_unlock(&reg_mutex);
	return ret;
}

u16 ar7240sw_phy_read(struct mii_bus *mii, unsigned phy_addr,
		      unsigned reg_addr)
{
	u32 t, val = 0xffff;
	int err;

	if (phy_addr >= AR7240_NUM_PHYS)
		return 0xffff;

	mutex_lock(&reg_mutex);
	t = (reg_addr << AR7240_MDIO_CTRL_REG_ADDR_S) |
	    (phy_addr << AR7240_MDIO_CTRL_PHY_ADDR_S) |
	    AR7240_MDIO_CTRL_MASTER_EN |
	    AR7240_MDIO_CTRL_BUSY |
	    AR7240_MDIO_CTRL_CMD_READ;

	__ar7240sw_reg_write(mii, AR7240_REG_MDIO_CTRL, t);
	err = __ar7240sw_reg_wait(mii, AR7240_REG_MDIO_CTRL,
				  AR7240_MDIO_CTRL_BUSY, 0, 5);
	if (!err)
		val = __ar7240sw_reg_read(mii, AR7240_REG_MDIO_CTRL);
	mutex_unlock(&reg_mutex);

	return val & AR7240_MDIO_CTRL_DATA_M;
}

int ar7240sw_phy_write(struct mii_bus *mii, unsigned phy_addr,
		       unsigned reg_addr, u16 reg_val)
{
	u32 t;
	int ret;

	if (phy_addr >= AR7240_NUM_PHYS)
		return -EINVAL;

	mutex_lock(&reg_mutex);
	t = (phy_addr << AR7240_MDIO_CTRL_PHY_ADDR_S) |
	    (reg_addr << AR7240_MDIO_CTRL_REG_ADDR_S) |
	    AR7240_MDIO_CTRL_MASTER_EN |
	    AR7240_MDIO_CTRL_BUSY |
	    AR7240_MDIO_CTRL_CMD_WRITE |
	    reg_val;

	__ar7240sw_reg_write(mii, AR7240_REG_MDIO_CTRL, t);
	ret = __ar7240sw_reg_wait(mii, AR7240_REG_MDIO_CTRL,
				  AR7240_MDIO_CTRL_BUSY, 0, 5);
	mutex_unlock(&reg_mutex);

	return ret;
}

static int ar7240sw_capture_stats(struct ar7240sw *as)
{
	struct mii_bus *mii = as->mii_bus;
	int ret;

	/* Capture the hardware statistics for all ports */
	ar7240sw_reg_write(mii, AR7240_REG_MIB_FUNCTION0,
			   (AR7240_MIB_FUNC_CAPTURE << AR7240_MIB_FUNC_S));

	/* Wait for the capturing to complete. */
	ret = ar7240sw_reg_wait(mii, AR7240_REG_MIB_FUNCTION0,
				AR7240_MIB_BUSY, 0, 10);
	return ret;
}

static void ar7240sw_disable_port(struct ar7240sw *as, unsigned port)
{
	ar7240sw_reg_write(as->mii_bus, AR7240_REG_PORT_CTRL(port),
			   AR7240_PORT_CTRL_STATE_DISABLED);
}

static void ar7240sw_setup(struct ar7240sw *as)
{
	struct mii_bus *mii = as->mii_bus;

	/* Enable CPU port, and disable mirror port */
	ar7240sw_reg_write(mii, AR7240_REG_CPU_PORT,
			   AR7240_CPU_PORT_EN |
			   (15 << AR7240_MIRROR_PORT_S));

	/* Setup TAG priority mapping */
	ar7240sw_reg_write(mii, AR7240_REG_TAG_PRIORITY, 0xfa50);

	/* Enable ARP frame acknowledge, aging, MAC replacing */
	ar7240sw_reg_write(mii, AR7240_REG_AT_CTRL,
		0x2b /* 5 min age time */ |
		AR7240_AT_CTRL_AGE_EN |
		AR7240_AT_CTRL_ARP_EN |
		AR7240_AT_CTRL_LEARN_CHANGE);

	/* Enable Broadcast frames transmitted to the CPU */
	ar7240sw_reg_set(mii, AR7240_REG_FLOOD_MASK,
			 AR7240_FLOOD_MASK_BROAD_TO_CPU);

	/* setup MTU */
	ar7240sw_reg_rmw(mii, AR7240_REG_GLOBAL_CTRL, AR7240_GLOBAL_CTRL_MTU_M,
			 1536);

	/* setup Service TAG */
	ar7240sw_reg_rmw(mii, AR7240_REG_SERVICE_TAG, AR7240_SERVICE_TAG_M, 0);
}

static int ar7240sw_reset(struct ar7240sw *as)
{
	struct mii_bus *mii = as->mii_bus;
	int ret;
	int i;

	/* Set all ports to disabled state. */
	for (i = 0; i < AR7240_NUM_PORTS; i++)
		ar7240sw_disable_port(as, i);

	/* Wait for transmit queues to drain. */
	msleep(2);

	/* Reset the switch. */
	ar7240sw_reg_write(mii, AR7240_REG_MASK_CTRL,
			   AR7240_MASK_CTRL_SOFT_RESET);

	ret = ar7240sw_reg_wait(mii, AR7240_REG_MASK_CTRL,
				AR7240_MASK_CTRL_SOFT_RESET, 0, 1000);

	ar7240sw_setup(as);
	return ret;
}

static void ar7240sw_setup_port(struct ar7240sw *as, unsigned port, u8 portmask)
{
	struct mii_bus *mii = as->mii_bus;
	u32 ctrl;
	u32 dest_ports;
	u32 vlan;

	ctrl = AR7240_PORT_CTRL_STATE_FORWARD | AR7240_PORT_CTRL_LEARN |
		AR7240_PORT_CTRL_SINGLE_VLAN;

	if (port == AR7240_PORT_CPU) {
		ar7240sw_reg_write(mii, AR7240_REG_PORT_STATUS(port),
				   AR7240_PORT_STATUS_SPEED_1000 |
				   AR7240_PORT_STATUS_TXFLOW |
				   AR7240_PORT_STATUS_RXFLOW |
				   AR7240_PORT_STATUS_TXMAC |
				   AR7240_PORT_STATUS_RXMAC |
				   AR7240_PORT_STATUS_DUPLEX);
	} else {
		ar7240sw_reg_write(mii, AR7240_REG_PORT_STATUS(port),
				   AR7240_PORT_STATUS_LINK_AUTO);
	}

	/* Set the default VID for this port */
	if (as->vlan) {
		vlan = as->vlan_id[as->pvid[port]];
		vlan |= AR7240_PORT_VLAN_MODE_SECURE <<
			AR7240_PORT_VLAN_MODE_S;
	} else {
		vlan = port;
		vlan |= AR7240_PORT_VLAN_MODE_PORT_ONLY <<
			AR7240_PORT_VLAN_MODE_S;
	}

	if (as->vlan && (as->vlan_tagged & BIT(port))) {
		ctrl |= AR7240_PORT_CTRL_VLAN_MODE_ADD <<
			AR7240_PORT_CTRL_VLAN_MODE_S;
	} else {
		ctrl |= AR7240_PORT_CTRL_VLAN_MODE_STRIP <<
			AR7240_PORT_CTRL_VLAN_MODE_S;
	}

	if (!portmask) {
		if (port == AR7240_PORT_CPU)
			portmask = AR7240_PORT_MASK_BUT(AR7240_PORT_CPU);
		else
			portmask = AR7240_PORT_MASK(AR7240_PORT_CPU);
	}

	/* allow the port to talk to all other ports, but exclude its
	 * own ID to prevent frames from being reflected back to the
	 * port that they came from */
	dest_ports = AR7240_PORT_MASK_BUT(port);

	/* set default VID and and destination ports for this VLAN */
	vlan |= (portmask << AR7240_PORT_VLAN_DEST_PORTS_S);

	ar7240sw_reg_write(mii, AR7240_REG_PORT_CTRL(port), ctrl);
	ar7240sw_reg_write(mii, AR7240_REG_PORT_VLAN(port), vlan);
}

static int ar7240_set_addr(struct ar7240sw *as, u8 *addr)
{
	struct mii_bus *mii = as->mii_bus;
	u32 t;

	t = (addr[4] << 8) | addr[5];
	ar7240sw_reg_write(mii, AR7240_REG_MAC_ADDR0, t);

	t = (addr[0] << 24) | (addr[1] << 16) | (addr[2] << 8) | addr[3];
	ar7240sw_reg_write(mii, AR7240_REG_MAC_ADDR1, t);

	return 0;
}

static int
ar7240_set_vid(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct ar7240sw *as = sw_to_ar7240(dev);
	as->vlan_id[val->port_vlan] = val->value.i;
	return 0;
}

static int
ar7240_get_vid(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct ar7240sw *as = sw_to_ar7240(dev);
	val->value.i = as->vlan_id[val->port_vlan];
	return 0;
}

static int
ar7240_set_pvid(struct switch_dev *dev, int port, int vlan)
{
	struct ar7240sw *as = sw_to_ar7240(dev);

	/* make sure no invalid PVIDs get set */

	if (vlan >= dev->vlans)
		return -EINVAL;

	as->pvid[port] = vlan;
	return 0;
}

static int
ar7240_get_pvid(struct switch_dev *dev, int port, int *vlan)
{
	struct ar7240sw *as = sw_to_ar7240(dev);
	*vlan = as->pvid[port];
	return 0;
}

static int
ar7240_get_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct ar7240sw *as = sw_to_ar7240(dev);
	u8 ports = as->vlan_table[val->port_vlan];
	int i;

	val->len = 0;
	for (i = 0; i < AR7240_NUM_PORTS; i++) {
		struct switch_port *p;

		if (!(ports & (1 << i)))
			continue;

		p = &val->value.ports[val->len++];
		p->id = i;
		if (as->vlan_tagged & (1 << i))
			p->flags = (1 << SWITCH_PORT_FLAG_TAGGED);
		else
			p->flags = 0;
	}
	return 0;
}

static int
ar7240_set_ports(struct switch_dev *dev, struct switch_val *val)
{
	struct ar7240sw *as = sw_to_ar7240(dev);
	u8 *vt = &as->vlan_table[val->port_vlan];
	int i, j;

	*vt = 0;
	for (i = 0; i < val->len; i++) {
		struct switch_port *p = &val->value.ports[i];

		if (p->flags & (1 << SWITCH_PORT_FLAG_TAGGED))
			as->vlan_tagged |= (1 << p->id);
		else {
			as->vlan_tagged &= ~(1 << p->id);
			as->pvid[p->id] = val->port_vlan;

			/* make sure that an untagged port does not
			 * appear in other vlans */
			for (j = 0; j < AR7240_MAX_VLANS; j++) {
				if (j == val->port_vlan)
					continue;
				as->vlan_table[j] &= ~(1 << p->id);
			}
		}

		*vt |= 1 << p->id;
	}
	return 0;
}

static int
ar7240_set_vlan(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct ar7240sw *as = sw_to_ar7240(dev);
	as->vlan = !!val->value.i;
	return 0;
}

static int
ar7240_get_vlan(struct switch_dev *dev, const struct switch_attr *attr,
		struct switch_val *val)
{
	struct ar7240sw *as = sw_to_ar7240(dev);
	val->value.i = as->vlan;
	return 0;
}


static void
ar7240_vtu_op(struct ar7240sw *as, u32 op, u32 val)
{
	struct mii_bus *mii = as->mii_bus;

	if (ar7240sw_reg_wait(mii, AR7240_REG_VTU, AR7240_VTU_ACTIVE, 0, 5))
		return;

	if ((op & AR7240_VTU_OP) == AR7240_VTU_OP_LOAD) {
		val &= AR7240_VTUDATA_MEMBER;
		val |= AR7240_VTUDATA_VALID;
		ar7240sw_reg_write(mii, AR7240_REG_VTU_DATA, val);
	}
	op |= AR7240_VTU_ACTIVE;
	ar7240sw_reg_write(mii, AR7240_REG_VTU, op);
}

static int
ar7240_hw_apply(struct switch_dev *dev)
{
	struct ar7240sw *as = sw_to_ar7240(dev);
	u8 portmask[AR7240_NUM_PORTS];
	int i, j;

	/* flush all vlan translation unit entries */
	ar7240_vtu_op(as, AR7240_VTU_OP_FLUSH, 0);

	memset(portmask, 0, sizeof(portmask));
	if (as->vlan) {
		/* calculate the port destination masks and load vlans
		 * into the vlan translation unit */
		for (j = 0; j < AR7240_MAX_VLANS; j++) {
			u8 vp = as->vlan_table[j];

			if (!vp)
				continue;

			for (i = 0; i < AR7240_NUM_PORTS; i++) {
				u8 mask = (1 << i);
				if (vp & mask)
					portmask[i] |= vp & ~mask;
			}

			ar7240_vtu_op(as,
				AR7240_VTU_OP_LOAD |
				(as->vlan_id[j] << AR7240_VTU_VID_S),
				as->vlan_table[j]);
		}
	} else {
		/* vlan disabled:
		 * isolate all ports, but connect them to the cpu port */
		for (i = 0; i < AR7240_NUM_PORTS; i++) {
			if (i == AR7240_PORT_CPU)
				continue;

			portmask[i] = 1 << AR7240_PORT_CPU;
			portmask[AR7240_PORT_CPU] |= (1 << i);
		}
	}

	/* update the port destination mask registers and tag settings */
	for (i = 0; i < AR7240_NUM_PORTS; i++)
		ar7240sw_setup_port(as, i, portmask[i]);

	return 0;
}

static int
ar7240_reset_switch(struct switch_dev *dev)
{
	struct ar7240sw *as = sw_to_ar7240(dev);
	ar7240sw_reset(as);
	return 0;
}

static struct switch_attr ar7240_globals[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "enable_vlan",
		.description = "Enable VLAN mode",
		.set = ar7240_set_vlan,
		.get = ar7240_get_vlan,
		.max = 1
	},
};

static struct switch_attr ar7240_port[] = {
};

static struct switch_attr ar7240_vlan[] = {
	{
		.type = SWITCH_TYPE_INT,
		.name = "vid",
		.description = "VLAN ID",
		.set = ar7240_set_vid,
		.get = ar7240_get_vid,
		.max = 4094,
	},
};

static const struct switch_dev_ops ar7240_ops = {
	.attr_global = {
		.attr = ar7240_globals,
		.n_attr = ARRAY_SIZE(ar7240_globals),
	},
	.attr_port = {
		.attr = ar7240_port,
		.n_attr = ARRAY_SIZE(ar7240_port),
	},
	.attr_vlan = {
		.attr = ar7240_vlan,
		.n_attr = ARRAY_SIZE(ar7240_vlan),
	},
	.get_port_pvid = ar7240_get_pvid,
	.set_port_pvid = ar7240_set_pvid,
	.get_vlan_ports = ar7240_get_ports,
	.set_vlan_ports = ar7240_set_ports,
	.apply_config = ar7240_hw_apply,
	.reset_switch = ar7240_reset_switch,
};

static struct ar7240sw *ar7240_probe(struct ag71xx *ag)
{
	struct mii_bus *mii = ag->mii_bus;
	struct ar7240sw *as;
	struct switch_dev *swdev;
	u32 ctrl;
	u16 phy_id1;
	u16 phy_id2;
	u8 ver;
	int i;

	as = kzalloc(sizeof(*as), GFP_KERNEL);
	if (!as)
		return NULL;

	ar7240sw_init(as, mii);

	ctrl = ar7240sw_reg_read(mii, AR7240_REG_MASK_CTRL);

	ver = (ctrl >> AR7240_MASK_CTRL_VERSION_S) & AR7240_MASK_CTRL_VERSION_M;
	if (ver != 1) {
		pr_err("%s: unsupported chip, ctrl=%08x\n",
			ag->dev->name, ctrl);
		return NULL;
	}

	phy_id1 = ar7240sw_phy_read(mii, 0, MII_PHYSID1);
	phy_id2 = ar7240sw_phy_read(mii, 0, MII_PHYSID2);
	if (phy_id1 != AR7240_PHY_ID1 || phy_id2 != AR7240_PHY_ID2) {
		pr_err("%s: unknown phy id '%04x:%04x'\n",
		       ag->dev->name, phy_id1, phy_id2);
		return NULL;
	}

	swdev = &as->swdev;
	swdev->name = "AR7240 built-in switch";
	swdev->ports = AR7240_NUM_PORTS;
	swdev->cpu_port = AR7240_PORT_CPU;
	swdev->vlans = AR7240_MAX_VLANS;
	swdev->ops = &ar7240_ops;

	if (register_switch(&as->swdev, ag->dev) < 0) {
		kfree(as);
		return NULL;
	}

	pr_info("%s: Found an AR7240 built-in switch\n", ag->dev->name);

	/* initialize defaults */
	for (i = 0; i < AR7240_MAX_VLANS; i++)
		as->vlan_id[i] = i;

	as->vlan_table[0] = AR7240_PORT_MASK_ALL;

	return as;
}

static void link_function(struct work_struct *work) {
	struct ag71xx *ag = container_of(work, struct ag71xx, link_work.work);
	unsigned long flags;
	int i;
	int status = 0;

	for (i = 0; i < 4; i++) {
		int link = ar7240sw_phy_read(ag->mii_bus, i, MII_BMSR);
		if(link & BMSR_LSTATUS) {
			status = 1;
			break;
		}
	}

	spin_lock_irqsave(&ag->lock, flags);
	if(status != ag->link) {
		ag->link = status;
		ag71xx_link_adjust(ag);
	}
	spin_unlock_irqrestore(&ag->lock, flags);

	schedule_delayed_work(&ag->link_work, HZ / 2);
}

void ag71xx_ar7240_start(struct ag71xx *ag)
{
	struct ar7240sw *as = ag->phy_priv;

	ar7240sw_reset(as);

	ag->speed = SPEED_1000;
	ag->duplex = 1;

	ar7240_set_addr(as, ag->dev->dev_addr);
	ar7240_hw_apply(&as->swdev);

	schedule_delayed_work(&ag->link_work, HZ / 10);
}

void ag71xx_ar7240_stop(struct ag71xx *ag)
{
	cancel_delayed_work_sync(&ag->link_work);
}

int __devinit ag71xx_ar7240_init(struct ag71xx *ag)
{
	struct ar7240sw *as;

	as = ar7240_probe(ag);
	if (!as)
		return -ENODEV;

	ag->phy_priv = as;
	ar7240sw_reset(as);

	INIT_DELAYED_WORK(&ag->link_work, link_function);

	return 0;
}

void ag71xx_ar7240_cleanup(struct ag71xx *ag)
{
	struct ar7240sw *as = ag->phy_priv;

	if (!as)
		return;

	unregister_switch(&as->swdev);
	kfree(as);
	ag->phy_priv = NULL;
}
