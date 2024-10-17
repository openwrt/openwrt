// SPDX-License-Identifier: GPL-2.0-only
/*
 * Airoha AN8855 DSA Switch driver
 * Copyright (C) 2023 Min Yao <min.yao@airoha.com>
 */
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/iopoll.h>
#include <linux/mdio.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/phylink.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/reset.h>
#include <linux/gpio/consumer.h>
#include <net/dsa.h>
#include <linux/of_address.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/version.h>

#include "an8855.h"
#include "an8855_nl.h"
#include "an8855_phy.h"

/* AN8855 driver version */
#define ARHT_AN8855_DSA_DRIVER_VER	"1.0.2"

#define ARHT_CHIP_NAME                  "an8855"
#define ARHT_PROC_DIR                   "air_sw"
#define ARHT_PROC_NODE_DEVICE           "device"

struct proc_dir_entry *proc_an8855_dsa_dir;

static struct an8855_pcs *pcs_to_an8855_pcs(struct phylink_pcs *pcs)
{
	return container_of(pcs, struct an8855_pcs, pcs);
}

/* T830 AN8855 Reference Board */
static const struct an8855_led_cfg led_cfg[] = {
/*************************************************************************
 * Enable, LED idx, LED Polarity, LED ON event,  LED Blink event  LED Freq
 *************************************************************************
 */
	/* GPIO0 */
	{1, P4_LED0, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO1 */
	{1, P4_LED1, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO2 */
	{1, P0_LED0, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO3 */
	{1, P0_LED1, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO4 */
	{1, P1_LED0, LED_LOW,  LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO5 */
	{1, P1_LED1, LED_LOW,  LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO6 */
	{0, PHY_LED_MAX, LED_LOW,  LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO7 */
	{0, PHY_LED_MAX, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO8 */
	{0, PHY_LED_MAX, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO9 */
	{1, P2_LED0, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO10 */
	{1, P2_LED1, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO11 */
	{1, P3_LED0, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO12 */
	{1, P3_LED1, LED_HIGH,  LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO13 */
	{0, PHY_LED_MAX, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO14 */
	{0, PHY_LED_MAX, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO15 */
	{0, PHY_LED_MAX, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO16 */
	{0, PHY_LED_MAX, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO17 */
	{0, PHY_LED_MAX, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO18 */
	{0, PHY_LED_MAX, LED_HIGH, LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO19 */
	{0, PHY_LED_MAX, LED_LOW,  LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
	/* GPIO20 */
	{0, PHY_LED_MAX, LED_LOW,  LED_ON_EVENT, LED_BLK_EVENT, LED_FREQ},
};

/* String, offset, and register size in bytes if different from 4 bytes */
static const struct an8855_mib_desc an8855_mib[] = {
	MIB_DESC(1, 0x00, "TxDrop"),
	MIB_DESC(1, 0x04, "TxCrcErr"),
	MIB_DESC(1, 0x08, "TxUnicast"),
	MIB_DESC(1, 0x0c, "TxMulticast"),
	MIB_DESC(1, 0x10, "TxBroadcast"),
	MIB_DESC(1, 0x14, "TxCollision"),
	MIB_DESC(1, 0x18, "TxSingleCollision"),
	MIB_DESC(1, 0x1c, "TxMultipleCollision"),
	MIB_DESC(1, 0x20, "TxDeferred"),
	MIB_DESC(1, 0x24, "TxLateCollision"),
	MIB_DESC(1, 0x28, "TxExcessiveCollistion"),
	MIB_DESC(1, 0x2c, "TxPause"),
	MIB_DESC(1, 0x30, "TxPktSz64"),
	MIB_DESC(1, 0x34, "TxPktSz65To127"),
	MIB_DESC(1, 0x38, "TxPktSz128To255"),
	MIB_DESC(1, 0x3c, "TxPktSz256To511"),
	MIB_DESC(1, 0x40, "TxPktSz512To1023"),
	MIB_DESC(1, 0x44, "TxPktSz1024To1518"),
	MIB_DESC(1, 0x48, "TxPktSz1519ToMax"),
	MIB_DESC(2, 0x4c, "TxBytes"),
	MIB_DESC(1, 0x54, "TxOversizeDrop"),
	MIB_DESC(2, 0x58, "TxBadPktBytes"),
	MIB_DESC(1, 0x80, "RxDrop"),
	MIB_DESC(1, 0x84, "RxFiltering"),
	MIB_DESC(1, 0x88, "RxUnicast"),
	MIB_DESC(1, 0x8c, "RxMulticast"),
	MIB_DESC(1, 0x90, "RxBroadcast"),
	MIB_DESC(1, 0x94, "RxAlignErr"),
	MIB_DESC(1, 0x98, "RxCrcErr"),
	MIB_DESC(1, 0x9c, "RxUnderSizeErr"),
	MIB_DESC(1, 0xa0, "RxFragErr"),
	MIB_DESC(1, 0xa4, "RxOverSzErr"),
	MIB_DESC(1, 0xa8, "RxJabberErr"),
	MIB_DESC(1, 0xac, "RxPause"),
	MIB_DESC(1, 0xb0, "RxPktSz64"),
	MIB_DESC(1, 0xb4, "RxPktSz65To127"),
	MIB_DESC(1, 0xb8, "RxPktSz128To255"),
	MIB_DESC(1, 0xbc, "RxPktSz256To511"),
	MIB_DESC(1, 0xc0, "RxPktSz512To1023"),
	MIB_DESC(1, 0xc4, "RxPktSz1024To1518"),
	MIB_DESC(1, 0xc8, "RxPktSz1519ToMax"),
	MIB_DESC(2, 0xcc, "RxBytes"),
	MIB_DESC(1, 0xd4, "RxCtrlDrop"),
	MIB_DESC(1, 0xd8, "RxIngressDrop"),
	MIB_DESC(1, 0xdc, "RxArlDrop"),
	MIB_DESC(1, 0xe0, "FlowControlDrop"),
	MIB_DESC(1, 0xe4, "WredDrop"),
	MIB_DESC(1, 0xe8, "MirrorDrop"),
	MIB_DESC(2, 0xec, "RxBadPktBytes"),
	MIB_DESC(1, 0xf4, "RxsFlowSamplingPktDrop"),
	MIB_DESC(1, 0xf8, "RxsFlowTotalPktDrop"),
	MIB_DESC(1, 0xfc, "PortControlDrop"),
};

static int
an8855_mii_write(struct an8855_priv *priv, u32 reg, u32 val)
{
	struct mii_bus *bus = priv->bus;
	int ret = 0;

	ret = bus->write(bus, priv->phy_base, 0x1f, 0x4);
	ret = bus->write(bus, priv->phy_base, 0x10, 0);

	ret = bus->write(bus, priv->phy_base, 0x11, ((reg >> 16) & 0xFFFF));
	ret = bus->write(bus, priv->phy_base, 0x12, (reg & 0xFFFF));

	ret = bus->write(bus, priv->phy_base, 0x13, ((val >> 16) & 0xFFFF));
	ret = bus->write(bus, priv->phy_base, 0x14, (val & 0xFFFF));

	ret = bus->write(bus, priv->phy_base, 0x1f, 0);

	if (ret < 0) {
		dev_err(&bus->dev, "failed to write an8855 register\n");
		return ret;
	}

	return ret;
}

static u32
an8855_mii_read(struct an8855_priv *priv, u32 reg)
{
	struct mii_bus *bus = priv->bus;
	u16 lo, hi;
	int ret;

	ret = bus->write(bus, priv->phy_base, 0x1f, 0x4);
	ret = bus->write(bus, priv->phy_base, 0x10, 0);

	ret = bus->write(bus, priv->phy_base, 0x15, ((reg >> 16) & 0xFFFF));
	ret = bus->write(bus, priv->phy_base, 0x16, (reg & 0xFFFF));
	if (ret < 0) {
		dev_err(&bus->dev, "failed to read an8855 register\n");
		return ret;
	}

	lo = bus->read(bus, priv->phy_base, 0x18);
	hi = bus->read(bus, priv->phy_base, 0x17);

	ret = bus->write(bus, priv->phy_base, 0x1f, 0);
	if (ret < 0) {
		dev_err(&bus->dev, "failed to read an8855 register\n");
		return ret;
	}

	return (hi << 16) | (lo & 0xffff);
}

void
an8855_write(struct an8855_priv *priv, u32 reg, u32 val)
{
	struct mii_bus *bus = priv->bus;

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	an8855_mii_write(priv, reg, val);

	mutex_unlock(&bus->mdio_lock);
}

static u32
_an8855_read(struct an8855_dummy_poll *p)
{
	struct mii_bus *bus = p->priv->bus;
	u32 val;

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	val = an8855_mii_read(p->priv, p->reg);

	mutex_unlock(&bus->mdio_lock);

	return val;
}

u32
an8855_read(struct an8855_priv *priv, u32 reg)
{
	struct an8855_dummy_poll p;

	INIT_AN8855_DUMMY_POLL(&p, priv, reg);
	return _an8855_read(&p);
}

static void
an8855_rmw(struct an8855_priv *priv, u32 reg, u32 mask, u32 set)
{
	struct mii_bus *bus = priv->bus;
	u32 val;

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	val = an8855_mii_read(priv, reg);
	val &= ~mask;
	val |= set;
	an8855_mii_write(priv, reg, val);

	mutex_unlock(&bus->mdio_lock);
}

static void
an8855_set(struct an8855_priv *priv, u32 reg, u32 val)
{
	an8855_rmw(priv, reg, 0, val);
}

static void
an8855_clear(struct an8855_priv *priv, u32 reg, u32 val)
{
	an8855_rmw(priv, reg, val, 0);
}

static int
an8855_fdb_cmd(struct an8855_priv *priv, u32 cmd, u32 *rsp)
{
	u32 val;
	int ret;
	struct an8855_dummy_poll p;

	/* Set the command operating upon the MAC address entries */
	val = ATC_BUSY | cmd;
	an8855_write(priv, AN8855_ATC, val);

	INIT_AN8855_DUMMY_POLL(&p, priv, AN8855_ATC);
	ret = readx_poll_timeout(_an8855_read, &p, val,
				 !(val & ATC_BUSY), 20, 200000);
	if (ret < 0) {
		dev_err(priv->dev, "reset timeout\n");
		return ret;
	}

	if (rsp)
		*rsp = val;

	return 0;
}

static void
an8855_fdb_read(struct an8855_priv *priv, struct an8855_fdb *fdb)
{
	u32 reg[4];
	int i;

	/* Read from ARL table into an array */
	for (i = 0; i < 4; i++)
		reg[i] = an8855_read(priv, AN8855_ATRD0 + (i * 4));

	fdb->live = reg[0] & 0x1;
	fdb->type = (reg[0] >> 3) & 0x3;
	fdb->ivl = (reg[0] >> 9) & 0x1;
	fdb->vid = (reg[0] >> 10) & 0xfff;
	fdb->fid = (reg[0] >> 25) & 0xf;
	fdb->aging = (reg[1] >> 3) & 0x1ff;
	fdb->port_mask = reg[3] & 0xff;
	fdb->mac[0] = (reg[2] >> MAC_BYTE_0) & MAC_BYTE_MASK;
	fdb->mac[1] = (reg[2] >> MAC_BYTE_1) & MAC_BYTE_MASK;
	fdb->mac[2] = (reg[2] >> MAC_BYTE_2) & MAC_BYTE_MASK;
	fdb->mac[3] = (reg[2] >> MAC_BYTE_3) & MAC_BYTE_MASK;
	fdb->mac[4] = (reg[1] >> MAC_BYTE_4) & MAC_BYTE_MASK;
	fdb->mac[5] = (reg[1] >> MAC_BYTE_5) & MAC_BYTE_MASK;
	fdb->noarp = !!((reg[0] >> 1) & 0x3);
}

static void
an8855_fdb_write(struct an8855_priv *priv, u16 vid,
		 u8 port_mask, const u8 *mac, u8 add)
{
	u32 reg = 0;

	reg |= mac[3] << MAC_BYTE_3;
	reg |= mac[2] << MAC_BYTE_2;
	reg |= mac[1] << MAC_BYTE_1;
	reg |= mac[0] << MAC_BYTE_0;
	an8855_write(priv, AN8855_ATA1, reg);
	reg = 0;
	reg |= mac[5] << MAC_BYTE_5;
	reg |= mac[4] << MAC_BYTE_4;
	an8855_write(priv, AN8855_ATA2, reg);
	reg = 0;
	if (add)
		reg |= 0x1;
	reg |= 0x1 << 15;
	reg |= vid << 16;
	an8855_write(priv, AN8855_ATWD, reg);
	an8855_write(priv, AN8855_ATWD2, port_mask);
}

static int
an8855_pad_setup(struct dsa_switch *ds, phy_interface_t interface)
{
	return 0;
}

static void
an8855_mib_reset(struct dsa_switch *ds)
{
	struct an8855_priv *priv = ds->priv;

	an8855_write(priv, AN8855_MIB_CCR, CCR_MIB_FLUSH);
	an8855_write(priv, AN8855_MIB_CCR, CCR_MIB_ACTIVATE);
}

static int
an8855_cl22_read(struct an8855_priv *priv, int port, int regnum)
{
	return mdiobus_read_nested(priv->bus, port, regnum);
}

static int
an8855_cl22_write(struct an8855_priv *priv, int port, int regnum, u16 val)
{
	return mdiobus_write_nested(priv->bus, port, regnum, val);
}

static int
an8855_phy_read(struct dsa_switch *ds, int port, int regnum)
{
	struct an8855_priv *priv = ds->priv;

	port += priv->phy_base;
	return an8855_cl22_read(ds->priv, port, regnum);
}

static int
an8855_phy_write(struct dsa_switch *ds, int port, int regnum,
			    u16 val)
{
	struct an8855_priv *priv = ds->priv;

	port += priv->phy_base;
	return an8855_cl22_write(ds->priv, port, regnum, val);
}

static int
an8855_cl45_read(struct an8855_priv *priv, int port, int devad, int regnum)
{
	an8855_cl22_write(priv, port, 0x0d, devad);
	an8855_cl22_write(priv, port, 0x0e, regnum);
	an8855_cl22_write(priv, port, 0x0d, devad | (0x4000));
	return an8855_cl22_read(priv, port, 0x0e);
}

static int
an8855_cl45_write(struct an8855_priv *priv, int port, int devad, int regnum,
		      u16 val)
{
	an8855_cl22_write(priv, port, 0x0d, devad);
	an8855_cl22_write(priv, port, 0x0e, regnum);
	an8855_cl22_write(priv, port, 0x0d, devad | (0x4000));
	an8855_cl22_write(priv, port, 0x0e, val);

	return 0;
}

int
an8855_phy_cl22_read(struct an8855_priv *priv, int port, int regnum)
{
	port += priv->phy_base;
	return an8855_cl22_read(priv, port, regnum);
}

int
an8855_phy_cl22_write(struct an8855_priv *priv, int port, int regnum,
			    u16 val)
{
	port += priv->phy_base;
	return an8855_cl22_write(priv, port, regnum, val);
}

int
an8855_phy_cl45_read(struct an8855_priv *priv, int port, int devad, int regnum)
{
	port += priv->phy_base;
	return an8855_cl45_read(priv, port, devad, regnum);
}

int
an8855_phy_cl45_write(struct an8855_priv *priv, int port, int devad, int regnum,
		      u16 val)
{
	port += priv->phy_base;
	return an8855_cl45_write(priv, port, devad, regnum, val);
}

static void
an8855_get_strings(struct dsa_switch *ds, int port, u32 stringset,
		   uint8_t *data)
{
	int i;

	if (stringset != ETH_SS_STATS)
		return;

	for (i = 0; i < ARRAY_SIZE(an8855_mib); i++)
		strncpy(data + i * ETH_GSTRING_LEN, an8855_mib[i].name,
			ETH_GSTRING_LEN);
}

static void
an8855_get_ethtool_stats(struct dsa_switch *ds, int port, uint64_t *data)
{
	struct an8855_priv *priv = ds->priv;
	const struct an8855_mib_desc *mib;
	u32 reg, i;
	u64 hi;

	for (i = 0; i < ARRAY_SIZE(an8855_mib); i++) {
		mib = &an8855_mib[i];
		reg = AN8855_PORT_MIB_COUNTER(port) + mib->offset;

		data[i] = an8855_read(priv, reg);
		if (mib->size == 2) {
			hi = an8855_read(priv, reg + 4);
			data[i] |= hi << 32;
		}
	}
}

static int
an8855_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	if (sset != ETH_SS_STATS)
		return 0;

	return ARRAY_SIZE(an8855_mib);
}

static int
an8855_cpu_port_enable(struct dsa_switch *ds, int port)
{
	struct an8855_priv *priv = ds->priv;

	/* Setup max capability of CPU port at first */
	if (priv->info->cpu_port_config)
		priv->info->cpu_port_config(ds, port);

	/* Enable Airoha header mode on the cpu port */
	an8855_write(priv, AN8855_PVC_P(port),
		     PORT_SPEC_REPLACE_MODE | PORT_SPEC_TAG);

	/* Unknown multicast frame forwarding to the cpu port */
	an8855_write(priv, AN8855_UNMF, BIT(port));

	/* Set CPU port number */
	an8855_rmw(priv, AN8855_MFC, CPU_MASK, CPU_EN | CPU_PORT(port));

	/* CPU port gets connected to all user ports of
	 * the switch.
	 */
	an8855_write(priv, AN8855_PORTMATRIX_P(port),
		     PORTMATRIX_MATRIX(dsa_user_ports(priv->ds)));

	return 0;
}

static int
an8855_port_enable(struct dsa_switch *ds, int port, struct phy_device *phy)
{
	struct an8855_priv *priv = ds->priv;

	if (!dsa_is_user_port(ds, port))
		return 0;

	mutex_lock(&priv->reg_mutex);

	/* Allow the user port gets connected to the cpu port and also
	 * restore the port matrix if the port is the member of a certain
	 * bridge.
	 */
	priv->ports[port].pm |= PORTMATRIX_MATRIX(BIT(AN8855_CPU_PORT));
	priv->ports[port].enable = true;
	an8855_write(priv, AN8855_PORTMATRIX_P(port), priv->ports[port].pm);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static void
an8855_port_disable(struct dsa_switch *ds, int port)
{
	struct an8855_priv *priv = ds->priv;

	if (!dsa_is_user_port(ds, port))
		return;

	mutex_lock(&priv->reg_mutex);

	/* Clear up all port matrix which could be restored in the next
	 * enablement for the port.
	 */
	priv->ports[port].enable = false;
	an8855_write(priv, AN8855_PORTMATRIX_P(port), PORTMATRIX_CLR);

	mutex_unlock(&priv->reg_mutex);
}

static void
an8855_stp_state_set(struct dsa_switch *ds, int port, u8 state)
{
	struct an8855_priv *priv = ds->priv;
	u32 stp_state;

	if (dsa_is_unused_port(ds, port))
		return;

	switch (state) {
	case BR_STATE_DISABLED:
		stp_state = AN8855_STP_DISABLED;
		break;
	case BR_STATE_BLOCKING:
		stp_state = AN8855_STP_BLOCKING;
		break;
	case BR_STATE_LISTENING:
		stp_state = AN8855_STP_LISTENING;
		break;
	case BR_STATE_LEARNING:
		stp_state = AN8855_STP_LEARNING;
		break;
	case BR_STATE_FORWARDING:
	default:
		stp_state = AN8855_STP_FORWARDING;
		break;
	}

	an8855_rmw(priv, AN8855_SSP_P(port), FID_PST_MASK, stp_state);
}

static int
an8855_port_bridge_join(struct dsa_switch *ds, int port,
			struct dsa_bridge bridge, bool *tx_fwd_offload, struct netlink_ext_ack *extack)
{
	struct an8855_priv *priv = ds->priv;
	u32 port_bitmap = BIT(AN8855_CPU_PORT);
	int i;

	mutex_lock(&priv->reg_mutex);

	for (i = 0; i < AN8855_NUM_PORTS; i++) {
		/* Add this port to the port matrix of the other ports in the
		 * same bridge. If the port is disabled, port matrix is kept
		 * and not being setup until the port becomes enabled.
		 */
		if (dsa_is_user_port(ds, i) && i != port) {
			if (!dsa_port_offloads_bridge(dsa_to_port(ds, i), &bridge))
				continue;
			if (priv->ports[i].enable)
				an8855_set(priv, AN8855_PORTMATRIX_P(i),
					   PORTMATRIX_MATRIX(BIT(port)));
			priv->ports[i].pm |= PORTMATRIX_MATRIX(BIT(port));

			port_bitmap |= BIT(i);
		}
	}

	/* Add the all other ports to this port matrix. */
	if (priv->ports[port].enable)
		an8855_rmw(priv, AN8855_PORTMATRIX_P(port),
			   PORTMATRIX_MASK, PORTMATRIX_MATRIX(port_bitmap));
	priv->ports[port].pm |= PORTMATRIX_MATRIX(port_bitmap);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static void
an8855_port_set_vlan_unaware(struct dsa_switch *ds, int port)
{
	struct an8855_priv *priv = ds->priv;
	bool all_user_ports_removed = true;
	int i;

	/* When a port is removed from the bridge, the port would be set up
	 * back to the default as is at initial boot which is a VLAN-unaware
	 * port.
	 */
	an8855_rmw(priv, AN8855_PCR_P(port), PCR_PORT_VLAN_MASK,
		   AN8855_PORT_MATRIX_MODE);
	an8855_rmw(priv, AN8855_PVC_P(port), VLAN_ATTR_MASK | PVC_EG_TAG_MASK,
		   VLAN_ATTR(AN8855_VLAN_TRANSPARENT) |
		   PVC_EG_TAG(AN8855_VLAN_EG_CONSISTENT));

	for (i = 0; i < AN8855_NUM_PORTS; i++) {
		if (dsa_is_user_port(ds, i) &&
		    dsa_port_is_vlan_filtering(dsa_to_port(ds, i))) {
			all_user_ports_removed = false;
			break;
		}
	}

	/* CPU port also does the same thing until all user ports belonging to
	 * the CPU port get out of VLAN filtering mode.
	 */
	if (all_user_ports_removed) {
		an8855_write(priv, AN8855_PORTMATRIX_P(AN8855_CPU_PORT),
			     PORTMATRIX_MATRIX(dsa_user_ports(priv->ds)));
		an8855_write(priv, AN8855_PVC_P(AN8855_CPU_PORT),
			     PORT_SPEC_REPLACE_MODE | PORT_SPEC_TAG |
			     PVC_EG_TAG(AN8855_VLAN_EG_CONSISTENT));
	}
}

static void
an8855_port_set_vlan_aware(struct dsa_switch *ds, int port)
{
	struct an8855_priv *priv = ds->priv;

	/* Trapped into security mode allows packet forwarding through VLAN
	 * table lookup. CPU port is set to fallback mode to let untagged
	 * frames pass through.
	 */
	if (dsa_is_cpu_port(ds, port))
		an8855_rmw(priv, AN8855_PCR_P(port), PCR_PORT_VLAN_MASK,
			   AN8855_PORT_FALLBACK_MODE);
	else
		an8855_rmw(priv, AN8855_PCR_P(port), PCR_PORT_VLAN_MASK,
			   AN8855_PORT_SECURITY_MODE);

	/* Set the port as a user port which is to be able to recognize VID
	 * from incoming packets before fetching entry within the VLAN table.
	 */
	an8855_rmw(priv, AN8855_PVC_P(port), VLAN_ATTR_MASK | PVC_EG_TAG_MASK,
		   VLAN_ATTR(AN8855_VLAN_USER) |
		   PVC_EG_TAG(AN8855_VLAN_EG_DISABLED));
}

static void
an8855_port_bridge_leave(struct dsa_switch *ds, int port,
			 struct dsa_bridge bridge)
{
	struct an8855_priv *priv = ds->priv;
	int i;

	mutex_lock(&priv->reg_mutex);

	for (i = 0; i < AN8855_NUM_PORTS; i++) {
		/* Remove this port from the port matrix of the other ports
		 * in the same bridge. If the port is disabled, port matrix
		 * is kept and not being setup until the port becomes enabled.
		 */
		if (dsa_is_user_port(ds, i) && i != port) {
			if (!dsa_port_offloads_bridge(dsa_to_port(ds, i), &bridge))
				continue;
			if (priv->ports[i].enable)
				an8855_clear(priv, AN8855_PORTMATRIX_P(i),
					     PORTMATRIX_MATRIX(BIT(port)));
			priv->ports[i].pm &= PORTMATRIX_MATRIX(BIT(port));
		}
	}

	/* Set the cpu port to be the only one in the port matrix of
	 * this port.
	 */
	if (priv->ports[port].enable)
		an8855_rmw(priv, AN8855_PORTMATRIX_P(port), PORTMATRIX_MASK,
			   PORTMATRIX_MATRIX(BIT(AN8855_CPU_PORT)));
	priv->ports[port].pm = PORTMATRIX_MATRIX(BIT(AN8855_CPU_PORT));

	mutex_unlock(&priv->reg_mutex);
}

static int
an8855_port_fdb_add(struct dsa_switch *ds, int port,
		    const unsigned char *addr, u16 vid, struct dsa_db db)
{
	struct an8855_priv *priv = ds->priv;
	int ret;
	u8 port_mask = BIT(port);

	mutex_lock(&priv->reg_mutex);
	an8855_fdb_write(priv, vid, port_mask, addr, 1);
	ret = an8855_fdb_cmd(priv, AN8855_FDB_WRITE, NULL);
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static int
an8855_port_fdb_del(struct dsa_switch *ds, int port,
		    const unsigned char *addr, u16 vid, struct dsa_db db)
{
	struct an8855_priv *priv = ds->priv;
	int ret;
	u8 port_mask = BIT(port);

	mutex_lock(&priv->reg_mutex);
	an8855_fdb_write(priv, vid, port_mask, addr, 0);
	ret = an8855_fdb_cmd(priv, AN8855_FDB_WRITE, NULL);
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static int
an8855_port_fdb_dump(struct dsa_switch *ds, int port,
		     dsa_fdb_dump_cb_t *cb, void *data)
{
	struct an8855_priv *priv = ds->priv;
	struct an8855_fdb _fdb = { 0 };
	int cnt = 512;
	int num = 4;
	int index = 0;
	bool flag = false;
	int banks = 0;
	int i = 0;
	int ret = 0;
	u32 rsp = 0;

	mutex_lock(&priv->reg_mutex);

	an8855_write(priv, AN8855_ATWD2, (0x1 << port));
	ret = an8855_fdb_cmd(priv, ATC_MAT(0xc) | AN8855_FDB_START, &rsp);
	if (ret < 0)
		goto err;

	index = (rsp >> ATC_HASH) & ATC_HASH_MASK;
	if (index == (cnt - 1))
		flag = true;
	else
		flag = false;

	banks = (rsp >> ATC_HIT) & ATC_HIT_MASK;
	if (banks == 0) {
		mutex_unlock(&priv->reg_mutex);
		return 0;
	}
	for (i = 0; i < num; i++) {
		if ((banks >> i) & 0x1) {
			an8855_write(priv, AN8855_ATRDS, i);
			udelay(1000);
			an8855_fdb_read(priv, &_fdb);
			if (!_fdb.live)
				continue;
			if (_fdb.port_mask & BIT(port)) {
				ret = cb(_fdb.mac, _fdb.vid, _fdb.noarp, data);
				if (ret < 0)
					continue;
			}
		}
	}
	while (1) {
		if (flag == true)
			break;

		ret =
		    an8855_fdb_cmd(priv, ATC_MAT(0xc) | AN8855_FDB_NEXT, &rsp);
		index = (rsp >> ATC_HASH) & ATC_HASH_MASK;
		if (index == (cnt - 1))
			flag = true;
		else
			flag = false;

		banks = (rsp >> ATC_HIT) & ATC_HIT_MASK;
		if (banks == 0) {
			mutex_unlock(&priv->reg_mutex);
			return 0;
		}
		for (i = 0; i < num; i++) {
			if ((banks >> i) & 0x1) {
				an8855_write(priv, AN8855_ATRDS, i);
				udelay(1000);
				an8855_fdb_read(priv, &_fdb);
				if (!_fdb.live)
					continue;
				if (_fdb.port_mask & BIT(port)) {
					ret = cb(_fdb.mac, _fdb.vid, _fdb.noarp,
						 data);
					if (ret < 0)
						continue;
				}
			}
		}
	}

err:
	mutex_unlock(&priv->reg_mutex);
	return 0;
}

static int
an8855_vlan_cmd(struct an8855_priv *priv, enum an8855_vlan_cmd cmd, u16 vid)
{
	struct an8855_dummy_poll p;
	u32 val;
	int ret;

	if (vid > 0xFFF) {
		dev_err(priv->dev, "vid number invalid\n");
		return -EINVAL;
	}

	val = VTCR_BUSY | VTCR_FUNC(cmd) | vid;
	an8855_write(priv, AN8855_VTCR, val);

	INIT_AN8855_DUMMY_POLL(&p, priv, AN8855_VTCR);
	ret = readx_poll_timeout(_an8855_read, &p, val,
				 !(val & VTCR_BUSY), 20, 200000);
	if (ret < 0) {
		dev_err(priv->dev, "poll timeout\n");
		return ret;
	}

	return 0;
}

static int
an8855_port_vlan_filtering(struct dsa_switch *ds, int port, bool vlan_filtering,
						   struct netlink_ext_ack *extack)
{
	if (vlan_filtering) {
		/* The port is being kept as VLAN-unaware port when bridge is
		 * set up with vlan_filtering not being set, Otherwise, the
		 * port and the corresponding CPU port is required the setup
		 * for becoming a VLAN-aware port.
		 */
		an8855_port_set_vlan_aware(ds, port);
		an8855_port_set_vlan_aware(ds, AN8855_CPU_PORT);
	} else {
		an8855_port_set_vlan_unaware(ds, port);
	}

	return 0;
}

static void
an8855_hw_vlan_add(struct an8855_priv *priv,
				   struct an8855_hw_vlan_entry *entry)
{
	u8 new_members;
	u32 val;

	new_members = entry->old_members | BIT(entry->port) |
	    BIT(AN8855_CPU_PORT);

	/* Validate the entry with independent learning, create egress tag per
	 * VLAN and joining the port as one of the port members.
	 */
	val =
	    an8855_read(priv,
			AN8855_VARD0) & (ETAG_CTRL_MASK << PORT_EG_CTRL_SHIFT);
	val |= (IVL_MAC | VTAG_EN | PORT_MEM(new_members) | VLAN_VALID);
	an8855_write(priv, AN8855_VAWD0, val);
	an8855_write(priv, AN8855_VAWD1, 0);

	/* Decide whether adding tag or not for those outgoing packets from the
	 * port inside the VLAN.
	 */
	val =
	    entry->untagged ? AN8855_VLAN_EGRESS_UNTAG : AN8855_VLAN_EGRESS_TAG;
	an8855_rmw(priv, AN8855_VAWD0,
		   ETAG_CTRL_P_MASK(entry->port) << PORT_EG_CTRL_SHIFT,
		   ETAG_CTRL_P(entry->port, val) << PORT_EG_CTRL_SHIFT);

	/* CPU port is always taken as a tagged port for serving more than one
	 * VLANs across and also being applied with egress type stack mode for
	 * that VLAN tags would be appended after hardware special tag used as
	 * DSA tag.
	 */
	an8855_rmw(priv, AN8855_VAWD0,
		   ETAG_CTRL_P_MASK(AN8855_CPU_PORT) << PORT_EG_CTRL_SHIFT,
		   ETAG_CTRL_P(AN8855_CPU_PORT,
			       AN8855_VLAN_EGRESS_STACK) << PORT_EG_CTRL_SHIFT);
}

static void
an8855_hw_vlan_del(struct an8855_priv *priv,
				   struct an8855_hw_vlan_entry *entry)
{
	u8 new_members;
	u32 val;

	new_members = entry->old_members & ~BIT(entry->port);

	val = an8855_read(priv, AN8855_VARD0);
	if (!(val & VLAN_VALID)) {
		dev_err(priv->dev, "Cannot be deleted due to invalid entry\n");
		return;
	}

	/* If certain member apart from CPU port is still alive in the VLAN,
	 * the entry would be kept valid. Otherwise, the entry is got to be
	 * disabled.
	 */
	if (new_members && new_members != BIT(AN8855_CPU_PORT)) {
		val = IVL_MAC | VTAG_EN | PORT_MEM(new_members) | VLAN_VALID;
		an8855_write(priv, AN8855_VAWD0, val);
	} else {
		an8855_write(priv, AN8855_VAWD0, 0);
		an8855_write(priv, AN8855_VAWD1, 0);
	}
}

static void
an8855_hw_vlan_update(struct an8855_priv *priv, u16 vid,
		      struct an8855_hw_vlan_entry *entry,
		      an8855_vlan_op vlan_op)
{
	u32 val;

	/* Fetch entry */
	an8855_vlan_cmd(priv, AN8855_VTCR_RD_VID, vid);

	val = an8855_read(priv, AN8855_VARD0);

	entry->old_members = (val >> PORT_MEM_SHFT) & PORT_MEM_MASK;

	/* Manipulate entry */
	vlan_op(priv, entry);

	/* Flush result to hardware */
	an8855_vlan_cmd(priv, AN8855_VTCR_WR_VID, vid);
}

static int
an8855_port_vlan_add(struct dsa_switch *ds, int port,
		     const struct switchdev_obj_port_vlan *vlan,
		     struct netlink_ext_ack *extack)
{
	bool untagged = vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED;
	bool pvid = vlan->flags & BRIDGE_VLAN_INFO_PVID;
	struct an8855_hw_vlan_entry new_entry;
	struct an8855_priv *priv = ds->priv;

	mutex_lock(&priv->reg_mutex);

	an8855_hw_vlan_entry_init(&new_entry, port, untagged);
	an8855_hw_vlan_update(priv, vlan->vid, &new_entry, an8855_hw_vlan_add);

	if (pvid) {
		an8855_rmw(priv, AN8855_PVID_P(port), G0_PORT_VID_MASK,
			   G0_PORT_VID(vlan->vid));
		priv->ports[port].pvid = vlan->vid;
	}

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int
an8855_port_vlan_del(struct dsa_switch *ds, int port,
		     const struct switchdev_obj_port_vlan *vlan)
{
	struct an8855_hw_vlan_entry target_entry;
	struct an8855_priv *priv = ds->priv;
	u16 pvid;

	mutex_lock(&priv->reg_mutex);

	pvid = priv->ports[port].pvid;
	an8855_hw_vlan_entry_init(&target_entry, port, 0);
	an8855_hw_vlan_update(priv, vlan->vid, &target_entry,
			      an8855_hw_vlan_del);

	/* PVID is being restored to the default whenever the PVID port
	 * is being removed from the VLAN.
	 */
	if (pvid == vlan->vid)
		pvid = G0_PORT_VID_DEF;

	an8855_rmw(priv, AN8855_PVID_P(port), G0_PORT_VID_MASK, pvid);
	priv->ports[port].pvid = pvid;

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int an8855_port_mirror_add(struct dsa_switch *ds, int port,
				  struct dsa_mall_mirror_tc_entry *mirror,
				  bool ingress, struct netlink_ext_ack *extack)
{
	struct an8855_priv *priv = ds->priv;
	int monitor_port;
	u32 val;

	/* Check for existent entry */
	if ((ingress ? priv->mirror_rx : priv->mirror_tx) & BIT(port))
		return -EEXIST;

	val = an8855_read(priv, AN8855_MIR);

	/* AN8855 supports 4 monitor port, but only use first group */
	monitor_port = AN8855_MIRROR_PORT_GET(val);
	if (val & AN8855_MIRROR_EN && monitor_port != mirror->to_local_port)
		return -EEXIST;

	val |= AN8855_MIRROR_EN;
	val &= ~AN8855_MIRROR_MASK;
	val |= AN8855_MIRROR_PORT_SET(mirror->to_local_port);
	an8855_write(priv, AN8855_MIR, val);

	val = an8855_read(priv, AN8855_PCR_P(port));
	if (ingress) {
		val |= PORT_RX_MIR;
		priv->mirror_rx |= BIT(port);
	} else {
		val |= PORT_TX_MIR;
		priv->mirror_tx |= BIT(port);
	}
	an8855_write(priv, AN8855_PCR_P(port), val);

	return 0;
}

static void an8855_port_mirror_del(struct dsa_switch *ds, int port,
				   struct dsa_mall_mirror_tc_entry *mirror)
{
	struct an8855_priv *priv = ds->priv;
	u32 val;

	val = an8855_read(priv, AN8855_PCR_P(port));
	if (mirror->ingress) {
		val &= ~PORT_RX_MIR;
		priv->mirror_rx &= ~BIT(port);
	} else {
		val &= ~PORT_TX_MIR;
		priv->mirror_tx &= ~BIT(port);
	}
	an8855_write(priv, AN8855_PCR_P(port), val);

	if (!priv->mirror_rx && !priv->mirror_tx) {
		val = an8855_read(priv, AN8855_MIR);
		val &= ~AN8855_MIRROR_EN;
		an8855_write(priv, AN8855_MIR, val);
	}
}

static enum dsa_tag_protocol
air_get_tag_protocol(struct dsa_switch *ds, int port, enum dsa_tag_protocol mp)
{
	struct an8855_priv *priv = ds->priv;

	if (port != AN8855_CPU_PORT) {
		dev_warn(priv->dev, "port not matched with tagging CPU port\n");
		return DSA_TAG_PROTO_NONE;
	} else {
		return DSA_TAG_PROTO_ARHT;
	}
}

static int
setup_unused_ports(struct dsa_switch *ds, u32 pm)
{
	struct an8855_priv *priv = ds->priv;
	u32 egtag_mask = 0;
	u32 egtag_val = 0;
	int i;

	if (!pm)
		return 0;

	for (i = 0; i < AN8855_NUM_PORTS; i++) {
		if (!dsa_is_unused_port(ds, i))
			continue;

		/* Setup MAC port with maximum capability. */
		if (i == 5)
			if (priv->info->cpu_port_config)
				priv->info->cpu_port_config(ds, i);

		an8855_rmw(priv, AN8855_PORTMATRIX_P(i), PORTMATRIX_MASK,
			   AN8855_PORTMATRIX_P(pm));
		an8855_rmw(priv, AN8855_PCR_P(i), PCR_PORT_VLAN_MASK,
			   AN8855_PORT_SECURITY_MODE);
		egtag_mask |= ETAG_CTRL_P_MASK(i);
		egtag_val |= ETAG_CTRL_P(i, AN8855_VLAN_EGRESS_UNTAG);
	}

	/* Add unused ports to VLAN2 group for using IVL fdb. */
	an8855_write(priv, AN8855_VAWD0,
		     IVL_MAC | VTAG_EN | PORT_MEM(pm) | VLAN_VALID);
	an8855_rmw(priv, AN8855_VAWD0, egtag_mask << PORT_EG_CTRL_SHIFT,
		   egtag_val << PORT_EG_CTRL_SHIFT);
	an8855_write(priv, AN8855_VAWD1, 0);
	an8855_vlan_cmd(priv, AN8855_VTCR_WR_VID, AN8855_RESERVED_VLAN);

	for (i = 0; i < AN8855_NUM_PORTS; i++) {
		if (!dsa_is_unused_port(ds, i))
			continue;

		an8855_rmw(priv, AN8855_PVID_P(i), G0_PORT_VID_MASK,
			   G0_PORT_VID(AN8855_RESERVED_VLAN));
		an8855_rmw(priv, AN8855_SSP_P(i), FID_PST_MASK,
			   AN8855_STP_FORWARDING);

		dev_dbg(ds->dev, "Add unused port%d to reserved VLAN%d group\n",
			i, AN8855_RESERVED_VLAN);
	}

	return 0;
}

static int an8855_led_set_usr_def(struct dsa_switch *ds, u8 entity,
		int polar, u16 on_evt, u16 blk_evt, u8 led_freq)
{
	struct an8855_priv *priv = ds->priv;
	u32 cl45_data = 0;

	if (polar == LED_HIGH)
		on_evt |= LED_ON_POL;
	else
		on_evt &= ~LED_ON_POL;

	/* LED on event */
	an8855_phy_cl45_write(priv, (entity / 4), PHY_DEV1E,
		PHY_SINGLE_LED_ON_CTRL(entity % 4), on_evt | LED_ON_EN);

	/* LED blink event */
	an8855_phy_cl45_write(priv, (entity / 4), PHY_DEV1E,
		PHY_SINGLE_LED_BLK_CTRL(entity % 4), blk_evt);

	/* LED freq */
	switch (led_freq) {
	case AIR_LED_BLK_DUR_32M:
		cl45_data = 0x30e;
		break;
	case AIR_LED_BLK_DUR_64M:
		cl45_data = 0x61a;
		break;
	case AIR_LED_BLK_DUR_128M:
		cl45_data = 0xc35;
		break;
	case AIR_LED_BLK_DUR_256M:
		cl45_data = 0x186a;
		break;
	case AIR_LED_BLK_DUR_512M:
		cl45_data = 0x30d4;
		break;
	case AIR_LED_BLK_DUR_1024M:
		cl45_data = 0x61a8;
		break;
	default:
		break;
	}
	an8855_phy_cl45_write(priv, (entity / 4), PHY_DEV1E,
		PHY_SINGLE_LED_BLK_DUR(entity % 4), cl45_data);

	an8855_phy_cl45_write(priv, (entity / 4), PHY_DEV1E,
		PHY_SINGLE_LED_ON_DUR(entity % 4), (cl45_data >> 1));

	/* Disable DATA & BAD_SSD for port LED blink behavior */
	cl45_data = an8855_phy_cl45_read(priv, (entity / 4), PHY_DEV1E,
		PHY_PMA_CTRL);
	cl45_data &= ~BIT(0);
	cl45_data &= ~BIT(15);
	an8855_phy_cl45_write(priv, (entity / 4), PHY_DEV1E,
		PHY_PMA_CTRL, cl45_data);

	return 0;
}

static int an8855_led_set_mode(struct dsa_switch *ds, u8 mode)
{
	struct an8855_priv *priv = ds->priv;
	u16 cl45_data;

	cl45_data = an8855_phy_cl45_read(priv, 0, PHY_DEV1F, PHY_LED_BCR);
	switch (mode) {
	case AN8855_LED_MODE_DISABLE:
		cl45_data &= ~LED_BCR_EXT_CTRL;
		cl45_data &= ~LED_BCR_MODE_MASK;
		cl45_data |= LED_BCR_MODE_DISABLE;
		break;
	case AN8855_LED_MODE_USER_DEFINE:
		cl45_data |= LED_BCR_EXT_CTRL;
		cl45_data |= LED_BCR_CLK_EN;
		break;
	default:
		dev_err(priv->dev, "LED mode%d is not supported!\n", mode);
		return -EINVAL;
	}
	an8855_phy_cl45_write(priv, 0, PHY_DEV1F, PHY_LED_BCR, cl45_data);

	return 0;
}

static int an8855_led_set_state(struct dsa_switch *ds, u8 entity, u8 state)
{
	struct an8855_priv *priv = ds->priv;
	u16 cl45_data = 0;

	/* Change to per port contorl */
	cl45_data = an8855_phy_cl45_read(priv, (entity / 4), PHY_DEV1E,
		PHY_LED_CTRL_SELECT);

	if (state == 1)
		cl45_data |= (1 << (entity % 4));
	else
		cl45_data &= ~(1 << (entity % 4));

	an8855_phy_cl45_write(priv, (entity / 4), PHY_DEV1E,
		PHY_LED_CTRL_SELECT, cl45_data);

	/* LED enable setting */
	cl45_data = an8855_phy_cl45_read(priv, (entity / 4),
		PHY_DEV1E, PHY_SINGLE_LED_ON_CTRL(entity % 4));

	if (state == 1)
		cl45_data |= LED_ON_EN;
	else
		cl45_data &= ~LED_ON_EN;

	an8855_phy_cl45_write(priv, (entity / 4), PHY_DEV1E,
		PHY_SINGLE_LED_ON_CTRL(entity % 4), cl45_data);

	return 0;
}

static int an8855_led_init(struct dsa_switch *ds)
{
	struct an8855_priv *priv = ds->priv;
	u32 val, led_count = ARRAY_SIZE(led_cfg);
	int ret = 0, id;
	u32 tmp_val = 0;
	u32 tmp_id = 0;

	ret = an8855_led_set_mode(ds, AN8855_LED_MODE_USER_DEFINE);
	if (ret != 0) {
		dev_err(priv->dev, "led_set_mode fail(ret:%d)!\n", ret);
		return ret;
	}

	for (id = 0; id < led_count; id++) {
		ret = an8855_led_set_state(ds,
			led_cfg[id].phy_led_idx, led_cfg[id].en);
		if (ret != 0) {
			dev_err(priv->dev, "led_set_state fail(ret:%d)!\n", ret);
			return ret;
		}
		if (led_cfg[id].en == 1) {
			ret = an8855_led_set_usr_def(ds,
				led_cfg[id].phy_led_idx,
				led_cfg[id].pol, led_cfg[id].on_cfg,
				led_cfg[id].blk_cfg,
				led_cfg[id].led_freq);
			if (ret != 0) {
				dev_err(priv->dev, "led_set_usr_def fail!\n");
				return ret;
			}
		}
	}

	/* Setting for System LED & Loop LED */
	an8855_write(priv, RG_GPIO_OE, 0x0);
	an8855_write(priv, RG_GPIO_CTRL, 0x0);
	val = 0;
	an8855_write(priv, RG_GPIO_L_INV, val);

	val = 0x1001;
	an8855_write(priv, RG_GPIO_CTRL, val);
	val = an8855_read(priv, RG_GPIO_DATA);
	val |= BITS(1, 3);
	val &= ~(BIT(0));
	val &= ~(BIT(6));

	an8855_write(priv, RG_GPIO_DATA, val);
	val = an8855_read(priv, RG_GPIO_OE);
	val |= 0x41;
	an8855_write(priv, RG_GPIO_OE, val);

	/* Mapping between GPIO & LED */
	val = 0;
	for (id = 0; id < led_count; id++) {
		/* Skip GPIO6, due to GPIO6 does not support PORT LED */
		if (id == 6)
			continue;

		if (led_cfg[id].en == 1) {
			if (id < 7)
				val |= led_cfg[id].phy_led_idx << ((id % 4) * 8);
			else
				val |= led_cfg[id].phy_led_idx << (((id - 1) % 4) * 8);
		}

		if (id < 7)
			tmp_id = id;
		else
			tmp_id = id - 1;

		if ((tmp_id % 4) == 0x3) {
			an8855_write(priv, RG_GPIO_LED_SEL(tmp_id / 4), val);
			tmp_val = an8855_read(priv, RG_GPIO_LED_SEL(tmp_id / 4));
			val = 0;
		}
	}

	/* Turn on LAN LED mode */
	val = 0;
	for (id = 0; id < led_count; id++) {
		if (led_cfg[id].en == 1)
			val |= 0x1 << id;
	}
	an8855_write(priv, RG_GPIO_LED_MODE, val);

	/* Force clear blink pulse for per port LED */
	an8855_phy_cl45_write(priv, 0, PHY_DEV1F, PHY_LED_BLINK_DUR_CTRL, 0x1f);
	usleep_range(1000, 5000);
	an8855_phy_cl45_write(priv, 0, PHY_DEV1F, PHY_LED_BLINK_DUR_CTRL, 0);

	return 0;
}

static int
an8855_setup(struct dsa_switch *ds)
{
	struct an8855_priv *priv = ds->priv;
	struct an8855_dummy_poll p;
	u32 unused_pm = 0;
	u32 val, id, led_count = ARRAY_SIZE(led_cfg);
	int ret, i;

	/* Reset whole chip through gpio pin or memory-mapped registers for
	 * different type of hardware
	 */
	gpiod_set_value_cansleep(priv->reset, 0);
	usleep_range(100000, 150000);
	gpiod_set_value_cansleep(priv->reset, 1);
	usleep_range(100000, 150000);

	/* Waiting for AN8855 got to stable */
	INIT_AN8855_DUMMY_POLL(&p, priv, 0x1000009c);
	ret = readx_poll_timeout(_an8855_read, &p, val, val != 0, 20, 1000000);
	if (ret < 0) {
		dev_err(priv->dev, "reset timeout\n");
		return ret;
	}

	id = an8855_read(priv, AN8855_CREV);
	if (id != AN8855_ID) {
		dev_err(priv->dev, "chip %x can't be supported\n", id);
		return -ENODEV;
	}

	/* Reset the switch through internal reset */
	an8855_write(priv, AN8855_RST_CTRL, SYS_CTRL_SYS_RST);
	usleep_range(100000, 110000);

	/* change gphy smi address */
	if (priv->phy_base_new > 0) {
		an8855_write(priv, RG_GPHY_SMI_ADDR, priv->phy_base_new);
		priv->phy_base = priv->phy_base_new;
	}

	for (i = 0; i < AN8855_NUM_PHYS; i++) {
		val = an8855_phy_read(ds, i, MII_BMCR);
		val |= BMCR_ISOLATE;
		an8855_phy_write(ds, i, MII_BMCR, val);
	}

	/* AN8855H need to setup before switch init */
	val = an8855_read(priv, PKG_SEL);
	if ((val & 0x7) == PAG_SEL_AN8855H) {
		/* Invert for LED activity change */
		val = an8855_read(priv, RG_GPIO_L_INV);
		for (id = 0; id < led_count; id++) {
			if ((led_cfg[id].pol == LED_HIGH) &&
				(led_cfg[id].en == 1))
				val |= 0x1 << id;
		}
		an8855_write(priv, RG_GPIO_L_INV, (val | 0x1));

		/* MCU NOP CMD */
		an8855_write(priv, RG_GDMP_RAM, 0x846);
		an8855_write(priv, RG_GDMP_RAM + 4, 0x4a);

		/* Enable MCU */
		val = an8855_read(priv, RG_CLK_CPU_ICG);
		an8855_write(priv, RG_CLK_CPU_ICG, val | MCU_ENABLE);
		usleep_range(1000, 5000);

		/* Disable MCU watchdog */
		val = an8855_read(priv, RG_TIMER_CTL);
		an8855_write(priv, RG_TIMER_CTL, (val & (~WDOG_ENABLE)));

		/* Configure interrupt */
		an8855_write(priv, RG_INTB_MODE, (0x1 << priv->intr_pin));

		/* LED settings for T830 reference board */
		ret = an8855_led_init(ds);
		if (ret < 0) {
			dev_err(priv->dev, "an8855_led_init fail. (ret=%d)\n", ret);
			return ret;
		}
	}

	/* Adjust to reduce noise */
	for (i = 0; i < AN8855_NUM_PHYS; i++) {
		an8855_phy_cl45_write(priv, i, PHY_DEV1E,
			PHY_TX_PAIR_DLY_SEL_GBE, 0x4040);

		an8855_phy_cl45_write(priv, i, PHY_DEV1E,
			PHY_RXADC_CTRL, 0x1010);

		an8855_phy_cl45_write(priv, i, PHY_DEV1E,
			PHY_RXADC_REV_0, 0x100);

		an8855_phy_cl45_write(priv, i, PHY_DEV1E,
			PHY_RXADC_REV_1, 0x100);
	}

	/* Let phylink decide the interface later. */
	priv->p5_interface = PHY_INTERFACE_MODE_NA;

	/* BPDU to CPU port */
	//an8855_rmw(priv, AN8855_CFC, AN8855_CPU_PMAP_MASK,
	//         BIT(AN8855_CPU_PORT));
	an8855_rmw(priv, AN8855_BPC, AN8855_BPDU_PORT_FW_MASK,
		   AN8855_BPDU_CPU_ONLY);

	val = an8855_read(priv, AN8855_CKGCR);
	val &= ~(CKG_LNKDN_GLB_STOP | CKG_LNKDN_PORT_STOP);
	an8855_write(priv, AN8855_CKGCR, val);

	/* Enable and reset MIB counters */
	an8855_mib_reset(ds);

	for (i = 0; i < AN8855_NUM_PORTS; i++) {
		/* Disable forwarding by default on all ports */
		an8855_rmw(priv, AN8855_PORTMATRIX_P(i), PORTMATRIX_MASK,
			   PORTMATRIX_CLR);
		if (dsa_is_unused_port(ds, i))
			unused_pm |= BIT(i);
		else if (dsa_is_cpu_port(ds, i))
			an8855_cpu_port_enable(ds, i);
		else
			an8855_port_disable(ds, i);
		/* Enable consistent egress tag */
		an8855_rmw(priv, AN8855_PVC_P(i), PVC_EG_TAG_MASK,
			   PVC_EG_TAG(AN8855_VLAN_EG_CONSISTENT));
	}

	for (i = 0; i < AN8855_NUM_PHYS; i++) {
		val = an8855_phy_read(ds, i, MII_BMCR);
		val &= ~BMCR_ISOLATE;
		an8855_phy_write(ds, i, MII_BMCR, val);
	}

	an8855_phy_setup(ds);

	/* PHY restart AN*/
	for (i = 0; i < AN8855_NUM_PHYS; i++)
		an8855_phy_write(ds, i, MII_BMCR, 0x1240);

	/* Group and enable unused ports as a standalone dumb switch. */
	setup_unused_ports(ds, unused_pm);

	ds->configure_vlan_while_not_filtering = true;

	/* Flush the FDB table */
	ret = an8855_fdb_cmd(priv, AN8855_FDB_FLUSH, NULL);
	if (ret < 0)
		return ret;

	return 0;
}

static bool
an8855_phy_supported(struct dsa_switch *ds, int port,
		     const struct phylink_link_state *state)
{
	struct an8855_priv *priv = ds->priv;

	switch (port) {
	case 0:		/* Internal phy */
	case 1:
	case 2:
	case 3:
	case 4:
		if (state->interface != PHY_INTERFACE_MODE_GMII)
			goto unsupported;
		break;
	case 5:
		if (state->interface != PHY_INTERFACE_MODE_SGMII
		    && state->interface != PHY_INTERFACE_MODE_RGMII
			&& state->interface != PHY_INTERFACE_MODE_2500BASEX)
			goto unsupported;
		break;
	default:
		dev_err(priv->dev, "%s: unsupported port: %i\n", __func__,
			port);
		goto unsupported;
	}

	return true;

unsupported:
	return false;
}

static int
an8855_rgmii_setup(struct an8855_priv *priv, u32 port,
			      phy_interface_t interface,
			      struct phy_device *phydev)
{
	return 0;
}

static void
an8855_sgmii_validate(struct an8855_priv *priv, int port,
				  unsigned long *supported)
{
	switch (port) {
	case 5:
		phylink_set(supported, 1000baseX_Full);
		phylink_set(supported, 2500baseX_Full);
	}
}

static bool
an8855_is_mac_port(u32 port)
{
	return (port == 5);
}

static int
an8855_set_hsgmii_mode(struct an8855_priv *priv)
{
	u32 val = 0;

	/* TX FIR - improve TX EYE */
	val = an8855_read(priv, INTF_CTRL_10);
	val &= ~(0x3f << 16);
	val |= BIT(21);
	val &= ~(0x1f << 24);
	val |= (0x4 << 24);
	val |= BIT(29);
	an8855_write(priv, INTF_CTRL_10, val);

	val = an8855_read(priv, INTF_CTRL_11);
	val &= ~(0x3f);
	val |= BIT(6);
	an8855_write(priv, INTF_CTRL_11, val);

	/* RX CDR - improve RX Jitter Tolerance */
	val = an8855_read(priv, RG_QP_CDR_LPF_BOT_LIM);
	val &= ~(0x7 << 24);
	val |= (0x5 << 24);
	val &= ~(0x7 << 20);
	val |= (0x5 << 20);
	an8855_write(priv, RG_QP_CDR_LPF_BOT_LIM, val);

	/* PLL */
	val = an8855_read(priv, QP_DIG_MODE_CTRL_1);
	val &= ~(0x3 << 2);
	val |= (0x1 << 2);
	an8855_write(priv, QP_DIG_MODE_CTRL_1, val);

	/* PLL - LPF */
	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~(0x3 << 0);
	val |= (0x1 << 0);
	val &= ~(0x7 << 2);
	val |= (0x5 << 2);
	val &= ~BITS(6, 7);
	val &= ~(0x7 << 8);
	val |= (0x3 << 8);
	val |= BIT(29);
	val &= ~BITS(12, 13);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - ICO */
	val = an8855_read(priv, PLL_CTRL_4);
	val |= BIT(2);
	an8855_write(priv, PLL_CTRL_4, val);

	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~BIT(14);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - CHP */
	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~(0xf << 16);
	val |= (0x6 << 16);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - PFD */
	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~(0x3 << 20);
	val |= (0x1 << 20);
	val &= ~(0x3 << 24);
	val |= (0x1 << 24);
	val &= ~BIT(26);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - POSTDIV */
	val = an8855_read(priv, PLL_CTRL_2);
	val |= BIT(22);
	val &= ~BIT(27);
	val &= ~BIT(28);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - SDM */
	val = an8855_read(priv, PLL_CTRL_4);
	val &= ~BITS(3, 4);
	an8855_write(priv, PLL_CTRL_4, val);

	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~BIT(30);
	an8855_write(priv, PLL_CTRL_2, val);

	val = an8855_read(priv, SS_LCPLL_PWCTL_SETTING_2);
	val &= ~(0x3 << 16);
	val |= (0x1 << 16);
	an8855_write(priv, SS_LCPLL_PWCTL_SETTING_2, val);

	an8855_write(priv, SS_LCPLL_TDC_FLT_2, 0x7a000000);
	an8855_write(priv, SS_LCPLL_TDC_PCW_1, 0x7a000000);

	val = an8855_read(priv, SS_LCPLL_TDC_FLT_5);
	val &= ~BIT(24);
	an8855_write(priv, SS_LCPLL_TDC_FLT_5, val);

	val = an8855_read(priv, PLL_CK_CTRL_0);
	val &= ~BIT(8);
	an8855_write(priv, PLL_CK_CTRL_0, val);

	/* PLL - SS */
	val = an8855_read(priv, PLL_CTRL_3);
	val &= ~BITS(0, 15);
	an8855_write(priv, PLL_CTRL_3, val);

	val = an8855_read(priv, PLL_CTRL_4);
	val &= ~BITS(0, 1);
	an8855_write(priv, PLL_CTRL_4, val);

	val = an8855_read(priv, PLL_CTRL_3);
	val &= ~BITS(16, 31);
	an8855_write(priv, PLL_CTRL_3, val);

	/* PLL - TDC */
	val = an8855_read(priv, PLL_CK_CTRL_0);
	val &= ~BIT(9);
	an8855_write(priv, PLL_CK_CTRL_0, val);

	val = an8855_read(priv, RG_QP_PLL_SDM_ORD);
	val |= BIT(3);
	val |= BIT(4);
	an8855_write(priv, RG_QP_PLL_SDM_ORD, val);

	val = an8855_read(priv, RG_QP_RX_DAC_EN);
	val &= ~(0x3 << 16);
	val |= (0x2 << 16);
	an8855_write(priv, RG_QP_RX_DAC_EN, val);

	/* TCL Disable (only for Co-SIM) */
	val = an8855_read(priv, PON_RXFEDIG_CTRL_0);
	val &= ~BIT(12);
	an8855_write(priv, PON_RXFEDIG_CTRL_0, val);

	/* TX Init */
	val = an8855_read(priv, RG_QP_TX_MODE_16B_EN);
	val &= ~BIT(0);
	val &= ~(0xffff << 16);
	val |= (0x4 << 16);
	an8855_write(priv, RG_QP_TX_MODE_16B_EN, val);

	/* RX Control */
	val = an8855_read(priv, RG_QP_RXAFE_RESERVE);
	val |= BIT(11);
	an8855_write(priv, RG_QP_RXAFE_RESERVE, val);

	val = an8855_read(priv, RG_QP_CDR_LPF_MJV_LIM);
	val &= ~(0x3 << 4);
	val |= (0x1 << 4);
	an8855_write(priv, RG_QP_CDR_LPF_MJV_LIM, val);

	val = an8855_read(priv, RG_QP_CDR_LPF_SETVALUE);
	val &= ~(0xf << 25);
	val |= (0x1 << 25);
	val &= ~(0x7 << 29);
	val |= (0x6 << 29);
	an8855_write(priv, RG_QP_CDR_LPF_SETVALUE, val);

	val = an8855_read(priv, RG_QP_CDR_PR_CKREF_DIV1);
	val &= ~(0x1f << 8);
	val |= (0xf << 8);
	an8855_write(priv, RG_QP_CDR_PR_CKREF_DIV1, val);

	val = an8855_read(priv, RG_QP_CDR_PR_KBAND_DIV_PCIE);
	val &= ~(0x3f << 0);
	val |= (0x19 << 0);
	val &= ~BIT(6);
	an8855_write(priv, RG_QP_CDR_PR_KBAND_DIV_PCIE, val);

	val = an8855_read(priv, RG_QP_CDR_FORCE_IBANDLPF_R_OFF);
	val &= ~(0x7f << 6);
	val |= (0x21 << 6);
	val &= ~(0x3 << 16);
	val |= (0x2 << 16);
	val &= ~BIT(13);
	an8855_write(priv, RG_QP_CDR_FORCE_IBANDLPF_R_OFF, val);

	val = an8855_read(priv, RG_QP_CDR_PR_KBAND_DIV_PCIE);
	val &= ~BIT(30);
	an8855_write(priv, RG_QP_CDR_PR_KBAND_DIV_PCIE, val);

	val = an8855_read(priv, RG_QP_CDR_PR_CKREF_DIV1);
	val &= ~(0x7 << 24);
	val |= (0x4 << 24);
	an8855_write(priv, RG_QP_CDR_PR_CKREF_DIV1, val);

	val = an8855_read(priv, RX_CTRL_26);
	val |= BIT(23);
	val &= ~BIT(24);
	val |= BIT(26);
	an8855_write(priv, RX_CTRL_26, val);

	val = an8855_read(priv, RX_DLY_0);
	val &= ~(0xff << 0);
	val |= (0x6f << 0);
	val |= BITS(8, 13);
	an8855_write(priv, RX_DLY_0, val);

	val = an8855_read(priv, RX_CTRL_42);
	val &= ~(0x1fff << 0);
	val |= (0x150 << 0);
	an8855_write(priv, RX_CTRL_42, val);

	val = an8855_read(priv, RX_CTRL_2);
	val &= ~(0x1fff << 16);
	val |= (0x150 << 16);
	an8855_write(priv, RX_CTRL_2, val);

	val = an8855_read(priv, PON_RXFEDIG_CTRL_9);
	val &= ~(0x7 << 0);
	val |= (0x1 << 0);
	an8855_write(priv, PON_RXFEDIG_CTRL_9, val);

	val = an8855_read(priv, RX_CTRL_8);
	val &= ~(0xfff << 16);
	val |= (0x200 << 16);
	val &= ~(0x7fff << 0);
	val |= (0xfff << 0);
	an8855_write(priv, RX_CTRL_8, val);

	/* Frequency memter */
	val = an8855_read(priv, RX_CTRL_5);
	val &= ~(0xfffff << 10);
	val |= (0x10 << 10);
	an8855_write(priv, RX_CTRL_5, val);

	val = an8855_read(priv, RX_CTRL_6);
	val &= ~(0xfffff << 0);
	val |= (0x64 << 0);
	an8855_write(priv, RX_CTRL_6, val);

	val = an8855_read(priv, RX_CTRL_7);
	val &= ~(0xfffff << 0);
	val |= (0x2710 << 0);
	an8855_write(priv, RX_CTRL_7, val);

	val = an8855_read(priv, PLL_CTRL_0);
	val |= BIT(0);
	an8855_write(priv, PLL_CTRL_0, val);

	/* PCS Init */
	val = an8855_read(priv, RG_HSGMII_PCS_CTROL_1);
	val &= ~BIT(30);
	an8855_write(priv, RG_HSGMII_PCS_CTROL_1, val);

	/* Rate Adaption */
	val = an8855_read(priv, RATE_ADP_P0_CTRL_0);
	val &= ~BIT(31);
	an8855_write(priv, RATE_ADP_P0_CTRL_0, val);

	val = an8855_read(priv, RG_RATE_ADAPT_CTRL_0);
	val |= BIT(0);
	val |= BIT(4);
	val |= BITS(26, 27);
	an8855_write(priv, RG_RATE_ADAPT_CTRL_0, val);

	/* Disable AN */
	val = an8855_read(priv, SGMII_REG_AN0);
	val &= ~BIT(12);
	an8855_write(priv, SGMII_REG_AN0, val);

	/* Force Speed */
	val = an8855_read(priv, SGMII_STS_CTRL_0);
	val |= BIT(2);
	val |= BITS(4, 5);
	an8855_write(priv, SGMII_STS_CTRL_0, val);

	/* bypass flow control to MAC */
	an8855_write(priv, MSG_RX_LIK_STS_0, 0x01010107);
	an8855_write(priv, MSG_RX_LIK_STS_2, 0x00000EEF);

	return 0;
}

static int
an8855_sgmii_setup(struct an8855_priv *priv, int mode)
{
	u32 val = 0;

	/* TX FIR - improve TX EYE */
	val = an8855_read(priv, INTF_CTRL_10);
	val &= ~(0x3f << 16);
	val |= BIT(21);
	val &= ~(0x1f << 24);
	val |= BIT(29);
	an8855_write(priv, INTF_CTRL_10, val);

	val = an8855_read(priv, INTF_CTRL_11);
	val &= ~(0x3f);
	val |= (0xd << 0);
	val |= BIT(6);
	an8855_write(priv, INTF_CTRL_11, val);

	/* RX CDR - improve RX Jitter Tolerance */
	val = an8855_read(priv, RG_QP_CDR_LPF_BOT_LIM);
	val &= ~(0x7 << 24);
	val |= (0x6 << 24);
	val &= ~(0x7 << 20);
	val |= (0x6 << 20);
	an8855_write(priv, RG_QP_CDR_LPF_BOT_LIM, val);

	/* PMA Init */
	/* PLL */
	val = an8855_read(priv, QP_DIG_MODE_CTRL_1);
	val &= ~BITS(2, 3);
	an8855_write(priv, QP_DIG_MODE_CTRL_1, val);

	/* PLL - LPF */
	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~(0x3 << 0);
	val |= (0x1 << 0);
	val &= ~(0x7 << 2);
	val |= (0x5 << 2);
	val &= ~BITS(6, 7);
	val &= ~(0x7 << 8);
	val |= (0x3 << 8);
	val |= BIT(29);
	val &= ~BITS(12, 13);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - ICO */
	val = an8855_read(priv, PLL_CTRL_4);
	val |= BIT(2);
	an8855_write(priv, PLL_CTRL_4, val);

	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~BIT(14);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - CHP */
	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~(0xf << 16);
	val |= (0x4 << 16);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - PFD */
	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~(0x3 << 20);
	val |= (0x1 << 20);
	val &= ~(0x3 << 24);
	val |= (0x1 << 24);
	val &= ~BIT(26);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - POSTDIV */
	val = an8855_read(priv, PLL_CTRL_2);
	val |= BIT(22);
	val &= ~BIT(27);
	val &= ~BIT(28);
	an8855_write(priv, PLL_CTRL_2, val);

	/* PLL - SDM */
	val = an8855_read(priv, PLL_CTRL_4);
	val &= ~BITS(3, 4);
	an8855_write(priv, PLL_CTRL_4, val);

	val = an8855_read(priv, PLL_CTRL_2);
	val &= ~BIT(30);
	an8855_write(priv, PLL_CTRL_2, val);

	val = an8855_read(priv, SS_LCPLL_PWCTL_SETTING_2);
	val &= ~(0x3 << 16);
	val |= (0x1 << 16);
	an8855_write(priv, SS_LCPLL_PWCTL_SETTING_2, val);

	an8855_write(priv, SS_LCPLL_TDC_FLT_2, 0x48000000);
	an8855_write(priv, SS_LCPLL_TDC_PCW_1, 0x48000000);

	val = an8855_read(priv, SS_LCPLL_TDC_FLT_5);
	val &= ~BIT(24);
	an8855_write(priv, SS_LCPLL_TDC_FLT_5, val);

	val = an8855_read(priv, PLL_CK_CTRL_0);
	val &= ~BIT(8);
	an8855_write(priv, PLL_CK_CTRL_0, val);

	/* PLL - SS */
	val = an8855_read(priv, PLL_CTRL_3);
	val &= ~BITS(0, 15);
	an8855_write(priv, PLL_CTRL_3, val);

	val = an8855_read(priv, PLL_CTRL_4);
	val &= ~BITS(0, 1);
	an8855_write(priv, PLL_CTRL_4, val);

	val = an8855_read(priv, PLL_CTRL_3);
	val &= ~BITS(16, 31);
	an8855_write(priv, PLL_CTRL_3, val);

	/* PLL - TDC */
	val = an8855_read(priv, PLL_CK_CTRL_0);
	val &= ~BIT(9);
	an8855_write(priv, PLL_CK_CTRL_0, val);

	val = an8855_read(priv, RG_QP_PLL_SDM_ORD);
	val |= BIT(3);
	val |= BIT(4);
	an8855_write(priv, RG_QP_PLL_SDM_ORD, val);

	val = an8855_read(priv, RG_QP_RX_DAC_EN);
	val &= ~(0x3 << 16);
	val |= (0x2 << 16);
	an8855_write(priv, RG_QP_RX_DAC_EN, val);

	/* PLL - TCL Disable (only for Co-SIM) */
	val = an8855_read(priv, PON_RXFEDIG_CTRL_0);
	val &= ~BIT(12);
	an8855_write(priv, PON_RXFEDIG_CTRL_0, val);

	/* TX Init */
	val = an8855_read(priv, RG_QP_TX_MODE_16B_EN);
	val &= ~BIT(0);
	val &= ~BITS(16, 31);
	an8855_write(priv, RG_QP_TX_MODE_16B_EN, val);

	/* RX Init */
	val = an8855_read(priv, RG_QP_RXAFE_RESERVE);
	val |= BIT(11);
	an8855_write(priv, RG_QP_RXAFE_RESERVE, val);

	val = an8855_read(priv, RG_QP_CDR_LPF_MJV_LIM);
	val &= ~(0x3 << 4);
	val |= (0x2 << 4);
	an8855_write(priv, RG_QP_CDR_LPF_MJV_LIM, val);

	val = an8855_read(priv, RG_QP_CDR_LPF_SETVALUE);
	val &= ~(0xf << 25);
	val |= (0x1 << 25);
	val &= ~(0x7 << 29);
	val |= (0x6 << 29);
	an8855_write(priv, RG_QP_CDR_LPF_SETVALUE, val);

	val = an8855_read(priv, RG_QP_CDR_PR_CKREF_DIV1);
	val &= ~(0x1f << 8);
	val |= (0xc << 8);
	an8855_write(priv, RG_QP_CDR_PR_CKREF_DIV1, val);

	val = an8855_read(priv, RG_QP_CDR_PR_KBAND_DIV_PCIE);
	val &= ~(0x3f << 0);
	val |= (0x19 << 0);
	val &= ~BIT(6);
	an8855_write(priv, RG_QP_CDR_PR_KBAND_DIV_PCIE, val);

	val = an8855_read(priv, RG_QP_CDR_FORCE_IBANDLPF_R_OFF);
	val &= ~(0x7f << 6);
	val |= (0x21 << 6);
	val &= ~(0x3 << 16);
	val |= (0x2 << 16);
	val &= ~BIT(13);
	an8855_write(priv, RG_QP_CDR_FORCE_IBANDLPF_R_OFF, val);

	val = an8855_read(priv, RG_QP_CDR_PR_KBAND_DIV_PCIE);
	val &= ~BIT(30);
	an8855_write(priv, RG_QP_CDR_PR_KBAND_DIV_PCIE, val);

	val = an8855_read(priv, RG_QP_CDR_PR_CKREF_DIV1);
	val &= ~(0x7 << 24);
	val |= (0x4 << 24);
	an8855_write(priv, RG_QP_CDR_PR_CKREF_DIV1, val);

	val = an8855_read(priv, RX_CTRL_26);
	val |= BIT(23);
	val &= ~BIT(24);
		val |= BIT(26);
	an8855_write(priv, RX_CTRL_26, val);

	val = an8855_read(priv, RX_DLY_0);
	val &= ~(0xff << 0);
	val |= (0x6f << 0);
	val |= BITS(8, 13);
	an8855_write(priv, RX_DLY_0, val);

	val = an8855_read(priv, RX_CTRL_42);
	val &= ~(0x1fff << 0);
	val |= (0x150 << 0);
	an8855_write(priv, RX_CTRL_42, val);

	val = an8855_read(priv, RX_CTRL_2);
	val &= ~(0x1fff << 16);
	val |= (0x150 << 16);
	an8855_write(priv, RX_CTRL_2, val);

	val = an8855_read(priv, PON_RXFEDIG_CTRL_9);
	val &= ~(0x7 << 0);
	val |= (0x1 << 0);
	an8855_write(priv, PON_RXFEDIG_CTRL_9, val);

	val = an8855_read(priv, RX_CTRL_8);
	val &= ~(0xfff << 16);
	val |= (0x200 << 16);
	val &= ~(0x7fff << 0);
	val |= (0xfff << 0);
	an8855_write(priv, RX_CTRL_8, val);

	/* Frequency memter */
	val = an8855_read(priv, RX_CTRL_5);
	val &= ~(0xfffff << 10);
	val |= (0x28 << 10);
	an8855_write(priv, RX_CTRL_5, val);

	val = an8855_read(priv, RX_CTRL_6);
	val &= ~(0xfffff << 0);
	val |= (0x64 << 0);
	an8855_write(priv, RX_CTRL_6, val);

	val = an8855_read(priv, RX_CTRL_7);
	val &= ~(0xfffff << 0);
	val |= (0x2710 << 0);
	an8855_write(priv, RX_CTRL_7, val);

	val = an8855_read(priv, PLL_CTRL_0);
	val |= BIT(0);
	an8855_write(priv, PLL_CTRL_0, val);

	if (mode == SGMII_MODE_FORCE) {
		/* PCS Init */
		val = an8855_read(priv, QP_DIG_MODE_CTRL_0);
		val &= ~BIT(0);
		val &= ~BITS(4, 5);
		an8855_write(priv, QP_DIG_MODE_CTRL_0, val);

		val = an8855_read(priv, RG_HSGMII_PCS_CTROL_1);
		val &= ~BIT(30);
		an8855_write(priv, RG_HSGMII_PCS_CTROL_1, val);

		/* Rate Adaption - GMII path config. */
		val = an8855_read(priv, RG_AN_SGMII_MODE_FORCE);
		val |= BIT(0);
		val &= ~BITS(4, 5);
		an8855_write(priv, RG_AN_SGMII_MODE_FORCE, val);

		val = an8855_read(priv, SGMII_STS_CTRL_0);
		val |= BIT(2);
		val &= ~(0x3 << 4);
		val |= (0x2 << 4);
		an8855_write(priv, SGMII_STS_CTRL_0, val);

		val = an8855_read(priv, SGMII_REG_AN0);
		val &= ~BIT(12);
		an8855_write(priv, SGMII_REG_AN0, val);

		val = an8855_read(priv, PHY_RX_FORCE_CTRL_0);
		val |= BIT(4);
		an8855_write(priv, PHY_RX_FORCE_CTRL_0, val);

		val = an8855_read(priv, RATE_ADP_P0_CTRL_0);
		val &= ~BITS(0, 3);
		val |= BIT(28);
		an8855_write(priv, RATE_ADP_P0_CTRL_0, val);

		val = an8855_read(priv, RG_RATE_ADAPT_CTRL_0);
		val |= BIT(0);
		val |= BIT(4);
		val |= BITS(26, 27);
		an8855_write(priv, RG_RATE_ADAPT_CTRL_0, val);
	} else {
		/* PCS Init */
		val = an8855_read(priv, RG_HSGMII_PCS_CTROL_1);
		val &= ~BIT(30);
		an8855_write(priv, RG_HSGMII_PCS_CTROL_1, val);

		/* Set AN Ability - Interrupt */
		val = an8855_read(priv, SGMII_REG_AN_FORCE_CL37);
		val |= BIT(0);
		an8855_write(priv, SGMII_REG_AN_FORCE_CL37, val);

		val = an8855_read(priv, SGMII_REG_AN_13);
		val &= ~(0x3f << 0);
		val |= (0xb << 0);
		val |= BIT(8);
		an8855_write(priv, SGMII_REG_AN_13, val);

		/* Rate Adaption - GMII path config. */
		val = an8855_read(priv, SGMII_REG_AN0);
		val |= BIT(12);
		an8855_write(priv, SGMII_REG_AN0, val);

		val = an8855_read(priv, MII_RA_AN_ENABLE);
		val |= BIT(0);
		an8855_write(priv, MII_RA_AN_ENABLE, val);

		val = an8855_read(priv, RATE_ADP_P0_CTRL_0);
		val |= BIT(28);
		an8855_write(priv, RATE_ADP_P0_CTRL_0, val);

		val = an8855_read(priv, RG_RATE_ADAPT_CTRL_0);
		val |= BIT(0);
		val |= BIT(4);
		val |= BITS(26, 27);
		an8855_write(priv, RG_RATE_ADAPT_CTRL_0, val);

		/* Only for Co-SIM */

		/* AN Speed up (Only for Co-SIM) */

		/* Restart AN */
		val = an8855_read(priv, SGMII_REG_AN0);
		val |= BIT(9);
		an8855_write(priv, SGMII_REG_AN0, val);
	}

	/* bypass flow control to MAC */
	an8855_write(priv, MSG_RX_LIK_STS_0, 0x01010107);
	an8855_write(priv, MSG_RX_LIK_STS_2, 0x00000EEF);

	return 0;
}

static int
an8855_sgmii_setup_mode_an(struct an8855_priv *priv, int port,
				      phy_interface_t interface)
{
	return an8855_sgmii_setup(priv, SGMII_MODE_AN);
}

static int
an8855_mac_config(struct dsa_switch *ds, int port, unsigned int mode,
		  phy_interface_t interface)
{
	struct an8855_priv *priv = ds->priv;
	struct phy_device *phydev;
	const struct dsa_port *dp;

	if (!an8855_is_mac_port(port)) {
		dev_err(priv->dev, "port %d is not a MAC port\n", port);
		return -EINVAL;
	}

	switch (interface) {
	case PHY_INTERFACE_MODE_RGMII:
		dp = dsa_to_port(ds, port);
		phydev = (dp->slave) ? dp->slave->phydev : NULL;
		return an8855_rgmii_setup(priv, port, interface, phydev);
	case PHY_INTERFACE_MODE_SGMII:
		return an8855_sgmii_setup_mode_an(priv, port, interface);
	case PHY_INTERFACE_MODE_2500BASEX:
		if (phylink_autoneg_inband(mode))
			return -EINVAL;
		return an8855_set_hsgmii_mode(priv);
	default:
		return -EINVAL;
	}

	return -EINVAL;
}

static struct phylink_pcs *
an8855_phylink_mac_select_pcs(struct dsa_switch *ds, int port,
			      phy_interface_t interface)
{
	struct an8855_priv *priv = ds->priv;
	switch (interface) {
	case PHY_INTERFACE_MODE_TRGMII:
	case PHY_INTERFACE_MODE_SGMII:
	case PHY_INTERFACE_MODE_1000BASEX:
	case PHY_INTERFACE_MODE_2500BASEX:
		return &priv->pcs[port].pcs;
	default:
		return NULL;
	}
}

static void an8855_phylink_pcs_link_up(struct phylink_pcs *pcs,
				       unsigned int mode,
				       phy_interface_t interface,
				       int speed, int duplex)
{
	if (pcs->ops->pcs_link_up)
		pcs->ops->pcs_link_up(pcs, mode, interface, speed, duplex);
}

static void
an8855_phylink_mac_link_up(struct dsa_switch *ds, int port,
				       unsigned int mode,
				       phy_interface_t interface,
				       struct phy_device *phydev,
				       int speed, int duplex,
				       bool tx_pause, bool rx_pause)
{
	struct an8855_priv *priv = ds->priv;
	u32 mcr;

	mcr = an8855_read(priv, AN8855_PMCR_P(port));
	mcr |= PMCR_RX_EN | PMCR_TX_EN | PMCR_FORCE_LNK;
	mcr &=
	    ~(PMCR_FORCE_FDX | PMCR_SPEED_MASK | PMCR_TX_FC_EN | PMCR_RX_FC_EN);

	if (interface == PHY_INTERFACE_MODE_RGMII
	    || interface == PHY_INTERFACE_MODE_SGMII) {
		speed = SPEED_1000;
		duplex = DUPLEX_FULL;
	} else if (interface == PHY_INTERFACE_MODE_2500BASEX) {
		speed = SPEED_2500;
		duplex = DUPLEX_FULL;
	}

	switch (speed) {
	case SPEED_2500:
		mcr |= PMCR_FORCE_SPEED_2500;
		break;
	case SPEED_1000:
		mcr |= PMCR_FORCE_SPEED_1000;
		if (priv->eee_enable & BIT(port))
			mcr |= PMCR_FORCE_EEE1G;
		break;
	case SPEED_100:
		mcr |= PMCR_FORCE_SPEED_100;
		if (priv->eee_enable & BIT(port))
			mcr |= PMCR_FORCE_EEE100;
		break;
	}
	if (duplex == DUPLEX_FULL) {
		mcr |= PMCR_FORCE_FDX;
		if (tx_pause)
			mcr |= PMCR_TX_FC_EN;
		if (rx_pause)
			mcr |= PMCR_RX_FC_EN;
	}

	an8855_write(priv, AN8855_PMCR_P(port), mcr);
}

static int
an8855_cpu_port_config(struct dsa_switch *ds, int port)
{
	struct an8855_priv *priv = ds->priv;
	phy_interface_t interface = PHY_INTERFACE_MODE_NA;
	int speed;

	switch (port) {
	case 5:
		interface = PHY_INTERFACE_MODE_2500BASEX;

		priv->p5_interface = interface;
		break;
	};
	if (interface == PHY_INTERFACE_MODE_NA)
		dev_err(priv->dev, "invalid interface\n");

	if (interface == PHY_INTERFACE_MODE_2500BASEX)
		speed = SPEED_2500;
	else
		speed = SPEED_1000;

	an8855_mac_config(ds, port, MLO_AN_FIXED, interface);
	an8855_write(priv, AN8855_PMCR_P(port),
		     PMCR_CPU_PORT_SETTING(priv->id));
	an8855_phylink_pcs_link_up(&priv->pcs[port].pcs, MLO_AN_FIXED,
				   interface, speed, DUPLEX_FULL);
	an8855_phylink_mac_link_up(ds, port, MLO_AN_FIXED, interface, NULL,
				   speed, DUPLEX_FULL, true, true);

	return 0;
}

static void
an8855_mac_port_validate(struct dsa_switch *ds, int port,
				     unsigned long *supported)
{
	struct an8855_priv *priv = ds->priv;

	an8855_sgmii_validate(priv, port, supported);
}

static int
an8855_pcs_validate(struct phylink_pcs *pcs,
			unsigned long *supported,
			const struct phylink_link_state *state)
{
	/* Autonegotiation is not supported in TRGMII nor 802.3z modes */
	if (state->interface == PHY_INTERFACE_MODE_TRGMII ||
	    phy_interface_mode_is_8023z(state->interface))
		phylink_clear(supported, Autoneg);

	return 0;
}

static int
an8855_get_mac_eee(struct dsa_switch *ds, int port,
			      struct ethtool_eee *e)
{
	struct an8855_priv *priv = ds->priv;
	u32 eeecr, pmsr, ckgcr;

	e->eee_enabled = !!(priv->eee_enable & BIT(port));

	if (e->eee_enabled) {
		eeecr = an8855_read(priv, AN8855_PMEEECR_P(port));
		e->tx_lpi_enabled = !(eeecr & LPI_MODE_EN);
		ckgcr = an8855_read(priv, AN8855_CKGCR);
		e->tx_lpi_timer =
		    ((ckgcr & LPI_TXIDLE_THD_MASK) >> LPI_TXIDLE_THD) / 500;
		pmsr = an8855_read(priv, AN8855_PMSR_P(port));
		e->eee_active = e->eee_enabled
		    && !!(pmsr & (PMSR_EEE1G | PMSR_EEE100M));
	} else {
		e->tx_lpi_enabled = 0;
		e->tx_lpi_timer = 0;
		e->eee_active = 0;
	}
	return 0;
}

static int
an8855_set_mac_eee(struct dsa_switch *ds, int port,
			      struct ethtool_eee *e)
{
	struct an8855_priv *priv = ds->priv;
	u32 eeecr;

	if (e->eee_enabled) {
		priv->eee_enable |= BIT(port);
		eeecr = an8855_read(priv, AN8855_PMEEECR_P(port));
		eeecr &= ~LPI_MODE_EN;
		if (e->tx_lpi_enabled)
			eeecr |= LPI_MODE_EN;
		an8855_write(priv, AN8855_PMEEECR_P(port), eeecr);
	} else {
		priv->eee_enable &= ~(BIT(port));
		eeecr = an8855_read(priv, AN8855_PMEEECR_P(port));
		eeecr &= ~LPI_MODE_EN;
		an8855_write(priv, AN8855_PMEEECR_P(port), eeecr);
	}

	return 0;
}

static void
an8855_pcs_get_state(struct phylink_pcs *pcs,
			      struct phylink_link_state *state)
{
	struct an8855_priv *priv = pcs_to_an8855_pcs(pcs)->priv;
	int port = pcs_to_an8855_pcs(pcs)->port;
	u32 pmsr;

	if (port < 0 || port >= AN8855_NUM_PORTS)
		return;

	pmsr = an8855_read(priv, AN8855_PMSR_P(port));

	state->link = (pmsr & PMSR_LINK);
	state->an_complete = state->link;
	state->duplex = !!(pmsr & PMSR_DPX);

	switch (pmsr & PMSR_SPEED_MASK) {
	case PMSR_SPEED_10:
		state->speed = SPEED_10;
		break;
	case PMSR_SPEED_100:
		state->speed = SPEED_100;
		break;
	case PMSR_SPEED_1000:
		state->speed = SPEED_1000;
		break;
	case PMSR_SPEED_2500:
		state->speed = SPEED_2500;
		break;
	default:
		state->speed = SPEED_UNKNOWN;
		break;
	}

	state->pause &= ~(MLO_PAUSE_RX | MLO_PAUSE_TX);
	if (pmsr & PMSR_RX_FC)
		state->pause |= MLO_PAUSE_RX;
	if (pmsr & PMSR_TX_FC)
		state->pause |= MLO_PAUSE_TX;
}

static int an8855_pcs_config(struct phylink_pcs *pcs, unsigned int mode,
			     phy_interface_t interface,
			     const unsigned long *advertising,
			     bool permit_pause_to_mac)
{
	return 0;
}

static void an8855_pcs_an_restart(struct phylink_pcs *pcs)
{
}

static const struct phylink_pcs_ops an8855_pcs_ops = {
	.pcs_validate = an8855_pcs_validate,
	.pcs_get_state = an8855_pcs_get_state,
	.pcs_config = an8855_pcs_config,
	.pcs_an_restart = an8855_pcs_an_restart,
};

static int
an8855_sw_setup(struct dsa_switch *ds)
{
	struct an8855_priv *priv = ds->priv;
	int i;
	for (i = 0; i < priv->ds->num_ports; i++) {
		priv->pcs[i].pcs.ops = priv->info->pcs_ops;
		priv->pcs[i].pcs.neg_mode = true;
		priv->pcs[i].priv = priv;
		priv->pcs[i].port = i;
	}

	int ret = priv->info->sw_setup(ds);
	return ret;
}

static int
an8855_sw_phy_read(struct dsa_switch *ds, int port, int regnum)
{
	struct an8855_priv *priv = ds->priv;

	return priv->info->phy_read(ds, port, regnum);
}

static int
an8855_sw_phy_write(struct dsa_switch *ds, int port, int regnum, u16 val)
{
	struct an8855_priv *priv = ds->priv;

	return priv->info->phy_write(ds, port, regnum, val);
}

static int an8855_proc_device_read(struct seq_file *seq, void *v)
{
	seq_printf(seq, "%s\n", ARHT_CHIP_NAME);

	return 0;
}

static int an8855_proc_device_open(struct inode *inode, struct file *file)
{
	return single_open(file, an8855_proc_device_read, 0);
}

static const struct proc_ops an8855_proc_device_fops = {
	.proc_open	= an8855_proc_device_open,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_release	= single_release,
};

static int an8855_proc_device_init(void)
{
	if (!proc_an8855_dsa_dir)
		proc_an8855_dsa_dir = proc_mkdir(ARHT_PROC_DIR, 0);

	proc_create(ARHT_PROC_NODE_DEVICE, 0400, proc_an8855_dsa_dir,
			&an8855_proc_device_fops);

	return 0;
}

static void an8855_proc_device_exit(void)
{
	remove_proc_entry(ARHT_PROC_NODE_DEVICE, 0);
}

static const struct dsa_switch_ops an8855_switch_ops = {
	.get_tag_protocol = air_get_tag_protocol,
	.setup = an8855_sw_setup,
	.get_strings = an8855_get_strings,
	.phy_read = an8855_sw_phy_read,
	.phy_write = an8855_sw_phy_write,
	.get_ethtool_stats = an8855_get_ethtool_stats,
	.get_sset_count = an8855_get_sset_count,
	.port_enable = an8855_port_enable,
	.port_disable = an8855_port_disable,
	.port_stp_state_set = an8855_stp_state_set,
	.port_bridge_join = an8855_port_bridge_join,
	.port_bridge_leave = an8855_port_bridge_leave,
	.port_fdb_add = an8855_port_fdb_add,
	.port_fdb_del = an8855_port_fdb_del,
	.port_fdb_dump = an8855_port_fdb_dump,
	.port_vlan_filtering = an8855_port_vlan_filtering,
	.port_vlan_add = an8855_port_vlan_add,
	.port_vlan_del = an8855_port_vlan_del,
	.port_mirror_add = an8855_port_mirror_add,
	.port_mirror_del = an8855_port_mirror_del,
	.phylink_mac_select_pcs	= an8855_phylink_mac_select_pcs,
	.get_mac_eee = an8855_get_mac_eee,
	.set_mac_eee = an8855_set_mac_eee,
};

static const struct an8855_dev_info an8855_table[] = {
	[ID_AN8855] = {
		.id = ID_AN8855,
		.pcs_ops = &an8855_pcs_ops,
		.sw_setup = an8855_setup,
		.phy_read = an8855_phy_read,
		.phy_write = an8855_phy_write,
		.pad_setup = an8855_pad_setup,
		.cpu_port_config = an8855_cpu_port_config,
		.phy_mode_supported = an8855_phy_supported,
		.mac_port_validate = an8855_mac_port_validate,
		.mac_port_config = an8855_mac_config,
	},
};

static const struct of_device_id an8855_of_match[] = {
	{.compatible = "airoha,an8855", .data = &an8855_table[ID_AN8855],
	},
	{ /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, an8855_of_match);

static int
an8855_probe(struct mdio_device *mdiodev)
{
	struct an8855_priv *priv;
	struct device_node *dn;
	struct device_node *switch_node = NULL;
	int ret;

	dn = mdiodev->dev.of_node;

	priv = devm_kzalloc(&mdiodev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->ds = devm_kzalloc(&mdiodev->dev, sizeof(*priv->ds), GFP_KERNEL);
	if (!priv->ds)
		return -ENOMEM;

	priv->ds->dev = &mdiodev->dev;
	priv->ds->num_ports = AN8855_NUM_PORTS;

	/* Get the hardware identifier from the devicetree node.
	 * We will need it for some of the clock and regulator setup.
	 */
	priv->info = of_device_get_match_data(&mdiodev->dev);
	if (!priv->info)
		return -EINVAL;

	/* Sanity check if these required device operations are filled
	 * properly.
	 */
	if (!priv->info->sw_setup || !priv->info->pad_setup ||
	    !priv->info->phy_read || !priv->info->phy_write ||
	    !priv->info->phy_mode_supported ||
	    !priv->info->mac_port_validate ||
	    !priv->info->mac_port_config)
		return -EINVAL;

	dev_info(&mdiodev->dev, "Airoha AN8855 DSA driver, version %s\n",
			ARHT_AN8855_DSA_DRIVER_VER);
	priv->phy_base = AN8855_GPHY_SMI_ADDR_DEFAULT;
	priv->id = priv->info->id;

	priv->reset = devm_gpiod_get_optional(&mdiodev->dev, "reset",
					      GPIOD_OUT_LOW);
	if (IS_ERR(priv->reset)) {
		dev_err(&mdiodev->dev, "Couldn't get our reset line\n");
		return PTR_ERR(priv->reset);
	}

	switch_node = of_find_node_by_name(NULL, "switch0");
	if (switch_node) {
		priv->base = of_iomap(switch_node, 0);
		if (priv->base == NULL) {
			dev_err(&mdiodev->dev, "of_iomap failed\n");
			return -ENOMEM;
		}
	}

	ret = of_property_read_u32(dn, "changesmiaddr", &priv->phy_base_new);
	if ((ret < 0) || (priv->phy_base_new > 0x1f))
		priv->phy_base_new = -1;

	/* Assign AN8855 interrupt pin */
	if (of_property_read_u32(dn, "airoha,intr", &priv->intr_pin))
		priv->intr_pin = AN8855_DFL_INTR_ID;

	if (of_property_read_u32(dn, "airoha,extSurge", &priv->extSurge))
		priv->extSurge = AN8855_DFL_EXT_SURGE;

	priv->bus = mdiodev->bus;
	priv->dev = &mdiodev->dev;
	priv->ds->priv = priv;
	priv->ds->ops = &an8855_switch_ops;
	mutex_init(&priv->reg_mutex);
	dev_set_drvdata(&mdiodev->dev, priv);

	ret = dsa_register_switch(priv->ds);
	if (ret) {
		if (priv->base)
			iounmap(priv->base);

		return ret;
	}
	an8855_nl_init(&priv);

	an8855_proc_device_init();
	return 0;
}

static void
an8855_remove(struct mdio_device *mdiodev)
{
	struct an8855_priv *priv = dev_get_drvdata(&mdiodev->dev);

	dsa_unregister_switch(priv->ds);
	mutex_destroy(&priv->reg_mutex);

	if (priv->base)
		iounmap(priv->base);

	an8855_proc_device_exit();

	an8855_nl_exit();
}

static void an8855_shutdown(struct mdio_device *mdiodev)
{
	struct an8855_priv *priv = dev_get_drvdata(&mdiodev->dev);

	if (!priv)
		return;

	dsa_switch_shutdown(priv->ds);

	dev_set_drvdata(&mdiodev->dev, NULL);
}

static struct mdio_driver an8855_mdio_driver = {
	.probe = an8855_probe,
	.remove = an8855_remove,
	.shutdown = an8855_shutdown,
	.mdiodrv.driver = {
		.name = "an8855",
		.of_match_table = an8855_of_match,
	},
};

mdio_module_driver(an8855_mdio_driver);

MODULE_AUTHOR("Min Yao <min.yao@airoha.com>");
MODULE_DESCRIPTION("Driver for Airoha AN8855 Switch");
MODULE_VERSION(ARHT_AN8855_DSA_DRIVER_VER);
MODULE_LICENSE("GPL");
