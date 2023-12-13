/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
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
#define _RTW_WLAN_UTIL_C_

#include <drv_types.h>

unsigned char ARTHEROS_OUI1[] = {0x00, 0x03, 0x7f};
unsigned char ARTHEROS_OUI2[] = {0x00, 0x13, 0x74};

unsigned char BROADCOM_OUI1[] = {0x00, 0x10, 0x18};
unsigned char BROADCOM_OUI2[] = {0x00, 0x0a, 0xf7};
unsigned char BROADCOM_OUI3[] = {0x00, 0x05, 0xb5};


unsigned char CISCO_OUI[] = {0x00, 0x40, 0x96};
unsigned char MARVELL_OUI[] = {0x00, 0x50, 0x43};
unsigned char RALINK_OUI[] = {0x00, 0x0c, 0x43};
unsigned char REALTEK_OUI[] = {0x00, 0xe0, 0x4c};
unsigned char AIRGOCAP_OUI[] = {0x00, 0x0a, 0xf5};

unsigned char REALTEK_96B_IE[] = {0x00, 0xe0, 0x4c, 0x02, 0x01, 0x20};

extern unsigned char RTW_WPA_OUI[];
extern unsigned char WPA_TKIP_CIPHER[4];
extern unsigned char RSN_TKIP_CIPHER[4];

#define R2T_PHY_DELAY	(0)

/* #define WAIT_FOR_BCN_TO_MIN	(3000) */
#define WAIT_FOR_BCN_TO_MIN	(6000)
#define WAIT_FOR_BCN_TO_MAX	(20000)

static u8 rtw_basic_rate_cck[4] = {
	IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK, IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK, IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK
};

static u8 rtw_basic_rate_ofdm[3] = {
	IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK, IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

static u8 rtw_basic_rate_mix[7] = {
	IEEE80211_CCK_RATE_1MB | IEEE80211_BASIC_RATE_MASK, IEEE80211_CCK_RATE_2MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_CCK_RATE_5MB | IEEE80211_BASIC_RATE_MASK, IEEE80211_CCK_RATE_11MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_6MB | IEEE80211_BASIC_RATE_MASK, IEEE80211_OFDM_RATE_12MB | IEEE80211_BASIC_RATE_MASK,
	IEEE80211_OFDM_RATE_24MB | IEEE80211_BASIC_RATE_MASK
};

extern u8	WIFI_CCKRATES[];
bool rtw_is_cck_rate(u8 rate)
{
	int i;

	for (i = 0; i < 4; i++)
		if ((WIFI_CCKRATES[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}

extern u8	WIFI_OFDMRATES[];
bool rtw_is_ofdm_rate(u8 rate)
{
	int i;

	for (i = 0; i < 8; i++)
		if ((WIFI_OFDMRATES[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}

/* test if rate is defined in rtw_basic_rate_cck */
bool rtw_is_basic_rate_cck(u8 rate)
{
	int i;

	for (i = 0; i < 4; i++)
		if ((rtw_basic_rate_cck[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}

/* test if rate is defined in rtw_basic_rate_ofdm */
bool rtw_is_basic_rate_ofdm(u8 rate)
{
	int i;

	for (i = 0; i < 3; i++)
		if ((rtw_basic_rate_ofdm[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}

/* test if rate is defined in rtw_basic_rate_mix */
bool rtw_is_basic_rate_mix(u8 rate)
{
	int i;

	for (i = 0; i < 7; i++)
		if ((rtw_basic_rate_mix[i] & 0x7F) == (rate & 0x7F))
			return 1;
	return 0;
}
int cckrates_included(unsigned char *rate, int ratelen)
{
	int	i;

	for (i = 0; i < ratelen; i++) {
		if ((((rate[i]) & 0x7f) == 2)	|| (((rate[i]) & 0x7f) == 4) ||
		    (((rate[i]) & 0x7f) == 11)  || (((rate[i]) & 0x7f) == 22))
			return _TRUE;
	}

	return _FALSE;

}

int cckratesonly_included(unsigned char *rate, int ratelen)
{
	int	i;

	for (i = 0; i < ratelen; i++) {
		if ((((rate[i]) & 0x7f) != 2) && (((rate[i]) & 0x7f) != 4) &&
		    (((rate[i]) & 0x7f) != 11)  && (((rate[i]) & 0x7f) != 22))
			return _FALSE;
	}

	return _TRUE;
}

s8 rtw_get_sta_rx_nss(_adapter *adapter, struct sta_info *psta)
{
	s8 nss = 1;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;

	if (!psta)
		return nss;

	nss = get_phy_rx_nss(adapter, padapter_link);

#ifdef CONFIG_80211N_HT
	#ifdef CONFIG_80211AC_VHT
	#ifdef CONFIG_80211AX_HE
	if (psta->hepriv.he_option)
		nss = psta->phl_sta->asoc_cap.nss_tx;
	else
	#endif /* CONFIG_80211AX_HE */
	if (psta->vhtpriv.vht_option)
		nss = rtw_min(nss, rtw_vht_mcsmap_to_nss(psta->vhtpriv.vht_mcs_map));
	else
	#endif /* CONFIG_80211AC_VHT */
	if (psta->htpriv.ht_option)
		nss = rtw_min(nss, rtw_ht_mcsset_to_nss(psta->htpriv.ht_cap.supp_mcs_set));
#endif /*CONFIG_80211N_HT*/
	RTW_INFO("%s: %d ss\n", __func__, nss);
	return nss;
}

s8 rtw_get_sta_tx_nss(_adapter *adapter, struct sta_info *psta)
{
	s8 nss = 1;
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;

	if (!psta)
		return nss;

	nss = get_phy_tx_nss(adapter, padapter_link);

#ifdef CONFIG_80211N_HT
	#ifdef CONFIG_80211AC_VHT
	#ifdef CONFIG_80211AX_HE
	if (psta->hepriv.he_option)
		nss = psta->phl_sta->asoc_cap.nss_rx;
	else
	#endif /* CONFIG_80211AX_HE */
	if (psta->vhtpriv.vht_option)
		nss = rtw_min(nss, rtw_vht_mcsmap_to_nss(psta->vhtpriv.vht_mcs_map));
	else
	#endif /* CONFIG_80211AC_VHT */
	if (psta->htpriv.ht_option)
		nss = rtw_min(nss, rtw_ht_mcsset_to_nss(psta->htpriv.ht_cap.supp_mcs_set));
#endif /*CONFIG_80211N_HT*/
	RTW_INFO("%s: %d SS\n", __func__, nss);
	return nss;
}

unsigned char ratetbl_val_2wifirate(unsigned char rate)
{
	unsigned char val = 0;

	switch (rate & 0x7f) {
	case 0:
		val = IEEE80211_CCK_RATE_1MB;
		break;

	case 1:
		val = IEEE80211_CCK_RATE_2MB;
		break;

	case 2:
		val = IEEE80211_CCK_RATE_5MB;
		break;

	case 3:
		val = IEEE80211_CCK_RATE_11MB;
		break;

	case 4:
		val = IEEE80211_OFDM_RATE_6MB;
		break;

	case 5:
		val = IEEE80211_OFDM_RATE_9MB;
		break;

	case 6:
		val = IEEE80211_OFDM_RATE_12MB;
		break;

	case 7:
		val = IEEE80211_OFDM_RATE_18MB;
		break;

	case 8:
		val = IEEE80211_OFDM_RATE_24MB;
		break;

	case 9:
		val = IEEE80211_OFDM_RATE_36MB;
		break;

	case 10:
		val = IEEE80211_OFDM_RATE_48MB;
		break;

	case 11:
		val = IEEE80211_OFDM_RATE_54MB;
		break;

	}

	return val;

}

int is_basicrate(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, unsigned char rate)
{
	int i;
	unsigned char val;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;

	for (i = 0; i < NumRates; i++) {
		val = pmlmeext->basicrate[i];

		if ((val != 0xff) && (val != 0xfe)) {
			if (rate == ratetbl_val_2wifirate(val))
				return _TRUE;
		}
	}

	return _FALSE;
}

unsigned int ratetbl2rateset(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
				unsigned char *rateset)
{
	int i;
	unsigned char rate;
	unsigned int	len = 0;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;

	for (i = 0; i < NumRates; i++) {
		rate = pmlmeext->datarate[i];

		if (rtw_get_oper_ch(padapter, padapter_link) > 14 && rate < _6M_RATE_) /*5G no support CCK rate*/
			continue;

		switch (rate) {
		case 0xff:
			return len;

		case 0xfe:
			continue;

		default:
			rate = ratetbl_val_2wifirate(rate);

			if (is_basicrate(padapter, padapter_link, rate) == _TRUE)
				rate |= IEEE80211_BASIC_RATE_MASK;

			rateset[len] = rate;
			len++;
			break;
		}
	}
	return len;
}

void get_rate_set(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			unsigned char *pbssrate, int *bssrate_len)
{
	unsigned char supportedrates[NumRates];

	_rtw_memset(supportedrates, 0, NumRates);
	*bssrate_len = ratetbl2rateset(padapter, padapter_link, supportedrates);
	_rtw_memcpy(pbssrate, supportedrates, *bssrate_len);
}

void set_mcs_rate_by_mask(u8 *mcs_set, u32 mask)
{
	u8 mcs_rate_1r = (u8)(mask & 0xff);
	u8 mcs_rate_2r = (u8)((mask >> 8) & 0xff);
	u8 mcs_rate_3r = (u8)((mask >> 16) & 0xff);
	u8 mcs_rate_4r = (u8)((mask >> 24) & 0xff);

	mcs_set[0] &= mcs_rate_1r;
	mcs_set[1] &= mcs_rate_2r;
	mcs_set[2] &= mcs_rate_3r;
	mcs_set[3] &= mcs_rate_4r;
}

void UpdateBrateTbl(
	_adapter *adapter,
	u8			*mBratesOS
)
{
	u8	i;
	u8	rate;

	/* 1M, 2M, 5.5M, 11M, 6M, 12M, 24M are mandatory. */
	for (i = 0; i < NDIS_802_11_LENGTH_RATES_EX; i++) {
		rate = mBratesOS[i] & 0x7f;
		switch (rate) {
		case IEEE80211_CCK_RATE_1MB:
		case IEEE80211_CCK_RATE_2MB:
		case IEEE80211_CCK_RATE_5MB:
		case IEEE80211_CCK_RATE_11MB:
		case IEEE80211_OFDM_RATE_6MB:
		case IEEE80211_OFDM_RATE_12MB:
		case IEEE80211_OFDM_RATE_24MB:
			mBratesOS[i] |= IEEE80211_BASIC_RATE_MASK;
			break;
		}
	}

}

void UpdateBrateTblForSoftAP(u8 *bssrateset, u32 bssratelen)
{
	u8	i;
	u8	rate;

	for (i = 0; i < bssratelen; i++) {
		rate = bssrateset[i] & 0x7f;
		switch (rate) {
		case IEEE80211_CCK_RATE_1MB:
		case IEEE80211_CCK_RATE_2MB:
		case IEEE80211_CCK_RATE_5MB:
		case IEEE80211_CCK_RATE_11MB:
			bssrateset[i] |= IEEE80211_BASIC_RATE_MASK;
			break;
		}
	}

}
/*rtw_phl_mr_get_chandef(dvobj->phl, adapter->phl_role, &chandef); => mr union chan*/
/*rtw_phl_get_cur_hal_chdef(adapter->phl_role, &chandef) => hal chan*/

void rtw_get_oper_chdef(_adapter *adapter, struct _ADAPTER_LINK *adapter_link, struct rtw_chan_def *chandef)
{
	if (!adapter->phl_role)
		return;

	if (rtw_phl_get_cur_hal_chdef(adapter->phl_role, adapter_link->wrlink, chandef) != RTW_PHL_STATUS_SUCCESS)
		RTW_ERR("%s failed\n", __func__);
}

u8 rtw_get_oper_band(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	struct rtw_chan_def cur_chandef = {0};

	rtw_get_oper_chdef(adapter, adapter_link, &cur_chandef);
	return cur_chandef.band;
}

u8 rtw_get_oper_ch(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	struct rtw_chan_def cur_chandef = {0};

	rtw_get_oper_chdef(adapter, adapter_link, &cur_chandef);
	return cur_chandef.chan;
}

u8 rtw_get_oper_bw(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	struct rtw_chan_def cur_chandef = {0};

	rtw_get_oper_chdef(adapter, adapter_link, &cur_chandef);
	return cur_chandef.bw;
}

u8 rtw_get_oper_choffset(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	struct rtw_chan_def cur_chandef = {0};

	rtw_get_oper_chdef(adapter, adapter_link, &cur_chandef);
	return cur_chandef.offset;
}

int rtw_get_oper_chdef_by_hwband(struct dvobj_priv *dvobj, enum phl_band_idx band_idx
	, struct rtw_chan_def *chandef)
{
	if (rtw_phl_get_cur_hal_chdef_by_hwband(GET_PHL_INFO(dvobj), band_idx, chandef) != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s failed\n", __func__);
		return _FAIL;
	}
	return _SUCCESS;
}

int rtw_get_oper_bchbw_by_hwband(struct dvobj_priv *dvobj, enum phl_band_idx band_idx
	, enum band_type *band, u8 *ch, u8 *bw, u8 *offset)
{
	struct rtw_chan_def chdef;

	if (rtw_phl_get_cur_hal_chdef_by_hwband(GET_PHL_INFO(dvobj), band_idx, &chdef) != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s failed\n", __func__);
		*ch = 0;
		return _FAIL;
	}

	*band = chdef.band;
	*ch = chdef.chan;
	*bw = chdef.bw;
	*offset = chdef.offset;
	return _SUCCESS;
}

inline systime rtw_get_on_oper_ch_time(_adapter *adapter)
{
	return adapter_to_dvobj(adapter)->on_oper_ch_time;
}

inline systime rtw_get_on_cur_ch_time(_adapter *adapter)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);

	if (adapter_link && adapter_link->mlmeextpriv.chandef.chan == rtw_get_oper_ch(adapter, adapter_link))
		return adapter_to_dvobj(adapter)->on_oper_ch_time;
	else
		return 0;
}

#if CONFIG_ALLOW_FUNC_2G_5G_ONLY
RTW_FUNC_2G_5G_ONLY void set_channel_bwmode(_adapter *padapter,
				struct _ADAPTER_LINK *padapter_link,
				unsigned char channel,
				unsigned char channel_offset,
				unsigned short bwmode,
				u8 do_rfk)
{
	rtw_hw_set_ch_bw(padapter, padapter_link, (channel > 14) ? BAND_ON_5G : BAND_ON_24G,
		channel, (enum channel_width)bwmode, channel_offset, do_rfk);
}
#endif

void set_bch_bwmode(_adapter *padapter,
				struct _ADAPTER_LINK *padapter_link,
				enum band_type band,
				unsigned char channel,
				unsigned char channel_offset,
				unsigned short bwmode,
				u8 do_rfk)
{
	rtw_hw_set_ch_bw(padapter, padapter_link, band,
		channel, (enum channel_width)bwmode, channel_offset, do_rfk);
}

__inline u8 *get_my_bssid(WLAN_BSSID_EX *pnetwork)
{
	return pnetwork->MacAddress;
}

u16 get_beacon_interval(WLAN_BSSID_EX *bss)
{
	unsigned short val;
	_rtw_memcpy((unsigned char *)&val, rtw_get_beacon_interval_from_ie(bss->IEs), 2);

	return le16_to_cpu(val);

}

int is_client_associated_to_ap(_adapter *padapter)
{
	struct mlme_ext_priv	*pmlmeext;
	struct mlme_ext_info	*pmlmeinfo;

	if (!padapter)
		return _FAIL;

	pmlmeext = &padapter->mlmeextpriv;
	pmlmeinfo = &(pmlmeext->mlmext_info);

	if ((pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) && ((pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE))
		return _TRUE;
	else
		return _FAIL;
}

int is_client_associated_to_ibss(_adapter *padapter)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if ((pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) && ((pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE))
		return _TRUE;
	else
		return _FAIL;
}

/*GEORGIA_TODO_FIXIT*/
#define GET_H2CCMD_MSRRPT_PARM_OPMODE(__pH2CCmd) 	LE_BITS_TO_1BYTE(((u8 *)(__pH2CCmd)), 0, 1)
#define GET_H2CCMD_MSRRPT_PARM_ROLE(__pH2CCmd)	LE_BITS_TO_1BYTE(((u8 *)(__pH2CCmd)), 4, 4)

int is_IBSS_empty(_adapter *padapter)
{
/* ToDo */
#if 0
	int i;
	struct macid_ctl_t *macid_ctl = &padapter->dvobj->macid_ctl;

	for (i = 0; i < macid_ctl->num; i++) {
		if (!rtw_macid_is_used(macid_ctl, i))
			continue;
		if (!rtw_macid_is_iface_specific(macid_ctl, i, padapter))
			continue;
		if (!GET_H2CCMD_MSRRPT_PARM_OPMODE(&macid_ctl->h2c_msr[i]))
			continue;
		if (GET_H2CCMD_MSRRPT_PARM_ROLE(&macid_ctl->h2c_msr[i]) == H2C_MSR_ROLE_ADHOC)
			return _FAIL;
	}
#endif
	return _TRUE;
}

unsigned int decide_wait_for_beacon_timeout(unsigned int bcn_interval)
{
	if ((bcn_interval << 2) < WAIT_FOR_BCN_TO_MIN)
		return WAIT_FOR_BCN_TO_MIN;
	else if ((bcn_interval << 2) > WAIT_FOR_BCN_TO_MAX)
		return WAIT_FOR_BCN_TO_MAX;
	else
		return bcn_interval << 2;
}

#if defined(CONFIG_P2P) && defined(CONFIG_WFD)
void rtw_process_wfd_ie(_adapter *adapter, u8 *wfd_ie, u8 wfd_ielen, const char *tag)
{
	struct wifidirect_info *wdinfo = &adapter->wdinfo;

	u8 *attr_content;
	u32 attr_contentlen = 0;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_MIRACAST))
		return;

	RTW_INFO("[%s] Found WFD IE\n", tag);
	attr_content = rtw_get_wfd_attr_content(wfd_ie, wfd_ielen, WFD_ATTR_DEVICE_INFO, NULL, &attr_contentlen);
	if (attr_content && attr_contentlen) {
		wdinfo->wfd_info->peer_rtsp_ctrlport = RTW_GET_BE16(attr_content + 2);
		RTW_INFO("[%s] Peer PORT NUM = %d\n", tag, wdinfo->wfd_info->peer_rtsp_ctrlport);
	}
}

void rtw_process_wfd_ies(_adapter *adapter, u8 *ies, u8 ies_len, const char *tag)
{
	u8 *wfd_ie;
	u32	wfd_ielen;

	if (!rtw_hw_chk_wl_func(adapter_to_dvobj(adapter), WL_FUNC_MIRACAST))
		return;

	wfd_ie = rtw_get_wfd_ie(ies, ies_len, NULL, &wfd_ielen);
	while (wfd_ie) {
		rtw_process_wfd_ie(adapter, wfd_ie, wfd_ielen, tag);
		wfd_ie = rtw_get_wfd_ie(wfd_ie + wfd_ielen, (ies + ies_len) - (wfd_ie + wfd_ielen), NULL, &wfd_ielen);
	}
}
#endif /* defined(CONFIG_P2P) && defined(CONFIG_WFD) */

int WMM_param_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs	pIE)
{
	/* struct registry_priv	*pregpriv = &padapter->registrypriv; */
	struct link_mlme_priv	*pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (pmlmepriv->qospriv.qos_option == 0) {
		pmlmeinfo->WMM_enable = 0;
		return _FALSE;
	}

	if (_rtw_memcmp(&(pmlmeinfo->WMM_param), (pIE->data + 6), sizeof(struct WMM_para_element)))
		return _FALSE;
	else
		_rtw_memcpy(&(pmlmeinfo->WMM_param), (pIE->data + 6), sizeof(struct WMM_para_element));
	pmlmeinfo->WMM_enable = 1;
	return _TRUE;

#if 0
	if (pregpriv->wifi_spec == 1) {
		if (pmlmeinfo->WMM_enable == 1) {
			/* todo: compare the parameter set count & decide wheher to update or not */
			return _FAIL;
		} else {
			pmlmeinfo->WMM_enable = 1;
			_rtw_rtw_memcpy(&(pmlmeinfo->WMM_param), (pIE->data + 6), sizeof(struct WMM_para_element));
			return _TRUE;
		}
	} else {
		pmlmeinfo->WMM_enable = 0;
		return _FAIL;
	}
#endif

}

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
u8 rtw_is_tbtx_capabilty(u8 *p, u8 len){
	int i;
	u8 tbtx_cap_ie[8] = {0x00, 0xe0, 0x4c, 0x01, 0x00, 0x00, 0x00, 0x00};

	for (i = 0; i < len; i++) {
		if (*(p + i) != tbtx_cap_ie[i]) 
			return _FALSE;
		else
			continue;
	}
	return _TRUE;
}
#endif

void WMMOnAssocRsp(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	u8	ACI, ACM, AIFS, ECWMin, ECWMax, aSifsTime;
	u8	acm_mask;
	u16	TXOP;
	u32	acParm, i;
	u32	edca[4], inx[4];
	u8 ac_be = 0;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	acm_mask = 0;
	if (WIFI_ROLE_LINK_IS_ON_5G(padapter_link) ||
		(pmlmeext->cur_wireless_mode & WLAN_MD_11N))
		aSifsTime = 16;
	else
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
		rtw_hw_set_edca(padapter, padapter_link, 0, acParm);
		rtw_update_phl_sta_edca(padapter, padapter_link, RTW_AC_BE, acParm);
		rtw_hw_set_edca(padapter, padapter_link, 1, acParm);
		rtw_update_phl_sta_edca(padapter, padapter_link, RTW_AC_BK, acParm);
		rtw_hw_set_edca(padapter, padapter_link, 2, acParm);
		rtw_update_phl_sta_edca(padapter, padapter_link, RTW_AC_VI, acParm);

		ECWMin = 2;
		ECWMax = 3;
		TXOP = 0x2f;
		acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
		rtw_hw_set_edca(padapter, padapter_link, 3, acParm);
		rtw_update_phl_sta_edca(padapter, padapter_link, RTW_AC_VO, acParm);
	} else {
		edca[0] = edca[1] = edca[2] = edca[3] = 0;

		for (i = 0; i < 4; i++) {
			ACI = (pmlmeinfo->WMM_param.ac_param[i].ACI_AIFSN >> 5) & 0x03;
			ACM = (pmlmeinfo->WMM_param.ac_param[i].ACI_AIFSN >> 4) & 0x01;

			/* AIFS = AIFSN * slot time + SIFS - r2t phy delay */
			AIFS = (pmlmeinfo->WMM_param.ac_param[i].ACI_AIFSN & 0x0f) * pmlmeinfo->slotTime + aSifsTime;

			ECWMin = (pmlmeinfo->WMM_param.ac_param[i].CW & 0x0f);
			ECWMax = (pmlmeinfo->WMM_param.ac_param[i].CW & 0xf0) >> 4;
			TXOP = le16_to_cpu(pmlmeinfo->WMM_param.ac_param[i].TXOP_limit);

			acParm = AIFS | (ECWMin << 8) | (ECWMax << 12) | (TXOP << 16);
			rtw_hw_set_edca(padapter, padapter_link, ACI, acParm);
			rtw_update_phl_sta_edca(padapter, padapter_link, ACI, acParm);

			switch (ACI) {
			case 0x0:
				acm_mask |= (ACM ? BIT(1) : 0);
				edca[XMIT_BE_QUEUE] = acParm;
				break;

			case 0x1:
				/* acm_mask |= (ACM? BIT(0):0); */
				edca[XMIT_BK_QUEUE] = acParm;
				break;

			case 0x2:
				acm_mask |= (ACM ? BIT(2) : 0);
				edca[XMIT_VI_QUEUE] = acParm;
				break;

			case 0x3:
				acm_mask |= (ACM ? BIT(3) : 0);
				edca[XMIT_VO_QUEUE] = acParm;
				break;
			}

			RTW_INFO("WMM(%x): %x, %x\n", ACI, ACM, acParm);

			if (i == ac_be) {
				padapter->last_edca = acParm;
				acParm = rtw_get_turbo_edca(padapter, AIFS, ECWMin, ECWMax, TXOP);
				if (acParm) {
					rtw_hw_set_edca(padapter, padapter_link, ACI, acParm);
					padapter->last_edca = acParm;
				}
			}

		}

		if (padapter->registrypriv.acm_method == 1)
			rtw_hal_set_hwreg(padapter, HW_VAR_ACM_CTRL, (u8 *)(&acm_mask));
		else
			padapter->mlmepriv.acm_mask = acm_mask;

		inx[0] = 0;
		inx[1] = 1;
		inx[2] = 2;
		inx[3] = 3;

		if (pregpriv->wifi_spec == 1) {
			u32	j, tmp, change_inx = _FALSE;

			/* entry indx: 0->vo, 1->vi, 2->be, 3->bk. */
			for (i = 0; i < 4; i++) {
				for (j = i + 1; j < 4; j++) {
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

		for (i = 0; i < 4; i++) {
			pxmitpriv->wmm_para_seq[i] = inx[i];
			RTW_INFO("wmm_para_seq(%d): %d\n", i, pxmitpriv->wmm_para_seq[i]);
		}
	}
}

static void bwmode_update_check(_adapter *padapter,struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE)
{
#ifdef CONFIG_80211N_HT
	unsigned char	 new_bwmode;
	unsigned char  new_ch_offset;
	struct HT_info_element	*pHT_info;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct link_mlme_priv	*pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct ht_priv			*phtpriv = &pmlmepriv->htpriv;
	u8	cbw40_enable = 0;

	if (!pIE)
		return;

	if (phtpriv->ht_option == _FALSE)
		return;

	if (pmlmeext->chandef.bw >= CHANNEL_WIDTH_80)
		return;

	if (pIE->Length > sizeof(struct HT_info_element))
		return;

	pHT_info = (struct HT_info_element *)pIE->data;

	if (rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_40M)) {
		if (pmlmeext->chandef.chan > 14) {
			if (REGSTY_IS_BW_5G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_40))
				cbw40_enable = 1;
		} else {
			if (REGSTY_IS_BW_2G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_40))
				cbw40_enable = 1;
		}
	}

	if ((pHT_info->infos[0] & BIT(2)) && cbw40_enable) {
		new_bwmode = CHANNEL_WIDTH_40;

		switch (pHT_info->infos[0] & 0x3) {
		case 1:
			new_ch_offset = CHAN_OFFSET_UPPER;
			break;

		case 3:
			new_ch_offset = CHAN_OFFSET_LOWER;
			break;

		default:
			new_bwmode = CHANNEL_WIDTH_20;
			new_ch_offset = CHAN_OFFSET_NO_EXT;
			break;
		}
	} else {
		new_bwmode = CHANNEL_WIDTH_20;
		new_ch_offset = CHAN_OFFSET_NO_EXT;
	}


	if ((new_bwmode != pmlmeext->chandef.bw || new_ch_offset != pmlmeext->chandef.offset)
	    && new_bwmode < pmlmeext->chandef.bw
	   ) {
		pmlmeinfo->bwmode_updated = _TRUE;

		pmlmeext->chandef.bw = new_bwmode;
		pmlmeext->chandef.offset = new_ch_offset;

		/* update HT info also */
		HT_info_handler(padapter, padapter_link, pIE);
	} else
		pmlmeinfo->bwmode_updated = _FALSE;


	if (_TRUE == pmlmeinfo->bwmode_updated) {
		struct sta_info *psta;
		WLAN_BSSID_EX	*cur_network = &(pmlmeinfo->network);
		struct sta_priv	*pstapriv = &padapter->stapriv;

		/* set_channel_bwmode(padapter, pmlmeext->chandef.chan, pmlmeext->chandef.offset, pmlmeext->chandef.bw); */


		/* update ap's stainfo */
		psta = rtw_get_stainfo(pstapriv, cur_network->MacAddress);
		if (psta) {
			struct ht_priv	*phtpriv_sta = &psta->htpriv;

			if (phtpriv_sta->ht_option) {
				/* bwmode				 */
				psta->phl_sta->chandef.bw = pmlmeext->chandef.bw;
				phtpriv_sta->ch_offset = pmlmeext->chandef.offset;
			} else {
				psta->phl_sta->chandef.bw = CHANNEL_WIDTH_20;
				phtpriv_sta->ch_offset = CHAN_OFFSET_NO_EXT;
			}

			rtw_phl_cmd_change_stainfo(adapter_to_dvobj(padapter)->phl,
					   psta->phl_sta,
					   STA_CHG_RAMASK,
					   NULL,
					   0,
					   PHL_CMD_NO_WAIT,
					   0);
		}

		/* pmlmeinfo->bwmode_updated = _FALSE; */ /* bwmode_updated done, reset it! */
	}
#endif /* CONFIG_80211N_HT */
}

#ifdef PRIVATE_R
void Supported_rate_infra_ap(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE)
{
	unsigned int	i;
	struct link_mlme_ext_priv		*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info		*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (pIE == NULL)
		return;

	for (i = 0 ; i < pIE->Length; i++)
		pmlmeinfo->SupportedRates_infra_ap[i] = (pIE->data[i]);

}

void Extended_Supported_rate_infra_ap(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE)
{
	unsigned int i, j;
	struct link_mlme_ext_priv		*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info		*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (pIE == NULL)
		return;

	if (pIE->Length > 0) {
		for (i = 0; i < NDIS_802_11_LENGTH_RATES_EX; i++) {
			if (pmlmeinfo->SupportedRates_infra_ap[i] == 0)
				break;
		}
		for (j = 0; j < pIE->Length; j++)
			pmlmeinfo->SupportedRates_infra_ap[i+j] = (pIE->data[j]);
	}

}

void HT_get_ss_from_mcs_set(u8 *mcs_set, u8 *Rx_ss)
{
	u8 i, j;
	u8 r_ss = 0, t_ss = 0;

	for (i = 0; i < 4; i++) {
		if ((mcs_set[3-i] & 0xff) != 0x00) {
			r_ss = 4-i;
			break;
		}
	}

	*Rx_ss = r_ss;
}

void HT_caps_handler_infra_ap(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, PNDIS_802_11_VARIABLE_IEs pIE)
{
	unsigned int	i;
	u8	cur_stbc_cap_infra_ap = 0;
	struct link_mlme_priv		*pmlmepriv = &padapter_link->mlmepriv;
	struct ht_priv_infra_ap		*phtpriv = &pmlmepriv->htpriv_infra_ap;

	struct link_mlme_ext_priv		*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info		*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (pIE == NULL)
		return;

	pmlmeinfo->ht_vht_received |= BIT(0);

	/*copy MCS_SET*/
	for (i = 3; i < 19; i++)
		phtpriv->MCS_set_infra_ap[i-3] = (pIE->data[i]);

	/*get number of stream from mcs set*/
	HT_get_ss_from_mcs_set(phtpriv->MCS_set_infra_ap, &phtpriv->Rx_ss_infra_ap);

	phtpriv->rx_highest_data_rate_infra_ap = le16_to_cpu(GET_HT_CAP_ELE_RX_HIGHEST_DATA_RATE(pIE->data));

	phtpriv->ldpc_cap_infra_ap = GET_HT_CAP_ELE_LDPC_CAP(pIE->data);

	if (GET_HT_CAP_ELE_RX_STBC(pIE->data))
		SET_FLAG(cur_stbc_cap_infra_ap, STBC_HT_ENABLE_RX);
	if (GET_HT_CAP_ELE_TX_STBC(pIE->data))
		SET_FLAG(cur_stbc_cap_infra_ap, STBC_HT_ENABLE_TX);
	phtpriv->stbc_cap_infra_ap = cur_stbc_cap_infra_ap;

	/*store ap info SGI 20m 40m*/
	phtpriv->sgi_20m_infra_ap = GET_HT_CAP_ELE_SHORT_GI20M(pIE->data);
	phtpriv->sgi_40m_infra_ap = GET_HT_CAP_ELE_SHORT_GI40M(pIE->data);

	/*store ap info for supported channel bandwidth*/
	phtpriv->channel_width_infra_ap = GET_HT_CAP_ELE_CHL_WIDTH(pIE->data);
}
#endif /* PRIVATE_R */

void HT_caps_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE)
{
#ifdef CONFIG_80211N_HT
	unsigned int i;
	u8 max_AMPDU_len, min_MPDU_spacing;
	u8 cur_ldpc_cap = 0, cur_stbc_cap = 0, cur_beamform_cap = 0, rx_nss = 0;
	struct rtw_wifi_role_t *wrole = padapter->phl_role;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_priv *pmlmepriv = &padapter_link->mlmepriv;
	struct protocol_cap_t *protocol_cap = &padapter_link->wrlink->protocol_cap;
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;
	struct sta_info *sta = NULL;
	struct rtw_phl_stainfo_t *phl_sta = NULL;
#ifdef CONFIG_DISABLE_MCS13TO15
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
#endif

	if (pIE == NULL)
		return;

	if (phtpriv->ht_option == _FALSE)
		return;

	pmlmeinfo->HT_caps_enable = 1;

	for (i = 0; i < (pIE->Length); i++) {
		if (i != 2) {
			/*	Commented by Albert 2010/07/12 */
			/*	Got the endian issue here. */
			pmlmeinfo->HT_caps.u.HT_cap[i] &= (pIE->data[i]);
		} else {
			/* AMPDU Parameters field */

			/* Get MIN of MAX AMPDU Length Exp */
			if ((pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x3) > (pIE->data[i] & 0x3))
				max_AMPDU_len = (pIE->data[i] & 0x3);
			else
				max_AMPDU_len = (pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x3);

			/* Get MAX of MIN MPDU Start Spacing */
			if ((pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x1c) > (pIE->data[i] & 0x1c))
				min_MPDU_spacing = (pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x1c);
			else
				min_MPDU_spacing = (pIE->data[i] & 0x1c);

			pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para = max_AMPDU_len | min_MPDU_spacing;
		}
	}

	/*	Commented by Albert 2010/07/12 */
	/*	Have to handle the endian issue after copying. */
	/*	HT_ext_caps didn't be used yet.	 */
	pmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info = le16_to_cpu(pmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info);
	pmlmeinfo->HT_caps.u.HT_cap_element.HT_ext_caps = le16_to_cpu(pmlmeinfo->HT_caps.u.HT_cap_element.HT_ext_caps);

	/* update the MCS set */
	for (i = 0; i < 16; i++)
		pmlmeinfo->HT_caps.u.HT_cap_element.MCS_rate[i] &= pmlmeext->default_supported_mcs_set[i];

	rx_nss = get_phy_rx_nss(padapter, padapter_link);

	switch (rx_nss) {
	case 1:
		set_mcs_rate_by_mask(pmlmeinfo->HT_caps.u.HT_cap_element.MCS_rate, MCS_RATE_1R);
		break;
	case 2:
		#ifdef CONFIG_DISABLE_MCS13TO15
		if (pmlmeext->chandef.bw == CHANNEL_WIDTH_40 && pregistrypriv->wifi_spec != 1)
			set_mcs_rate_by_mask(pmlmeinfo->HT_caps.u.HT_cap_element.MCS_rate, MCS_RATE_2R_13TO15_OFF);
		else
		#endif
			set_mcs_rate_by_mask(pmlmeinfo->HT_caps.u.HT_cap_element.MCS_rate, MCS_RATE_2R);
		break;
	case 3:
		set_mcs_rate_by_mask(pmlmeinfo->HT_caps.u.HT_cap_element.MCS_rate, MCS_RATE_3R);
		break;
	case 4:
		set_mcs_rate_by_mask(pmlmeinfo->HT_caps.u.HT_cap_element.MCS_rate, MCS_RATE_4R);
		break;
	default:
		RTW_WARN("rf_type:%d or tx_nss:%u is not expected\n", GET_HAL_RFPATH(adapter_to_dvobj(padapter)), rx_nss);
	}

	if (check_fwstate(&padapter->mlmepriv, WIFI_AP_STATE)) {
		/* Config STBC setting */
		if (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_TX) && GET_HT_CAP_ELE_RX_STBC(pIE->data)) {
			SET_FLAG(cur_stbc_cap, STBC_HT_ENABLE_TX);
			RTW_INFO("Enable HT Tx STBC !\n");
		}
		phtpriv->stbc_cap = cur_stbc_cap;

#ifdef CONFIG_BEAMFORMING
		/* Config Tx beamforming setting */
		if (TEST_FLAG(phtpriv->beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE) &&
		    GET_HT_CAP_TXBF_EXPLICIT_COMP_STEERING_CAP(pIE->data)) {
			SET_FLAG(cur_beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE);
			/* Shift to BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP*/
			SET_FLAG(cur_beamform_cap, GET_HT_CAP_TXBF_CHNL_ESTIMATION_NUM_ANTENNAS(pIE->data) << 6);
		}

		if (TEST_FLAG(phtpriv->beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE) &&
		    GET_HT_CAP_TXBF_EXPLICIT_COMP_FEEDBACK_CAP(pIE->data)) {
			SET_FLAG(cur_beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE);
			/* Shift to BEAMFORMING_HT_BEAMFORMER_STEER_NUM*/
			SET_FLAG(cur_beamform_cap, GET_HT_CAP_TXBF_COMP_STEERING_NUM_ANTENNAS(pIE->data) << 4);
		}
		phtpriv->beamform_cap = cur_beamform_cap;
		if (cur_beamform_cap)
			RTW_INFO("AP HT Beamforming Cap = 0x%02X\n", cur_beamform_cap);
#endif /*CONFIG_BEAMFORMING*/
	} else {
		/*WIFI_STATION_STATEorI_ADHOC_STATE or WIFI_ADHOC_MASTER_STATE*/
		sta = rtw_get_stainfo(&padapter->stapriv, pmlmeinfo->network.MacAddress);
		if (!sta) {
			RTW_ERR(FUNC_ADPT_FMT ": STA(" MAC_FMT ") not found!\n",
				FUNC_ADPT_ARG(padapter), MAC_ARG(pmlmeinfo->network.MacAddress));
			return;
		}
		if (!sta->phl_sta) {
			RTW_ERR(FUNC_ADPT_FMT ": PHL STA(" MAC_FMT ") not exist!\n",
				FUNC_ADPT_ARG(padapter), MAC_ARG(pmlmeinfo->network.MacAddress));
			return;
		}
		phl_sta = sta->phl_sta;

		/* Config LDPC Coding Capability */
		if (TEST_FLAG(phtpriv->ldpc_cap, LDPC_HT_ENABLE_TX) && GET_HT_CAP_ELE_LDPC_CAP(pIE->data)) {
			SET_FLAG(cur_ldpc_cap, (LDPC_HT_ENABLE_TX | LDPC_HT_CAP_TX));
			RTW_INFO("Enable HT Tx LDPC!\n");
			phl_sta->asoc_cap.ht_ldpc = 1;
		}
		phtpriv->ldpc_cap = cur_ldpc_cap;

		/* Config STBC setting */
		if (TEST_FLAG(phtpriv->stbc_cap, STBC_HT_ENABLE_TX) && GET_HT_CAP_ELE_RX_STBC(pIE->data)) {
			SET_FLAG(cur_stbc_cap, (STBC_HT_ENABLE_TX | STBC_HT_CAP_TX));
			RTW_INFO("Enable HT Tx STBC!\n");
			phl_sta->asoc_cap.stbc_ht_rx =
				protocol_cap->stbc_ht_tx ? GET_HT_CAP_ELE_RX_STBC(pIE->data) : 0;
		}
		phtpriv->stbc_cap = cur_stbc_cap;
		phl_sta->asoc_cap.stbc_ht_tx = GET_HT_CAP_ELE_TX_STBC(pIE->data);

#ifdef CONFIG_BEAMFORMING
		/* Config beamforming setting */
		if (TEST_FLAG(phtpriv->beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE) &&
		    GET_HT_CAP_TXBF_EXPLICIT_COMP_STEERING_CAP(pIE->data)) {
			SET_FLAG(cur_beamform_cap, BEAMFORMING_HT_BEAMFORMEE_ENABLE);
			/* Shift to BEAMFORMING_HT_BEAMFORMEE_CHNL_EST_CAP*/
			SET_FLAG(cur_beamform_cap, GET_HT_CAP_TXBF_CHNL_ESTIMATION_NUM_ANTENNAS(pIE->data) << 6);
		}

		if (TEST_FLAG(phtpriv->beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE) &&
		    GET_HT_CAP_TXBF_EXPLICIT_COMP_FEEDBACK_CAP(pIE->data)) {
			SET_FLAG(cur_beamform_cap, BEAMFORMING_HT_BEAMFORMER_ENABLE);
			/* Shift to BEAMFORMING_HT_BEAMFORMER_STEER_NUM*/
			SET_FLAG(cur_beamform_cap, GET_HT_CAP_TXBF_COMP_STEERING_NUM_ANTENNAS(pIE->data) << 4);
		}
		phtpriv->beamform_cap = cur_beamform_cap;
		if (cur_beamform_cap)
			RTW_INFO("Client HT Beamforming Cap = 0x%02X\n", cur_beamform_cap);
#endif /*CONFIG_BEAMFORMING*/
	}

#endif /* CONFIG_80211N_HT */
}

void HT_info_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE)
{
#ifdef CONFIG_80211N_HT
	struct link_mlme_priv		*pmlmepriv = &padapter_link->mlmepriv;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	struct ht_priv			*phtpriv = &pmlmepriv->htpriv;

	if (pIE == NULL)
		return;

	if (phtpriv->ht_option == _FALSE)
		return;


	if (pIE->Length > sizeof(struct HT_info_element))
		return;

	pmlmeinfo->HT_info_enable = 1;
	_rtw_memcpy(&(pmlmeinfo->HT_info), pIE->data, pIE->Length);
#endif /* CONFIG_80211N_HT */
	return;
}

void HTOnAssocRsp(_adapter *padapter)
{
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	unsigned char		max_AMPDU_len;
	unsigned char		min_MPDU_spacing;
	/* struct registry_priv	 *pregpriv = &padapter->registrypriv; */
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	RTW_INFO("%s\n", __FUNCTION__);

	if ((pmlmeinfo->HT_info_enable) && (pmlmeinfo->HT_caps_enable))
		pmlmeinfo->HT_enable = 1;
	else {
		pmlmeinfo->HT_enable = 0;
		/* set_channel_bwmode(padapter, pmlmeext->cur_channel, pmlmeext->cur_ch_offset, pmlmeext->cur_bwmode); */
		return;
	}

	/* handle A-MPDU parameter field */
	/*
		AMPDU_para [1:0]:Max AMPDU Len => 0:8k , 1:16k, 2:32k, 3:64k
		AMPDU_para [4:2]:Min MPDU Start Spacing
	*/
	max_AMPDU_len = pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x03;

	min_MPDU_spacing = (pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x1c) >> 2;

	rtw_hal_set_hwreg(padapter, HW_VAR_AMPDU_MIN_SPACE, (u8 *)(&min_MPDU_spacing));
#ifdef CONFIG_80211N_HT
	rtw_hal_set_hwreg(padapter, HW_VAR_AMPDU_FACTOR, (u8 *)(&max_AMPDU_len));
#endif /* CONFIG_80211N_HT */
#if 0 /* move to rtw_update_ht_cap() */
	if ((pregpriv->bw_mode > 0) &&
	    (pmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info & BIT(1)) &&
	    (pmlmeinfo->HT_info.infos[0] & BIT(2))) {
		/* switch to the 40M Hz mode accoring to the AP */
		pmlmeext->cur_bwmode = CHANNEL_WIDTH_40;
		switch ((pmlmeinfo->HT_info.infos[0] & 0x3)) {
		case IEEE80211_SCA:
			pmlmeext->cur_ch_offset = CHAN_OFFSET_UPPER;
			break;

		case IEEE80211_SCB:
			pmlmeext->cur_ch_offset = CHAN_OFFSET_LOWER;
			break;

		default:
			pmlmeext->cur_ch_offset = CHAN_OFFSET_NO_EXT;
			break;
		}
	}
#endif

	/* set_channel_bwmode(padapter, pmlmeext->cur_channel, pmlmeext->cur_ch_offset, pmlmeext->cur_bwmode); */

#if 0 /* move to rtw_update_ht_cap() */
	/*  */
	/* Config SM Power Save setting */
	/*  */
	pmlmeinfo->SM_PS = (pmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info & 0x0C) >> 2;
	if (pmlmeinfo->SM_PS == SM_PS_STATIC) {
#if 0
		u8 i;
		/* update the MCS rates */
		for (i = 0; i < 16; i++)
			pmlmeinfo->HT_caps.HT_cap_element.MCS_rate[i] &= MCS_rate_1R[i];
#endif
		RTW_INFO("%s(): SM_PS_STATIC\n", __FUNCTION__);
	}

	/*  */
	/* Config current HT Protection mode. */
	/*  */
	pmlmeinfo->HT_protection = pmlmeinfo->HT_info.infos[1] & 0x3;
#endif

}

void ERP_IE_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		PNDIS_802_11_VARIABLE_IEs pIE)
{
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (pIE->Length > 1)
		return;

	pmlmeinfo->ERP_enable = 1;
	_rtw_memcpy(&(pmlmeinfo->ERP_IE), pIE->data, pIE->Length);
}

void VCS_update(_adapter *padapter, struct sta_info *psta)
{
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	switch (pregpriv->vrtl_carrier_sense) { /* 0:off 1:on 2:auto */
	case 0: /* off */
		psta->rtsen = 0;
		psta->cts2self = 0;
		break;

	case 1: /* on */
		if (pregpriv->vcs_type == 1) { /* 1:RTS/CTS 2:CTS to self */
			psta->rtsen = 1;
			psta->cts2self = 0;
		} else {
			psta->rtsen = 0;
			psta->cts2self = 1;
		}
		break;

	case 2: /* auto */
	default:
		if (((pmlmeinfo->ERP_enable) && (pmlmeinfo->ERP_IE & BIT(1)))
			/*||(pmlmepriv->ht_op_mode & HT_INFO_OPERATION_MODE_NON_GF_DEVS_PRESENT)*/
		) {
			if (pregpriv->vcs_type == 1) {
				psta->rtsen = 1;
				psta->cts2self = 0;
			} else {
				psta->rtsen = 0;
				psta->cts2self = 1;
			}
		} else {
			psta->rtsen = 0;
			psta->cts2self = 0;
		}
		break;
	}

	switch (pregpriv->hw_rts_en) { /* 0:disable 1:enable */
	case 0:
		psta->hw_rts_en = 0;
		break;
	case 1:
		psta->hw_rts_en = 1;
		break;
	default:
		RTW_WARN("%s: unexpected value(%d) for hw_rts_en.\n", __func__, pregpriv->hw_rts_en);
		break;
	}

}

void	update_ldpc_stbc_cap(struct sta_info *psta)
{
#ifdef CONFIG_80211N_HT

#ifdef CONFIG_80211AC_VHT
#ifdef CONFIG_80211AX_HE
	/* CONFIG_80211AX_HE_TODO */
#endif /* CONFIG_80211AX_HE */
	if (psta->vhtpriv.vht_option) {
		if (TEST_FLAG(psta->vhtpriv.ldpc_cap, LDPC_VHT_ENABLE_TX))
			psta->phl_sta->asoc_cap.vht_ldpc = 1;
		else
			psta->phl_sta->asoc_cap.vht_ldpc = 0;
	} else
#endif /* CONFIG_80211AC_VHT */
		if (psta->htpriv.ht_option) {
			if (TEST_FLAG(psta->htpriv.ldpc_cap, LDPC_HT_ENABLE_TX))
				psta->phl_sta->asoc_cap.ht_ldpc = 1;
			else
				psta->phl_sta->asoc_cap.ht_ldpc = 0;
		} else {
			psta->phl_sta->asoc_cap.vht_ldpc = 0;
			psta->phl_sta->asoc_cap.ht_ldpc = 0;
		}

#endif /* CONFIG_80211N_HT */
}

int check_ielen(u8 *start, uint len)
{
	int left = len;
	u8 *pos = start;
	u8 id, elen;

	while (left >= 2) {
		id = *pos++;
		elen = *pos++;
		left -= 2;

		if (elen > left) {
			RTW_ERR("IEEE 802.11 element parse failed (id=%d elen=%d left=%lu)\n",
					id, elen, (unsigned long) left);
			return _FALSE;
		}
		if ((id == WLAN_EID_VENDOR_SPECIFIC) && (elen < 3))
				return _FALSE;

		left -= elen;
		pos += elen;
	}
	if (left)
		return _FALSE;

	return _TRUE;
}

int validate_beacon_len(u8 *pframe, u32 len)
{
	u8 ie_offset = _BEACON_IE_OFFSET_ + sizeof(struct rtw_ieee80211_hdr_3addr);

	if (len < ie_offset) {
		RTW_ERR("%s: incorrect beacon length(%d)\n", __func__, len);
		rtw_warn_on(1);
		return _FALSE;
	}

	if (check_ielen(pframe + ie_offset, len - ie_offset) == _FALSE)
		return _FALSE;

	return _TRUE;
}


u8 support_rate_ranges[] = {
	IEEE80211_CCK_RATE_1MB,
	IEEE80211_CCK_RATE_2MB,
	IEEE80211_CCK_RATE_5MB,
	IEEE80211_CCK_RATE_11MB,
	IEEE80211_OFDM_RATE_6MB,
	IEEE80211_OFDM_RATE_9MB,
	IEEE80211_OFDM_RATE_12MB,
	IEEE80211_OFDM_RATE_18MB,
	IEEE80211_PBCC_RATE_22MB,
	IEEE80211_FREAK_RATE_22_5MB,
	IEEE80211_OFDM_RATE_24MB,
	IEEE80211_OFDM_RATE_36MB,
	IEEE80211_OFDM_RATE_48MB,
	IEEE80211_OFDM_RATE_54MB,
};

inline bool match_ranges(u16 EID, u32 value)
{
	int i;
	int nr_range;

	switch (EID) {
	case _EXT_SUPPORTEDRATES_IE_:
	case _SUPPORTEDRATES_IE_:
		nr_range = sizeof(support_rate_ranges)/sizeof(u8);
		for (i = 0; i < nr_range; i++) {
			/*	clear bit7 before searching.	*/
			value &= ~BIT(7);
			if (value == support_rate_ranges[i])
				return _TRUE;
		}
		break;
	default:
		break;
	};
	return _FALSE;
}

/*
 * rtw_validate_value: validate the IE contain.
 *
 *	Input : 
 *		EID : Element ID
 *		p	: IE buffer (without EID & length)
 *		len	: IE length
 *	return: 
 * 		_TRUE	: All Values are validated.
 *		_FALSE	: At least one value is NOT validated.
 */
bool rtw_validate_value(u16 EID, u8 *p, u16 len)
{
	u8 rate;
	u32 i, nr_val;

	switch (EID) {
	case _EXT_SUPPORTEDRATES_IE_:
	case _SUPPORTEDRATES_IE_:
		nr_val = len;
		for (i=0; i<nr_val; i++) {
			rate = *(p+i);
			/*
			 * Skip HT, VHT, HE and SAE H2E only BSS membership selectors
			 * since they're not rates.
			 */
			if (rate == (0x80 | BSS_MEMBERSHIP_SELECTOR_HT_PHY) ||
			    rate == (0x80 | BSS_MEMBERSHIP_SELECTOR_VHT_PHY) ||
			    rate == (0x80 | BSS_MEMBERSHIP_SELECTOR_HE_PHY) ||
			    rate == (0x80 | BSS_MEMBERSHIP_SELECTOR_SAE_H2E))
				continue;
			if (match_ranges(EID, rate) == _FALSE)
				return _FALSE;
		}
		break;
	default:
		break;
	};
	return _TRUE;
}

bool is_hidden_ssid(char *ssid, int len)
{
	return len == 0 || is_all_null(ssid, len) == _TRUE;
}

inline bool hidden_ssid_ap(WLAN_BSSID_EX *snetwork)
{
	return ((snetwork->Ssid.SsidLength == 0) ||  
		is_all_null(snetwork->Ssid.Ssid, snetwork->Ssid.SsidLength) == _TRUE);
}

/*
	Get SSID if this ilegal frame(probe resp) comes from a hidden SSID AP.
	Update the SSID to the corresponding pnetwork in scan queue.
*/
void rtw_absorb_ssid_ifneed(_adapter *padapter, WLAN_BSSID_EX *bssid, u8 *pframe)
{
	struct wlan_network *scanned = NULL;
	WLAN_BSSID_EX	*snetwork;
	u8 ie_offset, *p=NULL, *next_ie=NULL;
	u8 *mac;
	sint ssid_len_ori;
	u32 remain_len = 0;
	u8 backupIE[MAX_IE_SZ];
	u16 subtype;

	mac = get_addr2_ptr(pframe);
	subtype = get_frame_sub_type(pframe);

	if (subtype == WIFI_BEACON) {
		bssid->Reserved[0] = BSS_TYPE_BCN;
		ie_offset = _BEACON_IE_OFFSET_;
	} else {
		/* FIXME : more type */
		if (subtype == WIFI_PROBERSP) {
			ie_offset = _PROBERSP_IE_OFFSET_;
			bssid->Reserved[0] = BSS_TYPE_PROB_RSP;
		} else if (subtype == WIFI_PROBEREQ) {
			ie_offset = _PROBEREQ_IE_OFFSET_;
			bssid->Reserved[0] = BSS_TYPE_PROB_REQ;
		} else {
			bssid->Reserved[0] = BSS_TYPE_UNDEF;
			ie_offset = _FIXED_IE_LENGTH_;
		}
	}
	
	_rtw_spinlock_bh(&padapter->mlmepriv.scanned_queue.lock);
	scanned = _rtw_find_network(&padapter->mlmepriv.scanned_queue, mac);
	if (!scanned) {
		_rtw_spinunlock_bh(&padapter->mlmepriv.scanned_queue.lock);
		return;
	}

	snetwork = &(scanned->network);
	/* scan queue records as Hidden SSID && Input frame is NOT Hidden SSID	*/
	if (hidden_ssid_ap(snetwork) && !hidden_ssid_ap(bssid)) {
		p = rtw_get_ie(snetwork->IEs+ie_offset, _SSID_IE_, &ssid_len_ori, snetwork->IELength-ie_offset);
		if (!p) {
			_rtw_spinunlock_bh(&padapter->mlmepriv.scanned_queue.lock);
			return;
		}
		next_ie = p + 2 + ssid_len_ori;
		remain_len = snetwork->IELength - (next_ie - snetwork->IEs);
		scanned->network.Ssid.SsidLength = bssid->Ssid.SsidLength;
		_rtw_memcpy(scanned->network.Ssid.Ssid, bssid->Ssid.Ssid, bssid->Ssid.SsidLength);

		//update pnetwork->ssid, pnetwork->ssidlen
		_rtw_memcpy(backupIE, next_ie, remain_len);
		*(p+1) = bssid->Ssid.SsidLength;
		_rtw_memcpy(p+2, bssid->Ssid.Ssid, bssid->Ssid.SsidLength);
		_rtw_memcpy(p+2+bssid->Ssid.SsidLength, backupIE, remain_len);
		snetwork->IELength += bssid->Ssid.SsidLength;
	}
	_rtw_spinunlock_bh(&padapter->mlmepriv.scanned_queue.lock);
}

#ifdef DBG_RX_BCN
void rtw_debug_rx_bcn(_adapter *adapter, u8 *pframe, u32 packet_len)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	struct link_mlme_ext_priv *pmlmeext = &adapter_link->mlmeextpriv;
	struct link_mlme_ext_info *mlmeinfo = &(pmlmeext->mlmext_info);
	u16 sn = ((struct rtw_ieee80211_hdr_3addr *)pframe)->seq_ctl >> 4;
	u64 tsf, tsf_offset;
	u8 dtim_cnt, dtim_period, tim_bmap, tim_pvbit;

	update_TSF(pmlmeext, pframe, packet_len);
	tsf = pmlmeext->TSFValue;
	tsf_offset = rtw_modular64(pmlmeext->TSFValue, (mlmeinfo->bcn_interval * 1024));

	/*get TIM IE*/
	/*DTIM Count*/
	dtim_cnt = pmlmeext->tim[0];
	/*DTIM Period*/
	dtim_period = pmlmeext->tim[1];
	/*Bitmap*/
	tim_bmap = pmlmeext->tim[2];
	/*Partial VBitmap AID 0 ~ 7*/
	tim_pvbit = pmlmeext->tim[3];

	RTW_INFO("[BCN] SN-%d, TSF-%lld(us), offset-%lld, bcn_interval-%d DTIM-%d[%d] bitmap-0x%02x-0x%02x\n",
		sn, tsf, tsf_offset, mlmeinfo->bcn_interval, dtim_period, dtim_cnt, tim_bmap, tim_pvbit);
}
#endif

/*
 * rtw_get_bcn_keys: get beacon keys from recv frame
 *
 * TODO:
 *	WLAN_EID_COUNTRY
 *	WLAN_EID_ERP_INFO
 *	WLAN_EID_CHANNEL_SWITCH
 *	WLAN_EID_PWR_CONSTRAINT
 */
int _rtw_get_bcn_keys(u8 *cap_info, u32 buf_len, u8 def_ch
	, struct _ADAPTER_LINK *adapter_link
	, struct beacon_keys *recv_beacon)
{
	int left;
	u16 capability;
	unsigned char *pos;
	struct rtw_ieee802_11_elems elems;

	_rtw_memset(recv_beacon, 0, sizeof(*recv_beacon));

	/* checking capabilities */
	capability = le16_to_cpu(*(unsigned short *)(cap_info));

	/* checking IEs */
	left = buf_len - 2;
	pos = cap_info + 2;
	if (rtw_ieee802_11_parse_elems(pos, left, &elems, 1) == ParseFailed)
		return _FALSE;

	if (elems.ht_capabilities) {
		if (elems.ht_capabilities_len != 26)
			return _FALSE;
	}

	if (elems.ht_operation) {
		if (elems.ht_operation_len != 22)
			return _FALSE;
	}

	if (elems.vht_capabilities) {
		if (elems.vht_capabilities_len != 12)
			return _FALSE;
	}

	if (elems.vht_operation) {
		if (elems.vht_operation_len != 5)
			return _FALSE;
	}

	if (rtw_ies_get_supported_rate(pos, left, recv_beacon->rate_set, &recv_beacon->rate_num) == _FAIL)
		return _FALSE;

	if (cckratesonly_included(recv_beacon->rate_set, recv_beacon->rate_num) == _TRUE)
		recv_beacon->proto_cap |= PROTO_CAP_11B;
	else if (cckrates_included(recv_beacon->rate_set, recv_beacon->rate_num) == _TRUE)
		recv_beacon->proto_cap |= PROTO_CAP_11B | PROTO_CAP_11G;
	else
		recv_beacon->proto_cap |= PROTO_CAP_11G;

	if (elems.ht_capabilities && elems.ht_operation)
		recv_beacon->proto_cap |= PROTO_CAP_11N;

	if (elems.vht_capabilities && elems.vht_operation)
		recv_beacon->proto_cap |= PROTO_CAP_11AC;

	if (elems.he_capabilities && elems.he_operation)
		recv_beacon->proto_cap |= PROTO_CAP_11AX;

	/* check bw and channel offset */
	rtw_ies_get_chbw(pos, left, &recv_beacon->ch, &recv_beacon->bw, &recv_beacon->offset, 1, 1);
	if (!recv_beacon->ch)
		recv_beacon->ch = def_ch;

	/* checking SSID */
	if (elems.ssid) {
		if (elems.ssid_len > sizeof(recv_beacon->ssid))
			return _FALSE;

		_rtw_memcpy(recv_beacon->ssid, elems.ssid, elems.ssid_len);
		recv_beacon->ssid_len = elems.ssid_len;
	}

	/* checking RSN first */
	if (elems.rsn_ie && elems.rsn_ie_len) {
		recv_beacon->encryp_protocol = ENCRYP_PROTOCOL_RSN;
		rtw_parse_wpa2_ie(elems.rsn_ie - 2, elems.rsn_ie_len + 2,
			&recv_beacon->group_cipher, &recv_beacon->pairwise_cipher,
			NULL, &recv_beacon->akm, NULL, NULL);
	}
	/* checking WPA secon */
	else if (elems.wpa_ie && elems.wpa_ie_len) {
		recv_beacon->encryp_protocol = ENCRYP_PROTOCOL_WPA;
		rtw_parse_wpa_ie(elems.wpa_ie - 2, elems.wpa_ie_len + 2,
			&recv_beacon->group_cipher, &recv_beacon->pairwise_cipher,
				 &recv_beacon->akm);
	} else if (capability & BIT(4))
		recv_beacon->encryp_protocol = ENCRYP_PROTOCOL_WEP;

	if (adapter_link) {
		struct link_mlme_ext_priv *pmlmeext = &adapter_link->mlmeextpriv;

		if (elems.tim && elems.tim_len) {
			#ifdef DBG_RX_BCN
			_rtw_memcpy(pmlmeext->tim, elems.tim, 4);
			#endif
			pmlmeext->dtim = elems.tim[1];
		}

		/* checking RTW TBTX */
		#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
		if (elems.tbtx_cap && elems.tbtx_cap_len) {
			struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

			if (rtw_is_tbtx_capabilty(elems.tbtx_cap, elems.tbtx_cap_len))
				RTW_DBG("AP support TBTX\n");
		}
		#endif
	}

	return _TRUE;
}

int rtw_get_bcn_keys(_adapter *adapter, struct _ADAPTER_LINK *adapter_link
	, u8 *whdr, u32 flen, struct beacon_keys *bcn_keys)
{
	return _rtw_get_bcn_keys(
		whdr + WLAN_HDR_A3_LEN + 10
		, flen - WLAN_HDR_A3_LEN - 10
		, adapter_link->mlmeextpriv.chandef.chan, adapter_link
		, bcn_keys);
}

int rtw_get_bcn_keys_from_bss(WLAN_BSSID_EX *bss, struct beacon_keys *bcn_keys)
{
	return _rtw_get_bcn_keys(
		bss->IEs + 10
		, bss->IELength - 10
		, bss->Configuration.DSConfig, NULL
		, bcn_keys);
}

int rtw_update_bcn_keys_of_network(struct wlan_network *network)
{
	network->bcn_keys_valid = rtw_get_bcn_keys_from_bss(&network->network, &network->bcn_keys);
	return network->bcn_keys_valid;
}

#define CIPHER_STR(c, type)	c & WPA_CIPHER_##type ? "["#type"]" : ""
#define CIPHER_FMT		"%s%s%s%s%s%s%s%s%s%s%s%s"
#define CIPHER_ARG(c)		CIPHER_STR(c, NONE), \
				CIPHER_STR(c, WEP40), \
				CIPHER_STR(c, WEP104), \
				CIPHER_STR(c, TKIP), \
				CIPHER_STR(c, CCMP), \
				CIPHER_STR(c, GCMP), \
				CIPHER_STR(c, GCMP_256), \
				CIPHER_STR(c, CCMP_256), \
				CIPHER_STR(c, BIP_CMAC_128), \
				CIPHER_STR(c, BIP_GMAC_128), \
				CIPHER_STR(c, BIP_GMAC_256), \
				CIPHER_STR(c, BIP_CMAC_256)
#define AKM_STR(akm, type)	akm & WLAN_AKM_TYPE_##type ? "["#type"]" : ""

void rtw_dump_bcn_keys(void *sel, struct beacon_keys *recv_beacon)
{
	u8 ssid[IW_ESSID_MAX_SIZE + 1];

	_rtw_memcpy(ssid, recv_beacon->ssid, recv_beacon->ssid_len);
	ssid[recv_beacon->ssid_len] = '\0';

	RTW_PRINT_SEL(sel, "ssid = %s (len = %u)\n", ssid, recv_beacon->ssid_len);
	RTW_PRINT_SEL(sel, "ch = %u,%u,%u\n"
		, recv_beacon->ch, recv_beacon->bw, recv_beacon->offset);
	RTW_PRINT_SEL(sel, "proto_cap = 0x%02x\n", recv_beacon->proto_cap);
	RTW_MAP_DUMP_SEL(sel, "rate_set = "
		, recv_beacon->rate_set, recv_beacon->rate_num);
	RTW_PRINT_SEL(sel, "sec = %d\n", recv_beacon->encryp_protocol);
	RTW_PRINT_SEL(sel, "GTK = 0x%x " CIPHER_FMT "\n"
		, recv_beacon->group_cipher
		, CIPHER_ARG(recv_beacon->group_cipher));
	RTW_PRINT_SEL(sel, "PTK = 0x%x " CIPHER_FMT "\n"
		, recv_beacon->pairwise_cipher
		, CIPHER_ARG(recv_beacon->pairwise_cipher));
	RTW_PRINT_SEL(sel, "AKM = 0x%08x %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n"
		, recv_beacon->akm
		, AKM_STR(recv_beacon->akm, 8021X)
		, AKM_STR(recv_beacon->akm, PSK)
		, AKM_STR(recv_beacon->akm, FT_8021X)
		, AKM_STR(recv_beacon->akm, FT_PSK)
		, AKM_STR(recv_beacon->akm, 8021X_SHA256)
		, AKM_STR(recv_beacon->akm, PSK_SHA256)
		, AKM_STR(recv_beacon->akm, TDLS)
		, AKM_STR(recv_beacon->akm, SAE)
		, AKM_STR(recv_beacon->akm, FT_OVER_SAE)
		, AKM_STR(recv_beacon->akm, 8021X_SUITE_B)
		, AKM_STR(recv_beacon->akm, 8021X_SUITE_B_192)
		, AKM_STR(recv_beacon->akm, FILS_SHA256)
		, AKM_STR(recv_beacon->akm, FILS_SHA384)
		, AKM_STR(recv_beacon->akm, FT_FILS_SHA256)
		, AKM_STR(recv_beacon->akm, FT_FILS_SHA384));
}

bool rtw_bcn_key_compare(struct beacon_keys *cur, struct beacon_keys *recv)
{
#define BCNKEY_VERIFY_PROTO_CAP 0
#define BCNKEY_VERIFY_WHOLE_RATE_SET 0

	struct beacon_keys tmp;
	bool ret = _FALSE;

	if (!rtw_is_chbw_grouped(cur->ch, cur->bw, cur->offset
			, recv->ch, recv->bw, recv->offset))
		goto exit;

	_rtw_memcpy(&tmp, cur, sizeof(tmp));

	/* check fields excluding below */
	tmp.ch = recv->ch;
	tmp.bw = recv->bw;
	tmp.offset = recv->offset;
	if (!BCNKEY_VERIFY_PROTO_CAP)
		tmp.proto_cap = recv->proto_cap;
	if (!BCNKEY_VERIFY_WHOLE_RATE_SET) {
		tmp.rate_num = recv->rate_num;
		_rtw_memcpy(tmp.rate_set, recv->rate_set, 12);
	}

	if (_rtw_memcmp(&tmp, recv, sizeof(*recv)) == _FALSE)
		goto exit;

	ret = _TRUE;

exit:
	return ret;
}

int rtw_check_bcn_info(_adapter *adapter, struct _ADAPTER_LINK *adapter_link,
		u8 *pframe, u32 packet_len)
{
	u8 *pbssid = GetAddr3Ptr(pframe);
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct beacon_keys *cur_beacon = &adapter_link->mlmepriv.cur_beacon_keys;
	struct beacon_keys recv_beacon;
	int ret = 0;
	u8 ifbmp_m = rtw_mi_get_ap_mesh_ifbmp(adapter);
	u8 ifbmp_s = rtw_mi_get_ld_sta_ifbmp(adapter);
	struct dvobj_priv *d = adapter_to_dvobj(adapter);
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	u8 ssid_is_hidden = _FALSE;
	struct link_mlme_ext_priv *plmlmeext = &(adapter_link->mlmeextpriv);
	struct wlan_network *cur_network = &(adapter_link->mlmepriv.cur_network);

	if (is_client_associated_to_ap(adapter) == _FALSE)
		goto exit_success;

	if (rtw_get_bcn_keys(adapter, adapter_link, pframe, packet_len, &recv_beacon) == _FALSE)
		goto exit_success; /* parsing failed => broken IE */

#ifdef DBG_RX_BCN
	rtw_debug_rx_bcn(adapter, pframe, packet_len);
#endif
	ssid_is_hidden = is_hidden_ssid(recv_beacon.ssid, recv_beacon.ssid_len);

	if (recv_beacon.ssid_len != cur_beacon->ssid_len) {
		pmlmeinfo->illegal_beacon_code |= SSID_LENGTH_CHANGED;
		pmlmeinfo->illegal_beacon_code |= SSID_CHANGED;
		if (!ssid_is_hidden) {
			RTW_WARN("%s: Ignore ssid len change new %d old %d\n",
				 __func__, recv_beacon.ssid_len,
				 cur_beacon->ssid_len);
		}
	} else if ((_rtw_memcmp(recv_beacon.ssid,
				cur_beacon->ssid,
				cur_beacon->ssid_len) == _FALSE)) {
		pmlmeinfo->illegal_beacon_code |= SSID_CHANGED;
		if (!ssid_is_hidden) {
			RTW_INFO_DUMP("[old ssid]: ",
				      cur_beacon->ssid, cur_beacon->ssid_len);
			RTW_INFO_DUMP("[new ssid]: ",
				      recv_beacon.ssid, cur_beacon->ssid_len);
		}
	}

	/* hidden ssid, replace with current beacon ssid directly */
	if (ssid_is_hidden) {
		_rtw_memcpy(recv_beacon.ssid, cur_beacon->ssid, cur_beacon->ssid_len);
		recv_beacon.ssid_len = cur_beacon->ssid_len;
	}

#ifdef CONFIG_ECSA_PHL
	if (check_fwstate(pmlmepriv, WIFI_CSA_UPDATE_BEACON)) {
		u8 u_ch, u_offset, u_bw;
		u8 bcn_ch, bcn_bw, bcn_offset;
		struct sta_info *psta = NULL;
		struct rtw_chan_def mr_chdef = {0};
		struct rtw_chan_def new_chdef = {0};

		/* get union ch/bw/offset from chan_ctx */
		rtw_phl_mr_get_chandef(d->phl, adapter->phl_role, adapter_link->wrlink, &mr_chdef);
		u_ch = mr_chdef.chan;
		u_offset = (u8)mr_chdef.offset;
		u_bw = (u8)mr_chdef.bw;

		#ifdef DBG_CSA
		RTW_INFO("CSA : Wait AP for updating its beacon, wait %u beacon, union.ch = %u, recv_beacon.ch = %u\n",
			pmlmepriv->bcn_cnts_after_csa + 1, u_ch, recv_beacon.ch);
		#endif

		/* always wait for AP to update its beacon WAIT_BCN_TIMES beacon */
		if (pmlmepriv->bcn_cnts_after_csa < WAIT_BCN_TIMES) {
			if (u_ch == recv_beacon.ch)
				pmlmepriv->bcn_cnts_after_csa += 1;
			goto exit_success;
		} else
			pmlmepriv->bcn_cnts_after_csa = 0;

		_rtw_memcpy(cur_beacon, &recv_beacon, sizeof(recv_beacon));
		clr_fwstate(pmlmepriv, WIFI_CSA_UPDATE_BEACON);

		bcn_ch = recv_beacon.ch;
		bcn_bw = recv_beacon.bw;
		bcn_offset = recv_beacon.offset;

		#ifdef DBG_CSA
		RTW_INFO("CSA : copy new beacon, recv_beacon.ch = %u, recv_beacon.bw = %u, recv_beacon.offset = %u\n",
			bcn_ch, bcn_bw, bcn_offset);
		rtw_dump_bcn_keys(RTW_DBGDUMP, &recv_beacon);
		#endif

		_cancel_timer_nowait(&adapter->mlmeextpriv.csa_wait_bcn_timer);

		/* beacon bw/offset is different from CSA IE */
		if ((bcn_bw > u_bw) ||
			(bcn_offset != u_offset &&
			u_offset != CHAN_OFFSET_NO_EXT &&
			bcn_offset != CHAN_OFFSET_NO_EXT)) {

			plmlmeext->chandef.bw = bcn_bw;
			plmlmeext->chandef.offset = bcn_offset;
			/* updaet STA mode DSConfig , ap mode will update in rtw_change_bss_bchbw_cmd */
			cur_network->network.Configuration.DSConfig = bcn_ch;

			/* update wifi role chandef */
			rtw_hw_update_chan_def(adapter, adapter_link);
			/* update chanctx */
			if (rtw_phl_mr_upt_chandef(d->phl, adapter_link->wrlink) == RTW_PHL_STATUS_FAILURE)
				RTW_ERR("CSA : update chanctx fail\n");

			rtw_phl_mr_get_chandef(d->phl, adapter->phl_role, adapter_link->wrlink, &new_chdef);

			#ifdef CONFIG_AP_MODE
			if (ifbmp_m) {
				rtw_change_bss_bchbw_cmd(dvobj_get_primary_adapter(d), 0
					, ifbmp_m, 0, new_chdef.band, new_chdef.chan, REQ_BW_ORI, REQ_OFFSET_NONE);
			} else
			#endif
			{
				#ifdef CONFIG_DFS_MASTER
				rtw_dfs_rd_en_dec_on_mlme_act(adapter, adapter_link, MLME_OPCH_SWITCH, ifbmp_s);
				#endif
				rtw_set_chbw_cmd(adapter, adapter_link, new_chdef.band,
						new_chdef.chan, (u8)new_chdef.bw, (u8)new_chdef.offset, 0);
			}
			RTW_INFO("CSA : after update bw/offset, new_bw=%d, new_offset=%d \n", \
				(u8)new_chdef.bw, (u8)new_chdef.offset);
		} else {
			RTW_INFO("CSA : Our bw/offset is same as AP\n");
		}

		/* update RA mask */
		psta = rtw_get_stainfo(&adapter->stapriv, get_link_bssid(&adapter_link->mlmepriv));
		if (psta) {
			psta->phl_sta->chandef.bw = plmlmeext->chandef.bw;
			rtw_phl_cmd_change_stainfo(GET_PHL_INFO(d),
						psta->phl_sta,
						STA_CHG_RAMASK,
						NULL,
						0,
						PHL_CMD_NO_WAIT,
						0);
		}

		RTW_INFO("CSA : update beacon done, WIFI_CSA_UPDATE_BEACON is clear\n");
	}
#endif /* CONFIG_ECSA_PHL */

	if (_rtw_memcmp(&recv_beacon, cur_beacon, sizeof(recv_beacon)) == _FALSE) {
		RTW_INFO(FUNC_ADPT_FMT" new beacon occur!!\n", FUNC_ADPT_ARG(adapter));
		RTW_INFO(FUNC_ADPT_FMT" cur beacon key:\n", FUNC_ADPT_ARG(adapter));
		rtw_dump_bcn_keys(RTW_DBGDUMP, cur_beacon);
		RTW_INFO(FUNC_ADPT_FMT" new beacon key:\n", FUNC_ADPT_ARG(adapter));
		rtw_dump_bcn_keys(RTW_DBGDUMP, &recv_beacon);

		if (recv_beacon.ch != cur_beacon->ch)
			pmlmeinfo->illegal_beacon_code |= BEACON_CHANNEL_CHANGED;
		if (recv_beacon.encryp_protocol != cur_beacon->encryp_protocol)
			pmlmeinfo->illegal_beacon_code |= ENCRYPT_PROTOCOL_CHANGED;
		if (recv_beacon.pairwise_cipher != cur_beacon->pairwise_cipher)
			pmlmeinfo->illegal_beacon_code |= PAIRWISE_CIPHER_CHANGED;
		if (recv_beacon.group_cipher != cur_beacon->group_cipher)
			pmlmeinfo->illegal_beacon_code |= GROUP_CIPHER_CHANGED;
		if (recv_beacon.akm != cur_beacon->akm)
			pmlmeinfo->illegal_beacon_code |= IS_8021X_CHANGED;

		if (rtw_bcn_key_compare(cur_beacon, &recv_beacon) == _FALSE)
			goto exit;

		_rtw_memcpy(cur_beacon, &recv_beacon, sizeof(recv_beacon));
	}

exit_success:
	ret = 1;

exit:
	return ret;
}

void update_beacon_info(_adapter *padapter, u8 *pframe, uint pkt_len, struct sta_info *psta)
{
	unsigned int i;
	unsigned int len;
	PNDIS_802_11_VARIABLE_IEs	pIE;
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;

#ifdef CONFIG_TDLS
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
	u8 tdls_prohibited[] = { 0x00, 0x00, 0x00, 0x00, 0x10 }; /* bit(38): TDLS_prohibited */
#endif /* CONFIG_TDLS */

	len = pkt_len - (_BEACON_IE_OFFSET_ + WLAN_HDR_A3_LEN);

	for (i = 0; i < len;) {
		pIE = (PNDIS_802_11_VARIABLE_IEs)(pframe + (_BEACON_IE_OFFSET_ + WLAN_HDR_A3_LEN) + i);

		switch (pIE->ElementID) {
		case _VENDOR_SPECIFIC_IE_:
			/* to update WMM paramter set while receiving beacon */
			if (_rtw_memcmp(pIE->data, WMM_PARA_OUI, 6) && pIE->Length == WLAN_WMM_LEN)	/* WMM */
				(WMM_param_handler(padapter, padapter_link, pIE)) ? report_wmm_edca_update(padapter) : 0;

			break;

		case _HT_EXTRA_INFO_IE_:	/* HT info */
			/* HT_info_handler(padapter, pIE); */
			bwmode_update_check(padapter, padapter_link, pIE);
			break;
#ifdef CONFIG_80211AC_VHT
		case EID_OpModeNotification:
			rtw_process_vht_op_mode_notify(padapter, pIE->data, psta);
			break;
#endif /* CONFIG_80211AC_VHT */
		case _ERPINFO_IE_:
			ERP_IE_handler(padapter, padapter_link, pIE);
			VCS_update(padapter, psta);
			break;

#ifdef CONFIG_TDLS
		case WLAN_EID_EXT_CAP:
			if (check_ap_tdls_prohibited(pIE->data, pIE->Length) == _TRUE)
				ptdlsinfo->ap_prohibited = _TRUE;
			if (check_ap_tdls_ch_switching_prohibited(pIE->data, pIE->Length) == _TRUE)
				ptdlsinfo->ch_switch_prohibited = _TRUE;
			break;
#endif /* CONFIG_TDLS */
#ifdef CONFIG_80211AX_HE
		case WLAN_EID_EXTENSION:
			if (pIE->data[0] == WLAN_EID_EXTENSION_HE_OPERATION)
				HE_operation_handler(padapter, padapter_link, pIE);
			else if (pIE->data[0] == WLAN_EID_EXTENSION_HE_MU_EDCA)
				HE_mu_edca_handler(padapter, padapter_link, pIE, _FALSE);
			break;
#endif
		default:
			break;
		}

		i += (pIE->Length + 2);
	}
}

#ifdef CONFIG_ECSA_PHL
void process_csa_ie(_adapter *padapter, u8 *ies, uint ies_len)
{
	struct core_ecsa_info *ecsa_info = &(padapter->ecsa_info);
	struct rtw_phl_ecsa_param *ecsa_param = &(ecsa_info->phl_ecsa_param);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct ieee80211_info_element *ie;
	u8 *wide_bw_ie;
	uint wide_bw_ie_len;
	u8 csa_ch = 0, csa_offset = 0, csa_ch_width = 0xff, seg_0 = 0, seg_1 = 0;
	u8 csa_switch_cnt = 0, csa_mode = 0;
	u8 ecsa_mode = 0, ecsa_op_class = 0, ecsa_ch = 0, ecsa_switch_cnt = 0;

#ifdef DBG_CSA
	u8 *p;
	u32 ie_len = 0;
	p = rtw_get_ie(ies, WLAN_EID_CHANNEL_SWITCH, &ie_len, ies_len);
	if (p && ie_len > 0)
		RTW_INFO("CSA : %s, CH = %u, count = %u\n",__func__, *(p+2+1), *(p+2+2));
#endif

	/* compare with scheduling CSA to block incoming CSA IE */
	if (rtw_mr_is_ecsa_running(padapter)
		|| check_fwstate(pmlmepriv, WIFI_CSA_UPDATE_BEACON))
		return;

	for_each_ie(ie, ies, ies_len) {
		switch (ie->id) {
		case WLAN_EID_CHANNEL_SWITCH:
			csa_mode = *(ie->data);
			csa_ch = *(ie->data + 1);
			csa_switch_cnt = *(ie->data + 2);

			RTW_INFO("CSA : CHANNEL_SWITCH IE, mode = %u, ch = %u, switch count = %u\n",
				csa_mode, csa_ch, csa_switch_cnt);
			break;
		/* This element is not necessary for channel switching */
		case WLAN_EID_SECONDARY_CHANNEL_OFFSET:
			csa_offset = *(ie->data);

			RTW_INFO("CSA : SECONDARY_CHANNEL_OFFSET IE, secondary_offset = %u\n",
				csa_offset);
			break;
		case WLAN_EID_CHANNEL_SWITCH_WRAPPER:
			wide_bw_ie = rtw_get_ie(ie->data, WLAN_EID_VHT_WIDE_BW_CHSWITCH, &wide_bw_ie_len, ie->len);
			csa_ch_width = *(wide_bw_ie + 2);
			seg_0 = *(wide_bw_ie + 3);
			seg_1 = *(wide_bw_ie + 4);

			RTW_INFO("CSA : WIDE_BW_CHSWITCH IE, channel width = %u, segment_0 = %u, segment_1 = %u\n",
				csa_ch_width, seg_0, seg_1);
			break;
		case WLAN_EID_ECSA:
			ecsa_mode = *(ie->data);
			ecsa_op_class = *(ie->data + 1);
			ecsa_ch = *(ie->data + 2);
			ecsa_switch_cnt = *(ie->data + 3);

			RTW_INFO("CSA : EXT_CHANNEL_SWITCH IE, mode = %u, op class = %u, ch = %u, switch count = %u\n",
				ecsa_mode, ecsa_op_class, ecsa_ch, ecsa_switch_cnt);
			break;
		default:
			break;
		}
	}

	/* Check conflict of CSA IE and ECSA IE */
	if (rtw_is_ecsa_enabled(padapter) && ecsa_ch != 0) {
		if (csa_ch == 0) { /* has ECSA IE, no CSA IE */
			csa_mode = ecsa_mode;
			csa_switch_cnt = ecsa_switch_cnt;
			csa_ch = ecsa_ch;
		} else if (csa_mode != ecsa_mode ||
				csa_ch != ecsa_ch ||
				csa_switch_cnt != ecsa_switch_cnt) { /*has both IE, but conflict */
			RTW_ERR("CSA : There has a conflict of CSA IE and ECSA IE\n");
			return;
		}
	}

	if (csa_ch != 0) {
		ecsa_param->ecsa_type = ECSA_TYPE_STA;
		ecsa_param->mode = csa_mode;
		ecsa_param->op_class = ecsa_op_class;
		ecsa_param->count = csa_switch_cnt;
		ecsa_param->new_chan_def.band = rtw_get_band_type(csa_ch);
		ecsa_param->new_chan_def.chan = csa_ch;
		/* The channel width defined in 802.11 spec */
		ecsa_info->channel_width = csa_ch_width;
		ecsa_param->new_chan_def.offset = csa_offset;
		ecsa_param->new_chan_def.center_freq1 = seg_0;
		ecsa_param->new_chan_def.center_freq2 = seg_1;
		ecsa_param->flag = 0;
		ecsa_param->delay_start_ms = 0;

		SET_ECSA_STATE(padapter, ECSA_ST_SW_START);
		rtw_trigger_phl_ecsa_start(padapter);
	}
}
#endif /* CONFIG_ECSA_PHL */

enum eap_type parsing_eapol_packet(_adapter *padapter, u8 *key_payload, struct sta_info *psta, u8 trx_type)
{
	struct security_priv *psecuritypriv = &(padapter->securitypriv);
	struct ieee802_1x_hdr *hdr;
	struct wpa_eapol_key *key;
	u16 key_info, key_data_length;
	char *trx_msg = trx_type ? "send" : "recv";
	enum eap_type eapol_type;

	hdr = (struct ieee802_1x_hdr *) key_payload;

	 /* WPS - eapol start packet */
	if (hdr->type == 1 && hdr->length == 0) {
		RTW_INFO("%s eapol start packet\n", trx_msg);
		return EAPOL_START;
	}

	if (hdr->type == 0) { /* WPS - eapol packet */
		RTW_INFO("%s eapol packet\n", trx_msg);
		return EAPOL_PACKET;
	}

	key = (struct wpa_eapol_key *) (hdr + 1);
	key_info = be16_to_cpu(*((u16 *)(key->key_info)));
	key_data_length = be16_to_cpu(*((u16 *)(key->key_data_length)));

	if (!(key_info & WPA_KEY_INFO_KEY_TYPE)) { /* WPA group key handshake */
		if (key_info & WPA_KEY_INFO_ACK) {
			RTW_PRINT("%s eapol packet - WPA Group Key 1/2\n", trx_msg);
			eapol_type = EAPOL_WPA_GROUP_KEY_1_2;
		} else {
			RTW_PRINT("%s eapol packet - WPA Group Key 2/2\n", trx_msg);
			eapol_type = EAPOL_WPA_GROUP_KEY_2_2;

			/* WPA key-handshake has completed */
			if (psecuritypriv->ndisauthtype == Ndis802_11AuthModeWPAPSK)
				psta->state &= (~WIFI_UNDER_KEY_HANDSHAKE);
		}
	} else if (key_info & WPA_KEY_INFO_MIC) {
		if (key_data_length == 0) {
			RTW_PRINT("%s eapol packet 4/4\n", trx_msg);
			eapol_type = EAPOL_4_4;
		} else if (key_info & WPA_KEY_INFO_ACK) {
			RTW_PRINT("%s eapol packet 3/4\n", trx_msg);
			eapol_type = EAPOL_3_4;
		} else {
			RTW_PRINT("%s eapol packet 2/4\n", trx_msg);
			eapol_type = EAPOL_2_4;
		}
	} else {
		RTW_PRINT("%s eapol packet 1/4\n", trx_msg);
		eapol_type = EAPOL_1_4;
	}

	return eapol_type;
}

unsigned int is_ap_in_tkip(struct _ADAPTER_LINK *padapter_link)
{
	u32 i;
	PNDIS_802_11_VARIABLE_IEs	pIE;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX		*cur_network = &(pmlmeinfo->network);

	if (rtw_get_capability((WLAN_BSSID_EX *)cur_network) & WLAN_CAPABILITY_PRIVACY) {
		for (i = sizeof(NDIS_802_11_FIXED_IEs); i < pmlmeinfo->network.IELength;) {
			pIE = (PNDIS_802_11_VARIABLE_IEs)(pmlmeinfo->network.IEs + i);

			switch (pIE->ElementID) {
			case _VENDOR_SPECIFIC_IE_:
				if ((_rtw_memcmp(pIE->data, RTW_WPA_OUI, 4)) && (_rtw_memcmp((pIE->data + 12), WPA_TKIP_CIPHER, 4)))
					return _TRUE;
				break;

			case _RSN_IE_2_:
				if (_rtw_memcmp((pIE->data + 8), RSN_TKIP_CIPHER, 4))
					return _TRUE;

			default:
				break;
			}

			i += (pIE->Length + 2);
		}

		return _FALSE;
	} else
		return _FALSE;

}

unsigned int should_forbid_n_rate(_adapter *padapter)
{
	u32 i;
	PNDIS_802_11_VARIABLE_IEs	pIE;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	WLAN_BSSID_EX  *cur_network = &pmlmepriv->dev_cur_network.network;

	if (rtw_get_capability((WLAN_BSSID_EX *)cur_network) & WLAN_CAPABILITY_PRIVACY) {
		for (i = sizeof(NDIS_802_11_FIXED_IEs); i < cur_network->IELength;) {
			pIE = (PNDIS_802_11_VARIABLE_IEs)(cur_network->IEs + i);

			switch (pIE->ElementID) {
			case _VENDOR_SPECIFIC_IE_:
				if (_rtw_memcmp(pIE->data, RTW_WPA_OUI, 4) &&
				    ((_rtw_memcmp((pIE->data + 12), WPA_CIPHER_SUITE_CCMP, 4)) ||
				     (_rtw_memcmp((pIE->data + 16), WPA_CIPHER_SUITE_CCMP, 4))))
					return _FALSE;
				break;

			case _RSN_IE_2_:
				if ((_rtw_memcmp((pIE->data + 8), RSN_CIPHER_SUITE_CCMP, 4))  ||
				    (_rtw_memcmp((pIE->data + 12), RSN_CIPHER_SUITE_CCMP, 4)))
					return _FALSE;

			default:
				break;
			}

			i += (pIE->Length + 2);
		}

		return _TRUE;
	} else
		return _FALSE;

}


unsigned int is_ap_in_wep(struct _ADAPTER_LINK *padapter_link)
{
	u32 i;
	PNDIS_802_11_VARIABLE_IEs	pIE;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX		*cur_network = &(pmlmeinfo->network);

	if (rtw_get_capability((WLAN_BSSID_EX *)cur_network) & WLAN_CAPABILITY_PRIVACY) {
		for (i = sizeof(NDIS_802_11_FIXED_IEs); i < pmlmeinfo->network.IELength;) {
			pIE = (PNDIS_802_11_VARIABLE_IEs)(pmlmeinfo->network.IEs + i);

			switch (pIE->ElementID) {
			case _VENDOR_SPECIFIC_IE_:
				if (_rtw_memcmp(pIE->data, RTW_WPA_OUI, 4))
					return _FALSE;
				break;

			case _RSN_IE_2_:
				return _FALSE;

			default:
				break;
			}

			i += (pIE->Length + 2);
		}

		return _TRUE;
	} else
		return _FALSE;

}

int wifirate2_ratetbl_inx(unsigned char rate);
int wifirate2_ratetbl_inx(unsigned char rate)
{
	int	inx = 0;
	rate = rate & 0x7f;

	switch (rate) {
	case 54*2:
		inx = 11;
		break;

	case 48*2:
		inx = 10;
		break;

	case 36*2:
		inx = 9;
		break;

	case 24*2:
		inx = 8;
		break;

	case 18*2:
		inx = 7;
		break;

	case 12*2:
		inx = 6;
		break;

	case 9*2:
		inx = 5;
		break;

	case 6*2:
		inx = 4;
		break;

	case 11*2:
		inx = 3;
		break;
	case 11:
		inx = 2;
		break;

	case 2*2:
		inx = 1;
		break;

	case 1*2:
		inx = 0;
		break;

	}
	return inx;
}

unsigned int update_basic_rate(unsigned char *ptn, unsigned int ptn_sz)
{
	unsigned int i, num_of_rate;
	unsigned int mask = 0;

	num_of_rate = (ptn_sz > NumRates) ? NumRates : ptn_sz;

	for (i = 0; i < num_of_rate; i++) {
		if ((*(ptn + i)) & 0x80)
			mask |= 0x1 << wifirate2_ratetbl_inx(*(ptn + i));
	}
	return mask;
}

unsigned int update_supported_rate(unsigned char *ptn, unsigned int ptn_sz)
{
	unsigned int i, num_of_rate;
	unsigned int mask = 0;

	num_of_rate = (ptn_sz > NumRates) ? NumRates : ptn_sz;

	for (i = 0; i < num_of_rate; i++)
		mask |= 0x1 << wifirate2_ratetbl_inx(*(ptn + i));

	return mask;
}

int support_short_GI(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			struct HT_caps_element *pHT_caps, u8 bwmode)
{
	unsigned char					bit_offset;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (!(pmlmeinfo->HT_enable))
		return _FAIL;

	bit_offset = (bwmode & CHANNEL_WIDTH_40) ? 6 : 5;

	if (pHT_caps->u.HT_cap_element.HT_caps_info & (0x1 << bit_offset))
		return _SUCCESS;
	else
		return _FAIL;
}

unsigned char get_highest_rate_idx(u64 mask)
{
	int i;
	unsigned char rate_idx = 0;

	for (i = 63; i >= 0; i--) {
		if ((mask >> i) & 0x01) {
			rate_idx = i;
			break;
		}
	}

	return rate_idx;
}
unsigned char get_lowest_rate_idx_ex(u64 mask, int start_bit)
{
	int i;
	unsigned char rate_idx = 0;

	for (i = start_bit; i < 64; i++) {
		if ((mask >> i) & 0x01) {
			rate_idx = i;
			break;
		}
	}

	return rate_idx;
}

u8 get_highest_bw_cap(u8 bwmode)
{
	u8 hbw = CHANNEL_WIDTH_20;

	if (bwmode & BW_CAP_80_80M)
		hbw = CHANNEL_WIDTH_80_80;
	else if (bwmode & BW_CAP_160M)
		hbw = CHANNEL_WIDTH_160;
	else if (bwmode & BW_CAP_80M)
		hbw = CHANNEL_WIDTH_80;
	else if (bwmode & BW_CAP_40M)
		hbw = CHANNEL_WIDTH_40;
	else if (bwmode & BW_CAP_20M)
		hbw = CHANNEL_WIDTH_20;
	else if (bwmode & BW_CAP_10M)
		hbw = CHANNEL_WIDTH_10;
	else if (bwmode & BW_CAP_5M)
		hbw = CHANNEL_WIDTH_5;

	return hbw;
}

/* Update RRSR and Rate for USERATE */
void update_tx_basic_rate(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 wirelessmode)
{
	NDIS_802_11_RATES_EX	supported_rates;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;

	_rtw_memset(supported_rates, 0, NDIS_802_11_LENGTH_RATES_EX);

	/* clear B mod if current channel is in 5G band, avoid tx cck rate in 5G band. */
	if (pmlmeext->chandef.chan > 14)
		wirelessmode &= ~(WLAN_MD_11B);

	if ((wirelessmode & WLAN_MD_11B) && (wirelessmode == WLAN_MD_11B))
		_rtw_memcpy(supported_rates, rtw_basic_rate_cck, 4);
	else if (wirelessmode & WLAN_MD_11B)
		_rtw_memcpy(supported_rates, rtw_basic_rate_mix, 7);
	else
		_rtw_memcpy(supported_rates, rtw_basic_rate_ofdm, 3);

	if (wirelessmode & WLAN_MD_11B)
		update_mgnt_tx_rate(padapter, padapter_link, IEEE80211_CCK_RATE_1MB);
	else
		update_mgnt_tx_rate(padapter, padapter_link, IEEE80211_OFDM_RATE_6MB);

	rtw_hal_set_hwreg(padapter, HW_VAR_BASIC_RATE, supported_rates);
}

unsigned char check_assoc_AP(u8 *pframe, uint len)
{
	unsigned int	i;
	PNDIS_802_11_VARIABLE_IEs	pIE;

	for (i = sizeof(NDIS_802_11_FIXED_IEs); i < len;) {
		pIE = (PNDIS_802_11_VARIABLE_IEs)(pframe + i);

		switch (pIE->ElementID) {
		case _VENDOR_SPECIFIC_IE_:
			if ((_rtw_memcmp(pIE->data, ARTHEROS_OUI1, 3)) || (_rtw_memcmp(pIE->data, ARTHEROS_OUI2, 3))) {
				RTW_INFO("link to Artheros AP\n");
				return HT_IOT_PEER_ATHEROS;
			} else if ((_rtw_memcmp(pIE->data, BROADCOM_OUI1, 3))
				   || (_rtw_memcmp(pIE->data, BROADCOM_OUI2, 3))
				|| (_rtw_memcmp(pIE->data, BROADCOM_OUI3, 3))) {
				RTW_INFO("link to Broadcom AP\n");
				return HT_IOT_PEER_BROADCOM;
			} else if (_rtw_memcmp(pIE->data, MARVELL_OUI, 3)) {
				RTW_INFO("link to Marvell AP\n");
				return HT_IOT_PEER_MARVELL;
			} else if (_rtw_memcmp(pIE->data, RALINK_OUI, 3)) {
				RTW_INFO("link to Ralink AP\n");
				return HT_IOT_PEER_RALINK;
			} else if (_rtw_memcmp(pIE->data, CISCO_OUI, 3)) {
				RTW_INFO("link to Cisco AP\n");
				return HT_IOT_PEER_CISCO;
			} else if (_rtw_memcmp(pIE->data, REALTEK_OUI, 3)) {
				u32	Vender = HT_IOT_PEER_REALTEK;

				if (pIE->Length >= 5) {
					if (pIE->data[4] == 1) {
						/* if(pIE->data[5] & RT_HT_CAP_USE_LONG_PREAMBLE) */
						/*	bssDesc->BssHT.RT2RT_HT_Mode |= RT_HT_CAP_USE_LONG_PREAMBLE; */

						if (pIE->data[5] & RT_HT_CAP_USE_92SE) {
							/* bssDesc->BssHT.RT2RT_HT_Mode |= RT_HT_CAP_USE_92SE; */
							Vender = HT_IOT_PEER_REALTEK_92SE;
						}
					}

					if (pIE->data[5] & RT_HT_CAP_USE_SOFTAP)
						Vender = HT_IOT_PEER_REALTEK_SOFTAP;

					if (pIE->data[4] == 2) {
						if (pIE->data[6] & RT_HT_CAP_USE_JAGUAR_CBV) {
							Vender = HT_IOT_PEER_REALTEK_JAGUAR_CBVAP;
							RTW_INFO("link to Realtek JAGUAR_CBVAP\n");
						}
						if (pIE->data[6] & RT_HT_CAP_USE_JAGUAR_CCV) {
							Vender = HT_IOT_PEER_REALTEK_JAGUAR_CCVAP;
							RTW_INFO("link to Realtek JAGUAR_CCVAP\n");
						}
					}
				}

				RTW_INFO("link to Realtek AP\n");
				return Vender;
			} else if (_rtw_memcmp(pIE->data, AIRGOCAP_OUI, 3)) {
				RTW_INFO("link to Airgo Cap\n");
				return HT_IOT_PEER_AIRGO;
			} else
				break;

		default:
			break;
		}

		i += (pIE->Length + 2);
	}

	RTW_INFO("link to new AP\n");
	return HT_IOT_PEER_UNKNOWN;
}

void get_assoc_AP_Vendor(char *vendor, u8 assoc_AP_vendor)
{
	switch (assoc_AP_vendor) {
	
	case HT_IOT_PEER_UNKNOWN:
	sprintf(vendor, "%s", "unknown");
	break;

	case HT_IOT_PEER_REALTEK:
	case HT_IOT_PEER_REALTEK_92SE:
	case HT_IOT_PEER_REALTEK_SOFTAP:
	case HT_IOT_PEER_REALTEK_JAGUAR_CBVAP:
	case HT_IOT_PEER_REALTEK_JAGUAR_CCVAP:

	sprintf(vendor, "%s", "Realtek");
	break;

	case HT_IOT_PEER_BROADCOM:
	sprintf(vendor, "%s", "Broadcom");
	break;

	case HT_IOT_PEER_MARVELL:
	sprintf(vendor, "%s", "Marvell");
	break;

	case HT_IOT_PEER_RALINK:
	sprintf(vendor, "%s", "Ralink");
	break;

	case HT_IOT_PEER_CISCO:
	sprintf(vendor, "%s", "Cisco");
	break;

	case HT_IOT_PEER_AIRGO:
	sprintf(vendor, "%s", "Airgo");
	break;

	case HT_IOT_PEER_ATHEROS:
	sprintf(vendor, "%s", "Atheros");
	break;

	default:
	sprintf(vendor, "%s", "unkown");
	break;
	}

}
#ifdef CONFIG_RTS_FULL_BW
void rtw_parse_sta_vendor_ie_8812(_adapter *adapter, struct sta_info *sta, u8 *tlv_ies, u16 tlv_ies_len)
{
	unsigned char REALTEK_OUI[] = {0x00,0xe0, 0x4c};
	u8 *p;

	p = rtw_get_ie_ex(tlv_ies, tlv_ies_len, WLAN_EID_VENDOR_SPECIFIC, REALTEK_OUI, 3, NULL, NULL);
	if (!p)
		goto exit;
	else {
		if(*(p+1) > 6 ) {

			if(*(p+6) != 2)
				goto exit;
			
			if(*(p+8) == RT_HT_CAP_USE_JAGUAR_CBV)
				sta->vendor_8812 = TRUE;
			else if (*(p+8) == RT_HT_CAP_USE_JAGUAR_CCV)
				sta->vendor_8812 = TRUE;
		}
	}
exit:
	return;
}
#endif/*CONFIG_RTS_FULL_BW*/

#ifdef CONFIG_80211AC_VHT
void get_vht_bf_cap(u8 *pframe, uint len, struct vht_bf_cap *bf_cap)
{
	unsigned int i;
	PNDIS_802_11_VARIABLE_IEs pIE;

	for (i = sizeof(NDIS_802_11_FIXED_IEs); i < len;) {
		pIE = (PNDIS_802_11_VARIABLE_IEs)(pframe + i);

		switch (pIE->ElementID) {

		case EID_VHTCapability:
			bf_cap->is_mu_bfer = GET_VHT_CAPABILITY_ELE_MU_BFER(pIE->data);
			bf_cap->su_sound_dim = GET_VHT_CAPABILITY_ELE_SU_BFER_SOUND_DIM_NUM(pIE->data);
			break;
		default:
			break;
		}
		i += (pIE->Length + 2);
	}
}
#endif

void update_capinfo(_adapter *adapter, struct _ADAPTER_LINK *adapter_link, u16 updateCap)
{
	struct link_mlme_ext_priv	*pmlmeext = &adapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	BOOLEAN		ShortPreamble;

	/* Check preamble mode, 2005.01.06, by rcnjko. */
	/* Mark to update preamble value forever, 2008.03.18 by lanhsin */
	/* if( pMgntInfo->RegPreambleMode == PREAMBLE_AUTO ) */
	{

		if (updateCap & cShortPreamble) {
			/* Short Preamble */
			if (pmlmeinfo->preamble_mode != PREAMBLE_SHORT) { /* PREAMBLE_LONG or PREAMBLE_AUTO */
				ShortPreamble = _TRUE;
				pmlmeinfo->preamble_mode = PREAMBLE_SHORT;
				rtw_hal_set_hwreg(adapter, HW_VAR_ACK_PREAMBLE, (u8 *)&ShortPreamble);
			}
		} else {
			/* Long Preamble */
			if (pmlmeinfo->preamble_mode != PREAMBLE_LONG) { /* PREAMBLE_SHORT or PREAMBLE_AUTO */
				ShortPreamble = _FALSE;
				pmlmeinfo->preamble_mode = PREAMBLE_LONG;
				rtw_hal_set_hwreg(adapter, HW_VAR_ACK_PREAMBLE, (u8 *)&ShortPreamble);
			}
		}
	}

	if (updateCap & cIBSS) {
		/* Filen: See 802.11-2007 p.91 */
		pmlmeinfo->slotTime = NON_SHORT_SLOT_TIME;
	} else {
		/* Filen: See 802.11-2007 p.90 */
		if (pmlmeext->cur_wireless_mode & (WLAN_MD_11N | WLAN_MD_11A | WLAN_MD_11AC))
			pmlmeinfo->slotTime = SHORT_SLOT_TIME;
		else if (pmlmeext->cur_wireless_mode & (WLAN_MD_11G)) {
			if ((updateCap & cShortSlotTime) /* && (!(pMgntInfo->pHTInfo->RT2RT_HT_Mode & RT_HT_CAP_USE_LONG_PREAMBLE)) */) {
				/* Short Slot Time */
				pmlmeinfo->slotTime = SHORT_SLOT_TIME;
			} else {
				/* Long Slot Time */
				pmlmeinfo->slotTime = NON_SHORT_SLOT_TIME;
			}
		} else {
			/* B Mode */
			pmlmeinfo->slotTime = NON_SHORT_SLOT_TIME;
		}
	}

	rtw_hal_set_hwreg(adapter, HW_VAR_SLOT_TIME, &pmlmeinfo->slotTime);

}

/*
* set adapter.mlmeextpriv.mlmext_info.HT_enable
* set adapter.mlmeextpriv.cur_wireless_mode
* set SIFS register
* set mgmt tx rate
*/
void update_wireless_mode(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	int ratelen, network_type = 0;
	u32 SIFS_Timer;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX		*cur_network = &(pmlmeinfo->network);
	unsigned char			*rate = cur_network->SupportedRates;
#ifdef CONFIG_P2P
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P */

	ratelen = rtw_get_rateset_len(cur_network->SupportedRates);

	if ((pmlmeinfo->HT_info_enable) && (pmlmeinfo->HT_caps_enable))
		pmlmeinfo->HT_enable = 1;

#if CONFIG_IEEE80211_BAND_6GHZ
	if (pmlmeext->chandef.band == BAND_ON_6G)
		network_type = WLAN_MD_6G_MIX;
	else
#endif
	if (pmlmeext->chandef.band == BAND_ON_5G) {
		if (pmlmeinfo->HE_enable)
			network_type = WLAN_MD_11AX;
		else if (pmlmeinfo->VHT_enable)
			network_type = WLAN_MD_11AC;
		else if (pmlmeinfo->HT_enable)
			network_type = WLAN_MD_11N;

		network_type |= WLAN_MD_11A;
	} else {
		if (pmlmeinfo->HE_enable)
			network_type = WLAN_MD_11AX;
		else if (pmlmeinfo->VHT_enable)
			network_type = WLAN_MD_11AC;
		else if (pmlmeinfo->HT_enable)
			network_type = WLAN_MD_11N;

		if ((cckratesonly_included(rate, ratelen)) == _TRUE)
			network_type |= WLAN_MD_11B;
		else if ((cckrates_included(rate, ratelen)) == _TRUE)
			network_type |= WLAN_MD_11BG;
		else
			network_type |= WLAN_MD_11G;
	}

	pmlmeext->cur_wireless_mode = network_type & padapter->registrypriv.wireless_mode;
	/* RTW_INFO("network_type=%02x, padapter->registrypriv.wireless_mode=%02x\n", network_type, padapter->registrypriv.wireless_mode); */

	if ((pmlmeext->cur_wireless_mode & WLAN_MD_11B) &&
	    #ifdef CONFIG_P2P
	    !rtw_p2p_chk_role(pwdinfo, P2P_ROLE_CLIENT) &&
	    !rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO) &&
	    #endif
	    1)
		update_mgnt_tx_rate(padapter, padapter_link, IEEE80211_CCK_RATE_1MB);
	else
		update_mgnt_tx_rate(padapter, padapter_link, IEEE80211_OFDM_RATE_6MB);
}

void update_sta_basic_rate(struct sta_info *psta, u8 wireless_mode)
{
	if (is_supported_tx_cck(wireless_mode)) {
		/* Only B, B/G, and B/G/N AP could use CCK rate */
		_rtw_memcpy(psta->bssrateset, rtw_basic_rate_cck, 4);
		psta->bssratelen = 4;
	} else {
		_rtw_memcpy(psta->bssrateset, rtw_basic_rate_ofdm, 3);
		psta->bssratelen = 3;
	}
}

int rtw_ies_get_supported_rate(u8 *ies, uint ies_len, u8 *rate_set, u8 *rate_num)
{
	u8 *ie, *p;
	unsigned int ie_len;
	int i, j;

	struct support_rate_handler support_rate_tbl[] = {
		{IEEE80211_CCK_RATE_1MB, 		_FALSE,		_FALSE},
		{IEEE80211_CCK_RATE_2MB, 		_FALSE,		_FALSE},
		{IEEE80211_CCK_RATE_5MB, 		_FALSE,		_FALSE},
		{IEEE80211_CCK_RATE_11MB,		_FALSE,		_FALSE},
		{IEEE80211_OFDM_RATE_6MB,		_FALSE,		_FALSE},
		{IEEE80211_OFDM_RATE_9MB,		_FALSE,		_FALSE},
		{IEEE80211_OFDM_RATE_12MB,		_FALSE,		_FALSE},
		{IEEE80211_OFDM_RATE_18MB,		_FALSE,		_FALSE},
		{IEEE80211_OFDM_RATE_24MB,		_FALSE,		_FALSE},
		{IEEE80211_OFDM_RATE_36MB,		_FALSE,		_FALSE},
		{IEEE80211_OFDM_RATE_48MB,		_FALSE,		_FALSE},
		{IEEE80211_OFDM_RATE_54MB,		_FALSE,		_FALSE},
	};
		
	if (!rate_set || !rate_num)
		return _FALSE;

	*rate_num = 0;
	ie = rtw_get_ie(ies, _SUPPORTEDRATES_IE_, &ie_len, ies_len);
	if (ie == NULL)
		goto ext_rate;

	/* get valid supported rates */
	for (i = 0; i < 12; i++) {
		p = ie + 2;
		for (j = 0; j < ie_len; j++) {
			if ((*p & ~BIT(7)) == support_rate_tbl[i].rate){
				support_rate_tbl[i].existence = _TRUE;
				if ((*p) & BIT(7))
					support_rate_tbl[i].basic = _TRUE;
			}
			p++;
		}
	}

ext_rate:
	ie = rtw_get_ie(ies, _EXT_SUPPORTEDRATES_IE_, &ie_len, ies_len);
	if (ie) {
		/* get valid extended supported rates */
		for (i = 0; i < 12; i++) {
			p = ie + 2;
			for (j = 0; j < ie_len; j++) {
				if ((*p & ~BIT(7)) == support_rate_tbl[i].rate){
					support_rate_tbl[i].existence = _TRUE;
					if ((*p) & BIT(7))
						support_rate_tbl[i].basic = _TRUE;
				}
				p++;
			}
		}
	}

	for (i = 0; i < 12; i++){
		if (support_rate_tbl[i].existence){
			if (support_rate_tbl[i].basic)
				rate_set[*rate_num] = support_rate_tbl[i].rate | IEEE80211_BASIC_RATE_MASK;
			else
				rate_set[*rate_num] = support_rate_tbl[i].rate;
			*rate_num += 1;
		}
	}

	if (*rate_num == 0)
		return _FAIL;

	if (0) {
		int i;

		for (i = 0; i < *rate_num; i++)
			RTW_INFO("rate:0x%02x\n", *(rate_set + i));
	}

	return _SUCCESS;
}

void process_addba_req(_adapter *padapter, u8 *paddba_req, u8 *addr)
{
	struct sta_info *psta;
	u16 tid, start_seq, param;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct ADDBA_request	*preq = (struct ADDBA_request *)paddba_req;
	u8 size, accept = _FALSE;

	psta = rtw_get_stainfo(pstapriv, addr);
	if (!psta)
		goto exit;

	start_seq = le16_to_cpu(preq->BA_starting_seqctrl) >> 4;
	param = le16_to_cpu(preq->BA_para_set);
	tid = (param >> 2) & 0x0f;

	accept = rtw_rx_ampdu_is_accept(padapter);
	if (padapter->fix_rx_ampdu_size != RX_AMPDU_SIZE_INVALID)
		size = padapter->fix_rx_ampdu_size;
	else {
		size = rtw_rx_ampdu_size(padapter);
		size = rtw_min(size, rx_ampdu_size_sta_limit(padapter, psta));
	}

	if (accept == _TRUE)
		rtw_addbarsp_cmd(padapter, addr, tid, preq, 0, size, start_seq);
	else
		rtw_addbarsp_cmd(padapter, addr, tid, preq, 37, size, start_seq);/* reject ADDBA Req */

exit:
	return;
}

void rtw_process_bar_frame(_adapter *padapter, union recv_frame *precv_frame)
{
	struct sta_priv *pstapriv = &padapter->stapriv;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	struct sta_info *psta = NULL;
	struct recv_reorder_ctrl *preorder_ctrl = NULL;
	u8 tid = 0;
	u16 start_seq=0;

	psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
	if (psta == NULL)
		goto exit;

	tid = ((cpu_to_le16((*(u16 *)(pframe + 16))) & 0xf000) >> 12);
	preorder_ctrl = &psta->recvreorder_ctrl[tid];
	start_seq = ((cpu_to_le16(*(u16 *)(pframe + 18))) >> 4);
	preorder_ctrl->indicate_seq = start_seq;

	rtw_phl_rx_bar(padapter->dvobj->phl, psta->phl_sta, tid, start_seq);
	/* for Debug use */
	if (0)
		RTW_INFO(FUNC_ADPT_FMT" tid=%d, start_seq=%d\n", FUNC_ADPT_ARG(padapter),  tid, start_seq);

exit:
	return;
}

void update_TSF(struct link_mlme_ext_priv *pmlmeext, u8 *pframe, uint len)
{
	u8 *pIE;
	u32 *pbuf;

	pIE = pframe + sizeof(struct rtw_ieee80211_hdr_3addr);
	pbuf = (u32 *)pIE;

	pmlmeext->TSFValue = le32_to_cpu(*(pbuf + 1));

	pmlmeext->TSFValue = pmlmeext->TSFValue << 32;

	pmlmeext->TSFValue |= le32_to_cpu(*pbuf);
}

#ifdef CONFIG_BCN_RECV_TIME
/*	calculate beacon receiving time
	1.RxBCNTime(CCK_1M) = [192us(preamble)] + [length of beacon(byte)*8us] + [10us]
	2.RxBCNTime(OFDM_6M) = [8us(S) + 8us(L) + 4us(L-SIG)] + [(length of beacon(byte)/3 + 1] *4us] + [10us]
*/
inline u16 _rx_bcn_time_calculate(uint bcn_len, u8 data_rate)
{
	u16 rx_bcn_time = 0;/*us*/

	if (data_rate == DESC_RATE1M)
		rx_bcn_time = 192 + bcn_len * 8 + 10;
	else if(data_rate == DESC_RATE6M)
		rx_bcn_time = 8 + 8 + 4 + (bcn_len /3 + 1) * 4 + 10;
/*
	else
		RTW_ERR("%s invalid data rate(0x%02x)\n", __func__, data_rate);
*/
	return rx_bcn_time;
}
void rtw_rx_bcn_time_update(_adapter *adapter, struct _ADAPTER_LINK *adapter_link, uint bcn_len, u8 data_rate)
{
	struct link_mlme_ext_priv *pmlmeext = &adapter_link->mlmeextpriv;

	pmlmeext->bcn_rx_time = _rx_bcn_time_calculate(bcn_len, data_rate);
}
#endif

void beacon_timing_control(_adapter *padapter)
{
	rtw_hal_bcn_param_setting(padapter);
}

inline void rtw_collect_bcn_info(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	struct link_mlme_ext_priv *pmlmeext = &adapter_link->mlmeextpriv;

	if (!is_client_associated_to_ap(adapter))
		return;

	pmlmeext->cur_bcn_cnt = pmlmeext->bcn_cnt - pmlmeext->last_bcn_cnt;
	pmlmeext->last_bcn_cnt = pmlmeext->bcn_cnt;
	/*TODO get offset of bcn's timestamp*/
	/*pmlmeext->bcn_timestamp;*/
}

inline bool rtw_bmp_is_set(const u8 *bmp, u8 bmp_len, u8 id)
{
	if (id / 8 >= bmp_len)
		return 0;

	return bmp[id / 8] & BIT(id % 8);
}

inline void rtw_bmp_set(u8 *bmp, u8 bmp_len, u8 id)
{
	if (id / 8 < bmp_len)
		bmp[id / 8] |= BIT(id % 8);
}

inline void rtw_bmp_clear(u8 *bmp, u8 bmp_len, u8 id)
{
	if (id / 8 < bmp_len)
		bmp[id / 8] &= ~BIT(id % 8);
}

inline bool rtw_bmp_not_empty(const u8 *bmp, u8 bmp_len)
{
	int i;

	for (i = 0; i < bmp_len; i++) {
		if (bmp[i])
			return 1;
	}

	return 0;
}

inline bool rtw_bmp_not_empty_exclude_bit0(const u8 *bmp, u8 bmp_len)
{
	int i;

	for (i = 0; i < bmp_len; i++) {
		if (i == 0) {
			if (bmp[i] & 0xFE)
				return 1;
		} else {
			if (bmp[i])
				return 1;
		}
	}

	return 0;
}

#ifdef CONFIG_AP_MODE
/* Check the id be set or not in map , if yes , return a none zero value*/
bool rtw_tim_map_is_set(_adapter *padapter, const u8 *map, u8 id)
{
	return rtw_bmp_is_set(map, padapter->stapriv.aid_bmp_len, id);
}

/* Set the id into map array*/
void rtw_tim_map_set(_adapter *padapter, u8 *map, u8 id)
{
	rtw_bmp_set(map, padapter->stapriv.aid_bmp_len, id);
}

/* Clear the id from map array*/
void rtw_tim_map_clear(_adapter *padapter, u8 *map, u8 id)
{
	rtw_bmp_clear(map, padapter->stapriv.aid_bmp_len, id);
}

/* Check have anyone bit be set , if yes return true*/
bool rtw_tim_map_anyone_be_set(_adapter *padapter, const u8 *map)
{
	return rtw_bmp_not_empty(map, padapter->stapriv.aid_bmp_len);
}

/* Check have anyone bit be set exclude bit0 , if yes return true*/
bool rtw_tim_map_anyone_be_set_exclude_aid0(_adapter *padapter, const u8 *map)
{
	return rtw_bmp_not_empty_exclude_bit0(map, padapter->stapriv.aid_bmp_len);
}
#endif /* CONFIG_AP_MODE */

_adapter *dvobj_get_unregisterd_adapter(struct dvobj_priv *dvobj)
{
	_adapter *adapter = NULL;
	int i;

	for (i = 0; i < dvobj->iface_nums; i++) {
		if (dvobj->padapters[i]->registered == 0)
			break;
	}

	if (i < dvobj->iface_nums)
		adapter = dvobj->padapters[i];

	return adapter;
}

_adapter *dvobj_get_adapter_by_addr(struct dvobj_priv *dvobj, u8 *addr)
{
	_adapter *adapter = NULL;
	int i;

	for (i = 0; i < dvobj->iface_nums; i++) {
		if (_rtw_memcmp(dvobj->padapters[i]->mac_addr, addr, ETH_ALEN) == _TRUE)
			break;
	}

	if (i < dvobj->iface_nums)
		adapter = dvobj->padapters[i];

	return adapter;
}

static u32 rtw_get_he_bitrate(u8 mcs, u8 bw, u8 nss, u8 sgi)
{
	static const u32 base[4][3][12] = { /*[bw][GI][MCS] */
		/* BW20 */
		{{   8600000,  17200000,  25800000,  34400000, /* GI 0.8u */
		    51600000,  68800000,  77400000,  86000000,
		   103200000, 114700000, 129000000, 143400000,
		},
		{    8100000,  16300000,  24400000,  32500000, /* GI 1.6u */
		    48800000,  65000000,  73100000,  81300000,
		    97500000, 108300000, 121900000, 135400000,
		},
		{    7300000,  14600000,  21900000,  29300000, /* GI 3.2u */
		    43900000,  58500000,  65800000,  73100000,
		    87800000,  97500000, 109700000, 121900000,
		}},
		/* BW40 */
		{{  17200000,  34400000,  51600000,  68800000, /* GI 0.8u */
		   103200000, 137600000, 154900000, 172100000,
		   206500000, 229400000, 258100000, 286800000,
		},
		{   16300000,  32500000,  48800000,  65000000, /* GI 1.6u */
		    97500000, 130000000, 146300000, 162500000,
		   195000000, 216700000, 243800000, 270800000,
		},
		{   14600000,  29300000,  43900000,  58500000, /* GI 3.2u */
		    87800000, 117000000, 131600000, 146300000,
		   175500000, 195000000, 219400000, 243800000,
		}},
		/* BW80 */
		{{  36000000,  72100000, 108100000, 144100000, /* GI 0.8u */
		   216200000, 288200000, 324300000, 360300000,
		   432400000, 480400000, 540400000, 600500000,
		},
		{   34000000,  68100000, 102100000, 136100000, /* GI 1.6u */
		   204200000, 272200000, 306300000, 340300000,
		   408300000, 453700000, 510400000, 567100000,
		},
		{   30600000,  61300000,  91900000, 122500000, /* GI 3.2u */
		   183800000, 245000000, 275600000, 306300000,
		   367500000, 408300000, 459400000, 510400000,
		}},
		/* BW160 and BW80+80 */
		{{  72100000, 144100000, 216200000, 288200000, /* GI 0.8u */
		   432400000, 576500000, 648500000, 720600000,
		   864700000, 960800000,1080900000,1201000000,
		},
		{   68100000, 136100000, 204200000, 272200000, /* GI 1.6u */
		   408300000, 544400000, 612500000, 680600000,
		   816700000, 907400000,1020800000,1134300000,
		},
		{   61300000, 122500000, 183800000, 245000000, /* GI 3.2u */
		   367500000, 490000000, 551300000, 612500000,
		   735000000, 816700000, 918800000,1020800000,
		}}
	};
	u32 bitrate;
	int bw_idx, sgi_idx;

	if (mcs > 11) {
		RTW_INFO("Invalid mcs = %d\n", mcs);
		return 0;
	}

	if (nss > 4 || nss < 1) {
		RTW_INFO("Now only support nss = 1, 2, 3, 4\n");
		return 0;
	}

	switch (bw) {
	case CHANNEL_WIDTH_80_80:
	case CHANNEL_WIDTH_160:
		bw_idx = 3;
		break;
	case CHANNEL_WIDTH_80:
		bw_idx = 2;
		break;
	case CHANNEL_WIDTH_40:
		bw_idx = 1;
		break;
	case CHANNEL_WIDTH_20:
		bw_idx = 0;
		break;
	default:
		RTW_INFO("bw = %d currently not supported\n", bw);
		return 0;
	}

	/* refer to mdata.rx_gi_ltf */
	switch (sgi) {
	case RTW_GILTF_LGI_4XHE32:
		sgi_idx = 2; /* 3.2 GI */
		break;
	case RTW_GILTF_2XHE16:
	case RTW_GILTF_1XHE16:
		sgi_idx = 1; /* 1.6 GI */
		break;
	case RTW_GILTF_SGI_4XHE08:
	case RTW_GILTF_2XHE08:
	case RTW_GILTF_1XHE08:
		sgi_idx = 0; /* 0.8 GI */
		break;
	default:
		RTW_INFO("gi_ltf = %d currently not supported\n", sgi);
		return 0;
	}
	bitrate = base[bw_idx][sgi_idx][mcs];
	bitrate *= nss;
	return (bitrate/100000);
}

static u32 rtw_get_vht_bitrate(u8 mcs, u8 bw, u8 nss, u8 sgi)
{
	static const u32 base[4][10] = {
		{   6500000,
		   13000000,
		   19500000,
		   26000000,
		   39000000,
		   52000000,
		   58500000,
		   65000000,
		   78000000,
		/* not in the spec, but some devices use this: */
		   86500000,
		},
		{  13500000,
		   27000000,
		   40500000,
		   54000000,
		   81000000,
		  108000000,
		  121500000,
		  135000000,
		  162000000,
		  180000000,
		},
		{  29300000,
		   58500000,
		   87800000,
		  117000000,
		  175500000,
		  234000000,
		  263300000,
		  292500000,
		  351000000,
		  390000000,
		},
		{  58500000,
		  117000000,
		  175500000,
		  234000000,
		  351000000,
		  468000000,
		  526500000,
		  585000000,
		  702000000,
		  780000000,
		},
	};
	u32 bitrate;
	int bw_idx;

	if (mcs > 9) {
		RTW_INFO("Invalid mcs = %d\n", mcs);
		return 0;
	}

	if (nss > 4 || nss < 1) {
		RTW_INFO("Now only support nss = 1, 2, 3, 4\n");
	}

	switch (bw) {
	case CHANNEL_WIDTH_160:
		bw_idx = 3;
		break;
	case CHANNEL_WIDTH_80:
		bw_idx = 2;
		break;
	case CHANNEL_WIDTH_40:
		bw_idx = 1;
		break;
	case CHANNEL_WIDTH_20:
		bw_idx = 0;
		break;
	default:
		RTW_INFO("bw = %d currently not supported\n", bw);
		return 0;
	}

	bitrate = base[bw_idx][mcs];
	bitrate *= nss;

	if (sgi)
		bitrate = (bitrate / 9) * 10;

	/* do NOT round down here */
	return (bitrate + 50000) / 100000;
}

static u32 rtw_get_ht_bitrate(u8 mcs, u8 bw, u8 sgi)
{
	int modulation, streams, bitrate;

	/* the formula below does only work for MCS values smaller than 32 */
	if (mcs >= 32) {
		RTW_INFO("Invalid mcs = %d\n", mcs);
		return 0;
	}

	if (bw > 1) {
		RTW_INFO("Now HT only support bw = 0(20Mhz), 1(40Mhz)\n");
		return 0;
	}

	modulation = mcs & 7;
	streams = (mcs >> 3) + 1;

	bitrate = (bw == 1) ? 13500000 : 6500000;

	if (modulation < 4)
		bitrate *= (modulation + 1);
	else if (modulation == 4)
		bitrate *= (modulation + 2);
	else
		bitrate *= (modulation + 3);

	bitrate *= streams;

	if (sgi)
		bitrate = (bitrate / 9) * 10;

	/* do NOT round down here */
	return (bitrate + 50000) / 100000;
}

/**
 * @bw: 0(20Mhz), 1(40Mhz), 2(80Mhz), 3(160Mhz)
 * @data_rate: enum rtw_data_rate;
 * @sgi: enum rtw_gi_ltf
 * Returns: bitrate in 100kbps
 */
u32 rtw_desc_rate_to_bitrate(u8 bw, u16 data_rate, u8 sgi)
{
	u32 bitrate = DESC_RATE1M;

	if (data_rate <= DESC_RATE54M){
		u16 ofdm_rate[12] = {10, 20, 55, 110,
			60, 90, 120, 180, 240, 360, 480, 540};
		bitrate = ofdm_rate[data_rate];
	} else if ((RTW_DATA_RATE_MCS0 <= data_rate) &&
		   (data_rate <= RTW_DATA_RATE_MCS31)) {
		u8 mcs = data_rate - RTW_DATA_RATE_MCS0;
		bitrate = rtw_get_ht_bitrate(mcs, bw, sgi);
	} else if ((RTW_DATA_RATE_VHT_NSS1_MCS0 <= data_rate) &&
		   (data_rate <= RTW_DATA_RATE_VHT_NSS4_MCS9)) {
		u8 mcs = (data_rate - RTW_DATA_RATE_VHT_NSS1_MCS0) % 10;
		u8 nss = ((data_rate - RTW_DATA_RATE_VHT_NSS1_MCS0) / 10) + 1;
		bitrate = rtw_get_vht_bitrate(mcs, bw, nss, sgi);
	} else if ((RTW_DATA_RATE_HE_NSS1_MCS0 <= data_rate) &&
		   (data_rate <= RTW_DATA_RATE_HE_NSS4_MCS11)) {
		u8 mcs = (data_rate - RTW_DATA_RATE_HE_NSS1_MCS0) % 12;
		u8 nss = ((data_rate - RTW_DATA_RATE_HE_NSS1_MCS0) / 12) + 1;
		bitrate = rtw_get_he_bitrate(mcs, bw, nss, sgi);
	} else {
		/* 60Ghz ??? */
		bitrate = 1;
	}

	return bitrate;
}

u16 rtw_get_current_tx_rate(_adapter *adapter, struct sta_info *psta)
{
	u16 rate_id = 0;
	struct rtw_phl_rainfo ra_info;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	if (!psta)
		return rate_id;

	if (adapter->fix_rate != NO_FIX_RATE)
		rate_id = GET_FIX_RATE(adapter->fix_rate);
	else {
		rtw_phl_query_rainfo(dvobj->phl, psta->phl_sta, &ra_info);
		rate_id = ra_info.rate; /* enum rtw_data_rate */
	}

	return rate_id;
}

u8 rtw_get_current_tx_sgi(_adapter *adapter, struct sta_info *psta)
{
	u8 curr_tx_sgi = 0;
	struct rtw_phl_rainfo ra_info;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	if (!psta)
		return curr_tx_sgi;

	if (adapter->fix_rate != NO_FIX_RATE)
		/* fix rate */
		curr_tx_sgi = GET_FIX_RATE_SGI(adapter->fix_rate);
	else {
		rtw_phl_query_rainfo(dvobj->phl, psta->phl_sta, &ra_info);
		curr_tx_sgi = ra_info.gi_ltf;
	}

	return curr_tx_sgi;
}

bool rtw_chk_phy_can_append_actrl(_adapter *padapter, struct sta_info *psta)
{
	bool is_actrl = _FALSE;
	struct rtw_phl_rainfo ra_info;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	if (!psta)
		return is_actrl;

	if (padapter->fix_rate != NO_FIX_RATE)
		if (rtw_get_current_tx_rate(padapter, psta) < RTW_DATA_RATE_MCS0)
			return is_actrl;

	rtw_phl_query_rainfo(dvobj->phl, psta->phl_sta, &ra_info);
	is_actrl = ra_info.is_actrl;

	return is_actrl;
}

void rtw_get_current_rx_info(_adapter *adapter, struct sta_info *psta,
	u16 *rate, u8 *bw, u8 *gi_ltf)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	if (!psta)
		return;

	rtw_phl_get_rx_stat(dvobj->phl, psta->phl_sta, rate, bw, gi_ltf);
}

#ifdef CONFIG_RTW_MULTI_AP
u8 rtw_get_ch_utilization(_adapter *adapter)
{
	struct dvobj_priv *d = adapter_to_dvobj(adapter);
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(d);
	void *phl = GET_PHL_INFO(d);
	struct rtw_env_report rpt;
	u16 clm, nhm, ch_util;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);

	rtw_phl_get_env_rpt(phl, &rpt, adapter_link->wrlink->hw_band);

	clm = rpt.nhm_cca_ratio;
	nhm = rpt.nhm_ratio;

	ch_util = clm / 3 + (2 * (nhm / 3));
	/* For Multi-AP, scaling 0-100 to 0-255 */
	ch_util = 255 * ch_util / 100;

	return (u8)ch_util;
}

void rtw_ch_util_rpt(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	_adapter *iface;
	int i, j;
	u8 i_rpts = 0;
	u8 *ch_util;
	u8 **bssid;
	u8 threshold = GET_PRIMARY_ADAPTER(adapter)->ch_util_threshold;
	u8 need_rpt = 0;

	if (threshold == 0)
		return;

	ch_util = rtw_zmalloc(sizeof(u8) * dvobj->iface_nums);
	if (!ch_util)
		goto err_out;

	bssid = (u8 **)rtw_zmalloc(sizeof(u8 *) * dvobj->iface_nums);
	if (!bssid)
		goto err_out1;

	for (j = 0; j < dvobj->iface_nums; j++) {
		*(bssid + j) = (u8 *)rtw_zmalloc(sizeof(u8) * ETH_ALEN);
		if (!(*(bssid + j)))
			goto err_out2;
	}

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if ((iface) && MLME_IS_AP(iface) &&
			rtw_iface_at_same_hwband(adapter, iface)) {

			*(ch_util + i_rpts) = rtw_get_ch_utilization(iface);
			_rtw_memcpy(*(bssid + i_rpts), iface->mac_addr, ETH_ALEN);
			if (*(ch_util + i_rpts) > threshold)
				need_rpt = 1;

			i_rpts++;
		}
	}

	if (need_rpt)
		rtw_nlrtw_ch_util_rpt(adapter, i_rpts, ch_util, bssid);

	rtw_mfree(ch_util, sizeof(u8) * dvobj->iface_nums);
	for (i = 0; i < dvobj->iface_nums; i++)
		rtw_mfree(*(bssid + i), ETH_ALEN);

	rtw_mfree(bssid, sizeof(u8 *) * dvobj->iface_nums);

	return;

err_out2:
	for (i = 0; i < j; i++)
		rtw_mfree(*(bssid + i), sizeof(u8) * ETH_ALEN);
	rtw_mfree(bssid, sizeof(sizeof(u8 *) * dvobj->iface_nums));
err_out1:
	rtw_mfree(ch_util, sizeof(u8) * dvobj->iface_nums);
err_out:
	RTW_INFO("[%s] rtw_zmalloc fail\n", __func__);
}
#endif


/*
 * rtw_set_mac_addr_hw() - Set HW MAC address
 * @adapter:	struct PADAPTER
 * @mac_addr:   6-bytes mac address
 *
 * Set Wifi Mac address by writing to the relative HW registers,
 *
 */
void rtw_set_mac_addr_hw(_adapter *padapter, u8 *mac_addr)
{
	struct dvobj_priv *d = adapter_to_dvobj(padapter);
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct rtw_wifi_role_t *phl_role = padapter->phl_role;
	struct rtw_phl_stainfo_t *phl_sta_self = NULL;
	struct sta_info *psta;
	_list *plist, *phead;
	void *phl = GET_PHL_INFO(d);
	u32 index, new_index;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	/* sta_hash */
	phl_sta_self = rtw_phl_get_stainfo_self(phl, padapter_link->wrlink);
	if (phl_sta_self) {
		index = wifi_mac_hash(phl_role->mac_addr);
		new_index = wifi_mac_hash(mac_addr);

		_rtw_spinlock_bh(&(pstapriv->sta_hash_lock));
		phead = &(pstapriv->sta_hash[index]);
		plist = get_next(phead);

		/* find out the one with old mac address, should be only one */
		psta = NULL;
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

			if (psta->phl_sta == phl_sta_self ||
			    (_rtw_memcmp(psta->phl_sta->mac_addr, phl_role->mac_addr, ETH_ALEN) == _TRUE)) {
				break;
			}

			psta = NULL;
			plist = get_next(plist);
		}

		if (psta) {
			rtw_list_delete(&psta->hash_list);
			rtw_list_insert_tail(&psta->hash_list, &(pstapriv->sta_hash[new_index]));
		}

		_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);
	}

	rtw_phl_cmd_wrole_change(phl,
				 padapter->phl_role,
				 NULL,
				 WR_CHG_MADDR,
				 mac_addr,
				 ETH_ALEN,
				 PHL_CMD_DIRECTLY,
				 0);
}
