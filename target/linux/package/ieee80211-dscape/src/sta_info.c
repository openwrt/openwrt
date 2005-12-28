/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>

#include <net/ieee80211.h>
#include "ieee80211_i.h"
#include "ieee80211_proc.h"
#include "rate_control.h"


/* Caller must hold local->sta_lock */
static void sta_info_hash_add(struct ieee80211_local *local,
			      struct sta_info *sta)
{
	sta->hnext = local->sta_hash[STA_HASH(sta->addr)];
	local->sta_hash[STA_HASH(sta->addr)] = sta;
}


/* Caller must hold local->sta_lock */
static void sta_info_hash_del(struct ieee80211_local *local,
			      struct sta_info *sta)
{
	struct sta_info *s;

	s = local->sta_hash[STA_HASH(sta->addr)];
	if (s == NULL)
		return;
	if (memcmp(s->addr, sta->addr, ETH_ALEN) == 0) {
		local->sta_hash[STA_HASH(sta->addr)] = s->hnext;
		return;
	}

	while (s->hnext != NULL &&
	       memcmp(s->hnext->addr, sta->addr, ETH_ALEN) != 0)
		s = s->hnext;
	if (s->hnext != NULL)
		s->hnext = s->hnext->hnext;
	else
		printk(KERN_ERR "%s: could not remove STA " MACSTR " from "
		       "hash table\n", local->mdev->name, MAC2STR(sta->addr));
}


struct sta_info * sta_info_get(struct ieee80211_local *local, u8 *addr)
{
	struct sta_info *sta;

	spin_lock_bh(&local->sta_lock);
	sta = local->sta_hash[STA_HASH(addr)];
	while (sta) {
		if (memcmp(sta->addr, addr, ETH_ALEN) == 0) {
			atomic_inc(&sta->users);
			break;
		}
		sta = sta->hnext;
	}
	spin_unlock_bh(&local->sta_lock);

	return sta;
}


int sta_info_min_txrate_get(struct ieee80211_local *local)
{
	struct sta_info *sta;
        int min_txrate = 9999999;
        int i;

	spin_lock_bh(&local->sta_lock);
	for (i = 0; i < STA_HASH_SIZE; i++) {
		sta = local->sta_hash[i];
		while (sta) {
			if (sta->txrate < min_txrate)
				min_txrate = sta->txrate;
			sta = sta->hnext;
		}
	}
	spin_unlock_bh(&local->sta_lock);
	if (min_txrate == 9999999)
		min_txrate = 0;

	return min_txrate;
}


void sta_info_release(struct ieee80211_local *local, struct sta_info *sta)
{
	struct sk_buff *skb;

	if (!atomic_dec_and_test(&sta->users))
		return;

	/* free sta structure; it has already been removed from
	 * hash table etc. external structures. Make sure that all
	 * buffered frames are release (one might have been added
	 * after sta_info_free() was called). */
	while ((skb = skb_dequeue(&sta->ps_tx_buf)) != NULL) {
		local->total_ps_buffered--;
		dev_kfree_skb_any(skb);
	}
	while ((skb = skb_dequeue(&sta->tx_filtered)) != NULL) {
		dev_kfree_skb_any(skb);
	}
	rate_control_free_sta(local, sta->rate_ctrl_priv);
	kfree(sta);
}


struct sta_info * sta_info_add(struct ieee80211_local *local,
			       struct net_device *dev, u8 *addr)
{
	struct sta_info *sta;

	sta = kmalloc(sizeof(*sta), GFP_ATOMIC);
	if (!sta)
		return NULL;

	memset(sta, 0, sizeof(*sta));

	sta->rate_ctrl_priv = rate_control_alloc_sta(local);
	if (sta->rate_ctrl_priv == NULL) {
		kfree(sta);
		return NULL;
	}

        memcpy(sta->addr, addr, ETH_ALEN);
        sta->dev = dev;
	skb_queue_head_init(&sta->ps_tx_buf);
	skb_queue_head_init(&sta->tx_filtered);
	atomic_inc(&sta->users); /* sta in hashlist etc, decremented by
				  * sta_info_free() */
	atomic_inc(&sta->users); /* sta used by caller, decremented by
				  * sta_info_release() */
	spin_lock_bh(&local->sta_lock);
	list_add(&sta->list, &local->sta_list);
	local->num_sta++;
        sta_info_hash_add(local, sta);
	spin_unlock_bh(&local->sta_lock);
	if (local->hw->sta_table_notification)
		local->hw->sta_table_notification(local->mdev, local->num_sta);
	sta->key_idx_compression = HW_KEY_IDX_INVALID;

#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
	printk(KERN_DEBUG "%s: Added STA " MACSTR "\n",
	       dev->name, MAC2STR(addr));
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */

	if (!in_interrupt()) {
		ieee80211_proc_init_sta(local, sta);
	} else {
		/* procfs entry adding might sleep, so schedule process context
		 * task for adding proc entry for STAs that do not yet have
		 * one. */
		schedule_work(&local->sta_proc_add);
	}

	return sta;
}


void sta_info_free(struct ieee80211_local *local, struct sta_info *sta,
		   int locked)
{
	struct sk_buff *skb;
	struct ieee80211_sub_if_data *sdata;

	if (!locked)
		spin_lock_bh(&local->sta_lock);
	sta_info_hash_del(local, sta);
	list_del(&sta->list);
	sdata = IEEE80211_DEV_TO_SUB_IF(sta->dev);
	if (sta->flags & WLAN_STA_PS) {
		sta->flags &= ~WLAN_STA_PS;
		if (sdata->bss)
			atomic_dec(&sdata->bss->num_sta_ps);
	}
	local->num_sta--;
	sta_info_remove_aid_ptr(sta);
	if (!locked)
		spin_unlock_bh(&local->sta_lock);
	if (local->hw->sta_table_notification)
		local->hw->sta_table_notification(local->mdev, local->num_sta);

	while ((skb = skb_dequeue(&sta->ps_tx_buf)) != NULL) {
		local->total_ps_buffered--;
		dev_kfree_skb_any(skb);
	}
	while ((skb = skb_dequeue(&sta->tx_filtered)) != NULL) {
		dev_kfree_skb_any(skb);
	}

	if (sta->key) {
		if (local->hw->set_key) {
			struct ieee80211_key_conf *key;
			key = ieee80211_key_data2conf(local, sta->key);
			if (key) {
				local->hw->set_key(local->mdev, DISABLE_KEY,
						   sta->addr, key, sta->aid);
				kfree(key);
			}
		}
		kfree(sta->key);
		sta->key = NULL;
	} else if (sta->key_idx_compression != HW_KEY_IDX_INVALID) {
		struct ieee80211_key_conf conf;
		memset(&conf, 0, sizeof(conf));
		conf.hw_key_idx = sta->key_idx_compression;
		conf.alg = ALG_NULL;
		conf.force_sw_encrypt = 1;
		local->hw->set_key(local->mdev, DISABLE_KEY, sta->addr, &conf,
				   sta->aid);
		sta->key_idx_compression = HW_KEY_IDX_INVALID;
	}

#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
	printk(KERN_DEBUG "%s: Removed STA " MACSTR "\n",
	       local->mdev->name, MAC2STR(sta->addr));
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */

	ieee80211_proc_deinit_sta(local, sta);

	if (atomic_read(&sta->users) != 1) {
		/* This is OK, but printed for debugging. The station structure
		 * will be removed when the other user of the data calls
		 * sta_info_release(). */
		printk(KERN_DEBUG "%s: STA " MACSTR " users count %d when "
		       "removing it\n", local->mdev->name, MAC2STR(sta->addr),
		       atomic_read(&sta->users));
	}

	sta_info_release(local, sta);
}


static inline int sta_info_buffer_expired(struct sk_buff *skb)
{
        struct ieee80211_tx_packet_data *pkt_data;
	if (!skb)
		return 0;

	/* TODO: this could be improved by passing STA listen interval into
	 * the kernel driver and expiring frames after 2 x listen_interval x
         * beacon interval */

        pkt_data = (struct ieee80211_tx_packet_data *) skb->cb;

        if (pkt_data->magic != IEEE80211_CB_MAGIC)
                return 1;

	return time_after(jiffies, pkt_data->jiffies + STA_TX_BUFFER_EXPIRE);
}


static void sta_info_cleanup_expire_buffered(struct ieee80211_local *local,
					     struct sta_info *sta)
{
	unsigned long flags;
	struct sk_buff *skb;

	if (skb_queue_empty(&sta->ps_tx_buf))
		return;

	for (;;) {
		spin_lock_irqsave(&sta->ps_tx_buf.lock, flags);
		skb = skb_peek(&sta->ps_tx_buf);
		if (sta_info_buffer_expired(skb))
			skb = __skb_dequeue(&sta->ps_tx_buf);
		else
			skb = NULL;
		spin_unlock_irqrestore(&sta->ps_tx_buf.lock, flags);

		if (skb) {
			local->total_ps_buffered--;
			printk(KERN_DEBUG "Buffered frame expired (STA "
			       MACSTR ")\n", MAC2STR(sta->addr));
			dev_kfree_skb(skb);
		} else
			break;
	}
}


static void sta_info_cleanup(unsigned long data)
{
	struct ieee80211_local *local = (struct ieee80211_local *) data;
	struct list_head *ptr;

	spin_lock_bh(&local->sta_lock);
	ptr = local->sta_list.next;
	while (ptr && ptr != &local->sta_list) {
		struct sta_info *sta = (struct sta_info *) ptr;
		atomic_inc(&sta->users);
		sta_info_cleanup_expire_buffered(local, sta);
		sta_info_release(local, sta);
		ptr = ptr->next;
	}
	spin_unlock_bh(&local->sta_lock);

	local->sta_cleanup.expires = jiffies + STA_INFO_CLEANUP_INTERVAL;
	add_timer(&local->sta_cleanup);
}


static void sta_info_proc_add_task(void *data)
{
	struct ieee80211_local *local = data;
	struct list_head *ptr;
	struct sta_info *sta;
	int max_adds = 100;

	while (max_adds > 0) {
		sta = NULL;
		spin_lock_bh(&local->sta_lock);
		list_for_each(ptr, &local->sta_list) {
			sta = list_entry(ptr, struct sta_info, list);
			if (!sta->proc_entry_added) {
				atomic_inc(&sta->users);
				break;
			}
			sta = NULL;
		}
		spin_unlock_bh(&local->sta_lock);

		if (!sta)
			break;

		ieee80211_proc_init_sta(local, sta);
		atomic_dec(&sta->users);

		max_adds--;
	}
}


void sta_info_init(struct ieee80211_local *local)
{
	spin_lock_init(&local->sta_lock);
	INIT_LIST_HEAD(&local->sta_list);

	init_timer(&local->sta_cleanup);
	local->sta_cleanup.expires = jiffies + STA_INFO_CLEANUP_INTERVAL;
	local->sta_cleanup.data = (unsigned long) local;
	local->sta_cleanup.function = sta_info_cleanup;

	INIT_WORK(&local->sta_proc_add, sta_info_proc_add_task, local);
}

void sta_info_start(struct ieee80211_local *local)
{
	add_timer(&local->sta_cleanup);
}

void sta_info_stop(struct ieee80211_local *local)
{
	struct list_head *ptr;

	del_timer(&local->sta_cleanup);

	ptr = local->sta_list.next;
	while (ptr && ptr != &local->sta_list) {
		struct sta_info *sta = (struct sta_info *) ptr;
		ptr = ptr->next;
		sta_info_free(local, sta, 0);
	}
}


void sta_info_remove_aid_ptr(struct sta_info *sta)
{
	struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(sta->dev);
	if (sta->aid <= 0 || !sdata->bss)
		return;

	sdata->bss->sta_aid[sta->aid - 1] = NULL;
	if (sta->aid == sdata->bss->max_aid) {
		while (sdata->bss->max_aid > 0 &&
		       sdata->bss->sta_aid[sdata->bss->max_aid - 1] == NULL)
			sdata->bss->max_aid--;
	}
}


/**
 * sta_info_flush - flush matching STA entries from the STA table
 * @local: local interface data
 * @dev: matching rule for the net device (sta->dev) or %NULL to match all STAs
 */
void sta_info_flush(struct ieee80211_local *local, struct net_device *dev)
{
        struct list_head *ptr, *n;

	spin_lock_bh(&local->sta_lock);

	list_for_each_safe(ptr, n, &local->sta_list) {
		struct sta_info *sta = list_entry(ptr, struct sta_info, list);
		if (dev == NULL || dev == sta->dev)
			sta_info_free(local, sta, 1);
	}
	spin_unlock_bh(&local->sta_lock);
}
