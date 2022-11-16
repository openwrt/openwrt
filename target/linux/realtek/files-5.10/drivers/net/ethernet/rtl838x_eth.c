// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/drivers/net/ethernet/rtl838x_eth.c
 * Copyright (C) 2020 B. Koblitz
 */

#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/module.h>
#include <linux/phylink.h>
#include <linux/pkt_sched.h>
#include <net/dsa.h>
#include <net/switchdev.h>
#include <asm/cacheflush.h>

#include "rtl838x_eth.h"

/*
 * Maximum number of RX rings is 8 on RTL83XX and 32 on the 93XX
 * The ring is assigned by switch based on packet/port priortity
 * Maximum number of TX rings is 2, Ring 2 being the high priority
 * ring on the RTL93xx SoCs. MAX_RXLEN gives the maximum length
 * for an RX ring, MAX_ENTRIES the maximum number of entries
 * available in total for all queues.
 */
#define RTNC_MAX_RXRINGS	32
#define RTNC_MAX_RXLEN		300
#define RTNC_MAX_ENTRIES	(300 * 8)
#define RTNC_TXRINGS		2
#define RTNC_TXRINGLEN		160
#define RTNC_NOTIFY_EVENTS	10
#define RTNC_NOTIFY_BLOCKS	10
#define RTNC_TX_EN_83XX		0x8
#define RTNC_RX_EN_83XX		0x4
#define RTNC_TX_EN_93XX		0x20
#define RTNC_RX_EN_93XX		0x10
#define RTNC_TX_DO		0x2
#define RTNC_OWN_CPU		0x0
#define RTNC_OWN_ETH		0x1
#define RTNC_WRAP		0x2
#define RTNC_MAX_PORTS		57
#define RTNC_MAX_SMI_BUSSES	4
#define RTNC_RING_BUFFER	1600

struct rtnc_hdr {
	uint8_t			*buf;
	uint16_t		reserved;
	uint16_t		size;		/* buffer size */
	uint16_t		offset;
	uint16_t		len;		/* pkt len */
	uint16_t		cpu_tag[10];	/* tag[0] reserved on RTL83xx */
} __packed __aligned(1);

struct n_event {
	uint32_t		type:2;
	uint32_t		fidVid:12;
	uint64_t		mac:48;
	uint32_t		slp:6;
	uint32_t		valid:1;
	uint32_t		reserved:27;
} __packed __aligned(1);

struct ring_b {
	uint32_t		rx_r[RTNC_MAX_RXRINGS][RTNC_MAX_RXLEN];
	uint32_t		tx_r[RTNC_TXRINGS][RTNC_TXRINGLEN];
	struct rtnc_hdr		rx_header[RTNC_MAX_RXRINGS][RTNC_MAX_RXLEN];
	struct rtnc_hdr		tx_header[RTNC_TXRINGS][RTNC_TXRINGLEN];
	uint32_t		c_rx[RTNC_MAX_RXRINGS];
	uint32_t		c_tx[RTNC_TXRINGS];
};

struct notify_block {
	struct n_event		events[RTNC_NOTIFY_EVENTS];
};

struct notify_b {
	struct notify_block	blocks[RTNC_NOTIFY_BLOCKS];
	u32			reserved1[8];
	u32			ring[RTNC_NOTIFY_BLOCKS];
	u32			reserved2[8];
};

struct rtnc_rx_q {
	int			id;
	struct rtnc_priv	*priv;
	struct napi_struct	napi;
};

struct rtnc_priv {
	struct net_device	*netdev;
	struct platform_device	*pdev;
	dma_addr_t		ring_dma;
	struct ring_b		*ring;
	dma_addr_t		notify_dma;
	struct notify_b		*notify;
	dma_addr_t		rxspace_dma;
	char			*rxspace;
	dma_addr_t		txspace_dma;
	char			*txspace;
	spinlock_t		lock;
	struct mii_bus		*mii_bus;
	struct rtnc_rx_q	rx_qs[RTNC_MAX_RXRINGS];
	struct phylink		*phylink;
	struct phylink_config	phylink_config;
	u16			id;
	u16			family_id;
	const struct rtnc_reg	*r;
	u8			cpu_port;
	u32			lastEvent;
	u16			rxrings;
	u16			rxringlen;
	u8			smi_bus[RTNC_MAX_PORTS];
	u8			smi_addr[RTNC_MAX_PORTS];
	u32			sds_id[RTNC_MAX_PORTS];
	bool			smi_bus_isc45[RTNC_MAX_SMI_BUSSES];
	bool			phy_is_internal[RTNC_MAX_PORTS];
	phy_interface_t		interfaces[RTNC_MAX_PORTS];
};

struct rtnc_dsa_tag {
	u8			reason;
	u8			queue;
	u16			port;
	u8			l2_offloaded;
	u8			prio;
	bool			crc_error;
};

struct fdb_update_work {
	struct work_struct	work;
	struct net_device	*ndev;
	u64			macs[RTNC_NOTIFY_EVENTS + 1];
};

extern int rtl838x_phy_init(struct rtnc_priv *priv);
extern int rtl838x_read_sds_phy(int phy_addr, int phy_reg);
extern int rtl839x_read_sds_phy(int phy_addr, int phy_reg);
extern int rtl839x_write_sds_phy(int phy_addr, int phy_reg, u16 v);
extern int rtl930x_read_sds_phy(int phy_addr, int page, int phy_reg);
extern int rtl930x_write_sds_phy(int phy_addr, int page, int phy_reg, u16 v);
extern int rtl931x_read_sds_phy(int phy_addr, int page, int phy_reg);
extern int rtl931x_write_sds_phy(int phy_addr, int page, int phy_reg, u16 v);
extern int rtl930x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val);
extern int rtl930x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val);
extern int rtl931x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val);
extern int rtl931x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val);

/*
 * ----------------------------------------------------------------------------
 * The following part contains MDIO functions that might justify a separate
 * driver in /drivers/net/mdio. For the time being keep them here.
 */

static int rtl838x_mdio_read_paged(struct mii_bus *bus, int mii_id, u16 page, int regnum)
{
	u32 val;
	int err;
	struct rtnc_priv *priv = bus->priv;

	if (mii_id >= 24 && mii_id <= 27 && priv->id == RTL8380_SOC_ID)
		return rtl838x_read_sds_phy(mii_id, regnum);

	if (regnum & (MII_ADDR_C45 | MII_ADDR_C22_MMD)) {
		err = rtl838x_read_mmd_phy(mii_id,
					   mdiobus_c45_devad(regnum),
					   regnum, &val);
		pr_debug("MMD: %d dev %x register %x read %x, err %d\n", mii_id,
			 mdiobus_c45_devad(regnum), mdiobus_c45_regad(regnum),
			 val, err);
	} else {
		pr_debug("PHY: %d register %x read %x, err %d\n", mii_id, regnum, val, err);
		err = rtl838x_read_phy(mii_id, page, regnum, &val);
	}
	if (err)
		return err;
	return val;
}

static int rtl838x_mdio_read(struct mii_bus *bus, int mii_id, int regnum)
{
	return rtl838x_mdio_read_paged(bus, mii_id, 0, regnum);
}

static int rtl839x_mdio_read_paged(struct mii_bus *bus, int mii_id, u16 page, int regnum)
{
	u32 val;
	int err;
	struct rtnc_priv *priv = bus->priv;

	if (mii_id >= 48 && mii_id <= 49 && priv->id == RTL8393_SOC_ID)
		return rtl839x_read_sds_phy(mii_id, regnum);

	if (regnum & (MII_ADDR_C45 | MII_ADDR_C22_MMD)) {
		err = rtl839x_read_mmd_phy(mii_id,
					   mdiobus_c45_devad(regnum),
					   regnum, &val);
		pr_debug("MMD: %d dev %x register %x read %x, err %d\n", mii_id,
			 mdiobus_c45_devad(regnum), mdiobus_c45_regad(regnum),
			 val, err);
	} else {
		err = rtl839x_read_phy(mii_id, page, regnum, &val);
		pr_debug("PHY: %d register %x read %x, err %d\n", mii_id, regnum, val, err);
	}
		if (err)
		return err;
	return val;
}

static int rtl839x_mdio_read(struct mii_bus *bus, int mii_id, int regnum)
{
	return rtl839x_mdio_read_paged(bus, mii_id, 0, regnum);
}

static int rtl930x_mdio_read_paged(struct mii_bus *bus, int mii_id, u16 page, int regnum)
{
	u32 val;
	int err;
	struct rtnc_priv *priv = bus->priv;

	if (priv->phy_is_internal[mii_id])
		return rtl930x_read_sds_phy(priv->sds_id[mii_id], page, regnum);

	if (regnum & (MII_ADDR_C45 | MII_ADDR_C22_MMD)) {
		err = rtl930x_read_mmd_phy(mii_id,
					   mdiobus_c45_devad(regnum),
					   regnum, &val);
		pr_debug("MMD: %d dev %x register %x read %x, err %d\n", mii_id,
			 mdiobus_c45_devad(regnum), mdiobus_c45_regad(regnum),
			 val, err);
	} else {
		err = rtl930x_read_phy(mii_id, page, regnum, &val);
		pr_debug("PHY: %d register %x read %x, err %d\n", mii_id, regnum, val, err);
	}
	if (err)
		return err;
	return val;
}

static int rtl930x_mdio_read(struct mii_bus *bus, int mii_id, int regnum)
{
	return rtl930x_mdio_read_paged(bus, mii_id, 0, regnum);
}

static int rtl931x_mdio_read_paged(struct mii_bus *bus, int mii_id, u16 page, int regnum)
{
	u32 val;
	int err, v;
	struct rtnc_priv *priv = bus->priv;

	pr_debug("%s: In here, port %d\n", __func__, mii_id);
	if (priv->phy_is_internal[mii_id]) {
		v = rtl931x_read_sds_phy(priv->sds_id[mii_id], page, regnum);
		if (v < 0) {
			err = v;
		} else {
			err = 0;
			val = v;
		}
	} else {
		if (regnum & (MII_ADDR_C45 | MII_ADDR_C22_MMD)) {
			err = rtl931x_read_mmd_phy(mii_id,
						   mdiobus_c45_devad(regnum),
						   regnum, &val);
			pr_debug("MMD: %d dev %x register %x read %x, err %d\n", mii_id,
				 mdiobus_c45_devad(regnum), mdiobus_c45_regad(regnum),
				 val, err);
		} else {
			err = rtl931x_read_phy(mii_id, page, regnum, &val);
			pr_debug("PHY: %d register %x read %x, err %d\n", mii_id, regnum, val, err);
		}
	}

	if (err)
		return err;
	return val;
}

static int rtl931x_mdio_read(struct mii_bus *bus, int mii_id, int regnum)
{
	return rtl931x_mdio_read_paged(bus, mii_id, 0, regnum);
}

static int rtl838x_mdio_write_paged(struct mii_bus *bus, int mii_id, u16 page,
				    int regnum, u16 value)
{
	u32 offset = 0;
	struct rtnc_priv *priv = bus->priv;
	int err;

	if (mii_id >= 24 && mii_id <= 27 && priv->id == RTL8380_SOC_ID) {
		if (mii_id == 26)
			offset = 0x100;
		sw_w32(value, RTL838X_SDS4_FIB_REG0 + offset + (regnum << 2));
		return 0;
	}

	if (regnum & (MII_ADDR_C45 | MII_ADDR_C22_MMD)) {
		err = rtl838x_write_mmd_phy(mii_id, mdiobus_c45_devad(regnum),
					    regnum, value);
		pr_debug("MMD: %d dev %x register %x write %x, err %d\n", mii_id,
			 mdiobus_c45_devad(regnum), mdiobus_c45_regad(regnum),
			 value, err);

		return err;
	}
	err = rtl838x_write_phy(mii_id, page, regnum, value);
	pr_debug("PHY: %d register %x write %x, err %d\n", mii_id, regnum, value, err);
	return err;
}

static int rtl838x_mdio_write(struct mii_bus *bus, int mii_id,
			      int regnum, u16 value)
{
	return rtl838x_mdio_write_paged(bus, mii_id, 0, regnum, value);
}

static int rtl839x_mdio_write_paged(struct mii_bus *bus, int mii_id, u16 page,
				    int regnum, u16 value)
{
	struct rtnc_priv *priv = bus->priv;
	int err;

	if (mii_id >= 48 && mii_id <= 49 && priv->id == RTL8393_SOC_ID)
		return rtl839x_write_sds_phy(mii_id, regnum, value);

	if (regnum & (MII_ADDR_C45 | MII_ADDR_C22_MMD)) {
		err = rtl839x_write_mmd_phy(mii_id, mdiobus_c45_devad(regnum),
					    regnum, value);
		pr_debug("MMD: %d dev %x register %x write %x, err %d\n", mii_id,
			 mdiobus_c45_devad(regnum), mdiobus_c45_regad(regnum),
			 value, err);

		return err;
	}

	err = rtl839x_write_phy(mii_id, page, regnum, value);
	pr_debug("PHY: %d register %x write %x, err %d\n", mii_id, regnum, value, err);
	return err;
}

static int rtl839x_mdio_write(struct mii_bus *bus, int mii_id,
			      int regnum, u16 value)
{
	return rtl839x_mdio_write_paged(bus, mii_id, 0, regnum, value);
}

static int rtl930x_mdio_write_paged(struct mii_bus *bus, int mii_id, u16 page,
				    int regnum, u16 value)
{
	struct rtnc_priv *priv = bus->priv;
	int err;

	if (priv->phy_is_internal[mii_id])
		return rtl930x_write_sds_phy(priv->sds_id[mii_id], page, regnum, value);

	if (regnum & (MII_ADDR_C45 | MII_ADDR_C22_MMD))
		return rtl930x_write_mmd_phy(mii_id, mdiobus_c45_devad(regnum),
					     regnum, value);

	err = rtl930x_write_phy(mii_id, page, regnum, value);
	pr_debug("PHY: %d register %x write %x, err %d\n", mii_id, regnum, value, err);
	return err;
}

static int rtl930x_mdio_write(struct mii_bus *bus, int mii_id,
			      int regnum, u16 value)
{
	return rtl930x_mdio_write_paged(bus, mii_id, 0, regnum, value);
}

static int rtl931x_mdio_write_paged(struct mii_bus *bus, int mii_id, u16 page,
				    int regnum, u16 value)
{
	struct rtnc_priv *priv = bus->priv;
	int err;

	if (priv->phy_is_internal[mii_id])
		return rtl931x_write_sds_phy(priv->sds_id[mii_id], page, regnum, value);

	if (regnum & (MII_ADDR_C45 | MII_ADDR_C22_MMD)) {
		err = rtl931x_write_mmd_phy(mii_id, mdiobus_c45_devad(regnum),
					    regnum, value);
		pr_debug("MMD: %d dev %x register %x write %x, err %d\n", mii_id,
			 mdiobus_c45_devad(regnum), mdiobus_c45_regad(regnum),
			 value, err);

		return err;
	}

	err = rtl931x_write_phy(mii_id, page, regnum, value);
	pr_debug("PHY: %d register %x write %x, err %d\n", mii_id, regnum, value, err);
	return err;
}

static int rtl931x_mdio_write(struct mii_bus *bus, int mii_id,
			      int regnum, u16 value)
{
	return rtl931x_mdio_write_paged(bus, mii_id, 0, regnum, value);
}

static int rtl838x_mdio_reset(struct mii_bus *bus)
{
	pr_debug("%s called\n", __func__);
	/* Disable MAC polling the PHY so that we can start configuration */
	sw_w32(0x00000000, RTL838X_SMI_POLL_CTRL);

	/* Enable PHY control via SoC */
	sw_w32_mask(0, 1 << 15, RTL838X_SMI_GLB_CTRL);

	// Probably should reset all PHYs here...
	return 0;
}

static int rtl839x_mdio_reset(struct mii_bus *bus)
{
	return 0;

	pr_debug("%s called\n", __func__);
	/* BUG: The following does not work, but should! */
	/* Disable MAC polling the PHY so that we can start configuration */
	sw_w32(0x00000000, RTL839X_SMI_PORT_POLLING_CTRL);
	sw_w32(0x00000000, RTL839X_SMI_PORT_POLLING_CTRL + 4);
	/* Disable PHY polling via SoC */
	sw_w32_mask(1 << 7, 0, RTL839X_SMI_GLB_CTRL);

	// Probably should reset all PHYs here...
	return 0;
}

u8 mac_type_bit[RTL930X_CPU_PORT] = {0, 0, 0, 0, 2, 2, 2, 2, 4, 4, 4, 4, 6, 6, 6, 6,
				     8, 8, 8, 8, 10, 10, 10, 10, 12, 15, 18, 21};

static int rtl930x_mdio_reset(struct mii_bus *bus)
{
	int i;
	int pos;
	struct rtnc_priv *priv = bus->priv;
	u32 c45_mask = 0;
	u32 poll_sel[2];
	u32 poll_ctrl = 0;
	u32 private_poll_mask = 0;
	u32 v;
	bool uses_usxgmii = false; // For the Aquantia PHYs
	bool uses_hisgmii = false; // For the RTL8221/8226

	// Mapping of port to phy-addresses on an SMI bus
	poll_sel[0] = poll_sel[1] = 0;
	for (i = 0; i < RTL930X_CPU_PORT; i++) {
		if (priv->smi_bus[i] > 3)
			continue;
		pos = (i % 6) * 5;
		sw_w32_mask(0x1f << pos, priv->smi_addr[i] << pos,
			    RTL930X_SMI_PORT0_5_ADDR + (i / 6) * 4);

		pos = (i * 2) % 32;
		poll_sel[i / 16] |= priv->smi_bus[i] << pos;
		poll_ctrl |= BIT(20 + priv->smi_bus[i]);
	}

	// Configure which SMI bus is behind which port number
	sw_w32(poll_sel[0], RTL930X_SMI_PORT0_15_POLLING_SEL);
	sw_w32(poll_sel[1], RTL930X_SMI_PORT16_27_POLLING_SEL);

	// Disable POLL_SEL for any SMI bus with a normal PHY (not RTL8295R for SFP+)
	sw_w32_mask(poll_ctrl, 0, RTL930X_SMI_GLB_CTRL);

	// Configure which SMI busses are polled in c45 based on a c45 PHY being on that bus
	for (i = 0; i < 4; i++)
		if (priv->smi_bus_isc45[i])
			c45_mask |= BIT(i + 16);

	pr_info("c45_mask: %08x\n", c45_mask);
	sw_w32_mask(0, c45_mask, RTL930X_SMI_GLB_CTRL);

	// Set the MAC type of each port according to the PHY-interface
	// Values are FE: 2, GE: 3, XGE/2.5G: 0(SERDES) or 1(otherwise), SXGE: 0
	v = 0;
	for (i = 0; i < RTL930X_CPU_PORT; i++) {
		switch (priv->interfaces[i]) {
		case PHY_INTERFACE_MODE_10GBASER:
			break;			// Serdes: Value = 0

		case PHY_INTERFACE_MODE_HSGMII:
			private_poll_mask |= BIT(i);
			// fallthrough
		case PHY_INTERFACE_MODE_USXGMII:
			v |= BIT(mac_type_bit[i]);
			uses_usxgmii = true;
			break;

		case PHY_INTERFACE_MODE_QSGMII:
			private_poll_mask |= BIT(i);
			v |= 3 << mac_type_bit[i];
			break;

		default:
			break;
		}
	}
	sw_w32(v, RTL930X_SMI_MAC_TYPE_CTRL);

	// Set the private polling mask for all Realtek PHYs (i.e. not the 10GBit Aquantia ones)
	sw_w32(private_poll_mask, RTL930X_SMI_PRVTE_POLLING_CTRL);

	/* The following magic values are found in the port configuration, they seem to
	 * define different ways of polling a PHY. The below is for the Aquantia PHYs of
	 * the XGS1250 and the RTL8226 of the XGS1210 */
	if (uses_usxgmii) {
		sw_w32(0x01010000, RTL930X_SMI_10GPHY_POLLING_REG0_CFG);
		sw_w32(0x01E7C400, RTL930X_SMI_10GPHY_POLLING_REG9_CFG);
		sw_w32(0x01E7E820, RTL930X_SMI_10GPHY_POLLING_REG10_CFG);
	}
	if (uses_hisgmii) {
		sw_w32(0x011FA400, RTL930X_SMI_10GPHY_POLLING_REG0_CFG);
		sw_w32(0x013FA412, RTL930X_SMI_10GPHY_POLLING_REG9_CFG);
		sw_w32(0x017FA414, RTL930X_SMI_10GPHY_POLLING_REG10_CFG);
	}

	pr_debug("%s: RTL930X_SMI_GLB_CTRL %08x\n", __func__,
		 sw_r32(RTL930X_SMI_GLB_CTRL));
	pr_debug("%s: RTL930X_SMI_PORT0_15_POLLING_SEL %08x\n", __func__,
		 sw_r32(RTL930X_SMI_PORT0_15_POLLING_SEL));
	pr_debug("%s: RTL930X_SMI_PORT16_27_POLLING_SEL %08x\n", __func__,
		 sw_r32(RTL930X_SMI_PORT16_27_POLLING_SEL));
	pr_debug("%s: RTL930X_SMI_MAC_TYPE_CTRL %08x\n", __func__,
		 sw_r32(RTL930X_SMI_MAC_TYPE_CTRL));
	pr_debug("%s: RTL930X_SMI_10GPHY_POLLING_REG0_CFG %08x\n", __func__,
		 sw_r32(RTL930X_SMI_10GPHY_POLLING_REG0_CFG));
	pr_debug("%s: RTL930X_SMI_10GPHY_POLLING_REG9_CFG %08x\n", __func__,
		 sw_r32(RTL930X_SMI_10GPHY_POLLING_REG9_CFG));
	pr_debug("%s: RTL930X_SMI_10GPHY_POLLING_REG10_CFG %08x\n", __func__,
		 sw_r32(RTL930X_SMI_10GPHY_POLLING_REG10_CFG));
	pr_debug("%s: RTL930X_SMI_PRVTE_POLLING_CTRL %08x\n", __func__,
		 sw_r32(RTL930X_SMI_PRVTE_POLLING_CTRL));
	return 0;
}

static int rtl931x_mdio_reset(struct mii_bus *bus)
{
	int i;
	int pos;
	struct rtnc_priv *priv = bus->priv;
	u32 c45_mask = 0;
	u32 poll_sel[4];
	u32 poll_ctrl = 0;
	bool mdc_on[4];

	pr_info("%s called\n", __func__);
	// Disable port polling for configuration purposes
	sw_w32(0, RTL931X_SMI_PORT_POLLING_CTRL);
	sw_w32(0, RTL931X_SMI_PORT_POLLING_CTRL + 4);
	msleep(100);

	mdc_on[0] = mdc_on[1] = mdc_on[2] = mdc_on[3] = false;
	// Mapping of port to phy-addresses on an SMI bus
	poll_sel[0] = poll_sel[1] = poll_sel[2] = poll_sel[3] = 0;
	for (i = 0; i < 56; i++) {
		pos = (i % 6) * 5;
		sw_w32_mask(0x1f << pos, priv->smi_addr[i] << pos, RTL931X_SMI_PORT_ADDR + (i / 6) * 4);
		pos = (i * 2) % 32;
		poll_sel[i / 16] |= priv->smi_bus[i] << pos;
		poll_ctrl |= BIT(20 + priv->smi_bus[i]);
		mdc_on[priv->smi_bus[i]] = true;
	}

	// Configure which SMI bus is behind which port number
	for (i = 0; i < 4; i++) {
		pr_info("poll sel %d, %08x\n", i, poll_sel[i]);
		sw_w32(poll_sel[i], RTL931X_SMI_PORT_POLLING_SEL + (i * 4));
	}

	// Configure which SMI busses
	pr_info("%s: WAS RTL931X_MAC_L2_GLOBAL_CTRL2 %08x\n", __func__, sw_r32(RTL931X_MAC_L2_GLOBAL_CTRL2));
	pr_info("c45_mask: %08x, RTL931X_SMI_GLB_CTRL0 was %X", c45_mask, sw_r32(RTL931X_SMI_GLB_CTRL0));
	for (i = 0; i < 4; i++) {
		// bus is polled in c45
		if (priv->smi_bus_isc45[i])
			c45_mask |= 0x2 << (i * 2);  // Std. C45, non-standard is 0x3
		// Enable bus access via MDC
		if (mdc_on[i])
			sw_w32_mask(0, BIT(9 + i), RTL931X_MAC_L2_GLOBAL_CTRL2);
	}

	pr_info("%s: RTL931X_MAC_L2_GLOBAL_CTRL2 %08x\n", __func__, sw_r32(RTL931X_MAC_L2_GLOBAL_CTRL2));
	pr_info("c45_mask: %08x, RTL931X_SMI_GLB_CTRL0 was %X", c45_mask, sw_r32(RTL931X_SMI_GLB_CTRL0));

	/* We have a 10G PHY enable polling
	sw_w32(0x01010000, RTL931X_SMI_10GPHY_POLLING_SEL2);
	sw_w32(0x01E7C400, RTL931X_SMI_10GPHY_POLLING_SEL3);
	sw_w32(0x01E7E820, RTL931X_SMI_10GPHY_POLLING_SEL4);
*/
	sw_w32_mask(0xff, c45_mask, RTL931X_SMI_GLB_CTRL1);

	return 0;
}

static int rtnc_931x_chip_init(struct rtnc_priv *priv)
{
	pr_info("In %s\n", __func__);

	// Initialize Encapsulation memory and wait until finished
	sw_w32(0x1, RTL931X_MEM_ENCAP_INIT);
	do { } while (sw_r32(RTL931X_MEM_ENCAP_INIT) & 1);
	pr_info("%s: init ENCAP done\n", __func__);

	// Initialize Managemen Information Base memory and wait until finished
	sw_w32(0x1, RTL931X_MEM_MIB_INIT);
	do { } while (sw_r32(RTL931X_MEM_MIB_INIT) & 1);
	pr_info("%s: init MIB done\n", __func__);

	// Initialize ACL (PIE) memory and wait until finished
	sw_w32(0x1, RTL931X_MEM_ACL_INIT);
	do { } while (sw_r32(RTL931X_MEM_ACL_INIT) & 1);
	pr_info("%s: init ACL done\n", __func__);

	// Initialize ALE memory and wait until finished
	sw_w32(0xFFFFFFFF, RTL931X_MEM_ALE_INIT_0);
	do { } while (sw_r32(RTL931X_MEM_ALE_INIT_0));
	sw_w32(0x7F, RTL931X_MEM_ALE_INIT_1);
	sw_w32(0x7ff, RTL931X_MEM_ALE_INIT_2);
	do { } while (sw_r32(RTL931X_MEM_ALE_INIT_2) & 0x7ff);
	pr_info("%s: init ALE done\n", __func__);

	// Enable ESD auto recovery
	sw_w32(0x1, RTL931X_MDX_CTRL_RSVD);

	// Init SPI, is this for thermal control or what?
	sw_w32_mask(0x7 << 11, 0x2 << 11, RTL931X_SPI_CTRL0);

	return 0;
}

static int rtl838x_mdio_init(struct rtnc_priv *priv)
{
	struct device_node *mii_np, *dn;
	u32 pn;
	int ret;

	pr_debug("%s called\n", __func__);
	mii_np = of_get_child_by_name(priv->pdev->dev.of_node, "mdio-bus");

	if (!mii_np) {
		dev_err(&priv->pdev->dev, "no %s child node found", "mdio-bus");
		return -ENODEV;
	}

	if (!of_device_is_available(mii_np)) {
		ret = -ENODEV;
		goto err_put_node;
	}

	priv->mii_bus = devm_mdiobus_alloc(&priv->pdev->dev);
	if (!priv->mii_bus) {
		ret = -ENOMEM;
		goto err_put_node;
	}

	switch(priv->family_id) {
	case RTL8380_FAMILY_ID:
		priv->mii_bus->name = "rtl838x-eth-mdio";
		priv->mii_bus->read = rtl838x_mdio_read;
		priv->mii_bus->read_paged = rtl838x_mdio_read_paged;
		priv->mii_bus->write = rtl838x_mdio_write;
		priv->mii_bus->write_paged = rtl838x_mdio_write_paged;
		priv->mii_bus->reset = rtl838x_mdio_reset;
		break;
	case RTL8390_FAMILY_ID:
		priv->mii_bus->name = "rtl839x-eth-mdio";
		priv->mii_bus->read = rtl839x_mdio_read;
		priv->mii_bus->read_paged = rtl839x_mdio_read_paged;
		priv->mii_bus->write = rtl839x_mdio_write;
		priv->mii_bus->write_paged = rtl839x_mdio_write_paged;
		priv->mii_bus->reset = rtl839x_mdio_reset;
		break;
	case RTL9300_FAMILY_ID:
		priv->mii_bus->name = "rtl930x-eth-mdio";
		priv->mii_bus->read = rtl930x_mdio_read;
		priv->mii_bus->read_paged = rtl930x_mdio_read_paged;
		priv->mii_bus->write = rtl930x_mdio_write;
		priv->mii_bus->write_paged = rtl930x_mdio_write_paged;
		priv->mii_bus->reset = rtl930x_mdio_reset;
		priv->mii_bus->probe_capabilities = MDIOBUS_C22_C45;
		break;
	case RTL9310_FAMILY_ID:
		priv->mii_bus->name = "rtl931x-eth-mdio";
		priv->mii_bus->read = rtl931x_mdio_read;
		priv->mii_bus->read_paged = rtl931x_mdio_read_paged;
		priv->mii_bus->write = rtl931x_mdio_write;
		priv->mii_bus->write_paged = rtl931x_mdio_write_paged;
		priv->mii_bus->reset = rtl931x_mdio_reset;
		priv->mii_bus->probe_capabilities = MDIOBUS_C22_C45;
		break;
	}
	priv->mii_bus->access_capabilities = MDIOBUS_ACCESS_C22_MMD;
	priv->mii_bus->priv = priv;
	priv->mii_bus->parent = &priv->pdev->dev;

	for_each_node_by_name(dn, "ethernet-phy") {
		u32 smi_addr[2];

		if (of_property_read_u32(dn, "reg", &pn))
			continue;

		if (of_property_read_u32_array(dn, "rtl9300,smi-address", &smi_addr[0], 2)) {
			smi_addr[0] = 0;
			smi_addr[1] = pn;
		}

		if (of_property_read_u32(dn, "sds", &priv->sds_id[pn]))
			priv->sds_id[pn] = -1;
		else {
			pr_info("set sds port %d to %d\n", pn, priv->sds_id[pn]);
		}

		if (pn < RTNC_MAX_PORTS) {
			priv->smi_bus[pn] = smi_addr[0];
			priv->smi_addr[pn] = smi_addr[1];
		} else {
			pr_err("%s: illegal port number %d\n", __func__, pn);
		}

		if (of_device_is_compatible(dn, "ethernet-phy-ieee802.3-c45"))
			priv->smi_bus_isc45[smi_addr[0]] = true;

		if (of_property_read_bool(dn, "phy-is-integrated")) {
			priv->phy_is_internal[pn] = true;
		}
	}

	dn = of_find_compatible_node(NULL, NULL, "realtek,rtl83xx-switch");
	if (!dn) {
		dev_err(&priv->pdev->dev, "No RTL switch node in DTS\n");
		return -ENODEV;
	}

	for_each_node_by_name(dn, "port") {
		if (of_property_read_u32(dn, "reg", &pn))
			continue;
		pr_debug("%s Looking at port %d\n", __func__, pn);
		if (pn > priv->cpu_port)
			continue;
		if (of_get_phy_mode(dn, &priv->interfaces[pn]))
			priv->interfaces[pn] = PHY_INTERFACE_MODE_NA;
		pr_debug("%s phy mode of port %d is %s\n", __func__, pn, phy_modes(priv->interfaces[pn]));
	}

	snprintf(priv->mii_bus->id, MII_BUS_ID_SIZE, "%pOFn", mii_np);
	ret = of_mdiobus_register(priv->mii_bus, mii_np);

err_put_node:
	of_node_put(mii_np);
	return ret;
}

static int rtl838x_mdio_remove(struct rtnc_priv *priv)
{
	pr_debug("%s called\n", __func__);
	if (!priv->mii_bus)
		return 0;

	mdiobus_unregister(priv->mii_bus);
	mdiobus_free(priv->mii_bus);

	return 0;
}

/*
 * End of MDIO driver part.
 * ----------------------------------------------------------------------------
 */

static void rtnc_838x_create_tx_header(struct rtnc_hdr *h, unsigned int dest_port, int prio)
{
	// cpu_tag[0] is reserved on the RTL83XX SoCs
	h->cpu_tag[1] = 0x0401;  // BIT 10: RTL8380_CPU_TAG, BIT0: L2LEARNING on
	h->cpu_tag[2] = 0x0200;  // Set only AS_DPM, to enable DPM settings below
	h->cpu_tag[3] = 0x0000;
	h->cpu_tag[4] = BIT(dest_port) >> 16;
	h->cpu_tag[5] = BIT(dest_port) & 0xffff;

	/* Set internal priority (PRI) and enable (AS_PRI) */
	if (prio >= 0)
		h->cpu_tag[2] |= ((prio & 0x7) | BIT(3)) << 12;
}

static void rtnc_839x_create_tx_header(struct rtnc_hdr *h, unsigned int dest_port, int prio)
{
	// cpu_tag[0] is reserved on the RTL83XX SoCs
	h->cpu_tag[1] = 0x0100; // RTL8390_CPU_TAG marker
	h->cpu_tag[2] = BIT(4) | BIT(7); /* AS_DPM (4) and L2LEARNING (7) flags */
	h->cpu_tag[3] = h->cpu_tag[4] = h->cpu_tag[5] = 0;
	// h->cpu_tag[1] |= BIT(1) | BIT(0); // Bypass filter 1/2
	if (dest_port >= 32) {
		dest_port -= 32;
		h->cpu_tag[2] |= (BIT(dest_port) >> 16) & 0xf;
		h->cpu_tag[3] = BIT(dest_port) & 0xffff;
	} else {
		h->cpu_tag[4] = BIT(dest_port) >> 16;
		h->cpu_tag[5] = BIT(dest_port) & 0xffff;
	}

	/* Set internal priority (PRI) and enable (AS_PRI) */
	if (prio >= 0)
		h->cpu_tag[2] |= ((prio & 0x7) | BIT(3)) << 8;
}

static void rtnc_930x_create_tx_header(struct rtnc_hdr *h, unsigned int dest_port, int prio)
{
	h->cpu_tag[0] = 0x8000;  // CPU tag marker
	h->cpu_tag[1] = h->cpu_tag[2] = 0;
	h->cpu_tag[3] = 0;
	h->cpu_tag[4] = 0;
	h->cpu_tag[5] = 0;
	h->cpu_tag[6] = BIT(dest_port) >> 16;
	h->cpu_tag[7] = BIT(dest_port) & 0xffff;

	/* Enable (AS_QID) and set priority queue (QID) */
	if (prio >= 0)
		h->cpu_tag[2] = (BIT(5) | (prio & 0x1f)) << 8;
}

static void rtnc_931x_create_tx_header(struct rtnc_hdr *h, unsigned int dest_port, int prio)
{
	h->cpu_tag[0] = 0x8000;  // CPU tag marker
	h->cpu_tag[1] = h->cpu_tag[2] = 0;
	h->cpu_tag[3] = 0;
	h->cpu_tag[4] = h->cpu_tag[5] = h->cpu_tag[6] = h->cpu_tag[7] = 0;
	if (dest_port >= 32) {
		dest_port -= 32;
		h->cpu_tag[4] = BIT(dest_port) >> 16;
		h->cpu_tag[5] = BIT(dest_port) & 0xffff;
	} else {
		h->cpu_tag[6] = BIT(dest_port) >> 16;
		h->cpu_tag[7] = BIT(dest_port) & 0xffff;
	}

	/* Enable (AS_QID) and set priority queue (QID) */
	if (prio >= 0)
		h->cpu_tag[2] = (BIT(5) | (prio & 0x1f)) << 8;
}

static void rtnc_93xx_header_vlan_set(struct rtnc_hdr *h, int vlan)
{
	h->cpu_tag[2] |= BIT(4); // Enable VLAN forwarding offload
	h->cpu_tag[2] |= (vlan >> 8) & 0xf;
	h->cpu_tag[3] |= (vlan & 0xff) << 8;
}

/*
 * On the RTL93XX, the RTL93XX_DMA_IF_RX_RING_CNTR track the fill level of 
 * the rings. Writing x into these registers substracts x from its content.
 * When the content reaches the ring size, the ASIC no longer adds
 * packets to this receive queue.
 */
void rtnc_838x_update_cntr(int r, int released)
{
	// This feature is not available on RTL838x SoCs
}

void rtnc_839x_update_cntr(int r, int released)
{
	// This feature is not available on RTL839x SoCs
}

void rtnc_930x_update_cntr(int r, int released)
{
	int pos = (r % 3) * 10;
	u32 reg = RTL930X_DMA_IF_RX_RING_CNTR + ((r / 3) << 2);
	u32 v = sw_r32(reg);

	v = (v >> pos) & 0x3ff;
	pr_debug("RX: Work done %d, old value: %d, pos %d, reg %04x\n", released, v, pos, reg);
	sw_w32_mask(0x3ff << pos, released << pos, reg);
	sw_w32(v, reg);
}

void rtnc_931x_update_cntr(int r, int released)
{
	int pos = (r % 3) * 10;
	u32 reg = RTL931X_DMA_IF_RX_RING_CNTR + ((r / 3) << 2);
	u32 v = sw_r32(reg);

	v = (v >> pos) & 0x3ff;
	sw_w32_mask(0x3ff << pos, released << pos, reg);
	sw_w32(v, reg);
}

bool rtnc_838x_decode_tag(struct rtnc_hdr *h, struct rtnc_dsa_tag *t)
{
	/* cpu_tag[0] is reserved. Fields are off-by-one */
	t->reason = h->cpu_tag[4] & 0xf;
	t->queue = (h->cpu_tag[1] & 0xe0) >> 5;
	t->port = h->cpu_tag[1] & 0x1f;
	t->crc_error = t->reason == 13;

	pr_debug("Reason: %d\n", t->reason);
	if (t->reason != 6) // NIC_RX_REASON_SPECIAL_TRAP
		t->l2_offloaded = 1;
	else
		t->l2_offloaded = 0;

	return t->l2_offloaded;
}

bool rtnc_839x_decode_tag(struct rtnc_hdr *h, struct rtnc_dsa_tag *t)
{
	/* cpu_tag[0] is reserved. Fields are off-by-one */
	t->reason = h->cpu_tag[5] & 0x1f;
	t->queue = (h->cpu_tag[4] & 0xe000) >> 13;
	t->port = h->cpu_tag[1] & 0x3f;
	t->crc_error = h->cpu_tag[4] & BIT(6);

	pr_debug("Reason: %d\n", t->reason);
	if ((t->reason >= 7 && t->reason <= 13) || // NIC_RX_REASON_RMA
	    (t->reason >= 23 && t->reason <= 25))  // NIC_RX_REASON_SPECIAL_TRAP
		t->l2_offloaded = 0;
	else
		t->l2_offloaded = 1;

	return t->l2_offloaded;
}

bool rtnc_930x_decode_tag(struct rtnc_hdr *h, struct rtnc_dsa_tag *t)
{
	t->reason = h->cpu_tag[7] & 0x3f;
	t->queue =  (h->cpu_tag[2] >> 11) & 0x1f;
	t->port = (h->cpu_tag[0] >> 8) & 0x1f;
	t->crc_error = h->cpu_tag[1] & BIT(6);

	pr_debug("Reason %d, port %d, queue %d\n", t->reason, t->port, t->queue);
	if (t->reason >= 19 && t->reason <= 27)
		t->l2_offloaded = 0;
	else
		t->l2_offloaded = 1;

	return t->l2_offloaded;
}

bool rtnc_931x_decode_tag(struct rtnc_hdr *h, struct rtnc_dsa_tag *t)
{
	t->reason = h->cpu_tag[7] & 0x3f;
	t->queue =  (h->cpu_tag[2] >> 11) & 0x1f;
	t->port = (h->cpu_tag[0] >> 8) & 0x3f;
	t->crc_error = h->cpu_tag[1] & BIT(6);

	if (t->reason != 63)
		pr_info("%s: Reason %d, port %d, queue %d\n", __func__, t->reason, t->port, t->queue);
	if (t->reason >= 19 && t->reason <= 27)	// NIC_RX_REASON_RMA
		t->l2_offloaded = 0;
	else
		t->l2_offloaded = 1;

	return t->l2_offloaded;
}

/*
 * Discard the RX ring-buffers, called as part of the net-ISR
 * when the buffer runs over
 */
static void rtnc_rb_cleanup(struct rtnc_priv *priv, int status)
{
	int r, idx;
	u32	*last;
	struct rtnc_hdr *h;
	struct ring_b *ring = priv->ring;

	for (r = 0; r < priv->rxrings; r++) {
		pr_debug("In %s working on r: %d\n", __func__, r);
		last = (u32 *)KSEG1ADDR(sw_r32(priv->r->dma_if_rx_cur + r * 4));
		idx = ring->c_rx[r];
		do {
			if ((ring->rx_r[r][idx] & 0x1))
				break;
			pr_debug("Got something: %d\n", idx);
			h = &ring->rx_header[r][idx];
			memset(h, 0, sizeof(struct rtnc_hdr));
			h->buf = (u8 *)KSEG1ADDR(priv->rxspace
					+ r * priv->rxringlen * RTNC_RING_BUFFER
					+ idx * RTNC_RING_BUFFER);
			h->size = RTNC_RING_BUFFER;
			/* make sure the header is visible to the ASIC */
			mb();

			ring->rx_r[r][idx] = KSEG1ADDR(h) |
				(idx == (priv->rxringlen - 1) ? RTNC_OWN_ETH | RTNC_WRAP : RTNC_OWN_ETH);
			idx = (idx + 1) % priv->rxringlen;
		} while (&ring->rx_r[r][idx] != last);
		ring->c_rx[r] = idx;
	}
}

void rtnc_fdb_sync(struct work_struct *work)
{
	const struct fdb_update_work *uw =
		container_of(work, struct fdb_update_work, work);
	struct switchdev_notifier_fdb_info info;
	u8 addr[ETH_ALEN];
	int i = 0;
	int action;

	while (uw->macs[i]) {
		action = (uw->macs[i] & (1ULL << 63)) ? SWITCHDEV_FDB_ADD_TO_BRIDGE
				: SWITCHDEV_FDB_DEL_TO_BRIDGE;
		u64_to_ether_addr(uw->macs[i] & 0xffffffffffffULL, addr);
		info.addr = &addr[0];
		info.vid = 0;
		info.offloaded = 1;
		pr_debug("FDB entry %d: %llx, action %d\n", i, uw->macs[0], action);
		call_switchdev_notifiers(action, uw->ndev, &info.info, NULL);
		i++;
	}
	kfree(work);
}

static void rtnc_839x_l2_notification_handler(struct rtnc_priv *priv)
{
	struct notify_b *nb = priv->notify;
	u32 e = priv->lastEvent;
	struct n_event *event;
	int i;
	u64 mac;
	struct fdb_update_work *w;

	while (!(nb->ring[e] & 1)) {
		w = kzalloc(sizeof(*w), GFP_ATOMIC);
		if (!w) {
			pr_err("Out of memory: %s", __func__);
			return;
		}
		INIT_WORK(&w->work, rtnc_fdb_sync);

		for (i = 0; i < RTNC_NOTIFY_EVENTS; i++) {
			event = &nb->blocks[e].events[i];
			if (!event->valid)
				continue;
			mac = event->mac;
			if (event->type)
				mac |= 1ULL << 63;
			w->ndev = priv->netdev;
			w->macs[i] = mac;
		}

		/* Hand the ring entry back to the switch */
		nb->ring[e] = nb->ring[e] | RTNC_OWN_ETH;
		e = (e + 1) % RTNC_NOTIFY_BLOCKS;

		w->macs[i] = 0ULL;
		schedule_work(&w->work);
	}
	priv->lastEvent = e;
}

static irqreturn_t rtnc_83xx_net_irq(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct rtnc_priv *priv = netdev_priv(dev);
	u32 status = sw_r32(priv->r->dma_if_intr_sts);
	int i;

	pr_debug("IRQ: %08x\n", status);

	/*  TX interrupts are ignored for now */
	if ((status & 0x0f0000)) {
	}

	/* RX interrupt */
	if (status & 0x00ff00) {
		/* Disable RX interrupt until end of NAPI receiving */
		sw_w32_mask(0x00ff00 & status, 0, priv->r->dma_if_intr_msk);
		for (i = 0; i < priv->rxrings; i++) {
			if (status & BIT(i + 8)) {
				pr_debug("Scheduling queue: %d\n", i);
				napi_schedule(&priv->rx_qs[i].napi);
			}
		}
	}

	/* RX buffer overrun interrupts are ignored for now */
	if (status & 0x0000ff) {
	}

	/* Notification interrupts */
	if (status & 0x700000) {
		if (priv->family_id == RTL8390_FAMILY_ID)
			rtnc_839x_l2_notification_handler(priv);
	}

	/* Acknowledge interrupts */
	sw_w32(status, priv->r->dma_if_intr_sts);
	return IRQ_HANDLED;
}

static irqreturn_t rtnc_93xx_net_irq(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct rtnc_priv *priv = netdev_priv(dev);
	u32 status_rx_r = sw_r32(priv->r->dma_if_intr_rx_runout_sts);
	u32 status_rx = sw_r32(priv->r->dma_if_intr_rx_done_sts);
	u32 status_tx = sw_r32(priv->r->dma_if_intr_tx_done_sts);
	int i;

	pr_debug("In %s, status_tx: %08x, status_rx: %08x, status_rx_r: %08x\n",
		__func__, status_tx, status_rx, status_rx_r);

	/*  Ignore TX interrupt */
	if (status_tx) {
		/* Clear ISR */
		pr_debug("TX done\n");
		sw_w32(status_tx, priv->r->dma_if_intr_tx_done_sts);
	}

	/* RX interrupt */
	if (status_rx) {
		pr_debug("RX IRQ\n");
		/* ACK and disable RX interrupt for given rings */
		sw_w32(status_rx, priv->r->dma_if_intr_rx_done_sts);
		sw_w32_mask(status_rx, 0, priv->r->dma_if_intr_rx_done_msk);
		for (i = 0; i < priv->rxrings; i++) {
			if (status_rx & BIT(i)) {
				pr_debug("Scheduling queue: %d\n", i);
				napi_schedule(&priv->rx_qs[i].napi);
			}
		}
	}

	/* RX buffer overrun */
	if (status_rx_r) {
		pr_debug("RX buffer overrun: status %x, mask: %x\n",
			 status_rx_r, sw_r32(priv->r->dma_if_intr_rx_runout_msk));
		sw_w32(status_rx_r, priv->r->dma_if_intr_rx_runout_sts);
		rtnc_rb_cleanup(priv, status_rx_r);
	}

	return IRQ_HANDLED;
}

static const struct rtnc_reg rtnc_838x_reg = {
	.net_irq = rtnc_83xx_net_irq,
	.mac_port_ctrl = rtnc_838x_mac_port_ctrl,
	.dma_if_intr_sts = RTL838X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTL838X_DMA_IF_INTR_MSK,
	.dma_if_ctrl = RTL838X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = RTL838X_MAC_FORCE_MODE_CTRL,
	.dma_rx_base = RTL838X_DMA_RX_BASE,
	.dma_tx_base = RTL838X_DMA_TX_BASE,
	.dma_if_rx_ring_size = rtnc_838x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtnc_838x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = RTL838X_DMA_IF_RX_CUR,
	.rst_glb_ctrl = RTL838X_RST_GLB_CTRL_0,
	.get_mac_link_sts = rtnc_838x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtnc_838x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtnc_838x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtnc_838x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtnc_838x_get_mac_tx_pause_sts,
	.mac = RTL838X_MAC,
	.l2_tbl_flush_ctrl = RTL838X_L2_TBL_FLUSH_CTRL,
	.update_cntr = rtnc_838x_update_cntr,
	.create_tx_header = rtnc_838x_create_tx_header,
	.decode_tag = rtnc_838x_decode_tag,
};

static const struct rtnc_reg rtnc_839x_reg = {
	.net_irq = rtnc_83xx_net_irq,
	.mac_port_ctrl = rtnc_839x_mac_port_ctrl,
	.dma_if_intr_sts = RTL839X_DMA_IF_INTR_STS,
	.dma_if_intr_msk = RTL839X_DMA_IF_INTR_MSK,
	.dma_if_ctrl = RTL839X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = RTL839X_MAC_FORCE_MODE_CTRL,
	.dma_rx_base = RTL839X_DMA_RX_BASE,
	.dma_tx_base = RTL839X_DMA_TX_BASE,
	.dma_if_rx_ring_size = rtnc_839x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtnc_839x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = RTL839X_DMA_IF_RX_CUR,
	.rst_glb_ctrl = RTL839X_RST_GLB_CTRL,
	.get_mac_link_sts = rtnc_839x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtnc_839x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtnc_839x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtnc_839x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtnc_839x_get_mac_tx_pause_sts,
	.mac = RTL839X_MAC,
	.l2_tbl_flush_ctrl = RTL839X_L2_TBL_FLUSH_CTRL,
	.update_cntr = rtnc_839x_update_cntr,
	.create_tx_header = rtnc_839x_create_tx_header,
	.decode_tag = rtnc_839x_decode_tag,
};

static const struct rtnc_reg rtnc_930x_reg = {
	.net_irq = rtnc_93xx_net_irq,
	.mac_port_ctrl = rtnc_930x_mac_port_ctrl,
	.dma_if_intr_rx_runout_sts = RTL930X_DMA_IF_INTR_RX_RUNOUT_STS,
	.dma_if_intr_rx_done_sts = RTL930X_DMA_IF_INTR_RX_DONE_STS,
	.dma_if_intr_tx_done_sts = RTL930X_DMA_IF_INTR_TX_DONE_STS,
	.dma_if_intr_rx_runout_msk = RTL930X_DMA_IF_INTR_RX_RUNOUT_MSK,
	.dma_if_intr_rx_done_msk = RTL930X_DMA_IF_INTR_RX_DONE_MSK,
	.dma_if_intr_tx_done_msk = RTL930X_DMA_IF_INTR_TX_DONE_MSK,
	.l2_ntfy_if_intr_sts = RTL930X_L2_NTFY_IF_INTR_STS,
	.l2_ntfy_if_intr_msk = RTL930X_L2_NTFY_IF_INTR_MSK,
	.dma_if_ctrl = RTL930X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = RTL930X_MAC_FORCE_MODE_CTRL,
	.dma_rx_base = RTL930X_DMA_RX_BASE,
	.dma_tx_base = RTL930X_DMA_TX_BASE,
	.dma_if_rx_ring_size = rtnc_930x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtnc_930x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = RTL930X_DMA_IF_RX_CUR,
	.rst_glb_ctrl = RTL930X_RST_GLB_CTRL_0,
	.get_mac_link_sts = rtnc_930x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtnc_930x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtnc_930x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtnc_930x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtnc_930x_get_mac_tx_pause_sts,
	.mac = RTL930X_MAC_L2_ADDR_CTRL,
	.l2_tbl_flush_ctrl = RTL930X_L2_TBL_FLUSH_CTRL,
	.update_cntr = rtnc_930x_update_cntr,
	.create_tx_header = rtnc_930x_create_tx_header,
	.decode_tag = rtnc_930x_decode_tag,
};

static const struct rtnc_reg rtnc_931x_reg = {
	.net_irq = rtnc_93xx_net_irq,
	.mac_port_ctrl = rtnc_931x_mac_port_ctrl,
	.dma_if_intr_rx_runout_sts = RTL931X_DMA_IF_INTR_RX_RUNOUT_STS,
	.dma_if_intr_rx_done_sts = RTL931X_DMA_IF_INTR_RX_DONE_STS,
	.dma_if_intr_tx_done_sts = RTL931X_DMA_IF_INTR_TX_DONE_STS,
	.dma_if_intr_rx_runout_msk = RTL931X_DMA_IF_INTR_RX_RUNOUT_MSK,
	.dma_if_intr_rx_done_msk = RTL931X_DMA_IF_INTR_RX_DONE_MSK,
	.dma_if_intr_tx_done_msk = RTL931X_DMA_IF_INTR_TX_DONE_MSK,
	.l2_ntfy_if_intr_sts = RTL931X_L2_NTFY_IF_INTR_STS,
	.l2_ntfy_if_intr_msk = RTL931X_L2_NTFY_IF_INTR_MSK,
	.dma_if_ctrl = RTL931X_DMA_IF_CTRL,
	.mac_force_mode_ctrl = RTL931X_MAC_FORCE_MODE_CTRL,
	.dma_rx_base = RTL931X_DMA_RX_BASE,
	.dma_tx_base = RTL931X_DMA_TX_BASE,
	.dma_if_rx_ring_size = rtnc_931x_dma_if_rx_ring_size,
	.dma_if_rx_ring_cntr = rtnc_931x_dma_if_rx_ring_cntr,
	.dma_if_rx_cur = RTL931X_DMA_IF_RX_CUR,
	.rst_glb_ctrl = RTL931X_RST_GLB_CTRL,
	.get_mac_link_sts = rtnc_931x_get_mac_link_sts,
	.get_mac_link_dup_sts = rtnc_931x_get_mac_link_dup_sts,
	.get_mac_link_spd_sts = rtnc_931x_get_mac_link_spd_sts,
	.get_mac_rx_pause_sts = rtnc_931x_get_mac_rx_pause_sts,
	.get_mac_tx_pause_sts = rtnc_931x_get_mac_tx_pause_sts,
	.mac = RTL931X_MAC_L2_ADDR_CTRL,
	.l2_tbl_flush_ctrl = RTL931X_L2_TBL_FLUSH_CTRL,
	.update_cntr = rtnc_931x_update_cntr,
	.create_tx_header = rtnc_931x_create_tx_header,
	.decode_tag = rtnc_931x_decode_tag,
};

static void rtnc_hw_reset(struct rtnc_priv *priv)
{
	u32 int_saved, nbuf;
	u32 reset_mask;
	int i, pos;
	
	pr_info("RESETTING %x, CPU_PORT %d\n", priv->family_id, priv->cpu_port);
	sw_w32_mask(0x3, 0, priv->r->mac_port_ctrl(priv->cpu_port));
	mdelay(100);

	/* Disable and clear interrupts */
	if (priv->family_id == RTL9300_FAMILY_ID || priv->family_id == RTL9310_FAMILY_ID) {
		sw_w32(0x00000000, priv->r->dma_if_intr_rx_runout_msk);
		sw_w32(0xffffffff, priv->r->dma_if_intr_rx_runout_sts);
		sw_w32(0x00000000, priv->r->dma_if_intr_rx_done_msk);
		sw_w32(0xffffffff, priv->r->dma_if_intr_rx_done_sts);
		sw_w32(0x00000000, priv->r->dma_if_intr_tx_done_msk);
		sw_w32(0x0000000f, priv->r->dma_if_intr_tx_done_sts);
	} else {
		sw_w32(0x00000000, priv->r->dma_if_intr_msk);
		sw_w32(0xffffffff, priv->r->dma_if_intr_sts);
	}

	if (priv->family_id == RTL8390_FAMILY_ID) {
		/* Preserve L2 notification and NBUF settings */
		int_saved = sw_r32(priv->r->dma_if_intr_msk);
		nbuf = sw_r32(RTL839X_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL);

		/* Disable link change interrupt on RTL839x */
		sw_w32(0, RTL839X_IMR_PORT_LINK_STS_CHG);
		sw_w32(0, RTL839X_IMR_PORT_LINK_STS_CHG + 4);

		sw_w32(0x00000000, priv->r->dma_if_intr_msk);
		sw_w32(0xffffffff, priv->r->dma_if_intr_sts);
	}

	/* Reset NIC (SW_NIC_RST) and queues (SW_Q_RST) */
	if (priv->family_id == RTL9300_FAMILY_ID || priv->family_id == RTL9310_FAMILY_ID)
		reset_mask = 0x6;
	else
		reset_mask = 0xc;

	sw_w32(reset_mask, priv->r->rst_glb_ctrl);

	do { /* Wait for reset of NIC and Queues done */
		udelay(20);
	} while (sw_r32(priv->r->rst_glb_ctrl) & reset_mask);
	mdelay(100);

	/* Setup Head of Line */
	if (priv->family_id == RTL8380_FAMILY_ID)
		sw_w32(0, RTL838X_DMA_IF_RX_RING_SIZE);  // Disabled on RTL8380
	if (priv->family_id == RTL8390_FAMILY_ID)
		sw_w32(0xffffffff, RTL839X_DMA_IF_RX_RING_CNTR);
	if (priv->family_id == RTL9300_FAMILY_ID || priv->family_id == RTL9310_FAMILY_ID) {
		for (i = 0; i < priv->rxrings; i++) {
			pos = (i % 3) * 10;
			sw_w32_mask(0x3ff << pos, 0, priv->r->dma_if_rx_ring_size(i));
			sw_w32_mask(0x3ff << pos, priv->rxringlen,
				    priv->r->dma_if_rx_ring_cntr(i));
		}
	}

	/* Re-enable link change interrupt */
	if (priv->family_id == RTL8390_FAMILY_ID) {
		sw_w32(0xffffffff, RTL839X_ISR_PORT_LINK_STS_CHG);
		sw_w32(0xffffffff, RTL839X_ISR_PORT_LINK_STS_CHG + 4);
		sw_w32(0xffffffff, RTL839X_IMR_PORT_LINK_STS_CHG);
		sw_w32(0xffffffff, RTL839X_IMR_PORT_LINK_STS_CHG + 4);

		/* Restore notification settings: on RTL838x these bits are null */
		sw_w32_mask(7 << 20, int_saved & (7 << 20), priv->r->dma_if_intr_msk);
		sw_w32(nbuf, RTL839X_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL);
	}
}

static void rtnc_hw_ring_setup(struct rtnc_priv *priv)
{
	int i;
	struct ring_b *ring = priv->ring;

	for (i = 0; i < priv->rxrings; i++)
		sw_w32(KSEG1ADDR(&ring->rx_r[i]), priv->r->dma_rx_base + i * 4);

	for (i = 0; i < RTNC_TXRINGS; i++)
		sw_w32(KSEG1ADDR(&ring->tx_r[i]), priv->r->dma_tx_base + i * 4);
}

static void rtnc_838x_hw_en_rxtx(struct rtnc_priv *priv)
{
	/* Disable Head of Line features for all RX rings */
	sw_w32(0xffffffff, priv->r->dma_if_rx_ring_size(0));

	/* Truncate RX buffer to 0x640 (1600) bytes, pad TX */
	sw_w32(0x06400020, priv->r->dma_if_ctrl);

	/* Enable RX done and RX overflow interrupts */
	sw_w32(0xffff, priv->r->dma_if_intr_msk);

	/* Enable DMA, engine expects empty FCS field */
	sw_w32_mask(0, RTNC_RX_EN_83XX | RTNC_TX_EN_83XX, priv->r->dma_if_ctrl);

	/* Restart TX/RX to CPU port */
	sw_w32_mask(0x0, 0x3, priv->r->mac_port_ctrl(priv->cpu_port));
	/* Set Speed, duplex, flow control
	 * FORCE_EN | LINK_EN | NWAY_EN | DUP_SEL
	 * | SPD_SEL = 0b10 | FORCE_FC_EN | PHY_MASTER_SLV_MANUAL_EN
	 * | MEDIA_SEL
	 */
	sw_w32(0x6192F, priv->r->mac_force_mode_ctrl + priv->cpu_port * 4);

	/* Enable CRC checks on CPU-port */
	sw_w32_mask(0, BIT(3), priv->r->mac_port_ctrl(priv->cpu_port));
}

static void rtnc_839x_hw_en_rxtx(struct rtnc_priv *priv)
{
	/* Setup CPU-Port: RX Buffer */
	sw_w32(0x0000c808, priv->r->dma_if_ctrl);

	/* Enable Notify, RX done and RX overflow interrupts */
	sw_w32(0x0070ffff, priv->r->dma_if_intr_msk); // Notify IRQ!

	/* Enable DMA */
	sw_w32_mask(0, RTNC_RX_EN_83XX | RTNC_TX_EN_83XX, priv->r->dma_if_ctrl);

	/* Restart TX/RX to CPU port, enable CRC checking */
	sw_w32_mask(0x0, 0x3 | BIT(3), priv->r->mac_port_ctrl(priv->cpu_port));

	/* CPU port joins Lookup Miss Flooding Portmask */
	// TODO: The code below should also work for the RTL838x
	sw_w32(0x28000, RTL839X_TBL_ACCESS_L2_CTRL);
	sw_w32_mask(0, 0x80000000, RTL839X_TBL_ACCESS_L2_DATA(0));
	sw_w32(0x38000, RTL839X_TBL_ACCESS_L2_CTRL);

	/* Force CPU port link up */
	sw_w32_mask(0, 3, priv->r->mac_force_mode_ctrl + priv->cpu_port * 4);
}

static void rtnc_93xx_hw_en_rxtx(struct rtnc_priv *priv)
{
	int i, pos;
	u32 v;

	/* Setup CPU-Port: RX Buffer truncated at 1600 Bytes */
	sw_w32(0x06400040, priv->r->dma_if_ctrl);

	for (i = 0; i < priv->rxrings; i++) {
		pos = (i % 3) * 10;
		sw_w32_mask(0x3ff << pos, priv->rxringlen << pos, priv->r->dma_if_rx_ring_size(i));

		// Some SoCs have issues with missing underflow protection
		v = (sw_r32(priv->r->dma_if_rx_ring_cntr(i)) >> pos) & 0x3ff;
		sw_w32_mask(0x3ff << pos, v, priv->r->dma_if_rx_ring_cntr(i));
	}

	/* Enable Notify, RX done, RX overflow and TX done interrupts */
	sw_w32(0xffffffff, priv->r->dma_if_intr_rx_runout_msk);
	sw_w32(0xffffffff, priv->r->dma_if_intr_rx_done_msk);
	sw_w32(0x0000000f, priv->r->dma_if_intr_tx_done_msk);

	/* Enable DMA */
	sw_w32_mask(0, RTNC_RX_EN_93XX | RTNC_TX_EN_93XX, priv->r->dma_if_ctrl);

	/* Restart TX/RX to CPU port, enable CRC checking */
	sw_w32_mask(0x0, 0x3 | BIT(4), priv->r->mac_port_ctrl(priv->cpu_port));

	if (priv->family_id == RTL9300_FAMILY_ID)
		sw_w32_mask(0, BIT(priv->cpu_port), RTL930X_L2_UNKN_UC_FLD_PMSK);
	else
		sw_w32_mask(0, BIT(priv->cpu_port), RTL931X_L2_UNKN_UC_FLD_PMSK);

	if (priv->family_id == RTL9300_FAMILY_ID)
		sw_w32(0x217, priv->r->mac_force_mode_ctrl + priv->cpu_port * 4);
	else
		sw_w32(0x2a1d, priv->r->mac_force_mode_ctrl + priv->cpu_port * 4);
}

static void rtnc_setup_ring_buffer(struct rtnc_priv *priv)
{
	int i, j;
	char *buf;
	struct rtnc_hdr *h;
	struct ring_b *ring = priv->ring;

	buf = (u8 *)KSEG1ADDR(priv->rxspace);
	for (i = 0; i < priv->rxrings; i++) {
		for (j = 0; j < priv->rxringlen; j++, buf += RTNC_RING_BUFFER) {
			h = &ring->rx_header[i][j];
			memset(h, 0, sizeof(struct rtnc_hdr));
			h->buf = buf;
			h->size = RTNC_RING_BUFFER;
			ring->rx_r[i][j] = KSEG1ADDR(h) | RTNC_OWN_ETH;
		}
		ring->rx_r[i][j - 1] |= RTNC_WRAP;
		ring->c_rx[i] = 0;
	}

	buf = (u8 *)KSEG1ADDR(priv->txspace);
	for (i = 0; i < RTNC_TXRINGS; i++) {
		for (j = 0; j < RTNC_TXRINGLEN; j++, buf += RTNC_RING_BUFFER) {
			h = &ring->tx_header[i][j];
			memset(h, 0, sizeof(struct rtnc_hdr));
			h->buf = buf;
			h->size = RTNC_RING_BUFFER;
			ring->tx_r[i][j] = KSEG1ADDR(h) | RTNC_OWN_CPU;
		}
		ring->tx_r[i][j - 1] |= RTNC_WRAP;
		ring->c_tx[i] = 0;
	}
}

static void rtnc_839x_setup_notify_ring_buffer(struct rtnc_priv *priv)
{
	int i;
	struct notify_b *b = priv->notify;

	for (i = 0; i < RTNC_NOTIFY_BLOCKS; i++)
		b->ring[i] = KSEG1ADDR(&b->blocks[i]) | 1 | (i == (RTNC_NOTIFY_BLOCKS - 1) ? RTNC_WRAP : 0);

	sw_w32((u32) b->ring, RTL839X_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL);
	sw_w32_mask(0x3ff << 2, 100 << 2, RTL839X_L2_NOTIFICATION_CTRL);

	/* Setup notification events */
	sw_w32_mask(0, 1 << 14, RTL839X_L2_CTRL_0); // RTL8390_L2_CTRL_0_FLUSH_NOTIFY_EN
	sw_w32_mask(0, 1 << 12, RTL839X_L2_NOTIFICATION_CTRL); // SUSPEND_NOTIFICATION_EN

	/* Enable Notification */
	sw_w32_mask(0, 1 << 0, RTL839X_L2_NOTIFICATION_CTRL);
	priv->lastEvent = 0;
}

static int rtnc_ndo_open(struct net_device *ndev)
{
	unsigned long flags;
	struct rtnc_priv *priv = netdev_priv(ndev);
	int i;

	pr_debug("%s called: RX rings %d(length %d), TX rings %d(length %d)\n",
		__func__, priv->rxrings, priv->rxringlen, RTNC_TXRINGS, RTNC_TXRINGLEN);

	spin_lock_irqsave(&priv->lock, flags);
	rtnc_hw_reset(priv);
	rtnc_setup_ring_buffer(priv);
	if (priv->family_id == RTL8390_FAMILY_ID) {
		rtnc_839x_setup_notify_ring_buffer(priv);
		/* Make sure the ring structure is visible to the ASIC */
		mb();
		flush_cache_all();
	}

	rtnc_hw_ring_setup(priv);
	phylink_start(priv->phylink);

	for (i = 0; i < priv->rxrings; i++)
		napi_enable(&priv->rx_qs[i].napi);

	switch (priv->family_id) {
	case RTL8380_FAMILY_ID:
		rtnc_838x_hw_en_rxtx(priv);
		/* Trap IGMP/MLD traffic to CPU-Port */
		sw_w32(0x3, RTL838X_SPCL_TRAP_IGMP_CTRL);
		/* Flush learned FDB entries on link down of a port */
		sw_w32_mask(0, BIT(7), RTL838X_L2_CTRL_0);
		break;

	case RTL8390_FAMILY_ID:
		rtnc_839x_hw_en_rxtx(priv);
		// Trap MLD and IGMP messages to CPU_PORT
		sw_w32(0x3, RTL839X_SPCL_TRAP_IGMP_CTRL);
		/* Flush learned FDB entries on link down of a port */
		sw_w32_mask(0, BIT(7), RTL839X_L2_CTRL_0);
		break;

	case RTL9300_FAMILY_ID:
		rtnc_93xx_hw_en_rxtx(priv);
		/* Flush learned FDB entries on link down of a port */
		sw_w32_mask(0, BIT(7), RTL930X_L2_CTRL);
		// Trap MLD and IGMP messages to CPU_PORT
		sw_w32((0x2 << 3) | 0x2,  RTL930X_VLAN_APP_PKT_CTRL);
		break;

	case RTL9310_FAMILY_ID:
		rtnc_93xx_hw_en_rxtx(priv);

		// Trap MLD and IGMP messages to CPU_PORT
		sw_w32((0x2 << 3) | 0x2,  RTL931X_VLAN_APP_PKT_CTRL);

		// Disable External CPU access to switch, clear EXT_CPU_EN
		sw_w32_mask(BIT(2), 0, RTL931X_MAC_L2_GLOBAL_CTRL2);

		// Set PCIE_PWR_DOWN
		sw_w32_mask(0, BIT(1), RTL931X_PS_SOC_CTRL);
		break;
	}

	netif_tx_start_all_queues(ndev);

	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

static void rtnc_hw_stop(struct rtnc_priv *priv)
{
	u32 force_mac = priv->family_id == RTL8380_FAMILY_ID ? 0x6192C : 0x75;
	u32 clear_irq = priv->family_id == RTL8380_FAMILY_ID ? 0x000fffff : 0x007fffff;
	int i;

	// Disable RX/TX from/to CPU-port
	sw_w32_mask(0x3, 0, priv->r->mac_port_ctrl(priv->cpu_port));

	/* Disable traffic */
	if (priv->family_id == RTL9300_FAMILY_ID || priv->family_id == RTL9310_FAMILY_ID)
		sw_w32_mask(RTNC_RX_EN_93XX | RTNC_TX_EN_93XX, 0, priv->r->dma_if_ctrl);
	else
		sw_w32_mask(RTNC_RX_EN_83XX | RTNC_TX_EN_83XX, 0, priv->r->dma_if_ctrl);
	mdelay(200); // Test, whether this is needed

	/* Block all ports */
	if (priv->family_id == RTL8380_FAMILY_ID) {
		sw_w32(0x03000000, RTL838X_TBL_ACCESS_DATA_0(0));
		sw_w32(0x00000000, RTL838X_TBL_ACCESS_DATA_0(1));
		sw_w32(1 << 15 | 2 << 12, RTL838X_TBL_ACCESS_CTRL_0);
	}

	/* Flush L2 address cache */
	if (priv->family_id == RTL8380_FAMILY_ID) {
		for (i = 0; i <= priv->cpu_port; i++) {
			sw_w32(1 << 26 | 1 << 23 | i << 5, priv->r->l2_tbl_flush_ctrl);
			do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & (1 << 26));
		}
	} else if (priv->family_id == RTL8390_FAMILY_ID) {
		for (i = 0; i <= priv->cpu_port; i++) {
			sw_w32(1 << 28 | 1 << 25 | i << 5, priv->r->l2_tbl_flush_ctrl);
			do { } while (sw_r32(priv->r->l2_tbl_flush_ctrl) & (1 << 28));
		}
	}
	// TODO: L2 flush register is 64 bit on RTL931X and 930X

	/* CPU-Port: Link down */
	if (priv->family_id == RTL8380_FAMILY_ID || priv->family_id == RTL8390_FAMILY_ID)
		sw_w32(force_mac, priv->r->mac_force_mode_ctrl + priv->cpu_port * 4);
	else if (priv->family_id == RTL9300_FAMILY_ID)
		sw_w32_mask(0x3, 0, priv->r->mac_force_mode_ctrl + priv->cpu_port *4);
	else if (priv->family_id == RTL9310_FAMILY_ID)
		sw_w32_mask(BIT(0) | BIT(9), 0, priv->r->mac_force_mode_ctrl + priv->cpu_port *4);
	mdelay(100);

	/* Disable all TX/RX interrupts */
	if (priv->family_id == RTL9300_FAMILY_ID || priv->family_id == RTL9310_FAMILY_ID) {
		sw_w32(0x00000000, priv->r->dma_if_intr_rx_runout_msk);
		sw_w32(0xffffffff, priv->r->dma_if_intr_rx_runout_sts);
		sw_w32(0x00000000, priv->r->dma_if_intr_rx_done_msk);
		sw_w32(0xffffffff, priv->r->dma_if_intr_rx_done_sts);
		sw_w32(0x00000000, priv->r->dma_if_intr_tx_done_msk);
		sw_w32(0x0000000f, priv->r->dma_if_intr_tx_done_sts);
	} else {
		sw_w32(0x00000000, priv->r->dma_if_intr_msk);
		sw_w32(clear_irq, priv->r->dma_if_intr_sts);
	}

	/* Disable TX/RX DMA */
	sw_w32(0x00000000, priv->r->dma_if_ctrl);
	mdelay(200);
}

static int rtnc_ndo_stop(struct net_device *ndev)
{
	unsigned long flags;
	int i;
	struct rtnc_priv *priv = netdev_priv(ndev);

	pr_info("in %s\n", __func__);

	phylink_stop(priv->phylink);
	rtnc_hw_stop(priv);

	for (i = 0; i < priv->rxrings; i++)
		napi_disable(&priv->rx_qs[i].napi);

	netif_tx_stop_all_queues(ndev);

	return 0;
}

static void rtnc_838x_ndo_set_rx_mode(struct net_device *ndev)
{
	/*
	 * Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(21, 0) = 0x3FFFFF
	 */
	if (!(ndev->flags & (IFF_PROMISC | IFF_ALLMULTI))) {
		sw_w32(0x0, RTL838X_RMA_CTRL_0);
		sw_w32(0x0, RTL838X_RMA_CTRL_1);
	}
	if (ndev->flags & IFF_ALLMULTI)
		sw_w32(GENMASK(21, 0), RTL838X_RMA_CTRL_0);
	if (ndev->flags & IFF_PROMISC) {
		sw_w32(GENMASK(21, 0), RTL838X_RMA_CTRL_0);
		sw_w32(0x7fff, RTL838X_RMA_CTRL_1);
	}
}

static void rtnc_839x_ndo_set_rx_mode(struct net_device *ndev)
{
	/*
	 * Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(31, 2) = 0xFFFFFFFC
	 * Lower two bits are reserved, corresponding to RMA 01-80-C2-00-00-00
	 * CTRL_1_FULL = CTRL_2_FULL = GENMASK(31, 0)
	 */
	if (!(ndev->flags & (IFF_PROMISC | IFF_ALLMULTI))) {
		sw_w32(0x0, RTL839X_RMA_CTRL_0);
		sw_w32(0x0, RTL839X_RMA_CTRL_1);
		sw_w32(0x0, RTL839X_RMA_CTRL_2);
		sw_w32(0x0, RTL839X_RMA_CTRL_3);
	}
	if (ndev->flags & IFF_ALLMULTI) {
		sw_w32(GENMASK(31, 2), RTL839X_RMA_CTRL_0);
		sw_w32(GENMASK(31, 0), RTL839X_RMA_CTRL_1);
		sw_w32(GENMASK(31, 0), RTL839X_RMA_CTRL_2);
	}
	if (ndev->flags & IFF_PROMISC) {
		sw_w32(GENMASK(31, 2), RTL839X_RMA_CTRL_0);
		sw_w32(GENMASK(31, 0), RTL839X_RMA_CTRL_1);
		sw_w32(GENMASK(31, 0), RTL839X_RMA_CTRL_2);
		sw_w32(0x3ff, RTL839X_RMA_CTRL_3);
	}
}

static void rtnc_930x_ndo_set_rx_mode(struct net_device *ndev)
{
	/*
	 * Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(31, 2) = 0xFFFFFFFC
	 * Lower two bits are reserved, corresponding to RMA 01-80-C2-00-00-00
	 * CTRL_1_FULL = CTRL_2_FULL = GENMASK(31, 0)
	 */
	if (ndev->flags & (IFF_ALLMULTI | IFF_PROMISC)) {
		sw_w32(GENMASK(31, 2), RTL930X_RMA_CTRL_0);
		sw_w32(GENMASK(31, 0), RTL930X_RMA_CTRL_1);
		sw_w32(GENMASK(31, 0), RTL930X_RMA_CTRL_2);
	} else {
		sw_w32(0x0, RTL930X_RMA_CTRL_0);
		sw_w32(0x0, RTL930X_RMA_CTRL_1);
		sw_w32(0x0, RTL930X_RMA_CTRL_2);
	}
}

static void rtnc_931x_ndo_set_rx_mode(struct net_device *ndev)
{
	/*
	 * Flood all classes of RMA addresses (01-80-C2-00-00-{01..2F})
	 * CTRL_0_FULL = GENMASK(31, 2) = 0xFFFFFFFC
	 * Lower two bits are reserved, corresponding to RMA 01-80-C2-00-00-00.
	 * CTRL_1_FULL = CTRL_2_FULL = GENMASK(31, 0)
	 */
	if (ndev->flags & (IFF_ALLMULTI | IFF_PROMISC)) {
		sw_w32(GENMASK(31, 2), RTL931X_RMA_CTRL_0);
		sw_w32(GENMASK(31, 0), RTL931X_RMA_CTRL_1);
		sw_w32(GENMASK(31, 0), RTL931X_RMA_CTRL_2);
	} else {
		sw_w32(0x0, RTL931X_RMA_CTRL_0);
		sw_w32(0x0, RTL931X_RMA_CTRL_1);
		sw_w32(0x0, RTL931X_RMA_CTRL_2);
	}
}

static void rtnc_ndo_tx_timeout(struct net_device *ndev, unsigned int txqueue)
{
	unsigned long flags;
	struct rtnc_priv *priv = netdev_priv(ndev);

	pr_warn("%s\n", __func__);
	spin_lock_irqsave(&priv->lock, flags);
	rtnc_hw_stop(priv);
	rtnc_hw_ring_setup(priv);
	rtnc_838x_hw_en_rxtx(priv);
	netif_trans_update(ndev);
	netif_start_queue(ndev);
	spin_unlock_irqrestore(&priv->lock, flags);
}

static int rtnc_ndo_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	int len, i;
	struct rtnc_priv *priv = netdev_priv(dev);
	struct ring_b *ring = priv->ring;
	uint32_t val;
	int ret;
	unsigned long flags;
	struct rtnc_hdr *h;
	int dest_port = -1;
	int q = skb_get_queue_mapping(skb) % RTNC_TXRINGS;

	if (q) // Check for high prio queue
		pr_debug("SKB priority: %d\n", skb->priority);

	spin_lock_irqsave(&priv->lock, flags);
	len = skb->len;

	/* Check for DSA tagging at the end of the buffer */
	if (netdev_uses_dsa(dev) && skb->data[len-4] == 0x80
			&& skb->data[len-3] < priv->cpu_port
			&& skb->data[len-2] == 0x10
			&& skb->data[len-1] == 0x00) {
		/* Reuse tag space for CRC if possible */
		dest_port = skb->data[len-3];
		skb->data[len-4] = skb->data[len-3] = skb->data[len-2] = skb->data[len-1] = 0x00;
		len -= 4;
	}

	len += 4; // Add space for CRC

	if (skb_padto(skb, len)) {
		ret = NETDEV_TX_OK;
		goto txdone;
	}

	/* We can send this packet if CPU owns the descriptor */
	if (!(ring->tx_r[q][ring->c_tx[q]] & RTNC_OWN_ETH)) {

		/* Set descriptor for tx */
		h = &ring->tx_header[q][ring->c_tx[q]];
		h->size = len;
		h->len = len;
		// On RTL8380 SoCs, small packet lengths being sent need adjustments
		if (priv->family_id == RTL8380_FAMILY_ID) {
			if (len < ETH_ZLEN - 4)
				h->len -= 4;
		}

		if (dest_port >= 0)
			priv->r->create_tx_header(h, dest_port, skb->priority >> 1);

		/* Copy packet data to tx buffer */
		memcpy((void *)KSEG1ADDR(h->buf), skb->data, len);
		/* Make sure packet data is visible to ASIC */
		wmb();

		/* Hand over to switch */
		ring->tx_r[q][ring->c_tx[q]] |= RTNC_OWN_ETH;

		// Before starting TX, prevent a Lextra bus bug on RTL8380 SoCs
		if (priv->family_id == RTL8380_FAMILY_ID) {
			for (i = 0; i < 10; i++) {
				val = sw_r32(priv->r->dma_if_ctrl);
				if ((val & 0xc) == 0xc)
					break;
			}
		}

		/* Tell switch to send data */
		if (priv->family_id == RTL9310_FAMILY_ID
			|| priv->family_id == RTL9300_FAMILY_ID) {
			// Ring ID q == 0: Low priority, Ring ID = 1: High prio queue
			if (!q)
				sw_w32_mask(0, BIT(2), priv->r->dma_if_ctrl);
			else
				sw_w32_mask(0, BIT(3), priv->r->dma_if_ctrl);
		} else {
			sw_w32_mask(0, RTNC_TX_DO, priv->r->dma_if_ctrl);
		}

		dev->stats.tx_packets++;
		dev->stats.tx_bytes += len;
		dev_kfree_skb(skb);
		ring->c_tx[q] = (ring->c_tx[q] + 1) % RTNC_TXRINGLEN;
		ret = NETDEV_TX_OK;
	} else {
		dev_warn(&priv->pdev->dev, "Data is owned by switch\n");
		ret = NETDEV_TX_BUSY;
	}
txdone:
	spin_unlock_irqrestore(&priv->lock, flags);
	return ret;
}

/*
 * Return queue number for TX. On the RTL83XX, these queues have equal priority
 * so we do round-robin
 */
u16 rtnc_83xx_ndo_select_queue(struct net_device *dev, struct sk_buff *skb,
			       struct net_device *sb_dev)
{
	static u8 last = 0;

	last++;
	return last % RTNC_TXRINGS;
}

/*
 * Return queue number for TX. On the RTL93XX, queue 1 is the high priority queue
 */
u16 rtnc_93xx_ndo_select_queue(struct net_device *dev, struct sk_buff *skb,
			       struct net_device *sb_dev)
{
	if (skb->priority >= TC_PRIO_CONTROL)
		return 1;
	return 0;
}

static int rtnc_hw_receive(struct net_device *dev, int r, int budget)
{
	struct rtnc_priv *priv = netdev_priv(dev);
	struct ring_b *ring = priv->ring;
	struct sk_buff *skb;
	unsigned long flags;
	int i, len, work_done = 0, idx;
	unsigned int val;
	struct rtnc_hdr *h;
	bool dsa = netdev_uses_dsa(dev);
	struct rtnc_dsa_tag tag;

	pr_debug("---------------------------------------------------------- RX - %d\n", r);
	spin_lock_irqsave(&priv->lock, flags);

	idx = ring->c_rx[r];
	while (!(ring->rx_r[r][idx] & RTNC_OWN_ETH) && (work_done < budget)) {

		h = &ring->rx_header[r][idx];
		len = h->len;
		if (!len)
			break;
		work_done++;

		/* Reuse CRC for DSA tag or strip it otherwise */
		if (!dsa)
			len -= 4;

		skb = napi_alloc_skb(&priv->rx_qs[r].napi, len);

		if (likely(skb)) {
			/* BUG: Prevent bug on RTL838x SoCs*/
			if (priv->family_id == RTL8380_FAMILY_ID) {
				sw_w32(0xffffffff, priv->r->dma_if_rx_ring_size(0));
				for (i = 0; i < priv->rxrings; i++) {
					/* Update each ring cnt */
					val = sw_r32(priv->r->dma_if_rx_ring_cntr(i));
					sw_w32(val, priv->r->dma_if_rx_ring_cntr(i));
				}
			}

			/* Make new data visible for CPU */
			mb();
			dma_sync_single_for_device(&priv->pdev->dev, CPHYSADDR(h->buf), len, DMA_FROM_DEVICE);
			skb_put_data(skb, (u8 *)KSEG0ADDR(h->buf), len);
			/* Overwrite CRC with cpu_tag */
			if (dsa) {
				priv->r->decode_tag(h, &tag);
				skb->data[len-4] = 0x80;
				skb->data[len-3] = tag.port;
				skb->data[len-2] = 0x10;
				skb->data[len-1] = 0x00;
				if (tag.l2_offloaded)
					skb->data[len-3] |= 0x40;
			}

			if (tag.queue >= 0)
				pr_debug("Queue: %d, len: %d, reason %d port %d\n",
					 tag.queue, len, tag.reason, tag.port);

			skb->protocol = eth_type_trans(skb, dev);
			if (dev->features & NETIF_F_RXCSUM) {
				if (tag.crc_error)
					skb_checksum_none_assert(skb);
				else
					skb->ip_summed = CHECKSUM_UNNECESSARY;
			}
			napi_gro_receive(&priv->rx_qs[r].napi, skb);

			dev->stats.rx_packets++;
			dev->stats.rx_bytes += len;
		} else {
			if (net_ratelimit())
				dev_warn(&dev->dev, "low on memory - packet dropped\n");
			dev->stats.rx_dropped++;
		}

		ring->rx_r[r][idx] = KSEG1ADDR(h) |
			(idx == (priv->rxringlen - 1) ? RTNC_OWN_ETH | RTNC_WRAP : RTNC_OWN_ETH);
		idx = (idx + 1) % priv->rxringlen;
	};

	// Update counters
	priv->r->update_cntr(r, 0);
	ring->c_rx[r] = idx;

	spin_unlock_irqrestore(&priv->lock, flags);

	return work_done;
}

static int rtnc_poll_rx(struct napi_struct *napi, int budget)
{
	struct rtnc_rx_q *rx_q = container_of(napi, struct rtnc_rx_q, napi);
	struct rtnc_priv *priv = rx_q->priv;
	int work_done = 0;
	int r = rx_q->id;
	int work;

	while (work_done < budget) {
		work = rtnc_hw_receive(priv->netdev, r, budget - work_done);
		if (!work)
			break;
		work_done += work;
	}

	if (work_done < budget && napi_complete_done(napi, work_done)) {
		/* Enable RX interrupt */
		if (priv->family_id == RTL9300_FAMILY_ID || priv->family_id == RTL9310_FAMILY_ID)
			sw_w32(0xffffffff, priv->r->dma_if_intr_rx_done_msk);
		else
			sw_w32_mask(0, 0x000ff | BIT(r + 8), priv->r->dma_if_intr_msk);
	}
	return work_done;
}


static void rtnc_validate(struct phylink_config *config,
			 unsigned long *supported,
			 struct phylink_link_state *state)
{
	__ETHTOOL_DECLARE_LINK_MODE_MASK(mask) = { 0, };

	pr_debug("In %s\n", __func__);

	if (!phy_interface_mode_is_rgmii(state->interface) &&
	    state->interface != PHY_INTERFACE_MODE_1000BASEX &&
	    state->interface != PHY_INTERFACE_MODE_MII &&
	    state->interface != PHY_INTERFACE_MODE_REVMII &&
	    state->interface != PHY_INTERFACE_MODE_GMII &&
	    state->interface != PHY_INTERFACE_MODE_QSGMII &&
	    state->interface != PHY_INTERFACE_MODE_INTERNAL &&
	    state->interface != PHY_INTERFACE_MODE_SGMII) {
		bitmap_zero(supported, __ETHTOOL_LINK_MODE_MASK_NBITS);
		pr_err("Unsupported interface: %d\n", state->interface);
		return;
	}

	/* Allow all the expected bits */
	phylink_set(mask, Autoneg);
	phylink_set_port_modes(mask);
	phylink_set(mask, Pause);
	phylink_set(mask, Asym_Pause);

	/* With the exclusion of MII and Reverse MII, we support Gigabit,
	 * including Half duplex
	 */
	if (state->interface != PHY_INTERFACE_MODE_MII &&
	    state->interface != PHY_INTERFACE_MODE_REVMII) {
		phylink_set(mask, 1000baseT_Full);
		phylink_set(mask, 1000baseT_Half);
	}

	phylink_set(mask, 10baseT_Half);
	phylink_set(mask, 10baseT_Full);
	phylink_set(mask, 100baseT_Half);
	phylink_set(mask, 100baseT_Full);

	bitmap_and(supported, supported, mask,
		   __ETHTOOL_LINK_MODE_MASK_NBITS);
	bitmap_and(state->advertising, state->advertising, mask,
		   __ETHTOOL_LINK_MODE_MASK_NBITS);
}


static void rtnc_mac_config(struct phylink_config *config,
			       unsigned int mode,
			       const struct phylink_link_state *state)
{
	/* This is only being called for the master device,
	 * i.e. the CPU-Port. We don't need to do anything.
	 */

	pr_info("In %s, mode %x\n", __func__, mode);
}

static void rtnc_mac_an_restart(struct phylink_config *config)
{
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rtnc_priv *priv = netdev_priv(dev);

	/* This works only on RTL838x chips */
	if (priv->family_id != RTL8380_FAMILY_ID)
		return;

	pr_debug("In %s\n", __func__);
	/* Restart by disabling and re-enabling link */
	sw_w32(0x6192D, priv->r->mac_force_mode_ctrl + priv->cpu_port * 4);
	mdelay(20);
	sw_w32(0x6192F, priv->r->mac_force_mode_ctrl + priv->cpu_port * 4);
}

static void rtnc_mac_pcs_get_state(struct phylink_config *config,
				  struct phylink_link_state *state)
{
	u32 speed;
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rtnc_priv *priv = netdev_priv(dev);
	int port = priv->cpu_port;

	pr_info("In %s\n", __func__);

	state->link = priv->r->get_mac_link_sts(port) ? 1 : 0;
	state->duplex = priv->r->get_mac_link_dup_sts(port) ? 1 : 0;

	pr_info("%s link status is %d\n", __func__, state->link);
	speed = priv->r->get_mac_link_spd_sts(port);
	switch (speed) {
	case 0:
		state->speed = SPEED_10;
		break;
	case 1:
		state->speed = SPEED_100;
		break;
	case 2:
		state->speed = SPEED_1000;
		break;
	case 5:
		state->speed = SPEED_2500;
		break;
	case 6:
		state->speed = SPEED_5000;
		break;
	case 4:
		state->speed = SPEED_10000;
		break;
	default:
		state->speed = SPEED_UNKNOWN;
		break;
	}

	state->pause &= (MLO_PAUSE_RX | MLO_PAUSE_TX);
	if (priv->r->get_mac_rx_pause_sts(port))
		state->pause |= MLO_PAUSE_RX;
	if (priv->r->get_mac_tx_pause_sts(port))
		state->pause |= MLO_PAUSE_TX;
}

static void rtnc_mac_link_down(struct phylink_config *config,
				  unsigned int mode,
				  phy_interface_t interface)
{
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rtnc_priv *priv = netdev_priv(dev);

	pr_debug("In %s\n", __func__);
	/* Stop TX/RX to port */
	sw_w32_mask(0x03, 0, priv->r->mac_port_ctrl(priv->cpu_port));
}

static void rtnc_mac_link_up(struct phylink_config *config,
			    struct phy_device *phy, unsigned int mode,
			    phy_interface_t interface, int speed, int duplex,
			    bool tx_pause, bool rx_pause)
{
	struct net_device *dev = container_of(config->dev, struct net_device, dev);
	struct rtnc_priv *priv = netdev_priv(dev);

	pr_debug("In %s\n", __func__);
	/* Restart TX/RX to port */
	sw_w32_mask(0, 0x03, priv->r->mac_port_ctrl(priv->cpu_port));
}

static void rtnc_set_mac_hw(struct net_device *dev, u8 *mac)
{
	struct rtnc_priv *priv = netdev_priv(dev);
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	pr_debug("In %s\n", __func__);
	sw_w32((mac[0] << 8) | mac[1], priv->r->mac);
	sw_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5], priv->r->mac + 4);

	if (priv->family_id == RTL8380_FAMILY_ID) {
		/* 2 more registers, ALE/MAC block */
		sw_w32((mac[0] << 8) | mac[1], RTL838X_MAC_ALE);
		sw_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5],
		       (RTL838X_MAC_ALE + 4));

		sw_w32((mac[0] << 8) | mac[1], RTL838X_MAC2);
		sw_w32((mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5],
		       RTL838X_MAC2 + 4);
	}
	spin_unlock_irqrestore(&priv->lock, flags);
}

static int rtnc_set_mac_address(struct net_device *dev, void *p)
{
	struct rtnc_priv *priv = netdev_priv(dev);
	const struct sockaddr *addr = p;
	u8 *mac = (u8 *) (addr->sa_data);

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
	rtnc_set_mac_hw(dev, mac);

	pr_info("Using MAC %08x%08x\n", sw_r32(priv->r->mac), sw_r32(priv->r->mac + 4));
	return 0;
}

static int rtnc_839x_init_mac(struct rtnc_priv *priv)
{
	// We will need to set-up EEE and the egress-rate limitation
	return 0;
}

static int rtnc_838x_init_mac(struct rtnc_priv *priv)
{
	int i;

	if (priv->family_id == RTL8390_FAMILY_ID)
		return rtnc_839x_init_mac(priv);

    // At present we do not know how to set up EEE on any other SoC than RTL8380
	if (priv->family_id != RTL8380_FAMILY_ID)
		return 0;

	pr_info("%s\n", __func__);
	/* fix timer for EEE */
	sw_w32(0x5001411, RTL838X_EEE_TX_TIMER_GIGA_CTRL);
	sw_w32(0x5001417, RTL838X_EEE_TX_TIMER_GELITE_CTRL);

	/* Init VLAN. TODO: Understand what is being done, here */
	if (priv->id == RTL8382_SOC_ID) {
		for (i = 0; i <= 28; i++)
			sw_w32(0, 0xd57c + i * 0x80);
	}
	if (priv->id == RTL8380_SOC_ID) {
		for (i = 8; i <= 28; i++)
			sw_w32(0, 0xd57c + i * 0x80);
	}
	return 0;
}

static int rtnc_get_link_ksettings(struct net_device *ndev,
				      struct ethtool_link_ksettings *cmd)
{
	struct rtnc_priv *priv = netdev_priv(ndev);

	pr_debug("%s called\n", __func__);
	return phylink_ethtool_ksettings_get(priv->phylink, cmd);
}

static int rtnc_set_link_ksettings(struct net_device *ndev,
				      const struct ethtool_link_ksettings *cmd)
{
	struct rtnc_priv *priv = netdev_priv(ndev);

	pr_debug("%s called\n", __func__);
	return phylink_ethtool_ksettings_set(priv->phylink, cmd);
}

static netdev_features_t rtnc_ndo_fix_features(struct net_device *dev,
					  netdev_features_t features)
{
	return features;
}

static int rtnc_83xx_ndo_set_features(struct net_device *dev, netdev_features_t features)
{
	struct rtnc_priv *priv = netdev_priv(dev);

	if ((features ^ dev->features) & NETIF_F_RXCSUM) {
		if (!(features & NETIF_F_RXCSUM))
			sw_w32_mask(BIT(3), 0, priv->r->mac_port_ctrl(priv->cpu_port));
		else
			sw_w32_mask(0, BIT(3), priv->r->mac_port_ctrl(priv->cpu_port));
	}

	return 0;
}

static int rtnc_93xx_ndo_set_features(struct net_device *dev, netdev_features_t features)
{
	struct rtnc_priv *priv = netdev_priv(dev);

	if ((features ^ dev->features) & NETIF_F_RXCSUM) {
		if (!(features & NETIF_F_RXCSUM))
			sw_w32_mask(BIT(4), 0, priv->r->mac_port_ctrl(priv->cpu_port));
		else
			sw_w32_mask(0, BIT(4), priv->r->mac_port_ctrl(priv->cpu_port));
	}

	return 0;
}

static const struct net_device_ops rtnc_838x_net_device_ops = {
	.ndo_open = rtnc_ndo_open,
	.ndo_stop = rtnc_ndo_stop,
	.ndo_start_xmit = rtnc_ndo_start_xmit,
	.ndo_select_queue = rtnc_83xx_ndo_select_queue,
	.ndo_set_mac_address = rtnc_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rtnc_838x_ndo_set_rx_mode,
	.ndo_tx_timeout = rtnc_ndo_tx_timeout,
	.ndo_set_features = rtnc_83xx_ndo_set_features,
	.ndo_fix_features = rtnc_ndo_fix_features,
	.ndo_setup_tc = rtl83xx_setup_tc,
};

static const struct net_device_ops rtnc_839x_net_device_ops = {
	.ndo_open = rtnc_ndo_open,
	.ndo_stop = rtnc_ndo_stop,
	.ndo_start_xmit = rtnc_ndo_start_xmit,
	.ndo_select_queue = rtnc_83xx_ndo_select_queue,
	.ndo_set_mac_address = rtnc_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rtnc_839x_ndo_set_rx_mode,
	.ndo_tx_timeout = rtnc_ndo_tx_timeout,
	.ndo_set_features = rtnc_83xx_ndo_set_features,
	.ndo_fix_features = rtnc_ndo_fix_features,
	.ndo_setup_tc = rtl83xx_setup_tc,
};

static const struct net_device_ops rtnc_930x_net_device_ops = {
	.ndo_open = rtnc_ndo_open,
	.ndo_stop = rtnc_ndo_stop,
	.ndo_start_xmit = rtnc_ndo_start_xmit,
	.ndo_select_queue = rtnc_93xx_ndo_select_queue,
	.ndo_set_mac_address = rtnc_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rtnc_930x_ndo_set_rx_mode,
	.ndo_tx_timeout = rtnc_ndo_tx_timeout,
	.ndo_set_features = rtnc_93xx_ndo_set_features,
	.ndo_fix_features = rtnc_ndo_fix_features,
	.ndo_setup_tc = rtl83xx_setup_tc,
};

static const struct net_device_ops rtnc_931x_net_device_ops = {
	.ndo_open = rtnc_ndo_open,
	.ndo_stop = rtnc_ndo_stop,
	.ndo_start_xmit = rtnc_ndo_start_xmit,
	.ndo_select_queue = rtnc_93xx_ndo_select_queue,
	.ndo_set_mac_address = rtnc_set_mac_address,
	.ndo_validate_addr = eth_validate_addr,
	.ndo_set_rx_mode = rtnc_931x_ndo_set_rx_mode,
	.ndo_tx_timeout = rtnc_ndo_tx_timeout,
	.ndo_set_features = rtnc_93xx_ndo_set_features,
	.ndo_fix_features = rtnc_ndo_fix_features,
};

static const struct phylink_mac_ops rtnc_phylink_mac_ops = {
	.validate = rtnc_validate,
	.mac_pcs_get_state = rtnc_mac_pcs_get_state,
	.mac_an_restart = rtnc_mac_an_restart,
	.mac_config = rtnc_mac_config,
	.mac_link_down = rtnc_mac_link_down,
	.mac_link_up = rtnc_mac_link_up,
};

static const struct ethtool_ops rtnc_ethtool_ops = {
	.get_link_ksettings = rtnc_get_link_ksettings,
	.set_link_ksettings = rtnc_set_link_ksettings,
};

static void rtnc_soc_info(int *soc_id, int *soc_family)
{
	int id;

	id = sw_r32(RTL838X_MODEL_NAME_INFO);
	id = id >> 16 & 0xFFFF;

	switch (id) {
	case RTL8328_SOC_ID:
		*soc_id = id;
		*soc_family = RTL8328_FAMILY_ID;
		return;
	case RTL8332_SOC_ID:
	case RTL8380_SOC_ID:
	case RTL8382_SOC_ID:
		*soc_id = id;
		*soc_family = RTL8380_FAMILY_ID;
		return;
	}

	id = sw_r32(RTL839X_MODEL_NAME_INFO);
	id = id >> 16 & 0xFFFF;

	switch (id) {
	case RTL8390_SOC_ID:
	case RTL8391_SOC_ID:
	case RTL8392_SOC_ID:
	case RTL8393_SOC_ID:
		*soc_id = id;
		*soc_family = RTL8390_FAMILY_ID;
		return;
	}

	id = sw_r32(RTL93XX_MODEL_NAME_INFO);
	id = id >> 16 & 0xFFFF;

	switch (id) {
	case RTL9301_SOC_ID:
	case RTL9302_SOC_ID:
	case RTL9303_SOC_ID:
		*soc_id = id;
		*soc_family = RTL9300_FAMILY_ID;
		return;
	case RTL9313_SOC_ID:
		*soc_id = id;
		*soc_family = RTL9310_FAMILY_ID;
		return;
	}

	*soc_id = 0;
	*soc_family = 1;
}

static int __init rtnc_probe(struct platform_device *pdev)
{
	struct net_device *dev;
	struct device_node *dn = pdev->dev.of_node;
	struct rtnc_priv *priv;
	struct resource *res, *mem;
	phy_interface_t phy_mode;
	struct phylink *phylink;
	int err = 0, i, rxrings, rxringlen;
	int soc_id, soc_family;

	pr_info("Probing Realtek SoC ethernet pdev: %x, dev: %x\n",
		(u32)pdev, (u32)(&(pdev->dev)));

	if (!dn) {
		dev_err(&pdev->dev, "No DT found\n");
		return -EINVAL;
	}

	rtnc_soc_info(&soc_id, &soc_family);
	if (soc_id) {
		pr_info("Realtek SoC ethernet ID %4x, family %4x\n",
			soc_id, soc_family);
	} else {
		pr_err("Realtek SoC ethernet ID not detected\n");
		return -ENODEV;
	}

	rxrings = (soc_family == RTL8380_FAMILY_ID 
			|| soc_family == RTL8390_FAMILY_ID) ? 8 : 32;
	rxrings = rxrings > RTNC_MAX_RXRINGS ? RTNC_MAX_RXRINGS : rxrings;
	rxringlen = RTNC_MAX_ENTRIES / rxrings;
	rxringlen = rxringlen > RTNC_MAX_RXLEN ? RTNC_MAX_RXLEN : rxringlen;

	dev = alloc_etherdev_mqs(sizeof(struct rtnc_priv), RTNC_TXRINGS, rxrings);
	if (!dev) {
		err = -ENOMEM;
		goto err_free;
	}
	SET_NETDEV_DEV(dev, &pdev->dev);
	priv = netdev_priv(dev);

	priv->notify = dmam_alloc_coherent(&pdev->dev, sizeof(struct notify_b),
					    &priv->notify_dma, GFP_KERNEL);
	if (!priv->notify) {
		dev_err(&pdev->dev, "cannot allocate notify buffer\n");
		err = -ENOMEM;
		goto err_free;
	}

	priv->rxspace = dmam_alloc_coherent(&pdev->dev, rxrings * rxringlen * RTNC_RING_BUFFER,
					    &priv->rxspace_dma, GFP_KERNEL);
	if (!priv->notify) {
		dev_err(&pdev->dev, "cannot allocate RX buffer\n");
		err = -ENOMEM;
		goto err_free;
	}

	priv->txspace = dmam_alloc_coherent(&pdev->dev, RTNC_TXRINGS * RTNC_TXRINGLEN * RTNC_RING_BUFFER,
					    &priv->txspace_dma, GFP_KERNEL);
	if (!priv->notify) {
		dev_err(&pdev->dev, "cannot allocate TX buffer\n");
		err = -ENOMEM;
		goto err_free;
	}

	priv->ring = dmam_alloc_coherent(&pdev->dev, sizeof(struct ring_b),
					    &priv->ring_dma, GFP_KERNEL);
	if (!priv->ring) {
		dev_err(&pdev->dev, "cannot allocate ring buffer\n");
		err = -ENOMEM;
		goto err_free;
	}

	spin_lock_init(&priv->lock);

	dev->ethtool_ops = &rtnc_ethtool_ops;
	dev->min_mtu = ETH_ZLEN;
	dev->max_mtu = 1536;
	dev->features = NETIF_F_RXCSUM | NETIF_F_HW_CSUM;
	dev->hw_features = NETIF_F_RXCSUM;

	priv->id = soc_id;
	priv->family_id = soc_family;

	switch (priv->family_id) {
	case RTL8380_FAMILY_ID:
		priv->cpu_port = RTL838X_CPU_PORT;
		priv->r = &rtnc_838x_reg;
		dev->netdev_ops = &rtnc_838x_net_device_ops;
		break;
	case RTL8390_FAMILY_ID:
		priv->cpu_port = RTL839X_CPU_PORT;
		priv->r = &rtnc_839x_reg;
		dev->netdev_ops = &rtnc_839x_net_device_ops;
		break;
	case RTL9300_FAMILY_ID:
		priv->cpu_port = RTL930X_CPU_PORT;
		priv->r = &rtnc_930x_reg;
		dev->netdev_ops = &rtnc_930x_net_device_ops;
		break;
	case RTL9310_FAMILY_ID:
		priv->cpu_port = RTL931X_CPU_PORT;
		priv->r = &rtnc_931x_reg;
		dev->netdev_ops = &rtnc_931x_net_device_ops;
		rtnc_931x_chip_init(priv);
		break;
	default:
		pr_err("Unknown SoC family\n");
		return -ENODEV;
	}
	priv->rxringlen = rxringlen;
	priv->rxrings = rxrings;

	/* Obtain device IRQ number */
	dev->irq = platform_get_irq(pdev, 0);
	if (dev->irq < 0) {
		dev_err(&pdev->dev, "cannot obtain network-device IRQ\n");
		goto err_free;
	}

	err = devm_request_irq(&pdev->dev, dev->irq, priv->r->net_irq,
			       IRQF_SHARED, dev->name, dev);
	if (err) {
		dev_err(&pdev->dev, "%s: could not acquire interrupt: %d\n",
			   __func__, err);
		goto err_free;
	}

	rtnc_838x_init_mac(priv);

	/* try to get mac address in the following order:
	 * 1) from device tree data
	 * 2) from internal registers set by bootloader
	 */
	of_get_mac_address(pdev->dev.of_node, dev->dev_addr);
	if (is_valid_ether_addr(dev->dev_addr)) {
		rtnc_set_mac_hw(dev, (u8 *)dev->dev_addr);
	} else {
		dev->dev_addr[0] = (sw_r32(priv->r->mac) >> 8) & 0xff;
		dev->dev_addr[1] = sw_r32(priv->r->mac) & 0xff;
		dev->dev_addr[2] = (sw_r32(priv->r->mac + 4) >> 24) & 0xff;
		dev->dev_addr[3] = (sw_r32(priv->r->mac + 4) >> 16) & 0xff;
		dev->dev_addr[4] = (sw_r32(priv->r->mac + 4) >> 8) & 0xff;
		dev->dev_addr[5] = sw_r32(priv->r->mac + 4) & 0xff;
	}
	/* if the address is invalid, use a random value */
	if (!is_valid_ether_addr(dev->dev_addr)) {
		struct sockaddr sa = { AF_UNSPEC };

		netdev_warn(dev, "Invalid MAC address, using random\n");
		eth_hw_addr_random(dev);
		memcpy(sa.sa_data, dev->dev_addr, ETH_ALEN);
		if (rtnc_set_mac_address(dev, &sa))
			netdev_warn(dev, "Failed to set MAC address.\n");
	}
	pr_info("Using MAC %08x%08x\n", sw_r32(priv->r->mac),
					sw_r32(priv->r->mac + 4));
	strcpy(dev->name, "eth%d");
	priv->pdev = pdev;
	priv->netdev = dev;

	err = rtl838x_mdio_init(priv);
	if (err)
		goto err_free;

	err = register_netdev(dev);
	if (err)
		goto err_free;

	for (i = 0; i < priv->rxrings; i++) {
		priv->rx_qs[i].id = i;
		priv->rx_qs[i].priv = priv;
		netif_napi_add(dev, &priv->rx_qs[i].napi, rtnc_poll_rx, NAPI_POLL_WEIGHT);
	}

	platform_set_drvdata(pdev, dev);

	phy_mode = PHY_INTERFACE_MODE_NA;
	err = of_get_phy_mode(dn, &phy_mode);
	if (err < 0) {
		dev_err(&pdev->dev, "incorrect phy-mode\n");
		err = -EINVAL;
		goto err_free;
	}
	priv->phylink_config.dev = &dev->dev;
	priv->phylink_config.type = PHYLINK_NETDEV;

	phylink = phylink_create(&priv->phylink_config, pdev->dev.fwnode,
				 phy_mode, &rtnc_phylink_mac_ops);

	if (IS_ERR(phylink)) {
		err = PTR_ERR(phylink);
		goto err_free;
	}
	priv->phylink = phylink;

	return 0;

err_free:
	pr_err("Error setting up netdev, freeing it again.\n");
	free_netdev(dev);
	return err;
}

static int rtnc_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	struct rtnc_priv *priv = netdev_priv(dev);
	int i;

	if (dev) {
		pr_info("Removing platform driver for rtl838x-eth\n");
		rtl838x_mdio_remove(priv);
		rtnc_hw_stop(priv);

		netif_tx_stop_all_queues(dev);

		for (i = 0; i < priv->rxrings; i++)
			netif_napi_del(&priv->rx_qs[i].napi);

		unregister_netdev(dev);
		free_netdev(dev);
	}
	return 0;
}

static const struct of_device_id rtnc_of_ids[] = {
	{ .compatible = "realtek,rtl838x-eth"},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtnc_of_ids);

static struct platform_driver rtnc_driver = {
	.probe = rtnc_probe,
	.remove = rtnc_remove,
	.driver = {
		.name = "rtl838x-eth",
		.pm = NULL,
		.of_match_table = rtnc_of_ids,
	},
};

module_platform_driver(rtnc_driver);

MODULE_AUTHOR("B. Koblitz");
MODULE_DESCRIPTION("RTL838X SoC Ethernet Driver");
MODULE_LICENSE("GPL");
