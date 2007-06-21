/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
 * Copyright (c) 2006-2007 Jiri Benc <jbenc@suse.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/skbuff.h>
#include <linux/compiler.h>

#include <net/mac80211.h>
#include "ieee80211_i.h"
#include "ieee80211_rate.h"
#include "debugfs.h"

static void rate_control_lowest_tx_status(void *priv, struct net_device *dev,
                                          struct sk_buff *skb,
                                          struct ieee80211_tx_status *status)
{
}

static struct ieee80211_rate *
rate_control_lowest_get_rate(void *priv, struct net_device *dev,
                             struct sk_buff *skb,
                             struct rate_control_extra *extra)
{
	struct ieee80211_hw_mode *mode = extra->mode;
	int i;

	for (i = 0; i < mode->num_rates; i++) {
		struct ieee80211_rate *rate = &mode->rates[i];

		if (rate->flags & IEEE80211_RATE_SUPPORTED)
			return rate;
	}
	return &mode->rates[0];
}

static void rate_control_lowest_rate_init(void *priv, void *priv_sta,
                                          struct ieee80211_local *local,
                                          struct sta_info *sta)
{
	sta->txrate = 0;
}

static void *rate_control_lowest_alloc(struct ieee80211_local *local)
{
	return local;
}

static void rate_control_lowest_free(void *priv)
{
}

static void rate_control_lowest_clear(void *priv)
{
}

static void *rate_control_lowest_alloc_sta(void *priv, gfp_t gfp)
{
	return priv;
}

static void rate_control_lowest_free_sta(void *priv, void *priv_sta)
{
}

static struct rate_control_ops rate_control_lowest = {
	.module = THIS_MODULE,
	.name = "lowest",
	.tx_status = rate_control_lowest_tx_status,
	.get_rate = rate_control_lowest_get_rate,
	.rate_init = rate_control_lowest_rate_init,
	.clear = rate_control_lowest_clear,
	.alloc = rate_control_lowest_alloc,
	.free = rate_control_lowest_free,
	.alloc_sta = rate_control_lowest_alloc_sta,
	.free_sta = rate_control_lowest_free_sta,
};

static int __init rate_control_lowest_init(void)
{
	return ieee80211_rate_control_register(&rate_control_lowest);
}


static void __exit rate_control_lowest_exit(void)
{
	ieee80211_rate_control_unregister(&rate_control_lowest);
}


module_init(rate_control_lowest_init);
module_exit(rate_control_lowest_exit);

MODULE_DESCRIPTION("Forced 1 mbps rate control module for mac80211");
MODULE_LICENSE("GPL");

