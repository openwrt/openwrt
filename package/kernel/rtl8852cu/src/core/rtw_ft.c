/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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

#ifdef CONFIG_RTW_80211R

#ifndef RTW_FT_DBG
	#define RTW_FT_DBG	0
#endif
#if RTW_FT_DBG
	#define RTW_FT_INFO(fmt, arg...)	\
		RTW_INFO(fmt, arg)
	#define RTW_FT_DUMP(str, data, len)	\
		RTW_INFO_DUMP(str, data, len)
#else
	#define RTW_FT_INFO(fmt, arg...) do {} while (0)
	#define RTW_FT_DUMP(str, data, len) do {} while (0)
#endif

void rtw_ft_info_init(struct ft_roam_info *pft)
{
	_rtw_memset(pft, 0, sizeof(struct ft_roam_info));
	pft->ft_flags = 0
		| RTW_FT_EN
		| RTW_FT_OTD_EN
#ifdef CONFIG_RTW_BTM_ROAM
		| RTW_FT_BTM_ROAM
#endif
		;
	pft->ft_updated_bcn = _FALSE;
	RTW_FT_INFO("%s : ft_flags=0x%02x\n", __func__, pft->ft_flags);
}

ssize_t rtw_ft_proc_flags_set(struct file *file,
	const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	char tmp[32];
	u8 flags;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhx", &flags);
		if (num == 1)
			adapter->mlmepriv.ft_roam.ft_flags = flags;
	}

	return count;

}

int rtw_ft_proc_flags_get(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "0x%02x\n", adapter->mlmepriv.ft_roam.ft_flags);

	return 0;
}

u8 rtw_ft_chk_roaming_candidate(
	_adapter *padapter, struct wlan_network *competitor)
{
	u8 *pmdie;
	u32 mdie_len = 0;
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (!(pmdie = rtw_get_ie(&competitor->network.IEs[12], _MDIE_,
		&mdie_len, competitor->network.IELength-12))) {
		RTW_INFO("FT : MDIE not foud in competitor!\n");
		return _FALSE;
	}

	if (!_rtw_memcmp(&pft_roam->mdid, (pmdie+2), 2)) {
		RTW_INFO("FT : unmatched MDIE!\n");
		return _FALSE;
	}

	/*The candidate don't support over-the-DS*/
	if (rtw_ft_valid_otd_candidate(padapter, pmdie)) {
		RTW_INFO("FT: ignore the candidate("
			MAC_FMT ") for over-the-DS\n",
			MAC_ARG(competitor->network.MacAddress));
		/* rtw_ft_clr_flags(padapter, RTW_FT_PEER_OTD_EN); */
		return _FALSE;
	}

	if (rtw_ft_chk_flags(padapter, RTW_FT_TEST_RSSI_ROAM)) {
		if (!_rtw_memcmp(
			padapter_link->mlmepriv.cur_network.network.MacAddress,
			competitor->network.MacAddress, ETH_ALEN)) {
			competitor->network.PhyInfo.rssi +=20;
			RTW_FT_INFO("%s : update "MAC_FMT
				" RSSI to %d for RTW_FT_TEST_RSSI_ROAM\n",
				__func__, MAC_ARG(competitor->network.MacAddress),
				(int)competitor->network.PhyInfo.rssi);
			rtw_ft_clr_flags(padapter, RTW_FT_TEST_RSSI_ROAM);
		}
	}

	return _TRUE;
}

void rtw_ft_update_stainfo(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, WLAN_BSSID_EX *pnetwork)
{
	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct sta_info		*psta = NULL;
	struct rtw_phl_mld_t	*pmld = NULL;

	psta = rtw_get_stainfo(pstapriv, pnetwork->MacAddress);
	if (psta == NULL) {
		pmld = rtw_phl_alloc_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)),
					padapter->phl_role, pnetwork->MacAddress, DTYPE);
		/* main_id is don't care for self sta */
		psta = rtw_alloc_stainfo(pstapriv, pnetwork->MacAddress, DTYPE, 0,
					padapter_link->wrlink->id, PHL_CMD_DIRECTLY);
	}

	if (padapter->securitypriv.dot11AuthAlgrthm ==
		dot11AuthAlgrthm_8021X) {
		padapter_link->securitypriv.binstallGrpkey = _FALSE;
		padapter_link->securitypriv.bgrpkey_handshake = _FALSE;
		padapter->securitypriv.busetkipkey = _FALSE;

		psta->ieee8021x_blocked = _TRUE;
		psta->dot118021XPrivacy = \
			padapter->securitypriv.dot11PrivacyAlgrthm;
		_rtw_memset((u8 *)&psta->dot118021x_UncstKey,
				0, sizeof(union Keytype));
		_rtw_memset((u8 *)&psta->dot11tkiprxmickey,
				0, sizeof(union Keytype));
		_rtw_memset((u8 *)&psta->dot11tkiptxmickey,
				0, sizeof(union Keytype));
	}

}

void rtw_ft_reassoc_event_callback(_adapter *padapter, u8 *pbuf)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct stassoc_event *pstassoc = (struct stassoc_event *)pbuf;
	struct ft_roam_info *pft_roam = &(pmlmepriv->ft_roam);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = (WLAN_BSSID_EX *)&(pmlmeinfo->network);
	struct cfg80211_ft_event_params ft_evt_parms;

	_rtw_memset(&ft_evt_parms, 0, sizeof(ft_evt_parms));
	rtw_ft_update_stainfo(padapter, padapter_link, pnetwork);
	ft_evt_parms.ies_len = pft_roam->ft_event.ies_len;
	ft_evt_parms.ies =  rtw_zmalloc(ft_evt_parms.ies_len);
	if (ft_evt_parms.ies) {
		_rtw_memcpy((void *)ft_evt_parms.ies,
			pft_roam->ft_event.ies, ft_evt_parms.ies_len);
	 } else
		goto err_2;

	ft_evt_parms.target_ap = rtw_zmalloc(ETH_ALEN);
	if (ft_evt_parms.target_ap) {
		_rtw_memcpy((void *)ft_evt_parms.target_ap,
			pstassoc->macaddr, ETH_ALEN);
	} else
		goto err_1;

	ft_evt_parms.ric_ies = pft_roam->ft_event.ric_ies;
	ft_evt_parms.ric_ies_len = pft_roam->ft_event.ric_ies_len;

	/* It's a KERNEL issue between v4.11 ~ v4.16,
	* <= v4.10, NLMSG_DEFAULT_SIZE is used for nlmsg_new().
	* v4.11 ~ v4.16, only used "100 + >ric_ies_len" for nlmsg_new()
	*	even then DRIVER don't support RIC.
	* >= v4.17, issue should correct as "100 + ies_len + ric_ies_len".
	*/
	#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)) && \
	(LINUX_VERSION_CODE < KERNEL_VERSION(4, 17, 0)))
		if (!ft_evt_parms.ric_ies_len)
			ft_evt_parms.ric_ies_len = ft_evt_parms.ies_len;
		else
			ft_evt_parms.ric_ies_len += ft_evt_parms.ies_len;
	#endif

	rtw_ft_lock_set_status(padapter, RTW_FT_AUTHENTICATED_STA);
	rtw_cfg80211_ft_event(padapter, &ft_evt_parms);
	RTW_INFO("%s: to "MAC_FMT"\n", __func__,
			MAC_ARG(ft_evt_parms.target_ap));

	rtw_mfree((u8 *)pft_roam->ft_event.target_ap, ETH_ALEN);
err_1:
	rtw_mfree((u8 *)ft_evt_parms.ies, ft_evt_parms.ies_len);
err_2:
	return;
}

void rtw_ft_validate_akm_type(_adapter  *padapter,
	WLAN_BSSID_EX *network)
{
	struct security_priv *psecuritypriv = &(padapter->securitypriv);
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);
	u32 tmp_len;
	u8 *ptmp;

	/*IEEE802.11-2012 Std. Table 8-101-AKM suite selectors*/
	if (rtw_ft_valid_akm(padapter, psecuritypriv->rsn_akm_suite_type)) {
		ptmp = rtw_get_ie(&network->IEs[12],
				_MDIE_, &tmp_len,
				(network->IELength-12));
		if (ptmp) {
			pft_roam->mdid = *(u16 *)(ptmp+2);
			pft_roam->ft_cap = *(ptmp+4);

			RTW_INFO("FT: target "MAC_FMT
				" mdid=(0x%2x), capacity=(0x%2x)\n",
				MAC_ARG(network->MacAddress),
				pft_roam->mdid, pft_roam->ft_cap);

			rtw_ft_set_flags(padapter, RTW_FT_PEER_EN);

			RTW_FT_INFO("%s : peer support FTOTA(0x%02x),"
				" ft_roam_on_expired=0x%02x\n",
				__func__, pft_roam->ft_flags,
				pft_roam->ft_roam_on_expired);

			if (rtw_ft_otd_roam_en(padapter)) {
				rtw_ft_set_flags(padapter, RTW_FT_PEER_OTD_EN);
				RTW_FT_INFO("%s : peer support FTOTD(0x%02x)\n",
					__func__, pft_roam->ft_flags);
			} else {
				pft_roam->ft_cap &= ~BIT(0);
				rtw_ft_clr_flags(padapter, RTW_FT_PEER_OTD_EN);
				RTW_FT_INFO("%s : update mdid=0x%02x, "
					"cap=0x%02x, flag=0x%02x\n",
					__func__, pft_roam->mdid,
					pft_roam->ft_cap,
					pft_roam->ft_flags);
			}
		} else {
			/* Don't use FT roaming if target AP cannot support FT */
			rtw_ft_clr_flags(padapter,
				(RTW_FT_PEER_EN|RTW_FT_PEER_OTD_EN));
			rtw_ft_reset_status(padapter);
		}
	} else {
		/* It could be a non-FT connection */
		rtw_ft_clr_flags(padapter,
				(RTW_FT_PEER_EN|RTW_FT_PEER_OTD_EN));
		rtw_ft_reset_status(padapter);
	}

	RTW_FT_INFO("%s : ft_flags=0x%02x\n", __func__, pft_roam->ft_flags);
}

void rtw_ft_update_bcn(_adapter *padapter, union recv_frame *precv_frame)
{
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info *pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint len = precv_frame->u.hdr.len;
	WLAN_BSSID_EX *pbss;

	if (rtw_ft_chk_status(padapter,RTW_FT_ASSOCIATED_STA)
		&& (pmlmepriv->ft_roam.ft_updated_bcn == _FALSE)) {
		pbss = (WLAN_BSSID_EX*)rtw_malloc(sizeof(WLAN_BSSID_EX));
		if (pbss) {
			if (collect_bss_info(
				padapter, precv_frame, pbss) == _SUCCESS) {
				struct beacon_keys recv_beacon;

				/* Update adapter network info */
				rtw_update_adapter_network(
					&(pmlmepriv->dev_cur_network.network),
					pbss, padapter, _TRUE);

				rtw_update_network(
					&(padapter_link->mlmepriv.cur_network.network),
					pbss, padapter, _TRUE);

				/* Move into rtw_get_bcn_keys */
				/* rtw_get_bcn_info(&(pmlmepriv->cur_network)); */

				/* update bcn keys */
				if (rtw_get_bcn_keys(padapter, padapter_link, pframe, len,
						&recv_beacon) == _TRUE) {

					RTW_FT_INFO("%s: beacon keys ready\n",
						__func__);

					_rtw_memcpy(
						&padapter_link->mlmepriv.cur_beacon_keys,
						&recv_beacon,
						sizeof(recv_beacon));

					if (is_hidden_ssid(
						recv_beacon.ssid,
						recv_beacon.ssid_len)) {

						_rtw_memcpy(
						padapter_link->mlmepriv.cur_beacon_keys.ssid,
						pmlmeinfo->network.Ssid.Ssid,
						IW_ESSID_MAX_SIZE);

						padapter_link->mlmepriv.cur_beacon_keys.ssid_len = \
						pmlmeinfo->network.Ssid.SsidLength;
					}
				} else {
					RTW_ERR("%s: get beacon keys failed\n",
						__func__);
					_rtw_memset(
						&padapter_link->mlmepriv.cur_beacon_keys,
						0, sizeof(recv_beacon));
				}
				#ifdef CONFIG_BCN_CNT_CONFIRM_HDL
				pmlmepriv->new_beacon_cnts = 0;
				#endif
			}
			rtw_mfree((u8*)pbss, sizeof(WLAN_BSSID_EX));
		}

		/* check the vendor of the assoc AP */
		padapter->mlmeextpriv.mlmext_info.assoc_AP_vendor =
			check_assoc_AP(
				pframe + sizeof(struct rtw_ieee80211_hdr_3addr),
				(len - sizeof(struct rtw_ieee80211_hdr_3addr))
				);

		/* update TSF Value */
		update_TSF(pmlmeext, pframe, len);
		pmlmeext->bcn_cnt = 0;
		pmlmeext->last_bcn_cnt = 0;
		pmlmepriv->ft_roam.ft_updated_bcn = _TRUE;
	}
}

void rtw_ft_start_clnt_join(_adapter *padapter)
{
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct	mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct ft_roam_info *pft_roam = &(pmlmepriv->ft_roam);

	if (rtw_ft_otd_roam(padapter)) {
		pmlmeinfo->state = WIFI_FW_AUTH_SUCCESS | WIFI_FW_STATION_STATE;
		pft_roam->ft_event.ies =
			(pft_roam->ft_action + \
			sizeof(struct rtw_ieee80211_hdr_3addr) + 16);
		pft_roam->ft_event.ies_len =
			(pft_roam->ft_action_len - \
			sizeof(struct rtw_ieee80211_hdr_3addr));

		/*Not support RIC*/
		pft_roam->ft_event.ric_ies =  NULL;
		pft_roam->ft_event.ric_ies_len = 0;
		rtw_ft_report_evt(padapter);
		return;
	}

	pmlmeinfo->state = WIFI_FW_AUTH_NULL | WIFI_FW_STATION_STATE;
	start_clnt_auth(padapter);
}

u8 rtw_ft_update_rsnie(
	_adapter *padapter, u8 bwrite,
	struct pkt_attrib *pattrib, u8 **pframe)
{
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);
	u8 *pie;
	u32 len;

	pie = rtw_get_ie(pft_roam->updated_ft_ies, EID_WPA2, &len,
			pft_roam->updated_ft_ies_len);

	if (!bwrite)
		return (pie)?_SUCCESS:_FAIL;

	if (pie) {
		*pframe = rtw_set_ie(((u8 *)*pframe), EID_WPA2, len,
				pie+2, &(pattrib->pktlen));
	} else
		return _FAIL;

	return _SUCCESS;
}

static u8 rtw_ft_update_mdie(
	_adapter *padapter, struct pkt_attrib *pattrib, u8 **pframe)
{
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);
	u8 *pie, mdie[3];
	u32 len = 3;

	if (rtw_ft_roam(padapter)) {
		if ((pie = rtw_get_ie(pft_roam->updated_ft_ies, _MDIE_,
				&len, pft_roam->updated_ft_ies_len))) {
			pie = (pie + 2); /* ignore md-id & length */
		} else
			return _FAIL;
	} else {
		*((u16 *)&mdie[0]) = pft_roam->mdid;
		mdie[2] = pft_roam->ft_cap;
		pie = &mdie[0];
	}

	*pframe = rtw_set_ie(((u8 *)*pframe), _MDIE_,
			len, pie, &(pattrib->pktlen));
	return _SUCCESS;
}

static u8 rtw_ft_update_ftie(
	_adapter *padapter, struct pkt_attrib *pattrib, u8 **pframe)
{
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);
	u8 *pie;
	u32 len;

	if ((pie = rtw_get_ie(pft_roam->updated_ft_ies, _FTIE_, &len,
				pft_roam->updated_ft_ies_len)) != NULL) {
		*pframe = rtw_set_ie(*pframe, _FTIE_, len,
				(pie+2), &(pattrib->pktlen));
	} else
		return _FAIL;

	return _SUCCESS;
}

void rtw_ft_build_auth_req_ies(_adapter *padapter,
	struct pkt_attrib *pattrib, u8 **pframe)
{
	u8 ftie_append = _TRUE;

	if (!pattrib || !(*pframe))
		return;

	if (!rtw_ft_roam(padapter))
		return;

	ftie_append = rtw_ft_update_rsnie(padapter, _TRUE, pattrib, pframe);
	rtw_ft_update_mdie(padapter, pattrib, pframe);
	if (ftie_append)
		rtw_ft_update_ftie(padapter, pattrib, pframe);
}

void rtw_ft_build_assoc_req_ies(_adapter *padapter,
	u8 is_reassoc, struct pkt_attrib *pattrib, u8 **pframe)
{
	if (!pattrib || !(*pframe))
		return;

	if (rtw_ft_chk_flags(padapter, RTW_FT_PEER_EN))
		rtw_ft_update_mdie(padapter, pattrib, pframe);

	if ((!is_reassoc) || (!rtw_ft_roam(padapter)))
		return;

	if (rtw_ft_update_rsnie(padapter, _FALSE, pattrib, pframe))
		rtw_ft_update_ftie(padapter, pattrib, pframe);
}

u8 rtw_ft_update_auth_rsp_ies(_adapter *padapter, u8 *pframe, u32 len)
{
	u8 ret = _SUCCESS;
	u8 target_ap_addr[ETH_ALEN] = {0};
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct ft_roam_info *pft_roam = &(pmlmepriv->ft_roam);

	if (!rtw_ft_roam(padapter))
		return _FAIL;

	/*rtw_ft_report_reassoc_evt already,
	 * and waiting for cfg80211_rtw_update_ft_ies */
	if (rtw_ft_authed_sta(padapter))
		return ret;

	if (!pframe || !len)
		return _FAIL;

	rtw_buf_update(&pmlmepriv->auth_rsp,
		&pmlmepriv->auth_rsp_len, pframe, len);
	pft_roam->ft_event.ies =
		(pmlmepriv->auth_rsp + \
		sizeof(struct rtw_ieee80211_hdr_3addr) + 6);
	pft_roam->ft_event.ies_len =
		(pmlmepriv->auth_rsp_len - \
		sizeof(struct rtw_ieee80211_hdr_3addr) - 6);

	/*Not support RIC*/
	pft_roam->ft_event.ric_ies =  NULL;
	pft_roam->ft_event.ric_ies_len =  0;
	_rtw_memcpy(target_ap_addr, pmlmepriv->assoc_bssid, ETH_ALEN);
	rtw_ft_report_reassoc_evt(padapter, target_ap_addr);

	return ret;
}

static void rtw_ft_start_clnt_action(_adapter *padapter, u8 *pTargetAddr)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;

	rtw_ft_set_status(padapter, RTW_FT_REQUESTING_STA);
	rtw_ft_issue_action_req(padapter, pTargetAddr);
	_set_timer(&pmlmeext->ft_link_timer, REASSOC_TO);
}

void rtw_ft_start_roam(_adapter *padapter, u8 *pTargetAddr)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;

	if (rtw_ft_otd_roam(padapter)) {
		RTW_FT_INFO("%s : try OTD roaming\n", __func__);
		rtw_ft_start_clnt_action(padapter, pTargetAddr);
	} else {
		/* wait a little time to retrieve packets buffered
			in the current ap while scan*/
		RTW_FT_INFO("%s : start roaming timer\n", __func__);
		_set_timer(&pmlmeext->ft_roam_timer, 30);
	}
}

void rtw_ft_issue_action_req(_adapter *padapter, u8 *pTargetAddr)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct xmit_priv *pxmitpriv = &(padapter->xmitpriv);
	struct xmit_frame *pmgntframe;
	struct rtw_ieee80211_hdr *pwlanhdr;
	struct pkt_attrib *pattrib;
	u8 *pframe;
	u8 category = RTW_WLAN_CATEGORY_FT;
	u8 action = RTW_WLAN_ACTION_FT_REQ;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return;

	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);
	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;
	pwlanhdr->frame_ctl = 0;

	_rtw_memcpy(pwlanhdr->addr1, get_my_bssid(&pmlmeinfo->network), ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&pmlmeinfo->network), ETH_ALEN);

	SetSeqNum(pwlanhdr, padapter->mlmeextpriv.mgnt_seq);
	padapter->mlmeextpriv.mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_ACTION);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	pframe = rtw_set_fixed_ie(pframe, 1, &(category), &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 1, &(action), &(pattrib->pktlen));

	/* ToDo CONFIG_RTW_MLD: MLD address or link address */
	_rtw_memcpy(pframe, adapter_mac_addr(padapter), ETH_ALEN);
	pframe += ETH_ALEN;
	pattrib->pktlen += ETH_ALEN;

	_rtw_memcpy(pframe, pTargetAddr, ETH_ALEN);
	pframe += ETH_ALEN;
	pattrib->pktlen += ETH_ALEN;

	rtw_ft_update_mdie(padapter, pattrib, &pframe);
	if (rtw_ft_update_rsnie(padapter, _TRUE, pattrib, &pframe))
		rtw_ft_update_ftie(padapter, pattrib, &pframe);

	RTW_INFO("FT : issue RTW_WLAN_ACTION_FT_REQ\n");
	pattrib->last_txcmdsz = pattrib->pktlen;
	dump_mgntframe(padapter, pmgntframe);
}

void rtw_ft_report_evt(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct ft_roam_info *pft_roam = &(pmlmepriv->ft_roam);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *pnetwork = (WLAN_BSSID_EX *)&(pmlmeinfo->network);
	struct cfg80211_ft_event_params ft_evt_parms;

	_rtw_memset(&ft_evt_parms, 0, sizeof(ft_evt_parms));
	rtw_ft_update_stainfo(padapter, padapter_link, pnetwork);

	if (!pnetwork)
		goto err_2;

	ft_evt_parms.ies_len = pft_roam->ft_event.ies_len;
	ft_evt_parms.ies =  rtw_zmalloc(ft_evt_parms.ies_len);
	if (ft_evt_parms.ies) {
		_rtw_memcpy((void *)ft_evt_parms.ies,
			pft_roam->ft_event.ies, ft_evt_parms.ies_len);
	 } else
		goto err_2;

	ft_evt_parms.target_ap = rtw_zmalloc(ETH_ALEN);
	if (ft_evt_parms.target_ap) {
		_rtw_memcpy((void *)ft_evt_parms.target_ap,
			pnetwork->MacAddress, ETH_ALEN);
	} else
		goto err_1;

	ft_evt_parms.ric_ies = pft_roam->ft_event.ric_ies;
	ft_evt_parms.ric_ies_len = pft_roam->ft_event.ric_ies_len;

	/* It's a KERNEL issue between v4.11 ~ v4.16,
	* <= v4.10, NLMSG_DEFAULT_SIZE is used for nlmsg_new().
	* v4.11 ~ v4.16, only used "100 + >ric_ies_len" for nlmsg_new()
	*	even then DRIVER don't support RIC.
	* >= v4.17, issue should correct as "100 + ies_len + ric_ies_len".
	*/
	#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)) && \
	(LINUX_VERSION_CODE < KERNEL_VERSION(4, 17, 0)))
		ft_evt_parms.ric_ies_len = (ft_evt_parms.ies_len <= 100 )?
			(0):(ft_evt_parms.ies_len - 100);
	#endif

	rtw_ft_lock_set_status(padapter, RTW_FT_AUTHENTICATED_STA);
	rtw_cfg80211_ft_event(padapter, &ft_evt_parms);
	RTW_INFO("FT: rtw_ft_report_evt\n");
	rtw_mfree((u8 *)pft_roam->ft_event.target_ap, ETH_ALEN);
err_1:
	rtw_mfree((u8 *)ft_evt_parms.ies, ft_evt_parms.ies_len);
err_2:
	return;
}

void rtw_ft_report_reassoc_evt(_adapter *padapter, u8 *pMacAddr)
{
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct cmd_priv *pcmdpriv = &(adapter_to_dvobj(padapter)->cmdpriv);
	struct cmd_obj *pcmd_obj = NULL;
	struct stassoc_event *passoc_sta_evt = NULL;
	struct rtw_evt_header *evt_hdr = NULL;
	u8 *pevtcmd = NULL;
	u32 cmdsz = 0;

	pcmd_obj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmd_obj == NULL)
		return;

	cmdsz = (sizeof(struct stassoc_event) + sizeof(struct rtw_evt_header));
	pevtcmd = (u8 *)rtw_zmalloc(cmdsz);
	if (pevtcmd == NULL) {
		rtw_mfree((u8 *)pcmd_obj, sizeof(struct cmd_obj));
		return;
	}

	_rtw_init_listhead(&pcmd_obj->list);
	pcmd_obj->padapter = padapter;
	pcmd_obj->band_idx = HW_BAND_MAX;
	pcmd_obj->cmdcode = CMD_SET_MLME_EVT;
	pcmd_obj->cmdsz = cmdsz;
	pcmd_obj->parmbuf = pevtcmd;
	pcmd_obj->rsp = NULL;
	pcmd_obj->rspsz  = 0;

	evt_hdr = (struct rtw_evt_header *)(pevtcmd);
	evt_hdr->len = sizeof(struct stassoc_event);
	evt_hdr->id = EVT_FT_REASSOC;
	evt_hdr->seq = ATOMIC_INC_RETURN(&pmlmeext->event_seq);

	passoc_sta_evt = \
			(struct stassoc_event *)(pevtcmd + \
			sizeof(struct rtw_evt_header));
	_rtw_memcpy((unsigned char *)(&(passoc_sta_evt->macaddr)),
			pMacAddr, ETH_ALEN);
	rtw_enqueue_cmd(pcmdpriv, pcmd_obj);
}

void rtw_ft_link_timer_hdl(void *ctx)
{
	_adapter *padapter = (_adapter *)ctx;
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct ft_roam_info *pft_roam = &(pmlmepriv->ft_roam);

	if (rtw_ft_chk_status(padapter, RTW_FT_REQUESTING_STA)) {
		if (pft_roam->ft_req_retry_cnt < RTW_FT_ACTION_REQ_LMT) {
			pft_roam->ft_req_retry_cnt++;
			rtw_ft_issue_action_req(padapter,
			(u8 *)pmlmepriv->roam_network->network.MacAddress);
			_set_timer(&pmlmeext->ft_link_timer, REASSOC_TO);
		} else {
			pft_roam->ft_req_retry_cnt = 0;
			if (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) {
				rtw_ft_set_status(padapter,
					RTW_FT_ASSOCIATED_STA);
			} else
				rtw_ft_reset_status(padapter);
		}
	}
}

void rtw_ft_roam_timer_hdl(void *ctx)
{
	_adapter *padapter = (_adapter *)ctx;
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	RTW_FT_INFO("%s : try roaming\n", __func__);
	receive_disconnect(padapter,
			pmlmepriv->cur_network.network.MacAddress,
			WLAN_REASON_ACTIVE_ROAM, _FALSE);
	pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
	pmlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_FT;
	pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
}

void rtw_ft_roam_status_reset(_adapter *padapter)
{
	struct ft_roam_info *pft_roam = &(padapter->mlmepriv.ft_roam);

	if ((rtw_to_roam(padapter) > 0) &&
		(!rtw_ft_chk_status(padapter, RTW_FT_REQUESTED_STA))) {
		rtw_ft_reset_status(padapter);
	}

	padapter->mlmepriv.ft_roam.ft_updated_bcn = _FALSE;
}

void rtw_ft_peer_info_init(struct sta_info *psta)
{
	struct rtw_sta_ft_info_t *peer = &(psta->ft_peer);
	_rtw_memset(peer, 0, sizeof(struct rtw_sta_ft_info_t));
}

void rtw_ft_peer_info_free(struct sta_info *psta)
{
	struct rtw_sta_ft_info_t *peer = &(psta->ft_peer);

	rtw_buf_free(&peer->md_ie, &peer->md_len);
	rtw_buf_free(&peer->rsn_ie, &peer->rsn_len);
	rtw_buf_free(&peer->ft_ie, &peer->ft_len);
}

int rtw_ft_update_sta_ies(_adapter *padapter,
	struct cfg80211_update_ft_ies_params *ie)
{
	struct security_priv *psecuritypriv = &(padapter->securitypriv);
	struct sta_priv *pstapriv = &(padapter->stapriv);
	struct sta_info *psta;
	struct rtw_sta_ft_info_t *peer_info;
	u8 *ptr, *paddr;
	u32 len;
	int ret = 0;

	if (!CHECK_BIT(WLAN_AKM_TYPE_FT_PSK, psecuritypriv->akmp)
		&& !CHECK_BIT(WLAN_AKM_TYPE_FT_8021X, psecuritypriv->akmp)
		&& !CHECK_BIT(WLAN_AKM_TYPE_FT_OVER_SAE, psecuritypriv->akmp)) {
		RTW_FT_INFO("%s : AKM=0x%x\n", __func__, psecuritypriv->akmp);
		goto exit;
	}

	if ((ie->ie_len == 0) ||
		(ie->ie_len >= RTW_FT_MAX_IE_SZ)) {
		ret = - EINVAL;
		goto exit;
	}

	if ((ptr = rtw_get_ie(ie->ie, WLAN_EID_VENDOR_SPECIFIC,
			&len , ie->ie_len)) == NULL) {
		RTW_ERR("FT : no station mac address found in ies\n");
		ret = -EINVAL;
		goto exit;
	}

	if (len != ETH_ALEN) {
		RTW_ERR("FT : invalid station mac length(%u)\n", len);
		ret = -EINVAL;
		goto exit;
	}

	paddr = ptr + 2;
	if ((psta = rtw_get_stainfo(pstapriv, paddr)) == NULL) {
		RTW_ERR("FT : sta "MAC_FMT" not found!\n", MAC_ARG(paddr));
		ret = -EINVAL;
		goto exit;
	}

	RTW_FT_INFO("%s : update sta "MAC_FMT" ie, len=%lu\n",
			__func__, MAC_ARG(paddr), ie->ie_len);
	RTW_FT_DUMP("ie = ", ie->ie, ie->ie_len);

	peer_info = &(psta->ft_peer);
	if ((ptr = rtw_get_ie(ie->ie, EID_WPA2, &len , ie->ie_len)) != NULL) {
		rtw_buf_update(&peer_info->rsn_ie,
			&peer_info->rsn_len, ptr + 2, len);
		RTW_FT_DUMP("rsn_ie = ", peer_info->rsn_ie,
			peer_info->rsn_len);
	}

	if ((ptr = rtw_get_ie(ie->ie, _MDIE_, &len , ie->ie_len)) != NULL) {
		rtw_buf_update(&peer_info->md_ie,
			&peer_info->md_len, ptr + 2, len);
		RTW_FT_DUMP("md_ie = ",
			peer_info->md_ie, peer_info->md_len);
	}

	if ((ptr = rtw_get_ie(ie->ie, _FTIE_, &len , ie->ie_len)) != NULL) {
		rtw_buf_update(&peer_info->ft_ie,
			&peer_info->ft_len, ptr + 2, len);
		RTW_FT_DUMP("ft_ie = ",
			peer_info->ft_ie, peer_info->ft_len);
	}

	if (psta->authalg == WLAN_AUTH_FT)
		issue_asocrsp(padapter, 0, psta, WIFI_REASSOCRSP);
exit:
	return ret;
}

void rtw_ft_update_assocresp_ies(struct net_device *net,
	struct cfg80211_ap_settings *settings)
{
	_adapter *padapter = (_adapter *)rtw_netdev_priv(net);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	struct ft_roam_info *pinfo = &(pmlmepriv->ft_roam);
	u8 *ie, *bcn_tail_ies, *assocrsp_ies;
	int ielen, bcn_tail_len, assocrsp_ies_len, ret = 0;

	bcn_tail_ies = (u8 *)settings->beacon.tail;
	bcn_tail_len = (u32)settings->beacon.tail_len;
	assocrsp_ies = (u8 *)settings->beacon.assocresp_ies;
	assocrsp_ies_len = (u32)settings->beacon.assocresp_ies_len;

	if ((bcn_tail_len > 0) &&
		((ie = rtw_get_ie(bcn_tail_ies, _MDIE_,
			&ielen , bcn_tail_len)) != NULL)) {
		pinfo->mdid = *(u16 *)(ie + 2);
		pinfo->ft_cap = *(ie + 4);
		RTW_FT_INFO("%s : mdid = 0x%04x, cap=0x%02x\n",
			__func__, pinfo->mdid, pinfo->ft_cap);
	}

	if ((assocrsp_ies_len > 0) &&
		((ie = rtw_get_ie(assocrsp_ies, _FTIE_,
			&ielen , assocrsp_ies_len)) != NULL)) {
			_rtw_memset(pinfo->updated_ft_ies, 0, RTW_FT_MAX_IE_SZ);
			_rtw_memcpy(pinfo->updated_ft_ies, ie + 2, ielen);
			pinfo->updated_ft_ies_len  = ielen;
			RTW_FT_DUMP("FT : ft ie = ", ie + 2, ielen);
	}

}

void rtw_ft_process_ft_auth_rsp(_adapter *padapter, u8 *pframe, u32 len)
{
	struct sta_priv *pstapriv = &(padapter->stapriv);
	struct sta_info *psta = NULL;
	u8 *ptr;
	u32 plen;

	ptr = pframe + IEEE80211_3ADDR_LEN + _AUTH_IE_OFFSET_;
	plen = len - IEEE80211_3ADDR_LEN - _AUTH_IE_OFFSET_;

	psta = rtw_get_stainfo(pstapriv, GetAddr1Ptr(pframe));
	if (psta) {
		_rtw_spinlock_bh(&psta->lock);
		psta->state &= ~WIFI_FW_AUTH_NULL;
		psta->state |= WIFI_FW_AUTH_SUCCESS;
		psta->expire_to = pstapriv->assoc_to;
		_rtw_spinunlock_bh(&psta->lock);
	}
}

void rtw_ft_build_assoc_rsp_ies(_adapter *padapter,
	struct sta_info *psta, struct pkt_attrib *pattrib, u8 **pframe)
{
	struct ft_roam_info *prinfo = &(padapter->mlmepriv.ft_roam);
	struct rtw_sta_ft_info_t *peer_info;
	u8 *pie, mdie[3] = {0};
	u32 ielen;

	if (psta == NULL)
		return;

	if (psta->authalg != WLAN_AUTH_FT) {
		*((u16 *)&mdie[0]) = prinfo->mdid;
		mdie[2] = prinfo->ft_cap;
		pie = mdie;
		ielen = 3;

		if (prinfo->mdid > 0) {
		*pframe = rtw_set_ie(((u8 *)*pframe),
			_MDIE_, ielen , pie, &(pattrib->pktlen));
		}

		if (prinfo->updated_ft_ies_len > 0) {
		*pframe = rtw_set_ie(((u8 *)*pframe),
			_FTIE_, prinfo->updated_ft_ies_len,
			prinfo->updated_ft_ies, &(pattrib->pktlen));
		}
	} else {
		peer_info = &(psta->ft_peer);
		if (peer_info->rsn_len > 0) {
			*pframe = rtw_set_ie(((u8 *)*pframe), EID_WPA2,
					peer_info->rsn_len, peer_info->rsn_ie,
					&(pattrib->pktlen));
		}

		if (peer_info->md_len > 0) {
			*pframe = rtw_set_ie(((u8 *)*pframe), _MDIE_,
					peer_info->md_len, peer_info->md_ie,
					&(pattrib->pktlen));
		}

		if (peer_info->ft_len > 0) {
			*pframe = rtw_set_ie(((u8 *)*pframe), _FTIE_,
					peer_info->ft_len, peer_info->ft_ie,
					&(pattrib->pktlen));
		}
	}

}
#endif /* CONFIG_RTW_80211R */
