/*
 * Copyright (c) 2008 Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* mac80211 and PCI callbacks */

#include <linux/nl80211.h>
#include "core.h"

#define ATH_PCI_VERSION "0.1"

#define IEEE80211_HTCAP_MAXRXAMPDU_FACTOR	13
#define IEEE80211_ACTION_CAT_HT			7
#define IEEE80211_ACTION_HT_TXCHWIDTH		0

static char *dev_info = "ath9k";

MODULE_AUTHOR("Atheros Communications");
MODULE_DESCRIPTION("Support for Atheros 802.11n wireless LAN cards.");
MODULE_SUPPORTED_DEVICE("Atheros 802.11n WLAN cards");
MODULE_LICENSE("Dual BSD/GPL");

static struct pci_device_id ath_pci_id_table[] __devinitdata = {
	{ PCI_VDEVICE(ATHEROS, 0x0023) }, /* PCI   */
	{ PCI_VDEVICE(ATHEROS, 0x0024) }, /* PCI-E */
	{ PCI_VDEVICE(ATHEROS, 0x0027) }, /* PCI   */
	{ PCI_VDEVICE(ATHEROS, 0x0029) }, /* PCI   */
	{ PCI_VDEVICE(ATHEROS, 0x002A) }, /* PCI-E */
	{ 0 }
};

static int test_update_chan(enum ieee80211_band band,
			    const struct hal_channel *chan,
			    struct ath_softc *sc)
{
	int i;

	for (i = 0; i < sc->sbands[band].n_channels; i++) {
		if (sc->channels[band][i].center_freq == chan->channel)
			return 1;
	}

	return 0;
}

static int ath_check_chanflags(struct ieee80211_channel *chan,
			       u_int32_t mode,
			       struct ath_softc *sc)
{
	struct ieee80211_hw *hw = sc->hw;
	struct ieee80211_supported_band *band;
	struct ieee80211_channel *band_channel;
	int i;

	band = hw->wiphy->bands[chan->band];

	for (i = 0; i < band->n_channels; i++) {
		band_channel = &band->channels[i];

		if ((band_channel->center_freq == chan->center_freq) &&
		    ((band_channel->hw_value & mode) == mode))
			return 1;
	}
	return 0;
}

static int ath_setkey_tkip(struct ath_softc *sc,
			   struct ieee80211_key_conf *key,
			   struct hal_keyval *hk,
			   const u8 *addr)
{
	u8 *key_rxmic = NULL;
	u8 *key_txmic = NULL;

	key_txmic = key->key + NL80211_TKIP_DATA_OFFSET_TX_MIC_KEY;
	key_rxmic = key->key + NL80211_TKIP_DATA_OFFSET_RX_MIC_KEY;

	if (addr == NULL) {
		/* Group key installation */
		memcpy(hk->kv_mic,  key_rxmic, sizeof(hk->kv_mic));
		return ath_keyset(sc, key->keyidx, hk, addr);
	}
	if (!sc->sc_splitmic) {
		/*
		 * data key goes at first index,
		 * the hal handles the MIC keys at index+64.
		 */
		memcpy(hk->kv_mic, key_rxmic, sizeof(hk->kv_mic));
		memcpy(hk->kv_txmic, key_txmic, sizeof(hk->kv_txmic));
		return ath_keyset(sc, key->keyidx, hk, addr);
	}
	/*
	 * TX key goes at first index, RX key at +32.
	 * The hal handles the MIC keys at index+64.
	 */
	memcpy(hk->kv_mic, key_txmic, sizeof(hk->kv_mic));
	if (!ath_keyset(sc, key->keyidx, hk, NULL)) {
		/* Txmic entry failed. No need to proceed further */
		DPRINTF(sc, ATH_DEBUG_KEYCACHE,
			"%s Setting TX MIC Key Failed\n", __func__);
		return 0;
	}

	memcpy(hk->kv_mic, key_rxmic, sizeof(hk->kv_mic));
	/* XXX delete tx key on failure? */
	return ath_keyset(sc, key->keyidx+32, hk, addr);
}

static int ath_key_config(struct ath_softc *sc,
			  const u8 *addr,
			  struct ieee80211_key_conf *key)
{
	struct ieee80211_vif *vif;
	struct hal_keyval hk;
	const u8 *mac = NULL;
	int ret = 0;
	enum ieee80211_if_types opmode;

	memset(&hk, 0, sizeof(hk));

	switch (key->alg) {
	case ALG_WEP:
		hk.kv_type = HAL_CIPHER_WEP;
		break;
	case ALG_TKIP:
		hk.kv_type = HAL_CIPHER_TKIP;
		break;
	case ALG_CCMP:
		hk.kv_type = HAL_CIPHER_AES_CCM;
		break;
	default:
		return -EINVAL;
	}

	hk.kv_len  = key->keylen;
	memcpy(hk.kv_val, key->key, key->keylen);

	if (!sc->sc_vaps[0])
		return -EIO;

	vif = sc->sc_vaps[0]->av_if_data;
	opmode = vif->type;

	/*
	 *  Strategy:
	 *   For _M_STA mc tx, we will not setup a key at all since we never
	 *   tx mc.
	 *   _M_STA mc rx, we will use the keyID.
	 *   for _M_IBSS mc tx, we will use the keyID, and no macaddr.
	 *   for _M_IBSS mc rx, we will alloc a slot and plumb the mac of the
	 *   peer node. BUT we will plumb a cleartext key so that we can do
	 *   perSta default key table lookup in software.
	 */
	if (is_broadcast_ether_addr(addr)) {
		switch (opmode) {
		case IEEE80211_IF_TYPE_STA:
			/* default key:  could be group WPA key
			 * or could be static WEP key */
			mac = NULL;
			break;
		case IEEE80211_IF_TYPE_IBSS:
			break;
		case IEEE80211_IF_TYPE_AP:
			break;
		default:
			ASSERT(0);
			break;
		}
	} else {
		mac = addr;
	}

	if (key->alg == ALG_TKIP)
		ret = ath_setkey_tkip(sc, key, &hk, mac);
	else
		ret = ath_keyset(sc, key->keyidx, &hk, mac);

	if (!ret)
		return -EIO;

	sc->sc_keytype = hk.kv_type;
	return 0;
}

static void ath_key_delete(struct ath_softc *sc, struct ieee80211_key_conf *key)
{
#define ATH_MAX_NUM_KEYS 4
	int freeslot;

	freeslot = (key->keyidx >= ATH_MAX_NUM_KEYS) ? 1 : 0;
	ath_key_reset(sc, key->keyidx, freeslot);
#undef ATH_MAX_NUM_KEYS
}

static void setup_ht_cap(struct ieee80211_ht_info *ht_info)
{
/* Until mac80211 includes these fields */

#define IEEE80211_HT_CAP_DSSSCCK40 0x1000
#define	IEEE80211_HT_CAP_MAXRXAMPDU_65536 0x3   /* 2 ^ 16 */
#define	IEEE80211_HT_CAP_MPDUDENSITY_8 0x6     	/* 8 usec */

	ht_info->ht_supported = 1;
	ht_info->cap = (u16)IEEE80211_HT_CAP_SUP_WIDTH
			|(u16)IEEE80211_HT_CAP_MIMO_PS
			|(u16)IEEE80211_HT_CAP_SGI_40
			|(u16)IEEE80211_HT_CAP_DSSSCCK40;

	ht_info->ampdu_factor = IEEE80211_HT_CAP_MAXRXAMPDU_65536;
	ht_info->ampdu_density = IEEE80211_HT_CAP_MPDUDENSITY_8;
	/* setup supported mcs set */
	memset(ht_info->supp_mcs_set, 0, 16);
	ht_info->supp_mcs_set[0] = 0xff;
	ht_info->supp_mcs_set[1] = 0xff;
	ht_info->supp_mcs_set[12] = IEEE80211_HT_CAP_MCS_TX_DEFINED;
}

static int ath_rate2idx(struct ath_softc *sc, int rate)
{
	int i = 0, cur_band, n_rates;
	struct ieee80211_hw *hw = sc->hw;

	cur_band = hw->conf.channel->band;
	n_rates = sc->sbands[cur_band].n_bitrates;

	for (i = 0; i < n_rates; i++) {
		if (sc->sbands[cur_band].bitrates[i].bitrate == rate)
			break;
	}

	/*
	 * NB:mac80211 validates rx rate index against the supported legacy rate
	 * index only (should be done against ht rates also), return the highest
	 * legacy rate index for rx rate which does not match any one of the
	 * supported basic and extended rates to make mac80211 happy.
	 * The following hack will be cleaned up once the issue with
	 * the rx rate index validation in mac80211 is fixed.
	 */
	if (i == n_rates)
		return n_rates - 1;
	return i;
}

static void ath9k_rx_prepare(struct ath_softc *sc,
			     struct sk_buff *skb,
			     struct ath_recv_status *status,
			     struct ieee80211_rx_status *rx_status)
{
	struct ieee80211_hw *hw = sc->hw;
	struct ieee80211_channel *curchan = hw->conf.channel;

	memset(rx_status, 0, sizeof(struct ieee80211_rx_status));

	rx_status->mactime = status->tsf;
	rx_status->band = curchan->band;
	rx_status->freq =  curchan->center_freq;
	rx_status->signal = (status->rssi * 64) / 100;
	rx_status->noise = ATH_DEFAULT_NOISE_FLOOR;
	rx_status->rate_idx = ath_rate2idx(sc, (status->rateKbps / 100));
	rx_status->antenna = status->antenna;

	if (status->flags & ATH_RX_MIC_ERROR)
		rx_status->flag |= RX_FLAG_MMIC_ERROR;
	if (status->flags & ATH_RX_FCS_ERROR)
		rx_status->flag |= RX_FLAG_FAILED_FCS_CRC;

	rx_status->flag |= RX_FLAG_TSFT;
}

/*
 * Update all associated nodes and VAPs
 *
 * Called when local channel width changed.  e.g. if AP mode,
 * update all associated STAs when the AP's channel width changes.
 */
static void cwm_rate_updateallnodes(struct ath_softc *sc)
{
	int flags = 0, error;
	struct ieee80211_vif *vif;
	enum ieee80211_if_types opmode;
	struct ieee80211_hw *hw = sc->hw;

	if (sc->sc_vaps[0]) {
		vif = sc->sc_vaps[0]->av_if_data;
		opmode = vif->type;
		switch (opmode) {
		case IEEE80211_IF_TYPE_STA:
			/* sync with next received beacon */
			flags |= ATH_IF_BEACON_SYNC;
			if (hw->conf.ht_conf.ht_supported)
				flags |= ATH_IF_HT;
			error = ath_vap_up(sc, 0,
			   /* sc->sc_vaps[i]->av_btxctl->if_id,  FIX ME if_id */
			   /* sc->sc_vaps[i]->bssid, FIX ME bssid */
					   sc->sc_curbssid,
					   sc->sc_curaid,
					   flags);
			if (error)/* FIX ME if_id */
				DPRINTF(sc, ATH_DEBUG_CWM,
					"%s: Unable to up vap: "
					"%d\n", __func__, 0);
			else
				DPRINTF(sc, ATH_DEBUG_CWM,
					"%s: VAP up for id: "
					"%d\n", __func__, 0);
			break;
		case IEEE80211_IF_TYPE_IBSS:
		case IEEE80211_IF_TYPE_AP:
			/* FIXME */
			break;
		default:
			break;
		}
	}
}

/* Action: switch MAC from 40 to 20  (OR) 20 to 40 based on ch_width arg */
static void cwm_action_mac_change_chwidth(struct ath_softc *sc,
				   enum hal_ht_macmode ch_width)
{
	ath_set_macmode(sc, ch_width);

	/* notify rate control of new mode (select new rate table) */
	cwm_rate_updateallnodes(sc);

	/* XXX: all virtual APs - send ch width action management frame */
}

static u_int8_t parse_mpdudensity(u_int8_t mpdudensity)
{
	/*
	 * 802.11n D2.0 defined values for "Minimum MPDU Start Spacing":
	 *   0 for no restriction
	 *   1 for 1/4 us
	 *   2 for 1/2 us
	 *   3 for 1 us
	 *   4 for 2 us
	 *   5 for 4 us
	 *   6 for 8 us
	 *   7 for 16 us
	 */
	switch (mpdudensity) {
	case 0:
		return 0;
	case 1:
	case 2:
	case 3:
		/* Our lower layer calculations limit our precision to
		   1 microsecond */
		return 1;
	case 4:
		return 2;
	case 5:
		return 4;
	case 6:
		return 8;
	case 7:
		return 16;
	default:
		return 0;
	}
}

static int ath9k_start(struct ieee80211_hw *hw)
{
	struct ath_softc *sc = hw->priv;
	struct ieee80211_channel *curchan = hw->conf.channel;
	struct hal_channel hchan;
	int error = 0;

	DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Starting driver with "
		"initial channel: %d MHz\n", __func__, curchan->center_freq);

	/* setup initial channel */

	hchan.channel = curchan->center_freq;
	hchan.channelFlags = ath_chan2flags(curchan, sc);

	/* open ath_dev */
	error = ath_open(sc, &hchan);
	if (error) {
		DPRINTF(sc, ATH_DEBUG_FATAL,
			"%s: Unable to complete ath_open\n", __func__);
		return error;
	}

	ieee80211_wake_queues(hw);
	return 0;
}

static int ath9k_tx(struct ieee80211_hw *hw,
		    struct sk_buff *skb)
{
	struct ath_softc *sc = hw->priv;
	int hdrlen, padsize;

	/* Add the padding after the header if this is not already done */
	hdrlen = ieee80211_get_hdrlen_from_skb(skb);
	if (hdrlen & 3) {
		padsize = hdrlen % 4;
		if (skb_headroom(skb) < padsize)
			return -1;
		skb_push(skb, padsize);
		memmove(skb->data, skb->data + padsize, hdrlen);
	}

	DPRINTF(sc, ATH_DEBUG_XMIT, "%s: transmitting packet, skb: %p\n",
		__func__,
		skb);

	if (ath_tx_start(sc, skb) != 0) {
		DPRINTF(sc, ATH_DEBUG_XMIT, "%s: TX failed\n", __func__);
		dev_kfree_skb_any(skb);
		/* FIXME: Check for proper return value from ATH_DEV */
		return 0;
	}

	return 0;
}

static int ath9k_beacon_update(struct ieee80211_hw *hw,
			       struct sk_buff *skb)

{
	struct ath_softc *sc = hw->priv;

	DPRINTF(sc, ATH_DEBUG_BEACON, "%s: Update Beacon\n", __func__);
	return ath9k_tx(hw, skb);
}

static void ath9k_stop(struct ieee80211_hw *hw)
{
	struct ath_softc *sc = hw->priv;
	int error;

	DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Driver halt\n", __func__);

	error = ath_suspend(sc);
	if (error)
		DPRINTF(sc, ATH_DEBUG_CONFIG,
			"%s: Device is no longer present\n", __func__);

	ieee80211_stop_queues(hw);
}

static int ath9k_add_interface(struct ieee80211_hw *hw,
			       struct ieee80211_if_init_conf *conf)
{
	struct ath_softc *sc = hw->priv;
	int error, ic_opmode = 0;

	/* Support only vap for now */

	if (sc->sc_nvaps)
		return -1;

	switch (conf->type) {
	case IEEE80211_IF_TYPE_STA:
		ic_opmode = HAL_M_STA;
	default:
		break;
	}

	DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Attach a VAP of type: %d\n",
		__func__,
		ic_opmode);

	error = ath_vap_attach(sc, 0, conf->vif, ic_opmode, ic_opmode, 0);
	if (error) {
		DPRINTF(sc, ATH_DEBUG_FATAL,
			"%s: Unable to attach vap, error: %d\n",
			__func__, error);
		goto bad;
	}

	return 0;
bad:
	return -1;
}

static void ath9k_remove_interface(struct ieee80211_hw *hw,
				   struct ieee80211_if_init_conf *conf)
{
	struct ath_softc *sc = hw->priv;
	int error, flags = 0;

	DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Detach VAP\n", __func__);

	flags |= ATH_IF_HW_OFF;

	error = ath_vap_down(sc, 0, flags);
	if (error)
		DPRINTF(sc, ATH_DEBUG_FATAL,
			"%s: Unable to down vap, error: %d\n", __func__, error);

	error = ath_vap_detach(sc, 0);
	if (error)
		DPRINTF(sc, ATH_DEBUG_FATAL,
			"%s: Unable to detach vap, error: %d\n",
			__func__, error);
}

static int ath9k_config(struct ieee80211_hw *hw,
			struct ieee80211_conf *conf)
{
	struct ath_softc *sc = hw->priv;
	struct ieee80211_channel *curchan = hw->conf.channel;
	struct hal_channel hchan;

	DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Set channel: %d MHz\n",
		__func__,
		curchan->center_freq);

	hchan.channel = curchan->center_freq;
	hchan.channelFlags = ath_chan2flags(curchan, sc);
	sc->sc_config.txpowlimit = 2 * conf->power_level;

	/* set h/w channel */
	if (ath_set_channel(sc, &hchan) < 0)
		DPRINTF(sc, ATH_DEBUG_FATAL, "%s: Unable to set channel\n",
			__func__);

	return 0;
}

static int ath9k_config_interface(struct ieee80211_hw *hw,
				  struct ieee80211_vif *vif,
				  struct ieee80211_if_conf *conf)
{
	struct ath_softc *sc = hw->priv;
	int error = 0, flags = 0;
	struct sk_buff *beacon;

	if (!conf->bssid)
		return 0;

	switch (vif->type) {
	case IEEE80211_IF_TYPE_STA:
		/* XXX: Handle (conf->changed & IEEE80211_IFCC_SSID) */
		flags |= ATH_IF_HW_ON;
		/* sync with next received beacon */
		flags |= ATH_IF_BEACON_SYNC;

		DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Bring up VAP: %d\n",
			__func__, 0);

		error = ath_vap_up(sc, 0, conf->bssid, 0, flags);
		if (error) {
			DPRINTF(sc, ATH_DEBUG_FATAL,
			"%s: Unable to bring up VAP: %d, error: %d\n",
			__func__, 0, error);
			return -1;
		}

		break;
	case IEEE80211_IF_TYPE_IBSS:
		if (!(conf->changed & IEEE80211_IFCC_BEACON))
			break;
		beacon = ieee80211_beacon_get(hw, vif);
		if (!beacon)
			return -ENOMEM;
		ath9k_beacon_update(hw, beacon);
	default:
		break;
	}

	return 0;
}

#define SUPPORTED_FILTERS			\
	(FIF_PROMISC_IN_BSS |			\
	FIF_ALLMULTI |				\
	FIF_CONTROL |				\
	FIF_OTHER_BSS |				\
	FIF_BCN_PRBRESP_PROMISC |		\
	FIF_FCSFAIL)

/* Accept unicast, bcast and mcast frames */

static void ath9k_configure_filter(struct ieee80211_hw *hw,
				   unsigned int changed_flags,
				   unsigned int *total_flags,
				   int mc_count,
				   struct dev_mc_list *mclist)
{
	struct ath_softc *sc = hw->priv;

	changed_flags &= SUPPORTED_FILTERS;
	*total_flags &= SUPPORTED_FILTERS;

	if (changed_flags & FIF_BCN_PRBRESP_PROMISC) {
		if (*total_flags & FIF_BCN_PRBRESP_PROMISC)
			ath_scan_start(sc);
		else
			ath_scan_end(sc);
	}
}

static void ath9k_sta_notify(struct ieee80211_hw *hw,
			     struct ieee80211_vif *vif,
			     enum sta_notify_cmd cmd,
			     const u8 *addr)
{
	struct ath_softc *sc = hw->priv;
	struct ath_node *an;
	unsigned long flags;
	DECLARE_MAC_BUF(mac);

	spin_lock_irqsave(&sc->node_lock, flags);
	an = ath_node_find(sc, (u8 *) addr);
	spin_unlock_irqrestore(&sc->node_lock, flags);

	switch (cmd) {
	case STA_NOTIFY_ADD:
		spin_lock_irqsave(&sc->node_lock, flags);
		if (!an) {
			ath_node_attach(sc, (u8 *)addr, 0);
			DPRINTF(sc, ATH_DEBUG_NODE, "%s: Attach a node: %s\n",
				__func__,
				print_mac(mac, addr));
		} else {
			ath_node_get(sc, (u8 *)addr);
		}
		spin_unlock_irqrestore(&sc->node_lock, flags);
		break;
	case STA_NOTIFY_REMOVE:
		if (!an)
			DPRINTF(sc, ATH_DEBUG_FATAL,
				"%s: Removal of a non-existent node\n",
				__func__);
		else {
			ath_node_put(sc, an, ATH9K_BH_STATUS_INTACT);
			DPRINTF(sc, ATH_DEBUG_NODE, "%s: Put a node: %s\n",
				__func__,
				print_mac(mac, addr));
		}
		break;
	default:
		break;
	}
}

static int ath9k_conf_tx(struct ieee80211_hw *hw,
			 u16 queue,
			 const struct ieee80211_tx_queue_params *params)
{
	struct ath_softc *sc = hw->priv;
	struct hal_txq_info qi;
	int ret = 0, qnum;

	if (queue >= WME_NUM_AC)
		return 0;

	qi.tqi_aifs = params->aifs;
	qi.tqi_cwmin = params->cw_min;
	qi.tqi_cwmax = params->cw_max;
	qi.tqi_burstTime = params->txop;
	qnum = ath_get_hal_qnum(queue, sc);

	DPRINTF(sc, ATH_DEBUG_CONFIG,
		"%s: Configure tx [queue/halq] [%d/%d],  "
		"aifs: %d, cw_min: %d, cw_max: %d, txop: %d\n",
		__func__,
		queue,
		qnum,
		params->aifs,
		params->cw_min,
		params->cw_max,
		params->txop);

	ret = ath_txq_update(sc, qnum, &qi);
	if (ret)
		DPRINTF(sc, ATH_DEBUG_FATAL,
			"%s: TXQ Update failed\n", __func__);

	return ret;
}

static int ath9k_set_key(struct ieee80211_hw *hw,
			 enum set_key_cmd cmd,
			 const u8 *local_addr,
			 const u8 *addr,
			 struct ieee80211_key_conf *key)
{
	struct ath_softc *sc = hw->priv;
	int ret = 0;

	DPRINTF(sc, ATH_DEBUG_KEYCACHE, " %s: Set HW Key\n", __func__);

	switch (cmd) {
	case SET_KEY:
		ret = ath_key_config(sc, addr, key);
		if (!ret) {
			set_bit(key->keyidx, sc->sc_keymap);
			key->hw_key_idx = key->keyidx;
			/* push IV and Michael MIC generation to stack */
			key->flags |= IEEE80211_KEY_FLAG_GENERATE_IV;
			key->flags |= IEEE80211_KEY_FLAG_GENERATE_MMIC;
		}
		break;
	case DISABLE_KEY:
		ath_key_delete(sc, key);
		clear_bit(key->keyidx, sc->sc_keymap);
		sc->sc_keytype = HAL_CIPHER_CLR;
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static void ath9k_ht_conf(struct ath_softc *sc,
			  struct ieee80211_bss_conf *bss_conf)
{
#define IEEE80211_HT_CAP_40MHZ_INTOLERANT BIT(14)
	struct ath_ht_info *ht_info = &sc->sc_ht_info;

	if (bss_conf->assoc_ht) {
		ht_info->ext_chan_offset =
			bss_conf->ht_bss_conf->bss_cap &
				IEEE80211_HT_IE_CHA_SEC_OFFSET;

		if (!(bss_conf->ht_conf->cap &
			IEEE80211_HT_CAP_40MHZ_INTOLERANT) &&
			    (bss_conf->ht_bss_conf->bss_cap &
				IEEE80211_HT_IE_CHA_WIDTH))
			ht_info->tx_chan_width = HAL_HT_MACMODE_2040;
		else
			ht_info->tx_chan_width = HAL_HT_MACMODE_20;

		cwm_action_mac_change_chwidth(sc, ht_info->tx_chan_width);
		ht_info->maxampdu = 1 << (IEEE80211_HTCAP_MAXRXAMPDU_FACTOR +
					bss_conf->ht_conf->ampdu_factor);
		ht_info->mpdudensity =
			parse_mpdudensity(bss_conf->ht_conf->ampdu_density);

	}

#undef IEEE80211_HT_CAP_40MHZ_INTOLERANT
}

static void ath9k_bss_assoc_info(struct ath_softc *sc,
				 struct ieee80211_bss_conf *bss_conf)
{
	struct ieee80211_hw *hw = sc->hw;
	struct ieee80211_channel *curchan = hw->conf.channel;
	struct hal_channel hchan;

	if (bss_conf->assoc) {
		/* FIXME : Do we need any other info
		 * which is part of association */
		DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Bss Info ASSOC %d\n",
			__func__,
			bss_conf->aid);
		sc->sc_curaid = bss_conf->aid;

		DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Set channel: %d MHz\n",
			__func__,
			curchan->center_freq);

		hchan.channel = curchan->center_freq;
		hchan.channelFlags = ath_chan2flags(curchan, sc);

		/* set h/w channel */
		if (ath_set_channel(sc, &hchan) < 0)
			DPRINTF(sc, ATH_DEBUG_FATAL,
				"%s: Unable to set channel\n",
				__func__);
	} else {
		DPRINTF(sc, ATH_DEBUG_CONFIG,
		"%s: Bss Info DISSOC\n", __func__);
		sc->sc_curaid = 0;
	}
}

static void ath9k_bss_info_changed(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif,
				   struct ieee80211_bss_conf *bss_conf,
				   u32 changed)
{
	struct ath_softc *sc = hw->priv;

	if (changed & BSS_CHANGED_ERP_PREAMBLE) {
		DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: BSS Changed PREAMBLE %d\n",
			__func__,
			bss_conf->use_short_preamble);
		if (bss_conf->use_short_preamble)
			sc->sc_flags |= ATH_PREAMBLE_SHORT;
		else
			sc->sc_flags &= ~ATH_PREAMBLE_SHORT;
	}

	if (changed & BSS_CHANGED_ERP_CTS_PROT) {
		DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: BSS Changed CTS PROT %d\n",
			__func__,
			bss_conf->use_cts_prot);
		if (bss_conf->use_cts_prot &&
		    hw->conf.channel->band != IEEE80211_BAND_5GHZ)
			sc->sc_flags |= ATH_PROTECT_ENABLE;
		else
			sc->sc_flags &= ~ATH_PROTECT_ENABLE;
	}

	if (changed & BSS_CHANGED_HT) {
		DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: BSS Changed HT %d\n",
			__func__,
			bss_conf->assoc_ht);
		ath9k_ht_conf(sc, bss_conf);
	}

	if (changed & BSS_CHANGED_ASSOC) {
		DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: BSS Changed ASSOC %d\n",
			__func__,
			bss_conf->assoc);
		ath9k_bss_assoc_info(sc, bss_conf);
	}
}

static u64 ath9k_get_tsf(struct ieee80211_hw *hw)
{
	u_int64_t tsf;
	struct ath_softc *sc = hw->priv;
	struct ath_hal *ah = sc->sc_ah;

	tsf = ath9k_hw_gettsf64(ah);

	return tsf;
}

static void ath9k_reset_tsf(struct ieee80211_hw *hw)
{
	struct ath_softc *sc = hw->priv;
	struct ath_hal *ah = sc->sc_ah;

	ath9k_hw_reset_tsf(ah);
}

static int ath9k_ampdu_action(struct ieee80211_hw *hw,
		       enum ieee80211_ampdu_mlme_action action,
		       const u8 *addr,
		       u16 tid,
		       u16 *ssn)
{
	struct ath_softc *sc = hw->priv;
	int ret = 0;

	switch (action) {
	case IEEE80211_AMPDU_RX_START:
		ret = ath_rx_aggr_start(sc, addr, tid, ssn);
		if (ret < 0)
			DPRINTF(sc, ATH_DEBUG_FATAL,
				"%s: Unable to start RX aggregation\n",
				__func__);
		break;
	case IEEE80211_AMPDU_RX_STOP:
		ret = ath_rx_aggr_stop(sc, addr, tid);
		if (ret < 0)
			DPRINTF(sc, ATH_DEBUG_FATAL,
				"%s: Unable to stop RX aggregation\n",
				__func__);
		break;
	case IEEE80211_AMPDU_TX_START:
		ret = ath_tx_aggr_start(sc, addr, tid, ssn);
		if (ret < 0)
			DPRINTF(sc, ATH_DEBUG_FATAL,
				"%s: Unable to start TX aggregation\n",
				__func__);
		else
			ieee80211_start_tx_ba_cb_irqsafe(hw, (u8 *)addr, tid);
		break;
	case IEEE80211_AMPDU_TX_STOP:
		ret = ath_tx_aggr_stop(sc, addr, tid);
		if (ret < 0)
			DPRINTF(sc, ATH_DEBUG_FATAL,
				"%s: Unable to stop TX aggregation\n",
				__func__);

		ieee80211_stop_tx_ba_cb_irqsafe(hw, (u8 *)addr, tid);
		break;
	default:
		DPRINTF(sc, ATH_DEBUG_FATAL,
			"%s: Unknown AMPDU action\n", __func__);
	}

	return ret;
}

static struct ieee80211_ops ath9k_ops = {
	.tx 		    = ath9k_tx,
	.start 		    = ath9k_start,
	.stop 		    = ath9k_stop,
	.add_interface 	    = ath9k_add_interface,
	.remove_interface   = ath9k_remove_interface,
	.config 	    = ath9k_config,
	.config_interface   = ath9k_config_interface,
	.configure_filter   = ath9k_configure_filter,
	.get_stats          = NULL,
	.sta_notify         = ath9k_sta_notify,
	.conf_tx 	    = ath9k_conf_tx,
	.get_tx_stats 	    = NULL,
	.bss_info_changed   = ath9k_bss_info_changed,
	.set_tim            = NULL,
	.set_key            = ath9k_set_key,
	.hw_scan            = NULL,
	.get_tkip_seq       = NULL,
	.set_rts_threshold  = NULL,
	.set_frag_threshold = NULL,
	.set_retry_limit    = NULL,
	.get_tsf 	    = ath9k_get_tsf,
	.reset_tsf 	    = ath9k_reset_tsf,
	.tx_last_beacon     = NULL,
	.ampdu_action       = ath9k_ampdu_action
};

u_int32_t ath_chan2flags(struct ieee80211_channel *chan,
				struct ath_softc *sc)
{
	struct ieee80211_hw *hw = sc->hw;
	struct ath_ht_info *ht_info = &sc->sc_ht_info;

	if (sc->sc_scanning) {
		if (chan->band == IEEE80211_BAND_5GHZ) {
			if (ath_check_chanflags(chan, CHANNEL_A_HT20, sc))
				return CHANNEL_A_HT20;
			else
				return CHANNEL_A;
		} else {
			if (ath_check_chanflags(chan, CHANNEL_G_HT20, sc))
				return CHANNEL_G_HT20;
			else if (ath_check_chanflags(chan, CHANNEL_G, sc))
				return CHANNEL_G;
			else
				return CHANNEL_B;
		}
	} else {
		if (chan->band == IEEE80211_BAND_2GHZ) {
			if (!hw->conf.ht_conf.ht_supported) {
				if (ath_check_chanflags(chan, CHANNEL_G, sc))
					return CHANNEL_G;
				else
					return CHANNEL_B;
			}
			if ((ht_info->ext_chan_offset ==
			     IEEE80211_HT_IE_CHA_SEC_NONE) &&
			    (ht_info->tx_chan_width == HAL_HT_MACMODE_20))
				return CHANNEL_G_HT20;
			if ((ht_info->ext_chan_offset ==
			     IEEE80211_HT_IE_CHA_SEC_ABOVE) &&
			    (ht_info->tx_chan_width == HAL_HT_MACMODE_2040))
				return CHANNEL_G_HT40PLUS;
			if ((ht_info->ext_chan_offset ==
			     IEEE80211_HT_IE_CHA_SEC_BELOW) &&
			    (ht_info->tx_chan_width == HAL_HT_MACMODE_2040))
				return CHANNEL_G_HT40MINUS;
			return CHANNEL_B;
		} else {
			if (!hw->conf.ht_conf.ht_supported)
				return CHANNEL_A;
			if ((ht_info->ext_chan_offset ==
			     IEEE80211_HT_IE_CHA_SEC_NONE) &&
			    (ht_info->tx_chan_width == HAL_HT_MACMODE_20))
				return CHANNEL_A_HT20;
			if ((ht_info->ext_chan_offset ==
			     IEEE80211_HT_IE_CHA_SEC_ABOVE) &&
			    (ht_info->tx_chan_width == HAL_HT_MACMODE_2040))
				return CHANNEL_A_HT40PLUS;
			if ((ht_info->ext_chan_offset ==
			     IEEE80211_HT_IE_CHA_SEC_BELOW) &&
			    (ht_info->tx_chan_width == HAL_HT_MACMODE_2040))
				return CHANNEL_A_HT40MINUS;
			return CHANNEL_A;
		}
	}
}

void ath_setup_channel_list(struct ath_softc *sc,
			    enum ieee80211_clist_cmd cmd,
			    const struct hal_channel *chans,
			    int nchan,
			    const u_int8_t *regclassids,
			    u_int nregclass,
			    int countrycode)
{
	const struct hal_channel *c;
	int i, a = 0, b = 0, flags;

	if (countrycode == CTRY_DEFAULT) {
		for (i = 0; i < nchan; i++) {
			c = &chans[i];
			flags = 0;
			/* XXX: Ah! make more readable, and
			 * idententation friendly */
			if (IS_CHAN_2GHZ(c) &&
			    !test_update_chan(IEEE80211_BAND_2GHZ, c, sc)) {
				sc->channels[IEEE80211_BAND_2GHZ][a].band =
					IEEE80211_BAND_2GHZ;
				sc->channels[IEEE80211_BAND_2GHZ][a].
					center_freq =
					c->channel;
				sc->channels[IEEE80211_BAND_2GHZ][a].max_power =
					c->maxTxPower;
				sc->channels[IEEE80211_BAND_2GHZ][a].hw_value =
					c->channelFlags;

				if (c->privFlags & CHANNEL_DISALLOW_ADHOC)
					flags |= IEEE80211_CHAN_NO_IBSS;
				if (IS_CHAN_PASSIVE(c))
					flags |= IEEE80211_CHAN_PASSIVE_SCAN;

				sc->channels[IEEE80211_BAND_2GHZ][a].flags =
					flags;
				sc->sbands[IEEE80211_BAND_2GHZ].n_channels++;
				a++;
				DPRINTF(sc, ATH_DEBUG_CONFIG,
					"%s: 2MHz channel: %d, "
					"channelFlags: 0x%x\n",
					__func__,
					c->channel,
					c->channelFlags);
			} else if (IS_CHAN_5GHZ(c) &&
			 !test_update_chan(IEEE80211_BAND_5GHZ, c, sc)) {
				sc->channels[IEEE80211_BAND_5GHZ][b].band =
					IEEE80211_BAND_5GHZ;
				sc->channels[IEEE80211_BAND_5GHZ][b].
					center_freq =
					c->channel;
				sc->channels[IEEE80211_BAND_5GHZ][b].max_power =
					c->maxTxPower;
				sc->channels[IEEE80211_BAND_5GHZ][b].hw_value =
					c->channelFlags;

				if (c->privFlags & CHANNEL_DISALLOW_ADHOC)
					flags |= IEEE80211_CHAN_NO_IBSS;
				if (IS_CHAN_PASSIVE(c))
					flags |= IEEE80211_CHAN_PASSIVE_SCAN;

				sc->channels[IEEE80211_BAND_5GHZ][b].
					flags = flags;
				sc->sbands[IEEE80211_BAND_5GHZ].n_channels++;
				b++;
				DPRINTF(sc, ATH_DEBUG_CONFIG,
					"%s: 5MHz channel: %d, "
					"channelFlags: 0x%x\n",
					__func__,
					c->channel,
					c->channelFlags);
			}
		}
	}
}

void ath_get_beaconconfig(struct ath_softc *sc,
			  int if_id,
			  struct ath_beacon_config *conf)
{
	struct ieee80211_hw *hw = sc->hw;

	/* fill in beacon config data */

	conf->beacon_interval = hw->conf.beacon_int;
	conf->listen_interval = 100;
	conf->dtim_count = 1;
	conf->bmiss_timeout = ATH_DEFAULT_BMISS_LIMIT * conf->listen_interval;
}

struct sk_buff *ath_get_beacon(struct ath_softc *sc,
			       int if_id,
			       struct ath_beacon_offset *bo,
			       struct ath_tx_control *txctl)
{
	return NULL;
}

int ath_update_beacon(struct ath_softc *sc,
		      int if_id,
		      struct ath_beacon_offset *bo,
		      struct sk_buff *skb,
		      int mcast)
{
	return 0;
}

void ath_tx_complete(struct ath_softc *sc, struct sk_buff *skb,
		     struct ath_xmit_status *tx_status, struct ath_node *an)
{
	struct ieee80211_hw *hw = sc->hw;
	struct ieee80211_tx_info *tx_info = IEEE80211_SKB_CB(skb);

	DPRINTF(sc, ATH_DEBUG_XMIT,
		"%s: TX complete: skb: %p\n", __func__, skb);

	if (tx_info->flags & IEEE80211_TX_CTL_NO_ACK ||
		tx_info->flags & IEEE80211_TX_STAT_TX_FILTERED) {
		/* free driver's private data area of tx_info */
		if (tx_info->driver_data[0] != NULL)
			kfree(tx_info->driver_data[0]);
			tx_info->driver_data[0] = NULL;
	}

	if (tx_status->flags & ATH_TX_BAR) {
		tx_info->flags |= IEEE80211_TX_STAT_AMPDU_NO_BACK;
		tx_status->flags &= ~ATH_TX_BAR;
	}
	if (tx_status->flags)
		tx_info->status.excessive_retries = 1;

	tx_info->status.retry_count = tx_status->retries;

	ieee80211_tx_status(hw, skb);
	if (an)
		ath_node_put(sc, an, ATH9K_BH_STATUS_CHANGE);
}

int ath__rx_indicate(struct ath_softc *sc,
		     struct sk_buff *skb,
		     struct ath_recv_status *status,
		     u_int16_t keyix)
{
	struct ieee80211_hw *hw = sc->hw;
	struct ath_node *an = NULL;
	struct ieee80211_rx_status rx_status;
	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *) skb->data;
	int hdrlen = ieee80211_get_hdrlen_from_skb(skb);
	int padsize;
	enum ATH_RX_TYPE st;

	/* see if any padding is done by the hw and remove it */
	if (hdrlen & 3) {
		padsize = hdrlen % 4;
		memmove(skb->data + padsize, skb->data, hdrlen);
		skb_pull(skb, padsize);
	}

	/* remove FCS before passing up to protocol stack */
	skb_trim(skb, (skb->len - FCS_LEN));

	/* Prepare rx status */
	ath9k_rx_prepare(sc, skb, status, &rx_status);

	if (!(keyix == HAL_RXKEYIX_INVALID) &&
	    !(status->flags & ATH_RX_DECRYPT_ERROR)) {
		rx_status.flag |= RX_FLAG_DECRYPTED;
	} else if ((le16_to_cpu(hdr->frame_control) & IEEE80211_FCTL_PROTECTED)
		   && !(status->flags & ATH_RX_DECRYPT_ERROR)
		   && skb->len >= hdrlen + 4) {
		keyix = skb->data[hdrlen + 3] >> 6;

		if (test_bit(keyix, sc->sc_keymap))
			rx_status.flag |= RX_FLAG_DECRYPTED;
	}

	spin_lock_bh(&sc->node_lock);
	an = ath_node_find(sc, hdr->addr2);
	spin_unlock_bh(&sc->node_lock);

	if (an) {
		ath_rx_input(sc, an,
			     hw->conf.ht_conf.ht_supported,
			     skb, status, &st);
	}
	if (!an || (st != ATH_RX_CONSUMED))
		__ieee80211_rx(hw, skb, &rx_status);

	return 0;
}

int ath_rx_subframe(struct ath_node *an,
		    struct sk_buff *skb,
		    struct ath_recv_status *status)
{
	struct ath_softc *sc = an->an_sc;
	struct ieee80211_hw *hw = sc->hw;
	struct ieee80211_rx_status rx_status;

	/* Prepare rx status */
	ath9k_rx_prepare(sc, skb, status, &rx_status);
	if (!(status->flags & ATH_RX_DECRYPT_ERROR))
		rx_status.flag |= RX_FLAG_DECRYPTED;

	__ieee80211_rx(hw, skb, &rx_status);

	return 0;
}

enum hal_ht_macmode ath_cwm_macmode(struct ath_softc *sc)
{
	return sc->sc_ht_info.tx_chan_width;
}

void ath_setup_rate(struct ath_softc *sc,
		    enum wireless_mode wMode,
		    enum RATE_TYPE type,
		    const struct hal_rate_table *rt)
{
	int i, maxrates, a = 0, b = 0;
	struct ieee80211_supported_band *band_2ghz;
	struct ieee80211_supported_band *band_5ghz;
	struct ieee80211_rate *rates_2ghz;
	struct ieee80211_rate *rates_5ghz;

	if ((wMode >= WIRELESS_MODE_MAX) || (type != NORMAL_RATE))
		return;

	band_2ghz = &sc->sbands[IEEE80211_BAND_2GHZ];
	band_5ghz = &sc->sbands[IEEE80211_BAND_5GHZ];
	rates_2ghz = sc->rates[IEEE80211_BAND_2GHZ];
	rates_5ghz = sc->rates[IEEE80211_BAND_5GHZ];

	if (rt->rateCount > ATH_RATE_MAX)
		maxrates = ATH_RATE_MAX;
	else
		maxrates = rt->rateCount;

	if ((band_2ghz->n_bitrates != 0) && (band_5ghz->n_bitrates != 0)) {
		DPRINTF(sc, ATH_DEBUG_CONFIG,
			"%s: Rates already setup\n", __func__);
		return;
	}

	for (i = 0; i < maxrates; i++) {
		switch (wMode) {
		case WIRELESS_MODE_11b:
		case WIRELESS_MODE_11g:
			rates_2ghz[a].bitrate = rt->info[i].rateKbps / 100;
			rates_2ghz[a].hw_value = rt->info[i].rateCode;
			a++;
			band_2ghz->n_bitrates = a;
			break;
		case WIRELESS_MODE_11a:
			rates_5ghz[b].bitrate = rt->info[i].rateKbps / 100;
			rates_5ghz[b].hw_value = rt->info[i].rateCode;
			b++;
			band_5ghz->n_bitrates = b;
			break;
		default:
			break;
		}
	}

	if (band_2ghz->n_bitrates) {
		for (i = 0; i < band_2ghz->n_bitrates; i++) {
			DPRINTF(sc, ATH_DEBUG_CONFIG,
				"%s: 2GHz Rate: %2dMbps, ratecode: %2d\n",
				__func__,
				rates_2ghz[i].bitrate / 10,
				rates_2ghz[i].hw_value);
		}
	} else if (band_5ghz->n_bitrates) {
		for (i = 0; i < band_5ghz->n_bitrates; i++) {
			DPRINTF(sc, ATH_DEBUG_CONFIG,
				"%s: 5Ghz Rate: %2dMbps, ratecode: %2d\n",
				__func__,
				rates_5ghz[i].bitrate / 10,
				rates_5ghz[i].hw_value);
		}
	}
}

static int ath_detach(struct ath_softc *sc)
{
	struct ieee80211_hw *hw = sc->hw;

	DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Detach ATH hw\n", __func__);

	/* Unregister hw */

	ieee80211_unregister_hw(hw);

	/* unregister Rate control */
	ath_rate_control_unregister();

	/* tx/rx cleanup */

	ath_rx_cleanup(sc);
	ath_tx_cleanup(sc);

	/* Deinit */

	ath_deinit(sc);

	return 0;
}

static int ath_attach(u_int16_t devid,
		      struct ath_softc *sc)
{
	struct ieee80211_hw *hw = sc->hw;
	int error = 0;

	DPRINTF(sc, ATH_DEBUG_CONFIG, "%s: Attach ATH hw\n", __func__);

	error = ath_init(devid, sc);
	if (error != 0)
		return error;

	/* Init nodes */

	INIT_LIST_HEAD(&sc->node_list);
	spin_lock_init(&sc->node_lock);

	/* get mac address from hardware and set in mac80211 */

	SET_IEEE80211_PERM_ADDR(hw, sc->sc_myaddr);

	/* setup channels and rates */

	sc->sbands[IEEE80211_BAND_2GHZ].channels =
		sc->channels[IEEE80211_BAND_2GHZ];
	sc->sbands[IEEE80211_BAND_2GHZ].bitrates =
		sc->rates[IEEE80211_BAND_2GHZ];
	sc->sbands[IEEE80211_BAND_2GHZ].band = IEEE80211_BAND_2GHZ;

	if (sc->sc_hashtsupport)
		/* Setup HT capabilities for 2.4Ghz*/
		setup_ht_cap(&sc->sbands[IEEE80211_BAND_2GHZ].ht_info);

	hw->wiphy->bands[IEEE80211_BAND_2GHZ] =
		&sc->sbands[IEEE80211_BAND_2GHZ];

	if (sc->sc_ah->ah_caps.halWirelessModes & ATH9K_MODE_SEL_11A) {
		sc->sbands[IEEE80211_BAND_5GHZ].channels =
			sc->channels[IEEE80211_BAND_5GHZ];
		sc->sbands[IEEE80211_BAND_5GHZ].bitrates =
			sc->rates[IEEE80211_BAND_5GHZ];
		sc->sbands[IEEE80211_BAND_5GHZ].band =
			IEEE80211_BAND_5GHZ;

		if (sc->sc_hashtsupport)
			/* Setup HT capabilities for 5Ghz*/
			setup_ht_cap(&sc->sbands[IEEE80211_BAND_5GHZ].ht_info);

		hw->wiphy->bands[IEEE80211_BAND_5GHZ] =
			&sc->sbands[IEEE80211_BAND_5GHZ];
	}

	/* FIXME: Have to figure out proper hw init values later */

	hw->queues = 4;
	hw->ampdu_queues = 1;

	/* Register rate control */
	hw->rate_control_algorithm = "ath9k_rate_control";
	error = ath_rate_control_register();
	if (error != 0) {
		DPRINTF(sc, ATH_DEBUG_FATAL,
			"%s: Unable to register rate control "
			"algorithm:%d\n", __func__, error);
		ath_rate_control_unregister();
		goto bad;
	}

	error = ieee80211_register_hw(hw);
	if (error != 0) {
		ath_rate_control_unregister();
		goto bad;
	}

	/* initialize tx/rx engine */

	error = ath_tx_init(sc, ATH_TXBUF);
	if (error != 0)
		goto bad1;

	error = ath_rx_init(sc, ATH_RXBUF);
	if (error != 0)
		goto bad1;

	return 0;
bad1:
	ath_detach(sc);
bad:
	return error;
}

static irqreturn_t ath_isr(int irq, void *dev_id)
{
	struct ath_softc *sc = dev_id;
	int sched;

	/* always acknowledge the interrupt */
	sched = ath_intr(sc);

	switch (sched) {
	case ATH_ISR_NOSCHED:
		return IRQ_HANDLED;
	case ATH_ISR_NOTMINE:
		return IRQ_NONE;
	default:
		tasklet_schedule(&sc->intr_tq);
		return IRQ_HANDLED;

	}
}

static int ath_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	void __iomem *mem;
	struct ath_softc *sc;
	struct ieee80211_hw *hw;
	const char *athname;
	u_int8_t csz;
	u32 val;
	int ret = 0;

	if (pci_enable_device(pdev))
		return -EIO;

	/* XXX 32-bit addressing only */
	if (pci_set_dma_mask(pdev, 0xffffffff)) {
		printk(KERN_ERR "ath_pci: 32-bit DMA not available\n");
		ret = -ENODEV;
		goto bad;
	}

	/*
	 * Cache line size is used to size and align various
	 * structures used to communicate with the hardware.
	 */
	pci_read_config_byte(pdev, PCI_CACHE_LINE_SIZE, &csz);
	if (csz == 0) {
		/*
		 * Linux 2.4.18 (at least) writes the cache line size
		 * register as a 16-bit wide register which is wrong.
		 * We must have this setup properly for rx buffer
		 * DMA to work so force a reasonable value here if it
		 * comes up zero.
		 */
		csz = L1_CACHE_BYTES / sizeof(u_int32_t);
		pci_write_config_byte(pdev, PCI_CACHE_LINE_SIZE, csz);
	}
	/*
	 * The default setting of latency timer yields poor results,
	 * set it to the value used by other systems. It may be worth
	 * tweaking this setting more.
	 */
	pci_write_config_byte(pdev, PCI_LATENCY_TIMER, 0xa8);

	pci_set_master(pdev);

	/*
	 * Disable the RETRY_TIMEOUT register (0x41) to keep
	 * PCI Tx retries from interfering with C3 CPU state.
	 */
	pci_read_config_dword(pdev, 0x40, &val);
	if ((val & 0x0000ff00) != 0)
		pci_write_config_dword(pdev, 0x40, val & 0xffff00ff);

	ret = pci_request_region(pdev, 0, "ath9k");
	if (ret) {
		dev_err(&pdev->dev, "PCI memory region reserve error\n");
		ret = -ENODEV;
		goto bad;
	}

	mem = pci_iomap(pdev, 0, 0);
	if (!mem) {
		printk(KERN_ERR "PCI memory map error\n") ;
		ret = -EIO;
		goto bad1;
	}

	hw = ieee80211_alloc_hw(sizeof(struct ath_softc), &ath9k_ops);
	if (hw == NULL) {
		printk(KERN_ERR "ath_pci: no memory for ieee80211_hw\n");
		goto bad2;
	}

	SET_IEEE80211_DEV(hw, &pdev->dev);
	pci_set_drvdata(pdev, hw);

	sc = hw->priv;
	sc->hw = hw;
	sc->pdev = pdev;
	sc->mem = mem;

	if (ath_attach(id->device, sc) != 0) {
		ret = -ENODEV;
		goto bad3;
	}

	/* setup interrupt service routine */

	if (request_irq(pdev->irq, ath_isr, IRQF_SHARED, "ath", sc)) {
		printk(KERN_ERR "%s: request_irq failed\n",
			wiphy_name(hw->wiphy));
		ret = -EIO;
		goto bad4;
	}

	athname = ath9k_hw_probe(id->vendor, id->device);

	printk(KERN_INFO "%s: %s: mem=0x%lx, irq=%d\n",
	       wiphy_name(hw->wiphy),
	       athname ? athname : "Atheros ???",
	       (unsigned long)mem, pdev->irq);

	return 0;
bad4:
	ath_detach(sc);
bad3:
	ieee80211_free_hw(hw);
bad2:
	pci_iounmap(pdev, mem);
bad1:
	pci_release_region(pdev, 0);
bad:
	pci_disable_device(pdev);
	return ret;
}

static void ath_pci_remove(struct pci_dev *pdev)
{
	struct ieee80211_hw *hw = pci_get_drvdata(pdev);
	struct ath_softc *sc = hw->priv;

	if (pdev->irq)
		free_irq(pdev->irq, sc);
	ath_detach(sc);
	pci_iounmap(pdev, sc->mem);
	pci_release_region(pdev, 0);
	pci_disable_device(pdev);
	ieee80211_free_hw(hw);
}

#ifdef CONFIG_PM

static int ath_pci_suspend(struct pci_dev *pdev, pm_message_t state)
{
	pci_save_state(pdev);
	pci_disable_device(pdev);
	pci_set_power_state(pdev, 3);

	return 0;
}

static int ath_pci_resume(struct pci_dev *pdev)
{
	u32 val;
	int err;

	err = pci_enable_device(pdev);
	if (err)
		return err;
	pci_restore_state(pdev);
	/*
	 * Suspend/Resume resets the PCI configuration space, so we have to
	 * re-disable the RETRY_TIMEOUT register (0x41) to keep
	 * PCI Tx retries from interfering with C3 CPU state
	 */
	pci_read_config_dword(pdev, 0x40, &val);
	if ((val & 0x0000ff00) != 0)
		pci_write_config_dword(pdev, 0x40, val & 0xffff00ff);

	return 0;
}

#endif /* CONFIG_PM */

MODULE_DEVICE_TABLE(pci, ath_pci_id_table);

static struct pci_driver ath_pci_driver = {
	.name       = "ath9k",
	.id_table   = ath_pci_id_table,
	.probe      = ath_pci_probe,
	.remove     = ath_pci_remove,
#ifdef CONFIG_PM
	.suspend    = ath_pci_suspend,
	.resume     = ath_pci_resume,
#endif /* CONFIG_PM */
};

static int __init init_ath_pci(void)
{
	printk(KERN_INFO "%s: %s\n", dev_info, ATH_PCI_VERSION);

	if (pci_register_driver(&ath_pci_driver) < 0) {
		printk(KERN_ERR
			"ath_pci: No devices found, driver not installed.\n");
		pci_unregister_driver(&ath_pci_driver);
		return -ENODEV;
	}

	return 0;
}
module_init(init_ath_pci);

static void __exit exit_ath_pci(void)
{
	pci_unregister_driver(&ath_pci_driver);
	printk(KERN_INFO "%s: driver unloaded\n", dev_info);
}
module_exit(exit_ath_pci);
