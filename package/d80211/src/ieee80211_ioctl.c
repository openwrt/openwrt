/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005-2006, Devicescape Software, Inc.
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
#include <linux/etherdevice.h>
#include <linux/if_arp.h>
#include <linux/wireless.h>
#include <net/iw_handler.h>
#include <asm/uaccess.h>

#include <net/d80211.h>
#include "ieee80211_i.h"
#include "hostapd_ioctl.h"
#include "ieee80211_rate.h"
#include "wpa.h"
#include "aes_ccm.h"


static int ieee80211_regdom = 0x10; /* FCC */
module_param(ieee80211_regdom, int, 0444);
MODULE_PARM_DESC(ieee80211_regdom, "IEEE 802.11 regulatory domain; 64=MKK");

/*
 * If firmware is upgraded by the vendor, additional channels can be used based
 * on the new Japanese regulatory rules. This is indicated by setting
 * ieee80211_japan_5ghz module parameter to one when loading the 80211 kernel
 * module.
 */
static int ieee80211_japan_5ghz /* = 0 */;
module_param(ieee80211_japan_5ghz, int, 0444);
MODULE_PARM_DESC(ieee80211_japan_5ghz, "Vendor-updated firmware for 5 GHz");


static int ieee80211_ioctl_set_beacon(struct net_device *dev,
				      struct prism2_hostapd_param *param,
				      int param_len,
				      int flag)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_ap *ap;
	u8 **b_head, **b_tail;
	int *b_head_len, *b_tail_len;
	int len;

	len = ((char *) param->u.beacon.data - (char *) param) +
		param->u.beacon.head_len + param->u.beacon.tail_len;

	if (param_len > len)
		param_len = len;
	else if (param_len != len)
		return -EINVAL;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_IF_TYPE_AP)
		return -EINVAL;
	ap = &sdata->u.ap;

	switch (flag) {
	case 0:
		b_head = &ap->beacon_head;
		b_tail = &ap->beacon_tail;
		b_head_len = &ap->beacon_head_len;
		b_tail_len = &ap->beacon_tail_len;
		break;
	default:
		printk(KERN_DEBUG "%s: unknown beacon flag %d\n",
		       dev->name, flag);
		return -EINVAL;
	}

	kfree(*b_head);
	kfree(*b_tail);
	*b_head = NULL;
	*b_tail = NULL;

	*b_head_len = param->u.beacon.head_len;
	*b_tail_len = param->u.beacon.tail_len;

	*b_head = kmalloc(*b_head_len, GFP_KERNEL);
	if (*b_head)
		memcpy(*b_head, param->u.beacon.data, *b_head_len);
	else {
		printk(KERN_DEBUG "%s: failed to allocate beacon_head\n",
		       dev->name);
		return -ENOMEM;
	}

	if (*b_tail_len > 0) {
		*b_tail = kmalloc(*b_tail_len, GFP_KERNEL);
		if (*b_tail)
			memcpy(*b_tail, param->u.beacon.data + (*b_head_len),
			       (*b_tail_len));
		else {
			printk(KERN_DEBUG "%s: failed to allocate "
			       "beacon_tail\n", dev->name);
			return -ENOMEM;
		}
	}

	return ieee80211_if_config_beacon(dev);
}


static int ieee80211_ioctl_get_hw_features(struct net_device *dev,
					   struct prism2_hostapd_param *param,
					   int param_len)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	u8 *pos = param->u.hw_features.data;
	int left = param_len - (pos - (u8 *) param);
	int i;
	struct hostapd_ioctl_hw_modes_hdr *hdr;
	struct ieee80211_rate_data *rate;
	struct ieee80211_channel_data *chan;
	struct ieee80211_hw_mode *mode;

	param->u.hw_features.flags = 0;
	if (local->hw.flags & IEEE80211_HW_DATA_NULLFUNC_ACK)
		param->u.hw_features.flags |= HOSTAP_HW_FLAG_NULLFUNC_OK;

	param->u.hw_features.num_modes = 0;
	list_for_each_entry(mode, &local->modes_list, list) {
		int clen, rlen;

		param->u.hw_features.num_modes++;
		clen = mode->num_channels * sizeof(struct ieee80211_channel_data);
		rlen = mode->num_rates * sizeof(struct ieee80211_rate_data);
		if (left < sizeof(*hdr) + clen + rlen)
			return -E2BIG;
		left -= sizeof(*hdr) + clen + rlen;

		hdr = (struct hostapd_ioctl_hw_modes_hdr *) pos;
		hdr->mode = mode->mode;
		hdr->num_channels = mode->num_channels;
		hdr->num_rates = mode->num_rates;

		pos = (u8 *) (hdr + 1);
		chan = (struct ieee80211_channel_data *) pos;
		for (i = 0; i < mode->num_channels; i++) {
			chan[i].chan = mode->channels[i].chan;
			chan[i].freq = mode->channels[i].freq;
			chan[i].flag = mode->channels[i].flag;
		}
		pos += clen;

		rate = (struct ieee80211_rate_data *) pos;
		for (i = 0; i < mode->num_rates; i++) {
			rate[i].rate = mode->rates[i].rate;
			rate[i].flags = mode->rates[i].flags;
		}
		pos += rlen;
	}

	return 0;
}


static int ieee80211_ioctl_scan(struct net_device *dev,
                                struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	if (!local->ops->passive_scan)
		return -EOPNOTSUPP;

	if ((param->u.scan.now == 1) && (local->scan.in_scan == 1))
		return -EBUSY;

        if (param->u.scan.our_mode_only >= 0)
                local->scan.our_mode_only = param->u.scan.our_mode_only;
        if (param->u.scan.interval >= 0)
                local->scan.interval = param->u.scan.interval;
        if (param->u.scan.listen >= 0)
                local->scan.time = param->u.scan.listen;
		if (param->u.scan.channel > 0)
			local->scan.channel = param->u.scan.channel;
        if (param->u.scan.now == 1) {
                local->scan.in_scan = 0;
                mod_timer(&local->scan.timer, jiffies);
        }

        param->u.scan.our_mode_only = local->scan.our_mode_only;
        param->u.scan.interval = local->scan.interval;
        param->u.scan.listen = local->scan.time;
	if (local->scan.in_scan == 1)
		param->u.scan.last_rx = -1;
	else {
		param->u.scan.last_rx = local->scan.rx_packets;
		local->scan.rx_packets = -1;
	}
	param->u.scan.channel =
		local->scan.mode->channels[local->scan.chan_idx].chan;

	return 0;
}


static int ieee80211_ioctl_flush(struct net_device *dev,
				 struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	sta_info_flush(local, NULL);
	return 0;
}


/* Layer 2 Update frame (802.2 Type 1 LLC XID Update response) */
struct iapp_layer2_update {
	u8 da[ETH_ALEN]; /* broadcast */
	u8 sa[ETH_ALEN]; /* STA addr */
	__be16 len; /* 6 */
	u8 dsap; /* 0 */
	u8 ssap; /* 0 */
	u8 control;
	u8 xid_info[3];
} __attribute__ ((packed));

static void ieee80211_send_layer2_update(struct net_device *dev,
					 const u8 *addr)
{
	struct iapp_layer2_update *msg;
	struct sk_buff *skb;

	/* Send Level 2 Update Frame to update forwarding tables in layer 2
	 * bridge devices */

	skb = dev_alloc_skb(sizeof(*msg));
	if (!skb)
		return;
	msg = (struct iapp_layer2_update *) skb_put(skb, sizeof(*msg));

	/* 802.2 Type 1 Logical Link Control (LLC) Exchange Identifier (XID)
	 * Update response frame; IEEE Std 802.2-1998, 5.4.1.2.1 */

	memset(msg->da, 0xff, ETH_ALEN);
	memcpy(msg->sa, addr, ETH_ALEN);
	msg->len = htons(6);
	msg->dsap = 0;
	msg->ssap = 0x01; /* NULL LSAP, CR Bit: Response */
	msg->control = 0xaf; /* XID response lsb.1111F101.
			      * F=0 (no poll command; unsolicited frame) */
	msg->xid_info[0] = 0x81; /* XID format identifier */
	msg->xid_info[1] = 1; /* LLC types/classes: Type 1 LLC */
	msg->xid_info[2] = 0; /* XID sender's receive window size (RW) */

	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	memset(skb->cb, 0, sizeof(skb->cb));
	netif_rx(skb);
}


static int ieee80211_ioctl_add_sta(struct net_device *dev,
				   struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct sta_info *sta;
	u32 rates;
	int i, j;
        struct ieee80211_sub_if_data *sdata;
	int add_key_entry = 1;

	/* Prevent a race with changing the rate control algorithm */
	if (!netif_running(dev))
		return -ENETDOWN;

	sta = sta_info_get(local, param->sta_addr);

	if (!sta) {
		sta = sta_info_add(local, dev, param->sta_addr, GFP_KERNEL);
		if (!sta)
			return -ENOMEM;
        }

	if (sta->dev != dev) {
		/* Binding STA to a new interface, so remove all references to
		 * the old BSS. */
		spin_lock_bh(&local->sta_lock);
		sta_info_remove_aid_ptr(sta);
		spin_unlock_bh(&local->sta_lock);
	}

        /* TODO
         * We "steal" the device in case someone owns it
         * This will hurt WDS links and such when we have a
         * WDS link and a client associating from the same station
         */
        sta->dev = dev;
        sdata = IEEE80211_DEV_TO_SUB_IF(sta->dev);

	sta->flags |= WLAN_STA_AUTH | WLAN_STA_ASSOC;
	sta->aid = param->u.add_sta.aid;
	if (sta->aid > IEEE80211_MAX_AID)
		sta->aid = 0;
	sta->listen_interval = param->u.add_sta.listen_interval;

	rates = 0;
	for (i = 0; i < sizeof(param->u.add_sta.supp_rates); i++) {
		int rate = (param->u.add_sta.supp_rates[i] & 0x7f) * 5;
		if (local->hw.conf.phymode == MODE_ATHEROS_TURBO ||
		    local->hw.conf.phymode == MODE_ATHEROS_TURBOG)
			rate *= 2;
		for (j = 0; j < local->num_curr_rates; j++) {
			if (local->curr_rates[j].rate == rate)
				rates |= BIT(j);
		}

	}
	sta->supp_rates = rates;

	rate_control_rate_init(sta, local);

	if (param->u.add_sta.wds_flags & 0x01)
		sta->flags |= WLAN_STA_WDS;
	else
		sta->flags &= ~WLAN_STA_WDS;

	if (add_key_entry && !sta->key && !sdata->default_key &&
	    local->ops->set_key) {
		struct ieee80211_key_conf conf;
		/* Add key cache entry with NULL key type because this may used
		 * for TX filtering. */
		memset(&conf, 0, sizeof(conf));
		conf.hw_key_idx = HW_KEY_IDX_INVALID;
		conf.alg = ALG_NULL;
		conf.flags |= IEEE80211_KEY_FORCE_SW_ENCRYPT;
		if (local->ops->set_key(local_to_hw(local), SET_KEY,
				       sta->addr, &conf, sta->aid)) {
			sta->key_idx_compression = HW_KEY_IDX_INVALID;
		} else {
			sta->key_idx_compression = conf.hw_key_idx;
		}
	}

	sta_info_put(sta);

	if (sdata->type == IEEE80211_IF_TYPE_AP ||
	    sdata->type == IEEE80211_IF_TYPE_VLAN)
		ieee80211_send_layer2_update(dev, param->sta_addr);

	return 0;
}


static int ieee80211_ioctl_remove_sta(struct net_device *dev,
				      struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct sta_info *sta;

	sta = sta_info_get(local, param->sta_addr);
	if (sta) {
		sta_info_put(sta);
		sta_info_free(sta, 0);
	}

	return sta ? 0 : -ENOENT;
}


static int ieee80211_ioctl_get_dot11counterstable(struct net_device *dev,
					struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
        struct ieee80211_low_level_stats stats;

	memset(&stats, 0, sizeof(stats));
	if (local->ops->get_stats)
		local->ops->get_stats(local_to_hw(local), &stats);
        param->u.dot11CountersTable.dot11TransmittedFragmentCount =
                local->dot11TransmittedFragmentCount;
        param->u.dot11CountersTable.dot11MulticastTransmittedFrameCount =
                local->dot11MulticastTransmittedFrameCount;
        param->u.dot11CountersTable.dot11ReceivedFragmentCount =
                local->dot11ReceivedFragmentCount;
        param->u.dot11CountersTable.dot11MulticastReceivedFrameCount =
                local->dot11MulticastReceivedFrameCount;
        param->u.dot11CountersTable.dot11TransmittedFrameCount =
                local->dot11TransmittedFrameCount;
        param->u.dot11CountersTable.dot11FCSErrorCount =
                stats.dot11FCSErrorCount;
        param->u.dot11CountersTable.dot11ACKFailureCount =
                stats.dot11ACKFailureCount;
        param->u.dot11CountersTable.dot11RTSFailureCount =
                stats.dot11RTSFailureCount;
        param->u.dot11CountersTable.dot11RTSSuccessCount =
                stats.dot11RTSSuccessCount;

        return 0;
}


static int ieee80211_ioctl_get_info_sta(struct net_device *dev,
					struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct sta_info *sta;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff) {
		struct net_device_stats *stats;

		stats = ieee80211_dev_stats(local->mdev);
		param->u.get_info_sta.rx_bytes = stats->rx_bytes;
		param->u.get_info_sta.tx_bytes = stats->tx_bytes;
		/* go through all STAs and get STA with lowest max. rate */
		param->u.get_info_sta.current_tx_rate =
			local->curr_rates[sta_info_min_txrate_get(local)].rate;
		return 0;
	}

	sta = sta_info_get(local, param->sta_addr);

	if (!sta)
		return -ENOENT;

	param->u.get_info_sta.inactive_msec =
		jiffies_to_msecs(jiffies - sta->last_rx);
	param->u.get_info_sta.rx_packets = sta->rx_packets;
	param->u.get_info_sta.tx_packets = sta->tx_packets;
	param->u.get_info_sta.rx_bytes = sta->rx_bytes;
	param->u.get_info_sta.tx_bytes = sta->tx_bytes;
	param->u.get_info_sta.channel_use = sta->channel_use;
        param->u.get_info_sta.flags = sta->flags;
	if (sta->txrate >= 0 && sta->txrate < local->num_curr_rates)
		param->u.get_info_sta.current_tx_rate =
			local->curr_rates[sta->txrate].rate;
	param->u.get_info_sta.num_ps_buf_frames =
		skb_queue_len(&sta->ps_tx_buf);
	param->u.get_info_sta.tx_retry_failed = sta->tx_retry_failed;
	param->u.get_info_sta.tx_retry_count = sta->tx_retry_count;
	param->u.get_info_sta.last_rssi = sta->last_rssi;
	param->u.get_info_sta.last_ack_rssi = sta->last_ack_rssi[2];

	sta_info_put(sta);

	return 0;
}


static int ieee80211_ioctl_set_flags_sta(struct net_device *dev,
					 struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct sta_info *sta;

	sta = sta_info_get(local, param->sta_addr);
	if (sta) {
		sta->flags |= param->u.set_flags_sta.flags_or;
		sta->flags &= param->u.set_flags_sta.flags_and;
		if (local->ops->set_port_auth &&
		    (param->u.set_flags_sta.flags_or & WLAN_STA_AUTHORIZED) &&
		    local->ops->set_port_auth(local_to_hw(local), sta->addr, 1))
			printk(KERN_DEBUG "%s: failed to set low-level driver "
			       "PAE state (authorized) for " MAC_FMT "\n",
			       dev->name, MAC_ARG(sta->addr));
		if (local->ops->set_port_auth &&
		    !(param->u.set_flags_sta.flags_and & WLAN_STA_AUTHORIZED) &&
		    local->ops->set_port_auth(local_to_hw(local), sta->addr, 0))
			printk(KERN_DEBUG "%s: failed to set low-level driver "
			       "PAE state (unauthorized) for " MAC_FMT "\n",
			       dev->name, MAC_ARG(sta->addr));
		sta_info_put(sta);
	}

	return sta ? 0 : -ENOENT;
}


int ieee80211_set_hw_encryption(struct net_device *dev,
				struct sta_info *sta, u8 addr[ETH_ALEN],
				struct ieee80211_key *key)
{
	struct ieee80211_key_conf *keyconf = NULL;
	struct ieee80211_local *local = dev->ieee80211_ptr;
	int rc = 0;

	/* default to sw encryption; this will be cleared by low-level
	 * driver if the hw supports requested encryption */
	if (key)
		key->force_sw_encrypt = 1;

	if (key && local->ops->set_key &&
	    (keyconf = ieee80211_key_data2conf(local, key))) {
		if (local->ops->set_key(local_to_hw(local), SET_KEY, addr,
				       keyconf, sta ? sta->aid : 0)) {
			rc = HOSTAP_CRYPT_ERR_KEY_SET_FAILED;
			key->force_sw_encrypt = 1;
			key->hw_key_idx = HW_KEY_IDX_INVALID;
		} else {
			key->force_sw_encrypt =
				!!(keyconf->flags & IEEE80211_KEY_FORCE_SW_ENCRYPT);
			key->hw_key_idx =
				keyconf->hw_key_idx;

		}
	}
	kfree(keyconf);

	return rc;
}


static int ieee80211_set_encryption(struct net_device *dev, u8 *sta_addr,
				    int idx, int alg, int set_tx_key, int *err,
				    const u8 *_key, size_t key_len)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	int ret = 0;
	struct sta_info *sta;
	struct ieee80211_key *key, *old_key;
	int try_hwaccel = 1;
        struct ieee80211_key_conf *keyconf;
        struct ieee80211_sub_if_data *sdata;

        sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	if (sta_addr[0] == 0xff && sta_addr[1] == 0xff &&
	    sta_addr[2] == 0xff && sta_addr[3] == 0xff &&
	    sta_addr[4] == 0xff && sta_addr[5] == 0xff) {
		sta = NULL;
		if (idx >= NUM_DEFAULT_KEYS) {
			printk(KERN_DEBUG "%s: set_encrypt - invalid idx=%d\n",
			       dev->name, idx);
			return -EINVAL;
		}
		key = sdata->keys[idx];

		/* TODO: consider adding hwaccel support for these; at least
		 * Atheros key cache should be able to handle this since AP is
		 * only transmitting frames with default keys. */
		/* FIX: hw key cache can be used when only one virtual
		 * STA is associated with each AP. If more than one STA
		 * is associated to the same AP, software encryption
		 * must be used. This should be done automatically
		 * based on configured station devices. For the time
		 * being, this can be only set at compile time. */
	} else {
		set_tx_key = 0;
		if (idx != 0) {
			printk(KERN_DEBUG "%s: set_encrypt - non-zero idx for "
			       "individual key\n", dev->name);
			return -EINVAL;
		}

		sta = sta_info_get(local, sta_addr);
		if (!sta) {
			if (err)
				*err = HOSTAP_CRYPT_ERR_UNKNOWN_ADDR;
#ifdef CONFIG_D80211_VERBOSE_DEBUG
			printk(KERN_DEBUG "%s: set_encrypt - unknown addr "
			       MAC_FMT "\n",
			       dev->name, MAC_ARG(sta_addr));
#endif /* CONFIG_D80211_VERBOSE_DEBUG */

			return -ENOENT;
		}

		key = sta->key;
	}

	/* FIX:
	 * Cannot configure default hwaccel keys with WEP algorithm, if
	 * any of the virtual interfaces is using static WEP
	 * configuration because hwaccel would otherwise try to decrypt
	 * these frames.
	 *
	 * For now, just disable WEP hwaccel for broadcast when there is
	 * possibility of conflict with default keys. This can maybe later be
	 * optimized by using non-default keys (at least with Atheros ar521x).
	 */
	if (!sta && alg == ALG_WEP && !local->default_wep_only &&
	    sdata->type != IEEE80211_IF_TYPE_IBSS &&
	    sdata->type != IEEE80211_IF_TYPE_AP) {
		try_hwaccel = 0;
	}

	if (local->hw.flags & IEEE80211_HW_DEVICE_HIDES_WEP) {
		/* Software encryption cannot be used with devices that hide
		 * encryption from the host system, so always try to use
		 * hardware acceleration with such devices. */
		try_hwaccel = 1;
	}

	if ((local->hw.flags & IEEE80211_HW_NO_TKIP_WMM_HWACCEL) &&
	    alg == ALG_TKIP) {
		if (sta && (sta->flags & WLAN_STA_WME)) {
		/* Hardware does not support hwaccel with TKIP when using WMM.
		 */
			try_hwaccel = 0;
		}
		else if (sdata->type == IEEE80211_IF_TYPE_STA) {
			sta = sta_info_get(local, sdata->u.sta.bssid);
			if (sta) {
				if (sta->flags & WLAN_STA_WME) {
					try_hwaccel = 0;
				}
				sta_info_put(sta);
				sta = NULL;
			}
		}
	}

	if (alg == ALG_NONE) {
		keyconf = NULL;
		if (try_hwaccel && key &&
		    key->hw_key_idx != HW_KEY_IDX_INVALID &&
		    local->ops->set_key &&
		    (keyconf = ieee80211_key_data2conf(local, key)) != NULL &&
		    local->ops->set_key(local_to_hw(local), DISABLE_KEY,
				       sta_addr, keyconf, sta ? sta->aid : 0)) {
			if (err)
				*err = HOSTAP_CRYPT_ERR_KEY_SET_FAILED;
			printk(KERN_DEBUG "%s: set_encrypt - low-level disable"
			       " failed\n", dev->name);
			ret = -EINVAL;
		}
		kfree(keyconf);

		if (key && sdata->default_key == key) {
			ieee80211_key_sysfs_remove_default(sdata);
			sdata->default_key = NULL;
		}
		ieee80211_key_sysfs_remove(key);
		if (sta)
			sta->key = NULL;
		else
			sdata->keys[idx] = NULL;
		ieee80211_key_free(key);
		key = NULL;
	} else {
		old_key = key;
		key = ieee80211_key_alloc(sta ? NULL : sdata, idx, key_len,
					  GFP_KERNEL);
		if (!key) {
			ret = -ENOMEM;
			goto err_out;
		}

		/* default to sw encryption; low-level driver sets these if the
		 * requested encryption is supported */
		key->hw_key_idx = HW_KEY_IDX_INVALID;
		key->force_sw_encrypt = 1;

		key->alg = alg;
		key->keyidx = idx;
		key->keylen = key_len;
		memcpy(key->key, _key, key_len);
		if (set_tx_key)
			key->default_tx_key = 1;

		if (alg == ALG_CCMP) {
			/* Initialize AES key state here as an optimization
			 * so that it does not need to be initialized for every
			 * packet. */
			key->u.ccmp.tfm = ieee80211_aes_key_setup_encrypt(
				key->key);
			if (!key->u.ccmp.tfm) {
				ret = -ENOMEM;
				goto err_free;
			}
		}

		if (old_key && sdata->default_key == old_key) {
			ieee80211_key_sysfs_remove_default(sdata);
			sdata->default_key = NULL;
		}
		ieee80211_key_sysfs_remove(old_key);
		if (sta)
			sta->key = key;
		else
			sdata->keys[idx] = key;
		ieee80211_key_free(old_key);
		if (sta)
			key->kobj.parent = &sta->kobj;
		ret = ieee80211_key_sysfs_add(key);
		if (ret)
			goto err_null;

		if (try_hwaccel &&
		    (alg == ALG_WEP || alg == ALG_TKIP || alg == ALG_CCMP)) {
			int e = ieee80211_set_hw_encryption(dev, sta, sta_addr,
							    key);
			if (err)
				*err = e;
		}
	}

	if (set_tx_key || (!sta && !sdata->default_key && key)) {
		sdata->default_key = key;
		if (ieee80211_key_sysfs_add_default(sdata))
			printk(KERN_WARNING "%s: cannot create symlink to "
			       "default key\n", dev->name);
		if (local->ops->set_key_idx &&
		    local->ops->set_key_idx(local_to_hw(local), idx))
			printk(KERN_DEBUG "%s: failed to set TX key idx for "
			       "low-level driver\n", dev->name);
	}

	if (sta)
		sta_info_put(sta);

	return 0;

err_null:
	if (sta)
		sta->key = NULL;
	else
		sdata->keys[idx] = NULL;
err_free:
	ieee80211_key_free(key);
err_out:
	if (sta)
		sta_info_put(sta);
	return ret;
}


static int ieee80211_ioctl_set_encryption(struct net_device *dev,
					  struct prism2_hostapd_param *param,
					  int param_len)
{
	int alg;

	param->u.crypt.err = 0;
	param->u.crypt.alg[HOSTAP_CRYPT_ALG_NAME_LEN - 1] = '\0';

	if (param_len <
	    (int) ((char *) param->u.crypt.key - (char *) param) +
	    param->u.crypt.key_len) {
		printk(KERN_DEBUG "%s: set_encrypt - invalid param_lem\n",
		       dev->name);
		return -EINVAL;
        }

	if (strcmp(param->u.crypt.alg, "none") == 0)
		alg = ALG_NONE;
	else if (strcmp(param->u.crypt.alg, "WEP") == 0)
		alg = ALG_WEP;
	else if (strcmp(param->u.crypt.alg, "TKIP") == 0) {
		if (param->u.crypt.key_len != ALG_TKIP_KEY_LEN) {
			printk(KERN_DEBUG "%s: set_encrypt - invalid TKIP key "
			       "length %d\n", dev->name,
			       param->u.crypt.key_len);
			return -EINVAL;
		}
		alg = ALG_TKIP;
	} else if (strcmp(param->u.crypt.alg, "CCMP") == 0) {
		if (param->u.crypt.key_len != ALG_CCMP_KEY_LEN) {
			printk(KERN_DEBUG "%s: set_encrypt - invalid CCMP key "
			       "length %d\n", dev->name,
			       param->u.crypt.key_len);
			return -EINVAL;
		}
		alg = ALG_CCMP;
	} else {
		param->u.crypt.err = HOSTAP_CRYPT_ERR_UNKNOWN_ALG;
		printk(KERN_DEBUG "%s: set_encrypt - unknown alg\n",
		       dev->name);
		return -EINVAL;
	}

	return ieee80211_set_encryption(
		dev, param->sta_addr,
		param->u.crypt.idx, alg,
		param->u.crypt.flags & HOSTAP_CRYPT_FLAG_SET_TX_KEY,
		&param->u.crypt.err, param->u.crypt.key,
		param->u.crypt.key_len);
}


static int ieee80211_ioctl_get_encryption(struct net_device *dev,
					  struct prism2_hostapd_param *param,
					  int param_len)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	int ret = 0;
	struct sta_info *sta;
	struct ieee80211_key **key;
	int max_key_len;
        struct ieee80211_sub_if_data *sdata;
	u8 *pos;

        sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	param->u.crypt.err = 0;

	max_key_len = param_len -
		(int) ((char *) param->u.crypt.key - (char *) param);
	if (max_key_len < 0)
		return -EINVAL;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff) {
		sta = NULL;
		if (param->u.crypt.idx >= NUM_DEFAULT_KEYS) {
			param->u.crypt.idx = sdata->default_key ?
				sdata->default_key->keyidx : 0;
			return 0;
		} else
			key = &sdata->keys[param->u.crypt.idx];
	} else {
		sta = sta_info_get(local, param->sta_addr);
		if (!sta) {
			param->u.crypt.err = HOSTAP_CRYPT_ERR_UNKNOWN_ADDR;
			return -EINVAL;
		}

		key = &sta->key;
	}

	memset(param->u.crypt.seq_counter, 0, HOSTAP_SEQ_COUNTER_SIZE);
	if (!*key) {
		memcpy(param->u.crypt.alg, "none", 5);
		param->u.crypt.key_len = 0;
		param->u.crypt.idx = 0xff;
	} else {
		switch ((*key)->alg) {
		case ALG_WEP:
			memcpy(param->u.crypt.alg, "WEP", 4);
			break;
		case ALG_TKIP:
		{
			u32 iv32;
			u16 iv16;

			memcpy(param->u.crypt.alg, "TKIP", 5);
			if (local->ops->get_sequence_counter) {
			/* Get transmit counter from low level driver */
				if (local->ops->get_sequence_counter(
						local_to_hw(local),
						param->sta_addr,
						(*key)->keyidx,
						IEEE80211_SEQ_COUNTER_TX,
						&iv32,
						&iv16)) {
					/* Error getting value from device */
					return -EIO;
				}
			} else {
				/* Get it from our own local data */
				iv32 = (*key)->u.tkip.iv32;
				iv16 = (*key)->u.tkip.iv16;
			}
			pos = param->u.crypt.seq_counter;
			*pos++ = iv16 & 0xff;
			*pos++ = (iv16 >> 8) & 0xff;
			*pos++ = iv32 & 0xff;
			*pos++ = (iv32 >> 8) & 0xff;
			*pos++ = (iv32 >> 16) & 0xff;
			*pos++ = (iv32 >> 24) & 0xff;
			break;
			}
		case ALG_CCMP:
		{
			u8 *pn;
			memcpy(param->u.crypt.alg, "CCMP", 5);
			pos = param->u.crypt.seq_counter;
			pn = (*key)->u.ccmp.tx_pn;
			*pos++ = pn[5];
			*pos++ = pn[4];
			*pos++ = pn[3];
			*pos++ = pn[2];
			*pos++ = pn[1];
			*pos++ = pn[0];
			break;
		}
		default:
			memcpy(param->u.crypt.alg, "unknown", 8);
			break;
		}

		if (max_key_len < (*key)->keylen)
			ret = -E2BIG;
		else {
			param->u.crypt.key_len = (*key)->keylen;
			memcpy(param->u.crypt.key, (*key)->key,
			       (*key)->keylen);
		}
	}

	if (sta)
		sta_info_put(sta);

	return ret;
}


#ifdef CONFIG_HOSTAPD_WPA_TESTING
static int ieee80211_ioctl_wpa_trigger(struct net_device *dev,
				       struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct sta_info *sta;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff) {
		local->wpa_trigger = param->u.wpa_trigger.trigger;
		return 0;
	}

	sta = sta_info_get(local, param->sta_addr);
	if (!sta) {
		printk(KERN_DEBUG "%s: wpa_trigger - unknown addr\n",
		       dev->name);
		return -EINVAL;
	}

	sta->wpa_trigger = param->u.wpa_trigger.trigger;

	sta_info_put(sta);
	return 0;
}
#endif /* CONFIG_HOSTAPD_WPA_TESTING */


static int ieee80211_ioctl_set_rate_sets(struct net_device *dev,
					 struct prism2_hostapd_param *param,
					 int param_len)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	u16 *pos = (u16 *) param->u.set_rate_sets.data;
	int left = param_len - ((u8 *) pos - (u8 *) param);
	int i, mode, num_supp, num_basic, *supp, *basic, *prev;

	mode = param->u.set_rate_sets.mode;
	num_supp = param->u.set_rate_sets.num_supported_rates;
	num_basic = param->u.set_rate_sets.num_basic_rates;

	if (left < (num_supp + num_basic) * 2) {
		printk(KERN_WARNING "%s: invalid length in hostapd set rate "
		       "sets ioctl (%d != %d)\n", dev->name, left,
		       (num_supp + num_basic) * 2);
		return -EINVAL;
	}

	supp = (int *) kmalloc((num_supp + 1) * sizeof(int), GFP_KERNEL);
	basic = (int *) kmalloc((num_basic + 1) * sizeof(int), GFP_KERNEL);

	if (!supp || !basic) {
		kfree(supp);
		kfree(basic);
		return -ENOMEM;
	}

	for (i = 0; i < num_supp; i++)
		supp[i] = *pos++;
	supp[i] = -1;

	for (i = 0; i < num_basic; i++)
		basic[i] = *pos++;
	basic[i] = -1;

	if (num_supp == 0) {
		kfree(supp);
		supp = NULL;
	}

	if (num_basic == 0) {
		kfree(basic);
		basic = NULL;
	}

	prev = local->supp_rates[mode];
	local->supp_rates[mode] = supp;
	kfree(prev);

	prev = local->basic_rates[mode];
	local->basic_rates[mode] = basic;
	kfree(prev);

	if (mode == local->hw.conf.phymode) {
		/* TODO: should update STA TX rates and remove STAs if they
		 * do not have any remaining supported rates after the change
		 */
		ieee80211_prepare_rates(local);
	}

	return 0;
}


static int ieee80211_ioctl_add_if(struct net_device *dev,
				  struct prism2_hostapd_param *param,
				  int param_len)
{
	u8 *pos = param->u.if_info.data;
        int left = param_len - ((u8 *) pos - (u8 *) param);
	struct net_device *new_dev;
	int res;
	struct hostapd_if_wds *wds;
	struct hostapd_if_bss *bss;

	printk(KERN_WARNING "PRISM2_HOSTAPD_ADD_IF ioctl is deprecated!");
	switch (param->u.if_info.type) {
	case HOSTAP_IF_WDS:
		wds = (struct hostapd_if_wds *) param->u.if_info.data;

                if (left < sizeof(struct hostapd_if_wds))
                        return -EPROTO;

		res = ieee80211_if_add(dev, param->u.if_info.name, 0, &new_dev);
		if (res)
			return res;
		ieee80211_if_set_type(new_dev, IEEE80211_IF_TYPE_WDS);
		res = ieee80211_if_update_wds(new_dev, wds->remote_addr);
		if (res)
			__ieee80211_if_del(dev->ieee80211_ptr,
					   IEEE80211_DEV_TO_SUB_IF(new_dev));
		return res;
	case HOSTAP_IF_VLAN:
		if (left < sizeof(struct hostapd_if_vlan))
			return -EPROTO;

		res = ieee80211_if_add(dev, param->u.if_info.name, 0, &new_dev);
		if (res)
			return res;
		ieee80211_if_set_type(new_dev, IEEE80211_IF_TYPE_VLAN);
#if 0
		res = ieee80211_if_update_vlan(new_dev, vlan->id);
		if (res)
			__ieee80211_if_del(dev->ieee80211_ptr,
					   IEEE80211_DEV_TO_SUB_IF(new_dev));
#endif
		return res;
	case HOSTAP_IF_BSS:
		bss = (struct hostapd_if_bss *) param->u.if_info.data;

                if (left < sizeof(struct hostapd_if_bss))
                        return -EPROTO;

		res = ieee80211_if_add(dev, param->u.if_info.name, 0, &new_dev);
		if (res)
			return res;
		ieee80211_if_set_type(new_dev, IEEE80211_IF_TYPE_AP);
		memcpy(new_dev->dev_addr, bss->bssid, ETH_ALEN);
		return 0;
	case HOSTAP_IF_STA:
                if (left < sizeof(struct hostapd_if_sta))
                        return -EPROTO;

		res = ieee80211_if_add(dev, param->u.if_info.name, 0, &new_dev);
		if (res)
			return res;
		ieee80211_if_set_type(new_dev, IEEE80211_IF_TYPE_STA);
		return 0;
	default:
		return -EINVAL;
	}

	return 0;
}

static int ieee80211_ioctl_remove_if(struct net_device *dev,
				     struct prism2_hostapd_param *param)
{
	unsigned int type;

	switch (param->u.if_info.type) {
	case HOSTAP_IF_WDS:
		type = IEEE80211_IF_TYPE_WDS;
		break;
	case HOSTAP_IF_VLAN:
		type = IEEE80211_IF_TYPE_VLAN;
		break;
	case HOSTAP_IF_BSS:
		type = IEEE80211_IF_TYPE_AP;
		break;
	case HOSTAP_IF_STA:
		type = IEEE80211_IF_TYPE_STA;
		break;
	default:
		return -EINVAL;
	}

	return ieee80211_if_remove(dev, param->u.if_info.name, type);
}

static int ieee80211_ioctl_update_if(struct net_device *dev,
				     struct prism2_hostapd_param *param,
				     int param_len)
{
	u8 *pos = param->u.if_info.data;
        int left = param_len - ((u8 *) pos - (u8 *) param);

	if (param->u.if_info.type == HOSTAP_IF_WDS) {
		struct hostapd_if_wds *wds =
			(struct hostapd_if_wds *) param->u.if_info.data;
		struct ieee80211_local *local = dev->ieee80211_ptr;
		struct net_device *wds_dev = NULL;
		struct ieee80211_sub_if_data *sdata;

		if (left < sizeof(struct ieee80211_if_wds))
			return -EPROTO;

		list_for_each_entry(sdata, &local->sub_if_list, list) {
			if (strcmp(param->u.if_info.name,
				   sdata->dev->name) == 0) {
				wds_dev = sdata->dev;
				break;
			}
		}

		if (!wds_dev || sdata->type != IEEE80211_IF_TYPE_WDS)
			return -ENODEV;

		return ieee80211_if_update_wds(wds_dev, wds->remote_addr);
	} else {
		return -EOPNOTSUPP;
	}
}


static int ieee80211_ioctl_flush_ifs(struct net_device *dev,
				     struct prism2_hostapd_param *param)
{
	ieee80211_if_flush(dev);
	return 0;
}


static int ieee80211_ioctl_scan_req(struct net_device *dev,
				    struct prism2_hostapd_param *param,
				    int param_len)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	u8 *pos = param->u.scan_req.ssid;
	int left = param_len - ((u8 *) pos - (u8 *) param);
	int len = param->u.scan_req.ssid_len;

	if (local->user_space_mlme)
		return -EOPNOTSUPP;

	if (!netif_running(dev))
		return -ENETDOWN;

	if (left < len || len > IEEE80211_MAX_SSID_LEN)
		return -EINVAL;

	return ieee80211_sta_req_scan(dev, pos, len);
}


static int ieee80211_ioctl_sta_get_state(struct net_device *dev,
					 struct prism2_hostapd_param *param)
{
	struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_IF_TYPE_STA &&
	    sdata->type != IEEE80211_IF_TYPE_IBSS)
		return -EINVAL;
	param->u.sta_get_state.state = sdata->u.sta.state;
	return 0;
}


static int ieee80211_ioctl_mlme(struct net_device *dev,
				struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata;

	if (local->user_space_mlme)
		return -EOPNOTSUPP;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_IF_TYPE_STA &&
	    sdata->type != IEEE80211_IF_TYPE_IBSS)
		return -EINVAL;
	switch (param->u.mlme.cmd) {
	case MLME_STA_DEAUTH:
		return ieee80211_sta_deauthenticate(dev, param->u.mlme.reason_code);
	case MLME_STA_DISASSOC:
		return ieee80211_sta_disassociate(dev, param->u.mlme.reason_code);
	}
	return 0;
}


static int ieee80211_ioctl_get_load_stats(struct net_device *dev,
					  struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	param->u.get_load_stats.channel_use = local->channel_use;
/*	if (param->u.get_load_stats.flags & LOAD_STATS_CLEAR)
		local->channel_use = 0; */ /* now it's not raw counter */

	return 0;
}


static int ieee80211_ioctl_set_sta_vlan(struct net_device *dev,
                                        struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
        struct sta_info *sta;

        sta = sta_info_get(local, param->sta_addr);
        if (sta) {
                struct net_device *new_vlan_dev;
                new_vlan_dev =
			dev_get_by_name(param->u.set_sta_vlan.vlan_name);
                if (new_vlan_dev) {
#if 0
			printk("%s: Station " MAC_FMT " moved to vlan: %s\n",
			       dev->name, MAC_ARG(param->sta_addr),
                               new_vlan_dev->name);
#endif
			if (sta->dev != new_vlan_dev) {
				ieee80211_send_layer2_update(new_vlan_dev,
							     sta->addr);
			}
                        sta->dev = new_vlan_dev;
			sta->vlan_id = param->u.set_sta_vlan.vlan_id;
                        dev_put(new_vlan_dev);
                }
		sta_info_put(sta);
	}

	return sta ? 0 : -ENOENT;
}


static int ieee80211_set_gen_ie(struct net_device *dev, u8 *ie, size_t len)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_local *local = dev->ieee80211_ptr;

	if (local->user_space_mlme)
		return -EOPNOTSUPP;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_IF_TYPE_STA ||
	    sdata->type == IEEE80211_IF_TYPE_IBSS)
		return ieee80211_sta_set_extra_ie(dev, ie, len);

	if (sdata->type == IEEE80211_IF_TYPE_AP) {
		kfree(sdata->u.ap.generic_elem);
		sdata->u.ap.generic_elem = kmalloc(len, GFP_KERNEL);
		if (!sdata->u.ap.generic_elem)
			return -ENOMEM;
		memcpy(sdata->u.ap.generic_elem, ie, len);
		sdata->u.ap.generic_elem_len = len;
		return ieee80211_if_config(dev);
	}
	return -EOPNOTSUPP;
}


static int
ieee80211_ioctl_set_generic_info_elem(struct net_device *dev,
				      struct prism2_hostapd_param *param,
				      int param_len)
{
	u8 *pos = param->u.set_generic_info_elem.data;
        int left = param_len - ((u8 *) pos - (u8 *) param);
	int len = param->u.set_generic_info_elem.len;

	if (left < len)
		return -EINVAL;

	return ieee80211_set_gen_ie(dev, pos, len);
}


static int ieee80211_ioctl_set_regulatory_domain(struct net_device *dev,
					    struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_conf *conf = &local->hw.conf;
        conf->regulatory_domain = param->u.set_regulatory_domain.rd;
        return 0;
}


static int ieee80211_ioctl_set_radio_enabled(struct net_device *dev,
					     int val)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_conf *conf = &local->hw.conf;

	conf->radio_enabled = val;
	return ieee80211_hw_config(dev->ieee80211_ptr);
}

static int
ieee80211_ioctl_set_tx_queue_params(struct net_device *dev,
				    struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_tx_queue_params qparam;

	if (!local->ops->conf_tx) {
		printk(KERN_DEBUG "%s: low-level driver does not support TX "
		       "queue configuration\n", dev->name);
		return -EOPNOTSUPP;
	}

	memset(&qparam, 0, sizeof(qparam));
	qparam.aifs = param->u.tx_queue_params.aifs;
	qparam.cw_min = param->u.tx_queue_params.cw_min;
	qparam.cw_max = param->u.tx_queue_params.cw_max;
	qparam.burst_time = param->u.tx_queue_params.burst_time;

	return local->ops->conf_tx(local_to_hw(local),
				  param->u.tx_queue_params.queue,
				  &qparam);
}


static int ieee80211_ioctl_get_tx_stats(struct net_device *dev,
					struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_tx_queue_stats stats;
	int ret, i;

	if (!local->ops->get_tx_stats)
		return -EOPNOTSUPP;

	memset(&stats, 0, sizeof(stats));
	ret = local->ops->get_tx_stats(local_to_hw(local), &stats);
	if (ret)
		return ret;

	for (i = 0; i < 4; i++) {
		param->u.get_tx_stats.data[i].len = stats.data[i].len;
		param->u.get_tx_stats.data[i].limit = stats.data[i].limit;
		param->u.get_tx_stats.data[i].count = stats.data[i].count;
	}

	return 0;
}


static int ieee80211_ioctl_set_channel_flag(struct net_device *dev,
					    struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_hw_mode *mode;
	struct ieee80211_channel *chan = NULL;
	int i;

	list_for_each_entry(mode, &local->modes_list, list) {
		if (mode->mode == param->u.set_channel_flag.mode)
			goto found;
	}
	return -ENOENT;
found:

	for (i = 0; i < mode->num_channels; i++) {
		chan = &mode->channels[i];
		if (chan->chan == param->u.set_channel_flag.chan)
			break;
		chan = NULL;
	}

	if (!chan)
		return -ENOENT;

	chan->flag = param->u.set_channel_flag.flag;
        chan->power_level = param->u.set_channel_flag.power_level;
        chan->antenna_max = param->u.set_channel_flag.antenna_max;

	return 0;
}


static int ieee80211_ioctl_set_quiet_params(struct net_device *dev,
					    struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_conf *conf = &local->hw.conf;

	conf->quiet_duration = param->u.quiet.duration;
	conf->quiet_offset = param->u.quiet.offset;
	conf->quiet_period = param->u.quiet.period;
	return 0;
}


static int ieee80211_ioctl_set_radar_params(struct net_device *dev,
					    struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_conf *conf = &local->hw.conf;

	conf->radar_firpwr_threshold = param->u.radar.radar_firpwr_threshold;
	conf->radar_rssi_threshold = param->u.radar.radar_rssi_threshold;
	conf->pulse_height_threshold = param->u.radar.pulse_height_threshold;
	conf->pulse_rssi_threshold = param->u.radar.pulse_rssi_threshold;
	conf->pulse_inband_threshold = param->u.radar.pulse_inband_threshold;
	return 0;
}


static int ieee80211_ioctl_priv_hostapd(struct net_device *dev,
					struct iw_point *p)
{
	struct prism2_hostapd_param *param;
	int ret = 0;

	if (p->length < sizeof(struct prism2_hostapd_param) ||
	    p->length > PRISM2_HOSTAPD_MAX_BUF_SIZE || !p->pointer) {
		printk(KERN_DEBUG "%s: hostapd ioctl: ptr=%p len=%d min=%d "
		       "max=%d\n", dev->name, p->pointer, p->length,
		       (int)sizeof(struct prism2_hostapd_param),
		       PRISM2_HOSTAPD_MAX_BUF_SIZE);
		return -EINVAL;
	}

	param = (struct prism2_hostapd_param *) kmalloc(p->length, GFP_KERNEL);
	if (!param)
		return -ENOMEM;

	if (copy_from_user(param, p->pointer, p->length)) {
		ret = -EFAULT;
		goto out;
	}

	switch (param->cmd) {
	case PRISM2_HOSTAPD_FLUSH:
		ret = ieee80211_ioctl_flush(dev, param);
		break;
	case PRISM2_HOSTAPD_ADD_STA:
		ret = ieee80211_ioctl_add_sta(dev, param);
		break;
	case PRISM2_HOSTAPD_REMOVE_STA:
		ret = ieee80211_ioctl_remove_sta(dev, param);
		break;
	case PRISM2_HOSTAPD_GET_INFO_STA:
		ret = ieee80211_ioctl_get_info_sta(dev, param);
		break;
	case PRISM2_SET_ENCRYPTION:
		ret = ieee80211_ioctl_set_encryption(dev, param, p->length);
		break;
	case PRISM2_GET_ENCRYPTION:
		ret = ieee80211_ioctl_get_encryption(dev, param, p->length);
		break;
	case PRISM2_HOSTAPD_SET_FLAGS_STA:
		ret = ieee80211_ioctl_set_flags_sta(dev, param);
		break;
	case PRISM2_HOSTAPD_SET_BEACON:
		ret = ieee80211_ioctl_set_beacon(dev, param, p->length, 0);
		break;
	case PRISM2_HOSTAPD_GET_HW_FEATURES:
		ret = ieee80211_ioctl_get_hw_features(dev, param, p->length);
		break;
        case PRISM2_HOSTAPD_SCAN:
		ret = ieee80211_ioctl_scan(dev, param);
		break;
#ifdef CONFIG_HOSTAPD_WPA_TESTING
        case PRISM2_HOSTAPD_WPA_TRIGGER:
		ret = ieee80211_ioctl_wpa_trigger(dev, param);
		break;
#endif /* CONFIG_HOSTAPD_WPA_TESTING */
        case PRISM2_HOSTAPD_SET_RATE_SETS:
		ret = ieee80211_ioctl_set_rate_sets(dev, param, p->length);
		break;
	case PRISM2_HOSTAPD_ADD_IF:
                ret = ieee80211_ioctl_add_if(dev, param, p->length);
                break;
	case PRISM2_HOSTAPD_REMOVE_IF:
                ret = ieee80211_ioctl_remove_if(dev, param);
                break;
	case PRISM2_HOSTAPD_GET_DOT11COUNTERSTABLE:
		ret = ieee80211_ioctl_get_dot11counterstable(dev, param);
		break;
        case PRISM2_HOSTAPD_GET_LOAD_STATS:
		ret = ieee80211_ioctl_get_load_stats(dev, param);
		break;
        case PRISM2_HOSTAPD_SET_STA_VLAN:
                ret = ieee80211_ioctl_set_sta_vlan(dev, param);
		break;
        case PRISM2_HOSTAPD_SET_GENERIC_INFO_ELEM:
                ret = ieee80211_ioctl_set_generic_info_elem(dev, param,
							    p->length);
		break;
        case PRISM2_HOSTAPD_SET_CHANNEL_FLAG:
		ret = ieee80211_ioctl_set_channel_flag(dev, param);
		break;
        case PRISM2_HOSTAPD_SET_REGULATORY_DOMAIN:
		ret = ieee80211_ioctl_set_regulatory_domain(dev, param);
		break;
	case PRISM2_HOSTAPD_SET_TX_QUEUE_PARAMS:
		ret = ieee80211_ioctl_set_tx_queue_params(dev, param);
		break;
	case PRISM2_HOSTAPD_GET_TX_STATS:
		ret = ieee80211_ioctl_get_tx_stats(dev, param);
		break;
	case PRISM2_HOSTAPD_UPDATE_IF:
                ret = ieee80211_ioctl_update_if(dev, param, p->length);
                break;
	case PRISM2_HOSTAPD_SCAN_REQ:
                ret = ieee80211_ioctl_scan_req(dev, param, p->length);
                break;
	case PRISM2_STA_GET_STATE:
		ret = ieee80211_ioctl_sta_get_state(dev, param);
		break;
	case PRISM2_HOSTAPD_MLME:
		ret = ieee80211_ioctl_mlme(dev, param);
		break;
	case PRISM2_HOSTAPD_FLUSH_IFS:
		ret = ieee80211_ioctl_flush_ifs(dev, param);
		break;
	case PRISM2_HOSTAPD_SET_RADAR_PARAMS:
		ret = ieee80211_ioctl_set_radar_params(dev, param);
		break;
	case PRISM2_HOSTAPD_SET_QUIET_PARAMS:
		ret = ieee80211_ioctl_set_quiet_params(dev, param);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}

	if (copy_to_user(p->pointer, param, p->length))
		ret = -EFAULT;

 out:
	kfree(param);

	return ret;
}


static int ieee80211_ioctl_giwname(struct net_device *dev,
				   struct iw_request_info *info,
				   char *name, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	switch (local->hw.conf.phymode) {
	case MODE_IEEE80211A:
		strcpy(name, "IEEE 802.11a");
		break;
	case MODE_IEEE80211B:
		strcpy(name, "IEEE 802.11b");
		break;
	case MODE_IEEE80211G:
		strcpy(name, "IEEE 802.11g");
		break;
	case MODE_ATHEROS_TURBO:
		strcpy(name, "5GHz Turbo");
		break;
	default:
		strcpy(name, "IEEE 802.11");
		break;
	}

	return 0;
}


static int ieee80211_ioctl_giwrange(struct net_device *dev,
				 struct iw_request_info *info,
				 struct iw_point *data, char *extra)
{
	struct iw_range *range = (struct iw_range *) extra;

	data->length = sizeof(struct iw_range);
	memset(range, 0, sizeof(struct iw_range));

	range->we_version_compiled = WIRELESS_EXT;
	range->we_version_source = 14;
	range->retry_capa = IW_RETRY_LIMIT;
	range->retry_flags = IW_RETRY_LIMIT;
	range->min_retry = 0;
	range->max_retry = 255;
	range->min_rts = 0;
	range->max_rts = 2347;
	range->min_frag = 256;
	range->max_frag = 2346;

	range->max_qual.qual = 100;
	range->max_qual.level = 146;  /* set floor at -110 dBm (146 - 256) */
	range->max_qual.noise = 146;
	range->max_qual.updated = IW_QUAL_ALL_UPDATED;

	range->avg_qual.qual = 50;
	range->avg_qual.level = 0;
	range->avg_qual.noise = 0;
	range->avg_qual.updated = IW_QUAL_ALL_UPDATED;

	return 0;
}


struct ieee80211_channel_range {
	short start_freq;
	short end_freq;
	unsigned char power_level;
	unsigned char antenna_max;
};

static const struct ieee80211_channel_range ieee80211_fcc_channels[] = {
	{ 2412, 2462, 27, 6 } /* IEEE 802.11b/g, channels 1..11 */,
	{ 5180, 5240, 17, 6 } /* IEEE 802.11a, channels 36..48 */,
	{ 5260, 5320, 23, 6 } /* IEEE 802.11a, channels 52..64 */,
	{ 5745, 5825, 30, 6 } /* IEEE 802.11a, channels 149..165, outdoor */,
	{ 0 }
};

static const struct ieee80211_channel_range ieee80211_mkk_channels[] = {
	{ 2412, 2472, 20, 6 } /* IEEE 802.11b/g, channels 1..13 */,
	{ 5170, 5240, 20, 6 } /* IEEE 802.11a, channels 34..48 */,
	{ 5260, 5320, 20, 6 } /* IEEE 802.11a, channels 52..64 */,
	{ 0 }
};


static const struct ieee80211_channel_range *channel_range =
	ieee80211_fcc_channels;


static void ieee80211_unmask_channel(struct net_device *dev, int mode,
				     struct ieee80211_channel *chan)
{
	int i;

	chan->flag = 0;

	if (ieee80211_regdom == 64 &&
	    (mode == MODE_ATHEROS_TURBO || mode == MODE_ATHEROS_TURBOG)) {
		/* Do not allow Turbo modes in Japan. */
		return;
	}

	for (i = 0; channel_range[i].start_freq; i++) {
		const struct ieee80211_channel_range *r = &channel_range[i];
		if (r->start_freq <= chan->freq && r->end_freq >= chan->freq) {
			if (ieee80211_regdom == 64 && !ieee80211_japan_5ghz &&
			    chan->freq >= 5260 && chan->freq <= 5320) {
				/*
				 * Skip new channels in Japan since the
				 * firmware was not marked having been upgraded
				 * by the vendor.
				 */
				continue;
			}

			if (ieee80211_regdom == 0x10 &&
			    (chan->freq == 5190 || chan->freq == 5210 ||
			     chan->freq == 5230)) {
				    /* Skip MKK channels when in FCC domain. */
				    continue;
			}

			chan->flag |= IEEE80211_CHAN_W_SCAN |
				IEEE80211_CHAN_W_ACTIVE_SCAN |
				IEEE80211_CHAN_W_IBSS;
			chan->power_level = r->power_level;
			chan->antenna_max = r->antenna_max;

			if (ieee80211_regdom == 64 &&
			    (chan->freq == 5170 || chan->freq == 5190 ||
			     chan->freq == 5210 || chan->freq == 5230)) {
				/*
				 * New regulatory rules in Japan have backwards
				 * compatibility with old channels in 5.15-5.25
				 * GHz band, but the station is not allowed to
				 * use active scan on these old channels.
				 */
				chan->flag &= ~IEEE80211_CHAN_W_ACTIVE_SCAN;
			}

			if (ieee80211_regdom == 64 &&
			    (chan->freq == 5260 || chan->freq == 5280 ||
			     chan->freq == 5300 || chan->freq == 5320)) {
				/*
				 * IBSS is not allowed on 5.25-5.35 GHz band
				 * due to radar detection requirements.
				 */
				chan->flag &= ~IEEE80211_CHAN_W_IBSS;
			}

			break;
		}
	}
}


static int ieee80211_unmask_channels(struct net_device *dev)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_hw_mode *mode;
	int c;

	list_for_each_entry(mode, &local->modes_list, list) {
		for (c = 0; c < mode->num_channels; c++) {
			ieee80211_unmask_channel(dev, mode->mode,
						 &mode->channels[c]);
		}
	}
	return 0;
}


int ieee80211_init_client(struct net_device *dev)
{
	if (ieee80211_regdom == 0x40)
		channel_range = ieee80211_mkk_channels;
	ieee80211_unmask_channels(dev);
	return 0;
}


static int ieee80211_ioctl_siwmode(struct net_device *dev,
				   struct iw_request_info *info,
				   __u32 *mode, char *extra)
{
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	int type;

	if (sdata->type == IEEE80211_IF_TYPE_VLAN)
		return -EOPNOTSUPP;
	if (netif_running(dev))
		return -EBUSY;

	switch (*mode) {
	case IW_MODE_MASTER:
		type = IEEE80211_IF_TYPE_AP;
		break;
	case IW_MODE_INFRA:
		type = IEEE80211_IF_TYPE_STA;
		break;
	case IW_MODE_ADHOC:
		type = IEEE80211_IF_TYPE_IBSS;
		break;
	case IW_MODE_MONITOR:
		type = IEEE80211_IF_TYPE_MNTR;
		break;
	case IW_MODE_REPEAT:
		type = IEEE80211_IF_TYPE_WDS;
		break;
	default:
		return -EINVAL;
	}

	if (type != sdata->type) {
		ieee80211_if_reinit(dev);
		ieee80211_if_set_type(dev, type);
	}
	return 0;
}


static int ieee80211_ioctl_giwmode(struct net_device *dev,
				   struct iw_request_info *info,
				   __u32 *mode, char *extra)
{
	struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	switch (sdata->type) {
	case IEEE80211_IF_TYPE_AP:
		*mode = IW_MODE_MASTER;
		break;
	case IEEE80211_IF_TYPE_STA:
		*mode = IW_MODE_INFRA;
		break;
	case IEEE80211_IF_TYPE_IBSS:
		*mode = IW_MODE_ADHOC;
		break;
	case IEEE80211_IF_TYPE_MNTR:
		*mode = IW_MODE_MONITOR;
		break;
	case IEEE80211_IF_TYPE_WDS:
		*mode = IW_MODE_REPEAT;
		break;
	case IEEE80211_IF_TYPE_VLAN:
		*mode = IW_MODE_SECOND;		/* FIXME */
		break;
	default:
		*mode = IW_MODE_AUTO;
		break;
	}
	return 0;
}


int ieee80211_ioctl_siwfreq(struct net_device *dev,
			    struct iw_request_info *info,
			    struct iw_freq *freq, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_hw_mode *mode;
	int c, nfreq, set = 0;

	/* freq->e == 0: freq->m = channel; otherwise freq = m * 10^e */
	if (freq->e == 0)
		nfreq = -1;
	else {
		int i, div = 1000000;
		for (i = 0; i < freq->e; i++)
			div /= 10;
		if (div > 0)
			nfreq = freq->m / div;
		else
			return -EINVAL;
	}

	list_for_each_entry(mode, &local->modes_list, list) {
		for (c = 0; c < mode->num_channels; c++) {
			struct ieee80211_channel *chan = &mode->channels[c];
			if (chan->flag & IEEE80211_CHAN_W_SCAN &&
			    ((freq->e == 0 && chan->chan == freq->m) ||
			     (freq->e > 0 && nfreq == chan->freq)) &&
			    (local->enabled_modes & (1 << mode->mode))) {
				/* Use next_mode as the mode preference to
				 * resolve non-unique channel numbers. */
				if (set && mode->mode != local->next_mode)
					continue;

				local->hw.conf.channel = chan->chan;
				local->hw.conf.channel_val = chan->val;
                                local->hw.conf.power_level = chan->power_level;
				local->hw.conf.freq = chan->freq;
				local->hw.conf.phymode = mode->mode;
                                local->hw.conf.antenna_max = chan->antenna_max;
				set++;
			}
		}
	}

	if (set) {
		local->sta_scanning = 0; /* Abort possible scan */
		return ieee80211_hw_config(local);
	}

	return -EINVAL;
}


static int ieee80211_ioctl_giwfreq(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_freq *freq, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	/* TODO: in station mode (Managed/Ad-hoc) might need to poll low-level
	 * driver for the current channel with firmware-based management */

	freq->m = local->hw.conf.freq;
	freq->e = 6;

	return 0;
}


static int ieee80211_ioctl_siwessid(struct net_device *dev,
				    struct iw_request_info *info,
				    struct iw_point *data, char *ssid)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata;
        size_t len = data->length;

	/* iwconfig uses nul termination in SSID.. */
	if (len > 0 && ssid[len - 1] == '\0')
		len--;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_IF_TYPE_STA ||
	    sdata->type == IEEE80211_IF_TYPE_IBSS) {
		if (local->user_space_mlme) {
			if (len > IEEE80211_MAX_SSID_LEN)
				return -EINVAL;
			memcpy(sdata->u.sta.ssid, ssid, len);
			sdata->u.sta.ssid_len = len;
			return 0;
		}
		return ieee80211_sta_set_ssid(dev, ssid, len);
	}

	if (sdata->type == IEEE80211_IF_TYPE_AP) {
		memcpy(sdata->u.ap.ssid, ssid, len);
		memset(sdata->u.ap.ssid + len, 0,
		       IEEE80211_MAX_SSID_LEN - len);
		sdata->u.ap.ssid_len = len;
		return ieee80211_if_config(dev);
	}
	return -EOPNOTSUPP;
}


static int ieee80211_ioctl_giwessid(struct net_device *dev,
				    struct iw_request_info *info,
				    struct iw_point *data, char *ssid)
{
	size_t len;

	struct ieee80211_sub_if_data *sdata;
	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_IF_TYPE_STA ||
	    sdata->type == IEEE80211_IF_TYPE_IBSS) {
		int res = ieee80211_sta_get_ssid(dev, ssid, &len);
		if (res == 0) {
			data->length = len;
			data->flags = 1;
		} else
			data->flags = 0;
		return res;
	}

	if (sdata->type == IEEE80211_IF_TYPE_AP) {
		len = sdata->u.ap.ssid_len;
		if (len > IW_ESSID_MAX_SIZE)
			len = IW_ESSID_MAX_SIZE;
		memcpy(ssid, sdata->u.ap.ssid, len);
		data->length = len;
		data->flags = 1;
		return 0;
	}
	return -EOPNOTSUPP;
}


static int ieee80211_ioctl_siwap(struct net_device *dev,
				 struct iw_request_info *info,
				 struct sockaddr *ap_addr, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_IF_TYPE_STA ||
	    sdata->type == IEEE80211_IF_TYPE_IBSS) {
		if (local->user_space_mlme) {
			memcpy(sdata->u.sta.bssid, (u8 *) &ap_addr->sa_data,
			       ETH_ALEN);
			return 0;
		}
		return ieee80211_sta_set_bssid(dev, (u8 *) &ap_addr->sa_data);
	} else if (sdata->type == IEEE80211_IF_TYPE_WDS) {
		if (memcmp(sdata->u.wds.remote_addr, (u8 *) &ap_addr->sa_data,
			   ETH_ALEN) == 0)
			return 0;
		return ieee80211_if_update_wds(dev, (u8 *) &ap_addr->sa_data);
	}

	return -EOPNOTSUPP;
}


static int ieee80211_ioctl_giwap(struct net_device *dev,
				 struct iw_request_info *info,
				 struct sockaddr *ap_addr, char *extra)
{
        struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_IF_TYPE_STA ||
	    sdata->type == IEEE80211_IF_TYPE_IBSS) {
		ap_addr->sa_family = ARPHRD_ETHER;
		memcpy(&ap_addr->sa_data, sdata->u.sta.bssid, ETH_ALEN);
		return 0;
	} else if (sdata->type == IEEE80211_IF_TYPE_WDS) {
		ap_addr->sa_family = ARPHRD_ETHER;
		memcpy(&ap_addr->sa_data, sdata->u.wds.remote_addr, ETH_ALEN);
		return 0;
	}

	return -EOPNOTSUPP;
}


static int ieee80211_ioctl_siwscan(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_point *data, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	u8 *ssid = NULL;
	size_t ssid_len = 0;

	if (!netif_running(dev))
		return -ENETDOWN;

	if (local->scan_flags & IEEE80211_SCAN_MATCH_SSID) {
		if (sdata->type == IEEE80211_IF_TYPE_STA ||
		    sdata->type == IEEE80211_IF_TYPE_IBSS) {
			ssid = sdata->u.sta.ssid;
			ssid_len = sdata->u.sta.ssid_len;
		} else if (sdata->type == IEEE80211_IF_TYPE_AP) {
			ssid = sdata->u.ap.ssid;
			ssid_len = sdata->u.ap.ssid_len;
		} else
			return -EINVAL;
	}
	return ieee80211_sta_req_scan(dev, ssid, ssid_len);
}


static int ieee80211_ioctl_giwscan(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_point *data, char *extra)
{
	int res;
	struct ieee80211_local *local = dev->ieee80211_ptr;
	if (local->sta_scanning)
		return -EAGAIN;
	res = ieee80211_sta_scan_results(dev, extra, data->length);
	if (res >= 0) {
		data->length = res;
		return 0;
	}
	data->length = 0;
	return res;
}


static int ieee80211_ioctl_siwrts(struct net_device *dev,
				  struct iw_request_info *info,
				  struct iw_param *rts, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	if (rts->disabled)
		local->rts_threshold = IEEE80211_MAX_RTS_THRESHOLD;
	else if (rts->value < 0 || rts->value > IEEE80211_MAX_RTS_THRESHOLD)
		return -EINVAL;
	else
		local->rts_threshold = rts->value;

	/* If the wlan card performs RTS/CTS in hardware/firmware,
	 * configure it here */

	if (local->ops->set_rts_threshold)
		local->ops->set_rts_threshold(local_to_hw(local),
					     local->rts_threshold);

	return 0;
}

static int ieee80211_ioctl_giwrts(struct net_device *dev,
				  struct iw_request_info *info,
				  struct iw_param *rts, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	rts->value = local->rts_threshold;
	rts->disabled = (rts->value >= IEEE80211_MAX_RTS_THRESHOLD);
	rts->fixed = 1;

	return 0;
}


static int ieee80211_ioctl_siwfrag(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_param *frag, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	if (frag->disabled)
		local->fragmentation_threshold = IEEE80211_MAX_FRAG_THRESHOLD;
	else if (frag->value < 256 ||
		 frag->value > IEEE80211_MAX_FRAG_THRESHOLD)
		return -EINVAL;
	else {
		/* Fragment length must be even, so strip LSB. */
		local->fragmentation_threshold = frag->value & ~0x1;
	}

	/* If the wlan card performs fragmentation in hardware/firmware,
	 * configure it here */

	if (local->ops->set_frag_threshold)
		local->ops->set_frag_threshold(
			local_to_hw(local),
			local->fragmentation_threshold);

	return 0;
}

static int ieee80211_ioctl_giwfrag(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_param *frag, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	frag->value = local->fragmentation_threshold;
	frag->disabled = (frag->value >= IEEE80211_MAX_RTS_THRESHOLD);
	frag->fixed = 1;

	return 0;
}


static int ieee80211_ioctl_siwretry(struct net_device *dev,
				    struct iw_request_info *info,
				    struct iw_param *retry, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	if (retry->disabled ||
	    (retry->flags & IW_RETRY_TYPE) != IW_RETRY_LIMIT)
		return -EINVAL;

	if (retry->flags & IW_RETRY_MAX)
		local->long_retry_limit = retry->value;
	else if (retry->flags & IW_RETRY_MIN)
		local->short_retry_limit = retry->value;
	else {
		local->long_retry_limit = retry->value;
		local->short_retry_limit = retry->value;
	}

	if (local->ops->set_retry_limit) {
		return local->ops->set_retry_limit(
			local_to_hw(local),
			local->short_retry_limit,
			local->long_retry_limit);
	}

	return 0;
}


static int ieee80211_ioctl_giwretry(struct net_device *dev,
				    struct iw_request_info *info,
				    struct iw_param *retry, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;

	retry->disabled = 0;
	if ((retry->flags & IW_RETRY_TYPE) != IW_RETRY_LIMIT)
		return -EINVAL;
	if (retry->flags & IW_RETRY_MAX) {
		retry->flags = IW_RETRY_LIMIT | IW_RETRY_MAX;
		retry->value = local->long_retry_limit;
	} else {
		retry->flags = IW_RETRY_LIMIT;
		retry->value = local->short_retry_limit;
		if (local->long_retry_limit != local->short_retry_limit)
			retry->flags |= IW_RETRY_MIN;
	}

	return 0;
}


static void ieee80211_ioctl_unmask_channels(struct ieee80211_local *local)
{
	struct ieee80211_hw_mode *mode;
	int c;

	list_for_each_entry(mode, &local->modes_list, list) {
		for (c = 0; c < mode->num_channels; c++) {
			struct ieee80211_channel *chan = &mode->channels[c];
			chan->flag |= IEEE80211_CHAN_W_SCAN;
		}
	}
}


static int ieee80211_ioctl_test_mode(struct net_device *dev, int mode)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	int ret = -EOPNOTSUPP;

	if (mode == IEEE80211_TEST_UNMASK_CHANNELS) {
		ieee80211_ioctl_unmask_channels(local);
		ret = 0;
	}

	if (local->ops->test_mode)
		ret = local->ops->test_mode(local_to_hw(local), mode);

	return ret;
}


static int ieee80211_ioctl_clear_keys(struct net_device *dev)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_key_conf key;
	int i;
	u8 addr[ETH_ALEN];
	struct ieee80211_key_conf *keyconf;
	struct ieee80211_sub_if_data *sdata;
	struct sta_info *sta;

	memset(addr, 0xff, ETH_ALEN);
	list_for_each_entry(sdata, &local->sub_if_list, list) {
		for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
			keyconf = NULL;
			if (sdata->keys[i] &&
			    !sdata->keys[i]->force_sw_encrypt &&
			    local->ops->set_key &&
			    (keyconf = ieee80211_key_data2conf(local,
							       sdata->keys[i])))
				local->ops->set_key(local_to_hw(local),
						   DISABLE_KEY, addr,
						   keyconf, 0);
			kfree(keyconf);
			ieee80211_key_free(sdata->keys[i]);
			sdata->keys[i] = NULL;
		}
		sdata->default_key = NULL;
	}

	spin_lock_bh(&local->sta_lock);
	list_for_each_entry(sta, &local->sta_list, list) {
		keyconf = NULL;
		if (sta->key && !sta->key->force_sw_encrypt &&
		    local->ops->set_key &&
		    (keyconf = ieee80211_key_data2conf(local, sta->key)))
			local->ops->set_key(local_to_hw(local), DISABLE_KEY,
					   sta->addr, keyconf, sta->aid);
		kfree(keyconf);
		ieee80211_key_free(sta->key);
		sta->key = NULL;
	}
	spin_unlock_bh(&local->sta_lock);

	memset(&key, 0, sizeof(key));
	if (local->ops->set_key &&
		    local->ops->set_key(local_to_hw(local), REMOVE_ALL_KEYS,
				       NULL, &key, 0))
		printk(KERN_DEBUG "%s: failed to remove hwaccel keys\n",
		       dev->name);

	return 0;
}


static int
ieee80211_ioctl_force_unicast_rate(struct net_device *dev,
				   struct ieee80211_sub_if_data *sdata,
				   int rate)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	int i;

	if (sdata->type != IEEE80211_IF_TYPE_AP)
		return -ENOENT;

	if (rate == 0) {
		sdata->u.ap.force_unicast_rateidx = -1;
		return 0;
	}

	for (i = 0; i < local->num_curr_rates; i++) {
		if (local->curr_rates[i].rate == rate) {
			sdata->u.ap.force_unicast_rateidx = i;
			return 0;
		}
	}
	return -EINVAL;
}


static int
ieee80211_ioctl_max_ratectrl_rate(struct net_device *dev,
				  struct ieee80211_sub_if_data *sdata,
				  int rate)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	int i;

	if (sdata->type != IEEE80211_IF_TYPE_AP)
		return -ENOENT;

	if (rate == 0) {
		sdata->u.ap.max_ratectrl_rateidx = -1;
		return 0;
	}

	for (i = 0; i < local->num_curr_rates; i++) {
		if (local->curr_rates[i].rate == rate) {
			sdata->u.ap.max_ratectrl_rateidx = i;
			return 0;
		}
	}
	return -EINVAL;
}


static void ieee80211_key_enable_hwaccel(struct ieee80211_local *local,
					 struct ieee80211_key *key)
{
	struct ieee80211_key_conf *keyconf;
	u8 addr[ETH_ALEN];

	if (!key || key->alg != ALG_WEP || !key->force_sw_encrypt ||
	    (local->hw.flags & IEEE80211_HW_DEVICE_HIDES_WEP))
		return;

	memset(addr, 0xff, ETH_ALEN);
	keyconf = ieee80211_key_data2conf(local, key);
	if (keyconf && local->ops->set_key &&
	    local->ops->set_key(local_to_hw(local),
	    		       SET_KEY, addr, keyconf, 0) == 0) {
		key->force_sw_encrypt =
			!!(keyconf->flags & IEEE80211_KEY_FORCE_SW_ENCRYPT);
		key->hw_key_idx = keyconf->hw_key_idx;
	}
	kfree(keyconf);
}


static void ieee80211_key_disable_hwaccel(struct ieee80211_local *local,
					  struct ieee80211_key *key)
{
	struct ieee80211_key_conf *keyconf;
	u8 addr[ETH_ALEN];

	if (!key || key->alg != ALG_WEP || key->force_sw_encrypt ||
	    (local->hw.flags & IEEE80211_HW_DEVICE_HIDES_WEP))
		return;

	memset(addr, 0xff, ETH_ALEN);
	keyconf = ieee80211_key_data2conf(local, key);
	if (keyconf && local->ops->set_key)
		local->ops->set_key(local_to_hw(local), DISABLE_KEY,
				   addr, keyconf, 0);
	kfree(keyconf);
	key->force_sw_encrypt = 1;
}


static int ieee80211_ioctl_default_wep_only(struct ieee80211_local *local,
					    int value)
{
	int i;
	struct ieee80211_sub_if_data *sdata;

	local->default_wep_only = value;
	list_for_each_entry(sdata, &local->sub_if_list, list)
		for (i = 0; i < NUM_DEFAULT_KEYS; i++)
			if (value)
				ieee80211_key_enable_hwaccel(local,
							     sdata->keys[i]);
			else
				ieee80211_key_disable_hwaccel(local,
							      sdata->keys[i]);

	return 0;
}


void ieee80211_update_default_wep_only(struct ieee80211_local *local)
{
	int i = 0;
	struct ieee80211_sub_if_data *sdata;

	spin_lock_bh(&local->sub_if_lock);
	list_for_each_entry(sdata, &local->sub_if_list, list) {

		if (sdata->dev == local->mdev)
			continue;

		/* If there is an AP interface then depend on userspace to
		   set default_wep_only correctly. */
		if (sdata->type == IEEE80211_IF_TYPE_AP) {
			spin_unlock_bh(&local->sub_if_lock);
			return;
		}

		i++;
	}

	if (i <= 1)
		ieee80211_ioctl_default_wep_only(local, 1);
	else
		ieee80211_ioctl_default_wep_only(local, 0);

	spin_unlock_bh(&local->sub_if_lock);
}


static int ieee80211_ioctl_prism2_param(struct net_device *dev,
					struct iw_request_info *info,
					void *wrqu, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata;
	int *i = (int *) extra;
	int param = *i;
	int value = *(i + 1);
	int ret = 0;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	switch (param) {
	case PRISM2_PARAM_HOST_ENCRYPT:
	case PRISM2_PARAM_HOST_DECRYPT:
		/* TODO: implement these; return success now to prevent
		 * hostapd from aborting */
		break;

	case PRISM2_PARAM_BEACON_INT:
		local->hw.conf.beacon_int = value;
		if (ieee80211_hw_config(local))
			ret = -EINVAL;
		break;

	case PRISM2_PARAM_AP_BRIDGE_PACKETS:
		local->bridge_packets = value;
		break;

	case PRISM2_PARAM_AP_AUTH_ALGS:
		if (sdata->type == IEEE80211_IF_TYPE_STA ||
		    sdata->type == IEEE80211_IF_TYPE_IBSS) {
			sdata->u.sta.auth_algs = value;
		} else
			ret = -EOPNOTSUPP;
		break;

	case PRISM2_PARAM_DTIM_PERIOD:
		if (value < 1)
			ret = -EINVAL;
		else if (sdata->type != IEEE80211_IF_TYPE_AP)
			ret = -ENOENT;
		else
			sdata->u.ap.dtim_period = value;
		break;

	case PRISM2_PARAM_IEEE_802_1X:
		sdata->ieee802_1x = value;
		if (local->ops->set_ieee8021x &&
		    local->ops->set_ieee8021x(local_to_hw(local), value))
			printk(KERN_DEBUG "%s: failed to set IEEE 802.1X (%d) "
			       "for low-level driver\n", dev->name, value);
		break;

	case PRISM2_PARAM_CTS_PROTECT_ERP_FRAMES:
		local->cts_protect_erp_frames = value;
		break;

	case PRISM2_PARAM_DROP_UNENCRYPTED:
		sdata->drop_unencrypted = value;
		break;

	case PRISM2_PARAM_PREAMBLE:
		local->short_preamble = value;
		break;

        case PRISM2_PARAM_STAT_TIME:
                if (!local->stat_time && value) {
                        local->stat_timer.expires = jiffies + HZ * value / 100;
                        add_timer(&local->stat_timer);
                } else if (local->stat_time && !value) {
                        del_timer_sync(&local->stat_timer);
                }
                local->stat_time = value;
                break;
	case PRISM2_PARAM_SHORT_SLOT_TIME:
		if (value)
			local->hw.conf.flags |= IEEE80211_CONF_SHORT_SLOT_TIME;
		else
			local->hw.conf.flags &= ~IEEE80211_CONF_SHORT_SLOT_TIME;
		if (ieee80211_hw_config(local))
			ret = -EINVAL;
		break;

        case PRISM2_PARAM_PRIVACY_INVOKED:
		if (local->ops->set_privacy_invoked)
			ret = local->ops->set_privacy_invoked(
					local_to_hw(local), value);
		break;

	case PRISM2_PARAM_TEST_MODE:
		ret = ieee80211_ioctl_test_mode(dev, value);
		break;

	case PRISM2_PARAM_NEXT_MODE:
		local->next_mode = value;
		break;

	case PRISM2_PARAM_CLEAR_KEYS:
		ret = ieee80211_ioctl_clear_keys(dev);
		break;

	case PRISM2_PARAM_RADIO_ENABLED:
		ret = ieee80211_ioctl_set_radio_enabled(dev, value);
		break;

	case PRISM2_PARAM_ANTENNA_SEL:
		local->hw.conf.antenna_sel = value;
		if (ieee80211_hw_config(local))
			ret = -EINVAL;
		break;

	case PRISM2_PARAM_ANTENNA_MODE:
		local->hw.conf.antenna_mode = value;
		if (ieee80211_hw_config(local))
			ret = -EINVAL;
		break;

	case PRISM2_PARAM_BROADCAST_SSID:
		if ((value < 0) || (value > 1))
			ret = -EINVAL;
		else if (value)
			local->hw.conf.flags |= IEEE80211_CONF_SSID_HIDDEN;
		else
			local->hw.conf.flags &= ~IEEE80211_CONF_SSID_HIDDEN;
		break;

	case PRISM2_PARAM_STA_ANTENNA_SEL:
		local->sta_antenna_sel = value;
		break;

	case PRISM2_PARAM_FORCE_UNICAST_RATE:
		ret = ieee80211_ioctl_force_unicast_rate(dev, sdata, value);
		break;

	case PRISM2_PARAM_MAX_RATECTRL_RATE:
		ret = ieee80211_ioctl_max_ratectrl_rate(dev, sdata, value);
		break;

	case PRISM2_PARAM_RATE_CTRL_NUM_UP:
		local->rate_ctrl_num_up = value;
		break;

	case PRISM2_PARAM_RATE_CTRL_NUM_DOWN:
		local->rate_ctrl_num_down = value;
		break;

	case PRISM2_PARAM_TX_POWER_REDUCTION:
		if (value < 0)
			ret = -EINVAL;
		else
			local->hw.conf.tx_power_reduction = value;
		break;

	case PRISM2_PARAM_EAPOL:
		sdata->eapol = value;
		break;

	case PRISM2_PARAM_KEY_TX_RX_THRESHOLD:
		local->key_tx_rx_threshold = value;
		break;

	case PRISM2_PARAM_KEY_INDEX:
		if (value < 0 || value >= NUM_DEFAULT_KEYS)
			ret = -EINVAL;
		else if (!sdata->keys[value])
			ret = -ENOENT;
		else
			sdata->default_key = sdata->keys[value];
		break;

	case PRISM2_PARAM_DEFAULT_WEP_ONLY:
		ret = ieee80211_ioctl_default_wep_only(local, value);
		break;

	case PRISM2_PARAM_WIFI_WME_NOACK_TEST:
		local->wifi_wme_noack_test = value;
		break;

	case PRISM2_PARAM_ALLOW_BROADCAST_ALWAYS:
		local->allow_broadcast_always = value;
		break;

	case PRISM2_PARAM_SCAN_FLAGS:
		local->scan_flags = value;
		break;

	case PRISM2_PARAM_MIXED_CELL:
		if (sdata->type != IEEE80211_IF_TYPE_STA &&
		    sdata->type != IEEE80211_IF_TYPE_IBSS)
			ret = -EINVAL;
		else
			sdata->u.sta.mixed_cell = !!value;
		break;

	case PRISM2_PARAM_KEY_MGMT:
		if (sdata->type != IEEE80211_IF_TYPE_STA)
			ret = -EINVAL;
		else
			sdata->u.sta.key_mgmt = value;
		break;

	case PRISM2_PARAM_HW_MODES:
		local->enabled_modes = value;
		break;

	case PRISM2_PARAM_CREATE_IBSS:
		if (sdata->type != IEEE80211_IF_TYPE_IBSS)
			ret = -EINVAL;
		else
			sdata->u.sta.create_ibss = !!value;
		break;
	case PRISM2_PARAM_WMM_ENABLED:
		if (sdata->type != IEEE80211_IF_TYPE_STA &&
		    sdata->type != IEEE80211_IF_TYPE_IBSS)
			ret = -EINVAL;
		else
			sdata->u.sta.wmm_enabled = !!value;
		break;
	case PRISM2_PARAM_RADAR_DETECT:
		local->hw.conf.radar_detect = value;
		break;
	case PRISM2_PARAM_SPECTRUM_MGMT:
		local->hw.conf.spect_mgmt = value;
		break;
	case PRISM2_PARAM_MGMT_IF:
		if (value == 1) {
			if (!local->apdev)
				ret = ieee80211_if_add_mgmt(local);
		} else if (value == 0) {
			if (local->apdev)
				ieee80211_if_del_mgmt(local);
		} else
			ret = -EINVAL;
		break;
	case PRISM2_PARAM_USER_SPACE_MLME:
		local->user_space_mlme = value;
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}

	return ret;
}


static int ieee80211_ioctl_get_prism2_param(struct net_device *dev,
					    struct iw_request_info *info,
					    void *wrqu, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata;
	int *param = (int *) extra;
	int ret = 0;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	switch (*param) {
	case PRISM2_PARAM_BEACON_INT:
		*param = local->hw.conf.beacon_int;
		break;

	case PRISM2_PARAM_AP_BRIDGE_PACKETS:
		*param = local->bridge_packets;
		break;

	case PRISM2_PARAM_AP_AUTH_ALGS:
		if (sdata->type == IEEE80211_IF_TYPE_STA ||
		    sdata->type == IEEE80211_IF_TYPE_IBSS) {
			*param = sdata->u.sta.auth_algs;
		} else
			ret = -EOPNOTSUPP;
		break;

	case PRISM2_PARAM_DTIM_PERIOD:
		if (sdata->type != IEEE80211_IF_TYPE_AP)
			ret = -ENOENT;
		else
			*param = sdata->u.ap.dtim_period;
		break;

	case PRISM2_PARAM_IEEE_802_1X:
		*param = sdata->ieee802_1x;
		break;

	case PRISM2_PARAM_CTS_PROTECT_ERP_FRAMES:
		*param = local->cts_protect_erp_frames;
		break;

	case PRISM2_PARAM_DROP_UNENCRYPTED:
		*param = sdata->drop_unencrypted;
		break;

	case PRISM2_PARAM_PREAMBLE:
		*param = local->short_preamble;
		break;

        case PRISM2_PARAM_STAT_TIME:
                *param = local->stat_time;
                break;
	case PRISM2_PARAM_SHORT_SLOT_TIME:
		*param = !!(local->hw.conf.flags & IEEE80211_CONF_SHORT_SLOT_TIME);
		break;

	case PRISM2_PARAM_NEXT_MODE:
		*param = local->next_mode;
		break;

	case PRISM2_PARAM_ANTENNA_SEL:
		*param = local->hw.conf.antenna_sel;
		break;

	case PRISM2_PARAM_ANTENNA_MODE:
		*param = local->hw.conf.antenna_mode;
		break;

	case PRISM2_PARAM_BROADCAST_SSID:
		*param = !!(local->hw.conf.flags & IEEE80211_CONF_SSID_HIDDEN);
		break;

	case PRISM2_PARAM_STA_ANTENNA_SEL:
		*param = local->sta_antenna_sel;
		break;

	case PRISM2_PARAM_RATE_CTRL_NUM_UP:
		*param = local->rate_ctrl_num_up;
		break;

	case PRISM2_PARAM_RATE_CTRL_NUM_DOWN:
		*param = local->rate_ctrl_num_down;
		break;

	case PRISM2_PARAM_TX_POWER_REDUCTION:
		*param = local->hw.conf.tx_power_reduction;
		break;

	case PRISM2_PARAM_EAPOL:
		*param = sdata->eapol;
		break;

	case PRISM2_PARAM_KEY_TX_RX_THRESHOLD:
		*param = local->key_tx_rx_threshold;
		break;

	case PRISM2_PARAM_KEY_INDEX:
		if (!sdata->default_key)
			ret = -ENOENT;
		else if (sdata->default_key == sdata->keys[0])
			*param = 0;
		else if (sdata->default_key == sdata->keys[1])
			*param = 1;
		else if (sdata->default_key == sdata->keys[2])
			*param = 2;
		else if (sdata->default_key == sdata->keys[3])
			*param = 3;
		else
			ret = -ENOENT;
		break;

	case PRISM2_PARAM_DEFAULT_WEP_ONLY:
		*param = local->default_wep_only;
		break;

	case PRISM2_PARAM_WIFI_WME_NOACK_TEST:
		*param = local->wifi_wme_noack_test;
		break;

	case PRISM2_PARAM_ALLOW_BROADCAST_ALWAYS:
		*param = local->allow_broadcast_always;
		break;

	case PRISM2_PARAM_SCAN_FLAGS:
		*param = local->scan_flags;
		break;

	case PRISM2_PARAM_HW_MODES:
		*param = local->enabled_modes;
		break;

	case PRISM2_PARAM_CREATE_IBSS:
		if (sdata->type != IEEE80211_IF_TYPE_IBSS)
			ret = -EINVAL;
		else
			*param = !!sdata->u.sta.create_ibss;
		break;

	case PRISM2_PARAM_MIXED_CELL:
		if (sdata->type != IEEE80211_IF_TYPE_STA &&
		    sdata->type != IEEE80211_IF_TYPE_IBSS)
			ret = -EINVAL;
		else
			*param = !!sdata->u.sta.mixed_cell;
		break;

	case PRISM2_PARAM_KEY_MGMT:
		if (sdata->type != IEEE80211_IF_TYPE_STA)
			ret = -EINVAL;
		else
			*param = sdata->u.sta.key_mgmt;
		break;
	case PRISM2_PARAM_WMM_ENABLED:
		if (sdata->type != IEEE80211_IF_TYPE_STA &&
		    sdata->type != IEEE80211_IF_TYPE_IBSS)
			ret = -EINVAL;
		else
			*param = !!sdata->u.sta.wmm_enabled;
		break;
	case PRISM2_PARAM_MGMT_IF:
		if (local->apdev)
			*param = local->apdev->ifindex;
		else
			ret = -ENOENT;
		break;
	case PRISM2_PARAM_USER_SPACE_MLME:
		*param = local->user_space_mlme;
		break;

	default:
		ret = -EOPNOTSUPP;
		break;
	}

	return ret;
}


static int ieee80211_ioctl_test_param(struct net_device *dev,
				      struct iw_request_info *info,
				      void *wrqu, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	int *i = (int *) extra;
	int param = *i;
	int value = *(i + 1);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (local->ops->test_param)
		return local->ops->test_param(local_to_hw(local),
					     param, value);

	return -EOPNOTSUPP;
}


static int ieee80211_ioctl_siwmlme(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_point *data, char *extra)
{
	struct ieee80211_sub_if_data *sdata;
	struct iw_mlme *mlme = (struct iw_mlme *) extra;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_IF_TYPE_STA &&
	    sdata->type != IEEE80211_IF_TYPE_IBSS)
		return -EINVAL;

	switch (mlme->cmd) {
	case IW_MLME_DEAUTH:
		/* TODO: mlme->addr.sa_data */
		return ieee80211_sta_deauthenticate(dev, mlme->reason_code);
	case IW_MLME_DISASSOC:
		/* TODO: mlme->addr.sa_data */
		return ieee80211_sta_disassociate(dev, mlme->reason_code);
	default:
		return -EOPNOTSUPP;
	}
}


static int ieee80211_ioctl_siwencode(struct net_device *dev,
				     struct iw_request_info *info,
				     struct iw_point *erq, char *keybuf)
{
	struct ieee80211_sub_if_data *sdata;
	int idx, i, alg = ALG_WEP;
	u8 bcaddr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	idx = erq->flags & IW_ENCODE_INDEX;
	if (idx < 1 || idx > 4) {
		idx = -1;
		if (!sdata->default_key)
			idx = 0;
		else for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
			if (sdata->default_key == sdata->keys[i]) {
				idx = i;
				break;
			}
		}
		if (idx < 0)
			return -EINVAL;
	} else
		idx--;

	if (erq->flags & IW_ENCODE_DISABLED)
		alg = ALG_NONE;
	else if (erq->length == 0) {
		/* No key data - just set the default TX key index */
		if (sdata->default_key != sdata->keys[idx]) {
			if (sdata->default_key)
				ieee80211_key_sysfs_remove_default(sdata);
			sdata->default_key = sdata->keys[idx];
			if (sdata->default_key)
				ieee80211_key_sysfs_add_default(sdata);
		}
		return 0;
	}

	return ieee80211_set_encryption(
		dev, bcaddr,
		idx, alg,
		!sdata->default_key,
		NULL, keybuf, erq->length);
}


static int ieee80211_ioctl_giwencode(struct net_device *dev,
				     struct iw_request_info *info,
				     struct iw_point *erq, char *key)
{
	struct ieee80211_sub_if_data *sdata;
	int idx, i;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	idx = erq->flags & IW_ENCODE_INDEX;
	if (idx < 1 || idx > 4) {
		idx = -1;
		if (!sdata->default_key)
			idx = 0;
		else for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
			if (sdata->default_key == sdata->keys[i]) {
				idx = i;
				break;
			}
		}
		if (idx < 0)
			return -EINVAL;
	} else
		idx--;

	erq->flags = idx + 1;

	if (!sdata->keys[idx]) {
		erq->length = 0;
		erq->flags |= IW_ENCODE_DISABLED;
		return 0;
	}

	memcpy(key, sdata->keys[idx]->key,
	       min((int)erq->length, sdata->keys[idx]->keylen));
	erq->length = sdata->keys[idx]->keylen;
	erq->flags |= IW_ENCODE_ENABLED;

	return 0;
}


static int ieee80211_ioctl_siwgenie(struct net_device *dev,
				    struct iw_request_info *info,
				    struct iw_point *data, char *extra)
{
	return ieee80211_set_gen_ie(dev, extra, data->length);
}


static int ieee80211_ioctl_siwauth(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_param *data, char *extra)
{
	struct ieee80211_local *local = dev->ieee80211_ptr;
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	int ret = 0;

	switch (data->flags & IW_AUTH_INDEX) {
	case IW_AUTH_WPA_VERSION:
	case IW_AUTH_CIPHER_PAIRWISE:
	case IW_AUTH_CIPHER_GROUP:
	case IW_AUTH_WPA_ENABLED:
	case IW_AUTH_RX_UNENCRYPTED_EAPOL:
		break;
	case IW_AUTH_KEY_MGMT:
		if (sdata->type != IEEE80211_IF_TYPE_STA)
			ret = -EINVAL;
		else {
			/*
			 * TODO: sdata->u.sta.key_mgmt does not match with WE18
			 * value completely; could consider modifying this to
			 * be closer to WE18. For now, this value is not really
			 * used for anything else than Privacy matching, so the
			 * current code here should be more or less OK.
			 */
			if (data->value & IW_AUTH_KEY_MGMT_802_1X) {
				sdata->u.sta.key_mgmt =
					IEEE80211_KEY_MGMT_WPA_EAP;
			} else if (data->value & IW_AUTH_KEY_MGMT_PSK) {
				sdata->u.sta.key_mgmt =
					IEEE80211_KEY_MGMT_WPA_PSK;
			} else {
				sdata->u.sta.key_mgmt =
					IEEE80211_KEY_MGMT_NONE;
			}
		}
		break;
	case IW_AUTH_80211_AUTH_ALG:
		if (sdata->type == IEEE80211_IF_TYPE_STA ||
		    sdata->type == IEEE80211_IF_TYPE_IBSS)
			sdata->u.sta.auth_algs = data->value;
		else
			ret = -EOPNOTSUPP;
		break;
	case IW_AUTH_PRIVACY_INVOKED:
		if (local->ops->set_privacy_invoked)
			ret = local->ops->set_privacy_invoked(
					local_to_hw(local), data->value);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}
	return ret;
}

/* Get wireless statistics.  Called by /proc/net/wireless and by SIOCGIWSTATS */
static struct iw_statistics *ieee80211_get_wireless_stats(struct net_device *net_dev)
{
	struct ieee80211_local *local = net_dev->ieee80211_ptr;
	struct iw_statistics * wstats = &local->wstats;
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(net_dev);
	struct sta_info *sta;
	static int tmp_level = 0;
	static int tmp_qual = 0;

	sta = sta_info_get(local, sdata->u.sta.bssid);
	if (!sta) {
		wstats->discard.fragment = 0;
		wstats->discard.misc = 0;
		wstats->qual.qual = 0;
		wstats->qual.level = 0;
		wstats->qual.noise = 0;
		wstats->qual.updated = IW_QUAL_ALL_INVALID;
	} else {
		if (!tmp_level) {	/* get initial values */
			tmp_level = sta->last_signal;
			tmp_qual = sta->last_rssi;
		} else {		/* smooth results */
			tmp_level = (15 * tmp_level + sta->last_signal)/16;
			tmp_qual = (15 * tmp_qual + sta->last_rssi)/16;
		}
		wstats->qual.level = tmp_level;
		wstats->qual.qual = 100*tmp_qual/local->hw.maxssi;
		wstats->qual.noise = sta->last_noise;
		wstats->qual.updated = IW_QUAL_ALL_UPDATED | IW_QUAL_DBM;
		sta_info_put(sta);
	}
	return wstats;
}

static int ieee80211_ioctl_giwauth(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_param *data, char *extra)
{
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	int ret = 0;

	switch (data->flags & IW_AUTH_INDEX) {
	case IW_AUTH_80211_AUTH_ALG:
		if (sdata->type == IEEE80211_IF_TYPE_STA ||
		    sdata->type == IEEE80211_IF_TYPE_IBSS)
			data->value = sdata->u.sta.auth_algs;
		else
			ret = -EOPNOTSUPP;
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}
	return ret;
}


static int ieee80211_ioctl_siwencodeext(struct net_device *dev,
					struct iw_request_info *info,
					struct iw_point *erq, char *extra)
{
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	struct iw_encode_ext *ext = (struct iw_encode_ext *) extra;
	int alg, idx, i;

	switch (ext->alg) {
	case IW_ENCODE_ALG_NONE:
		alg = ALG_NONE;
		break;
	case IW_ENCODE_ALG_WEP:
		alg = ALG_WEP;
		break;
	case IW_ENCODE_ALG_TKIP:
		alg = ALG_TKIP;
		break;
	case IW_ENCODE_ALG_CCMP:
		alg = ALG_CCMP;
		break;
	default:
		return -EOPNOTSUPP;
	}

	if (erq->flags & IW_ENCODE_DISABLED)
		alg = ALG_NONE;

	idx = erq->flags & IW_ENCODE_INDEX;
	if (idx < 1 || idx > 4) {
		idx = -1;
		if (!sdata->default_key)
			idx = 0;
		else for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
			if (sdata->default_key == sdata->keys[i]) {
				idx = i;
				break;
			}
		}
		if (idx < 0)
			return -EINVAL;
	} else
		idx--;

	return ieee80211_set_encryption(dev, ext->addr.sa_data, idx, alg,
					ext->ext_flags &
					IW_ENCODE_EXT_SET_TX_KEY,
					NULL, ext->key, ext->key_len);
}


static const struct iw_priv_args ieee80211_ioctl_priv[] = {
	{ PRISM2_IOCTL_PRISM2_PARAM,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, "param" },
	{ PRISM2_IOCTL_GET_PRISM2_PARAM,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 1, "get_param" },
	{ PRISM2_IOCTL_TEST_PARAM,
	  IW_PRIV_TYPE_INT | IW_PRIV_SIZE_FIXED | 2, 0, "test_param" },
};


int ieee80211_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct iwreq *wrq = (struct iwreq *) rq;
	int ret = 0;

	switch (cmd) {
		/* Private ioctls (iwpriv) that have not yet been converted
		 * into new wireless extensions API */
	case PRISM2_IOCTL_TEST_PARAM:
		ret = ieee80211_ioctl_test_param(dev, NULL, &wrq->u,
						 (char *) &wrq->u);
		break;
	case PRISM2_IOCTL_HOSTAPD:
		if (!capable(CAP_NET_ADMIN)) ret = -EPERM;
		else ret = ieee80211_ioctl_priv_hostapd(dev, &wrq->u.data);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}

	return ret;
}


/* Structures to export the Wireless Handlers */

static const iw_handler ieee80211_handler[] =
{
	(iw_handler) NULL,				/* SIOCSIWCOMMIT */
	(iw_handler) ieee80211_ioctl_giwname,		/* SIOCGIWNAME */
	(iw_handler) NULL,				/* SIOCSIWNWID */
	(iw_handler) NULL,				/* SIOCGIWNWID */
	(iw_handler) ieee80211_ioctl_siwfreq,		/* SIOCSIWFREQ */
	(iw_handler) ieee80211_ioctl_giwfreq,		/* SIOCGIWFREQ */
	(iw_handler) ieee80211_ioctl_siwmode,		/* SIOCSIWMODE */
	(iw_handler) ieee80211_ioctl_giwmode,		/* SIOCGIWMODE */
	(iw_handler) NULL,				/* SIOCSIWSENS */
	(iw_handler) NULL,				/* SIOCGIWSENS */
	(iw_handler) NULL /* not used */,		/* SIOCSIWRANGE */
	(iw_handler) ieee80211_ioctl_giwrange,		/* SIOCGIWRANGE */
	(iw_handler) NULL /* not used */,		/* SIOCSIWPRIV */
	(iw_handler) NULL /* kernel code */,		/* SIOCGIWPRIV */
	(iw_handler) NULL /* not used */,		/* SIOCSIWSTATS */
	(iw_handler) NULL /* kernel code */,		/* SIOCGIWSTATS */
	iw_handler_set_spy,				/* SIOCSIWSPY */
	iw_handler_get_spy,				/* SIOCGIWSPY */
	iw_handler_set_thrspy,				/* SIOCSIWTHRSPY */
	iw_handler_get_thrspy,				/* SIOCGIWTHRSPY */
	(iw_handler) ieee80211_ioctl_siwap,		/* SIOCSIWAP */
	(iw_handler) ieee80211_ioctl_giwap,		/* SIOCGIWAP */
	(iw_handler) ieee80211_ioctl_siwmlme,		/* SIOCSIWMLME */
	(iw_handler) NULL,				/* SIOCGIWAPLIST */
	(iw_handler) ieee80211_ioctl_siwscan,		/* SIOCSIWSCAN */
	(iw_handler) ieee80211_ioctl_giwscan,		/* SIOCGIWSCAN */
	(iw_handler) ieee80211_ioctl_siwessid,		/* SIOCSIWESSID */
	(iw_handler) ieee80211_ioctl_giwessid,		/* SIOCGIWESSID */
	(iw_handler) NULL,				/* SIOCSIWNICKN */
	(iw_handler) NULL,				/* SIOCGIWNICKN */
	(iw_handler) NULL,				/* -- hole -- */
	(iw_handler) NULL,				/* -- hole -- */
	(iw_handler) NULL,				/* SIOCSIWRATE */
	(iw_handler) NULL,				/* SIOCGIWRATE */
	(iw_handler) ieee80211_ioctl_siwrts,		/* SIOCSIWRTS */
	(iw_handler) ieee80211_ioctl_giwrts,		/* SIOCGIWRTS */
	(iw_handler) ieee80211_ioctl_siwfrag,		/* SIOCSIWFRAG */
	(iw_handler) ieee80211_ioctl_giwfrag,		/* SIOCGIWFRAG */
	(iw_handler) NULL,				/* SIOCSIWTXPOW */
	(iw_handler) NULL,				/* SIOCGIWTXPOW */
	(iw_handler) ieee80211_ioctl_siwretry,		/* SIOCSIWRETRY */
	(iw_handler) ieee80211_ioctl_giwretry,		/* SIOCGIWRETRY */
	(iw_handler) ieee80211_ioctl_siwencode,		/* SIOCSIWENCODE */
	(iw_handler) ieee80211_ioctl_giwencode,		/* SIOCGIWENCODE */
	(iw_handler) NULL,				/* SIOCSIWPOWER */
	(iw_handler) NULL,				/* SIOCGIWPOWER */
	(iw_handler) NULL,				/* -- hole -- */
	(iw_handler) NULL,				/* -- hole -- */
	(iw_handler) ieee80211_ioctl_siwgenie,		/* SIOCSIWGENIE */
	(iw_handler) NULL,				/* SIOCGIWGENIE */
	(iw_handler) ieee80211_ioctl_siwauth,		/* SIOCSIWAUTH */
	(iw_handler) ieee80211_ioctl_giwauth,		/* SIOCGIWAUTH */
	(iw_handler) ieee80211_ioctl_siwencodeext,	/* SIOCSIWENCODEEXT */
	(iw_handler) NULL,				/* SIOCGIWENCODEEXT */
	(iw_handler) NULL,				/* SIOCSIWPMKSA */
	(iw_handler) NULL,				/* -- hole -- */
};

static const iw_handler ieee80211_private_handler[] =
{							/* SIOCIWFIRSTPRIV + */
	(iw_handler) ieee80211_ioctl_prism2_param,	/* 0 */
	(iw_handler) ieee80211_ioctl_get_prism2_param,	/* 1 */
};

const struct iw_handler_def ieee80211_iw_handler_def =
{
	.num_standard	= sizeof(ieee80211_handler) / sizeof(iw_handler),
	.num_private	= sizeof(ieee80211_private_handler) /
			  sizeof(iw_handler),
	.num_private_args = sizeof(ieee80211_ioctl_priv) /
			    sizeof(struct iw_priv_args),
	.standard	= (iw_handler *) ieee80211_handler,
	.private	= (iw_handler *) ieee80211_private_handler,
	.private_args	= (struct iw_priv_args *) ieee80211_ioctl_priv,
	.get_wireless_stats = ieee80211_get_wireless_stats,
};

/* Wireless handlers for master interface */

static const iw_handler ieee80211_master_handler[] =
{
	[SIOCGIWNAME  - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_giwname,
	[SIOCSIWFREQ  - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_siwfreq,
	[SIOCGIWFREQ  - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_giwfreq,
	[SIOCGIWRANGE - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_giwrange,
	[SIOCSIWRTS   - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_siwrts,
	[SIOCGIWRTS   - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_giwrts,
	[SIOCSIWFRAG  - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_siwfrag,
	[SIOCGIWFRAG  - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_giwfrag,
	[SIOCSIWRETRY - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_siwretry,
	[SIOCGIWRETRY - SIOCIWFIRST] = (iw_handler) ieee80211_ioctl_giwretry,
};

const struct iw_handler_def ieee80211_iw_master_handler_def =
{
	.num_standard	= sizeof(ieee80211_master_handler) / sizeof(iw_handler),
	.standard	= ieee80211_master_handler,
};
