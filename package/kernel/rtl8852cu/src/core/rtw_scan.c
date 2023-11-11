/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#include <drv_types.h>

/*rtw_mlme.c*/
void rtw_init_sitesurvey_parm(_adapter *padapter, struct sitesurvey_parm *pparm)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	_rtw_memset(pparm, 0, sizeof(struct sitesurvey_parm));
	pparm->scan_mode = pmlmepriv->scan_mode;
}

#ifdef CONFIG_SET_SCAN_DENY_TIMER
inline bool rtw_is_scan_deny(_adapter *adapter)
{
	struct mlme_priv *mlmepriv = &adapter->mlmepriv;
	return (ATOMIC_READ(&mlmepriv->set_scan_deny) != 0) ? _TRUE : _FALSE;
}
inline void rtw_clear_scan_deny(_adapter *adapter)
{
	struct mlme_priv *mlmepriv = &adapter->mlmepriv;
	ATOMIC_SET(&mlmepriv->set_scan_deny, 0);
	if (0)
		RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(adapter));
}

void rtw_set_scan_deny_timer_hdl(void *ctx)
{
	_adapter *adapter = (_adapter *)ctx;

	rtw_clear_scan_deny(adapter);
}
void rtw_set_scan_deny(_adapter *adapter, u32 ms)
{
	struct mlme_priv *mlmepriv = &adapter->mlmepriv;
	if (0)
		RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(adapter));
	ATOMIC_SET(&mlmepriv->set_scan_deny, 1);
	_set_timer(&mlmepriv->set_scan_deny_timer, ms);
}
#endif

void rtw_drv_scan_by_self(_adapter *padapter, u8 reason)
{
	struct sitesurvey_parm *parm = NULL;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	int i;
#if 1
	u8 ssc_chk;

	ssc_chk = rtw_sitesurvey_condition_check(padapter, _FALSE);
	if( ssc_chk == SS_DENY_BUSY_TRAFFIC) {
		#ifdef CONFIG_LAYER2_ROAMING
		if (rtw_chk_roam_flags(padapter, RTW_ROAM_ACTIVE) && pmlmepriv->need_to_roam == _TRUE) {
			RTW_INFO(FUNC_ADPT_FMT" need to roam, don't care BusyTraffic\n", FUNC_ADPT_ARG(padapter));
		 } else
		#endif
		{
			RTW_INFO(FUNC_ADPT_FMT" exit BusyTraffic\n", FUNC_ADPT_ARG(padapter));
			goto exit;
		}
	} else if (ssc_chk != SS_ALLOW) {
		goto exit;
	}

	if (!rtw_is_adapter_up(padapter))
		goto exit;
#else
	if (rtw_is_scan_deny(padapter))
		goto exit;

	if (!rtw_is_adapter_up(padapter))
		goto exit;

	if (rtw_mi_busy_traffic_check(padapter)) {
#ifdef CONFIG_LAYER2_ROAMING
		if (rtw_chk_roam_flags(padapter, RTW_ROAM_ACTIVE) && pmlmepriv->need_to_roam == _TRUE) {
			RTW_INFO("need to roam, don't care BusyTraffic\n");
		} else
#endif
		{
			RTW_INFO(FUNC_ADPT_FMT" exit BusyTraffic\n", FUNC_ADPT_ARG(padapter));
			goto exit;
		}
	}
	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) && check_fwstate(pmlmepriv, WIFI_UNDER_WPS)) {
		RTW_INFO(FUNC_ADPT_FMT" WIFI_AP_STATE && WIFI_UNDER_WPS\n", FUNC_ADPT_ARG(padapter));
		goto exit;
	}
	if (check_fwstate(pmlmepriv, (WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING)) == _TRUE) {
		RTW_INFO(FUNC_ADPT_FMT" WIFI_UNDER_SURVEY|WIFI_UNDER_LINKING\n", FUNC_ADPT_ARG(padapter));
		goto exit;
	}

#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_mi_buddy_check_fwstate(padapter, (WIFI_UNDER_SURVEY | WIFI_UNDER_LINKING | WIFI_UNDER_WPS))) {
		RTW_INFO(FUNC_ADPT_FMT", but buddy_intf is under scanning or linking or wps_phase\n", FUNC_ADPT_ARG(padapter));
		goto exit;
	}
#endif
#endif

	RTW_INFO(FUNC_ADPT_FMT" reason:0x%02x\n", FUNC_ADPT_ARG(padapter), reason);

	parm = rtw_malloc(sizeof(*parm));
	if (parm == NULL)
		goto exit;

	/* only for 20/40 BSS */
	if (reason == RTW_AUTO_SCAN_REASON_2040_BSS) {
		rtw_init_sitesurvey_parm(padapter, parm);
		for (i=0;i<14;i++) {
			parm->ch[i].hw_value = i + 1;
			parm->ch[i].flags = RTW_IEEE80211_CHAN_PASSIVE_SCAN;
		}
		parm->ch_num = 14;
		rtw_sitesurvey_cmd(padapter, parm);
		goto exit;
	}

#ifdef CONFIG_RTW_MBO
#if defined(CONFIG_RTW_WNM) || defined(CONFIG_RTW_80211K)
	if ((reason == RTW_AUTO_SCAN_REASON_ROAM)
		&& (rtw_roam_nb_scan_list_set(padapter, parm)))
		goto exit;
#endif
#endif

	rtw_sitesurvey_cmd(padapter, NULL);
exit:
	if (parm)
		rtw_mfree(parm, sizeof(*parm));

	return;
}

#ifdef CONFIG_RTW_ACS
u8 rtw_set_acs_sitesurvey(_adapter *adapter)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(adapter);
	void *phl = adapter_to_dvobj(adapter)->phl;
	struct sitesurvey_parm parm;
	u8 uch;
	struct rtw_chan_def union_chdef = {0};
	u8 ch_num = 0;
	int i;
	enum band_type band;
	u8 (*center_chs_num)(u8) = NULL;
	u8 (*center_chs)(u8, u8) = NULL;
	u8 ret = _FAIL;

	if (rtw_phl_mr_get_chandef(phl, adapter->phl_role, padapter_link->wrlink,
		&union_chdef) != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("fun:%s line:%d get chandef failed\n", __FUNCTION__, __LINE__);
		rtw_warn_on(1);
		goto exit;
	}
	uch = union_chdef.chan;

	_rtw_memset(&parm, 0, sizeof(struct sitesurvey_parm));
	parm.scan_mode = RTW_PHL_SCAN_PASSIVE;
	parm.bw = CHANNEL_WIDTH_20;
	parm.acs = 1;

	for (band = BAND_ON_24G; band < BAND_MAX; band++) {
		if (band == BAND_ON_24G) {
			center_chs_num = center_chs_2g_num;
			center_chs = center_chs_2g;
		} else
		#if CONFIG_IEEE80211_BAND_5GHZ
		if (band == BAND_ON_5G) {
			center_chs_num = center_chs_5g_num;
			center_chs = center_chs_5g;
		} else
		#endif
		{
			center_chs_num = NULL;
			center_chs = NULL;
		}

		if (!center_chs_num || !center_chs)
			continue;

		if (rfctl->ch_sel_within_same_band) {
			if (rtw_is_2g_ch(uch) && band != BAND_ON_24G)
				continue;
			#if CONFIG_IEEE80211_BAND_5GHZ
			if (rtw_is_5g_ch(uch) && band != BAND_ON_5G)
				continue;
			#endif
		}

		ch_num = center_chs_num(CHANNEL_WIDTH_20);
		for (i = 0; i < ch_num && parm.ch_num < RTW_CHANNEL_SCAN_AMOUNT; i++) {
			parm.ch[parm.ch_num].hw_value = center_chs(CHANNEL_WIDTH_20, i);
			parm.ch[parm.ch_num].flags = RTW_IEEE80211_CHAN_PASSIVE_SCAN;
			parm.ch_num++;
		}
	}

	ret = rtw_sitesurvey_cmd(adapter, &parm);

exit:
	return ret;
}
#endif /* CONFIG_RTW_ACS */

static u32 _rtw_wait_scan_done(_adapter *adapter, u32 timeout_ms)
{
	systime start;
	u32 pass_ms;
	struct mlme_priv *pmlmepriv = &(adapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);
	u8 abort_timeout = false;

	start = rtw_get_current_time();

	while ((rtw_cfg80211_get_is_roch(adapter) == _TRUE || check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY))
	       && rtw_get_passing_time_ms(start) <= timeout_ms) {

		if (RTW_CANNOT_RUN(adapter_to_dvobj(adapter)))
			break;

		RTW_INFO(FUNC_NDEV_FMT"fw_state=WIFI_UNDER_SURVEY!\n", FUNC_NDEV_ARG(adapter->pnetdev));
		rtw_msleep_os(20);
		abort_timeout = true;
	}

	if (_TRUE == abort_timeout && check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY)) {
		if (!RTW_CANNOT_RUN(adapter_to_dvobj(adapter)))
			RTW_ERR(FUNC_NDEV_FMT"waiting for scan_abort time out!\n",
					FUNC_NDEV_ARG(adapter->pnetdev));
		pmlmeext->scan_abort_to = _TRUE;
		rtw_indicate_scan_done(adapter, _TRUE);
	}

	pmlmeext->scan_abort = _FALSE;
	RTW_INFO(FUNC_ADPT_FMT "- %s....scan_abort:%d\n",
		FUNC_ADPT_ARG(adapter), __func__, pmlmeext->scan_abort);
	pass_ms = rtw_get_passing_time_ms(start);

	RTW_INFO("%s scan timeout value:%d ms, total take:%d ms\n",
				__func__, timeout_ms, pass_ms);
	return pass_ms;
}

/*
* timeout_ms > 0:rtw_scan_abort_timeout , = 0:rtw_scan_wait_completed
*/
u32 rtw_scan_abort(_adapter *adapter, u32 timeout_ms)
{
	struct mlme_priv *pmlmepriv = &(adapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	u32 pass_ms = 0;

	if (rtw_cfg80211_get_is_roch(adapter) == _TRUE || check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY)) {
		pmlmeext->scan_abort = _TRUE;
		RTW_INFO(FUNC_ADPT_FMT "- %s....scan_abort:%d\n",
			FUNC_ADPT_ARG(adapter), __func__, pmlmeext->scan_abort);
		rtw_sctx_init(&pmlmeext->sitesurvey_res.sctx, timeout_ms);

		#ifdef CONFIG_CMD_SCAN
		if (pmlmeext->sitesurvey_res.scan_param)
			psts = rtw_phl_cmd_scan_cancel(adapter_to_dvobj(adapter)->phl,
					pmlmeext->sitesurvey_res.scan_param);
		#else
		psts = rtw_phl_scan_cancel(adapter_to_dvobj(adapter)->phl);
		#endif

		if (psts == RTW_PHL_STATUS_SUCCESS)
			rtw_sctx_wait(&pmlmeext->sitesurvey_res.sctx, __func__);
		pass_ms = _rtw_wait_scan_done(adapter, timeout_ms);
	}
	return pass_ms;
}

void rtw_scan_abort_no_wait(_adapter *adapter)
{
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;

	#ifdef CONFIG_CMD_SCAN
	if (pmlmeext->sitesurvey_res.scan_param)
		rtw_phl_cmd_scan_cancel(adapter_to_dvobj(adapter)->phl,
			pmlmeext->sitesurvey_res.scan_param);
	#else
	rtw_phl_scan_cancel(adapter_to_dvobj(adapter)->phl);
	#endif
}

static inline bool _rtw_scan_abort_check(_adapter *adapter, const char *caller)
{
	struct	mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	struct submit_ctx *sctx = &pmlmeext->sitesurvey_res.sctx;

	RTW_INFO(FUNC_ADPT_FMT "- %s....scan_abort:%d\n",
			FUNC_ADPT_ARG(adapter), __func__, pmlmeext->scan_abort);

	if (pmlmeext->scan_abort == _FALSE)
		return _FALSE;

	if (pmlmeext->scan_abort_to) {
		RTW_ERR("%s scan abort timeout\n", caller);
		rtw_warn_on(1);
	}

	pmlmeext->scan_abort = _FALSE;
	pmlmeext->scan_abort_to = _FALSE;
	if (sctx) {
		RTW_INFO("%s scan abort .....(%d ms)\n", caller, rtw_get_passing_time_ms(sctx->submit_time));
		rtw_sctx_done(&sctx);
	}
	return _TRUE;
}
static struct wlan_network *alloc_network(struct	mlme_priv *pmlmepriv) /* (_queue	*free_queue) */
{
	struct	wlan_network	*pnetwork;
	pnetwork = _rtw_alloc_network(pmlmepriv);
	return pnetwork;
}

static void update_current_network(_adapter *adapter, WLAN_BSSID_EX *pnetwork)
{
	struct	mlme_priv *pmlmepriv = &(adapter->mlmepriv);
	struct	_ADAPTER_LINK *adapter_link = NULL;
	struct	link_mlme_priv *lmlmepriv;

	if ((check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)) {
#ifdef CONFIG_80211BE_EHT
		if (pnetwork->is_mld)
			adapter_link = rtw_get_adapter_link_by_linkid(adapter, pnetwork->link_id);
#endif
		if(!adapter_link)
			adapter_link = GET_PRIMARY_LINK(adapter);

		lmlmepriv = &(adapter_link->mlmepriv);

		if (is_same_network(&(lmlmepriv->cur_network.network), pnetwork)) {
			rtw_update_network(&(lmlmepriv->cur_network.network), pnetwork, adapter, _TRUE);
			rtw_update_protection(adapter, (lmlmepriv->cur_network.network.IEs) + sizeof(NDIS_802_11_FIXED_IEs),
			      	lmlmepriv->cur_network.network.IELength);
		}
	}
}

#ifdef CONFIG_80211BE_EHT
void update_mld_link_from_ml_link_info(_adapter *adapter, struct wlan_network *pnetwork,
		struct rtw_phl_per_sta_profile_element *per_sta_profile)
{
	u8 *p;
	u8 eid_eht_ml = EID_EXT_MULTI_LINK;
	u32 update_ie_len = 0;
	u32 val;

	if (per_sta_profile == NULL)
		return;

	/* STA Control field */
	if (per_sta_profile->complete_profile)
		pnetwork->network.is_complete_profile = _TRUE;
	if (per_sta_profile->mac_addr_present)
		_rtw_memcpy(pnetwork->network.MacAddress, per_sta_profile->mac_addr, ETH_ALEN);
	if (per_sta_profile->bcn_interval_present) {
		pnetwork->network.Configuration.BeaconPeriod = per_sta_profile->bcn_interval;
		val = cpu_to_le32(per_sta_profile->bcn_interval);
		p = rtw_get_beacon_interval_from_ie(pnetwork->network.IEs);
		_rtw_memcpy(p , (unsigned char *)&val, _BEACON_ITERVAL_);
	}
	if (per_sta_profile->dtim_info_present) {
		/* ToDo CONFIG_RTW_MLD: update dtim in TIM element
		** which may also be related to tid-to-link mapping */
	}

	/* ToDo CONFIG_RTW_MLD: STA Info field */

	/* STA Profile field */
	/* Replace capability */
	p = rtw_get_capability_from_ie(pnetwork->network.IEs);
	_rtw_memcpy(p, per_sta_profile->sta_profile, _CAPABILITY_);

	update_ie_len = pnetwork->network.IELength - _FIXED_IE_LENGTH_;

	/* Update IEs with inheritance */
	rtw_update_ies_with_inheritance(pnetwork->network.IEs + _FIXED_IE_LENGTH_,
				&update_ie_len, per_sta_profile->sta_profile + _CAPABILITY_,
				per_sta_profile->sta_profile_len - _CAPABILITY_);

	pnetwork->network.IELength = update_ie_len +  _FIXED_IE_LENGTH_;

}

struct rtw_phl_tbtt_info *rtw_get_next_mld_tbtt_info(_adapter *adapter,
		struct rtw_phl_rnb_rpt_element *rnb_ele, u8 *start_idx)
{
	struct rtw_phl_neighbor_ap *nb_ap;
	struct tbtt_info_header *hdr;
	struct rtw_phl_tbtt_info *tbtt_info = NULL;
	u8 j;

	while (*start_idx < rnb_ele->nb_ap_num) {
		nb_ap = &rnb_ele->nb_aps[*start_idx];
		hdr = &nb_ap->tbtt_info_hdr;
		if (!hdr->is_legal || !hdr->mld_param_is_present)
			continue;
		for (j = 0; j < hdr->cnt + 1; j++) {
			tbtt_info = &nb_ap->tbtt_infos[j];
			if (tbtt_info->mld_param.mld_id == 0)
				return tbtt_info;
		}
		*start_idx++;
	}
	return NULL;
}

void update_mld_link_from_rnb_info(_adapter *adapter, struct wlan_network *pnetwork,
		struct rtw_phl_neighbor_ap *nb_ap, struct rtw_phl_tbtt_info *tbtt_info)
{
	struct tbtt_info_header *hdr = &nb_ap->tbtt_info_hdr;
	pnetwork->network.Configuration.DSConfig = nb_ap->ch;
	/* ToDo CONFIG_RTW_MLD: offset, short_ssid, bss_param, max_tx_pwr, mld_param.bss_params_chg_cnt */
	if (hdr->bssid_is_present)
		_rtw_memcpy(&pnetwork->network.MacAddress, tbtt_info->bssid, ETH_ALEN);
	if (hdr->mld_param_is_present)
		pnetwork->network.link_id = tbtt_info->mld_param.link_id;
}

u32 rtw_parse_reduced_nb_rpt(_adapter *adapter, u8 *ies, u32 ies_len,
		struct rtw_phl_rnb_rpt_element *rnb_ele)
{
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(adapter_to_dvobj(adapter));
	u32 rnb_rpt_ie_len;
	u8 *p;

	p = rtw_get_ie(ies, EID_REDUCED_NEIGHBOR_REPORT,
			&rnb_rpt_ie_len, ies_len);
	if (p == NULL || rnb_rpt_ie_len == 0)
		return 0;

	rtw_phl_parse_reduced_nb_rpt(phl_com, p + 2, rnb_rpt_ie_len, rnb_ele);
	return rnb_rpt_ie_len;
}

struct wlan_network *update_scanned_mld_network(_adapter *adapter, WLAN_BSSID_EX *target,
		struct wlan_network *choice, struct wlan_network *found_network, int target_find)
{
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	bool update_ie = _FALSE;
	u8 network_num = 0;
	struct wlan_mld_network *pmld_network = NULL;
	struct wlan_network *pnetwork = NULL;
	struct rtw_phl_ml_element *ml_ele = NULL;
	struct basic_ml *basic_ml;
	struct rtw_phl_rnb_rpt_element *rnb_ele = NULL;
	struct rtw_phl_tbtt_info *tbtt_info;
	u8 lidx = 0;
	u8 i = 0;
	u8 lnw_idx;

	ml_ele = (struct rtw_phl_ml_element *)rtw_zmalloc(sizeof(struct rtw_phl_ml_element));
	rnb_ele = (struct rtw_phl_rnb_rpt_element *)rtw_zmalloc(sizeof(struct rtw_phl_rnb_rpt_element));

	if (!ml_ele || !rnb_ele)
		goto _exit;

	/* Reduced Neighbor Report element */
	if (!rtw_parse_reduced_nb_rpt(adapter, target->IEs + _FIXED_IE_LENGTH_,
			target->IELength - _FIXED_IE_LENGTH_, rnb_ele))
		goto _exit;

	/* Multilink element */
	if (!rtw_parse_ml_ie(adapter, target->IEs + _FIXED_IE_LENGTH_,
				target->IELength - _FIXED_IE_LENGTH_, ml_ele))
		goto _exit;

	basic_ml =  &ml_ele->common_info.basic_ml;

	if (basic_ml->link_id_info_present)
		target->link_id = basic_ml->link_id;

	if (!target_find) {
		if (_rtw_queue_empty(&(pmlmepriv->free_bss_pool)) == _TRUE) {
			/* If there are no more slots, expire the choice */
			/* list_del_init(&choice->list); */
			pnetwork = choice;
			if (pnetwork == NULL)
				goto _exit;

			pmld_network = pnetwork->network.mld_network;
			_rtw_memcpy(pmld_network->mac_addr, basic_ml->mld_address, ETH_ALEN);
			network_num = pmld_network->network_num;
			do {
				pnetwork = GET_LINK_NETWORK(pmld_network, lidx);

				if (lidx == 0) {
					pnetwork->network.is_complete_profile = _TRUE;
					/* Replace the link network */
					_rtw_memcpy(&(pnetwork->network), target,  get_WLAN_BSSID_EX_sz(target));
				} else {
					tbtt_info = rtw_get_next_mld_tbtt_info(adapter, rnb_ele, &i);
					if (i >= rnb_ele->nb_ap_num || !tbtt_info)
						break;

					pnetwork->network.is_complete_profile = _FALSE;
					/* Replace the link network */
					_rtw_memcpy(&(pnetwork->network), target, get_WLAN_BSSID_EX_sz(target));
					update_mld_link_from_rnb_info(adapter, pnetwork, &rnb_ele->nb_aps[i], tbtt_info);
					i++;
					for (lnw_idx = 0; lnw_idx < ml_ele->profile_num; lnw_idx++) {
						if (pnetwork->network.link_id == ml_ele->profile[lnw_idx].link_id) {
							update_mld_link_from_ml_link_info(adapter, pnetwork, &ml_ele->profile[lnw_idx]);
							break;
						}
					}
				}

				pnetwork->bcn_keys_valid = 0;
				/* pnetwork is complete profile if lidx==0 or ml_ele->profile is found */
				if (pnetwork->network.is_complete_profile) {
					if (target->Reserved[0] == BSS_TYPE_BCN || target->Reserved[0] == BSS_TYPE_PROB_RSP)
						rtw_update_bcn_keys_of_network(pnetwork);
				}
				/* variable initialize */
				pnetwork->fixed = _FALSE;
				pnetwork->last_scanned = rtw_get_current_time();
				pnetwork->last_non_hidden_ssid_ap = pnetwork->last_scanned;
				#if defined(CONFIG_RTW_MESH) && CONFIG_RTW_MESH_ACNODE_PREVENT
				pnetwork->acnode_stime = 0;
				pnetwork->acnode_notify_etime = 0;
				#endif

				pnetwork->network_type = 0;
				pnetwork->aid = 0;
				pnetwork->join_res = 0;

				/* bss info not receving from the right channel */
				if (pnetwork->network.PhyInfo.SignalQuality == 101)
					pnetwork->network.PhyInfo.SignalQuality = 0;

				/* Link mld_network and network */
				pnetwork->network.mld_network = pmld_network;

				lidx++;

			} while (i < rnb_ele->nb_ap_num && lidx < network_num);
			/* Free the rest networks */
			while (lidx < network_num) {
				pnetwork = GET_LINK_NETWORK(pmld_network, lidx);
				_rtw_free_network_nolock(pmlmepriv, pnetwork);
				pmld_network->link_network[lidx] = NULL;
				pmld_network->network_num--;
				lidx++;
			}
		}
		else {
			/* MLD Mac Address */
			pmld_network = rtw_alloc_mld_network(adapter, basic_ml->mld_address);
			if (!pmld_network)
				goto _exit;
			target->Length = get_WLAN_BSSID_EX_sz(target);
			do {
				if (pmld_network->network_num >= RTW_NETWORK_LINK_MAX)
					break;

				pnetwork = alloc_network(pmlmepriv); /* will update scan_time */

				if (!pnetwork)
					goto _exit;

				if (lidx == 0) {
					pnetwork->network.is_complete_profile = _TRUE;
					_rtw_memcpy(&(pnetwork->network), target, target->Length);
				} else {
					tbtt_info = rtw_get_next_mld_tbtt_info(adapter, rnb_ele, &i);
					if (i >= rnb_ele->nb_ap_num || !tbtt_info)
						break;

					pnetwork->network.is_complete_profile = _FALSE;
					_rtw_memcpy(&(pnetwork->network), target, target->Length);
					update_mld_link_from_rnb_info(adapter, pnetwork, &rnb_ele->nb_aps[i], tbtt_info);
					i++;
					for (lnw_idx = 0; lnw_idx < ml_ele->profile_num; lnw_idx++) {
						if (pnetwork->network.link_id == ml_ele->profile[lnw_idx].link_id) {
							update_mld_link_from_ml_link_info(adapter, pnetwork, &ml_ele->profile[lnw_idx]);
							break;
						}
					}
				}

				pnetwork->bcn_keys_valid = 0;
				/* pnetwork is complete profile if lidx==0 or ml_ele->profile is found */
				if (pnetwork->network.is_complete_profile) {
					if (target->Reserved[0] == BSS_TYPE_BCN || target->Reserved[0] == BSS_TYPE_PROB_RSP)
						rtw_update_bcn_keys_of_network(pnetwork);
				}

				pmld_network->link_network[pmld_network->network_num] = pnetwork;
				pmld_network->network_num++;

				pnetwork->last_scanned = rtw_get_current_time();

				/* bss info not receving from the right channel */
				if (pnetwork->network.PhyInfo.SignalQuality == 101)
					pnetwork->network.PhyInfo.SignalQuality = 0;

				rtw_list_insert_tail(&(pnetwork->list), &(pmlmepriv->scanned_queue.queue));

				/* Link mld_network and network */
				pnetwork->network.mld_network = pmld_network;

				lidx++;

			} while (i < rnb_ele->nb_ap_num);
		}
	}
	else {
		#if defined(CONFIG_RTW_MESH) && CONFIG_RTW_MESH_ACNODE_PREVENT
		systime last_scanned;
		#endif
		struct beacon_keys bcn_keys;
		bool bcn_keys_valid;
		bool is_hidden_ssid_ap;
		struct wlan_network *link_network;
		WLAN_BSSID_EX *link_wlan = NULL;
		pmld_network = found_network->network.mld_network;
		link_network = (struct wlan_network *)rtw_zmalloc(sizeof(struct wlan_network));
		if (!link_network)
			goto _exit;

		/* complete when the primary network has been updated and there is no more per-sta profile */
		do {
			update_ie = _FALSE;
			_rtw_memset(link_network, 0, sizeof(struct wlan_network));
			_rtw_memcpy(&link_network->network, &found_network->network, get_WLAN_BSSID_EX_sz(&found_network->network));
			if (lidx == 0) {
				pnetwork = found_network;
				link_network->network.is_complete_profile = _TRUE;
			} else {
				tbtt_info = rtw_get_next_mld_tbtt_info(adapter, rnb_ele, &i);
				if (i >= rnb_ele->nb_ap_num || !tbtt_info)
					break;

				pnetwork = rtw_get_link_network_by_linkid(pmld_network, tbtt_info->mld_param.link_id);
				if (!pnetwork) {
					i++;
					lidx++;
					continue;
				}
				link_network->network.is_complete_profile = _FALSE;
				update_mld_link_from_rnb_info(adapter, link_network, &rnb_ele->nb_aps[i], tbtt_info);
				i++;
				for (lnw_idx = 0; lnw_idx < ml_ele->profile_num; lnw_idx++) {
					if (link_network->network.link_id == ml_ele->profile[lnw_idx].link_id) {
						update_mld_link_from_ml_link_info(adapter, link_network, &ml_ele->profile[lnw_idx]);
						break;
					}
				}
			}

			link_wlan = &link_network->network;

			#if defined(CONFIG_RTW_MESH) && CONFIG_RTW_MESH_ACNODE_PREVENT
			last_scanned = pnetwork->last_scanned;
			#endif

			bcn_keys_valid = 0;
			is_hidden_ssid_ap = 0;
			pnetwork->last_scanned = rtw_get_current_time();

			if (link_wlan->Reserved[0] == BSS_TYPE_BCN || link_wlan->Reserved[0] == BSS_TYPE_PROB_RSP) {
				if (link_wlan->InfrastructureMode == Ndis802_11Infrastructure) {
					is_hidden_ssid_ap = hidden_ssid_ap(link_wlan);
					if (!is_hidden_ssid_ap) /* update last time it's non hidden ssid AP */
						pnetwork->last_non_hidden_ssid_ap = rtw_get_current_time();
				}
				/* pnetwork is complete profile if lidx==0 or ml_ele->profile is found */
				if (link_wlan->is_complete_profile)
					bcn_keys_valid = rtw_get_bcn_keys_from_bss(link_wlan, &bcn_keys);
			}

			if (link_wlan->is_complete_profile) {
				if (link_wlan->InfrastructureMode == Ndis802_11_mesh
					|| link_wlan->Reserved[0] >= pnetwork->network.Reserved[0])
					update_ie = _TRUE;
				else if (link_wlan->InfrastructureMode == Ndis802_11Infrastructure && !pnetwork->fixed
					&& rtw_get_passing_time_ms(pnetwork->last_non_hidden_ssid_ap) > SCANQUEUE_LIFETIME)
					update_ie = _TRUE;
				else if (bcn_keys_valid) {
					if (is_hidden_ssid(bcn_keys.ssid, bcn_keys.ssid_len)) {
						/* hidden ssid, replace with current beacon ssid directly */
						_rtw_memcpy(bcn_keys.ssid, pnetwork->bcn_keys.ssid, pnetwork->bcn_keys.ssid_len);
						bcn_keys.ssid_len = pnetwork->bcn_keys.ssid_len;
					}
					if (rtw_bcn_key_compare(&pnetwork->bcn_keys, &bcn_keys) == _FALSE)
						update_ie = _TRUE;
				}
			}

			#if defined(CONFIG_RTW_MESH) && CONFIG_RTW_MESH_ACNODE_PREVENT
			if (!MLME_IS_MESH(adapter) || !MLME_IS_ASOC(adapter)
				|| pnetwork->network.Configuration.DSConfig != link_wlan->Configuration.DSConfig
				|| rtw_get_passing_time_ms(last_scanned) > adapter->mesh_cfg.peer_sel_policy.scanr_exp_ms
				|| !rtw_bss_is_same_mbss(&pnetwork->network, link_wlan)
			) {
				pnetwork->acnode_stime = 0;
				pnetwork->acnode_notify_etime = 0;
			}
			#endif

			if (bcn_keys_valid) {
				_rtw_memcpy(&pnetwork->bcn_keys, &bcn_keys, sizeof(bcn_keys));
				pnetwork->bcn_keys_valid = 1;
			} else if (update_ie)
				pnetwork->bcn_keys_valid = 0;

			rtw_update_network(&(pnetwork->network), link_wlan, adapter, update_ie);

			/* Link mld_network and network */
			pnetwork->network.mld_network = pmld_network;

			lidx++;

		} while (i < rnb_ele->nb_ap_num);

		if (link_network)
			rtw_mfree((unsigned char *)link_network, sizeof(struct wlan_network));
	}

	pmld_network->last_scanned = rtw_get_current_time();

	for (i = 0; i < pmld_network->network_num; i++) {
		#ifdef CONFIG_80211D
		rtw_update_scanned_network_cisr(adapter_to_rfctl(adapter), pmld_network->link_network[i]);
		#endif

		#if defined(CONFIG_RTW_MESH) && CONFIG_RTW_MESH_ACNODE_PREVENT
		if (MLME_IS_MESH(adapter) && MLME_IS_ASOC(adapter))
			rtw_mesh_update_scanned_acnode_status(adapter, pmld_network->link_network[i]);
		#endif
	}

_exit:
	if (ml_ele)
		rtw_mfree((unsigned char *)ml_ele, sizeof(struct rtw_phl_ml_element));
	if (rnb_ele)
		rtw_mfree((unsigned char *)rnb_ele, sizeof(struct rtw_phl_rnb_rpt_element));

	if (pmld_network)
		pnetwork = GET_LINK_NETWORK(pmld_network, RTW_RLINK_PRIMARY);
	else
		pnetwork = NULL;
	return pnetwork;
}
#endif

/*Caller must hold pmlmepriv->lock first.*/
static bool update_scanned_network(_adapter *adapter, WLAN_BSSID_EX *target)
{
	_list	*plist, *phead;
	u32	bssid_ex_sz;
	struct mlme_priv	*pmlmepriv = &(adapter->mlmepriv);
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(adapter->wdinfo);
#endif /* CONFIG_P2P */
	_queue	*queue	= &(pmlmepriv->scanned_queue);
	struct wlan_network	*pnetwork = NULL;
	struct wlan_network	*choice = NULL;
	int target_find = 0;
	bool update_ie = _FALSE;
	_queue *mld_queue = &(pmlmepriv->scanned_mld_queue);

	_rtw_spinlock_bh(&queue->lock);
	_rtw_spinlock_bh(&mld_queue->lock);
	phead = get_list_head(queue);
	plist = get_next(phead);

#if 0
	RTW_INFO("%s => ssid:%s , rssi:%ld , ss:%d\n",
		__func__, target->Ssid.Ssid, target->PhyInfo.rssi, target->PhyInfo.SignalStrength);
#endif

	while (1) {
		if (rtw_end_of_queue_search(phead, plist) == _TRUE)
			break;

		pnetwork = LIST_CONTAINOR(plist, struct wlan_network, list);

#ifdef CONFIG_P2P
		if (_rtw_memcmp(pnetwork->network.MacAddress, target->MacAddress, ETH_ALEN) &&
		    _rtw_memcmp(pnetwork->network.Ssid.Ssid, "DIRECT-", 7) &&
		    rtw_get_p2p_ie(pnetwork->network.IEs + _FIXED_IE_LENGTH_,
				   pnetwork->network.IELength - _FIXED_IE_LENGTH_,
				   NULL, NULL)) {
			target_find = 1;
			break;
		}
#endif

		if (is_same_network(&(pnetwork->network), target)) {
			target_find = 1;
			break;
		}

		if (rtw_roam_flags(adapter)) {
			/* TODO: don't  select netowrk in the same ess as choice if it's new enough*/
		}
		if (pnetwork->fixed) {
			plist = get_next(plist);
			continue;
		}

		/* ToDo CONFIG_RTW_MLD:
		** if the MLD network in the scanned_mld_queue has fewer links than that of target MLD,
		** may need to get a set of choice to free */
#ifdef CONFIG_RSSI_PRIORITY
		if ((choice == NULL) || (pnetwork->network.PhyInfo.SignalStrength < choice->network.PhyInfo.SignalStrength)) {
			#ifdef CONFIG_RTW_MESH
			if (!MLME_IS_MESH(adapter) || !MLME_IS_ASOC(adapter)
				|| !rtw_bss_is_same_mbss(&pmlmepriv->dev_cur_network.network, &pnetwork->network))
			#endif
			if (target->is_mld == pnetwork->network.is_mld)
				choice = pnetwork;
		}
#else
		if (choice == NULL || rtw_time_after(choice->last_scanned, pnetwork->last_scanned)) {
			#ifdef CONFIG_RTW_MESH
			if (!MLME_IS_MESH(adapter) || !MLME_IS_ASOC(adapter)
				|| !rtw_bss_is_same_mbss(&pmlmepriv->dev_cur_network.network, &pnetwork->network))
			#endif
			if (target->is_mld == pnetwork->network.is_mld)
				choice = pnetwork;
		}
#endif
		plist = get_next(plist);

	}


	/* If we didn't find a match, then get a new network slot to initialize
	 * with this beacon's information */
	/* if (rtw_end_of_queue_search(phead,plist)== _TRUE) { */
#ifdef CONFIG_80211BE_EHT
	if (target->is_mld) {
		pnetwork = update_scanned_mld_network(adapter, target, choice, pnetwork, target_find);
	} else /* !target->is_mld == legacy path*/
#endif /* CONFIG_80211BE_EHT */
	{
		if (!target_find) {
			if (_rtw_queue_empty(&(pmlmepriv->free_bss_pool)) == _TRUE) {
				/* If there are no more slots, expire the choice */
				/* list_del_init(&choice->list); */
				pnetwork = choice;
				if (pnetwork == NULL)
					goto unlock_scan_queue;

#ifdef CONFIG_RSSI_PRIORITY
				RTW_DBG("%s => ssid:%s ,bssid:"MAC_FMT"  will be deleted from scanned_queue (rssi:%d , ss:%d)\n",
				__func__, pnetwork->network.Ssid.Ssid, MAC_ARG(pnetwork->network.MacAddress),
				pnetwork->network.PhyInfo.rssi, pnetwork->network.PhyInfo.SignalStrength);
#else
				RTW_DBG("%s => ssid:%s ,bssid:"MAC_FMT" will be deleted from scanned_queue\n",
				__func__, pnetwork->network.Ssid.Ssid, MAC_ARG(pnetwork->network.MacAddress));
#endif

				_rtw_memcpy(&(pnetwork->network), target,  get_WLAN_BSSID_EX_sz(target));
				pnetwork->bcn_keys_valid = 0;
				if (target->Reserved[0] == BSS_TYPE_BCN || target->Reserved[0] == BSS_TYPE_PROB_RSP)
					rtw_update_bcn_keys_of_network(pnetwork);
				/* variable initialize */
				pnetwork->fixed = _FALSE;
				pnetwork->last_scanned = rtw_get_current_time();
				pnetwork->last_non_hidden_ssid_ap = pnetwork->last_scanned;
				#if defined(CONFIG_RTW_MESH) && CONFIG_RTW_MESH_ACNODE_PREVENT
				pnetwork->acnode_stime = 0;
				pnetwork->acnode_notify_etime = 0;
				#endif

				pnetwork->network_type = 0;
				pnetwork->aid = 0;
				pnetwork->join_res = 0;

				/* bss info not receving from the right channel */
				if (pnetwork->network.PhyInfo.SignalQuality == 101)
					pnetwork->network.PhyInfo.SignalQuality = 0;
			} else {
				/* Otherwise just pull from the free list */

				pnetwork = alloc_network(pmlmepriv); /* will update scan_time */
				if (pnetwork == NULL)
					goto unlock_scan_queue;

				bssid_ex_sz = get_WLAN_BSSID_EX_sz(target);
				target->Length = bssid_ex_sz;

				_rtw_memcpy(&(pnetwork->network), target, bssid_ex_sz);
				pnetwork->bcn_keys_valid = 0;
				if (target->Reserved[0] == BSS_TYPE_BCN || target->Reserved[0] == BSS_TYPE_PROB_RSP)
					rtw_update_bcn_keys_of_network(pnetwork);

				/* bss info not receving from the right channel */
				if (pnetwork->network.PhyInfo.SignalQuality == 101)
					pnetwork->network.PhyInfo.SignalQuality = 0;

				rtw_list_insert_tail(&(pnetwork->list), &(queue->queue));
			}
		}
		else {
			/* we have an entry and we are going to update it. But this entry may
			 * be already expired. In this case we do the same as we found a new
		 	* net and call the new_net handler
		 	*/
			#if defined(CONFIG_RTW_MESH) && CONFIG_RTW_MESH_ACNODE_PREVENT
			systime last_scanned = pnetwork->last_scanned;
			#endif
			struct beacon_keys bcn_keys;
			bool bcn_keys_valid = 0;
			bool is_hidden_ssid_ap = 0;

			pnetwork->last_scanned = rtw_get_current_time();

			if (target->Reserved[0] == BSS_TYPE_BCN || target->Reserved[0] == BSS_TYPE_PROB_RSP) {
				if (target->InfrastructureMode == Ndis802_11Infrastructure) {
					is_hidden_ssid_ap = hidden_ssid_ap(target);
					if (!is_hidden_ssid_ap) /* update last time it's non hidden ssid AP */
						pnetwork->last_non_hidden_ssid_ap = rtw_get_current_time();
				}
				bcn_keys_valid = rtw_get_bcn_keys_from_bss(target, &bcn_keys);
			}

			if (target->InfrastructureMode == Ndis802_11_mesh
				|| target->Reserved[0] >= pnetwork->network.Reserved[0])
				update_ie = _TRUE;
			else if (target->InfrastructureMode == Ndis802_11Infrastructure && !pnetwork->fixed
				&& rtw_get_passing_time_ms(pnetwork->last_non_hidden_ssid_ap) > SCANQUEUE_LIFETIME)
				update_ie = _TRUE;
			else if (bcn_keys_valid) {
				if (is_hidden_ssid(bcn_keys.ssid, bcn_keys.ssid_len)) {
					/* hidden ssid, replace with current beacon ssid directly */
					_rtw_memcpy(bcn_keys.ssid, pnetwork->bcn_keys.ssid, pnetwork->bcn_keys.ssid_len);
					bcn_keys.ssid_len = pnetwork->bcn_keys.ssid_len;
				}
				if (rtw_bcn_key_compare(&pnetwork->bcn_keys, &bcn_keys) == _FALSE)
					update_ie = _TRUE;
			}

			#if defined(CONFIG_RTW_MESH) && CONFIG_RTW_MESH_ACNODE_PREVENT
			if (!MLME_IS_MESH(adapter) || !MLME_IS_ASOC(adapter)
				|| pnetwork->network.Configuration.DSConfig != target->Configuration.DSConfig
				|| rtw_get_passing_time_ms(last_scanned) > adapter->mesh_cfg.peer_sel_policy.scanr_exp_ms
				|| !rtw_bss_is_same_mbss(&pnetwork->network, target)
			) {
				pnetwork->acnode_stime = 0;
				pnetwork->acnode_notify_etime = 0;
			}
			#endif

			if (bcn_keys_valid) {
				_rtw_memcpy(&pnetwork->bcn_keys, &bcn_keys, sizeof(bcn_keys));
				pnetwork->bcn_keys_valid = 1;
			} else if (update_ie)
				pnetwork->bcn_keys_valid = 0;

			rtw_update_network(&(pnetwork->network), target, adapter, update_ie);
		}

		#ifdef CONFIG_80211D
		rtw_update_scanned_network_cisr(adapter_to_rfctl(adapter), pnetwork);
		#endif

		#if defined(CONFIG_RTW_MESH) && CONFIG_RTW_MESH_ACNODE_PREVENT
		if (MLME_IS_MESH(adapter) && MLME_IS_ASOC(adapter))
			rtw_mesh_update_scanned_acnode_status(adapter, pnetwork);
		#endif
	}

unlock_scan_queue:
	_rtw_spinunlock_bh(&mld_queue->lock);
	_rtw_spinunlock_bh(&queue->lock);

#ifdef CONFIG_RTW_MESH
	if (pnetwork && MLME_IS_MESH(adapter)
		&& check_fwstate(pmlmepriv, WIFI_ASOC_STATE)
		&& !check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY)
	)
		rtw_chk_candidate_peer_notify(adapter, pnetwork);
#endif

	return update_ie;
}

static void add_network(_adapter *adapter, WLAN_BSSID_EX *pnetwork)
{
	bool update_ie;
	/* _queue *queue = &(pmlmepriv->scanned_queue); */
	/* _rtw_spinlock_bh(&queue->lock); */

#if defined(CONFIG_P2P) && defined(CONFIG_P2P_REMOVE_GROUP_INFO)
	if (adapter->registrypriv.wifi_spec == 0)
		rtw_bss_ex_del_p2p_attr(pnetwork, P2P_ATTR_GROUP_INFO);
#endif

#ifdef CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST
	if (adapter->registrypriv.ignore_go_in_scan) {
		if(rtw_chk_p2p_wildcard_ssid(pnetwork) == _SUCCESS ||
			rtw_chk_p2p_ie(pnetwork) == _SUCCESS)
			return;
	}
	/*100 was follow n & ac IC setting SignalStrength rang was 0~100*/
	if(adapter->registrypriv->ignore_low_rssi_in_scan != 0xff &&
		pnetwork->PhyInfo.rssi < (adapter->registrypriv->ignore_low_rssi_in_scan - 100))
		return;
#endif /*CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST*/

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_MIRACAST))
		rtw_bss_ex_del_wfd_ie(pnetwork);

	/* Wi-Fi driver will update the current network if the scan result of the connected AP be updated by scan. */
	update_ie = update_scanned_network(adapter, pnetwork);

	if (update_ie)
		update_current_network(adapter, pnetwork);

	/* _rtw_spinunlock_bh(&queue->lock); */

}

void dump_scanned_queue(void *sel, _adapter *adapter)
{
#ifdef CONFIG_80211D
#define SURVEY_INFO_TITLE_FMT_80211D " %-8s"
#define SURVEY_INFO_VALUE_FMT_80211D " %c   "ALPHA2_FMT" %u"
#define SURVEY_INFO_TITLE_ARG_80211D , "alpha2"
#define SURVEY_INFO_VALUE_ARG_80211D , rfctl->effected_cisr == &pnetwork->cisr ? '*' : ' ' \
	, show_cisr && is_alpha(pnetwork->cisr.alpha2[0]) ? pnetwork->cisr.alpha2[0] : '-' \
	, show_cisr && is_alpha(pnetwork->cisr.alpha2[1]) ? pnetwork->cisr.alpha2[1] : '-' \
	, show_cisr ? pnetwork->cisr.status : COUNTRY_IE_SLAVE_NOCOUNTRY
#else
#define SURVEY_INFO_TITLE_FMT_80211D ""
#define SURVEY_INFO_VALUE_FMT_80211D ""
#define SURVEY_INFO_TITLE_ARG_80211D
#define SURVEY_INFO_VALUE_ARG_80211D
#endif

#ifdef CONFIG_80211D
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	bool show_cisr = rfctl->cis_enabled && (rfctl->cis_flags & CISF_ENV_BSS);
#endif
	struct mlme_priv *mlme = &adapter->mlmepriv;
	_queue *queue = &mlme->scanned_queue;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(adapter);
	struct link_mlme_priv *lmlme = &(alink->mlmepriv);
	struct wlan_network *pnetwork = NULL;
	_list *plist, *phead;
	s32 notify_signal;
	s16 notify_noise = 0;
	u16 index = 0, ie_cap = 0;
	unsigned char *ie_wpa = NULL, *ie_wpa2 = NULL, *ie_wps = NULL;
	unsigned char *ie_wpa3 = NULL;
	unsigned char *ie_p2p = NULL;
	char *flag_str = NULL;
	int ielen = 0;
	u32 wpsielen = 0;
	u32 akm;
#ifdef CONFIG_RTW_MESH
	const char *ssid_title_str = "ssid/mesh_id";
#else
	const char *ssid_title_str = "ssid";
#endif

	flag_str = rtw_zmalloc(64);
	if (!flag_str)
		return;

	RTW_PRINT_SEL(sel, "%-5s %-17s %-4s %-3s %-4s %-4s %-5s"
		SURVEY_INFO_TITLE_FMT_80211D
		" %-5s %-32s %s\n"
		, "index", "bssid", "band", "ch", "RSSI", "SdBm", "Noise"
		SURVEY_INFO_TITLE_ARG_80211D
		, "age", "flag", ssid_title_str);

	_rtw_spinlock_bh(&queue->lock);

	phead = get_list_head(queue);
	plist = get_next(phead);

	while (!rtw_end_of_queue_search(phead, plist)) {
		pnetwork = LIST_CONTAINOR(plist, struct wlan_network, list);
		plist = get_next(plist);

		if (check_fwstate(mlme, WIFI_ASOC_STATE) == _TRUE &&
		    is_same_network(&lmlme->cur_network.network, &pnetwork->network)) {
			notify_signal = rtw_phl_rssi_to_dbm(adapter->recvinfo.signal_strength);/* dbm */
		} else {
			notify_signal = rtw_phl_rssi_to_dbm(pnetwork->network.PhyInfo.SignalStrength);/* dbm */
		}

#if 0 /*def CONFIG_BACKGROUND_NOISE_MONITOR*/
		if (IS_NM_ENABLE(adapter))
			notify_noise = rtw_noise_query_by_chan_num(adapter, pnetwork->network.Configuration.DSConfig);
#endif

		ie_wpa = rtw_get_wpa_ie(&pnetwork->network.IEs[12], &ielen, pnetwork->network.IELength - 12);
		ie_wpa2 = rtw_get_wpa2_ie(&pnetwork->network.IEs[12], &ielen, pnetwork->network.IELength - 12);
		ie_wpa3 = NULL;
		if (ie_wpa2 &&
		    rtw_parse_wpa2_ie(ie_wpa2, ielen+2, NULL, NULL, NULL, &akm,
				      NULL, NULL) == _SUCCESS) {
			if (akm & WLAN_AKM_TYPE_SAE) {
				ie_wpa3 = ie_wpa2;
				if (akm == WLAN_AKM_TYPE_SAE)
					ie_wpa2 = NULL;
			}
		}
		ie_cap = rtw_get_capability(&pnetwork->network);
		ie_wps = rtw_get_wps_ie(&pnetwork->network.IEs[12], pnetwork->network.IELength - 12, NULL, &wpsielen);
		ie_p2p = rtw_get_p2p_ie(&pnetwork->network.IEs[12], pnetwork->network.IELength - 12, NULL, &ielen);
		sprintf(flag_str, "%s%s%s%s%s%s%s%s%s",
			(ie_wpa) ? "[WPA]" : "",
			(ie_wpa2) ? "[WPA2]" : "",
			(ie_wpa3) ? "[WPA3]" : "",
			(!ie_wpa && !ie_wpa && ie_cap & BIT(4)) ? "[WEP]" : "",
			(ie_wps) ? "[WPS]" : "",
			(pnetwork->network.InfrastructureMode == Ndis802_11IBSS) ? "[IBSS]" :
				(pnetwork->network.InfrastructureMode == Ndis802_11_mesh) ? "[MESH]" : "",
			(ie_cap & BIT(0)) ? "[ESS]" : "",
			(pnetwork->network.Reserved[0] == BSS_TYPE_BCN) ? "[B]" : \
			(pnetwork->network.Reserved[0] == BSS_TYPE_PROB_RSP) ? "[P]" : "[U]",
			(ie_p2p) ? "[P2P]" : "");

		RTW_PRINT_SEL(sel, "%5d "MAC_FMT" %4s %3d %4d %4d %5d"
			SURVEY_INFO_VALUE_FMT_80211D
			" %5d %-32s %s\n"
			, ++index
			, MAC_ARG(pnetwork->network.MacAddress)
			, band_str(BSS_EX_OP_BAND(&pnetwork->network))
			, BSS_EX_OP_CH(&pnetwork->network)
			, pnetwork->network.PhyInfo.rssi
			, notify_signal
			, notify_noise
			SURVEY_INFO_VALUE_ARG_80211D
			, rtw_get_passing_time_ms(pnetwork->last_scanned)
			, flag_str
			, pnetwork->network.InfrastructureMode == Ndis802_11_mesh ? pnetwork->network.mesh_id.Ssid : pnetwork->network.Ssid.Ssid
		);
	}

	_rtw_spinunlock_bh(&queue->lock);

	rtw_mfree(flag_str, 64);
}

#ifdef CONFIG_STA_MULTIPLE_BSSID
static inline void rtw_gen_new_bssid(const u8 *bssid, u8 max_bssid_ind,
					  u8 mbssid_index, u8 *new_bssid)
{
	u8 i = 0;
	u8 max_num = 1;
	u8 B;
	u8 new_a5;

	for (i = 0; i < max_bssid_ind; i++)
		max_num = max_num * 2;
	/*RTW_INFO("%s, max_num=%d\n", __func__, max_num);*/
	/*RTW_INFO("%s, %02x,%02x,%02x,%02x,%02x,%02x \n", __func__, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);*/

	B = bssid[5] % max_num;

	new_a5 = bssid[5] - B + ((B + mbssid_index) % max_num);

	new_bssid[0] = bssid[0];
	new_bssid[1] = bssid[1];
	new_bssid[2] = bssid[2];
	new_bssid[3] = bssid[3];
	new_bssid[4] = bssid[4];
	new_bssid[5] = new_a5;

	/*RTW_INFO("%s, %02x,%02x,%02x,%02x,%02x,%02x \n", __func__, new_bssid[0], new_bssid[1], new_bssid[2], new_bssid[3], new_bssid[4], new_bssid[5]);*/
}

void add_mbssid_network(_adapter *padapter, WLAN_BSSID_EX *ref_bss)
{
	WLAN_BSSID_EX *pbss;
	u32 sub_ies_len;
	u8 *mbssid_ie_ptr = NULL;
	PNDIS_802_11_VARIABLE_IEs pIE, sub_pie;
	u8 max_bssid_indicator;
	int i,j;
	u8* mbssid_ie;
	sint mbssid_len;
	u8 mbssid_index;
	u8 copy_ie_offset;
	u32 copy_ie_len = 0;

	mbssid_ie = rtw_get_ie(ref_bss->IEs + _BEACON_IE_OFFSET_
		                              , WLAN_EID_MULTIPLE_BSSID
		                              , &mbssid_len
		                              , (ref_bss->IELength- _BEACON_IE_OFFSET_));
	if (!mbssid_ie)
		return;
#if 0
	else
		RTW_PRINT_DUMP("mbssid_ie: ", (const u8 *)mbssid_ie, mbssid_len);
#endif

	mbssid_ie_ptr = mbssid_ie;
	max_bssid_indicator = GET_MBSSID_MAX_BSSID_INDOCATOR(mbssid_ie_ptr);
	/*RTW_INFO("%s, max_bssid_indicator=%d\n", __func__, max_bssid_indicator);*/
	mbssid_ie_ptr = mbssid_ie_ptr + MBSSID_MAX_BSSID_INDICATOR_OFFSET;

	for (i = 0; i + 1 < mbssid_len;) {
		pIE = (PNDIS_802_11_VARIABLE_IEs)(mbssid_ie_ptr + i);

		switch (pIE->ElementID) {
		case MBSSID_NONTRANSMITTED_BSSID_PROFILE_ID:
			sub_ies_len = pIE->Length;
			pbss = (WLAN_BSSID_EX *)rtw_zmalloc(sizeof(WLAN_BSSID_EX));
			if (pbss) {
				_rtw_memcpy(pbss, ref_bss, sizeof(WLAN_BSSID_EX));
				_rtw_memset(pbss->IEs, 0, MAX_IE_SZ);
				copy_ie_len =  _TIMESTAMP_ + _BEACON_ITERVAL_;
				_rtw_memcpy(pbss->IEs, ref_bss->IEs, copy_ie_len);
			} else {
				return;
			}

			for (j = 0; j + 1 < sub_ies_len;) {
				sub_pie = (PNDIS_802_11_VARIABLE_IEs)(pIE->data + j);
				switch (sub_pie->ElementID) {
				case WLAN_EID_NON_TX_BSSID_CAP:
					/*RTW_INFO("%s, sub_pie->Length=%d\n", __func__, sub_pie->Length);*/
					/*RTW_PRINT_DUMP("WLAN_EID_NON_TX_BSSID_CAP: ", (const u8 *)sub_pie->data, sub_pie->Length);*/
					copy_ie_offset =  _TIMESTAMP_ + _BEACON_ITERVAL_;
					_rtw_memcpy(pbss->IEs + copy_ie_offset, sub_pie->data, sub_pie->Length);
					break;
				case WLAN_EID_SSID:
					/*RTW_PRINT_DUMP("WLAN_EID_SSID: ", (const u8 *)sub_pie->data, sub_pie->Length);*/
					/*RTW_INFO("%s, ref_bss->IELength=%d\n", __func__, ref_bss->IELength);*/
					/*RTW_PRINT_DUMP("A ref_bss->IEs: ", (const u8 *)ref_bss->IEs, ref_bss->IELength);*/
					copy_ie_offset =  _TIMESTAMP_ + _BEACON_ITERVAL_ + _CAPABILITY_;
					copy_ie_len =  WLAN_IE_ID_LEN + WLAN_IE_LEN_LEN;
					_rtw_memcpy(pbss->IEs + copy_ie_offset, sub_pie, copy_ie_len);

					copy_ie_offset = copy_ie_offset + WLAN_IE_ID_LEN + WLAN_IE_LEN_LEN;
					_rtw_memcpy(pbss->IEs + copy_ie_offset, sub_pie->data, sub_pie->Length);
					_rtw_memcpy(pbss->IEs + copy_ie_offset + sub_pie->Length
						                , ref_bss->IEs + copy_ie_offset + ref_bss->Ssid.SsidLength
						                , ref_bss->IELength - (copy_ie_offset + ref_bss->Ssid.SsidLength));

					pbss->IELength = ref_bss->IELength + (sub_pie->Length - ref_bss->Ssid.SsidLength);
					/*RTW_INFO("%s, ref_bss->Ssid.SsidLength=%d\n", __func__, ref_bss->Ssid.SsidLength);*/
					/*RTW_INFO("%s, sub_pie->Length=%d\n", __func__, sub_pie->Length);*/
					/*RTW_INFO("%s, pbss->IELength=%d\n", __func__, pbss->IELength);*/
					/*RTW_PRINT_DUMP("B pbss->IEs: ", (const u8 *)pbss->IEs, pbss->IELength);*/

					_rtw_memset(pbss->Ssid.Ssid, 0, pbss->Ssid.SsidLength);
					_rtw_memcpy(pbss->Ssid.Ssid, sub_pie->data, sub_pie->Length);
					pbss->Ssid.SsidLength = sub_pie->Length;
					break;
				case WLAN_EID_MULTI_BSSID_IDX:
					/*RTW_INFO("%s, sub_pie->Length=%d\n", __func__, sub_pie->Length);*/
					/*RTW_PRINT_DUMP("WLAN_EID_MULTI_BSSID_IDX: ", (const u8 *)sub_pie->data, sub_pie->Length);*/
					_rtw_memcpy(&mbssid_index, sub_pie->data, sub_pie->Length);
					/*RTW_INFO("%s,mbssid_index=%d\n", __func__, mbssid_index);*/
					rtw_gen_new_bssid(ref_bss->MacAddress, max_bssid_indicator
						                          , mbssid_index, pbss->MacAddress);
					pbss->mbssid_index = mbssid_index;
					break;
				default:
					break;
				}

				j += (sub_pie->Length + WLAN_IE_ID_LEN + WLAN_IE_LEN_LEN);
				/*RTW_INFO("%s, j=%d\n", __func__, j);*/
			}
			pbss->is_mbssid = _TRUE;
			add_network(padapter, pbss);
			rtw_mfree((u8 *)pbss, sizeof(WLAN_BSSID_EX));
			break;
		case MBSSID_VENDOR_SPECIFIC_ID:
			break;
		default:
			break;
		}

		i += (pIE->Length + WLAN_IE_ID_LEN + WLAN_IE_LEN_LEN);
		/*RTW_INFO("%s, i=%d\n", __func__, i);*/
	}
}
#endif

void rtw_survey_event_callback(_adapter	*adapter, u8 *pbuf)
{
	u32 len;
	u8 val8;
	WLAN_BSSID_EX *pnetwork;
	struct	mlme_priv	*pmlmepriv = &(adapter->mlmepriv);

	pnetwork = (WLAN_BSSID_EX *)pbuf;

	len = get_WLAN_BSSID_EX_sz(pnetwork);
	if (len > (sizeof(WLAN_BSSID_EX))) {
		return;
	}

#ifdef CONFIG_RTW_80211K
    	val8 = 0;
	rtw_hal_get_hwreg(adapter, HW_VAR_FREECNT, &val8);

	/* use TSF if no free run counter */
	if (val8==0)
		pnetwork->PhyInfo.free_cnt = (u32)rtw_hal_get_tsftr_by_port(
			adapter, rtw_hal_get_port(adapter));
#endif

	if (pnetwork->InfrastructureMode == Ndis802_11Infrastructure) {
		if (MLME_IS_SCAN(adapter)) {
			adapter->mlmeextpriv.sitesurvey_res.activate_ch_cnt
				+= rtw_process_beacon_hint(adapter_to_rfctl(adapter), pnetwork);
		}
	}

	_rtw_spinlock_bh(&pmlmepriv->lock);

	/* update IBSS_network 's timestamp */
	if ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE)) == _TRUE) {
		if (_rtw_memcmp(&(pmlmepriv->dev_cur_network.network.MacAddress), pnetwork->MacAddress, ETH_ALEN)) {
			struct wlan_network *ibss_wlan = NULL;

			_rtw_memcpy(pmlmepriv->dev_cur_network.network.IEs, pnetwork->IEs, 8);
			_rtw_spinlock_bh(&(pmlmepriv->scanned_queue.lock));
			ibss_wlan = _rtw_find_network(&pmlmepriv->scanned_queue,  pnetwork->MacAddress);
			if (ibss_wlan) {
				_rtw_memcpy(ibss_wlan->network.IEs , pnetwork->IEs, 8);
				_rtw_spinunlock_bh(&(pmlmepriv->scanned_queue.lock));
				goto exit;
			}
			_rtw_spinunlock_bh(&(pmlmepriv->scanned_queue.lock));
		}
	}

	/* lock pmlmepriv->lock when you accessing network_q */
	if ((check_fwstate(pmlmepriv, WIFI_UNDER_LINKING)) == _FALSE) {
		if (pnetwork->Ssid.Ssid[0] == 0)
			pnetwork->Ssid.SsidLength = 0;
		add_network(adapter, pnetwork);
#ifdef CONFIG_STA_MULTIPLE_BSSID
		add_mbssid_network(adapter, pnetwork);
#endif
	}

exit:
	_rtw_spinunlock_bh(&pmlmepriv->lock);


	return;
}

void rtw_surveydone_event_callback(_adapter *adapter, u8 *pbuf)
{
	struct surveydone_event *parm = (struct surveydone_event *)pbuf;
	struct	mlme_priv	*pmlmepriv = &adapter->mlmepriv;
	struct mlme_ext_priv	*pmlmeext = &adapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &pmlmeext->mlmext_info;

	_rtw_spinlock_bh(&pmlmepriv->lock);
	if (pmlmepriv->wps_probe_req_ie) {
		u32 free_len = pmlmepriv->wps_probe_req_ie_len;
		pmlmepriv->wps_probe_req_ie_len = 0;
		rtw_mfree(pmlmepriv->wps_probe_req_ie, free_len);
		pmlmepriv->wps_probe_req_ie = NULL;
	}

	if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY) == _FALSE) {
		RTW_INFO(FUNC_ADPT_FMT" fw_state:0x%x\n", FUNC_ADPT_ARG(adapter), get_fwstate(pmlmepriv));
		/* rtw_warn_on(1); */
	}

	_clr_fwstate_(pmlmepriv, WIFI_UNDER_SURVEY);
	_rtw_spinunlock_bh(&pmlmepriv->lock);

	_rtw_spinlock_bh(&pmlmepriv->lock);
	#ifdef CONFIG_SIGNAL_STAT_PROCESS
	rtw_set_signal_stat_timer(&adapter->recvinfo);
	#endif
	if (pmlmepriv->to_join == _TRUE) {
		if ((check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE)) {
			if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _FALSE) {
				set_fwstate(pmlmepriv, WIFI_UNDER_LINKING);

				if (rtw_select_and_join_from_scanned_queue(pmlmepriv) == _SUCCESS) {
					/*_set_timer(&pmlmepriv->assoc_timer, MAX_JOIN_TIMEOUT);*/
					set_assoc_timer(pmlmepriv, MAX_JOIN_TIMEOUT);
				}
				else {
					WLAN_BSSID_EX    *pdev_network = &(adapter->registrypriv.dev_network);
					u8 *pibss = adapter->registrypriv.dev_network.MacAddress;

					/* pmlmepriv->fw_state ^= WIFI_UNDER_SURVEY; */ /* because don't set assoc_timer */
					_clr_fwstate_(pmlmepriv, WIFI_UNDER_SURVEY);


					_rtw_memset(&pdev_network->Ssid, 0, sizeof(NDIS_802_11_SSID));
					_rtw_memcpy(&pdev_network->Ssid, &pmlmepriv->assoc_ssid, sizeof(NDIS_802_11_SSID));

					rtw_update_registrypriv_dev_network(adapter);
					rtw_generate_random_ibss(pibss);

					/*pmlmepriv->fw_state = WIFI_ADHOC_MASTER_STATE;*/
					init_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE);

					if (rtw_create_ibss_cmd(adapter, 0) != _SUCCESS)
						RTW_ERR("rtw_create_ibss_cmd FAIL\n");

					rtw_clear_to_join_status(adapter);
				}
			}
		} else {
			int s_ret;
			set_fwstate(pmlmepriv, WIFI_UNDER_LINKING);
			rtw_clear_to_join_status(adapter);
			s_ret = rtw_select_and_join_from_scanned_queue(pmlmepriv);
			if (_SUCCESS == s_ret) {
				/*_set_timer(&pmlmepriv->assoc_timer, MAX_JOIN_TIMEOUT);*/
				set_assoc_timer(pmlmepriv, MAX_JOIN_TIMEOUT);
			} else if (s_ret == 2) { /* there is no need to wait for join */
				_clr_fwstate_(pmlmepriv, WIFI_UNDER_LINKING);
				rtw_indicate_connect(adapter);
			} else {
				RTW_INFO("try_to_join, but select scanning queue fail, to_roam:%d\n", rtw_to_roam(adapter));

				if (rtw_to_roam(adapter) != 0) {
					struct sitesurvey_parm *scan_parm = NULL;
					u8 ssc_chk = rtw_sitesurvey_condition_check(adapter, _FALSE);

					scan_parm = rtw_malloc(sizeof(*scan_parm));
					if (scan_parm) {
						rtw_init_sitesurvey_parm(adapter, scan_parm);
						_rtw_memcpy(&scan_parm->ssid[0], &pmlmepriv->assoc_ssid, sizeof(NDIS_802_11_SSID));
						scan_parm->ssid_num = 1;
					}

					if (scan_parm == NULL || rtw_dec_to_roam(adapter) == 0
						|| (ssc_chk != SS_ALLOW && ssc_chk != SS_DENY_BUSY_TRAFFIC)
						|| _SUCCESS != rtw_sitesurvey_cmd(adapter, scan_parm)
					   ) {
						rtw_set_to_roam(adapter, 0);
						if (MLME_IS_ASOC(adapter) == _TRUE)
							rtw_free_assoc_resources(adapter, _TRUE);
						rtw_indicate_disconnect(adapter, 0, _FALSE);
					} else
						pmlmepriv->to_join = _TRUE;

					if (scan_parm)
						rtw_mfree(scan_parm, sizeof(*scan_parm));
				} else
					rtw_indicate_disconnect(adapter, 0, _FALSE);
				_clr_fwstate_(pmlmepriv, WIFI_UNDER_LINKING);
			}
		}
	} else {
		if (rtw_chk_roam_flags(adapter, RTW_ROAM_ACTIVE)
                #if (defined(CONFIG_RTW_WNM) && defined(CONFIG_RTW_80211R))
                        || rtw_wnm_btm_roam_triggered(adapter)
                #endif
		) {
			if (MLME_IS_STA(adapter)
			    && check_fwstate(pmlmepriv, WIFI_ASOC_STATE)) {
				if (rtw_select_roaming_candidate(pmlmepriv) == _SUCCESS) {
#ifdef CONFIG_RTW_80211R
					rtw_ft_start_roam(adapter,
						(u8 *)pmlmepriv->roam_network->network.MacAddress);
#else
					receive_disconnect(adapter, pmlmepriv->dev_cur_network.network.MacAddress
						, WLAN_REASON_ACTIVE_ROAM, _FALSE);
					pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
					pmlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_ROAMING;
					pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
#endif
				}
			}
		}
	}

	RTW_INFO("scan complete in %dms\n",rtw_get_passing_time_ms(pmlmepriv->scan_start_time));

	_rtw_spinunlock_bh(&pmlmepriv->lock);

#ifdef CONFIG_P2P_PS
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		p2p_ps_wk_cmd(adapter, P2P_PS_SCAN_DONE, 0);
#endif /* CONFIG_P2P_PS */

	rtw_mi_os_xmit_schedule(adapter);
#ifdef CONFIG_DRVEXT_MODULE_WSC
	drvext_surveydone_callback(&adapter->drvextpriv);
#endif

#ifdef DBG_CONFIG_ERROR_DETECT
	{
		struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
		if (pmlmeext->sitesurvey_res.bss_cnt == 0) {
			/* rtw_hal_sreset_reset(adapter); */
		}
	}
#endif

#ifdef CONFIG_IOCTL_CFG80211
	rtw_cfg80211_surveydone_event_callback(adapter);
#endif /* CONFIG_IOCTL_CFG80211 */

	rtw_indicate_scan_done(adapter, pmlmeext->scan_abort);

#if defined(CONFIG_CONCURRENT_MODE) && defined(CONFIG_IOCTL_CFG80211)
	rtw_cfg80211_indicate_scan_done_for_buddy(adapter, _FALSE);
#endif

	if (parm->activate_ch_cnt)
		rtw_beacon_hint_ch_change_notifier(adapter_to_rfctl(adapter));

#ifdef CONFIG_80211D
	if (adapter_to_rfctl(adapter)->cis_enabled
		&& (adapter_to_rfctl(adapter)->cis_flags & CISF_ENV_BSS))
		rtw_apply_scan_network_country_ie_cmd(adapter, RTW_CMDF_DIRECTLY);
#endif

#ifdef CONFIG_RTW_MESH
	#if CONFIG_RTW_MESH_OFFCH_CAND
	if (rtw_mesh_offch_candidate_accepted(adapter)) {
		u8 ch;

		ch = rtw_mesh_select_operating_ch(adapter);
		if (ch && pmlmepriv->dev_cur_network.network.Configuration.DSConfig != ch) {
			u8 ifbmp = rtw_mi_get_ap_mesh_ifbmp(adapter);
			s8 band = REQ_BAND_NONE; /* TODO */

			if (ifbmp) {
				/* switch to selected channel */
				rtw_change_bss_bchbw_cmd(adapter, RTW_CMDF_DIRECTLY, ifbmp, 0, band, ch, REQ_BW_ORI, REQ_OFFSET_NONE);
				issue_probereq_ex(adapter, &pmlmepriv->dev_cur_network.network.mesh_id, NULL, 0, 0, 0, 0);
			} else
				rtw_warn_on(1);
		}
	}
	#endif
#endif /* CONFIG_RTW_MESH */

#ifdef CONFIG_RTW_ACS
	if (parm->acs) {
		u8 ifbmp = rtw_mi_get_ap_mesh_ifbmp(adapter);

		if (ifbmp)
			rtw_change_bss_bchbw_cmd(adapter, RTW_CMDF_DIRECTLY, ifbmp, 0, REQ_BAND_NONE, REQ_CH_INT_INFO, REQ_BW_ORI, REQ_OFFSET_NONE);
	}
#endif
}

u8 _rtw_sitesurvey_condition_check(const char *caller, _adapter *adapter, bool check_sc_interval)
{
	u8 ss_condition = SS_ALLOW;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct registry_priv *registry_par = &adapter->registrypriv;


#ifdef CONFIG_MP_INCLUDED
	if (rtw_mp_mode_check(adapter)) {
		RTW_INFO("%s ("ADPT_FMT") MP mode block Scan request\n", caller, ADPT_ARG(adapter));
		ss_condition = SS_DENY_MP_MODE;
		goto _exit;
	}
#endif

#ifdef DBG_LA_MODE
	if(registry_par->la_mode_en == 1 && MLME_IS_ASOC(adapter)) {
		RTW_INFO("%s ("ADPT_FMT") LA debug mode block Scan request\n", caller, ADPT_ARG(adapter));
		ss_condition = SS_DENY_LA_MODE;
		goto _exit;
	}
#endif

#ifdef CONFIG_IOCTL_CFG80211
	if (adapter_wdev_data(adapter)->block_scan == _TRUE) {
		RTW_INFO("%s ("ADPT_FMT") wdev_priv.block_scan is set\n", caller, ADPT_ARG(adapter));
		ss_condition = SS_DENY_BLOCK_SCAN;
		goto _exit;
	}
#endif

	if (adapter_to_dvobj(adapter)->scan_deny == _TRUE) {
		RTW_INFO("%s ("ADPT_FMT") tpt mode, scan deny!\n", caller, ADPT_ARG(adapter));
		ss_condition = SS_DENY_BLOCK_SCAN;
		goto _exit;
	}

	if (rtw_is_scan_deny(adapter)) {
		RTW_INFO("%s ("ADPT_FMT") : scan deny\n", caller, ADPT_ARG(adapter));
		ss_condition = SS_DENY_BY_DRV;
		goto _exit;
	}

	#if 0 /*GEORGIA_TODO_FIXIT*/
	if (adapter_to_rfctl(adapter)->adaptivity_en
	    && rtw_hal_get_phy_edcca_flag(adapter)
	    && rtw_is_2g_ch(GET_PHL_COM(adapter_to_dvobj(adapter))->current_channel)) {
		RTW_WARN(FUNC_ADPT_FMT": Adaptivity block scan! (ch=%u)\n",
			 FUNC_ADPT_ARG(adapter),
			 GET_PHL_COM(adapter_to_dvobj(adapter))->current_channel);
		ss_condition = SS_DENY_ADAPTIVITY;
		goto _exit;
	}
	#endif
	if (check_fwstate(pmlmepriv, WIFI_AP_STATE)){
		if(check_fwstate(pmlmepriv, WIFI_UNDER_WPS)) {
			RTW_INFO("%s ("ADPT_FMT") : scan abort!! AP mode process WPS\n", caller, ADPT_ARG(adapter));
			ss_condition = SS_DENY_SELF_AP_UNDER_WPS;
			goto _exit;
		} else if (check_fwstate(pmlmepriv, WIFI_UNDER_LINKING) == _TRUE) {
			RTW_INFO("%s ("ADPT_FMT") : scan abort!!AP mode under linking (fwstate=0x%x)\n",
				caller, ADPT_ARG(adapter), pmlmepriv->fw_state);
			ss_condition = SS_DENY_SELF_AP_UNDER_LINKING;
			goto _exit;
		} else if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY) == _TRUE) {
			RTW_INFO("%s ("ADPT_FMT") : scan abort!!AP mode under survey (fwstate=0x%x)\n",
				caller, ADPT_ARG(adapter), pmlmepriv->fw_state);
			ss_condition = SS_DENY_SELF_AP_UNDER_SURVEY;
			goto _exit;
		}
	} else {
		if (check_fwstate(pmlmepriv, WIFI_UNDER_LINKING) == _TRUE) {
			RTW_INFO("%s ("ADPT_FMT") : scan abort!!STA mode under linking (fwstate=0x%x)\n",
				caller, ADPT_ARG(adapter), pmlmepriv->fw_state);
			ss_condition = SS_DENY_SELF_STA_UNDER_LINKING;
			goto _exit;
		} else if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY) == _TRUE) {
			RTW_INFO("%s ("ADPT_FMT") : scan abort!!STA mode under survey (fwstate=0x%x)\n",
				caller, ADPT_ARG(adapter), pmlmepriv->fw_state);
			ss_condition = SS_DENY_SELF_STA_UNDER_SURVEY;
			goto _exit;
		}
	}

#ifdef CONFIG_CONCURRENT_MODE
#ifdef PRIVATE_R
	if (rtw_mi_buddy_check_fwstate(adapter, WIFI_UNDER_LINKING)) {
#else
	if (rtw_mi_buddy_check_fwstate(adapter, WIFI_UNDER_LINKING | WIFI_UNDER_WPS)) {
#endif /* PRIVATE_R */
		RTW_INFO("%s ("ADPT_FMT") : scan abort!! buddy_intf under linking or wps\n", caller, ADPT_ARG(adapter));
		ss_condition = SS_DENY_BUDDY_UNDER_LINK_WPS;
		goto _exit;

	} else if (rtw_mi_buddy_check_fwstate(adapter, WIFI_UNDER_SURVEY)) {
		RTW_INFO("%s ("ADPT_FMT") : scan abort!! buddy_intf under survey\n", caller, ADPT_ARG(adapter));
		ss_condition = SS_DENY_BUDDY_UNDER_SURVEY;
		goto _exit;
	}
#endif /* CONFIG_CONCURRENT_MODE */

#ifdef RTW_BUSY_DENY_SCAN
	/*
	 * busy traffic check
	 * Rules:
	 * 1. If (scan interval <= BUSY_TRAFFIC_SCAN_DENY_PERIOD) always allow
	 *    scan, otherwise goto rule 2.
	 * 2. Deny scan if any interface is busy, otherwise allow scan.
	 */
	if (pmlmepriv->lastscantime
	    && (rtw_get_passing_time_ms(pmlmepriv->lastscantime) >
		registry_par->scan_interval_thr)
	    && rtw_mi_busy_traffic_check(adapter)) {
		RTW_WARN("%s ("ADPT_FMT") : scan abort!! BusyTraffic\n",
			 caller, ADPT_ARG(adapter));
		ss_condition = SS_DENY_BUSY_TRAFFIC;
		goto _exit;
	}
#endif /* RTW_BUSY_DENY_SCAN */

_exit:
	return ss_condition;
}


/*rtw_mlme_ext.c*/
void sitesurvey_set_offch_state(_adapter *adapter, u8 scan_state)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	_rtw_mutex_lock_interruptible(&rfctl->offch_mutex);

	switch (scan_state) {
	case SCAN_DISABLE:
	case SCAN_BACK_OP:
		rfctl->offch_state = OFFCHS_NONE;
		break;
	case SCAN_START:
	case SCAN_LEAVING_OP:
		rfctl->offch_state = OFFCHS_LEAVING_OP;
		break;
	case SCAN_ENTER:
	case SCAN_LEAVE_OP:
		rfctl->offch_state = OFFCHS_LEAVE_OP;
		break;
	case SCAN_COMPLETE:
	case SCAN_BACKING_OP:
		rfctl->offch_state = OFFCHS_BACKING_OP;
		break;
	default:
		break;
	}

	_rtw_mutex_unlock(&rfctl->offch_mutex);
}
static u8 rtw_scan_sparse(_adapter *adapter, struct rtw_ieee80211_channel *ch, u8 ch_num)
{
	/* interval larger than this is treated as backgroud scan */
#ifndef RTW_SCAN_SPARSE_BG_INTERVAL_MS
#define RTW_SCAN_SPARSE_BG_INTERVAL_MS 12000
#endif

#ifndef RTW_SCAN_SPARSE_CH_NUM_MIRACAST
#define RTW_SCAN_SPARSE_CH_NUM_MIRACAST 1
#endif
#ifndef RTW_SCAN_SPARSE_CH_NUM_BG
#define RTW_SCAN_SPARSE_CH_NUM_BG 4
#endif

#define SCAN_SPARSE_CH_NUM_INVALID 255

	static u8 token = 255;
	u32 interval;
	bool busy_traffic = _FALSE;
	bool miracast_enabled = _FALSE;
	bool bg_scan = _FALSE;
	u8 max_allow_ch = SCAN_SPARSE_CH_NUM_INVALID;
	u8 scan_division_num;
	u8 ret_num = ch_num;
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;

	if (mlmeext->last_scan_time == 0)
		mlmeext->last_scan_time = rtw_get_current_time();

	interval = rtw_get_passing_time_ms(mlmeext->last_scan_time);


	if (rtw_mi_busy_traffic_check(adapter))
		busy_traffic = _TRUE;

	if (rtw_mi_check_miracast_enabled(adapter))
		miracast_enabled = _TRUE;

	if (interval > RTW_SCAN_SPARSE_BG_INTERVAL_MS)
		bg_scan = _TRUE;

	/* max_allow_ch by conditions*/

#if RTW_SCAN_SPARSE_MIRACAST
	if (miracast_enabled == _TRUE && busy_traffic == _TRUE)
		max_allow_ch = rtw_min(max_allow_ch, RTW_SCAN_SPARSE_CH_NUM_MIRACAST);
#endif

#if RTW_SCAN_SPARSE_BG
	if (bg_scan == _TRUE)
		max_allow_ch = rtw_min(max_allow_ch, RTW_SCAN_SPARSE_CH_NUM_BG);
#endif


	if (max_allow_ch != SCAN_SPARSE_CH_NUM_INVALID) {
		int i;
		int k = 0;

		scan_division_num = (ch_num / max_allow_ch) + ((ch_num % max_allow_ch) ? 1 : 0);
		token = (token + 1) % scan_division_num;

		if (0)
			RTW_INFO("scan_division_num:%u, token:%u\n", scan_division_num, token);

		for (i = 0; i < ch_num; i++) {
			if (ch[i].hw_value && (i % scan_division_num) == token
			   ) {
				if (i != k)
					_rtw_memcpy(&ch[k], &ch[i], sizeof(struct rtw_ieee80211_channel));
				k++;
			}
		}

		_rtw_memset(&ch[k], 0, sizeof(struct rtw_ieee80211_channel));

		ret_num = k;
		mlmeext->last_scan_time = rtw_get_current_time();
	}

	return ret_num;
}

static int rtw_scan_ch_decision(_adapter *padapter, struct rtw_ieee80211_channel *out,
		u32 out_num, struct rtw_ieee80211_channel *in, u32 in_num, bool no_sparse)
{
	int i, j;
	int set_idx;
	u8 chan;
	struct rtw_chset *chset = adapter_to_chset(padapter);
	struct registry_priv *regsty = dvobj_to_regsty(adapter_to_dvobj(padapter));

	/* clear first */
	_rtw_memset(out, 0, sizeof(struct rtw_ieee80211_channel) * out_num);

	/* acquire channels from in */
	j = 0;
	for (i = 0; i < in_num; i++) {

		if (0)
			RTW_INFO(FUNC_ADPT_FMT" "CHAN_FMT"\n", FUNC_ADPT_ARG(padapter), CHAN_ARG(&in[i]));

		if (!in[i].hw_value || (in[i].flags & RTW_IEEE80211_CHAN_DISABLED))
			continue;
		if (rtw_mlme_band_check(padapter, in[i].hw_value) == _FALSE)
			continue;

		set_idx = rtw_chset_search_bch(chset, in[i].band, in[i].hw_value);

		if (set_idx >= 0) {
			if (j >= out_num) {
				RTW_PRINT(FUNC_ADPT_FMT" out_num:%u not enough\n",
					  FUNC_ADPT_ARG(padapter), out_num);
				break;
			}

			_rtw_memcpy(&out[j], &in[i], sizeof(struct rtw_ieee80211_channel));

			if (chset->chs[set_idx].flags & (RTW_CHF_NO_IR | RTW_CHF_DFS))
				out[j].flags |= RTW_IEEE80211_CHAN_PASSIVE_SCAN;

			j++;
		}
		if (j >= out_num)
			break;
	}

	/* if out is empty, use chset as default */
	if (j == 0) {
		for (i = 0; i < chset->chs_len; i++) {
			if (chset->chs[i].flags & RTW_CHF_DIS)
				continue;
			chan = chset->chs[i].ChannelNum;
			if (rtw_mlme_band_check(padapter, chan) == _TRUE) {
				if (rtw_mlme_ignore_chan(padapter, chan) == _TRUE)
					continue;

				if (j >= out_num) {
					RTW_PRINT(FUNC_ADPT_FMT" out_num:%u not enough\n",
						FUNC_ADPT_ARG(padapter), out_num);
					break;
				}

				out[j].hw_value = chan;
				out[j].band = chset->chs[i].band;
				if (0)
					RTW_INFO(FUNC_ADPT_FMT"band:%d ch:%u\n",
						FUNC_ADPT_ARG(padapter), out[j].band, chan);

				if (chset->chs[i].flags & (RTW_CHF_NO_IR | RTW_CHF_DFS))
					out[j].flags |= RTW_IEEE80211_CHAN_PASSIVE_SCAN;

				j++;
			}
		}
	}

	if (!no_sparse
		&& !regsty->wifi_spec
		&& j > 6 /* assume ch_num > 6 is normal scan */
	) {
		/* scan_sparse */
		j = rtw_scan_sparse(padapter, out, j);
	}

	return j;
}
#ifdef CONFIG_SCAN_BACKOP
u8 rtw_scan_backop_decision(_adapter *adapter)
{
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct mi_state mstate;
	struct mr_query_info info = {0};
	u8 backop_flags = 0;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);

	rtw_mi_status(adapter, &mstate);
	rtw_phl_mr_query_info(dvobj->phl, adapter->phl_role,
			adapter_link->wrlink, &info);

	if ((info.cur_info.ld_sta_num && mlmeext_chk_scan_backop_flags_sta(mlmeext, SS_BACKOP_EN))
		|| (info.cur_info.sta_num && mlmeext_chk_scan_backop_flags_sta(mlmeext, SS_BACKOP_EN_NL)))
		backop_flags |= mlmeext_scan_backop_flags_sta(mlmeext);

#ifdef CONFIG_AP_MODE
	if ((info.cur_info.ld_ap_num && mlmeext_chk_scan_backop_flags_ap(mlmeext, SS_BACKOP_EN))
		|| (info.cur_info.ap_num && mlmeext_chk_scan_backop_flags_ap(mlmeext, SS_BACKOP_EN_NL)))
		backop_flags |= mlmeext_scan_backop_flags_ap(mlmeext);
#endif

#ifdef CONFIG_RTW_MESH
	if ((MSTATE_MESH_LD_NUM(&mstate) && mlmeext_chk_scan_backop_flags_mesh(mlmeext, SS_BACKOP_EN))
		|| (MSTATE_MESH_NUM(&mstate) && mlmeext_chk_scan_backop_flags_mesh(mlmeext, SS_BACKOP_EN_NL)))
		backop_flags |= mlmeext_scan_backop_flags_mesh(mlmeext);
#endif

	return backop_flags;
}
#endif


#ifdef CONFIG_FSM
u8 sitesurvey_cmd_hdl(_adapter *padapter, u8 *pbuf)
{
	RTW_ERR("%s executed??\n", __func__);
	rtw_warn_on(1);
	return 0;
}
void rtw_survey_cmd_callback(_adapter  *padapter, struct cmd_obj *pcmd)
{
	RTW_ERR("%s executed??\n", __func__);
	rtw_warn_on(1);
}
#endif

/* remain on channel priv */
#define ROCH_CH_READY	0x1

struct scan_priv {
	_adapter *padapter;

	/* for remain on channel callback */
	struct wireless_dev *wdev;
	struct ieee80211_channel channel;
	u8 channel_type;
	unsigned int duration;
	u64 cookie;

	u8 restore_ch;

	u8 roch_step;
#ifdef CONFIG_RTW_80211K
	u32 rrm_token;	/* 80211k use it to identify caller */
#endif
};

#ifdef CONFIG_CMD_SCAN
static struct rtw_phl_scan_param *_alloc_phl_param(_adapter *adapter, u8 scan_ch_num)
{
	struct rtw_phl_scan_param *phl_param = NULL;
	struct scan_priv *scan_priv = NULL;

	if (scan_ch_num == 0) {
		RTW_ERR("%s scan_ch_num = 0\n", __func__);
		goto _err_exit;
	}
	/*create mem of PHL Scan parameter*/
	phl_param = rtw_zmalloc(sizeof(*phl_param));
	if (phl_param == NULL) {
		RTW_ERR("%s alloc phl_param fail\n", __func__);
		goto _err_exit;
	}

	scan_priv = rtw_zmalloc(sizeof(*scan_priv));
	if (scan_priv == NULL) {
		RTW_ERR("%s alloc scan_priv fail\n", __func__);
		goto _err_scanpriv;
	}
	scan_priv->padapter = adapter;
	phl_param->priv = scan_priv;
	phl_param->wifi_role = adapter->phl_role;
	phl_param->back_op.mode = SCAN_BKOP_NONE;

	phl_param->ch_sz = sizeof(struct phl_scan_channel) * (scan_ch_num + 1);
	phl_param->ch = rtw_zmalloc(phl_param->ch_sz);

	if (phl_param->ch == NULL) {
		RTW_ERR("%s: alloc phl scan ch fail\n", __func__);
		goto _err_param_ch;
	}

	return phl_param;

_err_param_ch:
	if (scan_priv)
		rtw_mfree(scan_priv, sizeof(*scan_priv));
_err_scanpriv:
	if (phl_param)
		rtw_mfree(phl_param, sizeof(*phl_param));
_err_exit:
	rtw_warn_on(1);
	return phl_param;
}

static u8 _free_phl_param(_adapter *adapter, struct rtw_phl_scan_param *phl_param)
{
	u8 res = _FAIL;

	if (!phl_param)
		return res;

	if (phl_param->ch)
		rtw_mfree(phl_param->ch, phl_param->ch_sz);
	if (phl_param->priv)
		rtw_mfree(phl_param->priv, sizeof(struct scan_priv));
	rtw_mfree(phl_param, sizeof(struct rtw_phl_scan_param));

	res = _SUCCESS;
	return res;
}
#endif /*CONFIG_CMD_SCAN*/
static int scan_issue_pbreq_cb(void *priv, struct rtw_phl_scan_param *param, u8 sctrl_idx)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_wifi_role_t *wrole = param->sctrl[sctrl_idx].wrole;
	NDIS_802_11_SSID ssid;
	int i;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = NULL;

	if (wrole == NULL) {
		rtw_warn_on(1);
		return 0;
	}

	padapter = dvobj->padapters[wrole->id];
	if (!rtw_is_adapter_up(padapter))
		return 0;
	padapter_link = GET_PRIMARY_LINK(padapter);

	/* active scan behavior */
	if (padapter->registrypriv.wifi_spec)
		issue_probereq(padapter, padapter_link, NULL, NULL);
	else
		issue_probereq_ex(padapter, padapter_link, NULL, NULL, 0, 0, 0, 0);

	issue_probereq(padapter, padapter_link, NULL, NULL);

	for (i = 0; i < param->ssid_num; i++) {
		if (param->ssid[i].ssid_len == 0)
			continue;

		ssid.SsidLength = param->ssid[i].ssid_len;
		_rtw_memcpy(ssid.Ssid, &param->ssid[i].ssid, ssid.SsidLength);
		/* IOT issue,
		 * Send one probe req without WPS IE,
		 * when not wifi_spec
		 */
		if (padapter->registrypriv.wifi_spec)
			issue_probereq(padapter, padapter_link, &ssid, NULL);
		else
			issue_probereq_ex(padapter, padapter_link,&ssid, NULL, 0, 0, 0, 0);

		issue_probereq(padapter, padapter_link, &ssid, NULL);
	}

	return 0;
}

static int scan_complete_cb(void *priv, struct rtw_phl_scan_param *param)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	struct	mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	bool acs = _FALSE;
	int ret = _FAIL;

	if (!rtw_is_adapter_up(padapter))
		goto _exit;

	#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
	{
		struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);

		if (pwdev_priv->random_mac_enabled
		    && (MLME_IS_STA(padapter))
		    && (check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE) == _FALSE))
			rtw_set_mac_addr_hw(padapter, adapter_mac_addr(padapter));

		pwdev_priv->random_mac_enabled = false;
	}
	#endif /* CONFIG_RTW_SCAN_RAND */

	mlmeext_set_scan_state(pmlmeext, SCAN_DISABLE);

	report_surveydone_event(padapter, acs, RTW_CMDF_DIRECTLY);
	ret = _SUCCESS;

_exit:
	RTW_INFO(FUNC_ADPT_FMT" takes %d ms to scan %d channels\n",
			FUNC_ADPT_ARG(padapter), param->total_scan_time,
			param->ch_num);
	_rtw_scan_abort_check(padapter, __func__);

#ifdef CONFIG_CMD_SCAN
	_free_phl_param(padapter, param);
	pmlmeext->sitesurvey_res.scan_param = NULL;
#else
	rtw_mfree(scan_priv, sizeof(*scan_priv));
#endif

	return ret;
}

static int scan_start_cb(void *priv, struct rtw_phl_scan_param *param)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;

	#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
	{
		struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);

		if (pwdev_priv->random_mac_enabled
		    && (MLME_IS_STA(padapter))
		    && (check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE) == _FALSE)) {
			u16 seq_num;

			rtw_set_mac_addr_hw(padapter, pwdev_priv->pno_mac_addr);
			get_random_bytes(&seq_num, 2);
			pwdev_priv->pno_scan_seq_num = seq_num & 0xFFF;
		} else {
			pwdev_priv->random_mac_enabled = false;
		}
	}
	#endif /* CONFIG_RTW_SCAN_RAND */

	pmlmeext->sitesurvey_res.bss_cnt = 0;
	pmlmeext->sitesurvey_res.activate_ch_cnt = 0;
	//TODO remove
	mlmeext_set_scan_state(pmlmeext, SCAN_PROCESS);
	#ifdef CONFIG_CMD_SCAN
	pmlmeext->sitesurvey_res.scan_param = param;
	#endif
	return 0;
}

#ifdef CONFIG_P2P
static int scan_issue_p2p_pbreq_cb(void *priv, struct rtw_phl_scan_param *param, u8 sctrl_idx)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_wifi_role_t *wrole = param->sctrl[sctrl_idx].wrole;

	if (wrole == NULL) {
		rtw_warn_on(1);
		return 0;
	}

	padapter = dvobj->padapters[wrole->id];

	if (!rtw_is_adapter_up(padapter))
		return 0;

	issue_probereq_p2p(padapter, NULL);
	issue_probereq_p2p(padapter, NULL);
	issue_probereq_p2p(padapter, NULL);
	return 0;
}
#endif

static int scan_ch_ready_cb(void *priv, struct rtw_phl_scan_param *param, u8 sctrl_idx)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	struct phl_scan_channel *scan_ch = param->sctrl[sctrl_idx].scan_ch;

	RTW_INFO("%s ch:%d\n", __func__, scan_ch->channel);
	return 0;
}


static struct rtw_phl_scan_ops scan_ops_cb = {
	.scan_start = scan_start_cb,
	.scan_ch_ready = scan_ch_ready_cb,
	.scan_complete = scan_complete_cb,
	.scan_issue_pbreq = scan_issue_pbreq_cb,
	/*.scan_issue_null_data = scan_issu_null_data_cb*/
};

#ifdef CONFIG_P2P
static struct rtw_phl_scan_ops scan_ops_p2p_cb = {
	.scan_start = scan_start_cb,
	.scan_ch_ready = scan_ch_ready_cb,
	.scan_complete = scan_complete_cb,
	.scan_issue_pbreq = scan_issue_p2p_pbreq_cb,
	/*.scan_issue_null_data = scan_issu_null_data_cb*/
};
#endif

#ifdef CONFIG_RTW_80211K
static int scan_complete_rrm_cb(void *priv, struct rtw_phl_scan_param *param)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	int ret = _FAIL;

	if (!rtw_is_adapter_up(padapter))
		goto _exit;

	mlmeext_set_scan_state(pmlmeext, SCAN_DISABLE);
	_rtw_spinlock_bh(&pmlmepriv->lock);
	_clr_fwstate_(pmlmepriv, WIFI_UNDER_SURVEY);
	_rtw_spinunlock_bh(&pmlmepriv->lock);

	/* inform RRM scan complete */
	rm_post_event(padapter, scan_priv->rrm_token, RM_EV_survey_done);
	ret = _SUCCESS;

_exit:
	RTW_INFO(FUNC_ADPT_FMT" takes %d ms to scan %d channels\n",
		FUNC_ADPT_ARG(padapter), param->total_scan_time,
		param->ch_num);
	_rtw_scan_abort_check(padapter, __func__);

#ifdef CONFIG_CMD_SCAN
	_free_phl_param(padapter, param);
	pmlmeext->sitesurvey_res.scan_param = NULL;
#else
	rtw_mfree(scan_priv, sizeof(*scan_priv));
#endif
	return ret;
}

static struct rtw_phl_scan_ops scan_ops_rrm_cb = {
	.scan_start = scan_start_cb,
	.scan_ch_ready = scan_ch_ready_cb,
	.scan_complete = scan_complete_rrm_cb,
	.scan_issue_pbreq = scan_issue_pbreq_cb,
	/*.scan_issue_null_data = scan_issu_null_data_cb*/
};
#endif /* CONFIG_RTW_80211K */

#ifndef SCAN_PER_CH_EX_TIME
#define SCAN_PER_CH_EX_TIME	40 /*8852bs sw ch overhead*/
#endif /*SCAN_PER_CH_EX_TIME*/
#ifdef CONFIG_CMD_SCAN
static u32 rtw_scan_timeout_decision(_adapter *padapter,
	struct rtw_phl_scan_param *phl_param, u16 scan_ch_ms)
{
	u32 scan_timeout_ms = 0;
	u8 max_chan_num;
	u32 back_op_times = 0;
	u16 backop_ms = 0;
	u8 scan_cnt_max;
	u16 ext_act_ms = 0;
	int i;
	u8 issue_null_time;
	u16 p_ch_ex_time;
	u32 non_op_buf;
	#ifdef CONFIG_SCAN_BACKOP
	u8 backop_cout = 0;
	#endif /*CONFIG_SCAN_BACKOP*/
	max_chan_num = phl_param->ch_num;


	if (padapter->registrypriv.scan_pch_ex_time != 0)
		p_ch_ex_time = padapter->registrypriv.scan_pch_ex_time;
	else
		p_ch_ex_time = SCAN_PER_CH_EX_TIME;

	/*issue null time,null(0)+null(1),undefine flag phl sleep 50ms*/
	issue_null_time = 10;
	#ifndef RTW_WKARD_TX_NULL_WD_RP
	issue_null_time += 50;
	#endif /*RTW_WKARD_TX_NULL_WD_RP*/

	#ifdef CONFIG_SCAN_BACKOP
	if (phl_param->back_op.mode == SCAN_BKOP_CNT) {
		backop_ms = phl_param->back_op.ch_dur_ms;
		scan_cnt_max = phl_param->back_op.ch_intv;
		if (scan_cnt_max == 0) {
			scan_cnt_max = 1;
			RTW_ERR("%s scan_cnt_max=0\n", __func__);
			rtw_warn_on(1);
		}
		backop_cout = max_chan_num / scan_cnt_max;
		back_op_times = backop_cout * (backop_ms + p_ch_ex_time + issue_null_time);
	}
	#endif

	/* Extended active scan time */
	for (i = 0; i < max_chan_num; i++) {
		if (phl_param->ch[i].ext_act_scan == EXT_ACT_SCAN_ENABLE)
			ext_act_ms += phl_param->ext_act_scan_period;
	}

	/*non op channel buffer time + scan start/done issue null*/
	non_op_buf = max_chan_num * p_ch_ex_time + issue_null_time;

	scan_timeout_ms = (scan_ch_ms * max_chan_num) + back_op_times
			  + ext_act_ms + non_op_buf;
	#ifdef DBG_SITESURVEY
	RTW_INFO("%s , max_chan_num = %d , scan_cnt_max=%d,p_ch_ex_time=%d, "
			"issue_null_time=%d\n",
			__func__,max_chan_num, scan_cnt_max,
			p_ch_ex_time, issue_null_time);

	RTW_INFO(FUNC_ADPT_FMT ": scan_timeout_ms=%u, ch_num:%u scan_ch_ms=%u, "
		 "back_op_times=%u, ext_act_ms=%u\n",
		 FUNC_ADPT_ARG(padapter), scan_timeout_ms,
		 max_chan_num, scan_ch_ms, back_op_times, ext_act_ms);
	#endif /*DBG_SITESURVEY*/

	return scan_timeout_ms;
}

/*
rtw_sitesurvey_cmd(~)
	### NOTE:#### (!!!!)
	MUST TAKE CARE THAT BEFORE CALLING THIS FUNC, YOU SHOULD HAVE LOCKED pmlmepriv->lock
*/
static void scan_channel_list_filled(_adapter *padapter,
	struct rtw_phl_scan_param *phl_param, struct sitesurvey_parm *param)
{
	struct phl_scan_channel *phl_ch = phl_param->ch;
	u8 i = 0;
	#ifdef CONFIG_RTW_ACS
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	phl_param->acs = _TRUE;
	phl_param->nhm_include_cca = _FALSE;
	#endif /*CONFIG_RTW_ACS*/

	for (i = 0; i < param->ch_num; i++) {
		phl_ch[i].channel = param->ch[i].hw_value;
		phl_ch[i].band = param->ch[i].band;
		phl_ch[i].scan_mode = NORMAL_SCAN_MODE;
		phl_ch[i].bw = param->bw;
		phl_ch[i].duration = param->duration;

		#ifdef CONFIG_RTW_ACS
		phl_ch[i].acs_idx = rtw_phl_get_acs_chnl_tbl_idx(dvobj->phl, phl_ch[i].band ,phl_ch[i].channel);
		#endif /*CONFIG_RTW_ACS*/

		if (param->ch[i].flags & RTW_IEEE80211_CHAN_PASSIVE_SCAN) {
			phl_ch[i].type = RTW_PHL_SCAN_PASSIVE;
		} else {
			phl_ch[i].type = RTW_PHL_SCAN_ACTIVE;
			phl_ch[i].duration = ACTIVE_CH_SURVEY_TO;
		}
	}
	phl_param->ch_num = param->ch_num;
}

u8 rtw_sitesurvey_cmd(_adapter *padapter, struct sitesurvey_parm *pparm)
{
	u8 res = _FAIL;
	u8 i;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct rtw_phl_scan_param *phl_param = NULL;
	struct rtw_ieee80211_channel ch[RTW_CHANNEL_SCAN_AMOUNT] = {0};
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct sitesurvey_parm *tmp_parm = NULL;
	struct mlme_ext_priv *mlmeext = &padapter->mlmeextpriv;
	struct ss_res *ss = &mlmeext->sitesurvey_res;
	struct rtw_chset *chset = adapter_to_chset(padapter);
#ifdef CONFIG_RTW_80211K
	struct scan_priv *scan_priv = NULL;
#endif

	if (pparm == NULL) {
		tmp_parm = rtw_zmalloc(sizeof(struct sitesurvey_parm));
		if (tmp_parm == NULL) {
			RTW_ERR("%s alloc tmp_parm fail\n", __func__);
			goto _err_exit;
		}
		rtw_init_sitesurvey_parm(padapter, tmp_parm);
		pparm = tmp_parm;
	}

	/* backup original ch list */
	_rtw_memcpy(ch, pparm->ch,
		sizeof(struct rtw_ieee80211_channel) * pparm->ch_num);

	/* modify ch list according to chanel plan */
	pparm->ch_num = rtw_scan_ch_decision(padapter,
					pparm->ch, RTW_CHANNEL_SCAN_AMOUNT,
					ch, pparm->ch_num, pparm->acs);

	if (pparm->duration == 0)
		pparm->duration = ss->scan_ch_ms; /* ms */

	/*create mem of PHL Scan parameter*/
	phl_param = _alloc_phl_param(padapter, pparm->ch_num);
	if (phl_param == NULL) {
		RTW_ERR("%s alloc phl_param fail\n", __func__);
		goto _err_param;
	}

	/* STEP_1 transfer to rtw channel list to phl channel list */
	scan_channel_list_filled(padapter, phl_param, pparm);

	/* STEP_2 copy the ssid info to phl param */
	phl_param->ssid_num = rtw_min(pparm->ssid_num, SCAN_SSID_AMOUNT);
	for (i = 0; i < phl_param->ssid_num; ++i) {
		phl_param->ssid[i].ssid_len = pparm->ssid[i].SsidLength;
		_rtw_memcpy(&phl_param->ssid[i].ssid, &pparm->ssid[i].Ssid, phl_param->ssid[i].ssid_len);
	}

	/* STEP_2.1 set EXT_ACT_SCAN_ENABLE for hidden AP scan */
	if (phl_param->ssid[0].ssid_len) {
		phl_param->ext_act_scan_period = RTW_EXTEND_ACTIVE_SCAN_PERIOD;
		for (i = 0; i < phl_param->ch_num; i++) {
			int chset_idx;
			chset_idx = rtw_chset_search_bch(chset,
							phl_param->ch[i].band, phl_param->ch[i].channel);
			if (chset_idx < 0) {
				RTW_ERR(FUNC_ADPT_FMT ": cann't find ch %u in chset!\n",
					FUNC_ADPT_ARG(padapter), phl_param->ch[i].channel);
				continue;
			}

			if ((phl_param->ch[i].type == RTW_PHL_SCAN_PASSIVE)
			    && !CH_IS_NON_OCP(&chset->chs[chset_idx]))
				phl_param->ch[i].ext_act_scan = EXT_ACT_SCAN_ENABLE;
		}
	}
	#ifdef CONFIG_DBCC_SUPPORT
	phl_param->opt = SCAN_CMD_OPT_DB;
	#endif

	/* STEP_3 set ops according to scan_type */
	switch (pparm->scan_type) {
	#ifdef CONFIG_P2P
	case RTW_SCAN_P2P:
		phl_param->ops = &scan_ops_p2p_cb;
	break;
	#endif

	#ifdef CONFIG_RTW_80211K
	case RTW_SCAN_RRM:
		phl_param->ops = &scan_ops_rrm_cb;
		scan_priv = (struct scan_priv *)phl_param->priv;
		scan_priv->rrm_token = pparm->rrm_token;
		ss->token = pparm->rrm_token;
	break;
	#endif

	case RTW_SCAN_NORMAL:
	default:
		phl_param->ops = &scan_ops_cb;
		#ifdef CONFIG_SCAN_BACKOP
		if (rtw_scan_backop_decision(padapter)) {
			phl_param->back_op.ch_dur_ms = ss->backop_ms;
			phl_param->back_op.mode = SCAN_BKOP_CNT;
			phl_param->back_op.ch_intv = ss->scan_cnt_max;
		} else
		#endif /* CONFIG_SCAN_BACKOP */
		{
			phl_param->back_op.mode = SCAN_BKOP_NONE;
		}
	break;
	}

	phl_param->max_scan_time = rtw_scan_timeout_decision(padapter, phl_param, pparm->duration);

	/* STEP_4 reset variables for each scan */
	for (i = 0; i < chset->chs_len; i++)
		chset->chs[i].hidden_bss_cnt = 0;

	set_fwstate(pmlmepriv, WIFI_UNDER_SURVEY);
	if(rtw_phl_cmd_scan_request(dvobj->phl, phl_param) != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s request scam_cmd failed\n", __func__);
		_clr_fwstate_(pmlmepriv, WIFI_UNDER_SURVEY);
		goto _err_req_param;
	}

	pmlmeext->sitesurvey_res.scan_param = phl_param;
	rtw_free_network_queue(padapter, _FALSE);
	rtw_free_mld_network_queue(padapter, _FALSE);

	pmlmepriv->scan_start_time = rtw_get_current_time();

	rtw_led_control(padapter, LED_CTL_SITE_SURVEY);
	if (tmp_parm)
		rtw_mfree(tmp_parm, sizeof(*tmp_parm));
	res = _SUCCESS;
	return res;

_err_req_param:
	_free_phl_param(padapter, phl_param);
_err_param:
	if (tmp_parm)
		rtw_mfree(tmp_parm, sizeof(*tmp_parm));
_err_exit:
	rtw_warn_on(1);
	return res;
}

#else /*!CONFIG_CMD_SCAN*/
static u32 rtw_scan_timeout_decision(_adapter *padapter)
{
	u32 back_op_times= 0;
	u8 max_chan_num;
	u16 scan_ms;
	u8 issue_null_time;
	u16 p_ch_ex_time;
	u32 non_op_buf;

	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct ss_res *ss = &pmlmeext->sitesurvey_res;
	#ifdef CONFIG_SCAN_BACKOP
	u8 backop_cout = 0;
	#endif /*CONFIG_SCAN_BACKOP*/

	if (padapter->registrypriv.scan_pch_ex_time != 0)
		p_ch_ex_time = padapter->registrypriv.scan_pch_ex_time;
	else
		p_ch_ex_time = SCAN_PER_CH_EX_TIME;

	/*issue null time,null(0)+null(1),undefine flag phl sleep 50ms*/
	issue_null_time = 10;
	#ifndef RTW_WKARD_TX_NULL_WD_RP
	issue_null_time += 50;
	#endif /*RTW_WKARD_TX_NULL_WD_RP*/

	if (is_supported_5g(padapter->registrypriv.band_type)
		&& is_supported_24g(padapter->registrypriv.band_type))
		max_chan_num = MAX_CHANNEL_NUM_2G_5G;/* dual band */
	else
		max_chan_num = MAX_CHANNEL_NUM_2G;/*single band*/

	#ifdef CONFIG_SCAN_BACKOP
	if(ss->scan_cnt_max != 0)
		backop_cout = max_chan_num / ss->scan_cnt_max;
	/* delay 50ms to protect nulldata(1) */
	if (rtw_scan_backop_decision(padapter))
		back_op_times = backop_cout * (ss->backop_ms + p_ch_ex_time + issue_null_time);
	#endif /*CONFIG_SCAN_BACKOP*/

	if (ss->duration)
		scan_ms = ss->duration;
	else
	#if defined(CONFIG_RTW_ACS) && defined(CONFIG_RTW_ACS_DBG)
	if (IS_ACS_ENABLE(padapter) && rtw_is_acs_st_valid(padapter))
		scan_ms = rtw_acs_get_adv_st(padapter);
	else
	#endif /*CONFIG_RTW_ACS*/
		scan_ms = ss->scan_ch_ms;

	/*non op channel buffer time + scan start/done issue null*/
	non_op_buf = max_chan_num * p_ch_ex_time + issue_null_time;

	ss->scan_timeout_ms = (scan_ms * max_chan_num) + back_op_times + non_op_buf;
	#ifdef DBG_SITESURVEY
	RTW_INFO("%s, max_chan_num(%d), p_ch_ex_time(%d), \
		 ss->scan_cnt_max=%d issue_null_time=%d\n" \
		 , __func__, max_chan_num, p_ch_ex_time,
		 ss->scan_cnt_max, issue_null_time);
	RTW_INFO("%s , scan_timeout_ms = %d (ms), scan_ms=%d (ms), back_op_times=%d (ms), ss->duration=%d (ms)\n"
		, __func__, ss->scan_timeout_ms, scan_ms, back_op_times, ss->duration);
	#endif /*DBG_SITESURVEY*/

	return ss->scan_timeout_ms;
}

/**
 * prepare phl_channel list according to SCAN type
 *
 */
static int scan_channel_list_preparation(_adapter *padapter,
	struct rtw_phl_scan_param *dst, struct sitesurvey_parm *src)
{
	struct phl_scan_channel *phl_ch = NULL;
	int phl_ch_sz = 0;
	int i;

	phl_ch_sz = sizeof(struct phl_scan_channel) * (src->ch_num + 1);

	phl_ch = rtw_malloc(phl_ch_sz);
	if (phl_ch == NULL) {
		RTW_ERR("scan: alloc phl scan ch fail\n");
		return -1;
	}
	_rtw_memset(phl_ch, 0, phl_ch_sz);

	i = 0;
	while (i < src->ch_num) {

		phl_ch[i].channel = src->ch[i].hw_value;
		phl_ch[i].scan_mode = NORMAL_SCAN_MODE;
		phl_ch[i].bw = src->bw;
		phl_ch[i].duration = src->duration;

		if (src->ch[i].flags & RTW_IEEE80211_CHAN_PASSIVE_SCAN) {
			phl_ch[i].type = RTW_PHL_SCAN_PASSIVE;

		} else {
			phl_ch[i].type = RTW_PHL_SCAN_ACTIVE;

			/* reduce scan time in active channel */
			if (src->scan_type == RTW_SCAN_NORMAL)
				phl_ch[i].duration = src->duration >> 1;
		}
		i++;
	}

	dst->ch = phl_ch;
	dst->ch_sz = phl_ch_sz;
	dst->ch_num = src->ch_num;

	return 0;
}

u32 rtw_site_survey_fsm(_adapter *padapter, struct cmd_obj *pcmd)
{
	u32 res = RTW_PHL_STATUS_FAILURE;
	struct scan_priv *scan_priv;
	struct rtw_phl_scan_param *phl_param;
	struct sitesurvey_parm *rtw_param;
	struct rtw_ieee80211_channel ch[RTW_CHANNEL_SCAN_AMOUNT];
	u8 i;

	scan_priv = rtw_malloc(sizeof(*scan_priv));
	if (scan_priv == NULL) {
		RTW_ERR("scan: %s alloc scan_priv fail\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}
	_rtw_memset(scan_priv, 0, sizeof(*scan_priv));
	scan_priv->padapter = padapter;
	rtw_param = (struct sitesurvey_parm *)pcmd->parmbuf;

	if (rtw_param->duration == 0)
		rtw_param->duration = SURVEY_TO; /* ms */

	/* backup original ch list */
	_rtw_memcpy(ch, rtw_param->ch,
		sizeof(struct rtw_ieee80211_channel) *
		rtw_param->ch_num);

	/* modify ch list according to chanel plan */
	rtw_param->ch_num = rtw_scan_ch_decision(padapter,
				rtw_param->ch, RTW_CHANNEL_SCAN_AMOUNT,
				ch, rtw_param->ch_num, rtw_param->acs);

	phl_param = rtw_malloc(sizeof(*phl_param));
	if (phl_param == NULL) {
		RTW_ERR("scan: %s alloc param fail\n", __func__);
		if (scan_priv)
			rtw_mfree(scan_priv, sizeof(*scan_priv));
			return RTW_PHL_STATUS_FAILURE;
		}
	_rtw_memset(phl_param, 0, sizeof(*phl_param));

	/* transfer to rtw channel list to phl channel list */
	scan_channel_list_preparation(padapter, phl_param, rtw_param);

	/* copy the ssid info to phl param */
	phl_param->ssid_num = rtw_min(rtw_param->ssid_num, SCAN_SSID_AMOUNT);
	for (i = 0; i < phl_param->ssid_num; ++i) {
		phl_param->ssid[i].ssid_len = rtw_param->ssid[i].SsidLength;
		_rtw_memcpy(&phl_param->ssid[i].ssid, &rtw_param->ssid[i].Ssid, phl_param->ssid[i].ssid_len);
	}

	switch (rtw_param->scan_type) {
#ifdef CONFIG_P2P
	case RTW_SCAN_P2P:
		phl_param->ops = &scan_ops_p2p_cb;
	break;
#endif
#ifdef CONFIG_RTW_80211K
	case RTW_SCAN_RRM:
		phl_param->ops = &scan_ops_rrm_cb;
		if (rtw_param->ch_num > 13) {
			phl_param->back_op.mode = SCAN_BKOP_CNT;
			phl_param->back_op.ch_intv = 3;
		}
	break;
#endif
	case RTW_SCAN_NORMAL:
	default:
		phl_param->ops = &scan_ops_cb;
		phl_param->back_op.mode = SCAN_BKOP_CNT;
		phl_param->back_op.ch_intv = 3;
		break;
	}
	phl_param->priv = scan_priv;
	phl_param->wifi_role = padapter->phl_role;

	res = rtw_phl_scan_request(adapter_to_dvobj(padapter)->phl, phl_param, TO_TAIL);
	rtw_mfree(phl_param->ch, phl_param->ch_sz);
	rtw_mfree(phl_param, sizeof(*phl_param));

	return res;
}

u8 rtw_sitesurvey_cmd(_adapter *padapter, struct sitesurvey_parm *pparm)
{
	u8 res = _FAIL;
	struct cmd_obj *cmd;
	struct sitesurvey_parm	*psurveyPara;
	struct cmd_priv	*pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;

#ifdef CONFIG_LPS
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		rtw_lps_ctrl_wk_cmd(padapter, LPS_CTRL_SCAN, 0);
#endif

#ifdef CONFIG_P2P_PS
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		p2p_ps_wk_cmd(padapter, P2P_PS_SCAN, 1);
#endif /* CONFIG_P2P_PS */

	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL)
		return _FAIL;
	cmd->padapter = padapter;

	psurveyPara = (struct sitesurvey_parm *)rtw_zmalloc(sizeof(struct sitesurvey_parm));
	if (psurveyPara == NULL) {
		rtw_mfree((unsigned char *) cmd, sizeof(struct cmd_obj));
		return _FAIL;
	}

	if (pparm)
		_rtw_memcpy(psurveyPara, pparm, sizeof(struct sitesurvey_parm));
	else
		psurveyPara->scan_mode = pmlmepriv->scan_mode;

	rtw_free_network_queue(padapter, _FALSE);
	rtw_free_mld_network_queue(padapter, _FALSE);

	init_h2fwcmd_w_parm_no_rsp(cmd, psurveyPara, CMD_SITE_SURVEY);

	set_fwstate(pmlmepriv, WIFI_UNDER_SURVEY);


	res = rtw_enqueue_cmd(pcmdpriv, cmd);

	if (res == _SUCCESS) {
		u32 scan_timeout_ms;

		pmlmepriv->scan_start_time = rtw_get_current_time();
		scan_timeout_ms = rtw_scan_timeout_decision(padapter);
		mlme_set_scan_to_timer(pmlmepriv,scan_timeout_ms);

		rtw_led_control(padapter, LED_CTL_SITE_SURVEY);
	} else {
		_clr_fwstate_(pmlmepriv, WIFI_UNDER_SURVEY);
	}


	return res;
}
#endif/*CONFIG_CMD_SCAN*/


/* inform caller phl_scan are ready on remain channel */
static int roch_ready_cb(void *priv, struct rtw_phl_scan_param *param, u8 sctrl_idx)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	struct cfg80211_roch_info *pcfg80211_rochinfo =
		&padapter->cfg80211_rochinfo;

	RTW_INFO("%s cookie:0x%llx\n", __func__,
		pcfg80211_rochinfo->remain_on_ch_cookie);

	if ((scan_priv->roch_step & ROCH_CH_READY))
		return 0;

	scan_priv->roch_step |= ROCH_CH_READY;

	rtw_cfg80211_ready_on_channel(
		scan_priv->wdev,
		scan_priv->cookie,
		&scan_priv->channel,
		scan_priv->channel_type,
		scan_priv->duration,
		GFP_KERNEL);
	return 0;
}

static int roch_off_ch_tx_cb(void *priv,
	struct rtw_phl_scan_param *param, void *data)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(scan_priv->padapter);

#ifdef CONFIG_CMD_SCAN
	RTW_ERR("CMD_SCAN call %s\n", __func__);
	rtw_warn_on(1);
#else
	phl_cmd_complete_job(dvobj->phl, (struct phl_cmd_job *)data);
#endif
	return 0;
}

#ifdef CONFIG_P2P
static int p2p_roch_complete_cb(void *priv, struct rtw_phl_scan_param *param)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	int ret = _FAIL;
	struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);
	struct cfg80211_roch_info *pcfg80211_rochinfo =
		&padapter->cfg80211_rochinfo;
	struct wifidirect_info *pwdinfo = &padapter->wdinfo;

	if (!rtw_is_adapter_up(padapter))
		goto _exit;

	mlmeext_set_scan_state(&padapter->mlmeextpriv, SCAN_DISABLE);

	/* roch_ready() and roch_complete() MUST be a PAIR
	 * otherwise will caurse wpa_supplicant hang!!!
	 * This case may happen when someone cancel remain on ch
	 * before it really start. (called roch_ready()).
	 */
	if (!(scan_priv->roch_step & ROCH_CH_READY))
		roch_ready_cb(priv, param, 0);

#ifndef CONFIG_CMD_SCAN
	rtw_back_opch(padapter);
#endif
#ifdef CONFIG_DEBUG_CFG80211
	RTW_INFO("%s, role=%d\n", __func__, rtw_p2p_role(pwdinfo));
#endif

	rtw_cfg80211_set_is_roch(padapter, _FALSE);
	pcfg80211_rochinfo->ro_ch_wdev = NULL;
	rtw_cfg80211_set_last_ro_ch_time(padapter);

	ret = _SUCCESS;
_exit:
	/* callback to cfg80211 */
	rtw_cfg80211_remain_on_channel_expired(scan_priv->wdev
		, scan_priv->cookie
		, &scan_priv->channel
		, scan_priv->channel_type, GFP_KERNEL);

	RTW_INFO("cfg80211_remain_on_channel_expired cookie:0x%llx\n"
		, pcfg80211_rochinfo->remain_on_ch_cookie);

	RTW_INFO(FUNC_ADPT_FMT" takes %d ms to scan %d channels\n",
			FUNC_ADPT_ARG(padapter), param->total_scan_time,
			param->ch_num);
	_rtw_scan_abort_check(padapter, __func__);

#ifdef CONFIG_CMD_SCAN
	_free_phl_param(padapter, param);
	padapter->mlmeextpriv.sitesurvey_res.scan_param = NULL;
#else
	rtw_mfree(scan_priv, sizeof(*scan_priv));
#endif
	return ret;
}

static int p2p_roch_start_cb(void *priv, struct rtw_phl_scan_param *param)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	struct cfg80211_roch_info *pcfg80211_rochinfo;

	pcfg80211_rochinfo = &padapter->cfg80211_rochinfo;

	//TODO remove
	mlmeext_set_scan_state(&padapter->mlmeextpriv, SCAN_PROCESS);

	rtw_cfg80211_set_is_roch(padapter, _TRUE);
	pcfg80211_rochinfo->ro_ch_wdev = scan_priv->wdev;
	pcfg80211_rochinfo->remain_on_ch_cookie = scan_priv->cookie;
	pcfg80211_rochinfo->duration = scan_priv->duration;
	rtw_cfg80211_set_last_ro_ch_time(padapter);
	_rtw_memcpy(&pcfg80211_rochinfo->remain_on_ch_channel,
		&scan_priv->channel, sizeof(struct ieee80211_channel));
	#if (KERNEL_VERSION(3, 8, 0) > LINUX_VERSION_CODE)
	pcfg80211_rochinfo->remain_on_ch_type = scan_priv->channel_type;
	#endif
	pcfg80211_rochinfo->restore_channel = scan_priv->restore_ch;

	#ifdef CONFIG_CMD_SCAN
	padapter->mlmeextpriv.sitesurvey_res.scan_param = param;
	#endif

	return 0;
}
#endif

static int roch_start_cb(void *priv, struct rtw_phl_scan_param *param)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;

	mlmeext_set_scan_state(&padapter->mlmeextpriv, SCAN_PROCESS);
	rtw_cfg80211_set_is_roch(padapter, _TRUE);
	#ifdef CONFIG_CMD_SCAN
	padapter->mlmeextpriv.sitesurvey_res.scan_param = param;
	#endif

	return 0;
}

static int roch_complete_cb(void *priv, struct rtw_phl_scan_param *param)
{
	struct scan_priv *scan_priv = (struct scan_priv *)priv;
	_adapter *padapter = scan_priv->padapter;
	struct cfg80211_roch_info *pcfg80211_rochinfo =
		&padapter->cfg80211_rochinfo;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	int ret = _FAIL;

	if (!rtw_is_adapter_up(padapter))
		goto _exit;

	mlmeext_set_scan_state(pmlmeext, SCAN_DISABLE);

	/* roch_ready() and roch_complete() MUST be a PAIR
	 * otherwise will caurse wpa_supplicant hang!!!
	 * This case may happen when someone cancel remain on ch
	 * before it really start. (called roch_ready()).
	 */
	if (!(scan_priv->roch_step & ROCH_CH_READY))
		roch_ready_cb(priv, param, 0);

	rtw_cfg80211_set_is_roch(padapter, _FALSE);

	ret = _SUCCESS;

_exit:
	/* callback to cfg80211 */
	rtw_cfg80211_remain_on_channel_expired(scan_priv->wdev
		, scan_priv->cookie
		, &scan_priv->channel
		, scan_priv->channel_type, GFP_KERNEL);

	RTW_INFO("cfg80211_remain_on_channel_expired cookie:0x%llx\n"
		, pcfg80211_rochinfo->remain_on_ch_cookie);

	RTW_INFO(FUNC_ADPT_FMT" takes %d ms to scan %d channels\n",
			FUNC_ADPT_ARG(padapter), param->total_scan_time,
			param->ch_num);
	_rtw_scan_abort_check(padapter, __func__);

#ifdef CONFIG_CMD_SCAN
	_free_phl_param(padapter, param);
	pmlmeext->sitesurvey_res.scan_param = NULL;
#else
	rtw_mfree(scan_priv, sizeof(*scan_priv));
#endif
	return ret;
}

#ifdef CONFIG_P2P
/* p2p remain on channel */
static struct rtw_phl_scan_ops p2p_remain_ops_cb = {
	.scan_start = p2p_roch_start_cb,
	.scan_ch_ready = roch_ready_cb,
	.scan_off_ch_tx = roch_off_ch_tx_cb,
	.scan_complete = p2p_roch_complete_cb,
	/*.scan_issue_null_data = scan_issu_null_data_cb*/
};
#endif

/* normal remain on channel */
static struct rtw_phl_scan_ops remain_ops_cb = {
	.scan_start = roch_start_cb,
	.scan_ch_ready = roch_ready_cb,
	.scan_off_ch_tx = roch_off_ch_tx_cb,
	.scan_complete = roch_complete_cb,
	/*.scan_issue_null_data = scan_issu_null_data_cb*/
};

#ifdef CONFIG_IOCTL_CFG80211
static u8 roch_stay_in_cur_chan(_adapter *padapter)
{
	int i;
	_adapter *iface;
	struct mlme_priv *pmlmepriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u8 rst = _FALSE;

	for (i = 0; i < dvobj->iface_nums; i++) {
		if (!dvobj->padapters[i])
			continue;

		iface = dvobj->padapters[i];
		if (!rtw_iface_at_same_hwband(padapter, iface))
			continue;

		pmlmepriv = &iface->mlmepriv;

		if (check_fwstate(pmlmepriv, WIFI_UNDER_LINKING | WIFI_UNDER_WPS | WIFI_UNDER_KEY_HANDSHAKE) == _TRUE) {
			RTW_INFO(ADPT_FMT"- WIFI_UNDER_LINKING |WIFI_UNDER_WPS | WIFI_UNDER_KEY_HANDSHAKE (mlme state:0x%x)\n",
					ADPT_ARG(iface), get_fwstate(&iface->mlmepriv));
			rst = _TRUE;
			break;
		}
		#ifdef CONFIG_AP_MODE
		if (MLME_IS_AP(iface) || MLME_IS_MESH(iface)) {
			if (rtw_ap_sta_states_check(iface) == _TRUE) {
				rst = _TRUE;
			break;
			}
		}
		#endif
	}

	return rst;
}

#ifdef CONFIG_CMD_SCAN
u8 rtw_phl_remain_on_ch_cmd(_adapter *padapter,
	u64 cookie, struct wireless_dev *wdev,
	struct ieee80211_channel *ch, u8 ch_type,
	unsigned int duration, struct back_op_param *bkop_parm,
	u8 is_p2p)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_phl_scan_param *phl_param = NULL;
	struct scan_priv *scan_priv = NULL;
	u16 remain_ch;
	u8 chan_num;
	u8 res = _FAIL;
	struct rtw_chan_def u_chdef = {0};
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	/* prepare remain channel - check channel */
	remain_ch = (u16)ieee80211_frequency_to_channel(ch->center_freq);
	if (roch_stay_in_cur_chan(padapter) == _TRUE) {
		if (rtw_phl_mr_get_chandef(dvobj->phl, padapter->phl_role,
					padapter_link->wrlink, &u_chdef)
						!= RTW_PHL_STATUS_SUCCESS) {
			RTW_ERR("%s get union chandef failed\n", __func__);
			rtw_warn_on(1);
		}
		remain_ch = u_chdef.chan;
		RTW_INFO(FUNC_ADPT_FMT" stay in union ch:%d\n",
			FUNC_ADPT_ARG(padapter), remain_ch);
	}
	chan_num = 1;

	phl_param = _alloc_phl_param(padapter, chan_num);
	if (phl_param == NULL) {
		RTW_ERR("%s alloc phl_param fail\n", __func__);
		goto _err_exit;
	}

	/*** fill phl parameter - scan_priv ***/
	scan_priv = (struct scan_priv *)phl_param->priv;
	scan_priv->padapter = padapter;
	scan_priv->wdev = wdev;
	_rtw_memcpy(&scan_priv->channel, ch, sizeof(*ch));
	scan_priv->channel_type = ch_type;
	scan_priv->cookie = cookie;
	scan_priv->duration = duration;
	scan_priv->restore_ch = rtw_get_oper_ch(padapter, padapter_link);

	/* fill phl param - chan */
	phl_param->ch->channel = remain_ch;
	phl_param->ch->bw = CHANNEL_WIDTH_20;
	phl_param->ch->band = nl80211_band_to_rtw_band(ch->band);
	phl_param->ch->duration = duration;
	phl_param->ch->scan_mode = P2P_LISTEN_MODE;
	phl_param->ch_num = chan_num;

	/* fill back op param */
	phl_param->back_op.mode = SCAN_BKOP_TIMER;
	phl_param->back_op.ch_intv = 1;
	phl_param->back_op.ch_dur_ms = bkop_parm->on_ch_dur;/*op_ch time*/
	phl_param->back_op.off_ch_dur_ms = bkop_parm->off_ch_dur;/*ro_ch time*/
	phl_param->back_op.off_ch_ext_dur_ms = bkop_parm->off_ch_ext_dur;

	#ifdef CONFIG_DBCC_SUPPORT
	phl_param->opt = SCAN_CMD_OPT_DB;
	#endif
	#ifdef CONFIG_P2P
	/* set ops according to is_p2p */
	if (is_p2p)
		phl_param->ops = &p2p_remain_ops_cb;
	else
	#endif
		phl_param->ops = &remain_ops_cb;

	phl_param->max_scan_time = duration;
	if(rtw_phl_cmd_scan_request(dvobj->phl, phl_param) == RTW_PHL_STATUS_FAILURE) {
		RTW_ERR("%s request scam_cmd failed\n", __func__);
		goto _err_req_param;
	}

	RTW_INFO(FUNC_ADPT_FMT" ch:%u duration:%d, cookie:0x%llx\n"
			, FUNC_ADPT_ARG(padapter), remain_ch,	duration, cookie);
	res = _SUCCESS;
	return res;

_err_req_param:
	_free_phl_param(padapter, phl_param);
_err_exit:
	rtw_warn_on(1);
	return res;
}

#else
u8 rtw_phl_remain_on_ch_cmd(_adapter *padapter,
	u64 cookie, struct wireless_dev *wdev,
	struct ieee80211_channel *ch, u8 ch_type,
	unsigned int duration, struct back_op_param *bkop_parm,
	u8 is_p2p)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_phl_scan_param phl_param;
	struct scan_priv *scan_priv;
	struct phl_scan_channel phl_ch;
	int phl_ch_sz = 0;
	u16 remain_ch;
	u8 res = _FAIL;
	struct rtw_chan_def u_chdef = {0};

	_rtw_memset(&phl_param, 0, sizeof(phl_param));

	scan_priv = rtw_malloc(sizeof(*scan_priv));
	if (scan_priv == NULL) {
		RTW_ERR("scan: %s alloc scan_priv fail\n", __func__);
		return res;
	}
	_rtw_memset(scan_priv, 0, sizeof(*scan_priv));

	scan_priv->padapter = padapter;
	scan_priv->wdev = wdev;
	_rtw_memcpy(&scan_priv->channel, ch, sizeof(*ch));
	scan_priv->channel_type = ch_type;

	scan_priv->cookie = cookie;
	scan_priv->duration = duration;
	scan_priv->restore_ch = rtw_get_oper_ch(padapter);

	phl_param.priv = scan_priv;

	/* check channel */
	remain_ch = (u16)ieee80211_frequency_to_channel(ch->center_freq);

	if (roch_stay_in_cur_chan(padapter) == _TRUE) {
		if (rtw_phl_mr_get_chandef(dvobj->phl, padapter->phl_role, &u_chdef)
							!= RTW_PHL_STATUS_SUCCESS) {
			RTW_ERR("%s get union chandef failed\n", __func__);
			rtw_warn_on(1);
		}
		remain_ch = u_chdef.chan;
		RTW_INFO(FUNC_ADPT_FMT" stay in union ch:%d\n",
			FUNC_ADPT_ARG(padapter), remain_ch);
	}

	/* prepare remain channel */
	phl_ch_sz = sizeof(struct phl_scan_channel);
	_rtw_memset(&phl_ch, 0, phl_ch_sz);

	phl_ch.channel = remain_ch;
	phl_ch.duration = scan_priv->duration;
	phl_ch.scan_mode = NORMAL_SCAN_MODE;
	phl_ch.bw = CHANNEL_WIDTH_20;

	phl_param.ch = &phl_ch;
	phl_param.ch_sz = phl_ch_sz;
	phl_param.ch_num = 1;
	phl_param.wifi_role = padapter->phl_role;

	phl_param.back_op.mode = SCAN_BKOP_TIMER;
	phl_param.back_op.ch_dur_ms = bkop_parm->on_ch_dur;
	phl_param.back_op.off_ch_dur_ms = bkop_parm->off_ch_dur;
	phl_param.back_op.off_ch_ext_dur_ms = bkop_parm->off_ch_ext_dur;

#ifdef CONFIG_P2P
	if (is_p2p)
		phl_param.ops = &p2p_remain_ops_cb;
	else
#endif
		phl_param.ops = &remain_ops_cb;

	RTW_INFO(FUNC_ADPT_FMT" ch:%u duration:%d, cookie:0x%llx\n"
		, FUNC_ADPT_ARG(padapter), phl_ch.channel,
		scan_priv->duration, cookie);

	/* sent message to request phl scan
	 * IMMEDIATE imply cancelling previous scan request if has
	 */
	rtw_phl_scan_request(dvobj->phl, &phl_param, IMMEDIATE);

	/* scan_priv will be cancelled in roch_complete_cb */
	res = _SUCCESS;
	return res;
}
#endif
#endif /*CONFIG_IOCTL_CFG80211*/

