/* SPDX-License-Identifier: GPL-2.0-only */
/* Flowtable offload for the first-generation Ralink PPE */

#ifndef __RALINK_PPE_V1_OFFLOAD_H
#define __RALINK_PPE_V1_OFFLOAD_H

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/rhashtable-types.h>
#include <linux/spinlock.h>

#include "ppe_v1.h"

struct fe_priv;
struct sk_buff;

/* Original-direction tuple as learned by the hardware, in host order */
struct ppe_v1_tuple {
	u32 sip;
	u32 dip;
	u16 sport;
	u16 dport;
	u32 proto;
};

/* A flow block (flowtable) bound to the GMAC */
struct ppe_v1_block {
	struct ppe_v1 *ppe;
};

struct ppe_v1_flow {
	struct rhash_head node;
	struct rhash_head tuple_node;
	struct list_head list;
	unsigned long cookie;
	struct ppe_v1_block *block;
	struct ppe_v1_tuple key;
	/* bind template; ib1 only carries the L2 actions and the TTL bit */
	struct ppe_v1_foe_entry foe;
	/* FoE entry index once bound, or -1; checked against the entry */
	int hash;
};

struct ppe_v1_stats {
	u32 bind;
	u32 bind_no_flow;
	u32 bind_udp_nocsum;
	u32 bind_race;
};

struct ppe_v1 {
	struct net_device *netdev;

	struct ppe_v1_foe_entry *foe;
	dma_addr_t foe_phys;
	u32 entries;

	/* protects the FoE entries, ppe_v1_flow::hash and tuple_ht removal */
	spinlock_t lock;
	/* Serializes flow offload commands, and protects the flow list, the
	 * bound blocks and the running state.
	 */
	struct mutex mutex;
	struct rhashtable flow_ht;
	struct rhashtable tuple_ht;
	struct list_head flows;
	struct list_head block_cb_list;
	bool running;

	struct ppe_v1_stats stats;
	struct dentry *debugfs;
};

#ifdef CONFIG_NET_RALINK_PPE_V1
struct ppe_v1 *ppe_v1_create(struct fe_priv *priv, struct net_device *netdev);
void ppe_v1_start(struct ppe_v1 *ppe);
void ppe_v1_stop(struct ppe_v1 *ppe);
void ppe_v1_rx(struct ppe_v1 *ppe, struct sk_buff *skb, u32 rxd4);
int ppe_v1_setup_tc(struct ppe_v1 *ppe, enum tc_setup_type type,
		    void *type_data);
#else
static inline struct ppe_v1 *ppe_v1_create(struct fe_priv *priv,
					   struct net_device *netdev)
{
	return NULL;
}

static inline void ppe_v1_start(struct ppe_v1 *ppe) {}
static inline void ppe_v1_stop(struct ppe_v1 *ppe) {}

static inline void ppe_v1_rx(struct ppe_v1 *ppe, struct sk_buff *skb,
			     u32 rxd4)
{
}

static inline int ppe_v1_setup_tc(struct ppe_v1 *ppe,
				  enum tc_setup_type type, void *type_data)
{
	return -EOPNOTSUPP;
}
#endif

/* internal, shared between ppe_v1.c and ppe_v1_offload.c */
void ppe_v1_set_steering(struct ppe_v1 *ppe);
int ppe_v1_flow_commit(struct ppe_v1 *ppe, struct ppe_v1_flow *flow);
void ppe_v1_flow_clear(struct ppe_v1 *ppe, struct ppe_v1_flow *flow);
int ppe_v1_flow_idle_time(struct ppe_v1 *ppe, struct ppe_v1_flow *flow);
int ppe_v1_offload_init(struct ppe_v1 *ppe);
void ppe_v1_offload_cleanup(struct ppe_v1 *ppe);

#endif /* __RALINK_PPE_V1_OFFLOAD_H */
