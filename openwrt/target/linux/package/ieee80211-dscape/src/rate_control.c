/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
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
#include <linux/compiler.h>

#include <net/ieee80211.h>
#include "ieee80211_i.h"
#include "rate_control.h"


/* This is a minimal implementation of TX rate controlling that can be used
 * as the default when no improved mechanisms are available. */


#define RATE_CONTROL_EMERG_DEC 2
#define RATE_CONTROL_INTERVAL (HZ / 20)
#define RATE_CONTROL_MIN_TX 10


static void rate_control_rate_inc(struct ieee80211_local *local,
				  struct sta_info *sta)
{
        struct ieee80211_sub_if_data *sdata;
	int i = sta->txrate;
	int maxrate;

        sdata = IEEE80211_DEV_TO_SUB_IF(sta->dev);
	if (sdata->bss && sdata->bss->force_unicast_rateidx > -1) {
		/* forced unicast rate - do not change STA rate */
		return;
	}

	maxrate = sdata->bss ? sdata->bss->max_ratectrl_rateidx : -1;

	if (i > local->num_curr_rates)
		i = local->num_curr_rates - 2;

	while (i + 1 < local->num_curr_rates) {
		i++;
		if (sta->supp_rates & BIT(i) &&
		    local->curr_rates[i].flags & IEEE80211_RATE_SUPPORTED &&
		    (maxrate < 0 || i <= maxrate)) {
			sta->txrate = i;
			break;
		}
	}
}


static void rate_control_rate_dec(struct ieee80211_local *local,
				  struct sta_info *sta)
{
        struct ieee80211_sub_if_data *sdata;
	int i = sta->txrate;

        sdata = IEEE80211_DEV_TO_SUB_IF(sta->dev);
	if (sdata->bss && sdata->bss->force_unicast_rateidx > -1) {
		/* forced unicast rate - do not change STA rate */
		return;
	}

	if (i > local->num_curr_rates)
		i = local->num_curr_rates;

	while (i > 0) {
		i--;
		if (sta->supp_rates & BIT(i) &&
		    local->curr_rates[i].flags & IEEE80211_RATE_SUPPORTED) {
			sta->txrate = i;
			break;
		}
	}
}


static struct ieee80211_rate *
rate_control_lowest_rate(struct ieee80211_local *local)
{
	int i;

	for (i = 0; i < local->num_curr_rates; i++) {
		struct ieee80211_rate *rate = &local->curr_rates[i];

		if (rate->flags & IEEE80211_RATE_SUPPORTED
			)
			return rate;
	}

	printk(KERN_DEBUG "rate_control_lowest_rate - no supported rates "
	       "found\n");
	return &local->curr_rates[0];
}


struct global_rate_control {
	int dummy;
};

struct sta_rate_control {
        unsigned long last_rate_change;
        u32 tx_num_failures;
        u32 tx_num_xmit;

        unsigned long avg_rate_update;
        u32 tx_avg_rate_sum;
        u32 tx_avg_rate_num;
};


static void rate_control_simple_tx_status(struct net_device *dev,
					  struct sk_buff *skb,
					  struct ieee80211_tx_status *status)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	struct sta_info *sta;
	struct sta_rate_control *srctrl;

        sta = sta_info_get(local, hdr->addr1);

	if (!sta)
            return;

	srctrl = sta->rate_ctrl_priv;
	srctrl->tx_num_xmit++;
	if (status->excessive_retries) {
		sta->antenna_sel = sta->antenna_sel == 1 ? 2 : 1;
		if (local->sta_antenna_sel == STA_ANTENNA_SEL_SW_CTRL_DEBUG) {
			printk(KERN_DEBUG "%s: " MACSTR " TX antenna --> %d "
			       "(@%lu)\n",
			       dev->name, MAC2STR(hdr->addr1),
			       sta->antenna_sel, jiffies);
		}
		srctrl->tx_num_failures++;
		sta->tx_retry_failed++;
		sta->tx_num_consecutive_failures++;
		sta->tx_num_mpdu_fail++;
	} else {
		sta->last_ack_rssi[0] = sta->last_ack_rssi[1];
		sta->last_ack_rssi[1] = sta->last_ack_rssi[2];
		sta->last_ack_rssi[2] = status->ack_signal;
		sta->tx_num_consecutive_failures = 0;
		sta->tx_num_mpdu_ok++;
	}
	sta->tx_retry_count += status->retry_count;
	sta->tx_num_mpdu_fail += status->retry_count;

	if (time_after(jiffies,
		       srctrl->last_rate_change + RATE_CONTROL_INTERVAL) &&
		srctrl->tx_num_xmit > RATE_CONTROL_MIN_TX) {
		u32 per_failed;
		srctrl->last_rate_change = jiffies;

		per_failed = (100 * sta->tx_num_mpdu_fail) /
			(sta->tx_num_mpdu_fail + sta->tx_num_mpdu_ok);
		/* TODO: calculate average per_failed to make adjusting
		 * parameters easier */
#if 0
		if (net_ratelimit()) {
			printk(KERN_DEBUG "MPDU fail=%d ok=%d per_failed=%d\n",
			       sta->tx_num_mpdu_fail, sta->tx_num_mpdu_ok,
			       per_failed);
		}
#endif

		if (per_failed > local->rate_ctrl_num_down) {
			rate_control_rate_dec(local, sta);
		} else if (per_failed < local->rate_ctrl_num_up) {
			rate_control_rate_inc(local, sta);
		}
		srctrl->tx_avg_rate_sum += local->curr_rates[sta->txrate].rate;
		srctrl->tx_avg_rate_num++;
		srctrl->tx_num_failures = 0;
		srctrl->tx_num_xmit = 0;
	} else if (sta->tx_num_consecutive_failures >=
		   RATE_CONTROL_EMERG_DEC) {
		rate_control_rate_dec(local, sta);
	}

	if (srctrl->avg_rate_update + 60 * HZ < jiffies) {
		srctrl->avg_rate_update = jiffies;
		if (srctrl->tx_avg_rate_num > 0) {
#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
			printk(KERN_DEBUG "%s: STA " MACSTR " Average rate: "
			       "%d (%d/%d)\n",
                               dev->name, MAC2STR(sta->addr),
			       srctrl->tx_avg_rate_sum /
			       srctrl->tx_avg_rate_num,
			       srctrl->tx_avg_rate_sum,
			       srctrl->tx_avg_rate_num);
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */
			srctrl->tx_avg_rate_sum = 0;
			srctrl->tx_avg_rate_num = 0;
		}
	}

	sta_info_release(local, sta);
}


static struct ieee80211_rate *
rate_control_simple_get_rate(struct net_device *dev, struct sk_buff *skb,
			     struct rate_control_extra *extra)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	struct sta_info *sta;
	int rateidx, nonerp_idx;
	u16 fc;

	memset(extra, 0, sizeof(*extra));

	fc = le16_to_cpu(hdr->frame_control);
	if (WLAN_FC_GET_TYPE(fc) != WLAN_FC_TYPE_DATA ||
	    (hdr->addr1[0] & 0x01)) {
		/* Send management frames and broadcast/multicast data using
		 * lowest rate. */
		/* TODO: this could probably be improved.. */
		return rate_control_lowest_rate(local);
	}

	sta = sta_info_get(local, hdr->addr1);

	if (!sta)
		return rate_control_lowest_rate(local);

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->bss && sdata->bss->force_unicast_rateidx > -1)
		sta->txrate = sdata->bss->force_unicast_rateidx;

	rateidx = sta->txrate;

	if (rateidx >= local->num_curr_rates)
		rateidx = local->num_curr_rates - 1;

	sta->last_txrate = rateidx;
	nonerp_idx = rateidx;
	while (nonerp_idx > 0 &&
	       ((local->curr_rates[nonerp_idx].flags & IEEE80211_RATE_ERP) ||
		!(local->curr_rates[nonerp_idx].flags &
		  IEEE80211_RATE_SUPPORTED) ||
		!(sta->supp_rates & BIT(nonerp_idx))))
		nonerp_idx--;
	extra->nonerp_idx = nonerp_idx;
	extra->nonerp = &local->curr_rates[extra->nonerp_idx];

	sta_info_release(local, sta);

	return &local->curr_rates[rateidx];
}


static void rate_control_simple_rate_init(struct ieee80211_local *local,
					  struct sta_info *sta)
{
	int i;
	sta->txrate = 0;
	/* TODO: what is a good starting rate for STA? About middle? Maybe not
	 * the lowest or the highest rate.. Could consider using RSSI from
	 * previous packets? Need to have IEEE 802.1X auth succeed immediately
	 * after assoc.. */
	for (i = 0; i < local->num_curr_rates; i++) {
		if ((sta->supp_rates & BIT(i)) &&
		    (local->curr_rates[i].flags & IEEE80211_RATE_SUPPORTED))
			sta->txrate = i;
	}
}


static void * rate_control_simple_alloc(struct ieee80211_local *local)
{
	struct global_rate_control *rctrl;

	rctrl = kmalloc(sizeof(*rctrl), GFP_ATOMIC);
	if (rctrl == NULL) {
		return NULL;
	}
	memset(rctrl, 0, sizeof(*rctrl));
	return rctrl;
}


static void rate_control_simple_free(void *priv)
{
	struct global_rate_control *rctrl = priv;
	kfree(rctrl);
}


static void rate_control_simple_clear(void *priv)
{
}


static void * rate_control_simple_alloc_sta(void)
{
	struct sta_rate_control *rctrl;

	rctrl = kmalloc(sizeof(*rctrl), GFP_ATOMIC);
	if (rctrl == NULL) {
		return NULL;
	}
	memset(rctrl, 0, sizeof(*rctrl));
	return rctrl;
}


static void rate_control_simple_free_sta(void *priv)
{
	struct sta_rate_control *rctrl = priv;
	kfree(rctrl);
}


static int rate_control_simple_status_sta(struct ieee80211_local *local,
					  struct sta_info *sta, char *buf)
{
	char *p = buf;
	struct sta_rate_control *srctrl = sta->rate_ctrl_priv;

	p += sprintf(p, "tx_avg_rate_sum=%d\n", srctrl->tx_avg_rate_sum);
	p += sprintf(p, "tx_avg_rate_num=%d\n", srctrl->tx_avg_rate_num);
	if (srctrl->tx_avg_rate_num)
		p += sprintf(p, "tx_avg_rate_avg=%d\n",
			     srctrl->tx_avg_rate_sum /
			     srctrl->tx_avg_rate_num);
	return p - buf;
}


static int rate_control_simple_status_global(struct ieee80211_local *local,
					     char *buf)
{
	return 0;
}


static struct rate_control_ops rate_control_simple = {
	.name = "simple",
	.tx_status = rate_control_simple_tx_status,
	.get_rate = rate_control_simple_get_rate,
	.rate_init = rate_control_simple_rate_init,
	.clear = rate_control_simple_clear,
	.status_sta = rate_control_simple_status_sta,
	.status_global = rate_control_simple_status_global,
	.alloc = rate_control_simple_alloc,
	.free = rate_control_simple_free,
	.alloc_sta = rate_control_simple_alloc_sta,
	.free_sta = rate_control_simple_free_sta,
};


int __init rate_control_simple_init(void)
{
	return ieee80211_rate_control_register(&rate_control_simple);
}


static void __exit rate_control_simple_exit(void)
{
	ieee80211_rate_control_unregister(&rate_control_simple);
}


module_init(rate_control_simple_init);
module_exit(rate_control_simple_exit);
