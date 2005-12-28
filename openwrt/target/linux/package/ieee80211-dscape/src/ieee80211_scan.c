/*
 * Copyright 2002-2004, Instant802 Networks, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/skbuff.h>

#include <net/ieee80211.h>
#include "ieee80211_i.h"
#include "rate_control.h"


/* Maximum number of seconds to wait for the traffic load to get below
 * threshold before forcing a passive scan. */
#define MAX_SCAN_WAIT 60
/* Threshold (pkts/sec TX or RX) for delaying passive scan */
#define SCAN_TXRX_THRESHOLD 75

static void get_channel_params(struct ieee80211_local *local, int channel,
				struct ieee80211_hw_modes **mode,
				struct ieee80211_channel **chan)
{
	int m;
	
	for (m = 0; m < local->hw->num_modes; m++) {
		*mode = &local->hw->modes[m];
		if ((*mode)->mode == local->conf.phymode)
			break;
	}
	local->scan.mode_idx = m;
	local->scan.chan_idx = 0;
	do {
		*chan = &(*mode)->channels[local->scan.chan_idx];
		if ((*chan)->chan == channel) {
			return;
		}
		local->scan.chan_idx++;
	} while (local->scan.chan_idx < (*mode)->num_channels);
	*chan = NULL;
}


static void next_chan_same_mode(struct ieee80211_local *local,
				struct ieee80211_hw_modes **mode,
				struct ieee80211_channel **chan)
{
	int m, prev;

	for (m = 0; m < local->hw->num_modes; m++) {
		*mode = &local->hw->modes[m];
		if ((*mode)->mode == local->conf.phymode)
			break;
	}
	local->scan.mode_idx = m;

	/* Select next channel - scan only channels marked with W_SCAN flag */
	prev = local->scan.chan_idx;
	do {
		local->scan.chan_idx++;
		if (local->scan.chan_idx >= (*mode)->num_channels)
			local->scan.chan_idx = 0;
		*chan = &(*mode)->channels[local->scan.chan_idx];
		if ((*chan)->flag & IEEE80211_CHAN_W_SCAN)
			break;
	} while (local->scan.chan_idx != prev);
}


static void next_chan_all_modes(struct ieee80211_local *local,
				struct ieee80211_hw_modes **mode,
				struct ieee80211_channel **chan)
{
	int prev, prev_m;

	if (local->scan.mode_idx >= local->hw->num_modes) {
		local->scan.mode_idx = 0;
		local->scan.chan_idx = 0;
	}

	/* Select next channel - scan only channels marked with W_SCAN flag */
	prev = local->scan.chan_idx;
	prev_m = local->scan.mode_idx;
	do {
		*mode = &local->hw->modes[local->scan.mode_idx];
		local->scan.chan_idx++;
		if (local->scan.chan_idx >= (*mode)->num_channels) {
			local->scan.chan_idx = 0;
			local->scan.mode_idx++;
			if (local->scan.mode_idx >= local->hw->num_modes)
				local->scan.mode_idx = 0;
			*mode = &local->hw->modes[local->scan.mode_idx];
		}
		*chan = &(*mode)->channels[local->scan.chan_idx];
		if ((*chan)->flag & IEEE80211_CHAN_W_SCAN)
			break;
	} while (local->scan.chan_idx != prev ||
		 local->scan.mode_idx != prev_m);
}


static void ieee80211_scan_start(struct net_device *dev,
				 struct ieee80211_scan_conf *conf)
{
	struct ieee80211_local *local = dev->priv;
	int old_mode_idx = local->scan.mode_idx;
	int old_chan_idx = local->scan.chan_idx;
	struct ieee80211_hw_modes *mode = NULL;
	struct ieee80211_channel *chan = NULL;
	int ret;

	if (local->hw->passive_scan == 0) {
		printk(KERN_DEBUG "%s: Scan handler called, yet the hardware "
		       "does not support passive scanning. Disabled.\n",
		       dev->name);
		return;
	}

	if ((local->scan.tries < MAX_SCAN_WAIT &&
	     local->scan.txrx_count > SCAN_TXRX_THRESHOLD)) {
		local->scan.tries++;
		/* Count TX/RX packets during one second interval and allow
		 * scan to start only if the number of packets is below the
		 * threshold. */
		local->scan.txrx_count = 0;
		local->scan.timer.expires = jiffies + HZ;
		add_timer(&local->scan.timer);
		return;
	}

	if (local->scan.skb == NULL) {
		printk(KERN_DEBUG "%s: Scan start called even though scan.skb "
		       "is not set\n", dev->name);
	}

	if (local->scan.our_mode_only) {
		if (local->scan.channel > 0) {
			get_channel_params(local, local->scan.channel, &mode,
					   &chan);
		} else
			next_chan_same_mode(local, &mode, &chan);
	}
	else
		next_chan_all_modes(local, &mode, &chan);

	conf->scan_channel = chan->chan;
	conf->scan_freq = chan->freq;
	conf->scan_channel_val = chan->val;
	conf->scan_phymode = mode->mode;
	conf->scan_power_level = chan->power_level;
	conf->scan_antenna_max = chan->antenna_max;
	conf->scan_time = 2 * local->hw->channel_change_time +
		local->scan.time; /* 10ms scan time+hardware changes */
	conf->skb = local->scan.skb ?
		skb_clone(local->scan.skb, GFP_ATOMIC) : NULL;
	conf->tx_control = &local->scan.tx_control;
#if 0
	printk(KERN_DEBUG "%s: Doing scan on mode: %d freq: %d chan: %d "
	       "for %d ms\n",
	       dev->name, conf->scan_phymode, conf->scan_freq,
	       conf->scan_channel, conf->scan_time);
#endif
	local->scan.rx_packets = 0;
	local->scan.rx_beacon = 0;
	local->scan.freq = chan->freq;
	local->scan.in_scan = 1;

	ieee80211_netif_oper(dev, NETIF_STOP);

	ret = local->hw->passive_scan(dev, IEEE80211_SCAN_START, conf);

	if (ret == 0) {
		long usec = local->hw->channel_change_time +
			local->scan.time;
		usec += 1000000L / HZ - 1;
		usec /= 1000000L / HZ;
		local->scan.timer.expires = jiffies + usec;
	} else {
		local->scan.in_scan = 0;
		if (conf->skb)
			dev_kfree_skb(conf->skb);
		ieee80211_netif_oper(dev, NETIF_WAKE);
		if (ret == -EAGAIN) {
			local->scan.timer.expires = jiffies +
				(local->scan.interval * HZ / 100);
			local->scan.mode_idx = old_mode_idx;
			local->scan.chan_idx = old_chan_idx;
		} else {
			printk(KERN_DEBUG "%s: Got unknown error from "
			       "passive_scan %d\n", dev->name, ret);
			local->scan.timer.expires = jiffies +
				(local->scan.interval * HZ);
		}
		local->scan.in_scan = 0;
	}

	add_timer(&local->scan.timer);
}


static void ieee80211_scan_stop(struct net_device *dev,
				struct ieee80211_scan_conf *conf)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_hw_modes *mode;
	struct ieee80211_channel *chan;
	int wait;

	if (local->hw->passive_scan == NULL)
		return;

	if (local->scan.mode_idx >= local->hw->num_modes) {
		local->scan.mode_idx = 0;
		local->scan.chan_idx = 0;
	}

	mode = &local->hw->modes[local->scan.mode_idx];

	if (local->scan.chan_idx >= mode->num_channels) {
		local->scan.chan_idx = 0;
	}

	chan = &mode->channels[local->scan.chan_idx];

	local->hw->passive_scan(dev, IEEE80211_SCAN_END, conf);

#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
	printk(KERN_DEBUG "%s: Did scan on mode: %d freq: %d chan: %d "
	       "GOT: %d Beacon: %d (%d)\n",
	       dev->name,
	       mode->mode, chan->freq, chan->chan,
	       local->scan.rx_packets, local->scan.rx_beacon,
	       local->scan.tries);
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */
	local->scan.num_scans++;

	local->scan.in_scan = 0;
	ieee80211_netif_oper(dev, NETIF_WAKE);

	local->scan.tries = 0;
	/* Use random interval of scan.interval .. 2 * scan.interval */
	wait = (local->scan.interval * HZ * ((net_random() & 127) + 128)) /
		128;
	local->scan.timer.expires = jiffies + wait;
		
	add_timer(&local->scan.timer);
}


static void ieee80211_scan_handler(unsigned long uldev)
{
        struct net_device *dev = (struct net_device *) uldev;
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_scan_conf conf;

	if (local->scan.interval == 0 && !local->scan.in_scan) {
		/* Passive scanning is disabled - keep the timer always
		 * running to make code cleaner. */
		local->scan.timer.expires = jiffies + 10 * HZ;
		add_timer(&local->scan.timer);
		return;
	}

	memset(&conf, 0, sizeof(struct ieee80211_scan_conf));
	conf.running_freq = local->conf.freq;
	conf.running_channel = local->conf.channel;
        conf.running_phymode = local->conf.phymode;
	conf.running_channel_val = local->conf.channel_val;
        conf.running_power_level = local->conf.power_level;
        conf.running_antenna_max = local->conf.antenna_max;

	if (local->scan.in_scan == 0)
		ieee80211_scan_start(dev, &conf);
	else
		ieee80211_scan_stop(dev, &conf);
}


void ieee80211_init_scan(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_hdr hdr;
	u16 fc;
	int len = 10;
	struct rate_control_extra extra;

	/* Only initialize passive scanning if the hardware supports it */
	if (!local->hw->passive_scan) {
		local->scan.skb = NULL;
		memset(&local->scan.tx_control, 0,
		       sizeof(local->scan.tx_control));
		printk(KERN_DEBUG "%s: Does not support passive scan, "
		       "disabled\n", dev->name);
		return;
	}

	local->scan.interval = 0;
	local->scan.our_mode_only = 1;
	local->scan.time = 10000;
	local->scan.timer.function = ieee80211_scan_handler;
	local->scan.timer.data = (unsigned long) dev;
	local->scan.timer.expires = jiffies + local->scan.interval * HZ;
	add_timer(&local->scan.timer);

	/* Create a CTS from for broadcasting before
	 * the low level changes channels */
	local->scan.skb = alloc_skb(len, GFP_KERNEL);
	if (local->scan.skb == NULL) {
		printk(KERN_WARNING "%s: Failed to allocate CTS packet for "
		       "passive scan\n", dev->name);
		return;
	}

	fc = (WLAN_FC_TYPE_CTRL << 2) | (WLAN_FC_STYPE_CTS << 4);
	hdr.frame_control = cpu_to_le16(fc);
	hdr.duration_id =
		cpu_to_le16(2 * local->hw->channel_change_time +
			    local->scan.time);
	memcpy(hdr.addr1, dev->dev_addr, ETH_ALEN); /* DA */
	hdr.seq_ctrl = 0;

	memcpy(skb_put(local->scan.skb, len), &hdr, len);

	memset(&local->scan.tx_control, 0, sizeof(local->scan.tx_control));
	local->scan.tx_control.key_idx = HW_KEY_IDX_INVALID;
	local->scan.tx_control.do_not_encrypt = 1;
	memset(&extra, 0, sizeof(extra));
	extra.endidx = local->num_curr_rates;
	local->scan.tx_control.tx_rate =
		rate_control_get_rate(dev, local->scan.skb, &extra)->val;
	local->scan.tx_control.no_ack = 1;
}


void ieee80211_stop_scan(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;

	if (local->hw->passive_scan != 0) {
		del_timer_sync(&local->scan.timer);
		dev_kfree_skb(local->scan.skb);
		local->scan.skb = NULL;
	}
}
