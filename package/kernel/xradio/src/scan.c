/*
 * Scan implementation for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/sched.h>
#include "xradio.h"
#include "scan.h"
#include "sta.h"
#include "pm.h"

static void xradio_scan_restart_delayed(struct xradio_vif *priv);

static void xradio_remove_wps_p2p_ie(struct wsm_template_frame *frame)
{
	u8 *ies;
	u32 ies_len;
	u32 ie_len;
	u32 p2p_ie_len = 0;
	u32 wps_ie_len = 0;


	ies = &frame->skb->data[sizeof(struct ieee80211_hdr_3addr)];
	ies_len = frame->skb->len - sizeof(struct ieee80211_hdr_3addr);
	while (ies_len >= 6) {
		ie_len = ies[1] + 2;
		ies_len -= ie_len;
		if ((ies[0] == WLAN_EID_VENDOR_SPECIFIC) && 
			  (ies[2] == 0x00 && ies[3] == 0x50 && 
			   ies[4] == 0xf2 && ies[5] == 0x04)) {
			wps_ie_len = ie_len;
			memmove(ies, ies + ie_len, ies_len);
		} else if ((ies[0] == WLAN_EID_VENDOR_SPECIFIC) &&
		           (ies[2] == 0x50 && ies[3] == 0x6f && 
		            ies[4] == 0x9a && ies[5] == 0x09)) {
			p2p_ie_len = ie_len;
			memmove(ies, ies + ie_len, ies_len);
		} else {
			ies += ie_len;
		}
	}

	if (p2p_ie_len || wps_ie_len) {
		skb_trim(frame->skb, frame->skb->len - (p2p_ie_len + wps_ie_len));
	}
}

static int xradio_scan_start(struct xradio_vif *priv, struct wsm_scan *scan)
{
	int ret, i;
#ifdef FPGA_SETUP
	int tmo = 5000;
#else
	int tmo = 5000;
#endif
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);


	for (i = 0; i < scan->numOfChannels; ++i)
		tmo += scan->ch[i].maxChannelTime + 10;

	atomic_set(&hw_priv->scan.in_progress, 1);
	atomic_set(&hw_priv->recent_scan, 1);
	xradio_pm_stay_awake(&hw_priv->pm_state, tmo * HZ / 1000);
	ret = wsm_scan(hw_priv, scan, priv->if_id);
	if (unlikely(ret)) {
		scan_printk(XRADIO_DBG_WARN, "%s,wsm_scan failed!\n", __func__);
		atomic_set(&hw_priv->scan.in_progress, 0);
		xradio_scan_restart_delayed(priv);
	} else {
		queue_delayed_work(hw_priv->workqueue, &hw_priv->scan.timeout,
						tmo * HZ / 1000);
	}
	return ret;
}

#ifdef ROAM_OFFLOAD
static int xradio_sched_scan_start(struct xradio_vif *priv, struct wsm_scan *scan)
{
	int ret;
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);


	ret = wsm_scan(hw_priv, scan, priv->if_id);
	if (unlikely(ret)) {
		atomic_set(&hw_priv->scan.in_progress, 0);
		scan_printk(XRADIO_DBG_WARN,"%s,wsm_scan failed!\n", __func__);
	}
	return ret;
}
#endif /*ROAM_OFFLOAD*/

int xradio_hw_scan(struct ieee80211_hw *hw,
		   struct ieee80211_vif *vif,
		   struct ieee80211_scan_request *hw_req)
{
	struct cfg80211_scan_request *req = &hw_req->req;
	struct xradio_common *hw_priv = hw->priv;
	struct xradio_vif *priv = xrwl_get_vif_from_ieee80211(vif);
	struct wsm_template_frame frame = {
		.frame_type = WSM_FRAME_TYPE_PROBE_REQUEST,
	};
	int i;

	/* Scan when P2P_GO corrupt firmware MiniAP mode */
	if (priv->join_status == XRADIO_JOIN_STATUS_AP) {
		scan_printk(XRADIO_DBG_WARN,"%s, can't scan in AP mode!\n", __func__);
		return -EOPNOTSUPP;
	}

	if (hw_priv->bh_error) {
		scan_printk(XRADIO_DBG_NIY, "Ignoring scan bh error occur!\n");
		return -EBUSY;
	}

	if (work_pending(&priv->offchannel_work) ||
			(hw_priv->roc_if_id != -1)) {
		scan_printk(XRADIO_DBG_WARN, "Offchannel work pending, "
		            "ignoring scan work %d\n",  hw_priv->roc_if_id);
		return -EBUSY;
	}

	if (req->n_ssids == 1 && !req->ssids[0].ssid_len)
		req->n_ssids = 0;

	scan_printk(XRADIO_DBG_NIY, "vif%d Scan request(%s-%dchs) for %d SSIDs.\n",
	            priv->if_id, (req->channels[0]->band==NL80211_BAND_2GHZ)?"2.4G":"5G", 
	            req->n_channels, req->n_ssids);
	
	/*delay multiple ssids scan of vif0 for 3s when connnetting to a node*/
	if(hw_priv->connet_time[0] > 0 && req->n_ssids == 0 && priv->if_id == 0) {
		long timeleft0 = hw_priv->connet_time[0] + SCAN_MAX_DELAY - jiffies;
		if(jiffies >= hw_priv->connet_time[0] && timeleft0 > 0) {
			scan_printk(XRADIO_DBG_NIY, "vif0 connetting, scan delay %ldms\n", 
			            timeleft0*1000/HZ);
			return -EBUSY;
		}
		hw_priv->connet_time[0] = 0;
	}

	if (req->n_ssids > hw->wiphy->max_scan_ssids){
		scan_printk(XRADIO_DBG_ERROR, "%s: ssids is too much(%d)\n", 
		            __func__, req->n_ssids);
		return -EINVAL;
	}

	/* TODO by Icenowy: so strange function call */
	frame.skb = ieee80211_probereq_get(hw, vif->addr, NULL, 0, 0);
	if (!frame.skb) {
		scan_printk(XRADIO_DBG_ERROR, "%s: ieee80211_probereq_get failed!\n", 
		            __func__);
		return -ENOMEM;
	}

#ifdef ROAM_OFFLOAD
	if (priv->join_status != XRADIO_JOIN_STATUS_STA) {
		if (req->channels[0]->band == NL80211_BAND_2GHZ)
			hw_priv->num_scanchannels = 0;
		else
			hw_priv->num_scanchannels = hw_priv->num_2g_channels;

		for (i=0; i < req->n_channels; i++) {
			hw_priv->scan_channels[hw_priv->num_scanchannels + i].number = \
				req->channels[i]->hw_value;
			if (req->channels[i]->flags & IEEE80211_CHAN_NO_IR) {
				hw_priv->scan_channels[hw_priv->num_scanchannels + i].minChannelTime = 50;
				hw_priv->scan_channels[hw_priv->num_scanchannels + i].maxChannelTime = 110;
			}
			else {
				hw_priv->scan_channels[hw_priv->num_scanchannels + i].minChannelTime = 10;
				hw_priv->scan_channels[hw_priv->num_scanchannels + i].maxChannelTime = 40;
				hw_priv->scan_channels[hw_priv->num_scanchannels + i].number |= \
					XRADIO_SCAN_TYPE_ACTIVE;
			}
			hw_priv->scan_channels[hw_priv->num_scanchannels + i].txPowerLevel = \
				req->channels[i]->max_power;
			if (req->channels[0]->band == NL80211_BAND_5GHZ)
				hw_priv->scan_channels[hw_priv->num_scanchannels + i].number |= \
					XRADIO_SCAN_BAND_5G;
		}
		if (req->channels[0]->band == NL80211_BAND_2GHZ)
			hw_priv->num_2g_channels = req->n_channels;
		else
			hw_priv->num_5g_channels = req->n_channels;
	}
	hw_priv->num_scanchannels = hw_priv->num_2g_channels + hw_priv->num_5g_channels;
#endif /*ROAM_OFFLOAD*/

	/* will be unlocked in xradio_scan_work() */
	down(&hw_priv->scan.lock);
	mutex_lock(&hw_priv->conf_mutex);

		if (frame.skb) {
			int ret = 0;
			if (priv->if_id == 0)
				xradio_remove_wps_p2p_ie(&frame);
			ret = wsm_set_template_frame(hw_priv, &frame, priv->if_id);
			if (ret) {
				mutex_unlock(&hw_priv->conf_mutex);
				up(&hw_priv->scan.lock);
				dev_kfree_skb(frame.skb);
				scan_printk(XRADIO_DBG_ERROR, "%s: wsm_set_template_frame failed: %d.\n",
				             __func__, ret);
				return ret;
			}
		}

		wsm_vif_lock_tx(priv);

		BUG_ON(hw_priv->scan.req);
		hw_priv->scan.req     = req;
		hw_priv->scan.n_ssids = 0;
		hw_priv->scan.status  = 0;
		hw_priv->scan.begin   = &req->channels[0];
		hw_priv->scan.curr    = hw_priv->scan.begin;
		hw_priv->scan.end     = &req->channels[req->n_channels];
		hw_priv->scan.output_power = hw_priv->output_power;
		hw_priv->scan.if_id = priv->if_id;
		/* TODO:COMBO: Populate BIT4 in scanflags to decide on which MAC
		 * address the SCAN request will be sent */

		for (i = 0; i < req->n_ssids; ++i) {
			struct wsm_ssid *dst = &hw_priv->scan.ssids[hw_priv->scan.n_ssids];
			BUG_ON(req->ssids[i].ssid_len > sizeof(dst->ssid));
			memcpy(&dst->ssid[0], req->ssids[i].ssid, sizeof(dst->ssid));
			dst->length = req->ssids[i].ssid_len;
			++hw_priv->scan.n_ssids;
		}

		mutex_unlock(&hw_priv->conf_mutex);

		if (frame.skb)
			dev_kfree_skb(frame.skb);
		queue_work(hw_priv->workqueue, &hw_priv->scan.work);

	return 0;
}

#ifdef ROAM_OFFLOAD
int xradio_hw_sched_scan_start(struct ieee80211_hw *hw,
		   struct ieee80211_vif *vif,
		   struct cfg80211_sched_scan_request *req,
		   struct ieee80211_sched_scan_ies *ies)
{
	struct xradio_common *hw_priv = hw->priv;
	struct xradio_vif *priv = xrwl_get_vif_from_ieee80211(vif);
	struct wsm_template_frame frame = {
		.frame_type = WSM_FRAME_TYPE_PROBE_REQUEST,
	};
	int i;
	

	scan_printk(XRADIO_DBG_WARN, "Scheduled scan request-->.\n");
	if (!priv->vif)
		return -EINVAL;

	/* Scan when P2P_GO corrupt firmware MiniAP mode */
	if (priv->join_status == XRADIO_JOIN_STATUS_AP) {
		scan_printk(XRADIO_DBG_WARN,"%s, can't scan in AP mode!\n", __func__);
		return -EOPNOTSUPP;
	}

	scan_printk(XRADIO_DBG_WARN, "Scheduled scan: n_ssids %d, ssid[0].len = %d\n", 
	            req->n_ssids, req->ssids[0].ssid_len);
	if (req->n_ssids == 1 && !req->ssids[0].ssid_len)
		req->n_ssids = 0;

	scan_printk(XRADIO_DBG_NIY, "[SCAN] Scan request for %d SSIDs.\n", 
	            req->n_ssids);

	if (req->n_ssids > hw->wiphy->max_scan_ssids) [
		scan_printk(XRADIO_DBG_ERROR, "%s: ssids is too much(%d)\n", 
		            __func__, req->n_ssids);
		return -EINVAL;
	}

	frame.skb = ieee80211_probereq_get(hw, priv->vif, NULL, 0,
	                                   ies->ie[0], ies->len[0]);
	if (!frame.skb) {
		scan_printk(XRADIO_DBG_ERROR, "%s: ieee80211_probereq_get failed!\n", 
		            __func__);
		return -ENOMEM;
	}

	/* will be unlocked in xradio_scan_work() */
	down(&hw_priv->scan.lock);
	mutex_lock(&hw_priv->conf_mutex);
	if (frame.skb) {
		int ret;
		if (priv->if_id == 0)
			xradio_remove_wps_p2p_ie(&frame);
		ret = wsm_set_template_frame(hw_priv, &frame, priv->if_id);
		if (0 == ret) {
			/* Host want to be the probe responder. */
			ret = wsm_set_probe_responder(priv, true);
		}
		if (ret) {
			mutex_unlock(&hw_priv->conf_mutex);
			up(&hw_priv->scan.lock);
			dev_kfree_skb(frame.skb);
			scan_printk(XRADIO_DBG_ERROR, "%s: wsm_set_probe_responder failed: %d.\n",
				             __func__, ret);
			return ret;
		}
	}

	wsm_lock_tx(hw_priv);
	BUG_ON(hw_priv->scan.req);
	hw_priv->scan.sched_req = req;
	hw_priv->scan.n_ssids = 0;
	hw_priv->scan.status = 0;
	hw_priv->scan.begin = &req->channels[0];
	hw_priv->scan.curr = hw_priv->scan.begin;
	hw_priv->scan.end = &req->channels[req->n_channels];
	hw_priv->scan.output_power = hw_priv->output_power;

	for (i = 0; i < req->n_ssids; ++i) {
		u8 j;
		struct wsm_ssid *dst = &hw_priv->scan.ssids[hw_priv->scan.n_ssids];
		BUG_ON(req->ssids[i].ssid_len > sizeof(dst->ssid));
		memcpy(&dst->ssid[0], req->ssids[i].ssid, sizeof(dst->ssid));
		dst->length = req->ssids[i].ssid_len;
		++hw_priv->scan.n_ssids;
		scan_printk(XRADIO_DBG_NIY, "SSID %d\n",i);
		for(j=0; j<req->ssids[i].ssid_len; j++)
			scan_printk(XRADIO_DBG_NIY, "0x%x\n", req->ssids[i].ssid[j]);
	}
	mutex_unlock(&hw_priv->conf_mutex);

	if (frame.skb)
		dev_kfree_skb(frame.skb);
	queue_work(hw_priv->workqueue, &hw_priv->scan.swork);
	scan_printk(XRADIO_DBG_NIY, "<-- Scheduled scan request.\n");
	return 0;
}
#endif /*ROAM_OFFLOAD*/

void xradio_scan_work(struct work_struct *work)
{
	struct xradio_common *hw_priv = container_of(work,
						struct xradio_common,
						scan.work);
	struct xradio_vif *priv;
	struct ieee80211_channel **it;
	struct wsm_scan scan = {
		.scanType = WSM_SCAN_TYPE_FOREGROUND,
		.scanFlags = 0, /* TODO:COMBO */
		//.scanFlags = WSM_SCAN_FLAG_SPLIT_METHOD, /* TODO:COMBO */
	};
	bool first_run;
	int i;
	const u32 ProbeRequestTime  = 2;
	const u32 ChannelRemainTime = 15;
	u32 maxChannelTime;
	struct cfg80211_scan_info scan_info;


	priv = __xrwl_hwpriv_to_vifpriv(hw_priv, hw_priv->scan.if_id);

	/*TODO: COMBO: introduce locking so vif is not removed in meanwhile */
	if (!priv) {
		scan_printk(XRADIO_DBG_WARN, "interface removed, "
		            "ignoring scan work\n");
		return;
	}

	if (priv->if_id)
		scan.scanFlags |= WSM_FLAG_MAC_INSTANCE_1;
	else
		scan.scanFlags &= ~WSM_FLAG_MAC_INSTANCE_1;

	/* No need to set WSM_SCAN_FLAG_FORCE_BACKGROUND in BSS_LOSS work. 
	 * yangfh 2015-11-11 18:45:02 */
	//xradio_for_each_vif(hw_priv, vif, i) {
	//	if (!vif)
	//		continue;
	//	if (vif->bss_loss_status > XRADIO_BSS_LOSS_NONE)
	//		scan.scanFlags |= WSM_SCAN_FLAG_FORCE_BACKGROUND;
	//}

	first_run = (hw_priv->scan.begin == hw_priv->scan.curr &&
	             hw_priv->scan.begin != hw_priv->scan.end);
	if (first_run) {
		/* Firmware gets crazy if scan request is sent
		 * when STA is joined but not yet associated.
		 * Force unjoin in this case. */
		if (cancel_delayed_work_sync(&priv->join_timeout) > 0)
			xradio_join_timeout(&priv->join_timeout.work);
	}

	mutex_lock(&hw_priv->conf_mutex);
	if (first_run) {
#if 0
			if (priv->join_status == XRADIO_JOIN_STATUS_STA &&
			    !(priv->powersave_mode.pmMode & WSM_PSM_PS)) {
				struct wsm_set_pm pm = priv->powersave_mode;
				pm.pmMode = WSM_PSM_PS;
				xradio_set_pm(priv, &pm);
			} else
#endif
			if (priv->join_status == XRADIO_JOIN_STATUS_MONITOR) {
				/* FW bug: driver has to restart p2p-dev mode
				 * after scan */
				xradio_disable_listening(priv);
			}
	}

	if (!hw_priv->scan.req || (hw_priv->scan.curr == hw_priv->scan.end)) {
			if (hw_priv->scan.output_power != hw_priv->output_power) {
			/* TODO:COMBO: Change when mac80211 implementation
			 * is available for output power also */
					WARN_ON(wsm_set_output_power(hw_priv, hw_priv->output_power * 10,
					                             priv->if_id));
			}

#if 0
		if (priv->join_status == XRADIO_JOIN_STATUS_STA &&
		    !(priv->powersave_mode.pmMode & WSM_PSM_PS))
			xradio_set_pm(priv, &priv->powersave_mode);
#endif

		if (hw_priv->scan.status < 0)
			scan_printk(XRADIO_DBG_ERROR, "Scan failed (%d).\n", hw_priv->scan.status);
		else if (hw_priv->scan.req)
			scan_printk(XRADIO_DBG_NIY, "Scan completed.\n");
		else
			scan_printk(XRADIO_DBG_NIY, "Scan canceled.\n");

		hw_priv->scan.req = NULL;
		xradio_scan_restart_delayed(priv);
		wsm_unlock_tx(hw_priv);
		mutex_unlock(&hw_priv->conf_mutex);
		memset(&scan_info, 0, sizeof(scan_info));
		scan_info.aborted = hw_priv->scan.status ? 1 : 0;
		ieee80211_scan_completed(hw_priv->hw, &scan_info);
		up(&hw_priv->scan.lock);
		return;

	} else {
		struct ieee80211_channel *first = *hw_priv->scan.curr;
		for (it = hw_priv->scan.curr + 1, i = 1;
		     it != hw_priv->scan.end && i < WSM_SCAN_MAX_NUM_OF_CHANNELS;
		     ++it, ++i) {
			if ((*it)->band != first->band)
				break;
			if (((*it)->flags ^ first->flags) & IEEE80211_CHAN_NO_IR)
				break;
			if (!(first->flags & IEEE80211_CHAN_NO_IR) &&
			    (*it)->max_power != first->max_power)
				break;
		}
		scan.band = first->band;

		if (hw_priv->scan.req->no_cck)
			scan.maxTransmitRate = WSM_TRANSMIT_RATE_6;
		else
			scan.maxTransmitRate = WSM_TRANSMIT_RATE_1;

		/* TODO: Is it optimal? */
		scan.numOfProbeRequests = (first->flags & IEEE80211_CHAN_NO_IR) ? 0 : 2;

		scan.numOfSSIDs = hw_priv->scan.n_ssids;
		scan.ssids = &hw_priv->scan.ssids[0];
		scan.numOfChannels = it - hw_priv->scan.curr;
		/* TODO: Is it optimal? */
		scan.probeDelay = 100;
		/* It is not stated in WSM specification, however
		 * FW team says that driver may not use FG scan
		 * when joined. */
		if (priv->join_status == XRADIO_JOIN_STATUS_STA) {
			scan.scanType = WSM_SCAN_TYPE_BACKGROUND;
			scan.scanFlags = WSM_SCAN_FLAG_FORCE_BACKGROUND;
		}
		scan.ch = kzalloc(sizeof(struct wsm_scan_ch[it - hw_priv->scan.curr]), GFP_KERNEL);
		if (!scan.ch) {
			hw_priv->scan.status = -ENOMEM;
			scan_printk(XRADIO_DBG_ERROR, "xr_kzalloc wsm_scan_ch failed.\n");
			goto fail;
		}
		maxChannelTime = (scan.numOfSSIDs * scan.numOfProbeRequests *ProbeRequestTime) + 
		                  ChannelRemainTime;
		maxChannelTime = (maxChannelTime < 35) ? 35 : maxChannelTime;
		for (i = 0; i < scan.numOfChannels; ++i) {
			scan.ch[i].number = hw_priv->scan.curr[i]->hw_value;


				if (hw_priv->scan.curr[i]->flags & IEEE80211_CHAN_NO_IR) {
					scan.ch[i].minChannelTime = 50;
					scan.ch[i].maxChannelTime = 110;
				} else {
					scan.ch[i].minChannelTime = 15;
					scan.ch[i].maxChannelTime = maxChannelTime;
				}


		}

			if (!(first->flags & IEEE80211_CHAN_NO_IR) &&
			    hw_priv->scan.output_power != first->max_power) {
			    hw_priv->scan.output_power = first->max_power;
				/* TODO:COMBO: Change after mac80211 implementation
				* complete */
				WARN_ON(wsm_set_output_power(hw_priv, hw_priv->scan.output_power * 10,
				                             priv->if_id));
			}

			down(&hw_priv->scan.status_lock);
			hw_priv->scan.status = xradio_scan_start(priv, &scan);

		kfree(scan.ch);
		if (WARN_ON(hw_priv->scan.status)) {
			scan_printk(XRADIO_DBG_ERROR, "scan failed, status=%d.\n",
			            hw_priv->scan.status);
			up(&hw_priv->scan.status_lock);
			goto fail;
		}
		up(&hw_priv->scan.status_lock);
		hw_priv->scan.curr = it;
	}
	mutex_unlock(&hw_priv->conf_mutex);
	return;

fail:
	hw_priv->scan.curr = hw_priv->scan.end;
	mutex_unlock(&hw_priv->conf_mutex);
	queue_work(hw_priv->workqueue, &hw_priv->scan.work);
	return;
}

#ifdef ROAM_OFFLOAD
void xradio_sched_scan_work(struct work_struct *work)
{
	struct xradio_common *hw_priv = container_of(work, struct xradio_common,
		scan.swork);
	struct wsm_scan scan;
	struct wsm_ssid scan_ssid;
	int i;
	struct xradio_vif *priv = NULL;


	priv = xrwl_hwpriv_to_vifpriv(hw_priv, hw_priv->scan.if_id);
	if (unlikely(!priv)) {
		WARN_ON(1);
		return;
	}

	spin_unlock(&priv->vif_lock);
	/* Firmware gets crazy if scan request is sent
	 * when STA is joined but not yet associated.
	 * Force unjoin in this case. */
	if (cancel_delayed_work_sync(&priv->join_timeout) > 0) {
		xradio_join_timeout(&priv->join_timeout.work);
	}
	mutex_lock(&hw_priv->conf_mutex);
	hw_priv->auto_scanning = 1;
	scan.band = 0;

	if (priv->join_status == XRADIO_JOIN_STATUS_STA)
		scan.scanType = 3; /* auto background */
	else
		scan.scanType = 2; /* auto foreground */

	scan.scanFlags = 0x01; /* bit 0 set => forced background scan */
	scan.maxTransmitRate = WSM_TRANSMIT_RATE_6;
	scan.autoScanInterval = (0xba << 24)|(30 * 1024); /* 30 seconds, -70 rssi */
	scan.numOfProbeRequests = 1;
	//scan.numOfChannels = 11;
	scan.numOfChannels = hw_priv->num_scanchannels;
	scan.numOfSSIDs = 1;
	scan.probeDelay = 100;
	scan_ssid.length = priv->ssid_length;
	memcpy(scan_ssid.ssid, priv->ssid, priv->ssid_length);
	scan.ssids = &scan_ssid;

	scan.ch = xr_kzalloc(sizeof(struct wsm_scan_ch[scan.numOfChannels]), false);
	if (!scan.ch) {
		scan_printk(XRADIO_DBG_ERROR, "xr_kzalloc wsm_scan_ch failed.\n");
		hw_priv->scan.status = -ENOMEM;
		goto fail;
	}

	for (i = 0; i < scan.numOfChannels; i++) {
		scan.ch[i].number = hw_priv->scan_channels[i].number;
		scan.ch[i].minChannelTime = hw_priv->scan_channels[i].minChannelTime;
		scan.ch[i].maxChannelTime = hw_priv->scan_channels[i].maxChannelTime;
		scan.ch[i].txPowerLevel = hw_priv->scan_channels[i].txPowerLevel;
	}

#if 0
	for (i = 1; i <= scan.numOfChannels; i++) {
		scan.ch[i-1].number = i;
		scan.ch[i-1].minChannelTime = 10;
		scan.ch[i-1].maxChannelTime = 40;
	}
#endif

	hw_priv->scan.status = xradio_sched_scan_start(priv, &scan);
	kfree(scan.ch);
	if (hw_priv->scan.status) {
		scan_printk(XRADIO_DBG_ERROR, "scan failed, status=%d.\n",
			            hw_priv->scan.status);
		goto fail;
	}
	mutex_unlock(&hw_priv->conf_mutex);
	return;

fail:
	mutex_unlock(&hw_priv->conf_mutex);
	queue_work(hw_priv->workqueue, &hw_priv->scan.swork);
	return;
}

void xradio_hw_sched_scan_stop(struct xradio_common *hw_priv)
{
	struct xradio_vif *priv = NULL;

	priv = xrwl_hwpriv_to_vifpriv(hw_priv,hw_priv->scan.if_id);
	if (unlikely(!priv))
		return;

	spin_unlock(&priv->vif_lock);
	wsm_stop_scan(hw_priv, priv->if_id);

	return;
}
#endif /*ROAM_OFFLOAD*/


static void xradio_scan_restart_delayed(struct xradio_vif *priv)
{
	struct xradio_common *hw_priv = xrwl_vifpriv_to_hwpriv(priv);


	if (priv->delayed_link_loss) {
		int tmo = hw_priv->scan.direct_probe ? 0 : priv->cqm_beacon_loss_count;

		priv->delayed_link_loss = 0;
		/* Restart beacon loss timer and requeue
		   BSS loss work. */
		scan_printk(XRADIO_DBG_WARN, "[CQM] Requeue BSS loss in %d "
		           "beacons.\n", tmo);
		cancel_delayed_work_sync(&priv->bss_loss_work);
		queue_delayed_work(hw_priv->workqueue, &priv->bss_loss_work,
		                   tmo * HZ / 10);
		
	}

	/* FW bug: driver has to restart p2p-dev mode after scan. */
	if (priv->join_status == XRADIO_JOIN_STATUS_MONITOR) {
		/*xradio_enable_listening(priv);*/
		WARN_ON(1);
		xradio_update_filtering(priv);
		scan_printk(XRADIO_DBG_WARN, "driver has to restart "
		            "p2p-dev mode after scan");
	}

	if (atomic_xchg(&priv->delayed_unjoin, 0)) {
		if (queue_work(hw_priv->workqueue, &priv->unjoin_work) <= 0)
			wsm_unlock_tx(hw_priv);
	}
}

static void xradio_scan_complete(struct xradio_common *hw_priv, int if_id)
{
	struct xradio_vif *priv;
	atomic_xchg(&hw_priv->recent_scan, 0);


	if (hw_priv->scan.direct_probe) {
		mutex_lock(&hw_priv->conf_mutex);
		priv = __xrwl_hwpriv_to_vifpriv(hw_priv, if_id);
		if (priv) {
			scan_printk(XRADIO_DBG_MSG, "Direct probe complete.\n");
			xradio_scan_restart_delayed(priv);
		} else {
			scan_printk(XRADIO_DBG_MSG, "Direct probe complete without interface!\n");
		}
		mutex_unlock(&hw_priv->conf_mutex);
		hw_priv->scan.direct_probe = 0;
		up(&hw_priv->scan.lock);
		wsm_unlock_tx(hw_priv);
	} else {
		xradio_scan_work(&hw_priv->scan.work);
	}
}

void xradio_scan_complete_cb(struct xradio_common *hw_priv,
                             struct wsm_scan_complete *arg)
{
	struct xradio_vif *priv = xrwl_hwpriv_to_vifpriv(hw_priv,
					hw_priv->scan.if_id);


	if (unlikely(!priv))
		return;

#ifdef ROAM_OFFLOAD
	if (hw_priv->auto_scanning)
		queue_delayed_work(hw_priv->workqueue, &hw_priv->scan.timeout, 0);
#endif /*ROAM_OFFLOAD*/

	if (unlikely(priv->mode == NL80211_IFTYPE_UNSPECIFIED)) {
		/* STA is stopped. */
		spin_unlock(&priv->vif_lock);
		scan_printk(XRADIO_DBG_WARN, "%s: priv->mode UNSPECIFIED.\n", __func__);
		return;
	}
	spin_unlock(&priv->vif_lock);

	/*
	if(hw_priv->scan.status == -ETIMEDOUT)
		scan_printk(XRADIO_DBG_WARN, "Scan timeout already occured. "
		            "Don't cancel work");
	if ((hw_priv->scan.status != -ETIMEDOUT) &&
		(cancel_delayed_work_sync(&hw_priv->scan.timeout) > 0)) {
		hw_priv->scan.status = 1;
		queue_delayed_work(hw_priv->workqueue, &hw_priv->scan.timeout, 0);
	}
	*/  // should not involve status as a condition

	if (cancel_delayed_work_sync(&hw_priv->scan.timeout) > 0) {
		down(&hw_priv->scan.status_lock);
		hw_priv->scan.status = 1;
		up(&hw_priv->scan.status_lock);
		queue_delayed_work(hw_priv->workqueue, &hw_priv->scan.timeout, 0);
	}
}

void xradio_scan_timeout(struct work_struct *work)
{
	struct xradio_common *hw_priv =
		container_of(work, struct xradio_common, scan.timeout.work);


	if (likely(atomic_xchg(&hw_priv->scan.in_progress, 0))) {
		if (hw_priv->scan.status > 0)
			hw_priv->scan.status = 0;
		else if (!hw_priv->scan.status) {
			scan_printk(XRADIO_DBG_WARN, "Timeout waiting for scan "
			            "complete notification.\n");
			hw_priv->scan.status = -ETIMEDOUT;
			hw_priv->scan.curr   = hw_priv->scan.end;
			WARN_ON(wsm_stop_scan(hw_priv, hw_priv->scan.if_id ? 1 : 0));
		}
		xradio_scan_complete(hw_priv, hw_priv->scan.if_id);
#ifdef ROAM_OFFLOAD
	} else if (hw_priv->auto_scanning) {
		hw_priv->auto_scanning = 0;
		ieee80211_sched_scan_results(hw_priv->hw);
#endif /*ROAM_OFFLOAD*/
	}
}

void xradio_probe_work(struct work_struct *work)
{
	struct xradio_common *hw_priv =
		container_of(work, struct xradio_common, scan.probe_work.work);
	struct xradio_vif *priv;
	u8 queueId = xradio_queue_get_queue_id(hw_priv->pending_frame_id);
	struct xradio_queue *queue = &hw_priv->tx_queue[queueId];
	const struct xradio_txpriv *txpriv;
	struct wsm_tx *wsm;
	struct wsm_template_frame frame = {
		.frame_type = WSM_FRAME_TYPE_PROBE_REQUEST,
	};
	struct wsm_ssid ssids[1] = {{
		.length = 0,
	} };
	struct wsm_scan_ch ch[1] = {{
		.minChannelTime = 0,
		.maxChannelTime = 10,
	} };
	struct wsm_scan scan = {
		.scanType = WSM_SCAN_TYPE_FOREGROUND,
		.numOfProbeRequests = 1,
		.probeDelay = 0,
		.numOfChannels = 1,
		.ssids = ssids,
		.ch = ch,
	};
	u8 *ies;
	size_t ies_len;
	int ret = 1;
	scan_printk(XRADIO_DBG_MSG, "%s:Direct probe.\n", __func__);

	BUG_ON(queueId >= 4);
	BUG_ON(!hw_priv->channel);

	mutex_lock(&hw_priv->conf_mutex);
	if (unlikely(down_trylock(&hw_priv->scan.lock))) {
		/* Scan is already in progress. Requeue self. */
		schedule();
		queue_delayed_work(hw_priv->workqueue, &hw_priv->scan.probe_work, 
		                   HZ / 10);
		mutex_unlock(&hw_priv->conf_mutex);
		return;
	}

	if (xradio_queue_get_skb(queue,	hw_priv->pending_frame_id, &frame.skb, &txpriv)) {
		up(&hw_priv->scan.lock);
		mutex_unlock(&hw_priv->conf_mutex);
		wsm_unlock_tx(hw_priv);
		scan_printk(XRADIO_DBG_ERROR, "%s:xradio_queue_get_skb error!\n", __func__);
		return;
	}
	priv = __xrwl_hwpriv_to_vifpriv(hw_priv, txpriv->if_id);
	if (!priv) {
		up(&hw_priv->scan.lock);
		mutex_unlock(&hw_priv->conf_mutex);
		scan_printk(XRADIO_DBG_ERROR, "%s:priv error!\n", __func__);
		return;
	}
	wsm = (struct wsm_tx *)frame.skb->data;
	scan.maxTransmitRate = wsm->maxTxRate;
	scan.band = (hw_priv->channel->band == NL80211_BAND_5GHZ) ?
	             WSM_PHY_BAND_5G : WSM_PHY_BAND_2_4G;
	if (priv->join_status == XRADIO_JOIN_STATUS_STA) {
		scan.scanType  = WSM_SCAN_TYPE_BACKGROUND;
		scan.scanFlags = WSM_SCAN_FLAG_FORCE_BACKGROUND;
		if (priv->if_id)
			scan.scanFlags |= WSM_FLAG_MAC_INSTANCE_1;
		else
			scan.scanFlags &= ~WSM_FLAG_MAC_INSTANCE_1;
	}

	/* No need to set WSM_SCAN_FLAG_FORCE_BACKGROUND in BSS_LOSS work. 
	 * yangfh 2015-11-11 18:45:02 */
	//xradio_for_each_vif(hw_priv, vif, i) {
	//	if (!vif)
	//		continue;
	//	if (vif->bss_loss_status > XRADIO_BSS_LOSS_NONE)
	//		scan.scanFlags |= WSM_SCAN_FLAG_FORCE_BACKGROUND;
	//}
	
	ch[0].number = hw_priv->channel->hw_value;
	skb_pull(frame.skb, txpriv->offset);
	ies = &frame.skb->data[sizeof(struct ieee80211_hdr_3addr)];
	ies_len = frame.skb->len - sizeof(struct ieee80211_hdr_3addr);

	if (ies_len) {
		u8 *ssidie = (u8 *)cfg80211_find_ie(WLAN_EID_SSID, ies, ies_len);
		if (ssidie && ssidie[1] && ssidie[1] <= sizeof(ssids[0].ssid)) {
			u8 *nextie = &ssidie[2 + ssidie[1]];
			/* Remove SSID from the IE list. It has to be provided
			 * as a separate argument in xradio_scan_start call */

			/* Store SSID localy */
			ssids[0].length = ssidie[1];
			memcpy(ssids[0].ssid, &ssidie[2], ssids[0].length);
			scan.numOfSSIDs = 1;

			/* Remove SSID from IE list */
			ssidie[1] = 0;
			memmove(&ssidie[2], nextie, &ies[ies_len] - nextie);
			skb_trim(frame.skb, frame.skb->len - ssids[0].length);
		}
	}

	if (priv->if_id == 0)
		xradio_remove_wps_p2p_ie(&frame);

	/* FW bug: driver has to restart p2p-dev mode after scan */
	if (priv->join_status == XRADIO_JOIN_STATUS_MONITOR) {
		WARN_ON(1);
		/*xradio_disable_listening(priv);*/
	}
	ret = WARN_ON(wsm_set_template_frame(hw_priv, &frame,
				priv->if_id));

	hw_priv->scan.direct_probe = 1;
	hw_priv->scan.if_id = priv->if_id;
	if (!ret) {
		wsm_flush_tx(hw_priv);
		ret = WARN_ON(xradio_scan_start(priv, &scan));
	}
	mutex_unlock(&hw_priv->conf_mutex);

	skb_push(frame.skb, txpriv->offset);
	if (!ret)
		IEEE80211_SKB_CB(frame.skb)->flags |= IEEE80211_TX_STAT_ACK;

		BUG_ON(xradio_queue_remove(queue, hw_priv->pending_frame_id));

	if (ret) {
		hw_priv->scan.direct_probe = 0;
		up(&hw_priv->scan.lock);
		wsm_unlock_tx(hw_priv);
	}

	return;
}
