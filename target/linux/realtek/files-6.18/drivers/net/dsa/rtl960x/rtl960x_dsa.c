// SPDX-License-Identifier: GPL-2.0-only
/*
 * DSA switch driver for the Realtek RTL9607C family switch core.
 *
 * The RTL9607C family of SoCs has an embedded 11 port switch controller.
 * rtl960x_dsa_phylink_get_caps() describes the ports this driver supports.
 */

#include <linux/bitfield.h>
#include <linux/if_bridge.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
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

/*
 * Port isolation registers - one egress portmask per port. Bits 28:11 are
 * the extension ports of MAC7/9/10, which the driver leaves clear.
 */
#define RTL960X_PORT_ISOLATION_REG(_port)	(0x27000 + (_port) * 4)
#define   RTL960X_PORT_ISOLATION_MASK		GENMASK(10, 0)

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

/* VLAN ingress filter register */
#define RTL960X_VLAN_INGRESS_REG		0x13004
#define   RTL960X_VLAN_INGRESS_MASK		GENMASK(10, 0)
#define   RTL960X_VLAN_INGRESS_FILTER_PORT_EN_MASK(_p)	BIT(_p)

/* VLAN control register */
#define RTL960X_VLAN_CTRL_REG			0x13008
#define   RTL960X_VLAN_FILTER_EN_MASK		BIT(0)

/* VLAN egress mode register */
#define RTL960X_VLAN_EGRESS_TAG_REG(_p)		(0x2a000 + 4 * _p)
#define   RTL960X_VLAN_EGRESS_MODE_MASK		GENMASK(1, 0)

/**
 * enum rtl960x_vlan_egress_mode - port VLAN egress mode
 * @RTL960X_VLAN_EGRESS_MODE_ORIGINAL: follow untag mask in VLAN4k table entry
 * @RTL960X_VLAN_EGRESS_MODE_KEEP: the VLAN tag format of egressed packets
 * will remain the same as their ingressed format, but the priority and VID
 * fields may be altered
 * @RTL960X_VLAN_EGRESS_MODE_PRI_TAG: always egress with priority tag
 * @RTL960X_VLAN_EGRESS_MODE_REAL_KEEP: the VLAN tag format of egressed
 * packets will remain the same as their ingressed format, and neither the
 * priority nor VID fields can be altered
 */
enum rtl960x_vlan_egress_mode {
	RTL960X_VLAN_EGRESS_MODE_ORIGINAL = 0,
	RTL960X_VLAN_EGRESS_MODE_KEEP = 1,
	RTL960X_VLAN_EGRESS_MODE_PRI_TAG = 2,
	RTL960X_VLAN_EGRESS_MODE_REAL_KEEP = 3,
};

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

#define RTL960X_TABLE_ACCESS_REG	0x12000
#define   RTL960X_TABLE_CMD_TYPE_MSK	GENMASK(4, 3)
#define   RTL960X_TABLE_RESET_CMD	2
#define   RTL960X_TABLE_TYPE_MSK	GENMASK(2, 0)
#define   RTL960X_TABLE_VLAN		1
#define RTL960X_TABLE_ACCESS_STS_REG	0x12004
#define   RTL960X_TABLE_BUSY_FLAG	BIT(13)

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

static int rtl960x_port_set_ingress_filtering(struct rtl960x_dsa *priv,
					      int port, bool enable)
{
	/* Ingress filtering enabled: Discard VLAN-tagged frames if the port is
	 * not a member of the VLAN with which the packet is associated.
	 * Untagged packets will also be discarded unless the port has a PVID
	 * programmed. Priority-tagged frames are treated as untagged frames.
	 *
	 * Ingress filtering disabled: Accept all tagged and untagged frames.
	 */
	return regmap_assign_bits(priv->map, RTL960X_VLAN_INGRESS_REG,
				  RTL960X_VLAN_INGRESS_FILTER_PORT_EN_MASK(port),
				  enable);
}

static int rtl960x_port_set_vlan_egress_mode(struct rtl960x_dsa *priv, int port,
					     enum rtl960x_vlan_egress_mode mode)
{
	u32 val;

	val = FIELD_PREP(RTL960X_VLAN_EGRESS_MODE_MASK, mode);
	return regmap_update_bits(priv->map, RTL960X_VLAN_EGRESS_TAG_REG(port),
				  RTL960X_VLAN_EGRESS_MODE_MASK, val);
}

static int rtl960x_vlan_set_transparent(struct rtl960x_dsa *priv, u16 vid)
{
	u16 member = priv->mbr_mask[vid];

	if (priv->transparent_mask)
		member |= priv->transparent_mask | dsa_cpu_ports(priv->ds);

	return rtl960x_vlan_4k_set(priv, vid, member, priv->untag_mask[vid]);
}

/* RTL9607C has no VLAN "transparent" register like rtl8365mb, so ports that do
 * not filter VLANs and cpu ports are made members of every VLAN instead.
 */
static int rtl960x_vlan_set_all_transparent(struct rtl960x_dsa *priv)
{
	int ret;

	for (int vid = 0; vid < RTL960X_MAX_4K_VID; vid++) {
		ret = rtl960x_vlan_set_transparent(priv, vid);
		if (ret)
			return ret;
	}

	return 0;
}

/* Program the PVID that the port should use for untagged frames: the PVID of
 * the bridge while the port filters VLANs, and the neutral PVID (VID 0)
 * otherwise. A VLAN-unaware bridge, like a standalone port, gets its FDB and
 * MDB entries with VID 0 from DSA. As the switch looks up its forwarding
 * database by the VLAN that it classifies a frame to, a bridge PVID would make
 * those entries miss every lookup.
 */
static int rtl960x_port_commit_pvid(struct dsa_switch *ds, int port,
				    bool vlan_filtering,
				    struct netlink_ext_ack *extack)
{
	struct rtl960x_dsa *priv = ds->priv;
	u16 pvid;

	if (vlan_filtering)
		pvid = priv->bridge_pvid[port];
	else
		pvid = 0;

	return rtl960x_vlan_pvid_port_commit(ds, port, pvid, vlan_filtering,
					     extack);
}

static int rtl960x_port_vlan_filtering(struct dsa_switch *ds, int port,
				       bool vlan_filtering,
				       struct netlink_ext_ack *extack)
{
	enum rtl960x_vlan_egress_mode mode;
	struct rtl960x_dsa *priv = ds->priv;
	u16 prev_transparent;
	int ret;

	dev_dbg(priv->dev, "port %d: %s VLAN filtering\n", port,
		vlan_filtering ? "enable" : "disable");

	mutex_lock(&priv->vlan_lock);

	/* When vlan filter is enable/disabled in a bridge, this function is
	 * called for all member ports. We need to enable/disable ingress
	 * VLAN membership check.
	 */
	ret = rtl960x_port_set_ingress_filtering(priv, port, vlan_filtering);
	if (ret)
		goto out_unlock;

	prev_transparent = priv->transparent_mask;
	if (dsa_is_user_port(ds, port)) {
		if (vlan_filtering)
			priv->transparent_mask &= ~BIT(port);
		else
			priv->transparent_mask |= BIT(port);
	}

	if (priv->transparent_mask != prev_transparent) {
		ret = rtl960x_vlan_set_all_transparent(priv);
		if (ret)
			goto undo_transparent;
	}

	/* Also sets which frames the port accepts: while filtering, untagged
	 * frames are only accepted if the bridge gave the port a PVID.
	 */
	ret = rtl960x_port_commit_pvid(ds, port, vlan_filtering, extack);
	if (ret)
		goto undo_transparent;

	/* When VLAN filtering is disabled, preserve frames exactly as received.
	 * Otherwise, the VLAN egress pipeline may still alter tag state
	 * according to VLAN membership and untag configuration.
	 */
	if (vlan_filtering)
		mode = RTL960X_VLAN_EGRESS_MODE_ORIGINAL;
	else
		mode = RTL960X_VLAN_EGRESS_MODE_REAL_KEEP;

	ret = rtl960x_port_set_vlan_egress_mode(priv, port, mode);
	if (ret)
		goto undo_commit_pvid;

	goto out_unlock;

undo_commit_pvid:
	(void)rtl960x_port_commit_pvid(ds, port, !vlan_filtering, NULL);
undo_transparent:
	if (priv->transparent_mask != prev_transparent) {
		priv->transparent_mask = prev_transparent;
		(void)rtl960x_vlan_set_all_transparent(priv);
	}
	rtl960x_port_set_ingress_filtering(priv, port, !vlan_filtering);

out_unlock:
	mutex_unlock(&priv->vlan_lock);
	return ret;
}

static int rtl960x_port_vlan_add(struct dsa_switch *ds, int port,
				 const struct switchdev_obj_port_vlan *vlan,
				 struct netlink_ext_ack *extack)
{
	bool untagged = !!(vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED);
	bool pvid = !!(vlan->flags & BRIDGE_VLAN_INFO_PVID);
	struct rtl960x_dsa *priv = ds->priv;
	u16 prev_bridge_pvid, prev_mbr, prev_untag;
	u16 *bridge_pvid;
	bool vlan_filtering;
	int ret;

	dev_dbg(priv->dev, "add VLAN %d on port %d, %s, %s\n",
		vlan->vid, port, untagged ? "untagged" : "tagged",
		pvid ? "PVID" : "no PVID");

	/* VID == 0 is reserved in this driver */
	if (vlan->vid == 0) {
		NL_SET_ERR_MSG_MOD(extack,
				   "VLAN 0 is reserved by this driver");
		return -EOPNOTSUPP;
	}

	mutex_lock(&priv->vlan_lock);

	vlan_filtering = dsa_port_is_vlan_filtering(dsa_to_port(ds, port));
	bridge_pvid = &priv->bridge_pvid[port];

	/* Set PVID if needed, or unset it if this VLAN was the PVID */
	prev_bridge_pvid = *bridge_pvid;
	if (pvid)
		*bridge_pvid = vlan->vid;
	else if (*bridge_pvid == vlan->vid)
		*bridge_pvid = 0;

	ret = rtl960x_port_commit_pvid(ds, port, vlan_filtering, extack);
	if (ret)
		goto undo_bridge_pvid;

	/* add port to vlan4k. It knows nothing about PVID */
	prev_mbr = priv->mbr_mask[vlan->vid];
	prev_untag = priv->untag_mask[vlan->vid];
	priv->mbr_mask[vlan->vid] |= BIT(port);
	if (untagged)
		priv->untag_mask[vlan->vid] |= BIT(port);
	else
		priv->untag_mask[vlan->vid] &= ~BIT(port);

	ret = rtl960x_vlan_set_transparent(priv, vlan->vid);
	if (ret)
		goto undo_commit_pvid;

	goto out_unlock;

undo_commit_pvid:
	priv->mbr_mask[vlan->vid] = prev_mbr;
	priv->untag_mask[vlan->vid] = prev_untag;
	*bridge_pvid = prev_bridge_pvid;
	(void)rtl960x_port_commit_pvid(ds, port, vlan_filtering, NULL);
	goto out_unlock;
undo_bridge_pvid:
	*bridge_pvid = prev_bridge_pvid;
out_unlock:
	mutex_unlock(&priv->vlan_lock);
	return ret;
}

static int rtl960x_port_vlan_del(struct dsa_switch *ds, int port,
				 const struct switchdev_obj_port_vlan *vlan)
{
	bool untagged = !!(vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED);
	bool pvid = !!(vlan->flags & BRIDGE_VLAN_INFO_PVID);
	struct rtl960x_dsa *priv = ds->priv;
	u16 prev_bridge_pvid, prev_mbr, prev_untag;
	u16 *bridge_pvid;
	bool vlan_filtering;
	int ret;

	dev_dbg(priv->dev, "del VLAN %d on port %d, %s, %s\n",
		vlan->vid, port, untagged ? "untagged" : "tagged",
		pvid ? "PVID" : "no PVID");

	/* VID == 0 is reserved in this driver */
	if (vlan->vid == 0)
		return -EOPNOTSUPP;

	vlan_filtering = dsa_port_is_vlan_filtering(dsa_to_port(ds, port));
	bridge_pvid = &priv->bridge_pvid[port];

	mutex_lock(&priv->vlan_lock);
	prev_bridge_pvid = *bridge_pvid;
	if (*bridge_pvid == vlan->vid)
		*bridge_pvid = 0;

	ret = rtl960x_port_commit_pvid(ds, port, vlan_filtering, NULL);
	if (ret) {
		/* keep the state in line with the PVID still in hardware */
		*bridge_pvid = prev_bridge_pvid;
		goto out_unlock;
	}

	prev_mbr = priv->mbr_mask[vlan->vid];
	prev_untag = priv->untag_mask[vlan->vid];
	priv->mbr_mask[vlan->vid] &= ~BIT(port);
	priv->untag_mask[vlan->vid] &= ~BIT(port);

	ret = rtl960x_vlan_set_transparent(priv, vlan->vid);
	if (ret)
		goto out_undo_priv_mask;
	/* There is little incentive to try to undo the removal of PVID
	 * (if it was really in use) as an error here might indicate
	 * the ASIC stopped to answer. Restore the configured masks.
	 */
	goto out_unlock;

out_undo_priv_mask:
	priv->mbr_mask[vlan->vid] = prev_mbr;
	priv->untag_mask[vlan->vid] = prev_untag;
out_unlock:
	mutex_unlock(&priv->vlan_lock);
	return ret;
}

/* Should be moved to rtl960x_table at some point */
static int rtl960x_vlan_table_reset(struct rtl960x_dsa *priv)
{
	u32 val;
	int ret;

	ret = regmap_write(priv->map, RTL960X_TABLE_ACCESS_REG,
			   FIELD_PREP(RTL960X_TABLE_CMD_TYPE_MSK, RTL960X_TABLE_RESET_CMD) |
			   FIELD_PREP(RTL960X_TABLE_TYPE_MSK, RTL960X_TABLE_VLAN));
	if (ret)
		return ret;

	return regmap_read_poll_timeout(priv->map, RTL960X_TABLE_ACCESS_STS_REG, val,
					!FIELD_GET(RTL960X_TABLE_BUSY_FLAG, val),
					10, 10000);
}

/* VLAN support is always enabled in the switch.
 *
 * Standalone forwarding relies on transparent VLAN mode combined with per-port
 * isolation masks restricting egress to CPU ports only.
 *
 */
static int rtl960x_vlan_setup(struct dsa_switch *ds)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct dsa_port *dp;
	int ret;

	ret = rtl960x_vlan_table_reset(priv);
	if (ret) {
		dev_err(priv->dev, "Failed to reset VLAN table\n");
		return ret;
	}

	dsa_switch_for_each_available_port(dp, ds) {
		/* Start every port with the neutral PVID */
		ret = rtl960x_vlan_port_set_pvid(priv, dp->index, 0);
		if (ret) {
			dev_err(priv->dev, "Failed to reset PVID on port %d\n",
				dp->index);
			return ret;
		}

		/* Disable vlan-filtering for all ports */
		ret = rtl960x_port_vlan_filtering(ds, dp->index, false, NULL);
		if (ret) {
			dev_err(priv->dev,
				"Failed to disable vlan filtering on port %d\n",
				dp->index);
			return ret;
		}
	}

	/* VLAN is always enabled. */
	return regmap_set_bits(priv->map, RTL960X_VLAN_CTRL_REG,
			       RTL960X_VLAN_FILTER_EN_MASK);
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

/**
 * rtl960x_port_fast_age() - flush dynamic FDB entries learned on a port
 * @ds: DSA switch instance
 * @port: port index
 *
 * This function requests the switch to age out dynamic FDB entries learned on
 * @port.
 *
 * Context: Can sleep.
 * Return: Nothing.
 */
static void rtl960x_port_fast_age(struct dsa_switch *ds, int port)
{
	struct rtl960x_dsa *priv = ds->priv;
	int ret;

	ret = rtl960x_l2_flush(priv, port, 0);
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

	/* Start with all ports blocked, including unused ports */
	dsa_switch_for_each_port(dp, ds) {
		/* Set the initial STP state of all ports to DISABLED, otherwise
		 * ports will still forward frames to the CPU despite being
		 * administratively down by default.
		 */
		rtl960x_port_stp_state_set(ds, dp->index, BR_STATE_DISABLED);

		/* Start with all port completely isolated */
		ret = rtl960x_port_set_isolation(priv, dp->index, 0);
		if (ret)
			return ret;

		/* Disable learning */
		ret = rtl960x_port_set_learning(priv, dp->index, false);
		if (ret)
			return ret;

		/* Collect CPU ports. If we support cascade switches, it should
		 * also include the upstream DSA ports.
		 */
		if (dsa_port_is_cpu(dp))
			cpu_ports |= BIT(dp->index);

		/* Every port is a member of the neutral VLAN */
		priv->mbr_mask[0] |= BIT(dp->index);
	}

	/* User ports forward only to the CPU ports */
	dsa_switch_for_each_user_port(dp, ds) {
		ret = rtl960x_port_set_isolation(priv, dp->index, cpu_ports);
		if (ret)
			return ret;

		user_ports |= BIT(dp->index);
	}

	/* CPU ports forward to every user port */
	dsa_switch_for_each_cpu_port(dp, ds) {
		ret = rtl960x_cpu_port_setup(priv, dp->index);
		if (ret)
			return ret;

		ret = rtl960x_port_set_isolation(priv, dp->index, user_ports);
		if (ret)
			return ret;
	}

	ds->max_num_bridges = 1;

	ds->assisted_learning_on_cpu_port = true;

	ret = rtl960x_port_mib_reset(priv, user_ports | cpu_ports);
	if (ret)
		return ret;

	/* Setup VLAN */
	return rtl960x_vlan_setup(ds);
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
	 * Add this port to the isolation group of every other port
	 * offloading this bridge.
	 */
	dsa_switch_for_each_bridge_member(dp, ds, bridge.dev) {
		/* Handle this port after */
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

	ret = rtl960x_port_set_learning(priv, port, true);
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
		/* Handle this port after */
		if (dp->index == port)
			continue;

		ret = rtl960x_port_remove_isolation(priv, dp->index, BIT(port));
		if (ret)
			dev_err(priv->dev,
				"failed to remove port %d from isolation group of port %d: %pe\n",
				port, dp->index, ERR_PTR(ret));

		mask |= BIT(dp->index);
	}

	ret = rtl960x_port_set_learning(priv, port, false);
	if (ret)
		dev_err(priv->dev,
			"failed to disable learning on port %d: %pe\n",
			port, ERR_PTR(ret));

	/* Remove those ports from the isolation group of this port */
	ret = rtl960x_port_remove_isolation(priv, port, mask);
	if (ret)
		dev_err(priv->dev,
			"failed to remove isolation group of port %d: %pe\n",
			port, ERR_PTR(ret));
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
	.port_mdb_add		= rtl960x_port_mdb_add,
	.port_mdb_del		= rtl960x_port_mdb_del,
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

	ret = devm_mutex_init(dev, &priv->vlan_lock);
	if (ret)
		return ret;

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
