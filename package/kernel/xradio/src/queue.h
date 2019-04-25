/*
 * queue operations for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#ifndef XRADIO_QUEUE_H_INCLUDED
#define XRADIO_QUEUE_H_INCLUDED

/* private */ struct xradio_queue_item;

/* extern */ struct sk_buff;
/* extern */ struct wsm_tx;
/* extern */ struct xradio_common;
/* extern */ struct xradio_vif;
/* extern */ struct ieee80211_tx_queue_stats;
/* extern */ struct xradio_txpriv;

/* forward */ struct xradio_queue_stats;

typedef void (*xradio_queue_skb_dtor_t)(struct xradio_common *priv,
                                        struct sk_buff *skb,
                                        const struct xradio_txpriv *txpriv);

struct xradio_queue {
	struct                    xradio_queue_stats *stats;
	size_t                    capacity;
	size_t                    num_queued;
	size_t                    num_queued_vif[XRWL_MAX_VIFS];
	size_t                    num_pending;
	size_t                    num_pending_vif[XRWL_MAX_VIFS];
	size_t                    num_sent;
	struct xradio_queue_item *pool;
	struct list_head          queue;
	struct list_head          free_pool;
	struct list_head          pending;
	int                       tx_locked_cnt;
	int                      *link_map_cache[XRWL_MAX_VIFS];
	bool                      overfull;
	spinlock_t                lock;
	u8                        queue_id;
	u8                        generation;
	struct timer_list	        gc;
	unsigned long             ttl;
};

struct xradio_queue_stats {
	spinlock_t              lock;
	int                    *link_map_cache[XRWL_MAX_VIFS];
	int                     num_queued[XRWL_MAX_VIFS];
	size_t                  map_capacity;
	wait_queue_head_t       wait_link_id_empty;
	xradio_queue_skb_dtor_t skb_dtor;
	struct xradio_common   *hw_priv;
};

struct xradio_txpriv {
	u8 link_id;
	u8 raw_link_id;
	u8 tid;
	u8 rate_id;
	u8 offset;
	u8 if_id;
	u8 offchannel_if_id;
	u8 use_bg_rate;
};

int xradio_queue_stats_init(struct xradio_queue_stats *stats,
                            size_t map_capacity,
                            xradio_queue_skb_dtor_t skb_dtor,
                            struct xradio_common *priv);
int xradio_queue_init(struct xradio_queue *queue,
                      struct xradio_queue_stats *stats,
                      u8 queue_id,
                      size_t capacity,
                      unsigned long ttl);
int xradio_queue_clear(struct xradio_queue *queue, int if_id);
void xradio_queue_stats_deinit(struct xradio_queue_stats *stats);
void xradio_queue_deinit(struct xradio_queue *queue);

size_t xradio_queue_get_num_queued(struct xradio_vif *priv,
                                   struct xradio_queue *queue,
                                   u32 link_id_map);
int xradio_queue_put(struct xradio_queue *queue,
                     struct sk_buff *skb, struct xradio_txpriv *txpriv);
int xradio_queue_get(struct xradio_queue *queue,
                     int if_id, u32 link_id_map,
                     struct wsm_tx **tx,
                     struct ieee80211_tx_info **tx_info,
                     struct xradio_txpriv **txpriv);

int xradio_queue_requeue(struct xradio_queue *queue, u32 packetID, bool check);

int xradio_queue_requeue_all(struct xradio_queue *queue);
int xradio_queue_remove(struct xradio_queue *queue,
                        u32 packetID);

int xradio_queue_get_skb(struct xradio_queue *queue, u32 packetID,
                         struct sk_buff **skb,
                         const struct xradio_txpriv **txpriv);
void xradio_queue_lock(struct xradio_queue *queue);
void xradio_queue_unlock(struct xradio_queue *queue);
bool xradio_queue_get_xmit_timestamp(struct xradio_queue *queue,
                                     unsigned long *timestamp, int if_id,
                                     u32 pending_frameID, u32 *Old_frame_ID);
bool xradio_query_txpkt_timeout(struct xradio_common *hw_priv, int if_id,
                                u32 pending_pkt_id, long *timeout);


bool xradio_queue_stats_is_empty(struct xradio_queue_stats *stats,
                                 u32 link_id_map, int if_id);

static inline u8 xradio_queue_get_queue_id(u32 packetID)
{
	return (packetID >> 16) & 0xF;
}

static inline u8 xradio_queue_get_if_id(u32 packetID)
{
	return (packetID >> 20) & 0xF;
}

static inline u8 xradio_queue_get_link_id(u32 packetID)
{
	return (packetID >> 24) & 0xF;
}

static inline u8 xradio_queue_get_generation(u32 packetID)
{
	return (packetID >>  8) & 0xFF;
}

#endif /* XRADIO_QUEUE_H_INCLUDED */
