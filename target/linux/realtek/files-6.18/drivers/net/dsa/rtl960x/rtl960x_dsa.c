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

/* Per port MIB statistics */
#define RTL960X_STAT_PORT_TX_MIB	(0x32000)
#define RTL960X_STAT_PORT_RX_MIB	(0x32600)
#define   RTL960X_STAT_PORT_OFFSET(p)	(128 * p)
#define RTL960X_STAT_PORT_OAM_MIB	(0x32c00)

#define RTL960X_STAT_CTRL		0x34000
#define RTL960X_STAT_PORT_RESET		0x34014
#define RTL960X_STAT_RESET_CONFIG	0x3401c

/* The DSA callback .get_stats64 runs in atomic context, so we are not allowed
 * to block. On the other hand, accessing MIB counters absolutely requires us to
 * block. The solution is thus to schedule work which polls the MIB counters
 * asynchronously and updates some private data, which the callback can then
 * fetch atomically. Three seconds should be a good enough polling interval.
 */
#define RTL960X_STATS_INTERVAL_JIFFIES	(3 * HZ)

enum rtl960x_mib_counter_index {
	/* TX */
	RTL960X_MIB_etherStatsTxMulticastPkts,
	RTL960X_MIB_etherStatsTxBroadcastPkts,
	RTL960X_MIB_etherStatsTxUndersizePkts,
	RTL960X_MIB_etherStatsTxOversizePkts,
	RTL960X_MIB_etherStatsTxPkts64Octets,
	RTL960X_MIB_etherStatsTxPkts65to127Octets,
	RTL960X_MIB_etherStatsTxPkts128to255Octets,
	RTL960X_MIB_etherStatsTxPkts256to511Octets,
	RTL960X_MIB_etherStatsTxPkts512to1023Octets,
	RTL960X_MIB_etherStatsTxPkts1024to1518Octets,
	RTL960X_MIB_ifOutOctets,
	RTL960X_MIB_dot3StatsSingleCollisionFrames,
	RTL960X_MIB_dot3StatsMultipleCollisionFrames,
	RTL960X_MIB_dot3StatsDeferredTransmissions,
	RTL960X_MIB_dot3StatsLateCollisions,
	RTL960X_MIB_etherStatsCollisions,
	RTL960X_MIB_dot3StatsExcessiveCollisions,
	RTL960X_MIB_dot3OutPauseFrames,
	RTL960X_MIB_ifOutDiscards,
	RTL960X_MIB_etherStatsTxPkts1519toMaxOctets,
	RTL960X_MIB_dot1dTpPortInDiscards,
	RTL960X_MIB_ifOutUcastPkts,
	RTL960X_MIB_ifOutMulticastPkts,
	RTL960X_MIB_ifOutBrocastPkts,
	/* RX */
	RTL960X_MIB_ifInOctets,
	RTL960X_MIB_etherStatsCRCAlignErrors,
	RTL960X_MIB_dot3StatsSymbolErrors,
	RTL960X_MIB_dot3InPauseFrames,
	RTL960X_MIB_dot3ControlInUnknownOpcodes,
	RTL960X_MIB_etherStatsFragments,
	RTL960X_MIB_etherStatsJabbers,
	RTL960X_MIB_ifInUcastPkts,
	RTL960X_MIB_etherStatsDropEvents,
	RTL960X_MIB_ifInMulticastPkts,
	RTL960X_MIB_ifInBroadcastPkts,
	RTL960X_MIB_etherStatsRxPkts1519toMaxOctets,
	RTL960X_MIB_etherStatsRxUndersizePkts,
	RTL960X_MIB_etherStatsRxOversizePkts,
	RTL960X_MIB_etherStatsRxPkts64Octets;
	RTL960X_MIB_etherStatsRxPkts65to127Octets;
	RTL960X_MIB_etherStatsRxPkts128to255Octets;
	RTL960X_MIB_etherStatsRxPkts256to511Octets;
	RTL960X_MIB_etherStatsRxPkts512to1023Octets;
	RTL960X_MIB_etherStatsRxPkts1024to1518Octets;
	/* OAM */
	RTL960X_MIB_outOampduPkts,
	RTL960X_MIB_inOampduPkts,
	RTL960X_MIB_END,
};

struct rtl960x_mib_counter {
	u32 reg;
	u32 offset;
	u32 length;
	const char *name;
};

#define RTL960X_MAKE_TX_MIB_COUNTER(_offset, _length, _name) \
		[RTL960X_MIB_ ## _name] = {RTL960X_STAT_PORT_TX_MIB, _offset, \
					   _length, #_name }
#define RTL960X_MAKE_RX_MIB_COUNTER(_offset, _length, _name) \
		[RTL960X_MIB_ ## _name] = {RTL960X_STAT_PORT_RX_MIB, _offset, \
					   _length, #_name }
#define RTL960X_MAKE_OAM_MIB_COUNTER(_offset, _length, _name) \
		[RTL960X_MIB_ ## _name] = {RTL960X_STAT_PORT_OAM_MIB, _offset, \
					   _length, #_name }

static struct rtl960x_mib_counter rtl960x_mib_counters[] = {
	RTL960X_MAKE_TX_MIB_COUNTER(124, 1, etherStatsTxMulticastPkts),
	RTL960X_MAKE_TX_MIB_COUNTER(120, 1, etherStatsTxBroadcastPkts),
	RTL960X_MAKE_TX_MIB_COUNTER(116, 1, etherStatsTxUndersizePkts),
	RTL960X_MAKE_TX_MIB_COUNTER(112, 1, etherStatsTxOversizePkts),
	RTL960X_MAKE_TX_MIB_COUNTER(108, 1, etherStatsTxPkts64Octets),
	RTL960X_MAKE_TX_MIB_COUNTER(104, 1, etherStatsTxPkts65to127Octets),
	RTL960X_MAKE_TX_MIB_COUNTER(100, 1, etherStatsTxPkts128to255Octets),
	RTL960X_MAKE_TX_MIB_COUNTER(96, 1, etherStatsTxPkts256to511Octets),
	RTL960X_MAKE_TX_MIB_COUNTER(92, 1, etherStatsTxPkts512to1023Octets),
	RTL960X_MAKE_TX_MIB_COUNTER(88, 1, etherStatsTxPkts1024to1518Octets),
	RTL960X_MAKE_TX_MIB_COUNTER(84, 2, ifOutOctets),
	RTL960X_MAKE_TX_MIB_COUNTER(76, 1, dot3StatsSingleCollisionFrames),
	RTL960X_MAKE_TX_MIB_COUNTER(72, 1, dot3StatsMultipleCollisionFrames),
	RTL960X_MAKE_TX_MIB_COUNTER(68, 1, dot3StatsDeferredTransmissions),
	RTL960X_MAKE_TX_MIB_COUNTER(64, 1, dot3StatsLateCollisions),
	RTL960X_MAKE_TX_MIB_COUNTER(60, 1, etherStatsCollisions),
	RTL960X_MAKE_TX_MIB_COUNTER(56, 1, dot3StatsExcessiveCollisions),
	RTL960X_MAKE_TX_MIB_COUNTER(52, 1, dot3OutPauseFrames),
	RTL960X_MAKE_TX_MIB_COUNTER(48, 1, ifOutDiscards),
	RTL960X_MAKE_TX_MIB_COUNTER(44, 1, etherStatsTxPkts1519toMaxOctets),
	RTL960X_MAKE_TX_MIB_COUNTER(36, 1, dot1dTpPortInDiscards),
	RTL960X_MAKE_TX_MIB_COUNTER(32, 1, ifOutUcastPkts),
	RTL960X_MAKE_TX_MIB_COUNTER(28, 1, ifOutMulticastPkts),
	RTL960X_MAKE_TX_MIB_COUNTER(24, 1, ifOutBroadcastPkts),

	RTL960X_MAKE_RX_MIB_COUNTER(124, 2, ifInOctets),
	RTL960X_MAKE_RX_MIB_COUNTER(116, 1, etherStatsCRCAlignErrors),
	RTL960X_MAKE_RX_MIB_COUNTER(112, 1, dot3StatsSymbolErrors),
	RTL960X_MAKE_RX_MIB_COUNTER(108, 1, dot3InPauseFrames),
	RTL960X_MAKE_RX_MIB_COUNTER(104, 1, dot3ControlInUnknownOpcodes),
	RTL960X_MAKE_RX_MIB_COUNTER(100, 1, etherStatsFragments),
	RTL960X_MAKE_RX_MIB_COUNTER(96, 1, etherStatsJabbers),
	RTL960X_MAKE_RX_MIB_COUNTER(92, 1, ifInUcastPkts),
	RTL960X_MAKE_RX_MIB_COUNTER(88, 1, etherStatsDropEvents),
	RTL960X_MAKE_RX_MIB_COUNTER(84, 1, ifInMulticastPkts),
	RTL960X_MAKE_RX_MIB_COUNTER(80, 2, ifInBroadcastPkts),
	RTL960X_MAKE_RX_MIB_COUNTER(76, 1, etherStatsRxPkts1519toMaxOctets),
	RTL960X_MAKE_RX_MIB_COUNTER(68, 1, etherStatsRxUndersizePkts),
	RTL960X_MAKE_RX_MIB_COUNTER(64, 1, etherStatsRxOversizePkts),
	RTL960X_MAKE_RX_MIB_COUNTER(60, 1, etherStatsRxPkts64Octets),
	RTL960X_MAKE_RX_MIB_COUNTER(56, 1, etherStatsRxPkts65to127Octets),
	RTL960X_MAKE_RX_MIB_COUNTER(52, 1, etherStatsRxPkts128to255Octets),
	RTL960X_MAKE_RX_MIB_COUNTER(48, 1, etherStatsRxPkts256to511Octets),
	RTL960X_MAKE_RX_MIB_COUNTER(44, 1, etherStatsRxPkts512to1023Octets),
	RTL960X_MAKE_RX_MIB_COUNTER(40, 1, etherStatsRxPkts1024to1518Octets),

	RTL960X_MAKE_OAM_MIB_COUNTER(4, 1, outOampduPkts),
	RTL960X_MAKE_OAM_MIB_COUNTER(0, 1, inOampduPkts),
};

static_assert(ARRAY_SIZE(rtl960x_mib_counters) == RTL960X_MIB_END);

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


static int rtl960x_mib_counter_read(struct rtl960x_dsa *priv, int port, u32 reg,
				    u32 offset, u32 length, u64 *mibvalue)
{
	u64 tmpvalue = 0;
	u32 val;
	int ret, i;

	/* Read the MIB counter 32 bits at a time */
	for(i = 0, i < length; i++) {
		ret = regmap_read(priv->map,
				  reg + offset + RTL960X_STAT_PORT_OFFSET(port) - i * 4, &val);
		if (ret)
			return ret;

		tmpvalue = tmpvalue << 32 | val;
	}

	/* Only commit the result if no error occurred */
	*mibvalue = tmpvalue;

	return 0;
}

static void rtl960x_get_ethtool_stats(struct dsa_switch *ds, int port, u64 *data)
{
	struct rtl960x_dsa *priv = ds->priv;
	int ret, i;

	mutex_lock(&priv->mib_lock);
	for (i = 0; i < RTL960X_MIB_END; i++) {
		struct rtl960x_mib_counter *mib = &rtl960x_mib_counters[i];

		ret = rtl960x_mib_counter_read(priv, port, mib->reg, mib->offset,
					       mib->length, &data[i]);

		if (ret) {
			dev_err(priv->dev,
				"failed to read port %d counters: %pe\n", port,
				ERR_PTR(ret));
			break;
		}
	}
	mutex_unlock(&priv->mib_lock);
}


static void rtl960x_get_strings(struct dsa_switch *ds, int port, u32 stringset, u8 *data)
{
	int i;

	if (stringset != ETH_SS_STATS)
		return;

	for (i = 0; i < RTL960X_MIB_END; i++) {
		struct rtl960x_mib_counter *mib = &rtl960x_mib_counters[i];
		ethtool_puts(&data, mib->name);
	}
}

static int rtl960x_get_sset_count(struct dsa_switch *ds, int port, int sset)
{
	if (sset != ETH_SS_STATS)
		return -EOPNOTSUPP;

	return RTL960X_MIB_END;
}

static void rtl960x_get_phy_stats(struct dsa_switch *ds, int port,
				  struct ethtool_eth_phy_stats *phy_stats)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_mib_counter *mib;

	mib = &rtl960x_mib_counters[RTL960X_MIB_dot3StatsSymbolErrors];

	mutex_lock(&priv->mib_lock);
	rtl960x_mib_counter_read(priv, port, mib->offset, mib->length,
				 &phy_stats->SymbolErrorDuringCarrier);
	mutex_unlock(&priv->mib_lock);
}

static void rtl960x_get_mac_stats(struct dsa_switch *ds, int port,
				  struct ethtool_eth_mac_stats *mac_stats)
{
	u64 cnt[RTL960X_MIB_END] = {
		[RTL960X_MIB_ifOutOctets] = 1,
		[RTL960X_MIB_ifOutUcastPkts] = 1,
		[RTL960X_MIB_ifOutMulticastPkts] = 1,
		[RTL960X_MIB_ifOutBroadcastPkts] = 1,
		[RTL960X_MIB_dot3OutPauseFrames] = 1,
		[RTL960X_MIB_ifOutDiscards] = 1,
		[RTL960X_MIB_ifInOctets] = 1,
		[RTL960X_MIB_ifInUcastPkts] = 1,
		[RTL960X_MIB_ifInMulticastPkts] = 1,
		[RTL960X_MIB_ifInBroadcastPkts] = 1,
		[RTL960X_MIB_dot3InPauseFrames] = 1,
		[RTL960X_MIB_dot3StatsSingleCollisionFrames] = 1,
		[RTL960X_MIB_dot3StatsMultipleCollisionFrames] = 1,
		[RTL960X_MIB_etherStatsCRCAlignErrors] = 1,
		[RTL960X_MIB_dot3StatsDeferredTransmissions] = 1,
		[RTL960X_MIB_dot3StatsLateCollisions] = 1,
		[RTL960X_MIB_dot3StatsExcessiveCollisions] = 1,

	};
	struct realtek_priv *priv = ds->priv;
	struct rtl960x *mb;
	int ret;
	int i;

	mutex_lock(&priv->mib_lock);
	for (i = 0; i < RTL960X_MIB_END; i++) {
		struct rtl960x_mib_counter *mib = &rtl960x_mib_counters[i];

		/* Only fetch required MIB counters (marked = 1 above) */
		if (!cnt[i])
			continue;

		ret = rtl960x_mib_counter_read(priv, port, mib->offset,
					       mib->length, &cnt[i]);
		if (ret)
			break;
	}
	mutex_unlock(&mb->mib_lock);

	/* The RTL8365MB-VC exposes MIB objects, which we have to translate into
	 * IEEE 802.3 Managed Objects. This is not always completely faithful,
	 * but we try out best. See RFC 3635 for a detailed treatment of the
	 * subject.
	 */

	mac_stats->FramesTransmittedOK = cnt[RTL960X_MIB_ifOutUcastPkts] +
	cnt[RTL960X_MIB_ifOutMulticastPkts] + cnt[RTL960X_MIB_ifOutBroadcastPkts] +
	cnt[RTL960X_MIB_dot3OutPauseFrames] - cnt[RTL960X_MIB_ifOutDiscards];
	mac_stats->SingleCollisionFrames = cnt[RTL960X_MIB_dot3StatsSingleCollisionFrames];
	mac_stats->MultipleCollisionFrames = cnt[RTL960X_MIB_dot3StatsMultipleCollisionFrames];
	mac_stats->FramesReceivedOK = cnt[RTL960X_MIB_ifInUcastPkts] +
	cnt[RTL960X_MIB_ifInMulticastPkts] + cnt[RTL960X_MIB_ifInBroadcastPkts] +
	cnt[RTL960X_MIB_dot3InPauseFrames];
	mac_stats->FrameCheckSequenceErrors = cnt[RTL960X_MIB_etherStatsCRCAlignErrors];
	mac_stats->OctetsTransmittedOK = cnt[RTL960X_MIB_ifOutOctets] -
	18 * mac_stats->FramesTransmittedOK;
	mac_stats->FramesWithDeferredXmissions = cnt[RTL960X_MIB_dot3StatsDeferredTransmissions];
	mac_stats->LateCollisions = cnt[RTL960X_MIB_dot3StatsLateCollisions];
	mac_stats->FramesAbortedDueToXSColls = cnt[RTL960X_MIB_dot3StatsExcessiveCollisions];
	mac_stats->OctetsReceivedOK = cnt[RTL960X_MIB_ifInOctets] -
	18 * mac_stats->FramesReceivedOK;
	mac_stats->MulticastFramesXmittedOK = cnt[RTL960X_MIB_ifOutMulticastPkts];
	mac_stats->BroadcastFramesXmittedOK = cnt[RTL960X_MIB_ifOutBroadcastPkts];
	mac_stats->MulticastFramesReceivedOK = cnt[RTL960X_MIB_ifInMulticastPkts];
	mac_stats->BroadcastFramesReceivedOK = cnt[RTL960X_MIB_ifInBroadcastPkts];
}

static void rtl960x_get_ctrl_stats(struct dsa_switch *ds, int port,
				   struct ethtool_eth_ctrl_stats *ctrl_stats)
{
	struct realtek_priv *priv = ds->priv;
	struct rtl960x_mib_counter *mib;

	mib = &rtl960x_mib_counters[RTL960X_MIB_dot3ControlInUnknownOpcodes];

	mutex_lock(&priv->mib_lock);
	rtl960x_mib_counter_read(priv, port, mib->offset, mib->length,
				   &ctrl_stats->UnsupportedOpcodesReceived);
	mutex_unlock(&priv->mib_lock);
}

static void rtl960x_stats_update(struct realtek_priv *priv, int port)
{
	u64 cnt[RTL960X_MIB_END] = {
		[RTL960X_MIB_ifOutOctets] = 1,
		[RTL960X_MIB_ifOutUcastPkts] = 1,
		[RTL960X_MIB_ifOutMulticastPkts] = 1,
		[RTL960X_MIB_ifOutBroadcastPkts] = 1,
		[RTL960X_MIB_ifOutDiscards] = 1,
		[RTL960X_MIB_ifInOctets] = 1,
		[RTL960X_MIB_ifInUcastPkts] = 1,
		[RTL960X_MIB_ifInMulticastPkts] = 1,
		[RTL960X_MIB_ifInBroadcastPkts] = 1,
		[RTL960X_MIB_etherStatsDropEvents] = 1,
		[RTL960X_MIB_etherStatsCollisions] = 1,
		[RTL960X_MIB_etherStatsFragments] = 1,
		[RTL960X_MIB_etherStatsJabbers] = 1,
		[RTL960X_MIB_etherStatsCRCAlignErrors] = 1,
		[RTL960X_MIB_dot3StatsLateCollisions] = 1,
	};
	struct rtnl_link_stats64 *stats;
	int ret;
	int i;

	stats = &priv->ports[port].stats;

	mutex_lock(&priv->mib_lock);
	for (i = 0; i < RTL960X_MIB_END; i++) {
		struct rtl960x_mib_counter *c = &rtl960x_mib_counters[i];

		/* Only fetch required MIB counters (marked = 1 above) */
		if (!cnt[i])
			continue;

		ret = rtl960x_mib_counter_read(priv, port, c->offset,
					       c->length, &cnt[i]);
		if (ret)
			break;
	}
	mutex_unlock(&priv->mib_lock);

	/* Don't update statistics if there was an error reading the counters */
	if (ret)
		return;

	spin_lock(&priv->ports[port].stats_lock);

	stats->rx_packets = cnt[RTL960X_MIB_ifInUcastPkts] +
			    cnt[RTL960X_MIB_ifInMulticastPkts] +
			    cnt[RTL960X_MIB_ifInBroadcastPkts];

	stats->tx_packets = cnt[RTL960X_MIB_ifOutUcastPkts] +
			    cnt[RTL960X_MIB_ifOutMulticastPkts] +
			    cnt[RTL960X_MIB_ifOutBroadcastPkts];

	/* if{In,Out}Octets includes FCS - remove it */
	stats->rx_bytes = cnt[RTL960X_MIB_ifInOctets] - 4 * stats->rx_packets;
	stats->tx_bytes = cnt[RTL960X_MIB_ifOutOctets] - 4 * stats->tx_packets;

	stats->rx_dropped = cnt[RTL960X_MIB_etherStatsDropEvents];
	stats->tx_dropped = cnt[RTL960X_MIB_ifOutDiscards];

	stats->multicast = cnt[RTL960X_MIB_ifInMulticastPkts];
	stats->collisions = cnt[RTL960X_MIB_etherStatsCollisions];

	stats->rx_length_errors = cnt[RTL960X_MIB_etherStatsFragments] +
				  cnt[RTL960X_MIB_etherStatsJabbers];
	stats->rx_crc_errors = cnt[RTL960X_MIB_etherStatsCRCAlignErrors];
	stats->rx_errors = stats->rx_length_errors + stats->rx_crc_errors;

	stats->tx_aborted_errors = cnt[RTL960X_MIB_ifOutDiscards];
	stats->tx_window_errors = cnt[RTL960X_MIB_dot3StatsLateCollisions];
	stats->tx_errors = stats->tx_aborted_errors + stats->tx_window_errors;

	spin_unlock(&mb->ports[port].stats_lock);
}

static void rtl960x_stats_poll(struct work_struct *work)
{
	struct rtl960x_port *p = container_of(to_delayed_work(work),
					      struct rtl960x_port,
				       mib_work);
	struct realtek_priv *priv = p->priv;

	rtl960x_stats_update(priv, p->index);

	schedule_delayed_work(&p->mib_work, RTL960X_STATS_INTERVAL_JIFFIES);
}

static void rtl960x_get_stats64(struct dsa_switch *ds, int port,
				struct rtnl_link_stats64 *s)
{
	struct rtl960x_dsa *priv = ds->priv;
	struct rtl960x_port *p;

	p = &priv->ports[port];

	spin_lock(&p->stats_lock);
	memcpy(s, &p->stats, sizeof(*s));
	spin_unlock(&p->stats_lock);
}

static int rtl960x_stats_setup(struct rtl960x_dsa *priv)
{
	struct dsa_switch *ds = &priv->ds;
	struct dsa_port *dp;
	int ret;

	/* Per-chip global mutex to protect MIB counter access, since doing
	 * so requires accessing a series of registers in a particular order.
	 */
	ret = devm_mutex_init(priv->dev, &priv->mib_lock);
	if (ret)
		return ret;

	dsa_switch_for_each_available_port(dp, ds) {
		struct rtl960x_port *p = &priv->ports[dp->index];

		/* Per-port spinlock to protect the stats64 data */
		spin_lock_init(&p->stats_lock);

		/* This work polls the MIB counters and keeps the stats64 data
		 * up-to-date.
		 */
		INIT_DELAYED_WORK(&p->mib_work, rtl960x_stats_poll);
	}

	return 0;
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
		struct rtl8365mb_port *p = &priv->ports[dp->index];

		ret = rtl960x_port_set_isolation(priv, dp->index, 0);
		if (ret)
			return ret;

		ret = rtl960x_port_set_learning(priv, dp->index, false);
		if (ret)
			return ret;

		/* Set up per-port private data */
		p->priv = priv;
		p->index = dp->index;

		if (dsa_port_is_cpu(dp))
			cpu_ports |= BIT(dp->index);
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

	/* One reserved VID per offloaded bridge; DSA numbers them for us. */
	ds->max_num_bridges = RTL960X_NUM_BRIDGE_VIDS;

	ds->assisted_learning_on_cpu_port = true;

	/* Every user port starts standalone (see rtl960x_vlan_setup). */
	ret = rtl960x_vlan_setup(ds);
	if (ret)
		return ret;

	/* Start statistics counter polling */
	ret = rtl960x_stats_setup(priv);
	if (ret)
		dev_err(priv->dev, "failed to setup stats: %pe\n", ERR_PTR(ret));

	return ret;
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
	.get_strings = rtl960x_get_stings,
	.get_ethtool_stats = rtl960x_get_ethtool_stats,
	.get_sset_count = rtl960x_get_sset_count,
	.get_eth_phy_stats = rtl960x_get_phy_stats,
	.get_eth_mac_stats = rtl960x_get_mac_stats,
	.get_eth_ctrl_stats = rtl960x_get_ctrl_stats,
	.get_stats64 = rtl960x_get_stats64,
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
