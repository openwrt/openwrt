/*
 *  DSA driver for the built-in ethernet switch of the Atheros AR7240 SoC
 *  Copyright (c) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This file was based on:
 *    net/dsa/mv88e6060.c - Driver for Marvell 88e6060 switch chips
 *    Copyright (c) 2008 Marvell Semiconductor
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

#include "dsa_priv.h"

#define BITM(_count)	(BIT(_count) - 1)

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

#define AR7240_REG_AT_CTRL		0x5c
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

struct ar7240sw {
	struct mii_bus	*mii_bus;
	struct mutex	reg_mutex;
	struct mutex	stats_mutex;
};

struct ar7240sw_hw_stat {
	char string[ETH_GSTRING_LEN];
	int sizeof_stat;
	int reg;
};

static inline struct ar7240sw *dsa_to_ar7240sw(struct dsa_switch *ds)
{
	return (struct ar7240sw *)(ds + 1);
}

static inline void ar7240sw_init(struct ar7240sw *as, struct mii_bus *mii)
{
	as->mii_bus = mii;
	mutex_init(&as->reg_mutex);
	mutex_init(&as->stats_mutex);
}

static inline u16 mk_phy_addr(u32 reg)
{
	return (0x17 & ((reg >> 4) | 0x10));
}

static inline u16 mk_phy_reg(u32 reg)
{
	return ((reg << 1) & 0x1e);
}

static inline u16 mk_high_addr(u32 reg)
{
	return ((reg >> 7) & 0x1ff);
}

static u32 __ar7240sw_reg_read(struct ar7240sw *as, u32 reg)
{
	struct mii_bus *mii = as->mii_bus;
	u16 phy_addr;
	u16 phy_reg;
	u32 hi, lo;

	reg = (reg & 0xfffffffc) >> 2;

	mdiobus_write(mii, 0x1f, 0x10, mk_high_addr(reg));

	phy_addr = mk_phy_addr(reg);
	phy_reg = mk_phy_reg(reg);

	lo = (u32) mdiobus_read(mii, phy_addr, phy_reg);
	hi = (u32) mdiobus_read(mii, phy_addr, phy_reg + 1);

	return ((hi << 16) | lo);
}

static void __ar7240sw_reg_write(struct ar7240sw *as, u32 reg, u32 val)
{
	struct mii_bus *mii = as->mii_bus;
	u16 phy_addr;
	u16 phy_reg;

	reg = (reg & 0xfffffffc) >> 2;

	mdiobus_write(mii, 0x1f, 0x10, mk_high_addr(reg));

	phy_addr = mk_phy_addr(reg);
	phy_reg = mk_phy_reg(reg);

	mdiobus_write(mii, phy_addr, phy_reg + 1, (val >> 16));
	mdiobus_write(mii, phy_addr, phy_reg, (val & 0xffff));
}

static u32 ar7240sw_reg_read(struct ar7240sw *as, u32 reg_addr)
{
	u32 ret;

	mutex_lock(&as->reg_mutex);
	ret = __ar7240sw_reg_read(as, reg_addr);
	mutex_unlock(&as->reg_mutex);

	return ret;
}

static void ar7240sw_reg_write(struct ar7240sw *as, u32 reg_addr, u32 reg_val)
{
	mutex_lock(&as->reg_mutex);
	__ar7240sw_reg_write(as, reg_addr, reg_val);
	mutex_unlock(&as->reg_mutex);
}

static u32 ar7240sw_reg_rmw(struct ar7240sw *as, u32 reg, u32 mask, u32 val)
{
	u32 t;

	mutex_lock(&as->reg_mutex);
	t = __ar7240sw_reg_read(as, reg);
	t &= ~mask;
	t |= val;
	__ar7240sw_reg_write(as, reg, t);
	mutex_unlock(&as->reg_mutex);

	return t;
}

static void ar7240sw_reg_set(struct ar7240sw *as, u32 reg, u32 val)
{
	u32 t;

	mutex_lock(&as->reg_mutex);
	t = __ar7240sw_reg_read(as, reg);
	t |= val;
	__ar7240sw_reg_write(as, reg, t);
	mutex_unlock(&as->reg_mutex);
}

static int ar7240sw_reg_wait(struct ar7240sw *as, u32 reg, u32 mask, u32 val,
			     unsigned timeout)
{
	int i;

	for (i = 0; i < timeout; i++) {
		u32 t;

		t = ar7240sw_reg_read(as, reg);
		if ((t & mask) == val)
			return 0;

		msleep(1);
	}

	return -ETIMEDOUT;
}

static u16 ar7240sw_phy_read(struct ar7240sw *as, unsigned phy_addr,
			     unsigned reg_addr)
{
	u32 t;
	int err;

	if (phy_addr >= AR7240_NUM_PHYS)
		return 0xffff;

	t = (reg_addr << AR7240_MDIO_CTRL_REG_ADDR_S) |
	    (phy_addr << AR7240_MDIO_CTRL_PHY_ADDR_S) |
	    AR7240_MDIO_CTRL_MASTER_EN |
	    AR7240_MDIO_CTRL_BUSY |
	    AR7240_MDIO_CTRL_CMD_READ;

	ar7240sw_reg_write(as, AR7240_REG_MDIO_CTRL, t);
	err = ar7240sw_reg_wait(as, AR7240_REG_MDIO_CTRL,
				AR7240_MDIO_CTRL_BUSY, 0, 5);
	if (err)
		return 0xffff;

	t = ar7240sw_reg_read(as, AR7240_REG_MDIO_CTRL);
	return (t & AR7240_MDIO_CTRL_DATA_M);
}

static int ar7240sw_phy_write(struct ar7240sw *as, unsigned phy_addr,
			      unsigned reg_addr, u16 reg_val)
{
	u32 t;
	int ret;

	if (phy_addr >= AR7240_NUM_PHYS)
		return -EINVAL;

	t = (phy_addr << AR7240_MDIO_CTRL_PHY_ADDR_S) |
	    (reg_addr << AR7240_MDIO_CTRL_REG_ADDR_S) |
	    AR7240_MDIO_CTRL_MASTER_EN |
	    AR7240_MDIO_CTRL_BUSY |
	    AR7240_MDIO_CTRL_CMD_WRITE |
	    reg_val;

	ar7240sw_reg_write(as, AR7240_REG_MDIO_CTRL, t);
	ret = ar7240sw_reg_wait(as, AR7240_REG_MDIO_CTRL,
				AR7240_MDIO_CTRL_BUSY, 0, 5);
	return ret;
}

static int ar7240sw_capture_stats(struct ar7240sw *as)
{
	int ret;

	/* Capture the hardware statistics for all ports */
	ar7240sw_reg_write(as, AR7240_REG_MIB_FUNCTION0,
			   (AR7240_MIB_FUNC_CAPTURE << AR7240_MIB_FUNC_S));

	/* Wait for the capturing to complete. */
	ret = ar7240sw_reg_wait(as, AR7240_REG_MIB_FUNCTION0,
				AR7240_MIB_BUSY, 0, 10);
	return ret;
}

static void ar7240sw_disable_port(struct ar7240sw *as, unsigned port)
{
	ar7240sw_reg_write(as, AR7240_REG_PORT_CTRL(port),
			   AR7240_PORT_CTRL_STATE_DISABLED);
}

static int ar7240sw_reset(struct ar7240sw *as)
{
	int ret;
	int i;

	/* Set all ports to disabled state. */
	for (i = 0; i < AR7240_NUM_PORTS; i++)
		ar7240sw_disable_port(as, i);

	/* Wait for transmit queues to drain. */
	msleep(2);

	/* Reset the switch. */
	ar7240sw_reg_write(as, AR7240_REG_MASK_CTRL,
			   AR7240_MASK_CTRL_SOFT_RESET);

	ret = ar7240sw_reg_wait(as, AR7240_REG_MASK_CTRL,
			        AR7240_MASK_CTRL_SOFT_RESET, 0, 1000);
	return ret;
}

static void ar7240sw_setup(struct ar7240sw *as)
{
	/* Enable CPU port, and disable mirror port */
	ar7240sw_reg_write(as, AR7240_REG_CPU_PORT,
			   AR7240_CPU_PORT_EN |
			   (15 << AR7240_MIRROR_PORT_S));

	/* Setup TAG priority mapping */
	ar7240sw_reg_write(as, AR7240_REG_TAG_PRIORITY, 0xfa50);

	/* Enable ARP frame acknowledge */
	ar7240sw_reg_set(as, AR7240_REG_AT_CTRL, AR7240_AT_CTRL_ARP_EN);

	/* Enable Broadcast frames transmitted to the CPU */
	ar7240sw_reg_set(as, AR7240_REG_FLOOD_MASK,
			 AR7240_FLOOD_MASK_BROAD_TO_CPU);

	/* setup MTU */
	ar7240sw_reg_rmw(as, AR7240_REG_GLOBAL_CTRL, AR7240_GLOBAL_CTRL_MTU_M,
			 1536);

	/* setup Service TAG */
	ar7240sw_reg_rmw(as, AR7240_REG_SERVICE_TAG, AR7240_SERVICE_TAG_M,
			 ETH_P_QINQ);
}

static void ar7240sw_setup_port(struct ar7240sw *as, unsigned port)
{
	u32 ctrl;
	u32 dest_ports;
	u32 vlan;

	ctrl = AR7240_PORT_CTRL_STATE_FORWARD;

	if (port == AR7240_PORT_CPU) {
		ar7240sw_reg_write(as, AR7240_REG_PORT_STATUS(port),
				   AR7240_PORT_STATUS_SPEED_1000 |
				   AR7240_PORT_STATUS_TXFLOW |
				   AR7240_PORT_STATUS_RXFLOW |
				   AR7240_PORT_STATUS_TXMAC |
				   AR7240_PORT_STATUS_RXMAC |
				   AR7240_PORT_STATUS_DUPLEX);

		/* allow the CPU port to talk to each of the 'real' ports */
		dest_ports = AR7240_PORT_MASK_BUT(port);

		/* remove service tag from ingress frames */
		ctrl |= AR7240_PORT_CTRL_DOUBLE_TAG;
	} else {
		ar7240sw_reg_write(as, AR7240_REG_PORT_STATUS(port),
				   AR7240_PORT_STATUS_LINK_AUTO);

		/*
		 * allow each of the 'real' ports to only talk to the CPU
		 * port.
		 */
		dest_ports = AR7240_PORT_MASK(port) |
			     AR7240_PORT_MASK(AR7240_PORT_CPU);

		/* add service tag to egress frames */
		ctrl |= (AR7240_PORT_CTRL_VLAN_MODE_DOUBLE_TAG <<
			 AR7240_PORT_CTRL_VLAN_MODE_S);
	}

	/* set default VID and and destination ports for this VLAN */
	vlan = port;
	vlan |= (dest_ports << AR7240_PORT_VLAN_DEST_PORTS_S);

	ar7240sw_reg_write(as, AR7240_REG_PORT_CTRL(port), ctrl);
	ar7240sw_reg_write(as, AR7240_REG_PORT_VLAN(port), vlan);
}

static char *ar7240_dsa_probe(struct mii_bus *mii, int sw_addr)
{
	struct ar7240sw as;
	u32 ctrl;
	u16 phy_id1;
	u16 phy_id2;
	u8 ver;

	ar7240sw_init(&as, mii);

	ctrl = ar7240sw_reg_read(&as, AR7240_REG_MASK_CTRL);

	ver = (ctrl >> AR7240_MASK_CTRL_VERSION_S) & AR7240_MASK_CTRL_VERSION_M;
	if (ver != 1) {
		pr_err("ar7240_dsa: unsupported chip, ctrl=%08x\n", ctrl);
		return NULL;
	}

	phy_id1 = ar7240sw_phy_read(&as, 0, MII_PHYSID1);
	phy_id2 = ar7240sw_phy_read(&as, 0, MII_PHYSID2);
	if (phy_id1 != AR7240_PHY_ID1 || phy_id2 != AR7240_PHY_ID2) {
		pr_err("ar7240_dsa: unknown phy id '%04x:%04x'\n",
		       phy_id1, phy_id2);
		return NULL;
	}

	return "Atheros AR7240 built-in";
}

static int ar7240_dsa_setup(struct dsa_switch *ds)
{
	struct ar7240sw *as = dsa_to_ar7240sw(ds);
	int i;
	int ret;

	ar7240sw_init(as, ds->master_mii_bus);

	ret = ar7240sw_reset(as);
	if (ret)
		return ret;

	ar7240sw_setup(as);

	for (i = 0; i < AR7240_NUM_PORTS; i++) {
		if (dsa_is_cpu_port(ds, i) || (ds->phys_port_mask & (1 << i)))
			ar7240sw_setup_port(as, i);
		else
			ar7240sw_disable_port(as, i);
	}

	return 0;
}

static int ar7240_dsa_set_addr(struct dsa_switch *ds, u8 *addr)
{
	struct ar7240sw *as = dsa_to_ar7240sw(ds);
	u32 t;

	t = (addr[4] << 8) | addr[5];
	ar7240sw_reg_write(as, AR7240_REG_MAC_ADDR0, t);

	t = (addr[0] << 24) | (addr[1] << 16) | (addr[2] << 8) | addr[3];
	ar7240sw_reg_write(as, AR7240_REG_MAC_ADDR0, t);

	return 0;
}

static int ar7240_iort_to_phy_addr(int port)
{
	if (port > 0 && port < AR7240_NUM_PORTS)
		return port - 1;

	return -EINVAL;
}

static int ar7240_dsa_phy_read(struct dsa_switch *ds, int port, int regnum)
{
	struct ar7240sw *as = dsa_to_ar7240sw(ds);
	int phy_addr;

	phy_addr = ar7240_iort_to_phy_addr(port);
	if (phy_addr < 0)
		return 0xffff;

	return ar7240sw_phy_read(as, phy_addr, regnum);
}

static int ar7240_dsa_phy_write(struct dsa_switch *ds, int port, int regnum,
				u16 val)
{
	struct ar7240sw *as = dsa_to_ar7240sw(ds);
	int phy_addr;

	phy_addr = ar7240_iort_to_phy_addr(port);
	if (phy_addr < 0)
		return 0xffff;

	return ar7240sw_phy_write(as, phy_addr, regnum, val);
}

static const char *ar7240sw_speed_str(unsigned speed)
{
	switch (speed) {
	case AR7240_PORT_STATUS_SPEED_10:
		return "10";
	case AR7240_PORT_STATUS_SPEED_100:
		return "100";
	case AR7240_PORT_STATUS_SPEED_1000:
		return "1000";
	}

	return "????";
}

static void ar7240_dsa_poll_link(struct dsa_switch *ds)
{
	struct ar7240sw *as = dsa_to_ar7240sw(ds);
	int i;

	for (i = 0; i < DSA_MAX_PORTS; i++) {
		struct net_device *dev;
		u32 status;
		int link;
		unsigned speed;
		int duplex;

		dev = ds->ports[i];
		if (dev == NULL)
			continue;

		link = 0;
		if (dev->flags & IFF_UP) {
			status = ar7240sw_reg_read(as,
						   AR7240_REG_PORT_STATUS(i));
			link = !!(status & AR7240_PORT_STATUS_LINK_UP);
		}

		if (!link) {
			if (netif_carrier_ok(dev)) {
				pr_info("%s: link down\n", dev->name);
				netif_carrier_off(dev);
			}
			continue;
		}

		speed = (status & AR7240_PORT_STATUS_SPEED_M);
		duplex = (status & AR7240_PORT_STATUS_DUPLEX) ? 1 : 0;
		if (!netif_carrier_ok(dev)) {
			pr_info("%s: link up, %sMb/s, %s duplex",
				dev->name,
				ar7240sw_speed_str(speed),
				duplex ? "full" : "half");
			netif_carrier_on(dev);
		}
	}
}

static const struct ar7240sw_hw_stat ar7240_hw_stats[] = {
	{ "rx_broadcast"	, 4, AR7240_STATS_RXBROAD, },
	{ "rx_pause"		, 4, AR7240_STATS_RXPAUSE, },
	{ "rx_multicast"	, 4, AR7240_STATS_RXMULTI, },
	{ "rx_fcs_error"	, 4, AR7240_STATS_RXFCSERR, },
	{ "rx_align_error"	, 4, AR7240_STATS_RXALIGNERR, },
	{ "rx_undersize"	, 4, AR7240_STATS_RXRUNT, },
	{ "rx_fragments"	, 4, AR7240_STATS_RXFRAGMENT, },
	{ "rx_64bytes"		, 4, AR7240_STATS_RX64BYTE, },
	{ "rx_65_127bytes"	, 4, AR7240_STATS_RX128BYTE, },
	{ "rx_128_255bytes"	, 4, AR7240_STATS_RX256BYTE, },
	{ "rx_256_511bytes"	, 4, AR7240_STATS_RX512BYTE, },
	{ "rx_512_1023bytes"	, 4, AR7240_STATS_RX1024BYTE, },
	{ "rx_1024_1518bytes"	, 4, AR7240_STATS_RX1518BYTE, },
	{ "rx_1519_max_bytes"	, 4, AR7240_STATS_RXMAXBYTE, },
	{ "rx_oversize"		, 4, AR7240_STATS_RXTOOLONG, },
	{ "rx_good_bytes"	, 8, AR7240_STATS_RXGOODBYTE, },
	{ "rx_bad_bytes"	, 8, AR7240_STATS_RXBADBYTE, },
	{ "rx_overflow"		, 4, AR7240_STATS_RXOVERFLOW, },
	{ "filtered"		, 4, AR7240_STATS_FILTERED, },
	{ "tx_broadcast"	, 4, AR7240_STATS_TXBROAD, },
	{ "tx_pause"		, 4, AR7240_STATS_TXPAUSE, },
	{ "tx_multicast"	, 4, AR7240_STATS_TXMULTI, },
	{ "tx_underrun"		, 4, AR7240_STATS_TXUNDERRUN, },
	{ "tx_64bytes"		, 4, AR7240_STATS_TX64BYTE, },
	{ "tx_65_127bytes"	, 4, AR7240_STATS_TX128BYTE, },
	{ "tx_128_255bytes"	, 4, AR7240_STATS_TX256BYTE, },
	{ "tx_256_511bytes"	, 4, AR7240_STATS_TX512BYTE, },
	{ "tx_512_1023bytes"	, 4, AR7240_STATS_TX1024BYTE, },
	{ "tx_1024_1518bytes"	, 4, AR7240_STATS_TX1518BYTE, },
	{ "tx_1519_max_bytes"	, 4, AR7240_STATS_TXMAXBYTE, },
	{ "tx_oversize"		, 4, AR7240_STATS_TXOVERSIZE, },
	{ "tx_bytes"		, 8, AR7240_STATS_TXBYTE, },
	{ "tx_collisions"	, 4, AR7240_STATS_TXCOLLISION, },
	{ "tx_abort_collisions"	, 4, AR7240_STATS_TXABORTCOL, },
	{ "tx_multi_collisions"	, 4, AR7240_STATS_TXMULTICOL, },
	{ "tx_single_collisions", 4, AR7240_STATS_TXSINGLECOL, },
	{ "tx_excessive_deferred", 4, AR7240_STATS_TXEXCDEFER, },
	{ "tx_deferred"		, 4, AR7240_STATS_TXDEFER, },
	{ "tx_late_collisions"	, 4, AR7240_STATS_TXLATECOL, },
};

static void ar7240_dsa_get_strings(struct dsa_switch *ds, int port,
				   uint8_t *data)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ar7240_hw_stats); i++) {
		memcpy(data + i * ETH_GSTRING_LEN,
		       ar7240_hw_stats[i].string, ETH_GSTRING_LEN);
	}
}

static void ar7240_dsa_get_ethtool_stats(struct dsa_switch *ds, int port,
					 uint64_t *data)
{
	struct ar7240sw *as = dsa_to_ar7240sw(ds);
	int err;
	int i;

	mutex_lock(&as->stats_mutex);

	err = ar7240sw_capture_stats(as);
	if (err)
		goto unlock;

	for (i = 0; i < ARRAY_SIZE(ar7240_hw_stats); i++) {
		const struct ar7240sw_hw_stat *s = &ar7240_hw_stats[i];
		u32 reg = AR7240_REG_STATS_BASE(port);
		u32 low;
		u32 high;

		low = ar7240sw_reg_read(as, reg + s->reg);
		if (s->sizeof_stat == 8)
			high = ar7240sw_reg_read(as, reg + s->reg);
		else
			high = 0;

		data[i] = (((u64) high) << 32) | low;
	}

 unlock:
	mutex_unlock(&as->stats_mutex);
}

static int ar7240_dsa_get_sset_count(struct dsa_switch *ds)
{
	return ARRAY_SIZE(ar7240_hw_stats);
}

static struct dsa_switch_driver ar7240_dsa_driver = {
	.tag_protocol		= htons(ETH_P_QINQ),
	.priv_size		= sizeof(struct ar7240sw),
	.probe			= ar7240_dsa_probe,
	.setup			= ar7240_dsa_setup,
	.set_addr		= ar7240_dsa_set_addr,
	.phy_read		= ar7240_dsa_phy_read,
	.phy_write		= ar7240_dsa_phy_write,
	.poll_link		= ar7240_dsa_poll_link,
	.get_strings		= ar7240_dsa_get_strings,
	.get_ethtool_stats	= ar7240_dsa_get_ethtool_stats,
	.get_sset_count		= ar7240_dsa_get_sset_count,
};

int __init dsa_ar7240_init(void)
{
	register_switch_driver(&ar7240_dsa_driver);
	return 0;
}
module_init(dsa_ar7240_init);

void __exit dsa_ar7240_cleanup(void)
{
	unregister_switch_driver(&ar7240_dsa_driver);
}
module_exit(dsa_ar7240_cleanup);
