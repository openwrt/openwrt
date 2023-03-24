// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2009 Felix Fietkau <nbd@nbd.name>
 * Copyright (C) 2011-2012, 2020-2021 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (c) 2015, 2019, The Linux Foundation. All rights reserved.
 * Copyright (c) 2016 John Crispin <john@phrozen.org>
 * Copyright (c) 2021 Robert Marko <robert.marko@sartura.hr>
 */

#include <linux/version.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/mdio.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/phy.h>
#include <linux/phylink.h>
#include <linux/reset.h>
#include <net/dsa.h>

#include "qca8k-ipq4019.h"

#define MIB_DESC(_s, _o, _n)	\
	{			\
		.size = (_s),	\
		.offset = (_o),	\
		.name = (_n),	\
	}

static const struct qca8k_mib_desc ar8327_mib[] = {
	MIB_DESC(1, 0x00, "RxBroad"),
	MIB_DESC(1, 0x04, "RxPause"),
	MIB_DESC(1, 0x08, "RxMulti"),
	MIB_DESC(1, 0x0c, "RxFcsErr"),
	MIB_DESC(1, 0x10, "RxAlignErr"),
	MIB_DESC(1, 0x14, "RxRunt"),
	MIB_DESC(1, 0x18, "RxFragment"),
	MIB_DESC(1, 0x1c, "Rx64Byte"),
	MIB_DESC(1, 0x20, "Rx128Byte"),
	MIB_DESC(1, 0x24, "Rx256Byte"),
	MIB_DESC(1, 0x28, "Rx512Byte"),
	MIB_DESC(1, 0x2c, "Rx1024Byte"),
	MIB_DESC(1, 0x30, "Rx1518Byte"),
	MIB_DESC(1, 0x34, "RxMaxByte"),
	MIB_DESC(1, 0x38, "RxTooLong"),
	MIB_DESC(2, 0x3c, "RxGoodByte"),
	MIB_DESC(2, 0x44, "RxBadByte"),
	MIB_DESC(1, 0x4c, "RxOverFlow"),
	MIB_DESC(1, 0x50, "Filtered"),
	MIB_DESC(1, 0x54, "TxBroad"),
	MIB_DESC(1, 0x58, "TxPause"),
	MIB_DESC(1, 0x5c, "TxMulti"),
	MIB_DESC(1, 0x60, "TxUnderRun"),
	MIB_DESC(1, 0x64, "Tx64Byte"),
	MIB_DESC(1, 0x68, "Tx128Byte"),
	MIB_DESC(1, 0x6c, "Tx256Byte"),
	MIB_DESC(1, 0x70, "Tx512Byte"),
	MIB_DESC(1, 0x74, "Tx1024Byte"),
	MIB_DESC(1, 0x78, "Tx1518Byte"),
	MIB_DESC(1, 0x7c, "TxMaxByte"),
	MIB_DESC(1, 0x80, "TxOverSize"),
	MIB_DESC(2, 0x84, "TxByte"),
	MIB_DESC(1, 0x8c, "TxCollision"),
	MIB_DESC(1, 0x90, "TxAbortCol"),
	MIB_DESC(1, 0x94, "TxMultiCol"),
	MIB_DESC(1, 0x98, "TxSingleCol"),
	MIB_DESC(1, 0x9c, "TxExcDefer"),
	MIB_DESC(1, 0xa0, "TxDefer"),
	MIB_DESC(1, 0xa4, "TxLateCol"),
	MIB_DESC(1, 0xa8, "RXUnicast"),
	MIB_DESC(1, 0xac, "TXunicast"),
};

static int
qca8k_read(struct qca8k_priv *priv, u32 reg, u32 *val)
{
	return regmap_read(priv->regmap, reg, val);
}

static int
qca8k_write(struct qca8k_priv *priv, u32 reg, u32 val)
{
	return regmap_write(priv->regmap, reg, val);
}

static int
qca8k_rmw(struct qca8k_priv *priv, u32 reg, u32 mask, u32 write_val)
{
	return regmap_update_bits(priv->regmap, reg, mask, write_val);
}

static int
qca8k_reg_set(struct qca8k_priv *priv, u32 reg, u32 val)
{
	return regmap_set_bits(priv->regmap, reg, val);
}

static int
qca8k_reg_clear(struct qca8k_priv *priv, u32 reg, u32 val)
{
	return regmap_clear_bits(priv->regmap, reg, val);
}

static const struct regmap_range qca8k_readable_ranges[] = {
	regmap_reg_range(0x0000, 0x00e4), /* Global control */
	regmap_reg_range(0x0100, 0x0168), /* EEE control */
	regmap_reg_range(0x0200, 0x0270), /* Parser control */
	regmap_reg_range(0x0400, 0x0454), /* ACL */
	regmap_reg_range(0x0600, 0x0718), /* Lookup */
	regmap_reg_range(0x0800, 0x0b70), /* QM */
	regmap_reg_range(0x0c00, 0x0c80), /* PKT */
	regmap_reg_range(0x0e00, 0x0e98), /* L3 */
	regmap_reg_range(0x1000, 0x10ac), /* MIB - Port0 */
	regmap_reg_range(0x1100, 0x11ac), /* MIB - Port1 */
	regmap_reg_range(0x1200, 0x12ac), /* MIB - Port2 */
	regmap_reg_range(0x1300, 0x13ac), /* MIB - Port3 */
	regmap_reg_range(0x1400, 0x14ac), /* MIB - Port4 */
	regmap_reg_range(0x1500, 0x15ac), /* MIB - Port5 */
	regmap_reg_range(0x1600, 0x16ac), /* MIB - Port6 */

};

static const struct regmap_access_table qca8k_readable_table = {
	.yes_ranges = qca8k_readable_ranges,
	.n_yes_ranges = ARRAY_SIZE(qca8k_readable_ranges),
};

static struct regmap_config qca8k_ipq4019_regmap_config = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
	.max_register = 0x16ac, /* end MIB - Port6 range */
	.rd_table = &qca8k_readable_table,
};

static struct regmap_config qca8k_ipq4019_psgmii_phy_regmap_config = {
	.name = "psgmii-phy",
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
	.max_register = 0x7fc,
};

static int
qca8k_busy_wait(struct qca8k_priv *priv, u32 reg, u32 mask)
{
	u32 val;

	return regmap_read_poll_timeout(priv->regmap, reg, val,
					!(val & mask),
					0,
					QCA8K_BUSY_WAIT_TIMEOUT);
}

static int
qca8k_fdb_read(struct qca8k_priv *priv, struct qca8k_fdb *fdb)
{
	u32 reg[4], val;
	int i, ret;

	/* load the ARL table into an array */
	for (i = 0; i < 4; i++) {
		ret = qca8k_read(priv, QCA8K_REG_ATU_DATA0 + (i * 4), &val);
		if (ret < 0)
			return ret;

		reg[i] = val;
	}

	/* vid - 83:72 */
	fdb->vid = (reg[2] >> QCA8K_ATU_VID_S) & QCA8K_ATU_VID_M;
	/* aging - 67:64 */
	fdb->aging = reg[2] & QCA8K_ATU_STATUS_M;
	/* portmask - 54:48 */
	fdb->port_mask = (reg[1] >> QCA8K_ATU_PORT_S) & QCA8K_ATU_PORT_M;
	/* mac - 47:0 */
	fdb->mac[0] = (reg[1] >> QCA8K_ATU_ADDR0_S) & 0xff;
	fdb->mac[1] = reg[1] & 0xff;
	fdb->mac[2] = (reg[0] >> QCA8K_ATU_ADDR2_S) & 0xff;
	fdb->mac[3] = (reg[0] >> QCA8K_ATU_ADDR3_S) & 0xff;
	fdb->mac[4] = (reg[0] >> QCA8K_ATU_ADDR4_S) & 0xff;
	fdb->mac[5] = reg[0] & 0xff;

	return 0;
}

static void
qca8k_fdb_write(struct qca8k_priv *priv, u16 vid, u8 port_mask, const u8 *mac,
		u8 aging)
{
	u32 reg[3] = { 0 };
	int i;

	/* vid - 83:72 */
	reg[2] = (vid & QCA8K_ATU_VID_M) << QCA8K_ATU_VID_S;
	/* aging - 67:64 */
	reg[2] |= aging & QCA8K_ATU_STATUS_M;
	/* portmask - 54:48 */
	reg[1] = (port_mask & QCA8K_ATU_PORT_M) << QCA8K_ATU_PORT_S;
	/* mac - 47:0 */
	reg[1] |= mac[0] << QCA8K_ATU_ADDR0_S;
	reg[1] |= mac[1];
	reg[0] |= mac[2] << QCA8K_ATU_ADDR2_S;
	reg[0] |= mac[3] << QCA8K_ATU_ADDR3_S;
	reg[0] |= mac[4] << QCA8K_ATU_ADDR4_S;
	reg[0] |= mac[5];

	/* load the array into the ARL table */
	for (i = 0; i < 3; i++)
		qca8k_write(priv, QCA8K_REG_ATU_DATA0 + (i * 4), reg[i]);
}

static int
qca8k_fdb_access(struct qca8k_priv *priv, enum qca8k_fdb_cmd cmd, int port)
{
	u32 reg;
	int ret;

	/* Set the command and FDB index */
	reg = QCA8K_ATU_FUNC_BUSY;
	reg |= cmd;
	if (port >= 0) {
		reg |= QCA8K_ATU_FUNC_PORT_EN;
		reg |= (port & QCA8K_ATU_FUNC_PORT_M) << QCA8K_ATU_FUNC_PORT_S;
	}

	/* Write the function register triggering the table access */
	ret = qca8k_write(priv, QCA8K_REG_ATU_FUNC, reg);
	if (ret)
		return ret;

	/* wait for completion */
	ret = qca8k_busy_wait(priv, QCA8K_REG_ATU_FUNC, QCA8K_ATU_FUNC_BUSY);
	if (ret)
		return ret;

	/* Check for table full violation when adding an entry */
	if (cmd == QCA8K_FDB_LOAD) {
		ret = qca8k_read(priv, QCA8K_REG_ATU_FUNC, &reg);
		if (ret < 0)
			return ret;
		if (reg & QCA8K_ATU_FUNC_FULL)
			return -1;
	}

	return 0;
}

static int
qca8k_fdb_next(struct qca8k_priv *priv, struct qca8k_fdb *fdb, int port)
{
	int ret;

	qca8k_fdb_write(priv, fdb->vid, fdb->port_mask, fdb->mac, fdb->aging);
	ret = qca8k_fdb_access(priv, QCA8K_FDB_NEXT, port);
	if (ret < 0)
		return ret;

	return qca8k_fdb_read(priv, fdb);
}

static int
qca8k_fdb_add(struct qca8k_priv *priv, const u8 *mac, u16 port_mask,
	      u16 vid, u8 aging)
{
	int ret;

	mutex_lock(&priv->reg_mutex);
	qca8k_fdb_write(priv, vid, port_mask, mac, aging);
	ret = qca8k_fdb_access(priv, QCA8K_FDB_LOAD, -1);
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static int
qca8k_fdb_del(struct qca8k_priv *priv, const u8 *mac, u16 port_mask, u16 vid)
{
	int ret;

	mutex_lock(&priv->reg_mutex);
	qca8k_fdb_write(priv, vid, port_mask, mac, 0);
	ret = qca8k_fdb_access(priv, QCA8K_FDB_PURGE, -1);
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static void
qca8k_fdb_flush(struct qca8k_priv *priv)
{
	mutex_lock(&priv->reg_mutex);
	qca8k_fdb_access(priv, QCA8K_FDB_FLUSH, -1);
	mutex_unlock(&priv->reg_mutex);
}

static int
qca8k_vlan_access(struct qca8k_priv *priv, enum qca8k_vlan_cmd cmd, u16 vid)
{
	u32 reg;
	int ret;

	/* Set the command and VLAN index */
	reg = QCA8K_VTU_FUNC1_BUSY;
	reg |= cmd;
	reg |= vid << QCA8K_VTU_FUNC1_VID_S;

	/* Write the function register triggering the table access */
	ret = qca8k_write(priv, QCA8K_REG_VTU_FUNC1, reg);
	if (ret)
		return ret;

	/* wait for completion */
	ret = qca8k_busy_wait(priv, QCA8K_REG_VTU_FUNC1, QCA8K_VTU_FUNC1_BUSY);
	if (ret)
		return ret;

	/* Check for table full violation when adding an entry */
	if (cmd == QCA8K_VLAN_LOAD) {
		ret = qca8k_read(priv, QCA8K_REG_VTU_FUNC1, &reg);
		if (ret < 0)
			return ret;
		if (reg & QCA8K_VTU_FUNC1_FULL)
			return -ENOMEM;
	}

	return 0;
}

static int
qca8k_vlan_add(struct qca8k_priv *priv, u8 port, u16 vid, bool untagged)
{
	u32 reg;
	int ret;

	/*
	   We do the right thing with VLAN 0 and treat it as untagged while
	   preserving the tag on egress.
	 */
	if (vid == 0)
		return 0;

	mutex_lock(&priv->reg_mutex);
	ret = qca8k_vlan_access(priv, QCA8K_VLAN_READ, vid);
	if (ret < 0)
		goto out;

	ret = qca8k_read(priv, QCA8K_REG_VTU_FUNC0, &reg);
	if (ret < 0)
		goto out;
	reg |= QCA8K_VTU_FUNC0_VALID | QCA8K_VTU_FUNC0_IVL_EN;
	reg &= ~(QCA8K_VTU_FUNC0_EG_MODE_MASK << QCA8K_VTU_FUNC0_EG_MODE_S(port));
	if (untagged)
		reg |= QCA8K_VTU_FUNC0_EG_MODE_UNTAG <<
				QCA8K_VTU_FUNC0_EG_MODE_S(port);
	else
		reg |= QCA8K_VTU_FUNC0_EG_MODE_TAG <<
				QCA8K_VTU_FUNC0_EG_MODE_S(port);

	ret = qca8k_write(priv, QCA8K_REG_VTU_FUNC0, reg);
	if (ret)
		goto out;
	ret = qca8k_vlan_access(priv, QCA8K_VLAN_LOAD, vid);

out:
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static int
qca8k_vlan_del(struct qca8k_priv *priv, u8 port, u16 vid)
{
	u32 reg, mask;
	int ret, i;
	bool del;

	mutex_lock(&priv->reg_mutex);
	ret = qca8k_vlan_access(priv, QCA8K_VLAN_READ, vid);
	if (ret < 0)
		goto out;

	ret = qca8k_read(priv, QCA8K_REG_VTU_FUNC0, &reg);
	if (ret < 0)
		goto out;
	reg &= ~(3 << QCA8K_VTU_FUNC0_EG_MODE_S(port));
	reg |= QCA8K_VTU_FUNC0_EG_MODE_NOT <<
			QCA8K_VTU_FUNC0_EG_MODE_S(port);

	/* Check if we're the last member to be removed */
	del = true;
	for (i = 0; i < QCA8K_NUM_PORTS; i++) {
		mask = QCA8K_VTU_FUNC0_EG_MODE_NOT;
		mask <<= QCA8K_VTU_FUNC0_EG_MODE_S(i);

		if ((reg & mask) != mask) {
			del = false;
			break;
		}
	}

	if (del) {
		ret = qca8k_vlan_access(priv, QCA8K_VLAN_PURGE, vid);
	} else {
		ret = qca8k_write(priv, QCA8K_REG_VTU_FUNC0, reg);
		if (ret)
			goto out;
		ret = qca8k_vlan_access(priv, QCA8K_VLAN_LOAD, vid);
	}

out:
	mutex_unlock(&priv->reg_mutex);

	return ret;
}

static int
qca8k_mib_init(struct qca8k_priv *priv)
{
	int ret;

	mutex_lock(&priv->reg_mutex);
	ret = qca8k_reg_set(priv, QCA8K_REG_MIB, QCA8K_MIB_FLUSH | QCA8K_MIB_BUSY);
	if (ret)
		goto exit;

	ret = qca8k_busy_wait(priv, QCA8K_REG_MIB, QCA8K_MIB_BUSY);
	if (ret)
		goto exit;

	ret = qca8k_reg_set(priv, QCA8K_REG_MIB, QCA8K_MIB_CPU_KEEP);
	if (ret)
		goto exit;

	ret = qca8k_write(priv, QCA8K_REG_MODULE_EN, QCA8K_MODULE_EN_MIB);

exit:
	mutex_unlock(&priv->reg_mutex);
	return ret;
}

static void
qca8k_port_set_status(struct qca8k_priv *priv, int port, int enable)
{
	u32 mask = QCA8K_PORT_STATUS_TXMAC | QCA8K_PORT_STATUS_RXMAC;

	/* Port 0 is internally connected to the CPU
	 * TODO: Probably check for RGMII as well if it doesnt work
	 * in RGMII mode.
	 */
	if (port > QCA8K_CPU_PORT)
		mask |= QCA8K_PORT_STATUS_LINK_AUTO;

	if (enable)
		qca8k_reg_set(priv, QCA8K_REG_PORT_STATUS(port), mask);
	else
		qca8k_reg_clear(priv, QCA8K_REG_PORT_STATUS(port), mask);
}

static int
qca8k_setup_port(struct dsa_switch *ds, int port)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	int ret;

	/* CPU port gets connected to all user ports of the switch */
	if (dsa_is_cpu_port(ds, port)) {
		ret = qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(QCA8K_CPU_PORT),
				QCA8K_PORT_LOOKUP_MEMBER, dsa_user_ports(ds));
		if (ret)
			return ret;

		/* Disable CPU ARP Auto-learning by default */
		ret = qca8k_reg_clear(priv, QCA8K_PORT_LOOKUP_CTRL(QCA8K_CPU_PORT),
				      QCA8K_PORT_LOOKUP_LEARN);
		if (ret)
			return ret;
	}

	/* Individual user ports get connected to CPU port only */
	if (dsa_is_user_port(ds, port)) {
		int shift = 16 * (port % 2);

		ret = qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(port),
				QCA8K_PORT_LOOKUP_MEMBER,
				BIT(QCA8K_CPU_PORT));
		if (ret)
			return ret;

		/* Enable ARP Auto-learning by default */
		ret = qca8k_reg_set(priv, QCA8K_PORT_LOOKUP_CTRL(port),
				    QCA8K_PORT_LOOKUP_LEARN);
		if (ret)
			return ret;

		/* For port based vlans to work we need to set the
		 * default egress vid
		 */
		ret = qca8k_rmw(priv, QCA8K_EGRESS_VLAN(port),
				0xfff << shift,
				QCA8K_PORT_VID_DEF << shift);
		if (ret)
			return ret;

		ret = qca8k_write(priv, QCA8K_REG_PORT_VLAN_CTRL0(port),
				  QCA8K_PORT_VLAN_CVID(QCA8K_PORT_VID_DEF) |
				  QCA8K_PORT_VLAN_SVID(QCA8K_PORT_VID_DEF));
		if (ret)
			return ret;
	}

	return 0;
}

static int
qca8k_setup(struct dsa_switch *ds)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	int ret, i;

	/* Make sure that port 0 is the cpu port */
	if (!dsa_is_cpu_port(ds, 0)) {
		dev_err(priv->dev, "port 0 is not the CPU port");
		return -EINVAL;
	}

	/* Enable CPU Port */
	ret = qca8k_reg_set(priv, QCA8K_REG_GLOBAL_FW_CTRL0,
			    QCA8K_GLOBAL_FW_CTRL0_CPU_PORT_EN);
	if (ret) {
		dev_err(priv->dev, "failed enabling CPU port");
		return ret;
	}

	/* Enable MIB counters */
	ret = qca8k_mib_init(priv);
	if (ret)
		dev_warn(priv->dev, "MIB init failed");

	/* Enable QCA header mode on the cpu port */
	ret = qca8k_write(priv, QCA8K_REG_PORT_HDR_CTRL(QCA8K_CPU_PORT),
			  QCA8K_PORT_HDR_CTRL_ALL << QCA8K_PORT_HDR_CTRL_TX_S |
			  QCA8K_PORT_HDR_CTRL_ALL << QCA8K_PORT_HDR_CTRL_RX_S);
	if (ret) {
		dev_err(priv->dev, "failed enabling QCA header mode");
		return ret;
	}

	/* Disable forwarding by default on all ports */
	for (i = 0; i < QCA8K_NUM_PORTS; i++) {
		ret = qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(i),
				QCA8K_PORT_LOOKUP_MEMBER, 0);
		if (ret)
			return ret;
	}

	/* Disable MAC by default on all ports */
	for (i = 1; i < QCA8K_NUM_PORTS; i++)
		qca8k_port_set_status(priv, i, 0);

	/* Forward all unknown frames to CPU port for Linux processing */
	ret = qca8k_write(priv, QCA8K_REG_GLOBAL_FW_CTRL1,
			  BIT(QCA8K_CPU_PORT) << QCA8K_GLOBAL_FW_CTRL1_IGMP_DP_S |
			  BIT(QCA8K_CPU_PORT) << QCA8K_GLOBAL_FW_CTRL1_BC_DP_S |
			  BIT(QCA8K_CPU_PORT) << QCA8K_GLOBAL_FW_CTRL1_MC_DP_S |
			  BIT(QCA8K_CPU_PORT) << QCA8K_GLOBAL_FW_CTRL1_UC_DP_S);
	if (ret)
		return ret;

	/* Setup connection between CPU port & user ports */
	for (i = 0; i < QCA8K_NUM_PORTS; i++) {
		ret = qca8k_setup_port(ds, i);
		if (ret)
			return ret;
	}

	/* Setup our port MTUs to match power on defaults */
	for (i = 0; i < QCA8K_NUM_PORTS; i++)
		/* Set per port MTU to 1500 as the MTU change function
		 * will add the overhead and if its set to 1518 then it
		 * will apply the overhead again and we will end up with
		 * MTU of 1536 instead of 1518
		 */
		priv->port_mtu[i] = ETH_DATA_LEN;
	ret = qca8k_write(priv, QCA8K_MAX_FRAME_SIZE, ETH_FRAME_LEN + ETH_FCS_LEN);
	if (ret)
		dev_warn(priv->dev, "failed setting MTU settings");

	/* Flush the FDB table */
	qca8k_fdb_flush(priv);

	/* We don't have interrupts for link changes, so we need to poll */
	ds->pcs_poll = true;

	/* CPU port HW learning doesnt work correctly, so let DSA handle it */
	ds->assisted_learning_on_cpu_port = true;

	return 0;
}

static int psgmii_vco_calibrate(struct qca8k_priv *priv)
{
	int val, ret;

	if (!priv->psgmii_ethphy) {
		dev_err(priv->dev, "PSGMII eth PHY missing, calibration failed!\n");
		return -ENODEV;
	}

	/* Fix PSGMII RX 20bit */
	ret = phy_write(priv->psgmii_ethphy, MII_BMCR, 0x5b);
	/* Reset PHY PSGMII */
	ret = phy_write(priv->psgmii_ethphy, MII_BMCR, 0x1b);
	/* Release PHY PSGMII reset */
	ret = phy_write(priv->psgmii_ethphy, MII_BMCR, 0x5b);

	/* Poll for VCO PLL calibration finish - Malibu(QCA8075) */
	ret = phy_read_mmd_poll_timeout(priv->psgmii_ethphy,
					MDIO_MMD_PMAPMD,
					0x28, val,
					(val & BIT(0)),
					10000, 1000000,
					false);
	if (ret) {
		dev_err(priv->dev, "QCA807x PSGMII VCO calibration PLL not ready\n");
		return ret;
	}
	mdelay(50);

	/* Freeze PSGMII RX CDR */
	ret = phy_write(priv->psgmii_ethphy, MII_RESV2, 0x2230);

	/* Start PSGMIIPHY VCO PLL calibration */
	ret = regmap_set_bits(priv->psgmii,
			PSGMIIPHY_VCO_CALIBRATION_CONTROL_REGISTER_1,
			PSGMIIPHY_REG_PLL_VCO_CALIB_RESTART);

	/* Poll for PSGMIIPHY PLL calibration finish - Dakota(IPQ40xx) */
	ret = regmap_read_poll_timeout(priv->psgmii,
				       PSGMIIPHY_VCO_CALIBRATION_CONTROL_REGISTER_2,
				       val, val & PSGMIIPHY_REG_PLL_VCO_CALIB_READY,
				       10000, 1000000);
	if (ret) {
		dev_err(priv->dev, "IPQ PSGMIIPHY VCO calibration PLL not ready\n");
		return ret;
	}
	mdelay(50);

	/* Release PSGMII RX CDR */
	ret = phy_write(priv->psgmii_ethphy, MII_RESV2, 0x3230);
	/* Release PSGMII RX 20bit */
	ret = phy_write(priv->psgmii_ethphy, MII_BMCR, 0x5f);
	mdelay(200);

	return ret;
}

static void
qca8k_switch_port_loopback_on_off(struct qca8k_priv *priv, int port, int on)
{
	u32 val = QCA8K_PORT_LOOKUP_LOOPBACK;

	if (on == 0)
		val = 0;

	qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(port),
		  QCA8K_PORT_LOOKUP_LOOPBACK, val);
}

static int
qca8k_wait_for_phy_link_state(struct phy_device *phy, int need_status)
{
	int a;
	u16 status;

	for (a = 0; a < 100; a++) {
		status = phy_read(phy, MII_QCA8075_SSTATUS);
		status &= QCA8075_PHY_SPEC_STATUS_LINK;
		status = !!status;
		if (status == need_status)
			return 0;
		mdelay(8);
	}

	return -1;
}

static void
qca8k_phy_loopback_on_off(struct qca8k_priv *priv, struct phy_device *phy,
			  int sw_port, int on)
{
	if (on) {
		phy_write(phy, MII_BMCR, BMCR_ANENABLE | BMCR_RESET);
		phy_modify(phy, MII_BMCR, BMCR_PDOWN, BMCR_PDOWN);
		qca8k_wait_for_phy_link_state(phy, 0);
		qca8k_write(priv, QCA8K_REG_PORT_STATUS(sw_port), 0);
		phy_write(phy, MII_BMCR,
			BMCR_SPEED1000 |
			BMCR_FULLDPLX |
			BMCR_LOOPBACK);
		qca8k_wait_for_phy_link_state(phy, 1);
		qca8k_write(priv, QCA8K_REG_PORT_STATUS(sw_port),
			QCA8K_PORT_STATUS_SPEED_1000 |
			QCA8K_PORT_STATUS_TXMAC |
			QCA8K_PORT_STATUS_RXMAC |
			QCA8K_PORT_STATUS_DUPLEX);
		qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(sw_port),
			QCA8K_PORT_LOOKUP_STATE_FORWARD,
			QCA8K_PORT_LOOKUP_STATE_FORWARD);
	} else { /* off */
		qca8k_write(priv, QCA8K_REG_PORT_STATUS(sw_port), 0);
		qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(sw_port),
			QCA8K_PORT_LOOKUP_STATE_DISABLED,
			QCA8K_PORT_LOOKUP_STATE_DISABLED);
		phy_write(phy, MII_BMCR, BMCR_SPEED1000 | BMCR_ANENABLE | BMCR_RESET);
		/* turn off the power of the phys - so that unused
			 ports do not raise links */
		phy_modify(phy, MII_BMCR, BMCR_PDOWN, BMCR_PDOWN);
	}
}

static void
qca8k_phy_pkt_gen_prep(struct qca8k_priv *priv, struct phy_device *phy,
		       int pkts_num, int on)
{
	if (on) {
		/* enable CRC checker and packets counters */
		phy_write_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_CRC_AND_PKTS_COUNT, 0);
		phy_write_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_CRC_AND_PKTS_COUNT,
			QCA8075_MMD7_CNT_FRAME_CHK_EN | QCA8075_MMD7_CNT_SELFCLR);
		qca8k_wait_for_phy_link_state(phy, 1);
		/* packet number */
		phy_write_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_PKT_GEN_PKT_NUMB, pkts_num);
		/* pkt size - 1504 bytes + 20 bytes */
		phy_write_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_PKT_GEN_PKT_SIZE, 1504);
	} else { /* off */
		/* packet number */
		phy_write_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_PKT_GEN_PKT_NUMB, 0);
		/* disable CRC checker and packet counter */
		phy_write_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_CRC_AND_PKTS_COUNT, 0);
		/* disable traffic gen */
		phy_write_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_PKT_GEN_CTRL, 0);
	}
}

static void
qca8k_wait_for_phy_pkt_gen_fin(struct qca8k_priv *priv, struct phy_device *phy)
{
	int val;
	/* wait for all traffic end: 4096(pkt num)*1524(size)*8ns(125MHz)=49938us */
	phy_read_mmd_poll_timeout(phy, MDIO_MMD_AN, QCA8075_MMD7_PKT_GEN_CTRL,
				  val, !(val & QCA8075_MMD7_PKT_GEN_INPROGR),
				  50000, 1000000, true);
}

static void
qca8k_start_phy_pkt_gen(struct phy_device *phy)
{
	/* start traffic gen */
	phy_write_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_PKT_GEN_CTRL,
		      QCA8075_MMD7_PKT_GEN_START | QCA8075_MMD7_PKT_GEN_INPROGR);
}

static int
qca8k_start_all_phys_pkt_gens(struct qca8k_priv *priv)
{
	struct phy_device *phy;
	phy = phy_device_create(priv->bus, QCA8075_MDIO_BRDCST_PHY_ADDR,
		0, 0, NULL);
	if (!phy) {
		dev_err(priv->dev, "unable to create mdio broadcast PHY(0x%x)\n",
			QCA8075_MDIO_BRDCST_PHY_ADDR);
		return -ENODEV;
	}

	qca8k_start_phy_pkt_gen(phy);

	phy_device_free(phy);
	return 0;
}

static int
qca8k_get_phy_pkt_gen_test_result(struct phy_device *phy, int pkts_num)
{
	u32 tx_ok, tx_error;
	u32 rx_ok, rx_error;
	u32 tx_ok_high16;
	u32 rx_ok_high16;
	u32 tx_all_ok, rx_all_ok;

	/* check counters */
	tx_ok = phy_read_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_EG_FRAME_RECV_CNT_LO);
	tx_ok_high16 = phy_read_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_EG_FRAME_RECV_CNT_HI);
	tx_error = phy_read_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_EG_FRAME_ERR_CNT);
	rx_ok = phy_read_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_IG_FRAME_RECV_CNT_LO);
	rx_ok_high16 = phy_read_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_IG_FRAME_RECV_CNT_HI);
	rx_error = phy_read_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_IG_FRAME_ERR_CNT);
	tx_all_ok = tx_ok + (tx_ok_high16 << 16);
	rx_all_ok = rx_ok + (rx_ok_high16 << 16);

	if (tx_all_ok < pkts_num)
		return -1;
	if(rx_all_ok < pkts_num)
		return -2;
	if(tx_error)
		return -3;
	if(rx_error)
		return -4;
	return 0; /* test is ok */
}

static
void qca8k_phy_broadcast_write_on_off(struct qca8k_priv *priv,
				      struct phy_device *phy, int on)
{
	u32 val;

	val = phy_read_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_MDIO_BRDCST_WRITE);

	if (on == 0)
		val &= ~QCA8075_MMD7_MDIO_BRDCST_WRITE_EN;
	else
		val |= QCA8075_MMD7_MDIO_BRDCST_WRITE_EN;

	phy_write_mmd(phy, MDIO_MMD_AN, QCA8075_MMD7_MDIO_BRDCST_WRITE, val);
}

static int
qca8k_test_dsa_port_for_errors(struct qca8k_priv *priv, struct phy_device *phy,
			       int port, int test_phase)
{
	int res = 0;
	const int test_pkts_num = QCA8075_PKT_GEN_PKTS_COUNT;

	if (test_phase == 1) { /* start test preps */
		qca8k_phy_loopback_on_off(priv, phy, port, 1);
		qca8k_switch_port_loopback_on_off(priv, port, 1);
		qca8k_phy_broadcast_write_on_off(priv, phy, 1);
		qca8k_phy_pkt_gen_prep(priv, phy, test_pkts_num, 1);
	} else if (test_phase == 2) {
		/* wait for test results, collect it and cleanup */
		qca8k_wait_for_phy_pkt_gen_fin(priv, phy);
		res = qca8k_get_phy_pkt_gen_test_result(phy, test_pkts_num);
		qca8k_phy_pkt_gen_prep(priv, phy, test_pkts_num, 0);
		qca8k_phy_broadcast_write_on_off(priv, phy, 0);
		qca8k_switch_port_loopback_on_off(priv, port, 0);
		qca8k_phy_loopback_on_off(priv, phy, port, 0);
	}

	return res;
}

static int
qca8k_do_dsa_sw_ports_self_test(struct qca8k_priv *priv, int parallel_test)
{
	struct device_node *dn = priv->dev->of_node;
	struct device_node *ports, *port;
	struct device_node *phy_dn;
	struct phy_device *phy;
	int reg, err = 0, test_phase;
	u32 tests_result = 0;

	ports = of_get_child_by_name(dn, "ports");
	if (!ports) {
		dev_err(priv->dev, "no ports child node found\n");
			return -EINVAL;
	}

	for (test_phase = 1; test_phase <= 2; test_phase++) {
		if (parallel_test && test_phase == 2) {
			err = qca8k_start_all_phys_pkt_gens(priv);
			if (err)
				goto error;
		}
		for_each_available_child_of_node(ports, port) {
			err = of_property_read_u32(port, "reg", &reg);
			if (err)
				goto error;
			if (reg >= QCA8K_NUM_PORTS) {
				err = -EINVAL;
				goto error;
			}
			phy_dn = of_parse_phandle(port, "phy-handle", 0);
			if (phy_dn) {
				phy = of_phy_find_device(phy_dn);
				of_node_put(phy_dn);
				if (phy) {
					int result;
					result = qca8k_test_dsa_port_for_errors(priv,
						phy, reg, test_phase);
					if (!parallel_test && test_phase == 1)
						qca8k_start_phy_pkt_gen(phy);
					put_device(&phy->mdio.dev);
					if (test_phase == 2) {
						tests_result <<= 1;
						if (result)
							tests_result |= 1;
					}
				}
			}
		}
	}

end:
	of_node_put(ports);
	qca8k_fdb_flush(priv);
	return tests_result;
error:
	tests_result |= 0xf000;
	goto end;
}

static int
psgmii_vco_calibrate_and_test(struct dsa_switch *ds)
{
	int ret, a, test_result;
	struct qca8k_priv *priv = ds->priv;

	for (a = 0; a <= QCA8K_PSGMII_CALB_NUM; a++) {
		ret = psgmii_vco_calibrate(priv);
		if (ret)
			return ret;
		/* first we run serial test */
		test_result = qca8k_do_dsa_sw_ports_self_test(priv, 0);
		/* and if it is ok then we run the test in parallel */
		if (!test_result)
			test_result = qca8k_do_dsa_sw_ports_self_test(priv, 1);
		if (!test_result) {
			if (a > 0) {
				dev_warn(priv->dev, "PSGMII work was stabilized after %d "
					"calibration retries !\n", a);
			}
			return 0;
		} else {
			schedule();
			if (a > 0 && a % 10 == 0) {
				dev_err(priv->dev, "PSGMII work is unstable !!! "
					"Let's try to wait a bit ... %d\n", a);
				set_current_state(TASK_INTERRUPTIBLE);
				schedule_timeout(msecs_to_jiffies(a * 100));
			}
		}
	}

	panic("PSGMII work is unstable !!! "
		"Repeated recalibration attempts did not help(0x%x) !\n",
		test_result);

	return -EFAULT;
}

static int
ipq4019_psgmii_configure(struct dsa_switch *ds)
{
	struct qca8k_priv *priv = ds->priv;
	int ret;

	if (!priv->psgmii_calibrated) {
		ret = psgmii_vco_calibrate_and_test(ds);

		ret = regmap_clear_bits(priv->psgmii, PSGMIIPHY_MODE_CONTROL,
					PSGMIIPHY_MODE_ATHR_CSCO_MODE_25M);
		ret = regmap_write(priv->psgmii, PSGMIIPHY_TX_CONTROL,
				   PSGMIIPHY_TX_CONTROL_MAGIC_VALUE);

		priv->psgmii_calibrated = true;

		return ret;
	}

	return 0;
}

static void
qca8k_phylink_mac_config(struct dsa_switch *ds, int port, unsigned int mode,
			 const struct phylink_link_state *state)
{
	struct qca8k_priv *priv = ds->priv;

	switch (port) {
	case 0:
		/* CPU port, no configuration needed */
		return;
	case 1:
	case 2:
	case 3:
		if (state->interface == PHY_INTERFACE_MODE_PSGMII)
			if (ipq4019_psgmii_configure(ds))
				dev_err(ds->dev, "PSGMII configuration failed!\n");
		return;
	case 4:
	case 5:
		if (state->interface == PHY_INTERFACE_MODE_RGMII ||
		    state->interface == PHY_INTERFACE_MODE_RGMII_ID ||
		    state->interface == PHY_INTERFACE_MODE_RGMII_RXID ||
		    state->interface == PHY_INTERFACE_MODE_RGMII_TXID) {
			qca8k_reg_set(priv, QCA8K_REG_RGMII_CTRL, QCA8K_RGMII_CTRL_CLK);
		}

		if (state->interface == PHY_INTERFACE_MODE_PSGMII)
			if (ipq4019_psgmii_configure(ds))
				dev_err(ds->dev, "PSGMII configuration failed!\n");
		return;
	default:
		dev_err(ds->dev, "%s: unsupported port: %i\n", __func__, port);
		return;
	}
}

static void
qca8k_phylink_validate(struct dsa_switch *ds, int port,
		       unsigned long *supported,
		       struct phylink_link_state *state)
{
	__ETHTOOL_DECLARE_LINK_MODE_MASK(mask) = { 0, };

	switch (port) {
	case 0: /* CPU port */
		if (state->interface != PHY_INTERFACE_MODE_INTERNAL)
			goto unsupported;
		break;
	case 1:
	case 2:
	case 3:
		/* Only PSGMII mode is supported */
		if (state->interface != PHY_INTERFACE_MODE_PSGMII)
			goto unsupported;
		break;
	case 4:
	case 5:
		/* PSGMII and RGMII modes are supported */
		if (state->interface != PHY_INTERFACE_MODE_PSGMII &&
		    state->interface != PHY_INTERFACE_MODE_RGMII &&
		    state->interface != PHY_INTERFACE_MODE_RGMII_ID &&
		    state->interface != PHY_INTERFACE_MODE_RGMII_RXID &&
		    state->interface != PHY_INTERFACE_MODE_RGMII_TXID)
			goto unsupported;
		break;
	default:
unsupported:
		dev_warn(ds->dev, "interface '%s' (%d) on port %d is not supported\n",
			 phy_modes(state->interface), state->interface, port);
		linkmode_zero(supported);
		return;
	}

	if (port == 0) {
		phylink_set_port_modes(mask);

		phylink_set(mask, 1000baseT_Full);

		phylink_set(mask, Pause);
		phylink_set(mask, Asym_Pause);

		linkmode_and(supported, supported, mask);
		linkmode_and(state->advertising, state->advertising, mask);
	} else {
		/* Simply copy what PHYs tell us */
		linkmode_copy(state->advertising, supported);
	}
}

static int
qca8k_phylink_mac_link_state(struct dsa_switch *ds, int port,
			     struct phylink_link_state *state)
{
	struct qca8k_priv *priv = ds->priv;
	u32 reg;
	int ret;

	ret = qca8k_read(priv, QCA8K_REG_PORT_STATUS(port), &reg);
	if (ret < 0)
		return ret;

	state->link = !!(reg & QCA8K_PORT_STATUS_LINK_UP);
	state->an_complete = state->link;
	state->an_enabled = !!(reg & QCA8K_PORT_STATUS_LINK_AUTO);
	state->duplex = (reg & QCA8K_PORT_STATUS_DUPLEX) ? DUPLEX_FULL :
							   DUPLEX_HALF;

	switch (reg & QCA8K_PORT_STATUS_SPEED) {
	case QCA8K_PORT_STATUS_SPEED_10:
		state->speed = SPEED_10;
		break;
	case QCA8K_PORT_STATUS_SPEED_100:
		state->speed = SPEED_100;
		break;
	case QCA8K_PORT_STATUS_SPEED_1000:
		state->speed = SPEED_1000;
		break;
	default:
		state->speed = SPEED_UNKNOWN;
		break;
	}

	state->pause = MLO_PAUSE_NONE;
	if (reg & QCA8K_PORT_STATUS_RXFLOW)
		state->pause |= MLO_PAUSE_RX;
	if (reg & QCA8K_PORT_STATUS_TXFLOW)
		state->pause |= MLO_PAUSE_TX;

	return 1;
}

static void
qca8k_phylink_mac_link_down(struct dsa_switch *ds, int port, unsigned int mode,
			    phy_interface_t interface)
{
	struct qca8k_priv *priv = ds->priv;

	qca8k_port_set_status(priv, port, 0);
}

static void
qca8k_phylink_mac_link_up(struct dsa_switch *ds, int port, unsigned int mode,
			  phy_interface_t interface, struct phy_device *phydev,
			  int speed, int duplex, bool tx_pause, bool rx_pause)
{
	struct qca8k_priv *priv = ds->priv;
	u32 reg;

	if (phylink_autoneg_inband(mode)) {
		reg = QCA8K_PORT_STATUS_LINK_AUTO;
	} else {
		switch (speed) {
		case SPEED_10:
			reg = QCA8K_PORT_STATUS_SPEED_10;
			break;
		case SPEED_100:
			reg = QCA8K_PORT_STATUS_SPEED_100;
			break;
		case SPEED_1000:
			reg = QCA8K_PORT_STATUS_SPEED_1000;
			break;
		default:
			reg = QCA8K_PORT_STATUS_LINK_AUTO;
			break;
		}

		if (duplex == DUPLEX_FULL)
			reg |= QCA8K_PORT_STATUS_DUPLEX;

		if (rx_pause || dsa_is_cpu_port(ds, port))
			reg |= QCA8K_PORT_STATUS_RXFLOW;

		if (tx_pause || dsa_is_cpu_port(ds, port))
			reg |= QCA8K_PORT_STATUS_TXFLOW;
	}

	reg |= QCA8K_PORT_STATUS_TXMAC | QCA8K_PORT_STATUS_RXMAC;

	qca8k_write(priv, QCA8K_REG_PORT_STATUS(port), reg);
}

static void
qca8k_get_strings(struct dsa_switch *ds, int port, u32 stringset, uint8_t *data)
{
	int i;

	if (stringset != ETH_SS_STATS)
		return;

	for (i = 0; i < ARRAY_SIZE(ar8327_mib); i++)
		strncpy(data + i * ETH_GSTRING_LEN, ar8327_mib[i].name,
			ETH_GSTRING_LEN);
}

static void
qca8k_get_ethtool_stats(struct dsa_switch *ds, int port,
			uint64_t *data)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	const struct qca8k_mib_desc *mib;
	u32 reg, i, val;
	u32 hi = 0;
	int ret;

	for (i = 0; i < ARRAY_SIZE(ar8327_mib); i++) {
		mib = &ar8327_mib[i];
		reg = QCA8K_PORT_MIB_COUNTER(port) + mib->offset;

		ret = qca8k_read(priv, reg, &val);
		if (ret < 0)
			continue;

		if (mib->size == 2) {
			ret = qca8k_read(priv, reg + 4, &hi);
			if (ret < 0)
				continue;
		}

		data[i] = val;
		if (mib->size == 2)
			data[i] |= (u64)hi << 32;
	}
}

static int
qca8k_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	if (sset != ETH_SS_STATS)
		return 0;

	return ARRAY_SIZE(ar8327_mib);
}

static int
qca8k_set_mac_eee(struct dsa_switch *ds, int port, struct ethtool_eee *eee)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	u32 lpi_en = QCA8K_REG_EEE_CTRL_LPI_EN(port);
	u32 reg;
	int ret;

	mutex_lock(&priv->reg_mutex);
	ret = qca8k_read(priv, QCA8K_REG_EEE_CTRL, &reg);
	if (ret < 0)
		goto exit;

	if (eee->eee_enabled)
		reg |= lpi_en;
	else
		reg &= ~lpi_en;
	ret = qca8k_write(priv, QCA8K_REG_EEE_CTRL, reg);

exit:
	mutex_unlock(&priv->reg_mutex);
	return ret;
}

static int
qca8k_get_mac_eee(struct dsa_switch *ds, int port, struct ethtool_eee *e)
{
	/* Nothing to do on the port's MAC */
	return 0;
}

static void
qca8k_port_stp_state_set(struct dsa_switch *ds, int port, u8 state)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	u32 stp_state;

	switch (state) {
	case BR_STATE_DISABLED:
		stp_state = QCA8K_PORT_LOOKUP_STATE_DISABLED;
		break;
	case BR_STATE_BLOCKING:
		stp_state = QCA8K_PORT_LOOKUP_STATE_BLOCKING;
		break;
	case BR_STATE_LISTENING:
		stp_state = QCA8K_PORT_LOOKUP_STATE_LISTENING;
		break;
	case BR_STATE_LEARNING:
		stp_state = QCA8K_PORT_LOOKUP_STATE_LEARNING;
		break;
	case BR_STATE_FORWARDING:
	default:
		stp_state = QCA8K_PORT_LOOKUP_STATE_FORWARD;
		break;
	}

	qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(port),
		  QCA8K_PORT_LOOKUP_STATE_MASK, stp_state);
}

static int
qca8k_port_bridge_join(struct dsa_switch *ds, int port, struct net_device *br)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	int port_mask, cpu_port;
	int i, ret;

	cpu_port = dsa_to_port(ds, port)->cpu_dp->index;
	port_mask = BIT(cpu_port);

	for (i = 0; i < QCA8K_NUM_PORTS; i++) {
		if (dsa_is_cpu_port(ds, i))
			continue;
		if (dsa_to_port(ds, i)->bridge_dev != br)
			continue;
		/* Add this port to the portvlan mask of the other ports
		 * in the bridge
		 */
		ret = qca8k_reg_set(priv,
				    QCA8K_PORT_LOOKUP_CTRL(i),
				    BIT(port));
		if (ret)
			return ret;
		if (i != port)
			port_mask |= BIT(i);
	}

	/* Add all other ports to this ports portvlan mask */
	ret = qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(port),
			QCA8K_PORT_LOOKUP_MEMBER, port_mask);

	return ret;
}

static void
qca8k_port_bridge_leave(struct dsa_switch *ds, int port, struct net_device *br)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	int cpu_port, i;

	cpu_port = dsa_to_port(ds, port)->cpu_dp->index;

	for (i = 0; i < QCA8K_NUM_PORTS; i++) {
		if (dsa_is_cpu_port(ds, i))
			continue;
		if (dsa_to_port(ds, i)->bridge_dev != br)
			continue;
		/* Remove this port to the portvlan mask of the other ports
		 * in the bridge
		 */
		qca8k_reg_clear(priv,
				QCA8K_PORT_LOOKUP_CTRL(i),
				BIT(port));
	}

	/* Set the cpu port to be the only one in the portvlan mask of
	 * this port
	 */
	qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(port),
		  QCA8K_PORT_LOOKUP_MEMBER, BIT(cpu_port));
}

static int
qca8k_port_enable(struct dsa_switch *ds, int port,
		  struct phy_device *phy)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;

	qca8k_port_set_status(priv, port, 1);
	priv->port_sts[port].enabled = 1;

	if (dsa_is_user_port(ds, port))
		phy_support_asym_pause(phy);

	return 0;
}

static void
qca8k_port_disable(struct dsa_switch *ds, int port)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;

	qca8k_port_set_status(priv, port, 0);
	priv->port_sts[port].enabled = 0;
}

static int
qca8k_port_change_mtu(struct dsa_switch *ds, int port, int new_mtu)
{
	struct qca8k_priv *priv = ds->priv;
	int i, mtu = 0;

	priv->port_mtu[port] = new_mtu;

	for (i = 0; i < QCA8K_NUM_PORTS; i++)
		if (priv->port_mtu[i] > mtu)
			mtu = priv->port_mtu[i];

	/* Include L2 header / FCS length */
	return qca8k_write(priv, QCA8K_MAX_FRAME_SIZE, mtu + ETH_HLEN + ETH_FCS_LEN);
}

static int
qca8k_port_max_mtu(struct dsa_switch *ds, int port)
{
	return QCA8K_MAX_MTU;
}

static int
qca8k_port_fdb_insert(struct qca8k_priv *priv, const u8 *addr,
		      u16 port_mask, u16 vid)
{
	/* Set the vid to the port vlan id if no vid is set */
	if (!vid)
		vid = QCA8K_PORT_VID_DEF;

	return qca8k_fdb_add(priv, addr, port_mask, vid,
			     QCA8K_ATU_STATUS_STATIC);
}

static int
qca8k_port_fdb_add(struct dsa_switch *ds, int port,
		   const unsigned char *addr, u16 vid)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	u16 port_mask = BIT(port);

	return qca8k_port_fdb_insert(priv, addr, port_mask, vid);
}

static int
qca8k_port_fdb_del(struct dsa_switch *ds, int port,
		   const unsigned char *addr, u16 vid)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	u16 port_mask = BIT(port);

	if (!vid)
		vid = QCA8K_PORT_VID_DEF;

	return qca8k_fdb_del(priv, addr, port_mask, vid);
}

static int
qca8k_port_fdb_dump(struct dsa_switch *ds, int port,
		    dsa_fdb_dump_cb_t *cb, void *data)
{
	struct qca8k_priv *priv = (struct qca8k_priv *)ds->priv;
	struct qca8k_fdb _fdb = { 0 };
	int cnt = QCA8K_NUM_FDB_RECORDS;
	bool is_static;
	int ret = 0;

	mutex_lock(&priv->reg_mutex);
	while (cnt-- && !qca8k_fdb_next(priv, &_fdb, port)) {
		if (!_fdb.aging)
			break;
		is_static = (_fdb.aging == QCA8K_ATU_STATUS_STATIC);
		ret = cb(_fdb.mac, _fdb.vid, is_static, data);
		if (ret)
			break;
	}
	mutex_unlock(&priv->reg_mutex);

	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
static int
qca8k_port_vlan_filtering(struct dsa_switch *ds, int port, bool vlan_filtering,
			  struct switchdev_trans *trans)
#else
static int
qca8k_port_vlan_filtering(struct dsa_switch *ds, int port, bool vlan_filtering,
                          struct netlink_ext_ack *extack)
#endif
{
	struct qca8k_priv *priv = ds->priv;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
	if (switchdev_trans_ph_prepare(trans))
		return 0;
#endif

	if (vlan_filtering) {
		qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(port),
			  QCA8K_PORT_LOOKUP_VLAN_MODE,
			  QCA8K_PORT_LOOKUP_VLAN_MODE_SECURE);
	} else {
		qca8k_rmw(priv, QCA8K_PORT_LOOKUP_CTRL(port),
			  QCA8K_PORT_LOOKUP_VLAN_MODE,
			  QCA8K_PORT_LOOKUP_VLAN_MODE_NONE);
	}

	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
static int
qca8k_port_vlan_prepare(struct dsa_switch *ds, int port,
			const struct switchdev_obj_port_vlan *vlan)
{
	return 0;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
static void
qca8k_port_vlan_add(struct dsa_switch *ds, int port,
		    const struct switchdev_obj_port_vlan *vlan)
#else
static int
qca8k_port_vlan_add(struct dsa_switch *ds, int port,
		    const struct switchdev_obj_port_vlan *vlan,
		    struct netlink_ext_ack *extack)
#endif
{
	bool untagged = vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED;
	bool pvid = vlan->flags & BRIDGE_VLAN_INFO_PVID;
	struct qca8k_priv *priv = ds->priv;
	int ret = 0;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
	u16 vid;

	for (vid = vlan->vid_begin; vid <= vlan->vid_end && !ret; ++vid)
		ret = qca8k_vlan_add(priv, port, vid, untagged);
#else
	ret = qca8k_vlan_add(priv, port, vlan->vid, untagged);
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
	if (ret)
		dev_err(priv->dev, "Failed to add VLAN to port %d (%d)", port, ret);
#else
	if (ret) {
		dev_err(priv->dev, "Failed to add VLAN to port %d (%d)", port, ret);
		return ret;
	}
#endif

	if (pvid) {
		int shift = 16 * (port % 2);

		qca8k_rmw(priv, QCA8K_EGRESS_VLAN(port),
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
			  0xfff << shift,
			  vlan->vid_end << shift);
#else
			  0xfff << shift, vlan->vid << shift);
#endif
		qca8k_write(priv, QCA8K_REG_PORT_VLAN_CTRL0(port),
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
			    QCA8K_PORT_VLAN_CVID(vlan->vid_end) |
			    QCA8K_PORT_VLAN_SVID(vlan->vid_end));
#else
			    QCA8K_PORT_VLAN_CVID(vlan->vid) |
			    QCA8K_PORT_VLAN_SVID(vlan->vid));
#endif
	}
#if LINUX_VERSION_CODE > KERNEL_VERSION(5,12,0)
	return 0;
#endif
}

static int
qca8k_port_vlan_del(struct dsa_switch *ds, int port,
		    const struct switchdev_obj_port_vlan *vlan)
{
	struct qca8k_priv *priv = ds->priv;
	int ret = 0;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
	u16 vid;

	for (vid = vlan->vid_begin; vid <= vlan->vid_end && !ret; ++vid)
		ret = qca8k_vlan_del(priv, port, vid);
#else
	ret = qca8k_vlan_del(priv, port, vlan->vid);
#endif
	if (ret)
		dev_err(priv->dev, "Failed to delete VLAN from port %d (%d)", port, ret);

	return ret;
}

static enum dsa_tag_protocol
qca8k_get_tag_protocol(struct dsa_switch *ds, int port,
		       enum dsa_tag_protocol mp)
{
	return DSA_TAG_PROTO_IPQ4019;
}

static const struct dsa_switch_ops qca8k_switch_ops = {
	.get_tag_protocol	= qca8k_get_tag_protocol,
	.setup			= qca8k_setup,
	.get_strings		= qca8k_get_strings,
	.get_ethtool_stats	= qca8k_get_ethtool_stats,
	.get_sset_count		= qca8k_get_sset_count,
	.get_mac_eee		= qca8k_get_mac_eee,
	.set_mac_eee		= qca8k_set_mac_eee,
	.port_enable		= qca8k_port_enable,
	.port_disable		= qca8k_port_disable,
	.port_change_mtu	= qca8k_port_change_mtu,
	.port_max_mtu		= qca8k_port_max_mtu,
	.port_stp_state_set	= qca8k_port_stp_state_set,
	.port_bridge_join	= qca8k_port_bridge_join,
	.port_bridge_leave	= qca8k_port_bridge_leave,
	.port_fdb_add		= qca8k_port_fdb_add,
	.port_fdb_del		= qca8k_port_fdb_del,
	.port_fdb_dump		= qca8k_port_fdb_dump,
	.port_vlan_filtering	= qca8k_port_vlan_filtering,
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,12,0)
	.port_vlan_prepare	= qca8k_port_vlan_prepare,
#endif
	.port_vlan_add		= qca8k_port_vlan_add,
	.port_vlan_del		= qca8k_port_vlan_del,
	.phylink_validate	= qca8k_phylink_validate,
	.phylink_mac_link_state	= qca8k_phylink_mac_link_state,
	.phylink_mac_config	= qca8k_phylink_mac_config,
	.phylink_mac_link_down	= qca8k_phylink_mac_link_down,
	.phylink_mac_link_up	= qca8k_phylink_mac_link_up,
};

static int
qca8k_ipq4019_probe(struct platform_device *pdev)
{
	struct qca8k_priv *priv;
	void __iomem *base, *psgmii;
	struct device_node *np = pdev->dev.of_node, *mdio_np, *psgmii_ethphy_np;
	int ret;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = &pdev->dev;

	base = devm_platform_ioremap_resource_byname(pdev, "base");
	if (IS_ERR(base))
		return PTR_ERR(base);

	priv->regmap = devm_regmap_init_mmio(priv->dev, base,
					     &qca8k_ipq4019_regmap_config);
	if (IS_ERR(priv->regmap)) {
		ret = PTR_ERR(priv->regmap);
		dev_err(priv->dev, "base regmap initialization failed, %d\n", ret);
		return ret;
	}

	psgmii = devm_platform_ioremap_resource_byname(pdev, "psgmii_phy");
	if (IS_ERR(psgmii))
		return PTR_ERR(psgmii);

	priv->psgmii = devm_regmap_init_mmio(priv->dev, psgmii,
					     &qca8k_ipq4019_psgmii_phy_regmap_config);
	if (IS_ERR(priv->psgmii)) {
		ret = PTR_ERR(priv->psgmii);
		dev_err(priv->dev, "PSGMII regmap initialization failed, %d\n", ret);
		return ret;
	}

	mdio_np = of_parse_phandle(np, "mdio", 0);
	if (!mdio_np) {
		dev_err(&pdev->dev, "unable to get MDIO bus phandle\n");
		of_node_put(mdio_np);
		return -EINVAL;
	}

	priv->bus = of_mdio_find_bus(mdio_np);
	of_node_put(mdio_np);
	if (!priv->bus) {
		dev_err(&pdev->dev, "unable to find MDIO bus\n");
		return -EPROBE_DEFER;
	}

	psgmii_ethphy_np = of_parse_phandle(np, "psgmii-ethphy", 0);
	if (!psgmii_ethphy_np) {
		dev_dbg(&pdev->dev, "unable to get PSGMII eth PHY phandle\n");
		of_node_put(psgmii_ethphy_np);
	}

	if (psgmii_ethphy_np) {
		priv->psgmii_ethphy = of_phy_find_device(psgmii_ethphy_np);
		of_node_put(psgmii_ethphy_np);
		if (!priv->psgmii_ethphy) {
			dev_err(&pdev->dev, "unable to get PSGMII eth PHY\n");
			return -ENODEV;
		}
	}

	priv->ds = devm_kzalloc(priv->dev, sizeof(*priv->ds), GFP_KERNEL);
	if (!priv->ds)
		return -ENOMEM;

	priv->ds->dev = priv->dev;
	priv->ds->num_ports = QCA8K_NUM_PORTS;
	priv->ds->priv = priv;
	priv->ops = qca8k_switch_ops;
	priv->ds->ops = &priv->ops;

	mutex_init(&priv->reg_mutex);
	platform_set_drvdata(pdev, priv);

	return dsa_register_switch(priv->ds);
}

static int
qca8k_ipq4019_remove(struct platform_device *pdev)
{
	struct qca8k_priv *priv = dev_get_drvdata(&pdev->dev);
	int i;

	if (!priv)
		return 0;

	for (i = 0; i < QCA8K_NUM_PORTS; i++)
		qca8k_port_set_status(priv, i, 0);

	dsa_unregister_switch(priv->ds);

	dev_set_drvdata(&pdev->dev, NULL);

	return 0;
}

static const struct of_device_id qca8k_ipq4019_of_match[] = {
	{ .compatible = "qca,ipq4019-qca8337n" },
	{ /* sentinel */ },
};

static struct platform_driver qca8k_ipq4019_driver = {
	.probe = qca8k_ipq4019_probe,
	.remove = qca8k_ipq4019_remove,
	.driver = {
		.name = "qca8k-ipq4019",
		.of_match_table = qca8k_ipq4019_of_match,
	},
};

module_platform_driver(qca8k_ipq4019_driver);

MODULE_AUTHOR("Mathieu Olivari, John Crispin <john@phrozen.org>");
MODULE_AUTHOR("Gabor Juhos <j4g8y7@gmail.com>, Robert Marko <robert.marko@sartura.hr>");
MODULE_DESCRIPTION("Qualcomm IPQ4019 built-in switch driver");
MODULE_LICENSE("GPL v2");
