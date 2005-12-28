/*
 * Copyright 2003-2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>

#ifdef CONFIG_PROC_FS

#include <net/ieee80211.h>
#include <net/ieee80211_common.h>
#include <net/ieee80211_mgmt.h>
#include "ieee80211_i.h"
#include "sta_info.h"
#include "ieee80211_proc.h"
#include "rate_control.h"


static struct proc_dir_entry *ieee80211_proc;

#define PROC_LIMIT (PAGE_SIZE - 80)


static char * ieee80211_proc_key(char *p, struct ieee80211_key *key,
				 int idx, int def_key)
{
	int i;
	u8 *tpn, *rpn;

	if (!key)
		return p;

	p += sprintf(p, "key[%d]%s len=%d sw_encrypt=%d idx=%d hwidx=%d "
		     "tx_rx_count=%d",
		     idx, def_key ? "*" : "", key->keylen,
		     key->force_sw_encrypt, key->keyidx, key->hw_key_idx,
		     key->tx_rx_count);
	switch (key->alg) {
	case ALG_WEP:
		p += sprintf(p, " alg=WEP");
		break;
	case ALG_TKIP:
		p += sprintf(p, " alg=TKIP iv(tx)=%08x %04x",
			     key->u.tkip.iv32, key->u.tkip.iv16);
		for (i = 0; i < NUM_RX_DATA_QUEUES; i++) {
			if (key->u.tkip.iv32_rx[i] == 0 &&
			    key->u.tkip.iv16_rx[i] == 0)
				continue;
			p += sprintf(p, " iv(rx %d)=%08x %04x", i,
				     key->u.tkip.iv32_rx[i],
				     key->u.tkip.iv16_rx[i]);
		}
		break;
	case ALG_CCMP:
		tpn = key->u.ccmp.tx_pn;
		p += sprintf(p, " alg=CCMP PN(tx)=%02x%02x%02x%02x%02x%02x",
			     tpn[0], tpn[1], tpn[2], tpn[3], tpn[4], tpn[5]);
		for (i = 0; i < NUM_RX_DATA_QUEUES; i++) {
			rpn = key->u.ccmp.rx_pn[i];
			if (memcmp(rpn, "\x00\x00\x00\x00\x00\x00", 6) == 0)
				continue;
			p += sprintf(p, " PN(rx %d)=%02x%02x%02x%02x%02x%02x",
				     i, rpn[0], rpn[1], rpn[2], rpn[3], rpn[4],
				     rpn[5]);
		}
		p += sprintf(p, " replays=%u", key->u.ccmp.replays);
		break;
	default:
		break;
	}

	p += sprintf(p, " key=");
	for (i = 0; i < key->keylen; i++)
		p += sprintf(p, "%02x", key->key[i]);
	p += sprintf(p, "\n");
	return p;
}


static char * ieee80211_proc_sub_if_norm(char *p,
					 struct ieee80211_if_norm *norm)
{
	p += sprintf(p, "type=norm\n");
	if (norm->beacon_head)
		p += sprintf(p, "beacon_head_len=%d\n", norm->beacon_head_len);
	if (norm->beacon_tail)
		p += sprintf(p, "beacon_tail_len=%d\n", norm->beacon_tail_len);
	p += sprintf(p,
		     "max_aid=%d\n"
		     "num_sta_ps=%d\n"
		     "num_buffered_multicast=%u\n"
		     "dtim_period=%d\n"
		     "dtim_count=%d\n"
		     "num_beacons=%d\n"
		     "force_unicast_rateidx=%d\n"
		     "max_ratectrl_rateidx=%d\n",
		     norm->max_aid, atomic_read(&norm->num_sta_ps),
		     skb_queue_len(&norm->ps_bc_buf),
		     norm->dtim_period, norm->dtim_count, norm->num_beacons,
		     norm->force_unicast_rateidx, norm->max_ratectrl_rateidx);
	return p;
}


static char * ieee80211_proc_sub_if_sta(char *p,
					struct ieee80211_if_sta *ifsta)
{
	p += sprintf(p, "type=sta\n");
	p += sprintf(p,
		     "state=%d\n"
		     "bssid=" MACSTR "\n"
		     "prev_bssid=" MACSTR "\n"
		     "ssid_len=%zd\n"
		     "aid=%d\n"
		     "ap_capab=0x%x\n"
		     "capab=0x%x\n"
		     "extra_ie_len=%zd\n"
		     "auth_tries=%d\n"
		     "assoc_tries=%d\n"
		     "flags=%s%s%s%s%s%s%s\n"
		     "auth_algs=0x%x\n"
		     "auth_alg=%d\n"
		     "auth_transaction=%d\n",
		     ifsta->state,
		     MAC2STR(ifsta->bssid),
		     MAC2STR(ifsta->prev_bssid),
		     ifsta->ssid_len,
		     ifsta->aid,
		     ifsta->ap_capab,
		     ifsta->capab,
		     ifsta->extra_ie_len,
		     ifsta->auth_tries,
		     ifsta->assoc_tries,
		     ifsta->ssid_set ? "[SSID]" : "",
		     ifsta->bssid_set ? "[BSSID]" : "",
		     ifsta->prev_bssid_set ? "[prev BSSID" : "",
		     ifsta->authenticated ? "[AUTH]" : "",
		     ifsta->associated ? "[ASSOC]" : "",
		     ifsta->probereq_poll ? "[PROBEREQ POLL]" : "",
		     ifsta->use_protection ? "[CTS prot]" : "",
		     ifsta->auth_algs,
		     ifsta->auth_alg,
		     ifsta->auth_transaction);
	return p;
}


static char * ieee80211_proc_sub_if(char *p,
				    struct ieee80211_sub_if_data *sdata)
{
	if (sdata == NULL)
		return p;

	if (sdata->bss)
		p += sprintf(p, "bss=%p\n", sdata->bss);

	switch (sdata->type) {
	case IEEE80211_SUB_IF_TYPE_NORM:
		p = ieee80211_proc_sub_if_norm(p, &sdata->u.norm);
		break;
	case IEEE80211_SUB_IF_TYPE_WDS:
		p += sprintf(p, "type=wds\n");
		p += sprintf(p, "wds.peer=" MACSTR "\n",
			     MAC2STR(sdata->u.wds.remote_addr));
		break;
	case IEEE80211_SUB_IF_TYPE_VLAN:
		p += sprintf(p, "type=vlan\n");
		p += sprintf(p, "vlan.id=%d\n", sdata->u.vlan.id);
		break;
	case IEEE80211_SUB_IF_TYPE_STA:
		p = ieee80211_proc_sub_if_sta(p, &sdata->u.sta);
		break;
	}
	p += sprintf(p, "channel_use=%d\n", sdata->channel_use);
	p += sprintf(p, "drop_unencrypted=%d\n", sdata->drop_unencrypted);
	p += sprintf(p, "eapol=%d\n", sdata->eapol);
	p += sprintf(p, "ieee802_1x=%d\n", sdata->ieee802_1x);

	return p;
}


static int ieee80211_proc_iface_read(char *page, char **start, off_t off,
				     int count, int *eof, void *data)
{
	char *p = page;
	struct net_device *dev = (struct net_device *) data;
	struct ieee80211_sub_if_data *sdata;
	int i;

	if (off != 0) {
		*eof = 1;
		return 0;
	}

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (!sdata)
		return -1;

        p = ieee80211_proc_sub_if(p, sdata);

	for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
		if (sdata->keys[i] == NULL)
			continue;

		p = ieee80211_proc_key(p, sdata->keys[i], i,
				       sdata->keys[i] == sdata->default_key);
	}

	return (p - page);
}


static int ieee80211_proc_sta_read(char *page, char **start, off_t off,
				   int count, int *eof, void *data)
{
	char *p = page;
	struct sta_info *sta = (struct sta_info *) data;
	struct ieee80211_local *local;
	int inactive, i;

	if (off != 0) {
		*eof = 1;
		return 0;
	}

	if (!sta || !sta->dev)
		return -1;

	p += sprintf(p, "users=%d\n", atomic_read(&sta->users));
	p += sprintf(p, "aid=%d\n", sta->aid);
	p += sprintf(p, "flags=0x%x %s%s%s%s%s%s%s%s%s%s\n", sta->flags,
		     sta->flags & WLAN_STA_AUTH ? "[AUTH]" : "",
		     sta->flags & WLAN_STA_ASSOC ? "[ASSOC]" : "",
		     sta->flags & WLAN_STA_PS ? "[PS]" : "",
		     sta->flags & WLAN_STA_TIM ? "[TIM]" : "",
		     sta->flags & WLAN_STA_PERM ? "[PERM]" : "",
		     sta->flags & WLAN_STA_AUTHORIZED ? "[AUTHORIZED]" : "",
		     sta->flags & WLAN_STA_SHORT_PREAMBLE ?
		     "[SHORT PREAMBLE]" : "",
		     sta->flags & WLAN_STA_WME ? "[WME]" : "",
		     sta->flags & WLAN_STA_WDS ? "[WDS]" : "",
		     sta->flags & WLAN_STA_XR ? "[XR]" : "");
	p += sprintf(p, "key_idx_compression=%d\n",
		     sta->key_idx_compression);
	p += sprintf(p, "dev=%s\n", sta->dev->name);
	if (sta->vlan_id > 0)
		p += sprintf(p, "vlan_id=%d\n", sta->vlan_id);
	p += sprintf(p, "rx_packets=%lu\ntx_packets=%lu\nrx_bytes=%lu\n"
		     "tx_bytes=%lu\nrx_duplicates=%lu\nrx_fragments=%lu\n"
		     "rx_dropped=%lu\ntx_fragments=%lu\ntx_filtered=%lu\n",
		     sta->rx_packets, sta->tx_packets,
		     sta->rx_bytes, sta->tx_bytes,
		     sta->num_duplicates, sta->rx_fragments, sta->rx_dropped,
		     sta->tx_fragments, sta->tx_filtered_count);
	p = ieee80211_proc_key(p, sta->key, 0, 1);

	local = (struct ieee80211_local *) sta->dev->priv;
	if (sta->txrate >= 0 && sta->txrate < local->num_curr_rates) {
		p += sprintf(p, "txrate=%d\n",
			     local->curr_rates[sta->txrate].rate);
	}
	if (sta->last_txrate >= 0 &&
	    sta->last_txrate < local->num_curr_rates) {
		p += sprintf(p, "last_txrate=%d\n",
			     local->curr_rates[sta->last_txrate].rate);
	}
	p += sprintf(p, "num_ps_buf_frames=%u\n",
		     skb_queue_len(&sta->ps_tx_buf));
	p += sprintf(p, "tx_retry_failed=%lu\n", sta->tx_retry_failed);
	p += sprintf(p, "tx_retry_count=%lu\n", sta->tx_retry_count);
	p += sprintf(p, "last_rssi=%d\n", sta->last_rssi);
	p += sprintf(p, "last_ack_rssi=%d %d %d\n",
		     sta->last_ack_rssi[0], sta->last_ack_rssi[1],
		     sta->last_ack_rssi[2]);
	if (sta->last_ack)
		p += sprintf(p, "last_ack_ms=%d\n",
			     jiffies_to_msecs(jiffies - sta->last_ack));
	inactive = jiffies - sta->last_rx;
	p += sprintf(p, "inactive_msec=%d\n", jiffies_to_msecs(inactive));
	p += sprintf(p, "channel_use=%d\n", sta->channel_use);
	p += sprintf(p, "wep_weak_iv_count=%d\n", sta->wep_weak_iv_count);
#ifdef CONFIG_IEEE80211_DEBUG_COUNTERS
	p += sprintf(p, "wme_rx_queue=");
	for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
		p += sprintf(p, "%u ", sta->wme_rx_queue[i]);
	p += sprintf(p, "\n");

	p += sprintf(p, "wme_tx_queue=");
	for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
		p += sprintf(p, "%u ", sta->wme_tx_queue[i]);
	p += sprintf(p, "\n");
#endif /* CONFIG_IEEE80211_DEBUG_COUNTERS */
	p += sprintf(p, "last_seq_ctrl=");
	for (i = 0; i < NUM_RX_DATA_QUEUES; i++) {
			p += sprintf(p, "%x ", sta->last_seq_ctrl[i]);
	}
	p += sprintf(p, "\n");

	p += rate_control_status_sta(local, sta, p);

	return (p - page);
}


static int ieee80211_proc_counters_read(char *page, char **start, off_t off,
					int count, int *eof, void *data)
{
	char *p = page;
	struct ieee80211_local *local = (struct ieee80211_local *) data;
        struct ieee80211_low_level_stats stats;

	if (off != 0) {
		*eof = 1;
		return 0;
	}

	p += sprintf(p,
		     "TransmittedFragmentCount=%u\n"
		     "MulticastTransmittedFrameCount=%u\n"
		     "FailedCount=%u\n"
		     "RetryCount=%u\n"
		     "MultipleRetryCount=%d\n"
		     "FrameDuplicateCount=%d\n"
		     "ReceivedFragmentCount=%u\n"
		     "MulticastReceivedFrameCount=%u\n"
		     "TransmittedFrameCount=%u\n"
		     "WEPUndecryptableCount=%u\n",
		     local->dot11TransmittedFragmentCount,
		     local->dot11MulticastTransmittedFrameCount,
		     local->dot11FailedCount,
		     local->dot11RetryCount,
		     local->dot11MultipleRetryCount,
		     local->dot11FrameDuplicateCount,
		     local->dot11ReceivedFragmentCount,
		     local->dot11MulticastReceivedFrameCount,
		     local->dot11TransmittedFrameCount,
		     local->dot11WEPUndecryptableCount);

	memset(&stats, 0, sizeof(stats));
	if (local->hw->get_stats &&
	    local->hw->get_stats(local->mdev, &stats) == 0) {
		p += sprintf(p,
			     "ACKFailureCount=%u\n"
			     "RTSFailureCount=%u\n"
			     "FCSErrorCount=%u\n"
			     "RTSSuccessCount=%u\n",
			     stats.dot11ACKFailureCount,
			     stats.dot11RTSFailureCount,
			     stats.dot11FCSErrorCount,
			     stats.dot11RTSSuccessCount);
	}

	return (p - page);
}


static int ieee80211_proc_debug_read(char *page, char **start, off_t off,
				     int count, int *eof, void *data)
{
	char *p = page;
	struct ieee80211_local *local = (struct ieee80211_local *) data;
	int i;

	if (off != 0) {
		*eof = 1;
		return 0;
	}

#ifdef CONFIG_IEEE80211_DEBUG_COUNTERS
	p += sprintf(p,
		     "tx_handlers_drop=%u\n"
		     "tx_handlers_queued=%u\n"
		     "tx_handlers_drop_unencrypted=%u\n"
		     "tx_handlers_drop_fragment=%u\n"
		     "tx_handlers_drop_wep=%u\n"
		     "tx_handlers_drop_rate_limit=%u\n"
		     "tx_handlers_drop_not_assoc=%u\n"
		     "tx_handlers_drop_unauth_port=%u\n"
		     "rx_handlers_drop=%u\n"
		     "rx_handlers_queued=%u\n"
		     "rx_handlers_drop_nullfunc=%u\n"
		     "rx_handlers_drop_defrag=%u\n"
		     "rx_handlers_drop_short=%u\n"
		     "rx_handlers_drop_passive_scan=%u\n"
		     "tx_expand_skb_head=%u\n"
		     "tx_expand_skb_head_cloned=%u\n"
		     "rx_expand_skb_head=%u\n"
		     "rx_expand_skb_head2=%u\n"
		     "rx_handlers_fragments=%u\n"
		     "tx_status_drop=%u\n",
		     local->tx_handlers_drop,
		     local->tx_handlers_queued,
		     local->tx_handlers_drop_unencrypted,
		     local->tx_handlers_drop_fragment,
		     local->tx_handlers_drop_wep,
		     local->tx_handlers_drop_rate_limit,
		     local->tx_handlers_drop_not_assoc,
		     local->tx_handlers_drop_unauth_port,
		     local->rx_handlers_drop,
		     local->rx_handlers_queued,
		     local->rx_handlers_drop_nullfunc,
		     local->rx_handlers_drop_defrag,
		     local->rx_handlers_drop_short,
		     local->rx_handlers_drop_passive_scan,
		     local->tx_expand_skb_head,
		     local->tx_expand_skb_head_cloned,
		     local->rx_expand_skb_head,
		     local->rx_expand_skb_head2,
		     local->rx_handlers_fragments,
		     local->tx_status_drop);
	{
		int i;
		p += sprintf(p, "wme_rx_queue=");
		for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
			p += sprintf(p, " %u", local->wme_rx_queue[i]);
		p += sprintf(p, "\n");

		p += sprintf(p, "wme_tx_queue=");
		for (i = 0; i < NUM_RX_DATA_QUEUES; i++)
			p += sprintf(p, " %u", local->wme_tx_queue[i]);
		p += sprintf(p, "\n");
	}
#endif /* CONFIG_IEEE80211_DEBUG_COUNTERS */

	p += sprintf(p, "num_scans=%u\n", local->scan.num_scans);

	p += sprintf(p,
		     "conf.bss_count=%d\n"
		     "bss_dev_count=%u\n",
		     local->conf.bss_count, local->bss_dev_count);
	for (i = 0; i < local->conf.bss_count; i++) {
		p += sprintf(p, "bss_dev[%d]=%p (%s)\n",
			     i, local->bss_devs[i],
			     (i < local->bss_dev_count && local->bss_devs[i]) ?
			     local->bss_devs[i]->name : "N/A");
	}

	return (p - page);
}


static const char * ieee80211_mode_str_short(int mode)
{
	switch (mode) {
	case MODE_IEEE80211A:
		return "802.11a";
	case MODE_IEEE80211B:
		return "802.11b";
	case MODE_IEEE80211G:
		return "802.11g";
	case MODE_ATHEROS_TURBO:
		return "AtherosTurbo";
	default:
		return "UNKNOWN";
	}
}


static const char * ieee80211_mode_str(int mode)
{
	switch (mode) {
	case MODE_IEEE80211A:
		return "IEEE 802.11a";
	case MODE_IEEE80211B:
		return "IEEE 802.11b";
	case MODE_IEEE80211G:
		return "IEEE 802.11g";
	case MODE_ATHEROS_TURBO:
		return "Atheros Turbo (5 GHz)";
	default:
		return "UNKNOWN";
	}
}


static int ieee80211_proc_info_read(char *page, char **start, off_t off,
				    int count, int *eof, void *data)
{
	char *p = page;
	struct ieee80211_local *local = (struct ieee80211_local *) data;
	int m;
	struct ieee80211_hw_modes *mode;

	if (off != 0) {
		*eof = 1;
		return 0;
	}

	p += sprintf(p, "hw_name=%s\n", local->hw->name);
	p += sprintf(p, "modes=");
	for (m = 0; m < local->hw->num_modes; m++) {
		mode = &local->hw->modes[m];
		p += sprintf(p, "[%s]", ieee80211_mode_str_short(mode->mode));
	}
	p += sprintf(p, "\n");
	if (local->rate_ctrl && local->rate_ctrl_priv)
		p+= sprintf(p, "rate_ctrl_alg=%s\n", local->rate_ctrl->name);
	return (p - page);
}


static int ieee80211_proc_config_read(char *page, char **start, off_t off,
				      int count, int *eof, void *data)
{
	char *p = page;
	struct ieee80211_local *local = (struct ieee80211_local *) data;

	if (off != 0) {
		*eof = 1;
		return 0;
	}

	p += sprintf(p,
		     "low_level_driver=%s\n"
		     "channel=%d\n"
		     "freq=%d\n"
		     "mode=%s\n"
			 "802.11h=%d\n"
		     "wep_iv=0x%06x\n"
		     "antenna_sel=%d\n"
		     "calib_int=%d\n"
		     "tx_power_reduction=%d.%d dBm\n"
		     "bridge_packets=%d\n"
		     "key_tx_rx_threshold=%d\n"
		     "rts_threshold=%d\n"
		     "fragmentation_threshold=%d\n"
		     "short_retry_limit=%d\n"
		     "long_retry_limit=%d\n"
		     "total_ps_buffered=%d\n",
		     local->hw->name ? local->hw->name : "N/A",
		     local->conf.channel,
		     local->conf.freq,
		     ieee80211_mode_str(local->conf.phymode),
			 local->conf.radar_detect,
		     local->wep_iv & 0xffffff,
		     local->conf.antenna_sel,
		     local->conf.calib_int,
		     local->conf.tx_power_reduction / 10,
		     local->conf.tx_power_reduction % 10,
		     local->bridge_packets,
		     local->key_tx_rx_threshold,
		     local->rts_threshold,
		     local->fragmentation_threshold,
		     local->short_retry_limit,
		     local->long_retry_limit,
		     local->total_ps_buffered);

	return (p - page);
}


static int ieee80211_proc_channels_read(char *page, char **start, off_t off,
					int count, int *eof, void *data)
{
	char *p = page;
	struct ieee80211_local *local = (struct ieee80211_local *) data;
	int m, c;
	struct ieee80211_hw_modes *mode;
	struct ieee80211_channel *chan;

	if (off != 0) {
		*eof = 1;
		return 0;
	}

	p += sprintf(p, "MODE CHAN FREQ TXPOWER ANTMAX FLAGS\n");
	for (m = 0; m < local->hw->num_modes; m++) {
		mode = &local->hw->modes[m];
		for (c = 0; c < mode->num_channels; c++) {
			chan = &mode->channels[c];
			p += sprintf(p, "%d %d %d %d %d %s%s%s\n",
				     mode->mode, chan->chan, chan->freq,
				     chan->power_level, chan->antenna_max,
				     chan->flag & IEEE80211_CHAN_W_SCAN ?
				     "[W_SCAN]" : "",
				     chan->flag & IEEE80211_CHAN_W_ACTIVE_SCAN
				     ? "[W_ACTIVE_SCAN]" : "",
				     chan->flag & IEEE80211_CHAN_W_IBSS ?
				     "[W_IBSS]" : "");
		}
	}
	return (p - page);
}


static int ieee80211_proc_rates_read(char *page, char **start, off_t off,
				     int count, int *eof, void *data)
{
	char *p = page;
	struct ieee80211_local *local = (struct ieee80211_local *) data;
	int r;
	struct ieee80211_rate *rate;

	if (off != 0) {
		*eof = 1;
		return 0;
	}

	p += sprintf(p, "RATE VAL VAL2 MIN_RSSI_ACK MIN_RSSI_ACK_DELTA "
		     "FLAGS\n");
	for (r = 0; r < local->num_curr_rates; r++) {
		rate = &local->curr_rates[r];
		p += sprintf(p, "%d %d %d %d %d 0x%x %s%s%s%s%s%s%s%s\n",
			     rate->rate, rate->val, rate->val2,
			     rate->min_rssi_ack, rate->min_rssi_ack_delta,
			     rate->flags,
			     rate->flags & IEEE80211_RATE_ERP ? "[ERP]" : "",
			     rate->flags & IEEE80211_RATE_BASIC ?
			     "[BASIC]" : "",
			     rate->flags & IEEE80211_RATE_PREAMBLE2 ?
			     "[PREAMBLE2]" : "",
			     rate->flags & IEEE80211_RATE_SUPPORTED ?
			     "[SUPPORTED]" : "",
			     rate->flags & IEEE80211_RATE_OFDM ? "[OFDM]" : "",
			     rate->flags & IEEE80211_RATE_CCK ? "[CCK]" : "",
			     rate->flags & IEEE80211_RATE_TURBO ?
			     "[TURBO]" : "",
			     rate->flags & IEEE80211_RATE_MANDATORY ?
			     "[MANDATORY]" : "");
	}
	return (p - page);
}


static int ieee80211_proc_multicast_read(char *page, char **start, off_t off,
					 int count, int *eof, void *data)
{
	char *p = page;
	struct ieee80211_local *local = (struct ieee80211_local *) data;

	if (off != 0) {
		*eof = 1;
		return 0;
	}

	return rate_control_status_global(local, p);

}


void ieee80211_proc_init_sta(struct ieee80211_local *local,
			     struct sta_info *sta)
{
	char buf[30];
	struct proc_dir_entry *entry;

	sprintf(buf, MACSTR, MAC2STR(sta->addr));

	if (!local->proc_sta)
		return;

	entry = create_proc_read_entry(buf, 0, local->proc_sta,
				       ieee80211_proc_sta_read, sta);
	if (entry) {
		entry->mode &= ~(S_IRWXG | S_IRWXO);
		sta->proc_entry_added = 1;
	}
}


void ieee80211_proc_deinit_sta(struct ieee80211_local *local,
			       struct sta_info *sta)
{
	char buf[30];
	sprintf(buf, MACSTR, MAC2STR(sta->addr));
	if (local->proc_sta) {
		remove_proc_entry(buf, local->proc_sta);
		sta->proc_entry_added = 0;
	}
}


void ieee80211_proc_init_virtual(struct net_device *dev)
{
	struct proc_dir_entry *entry;
	struct ieee80211_local *local = (struct ieee80211_local *) dev->priv;

	if (!local->proc_iface)
		return;

	entry = create_proc_read_entry(dev->name, 0, local->proc_iface,
				       ieee80211_proc_iface_read, dev);
	if (entry)
		entry->mode &= ~(S_IRWXG | S_IRWXO);
}


void ieee80211_proc_deinit_virtual(struct net_device *dev)
{
	struct ieee80211_local *local = (struct ieee80211_local *) dev->priv;

	if (local->proc_iface)
		remove_proc_entry(dev->name, local->proc_iface);
}


void ieee80211_proc_init_interface(struct ieee80211_local *local)
{
	if (!ieee80211_proc)
		return;

	local->proc = proc_mkdir(local->wdev->name, ieee80211_proc);
	if (!local->proc)
		return;

	local->proc_sta = proc_mkdir("sta", local->proc);
	local->proc_iface = proc_mkdir("iface", local->proc);
	create_proc_read_entry("counters", 0, local->proc,
                               ieee80211_proc_counters_read, local);
	create_proc_read_entry("config", 0, local->proc,
                               ieee80211_proc_config_read, local);
	create_proc_read_entry("channels", 0, local->proc,
                               ieee80211_proc_channels_read, local);
	create_proc_read_entry("rates", 0, local->proc,
			       ieee80211_proc_rates_read, local);
	create_proc_read_entry("multicast", 0, local->proc,
			       ieee80211_proc_multicast_read, local);
	create_proc_read_entry("debug", 0, local->proc,
                               ieee80211_proc_debug_read, local);
	create_proc_read_entry("info", 0, local->proc,
			       ieee80211_proc_info_read, local);
	ieee80211_proc_init_virtual(local->wdev);
}


void ieee80211_proc_deinit_interface(struct ieee80211_local *local)
{
	if (!local->proc)
		return;

	ieee80211_proc_deinit_virtual(local->wdev);
	remove_proc_entry("iface", local->proc);
	remove_proc_entry("sta", local->proc);
        remove_proc_entry("counters", local->proc);
        remove_proc_entry("debug", local->proc);
        remove_proc_entry("config", local->proc);
        remove_proc_entry("channels", local->proc);
	remove_proc_entry("rates", local->proc);
	remove_proc_entry("multicast", local->proc);
	remove_proc_entry("info", local->proc);
	local->proc = NULL;
	remove_proc_entry(local->wdev->name, ieee80211_proc);
}


void ieee80211_proc_init(void)
{
	if (proc_net == NULL) {
		ieee80211_proc = NULL;
		return;
	}

	ieee80211_proc = proc_mkdir("ieee80211", proc_net);
	if (!ieee80211_proc)
		printk(KERN_WARNING "Failed to mkdir /proc/net/ieee80211\n");
}


void ieee80211_proc_deinit(void)
{
	if (!ieee80211_proc)
		return;

	ieee80211_proc = NULL;
	remove_proc_entry("ieee80211", proc_net);
}

#endif /* CONFIG_PROC_FS */
