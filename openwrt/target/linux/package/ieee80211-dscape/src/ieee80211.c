/*
 * Copyright 2002-2005, Instant802 Networks, Inc.
 * Copyright 2005, Devicescape Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

#include <linux/config.h>
#include <linux/version.h>
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
#include <linux/compiler.h>

#include <net/ieee80211.h>
#include <net/ieee80211_common.h>
#include <net/ieee80211_mgmt.h>
#include "ieee80211_i.h"
#include "ieee80211_proc.h"
#include "rate_control.h"
#include "wep.h"
#include "wpa.h"
#include "tkip.h"
#include "wme.h"


/* See IEEE 802.1H for LLC/SNAP encapsulation/decapsulation */
/* Ethernet-II snap header (RFC1042 for most EtherTypes) */
static unsigned char rfc1042_header[] =
{ 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };
/* Bridge-Tunnel header (for EtherTypes ETH_P_AARP and ETH_P_IPX) */
static unsigned char bridge_tunnel_header[] =
{ 0xaa, 0xaa, 0x03, 0x00, 0x00, 0xf8 };
/* No encapsulation header if EtherType < 0x600 (=length) */

static unsigned char eapol_header[] =
{ 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x88, 0x8e };


struct rate_control_algs {
	struct rate_control_algs *next;
	struct rate_control_ops *ops;
};

static struct rate_control_algs *ieee80211_rate_ctrl_algs;

static int rate_control_initialize(struct ieee80211_local *local);


static u8 * ieee80211_get_bssid(struct ieee80211_hdr *hdr, size_t len);


struct ieee80211_key_conf *
ieee80211_key_data2conf(struct ieee80211_local *local,
			struct ieee80211_key *data)
{
	struct ieee80211_key_conf *conf;

	conf = kmalloc(sizeof(*conf) + data->keylen, GFP_ATOMIC);
	if (conf == NULL)
		return NULL;

	conf->hw_key_idx = data->hw_key_idx;
	conf->alg = data->alg;
	conf->keylen = data->keylen;
	conf->force_sw_encrypt = data->force_sw_encrypt;
	conf->keyidx = data->keyidx;
	conf->default_tx_key = data->default_tx_key;
	conf->default_wep_only = local->default_wep_only;
	memcpy(conf->key, data->key, data->keylen);

	return conf;
}


static int rate_list_match(int *rate_list, int rate)
{
	int i;

	if (rate_list == NULL)
		return 0;

	for (i = 0; rate_list[i] >= 0; i++)
		if (rate_list[i] == rate)
			return 1;

	return 0;
}


void ieee80211_prepare_rates(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	int i;

	for (i = 0; i < local->num_curr_rates; i++) {
		struct ieee80211_rate *rate = &local->curr_rates[i];

		rate->flags &= ~(IEEE80211_RATE_SUPPORTED |
				 IEEE80211_RATE_BASIC);

		if (local->supp_rates[local->conf.phymode]) {
			if (!rate_list_match(local->supp_rates
					     [local->conf.phymode],
					     rate->rate))
				continue;
		}

		rate->flags |= IEEE80211_RATE_SUPPORTED;

		/* Use configured basic rate set if it is available. If not,
		 * use defaults that are sane for most cases. */
		if (local->basic_rates[local->conf.phymode]) {
			if (rate_list_match(local->basic_rates
					    [local->conf.phymode],
					    rate->rate))
				rate->flags |= IEEE80211_RATE_BASIC;
		} else switch (local->conf.phymode) {
		case MODE_IEEE80211A:
			if (rate->rate == 60 || rate->rate == 120 ||
			    rate->rate == 240)
				rate->flags |= IEEE80211_RATE_BASIC;
			break;
		case MODE_IEEE80211B:
			if (rate->rate == 10 || rate->rate == 20)
				rate->flags |= IEEE80211_RATE_BASIC;
			break;
		case MODE_ATHEROS_TURBO:
			if (rate->rate == 120 || rate->rate == 240 ||
			    rate->rate == 480)
				rate->flags |= IEEE80211_RATE_BASIC;
			break;
		case MODE_IEEE80211G:
			if (rate->rate == 10 || rate->rate == 20 ||
			    rate->rate == 55 || rate->rate == 110)
				rate->flags |= IEEE80211_RATE_BASIC;
			break;
		}

		/* Set ERP and MANDATORY flags based on phymode */
		switch (local->conf.phymode) {
		case MODE_IEEE80211A:
			if (rate->rate == 60 || rate->rate == 120 ||
			    rate->rate == 240)
				rate->flags |= IEEE80211_RATE_MANDATORY;
			break;
		case MODE_IEEE80211B:
			if (rate->rate == 10)
				rate->flags |= IEEE80211_RATE_MANDATORY;
			break;
		case MODE_ATHEROS_TURBO:
			break;
		case MODE_IEEE80211G:
			if (rate->rate == 10 || rate->rate == 20 ||
			    rate->rate == 55 || rate->rate == 110 ||
			    rate->rate == 60 || rate->rate == 120 ||
			    rate->rate == 240)
				rate->flags |= IEEE80211_RATE_MANDATORY;
			if (rate->rate != 10 && rate->rate != 20 &&
			    rate->rate != 55 && rate->rate != 110)
				rate->flags |= IEEE80211_RATE_ERP;
			break;
		}
	}
}


static void ieee80211_key_threshold_notify(struct net_device *dev,
					   struct ieee80211_key *key,
					   struct sta_info *sta)
{
	struct sk_buff *skb;
	struct ieee80211_msg_key_notification *msg;

	skb = dev_alloc_skb(sizeof(struct ieee80211_frame_info) +
			    sizeof(struct ieee80211_msg_key_notification));
	if (skb == NULL)
		return;

	skb_reserve(skb, sizeof(struct ieee80211_frame_info));
	msg = (struct ieee80211_msg_key_notification *)
		skb_put(skb, sizeof(struct ieee80211_msg_key_notification));
	msg->tx_rx_count = key->tx_rx_count;
	memcpy(msg->ifname, dev->name, IFNAMSIZ);
	if (sta)
		memcpy(msg->addr, sta->addr, ETH_ALEN);
	else
		memset(msg->addr, 0xff, ETH_ALEN);

	key->tx_rx_count = 0;

	ieee80211_rx_mgmt(dev, skb, 0,
			  ieee80211_msg_key_threshold_notification);
}


int ieee80211_get_hdrlen(u16 fc)
{
	int hdrlen = 24;

	switch (WLAN_FC_GET_TYPE(fc)) {
	case WLAN_FC_TYPE_DATA:
		if ((fc & WLAN_FC_FROMDS) && (fc & WLAN_FC_TODS))
			hdrlen = 30; /* Addr4 */
		if (WLAN_FC_GET_STYPE(fc) & 0x08)
			hdrlen += 2; /* QoS Control Field */
		break;
	case WLAN_FC_TYPE_CTRL:
		switch (WLAN_FC_GET_STYPE(fc)) {
		case WLAN_FC_STYPE_CTS:
		case WLAN_FC_STYPE_ACK:
			hdrlen = 10;
			break;
		default:
			hdrlen = 16;
			break;
		}
		break;
	}

	return hdrlen;
}


int ieee80211_get_hdrlen_from_skb(struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	int hdrlen;

	if (unlikely(skb->len < 10))
		return 0;
	hdrlen = ieee80211_get_hdrlen(le16_to_cpu(hdr->frame_control));
	if (unlikely(hdrlen > skb->len))
		return 0;
	return hdrlen;
}


#ifdef IEEE80211_VERBOSE_DEBUG_FRAME_DUMP
static void ieee80211_dump_frame(const char *ifname, const char *title,
				 struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	u16 fc;
	int hdrlen;

	printk(KERN_DEBUG "%s: %s (len=%d)", ifname, title, skb->len);
	if (skb->len < 4) {
		printk("\n");
		return;
	}

	fc = le16_to_cpu(hdr->frame_control);
	hdrlen = ieee80211_get_hdrlen(fc);
	if (hdrlen > skb->len)
		hdrlen = skb->len;
	if (hdrlen >= 4)
		printk(" FC=0x%04x DUR=0x%04x",
		       fc, le16_to_cpu(hdr->duration_id));
	if (hdrlen >= 10)
		printk(" A1=" MACSTR, MAC2STR(hdr->addr1));
	if (hdrlen >= 16)
		printk(" A2=" MACSTR, MAC2STR(hdr->addr2));
	if (hdrlen >= 24)
		printk(" A3=" MACSTR, MAC2STR(hdr->addr3));
	if (hdrlen >= 30)
		printk(" A4=" MACSTR, MAC2STR(hdr->addr4));
	printk("\n");
}
#else /* IEEE80211_VERBOSE_DEBUG_FRAME_DUMP */
static inline void ieee80211_dump_frame(const char *ifname, const char *title,
					struct sk_buff *skb)
{
}
#endif /* IEEE80211_VERBOSE_DEBUG_FRAME_DUMP */


static int ieee80211_is_eapol(struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr;
	u16 fc;
	int hdrlen;

	if (unlikely(skb->len < 10))
		return 0;

	hdr = (struct ieee80211_hdr *) skb->data;
	fc = le16_to_cpu(hdr->frame_control);

	if (unlikely(!WLAN_FC_DATA_PRESENT(fc)))
		return 0;

	hdrlen = ieee80211_get_hdrlen(fc);

	if (unlikely(skb->len >= hdrlen + sizeof(eapol_header) &&
		     memcmp(skb->data + hdrlen, eapol_header,
			    sizeof(eapol_header)) == 0))
		return 1;

	return 0;
}


static ieee80211_txrx_result
ieee80211_tx_h_rate_ctrl(struct ieee80211_txrx_data *tx)
{
	struct rate_control_extra extra;

	memset(&extra, 0, sizeof(extra));
	extra.mgmt_data = tx->sdata &&
		tx->sdata->type == IEEE80211_SUB_IF_TYPE_MGMT;
	extra.ethertype = tx->ethertype;
	extra.startidx  = 0;
	extra.endidx    = tx->local->num_curr_rates;


        tx->u.tx.rate = rate_control_get_rate(tx->dev, tx->skb, &extra);
	if (unlikely(extra.probe != NULL)) {
		tx->u.tx.control->rate_ctrl_probe = 1;
		tx->u.tx.probe_last_frag = 1;
//		tx->u.tx.control->alt_retry_rate = tx->u.tx.rate->val;
		tx->u.tx.rate = extra.probe;
	} else {
//		tx->u.tx.control->alt_retry_rate = -1;
	}
	if (!tx->u.tx.rate)
		return TXRX_DROP;
	if (tx->local->conf.phymode == MODE_IEEE80211G &&
	    tx->local->cts_protect_erp_frames && tx->fragmented &&
	    extra.nonerp) {
		tx->u.tx.last_frag_rate = tx->u.tx.rate;
		tx->u.tx.last_frag_rateidx = extra.rateidx;
		tx->u.tx.probe_last_frag = extra.probe ? 1 : 0;

		tx->u.tx.rate = extra.nonerp;
//		tx->u.tx.control->rateidx = extra.nonerp_idx;
		tx->u.tx.control->rate_ctrl_probe = 0;
	} else {
		tx->u.tx.last_frag_rate = tx->u.tx.rate;
		tx->u.tx.last_frag_rateidx = extra.rateidx;
//		tx->u.tx.control->rateidx = extra.rateidx;
	}
	tx->u.tx.control->tx_rate = tx->u.tx.rate->val;
	if ((tx->u.tx.rate->flags & IEEE80211_RATE_PREAMBLE2) &&
	    tx->local->short_preamble &&
	    (!tx->sta || (tx->sta->flags & WLAN_STA_SHORT_PREAMBLE))) {
		tx->u.tx.short_preamble = 1;
		tx->u.tx.control->tx_rate = tx->u.tx.rate->val2;
	}

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_tx_h_select_key(struct ieee80211_txrx_data *tx)
{
	if (tx->sta)
		tx->u.tx.control->key_idx = tx->sta->key_idx_compression;
	else
		tx->u.tx.control->key_idx = HW_KEY_IDX_INVALID;

	if (unlikely(tx->u.tx.control->do_not_encrypt))
		tx->key = NULL;
	else if (tx->sta && tx->sta->key)
		tx->key = tx->sta->key;
	else if (tx->sdata->default_key)
		tx->key = tx->sdata->default_key;
	else if (tx->sdata->drop_unencrypted && !(tx->sdata->eapol && ieee80211_is_eapol(tx->skb))) {
		I802_DEBUG_INC(tx->local->tx_handlers_drop_unencrypted);
		return TXRX_DROP;
	} else
		tx->key = NULL;

	if (tx->key) {
		tx->key->tx_rx_count++;
		if (unlikely(tx->local->key_tx_rx_threshold &&
			     tx->key->tx_rx_count >
			     tx->local->key_tx_rx_threshold)) {
			ieee80211_key_threshold_notify(tx->dev, tx->key,
						       tx->sta);
		}
	}

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_tx_h_fragment(struct ieee80211_txrx_data *tx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) tx->skb->data;
	size_t hdrlen, per_fragm, num_fragm, payload_len, left;
	struct sk_buff **frags, *first, *frag;
	int i;
        u8 *pos;
	int frag_threshold = tx->local->fragmentation_threshold;

	if (!tx->fragmented)
		return TXRX_CONTINUE;


	first = tx->skb;

	hdrlen = ieee80211_get_hdrlen(tx->fc);
	payload_len = first->len - hdrlen;
	per_fragm = frag_threshold - hdrlen - 4 /* FCS */;
	num_fragm = (payload_len + per_fragm - 1) / per_fragm;

	frags = (struct sk_buff **)
		kmalloc(num_fragm * sizeof(struct sk_buff *), GFP_ATOMIC);
	if (frags == NULL)
		goto fail;
	memset(frags, 0, num_fragm * sizeof(struct sk_buff *));

	hdr->frame_control |= cpu_to_le16(WLAN_FC_MOREFRAG);
	pos = first->data + hdrlen + per_fragm;
	left = payload_len - per_fragm;
	for (i = 0; i < num_fragm - 1; i++) {
		struct ieee80211_hdr *fhdr;
		size_t copylen;

		if (left <= 0)
			goto fail;

		/* reserve enough extra head and tail room for possible
		 * encryption */
#define IEEE80211_ENCRYPT_HEADROOM 8
#define IEEE80211_ENCRYPT_TAILROOM 12
		frag = frags[i] =
			dev_alloc_skb(frag_threshold +
				      IEEE80211_ENCRYPT_HEADROOM +
				      IEEE80211_ENCRYPT_TAILROOM);
		if (!frag)
			goto fail;
		/* Make sure that all fragments use the same priority so
		 * that they end up using the same TX queue */
		frag->priority = first->priority;
		skb_reserve(frag, IEEE80211_ENCRYPT_HEADROOM);
		fhdr = (struct ieee80211_hdr *) skb_put(frag, hdrlen);
		memcpy(fhdr, first->data, hdrlen);
		if (i == num_fragm - 2)
			fhdr->frame_control &= cpu_to_le16(~WLAN_FC_MOREFRAG);
		fhdr->seq_ctrl = cpu_to_le16(i + 1);
		copylen = left > per_fragm ? per_fragm : left;
		memcpy(skb_put(frag, copylen), pos, copylen);

		pos += copylen;
		left -= copylen;
	}
	skb_trim(first, hdrlen + per_fragm);

	tx->u.tx.num_extra_frag = num_fragm - 1;
	tx->u.tx.extra_frag = frags;

	return TXRX_CONTINUE;

 fail:
	printk(KERN_DEBUG "%s: failed to fragment frame\n", tx->dev->name);
	if (frags) {
		for (i = 0; i < num_fragm - 1; i++)
			if (frags[i])
				dev_kfree_skb(frags[i]);
		kfree(frags);
	}
	I802_DEBUG_INC(tx->local->tx_handlers_drop_fragment);
	return TXRX_DROP;
}


static int wep_encrypt_skb(struct ieee80211_txrx_data *tx, struct sk_buff *skb)
{
	if (tx->key->force_sw_encrypt || tx->local->conf.sw_encrypt) {
		if (ieee80211_wep_encrypt(tx->local, skb, tx->key))
			return -1;
	} else {
		tx->u.tx.control->key_idx = tx->key->hw_key_idx;
		if (tx->local->hw->wep_include_iv) {
			if (ieee80211_wep_add_iv(tx->local, skb, tx->key) ==
			    NULL)
				return -1;
		}
        }
	return 0;
}


void ieee80211_tx_set_iswep(struct ieee80211_txrx_data *tx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) tx->skb->data;

	hdr->frame_control |= cpu_to_le16(WLAN_FC_ISWEP);
	if (tx->u.tx.extra_frag) {
		struct ieee80211_hdr *fhdr;
		int i;
		for (i = 0; i < tx->u.tx.num_extra_frag; i++) {
			fhdr = (struct ieee80211_hdr *)
				tx->u.tx.extra_frag[i]->data;
			fhdr->frame_control |= cpu_to_le16(WLAN_FC_ISWEP);
		}
	}
}


static ieee80211_txrx_result
ieee80211_tx_h_wep_encrypt(struct ieee80211_txrx_data *tx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) tx->skb->data;
	u16 fc;

        fc = le16_to_cpu(hdr->frame_control);

	if (!tx->key || tx->key->alg != ALG_WEP ||
	    (WLAN_FC_GET_TYPE(fc) != WLAN_FC_TYPE_DATA &&
	     (WLAN_FC_GET_TYPE(fc) != WLAN_FC_TYPE_MGMT ||
	      WLAN_FC_GET_STYPE(fc) != WLAN_FC_STYPE_AUTH)))
		return TXRX_CONTINUE;

	tx->u.tx.control->iv_len = WEP_IV_LEN;
	tx->u.tx.control->icv_len = WEP_ICV_LEN;
	ieee80211_tx_set_iswep(tx);

	if (wep_encrypt_skb(tx, tx->skb) < 0) {
		I802_DEBUG_INC(tx->local->tx_handlers_drop_wep);
		return TXRX_DROP;
	}

	if (tx->u.tx.extra_frag) {
		int i;
		for (i = 0; i < tx->u.tx.num_extra_frag; i++) {
                        if (wep_encrypt_skb(tx, tx->u.tx.extra_frag[i]) < 0) {
				I802_DEBUG_INC(tx->local->
					       tx_handlers_drop_wep);
                                return TXRX_DROP;
			}
		}
	}

	return TXRX_CONTINUE;
}


static inline int ceiling_div(int dividend, int divisor)
{
	return ((dividend + divisor - 1) / divisor);
}


static int ieee80211_frame_duration(struct ieee80211_local *local, size_t len,
				    int rate, int erp, int short_preamble)
{
	int dur;

	/* calculate duration (in microseconds, rounded up to next higher
	 * integer if it includes a fractional microsecond) to send frame of
	 * len bytes (does not include FCS) at the given rate. Duration will
	 * also include SIFS.
	 *
	 * rate is in 100 kbps, so divident is multiplied by 10 in the
	 * ceiling_div() operations.
	 */

	if (local->conf.phymode == MODE_IEEE80211A || erp ||
	    local->conf.phymode == MODE_ATHEROS_TURBO) {
		/*
		 * OFDM:
		 *
		 * N_DBPS = DATARATE x 4
		 * N_SYM = Ceiling((16+8xLENGTH+6) / N_DBPS)
		 *	(16 = SIGNAL time, 6 = tail bits)
		 * TXTIME = T_PREAMBLE + T_SIGNAL + T_SYM x N_SYM + Signal Ext
		 *
		 * T_SYM = 4 usec
		 * 802.11a - 17.5.2: aSIFSTime = 16 usec
		 * 802.11g - 19.8.4: aSIFSTime = 10 usec +
		 *	signal ext = 6 usec
		 */
		/* FIX: Atheros Turbo may have different (shorter) duration? */
		dur = 16; /* SIFS + signal ext */
		dur += 16; /* 17.3.2.3: T_PREAMBLE = 16 usec */
		dur += 4; /* 17.3.2.3: T_SIGNAL = 4 usec */
		dur += 4 * ceiling_div((16 + 8 * (len + 4) + 6) * 10,
				       4 * rate); /* T_SYM x N_SYM */
	} else {
		/*
		 * 802.11b or 802.11g with 802.11b compatibility:
		 * 18.3.4: TXTIME = PreambleLength + PLCPHeaderTime +
		 * Ceiling(((LENGTH+PBCC)x8)/DATARATE). PBCC=0.
		 *
		 * 802.11 (DS): 15.3.3, 802.11b: 18.3.4
		 * aSIFSTime = 10 usec
		 * aPreambleLength = 144 usec or 72 usec with short preamble
		 * aPLCPHeaderLength = 48 ms or 24 ms with short preamble
		 */
		dur = 10; /* aSIFSTime = 10 usec */
		dur += short_preamble ? (72 + 24) : (144 + 48);

		dur += ceiling_div(8 * (len + 4) * 10, rate);
	}

	return dur;
}


static u16 ieee80211_duration(struct ieee80211_txrx_data *tx, int group_addr,
			      int next_frag_len)
{
	int rate, mrate, erp, dur, i;
	struct ieee80211_rate *txrate = tx->u.tx.rate;
	struct ieee80211_local *local = tx->local;

	erp = txrate->flags & IEEE80211_RATE_ERP;

	/*
	 * data and mgmt (except PS Poll):
	 * - during CFP: 32768
	 * - during contention period:
	 *   if addr1 is group address: 0
	 *   if more fragments = 0 and addr1 is individual address: time to
	 *      transmit one ACK plus SIFS
	 *   if more fragments = 1 and addr1 is individual address: time to
	 *      transmit next fragment plus 2 x ACK plus 3 x SIFS
	 *
	 * IEEE 802.11, 9.6:
	 * - control response frame (CTS or ACK) shall be transmitted using the
	 *   same rate as the immediately previous frame in the frame exchange
	 *   sequence, if this rate belongs to the PHY mandatory rates, or else
	 *   at the highest possible rate belonging to the PHY rates in the
	 *   BSSBasicRateSet
	 */

	if (WLAN_FC_GET_TYPE(tx->fc) == WLAN_FC_TYPE_CTRL) {
		/* TODO: These control frames are not currently sent by
		 * 80211.o, but should they be implemented, this function
		 * needs to be updated to support duration field calculation.
		 *
		 * RTS: time needed to transmit pending data/mgmt frame plus
		 *    one CTS frame plus one ACK frame plus 3 x SIFS
		 * CTS: duration of immediately previous RTS minus time
		 *    required to transmit CTS and its SIFS
		 * ACK: 0 if immediately previous directed data/mgmt had
		 *    more=0, with more=1 duration in ACK frame is duration
		 *    from previous frame minus time needed to transmit ACK
		 *    and its SIFS
		 * PS Poll: BIT(15) | BIT(14) | aid
		 */
		return 0;
	}

	/* data/mgmt */
	if (0 /* FIX: data/mgmt during CFP */)
		return 32768;

	if (group_addr) /* Group address as the destination - no ACK */
		return 0;

	/* Individual destination address:
	 * IEEE 802.11, Ch. 9.6 (after IEEE 802.11g changes)
	 * CTS and ACK frames shall be transmitted using the highest rate in
	 * basic rate set that is less than or equal to the rate of the
	 * immediately previous frame and that is using the same modulation
	 * (CCK or OFDM). If no basic rate set matches with these requirements,
	 * the highest mandatory rate of the PHY that is less than or equal to
	 * the rate of the previous frame is used.
	 * Mandatory rates for IEEE 802.11g PHY: 1, 2, 5.5, 11, 6, 12, 24 Mbps
	 */
	rate = -1;
	mrate = 10; /* use 1 Mbps if everything fails */
	for (i = 0; i < local->num_curr_rates; i++) {
		struct ieee80211_rate *r = &local->curr_rates[i];
		if (r->rate > txrate->rate)
			break;

		if (IEEE80211_RATE_MODULATION(txrate->flags) !=
		    IEEE80211_RATE_MODULATION(r->flags))
			continue;

		if (r->flags & IEEE80211_RATE_BASIC)
			rate = r->rate;
		else if (r->flags & IEEE80211_RATE_MANDATORY)
			mrate = r->rate;
	}
	if (rate == -1) {
		/* No matching basic rate found; use highest suitable mandatory
		 * PHY rate */
		rate = mrate;
	}

	/* Time needed to transmit ACK
	 * (10 bytes + 4-byte FCS = 112 bits) plus SIFS; rounded up
	 * to closest integer */

	dur = ieee80211_frame_duration(local, 10, rate, erp,
                                       local->short_preamble);

	if (next_frag_len) {
		/* Frame is fragmented: duration increases with time needed to
		 * transmit next fragment plus ACK and 2 x SIFS. */
		dur *= 2; /* ACK + SIFS */
		/* next fragment */
		dur += ieee80211_frame_duration(local, next_frag_len,
						txrate->rate, erp,
						local->short_preamble);
	}

        return dur;
}


static ieee80211_txrx_result
ieee80211_tx_h_misc(struct ieee80211_txrx_data *tx)
{
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) tx->skb->data;
	u16 dur;
	struct ieee80211_tx_control *control = tx->u.tx.control;

	if (!MULTICAST_ADDR(hdr->addr1)) {
		if (tx->skb->len >= tx->local->rts_threshold &&
		    tx->local->rts_threshold < IEEE80211_MAX_RTS_THRESHOLD) {
			control->use_rts_cts = 1;
			control->retry_limit =
				tx->local->long_retry_limit;
		} else {
			control->retry_limit =
				tx->local->short_retry_limit;
		}
	} else {
		control->retry_limit = 1;
	}

	if (tx->fragmented) {
		/* Do not use multiple retry rates when sending fragmented
		 * frames.
		 * TODO: The last fragment could still use multiple retry
		 * rates. */
//		control->alt_retry_rate = -1;
	}

	/* Use CTS protection for unicast frames sent using extended rates if
	 * there are associated non-ERP stations and RTS/CTS is not configured
	 * for the frame. */
	if (tx->local->conf.phymode == MODE_IEEE80211G &&
	    (tx->u.tx.rate->flags & IEEE80211_RATE_ERP) &&
	    tx->u.tx.unicast &&
	    tx->local->cts_protect_erp_frames &&
	    !control->use_rts_cts)
		control->use_cts_protect = 1;


	/* Setup duration field for the first fragment of the frame. Duration
	 * for remaining fragments will be updated when they are being sent
	 * to low-level driver in ieee80211_tx(). */
	dur = ieee80211_duration(tx, MULTICAST_ADDR(hdr->addr1),
				 tx->fragmented ? tx->u.tx.extra_frag[0]->len :
				 0);
	hdr->duration_id = cpu_to_le16(dur);

	if (control->use_rts_cts || control->use_cts_protect) {
		struct ieee80211_rate *rate;
		int erp = tx->u.tx.rate->flags & IEEE80211_RATE_ERP;

		/* Do not use multiple retry rates when using RTS/CTS */
//		control->alt_retry_rate = -1;

		/* Use min(data rate, max base rate) as CTS/RTS rate */
		rate = tx->u.tx.rate;
		while (rate > tx->local->curr_rates &&
		       !(rate->flags & IEEE80211_RATE_BASIC))
			rate--;


		if (control->use_rts_cts)
			dur += ieee80211_frame_duration(tx->local, 10,
							rate->rate, erp,
							tx->local->
							short_preamble);
		dur += ieee80211_frame_duration(tx->local, tx->skb->len,
						tx->u.tx.rate->rate, erp,
						tx->u.tx.short_preamble);
		control->rts_cts_duration = dur;
		control->rts_cts_rate = rate->val;
	}

	if (tx->sta) {
		tx->sta->tx_packets++;
		tx->sta->tx_fragments++;
		tx->sta->tx_bytes += tx->skb->len;
		if (tx->u.tx.extra_frag) {
			int i;
			tx->sta->tx_fragments += tx->u.tx.num_extra_frag;
			for (i = 0; i < tx->u.tx.num_extra_frag; i++) {
				tx->sta->tx_bytes +=
					tx->u.tx.extra_frag[i]->len;
			}
		}
        }
	tx->local->scan.txrx_count++;

	return TXRX_CONTINUE;
}


static void ieee80211_rate_limit(unsigned long data)
{
	struct ieee80211_local *local = (struct ieee80211_local *) data;

	if (local->rate_limit) {
                local->rate_limit_bucket += local->rate_limit;
                if (local->rate_limit_bucket > local->rate_limit_burst)
                        local->rate_limit_bucket = local->rate_limit_burst;
		local->rate_limit_timer.expires = jiffies + HZ;
		add_timer(&local->rate_limit_timer);
        }

}

static ieee80211_txrx_result
ieee80211_tx_h_rate_limit(struct ieee80211_txrx_data *tx)
{

	if (likely(!tx->local->rate_limit || tx->u.tx.unicast))
                return TXRX_CONTINUE;

	/* rate limit */
        if (tx->local->rate_limit_bucket) {
                tx->local->rate_limit_bucket--;
                return TXRX_CONTINUE;
        }

	I802_DEBUG_INC(tx->local->tx_handlers_drop_rate_limit);
	return TXRX_DROP;
}



static ieee80211_txrx_result
ieee80211_tx_h_check_assoc(struct ieee80211_txrx_data *tx)
{
#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
	struct sk_buff *skb = tx->skb;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */
	u32 sta_flags;

	if (unlikely(tx->local->sta_scanning != 0) &&
	    (WLAN_FC_GET_TYPE(tx->fc) != WLAN_FC_TYPE_MGMT ||
	     WLAN_FC_GET_STYPE(tx->fc) != WLAN_FC_STYPE_PROBE_REQ))
		return TXRX_DROP;

	if (tx->u.tx.ps_buffered)
		return TXRX_CONTINUE;

	sta_flags = tx->sta ? tx->sta->flags : 0;

	if (likely(tx->u.tx.unicast)) {
		if (unlikely(!(sta_flags & WLAN_STA_ASSOC) &&
			     tx->local->conf.mode != IW_MODE_ADHOC &&
			     WLAN_FC_GET_TYPE(tx->fc) == WLAN_FC_TYPE_DATA)) {
#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
			printk(KERN_DEBUG "%s: dropped data frame to not "
			       "associated station " MACSTR "\n",
			       tx->dev->name, MAC2STR(hdr->addr1));
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */
			I802_DEBUG_INC(tx->local->tx_handlers_drop_not_assoc);
			return TXRX_DROP;
		}
	} else {
		if (unlikely(WLAN_FC_GET_TYPE(tx->fc) == WLAN_FC_TYPE_DATA &&
			     tx->local->num_sta == 0 &&
			     !tx->local->allow_broadcast_always &&
			     tx->local->conf.mode != IW_MODE_ADHOC)) {
			/*
			 * No associated STAs - no need to send multicast
			 * frames.
			 */
			return TXRX_DROP;
		}
		return TXRX_CONTINUE;
	}

	if (unlikely(!tx->u.tx.mgmt_interface && tx->sdata->ieee802_1x &&
		     !(sta_flags & WLAN_STA_AUTHORIZED))) {
#ifdef CONFIG_IEEE80211_DEBUG
		struct ieee80211_hdr *hdr =
			(struct ieee80211_hdr *) tx->skb->data;
		printk(KERN_DEBUG "%s: dropped frame to " MACSTR
		       " (unauthorized port)\n", tx->dev->name,
		       MAC2STR(hdr->addr1));
#endif
		I802_DEBUG_INC(tx->local->tx_handlers_drop_unauth_port);
		return TXRX_DROP;
	}

	return TXRX_CONTINUE;
}


/* This function is called whenever the AP is about to exceed the maximum limit
 * of buffered frames for power saving STAs. This situation should not really
 * happen often during normal operation, so dropping the oldest buffered packet
 * from each queue should be OK to make some room for new frames. */
static void purge_old_ps_buffers(struct ieee80211_local *local)
{
	int total = 0, purged = 0;
	struct sk_buff *skb;
	struct list_head *ptr;

	spin_lock_bh(&local->sub_if_lock);
	list_for_each(ptr, &local->sub_if_list)	{
		struct ieee80211_if_norm *norm;
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);
		if (sdata->dev == local->mdev ||
		    sdata->type != IEEE80211_SUB_IF_TYPE_NORM)
			continue;
		norm = &sdata->u.norm;
		skb = skb_dequeue(&norm->ps_bc_buf);
		if (skb) {
			purged++;
			dev_kfree_skb(skb);
		}
		total += skb_queue_len(&norm->ps_bc_buf);
	}
	spin_unlock_bh(&local->sub_if_lock);

	spin_lock_bh(&local->sta_lock);
	list_for_each(ptr, &local->sta_list) {
		struct sta_info *sta =
			list_entry(ptr, struct sta_info, list);
		skb = skb_dequeue(&sta->ps_tx_buf);
		if (skb) {
			purged++;
			dev_kfree_skb(skb);
		}
		total += skb_queue_len(&sta->ps_tx_buf);
	}
	spin_unlock_bh(&local->sta_lock);

	local->total_ps_buffered = total;
	printk(KERN_DEBUG "%s: PS buffers full - purged %d frames\n",
	       local->mdev->name, purged);
}


static inline ieee80211_txrx_result
ieee80211_tx_h_multicast_ps_buf(struct ieee80211_txrx_data *tx)
{
	/* broadcast/multicast frame */
	/* If any of the associated stations is in power save mode,
	 * the frame is buffered to be sent after DTIM beacon frame */
	if (tx->local->hw->host_broadcast_ps_buffering &&
	    tx->sdata->type != IEEE80211_SUB_IF_TYPE_WDS &&
	    tx->sdata->bss && atomic_read(&tx->sdata->bss->num_sta_ps) &&
	    !(tx->fc & WLAN_FC_ORDER)) {
		if (tx->local->total_ps_buffered >= TOTAL_MAX_TX_BUFFER)
			purge_old_ps_buffers(tx->local);
		if (skb_queue_len(&tx->sdata->bss->ps_bc_buf) >=
		    AP_MAX_BC_BUFFER) {
			if (net_ratelimit()) {
				printk(KERN_DEBUG "%s: BC TX buffer full - "
				       "dropping the oldest frame\n",
				       tx->dev->name);
			}
			dev_kfree_skb(skb_dequeue(&tx->sdata->bss->ps_bc_buf));
		} else
			tx->local->total_ps_buffered++;
                skb_queue_tail(&tx->sdata->bss->ps_bc_buf, tx->skb);
		return TXRX_QUEUED;
        }

	return TXRX_CONTINUE;
}


static inline ieee80211_txrx_result
ieee80211_tx_h_unicast_ps_buf(struct ieee80211_txrx_data *tx)
{
	struct sta_info *sta = tx->sta;

	if (unlikely(!sta ||
		     (WLAN_FC_GET_TYPE(tx->fc) == WLAN_FC_TYPE_MGMT &&
		      WLAN_FC_GET_STYPE(tx->fc) == WLAN_FC_STYPE_PROBE_RESP)))
		return TXRX_CONTINUE;

	if (unlikely((sta->flags & WLAN_STA_PS) && !sta->pspoll)) {
		struct ieee80211_tx_packet_data *pkt_data;
#ifdef IEEE80211_VERBOSE_DEBUG_PS
		printk(KERN_DEBUG "STA " MACSTR " aid %d: PS buffer (entries "
		       "before %d)\n",
		       MAC2STR(sta->addr), sta->aid,
		       skb_queue_len(&sta->ps_tx_buf));
#endif /* IEEE80211_VERBOSE_DEBUG_PS */
		sta->flags |= WLAN_STA_TIM;
		if (tx->local->total_ps_buffered >= TOTAL_MAX_TX_BUFFER)
			purge_old_ps_buffers(tx->local);
		if (skb_queue_len(&sta->ps_tx_buf) >= STA_MAX_TX_BUFFER) {
			struct sk_buff *old = skb_dequeue(&sta->ps_tx_buf);
			if (net_ratelimit()) {
				printk(KERN_DEBUG "%s: STA " MACSTR " TX "
				       "buffer full - dropping oldest frame\n",
				       tx->dev->name, MAC2STR(sta->addr));
			}
			dev_kfree_skb(old);
		} else
			tx->local->total_ps_buffered++;
		/* Queue frame to be sent after STA sends an PS Poll frame */
		if (skb_queue_empty(&sta->ps_tx_buf) && tx->local->hw->set_tim)
			tx->local->hw->set_tim(tx->dev, sta->aid, 1);
		pkt_data = (struct ieee80211_tx_packet_data *)tx->skb->cb;
		pkt_data->jiffies = jiffies;
                skb_queue_tail(&sta->ps_tx_buf, tx->skb);
		return TXRX_QUEUED;
	}
#ifdef IEEE80211_VERBOSE_DEBUG_PS
	else if (unlikely(sta->flags & WLAN_STA_PS)) {
		printk(KERN_DEBUG "%s: STA " MACSTR " in PS mode, but pspoll "
		       "set -> send frame\n", tx->dev->name,
		       MAC2STR(sta->addr));
	}
#endif /* IEEE80211_VERBOSE_DEBUG_PS */
	sta->pspoll = 0;

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_tx_h_ps_buf(struct ieee80211_txrx_data *tx)
{
	if (unlikely(tx->u.tx.ps_buffered))
		return TXRX_CONTINUE;

	if (tx->u.tx.unicast)
		return ieee80211_tx_h_unicast_ps_buf(tx);
	else
		return ieee80211_tx_h_multicast_ps_buf(tx);
}


static void inline ieee80211_tx_prepare(struct ieee80211_txrx_data *tx,
					struct sk_buff *skb,
					struct net_device *dev,
					struct ieee80211_tx_control *control)
{
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
        struct ieee80211_tx_packet_data *pkt_data;
	int hdrlen;

        pkt_data = (struct ieee80211_tx_packet_data *)skb->cb;

	memset(tx, 0, sizeof(*tx));
	tx->skb = skb;
	tx->dev = pkt_data->sdata->dev; /* use original interface */
	tx->local = local;
        tx->sdata = pkt_data->sdata;
	tx->sta = sta_info_get(local, hdr->addr1);
	tx->fc = le16_to_cpu(hdr->frame_control);
        control->power_level = local->conf.power_level;
	tx->u.tx.control = control;
        tx->u.tx.unicast = !MULTICAST_ADDR(hdr->addr1);
        control->no_ack = MULTICAST_ADDR(hdr->addr1);
	tx->fragmented = local->fragmentation_threshold <
		IEEE80211_MAX_FRAG_THRESHOLD && tx->u.tx.unicast &&
		skb->len + 4 /* FCS */ > local->fragmentation_threshold &&
		(local->hw->set_frag_threshold == NULL);
	if (tx->sta == NULL)
		control->clear_dst_mask = 1;
	else if (tx->sta->clear_dst_mask) {
		control->clear_dst_mask = 1;
		tx->sta->clear_dst_mask = 0;
	}
	control->antenna_sel = local->conf.antenna_sel;
	if (local->sta_antenna_sel != STA_ANTENNA_SEL_AUTO && tx->sta)
		control->antenna_sel = tx->sta->antenna_sel;
	hdrlen = ieee80211_get_hdrlen(tx->fc);
	if (skb->len > hdrlen + sizeof(rfc1042_header) + 2) {
		u8 *pos = &skb->data[hdrlen + sizeof(rfc1042_header)];
		tx->ethertype = (pos[0] << 8) | pos[1];
	}

}


static int ieee80211_tx(struct net_device *dev, struct sk_buff *skb,
			struct ieee80211_tx_control *control, int mgmt)
{
	struct ieee80211_local *local = dev->priv;
	struct sta_info *sta;
	ieee80211_tx_handler *handler;
	struct ieee80211_txrx_data tx;
	ieee80211_txrx_result res = TXRX_DROP;
        int ret, i;

	if (unlikely(skb->len < 10)) {
		dev_kfree_skb(skb);
		return 0;
	}

	ieee80211_tx_prepare(&tx, skb, dev, control);
	sta = tx.sta;
	tx.u.tx.mgmt_interface = mgmt;

	for (handler = local->tx_handlers; *handler != NULL; handler++) {
		res = (*handler)(&tx);
		if (res != TXRX_CONTINUE)
			break;
	}

	skb = tx.skb; /* handlers are allowed to change skb */

	if (sta)
		sta_info_release(local, sta);

	if (unlikely(res == TXRX_DROP)) {
		I802_DEBUG_INC(local->tx_handlers_drop);
		goto drop;
	}

	if (unlikely(res == TXRX_QUEUED)) {
		I802_DEBUG_INC(local->tx_handlers_queued);
		return 0;
	}

	ieee80211_dump_frame(dev->name, "TX to low-level driver", skb);
	ret = local->hw->tx(dev, skb, control);
#ifdef IEEE80211_LEDS
	if (!ret && local->tx_led_counter++ == 0) {
                ieee80211_tx_led(1, dev);
        }
#endif /* IEEE80211_LEDS */
	if (tx.u.tx.extra_frag) {
		if (ret > 0) {
			/* Must free all fragments and return 0 since skb data
			 * has been fragmented into multiple buffers.
			 * TODO: could free extra fragments and restore skb to
			 * the original form since the data is still there and
			 * then return nonzero so that Linux netif would
			 * retry. */
			goto drop;
		}

		skb = NULL; /* skb is now owned by low-level driver */
		control->use_rts_cts = 0;
		control->use_cts_protect = 0;
		control->clear_dst_mask = 0;
		for (i = 0; i < tx.u.tx.num_extra_frag; i++) {
			int next_len, dur;
			struct ieee80211_hdr *hdr =
				(struct ieee80211_hdr *)
				tx.u.tx.extra_frag[i]->data;
			if (i + 1 < tx.u.tx.num_extra_frag)
				next_len = tx.u.tx.extra_frag[i + 1]->len;
			else {
				next_len = 0;
				tx.u.tx.rate = tx.u.tx.last_frag_rate;
				tx.u.tx.control->tx_rate = tx.u.tx.rate->val;
//				tx.u.tx.control->rateidx =
//					tx.u.tx.last_frag_rateidx;
				tx.u.tx.control->rate_ctrl_probe =
					tx.u.tx.probe_last_frag;
			}
			dur = ieee80211_duration(&tx, 0, next_len);
			hdr->duration_id = cpu_to_le16(dur);

			ieee80211_dump_frame(dev->name,
					     "TX to low-level driver", skb);
			ret = local->hw->tx(dev, tx.u.tx.extra_frag[i],
					    control);
			if (ret > 0)
				goto drop;
#ifdef IEEE80211_LEDS
			if (local->tx_led_counter++ == 0) {
                                ieee80211_tx_led(1, dev);
                        }
#endif /* IEEE80211_LEDS */
			tx.u.tx.extra_frag[i] = NULL;
		}
		kfree(tx.u.tx.extra_frag);
        }
        if (ret == -1)
		ret = 0;
	return ret;

 drop:
	if (skb)
		dev_kfree_skb(skb);
	for (i = 0; i < tx.u.tx.num_extra_frag; i++)
		if (tx.u.tx.extra_frag[i])
			dev_kfree_skb(tx.u.tx.extra_frag[i]);
        kfree(tx.u.tx.extra_frag);
	return 0;
}


static int ieee80211_master_start_xmit(struct sk_buff *skb,
				       struct net_device *dev)
{
	struct ieee80211_tx_control control;
	struct ieee80211_tx_packet_data *pkt_data;
	struct ieee80211_sub_if_data *sdata;
	int ret = 1;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	/*
	 * copy control out of the skb so other people can use skb->cb
	 */
	pkt_data = (struct ieee80211_tx_packet_data *)skb->cb;
	if (unlikely(pkt_data->magic != IEEE80211_CB_MAGIC)) {
		printk(KERN_WARNING "%s: Someone messed with our skb->cb\n",
		       dev->name);
		dev_kfree_skb(skb);
		return 0;
	}
	memcpy(&control, &pkt_data->control,
	       sizeof(struct ieee80211_tx_control));

	ret = ieee80211_tx(dev, skb, &control,
			   pkt_data->sdata->type ==
			   IEEE80211_SUB_IF_TYPE_MGMT);

        return ret;
}


/**
 * ieee80211_subif_start_xmit - netif start_xmit function for Ethernet-type
 * subinterfaces (wlan#, WDS, and VLAN interfaces)
 * @skb: packet to be sent
 * @dev: incoming interface
 *
 * Returns: 0 on success (and frees skb in this case) or 1 on failure (skb will
 * not be freed, and caller is responsible for either retrying later or freeing
 * skb).
 *
 * This function takes in an Ethernet header and encapsulates it with suitable
 * IEEE 802.11 header based on which interface the packet is coming in. The
 * encapsulated packet will then be passed to master interface, wlan#.11, for
 * transmission (through low-level driver).
 */
static int ieee80211_subif_start_xmit(struct sk_buff *skb,
				      struct net_device *dev)
{
        struct ieee80211_local *local = (struct ieee80211_local *) dev->priv;
	struct ieee80211_tx_packet_data *pkt_data;
        struct ieee80211_sub_if_data *sdata;
	int ret = 1, head_need;
	u16 ethertype, hdrlen, fc;
	struct ieee80211_hdr hdr;
	u8 *encaps_data;
        int encaps_len, skip_header_bytes;
	int nh_pos, h_pos, no_encrypt = 0;
	struct sta_info *sta;

        sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	if (unlikely(skb->len < ETH_HLEN)) {
		printk(KERN_DEBUG "%s: short skb (len=%d)\n",
		       dev->name, skb->len);
		ret = 0;
		goto fail;
	}

	nh_pos = skb->nh.raw - skb->data;
	h_pos = skb->h.raw - skb->data;

	/* convert Ethernet header to proper 802.11 header (based on
	 * operation mode) */
	ethertype = (skb->data[12] << 8) | skb->data[13];
	/* TODO: handling for 802.1x authorized/unauthorized port */
	fc = (WLAN_FC_TYPE_DATA << 2) | (WLAN_FC_STYPE_DATA << 4);

        if (likely(sdata->type == IEEE80211_SUB_IF_TYPE_NORM ||
		   sdata->type == IEEE80211_SUB_IF_TYPE_VLAN)) {
		if (local->conf.mode == IW_MODE_MASTER) {
			fc |= WLAN_FC_FROMDS;
			/* DA BSSID SA */
			memcpy(hdr.addr1, skb->data, ETH_ALEN);
			memcpy(hdr.addr2, dev->dev_addr, ETH_ALEN);
			memcpy(hdr.addr3, skb->data + ETH_ALEN, ETH_ALEN);
		} else if (local->conf.mode == IW_MODE_INFRA) {
			fc |= WLAN_FC_TODS;
			/* BSSID SA DA */
			memcpy(hdr.addr1, local->bssid, ETH_ALEN);
			memcpy(hdr.addr2, skb->data + ETH_ALEN, ETH_ALEN);
			memcpy(hdr.addr3, skb->data, ETH_ALEN);
		} else if (local->conf.mode == IW_MODE_ADHOC) {
			/* DA SA BSSID */
			memcpy(hdr.addr1, skb->data, ETH_ALEN);
			memcpy(hdr.addr2, skb->data + ETH_ALEN, ETH_ALEN);
			memcpy(hdr.addr3, local->bssid, ETH_ALEN);
		}
                hdrlen = 24;
        } else if (sdata->type == IEEE80211_SUB_IF_TYPE_WDS) {
                fc |= WLAN_FC_FROMDS | WLAN_FC_TODS;
		/* RA TA DA SA */
                memcpy(hdr.addr1, sdata->u.wds.remote_addr, ETH_ALEN);
                memcpy(hdr.addr2, dev->dev_addr, ETH_ALEN);
                memcpy(hdr.addr3, skb->data, ETH_ALEN);
                memcpy(hdr.addr4, skb->data + ETH_ALEN, ETH_ALEN);
                hdrlen = 30;
        } else if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
		if (local->conf.mode == IW_MODE_INFRA) {
			fc |= WLAN_FC_TODS;
			/* BSSID SA DA */
			memcpy(hdr.addr1, sdata->u.sta.bssid, ETH_ALEN);
			memcpy(hdr.addr2, skb->data + ETH_ALEN, ETH_ALEN);
			memcpy(hdr.addr3, skb->data, ETH_ALEN);
		} else {
			/* DA SA BSSID */
			memcpy(hdr.addr1, skb->data, ETH_ALEN);
			memcpy(hdr.addr2, skb->data + ETH_ALEN, ETH_ALEN);
			memcpy(hdr.addr3, sdata->u.sta.bssid, ETH_ALEN);
		}
		hdrlen = 24;
        } else {
                ret = 0;
                goto fail;
        }
	
	/* receiver is QoS enabled, use a QoS type frame */
	sta = sta_info_get(local, hdr.addr1);
	if (sta) {
		if (sta->flags & WLAN_STA_WME) {
			fc |= WLAN_FC_STYPE_QOS_DATA << 4;
			hdrlen += 2;
		}
		sta_info_release(local, sta);
	}
	
	hdr.frame_control = cpu_to_le16(fc);
	hdr.duration_id = 0;
	hdr.seq_ctrl = 0;

	skip_header_bytes = ETH_HLEN;
	if (ethertype == ETH_P_AARP || ethertype == ETH_P_IPX) {
		encaps_data = bridge_tunnel_header;
		encaps_len = sizeof(bridge_tunnel_header);
		skip_header_bytes -= 2;
	} else if (ethertype >= 0x600) {
		encaps_data = rfc1042_header;
		encaps_len = sizeof(rfc1042_header);
		skip_header_bytes -= 2;
	} else {
		encaps_data = NULL;
		encaps_len = 0;
	}

	skb_pull(skb, skip_header_bytes);
	nh_pos -= skip_header_bytes;
	h_pos -= skip_header_bytes;

	/* TODO: implement support for fragments so that there is no need to
	 * reallocate and copy payload; it might be enough to support one
	 * extra fragment that would be copied in the beginning of the frame
	 * data.. anyway, it would be nice to include this into skb structure
	 * somehow
	 *
	 * There are few options for this:
	 * use skb->cb as an extra space for 802.11 header
	 * allocate new buffer if not enough headroom
	 * make sure that there is enough headroom in every skb by increasing
	 * build in headroom in __dev_alloc_skb() (linux/skbuff.h) and
	 * alloc_skb() (net/core/skbuff.c)
	 */
	head_need = hdrlen + encaps_len + (local->hw->extra_hdr_room ? 2 : 0);
	head_need -= skb_headroom(skb);

	/* We are going to modify skb data, so make a copy of it if happens to
	 * be cloned. This could happen, e.g., with Linux bridge code passing
	 * us broadcast frames. */

	if (head_need > 0 || skb_cloned(skb)) {
#if 0
		printk(KERN_DEBUG "%s: need to reallocate buffer for %d bytes "
		       "of headroom\n", dev->name, head_need);
#endif

		if (skb_cloned(skb))
			I802_DEBUG_INC(local->tx_expand_skb_head_cloned);
		else
			I802_DEBUG_INC(local->tx_expand_skb_head);
		/* Since we have to reallocate the buffer, make sure that there
		 * is enough room for possible WEP IV/ICV and TKIP (8 bytes
		 * before payload and 12 after). */
		if (pskb_expand_head(skb, (head_need > 0 ? head_need + 8 : 8),
				     12, GFP_ATOMIC)) {
			printk(KERN_DEBUG "%s: failed to reallocate TX buffer"
			       "\n", dev->name);
			goto fail;
		}
	}

	if (encaps_data) {
		memcpy(skb_push(skb, encaps_len), encaps_data, encaps_len);
		nh_pos += encaps_len;
		h_pos += encaps_len;
	}
	memcpy(skb_push(skb, hdrlen), &hdr, hdrlen);
	nh_pos += hdrlen;
	h_pos += hdrlen;

	pkt_data = (struct ieee80211_tx_packet_data *)skb->cb;
	memset(pkt_data, 0, sizeof(struct ieee80211_tx_packet_data));
	pkt_data->magic = IEEE80211_CB_MAGIC;
	pkt_data->sdata = sdata;
	pkt_data->control.do_not_encrypt = no_encrypt;

	skb->dev = sdata->master;
	sdata->stats.tx_packets++;
	sdata->stats.tx_bytes += skb->len;

	/* Update skb pointers to various headers since this modified frame
	 * is going to go through Linux networking code that may potentially
	 * need things like pointer to IP header. */
	skb->mac.raw = skb->data;
	skb->nh.raw = skb->data + nh_pos;
	skb->h.raw = skb->data + h_pos;


        dev_queue_xmit(skb);

        return 0;

 fail:
	if (!ret)
                dev_kfree_skb(skb);

	return ret;
}


/*
 * This is the transmit routine for the 802.11 type interfaces
 * called by upper layers of the linux networking
 * stack when it has a frame to transmit
 */
static int
ieee80211_mgmt_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_tx_packet_data *pkt_data;
	struct ieee80211_hdr *hdr;
	u16 fc;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	if (skb->len < 10) {
		dev_kfree_skb(skb);
		return 0;
	}

	hdr = (struct ieee80211_hdr *) skb->data;
	fc = le16_to_cpu(hdr->frame_control);

        pkt_data = (struct ieee80211_tx_packet_data *)skb->cb;
	memset(pkt_data, 0, sizeof(struct ieee80211_tx_packet_data));
	pkt_data->magic = IEEE80211_CB_MAGIC;
        pkt_data->sdata = sdata;

	if (WLAN_FC_GET_TYPE(fc) == WLAN_FC_TYPE_MGMT &&
	    WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_PROBE_RESP)
		pkt_data->control.pkt_type = PKT_PROBE_RESP;

        skb->priority = 20; /* use hardcode priority for mgmt TX queue */
	skb->dev = sdata->master;

	/*
	 * We're using the protocol field of the the frame control header
	 * to request TX callback for hostapd. BIT(1) is checked.
	 */
	if ((fc & BIT(1)) == BIT(1)) {
		pkt_data->control.req_tx_status = 1;
		fc &= ~BIT(1);
		hdr->frame_control = cpu_to_le16(fc);
	}



	pkt_data->control.do_not_encrypt = !(fc & WLAN_FC_ISWEP);

	sdata->stats.tx_packets++;
        sdata->stats.tx_bytes += skb->len;

        dev_queue_xmit(skb);

	return 0;
}


static void ieee80211_beacon_add_tim(struct ieee80211_local *local,
				     struct ieee80211_if_norm *bss,
				     struct sk_buff *skb)
{
	u8 *pos, *tim;
	int aid0 = 0;
	int i, num_bits = 0, n1, n2;
	u8 bitmap[251];

	/* Generate bitmap for TIM only if there are any STAs in power save
	 * mode. */
	if (atomic_read(&bss->num_sta_ps) > 0 && bss->max_aid > 0) {
		memset(bitmap, 0, sizeof(bitmap));
		spin_lock_bh(&local->sta_lock);
		for (i = 0; i < bss->max_aid; i++) {
			if (bss->sta_aid[i] &&
			    (!skb_queue_empty(&bss->sta_aid[i]->ps_tx_buf) ||
			     !skb_queue_empty(&bss->sta_aid[i]->tx_filtered)))
			{
				bitmap[(i + 1) / 8] |= 1 << (i + 1) % 8;
				num_bits++;
			}
		}
		spin_unlock_bh(&local->sta_lock);
	}

	if (bss->dtim_count == 0)
		bss->dtim_count = bss->dtim_period - 1;
	else
		bss->dtim_count--;

	tim = pos = (u8 *) skb_put(skb, 6);
	*pos++ = WLAN_EID_TIM;
	*pos++ = 4;
	*pos++ = bss->dtim_count;
	*pos++ = bss->dtim_period;

	if (bss->dtim_count == 0 && !skb_queue_empty(&bss->ps_bc_buf)) {
		aid0 = 1;
	}

	if (num_bits) {
		/* Find largest even number N1 so that bits numbered 1 through
		 * (N1 x 8) - 1 in the bitmap are 0 and number N2 so that bits
		 * (N2 + 1) x 8 through 2007 are 0. */
		n1 = 0;
		for (i = 0; i < sizeof(bitmap); i++) {
			if (bitmap[i]) {
				n1 = i & 0xfe;
				break;
			}
		}
		n2 = n1;
		for (i = sizeof(bitmap) - 1; i >= n1; i--) {
			if (bitmap[i]) {
				n2 = i;
				break;
			}
		}

		/* Bitmap control */
		*pos++ = n1 | (aid0 ? 1 : 0);
		/* Part Virt Bitmap */
		memcpy(pos, bitmap + n1, n2 - n1 + 1);

		tim[1] = n2 - n1 + 4;
		skb_put(skb, n2 - n1);
	} else {
		*pos++ = aid0 ? 1 : 0; /* Bitmap control */
		*pos++ = 0; /* Part Virt Bitmap */
	}
}




struct sk_buff * ieee80211_beacon_get(struct net_device *dev, int bss_idx,
				      struct ieee80211_tx_control *control)
{
	struct ieee80211_local *local = dev->priv;
	struct sk_buff *skb;
	struct net_device *bdev;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct ieee80211_if_norm *norm = NULL;
	struct ieee80211_rate *rate;
	struct rate_control_extra extra;
	u8 *b_head, *b_tail;
	int bh_len, bt_len;


	spin_lock_bh(&local->sub_if_lock);
	if (bss_idx < 0 || bss_idx >= local->bss_dev_count)
		bdev = NULL;
	else {
		bdev = local->bss_devs[bss_idx];
		sdata = IEEE80211_DEV_TO_SUB_IF(bdev);
		norm = &sdata->u.norm;
	}
	spin_unlock_bh(&local->sub_if_lock);

	if (bdev == NULL || norm == NULL || norm->beacon_head == NULL) {
#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
		if (net_ratelimit())
			printk(KERN_DEBUG "no beacon data avail for idx=%d "
			       "(%s)\n", bss_idx, bdev ? bdev->name : "N/A");
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */
		return NULL;
	}

	/* Assume we are generating the normal beacon locally */
	b_head = norm->beacon_head;
	b_tail = norm->beacon_tail;
	bh_len = norm->beacon_head_len;
	bt_len = norm->beacon_tail_len;


	skb = dev_alloc_skb(bh_len + bt_len + 256 /* maximum TIM len */);
	if (!skb)
		return NULL;

	memcpy(skb_put(skb, bh_len), b_head, bh_len);

	ieee80211_beacon_add_tim(local, norm, skb);

	if (b_tail) {
		memcpy(skb_put(skb, bt_len), b_tail, bt_len);
	}

	memset(&extra, 0, sizeof(extra));
	extra.endidx = local->num_curr_rates;


	rate = rate_control_get_rate(dev, skb, &extra);
	if (rate == NULL) {
		if (net_ratelimit()) {
			printk(KERN_DEBUG "%s: ieee80211_beacon_get: no rate "
			       "found\n", dev->name);
		}
		dev_kfree_skb(skb);
		return NULL;
	}

	control->tx_rate = (local->short_preamble &&
			    (rate->flags & IEEE80211_RATE_PREAMBLE2)) ?
		rate->val2 : rate->val;
	control->antenna_sel = local->conf.antenna_sel;
	control->power_level = local->conf.power_level;
	control->no_ack = 1;
	control->retry_limit = 1;
	control->rts_cts_duration = 0;
	control->clear_dst_mask = 1;


	norm->num_beacons++;
	return skb;
}

struct sk_buff *
ieee80211_get_buffered_bc(struct net_device *dev, int bss_idx,
			  struct ieee80211_tx_control *control)
{
	struct ieee80211_local *local = dev->priv;
	struct sk_buff *skb;
	struct sta_info *sta;
	ieee80211_tx_handler *handler;
	struct ieee80211_txrx_data tx;
	ieee80211_txrx_result res = TXRX_DROP;
	struct net_device *bdev;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_norm *bss;


	spin_lock_bh(&local->sub_if_lock);
	if (bss_idx < 0 || bss_idx >= local->bss_dev_count) {
		bdev = NULL;
		bss = NULL;
	} else {
		bdev = local->bss_devs[bss_idx];
		sdata = IEEE80211_DEV_TO_SUB_IF(bdev);
		bss = &sdata->u.norm;
	}
	spin_unlock_bh(&local->sub_if_lock);
	if (bdev == NULL || bss == NULL || bss->beacon_head == NULL)
		return NULL;

	if (bss->dtim_count != 0)
		return NULL; /* send buffered bc/mc only after DTIM beacon */
	skb = skb_dequeue(&bss->ps_bc_buf);
	memset(control, 0, sizeof(*control));
	if (skb == NULL)
		return NULL;
	local->total_ps_buffered--;

	if (!skb_queue_empty(&bss->ps_bc_buf) && skb->len >= 2) {
		struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
		/* more buffered multicast/broadcast frames ==> set MoreData
		 * flag in IEEE 802.11 header to inform PS STAs */
		hdr->frame_control |= cpu_to_le16(WLAN_FC_MOREDATA);
	}

	ieee80211_tx_prepare(&tx, skb, dev, control);
	sta = tx.sta;
	tx.u.tx.ps_buffered = 1;

	for (handler = local->tx_handlers; *handler != NULL; handler++) {
		res = (*handler)(&tx);
		if (res == TXRX_DROP || res == TXRX_QUEUED)
			break;
	}

	if (res == TXRX_DROP) {
		I802_DEBUG_INC(local->tx_handlers_drop);
		dev_kfree_skb(skb);
		skb = NULL;
	} else if (res == TXRX_QUEUED) {
		I802_DEBUG_INC(local->tx_handlers_queued);
		skb = NULL;
	}

	if (sta)
		sta_info_release(local, sta);

	return skb;
}


int ieee80211_hw_config(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	int i, ret = 0;

#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
	printk(KERN_DEBUG "HW CONFIG: channel=%d freq=%d mode=%d "
	       "phymode=%d\n", local->conf.channel, local->conf.freq,
	       local->conf.mode, local->conf.phymode);
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */

	if (local->hw->config)
		ret = local->hw->config(dev, &local->conf);

	for (i = 0; i < local->hw->num_modes; i++) {
		struct ieee80211_hw_modes *mode = &local->hw->modes[i];
		if (mode->mode == local->conf.phymode) {
			if (local->curr_rates != mode->rates) {
				rate_control_clear(local);
			}
			local->curr_rates = mode->rates;
			local->num_curr_rates = mode->num_rates;
			ieee80211_prepare_rates(dev);
			break;
		}
	}

	return ret;
}


struct ieee80211_conf *ieee80211_get_hw_conf(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
        return &local->conf;
}


static int ieee80211_change_mtu(struct net_device *dev, int new_mtu)
{
	/* FIX: what would be proper limits for MTU?
	 * This interface uses 802.3 frames. */
	if (new_mtu < 256 || new_mtu > 2304 - 24 - 6) {
		printk(KERN_WARNING "%s: invalid MTU %d\n",
		       dev->name, new_mtu);
		return -EINVAL;
	}

#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
	printk(KERN_DEBUG "%s: setting MTU %d\n", dev->name, new_mtu);
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */
	dev->mtu = new_mtu;
	return 0;
}


static int ieee80211_change_mtu_apdev(struct net_device *dev, int new_mtu)
{
	/* FIX: what would be proper limits for MTU?
	 * This interface uses 802.11 frames. */
	if (new_mtu < 256 || new_mtu > 2304) {
		printk(KERN_WARNING "%s: invalid MTU %d\n",
		       dev->name, new_mtu);
		return -EINVAL;
	}

#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
	printk(KERN_DEBUG "%s: setting MTU %d\n", dev->name, new_mtu);
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */
	dev->mtu = new_mtu;
	return 0;
}


static void ieee80211_tx_timeout(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;

	printk(KERN_WARNING "%s: resetting interface.\n", dev->name);

	if (local->hw->reset(dev))
		printk(KERN_ERR "%s: failed to reset interface.\n", dev->name);
	else
		netif_wake_queue(dev);
}


static int ieee80211_set_mac_address(struct net_device *dev, void *addr)
{
	struct ieee80211_local *local = dev->priv;
	struct sockaddr *a = addr;
	struct list_head *ptr;
	int res;

	if (!local->hw->set_mac_address)
		return -EOPNOTSUPP;

	res = local->hw->set_mac_address(dev, addr);
	if (res)
		return res;

	list_for_each(ptr, &local->sub_if_list)	{
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);
		memcpy(sdata->dev->dev_addr, a->sa_data, ETH_ALEN);
	}

	return 0;
}


static struct net_device_stats *ieee80211_get_stats(struct net_device *dev)
{
	struct ieee80211_sub_if_data *sdata;
	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	return &(sdata->stats);
}


static int ieee80211_open(struct net_device *dev)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_local *local = dev->priv;
	int res;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

        if (local->open_count == 0) {
		res = local->hw->open(sdata->master);
		if (res)
			return res;
		ieee80211_init_scan(sdata->master);
	}
        local->open_count++;

	netif_start_queue(dev);
	return 0;
}


static int ieee80211_stop(struct net_device *dev)
{
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_local *local = dev->priv;
	int res;

	sdata = IEEE80211_DEV_TO_SUB_IF(dev);

        netif_stop_queue(dev);

	local->open_count--;
        if (local->open_count == 0) {
		ieee80211_stop_scan(sdata->master);
		res = local->hw->stop(sdata->master);
		if (res)
			return res;
        }

	return 0;
}


static int header_parse_80211(struct sk_buff *skb, unsigned char *haddr)
{
	memcpy(haddr, skb->mac.raw + 10, ETH_ALEN); /* addr2 */
	return ETH_ALEN;
}


static struct net_device *
ieee80211_get_wds_dev(struct ieee80211_local *local, u8 *addr)
{
	struct list_head *ptr;

	list_for_each(ptr, &local->sub_if_list)	{
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);
		if (sdata->type == IEEE80211_SUB_IF_TYPE_WDS &&
		    memcmp(addr, sdata->u.wds.remote_addr, ETH_ALEN) == 0)
			return sdata->dev;
	}

	return NULL;
}


static struct net_device * ieee80211_own_bssid(struct ieee80211_local *local,
					       u8 *addr)
{
	int i;
	struct net_device *dev = NULL;

	spin_lock_bh(&local->sub_if_lock);
	for (i = 0; i < local->bss_dev_count; i++) {
		if ((memcmp(local->bss_devs[i]->dev_addr, addr, ETH_ALEN) == 0)
		) {
			dev = local->bss_devs[i];
			break;
		}
	}
	spin_unlock_bh(&local->sub_if_lock);

	return dev;
}




static struct net_device * ieee80211_sta_bssid(struct ieee80211_local *local,
					       u8 *addr, u8 *a1,
					       int *sta_multicast)
{
	struct list_head *ptr;
	int multicast;
	u8 *own_addr = local->mdev->dev_addr;

	multicast = a1[0] & 0x01;

	/* Try O(1) lookup for a common case of only one AP being used. */
	if (own_addr[0] == a1[0] && own_addr[1] == a1[1] &&
	    own_addr[2] == a1[2]) {
		int index = (((int) a1[3] << 16) | ((int) a1[4] << 8) | a1[5])
			- (((int) own_addr[3] << 16) |
			   ((int) own_addr[4] << 8) | own_addr[5]);
		if (index >= 0 && index < local->conf.bss_count &&
		    local->sta_devs[index]) {
			struct net_device *dev = local->sta_devs[index];
			struct ieee80211_sub_if_data *sdata;
			sdata = IEEE80211_DEV_TO_SUB_IF(dev);
			if (memcmp(addr, sdata->u.sta.bssid, ETH_ALEN) == 0) {
				*sta_multicast = multicast;
				return dev;
			}
		}
	}

	if (!multicast)
		return NULL;

	/* Could not find station interface, resort to O(n) lookup. */
	list_for_each(ptr, &local->sub_if_list) {
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA)
			continue;
		if (!multicast &&
		    memcmp(a1, sdata->dev->dev_addr, ETH_ALEN) != 0)
			continue;

		if (memcmp(addr, sdata->u.sta.bssid, ETH_ALEN) == 0 ||
		    (memcmp(addr, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0 &&
		     local->conf.mode == IW_MODE_ADHOC)) {
			*sta_multicast = multicast;
			return sdata->dev;
		}
	}

	return NULL;
}


static int ieee80211_own_addr(struct net_device *dev, u8 *addr)
{
	struct ieee80211_local *local = dev->priv;
	u8 *own = dev->dev_addr;
	int index;

	/* Optimization: assume that BSSID mask does not change for first
	 * three octets. */
	if (own[0] != addr[0] || own[1] != addr[1] || own[2] != addr[2])
		return 0;

	index = (((int) addr[3] << 16) | ((int) addr[4] << 8) | addr[5]) -
		(((int) own[3] << 16) | ((int) own[4] << 8) | own[5]);
	if (index >= 0 && index < local->conf.bss_count &&
	    local->sta_devs[index])
		return 1;

	return 0;
}


static ieee80211_txrx_result
ieee80211_rx_h_data(struct ieee80211_txrx_data *rx)
{
	struct net_device *dev = rx->dev;
	struct ieee80211_local *local = rx->local;
        struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) rx->skb->data;
	u16 fc, hdrlen, ethertype;
	u8 *payload;
	u8 dst[ETH_ALEN];
	u8 src[ETH_ALEN];
        struct sk_buff *skb = rx->skb, *skb2;
        struct ieee80211_sub_if_data *sdata;

	fc = rx->fc;
	if (unlikely(WLAN_FC_GET_TYPE(fc) != WLAN_FC_TYPE_DATA))
		return TXRX_CONTINUE;

	if (unlikely(!WLAN_FC_DATA_PRESENT(fc)))
		return TXRX_DROP;

	hdrlen = ieee80211_get_hdrlen(fc);

	/* convert IEEE 802.11 header + possible LLC headers into Ethernet
	 * header
	 * IEEE 802.11 address fields:
	 * ToDS FromDS Addr1 Addr2 Addr3 Addr4
	 *   0     0   DA    SA    BSSID n/a
	 *   0     1   DA    BSSID SA    n/a
	 *   1     0   BSSID SA    DA    n/a
	 *   1     1   RA    TA    DA    SA
	 */

	switch (fc & (WLAN_FC_TODS | WLAN_FC_FROMDS)) {
	case WLAN_FC_TODS:
		/* BSSID SA DA */
		memcpy(dst, hdr->addr3, ETH_ALEN);
		memcpy(src, hdr->addr2, ETH_ALEN);

		if (unlikely(local->conf.mode != IW_MODE_MASTER ||
			     !ieee80211_own_bssid(local, hdr->addr1))) {
			printk(KERN_DEBUG "%s: dropped ToDS frame (BSSID="
			       MACSTR " SA=" MACSTR " DA=" MACSTR ")\n",
			       dev->name, MAC2STR(hdr->addr1),
			       MAC2STR(hdr->addr2), MAC2STR(hdr->addr3));
			return TXRX_DROP;
		}
		break;
	case (WLAN_FC_TODS | WLAN_FC_FROMDS):
		/* RA TA DA SA */
		memcpy(dst, hdr->addr3, ETH_ALEN);
		memcpy(src, hdr->addr4, ETH_ALEN);

		dev = ieee80211_get_wds_dev(local, hdr->addr2);
		if (!dev || memcmp(hdr->addr1, dev->dev_addr, ETH_ALEN) != 0) {
			printk(KERN_DEBUG "%s: dropped FromDS&ToDS frame (RA="
			       MACSTR " TA=" MACSTR " DA=" MACSTR " SA="
			       MACSTR ")\n",
			       rx->dev->name, MAC2STR(hdr->addr1),
			       MAC2STR(hdr->addr2), MAC2STR(hdr->addr3),
			       MAC2STR(hdr->addr4));
			return TXRX_DROP;
		}
		break;
	case WLAN_FC_FROMDS:
		/* DA BSSID SA */
		memcpy(dst, hdr->addr1, ETH_ALEN);
		memcpy(src, hdr->addr3, ETH_ALEN);

		sdata = IEEE80211_DEV_TO_SUB_IF(dev);
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA ||
		    memcmp(hdr->addr3, dev->dev_addr, ETH_ALEN) == 0 ||
		    memcmp(hdr->addr2, sdata->u.sta.bssid, ETH_ALEN) != 0) {
			return TXRX_DROP;
		}
		break;
	case 0:
		/* DA SA BSSID */
		memcpy(dst, hdr->addr1, ETH_ALEN);
		memcpy(src, hdr->addr2, ETH_ALEN);

		if (local->conf.mode != IW_MODE_ADHOC ||
		    memcmp(hdr->addr3, local->bssid, ETH_ALEN) != 0) {
			if (net_ratelimit()) {
				printk(KERN_DEBUG "%s: dropped IBSS frame (DA="
				       MACSTR " SA=" MACSTR " BSSID=" MACSTR
				       ")\n",
				       dev->name, MAC2STR(hdr->addr1),
				       MAC2STR(hdr->addr2),
				       MAC2STR(hdr->addr3));
			}
			return TXRX_DROP;
		}
		break;
	}

	payload = skb->data + hdrlen;

	if (unlikely(skb->len - hdrlen < 8)) {
		if (net_ratelimit()) {
			printk(KERN_DEBUG "%s: RX too short data frame "
			       "payload\n", dev->name);
		}
		return TXRX_DROP;
	}

	ethertype = (payload[6] << 8) | payload[7];

	if (likely((memcmp(payload, rfc1042_header, 6) == 0 &&
		    ethertype != ETH_P_AARP && ethertype != ETH_P_IPX) ||
		   memcmp(payload, bridge_tunnel_header, 6) == 0)) {
		/* remove RFC1042 or Bridge-Tunnel encapsulation and
		 * replace EtherType */
		skb_pull(skb, hdrlen + 6);
		memcpy(skb_push(skb, ETH_ALEN), src, ETH_ALEN);
		memcpy(skb_push(skb, ETH_ALEN), dst, ETH_ALEN);
	} else {
		struct ethhdr *ehdr;
                unsigned short len;
		skb_pull(skb, hdrlen);
                len = htons(skb->len);
		ehdr = (struct ethhdr *)skb_push(skb, sizeof(struct ethhdr));
		memcpy(ehdr->h_dest, dst, ETH_ALEN);
		memcpy(ehdr->h_source, src, ETH_ALEN);
                ehdr->h_proto = len;
	}

        if (rx->sta && !rx->sta->assoc_ap &&
	    !(rx->sta && (rx->sta->flags & WLAN_STA_WDS)))
                skb->dev = rx->sta->dev;
        else
                skb->dev = dev;

        skb2 = NULL;
        sdata = IEEE80211_DEV_TO_SUB_IF(dev);

        /*
         * don't count the master since the low level code
         * counts it already for us.
         */
        if (skb->dev != sdata->master) {
		sdata->stats.rx_packets++;
		sdata->stats.rx_bytes += skb->len;
        }

	if (local->bridge_packets && sdata->type != IEEE80211_SUB_IF_TYPE_WDS
	    && sdata->type != IEEE80211_SUB_IF_TYPE_STA) {
		if (MULTICAST_ADDR(skb->data)) {
			/* send multicast frames both to higher layers in
			 * local net stack and back to the wireless media */
			skb2 = skb_copy(skb, GFP_ATOMIC);
			if (skb2 == NULL)
				printk(KERN_DEBUG "%s: failed to clone "
				       "multicast frame\n", dev->name);
		} else {
			struct sta_info *dsta;
                        dsta = sta_info_get(local, skb->data);
                        if (dsta && dsta->dev == NULL) {
                                printk(KERN_DEBUG "Station with null dev "
				       "structure!\n");
                        } else if (dsta && dsta->dev == dev) {
				/* Destination station is associated to this
				 * AP, so send the frame directly to it and
				 * do not pass the frame to local net stack.
                                 */
				skb2 = skb;
				skb = NULL;
			}
			if (dsta)
				sta_info_release(local, dsta);
		}
	}

	if (skb) {
		/* deliver to local stack */
		skb->protocol = eth_type_trans(skb, dev);
		memset(skb->cb, 0, sizeof(skb->cb));
		netif_rx(skb);
	}

	if (skb2) {
		/* send to wireless media */
		skb2->protocol = __constant_htons(ETH_P_802_3);
		skb2->mac.raw = skb2->nh.raw = skb2->data;
		dev_queue_xmit(skb2);
        }

	return TXRX_QUEUED;
}


static struct ieee80211_rate *
ieee80211_get_rate(struct ieee80211_local *local, int phymode, int hw_rate)
{
	int m, r;

	for (m = 0; m < local->hw->num_modes; m++) {
		struct ieee80211_hw_modes *mode = &local->hw->modes[m];
		if (mode->mode != phymode)
			continue;
		for (r = 0; r < mode->num_rates; r++) {
			struct ieee80211_rate *rate = &mode->rates[r];
			if (rate->val == hw_rate ||
			    (rate->flags & IEEE80211_RATE_PREAMBLE2 &&
			     rate->val2 == hw_rate))
				return rate;
		}
	}

	return NULL;
}


void
ieee80211_rx_mgmt(struct net_device *dev, struct sk_buff *skb,
		  struct ieee80211_rx_status *status, u32 msg_type)
{
        struct ieee80211_local *local = dev->priv;
        struct ieee80211_frame_info *fi;
        size_t hlen;
        struct ieee80211_sub_if_data *sdata;

        dev = local->apdev;
        skb->dev = dev;

        sdata = IEEE80211_DEV_TO_SUB_IF(dev);

	if (skb_headroom(skb) < sizeof(struct ieee80211_frame_info)) {
		I802_DEBUG_INC(local->rx_expand_skb_head);
		if (pskb_expand_head(skb, sizeof(struct ieee80211_frame_info),
				     0, GFP_ATOMIC)) {
			dev_kfree_skb(skb);
                        return;
		}
	}

	hlen = sizeof(struct ieee80211_frame_info);
	if (msg_type == ieee80211_msg_monitor)
		hlen -= sizeof(fi->msg_type);

	fi = (struct ieee80211_frame_info *) skb_push(skb, hlen);
        memset(fi, 0, hlen);
	if (msg_type != ieee80211_msg_monitor)
		fi->msg_type = htonl(msg_type);
	fi->version = htonl(IEEE80211_FI_VERSION);
        fi->length = htonl(hlen);
        if (status) {
//                struct timespec ts;
		struct ieee80211_rate *rate;

#if 0
                jiffies_to_timespec(status->hosttime, &ts);
		fi->hosttime = cpu_to_be64(ts.tv_sec * 1000000 +
					   ts.tv_nsec / 1000);
		fi->mactime = cpu_to_be64(status->mactime);
#endif
		switch (status->phymode) {
                case MODE_IEEE80211A:
                        fi->phytype = htonl(ieee80211_phytype_ofdm_dot11_a);
                        break;
                case MODE_IEEE80211B:
                        fi->phytype = htonl(ieee80211_phytype_dsss_dot11_b);
                        break;
                case MODE_IEEE80211G:
                        fi->phytype = htonl(ieee80211_phytype_pbcc_dot11_g);
                        break;
                case MODE_ATHEROS_TURBO:
			fi->phytype =
				htonl(ieee80211_phytype_dsss_dot11_turbo);
                        break;
                default:
                        fi->phytype = 0xAAAAAAAA;
			break;
                }
                fi->channel = htonl(status->channel);
		rate = ieee80211_get_rate(local, status->phymode,
					  status->rate);
		if (rate) {
			fi->datarate = htonl(rate->rate);
			if (rate->flags & IEEE80211_RATE_PREAMBLE2) {
				if (status->rate == rate->val)
					fi->preamble = htonl(2); /* long */
				else if (status->rate == rate->val2)
					fi->preamble = htonl(1); /* short */
			} else
				fi->preamble = htonl(0);
		} else {
			fi->datarate = htonl(0);
			fi->preamble = htonl(0);
		}

                fi->antenna = htonl(status->antenna);
                fi->priority = 0xffffffff; /* no clue */
                fi->ssi_type = htonl(ieee80211_ssi_raw);
                fi->ssi_signal = htonl(status->ssi);
                fi->ssi_noise = 0x00000000;
                fi->encoding = 0;
	} else {
                fi->ssi_type = htonl(ieee80211_ssi_none);
        }

        sdata->stats.rx_packets++;
        sdata->stats.rx_bytes += skb->len;

        skb->mac.raw = skb->data;
	skb->ip_summed = CHECKSUM_UNNECESSARY;
	skb->pkt_type = PACKET_OTHERHOST;
	skb->protocol = __constant_htons(ETH_P_802_2);
	memset(skb->cb, 0, sizeof(skb->cb));
        netif_rx(skb);
}


int ieee80211_radar_status(struct net_device *dev, int channel, int radar,
			   int radar_type)
{
	struct sk_buff *skb;
	struct ieee80211_radar_info *msg;

	skb = dev_alloc_skb(sizeof(struct ieee80211_frame_info) +
			    sizeof(struct ieee80211_radar_info));

	if (skb == NULL)
		return -ENOMEM;
	skb_reserve(skb, sizeof(struct ieee80211_frame_info));

	msg = (struct ieee80211_radar_info *)
		skb_put(skb, sizeof(struct ieee80211_radar_info));
	msg->channel = channel;
	msg->radar = radar;
	msg->radar_type = radar_type;

	ieee80211_rx_mgmt(dev, skb, 0, ieee80211_msg_radar);
	return 0;
}


int ieee80211_set_aid_for_sta(struct net_device *dev, u8 *peer_address,
			      u16 aid)
{
        struct sk_buff *skb;
        struct ieee80211_msg_set_aid_for_sta *msg;

	skb = dev_alloc_skb(sizeof(struct ieee80211_frame_info) +
			    sizeof(struct ieee80211_msg_set_aid_for_sta));

        if (skb == NULL)
		return -ENOMEM;
	skb_reserve(skb, sizeof(struct ieee80211_frame_info));

        msg = (struct ieee80211_msg_set_aid_for_sta *)
		skb_put(skb, sizeof(struct ieee80211_msg_set_aid_for_sta));
	memcpy(msg->sta_address, peer_address, ETH_ALEN);
        msg->aid = aid;

        ieee80211_rx_mgmt(dev, skb, 0, ieee80211_msg_set_aid_for_sta);
        return 0;
}


static void ap_sta_ps_start(struct net_device *dev, struct sta_info *sta)
{
	struct ieee80211_sub_if_data *sdata;
	sdata = IEEE80211_DEV_TO_SUB_IF(sta->dev);

	if (sdata->bss)
		atomic_inc(&sdata->bss->num_sta_ps);
	sta->flags |= WLAN_STA_PS;
	sta->pspoll = 0;
#ifdef IEEE80211_VERBOSE_DEBUG_PS
	printk(KERN_DEBUG "%s: STA " MACSTR " aid %d enters power "
	       "save mode\n", dev->name, MAC2STR(sta->addr), sta->aid);
#endif /* IEEE80211_VERBOSE_DEBUG_PS */
}


static int ap_sta_ps_end(struct net_device *dev, struct sta_info *sta)
{
	struct ieee80211_local *local = dev->priv;
	struct sk_buff *skb;
	int sent = 0;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_tx_packet_data *pkt_data;

	sdata = IEEE80211_DEV_TO_SUB_IF(sta->dev);
	if (sdata->bss)
		atomic_dec(&sdata->bss->num_sta_ps);
	sta->flags &= ~(WLAN_STA_PS | WLAN_STA_TIM);
	sta->pspoll = 0;
	if (!skb_queue_empty(&sta->ps_tx_buf) && local->hw->set_tim)
		local->hw->set_tim(dev, sta->aid, 0);
#ifdef IEEE80211_VERBOSE_DEBUG_PS
	printk(KERN_DEBUG "%s: STA " MACSTR " aid %d exits power "
	       "save mode\n", dev->name, MAC2STR(sta->addr), sta->aid);
#endif /* IEEE80211_VERBOSE_DEBUG_PS */
	/* Send all buffered frames to the station */
	while ((skb = skb_dequeue(&sta->tx_filtered)) != NULL) {
		pkt_data = (struct ieee80211_tx_packet_data *) skb->cb;
		sent++;
		pkt_data->control.requeue = 1;
		dev_queue_xmit(skb);
	}
	while ((skb = skb_dequeue(&sta->ps_tx_buf)) != NULL) {
		pkt_data = (struct ieee80211_tx_packet_data *) skb->cb;
		local->total_ps_buffered--;
		sent++;
#ifdef IEEE80211_VERBOSE_DEBUG_PS
		printk(KERN_DEBUG "%s: STA " MACSTR " aid %d send PS frame "
		       "since STA not sleeping anymore\n", dev->name,
		       MAC2STR(sta->addr), sta->aid);
#endif /* IEEE80211_VERBOSE_DEBUG_PS */
		pkt_data->control.requeue = 1;
		dev_queue_xmit(skb);
	}

	return sent;
}


static ieee80211_txrx_result
ieee80211_rx_h_ieee80211_rx_h_ps_poll(struct ieee80211_txrx_data *rx)
{
	struct sk_buff *skb;
	int no_pending_pkts;

	if (likely(!rx->sta || WLAN_FC_GET_TYPE(rx->fc) != WLAN_FC_TYPE_CTRL ||
		   WLAN_FC_GET_STYPE(rx->fc) != WLAN_FC_STYPE_PSPOLL))
		return TXRX_CONTINUE;

	skb = skb_dequeue(&rx->sta->tx_filtered);
	if (skb == NULL) {
		skb = skb_dequeue(&rx->sta->ps_tx_buf);
		if (skb)
			rx->local->total_ps_buffered--;
	}
	no_pending_pkts = skb_queue_empty(&rx->sta->tx_filtered) &&
		skb_queue_empty(&rx->sta->ps_tx_buf);

	if (skb) {
		struct ieee80211_hdr *hdr =
			(struct ieee80211_hdr *) skb->data;

		/* tell TX path to send one frame even though the STA may
		 * still remain is PS mode after this frame exchange */
		rx->sta->pspoll = 1;

#ifdef IEEE80211_VERBOSE_DEBUG_PS
		printk(KERN_DEBUG "STA " MACSTR " aid %d: PS Poll (entries "
		       "after %d)\n",
		       MAC2STR(rx->sta->addr), rx->sta->aid,
		       skb_queue_len(&rx->sta->ps_tx_buf));
#endif /* IEEE80211_VERBOSE_DEBUG_PS */

		/* Use MoreData flag to indicate whether there are more
		 * buffered frames for this STA */
		if (no_pending_pkts) {
			hdr->frame_control &= cpu_to_le16(~WLAN_FC_MOREDATA);
			rx->sta->flags &= ~WLAN_STA_TIM;
		} else
			hdr->frame_control |= cpu_to_le16(WLAN_FC_MOREDATA);

		dev_queue_xmit(skb);

		if (no_pending_pkts && rx->local->hw->set_tim)
			rx->local->hw->set_tim(rx->dev, rx->sta->aid, 0);
#ifdef IEEE80211_VERBOSE_DEBUG_PS
	} else if (!rx->u.rx.sent_ps_buffered) {
		printk(KERN_DEBUG "%s: STA " MACSTR " sent PS Poll even "
		       "though there is no buffered frames for it\n",
		       rx->dev->name, MAC2STR(rx->sta->addr));
#endif /* IEEE80211_VERBOSE_DEBUG_PS */

	}

	/* Free PS Poll skb here instead of returning TXRX_DROP that would
	 * count as an dropped frame. */
        dev_kfree_skb(rx->skb);

	return TXRX_QUEUED;
}


static inline struct ieee80211_fragment_entry *
ieee80211_reassemble_add(struct ieee80211_local *local,
			 unsigned int frag, unsigned int seq, int rx_queue,
			 struct sk_buff **skb)
{
        struct ieee80211_fragment_entry *entry;
	int idx;

	idx = local->fragment_next;
	entry = &local->fragments[local->fragment_next++];
	if (local->fragment_next >= IEEE80211_FRAGMENT_MAX)
		local->fragment_next = 0;

	if (entry->skb) {
#ifdef CONFIG_IEEE80211_DEBUG
		struct ieee80211_hdr *hdr =
			(struct ieee80211_hdr *) entry->skb->data;
		printk(KERN_DEBUG "%s: RX reassembly removed oldest "
		       "fragment entry (idx=%d age=%lu seq=%d last_frag=%d "
		       "addr1=" MACSTR " addr2=" MACSTR "\n",
		       local->mdev->name, idx,
		       jiffies - entry->first_frag_time, entry->seq,
		       entry->last_frag, MAC2STR(hdr->addr1),
		       MAC2STR(hdr->addr2));
#endif /* CONFIG_IEEE80211_DEBUG */
		dev_kfree_skb(entry->skb);
	}

	entry->skb = *skb;
	*skb = NULL;
	entry->first_frag_time = jiffies;
	entry->seq = seq;
	entry->rx_queue = rx_queue;
        entry->last_frag = frag;
	entry->ccmp = 0;

	return entry;
}


static inline struct ieee80211_fragment_entry *
ieee80211_reassemble_find(struct ieee80211_local *local,
			  u16 fc, unsigned int frag, unsigned int seq,
			  int rx_queue, struct ieee80211_hdr *hdr)
{
	struct ieee80211_fragment_entry *entry;
        int i, idx;

	idx = local->fragment_next;
	for (i = 0; i < IEEE80211_FRAGMENT_MAX; i++) {
		struct ieee80211_hdr *f_hdr;
		u16 f_fc;

		idx--;
		if (idx < 0)
			idx = IEEE80211_FRAGMENT_MAX - 1;

		entry = &local->fragments[idx];
		if (!entry->skb || entry->seq != seq ||
		    entry->rx_queue != rx_queue ||
		    entry->last_frag + 1 != frag)
			continue;

		f_hdr = (struct ieee80211_hdr *) entry->skb->data;
		f_fc = le16_to_cpu(f_hdr->frame_control);

		if (WLAN_FC_GET_TYPE(fc) != WLAN_FC_GET_TYPE(f_fc) ||
		    memcmp(hdr->addr1, f_hdr->addr1, ETH_ALEN) != 0 ||
		    memcmp(hdr->addr2, f_hdr->addr2, ETH_ALEN) != 0)
			continue;

		if (entry->first_frag_time + 2 * HZ < jiffies) {
			dev_kfree_skb(entry->skb);
			entry->skb = NULL;
			continue;
		}
		return entry;
        }

	return NULL;
}


static ieee80211_txrx_result
ieee80211_rx_h_defragment(struct ieee80211_txrx_data *rx)
{
	struct ieee80211_hdr *hdr;
	u16 sc;
	unsigned int frag, seq;
	struct ieee80211_fragment_entry *entry;

	hdr = (struct ieee80211_hdr *) rx->skb->data;
	sc = le16_to_cpu(hdr->seq_ctrl);
	frag = WLAN_GET_SEQ_FRAG(sc);

	if (likely((!(rx->fc & WLAN_FC_MOREFRAG) && frag == 0) ||
		   (rx->skb)->len < 24 || MULTICAST_ADDR(hdr->addr1))) {
		/* not fragmented */
		goto out;
	}
	I802_DEBUG_INC(rx->local->rx_handlers_fragments);

	seq = WLAN_GET_SEQ_SEQ(sc);

	if (frag == 0) {
		/* This is the first fragment of a new frame. */
		entry = ieee80211_reassemble_add(rx->local, frag, seq,
						 rx->u.rx.queue, &(rx->skb));
		if (rx->key && rx->key->alg == ALG_CCMP &&
		    (rx->fc & WLAN_FC_ISWEP)) {
			/* Store CCMP PN so that we can verify that the next
			 * fragment has a sequential PN value. */
			entry->ccmp = 1;
			memcpy(entry->last_pn,
			       rx->key->u.ccmp.rx_pn[rx->u.rx.queue],
			       CCMP_PN_LEN);
		}
		return TXRX_QUEUED;
	}

	/* This is a fragment for a frame that should already be pending in
	 * fragment cache. Add this fragment to the end of the pending entry.
	 */
	entry = ieee80211_reassemble_find(rx->local, rx->fc, frag, seq,
					  rx->u.rx.queue, hdr);
	if (!entry) {
		I802_DEBUG_INC(rx->local->rx_handlers_drop_defrag);
		return TXRX_DROP;
	}

	/* Verify that MPDUs within one MSDU have sequential PN values.
	 * (IEEE 802.11i, 8.3.3.4.5) */
	if (entry->ccmp) {
		int i;
		u8 pn[CCMP_PN_LEN], *rpn;
		if (rx->key == NULL || rx->key->alg != ALG_CCMP)
			return TXRX_DROP;
		memcpy(pn, entry->last_pn, CCMP_PN_LEN);
		for (i = CCMP_PN_LEN - 1; i >= 0; i--) {
			pn[i]++;
			if (pn[i])
				break;
		}
		rpn = rx->key->u.ccmp.rx_pn[rx->u.rx.queue];
		if (memcmp(pn, rpn, CCMP_PN_LEN) != 0) {
			printk(KERN_DEBUG "%s: defrag: CCMP PN not sequential"
			       " A2=" MACSTR " PN=%02x%02x%02x%02x%02x%02x "
			       "(expected %02x%02x%02x%02x%02x%02x)\n",
			       rx->dev->name, MAC2STR(hdr->addr2),
			       rpn[0], rpn[1], rpn[2], rpn[3], rpn[4], rpn[5],
			       pn[0], pn[1], pn[2], pn[3], pn[4], pn[5]);
			return TXRX_DROP;
		}
		memcpy(entry->last_pn, pn, CCMP_PN_LEN);
	}

	/* TODO: could gather list of skb's and reallocate data buffer only
	 * after finding out the total length of the frame */
	skb_pull(rx->skb, ieee80211_get_hdrlen(rx->fc));
	if (skb_tailroom(entry->skb) < rx->skb->len) {
		I802_DEBUG_INC(rx->local->rx_expand_skb_head2);
		if (unlikely(pskb_expand_head(entry->skb, 0, rx->skb->len,
					      GFP_ATOMIC))) {
			I802_DEBUG_INC(rx->local->rx_handlers_drop_defrag);
			return TXRX_DROP;
		}
	}
	memcpy(skb_put(entry->skb, rx->skb->len), rx->skb->data, rx->skb->len);
	entry->last_frag = frag;
	dev_kfree_skb(rx->skb);

	if (rx->fc & WLAN_FC_MOREFRAG) {
                rx->skb = NULL;
		return TXRX_QUEUED;
	}

	/* Complete frame has been reassembled - process it now */
	rx->skb = entry->skb;
	rx->fragmented = 1;
        entry->skb = NULL;

 out:
	if (rx->sta)
		rx->sta->rx_packets++;
	if (MULTICAST_ADDR(hdr->addr1))
		rx->local->dot11MulticastReceivedFrameCount++;
#ifdef IEEE80211_LEDS
        else
		ieee80211_rx_led(2, rx->dev);
#endif /* IEEE80211_LEDS */
	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_rx_h_monitor(struct ieee80211_txrx_data *rx)
{
	if (rx->local->conf.mode == IW_MODE_MONITOR) {
		ieee80211_rx_mgmt(rx->dev, rx->skb, rx->u.rx.status,
                                  ieee80211_msg_monitor);
		return TXRX_QUEUED;
	}

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_rx_h_check(struct ieee80211_txrx_data *rx)
{
	struct ieee80211_hdr *hdr;
	int always_sta_key;
	hdr = (struct ieee80211_hdr *) rx->skb->data;

	/* Drop duplicate 802.11 retransmissions (IEEE 802.11 Chap. 9.2.9) */
	if (rx->sta && !MULTICAST_ADDR(hdr->addr1)) {
		if (unlikely(rx->fc & WLAN_FC_RETRY &&
			     rx->sta->last_seq_ctrl[rx->u.rx.queue] ==
			     hdr->seq_ctrl)) {
			rx->local->dot11FrameDuplicateCount++;
			rx->sta->num_duplicates++;
			return TXRX_DROP;
		} else
			rx->sta->last_seq_ctrl[rx->u.rx.queue] = hdr->seq_ctrl;
	}

	if (rx->local->hw->rx_includes_fcs && rx->skb->len > FCS_LEN)
		skb_trim(rx->skb, rx->skb->len - FCS_LEN);

	if (unlikely(rx->skb->len < 16)) {
		I802_DEBUG_INC(rx->local->rx_handlers_drop_short);
		return TXRX_DROP;
	}

	/* Filter out foreign unicast packets when in promiscuous mode.
	 * FIX: Filter out multicast to foreign BSSID. */
	if (rx->local->conf.mode == IW_MODE_INFRA &&
	    !MULTICAST_ADDR(hdr->addr1) &&
	    !ieee80211_own_addr(rx->dev, hdr->addr1))
		return TXRX_DROP;

	/* Drop disallowed frame classes based on STA auth/assoc state;
	 * IEEE 802.11, Chap 5.5.
	 *
	 * 80211.o does filtering only based on association state, i.e., it
	 * drops Class 3 frames from not associated stations. hostapd sends
	 * deauth/disassoc frames when needed. In addition, hostapd is
	 * responsible for filtering on both auth and assoc states.
	 */
	if (unlikely((WLAN_FC_GET_TYPE(rx->fc) == WLAN_FC_TYPE_DATA ||
		      (WLAN_FC_GET_TYPE(rx->fc) == WLAN_FC_TYPE_CTRL &&
		       WLAN_FC_GET_STYPE(rx->fc) == WLAN_FC_STYPE_PSPOLL)) &&
		     rx->local->conf.mode != IW_MODE_ADHOC &&
		     (!rx->sta || !(rx->sta->flags & WLAN_STA_ASSOC)))) {
		if (!(rx->fc & WLAN_FC_FROMDS) && !(rx->fc & WLAN_FC_TODS)) {
			/* Drop IBSS frames silently. */
			return TXRX_DROP;
		}

		ieee80211_rx_mgmt(rx->dev, rx->skb, rx->u.rx.status,
				  ieee80211_msg_sta_not_assoc);
		return TXRX_QUEUED;
	}

	if (rx->local->conf.mode == IW_MODE_INFRA)
		always_sta_key = 0;
	else
		always_sta_key = 1;

	if (rx->sta && rx->sta->key && always_sta_key) {
		rx->key = rx->sta->key;
        } else {
		if (!rx->sdata) {
			printk(KERN_DEBUG "%s: sdata was null in packet!!\n",
			       rx->dev->name);
			printk(KERN_DEBUG "%s: Addr1: " MACSTR "\n",
			       rx->dev->name, MAC2STR(hdr->addr1));
			printk(KERN_DEBUG "%s: Addr2: " MACSTR "\n",
			       rx->dev->name, MAC2STR(hdr->addr2));
			printk(KERN_DEBUG "%s: Addr3: " MACSTR "\n",
			       rx->dev->name, MAC2STR(hdr->addr3));
			return TXRX_DROP;
		}
		if (rx->sta && rx->sta->key)
			rx->key = rx->sta->key;
		else
			rx->key = rx->sdata->default_key;

		if (rx->local->hw->wep_include_iv &&
		    rx->fc & WLAN_FC_ISWEP) {
			int keyidx = ieee80211_wep_get_keyidx(rx->skb);

			if (keyidx >= 0 && keyidx < NUM_DEFAULT_KEYS &&
			    (rx->sta == NULL || rx->sta->key == NULL ||
			     keyidx > 0)) {
				rx->key = rx->sdata->keys[keyidx];
			}
			if (!rx->key) {
				printk(KERN_DEBUG "%s: RX WEP frame with "
				       "unknown keyidx %d (A1=" MACSTR " A2="
				       MACSTR " A3=" MACSTR ")\n",
				       rx->dev->name, keyidx,
				       MAC2STR(hdr->addr1),
				       MAC2STR(hdr->addr2),
				       MAC2STR(hdr->addr3));
				ieee80211_rx_mgmt(
					rx->dev, rx->skb, rx->u.rx.status,
					ieee80211_msg_wep_frame_unknown_key);
				return TXRX_QUEUED;
			}
		}
        }

	if (rx->fc & WLAN_FC_ISWEP && rx->key) {
		rx->key->tx_rx_count++;
		if (unlikely(rx->local->key_tx_rx_threshold &&
			     rx->key->tx_rx_count >
			     rx->local->key_tx_rx_threshold)) {
			ieee80211_key_threshold_notify(rx->dev, rx->key,
						       rx->sta);
		}
	}

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_rx_h_sta_process(struct ieee80211_txrx_data *rx)
{
	struct sta_info *sta = rx->sta;
	struct net_device *dev = rx->dev;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) rx->skb->data;

	if (!sta)
		return TXRX_CONTINUE;

	/* Update last_rx only for IBSS packets which are for the current
	 * BSSID to avoid keeping the current IBSS network alive in cases where
	 * other STAs are using different BSSID. */
	if (rx->local->conf.mode == IW_MODE_ADHOC) {
		u8 *bssid = ieee80211_get_bssid(hdr, rx->skb->len);
		if (memcmp(bssid, rx->local->bssid, ETH_ALEN) == 0)
			sta->last_rx = jiffies;
	} else
	if (!MULTICAST_ADDR(hdr->addr1) ||
	    rx->local->conf.mode == IW_MODE_INFRA) {
		/* Update last_rx only for unicast frames in order to prevent
		 * the Probe Request frames (the only broadcast frames from a
		 * STA in infrastructure mode) from keeping a connection alive.
		 */
		sta->last_rx = jiffies;
	}
	sta->rx_fragments++;
	sta->rx_bytes += rx->skb->len;
	sta->last_rssi = rx->u.rx.status->ssi;

	if (!(rx->fc & WLAN_FC_MOREFRAG)) {
		/* Change STA power saving mode only in the end of a frame
		 * exchange sequence */
		if ((sta->flags & WLAN_STA_PS) && !(rx->fc & WLAN_FC_PWRMGT))
			rx->u.rx.sent_ps_buffered += ap_sta_ps_end(dev, sta);
		else if (!(sta->flags & WLAN_STA_PS) &&
			 (rx->fc & WLAN_FC_PWRMGT))
			ap_sta_ps_start(dev, sta);
	}

	/* Drop data::nullfunc frames silently, since they are used only to
	 * control station power saving mode. */
	if (WLAN_FC_GET_TYPE(rx->fc) == WLAN_FC_TYPE_DATA &&
	    WLAN_FC_GET_STYPE(rx->fc) == WLAN_FC_STYPE_NULLFUNC) {
		I802_DEBUG_INC(rx->local->rx_handlers_drop_nullfunc);
		/* Update counter and free packet here to avoid counting this
		 * as a dropped packed. */
		sta->rx_packets++;
		dev_kfree_skb(rx->skb);
		return TXRX_QUEUED;
	}

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_rx_h_wep_weak_iv_detection(struct ieee80211_txrx_data *rx)
{
	if (!rx->sta || !(rx->fc & WLAN_FC_ISWEP) ||
	    WLAN_FC_GET_TYPE(rx->fc) != WLAN_FC_TYPE_DATA || !rx->key ||
	    rx->key->alg != ALG_WEP)
		return TXRX_CONTINUE;

	/* Check for weak IVs, if hwaccel did not remove IV from the frame */
	if (rx->local->hw->wep_include_iv ||
	    rx->key->force_sw_encrypt || rx->local->conf.sw_decrypt) {
		u8 *iv = ieee80211_wep_is_weak_iv(rx->skb, rx->key);
		if (iv) {
			rx->sta->wep_weak_iv_count++;
		}
	}

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_rx_h_wep_decrypt(struct ieee80211_txrx_data *rx)
{
	/* If the device handles decryption totally, skip this test */
	if (rx->local->hw->device_hides_wep)
		return TXRX_CONTINUE;

	if ((rx->key && rx->key->alg != ALG_WEP) ||
	    !(rx->fc & WLAN_FC_ISWEP) ||
	    (WLAN_FC_GET_TYPE(rx->fc) != WLAN_FC_TYPE_DATA &&
	     (WLAN_FC_GET_TYPE(rx->fc) != WLAN_FC_TYPE_MGMT ||
	      WLAN_FC_GET_STYPE(rx->fc) != WLAN_FC_STYPE_AUTH)))
		return TXRX_CONTINUE;

	if (!rx->key) {
		printk(KERN_DEBUG "%s: RX WEP frame, but no key set\n",
		       rx->dev->name);
		return TXRX_DROP;
	}

	if (!(rx->u.rx.status->flag & RX_FLAG_DECRYPTED) ||
	    rx->key->force_sw_encrypt || rx->local->conf.sw_decrypt) {
		if (ieee80211_wep_decrypt(rx->local, rx->skb, rx->key)) {
			printk(KERN_DEBUG "%s: RX WEP frame, decrypt "
			       "failed\n", rx->dev->name);
			return TXRX_DROP;
		}
	} else if (rx->local->hw->wep_include_iv) {
		ieee80211_wep_remove_iv(rx->local, rx->skb, rx->key);
		/* remove ICV */
		skb_trim(rx->skb, rx->skb->len - 4);
	}

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_rx_h_802_1x_pae(struct ieee80211_txrx_data *rx)
{
	if (rx->sdata->eapol && ieee80211_is_eapol(rx->skb) &&
	    rx->local->conf.mode != IW_MODE_INFRA) {
		/* Pass both encrypted and unencrypted EAPOL frames to user
		 * space for processing. */
		ieee80211_rx_mgmt(rx->dev, rx->skb, rx->u.rx.status,
                                  ieee80211_msg_normal);
		return TXRX_QUEUED;
	}

	if (unlikely(rx->sdata->ieee802_1x &&
		     WLAN_FC_GET_TYPE(rx->fc) == WLAN_FC_TYPE_DATA &&
		     WLAN_FC_GET_STYPE(rx->fc) != WLAN_FC_STYPE_NULLFUNC &&
		     (!rx->sta || !(rx->sta->flags & WLAN_STA_AUTHORIZED)) &&
		     !ieee80211_is_eapol(rx->skb))) {
#ifdef CONFIG_IEEE80211_DEBUG
		struct ieee80211_hdr *hdr =
			(struct ieee80211_hdr *) rx->skb->data;
		printk(KERN_DEBUG "%s: dropped frame from " MACSTR
		       " (unauthorized port)\n", rx->dev->name,
		       MAC2STR(hdr->addr2));
#endif /* CONFIG_IEEE80211_DEBUG */
		return TXRX_DROP;
	}

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_rx_h_drop_unencrypted(struct ieee80211_txrx_data *rx)
{
	/*  If the device handles decryption totally, skip this test */
	if (rx->local->hw->device_hides_wep)
		return TXRX_CONTINUE;

	/* Drop unencrypted frames if key is set. */
	if (unlikely(!(rx->fc & WLAN_FC_ISWEP) &&
		     WLAN_FC_GET_TYPE(rx->fc) == WLAN_FC_TYPE_DATA &&
		     WLAN_FC_GET_STYPE(rx->fc) != WLAN_FC_STYPE_NULLFUNC &&
		     (rx->key || rx->sdata->drop_unencrypted) &&
		     (rx->sdata->eapol == 0 ||
		      !ieee80211_is_eapol(rx->skb)))) {
		printk(KERN_DEBUG "%s: RX non-WEP frame, but expected "
		       "encryption\n", rx->dev->name);
		return TXRX_DROP;
	}
	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_rx_h_mgmt(struct ieee80211_txrx_data *rx)
{
        struct ieee80211_sub_if_data *sdata;
	sdata = IEEE80211_DEV_TO_SUB_IF(rx->dev);
	if (sdata->type == IEEE80211_SUB_IF_TYPE_STA) {
		ieee80211_sta_rx_mgmt(rx->dev, rx->skb, rx->u.rx.status);
	} else {
		/* Management frames are sent to hostapd for processing */
		ieee80211_rx_mgmt(rx->dev, rx->skb, rx->u.rx.status,
				  ieee80211_msg_normal);
	}
	return TXRX_QUEUED;
}


static ieee80211_txrx_result
ieee80211_rx_h_passive_scan(struct ieee80211_txrx_data *rx)
{
	struct ieee80211_local *local = rx->local;
        struct sk_buff *skb = rx->skb;

	if (unlikely(local->sta_scanning != 0)) {
		ieee80211_sta_rx_scan(rx->dev, skb, rx->u.rx.status);
		return TXRX_QUEUED;
	}

	if (WLAN_FC_GET_TYPE(rx->fc) == WLAN_FC_TYPE_DATA)
		local->scan.txrx_count++;
        if (unlikely(local->scan.in_scan != 0 &&
		     rx->u.rx.status->freq == local->scan.freq)) {
                struct ieee80211_hdr *hdr;
                u16 fc;

                local->scan.rx_packets++;

                hdr = (struct ieee80211_hdr *) skb->data;
                fc = le16_to_cpu(hdr->frame_control);

                if (WLAN_FC_GET_TYPE(fc) == WLAN_FC_TYPE_MGMT &&
		    WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_BEACON) {
			local->scan.rx_beacon++;
			/* Need to trim FCS here because it is normally
			 * removed only after this passive scan handler. */
			if (rx->local->hw->rx_includes_fcs &&
			    rx->skb->len > FCS_LEN)
				skb_trim(rx->skb, rx->skb->len - FCS_LEN);

                        ieee80211_rx_mgmt(rx->dev, rx->skb, rx->u.rx.status,
                                          ieee80211_msg_passive_scan);
                        return TXRX_QUEUED;
                } else {
			I802_DEBUG_INC(local->rx_handlers_drop_passive_scan);
                        return TXRX_DROP;
                }
        }

        return TXRX_CONTINUE;
}


static u8 * ieee80211_get_bssid(struct ieee80211_hdr *hdr, size_t len)
{
	u16 fc;

	if (len < 24)
		return NULL;

	fc = le16_to_cpu(hdr->frame_control);

	switch (WLAN_FC_GET_TYPE(fc)) {
	case WLAN_FC_TYPE_DATA:
		switch (fc & (WLAN_FC_TODS | WLAN_FC_FROMDS)) {
		case WLAN_FC_TODS:
			return hdr->addr1;
		case (WLAN_FC_TODS | WLAN_FC_FROMDS):
			return NULL;
		case WLAN_FC_FROMDS:
			return hdr->addr2;
		case 0:
			return hdr->addr3;
		}
		break;
	case WLAN_FC_TYPE_MGMT:
		return hdr->addr3;
	case WLAN_FC_TYPE_CTRL:
		if (WLAN_FC_GET_STYPE(fc) == WLAN_FC_STYPE_PSPOLL)
			return hdr->addr1;
		else
			return NULL;
	}

	return NULL;
}


static struct net_device * ieee80211_get_rx_dev(struct ieee80211_local *local,
						struct ieee80211_hdr *hdr,
						size_t len, int *sta_broadcast)
{
	u8 *bssid;
	struct net_device *dev;
	u16 fc;

	bssid = ieee80211_get_bssid(hdr, len);
	if (bssid) {
		dev = ieee80211_own_bssid(local, bssid);
		if (!dev && (local->conf.mode == IW_MODE_INFRA ||
			     local->conf.mode == IW_MODE_ADHOC))
			dev = ieee80211_sta_bssid(local, bssid, hdr->addr1,
						  sta_broadcast);
		if (dev)
			return dev;
	}

	if (len >= 30) {
		fc = le16_to_cpu(hdr->frame_control);
		if (WLAN_FC_GET_TYPE(fc) == WLAN_FC_TYPE_DATA &&
		    (fc & (WLAN_FC_TODS | WLAN_FC_FROMDS)) ==
		    (WLAN_FC_TODS | WLAN_FC_FROMDS)) {
			dev = ieee80211_get_wds_dev(local, hdr->addr2);
			if (dev)
				return dev;
		}
	}

	/* Default to default device if nothing else matches */
	return local->wdev;
}


static void ieee80211_rx_michael_mic_report(struct net_device *dev,
					    struct ieee80211_hdr *hdr,
					    struct sta_info *sta,
					    struct ieee80211_txrx_data *rx)
{
	int keyidx, hdrlen;

	hdrlen = ieee80211_get_hdrlen_from_skb(rx->skb);
	if (rx->skb->len >= hdrlen + 4)
		keyidx = rx->skb->data[hdrlen + 3] >> 6;
	else
		keyidx = -1;

	/* TODO: verify that this is not triggered by fragmented
	 * frames (hw does not verify MIC for them). */
	printk(KERN_DEBUG "%s: TKIP hwaccel reported Michael MIC "
	       "failure from " MACSTR " to " MACSTR " keyidx=%d\n",
	       dev->name, MAC2STR(hdr->addr2), MAC2STR(hdr->addr1), keyidx);

	if (sta == NULL) {
		/* Some hardware versions seem to generate incorrect
		 * Michael MIC reports; ignore them to avoid triggering
		 * countermeasures. */
		printk(KERN_DEBUG "%s: ignored spurious Michael MIC "
		       "error for unknown address " MACSTR "\n",
		       dev->name, MAC2STR(hdr->addr2));
		goto ignore;
	}

	if (!(rx->fc & WLAN_FC_ISWEP)) {
		printk(KERN_DEBUG "%s: ignored spurious Michael MIC "
		       "error for a frame with no ISWEP flag (src "
		       MACSTR ")\n", dev->name, MAC2STR(hdr->addr2));
		goto ignore;
	}

	if (rx->local->hw->wep_include_iv &&
	    rx->local->conf.mode == IW_MODE_MASTER) {
		int keyidx = ieee80211_wep_get_keyidx(rx->skb);
		/* AP with Pairwise keys support should never receive Michael
		 * MIC errors for non-zero keyidx because these are reserved
		 * for group keys and only the AP is sending real multicast
		 * frames in BSS. */
		if (keyidx) {
			printk(KERN_DEBUG "%s: ignored Michael MIC error for "
			       "a frame with non-zero keyidx (%d) (src " MACSTR
			       ")\n", dev->name, keyidx, MAC2STR(hdr->addr2));
			goto ignore;
		}
	}

	if (WLAN_FC_GET_TYPE(rx->fc) != WLAN_FC_TYPE_DATA &&
	    (WLAN_FC_GET_TYPE(rx->fc) != WLAN_FC_TYPE_MGMT ||
	     WLAN_FC_GET_STYPE(rx->fc) != WLAN_FC_STYPE_AUTH)) {
		printk(KERN_DEBUG "%s: ignored spurious Michael MIC "
		       "error for a frame that cannot be encrypted "
		       "(fc=0x%04x) (src " MACSTR ")\n",
		       dev->name, rx->fc, MAC2STR(hdr->addr2));
		goto ignore;
	}

	do {
		union iwreq_data wrqu;
		char *buf = kmalloc(128, GFP_ATOMIC);
		if (buf == NULL)
			break;

		/* TODO: needed parameters: count, key type, TSC */
		sprintf(buf, "MLME-MICHAELMICFAILURE.indication("
			"keyid=%d %scast addr=" MACSTR ")",
			keyidx, hdr->addr1[0] & 0x01 ? "broad" : "uni",
			MAC2STR(hdr->addr2));
		memset(&wrqu, 0, sizeof(wrqu));
		wrqu.data.length = strlen(buf);
		wireless_send_event(rx->dev, IWEVCUSTOM, &wrqu, buf);
		kfree(buf);
	} while (0);

	/* TODO: consider verifying the MIC error report with software
	 * implementation if we get too many spurious reports from the
	 * hardware. */
	ieee80211_rx_mgmt(rx->dev, rx->skb, rx->u.rx.status,
			  ieee80211_msg_michael_mic_failure);
	return;

 ignore:
	dev_kfree_skb(rx->skb);
	rx->skb = NULL;
}


static void ieee80211_sta_rx_broadcast(struct ieee80211_txrx_data *rx)
{
	struct ieee80211_local *local = rx->dev->priv;
	u8 *_bssid, bssid[ETH_ALEN];
	struct sk_buff *orig_skb = rx->skb, *skb;
	struct ieee80211_hdr *hdr;
	ieee80211_rx_handler *handler;
	ieee80211_txrx_result res;
	struct list_head *ptr;

	hdr = (struct ieee80211_hdr *) orig_skb->data;
	_bssid = ieee80211_get_bssid(hdr, orig_skb->len);
	if (_bssid == NULL) {
		dev_kfree_skb(orig_skb);
		return;
	}
	memcpy(bssid, _bssid, ETH_ALEN);

	list_for_each(ptr, &local->sub_if_list) {
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);
		if (sdata->type != IEEE80211_SUB_IF_TYPE_STA ||
		    (memcmp(bssid, sdata->u.sta.bssid, ETH_ALEN) != 0 &&
		     !(bssid[0] & 0x01)))
			continue;

		skb = skb_copy(orig_skb, GFP_ATOMIC);
		if (skb == NULL) {
			if (net_ratelimit()) {
				printk(KERN_DEBUG "%s: failed to copy "
				       "multicast frame for %s",
				       rx->dev->name, sdata->dev->name);
			}
			continue;
		}

		hdr = (struct ieee80211_hdr *) skb->data;
		rx->skb = skb;
		rx->dev = sdata->dev;
		rx->sdata = IEEE80211_DEV_TO_SUB_IF(rx->dev);

		res = TXRX_DROP;
		for (handler = local->rx_handlers; *handler != NULL; handler++)
		{
			res = (*handler)(rx);
			if (res == TXRX_DROP || res == TXRX_QUEUED)
				break;
		}

		if (res == TXRX_DROP || *handler == NULL)
			dev_kfree_skb(skb);
	}

	dev_kfree_skb(orig_skb);
}


/*
 * This is the receive path handler. It is called by a low level driver when an
 * 802.11 MPDU is received from the hardware.
 */
void ieee80211_rx(struct net_device *dev, struct sk_buff *skb,
		  struct ieee80211_rx_status *status)
{
	struct ieee80211_local *local = dev->priv;
	struct sta_info *sta;
	struct ieee80211_hdr *hdr;
	ieee80211_rx_handler *handler;
	struct ieee80211_txrx_data rx;
        ieee80211_txrx_result res = TXRX_DROP;
	u16 type;
	int sta_broadcast = 0;

	hdr = (struct ieee80211_hdr *) skb->data;
	memset(&rx, 0, sizeof(rx));
	rx.skb = skb;
	rx.local = local;
	if (skb->len >= 16) {
		sta = rx.sta = sta_info_get(local, hdr->addr2);
		if (unlikely(sta == NULL &&
			     local->conf.mode == IW_MODE_ADHOC)) {
			u8 *bssid = ieee80211_get_bssid(hdr, skb->len);
			if (bssid &&
			    memcmp(bssid, local->bssid, ETH_ALEN) == 0)
				sta = rx.sta =
					ieee80211_ibss_add_sta(dev, skb, bssid,
							       hdr->addr2);
		}
	} else
		sta = rx.sta = NULL;
        if (sta && !sta->assoc_ap && !(sta->flags & WLAN_STA_WDS))
		rx.dev = sta->dev;
	else
		rx.dev = ieee80211_get_rx_dev(local, hdr, skb->len,
					      &sta_broadcast);

	rx.sdata = IEEE80211_DEV_TO_SUB_IF(rx.dev);
	rx.u.rx.status = status;
	rx.fc = skb->len >= 2 ? le16_to_cpu(hdr->frame_control) : 0;
	type = WLAN_FC_GET_TYPE(rx.fc);
	if (type == WLAN_FC_TYPE_DATA || type == WLAN_FC_TYPE_MGMT)
		local->dot11ReceivedFragmentCount++;
	if (sta_broadcast) {
		ieee80211_sta_rx_broadcast(&rx);
		goto end;
	}

        if ((status->flag & RX_FLAG_MMIC_ERROR)) {
		ieee80211_rx_michael_mic_report(dev, hdr, sta, &rx);
		goto end;
        }

	for (handler = local->rx_handlers; *handler != NULL; handler++) {
		res = (*handler)(&rx);
		if (res != TXRX_CONTINUE) {
			if (res == TXRX_DROP) {
				I802_DEBUG_INC(local->rx_handlers_drop);
				if (sta)
					sta->rx_dropped++;
			}
			if (res == TXRX_QUEUED)
				I802_DEBUG_INC(local->rx_handlers_queued);
			break;
		}
	}

	if (res == TXRX_DROP || *handler == NULL)
		dev_kfree_skb(skb);

  end:
	if (sta)
                sta_info_release(local, sta);
}


static ieee80211_txrx_result
ieee80211_tx_h_load_stats(struct ieee80211_txrx_data *tx)
{
        struct ieee80211_local *local = tx->local;
	struct sk_buff *skb = tx->skb;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	u32 load = 0, hdrtime;

	/* TODO: this could be part of tx_status handling, so that the number
	 * of retries would be known; TX rate should in that case be stored
	 * somewhere with the packet */

	/* Estimate total channel use caused by this frame */

	/* 1 bit at 1 Mbit/s takes 1 usec; in channel_use values,
	 * 1 usec = 1/8 * (1080 / 10) = 13.5 */

	if (local->conf.phymode == MODE_IEEE80211A ||
	    local->conf.phymode == MODE_ATHEROS_TURBO ||
            local->conf.phymode == MODE_ATHEROS_TURBOG ||
	    (local->conf.phymode == MODE_IEEE80211G &&
	     tx->u.tx.rate->flags & IEEE80211_RATE_ERP))
		hdrtime = CHAN_UTIL_HDR_SHORT;
	else
		hdrtime = CHAN_UTIL_HDR_LONG;

	load = hdrtime;
	if (!MULTICAST_ADDR(hdr->addr1))
		load += hdrtime;

	if (tx->u.tx.control->use_rts_cts)
		load += 2 * hdrtime;
	else if (tx->u.tx.control->use_cts_protect)
		load += hdrtime;

	load += skb->len * tx->u.tx.rate->rate_inv;

	if (tx->u.tx.extra_frag) {
		int i;
		for (i = 0; i < tx->u.tx.num_extra_frag; i++) {
			load += 2 * hdrtime;
			load += tx->u.tx.extra_frag[i]->len *
				tx->u.tx.rate->rate;
		}
	}

	/* Divide channel_use by 8 to avoid wrapping around the counter */
        load >>= CHAN_UTIL_SHIFT;
        local->channel_use_raw += load;
	if (tx->sta)
		tx->sta->channel_use_raw += load;
        tx->sdata->channel_use_raw += load;

	return TXRX_CONTINUE;
}


static ieee80211_txrx_result
ieee80211_rx_h_load_stats(struct ieee80211_txrx_data *rx)
{
        struct ieee80211_local *local = rx->local;
	struct sk_buff *skb = rx->skb;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	u32 load = 0, hdrtime;
	struct ieee80211_rate *rate;
	int i;

	/* Estimate total channel use caused by this frame */

	if (unlikely(local->num_curr_rates < 0))
		return TXRX_CONTINUE;

	rate = &local->curr_rates[0];
	for (i = 0; i < local->num_curr_rates; i++) {
		if (local->curr_rates[i].val == rx->u.rx.status->rate) {
			rate = &local->curr_rates[i];
			break;
		}
	}

	/* 1 bit at 1 Mbit/s takes 1 usec; in channel_use values,
	 * 1 usec = 1/8 * (1080 / 10) = 13.5 */

	if (local->conf.phymode == MODE_IEEE80211A ||
	    local->conf.phymode == MODE_ATHEROS_TURBO ||
            local->conf.phymode == MODE_ATHEROS_TURBOG ||
	    (local->conf.phymode == MODE_IEEE80211G &&
	     rate->flags & IEEE80211_RATE_ERP))
		hdrtime = CHAN_UTIL_HDR_SHORT;
	else
		hdrtime = CHAN_UTIL_HDR_LONG;

	load = hdrtime;
	if (!MULTICAST_ADDR(hdr->addr1))
		load += hdrtime;

	load += skb->len * rate->rate_inv;

	/* Divide channel_use by 8 to avoid wrapping around the counter */
        load >>= CHAN_UTIL_SHIFT;
        local->channel_use_raw += load;
	if (rx->sta)
		rx->sta->channel_use_raw += load;
        rx->sdata->channel_use_raw += load;

	return TXRX_CONTINUE;
}


static void ieee80211_stat_refresh(unsigned long data)
{
	struct ieee80211_local *local = (struct ieee80211_local *) data;
        struct list_head *ptr, *n;

	if (!local->stat_time)
		return;

	/* go through all stations */
	spin_lock_bh(&local->sta_lock);
	list_for_each(ptr, &local->sta_list) {
		struct sta_info *sta =
			list_entry(ptr, struct sta_info, list);
		sta->channel_use = (sta->channel_use_raw / local->stat_time) /
			CHAN_UTIL_PER_10MS;
		sta->channel_use_raw = 0;
	}
	spin_unlock_bh(&local->sta_lock);

	/* go through all subinterfaces */
	list_for_each_safe(ptr, n, &local->sub_if_list) {
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);
                sdata->channel_use = (sdata->channel_use_raw /
				      local->stat_time) / CHAN_UTIL_PER_10MS;
                sdata->channel_use_raw = 0;

        }

	/* hardware interface */
	local->channel_use = (local->channel_use_raw /
			      local->stat_time) / CHAN_UTIL_PER_10MS;
	local->channel_use_raw = 0;

	local->stat_timer.expires = jiffies + HZ * local->stat_time / 100;
	add_timer(&local->stat_timer);
}


/* This is a version of the rx handler that can be called from hard irq
 * context. Post the skb on the queue and schedule the tasklet */
void ieee80211_rx_irqsafe(struct net_device *dev, struct sk_buff *skb,
			  struct ieee80211_rx_status *status)
{
        struct ieee80211_local *local = dev->priv;

        skb->dev = dev;
        memcpy(skb->cb, status, sizeof(struct ieee80211_rx_status));
        skb->pkt_type = ieee80211_rx_msg;
        skb_queue_tail(&local->skb_queue, skb);
        tasklet_schedule(&local->tasklet);
}


void ieee80211_tx_status_irqsafe(struct net_device *dev, struct sk_buff *skb,
				 struct ieee80211_tx_status *status)
{
        struct ieee80211_local *local = dev->priv;
	int tmp;

	if (status->tx_filtered || status->excessive_retries) {
		/* Need to save a copy of skb->cb somewhere. Storing it in the
		 * end of the data might not be the most efficient way of doing
		 * this (since it may require reallocation of packet data), but
		 * should be good enough for now since tx_filtered or
		 * excessive_retries should not be triggered that often. */
		if (skb_is_nonlinear(skb)) {
			if (skb_linearize(skb, GFP_ATOMIC)) {
				printk(KERN_DEBUG "%s: Failed to linearize "
				       "skb\n", dev->name);
				dev_kfree_skb_irq(skb);
				return;
			}
		}
		if (skb_tailroom(skb) < sizeof(skb->cb) &&
		    pskb_expand_head(skb, 0, sizeof(skb->cb), GFP_ATOMIC)) {
			printk(KERN_DEBUG "%s: Failed to store skb->cb "
			       "in skb->data for TX filtered frame\n",
			       dev->name);
			dev_kfree_skb_irq(skb);
			return;
		}
		memcpy(skb_put(skb, sizeof(skb->cb)), skb->cb,
		       sizeof(skb->cb));
	}

        skb->dev = dev;
        memcpy(skb->cb, status, sizeof(struct ieee80211_tx_status));
        skb->pkt_type = ieee80211_tx_status_msg;
	skb_queue_tail(status->req_tx_status ?
		       &local->skb_queue : &local->skb_queue_unreliable, skb);
	tmp = skb_queue_len(&local->skb_queue) +
		skb_queue_len(&local->skb_queue_unreliable);
	while (tmp > IEEE80211_IRQSAFE_QUEUE_LIMIT &&
	       (skb = skb_dequeue(&local->skb_queue_unreliable))) {
		dev_kfree_skb_irq(skb);
		tmp--;
		I802_DEBUG_INC(local->tx_status_drop);
	}
        tasklet_schedule(&local->tasklet);
}


static void ieee80211_tasklet_handler(unsigned long data)
{
        struct ieee80211_local *local = (struct ieee80211_local *) data;
        struct sk_buff *skb;
	struct ieee80211_rx_status rx_status;
	struct ieee80211_tx_status tx_status;

	while ((skb = skb_dequeue(&local->skb_queue)) ||
	       (skb = skb_dequeue(&local->skb_queue_unreliable))) {
                switch (skb->pkt_type) {
		case ieee80211_rx_msg:
			/* Make a copy of the RX status because the original
			 * skb may be freed during processing. Clear skb->type
			 * in order to not confuse kernel netstack. */
			memcpy(&rx_status, skb->cb, sizeof(rx_status));
			skb->pkt_type = 0;
			ieee80211_rx(skb->dev, skb, &rx_status);
			break;
		case ieee80211_tx_status_msg:
			/* Make a copy of the TX status because the original
			 * skb may be freed during processing. */
			memcpy(&tx_status, skb->cb, sizeof(tx_status));
			skb->pkt_type = 0;
			if ((tx_status.tx_filtered ||
			     tx_status.excessive_retries) &&
			    skb->len >= sizeof(skb->cb)) {
				/* Restore skb->cb from the copy that was made
				 * in ieee80211_tx_status_irqsafe() */
				memcpy(skb->cb,
				       skb->data + skb->len - sizeof(skb->cb),
				       sizeof(skb->cb));
				skb_trim(skb, skb->len - sizeof(skb->cb));
			}
			ieee80211_tx_status(skb->dev, skb, &tx_status);
			break;
		default: /* should never get here! */
			printk(KERN_ERR "%s: Unknown message type (%d)\n",
			       local->wdev->name, skb->pkt_type);
			dev_kfree_skb(skb);
			break;
                }
        }
}


/* Remove added headers (e.g., QoS control), encryption header/MIC, etc. to
 * make a prepared TX frame (one that has been given to hw) to look like brand
 * new IEEE 802.11 frame that is ready to go through TX processing again. */
static void ieee80211_remove_tx_extra(struct ieee80211_local *local,
				      struct ieee80211_key *key,
				      struct sk_buff *skb)
{
	int hdrlen, iv_len, mic_len;

	if (key == NULL)
		return;

	hdrlen = ieee80211_get_hdrlen_from_skb(skb);

	switch (key->alg) {
	case ALG_WEP:
		iv_len = WEP_IV_LEN;
		mic_len = WEP_ICV_LEN;
		break;
	case ALG_TKIP:
		iv_len = TKIP_IV_LEN;
		mic_len = TKIP_ICV_LEN;
		break;
	case ALG_CCMP:
		iv_len = CCMP_HDR_LEN;
		mic_len = CCMP_MIC_LEN;
		break;
	default:
		return;
	}

	if (skb->len >= mic_len && key->force_sw_encrypt)
		skb_trim(skb, skb->len - mic_len);
	if (skb->len >= iv_len && skb->len > hdrlen) {
		memmove(skb->data + iv_len, skb->data, hdrlen);
		skb_pull(skb, iv_len);
	}

	{
		struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
		u16 fc = le16_to_cpu(hdr->frame_control);
		if ((fc & 0x8C) == 0x88) /* QoS Control Field */ {
			fc &= ~(WLAN_FC_STYPE_QOS_DATA << 4);
			hdr->frame_control = cpu_to_le16(fc);
			memmove(skb->data + 2, skb->data, hdrlen - 2);
			skb_pull(skb, 2);
		}
	}
}


void ieee80211_tx_status(struct net_device *dev, struct sk_buff *skb,
			 struct ieee80211_tx_status *status)
{
	struct sk_buff *skb2;
        struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
        struct ieee80211_local *local = dev->priv;
	struct ieee80211_tx_packet_data *pkt_data =
		(struct ieee80211_tx_packet_data *) skb->cb;
	u16 frag, type;
	u32 msg_type;

	if (!status) {
                printk(KERN_ERR
		       "%s: ieee80211_tx_status called with NULL status\n",
		       dev->name);
		dev_kfree_skb(skb);
		return;
	}

	if (status->excessive_retries) {
		struct sta_info *sta;
		

		sta = sta_info_get(local, hdr->addr1);
		if (sta) {
			if (sta->flags & WLAN_STA_PS) {
				/* The STA is in power save mode, so assume
				 * that this TX packet failed because of that.
				 */
				status->excessive_retries = 0;
				status->tx_filtered = 1;
			}
			sta_info_release(local, sta);
		}
	}

	if (status->tx_filtered) {
		struct sta_info *sta;
		sta = sta_info_get(local, hdr->addr1);
		if (sta) {

			sta->tx_filtered_count++;

			/* Clear the TX filter mask for this STA when sending
			 * the next packet. If the STA went to power save mode,
			 * this will happen when it is waking up for the next
			 * time. */
			sta->clear_dst_mask = 1;

			/* TODO: Is the WLAN_STA_PS flag always set here or is
			 * the race between RX and TX status causing some
			 * packets to be filtered out before 80211.o gets an
			 * update for PS status? This seems to be the case, so
			 * no changes are likely to be needed. */
			if (sta->flags & WLAN_STA_PS &&
			    skb_queue_len(&sta->tx_filtered) <
			    STA_MAX_TX_BUFFER) {
				ieee80211_remove_tx_extra(local, sta->key,
							  skb);
				skb_queue_tail(&sta->tx_filtered, skb);
			} else if (!(sta->flags & WLAN_STA_PS) &&
				   !pkt_data->control.requeue) {
				/* Software retry the packet once */
				pkt_data->control.requeue = 1;
				ieee80211_remove_tx_extra(local, sta->key,
							  skb);
				dev_queue_xmit(skb);
			} else {
				if (net_ratelimit()) {
					printk(KERN_DEBUG "%s: dropped TX "
					       "filtered frame queue_len=%d "
					       "PS=%d @%lu\n",
					       dev->name,
					       skb_queue_len(
						       &sta->tx_filtered),
					       !!(sta->flags & WLAN_STA_PS),
					       jiffies);
				}
				dev_kfree_skb(skb);
			}
			sta_info_release(local, sta);
			return;
		}
	} else {
		rate_control_tx_status(dev, skb, status);
	}

#ifdef IEEE80211_LEDS
        if (local->tx_led_counter && (local->tx_led_counter-- == 1)) {
                ieee80211_tx_led(0, dev);
        }
#endif /* IEEE80211_LEDS */
        /* SNMP counters
	 * Fragments are passed to low-level drivers as separate skbs, so these
	 * are actually fragments, not frames. Update frame counters only for
	 * the first fragment of the frame. */

	frag = WLAN_GET_SEQ_FRAG(le16_to_cpu(hdr->seq_ctrl));
	type = WLAN_FC_GET_TYPE(le16_to_cpu(hdr->frame_control));

        if (status->ack) {
		if (frag == 0) {
			local->dot11TransmittedFrameCount++;
			if (MULTICAST_ADDR(hdr->addr1))
				local->dot11MulticastTransmittedFrameCount++;
			if (status->retry_count > 0)
				local->dot11RetryCount++;
			if (status->retry_count > 1)
				local->dot11MultipleRetryCount++;
		}

		/* This counter shall be incremented for an acknowledged MPDU
		 * with an individual address in the address 1 field or an MPDU
		 * with a multicast address in the address 1 field of type Data
		 * or Management. */
		if (!MULTICAST_ADDR(hdr->addr1) || type == WLAN_FC_TYPE_DATA ||
		    type == WLAN_FC_TYPE_MGMT)
			local->dot11TransmittedFragmentCount++;
        } else {
		if (frag == 0)
			local->dot11FailedCount++;
        }

        if (!status->req_tx_status) {
		dev_kfree_skb(skb);
		return;
	}

	msg_type = status->ack ? ieee80211_msg_tx_callback_ack :
		ieee80211_msg_tx_callback_fail;

	/* skb was the original skb used for TX. Clone it and give the clone
	 * to netif_rx(). Free original skb. */
	skb2 = skb_copy(skb, GFP_ATOMIC);
        if (!skb2) {
		dev_kfree_skb(skb);
		return;
	}
	dev_kfree_skb(skb);
	skb = skb2;

        /* Send frame to hostapd */
        ieee80211_rx_mgmt(dev, skb, NULL, msg_type);
}


/* TODO: implement register/unregister functions for adding TX/RX handlers
 * into ordered list */

static ieee80211_rx_handler ieee80211_rx_handlers[] =
{
	ieee80211_rx_h_parse_qos,
	ieee80211_rx_h_load_stats,
	ieee80211_rx_h_monitor,
	ieee80211_rx_h_passive_scan,
	ieee80211_rx_h_check,
	ieee80211_rx_h_sta_process,
	ieee80211_rx_h_ccmp_decrypt,
	ieee80211_rx_h_tkip_decrypt,
	ieee80211_rx_h_wep_weak_iv_detection,
	ieee80211_rx_h_wep_decrypt,
	ieee80211_rx_h_defragment,
	ieee80211_rx_h_ieee80211_rx_h_ps_poll,
	ieee80211_rx_h_michael_mic_verify,
	/* this must be after decryption - so header is counted in MPDU mic
	 * must be before pae and data, so QOS_DATA format frames
	 * are not passed to user space by these functions
	 */
	ieee80211_rx_h_remove_qos_control,
	ieee80211_rx_h_802_1x_pae,
	ieee80211_rx_h_drop_unencrypted,
	ieee80211_rx_h_data,
	ieee80211_rx_h_mgmt,
	NULL
};

static ieee80211_tx_handler ieee80211_tx_handlers[] =
{
	ieee80211_tx_h_rate_limit,
	ieee80211_tx_h_check_assoc,
	ieee80211_tx_h_ps_buf,
	ieee80211_tx_h_select_key,
	ieee80211_tx_h_michael_mic_add,
	ieee80211_tx_h_fragment,
	ieee80211_tx_h_tkip_encrypt,
	ieee80211_tx_h_ccmp_encrypt,
	ieee80211_tx_h_wep_encrypt,
	ieee80211_tx_h_rate_ctrl,
	ieee80211_tx_h_misc,
	ieee80211_tx_h_load_stats,
	NULL
};


static void ieee80211_if_sdata_init(struct ieee80211_sub_if_data *sdata)
{
	/* Default values for sub-interface parameters */
	sdata->drop_unencrypted = 0;
	sdata->eapol = 1;
}


static struct net_device *ieee80211_if_add(struct net_device *dev,
					   char *name, int locked)
{
	struct net_device *wds_dev = NULL, *tmp_dev;
        struct ieee80211_local *local = dev->priv;
	struct ieee80211_sub_if_data *sdata = NULL, *sdata_parent;
        int alloc_size;
	int ret;
	int i;

	/* ensure 32-bit alignment of our private data and hw private data */
	alloc_size = sizeof(struct net_device) + 3 +
		sizeof(struct ieee80211_sub_if_data) + 3;

        wds_dev = (struct net_device *) kmalloc(alloc_size, GFP_KERNEL);
        if (wds_dev == NULL)
		return NULL;

	memset(wds_dev, 0, alloc_size);
	wds_dev->priv = local;
	ether_setup(wds_dev);
	if (strlen(name) == 0) {
		i = 0;
		do {
			sprintf(wds_dev->name, "%s.%d", dev->name, i++);
			tmp_dev = dev_get_by_name(wds_dev->name);
			if (tmp_dev == NULL)
				break;
			dev_put(tmp_dev);
		} while (i < 10000);
	} else {
                snprintf(wds_dev->name, IFNAMSIZ, "%s", name);
	}

	memcpy(wds_dev->dev_addr, dev->dev_addr, ETH_ALEN);
	wds_dev->hard_start_xmit = ieee80211_subif_start_xmit;
        wds_dev->do_ioctl = ieee80211_ioctl;
	wds_dev->change_mtu = ieee80211_change_mtu;
        wds_dev->tx_timeout = ieee80211_tx_timeout;
        wds_dev->get_stats = ieee80211_get_stats;
        wds_dev->open = ieee80211_open;
	wds_dev->stop = ieee80211_stop;
	wds_dev->base_addr = dev->base_addr;
	wds_dev->irq = dev->irq;
	wds_dev->mem_start = dev->mem_start;
	wds_dev->mem_end = dev->mem_end;
	wds_dev->tx_queue_len = 0;

	sdata = IEEE80211_DEV_TO_SUB_IF(wds_dev);
        sdata->type = IEEE80211_SUB_IF_TYPE_NORM;
        sdata->master = local->mdev;
        sdata->dev = wds_dev;
	sdata->local = local;
	memset(&sdata->stats, 0, sizeof(struct net_device_stats));
	sdata_parent = IEEE80211_DEV_TO_SUB_IF(dev);
	if (sdata_parent->type == IEEE80211_SUB_IF_TYPE_NORM)
		sdata->bss = &sdata_parent->u.norm;
	else {
		printk(KERN_DEBUG "%s: could not set BSS pointer for new "
		       "interface %s\n", dev->name, wds_dev->name);
	}
	ieee80211_if_sdata_init(sdata);

        if (locked)
		ret = register_netdevice(wds_dev);
	else
		ret = register_netdev(wds_dev);
	if (ret) {
		kfree(wds_dev);
		return NULL;
	}

        list_add(&sdata->list, &local->sub_if_list);

        strcpy(name, wds_dev->name);

        return wds_dev;
}


int ieee80211_if_add_wds(struct net_device *dev, char *name,
                         struct ieee80211_if_wds *wds, int locked)
{
        struct net_device *wds_dev = NULL;
	struct ieee80211_sub_if_data *sdata = NULL;

        if (strlen(name) != 0) {
                wds_dev = dev_get_by_name(name);
                if (wds_dev) {
                        dev_put(wds_dev);
                        return -EEXIST;
                }
        }

        wds_dev = ieee80211_if_add(dev, name, locked);
        if (wds_dev == NULL)
                return -ENOANO;

	sdata = IEEE80211_DEV_TO_SUB_IF(wds_dev);
        sdata->type = IEEE80211_SUB_IF_TYPE_WDS;
        memcpy(&sdata->u.wds, wds, sizeof(struct ieee80211_if_wds));

#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
	printk(KERN_DEBUG
	       "%s: Added WDS Link to " MACSTR "\n",
	       wds_dev->name, MAC2STR(sdata->u.wds.remote_addr));
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */


	ieee80211_proc_init_virtual(wds_dev);

        return 0;
}


int ieee80211_if_update_wds(struct net_device *dev, char *name,
			    struct ieee80211_if_wds *wds, int locked)
{
	struct net_device *wds_dev = NULL;
	struct ieee80211_local *local = dev->priv;
	struct ieee80211_sub_if_data *sdata = NULL;
	struct sta_info *sta;
	struct list_head *ptr;

	list_for_each(ptr, &local->sub_if_list) {
		sdata = list_entry(ptr, struct ieee80211_sub_if_data, list);
		if (strcmp(name, sdata->dev->name) == 0) {
			wds_dev = sdata->dev;
			break;
		}
	}

	if (wds_dev == NULL || sdata->type != IEEE80211_SUB_IF_TYPE_WDS)
		return -ENODEV;

	/* Remove STA entry for the old peer */
	sta = sta_info_get(local, sdata->u.wds.remote_addr);
	if (sta) {
		sta_info_release(local, sta);
		sta_info_free(local, sta, 0);
	} else {
		printk(KERN_DEBUG "%s: could not find STA entry for WDS link "
		       "%s peer " MACSTR "\n",
		       dev->name, wds_dev->name,
		       MAC2STR(sdata->u.wds.remote_addr));
	}

	/* Update WDS link data */
        memcpy(&sdata->u.wds, wds, sizeof(struct ieee80211_if_wds));


        return 0;
}


static void ieee80211_if_init(struct net_device *dev)
{
        struct ieee80211_local *local = dev->priv;

        spin_lock_init(&local->sub_if_lock);
        INIT_LIST_HEAD(&local->sub_if_list);

}


int ieee80211_if_add_vlan(struct net_device *dev,
                          char *name,
                          struct ieee80211_if_vlan *vlan,
                          int locked)
{
        struct net_device *vlan_dev = NULL;
        struct ieee80211_sub_if_data *sdata = NULL;

        if (strlen(name) != 0) {
                vlan_dev = dev_get_by_name(name);
                if (vlan_dev) {
                        dev_put(vlan_dev);
                        return -EEXIST;
                }
        }

        vlan_dev = ieee80211_if_add(dev, name, locked);
        if (vlan_dev == NULL)
                return -ENOANO;

	sdata = IEEE80211_DEV_TO_SUB_IF(vlan_dev);
        sdata->type = IEEE80211_SUB_IF_TYPE_VLAN;
	ieee80211_proc_init_virtual(vlan_dev);
        return 0;
}


static void ieee80211_if_norm_init(struct ieee80211_sub_if_data *sdata)
{
	sdata->type = IEEE80211_SUB_IF_TYPE_NORM;
	sdata->u.norm.dtim_period = 2;
	sdata->u.norm.force_unicast_rateidx = -1;
	sdata->u.norm.max_ratectrl_rateidx = -1;
	skb_queue_head_init(&sdata->u.norm.ps_bc_buf);
	sdata->bss = &sdata->u.norm;
}


int ieee80211_if_add_norm(struct net_device *dev, char *name, u8 *bssid,
			  int locked)
{
        struct ieee80211_local *local = dev->priv;
        struct net_device *norm_dev = NULL;
        struct ieee80211_sub_if_data *sdata = NULL;

	if (local->bss_dev_count >= local->conf.bss_count)
		return -ENOBUFS;

        if (strlen(name) != 0) {
                norm_dev = dev_get_by_name(name);
                if (norm_dev) {
                        dev_put(norm_dev);
                        return -EEXIST;
                }
        }

        norm_dev = ieee80211_if_add(dev, name, locked);
        if (norm_dev == NULL)
                return -ENOANO;

	memcpy(norm_dev->dev_addr, bssid, ETH_ALEN);
	sdata = IEEE80211_DEV_TO_SUB_IF(norm_dev);
	ieee80211_if_norm_init(sdata);
	ieee80211_proc_init_virtual(norm_dev);
	spin_lock_bh(&local->sub_if_lock);
	local->bss_devs[local->bss_dev_count] = norm_dev;
	local->bss_dev_count++;
	spin_unlock_bh(&local->sub_if_lock);

        return 0;
}


static void ieee80211_addr_inc(u8 *addr)
{
	int pos = 5;
	while (pos >= 0) {
		addr[pos]++;
		if (addr[pos] != 0)
			break;
		pos--;
	}
}


int ieee80211_if_add_sta(struct net_device *dev, char *name, int locked)
{
	struct ieee80211_local *local = dev->priv;
	struct net_device *sta_dev;
	struct ieee80211_sub_if_data *sdata;
	struct ieee80211_if_sta *ifsta;
	int i;

	if (local->sta_dev_count >= local->conf.bss_count)
		return -ENOBUFS;

	if (strlen(name) != 0) {
		sta_dev = dev_get_by_name(name);
		if (sta_dev) {
			dev_put(sta_dev);
			return -EEXIST;
		}
	}

	sta_dev = ieee80211_if_add(dev, name, locked);
	if (sta_dev == NULL)
		return -ENOANO;

	sdata = IEEE80211_DEV_TO_SUB_IF(sta_dev);
	ifsta = &sdata->u.sta;
	sdata->type = IEEE80211_SUB_IF_TYPE_STA;
	ieee80211_proc_init_virtual(sta_dev);

	spin_lock_bh(&local->sub_if_lock);
	for (i = 0; i < local->conf.bss_count; i++) {
		if (local->sta_devs[i] == NULL) {
			local->sta_devs[i] = sta_dev;
			local->sta_dev_count++;
			printk(KERN_DEBUG "%s: using STA entry %d\n",
			       sta_dev->name, i);
			while (i > 0) {
				ieee80211_addr_inc(sta_dev->dev_addr);
				i--;
			}
			printk(KERN_DEBUG "%s: MAC address " MACSTR "\n",
			       sta_dev->name, MAC2STR(sta_dev->dev_addr));
			break;
		}
	}
	spin_unlock_bh(&local->sub_if_lock);

	init_timer(&ifsta->timer);
	ifsta->timer.data = (unsigned long) sta_dev;
	ifsta->timer.function = ieee80211_sta_timer;

	ifsta->capab = WLAN_CAPABILITY_ESS;
	ifsta->auth_algs = IEEE80211_AUTH_ALG_OPEN |
		IEEE80211_AUTH_ALG_SHARED_KEY;
	ifsta->create_ibss = 1;
	ifsta->wmm_enabled = 1;

	return 0;
}


static void ieee80211_if_del(struct ieee80211_local *local,
			     struct ieee80211_sub_if_data *sdata, int locked)
{
        struct sta_info *sta;
	u8 addr[ETH_ALEN];
	int i, j;
        struct list_head *ptr, *n;

	memset(addr, 0xff, ETH_ALEN);
	for (i = 0; i < NUM_DEFAULT_KEYS; i++) {
		if (!sdata->keys[i])
			continue;
#if 0
		/* Low-level driver has probably disabled hw
		 * already, so there is not really much point
		 * in disabling the keys at this point. */
		if (local->hw->set_key)
			local->hw->set_key(dev, DISABLE_KEY, addr,
					   local->keys[i], 0);
#endif
		kfree(sdata->keys[i]);
	}

	switch (sdata->type) {
	case IEEE80211_SUB_IF_TYPE_NORM:
		/* Remove all virtual interfaces that use this BSS
		 * as their sdata->bss */
		list_for_each_safe(ptr, n, &local->sub_if_list) {
			struct ieee80211_sub_if_data *tsdata =
				list_entry(ptr, struct ieee80211_sub_if_data,
					   list);

			if (tsdata != sdata && tsdata->bss == &sdata->u.norm) {
				printk(KERN_DEBUG "%s: removing virtual "
				       "interface %s because its BSS interface"
				       " is being removed\n",
				       sdata->dev->name, tsdata->dev->name);
				ieee80211_if_del(local, tsdata, locked);
			}
		}

		kfree(sdata->u.norm.beacon_head);
		kfree(sdata->u.norm.beacon_tail);
		spin_lock_bh(&local->sub_if_lock);
		for (j = 0; j < local->bss_dev_count; j++) {
			if (sdata->dev == local->bss_devs[j]) {
				if (j + 1 < local->bss_dev_count) {
					memcpy(&local->bss_devs[j],
					       &local->bss_devs[j + 1],
					       (local->bss_dev_count - j - 1) *
					       sizeof(local->bss_devs[0]));
					local->bss_devs[local->bss_dev_count -
							1] = NULL;
				} else
					local->bss_devs[j] = NULL;
				local->bss_dev_count--;
				break;
			}
		}
		spin_unlock_bh(&local->sub_if_lock);

		if (sdata->dev != local->mdev) {
			struct sk_buff *skb;
			while ((skb = skb_dequeue(&sdata->u.norm.ps_bc_buf))) {
				local->total_ps_buffered--;
				dev_kfree_skb(skb);
			}
		}

		break;
	case IEEE80211_SUB_IF_TYPE_WDS:
		sta = sta_info_get(local, sdata->u.wds.remote_addr);
		if (sta) {
			sta_info_release(local, sta);
			sta_info_free(local, sta, 0);
		} else {
#ifdef CONFIG_IEEE80211_VERBOSE_DEBUG
			printk(KERN_DEBUG "%s: Someone had deleted my STA "
			       "entry for the WDS link\n", sdata->dev->name);
#endif /* CONFIG_IEEE80211_VERBOSE_DEBUG */
		}
		break;
	case IEEE80211_SUB_IF_TYPE_STA:
		del_timer_sync(&sdata->u.sta.timer);
		if (local->scan_timer.data == (unsigned long) sdata->dev)
			del_timer_sync(&local->scan_timer);
		kfree(sdata->u.sta.extra_ie);
		sdata->u.sta.extra_ie = NULL;
		kfree(sdata->u.sta.assocreq_ies);
		sdata->u.sta.assocreq_ies = NULL;
		kfree(sdata->u.sta.assocresp_ies);
		sdata->u.sta.assocresp_ies = NULL;
		if (sdata->u.sta.probe_resp) {
			dev_kfree_skb(sdata->u.sta.probe_resp);
			sdata->u.sta.probe_resp = NULL;
		}
		for (i = 0; i < local->conf.bss_count; i++) {
			if (local->sta_devs[i] == sdata->dev) {
				local->sta_devs[i] = NULL;
				local->sta_dev_count--;
				break;
			}
		}

		break;
	}

	/* remove all STAs that are bound to this virtual interface */
	sta_info_flush(local, sdata->dev);

	list_del(&sdata->list);
	ieee80211_proc_deinit_virtual(sdata->dev);
	if (locked)
		unregister_netdevice(sdata->dev);
	else
		unregister_netdev(sdata->dev);
	/* Default data device and management device are allocated with the
	 * master device. All other devices are separately allocated and will
	 * be freed here. */
	if (sdata->dev != local->mdev && sdata->dev != local->wdev &&
	    sdata->dev != local->apdev)
		kfree(sdata->dev);
}


static int ieee80211_if_remove(struct net_device *dev, char *name, int id,
			       int locked)
{
        struct ieee80211_local *local = dev->priv;
        struct list_head *ptr, *n;

	/* Make sure not to touch sdata->master since it may
	 * have already been deleted, etc. */

	list_for_each_safe(ptr, n, &local->sub_if_list) {
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);

		if (sdata->type == id && strcmp(name, sdata->dev->name) == 0) {
			ieee80211_if_del(local, sdata, locked);
			break;
		}
	}

        return 0;
}


int ieee80211_if_remove_wds(struct net_device *dev, char *name, int locked)
{
        return ieee80211_if_remove(dev, name, IEEE80211_SUB_IF_TYPE_WDS,
				   locked);
}


int ieee80211_if_remove_vlan(struct net_device *dev, char *name, int locked)
{
        return ieee80211_if_remove(dev, name, IEEE80211_SUB_IF_TYPE_VLAN,
				   locked);
}


int ieee80211_if_remove_norm(struct net_device *dev, char *name, int locked)
{
	return ieee80211_if_remove(dev, name, IEEE80211_SUB_IF_TYPE_NORM,
				   locked);
}


int ieee80211_if_remove_sta(struct net_device *dev, char *name, int locked)
{
	return ieee80211_if_remove(dev, name, IEEE80211_SUB_IF_TYPE_STA,
				   locked);
}


int ieee80211_if_flush(struct net_device *dev, int locked)
{
        struct ieee80211_local *local = dev->priv;
        struct list_head *ptr, *n;

	list_for_each_safe(ptr, n, &local->sub_if_list) {
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);

		if (sdata->dev != local->mdev &&
		    sdata->dev != local->wdev &&
		    sdata->dev != local->apdev)
			ieee80211_if_del(local, sdata, locked);
	}

        return 0;
}


static void ieee80211_precalc_rates(struct ieee80211_hw *hw)
{
	struct ieee80211_hw_modes *mode;
	struct ieee80211_rate *rate;
	int m, r;

	for (m = 0; m < hw->num_modes; m++) {
		mode = &hw->modes[m];
		for (r = 0; r < mode->num_rates; r++) {
			rate = &mode->rates[r];
			rate->rate_inv = CHAN_UTIL_RATE_LCM / rate->rate;
		}
	}
}


struct net_device *ieee80211_alloc_hw(size_t priv_data_len,
				      void (*setup)(struct net_device *))
{
	struct net_device *dev, *apdev, *mdev;
        struct ieee80211_local *local;
        struct ieee80211_sub_if_data *sdata;
	int alloc_size;

	/* Ensure 32-bit alignment of our private data and hw private data.
	 * Each net_device is followed by a sub_if_data which which is used
	 * for wds/vlan information; it is aligned as well.
	 *
         * Sample memory map looks something like:
         *
         * 0000 *****************
         *      * net_dev       *
         * 015c *****************
         *      * sub_if        *
         * 017c *****************
         *      * local         *
         * 0b84 *****************
         *      * hw_priv       *
         * 1664 *****************
         *      * ap net_dev    *
         * 17c0 *****************
         *      * sub_if        *
	 *      *****************
	 *      * master net_dev*
	 *      *****************
	 *      * sub_if        *
         *      *****************
         */
        alloc_size = sizeof(struct net_device) +
                sizeof(struct ieee80211_sub_if_data) + 3 +
                sizeof(struct ieee80211_local) + 3 +
                priv_data_len + 3 +
                sizeof(struct net_device) + 3 +
		sizeof(struct ieee80211_sub_if_data) + 3 +
                sizeof(struct net_device) + 3 +
		sizeof(struct ieee80211_sub_if_data) + 3 +
		4096;
        mdev = (struct net_device *) kzalloc(alloc_size, GFP_KERNEL);
	if (mdev == NULL)
		return NULL;

        mdev->priv = (struct net_device *)
                (((long) mdev +
                  sizeof(struct net_device) +
                  sizeof(struct ieee80211_sub_if_data) + 3)
                 & ~3);
	local = mdev->priv;
	local->hw_priv = (void *)
                (((long) local + sizeof(struct ieee80211_local) + 3) & ~3);
	apdev = (struct net_device *)
		(((long) local->hw_priv + priv_data_len + 3) & ~3);
        dev = (struct net_device *)
		(((long) apdev +
		  sizeof(struct net_device) +
		  sizeof(struct ieee80211_sub_if_data) + 3)
                 & ~3);
        dev->priv = local;

	ether_setup(dev);
	memcpy(dev->name, "wlan%d", 7);

        dev->hard_start_xmit = ieee80211_subif_start_xmit;
        dev->do_ioctl = ieee80211_ioctl;
	dev->change_mtu = ieee80211_change_mtu;
        dev->tx_timeout = ieee80211_tx_timeout;
        dev->get_stats = ieee80211_get_stats;
        dev->open = ieee80211_open;
        dev->stop = ieee80211_stop;
	dev->tx_queue_len = 0;
	dev->set_mac_address = ieee80211_set_mac_address;

        local->wdev = dev;
	local->mdev = mdev;
        local->rx_handlers = ieee80211_rx_handlers;
        local->tx_handlers = ieee80211_tx_handlers;

	local->bridge_packets = 1;

	local->rts_threshold = IEEE80211_MAX_RTS_THRESHOLD;
	local->fragmentation_threshold = IEEE80211_MAX_FRAG_THRESHOLD;
	local->short_retry_limit = 7;
	local->long_retry_limit = 4;
	local->conf.calib_int = 60;
	local->rate_ctrl_num_up = RATE_CONTROL_NUM_UP;
	local->rate_ctrl_num_down = RATE_CONTROL_NUM_DOWN;
	local->conf.bss_count = 1;
	memset(local->conf.bssid_mask, 0xff, ETH_ALEN);
	local->bss_devs = kmalloc(sizeof(struct net_device *), GFP_KERNEL);
	if (local->bss_devs == NULL)
		goto fail;
	local->bss_devs[0] = local->wdev;
	local->bss_dev_count = 1;
	local->sta_devs = kmalloc(sizeof(struct net_device *), GFP_KERNEL);
	if (local->sta_devs == NULL)
		goto fail;
	local->sta_devs[0] = NULL;

        local->scan.in_scan = 0;
	local->hw_modes = (unsigned int) -1;

        init_timer(&local->scan.timer); /* clear it out */

        spin_lock_init(&local->generic_lock);
	init_timer(&local->rate_limit_timer);
	local->rate_limit_timer.function = ieee80211_rate_limit;
	local->rate_limit_timer.data = (unsigned long) local;
	init_timer(&local->stat_timer);
	local->stat_timer.function = ieee80211_stat_refresh;
	local->stat_timer.data = (unsigned long) local;
	ieee80211_rx_bss_list_init(dev);

        sta_info_init(local);

        ieee80211_if_init(dev);

        sdata = IEEE80211_DEV_TO_SUB_IF(dev);
        sdata->dev = dev;
        sdata->master = mdev;
        sdata->local = local;
	ieee80211_if_sdata_init(sdata);
	ieee80211_if_norm_init(sdata);
	list_add_tail(&sdata->list, &local->sub_if_list);

	if (strlen(dev->name) + 2 >= sizeof(dev->name))
		goto fail;

        apdev = (struct net_device *)
                (((long) local->hw_priv + priv_data_len + 3) & ~3);
        local->apdev = apdev;
	ether_setup(apdev);
	apdev->priv = local;
	apdev->hard_start_xmit = ieee80211_mgmt_start_xmit;
	apdev->change_mtu = ieee80211_change_mtu_apdev;
	apdev->get_stats = ieee80211_get_stats;
        apdev->open = ieee80211_open;
        apdev->stop = ieee80211_stop;
	apdev->type = ARPHRD_IEEE80211_PRISM;
        apdev->hard_header_parse = header_parse_80211;
	apdev->tx_queue_len = 0;
	sprintf(apdev->name, "%sap", dev->name);

        sdata = IEEE80211_DEV_TO_SUB_IF(apdev);
        sdata->type = IEEE80211_SUB_IF_TYPE_MGMT;
        sdata->dev = apdev;
        sdata->master = mdev;
        sdata->local = local;
        list_add_tail(&sdata->list, &local->sub_if_list);

	ether_setup(mdev);
	mdev->hard_start_xmit = ieee80211_master_start_xmit;
        mdev->do_ioctl = ieee80211_ioctl;
	mdev->change_mtu = ieee80211_change_mtu;
        mdev->tx_timeout = ieee80211_tx_timeout;
        mdev->get_stats = ieee80211_get_stats;
        mdev->open = ieee80211_open;
	mdev->stop = ieee80211_stop;
	mdev->type = ARPHRD_IEEE80211;
        mdev->hard_header_parse = header_parse_80211;
	sprintf(mdev->name, "%s.11", dev->name);

	sdata = IEEE80211_DEV_TO_SUB_IF(mdev);
        sdata->type = IEEE80211_SUB_IF_TYPE_NORM;
        sdata->dev = mdev;
        sdata->master = mdev;
        sdata->local = local;
        list_add_tail(&sdata->list, &local->sub_if_list);

        tasklet_init(&local->tasklet,
		     ieee80211_tasklet_handler,
		     (unsigned long) local);
        skb_queue_head_init(&local->skb_queue);
        skb_queue_head_init(&local->skb_queue_unreliable);

	if (setup)
		setup(mdev);

	return mdev;

 fail:
	ieee80211_free_hw(mdev);
	return NULL;
}

int ieee80211_register_hw(struct net_device *dev, struct ieee80211_hw *hw)
{
        struct ieee80211_local *local = dev->priv;
	int result;

	if (!hw)
		return -1;

	if (hw->version != IEEE80211_VERSION) {
		printk("ieee80211_register_hw - version mismatch: 80211.o "
		       "version %d, low-level driver version %d\n",
		       IEEE80211_VERSION, hw->version);
		return -1;
	}

	local->conf.mode = IW_MODE_MASTER;
	local->conf.beacon_int = 1000;

	ieee80211_update_hw(dev, hw);	/* Don't care about the result. */

	sta_info_start(local);

	result = register_netdev(local->wdev);
	if (result < 0)
		return -1;

	result = register_netdev(local->apdev);
	if (result < 0)
		goto fail_2nd_dev;

	if (hw->fraglist)
		dev->features |= NETIF_F_FRAGLIST;
	result = register_netdev(dev);
	if (result < 0)
		goto fail_3rd_dev;

	if (rate_control_initialize(local) < 0) {
		printk(KERN_DEBUG "%s: Failed to initialize rate control "
		       "algorithm\n", dev->name);
		goto fail_rate;
	}

	/* TODO: add rtnl locking around device creation and qdisc install */
	ieee80211_install_qdisc(dev);

        ieee80211_wep_init(local);
	ieee80211_proc_init_interface(local);
	return 0;

fail_rate:
	unregister_netdev(dev);
fail_3rd_dev:
	unregister_netdev(local->apdev);
fail_2nd_dev:
	unregister_netdev(local->wdev);
	sta_info_stop(local);
	return result;
}

int ieee80211_update_hw(struct net_device *dev, struct ieee80211_hw *hw)
{
        struct ieee80211_local *local = dev->priv;

	local->hw = hw;

	/* Backwards compatibility for low-level drivers that do not set number
	 * of TX queues. */
	if (hw->queues == 0)
		hw->queues = 1;

	memcpy(local->apdev->dev_addr, dev->dev_addr, ETH_ALEN);
	local->apdev->base_addr = dev->base_addr;
	local->apdev->irq = dev->irq;
	local->apdev->mem_start = dev->mem_start;
	local->apdev->mem_end = dev->mem_end;

        memcpy(local->wdev->dev_addr, dev->dev_addr, ETH_ALEN);
	local->wdev->base_addr = dev->base_addr;
	local->wdev->irq = dev->irq;
	local->wdev->mem_start = dev->mem_start;
	local->wdev->mem_end = dev->mem_end;

	if (!hw->modes || !hw->modes->channels || !hw->modes->rates ||
	    !hw->modes->num_channels || !hw->modes->num_rates)
		return -1;

	ieee80211_precalc_rates(hw);
	local->conf.phymode = hw->modes[0].mode;
	local->curr_rates = hw->modes[0].rates;
	local->num_curr_rates = hw->modes[0].num_rates;
	ieee80211_prepare_rates(dev);

	local->conf.freq = local->hw->modes[0].channels[0].freq;
	local->conf.channel = local->hw->modes[0].channels[0].chan;
	local->conf.channel_val = local->hw->modes[0].channels[0].val;
	/* FIXME: Invoke config to allow driver to set the channel. */

	return 0;
}

void ieee80211_unregister_hw(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
        struct list_head *ptr, *n;
	int i;

        tasklet_disable(&local->tasklet);
        /* TODO: skb_queue should be empty here, no need to do anything? */

	if (local->rate_limit)
		del_timer_sync(&local->rate_limit_timer);
	if (local->stat_time)
		del_timer_sync(&local->stat_timer);
	if (local->scan_timer.data)
		del_timer_sync(&local->scan_timer);
	ieee80211_rx_bss_list_deinit(dev);

	list_for_each_safe(ptr, n, &local->sub_if_list) {
		struct ieee80211_sub_if_data *sdata =
			list_entry(ptr, struct ieee80211_sub_if_data, list);
		ieee80211_if_del(local, sdata, 0);
	}

	sta_info_stop(local);

	for (i = 0; i < IEEE80211_FRAGMENT_MAX; i++)
		if (local->fragments[i].skb)
			dev_kfree_skb(local->fragments[i].skb);

	for (i = 0; i < NUM_IEEE80211_MODES; i++) {
		kfree(local->supp_rates[i]);
		kfree(local->basic_rates[i]);
        }

	kfree(local->conf.ssid);
	kfree(local->conf.generic_elem);

	ieee80211_proc_deinit_interface(local);

	skb_queue_purge(&local->skb_queue);
	skb_queue_purge(&local->skb_queue_unreliable);

	rate_control_free(local);
}

void ieee80211_free_hw(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;

	kfree(local->sta_devs);
	kfree(local->bss_devs);
	kfree(dev);
}

/* Perform netif operations on all configured interfaces */
int ieee80211_netif_oper(struct net_device *sdev, Netif_Oper op)
{
        struct ieee80211_local *local = sdev->priv;
        struct ieee80211_sub_if_data *sdata =  IEEE80211_DEV_TO_SUB_IF(sdev);
        struct net_device *dev = sdata->master;

        switch (op) {
        case NETIF_ATTACH:
            netif_device_attach(dev);
            break;
        case NETIF_DETACH:
            netif_device_detach(dev);
            break;
        case NETIF_START:
            netif_start_queue(dev);
            break;
        case NETIF_STOP:
            break;
        case NETIF_WAKE:
		if (local->scan.in_scan == 0) {
			netif_wake_queue(dev);
#if 1
			if (/* FIX: 802.11 qdisc in use */ 1)
				__netif_schedule(dev);
#endif
		}
            break;
        case NETIF_IS_STOPPED:
            if (netif_queue_stopped(dev))
                return 1;
            break;
        case NETIF_UPDATE_TX_START:
            dev->trans_start = jiffies;
            break;
        }

        return 0;
}


void * ieee80211_dev_hw_data(struct net_device *dev)
{
	struct ieee80211_local *local = dev->priv;
	return local->hw_priv;
}


void * ieee80211_dev_stats(struct net_device *dev)
{
	struct ieee80211_sub_if_data *sdata;
	sdata = IEEE80211_DEV_TO_SUB_IF(dev);
	return &(sdata->stats);
}


int ieee80211_rate_control_register(struct rate_control_ops *ops)
{
	struct rate_control_algs *alg;

	alg = kmalloc(sizeof(*alg), GFP_KERNEL);
	if (alg == NULL) {
		return -1;
	}
	memset(alg, 0, sizeof(*alg));
	alg->next = ieee80211_rate_ctrl_algs;
	alg->ops = ops;
	ieee80211_rate_ctrl_algs = alg;

	return 0;
}


void ieee80211_rate_control_unregister(struct rate_control_ops *ops)
{
	struct rate_control_algs *alg, *prev;

	prev = NULL;
	alg = ieee80211_rate_ctrl_algs;
	while (alg) {
		if (alg->ops == ops) {
			if (prev)
				prev->next = alg->next;
			else
				ieee80211_rate_ctrl_algs = alg->next;
			kfree(alg);
			break;
		}
		prev = alg;
		alg = alg->next;
	}
}


static int rate_control_initialize(struct ieee80211_local *local)
{
	struct rate_control_algs *algs;
	for (algs = ieee80211_rate_ctrl_algs; algs; algs = algs->next) {
		local->rate_ctrl = algs->ops;
		local->rate_ctrl_priv = rate_control_alloc(local);
		if (local->rate_ctrl_priv) {
			printk(KERN_DEBUG "%s: Selected rate control "
			       "algorithm '%s'\n", local->wdev->name,
			       local->rate_ctrl->name);
			return 0;
		}
	}

	printk(KERN_WARNING "%s: Failed to select rate control algorithm\n",
	       local->wdev->name);
	return -1;
}


static int __init ieee80211_init(void)
{
	struct sk_buff *skb;
	if (sizeof(struct ieee80211_tx_packet_data) > (sizeof(skb->cb))) {
		printk("80211: ieee80211_tx_packet_data is bigger "
		       "than the skb->cb (%d > %d)\n",
		       (int) sizeof(struct ieee80211_tx_packet_data),
		       (int) sizeof(skb->cb));
		return -EINVAL;
	}
	if (sizeof(struct ieee80211_rx_status) > sizeof(skb->cb)) {
		printk("80211: ieee80211_rx_status is bigger "
		       "than the skb->cb (%d > %d)\n",
		       (int) sizeof(struct ieee80211_rx_status),
		       (int) sizeof(skb->cb));
		return -EINVAL;
	}

	ieee80211_proc_init();
	{
		int ret = ieee80211_wme_register();
		if (ret) {
			printk(KERN_DEBUG "ieee80211_init: failed to "
			       "initialize WME (err=%d)\n", ret);
			ieee80211_proc_deinit();
			return ret;
		}
	}

	return 0;
}


static void __exit ieee80211_exit(void)
{
	ieee80211_wme_unregister();
	ieee80211_proc_deinit();
}


EXPORT_SYMBOL(ieee80211_alloc_hw);
EXPORT_SYMBOL(ieee80211_register_hw);
EXPORT_SYMBOL(ieee80211_update_hw);
EXPORT_SYMBOL(ieee80211_unregister_hw);
EXPORT_SYMBOL(ieee80211_free_hw);
EXPORT_SYMBOL(ieee80211_rx);
EXPORT_SYMBOL(ieee80211_tx_status);
EXPORT_SYMBOL(ieee80211_beacon_get);
EXPORT_SYMBOL(ieee80211_get_buffered_bc);
EXPORT_SYMBOL(ieee80211_netif_oper);
EXPORT_SYMBOL(ieee80211_dev_hw_data);
EXPORT_SYMBOL(ieee80211_dev_stats);
EXPORT_SYMBOL(ieee80211_get_hw_conf);
EXPORT_SYMBOL(ieee80211_set_aid_for_sta);
EXPORT_SYMBOL(ieee80211_rx_irqsafe);
EXPORT_SYMBOL(ieee80211_tx_status_irqsafe);
EXPORT_SYMBOL(ieee80211_get_hdrlen);
EXPORT_SYMBOL(ieee80211_get_hdrlen_from_skb);
EXPORT_SYMBOL(ieee80211_rate_control_register);
EXPORT_SYMBOL(ieee80211_rate_control_unregister);
EXPORT_SYMBOL(sta_info_get);
EXPORT_SYMBOL(sta_info_release);
EXPORT_SYMBOL(ieee80211_radar_status);

module_init(ieee80211_init);
module_exit(ieee80211_exit);
