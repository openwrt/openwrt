/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __RTL_OTTO_STATS_H
#define __RTL_OTTO_STATS_H

#include <linux/ethtool.h>
#include <linux/jiffies.h>
#include <linux/ktime.h>
#include <linux/netdevice.h>
#include <linux/spinlock.h>
#include <linux/types.h>

/* Packet statistics */
#define RTL838X_STAT_PORT_STD_MIB		(0x1200)
#define RTL839X_STAT_PORT_STD_MIB		(0xC000)
#define RTL930X_STAT_PORT_MIB_CNTR		(0x0664)
#define RTL930X_STAT_PORT_PRVTE_CNTR		(0x2364)
#define RTL838X_STAT_RST			(0x3100)
#define RTL839X_STAT_RST			(0xF504)
#define RTL930X_STAT_RST			(0x3240)
#define RTL931X_STAT_RST			(0x7ef4)
#define RTL838X_STAT_PORT_RST			(0x3104)
#define RTL839X_STAT_PORT_RST			(0xF508)
#define RTL930X_STAT_PORT_RST			(0x3244)
#define RTL931X_STAT_PORT_RST			(0x7ef8)
#define RTL838X_STAT_CTRL			(0x3108)
#define RTL839X_STAT_CTRL			(0x04cc)
#define RTL930X_STAT_CTRL			(0x3248)
#define RTL931X_STAT_CTRL			(0x5720)
#define RTL930X_STAT_PRVTE_DROP_COUNTER0	(0xB5B8)

/* This interval needs to be short enough to prevent an undetected counter
 * overflow. The octet counters don't need to be considered for this, because
 * they are 64 bits on all platforms. Based on the possible packets per second
 * at the highest supported speeds, an interval of a minute is probably a safe
 * choice for the other counters.
 */
#define RTLDSA_COUNTERS_POLL_INTERVAL	(60 * HZ)

/* Some SoC families require table access to get the HW counters. A mutex is
 * required for this access - which will potentially cause a sleep in the
 * current context. This is not always possible with .get_stats64 because it
 * is also called in atomic contexts.
 *
 * For these SoCs, the retrieval of the current counters in .get_stats64 is
 * skipped and the counters are simply retrieved a lot more often from the HW.
 */
#define RTLDSA_COUNTERS_FAST_POLL_INTERVAL	(3 * HZ)

struct rtldsa_counter {
	u64 val;
	u32 last;
};

struct rtldsa_counter_state {
	/**
	 * @lock: protect updates to members of the structure when the
	 * priv->counters_lock is not used. (see rtl931x_reg->stat_update_counters_atomically)
	 */
	spinlock_t lock;
	ktime_t last_update;

	struct rtldsa_counter symbol_errors;

	struct rtldsa_counter if_in_octets;
	struct rtldsa_counter if_out_octets;
	struct rtldsa_counter if_in_ucast_pkts;
	struct rtldsa_counter if_in_mcast_pkts;
	struct rtldsa_counter if_in_bcast_pkts;
	struct rtldsa_counter if_out_ucast_pkts;
	struct rtldsa_counter if_out_mcast_pkts;
	struct rtldsa_counter if_out_bcast_pkts;
	struct rtldsa_counter if_out_discards;
	struct rtldsa_counter single_collisions;
	struct rtldsa_counter multiple_collisions;
	struct rtldsa_counter deferred_transmissions;
	struct rtldsa_counter late_collisions;
	struct rtldsa_counter excessive_collisions;
	struct rtldsa_counter crc_align_errors;
	struct rtldsa_counter rx_pkts_over_max_octets;

	struct rtldsa_counter unsupported_opcodes;

	struct rtldsa_counter rx_undersize_pkts;
	struct rtldsa_counter rx_oversize_pkts;
	struct rtldsa_counter rx_fragments;
	struct rtldsa_counter rx_jabbers;

	struct rtldsa_counter tx_pkts[ETHTOOL_RMON_HIST_MAX];
	struct rtldsa_counter rx_pkts[ETHTOOL_RMON_HIST_MAX];

	struct rtldsa_counter drop_events;
	struct rtldsa_counter collisions;

	struct rtldsa_counter rx_pause_frames;
	struct rtldsa_counter tx_pause_frames;

	/** @link_stat_lock: Protect link_stat */
	spinlock_t link_stat_lock;

	/** @link_stat: Prepared return data for .get_stats64 which can be accessed without mutex */
	struct rtnl_link_stats64 link_stat;
};

enum mib_reg {
	MIB_REG_INVALID = 0,
	MIB_REG_STD,
	MIB_REG_PRV,
	MIB_TBL_STD,
	MIB_TBL_PRV,
};

#define MIB_ITEM(_reg, _offset, _size) \
		{.reg = _reg, .offset = _offset, .size = _size}

#define MIB_LIST_ITEM(_name, _item) \
		{.name = _name, .item = _item}

struct rtldsa_mib_item {
	enum mib_reg reg;
	unsigned int offset;
	unsigned int size;
};

struct rtldsa_mib_list_item {
	const char *name;
	struct rtldsa_mib_item item;
};

struct rtldsa_mib_desc {
	struct rtldsa_mib_item symbol_errors;

	struct rtldsa_mib_item if_in_octets;
	struct rtldsa_mib_item if_out_octets;
	struct rtldsa_mib_item if_in_ucast_pkts;
	struct rtldsa_mib_item if_in_mcast_pkts;
	struct rtldsa_mib_item if_in_bcast_pkts;
	struct rtldsa_mib_item if_out_ucast_pkts;
	struct rtldsa_mib_item if_out_mcast_pkts;
	struct rtldsa_mib_item if_out_bcast_pkts;
	struct rtldsa_mib_item if_out_discards;
	struct rtldsa_mib_item single_collisions;
	struct rtldsa_mib_item multiple_collisions;
	struct rtldsa_mib_item deferred_transmissions;
	struct rtldsa_mib_item late_collisions;
	struct rtldsa_mib_item excessive_collisions;
	struct rtldsa_mib_item crc_align_errors;
	struct rtldsa_mib_item rx_pkts_over_max_octets;

	struct rtldsa_mib_item unsupported_opcodes;

	struct rtldsa_mib_item rx_undersize_pkts;
	struct rtldsa_mib_item rx_oversize_pkts;
	struct rtldsa_mib_item rx_fragments;
	struct rtldsa_mib_item rx_jabbers;

	struct rtldsa_mib_item tx_pkts[ETHTOOL_RMON_HIST_MAX];
	struct rtldsa_mib_item rx_pkts[ETHTOOL_RMON_HIST_MAX];
	struct ethtool_rmon_hist_range rmon_ranges[ETHTOOL_RMON_HIST_MAX];

	struct rtldsa_mib_item drop_events;
	struct rtldsa_mib_item collisions;

	struct rtldsa_mib_item rx_pause_frames;
	struct rtldsa_mib_item tx_pause_frames;

	size_t list_count;
	const struct rtldsa_mib_list_item *list;
};

struct dsa_switch;
struct ethtool_eth_ctrl_stats;
struct ethtool_eth_mac_stats;
struct ethtool_eth_phy_stats;
struct ethtool_pause_stats;
struct ethtool_rmon_hist_range;
struct ethtool_rmon_stats;
struct rtl838x_switch_priv;
struct rtnl_link_stats64;

void rtldsa_stats_init(struct rtl838x_switch_priv *priv);
void rtldsa_stats_init_counters(struct rtl838x_switch_priv *priv);

void rtldsa_get_strings(struct dsa_switch *ds, int port, u32 stringset,
			u8 *data);
void rtldsa_get_ethtool_stats(struct dsa_switch *ds, int port, u64 *data);
int rtldsa_get_sset_count(struct dsa_switch *ds, int port, int sset);
void rtldsa_get_eth_phy_stats(struct dsa_switch *ds, int port,
			      struct ethtool_eth_phy_stats *phy_stats);
void rtldsa_get_eth_mac_stats(struct dsa_switch *ds, int port,
			      struct ethtool_eth_mac_stats *mac_stats);
void rtldsa_get_eth_ctrl_stats(struct dsa_switch *ds, int port,
			       struct ethtool_eth_ctrl_stats *ctrl_stats);
void rtldsa_get_rmon_stats(struct dsa_switch *ds, int port,
			   struct ethtool_rmon_stats *rmon_stats,
			   const struct ethtool_rmon_hist_range **ranges);
void rtldsa_get_stats64(struct dsa_switch *ds, int port,
			struct rtnl_link_stats64 *s);
void rtldsa_get_pause_stats(struct dsa_switch *ds, int port,
			    struct ethtool_pause_stats *pause_stats);

void rtldsa_counters_lock_register(struct rtl838x_switch_priv *priv, int port)
	__acquires(&priv->ports[port].counters.lock);
void rtldsa_counters_unlock_register(struct rtl838x_switch_priv *priv, int port)
	__releases(&priv->ports[port].counters.lock);
void rtldsa_counters_lock_table(struct rtl838x_switch_priv *priv, int port)
	__acquires(&priv->counters_lock);
void rtldsa_counters_unlock_table(struct rtl838x_switch_priv *priv, int port)
	__releases(&priv->ports[port].counters.lock);
void rtldsa_update_counters_atomically(struct rtl838x_switch_priv *priv,
				       int port);

#endif /* __RTL_OTTO_STATS_H */
