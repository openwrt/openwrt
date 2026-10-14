// SPDX-License-Identifier: GPL-2.0
/*
 * qca8386.c - DSA driver for the Qualcomm QCA8386 ("Manhattan"/MHT) switch
 *
 * The QCA8386 integrates a qca8k-family L2 switch core, two SerDes/UNIPHY
 * instances, four QCA8084 2.5G EPHYs and an NSS clock controller in one
 * package, all reached over the SoC MDIO bus. On the Linksys SPNMX57
 * (IPQ5018) the SoC talks to the switch CPU port (port 0) over a forced
 * 2.5G SGMII+ link and the switch bridges internally to the four EPHYs
 * (MDIO 1..4).
 *
 * Forked from the mainline v6.18 qca8k driver (drivers/net/dsa/qca/qca8k-8xxx.c
 * + qca8k-common.c), dropping the internal MDIO-master / eth-mgmt paths: on this
 * chip the four EPHYs are driven by the QCA8084 package driver on the same SoC
 * mdio1 bus, not through the switch's own MDIO master.
 *
 * Confirmed on hardware: the 32-bit indirect MDIO access (page written to
 * pseudo-PHY 0x18 reg 0x0c - the defining delta from qca8k, which uses reg 0),
 * device id 0x17, and the CPU port linking to the SoC gmac1 conduit at 2.5G.
 * The switch-core register offsets are QCA8337/isisc-compatible.
 *
 * Note: the NSS clock controller and the PHY package share this indirect MDIO
 * window, so the page register must never be cached - see qca8386_set_page().
 *
 * Copyright (c) 2015, 2019, The Linux Foundation. All rights reserved.
 * Copyright (c) 2016 John Crispin <john@phrozen.org>
 * Copyright (c) 2026 Luca C <clubdog123@gmail.com>
 */

#include <linux/module.h>
#include <linux/phy.h>
#include <linux/netdevice.h>
#include <linux/bitfield.h>
#include <linux/regmap.h>
#include <net/dsa.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#include <linux/mdio.h>
#include <linux/phylink.h>
#include <linux/gpio/consumer.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/delay.h>
#include <linux/if_bridge.h>

/* ---- QCA8386 identity + indirect-MDIO access ---------------------------- */

#define QCA8386_ID				0x17
/* The defining register-access delta from qca8k: page -> phy 0x18 reg 0x0c
 * (qca8k writes the page to reg 0), confirmed on the live bus.
 */
#define QCA8386_MDIO_PAGE_REG			0x0c

/* CPU port 0 + four user ports (1..4). qca8k has 7 (two CPU). */
#define QCA8386_NUM_PORTS			5
#define QCA8386_CPU_PORT			0

/* ---- Switch-core registers (QCA8337/isisc-family) ---------------------- */

#define QCA8386_REG_MASK_CTRL			0x000
#define   QCA8386_MASK_CTRL_REV_ID_MASK		GENMASK(7, 0)
#define   QCA8386_MASK_CTRL_REV_ID(x)		FIELD_GET(QCA8386_MASK_CTRL_REV_ID_MASK, x)
#define   QCA8386_MASK_CTRL_DEVICE_ID_MASK	GENMASK(15, 8)
#define   QCA8386_MASK_CTRL_DEVICE_ID(x)	FIELD_GET(QCA8386_MASK_CTRL_DEVICE_ID_MASK, x)

#define QCA8386_REG_PORT_STATUS(_i)		(0x07c + (_i) * 4)
#define   QCA8386_PORT_STATUS_SPEED		GENMASK(1, 0)
#define   QCA8386_PORT_STATUS_SPEED_10		0
#define   QCA8386_PORT_STATUS_SPEED_100		0x1
#define   QCA8386_PORT_STATUS_SPEED_1000	0x2
/* 2.5G is NOT a distinct code: the switch-core MAC reuses the 1000 code (2)
 * and the real rate comes from the SerDes. (SSDK: MHT_PORT_SPEED_2500M ==
 * MHT_PORT_SPEED_1000M == 2.)
 */
#define   QCA8386_PORT_STATUS_SPEED_2500	QCA8386_PORT_STATUS_SPEED_1000
#define   QCA8386_PORT_STATUS_TXMAC		BIT(2)
#define   QCA8386_PORT_STATUS_RXMAC		BIT(3)
#define   QCA8386_PORT_STATUS_TXFLOW		BIT(4)
#define   QCA8386_PORT_STATUS_RXFLOW		BIT(5)
#define   QCA8386_PORT_STATUS_DUPLEX		BIT(6)
#define   QCA8386_PORT_STATUS_LINK_AUTO		BIT(9)

#define QCA8386_REG_PORT_HDR_CTRL(_i)		(0x9c + (_i) * 4)
#define   QCA8386_PORT_HDR_CTRL_RX_MASK		GENMASK(3, 2)
#define   QCA8386_PORT_HDR_CTRL_TX_MASK		GENMASK(1, 0)
#define   QCA8386_PORT_HDR_CTRL_ALL		2

#define QCA8386_REG_GLOBAL_FW_CTRL0		0x620
#define   QCA8386_GLOBAL_FW_CTRL0_CPU_PORT_EN	BIT(10)

#define QCA8386_REG_GLOBAL_FW_CTRL1		0x624
#define   QCA8386_FW_CTRL1_IGMP_DP_MASK		GENMASK(30, 24)
#define   QCA8386_FW_CTRL1_BC_DP_MASK		GENMASK(22, 16)
#define   QCA8386_FW_CTRL1_MC_DP_MASK		GENMASK(14, 8)
#define   QCA8386_FW_CTRL1_UC_DP_MASK		GENMASK(6, 0)

#define QCA8386_PORT_LOOKUP_CTRL(_i)		(0x660 + (_i) * 0xc)
#define   QCA8386_PORT_LOOKUP_MEMBER		GENMASK(6, 0)
#define   QCA8386_PORT_LOOKUP_STATE_MASK	GENMASK(18, 16)
#define   QCA8386_PORT_LOOKUP_STATE(x)		FIELD_PREP(QCA8386_PORT_LOOKUP_STATE_MASK, x)
#define   QCA8386_PORT_LOOKUP_STATE_DISABLED	QCA8386_PORT_LOOKUP_STATE(0x0)
#define   QCA8386_PORT_LOOKUP_STATE_BLOCKING	QCA8386_PORT_LOOKUP_STATE(0x1)
#define   QCA8386_PORT_LOOKUP_STATE_LISTENING	QCA8386_PORT_LOOKUP_STATE(0x2)
#define   QCA8386_PORT_LOOKUP_STATE_LEARNING	QCA8386_PORT_LOOKUP_STATE(0x3)
#define   QCA8386_PORT_LOOKUP_STATE_FORWARD	QCA8386_PORT_LOOKUP_STATE(0x4)
#define   QCA8386_PORT_LOOKUP_LEARN		BIT(20)

#define QCA8386_PORT_VID_DEF			1
#define QCA8386_REG_PORT_VLAN_CTRL0(_i)		(0x420 + (_i) * 8)
#define   QCA8386_PORT_VLAN_CVID(x)		FIELD_PREP(GENMASK(27, 16), x)
#define   QCA8386_PORT_VLAN_SVID(x)		FIELD_PREP(GENMASK(11, 0), x)
#define QCA8386_EGRESS_VLAN(x)			(0x0c70 + (4 * ((x) / 2)))
#define   QCA8386_EGREES_VLAN_PORT_SHIFT(_i)	(16 * ((_i) % 2))
#define   QCA8386_EGREES_VLAN_PORT_MASK(_i)	(GENMASK(11, 0) << QCA8386_EGREES_VLAN_PORT_SHIFT(_i))
#define   QCA8386_EGREES_VLAN_PORT(_i, x)	((x) << QCA8386_EGREES_VLAN_PORT_SHIFT(_i))

/* Upper bound of the switch-core register window. */
#define QCA8386_MAX_REGISTER			0x16ac

struct qca8386_info {
	u8 id;
	const char *name;
};

/* The switch core plus MAC0..MAC5 TX/RX, in the order the vendor SSDK pulses
 * them in qca_mht_switch_reset().
 */
#define QCA8386_NUM_RESETS	13

static const char * const qca8386_reset_names[QCA8386_NUM_RESETS] = {
	"switch_core",
	"mac0_tx", "mac0_rx",
	"mac1_tx", "mac1_rx",
	"mac2_tx", "mac2_rx",
	"mac3_tx", "mac3_rx",
	"mac4_tx", "mac4_rx",
	"mac5_tx", "mac5_rx",
};

/* Four datapath clocks per port, in DT order: tx, rx, then the two that feed
 * the port's line side (SerDes on the CPU port, internal GEPHY on the user
 * ports). Port N's tx clock is index N * QCA8386_CLKS_PER_PORT.
 */
#define QCA8386_CLKS_PER_PORT	4
#define QCA8386_NUM_CLKS	(QCA8386_NUM_PORTS * QCA8386_CLKS_PER_PORT)

static const char * const qca8386_clk_names[QCA8386_NUM_CLKS] = {
	"mac0_tx", "mac0_rx", "mac0_tx_srds1", "mac0_rx_srds1",
	"mac1_tx", "mac1_rx", "mac1_gephy_tx", "mac1_gephy_rx",
	"mac2_tx", "mac2_rx", "mac2_gephy_tx", "mac2_gephy_rx",
	"mac3_tx", "mac3_rx", "mac3_gephy_tx", "mac3_gephy_rx",
	"mac4_tx", "mac4_rx", "mac4_gephy_tx", "mac4_gephy_rx",
};

/* Datapath clock rates, from the vendor's UQXGMII_SPEED_*_CLK defines. */
#define QCA8386_CLK_RATE_2500M	312500000UL
#define QCA8386_CLK_RATE_1000M	125000000UL
#define QCA8386_CLK_RATE_100M	25000000UL
#define QCA8386_CLK_RATE_10M	2500000UL

struct qca8386_priv {
	struct device *dev;
	struct mii_bus *bus;
	struct regmap *regmap;
	struct dsa_switch *ds;
	const struct qca8386_info *info;

	struct reset_control_bulk_data resets[QCA8386_NUM_RESETS];
	struct clk_bulk_data clks[QCA8386_NUM_CLKS];

	struct mutex reg_mutex;
	u8 switch_id;
	u8 switch_revision;
};

/* ---- 32-bit indirect MDIO ------------------------------------------------
 *
 * The QCA8386 is addressed like the rest of the QCA8084 package, NOT like the
 * QCA8337 this driver was forked from. For a switch-core byte offset:
 *
 *   page    = (offset >> 8) & 0xffff  -> pseudo-PHY 0x18, reg 0x0c
 *   phy_id  = 0x10 | ((offset >> 5) & 0x7)
 *   reg     =  offset       & 0x1f    -> low 16 bits
 *   reg | QCA8386_MII_REG_UPPER_16    -> high 16 bits
 *
 * qca8k halves the offset when computing the register and puts the upper half
 * at reg + 1; both are wrong here. See __qca8084_mii_read()/__qca8084_set_page()
 * in drivers/net/phy/qcom/qca808x.c, which drive this same chip.
 *
 * Getting this wrong is silent and very misleading: offset 0 decodes
 * identically under both schemes, so MASK_CTRL reads back a perfectly correct
 * chip ID while every other register reads 0 - and a write puts the low half
 * and then the high half into the *same* register, so the high half clobbers
 * the low one and nothing ever sticks.
 */

#define QCA8386_MII_REG_UPPER_16	BIT(1)

static void
qca8386_split_addr(u32 regaddr, u16 *r1, u16 *r2, u16 *page)
{
	*r1 = regaddr & 0x1f;
	*r2 = (regaddr >> 5) & 0x7;
	*page = (regaddr >> 8) & 0xffff;
}

static int
qca8386_mii_write_lo(struct mii_bus *bus, int phy_id, u32 regnum, u32 val)
{
	int ret;
	u16 lo;

	lo = val & 0xffff;
	ret = bus->write(bus, phy_id, regnum, lo);
	if (ret < 0)
		dev_err_ratelimited(&bus->dev,
				    "failed to write qca8386 32bit lo register\n");

	return ret;
}

static int
qca8386_mii_write_hi(struct mii_bus *bus, int phy_id, u32 regnum, u32 val)
{
	int ret;
	u16 hi;

	hi = (u16)(val >> 16);
	ret = bus->write(bus, phy_id, regnum, hi);
	if (ret < 0)
		dev_err_ratelimited(&bus->dev,
				    "failed to write qca8386 32bit hi register\n");

	return ret;
}

static int
qca8386_mii_read_lo(struct mii_bus *bus, int phy_id, u32 regnum, u32 *val)
{
	int ret;

	ret = bus->read(bus, phy_id, regnum);
	if (ret < 0)
		goto err;

	*val = ret & 0xffff;
	return 0;

err:
	dev_err_ratelimited(&bus->dev,
			    "failed to read qca8386 32bit lo register\n");
	*val = 0;

	return ret;
}

static int
qca8386_mii_read_hi(struct mii_bus *bus, int phy_id, u32 regnum, u32 *val)
{
	int ret;

	ret = bus->read(bus, phy_id, regnum);
	if (ret < 0)
		goto err;

	*val = ret << 16;
	return 0;

err:
	dev_err_ratelimited(&bus->dev,
			    "failed to read qca8386 32bit hi register\n");
	*val = 0;

	return ret;
}

static int
qca8386_mii_read32(struct mii_bus *bus, int phy_id, u32 regnum, u32 *val)
{
	u32 hi, lo;
	int ret;

	*val = 0;

	ret = qca8386_mii_read_lo(bus, phy_id, regnum, &lo);
	if (ret < 0)
		goto err;

	ret = qca8386_mii_read_hi(bus, phy_id,
				  regnum | QCA8386_MII_REG_UPPER_16, &hi);
	if (ret < 0)
		goto err;

	*val = lo | hi;

err:
	return ret;
}

static void
qca8386_mii_write32(struct mii_bus *bus, int phy_id, u32 regnum, u32 val)
{
	if (qca8386_mii_write_lo(bus, phy_id, regnum, val) < 0)
		return;

	qca8386_mii_write_hi(bus, phy_id,
			     regnum | QCA8386_MII_REG_UPPER_16, val);
}

static int
qca8386_set_page(struct qca8386_priv *priv, u16 page)
{
	struct mii_bus *bus = priv->bus;
	int ret;

	/* Always write the page - do NOT cache it.
	 *
	 * qca8k caches the current page to save an MDIO write, which is safe
	 * there because the switch owns its bus. Here the QCA8386's internal
	 * NSS clock controller (nsscc-qca8k) and the QCA8084 PHY package sit
	 * behind the SAME indirect window and write this very page register
	 * without updating our cache. A cached page therefore goes stale the
	 * moment nsscc touches the bus, and the next access would skip the
	 * page write and read/write the WRONG page.
	 */
	/* THE defining QCA8386 delta: page register is 0x0c, not 0. */
	ret = bus->write(bus, 0x18, QCA8386_MDIO_PAGE_REG, page);
	if (ret < 0) {
		dev_err_ratelimited(&bus->dev,
				    "failed to set qca8386 page\n");
		return ret;
	}

	usleep_range(1000, 2000);
	return 0;
}

static int
qca8386_read_mii(struct qca8386_priv *priv, u32 reg, u32 *val)
{
	struct mii_bus *bus = priv->bus;
	u16 r1, r2, page;
	int ret;

	qca8386_split_addr(reg, &r1, &r2, &page);

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	ret = qca8386_set_page(priv, page);
	if (ret < 0)
		goto exit;

	ret = qca8386_mii_read32(bus, 0x10 | r2, r1, val);

exit:
	mutex_unlock(&bus->mdio_lock);
	return ret;
}

static int
qca8386_write_mii(struct qca8386_priv *priv, u32 reg, u32 val)
{
	struct mii_bus *bus = priv->bus;
	u16 r1, r2, page;
	int ret;

	qca8386_split_addr(reg, &r1, &r2, &page);

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	ret = qca8386_set_page(priv, page);
	if (ret < 0)
		goto exit;

	qca8386_mii_write32(bus, 0x10 | r2, r1, val);

exit:
	mutex_unlock(&bus->mdio_lock);
	return ret;
}

static int
qca8386_regmap_update_bits_mii(struct qca8386_priv *priv, u32 reg,
			       u32 mask, u32 write_val)
{
	struct mii_bus *bus = priv->bus;
	u16 r1, r2, page;
	u32 val;
	int ret;

	qca8386_split_addr(reg, &r1, &r2, &page);

	mutex_lock_nested(&bus->mdio_lock, MDIO_MUTEX_NESTED);

	ret = qca8386_set_page(priv, page);
	if (ret < 0)
		goto exit;

	ret = qca8386_mii_read32(bus, 0x10 | r2, r1, &val);
	if (ret < 0)
		goto exit;

	val &= ~mask;
	val |= write_val;
	qca8386_mii_write32(bus, 0x10 | r2, r1, val);

exit:
	mutex_unlock(&bus->mdio_lock);

	return ret;
}

static int
qca8386_bulk_read(void *ctx, const void *reg_buf, size_t reg_len,
		  void *val_buf, size_t val_len)
{
	int i, count = val_len / sizeof(u32), ret;
	struct qca8386_priv *priv = ctx;
	u32 reg = *(u16 *)reg_buf;

	for (i = 0; i < count; i++, reg += sizeof(u32)) {
		ret = qca8386_read_mii(priv, reg, val_buf + i);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int
qca8386_bulk_gather_write(void *ctx, const void *reg_buf, size_t reg_len,
			  const void *val_buf, size_t val_len)
{
	int i, count = val_len / sizeof(u32), ret;
	struct qca8386_priv *priv = ctx;
	u32 reg = *(u16 *)reg_buf;
	u32 *val = (u32 *)val_buf;

	for (i = 0; i < count; i++, reg += sizeof(u32), val++) {
		ret = qca8386_write_mii(priv, reg, *val);
		if (ret < 0)
			return ret;
	}

	return 0;
}

static int
qca8386_bulk_write(void *ctx, const void *data, size_t bytes)
{
	return qca8386_bulk_gather_write(ctx, data, sizeof(u16),
					data + sizeof(u16), bytes - sizeof(u16));
}

static int
qca8386_regmap_update_bits(void *ctx, u32 reg, u32 mask, u32 write_val)
{
	struct qca8386_priv *priv = ctx;

	return qca8386_regmap_update_bits_mii(priv, reg, mask, write_val);
}

static const struct regmap_config qca8386_regmap_config = {
	.reg_bits = 16,
	.val_bits = 32,
	.reg_stride = 4,
	.max_register = QCA8386_MAX_REGISTER,
	.read = qca8386_bulk_read,
	.write = qca8386_bulk_write,
	.reg_update_bits = qca8386_regmap_update_bits,
	.disable_locking = true, /* locking handled by the read/write helpers */
	.cache_type = REGCACHE_NONE,
	.max_raw_read = 32,
	/* ATU regs suffer from a bug where some data is not correctly written;
	 * disable bulk write to correctly write ATU entries.
	 */
	.use_single_write = true,
};

/* ---- register helpers -------------------------------------------------- */

static int qca8386_read(struct qca8386_priv *priv, u32 reg, u32 *val)
{
	return regmap_read(priv->regmap, reg, val);
}

static int qca8386_write(struct qca8386_priv *priv, u32 reg, u32 val)
{
	return regmap_write(priv->regmap, reg, val);
}

static int qca8386_rmw(struct qca8386_priv *priv, u32 reg, u32 mask, u32 write_val)
{
	return regmap_update_bits(priv->regmap, reg, mask, write_val);
}

static int qca8386_read_switch_id(struct qca8386_priv *priv)
{
	u32 val;
	u8 id;
	int ret;

	if (!priv->info)
		return -ENODEV;

	ret = qca8386_read(priv, QCA8386_REG_MASK_CTRL, &val);
	if (ret < 0)
		return -ENODEV;

	id = QCA8386_MASK_CTRL_DEVICE_ID(val);
	if (id != priv->info->id) {
		dev_err(priv->dev,
			"Switch id detected %x but expected %x\n",
			id, priv->info->id);
		return -ENODEV;
	}

	priv->switch_id = id;
	priv->switch_revision = QCA8386_MASK_CTRL_REV_ID(val);

	dev_info(priv->dev, "%s: detected QCA8386 (id 0x%02x rev 0x%02x)\n",
		 priv->info->name, priv->switch_id, priv->switch_revision);

	return 0;
}

/* Retime a port's MAC datapath clocks for the negotiated link speed. The
 * vendor does this in mht_port_speed_clock_set() on every link change; the
 * RCGs pick their own parent from the requested rate, so we only ever set a
 * rate and never a parent.
 */
static void qca8386_port_clk_rate_set(struct qca8386_priv *priv, int port,
				      int speed)
{
	int idx = port * QCA8386_CLKS_PER_PORT;
	unsigned long rate;
	int ret;

	switch (speed) {
	case SPEED_2500:
		rate = QCA8386_CLK_RATE_2500M;
		break;
	case SPEED_1000:
		rate = QCA8386_CLK_RATE_1000M;
		break;
	case SPEED_100:
		rate = QCA8386_CLK_RATE_100M;
		break;
	case SPEED_10:
		rate = QCA8386_CLK_RATE_10M;
		break;
	default:
		return;
	}

	/* tx then rx - the DT order the clk_names table encodes. */
	ret = clk_set_rate(priv->clks[idx].clk, rate);
	if (ret)
		dev_warn(priv->dev, "port %d: tx clock -> %lu Hz failed: %d\n",
			 port, rate, ret);

	ret = clk_set_rate(priv->clks[idx + 1].clk, rate);
	if (ret)
		dev_warn(priv->dev, "port %d: rx clock -> %lu Hz failed: %d\n",
			 port, rate, ret);
}

static void qca8386_port_set_status(struct qca8386_priv *priv, int port, int enable)
{
	u32 mask = QCA8386_PORT_STATUS_TXMAC | QCA8386_PORT_STATUS_RXMAC;

	if (enable)
		qca8386_rmw(priv, QCA8386_REG_PORT_STATUS(port), 0, mask);
	else
		qca8386_rmw(priv, QCA8386_REG_PORT_STATUS(port), mask, 0);
}

/* ---- DSA / phylink ops ------------------------------------------------- */

static enum dsa_tag_protocol
qca8386_get_tag_protocol(struct dsa_switch *ds, int port,
			 enum dsa_tag_protocol mp)
{
	return DSA_TAG_PROTO_QCA;
}

static int qca8386_setup(struct dsa_switch *ds)
{
	struct qca8386_priv *priv = ds->priv;
	struct dsa_port *dp;
	u32 val;
	int ret;

	/* Everything below this point is a write into the switch core, and the
	 * core is still held in reset at this stage: its registers read back 0
	 * and writes are silently dropped (the MDIO window itself works - the
	 * chip ID register reads correctly). The vendor SSDK pulses the switch
	 * core and every MAC reset in qca_mht_switch_reset(), immediately after
	 * the SerDes/interface-mode init and immediately before
	 * qca_switch_init(). Same sequence, same place.
	 */
	ret = reset_control_bulk_reset(QCA8386_NUM_RESETS, priv->resets);
	if (ret) {
		dev_err(priv->dev, "failed to reset switch core: %d\n", ret);
		return ret;
	}

	/* The core needs a moment before it answers on the register bus. */
	usleep_range(1000, 2000);

	/* Ungate the datapath. Nothing else in the tree consumes these, so
	 * without this clk_disable_unused() switches them off again shortly
	 * after boot and no frame ever moves.
	 */
	ret = clk_bulk_prepare_enable(QCA8386_NUM_CLKS, priv->clks);
	if (ret) {
		dev_err(priv->dev, "failed to enable port clocks: %d\n", ret);
		return ret;
	}

	/* Enable the CPU port. */
	ret = qca8386_rmw(priv, QCA8386_REG_GLOBAL_FW_CTRL0,
			  QCA8386_GLOBAL_FW_CTRL0_CPU_PORT_EN,
			  QCA8386_GLOBAL_FW_CTRL0_CPU_PORT_EN);
	if (ret)
		goto err_clk_disable;

	/* QCA header mode (tx+rx) on the CPU port so DSA tagging works. */
	ret = qca8386_rmw(priv, QCA8386_REG_PORT_HDR_CTRL(QCA8386_CPU_PORT),
			  QCA8386_PORT_HDR_CTRL_RX_MASK | QCA8386_PORT_HDR_CTRL_TX_MASK,
			  FIELD_PREP(QCA8386_PORT_HDR_CTRL_RX_MASK, QCA8386_PORT_HDR_CTRL_ALL) |
			  FIELD_PREP(QCA8386_PORT_HDR_CTRL_TX_MASK, QCA8386_PORT_HDR_CTRL_ALL));
	if (ret)
		goto err_clk_disable;

	/* Start from a clean slate: no port forwards anywhere, and every user
	 * port's MAC is off until DSA enables it.
	 */
	dsa_switch_for_each_port(dp, ds) {
		ret = qca8386_rmw(priv, QCA8386_PORT_LOOKUP_CTRL(dp->index),
				  QCA8386_PORT_LOOKUP_MEMBER, 0);
		if (ret)
			goto err_clk_disable;
	}

	dsa_switch_for_each_user_port(dp, ds)
		qca8386_port_set_status(priv, dp->index, 0);

	/* Flood unknown unicast/multicast/broadcast/IGMP to the CPU port. */
	ret = qca8386_write(priv, QCA8386_REG_GLOBAL_FW_CTRL1,
			    FIELD_PREP(QCA8386_FW_CTRL1_IGMP_DP_MASK, dsa_cpu_ports(ds)) |
			    FIELD_PREP(QCA8386_FW_CTRL1_BC_DP_MASK, dsa_cpu_ports(ds)) |
			    FIELD_PREP(QCA8386_FW_CTRL1_MC_DP_MASK, dsa_cpu_ports(ds)) |
			    FIELD_PREP(QCA8386_FW_CTRL1_UC_DP_MASK, dsa_cpu_ports(ds)));
	if (ret)
		goto err_clk_disable;

	/* Standalone user ports: each one talks only to the CPU port, and the
	 * CPU port is a member of each of them. Also give every port a default
	 * egress VID so port-based VLAN behaves.
	 */
	dsa_switch_for_each_user_port(dp, ds) {
		u8 port = dp->index, cpu_port = dp->cpu_dp->index;

		ret = qca8386_rmw(priv, QCA8386_PORT_LOOKUP_CTRL(port),
				  QCA8386_PORT_LOOKUP_MEMBER, BIT(cpu_port));
		if (ret)
			goto err_clk_disable;

		ret = qca8386_rmw(priv, QCA8386_PORT_LOOKUP_CTRL(cpu_port),
				  BIT(port), BIT(port));
		if (ret)
			goto err_clk_disable;

		ret = qca8386_rmw(priv, QCA8386_EGRESS_VLAN(port),
				  QCA8386_EGREES_VLAN_PORT_MASK(port),
				  QCA8386_EGREES_VLAN_PORT(port, QCA8386_PORT_VID_DEF));
		if (ret)
			goto err_clk_disable;

		ret = qca8386_write(priv, QCA8386_REG_PORT_VLAN_CTRL0(port),
				    QCA8386_PORT_VLAN_CVID(QCA8386_PORT_VID_DEF) |
				    QCA8386_PORT_VLAN_SVID(QCA8386_PORT_VID_DEF));
		if (ret)
			goto err_clk_disable;
	}

	/* Read one of the registers we just wrote straight back. Before the
	 * switch-core reset this read returned 0 no matter what we wrote, so it
	 * is the cheapest proof in dmesg that the core is actually alive.
	 */
	ret = qca8386_read(priv, QCA8386_REG_GLOBAL_FW_CTRL0, &val);
	if (ret)
		goto err_clk_disable;

	dev_info(priv->dev,
		 "qca8386 setup complete (CPU port + %d user ports), FW_CTRL0=0x%08x\n",
		 QCA8386_NUM_PORTS - 1, val);

	if (!val)
		dev_warn(priv->dev,
			 "switch core still reads back zero - core may be held in reset\n");

	return 0;

err_clk_disable:
	clk_bulk_disable_unprepare(QCA8386_NUM_CLKS, priv->clks);

	return ret;
}

/* Bridge offload.
 *
 * Without this the switch never learns that lan1..lanN are bridged: setup()
 * leaves every user port a member of the CPU port only, so LAN-to-LAN frames
 * are punted to the CPU, bridged in software and punted back - crossing the
 * 2.5G conduit twice and capping port-to-port throughput at whatever the A53
 * can forward. Joining the ports in the switch's own port-member mask keeps
 * that traffic entirely inside the switch fabric.
 */
static int qca8386_update_port_member(struct qca8386_priv *priv, int port,
				      const struct net_device *bridge_dev,
				      bool join)
{
	struct dsa_port *dp = dsa_to_port(priv->ds, port), *other_dp;
	u32 port_mask = BIT(dp->cpu_dp->index);
	int i, ret;

	for (i = 0; i < QCA8386_NUM_PORTS; i++) {
		if (i == port || dsa_is_cpu_port(priv->ds, i))
			continue;

		other_dp = dsa_to_port(priv->ds, i);
		if (!dsa_port_offloads_bridge_dev(other_dp, bridge_dev))
			continue;

		/* add/remove this port to/from the other port's member mask */
		if (join) {
			port_mask |= BIT(i);
			ret = qca8386_rmw(priv, QCA8386_PORT_LOOKUP_CTRL(i),
					  BIT(port), BIT(port));
		} else {
			ret = qca8386_rmw(priv, QCA8386_PORT_LOOKUP_CTRL(i),
					  BIT(port), 0);
		}
		if (ret)
			return ret;
	}

	/* and all the other bridged ports into this port's member mask */
	return qca8386_rmw(priv, QCA8386_PORT_LOOKUP_CTRL(port),
			   QCA8386_PORT_LOOKUP_MEMBER, port_mask);
}

static int qca8386_port_bridge_join(struct dsa_switch *ds, int port,
				    struct dsa_bridge bridge,
				    bool *tx_fwd_offload,
				    struct netlink_ext_ack *extack)
{
	struct qca8386_priv *priv = ds->priv;

	return qca8386_update_port_member(priv, port, bridge.dev, true);
}

static void qca8386_port_bridge_leave(struct dsa_switch *ds, int port,
				      struct dsa_bridge bridge)
{
	struct qca8386_priv *priv = ds->priv;
	int ret;

	ret = qca8386_update_port_member(priv, port, bridge.dev, false);
	if (ret)
		dev_err(priv->dev,
			"failed to update port member on bridge leave: %d\n",
			ret);
}

static void qca8386_phylink_get_caps(struct dsa_switch *ds, int port,
				     struct phylink_config *config)
{
	switch (port) {
	case QCA8386_CPU_PORT: /* SoC uplink: forced 2.5G SGMII+ */
		__set_bit(PHY_INTERFACE_MODE_SGMII, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_2500BASEX, config->supported_interfaces);
		break;
	case 1:
	case 2:
	case 3:
	case 4: /* user ports face external QCA8084 EPHYs */
		__set_bit(PHY_INTERFACE_MODE_SGMII, config->supported_interfaces);
		__set_bit(PHY_INTERFACE_MODE_2500BASEX, config->supported_interfaces);
		break;
	default:
		dev_warn(ds->dev, "invalid port %d\n", port);
		return;
	}

	config->mac_capabilities = MAC_ASYM_PAUSE | MAC_SYM_PAUSE |
				   MAC_10 | MAC_100 | MAC_1000FD | MAC_2500FD;
}

static void
qca8386_phylink_mac_config(struct phylink_config *config, unsigned int mode,
			   const struct phylink_link_state *state)
{
	/* The CPU-port SerDes is configured by the QCA8084 package driver. */
}

static void
qca8386_phylink_mac_link_down(struct phylink_config *config, unsigned int mode,
			      phy_interface_t interface)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct qca8386_priv *priv = dp->ds->priv;

	qca8386_port_set_status(priv, dp->index, 0);
}

static void
qca8386_phylink_mac_link_up(struct phylink_config *config,
			    struct phy_device *phydev, unsigned int mode,
			    phy_interface_t interface, int speed, int duplex,
			    bool tx_pause, bool rx_pause)
{
	struct dsa_port *dp = dsa_phylink_to_port(config);
	struct qca8386_priv *priv = dp->ds->priv;
	int port = dp->index;
	u32 reg;

	/* Retime the datapath before the MAC is allowed to pass traffic. */
	qca8386_port_clk_rate_set(priv, port, speed);

	if (phylink_autoneg_inband(mode)) {
		reg = QCA8386_PORT_STATUS_LINK_AUTO;
	} else {
		switch (speed) {
		case SPEED_10:
			reg = QCA8386_PORT_STATUS_SPEED_10;
			break;
		case SPEED_100:
			reg = QCA8386_PORT_STATUS_SPEED_100;
			break;
		case SPEED_1000:
			reg = QCA8386_PORT_STATUS_SPEED_1000;
			break;
		case SPEED_2500:
			/* switch-core MAC reuses the 1000 code; rate is set by
			 * the SerDes.
			 */
			reg = QCA8386_PORT_STATUS_SPEED_2500;
			break;
		default:
			reg = QCA8386_PORT_STATUS_LINK_AUTO;
			break;
		}

		if (duplex == DUPLEX_FULL)
			reg |= QCA8386_PORT_STATUS_DUPLEX;

		if (rx_pause || dsa_port_is_cpu(dp))
			reg |= QCA8386_PORT_STATUS_RXFLOW;

		if (tx_pause || dsa_port_is_cpu(dp))
			reg |= QCA8386_PORT_STATUS_TXFLOW;
	}

	reg |= QCA8386_PORT_STATUS_TXMAC | QCA8386_PORT_STATUS_RXMAC;

	qca8386_write(priv, QCA8386_REG_PORT_STATUS(port), reg);
}

static const struct phylink_mac_ops qca8386_phylink_mac_ops = {
	.mac_config	= qca8386_phylink_mac_config,
	.mac_link_down	= qca8386_phylink_mac_link_down,
	.mac_link_up	= qca8386_phylink_mac_link_up,
};

static int qca8386_port_enable(struct dsa_switch *ds, int port,
			       struct phy_device *phy)
{
	qca8386_port_set_status(ds->priv, port, 1);

	return 0;
}

static void qca8386_port_disable(struct dsa_switch *ds, int port)
{
	qca8386_port_set_status(ds->priv, port, 0);
}

/* DSA drives even a standalone user port to BR_STATE_FORWARDING on link-up
 * through this op. Without it PORT_LOOKUP_STATE never leaves its reset value
 * and the user ports stay silent even though their membership is set.
 */
static void qca8386_port_stp_state_set(struct dsa_switch *ds, int port, u8 state)
{
	struct qca8386_priv *priv = ds->priv;
	u32 stp_state;

	switch (state) {
	case BR_STATE_DISABLED:
		stp_state = QCA8386_PORT_LOOKUP_STATE_DISABLED;
		break;
	case BR_STATE_BLOCKING:
		stp_state = QCA8386_PORT_LOOKUP_STATE_BLOCKING;
		break;
	case BR_STATE_LISTENING:
		stp_state = QCA8386_PORT_LOOKUP_STATE_LISTENING;
		break;
	case BR_STATE_LEARNING:
		stp_state = QCA8386_PORT_LOOKUP_STATE_LEARNING;
		break;
	case BR_STATE_FORWARDING:
	default:
		stp_state = QCA8386_PORT_LOOKUP_STATE_FORWARD;
		break;
	}

	qca8386_rmw(priv, QCA8386_PORT_LOOKUP_CTRL(port),
		    QCA8386_PORT_LOOKUP_STATE_MASK, stp_state);
}

static void qca8386_teardown(struct dsa_switch *ds)
{
	struct qca8386_priv *priv = ds->priv;

	clk_bulk_disable_unprepare(QCA8386_NUM_CLKS, priv->clks);
}

static const struct dsa_switch_ops qca8386_switch_ops = {
	.get_tag_protocol	= qca8386_get_tag_protocol,
	.setup			= qca8386_setup,
	.teardown		= qca8386_teardown,
	.phylink_get_caps	= qca8386_phylink_get_caps,
	.port_enable		= qca8386_port_enable,
	.port_disable		= qca8386_port_disable,
	.port_stp_state_set	= qca8386_port_stp_state_set,
	.port_bridge_join	= qca8386_port_bridge_join,
	.port_bridge_leave	= qca8386_port_bridge_leave,
};

/* ---- probe / remove ---------------------------------------------------- */

static int qca8386_sw_probe(struct mdio_device *mdiodev)
{
	struct qca8386_priv *priv;
	int ret, i;

	priv = devm_kzalloc(&mdiodev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = &mdiodev->dev;
	priv->bus = mdiodev->bus;
	priv->info = of_device_get_match_data(priv->dev);
	if (!priv->info)
		return -EINVAL;

	/* Claimed here rather than in setup() so that we defer cleanly if the
	 * nsscc clock/reset provider has not probed yet.
	 */
	for (i = 0; i < QCA8386_NUM_RESETS; i++)
		priv->resets[i].id = qca8386_reset_names[i];

	ret = devm_reset_control_bulk_get_exclusive(priv->dev, QCA8386_NUM_RESETS,
						    priv->resets);
	if (ret)
		return dev_err_probe(priv->dev, ret,
				     "failed to get switch core/MAC resets\n");

	for (i = 0; i < QCA8386_NUM_CLKS; i++)
		priv->clks[i].id = qca8386_clk_names[i];

	ret = devm_clk_bulk_get(priv->dev, QCA8386_NUM_CLKS, priv->clks);
	if (ret)
		return dev_err_probe(priv->dev, ret,
				     "failed to get port datapath clocks\n");

	/* The QCA8386 package reset (tlmm gpio24) is driven by the nsscc node
	 * (qcom,qca8084-nsscc), which owns it exclusively and pulses it in its
	 * own probe. We must NOT request it here as well - the second
	 * devm_gpiod_get() would fail with -EBUSY and take this driver down.
	 */

	priv->regmap = devm_regmap_init(priv->dev, NULL, priv,
					&qca8386_regmap_config);
	if (IS_ERR(priv->regmap)) {
		dev_err(priv->dev, "regmap initialization failed\n");
		return PTR_ERR(priv->regmap);
	}

	mutex_init(&priv->reg_mutex);

	ret = qca8386_read_switch_id(priv);
	if (ret)
		return ret;

	priv->ds = devm_kzalloc(priv->dev, sizeof(*priv->ds), GFP_KERNEL);
	if (!priv->ds)
		return -ENOMEM;

	priv->ds->dev = priv->dev;
	priv->ds->num_ports = QCA8386_NUM_PORTS;
	priv->ds->priv = priv;
	priv->ds->ops = &qca8386_switch_ops;
	priv->ds->phylink_mac_ops = &qca8386_phylink_mac_ops;

	dev_set_drvdata(priv->dev, priv);

	ret = dsa_register_switch(priv->ds);
	if (ret)
		return ret;

	return 0;
}

static void qca8386_sw_remove(struct mdio_device *mdiodev)
{
	struct qca8386_priv *priv = dev_get_drvdata(&mdiodev->dev);
	int i;

	if (!priv)
		return;

	for (i = 0; i < QCA8386_NUM_PORTS; i++)
		qca8386_port_set_status(priv, i, 0);

	dsa_unregister_switch(priv->ds);
}

static void qca8386_sw_shutdown(struct mdio_device *mdiodev)
{
	struct qca8386_priv *priv = dev_get_drvdata(&mdiodev->dev);

	if (!priv)
		return;

	dsa_switch_shutdown(priv->ds);

	dev_set_drvdata(&mdiodev->dev, NULL);
}

static const struct qca8386_info qca8386 = {
	.id = QCA8386_ID,
	.name = "qca8386",
};

static const struct of_device_id qca8386_of_match[] = {
	{ .compatible = "qca,qca8386", .data = &qca8386 },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, qca8386_of_match);

static struct mdio_driver qca8386_driver = {
	.probe = qca8386_sw_probe,
	.remove = qca8386_sw_remove,
	.shutdown = qca8386_sw_shutdown,
	.mdiodrv.driver = {
		.name = "qca8386",
		.of_match_table = qca8386_of_match,
	},
};

mdio_module_driver(qca8386_driver);

MODULE_AUTHOR("Luca C <clubdog123@gmail.com>");
MODULE_DESCRIPTION("Driver for QCA8386 ethernet switch");
MODULE_LICENSE("GPL");
