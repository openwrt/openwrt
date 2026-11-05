// SPDX-License-Identifier: GPL-2.0-only
/*
 * DSA switch driver for the Realtek RTL9607C family switch core.
 *
 * The RTL9607C family of SoCs has an embedded 11 port switch controller.
 * rtl960x_dsa_phylink_get_caps() describes the ports this driver supports.
 */

#include <linux/bitfield.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/if_bridge.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of.h>
#include <linux/phy.h>
#include <linux/phylink.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/soc/realtek/otto_table.h>
#include <net/dsa.h>

#include "rtl960x_dsa.h"
#include "rtl960x_l2.h"
#include "rtl960x_vlan.h"

/* Forced MAC ability of a port, used to force the CPU ports' link up */
#define RTL960X_FORCE_P_ABLTY_REG(_port)	(0x1cc + (_port) * 4)
#define   RTL960X_FORCE_P_ABLTY_NWAY		BIT(7)
#define   RTL960X_FORCE_P_ABLTY_LINK		BIT(4)
#define   RTL960X_FORCE_P_ABLTY_DUPLEX		BIT(2)
#define   RTL960X_FORCE_P_ABLTY_SPEED_MASK	GENMASK(1, 0)
#define   RTL960X_FORCE_P_ABLTY_SPEED_1000	2
#define RTL960X_ABLTY_FORCE_MODE_REG(_port)	(0x238 + (_port) * 4)

/*
 * Preferred CPU port for the user ports when the device tree describes more
 * than one. Port 7 can also be a CPU port (GMAC2) but is muxed with the
 * second HSGMII SerDes, so keep GMAC0 on port 9 as the default.
 */
#define RTL960X_DEFAULT_CPU_PORT		9

#define RTL960X_PORT_MASK	GENMASK(10, 0)
/*
 * Port isolation registers - one egress portmask per port. Bits 28:11 are
 * the extension ports of MAC7/9/10, which the driver leaves clear.
 */
#define RTL960X_PORT_ISOLATION_REG(_port)	(0x27000 + (_port) * 4)
#define   RTL960X_PORT_ISOLATION_MASK		RTL960X_PORT_MASK

/* MSTP port state registers - one per port, a 2-bit state per MSTI */
#define RTL960X_MSTI_CTRL_REG(_port)		(0x1704c + (_port) * 4)
#define   RTL960X_MSTI_CTRL_PORT_STATE_OFFSET(_msti)	((_msti) << 1)
#define   RTL960X_MSTI_CTRL_PORT_STATE_MASK(_msti) \
		(0x3 << RTL960X_MSTI_CTRL_PORT_STATE_OFFSET(_msti))

/*
 * Per-port L2 learning limit, two 12-bit counts per register. A limit of 0
 * disables learning; the register maximum 0xfff disables the limit.
 */
#define RTL960X_LUT_LRN_LIMITNO_REG(_port)	(0x17008 + ((_port) / 2) * 4)
#define   RTL960X_LUT_LRN_LIMITNO_SHIFT(_port)	(((_port) % 2) * 12)
#define   RTL960X_LUT_LRN_LIMITNO_MASK(_port)	(GENMASK(11, 0) << \
					 RTL960X_LUT_LRN_LIMITNO_SHIFT(_port))
/* What to do with a frame once the port's limit is reached, 2 bits per port */
#define RTL960X_LUT_LEARN_OVER_CTRL_REG		0x1c010
#define   RTL960X_LUT_LEARN_OVER_ACT_MASK(_port)	(0x3 << ((_port) * 2))
#define   RTL960X_LUT_LEARN_OVER_ACT_FORWARD	0	/* forward, do not learn */

/* Per port MIB statistics */
#define RTL960X_STAT_PORT_MIB_BASE		(0x32000)
#define RTL960X_STAT_PORT_MIB(x)		(RTL960X_STAT_PORT_MIB_BASE + (x) * 0x80)
/* Each define is an offset of RTL960X_STAT_PORT_MIB */
#define   RTL960X_IF_OUT_BROADCAST_PKTS		(0x64)
#define   RTL960X_IF_OUT_MULTICAST_PKTS		(0x60)
#define   RTL960X_IF_OUT_UCAST_PKTS		(0x5c)
#define   RTL960X_DOT1D_TP_PORT_IN_DISCARDS	(0x58)
#define   RTL960X_ETHER_STATS_TX_PKTS_1519_TO_MAX_OCTETS	(0x50)
#define   RTL960X_IF_OUT_DISCARDS		(0x4c)
#define   RTL960X_DOT3_OUT_PAUSE_FRAMES		(0x48)
#define   RTL960X_DOT3_STATS_EXCESSIVE_COLLISIONS	(0x44)
#define   RTL960X_ETHER_STATS_COLLISIONS	(0x40)
#define   RTL960X_DOT3_STATS_LATE_COLLISIONS	(0x3c)
#define   RTL960X_DOT3_STATS_DEFERRED_TRANSMISSIONS	(0x38)
#define   RTL960X_DOT3_STATS_MULTIPLE_COLLISION_FRAMES	(0x34)
#define   RTL960X_DOT3_STATS_SINGLE_COLLISION_FRAMES	(0x30)
#define   RTL960X_IF_OUT_OCTETS			(0x28) /* 64 bits */
#define   RTL960X_ETHER_STATS_TX_PKTS_1024_TO_1518_OCTETS	(0x24)
#define   RTL960X_ETHER_STATS_TX_PKTS_512_TO_1023_OCTETS	(0x20)
#define   RTL960X_ETHER_STATS_TX_PKTS_256_TO_511_OCTETS	(0x1c)
#define   RTL960X_ETHER_STATS_TX_PKTS_128_TO_255_OCTETS	(0x18)
#define   RTL960X_ETHER_STATS_TX_PKTS_65_TO_127_OCTETS	(0x14)
#define   RTL960X_ETHER_STATS_TX_PKTS_64_OCTETS	(0x10)
#define   RTL960X_ETHER_STATS_TX_OVERSIZE_PKTS	(0xc)
#define   RTL960X_ETHER_STATS_TX_UNDERSIZE_PKTS	(0x8)
#define   RTL960X_ETHER_STATS_TX_BROADCAST_PKTS	(0x4)
#define   RTL960X_ETHER_STATS_TX_MULTICAST_PKTS	(0x0)
#define STAT_RX_OFFSET	(0x600)
#define   RTL960X_ETHER_STATS_RX_PKTS_1024_TO_1518_OCTETS	(STAT_RX_OFFSET + 0x54)
#define   RTL960X_ETHER_STATS_RX_PKTS_512_TO_1023_OCTETS	(STAT_RX_OFFSET + 0x50)
#define   RTL960X_ETHER_STATS_RX_PKTS_256_TO_511_OCTETS	(STAT_RX_OFFSET + 0x4c)
#define   RTL960X_ETHER_STATS_RX_PKTS_128_TO_255_OCTETS	(STAT_RX_OFFSET + 0x48)
#define   RTL960X_ETHER_STATS_RX_PKTS_65_TO_127_OCTETS	(STAT_RX_OFFSET + 0x44)
#define   RTL960X_ETHER_STATS_RX_PKTS_64_OCTETS	(STAT_RX_OFFSET + 0x40)
#define   RTL960X_ETHER_STATS_RX_OVERSIZE_PKTS	(STAT_RX_OFFSET + 0x3c)
#define   RTL960X_ETHER_STATS_RX_UNDERSIZE_PKTS	(STAT_RX_OFFSET + 0x38)
#define   RTL960X_ETHER_STATS_RX_PKTS_1519_TO_MAX_OCTETS	(STAT_RX_OFFSET + 0x30)
#define   RTL960X_IF_IN_BROADCAST_PKTS		(STAT_RX_OFFSET + 0x2c)
#define   RTL960X_IF_IN_MULTICAST_PKTS		(STAT_RX_OFFSET + 0x28)
#define   RTL960X_ETHER_STATS_DROP_EVENTS	(STAT_RX_OFFSET + 0x24)
#define   RTL960X_IF_IN_UCAST_PKTS		(STAT_RX_OFFSET + 0x20)
#define   RTL960X_ETHER_STATS_JABBERS		(STAT_RX_OFFSET + 0x1c)
#define   RTL960X_ETHER_STATS_FRAGMENTS		(STAT_RX_OFFSET + 0x18)
#define   RTL960X_DOT3_CONTROL_IN_UNKNOWN_OPCODES	(STAT_RX_OFFSET + 0x14)
#define   RTL960X_DOT3_IN_PAUSE_FRAMES		(STAT_RX_OFFSET + 0x10)
#define   RTL960X_DOT3_STATS_SYMBOL_ERRORS	(STAT_RX_OFFSET + 0xc)
#define   RTL960X_ETHER_STATS_CRC_ALIGN_ERRORS	(STAT_RX_OFFSET + 0x8)
#define   RTL960X_IF_IN_OCTETS			(STAT_RX_OFFSET + 0x0) /* 64 bits */
#define RTL960X_STAT_PORT_OAM_MIB(x)		(RTL960X_STAT_PORT_MIB_BASE + (x) * 0x8)
#define STAT_OAM_OFFSET	(0xc00)
#define   RTL960X_OUT_OAM_PDU_PKTS	(STAT_OAM_OFFSET + 0x0)
#define   RTL960X_IN_OAM_PDU_PKTS	(STAT_OAM_OFFSET + 0x4)

#define RTL960X_STAT_CTRL		0x34000
#define RTL960X_STAT_PORT_RESET		0x34014
#define RTL960X_STAT_RESET		0x34018
#define   STAT_RESET_STATUS		BIT(3)
#define RTL960X_STAT_RESET_CONFIG	0x3401c
#define   STAT_RESET_CMD		BIT(5)

#define RTL960X_MAX_PACKET_LENGTH	0x3fff

/* Interrupt polarity register */
#define RTL960X_INTR_POLARITY_REG	0x1d000
#define   RTL960X_INTR_POLARITY_MASK	BIT(0)
#define   RTL960X_INTR_POLARITY_HIGH	0
#define   RTL960X_INTR_POLARITY_LOW	1

/* Interrupt mask/status register - enable/check specific interrupt types */
#define RTL960X_INTR_IMR_REG			0x1d00c
#define RTL960X_INTR_IMS_REG			0x1d010
#define   RTL960X_INTR_TOD_UPDATE_MASK		BIT(20)
#define   RTL960X_INTR_SFP_MASK			BIT(19)
#define   RTL960X_INTR_TOD_MASK			BIT(18)
#define   RTL960X_INTR_FB_EVENT_MASK		BIT(17)
#define   RTL960X_INTR_SWITCH_CRASH_MASK	BIT(16)
#define   RTL960X_INTR_SMARTCARD_MASK		BIT(15)
#define   RTL960X_INTR_THERMAL_ALARM_MASK	BIT(14)
#define   RTL960X_INTR_DYING_GASP_MASK		BIT(13)
#define   RTL960X_INTR_PTP_1_SEC_MASK		BIT(12)
#define   RTL960X_INTR_EPON_MASK		BIT(11)
#define   RTL960X_INTR_GPON_MASK		BIT(10)
#define   RTL960X_INTR_SERDES_MASK		BIT(9)
#define   RTL960X_INTR_GPHY_MASK		BIT(8)
#define   RTL960X_INTR_ACL_ACTION_MASK		BIT(7)
#define   RTL960X_INTR_DBGO_MASK		BIT(6)
#define   RTL960X_INTR_LOOP_DETECTION_MASK	BIT(5)
#define   RTL960X_INTR_SPECIAL_CONGEST_MASK	BIT(4)
#define   RTL960X_INTR_SPEED_CHANGE_MASK	BIT(3)
#define   RTL960X_INTR_LEARN_OVER_MASK		BIT(2)
#define   RTL960X_INTR_METER_EXCEEDED_MASK	BIT(1)
#define   RTL960X_INTR_LINK_CHANGE_MASK		BIT(0)
/* The define would be too big listing everything above so genmask it is */
#define   RTL960X_INTR_ALL_MASK			GENMASK(20, 0)

/* Per-port interrupt type status registers */
#define RTL960X_INTR_STATUS_REG			0x1d014
#define   RTL960X_INTR_STAT_THERMAL_MASK	GENMASK(31, 28)
#define   RTL960X_INTR_STAT_GPHY_MASK		GENMASK(20, 16)
#define   RTL960X_INTR_STAT_PORT_LINKDOWN_MASK		RTL960X_PORT_MASK
#define RTL960X_INTR_STATUS1_REG		0x1d018
#define   RTL960X_INTR_STAT_PORT_LINKUP_MASK		(RTL960X_PORT_MASK << 16)
#define   RTL960X_INTR_STAT_PORT_SPEED_CHANGE_MASK	RTL960X_PORT_MASK

struct rtl960x_mib_counter {
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

static const struct rtl960x_mib_counter rtl960x_mib_counters[] = {
	MIB_DESC(RTL960X_ETHER_STATS_TX_MULTICAST_PKTS, 1, "etherStatsTxMulticastPkts"),
	MIB_DESC(RTL960X_ETHER_STATS_TX_BROADCAST_PKTS, 1, "etherStatsTxBroadcastPkts"),
	MIB_DESC(RTL960X_ETHER_STATS_TX_UNDERSIZE_PKTS, 1, "etherStatsTxUndersizePkts"),
	MIB_DESC(RTL960X_ETHER_STATS_TX_OVERSIZE_PKTS, 1, "etherStatsTxOversizePkts"),
	MIB_DESC(RTL960X_ETHER_STATS_COLLISIONS, 1, "etherStatsCollisions"),
	MIB_DESC(RTL960X_IF_OUT_DISCARDS, 1, "ifOutDiscards"),
	MIB_DESC(RTL960X_DOT1D_TP_PORT_IN_DISCARDS, 1, "dot1dTpPortInDiscards"),
	MIB_DESC(RTL960X_ETHER_STATS_DROP_EVENTS, 1, "etherStatsDropEvents"),
	MIB_DESC(RTL960X_ETHER_STATS_CRC_ALIGN_ERRORS, 1, "etherStatsCrcAlignErrors"),
};

enum rtl960x_stp_state {
	RTL960X_STP_STATE_DISABLED = 0,
	RTL960X_STP_STATE_BLOCKING = 1,
	RTL960X_STP_STATE_LEARNING = 2,
	RTL960X_STP_STATE_FORWARDING = 3,
};

static int rtl960x_cpu_port_setup(struct rtl960x_dsa *priv, int port)
{
	int ret;

	/* Force the link to the GMAC up at 1000/full */
	ret = regmap_write(priv->map, RTL960X_FORCE_P_ABLTY_REG(port),
			   RTL960X_FORCE_P_ABLTY_NWAY |
			   RTL960X_FORCE_P_ABLTY_LINK |
			   RTL960X_FORCE_P_ABLTY_DUPLEX |
			   FIELD_PREP(RTL960X_FORCE_P_ABLTY_SPEED_MASK,
				      RTL960X_FORCE_P_ABLTY_SPEED_1000));
	if (ret)
		return ret;

	return regmap_write(priv->map, RTL960X_ABLTY_FORCE_MODE_REG(port),
			    0xffff);
}

static int rtl960x_port_set_isolation(struct rtl960x_dsa *priv, int port,
				      u32 mask)
{
	return regmap_write(priv->map, RTL960X_PORT_ISOLATION_REG(port), mask);
}

static int rtl960x_port_add_isolation(struct rtl960x_dsa *priv, int port,
				      u32 mask)
{
	return regmap_set_bits(priv->map, RTL960X_PORT_ISOLATION_REG(port),
			       mask);
}

static int rtl960x_port_remove_isolation(struct rtl960x_dsa *priv, int port,
					 u32 mask)
{
	return regmap_clear_bits(priv->map, RTL960X_PORT_ISOLATION_REG(port),
				 mask);
}

static int rtl960x_port_mib_reset(struct rtl960x_dsa *priv, u32 pmask)
{
	u32 val;
	int ret;

	ret = regmap_set_bits(priv->map, RTL960X_STAT_PORT_RESET, pmask);
	if (ret)
		return ret;

	ret = regmap_set_bits(priv->map, RTL960X_STAT_RESET_CONFIG, STAT_RESET_CMD);
	if (ret)
		return ret;

	return regmap_read_poll_timeout(priv->map, RTL960X_STAT_RESET, val,
					!FIELD_GET(STAT_RESET_STATUS, val),
					10, 10000);
}

static void rtl960x_mib_counter_read(struct rtl960x_dsa *priv, int port, u32 offset,
				     u32 length, u64 *mibvalue)
{
	u32 low, high, high1, reg = RTL960X_STAT_PORT_MIB(port) + offset;

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

static void rtl960x_get_ethtool_stats(struct dsa_switch *ds, int port, u64 *data)
{
	struct rtl960x_dsa *priv = ds->priv;
	const struct rtl960x_mib_counter *mib;
	int i;

	for (i = 0; i < ARRAY_SIZE(rtl960x_mib_counters); i++) {
		mib = &rtl960x_mib_counters[i];

		rtl960x_mib_counter_read(priv, port, mib->offset, mib->length, data + i);
	}
}

static void rtl960x_get_strings(struct dsa_switch *ds, int port, u32 stringset, u8 *data)
{
	if (stringset != ETH_SS_STATS)
		return;

	for (int i = 0; i < ARRAY_SIZE(rtl960x_mib_counters); i++)
		ethtool_puts(&data, rtl960x_mib_counters[i].name);
}

static int rtl960x_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	if (sset != ETH_SS_STATS)
		return -EOPNOTSUPP;

	return ARRAY_SIZE(rtl960x_mib_counters);
}

static void rtl960x_get_phy_stats(struct dsa_switch *ds, int port,
				  struct ethtool_eth_phy_stats *phy_stats)
{
	struct rtl960x_dsa *priv = ds->priv;

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_STATS_SYMBOL_ERRORS, 1,
				 &phy_stats->SymbolErrorDuringCarrier);
}

static void rtl960x_get_mac_stats(struct dsa_switch *ds, int port,
				  struct ethtool_eth_mac_stats *mac_stats)
{
	struct rtl960x_dsa *priv = ds->priv;
	u64 data;

	/* The RTL960X exposes MIB objects, which we have to translate into
	 * IEEE 802.3 Managed Objects. This is not always completely faithful,
	 * but we try out best. See RFC 3635 for a detailed treatment of the
	 * subject.
	 */

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_UCAST_PKTS, 1,
				 &mac_stats->FramesTransmittedOK);

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_STATS_SINGLE_COLLISION_FRAMES, 1,
				 &mac_stats->SingleCollisionFrames);

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_STATS_MULTIPLE_COLLISION_FRAMES, 1,
				 &mac_stats->MultipleCollisionFrames);

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_IN_UCAST_PKTS, 1,
				 &mac_stats->FramesReceivedOK);

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_OCTETS, 2,
				 &mac_stats->OctetsTransmittedOK);

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_STATS_DEFERRED_TRANSMISSIONS, 1,
				 &mac_stats->FramesWithDeferredXmissions);

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_STATS_LATE_COLLISIONS, 1,
				 &mac_stats->LateCollisions);

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_STATS_EXCESSIVE_COLLISIONS, 1,
				 &mac_stats->FramesAbortedDueToXSColls);

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_IN_OCTETS, 2,
				 &mac_stats->OctetsReceivedOK);

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_MULTICAST_PKTS, 1,
				 &mac_stats->MulticastFramesXmittedOK);
	mac_stats->FramesTransmittedOK += mac_stats->MulticastFramesXmittedOK;

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_BROADCAST_PKTS, 1,
				 &mac_stats->BroadcastFramesXmittedOK);
	mac_stats->FramesTransmittedOK += mac_stats->BroadcastFramesXmittedOK;

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_OUT_PAUSE_FRAMES, 1,
				 &data);
	mac_stats->FramesTransmittedOK += data;

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_DISCARDS, 1, &data);
	mac_stats->FramesTransmittedOK -= data;

	mac_stats->OctetsTransmittedOK -= 18 * mac_stats->FramesTransmittedOK;

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_IN_MULTICAST_PKTS, 1,
				 &mac_stats->MulticastFramesReceivedOK);
	mac_stats->FramesReceivedOK += mac_stats->MulticastFramesReceivedOK;

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_IN_BROADCAST_PKTS, 1,
				 &mac_stats->BroadcastFramesReceivedOK);
	mac_stats->FramesReceivedOK += mac_stats->BroadcastFramesReceivedOK;

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_IN_PAUSE_FRAMES, 1,
				 &data);
	mac_stats->FramesReceivedOK += data;

	mac_stats->OctetsReceivedOK -= 18 * mac_stats->FramesReceivedOK;
}

static void rtl960x_get_ctrl_stats(struct dsa_switch *ds, int port,
				   struct ethtool_eth_ctrl_stats *ctrl_stats)
{
	struct rtl960x_dsa *priv = ds->priv;

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_CONTROL_IN_UNKNOWN_OPCODES, 1,
				   &ctrl_stats->UnsupportedOpcodesReceived);

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_OUT_PAUSE_FRAMES, 1,
				   &ctrl_stats->MACControlFramesTransmitted);

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_IN_PAUSE_FRAMES, 1,
				   &ctrl_stats->MACControlFramesReceived);
}

static const struct ethtool_rmon_hist_range rtl960x_rmon_ranges[] = {
	{ 0, 64 },
	{ 65, 127 },
	{ 128, 255 },
	{ 256, 511 },
	{ 512, 1023 },
	{ 1024, 1518 },
	{ 1519, RTL960X_MAX_PACKET_LENGTH },
	{ }
};

static void rtl960x_get_rmon_stats(struct dsa_switch *ds, int port,
				   struct ethtool_rmon_stats *rmon_stats,
				   const struct ethtool_rmon_hist_range **ranges)
{
	struct rtl960x_dsa *priv = ds->priv;

	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_RX_UNDERSIZE_PKTS, 1,
				 &rmon_stats->undersize_pkts);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_RX_OVERSIZE_PKTS, 1,
				 &rmon_stats->oversize_pkts);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_FRAGMENTS, 1,
				 &rmon_stats->fragments);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_JABBERS, 1,
				 &rmon_stats->jabbers);

	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_RX_PKTS_64_OCTETS, 1,
				 &rmon_stats->hist[0]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_RX_PKTS_65_TO_127_OCTETS, 1,
				 &rmon_stats->hist[1]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_RX_PKTS_128_TO_255_OCTETS, 1,
				 &rmon_stats->hist[2]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_RX_PKTS_256_TO_511_OCTETS, 1,
				 &rmon_stats->hist[3]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_RX_PKTS_512_TO_1023_OCTETS, 1,
				 &rmon_stats->hist[4]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_RX_PKTS_1024_TO_1518_OCTETS, 1,
				 &rmon_stats->hist[5]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_RX_PKTS_1519_TO_MAX_OCTETS, 1,
				 &rmon_stats->hist[6]);

	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_TX_PKTS_64_OCTETS, 1,
				 &rmon_stats->hist_tx[0]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_TX_PKTS_65_TO_127_OCTETS, 1,
				 &rmon_stats->hist_tx[1]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_TX_PKTS_128_TO_255_OCTETS, 1,
				 &rmon_stats->hist_tx[2]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_TX_PKTS_256_TO_511_OCTETS, 1,
				 &rmon_stats->hist_tx[3]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_TX_PKTS_512_TO_1023_OCTETS, 1,
				 &rmon_stats->hist_tx[4]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_TX_PKTS_1024_TO_1518_OCTETS, 1,
				 &rmon_stats->hist_tx[5]);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_TX_PKTS_1519_TO_MAX_OCTETS, 1,
				 &rmon_stats->hist_tx[6]);

	*ranges = rtl960x_rmon_ranges;
}

static void rtl960x_get_pause_stats(struct dsa_switch *ds, int port,
				    struct ethtool_pause_stats *pause_stats)
{
	struct rtl960x_dsa *priv = ds->priv;

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_OUT_PAUSE_FRAMES, 1,
				       &pause_stats->tx_pause_frames);

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_IN_PAUSE_FRAMES, 1,
				       &pause_stats->rx_pause_frames);
}

static void rtl960x_get_stats64(struct dsa_switch *ds, int port,
				struct rtnl_link_stats64 *s)
{

	struct rtl960x_dsa *priv = ds->priv;
	u64 data;

	/* MIB counter doesn't provide a FramesTransmittedOK but instead
	 * provide stats for Unicast, Broadcast and Multicast frames separately.
	 * To simulate a global frame counter, read Unicast and addition Multicast
	 * and Broadcast later
	 */

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_IN_UCAST_PKTS, 1,
				 &s->rx_packets);
	rtl960x_mib_counter_read(priv, port, RTL960X_IF_IN_MULTICAST_PKTS, 1,
				 &s->multicast);
	s->rx_packets += s->multicast;
	rtl960x_mib_counter_read(priv, port, RTL960X_IF_IN_BROADCAST_PKTS, 1,
				 &data);
	s->rx_packets += data;

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_UCAST_PKTS, 1,
				 &s->tx_packets);
	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_MULTICAST_PKTS, 1,
				 &data);
	s->tx_packets += data;
	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_BROADCAST_PKTS, 1,
				 &data);
	s->tx_packets += data;

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_IN_OCTETS, 2,
				 &s->rx_bytes);
	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_OCTETS, 2,
				 &s->tx_bytes);

	/* if{In,Out}Octets includes FCS - remove it */
	s->rx_bytes -= 4 * s->rx_packets;
	s->tx_bytes -= 4 * s->tx_packets;
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_DROP_EVENTS, 1,
				 &s->rx_dropped);

	rtl960x_mib_counter_read(priv, port, RTL960X_IF_OUT_DISCARDS, 1,
				 &s->tx_dropped);

	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_COLLISIONS, 1,
				 &s->collisions);

	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_FRAGMENTS, 1,
				 &s->rx_length_errors);
	rtl960x_mib_counter_read(priv, port, RTL960X_ETHER_STATS_JABBERS, 1,
				 &data);
	s->rx_length_errors += data;

	s->rx_errors = s->rx_length_errors;

	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_STATS_EXCESSIVE_COLLISIONS, 1,
				 &s->tx_aborted_errors);
	rtl960x_mib_counter_read(priv, port, RTL960X_DOT3_STATS_LATE_COLLISIONS, 1,
				 &s->tx_window_errors);

	s->tx_errors = s->tx_aborted_errors + s->tx_window_errors;
}

/*
 * A learn limit of 0 disables learning; the over-limit action is
 * forward-without-learning, so the port keeps forwarding.
 */
static int rtl960x_port_set_learning(struct rtl960x_dsa *priv, int port,
				     bool enable)
{
	int ret;

	ret = regmap_update_bits(priv->map,
				 RTL960X_LUT_LEARN_OVER_CTRL_REG,
				 RTL960X_LUT_LEARN_OVER_ACT_MASK(port),
				 RTL960X_LUT_LEARN_OVER_ACT_FORWARD);
	if (ret)
		return ret;

	return regmap_assign_bits(priv->map, RTL960X_LUT_LRN_LIMITNO_REG(port),
				  RTL960X_LUT_LRN_LIMITNO_MASK(port), enable);
}

static void rtl960x_port_fast_age(struct dsa_switch *ds, int port)
{
	struct rtl960x_dsa *priv = ds->priv;
	int ret;

	ret = rtl960x_l2_flush(priv, port);
	if (ret)
		dev_err(priv->dev, "failed to fast age on port %d: %pe\n",
			port, ERR_PTR(ret));
}

static void rtl960x_port_stp_state_set(struct dsa_switch *ds, int port,
				       u8 state)
{
	struct rtl960x_dsa *priv = ds->priv;
	enum rtl960x_stp_state val;
	int msti;

	switch (state) {
	case BR_STATE_DISABLED:
		val = RTL960X_STP_STATE_DISABLED;
		break;
	case BR_STATE_BLOCKING:
	case BR_STATE_LISTENING:
		val = RTL960X_STP_STATE_BLOCKING;
		break;
	case BR_STATE_LEARNING:
		val = RTL960X_STP_STATE_LEARNING;
		break;
	case BR_STATE_FORWARDING:
		val = RTL960X_STP_STATE_FORWARDING;
		break;
	default:
		dev_err(priv->dev, "invalid STP state: %u\n", state);
		return;
	}

	/*
	 * A VLAN entry's FID field doubles as its MSTI: standalone ports and
	 * VLAN-aware bridges use FID 0, VLAN-unaware bridges FID 1. MST is not
	 * offloaded, so mirror the port's single STP state into every MSTI.
	 */
	for (msti = 0; msti < RTL960X_NUM_MSTI; msti++)
		regmap_update_bits(priv->map, RTL960X_MSTI_CTRL_REG(port),
				   RTL960X_MSTI_CTRL_PORT_STATE_MASK(msti),
				   field_prep(RTL960X_MSTI_CTRL_PORT_STATE_MASK(msti),
					      val));
}

static int rtl960x_get_and_clear_status_reg(struct rtl960x_dsa *priv, u32 reg,
					    u32 *val)
{
	int ret;

	ret = regmap_read(priv->map, reg, val);
	if (ret)
		return ret;

	return regmap_write(priv->map, reg, *val);
}

static irqreturn_t rtl960x_irq(int irq, void *data)
{
	struct rtl960x_dsa *priv = data;
	unsigned long line_changes = 0;
	u32 stat;
	int line;
	int ret;

	ret = rtl960x_get_and_clear_status_reg(priv, RTL960X_INTR_IMS_REG,
					       &stat);
	if (ret)
		goto out_error;

	if (stat & (RTL960X_INTR_LINK_CHANGE_MASK | RTL960X_INTR_SPEED_CHANGE_MASK)) {
		u32 linkdown_ind, linkup_ind, speed_ind, val;

		ret = rtl960x_get_and_clear_status_reg(
			priv, RTL960X_INTR_STATUS_REG, &val);
		if (ret)
			goto out_error;

		linkdown_ind = FIELD_GET(RTL960X_INTR_STAT_PORT_LINKDOWN_MASK, val);

		ret = rtl960x_get_and_clear_status_reg(
			priv, RTL960X_INTR_STATUS1_REG, &val);
		if (ret)
			goto out_error;

		linkup_ind = FIELD_GET(RTL960X_INTR_STAT_PORT_LINKUP_MASK, val);

		speed_ind = FIELD_GET(RTL960X_INTR_STAT_PORT_SPEED_CHANGE_MASK, val);

		line_changes = linkdown_ind | linkup_ind | speed_ind;
	}

	if (!line_changes)
		goto out_none;

	for_each_set_bit(line, &line_changes, RTL960X_NUM_PORTS) {
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

static struct irq_chip rtl960x_irq_chip = {
	.name = "rtl960x-switch",
	/* The hardware doesn't support masking IRQs on a per-port basis */
};

static int rtl960x_irq_map(struct irq_domain *domain, unsigned int irq,
			     irq_hw_number_t hwirq)
{
	struct rtl960x_dsa *priv = domain->host_data;

	irq_set_chip_data(irq, priv);
	irq_set_chip_and_handler(irq, &rtl960x_irq_chip, handle_simple_irq);
	irq_set_nested_thread(irq, 1);
	irq_set_noprobe(irq);
	irq_set_parent(irq, priv->irq);

	return 0;
}

static void rtl960x_irq_unmap(struct irq_domain *d, unsigned int irq)
{
	irq_set_nested_thread(irq, 0);
	irq_set_chip_and_handler(irq, NULL, NULL);
	irq_set_chip_data(irq, NULL);
}

static const struct irq_domain_ops rtl960x_irqdomain_ops = {
	.map = rtl960x_irq_map,
	.unmap = rtl960x_irq_unmap,
	.xlate = irq_domain_xlate_onecell,
};

static int rtl960x_irq_enable(struct rtl960x_dsa *priv)
{
	return regmap_write(priv->map, RTL960X_INTR_IMR_REG,
			    RTL960X_INTR_LINK_CHANGE_MASK |
			    RTL960X_INTR_SPEED_CHANGE_MASK);
}

static int rtl960x_irq_disable(struct rtl960x_dsa *priv)
{
	return regmap_write(priv->map, RTL960X_INTR_IMR_REG, 0);
}

static void rtl960x_irq_teardown(struct rtl960x_dsa *priv)
{
	struct dsa_switch *ds = priv->ds;
	struct dsa_port *dp;
	int virq;

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

	if (priv->irq) {
		free_irq(priv->irq, priv);
		priv->irq = 0;
	}
}

static int rtl960x_irq_setup(struct rtl960x_dsa *priv)
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

	/* RTL960X IRQs cascade off this one */
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

	priv->irqdomain = irq_domain_create_linear(of_fwnode_handle(intc), RTL960X_NUM_PORTS,
						   &rtl960x_irqdomain_ops, priv);
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
		val = RTL960X_INTR_POLARITY_HIGH;
		break;
	case IRQF_TRIGGER_FALLING:
	case IRQF_TRIGGER_LOW:
		val = RTL960X_INTR_POLARITY_LOW;
		break;
	default:
		dev_err(priv->dev, "unsupported irq trigger type %u\n",
			irq_trig);
		ret = -EINVAL;
		goto out_teardown_irq;
	}

	ret = regmap_update_bits(priv->map, RTL960X_INTR_POLARITY_REG,
				 RTL960X_INTR_POLARITY_MASK, val);
	if (ret)
		goto out_teardown_irq;

	/* Disable the interrupt in case the chip has it enabled on reset */
	ret = rtl960x_irq_disable(priv);
	if (ret)
		goto out_teardown_irq;

	/* Clear the interrupt status register */
	ret = regmap_write(priv->map, RTL960X_INTR_IMS_REG,
			   RTL960X_INTR_ALL_MASK);
	if (ret)
		goto out_teardown_irq;

	ret = regmap_write(priv->map, RTL960X_INTR_STATUS_REG,
			   RTL960X_INTR_STAT_PORT_LINKDOWN_MASK |
			   RTL960X_INTR_STAT_GPHY_MASK);
	if (ret)
		goto out_teardown_irq;

	ret = regmap_write(priv->map, RTL960X_INTR_STATUS1_REG,
			   RTL960X_INTR_STAT_PORT_LINKUP_MASK |
			   RTL960X_INTR_STAT_PORT_SPEED_CHANGE_MASK);
	if (ret)
		goto out_teardown_irq;

	ret = request_threaded_irq(irq, NULL, rtl960x_irq, IRQF_ONESHOT,
				   "rtl960x-switch", priv);
	if (ret) {
		dev_err(priv->dev, "failed to request irq: %pe\n",
			ERR_PTR(ret));
		goto out_teardown_irq;
	}

	ret = rtl960x_irq_enable(priv);
	if (ret)
		goto out_teardown_irq;

	of_node_put(intc);

	return 0;

out_teardown_irq:
	rtl960x_irq_teardown(priv);

out_put_node:
	priv->irq = 0;
	of_node_put(intc);

	return ret;
}

static enum dsa_tag_protocol rtl960x_dsa_get_tag_protocol(struct dsa_switch *ds,
							  int port,
							  enum dsa_tag_protocol mp)
{
	return DSA_TAG_PROTO_RTL_OTTO;
}

static int rtl960x_dsa_setup(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	u32 cpu_ports = 0, user_ports = 0;
	struct dsa_port *dp;
	int ret;

	/* Set up cascading IRQs */
	ret = rtl960x_irq_setup(priv);
	if (ret == -EPROBE_DEFER)
		return ret;
	else if (ret)
		dev_info(priv->dev, "no interrupt support\n");

	/*
	 * Start with all ports completely isolated and not learning, the CPU
	 * and unused ports included, as rtl8365mb does. Learning is enabled
	 * per user port by the bridge's BR_LEARNING flag; a standalone port
	 * must not learn, or a destination it learned on another port would
	 * be blocked by isolation instead of reaching the CPU for the software
	 * bridge. Unknown destinations are flooded within the ingress port's
	 * isolation group, which is how CPU-bound traffic reaches the CPU.
	 */
	dsa_switch_for_each_port(dp, ds) {
		ret = rtl960x_port_set_isolation(priv, dp->index, 0);
		if (ret)
			goto out_teardown_irq;

		ret = rtl960x_port_set_learning(priv, dp->index, false);
		if (ret)
			goto out_teardown_irq;

		if (dsa_port_is_cpu(dp))
			cpu_ports |= BIT(dp->index);
	}

	/* User ports forward only to the CPU ports */
	dsa_switch_for_each_user_port(dp, ds) {
		ret = rtl960x_port_set_isolation(priv, dp->index, cpu_ports);
		if (ret)
			goto out_teardown_irq;

		user_ports |= BIT(dp->index);
	}

	/* CPU ports forward to every user port */
	dsa_switch_for_each_cpu_port(dp, ds) {
		ret = rtl960x_cpu_port_setup(priv, dp->index);
		if (ret)
			goto out_teardown_irq;

		ret = rtl960x_port_set_isolation(priv, dp->index, user_ports);
		if (ret)
			goto out_teardown_irq;
	}

	/* One reserved VID per offloaded bridge; DSA numbers them for us. */
	ds->max_num_bridges = RTL960X_NUM_BRIDGE_VIDS;

	ds->assisted_learning_on_cpu_port = true;

	ret = rtl960x_port_mib_reset(priv, user_ports | cpu_ports);
	if (ret)
		goto out_teardown_irq;

	/* Every user port starts standalone (see rtl960x_vlan_setup). */
	ret = rtl960x_vlan_setup(ds);
	if (ret)
		goto out_teardown_irq;

	return 0;

out_teardown_irq:
	rtl960x_irq_teardown(priv);

	return ret;
}

static void rtl960x_teardown(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;

	rtl960x_irq_teardown(priv);
}

static int rtl960x_port_bridge_join(struct dsa_switch *ds, int port,
				    struct dsa_bridge bridge,
				    bool *tx_fwd_offload,
				    struct netlink_ext_ack *extack)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct dsa_port *dp;
	u32 mask = 0;
	int ret;

	dev_dbg(priv->dev, "bridge %d join port %d\n", bridge.num, port);

	/*
	 * The port forwards as standalone until here. Block it before the
	 * isolation groups are opened, so nothing crosses into the bridge
	 * before its VLANs are replayed; DSA then syncs the bridge port's STP
	 * state, and the bridge enables the port. A join DSA rolls back later
	 * is put back into forwarding by rtl960x_port_bridge_leave(); a
	 * failure below restores it here, as DSA does not call leave then.
	 */
	rtl960x_port_stp_state_set(ds, port, BR_STATE_BLOCKING);

	/*
	 * Add this port to the isolation group of every other port
	 * offloading this bridge.
	 */
	dsa_switch_for_each_bridge_member(dp, ds, bridge.dev) {
		if (dp->index == port)
			continue;

		ret = rtl960x_port_add_isolation(priv, dp->index, BIT(port));
		if (ret)
			goto undo_isolation;

		mask |= BIT(dp->index);
	}

	/* Add those ports to the isolation group of this port */
	ret = rtl960x_port_add_isolation(priv, port, mask);
	if (ret)
		goto undo_isolation;

	ret = rtl960x_vlan_port_bridge_join(ds, port, bridge, extack);
	if (ret)
		goto undo_self_isolation;

	return 0;

undo_self_isolation:
	rtl960x_port_remove_isolation(priv, port, mask);

undo_isolation:
	dsa_switch_for_each_user_port(dp, ds) {
		if (mask & BIT(dp->index))
			rtl960x_port_remove_isolation(priv, dp->index,
						      BIT(port));
	}
	rtl960x_port_stp_state_set(ds, port, BR_STATE_FORWARDING);

	return ret;
}

static void rtl960x_port_bridge_leave(struct dsa_switch *ds, int port,
				      struct dsa_bridge bridge)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct dsa_port *dp;
	u32 mask = 0;
	int ret;

	dev_dbg(priv->dev, "bridge %d leave port %d\n", bridge.num, port);

	/*
	 * Remove this port from the isolation group of every other port
	 * offloading this bridge.
	 */
	dsa_switch_for_each_bridge_member(dp, ds, bridge.dev) {
		if (dp->index == port)
			continue;

		ret = rtl960x_port_remove_isolation(priv, dp->index, BIT(port));
		if (ret)
			dev_err(priv->dev,
				"failed to remove port %d from isolation group of port %d: %pe\n",
				port, dp->index, ERR_PTR(ret));

		mask |= BIT(dp->index);
	}

	/* Remove those ports from the isolation group of this port */
	ret = rtl960x_port_remove_isolation(priv, port, mask);
	if (ret)
		dev_err(priv->dev,
			"failed to remove isolation group of port %d: %pe\n",
			port, ERR_PTR(ret));

	/*
	 * On a normal leave the bridge disables the port, which fast-ages it,
	 * and DSA then clears BR_LEARNING and puts the port back into
	 * forwarding. A join that DSA rolls back after inheriting the bridge
	 * port's flags and (disabled) STP state leaves without any of that,
	 * so restore the standalone state here: learning off, the port's
	 * dynamic entries flushed, the standalone VLAN state, and forwarding
	 * last, once the port is standalone again.
	 */
	ret = rtl960x_port_set_learning(priv, port, false);
	if (ret)
		dev_err(priv->dev, "failed to disable learning on port %d: %pe\n",
			port, ERR_PTR(ret));
	rtl960x_port_fast_age(ds, port);
	rtl960x_vlan_port_bridge_leave(ds, port, bridge);
	rtl960x_port_stp_state_set(ds, port, BR_STATE_FORWARDING);
}

static int rtl960x_port_pre_bridge_flags(struct dsa_switch *ds, int port,
					 struct switchdev_brport_flags flags,
					 struct netlink_ext_ack *extack)
{
	/* Flooding follows the hardware defaults (everything is flooded). */
	if (flags.mask & ~BR_LEARNING)
		return -EINVAL;

	return 0;
}

static int rtl960x_port_bridge_flags(struct dsa_switch *ds, int port,
				     struct switchdev_brport_flags flags,
				     struct netlink_ext_ack *extack)
{
	struct rtl960x_dsa *priv = ds->priv;

	if (flags.mask & BR_LEARNING)
		return rtl960x_port_set_learning(priv, port,
						 !!(flags.val & BR_LEARNING));

	return 0;
}

static void rtl960x_dsa_phylink_get_caps(struct dsa_switch *ds, int port,
					 struct phylink_config *config)
{
	switch (port) {
	/* Ports connected to the integrated GPHYs. There is no MII pinout. */
	case 0 ... 3:
		__set_bit(PHY_INTERFACE_MODE_INTERNAL,
			  config->supported_interfaces);
		/*
		 * GMII is the default interface mode for phylib, so we have
		 * to support it for ports with integrated PHY.
		 */
		__set_bit(PHY_INTERFACE_MODE_GMII,
			  config->supported_interfaces);

		config->mac_capabilities = MAC_SYM_PAUSE | MAC_ASYM_PAUSE |
					   MAC_10 | MAC_100 | MAC_1000FD;
		break;

	/*
	 * CPU ports connected to GMAC0 and GMAC1, forced to 1000/full without
	 * flow control in rtl960x_cpu_port_setup().
	 */
	case 9:
	case 10:
		__set_bit(PHY_INTERFACE_MODE_INTERNAL,
			  config->supported_interfaces);

		config->mac_capabilities = MAC_1000FD;
		break;

	/*
	 * Port 5 (PON SerDes) and ports 6 and 7 (HSGMII SerDes) need SerDes
	 * configuration this driver does not implement; port 7 as the GMAC2
	 * CPU port is also not supported yet. Ports 4 and 8 are unused. Leave
	 * supported_interfaces empty so phylink rejects them rather than
	 * accepting a mode this driver never configures.
	 */
	default:
		break;
	}
}

static struct dsa_port *
rtl960x_dsa_preferred_default_local_cpu_port(struct dsa_switch *ds)
{
	struct dsa_port *cpu_dp = dsa_to_port(ds, RTL960X_DEFAULT_CPU_PORT);

	if (dsa_port_is_cpu(cpu_dp))
		return cpu_dp;

	return NULL;
}

static void rtl960x_dsa_mac_config(struct phylink_config *config,
				   unsigned int mode,
				   const struct phylink_link_state *state)
{
}

static void rtl960x_dsa_mac_link_down(struct phylink_config *config,
				      unsigned int mode,
				      phy_interface_t interface)
{
}

static void rtl960x_dsa_mac_link_up(struct phylink_config *config,
				    struct phy_device *phydev,
				    unsigned int mode,
				    phy_interface_t interface,
				    int speed, int duplex,
				    bool tx_pause, bool rx_pause)
{
	/*
	 * The internal GPHYs and the switch MAC sync speed/duplex in hardware,
	 * so nothing to program here; phylink uses the PHY link state to drive
	 * the per-port netdev carrier.
	 */
}

static const struct phylink_mac_ops rtl960x_dsa_phylink_mac_ops = {
	.mac_config	= rtl960x_dsa_mac_config,
	.mac_link_down	= rtl960x_dsa_mac_link_down,
	.mac_link_up	= rtl960x_dsa_mac_link_up,
};

static const struct dsa_switch_ops rtl960x_dsa_ops = {
	.get_tag_protocol	= rtl960x_dsa_get_tag_protocol,
	.setup			= rtl960x_dsa_setup,
	.teardown		= rtl960x_teardown,
	.preferred_default_local_cpu_port = rtl960x_dsa_preferred_default_local_cpu_port,
	.phylink_get_caps	= rtl960x_dsa_phylink_get_caps,
	.port_bridge_join	= rtl960x_port_bridge_join,
	.port_bridge_leave	= rtl960x_port_bridge_leave,
	.port_pre_bridge_flags	= rtl960x_port_pre_bridge_flags,
	.port_bridge_flags	= rtl960x_port_bridge_flags,
	.port_stp_state_set	= rtl960x_port_stp_state_set,
	.port_fast_age		= rtl960x_port_fast_age,
	.port_vlan_filtering	= rtl960x_port_vlan_filtering,
	.port_vlan_add		= rtl960x_port_vlan_add,
	.port_vlan_del		= rtl960x_port_vlan_del,
	.port_fdb_add		= rtl960x_port_fdb_add,
	.port_fdb_del		= rtl960x_port_fdb_del,
	.port_fdb_dump		= rtl960x_port_fdb_dump,
	.get_strings		= rtl960x_get_strings,
	.get_ethtool_stats	= rtl960x_get_ethtool_stats,
	.get_sset_count		= rtl960x_get_sset_count,
	.get_eth_phy_stats	= rtl960x_get_phy_stats,
	.get_eth_mac_stats	= rtl960x_get_mac_stats,
	.get_eth_ctrl_stats	= rtl960x_get_ctrl_stats,
	.get_rmon_stats		= rtl960x_get_rmon_stats,
	.get_stats64		= rtl960x_get_stats64,
	.get_pause_stats	= rtl960x_get_pause_stats,
};

static int rtl960x_dsa_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rtl960x_dsa *priv;
	struct dsa_switch *ds;
	int ret;

	ret = otto_table_loaded();
	if (ret)
		return dev_err_probe(dev, ret, "table access not available\n");

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	ds = devm_kzalloc(dev, sizeof(*ds), GFP_KERNEL);
	if (!ds)
		return -ENOMEM;

	priv->dev = dev;
	priv->ds = ds;

	ret = devm_mutex_init(dev, &priv->l2_lock);
	if (ret)
		return ret;

	priv->map = syscon_node_to_regmap(dev->of_node);
	if (IS_ERR(priv->map))
		return dev_err_probe(dev, PTR_ERR(priv->map),
				     "failed to get switch-core regmap\n");

	ds->dev = dev;
	ds->num_ports = RTL960X_NUM_PORTS;
	ds->ops = &rtl960x_dsa_ops;
	ds->phylink_mac_ops = &rtl960x_dsa_phylink_mac_ops;
	ds->priv = priv;

	platform_set_drvdata(pdev, priv);

	return dsa_register_switch(ds);
}

static void rtl960x_dsa_remove(struct platform_device *pdev)
{
	struct rtl960x_dsa *priv = platform_get_drvdata(pdev);

	if (priv)
		dsa_unregister_switch(priv->ds);
}

static void rtl960x_dsa_shutdown(struct platform_device *pdev)
{
	struct rtl960x_dsa *priv = platform_get_drvdata(pdev);

	if (priv)
		dsa_switch_shutdown(priv->ds);

	platform_set_drvdata(pdev, NULL);
}

static const struct of_device_id rtl960x_dsa_of_match[] = {
	{ .compatible = "realtek,rtl9607c-switch" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtl960x_dsa_of_match);

static struct platform_driver rtl960x_dsa_driver = {
	.probe = rtl960x_dsa_probe,
	.remove = rtl960x_dsa_remove,
	.shutdown = rtl960x_dsa_shutdown,
	.driver = {
		.name = "rtl960x-switch",
		.of_match_table = rtl960x_dsa_of_match,
	},
};
module_platform_driver(rtl960x_dsa_driver);

MODULE_DESCRIPTION("Realtek RTL9607C family DSA switch driver");
MODULE_AUTHOR("Taiga Ogawa");
MODULE_LICENSE("GPL");
