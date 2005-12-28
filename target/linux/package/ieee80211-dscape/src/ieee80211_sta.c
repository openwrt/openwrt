/*
 * BSS client mode implementation
 * Copyright 2003, Jouni Malinen <jkmaline@cc.hut.fi>
 * Copyright 2004, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* TODO:
 * BSS table: use <BSSID,SSID> as the key to support multi-SSID APs
 * order BSS list by RSSI(?) ("quality of AP")
 * scan result table filtering (by capability (privacy, IBSS/BSS, WPA/RSN IE,
 *    SSID)
 */
#include <linux/config.h>
#include <linux/version.h>
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/wireless.h>
#include <linux/random.h>
#include <net/iw_handler.h>
#include <asm/types.h>
#include <asm/delay.h>

#include <net/ieee80211.h>
#include <net/ieee80211_mgmt.h>
#include "ieee80211_i.h"
#include "rate_control.h"
#include "hostapd_ioctl.h"

/* #define IEEE80211_IBSS_DEBUG */

#define IEEE80211_AUTH_TIMEOUT (HZ / 5)
#define IEEE80211_AUTH_MAX_TRIES 3
#define IEEE80211_ASSOC_TIMEOUT (HZ / 5)
#define IEEE80211_ASSOC_MAX_TRIES 3
#define IEEE80211_MONITORING_INTERVAL (2 * HZ)
#define IEEE80211_PROBE_INTERVAL (60 * HZ)
#define IEEE80211_RETRY_AUTH_INTERVAL (1 * HZ)
#define IEEE80211_SCAN_INTERVAL (2 * HZ)
#define IEEE80211_SCAN_INTERVAL_SLOW (15 * HZ)
#define IEEE80211_IBSS_JOIN_TIMEOUT (20 * HZ)

#define IEEE80211_PROBE_DELAY (HZ / 33)
#define IEEE80211_CHANNEL_TIME (HZ / 33)
#define IEEE80211_PASSIVE_CHANNEL_TIME (HZ / 5)
#define IEEE80211_SCAN_RESULT_EXPIRE (10 * HZ)
#define IEEE80211_IBSS_MERGE_INTERVAL (30 * HZ)
#define IEEE80211_IBSS_INACTIVITY_LIMIT (60 * HZ)

#define IEEE80211_IBSS_MAX_STA_ENTRIES 128


#define IEEE80211_FC(type, stype) cpu_to_le16((type << 2) | (stype << 4))

#define ERP_INFO_USE_PROTECTION BIT(1)

static void ieee80211_send_probe_req(struct net_device *dev, u8 *dst,
				     u8 *ssid, size_t ssid_len);
static struct ieee80211_sta_bss *
ieee80211_rx_bss_get(struct net_device *dev, u8 *bssid);
static void ieee80211_rx_bss_put(struct net_device *dev,
				 struct ieee80211_sta_bss *bss);
static int ieee80211_sta_find_ibss(struct net_device *dev,
				   struct ieee80211_if_sta *ifsta);
static int ieee80211_sta_wep_configured(struct net_device *dev);


/* Parsed Information Elements */
struct ieee802_11_elems {
	u8 *ssid;
	u8 ssid_len;
	u8 *supp_rates;
	u8 supp_rates_len;
	u8 *fh_params;
	u8 fh_params_len;
	u8 *ds_params;
	u8 ds_params_len;
	u8 *cf_params;
	u8 cf_params_len;
	u8 *tim;
	u8 tim_len;
	u8 *ibss_params;
	u8 ibss_params_len;
	u8 *challenge;
	u8 challenge_len;
	u8 *wpa;
	u8 wpa_len;
	u8 *rsn;
	u8 rsn_len;
	u8 *erp_info;
	u8 erp_info_len;
	u8 *ext_supp_rates;
	u8 ext_supp_rates_len;
	u8 *wmm_info;
	u8 wmm_info_len;
	u8 *wmm_param;
	u8 wmm_param_len;
};

typedef enum { ParseOK = 0, ParseUnknown = 1, ParseFailed = -1 } ParseRes;


static ParseRes ieee802_11_parse_elems(u8 *start, size_t len,
				       struct ieee802_11_elems *elems)
{
	size_t left = len;
	u8 *pos = start;
	int unknown = 0;

	memset(elems, 0, sizeof(*elems));

	while (left >= 2) {
		u8 id, elen;

		id = *pos++;
		elen = *pos++;
		left -= 2;

		if (elen > left) {
#if 0
			if (net_ratelimit())
				printk(KERN_DEBUG "IEEE 802.11 element parse "
				       "failed (id=%d elen=%d left=%d)\n",
				       id, elen, left);
#endif
			return ParseFailed;
		}

		switch (id) {
		case WLAN_EID_SSID:
			elems->ssid = pos;
			elems->ssid_len = elen;
			break;
		case WLAN_EID_SUPP_RATES:
			elems->supp_rates = pos;
			elems->supp_rates_len = elen;
			break;
		case WLAN_EID_FH_PARAMS:
			elems->fh_params = pos;
			elems->fh_params_len = elen;
			break;
		case WLAN_EID_DS_PARAMS:
			elems->ds_params = pos;
			elems->ds_params_len = elen;
			break;
		case WLAN_EID_CF_PARAMS:
			elems->cf_params = pos;
			elems->cf_params_len = elen;
			break;
		case WLAN_EID_TIM:
			elems->tim = pos;
			elems->tim_len = elen;
			break;
		case WLAN_EID_IBSS_PARAMS:
			elems->ibss_params = pos;
			elems->ibss_params_len = elen;
			break;
		case WLAN_EID_CHALLENGE:
			elems->challenge = pos;
			elems->challenge_len = elen;
			break;
		case WLAN_EID_WPA:
			if (elen >= 4 && pos[0] == 0x00 && pos[1] == 0x50 &&
			    pos[2] == 0xf2) {
				/* Microsoft OUI (00:50:F2) */
				if (pos[3] == 1) {
					/* OUI Type 1 - WPA IE */
					elems->wpa = pos;
					elems->wpa_len = elen;
				} else if (elen >= 5 && pos[3] == 2) {
					if (pos[4] == 0) {
						elems->wmm_info = pos;
						elems->wmm_info_len = elen;
					} else if (pos[4] == 1) {
						elems->wmm_param = pos;
						elems->wmm_param_len = elen;
					}
				}
			}
			break;
		case WLAN_EID_RSN:
			elems->rsn = pos;
			elems->rsn_len = elen;
			break;
		case WLAN_EID_ERP_INFO:
			elems->erp_info = pos;
			elems->erp_info_len = elen;
			break;
		case WLAN_EID_EXT_SUPP_RATES:
			elems->ext_supp_rates = pos;
			elems->ext_supp_rates_len = elen;
			break;
		default:
#if 0
			printk(KERN_DEBUG "IEEE 802.11 element parse ignored "
				      "unknown element (id=%d elen=%d)\n",
				      id, elen);
#endif
			unknown++;
			break;
		}

		left -= elen;
		pos += elen;
	}

	if (left)
		return ParseFailed;

	return unknown ? ParseUnknown : ParseOK;
}




static int ecw2cw(int ecw)
{
	int cw = 1;
	while (ecw > 0) {
		cw <<= 1;
		ecw--;
	}
	return cw - 1;
}


static void ieee80211_sta_wmm_params(struct net_device *dev,
				     struct ieee80211_if_sta *ifsta,
				     u8 *wmm_param, size_t wmm_param_len)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_tx_queue_params params;
	size_t left;
	int count;
	u8 *pos;

	if (wmm_param_len < 8 || wmm_param[5] /* version */ != 1)
		return;
	count = wmm_param[6] & 0x0f;
	if (count == ifsta->wmm_last_param_set)
		return;
	ifsta->wmm_last_param_set = count;

	pos = wmm_param + 8;
	left = wmm_param_len - 8;

	memset(&params, 0, sizeof(params));

	if (local->hw->conf_tx == NULL)
		return;

	local->wmm_acm = 0;
	for (; left >= 4; left -= 4, pos += 4) {
		int aci = (pos[0] >> 5) & 0x03;
		int acm = (pos[0] >> 4) & 0x01;
		int queue;

		switch (aci) {
		case 1:
			queue = IEEE80211_TX_QUEUE_DATA3;
			if (acm) {
				local->wmm_acm |= BIT(1) | BIT(2);
			}
			break;
		case 2:
			queue = IEEE80211_TX_QUEUE_DATA1;
			if (acm) {
				local->wmm_acm |= BIT(4) | BIT(5);
			}
			break;
		case 3:
			queue = IEEE80211_TX_QUEUE_DATA0;
			if (acm) {
				local->wmm_acm |= BIT(6) | BIT(7);
			}
			break;
		case 0:
		default:
			queue = IEEE80211_TX_QUEUE_DATA2;
			if (acm) {
				local->wmm_acm |= BIT(0) | BIT(3);
			}
			break;
		}

		params.aifs = pos[0] & 0x0f;
		params.cw_max = ecw2cw((pos[1] & 0xf0) >> 4);
		params.cw_min = ecw2cw(pos[1] & 0x0f);
		/* TXOP is in units of 32 usec; burst_time in 0.1 ms */
		params.burst_time = (pos[2] | (pos[3] << 8)) * 32 / 100;
		printk(KERN_DEBUG "%s: WMM queue=%d aci=%d acm=%d aifs=%d "
		       "cWmin=%d cWmax=%d burst=%d\n",
		       dev->name, queue, aci, acm, params.aifs, params.cw_min,
		       params.cw_max, params.burst_time);
		/* TODO: handle ACM (block TX, fallback to next lowest allowed
		 * AC for now) */
		if (local->hw->conf_tx(local->mdev, queue, &params)) {
			printk(KERN_DEBUG "%s: failed to set TX queue "
			       "parameters for queue %d\n", dev->name, queue);
		}
	}
}


static void ieee80211_sta_send_associnfo(struct net_device *dev,
					 struct ieee80211_if_sta *ifsta)
{
	char *buf;
	size_t len;
	int i;
	union iwreq_data wrqu;

	if (ifsta->assocreq_ies == NULL && ifsta->assocresp_ies == NULL)
		return;

	buf = kmalloc(50 + 2 * (ifsta->assocreq_ies_len +
				ifsta->assocresp_ies_len), GFP_ATOMIC);
	if (buf == NULL)
		return;

	len = sprintf(buf, "ASSOCINFO(");
	if (ifsta->assocreq_ies) {
		len += sprintf(buf + len, "ReqIEs=");
		for (i = 0; i < ifsta->assocreq_ies_len; i++) {
			len += sprintf(buf + len, "%02x",
				       ifsta->assocreq_ies[i]);
		}
	}
	if (ifsta->assocresp_ies) {
		if (ifsta->assocreq_ies)
			len += sprintf(buf + len, " ");
		len += sprintf(buf + len, "RespIEs=");
		for (i = 0; i < ifsta->assocresp_ies_len; i++) {
			len += sprintf(buf + len, "%02x",
				       ifsta->assocresp_ies[i]);
		}
	}
	len += sprintf(buf + len, ")");

	if (len > IW_CUSTOM_MAX) {
		len = sprintf(buf, "ASSOCRESPIE=");
		for (i = 0; i < ifsta->assocresp_ies_len; i++) {
			len += sprintf(buf + len, "%02x",
				       ifsta->assocresp_ies[i]);
		}
	}

	memset(&wrqu, 0, sizeof(wrqu));
	wrqu.data.length = len;
	wireless_send_event(dev, IWEVCUSTOM, &wrqu, buf);

	kfree(buf);
}


static void ieee80211_set_associated(struct net_device *dev,
				     struct ieee80211_if_sta *ifsta, int assoc)
{
	union iwreq_data wrqu;

	if (ifsta->associated == assoc)
		return;

	ifsta->associated = assoc;

	if (assoc) {
		struct ieee80211_sub_if_data *sdata;
		sdata = IEEE80211_DEV_TO_SUB_IF(dev);
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			return;
		ifsta->prev_bssid_set = 1;
		memcpy(ifsta->prev_bssid, sdata->u.sta.bssid, ETH_ALEN);
		memcpy(wrqu.ap_addr.sa_data, sdata->u.sta.bssid, ETH_ALEN);
		ieee80211_sta_send_associnfo(dev, ifsta);
	} else {
		memset(wrqu.ap_addr.sa_data, 0, ETH_ALEN);
	}
	wrqu.ap_addr.sa_family = ARPHRD_ETHER;
	wireless_send_event(dev, SIOCGIWAP, &wrqu, NULL);
	ifsta->last_probe = jiffies;
}


static void ieee80211_sta_tx(struct net_device *dev, struct sk_buff *skb,
			     int encrypt, int probe_resp)
{
        struct ieee80211_sub_if_data *sdata;
	struct ieee80211_tx_packet_data *pkt_data;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	skb->dev = sdata->master;
	skb->mac.raw = skb->nh.raw = skb->h.raw = skb->data;

	pkt_data = (struct ieee80211_tx_packet_data *) skb->cb;
	memset(pkt_data, 0, sizeof(struct ieee80211_tx_packet_data));
	pkt_data->magic = IEEE80211_CB_MAGIC;
	pkt_data->sdata = sdata;
	pkt_data->control.do_not_encrypt = !encrypt;
	if (probe_resp)
		pkt_data->control.pkt_type = PKT_PROBE_RESP;

	dev_queue_xmit(skb);
}


static void ieee80211_send_auth(struct net_device *dev,
				struct ieee80211_if_sta *ifsta,
				int transaction, u8 *extra, size_t extra_len,
				int encrypt)
{
	struct sk_buff *skb;
	struct ieee80211_mgmt *mgmt;

	skb = dev_alloc_skb(sizeof(*mgmt) + 6 + extra_len);
	if (skb == NULL) {
		printk(KERN_DEBUG "%s: failed to allocate buffer for auth "
		       "frame\n", dev->name);
		return;
	}

	mgmt = (struct ieee80211_mgmt *) skb_put(skb, 24 + 6);
	memset(mgmt, 0, 24 + 6);
	mgmt->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
					   WLAN_FC_STYPE_AUTH);
	if (encrypt)
		mgmt->frame_control |= cpu_to_le16(WLAN_FC_ISWEP);
	memcpy(mgmt->da, ifsta->bssid, ETH_ALEN);
	memcpy(mgmt->sa, dev->dev_addr, ETH_ALEN);
	memcpy(mgmt->bssid, ifsta->bssid, ETH_ALEN);
	mgmt->u.auth.auth_alg = cpu_to_le16(ifsta->auth_alg);
	mgmt->u.auth.auth_transaction = cpu_to_le16(transaction);
	ifsta->auth_transaction = transaction + 1;
	mgmt->u.auth.status_code = cpu_to_le16(0);
	if (extra)
		memcpy(skb_put(skb, extra_len), extra, extra_len);

	ieee80211_sta_tx(dev, skb, encrypt, 0);
}


static void ieee80211_authenticate(struct net_device *dev,
				   struct ieee80211_if_sta *ifsta)
{
	ifsta->auth_tries++;
	if (ifsta->auth_tries > IEEE80211_AUTH_MAX_TRIES) {
		printk(KERN_DEBUG "%s: authentication with AP " MACSTR
		       " timed out\n",
		       dev->name, MAC2STR(ifsta->bssid));
		return;
	}

	ifsta->state = IEEE80211_AUTHENTICATE;
	printk(KERN_DEBUG "%s: authenticate with AP " MACSTR "\n",
	       dev->name, MAC2STR(ifsta->bssid));

	ieee80211_send_auth(dev, ifsta, 1, NULL, 0, 0);

	mod_timer(&ifsta->timer, jiffies + IEEE80211_AUTH_TIMEOUT);
}


static void ieee80211_send_assoc(struct net_device *dev,
				 struct ieee80211_if_sta *ifsta)
{
	struct ieee80211_local *local = dev->priv;
	struct sk_buff *skb;
	struct ieee80211_mgmt *mgmt;
	u8 *pos, *ies;
	int i, len;
	u16 capab;
	struct ieee80211_sta_bss *bss;
	int wmm = 0;

	skb = dev_alloc_skb(sizeof(*mgmt) + 200 + ifsta->extra_ie_len +
			    ifsta->ssid_len);
	if (skb == NULL) {
		printk(KERN_DEBUG "%s: failed to allocate buffer for assoc "
		       "frame\n", dev->name);
		return;
	}

	capab = ifsta->capab;
	if (local->conf.phymode == MODE_IEEE80211G) {
		capab |= WLAN_CAPABILITY_SHORT_SLOT_TIME |
			WLAN_CAPABILITY_SHORT_PREAMBLE;
	}
	bss = ieee80211_rx_bss_get(dev, ifsta->bssid);
	if (bss) {
		if (bss->capability & WLAN_CAPABILITY_PRIVACY)
			capab |= WLAN_CAPABILITY_PRIVACY;
		if (bss->wmm_ie) {
			wmm = 1;
		}
		ieee80211_rx_bss_put(dev, bss);
	}

	mgmt = (struct ieee80211_mgmt *) skb_put(skb, 24);
	memset(mgmt, 0, 24);
	memcpy(mgmt->da, ifsta->bssid, ETH_ALEN);
	memcpy(mgmt->sa, dev->dev_addr, ETH_ALEN);
	memcpy(mgmt->bssid, ifsta->bssid, ETH_ALEN);

	if (ifsta->prev_bssid_set) {
		skb_put(skb, 10);
		mgmt->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
						   WLAN_FC_STYPE_REASSOC_REQ);
		mgmt->u.reassoc_req.capab_info = cpu_to_le16(capab);
		mgmt->u.reassoc_req.listen_interval = cpu_to_le16(1);
		memcpy(mgmt->u.reassoc_req.current_ap, ifsta->prev_bssid,
		       ETH_ALEN);
	} else {
		skb_put(skb, 4);
		mgmt->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
						   WLAN_FC_STYPE_ASSOC_REQ);
		mgmt->u.assoc_req.capab_info = cpu_to_le16(capab);
		mgmt->u.assoc_req.listen_interval = cpu_to_le16(1);
	}

	/* SSID */
	ies = pos = skb_put(skb, 2 + ifsta->ssid_len);
	*pos++ = WLAN_EID_SSID;
	*pos++ = ifsta->ssid_len;
	memcpy(pos, ifsta->ssid, ifsta->ssid_len);

	len = local->num_curr_rates;
	if (len > 8)
		len = 8;
	pos = skb_put(skb, len + 2);
	*pos++ = WLAN_EID_SUPP_RATES;
	*pos++ = len;
	for (i = 0; i < len; i++) {
		int rate = local->curr_rates[i].rate;
		if (local->conf.phymode == MODE_ATHEROS_TURBO)
			rate /= 2;
		*pos++ = (u8) (rate / 5);
	}

	if (local->num_curr_rates > len) {
		pos = skb_put(skb, local->num_curr_rates - len + 2);
		*pos++ = WLAN_EID_EXT_SUPP_RATES;
		*pos++ = local->num_curr_rates - len;
		for (i = len; i < local->num_curr_rates; i++) {
			int rate = local->curr_rates[i].rate;
			if (local->conf.phymode == MODE_ATHEROS_TURBO)
				rate /= 2;
			*pos++ = (u8) (rate / 5);
		}
	}

	if (ifsta->extra_ie) {
		pos = skb_put(skb, ifsta->extra_ie_len);
		memcpy(pos, ifsta->extra_ie, ifsta->extra_ie_len);
	}

	if (wmm && ifsta->wmm_enabled) {
		pos = skb_put(skb, 9);
		*pos++ = WLAN_EID_VENDOR_SPECIFIC;
		*pos++ = 7; /* len */
		*pos++ = 0x00; /* Microsoft OUI 00:50:F2 */
		*pos++ = 0x50;
		*pos++ = 0xf2;
		*pos++ = 2; /* WME */
		*pos++ = 0; /* WME info */
		*pos++ = 1; /* WME ver */
		*pos++ = 0;
	}

	kfree(ifsta->assocreq_ies);
	ifsta->assocreq_ies_len = (skb->data + skb->len) - ies;
	ifsta->assocreq_ies = kmalloc(ifsta->assocreq_ies_len, GFP_ATOMIC);
	if (ifsta->assocreq_ies)
		memcpy(ifsta->assocreq_ies, ies, ifsta->assocreq_ies_len);

	ieee80211_sta_tx(dev, skb, 0, 0);
}


static void ieee80211_send_deauth(struct net_device *dev,
				  struct ieee80211_if_sta *ifsta, u16 reason)
{
	struct sk_buff *skb;
	struct ieee80211_mgmt *mgmt;

	skb = dev_alloc_skb(sizeof(*mgmt));
	if (skb == NULL) {
		printk(KERN_DEBUG "%s: failed to allocate buffer for deauth "
		       "frame\n", dev->name);
		return;
	}

	mgmt = (struct ieee80211_mgmt *) skb_put(skb, 24);
	memset(mgmt, 0, 24);
	memcpy(mgmt->da, ifsta->bssid, ETH_ALEN);
	memcpy(mgmt->sa, dev->dev_addr, ETH_ALEN);
	memcpy(mgmt->bssid, ifsta->bssid, ETH_ALEN);
	mgmt->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
					   WLAN_FC_STYPE_DEAUTH);
	skb_put(skb, 2);
	mgmt->u.deauth.reason_code = cpu_to_le16(reason);

	ieee80211_sta_tx(dev, skb, 0, 0);
}


static void ieee80211_send_disassoc(struct net_device *dev,
				    struct ieee80211_if_sta *ifsta, u16 reason)
{
	struct sk_buff *skb;
	struct ieee80211_mgmt *mgmt;

	skb = dev_alloc_skb(sizeof(*mgmt));
	if (skb == NULL) {
		printk(KERN_DEBUG "%s: failed to allocate buffer for disassoc "
		       "frame\n", dev->name);
		return;
	}

	mgmt = (struct ieee80211_mgmt *) skb_put(skb, 24);
	memset(mgmt, 0, 24);
	memcpy(mgmt->da, ifsta->bssid, ETH_ALEN);
	memcpy(mgmt->sa, dev->dev_addr, ETH_ALEN);
	memcpy(mgmt->bssid, ifsta->bssid, ETH_ALEN);
	mgmt->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
					   WLAN_FC_STYPE_DISASSOC);
	skb_put(skb, 2);
	mgmt->u.disassoc.reason_code = cpu_to_le16(reason);

	ieee80211_sta_tx(dev, skb, 0, 0);
}


static int ieee80211_privacy_mismatch(struct net_device *dev,
				      struct ieee80211_if_sta *ifsta)
{
	struct ieee80211_sta_bss *bss;
	int res = 0;

	if (ifsta == NULL || ifsta->mixed_cell ||
	    ifsta->key_mgmt != IEEE80211_KEY_MGMT_NONE)
		return 0;

	bss = ieee80211_rx_bss_get(dev, ifsta->bssid);
	if (bss == NULL)
		return 0;

	if (ieee80211_sta_wep_configured(dev) !=
	    !!(bss->capability & WLAN_CAPABILITY_PRIVACY))
		res = 1;

	ieee80211_rx_bss_put(dev, bss);

	return res;
}


static void ieee80211_associate(struct net_device *dev,
				struct ieee80211_if_sta *ifsta)
{
	ifsta->assoc_tries++;
	if (ifsta->assoc_tries > IEEE80211_ASSOC_MAX_TRIES) {
		printk(KERN_DEBUG "%s: association with AP " MACSTR
		       " timed out\n",
		       dev->name, MAC2STR(ifsta->bssid));
		return;
	}

	ifsta->state = IEEE80211_ASSOCIATE;
	printk(KERN_DEBUG "%s: associate with AP " MACSTR "\n",
	       dev->name, MAC2STR(ifsta->bssid));
	if (ieee80211_privacy_mismatch(dev, ifsta)) {
		printk(KERN_DEBUG "%s: mismatch in privacy configuration and "
		       "mixed-cell disabled - abort association\n", dev->name);
		return;
	}

	ieee80211_send_assoc(dev, ifsta);

	mod_timer(&ifsta->timer, jiffies + IEEE80211_ASSOC_TIMEOUT);
}


static void ieee80211_associated(struct net_device *dev,
				 struct ieee80211_if_sta *ifsta)
{
	struct ieee80211_local *local = dev->priv;
	struct sta_info *sta;
	int disassoc;

	/* TODO: start monitoring current AP signal quality and number of
	 * missed beacons. Scan other channels every now and then and search
	 * for better APs. */
	/* TODO: remove expired BSSes */

	ifsta->state = IEEE80211_ASSOCIATED;

	sta = sta_info_get(local, ifsta->bssid);
	if (sta == NULL) {
		printk(KERN_DEBUG "%s: No STA entry for own AP " MACSTR "\n",
		       dev->name, MAC2STR(ifsta->bssid));
		disassoc = 1;
	} else {
		disassoc = 0;
		if (time_after(jiffies,
			       sta->last_rx + IEEE80211_MONITORING_INTERVAL)) {
			if (ifsta->probereq_poll) {
				printk(KERN_DEBUG "%s: No ProbeResp from "
				       "current AP " MACSTR " - assume out of "
				       "range\n",
				       dev->name, MAC2STR(ifsta->bssid));
				disassoc = 1;
			} else {
				ieee80211_send_probe_req(dev, ifsta->bssid,
							 local->scan_ssid,
							 local->scan_ssid_len);
				ifsta->probereq_poll = 1;
			}
		} else {
			ifsta->probereq_poll = 0;
			if (time_after(jiffies, ifsta->last_probe +
				       IEEE80211_PROBE_INTERVAL)) {
				ifsta->last_probe = jiffies;
				ieee80211_send_probe_req(dev, ifsta->bssid,
							 ifsta->ssid,
							 ifsta->ssid_len);
			}
		}
		sta_info_release(local, sta);
	}
	if (disassoc) {	
		union iwreq_data wrqu;
		memset(wrqu.ap_addr.sa_data, 0, ETH_ALEN);
		wrqu.ap_addr.sa_family = ARPHRD_ETHER;
		wireless_send_event(dev, SIOCGIWAP, &wrqu, NULL);
		mod_timer(&ifsta->timer,
			  jiffies + IEEE80211_MONITORING_INTERVAL + 30 * HZ);
	} else {
		mod_timer(&ifsta->timer,
			  jiffies + IEEE80211_MONITORING_INTERVAL);
	}
}


static void ieee80211_send_probe_req(struct net_device *dev, u8 *dst,
				     u8 *ssid, size_t ssid_len)
{
	struct ieee80211_local *local = dev->priv;
	struct sk_buff *skb;
	struct ieee80211_mgmt *mgmt;
	u8 *pos, *supp_rates, *esupp_rates = NULL;
	int i;

	skb = dev_alloc_skb(sizeof(*mgmt) + 200);
	if (skb == NULL) {
		printk(KERN_DEBUG "%s: failed to allocate buffer for probe "
		       "request\n", dev->name);
		return;
	}

	mgmt = (struct ieee80211_mgmt *) skb_put(skb, 24);
	memset(mgmt, 0, 24);
	mgmt->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
					   WLAN_FC_STYPE_PROBE_REQ);
	memcpy(mgmt->sa, dev->dev_addr, ETH_ALEN);
	if (dst) {
		memcpy(mgmt->da, dst, ETH_ALEN);
		memcpy(mgmt->bssid, dst, ETH_ALEN);
	} else {
		memset(mgmt->da, 0xff, ETH_ALEN);
		memset(mgmt->bssid, 0xff, ETH_ALEN);
	}
	pos = skb_put(skb, 2 + ssid_len);
	*pos++ = WLAN_EID_SSID;
	*pos++ = ssid_len;
	memcpy(pos, ssid, ssid_len);

	supp_rates = skb_put(skb, 2);
	supp_rates[0] = WLAN_EID_SUPP_RATES;
	supp_rates[1] = 0;
	for (i = 0; i < local->num_curr_rates; i++) {
		struct ieee80211_rate *rate = &local->curr_rates[i];
		if (!(rate->flags & IEEE80211_RATE_SUPPORTED))
			continue;
		if (esupp_rates) {
			pos = skb_put(skb, 1);
			esupp_rates[1]++;
		} else if (supp_rates[1] == 8) {
			esupp_rates = skb_put(skb, 3);
			esupp_rates[0] = WLAN_EID_EXT_SUPP_RATES;
			esupp_rates[1] = 1;
			pos = &esupp_rates[2];
		} else {
			pos = skb_put(skb, 1);
			supp_rates[1]++;
		}
		if (local->conf.phymode == MODE_ATHEROS_TURBO)
			*pos = rate->rate / 10;
		else
			*pos = rate->rate / 5;
	}

	ieee80211_sta_tx(dev, skb, 0, 0);
}


static int ieee80211_sta_wep_configured(struct net_device *dev)
{
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata == NULL || sdata->default_key == NULL ||
	    sdata->default_key->alg != ALG_WEP)
		return 0;
	return 1;
}


static void ieee80211_auth_completed(struct net_device *dev,
				     struct ieee80211_if_sta *ifsta)
{
	printk(KERN_DEBUG "%s: authenticated\n", dev->name);
	ifsta->authenticated = 1;
	ieee80211_associate(dev, ifsta);
}


static void ieee80211_auth_challenge(struct net_device *dev,
				     struct ieee80211_if_sta *ifsta,
				     struct ieee80211_mgmt *mgmt,
				     size_t len,
				     struct ieee80211_rx_status *rx_status)
{
	u8 *pos;
	struct ieee802_11_elems elems;

	printk(KERN_DEBUG "%s: replying to auth challenge\n", dev->name);
	pos = mgmt->u.auth.variable;
	if (ieee802_11_parse_elems(pos, len - (pos - (u8 *) mgmt), &elems)
	    == ParseFailed) {
		printk(KERN_DEBUG "%s: failed to parse Auth(challenge)\n",
		       dev->name);
		return;
	}
	if (elems.challenge == NULL) {
		printk(KERN_DEBUG "%s: no challenge IE in shared key auth "
		       "frame\n", dev->name);
		return;
	}
	ieee80211_send_auth(dev, ifsta, 3, elems.challenge - 2,
			    elems.challenge_len + 2, 1);
}


static void ieee80211_rx_mgmt_auth(struct net_device *dev,
				   struct ieee80211_if_sta *ifsta,
				   struct ieee80211_mgmt *mgmt,
				   size_t len,
				   struct ieee80211_rx_status *rx_status)
{
	struct ieee80211_local *local = dev->priv;
	u16 auth_alg, auth_transaction, status_code;

	if (ifsta->state != IEEE80211_AUTHENTICATE &&
	    local->conf.mode != IW_MODE_ADHOC) {
		printk(KERN_DEBUG "%s: authentication frame received from "
		       MACSTR ", but not in authenticate state - ignored\n",
		       dev->name, MAC2STR(mgmt->sa));
		return;
	}

	if (len < 24 + 6) {
		printk(KERN_DEBUG "%s: too short (%zd) authentication frame "
		       "received from " MACSTR " - ignored\n",
		       dev->name, len, MAC2STR(mgmt->sa));
		return;
	}

	if (local->conf.mode != IW_MODE_ADHOC &&
	    memcmp(ifsta->bssid, mgmt->sa, ETH_ALEN) != 0) {
		printk(KERN_DEBUG "%s: authentication frame received from "
		       "unknown AP (SA=" MACSTR " BSSID=" MACSTR ") - "
		       "ignored\n", dev->name, MAC2STR(mgmt->sa),
		       MAC2STR(mgmt->bssid));
		return;
	}

	if (local->conf.mode == IW_MODE_ADHOC &&
	    memcmp(ifsta->bssid, mgmt->bssid, ETH_ALEN) != 0) {
		printk(KERN_DEBUG "%s: authentication frame received from "
		       "unknown BSSID (SA=" MACSTR " BSSID=" MACSTR ") - "
		       "ignored\n", dev->name, MAC2STR(mgmt->sa),
		       MAC2STR(mgmt->bssid));
		return;
	}

	auth_alg = le16_to_cpu(mgmt->u.auth.auth_alg);
	auth_transaction = le16_to_cpu(mgmt->u.auth.auth_transaction);
	status_code = le16_to_cpu(mgmt->u.auth.status_code);

	printk(KERN_DEBUG "%s: RX authentication from " MACSTR " (alg=%d "
	       "transaction=%d status=%d)\n",
	       dev->name, MAC2STR(mgmt->sa), auth_alg,
	       auth_transaction, status_code);

	if (local->conf.mode == IW_MODE_ADHOC) {
		/* IEEE 802.11 standard does not require authentication in IBSS
		 * networks and most implementations do not seem to use it.
		 * However, try to reply to authentication attempts if someone
		 * has actually implemented this.
		 * TODO: Could implement shared key authentication. */
		if (auth_alg != WLAN_AUTH_OPEN || auth_transaction != 1) {
			printk(KERN_DEBUG "%s: unexpected IBSS authentication "
			       "frame (alg=%d transaction=%d)\n",
			       dev->name, auth_alg, auth_transaction);
			return;
		}
		ieee80211_send_auth(dev, ifsta, 2, NULL, 0, 0);
	}

	if (auth_alg != ifsta->auth_alg ||
	    auth_transaction != ifsta->auth_transaction) {
		printk(KERN_DEBUG "%s: unexpected authentication frame "
		       "(alg=%d transaction=%d)\n",
		       dev->name, auth_alg, auth_transaction);
		return;
	}

	if (status_code != WLAN_STATUS_SUCCESS) {
		printk(KERN_DEBUG "%s: AP denied authentication (auth_alg=%d "
		       "code=%d)\n", dev->name, ifsta->auth_alg, status_code);
		if (status_code == WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG) {
			const int num_algs = 3;
			u8 algs[num_algs];
			int i, pos;
			algs[0] = algs[1] = algs[2] = 0xff;
			if (ifsta->auth_algs & IEEE80211_AUTH_ALG_OPEN)
				algs[0] = WLAN_AUTH_OPEN;
			if (ifsta->auth_algs & IEEE80211_AUTH_ALG_SHARED_KEY)
				algs[1] = WLAN_AUTH_SHARED_KEY;
			if (ifsta->auth_algs & IEEE80211_AUTH_ALG_LEAP)
				algs[2] = WLAN_AUTH_LEAP;
			if (ifsta->auth_alg == WLAN_AUTH_OPEN)
				pos = 0;
			else if (ifsta->auth_alg == WLAN_AUTH_SHARED_KEY)
				pos = 1;
			else
				pos = 2;
			for (i = 0; i < num_algs; i++) {
				pos++;
				if (pos >= num_algs)
					pos = 0;
				if (algs[pos] == ifsta->auth_alg ||
				    algs[pos] == 0xff)
					continue;
				if (algs[pos] == WLAN_AUTH_SHARED_KEY &&
				    !ieee80211_sta_wep_configured(dev))
					continue;
				ifsta->auth_alg = algs[pos];
				printk(KERN_DEBUG "%s: set auth_alg=%d for "
				       "next try\n",
				       dev->name, ifsta->auth_alg);
				break;
			}
		}
		return;
	}

	switch (ifsta->auth_alg) {
	case WLAN_AUTH_OPEN:
	case WLAN_AUTH_LEAP:
		ieee80211_auth_completed(dev, ifsta);
		break;
	case WLAN_AUTH_SHARED_KEY:
		if (ifsta->auth_transaction == 4)
			ieee80211_auth_completed(dev, ifsta);
		else
			ieee80211_auth_challenge(dev, ifsta, mgmt, len,
						 rx_status);
		break;
	}
}


static void ieee80211_rx_mgmt_deauth(struct net_device *dev,
				     struct ieee80211_if_sta *ifsta,
				     struct ieee80211_mgmt *mgmt,
				     size_t len,
				     struct ieee80211_rx_status *rx_status)
{
	u16 reason_code;

	if (len < 24 + 2) {
		printk(KERN_DEBUG "%s: too short (%zd) deauthentication frame "
		       "received from " MACSTR " - ignored\n",
		       dev->name, len, MAC2STR(mgmt->sa));
		return;
	}

	if (memcmp(ifsta->bssid, mgmt->sa, ETH_ALEN) != 0) {
		printk(KERN_DEBUG "%s: deauthentication frame received from "
		       "unknown AP (SA=" MACSTR " BSSID=" MACSTR ") - "
		       "ignored\n", dev->name, MAC2STR(mgmt->sa),
		       MAC2STR(mgmt->bssid));
		return;
	}

	reason_code = le16_to_cpu(mgmt->u.deauth.reason_code);

	printk(KERN_DEBUG "%s: RX deauthentication from " MACSTR
	       " (reason=%d)\n",
	       dev->name, MAC2STR(mgmt->sa), reason_code);

	if (ifsta->authenticated) {
		printk(KERN_DEBUG "%s: deauthenticated\n", dev->name);
	}

	if (ifsta->state == IEEE80211_AUTHENTICATE ||
	    ifsta->state == IEEE80211_ASSOCIATE ||
	    ifsta->state == IEEE80211_ASSOCIATED) {
		ifsta->state = IEEE80211_AUTHENTICATE;
		mod_timer(&ifsta->timer,
			  jiffies + IEEE80211_RETRY_AUTH_INTERVAL);
	}

	ieee80211_set_associated(dev, ifsta, 0);
	ifsta->authenticated = 0;
}


static void ieee80211_rx_mgmt_disassoc(struct net_device *dev,
				       struct ieee80211_if_sta *ifsta,
				       struct ieee80211_mgmt *mgmt,
				       size_t len,
				       struct ieee80211_rx_status *rx_status)
{
	u16 reason_code;

	if (len < 24 + 2) {
		printk(KERN_DEBUG "%s: too short (%zd) disassociation frame "
		       "received from " MACSTR " - ignored\n",
		       dev->name, len, MAC2STR(mgmt->sa));
		return;
	}

	if (memcmp(ifsta->bssid, mgmt->sa, ETH_ALEN) != 0) {
		printk(KERN_DEBUG "%s: disassociation frame received from "
		       "unknown AP (SA=" MACSTR " BSSID=" MACSTR ") - "
		       "ignored\n", dev->name, MAC2STR(mgmt->sa),
		       MAC2STR(mgmt->bssid));
		return;
	}

	reason_code = le16_to_cpu(mgmt->u.disassoc.reason_code);

	printk(KERN_DEBUG "%s: RX disassociation from " MACSTR
	       " (reason=%d)\n",
	       dev->name, MAC2STR(mgmt->sa), reason_code);

	if (ifsta->associated)
		printk(KERN_DEBUG "%s: disassociated\n", dev->name);

	if (ifsta->state == IEEE80211_ASSOCIATED) {
		ifsta->state = IEEE80211_ASSOCIATE;
		mod_timer(&ifsta->timer,
			  jiffies + IEEE80211_RETRY_AUTH_INTERVAL);
	}

	ieee80211_set_associated(dev, ifsta, 0);
}


static void ieee80211_rx_mgmt_assoc_resp(struct net_device *dev,
					 struct ieee80211_if_sta *ifsta,
					 struct ieee80211_mgmt *mgmt,
					 size_t len,
					 struct ieee80211_rx_status *rx_status,
					 int reassoc)
{
	struct ieee80211_local *local = dev->priv;
	struct sta_info *sta;
	u32 rates;
	u16 capab_info, status_code, aid;
	struct ieee802_11_elems elems;
	u8 *pos;
	int i, j;

	/* AssocResp and ReassocResp have identical structure, so process both
	 * of them in this function. */

	if (ifsta->state != IEEE80211_ASSOCIATE) {
		printk(KERN_DEBUG "%s: association frame received from "
		       MACSTR ", but not in associate state - ignored\n",
		       dev->name, MAC2STR(mgmt->sa));
		return;
	}

	if (len < 24 + 6) {
		printk(KERN_DEBUG "%s: too short (%zd) association frame "
		       "received from " MACSTR " - ignored\n",
		       dev->name, len, MAC2STR(mgmt->sa));
		return;
	}

	if (memcmp(ifsta->bssid, mgmt->sa, ETH_ALEN) != 0) {
		printk(KERN_DEBUG "%s: association frame received from "
		       "unknown AP (SA=" MACSTR " BSSID=" MACSTR ") - "
		       "ignored\n", dev->name, MAC2STR(mgmt->sa),
		       MAC2STR(mgmt->bssid));
		return;
	}

	capab_info = le16_to_cpu(mgmt->u.assoc_resp.capab_info);
	status_code = le16_to_cpu(mgmt->u.assoc_resp.status_code);
	aid = le16_to_cpu(mgmt->u.assoc_resp.aid);
	if ((aid & (BIT(15) | BIT(14))) != (BIT(15) | BIT(14)))
		printk(KERN_DEBUG "%s: invalid aid value %d; bits 15:14 not "
		       "set\n", dev->name, aid);
	aid &= ~(BIT(15) | BIT(14));

	printk(KERN_DEBUG "%s: RX %sssocResp from " MACSTR " (capab=0x%x "
	       "status=%d aid=%d)\n",
	       dev->name, reassoc ? "Rea" : "A", MAC2STR(mgmt->sa),
	       capab_info, status_code, aid);

	if (status_code != WLAN_STATUS_SUCCESS) {
		printk(KERN_DEBUG "%s: AP denied association (code=%d)\n",
		       dev->name, status_code);
		return;
	}

	pos = mgmt->u.assoc_resp.variable;
	if (ieee802_11_parse_elems(pos, len - (pos - (u8 *) mgmt), &elems)
	    == ParseFailed) {
		printk(KERN_DEBUG "%s: failed to parse AssocResp\n",
		       dev->name);
		return;
	}

	if (elems.supp_rates == NULL) {
		printk(KERN_DEBUG "%s: no SuppRates element in AssocResp\n",
		       dev->name);
		return;
	}

	printk(KERN_DEBUG "%s: associated\n", dev->name);
	ifsta->aid = aid;
	ifsta->ap_capab = capab_info;

	kfree(ifsta->assocresp_ies);
	ifsta->assocresp_ies_len = len - (pos - (u8 *) mgmt);
	ifsta->assocresp_ies = kmalloc(ifsta->assocresp_ies_len, GFP_ATOMIC);
	if (ifsta->assocresp_ies)
		memcpy(ifsta->assocresp_ies, pos, ifsta->assocresp_ies_len);

	ieee80211_set_associated(dev, ifsta, 1);

	/* Add STA entry for the AP */
	sta = sta_info_get(local, ifsta->bssid);
	if (sta == NULL) {
		sta = sta_info_add(local, dev, ifsta->bssid);
		if (sta == NULL) {
			printk(KERN_DEBUG "%s: failed to add STA entry for the"
			       " AP\n", dev->name);
			return;
		}
	}

	sta->dev = dev;
	sta->flags |= WLAN_STA_AUTH | WLAN_STA_ASSOC;
	sta->assoc_ap = 1;

	rates = 0;
	for (i = 0; i < elems.supp_rates_len; i++) {
		int rate = (elems.supp_rates[i] & 0x7f) * 5;
		if (local->conf.phymode == MODE_ATHEROS_TURBO)
			rate *= 2;
		for (j = 0; j < local->num_curr_rates; j++)
			if (local->curr_rates[j].rate == rate)
				rates |= BIT(j);
	}
	for (i = 0; i < elems.ext_supp_rates_len; i++) {
		int rate = (elems.ext_supp_rates[i] & 0x7f) * 5;
		if (local->conf.phymode == MODE_ATHEROS_TURBO)
			rate *= 2;
		for (j = 0; j < local->num_curr_rates; j++)
			if (local->curr_rates[j].rate == rate)
				rates |= BIT(j);
	}
	sta->supp_rates = rates;

	rate_control_rate_init(local, sta);

	if (elems.wmm_param && ifsta->wmm_enabled) {
		sta->flags |= WLAN_STA_WME;
		ieee80211_sta_wmm_params(dev, ifsta, elems.wmm_param,
					 elems.wmm_param_len);
	}


	sta_info_release(local, sta);

	ieee80211_associated(dev, ifsta);
}


/* Caller must hold local->sta_bss_lock */
static void __ieee80211_rx_bss_hash_add(struct net_device *dev,
					struct ieee80211_sta_bss *bss)
{
	struct ieee80211_local *local = dev->priv;
	bss->hnext = local->sta_bss_hash[STA_HASH(bss->bssid)];
	local->sta_bss_hash[STA_HASH(bss->bssid)] = bss;
}


/* Caller must hold local->sta_bss_lock */
static void __ieee80211_rx_bss_hash_del(struct net_device *dev,
					struct ieee80211_sta_bss *bss)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sta_bss *b, *prev = NULL;
	b = local->sta_bss_hash[STA_HASH(bss->bssid)];
	while (b) {
		if (b == bss) {
			if (prev == NULL) {
				local->sta_bss_hash[STA_HASH(bss->bssid)] =
					bss->hnext;
			} else {
				prev->hnext = bss->hnext;
			}
			break;
		}
		prev = b;
		b = b->hnext;
	}
}


static struct ieee80211_sta_bss *
ieee80211_rx_bss_add(struct net_device *dev, u8 *bssid)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sta_bss *bss;

	bss = kmalloc(sizeof(*bss), GFP_ATOMIC);
	if (bss == NULL)
		return NULL;
	memset(bss, 0, sizeof(*bss));
	atomic_inc(&bss->users);
	atomic_inc(&bss->users);
	memcpy(bss->bssid, bssid, ETH_ALEN);

	spin_lock_bh(&local->sta_bss_lock);
	/* TODO: order by RSSI? */
	list_add_tail(&bss->list, &local->sta_bss_list);
	__ieee80211_rx_bss_hash_add(dev, bss);
	spin_unlock_bh(&local->sta_bss_lock);
	return bss;
}


static struct ieee80211_sta_bss *
ieee80211_rx_bss_get(struct net_device *dev, u8 *bssid)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sta_bss *bss;

	spin_lock_bh(&local->sta_bss_lock);
	bss = local->sta_bss_hash[STA_HASH(bssid)];
	while (bss) {
		if (memcmp(bss->bssid, bssid, ETH_ALEN) == 0) {
			atomic_inc(&bss->users);
			break;
		}
		bss = bss->hnext;
	}
	spin_unlock_bh(&local->sta_bss_lock);
	return bss;
}


static void ieee80211_rx_bss_free(struct ieee80211_sta_bss *bss)
{
	kfree(bss->wpa_ie);
	kfree(bss->rsn_ie);
	kfree(bss->wmm_ie);
	kfree(bss);
}


static void ieee80211_rx_bss_put(struct net_device *dev,
				 struct ieee80211_sta_bss *bss)
{
	struct ieee80211_local *local = dev->priv;
	if (!atomic_dec_and_test(&bss->users))
		return;

	spin_lock_bh(&local->sta_bss_lock);
	__ieee80211_rx_bss_hash_del(dev, bss);
	list_del(&bss->list);
	spin_unlock_bh(&local->sta_bss_lock);
	ieee80211_rx_bss_free(bss);
}


void ieee80211_rx_bss_list_init(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	spin_lock_init(&local->sta_bss_lock);
	INIT_LIST_HEAD(&local->sta_bss_list);
}


void ieee80211_rx_bss_list_deinit(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sta_bss *bss;
	struct list_head *ptr;

	for (;;) {
		ptr = local->sta_bss_list.next;
		if (!ptr || ptr == &local->sta_bss_list)
			break;
		bss = list_entry(ptr, struct ieee80211_sta_bss, list);
		ieee80211_rx_bss_put(dev, bss);
	}
}


static void ieee80211_rx_bss_info(struct net_device *dev,
				  struct ieee80211_mgmt *mgmt,
				  size_t len,
				  struct ieee80211_rx_status *rx_status,
				  int beacon)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee802_11_elems elems;
	size_t baselen;
	int channel, invalid = 0, clen;
	struct ieee80211_sta_bss *bss;
	struct sta_info *sta;
	struct ieee80211_sub_if_data *sdata;
	u64 timestamp;
	u8 *pos;

	if (!beacon && memcmp(mgmt->da, dev->dev_addr, ETH_ALEN))
		return; /* ignore ProbeResp to foreign address */

#if 0
	printk(KERN_DEBUG "%s: RX %s from " MACSTR " to " MACSTR "\n",
	       dev->name, beacon ? "Beacon" : "Probe Response",
	       MAC2STR(mgmt->sa), MAC2STR(mgmt->da));
#endif

	baselen = (u8 *) mgmt->u.beacon.variable - (u8 *) mgmt;
	if (baselen > len)
		return;

	pos = mgmt->u.beacon.timestamp;
	timestamp = ((u64) pos[7] << 56) | ((u64) pos[6] << 48) |
		((u64) pos[5] << 40) | ((u64) pos[4] << 32) |
		((u64) pos[3] << 24) | ((u64) pos[2] << 16) |
		((u64) pos[1] << 8) | ((u64) pos[0]);

	if (local->conf.mode == IW_MODE_ADHOC && beacon &&
	    memcmp(mgmt->bssid, local->bssid, ETH_ALEN) == 0) {
#ifdef IEEE80211_IBSS_DEBUG
		static unsigned long last_tsf_debug = 0;
		u64 tsf;
		if (local->hw->get_tsf)
			tsf = local->hw->get_tsf(local->mdev);
		else
			tsf = -1LLU;
		if (time_after(jiffies, last_tsf_debug + 5 * HZ)) {
			printk(KERN_DEBUG "RX beacon SA=" MACSTR " BSSID="
			       MACSTR " TSF=0x%llx BCN=0x%llx diff=%lld "
			       "@%ld\n",
			       MAC2STR(mgmt->sa), MAC2STR(mgmt->bssid),
			       tsf, timestamp, tsf - timestamp, jiffies);
			last_tsf_debug = jiffies;
		}
#endif /* IEEE80211_IBSS_DEBUG */
	}

	if (ieee802_11_parse_elems(mgmt->u.beacon.variable, len - baselen,
				   &elems) == ParseFailed)
		invalid = 1;

	if (local->conf.mode == IW_MODE_ADHOC && elems.supp_rates &&
	    memcmp(mgmt->bssid, local->bssid, ETH_ALEN) == 0 &&
	    (sta = sta_info_get(local, mgmt->sa)) &&
	    (sdata = IEEE80211_DEV_TO_SUB_IF(dev)) &&
	    sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
		struct ieee80211_rate *rates;
		size_t num_rates;
		u32 supp_rates, prev_rates;
		int i, j, oper_mode;

		rates = local->curr_rates;
		num_rates = local->num_curr_rates;
		oper_mode = local->sta_scanning ? local->scan_oper_phymode :
			local->conf.phymode;
		for (i = 0; i < local->hw->num_modes; i++) {
			struct ieee80211_hw_modes *mode = &local->hw->modes[i];
			if (oper_mode == mode->mode) {
				rates = mode->rates;
				num_rates = mode->num_rates;
				break;
			}
		}

		supp_rates = 0;
		for (i = 0; i < elems.supp_rates_len +
			     elems.ext_supp_rates_len; i++) {
			u8 rate = 0;
			int own_rate;
			if (i < elems.supp_rates_len)
				rate = elems.supp_rates[i];
			else if (elems.ext_supp_rates)
				rate = elems.ext_supp_rates
					[i - elems.supp_rates_len];
			own_rate = 5 * (rate & 0x7f);
			if (oper_mode == MODE_ATHEROS_TURBO)
				own_rate *= 2;
			for (j = 0; j < num_rates; j++)
				if (rates[j].rate == own_rate)
					supp_rates |= BIT(j);
		}

		prev_rates = sta->supp_rates;
		sta->supp_rates &= supp_rates;
		if (sta->supp_rates == 0) {
			/* No matching rates - this should not really happen.
			 * Make sure that at least one rate is marked
			 * supported to avoid issues with TX rate ctrl. */
			
			sta->supp_rates = sdata->u.sta.supp_rates_bits;
		}
		if (sta->supp_rates != prev_rates) {
			printk(KERN_DEBUG "%s: updated supp_rates set for "
			       MACSTR " based on beacon info (0x%x & 0x%x -> "
			       "0x%x)\n",
			       dev->name, MAC2STR(sta->addr), prev_rates,
			       supp_rates, sta->supp_rates);
		}
		sta_info_release(local, sta);
	}

	if (elems.ssid == NULL)
		return;

	if (elems.ds_params && elems.ds_params_len == 1)
		channel = elems.ds_params[0];
	else
		channel = rx_status->channel;

	bss = ieee80211_rx_bss_get(dev, mgmt->bssid);
	if (bss == NULL) {
		bss = ieee80211_rx_bss_add(dev, mgmt->bssid);
		if (bss == NULL)
			return;
	} else {
#if 0
		/* TODO: order by RSSI? */
		spin_lock_bh(&local->sta_bss_lock);
		list_move_tail(&bss->list, &local->sta_bss_list);
		spin_unlock_bh(&local->sta_bss_lock);
#endif
	}

	if (bss->probe_resp && beacon) {
		/* Do not allow beacon to override data from Probe Response. */
		ieee80211_rx_bss_put(dev, bss);
		return;
	}

	bss->beacon_int = le16_to_cpu(mgmt->u.beacon.beacon_int);
	bss->capability = le16_to_cpu(mgmt->u.beacon.capab_info);
	if (elems.ssid && elems.ssid_len <= IEEE80211_MAX_SSID_LEN) {
		memcpy(bss->ssid, elems.ssid, elems.ssid_len);
		bss->ssid_len = elems.ssid_len;
	}

	bss->supp_rates_len = 0;
	if (elems.supp_rates) {
		clen = IEEE80211_MAX_SUPP_RATES - bss->supp_rates_len;
		if (clen > elems.supp_rates_len)
			clen = elems.supp_rates_len;
		memcpy(&bss->supp_rates[bss->supp_rates_len], elems.supp_rates,
		       clen);
		bss->supp_rates_len += clen;
	}
	if (elems.ext_supp_rates) {
		clen = IEEE80211_MAX_SUPP_RATES - bss->supp_rates_len;
		if (clen > elems.ext_supp_rates_len)
			clen = elems.ext_supp_rates_len;
		memcpy(&bss->supp_rates[bss->supp_rates_len],
		       elems.ext_supp_rates, clen);
		bss->supp_rates_len += clen;
	}

	if (elems.wpa &&
	    (bss->wpa_ie == NULL || bss->wpa_ie_len != elems.wpa_len ||
	     memcmp(bss->wpa_ie, elems.wpa, elems.wpa_len))) {
		kfree(bss->wpa_ie);
		bss->wpa_ie = kmalloc(elems.wpa_len + 2, GFP_ATOMIC);
		if (bss->wpa_ie) {
			memcpy(bss->wpa_ie, elems.wpa - 2, elems.wpa_len + 2);
			bss->wpa_ie_len = elems.wpa_len + 2;
		} else
			bss->wpa_ie_len = 0;
	} else if (!elems.wpa && bss->wpa_ie) {
		kfree(bss->wpa_ie);
		bss->wpa_ie = NULL;
		bss->wpa_ie_len = 0;
	}

	if (elems.rsn &&
	    (bss->rsn_ie == NULL || bss->rsn_ie_len != elems.rsn_len ||
	     memcmp(bss->rsn_ie, elems.rsn, elems.rsn_len))) {
		kfree(bss->rsn_ie);
		bss->rsn_ie = kmalloc(elems.rsn_len + 2, GFP_ATOMIC);
		if (bss->rsn_ie) {
			memcpy(bss->rsn_ie, elems.rsn - 2, elems.rsn_len + 2);
			bss->rsn_ie_len = elems.rsn_len + 2;
		} else
			bss->rsn_ie_len = 0;
	} else if (!elems.rsn && bss->rsn_ie) {
		kfree(bss->rsn_ie);
		bss->rsn_ie = NULL;
		bss->rsn_ie_len = 0;
	}

	if (elems.wmm_param &&
	    (bss->wmm_ie == NULL || bss->wmm_ie_len != elems.wmm_param_len ||
	     memcmp(bss->wmm_ie, elems.wmm_param, elems.wmm_param_len))) {
		kfree(bss->wmm_ie);
		bss->wmm_ie = kmalloc(elems.wmm_param_len + 2, GFP_ATOMIC);
		if (bss->wmm_ie) {
			memcpy(bss->wmm_ie, elems.wmm_param - 2,
			       elems.wmm_param_len + 2);
			bss->wmm_ie_len = elems.wmm_param_len + 2;
		} else
			bss->wmm_ie_len = 0;
	} else if (!elems.wmm_param && bss->wmm_ie) {
		kfree(bss->wmm_ie);
		bss->wmm_ie = NULL;
		bss->wmm_ie_len = 0;
	}


	bss->hw_mode = local->conf.phymode;
	bss->channel = channel;
	bss->freq = local->conf.freq;
	if (channel != local->conf.channel &&
	    (local->conf.phymode == MODE_IEEE80211G ||
	     local->conf.phymode == MODE_IEEE80211B) &&
	    channel >= 1 && channel <= 14) {
		static const int freq_list[] = {
			2412, 2417, 2422, 2427, 2432, 2437, 2442,
			2447, 2452, 2457, 2462, 2467, 2472, 2484
		};
		/* IEEE 802.11g/b mode can receive packets from neighboring
		 * channels, so map the channel into frequency. */
		bss->freq = freq_list[channel - 1];
	}
	bss->timestamp = timestamp;
	bss->last_update = jiffies;
	bss->rssi = rx_status->ssi;
	if (!beacon)
		bss->probe_resp++;
	ieee80211_rx_bss_put(dev, bss);
}


static void ieee80211_rx_mgmt_probe_resp(struct net_device *dev,
					 struct ieee80211_mgmt *mgmt,
					 size_t len,
					 struct ieee80211_rx_status *rx_status)
{
	ieee80211_rx_bss_info(dev, mgmt, len, rx_status, 0);
}


static void ieee80211_rx_mgmt_beacon(struct net_device *dev,
				     struct ieee80211_mgmt *mgmt,
				     size_t len,
				     struct ieee80211_rx_status *rx_status)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_sta *ifsta;
	int use_protection;
	size_t baselen;
	struct ieee802_11_elems elems;

	ieee80211_rx_bss_info(dev, mgmt, len, rx_status, 1);

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
		return;
	ifsta = &sdata->u.sta;

	if (!ifsta->associated ||
	    memcmp(ifsta->bssid, mgmt->bssid, ETH_ALEN) != 0)
		return;

	/* Process beacon from the current BSS */
	baselen = (u8 *) mgmt->u.beacon.variable - (u8 *) mgmt;
	if (baselen > len)
		return;

	if (ieee802_11_parse_elems(mgmt->u.beacon.variable, len - baselen,
				   &elems) == ParseFailed)
		return;

	use_protection = 0;
	if (elems.erp_info && elems.erp_info_len >= 1) {
		use_protection =
			(elems.erp_info[0] & ERP_INFO_USE_PROTECTION) != 0;
	}

	if (use_protection != !!ifsta->use_protection) {
		if (net_ratelimit()) {
			printk(KERN_DEBUG "%s: CTS protection %s (BSSID="
			       MACSTR ")\n",
			       dev->name,
			       use_protection ? "enabled" : "disabled",
			       MAC2STR(ifsta->bssid));
		}
		ifsta->use_protection = use_protection ? 1 : 0;
		local->cts_protect_erp_frames = use_protection;
	}

	if (elems.wmm_param && ifsta->wmm_enabled) {
		ieee80211_sta_wmm_params(dev, ifsta, elems.wmm_param,
					 elems.wmm_param_len);
	}
}


static void ieee80211_rx_mgmt_probe_req(struct net_device *dev,
					struct ieee80211_if_sta *ifsta,
					struct ieee80211_mgmt *mgmt,
					size_t len,
					struct ieee80211_rx_status *rx_status)
{
	struct ieee80211_local *local = dev->priv;
	int tx_last_beacon;
	struct sk_buff *skb;
	struct ieee80211_mgmt *resp;
	u8 *pos, *end;

	if (local->conf.mode != IW_MODE_ADHOC ||
	    ifsta->state != IEEE80211_IBSS_JOINED ||
	    len < 24 + 2 || ifsta->probe_resp == NULL)
		return;

	if (local->hw->tx_last_beacon)
		tx_last_beacon = local->hw->tx_last_beacon(local->mdev);
	else
		tx_last_beacon = 1;

#ifdef IEEE80211_IBSS_DEBUG
	printk(KERN_DEBUG "%s: RX ProbeReq SA=" MACSTR " DA=" MACSTR " BSSID="
	       MACSTR " (tx_last_beacon=%d)\n",
	       dev->name, MAC2STR(mgmt->sa), MAC2STR(mgmt->da),
	       MAC2STR(mgmt->bssid), tx_last_beacon);
#endif /* IEEE80211_IBSS_DEBUG */

	if (!tx_last_beacon)
		return;

	if (memcmp(mgmt->bssid, ifsta->bssid, ETH_ALEN) != 0 &&
	    memcmp(mgmt->bssid, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) != 0)
		return;

	end = ((u8 *) mgmt) + len;
	pos = mgmt->u.probe_req.variable;
	if (pos[0] != WLAN_EID_SSID ||
	    pos + 2 + pos[1] > end) {
		if (net_ratelimit()) {
			printk(KERN_DEBUG "%s: Invalid SSID IE in ProbeReq "
			       "from " MACSTR "\n",
			       dev->name, MAC2STR(mgmt->sa));
		}
		return;
	}
	if (pos[1] != 0 &&
	    (pos[1] != ifsta->ssid_len ||
	     memcmp(pos + 2, ifsta->ssid, ifsta->ssid_len) != 0)) {
		/* Ignore ProbeReq for foreign SSID */
		return;
	}

	/* Reply with ProbeResp */
	skb = skb_copy(ifsta->probe_resp, GFP_ATOMIC);
	if (skb == NULL)
		return;

	resp = (struct ieee80211_mgmt *) skb->data;
	memcpy(resp->da, mgmt->sa, ETH_ALEN);
#ifdef IEEE80211_IBSS_DEBUG
	printk(KERN_DEBUG "%s: Sending ProbeResp to " MACSTR "\n",
	       dev->name, MAC2STR(resp->da));
#endif /* IEEE80211_IBSS_DEBUG */
	ieee80211_sta_tx(dev, skb, 0, 1);
}


void ieee80211_sta_rx_mgmt(struct net_device *dev, struct sk_buff *skb,
			   struct ieee80211_rx_status *rx_status)
{
        struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_sta *ifsta;
	struct ieee80211_mgmt *mgmt;
	u16 fc;

	if (skb->len < 24)
		goto fail;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_SUB_IF_TYPE_STA) {
		printk(KERN_DEBUG "%s: ieee80211_sta_rx_mgmt: non-STA "
		       "interface (type=%d)\n", dev->name, sdata->type);
		goto fail;
	}
	ifsta = &sdata->u.sta;

	mgmt = (struct ieee80211_mgmt *) skb->data;
	fc = le16_to_cpu(mgmt->frame_control);

	switch (WLAN_FC_GET_STYPE(fc)) {
	case WLAN_FC_STYPE_PROBE_REQ:
		ieee80211_rx_mgmt_probe_req(dev, ifsta, mgmt, skb->len,
					    rx_status);
		break;
	case WLAN_FC_STYPE_PROBE_RESP:
		ieee80211_rx_mgmt_probe_resp(dev, mgmt, skb->len, rx_status);
		break;
	case WLAN_FC_STYPE_BEACON:
		ieee80211_rx_mgmt_beacon(dev, mgmt, skb->len, rx_status);
		break;
	case WLAN_FC_STYPE_AUTH:
		ieee80211_rx_mgmt_auth(dev, ifsta, mgmt, skb->len, rx_status);
		break;
	case WLAN_FC_STYPE_ASSOC_RESP:
		ieee80211_rx_mgmt_assoc_resp(dev, ifsta, mgmt, skb->len,
					     rx_status, 0);
		break;
	case WLAN_FC_STYPE_REASSOC_RESP:
		ieee80211_rx_mgmt_assoc_resp(dev, ifsta, mgmt, skb->len,
					     rx_status, 1);
		break;
	case WLAN_FC_STYPE_DEAUTH:
		ieee80211_rx_mgmt_deauth(dev, ifsta, mgmt, skb->len,
					 rx_status);
		break;
	case WLAN_FC_STYPE_DISASSOC:
		ieee80211_rx_mgmt_disassoc(dev, ifsta, mgmt, skb->len,
					   rx_status);
		break;
	default:
		printk(KERN_DEBUG "%s: received unknown management frame - "
		       "stype=%d\n", dev->name, WLAN_FC_GET_STYPE(fc));
		break;
	}

 fail:
	dev_kfree_skb(skb);
}


void ieee80211_sta_rx_scan(struct net_device *dev, struct sk_buff *skb,
			   struct ieee80211_rx_status *rx_status)
{
	struct ieee80211_mgmt *mgmt;
	u16 fc;

	if (skb->len < 24) {
		dev_kfree_skb(skb);
		return;
	}

	mgmt = (struct ieee80211_mgmt *) skb->data;
	fc = le16_to_cpu(mgmt->frame_control);

	if (WLAN_FC_GET_TYPE(fc) == WLAN_FC_TYPE_MGMT) {
		if (WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_PROBE_RESP) {
			ieee80211_rx_mgmt_probe_resp(dev, mgmt,
						     skb->len, rx_status);
		} else if (WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_BEACON) {
			ieee80211_rx_mgmt_beacon(dev, mgmt, skb->len,
						 rx_status);
		}
	}

	dev_kfree_skb(skb);
}


static int ieee80211_sta_active_ibss(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	struct list_head *ptr;
	int active = 0;
	struct sta_info *sta;

	spin_lock_bh(&local->sta_lock);
	list_for_each(ptr, &local->sta_list) {
		sta = list_entry(ptr, struct sta_info, list);
		if (sta->dev == dev &&
		    time_after(sta->last_rx + IEEE80211_IBSS_MERGE_INTERVAL,
			       jiffies)) {
			active++;
			break;
		}
	}
	spin_unlock_bh(&local->sta_lock);

	return active;
}


static void ieee80211_sta_expire(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	struct list_head *ptr, *n;
	struct sta_info *sta;

	spin_lock_bh(&local->sta_lock);
	list_for_each_safe(ptr, n, &local->sta_list) {
		sta = list_entry(ptr, struct sta_info, list);
		if (time_after(jiffies, sta->last_rx +
			       IEEE80211_IBSS_INACTIVITY_LIMIT)) {
			printk(KERN_DEBUG "%s: expiring inactive STA " MACSTR
			       "\n", dev->name, MAC2STR(sta->addr));
			sta_info_free(local, sta, 1);
		}
	}
	spin_unlock_bh(&local->sta_lock);
}


static void ieee80211_sta_merge_ibss(struct net_device *dev,
				     struct ieee80211_if_sta *ifsta)
{
	mod_timer(&ifsta->timer, jiffies + IEEE80211_IBSS_MERGE_INTERVAL);

	ieee80211_sta_expire(dev);
	if (ieee80211_sta_active_ibss(dev))
		return;

	printk(KERN_DEBUG "%s: No active IBSS STAs - trying to scan for other "
	       "IBSS networks with same SSID (merge)\n", dev->name);
	ieee80211_sta_req_scan(dev, ifsta->ssid, ifsta->ssid_len);
}


void ieee80211_sta_timer(unsigned long ptr)
{
	struct net_device *dev;
        struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_sta *ifsta;

	dev = (struct net_device *) ptr;
	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata->type != IEEE80211_SUB_IF_TYPE_STA) {
		printk(KERN_DEBUG "%s: ieee80211_sta_timer: non-STA interface "
		       "(type=%d)\n", dev->name, sdata->type);
		return;
	}
	ifsta = &sdata->u.sta;

	switch (ifsta->state) {
	case IEEE80211_DISABLED:
		break;
	case IEEE80211_AUTHENTICATE:
		ieee80211_authenticate(dev, ifsta);
		break;
	case IEEE80211_ASSOCIATE:
		ieee80211_associate(dev, ifsta);
		break;
	case IEEE80211_ASSOCIATED:
		ieee80211_associated(dev, ifsta);
		break;
	case IEEE80211_IBSS_SEARCH:
		ieee80211_sta_find_ibss(dev, ifsta);
		break;
	case IEEE80211_IBSS_JOINED:
		ieee80211_sta_merge_ibss(dev, ifsta);
		break;
	default:
		printk(KERN_DEBUG "ieee80211_sta_timer: Unknown state %d\n",
		       ifsta->state);
		break;
	}

	if (ieee80211_privacy_mismatch(dev, ifsta)) {
		printk(KERN_DEBUG "%s: privacy configuration mismatch and "
		       "mixed-cell disabled - disassociate\n", dev->name);

		ieee80211_send_disassoc(dev, ifsta, WLAN_REASON_UNSPECIFIED);
		ieee80211_set_associated(dev, ifsta, 0);
	}
}


static void ieee80211_sta_new_auth(struct net_device *dev,
				   struct ieee80211_if_sta *ifsta)
{
	struct ieee80211_local *local = dev->priv;

	if (local->conf.mode != IW_MODE_INFRA)
		return;

	if (local->hw->reset_tsf) {
		/* Reset own TSF to allow time synchronization work. */
		local->hw->reset_tsf(local->mdev);
	}

	ifsta->wmm_last_param_set = -1; /* allow any WMM update */


	if (ifsta->auth_algs & IEEE80211_AUTH_ALG_OPEN)
		ifsta->auth_alg = WLAN_AUTH_OPEN;
	else if (ifsta->auth_algs & IEEE80211_AUTH_ALG_SHARED_KEY)
		ifsta->auth_alg = WLAN_AUTH_SHARED_KEY;
	else if (ifsta->auth_algs & IEEE80211_AUTH_ALG_LEAP)
		ifsta->auth_alg = WLAN_AUTH_LEAP;
	else
		ifsta->auth_alg = WLAN_AUTH_OPEN;
	printk(KERN_DEBUG "%s: Initial auth_alg=%d\n", dev->name,
	       ifsta->auth_alg);
	ifsta->auth_transaction = -1;
	ifsta->auth_tries = ifsta->assoc_tries = 0;
	ieee80211_authenticate(dev, ifsta);
}


static int ieee80211_ibss_allowed(struct ieee80211_local *local)
{
	int m, c;

	for (m = 0; m < local->hw->num_modes; m++) {
		struct ieee80211_hw_modes *mode = &local->hw->modes[m];
		if (mode->mode != local->conf.phymode)
			continue;
		for (c = 0; c < mode->num_channels; c++) {
			struct ieee80211_channel *chan = &mode->channels[c];
			if (chan->flag & IEEE80211_CHAN_W_SCAN &&
			    chan->chan == local->conf.channel) {
				if (chan->flag & IEEE80211_CHAN_W_IBSS)
					return 1;
				break;
			}
		}
	}

	return 0;
}


extern int ieee80211_ioctl_siwfreq(struct net_device *dev,
				   struct iw_request_info *info,
				   struct iw_freq *freq, char *extra);

static int ieee80211_sta_join_ibss(struct net_device *dev,
				   struct ieee80211_if_sta *ifsta,
				   struct ieee80211_sta_bss *bss)
{
	struct ieee80211_local *local = dev->priv;
	struct iw_freq rq;
	int res, rates, i, j;
	struct sk_buff *skb;
	struct ieee80211_mgmt *mgmt;
	struct ieee80211_tx_control control;
	struct ieee80211_rate *rate;
	struct rate_control_extra extra;
	u8 *pos;
        struct ieee80211_sub_if_data *sdata;

	/* Remove possible STA entries from other IBSS networks. */
	sta_info_flush(local, NULL);

	if (local->hw->reset_tsf) {
		/* Reset own TSF to allow time synchronization work. */
		local->hw->reset_tsf(local->mdev);
	}
	memcpy(ifsta->bssid, bss->bssid, ETH_ALEN);
	memcpy(local->bssid, bss->bssid, ETH_ALEN);
	memcpy(local->conf.client_bssid, bss->bssid, ETH_ALEN);

	local->conf.beacon_int = bss->beacon_int >= 10 ? bss->beacon_int : 10;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	sdata->drop_unencrypted = bss->capability &
		cpu_to_le16(WLAN_CAPABILITY_PRIVACY) ? 1 : 0;

	memset(&rq, 0, sizeof(rq));
	rq.m = bss->freq * 100000;
	rq.e = 1;
	res = ieee80211_ioctl_siwfreq(dev, NULL, &rq, NULL);

	if (!ieee80211_ibss_allowed(local)) {
		printk(KERN_DEBUG "%s: IBSS not allowed on channel %d "
		       "(%d MHz)\n", dev->name, local->conf.channel,
		       local->conf.freq);
		return -1;
	}

	/* Set beacon template based on scan results */
	skb = dev_alloc_skb(400);
	do {
		if (skb == NULL)
			break;

		mgmt = (struct ieee80211_mgmt *)
			skb_put(skb, 24 + sizeof(mgmt->u.beacon));
		memset(mgmt, 0, 24 + sizeof(mgmt->u.beacon));
		mgmt->frame_control = IEEE80211_FC(WLAN_FC_TYPE_MGMT,
						   WLAN_FC_STYPE_BEACON);
		memset(mgmt->da, 0xff, ETH_ALEN);
		memcpy(mgmt->sa, dev->dev_addr, ETH_ALEN);
		memcpy(mgmt->bssid, ifsta->bssid, ETH_ALEN);
		mgmt->u.beacon.beacon_int =
			cpu_to_le16(local->conf.beacon_int);
		mgmt->u.beacon.capab_info = cpu_to_le16(bss->capability);

		pos = skb_put(skb, 2 + ifsta->ssid_len);
		*pos++ = WLAN_EID_SSID;
		*pos++ = ifsta->ssid_len;
		memcpy(pos, ifsta->ssid, ifsta->ssid_len);

		rates = bss->supp_rates_len;
		if (rates > 8)
			rates = 8;
		pos = skb_put(skb, 2 + rates);
		*pos++ = WLAN_EID_SUPP_RATES;
		*pos++ = rates;
		memcpy(pos, bss->supp_rates, rates);

		pos = skb_put(skb, 2 + 1);
		*pos++ = WLAN_EID_DS_PARAMS;
		*pos++ = 1;
		*pos++ = bss->channel;

		pos = skb_put(skb, 2 + 2);
		*pos++ = WLAN_EID_IBSS_PARAMS;
		*pos++ = 2;
		/* FIX: set ATIM window based on scan results */
		*pos++ = 0;
		*pos++ = 0;

		if (bss->supp_rates_len > 8) {
			rates = bss->supp_rates_len - 8;
			pos = skb_put(skb, 2 + rates);
			*pos++ = WLAN_EID_EXT_SUPP_RATES;
			*pos++ = rates;
			memcpy(pos, &bss->supp_rates[8], rates);
		}

		memset(&control, 0, sizeof(control));
		control.pkt_type = PKT_PROBE_RESP;
		memset(&extra, 0, sizeof(extra));
		extra.endidx = local->num_curr_rates;
		rate = rate_control_get_rate(dev, skb, &extra);
		if (rate == NULL) {
			printk(KERN_DEBUG "%s: Failed to determine TX rate "
			       "for IBSS beacon\n", dev->name);
			break;
		}
		control.tx_rate = (local->short_preamble &&
				   (rate->flags & IEEE80211_RATE_PREAMBLE2)) ?
			rate->val2 : rate->val;
		control.antenna_sel = local->conf.antenna_sel;
		control.power_level = local->conf.power_level;
		control.no_ack = 1;
		control.retry_limit = 1;
		control.rts_cts_duration = 0;

		ifsta->probe_resp = skb_copy(skb, GFP_ATOMIC);
		if (ifsta->probe_resp) {
			mgmt = (struct ieee80211_mgmt *)
				ifsta->probe_resp->data;
			mgmt->frame_control =
				IEEE80211_FC(WLAN_FC_TYPE_MGMT,
					     WLAN_FC_STYPE_PROBE_RESP);
		} else {
			printk(KERN_DEBUG "%s: Could not allocate ProbeResp "
			       "template for IBSS\n", dev->name);
		}

		if (local->hw->beacon_update &&
		    local->hw->beacon_update(dev, skb, &control) == 0) {
			printk(KERN_DEBUG "%s: Configured IBSS beacon "
			       "template based on scan results\n", dev->name);
			skb = NULL;
		}

		rates = 0;
		for (i = 0; i < bss->supp_rates_len; i++) {
			int rate = (bss->supp_rates[i] & 0x7f) * 5;
			if (local->conf.phymode == MODE_ATHEROS_TURBO)
				rate *= 2;
			for (j = 0; j < local->num_curr_rates; j++)
				if (local->curr_rates[j].rate == rate)
					rates |= BIT(j);
		}
		ifsta->supp_rates_bits = rates;
	} while (0);

	if (skb) {
		printk(KERN_DEBUG "%s: Failed to configure IBSS beacon "
		       "template\n", dev->name);
		dev_kfree_skb(skb);
	}

	ifsta->state = IEEE80211_IBSS_JOINED;
	mod_timer(&ifsta->timer, jiffies + IEEE80211_IBSS_MERGE_INTERVAL);

	ieee80211_rx_bss_put(dev, bss);

	return res;
}


static int ieee80211_sta_create_ibss(struct net_device *dev,
				     struct ieee80211_if_sta *ifsta)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sta_bss *bss;
        struct ieee80211_sub_if_data *sdata;
	u8 bssid[ETH_ALEN], *pos;
	int i;

#if 0
	/* Easier testing, use fixed BSSID. */
	memset(bssid, 0xfe, ETH_ALEN);
#else
	/* Generate random, not broadcast, locally administered BSSID. Mix in
	 * own MAC address to make sure that devices that do not have proper
	 * random number generator get different BSSID. */
	get_random_bytes(bssid, ETH_ALEN);
	for (i = 0; i < ETH_ALEN; i++)
		bssid[i] ^= dev->dev_addr[i];
	bssid[0] &= ~0x01;
	bssid[0] |= 0x02;
#endif

	printk(KERN_DEBUG "%s: Creating new IBSS network, BSSID " MACSTR "\n",
	       dev->name, MAC2STR(bssid));

	bss = ieee80211_rx_bss_add(dev, bssid);
	if (bss == NULL)
		return -ENOMEM;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	if (local->conf.beacon_int == 0)
		local->conf.beacon_int = 100;
	bss->beacon_int = local->conf.beacon_int;
	bss->hw_mode = local->conf.phymode;
	bss->channel = local->conf.channel;
	bss->freq = local->conf.freq;
	bss->last_update = jiffies;
	bss->capability = cpu_to_le16(WLAN_CAPABILITY_IBSS);
	if (sdata->default_key) {
		bss->capability |= cpu_to_le16(WLAN_CAPABILITY_PRIVACY);
	} else
		sdata->drop_unencrypted = 0;
	bss->supp_rates_len = local->num_curr_rates;
	pos = bss->supp_rates;
	for (i = 0; i < local->num_curr_rates; i++) {
		int rate = local->curr_rates[i].rate;
		if (local->conf.phymode == MODE_ATHEROS_TURBO)
			rate /= 2;
		*pos++ = (u8) (rate / 5);
	}

	return ieee80211_sta_join_ibss(dev, ifsta, bss);
}


static int ieee80211_sta_find_ibss(struct net_device *dev,
				   struct ieee80211_if_sta *ifsta)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sta_bss *bss;
	int found = 0;
	u8 bssid[ETH_ALEN];
	struct list_head *ptr;
	int active_ibss;

	if (ifsta->ssid_len == 0)
		return -EINVAL;

	active_ibss = ieee80211_sta_active_ibss(dev);
#ifdef IEEE80211_IBSS_DEBUG
	printk(KERN_DEBUG "%s: sta_find_ibss (active_ibss=%d)\n",
	       dev->name, active_ibss);
#endif /* IEEE80211_IBSS_DEBUG */
	spin_lock_bh(&local->sta_bss_lock);
	list_for_each(ptr, &local->sta_bss_list) {
		bss = list_entry(ptr, struct ieee80211_sta_bss, list);
		if (ifsta->ssid_len != bss->ssid_len ||
		    memcmp(ifsta->ssid, bss->ssid, bss->ssid_len) != 0
		    || !(bss->capability & WLAN_CAPABILITY_IBSS))
			continue;
#ifdef IEEE80211_IBSS_DEBUG
		printk(KERN_DEBUG "   bssid=" MACSTR " found\n",
		       MAC2STR(bss->bssid));
#endif /* IEEE80211_IBSS_DEBUG */
		memcpy(bssid, bss->bssid, ETH_ALEN);
		found = 1;
		if (active_ibss || memcmp(bssid, ifsta->bssid, ETH_ALEN) != 0)
			break;
	}
	spin_unlock_bh(&local->sta_bss_lock);

#ifdef IEEE80211_IBSS_DEBUG
	printk(KERN_DEBUG "   sta_find_ibss: selected " MACSTR " current "
	       MACSTR "\n", MAC2STR(bssid), MAC2STR(ifsta->bssid));
#endif /* IEEE80211_IBSS_DEBUG */
	if (found && memcmp(ifsta->bssid, bssid, ETH_ALEN) != 0 &&
	    (bss = ieee80211_rx_bss_get(dev, bssid))) {
		printk(KERN_DEBUG "%s: Selected IBSS BSSID " MACSTR
		       " based on configured SSID\n",
		       dev->name, MAC2STR(bssid));
		return ieee80211_sta_join_ibss(dev, ifsta, bss);
	}
#ifdef IEEE80211_IBSS_DEBUG
	printk(KERN_DEBUG "   did not try to join ibss\n");
#endif /* IEEE80211_IBSS_DEBUG */

	/* Selected IBSS not found in current scan results - try to scan */
	if (ifsta->state == IEEE80211_IBSS_JOINED && 
	    !ieee80211_sta_active_ibss(dev)) {
		mod_timer(&ifsta->timer,
			  jiffies + IEEE80211_IBSS_MERGE_INTERVAL);
	} else if (time_after(jiffies, local->last_scan_completed +
			      IEEE80211_SCAN_INTERVAL)) {
		printk(KERN_DEBUG "%s: Trigger new scan to find an IBSS to "
		       "join\n", dev->name);
		return ieee80211_sta_req_scan(dev, ifsta->ssid,
					      ifsta->ssid_len);
	} else if (ifsta->state != IEEE80211_IBSS_JOINED) {
		int interval = IEEE80211_SCAN_INTERVAL;

		if (time_after(jiffies, ifsta->ibss_join_req +
			       IEEE80211_IBSS_JOIN_TIMEOUT)) {
			if (ifsta->create_ibss &&
			    ieee80211_ibss_allowed(local))
				return ieee80211_sta_create_ibss(dev, ifsta);
			if (ifsta->create_ibss) {
				printk(KERN_DEBUG "%s: IBSS not allowed on the"
				       " configured channel %d (%d MHz)\n",
				       dev->name, local->conf.channel,
				       local->conf.freq);
			}

			/* No IBSS found - decrease scan interval and continue
			 * scanning. */
			interval = IEEE80211_SCAN_INTERVAL_SLOW;
		}

		ifsta->state = IEEE80211_IBSS_SEARCH;
		mod_timer(&ifsta->timer, jiffies + interval);
		return 0;
	}

	return 0;
}


int ieee80211_sta_set_ssid(struct net_device *dev, char *ssid, size_t len)
{
        struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_sta *ifsta;
	struct ieee80211_local *local = dev->priv;

	if (len > IEEE80211_MAX_SSID_LEN)
		return -EINVAL;

	/* TODO: This should always be done for IBSS, even if IEEE80211_QOS is
	 * not defined. */
	if (local->hw->conf_tx) {
		struct ieee80211_tx_queue_params qparam;
		int i;

		memset(&qparam, 0, sizeof(qparam));
		/* TODO: are these ok defaults for all hw_modes? */
		qparam.aifs = 2;
		qparam.cw_min =
			local->conf.phymode == MODE_IEEE80211B ? 31 : 15;
		qparam.cw_max = 1023;
		qparam.burst_time = 0;
		for (i = IEEE80211_TX_QUEUE_DATA0; i < NUM_TX_DATA_QUEUES; i++)
		{
			local->hw->conf_tx(dev, i + IEEE80211_TX_QUEUE_DATA0,
					   &qparam);
		}
		/* IBSS uses different parameters for Beacon sending */
		qparam.cw_min++;
		qparam.cw_min *= 2;
		qparam.cw_min--;
		local->hw->conf_tx(dev, IEEE80211_TX_QUEUE_BEACON, &qparam);
	}

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	ifsta = &sdata->u.sta;

	if (ifsta->ssid_len != len || memcmp(ifsta->ssid, ssid, len) != 0)
		ifsta->prev_bssid_set = 0;
	memcpy(ifsta->ssid, ssid, len);
	memset(ifsta->ssid + len, 0, IEEE80211_MAX_SSID_LEN - len);
	ifsta->ssid_len = len;

	ifsta->ssid_set = 1;
	if (local->conf.mode == IW_MODE_ADHOC && !ifsta->bssid_set) {
		ifsta->ibss_join_req = jiffies;
		ifsta->state = IEEE80211_IBSS_SEARCH;
		return ieee80211_sta_find_ibss(dev, ifsta);
	}

	if (ifsta->bssid_set && ifsta->state != IEEE80211_AUTHENTICATE)
		ieee80211_sta_new_auth(dev, ifsta);

	return 0;
}


int ieee80211_sta_get_ssid(struct net_device *dev, char *ssid, size_t *len)
{
        struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	struct ieee80211_if_sta *ifsta = &sdata->u.sta;
	memcpy(ssid, ifsta->ssid, ifsta->ssid_len);
	*len = ifsta->ssid_len;
	return 0;
}


int ieee80211_sta_set_bssid(struct net_device *dev, u8 *bssid)
{
	struct ieee80211_local *local = dev->priv;
        struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_sta *ifsta;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	ifsta = &sdata->u.sta;

	memcpy(ifsta->bssid, bssid, ETH_ALEN);
	if (local->conf.mode == IW_MODE_ADHOC)
		memcpy(local->bssid, bssid, ETH_ALEN);

	if (memcmp(bssid, "\x00\x00\x00\x00\x00\x00", ETH_ALEN) == 0)
		ifsta->bssid_set = 0;
	else
		ifsta->bssid_set = 1;
	if (ifsta->ssid_set)
		ieee80211_sta_new_auth(dev, ifsta);

	return 0;
}


static void ieee80211_sta_save_oper_chan(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	local->scan_oper_channel = local->conf.channel;
	local->scan_oper_channel_val = local->conf.channel_val;
	local->scan_oper_power_level = local->conf.power_level;
	local->scan_oper_freq = local->conf.freq;
	local->scan_oper_phymode = local->conf.phymode;
	local->scan_oper_antenna_max = local->conf.antenna_max;
}


static int ieee80211_sta_restore_oper_chan(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	local->conf.channel = local->scan_oper_channel;
	local->conf.channel_val = local->scan_oper_channel_val;
	local->conf.power_level = local->scan_oper_power_level;
	local->conf.freq = local->scan_oper_freq;
	local->conf.phymode = local->scan_oper_phymode;
	local->conf.antenna_max = local->scan_oper_antenna_max;
	return ieee80211_hw_config(dev);
}


static int ieee80211_active_scan(struct ieee80211_local *local)
{
	int m, c;

	for (m = 0; m < local->hw->num_modes; m++) {
		struct ieee80211_hw_modes *mode = &local->hw->modes[m];
		if (mode->mode != local->conf.phymode)
			continue;
		for (c = 0; c < mode->num_channels; c++) {
			struct ieee80211_channel *chan = &mode->channels[c];
			if (chan->flag & IEEE80211_CHAN_W_SCAN &&
			    chan->chan == local->conf.channel) {
				if (chan->flag & IEEE80211_CHAN_W_ACTIVE_SCAN)
					return 1;
				break;
			}
		}
	}

	return 0;
}


static void ieee80211_sta_scan_timer(unsigned long ptr)
{
	struct net_device *dev = (struct net_device *) ptr;
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_hw_modes *mode;
	struct ieee80211_channel *chan;
	int skip;
	union iwreq_data wrqu;

	if (!local->sta_scanning)
		return;

	switch (local->scan_state) {
	case SCAN_SET_CHANNEL:
		mode = &local->hw->modes[local->scan_hw_mode_idx];
		if (local->scan_hw_mode_idx >= local->hw->num_modes ||
		    (local->scan_hw_mode_idx + 1 == local->hw->num_modes &&
		     local->scan_channel_idx >= mode->num_channels)) {
			if (ieee80211_sta_restore_oper_chan(dev)) {
				printk(KERN_DEBUG "%s: failed to restore "
				       "operational channel after scan\n",
				       dev->name);
			}
			printk(KERN_DEBUG "%s: scan completed\n", dev->name);
			local->sta_scanning = 0;
			local->last_scan_completed = jiffies;
			memset(&wrqu, 0, sizeof(wrqu));
			wireless_send_event(dev, SIOCGIWSCAN, &wrqu, NULL);
			if (local->conf.mode == IW_MODE_ADHOC) {
				struct ieee80211_sub_if_data *sdata =
					IEEE80211_DEV_TO_SUB_IF(dev);
				struct ieee80211_if_sta *ifsta = &sdata->u.sta;
				if (!ifsta->bssid_set ||
				    (ifsta->state == IEEE80211_IBSS_JOINED &&
				     !ieee80211_sta_active_ibss(dev)))
					ieee80211_sta_find_ibss(dev, ifsta);
			}
			return;
		}
		skip = !(local->hw_modes & (1 << mode->mode));
		chan = &mode->channels[local->scan_channel_idx];
		if (!(chan->flag & IEEE80211_CHAN_W_SCAN) ||
		    (local->conf.mode == IW_MODE_ADHOC &&
		     !(chan->flag & IEEE80211_CHAN_W_IBSS)) ||
		    (local->hw_modes & (1 << MODE_IEEE80211G) &&
		     mode->mode == MODE_IEEE80211B && local->scan_skip_11b))
			skip = 1;

		if (!skip) {
#if 0
			printk(KERN_DEBUG "%s: scan channel %d (%d MHz)\n",
			       dev->name, chan->chan, chan->freq);
#endif

			local->conf.channel = chan->chan;
			local->conf.channel_val = chan->val;
			local->conf.power_level = chan->power_level;
			local->conf.freq = chan->freq;
			local->conf.phymode = mode->mode;
			local->conf.antenna_max = chan->antenna_max;
			if (ieee80211_hw_config(dev)) {
				printk(KERN_DEBUG "%s: failed to set channel "
				       "%d (%d MHz) for scan\n", dev->name,
				       chan->chan, chan->freq);
				skip = 1;
			}
		}

		local->scan_channel_idx++;
		if (local->scan_channel_idx >=
		    local->hw->modes[local->scan_hw_mode_idx].num_channels) {
			local->scan_hw_mode_idx++;
			local->scan_channel_idx = 0;
		}

		if (skip) {
			local->scan_timer.expires = jiffies;
			break;
		}

		local->scan_timer.expires = jiffies + IEEE80211_PROBE_DELAY;
		local->scan_state = SCAN_SEND_PROBE;
		break;
	case SCAN_SEND_PROBE:
		if (ieee80211_active_scan(local)) {
			ieee80211_send_probe_req(dev, NULL, local->scan_ssid,
						 local->scan_ssid_len);
			local->scan_timer.expires =
				jiffies + IEEE80211_CHANNEL_TIME;
		} else {
			local->scan_timer.expires =
				jiffies + IEEE80211_PASSIVE_CHANNEL_TIME;
		}
		local->scan_state = SCAN_SET_CHANNEL;
		break;
	}

	add_timer(&local->scan_timer);
}


int ieee80211_sta_req_scan(struct net_device *dev, u8 *ssid, size_t ssid_len)
{
	struct ieee80211_local *local = dev->priv;

	if (ssid_len > IEEE80211_MAX_SSID_LEN)
		return -EINVAL;

	/* MLME-SCAN.request (page 118)  page 144 (11.1.3.1)
	 * BSSType: INFRASTRUCTURE, INDEPENDENT, ANY_BSS
	 * BSSID: MACAddress
	 * SSID
	 * ScanType: ACTIVE, PASSIVE
	 * ProbeDelay: delay (in microseconds) to be used prior to transmitting
	 *    a Probe frame during active scanning
	 * ChannelList
	 * MinChannelTime (>= ProbeDelay), in TU
	 * MaxChannelTime: (>= MinChannelTime), in TU
	 */

	 /* MLME-SCAN.confirm
	  * BSSDescriptionSet
	  * ResultCode: SUCCESS, INVALID_PARAMETERS
	 */

	/* TODO: if assoc, move to power save mode for the duration of the
	 * scan */

	if (local->sta_scanning)
		return -EBUSY;

	printk(KERN_DEBUG "%s: starting scan\n", dev->name);

	ieee80211_sta_save_oper_chan(dev);

	local->sta_scanning = 1;
	/* TODO: stop TX queue? */

	if (ssid) {
		local->scan_ssid_len = ssid_len;
		memcpy(local->scan_ssid, ssid, ssid_len);
	} else
		local->scan_ssid_len = 0;
	local->scan_skip_11b = 1; /* FIX: clear this is 11g is not supported */
	local->scan_state = SCAN_SET_CHANNEL;
	local->scan_hw_mode_idx = 0;
	local->scan_channel_idx = 0;
	init_timer(&local->scan_timer);
	local->scan_timer.data = (unsigned long) dev;
	local->scan_timer.function = ieee80211_sta_scan_timer;
	local->scan_timer.expires = jiffies + 1;
	add_timer(&local->scan_timer);

	return 0;
}


static char *
ieee80211_sta_scan_result(struct net_device *dev,
			  struct ieee80211_sta_bss *bss,
			  char *current_ev, char *end_buf)
{
	struct ieee80211_local *local = dev->priv;
	struct iw_event iwe;

	if (time_after(jiffies,
		       bss->last_update + IEEE80211_SCAN_RESULT_EXPIRE))
		return current_ev;

	if (!(local->hw_modes & (1 << bss->hw_mode)))
		return current_ev;

	if (local->scan_flags & IEEE80211_SCAN_WPA_ONLY &&
	    bss->wpa_ie == NULL && bss->rsn_ie == NULL)
		return current_ev;

	if (local->scan_flags & IEEE80211_SCAN_MATCH_SSID &&
	    (local->scan_ssid_len != bss->ssid_len ||
	     memcmp(local->scan_ssid, bss->ssid, bss->ssid_len) != 0))
		return current_ev;

	memset(&iwe, 0, sizeof(iwe));
	iwe.cmd = SIOCGIWAP;
	iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
	memcpy(iwe.u.ap_addr.sa_data, bss->bssid, ETH_ALEN);
	current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe,
					  IW_EV_ADDR_LEN);

	memset(&iwe, 0, sizeof(iwe));
	iwe.cmd = SIOCGIWESSID;
	iwe.u.data.length = bss->ssid_len;
	iwe.u.data.flags = 1;
	current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe,
					  bss->ssid);

	if (bss->capability & (WLAN_CAPABILITY_ESS | WLAN_CAPABILITY_IBSS)) {
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = SIOCGIWMODE;
		if (bss->capability & WLAN_CAPABILITY_ESS)
			iwe.u.mode = IW_MODE_MASTER;
		else
			iwe.u.mode = IW_MODE_ADHOC;
		current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe,
						  IW_EV_UINT_LEN);
	}

	memset(&iwe, 0, sizeof(iwe));
	iwe.cmd = SIOCGIWFREQ;
	iwe.u.freq.m = bss->freq * 100000;
	iwe.u.freq.e = 1;
	current_ev = iwe_stream_add_event(current_ev, end_buf, &iwe,
					  IW_EV_FREQ_LEN);

	memset(&iwe, 0, sizeof(iwe));
	iwe.cmd = SIOCGIWENCODE;
	if (bss->capability & WLAN_CAPABILITY_PRIVACY)
		iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
	else
		iwe.u.data.flags = IW_ENCODE_DISABLED;
	iwe.u.data.length = 0;
	current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe, "");

	if (bss && bss->wpa_ie) {
		char *buf, *p;
		int i;
		buf = kmalloc(30 + bss->wpa_ie_len * 2, GFP_ATOMIC);
		if (buf) {
			p = buf;
			p += sprintf(p, "wpa_ie=");
			for (i = 0; i < bss->wpa_ie_len; i++)
				p+= sprintf(p, "%02x", bss->wpa_ie[i]);
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = IWEVCUSTOM;
			iwe.u.data.length = strlen(buf);
			current_ev = iwe_stream_add_point(current_ev, end_buf,
							  &iwe, buf);
			kfree(buf);
		}
	}

	if (bss && bss->rsn_ie) {
		char *buf, *p;
		int i;
		buf = kmalloc(30 + bss->rsn_ie_len * 2, GFP_ATOMIC);
		if (buf) {
			p = buf;
			p += sprintf(p, "rsn_ie=");
			for (i = 0; i < bss->rsn_ie_len; i++)
				p+= sprintf(p, "%02x", bss->rsn_ie[i]);
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = IWEVCUSTOM;
			iwe.u.data.length = strlen(buf);
			current_ev = iwe_stream_add_point(current_ev, end_buf,
							  &iwe, buf);
			kfree(buf);
		}
	}

	if (bss) {
		char *buf;
		buf = kmalloc(30, GFP_ATOMIC);
		if (buf) {
			memset(&iwe, 0, sizeof(iwe));
			iwe.cmd = IWEVCUSTOM;
			sprintf(buf, "tsf=%016llx", bss->timestamp);
			iwe.u.data.length = strlen(buf);
			current_ev = iwe_stream_add_point(current_ev, end_buf,
							  &iwe, buf);
			kfree(buf);
		}
	}

	do {
		char *buf, *p;
		int i;

		if (!(local->scan_flags & IEEE80211_SCAN_EXTRA_INFO))
			break;

		buf = kmalloc(100, GFP_ATOMIC);
		if (buf == NULL)
			break;

		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = IWEVCUSTOM;
		sprintf(buf, "bcn_int=%d", bss->beacon_int);
		iwe.u.data.length = strlen(buf);
		current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe,
						  buf);

		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = IWEVCUSTOM;
		sprintf(buf, "rssi=%d", bss->rssi);
		iwe.u.data.length = strlen(buf);
		current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe,
						  buf);

		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = IWEVCUSTOM;
		sprintf(buf, "capab=0x%04x", bss->capability);
		iwe.u.data.length = strlen(buf);
		current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe,
						  buf);

		p = buf;
		p += sprintf(p, "supp_rates=");
		for (i = 0; i < bss->supp_rates_len; i++)
			p+= sprintf(p, "%02x", bss->supp_rates[i]);
		memset(&iwe, 0, sizeof(iwe));
		iwe.cmd = IWEVCUSTOM;
		iwe.u.data.length = strlen(buf);
		current_ev = iwe_stream_add_point(current_ev, end_buf, &iwe,
						  buf);

		kfree(buf);
		break;
	} while (0);

	return current_ev;
}


int ieee80211_sta_scan_results(struct net_device *dev, char *buf, size_t len)
{
	struct ieee80211_local *local = dev->priv;
	struct list_head *ptr;
	char *current_ev = buf;
	char *end_buf = buf + len;
	struct ieee80211_sta_bss *bss;

	spin_lock_bh(&local->sta_bss_lock);
	list_for_each(ptr, &local->sta_bss_list) {
		bss = list_entry(ptr, struct ieee80211_sta_bss, list);
		current_ev = ieee80211_sta_scan_result(dev, bss, current_ev,
						       end_buf);
	}
	spin_unlock_bh(&local->sta_bss_lock);
	return current_ev - buf;
}


int ieee80211_sta_set_extra_ie(struct net_device *dev, char *ie, size_t len)
{
        struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	struct ieee80211_if_sta *ifsta = &sdata->u.sta;
	kfree(ifsta->extra_ie);
	if (len == 0) {
		ifsta->extra_ie = NULL;
		ifsta->extra_ie_len = 0;
		return 0;
	}
	ifsta->extra_ie = kmalloc(len, GFP_KERNEL);
	if (ifsta->extra_ie == NULL) {
		ifsta->extra_ie_len = 0;
		return -ENOMEM;
	}
	memcpy(ifsta->extra_ie, ie, len);
	ifsta->extra_ie_len = len;
	if (ifsta->bssid_set && ifsta->ssid_set &&
	    ifsta->state != IEEE80211_AUTHENTICATE)
		ieee80211_sta_new_auth(dev, ifsta);
	return 0;
}


struct sta_info * ieee80211_ibss_add_sta(struct net_device *dev,
					 struct sk_buff *skb, u8 *bssid,
					 u8 *addr)
{
	struct ieee80211_local *local = dev->priv;
	struct list_head *ptr;
	struct sta_info *sta;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct net_device *sta_dev = NULL;

	/* TODO: Could consider removing the least recently used entry and
	 * allow new one to be added. */
	if (local->num_sta >= IEEE80211_IBSS_MAX_STA_ENTRIES) {
		if (net_ratelimit()) {
			printk(KERN_DEBUG "%s: No room for a new IBSS STA "
			       "entry " MACSTR "\n", dev->name, MAC2STR(addr));
		}
		return NULL;
	}

	spin_lock_bh(&local->sub_if_lock);
	list_for_each(ptr, &local->sub_if_list) {
		sdata = list_entry(ptr, struct ieee80211_sub_if_data, list);
		if (sdata->type == IEEE80211_SUB_IF_TYPE_STA &&
		    memcmp(bssid, sdata->u.sta.bssid, ETH_ALEN) == 0) {
			sta_dev = sdata->dev;
			break;
		}
	}
	spin_unlock_bh(&local->sub_if_lock);

	if (sta_dev == NULL)
		return NULL;

	printk(KERN_DEBUG "%s: Adding new IBSS station " MACSTR " (dev=%s)\n",
	       dev->name, MAC2STR(addr), sta_dev->name);

	sta = sta_info_add(local, dev, addr);
	if (sta == NULL) {
		return NULL;
	}

	sta->dev = sta_dev;
	sta->supp_rates = sdata->u.sta.supp_rates_bits;

	rate_control_rate_init(local, sta);

	return sta; /* caller will call sta_info_release() */
}


int ieee80211_sta_deauthenticate(struct net_device *dev, u16 reason)
{
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	struct ieee80211_if_sta *ifsta = &sdata->u.sta;

	printk(KERN_DEBUG "%s: deauthenticate(reason=%d)\n",
	       dev->name, reason);

	if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
		return -EINVAL;

	ieee80211_send_deauth(dev, ifsta, reason);
	ieee80211_set_associated(dev, ifsta, 0);
	return 0;
}


int ieee80211_sta_disassociate(struct net_device *dev, u16 reason)
{
	struct ieee80211_sub_if_data *sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	struct ieee80211_if_sta *ifsta = &sdata->u.sta;

	printk(KERN_DEBUG "%s: disassociate(reason=%d)\n",
	       dev->name, reason);

	if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
		return -EINVAL;

	if (!ifsta->associated)
		return -1;

	ieee80211_send_disassoc(dev, ifsta, reason);
	ieee80211_set_associated(dev, ifsta, 0);
	return 0;
}
