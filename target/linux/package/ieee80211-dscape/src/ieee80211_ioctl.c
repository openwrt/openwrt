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
#include <linux/if_arp.h>
#include <linux/wireless.h>
#include <net/iw_handler.h>
#include <asm/uaccess.h>

#include <net/ieee80211.h>
#include <net/ieee80211_mgmt.h>
#include "ieee80211_i.h"
#include "hostapd_ioctl.h"
#include "rate_control.h"
#include "wpa.h"
#include "aes_ccm.h"


static int ieee80211_regdom = 0x10; /* FCC */
MODULE_PARM(ieee80211_regdom, "i");
MODULE_PARM_DESC(ieee80211_regdom, "IEEE 802.11 regulatory domain; 64=MKK");

/*
 * If firmware is upgraded by the vendor, additional channels can be used based
 * on the new Japanese regulatory rules. This is indicated by setting
 * ieee80211_japan_5ghz module parameter to one when loading the 80211 kernel
 * module.
 */
static int ieee80211_japan_5ghz /* = 0 */;
MODULE_PARM(ieee80211_japan_5ghz, "i");
MODULE_PARM_DESC(ieee80211_japan_5ghz, "Vendor-updated firmware for 5 GHz");


static int ieee80211_ioctl_set_beacon(struct net_device *dev,
				      struct prism2_hostapd_param *param,
				      int param_len,
				      int flag)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_norm     *norm;
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
	if (sdata->type != IEEE80211_SUB_IF_TYPE_NORM)
		return -EINVAL;
	norm = &sdata->u.norm;

	switch (flag) {
	case 0:
		b_head = &norm->beacon_head;
		b_tail = &norm->beacon_tail;
		b_head_len = &norm->beacon_head_len;
		b_tail_len = &norm->beacon_tail_len;
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

	return 0;
}


static int ieee80211_ioctl_get_hw_features(struct net_device *dev,
					   struct prism2_hostapd_param *param,
					   int param_len)
{
	struct ieee80211_local *local = dev->priv;
	u8 *pos = param->u.hw_features.data;
	int left = param_len - (pos - (u8 *) param);
	int mode, i;
	struct hostapd_ioctl_hw_modes_hdr *hdr;
	struct ieee80211_rate_data *rate;
	struct ieee80211_channel_data *chan;

	param->u.hw_features.flags = 0;
	if (local->hw->data_nullfunc_ack)
		param->u.hw_features.flags |= HOSTAP_HW_FLAG_NULLFUNC_OK;

	param->u.hw_features.num_modes = local->hw->num_modes;
	for (mode = 0; mode < local->hw->num_modes; mode++) {
		int clen, rlen;
		struct ieee80211_hw_modes *m = &local->hw->modes[mode];
		clen = m->num_channels * sizeof(struct ieee80211_channel_data);
		rlen = m->num_rates * sizeof(struct ieee80211_rate_data);
		if (left < sizeof(*hdr) + clen + rlen)
			return -E2BIG;
		left -= sizeof(*hdr) + clen + rlen;

		hdr = (struct hostapd_ioctl_hw_modes_hdr *) pos;
		hdr->mode = m->mode;
		hdr->num_channels = m->num_channels;
		hdr->num_rates = m->num_rates;

		pos = (u8 *) (hdr + 1);
		chan = (struct ieee80211_channel_data *) pos;
		for (i = 0; i < m->num_channels; i++) {
			chan[i].chan = m->channels[i].chan;
			chan[i].freq = m->channels[i].freq;
			chan[i].flag = m->channels[i].flag;
		}
		pos += clen;

		rate = (struct ieee80211_rate_data *) pos;
		for (i = 0; i < m->num_rates; i++) {
			rate[i].rate = m->rates[i].rate;
			rate[i].flags = m->rates[i].flags;
		}
		pos += rlen;
	}

	return 0;
}


static int ieee80211_ioctl_scan(struct net_device *dev,
                                struct prism2_hostapd_param *param)
{
        struct ieee80211_local *local = dev->priv;

	if (local->hw->passive_scan == NULL)
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
        param->u.scan.channel = local->hw->modes[local->scan.mode_idx].
		channels[local->scan.chan_idx].chan;

	return 0;
}


static int ieee80211_ioctl_flush(struct net_device *dev,
				 struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
	sta_info_flush(local, NULL);
	return 0;
}




static int ieee80211_ioctl_add_sta(struct net_device *dev,
				   struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
	struct sta_info *sta;
	u32 rates;
	int i, j;
        struct ieee80211_sub_if_data *sdata;
	int add_key_entry = 1;

	sta = sta_info_get(local, param->sta_addr);

        if (sta == NULL) {
		sta = sta_info_add(local, dev, param->sta_addr);
		if (sta == NULL)
			return -ENOMEM;
        }

	if (sta->dev != dev) {
		/* Binding STA to a new interface, so remove all references to
		 * the old BSS. */
		sta_info_remove_aid_ptr(sta);
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
	if (sta->aid > MAX_AID_TABLE_SIZE)
		sta->aid = 0;
	if (sta->aid > 0 && sdata->bss)
		sdata->bss->sta_aid[sta->aid - 1] = sta;
	if (sdata->bss && sta->aid > sdata->bss->max_aid)
		sdata->bss->max_aid = sta->aid;

	rates = 0;
	for (i = 0; i < sizeof(param->u.add_sta.supp_rates); i++) {
		int rate = (param->u.add_sta.supp_rates[i] & 0x7f) * 5;
		if (local->conf.phymode == MODE_ATHEROS_TURBO ||
		    local->conf.phymode == MODE_ATHEROS_TURBOG)
			rate *= 2;
		for (j = 0; j < local->num_curr_rates; j++) {
			if (local->curr_rates[j].rate == rate)
				rates |= BIT(j);
		}

	}
	sta->supp_rates = rates;

	rate_control_rate_init(local, sta);



	if (param->u.add_sta.wds_flags & 0x01)
		sta->flags |= WLAN_STA_WDS;
	else
		sta->flags &= ~WLAN_STA_WDS;

	if (add_key_entry && sta->key == NULL && sdata->default_key == NULL &&
	    local->hw->set_key) {
		struct ieee80211_key_conf conf;
		/* Add key cache entry with NULL key type because this may used
		 * for TX filtering. */
		memset(&conf, 0, sizeof(conf));
		conf.hw_key_idx = HW_KEY_IDX_INVALID;
		conf.alg = ALG_NULL;
		conf.force_sw_encrypt = 1;
		if (local->hw->set_key(dev, SET_KEY, sta->addr, &conf,
				       sta->aid)) {
			sta->key_idx_compression = HW_KEY_IDX_INVALID;
		} else {
			sta->key_idx_compression = conf.hw_key_idx;
		}
	}

	sta_info_release(local, sta);

	return 0;
}


static int ieee80211_ioctl_remove_sta(struct net_device *dev,
				      struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
	struct sta_info *sta;

	sta = sta_info_get(local, param->sta_addr);
	if (sta) {
		sta_info_release(local, sta);
		sta_info_free(local, sta, 1);
	}

	return sta ? 0 : -ENOENT;
}


static int ieee80211_ioctl_get_dot11counterstable(struct net_device *dev,
					struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
        struct ieee80211_low_level_stats stats;

	memset(&stats, 0, sizeof(stats));
	if (local->hw->get_stats)
		local->hw->get_stats(dev, &stats);
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
	struct ieee80211_local *local = dev->priv;
	struct sta_info *sta;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff) {
          struct ieee80211_sub_if_data *sdata;
          struct net_device_stats *stats;

          sdata = IEEE80211_DEV_TO_SUB_IF(dev);
          stats = ieee80211_dev_stats(sdata->master);
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

	sta_info_release(local, sta);

	return 0;
}


static int ieee80211_ioctl_set_flags_sta(struct net_device *dev,
					 struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
	struct sta_info *sta;

	sta = sta_info_get(local, param->sta_addr);
	if (sta) {
		sta->flags |= param->u.set_flags_sta.flags_or;
		sta->flags &= param->u.set_flags_sta.flags_and;
		if (local->hw->set_port_auth &&
		    (param->u.set_flags_sta.flags_or & WLAN_STA_AUTHORIZED) &&
		    local->hw->set_port_auth(local->mdev, sta->addr, 1))
			printk(KERN_DEBUG "%s: failed to set low-level driver "
			       "PAE state (authorized) for " MACSTR "\n",
			       dev->name, MAC2STR(sta->addr));
		if (local->hw->set_port_auth &&
		    !(param->u.set_flags_sta.flags_and & WLAN_STA_AUTHORIZED)
		    && local->hw->set_port_auth(local->mdev, sta->addr, 0))
			printk(KERN_DEBUG "%s: failed to set low-level driver "
			       "PAE state (unauthorized) for " MACSTR "\n",
			       dev->name, MAC2STR(sta->addr));
		sta_info_release(local, sta);
	}

	return sta ? 0 : -ENOENT;
}


int ieee80211_set_hw_encryption(struct net_device *dev,
				struct sta_info *sta, u8 addr[ETH_ALEN],
				struct ieee80211_key *key)
{
	struct ieee80211_key_conf *keyconf = NULL;
	struct ieee80211_local *local = dev->priv;
	int rc = 0;

	/* default to sw encryption; this will be cleared by low-level
	 * driver if the hw supports requested encryption */
	if (key)
		key->force_sw_encrypt = 1;

	if (key && local->hw->set_key &&
	    (!local->conf.sw_encrypt || !local->conf.sw_decrypt) &&
	    (keyconf = ieee80211_key_data2conf(local, key)) != NULL) {
		if (local->hw->set_key(dev, SET_KEY, addr,
				       keyconf, sta ? sta->aid : 0)) {
			rc = HOSTAP_CRYPT_ERR_KEY_SET_FAILED;
			key->force_sw_encrypt = 1;
			key->hw_key_idx = HW_KEY_IDX_INVALID;
		} else {
			key->force_sw_encrypt =
				keyconf->force_sw_encrypt;
			key->hw_key_idx =
				keyconf->hw_key_idx;

		}
	}
	kfree(keyconf);

	return rc;
}


static int ieee80211_ioctl_set_encryption(struct net_device *dev,
					  struct prism2_hostapd_param *param,
					  int param_len)
{
	struct ieee80211_local *local = dev->priv;
	int alg, ret = 0;
	struct sta_info *sta;
	struct ieee80211_key **key;
	int set_tx_key = 0, try_hwaccel = 1;
        struct ieee80211_key_conf *keyconf;
        struct ieee80211_sub_if_data *sdata;

        sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	param->u.crypt.err = 0;
	param->u.crypt.alg[HOSTAP_CRYPT_ALG_NAME_LEN - 1] = '\0';

	if (param_len <
	    (int) ((char *) param->u.crypt.key - (char *) param) +
	    param->u.crypt.key_len) {
		printk(KERN_DEBUG "%s: set_encrypt - invalid param_lem\n",
		       dev->name);
		return -EINVAL;
        }

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff) {
		sta = NULL;
		if (param->u.crypt.idx >= NUM_DEFAULT_KEYS) {
			printk(KERN_DEBUG "%s: set_encrypt - invalid idx=%d\n",
			       dev->name, param->u.crypt.idx);
			return -EINVAL;
		}
		key = &sdata->keys[param->u.crypt.idx];
		if (param->u.crypt.flags & HOSTAP_CRYPT_FLAG_SET_TX_KEY)
			set_tx_key = 1;

		/* Disable hwaccel for default keys when the interface is not
		 * the default one.
		 * TODO: consider adding hwaccel support for these; at least
		 * Atheros key cache should be able to handle this since AP is
		 * only transmitting frames with default keys. */
		/* FIX: hw key cache can be used when only one virtual
		 * STA is associated with each AP. If more than one STA
		 * is associated to the same AP, software encryption
		 * must be used. This should be done automatically
		 * based on configured station devices. For the time
		 * being, this can be only set at compile time. */
		if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
			if (0 /* FIX: more than one STA per AP */)
				try_hwaccel = 0;
		} else
		if (sdata->type != IEEE80211_SUB_IF_TYPE_NORM ||
		    dev != local->wdev)
			try_hwaccel = 0;
	} else {
		if (param->u.crypt.idx != 0) {
			printk(KERN_DEBUG "%s: set_encrypt - non-zero idx for "
			       "individual key\n", dev->name);
			return -EINVAL;
		}

		sta = sta_info_get(local, param->sta_addr);
		if (sta == NULL) {
			param->u.crypt.err = HOSTAP_CRYPT_ERR_UNKNOWN_ADDR;
#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
			printk(KERN_DEBUG "%s: set_encrypt - unknown addr "
			       MACSTR "\n",
			       dev->name, MAC2STR(param->sta_addr));
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */

			return -ENOENT;
		}

		key = &sta->key;
	}

	if (strcmp(param->u.crypt.alg, "none") == 0) {
		alg = ALG_NONE;
	} else if (strcmp(param->u.crypt.alg, "WEP") == 0) {
		alg = ALG_WEP;
	} else if (strcmp(param->u.crypt.alg, "TKIP") == 0) {
		if (param->u.crypt.key_len != ALG_TKIP_KEY_LEN) {
			printk(KERN_DEBUG "%s: set_encrypt - invalid TKIP key "
			       "length %d\n", dev->name,
			       param->u.crypt.key_len);
			ret = -EINVAL;
			goto done;
		}
		alg = ALG_TKIP;
	} else if (strcmp(param->u.crypt.alg, "CCMP") == 0) {
		if (param->u.crypt.key_len != ALG_CCMP_KEY_LEN) {
			printk(KERN_DEBUG "%s: set_encrypt - invalid CCMP key "
			       "length %d\n", dev->name,
			       param->u.crypt.key_len);
			ret = -EINVAL;
			goto done;
		}
		alg = ALG_CCMP;
	} else {
		param->u.crypt.err = HOSTAP_CRYPT_ERR_UNKNOWN_ALG;
		printk(KERN_DEBUG "%s: set_encrypt - unknown alg\n",
		       dev->name);
		ret = -EINVAL;
		goto done;
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
	    local->conf.mode != IW_MODE_ADHOC &&
	    local->conf.mode != IW_MODE_INFRA) {
		try_hwaccel = 0;
	}

	if (local->hw->device_hides_wep) {
		/* Software encryption cannot be used with devices that hide
		 * encryption from the host system, so always try to use
		 * hardware acceleration with such devices. */
		try_hwaccel = 1;
	}

	if (local->hw->no_tkip_wmm_hwaccel && alg == ALG_TKIP) {
		if (sta && (sta->flags & WLAN_STA_WME)) {
		/* Hardware does not support hwaccel with TKIP when using WMM.
		 */
			try_hwaccel = 0;
		}
		else if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
			sta = sta_info_get(local, sdata->u.sta.bssid);
			if (sta) {
				if (sta->flags & WLAN_STA_WME) {
					try_hwaccel = 0;
				}
				sta_info_release(local, sta);
				sta = NULL;
			}
		}
	}


	if (alg == ALG_NONE) {
		keyconf = NULL;
		if (try_hwaccel && *key && local->hw->set_key &&
		    (keyconf = ieee80211_key_data2conf(local, *key)) != NULL &&
		    local->hw->set_key(dev, DISABLE_KEY, param->sta_addr,
				       keyconf, sta ? sta->aid : 0)) {
			param->u.crypt.err = HOSTAP_CRYPT_ERR_KEY_SET_FAILED;
			printk(KERN_DEBUG "%s: set_encrypt - low-level disable"
			       " failed\n", dev->name);
			ret = -EINVAL;
		}
		kfree(keyconf);

		if (sdata->default_key == *key)
			sdata->default_key = NULL;
		kfree(*key);
		*key = NULL;
	} else {
		if (*key == NULL || (*key)->keylen < param->u.crypt.key_len) {
			kfree(*key);
			*key = kmalloc(sizeof(struct ieee80211_key) +
				       param->u.crypt.key_len, GFP_ATOMIC);
			if (*key == NULL) {
				ret = -ENOMEM;
				goto done;
			}
		}
		memset(*key, 0, sizeof(struct ieee80211_key) +
		       param->u.crypt.key_len);
		/* default to sw encryption; low-level driver sets these if the
		 * requested encryption is supported */
		(*key)->hw_key_idx = HW_KEY_IDX_INVALID;
		(*key)->force_sw_encrypt = 1;

		(*key)->alg = alg;
		(*key)->keyidx = param->u.crypt.idx;
		(*key)->keylen = param->u.crypt.key_len;
		memcpy((*key)->key, param->u.crypt.key,
		       param->u.crypt.key_len);
		if (set_tx_key)
			(*key)->default_tx_key = 1;

		if (alg == ALG_CCMP) {
			/* Initialize AES key state here as an optimization
			 * so that it does not need to be initialized for every
			 * packet. */
			ieee80211_aes_key_setup_encrypt(
				(*key)->u.ccmp.aes_state, (*key)->key);
		}

		if (try_hwaccel &&
		    (alg == ALG_WEP || alg == ALG_TKIP || alg == ALG_CCMP))
			param->u.crypt.err = ieee80211_set_hw_encryption(
				dev, sta, param->sta_addr, *key);
	}

	if (set_tx_key || (sta == NULL && sdata->default_key == NULL)) {
                sdata->default_key = *key;
		if (local->hw->set_key_idx &&
		    local->hw->set_key_idx(dev, param->u.crypt.idx))
			printk(KERN_DEBUG "%s: failed to set TX key idx for "
			       "low-level driver\n", dev->name);
	}

 done:
	if (sta)
		sta_info_release(local, sta);

	return ret;
}

static int ieee80211_ioctl_get_encryption(struct net_device *dev,
					  struct prism2_hostapd_param *param,
					  int param_len)
{
	struct ieee80211_local *local = dev->priv;
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
		if (param->u.crypt.idx > NUM_DEFAULT_KEYS) {
			param->u.crypt.idx = sdata->default_key ?
				sdata->default_key->keyidx : 0;
			return 0;
		} else
			key = &sdata->keys[param->u.crypt.idx];
	} else {
		sta = sta_info_get(local, param->sta_addr);
		if (sta == NULL) {
			param->u.crypt.err = HOSTAP_CRYPT_ERR_UNKNOWN_ADDR;
			return -EINVAL;
		}

		key = &sta->key;
	}


	memset(param->u.crypt.seq_counter, 0, HOSTAP_SEQ_COUNTER_SIZE);
	if (*key == NULL) {
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
			if (local->hw->get_sequence_counter) {
			/* Get transmit counter from low level driver */
				if (local->hw->get_sequence_counter(dev,
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
		sta_info_release(local, sta);

	return ret;
}


#ifdef CONFIG_HOSTAPD_WPA_TESTING
static int ieee80211_ioctl_wpa_trigger(struct net_device *dev,
				       struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
	struct sta_info *sta;

	if (param->sta_addr[0] == 0xff && param->sta_addr[1] == 0xff &&
	    param->sta_addr[2] == 0xff && param->sta_addr[3] == 0xff &&
	    param->sta_addr[4] == 0xff && param->sta_addr[5] == 0xff) {
		local->wpa_trigger = param->u.wpa_trigger.trigger;
		return 0;
	}

	sta = sta_info_get(local, param->sta_addr);
	if (sta == NULL) {
		printk(KERN_DEBUG "%s: wpa_trigger - unknown addr\n",
		       dev->name);
		return -EINVAL;
	}

	sta->wpa_trigger = param->u.wpa_trigger.trigger;

	sta_info_release(local, sta);
	return 0;
}
#endif /* CONFIG_HOSTAPD_WPA_TESTING */


static int ieee80211_ioctl_set_rate_sets(struct net_device *dev,
					 struct prism2_hostapd_param *param,
					 int param_len)
{
	struct ieee80211_local *local = dev->priv;
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

	if (mode == local->conf.phymode) {
		/* TODO: should update STA TX rates and remove STAs if they
		 * do not have any remaining supported rates after the change
		 */
		ieee80211_prepare_rates(dev);
	}

	return 0;
}


static int ieee80211_ioctl_add_if(struct net_device *dev,
				  struct prism2_hostapd_param *param,
				  int param_len)
{
	u8 *pos = param->u.if_info.data;
        int left = param_len - ((u8 *) pos - (u8 *) param);

        if (param->u.if_info.type == HOSTAP_IF_WDS) {
                struct ieee80211_if_wds iwds;
                struct hostapd_if_wds *wds =
			(struct hostapd_if_wds *) param->u.if_info.data;

                if (left < sizeof(struct ieee80211_if_wds))
                        return -EPROTO;

		memcpy(iwds.remote_addr, wds->remote_addr, ETH_ALEN);

		return ieee80211_if_add_wds(dev, param->u.if_info.name,
					    &iwds, 1);
	} else if (param->u.if_info.type == HOSTAP_IF_VLAN) {
		struct hostapd_if_vlan *vlan = (struct hostapd_if_vlan *) pos;
		struct ieee80211_if_vlan ivlan;

		if (left < sizeof(struct hostapd_if_vlan))
			return -EPROTO;

                ivlan.id = vlan->id;

		return ieee80211_if_add_vlan(dev, param->u.if_info.name,
					     &ivlan, 1);
        } else if (param->u.if_info.type == HOSTAP_IF_BSS) {
                struct hostapd_if_bss *bss =
			(struct hostapd_if_bss *) param->u.if_info.data;

                if (left < sizeof(struct hostapd_if_bss))
                        return -EPROTO;

		return ieee80211_if_add_norm(dev, param->u.if_info.name,
					     bss->bssid, 1);
        } else if (param->u.if_info.type == HOSTAP_IF_STA) {
#if 0
                struct hostapd_if_sta *sta =
			(struct hostapd_if_sta *) param->u.if_info.data;
#endif

                if (left < sizeof(struct hostapd_if_sta))
                        return -EPROTO;

		return ieee80211_if_add_sta(dev, param->u.if_info.name, 1);
	} else
                return -EINVAL;

	return 0;
}


static int ieee80211_ioctl_remove_if(struct net_device *dev,
				     struct prism2_hostapd_param *param)
{
	if (param->u.if_info.type == HOSTAP_IF_WDS) {
		return ieee80211_if_remove_wds(dev, param->u.if_info.name, 1);
	} else if (param->u.if_info.type == HOSTAP_IF_VLAN) {
		return ieee80211_if_remove_vlan(dev, param->u.if_info.name, 1);
	} else if (param->u.if_info.type == HOSTAP_IF_BSS) {
		return ieee80211_if_remove_norm(dev, param->u.if_info.name, 1);
	} else if (param->u.if_info.type == HOSTAP_IF_STA) {
		return ieee80211_if_remove_sta(dev, param->u.if_info.name, 1);
	} else {
                return -EINVAL;
	}
}


static int ieee80211_ioctl_update_if(struct net_device *dev,
				     struct prism2_hostapd_param *param,
				     int param_len)
{
	u8 *pos = param->u.if_info.data;
        int left = param_len - ((u8 *) pos - (u8 *) param);

	if (param->u.if_info.type == HOSTAP_IF_WDS) {
		struct ieee80211_if_wds iwds;
		struct hostapd_if_wds *wds =
			(struct hostapd_if_wds *) param->u.if_info.data;

		if (left < sizeof(struct ieee80211_if_wds))
			return -EPROTO;

		memcpy(iwds.remote_addr, wds->remote_addr, ETH_ALEN);

		return ieee80211_if_update_wds(dev, param->u.if_info.name,
					       &iwds, 1);
	} else {
		return -EOPNOTSUPP;
	}
}


static int ieee80211_ioctl_flush_ifs(struct net_device *dev,
				     struct prism2_hostapd_param *param)
{
	return ieee80211_if_flush(dev, 1);
}


static int ieee80211_ioctl_scan_req(struct net_device *dev,
				    struct prism2_hostapd_param *param,
				    int param_len)
{
	u8 *pos = param->u.scan_req.ssid;
	int left = param_len - ((u8 *) pos - (u8 *) param);
	int len = param->u.scan_req.ssid_len;

	if (left < len || len > IEEE80211_MAX_SSID_LEN)
		return -EINVAL;

	return ieee80211_sta_req_scan(dev, pos, len);
}


static int ieee80211_ioctl_sta_get_state(struct net_device *dev,
					 struct prism2_hostapd_param *param)
{
	struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
		return -EINVAL;
	param->u.sta_get_state.state = sdata->u.sta.state;
	return 0;
}


static int ieee80211_ioctl_mlme(struct net_device *dev,
				struct prism2_hostapd_param *param)
{
	struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
		return -EINVAL;
	switch (param->u.mlme.cmd) {
	case MLME_STA_DEAUTH:
		ieee80211_sta_deauthenticate(dev, param->u.mlme.reason_code);
		break;
	case MLME_STA_DISASSOC:
		ieee80211_sta_disassociate(dev, param->u.mlme.reason_code);
		break;
	}
	return 0;
}


static int ieee80211_ioctl_get_load_stats(struct net_device *dev,
					  struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;

	param->u.get_load_stats.channel_use = local->channel_use;
/*	if (param->u.get_load_stats.flags & LOAD_STATS_CLEAR)
		local->channel_use = 0; */ /* now it's not raw counter */

	return 0;
}


static int ieee80211_ioctl_set_sta_vlan(struct net_device *dev,
                                        struct prism2_hostapd_param *param)
{
        struct ieee80211_local *local = dev->priv;
        struct sta_info *sta;

        sta = sta_info_get(local, param->sta_addr);
        if (sta) {
                struct net_device *new_vlan_dev;
                new_vlan_dev =
			dev_get_by_name(param->u.set_sta_vlan.vlan_name);
                if (new_vlan_dev) {
#if 0
                        printk("%s: Station " MACSTR " moved to vlan: %s\n",
                               dev->name, MAC2STR(param->sta_addr),
                               new_vlan_dev->name);
#endif
                        sta->dev = new_vlan_dev;
			sta->vlan_id = param->u.set_sta_vlan.vlan_id;
                        dev_put(new_vlan_dev);
                }
		sta_info_release(local, sta);
	}

	return sta ? 0 : -ENOENT;
}


static int
ieee80211_ioctl_set_generic_info_elem(struct net_device *dev,
				      struct prism2_hostapd_param *param,
				      int param_len)
{
	struct ieee80211_local *local = dev->priv;
	u8 *pos = param->u.set_generic_info_elem.data;
        int left = param_len - ((u8 *) pos - (u8 *) param);
	int len = param->u.set_generic_info_elem.len;

	if (left < len)
		return -EINVAL;

	{
		struct ieee80211_sub_if_data *sdata;
		sdata = IEEE80211_DEV_TO_SUB_IF(dev);
		if (sdata->type == IEEE80211_SUB_IF_TYPE_STA)
			return ieee80211_sta_set_extra_ie(dev, pos, len);
	}
	kfree(local->conf.generic_elem);
	local->conf.generic_elem = kmalloc(len, GFP_KERNEL);
	if (local->conf.generic_elem == NULL)
		return -ENOMEM;
	memcpy(local->conf.generic_elem, pos, len);
	local->conf.generic_elem_len = len;

	return ieee80211_hw_config(dev);
}


static int ieee80211_ioctl_set_regulatory_domain(struct net_device *dev,
					    struct prism2_hostapd_param *param)
{
        struct ieee80211_conf *conf = ieee80211_get_hw_conf(dev);
        conf->regulatory_domain = param->u.set_regulatory_domain.rd;
        return 0;
}


static int ieee80211_ioctl_set_adm_status(struct net_device *dev,
					  int val)
{
        struct ieee80211_conf *conf = ieee80211_get_hw_conf(dev);
        conf->adm_status = val;
        return ieee80211_hw_config(dev);
}

static int
ieee80211_ioctl_set_tx_queue_params(struct net_device *dev,
				    struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_tx_queue_params qparam;

	if (!local->hw->conf_tx) {
		printk(KERN_DEBUG "%s: low-level driver does not support TX "
		       "queue configuration\n", dev->name);
		return -EOPNOTSUPP;
	}

	memset(&qparam, 0, sizeof(qparam));
	qparam.aifs = param->u.tx_queue_params.aifs;
	qparam.cw_min = param->u.tx_queue_params.cw_min;
	qparam.cw_max = param->u.tx_queue_params.cw_max;
	qparam.burst_time = param->u.tx_queue_params.burst_time;

	return local->hw->conf_tx(dev, param->u.tx_queue_params.queue,
				  &qparam);
}


static int ieee80211_ioctl_get_tx_stats(struct net_device *dev,
					struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_tx_queue_stats stats;
	int ret, i;

	if (!local->hw->get_tx_stats)
		return -EOPNOTSUPP;

	memset(&stats, 0, sizeof(stats));
	ret = local->hw->get_tx_stats(dev, &stats);
	if (ret)
		return ret;

	for (i = 0; i < 4; i++) {
		param->u.get_tx_stats.data[i].len = stats.data[i].len;
		param->u.get_tx_stats.data[i].limit = stats.data[i].limit;
		param->u.get_tx_stats.data[i].count = stats.data[i].count;
	}

	return 0;
}


static int ieee80211_ioctl_set_bss(struct net_device *dev,
				   struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_conf *conf = ieee80211_get_hw_conf(dev);
	int i, bss_count;
	int new_count = param->u.set_bss.bss_count;
	struct net_device **bss_devs, **prev;
	struct net_device **sta_devs, **prev_sta_devs;

	bss_count = 0;
	for (i = 0; i < conf->bss_count; i++) {
		if (local->bss_devs[i])
			bss_count++;
	}

	if (new_count < bss_count) {
		printk(KERN_DEBUG "%s: invalid BSS count %d (in use: %d)\n",
		       dev->name, new_count, bss_count);
		return -EINVAL;
	}

	bss_devs = kmalloc(new_count * sizeof(struct net_device *),
			   GFP_KERNEL);
	if (bss_devs == NULL)
		return -ENOMEM;
	sta_devs = kmalloc(new_count * sizeof(struct net_device *),
			   GFP_KERNEL);
	if (sta_devs == NULL) {
		kfree(bss_devs);
		return -ENOMEM;
	}

	spin_lock_bh(&local->sub_if_lock);
	memcpy(bss_devs, local->bss_devs,
	       bss_count * sizeof(struct net_device *));
	memset(&bss_devs[bss_count], 0,
	       (new_count - bss_count) * sizeof(struct net_device *));

	memcpy(conf->bssid_mask, param->u.set_bss.bssid_mask, ETH_ALEN);

	prev = local->bss_devs;
	local->bss_devs = bss_devs;
	conf->bss_count = new_count;

	memcpy(sta_devs, local->sta_devs,
	       bss_count * sizeof(struct net_device *));
	memset(&sta_devs[bss_count], 0,
	       (new_count - bss_count) * sizeof(struct net_device *));
	prev_sta_devs = local->sta_devs;
	local->sta_devs = sta_devs;

	spin_unlock_bh(&local->sub_if_lock);
	kfree(prev);
	kfree(prev_sta_devs);

	return ieee80211_hw_config(dev);
}


static int ieee80211_ioctl_set_channel_flag(struct net_device *dev,
					    struct prism2_hostapd_param *param)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_hw_modes *mode = NULL;
	struct ieee80211_channel *chan = NULL;
	int i;

	for (i = 0; i < local->hw->num_modes; i++) {
		mode = &local->hw->modes[i];
		if (mode->mode == param->u.set_channel_flag.mode)
			break;
		mode = NULL;
	}

	if (!mode)
		return -ENOENT;

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
	struct ieee80211_conf *conf = ieee80211_get_hw_conf(dev);
	conf->quiet_duration = param->u.quiet.duration;
	conf->quiet_offset = param->u.quiet.offset;
	conf->quiet_period = param->u.quiet.period;
	return 0;
}


static int ieee80211_ioctl_set_radar_params(struct net_device *dev,
					    struct prism2_hostapd_param *param)
{
	/* struct ieee80211_conf *conf = ieee80211_get_hw_conf(dev); */
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
	if (param == NULL)
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
	case PRISM2_HOSTAPD_SET_BSS:
		ret = ieee80211_ioctl_set_bss(dev, param);
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
	struct ieee80211_local *local = dev->priv;

	switch (local->conf.phymode) {
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
	struct ieee80211_local *local = dev->priv;
	int m, c;

	for (m = 0; m < local->hw->num_modes; m++) {
		struct ieee80211_hw_modes *mode = &local->hw->modes[m];
		for (c = 0; c < mode->num_channels; c++) {
			ieee80211_unmask_channel(dev, mode->mode,
						 &mode->channels[c]);
		}
	}
	return 0;
}


static int ieee80211_init_client(struct net_device *dev)
{
	if (ieee80211_regdom == 0x40)
		channel_range = ieee80211_mkk_channels;
	ieee80211_unmask_channels(dev);
	ieee80211_ioctl_set_adm_status(dev, 1);
	return 0;
}


static int ieee80211_is_client_mode(int iw_mode)
{
	return (iw_mode == IW_MODE_INFRA || iw_mode == IW_MODE_ADHOC);
}


static int ieee80211_ioctl_siwmode(struct net_device *dev,
				   struct iw_request_info *info,
				   __u32 *mode, char *extra)
{
	struct ieee80211_local *local = dev->priv;

	if (!ieee80211_is_client_mode(local->conf.mode) &&
	    ieee80211_is_client_mode(*mode)) {
		ieee80211_init_client(dev);
	}
	if (local->conf.mode != *mode) {
		struct ieee80211_sub_if_data *sdata =
			IEEE80211_DEV_TO_SUB_IF(dev);
		sta_info_flush(local, NULL);
		if (local->conf.mode == IW_MODE_ADHOC &&
		    sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
			/* Clear drop_unencrypted when leaving adhoc mode since
			 * only adhoc mode is using automatic setting for this
			 * in 80211.o. */
			sdata->drop_unencrypted = 0;
		}
		if (*mode == IW_MODE_MASTER) {
			/* AP mode does not currently use ACM bits to limit
			 * TX, so clear the bitfield here. */
			local->wmm_acm = 0;
		}
	}
	local->conf.mode = *mode;
	return ieee80211_hw_config(dev);
}


static int ieee80211_ioctl_giwmode(struct net_device *dev,
				   struct iw_request_info *info,
				   __u32 *mode, char *extra)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
		if (local->conf.mode == IW_MODE_ADHOC)
			*mode = IW_MODE_ADHOC;
		else
			*mode = IW_MODE_INFRA;
	} else
		*mode = local->conf.mode;
	return 0;
}


int ieee80211_ioctl_siwfreq(struct net_device *dev,
			    struct iw_request_info *info,
			    struct iw_freq *freq, char *extra)
{
	struct ieee80211_local *local = dev->priv;
	int m, c, nfreq, set = 0;

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

	for (m = 0; m < local->hw->num_modes; m++) {
		struct ieee80211_hw_modes *mode = &local->hw->modes[m];
		for (c = 0; c < mode->num_channels; c++) {
			struct ieee80211_channel *chan = &mode->channels[c];
			if (chan->flag & IEEE80211_CHAN_W_SCAN &&
			    ((freq->e == 0 && chan->chan == freq->m) ||
			     (freq->e > 0 && nfreq == chan->freq)) &&
			    (local->hw_modes & (1 << mode->mode))) {
				/* Use next_mode as the mode preference to
				 * resolve non-unique channel numbers. */
				if (set && mode->mode != local->next_mode)
					continue;

				local->conf.channel = chan->chan;
				local->conf.channel_val = chan->val;
                                local->conf.power_level = chan->power_level;
				local->conf.freq = chan->freq;
				local->conf.phymode = mode->mode;
                                local->conf.antenna_max = chan->antenna_max;
				set++;
			}
		}
	}

	if (set) {
		local->sta_scanning = 0; /* Abort possible scan */
		return ieee80211_hw_config(dev);
	}

	return -EINVAL;
}


static int ieee80211_ioctl_giwfreq(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_freq *freq, char *extra)
{
	struct ieee80211_local *local = dev->priv;

	/* TODO: in station mode (Managed/Ad-hoc) might need to poll low-level
	 * driver for the current channel with firmware-based management */

	freq->m = local->conf.freq;
	freq->e = 6;

	return 0;
}


static int ieee80211_ioctl_siwessid(struct net_device *dev,
				    struct iw_request_info *info,
				    struct iw_point *data, char *ssid)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sub_if_data *sdata;
        size_t len = data->length;

	/* iwconfig uses nul termination in SSID.. */
	if (len > 0 && ssid[len - 1] == '\0')
		len--;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_SUB_IF_TYPE_STA)
		return ieee80211_sta_set_ssid(dev, ssid, len);

	kfree(local->conf.ssid);
	local->conf.ssid = kmalloc(len + 1, GFP_KERNEL);
	if (local->conf.ssid == NULL)
		return -ENOMEM;
	memcpy(local->conf.ssid, ssid, len);
	local->conf.ssid[len] = '\0';
	local->conf.ssid_len = len;
	return ieee80211_hw_config(dev);
}


static int ieee80211_ioctl_giwessid(struct net_device *dev,
				    struct iw_request_info *info,
				    struct iw_point *data, char *ssid)
{
	struct ieee80211_local *local = dev->priv;
	size_t len;

	struct ieee80211_sub_if_data *sdata;
	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
		int res = ieee80211_sta_get_ssid(dev, ssid, &len);
		if (res == 0)
			data->length = len;
		return res;
	}

	len = local->conf.ssid_len;
	if (len > IW_ESSID_MAX_SIZE)
		len = IW_ESSID_MAX_SIZE;
	memcpy(ssid, local->conf.ssid, len);
	data->length = len;
	return 0;
}


static int ieee80211_ioctl_siwap(struct net_device *dev,
				 struct iw_request_info *info,
				 struct sockaddr *ap_addr, char *extra)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
		int changed_bssid = 0;
		if (memcmp(local->conf.client_bssid, (u8 *) &ap_addr->sa_data,
			   ETH_ALEN) != 0)
			changed_bssid = 1;
		memcpy(local->conf.client_bssid, (u8 *) &ap_addr->sa_data,
		       ETH_ALEN);
		if (changed_bssid && ieee80211_hw_config(dev)) {
			printk(KERN_DEBUG "%s: Failed to config new BSSID to "
			       "the low-level driver\n", dev->name);
		}
		return ieee80211_sta_set_bssid(dev, (u8 *) &ap_addr->sa_data);
	}

	return -EOPNOTSUPP;
}


static int ieee80211_ioctl_giwap(struct net_device *dev,
				 struct iw_request_info *info,
				 struct sockaddr *ap_addr, char *extra)
{
        struct ieee80211_sub_if_data *sdata;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
		ap_addr->sa_family = ARPHRD_ETHER;
		memcpy(&ap_addr->sa_data, sdata->u.sta.bssid, ETH_ALEN);
		return 0;
	}

	return -EOPNOTSUPP;
}


static int ieee80211_ioctl_siwscan(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_point *data, char *extra)
{
	struct ieee80211_local *local = dev->priv;
	u8 *ssid = NULL;
	size_t ssid_len = 0;

	if (local->scan_flags & IEEE80211_SCAN_MATCH_SSID) {
		ssid = local->conf.ssid;
		ssid_len = local->conf.ssid_len;
	}
	return ieee80211_sta_req_scan(dev, ssid, ssid_len);
}


static int ieee80211_ioctl_giwscan(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_point *data, char *extra)
{
	int res;
	struct ieee80211_local *local = dev->priv;
	if (local->sta_scanning)
		return -EAGAIN;
	res = ieee80211_sta_scan_results(dev, extra, IW_SCAN_MAX_DATA);
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
	struct ieee80211_local *local = dev->priv;

	if (rts->disabled)
		local->rts_threshold = IEEE80211_MAX_RTS_THRESHOLD;
	else if (rts->value < 0 || rts->value > IEEE80211_MAX_RTS_THRESHOLD)
		return -EINVAL;
	else
		local->rts_threshold = rts->value;

	/* If the wlan card performs RTS/CTS in hardware/firmware,
	 * configure it here */

	if (local->hw->set_rts_threshold) {
		local->hw->set_rts_threshold(dev, local->rts_threshold);
	}
	 
	return 0;
}

static int ieee80211_ioctl_giwrts(struct net_device *dev,
				  struct iw_request_info *info,
				  struct iw_param *rts, char *extra)
{
	struct ieee80211_local *local = dev->priv;

	rts->value = local->rts_threshold;
	rts->disabled = (rts->value >= IEEE80211_MAX_RTS_THRESHOLD);
	rts->fixed = 1;

	return 0;
}


static int ieee80211_ioctl_siwfrag(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_param *frag, char *extra)
{
	struct ieee80211_local *local = dev->priv;

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

	if (local->hw->set_frag_threshold) {
		local->hw->set_frag_threshold(
			dev, local->fragmentation_threshold);
	}
	 
	return 0;
}

static int ieee80211_ioctl_giwfrag(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_param *frag, char *extra)
{
	struct ieee80211_local *local = dev->priv;

	frag->value = local->fragmentation_threshold;
	frag->disabled = (frag->value >= IEEE80211_MAX_RTS_THRESHOLD);
	frag->fixed = 1;

	return 0;
}


static int ieee80211_ioctl_siwretry(struct net_device *dev,
				    struct iw_request_info *info,
				    struct iw_param *retry, char *extra)
{
	struct ieee80211_local *local = dev->priv;

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

	if (local->hw->set_retry_limit) {
		return local->hw->set_retry_limit(
			dev, local->short_retry_limit,
			local->long_retry_limit);
	}

	return 0;
}


static int ieee80211_ioctl_giwretry(struct net_device *dev,
				    struct iw_request_info *info,
				    struct iw_param *retry, char *extra)
{
	struct ieee80211_local *local = dev->priv;

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
	int m, c;

	for (m = 0; m < local->hw->num_modes; m++) {
		struct ieee80211_hw_modes *mode = &local->hw->modes[m];
		for (c = 0; c < mode->num_channels; c++) {
			struct ieee80211_channel *chan = &mode->channels[c];
			chan->flag |= IEEE80211_CHAN_W_SCAN;
		}
	}
}


static int ieee80211_ioctl_test_mode(struct net_device *dev, int mode)
{
	struct ieee80211_local *local = dev->priv;
	int ret = -EOPNOTSUPP;

	if (mode == IEEE80211_TEST_UNMASK_CHANNELS) {
		ieee80211_ioctl_unmask_channels(local);
		ret = 0;
	}

	if (local->hw->test_mode)
		ret = local->hw->test_mode(dev, mode);

	return ret;
}


static int ieee80211_ioctl_clear_keys(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_key_conf key;
	struct list_head *ptr;
	int i;
	u8 addr[ETH_ALEN];
	struct ieee80211_key_conf *keyconf;

	memset(addr, 0xff, ETH_ALEN);
	list_for_each(ptr, &local->sub_if_list)	{
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);
		for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
			keyconf = NULL;
			if (sdata->keys[i] &&
			    !sdata->keys[i]->force_sw_encrypt &&
			    local->hw->set_key &&
			    (keyconf = ieee80211_key_data2conf(local,
							       sdata->keys[i]))
			    != NULL)
				local->hw->set_key(dev, DISABLE_KEY, addr,
						   keyconf, 0);
			kfree(keyconf);
			kfree(sdata->keys[i]);
			sdata->keys[i] = NULL;
		}
		sdata->default_key = NULL;
	}

	spin_lock_bh(&local->sta_lock);
	list_for_each(ptr, &local->sta_list) {
		struct sta_info *sta =
			list_entry(ptr, struct sta_info, list);
		keyconf = NULL;
		if (sta->key && !sta->key->force_sw_encrypt &&
		    local->hw->set_key &&
		    (keyconf = ieee80211_key_data2conf(local, sta->key))
		    != NULL)
			local->hw->set_key(dev, DISABLE_KEY, sta->addr,
					   keyconf, sta->aid);
		kfree(keyconf);
		kfree(sta->key);
		sta->key = NULL;
	}
	spin_unlock_bh(&local->sta_lock);


	memset(&key, 0, sizeof(key));
	if (local->hw->set_key &&
		    local->hw->set_key(dev, REMOVE_ALL_KEYS, NULL,
				       &key, 0))
		printk(KERN_DEBUG "%s: failed to remove hwaccel keys\n",
		       dev->name);

	return 0;
}


static int
ieee80211_ioctl_force_unicast_rate(struct net_device *dev,
				   struct ieee80211_sub_if_data *sdata,
				   int rate)
{
	struct ieee80211_local *local = dev->priv;
	int i;

	if (sdata->type != IEEE80211_SUB_IF_TYPE_NORM)
		return -ENOENT;

	if (rate == 0) {
		sdata->u.norm.force_unicast_rateidx = -1;
		return 0;
	}

	for (i = 0; i < local->num_curr_rates; i++) {
		if (local->curr_rates[i].rate == rate) {
			sdata->u.norm.force_unicast_rateidx = i;
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
	struct ieee80211_local *local = dev->priv;
	int i;

	if (sdata->type != IEEE80211_SUB_IF_TYPE_NORM)
		return -ENOENT;

	if (rate == 0) {
		sdata->u.norm.max_ratectrl_rateidx = -1;
		return 0;
	}

	for (i = 0; i < local->num_curr_rates; i++) {
		if (local->curr_rates[i].rate == rate) {
			sdata->u.norm.max_ratectrl_rateidx = i;
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

	if (key == NULL || key->alg != ALG_WEP || !key->force_sw_encrypt ||
	    local->hw->device_hides_wep)
		return;

	memset(addr, 0xff, ETH_ALEN);
	keyconf = ieee80211_key_data2conf(local, key);
	if (keyconf && local->hw->set_key &&
	    local->hw->set_key(local->mdev, SET_KEY, addr, keyconf, 0) == 0) {
		key->force_sw_encrypt = keyconf->force_sw_encrypt;
		key->hw_key_idx = keyconf->hw_key_idx;
	}
	kfree(keyconf);
}


static void ieee80211_key_disable_hwaccel(struct ieee80211_local *local,
					  struct ieee80211_key *key)
{
	struct ieee80211_key_conf *keyconf;
	u8 addr[ETH_ALEN];

	if (key == NULL || key->alg != ALG_WEP || key->force_sw_encrypt ||
	    local->hw->device_hides_wep)
		return;

	memset(addr, 0xff, ETH_ALEN);
	keyconf = ieee80211_key_data2conf(local, key);
	if (keyconf && local->hw->set_key)
		local->hw->set_key(local->mdev, DISABLE_KEY, addr, keyconf, 0);
	kfree(keyconf);
	key->force_sw_encrypt = 1;
}


static int ieee80211_ioctl_default_wep_only(struct ieee80211_local *local,
					    int value)
{
	int i;
	struct list_head *ptr;

	local->default_wep_only = value;
	list_for_each(ptr, &local->sub_if_list)	{
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);
		for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
			if (value) {
				ieee80211_key_enable_hwaccel(local,
							     sdata->keys[i]);
			} else {
				ieee80211_key_disable_hwaccel(local,
							      sdata->keys[i]);
			}
		}
	}

	return 0;
}


static int ieee80211_ioctl_prism2_param(struct net_device *dev,
					struct iw_request_info *info,
					void *wrqu, char *extra)
{
	struct ieee80211_local *local = dev->priv;
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
		local->conf.beacon_int = value;
		if (ieee80211_hw_config(dev))
			ret = -EINVAL;
		break;

	case PRISM2_PARAM_AP_BRIDGE_PACKETS:
		local->bridge_packets = value;
		break;

	case PRISM2_PARAM_AP_AUTH_ALGS:
		if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
			sdata->u.sta.auth_algs = value;
		} else
			ret = -EOPNOTSUPP;
		break;

	case PRISM2_PARAM_DTIM_PERIOD:
		if (value < 1)
			ret = -EINVAL;
		else if (sdata->type != IEEE80211_SUB_IF_TYPE_NORM)
			ret = -ENOENT;
		else
			sdata->u.norm.dtim_period = value;
		break;

	case PRISM2_PARAM_IEEE_802_1X:
		sdata->ieee802_1x = value;
		if (local->hw->set_ieee8021x &&
		    local->hw->set_ieee8021x(dev, value))
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

	case PRISM2_PARAM_RATE_LIMIT_BURST:
		local->rate_limit_burst = value;
                local->rate_limit_bucket = value;
		break;

	case PRISM2_PARAM_RATE_LIMIT:
		/* number of packets (tokens) allowed per second */
		if (!local->rate_limit && value) {
                        if (!local->rate_limit_burst) local->rate_limit_burst =
				value;
                        local->rate_limit_bucket = local->rate_limit_burst;
			local->rate_limit_timer.expires = jiffies + HZ;
			add_timer(&local->rate_limit_timer);
		} else if (local->rate_limit && !value) {
			del_timer_sync(&local->rate_limit_timer);
		}
		local->rate_limit = value;
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
		local->conf.short_slot_time = value;
		if (ieee80211_hw_config(dev))
			ret = -EINVAL;
		break;

        case PRISM2_PARAM_PRIVACY_INVOKED:
		if (local->hw->set_privacy_invoked)
			ret = local->hw->set_privacy_invoked(dev, value);
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

	case PRISM2_PARAM_ADM_STATUS:
                ret = ieee80211_ioctl_set_adm_status(dev, value);
		break;

	case PRISM2_PARAM_ANTENNA_SEL:
		local->conf.antenna_sel = value;
		if (ieee80211_hw_config(dev))
			ret = -EINVAL;
		break;

	case PRISM2_PARAM_CALIB_INT:
		local->conf.calib_int = value;
		if (ieee80211_hw_config(dev))
			ret = -EINVAL;
		break;

	case PRISM2_PARAM_ANTENNA_MODE:
		local->conf.antenna_mode = value;
		if (ieee80211_hw_config(dev))
			ret = -EINVAL;
		break;

	case PRISM2_PARAM_BROADCAST_SSID:
		if ((value < 0) || (value > 1))
			ret = -EINVAL;
		else
			local->conf.ssid_hidden = value;
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
			local->conf.tx_power_reduction = value;
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
		else if (sdata->keys[value] == NULL)
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
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			ret = -EINVAL;
		else
			sdata->u.sta.mixed_cell = !!value;
		break;

	case PRISM2_PARAM_KEY_MGMT:
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			ret = -EINVAL;
		else
			sdata->u.sta.key_mgmt = value;
		break;

	case PRISM2_PARAM_HW_MODES:
		local->hw_modes = value;
		break;

	case PRISM2_PARAM_CREATE_IBSS:
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			ret = -EINVAL;
		else
			sdata->u.sta.create_ibss = !!value;
		break;
	case PRISM2_PARAM_WMM_ENABLED:
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			ret = -EINVAL;
		else
			sdata->u.sta.wmm_enabled = !!value;
		break;
	case PRISM2_PARAM_RADAR_DETECT:
		local->conf.radar_detect = value;
		break;
	case PRISM2_PARAM_SPECTRUM_MGMT:
		local->conf.spect_mgmt = value;
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
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sub_if_data *sdata;
	int *param = (int *) extra;
	int ret = 0;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	switch (*param) {
	case PRISM2_PARAM_BEACON_INT:
		*param = local->conf.beacon_int;
		break;

	case PRISM2_PARAM_AP_BRIDGE_PACKETS:
		*param = local->bridge_packets;
		break;

	case PRISM2_PARAM_AP_AUTH_ALGS:
		if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
			*param = sdata->u.sta.auth_algs;
		} else
			ret = -EOPNOTSUPP;
		break;

	case PRISM2_PARAM_DTIM_PERIOD:
		if (sdata->type != IEEE80211_SUB_IF_TYPE_NORM)
			ret = -ENOENT;
		else
			*param = sdata->u.norm.dtim_period;
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

	case PRISM2_PARAM_RATE_LIMIT_BURST:
		*param = local->rate_limit_burst;
		break;

	case PRISM2_PARAM_RATE_LIMIT:
		*param = local->rate_limit;
		break;

        case PRISM2_PARAM_STAT_TIME:
                *param = local->stat_time;
                break;
	case PRISM2_PARAM_SHORT_SLOT_TIME:
		*param = local->conf.short_slot_time;
		break;

	case PRISM2_PARAM_NEXT_MODE:
		*param = local->next_mode;
		break;

	case PRISM2_PARAM_ANTENNA_SEL:
		*param = local->conf.antenna_sel;
		break;

	case PRISM2_PARAM_CALIB_INT:
		*param = local->conf.calib_int;
		break;

	case PRISM2_PARAM_ANTENNA_MODE:
		*param = local->conf.antenna_mode;
		break;

	case PRISM2_PARAM_BROADCAST_SSID:
		*param = local->conf.ssid_hidden;
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
		*param = local->conf.tx_power_reduction;
		break;

	case PRISM2_PARAM_EAPOL:
		*param = sdata->eapol;
		break;

	case PRISM2_PARAM_KEY_TX_RX_THRESHOLD:
		*param = local->key_tx_rx_threshold;
		break;

	case PRISM2_PARAM_KEY_INDEX:
		if (sdata->default_key == NULL)
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
		*param = local->hw_modes;
		break;

	case PRISM2_PARAM_CREATE_IBSS:
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			ret = -EINVAL;
		else
			*param = !!sdata->u.sta.create_ibss;
		break;

	case PRISM2_PARAM_MIXED_CELL:
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			ret = -EINVAL;
		else
			*param = !!sdata->u.sta.mixed_cell;
		break;

	case PRISM2_PARAM_KEY_MGMT:
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			ret = -EINVAL;
		else
			*param = sdata->u.sta.key_mgmt;
		break;
	case PRISM2_PARAM_WMM_ENABLED:
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			ret = -EINVAL;
		else
			*param = !!sdata->u.sta.wmm_enabled;
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
	struct ieee80211_local *local = dev->priv;
	int *i = (int *) extra;
	int param = *i;
	int value = *(i + 1);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (local->hw->test_param)
		return local->hw->test_param(local->mdev, param, value);

	return -EOPNOTSUPP;
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


static int ieee80211_ioctl_giwpriv(struct net_device *dev,
				   struct iw_point *data)
{

	if (!data->pointer ||
	    !access_ok(VERIFY_WRITE, data->pointer,
			sizeof(ieee80211_ioctl_priv)))
		return -EINVAL;

	data->length = sizeof(ieee80211_ioctl_priv) /
		sizeof(ieee80211_ioctl_priv[0]);
	if (copy_to_user(data->pointer, ieee80211_ioctl_priv,
			 sizeof(ieee80211_ioctl_priv)))
		return -EINVAL;
	return 0;
}


int ieee80211_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct iwreq *wrq = (struct iwreq *) rq;
	int ret = 0;
	char ssid[IW_ESSID_MAX_SIZE + 1];

	switch (cmd) {
	case SIOCGIWNAME:
		ret = ieee80211_ioctl_giwname(dev, NULL, (char *) &wrq->u,
					      NULL);
		break;
	case SIOCSIWESSID:
		if (!wrq->u.essid.pointer)
			ret = -EINVAL;
		else if (wrq->u.essid.length > IW_ESSID_MAX_SIZE)
			ret = -E2BIG;
		else {
			if (copy_from_user(ssid, wrq->u.essid.pointer,
					   wrq->u.essid.length)) {
				ret = -EFAULT;
				break;
			}
			ret = ieee80211_ioctl_siwessid(dev, NULL,
						       &wrq->u.essid, ssid);
		}
		break;
	case SIOCGIWESSID:
		if (!wrq->u.essid.pointer) {
			ret = -EINVAL;
		} else {
			memset(ssid, 0, IW_ESSID_MAX_SIZE + 1);
			ret = ieee80211_ioctl_giwessid(dev, NULL,
						       &wrq->u.essid, ssid);
			if (copy_to_user(wrq->u.essid.pointer, ssid,
					 wrq->u.essid.length)) {
				ret = -EFAULT;
				break;
			}
		}
		break;

	case SIOCGIWRANGE:
	{
		struct iw_range range;
		if (!access_ok(VERIFY_WRITE, wrq->u.data.pointer,
				  sizeof(range))) {
			ret = -EFAULT;
			break;
		}
		ret = ieee80211_ioctl_giwrange(dev, NULL, &wrq->u.data,
					       (char *) &range);
		if (ret)
			break;
		if (copy_to_user(wrq->u.data.pointer, &range, sizeof(range)))
			ret = -EFAULT;
		break;
	}

	case SIOCSIWAP:
		ret = ieee80211_ioctl_siwap(dev, NULL, &wrq->u.ap_addr, NULL);
		break;
	case SIOCGIWAP:
		ret = ieee80211_ioctl_giwap(dev, NULL, &wrq->u.ap_addr, NULL);
		break;
	case SIOCSIWSCAN:
		ret = ieee80211_ioctl_siwscan(dev, NULL, &wrq->u.data, NULL);
		break;
	case SIOCGIWSCAN:
	{
		char *buf = kmalloc(IW_SCAN_MAX_DATA, GFP_KERNEL);
		if (buf == NULL) {
			ret = -ENOMEM;
			break;
		}
		ret = ieee80211_ioctl_giwscan(dev, NULL, &wrq->u.data, buf);
		if (ret == 0 &&
		    copy_to_user(wrq->u.data.pointer, buf, wrq->u.data.length))
			ret = -EFAULT;
		kfree(buf);
		break;
	}

	case SIOCSIWFREQ:
		ret = ieee80211_ioctl_siwfreq(dev, NULL, &wrq->u.freq, NULL);
		break;
	case SIOCGIWFREQ:
		ret = ieee80211_ioctl_giwfreq(dev, NULL, &wrq->u.freq, NULL);
		break;
	case SIOCSIWMODE:
		ret = ieee80211_ioctl_siwmode(dev, NULL, &wrq->u.mode, NULL);
		break;
	case SIOCGIWMODE:
		ret = ieee80211_ioctl_giwmode(dev, NULL, &wrq->u.mode, NULL);
		break;

	case SIOCSIWRTS:
		ret = ieee80211_ioctl_siwrts(dev, NULL, &wrq->u.rts, NULL);
		break;
	case SIOCGIWRTS:
		ret = ieee80211_ioctl_giwrts(dev, NULL, &wrq->u.rts, NULL);
		break;

	case SIOCSIWFRAG:
		ret = ieee80211_ioctl_siwfrag(dev, NULL, &wrq->u.frag, NULL);
		break;
	case SIOCGIWFRAG:
		ret = ieee80211_ioctl_giwfrag(dev, NULL, &wrq->u.frag, NULL);
		break;

	case SIOCSIWRETRY:
		ret = ieee80211_ioctl_siwretry(dev, NULL, &wrq->u.retry, NULL);
		break;
	case SIOCGIWRETRY:
		ret = ieee80211_ioctl_giwretry(dev, NULL, &wrq->u.retry, NULL);
		break;

	case PRISM2_IOCTL_PRISM2_PARAM:
		ret = ieee80211_ioctl_prism2_param(dev, NULL, &wrq->u,
						   (char *) &wrq->u);
		break;
	case PRISM2_IOCTL_GET_PRISM2_PARAM:
		ret = ieee80211_ioctl_get_prism2_param(dev, NULL, &wrq->u,
						       (char *) &wrq->u);
		break;
	case PRISM2_IOCTL_TEST_PARAM:
		ret = ieee80211_ioctl_test_param(dev, NULL, &wrq->u,
						 (char *) &wrq->u);
		break;
	case PRISM2_IOCTL_HOSTAPD:
		if (!capable(CAP_NET_ADMIN)) ret = -EPERM;
		else ret = ieee80211_ioctl_priv_hostapd(dev, &wrq->u.data);
		break;
	case SIOCGIWPRIV:
		ret = ieee80211_ioctl_giwpriv(dev, &wrq->u.data);
		break;
	default:
		ret = -EOPNOTSUPP;
		break;
	}

	return ret;
}
