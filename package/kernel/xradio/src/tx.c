/*
 * Datapath implementation for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <net/mac80211.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>

#include "xradio.h"
#include "wsm.h"
#include "bh.h"
#include "ap.h"
#include "sta.h"
#include "sdio.h"
#include "common.h"
#include "p2p.h"

#define B_RATE_INDEX   0     //11b rate for important short frames in 2.4G.
#define AG_RATE_INDEX  6     //11a/g rate for important short frames in 5G.
#define XRADIO_INVALID_RATE_ID (0xFF)

/* rate should fall quickly to avoid dropping frames by aps.
 * Add by yangfh 2014-9-22 13:39:57
 */
#define HIGH_RATE_MAX_RETRY  7

#ifdef TES_P2P_0002_ROC_RESTART
#include <linux/time.h>
#endif

//for test yangfh
extern u32 tx_retrylimit;
extern u32 tx_over_limit;
extern u32 tx_lower_limit;
extern int retry_mis;

static const struct ieee80211_rate *
xradio_get_tx_rate(const struct xradio_common *hw_priv,
		   const struct ieee80211_tx_rate *rate);

/* ******************************************************************** */
/* TX policy cache implementation					*/

static void tx_policy_dump(struct tx_policy *policy)
{
	txrx_printk(XRADIO_DBG_MSG, "[TX policy] "
		"%.1X%.1X%.1X%.1X%.1X%.1X%.1X%.1X"
		"%.1X%.1X%.1X%.1X%.1X%.1X%.1X%.1X"
		"%.1X%.1X%.1X%.1X%.1X%.1X%.1X%.1X: %d\n",
		policy->raw[0] & 0x0F,  policy->raw[0] >> 4,
		policy->raw[1] & 0x0F,  policy->raw[1] >> 4,
		policy->raw[2] & 0x0F,  policy->raw[2] >> 4,
		policy->raw[3] & 0x0F,  policy->raw[3] >> 4,
		policy->raw[4] & 0x0F,  policy->raw[4] >> 4,
		policy->raw[5] & 0x0F,  policy->raw[5] >> 4,
		policy->raw[6] & 0x0F,  policy->raw[6] >> 4,
		policy->raw[7] & 0x0F,  policy->raw[7] >> 4,
		policy->raw[8] & 0x0F,  policy->raw[8] >> 4,
		policy->raw[9] & 0x0F,  policy->raw[9] >> 4,
		policy->raw[10] & 0x0F,  policy->raw[10] >> 4,
		policy->raw[11] & 0x0F,  policy->raw[11] >> 4,
		policy->defined);
}

static void xradio_check_go_neg_conf_success(struct xradio_common *hw_priv,
						u8 *action)
{
	if (action[2] == 0x50 && action[3] == 0x6F && action[4] == 0x9A &&
		action[5] == 0x09 && action[6] == 0x02) {
		if(action[17] == 0) {
			hw_priv->is_go_thru_go_neg = true;
		}
		else {
			hw_priv->is_go_thru_go_neg = false;
		}
	}
}

static void xradio_check_prov_desc_req(struct xradio_common *hw_priv,
                                                u8 *action)
{
	if (action[2] == 0x50 && action[3] == 0x6F && action[4] == 0x9A &&
	    action[5] == 0x09 && action[6] == 0x07) {
		hw_priv->is_go_thru_go_neg = false;
	}
}

//modified by yangfh
static void tx_policy_build(const struct xradio_common *hw_priv,
	/* [out] */ struct tx_policy *policy,
	struct ieee80211_tx_rate *rates, size_t count)
{
	int i, j;
	struct ieee80211_rate * tmp_rate = NULL;
	unsigned limit = hw_priv->short_frame_max_tx_count;
	unsigned max_rates_cnt = count;
	unsigned total = 0;
	BUG_ON(rates[0].idx < 0);
	memset(policy, 0, sizeof(*policy));


	txrx_printk(XRADIO_DBG_NIY,"============================");
#if 0
	//debug yangfh
	for (i = 0; i < count; ++i) {
		if(rates[i].idx>=0) {
			tmp_rate = xradio_get_tx_rate(hw_priv, &rates[i]);
			txrx_printk(XRADIO_DBG_NIY,"[TX policy] Org %d.%dMps=%d", 
		            tmp_rate->bitrate/10, tmp_rate->bitrate%10, rates[i].count);
		}
	}
	txrx_printk(XRADIO_DBG_NIY,"----------------------------");
#endif
	
	/* minstrel is buggy a little bit, so distille
	 * incoming rates first.
	 */
	/* Sort rates in descending order. */
	total = rates[0].count;
	for (i = 1; i < count; ++i) {
		if (rates[i].idx > rates[i-1].idx) {
			rates[i].idx = rates[i-1].idx>0?(rates[i-1].idx-1):-1;
		}
		if (rates[i].idx < 0 || i>=limit) {
			count = i;
			break;
		} else {
			total += rates[i].count;
		}
	}

	/* Add lowest rate to the end when 11a/n. 
	 * Don't apply in 11b/g because p2p unsupport 1Mbps.
	 * TODO: it's better to do this in rate control of mac80211.
	 */
	if (((rates[0].flags & IEEE80211_TX_RC_MCS) || 
		   hw_priv->channel->band == NL80211_BAND_5GHZ) && 
		  count < max_rates_cnt && rates[count-1].idx != 0) {
		rates[count].idx   = 0;
		rates[count].count = rates[0].count;
		rates[count].flags = rates[0].flags;
		total += rates[count].count;
		count++;
	}

	/* adjust tx count to limit, rates should fall quickly 
	 * and lower rates should be more retry, because reorder 
	 * buffer of reciever will be timeout and clear probably.
	 */
	if (count < 2) {
		rates[0].count = limit;
		total = limit;
	} else {
		u8 end_retry = 0;  //the retry should be add to last rate.
		if (limit > HIGH_RATE_MAX_RETRY) {
			end_retry = limit - HIGH_RATE_MAX_RETRY;
			limit     = HIGH_RATE_MAX_RETRY;
		}
		for (i = 0; (limit != total) && (i < 100); ++i) {  //i<100 to avoid dead loop
			j = i % count;
			if(limit < total) {
				total += (rates[j].count > 1? -1 : 0);
				rates[j].count += (rates[j].count > 1? -1 : 0);
			} else {
				j = count - 1 - j;
				if (rates[j].count > 0) {
					total++;
					rates[j].count++;
				}
			}
		}
		if (end_retry) {
			rates[count-1].count += end_retry;
			limit += end_retry;
		}
	}
	
	/* Eliminate duplicates. */
	total = rates[0].count;
	for (i = 0, j = 1; j < count; ++j) {
		if (rates[j].idx == rates[i].idx) {
			rates[i].count += rates[j].count;
		} else if (rates[j].idx > rates[i].idx) {
			break;
		} else {
			++i;
			if (i != j)
				rates[i] = rates[j];
		}
		total += rates[j].count;
	}
	count = i + 1;

	/* Re-fill policy trying to keep every requested rate and with
	 * respect to the global max tx retransmission count. 
	 */
	if (limit < count)
		limit = count;
	if (total > limit) {
		for (i = 0; i < count; ++i) {
			int left = count - i - 1;
			if (rates[i].count > limit - left)
				rates[i].count = limit - left;
			limit -= rates[i].count;
		}
	}

	/* HACK!!! Device has problems (at least) switching from
	 * 54Mbps CTS to 1Mbps. This switch takes enormous amount
	 * of time (100-200 ms), leading to valuable throughput drop.
	 * As a workaround, additional g-rates are injected to the
	 * policy.
	 */
	if (count == 2 && !(rates[0].flags & IEEE80211_TX_RC_MCS) &&
			rates[0].idx > 4 && rates[0].count > 2 &&
			rates[1].idx < 2) {
		/* ">> 1" is an equivalent of "/ 2", but faster */
		int mid_rate = (rates[0].idx + 4) >> 1;

		/* Decrease number of retries for the initial rate */
		rates[0].count -= 2;

		if (mid_rate != 4) {
			/* Keep fallback rate at 1Mbps. */
			rates[3] = rates[1];

			/* Inject 1 transmission on lowest g-rate */
			rates[2].idx = 4;
			rates[2].count = 1;
			rates[2].flags = rates[1].flags;

			/* Inject 1 transmission on mid-rate */
			rates[1].idx = mid_rate;
			rates[1].count = 1;

			/* Fallback to 1 Mbps is a really bad thing,
			 * so let's try to increase probability of
			 * successful transmission on the lowest g rate
			 * even more */
			if (rates[0].count >= 3) {
				--rates[0].count;
				++rates[2].count;
			}

			/* Adjust amount of rates defined */
			count += 2;
		} else {
			/* Keep fallback rate at 1Mbps. */
			rates[2] = rates[1];

			/* Inject 2 transmissions on lowest g-rate */
			rates[1].idx = 4;
			rates[1].count = 2;

			/* Adjust amount of rates defined */
			count += 1;
		}
	}
	
	tmp_rate = (struct ieee80211_rate *)xradio_get_tx_rate(hw_priv, &rates[0]);
	if(tmp_rate)
		policy->defined = tmp_rate->hw_value + 1;

	for (i = 0; i < count; ++i) {
		register unsigned rateid, off, shift, retries;
		
		tmp_rate = (struct ieee80211_rate *)xradio_get_tx_rate(hw_priv, &rates[i]);
		if(tmp_rate) {
			rateid = tmp_rate->hw_value;
		} else {
			break;
		}
		off = rateid >> 3;		/* eq. rateid / 8 */
		shift = (rateid & 0x07) << 2;	/* eq. (rateid % 8) * 4 */

		retries = rates[i].count;
		if (unlikely(retries > 0x0F))
			rates[i].count = retries = 0x0F;
		policy->tbl[off] |= __cpu_to_le32(retries << shift);
		policy->retry_count += retries;
		txrx_printk(XRADIO_DBG_NIY,"[TX policy] %d.%dMps=%d", 
		            tmp_rate->bitrate/10, tmp_rate->bitrate%10, retries);
	}
	
	txrx_printk(XRADIO_DBG_MSG, "[TX policy] Dst Policy (%d): " \
		"%d:%d, %d:%d, %d:%d, %d:%d, %d:%d\n",
		count,
		rates[0].idx, rates[0].count,
		rates[1].idx, rates[1].count,
		rates[2].idx, rates[2].count,
		rates[3].idx, rates[3].count,
		rates[4].idx, rates[4].count);
}

static inline bool tx_policy_is_equal(const struct tx_policy *wanted,
					const struct tx_policy *cached)
{
	size_t count = wanted->defined >> 1;

	if (wanted->defined > cached->defined)
		return false;
	if (count) {
		if (memcmp(wanted->raw, cached->raw, count))
			return false;
	}
	if (wanted->defined & 1) {
		if ((wanted->raw[count] & 0x0F) != (cached->raw[count] & 0x0F))
			return false;
	}
	return true;
}

static int tx_policy_find(struct tx_policy_cache *cache,
				const struct tx_policy *wanted)
{
	/* O(n) complexity. Not so good, but there's only 8 entries in
	 * the cache.
	 * Also lru helps to reduce search time. */
	struct tx_policy_cache_entry *it;
	/* Search for policy in "used" list */
	list_for_each_entry(it, &cache->used, link) {
		if (tx_policy_is_equal(wanted, &it->policy))
			return it - cache->cache;
	}
	/* Then - in "free list" */
	list_for_each_entry(it, &cache->free, link) {
		if (tx_policy_is_equal(wanted, &it->policy))
			return it - cache->cache;
	}
	return -1;
}

static inline void tx_policy_use(struct tx_policy_cache *cache,
				 struct tx_policy_cache_entry *entry)
{
	++entry->policy.usage_count;
	list_move(&entry->link, &cache->used);
}

static inline int tx_policy_release(struct tx_policy_cache *cache,
				    struct tx_policy_cache_entry *entry)
{
	int ret = --entry->policy.usage_count;
	if (!ret)
		list_move(&entry->link, &cache->free);
	return ret;
}

/* ******************************************************************** */
/* External TX policy cache API						*/

void tx_policy_init(struct xradio_common *hw_priv)
{
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;
	int i;

	memset(cache, 0, sizeof(*cache));

	spin_lock_init(&cache->lock);
	INIT_LIST_HEAD(&cache->used);
	INIT_LIST_HEAD(&cache->free);

	for (i = 0; i < TX_POLICY_CACHE_SIZE; ++i)
		list_add(&cache->cache[i].link, &cache->free);
}

static int tx_policy_get(struct xradio_common *hw_priv,
		  struct ieee80211_tx_rate *rates,
		  u8 use_bg_rate, bool *renew)
{
	int idx;
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;
	struct tx_policy wanted;


	if(use_bg_rate) {
		u8 rate  = (u8)(use_bg_rate & 0x3f);
		u8 shitf = ((rate&0x7)<<2);
		u8 off   = (rate>>3);
		memset(&wanted, 0, sizeof(wanted));
		wanted.defined = rate + 1;
		wanted.retry_count = (hw_priv->short_frame_max_tx_count&0xf);
		wanted.tbl[off] = wanted.retry_count<<shitf;
		txrx_printk(XRADIO_DBG_NIY, "[TX policy] robust rate=%d\n", rate);
	} else
		tx_policy_build(hw_priv, &wanted, rates, IEEE80211_TX_MAX_RATES);

	spin_lock_bh(&cache->lock);
	idx = tx_policy_find(cache, &wanted);
	if (idx >= 0) {
		txrx_printk(XRADIO_DBG_MSG, "[TX policy] Used TX policy: %d\n",
					idx);
		*renew = false;
	} else {
		struct tx_policy_cache_entry *entry;
		if (WARN_ON_ONCE(list_empty(&cache->free))) {
			spin_unlock_bh(&cache->lock);
			txrx_printk(XRADIO_DBG_ERROR, "[TX policy] no policy cache\n");
			return XRADIO_INVALID_RATE_ID;
		}
		/* If policy is not found create a new one
		 * using the oldest entry in "free" list */
		*renew = true;
		entry = list_entry(cache->free.prev,
			struct tx_policy_cache_entry, link);
		entry->policy = wanted;
		idx = entry - cache->cache;
		txrx_printk(XRADIO_DBG_MSG, "[TX policy] New TX policy: %d\n",
					idx);
		tx_policy_dump(&entry->policy);
	}
	tx_policy_use(cache, &cache->cache[idx]);
	if (unlikely(list_empty(&cache->free))) {
		/* Lock TX queues. */
		txrx_printk(XRADIO_DBG_WARN, "[TX policy] policy cache used up\n");
		xradio_tx_queues_lock(hw_priv);
	}
	spin_unlock_bh(&cache->lock);

	return idx;
}

static void tx_policy_put(struct xradio_common *hw_priv, int idx)
{
	int usage, locked;
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;

	spin_lock_bh(&cache->lock);
	locked = list_empty(&cache->free);
	usage = tx_policy_release(cache, &cache->cache[idx]);
	if (unlikely(locked) && !usage) {
		/* Unlock TX queues. */
		xradio_tx_queues_unlock(hw_priv);
	}
	spin_unlock_bh(&cache->lock);
}

/*
bool tx_policy_cache_full(struct xradio_common *hw_priv)
{
	bool ret;
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;
	spin_lock_bh(&cache->lock);
	ret = list_empty(&cache->free);
	spin_unlock_bh(&cache->lock);
	return ret;
}
*/
extern u32 policy_upload;
extern u32 policy_num;
static int tx_policy_upload(struct xradio_common *hw_priv)
{
	struct tx_policy_cache *cache = &hw_priv->tx_policy_cache;
	int i;
	struct wsm_set_tx_rate_retry_policy arg = {
		.hdr = {
			.numTxRatePolicies = 0,
		}
	};
	int if_id = 0;

	spin_lock_bh(&cache->lock);
	/* Upload only modified entries. */
	for (i = 0; i < TX_POLICY_CACHE_SIZE; ++i) {
		struct tx_policy *src = &cache->cache[i].policy;
		if (src->retry_count && !src->uploaded) {
			struct wsm_set_tx_rate_retry_policy_policy *dst =
				&arg.tbl[arg.hdr.numTxRatePolicies];
			dst->policyIndex = i;
			dst->shortRetryCount = hw_priv->short_frame_max_tx_count-1;
			//only RTS need use longRetryCount, should be short_frame.
			dst->longRetryCount = hw_priv->short_frame_max_tx_count-1;
			
			/* BIT(2) - Terminate retries when Tx rate retry policy
			 *          finishes.
			 * BIT(3) - Count initial frame transmission as part of
			 *          rate retry counting but not as a retry
			 *          attempt */
			dst->policyFlags = BIT(2) | BIT(3);
			memcpy(dst->rateCountIndices, src->tbl,
					sizeof(dst->rateCountIndices));
			src->uploaded = 1;
			++arg.hdr.numTxRatePolicies;
		}
	}
	spin_unlock_bh(&cache->lock);
	atomic_set(&hw_priv->upload_count, 0);
	
	txrx_printk(XRADIO_DBG_MSG, "[TX policy] Upload %d policies\n",
				arg.hdr.numTxRatePolicies);

	/*TODO: COMBO*/
	return wsm_set_tx_rate_retry_policy(hw_priv, &arg, if_id);
}

void tx_policy_upload_work(struct work_struct *work)
{
	struct xradio_common *hw_priv =
		container_of(work, struct xradio_common, tx_policy_upload_work);

	WARN_ON(tx_policy_upload(hw_priv));
	wsm_unlock_tx(hw_priv);
}

/* ******************************************************************** */
/* xradio TX implementation						*/

struct xradio_txinfo {
	struct sk_buff *skb;
	unsigned queue;
	struct ieee80211_tx_info *tx_info;
	const struct ieee80211_rate *rate;
	struct ieee80211_hdr *hdr;
	size_t hdrlen;
	const u8 *da;
	struct xradio_sta_priv *sta_priv;
	struct xradio_txpriv txpriv;
};

u32 xradio_rate_mask_to_wsm(struct xradio_common *hw_priv, u32 rates)
{
	u32 ret = 0;
	int i;
	u32 n_bitrates = 
	          hw_priv->hw->wiphy->bands[hw_priv->channel->band]->n_bitrates;
	struct ieee80211_rate * bitrates = 
	          hw_priv->hw->wiphy->bands[hw_priv->channel->band]->bitrates;

	for (i = 0; i < n_bitrates; ++i) {
		if (rates & BIT(i))
			ret |= BIT(bitrates[i].hw_value);
	}
	return ret;
}

static const struct ieee80211_rate *
xradio_get_tx_rate(const struct xradio_common *hw_priv,
		   const struct ieee80211_tx_rate *rate)
{
	if (rate->idx < 0)
		return NULL;
	if (rate->flags & IEEE80211_TX_RC_MCS)
		return &hw_priv->mcs_rates[rate->idx];
	return &hw_priv->hw->wiphy->bands[hw_priv->channel->band]->
		bitrates[rate->idx];
}

inline static s8
xradio_get_rate_idx(const struct xradio_common *hw_priv, u8 flag, u16 hw_value)
{
	s16 ret = (s16)hw_value;
	if(flag & IEEE80211_TX_RC_MCS) {  //11n
		if(hw_value <= hw_priv->mcs_rates[7].hw_value && 
			 hw_value >= hw_priv->mcs_rates[0].hw_value)
			ret -= hw_priv->mcs_rates[0].hw_value;
		else 
			ret = -1;
	} else {  //11b/g
		if(hw_value>5 && hw_value<hw_priv->mcs_rates[0].hw_value) {
			ret -= hw_priv->hw->wiphy->bands[hw_priv->channel->band]->bitrates[0].hw_value;
			if(hw_priv->hw->wiphy->bands[hw_priv->channel->band]->bitrates[0].hw_value<5)  //11a
				ret -= 2;
		} else if(hw_value<4) {
			ret -= hw_priv->hw->wiphy->bands[hw_priv->channel->band]->bitrates[0].hw_value;
		} else {
			ret = -1;
		}
	}
	return (s8)ret;
}

static int
xradio_tx_h_calc_link_ids(struct xradio_vif *priv,
			  struct ieee80211_tx_control *control,
			  struct xradio_txinfo *t)
{

	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);

	if ((t->tx_info->flags & IEEE80211_TX_CTL_TX_OFFCHAN) ||
			(hw_priv->roc_if_id == priv->if_id))
		t->txpriv.offchannel_if_id = 2;
	else
		t->txpriv.offchannel_if_id = 0;

	if (likely(control->sta && t->sta_priv->link_id))
		t->txpriv.raw_link_id =
				t->txpriv.link_id =
				t->sta_priv->link_id;
	else if (priv->mode != NL80211_IFTYPE_AP)
		t->txpriv.raw_link_id =
				t->txpriv.link_id = 0;
	else if (is_multicast_ether_addr(t->da)) {
		if (priv->enable_beacon) {
			t->txpriv.raw_link_id = 0;
			t->txpriv.link_id = priv->link_id_after_dtim;
		} else {
			t->txpriv.raw_link_id = 0;
			t->txpriv.link_id = 0;
		}
	} else {
		t->txpriv.link_id =
			xradio_find_link_id(priv, t->da);
		/* Do not assign valid link id for deauth/disassoc frame being
		transmitted to an unassociated STA */
		if (!(t->txpriv.link_id) &&
			(ieee80211_is_deauth(t->hdr->frame_control) ||
			ieee80211_is_disassoc(t->hdr->frame_control))) {
					t->txpriv.link_id = 0;
		} else {
			if (!t->txpriv.link_id)
				t->txpriv.link_id = xradio_alloc_link_id(priv, t->da);
			if (!t->txpriv.link_id) {
				txrx_printk(XRADIO_DBG_ERROR,
				            "%s: No more link IDs available.\n", __func__);
				return -ENOENT;
			}
		}
		t->txpriv.raw_link_id = t->txpriv.link_id;
	}
	if (t->txpriv.raw_link_id)
		priv->link_id_db[t->txpriv.raw_link_id - 1].timestamp =
				jiffies;

#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	if (control->sta &&
			(control->sta->uapsd_queues & BIT(t->queue)))
		t->txpriv.link_id = priv->link_id_uapsd;
#endif /* CONFIG_XRADIO_USE_EXTENSIONS */
	return 0;
}

static void
xradio_tx_h_pm(struct xradio_vif *priv,
	       struct xradio_txinfo *t)
{
	if (unlikely(ieee80211_is_auth(t->hdr->frame_control))) {
		u32 mask = ~BIT(t->txpriv.raw_link_id);
		spin_lock_bh(&priv->ps_state_lock);
		priv->sta_asleep_mask &= mask;
		priv->pspoll_mask &= mask;
		spin_unlock_bh(&priv->ps_state_lock);
	}
}

static void
xradio_tx_h_calc_tid(struct xradio_vif *priv,
		     struct xradio_txinfo *t)
{
	if (ieee80211_is_data_qos(t->hdr->frame_control)) {
		u8 *qos = ieee80211_get_qos_ctl(t->hdr);
		t->txpriv.tid = qos[0] & IEEE80211_QOS_CTL_TID_MASK;
	} else if (ieee80211_is_data(t->hdr->frame_control)) {
		t->txpriv.tid = 0;
	}
}

/* IV/ICV injection. */
/* TODO: Quite unoptimal. It's better co modify mac80211
 * to reserve space for IV */
static int
xradio_tx_h_crypt(struct xradio_vif *priv,
		  struct xradio_txinfo *t)
{
	size_t iv_len;
	size_t icv_len;
	u8 *icv;

	if (!t->tx_info->control.hw_key ||
	    !(t->hdr->frame_control &
	     __cpu_to_le32(IEEE80211_FCTL_PROTECTED)))
		return 0;

	iv_len = t->tx_info->control.hw_key->iv_len;
	icv_len = t->tx_info->control.hw_key->icv_len;

	if (t->tx_info->control.hw_key->cipher == WLAN_CIPHER_SUITE_TKIP)
		icv_len += 8; /* MIC */

	if (unlikely((skb_headroom(t->skb) + skb_tailroom(t->skb) <
			 iv_len + icv_len + WSM_TX_EXTRA_HEADROOM) ||
			(skb_headroom(t->skb) <
			 iv_len + WSM_TX_EXTRA_HEADROOM))) {
		dev_dbg(priv->hw_priv->pdev,
			"no space allocated for crypto headers.\n"
			"headroom: %d, tailroom: %d, "
			"req_headroom: %d, req_tailroom: %d\n"
			"Please fix it in xradio_get_skb().\n",
			skb_headroom(t->skb), skb_tailroom(t->skb),
			iv_len + WSM_TX_EXTRA_HEADROOM, icv_len);
		return -ENOMEM;
	} else if (unlikely(skb_tailroom(t->skb) < icv_len)) {
		size_t offset = icv_len - skb_tailroom(t->skb);
		u8 *p;
		dev_dbg(priv->hw_priv->pdev,
			"Slowpath: tailroom is not big enough. "
			"Req: %d, got: %d.\n",
			icv_len, skb_tailroom(t->skb));

		p = skb_push(t->skb, offset);
		memmove(p, &p[offset], t->skb->len - offset);
		skb_trim(t->skb, t->skb->len - offset);
	}
	/* ccmp pkt from umac to driver,it has iv room,,so ccmp pkt do not add iv room */
	if (t->tx_info->control.hw_key->cipher != WLAN_CIPHER_SUITE_CCMP){
		u8 *newhdr;
		newhdr = skb_push(t->skb, iv_len);
		memmove(newhdr, newhdr + iv_len, t->hdrlen);
		t->hdr = (struct ieee80211_hdr *) newhdr;
	}
	t->hdrlen += iv_len;
	icv = skb_put(t->skb, icv_len);

	return 0;
}

static int
xradio_tx_h_align(struct xradio_vif *priv, struct xradio_txinfo *t,
                  u8 *flags)
{
	size_t offset = (size_t)t->skb->data & 3;
	u8 *newhdr;//add by dingxh


	if (!offset)
		return 0;

	if (skb_headroom(t->skb) < offset) {
		txrx_printk(XRADIO_DBG_ERROR,
			"Bug: no space allocated "
			"for DMA alignment.\n"
			"headroom: %d\n",
			skb_headroom(t->skb));
		return -ENOMEM;
	}
    //offset = 1or3 process   add by dingxh
	if (offset & 1) {
		newhdr = skb_push(t->skb, offset);
		memmove(newhdr, newhdr + offset, t->skb->len-offset);
		skb_trim(t->skb, t->skb->len-offset);
		t->hdr = (struct ieee80211_hdr *) newhdr;
		return 0;
	}
  //add by dingxh
	//offset=2 process
	skb_push(t->skb, offset);
	t->hdrlen += offset;
	t->txpriv.offset += offset;
	*flags |= WSM_TX_2BYTES_SHIFT;
	return 0;
}

static int
xradio_tx_h_action(struct xradio_vif *priv, struct xradio_txinfo *t)
{
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)t->hdr;

	if (ieee80211_is_action(t->hdr->frame_control) &&
			mgmt->u.action.category == WLAN_CATEGORY_BACK)
		return 1;
	else
		return 0;
}

/* Add WSM header */
static struct wsm_tx *
xradio_tx_h_wsm(struct xradio_vif *priv, struct xradio_txinfo *t)
{
	struct wsm_tx *wsm;
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);

	if (skb_headroom(t->skb) < sizeof(struct wsm_tx)) {
		txrx_printk(XRADIO_DBG_ERROR,
			"Bug: no space allocated "
			"for WSM header.\n"
			"headroom: %d\n",
			skb_headroom(t->skb));
		return NULL;
	}

	wsm = (struct wsm_tx *)skb_push(t->skb, sizeof(struct wsm_tx));
	t->txpriv.offset += sizeof(struct wsm_tx);
	memset(wsm, 0, sizeof(*wsm));
	wsm->hdr.len = __cpu_to_le16(t->skb->len);
	wsm->hdr.id  = __cpu_to_le16(0x0004);
	wsm->queueId = (t->txpriv.raw_link_id << 2) | wsm_queue_id_to_wsm(t->queue);
	if (wsm->hdr.len > hw_priv->wsm_caps.sizeInpChBuf) {
		txrx_printk(XRADIO_DBG_ERROR,"%s,msg length too big=%d\n",
		            __func__, wsm->hdr.len);
		wsm = NULL;
	}

	return wsm;
}

/* BT Coex specific handling */
static void
xradio_tx_h_bt(struct xradio_vif *priv, struct xradio_txinfo *t, struct wsm_tx *wsm)
{
	u8 priority = 0;
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);

	if (!hw_priv->is_BT_Present)
		return;

	if (unlikely(ieee80211_is_nullfunc(t->hdr->frame_control)))
		priority = WSM_EPTA_PRIORITY_MGT;
	else if (ieee80211_is_data(t->hdr->frame_control)) {
		/* Skip LLC SNAP header (+6) */
		u8 *payload = &t->skb->data[t->hdrlen];
		u16 *ethertype = (u16 *) &payload[6];
		if (unlikely(*ethertype == __be16_to_cpu(ETH_P_PAE)))
			priority = WSM_EPTA_PRIORITY_EAPOL;
	} else if (unlikely(ieee80211_is_assoc_req(t->hdr->frame_control) ||
		ieee80211_is_reassoc_req(t->hdr->frame_control))) {
		struct ieee80211_mgmt *mgt_frame =
				(struct ieee80211_mgmt *)t->hdr;

		if (mgt_frame->u.assoc_req.listen_interval <
						priv->listen_interval) {
			txrx_printk(XRADIO_DBG_MSG,
				"Modified Listen Interval to %d from %d\n",
				priv->listen_interval,
				mgt_frame->u.assoc_req.listen_interval);
			/* Replace listen interval derieved from
			 * the one read from SDD */
			mgt_frame->u.assoc_req.listen_interval =
				priv->listen_interval;
		}
	}

	if (likely(!priority)) {
		if (ieee80211_is_action(t->hdr->frame_control))
			priority = WSM_EPTA_PRIORITY_ACTION;
		else if (ieee80211_is_mgmt(t->hdr->frame_control))
			priority = WSM_EPTA_PRIORITY_MGT;
		else if (wsm->queueId == WSM_QUEUE_VOICE)
			priority = WSM_EPTA_PRIORITY_VOICE;
		else if (wsm->queueId == WSM_QUEUE_VIDEO)
			priority = WSM_EPTA_PRIORITY_VIDEO;
		else
			priority = WSM_EPTA_PRIORITY_DATA;
	}

	txrx_printk(XRADIO_DBG_MSG, "[TX] EPTA priority %d.\n",
		priority);

	wsm->flags |= priority << 1;
}

static int
xradio_tx_h_rate_policy(struct xradio_common *hw_priv, struct xradio_txinfo *t,
                        struct wsm_tx *wsm)
{
	bool tx_policy_renew = false;
	struct xradio_vif *priv =
				xrwl_get_vif_from_ieee80211(t->tx_info->control.vif);

	t->txpriv.rate_id = tx_policy_get(hw_priv,
		t->tx_info->control.rates, t->txpriv.use_bg_rate,
		&tx_policy_renew);
	if (t->txpriv.rate_id == XRADIO_INVALID_RATE_ID)
		return -EFAULT;

	wsm->flags |= t->txpriv.rate_id << 4;
	t->rate = xradio_get_tx_rate(hw_priv, &t->tx_info->control.rates[0]);
	if (t->txpriv.use_bg_rate)
		wsm->maxTxRate = (u8)(t->txpriv.use_bg_rate & 0x3f);
	else
		wsm->maxTxRate = t->rate->hw_value;

	if (t->rate->flags & IEEE80211_TX_RC_MCS) {
		if (priv->association_mode.greenfieldMode)
			wsm->htTxParameters |=
				__cpu_to_le32(WSM_HT_TX_GREENFIELD);
		else
			wsm->htTxParameters |=
				__cpu_to_le32(WSM_HT_TX_MIXED);
	}

	if (tx_policy_renew) {
		txrx_printk(XRADIO_DBG_MSG, "[TX] TX policy renew.\n");
		/* It's not so optimal to stop TX queues every now and then.
		 * Maybe it's better to reimplement task scheduling with
		 * a counter. */
		/* xradio_tx_queues_lock(priv); */
		/* Definetly better. TODO. */
		if (atomic_add_return(1, &hw_priv->upload_count) == 1) {
			wsm_lock_tx_async(hw_priv);
			if (queue_work(hw_priv->workqueue,
				  &hw_priv->tx_policy_upload_work) <= 0) {
				atomic_set(&hw_priv->upload_count, 0);
				wsm_unlock_tx(hw_priv);
			}
		}
	}
	return 0;
}

static bool
xradio_tx_h_pm_state(struct xradio_vif *priv, struct xradio_txinfo *t)
{
	int was_buffered = 1;


	if (t->txpriv.link_id == priv->link_id_after_dtim &&
			!priv->buffered_multicasts) {
		priv->buffered_multicasts = true;
		if (priv->sta_asleep_mask)
			queue_work(priv->hw_priv->workqueue,
				&priv->multicast_start_work);
	}

	if (t->txpriv.raw_link_id && t->txpriv.tid < XRADIO_MAX_TID)
		was_buffered = priv->link_id_db[t->txpriv.raw_link_id - 1]
				.buffered[t->txpriv.tid]++;

	return !was_buffered;
}

static void
xradio_tx_h_ba_stat(struct xradio_vif *priv,
		    struct xradio_txinfo *t)
{
	struct xradio_common *hw_priv = priv->hw_priv;


	if (priv->join_status != XRADIO_JOIN_STATUS_STA)
		return;
	if (!xradio_is_ht(&hw_priv->ht_oper))
		return;
	if (!priv->setbssparams_done)
		return;
	if (!ieee80211_is_data(t->hdr->frame_control))
		return;

	spin_lock_bh(&hw_priv->ba_lock);
	hw_priv->ba_acc += t->skb->len - t->hdrlen;
	if (!(hw_priv->ba_cnt_rx || hw_priv->ba_cnt)) {
		mod_timer(&hw_priv->ba_timer,
			jiffies + XRADIO_BLOCK_ACK_INTERVAL);
	}
	hw_priv->ba_cnt++;
	spin_unlock_bh(&hw_priv->ba_lock);
}

static int
xradio_tx_h_skb_pad(struct xradio_common *priv,
		    struct wsm_tx *wsm,
		    struct sk_buff *skb)
{
	size_t len = __le16_to_cpu(wsm->hdr.len);
	size_t padded_len = sdio_align_len(priv, len);


	if (WARN_ON(skb_padto(skb, padded_len) != 0)) {
		return -EINVAL;
	}
	return 0;
}

void xradio_tx(struct ieee80211_hw *dev, struct ieee80211_tx_control *control, struct sk_buff *skb)
{
	struct xradio_common *hw_priv = dev->priv;
	struct xradio_txinfo t = {
		.skb = skb,
		.queue = skb_get_queue_mapping(skb),
		.tx_info = IEEE80211_SKB_CB(skb),
		.hdr = (struct ieee80211_hdr *)skb->data,
		.txpriv.tid = XRADIO_MAX_TID,
		.txpriv.rate_id = XRADIO_INVALID_RATE_ID,
		.txpriv.use_bg_rate = 0,
	};
	struct ieee80211_sta *sta;
	struct wsm_tx *wsm;
	bool tid_update = 0;
	u8 flags = 0;
	int ret = 0;
	struct xradio_vif *priv;
	struct ieee80211_hdr *frame = (struct ieee80211_hdr *)skb->data;
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)skb->data;

	if (!skb->data)
		BUG_ON(1);

	if (!(t.tx_info->control.vif)) {
		ret = __LINE__;
		goto drop;
	}
	priv = xrwl_get_vif_from_ieee80211(t.tx_info->control.vif);
	if (!priv) {
		ret = __LINE__;
		goto drop;
	}

	if (atomic_read(&priv->enabled) == 0) {
		ret = __LINE__;
		goto drop;
	}

	//dhcp and 80211 frames are important, use b/g rate and delay scan.
	//it can make sense, such as accelerate connect.
	if (ieee80211_is_auth(frame->frame_control)) {
		hw_priv->connet_time[priv->if_id] = jiffies;
	} else if (ieee80211_is_data_present(frame->frame_control)) {
		/* since Umac had already alloc IV space in ccmp skb, so we need to add this iv_len as the new offset to LLC */
		u8* llc = NULL;
		if(t.tx_info->control.hw_key && 
		  	(t.hdr->frame_control & __cpu_to_le32(IEEE80211_FCTL_PROTECTED)) &&
		  		(t.tx_info->control.hw_key->cipher == WLAN_CIPHER_SUITE_CCMP))
			llc = skb->data+ieee80211_hdrlen(frame->frame_control) + t.tx_info->control.hw_key->iv_len;
		else
			llc = skb->data+ieee80211_hdrlen(frame->frame_control);
		if (is_dhcp(llc) || is_8021x(llc)) {
			t.txpriv.use_bg_rate = 
			hw_priv->hw->wiphy->bands[hw_priv->channel->band]->bitrates[0].hw_value;
			if (priv->vif->p2p)
				t.txpriv.use_bg_rate = AG_RATE_INDEX;
			t.txpriv.use_bg_rate |= 0x80;
		}
		if (t.txpriv.use_bg_rate){
			hw_priv->connet_time[priv->if_id] = jiffies;
		}
	} else if (ieee80211_is_deauth(frame->frame_control) ||
	           ieee80211_is_disassoc(frame->frame_control)) {
		hw_priv->connet_time[priv->if_id] = 0;
	}

#ifdef AP_HT_COMPAT_FIX
	if (ieee80211_is_assoc_req(frame->frame_control) && 
		priv->if_id == 0 && !(priv->ht_compat_det & 0x10)) {
		xradio_remove_ht_ie(priv, skb);
	}
#endif

#ifdef TES_P2P_0002_ROC_RESTART
	xradio_frame_monitor(hw_priv,skb,true);
#endif

	if (ieee80211_is_action(frame->frame_control) && 
		mgmt->u.action.category == WLAN_CATEGORY_PUBLIC) {
		u8 *action = (u8*)&mgmt->u.action.category;
		xradio_check_go_neg_conf_success(hw_priv, action);
		xradio_check_prov_desc_req(hw_priv, action);
	}

	t.txpriv.if_id = priv->if_id;
	t.hdrlen = ieee80211_hdrlen(t.hdr->frame_control);
	t.da = ieee80211_get_DA(t.hdr);
	t.sta_priv =
		(struct xradio_sta_priv *)&control->sta->drv_priv;

	if (WARN_ON(t.queue >= 4)) {
		ret = __LINE__;
		goto drop;
	}

	//spin_lock_bh(&hw_priv->tx_queue[t.queue].lock);
	//if ((priv->if_id == 0) &&
	//	(hw_priv->tx_queue[t.queue].num_queued_vif[0] >=
	//		hw_priv->vif0_throttle)) {
	//	spin_unlock_bh(&hw_priv->tx_queue[t.queue].lock);
	//	
	//	ret = __LINE__;
	//	goto drop;
	//} else if ((priv->if_id == 1) &&
	//	(hw_priv->tx_queue[t.queue].num_queued_vif[1] >=
	//		hw_priv->vif1_throttle)) {
	//	spin_unlock_bh(&hw_priv->tx_queue[t.queue].lock);
	//	
	//	ret = __LINE__;
	//	goto drop;
	//}
	//spin_unlock_bh(&hw_priv->tx_queue[t.queue].lock);

	ret = xradio_tx_h_calc_link_ids(priv, control, &t);
	if (ret) {
		ret = __LINE__;
		goto drop;
	}

	dev_dbg(hw_priv->pdev, "vif %d: tx, %d bytes queue %d, link_id %d(%d).\n",
			priv->if_id, skb->len, t.queue, t.txpriv.link_id, t.txpriv.raw_link_id);
	if(ieee80211_is_assoc_resp(frame->frame_control)){
		dev_dbg(hw_priv->pdev, "vif %d: association response\n", priv->if_id);
	}

	xradio_tx_h_pm(priv, &t);
	xradio_tx_h_calc_tid(priv, &t);
	ret = xradio_tx_h_crypt(priv, &t);
	if (ret) {
		ret = __LINE__;
		goto drop;
	}
	ret = xradio_tx_h_align(priv, &t, &flags);
	if (ret) {
		ret = __LINE__;
		goto drop;
	}
	ret = xradio_tx_h_action(priv, &t);
	if (ret) {
		ret = __LINE__;
		goto drop;
	}
	wsm = xradio_tx_h_wsm(priv, &t);
	if (!wsm) {
		ret = __LINE__;
		goto drop;
	}

	wsm->flags |= flags;
	xradio_tx_h_bt(priv, &t, wsm);
	ret = xradio_tx_h_rate_policy(hw_priv, &t, wsm);
	if (ret) {
		ret = __LINE__;
		goto drop;
	}

	ret = xradio_tx_h_skb_pad(hw_priv, wsm, skb);
	if (ret) {
		ret = __LINE__;
		goto drop;
	}

	rcu_read_lock();
	sta = rcu_dereference(control->sta);

	xradio_tx_h_ba_stat(priv, &t);
	spin_lock_bh(&priv->ps_state_lock);
	{
		tid_update = xradio_tx_h_pm_state(priv, &t);
		BUG_ON(xradio_queue_put(&hw_priv->tx_queue[t.queue],
				t.skb, &t.txpriv));
#ifdef ROC_DEBUG
		txrx_printk(XRADIO_DBG_ERROR, "QPUT %x, %pM, if_id - %d\n",
			t.hdr->frame_control, t.da, priv->if_id);
#endif
	}
	spin_unlock_bh(&priv->ps_state_lock);

#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	if (tid_update && sta)
		ieee80211_sta_set_buffered(sta,
				t.txpriv.tid, true);
#endif /* CONFIG_XRADIO_USE_EXTENSIONS */

	rcu_read_unlock();

	xradio_bh_wakeup(hw_priv);

	return;

drop:
	dev_dbg(hw_priv->pdev, "dropped tx at line %d, fctl=0x%04x.\n", ret, frame->frame_control);
	xradio_skb_dtor(hw_priv, skb, &t.txpriv);
	return;
}

void xradio_tx_confirm_cb(struct xradio_common *hw_priv,
			  struct wsm_tx_confirm *arg)
{
	u8 queue_id = xradio_queue_get_queue_id(arg->packetID);
	struct xradio_queue *queue = &hw_priv->tx_queue[queue_id];
	struct sk_buff *skb;
	const struct xradio_txpriv *txpriv;
	struct xradio_vif *priv;
	u32    feedback_retry = 0;

	priv = xrwl_hwpriv_to_vifpriv(hw_priv, arg->if_id);
	if (unlikely(!priv))
		return;

	if (unlikely(priv->mode == NL80211_IFTYPE_UNSPECIFIED)) {
		/* STA is stopped. */
		spin_unlock(&priv->vif_lock);
		return;
	}

	if (WARN_ON(queue_id >= 4)) {
		spin_unlock(&priv->vif_lock);
		return;
	}

	dev_dbg(hw_priv->pdev, "vif %d: tx confirm status=%d, retry=%d, lastRate=%d\n",
			priv->if_id, arg->status, arg->ackFailures, arg->txedRate);

	if ((arg->status == WSM_REQUEUE) &&
	    (arg->flags & WSM_TX_STATUS_REQUEUE)) {
		/* "Requeue" means "implicit suspend" */
		struct wsm_suspend_resume suspend = {
			.link_id = arg->link_id,
			.stop = 1,
			.multicast = !arg->link_id,
			.if_id = arg->if_id,
		};
		xradio_suspend_resume(priv, &suspend);
		txrx_printk(XRADIO_DBG_WARN, "Requeue for link_id %d (try %d)."
			" STAs asleep: 0x%.8X\n",
			arg->link_id,
			xradio_queue_get_generation(arg->packetID) + 1,
			priv->sta_asleep_mask);

		WARN_ON(xradio_queue_requeue(queue,
				arg->packetID, true));

		spin_lock_bh(&priv->ps_state_lock);
		if (!arg->link_id) {
			priv->buffered_multicasts = true;
			if (priv->sta_asleep_mask) {
				queue_work(hw_priv->workqueue,
					&priv->multicast_start_work);
			}
		}
		spin_unlock_bh(&priv->ps_state_lock);
		spin_unlock(&priv->vif_lock);
	} else if (!WARN_ON(xradio_queue_get_skb(
			queue, arg->packetID, &skb, &txpriv))) {
		struct ieee80211_tx_info *tx = IEEE80211_SKB_CB(skb);
		struct ieee80211_hdr *frame = (struct ieee80211_hdr *)&skb->data[txpriv->offset];
		int tx_count = arg->ackFailures;
		u8 ht_flags = 0;
		int i;

		//yangfh add to reset if_0 in firmware when STA-unjoined,
		//fix the errors when switch APs in combo mode.
		if (unlikely(ieee80211_is_disassoc(frame->frame_control) ||
			  ieee80211_is_deauth(frame->frame_control))) {
			if (priv->join_status == XRADIO_JOIN_STATUS_STA) {
				wms_send_deauth_to_self(hw_priv, priv);
				/* Shedule unjoin work */
				txrx_printk(XRADIO_DBG_WARN, "Issue unjoin command(TX) by self.\n");
				wsm_lock_tx_async(hw_priv);
				if (queue_work(hw_priv->workqueue, &priv->unjoin_work) <= 0)
					wsm_unlock_tx(hw_priv);
			}
		}

		if (priv->association_mode.greenfieldMode)
			ht_flags |= IEEE80211_TX_RC_GREEN_FIELD;

		//bss loss confirm.
		if (unlikely(priv->bss_loss_status == XRADIO_BSS_LOSS_CONFIRMING &&
		    priv->bss_loss_confirm_id == arg->packetID)) {
			spin_lock(&priv->bss_loss_lock);
			priv->bss_loss_status = arg->status?
			                        XRADIO_BSS_LOSS_CONFIRMED : XRADIO_BSS_LOSS_NONE;
			spin_unlock(&priv->bss_loss_lock);
		}

		if (likely(!arg->status)) {
			tx->flags |= IEEE80211_TX_STAT_ACK;
			priv->cqm_tx_failure_count = 0;
			++tx_count;

			if (arg->flags & WSM_TX_STATUS_AGGREGATION) {
				/* Do not report aggregation to mac80211:
				 * it confuses minstrel a lot. */
				/* tx->flags |= IEEE80211_TX_STAT_AMPDU; */
			}
		} else {
			/* TODO: Update TX failure counters */
			if (unlikely(priv->cqm_tx_failure_thold &&
			     (++priv->cqm_tx_failure_count >
			      priv->cqm_tx_failure_thold))) {
				priv->cqm_tx_failure_thold = 0;
				queue_work(hw_priv->workqueue,
						&priv->tx_failure_work);
			}
			if (tx_count)
				++tx_count;
		}
		spin_unlock(&priv->vif_lock);

		tx->status.ampdu_len = 1;
		tx->status.ampdu_ack_len = 1;

		txrx_printk(XRADIO_DBG_NIY,"feedback:%08x, %08x, %08x.\n", 
				         arg->rate_try[2], arg->rate_try[1], arg->rate_try[0]);
		if(txpriv->use_bg_rate) {   //bg rates
			tx->status.rates[0].count = arg->ackFailures+1;
		  tx->status.rates[0].idx   = 0;
		  tx->status.rates[1].idx   = -1;
		  tx->status.rates[2].idx   = -1;
		  tx->status.rates[3].idx   = -1;
		} else {
			int j;
			s8  txed_idx;
			register u8 rate_num=0, shift=0, retries=0;
			u8  flag = tx->status.rates[0].flags;
			
			//get retry rate idx.
			for(i=2; i>=0;i--) {
				if(arg->rate_try[i]) {
					for(j=7; j>=0;j--) {
						shift   = j<<2;
						retries = (arg->rate_try[i]>>shift)&0xf;
						if(retries) {
							feedback_retry += retries;
							txed_idx = xradio_get_rate_idx(hw_priv,flag,((i<<3)+j));
							txrx_printk(XRADIO_DBG_NIY, "rate_num=%d, hw=%d, idx=%d, "
							            "retries=%d, flag=%d", rate_num, ((i<<3)+j), 
							            txed_idx, retries, flag);
							if(likely(txed_idx>=0)) {
								tx->status.rates[rate_num].idx   = txed_idx;
								tx->status.rates[rate_num].count = retries;
								if (tx->status.rates[rate_num].flags & IEEE80211_TX_RC_MCS)
									tx->status.rates[rate_num].flags |= ht_flags;
								rate_num++;
								if(rate_num>=IEEE80211_TX_MAX_RATES) {
									i = -1;
									break;
								}
							}
						}
					}
				}
			}
			//clear other rate.
			for (i=rate_num; i < IEEE80211_TX_MAX_RATES; ++i) {
				tx->status.rates[i].count = 0;
				tx->status.rates[i].idx = -1;
			}
			//get successful rate idx.
			if(!arg->status) {
				txed_idx = xradio_get_rate_idx(hw_priv, flag, arg->txedRate);
				if(rate_num == 0) {
					tx->status.rates[0].idx = txed_idx;
					tx->status.rates[0].count = 1;
				} else if(rate_num <= IEEE80211_TX_MAX_RATES){
					--rate_num;
					if(txed_idx == tx->status.rates[rate_num].idx) {
						tx->status.rates[rate_num].count += 1;
					} else if(rate_num<(IEEE80211_TX_MAX_RATES-1)){
						++rate_num;
						tx->status.rates[rate_num].idx   = txed_idx;
						tx->status.rates[rate_num].count = 1;
					} else if(txed_idx >=0) {
						tx->status.rates[rate_num].idx   = txed_idx;
						tx->status.rates[rate_num].count = 1;
					}
				}
			} 
		}

		dev_dbg(hw_priv->pdev, "[TX policy] Ack: " \
		"%d:%d, %d:%d, %d:%d, %d:%d\n",
		tx->status.rates[0].idx, tx->status.rates[0].count,
		tx->status.rates[1].idx, tx->status.rates[1].count,
		tx->status.rates[2].idx, tx->status.rates[2].count,
		tx->status.rates[3].idx, tx->status.rates[3].count);
		

		xradio_queue_remove(queue, arg->packetID);
	}
}

static void xradio_notify_buffered_tx(struct xradio_vif *priv,
			       struct sk_buff *skb, int link_id, int tid)
{
#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	struct ieee80211_sta *sta;
	struct ieee80211_hdr *hdr;
	u8 *buffered;
	u8 still_buffered = 0;


	if (link_id && tid < XRADIO_MAX_TID) {
		buffered = priv->link_id_db
				[link_id - 1].buffered;

		spin_lock_bh(&priv->ps_state_lock);
		if (!WARN_ON(!buffered[tid]))
			still_buffered = --buffered[tid];
		spin_unlock_bh(&priv->ps_state_lock);

		if (!still_buffered && tid < XRADIO_MAX_TID) {
			hdr = (struct ieee80211_hdr *) skb->data;
			rcu_read_lock();
			sta = ieee80211_find_sta(priv->vif, hdr->addr1);
			if (sta)
				ieee80211_sta_set_buffered(sta, tid, false);
			rcu_read_unlock();
		}
	}
#endif /* CONFIG_XRADIO_USE_EXTENSIONS */
}

void xradio_skb_dtor(struct xradio_common *hw_priv,
		     struct sk_buff *skb,
		     const struct xradio_txpriv *txpriv)
{
	struct xradio_vif *priv =
		__xrwl_hwpriv_to_vifpriv(hw_priv, txpriv->if_id);


	skb_pull(skb, txpriv->offset);
	if (priv && txpriv->rate_id != XRADIO_INVALID_RATE_ID) {
		xradio_notify_buffered_tx(priv, skb,
				txpriv->raw_link_id, txpriv->tid);
		tx_policy_put(hw_priv, txpriv->rate_id);
	}
	ieee80211_tx_status(hw_priv->hw, skb);
}

#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
/* Workaround for WFD test case 6.1.10 */
void xradio_link_id_reset(struct work_struct *work)
{
	struct xradio_vif *priv =
		container_of(work, struct xradio_vif, linkid_reset_work);
	struct xradio_common *hw_priv = priv->hw_priv;
	int temp_linkid;


	if (!priv->action_linkid) {
		/* In GO mode we can receive ACTION frames without a linkID */
		temp_linkid = xradio_alloc_link_id(priv,
				&priv->action_frame_sa[0]);
		WARN_ON(!temp_linkid);
		if (temp_linkid) {
			/* Make sure we execute the WQ */
			flush_workqueue(hw_priv->workqueue);
			/* Release the link ID */
			spin_lock_bh(&priv->ps_state_lock);
			priv->link_id_db[temp_linkid - 1].prev_status =
				priv->link_id_db[temp_linkid - 1].status;
			priv->link_id_db[temp_linkid - 1].status =
				XRADIO_LINK_RESET;
			spin_unlock_bh(&priv->ps_state_lock);
			wsm_lock_tx_async(hw_priv);
			if (queue_work(hw_priv->workqueue,
				       &priv->link_id_work) <= 0)
				wsm_unlock_tx(hw_priv);
		}
	} else {
		spin_lock_bh(&priv->ps_state_lock);
		priv->link_id_db[priv->action_linkid - 1].prev_status =
			priv->link_id_db[priv->action_linkid - 1].status;
		priv->link_id_db[priv->action_linkid - 1].status =
			XRADIO_LINK_RESET_REMAP;
		spin_unlock_bh(&priv->ps_state_lock);
		wsm_lock_tx_async(hw_priv);
		if (queue_work(hw_priv->workqueue, &priv->link_id_work) <= 0)
				wsm_unlock_tx(hw_priv);
		flush_workqueue(hw_priv->workqueue);
	}
}
#endif
