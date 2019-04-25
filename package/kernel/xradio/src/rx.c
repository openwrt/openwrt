#include <net/mac80211.h>

#include "xradio.h"
#include "rx.h"
#include "ht.h"
#include "p2p.h"
#include "sta.h"
#include "bh.h"
#include "ap.h"

	// MRK: added copy of this tx.c function here for testing, renamed _rx

static void xradio_check_go_neg_conf_success_rx(struct xradio_common *hw_priv,
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


static int xradio_handle_pspoll(struct xradio_vif *priv,
				struct sk_buff *skb)
{
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);
	struct ieee80211_sta *sta;
	struct ieee80211_pspoll *pspoll =
		(struct ieee80211_pspoll *) skb->data;
	int link_id = 0;
	u32 pspoll_mask = 0;
	int drop = 1;
	int i;


	if (priv->join_status != XRADIO_JOIN_STATUS_AP)
		goto done;
	if (memcmp(priv->vif->addr, pspoll->bssid, ETH_ALEN))
		goto done;

	rcu_read_lock();
	sta = ieee80211_find_sta(priv->vif, pspoll->ta);
	if (sta) {
		struct xradio_sta_priv *sta_priv;
		sta_priv = (struct xradio_sta_priv *)&sta->drv_priv;
		link_id = sta_priv->link_id;
		pspoll_mask = BIT(sta_priv->link_id);
	}
	rcu_read_unlock();
	if (!link_id)
		goto done;

	priv->pspoll_mask |= pspoll_mask;
	drop = 0;

	/* Do not report pspols if data for given link id is
	 * queued already. */
	for (i = 0; i < 4; ++i) {
		if (xradio_queue_get_num_queued(priv,
				&hw_priv->tx_queue[i],
				pspoll_mask)) {
			xradio_bh_wakeup(hw_priv);
			drop = 1;
			break;
		}
	}
	txrx_printk(XRADIO_DBG_NIY, "[RX] PSPOLL: %s\n", drop ? "local" : "fwd");
done:
	return drop;
}


static void
xradio_rx_h_ba_stat(struct xradio_vif *priv,
		    size_t hdrlen, size_t skb_len )
{
	struct xradio_common *hw_priv = priv->hw_priv;


	if (priv->join_status != XRADIO_JOIN_STATUS_STA)
		return;
	if (!xradio_is_ht(&hw_priv->ht_oper))
		return;
	if (!priv->setbssparams_done)
		return;

	spin_lock_bh(&hw_priv->ba_lock);
	hw_priv->ba_acc_rx += skb_len - hdrlen;
	if (!(hw_priv->ba_cnt_rx || hw_priv->ba_cnt)) {
		mod_timer(&hw_priv->ba_timer,
			jiffies + XRADIO_BLOCK_ACK_INTERVAL);
	}
	hw_priv->ba_cnt_rx++;
	spin_unlock_bh(&hw_priv->ba_lock);
}

void xradio_rx_cb(struct xradio_vif *priv,
		  struct wsm_rx *arg,
		  struct sk_buff **skb_p)
{
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);
	struct sk_buff *skb = *skb_p;
	struct ieee80211_rx_status *hdr = IEEE80211_SKB_RXCB(skb);
	struct ieee80211_hdr *frame = (struct ieee80211_hdr *)skb->data;
#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)skb->data;
#endif
	struct xradio_link_entry *entry = NULL;
	unsigned long grace_period;
	bool early_data = false;
	size_t hdrlen = 0;
	u8   parse_iv_len = 0;

	dev_dbg(hw_priv->pdev, "vif %d: rx, status %u flags 0x%.8x",
			priv->if_id, arg->status, arg->flags);
	if(ieee80211_is_deauth(frame->frame_control))
		dev_dbg(hw_priv->pdev, "vif %d: deauth\n", priv->if_id);

	hdr->flag = 0;

	if (unlikely(priv->mode == NL80211_IFTYPE_UNSPECIFIED)) {
		/* STA is stopped. */
		goto drop;
	}
	
#ifdef TES_P2P_0002_ROC_RESTART
	xradio_frame_monitor(hw_priv,skb,false);
#endif

#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	if ((ieee80211_is_action(frame->frame_control))
	    && (mgmt->u.action.category == WLAN_CATEGORY_PUBLIC)) {
		u8 *action = (u8*)&mgmt->u.action.category;
		xradio_check_go_neg_conf_success_rx(hw_priv, action);
	}
#endif

	if (arg->link_id && (arg->link_id != XRADIO_LINK_ID_UNMAPPED)
			&& (arg->link_id <= XRADIO_MAX_STA_IN_AP_MODE)) {
		entry =	&priv->link_id_db[arg->link_id - 1];
		if (entry->status == XRADIO_LINK_SOFT &&
				ieee80211_is_data(frame->frame_control))
			early_data = true;
		entry->timestamp = jiffies;
	}
#if defined(CONFIG_XRADIO_USE_EXTENSIONS)
	else if ((arg->link_id == XRADIO_LINK_ID_UNMAPPED)
			&& (priv->vif->p2p == WSM_START_MODE_P2P_GO)
			&& ieee80211_is_action(frame->frame_control)
			&& (mgmt->u.action.category == WLAN_CATEGORY_PUBLIC)) {
		txrx_printk(XRADIO_DBG_NIY, "[RX] Going to MAP&RESET link ID\n");

		if (work_pending(&priv->linkid_reset_work))
			WARN_ON(1);

		memcpy(&priv->action_frame_sa[0],
				ieee80211_get_SA(frame), ETH_ALEN);
		priv->action_linkid = 0;
		schedule_work(&priv->linkid_reset_work);
	}

	if (arg->link_id && (arg->link_id != XRADIO_LINK_ID_UNMAPPED)
			&& (priv->vif->p2p == WSM_START_MODE_P2P_GO)
			&& ieee80211_is_action(frame->frame_control)
			&& (mgmt->u.action.category == WLAN_CATEGORY_PUBLIC)) {
		/* Link ID already exists for the ACTION frame.
		 * Reset and Remap */
		if (work_pending(&priv->linkid_reset_work))
			WARN_ON(1);
		memcpy(&priv->action_frame_sa[0],
				ieee80211_get_SA(frame), ETH_ALEN);
		priv->action_linkid = arg->link_id;
		schedule_work(&priv->linkid_reset_work);
	}
#endif
	if (unlikely(arg->status)) {
		if (arg->status == WSM_STATUS_MICFAILURE) {
			dev_err(priv->hw_priv->pdev, "[RX] IF=%d, MIC failure.\n",
			            priv->if_id);
			hdr->flag |= RX_FLAG_MMIC_ERROR;
		} else if (arg->status == WSM_STATUS_NO_KEY_FOUND) {
			dev_warn(priv->hw_priv->pdev, "received frame has no key status\n");
			//goto drop;
		} else {
			dev_err(priv->hw_priv->pdev, "[RX] IF=%d, Receive failure: %d.\n",
				priv->if_id, arg->status);
			goto drop;
		}
	}

	if (skb->len < sizeof(struct ieee80211_pspoll)) {
		dev_err(priv->hw_priv->pdev, "Malformed SDU rx'ed. "
		            "Size is lesser than IEEE header.\n");
		goto drop;
	}

	if (unlikely(ieee80211_is_pspoll(frame->frame_control)))
		if (xradio_handle_pspoll(priv, skb))
			goto drop;

	hdr->mactime = 0; /* Not supported by WSM */
	hdr->band = (arg->channelNumber > 14) ?
			NL80211_BAND_5GHZ : NL80211_BAND_2GHZ;
	hdr->freq = ieee80211_channel_to_frequency(
			arg->channelNumber,
			hdr->band);

#ifdef AP_HT_COMPAT_FIX
	if (!priv->ht_compat_det && priv->htcap &&
		ieee80211_is_data_qos(frame->frame_control)) {
		if(xradio_apcompat_detect(priv, arg->rxedRate))
			goto drop;
	}
#endif
       
	if (arg->rxedRate >= 14) {
		hdr->encoding = RX_ENC_HT;
		hdr->rate_idx = arg->rxedRate - 14;
	} else if (arg->rxedRate >= 4) {
		if (hdr->band == NL80211_BAND_5GHZ)
			hdr->rate_idx = arg->rxedRate - 6;
		else
			hdr->rate_idx = arg->rxedRate - 2;
	} else {
		hdr->rate_idx = arg->rxedRate;
	}

	hdr->signal = (s8)arg->rcpiRssi;
	hdr->antenna = 0;

	hdrlen = ieee80211_hdrlen(frame->frame_control);

	if (WSM_RX_STATUS_ENCRYPTION(arg->flags)) {
		size_t iv_len = 0, icv_len = 0;

		hdr->flag |= RX_FLAG_DECRYPTED;

		/* Oops... There is no fast way to ask mac80211 about
		 * IV/ICV lengths. Even defines are not exposed.*/
		switch (WSM_RX_STATUS_ENCRYPTION(arg->flags)) {
		case WSM_RX_STATUS_WEP:
			iv_len = 4 /* WEP_IV_LEN */;
			icv_len = 4 /* WEP_ICV_LEN */;
			break;
		case WSM_RX_STATUS_TKIP:
			iv_len = 8 /* TKIP_IV_LEN */;
			icv_len = 4 /* TKIP_ICV_LEN */
				+ 8 /*MICHAEL_MIC_LEN*/;
			break;
		case WSM_RX_STATUS_AES:
			iv_len = 8 /* CCMP_HDR_LEN */;
			icv_len = 8 /* CCMP_MIC_LEN */;
			break;
		case WSM_RX_STATUS_WAPI:
			iv_len = 18 /* WAPI_HDR_LEN */;
			icv_len = 16 /* WAPI_MIC_LEN */;
			hdr->flag |= RX_FLAG_IV_STRIPPED;
			break;
		default:
			WARN_ON("Unknown encryption type");
			goto drop;
		}

		/* Firmware strips ICV in case of MIC failure. */
		if (arg->status == WSM_STATUS_MICFAILURE) {
			icv_len = 0;
			hdr->flag |= RX_FLAG_IV_STRIPPED;
		}

		if (skb->len < hdrlen + iv_len + icv_len) {
			dev_err(priv->hw_priv->pdev, "Mailformed SDU rx'ed. "
				"Size is lesser than crypto headers.\n");
			goto drop;
		}

		if (WSM_RX_STATUS_ENCRYPTION(arg->flags) ==
		    WSM_RX_STATUS_TKIP) {
			/* Remove TKIP MIC 8 bytes*/
			memmove(skb->data + skb->len-icv_len, 
			        skb->data + skb->len-icv_len+8, 4);
			skb_trim(skb, skb->len - 8);
			hdr->flag |= RX_FLAG_MMIC_STRIPPED;
		} else if (unlikely(WSM_RX_STATUS_ENCRYPTION(arg->flags) ==
		           WSM_RX_STATUS_WAPI)) {
			/* Protocols not defined in mac80211 should be
			   stripped/crypted in driver/firmware */
			/* Remove IV, ICV and MIC */
			skb_trim(skb, skb->len - icv_len);
			memmove(skb->data + iv_len, skb->data, hdrlen);
			skb_pull(skb, iv_len);
		}
		parse_iv_len = iv_len;
	}

	if (ieee80211_is_beacon(frame->frame_control) &&
		!arg->status &&
		!memcmp(ieee80211_get_SA(frame), priv->join_bssid,ETH_ALEN)) {
		const u8 *tim_ie;
		u8 *ies;
		size_t ies_len;
		priv->disable_beacon_filter = false;
		queue_work(hw_priv->workqueue, &priv->update_filtering_work);
		ies = ((struct ieee80211_mgmt *)
			  (skb->data))->u.beacon.variable;
		ies_len = skb->len - (ies - (u8 *)(skb->data));

		tim_ie = xradio_get_ie(ies, ies_len, WLAN_EID_TIM);
		if (tim_ie) {
			struct ieee80211_tim_ie *tim =
				(struct ieee80211_tim_ie *)&tim_ie[2];

			if (priv->join_dtim_period != tim->dtim_period) {
				priv->join_dtim_period = tim->dtim_period;
				queue_work(hw_priv->workqueue,
					&priv->set_beacon_wakeup_period_work);
			}
		}
		if (unlikely(priv->disable_beacon_filter)) {
			priv->disable_beacon_filter = false;
			queue_work(hw_priv->workqueue,
				&priv->update_filtering_work);
		}
	}
#ifdef AP_HT_CAP_UPDATE
    if (priv->mode == NL80211_IFTYPE_AP           &&
        ieee80211_is_beacon(frame->frame_control) &&
        ((priv->ht_oper&HT_INFO_MASK) != 0x0011)  &&
        !arg->status){
        u8 *ies;
        size_t ies_len;
        const u8 *ht_cap;
        ies = ((struct ieee80211_mgmt *)(skb->data))->u.beacon.variable;
        ies_len = skb->len - (ies - (u8 *)(skb->data));
        ht_cap = xradio_get_ie(ies, ies_len, WLAN_EID_HT_CAPABILITY);
        if(!ht_cap) {
            priv->ht_oper |= 0x0011;
            queue_work(hw_priv->workqueue, &priv->ht_oper_update_work);
        }
    }
#endif

#ifdef AP_HT_COMPAT_FIX
	if (ieee80211_is_mgmt(frame->frame_control) && 
		priv->if_id == 0 && !(priv->ht_compat_det & 0x10)) {
		xradio_remove_ht_ie(priv, skb);
	}
#endif

#ifdef ROAM_OFFLOAD
	if ((ieee80211_is_beacon(frame->frame_control)||ieee80211_is_probe_resp(frame->frame_control)) &&
			!arg->status ) {
		if (hw_priv->auto_scanning && !atomic_read(&hw_priv->scan.in_progress))
			hw_priv->frame_rcvd = 1;

		if (!memcmp(ieee80211_get_SA(frame), priv->join_bssid, ETH_ALEN)) {
			if (hw_priv->beacon)
				dev_kfree_skb(hw_priv->beacon);
			hw_priv->beacon = skb_copy(skb, GFP_ATOMIC);
			if (!hw_priv->beacon)
				txrx_printk(XRADIO_DBG_ERROR, "sched_scan: own beacon storing failed\n");
		}
	}
#endif /*ROAM_OFFLOAD*/

	//don't delay scan before next connect, yangfh.
	if (ieee80211_is_deauth(frame->frame_control) ||
	    ieee80211_is_disassoc(frame->frame_control))
		hw_priv->connet_time[priv->if_id] = 0;

	/* Stay awake for 1sec. after frame is received to give
	 * userspace chance to react and acquire appropriate
	 * wakelock. */
	if (ieee80211_is_auth(frame->frame_control))
		grace_period = 5 * HZ;
	else if (ieee80211_is_deauth(frame->frame_control))
		grace_period = 5 * HZ;
	else
		grace_period = HZ;

	if (ieee80211_is_data(frame->frame_control))
		xradio_rx_h_ba_stat(priv, hdrlen, skb->len);

	xradio_pm_stay_awake(&hw_priv->pm_state, grace_period);

	if(xradio_realloc_resv_skb(hw_priv, *skb_p)) {
		*skb_p = NULL;
		return;
	}
	/* Try to  a packet for the case dev_alloc_skb failed in bh.*/
	if (unlikely(early_data)) {
		spin_lock_bh(&priv->ps_state_lock);
		/* Double-check status with lock held */
		if (entry->status == XRADIO_LINK_SOFT) {
			skb_queue_tail(&entry->rx_queue, skb);
			dev_warn(priv->hw_priv->pdev, "***skb_queue_tail\n");
		} else
			ieee80211_rx_irqsafe(priv->hw, skb);
		spin_unlock_bh(&priv->ps_state_lock);
	} else {
		ieee80211_rx_irqsafe(priv->hw, skb);
	}
	*skb_p = NULL;

	return;

drop:
	dev_warn(priv->hw_priv->pdev, "dropped received frame\n");
	return;
}
