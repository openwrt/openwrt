/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OTTO_STATS_H
#define _OTTO_STATS_H

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
#define OTTO_STATS_COUNTERS_POLL_INTERVAL	(60 * HZ)

/* Some SoC families require table access to get the HW counters. A mutex is
 * required for this access - which will potentially cause a sleep in the
 * current context. This is not always possible with .get_stats64 because it
 * is also called in atomic contexts.
 *
 * For these SoCs, the retrieval of the current counters in .get_stats64 is
 * skipped and the counters are simply retrieved a lot more often from the HW.
 */
#define OTTO_STATS_COUNTERS_FAST_POLL_INTERVAL	(3 * HZ)

struct otto_stats_counter {
	u64 val;
	u32 last;
};

struct otto_stats_counter_state {
	/**
	 * @lock: protect updates to members of the structure when the
	 * priv->counters_lock is not used. (see rtl931x_reg->stat_update_counters_atomically)
	 */
	spinlock_t lock;
	ktime_t last_update;

	struct otto_stats_counter symbol_errors;

	struct otto_stats_counter if_in_octets;
	struct otto_stats_counter if_out_octets;
	struct otto_stats_counter if_in_ucast_pkts;
	struct otto_stats_counter if_in_mcast_pkts;
	struct otto_stats_counter if_in_bcast_pkts;
	struct otto_stats_counter if_out_ucast_pkts;
	struct otto_stats_counter if_out_mcast_pkts;
	struct otto_stats_counter if_out_bcast_pkts;
	struct otto_stats_counter if_out_discards;
	struct otto_stats_counter single_collisions;
	struct otto_stats_counter multiple_collisions;
	struct otto_stats_counter deferred_transmissions;
	struct otto_stats_counter late_collisions;
	struct otto_stats_counter excessive_collisions;
	struct otto_stats_counter crc_align_errors;
	struct otto_stats_counter rx_pkts_over_max_octets;

	struct otto_stats_counter unsupported_opcodes;

	struct otto_stats_counter rx_undersize_pkts;
	struct otto_stats_counter rx_oversize_pkts;
	struct otto_stats_counter rx_fragments;
	struct otto_stats_counter rx_jabbers;

	struct otto_stats_counter tx_pkts[ETHTOOL_RMON_HIST_MAX];
	struct otto_stats_counter rx_pkts[ETHTOOL_RMON_HIST_MAX];

	struct otto_stats_counter drop_events;
	struct otto_stats_counter collisions;

	struct otto_stats_counter rx_pause_frames;
	struct otto_stats_counter tx_pause_frames;

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

struct otto_stats_mib_item {
	enum mib_reg reg;
	unsigned int offset;
	unsigned int size;
};

struct otto_stats_mib_list_item {
	const char *name;
	struct otto_stats_mib_item item;
};

struct otto_stats_mib_desc {
	struct otto_stats_mib_item symbol_errors;

	struct otto_stats_mib_item if_in_octets;
	struct otto_stats_mib_item if_out_octets;
	struct otto_stats_mib_item if_in_ucast_pkts;
	struct otto_stats_mib_item if_in_mcast_pkts;
	struct otto_stats_mib_item if_in_bcast_pkts;
	struct otto_stats_mib_item if_out_ucast_pkts;
	struct otto_stats_mib_item if_out_mcast_pkts;
	struct otto_stats_mib_item if_out_bcast_pkts;
	struct otto_stats_mib_item if_out_discards;
	struct otto_stats_mib_item single_collisions;
	struct otto_stats_mib_item multiple_collisions;
	struct otto_stats_mib_item deferred_transmissions;
	struct otto_stats_mib_item late_collisions;
	struct otto_stats_mib_item excessive_collisions;
	struct otto_stats_mib_item crc_align_errors;
	struct otto_stats_mib_item rx_pkts_over_max_octets;

	struct otto_stats_mib_item unsupported_opcodes;

	struct otto_stats_mib_item rx_undersize_pkts;
	struct otto_stats_mib_item rx_oversize_pkts;
	struct otto_stats_mib_item rx_fragments;
	struct otto_stats_mib_item rx_jabbers;

	struct otto_stats_mib_item tx_pkts[ETHTOOL_RMON_HIST_MAX];
	struct otto_stats_mib_item rx_pkts[ETHTOOL_RMON_HIST_MAX];
	struct ethtool_rmon_hist_range rmon_ranges[ETHTOOL_RMON_HIST_MAX];

	struct otto_stats_mib_item drop_events;
	struct otto_stats_mib_item collisions;

	struct otto_stats_mib_item rx_pause_frames;
	struct otto_stats_mib_item tx_pause_frames;

	size_t list_count;
	const struct otto_stats_mib_list_item *list;
};

extern const struct otto_stats_mib_desc otto_stats_838x_mib_desc;
extern const struct otto_stats_mib_desc otto_stats_839x_mib_desc;
extern const struct otto_stats_mib_desc otto_stats_930x_mib_desc;
extern const struct otto_stats_mib_desc otto_stats_931x_mib_desc;

struct dsa_switch;
struct ethtool_eth_ctrl_stats;
struct ethtool_eth_mac_stats;
struct ethtool_eth_phy_stats;
struct ethtool_pause_stats;
struct ethtool_rmon_hist_range;
struct ethtool_rmon_stats;
struct otto_dsa_priv;
struct rtnl_link_stats64;

void otto_stats_init(struct otto_dsa_priv *priv);
void otto_stats_init_counters(struct otto_dsa_priv *priv);
void otto_stats_838x_init(struct otto_dsa_priv *priv);
u64 otto_stats_931x_port_table_read(int port, unsigned int mib_size,
				    unsigned int mib_offset, bool is_pvt);

int otto_stats_packet_cntr_alloc(struct otto_dsa_priv *priv);
void otto_stats_packet_cntr_free(struct otto_dsa_priv *priv, int idx);
u32 otto_stats_838x_packet_cntr_read(struct otto_dsa_priv *priv, int counter);
void otto_stats_838x_packet_cntr_clear(struct otto_dsa_priv *priv, int counter);
u32 otto_stats_839x_packet_cntr_read(struct otto_dsa_priv *priv, int counter);
void otto_stats_839x_packet_cntr_clear(struct otto_dsa_priv *priv, int counter);
u32 otto_stats_930x_packet_cntr_read(struct otto_dsa_priv *priv, int counter);
void otto_stats_930x_packet_cntr_clear(struct otto_dsa_priv *priv, int counter);

void otto_stats_get_strings(struct dsa_switch *ds, int port, u32 stringset,
			    u8 *data);
void otto_stats_get_ethtool(struct dsa_switch *ds, int port, u64 *data);
int otto_stats_get_sset_count(struct dsa_switch *ds, int port, int sset);
void otto_stats_get_eth_phy(struct dsa_switch *ds, int port,
			    struct ethtool_eth_phy_stats *phy_stats);
void otto_stats_get_eth_mac(struct dsa_switch *ds, int port,
			    struct ethtool_eth_mac_stats *mac_stats);
void otto_stats_get_eth_ctrl(struct dsa_switch *ds, int port,
			     struct ethtool_eth_ctrl_stats *ctrl_stats);
void otto_stats_get_rmon(struct dsa_switch *ds, int port,
			 struct ethtool_rmon_stats *rmon_stats,
			 const struct ethtool_rmon_hist_range **ranges);
void otto_stats_get64(struct dsa_switch *ds, int port,
		      struct rtnl_link_stats64 *s);
void otto_stats_get_pause(struct dsa_switch *ds, int port,
			  struct ethtool_pause_stats *pause_stats);

void otto_stats_counters_lock_register(struct otto_dsa_priv *priv, int port)
	__acquires(&priv->ports[port].counters.lock);
void otto_stats_counters_unlock_register(struct otto_dsa_priv *priv, int port)
	__releases(&priv->ports[port].counters.lock);
void otto_stats_counters_lock_table(struct otto_dsa_priv *priv, int port)
	__acquires(&priv->counters_lock);
void otto_stats_counters_unlock_table(struct otto_dsa_priv *priv, int port)
	__releases(&priv->ports[port].counters.lock);
void otto_stats_update_counters_atomically(struct otto_dsa_priv *priv,
					   int port);

#endif /* _OTTO_STATS_H */
