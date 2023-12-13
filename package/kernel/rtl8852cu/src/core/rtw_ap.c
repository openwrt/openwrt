/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#define _RTW_AP_C_

#include <drv_types.h>

#ifdef CONFIG_AP_MODE

extern unsigned char	RTW_WPA_OUI[];
extern unsigned char	WMM_OUI[];
extern unsigned char	WPS_OUI[];
extern unsigned char	P2P_OUI[];
extern unsigned char	WFD_OUI[];

void init_mlme_ap_info(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);

	_rtw_spinlock_init(&pmlmepriv->bcn_update_lock);
	/* pmlmeext->bstart_bss = _FALSE; */
}

void free_mlme_ap_info(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);

	stop_ap_mode(padapter);
	_rtw_spinlock_free(&pmlmepriv->bcn_update_lock);

}

/*
* Set TIM IE
* return length of total TIM IE
*/
u8 rtw_set_tim_ie(u8 dtim_cnt, u8 dtim_period
	, const u8 *tim_bmp, u8 tim_bmp_len, u8 *tim_ie)
{
	u8 *p = tim_ie;
	u8 i, n1, n2;
	u8 bmp_len;

	if (rtw_bmp_not_empty(tim_bmp, tim_bmp_len)) {
		/* find the first nonzero octet in tim_bitmap */
		for (i = 0; i < tim_bmp_len; i++)
			if (tim_bmp[i])
				break;
		n1 = i & 0xFE;
	
		/* find the last nonzero octet in tim_bitmap, except octet 0 */
		for (i = tim_bmp_len - 1; i > 0; i--)
			if (tim_bmp[i])
				break;
		n2 = i;
		bmp_len = n2 - n1 + 1;
	} else {
		n1 = n2 = 0;
		bmp_len = 1;
	}

	*p++ = WLAN_EID_TIM;
	*p++ = 2 + 1 + bmp_len;
	*p++ = dtim_cnt;
	*p++ = dtim_period;
	*p++ = (rtw_bmp_is_set(tim_bmp, tim_bmp_len, 0) ? BIT0 : 0) | n1;
	_rtw_memcpy(p, tim_bmp + n1, bmp_len);

#if 0
	RTW_INFO("n1:%u, n2:%u, bmp_offset:%u, bmp_len:%u\n", n1, n2, n1 / 2, bmp_len);
	RTW_INFO_DUMP("tim_ie: ", tim_ie + 2, 2 + 1 + bmp_len);
#endif
	return 2 + 2 + 1 + bmp_len;
}

static void update_BCNTIM(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork_mlmeext = &(pmlmeinfo->network);
	unsigned char *pie = pnetwork_mlmeext->IEs;

#if 0


	/* update TIM IE */
	/* if(rtw_tim_map_anyone_be_set(padapter, pstapriv->tim_bitmap)) */
#endif
	if (_TRUE) {
		u8 *p, *dst_ie, *premainder_ie = NULL, *pbackup_remainder_ie = NULL;
		uint offset, tmp_len, tim_ielen, tim_ie_offset, remainder_ielen;

		p = rtw_get_ie(pie + _FIXED_IE_LENGTH_, _TIM_IE_, &tim_ielen, pnetwork_mlmeext->IELength - _FIXED_IE_LENGTH_);
		if (p != NULL && tim_ielen > 0) {
			tim_ielen += 2;

			premainder_ie = p + tim_ielen;

			tim_ie_offset = (sint)(p - pie);

			remainder_ielen = pnetwork_mlmeext->IELength - tim_ie_offset - tim_ielen;

			/*append TIM IE from dst_ie offset*/
			dst_ie = p;
		} else {
			tim_ielen = 0;

			/*calculate head_len*/
			offset = _FIXED_IE_LENGTH_;

			/* get ssid_ie len */
			p = rtw_get_ie(pie + _BEACON_IE_OFFSET_, _SSID_IE_, &tmp_len, (pnetwork_mlmeext->IELength - _BEACON_IE_OFFSET_));
			if (p != NULL)
				offset += tmp_len + 2;

			/*get supported rates len*/
			p = rtw_get_ie(pie + _BEACON_IE_OFFSET_, _SUPPORTEDRATES_IE_, &tmp_len, (pnetwork_mlmeext->IELength - _BEACON_IE_OFFSET_));
			if (p !=  NULL)
				offset += tmp_len + 2;

			/* DS Parameter Set IE */
			p = rtw_get_ie(pie + _BEACON_IE_OFFSET_, _DSSET_IE_, &tmp_len,
				pnetwork_mlmeext->IELength - _BEACON_IE_OFFSET_);
			if (p)
				offset += tmp_len + 2;

			premainder_ie = pie + offset;

			remainder_ielen = pnetwork_mlmeext->IELength - offset - tim_ielen;

			/*append TIM IE from offset*/
			dst_ie = pie + offset;

		}

		if (remainder_ielen > 0) {
			pbackup_remainder_ie = rtw_malloc(remainder_ielen);
			if (pbackup_remainder_ie && premainder_ie)
				_rtw_memcpy(pbackup_remainder_ie, premainder_ie, remainder_ielen);
		}

		/* append TIM IE */
		dst_ie += rtw_set_tim_ie(0, 1, pstapriv->tim_bitmap, pstapriv->aid_bmp_len, dst_ie);

		/*copy remainder IE*/
		if (pbackup_remainder_ie) {
			_rtw_memcpy(dst_ie, pbackup_remainder_ie, remainder_ielen);

			rtw_mfree(pbackup_remainder_ie, remainder_ielen);
		}

		offset = (uint)(dst_ie - pie);
		pnetwork_mlmeext->IELength = offset + remainder_ielen;

	}
}

void rtw_add_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index, u8 *data, u8 len)
{
	PNDIS_802_11_VARIABLE_IEs	pIE;
	u8	bmatch = _FALSE;
	u8	*pie = pnetwork->IEs;
	u8	*p = NULL, *dst_ie = NULL, *premainder_ie = NULL, *pbackup_remainder_ie = NULL;
	u32	i, offset, ielen = 0, ie_offset, remainder_ielen = 0;

	if (pnetwork->IELength + len > MAX_IE_SZ) {
		RTW_ERR("Can't add IE to beacon, because size is more than MAX_IE_SZ\n");
		return;
	}

	/* Search element id (index) exits or not */
	for (i = sizeof(NDIS_802_11_FIXED_IEs); i < pnetwork->IELength;) {
		pIE = (PNDIS_802_11_VARIABLE_IEs)(pnetwork->IEs + i);

		if (pIE->ElementID > index)
			break;
		else if (pIE->ElementID == index) { /* already exist the same IE */
			p = (u8 *)pIE;
			ielen = pIE->Length;
			bmatch = _TRUE;
			break;
		}

		p = (u8 *)pIE;
		ielen = pIE->Length;
		i += (pIE->Length + 2);
	}

	/* Backup remainder IE */
	if (p != NULL && ielen > 0) {
		ielen += 2;

		premainder_ie = p + ielen;

		ie_offset = (sint)(p - pie);

		remainder_ielen = pnetwork->IELength - ie_offset - ielen;

		if (bmatch)
			dst_ie = p;
		else
			dst_ie = (p + ielen);
	}

	if (dst_ie == NULL)
		return;

	if (remainder_ielen > 0) {
		pbackup_remainder_ie = rtw_malloc(remainder_ielen);
		if (pbackup_remainder_ie && premainder_ie)
			_rtw_memcpy(pbackup_remainder_ie, premainder_ie, remainder_ielen);
	}

	*dst_ie++ = index;
	*dst_ie++ = len;

	_rtw_memcpy(dst_ie, data, len);
	dst_ie += len;

	/* Append remainder IE */
	if (pbackup_remainder_ie) {
		_rtw_memcpy(dst_ie, pbackup_remainder_ie, remainder_ielen);

		rtw_mfree(pbackup_remainder_ie, remainder_ielen);
	}

	offset = (uint)(dst_ie - pie);
	pnetwork->IELength = offset + remainder_ielen;
}

void rtw_remove_bcn_ie(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index)
{
	u8 *p, *dst_ie = NULL, *premainder_ie = NULL, *pbackup_remainder_ie = NULL;
	uint offset, ielen, ie_offset, remainder_ielen = 0;
	u8	*pie = pnetwork->IEs;

	p = rtw_get_ie(pie + _FIXED_IE_LENGTH_, index, &ielen, pnetwork->IELength - _FIXED_IE_LENGTH_);
	if (p != NULL && ielen > 0) {
		ielen += 2;

		premainder_ie = p + ielen;

		ie_offset = (sint)(p - pie);

		remainder_ielen = pnetwork->IELength - ie_offset - ielen;

		dst_ie = p;
	} else
		return;

	if (remainder_ielen > 0) {
		pbackup_remainder_ie = rtw_malloc(remainder_ielen);
		if (pbackup_remainder_ie && premainder_ie)
			_rtw_memcpy(pbackup_remainder_ie, premainder_ie, remainder_ielen);
	}

	/* copy remainder IE */
	if (pbackup_remainder_ie) {
		_rtw_memcpy(dst_ie, pbackup_remainder_ie, remainder_ielen);

		rtw_mfree(pbackup_remainder_ie, remainder_ielen);
	}

	offset = (uint)(dst_ie - pie);
	pnetwork->IELength = offset + remainder_ielen;
}

/**
 * rtw_add_bcn_ie_ex - Add Element ID Extension into Element ID (0xff)
 * Frame format: | element id (0xff) | length | element id extension | content |
 *
 * @eid_ex: element id extension
 * @data: eid_ex + content
 * @len: length
*/
void rtw_add_bcn_ie_ex(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 eid_ex, u8 *data, u8 len)
{
	struct ieee80211_info_element *pie;
	bool find_ex_ie = _FALSE;
	bool bmatch = _FALSE;
	u8 *ies = pnetwork->IEs;
	u8 *p = NULL, *dst_ie = NULL, *premainder_ie = NULL, *pbackup_remainder_ie = NULL;
	u32 i, offset, ielen = 0, ie_offset, remainder_ielen = 0;

	if (pnetwork->IELength + len > MAX_IE_SZ) {
		RTW_ERR("Can't add IE to beacon, because size is more than MAX_IE_SZ\n");
		return;
	}

	/* Search element id extension exits or not */
	for (i = sizeof(NDIS_802_11_FIXED_IEs); i < pnetwork->IELength;) {
		pie = (struct ieee80211_info_element *)(ies + i);

		if (pie->id == WLAN_EID_EXTENSION) {
			find_ex_ie = _TRUE;
			if (pie->data[0] > eid_ex) { /* first byte of data is element id extension */
				break;
			} else if (pie->data[0] == eid_ex) { /* already exist the same Element ID Extension */
				p = (u8 *)pie;
				ielen = pie->len;
				bmatch = _TRUE;
				break;
			}
		} else if (find_ex_ie) { /* append to last element id (0xff) */
			break;
		}

		p = (u8 *)pie;
		ielen = pie->len;
		i += (pie->len + 2);
	}

	/* Backup remainder IE */
	if (p != NULL && ielen > 0) {
		ielen += 2;
		premainder_ie = p + ielen;
		ie_offset = (sint)(p - ies);
		remainder_ielen = pnetwork->IELength - ie_offset - ielen;

		if (bmatch)
			dst_ie = p;
		else
			dst_ie = (p + ielen);
	}

	if (dst_ie == NULL)
		return;

	if (remainder_ielen > 0) {
		pbackup_remainder_ie = rtw_malloc(remainder_ielen);
		if (pbackup_remainder_ie && premainder_ie)
			_rtw_memcpy(pbackup_remainder_ie, premainder_ie, remainder_ielen);
	}

	*dst_ie++ = WLAN_EID_EXTENSION;
	*dst_ie++ = len;

	_rtw_memcpy(dst_ie, data, len);
	dst_ie += len;

	/* Append remainder IE */
	if (pbackup_remainder_ie) {
		_rtw_memcpy(dst_ie, pbackup_remainder_ie, remainder_ielen);

		rtw_mfree(pbackup_remainder_ie, remainder_ielen);
	}

	offset = (uint)(dst_ie - ies);
	pnetwork->IELength = offset + remainder_ielen;
}

void rtw_remove_bcn_ie_ex(_adapter *padapter, WLAN_BSSID_EX *pnetwork, u8 index, u8* pindex_ex, u8 index_ex_len)
{
	u8 *p, *dst_ie = NULL, *premainder_ie = NULL, *pbackup_remainder_ie = NULL;
	uint offset, ielen, ie_offset, remainder_ielen = 0;
	u8	*pie = pnetwork->IEs;

	p = rtw_get_ie_ex(pie + _FIXED_IE_LENGTH_, pnetwork->IELength - _FIXED_IE_LENGTH_, index, pindex_ex, index_ex_len, NULL, &ielen);
	if (p != NULL && ielen > 0) {
		premainder_ie = p + ielen;

		ie_offset = (sint)(p - pie);

		remainder_ielen = pnetwork->IELength - ie_offset - ielen;

		dst_ie = p;
	} else
		return;

	if (remainder_ielen > 0) {
		pbackup_remainder_ie = rtw_malloc(remainder_ielen);
		if (pbackup_remainder_ie && premainder_ie)
			_rtw_memcpy(pbackup_remainder_ie, premainder_ie, remainder_ielen);
	}

	/* copy remainder IE */
	if (pbackup_remainder_ie) {
		_rtw_memcpy(dst_ie, pbackup_remainder_ie, remainder_ielen);

		rtw_mfree(pbackup_remainder_ie, remainder_ielen);
	}

	offset = (uint)(dst_ie - pie);
	pnetwork->IELength = offset + remainder_ielen;
}

u8 chk_sta_is_alive(struct sta_info *psta);
u8 chk_sta_is_alive(struct sta_info *psta)
{
	u8 ret = _FALSE;
#ifdef DBG_EXPIRATION_CHK
	RTW_INFO("sta:"MAC_FMT", rssi:%d, rx:"STA_PKTS_FMT", expire_to:%u, %s%ssq_len:%u\n"
		 , MAC_ARG(psta->phl_sta->mac_addr)
		 , 0 /* TODO: psta->phl_sta->hal_sta->rssi_stat.rssi */
		 /* , STA_RX_PKTS_ARG(psta) */
		 , STA_RX_PKTS_DIFF_ARG(psta)
		 , psta->expire_to
		 , psta->state & WIFI_SLEEP_STATE ? "PS, " : ""
		 , psta->state & WIFI_STA_ALIVE_CHK_STATE ? "SAC, " : ""
		 , psta->sleepq_len
		);
#endif

	/* if(sta_last_rx_pkts(psta) == sta_rx_pkts(psta)) */
	if ((psta->sta_stats.last_rx_data_pkts + psta->sta_stats.last_rx_ctrl_pkts) == (psta->sta_stats.rx_data_pkts + psta->sta_stats.rx_ctrl_pkts)) {
#if 0
		if (psta->state & WIFI_SLEEP_STATE)
			ret = _TRUE;
#endif
	} else
		ret = _TRUE;

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(psta->padapter)) {
		u8 bcn_alive, hwmp_alive;

		hwmp_alive = (psta->sta_stats.rx_hwmp_pkts !=
			      psta->sta_stats.last_rx_hwmp_pkts);
		bcn_alive = (psta->sta_stats.rx_beacon_pkts != 
			     psta->sta_stats.last_rx_beacon_pkts);
		/* The reference for nexthop_lookup */
		psta->alive = ret || hwmp_alive || bcn_alive;
		/* The reference for expire_timeout_chk */
		/* Exclude bcn_alive to avoid a misjudge condition
		   that a peer unexpectedly leave and restart quickly*/
		ret = ret || hwmp_alive;
	}
#endif

	sta_update_last_rx_pkts(psta);

	return ret;
}

/**
 * issue_aka_chk_frame - issue active keep alive check frame
 *	aka = active keep alive
 */
#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
static int issue_aka_chk_frame(_adapter *adapter, struct sta_info *psta)
{
	int ret = _FAIL;
	u8 *target_addr = psta->phl_sta->mac_addr;

	if (MLME_IS_AP(adapter)) {
		/* issue null data to check sta alive */
		if (psta->state & WIFI_SLEEP_STATE)
			ret = issue_nulldata(adapter,
					psta->padapter_link,
					target_addr, 0, 1, 50);
		else
			ret = issue_nulldata(adapter,
					psta->padapter_link,
					target_addr, 0, 3, 50);
	}

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter)) {
		struct rtw_mesh_path *mpath;

		rtw_rcu_read_lock();
		mpath = rtw_mesh_path_lookup(adapter, target_addr);
		if (!mpath) {
			mpath = rtw_mesh_path_add(adapter, target_addr);
			if (IS_ERR(mpath)) {
				rtw_rcu_read_unlock();
				RTW_ERR(FUNC_ADPT_FMT" rtw_mesh_path_add for "MAC_FMT" fail.\n",
					FUNC_ADPT_ARG(adapter), MAC_ARG(target_addr));
				return _FAIL;
			}
		}
		if (mpath->flags & RTW_MESH_PATH_ACTIVE)
			ret = _SUCCESS;
		else {
			u8 flags = RTW_PREQ_Q_F_START | RTW_PREQ_Q_F_PEER_AKA;
			/* issue PREQ to check peer alive */
			rtw_mesh_queue_preq(mpath, flags);
			ret = _FALSE;
		}
		rtw_rcu_read_unlock();
	}
#endif
	return ret;
}
#endif

void expire_timeout_chk(_adapter *padapter)
{
	_list	*phead, *plist;
	u8 updated = _FALSE;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	u8 chk_alive_num = 0;
	char chk_alive_list[NUM_STA];
	int i;
	int stainfo_offset;
	u8 flush_num = 0;
	char flush_list[NUM_STA]={0};
	#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
	#endif
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
#if defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK)
	u8 switch_channel_by_drv = _TRUE;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_chan_def hw_chdef = {0};
	struct rtw_chan_def union_chdef = {0};
	struct rtw_chan_def bk_chdef = {0};
#endif
	char del_asoc_list[NUM_STA];

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter)
		&& check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE)
	) {
		struct rtw_mesh_cfg *mcfg = &padapter->mesh_cfg;

		rtw_mesh_path_expire(padapter);

		/* TBD: up layer timeout mechanism */
		/* if (!mcfg->plink_timeout)
			return; */
#ifndef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
		return;
#endif
	}
#endif

#ifdef CONFIG_RTW_WDS
	rtw_wds_path_expire(padapter);
#endif

	_rtw_spinlock_bh(&pstapriv->auth_list_lock);

	phead = &pstapriv->auth_list;
	plist = get_next(phead);

	/* check auth_queue */
#ifdef DBG_EXPIRATION_CHK
	if (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		RTW_INFO(FUNC_ADPT_FMT" auth_list, cnt:%u\n"
			, FUNC_ADPT_ARG(padapter), pstapriv->auth_list_cnt);
	}
#endif
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, auth_list);

		plist = get_next(plist);


#ifdef CONFIG_ATMEL_RC_PATCH
		if (_rtw_memcmp((void *)(pstapriv->atmel_rc_pattern), (void *)(psta->phl_sta->mac_addr), ETH_ALEN) == _TRUE)
			continue;
		if (psta->flag_atmel_rc)
			continue;
#endif
		if (psta->expire_to > 0) {
			psta->expire_to--;
			if (psta->expire_to == 0) {
				stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
				if (stainfo_offset_valid(stainfo_offset))
					flush_list[flush_num++] = stainfo_offset;
				else
					rtw_warn_on(1);
			}
		}

	}

	_rtw_spinunlock_bh(&pstapriv->auth_list_lock);
	for (i = 0; i < flush_num; i++) {
		psta = rtw_get_stainfo_by_offset(pstapriv, flush_list[i]);
		RTW_INFO(FUNC_ADPT_FMT" auth expire "MAC_FMT"\n"
			, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr));
		rtw_free_mld_stainfo(padapter, psta->phl_sta->mld);
		psta = NULL;
	}

	#ifdef CONFIG_P2P
	if(rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO) && padapter->registrypriv.p2p_go_skip_keep_alive == _TRUE)
		return;
	#endif

	_rtw_spinlock_bh(&pstapriv->asoc_list_lock);

	phead = &pstapriv->asoc_list;
	plist = get_next(phead);

	/* check asoc_queue */
#ifdef DBG_EXPIRATION_CHK
	if (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		RTW_INFO(FUNC_ADPT_FMT" asoc_list, cnt:%u\n"
			, FUNC_ADPT_ARG(padapter), pstapriv->asoc_list_cnt);
	}
#endif
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);
#ifdef CONFIG_ATMEL_RC_PATCH
		RTW_INFO("%s:%d  psta=%p, %02x,%02x||%02x,%02x  \n\n", __func__,  __LINE__,
			psta, pstapriv->atmel_rc_pattern[0], pstapriv->atmel_rc_pattern[5], psta->phl_sta->mac_addr[0], psta->phl_sta->mac_addr[5]);
		if (_rtw_memcmp((void *)pstapriv->atmel_rc_pattern, (void *)(psta->phl_sta->mac_addr), ETH_ALEN) == _TRUE)
			continue;
		if (psta->flag_atmel_rc)
			continue;
		RTW_INFO("%s: debug line:%d\n", __func__, __LINE__);
#endif
#ifdef CONFIG_AUTO_AP_MODE
		if (psta->isrc)
			continue;
#endif
		if (chk_sta_is_alive(psta) || !psta->expire_to) {
			psta->expire_to = pstapriv->expire_to;
			psta->keep_alive_trycnt = 0;
			#if !defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && defined(CONFIG_80211N_HT)
			psta->under_exist_checking = 0;
			#endif
		} else
			psta->expire_to--;

#if !defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && defined(CONFIG_80211N_HT)
		if ((psta->flags & WLAN_STA_HT
#if CONFIG_IEEE80211_BAND_6GHZ
		    || psta->phl_sta->chandef.band == BAND_ON_6G
#endif
		    ) && (psta->ampdu_priv.agg_enable_bitmap || psta->under_exist_checking)) {
			/* check sta by delba(addba) for 11n STA */
			/* ToDo: use CCX report to check for all STAs */
			/* RTW_INFO("asoc check by DELBA/ADDBA! (pstapriv->expire_to=%d s)(psta->expire_to=%d s), [%02x, %d]\n", pstapriv->expire_to*2, psta->expire_to*2, psta->ampdu_priv.agg_enable_bitmap, psta->under_exist_checking); */

			if (psta->expire_to <= (pstapriv->expire_to - 50)) {
				RTW_INFO("asoc expire by DELBA/ADDBA! (%d s)\n", (pstapriv->expire_to - psta->expire_to) * 2);
				psta->under_exist_checking = 0;
				psta->expire_to = 0;
			} else if (psta->expire_to <= (pstapriv->expire_to - 3) && (psta->under_exist_checking == 0)) {
				RTW_INFO("asoc check by DELBA/ADDBA! (%d s)\n", (pstapriv->expire_to - psta->expire_to) * 2);
				psta->under_exist_checking = 1;
				/* tear down TX AMPDU */
				send_delba(padapter, 1, psta->phl_sta->mac_addr);/*  */ /* originator */
				psta->ampdu_priv.agg_enable_bitmap = 0x0;/* reset */
				psta->ampdu_priv.candidate_tid_bitmap = 0x0;/* reset */
			}
		}
#endif /* !defined(CONFIG_ACTIVE_KEEP_ALIVE_CHECK) && defined(CONFIG_80211N_HT) */

		if (psta->expire_to <= 0) {
			if (padapter->registrypriv.wifi_spec == 1) {
				psta->expire_to = pstapriv->expire_to;
				continue;
			}

#ifndef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
#ifdef CONFIG_80211N_HT

#define KEEP_ALIVE_TRYCNT (3)

			if (psta->keep_alive_trycnt > 0 && psta->keep_alive_trycnt <= KEEP_ALIVE_TRYCNT) {
				if (psta->state & WIFI_STA_ALIVE_CHK_STATE)
					psta->state ^= WIFI_STA_ALIVE_CHK_STATE;
				else
					psta->keep_alive_trycnt = 0;

			} else if ((psta->keep_alive_trycnt > KEEP_ALIVE_TRYCNT) && !(psta->state & WIFI_STA_ALIVE_CHK_STATE))
				psta->keep_alive_trycnt = 0;
			if ((psta->htpriv.ht_option == _TRUE
#if CONFIG_IEEE80211_BAND_6GHZ
			    || psta->phl_sta->chandef.band == BAND_ON_6G
#endif
			    ) && (psta->ampdu_priv.ampdu_enable == _TRUE)) {
				uint priority = 1; /* test using BK */
				u8 issued = 0;

				/* issued = (psta->ampdu_priv.agg_enable_bitmap>>priority)&0x1; */
				issued |= (psta->ampdu_priv.candidate_tid_bitmap >> priority) & 0x1;

				if (0 == issued) {
					if (!(psta->state & WIFI_STA_ALIVE_CHK_STATE)) {
						psta->ampdu_priv.candidate_tid_bitmap |= BIT((u8)priority);

						if (psta->state & WIFI_SLEEP_STATE)
							psta->expire_to = 2; /* 2x2=4 sec */
						else
							psta->expire_to = 1; /* 2 sec */

						psta->state |= WIFI_STA_ALIVE_CHK_STATE;

						/* add_ba_hdl(padapter, (u8*)paddbareq_parm); */

						RTW_INFO("issue addba_req to check if sta alive, keep_alive_trycnt=%d\n", psta->keep_alive_trycnt);

						issue_addba_req(padapter, psta->phl_sta->mac_addr, (u8)priority);

						_set_timer(&psta->addba_retry_timer, ADDBA_TO);

						psta->keep_alive_trycnt++;

						continue;
					}
				}
			}
			if (psta->keep_alive_trycnt > 0 && psta->state & WIFI_STA_ALIVE_CHK_STATE) {
				psta->keep_alive_trycnt = 0;
				psta->state ^= WIFI_STA_ALIVE_CHK_STATE;
				RTW_INFO("change to another methods to check alive if staion is at ps mode\n");
			}

#endif /* CONFIG_80211N_HT */
#endif /* CONFIG_ACTIVE_KEEP_ALIVE_CHECK	 */
			if (psta->state & WIFI_SLEEP_STATE) {
				if (!(psta->state & WIFI_STA_ALIVE_CHK_STATE)) {
					/* to check if alive by another methods if staion is at ps mode.					 */
					psta->expire_to = pstapriv->expire_to;
					psta->state |= WIFI_STA_ALIVE_CHK_STATE;

					/* RTW_INFO("alive chk, sta:" MAC_FMT " is at ps mode!\n", MAC_ARG(psta->phl_sta->mac_addr)); */

					/* to update bcn with tim_bitmap for this station */
					rtw_tim_map_set(padapter, pstapriv->tim_bitmap, psta->phl_sta->aid);
					rtw_update_beacon(padapter,
							padapter_link,
							_TIM_IE_, NULL, _TRUE, 0);

					if (!padapter->mlmeextpriv.active_keep_alive_check)
						continue;
				}
			}

			{
				int stainfo_offset;

				stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
				if (stainfo_offset_valid(stainfo_offset))
					chk_alive_list[chk_alive_num++] = stainfo_offset;
				continue;
			}
		} else {
			/* TODO: Aging mechanism to digest frames in sleep_q to avoid running out of xmitframe */
			if (psta->sleepq_len > (NR_XMITFRAME / pstapriv->asoc_list_cnt)
			    && padapter->xmitpriv.free_xmitframe_cnt < ((NR_XMITFRAME / pstapriv->asoc_list_cnt) / 2)
			   ) {
				RTW_INFO(FUNC_ADPT_FMT" sta:"MAC_FMT", sleepq_len:%u, free_xmitframe_cnt:%u, asoc_list_cnt:%u, clear sleep_q\n"
					, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr)
					, psta->sleepq_len, padapter->xmitpriv.free_xmitframe_cnt, pstapriv->asoc_list_cnt);
				wakeup_sta_to_xmit(padapter, psta);
			}
		}
	}

	_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

	if (chk_alive_num) {
		_rtw_memset(del_asoc_list, NUM_STA, NUM_STA);

		#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
		if (padapter->mlmeextpriv.active_keep_alive_check) {
			if (rtw_phl_mr_get_chandef(dvobj->phl, padapter->phl_role,
					padapter_link->wrlink,
					&union_chdef)
				!= RTW_PHL_STATUS_SUCCESS) {
				RTW_ERR("fun:%s line:%d get chandef failed\n", __FUNCTION__, __LINE__);
				rtw_warn_on(1);
			}

			if (pmlmeext->chandef.chan != union_chdef.chan)
				switch_channel_by_drv = _FALSE;

			/* switch to correct channel of current network before issue keep-alive frames */
			rtw_get_oper_chdef(padapter, padapter_link, &hw_chdef);
			if (switch_channel_by_drv == _TRUE && hw_chdef.chan != pmlmeext->chandef.chan) {
				_rtw_memcpy(&bk_chdef, &hw_chdef, sizeof(struct rtw_chan_def));
				set_channel_bwmode(padapter, padapter_link,
						union_chdef.chan, union_chdef.offset, union_chdef.bw, _FALSE);
			}
		}
		#endif /* CONFIG_ACTIVE_KEEP_ALIVE_CHECK */

		/* check loop */
		for (i = 0; i < chk_alive_num; i++) {
			#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
			int ret = _FAIL;
			#endif

			psta = rtw_get_stainfo_by_offset(pstapriv, chk_alive_list[i]);

			#ifdef CONFIG_ATMEL_RC_PATCH
			if (_rtw_memcmp(pstapriv->atmel_rc_pattern, psta->phl_sta->mac_addr, ETH_ALEN) == _TRUE)
				continue;
			if (psta->flag_atmel_rc)
				continue;
			#endif

			if (!(psta->state & WIFI_ASOC_STATE))
				continue;

			#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
			if (padapter->mlmeextpriv.active_keep_alive_check) {
				/* issue active keep alive frame to check */
				ret = issue_aka_chk_frame(padapter, psta);

				psta->keep_alive_trycnt++;
				if (ret == _SUCCESS) {
					RTW_INFO(FUNC_ADPT_FMT" asoc check, "MAC_FMT" is alive\n"
						, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr));
					psta->expire_to = pstapriv->expire_to;
					psta->keep_alive_trycnt = 0;
					continue;
				} else if (psta->keep_alive_trycnt <= 3) {
					RTW_INFO(FUNC_ADPT_FMT" asoc check, "MAC_FMT" keep_alive_trycnt=%d\n"
						, FUNC_ADPT_ARG(padapter) , MAC_ARG(psta->phl_sta->mac_addr), psta->keep_alive_trycnt);
					psta->expire_to = 1;
					continue;
				}
			}
			#endif /* CONFIG_ACTIVE_KEEP_ALIVE_CHECK */

			psta->keep_alive_trycnt = 0;
			del_asoc_list[i] = chk_alive_list[i];
			_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
			if (rtw_is_list_empty(&psta->asoc_list) == _FALSE) {
				rtw_list_delete(&psta->asoc_list);
				pstapriv->asoc_list_cnt--;
				#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
				if (psta->tbtx_enable)
					pstapriv->tbtx_asoc_list_cnt--;
				#endif				
				STA_SET_MESH_PLINK(psta, NULL);
			}
			_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);
		}

		/* delete loop */
		for (i = 0; i < chk_alive_num; i++) {
			u8 sta_addr[ETH_ALEN];

			if (del_asoc_list[i] >= NUM_STA)
				continue;

			psta = rtw_get_stainfo_by_offset(pstapriv, del_asoc_list[i]);
			_rtw_memcpy(sta_addr, psta->phl_sta->mac_addr, ETH_ALEN);

			RTW_INFO(FUNC_ADPT_FMT" asoc expire "MAC_FMT", state=0x%x\n"
				, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr), psta->state);
			/* ToDo CONFIG_RTW_MLD: call only once for each MLD */
			updated |= ap_free_sta(padapter, psta, _FALSE, WLAN_REASON_DEAUTH_LEAVING, _FALSE, _FALSE);
			#ifdef CONFIG_RTW_MESH
			if (MLME_IS_MESH(padapter))
				rtw_mesh_expire_peer(padapter, sta_addr);
			#endif
		}

		#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
		if (padapter->mlmeextpriv.active_keep_alive_check) {
			/* back to the original operation channel */
			if (switch_channel_by_drv == _TRUE && bk_chdef.chan > 0)
				set_channel_bwmode(padapter, padapter_link,
						bk_chdef.chan, bk_chdef.offset, bk_chdef.bw, _FALSE);
		}
		#endif
	}

	associated_clients_update(padapter, updated, STA_INFO_UPDATE_ALL);
}

#ifdef CONFIG_BMC_TX_RATE_SELECT
u8 rtw_ap_find_mini_tx_rate(_adapter *adapter)
{
	_list	*phead, *plist;
	u8 mini_tx_rate = DESC_RATEVHTSS4MCS9, sta_tx_rate;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &adapter->stapriv;

	_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		sta_tx_rate = psta->phl_sta->ra_info.curr_tx_rate & 0x7F;
		if (sta_tx_rate < mini_tx_rate)
			mini_tx_rate = sta_tx_rate;
	}
	_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

	return mini_tx_rate;
}

u8 rtw_ap_find_bmc_rate(_adapter *adapter, struct _ADAPTER_LINK *adapter_link, u8 tx_rate)
{
	u8 tx_ini_rate = DESC_RATE6M;

	switch (tx_rate) {
	case DESC_RATEVHTSS3MCS9:
	case DESC_RATEVHTSS3MCS8:
	case DESC_RATEVHTSS3MCS7:
	case DESC_RATEVHTSS3MCS6:
	case DESC_RATEVHTSS3MCS5:
	case DESC_RATEVHTSS3MCS4:
	case DESC_RATEVHTSS3MCS3:
	case DESC_RATEVHTSS2MCS9:
	case DESC_RATEVHTSS2MCS8:
	case DESC_RATEVHTSS2MCS7:
	case DESC_RATEVHTSS2MCS6:
	case DESC_RATEVHTSS2MCS5:
	case DESC_RATEVHTSS2MCS4:
	case DESC_RATEVHTSS2MCS3:
	case DESC_RATEVHTSS1MCS9:
	case DESC_RATEVHTSS1MCS8:
	case DESC_RATEVHTSS1MCS7:
	case DESC_RATEVHTSS1MCS6:
	case DESC_RATEVHTSS1MCS5:
	case DESC_RATEVHTSS1MCS4:
	case DESC_RATEVHTSS1MCS3:
	case DESC_RATEMCS15:
	case DESC_RATEMCS14:
	case DESC_RATEMCS13:
	case DESC_RATEMCS12:
	case DESC_RATEMCS11:
	case DESC_RATEMCS7:
	case DESC_RATEMCS6:
	case DESC_RATEMCS5:
	case DESC_RATEMCS4:
	case DESC_RATEMCS3:
	case DESC_RATE54M:
	case DESC_RATE48M:
	case DESC_RATE36M:
	case DESC_RATE24M:
		tx_ini_rate = DESC_RATE24M;
		break;
	case DESC_RATEVHTSS3MCS2:
	case DESC_RATEVHTSS3MCS1:
	case DESC_RATEVHTSS2MCS2:
	case DESC_RATEVHTSS2MCS1:
	case DESC_RATEVHTSS1MCS2:
	case DESC_RATEVHTSS1MCS1:
	case DESC_RATEMCS10:
	case DESC_RATEMCS9:
	case DESC_RATEMCS2:
	case DESC_RATEMCS1:
	case DESC_RATE18M:
	case DESC_RATE12M:
		tx_ini_rate = DESC_RATE12M;
		break;
	case DESC_RATEVHTSS3MCS0:
	case DESC_RATEVHTSS2MCS0:
	case DESC_RATEVHTSS1MCS0:
	case DESC_RATEMCS8:
	case DESC_RATEMCS0:
	case DESC_RATE9M:
	case DESC_RATE6M:
		tx_ini_rate = DESC_RATE6M;
		break;
	case DESC_RATE11M:
	case DESC_RATE5_5M:
	case DESC_RATE2M:
	case DESC_RATE1M:
		tx_ini_rate = DESC_RATE1M;
		break;
	default:
		tx_ini_rate = DESC_RATE6M;
		break;
	}

	if (WIFI_ROLE_LINK_IS_ON_5G(adapter_link))
		if (tx_ini_rate < DESC_RATE6M)
			tx_ini_rate = DESC_RATE6M;

	return tx_ini_rate;
}

void rtw_update_bmc_sta_tx_rate(_adapter *adapter)
{
	struct sta_info *psta = NULL;
	u8 tx_rate;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);

	psta = rtw_get_bcmc_stainfo(adapter, adapter_link);
	if (psta == NULL) {
		RTW_ERR(ADPT_FMT "could not get bmc_sta !!\n", ADPT_ARG(adapter));
		return;
	}

	if (adapter->bmc_tx_rate != MGN_UNKNOWN) {
		psta->init_rate = adapter->bmc_tx_rate;
		goto _exit;
	}

	if (adapter->stapriv.asoc_sta_count <= 2)
		goto _exit;

	tx_rate = rtw_ap_find_mini_tx_rate(adapter);
	#ifdef CONFIG_BMC_TX_LOW_RATE
	tx_rate = rtw_ap_find_bmc_rate(adapter, adapter_link, tx_rate);
	#endif

	psta->init_rate = hwrate_to_mrate(tx_rate);

_exit:
	RTW_INFO(ADPT_FMT" BMC Tx rate - %s\n", ADPT_ARG(adapter), MGN_RATE_STR(psta->init_rate));
}
#endif

void rtw_init_bmc_sta_tx_rate(_adapter *padapter, struct sta_info *psta)
{
/* ToDo: need API to query hal_sta->ra_info.ramask */
#if 0
#ifdef CONFIG_BMC_TX_LOW_RATE
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
#endif
	u8 rate_idx = 0;
	u8 brate_table[] = {MGN_1M, MGN_2M, MGN_5_5M, MGN_11M,
		MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M, MGN_48M, MGN_54M};

	if (!MLME_IS_AP(padapter) && !MLME_IS_MESH(padapter))
		return;

	if (padapter->bmc_tx_rate != MGN_UNKNOWN)
		psta->init_rate = padapter->bmc_tx_rate;
	else {
		#ifdef CONFIG_BMC_TX_LOW_RATE
		if (IsEnableHWOFDM(pmlmeext->cur_wireless_mode) && (psta->phl_sta->ra_info.ramask && 0xFF0))
			rate_idx = get_lowest_rate_idx_ex(psta->phl_sta->ra_info.ramask, 4); /*from basic rate*/
		else
			rate_idx = get_lowest_rate_idx(psta->phl_sta->ra_info.ramask); /*from basic rate*/
		#else
		rate_idx = get_highest_rate_idx(psta->phl_sta->ra_info.ramask); /*from basic rate*/
		#endif
		if (rate_idx < 12)
			psta->init_rate = brate_table[rate_idx];
		else
			psta->init_rate = MGN_1M;
	}
#endif
	RTW_INFO(ADPT_FMT" BMC Init Tx rate - %s\n", ADPT_ARG(padapter), MGN_RATE_STR(psta->init_rate));
}

#if defined(CONFIG_80211N_HT) && defined(CONFIG_BEAMFORMING)
void update_sta_info_apmode_ht_bf_cap(_adapter *padapter, struct sta_info *psta)
{
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct ht_priv	*phtpriv_ap = &pmlmepriv->htpriv;
	struct ht_priv	*phtpriv_sta = &psta->htpriv;

	u8 cur_beamform_cap = 0;

	/*Config Tx beamforming setting*/
	if (TEST_FLAG(phtpriv_ap->beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE) &&
		GET_HT_CAP_TXBF_EXPLICIT_COMP_STEERING_CAP((u8 *)(&phtpriv_sta->ht_cap))) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE);
		/*Shift to BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP*/
		SET_FLAG(cur_beamform_cap, GET_HT_CAP_TXBF_CHNL_ESTIMATION_NUM_ANTENNAS((u8 *)(&phtpriv_sta->ht_cap)) << 6);
	}

	if (TEST_FLAG(phtpriv_ap->beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE) &&
		GET_HT_CAP_TXBF_EXPLICIT_COMP_FEEDBACK_CAP((u8 *)(&phtpriv_sta->ht_cap))) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE);
		/*Shift to BEAMFORMING_HT_BEAMFORMER_STEER_NUM*/
		SET_FLAG(cur_beamform_cap, GET_HT_CAP_TXBF_COMP_STEERING_NUM_ANTENNAS((u8 *)(&phtpriv_sta->ht_cap)) << 4);
	}
	if (cur_beamform_cap)
		RTW_INFO("Client STA(%d) HT Beamforming Cap = 0x%02X\n", psta->phl_sta->aid, cur_beamform_cap);

	phtpriv_sta->beamform_cap = cur_beamform_cap;
}
#endif /*CONFIG_80211N_HT && CONFIG_BEAMFORMING*/

/* notes:
 * AID: 1~MAX for sta and 0 for bc/mc in ap/adhoc mode  */
void update_sta_info_apmode(_adapter *padapter, struct sta_info *psta)
{
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct rtw_wifi_role_t *wrole = padapter->phl_role;
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct protocol_cap_t *protocol_cap = &(padapter_link->wrlink->protocol_cap);
#ifdef CONFIG_80211N_HT
	struct ht_priv	*phtpriv_ap = &pmlmepriv->htpriv;
	struct ht_priv	*phtpriv_sta = &psta->htpriv;
	struct ampdu_priv *ampdu_priv_ap = &pmlmepriv->ampdu_priv;
	struct ampdu_priv *ampdu_priv_sta = &psta->ampdu_priv;
#endif /* CONFIG_80211N_HT */
	u8	cur_ldpc_cap = 0, cur_stbc_cap = 0;
	/* set intf_tag to if1 */
	/* psta->intf_tag = 0; */

	RTW_INFO("%s\n", __FUNCTION__);

	/*alloc macid when call rtw_alloc_stainfo(),release macid when call rtw_free_stainfo()*/

	if (!MLME_IS_MESH(padapter) && psecuritypriv->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X)
		psta->ieee8021x_blocked = _TRUE;
	else
		psta->ieee8021x_blocked = _FALSE;


	/* update sta's cap */

	psta->phl_sta->chandef.chan = pmlmeext->chandef.chan;
	psta->phl_sta->chandef.band = pmlmeext->chandef.band;

	/* ERP */
	VCS_update(padapter, psta);
#ifdef CONFIG_80211N_HT
	/* HT related cap */
	if (phtpriv_sta->ht_option) {
		/* check if sta supports rx ampdu */
		ampdu_priv_sta->ampdu_enable = ampdu_priv_ap->ampdu_enable;

		ampdu_priv_sta->rx_ampdu_min_spacing = (phtpriv_sta->ht_cap.ampdu_params_info & IEEE80211_HT_CAP_AMPDU_DENSITY) >> 2;

		/* bwmode */
		if ((phtpriv_sta->ht_cap.cap_info & phtpriv_ap->ht_cap.cap_info) & cpu_to_le16(IEEE80211_HT_CAP_SUP_WIDTH))
			psta->phl_sta->chandef.bw = CHANNEL_WIDTH_40;
		else
			psta->phl_sta->chandef.bw = CHANNEL_WIDTH_20;

		if (phtpriv_sta->op_present
			&& !GET_HT_OP_ELE_STA_CHL_WIDTH(phtpriv_sta->ht_op))
			psta->phl_sta->chandef.bw = CHANNEL_WIDTH_20;

		if (psta->ht_40mhz_intolerant)
			psta->phl_sta->chandef.bw = CHANNEL_WIDTH_20;

		if (pmlmeext->chandef.bw < psta->phl_sta->chandef.bw)
			psta->phl_sta->chandef.bw = pmlmeext->chandef.bw;

		phtpriv_sta->ch_offset = pmlmeext->chandef.offset;


		/* check if sta support s Short GI 20M */
		if ((phtpriv_sta->ht_cap.cap_info & phtpriv_ap->ht_cap.cap_info) & cpu_to_le16(IEEE80211_HT_CAP_SGI_20))
			phtpriv_sta->sgi_20m = _TRUE;

		/* check if sta support s Short GI 40M */
		if ((phtpriv_sta->ht_cap.cap_info & phtpriv_ap->ht_cap.cap_info) & cpu_to_le16(IEEE80211_HT_CAP_SGI_40)) {
			if (psta->phl_sta->chandef.bw == CHANNEL_WIDTH_40) /* according to psta->bw_mode */
				phtpriv_sta->sgi_40m = _TRUE;
			else
				phtpriv_sta->sgi_40m = _FALSE;
		}

		psta->qos_option = _TRUE;

		/* B0 Config LDPC Coding Capability */
		if (TEST_FLAG(phtpriv_ap->ldpc_cap, LDPC_HT_ENABLE_TX) &&
		    GET_HT_CAP_ELE_LDPC_CAP((u8 *)(&phtpriv_sta->ht_cap))) {
			SET_FLAG(cur_ldpc_cap, (LDPC_HT_ENABLE_TX | LDPC_HT_CAP_TX));
			RTW_INFO("Enable HT Tx LDPC for STA(%d)\n", psta->phl_sta->aid);
		}

		/* B7 B8 B9 Config STBC setting */
		if (TEST_FLAG(phtpriv_ap->stbc_cap, STBC_HT_ENABLE_TX) &&
		    GET_HT_CAP_ELE_RX_STBC((u8 *)(&phtpriv_sta->ht_cap))) {
			SET_FLAG(cur_stbc_cap, (STBC_HT_ENABLE_TX | STBC_HT_CAP_TX));
			RTW_INFO("Enable HT Tx STBC for STA(%d)\n", psta->phl_sta->aid);
		}

		psta->phl_sta->asoc_cap.stbc_ht_tx =
			GET_HT_CAP_ELE_TX_STBC((u8 *)(&phtpriv_sta->ht_cap));
		psta->phl_sta->asoc_cap.stbc_ht_rx =
			protocol_cap->stbc_ht_tx ?
			GET_HT_CAP_ELE_RX_STBC((u8 *)(&phtpriv_sta->ht_cap)) : 0;

		#ifdef CONFIG_BEAMFORMING
		update_sta_info_apmode_ht_bf_cap(padapter, psta);
		#endif
	} else {
		ampdu_priv_sta->ampdu_enable = _FALSE;

		phtpriv_sta->sgi_20m = _FALSE;
		phtpriv_sta->sgi_40m = _FALSE;
		psta->phl_sta->chandef.bw = CHANNEL_WIDTH_20;
		phtpriv_sta->ch_offset = CHAN_OFFSET_NO_EXT;
	}

	phtpriv_sta->ldpc_cap = cur_ldpc_cap;
	phtpriv_sta->stbc_cap = cur_stbc_cap;

	/* Rx AMPDU */
	send_delba(padapter, 0, psta->phl_sta->mac_addr);/* recipient */

	/* TX AMPDU */
	send_delba(padapter, 1, psta->phl_sta->mac_addr);/*  */ /* originator */
	ampdu_priv_sta->agg_enable_bitmap = 0x0;/* reset */
	ampdu_priv_sta->candidate_tid_bitmap = 0x0;/* reset */
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_80211AC_VHT
	update_sta_vht_info_apmode(padapter, psta);
#endif

#ifdef CONFIG_80211AX_HE
	update_sta_he_info_apmode(padapter, psta);
#endif

#if CONFIG_IEEE80211_BAND_6GHZ
	update_sta_6ghz_info_apmode(padapter, psta);
#endif

	psta->phl_sta->chandef.offset = (psta->phl_sta->chandef.bw > CHANNEL_WIDTH_20) ?
		pmlmeext->chandef.offset : CHAN_OFFSET_NO_EXT;

	/* ToDo: need API to inform hal_sta->ra_info.is_support_sgi */
	/* psta->phl_sta->ra_info.is_support_sgi = query_ra_short_GI(psta, rtw_get_tx_bw_mode(padapter, psta)); */
	update_ldpc_stbc_cap(psta);

	/* todo: init other variables */

	_rtw_memset((void *)&psta->sta_stats, 0, sizeof(struct stainfo_stats));

	_rtw_spinlock_bh(&psta->lock);

	/* Check encryption */
	if (!MLME_IS_MESH(padapter) && psecuritypriv->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X)
		psta->state |= WIFI_UNDER_KEY_HANDSHAKE;

	psta->state |= WIFI_ASOC_STATE;

	_rtw_spinunlock_bh(&psta->lock);
}

static void update_ap_info(_adapter *padapter, struct sta_info *psta)
{
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	WLAN_BSSID_EX *pnetwork = (WLAN_BSSID_EX *)&pmlmepriv->cur_network.network;
#ifdef CONFIG_80211N_HT
	struct ht_priv	*phtpriv_ap = &pmlmepriv->htpriv;
	struct ampdu_priv *ampdu_priv_ap = &pmlmepriv->ampdu_priv;
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter))
		psta->qos_option = 1;
#endif

	psta->phl_sta->wmode = pmlmeext->cur_wireless_mode;

	psta->bssratelen = rtw_get_rateset_len(pnetwork->SupportedRates);
	_rtw_memcpy(psta->bssrateset, pnetwork->SupportedRates, psta->bssratelen);

#ifdef CONFIG_80211N_HT
	/* HT related cap */
	if (phtpriv_ap->ht_option) {
		/* check if sta supports rx ampdu */
		/* ampdu_priv_ap->ampdu_enable = ampdu_priv_ap->ampdu_enable; */

		/* check if sta support s Short GI 20M */
		if ((phtpriv_ap->ht_cap.cap_info) & cpu_to_le16(IEEE80211_HT_CAP_SGI_20))
			phtpriv_ap->sgi_20m = _TRUE;
		/* check if sta support s Short GI 40M */
		if ((phtpriv_ap->ht_cap.cap_info) & cpu_to_le16(IEEE80211_HT_CAP_SGI_40))
			phtpriv_ap->sgi_40m = _TRUE;

		psta->qos_option = _TRUE;
	} else {
		ampdu_priv_ap->ampdu_enable = _FALSE;

		phtpriv_ap->sgi_20m = _FALSE;
		phtpriv_ap->sgi_40m = _FALSE;
	}

	_rtw_memcpy(&psta->phl_sta->chandef, &pmlmeext->chandef, sizeof(struct rtw_chan_def));
	phtpriv_ap->ch_offset = pmlmeext->chandef.offset;

	ampdu_priv_ap->agg_enable_bitmap = 0x0;/* reset */
	ampdu_priv_ap->candidate_tid_bitmap = 0x0;/* reset */

	_rtw_memcpy(&psta->htpriv, &pmlmepriv->htpriv, sizeof(struct ht_priv));

#ifdef CONFIG_80211AC_VHT
	_rtw_memcpy(&psta->vhtpriv, &pmlmepriv->vhtpriv, sizeof(struct vht_priv));

#ifdef CONFIG_80211AX_HE
	_rtw_memcpy(&psta->hepriv, &pmlmepriv->hepriv, sizeof(struct he_priv));
#endif /* CONFIG_80211AX_HE */

#endif /* CONFIG_80211AC_VHT */

#endif /* CONFIG_80211N_HT */

	_rtw_spinlock_bh(&psta->lock);
	psta->state |= WIFI_AP_STATE; /* Aries, add,fix bug of flush_cam_entry at STOP AP mode , 0724 */
	psta->state |= WIFI_ASOC_STATE;
	_rtw_spinunlock_bh(&psta->lock);

	rtw_init_bmc_sta_tx_rate(padapter, psta);
}

void rtw_ap_set_edca(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, enum rtw_ac ac, u32 parm)
{
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta = rtw_get_stainfo(pstapriv, padapter_link->mac_addr);

	if (psta && psta->phl_sta) {
		psta->phl_sta->asoc_cap.edca[ac].ac = ac;
		psta->phl_sta->asoc_cap.edca[ac].param = parm;
	}
	rtw_hw_set_edca(padapter, padapter_link, ac, parm);
}

static void rtw_set_hw_wmm_param(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	u8	AIFS, ECWMin, ECWMax, aSifsTime;
	u8	acm_mask;
	u16	TXOP;
	u32	acParm, i;
	u32	edca[4], inx[4];
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;

	acm_mask = 0;
#ifdef CONFIG_80211N_HT
	if (pregpriv->ht_enable &&
		(WIFI_ROLE_LINK_IS_ON_5G(padapter_link) || (pmlmeext->cur_wireless_mode & WLAN_MD_11N)))
		aSifsTime = 16;
	else
#endif /* CONFIG_80211N_HT */
		aSifsTime = 10;

	if (pmlmeinfo->WMM_enable == 0) {
		padapter->mlmepriv.acm_mask = 0;

		AIFS = aSifsTime + (2 * pmlmeinfo->slotTime);

		if (pmlmeext->cur_wireless_mode & (WLAN_MD_11G | WLAN_MD_11A)) {
			ECWMin = 4;
			ECWMax = 10;
		} else if (pmlmeext->cur_wireless_mode & WLAN_MD_11B) {
			ECWMin = 5;
			ECWMax = 10;
		} else {
			ECWMin = 4;
			ECWMax = 10;
		}

		TXOP = 0;
		acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
		rtw_ap_set_edca(padapter, padapter_link, 0, acParm);
		rtw_ap_set_edca(padapter, padapter_link, 1, acParm);
		rtw_ap_set_edca(padapter, padapter_link, 2, acParm);

		ECWMin = 2;
		ECWMax = 3;
		TXOP = 0x2f;
		acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
		rtw_ap_set_edca(padapter, padapter_link, 3, acParm);

	} else {
		edca[0] = edca[1] = edca[2] = edca[3] = 0;

		/*TODO:*/
		acm_mask = 0;
		padapter->mlmepriv.acm_mask = acm_mask;

#if 0
		/* BK */
		/* AIFS = AIFSN * slot time + SIFS - r2t phy delay */
#endif
		AIFS = (7 * pmlmeinfo->slotTime) + aSifsTime;
		ECWMin = 4;
		ECWMax = 10;
		TXOP = 0;
		acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
		rtw_ap_set_edca(padapter, padapter_link, 1, acParm);
		edca[XMIT_BK_QUEUE] = acParm;
		RTW_INFO("WMM(BK): %x\n", acParm);

		/* BE */
		AIFS = (3 * pmlmeinfo->slotTime) + aSifsTime;
		ECWMin = 4;
		ECWMax = 6;
		TXOP = 0;
		acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
		rtw_ap_set_edca(padapter, padapter_link, 0, acParm);
		edca[XMIT_BE_QUEUE] = acParm;
		RTW_INFO("WMM(BE): %x\n", acParm);

		/* VI */
		AIFS = (1 * pmlmeinfo->slotTime) + aSifsTime;
		ECWMin = 3;
		ECWMax = 4;
		TXOP = 94;
		acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
		rtw_ap_set_edca(padapter, padapter_link, 2, acParm);
		edca[XMIT_VI_QUEUE] = acParm;
		RTW_INFO("WMM(VI): %x\n", acParm);

		/* VO */
		AIFS = (1 * pmlmeinfo->slotTime) + aSifsTime;
		ECWMin = 2;
		ECWMax = 3;
		TXOP = 47;
		acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
		rtw_ap_set_edca(padapter, padapter_link, 3, acParm);
		edca[XMIT_VO_QUEUE] = acParm;
		RTW_INFO("WMM(VO): %x\n", acParm);


		if (padapter->registrypriv.acm_method == 1)
			rtw_hal_set_hwreg(padapter, HW_VAR_ACM_CTRL, (u8 *)(&acm_mask));
		else
			padapter->mlmepriv.acm_mask = acm_mask;

		inx[0] = 0;
		inx[1] = 1;
		inx[2] = 2;
		inx[3] = 3;

		if (padapter->registrypriv.wifi_spec == 1) {
			u32	j, tmp, change_inx = _FALSE;

			/* entry indx: 0->vo, 1->vi, 2->be, 3->bk. */
			for (i = 0 ; i < 4 ; i++) {
				for (j = i + 1 ; j < 4 ; j++) {
					/* compare CW and AIFS */
					if ((edca[j] & 0xFFFF) < (edca[i] & 0xFFFF))
						change_inx = _TRUE;
					else if ((edca[j] & 0xFFFF) == (edca[i] & 0xFFFF)) {
						/* compare TXOP */
						if ((edca[j] >> 16) > (edca[i] >> 16))
							change_inx = _TRUE;
					}

					if (change_inx) {
						tmp = edca[i];
						edca[i] = edca[j];
						edca[j] = tmp;

						tmp = inx[i];
						inx[i] = inx[j];
						inx[j] = tmp;

						change_inx = _FALSE;
					}
				}
			}
		}

		for (i = 0 ; i < 4 ; i++) {
			pxmitpriv->wmm_para_seq[i] = inx[i];
			RTW_INFO("wmm_para_seq(%d): %d\n", i, pxmitpriv->wmm_para_seq[i]);
		}

	}

}
static void rtw_ap_check_scan(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	_list		*plist, *phead;
	u32	delta_time, lifetime;
	struct	wlan_network	*pnetwork = NULL;
	WLAN_BSSID_EX *pbss = NULL;
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	_queue	*queue	= &(pmlmepriv->scanned_queue);
	u8 do_scan = _FALSE;
	u8 reason = RTW_AUTO_SCAN_REASON_UNSPECIFIED;
	struct link_mlme_priv *lmlmepriv = &padapter_link->mlmepriv;

	lifetime = SCANQUEUE_LIFETIME; /* 20 sec */

	_rtw_spinlock_bh(&(pmlmepriv->scanned_queue.lock));
	phead = get_list_head(queue);
	if (rtw_end_of_queue_search(phead, get_next(phead)) == _TRUE)
		if (padapter->registrypriv.wifi_spec) {
			do_scan = _TRUE;
			reason |= RTW_AUTO_SCAN_REASON_2040_BSS;
		}
	_rtw_spinunlock_bh(&(pmlmepriv->scanned_queue.lock));

#if defined(CONFIG_RTW_ACS) && defined(WKARD_ACS)
	if (padapter->registrypriv.acs_auto_scan) {
		do_scan = _TRUE;
		reason |= RTW_AUTO_SCAN_REASON_ACS;
		rtw_acs_start(padapter);
	}
#endif/*CONFIG_RTW_ACS*/

	if (_TRUE == do_scan) {
		RTW_INFO("%s : drv scans by itself and wait_completed\n", __func__);
		rtw_drv_scan_by_self(padapter, reason);
		rtw_scan_abort(padapter, 0);
	}

#if defined(CONFIG_RTW_ACS) && defined(WKARD_ACS)
	if (padapter->registrypriv.acs_auto_scan)
		rtw_acs_stop(padapter);
#endif

	_rtw_spinlock_bh(&(pmlmepriv->scanned_queue.lock));

	phead = get_list_head(queue);
	plist = get_next(phead);

	while (1) {

		if (rtw_end_of_queue_search(phead, plist) == _TRUE)
			break;

		pnetwork = LIST_CONTAINOR(plist, struct wlan_network, list);

		if (rtw_chset_search_ch(adapter_to_chset(padapter), pnetwork->network.Configuration.DSConfig) >= 0
		    && rtw_mlme_band_check(padapter, pnetwork->network.Configuration.DSConfig) == _TRUE
		    && _TRUE == rtw_validate_ssid(&(pnetwork->network.Ssid))) {
			delta_time = (u32) rtw_get_passing_time_ms(pnetwork->last_scanned);

			if (delta_time < lifetime) {

				uint ie_len = 0;
				u8 *pbuf = NULL;
				u8 *ie = NULL;

				pbss = &pnetwork->network;
				ie = pbss->IEs;

				/*check if HT CAP INFO IE exists or not*/
				pbuf = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_CAPABILITY_IE_, &ie_len, (pbss->IELength - _BEACON_IE_OFFSET_));
				if (pbuf == NULL) {
					/* HT CAP INFO IE don't exist, it is b/g mode bss.*/

					if (_FALSE == ATOMIC_READ(&lmlmepriv->olbc))
						ATOMIC_SET(&lmlmepriv->olbc, _TRUE);

					if (_FALSE == ATOMIC_READ(&lmlmepriv->olbc_ht))
						ATOMIC_SET(&lmlmepriv->olbc_ht, _TRUE);
					if (padapter->registrypriv.wifi_spec)
						RTW_INFO("%s: %s is a/b/g ap\n", __func__, pnetwork->network.Ssid.Ssid);
				}
			}
		}

		plist = get_next(plist);

	}

	_rtw_spinunlock_bh(&(pmlmepriv->scanned_queue.lock));
#ifdef CONFIG_80211N_HT
	lmlmepriv->num_sta_no_ht = 0; /* reset to 0 after ap do scanning*/
#endif
}

void rtw_start_bss_hdl_after_chbw_decided(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	WLAN_BSSID_EX *pnetwork = &(adapter_link->mlmepriv.cur_network.network);
	struct sta_info *sta = NULL;

	/* update cur_wireless_mode */
	update_wireless_mode(adapter, adapter_link);

	/* update RRSR and RTS_INIT_RATE register after set channel and bandwidth */
	UpdateBrateTbl(adapter, pnetwork->SupportedRates);
	rtw_hal_set_hwreg(adapter, HW_VAR_BASIC_RATE, pnetwork->SupportedRates);

	/* update capability after cur_wireless_mode updated */
	update_capinfo(adapter, adapter_link, rtw_get_capability(pnetwork));

	/* update AP's sta info */
	sta = rtw_get_stainfo(&adapter->stapriv, pnetwork->MacAddress);
	if (!sta) {
		RTW_INFO(FUNC_ADPT_FMT" !sta for macaddr="MAC_FMT"\n", FUNC_ADPT_ARG(adapter), MAC_ARG(pnetwork->MacAddress));
		rtw_warn_on(1);
		return;
	}

	update_ap_info(adapter, sta);
}

static void _rtw_iface_undersurvey_chk(const char *func, _adapter *adapter)
{
	int i;
	_adapter *iface;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct mlme_priv *pmlmepriv;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if ((iface) && rtw_is_adapter_up(iface)) {
			pmlmepriv = &iface->mlmepriv;
			if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY))
				RTW_ERR("%s ("ADPT_FMT") under survey\n", func, ADPT_ARG(iface));
		}
	}
}

void rtw_core_ap_swch_start(_adapter *padapter, struct createbss_parm *parm)
{
	struct dvobj_priv *pdvobj = padapter->dvobj;
	int i;
	u8 ht_option = 0;
	_adapter *iface;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct _ADAPTER_LINK *iface_link;

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

	parm->do_rfk = _TRUE;

	if (parm->ch_to_set != 0) {
		set_bch_bwmode(padapter, padapter_link, parm->req_band,
			parm->ch_to_set, parm->offset_to_set, parm->bw_to_set, parm->do_rfk);
		rtw_phl_mr_dump_cur_chandef(adapter_to_dvobj(padapter)->phl, padapter->phl_role);
	}

#if defined(CONFIG_IOCTL_CFG80211) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0))
	for (i = 0; i < pdvobj->iface_nums; i++) {
		iface = pdvobj->padapters[i];

		if (!(parm->ifbmp_ch_changed & BIT(i)) || !iface)
			continue;

		#ifdef CONFIG_80211N_HT
		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		iface_link = GET_PRIMARY_LINK(iface);
		ht_option = iface_link->mlmepriv.htpriv.ht_option;
		#endif

		rtw_cfg80211_ch_switch_notify(iface
			, &iface_link->mlmeextpriv.chandef
			, ht_option, 0);
	}
#endif /* defined(CONFIG_IOCTL_CFG80211) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)) */

	rtw_rfctl_update_op_mode(adapter_to_rfctl(padapter), parm->ifbmp, 1);
}

#ifdef CONFIG_DBCC_SUPPORT
static void _go_dbcc_update_ie(_adapter *a)
{
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(a);
	struct link_mlme_ext_priv *mlmeext = &(alink->mlmeextpriv);
	struct link_mlme_priv *mlmepriv = &(alink->mlmepriv);
	struct link_mlme_ext_info *mlmeinfo = &(mlmeext->mlmext_info);
	WLAN_BSSID_EX *network = &(mlmeinfo->network);
	struct HT_caps_element *ht_caps;
	u32 ht_cap_ie_len = 0;
	u8 *ht_cap_ie = NULL, *ie = network->IEs;

	if (!rtw_phl_mr_is_db(adapter_to_dvobj(a)->phl))
		return;
#ifdef CONFIG_P2P
	if (!MLME_IS_GO(a))
		return;
#ifdef CONFIG_80211N_HT
	if (!mlmepriv->htpriv.ht_option)
		return;

	if (!mlmeinfo->HT_caps_enable)
		return;

	ht_cap_ie = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_CAPABILITY_IE_, &ht_cap_ie_len, (network->IELength - _BEACON_IE_OFFSET_));
	if (ht_cap_ie && ht_cap_ie_len > 0) {
		ht_caps = (struct HT_caps_element *)(ht_cap_ie + 2);
		RTW_INFO(FUNC_ADPT_FMT": orig ht caps info = 0x%02x\n", FUNC_ADPT_ARG(a), ht_caps->u.HT_cap_element.HT_caps_info);
		SET_HT_CAP_ELE_SM_PS(&(ht_caps->u.HT_cap_element.HT_caps_info), SM_PS_STATIC);
		RTW_INFO(FUNC_ADPT_FMT": update ht caps info = 0x%02x\n", FUNC_ADPT_ARG(a), ht_caps->u.HT_cap_element.HT_caps_info);
	}
#endif
#endif
}
#endif

void rtw_core_ap_start(_adapter *padapter, struct createbss_parm *parm)
{
	struct dvobj_priv *pdvobj = padapter->dvobj;
	int i;
	_adapter *iface;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct _ADAPTER_LINK *iface_link;
	struct link_mlme_priv *mlme;

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));
	if (0) {
		RTW_INFO(FUNC_ADPT_FMT" done\n", FUNC_ADPT_ARG(padapter));
		dump_adapters_status(RTW_DBGDUMP , adapter_to_dvobj(padapter));
	}

	for (i = 0; i < pdvobj->iface_nums; i++) {
		iface = pdvobj->padapters[i];

		if (!(parm->ifbmp & BIT(i)) || !iface)
			continue;

		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		iface_link = GET_PRIMARY_LINK(iface);
		/* update beacon content only if bstart_bss is _TRUE */
		if (iface_link->mlmeextpriv.bstart_bss != _TRUE)
			continue;

		mlme = &(iface_link->mlmepriv);

		#ifdef CONFIG_80211N_HT
		if ((ATOMIC_READ(&mlme->olbc) == _TRUE) || (ATOMIC_READ(&mlme->olbc_ht) == _TRUE)) {
			/* AP is not starting a 40 MHz BSS in presence of an 802.11g BSS. */
			mlme->ht_op_mode &= (~HT_INFO_OPERATION_MODE_OP_MODE_MASK);
			mlme->ht_op_mode |= OP_MODE_MAY_BE_LEGACY_STAS;
			rtw_update_beacon(iface, iface_link, _HT_ADD_INFO_IE_, NULL, _FALSE, 0);
		}
		#endif

		rtw_update_beacon(iface, iface_link, _TIM_IE_, NULL, _FALSE, 0);
	}

#ifdef CONFIG_DBCC_SUPPORT
	_go_dbcc_update_ie(padapter);
#endif

	/* after chanctx_add & before send bcn */
#ifndef CONFIG_AP_CMD_DISPR
	rtw_hw_start_bss_network(padapter);
#endif

	rtw_scan_abort(padapter, 0);
	_rtw_iface_undersurvey_chk(__func__, padapter);

	/* send beacon */
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI) || defined(CONFIG_PCI_BCN_POLLING) || defined(CONFIG_HWSIM)
	for (i = 0; i < pdvobj->iface_nums; i++) {
		iface = pdvobj->padapters[i];
		if (!(parm->ifbmp & BIT(i)) || !iface)
			continue;

		if (send_beacon(iface) == _FAIL)
			RTW_INFO(ADPT_FMT" issue_beacon, fail!\n", ADPT_ARG(pdvobj->padapters[i]));
	}
#endif

	if (parm->is_change_chbw)
		rtw_phl_cmd_stop_beacon(adapter_to_dvobj(padapter)->phl, padapter_link->wrlink,
					_FALSE, PHL_CMD_DIRECTLY, 0);


#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	if (MLME_IS_AP(padapter) && padapter->tbtx_capability == _TRUE) {
		_set_timer(&pmlmeext->tbtx_token_dispatch_timer, 1);
		RTW_INFO("Start token dispatch\n");
	}
#endif
}
void rtw_core_ap_chan_decision(_adapter *padapter, struct createbss_parm *parm)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	bool is_chctx_add = false;
	struct rtw_chan_def new_chdef = {0};
	struct rtw_mr_chctx_info mr_cc_info = {0};

	if (!(parm->req_ch == 0 && padapter->mlmeextpriv.mlmext_info.state == WIFI_FW_AP_STATE))
		return;

#ifdef CONFIG_DBCC_SUPPORT
	_rtw_memcpy(&new_chdef, &pmlmepriv->ori_chandef, sizeof(struct rtw_chan_def));
	is_chctx_add = rtw_phl_chanctx_chk(dvobj->phl, padapter->phl_role,
						padapter_link->wrlink,
						&new_chdef, &mr_cc_info);

	RTW_INFO("%s => chctx_chk:%s\n", __func__, (is_chctx_add) ? "Y" : "N");
	RTW_INFO("PHL- CH:%d, BW:%d OFF:%d\n", new_chdef.chan, new_chdef.bw, new_chdef.offset);
	if (is_chctx_add == false && mr_cc_info.sugg_opmode == MR_OP_DBCC) {
		_rtw_memcpy(&pmlmeext->chandef, &pmlmepriv->ori_chandef, sizeof(struct rtw_chan_def));
		rtw_phl_mr_trig_dbcc_enable(dvobj->phl);
		parm->is_trigger_dbcc = _TRUE;
	}
#endif
}

void rtw_core_ap_prepare(_adapter *padapter, struct createbss_parm *parm)
{
	u8 mlme_act = MLME_ACTION_UNKNOWN;
	u16 bcn_interval;
	struct registry_priv *pregpriv = &padapter->registrypriv;
	struct security_priv *psecuritypriv = &(padapter->securitypriv);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	s8 req_band = REQ_BAND_NONE;
	s16 req_ch = REQ_CH_NONE, req_bw = REQ_BW_NONE, req_offset = REQ_OFFSET_NONE;
	struct rtw_chan_def chdef_to_set = {0};
	u8 do_rfk = _FALSE;
	int i;
	u8 ifbmp_ch_changed = 0;
#ifdef CONFIG_MCC_MODE
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);
	u8 mcc_sup = phl_com->dev_cap.mcc_sup;
#endif
	struct rtw_chan_def new_chdef = {0};
	struct rtw_mr_chctx_info mr_cc_info = {0};
	bool is_chctx_add = false;
	_adapter *iface = NULL;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct _ADAPTER_LINK *iface_link = NULL;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	WLAN_BSSID_EX *pnetwork = (WLAN_BSSID_EX *)&pmlmepriv->cur_network.network; /* used as input */
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork_mlmeext = &(pmlmeinfo->network);

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

	if (parm->req_ch != 0) {
		/* bypass other setting, go checking ch, bw, offset */
		mlme_act = MLME_OPCH_SWITCH;
		req_band = parm->req_band;
		req_ch = parm->req_ch;
		req_bw = parm->req_bw;
		req_offset = parm->req_offset;
		goto chbw_decision;
	} else {
		/* request comes from upper layer */
		if (MLME_IS_AP(padapter))
			mlme_act = MLME_AP_STARTED;
		else if (MLME_IS_MESH(padapter))
			mlme_act = MLME_MESH_STARTED;
		else
			rtw_warn_on(1);
		req_ch = 0;
		/* MLD network info copied from adapter->mlmepriv.cur_network */
		_rtw_memcpy(&padapter->mlmeextpriv.mlmext_info.dev_network,
			&padapter->mlmepriv.dev_cur_network.network,
			padapter->mlmepriv.dev_cur_network.network.Length);
		/* Link network info copied from adapter_link->mlmepriv.cur_network */
		_rtw_memcpy(pnetwork_mlmeext, pnetwork, pnetwork->Length);
	}

	bcn_interval = (u16)pnetwork->Configuration.BeaconPeriod;
	pmlmeinfo->bcn_interval = bcn_interval;

	/* check if there is wps ie, */
	/* if there is wpsie in beacon, the hostapd will update beacon twice when stating hostapd, */
	/* and at first time the security ie ( RSN/WPA IE) will not include in beacon. */
	if (NULL == rtw_get_wps_ie(pnetwork->IEs + _FIXED_IE_LENGTH_, pnetwork->IELength - _FIXED_IE_LENGTH_, NULL, NULL))
		pmlmeext->bstart_bss = _TRUE;

	/* todo: update wmm, ht cap */
	/* pmlmeinfo->WMM_enable; */
	/* pmlmeinfo->HT_enable; */
	if (pmlmepriv->qospriv.qos_option)
		pmlmeinfo->WMM_enable = _TRUE;

#ifdef CONFIG_80211N_HT
	if (pmlmepriv->htpriv.ht_option) {
		pmlmeinfo->WMM_enable = _TRUE;
		pmlmeinfo->HT_enable = _TRUE;
	} else
		pmlmeinfo->HT_enable = _FALSE;
#endif

#ifdef CONFIG_80211AC_VHT
	if (pmlmepriv->vhtpriv.vht_option)
		pmlmeinfo->VHT_enable = _TRUE;
	else
		pmlmeinfo->VHT_enable = _FALSE;
#endif

#ifdef CONFIG_80211AX_HE
	if (pmlmepriv->hepriv.he_option)
		pmlmeinfo->HE_enable = _TRUE;
	else
		pmlmeinfo->HE_enable = _FALSE;
#endif

	if (pmlmepriv->cur_network.join_res != _TRUE) { /* setting only at  first time */
		/* WEP Key will be set before this function, do not clear CAM. */
		if ((psecuritypriv->dot11PrivacyAlgrthm != _WEP40_) && (psecuritypriv->dot11PrivacyAlgrthm != _WEP104_)
			&& !MLME_IS_MESH(padapter) /* mesh group key is set before this function */
		)
			flush_all_cam_entry(padapter, PHL_CMD_DIRECTLY, 0);	/* clear CAM */
	}

#ifdef CONFIG_RTW_MULTI_AP
	rtw_map_config_monitor(padapter, mlme_act);
#endif

chbw_decision:
	/*SCC && MCC at the same hw band*/
	ifbmp_ch_changed = rtw_ap_bchbw_decision(padapter, parm->ifbmp, parm->excl_ifbmp, req_band
						, req_ch, req_bw, req_offset, &chdef_to_set);

	for (i = 0; i < dvobj->iface_nums; i++) {
		if (!(parm->ifbmp & BIT(i)) || !dvobj->padapters[i])
			continue;
		iface = dvobj->padapters[i];

		if (!rtw_iface_at_same_hwband(padapter, iface))
			continue;

		/* let pnetwork_mlme == pnetwork_mlmeext */
		/* MLD network info copied from adapter */
		_rtw_memcpy(&(iface->mlmepriv.dev_cur_network.network)
			, &(iface->mlmeextpriv.mlmext_info.dev_network)
			, iface->mlmeextpriv.mlmext_info.dev_network.Length);
		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		/* Link network info copied from adapter_link */
		iface_link = GET_PRIMARY_LINK(dvobj->padapters[i]);
		_rtw_memcpy(&(iface_link->mlmepriv.cur_network.network)
			, &(iface_link->mlmeextpriv.mlmext_info.network)
			, iface_link->mlmeextpriv.mlmext_info.network.Length);

		rtw_start_bss_hdl_after_chbw_decided(iface, iface_link);

		/* Set EDCA param reg after update cur_wireless_mode & update_capinfo */
		if (pregpriv->wifi_spec == 1)
			rtw_set_hw_wmm_param(iface, iface_link);
	}

	if (parm->is_change_chbw) {
		rtw_phl_cmd_stop_beacon(dvobj->phl, padapter_link->wrlink,
					_TRUE, PHL_CMD_DIRECTLY, 0);
		rtw_phl_chanctx_del(dvobj->phl, padapter->phl_role, padapter_link->wrlink, NULL);
	}

	new_chdef.band = pmlmeext->chandef.band;
	new_chdef.chan = pmlmeext->chandef.chan;
	new_chdef.bw = pmlmeext->chandef.bw;
	new_chdef.offset = pmlmeext->chandef.offset;

	is_chctx_add = rtw_phl_chanctx_add(dvobj->phl, padapter->phl_role,
						padapter_link->wrlink, &new_chdef, &mr_cc_info);

	rtw_hw_update_chan_def(padapter, padapter_link);

#if defined(CONFIG_DFS_MASTER)
	rtw_dfs_rd_en_dec_on_mlme_act(padapter, padapter_link, mlme_act, parm->excl_ifbmp);
#endif

	RTW_INFO("%s => chctx_add:%s\n", __func__, (is_chctx_add) ? "Y" : "N");
	RTW_INFO("PHL- BAND: %d, CH:%d, BW:%d OFF:%d\n", new_chdef.band, new_chdef.chan, new_chdef.bw, new_chdef.offset);
	if (chdef_to_set.chan != new_chdef.chan || chdef_to_set.band != new_chdef.band) {
		RTW_INFO("Core - BAND:%d, CH:%d, BW:%d OFF:%d\n", chdef_to_set.band, chdef_to_set.chan, chdef_to_set.bw, chdef_to_set.offset);
		RTW_ERR("chan set mismatch!!!\n");
	}

	if (is_chctx_add == false) {
		RTW_ERR("%s : rtw_phl_chanctx_add fail\n", __func__);
		if (0)
			rtw_warn_on(1);
	}

_exit:
	parm->ifbmp_ch_changed = ifbmp_ch_changed;
	parm->req_band = new_chdef.band;
	parm->ch_to_set = new_chdef.chan;
	parm->offset_to_set = new_chdef.offset;
	parm->bw_to_set = new_chdef.bw;
	parm->do_rfk = do_rfk;
}

int rtw_check_beacon_data(_adapter *padapter, u8 *pbuf,  int len)
{
	int ret = _SUCCESS;
	u8 *p;
	u8 *pHT_caps_ie = NULL;
	u8 *pHT_info_ie = NULL;
	u16 cap, ht_cap = _FALSE;
	uint ie_len = 0;
	int group_cipher, pairwise_cipher, gmcs;
	u32 akm;
	u8 mfp_opt = MFP_NO;
	u8	channel, network_type;
	u8 OUI1[] = {0x00, 0x50, 0xf2, 0x01};
	u8 WMM_PARA_IE[] = {0x00, 0x50, 0xf2, 0x02, 0x01, 0x01};
	u8 WIFI_ALLIANCE_OUI[] = {0x50, 0x6f, 0x9a};
	HT_CAP_AMPDU_DENSITY best_ampdu_density = 0;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_security_priv *lsecuritypriv = &padapter_link->securitypriv;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	WLAN_BSSID_EX *pbss_network = (WLAN_BSSID_EX *)&pmlmepriv->cur_network.network;
	u8 *ie = pbss_network->IEs;
	u8 vht_cap = _FALSE;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	struct rtw_chan_def chandef = {0};
	enum band_type band = BAND_ON_24G;
	u8 rf_num = 0;
	int ret_rm;
	/* SSID */
	/* Supported rates */
	/* DS Params */
	/* WLAN_EID_COUNTRY */
	/* ERP Information element */
	/* Extended supported rates */
	/* WPA/WPA2 */
	/* Radio Resource Management */
	/* Wi-Fi Wireless Multimedia Extensions */
	/* ht_capab, ht_oper */
	/* WPS IE */

	RTW_INFO("%s, len=%d\n", __FUNCTION__, len);

	if (!MLME_IS_AP(padapter) && !MLME_IS_MESH(padapter))
		return _FAIL;


	if (len > MAX_IE_SZ)
		return _FAIL;

	pbss_network->IELength = len;

	_rtw_memset(ie, 0, MAX_IE_SZ);

	_rtw_memcpy(ie, pbuf, pbss_network->IELength);


	if (padapter->mlmepriv.dev_cur_network.network.InfrastructureMode != Ndis802_11APMode
		&& padapter->mlmepriv.dev_cur_network.network.InfrastructureMode != Ndis802_11_mesh
	) {
		rtw_warn_on(1);
		return _FAIL;
	}


	rtw_ap_check_scan(padapter, padapter_link);


	pbss_network->PhyInfo.rssi= 0;

	/* MLD mac address */
	_rtw_memcpy(padapter->mlmepriv.dev_cur_network.network.MacAddress, adapter_mac_addr(padapter), ETH_ALEN);
	/* Link mac address */
	_rtw_memcpy(pbss_network->MacAddress, padapter_link->mac_addr, ETH_ALEN);

	/* beacon interval */
	p = rtw_get_beacon_interval_from_ie(ie);/* ie + 8;	 */ /* 8: TimeStamp, 2: Beacon Interval 2:Capability */
	/* pbss_network->Configuration.BeaconPeriod = le16_to_cpu(*(unsigned short*)p); */
	pbss_network->Configuration.BeaconPeriod = RTW_GET_LE16(p);

	/* capability */
	p = rtw_get_capability_from_ie(ie);
	cap = RTW_GET_LE16(p);

	/* SSID */
	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _SSID_IE_, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_));
	if (p && ie_len > 0) {
		_rtw_memset(&pbss_network->Ssid, 0, sizeof(NDIS_802_11_SSID));
		_rtw_memcpy(pbss_network->Ssid.Ssid, (p + 2), ie_len);
		pbss_network->Ssid.SsidLength = ie_len;
	}

#ifdef CONFIG_RTW_MESH
	/* Mesh ID */
	if (MLME_IS_MESH(padapter)) {
		p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, WLAN_EID_MESH_ID, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_));
		if (p && ie_len > 0) {
			_rtw_memset(&pbss_network->mesh_id, 0, sizeof(NDIS_802_11_SSID));
			_rtw_memcpy(pbss_network->mesh_id.Ssid, (p + 2), ie_len);
			pbss_network->mesh_id.SsidLength = ie_len;
		}
	}
#endif

	/* Parsing extended capabilities IE */
	rtw_parse_ext_cap_ie(pmlmepriv->ext_capab_ie_data, &(pmlmepriv->ext_capab_ie_len), ie \
		, pbss_network->IELength, _BEACON_IE_OFFSET_);

	/* channel and band */
	rtw_ies_get_bchbw(ie + _BEACON_IE_OFFSET_, pbss_network->IELength - _BEACON_IE_OFFSET_,
		&chandef.band, &chandef.chan, (u8 *)&chandef.bw, (u8 *)&chandef.offset,
		(u8 *)&chandef.center_freq1, (u8 *)&chandef.center_freq2, 1, 1, 1);

	if (chandef.chan != 0) {
		pbss_network->Configuration.Band = band = chandef.band;
		pbss_network->Configuration.DSConfig = channel = chandef.chan;
	}

	if (pbss_network->Configuration.Band == BAND_ON_6G)
		/* DSSET uses in 2.4G and 5G band; Remove DSSET when using 6G band */
		rtw_remove_bcn_ie(padapter, pbss_network, _DSSET_IE_);

	/*	support rate ie & ext support ie & IElen & SupportedRates	*/
	network_type = rtw_update_rate_bymode(pbss_network, pregistrypriv->wireless_mode);

	/* parsing ERP_IE */
	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _ERPINFO_IE_, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_));
	if (p && ie_len > 0)  {
		if(pregistrypriv->wireless_mode == WLAN_MD_11B)
		{
			pbss_network->IELength = pbss_network->IELength - *(p+1) - 2;
			ret_rm = rtw_ies_remove_ie(ie , &len, _BEACON_IE_OFFSET_, _ERPINFO_IE_,NULL,0);
			RTW_DBG("%s, remove_ie of ERP_IE=%d\n", __FUNCTION__, ret_rm);
		} else
			ERP_IE_handler(padapter, padapter_link, (PNDIS_802_11_VARIABLE_IEs)p);

	}

	/* update privacy/security */
	if (cap & BIT(4))
		pbss_network->Privacy = 1;
	else
		pbss_network->Privacy = 0;

	psecuritypriv->wpa_psk = 0;

	/* wpa2 */
	akm = 0;
	gmcs = 0;
	group_cipher = 0;
	pairwise_cipher = 0;
	psecuritypriv->wpa2_group_cipher = _NO_PRIVACY_;
	psecuritypriv->wpa2_pairwise_cipher = _NO_PRIVACY_;
	psecuritypriv->akmp = 0;
	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _RSN_IE_2_, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_));
	if (p && ie_len > 0) {
		if (rtw_parse_wpa2_ie(p, ie_len + 2, &group_cipher, &pairwise_cipher, &gmcs, &akm, &mfp_opt, NULL) == _SUCCESS) {
			psecuritypriv->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
			psecuritypriv->ndisauthtype = Ndis802_11AuthModeWPA2PSK;
			psecuritypriv->dot8021xalg = 1;/* psk,  todo:802.1x */
			psecuritypriv->wpa_psk |= BIT(1);

			psecuritypriv->wpa2_group_cipher = group_cipher;
			psecuritypriv->wpa2_pairwise_cipher = pairwise_cipher;
			psecuritypriv->akmp = akm;

#ifdef CONFIG_IOCTL_CFG80211
			/**
			 * Kernel < v5.x, the auth_type set as
			 * NL80211_AUTHTYPE_AUTOMATIC in
			 * cfg80211_rtw_start_ap(). if the AKM SAE in the RSN
			 * IE, we have to update the auth_type for SAE in
			 * rtw_check_beacon_data()
			 */
			if (CHECK_BIT(WLAN_AKM_TYPE_SAE, akm)) {
				RTW_INFO("%s: Auth type as SAE\n", __func__);
				psecuritypriv->auth_type = MLME_AUTHTYPE_SAE;
				psecuritypriv->auth_alg = WLAN_AUTH_SAE;
			}
#endif /* CONFIG_IOCTL_CFG80211 */
#if 0
			switch (group_cipher) {
			case WPA_CIPHER_NONE:
				psecuritypriv->wpa2_group_cipher = _NO_PRIVACY_;
				break;
			case WPA_CIPHER_WEP40:
				psecuritypriv->wpa2_group_cipher = _WEP40_;
				break;
			case WPA_CIPHER_TKIP:
				psecuritypriv->wpa2_group_cipher = _TKIP_;
				break;
			case WPA_CIPHER_CCMP:
				psecuritypriv->wpa2_group_cipher = _AES_;
				break;
			case WPA_CIPHER_WEP104:
				psecuritypriv->wpa2_group_cipher = _WEP104_;
				break;
			}

			switch (pairwise_cipher) {
			case WPA_CIPHER_NONE:
				psecuritypriv->wpa2_pairwise_cipher = _NO_PRIVACY_;
				break;
			case WPA_CIPHER_WEP40:
				psecuritypriv->wpa2_pairwise_cipher = _WEP40_;
				break;
			case WPA_CIPHER_TKIP:
				psecuritypriv->wpa2_pairwise_cipher = _TKIP_;
				break;
			case WPA_CIPHER_CCMP:
				psecuritypriv->wpa2_pairwise_cipher = _AES_;
				break;
			case WPA_CIPHER_WEP104:
				psecuritypriv->wpa2_pairwise_cipher = _WEP104_;
				break;
			}
#endif
		}

	}

	/* wpa */
	ie_len = 0;
	group_cipher = 0;
	pairwise_cipher = 0;
	psecuritypriv->wpa_group_cipher = _NO_PRIVACY_;
	psecuritypriv->wpa_pairwise_cipher = _NO_PRIVACY_;
	for (p = ie + _BEACON_IE_OFFSET_; ; p += (ie_len + 2)) {
		p = rtw_get_ie(p, _SSN_IE_1_, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_ - (ie_len + 2)));
		if ((p) && (_rtw_memcmp(p + 2, OUI1, 4))) {
			if (rtw_parse_wpa_ie(p, ie_len + 2, &group_cipher, &pairwise_cipher, NULL) == _SUCCESS) {
				psecuritypriv->dot11AuthAlgrthm = dot11AuthAlgrthm_8021X;
				psecuritypriv->ndisauthtype = Ndis802_11AuthModeWPAPSK;
				psecuritypriv->dot8021xalg = 1;/* psk,  todo:802.1x */

				psecuritypriv->wpa_psk |= BIT(0);

				psecuritypriv->wpa_group_cipher = group_cipher;
				psecuritypriv->wpa_pairwise_cipher = pairwise_cipher;

#if 0
				switch (group_cipher) {
				case WPA_CIPHER_NONE:
					psecuritypriv->wpa_group_cipher = _NO_PRIVACY_;
					break;
				case WPA_CIPHER_WEP40:
					psecuritypriv->wpa_group_cipher = _WEP40_;
					break;
				case WPA_CIPHER_TKIP:
					psecuritypriv->wpa_group_cipher = _TKIP_;
					break;
				case WPA_CIPHER_CCMP:
					psecuritypriv->wpa_group_cipher = _AES_;
					break;
				case WPA_CIPHER_WEP104:
					psecuritypriv->wpa_group_cipher = _WEP104_;
					break;
				}

				switch (pairwise_cipher) {
				case WPA_CIPHER_NONE:
					psecuritypriv->wpa_pairwise_cipher = _NO_PRIVACY_;
					break;
				case WPA_CIPHER_WEP40:
					psecuritypriv->wpa_pairwise_cipher = _WEP40_;
					break;
				case WPA_CIPHER_TKIP:
					psecuritypriv->wpa_pairwise_cipher = _TKIP_;
					break;
				case WPA_CIPHER_CCMP:
					psecuritypriv->wpa_pairwise_cipher = _AES_;
					break;
				case WPA_CIPHER_WEP104:
					psecuritypriv->wpa_pairwise_cipher = _WEP104_;
					break;
				}
#endif
			}

			break;

		}

		if ((p == NULL) || (ie_len == 0))
			break;

	}

	if (mfp_opt == MFP_INVALID) {
		RTW_INFO(FUNC_ADPT_FMT" invalid MFP setting\n", FUNC_ADPT_ARG(padapter));
		return _FAIL;
	}
	psecuritypriv->mfp_opt = mfp_opt;

	/* RRM */
	rm_update_cap(pbuf, padapter, len, _BEACON_IE_OFFSET_);

	/* wmm */
	ie_len = 0;
	pmlmepriv->qospriv.qos_option = 0;
#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter))
		pmlmepriv->qospriv.qos_option = 1;
#endif
	if (pregistrypriv->wmm_enable) {
		for (p = ie + _BEACON_IE_OFFSET_; ; p += (ie_len + 2)) {
			p = rtw_get_ie(p, _VENDOR_SPECIFIC_IE_, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_ - (ie_len + 2)));
			if ((p) && _rtw_memcmp(p + 2, WMM_PARA_IE, 6)) {
				pmlmepriv->qospriv.qos_option = 1;

				*(p + 8) |= BIT(7); /* QoS Info, support U-APSD */

				/* disable all ACM bits since the WMM admission control is not supported */
				*(p + 10) &= ~BIT(4); /* BE */
				*(p + 14) &= ~BIT(4); /* BK */
				*(p + 18) &= ~BIT(4); /* VI */
				*(p + 22) &= ~BIT(4); /* VO */

				WMM_param_handler(padapter, padapter_link,
						(PNDIS_802_11_VARIABLE_IEs)p);

				break;
			}

			if ((p == NULL) || (ie_len == 0))
				break;
		}
	}
#ifdef CONFIG_80211N_HT
	if(pregistrypriv->ht_enable && is_supported_ht(pregistrypriv->wireless_mode))
	{
		/* parsing HT_CAP_IE */
		p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_CAPABILITY_IE_, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_));
		if (p && ie_len > 0) {
			HT_CAP_AMPDU_FACTOR max_rx_ampdu_factor = MAX_AMPDU_FACTOR_64K;
			struct rtw_ieee80211_ht_cap *pht_cap = (struct rtw_ieee80211_ht_cap *)(p + 2);

			if (0) {
				RTW_INFO(FUNC_ADPT_FMT" HT_CAP_IE from upper layer:\n", FUNC_ADPT_ARG(padapter));
				dump_ht_cap_ie_content(RTW_DBGDUMP, p + 2, ie_len);
			}

			pHT_caps_ie = p;

			ht_cap = _TRUE;
			network_type |= WLAN_MD_11N;

			rtw_ht_use_default_setting(padapter, padapter_link);

			/* Update HT Capabilities Info field */
			if (pmlmepriv->htpriv.sgi_20m == _FALSE)
				pht_cap->cap_info &= ~(IEEE80211_HT_CAP_SGI_20);

			if (pmlmepriv->htpriv.sgi_40m == _FALSE)
				pht_cap->cap_info &= ~(IEEE80211_HT_CAP_SGI_40);

			if (!TEST_FLAG(pmlmepriv->htpriv.ldpc_cap, LDPC_HT_ENABLE_RX))
				pht_cap->cap_info &= ~(IEEE80211_HT_CAP_LDPC_CODING);

			if (!TEST_FLAG(pmlmepriv->htpriv.stbc_cap, STBC_HT_ENABLE_TX))
				pht_cap->cap_info &= ~(IEEE80211_HT_CAP_TX_STBC);

			if (!TEST_FLAG(pmlmepriv->htpriv.stbc_cap, STBC_HT_ENABLE_RX))
				pht_cap->cap_info &= ~(IEEE80211_HT_CAP_RX_STBC_3R);

			/* Update A-MPDU Parameters field */
			pht_cap->ampdu_params_info &= ~(IEEE80211_HT_CAP_AMPDU_FACTOR | IEEE80211_HT_CAP_AMPDU_DENSITY);

			if ((psecuritypriv->wpa_pairwise_cipher & WPA_CIPHER_CCMP) ||
				(psecuritypriv->wpa2_pairwise_cipher & WPA_CIPHER_CCMP)) {
				rtw_hal_get_def_var(padapter, padapter_link, HW_VAR_BEST_AMPDU_DENSITY, &best_ampdu_density);
				pht_cap->ampdu_params_info |= (IEEE80211_HT_CAP_AMPDU_DENSITY & (best_ampdu_density << 2));
			} else
				pht_cap->ampdu_params_info |= (IEEE80211_HT_CAP_AMPDU_DENSITY & 0x00);

			rtw_hal_get_def_var(padapter, padapter_link, HW_VAR_MAX_RX_AMPDU_FACTOR, &max_rx_ampdu_factor);
			pht_cap->ampdu_params_info |= (IEEE80211_HT_CAP_AMPDU_FACTOR & max_rx_ampdu_factor); /* set  Max Rx AMPDU size  to 64K */

			_rtw_memcpy(&(pmlmeinfo->HT_caps), pht_cap, sizeof(struct HT_caps_element));

			/* Update Supported MCS Set field */
			{
				u8 rx_nss = 0;
				int i;

				rx_nss = get_phy_rx_nss(padapter, padapter_link);

				/* RX MCS Bitmask */
				switch (rx_nss) {
				case 1:
					set_mcs_rate_by_mask(HT_CAP_ELE_RX_MCS_MAP(pht_cap), MCS_RATE_1R);
					break;
				case 2:
					set_mcs_rate_by_mask(HT_CAP_ELE_RX_MCS_MAP(pht_cap), MCS_RATE_2R);
					break;
				case 3:
					set_mcs_rate_by_mask(HT_CAP_ELE_RX_MCS_MAP(pht_cap), MCS_RATE_3R);
					break;
				case 4:
					set_mcs_rate_by_mask(HT_CAP_ELE_RX_MCS_MAP(pht_cap), MCS_RATE_4R);
					break;
				default:
					RTW_WARN("rf_type:%d or rx_nss:%u is not expected\n",
						GET_HAL_RFPATH(adapter_to_dvobj(padapter)), rx_nss);
				}
				for (i = 0; i < 10; i++)
					*(HT_CAP_ELE_RX_MCS_MAP(pht_cap) + i) &= pmlmeext->default_supported_mcs_set[i];
			}

#ifdef CONFIG_BEAMFORMING
			/* Use registry value to enable HT Beamforming. */
			/* ToDo: use configure file to set these capability. */
			pht_cap->tx_BF_cap_info = 0;

			/* HT Beamformer */
			if (TEST_FLAG(pmlmepriv->htpriv.beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE)) {
				/* Transmit NDP Capable */
				SET_HT_CAP_TXBF_TRANSMIT_NDP_CAP(pht_cap, 1);
				/* Explicit Compressed Steering Capable */
				SET_HT_CAP_TXBF_EXPLICIT_COMP_STEERING_CAP(pht_cap, 1);
				/* Compressed Steering Number Antennas */
				SET_HT_CAP_TXBF_COMP_STEERING_NUM_ANTENNAS(pht_cap, 1);
				rtw_hal_get_def_var(padapter, padapter_link, HAL_DEF_BEAMFORMER_CAP, (u8 *)&rf_num);
				if (rf_num > 3)
					rf_num = 3;
				SET_HT_CAP_TXBF_CHNL_ESTIMATION_NUM_ANTENNAS(pht_cap, rf_num);
			}

			/* HT Beamformee */
			if (TEST_FLAG(pmlmepriv->htpriv.beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE)) {
				/* Receive NDP Capable */
				SET_HT_CAP_TXBF_RECEIVE_NDP_CAP(pht_cap, 1);
				/* Explicit Compressed Beamforming Feedback Capable */
				SET_HT_CAP_TXBF_EXPLICIT_COMP_FEEDBACK_CAP(pht_cap, 2);
				rtw_hal_get_def_var(padapter, padapter_link, HAL_DEF_BEAMFORMEE_CAP, (u8 *)&rf_num);
				if (rf_num > 3)
					rf_num = 3;
				SET_HT_CAP_TXBF_COMP_STEERING_NUM_ANTENNAS(pht_cap, rf_num);
			}
#endif /* CONFIG_BEAMFORMING */

			_rtw_memcpy(&pmlmepriv->htpriv.ht_cap, p + 2, ie_len);

			if (0) {
				RTW_INFO(FUNC_ADPT_FMT" HT_CAP_IE driver masked:\n", FUNC_ADPT_ARG(padapter));
				dump_ht_cap_ie_content(RTW_DBGDUMP, p + 2, ie_len);
			}
		}

		/* parsing HT_INFO_IE */
		p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_ADD_INFO_IE_, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_));
		if (p && ie_len > 0) {
			pHT_info_ie = p;
			if (channel == 0)
				pbss_network->Configuration.DSConfig = GET_HT_OP_ELE_PRI_CHL(pHT_info_ie + 2);
			else if (channel != GET_HT_OP_ELE_PRI_CHL(pHT_info_ie + 2)) {
				RTW_INFO(FUNC_ADPT_FMT" ch inconsistent, DSSS:%u, HT primary:%u\n"
					, FUNC_ADPT_ARG(padapter), channel, GET_HT_OP_ELE_PRI_CHL(pHT_info_ie + 2));
			}
		}
	}
#endif /* CONFIG_80211N_HT */
	pmlmepriv->cur_network.network_type = network_type;

#ifdef CONFIG_80211N_HT
	pmlmepriv->htpriv.ht_option = _FALSE;

	if ((psecuritypriv->wpa2_pairwise_cipher & WPA_CIPHER_TKIP) ||
	    (psecuritypriv->wpa_pairwise_cipher & WPA_CIPHER_TKIP)) {
		/* todo: */
		/* ht_cap = _FALSE; */
	}

	/* ht_cap	 */
	if (pregistrypriv->ht_enable &&
		is_supported_ht(pregistrypriv->wireless_mode) && ht_cap == _TRUE &&
		(pbss_network->Configuration.Band == BAND_ON_24G ||
		 pbss_network->Configuration.Band == BAND_ON_5G)) {

		pmlmepriv->htpriv.ht_option = _TRUE;
		pmlmepriv->qospriv.qos_option = 1;

		pmlmepriv->ampdu_priv.ampdu_enable = pregistrypriv->ampdu_enable ? _TRUE : _FALSE;

		HT_caps_handler(padapter, padapter_link, (PNDIS_802_11_VARIABLE_IEs)pHT_caps_ie);

		HT_info_handler(padapter, padapter_link, (PNDIS_802_11_VARIABLE_IEs)pHT_info_ie);
	}
#endif

	pmlmepriv->upper_layer_setting = _FALSE;

#ifdef CONFIG_80211AC_VHT
	pmlmepriv->vhtpriv.vht_option = _FALSE;

	if (pmlmepriv->htpriv.ht_option == _TRUE
		&& pbss_network->Configuration.Band == BAND_ON_5G
		&& REGSTY_IS_11AC_ENABLE(pregistrypriv)
		&& is_supported_vht(pregistrypriv->wireless_mode)
		&& RFCTL_REG_EN_11AC(rfctl)) {
		/* Parsing VHT_CAP_IE */
		p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, WLAN_EID_VHT_CAPABILITY,
			&ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_));
		if (p && ie_len > 0)
			vht_cap = _TRUE;
		else
			RTW_INFO(FUNC_ADPT_FMT" No vht_capability_ie from hostapd/wpa_supplicant\n", FUNC_ADPT_ARG(padapter));

		/* Update VHT related IEs */
		if (vht_cap == _TRUE) {
			RTW_INFO(FUNC_ADPT_FMT" VHT IEs is configured by hostapd/wpa_supplicant\n", FUNC_ADPT_ARG(padapter));
			pmlmepriv->upper_layer_setting = _TRUE;
			pmlmepriv->vhtpriv.vht_option = _TRUE;

			rtw_check_for_vht20(padapter, ie + _BEACON_IE_OFFSET_,
					pbss_network->IELength - _BEACON_IE_OFFSET_);
			rtw_check_vht_ies(padapter, padapter_link, pbss_network);
		}
		else if (REGSTY_IS_11AC_AUTO(pregistrypriv)) {
			rtw_vht_ies_detach(padapter, padapter_link, pbss_network);
			rtw_vht_ies_attach(padapter, padapter_link, pbss_network);
		}
	}

	if (pmlmepriv->vhtpriv.vht_option == _FALSE)
		rtw_vht_ies_detach(padapter, padapter_link, pbss_network);
#endif /* CONFIG_80211AC_VHT */

#ifdef CONFIG_80211AX_HE
	pmlmepriv->hepriv.he_option = _FALSE;

	/* An HE STA is also a VHT STA if operating in the 5 GHz band */
	/* An HE STA is also a HT STA in the 2GHz band */

	if (REGSTY_IS_11AX_ENABLE(pregistrypriv)
		&& is_supported_he(pregistrypriv->wireless_mode)
		&& RFCTL_REG_EN_11AX(rfctl)
		&& ((band == BAND_ON_5G && pmlmepriv->vhtpriv.vht_option == _TRUE)
		|| (band == BAND_ON_24G && pmlmepriv->htpriv.ht_option == _TRUE)
		|| (band == BAND_ON_6G))) {
		u8 he_cap = _FALSE;
		u8 he_cap_eid_ext = WLAN_EID_EXTENSION_HE_CAPABILITY;

		p = rtw_get_ie_ex(ie + _BEACON_IE_OFFSET_, pbss_network->IELength - _BEACON_IE_OFFSET_,
			WLAN_EID_EXTENSION, &he_cap_eid_ext, 1, NULL, &ie_len);
		if (p && ie_len > 0)
			he_cap = _TRUE;

		/* If He capability is in beacon IE, enable he_option */
		pmlmepriv->hepriv.he_option = he_cap;

		/* Update HE related IEs */
		if (he_cap == _TRUE) {
			RTW_INFO(FUNC_ADPT_FMT" HE IEs is configured by hostapd/wpa_supplicant\n", FUNC_ADPT_ARG(padapter));
			pmlmepriv->upper_layer_setting = _TRUE;

			rtw_update_he_ies(padapter, padapter_link, pbss_network, &chandef);
		} else if (REGSTY_IS_11AX_AUTO(pregistrypriv)) {
			rtw_he_ies_detach(padapter, padapter_link, pbss_network);
			rtw_he_ies_attach(padapter, padapter_link, pbss_network, band);
		}
	}
	if (pmlmepriv->hepriv.he_option == _FALSE)
		rtw_he_ies_detach(padapter, padapter_link, pbss_network);
#endif

#ifdef CONFIG_80211BE_EHT
	padapter->mlmepriv.mlopriv.mlo_option = _FALSE;
	/* ToDo CONFIG_RTW_MLD: check if hostapd will provide hints to enable mlo */
#ifdef CONFIG_MLD_TEST
	padapter->mlmepriv.mlopriv.mlo_option = _TRUE;
#endif
#endif

#ifdef CONFIG_80211N_HT
	if(pregistrypriv->ht_enable && is_supported_ht(pregistrypriv->wireless_mode)
			&& pbss_network->Configuration.DSConfig <= 14 /*&& padapter->registrypriv.wifi_spec == 1*/)
	{
	#ifdef CONFIG_RTW_WNM
		rtw_wnm_add_btm_ext_cap(pmlmepriv->ext_capab_ie_data,
				&(pmlmepriv->ext_capab_ie_len));
	#endif
	#ifdef CONFIG_RTW_MBO
		rtw_mbo_add_internw_ext_cap(pmlmepriv->ext_capab_ie_data,
				&(pmlmepriv->ext_capab_ie_len));
	#endif
		rtw_add_ext_cap_info(pmlmepriv->ext_capab_ie_data, &(pmlmepriv->ext_capab_ie_len), BSS_COEXT);
		rtw_update_ext_cap_ie(pmlmepriv->ext_capab_ie_data, pmlmepriv->ext_capab_ie_len, pbss_network->IEs, \
			&(pbss_network->IELength), _BEACON_IE_OFFSET_);
	}
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_RTW_80211K
	padapter->rmpriv.enable = _FALSE;
	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, EID_RMEnabledCapability, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_));
	if (p && ie_len) {
		RTW_INFO("[%s]Get EID_RMEnabledCapability, len = %d\n", __func__, ie_len);
		padapter->rmpriv.enable = _TRUE;
		_rtw_memcpy(padapter->rmpriv.rm_en_cap_def, p + 2, ie_len);
	}
#endif /* CONFIG_RTW_80211K */

#ifdef CONFIG_RTW_MBO
	ie_len = 0;
	padapter->mlmepriv.mbopriv.enable = _FALSE;
	for (p = ie + _BEACON_IE_OFFSET_; ; p += (ie_len + 2)) {
		p = rtw_get_ie(p, _SSN_IE_1_, &ie_len, (pbss_network->IELength - _BEACON_IE_OFFSET_ - (ie_len + 2)));
		if ((p) && (_rtw_memcmp(p + 2, WIFI_ALLIANCE_OUI, 3)) && (*(p+5) == MBO_OUI_TYPE)) {
			/* find MBO-OCE information element */
			padapter->mlmepriv.mbopriv.enable = _TRUE;
			rtw_mbo_ie_handler(padapter, &padapter->mlmepriv.mbopriv, p + 6, ie_len - 4);
			break;
		}
		if ((p == NULL) || (ie_len == 0))
			break;
	}
#endif /* CONFIG_RTW_MBO */

	/* Build supported operating class element if ECSA enabled*/
#ifdef CONFIG_ECSA_PHL
	if (rtw_is_ecsa_enabled(padapter)) {
		u8 buf[32];
		ie_len = get_supported_op_class(padapter, buf, sizeof(buf));
		rtw_add_bcn_ie(padapter, pbss_network, WLAN_EID_SUPPORT_OP_CLASS,
					buf, ie_len);

		rtw_add_ext_cap_info(pmlmepriv->ext_capab_ie_data,
				&(pmlmepriv->ext_capab_ie_len), EXT_CH_SWITCH);
		rtw_update_ext_cap_ie(pmlmepriv->ext_capab_ie_data,
				pmlmepriv->ext_capab_ie_len, pbss_network->IEs,
				&(pbss_network->IELength), _BEACON_IE_OFFSET_);
	}
#endif

	pbss_network->Length = get_WLAN_BSSID_EX_sz((WLAN_BSSID_EX *)pbss_network);

	rtw_ies_get_bchbw(pbss_network->IEs + _BEACON_IE_OFFSET_,
		pbss_network->IELength - _BEACON_IE_OFFSET_,
		&pmlmepriv->ori_chandef.band,
		&pmlmepriv->ori_chandef.chan,
		(u8 *)&pmlmepriv->ori_chandef.bw,
		(u8 *)&pmlmepriv->ori_chandef.offset,
		(u8 *)&pmlmepriv->ori_chandef.center_freq1,
		(u8 *)&pmlmepriv->ori_chandef.center_freq2,
		1, 1, 1);

	rtw_warn_on(pmlmepriv->ori_chandef.chan == 0);

	ret = rtw_startbss_cmd(padapter, RTW_CMDF_WAIT_ACK);
	{
		int sk_band = RTW_GET_SCAN_BAND_SKIP(padapter);

		if (sk_band)
			RTW_CLR_SCAN_BAND_SKIP(padapter, sk_band);
	}

	if (ret) {
		rtw_indicate_connect(padapter);
		pmlmepriv->cur_network.join_res = _TRUE;/* for check if already set beacon */
	}

	return ret;

}

#if CONFIG_RTW_MACADDR_ACL
void rtw_macaddr_acl_init(_adapter *adapter, u8 period)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	struct wlan_acl_pool *acl;
	_queue *acl_node_q;
	int i;

	if (period >= RTW_ACL_PERIOD_NUM) {
		rtw_warn_on(1);
		return;
	}

	acl = &stapriv->acl_list[period];
	acl_node_q = &acl->acl_node_q;

	_rtw_spinlock_init(&(acl_node_q->lock));

	_rtw_spinlock_bh(&(acl_node_q->lock));
	_rtw_init_listhead(&(acl_node_q->queue));
	acl->num = 0;
	acl->mode = RTW_ACL_MODE_DISABLED;
	for (i = 0; i < NUM_ACL; i++) {
		_rtw_init_listhead(&acl->aclnode[i].list);
		acl->aclnode[i].valid = _FALSE;
	}
	_rtw_spinunlock_bh(&(acl_node_q->lock));
}

static void _rtw_macaddr_acl_deinit(_adapter *adapter, u8 period, bool clear_only)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	struct wlan_acl_pool *acl;
	_queue *acl_node_q;
	_list *head, *list;
	struct rtw_wlan_acl_node *acl_node;

	if (period >= RTW_ACL_PERIOD_NUM) {
		rtw_warn_on(1);
		return;
	}

	acl = &stapriv->acl_list[period];
	acl_node_q = &acl->acl_node_q;

	_rtw_spinlock_bh(&(acl_node_q->lock));
	head = get_list_head(acl_node_q);
	list = get_next(head);
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		acl_node = LIST_CONTAINOR(list, struct rtw_wlan_acl_node, list);
		list = get_next(list);

		if (acl_node->valid == _TRUE) {
			acl_node->valid = _FALSE;
			rtw_list_delete(&acl_node->list);
			acl->num--;
		}
	}
	_rtw_spinunlock_bh(&(acl_node_q->lock));

	if (!clear_only)
		_rtw_spinlock_free(&(acl_node_q->lock));

	rtw_warn_on(acl->num);
	acl->mode = RTW_ACL_MODE_DISABLED;
}

void rtw_macaddr_acl_deinit(_adapter *adapter, u8 period)
{
	_rtw_macaddr_acl_deinit(adapter, period, 0);
}

void rtw_macaddr_acl_clear(_adapter *adapter, u8 period)
{
	_rtw_macaddr_acl_deinit(adapter, period, 1);
}

void rtw_set_macaddr_acl(_adapter *adapter, u8 period, int mode)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	struct wlan_acl_pool *acl;

	if (period >= RTW_ACL_PERIOD_NUM) {
		rtw_warn_on(1);
		return;
	}

	acl = &stapriv->acl_list[period];

	RTW_INFO(FUNC_ADPT_FMT" p=%u, mode=%d\n"
		, FUNC_ADPT_ARG(adapter), period, mode);

	acl->mode = mode;
}

int rtw_acl_add_sta(_adapter *adapter, u8 period, const u8 *addr)
{
	_list *list, *head;
	u8 existed = 0;
	int i = -1, ret = 0;
	struct rtw_wlan_acl_node *acl_node;
	struct sta_priv *stapriv = &adapter->stapriv;
	struct wlan_acl_pool *acl;
	_queue *acl_node_q;

	if (period >= RTW_ACL_PERIOD_NUM) {
		rtw_warn_on(1);
		ret = -1;
		goto exit;
	}

	acl = &stapriv->acl_list[period];
	acl_node_q = &acl->acl_node_q;

	_rtw_spinlock_bh(&(acl_node_q->lock));

	head = get_list_head(acl_node_q);
	list = get_next(head);

	/* search for existed entry */
	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		acl_node = LIST_CONTAINOR(list, struct rtw_wlan_acl_node, list);
		list = get_next(list);

		if (_rtw_memcmp(acl_node->addr, addr, ETH_ALEN)) {
			if (acl_node->valid == _TRUE) {
				existed = 1;
				break;
			}
		}
	}
	if (existed)
		goto release_lock;

	if (acl->num >= NUM_ACL)
		goto release_lock;

	/* find empty one and use */
	for (i = 0; i < NUM_ACL; i++) {

		acl_node = &acl->aclnode[i];
		if (acl_node->valid == _FALSE) {

			_rtw_init_listhead(&acl_node->list);
			_rtw_memcpy(acl_node->addr, addr, ETH_ALEN);
			acl_node->valid = _TRUE;

			rtw_list_insert_tail(&acl_node->list, get_list_head(acl_node_q));
			acl->num++;
			break;
		}
	}

release_lock:
	_rtw_spinunlock_bh(&(acl_node_q->lock));

	if (!existed && (i < 0 || i >= NUM_ACL))
		ret = -1;

	RTW_INFO(FUNC_ADPT_FMT" p=%u "MAC_FMT" %s (acl_num=%d)\n"
		 , FUNC_ADPT_ARG(adapter), period, MAC_ARG(addr)
		, (existed ? "existed" : ((i < 0 || i >= NUM_ACL) ? "no room" : "added"))
		 , acl->num);
exit:
	return ret;
}

int rtw_acl_remove_sta(_adapter *adapter, u8 period, const u8 *addr)
{
	_list *list, *head;
	int ret = 0;
	struct rtw_wlan_acl_node *acl_node;
	struct sta_priv *stapriv = &adapter->stapriv;
	struct wlan_acl_pool *acl;
	_queue	*acl_node_q;
	u8 is_baddr = is_broadcast_mac_addr(addr);
	u8 match = 0;

	if (period >= RTW_ACL_PERIOD_NUM) {
		rtw_warn_on(1);
		goto exit;
	}

	acl = &stapriv->acl_list[period];
	acl_node_q = &acl->acl_node_q;

	_rtw_spinlock_bh(&(acl_node_q->lock));

	head = get_list_head(acl_node_q);
	list = get_next(head);

	while (rtw_end_of_queue_search(head, list) == _FALSE) {
		acl_node = LIST_CONTAINOR(list, struct rtw_wlan_acl_node, list);
		list = get_next(list);

		if (is_baddr || _rtw_memcmp(acl_node->addr, addr, ETH_ALEN)) {
			if (acl_node->valid == _TRUE) {
				acl_node->valid = _FALSE;
				rtw_list_delete(&acl_node->list);
				acl->num--;
				match = 1;
			}
		}
	}

	_rtw_spinunlock_bh(&(acl_node_q->lock));

	RTW_INFO(FUNC_ADPT_FMT" p=%u "MAC_FMT" %s (acl_num=%d)\n"
		 , FUNC_ADPT_ARG(adapter), period, MAC_ARG(addr)
		 , is_baddr ? "clear all" : (match ? "match" : "no found")
		 , acl->num);

exit:
	return ret;
}
#endif /* CONFIG_RTW_MACADDR_ACL */
#ifdef CONFIG_CMD_DISP
u8 rtw_ap_set_sta_key(_adapter *adapter, const u8 *addr, u8 alg, const u8 *key, u8 keyid, u8 gk)
{
	struct set_stakey_parm param;
	u8	res = _SUCCESS;

	_rtw_memcpy(param.addr, addr, ETH_ALEN);
	param.algorithm = alg;
	param.keyid = keyid;
	if (!!(alg & _SEC_TYPE_256_))
		_rtw_memcpy(param.key, key, 32);
	else
		_rtw_memcpy(param.key, key, 16);
	param.gk = gk;

	set_stakey_hdl(adapter, &param, PHL_CMD_NO_WAIT, 0);
exit:
	return res;
}

u8 rtw_ap_set_pairwise_key(_adapter *padapter, struct sta_info *psta)
{
	return rtw_ap_set_sta_key(padapter
		, psta->phl_sta->mac_addr
		, psta->dot118021XPrivacy
		, psta->dot118021x_UncstKey.skey
		, 0
		, 0
	);
}

static int rtw_ap_set_key(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			u8 *key, u8 alg, int keyid, u8 set_tx)
{
	u8 keylen;
	struct setkey_parm setkeyparm;
	int res = _SUCCESS;

	/* RTW_INFO("%s\n", __FUNCTION__); */

	_rtw_memset(&setkeyparm, 0, sizeof(struct setkey_parm));

	setkeyparm.keyid = (u8)keyid;
	if (is_wep_enc(alg))
		padapter->securitypriv.key_mask |= BIT(setkeyparm.keyid);

	setkeyparm.algorithm = alg;

	setkeyparm.set_tx = set_tx;

	switch (alg) {
	case _WEP40_:
		keylen = 5;
		break;
	case _WEP104_:
		keylen = 13;
		break;
	case _GCMP_256_:
	case _CCMP_256_:
		keylen = 32;
		break;
	case _TKIP_:
	case _TKIP_WTMIC_:
	case _AES_:
	case _GCMP_:
	#ifdef CONFIG_IEEE80211W
	case _BIP_CMAC_128_:
	#endif
	default:
		keylen = 16;
	}

	_rtw_memcpy(&(setkeyparm.key[0]), key, keylen);
	setkey_hdl(padapter, padapter_link, &setkeyparm, PHL_CMD_NO_WAIT, 0);

exit:
	return res;
}
#else /* CONFIG_FSM */
u8 rtw_ap_set_sta_key(_adapter *adapter, const u8 *addr, u8 alg, const u8 *key, u8 keyid, u8 gk)
{
	struct cmd_priv *cmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct cmd_obj *cmd;
	struct set_stakey_parm *param;
	u8	res = _SUCCESS;

	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmd->padapter = adapter;

	param = (struct set_stakey_parm *)rtw_zmalloc(sizeof(struct set_stakey_parm));
	if (param == NULL) {
		rtw_mfree((u8 *) cmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	init_h2fwcmd_w_parm_no_rsp(cmd, param, CMD_SET_STAKEY);

	_rtw_memcpy(param->addr, addr, ETH_ALEN);
	param->algorithm = alg;
	param->keyid = keyid;
	if (!!(alg & _SEC_TYPE_256_))
		_rtw_memcpy(param->key, key, 32);
	else
		_rtw_memcpy(param->key, key, 16);
	param->gk = gk;

	res = rtw_enqueue_cmd(cmdpriv, cmd);

exit:
	return res;
}

u8 rtw_ap_set_pairwise_key(_adapter *padapter, struct sta_info *psta)
{
	return rtw_ap_set_sta_key(padapter
		, psta->phl_sta->mac_addr
		, psta->dot118021XPrivacy
		, psta->dot118021x_UncstKey.skey
		, 0
		, 0
	);
}

static int rtw_ap_set_key(_adapter *padapter, u8 *key, u8 alg, int keyid, u8 set_tx)
{
	u8 keylen;
	struct cmd_obj *pcmd;
	struct setkey_parm *psetkeyparm;
	struct cmd_priv	*pcmdpriv = &(adapter_to_dvobj(padapter)->cmdpriv);
	int res = _SUCCESS;

	/* RTW_INFO("%s\n", __FUNCTION__); */

	pcmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	pcmd->padapter = padapter;

	psetkeyparm = (struct setkey_parm *)rtw_zmalloc(sizeof(struct setkey_parm));
	if (psetkeyparm == NULL) {
		rtw_mfree((unsigned char *)pcmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}


	psetkeyparm->keyid = (u8)keyid;
	if (is_wep_enc(alg))
		padapter->securitypriv.key_mask |= BIT(psetkeyparm->keyid);

	psetkeyparm->algorithm = alg;

	psetkeyparm->set_tx = set_tx;

	switch (alg) {
	case _WEP40_:
		keylen = 5;
		break;
	case _WEP104_:
		keylen = 13;
		break;
	case _GCMP_256_:
	case _CCMP_256_:
		keylen = 32;
		break;
	case _TKIP_:
	case _TKIP_WTMIC_:
	case _AES_:
	case _GCMP_:
	#ifdef CONFIG_IEEE80211W
	case _BIP_CMAC_128_:
	#endif
	default:
		keylen = 16;
	}

	_rtw_memcpy(&(psetkeyparm->key[0]), key, keylen);

	init_h2fwcmd_w_parm_no_rsp(pcmd, psetkeyparm, CMD_SET_KEY);

	res = rtw_enqueue_cmd(pcmdpriv, pcmd);

exit:

	return res;
}
#endif
int rtw_ap_set_group_key(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			u8 *key, u8 alg, int keyid)
{
	RTW_INFO("%s\n", __FUNCTION__);

	return rtw_ap_set_key(padapter, padapter_link, key, alg, keyid, 1);
}

int rtw_ap_set_wep_key(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		u8 *key, u8 keylen, int keyid, u8 set_tx)
{
	u8 alg;

	switch (keylen) {
	case 5:
		alg = _WEP40_;
		break;
	case 13:
		alg = _WEP104_;
		break;
	default:
		alg = _NO_PRIVACY_;
	}

	RTW_INFO("%s\n", __FUNCTION__);

	return rtw_ap_set_key(padapter, padapter_link, key, alg, keyid, set_tx);
}

u8 rtw_ap_bmc_frames_hdl(_adapter *padapter)
{
#define HIQ_XMIT_COUNTS (6)
	struct sta_info *psta_bmc;
	_list	*xmitframe_plist, *xmitframe_phead;
	struct xmit_frame *pxmitframe = NULL;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct sta_priv  *pstapriv = &padapter->stapriv;
	bool update_tim = _FALSE;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);


	if (padapter->registrypriv.wifi_spec != 1)
		return H2C_SUCCESS;


	psta_bmc = rtw_get_bcmc_stainfo(padapter, padapter_link);
	if (!psta_bmc)
		return H2C_SUCCESS;


	_rtw_spinlock_bh(&pxmitpriv->lock);

	if ((rtw_tim_map_is_set(padapter, pstapriv->tim_bitmap, 0)) && (psta_bmc->sleepq_len > 0)) {
		int tx_counts = 0;

		_update_beacon(padapter, padapter_link, _TIM_IE_, NULL, _FALSE, 0, "update TIM with TIB=1");

		RTW_INFO("sleepq_len of bmc_sta = %d\n", psta_bmc->sleepq_len);

		xmitframe_phead = get_list_head(&psta_bmc->sleep_q);
		xmitframe_plist = get_next(xmitframe_phead);

		while ((rtw_end_of_queue_search(xmitframe_phead, xmitframe_plist)) == _FALSE) {
			pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);

			xmitframe_plist = get_next(xmitframe_plist);

			rtw_list_delete(&pxmitframe->list);

			psta_bmc->sleepq_len--;
			tx_counts++;

			if (psta_bmc->sleepq_len > 0)
				pxmitframe->attrib.mdata = 1;
			else
				pxmitframe->attrib.mdata = 0;

			if (tx_counts == HIQ_XMIT_COUNTS)
				pxmitframe->attrib.mdata = 0;

			pxmitframe->attrib.triggered = 1;
			#if 0
			if (xmitframe_hiq_filter(pxmitframe) == _TRUE)
				pxmitframe->attrib.qsel = rtw_hal_get_qsel(padapter,QSLT_HIGH_ID);/*HIQ*/
			#endif

			rtw_intf_xmitframe_enqueue(padapter, pxmitframe);

			if (tx_counts == HIQ_XMIT_COUNTS)
				break;

		}

	} else {
		if (psta_bmc->sleepq_len == 0) {

			/*RTW_INFO("sleepq_len of bmc_sta = %d\n", psta_bmc->sleepq_len);*/

			if (rtw_tim_map_is_set(padapter, pstapriv->tim_bitmap, 0))
				update_tim = _TRUE;

			rtw_tim_map_clear(padapter, pstapriv->tim_bitmap, 0);
			rtw_tim_map_clear(padapter, pstapriv->sta_dz_bitmap, 0);

			if (update_tim == _TRUE) {
				RTW_INFO("clear TIB\n");
				_update_beacon(padapter, padapter_link, _TIM_IE_, NULL, _TRUE, 0, "bmc sleepq and HIQ empty");
			}
		}
	}

	_rtw_spinunlock_bh(&pxmitpriv->lock);

#if 0
	/* HIQ Check */
	rtw_hal_get_hwreg(padapter, HW_VAR_CHK_HI_QUEUE_EMPTY, &empty);

	while (_FALSE == empty && rtw_get_passing_time_ms(start) < 3000) {
		rtw_msleep_os(100);
		rtw_hal_get_hwreg(padapter, HW_VAR_CHK_HI_QUEUE_EMPTY, &empty);
	}


	printk("check if hiq empty=%d\n", empty);
#endif

	return H2C_SUCCESS;
}

#ifdef CONFIG_NATIVEAP_MLME

static void associated_stainfo_update(_adapter *padapter, struct sta_info *psta, u32 sta_info_type)
{
	struct link_mlme_priv *pmlmepriv = &(psta->padapter_link->mlmepriv);

	RTW_INFO("%s: "MAC_FMT", updated_type=0x%x\n", __func__, MAC_ARG(psta->phl_sta->mac_addr), sta_info_type);
#ifdef CONFIG_80211N_HT
	if (sta_info_type & STA_INFO_UPDATE_BW) {

		if ((psta->flags & WLAN_STA_HT) && !psta->ht_20mhz_set) {
			if (pmlmepriv->sw_to_20mhz) {
				psta->phl_sta->chandef.bw = CHANNEL_WIDTH_20;
				/*psta->htpriv.ch_offset = CHAN_OFFSET_NO_EXT;*/
				psta->htpriv.sgi_40m = _FALSE;
			} else {
				/*TODO: Switch back to 40MHZ?80MHZ*/
			}
		}
	}
#endif /* CONFIG_80211N_HT */
	/*
		if (sta_info_type & STA_INFO_UPDATE_RATE) {

		}
	*/

	if (sta_info_type & STA_INFO_UPDATE_PROTECTION_MODE)
		VCS_update(padapter, psta);

	/*
		if (sta_info_type & STA_INFO_UPDATE_CAP) {

		}

		if (sta_info_type & STA_INFO_UPDATE_HT_CAP) {

		}

		if (sta_info_type & STA_INFO_UPDATE_VHT_CAP) {

		}
	*/

}

static void update_bcn_erpinfo_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	unsigned char *p, *ie = pnetwork->IEs;
	u32 len = 0;

	RTW_INFO("%s, ERP_enable=%d\n", __FUNCTION__, pmlmeinfo->ERP_enable);

	if (!pmlmeinfo->ERP_enable)
		return;

	/* parsing ERP_IE */
	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _ERPINFO_IE_, &len, (pnetwork->IELength - _BEACON_IE_OFFSET_));
	if (p && len > 0) {
		PNDIS_802_11_VARIABLE_IEs pIE = (PNDIS_802_11_VARIABLE_IEs)p;

		if (pmlmepriv->num_sta_non_erp == 1)
			pIE->data[0] |= RTW_ERP_INFO_NON_ERP_PRESENT | RTW_ERP_INFO_USE_PROTECTION;
		else
			pIE->data[0] &= ~(RTW_ERP_INFO_NON_ERP_PRESENT | RTW_ERP_INFO_USE_PROTECTION);

		if (pmlmepriv->num_sta_no_short_preamble > 0)
			pIE->data[0] |= RTW_ERP_INFO_BARKER_PREAMBLE_MODE;
		else
			pIE->data[0] &= ~(RTW_ERP_INFO_BARKER_PREAMBLE_MODE);

		ERP_IE_handler(padapter, padapter_link, pIE);
	}

}

static void update_bcn_htcap_ie(_adapter *padapter)
{
	RTW_INFO("%s\n", __FUNCTION__);

}

static void update_bcn_htinfo_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
#ifdef CONFIG_80211N_HT
	/*
	u8 beacon_updated = _FALSE;
	u32 sta_info_update_type = STA_INFO_UPDATE_NONE;
	*/
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	unsigned char *p, *ie = pnetwork->IEs;
	u32 len = 0;

	if (pmlmepriv->htpriv.ht_option == _FALSE)
		return;

	if (pmlmeinfo->HT_info_enable != 1)
		return;


	RTW_INFO("%s current operation mode=0x%X\n",
		 __FUNCTION__, pmlmepriv->ht_op_mode);

	RTW_INFO("num_sta_40mhz_intolerant(%d), 20mhz_width_req(%d), intolerant_ch_rpt(%d), olbc(%d)\n",
		pmlmepriv->num_sta_40mhz_intolerant, pmlmepriv->ht_20mhz_width_req, pmlmepriv->ht_intolerant_ch_reported, ATOMIC_READ(&pmlmepriv->olbc));

	/*parsing HT_INFO_IE, currently only update ht_op_mode - pht_info->infos[1] & pht_info->infos[2] for wifi logo test*/
	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_ADD_INFO_IE_, &len, (pnetwork->IELength - _BEACON_IE_OFFSET_));
	if (p && len > 0) {
		struct HT_info_element *pht_info = NULL;

		pht_info = (struct HT_info_element *)(p + 2);

		/* for STA Channel Width/Secondary Channel Offset*/
		if ((pmlmepriv->sw_to_20mhz == 0) && (pmlmeext->chandef.chan <= 14)) {
			if ((pmlmepriv->num_sta_40mhz_intolerant > 0) || (pmlmepriv->ht_20mhz_width_req == _TRUE)
			    || (pmlmepriv->ht_intolerant_ch_reported == _TRUE) || (ATOMIC_READ(&pmlmepriv->olbc) == _TRUE)) {
				SET_HT_OP_ELE_2ND_CHL_OFFSET(pht_info, 0);
				SET_HT_OP_ELE_STA_CHL_WIDTH(pht_info, 0);

				pmlmepriv->sw_to_20mhz = 1;
				/*
				sta_info_update_type |= STA_INFO_UPDATE_BW;
				beacon_updated = _TRUE;
				*/

				RTW_INFO("%s:switching to 20Mhz\n", __FUNCTION__);

				/*TODO : cur_bwmode/cur_ch_offset switches to 20Mhz*/
			}
		} else {

			if ((pmlmepriv->num_sta_40mhz_intolerant == 0) && (pmlmepriv->ht_20mhz_width_req == _FALSE)
			    && (pmlmepriv->ht_intolerant_ch_reported == _FALSE) && (ATOMIC_READ(&pmlmepriv->olbc) == _FALSE)) {

				if (pmlmeext->chandef.bw >= CHANNEL_WIDTH_40) {

					SET_HT_OP_ELE_STA_CHL_WIDTH(pht_info, 1);

					SET_HT_OP_ELE_2ND_CHL_OFFSET(pht_info,
						(pmlmeext->chandef.offset == CHAN_OFFSET_UPPER) ?
						HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE : HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW);

					pmlmepriv->sw_to_20mhz = 0;
					/*
					sta_info_update_type |= STA_INFO_UPDATE_BW;
					beacon_updated = _TRUE;
					*/

					RTW_INFO("%s:switching back to 40Mhz\n", __FUNCTION__);
				}
			}
		}

		/* to update  ht_op_mode*/
		*(u16 *)(pht_info->infos + 1) = cpu_to_le16(pmlmepriv->ht_op_mode);

	}

	/*associated_clients_update(padapter, beacon_updated, sta_info_update_type);*/
#endif /* CONFIG_80211N_HT */
}

static void update_bcn_rsn_ie(_adapter *padapter)
{
	RTW_INFO("%s\n", __FUNCTION__);

}

static void update_bcn_wpa_ie(_adapter *padapter)
{
	RTW_INFO("%s\n", __FUNCTION__);

}

static void update_bcn_wmm_ie(_adapter *padapter)
{
	RTW_INFO("%s\n", __FUNCTION__);

}

static void update_bcn_wps_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	u8 *pwps_ie = NULL, *pwps_ie_src, *premainder_ie, *pbackup_remainder_ie = NULL;
	uint wps_ielen = 0, wps_offset, remainder_ielen;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(pmlmeinfo->network);
	unsigned char *ie = pnetwork->IEs;
	u32 ielen = pnetwork->IELength;


	RTW_INFO("%s\n", __FUNCTION__);

	pwps_ie = rtw_get_wps_ie(ie + _FIXED_IE_LENGTH_, ielen - _FIXED_IE_LENGTH_, NULL, &wps_ielen);

	if (pwps_ie == NULL || wps_ielen == 0)
		return;

	pwps_ie_src = pmlmepriv->wps_beacon_ie;
	if (pwps_ie_src == NULL)
		return;

	wps_offset = (uint)(pwps_ie - ie);

	premainder_ie = pwps_ie + wps_ielen;

	remainder_ielen = ielen - wps_offset - wps_ielen;

	if (remainder_ielen > 0) {
		pbackup_remainder_ie = rtw_malloc(remainder_ielen);
		if (pbackup_remainder_ie)
			_rtw_memcpy(pbackup_remainder_ie, premainder_ie, remainder_ielen);
	}

	wps_ielen = (uint)pwps_ie_src[1];/* to get ie data len */
	if ((wps_offset + wps_ielen + 2 + remainder_ielen) <= MAX_IE_SZ) {
		_rtw_memcpy(pwps_ie, pwps_ie_src, wps_ielen + 2);
		pwps_ie += (wps_ielen + 2);

		if (pbackup_remainder_ie)
			_rtw_memcpy(pwps_ie, pbackup_remainder_ie, remainder_ielen);

		/* update IELength */
		pnetwork->IELength = wps_offset + (wps_ielen + 2) + remainder_ielen;
	}

	if (pbackup_remainder_ie)
		rtw_mfree(pbackup_remainder_ie, remainder_ielen);

	/* deal with the case without set_tx_beacon_cmd() in rtw_update_beacon() */
#if defined(CONFIG_INTERRUPT_BASED_TXBCN) || defined(CONFIG_PCI_HCI)
	if ((padapter->mlmeextpriv.mlmext_info.state & 0x03) == WIFI_FW_AP_STATE) {
		u8 sr = 0;
		rtw_get_wps_attr_content(pwps_ie_src,  wps_ielen, WPS_ATTR_SELECTED_REGISTRAR, (u8 *)(&sr), NULL);

		if (sr) {
			set_fwstate(pmlmepriv, WIFI_UNDER_WPS);
			RTW_INFO("%s, set WIFI_UNDER_WPS\n", __func__);
		} else {
			clr_fwstate(pmlmepriv, WIFI_UNDER_WPS);
			RTW_INFO("%s, clr WIFI_UNDER_WPS\n", __func__);
		}
	}
#endif
}

static void update_bcn_p2p_ie(_adapter *padapter)
{

}

#ifdef CONFIG_ECSA_PHL
static bool _update_csa_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	struct core_ecsa_info *ecsa_info = &(padapter->ecsa_info);
	struct rtw_phl_ecsa_param *ecsa_param = &(ecsa_info->phl_ecsa_param);
	WLAN_BSSID_EX *pnetwork = &(padapter_link->mlmeextpriv.mlmext_info.network);
	u8 chan = ecsa_param->new_chan_def.chan;
	u8 count = ecsa_param->count;
	u8 *ies = pnetwork->IEs + _BEACON_IE_OFFSET_;
	sint ies_len = pnetwork->IELength - _BEACON_IE_OFFSET_;
	u8 *csa_ie;
	sint csa_ie_len;

	csa_ie = rtw_get_ie(ies, WLAN_EID_CHANNEL_SWITCH, &csa_ie_len, ies_len);

	if (csa_ie && csa_ie_len == CSA_IE_LEN) {
		if (count > 0) {
			/* update CSA IE */
			#ifdef DBG_CSA
			RTW_INFO("CSA : "FUNC_ADPT_FMT" ch=%u, count=%u, update CSA IE\n",
				FUNC_ADPT_ARG(padapter), chan, count);
			#endif

			csa_ie = csa_ie + 2;
			csa_ie[2] = count;
		} else {
			/* remove CSA IE */
			RTW_INFO("CSA : "FUNC_ADPT_FMT" ch=%u, count=%u, remove CSA IE\n",
				FUNC_ADPT_ARG(padapter), chan, count);

			rtw_remove_bcn_ie(padapter, pnetwork, WLAN_EID_CHANNEL_SWITCH);
			SET_ECSA_STATE(padapter, ECSA_ST_SW_DONE);
		}
		return _TRUE;
	} else {
		RTW_ERR("CSA : "FUNC_ADPT_FMT" unexpected case\n",
				FUNC_ADPT_ARG(padapter));
		return _FALSE;
	}
}

static bool _update_ecsa_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	struct core_ecsa_info *ecsa_info = &(padapter->ecsa_info);
	struct rtw_phl_ecsa_param *ecsa_param = &(ecsa_info->phl_ecsa_param);
	WLAN_BSSID_EX *pnetwork = &(padapter_link->mlmeextpriv.mlmext_info.network);
	u8 chan = ecsa_param->new_chan_def.chan;
	u8 count = ecsa_param->count;
	u8 *ies = pnetwork->IEs + _BEACON_IE_OFFSET_;
	sint ies_len = pnetwork->IELength - _BEACON_IE_OFFSET_;
	u8 *ecsa_ie;
	sint ecsa_ie_len;

	ecsa_ie = rtw_get_ie(ies, WLAN_EID_ECSA, &ecsa_ie_len, ies_len);

	if (ecsa_ie && ecsa_ie_len == ECSA_IE_LEN) {
		if (count > 0) {
			/* update CSA IE */
			#ifdef DBG_CSA
			RTW_INFO("CSA : "FUNC_ADPT_FMT" ch=%u, count=%u, update ECSA IE\n",
				FUNC_ADPT_ARG(padapter), chan, count);
			#endif
			ecsa_ie = ecsa_ie + 2;
			ecsa_ie[3] = count;
		} else {
			/* remove ECSA IE */
			RTW_INFO("CSA : "FUNC_ADPT_FMT" ch=%u, count=%u, remove ECSA IE\n",
				FUNC_ADPT_ARG(padapter), chan, count);

			rtw_remove_bcn_ie(padapter, pnetwork, WLAN_EID_ECSA);
			SET_ECSA_STATE(padapter, ECSA_ST_SW_DONE);
		}
		return _TRUE;
	} else {
		RTW_ERR("CSA : "FUNC_ADPT_FMT" unexpected case\n",
				FUNC_ADPT_ARG(padapter));
		return _FALSE;
	}
}
#endif

static u8 update_csa_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	u8 update = _FALSE;
#ifdef CONFIG_ECSA_PHL
	struct core_ecsa_info *ecsa_info = &(padapter->ecsa_info);
	struct rtw_phl_ecsa_param *ecsa_param = &(ecsa_info->phl_ecsa_param);

	if (!CHK_ECSA_STATE(padapter, ECSA_ST_SW_START))
		return _FALSE;

	update = _update_csa_ie(padapter, padapter_link);

	if (rtw_is_ecsa_enabled(padapter))
		update |= _update_ecsa_ie(padapter, padapter_link);

	if (ecsa_param->count > 0)
		ecsa_param->count--;

#endif /* CONFIG_ECSA_PHL */
	return update;
}

static void update_bcn_vendor_spec_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 *oui)
{
	RTW_INFO("%s\n", __FUNCTION__);

	if (_rtw_memcmp(RTW_WPA_OUI, oui, 4))
		update_bcn_wpa_ie(padapter);
	else if (_rtw_memcmp(WMM_OUI, oui, 4))
		update_bcn_wmm_ie(padapter);
	else if (_rtw_memcmp(WPS_OUI, oui, 4))
		update_bcn_wps_ie(padapter, padapter_link);
	else if (_rtw_memcmp(P2P_OUI, oui, 4))
		update_bcn_p2p_ie(padapter);
	else
		RTW_INFO("unknown OUI type!\n");


}

void _update_beacon(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		u8 ie_id, u8 *oui, u8 tx, u8 flags, const char *tag)
{
	struct link_mlme_priv *pmlmepriv;
	struct link_mlme_ext_priv *pmlmeext;
	bool updated = 1; /* treat as upadated by default */

	if (!padapter)
		return;

	pmlmepriv = &(padapter_link->mlmepriv);
	pmlmeext = &(padapter_link->mlmeextpriv);

	if (pmlmeext->bstart_bss == _FALSE)
		return;

	_rtw_spinlock_bh(&pmlmepriv->bcn_update_lock);

	switch (ie_id) {
	case _TIM_IE_:
		update_BCNTIM(padapter, padapter_link);
		break;

	case _ERPINFO_IE_:
		update_bcn_erpinfo_ie(padapter, padapter_link);
		break;

	case _HT_CAPABILITY_IE_:
		update_bcn_htcap_ie(padapter);
		break;

	case _RSN_IE_2_:
		update_bcn_rsn_ie(padapter);
		break;

	case _HT_ADD_INFO_IE_:
		update_bcn_htinfo_ie(padapter, padapter_link);
		break;

#ifdef CONFIG_RTW_MESH
	case WLAN_EID_MESH_CONFIG:
		updated = rtw_mesh_update_bss_peering_status(padapter, &(pmlmeext->mlmext_info.network));
		updated |= rtw_mesh_update_bss_formation_info(padapter, &(pmlmeext->mlmext_info.network));
		updated |= rtw_mesh_update_bss_forwarding_state(padapter, &(pmlmeext->mlmext_info.network));
		break;
#endif
	case WLAN_EID_CHANNEL_SWITCH:
		updated = update_csa_ie(padapter, padapter_link);
		break;

	case _VENDOR_SPECIFIC_IE_:
		update_bcn_vendor_spec_ie(padapter, padapter_link, oui);
		break;

	case 0xFF:
	default:
		break;
	}

	if (updated)
		pmlmepriv->update_bcn = _TRUE;

	_rtw_spinunlock_bh(&pmlmepriv->bcn_update_lock);

#ifndef CONFIG_INTERRUPT_BASED_TXBCN
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI) || defined(CONFIG_PCI_BCN_POLLING)
	if (tx && updated) {
		/* send_beacon(padapter); */ /* send_beacon must execute on TSR level */
		if (0)
			RTW_INFO(FUNC_ADPT_FMT" ie_id:%u - %s\n", FUNC_ADPT_ARG(padapter), ie_id, tag);
		if(flags == RTW_CMDF_WAIT_ACK)
			set_tx_beacon_cmd(padapter, padapter_link, RTW_CMDF_WAIT_ACK);
		else
			set_tx_beacon_cmd(padapter, padapter_link, 0);
	}
#else
	{
		/* PCI will issue beacon when BCN interrupt occurs.		 */
	}
#endif
#endif /* !CONFIG_INTERRUPT_BASED_TXBCN */
}

#ifdef CONFIG_80211N_HT

void rtw_process_public_act_bsscoex(_adapter *padapter, u8 *pframe, uint frame_len)
{
	struct sta_info *psta;
	struct sta_priv *pstapriv = &padapter->stapriv;
	u8 beacon_updated = _FALSE;
	struct link_mlme_priv *pmlmepriv;
	u8 *frame_body = pframe + sizeof(struct rtw_ieee80211_hdr_3addr);
	uint frame_body_len = frame_len - sizeof(struct rtw_ieee80211_hdr_3addr);
	u8 category, action;

	psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
	if (psta == NULL)
		return;
	pmlmepriv = &(psta->padapter_link->mlmepriv);


	category = frame_body[0];
	action = frame_body[1];

	if (frame_body_len > 0) {
		if ((frame_body[2] == EID_BSSCoexistence) && (frame_body[3] > 0)) {
			u8 ie_data = frame_body[4];

			if (ie_data & RTW_WLAN_20_40_BSS_COEX_40MHZ_INTOL) {
				if (psta->ht_40mhz_intolerant == 0) {
					psta->ht_40mhz_intolerant = 1;
					pmlmepriv->num_sta_40mhz_intolerant++;
					beacon_updated = _TRUE;
				}
			} else if (ie_data & RTW_WLAN_20_40_BSS_COEX_20MHZ_WIDTH_REQ)	{
				if (pmlmepriv->ht_20mhz_width_req == _FALSE) {
					pmlmepriv->ht_20mhz_width_req = _TRUE;
					beacon_updated = _TRUE;
				}
			} else
				beacon_updated = _FALSE;
		}
	}

	if (frame_body_len > 8) {
		/* if EID_BSSIntolerantChlReport ie exists */
		if ((frame_body[5] == EID_BSSIntolerantChlReport) && (frame_body[6] > 0)) {
			/*todo:*/
			if (pmlmepriv->ht_intolerant_ch_reported == _FALSE) {
				pmlmepriv->ht_intolerant_ch_reported = _TRUE;
				beacon_updated = _TRUE;
			}
		}
	}

	if (beacon_updated) {

		rtw_update_beacon(padapter, psta->padapter_link, _HT_ADD_INFO_IE_, NULL, _TRUE, 0);

		associated_stainfo_update(padapter, psta, STA_INFO_UPDATE_BW);
	}



}

void rtw_process_ht_action_smps(_adapter *padapter, u8 *ta, u8 ctrl_field)
{
	u8 e_field, m_field;
	struct sta_info *psta;
	struct sta_priv *pstapriv = &padapter->stapriv;

	psta = rtw_get_stainfo(pstapriv, ta);
	if (psta == NULL)
		return;

	e_field = (ctrl_field & BIT(0)) ? 1 : 0; /*SM Power Save Enabled*/
	m_field = (ctrl_field & BIT(1)) ? 1 : 0; /*SM Mode, 0:static SMPS, 1:dynamic SMPS*/

	if (e_field) {
		if (m_field) { /*mode*/
			psta->smps_mode = SM_PS_DYNAMIC;
			RTW_ERR("Don't support dynamic SMPS\n");
		}
		else
			psta->smps_mode = SM_PS_STATIC;
	} else {
		/*disable*/
		psta->smps_mode = SM_PS_DISABLE;
	}

	if (psta->smps_mode != SM_PS_DYNAMIC)
		rtw_ssmps_wk_cmd(padapter, psta, e_field, 1);
}

/*
op_mode
Set to 0 (HT pure) under the followign conditions
	- all STAs in the BSS are 20/40 MHz HT in 20/40 MHz BSS or
	- all STAs in the BSS are 20 MHz HT in 20 MHz BSS
Set to 1 (HT non-member protection) if there may be non-HT STAs
	in both the primary and the secondary channel
Set to 2 if only HT STAs are associated in BSS,
	however and at least one 20 MHz HT STA is associated
Set to 3 (HT mixed mode) when one or more non-HT STAs are associated
	(currently non-GF HT station is considered as non-HT STA also)
*/
int rtw_ht_operation_update(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	u16 cur_op_mode, new_op_mode;
	int op_mode_changes = 0;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct ht_priv	*phtpriv_ap = &pmlmepriv->htpriv;

	if (pmlmepriv->htpriv.ht_option == _FALSE)
		return 0;

	/*if (!iface->conf->ieee80211n || iface->conf->ht_op_mode_fixed)
		return 0;*/

	RTW_INFO("%s current operation mode=0x%X\n",
		 __FUNCTION__, pmlmepriv->ht_op_mode);

	if (!(pmlmepriv->ht_op_mode & HT_INFO_OPERATION_MODE_NON_GF_DEVS_PRESENT)
	    && pmlmepriv->num_sta_ht_no_gf) {
		pmlmepriv->ht_op_mode |=
			HT_INFO_OPERATION_MODE_NON_GF_DEVS_PRESENT;
		op_mode_changes++;
	} else if ((pmlmepriv->ht_op_mode &
		    HT_INFO_OPERATION_MODE_NON_GF_DEVS_PRESENT) &&
		   pmlmepriv->num_sta_ht_no_gf == 0) {
		pmlmepriv->ht_op_mode &=
			~HT_INFO_OPERATION_MODE_NON_GF_DEVS_PRESENT;
		op_mode_changes++;
	}

	if (!(pmlmepriv->ht_op_mode & HT_INFO_OPERATION_MODE_NON_HT_STA_PRESENT) &&
	    (pmlmepriv->num_sta_no_ht || ATOMIC_READ(&pmlmepriv->olbc_ht))) {
		pmlmepriv->ht_op_mode |= HT_INFO_OPERATION_MODE_NON_HT_STA_PRESENT;
		op_mode_changes++;
	} else if ((pmlmepriv->ht_op_mode &
		    HT_INFO_OPERATION_MODE_NON_HT_STA_PRESENT) &&
		   (pmlmepriv->num_sta_no_ht == 0 && !ATOMIC_READ(&pmlmepriv->olbc_ht))) {
		pmlmepriv->ht_op_mode &=
			~HT_INFO_OPERATION_MODE_NON_HT_STA_PRESENT;
		op_mode_changes++;
	}

	/* Note: currently we switch to the MIXED op mode if HT non-greenfield
	 * station is associated. Probably it's a theoretical case, since
	 * it looks like all known HT STAs support greenfield.
	 */
	new_op_mode = 0;
	if (pmlmepriv->num_sta_no_ht /*||
	    (pmlmepriv->ht_op_mode & HT_INFO_OPERATION_MODE_NON_GF_DEVS_PRESENT)*/)
		new_op_mode = OP_MODE_MIXED;
	else if ((phtpriv_ap->ht_cap.cap_info & IEEE80211_HT_CAP_SUP_WIDTH)
		 && pmlmepriv->num_sta_ht_20mhz)
		new_op_mode = OP_MODE_20MHZ_HT_STA_ASSOCED;
	else if (ATOMIC_READ(&pmlmepriv->olbc_ht))
		new_op_mode = OP_MODE_MAY_BE_LEGACY_STAS;
	else
		new_op_mode = OP_MODE_PURE;

	cur_op_mode = pmlmepriv->ht_op_mode & HT_INFO_OPERATION_MODE_OP_MODE_MASK;
	if (cur_op_mode != new_op_mode) {
		pmlmepriv->ht_op_mode &= ~HT_INFO_OPERATION_MODE_OP_MODE_MASK;
		pmlmepriv->ht_op_mode |= new_op_mode;
		op_mode_changes++;
	}

	RTW_INFO("%s new operation mode=0x%X changes=%d\n",
		 __FUNCTION__, pmlmepriv->ht_op_mode, op_mode_changes);

	return op_mode_changes;

}

#endif /* CONFIG_80211N_HT */

void associated_clients_update(_adapter *padapter, u8 updated, u32 sta_info_type)
{
	/* update associcated stations cap. */
	if (updated == _TRUE) {
		_list	*phead, *plist;
		struct sta_info *psta = NULL;
		struct sta_priv *pstapriv = &padapter->stapriv;

		_rtw_spinlock_bh(&pstapriv->asoc_list_lock);

		phead = &pstapriv->asoc_list;
		plist = get_next(phead);

		/* check asoc_queue */
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);

			plist = get_next(plist);

			associated_stainfo_update(padapter, psta, sta_info_type);
		}

		_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

	}

}

/* called > TSR LEVEL for USB or SDIO Interface*/
void bss_cap_update_on_sta_join(_adapter *padapter, struct sta_info *psta)
{
	u8 beacon_updated = _FALSE;
	struct registry_priv *pregistrypriv = &(padapter->registrypriv);
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);


#if 0
	if (!(psta->capability & WLAN_CAPABILITY_SHORT_PREAMBLE) &&
	    !psta->no_short_preamble_set) {
		psta->no_short_preamble_set = 1;
		pmlmepriv->num_sta_no_short_preamble++;
		if ((pmlmeext->cur_wireless_mode > WIRELESS_11B) &&
		    (pmlmepriv->num_sta_no_short_preamble == 1))
			ieee802_11_set_beacons(hapd->iface);
	}
#endif


	if (!(psta->flags & WLAN_STA_SHORT_PREAMBLE)) {
		if (!psta->no_short_preamble_set) {
			psta->no_short_preamble_set = 1;

			pmlmepriv->num_sta_no_short_preamble++;

			if ((pmlmeext->cur_wireless_mode > WLAN_MD_11B) &&
			    (pmlmepriv->num_sta_no_short_preamble == 1))
				beacon_updated = _TRUE;
		}
	} else {
		if (psta->no_short_preamble_set) {
			psta->no_short_preamble_set = 0;

			pmlmepriv->num_sta_no_short_preamble--;

			if ((pmlmeext->cur_wireless_mode > WLAN_MD_11B) &&
			    (pmlmepriv->num_sta_no_short_preamble == 0))
				beacon_updated = _TRUE;
		}
	}

#if 0
	if (psta->flags & WLAN_STA_NONERP && !psta->nonerp_set) {
		psta->nonerp_set = 1;
		pmlmepriv->num_sta_non_erp++;
		if (pmlmepriv->num_sta_non_erp == 1)
			ieee802_11_set_beacons(hapd->iface);
	}
#endif

	if (psta->flags & WLAN_STA_NONERP) {
		if (!psta->nonerp_set) {
			psta->nonerp_set = 1;

			pmlmepriv->num_sta_non_erp++;

			if (pmlmepriv->num_sta_non_erp == 1) {
				beacon_updated = _TRUE;
				rtw_update_beacon(padapter, padapter_link, _ERPINFO_IE_, NULL, _FALSE, 0);
			}
		}

	} else {
		if (psta->nonerp_set) {
			psta->nonerp_set = 0;

			pmlmepriv->num_sta_non_erp--;

			if (pmlmepriv->num_sta_non_erp == 0) {
				beacon_updated = _TRUE;
				rtw_update_beacon(padapter, padapter_link, _ERPINFO_IE_, NULL, _FALSE, 0);
			}
		}

	}


#if 0
	if (!(psta->capability & WLAN_CAPABILITY_SHORT_SLOT) &&
	    !psta->no_short_slot_time_set) {
		psta->no_short_slot_time_set = 1;
		pmlmepriv->num_sta_no_short_slot_time++;
		if ((pmlmeext->cur_wireless_mode > WIRELESS_11B) &&
		    (pmlmepriv->num_sta_no_short_slot_time == 1))
			ieee802_11_set_beacons(hapd->iface);
	}
#endif

	if (!(psta->capability & WLAN_CAPABILITY_SHORT_SLOT)) {
		if (!psta->no_short_slot_time_set) {
			psta->no_short_slot_time_set = 1;

			pmlmepriv->num_sta_no_short_slot_time++;

			if ((pmlmeext->cur_wireless_mode > WLAN_MD_11B) &&
			    (pmlmepriv->num_sta_no_short_slot_time == 1))
				beacon_updated = _TRUE;
		}
	} else {
		if (psta->no_short_slot_time_set) {
			psta->no_short_slot_time_set = 0;

			pmlmepriv->num_sta_no_short_slot_time--;

			if ((pmlmeext->cur_wireless_mode > WLAN_MD_11B) &&
			    (pmlmepriv->num_sta_no_short_slot_time == 0))
				beacon_updated = _TRUE;
		}
	}

#ifdef CONFIG_80211N_HT
	if(pregistrypriv->ht_enable &&
		is_supported_ht(pregistrypriv->wireless_mode)) {
		if (psta->flags & WLAN_STA_HT) {
			u16 ht_capab = le16_to_cpu(psta->htpriv.ht_cap.cap_info);

			RTW_INFO("HT: STA " MAC_FMT " HT Capabilities Info: 0x%04x\n",
				MAC_ARG(psta->phl_sta->mac_addr), ht_capab);

			if (psta->no_ht_set) {
				psta->no_ht_set = 0;
				pmlmepriv->num_sta_no_ht--;
			}

			if ((ht_capab & IEEE80211_HT_CAP_GRN_FLD) == 0) {
				if (!psta->no_ht_gf_set) {
					psta->no_ht_gf_set = 1;
					pmlmepriv->num_sta_ht_no_gf++;
				}
				RTW_INFO("%s STA " MAC_FMT " - no "
					 "greenfield, num of non-gf stations %d\n",
					 __FUNCTION__, MAC_ARG(psta->phl_sta->mac_addr),
					 pmlmepriv->num_sta_ht_no_gf);
			}

			if ((ht_capab & IEEE80211_HT_CAP_SUP_WIDTH) == 0) {
				if (!psta->ht_20mhz_set) {
					psta->ht_20mhz_set = 1;
					pmlmepriv->num_sta_ht_20mhz++;
				}
				RTW_INFO("%s STA " MAC_FMT " - 20 MHz HT, "
					 "num of 20MHz HT STAs %d\n",
					 __FUNCTION__, MAC_ARG(psta->phl_sta->mac_addr),
					 pmlmepriv->num_sta_ht_20mhz);
			}

			if (((ht_capab & RTW_IEEE80211_HT_CAP_40MHZ_INTOLERANT) != 0) &&
				(psta->ht_40mhz_intolerant == 0)) {
				psta->ht_40mhz_intolerant = 1;
				pmlmepriv->num_sta_40mhz_intolerant++;
				RTW_INFO("%s STA " MAC_FMT " - 40MHZ_INTOLERANT, ",
					   __FUNCTION__, MAC_ARG(psta->phl_sta->mac_addr));
			}

		} else {
			if (!psta->no_ht_set) {
				psta->no_ht_set = 1;
				pmlmepriv->num_sta_no_ht++;
			}
			if (pmlmepriv->htpriv.ht_option == _TRUE) {
				RTW_INFO("%s STA " MAC_FMT
					 " - no HT, num of non-HT stations %d\n",
					 __FUNCTION__, MAC_ARG(psta->phl_sta->mac_addr),
					 pmlmepriv->num_sta_no_ht);
			}
		}

		if (rtw_ht_operation_update(padapter, padapter_link) > 0)
		{
			rtw_update_beacon(padapter, padapter_link, _HT_CAPABILITY_IE_, NULL, _FALSE, 0);
			rtw_update_beacon(padapter, padapter_link, _HT_ADD_INFO_IE_, NULL, _FALSE, 0);
			beacon_updated = _TRUE;
		}
	}
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter)) {
		struct sta_priv *pstapriv = &padapter->stapriv;

		rtw_update_beacon(padapter, padapter_link, WLAN_EID_MESH_CONFIG, NULL, _FALSE, 0);
		if (pstapriv->asoc_list_cnt == 1)
			_set_timer(&padapter->mesh_atlm_param_req_timer, 0);
		beacon_updated = _TRUE;
	}
#endif

	if (beacon_updated)
		rtw_update_beacon(padapter, padapter_link, 0xFF, NULL, _TRUE, 0);

	/* update associcated stations cap. */
	associated_clients_update(padapter,  beacon_updated, STA_INFO_UPDATE_ALL);

	RTW_INFO("%s, updated=%d\n", __func__, beacon_updated);

}

u8 bss_cap_update_on_sta_leave(_adapter *padapter, struct sta_info *psta)
{
	u8 beacon_updated = _FALSE;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct link_mlme_priv *pmlmepriv = &(psta->padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &(psta->padapter_link->mlmeextpriv);

	if (!psta)
		return beacon_updated;

	if (rtw_tim_map_is_set(padapter, pstapriv->tim_bitmap, psta->phl_sta->aid)) {
		rtw_tim_map_clear(padapter, pstapriv->tim_bitmap, psta->phl_sta->aid);
		beacon_updated = _TRUE;
		rtw_update_beacon(padapter, psta->padapter_link, _TIM_IE_, NULL, _FALSE, 0);
	}

	if (psta->no_short_preamble_set) {
		psta->no_short_preamble_set = 0;
		pmlmepriv->num_sta_no_short_preamble--;
		if (pmlmeext->cur_wireless_mode > WLAN_MD_11B
		    && pmlmepriv->num_sta_no_short_preamble == 0)
			beacon_updated = _TRUE;
	}

	if (psta->nonerp_set) {
		psta->nonerp_set = 0;
		pmlmepriv->num_sta_non_erp--;
		if (pmlmepriv->num_sta_non_erp == 0) {
			beacon_updated = _TRUE;
			rtw_update_beacon(padapter, psta->padapter_link, _ERPINFO_IE_, NULL, _FALSE, 0);
		}
	}

	if (psta->no_short_slot_time_set) {
		psta->no_short_slot_time_set = 0;
		pmlmepriv->num_sta_no_short_slot_time--;
		if (pmlmeext->cur_wireless_mode > WLAN_MD_11B
		    && pmlmepriv->num_sta_no_short_slot_time == 0)
			beacon_updated = _TRUE;
	}

#ifdef CONFIG_80211N_HT
	if (psta->no_ht_gf_set) {
		psta->no_ht_gf_set = 0;
		pmlmepriv->num_sta_ht_no_gf--;
	}

	if (psta->no_ht_set) {
		psta->no_ht_set = 0;
		pmlmepriv->num_sta_no_ht--;
	}

	if (psta->ht_20mhz_set) {
		psta->ht_20mhz_set = 0;
		pmlmepriv->num_sta_ht_20mhz--;
	}

	if (psta->ht_40mhz_intolerant) {
		psta->ht_40mhz_intolerant = 0;
		if (pmlmepriv->num_sta_40mhz_intolerant > 0)
			pmlmepriv->num_sta_40mhz_intolerant--;
		else
			rtw_warn_on(1);
	}

	if (rtw_ht_operation_update(padapter, psta->padapter_link) > 0)
	{
		rtw_update_beacon(padapter, psta->padapter_link, _HT_CAPABILITY_IE_, NULL, _FALSE, 0);
		rtw_update_beacon(padapter, psta->padapter_link, _HT_ADD_INFO_IE_, NULL, _FALSE, 0);
	}
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter)) {
		rtw_update_beacon(padapter, psta->padapter_link, WLAN_EID_MESH_CONFIG, NULL, _FALSE, 0);
		if (pstapriv->asoc_list_cnt == 0)
			_cancel_timer_ex(&padapter->mesh_atlm_param_req_timer);
		beacon_updated = _TRUE;
	}
#endif

	if (beacon_updated == _TRUE)
		rtw_update_beacon(padapter, psta->padapter_link, 0xFF, NULL, _TRUE, 0);

#if 0
	/* update associated stations cap. */
	associated_clients_update(padapter,  beacon_updated, STA_INFO_UPDATE_ALL); /* move it to avoid deadlock */
#endif

	RTW_INFO("%s, updated=%d\n", __func__, beacon_updated);

	return beacon_updated;

}

u8 ap_free_sta(_adapter *padapter, struct sta_info *psta, bool active, u16 reason, bool enqueue, u8 disassoc)
{
	u8 beacon_updated = _FALSE;

	if (!psta)
		return beacon_updated;

	RTW_INFO("%s sta "MAC_FMT"\n", __func__, MAC_ARG(psta->phl_sta->mac_addr));

	if (active == _TRUE) {
#ifdef CONFIG_80211N_HT
		/* tear down Rx AMPDU */
		send_delba(padapter, 0, psta->phl_sta->mac_addr);/* recipient */

		/* tear down TX AMPDU */
		send_delba(padapter, 1, psta->phl_sta->mac_addr);/*  */ /* originator */

#endif /* CONFIG_80211N_HT */

		if (!MLME_IS_MESH(padapter)) {
			if (disassoc == _TRUE)
				issue_disassoc(padapter, psta->phl_sta->mac_addr, reason);
			else
				issue_deauth(padapter, psta->phl_sta->mac_addr, reason);
		}
	}

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter))
		rtw_mesh_path_flush_by_nexthop(psta);
#endif

#ifdef CONFIG_80211N_HT
	psta->ampdu_priv.agg_enable_bitmap = 0x0;/* reset */
	psta->ampdu_priv.candidate_tid_bitmap = 0x0;/* reset */
#endif

	_rtw_spinlock_bh(&psta->lock);
	psta->state &= ~(WIFI_ASOC_STATE | WIFI_UNDER_KEY_HANDSHAKE);

#ifdef CONFIG_IOCTL_CFG80211
	if ((psta->auth_len != 0) && (psta->pauth_frame != NULL)) {
		rtw_mfree(psta->pauth_frame, psta->auth_len);
		psta->pauth_frame = NULL;
		psta->auth_len = 0;
	}

	if (psta->passoc_req && psta->assoc_req_len > 0) {
		rtw_mfree(psta->passoc_req, psta->assoc_req_len);
		psta->passoc_req = NULL;
		psta->assoc_req_len = 0;
	}
#endif /* CONFIG_IOCTL_CFG80211 */
	_rtw_spinunlock_bh(&psta->lock);

	if (!MLME_IS_MESH(padapter)) {
		#ifdef CONFIG_RTW_WDS
		rtw_wds_path_flush_by_nexthop(psta);
		#endif

#ifdef CONFIG_IOCTL_CFG80211
		#ifdef COMPAT_KERNEL_RELEASE
		rtw_cfg80211_indicate_sta_disassoc(padapter, psta->phl_sta->mac_addr, reason);
		#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) && !defined(CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER)
		rtw_cfg80211_indicate_sta_disassoc(padapter, psta->phl_sta->mac_addr, reason);
		#else /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) && !defined(CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER) */
		/* will call rtw_cfg80211_indicate_sta_disassoc() in cmd_thread for old API context */
		#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 37)) && !defined(CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER) */
#else
		rtw_indicate_sta_disassoc_event(padapter, psta);
#endif
	}

	beacon_updated = bss_cap_update_on_sta_leave(padapter, psta);

	report_del_sta_event(padapter, psta->phl_sta->mac_addr, reason, enqueue, _FALSE);

	return beacon_updated;

}

int rtw_ap_inform_ch_switch(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 new_ch, u8 ch_offset)
{
	_list	*phead, *plist;
	int ret = 0;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	if ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)
		return ret;

	RTW_INFO(FUNC_NDEV_FMT" with ch:%u, offset:%u\n",
		 FUNC_NDEV_ARG(padapter->pnetdev), new_ch, ch_offset);

	_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);

	/* for each sta in asoc_queue */
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		issue_action_spct_ch_switch(padapter, padapter_link, psta->phl_sta->mac_addr, new_ch, ch_offset);
		psta->expire_to = ((pstapriv->expire_to * 2) > 5) ? 5 : (pstapriv->expire_to * 2);
	}
	_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

	issue_action_spct_ch_switch(padapter, padapter_link, bc_addr, new_ch, ch_offset);

	return ret;
}

int rtw_sta_flush(_adapter *padapter, bool enqueue)
{
	_list	*phead, *plist;
	int ret = 0;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 flush_num = 0;
	char flush_list[NUM_STA];
	int i;

	if (!MLME_IS_AP(padapter) && !MLME_IS_MESH(padapter))
		return ret;

	/* pick sta from sta asoc_queue */
	_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		int stainfo_offset;

		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		rtw_list_delete(&psta->asoc_list);
		pstapriv->asoc_list_cnt--;
		#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
		if (psta->tbtx_enable)
			pstapriv->tbtx_asoc_list_cnt--;
		#endif
		STA_SET_MESH_PLINK(psta, NULL);

		stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
		if (stainfo_offset_valid(stainfo_offset))
			flush_list[flush_num++] = stainfo_offset;
		else
			rtw_warn_on(1);
	}
	_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

	RTW_INFO(FUNC_NDEV_FMT" flush_num:%d\n", FUNC_NDEV_ARG(padapter->pnetdev), flush_num);

	/* call ap_free_sta() for each sta picked */
	for (i = 0; i < flush_num; i++) {
		u8 sta_addr[ETH_ALEN];

		psta = rtw_get_stainfo_by_offset(pstapriv, flush_list[i]);
		if (psta != NULL) {
			_rtw_memcpy(sta_addr, psta->phl_sta->mac_addr, ETH_ALEN);
			ap_free_sta(padapter, psta, _TRUE, WLAN_REASON_DEAUTH_LEAVING, enqueue, _FALSE);
		}
		#ifdef CONFIG_RTW_MESH
		if (MLME_IS_MESH(padapter))
			rtw_mesh_expire_peer(padapter, sta_addr);
		#endif
	}

	if (MLME_IS_ASOC(padapter) && !MLME_IS_MESH(padapter))
		issue_deauth(padapter, bc_addr, WLAN_REASON_DEAUTH_LEAVING);

	associated_clients_update(padapter, _TRUE, STA_INFO_UPDATE_ALL);

	return ret;
}

/* called > TSR LEVEL for USB or SDIO Interface*/
void sta_info_update(_adapter *padapter, struct sta_info *psta)
{
	int flags = psta->flags;
	struct link_mlme_priv *pmlmepriv = &(psta->padapter_link->mlmepriv);


	/* update wmm cap. */
	if (WLAN_STA_WME & flags)
		psta->qos_option = 1;
	else
		psta->qos_option = 0;

	if (pmlmepriv->qospriv.qos_option == 0)
		psta->qos_option = 0;


#ifdef CONFIG_80211N_HT
	/* update 802.11n ht cap. */
	if (WLAN_STA_HT & flags) {
		psta->htpriv.ht_option = _TRUE;
		psta->qos_option = 1;

		psta->smps_mode = (psta->htpriv.ht_cap.cap_info & IEEE80211_HT_CAP_SM_PS) >> 2;
	} else
		psta->htpriv.ht_option = _FALSE;

	if (pmlmepriv->htpriv.ht_option == _FALSE)
		psta->htpriv.ht_option = _FALSE;
#endif

#ifdef CONFIG_80211AC_VHT
	/* update 802.11AC vht cap. */
	if (WLAN_STA_VHT & flags)
		psta->vhtpriv.vht_option = _TRUE;
	else
		psta->vhtpriv.vht_option = _FALSE;

	if (pmlmepriv->vhtpriv.vht_option == _FALSE)
		psta->vhtpriv.vht_option = _FALSE;
#endif

#ifdef CONFIG_80211AX_HE
	/* update 802.11AX he cap. */
	if (WLAN_STA_HE & flags)
		psta->hepriv.he_option = _TRUE;
	else
		psta->hepriv.he_option = _FALSE;

	if (pmlmepriv->hepriv.he_option == _FALSE)
		psta->hepriv.he_option = _FALSE;

#if CONFIG_IEEE80211_BAND_6GHZ
	psta->smps_mode = GET_HE_6G_BAND_CAP_SM_PS(psta->hepriv.he_6g_band_cap + 1);
#endif
#endif

	update_sta_info_apmode(padapter, psta);
}

/* restore hw setting from sw data structures */
void rtw_ap_restore_network(_adapter *padapter)
{
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
	_list	*phead, *plist;
	u8 chk_alive_num = 0;
	char chk_alive_list[NUM_STA];
	int i;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_security_priv *psecuritypriv = &(padapter_link->securitypriv);

	rtw_setopmode_cmd(padapter
		, MLME_IS_AP(padapter) ? Ndis802_11APMode : Ndis802_11_mesh
		, RTW_CMDF_DIRECTLY
	);

	set_channel_bwmode(padapter, padapter_link,
			pmlmeext->chandef.chan,
			pmlmeext->chandef.offset,
			pmlmeext->chandef.bw,
			_FALSE);

	rtw_startbss_cmd(padapter, RTW_CMDF_DIRECTLY);

	if ((padapter->securitypriv.dot11PrivacyAlgrthm == _TKIP_) ||
	    (padapter->securitypriv.dot11PrivacyAlgrthm == _AES_)) {
		/* restore group key, WEP keys is restored in ips_leave() */
		rtw_set_key(padapter, padapter_link, psecuritypriv->dot118021XGrpKeyid, 0, _FALSE);
	}

	_rtw_spinlock_bh(&pstapriv->asoc_list_lock);

	phead = &pstapriv->asoc_list;
	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		int stainfo_offset;

		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
		if (stainfo_offset_valid(stainfo_offset))
			chk_alive_list[chk_alive_num++] = stainfo_offset;
	}

	_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

	for (i = 0; i < chk_alive_num; i++) {
		psta = rtw_get_stainfo_by_offset(pstapriv, chk_alive_list[i]);

		if (psta == NULL){
			RTW_INFO(FUNC_ADPT_FMT" sta_info is null\n", FUNC_ADPT_ARG(padapter));
		} else if (psta->state & WIFI_ASOC_STATE) {
			rtw_sta_media_status_rpt(padapter, psta, 1);
			/*
			rtw_phl_cmd_change_stainfo(adapter_to_dvobj(adapter)->phl,
						   sta->phl_sta,
						   STA_CHG_RAMASK,
						   NULL,
						   0,
						   PHL_CMD_DIRECTLY,
						   0);
			*/
			/* pairwise key */
			/* per sta pairwise key and settings */
			if ((padapter->securitypriv.dot11PrivacyAlgrthm == _TKIP_) ||
			    (padapter->securitypriv.dot11PrivacyAlgrthm == _AES_))
				rtw_setstakey_cmd(padapter, psta, UNICAST_KEY, _FALSE);
		}
	}

}

void start_ap_mode(_adapter *padapter)
{
	int i;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
#ifdef CONFIG_CONCURRENT_MODE
	struct security_priv *psecuritypriv = &padapter->securitypriv;
#endif
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	pmlmepriv->update_bcn = _FALSE;

	/*init_mlme_ap_info(padapter);*/

	pmlmeext->bstart_bss = _FALSE;

	pmlmepriv->num_sta_non_erp = 0;

	pmlmepriv->num_sta_no_short_slot_time = 0;

	pmlmepriv->num_sta_no_short_preamble = 0;

	pmlmepriv->num_sta_ht_no_gf = 0;
#ifdef CONFIG_80211N_HT
	pmlmepriv->num_sta_no_ht = 0;
#endif /* CONFIG_80211N_HT */
	pmlmeinfo->HT_info_enable = 0;
	pmlmeinfo->HT_caps_enable = 0;
	pmlmeinfo->HT_enable = 0;

	pmlmepriv->num_sta_ht_20mhz = 0;
	pmlmepriv->num_sta_40mhz_intolerant = 0;
	ATOMIC_SET(&pmlmepriv->olbc, _FALSE);
	ATOMIC_SET(&pmlmepriv->olbc_ht, _FALSE);

#ifdef CONFIG_80211N_HT
	pmlmepriv->ht_20mhz_width_req = _FALSE;
	pmlmepriv->ht_intolerant_ch_reported = _FALSE;
	pmlmepriv->ht_op_mode = 0;
	pmlmepriv->sw_to_20mhz = 0;
#endif

	_rtw_memset(pmlmepriv->ext_capab_ie_data, 0, sizeof(pmlmepriv->ext_capab_ie_data));
	pmlmepriv->ext_capab_ie_len = 0;

#ifdef CONFIG_CONCURRENT_MODE
	psecuritypriv->dot118021x_bmc_cam_id = INVALID_SEC_MAC_CAM_ID;
#endif

	for (i = 0 ;  i < pstapriv->max_aid; i++)
		pstapriv->sta_aid[i] = NULL;

#ifdef CONFIG_RTW_WDS
	if (MLME_IS_AP(padapter))
		rtw_wds_pathtbl_init(padapter);
#endif

	if (rtw_mi_check_status(padapter, MI_AP_MODE))
		RTW_SET_SCAN_BAND_SKIP(padapter, BAND_5G);
}

void stop_ap_mode(_adapter *padapter)
{
	u8 self_action = MLME_ACTION_UNKNOWN;
	struct sta_info *psta = NULL;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	int chanctx_num = 0;
	struct rtw_chan_def chan_def = {0};

	RTW_INFO("%s -"ADPT_FMT"\n", __func__, ADPT_ARG(padapter));

	if (MLME_IS_AP(padapter))
		self_action = MLME_AP_STOPPED;
	else if (MLME_IS_MESH(padapter))
		self_action = MLME_MESH_STOPPED;
	else
		rtw_warn_on(1);

	pmlmepriv->update_bcn = _FALSE;
	/*pmlmeext->bstart_bss = _FALSE;*/
	padapter->netif_up = _FALSE;
	/* _rtw_spinlock_free(&pmlmepriv->bcn_update_lock); */

	/* reset and init security priv , this can refine with rtw_reset_securitypriv */
	_rtw_memset((unsigned char *)&padapter->securitypriv, 0, sizeof(struct security_priv));
	padapter->securitypriv.ndisauthtype = Ndis802_11AuthModeOpen;
	padapter->securitypriv.ndisencryptstatus = Ndis802_11WEPDisabled;
	_rtw_memset((unsigned char *)&padapter_link->securitypriv, 0, sizeof(struct link_security_priv));

	rtw_rfctl_update_op_mode(adapter_to_rfctl(padapter), BIT(padapter->iface_id), 0);

	/* free scan queue */
	rtw_free_network_queue(padapter, _TRUE);
	rtw_free_mld_network_queue(padapter, _TRUE);

#if CONFIG_RTW_MACADDR_ACL
	rtw_macaddr_acl_clear(padapter, RTW_ACL_PERIOD_BSS);
#endif

	rtw_sta_flush(padapter, _FALSE);

#ifdef CONFIG_RTW_WDS
	adapter_set_use_wds(padapter, 0);
#endif
#ifdef CONFIG_RTW_MULTI_AP
	padapter->multi_ap = 0;
#endif
	rtw_free_mlme_priv_ie_data(&padapter->mlmepriv);

	pmlmeext->bstart_bss = _FALSE;

#ifdef CONFIG_RTW_MULTI_AP
	rtw_map_config_monitor(padapter, self_action);
#endif
#if 0
	chanctx_num = rtw_phl_chanctx_del(adapter_to_dvobj(padapter)->phl,
						padapter->phl_role, &chan_def);

	if (chanctx_num && chan_def.chan != 0)
		set_channel_bwmode(padapter, chan_def.chan, chan_def.offset, chan_def.bw, _FALSE);
#endif

#ifdef CONFIG_DFS_MASTER
	rtw_indicate_cac_state_on_bss_stop(padapter);
#endif

#ifdef CONFIG_RTW_WDS
	if (MLME_IS_AP(padapter))
		rtw_wds_pathtbl_unregister(padapter);
#endif
	_rtw_spinlock_free(&pmlmepriv->bcn_update_lock);
}

#endif /* CONFIG_NATIVEAP_MLME */

void rtw_ap_update_clients_rainfo(struct _ADAPTER *a, enum phl_cmd_type flag)
{
	struct dvobj_priv *d = adapter_to_dvobj(a);
	struct link_mlme_ext_priv *pmlmeext;
	struct sta_priv *pstapriv = &a->stapriv;
	struct sta_info *psta;
	const struct sta_info *ap_self_psta = rtw_get_stainfo(pstapriv, a->phl_role->mac_addr);
	_list *plist, *phead;
	u8 i;

	/* update RA mask of all clients */
	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);

		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);

			/* no need to update RA info of ap self */
			if (psta && psta != ap_self_psta) {
				pmlmeext = &psta->padapter_link->mlmeextpriv;
				psta->phl_sta->chandef.bw = pmlmeext->chandef.bw;
				rtw_phl_cmd_change_stainfo(GET_PHL_INFO(d),
							psta->phl_sta,
							STA_CHG_RAMASK,
							NULL,
							0,
							flag,
							0);
			}
		}
	}
	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);
}

void rtw_ap_update_bss_bchbw(_adapter *adapter, struct _ADAPTER_LINK *adapter_link,
			WLAN_BSSID_EX *bss, struct rtw_chan_def *chandef)
{
#define UPDATE_VHT_CAP 1
#define UPDATE_HT_CAP 1
	struct link_mlme_priv *mlmepriv = &adapter_link->mlmepriv;
#ifdef CONFIG_80211N_HT
	struct ht_priv *htpriv = &mlmepriv->htpriv;
#endif
#ifdef CONFIG_80211AC_VHT
	struct vht_priv *vhtpriv = &mlmepriv->vhtpriv;
#endif
	u8 *p;
	uint ie_len = 0;

#ifdef CONFIG_80211AX_HE
	struct he_priv *hepriv = &mlmepriv->hepriv;
	u8 *he_op_info = NULL;

	if ((hepriv->he_option == _TRUE) && (chandef->band == BAND_ON_6G)) {
		he_op_info = rtw_ies_get_he_6g_op_info_ie(
			bss->IEs + sizeof(NDIS_802_11_FIXED_IEs),
			bss->IELength - _FIXED_IE_LENGTH_);

		/* update channel in IE */
		if (he_op_info &&
		    (bss->Configuration.DSConfig != chandef->chan ||
		     bss->Configuration.Band != chandef->band)) {
			SET_HE_OP_INFO_PRIMARY_CHAN(he_op_info, chandef->chan);
			SET_HE_OP_INFO_CHAN_WIDTH(he_op_info, chandef->bw);
			SET_HE_OP_INFO_CENTER_FREQ_0(he_op_info, (u8)chandef->center_freq1);
			SET_HE_OP_INFO_CENTER_FREQ_1(he_op_info, (u8)chandef->center_freq2);
		}

		bss->Configuration.DSConfig = chandef->chan;

		if (bss->Configuration.Band != chandef->band) {
			bss->Configuration.Band = chandef->band;
			change_band_update_ie(adapter, adapter_link, bss, chandef);
		}

		/* TODO remove unnecessary IE ie., HT VHT IE */
		return;
	}
#endif
	/* update channel in IE */
	if (bss->Configuration.DSConfig != chandef->chan) {
		p = rtw_get_ie((bss->IEs + sizeof(NDIS_802_11_FIXED_IEs)), _DSSET_IE_,
			&ie_len, (bss->IELength - sizeof(NDIS_802_11_FIXED_IEs)));

		if (p && ie_len > 0)
			*(p + 2) = chandef->chan;

		bss->Configuration.DSConfig = chandef->chan;
	}

	/* band is changed, update ERP, support rate, ext support rate IE */
	if (bss->Configuration.Band != chandef->band) {
		bss->Configuration.Band = chandef->band;
		change_band_update_ie(adapter, adapter_link, bss, chandef);
	}

#ifdef CONFIG_80211AC_VHT
	if (vhtpriv->vht_option == _TRUE) {
		u8 *vht_cap_ie, *vht_op_ie;
		int vht_cap_ielen, vht_op_ielen;
		u8	center_freq;
		struct rtw_chan_def chdef = {0};

		vht_cap_ie = rtw_get_ie((bss->IEs + sizeof(NDIS_802_11_FIXED_IEs)), WLAN_EID_VHT_CAPABILITY, &vht_cap_ielen, (bss->IELength - sizeof(NDIS_802_11_FIXED_IEs)));
		vht_op_ie = rtw_get_ie((bss->IEs + sizeof(NDIS_802_11_FIXED_IEs)), WLAN_EID_VHT_OPERATION, &vht_op_ielen, (bss->IELength - sizeof(NDIS_802_11_FIXED_IEs)));
		chdef.chan = chandef->chan;
		chdef.bw = chandef->bw;
		chdef.offset = chandef->offset;
		chdef.band = chandef->band;
		center_freq = rtw_phl_get_center_ch(&chdef);

		/* update vht cap ie */
		if (vht_cap_ie && vht_cap_ielen) {
			#if UPDATE_VHT_CAP
			if ((chandef->bw == CHANNEL_WIDTH_160 || chandef->bw == CHANNEL_WIDTH_80_80) && vhtpriv->sgi_160m)
				SET_VHT_CAPABILITY_ELE_SHORT_GI160M(vht_cap_ie + 2, 1);
			else
				SET_VHT_CAPABILITY_ELE_SHORT_GI160M(vht_cap_ie + 2, 0);

			if (chandef->bw >= CHANNEL_WIDTH_80 && vhtpriv->sgi_80m)
				SET_VHT_CAPABILITY_ELE_SHORT_GI80M(vht_cap_ie + 2, 1);
			else
				SET_VHT_CAPABILITY_ELE_SHORT_GI80M(vht_cap_ie + 2, 0);
			#endif
		}

		/* update vht op ie */
		if (vht_op_ie && vht_op_ielen) {
			if (chandef->bw < CHANNEL_WIDTH_80) {
				RTW_INFO(FUNC_ADPT_FMT" update VHT 20/40M\n", FUNC_ADPT_ARG(adapter));
				SET_VHT_OPERATION_ELE_CHL_WIDTH(vht_op_ie + 2, CH_WIDTH_20_40M);
				SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(vht_op_ie + 2, 0);
				SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(vht_op_ie + 2, 0);
			} else if (chandef->bw == CHANNEL_WIDTH_80) {
				RTW_INFO(FUNC_ADPT_FMT" update VHT 80M, center_freq = %u\n", FUNC_ADPT_ARG(adapter), center_freq);
				SET_VHT_OPERATION_ELE_CHL_WIDTH(vht_op_ie + 2, CH_WIDTH_80_160M);
				SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(vht_op_ie + 2, center_freq);
				SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(vht_op_ie + 2, 0);
			} else {
				RTW_ERR(FUNC_ADPT_FMT" unsupported BW:%u\n", FUNC_ADPT_ARG(adapter), chandef->bw);
				rtw_warn_on(1);
			}
		}
	}
#endif /* CONFIG_80211AC_VHT */
#ifdef CONFIG_80211N_HT
	{
		u8 *ht_cap_ie, *ht_op_ie;
		int ht_cap_ielen, ht_op_ielen;

		ht_cap_ie = rtw_get_ie((bss->IEs + sizeof(NDIS_802_11_FIXED_IEs)), EID_HTCapability, &ht_cap_ielen, (bss->IELength - sizeof(NDIS_802_11_FIXED_IEs)));
		ht_op_ie = rtw_get_ie((bss->IEs + sizeof(NDIS_802_11_FIXED_IEs)), EID_HTInfo, &ht_op_ielen, (bss->IELength - sizeof(NDIS_802_11_FIXED_IEs)));

		/* update ht cap ie */
		if (ht_cap_ie && ht_cap_ielen) {
			#if UPDATE_HT_CAP
			if (chandef->bw >= CHANNEL_WIDTH_40)
				SET_HT_CAP_ELE_CHL_WIDTH(ht_cap_ie + 2, 1);
			else
				SET_HT_CAP_ELE_CHL_WIDTH(ht_cap_ie + 2, 0);

			if (chandef->bw >= CHANNEL_WIDTH_40 && htpriv->sgi_40m)
				SET_HT_CAP_ELE_SHORT_GI40M(ht_cap_ie + 2, 1);
			else
				SET_HT_CAP_ELE_SHORT_GI40M(ht_cap_ie + 2, 0);

			if (htpriv->sgi_20m)
				SET_HT_CAP_ELE_SHORT_GI20M(ht_cap_ie + 2, 1);
			else
				SET_HT_CAP_ELE_SHORT_GI20M(ht_cap_ie + 2, 0);
			#endif
		}

		/* update ht op ie */
		if (ht_op_ie && ht_op_ielen) {
			SET_HT_OP_ELE_PRI_CHL(ht_op_ie + 2, chandef->chan);
			switch (chandef->offset) {
			case CHAN_OFFSET_UPPER:
				SET_HT_OP_ELE_2ND_CHL_OFFSET(ht_op_ie + 2, IEEE80211_SCA);
				break;
			case CHAN_OFFSET_LOWER:
				SET_HT_OP_ELE_2ND_CHL_OFFSET(ht_op_ie + 2, IEEE80211_SCB);
				break;
			case CHAN_OFFSET_NO_EXT:
			default:
				break;
				SET_HT_OP_ELE_2ND_CHL_OFFSET(ht_op_ie + 2, IEEE80211_SCN);
			}

			if (chandef->bw >= CHANNEL_WIDTH_40)
				SET_HT_OP_ELE_STA_CHL_WIDTH(ht_op_ie + 2, 1);
			else
				SET_HT_OP_ELE_STA_CHL_WIDTH(ht_op_ie + 2, 0);
		}
	}
#endif /* CONFIG_80211N_HT */
}

static int rtw_ap_gen_HE_freq(enum band_type band, u8 ch, u8 bw, u8 offset, u8 *freq0, u8 *freq1)
{

	*freq0 = 0;
	*freq1 = 0;

	if (band != BAND_ON_6G)
		return -1;

	if (bw == CHANNEL_WIDTH_160) {
		*freq1 = rtw_get_center_ch_by_band(band, ch, bw, offset);
		*freq0 = rtw_get_center_ch_by_band(band, ch, CHANNEL_WIDTH_80, offset);
	} else {
		*freq0 = rtw_get_center_ch_by_band(band, ch, bw, offset);
		*freq1 = 0;
	}
	return 0;
}

static u8 rtw_ap_update_chbw_by_ifbmp(struct dvobj_priv *dvobj, u8 ifbmp
	, enum band_type cur_ie_band[], u8 cur_ie_ch[], u8 cur_ie_bw[], u8 cur_ie_offset[]
	, enum band_type dec_band[], u8 dec_ch[], u8 dec_bw[], u8 dec_offset[]
	, const char *caller)
{
	_adapter *iface;
	WLAN_BSSID_EX *network;
	u8 ifbmp_ch_changed = 0, freq0 = 0, freq1 = 0;
	int i;
	struct _ADAPTER_LINK *iface_link = NULL;
	struct link_mlme_ext_priv *mlmeext;

	for (i = 0; i < dvobj->iface_nums; i++) {
		if (!(ifbmp & BIT(i)) || !dvobj->padapters[i])
			continue;

		iface = dvobj->padapters[i];
		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		iface_link = GET_PRIMARY_LINK(iface);
		mlmeext = &(iface_link->mlmeextpriv);

		if (MLME_IS_ASOC(iface)) {
			RTW_INFO(FUNC_ADPT_FMT" %u,%u,%u,%u => %u,%u,%u,%u%s\n", caller, ADPT_ARG(iface)
				, mlmeext->chandef.band, mlmeext->chandef.chan, mlmeext->chandef.bw, mlmeext->chandef.offset
				, dec_band[i], dec_ch[i], dec_bw[i], dec_offset[i]
				, MLME_IS_OPCH_SW(iface) ? " OPCH_SW" : "");
		} else {
			RTW_INFO(FUNC_ADPT_FMT" %u,%u,%u,%u => %u,%u,%u,%u%s\n", caller, ADPT_ARG(iface)
				, cur_ie_band[i], cur_ie_ch[i], cur_ie_bw[i], cur_ie_offset[i]
				, dec_band[i], dec_ch[i], dec_bw[i], dec_offset[i]
				, MLME_IS_OPCH_SW(iface) ? " OPCH_SW" : "");
		}
	}

	for (i = 0; i < dvobj->iface_nums; i++) {
		if (!(ifbmp & BIT(i)) || !dvobj->padapters[i])
			continue;

		iface = dvobj->padapters[i];
		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		iface_link = GET_PRIMARY_LINK(iface);
		mlmeext = &(iface_link->mlmeextpriv);
		network = &(mlmeext->mlmext_info.network);

		/* ch setting differs from mlmeext.network IE */
		if (cur_ie_ch[i] != dec_ch[i]
			|| cur_ie_bw[i] != dec_bw[i]
			|| cur_ie_offset[i] != dec_offset[i])
			ifbmp_ch_changed |= BIT(i);

		/* ch setting differs from existing one */
		if (MLME_IS_ASOC(iface)
			&& (mlmeext->chandef.chan != dec_ch[i]
				|| mlmeext->chandef.band != dec_band[i]
				|| mlmeext->chandef.bw != dec_bw[i]
				|| mlmeext->chandef.offset != dec_offset[i])
		) {
			if (rtw_linked_check(iface) == _TRUE) {
				#ifdef CONFIG_SPCT_CH_SWITCH
				if (1)
					rtw_ap_inform_ch_switch(iface, iface_link, dec_ch[i], dec_offset[i]);
				else
				#endif
				#ifdef CONFIG_ECSA_PHL
				if (CHK_ECSA_STATE(iface, ECSA_ST_NONE))
				#endif
					rtw_sta_flush(iface, _FALSE);
			}
		}

		rtw_ap_gen_HE_freq(dec_band[i], dec_ch[i], dec_bw[i], dec_offset[i], &freq0,  &freq1);
		mlmeext->chandef.center_freq1 = (u32)freq0;
		mlmeext->chandef.center_freq2 = (u32)freq1;
		mlmeext->chandef.band = dec_band[i];
		mlmeext->chandef.chan = dec_ch[i];
		mlmeext->chandef.bw = dec_bw[i];
		mlmeext->chandef.offset = dec_offset[i];

		rtw_ap_update_bss_bchbw(iface, iface_link, network, &mlmeext->chandef);
	}

	return ifbmp_ch_changed;
}

static u8 rtw_ap_ch_specific_chk(_adapter *adapter, enum band_type band, u8 ch, u8 *bw, u8 *offset, const char *caller)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	struct rtw_chset *chset = adapter_to_chset(adapter);
	int ch_idx;
	u8 ret = _SUCCESS;

	ch_idx = rtw_chset_search_bch(chset, band, ch);
	if (ch_idx < 0) {
		RTW_WARN("%s ch:%u doesn't fit in chplan\n", caller, ch);
		ret = _FAIL;
		goto exit;
	}
	if (chset->chs[ch_idx].flags & RTW_CHF_NO_IR) {
		RTW_WARN("%s ch:%u is passive\n", caller, ch);
		ret = _FAIL;
		goto exit;
	}

	rtw_adjust_bchbw(adapter, band, ch, bw, offset);

	if (!rtw_get_offset_by_bchbw(band, ch, *bw, offset)) {
		RTW_WARN("%s %u, %u,%u has no valid offset\n", caller, band, ch, *bw);
		ret = _FAIL;
		goto exit;
	}

	while (!rtw_chset_is_bchbw_valid(chset, band, ch, *bw, *offset, 0, 0)
		|| (!rtw_rfctl_radar_detect_supported(rfctl) && rtw_chset_is_dfs_bchbw(chset, band, ch, *bw, *offset))
	) {
		if (*bw > CHANNEL_WIDTH_20)
			(*bw)--;
		if (*bw == CHANNEL_WIDTH_20) {
			*offset = CHAN_OFFSET_NO_EXT;
			break;
		}
	}


	if (!rtw_rfctl_radar_detect_supported(rfctl) && rtw_chset_is_dfs_bchbw(chset, band, ch, *bw, *offset)) {
		RTW_WARN("%s DFS channel %u can't be used\n", caller, ch);
		ret = _FAIL;
		goto exit;
	}

exit:
	return ret;
}

u8 rtw_ap_bchbw_decision(_adapter *adapter, u8 ifbmp, u8 excl_ifbmp, s8 req_band
	, s16 req_ch, s8 req_bw, s8 req_offset, struct rtw_chan_def *chdef)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct rtw_chset *chset = adapter_to_chset(adapter);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	bool ch_avail = _FALSE;
	enum band_type cur_ie_band[CONFIG_IFACE_NUMBER] = {BAND_ON_24G};
	u8 cur_ie_freq0[CONFIG_IFACE_NUMBER] = {0};
	u8 cur_ie_freq1[CONFIG_IFACE_NUMBER] = {0};
	u8 cur_ie_ch[CONFIG_IFACE_NUMBER] = {0};
	u8 cur_ie_bw[CONFIG_IFACE_NUMBER] = {0};
	u8 cur_ie_offset[CONFIG_IFACE_NUMBER] = {0};
	enum band_type dec_band[CONFIG_IFACE_NUMBER] = {BAND_ON_24G};
	u8 dec_hwband[CONFIG_IFACE_NUMBER] = {0};
	u8 dec_ch[CONFIG_IFACE_NUMBER] = {0};
	u8 dec_bw[CONFIG_IFACE_NUMBER] = {0};
	u8 dec_offset[CONFIG_IFACE_NUMBER] = {0};
	u8 u_ch = 0, u_bw = 0, u_offset = 0;
	enum band_type u_band = BAND_ON_24G;
	WLAN_BSSID_EX *network;
	struct mi_state mstate;
	struct mi_state mstate_others;
	bool set_u_ch = _FALSE;
	u8 ifbmp_others = 0xFF & ~ifbmp & ~excl_ifbmp;
	u8 ifbmp_ch_changed = 0;
	bool ifbmp_all_mesh = 0;
	_adapter *iface;
	int i;
#ifdef CONFIG_MCC_MODE
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);
	u8 mcc_sup = phl_com->dev_cap.mcc_sup;
#endif
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	struct _ADAPTER_LINK *iface_link;
	struct link_mlme_ext_priv *mlmeext;
#ifdef CONFIG_DBCC_SUPPORT
	enum phl_band_idx hw_band = adapter_link->wrlink->hw_band;

	dec_hwband[adapter->iface_id] = hw_band;
#endif

#ifdef CONFIG_RTW_MESH
	for (i = 0; i < dvobj->iface_nums; i++)
		if ((ifbmp & BIT(i)) && dvobj->padapters[i])
			if (!MLME_IS_MESH(dvobj->padapters[i]))
				break;
	ifbmp_all_mesh = i >= dvobj->iface_nums ? 1 : 0;
#endif

	RTW_INFO("%s ifbmp:0x%02x excl_ifbmp:0x%02x req:%d,%d,%d\n", __func__
		, ifbmp, excl_ifbmp, req_ch, req_bw, req_offset);
	rtw_mi_status_by_ifbmp(adapter, ifbmp, &mstate);
	rtw_mi_status_by_ifbmp(adapter, ifbmp_others, &mstate_others);
	RTW_INFO("%s others ld_sta_num:%u, lg_sta_num:%u, ap_num:%u, mesh_num:%u\n"
		, __func__, MSTATE_STA_LD_NUM(&mstate_others), MSTATE_STA_LG_NUM(&mstate_others)
		, MSTATE_AP_NUM(&mstate_others), MSTATE_MESH_NUM(&mstate_others));

	for (i = 0; i < dvobj->iface_nums; i++) {
		if (!(ifbmp & BIT(i)) || !dvobj->padapters[i])
			continue;
		iface = dvobj->padapters[i];
		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		iface_link = GET_PRIMARY_LINK(iface);
		mlmeext = &(iface_link->mlmeextpriv);
		network = &(mlmeext->mlmext_info.network);

		/* get current IE channel settings */
		rtw_ies_get_bchbw(BSS_EX_TLV_IES(network), BSS_EX_TLV_IES_LEN(network)
			, &cur_ie_band[i] , &cur_ie_ch[i] , &cur_ie_bw[i]
			, &cur_ie_offset[i], &cur_ie_freq0[i], &cur_ie_freq1[i], 1, 1, 1);

		/* prepare temporary channel setting decision */
		if (req_ch == 0) {
			/* request comes from upper layer, use cur_ie values */
			dec_band[i] = cur_ie_band[i];
			dec_ch[i] = cur_ie_ch[i];
			dec_bw[i] = cur_ie_bw[i];
			dec_offset[i] = cur_ie_offset[i];
		} else {
			/* use chbw of cur_ie updated with specifying req as temporary decision */

			if (req_ch <= REQ_CH_NONE) {
				dec_ch[i] = cur_ie_ch[i];
				dec_band[i] = cur_ie_band[i];
			} else {
				dec_ch[i] = req_ch;
				dec_band[i] = req_band;
			}

			if (req_bw <= REQ_BW_NONE) {
				if (req_bw == REQ_BW_ORI)
					dec_bw[i] = iface_link->mlmepriv.ori_chandef.bw;
				else
					dec_bw[i] = cur_ie_bw[i];
			} else {
				dec_bw[i] = req_bw;
			}

			dec_offset[i] = (req_offset <= REQ_OFFSET_NONE) ? cur_ie_offset[i] : req_offset;
			#ifdef CONFIG_DBCC_SUPPORT
			dec_hwband[i] = iface_link->wrlink->hw_band;
			#endif
		}
	}

	if (MSTATE_STA_LD_NUM(&mstate_others) || MSTATE_STA_LG_NUM(&mstate_others)
		|| MSTATE_AP_NUM(&mstate_others) || MSTATE_MESH_NUM(&mstate_others)
	) {
		/* has linked/linking STA or has AP/Mesh mode */
		rtw_warn_on(!rtw_mi_get_ch_setting_union_by_ifbmp(adapter, ifbmp_others, &u_band, &u_ch, &u_bw, &u_offset));
		RTW_INFO("%s others union:%u,%u,%u,%u\n", __func__, u_band, u_ch, u_bw, u_offset);
	}

#ifdef CONFIG_MCC_MODE
	if (mcc_sup && req_ch == 0) {
		/* will disable MCC in ap starter prepare after v10 merge */
		if (rtw_hw_mcc_chk_inprogress(adapter, adapter_link)) {
			u8 if_id = adapter->iface_id;

			mlmeext = &(adapter_link->mlmeextpriv);
			/* check channel settings are the same */
			if (cur_ie_ch[if_id] == mlmeext->chandef.chan
				&& cur_ie_band[if_id] == mlmeext->chandef.band
				&& cur_ie_bw[if_id] == mlmeext->chandef.bw
				&& cur_ie_offset[if_id] == mlmeext->chandef.offset) {

				RTW_INFO(FUNC_ADPT_FMT"req ch settings are the same as current ch setting, go to exit\n"
					, FUNC_ADPT_ARG(adapter));
				goto exit;
			} else {
				RTW_WARN(FUNC_ADPT_FMT"request channel settings are different from current channel setting(%d,%d,%d,%d,%d,%d), restart MCC\n"
					, FUNC_ADPT_ARG(adapter)
					, cur_ie_ch[if_id], cur_ie_bw[if_id], cur_ie_offset[if_id]
					, mlmeext->chandef.chan, mlmeext->chandef.bw, mlmeext->chandef.offset);
				#if 0
				rtw_hal_set_mcc_setting_disconnect(adapter);
				#endif
			}
		}
	}
#endif /* CONFIG_MCC_MODE */

	if (MSTATE_STA_LG_NUM(&mstate_others) || MSTATE_STA_LD_NUM(&mstate_others)
		|| MSTATE_AP_NUM(&mstate_others) || MSTATE_MESH_NUM(&mstate_others)
	) {
		/* has linked or linking STA mode or AP/Mesh mode */

		for (i = 0; i < dvobj->iface_nums; i++) {
			if (!(ifbmp & BIT(i)) || !dvobj->padapters[i])
				continue;
			iface = dvobj->padapters[i];

			if (!rtw_iface_at_same_hwband(adapter, iface))
				continue;

			/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
			iface_link = GET_PRIMARY_LINK(iface);
			#ifdef CONFIG_MCC_MODE
			if (mcc_sup) {
				if (!rtw_is_bchbw_grouped(u_band, u_ch, u_bw, u_offset,
					dec_band[i],  dec_ch[i], dec_bw[i], dec_offset[i])) {
					mlmeext = &(iface_link->mlmeextpriv);
					mlmeext->chandef.chan = chdef->chan = dec_ch[i];
					mlmeext->chandef.bw = chdef->bw = dec_bw[i];
					mlmeext->chandef.offset = chdef->offset = dec_offset[i];
					mlmeext->chandef.band = chdef->band = dec_band[i];

					/* Trigger CSA should update ifbmp_ch_changed */
					#ifdef CONFIG_ECSA_PHL
					if (CHK_ECSA_STATE(iface, ECSA_ST_SW_START) &&
					   (cur_ie_ch[i] != dec_ch[i]
					   || cur_ie_band[i] != dec_band[i]
					   || cur_ie_bw[i] != dec_bw[i]
					   || cur_ie_offset[i] != dec_offset[i])) {
						ifbmp_ch_changed |= BIT(i);
						RTW_INFO("CSA : "FUNC_ADPT_FMT" : Trigger CSA in mcc mode\n",
							FUNC_ADPT_ARG(iface));
					}

					RTW_INFO(FUNC_ADPT_FMT" enable mcc: %u,%u,%u,%u\n", FUNC_ADPT_ARG(iface)
						 , chdef->band, chdef->chan, chdef->bw, chdef->offset);
					#endif
					goto exit;
				}
			}
			#endif /* CONFIG_MCC_MODE */

			rtw_adjust_bchbw(iface, u_band, u_ch, &dec_bw[i], &dec_offset[i]);
			#ifdef CONFIG_RTW_MESH
			if (MLME_IS_MESH(iface))
				rtw_mesh_adjust_bchbw(u_band, u_ch, &dec_bw[i], &dec_offset[i]);
			#endif

			if (req_ch == 0 && dec_bw[i] > u_bw
				&& rtw_chset_is_dfs_bchbw(chset, u_band, u_ch, u_bw, u_offset)
			) {
				/* request comes from upper layer, prevent from additional channel waiting */
				dec_bw[i] = u_bw;
				if (dec_bw[i] == CHANNEL_WIDTH_20)
					dec_offset[i] = CHAN_OFFSET_NO_EXT;
			}

			/* follow */
			rtw_chset_sync_bchbw(chset
				, &dec_band[i], &dec_ch[i], &dec_bw[i], &dec_offset[i]
				, &u_band, &u_ch, &u_bw, &u_offset, 1, 0);
		}

		set_u_ch = _TRUE;

	} else {
		/* autonomous decision */
		u8 ori_ch = 0;
		u8 ori_band = 0;
		u8 max_bw;
		bool by_int_info;

		if (req_ch <= REQ_CH_NONE) /* channel is not specified */
			goto choose_chbw;

		/* get tmp dec union of ifbmp */
		for (i = 0; i < dvobj->iface_nums; i++) {
			if (!(ifbmp & BIT(i)) || !dvobj->padapters[i])
				continue;
			#ifdef CONFIG_DBCC_SUPPORT
			if (dec_hwband[i] != hw_band)
				continue;
			#endif

			if (u_ch == 0) {
				u_band = dec_band[i];
				u_ch = dec_ch[i];
				u_bw = dec_bw[i];
				u_offset = dec_offset[i];
				rtw_adjust_bchbw(adapter, u_band, u_ch, &u_bw, &u_offset);
				rtw_get_offset_by_bchbw(u_band, u_ch, u_bw, &u_offset);
			} else {
				enum band_type tmp_band = dec_band[i];
				u8 tmp_ch = dec_ch[i];
				u8 tmp_bw = dec_bw[i];
				u8 tmp_offset = dec_offset[i];

				rtw_adjust_bchbw(adapter, tmp_band, tmp_ch, &tmp_bw, &tmp_offset);
				rtw_get_offset_by_bchbw(tmp_band, tmp_ch, tmp_bw, &u_offset);
				rtw_warn_on(!rtw_is_bchbw_grouped(u_band, u_ch, u_bw, u_offset, tmp_band, tmp_ch, tmp_bw, tmp_offset));
				rtw_sync_bchbw(&tmp_band, &tmp_ch, &tmp_bw, &tmp_offset, &u_band, &u_ch, &u_bw, &u_offset);
			}
		}

		#ifdef CONFIG_RTW_MESH
		/* if ifbmp are all mesh, apply bw restriction */
		if (ifbmp_all_mesh)
			rtw_mesh_adjust_bchbw(u_band, u_ch, &u_bw, &u_offset);
		#endif

		RTW_INFO("%s ifbmp:0x%02x tmp union:%u,%u,%u,%u\n", __func__, ifbmp, u_band, u_ch, u_bw, u_offset);

		/* check if tmp dec union is usable */
		if (rtw_ap_ch_specific_chk(adapter, u_band, u_ch, &u_bw, &u_offset, __func__) == _FAIL) {
			/* channel can't be used */
			if (req_ch > 0) {
				/* specific channel and not from IE => don't change channel setting */
				goto exit;
			}
			goto choose_chbw;
		} else if (rtw_chset_is_bchbw_non_ocp(chset, u_band, u_ch, u_bw, u_offset)) {
			RTW_WARN("%s DFS channel %u,%u under non ocp\n", __func__, u_ch, u_bw);
			if (req_ch > 0 && req_bw > REQ_BW_NONE) {
				/* change_chbw with specific channel and specific bw, goto update_bss_chbw directly */
				goto update_bss_chbw;
			}
		} else
			goto update_bss_chbw;

choose_chbw:
		by_int_info = req_ch == REQ_CH_INT_INFO ? 1 : 0;
		if (req_band <= REQ_BAND_NONE || req_band >= BAND_MAX) {
			/* band is unspecified/invalid means channel is also unspecified */
			req_ch = 0;
			req_band = BAND_MAX;
		} else if (req_ch <= REQ_CH_NONE)
			req_ch = 0;
		max_bw = req_bw > REQ_BW_NONE ? req_bw : CHANNEL_WIDTH_20;
		for (i = 0; i < dvobj->iface_nums; i++) {
			if (!(ifbmp & BIT(i)) || !dvobj->padapters[i])
				continue;
			iface = dvobj->padapters[i];

			if (!rtw_iface_at_same_hwband(adapter, iface))
				continue;

			/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
			iface_link = GET_PRIMARY_LINK(iface);
			mlmeext = &(iface_link->mlmeextpriv);

			if (req_bw <= REQ_BW_NONE) {
				if (req_bw == REQ_BW_ORI) {
					if (max_bw < iface_link->mlmepriv.ori_chandef.bw)
						max_bw = iface_link->mlmepriv.ori_chandef.bw;
				} else {
					if (max_bw < cur_ie_bw[i])
						max_bw = cur_ie_bw[i];
				}
			}

			if (MSTATE_AP_NUM(&mstate) || MSTATE_MESH_NUM(&mstate)) {
				if (ori_ch == 0) {
					ori_band = mlmeext->chandef.band;
					ori_ch = mlmeext->chandef.chan;
				} else if (ori_ch != mlmeext->chandef.chan) {
					rtw_warn_on(1);
				}
			} else {
				if (ori_ch == 0) {
					ori_band = cur_ie_band[i];
					ori_ch = cur_ie_ch[i];
				} else if (ori_ch != cur_ie_ch[i]) {
					rtw_warn_on(1);
				}
			}
		}

		ch_avail = rtw_rfctl_choose_bchbw(rfctl
			, req_band, req_ch, max_bw
			, ori_band, ori_ch
			, &u_band, &u_ch, &u_bw
			, &u_offset, by_int_info, ifbmp_all_mesh, __func__);
		if (ch_avail == _FALSE)
			goto exit;

update_bss_chbw:
		for (i = 0; i < dvobj->iface_nums; i++) {
			if (!(ifbmp & BIT(i)) || !dvobj->padapters[i])
				continue;
			iface = dvobj->padapters[i];

			if (!rtw_iface_at_same_hwband(adapter, iface))
				continue;

			dec_ch[i] = u_ch;
			dec_band[i] = u_band;
			if (dec_bw[i] > u_bw)
				dec_bw[i] = u_bw;
			if (dec_bw[i] == CHANNEL_WIDTH_20)
				dec_offset[i] = CHAN_OFFSET_NO_EXT;
			else
				dec_offset[i] = u_offset;

			#ifdef CONFIG_RTW_MESH
			if (MLME_IS_MESH(iface))
				rtw_mesh_adjust_bchbw(dec_band[i], dec_ch[i], &dec_bw[i], &dec_offset[i]);
			#endif
		}

		set_u_ch = _TRUE;
	}

	ifbmp_ch_changed = rtw_ap_update_chbw_by_ifbmp(dvobj, ifbmp
							, cur_ie_band, cur_ie_ch, cur_ie_bw, cur_ie_offset
							, dec_band, dec_ch, dec_bw, dec_offset
							, __func__);

	if (u_ch != 0)
		RTW_INFO("%s union:%u,%u,%u,%u\n", __func__, u_band, u_ch, u_bw, u_offset);

	if (rtw_mi_check_fwstate(adapter, WIFI_UNDER_SURVEY)) {
		/* scanning, leave ch setting to scan state machine */
		set_u_ch = _FALSE;
		RTW_INFO("%s, %d NG union:%u,%u,%u,%u\n"
		, __FUNCTION__, __LINE__, u_band, u_ch, u_bw, u_offset);
	}

	if (set_u_ch == _TRUE) {
		chdef->band = u_band;
		chdef->chan = u_ch;
		chdef->bw = u_bw;
		chdef->offset= u_offset;
	}
exit:
	return ifbmp_ch_changed;
}

u8 rtw_ap_sta_states_check(_adapter *adapter)
{
	struct sta_info *psta;
	struct sta_priv *pstapriv = &adapter->stapriv;
	_list *plist, *phead;
	u8 rst = _FALSE;

	if (!MLME_IS_AP(adapter) && !MLME_IS_MESH(adapter))
		return _FALSE;

	if (pstapriv->auth_list_cnt !=0)
		return _TRUE;

	_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {

		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		if (!(psta->state & WIFI_ASOC_STATE)) {
			RTW_INFO(ADPT_FMT"- SoftAP/Mesh - sta under linking, its state = 0x%x\n", ADPT_ARG(adapter), psta->state);
			rst = _TRUE;
			break;
		} else if (psta->state & WIFI_UNDER_KEY_HANDSHAKE) {
			RTW_INFO(ADPT_FMT"- SoftAP/Mesh - sta under key handshaking, its state = 0x%x\n", ADPT_ARG(adapter), psta->state);
			rst = _TRUE;
			break;
		}
	}
	_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);
	return rst;
}

void rtw_ap_parse_sta_capability(_adapter *adapter, struct sta_info *sta, u8 *cap)
{
	sta->capability = RTW_GET_LE16(cap);
	if (sta->capability & WLAN_CAPABILITY_SHORT_PREAMBLE)
		sta->flags |= WLAN_STA_SHORT_PREAMBLE;
	else
		sta->flags &= ~WLAN_STA_SHORT_PREAMBLE;
}

u16 rtw_ap_parse_sta_supported_rates(_adapter *adapter, struct sta_info *sta, u8 *tlv_ies, u16 tlv_ies_len)
{
	u8 rate_set[12];
	u8 rate_num;
	int i;
	u16 status = _STATS_SUCCESSFUL_;

	rtw_ies_get_supported_rate(tlv_ies, tlv_ies_len, rate_set, &rate_num);
	if (rate_num == 0) {
		RTW_INFO(FUNC_ADPT_FMT" sta "MAC_FMT" with no supported rate\n"
			, FUNC_ADPT_ARG(adapter), MAC_ARG(sta->phl_sta->mac_addr));
		status = _STATS_FAILURE_;
		goto exit;
	}

	_rtw_memcpy(sta->bssrateset, rate_set, rate_num);
	sta->bssratelen = rate_num;

	if (MLME_IS_AP(adapter)) {
		/* this function force only CCK rates to be bassic rate... */
		UpdateBrateTblForSoftAP(sta->bssrateset, sta->bssratelen);
	}

	/* if (hapd->iface->current_mode->mode == HOSTAPD_MODE_IEEE80211G) */ /* ? */
	sta->flags |= WLAN_STA_NONERP;
	for (i = 0; i < sta->bssratelen; i++) {
		if ((sta->bssrateset[i] & 0x7f) > 22) {
			sta->flags &= ~WLAN_STA_NONERP;
			break;
		}
	}

exit:
	return status;
}

u16 rtw_ap_parse_sta_security_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems)
{
	struct security_priv *sec = &adapter->securitypriv;
	u8 *wpa_ie;
	int wpa_ie_len;
	int group_cipher = 0, pairwise_cipher = 0, gmcs = 0;
	u32 akm = 0;
	u8 mfp_opt = MFP_NO;
	u8 spp_opt = 0;
	u16 status = _STATS_SUCCESSFUL_;

	sta->dot8021xalg = 0;
	sta->wpa_psk = 0;
	sta->wpa_group_cipher = 0;
	sta->wpa2_group_cipher = 0;
	sta->wpa_pairwise_cipher = 0;
	sta->wpa2_pairwise_cipher = 0;
	sta->akm_suite_type = 0;
	_rtw_memset(sta->wpa_ie, 0, sizeof(sta->wpa_ie));

	if ((sec->wpa_psk & BIT(1)) && elems->rsn_ie) {
		wpa_ie = elems->rsn_ie;
		wpa_ie_len = elems->rsn_ie_len;

		if (rtw_parse_wpa2_ie(wpa_ie - 2, wpa_ie_len + 2, &group_cipher, &pairwise_cipher, &gmcs, &akm, &mfp_opt, &spp_opt) == _SUCCESS) {
			sta->dot8021xalg = 1;/* psk, todo:802.1x */
			sta->wpa_psk |= BIT(1);

			/* RSN optional field absent; the validation is already checked in rtw_rsne_info_parse() */
			if (!group_cipher) {
				RTW_INFO("STA lacks WPA2 Group Suite Cipher --> Default\n");
				group_cipher = sec->wpa2_group_cipher;
			}
			if (!pairwise_cipher) {
				RTW_INFO("STA lacks WPA2 Pairwise Suite Cipher --> Default\n");
				pairwise_cipher = sec->wpa2_pairwise_cipher;
			}
			if (!akm) {
				RTW_INFO("STA lacks WPA2 AKM Cipher --> Default\n");
				akm = sec->akmp;
			}

			sta->wpa2_group_cipher = group_cipher & sec->wpa2_group_cipher;
			sta->wpa2_pairwise_cipher = pairwise_cipher & sec->wpa2_pairwise_cipher;
			sta->akm_suite_type = akm;

			if (MLME_IS_AP(adapter) && (CHECK_BIT(WLAN_AKM_TYPE_SAE, akm)) && (MFP_NO == mfp_opt)) {
				status = WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION;
				goto exit;
			}

			/* RSN optional field exists but no match */
			if (!sta->wpa2_group_cipher) {
				status = WLAN_STATUS_GROUP_CIPHER_NOT_VALID;
				goto exit;
			}
			if (!sta->wpa2_pairwise_cipher) {
				status = WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID;
				goto exit;
			}
			if (MLME_IS_AP(adapter) && (!CHECK_BIT(sec->akmp, akm))) {
				status = WLAN_STATUS_AKMP_NOT_VALID;
				goto exit;
			}
		}
		else {
			status = WLAN_STATUS_INVALID_IE;
			goto exit;
		}
	}
	else if ((sec->wpa_psk & BIT(0)) && elems->wpa_ie) {
		wpa_ie = elems->wpa_ie;
		wpa_ie_len = elems->wpa_ie_len;

		if (rtw_parse_wpa_ie(wpa_ie - 2, wpa_ie_len + 2, &group_cipher, &pairwise_cipher, NULL) == _SUCCESS) {
			sta->dot8021xalg = 1;/* psk, todo:802.1x */
			sta->wpa_psk |= BIT(0);

			sta->wpa_group_cipher = group_cipher & sec->wpa_group_cipher;
			sta->wpa_pairwise_cipher = pairwise_cipher & sec->wpa_pairwise_cipher;

			if (!sta->wpa_group_cipher) {
				status = WLAN_STATUS_GROUP_CIPHER_NOT_VALID;
				goto exit;
			}

			if (!sta->wpa_pairwise_cipher) {
				status = WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID;
				goto exit;
			}
		} else {
			status = WLAN_STATUS_INVALID_IE;
			goto exit;
		}

	} else {
		wpa_ie = NULL;
		wpa_ie_len = 0;
	}

	if (sec->dot11PrivacyAlgrthm != _NO_PRIVACY_) {
		/*check if amsdu is allowed */
		if (rtw_check_amsdu_disable(adapter->registrypriv.amsdu_mode, spp_opt) == _TRUE)
			sta->flags |= WLAN_STA_AMSDU_DISABLE;
	}
	if ((sec->mfp_opt == MFP_REQUIRED && mfp_opt < MFP_OPTIONAL)
		|| (mfp_opt == MFP_REQUIRED && sec->mfp_opt < MFP_OPTIONAL)
	) {
		status = WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION;
		goto exit;
	}

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter)) {
		/* MFP is mandatory for secure mesh */
		if (adapter->mesh_info.mesh_auth_id)
			sta->flags |= WLAN_STA_MFP;
	} else
#endif
	if (sec->mfp_opt >= MFP_OPTIONAL && mfp_opt >= MFP_OPTIONAL)
		sta->flags |= WLAN_STA_MFP;

#ifdef CONFIG_IEEE80211W
	if ((sta->flags & WLAN_STA_MFP)
		&& (sec->mfp_opt >= MFP_OPTIONAL && mfp_opt >= MFP_OPTIONAL)
		&& security_type_bip_to_gmcs(sec->dot11wCipher) != gmcs
	) {
		status = WLAN_STATUS_CIPHER_REJECTED_PER_POLICY;
		goto exit;
	}
#endif

#ifdef CONFIG_IOCTL_CFG80211
	if (MLME_IS_AP(adapter) &&
		(sec->auth_type == MLME_AUTHTYPE_SAE) &&
		(CHECK_BIT(WLAN_AKM_TYPE_SAE, sta->akm_suite_type)) &&
		(WLAN_AUTH_OPEN == sta->authalg)) {
		/* WPA3-SAE, PMK caching */
		if (rtw_cached_pmkid(adapter, sta->phl_sta->mac_addr) == -1) {
			RTW_INFO("SAE: No PMKSA cache entry found\n");
			status = WLAN_STATUS_INVALID_PMKID;
			goto exit;
		} else {
			RTW_INFO("SAE: PMKSA cache entry found\n");
		}
	}
#endif /* CONFIG_IOCTL_CFG80211 */

	if (!MLME_IS_AP(adapter))
		goto exit;

	sta->flags &= ~(WLAN_STA_WPS | WLAN_STA_MAYBE_WPS);
	/* if (hapd->conf->wps_state && wpa_ie == NULL) { */ /* todo: to check ap if supporting WPS */
	if (wpa_ie == NULL) {
		if (elems->wps_ie) {
			RTW_INFO("STA included WPS IE in "
				 "(Re)Association Request - assume WPS is "
				 "used\n");
			sta->flags |= WLAN_STA_WPS;
			/* wpabuf_free(sta->wps_ie); */
			/* sta->wps_ie = wpabuf_alloc_copy(elems.wps_ie + 4, */
			/*				elems.wps_ie_len - 4); */
		} else {
			RTW_INFO("STA did not include WPA/RSN IE "
				 "in (Re)Association Request - possible WPS "
				 "use\n");
			sta->flags |= WLAN_STA_MAYBE_WPS;
		}

		/* AP support WPA/RSN, and sta is going to do WPS, but AP is not ready */
		/* that the selected registrar of AP is _FLASE */
		if ((sec->wpa_psk > 0)
			&& (sta->flags & (WLAN_STA_WPS | WLAN_STA_MAYBE_WPS))
		) {
			struct mlme_priv *mlme = &adapter->mlmepriv;

			if (mlme->wps_beacon_ie) {
				u8 selected_registrar = 0;

				rtw_get_wps_attr_content(mlme->wps_beacon_ie, mlme->wps_beacon_ie_len, WPS_ATTR_SELECTED_REGISTRAR, &selected_registrar, NULL);

				if (!selected_registrar) {
					RTW_INFO("selected_registrar is _FALSE , or AP is not ready to do WPS\n");
					status = _STATS_UNABLE_HANDLE_STA_;
					goto exit;
				}
			}
		}

	} else {
		int copy_len;

		if (sec->wpa_psk == 0) {
			RTW_INFO("STA " MAC_FMT
				": WPA/RSN IE in association request, but AP don't support WPA/RSN\n",
				MAC_ARG(sta->phl_sta->mac_addr));
			status = WLAN_STATUS_INVALID_IE;
			goto exit;
		}

		if (elems->wps_ie) {
			RTW_INFO("STA included WPS IE in "
				 "(Re)Association Request - WPS is "
				 "used\n");
			sta->flags |= WLAN_STA_WPS;
			copy_len = 0;
		} else
			copy_len = ((wpa_ie_len + 2) > sizeof(sta->wpa_ie)) ? (sizeof(sta->wpa_ie)) : (wpa_ie_len + 2);

		if (copy_len > 0)
			_rtw_memcpy(sta->wpa_ie, wpa_ie - 2, copy_len);
	}

exit:
	return status;
}

void rtw_ap_parse_sta_wmm_ie(_adapter *adapter, struct sta_info *sta, u8 *tlv_ies, u16 tlv_ies_len)
{
	struct link_mlme_priv *mlme = &sta->padapter_link->mlmepriv;
	unsigned char WMM_IE[] = {0x00, 0x50, 0xf2, 0x02, 0x00, 0x01};
	u8 *p;

	sta->flags &= ~WLAN_STA_WME;
	sta->qos_option = 0;
	sta->qos_info = 0;
	sta->has_legacy_ac = _TRUE;
	sta->uapsd_vo = 0;
	sta->uapsd_vi = 0;
	sta->uapsd_be = 0;
	sta->uapsd_bk = 0;

	if (!mlme->qospriv.qos_option)
		goto exit;

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter)) {
		/* QoS is mandatory in mesh */
		sta->flags |= WLAN_STA_WME;
	}
#endif

	p = rtw_get_ie_ex(tlv_ies, tlv_ies_len, WLAN_EID_VENDOR_SPECIFIC, WMM_IE, 6, NULL, NULL);
	if (!p)
		goto exit;

	sta->flags |= WLAN_STA_WME;
	sta->qos_option = 1;
	sta->qos_info = *(p + 8);
	sta->max_sp_len = (sta->qos_info >> 5) & 0x3;

	if ((sta->qos_info & 0xf) != 0xf)
		sta->has_legacy_ac = _TRUE;
	else
		sta->has_legacy_ac = _FALSE;

	if (sta->qos_info & 0xf) {
		if (sta->qos_info & BIT(0))
			sta->uapsd_vo = BIT(0) | BIT(1);
		else
			sta->uapsd_vo = 0;

		if (sta->qos_info & BIT(1))
			sta->uapsd_vi = BIT(0) | BIT(1);
		else
			sta->uapsd_vi = 0;

		if (sta->qos_info & BIT(2))
			sta->uapsd_bk = BIT(0) | BIT(1);
		else
			sta->uapsd_bk = 0;

		if (sta->qos_info & BIT(3))
			sta->uapsd_be = BIT(0) | BIT(1);
		else
			sta->uapsd_be = 0;
	}

exit:
	return;
}

void rtw_ap_parse_sta_ht_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems)
{
	struct link_mlme_priv *mlme = &sta->padapter_link->mlmepriv;

	sta->flags &= ~WLAN_STA_HT;

#ifdef CONFIG_80211N_HT
	if (mlme->htpriv.ht_option == _FALSE)
		goto exit;

	/* save HT capabilities in the sta object */
	_rtw_memset(&sta->htpriv.ht_cap, 0, sizeof(struct rtw_ieee80211_ht_cap));
	if (elems->ht_capabilities && elems->ht_capabilities_len >= sizeof(struct rtw_ieee80211_ht_cap)) {
		sta->flags |= WLAN_STA_HT;
		sta->flags |= WLAN_STA_WME;
		_rtw_memcpy(&sta->htpriv.ht_cap, elems->ht_capabilities, sizeof(struct rtw_ieee80211_ht_cap));

		if (elems->ht_operation && elems->ht_operation_len == HT_OP_IE_LEN) {
			_rtw_memcpy(sta->htpriv.ht_op, elems->ht_operation, HT_OP_IE_LEN);
			sta->htpriv.op_present = 1;
		}
	}
exit:
#endif

	return;
}

void rtw_ap_parse_sta_vht_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems)
{
	struct link_mlme_priv *mlme = &sta->padapter_link->mlmepriv;

	sta->flags &= ~WLAN_STA_VHT;

#ifdef CONFIG_80211AC_VHT
	if (mlme->vhtpriv.vht_option == _FALSE)
		goto exit;

	_rtw_memset(&sta->vhtpriv, 0, sizeof(struct vht_priv));
	if (elems->vht_capabilities && elems->vht_capabilities_len == VHT_CAP_IE_LEN) {
		sta->flags |= WLAN_STA_VHT;
		_rtw_memcpy(sta->vhtpriv.vht_cap, elems->vht_capabilities, VHT_CAP_IE_LEN);

		if (elems->vht_operation && elems->vht_operation_len== VHT_OP_IE_LEN) {
			_rtw_memcpy(sta->vhtpriv.vht_op, elems->vht_operation, VHT_OP_IE_LEN);
			sta->vhtpriv.op_present = 1;
		}

		if (elems->vht_op_mode_notify && elems->vht_op_mode_notify_len == 1) {
			_rtw_memcpy(&sta->vhtpriv.vht_op_mode_notify, elems->vht_op_mode_notify, 1);
			sta->vhtpriv.notify_present = 1;
		}
	}
exit:
#endif

	return;
}

void rtw_ap_parse_sta_he_ie(_adapter *adapter, struct sta_info *sta, struct rtw_ieee802_11_elems *elems)
{
	struct link_mlme_priv *mlme = &sta->padapter_link->mlmepriv;

	sta->flags &= ~WLAN_STA_HE;

#ifdef CONFIG_80211AX_HE
	if (mlme->hepriv.he_option == _FALSE)
		goto exit;

	_rtw_memset(&sta->hepriv, 0, sizeof(struct he_priv));
	if (elems->he_capabilities && (elems->he_capabilities_len <= HE_CAP_ELE_MAX_LEN)) {
		sta->flags |= WLAN_STA_HE;
		_rtw_memcpy(sta->hepriv.he_cap, elems->he_capabilities, elems->he_capabilities_len);

		if (elems->he_operation && (elems->he_operation_len <= HE_OPER_ELE_MAX_LEN)) {
			_rtw_memcpy(sta->hepriv.he_op, elems->he_operation, elems->he_operation_len);
			sta->hepriv.op_present = 1;
		}

#if 0
		if (elems->vht_op_mode_notify && elems->vht_op_mode_notify_len == 1) {
			_rtw_memcpy(&sta->vhtpriv.vht_op_mode_notify, elems->vht_op_mode_notify, 1);
			sta->vhtpriv.notify_present = 1;
		}
#endif
	}
#if CONFIG_IEEE80211_BAND_6GHZ
	if (elems->he_6g_band_cap && (elems->he_6g_band_cap_len == HE_6G_BAND_CAP_MAX_LEN))
		_rtw_memcpy(sta->hepriv.he_6g_band_cap, elems->he_6g_band_cap, elems->he_6g_band_cap_len);
#endif
exit:
#endif

	return;
}

void rtw_ap_parse_sta_multi_ap_ie(_adapter *adapter, struct sta_info *sta, u8 *ies, int ies_len)
{
	sta->flags &= ~WLAN_STA_MULTI_AP;

#ifdef CONFIG_RTW_MULTI_AP
	if (adapter->multi_ap
		&& (rtw_get_multi_ap_ie_ext(ies, ies_len) & MULTI_AP_BACKHAUL_STA)
	) {
		if (adapter->multi_ap & MULTI_AP_BACKHAUL_BSS) /* with backhaul bss, enable WDS */
			sta->flags |= WLAN_STA_MULTI_AP | WLAN_STA_WDS;
		else if (adapter->multi_ap & MULTI_AP_FRONTHAUL_BSS) /* fronthaul bss only */
			sta->flags |= WLAN_STA_MULTI_AP;
	}
#endif
}

#if CONFIG_RTW_AP_DATA_BMC_TO_UC
static bool rtw_ap_data_bmc_to_uc(_adapter *adapter
	, const u8 *da, const u8 *sa, const u8 *ori_ta
	, u16 os_qid, _list *b2u_list)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	struct xmit_priv *xmitpriv = &adapter->xmitpriv;
	_list *head, *list;
	struct sta_info *sta;
	char b2u_sta_id[NUM_STA];
	u8 b2u_sta_num = 0;
	bool bmc_need = _FALSE;
	int i;

	 _rtw_spinlock_bh(&stapriv->asoc_list_lock);
	head = &stapriv->asoc_list;
	list = get_next(head);

	while ((rtw_end_of_queue_search(head, list)) == _FALSE) {
		int stainfo_offset;

		sta = LIST_CONTAINOR(list, struct sta_info, asoc_list);
		list = get_next(list);

		stainfo_offset = rtw_stainfo_offset(stapriv, sta);
		if (stainfo_offset_valid(stainfo_offset))
			b2u_sta_id[b2u_sta_num++] = stainfo_offset;
	}
	_rtw_spinunlock_bh(&stapriv->asoc_list_lock);

	if (!b2u_sta_num)
		goto exit;

	for (i = 0; i < b2u_sta_num; i++) {
		struct xmit_frame *b2uframe = NULL;
		struct pkt_attrib *attrib;

		sta = rtw_get_stainfo_by_offset(stapriv, b2u_sta_id[i]);
		if (!(sta->state & WIFI_ASOC_STATE)
			|| _rtw_memcmp(sta->phl_sta->mac_addr, sa, ETH_ALEN) == _TRUE
			|| (ori_ta && _rtw_memcmp(sta->phl_sta->mac_addr, ori_ta, ETH_ALEN) == _TRUE)
			|| is_broadcast_mac_addr(sta->phl_sta->mac_addr)
			|| is_zero_mac_addr(sta->phl_sta->mac_addr))
			continue;

		if (core_tx_alloc_xmitframe(adapter, &b2uframe, os_qid) == FAIL) {
			bmc_need = _TRUE;
			break;
		}

		attrib = &b2uframe->attrib;

		_rtw_memcpy(attrib->ra, sta->phl_sta->mac_addr, ETH_ALEN);
		_rtw_memcpy(attrib->ta, sta->padapter_link->mac_addr, ETH_ALEN);
		#ifdef CONFIG_RTW_WDS
		if (adapter_use_wds(adapter) && (sta->flags & WLAN_STA_WDS)) {
			_rtw_memcpy(attrib->dst, da, ETH_ALEN);
			attrib->wds = 1;
		} else
		#endif
			_rtw_memcpy(attrib->dst, attrib->ra, ETH_ALEN);
		_rtw_memcpy(attrib->src, sa, ETH_ALEN);

		rtw_list_insert_tail(&b2uframe->list, b2u_list);
	}

exit:
	return bmc_need;
}

void dump_ap_b2u_flags(void *sel, _adapter *adapter)
{
	RTW_PRINT_SEL(sel, "%4s %4s\n", "src", "fwd");
	RTW_PRINT_SEL(sel, "0x%02x 0x%02x\n", adapter->b2u_flags_ap_src, adapter->b2u_flags_ap_fwd);
}
#endif /* CONFIG_RTW_AP_DATA_BMC_TO_UC */

static int rtw_ap_nexthop_resolve(_adapter *adapter, struct xmit_frame *xframe)
{
	struct pkt_attrib *attrib = &xframe->attrib;
	int ret = _SUCCESS;

#ifdef CONFIG_RTW_WDS
	if (adapter_use_wds(adapter)) {
		if (rtw_wds_nexthop_lookup(adapter, attrib->dst, attrib->ra) == 0) {
			if (_rtw_memcmp(attrib->dst, attrib->ra, ETH_ALEN) == _FALSE)
				attrib->wds = 1;
		} else {
			ret = _FAIL;
		}
	} else
#endif
		_rtw_memcpy(attrib->ra, attrib->dst, ETH_ALEN);

	return ret;
}

#ifdef CONFIG_RTW_WDS
static void rtw_ap_data_flood_for_unknown_da(_adapter *adapter, const u8 *da,
					     const u8 *sa, const u8 *ori_ta,
					     u16 os_qid, _list *f_list)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	struct xmit_priv *xmitpriv = &adapter->xmitpriv;
	_list *head, *list;
	struct sta_info *sta;
	char sta_id[NUM_STA];
	u8 sta_num = 0;
	int i;

	_rtw_spinlock_bh(&stapriv->asoc_list_lock);
	head = &stapriv->asoc_list;
	list = get_next(head);

	while ((rtw_end_of_queue_search(head, list)) == _FALSE) {
		int stainfo_offset;

		sta = LIST_CONTAINOR(list, struct sta_info, asoc_list);
		list = get_next(list);

		stainfo_offset = rtw_stainfo_offset(stapriv, sta);
		if (stainfo_offset_valid(stainfo_offset))
			sta_id[sta_num++] = stainfo_offset;
	}

	_rtw_spinunlock_bh(&stapriv->asoc_list_lock);

	if (!sta_num)
		goto exit;

	for (i = 0; i < sta_num; i++) {
		struct xmit_frame *frame;
		struct pkt_attrib *attrib;

		sta = rtw_get_stainfo_by_offset(stapriv, sta_id[i]);
		if (!(sta->state & WIFI_ASOC_STATE)
		    || !(sta->flags & WLAN_STA_WDS)
		    || _rtw_memcmp(sta->phl_sta->mac_addr, sa, ETH_ALEN) == _TRUE
		    || (ori_ta && _rtw_memcmp(sta->phl_sta->mac_addr, ori_ta, ETH_ALEN) == _TRUE)
		    || is_broadcast_mac_addr(sta->phl_sta->mac_addr)
		    || is_zero_mac_addr(sta->phl_sta->mac_addr))
			continue;

		if (core_tx_alloc_xmitframe(adapter, &frame, os_qid) == FAIL)
			break;

		attrib = &frame->attrib;

		_rtw_memcpy(attrib->ra, sta->phl_sta->mac_addr, ETH_ALEN);
		_rtw_memcpy(attrib->ta, adapter_mac_addr(adapter), ETH_ALEN);
		_rtw_memcpy(attrib->dst, da, ETH_ALEN);
		_rtw_memcpy(attrib->src, sa, ETH_ALEN);
		attrib->wds = 1;

		rtw_list_insert_tail(&frame->list, f_list);
	}

exit:
	return;
}
#endif /* CONFIG_RTW_WDS */

int rtw_ap_addr_resolve(_adapter *adapter, u16 os_qid, struct xmit_frame *xframe,
			struct sk_buff *pkt, _list *f_list)
{
	struct pkt_file pktfile;
	struct ethhdr etherhdr;
	struct pkt_attrib *attrib;
	u8 is_da_mcast;

	int res = _SUCCESS;

	_rtw_open_pktfile(pkt, &pktfile);
	if (_rtw_pktfile_read(&pktfile, (u8 *)&etherhdr, ETH_HLEN) != ETH_HLEN) {
		res = _FAIL;
		goto exit;
	}

	xframe->pkt = pkt;
#if defined(CONFIG_RTW_WDS) || CONFIG_RTW_AP_DATA_BMC_TO_UC
	_rtw_init_listhead(f_list);
#endif

	is_da_mcast = IS_MCAST(etherhdr.h_dest);
	if (is_da_mcast) {
		#if CONFIG_RTW_AP_DATA_BMC_TO_UC
		if (rtw_ap_src_b2u_policy_chk(adapter->b2u_flags_ap_src, etherhdr.h_dest)
		    && adapter->registrypriv.wifi_spec == 0
		    && adapter->xmitpriv.free_xmitframe_cnt > (NR_XMITFRAME / 4)) {
			if (rtw_ap_data_bmc_to_uc(adapter, etherhdr.h_dest,
						  etherhdr.h_source, NULL,
						  os_qid, f_list) == 0) {
				res = RTW_ORI_NO_NEED;
				goto exit;
			}
		}
		#endif
	}

	attrib = &xframe->attrib;

	_rtw_memcpy(attrib->dst, etherhdr.h_dest, ETH_ALEN);
	_rtw_memcpy(attrib->src, etherhdr.h_source, ETH_ALEN);
	_rtw_memcpy(attrib->ta, adapter_mac_addr(adapter), ETH_ALEN);

	if (is_da_mcast) {
		_rtw_memcpy(attrib->ra, attrib->dst, ETH_ALEN);
	} else {
		res = rtw_ap_nexthop_resolve(adapter, xframe);
		#ifdef CONFIG_RTW_WDS
		if (res != _SUCCESS) {
			/* unknown DA, flood frame to every WDS STA */
			rtw_ap_data_flood_for_unknown_da(adapter, etherhdr.h_dest,
							 etherhdr.h_source, NULL,
							 os_qid, f_list);
			res = RTW_ORI_NO_NEED;
		}
		#endif
	}

exit:
	return res;
}

int rtw_ap_rx_data_validate_hdr(_adapter *adapter, union recv_frame *rframe, struct sta_info **sta)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	struct rx_pkt_attrib *rattrib = &rframe->u.hdr.attrib;
	u8 *whdr = get_recvframe_data(rframe);
	u8 is_ra_bmc = 0;
	sint ret = _FAIL;

	if (!(MLME_STATE(adapter) & WIFI_ASOC_STATE))
		goto exit;

	switch (rattrib->to_fr_ds) {
	case 2:
		if (IS_MCAST(GetAddr1Ptr(whdr)))
			goto exit;
		_rtw_memcpy(rattrib->ra, GetAddr1Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->ta, get_addr2_ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->dst, GetAddr3Ptr(whdr), ETH_ALEN); /* may change after checking AMSDU subframe header */
		_rtw_memcpy(rattrib->src, get_addr2_ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->bssid, GetAddr1Ptr(whdr), ETH_ALEN);
		break;
	case 3:
		is_ra_bmc = IS_MCAST(GetAddr1Ptr(whdr)) ? 1 : 0;
		_rtw_memcpy(rattrib->ra, GetAddr1Ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->ta, get_addr2_ptr(whdr), ETH_ALEN);
		_rtw_memcpy(rattrib->dst, GetAddr3Ptr(whdr), ETH_ALEN); /* may change after checking AMSDU subframe header */
		_rtw_memcpy(rattrib->src, GetAddr4Ptr(whdr), ETH_ALEN); /* may change after checking AMSDU subframe header */
		if (!is_ra_bmc)
			_rtw_memcpy(rattrib->bssid, GetAddr1Ptr(whdr), ETH_ALEN);
		break;
	default:
		ret = RTW_RX_HANDLED; /* don't count for drop */
		goto exit;
	}

	if (rattrib->addr_cam_vld)
		*sta = rtw_get_stainfo_by_macid(stapriv, rattrib->macid);
	else
		*sta = rtw_get_stainfo(stapriv, rattrib->ta);

	if (*sta == NULL) {
		if (!is_ra_bmc && !IS_RADAR_DETECTED(adapter_to_rfctl(adapter))) {
			#ifndef CONFIG_CUSTOMER_ALIBABA_GENERAL
			RTW_INFO(FUNC_ADPT_FMT" issue_deauth to "MAC_FMT" with reason(7), unknown TA\n"
				, FUNC_ADPT_ARG(adapter), MAC_ARG(rattrib->ta));
			issue_deauth(adapter, rattrib->ta, WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
			#endif
		}
		ret = RTW_RX_HANDLED;
		goto exit;
	}

#ifdef CONFIG_RTW_WDS_AUTO_EN
	if (rattrib->to_fr_ds == 3 && !(sta->flags & WLAN_STA_WDS))
		sta->flags |= WLAN_STA_WDS;
#endif

	_rtw_spinlock_bh(&stapriv->active_time_lock);
	if ((*sta)->start_active == _FALSE) {
		(*sta)->start_active = _TRUE;
		(*sta)->start_active_time = rtw_get_current_time();
		(*sta)->latest_active_time = (*sta)->start_active_time;
	} else {
		(*sta)->latest_active_time = rtw_get_current_time();
	}
	_rtw_spinunlock_bh(&stapriv->active_time_lock);

	process_pwrbit_data(adapter, rframe, *sta);

	if ((get_frame_sub_type(whdr) & WIFI_QOS_DATA_TYPE) == WIFI_QOS_DATA_TYPE)
		process_wmmps_data(adapter, rframe, *sta);

	if (get_frame_sub_type(whdr) & BIT(6)) {
		/* No data, will not indicate to upper layer, temporily count it here */
		count_rx_stats(adapter, rframe, *sta);
		ret = RTW_RX_HANDLED;
		goto exit;
	}

	ret = _SUCCESS;

exit:
	return ret;
}

int rtw_ap_rx_msdu_act_check(union recv_frame *rframe
	, const u8 *da, const u8 *sa
	, u8 *msdu, enum rtw_rx_llc_hdl llc_hdl
	, struct xmit_frame **fwd_frame, _list *f_list)
{
	_adapter *adapter = rframe->u.hdr.adapter;
	struct rx_pkt_attrib *rattrib = &rframe->u.hdr.attrib;
	struct rtw_wds_path *wpath;
	u8 is_da_bmc = IS_MCAST(da);
	u8 is_da_self = !is_da_bmc && _rtw_memcmp(da, adapter_mac_addr(adapter), ETH_ALEN);
	u8 is_da_peer = 0;
	int in_wds_tbl = 0;
	u16 os_qid;
	struct xmit_frame *xframe;
	struct pkt_attrib *xattrib;
	u8 fwd_ra[ETH_ALEN] = {0};
	int act = 0;

#ifdef CONFIG_RTW_WDS
	/* update/create wds info for SA, RA */
	if (adapter_use_wds(adapter)
		&& (rframe->u.hdr.psta->state & WIFI_ASOC_STATE)
		&& _rtw_memcmp(sa, rframe->u.hdr.psta->phl_sta->mac_addr, ETH_ALEN) == _FALSE
	) {
		rtw_rcu_read_lock();
		wpath = rtw_wds_path_lookup(adapter, sa);
		if (!wpath)
			rtw_wds_path_add(adapter, sa, rframe->u.hdr.psta);
		else {
			rtw_wds_path_assign_nexthop(wpath, rframe->u.hdr.psta);
			wpath->last_update = rtw_get_current_time();
		}
		rtw_rcu_read_unlock();
	}
#endif

	/* SA is self, need no further process */
	if (_rtw_memcmp(sa, adapter_mac_addr(adapter), ETH_ALEN) == _TRUE)
		goto exit;

	if (is_da_bmc) {
		/* DA is bmc addr */
		act |= RTW_RX_MSDU_ACT_INDICATE;
		#if 0 /* TODO: ap_isolate */
		if (adapter->mlmepriv.ap_isolate)
			goto exit;
		#endif
		goto fwd_chk;

	}

	if (is_da_self) {
		/* DA is self, indicate */
		act |= RTW_RX_MSDU_ACT_INDICATE;
		goto exit;
	}

	/* DA is not self */
#ifdef CONFIG_RTW_WDS
	if (adapter_use_wds(adapter))
		in_wds_tbl = rtw_wds_nexthop_lookup(adapter, da, fwd_ra) == 0;
#endif
	if (!in_wds_tbl)
		is_da_peer = rtw_get_stainfo(&adapter->stapriv, da) ? 1 : 0;

	if (in_wds_tbl || is_da_peer) {
		/* DA is known (peer or can be forwarded by peer) */
		#if 0 /* TODO: ap_isolate */
		if (adapter->mlmepriv.ap_isolate) {
			#if defined(DBG_RX_DROP_FRAME)
			RTW_INFO("DBG_RX_DROP_FRAME "FUNC_ADPT_FMT" DA("MAC_FMT") through peer, ap_isolate\n"
				, FUNC_ADPT_ARG(adapter), MAC_ARG(da));
			#endif
			goto exit;
		}
		#endif
		goto fwd_chk;
	}

	/* DA is unknown*/
	act |= RTW_RX_MSDU_ACT_INDICATE;
	#if 0 /* TODO: ap_isolate */
	if (adapter->mlmepriv.ap_isolate) {
		/*
		* unknown DA and ap_isolate, indicate only
		* (bridge will not forward packets to originating port)
		*/
		goto exit;
	}
	#endif

fwd_chk:

	if (adapter->stapriv.asoc_list_cnt <= 1)
		goto exit;

	os_qid = rtw_os_recv_select_queue(msdu, llc_hdl);

#if defined(CONFIG_RTW_WDS) || CONFIG_RTW_AP_DATA_BMC_TO_UC
	_rtw_init_listhead(f_list);
#endif

#if CONFIG_RTW_AP_DATA_BMC_TO_UC
	if (is_da_bmc) {
		if (rtw_ap_fwd_b2u_policy_chk(adapter->b2u_flags_ap_fwd, da, rattrib->to_fr_ds == 3 && !IS_MCAST(rattrib->ra))
			&& adapter->registrypriv.wifi_spec == 0
			&& adapter->xmitpriv.free_xmitframe_cnt > (NR_XMITFRAME / 4)
		) {
			if (rtw_ap_data_bmc_to_uc(adapter
				, da, sa, rframe->u.hdr.psta->phl_sta->mac_addr
				, os_qid, f_list) == 0
			) {
				if (!rtw_is_list_empty(f_list))
					goto set_act_fwd;
				else
					goto exit;
			}
		}
	} else
#endif
#ifdef CONFIG_RTW_WDS
	if (adapter_use_wds(adapter) && !in_wds_tbl && !is_da_peer) {
		/* unknown DA, flood frame to every WDS STA except receiving one */
		rtw_ap_data_flood_for_unknown_da(adapter
			, da, sa, rframe->u.hdr.psta->phl_sta->mac_addr
			, os_qid, f_list);
		if (!rtw_is_list_empty(f_list))
			goto set_act_fwd;
		else
			goto exit;
	} else
#endif
	;

	if (core_tx_alloc_xmitframe(adapter, &xframe, os_qid) == FAIL) {
		#ifdef DBG_TX_DROP_FRAME
		RTW_INFO("DBG_TX_DROP_FRAME "FUNC_ADPT_FMT" core_tx_alloc_xmitframe fail\n"
			, FUNC_ADPT_ARG(adapter));
		#endif
		goto exit;
	}

	xattrib = &xframe->attrib;

	_rtw_memcpy(xattrib->dst, da, ETH_ALEN);
	_rtw_memcpy(xattrib->src, sa, ETH_ALEN);
	_rtw_memcpy(xattrib->ta, adapter_mac_addr(adapter), ETH_ALEN);

	#ifdef CONFIG_RTW_WDS
	if (in_wds_tbl && _rtw_memcmp(da, fwd_ra, ETH_ALEN) == _FALSE) {
		_rtw_memcpy(xattrib->ra, fwd_ra, ETH_ALEN);
		xattrib->wds = 1;
	} else
	#endif
		_rtw_memcpy(xattrib->ra, da, ETH_ALEN);

	*fwd_frame = xframe;

#if defined(CONFIG_RTW_WDS) || CONFIG_RTW_AP_DATA_BMC_TO_UC
set_act_fwd:
#endif
	act |= RTW_RX_MSDU_ACT_FORWARD;

exit:
	return act;
}

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
void rtw_issue_action_token_req(_adapter *padapter, struct sta_info *pstat)
{
	/* Token Request Format
	 	Category code :		1 Byte
		Action code : 		1 Byte
		Element field: 		4 Bytes, the duration of data transmission requested for the station.
	*/

	u8 val = 0x0;
	u8 category = RTW_WLAN_CATEGORY_TBTX;
	u32 tbtx_duration = TBTX_TX_DURATION*1000;
	u8 *pframe;
	unsigned short *fctrl;
	struct xmit_frame		*pmgntframe;
	struct pkt_attrib		*pattrib;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	struct xmit_priv *pxmitpriv = &(padapter->xmitpriv);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct _ADAPTER_LINK *padapter_link = pstat->padapter_link;
	struct link_mlme_ext_info	*lmlmeinfo = &(padapter_link->mlmeextpriv,mlmext_info);
	WLAN_BSSID_EX *pnetwork = &(lmlmeinfo->network);


	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	RTW_DBG("%s: %6ph\n", __FUNCTION__, pstat->phl_sta->mac_addr);
	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgnt_tx_rate(padapter, padapter_link, IEEE80211_OFDM_RATE_24MB); // issue action request using OFDM rate? 20190320 Bruce add
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy((void *)GetAddr1Ptr(pwlanhdr), pstat->phl_sta->mac_addr, ETH_ALEN);
	_rtw_memcpy((void *)get_addr2_ptr(pwlanhdr), padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy((void *)GetAddr3Ptr(pwlanhdr), get_my_bssid(&(pnetwork->network)), ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_ACTION);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	pframe = rtw_set_fixed_ie(pframe, 1, &(category), &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 1, &(val), &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 4, (unsigned char *)&(tbtx_duration), &(pattrib->pktlen));

	pattrib->last_txcmdsz = pattrib->pktlen;
	padapter->stapriv.last_token_holder = pstat;
	dump_mgntframe(padapter, pmgntframe);

}
#endif	/* CONFIG_RTW_TOKEN_BASED_XMIT */

void rtw_ap_set_sta_wmode(_adapter *padapter, struct sta_info *sta)
{
	struct link_mlme_priv *pmlmepriv = &(sta->padapter_link->mlmepriv);
	WLAN_BSSID_EX *pcur_network = (WLAN_BSSID_EX *)&pmlmepriv->cur_network.network;
	enum wlan_mode network_type = WLAN_MD_INVALID;

#ifdef CONFIG_80211AX_HE
	if (sta->hepriv.he_option == _TRUE)
		network_type = WLAN_MD_11AX;
#endif
#ifdef CONFIG_80211AC_VHT
	if (network_type == WLAN_MD_INVALID) {
		if (sta->vhtpriv.vht_option == _TRUE)
			network_type = WLAN_MD_11AC;
	}
#endif
#ifdef CONFIG_80211N_HT
	if (network_type == WLAN_MD_INVALID) {
		if (sta->htpriv.ht_option == _TRUE)
			network_type =  WLAN_MD_11N;
	}
#endif

	if (pcur_network->Configuration.DSConfig > 14)
		network_type |= WLAN_MD_11A;
	else {
		if ((cckratesonly_included(sta->bssrateset, sta->bssratelen)) == _TRUE)
			network_type |= WLAN_MD_11B;
		else if ((cckrates_included(sta->bssrateset, sta->bssratelen)) == _TRUE)
			network_type |= WLAN_MD_11BG;
		else
			network_type |= WLAN_MD_11G;
	}

	sta->phl_sta->wmode = network_type;
}

#if defined(CONFIG_RTW_ACS) && defined(WKARD_ACS)
void rtw_acs_start(_adapter *padapter)
{
	RTW_INFO(FUNC_ADPT_FMT" not support\n", FUNC_ADPT_ARG(padapter));
}

void rtw_acs_stop(_adapter *padapter)
{
	RTW_INFO(FUNC_ADPT_FMT" not support\n", FUNC_ADPT_ARG(padapter));
}
#endif /* defined(CONFIG_RTW_ACS) && defined(WKARD_ACS) */

#ifdef CONFIG_AP_CMD_DISPR

static enum rtw_phl_status _ap_start_swch_start_notify(struct _ADAPTER *padapter)
{
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	struct rtw_wifi_role_t *role = padapter->phl_role;
	enum rtw_phl_status status;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_START);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_SWCH_START);

	msg.band_idx = padapter->ap_start_cmd_bidx;
	msg.rsvd[0].ptr = (u8*)role;

	status = rtw_phl_send_msg_to_dispr(GET_PHL_INFO(d),
					       &msg, &attr, NULL);

	return status;
}

static void _ap_start_swch_done_notify_cb(void *priv, struct phl_msg *msg)
{
	struct _ADAPTER *a = (struct _ADAPTER *)priv;


	RTW_DBG(FUNC_ADPT_FMT ": ap_start_cmd_state=%u\n",
		FUNC_ADPT_ARG(a), a->ap_start_cmd_state);

	if (msg->inbuf) {
		rtw_vmfree(msg->inbuf, msg->inlen);
		msg->inbuf = NULL;
	}
}

static enum rtw_phl_status
_ap_start_swch_done_notify(struct _ADAPTER *a, struct rtw_chan_def *chandef)
{
	struct dvobj_priv *d = adapter_to_dvobj(a);
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	u8 *info = NULL;
	enum rtw_phl_status status;


	info = rtw_vmalloc(sizeof(struct rtw_chan_def));
	if (!info) {
		RTW_ERR(FUNC_ADPT_FMT ": Allocate msg hub buffer fail!\n",
			FUNC_ADPT_ARG(a));
		return RTW_PHL_STATUS_RESOURCE;
	}
	_rtw_memcpy(info, chandef, sizeof(struct rtw_chan_def));

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_START);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_SWCH_DONE);

	msg.band_idx = a->ap_start_cmd_bidx;
	msg.inbuf = info;
	msg.inlen = sizeof(struct rtw_chan_def);

	attr.completion.completion = _ap_start_swch_done_notify_cb;
	attr.completion.priv = a;

	status = rtw_phl_send_msg_to_dispr(GET_PHL_INFO(d),
					       &msg, &attr, NULL);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		rtw_vmfree(info, sizeof(struct rtw_chan_def));
		RTW_ERR(FUNC_ADPT_FMT ": send_msg_to_dispr fail(0x%x)!\n",
			FUNC_ADPT_ARG(a), status);
	}

	return status;
}

static void _ap_start_end_notify_cb(void *priv, struct phl_msg *msg)
{

	if (msg->inbuf) {
		rtw_vmfree(msg->inbuf, msg->inlen);
		msg->inbuf = NULL;
	}
}

static enum rtw_phl_status
_ap_start_end_notify(struct _ADAPTER *padapter, bool success, bool abort)
{
	struct rtw_wifi_role_t *role = padapter->phl_role;
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	u8 *info = NULL;

	RTW_DBG(FUNC_ADPT_FMT ": MSG_EVT_AP_START\n", FUNC_ADPT_ARG(padapter));

	info = rtw_vmalloc(sizeof(u8));
	if (info == NULL) {
		RTW_ERR("%s: alloc buffer failed!\n", __func__);
		return RTW_PHL_STATUS_RESOURCE;
	}

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_START);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_AP_START_END);
	if (success) {
		*info = RTW_PHL_STATUS_SUCCESS;
	} else {
		*info = RTW_PHL_STATUS_FAILURE;
	}

	msg.inbuf = info;
	msg.inlen = 1;
	msg.band_idx = padapter->ap_start_cmd_bidx;
	msg.rsvd[0].ptr = (u8*)role;

	if(abort)
		attr.opt = MSG_OPT_SEND_IN_ABORT;
	attr.completion.completion = _ap_start_end_notify_cb;
	attr.completion.priv = padapter;


	status = rtw_phl_send_msg_to_dispr(GET_PHL_INFO(d),
					       &msg, &attr, NULL);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		rtw_vmfree(info, sizeof(u8));
		RTW_ERR(FUNC_ADPT_FMT ": send_msg_to_dispr fail(0x%x)!\n",
			FUNC_ADPT_ARG(padapter), status);
	}

	return status;
}

static void ap_free_cmdobj(struct cmd_obj *pcmd)
{
	struct _ADAPTER *padapter = pcmd->padapter;

	if (!pcmd)
		return;


	if (pcmd->sctx) {
		if (pcmd->res == H2C_SUCCESS)
			rtw_sctx_done(&pcmd->sctx);
		else
			rtw_sctx_done_err(&pcmd->sctx, RTW_SCTX_DONE_CMD_ERROR);
	}

	rtw_free_cmd_obj(pcmd);

	return;
}

static void _ap_start_cmd_done(struct cmd_obj *pcmd)
{
	struct _ADAPTER *padapter = pcmd->padapter;
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	struct rtw_wifi_role_t *role = padapter->phl_role;
	enum rtw_phl_status status;

	RTW_INFO("%s: +\n", __func__);

	if (pcmd->cmd_token == 0){
		RTW_ERR("%s : token is NULL!\n", __func__);
		return;
	}

	status = rtw_phl_free_cmd_token(GET_PHL_INFO(d),
					padapter->ap_start_cmd_bidx,
					&(pcmd->cmd_token));

	padapter->ap_start_cmd_state = 0;

	if (status != RTW_PHL_STATUS_SUCCESS)
		RTW_ERR(FUNC_ADPT_FMT ": free_cmd_token fail(0x%x)!\n",
			FUNC_ADPT_ARG(padapter), status);

	ap_free_cmdobj(pcmd);

	RTW_INFO("%s: -\n", __func__);
}

enum rtw_phl_status rtw_free_bcn_entry(struct _ADAPTER *padapter){

	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

#ifdef RTW_PHL_BCN
	status = rtw_phl_free_bcn_entry(d->phl, padapter_link->wrlink);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_INFO("%s - phl_free_bcn_entry failed\n", __func__);
	}
#endif

	return status;
}

static enum phl_mdl_ret_code _ap_start_req_acquired(void *dispr, void *priv)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *padapter = pcmd->padapter;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct createbss_parm *parm = (struct createbss_parm *)pcmd->parmbuf;
	struct rtw_wifi_role_t *role = padapter->phl_role;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(padapter));
	padapter->ap_start_cmd_bidx = padapter_link->wrlink->hw_band;

	rtw_core_ap_chan_decision(padapter, parm);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_START);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_AP_START_PREPARE);
	msg.band_idx = padapter->ap_start_cmd_bidx;

	status = rtw_phl_send_msg_to_dispr(dvobj->phl,
	                                    &msg,
	                                    &attr,
	                                    NULL);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		#ifdef CONFIG_DBCC_SUPPORT
		if (parm->is_trigger_dbcc == _TRUE)
			rtw_phl_mr_cancel_dbcc_action(dvobj->phl);
		#endif
		return MDL_RET_FAIL;
	} else {
		SET_STATUS_FLAG(padapter->ap_start_cmd_state, CMD_APSTART_ACQUIRE);
		return MDL_RET_SUCCESS;
	}
}

static enum phl_mdl_ret_code _ap_start_req_abort(void *dispr, void *priv)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *padapter = pcmd->padapter;
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (TEST_STATUS_FLAG(padapter->ap_start_cmd_state, CMD_APSTART_ACQUIRE)) {
		status = _ap_start_end_notify(padapter, false, true);
	}

	padapter->ap_start_cmd_state = 0;
	RTW_INFO("%s\n", __func__);
	return MDL_RET_SUCCESS;
}

const char *ap_get_evt_str(u32 evt)
{
	switch (evt) {
	/* AP */
	case MSG_EVT_AP_START_PREPARE:
		return "MSG_EVT_AP_START_PREPARE";
	case MSG_EVT_AP_START:
		return "MSG_EVT_AP_START";
	case MSG_EVT_AP_START_END:
		return "MSG_EVT_AP_START_END";
	case MSG_EVT_AP_STOP_PREPARE:
		return "MSG_EVT_AP_STOP_PREPARE";
	case MSG_EVT_AP_STOP:
		return "MSG_EVT_AP_STOP";
	case MSG_EVT_AP_STOP_END:
		return "MSG_EVT_AP_STOP_END";

	case MSG_EVT_SWCH_START:
		return "MSG_EVT_SWCH_START";
	case MSG_EVT_SWCH_DONE:
		return "MSG_EVT_SWCH_DONE";
	case MSG_EVT_ROLE_NTFY:
		return "MSG_EVT_ROLE_NTFY";

	/* STA */
	case MSG_EVT_CONNECT_START:
		return "MSG_EVT_CONNECT_START";
	case MSG_EVT_CONNECT_LINKED:
		return "MSG_EVT_CONNECT_LINKED";
	case MSG_EVT_CONNECT_END:
		return "MSG_EVT_CONNECT_END";
	case MSG_EVT_DISCONNECT_PREPARE:
		return "MSG_EVT_DISCONNECT_PREPARE";
	case MSG_EVT_DISCONNECT:
		return "MSG_EVT_DISCONNECT";

	case MSG_EVT_DBCC_DISABLE:
		return "MSG_EVT_DBCC_DISABLE";
	default:
		printk("%s evt=0x%08x\n",__func__, evt);
		return "unknown EVT";
	}
	return "unknown EVT";
}

static enum phl_mdl_ret_code _ap_start_req_ev_hdlr(void *dispr, void *priv, struct phl_msg *msg)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *padapter = pcmd->padapter;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct createbss_parm *parm = (struct createbss_parm *)pcmd->parmbuf;
	struct rtw_wifi_role_t *wifi_role = padapter->phl_role;
	struct rtw_phl_com_t *phl_com = wifi_role->phl_com;
	void *d = phlcom_to_drvpriv(phl_com);
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	RTW_DBG(FUNC_ADPT_FMT ": + msg_id=0x%08x\n",
		FUNC_ADPT_ARG(padapter), msg->msg_id);

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START) {
		/*RTW_INFO(FUNC_ADPT_FMT ": Message is not from ap start module, "
			 "skip msg_id=0x%08x\n", FUNC_ADPT_ARG(padapter), msg->msg_id);
		RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));*/
		return MDL_RET_IGNORE;
	}

	RTW_INFO("[AP_START] %s("ADPT_FMT")\n",
		ap_get_evt_str(MSG_EVT_ID_FIELD(msg->msg_id)), ADPT_ARG(padapter));

	if (IS_MSG_FAIL(msg->msg_id)) {
		RTW_WARN(FUNC_ADPT_FMT ": cmd dispatcher notify cmd failure on "
			 "msg_id=0x%08x\n", FUNC_ADPT_ARG(padapter), msg->msg_id);
	}

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_AP_START_PREPARE:

		if ((parm->req_ch == 0 &&
			pmlmeinfo->state == WIFI_FW_AP_STATE) ||
			parm->req_ch != 0
		)
			rtw_core_ap_prepare(padapter, parm);

		status = _ap_start_swch_start_notify(padapter);
		if(status != RTW_PHL_STATUS_SUCCESS)
			RTW_ERR("%s :: [MSG_EVT_AP_START_PREPARE] rtw_phl_send_msg_to_dispr failed\n", __func__);
		break;

	case MSG_EVT_SWCH_START:
		rtw_core_ap_swch_start(padapter, parm);
		status = _ap_start_swch_done_notify(padapter, &padapter_link->wrlink->chandef);
		if(status != RTW_PHL_STATUS_SUCCESS)
			RTW_ERR("%s :: [SWCH_START] rtw_phl_send_msg_to_dispr failed\n", __func__);
		break;

	case MSG_EVT_SWCH_DONE:
		status = rtw_phl_ap_started(adapter_to_dvobj(padapter)->phl,
					padapter->ap_start_cmd_bidx, padapter->phl_role);
		if(status != RTW_PHL_STATUS_SUCCESS)
			RTW_ERR("%s :: [MSG_EVT_SWCH_DONE] rtw_phl_send_msg_to_dispr failed\n", __func__);
		break;

	case MSG_EVT_AP_START:
		rtw_core_ap_start(padapter, parm); /* send beacon */
		status = _ap_start_end_notify(padapter, true, false);
		break;

	case MSG_EVT_AP_START_END:

		if (msg->inbuf == NULL) {
			RTW_WARN("[MSG_EVT_AP_START_END] Msg info buffer NULL!\n");
			break;
		}

		if (*(msg->inbuf) == RTW_PHL_STATUS_FAILURE){
			RTW_WARN("[MSG_EVT_AP_START_END] Msg_inbuf is FAIL!\n");
			stop_ap_hdl(padapter);
		}

		_ap_start_cmd_done(pcmd);
		break;

	default:
		/* unknown state */
		break;
	}


	if (status != RTW_PHL_STATUS_SUCCESS) {
		/* Trigger stop AP flow when start AP failure */
		stop_ap_hdl(padapter);
		_ap_start_cmd_done(pcmd);
	}

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code _ap_start_req_set_info(void *dispr, void *priv, struct phl_module_op_info* info)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *padapter = pcmd->padapter;

	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(padapter));
	RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));

	return MDL_RET_IGNORE;
}

static enum phl_mdl_ret_code _ap_start_req_query_info(void *dispr, void *priv, struct phl_module_op_info* info)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *padapter = pcmd->padapter;
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);


	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(padapter));

	switch (info->op_code) {
	case FG_REQ_OP_GET_ROLE:
		info->outbuf = (u8*)padapter->phl_role;
		ret = MDL_RET_SUCCESS;
		break;

	case FG_REQ_OP_GET_ROLE_LINK:
		info->outbuf = (u8*)padapter_link->wrlink;
		ret = MDL_RET_SUCCESS;
		break;

	default:
		break;
	}

	RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));
	return ret;
}

static void rtw_cmd_ap_start_req_init(struct cmd_obj *pcmd, struct phl_cmd_token_req *fgreq)
{
	struct _ADAPTER *padapter = pcmd->padapter;
	u8 res = _SUCCESS;

	/* Fill foreground command request */
	fgreq->module_id= PHL_FG_MDL_AP_START;
	fgreq->opt = FG_CMD_OPT_EXCLUSIVE;
	fgreq->priv = pcmd;
	fgreq->role = padapter->phl_role;

	fgreq->acquired = _ap_start_req_acquired;
	fgreq->abort = _ap_start_req_abort;
	fgreq->msg_hdlr = _ap_start_req_ev_hdlr;
	fgreq->set_info = _ap_start_req_set_info;
	fgreq->query_info = _ap_start_req_query_info;

	padapter->ap_start_cmd_state = 0;
	pcmd->cmd_token = 0;

	RTW_INFO("%s\n", __func__);
}

enum rtw_phl_status rtw_ap_start_cmd(struct cmd_obj *pcmd)
{
	struct _ADAPTER *padapter = pcmd->padapter;
	struct phl_cmd_token_req fgreq={0};
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	enum rtw_phl_status status;
	struct rtw_wifi_role_t *role = padapter->phl_role;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	rtw_cmd_ap_start_req_init(pcmd, &fgreq);

	RTW_DBG(FUNC_ADPT_FMT ": \n", FUNC_ADPT_ARG(padapter));

	/* cmd_dispatcher would copy whole phl_cmd_token_req */
	status = rtw_phl_add_cmd_token_req(d->phl, padapter_link->wrlink->hw_band, &fgreq, &(pcmd->cmd_token));
	if ((status != RTW_PHL_STATUS_SUCCESS) &&
	   (status != RTW_PHL_STATUS_PENDING))
		goto error;

	status = RTW_PHL_STATUS_SUCCESS;

error:
	return status;

}

static void _ap_stop_cmd_done(struct cmd_obj *pcmd)
{
	struct _ADAPTER *padapter = pcmd->padapter;
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	struct rtw_wifi_role_t *role = padapter->phl_role;
	enum rtw_phl_status status;

	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(padapter));

	if (!padapter->ap_stop_cmd_token){
		RTW_ERR("%s : token is NULL!\n", __func__);
		return;
	}

	status = rtw_phl_free_cmd_token(GET_PHL_INFO(d),
					padapter->ap_stop_cmd_bidx,
					&padapter->ap_stop_cmd_token);

	padapter->ap_stop_cmd_token = 0;
	padapter->ap_stop_cmd_state = 0;
	if (status != RTW_PHL_STATUS_SUCCESS)
		RTW_ERR(FUNC_ADPT_FMT ": free_cmd_token fail(0x%x)!\n",
			FUNC_ADPT_ARG(padapter), status);

	ap_free_cmdobj(pcmd);

	RTW_INFO(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));
}

static enum rtw_phl_status _ap_stop_ap_stop_notify(struct _ADAPTER *adapter)
{
	struct dvobj_priv *d = adapter_to_dvobj(adapter);
	struct rtw_wifi_role_t *role = adapter->phl_role;
	enum rtw_phl_status status;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_STOP);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_AP_STOP);

	msg.band_idx = adapter->ap_stop_cmd_bidx;
	msg.rsvd[0].ptr = (u8*)role;

	status = rtw_phl_send_msg_to_dispr(GET_PHL_INFO(d),
					       &msg, &attr, NULL);

	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR(FUNC_ADPT_FMT ": send_msg_to_dispr fail(0x%x)!\n", FUNC_ADPT_ARG(adapter), status);
	}
	return status;
}

static enum rtw_phl_status _ap_stop_ap_end_notify(struct _ADAPTER *adapter)
{
	struct dvobj_priv *d = adapter_to_dvobj(adapter);
	struct rtw_wifi_role_t *role = adapter->phl_role;
	enum rtw_phl_status status;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_STOP);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_AP_STOP_END);

	msg.band_idx = adapter->ap_stop_cmd_bidx;
	msg.inbuf = (u8 *)role;

	status = rtw_phl_send_msg_to_dispr(GET_PHL_INFO(d),
					       &msg, &attr, NULL);

	if (status != RTW_PHL_STATUS_SUCCESS)
		RTW_ERR(FUNC_ADPT_FMT ": send_msg_to_dispr fail(0x%x)!\n", FUNC_ADPT_ARG(adapter), status);

	return status;
}

static void _ap_stop_abort_notify_cb(void *priv, struct phl_msg *msg)
{
	_adapter *padapter = (_adapter *)priv;
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	struct _ADAPTER *adapter = (struct _ADAPTER *)priv;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct rtw_chan_def chan_def = {0};
	u8 chctx_num = 0;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);

#ifdef RTW_PHL_BCN
	phl_status = rtw_phl_free_bcn_entry(d->phl, adapter_link->wrlink);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		RTW_INFO("%s - phl_free_bcn_entry failed\n", __func__);
	}
#endif
}

static enum rtw_phl_status _ap_stop_abort_notify(struct _ADAPTER *adapter, bool abort)
{
	struct dvobj_priv *d = adapter_to_dvobj(adapter);
	struct rtw_wifi_role_t *role = adapter->phl_role;
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_STOP);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_AP_STOP);
	msg.band_idx = adapter->ap_stop_cmd_bidx;
	msg.rsvd[0].ptr = (u8*)role;

	if(abort)
		attr.opt = MSG_OPT_SEND_IN_ABORT;
	attr.completion.completion = _ap_stop_abort_notify_cb;
	attr.completion.priv = adapter;

	status = rtw_phl_send_msg_to_dispr(d->phl,
	                            		&msg,
	                                	&attr,
	                            		NULL);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_INFO("[%s] Abort send msg fail!\n", __func__);
	}

	return status;
}

static enum phl_mdl_ret_code _ap_stop_req_acquired(void *dispr, void *priv)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *padapter = pcmd->padapter;
	struct rtw_wifi_role_t *role = padapter->phl_role;
	struct phl_msg msg = {0};
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(padapter));

	SET_STATUS_FLAG(padapter->ap_stop_cmd_state, CMD_APSTOP_ACQUIRE);
	padapter->ap_stop_cmd_bidx = padapter_link->wrlink->hw_band;

	rtw_phl_ap_stop(adapter_to_dvobj(padapter)->phl,
			padapter->ap_stop_cmd_bidx,role);

	RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));

	return MDL_RET_SUCCESS;

}

static enum phl_mdl_ret_code _ap_stop_req_abort(void *dispr, void *priv)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *padapter = pcmd->padapter;

	if(TEST_STATUS_FLAG(padapter->ap_stop_cmd_state, CMD_APSTOP_STARTED)){
		/* case1: MSG_EVT_AP_STOP has been processed*/
	}
	else if(TEST_STATUS_FLAG(padapter->ap_stop_cmd_state, CMD_APSTOP_ACQUIRE)){
		/* case2: MSG_EVT_AP_STOP in msgQ
		   case3: MSG_EVT_AP_STOP doesn't back to msg_hdlr yet */
		if (RTW_PHL_STATUS_SUCCESS != _ap_stop_abort_notify(padapter, true)) {
			/* do not execute core_mr_handle because do not know if the I/O operation is acceptable */
			//core_mr_delete_chctx(adapter, &chctx_num, &chan_def);
		}
	}
	else{
		/* apstop token stay in tokenOpQ */
	}

	padapter->ap_stop_cmd_state = 0;
	RTW_INFO("%s\n", __func__);
	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code _ap_stop_req_ev_hdlr(void *dispr, void *priv, struct phl_msg* msg)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *padapter = pcmd->padapter;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_wifi_role_t *wifi_role = padapter->phl_role;
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	RTW_DBG(FUNC_ADPT_FMT ": + msg_id=0x%08x\n",
		FUNC_ADPT_ARG(padapter), msg->msg_id);

	if ((MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) &&
		(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_MRC)) {
		RTW_DBG(FUNC_ADPT_FMT ": Message is not from ap stop/mrc module, "
			 "skip msg_id=0x%08x\n", FUNC_ADPT_ARG(padapter), msg->msg_id);
		RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));
		return MDL_RET_IGNORE;
	}

	RTW_INFO("[AP_STOP] %s("ADPT_FMT")\n",
		ap_get_evt_str(MSG_EVT_ID_FIELD(msg->msg_id)), ADPT_ARG(padapter));

	if (IS_MSG_FAIL(msg->msg_id)) {
		RTW_WARN(FUNC_ADPT_FMT ": cmd dispatcher notify cmd failure on "
			 "msg_id=0x%08x\n", FUNC_ADPT_ARG(padapter), msg->msg_id);
	}

	switch(MSG_EVT_ID_FIELD(msg->msg_id)) {
		case MSG_EVT_AP_STOP_PREPARE:
		{
			status = _ap_stop_ap_stop_notify(padapter);
			if (status != RTW_PHL_STATUS_SUCCESS) {
				RTW_ERR("%s :: [MSG_EVT_AP_STOP_PREPARE] ap_stop_notify fail\n", __func__);
				_ap_stop_cmd_done(pcmd);
			}
			break;
		}
		case MSG_EVT_AP_STOP:
		{
			#ifdef CONFIG_DBCC_SUPPORT
			struct rtw_mr_chctx_info mr_cc_info = {0};
			#endif

			stop_ap_hdl(padapter);

			#ifdef CONFIG_DBCC_SUPPORT
			rtw_phl_chanctx_chk(GET_PHL_INFO(dvobj),
							padapter->phl_role,
							padapter_link->wrlink,
							NULL,
							&mr_cc_info);
			if (mr_cc_info.sugg_opmode == MR_OP_DBCC) /*current stay in DBCC*/
				rtw_phl_mr_trig_dbcc_disable(GET_PHL_INFO(dvobj));
			#endif

			status = _ap_stop_ap_end_notify(padapter);
			if (status != RTW_PHL_STATUS_SUCCESS) {
				_ap_stop_cmd_done(pcmd);
				#ifdef CONFIG_DBCC_SUPPORT
				if (mr_cc_info.sugg_opmode == MR_OP_DBCC)
					rtw_phl_mr_cancel_dbcc_action(GET_PHL_INFO(dvobj));
				#endif
			}
			break;
		}
		case MSG_EVT_AP_STOP_END:
			_ap_stop_cmd_done(pcmd);
			break;

#ifdef CONFIG_DBCC_SUPPORT
		case MSG_EVT_DBCC_DISABLE:
			if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_MRC) {
				struct rtw_wifi_role_t *role;

				role = (struct rtw_wifi_role_t *)msg->inbuf;
				RTW_INFO("[STOP_AP-FG] WR(%d) RX MRC- DBCC DISABLE MSG\n", role->id);
			}
			break;
#endif

		default:
			/* unknown state */
		break;
	}

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code _ap_stop_req_set_info(void *dispr, void *priv, struct phl_module_op_info* info)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *padapter = pcmd->padapter;

	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(padapter));
	RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));

	return MDL_RET_IGNORE;
}

static enum phl_mdl_ret_code _ap_stop_req_query_info(void *dispr, void *priv, struct phl_module_op_info* info)
{
	struct cmd_obj *pcmd = (struct cmd_obj *)priv;
	struct _ADAPTER *a = pcmd->padapter;
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(a);


	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(a));

	switch (info->op_code) {
	case FG_REQ_OP_GET_ROLE:
		info->outbuf = (u8*)a->phl_role;
		ret = MDL_RET_SUCCESS;
		break;

	case FG_REQ_OP_GET_ROLE_LINK:
		info->outbuf = (u8*)alink->wrlink;
		ret = MDL_RET_SUCCESS;
		break;

	default:
		break;
	}

	RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(a));
	return ret;
}

static void rtw_cmd_ap_stop_req_init(struct cmd_obj *pcmd, struct phl_cmd_token_req *fgreq)
{
	struct _ADAPTER *padapter = pcmd->padapter;

	RTW_DBG(FUNC_ADPT_FMT ": \n", FUNC_ADPT_ARG(padapter));
	/* Fill foreground command request */
	fgreq->module_id= PHL_FG_MDL_AP_STOP;
	fgreq->opt = FG_CMD_OPT_EXCLUSIVE;
	fgreq->priv = pcmd;
	fgreq->role = padapter->phl_role;

	fgreq->acquired = _ap_stop_req_acquired;
	fgreq->abort = _ap_stop_req_abort;
	fgreq->msg_hdlr = _ap_stop_req_ev_hdlr;
	fgreq->set_info = _ap_stop_req_set_info;
	fgreq->query_info = _ap_stop_req_query_info;

	padapter->ap_stop_cmd_state = 0;
	padapter->ap_stop_cmd_token = 0;
}

enum rtw_phl_status rtw_ap_stop_cmd(struct cmd_obj *pcmd)
{
	struct _ADAPTER *padapter = pcmd->padapter;
	struct phl_cmd_token_req fgreq={0};
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	enum rtw_phl_status pstatus;
	struct rtw_wifi_role_t *role = padapter->phl_role;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(padapter));

	rtw_cmd_ap_stop_req_init(pcmd, &fgreq);

	/* cmd_dispatcher would copy whole phl_cmd_token_req */
	pstatus = rtw_phl_add_cmd_token_req(d->phl, padapter_link->wrlink->hw_band, &fgreq, &padapter->ap_stop_cmd_token);
	if((pstatus != RTW_PHL_STATUS_SUCCESS) &&
	   (pstatus != RTW_PHL_STATUS_PENDING))
		goto error;

	pstatus = RTW_PHL_STATUS_SUCCESS;

error:
	return pstatus;

}

static void _ap_add_del_sta_cmd_done(struct _ADAPTER *padapter)
{
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	struct rtw_wifi_role_t *role = padapter->phl_role;
	enum rtw_phl_status status;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	padapter->ap_add_del_sta_cmd_state = ADD_DEL_STA_ST_IDLE;

	if (!padapter->ap_add_del_sta_cmd_token){
		RTW_ERR("%s : token is NULL!\n", __func__);
		return;
	}

	status = rtw_phl_free_cmd_token(GET_PHL_INFO(d),
					padapter_link->wrlink->hw_band,
					&padapter->ap_add_del_sta_cmd_token);
	padapter->ap_add_del_sta_cmd_token = 0;

	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR(FUNC_ADPT_FMT ": free_cmd_token fail(0x%x)!\n",
			FUNC_ADPT_ARG(padapter), status);
		return;
	}

	RTW_INFO(FUNC_ADPT_FMT": free token success\n", FUNC_ADPT_ARG(padapter));
}

/* use this function must add lock */
static struct rtw_add_del_sta_obj *rtw_ap_get_stainfo_by_add_sta_list(struct _ADAPTER *padapter)
{
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct rtw_add_del_sta_obj *add_del_sta_obj = NULL;
	_list	*phead, *plist;

	phead = &pstapriv->add_sta_list;
	plist = get_next(phead);
	if (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		add_del_sta_obj = LIST_CONTAINOR(plist, struct rtw_add_del_sta_obj, list);
		plist = get_next(plist);
	}

	return add_del_sta_obj;
}

static enum phl_mdl_ret_code _ap_add_del_sta_req_acquired(void *dispr, void *priv)
{
	struct _ADAPTER *padapter = (struct _ADAPTER *)priv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct rtw_wifi_role_t *role = padapter->phl_role;
	struct rtw_add_del_sta_obj *add_del_sta_obj;
	struct sta_info *psta = NULL;
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_msg msg = {0};
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(padapter));

	_rtw_spinlock_bh(&padapter->ap_add_del_sta_lock);
	if (padapter->ap_add_del_sta_cmd_state != ADD_DEL_STA_ST_REQUESTING)
		RTW_ERR(FUNC_ADPT_FMT ": add_del_sta_st=%u, not requesting?!\n",
			FUNC_ADPT_ARG(padapter), padapter->ap_add_del_sta_cmd_state);

	padapter->ap_add_del_sta_cmd_state = ADD_DEL_STA_ST_ACQUIRED;

	add_del_sta_obj = rtw_ap_get_stainfo_by_add_sta_list(padapter);

	_rtw_spinunlock_bh(&padapter->ap_add_del_sta_lock);

	if (!add_del_sta_obj) {
		RTW_ERR(FUNC_ADPT_FMT": add_del_sta_obj is null\n",
			FUNC_ADPT_ARG(padapter));
			return status;
	}

	psta = add_del_sta_obj->sta;
	if (!psta) {
		RTW_ERR(FUNC_ADPT_FMT": psta is null\n", FUNC_ADPT_ARG(padapter));
		return status;
	}

	RTW_INFO(FUNC_ADPT_FMT ": %s STA\n",
		FUNC_ADPT_ARG(padapter), add_del_sta_obj->is_add_sta ? "ADD" : "DEL");

	if (add_del_sta_obj->is_add_sta)
		status = rtw_phl_connect_prepare(adapter_to_dvobj(padapter)->phl,
				padapter_link->wrlink->hw_band, padapter->phl_role,
				padapter_link->wrlink, psta->phl_sta->mac_addr);
	else
		status = rtw_phl_disconnect(adapter_to_dvobj(padapter)->phl,
				padapter_link->wrlink->hw_band,
				padapter->phl_role, _TRUE);

	RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));

	return status;

}

static enum phl_mdl_ret_code _ap_add_del_sta_req_abort(void *dispr, void *priv)
{
	struct _ADAPTER *padapter = (struct _ADAPTER *)priv;


	RTW_INFO("%s\n", __func__);
	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code _ap_add_del_sta_req_ev_hdlr(void *dispr, void *priv, struct phl_msg* msg)
{
	struct _ADAPTER *padapter = (struct _ADAPTER *)priv;
	struct rtw_wifi_role_t *wifi_role = padapter->phl_role;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct rtw_add_del_sta_obj *add_del_sta_obj = NULL;
	struct sta_info *psta = NULL;
	struct phl_msg next_msg = {0};
	_list	*phead, *plist;
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_stainfo_t *phl_stainfo;
#if defined(CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME)
	u8 *passoc_req = NULL;
	u32 assoc_req_len = 0;
#endif
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (MLME_IS_AP(padapter) == _FALSE)
		return MDL_RET_IGNORE;

	RTW_INFO(FUNC_ADPT_FMT ": + msg_id=0x%08x\n",
		FUNC_ADPT_ARG(padapter), msg->msg_id);

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_ADD_DEL_STA) {
		//RTW_INFO(FUNC_ADPT_FMT ": Message is not from ap add sta module, "
		//	 "skip msg_id=0x%08x\n", FUNC_ADPT_ARG(padapter), msg->msg_id);
		RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));
		return MDL_RET_IGNORE;
	}

	_rtw_spinlock_bh(&padapter->ap_add_del_sta_lock);

	RTW_INFO("add_sta_list, cnt:%u\n", pstapriv->add_sta_list_cnt);

	add_del_sta_obj = rtw_ap_get_stainfo_by_add_sta_list(padapter);
	if (!add_del_sta_obj || !add_del_sta_obj->sta) {
		_rtw_spinunlock_bh(&padapter->ap_add_del_sta_lock);
		RTW_ERR(FUNC_ADPT_FMT": add_del_sta_obj or psta is null\n",
			FUNC_ADPT_ARG(padapter));
		goto free_token;
	}
	psta = add_del_sta_obj->sta;
	_rtw_spinunlock_bh(&padapter->ap_add_del_sta_lock);

	RTW_INFO("[AP_STA] %s("ADPT_FMT") aid=%d\n",
		ap_get_evt_str(MSG_EVT_ID_FIELD(msg->msg_id)), ADPT_ARG(padapter), add_del_sta_obj->aid);


	if (IS_MSG_FAIL(msg->msg_id)) {
		RTW_WARN(FUNC_ADPT_FMT ": cmd dispatcher notify cmd failure on "
			 "msg_id=0x%08x\n", FUNC_ADPT_ARG(padapter), msg->msg_id);
	}

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_CONNECT_START:
#if defined(CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME)

		rtw_alloc_stainfo_hw(&padapter->stapriv, psta);
		rtw_hw_connected_apmode(padapter, psta);

		RTW_INFO("indicate_sta_assoc_event to upper layer - hostapd\n");

		_rtw_spinlock_bh(&psta->lock);
		if (psta->passoc_req && psta->assoc_req_len > 0) {
			passoc_req = rtw_zmalloc(psta->assoc_req_len);
			if (passoc_req) {
				assoc_req_len = psta->assoc_req_len;
				_rtw_memcpy(passoc_req, psta->passoc_req, assoc_req_len);
			}
		}
		_rtw_spinunlock_bh(&psta->lock);

		if (passoc_req && assoc_req_len > 0) {
			rtw_cfg80211_indicate_sta_assoc(padapter, passoc_req, assoc_req_len);
			rtw_mfree(passoc_req, assoc_req_len);
		}

		if (is_wep_enc(padapter->securitypriv.dot11PrivacyAlgrthm))
			rtw_ap_wep_pk_setting(padapter, psta);
		#ifdef CONFIG_RTS_FULL_BW
		rtw_set_rts_bw(adapter);
		#endif/*CONFIG_RTS_FULL_BW*/

		status = rtw_phl_connected(adapter_to_dvobj(padapter)->phl,
					padapter_link->wrlink->hw_band,
					padapter->phl_role, psta->phl_sta);
		if (status == RTW_PHL_STATUS_FAILURE) {
			RTW_ERR("%s :: [MSG_EVT_CONNECT_START] rtw_phl_send_msg_to_dispr failed\n",
				__func__);

			goto free_token;
		}
		break;

	case MSG_EVT_DISCONNECT_PREPARE:

		rtw_wfd_st_switch(psta, 0);
		psta->hw_decrypted = _FALSE;
		#ifdef CONFIG_IOCTL_CFG80211
		#ifdef COMPAT_KERNEL_RELEASE

		#elif (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37)) || defined(CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER)
		rtw_cfg80211_indicate_sta_disassoc(padapter, pstadel->macaddr, *(u16 *)pstadel->rsvd);
		#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37)) || defined(CONFIG_CFG80211_FORCE_COMPATIBLE_2_6_37_UNDER) */
		#endif /* CONFIG_IOCTL_CFG80211 */

		rtw_hw_disconnect(padapter, psta);

		rtw_free_mld_stainfo(padapter, psta->phl_sta->mld);

		status = rtw_phl_disconnect(adapter_to_dvobj(padapter)->phl,
					padapter_link->wrlink->hw_band,
					padapter->phl_role, _FALSE);
		if (status == RTW_PHL_STATUS_FAILURE) {
			RTW_ERR("%s :: [MSG_EVT_DISCONNECT_PREPARE] rtw_phl_send_msg_to_dispr failed\n", __func__);
			goto free_token;
		}
		break;
#endif
	case MSG_EVT_CONNECT_END:
	case MSG_EVT_DISCONNECT:

		_rtw_spinlock_bh(&padapter->ap_add_del_sta_lock);

		rtw_list_delete(&add_del_sta_obj->list);
		pstapriv->add_sta_list_cnt--;
		rtw_mfree(add_del_sta_obj, sizeof(struct rtw_add_del_sta_obj));
		add_del_sta_obj = NULL;

		if (pstapriv->add_sta_list_cnt > 0) {	/* token extend */
			add_del_sta_obj = rtw_ap_get_stainfo_by_add_sta_list(padapter);
			if (!add_del_sta_obj || !add_del_sta_obj->sta) {
				_rtw_spinunlock_bh(&padapter->ap_add_del_sta_lock);
				RTW_ERR(FUNC_ADPT_FMT": add_del_sta_obj or psta is null\n",
					FUNC_ADPT_ARG(padapter));
				goto free_token;
			}

			psta = add_del_sta_obj->sta;

			if (add_del_sta_obj->is_add_sta)
				status = rtw_phl_connect_prepare(adapter_to_dvobj(padapter)->phl,
								padapter_link->wrlink->hw_band,
								padapter->phl_role,
								padapter_link->wrlink,
								psta->phl_sta->mac_addr);
			else
				status = rtw_phl_disconnect(adapter_to_dvobj(padapter)->phl,
								padapter_link->wrlink->hw_band,
								padapter->phl_role,
								_TRUE);

			if (status != RTW_PHL_STATUS_SUCCESS) {
				_rtw_spinunlock_bh(&padapter->ap_add_del_sta_lock);
				RTW_ERR("%s :: [%s] rtw_phl_send_msg_to_dispr failed\n",
					__func__,
					(MSG_EVT_ID_FIELD(msg->msg_id) == MSG_EVT_CONNECT_END) ?
					"MSG_EVT_CONNECT_END" : "MSG_EVT_DISCONNECT");
				goto free_token;
			}

		} else {
			_rtw_spinunlock_bh(&padapter->ap_add_del_sta_lock);
			/* no more STA */
			status = RTW_PHL_STATUS_SUCCESS;
			goto free_token;
		}
		_rtw_spinunlock_bh(&padapter->ap_add_del_sta_lock);
		break;

	default:
		/* unknown state */
		RTW_ERR("%s :: [0x%04x] unknown state\n", __func__, MSG_EVT_ID_FIELD(msg->msg_id));
		break;
	}

	return MDL_RET_SUCCESS;

free_token:
	_rtw_spinlock_bh(&padapter->ap_add_del_sta_lock);
	if (add_del_sta_obj) {
		rtw_list_delete(&add_del_sta_obj->list);
		pstapriv->add_sta_list_cnt--;
		rtw_mfree(add_del_sta_obj, sizeof(struct rtw_add_del_sta_obj));
		add_del_sta_obj = NULL;
	}

	_ap_add_del_sta_cmd_done(padapter); /* free token */
	_rtw_spinunlock_bh(&padapter->ap_add_del_sta_lock);
	return status;
}

static enum phl_mdl_ret_code _ap_add_del_sta_req_set_info(void *dispr, void *priv, struct phl_module_op_info* info)
{
	struct _ADAPTER *padapter = (struct _ADAPTER *)priv;

	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(padapter));
	RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(padapter));

	return MDL_RET_IGNORE;
}

static enum phl_mdl_ret_code _ap_add_del_sta_req_query_info(void *dispr, void *priv, struct phl_module_op_info* info)
{
	struct _ADAPTER *a = (struct _ADAPTER *)priv;
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(a);


	RTW_DBG(FUNC_ADPT_FMT ": +\n", FUNC_ADPT_ARG(a));

	switch (info->op_code) {
	case FG_REQ_OP_GET_ROLE:
		info->outbuf = (u8*)a->phl_role;
		ret = MDL_RET_SUCCESS;
		break;

	case FG_REQ_OP_GET_ROLE_LINK:
		info->outbuf = (u8*)alink->wrlink;
		ret = MDL_RET_SUCCESS;
		break;

	default:
		break;
	}

	RTW_DBG(FUNC_ADPT_FMT ": -\n", FUNC_ADPT_ARG(a));
	return ret;
}

void rtw_cmd_ap_add_del_sta_req_free(struct _ADAPTER *padapter)
{
	_rtw_spinlock_free(&padapter->ap_add_del_sta_lock);
}

void rtw_cmd_ap_add_del_sta_req_init(struct _ADAPTER *padapter)
{
	struct phl_cmd_token_req *req;

	RTW_DBG(FUNC_ADPT_FMT ": \n", FUNC_ADPT_ARG(padapter));
	/* Fill foreground command request */

	_rtw_spinlock_init(&padapter->ap_add_del_sta_lock);

	req = &padapter->add_del_sta_req;
	req->module_id= PHL_FG_MDL_AP_ADD_DEL_STA;
	req->priv = padapter;
	req->role = padapter->phl_role;

	req->acquired = _ap_add_del_sta_req_acquired;
	req->abort = _ap_add_del_sta_req_abort;
	req->msg_hdlr = _ap_add_del_sta_req_ev_hdlr;
	req->set_info = _ap_add_del_sta_req_set_info;
	req->query_info = _ap_add_del_sta_req_query_info;

	padapter->ap_add_del_sta_cmd_token = 0;
	padapter->ap_add_del_sta_cmd_state = ADD_DEL_STA_ST_IDLE;
}

enum rtw_phl_status rtw_ap_add_del_sta_cmd(struct _ADAPTER *padapter)
{
	struct phl_cmd_token_req *fgreq;
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	enum rtw_phl_status status;
	struct rtw_wifi_role_t *role = padapter->phl_role;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	fgreq = &padapter->add_del_sta_req;
	/* cmd_dispatcher would copy whole phl_cmd_token_req */
	status = rtw_phl_add_cmd_token_req(d->phl,
				padapter_link->wrlink->hw_band,
				fgreq, &padapter->ap_add_del_sta_cmd_token);

	if ((status != RTW_PHL_STATUS_SUCCESS) &&
	   (status != RTW_PHL_STATUS_PENDING)) {
		RTW_ERR(FUNC_ADPT_FMT": add token fail\n", FUNC_ADPT_ARG(padapter));
		goto exit;
	}

	RTW_INFO(FUNC_ADPT_FMT ": add token success\n", FUNC_ADPT_ARG(padapter));
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

bool rtw_add_del_sta_cmd_check(struct _ADAPTER *padapter, unsigned char *MacAddr, bool add_sta)
{
	enum rtw_phl_status status = _FAIL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct rtw_add_del_sta_obj *add_del_sta_obj = NULL;
	struct sta_info *psta;

	psta = rtw_get_stainfo(&padapter->stapriv, MacAddr);
	if (!psta) {
		RTW_ERR(FUNC_ADPT_FMT": psta is null\n", FUNC_ADPT_ARG(padapter));
		return _FAIL;
	}

	add_del_sta_obj = (struct rtw_add_del_sta_obj *)rtw_zmalloc(sizeof(struct rtw_add_del_sta_obj));
	if (!add_del_sta_obj) {
		RTW_ERR(FUNC_ADPT_FMT": add_del_sta_obj is null\n", FUNC_ADPT_ARG(padapter));
		return _FAIL;
	}

	_rtw_init_listhead(&add_del_sta_obj->list);
	add_del_sta_obj->sta = psta;
	add_del_sta_obj->aid = psta->phl_sta->aid;
	add_del_sta_obj->is_add_sta = add_sta;

	_rtw_spinlock_bh(&padapter->ap_add_del_sta_lock);

	rtw_list_insert_tail(&add_del_sta_obj->list, &pstapriv->add_sta_list);
	pstapriv->add_sta_list_cnt++;

	if (padapter->ap_add_del_sta_cmd_state == ADD_DEL_STA_ST_IDLE) {
		status = rtw_ap_add_del_sta_cmd(padapter);
		if (status == RTW_PHL_STATUS_SUCCESS)
			padapter->ap_add_del_sta_cmd_state = ADD_DEL_STA_ST_REQUESTING;
	} else {
		RTW_INFO(FUNC_ADPT_FMT": reuse token\n", FUNC_ADPT_ARG(padapter));
	}

	_rtw_spinunlock_bh(&padapter->ap_add_del_sta_lock);
	return (status == RTW_PHL_STATUS_SUCCESS) ? _SUCCESS : _FAIL;
}

#endif
#endif	/* CONFIG_AP_MODE */
