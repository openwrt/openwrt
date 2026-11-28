// SPDX-License-Identifier: GPL-2.0-only
/* DSA switch driver for the Realtek RTL9607C family ethernet switch.
 *
 * The RTL9607C family of SoCs has an embedded 11 port switch controller.
 * It includes includes 4 integrated PHYs for the user facing ports, 1 Serdes
 * interface to operate with an external PON transciever, 1 HSGMII interface
 * that can connected to an external PHY but is muxed with USB3, 2nd HSGMII
 * interface that can be connected to external PHY, or be a CPU NIC port, but
 * is muxed with 2nd PCIE and lastly 2 primary CPU NIC ports.
 */

#include <linux/bitfield.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <net/dsa.h>

/* Family-specific data and limits */
#define RTL9607C_PHYADDRMAX		3
#define RTL9607C_PAGE_SELECT		31
#define RTL9607C_MAX_NUM_PORTS		11

/* Chip identification registers */
#define RTL9607C_CHIP_INFO_REG				0x10000
#define RTL9607C_CHIP_INFO_EN_MASK			GENMASK(31, 28)
#define   RTL9607C_CHIP_INFO_EN_VALUE			0xa
#define RTL9607C_CHIP_INFO_VER_MASK			GENMASK(20, 16)
#define RTL9607C_CHIP_INFO_ID_MASK			GENMASK(15, 0)

/* Chip reset register */
#define RTL9607C_SOFTWARE_RESET_REG			0x108
#define   RTL9607C_SOFTWARE_RESET_PONMAC_MASK		BIT(11)
#define   RTL9607C_SOFTWARE_RESET_SWITCHCORE_MASK	BIT(10)
#define   RTL9607C_SOFTWARE_RESET_GLOBAL_MASK		BIT(9)
#define   RTL9607C_SOFTWARE_RESET_CONFIG_MASK		BIT(8)
#define   RTL9607C_SOFTWARE_RESET_CHIP_MASK		BIT(7)
#define   RTL9607C_SOFTWARE_RESET_GPHY_MASK		BIT(6)
#define   RTL9607C_SOFTWARE_RESET_SERDES_MASK		GENMASK(2, 0)

/* Interrupt polarity register */
#define RTL9607C_INTR_POLARITY_REG	0x1d000
#define   RTL9607C_INTR_POLARITY_MASK	BIT(0)
#define   RTL9607C_INTR_POLARITY_HIGH	0
#define   RTL9607C_INTR_POLARITY_LOW	1

/* Interrupt mask/status register - enable/check specific interrupt types */
#define RTL9607C_INTR_IMR_REG			0x1d00c
#define RTL9607C_INTR_IMS_REG			0x1d010
#define   RTL9607C_INTR_TOD_UPDATE_MASK		BIT(20)
#define   RTL9607C_INTR_SFP_MASK		BIT(19)
#define   RTL9607C_INTR_TOD_MASK		BIT(18)
#define   RTL9607C_INTR_FB_EVENT_MASK		BIT(17)
#define   RTL9607C_INTR_SWITCH_CRASH_MASK	BIT(16)
#define   RTL9607C_INTR_SMARTCARD_MASK		BIT(15)
#define   RTL9607C_INTR_THERMAL_ALARM_MASK	BIT(14)
#define   RTL9607C_INTR_DYING_GASP_MASK		BIT(13)
#define   RTL9607C_INTR_PTP_1_SEC_MASK		BIT(12)
#define   RTL9607C_INTR_EPON_MASK		BIT(11)
#define   RTL9607C_INTR_GPON_MASK		BIT(10)
#define   RTL9607C_INTR_SERDES_MASK		BIT(9)
#define   RTL9607C_INTR_GPHY_MASK		BIT(8)
#define   RTL9607C_INTR_ACL_ACTION_MASK		BIT(7)
#define   RTL9607C_INTR_DBGO_MASK		BIT(6)
#define   RTL9607C_INTR_LOOP_DETECTION_MASK	BIT(5)
#define   RTL9607C_INTR_SPECIAL_CONGEST_MASK	BIT(4)
#define   RTL9607C_INTR_SPEED_CHANGE_MASK	BIT(3)
#define   RTL9607C_INTR_LEARN_OVER_MASK		BIT(2)
#define   RTL9607C_INTR_METER_EXCEEDED_MASK	BIT(1)
#define   RTL9607C_INTR_LINK_CHANGE_MASK	BIT(0)
/* The define would be too big listing everything above so genmask it is */
#define   RTL9607C_INTR_ALL_MASK		GENMASK(20, 0)

#define RTL9607C_PORT_MASK	GENMASK(10, 0)

/* Per-port interrupt type status registers */
#define RTL9607C_INTR_STATUS_REG			0x1d014
#define   RTL9607C_INTR_STAT_THERMAL_MASK		GENMASK(31, 28)
#define   RTL9607C_INTR_STAT_GPHY_MASK			GENMASK(20, 16)
#define   RTL9607C_INTR_STAT_PORT_LINKDOWN_MASK		RTL9607C_PORT_MASK
#define RTL9607C_INTR_STATUS1_REG			0x1d018
#define   RTL9607C_INTR_STAT_PORT_LINKUP_MASK		(RTL9607C_PORT_MASK << 16)
#define   RTL9607C_INTR_STAT_PORT_SPEED_CHANGE_MASK	RTL9607C_PORT_MASK

/* PHY indirect access registers */
#define RTL9607C_GPHY_INDIRECT_WRITE_DATA_REG		0x0
#define RTL9607C_GPHY_INDIRECT_COMMAND_REG		0x4
#define   RTL9607C_INDIRECT_COMMAND_RW_MASK		BIT(22)
#define     RTL9607C_INDIRECT_COMMAND_RW_READ		0
#define     RTL9607C_INDIRECT_COMMAND_RW_WRITE		1
#define   RTL9607C_INDIRECT_COMMAND_RUN			BIT(21)
#define   RTL9607C_INDIRECT_COMMAND_PHYID_MASK		GENMASK(20, 16)
#define   RTL9607C_INDIRECT_COMMAND_OCP_ADDR_MASK	GENMASK(15, 0)
#define RTL9607C_GPHY_INDIRECT_READ_DATA_REG		0x8
#define   RTL9607C_GPHY_STATUS_BUSY			BIT(16)
#define   RTL9607C_GPHY_READ_DATA_MASK			GENMASK(15, 0)

#define RTL9607C_WRAP_GPHY_MISC_REG		0x114
#define   RTL9607C_PHY_PATCH_DONE_MASK		BIT(0)

/* The PHY OCP addresses of PHY registers 0~16, 24~31 start here */
#define RTL9607C_PHY_OCP_ADDR_PHYREG_BASE	0xa400

/* Forced MAC ability of a port */
#define RTL9607C_FORCE_PORT_ABILITY_REG(_port)	(0x1cc + (_port) * 4)
#define   RTL9607C_FORCE_PORT_ABILITY_NWAY		BIT(7)
#define   RTL9607C_FORCE_PORT_ABILITY_LINK		BIT(4)
#define   RTL9607C_FORCE_PORT_ABILITY_DUPLEX		BIT(2)
#define   RTL9607C_FORCE_PORT_ABILITY_SPEED_MASK	GENMASK(1, 0)
#define   RTL9607C_FORCE_PORT_ABILITY_SPEED_1000	2
#define RTL9607C_ABILITY_FORCE_MODE_REG(_port)	(0x238 + (_port) * 4)

/* CPU tag control register */
#define RTL9607C_CPU_TAG_INSERT_CTRL_REG	0x230f4
#define RTL9607C_CPU_TAG_AWARE_CTRL_REG		0x230f8

/* Maximum packet length register */
#define RTL9607C_ACCEPT_MAX_LEN_REG(_port)	(0x11028 + (_port) * 4)
#define   RTL9607C_ACCEPT_MAX_LEN_MASK		GENMASK(13, 0)
#define RTL9607C_MAX_PACKET_LENGTH		0x3fff

/* Ethernet header, two stacked VLAN tags (802.1ad QinQ) and FCS */
#define RTL9607C_FRAME_OVERHEAD		(ETH_HLEN + 2 * VLAN_HLEN + ETH_FCS_LEN)

/* Port isolation (forwarding mask) registers */
#define RTL9607C_PORT_ISOLATION_REG(_port)	(0x27000 + (_port) * 4)
#define   RTL9607C_PORT_ISOLATION_MASK		RTL9607C_PORT_MASK

/* MSTP port state registers - indexed by tree instance */
#define RTL9607C_MSTI_CTRL_REG(_port)		(0x1704c + (_port) * 4)
#define   RTL9607C_MSTI_CTRL_PORT_STATE_OFFSET(_msti)	((_msti) << 1)
#define   RTL9607C_MSTI_CTRL_PORT_STATE_MASK(_msti) \
		(0x3 << RTL9607C_MSTI_CTRL_PORT_STATE_OFFSET(_msti))

/* Broadcast flooding port mask */
#define RTL9607C_UNKNOWN_BROADCAST_FLOOD_REG	  0x1c028

/* Unknown multicast DA flooding port mask */
#define RTL9607C_UNKNOWN_MULTICAST_FLOOD_REG	  0x1c02c

/* Unknown unicast DA flooding port mask */
#define RTL9607C_UNKNOWN_UNICAST_FLOOD_REG	0x1c030

/* Per port MIB statistics */
#define RTL9607C_STAT_PORT_MIB_BASE		(0x32000)
#define RTL9607C_STAT_PORT_MIB_REG(x)		(RTL9607C_STAT_PORT_MIB_BASE + (x) * 0x80)
/* Each define is an offset of RTL9607C_STAT_PORT_MIB */
#define   RTL9607C_IF_OUT_BROADCAST_PKTS		(0x64)
#define   RTL9607C_IF_OUT_MULTICAST_PKTS		(0x60)
#define   RTL9607C_IF_OUT_UCAST_PKTS			(0x5c)
#define   RTL9607C_DOT1D_TP_PORT_IN_DISCARDS		(0x58)
#define   RTL9607C_ETHER_STATS_TX_PKTS_1519_TO_MAX_OCTETS	(0x50)
#define   RTL9607C_IF_OUT_DISCARDS			(0x4c)
#define   RTL9607C_DOT3_OUT_PAUSE_FRAMES		(0x48)
#define   RTL9607C_DOT3_STATS_EXCESSIVE_COLLISIONS	(0x44)
#define   RTL9607C_ETHER_STATS_COLLISIONS		(0x40)
#define   RTL9607C_DOT3_STATS_LATE_COLLISIONS		(0x3c)
#define   RTL9607C_DOT3_STATS_DEFERRED_TRANSMISSIONS	(0x38)
#define   RTL9607C_DOT3_STATS_MULTIPLE_COLLISION_FRAMES	(0x34)
#define   RTL9607C_DOT3_STATS_SINGLE_COLLISION_FRAMES	(0x30)
#define   RTL9607C_IF_OUT_OCTETS			(0x28) /* 64 bits */
#define   RTL9607C_ETHER_STATS_TX_PKTS_1024_TO_1518_OCTETS	(0x24)
#define   RTL9607C_ETHER_STATS_TX_PKTS_512_TO_1023_OCTETS	(0x20)
#define   RTL9607C_ETHER_STATS_TX_PKTS_256_TO_511_OCTETS	(0x1c)
#define   RTL9607C_ETHER_STATS_TX_PKTS_128_TO_255_OCTETS	(0x18)
#define   RTL9607C_ETHER_STATS_TX_PKTS_65_TO_127_OCTETS		(0x14)
#define   RTL9607C_ETHER_STATS_TX_PKTS_64_OCTETS		(0x10)
#define   RTL9607C_ETHER_STATS_TX_OVERSIZE_PKTS		(0xc)
#define   RTL9607C_ETHER_STATS_TX_UNDERSIZE_PKTS	(0x8)
#define   RTL9607C_ETHER_STATS_TX_BROADCAST_PKTS	(0x4)
#define   RTL9607C_ETHER_STATS_TX_MULTICAST_PKTS	(0x0)
#define STAT_RX_OFFSET	(0x600)
#define   RTL9607C_ETHER_STATS_RX_PKTS_1024_TO_1518_OCTETS	(STAT_RX_OFFSET + 0x54)
#define   RTL9607C_ETHER_STATS_RX_PKTS_512_TO_1023_OCTETS	(STAT_RX_OFFSET + 0x50)
#define   RTL9607C_ETHER_STATS_RX_PKTS_256_TO_511_OCTETS	(STAT_RX_OFFSET + 0x4c)
#define   RTL9607C_ETHER_STATS_RX_PKTS_128_TO_255_OCTETS	(STAT_RX_OFFSET + 0x48)
#define   RTL9607C_ETHER_STATS_RX_PKTS_65_TO_127_OCTETS		(STAT_RX_OFFSET + 0x44)
#define   RTL9607C_ETHER_STATS_RX_PKTS_64_OCTETS		(STAT_RX_OFFSET + 0x40)
#define   RTL9607C_ETHER_STATS_RX_OVERSIZE_PKTS		(STAT_RX_OFFSET + 0x3c)
#define   RTL9607C_ETHER_STATS_RX_UNDERSIZE_PKTS	(STAT_RX_OFFSET + 0x38)
#define   RTL9607C_ETHER_STATS_RX_PKTS_1519_TO_MAX_OCTETS	(STAT_RX_OFFSET + 0x30)
#define   RTL9607C_IF_IN_BROADCAST_PKTS		(STAT_RX_OFFSET + 0x2c)
#define   RTL9607C_IF_IN_MULTICAST_PKTS		(STAT_RX_OFFSET + 0x28)
#define   RTL9607C_ETHER_STATS_DROP_EVENTS	(STAT_RX_OFFSET + 0x24)
#define   RTL9607C_IF_IN_UCAST_PKTS		(STAT_RX_OFFSET + 0x20)
#define   RTL9607C_ETHER_STATS_JABBERS		(STAT_RX_OFFSET + 0x1c)
#define   RTL9607C_ETHER_STATS_FRAGMENTS		(STAT_RX_OFFSET + 0x18)
#define   RTL9607C_DOT3_CONTROL_IN_UNKNOWN_OPCODES	(STAT_RX_OFFSET + 0x14)
#define   RTL960v_DOT3_IN_PAUSE_FRAMES		(STAT_RX_OFFSET + 0x10)
#define   RTL9607C_DOT3_STATS_SYMBOL_ERRORS	(STAT_RX_OFFSET + 0xc)
#define   RTL9607C_ETHER_STATS_CRC_ALIGN_ERRORS	(STAT_RX_OFFSET + 0x8)
#define   RTL9607C_IF_IN_OCTETS			(STAT_RX_OFFSET + 0x0) /* 64 bits */
#define RTL9607C_STAT_PORT_OAM_MIB(x)		(RTL9607C_STAT_PORT_MIB_BASE + (x) * 0x8)
#define STAT_OAM_OFFSET	(0xc00)
#define   RTL9607C_OUT_OAM_PDU_PKTS	(STAT_OAM_OFFSET + 0x0)
#define   RTL9607C_IN_OAM_PDU_PKTS	(STAT_OAM_OFFSET + 0x4)

#define RTL9607C_STAT_CTRL_REG			0x34000
#define RTL9607C_STAT_PORT_RESET_REG		0x34014
#define RTL9607C_STAT_RESET_REG			0x34018
#define   RTL9607C_STAT_RESET_STATUS_MASK	BIT(3)
#define RTL9607C_STAT_RESET_CONFIG_REG		0x3401c
#define   RTL9607C_STAT_RESET_CMD_MASK		BIT(5)

struct rtl9607c_mib_counter {
	u32 offset;
	u32 length;
	const char *name;
};

#define MIB_DESC(_o, _l, _n)	\
{			\
	.offset = (_o),	\
	.length = (_l),	\
	.name = (_n),	\
}

static const struct rtl9607c_mib_counter rtl9607c_mib_counters[] = {
	MIB_DESC(RTL9607C_ETHER_STATS_TX_MULTICAST_PKTS, 1, "etherStatsTxMulticastPkts"),
	MIB_DESC(RTL9607C_ETHER_STATS_TX_BROADCAST_PKTS, 1, "etherStatsTxBroadcastPkts"),
	MIB_DESC(RTL9607C_ETHER_STATS_TX_UNDERSIZE_PKTS, 1, "etherStatsTxUndersizePkts"),
	MIB_DESC(RTL9607C_ETHER_STATS_TX_OVERSIZE_PKTS, 1, "etherStatsTxOversizePkts"),
	MIB_DESC(RTL9607C_ETHER_STATS_COLLISIONS, 1, "etherStatsCollisions"),
	MIB_DESC(RTL9607C_IF_OUT_DISCARDS, 1, "ifOutDiscards"),
	MIB_DESC(RTL9607C_DOT1D_TP_PORT_IN_DISCARDS, 1, "dot1dTpPortInDiscards"),
	MIB_DESC(RTL9607C_ETHER_STATS_DROP_EVENTS, 1, "etherStatsDropEvents"),
	MIB_DESC(RTL9607C_ETHER_STATS_CRC_ALIGN_ERRORS, 1, "etherStatsCrcAlignErrors"),
};

/* Where all of the chip info struct will go */

/**
 * struct rtl9607c_chip_info - static chip-specific info
 * @name: human-readable chip name
 * @chip_id: chip identifier
 * @chip_ver: chip silicon revision
 * @port_mask: ports the chip has
 *
 * These data are specific to a given chip in the family of switches supported
 * by this driver. When adding support for another chip in the family, a new
 * chip info should be added to the rtl9607c_chip_infos array.
 */
struct rtl9607c_chip_info {
	const char *name;
	u32 chip_id;
	u32 chip_ver;
	u32 port_mask;
};

/* Every chip entry must set .port_mask through PORT_MASK(). PORT_MASK()
 * fails the build when a port does not exist in the family
 */
#define PORT_MASK(_m) \
	((_m) + \
	 BUILD_BUG_ON_ZERO((_m) & ~GENMASK(RTL9607C_MAX_NUM_PORTS - 1, 0)))

static const struct rtl9607c_chip_info rtl9607c_chip_infos[] = {
	{
		.name = "RTL9607C-VB",
		.chip_id = 0x6831,
		.chip_ver = 0x0040, /* TODO: FIND THE CORRECT VALUE */
		.port_mask = PORT_MASK(GENMASK(3, 0) | BIT(5) | BIT(6) | BIT(7) |
				       GENMASK(9, 10)),
	},
	{
		.name = "RTL9607C-VA",
		.chip_id = 0x6537,
		.chip_ver = 0x0040, /* TODO: FIND THE CORRECT VALUE */
		.port_mask = PORT_MASK(GENMASK(3, 0) | BIT(5) | BIT(6) | BIT(7) |
				       GENMASK(9, 10)),
	},
};

enum rtl9607c_stp_state {
	RTL9607C_STP_STATE_DISABLED = 0,
	RTL9607C_STP_STATE_BLOCKING = 1,
	RTL9607C_STP_STATE_LEARNING = 2,
	RTL9607C_STP_STATE_FORWARDING = 3,
};

/* Where the private per-port and driver data will go */

/**
 * struct rtl9607c - driver private data
 * @priv: pointer to parent realtek_priv data
 * @irq: registered IRQ or zero
 * @chip_info: chip-specific info about the attached switch
 *
 * Private data for this driver.
 */
struct rtl9607c {
	struct realtek_priv *priv;
	int irq;
	const struct rtl9607c_chip_info *chip_info;
	u16 page[RTL9607C_PHYADDRMAX];
};

static int rtl9607c_phy_poll_busy(struct realtek_priv *priv)
{
	u32 val;

	return regmap_read_poll_timeout(priv->map,
					RTL9607C_GPHY_INDIRECT_READ_DATA_REG,
					val, !(val & RTL9607C_GPHY_STATUS_BUSY),
					10, 1000);
}

static int rtl9607c_phy_ocp_run_cmd(struct realtek_priv *priv, int phy,
				    u32 ocp_addr, u32 cmd_type)
{
	u32 val;
	int ret;

	val = RTL9607C_INDIRECT_COMMAND_RUN;
	val |= FIELD_PREP(RTL9607C_INDIRECT_COMMAND_RW_MASK, cmd_type);
	val |= FIELD_PREP(RTL9607C_INDIRECT_COMMAND_PHYID_MASK, phy);
	val |= FIELD_PREP(RTL9607C_INDIRECT_COMMAND_OCP_ADDR_MASK, ocp_addr);
	ret = regmap_write(priv->map, RTL9607C_GPHY_INDIRECT_COMMAND_REG, val);
	if (ret)
		return ret;

	return rtl9607c_phy_poll_busy(priv);
}

static int rtl9607c_phy_ocp_read(struct realtek_priv *priv, int phy,
				 u32 ocp_addr, u16 *data)
{
	u32 val;
	int ret;

	mutex_lock(&priv->map_lock);

	ret = rtl9607c_phy_poll_busy(priv);
	if (ret)
		goto out;

	ret = rtl9607c_phy_ocp_run_cmd(priv, phy, ocp_addr,
				       RTL9607C_INDIRECT_COMMAND_RW_READ);
	if (ret)
		goto out;

	/* Get PHY register data */
	ret = regmap_read(priv->map, RTL9607C_GPHY_INDIRECT_READ_DATA_REG, &val);
	if (ret)
		goto out;

	*data = val & RTL9607C_GPHY_READ_DATA_MASK;

out:
	mutex_unlock(&priv->map_lock);

	return ret;
}

static int rtl9607c_phy_ocp_write(struct rtl9607c_dsa *priv, int phy,
				 u32 ocp_addr, u16 data)
{
	int ret;

	mutex_lock(&priv->map_lock);

	ret = rtl9607c_phy_poll_busy(priv);
	if (ret)
		goto out;

	/* Set PHY register data */
	ret = regmap_write(priv->map, RTL9607C_GPHY_INDIRECT_WRITE_DATA_REG, data);
	if (ret)
		goto out;

	ret = rtl9607c_phy_ocp_run_cmd(priv, phy, ocp_addr,
				       RTL9607C_INDIRECT_COMMAND_RW_WRITE);
	if (ret)
		goto out;

	ret = rtl9607c_phy_poll_busy(priv);
	if (ret)
		goto out;

out:
	mutex_unlock(&priv->map_lock);

	return ret;
}

static int rtl9607c_phy_read(struct realtek_priv *priv, int phy, int regnum)
{
	struct rtl9607c *mb = priv->chip_data;
	u32 ocp_addr;
	u16 val;
	int ret;

	if (phy > RTL9607C_PHYADDRMAX)
		return -EINVAL;

	if (regnum == RTL9607C_PAGE_SELECT)
		return mb->page[phy];
	else if (regnum > 15 && regnum < 24)
		ocp_addr = ((priv->page[phy] & 0xfff) << 4) + (regnum - 16) * 2;
	else
		ocp_addr = RTL9607C_PHY_OCP_ADDR_PHYREG_BASE + regnum * 2;

	ret = rtl9607c_phy_ocp_read(priv, phy, ocp_addr, &val);
	if (ret) {
		dev_err(priv->dev,
			"failed to read PHY%d reg %02x @ %04x, ret %pe\n", phy,
			regnum, ocp_addr, ERR_PTR(ret));
		return ret;
	}

	dev_dbg(priv->dev, "read PHY%d register 0x%02x @ %04x, val <- %04x\n",
		phy, regnum, ocp_addr, val);

	return val;
}

static int rtl9607c_phy_write(struct realtek_priv *priv, int phy, int regnum,
			      u16 val)
{
	struct rtl9607c *mb = priv->chip_data;
	u32 ocp_addr;
	int ret;

	if (phy > RTL9607C_PHYADDRMAX)
		return -EINVAL;

	if (regnum == RTL9607C_PAGE_SELECT) {
		if (val >= 0xffff)
			return -EINVAL;

		mb->page[phy] = val;
		return 0;
	} else if (regnum > 15 && regnum < 24) {
		ocp_addr = ((priv->page[phy] & 0xfff) << 4) + (regnum - 16) * 2;
	} else {
		ocp_addr = RTL9607C_PHY_OCP_ADDR_PHYREG_BASE + regnum * 2;
	}

	ret = rtl9607c_phy_ocp_write(priv, phy, ocp_addr, val);
	if (ret) {
		dev_err(priv->dev,
			"failed to write PHY%d reg %02x @ %04x, ret %pe\n", phy,
			regnum, ocp_addr, ERR_PTR(ret));
		return ret;
	}

	dev_dbg(priv->dev, "write PHY%d register 0x%02x @ %04x, val -> %04x\n",
		phy, regnum, ocp_addr, val);

	return 0;
}

static enum dsa_tag_protocol
rtl9607c_dsa_get_tag_protocol(struct dsa_switch *ds, int port,
			      enum dsa_tag_protocol mp)
{
	return DSA_TAG_PROTO_RTL8_4AP;
}

static void rtl9607c_dsa_phylink_get_caps(struct dsa_switch *ds, int port,
					 struct phylink_config *config)
{
	switch (port) {
	/* Ports connected to the integrated GPHYs */
	case 0 ... 3:
		__set_bit(PHY_INTERFACE_MODE_INTERNAL, config->supported_interfaces);
		/*
		 * GMII is the default interface mode for phylib, so we have
		 * to support it for ports with integrated PHY.
		 */
		__set_bit(PHY_INTERFACE_MODE_GMII, config->supported_interfaces);

		config->mac_capabilities = MAC_SYM_PAUSE | MAC_ASYM_PAUSE |
					   MAC_10 | MAC_100 | MAC_1000FD;
		break;
	/* Main CPU NIC ports */
	case 9:
	case 10:
		__set_bit(PHY_INTERFACE_MODE_INTERNAL, config->supported_interfaces);

		config->mac_capabilities = MAC_1000FD;
		break;
	/* PON and 2 HSGMII serdes are not supported yet */
	default:
		break;
	}
}

static void rtl9607c_dsa_mac_config(struct phylink_config *config,
				   unsigned int mode,
				   const struct phylink_link_state *state)
{
}

static void rtl9607c_dsa_mac_link_down(struct phylink_config *config,
				      unsigned int mode,
				      phy_interface_t interface)
{
}

static void rtl9607c_dsa_mac_link_up(struct phylink_config *config,
				    struct phy_device *phydev,
				    unsigned int mode,
				    phy_interface_t interface,
				    int speed, int duplex,
				    bool tx_pause, bool rx_pause)
{
}

static int rtl9607c_port_change_mtu(struct dsa_switch *ds, int port,
				    int new_mtu)
{
	struct realtek_priv *priv = ds->priv;
	int frame_size;

	frame_size = new_mtu + RTL9607C_FRAME_OVERHEAD;

	dev_dbg(priv->dev, "port %d changing mtu to %d (frame size: %d)\n",
		port, new_mtu, frame_size);

	return regmap_update_bits(priv->map, RTL9607C_ACCEPT_MAX_LEN(port),
				  RTL9607C_ACCEPT_MAX_LEN_MASK,
				  FIELD_PREP(RTL9607C_ACCEPT_MAX_LEN_MASK,
					     frame_size));
}

static int rtl9607c_port_max_mtu(struct dsa_switch *ds, int port)
{
	return RTL9607C_MAX_PACKET_LENGTH - RTL9607C_FRAME_OVERHEAD;
}

static void rtl9607c_port_stp_state_set(struct dsa_switch *ds, int port,
				       u8 state)
{
	struct realtek_priv *priv = ds->priv;
	enum rtl9607c_stp_state val;
	int msti = 0;

	switch (state) {
	case BR_STATE_DISABLED:
		val = RTL9607C_STP_STATE_DISABLED;
		break;
	case BR_STATE_BLOCKING:
	case BR_STATE_LISTENING:
		val = RTL9607C_STP_STATE_BLOCKING;
		break;
	case BR_STATE_LEARNING:
		val = RTL9607C_STP_STATE_LEARNING;
		break;
	case BR_STATE_FORWARDING:
		val = RTL9607C_STP_STATE_FORWARDING;
		break;
	default:
		dev_err(priv->dev, "invalid STP state: %u\n", state);
		return;
	}


	regmap_update_bits(priv->map, RTL9607C_MSTI_CTRL_REG(port),
				RTL9607C_MSTI_CTRL_PORT_STATE_MASK(msti),
				field_prep(RTL9607C_MSTI_CTRL_PORT_STATE_MASK(msti),
					   val));
}

/* Where VLAN functions should go in the future */

static int rtl9607c_port_set_ucast_flood(struct realtek_priv *priv, int port,
					 bool enable)
{
	/* Frames with unknown unicast DA will be flooded to a programmable
	 * port mask that by default includes all ports. Add or remove
	 * the specified port from this port mask accordingly.
	 */
	return regmap_assign_bits(priv->map, RTL9607C_UNKNOWN_UNICAST_FLOOD_REG,
				  BIT(port), enable);
}

static int rtl9607c_port_set_mcast_flood(struct realtek_priv *priv, int port,
					bool enable)
{
	return regmap_assign_bits(priv->map, RTL9607C_UNKNOWN_MULTICAST_FLOOD_REG,
				  BIT(port), enable);
}

static int rtl9607c_port_set_bcast_flood(struct realtek_priv *priv, int port,
					bool enable)
{
	return regmap_assign_bits(priv->map, RTL9607C_UNKNOWN_BROADCAST_FLOOD_REG,
				  BIT(port), enable);
}

/* Port isolation manipulation functions.
 *
 * The port isolation register controls the forwarding mask of a given
 * port. The switch will not forward packets ingressed on a given port
 * to ports which are not enabled in its forwarding mask.
 *
 * The port forwarding mask has the highest priority in forwarding
 * decisions. The only exception to this rule is when the switch
 * receives a packet on its CPU port with ALLOW=0. In that case the TX
 * field of the CPU tag will override the forwarding port mask.
 */
static int rtl9607c_port_set_isolation(struct realtek_priv *priv, int port,
				       u32 mask)
{
	return regmap_write(priv->map, RTL9607C_PORT_ISOLATION_REG(port), mask);
}

static int rtl9607c_port_add_isolation(struct realtek_priv *priv, int port,
				       u32 mask)
{
	return regmap_set_bits(priv->map, RTL9607C_PORT_ISOLATION_REG(port),
			       mask);
}

static int rtl9607c_port_remove_isolation(struct realtek_priv *priv, int port,
					  u32 mask)
{
	return regmap_clear_bits(priv->map, RTL9607C_PORT_ISOLATION_REG(port),
				 mask);
}

static int rtl9607c_port_mib_reset(struct realtek_priv *priv, u32 pmask)
{
	u32 val;
	int ret;

	ret = regmap_set_bits(priv->map, RTL9607C_STAT_PORT_RESET_REG, pmask);
	if (ret)
		return ret;

	ret = regmap_set_bits(priv->map, RTL9607C_STAT_RESET_CONFIG_REG,
			      RTL9607C_STAT_RESET_CMD_MASK);
	if (ret)
		return ret;

	return regmap_read_poll_timeout(priv->map, RTL9607C_STAT_RESET_REG, val,
					!FIELD_GET(RTL9607C_STAT_RESET_STATUS_MASK, val),
					10, 1000);
}

static void rtl9607c_mib_counter_read(struct rtl9607c_dsa *priv, int port, u32 offset,
				     u32 length, u64 *mibvalue)
{
	u32 low, high, high1, reg = RTL9607C_STAT_PORT_MIB_REG(port) + offset;

	if (length == 2) {
		regmap_read(priv->map, reg + 4, &high);
		regmap_read(priv->map, reg, &low);
		*mibvalue = low;
		regmap_read(priv->map, reg + 4, &high1);
		if (high != high1) {
			regmap_read(priv->map, reg, &low);
			*mibvalue = low;
		}
		*mibvalue |= (u64)high1 << 32;
	} else {
		regmap_read(priv->map, reg, &low);
		*mibvalue = low;
	}
}

static void rtl9607c_get_ethtool_stats(struct dsa_switch *ds, int port, u64 *data)
{
	struct realtek_priv *priv = ds->priv;
	const struct rtl9607c_mib_counter *mib;
	int i;

	for (i = 0; i < ARRAY_SIZE(rtl9607c_mib_counters); i++) {
		mib = &rtl9607c_mib_counters[i];

		rtl9607c_mib_counter_read(priv, port, mib->offset, mib->length, data + i);
	}
}

static void rtl9607c_get_strings(struct dsa_switch *ds, int port, u32 stringset, u8 *data)
{
	if (stringset != ETH_SS_STATS)
		return;

	for (int i = 0; i < ARRAY_SIZE(rtl9607c_mib_counters); i++)
		ethtool_puts(&data, rtl9607c_mib_counters[i].name);
}

static int rtl9607c_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	if (sset != ETH_SS_STATS)
		return -EOPNOTSUPP;

	return ARRAY_SIZE(rtl9607c_mib_counters);
}

static void rtl9607c_get_phy_stats(struct dsa_switch *ds, int port,
				  struct ethtool_eth_phy_stats *phy_stats)
{
	struct realtek_priv *priv = ds->priv;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_STATS_SYMBOL_ERRORS, 1,
				 &phy_stats->SymbolErrorDuringCarrier);
}

static void rtl9607c_get_mac_stats(struct dsa_switch *ds, int port,
				  struct ethtool_eth_mac_stats *mac_stats)
{
	struct realtek_priv *priv = ds->priv;
	u64 data;

	/* The RTL9607C exposes MIB objects, which we have to translate into
	 * IEEE 802.3 Managed Objects. This is not always completely faithful,
	 * but we try out best. See RFC 3635 for a detailed treatment of the
	 * subject.
	 */

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_UCAST_PKTS, 1,
				 &mac_stats->FramesTransmittedOK);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_STATS_SINGLE_COLLISION_FRAMES, 1,
				 &mac_stats->SingleCollisionFrames);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_STATS_MULTIPLE_COLLISION_FRAMES, 1,
				 &mac_stats->MultipleCollisionFrames);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_IN_UCAST_PKTS, 1,
				 &mac_stats->FramesReceivedOK);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_OCTETS, 2,
				 &mac_stats->OctetsTransmittedOK);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_STATS_DEFERRED_TRANSMISSIONS, 1,
				 &mac_stats->FramesWithDeferredXmissions);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_STATS_LATE_COLLISIONS, 1,
				 &mac_stats->LateCollisions);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_STATS_EXCESSIVE_COLLISIONS, 1,
				 &mac_stats->FramesAbortedDueToXSColls);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_IN_OCTETS, 2,
				 &mac_stats->OctetsReceivedOK);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_MULTICAST_PKTS, 1,
				 &mac_stats->MulticastFramesXmittedOK);
	mac_stats->FramesTransmittedOK += mac_stats->MulticastFramesXmittedOK;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_BROADCAST_PKTS, 1,
				 &mac_stats->BroadcastFramesXmittedOK);
	mac_stats->FramesTransmittedOK += mac_stats->BroadcastFramesXmittedOK;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_OUT_PAUSE_FRAMES, 1,
				 &data);
	mac_stats->FramesTransmittedOK += data;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_DISCARDS, 1, &data);
	mac_stats->FramesTransmittedOK -= data;

	mac_stats->OctetsTransmittedOK -= 18 * mac_stats->FramesTransmittedOK;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_IN_MULTICAST_PKTS, 1,
				 &mac_stats->MulticastFramesReceivedOK);
	mac_stats->FramesReceivedOK += mac_stats->MulticastFramesReceivedOK;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_IN_BROADCAST_PKTS, 1,
				 &mac_stats->BroadcastFramesReceivedOK);
	mac_stats->FramesReceivedOK += mac_stats->BroadcastFramesReceivedOK;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_IN_PAUSE_FRAMES, 1,
				 &data);
	mac_stats->FramesReceivedOK += data;

	mac_stats->OctetsReceivedOK -= 18 * mac_stats->FramesReceivedOK;
}

static void rtl9607c_get_ctrl_stats(struct dsa_switch *ds, int port,
				   struct ethtool_eth_ctrl_stats *ctrl_stats)
{
	struct realtek_priv *priv = ds->priv;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_CONTROL_IN_UNKNOWN_OPCODES, 1,
				 &ctrl_stats->UnsupportedOpcodesReceived);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_OUT_PAUSE_FRAMES, 1,
				 &ctrl_stats->MACControlFramesTransmitted);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_IN_PAUSE_FRAMES, 1,
				 &ctrl_stats->MACControlFramesReceived);
}

static const struct ethtool_rmon_hist_range rtl9607c_rmon_ranges[] = {
	{ 0, 64 },
	{ 65, 127 },
	{ 128, 255 },
	{ 256, 511 },
	{ 512, 1023 },
	{ 1024, 1518 },
	{ 1519, RTL9607C_MAX_PACKET_LENGTH },
	{ }
};

static void rtl9607c_get_rmon_stats(struct dsa_switch *ds, int port,
				   struct ethtool_rmon_stats *rmon_stats,
				   const struct ethtool_rmon_hist_range **ranges)
{
	struct realtek_priv *priv = ds->priv;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_RX_UNDERSIZE_PKTS, 1,
				 &rmon_stats->undersize_pkts);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_RX_OVERSIZE_PKTS, 1,
				 &rmon_stats->oversize_pkts);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_FRAGMENTS, 1,
				 &rmon_stats->fragments);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_JABBERS, 1,
				 &rmon_stats->jabbers);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_RX_PKTS_64_OCTETS, 1,
				 &rmon_stats->hist[0]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_RX_PKTS_65_TO_127_OCTETS, 1,
				 &rmon_stats->hist[1]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_RX_PKTS_128_TO_255_OCTETS, 1,
				 &rmon_stats->hist[2]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_RX_PKTS_256_TO_511_OCTETS, 1,
				 &rmon_stats->hist[3]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_RX_PKTS_512_TO_1023_OCTETS, 1,
				 &rmon_stats->hist[4]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_RX_PKTS_1024_TO_1518_OCTETS, 1,
				 &rmon_stats->hist[5]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_RX_PKTS_1519_TO_MAX_OCTETS, 1,
				 &rmon_stats->hist[6]);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_TX_PKTS_64_OCTETS, 1,
				 &rmon_stats->hist_tx[0]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_TX_PKTS_65_TO_127_OCTETS, 1,
				 &rmon_stats->hist_tx[1]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_TX_PKTS_128_TO_255_OCTETS, 1,
				 &rmon_stats->hist_tx[2]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_TX_PKTS_256_TO_511_OCTETS, 1,
				 &rmon_stats->hist_tx[3]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_TX_PKTS_512_TO_1023_OCTETS, 1,
				 &rmon_stats->hist_tx[4]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_TX_PKTS_1024_TO_1518_OCTETS, 1,
				 &rmon_stats->hist_tx[5]);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_TX_PKTS_1519_TO_MAX_OCTETS, 1,
				 &rmon_stats->hist_tx[6]);

	*ranges = rtl9607c_rmon_ranges;
}

static void rtl9607c_get_pause_stats(struct dsa_switch *ds, int port,
				    struct ethtool_pause_stats *pause_stats)
{
	struct realtek_priv *priv = ds->priv;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_OUT_PAUSE_FRAMES, 1,
				 &pause_stats->tx_pause_frames);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_IN_PAUSE_FRAMES, 1,
				 &pause_stats->rx_pause_frames);
}

static void rtl9607c_get_stats64(struct dsa_switch *ds, int port,
				struct rtnl_link_stats64 *s)
{

	struct realtek_priv *priv = ds->priv;
	u64 data;

	/* MIB counter doesn't provide a FramesTransmittedOK but instead
	 * provide stats for Unicast, Broadcast and Multicast frames separately.
	 * To simulate a global frame counter, read Unicast and addition Multicast
	 * and Broadcast later
	 */

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_IN_UCAST_PKTS, 1,
				 &s->rx_packets);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_IN_MULTICAST_PKTS, 1,
				 &s->multicast);
	s->rx_packets += s->multicast;
	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_IN_BROADCAST_PKTS, 1,
				 &data);
	s->rx_packets += data;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_UCAST_PKTS, 1,
				 &s->tx_packets);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_MULTICAST_PKTS, 1,
				 &data);
	s->tx_packets += data;
	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_BROADCAST_PKTS, 1,
				 &data);
	s->tx_packets += data;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_IN_OCTETS, 2,
				 &s->rx_bytes);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_OCTETS, 2,
				 &s->tx_bytes);

	/* if{In,Out}Octets includes FCS - remove it */
	s->rx_bytes -= 4 * s->rx_packets;
	s->tx_bytes -= 4 * s->tx_packets;
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_DROP_EVENTS, 1,
				 &s->rx_dropped);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_IF_OUT_DISCARDS, 1,
				 &s->tx_dropped);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_COLLISIONS, 1,
				 &s->collisions);

	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_FRAGMENTS, 1,
				 &s->rx_length_errors);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_ETHER_STATS_JABBERS, 1,
				 &data);
	s->rx_length_errors += data;

	s->rx_errors = s->rx_length_errors;

	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_STATS_EXCESSIVE_COLLISIONS, 1,
				 &s->tx_aborted_errors);
	rtl9607c_mib_counter_read(priv, port, RTL9607C_DOT3_STATS_LATE_COLLISIONS, 1,
				 &s->tx_window_errors);

	s->tx_errors = s->tx_aborted_errors + s->tx_window_errors;
}

static int rtl9607c_get_and_clear_status_reg(struct realtek_priv *priv, u32 reg,
					    u32 *val)
{
	int ret;

	ret = regmap_read(priv->map, reg, val);
	if (ret)
		return ret;

	return regmap_write(priv->map, reg, *val);
}

static irqreturn_t rtl9607c_irq(int irq, void *data)
{
	struct realtek_priv *priv = data;
	unsigned long line_changes = 0;
	u32 stat;
	int line;
	int ret;

	ret = rtl9607c_get_and_clear_status_reg(priv, RTL9607C_INTR_IMS_REG,
					       &stat);
	if (ret)
		goto out_error;

	if (stat & (RTL9607C_INTR_LINK_CHANGE_MASK | RTL9607C_INTR_SPEED_CHANGE_MASK)) {
		u32 linkdown_ind, linkup_ind, speed_ind, val;

		ret = rtl9607c_get_and_clear_status_reg(
			priv, RTL9607C_INTR_STATUS_REG, &val);
		if (ret)
			goto out_error;

		linkdown_ind = FIELD_GET(RTL9607C_INTR_STAT_PORT_LINKDOWN_MASK, val);

		ret = rtl9607c_get_and_clear_status_reg(
			priv, RTL9607C_INTR_STATUS1_REG, &val);
		if (ret)
			goto out_error;

		linkup_ind = FIELD_GET(RTL9607C_INTR_STAT_PORT_LINKUP_MASK, val);

		speed_ind = FIELD_GET(RTL9607C_INTR_STAT_PORT_SPEED_CHANGE_MASK, val);

		line_changes = linkdown_ind | linkup_ind | speed_ind;
	}

	if (!line_changes)
		goto out_none;

	for_each_set_bit(line, &line_changes, RTL9607C_NUM_PORTS) {
		int child_irq = irq_find_mapping(priv->irqdomain, line);

		if (!child_irq)
			continue;

		handle_nested_irq(child_irq);
	}

	return IRQ_HANDLED;

out_error:
	dev_err(priv->dev, "failed to read interrupt status: %pe\n",
		ERR_PTR(ret));

out_none:
	return IRQ_NONE;
}

static struct irq_chip rtl9607c_irq_chip = {
	.name = "rtl9607c-switch",
	/* The hardware doesn't support masking IRQs on a per-port basis */
};

static int rtl9607c_irq_map(struct irq_domain *domain, unsigned int irq,
			   irq_hw_number_t hwirq)
{
	struct realtek_priv *priv = domain->host_data;

	irq_set_chip_data(irq, priv);
	irq_set_chip_and_handler(irq, &rtl9607c_irq_chip, handle_simple_irq);
	irq_set_nested_thread(irq, 1);
	irq_set_noprobe(irq);
	irq_set_parent(irq, priv->irq);

	return 0;
}

static void rtl9607c_irq_unmap(struct irq_domain *d, unsigned int irq)
{
	irq_set_nested_thread(irq, 0);
	irq_set_chip_and_handler(irq, NULL, NULL);
	irq_set_chip_data(irq, NULL);
}

static const struct irq_domain_ops rtl9607c_irqdomain_ops = {
	.map = rtl9607c_irq_map,
	.unmap = rtl9607c_irq_unmap,
	.xlate = irq_domain_xlate_onecell,
};

static int rtl9607c_irq_enable(struct realtek_priv *priv)
{
	return regmap_write(priv->map, RTL9607C_INTR_IMR_REG,
			    RTL9607C_INTR_LINK_CHANGE_MASK |
			    RTL9607C_INTR_SPEED_CHANGE_MASK);
}

static int rtl9607c_irq_disable(struct realtek_priv *priv)
{
	return regmap_write(priv->map, RTL9607C_INTR_IMR_REG, 0);
}

static void rtl9607c_irq_teardown(struct realtek_priv *priv)
{
	struct dsa_switch *ds = priv->ds;
	struct dsa_port *dp;
	int virq;

	if (priv->irq) {
		free_irq(priv->irq, priv);
		priv->irq = 0;
	}

	if (priv->irqdomain) {
		/* Unused ports with a linked PHY still have an active IRQ
		 * mapping that must be disposed of during teardown. Loop
		 * through all ports.
		 */
		dsa_switch_for_each_port(dp, ds) {
			virq = irq_find_mapping(priv->irqdomain, dp->index);

			if (virq)
				irq_dispose_mapping(virq);
		}

		irq_domain_remove(priv->irqdomain);
		priv->irqdomain = NULL;
	}
}

static int rtl9607c_irq_setup(struct realtek_priv *priv)
{
	struct dsa_switch *ds = priv->ds;
	struct device_node *intc;
	struct dsa_port *dp;
	u32 irq_trig;
	int virq;
	int irq;
	u32 val;
	int ret;

	intc = of_get_child_by_name(priv->dev->of_node, "interrupt-controller");
	if (!intc) {
		dev_err(priv->dev, "missing child interrupt-controller node\n");
		return -EINVAL;
	}

	/* RTL9607C IRQs cascade off this one */
	irq = of_irq_get(intc, 0);
	if (irq <= 0) {
		if (!irq) {
			dev_err(priv->dev, "failed to map IRQ\n");
			ret = -EINVAL;
		} else {
			ret = dev_err_probe(priv->dev, irq,
					    "failed to get parent irq\n");
		}
		goto out_put_node;
	}

	/* Store the irq so that we know to map and free it during teardown */
	priv->irq = irq;

	priv->irqdomain = irq_domain_create_linear(of_fwnode_handle(intc), RTL9607C_NUM_PORTS,
						   &rtl9607c_irqdomain_ops, priv);
	if (!priv->irqdomain) {
		dev_err(priv->dev, "failed to add irq domain\n");
		ret = -ENOMEM;
		goto out_put_node;
	}

	dsa_switch_for_each_available_port(dp, ds) {
		virq = irq_create_mapping(priv->irqdomain, dp->index);
		if (!virq) {
			dev_err(priv->dev,
				"failed to create irq domain mapping\n");
			ret = -EINVAL;
			goto out_teardown_irq;
		}

		irq_set_parent(virq, irq);
	}

	/* Configure chip interrupt signal polarity */
	irq_trig = irq_get_trigger_type(irq);
	switch (irq_trig) {
	case IRQF_TRIGGER_RISING:
	case IRQF_TRIGGER_HIGH:
		val = RTL9607C_INTR_POLARITY_HIGH;
		break;
	case IRQF_TRIGGER_FALLING:
	case IRQF_TRIGGER_LOW:
		val = RTL9607C_INTR_POLARITY_LOW;
		break;
	default:
		dev_err(priv->dev, "unsupported irq trigger type %u\n",
			irq_trig);
		ret = -EINVAL;
		goto out_teardown_irq;
	}

	ret = regmap_update_bits(priv->map, RTL9607C_INTR_POLARITY_REG,
				 RTL9607C_INTR_POLARITY_MASK, val);
	if (ret)
		goto out_teardown_irq;

	/* Disable the interrupt in case the chip has it enabled on reset */
	ret = rtl9607c_irq_disable(priv);
	if (ret)
		goto out_teardown_irq;

	/* Clear the interrupt status register */
	ret = regmap_write(priv->map, RTL9607C_INTR_IMS_REG,
			   RTL9607C_INTR_ALL_MASK);
	if (ret)
		goto out_teardown_irq;

	ret = regmap_write(priv->map, RTL9607C_INTR_STATUS_REG,
			   RTL9607C_INTR_STAT_PORT_LINKDOWN_MASK |
			   RTL9607C_INTR_STAT_GPHY_MASK);
	if (ret)
		goto out_teardown_irq;

	ret = regmap_write(priv->map, RTL9607C_INTR_STATUS1_REG,
			   RTL9607C_INTR_STAT_PORT_LINKUP_MASK |
			   RTL9607C_INTR_STAT_PORT_SPEED_CHANGE_MASK);
	if (ret)
		goto out_teardown_irq;

	ret = request_threaded_irq(irq, NULL, rtl9607c_irq, IRQF_ONESHOT,
				   "rtl9607c-switch", priv);
	if (ret) {
		dev_err(priv->dev, "failed to request irq: %pe\n",
			ERR_PTR(ret));
		goto out_teardown_irq;
	}

	ret = rtl9607c_irq_enable(priv);
	if (ret)
		goto out_teardown_irq;

	of_node_put(intc);

	return 0;

out_teardown_irq:
	rtl9607c_irq_teardown(priv);

out_put_node:
	priv->irq = 0;
	of_node_put(intc);

	return ret;
}

static int rtl9607c_cpu_port_setup(struct realtek_priv *priv, int port)
{
	u32 gmac_id;
	int ret;

	switch (port) {
	case 9:
		gmac_id = 0;
		break;
	case 10:
		gmac_id = 1;
		break;
	case 7:
		gmac_id = 2;
		break;
	default:
		return -EINVAL;
	}

	/* Force CPU port to link up and 1000/full */
	ret = regmap_write(priv->map, RTL9607C_FORCE_PORT_ABILITY_REG(port),
			   RTL9607C_FORCE_PORT_ABILITY_LINK |
			   RTL9607C_FORCE_PORT_ABILITY_DUPLEX |
			   FIELD_PREP(RTL9607C_FORCE_PORT_ABILITY_SPEED_MASK,
				      RTL9607C_FORCE_PORT_ABILITY_SPEED_1000));
	if (ret)
		return ret;

	ret = regmap_write(priv->map, RTL9607C_ABILITY_FORCE_MODE_REG(port),
			   0xfff);
	if (ret)
		return ret;

	ret = regmap_set_bits(priv->map, RTL9607C_CPU_TAG_INSERT_CTRL, BIT(gmac_id));
	if (ret)
		return ret;

	return regmap_set_bits(priv->map, RTL9607C_CPU_TAG_AWARE_CTRL, BIT(gmac_id));
}

/* Where the potential chip reset function will go */

static int rtl9607c_dsa_setup(struct dsa_switch *ds)
{
	struct realtek_priv *priv = ds->priv;
	u32 cpu_ports = 0, user_ports = 0;
	struct rtl9607c *mb;
	struct dsa_port *dp;
	int ret;

	mb = priv->chip_data;

	/* Set up cascading IRQs */
	ret = rtl9607c_irq_setup(priv);
	if (ret == -EPROBE_DEFER)
		return ret;
	else if (ret)
		dev_info(priv->dev, "no interrupt support\n");

	dsa_switch_for_each_available_port(dp, ds) {
		if (!(mb->chip_info->port_mask & BIT(dp->index))) {
			dev_err(priv->dev, "port %d does not exist on %s\n",
				dp->index, mb->chip_info->name);
			ret = -EINVAL;
			goto out_teardown_irq;
		}
	}

	/* Where QoS init will go in the future */

	/* Start with all ports blocked, including unused ports */
	dsa_switch_for_each_port(dp, ds) {
		/* Set the initial STP state of all ports to DISABLED, otherwise
		 * ports will still forward frames to the CPU despite being
		 * administratively down by default.
		 */
		rtl9607c_port_stp_state_set(ds, dp->index, BR_STATE_DISABLED);

		/* Start with all port completely isolated */
		ret = rtl9607c_port_set_isolation(priv, dp->index, 0);
		if (ret)
			goto out_teardown_irq;

		/* Enable all types of flooding */
		ret = rtl83xx_setup_port_flood_control(priv, dp->index);
		if (ret)
			goto out_teardown_irq;

		if (dsa_port_is_cpu(dp))
			cpu_ports |= BIT(dp->index);
	}

	/* Configure user ports */
	dsa_switch_for_each_user_port(dp, ds) {
		/* Forward only to the CPU */
		ret = rtl9607c_port_set_isolation(priv, dp->index, cpu_ports);
		if (ret)
			goto out_teardown_irq;

		user_ports |= BIT(dp->index);
	}

	if (cpu_ports == 0) {
		dev_err(priv->dev, "no CPU port defined\n");
		ret = -EINVAL;
		goto out_teardown_irq;
	}

	/* Configure CPU ports and their tagging */
	dsa_switch_for_each_cpu_port(dp, ds) {
		ret = rtl9607c_cpu_port_setup(priv, dp->index);
		if (ret)
			goto out_teardown_irq;

		ret = rtl9607c_port_set_isolation(priv, dp->index, user_ports);
		if (ret)
			goto out_teardown_irq;
	}

	ds->assisted_learning_on_cpu_port = true;

	/* Where VLAN setup will go in the future */

	/*
	 * PHY_PATCH_DONE enables PHY control via SoC. This is required for PHY
	 * access, including patching and must be set before the PHYs are probed.
	 */
	ret = regmap_set_bits(priv->map, RTL9607C_WRAP_GPHY_MISC_REG,
			      RTL9607C_PHY_PATCH_DONE_MASK);
	if (ret)
		goto out_teardown_irq;

	ret = rtl83xx_setup_user_mdio(ds);
	if (ret) {
		dev_err(priv->dev, "could not set up MDIO bus\n");
		goto out_teardown_irq;
	}

	ret = rtl9607c_port_mib_reset(priv, user_ports | cpu_ports);
	if (ret)
		goto out_teardown_irq;

	return 0;

out_teardown_irq:
	rtl9607c_irq_teardown(priv);

	return ret;
}

static void rtl9607c_teardown(struct dsa_switch *ds)
{
	struct realtek_priv *priv = ds->priv;

	rtl9607c_irq_teardown(priv);
}

static int rtl9607c_get_chip_id_and_ver(struct regmap *map, u32 *id, u32 *ver)
{
	u32 chip_info;
	int ret;

	/* The CHIP_INFO_EN must be set to 0xa to enable chip information */
	ret = regmap_set_bits(map, RTL9607C_CHIP_INFO_REG,
			      FIELD_PREP(RTL9607C_CHIP_INFO_EN_MASK,
					 RTL9607C_CHIP_INFO_EN_VALUE));
	if (ret)
		return ret;

	ret = regmap_read(map, RTL9607C_CHIP_INFO_REG, chip_info);
	if (ret)
		return ret;

	*id = FIELD_GET(RTL9607C_CHIP_INFO_ID_MASK, chip_info);
	*ver = FIELD_GET(RTL9607C_CHIP_INFO_VER_MASK, chip_info);

	/* Reset CHIP_INFO_EN field */
	return regmap_clear_bits(map, RTL9607C_CHIP_INFO_REG,
				 RTL9607C_CHIP_INFO_EN_MASK);
}

static int rtl9607c_detect(struct realtek_priv *priv)
{
	struct rtl9607c *mb = priv->chip_data;
	u32 chip_id;
	u32 chip_ver;
	int ret;
	int i;

	ret = rtl9607c_get_chip_id_and_ver(priv->map, &chip_id, &chip_ver);
	if (ret) {
		dev_err(priv->dev, "failed to read chip id and version: %pe\n",
			ERR_PTR(ret));
		return ret;
	}

	for (i = 0; i < ARRAY_SIZE(rtl9607c_chip_infos); i++) {
		const struct rtl9607c_chip_info *ci = &rtl9607c_chip_infos[i];

		if (ci->chip_id == chip_id && ci->chip_ver == chip_ver) {
			mb->chip_info = ci;
			break;
		}
	}

	if (!mb->chip_info) {
		dev_err(priv->dev,
			"unrecognized switch (id=0x%04x, ver=0x%04x)", chip_id,
			chip_ver);
		return -ENODEV;
	}

	dev_info(priv->dev, "found an %s switch\n", mb->chip_info->name);

	/* port_mask can have gaps, so count up to the highest port, not the
	 * ports set
	 */
	priv->num_ports = fls(mb->chip_info->port_mask);
	mb->priv = priv;

	return 0;
}

static const struct phylink_mac_ops rtl9607c_phylink_mac_ops = {
	.mac_config	= rtl9607c_dsa_mac_config,
	.mac_link_down	= rtl9607c_dsa_mac_link_down,
	.mac_link_up	= rtl9607c_dsa_mac_link_up,
};

static const struct dsa_switch_ops rtl9607c_dsa_ops = {
	.get_tag_protocol = rtl9607c_dsa_get_tag_protocol,
	.setup = rtl9607c_dsa_setup,
	.teardown = rtl9607c_teardown,
	.phylink_get_caps = rtl9607c_dsa_phylink_get_caps,
	.port_stp_state_set = rtl9607c_port_stp_state_set,
	.get_strings = rtl9607c_get_strings,
	.get_ethtool_stats = rtl9607c_get_ethtool_stats,
	.get_sset_count = rtl9607c_get_sset_count,
	.get_eth_phy_stats = rtl9607c_get_phy_stats,
	.get_eth_mac_stats = rtl9607c_get_mac_stats,
	.get_eth_ctrl_stats  rtl9607c_get_ctrl_stats,
	.get_rmon_stats = rtl9607c_get_rmon_stats,
	.get_stats64 = rtl9607c_get_stats64,
	.get_pause_stats = rtl9607c_get_pause_stats,
	.port_change_mtu = rtl9607c_port_change_mtu,
	.port_max_mtu = rtl9607c_port_max_mtu,
};

static const struct realtek_ops rtl9607c_ops = {
	.detect = rtl9607c_detect,
	.port_add_isolation = rtl9607c_port_add_isolation,
	.port_remove_isolation = rtl9607c_port_remove_isolation,
	.port_set_ucast_flood = rtl9607c_port_set_ucast_flood,
	.port_set_mcast_flood = rtl9607c_port_set_mcast_flood,
	.port_set_bcast_flood = rtl9607c_port_set_bcast_flood,
	.phy_read = rtl9607c_phy_read,
	.phy_write = rtl9607c_phy_write,
};

const struct realtek_variant rtl9607c_variant = {
	.ds_ops = &rtl9607c_switch_ops,
	.ops = &rtl9607c_ops,
	.phylink_mac_ops = &rtl9607c_phylink_mac_ops,
	.chip_data_sz = sizeof(struct rtl9607c),
};

static const struct of_device_id rtl9607c_of_match[] = {
	{ .compatible = "realtek,rtl9607c-switch", .data = &rtl9607c_variant, },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, rtl9607c_of_match);

static struct platform_driver rtl9607c_mmio_driver = {
	.driver = {
		.name = "rtl9607c-mmio",
		.of_match_table = rtl9607c_of_match,
	},
	.probe  = realtek_mmio_probe,
	.remove = realtek_mmio_remove,
	.shutdown = realtek_mmio_shutdown,
};

module_platform_driver(rtl9607c_mmio_driver);

MODULE_AUTHOR("Rustam Adilov <adilov@disroot.org>");
MODULE_DESCRIPTION("Driver for RTL9607C ethernet switch");
MODULE_LICENSE("GPL");
MODULE_IMPORT_NS("REALTEK_DSA");
