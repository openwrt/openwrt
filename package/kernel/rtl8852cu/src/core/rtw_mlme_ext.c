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
#define _RTW_MLME_EXT_C_

#include <drv_types.h>

struct mlme_handler mlme_sta_tbl[] = {
	{WIFI_ASSOCREQ,		"OnAssocReq",	&OnAssocReq},
	{WIFI_ASSOCRSP,		"OnAssocRsp",	&OnAssocRsp},
	{WIFI_REASSOCREQ,	"OnReAssocReq",	&OnAssocReq},
	{WIFI_REASSOCRSP,	"OnReAssocRsp",	&OnAssocRsp},
	{WIFI_PROBEREQ,		"OnProbeReq",	&OnProbeReq},
	{WIFI_PROBERSP,		"OnProbeRsp",		&OnProbeRsp},

	/*----------------------------------------------------------
					below 2 are reserved
	-----------------------------------------------------------*/
	{0,					"DoReserved",		&DoReserved},
	{0,					"DoReserved",		&DoReserved},
	{WIFI_BEACON,		"OnBeacon",		&OnBeacon},
	{WIFI_ATIM,			"OnATIM",		&OnAtim},
	{WIFI_DISASSOC,		"OnDisassoc",		&OnDisassoc},
	{WIFI_AUTH,			"OnAuth",		&OnAuthClient},
	{WIFI_DEAUTH,		"OnDeAuth",		&OnDeAuth},
	{WIFI_ACTION,		"OnAction",		&OnAction},
	{WIFI_ACTION_NOACK, "OnActionNoAck",	&OnAction},
};

#ifdef _CONFIG_NATIVEAP_MLME_
struct mlme_handler mlme_ap_tbl[] = {
	{WIFI_ASSOCREQ,		"OnAssocReq",	&OnAssocReq},
	{WIFI_ASSOCRSP,		"OnAssocRsp",	&OnAssocRsp},
	{WIFI_REASSOCREQ,	"OnReAssocReq",	&OnAssocReq},
	{WIFI_REASSOCRSP,	"OnReAssocRsp",	&OnAssocRsp},
	{WIFI_PROBEREQ,		"OnProbeReq",	&OnProbeReq},
	{WIFI_PROBERSP,		"OnProbeRsp",		&OnProbeRsp},

	/*----------------------------------------------------------
					below 2 are reserved
	-----------------------------------------------------------*/
	{0,					"DoReserved",		&DoReserved},
	{0,					"DoReserved",		&DoReserved},
	{WIFI_BEACON,		"OnBeacon",		&OnBeacon},
	{WIFI_ATIM,			"OnATIM",		&OnAtim},
	{WIFI_DISASSOC,		"OnDisassoc",		&OnDisassoc},
	{WIFI_AUTH,			"OnAuth",		&OnAuth},
	{WIFI_DEAUTH,		"OnDeAuth",		&OnDeAuth},
	{WIFI_ACTION,		"OnAction",		&OnAction},
	{WIFI_ACTION_NOACK, "OnActionNoAck",	&OnAction},
};
#endif

struct action_handler OnAction_tbl[] = {
	{RTW_WLAN_CATEGORY_SPECTRUM_MGMT,	 "ACTION_SPECTRUM_MGMT", on_action_spct},
	{RTW_WLAN_CATEGORY_QOS, "ACTION_QOS", &OnAction_qos},
	{RTW_WLAN_CATEGORY_DLS, "ACTION_DLS", &OnAction_dls},
	{RTW_WLAN_CATEGORY_BACK, "ACTION_BACK", &OnAction_back},
	{RTW_WLAN_CATEGORY_PUBLIC, "ACTION_PUBLIC", on_action_public},
	{RTW_WLAN_CATEGORY_RADIO_MEAS, "ACTION_RADIO_MEAS", &on_action_rm},
	{RTW_WLAN_CATEGORY_FT, "ACTION_FT",	&OnAction_ft},
	{RTW_WLAN_CATEGORY_HT,	"ACTION_HT",	&OnAction_ht},
#ifdef CONFIG_IEEE80211W
	{RTW_WLAN_CATEGORY_SA_QUERY, "ACTION_SA_QUERY", &OnAction_sa_query},
#else
	{RTW_WLAN_CATEGORY_SA_QUERY, "ACTION_SA_QUERY", &DoReserved},
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_RTW_WNM
	{RTW_WLAN_CATEGORY_WNM, "ACTION_WNM", &on_action_wnm},
#endif
	{RTW_WLAN_CATEGORY_UNPROTECTED_WNM, "ACTION_UNPROTECTED_WNM", &DoReserved},
#ifdef CONFIG_RTW_MESH
	{RTW_WLAN_CATEGORY_MESH, "ACTION_MESH", &on_action_mesh},
	{RTW_WLAN_CATEGORY_SELF_PROTECTED, "ACTION_SELF_PROTECTED", &on_action_self_protected},
#endif
	{RTW_WLAN_CATEGORY_WMM, "ACTION_WMM", &OnAction_wmm},
	{RTW_WLAN_CATEGORY_VHT, "ACTION_VHT", &OnAction_vht},
	{RTW_WLAN_CATEGORY_HE, "ACTION_HE", &OnAction_he},
	{RTW_WLAN_CATEGORY_PROTECTED_HE, "ACTION_PROTECTED_HE", &OnAction_protected_he},
	{RTW_WLAN_CATEGORY_P2P, "ACTION_P2P", &OnAction_p2p},
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	{RTW_WLAN_CATEGORY_TBTX, "ACTION_TBTX_TOKEN", &OnAction_tbtx_token}
#endif
};


u8	null_addr[ETH_ALEN] = {0, 0, 0, 0, 0, 0};

/**************************************************
OUI definitions for the vendor specific IE
***************************************************/
unsigned char	RTW_WPA_OUI[] = {0x00, 0x50, 0xf2, 0x01};
unsigned char WMM_OUI[] = {0x00, 0x50, 0xf2, 0x02};
unsigned char	WPS_OUI[] = {0x00, 0x50, 0xf2, 0x04};
unsigned char	P2P_OUI[] = {0x50, 0x6F, 0x9A, 0x09};
unsigned char	WFD_OUI[] = {0x50, 0x6F, 0x9A, 0x0A};
unsigned char	DPP_OUI[] = {0x50, 0x6F, 0x9A, 0x1A};
unsigned char	MULTI_AP_OUI[] = {0x50, 0x6F, 0x9A, 0x1B};

unsigned char	WMM_INFO_OUI[] = {0x00, 0x50, 0xf2, 0x02, 0x00, 0x01};
unsigned char	WMM_PARA_OUI[] = {0x00, 0x50, 0xf2, 0x02, 0x01, 0x01};

unsigned char WPA_TKIP_CIPHER[4] = {0x00, 0x50, 0xf2, 0x02};
unsigned char RSN_TKIP_CIPHER[4] = {0x00, 0x0f, 0xac, 0x02};

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
unsigned char REALTEK_TBTX_IE[] = {0x00, 0xe0, 0x4c, 0x01, 0x00, 0x00, 0x00, 0x00};
#endif
extern unsigned char REALTEK_96B_IE[];

void init_channel_list(_adapter *padapter)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	struct registry_priv *regsty = adapter_to_regsty(padapter);
	struct rtw_chset *chset = adapter_to_chset(padapter);
	struct p2p_channels *channel_list = &rfctl->channel_list;

	struct p2p_oper_class_map op_class[] = {
		{ IEEE80211G,  81,   1,  13,  1, BW20 },
		{ IEEE80211G,  82,  14,  14,  1, BW20 },
#if 0 /* Do not enable HT40 on 2 GHz */
		{ IEEE80211G,  83,   1,   9,  1, BW40PLUS },
		{ IEEE80211G,  84,   5,  13,  1, BW40MINUS },
#endif
		{ IEEE80211A, 115,  36,  48,  4, BW20 },
		{ IEEE80211A, 116,  36,  44,  8, BW40PLUS },
		{ IEEE80211A, 117,  40,  48,  8, BW40MINUS },
		{ IEEE80211A, 124, 149, 161,  4, BW20 },
		{ IEEE80211A, 125, 149, 169,  4, BW20 },
		{ IEEE80211A, 126, 149, 157,  8, BW40PLUS },
		{ IEEE80211A, 127, 153, 161,  8, BW40MINUS },
		{ -1, 0, 0, 0, 0, BW20 }
	};

	int cla, op;

	cla = 0;

	for (op = 0; op_class[op].op_class; op++) {
		u8 ch;
		struct p2p_oper_class_map *o = &op_class[op];
		struct p2p_reg_class *reg = NULL;

		for (ch = o->min_chan; ch <= o->max_chan; ch += o->inc) {
			if (rtw_chset_search_ch(chset, ch) == -1)
				continue;
#if defined(CONFIG_80211N_HT) || defined(CONFIG_80211AC_VHT) /* CONFIG_80211AX_HE_TODO */
			if ((padapter->registrypriv.ht_enable == 0) && (o->inc == 8))
				continue;

			if ((REGSTY_IS_BW_5G_SUPPORT(regsty, CHANNEL_WIDTH_40)) &&
			    ((o->bw == BW40MINUS) || (o->bw == BW40PLUS)))
				continue;
#endif
			if (reg == NULL) {
				reg = &channel_list->reg_class[cla];
				cla++;
				reg->reg_class = o->op_class;
				reg->channels = 0;
			}
			reg->channel[reg->channels] = ch;
			reg->channels++;
		}
	}
	channel_list->reg_classes = cla;

}

static void rtw_rfctl_get_band_and_bw_bmp(struct dvobj_priv *dvobj, u8 *band_bmp, u8 bw_bmp_of_band[])
{
	struct registry_priv *regsty = dvobj_to_regsty(dvobj);

	if (is_supported_24g(regsty->band_type) && rtw_hw_chk_band_cap(dvobj, BAND_CAP_2G))
		*band_bmp |= BAND_CAP_2G;
#if CONFIG_IEEE80211_BAND_5GHZ
	if (is_supported_5g(regsty->band_type) && rtw_hw_chk_band_cap(dvobj, BAND_CAP_5G))
		*band_bmp |= BAND_CAP_5G;
#endif
#if CONFIG_IEEE80211_BAND_6GHZ
	if (is_supported_6g(regsty->band_type) && rtw_hw_chk_band_cap(dvobj, BAND_CAP_6G)
		&& REGSTY_IS_11AX_ENABLE(regsty) && is_supported_he(regsty->wireless_mode))
		*band_bmp |= BAND_CAP_6G;
#endif

	bw_bmp_of_band[BAND_ON_24G] = (ch_width_to_bw_cap(REGSTY_BW_2G(regsty) + 1) - 1) & (dvobj->phl_com->dev_cap.bw_sup);
#if CONFIG_IEEE80211_BAND_5GHZ
	bw_bmp_of_band[BAND_ON_5G] = (ch_width_to_bw_cap(REGSTY_BW_5G(regsty) + 1) - 1) & (dvobj->phl_com->dev_cap.bw_sup);
#endif
#if CONFIG_IEEE80211_BAND_6GHZ
	bw_bmp_of_band[BAND_ON_6G] = (ch_width_to_bw_cap(REGSTY_BW_6G(regsty) + 1) - 1) & (dvobj->phl_com->dev_cap.bw_sup);
#endif

#if CONFIG_IEEE80211_BAND_5GHZ
	if (!REGSTY_IS_11AC_ENABLE(regsty)
		|| !is_supported_vht(regsty->wireless_mode)
	)
		bw_bmp_of_band[BAND_ON_5G] &= ~(BW_CAP_80M | BW_CAP_160M);
#endif

	if (0) {
		RTW_INFO("REGSTY_BW_2G(regsty):%u\n", REGSTY_BW_2G(regsty));
		#if CONFIG_IEEE80211_BAND_5GHZ
		RTW_INFO("REGSTY_BW_5G(regsty):%u\n", REGSTY_BW_5G(regsty));
		#endif
		#if CONFIG_IEEE80211_BAND_6GHZ
		RTW_INFO("REGSTY_BW_6G(regsty):%u\n", REGSTY_BW_6G(regsty));
		#endif
		RTW_INFO("dev_cap(adapter)->bw_sup:0x%x\n", dvobj->phl_com->dev_cap.bw_sup);
		RTW_INFO("band_bmp:0x%x\n", *band_bmp);
		RTW_INFO("bw_bmp[2G]:0x%x\n", bw_bmp_of_band[BAND_ON_24G]);
		#if CONFIG_IEEE80211_BAND_5GHZ
		RTW_INFO("bw_bmp[5G]:0x%x\n", bw_bmp_of_band[BAND_ON_5G]);
		#endif
		#if CONFIG_IEEE80211_BAND_6GHZ
		RTW_INFO("bw_bmp[6G]:0x%x\n", bw_bmp_of_band[BAND_ON_6G]);
		#endif
	}
}

int rtw_rfctl_init(struct dvobj_priv *dvobj)
{
	struct registry_priv *regsty = dvobj_to_regsty(dvobj);
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);
	_adapter *adapter = dvobj_get_primary_adapter(dvobj);
	u8 band_bmp = 0;
	u8 bw_bmp_of_band[BAND_MAX] = {0};
	int ret;

	rtw_rfctl_get_band_and_bw_bmp(dvobj, &band_bmp, bw_bmp_of_band);

	rtw_rfctl_addl_ch_disable_conf_init(rfctl, regsty);

	rtw_regd_req_list_init(rfctl, regsty);

	rfctl->bcn_hint_valid_ms = regsty->bcn_hint_valid_ms;

#ifdef CONFIG_80211D
	rtw_rfctl_cis_init(rfctl, regsty);
#endif

	rfctl->last_edcca_mode_op_band = BAND_MAX;

	rtw_rfctl_dfs_init(rfctl, regsty);

	rtw_chset_init(&rfctl->chset, band_bmp);

	ret = rtw_rfctl_op_class_pref_init(rfctl, band_bmp, bw_bmp_of_band);

	return ret;
}

void rtw_rfctl_deinit(struct dvobj_priv *dvobj)
{
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);
	_adapter *adapter = dvobj_get_primary_adapter(dvobj);

	_rtw_mutex_free(&rfctl->offch_mutex);

	rtw_regd_req_list_free(rfctl);

#ifdef CONFIG_80211D
	rtw_rfctl_cis_deinit(rfctl);
#endif

	rtw_rfctl_op_class_pref_deinit(rfctl);
}

void rtw_rfctl_update_op_mode(struct rf_ctl_t *rfctl, u8 ifbmp_mod, u8 if_op)
{
	struct dvobj_priv *dvobj = rfctl_to_dvobj(rfctl);
	_adapter *iface;
	u8 op_class = 0;
	u8 op_ch = 0;
	s16 op_txpwr_max;
	u8 if_op_class[CONFIG_IFACE_NUMBER] = {0};
	u8 if_op_ch[CONFIG_IFACE_NUMBER] = {0};
	enum band_type band, u_band;
	u8 ch, bw, offset;
	u8 u_ch = 0, u_bw, u_offset;
	bool notify = 0;
	int i;
	struct _ADAPTER_LINK *iface_link;
	struct link_mlme_ext_priv *mlmeext;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface)
			continue;

		if (ifbmp_mod & BIT(i)) {
			if (!if_op)
				continue;
		} else if (!MLME_IS_ASOC(iface) || MLME_IS_OPCH_SW(iface))
			continue;

		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		iface_link = GET_PRIMARY_LINK(iface);
		mlmeext = &iface_link->mlmeextpriv;
		band = mlmeext->chandef.band;
		ch = mlmeext->chandef.chan;
		bw = mlmeext->chandef.bw;
		offset = mlmeext->chandef.offset;
		if_op_class[i] = rtw_get_op_class_by_bchbw(band, ch, bw, offset);
		if_op_ch[i] = if_op_class[i] ? ch : 0;

		if (!u_ch) {
			u_band = band;
			u_ch = ch;
			u_bw = bw;
			u_offset = offset;
		} else {
			rtw_warn_on(!rtw_is_bchbw_grouped(u_band, u_ch, u_bw, u_offset, band, ch, bw, offset));
			rtw_sync_bchbw(&band, &ch, &bw, &offset, &u_band, &u_ch, &u_bw, &u_offset);
		}
	}

	op_class = rtw_get_op_class_by_bchbw(u_band, u_ch, u_bw, u_offset);
	op_ch = op_class ? u_ch : 0;
	op_txpwr_max = rtw_rfctl_get_oper_txpwr_max_mbm(rfctl, u_ch, u_bw, u_offset, ifbmp_mod, if_op, 1);

	if (op_class != rfctl->op_class
		|| op_ch != rfctl->op_ch
		|| op_txpwr_max != rfctl->op_txpwr_max
		|| _rtw_memcmp(if_op_class, rfctl->if_op_class, sizeof(u8) * CONFIG_IFACE_NUMBER) == _FALSE
		|| _rtw_memcmp(if_op_ch, rfctl->if_op_ch, sizeof(u8) * CONFIG_IFACE_NUMBER) == _FALSE)
		notify = 1;

	rfctl->op_class = op_class;
	rfctl->op_ch = op_ch;
	rfctl->op_txpwr_max = op_txpwr_max;
	_rtw_memcpy(rfctl->if_op_class, if_op_class, sizeof(u8) * CONFIG_IFACE_NUMBER);
	_rtw_memcpy(rfctl->if_op_ch, if_op_ch, sizeof(u8) * CONFIG_IFACE_NUMBER);

	if (0)
		RTW_INFO("radio: %u,%u,%u,%u %d notify:%d\n", u_band, u_ch, u_bw, u_offset, op_txpwr_max, notify);
	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface)
			continue;
		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		iface_link = GET_PRIMARY_LINK(iface);
		mlmeext = &iface_link->mlmeextpriv;

		if (ifbmp_mod & BIT(i)) {
			if (!if_op)
				continue;
		} else if (!MLME_IS_ASOC(iface))
			continue;
		if (0)
			RTW_INFO(ADPT_FMT": %u,%u,%u,%u\n", ADPT_ARG(iface)
				, mlmeext->chandef.band, mlmeext->chandef.chan, mlmeext->chandef.bw, mlmeext->chandef.offset);
	}

	if (notify)
		rtw_nlrtw_radio_opmode_notify(rfctl);
}

/*
 * Check the @param ch is fit with setband setting of @param adapter
 * @adapter: the given adapter
 * @ch: the given channel number
 *
 * return _TRUE when check valid, _FALSE not valid
 */
bool rtw_mlme_band_check(_adapter *adapter, const u32 ch)
{
	if (adapter->setband == WIFI_FREQUENCY_BAND_AUTO /* 2.4G and 5G */
		|| (adapter->setband == WIFI_FREQUENCY_BAND_2GHZ && ch < 35) /* 2.4G only */
		|| (adapter->setband == WIFI_FREQUENCY_BAND_5GHZ && ch > 35) /* 5G only */
	)
		return _TRUE;
	return _FALSE;
}
inline void RTW_SET_SCAN_BAND_SKIP(_adapter *padapter, int skip_band)
{
	int bs = ATOMIC_READ(&padapter->bandskip);

	bs |= skip_band;
	ATOMIC_SET(&padapter->bandskip, bs);
}

inline void RTW_CLR_SCAN_BAND_SKIP(_adapter *padapter, int skip_band)
{
	int bs = ATOMIC_READ(&padapter->bandskip);

	bs &= ~(skip_band);
	ATOMIC_SET(&padapter->bandskip, bs);
}
inline int RTW_GET_SCAN_BAND_SKIP(_adapter *padapter)
{
	return ATOMIC_READ(&padapter->bandskip);
}

#define RTW_IS_SCAN_BAND_SKIP(padapter, skip_band) (ATOMIC_READ(&padapter->bandskip) & (skip_band))

bool rtw_mlme_ignore_chan(_adapter *adapter, const u32 ch)
{
	if (RTW_IS_SCAN_BAND_SKIP(adapter, BAND_24G) && ch < 35) /* SKIP 2.4G Band channel */
		return _TRUE;
	if (RTW_IS_SCAN_BAND_SKIP(adapter, BAND_5G)  && ch > 35) /* SKIP 5G Band channel */
		return _TRUE;

	return _FALSE;
}


/****************************************************************************

Following are the initialization functions for WiFi MLME

*****************************************************************************/

int init_hw_mlme_ext(_adapter *padapter)
{
	struct _ADAPTER_LINK *padapter_link = NULL;
	struct	link_mlme_ext_priv *pmlmeext = NULL;
	u8 lidx;
	u8 rx_bar_enble = _TRUE;

	/*
	 * Sync driver status and hardware setting
	 */
	/* set_opmode_cmd(padapter, infra_client_with_mlme); */ /* removed */
	rtw_hal_set_hwreg(padapter, HW_VAR_ENABLE_RX_BAR, &rx_bar_enble);

	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
		pmlmeext = &padapter_link->mlmeextpriv;
		set_channel_bwmode(padapter,
				padapter_link,
				pmlmeext->chandef.chan,
				pmlmeext->chandef.offset,
				pmlmeext->chandef.bw,
				_FALSE);
	}

	return _SUCCESS;
}

static void init_mlme_ext_priv_value(_adapter *padapter)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	ATOMIC_SET(&pmlmeext->event_seq, 0);
	pmlmeext->mgnt_seq = 0;/* reset to zero when disconnect at client mode */
#ifdef CONFIG_IEEE80211W
	pmlmeext->sa_query_seq = 0;
#endif
	pmlmeext->retry = 0;

	pmlmeext->dev_cur_wireless_mode = padapter->registrypriv.wireless_mode;

	mlmeext_set_scan_state(pmlmeext, SCAN_DISABLE);
	#if 0
	pmlmeext->sitesurvey_res.channel_idx = 0;
	#endif
	pmlmeext->sitesurvey_res.bss_cnt = 0;
	pmlmeext->sitesurvey_res.scan_ch_ms = SURVEY_TO;
	pmlmeext->sitesurvey_res.rx_ampdu_accept = RX_AMPDU_ACCEPT_INVALID;
	pmlmeext->sitesurvey_res.rx_ampdu_size = RX_AMPDU_SIZE_INVALID;
#ifdef CONFIG_SCAN_BACKOP
#ifdef CONFIG_LAYER2_ROAMING
	if (padapter->mlmepriv.roam_flags & RTW_ROAM_ACTIVE)
		mlmeext_assign_scan_backop_flags_sta(pmlmeext, SS_BACKOP_EN | SS_BACKOP_PS_ANNC | SS_BACKOP_TX_RESUME);
	else
#endif
	mlmeext_assign_scan_backop_flags_sta(pmlmeext, /*SS_BACKOP_EN|*/SS_BACKOP_PS_ANNC | SS_BACKOP_TX_RESUME);

	#ifdef CONFIG_SCAN_BACKOP_STA
	mlmeext_assign_scan_backop_flags_sta(pmlmeext, SS_BACKOP_EN | SS_BACKOP_PS_ANNC | SS_BACKOP_TX_RESUME);
	#endif /*CONFIG_SCAN_BACKOP_STA*/

	#ifdef CONFIG_AP_MODE
	mlmeext_assign_scan_backop_flags_ap(pmlmeext, SS_BACKOP_EN | SS_BACKOP_PS_ANNC | SS_BACKOP_TX_RESUME);
	#endif
	#ifdef CONFIG_RTW_MESH
	mlmeext_assign_scan_backop_flags_mesh(pmlmeext, /*SS_BACKOP_EN | */SS_BACKOP_PS_ANNC | SS_BACKOP_TX_RESUME);
	#endif
	#if 0
	pmlmeext->sitesurvey_res.scan_cnt = 0;
	#endif
	pmlmeext->sitesurvey_res.scan_cnt_max = RTW_SCAN_NUM_OF_CH;
	pmlmeext->sitesurvey_res.backop_ms = RTW_BACK_OP_CH_MS;
#endif
#if defined(CONFIG_ANTENNA_DIVERSITY) || defined(DBG_SCAN_SW_ANTDIV_BL)
	pmlmeext->sitesurvey_res.is_sw_antdiv_bl_scan = 0;
#endif
	pmlmeext->scan_abort = _FALSE;
	pmlmeext->scan_abort_to = _FALSE;

	pmlmeinfo->state = WIFI_FW_NULL_STATE;
	pmlmeinfo->reauth_count = 0;
	pmlmeinfo->reassoc_count = 0;
	pmlmeinfo->link_count = 0;
	pmlmeinfo->auth_seq = 0;
	pmlmeinfo->auth_algo = dot11AuthAlgrthm_Open;
	pmlmeinfo->key_index = 0;
	pmlmeinfo->iv = 0;
	pmlmeinfo->enc_algo = _NO_PRIVACY_;
	pmlmeinfo->authModeToggle = 0;

	_rtw_memset(pmlmeinfo->chg_txt, 0, 128);


	pmlmeinfo->dialogToken = 0;

	pmlmeext->action_public_rxseq = 0xffff;
	pmlmeext->action_public_dialog_token = 0xff;
}

void init_mlme_ext_timer(_adapter *padapter)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;

#if 0
	rtw_init_timer(&pmlmeext->survey_timer, survey_timer_hdl, padapter);
#endif
	rtw_init_timer(&pmlmeext->link_timer, link_timer_hdl, padapter);
#ifdef CONFIG_RTW_80211R
	rtw_init_timer(&pmlmeext->ft_link_timer, rtw_ft_link_timer_hdl, padapter);
	rtw_init_timer(&pmlmeext->ft_roam_timer, rtw_ft_roam_timer_hdl, padapter);
#endif
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	rtw_init_timer(&pmlmeext->tbtx_xmit_timer, rtw_tbtx_xmit_timer_hdl, padapter);
	rtw_init_timer(&pmlmeext->tbtx_token_dispatch_timer, rtw_tbtx_token_dispatch_timer_hdl, padapter);
#endif
#ifdef PRIVATE_R
	rtw_init_timer(&pmlmeext->find_remote_timer, find_remote_timer_hdl, padapter);
#ifdef CONFIG_P2P
	rtw_init_timer(&pmlmeext->hide_ssid_timer, hide_ssid_hdl, padapter);
#endif
#endif
#ifdef CONFIG_ECSA_PHL
	rtw_init_timer(&pmlmeext->csa_wait_bcn_timer, csa_wait_bcn_timer_hdl, padapter);
#endif
}

int	init_mlme_ext_priv(_adapter *padapter)
{
	int	res = _SUCCESS;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	/* We don't need to memset padapter->XXX to zero, because adapter is allocated by rtw_zvmalloc(). */
	/* _rtw_memset((u8 *)pmlmeext, 0, sizeof(struct mlme_ext_priv)); */

	pmlmeext->padapter = padapter;

	init_mlme_ext_priv_value(padapter);
	pmlmeinfo->bAcceptAddbaReq = pregistrypriv->bAcceptAddbaReq;

	init_mlme_ext_timer(padapter);

#ifdef CONFIG_AP_MODE
	init_mlme_ap_info(padapter);
#endif

	pmlmeext->last_scan_time = 0;


#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
	pmlmeext->active_keep_alive_check = _TRUE;
#else
	pmlmeext->active_keep_alive_check = _FALSE;
#endif

#ifdef DBG_FIXED_CHAN
	pmlmeext->fixed_chan = 0xFF;
#endif

#ifdef CONFIG_SUPPORT_STATIC_SMPS
	pmlmeext->ssmps_en = _FALSE;
	pmlmeext->ssmps_tx_tp_th = SSMPS_TX_TP_TH;/*Mbps*/
	pmlmeext->ssmps_rx_tp_th = SSMPS_RX_TP_TH;/*Mbps*/
	#ifdef DBG_STATIC_SMPS
	pmlmeext->ssmps_test = _FALSE;
	#endif
#endif

#ifdef CONFIG_CTRL_TXSS_BY_TP
	pmlmeext->txss_ctrl_en = _TRUE;
	pmlmeext->txss_tp_th = TXSS_TP_TH;
	pmlmeext->txss_tp_chk_cnt = TXSS_TP_CHK_CNT;
#endif

	pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
	pmlmeinfo->disconnect_code = DISCONNECTION_NOT_YET_OCCUR;
	pmlmeinfo->illegal_beacon_code = 0;
	pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;

	return res;

}

void free_mlme_ext_priv(struct mlme_ext_priv *pmlmeext)
{
	_adapter *padapter = pmlmeext->padapter;

	if (!padapter)
		return;

	if (dev_is_drv_stopped(adapter_to_dvobj(padapter))) {
		/*cancel_survey_timer(pmlmeext);*/ /*_cancel_timer_ex(&pmlmeext->survey_timer);*/
		cancel_link_timer(pmlmeext); /*_cancel_timer_ex(&pmlmeext->link_timer);*/
#ifdef PRIVATE_R
		_cancel_timer_ex(&pmlmeext->find_remote_timer);
#ifdef CONFIG_P2P
		_cancel_timer_ex(&pmlmeext->hide_ssid_timer);
#endif
#endif
#ifdef CONFIG_ECSA_PHL
		_cancel_timer_ex(&pmlmeext->csa_wait_bcn_timer);
#endif
	}
}

void init_link_mlme_default_rate_set(struct _ADAPTER_LINK *padapter_link)
{
	_adapter *padapter = padapter_link->adapter;
	struct	link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	unsigned	char end_set[1] = {0xff};
	u8	offset_datarate = 0;
	u8	offset_basicrate = 0;
#ifdef CONFIG_80211N_HT
	unsigned char	supported_mcs_set[16] = {0xff, 0xff, 0xff, 0xff, 0x00, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
#endif

	if (is_supported_tx_cck(padapter->registrypriv.wireless_mode)) {

		unsigned char	datarate_b[B_MODE_RATE_NUM] ={_1M_RATE_, _2M_RATE_, _5M_RATE_, _11M_RATE_};
		_rtw_memcpy(pmlmeext->datarate, datarate_b, B_MODE_RATE_NUM);
		_rtw_memcpy(pmlmeext->basicrate, datarate_b, B_MODE_RATE_NUM);
		offset_datarate += B_MODE_RATE_NUM;
		offset_basicrate += B_MODE_RATE_NUM;
		RTW_INFO("%s: support CCK\n", __func__);
	}
	if(is_suuported_tx_ofdm(padapter->registrypriv.wireless_mode)) {
		unsigned char	datarate_g[G_MODE_RATE_NUM] ={_6M_RATE_, _9M_RATE_, _12M_RATE_, _18M_RATE_,_24M_RATE_, _36M_RATE_, _48M_RATE_, _54M_RATE_};
		unsigned char	basicrate_g[G_MODE_BASIC_RATE_NUM] = {_6M_RATE_, _12M_RATE_, _24M_RATE_};
		_rtw_memcpy(pmlmeext->datarate + offset_datarate, datarate_g, G_MODE_RATE_NUM);
		_rtw_memcpy(pmlmeext->basicrate + offset_basicrate,basicrate_g, G_MODE_BASIC_RATE_NUM);
		offset_datarate += G_MODE_RATE_NUM;
		offset_basicrate += G_MODE_BASIC_RATE_NUM;
		RTW_INFO("%s: support OFDM\n", __func__);

	}
	_rtw_memcpy(pmlmeext->datarate + offset_datarate, end_set, 1);
	_rtw_memcpy(pmlmeext->basicrate + offset_basicrate, end_set, 1);

#ifdef CONFIG_80211N_HT
	if (padapter->registrypriv.ht_enable && is_supported_ht(padapter->registrypriv.wireless_mode))
		_rtw_memcpy(pmlmeext->default_supported_mcs_set, supported_mcs_set, sizeof(pmlmeext->default_supported_mcs_set));
#endif
}

int	rtw_init_link_mlme_ext_priv(struct _ADAPTER_LINK *padapter_link)
{
	_adapter *padapter = padapter_link->adapter;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct link_mlme_priv *pmlmepriv = &padapter_link->mlmepriv;
	int	res = _SUCCESS;

#ifdef CONFIG_AP_MODE /* moved from init_mlme_ap_info() in rtw_init_mlme_ext_priv() */
	_rtw_spinlock_init(&pmlmepriv->bcn_update_lock);
#endif

	/* moved from init_mlme_ext_priv_value() */
	pmlmeext->chandef.band = pregistrypriv->band;
	pmlmeext->chandef.chan = pregistrypriv->channel;
	pmlmeext->chandef.bw = CHANNEL_WIDTH_20;
	pmlmeext->chandef.offset = CHAN_OFFSET_NO_EXT;

	pmlmeext->cur_wireless_mode = pregistrypriv->wireless_mode;
	init_link_mlme_default_rate_set(padapter_link);

	if ((pmlmeext->chandef.chan > 14) || ((padapter->registrypriv.wireless_mode & WLAN_MD_11B) == 0))
		pmlmeext->tx_rate = IEEE80211_OFDM_RATE_6MB;
	else
		pmlmeext->tx_rate = IEEE80211_CCK_RATE_1MB;

	pmlmeinfo->slotTime = SHORT_SLOT_TIME;
	pmlmeinfo->preamble_mode = PREAMBLE_AUTO;

#ifdef PRIVATE_R
/*infra mode, used to store AP's info*/
	_rtw_memset(pmlmeinfo->SupportedRates_infra_ap, 0, NDIS_802_11_LENGTH_RATES_EX);
	pmlmeinfo->ht_vht_received = 0;
#endif /* PRIVATE_R */

	return res;
}

int rtw_match_self_addr(_adapter *padapter, const u8 *mac_addr)
{
	struct _ADAPTER_LINK *padapter_link = NULL;
	u8 lidx;
	/* ToDo CONFIG_RTW_MLD: TBD To see if MAC hdr conversion is enabled */
	/*if (_rtw_memcmp(mac_addr, adapter_mac_addr(padapter), ETH_ALEN))
		return _TRUE;
	*/
	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
		if (_rtw_memcmp(mac_addr, padapter_link->mac_addr, ETH_ALEN))
			return _TRUE;
	}
	return _FALSE;
}

int rtw_match_bssid_ext(_adapter *padapter, const u8 *mac_addr) /* mlmext_info->network */
{
	struct _ADAPTER_LINK *padapter_link = NULL;
	u8 lidx;
	/* ToDo CONFIG_RTW_MLD: TBD To see if MAC hdr conversion is enabled */
	/*if (_rtw_memcmp(mac_addr, get_my_bssid(&padapter->mlmeextpriv.mlmext_info.dev_network), ETH_ALEN))
		return _TRUE;*/
	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
#ifdef CONFIG_STA_MULTIPLE_BSSID
		if (_rtw_memcmp(mac_addr, get_my_bssid(&padapter_link->mlmeextpriv.mlmext_info.network), ETH_ALEN - 1)) {
			if (padapter_link->mlmeextpriv.mlmext_info.network.is_mbssid)
				return _TRUE;
		}
#endif
		if (_rtw_memcmp(mac_addr, get_my_bssid(&padapter_link->mlmeextpriv.mlmext_info.network), ETH_ALEN))
			return _TRUE;

	}
	return _FALSE;
}

int rtw_match_bssid(_adapter *padapter, const u8 *mac_addr) /* mlmepriv->cur_network.network */
{
	struct _ADAPTER_LINK *padapter_link = NULL;
	u8 lidx;
	/* ToDo CONFIG_RTW_MLD: TBD To see if MAC hdr conversion is enabled */
	/*if (_rtw_memcmp(mac_addr, get_bssid(&padapter->mlmepriv), ETH_ALEN))
		return _TRUE;*/
	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
		/* ToDo CONFIG_RTW_MLD: check wrlink->mstate linked or not */
		if (_rtw_memcmp(mac_addr, get_link_bssid(&padapter_link->mlmepriv), ETH_ALEN))
			return _TRUE;
	}
	return _FALSE;
}

#ifdef CONFIG_PATCH_JOIN_WRONG_CHANNEL
static u8 cmp_pkt_chnl_diff(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 *pframe, uint packet_len)
{
	/* if the channel is same, return 0. else return channel differential	 */
	uint len;
	u8 channel;
	u8 *p;

	p = rtw_get_ie(pframe + WLAN_HDR_A3_LEN + _BEACON_IE_OFFSET_, _DSSET_IE_, &len, packet_len - _BEACON_IE_OFFSET_);
	if (p) {
		channel = *(p + 2);
		if (padapter_link->mlmeextpriv.chandef.chan >= channel)
			return padapter_link->mlmeextpriv.chandef.chan - channel;
		else
			return channel - padapter_link->mlmeextpriv.chandef.chan;
	} else
		return 0;
}
#endif /* CONFIG_PATCH_JOIN_WRONG_CHANNEL */

static void _mgt_dispatcher(_adapter *padapter, struct mlme_handler *ptable, union recv_frame *precv_frame)
{
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 *pframe = precv_frame->u.hdr.rx_data;

	if (ptable->func) {
		/* receive the frames that ra(a1) is my address or ra(a1) is bc address. */
		if (!rtw_match_self_addr(padapter, GetAddr1Ptr(pframe)) &&
			!_rtw_memcmp(GetAddr1Ptr(pframe), bc_addr, ETH_ALEN))
#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
		{
			struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);

			if (!MLME_IS_STA(padapter))
				return;

			if (check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE) == _TRUE)
				return;

			if (pwdev_priv->random_mac_enabled == false)
				return;

			if (!_rtw_memcmp(GetAddr1Ptr(pframe), adapter_pno_mac_addr(padapter), ETH_ALEN))
				return;
		}
#else
			return;
#endif
		ptable->func(padapter, precv_frame);
	}
}

void mgt_dispatcher(_adapter *padapter, union recv_frame *precv_frame)
{
	int index;
	struct mlme_handler *ptable;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 *pframe = precv_frame->u.hdr.rx_data;
	struct sta_info *psta = rtw_get_stainfo(&padapter->stapriv, get_addr2_ptr(pframe));

#if 0
	{
		u8 *pbuf;
		pbuf = GetAddr1Ptr(pframe);
		RTW_INFO("A1-%x:%x:%x:%x:%x:%x\n", *pbuf, *(pbuf + 1), *(pbuf + 2), *(pbuf + 3), *(pbuf + 4), *(pbuf + 5));
		pbuf = get_addr2_ptr(pframe);
		RTW_INFO("A2-%x:%x:%x:%x:%x:%x\n", *pbuf, *(pbuf + 1), *(pbuf + 2), *(pbuf + 3), *(pbuf + 4), *(pbuf + 5));
		pbuf = GetAddr3Ptr(pframe);
		RTW_INFO("A3-%x:%x:%x:%x:%x:%x\n", *pbuf, *(pbuf + 1), *(pbuf + 2), *(pbuf + 3), *(pbuf + 4), *(pbuf + 5));
	}
#endif

	if (GetFrameType(pframe) != WIFI_MGT_TYPE) {
		return;
	}

	/* receive the frames that ra(a1) is my address or ra(a1) is bc address. */
	if (!rtw_match_self_addr(padapter, GetAddr1Ptr(pframe)) &&
		!_rtw_memcmp(GetAddr1Ptr(pframe), bc_addr, ETH_ALEN))
#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
		{
			struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);

			if (!MLME_IS_STA(padapter))
				return;

			if (check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE) == _TRUE)
				return;

			if (pwdev_priv->random_mac_enabled == false)
				return;

			if (!_rtw_memcmp(GetAddr1Ptr(pframe), adapter_pno_mac_addr(padapter), ETH_ALEN))
				return;
		}
#else
		return;
#endif

	ptable = mlme_sta_tbl;

	index = get_frame_sub_type(pframe) >> 4;

#ifdef CONFIG_TDLS
	if ((index << 4) == WIFI_ACTION) {
		/* category==public (4), action==TDLS_DISCOVERY_RESPONSE */
		if (*(pframe + 24) == RTW_WLAN_CATEGORY_PUBLIC && *(pframe + 25) == TDLS_DISCOVERY_RESPONSE) {
			RTW_INFO("[TDLS] Recv %s from "MAC_FMT"\n", rtw_tdls_action_txt(TDLS_DISCOVERY_RESPONSE), MAC_ARG(get_addr2_ptr(pframe)));
			On_TDLS_Dis_Rsp(padapter, precv_frame);
		}
	}
#endif /* CONFIG_TDLS */

	if (index >= (sizeof(mlme_sta_tbl) / sizeof(struct mlme_handler))) {
		return;
	}
	ptable += index;

#if 1
	if (psta != NULL) {
		if (GetRetry(pframe)) {
			if (precv_frame->u.hdr.attrib.seq_num == psta->RxMgmtFrameSeqNum) {
				/* drop the duplicate management frame */
				padapter->recvinfo.dbg_rx_dup_mgt_frame_drop_count++;
				RTW_INFO("Drop duplicate management frame with seq_num = %d.\n", precv_frame->u.hdr.attrib.seq_num);
				return;
			}
		}
		psta->RxMgmtFrameSeqNum = precv_frame->u.hdr.attrib.seq_num;
	}
#else

	if (GetRetry(pframe)) {
		/* return; */
	}
#endif

#ifdef CONFIG_AP_MODE
	switch (get_frame_sub_type(pframe)) {
	case WIFI_AUTH:
		if (MLME_IS_AP(padapter) || MLME_IS_MESH(padapter))
			ptable->func = &OnAuth;
		else
			ptable->func = &OnAuthClient;
		fallthrough;
	case WIFI_ASSOCREQ:
	case WIFI_REASSOCREQ:
		_mgt_dispatcher(padapter, ptable, precv_frame);
		#ifdef CONFIG_HOSTAPD_MLME
		if (MLME_IS_AP(padapter))
			rtw_hostapd_mlme_rx(padapter, precv_frame);
		#endif
		break;
	case WIFI_PROBEREQ:
		_mgt_dispatcher(padapter, ptable, precv_frame);
		#ifdef CONFIG_HOSTAPD_MLME
		if (MLME_IS_AP(padapter))
			rtw_hostapd_mlme_rx(padapter, precv_frame);
		#endif
		break;
	case WIFI_BEACON:
		_mgt_dispatcher(padapter, ptable, precv_frame);
		break;
	case WIFI_ACTION:
		_mgt_dispatcher(padapter, ptable, precv_frame);
		break;
	default:
		_mgt_dispatcher(padapter, ptable, precv_frame);
		#ifdef CONFIG_HOSTAPD_MLME
		if (MLME_IS_AP(padapter))
			rtw_hostapd_mlme_rx(padapter, precv_frame);
		#endif
		break;
	}
#else

	_mgt_dispatcher(padapter, ptable, precv_frame);

#endif

}

#ifdef CONFIG_P2P
u32 p2p_listen_state_process(_adapter *padapter, unsigned char *da)
{
	bool response = _TRUE;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);


#ifdef CONFIG_IOCTL_CFG80211
	if (rtw_cfg80211_get_is_roch(padapter) == _FALSE
		|| rtw_get_oper_ch(padapter, padapter_link) != padapter->wdinfo.listen_channel
		|| adapter_wdev_data(padapter)->p2p_enabled == _FALSE
		|| padapter->mlmepriv.wps_probe_resp_ie == NULL
		|| padapter->mlmepriv.p2p_probe_resp_ie == NULL
	) {
#ifdef CONFIG_DEBUG_CFG80211
		RTW_INFO(ADPT_FMT" DON'T issue_probersp_p2p: p2p_enabled:%d, wps_probe_resp_ie:%p, p2p_probe_resp_ie:%p\n"
			, ADPT_ARG(padapter)
			, adapter_wdev_data(padapter)->p2p_enabled
			, padapter->mlmepriv.wps_probe_resp_ie
			, padapter->mlmepriv.p2p_probe_resp_ie);
		RTW_INFO(ADPT_FMT" DON'T issue_probersp_p2p: is_ro_ch:%d, op_ch:%d, p2p_listen_channel:%d\n"
			, ADPT_ARG(padapter)
			, rtw_cfg80211_get_is_roch(padapter)
			, rtw_get_oper_ch(padapter, padapter_link)
			, padapter->wdinfo.listen_channel);
#endif
		response = _FALSE;
	}
#endif /* CONFIG_IOCTL_CFG80211 */

	if (response == _TRUE)
		issue_probersp_p2p(padapter, da);

	return _SUCCESS;
}
#endif /* CONFIG_P2P */

#ifdef PRIVATE_R
static bool check_vendor_ie(_adapter *padapter, u8 *ies, uint ies_len)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	unsigned int ielen;
	u8 *p;
	bool ret = _FALSE;
	unsigned int search_len = 0, remain_len = 0, total_vendor_ie_len = 0;
	int i;

	p = rtw_get_ie(ies, _VENDOR_SPECIFIC_IE_, (int *)&ielen, ies_len);

	while (p) {
		for (i = 0; i < MAX_VENDOR_IE_NUM; i++) {
			if (ielen == pmlmepriv->vendor_ie_len[i] &&
			    _rtw_memcmp(p, *(pmlmepriv->vendor_ie_filter + i), ielen + 2)) {
				ret = _TRUE;
				break;
			}
		}
		total_vendor_ie_len = ielen + 2;
		search_len = p - ies;
		remain_len = ies_len - search_len - total_vendor_ie_len;

		p = rtw_get_ie(p + total_vendor_ie_len, _VENDOR_SPECIFIC_IE_,
			       (int *)&ielen, remain_len);
	}
	return ret;
}
#ifdef CONFIG_P2P
static bool is_valid_probereq(_adapter *padapter, u8 *pframe, uint len)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX *cur = &(pmlmeinfo->dev_network);
	struct registry_priv *pregpriv = &padapter->registrypriv;
	bool ret = _FALSE;
	unsigned int ielen;
	u8 *p;

	p = rtw_get_ie(pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_, _SSID_IE_, (int *)&ielen,
		       len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_);

	if (p) {
		if ((_rtw_memcmp((void *)(p + 2), "DIRECT-", P2P_WILDCARD_SSID_LEN) &&
		     (ielen == P2P_WILDCARD_SSID_LEN ||
		      (ielen == cur->Ssid.SsidLength &&
		       _rtw_memcmp((void *)(p + 2), (void *)cur->Ssid.Ssid, cur->Ssid.SsidLength)))) ||
		    (ielen == 0 && pregpriv->go_hidden_ssid_mode != ALL_HIDE_SSID))
			ret = _TRUE;
	}
	return ret;
}

static void hide_ssid_in_beacon(_adapter *padapter, u8 *pframe, uint len)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct registry_priv *pregpriv = &padapter->registrypriv;
	u8 *wfd_ie;
	uint wfd_ielen;

	/* check if need to unhide/hide SSID in beacon for miracast */
	wfd_ie = rtw_get_wfd_ie(pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_,
				len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_,
				NULL, &wfd_ielen);
	if (wfd_ie) {
		if (pregpriv->go_hidden_ssid_mode & BIT(MIRACAST)) {
			rtw_clear_bit(MIRACAST, &pregpriv->go_hidden_ssid_mode);

			/* check if there is other feature show SSID in beacon */
			if (pregpriv->go_hidden_ssid_mode == MIRACAST_UNHIDE_SSID)
				issue_beacon(padapter, 0);

			ATOMIC_SET(&pregpriv->set_hide_ssid_timer, 1);
			_set_timer(&pmlmeext->hide_ssid_timer, 25000);
		} else {
			if (ATOMIC_READ(&pregpriv->set_hide_ssid_timer))
				_set_timer(&pmlmeext->hide_ssid_timer, 25000);
		}
	}
}
#endif
#endif

/****************************************************************************

Following are the callback functions for each subtype of the management frames

*****************************************************************************/

unsigned int OnProbeReq(_adapter *padapter, union recv_frame *precv_frame)
{
	unsigned int	ielen;
	unsigned char	*p;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX	*cur = &(pmlmeinfo->dev_network);
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint len = precv_frame->u.hdr.len;
	u8 is_valid_p2p_probereq = _FALSE;

#ifdef CONFIG_ATMEL_RC_PATCH
	u8 *target_ie = NULL, *wps_ie = NULL;
	u8 *start;
	uint search_len = 0, wps_ielen = 0, target_ielen = 0;
	struct sta_info	*psta;
	struct sta_priv *pstapriv = &padapter->stapriv;
#endif

#ifdef CONFIG_P2P
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	struct rx_pkt_attrib	*pattrib = &precv_frame->u.hdr.attrib;
	u8 wifi_test_chk_rate = 1;
#endif
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;
	void *phl = GET_PHL_INFO(adapter_to_dvobj(padapter));
	struct rtw_phl_mld_t *mld = NULL;
	u16 main_id = rtw_phl_get_macid_max_num(phl);
#ifdef CONFIG_80211BE_EHT
	u8 eid_eht_ml = EID_EXT_MULTI_LINK;
	struct rtw_phl_ml_element ml_ele = {0};
#endif

#ifdef CONFIG_80211BE_EHT
	/* Send probe response by driver to append ML IEs if support MLO */
	if (pmlmepriv->mlopriv.mlo_option)
		goto _skip_cfg80211_rx;
#endif

#ifdef CONFIG_IOCTL_CFG80211
#ifdef CONFIG_CFG80211_REPORT_PROBE_REQ
	if (GET_CFG80211_REPORT_MGMT(adapter_wdev_data(padapter),
		IEEE80211_STYPE_PROBE_REQ) == _TRUE) {

		#ifdef CONFIG_CONCURRENT_MODE
		if (MLME_IS_AP(padapter) &&
		    rtw_mi_buddy_check_fwstate(padapter, WIFI_UNDER_LINKING | WIFI_UNDER_SURVEY)) {
			#ifdef CONFIG_DEBUG_CFG80211
			RTW_INFO("%s : skip probe requst when buddy is linking or scanning\n", __func__);
			#endif
			/* don't report probe request */
			return _SUCCESS;
		}
		#endif

#ifdef PRIVATE_R
		if (pmlmepriv->vendor_ie_filter_enable) {
			/* check vendor ie for softAP and GO */
			if (check_fwstate(pmlmepriv, WIFI_AP_STATE) &&
				!check_vendor_ie(padapter, pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_,
				len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_))
				return _FAIL;
		}
#ifdef CONFIG_P2P
		if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
			if (!is_valid_probereq(padapter, pframe, len))
				return _FAIL;
			hide_ssid_in_beacon(padapter, pframe, len);
		}
#endif
#endif
		rtw_cfg80211_rx_probe_request(padapter, precv_frame);
		return _SUCCESS;
	} else {
		return _FAIL;
	}
#endif

#if !defined(RTW_WKARD_DIS_PROBE_REQ_RPT_TO_HOSTAPD)
	if (MLME_IS_AP(padapter) &&
		(GET_CFG80211_REPORT_MGMT(adapter_wdev_data(padapter), IEEE80211_STYPE_PROBE_REQ) == _TRUE)) {
		rtw_cfg80211_rx_probe_request(padapter, precv_frame);
		return _SUCCESS;
	}
#endif

#ifdef CONFIG_P2P
	if ((adapter_to_dvobj(padapter)->wpas_type == RTW_WPAS_W1FI) &&
		!rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DISABLE) &&
		(GET_CFG80211_REPORT_MGMT(adapter_wdev_data(padapter),
				IEEE80211_STYPE_PROBE_REQ) == _TRUE)) {
		rtw_cfg80211_rx_probe_request(padapter, precv_frame);
		return _SUCCESS;
	}
#endif
#endif /* CONFIG_IOCTL_CFG80211 */

_skip_cfg80211_rx:

#ifdef CONFIG_P2P
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DEVICE) ||
	    rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
		/*	Commented by Albert 2011/03/17 */
		/*	mcs_rate = 0->CCK 1M rate */
		/*	mcs_rate = 1->CCK 2M rate */
		/*	mcs_rate = 2->CCK 5.5M rate */
		/*	mcs_rate = 3->CCK 11M rate */
		/*	In the P2P mode, the driver should not support the CCK rate */

		/*	Commented by Kurt 2012/10/16 */
		/*	IOT issue: Google Nexus7 use 1M rate to send p2p_probe_req after GO nego completed and Nexus7 is client */
		if (padapter->registrypriv.wifi_spec == 1) {
			if (pattrib->data_rate <= DESC_RATE11M)
				wifi_test_chk_rate = 0;
		}

		if (wifi_test_chk_rate == 1) {
			is_valid_p2p_probereq = process_probe_req_p2p_ie(pwdinfo, pframe, len);
			if (is_valid_p2p_probereq == _TRUE) {
				if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_DEVICE)) {
					/* P2P Device only responds probe response in listen state(ROCH) */
					if (MLME_IS_ROCH(padapter))
						p2p_listen_state_process(padapter,  get_sa(pframe));
					return _SUCCESS;
				}

				if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO))
					goto _continue;
			}
		}
	}

_continue:
#endif /* CONFIG_P2P */

	if (MLME_IS_STA(padapter))
		return _SUCCESS;

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _FALSE &&
	    check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE | WIFI_AP_STATE | WIFI_MESH_STATE) == _FALSE)
		return _SUCCESS;


	/* RTW_INFO("+OnProbeReq\n"); */


#ifdef CONFIG_ATMEL_RC_PATCH
	wps_ie = rtw_get_wps_ie(
			      pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_,
			      len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_,
			      NULL, &wps_ielen);
	if (wps_ie)
		target_ie = rtw_get_wps_attr_content(wps_ie, wps_ielen, WPS_ATTR_MANUFACTURER, NULL, &target_ielen);
	if ((target_ie && (target_ielen == 4)) && (_TRUE == _rtw_memcmp((void *)target_ie, "Ozmo", 4))) {
		/* psta->flag_atmel_rc = 1; */
		unsigned char *sa_addr = get_sa(pframe);
		printk("%s: Find Ozmo RC -- %02x:%02x:%02x:%02x:%02x:%02x  \n\n",
		       __func__, *sa_addr, *(sa_addr + 1), *(sa_addr + 2), *(sa_addr + 3), *(sa_addr + 4), *(sa_addr + 5));
		_rtw_memcpy(pstapriv->atmel_rc_pattern, get_sa(pframe), ETH_ALEN);
	}
#endif


#ifdef CONFIG_AUTO_AP_MODE
	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE &&
	    pmlmepriv->dev_cur_network.join_res == _TRUE) {
		struct sta_info	*psta;
		u8 *mac_addr, *peer_addr;
		struct sta_priv *pstapriv = &padapter->stapriv;
		u8 RC_OUI[4] = {0x00, 0xE0, 0x4C, 0x0A};
		/* EID[1] + EID_LEN[1] + RC_OUI[4] + MAC[6] + PairingID[2] + ChannelNum[2] */

		p = rtw_get_ie(pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_, _VENDOR_SPECIFIC_IE_, (int *)&ielen,
			       len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_);

		if (!p || ielen != 14)
			goto _non_rc_device;

		if (!_rtw_memcmp(p + 2, RC_OUI, sizeof(RC_OUI)))
			goto _non_rc_device;

		if (!_rtw_memcmp(p + 6, get_sa(pframe), ETH_ALEN)) {
			RTW_INFO("%s, do rc pairing ("MAC_FMT"), but mac addr mismatch!("MAC_FMT")\n", __FUNCTION__,
				 MAC_ARG(get_sa(pframe)), MAC_ARG(p + 6));

			goto _non_rc_device;
		}

		RTW_INFO("%s, got the pairing device("MAC_FMT")\n", __FUNCTION__,  MAC_ARG(get_sa(pframe)));

		/* new a station */
		psta = rtw_get_stainfo(pstapriv, get_sa(pframe));
		if (psta == NULL) {
			/* allocate a new one */
			RTW_INFO("going to alloc stainfo for rc="MAC_FMT"\n",  MAC_ARG(get_sa(pframe)));
			mld = rtw_phl_alloc_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)), padapter->phl_role, get_sa(pframe), DTYPE);
			if (mld == NULL) {
				/* TODO: */
				RTW_INFO(" Exceed the upper limit of supported MLDs...\n");
				return _SUCCESS;
			}
			/* primary link only */
			psta = rtw_alloc_stainfo_sw(pstapriv, DTYPE, main_id, padapter_link->wrlink->id, get_sa(pframe));
			if (psta == NULL) {
				/* TODO: */
				RTW_INFO(" Exceed the upper limit of supported clients...\n");
				rtw_free_mld_stainfo(padapter), mld);
				return _SUCCESS;
			}
			rtw_phl_link_mld_stainfo(mld, psta->phl_sta);

			_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
			if (rtw_is_list_empty(&psta->asoc_list)) {
				psta->expire_to = pstapriv->expire_to;
				rtw_list_insert_tail(&psta->asoc_list, &pstapriv->asoc_list);
				pstapriv->asoc_list_cnt++;
				#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
				if (psta->tbtx_enable)
					pstapriv->tbtx_asoc_list_cnt++;
				#endif
			}
			_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

			/* generate pairing ID */
			mac_addr = adapter_mac_addr(padapter);
			peer_addr = psta->phl_sta->mac_addr;
			psta->pid = (u16)(((mac_addr[4] << 8) + mac_addr[5]) + ((peer_addr[4] << 8) + peer_addr[5]));

			/* update peer stainfo */
			psta->isrc = _TRUE;

			/* AID assignment */
			if (psta->phl_sta->aid > 0)
				RTW_INFO(FUNC_ADPT_FMT" old AID=%d\n", FUNC_ADPT_ARG(padapter), psta->phl_sta->aid);
			else {
				if (!rtw_aid_alloc(padapter, psta)) {
					RTW_INFO(FUNC_ADPT_FMT" no room for more AIDs\n", FUNC_ADPT_ARG(padapter));
					return _SUCCESS;
				}
				RTW_INFO(FUNC_ADPT_FMT" allocate new AID=%d\n", FUNC_ADPT_ARG(padapter), psta->phl_sta->aid);
			}

			psta->qos_option = 1;
			psta->phl_sta->chandef.bw = CHANNEL_WIDTH_20;
			psta->ieee8021x_blocked = _FALSE;
#ifdef CONFIG_80211N_HT
			if(padapter->registrypriv.ht_enable &&
				is_supported_ht(padapter->registrypriv.wireless_mode))
			{
				psta->htpriv.ht_option = _TRUE;
				psta->ampdu_priv.ampdu_enable = _FALSE;
				psta->htpriv.sgi_20m = _FALSE;
				psta->htpriv.sgi_40m = _FALSE;
				psta->htpriv.ch_offset = CHAN_OFFSET_NO_EXT;
				psta->ampdu_priv.agg_enable_bitmap = 0x0;/* reset */
				psta->ampdu_priv.candidate_tid_bitmap = 0x0;/* reset */
			}
#endif

			_rtw_memset((void *)&psta->sta_stats, 0, sizeof(struct stainfo_stats));

			_rtw_spinlock_bh(&psta->lock);
			psta->state |= WIFI_ASOC_STATE;
			_rtw_spinunlock_bh(&psta->lock);

			report_add_sta_event(padapter, psta->phl_sta->mac_addr);

		}

		issue_probersp(padapter, padapter_link, get_sa(pframe), _FALSE);

		return _SUCCESS;

	}

_non_rc_device:

	return _SUCCESS;

#endif /* CONFIG_AUTO_AP_MODE */


#ifdef CONFIG_CONCURRENT_MODE
	if (((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) &&
	    rtw_mi_buddy_check_fwstate(padapter, WIFI_UNDER_LINKING | WIFI_UNDER_SURVEY)) {
		/* don't process probe req */
		return _SUCCESS;
	}
#endif

#ifdef PRIVATE_R
	if (pmlmepriv->vendor_ie_filter_enable) {
		/* check vendor ie for softAP and GO */
		if (check_fwstate(pmlmepriv, WIFI_AP_STATE) &&
			!check_vendor_ie(padapter, pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_,
			len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_))
			goto exit;
	}
#ifdef CONFIG_P2P
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
		if (is_valid_probereq(padapter, pframe, len)) {
			hide_ssid_in_beacon(padapter, pframe, len);
			goto _issue_probersp;
		} else {
			goto exit;
		}
	}
#endif
#endif

	p = rtw_get_ie(pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_, _SSID_IE_, (int *)&ielen,
		       len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_);
	if (!p)
		goto exit;

	if (is_valid_p2p_probereq == _TRUE)
		goto _issue_probersp;

	if ((ielen != 0 &&
	     !_rtw_memcmp((void *)(p + 2), (void *)cur->Ssid.Ssid, cur->Ssid.SsidLength)) ||
	    (ielen == 0 && pmlmeinfo->hidden_ssid_mode))
		goto exit;

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter)) {
		p = rtw_get_ie(pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_, WLAN_EID_MESH_ID, (int *)&ielen,
				len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_);

		if (!p)
			goto exit;
		if (ielen != 0 && _rtw_memcmp((void *)(p + 2), (void *)cur->mesh_id.Ssid, cur->mesh_id.SsidLength) == _FALSE)
			goto exit;
	}
#endif

_issue_probersp:
	if ((check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE &&
		padapter_link->mlmepriv.cur_network.join_res == _TRUE) ||
		check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE)) {
#ifdef CONFIG_80211BE_EHT
		if (pmlmepriv->mlopriv.mlo_option == _TRUE) {
			p = rtw_get_ie_ex(pframe + WLAN_HDR_A3_LEN + _PROBEREQ_IE_OFFSET_,
				len - WLAN_HDR_A3_LEN - _PROBEREQ_IE_OFFSET_,
				WLAN_EID_EXTENSION, &eid_eht_ml, 1, NULL, (int *)&ielen);

			if (p != NULL && ielen >= EHT_ML_PROBEREQ_MIN_LEN) {
				rtw_phl_parse_ml_ie(GET_PHL_COM(adapter_to_dvobj(padapter)), p + 3, ielen - 3, &ml_ele);
				if (ml_ele.type == PROBE_REQUEST_ML) {
					issue_ml_probersp(padapter, padapter_link, get_sa(pframe), is_valid_p2p_probereq);
					goto exit;
				}
			}
		}
#endif
		/* RTW_INFO("+issue_probersp during ap mode\n"); */
		issue_probersp(padapter, padapter_link, get_sa(pframe), is_valid_p2p_probereq);
	}

exit:
	return _SUCCESS;

}

unsigned int OnProbeRsp(_adapter *padapter, union recv_frame *precv_frame)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	u8	*pframe = precv_frame->u.hdr.rx_data;
#ifdef CONFIG_P2P
	struct wifidirect_info	*pwdinfo = &padapter->wdinfo;
#endif
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;

	if ((mlmeext_chk_scan_state(pmlmeext, SCAN_PROCESS))
		|| (MLME_IS_MESH(padapter) && check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE))
	) {
		struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
		struct link_mlme_ext_info *lmlmeinfo = &padapter_link->mlmeextpriv.mlmext_info;

		if (rtw_match_bssid_ext(padapter, GetAddr3Ptr(pframe))
			&& (pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE && (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS))
		{
			if (!rtw_check_bcn_info(padapter, padapter_link, pframe, precv_frame->u.hdr.len)) {
				RTW_PRINT(FUNC_ADPT_FMT" ap has changed, disconnect now\n", FUNC_ADPT_ARG(padapter));
				receive_disconnect(padapter, lmlmeinfo->network.MacAddress , 0, _FALSE);
				pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
				pmlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_AP_BEACON_CHANGED;
				pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
			}
		}

		rtw_mi_report_survey_event(padapter, precv_frame);
		return _SUCCESS;
	}

#if 0 /* move to validate_recv_mgnt_frame */
	if (_rtw_memcmp(GetAddr3Ptr(pframe), get_my_bssid(&pmlmeinfo->network), ETH_ALEN)) {
		if (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) {
			psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
			if (psta != NULL)
				psta->sta_stats.rx_mgnt_pkts++;
		}
	}
#endif

	return _SUCCESS;

}

/* for 11n Logo 4.2.31/4.2.32 */
static void rtw_check_legacy_ap(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 *pframe, u32 len)
{

	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_priv *pmlmepriv = &padapter_link->mlmepriv;

	if (!padapter->registrypriv.wifi_spec)
		return;

	if(!MLME_IS_AP(padapter))
		return;


	if (pmlmeext->bstart_bss == _TRUE) {
		int left;
		unsigned char *pos;
		struct rtw_ieee802_11_elems elems;
#ifdef CONFIG_80211N_HT
		u16 cur_op_mode; 
#endif
		/* checking IEs */
		left = len - sizeof(struct rtw_ieee80211_hdr_3addr) - _BEACON_IE_OFFSET_;
		pos = pframe + sizeof(struct rtw_ieee80211_hdr_3addr) + _BEACON_IE_OFFSET_;
		if (rtw_ieee802_11_parse_elems(pos, left, &elems, 1) == ParseFailed) {
			RTW_INFO("%s: parse fail for "MAC_FMT"\n", __func__, MAC_ARG(GetAddr3Ptr(pframe)));
			return;
		}
#ifdef CONFIG_80211N_HT
		cur_op_mode = pmlmepriv->ht_op_mode & HT_INFO_OPERATION_MODE_OP_MODE_MASK;
#endif
		/* for legacy ap */
		if (elems.ht_capabilities == NULL && elems.ht_capabilities_len == 0) {

			if (0)
				RTW_INFO("%s: "MAC_FMT" is legacy ap\n", __func__, MAC_ARG(GetAddr3Ptr(pframe)));

			ATOMIC_SET(&pmlmepriv->olbc, _TRUE);
			ATOMIC_SET(&pmlmepriv->olbc_ht, _TRUE);
		}
			
	}
}

unsigned int OnBeacon(_adapter *padapter, union recv_frame *precv_frame)
{
	struct sta_info	*psta;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct sta_priv	*pstapriv = &padapter->stapriv;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint len = precv_frame->u.hdr.len;
	WLAN_BSSID_EX *pbss;
	int ret = _SUCCESS;
#ifdef CONFIG_TDLS
	struct sta_info *ptdls_sta;
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
#ifdef CONFIG_TDLS_CH_SW
	struct tdls_ch_switch *pchsw_info = &padapter->tdlsinfo.chsw_info;
#endif
#endif /* CONFIG_TDLS */
#if defined(CONFIG_MCC_MODE) || defined(CONFIG_RTW_LPS)
	struct rtw_bcn_pkt_info bcn_info = {0};
#endif
	void *phl = GET_PHL_INFO(adapter_to_dvobj(padapter));
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;
	struct link_mlme_ext_info *lmlmeinfo = &padapter_link->mlmeextpriv.mlmext_info;
	struct rtw_phl_mld_t *mld = NULL;
	u16 main_id = rtw_phl_get_macid_max_num(phl);

	if (pframe == NULL) {
		RTW_ERR(FUNC_ADPT_FMT" pframe == NULL\n", FUNC_ADPT_ARG(padapter));
		rtw_warn_on(1);
		return _SUCCESS;
	}
	if (validate_beacon_len(pframe, len) == _FALSE) {
		return _SUCCESS;
	}

	if (mlmeext_chk_scan_state(pmlmeext, SCAN_PROCESS)
		|| (MLME_IS_MESH(padapter) && check_fwstate(pmlmepriv, WIFI_ASOC_STATE))
	) {
		if (_rtw_memcmp(GetAddr3Ptr(pframe), get_my_bssid(&lmlmeinfo->network), ETH_ALEN)
			&& (pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE && (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS)
		) {
#ifndef RTW_PHL_TEST_FPGA
			if (!rtw_check_bcn_info(padapter, padapter_link, pframe, len))
			{
				RTW_PRINT(FUNC_ADPT_FMT" ap has changed, disconnect now\n", FUNC_ADPT_ARG(padapter));
				receive_disconnect(padapter, lmlmeinfo->network.MacAddress , 0, _FALSE);
				pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
				pmlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_AP_BEACON_CHANGED;
				pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
			}
#endif
		}
		report_survey_event(padapter, precv_frame);
		return _SUCCESS;
	}

	rtw_check_legacy_ap(padapter, padapter_link, pframe, len);

	if (rtw_match_bssid_ext(padapter, GetAddr3Ptr(pframe)))
	{
		if ((pmlmeinfo->state & WIFI_FW_AUTH_NULL)
			&& (rtw_sta_linking_test_wait_done() || pmlmeext->join_abort)
		) {
			if (rtw_sta_linking_test_force_fail() || pmlmeext->join_abort) {
				set_link_timer(pmlmeext, 1);
				return _SUCCESS;
			}

			/* we should update current network before auth, or some IE is wrong */
			pbss = (WLAN_BSSID_EX *)rtw_malloc(sizeof(WLAN_BSSID_EX));
			if (pbss) {
				if (collect_bss_info(padapter, precv_frame, pbss) == _SUCCESS) {
					struct beacon_keys recv_beacon;

					rtw_update_adapter_network(&(pmlmepriv->dev_cur_network.network), pbss, padapter, _TRUE);
					/* Update cur_network of the primary link */
					rtw_update_network(&(padapter_link->mlmepriv.cur_network.network), pbss, padapter, _TRUE);
					/* update bcn keys */
#ifdef RTW_PHL_TEST_FPGA
					if (1)
#else
					if (rtw_get_bcn_keys(padapter, padapter_link, pframe, len, &recv_beacon) == _TRUE)
#endif
					{
						RTW_INFO("%s: beacon keys ready\n", __func__);
						_rtw_memcpy(&padapter_link->mlmepriv.cur_beacon_keys,
							&recv_beacon, sizeof(recv_beacon));
						if (is_hidden_ssid(recv_beacon.ssid, recv_beacon.ssid_len)) {
							_rtw_memcpy(padapter_link->mlmepriv.cur_beacon_keys.ssid, lmlmeinfo->network.Ssid.Ssid, IW_ESSID_MAX_SIZE);
							padapter_link->mlmepriv.cur_beacon_keys.ssid_len = lmlmeinfo->network.Ssid.SsidLength;
						}
					} else {
						RTW_ERR("%s: get beacon keys failed\n", __func__);
						_rtw_memset(&padapter_link->mlmepriv.cur_beacon_keys, 0, sizeof(recv_beacon));
					}
				}
				rtw_mfree((u8 *)pbss, sizeof(WLAN_BSSID_EX));
			}

			/* check the vendor of the assoc AP */
			pmlmeinfo->assoc_AP_vendor = check_assoc_AP(pframe + sizeof(struct rtw_ieee80211_hdr_3addr), len - sizeof(struct rtw_ieee80211_hdr_3addr));
			rtw_update_phl_iot(padapter, pmlmeinfo->assoc_AP_vendor);

			/* update TSF Value */
			update_TSF(&padapter_link->mlmeextpriv, pframe, len);
			padapter_link->mlmeextpriv.bcn_cnt = 0;
			padapter_link->mlmeextpriv.last_bcn_cnt = 0;

#ifdef CONFIG_P2P_PS
			/* Comment by YiWei , in wifi p2p spec the "3.3 P2P Power Management" , "These mechanisms are available in a P2P Group in which only P2P Devices are associated." */
			/* process_p2p_ps_ie(padapter, (pframe + WLAN_HDR_A3_LEN), (len - WLAN_HDR_A3_LEN)); */
#endif /* CONFIG_P2P_PS */

#if defined(CONFIG_P2P) && defined(CONFIG_CONCURRENT_MODE)
			if (padapter->registrypriv.wifi_spec) {
				if (process_p2p_cross_connect_ie(padapter, (pframe + WLAN_HDR_A3_LEN), (len - WLAN_HDR_A3_LEN)) == _FALSE) {
					if (rtw_mi_buddy_check_mlmeinfo_state(padapter, WIFI_FW_AP_STATE)) {
						RTW_PRINT("no issue auth, P2P cross-connect does not permit\n ");
						return _SUCCESS;
					}
				}
			}
#endif /* CONFIG_P2P CONFIG_P2P and CONFIG_CONCURRENT_MODE */

			/* start auth */
			start_clnt_auth(padapter);

			return _SUCCESS;
		}

		if (((pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE) && (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS)) {
			psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
			if (psta != NULL) {
#ifdef CONFIG_PATCH_JOIN_WRONG_CHANNEL
				/* Merge from 8712 FW code */
				if (cmp_pkt_chnl_diff(padapter, padapter_link, pframe, len) != 0) {
					/* join wrong channel, deauth and reconnect           */
					issue_deauth(padapter, (&(lmlmeinfo->network))->MacAddress, WLAN_REASON_DEAUTH_LEAVING);

					report_del_sta_event(padapter, (&(lmlmeinfo.network))->MacAddress, WLAN_REASON_JOIN_WRONG_CHANNEL, _TRUE, _FALSE);
					pmlmeinfo->state &= (~WIFI_FW_ASSOC_SUCCESS);
					pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
					pmlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_JOIN_WRONG_CHANNEL;
					pmlmeinfo->wifi_reason_code = WLAN_REASON_DEAUTH_LEAVING;
					return _SUCCESS;
				}
#endif /* CONFIG_PATCH_JOIN_WRONG_CHANNEL */
#ifdef CONFIG_RTW_80211R
				rtw_ft_update_bcn(padapter, precv_frame);
#endif
#ifdef RTW_PHL_TEST_FPGA
				ret = 1;
#else
				ret = rtw_check_bcn_info(padapter, padapter_link, pframe, len);
#endif
				if (!ret) {
					RTW_PRINT(FUNC_ADPT_FMT" ap has changed, disconnect now\n", FUNC_ADPT_ARG(padapter));
					receive_disconnect(padapter, lmlmeinfo->network.MacAddress , 0, _FALSE);
					pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
					pmlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_AP_BEACON_CHANGED;
					pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
					return _SUCCESS;
				}

				/* update WMM, ERP in the beacon */
				/* todo: the timer is used instead of the number of the beacon received */
				if ((sta_rx_pkts(psta) & 0xf) == 0) {
					/* RTW_INFO("update_bcn_info\n"); */
					update_beacon_info(padapter, pframe, len, psta);
				}
				if (pmlmepriv->cur_network_scanned)
					pmlmepriv->cur_network_scanned->network.PhyInfo.rssi = precv_frame->u.hdr.attrib.phy_info.recv_signal_power;
				padapter_link->mlmeextpriv.bcn_cnt++;
#ifdef CONFIG_BCN_RECV_TIME
				rtw_rx_bcn_time_update(padapter, padapter_link, len, precv_frame->u.hdr.attrib.data_rate);
#endif
#if defined(CONFIG_MCC_MODE) || defined(CONFIG_RTW_LPS)
				update_TSF(&padapter_link->mlmeextpriv, pframe, len);
				bcn_info.sta = psta->phl_sta;
				bcn_info.tsf = padapter_link->mlmeextpriv.TSFValue;
				rtw_phl_sta_up_rx_bcn(GET_PHL_INFO(adapter_to_dvobj(padapter)), &bcn_info);
#endif
#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
				if (rtw_tdls_is_chsw_allowed(padapter) == _TRUE) {
					/* Send TDLS Channel Switch Request when receiving Beacon */
					if ((padapter->tdlsinfo.chsw_info.ch_sw_state & TDLS_CH_SW_INITIATOR_STATE) && (ATOMIC_READ(&pchsw_info->chsw_on) == _TRUE)
					    && (padapter_link->mlmeextpriv.chandef.chan == rtw_get_oper_ch(padapter))) {
						ptdls_sta = rtw_get_stainfo(&padapter->stapriv, padapter->tdlsinfo.chsw_info.addr);
						if (ptdls_sta != NULL) {
							if (ptdls_sta->tdls_sta_state | TDLS_LINKED_STATE)
								_set_timer(&ptdls_sta->stay_on_base_chnl_timer, TDLS_CH_SW_STAY_ON_BASE_CHNL_TIMEOUT);
						}
					}
				}
#endif
#endif /* CONFIG_TDLS */

				#ifdef CONFIG_ECSA_PHL
				process_csa_ie(padapter
					, pframe + WLAN_HDR_A3_LEN + _BEACON_IE_OFFSET_
					, len - (WLAN_HDR_A3_LEN + _BEACON_IE_OFFSET_));
				#endif

				#ifdef CONFIG_80211D
				alink_process_country_ie(padapter_link
					, pframe + WLAN_HDR_A3_LEN + _BEACON_IE_OFFSET_
					, len - (WLAN_HDR_A3_LEN + _BEACON_IE_OFFSET_));
				#endif

#ifdef CONFIG_P2P_PS
				process_p2p_ps_ie(padapter, (pframe + WLAN_HDR_A3_LEN), (len - WLAN_HDR_A3_LEN));
#endif /* CONFIG_P2P_PS */

				if (ATOMIC_READ(&padapter->need_tsf_sync_done) == _TRUE) {
					rtw_phl_sta_tsf_sync_done(GET_PHL_INFO(adapter_to_dvobj(padapter)), psta->phl_sta);
					ATOMIC_SET(&padapter->need_tsf_sync_done, _FALSE);
				}
#if 0 /* move to validate_recv_mgnt_frame */
				psta->sta_stats.rx_mgnt_pkts++;
#endif
			}

		} else if ((pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) {
			u8 rate_set[16];
			u8 rate_num = 0;

			psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
			if (psta != NULL) {
				/*
				* update WMM, ERP in the beacon
				* todo: the timer is used instead of the number of the beacon received
				*/
				if ((sta_rx_pkts(psta) & 0xf) == 0)
					update_beacon_info(padapter, pframe, len, psta);
			} else {
				rtw_ies_get_supported_rate(pframe + WLAN_HDR_A3_LEN + _BEACON_IE_OFFSET_, len - WLAN_HDR_A3_LEN - _BEACON_IE_OFFSET_, rate_set, &rate_num);
				if (rate_num == 0) {
					RTW_INFO(FUNC_ADPT_FMT" RX beacon with no supported rate\n", FUNC_ADPT_ARG(padapter));
					goto _END_ONBEACON_;
				}
				/* ToDo CONFIG_RTW_MLD: ad-hoc */
				mld = rtw_phl_alloc_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)), padapter->phl_role, get_addr2_ptr(pframe), DTYPE);
				if (mld == NULL) {
					RTW_INFO(FUNC_ADPT_FMT" Exceed the upper limit of supported MLDs\n", FUNC_ADPT_ARG(padapter));
					goto _END_ONBEACON_;
				}
				psta = rtw_alloc_stainfo_sw(pstapriv, DTYPE, main_id, RTW_RLINK_PRIMARY, get_addr2_ptr(pframe));
				if (psta == NULL) {
					RTW_INFO(FUNC_ADPT_FMT" Exceed the upper limit of supported clients\n", FUNC_ADPT_ARG(padapter));
					rtw_free_mld_stainfo(padapter, mld);
					goto _END_ONBEACON_;
				}
				rtw_phl_link_mld_stainfo(mld, psta->phl_sta);

				psta->expire_to = pstapriv->adhoc_expire_to;

				_rtw_memcpy(psta->bssrateset, rate_set, rate_num);
				psta->bssratelen = rate_num;

				/* update TSF Value */
				update_TSF(&padapter_link->mlmeextpriv, pframe, len);

				/* report sta add event */
				report_add_sta_event(padapter, get_addr2_ptr(pframe));
			}
		}
	}

_END_ONBEACON_:

	return _SUCCESS;

}

static u32 rtw_get_sta_num_by_state(_adapter *padapter, u32 state)
{
	_list	*plist, *phead;
	u32	index, sta_num = 0;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &(padapter->stapriv);

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);	
	for (index = 0; index < NUM_STA; index++) {		
		phead = &(pstapriv->sta_hash[index]);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info , hash_list);
			if ((psta->state & (state)))
				sta_num++;
			plist = get_next(plist);
		}
	}
	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

	/* RTW_INFO("%s : waiting for %u sta under linking \n", __func__, sta_num); */
	return sta_num;
}

static u8  rtw_defs_attack_chk(_adapter *padapter)
{
	struct mlme_priv *mlme = &(padapter->mlmepriv);
	u8 is_reject = _FALSE;
	u32 sta_limit = 0;
	u32 stime = rtw_systime_to_ms(rtw_get_current_time());
	static u32 ptime = 0;

	/* RTW_INFO("%s : ptime=%u, stime=%u, diff=%u\n", __func__, ptime, stime, (stime - ptime)); */
	if ((ptime > 0) && ((stime - ptime) < mlme->defs_lmt_time)) {
		sta_limit = rtw_get_sta_num_by_state(padapter, WIFI_FW_LINKING_STATE);
		if (sta_limit >= mlme->defs_lmt_sta)
			is_reject = _TRUE;
	}

	ptime = stime;
	/* RTW_INFO("%s : current linking num=%u\n", __func__, sta_limit); */
	return is_reject;	
}

unsigned int OnAuth(_adapter *padapter, union recv_frame *precv_frame)
{
#ifdef CONFIG_AP_MODE
	unsigned int	auth_mode, seq, ie_len;
	unsigned char	*sa, *p;
	u16	algorithm;
	int	status;
	static struct sta_info stat;
	static struct rtw_phl_stainfo_t phl_stat;
	struct	sta_info	*pstat = NULL;
	struct	sta_priv *pstapriv = &padapter->stapriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint len = precv_frame->u.hdr.len;
	u8	offset = 0;
	void *phl = GET_PHL_INFO(adapter_to_dvobj(padapter));
	struct rtw_wifi_role_t *phl_role = padapter->phl_role;
	struct rtw_phl_mld_t *mld = NULL;
	u16 main_id = rtw_phl_get_macid_max_num(phl);
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;

#ifdef CONFIG_CONCURRENT_MODE
	if (((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) &&
	    rtw_mi_buddy_check_fwstate(padapter, WIFI_UNDER_LINKING | WIFI_UNDER_SURVEY)) {
		/* don't process auth request; */
		return _SUCCESS;
	}
#endif /* CONFIG_CONCURRENT_MODE */

	if ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)
		return _FAIL;

	if (!MLME_IS_ASOC(padapter))
		return _SUCCESS;

#if defined(CONFIG_IOCTL_CFG80211) && defined(CONFIG_RTW_MESH)
	if (MLME_IS_MESH(padapter))
		return rtw_mesh_on_auth(padapter, precv_frame);
#endif

	RTW_INFO("+OnAuth\n");

	sa = get_addr2_ptr(pframe);

	auth_mode = psecuritypriv->dot11AuthAlgrthm;

	if (GetPrivacy(pframe)) {
		u8	*iv;
		struct rx_pkt_attrib	*prxattrib = &(precv_frame->u.hdr.attrib);

		prxattrib->hdrlen = WLAN_HDR_A3_LEN;
		prxattrib->encrypt = _WEP40_;

		iv = pframe + prxattrib->hdrlen;
		prxattrib->key_index = ((iv[3] >> 6) & 0x3);

		prxattrib->iv_len = 4;
		prxattrib->icv_len = 4;

		rtw_wep_decrypt(padapter, (u8 *)precv_frame);

		offset = 4;
	}

	algorithm = le16_to_cpu(*(u16 *)((SIZE_PTR)pframe + WLAN_HDR_A3_LEN + offset));
	seq	= le16_to_cpu(*(u16 *)((SIZE_PTR)pframe + WLAN_HDR_A3_LEN + offset + 2));

	RTW_INFO("auth alg=%x, seq=%X\n", algorithm, seq);

	if (rtw_check_invalid_mac_address(sa, _FALSE)){
		RTW_INFO("%s : reject invalid AUTH-req "MAC_FMT"\n", 
			__func__, MAC_ARG(get_addr2_ptr(pframe)));
		return _FAIL;
	}

	if(rtw_defs_attack_chk(padapter))  {
		struct sta_info *_psta;
		_psta = rtw_get_stainfo(pstapriv, sa);
		if ((_psta == NULL) || !(_psta->state & WIFI_FW_ASSOC_SUCCESS)) {
			status = _STATS_REFUSED_TEMPORARILY_;
			RTW_ERR("%s : refused temporarily for sa "MAC_FMT" !\n", __func__, MAC_ARG(sa));
			goto auth_fail;
		}
	}

	if (rtw_ap_linking_test_force_auth_fail()) {
		status = rtw_ap_linking_test_force_auth_fail();
		RTW_INFO(FUNC_ADPT_FMT" force auth fail with status:%u\n"
			, FUNC_ADPT_ARG(padapter), status);
		goto auth_fail;
	}

	if ((auth_mode == 2) && (algorithm != WLAN_AUTH_SAE) &&
	    (psecuritypriv->dot11PrivacyAlgrthm != _WEP40_) &&
	    (psecuritypriv->dot11PrivacyAlgrthm != _WEP104_))
		auth_mode = 0;

	if (((algorithm > 0 && auth_mode == 0) ||	/* rx a shared-key auth but shared not enabled */
	    (algorithm == 0 && auth_mode == 1))
	#ifdef  CONFIG_RTW_80211R_AP
		&& (algorithm != WLAN_AUTH_FT)
	#endif
	    ) {	/* rx a open-system auth but shared-key is enabled */
		RTW_INFO("auth rejected due to bad alg [alg=%d, auth_mib=%d] %02X%02X%02X%02X%02X%02X\n",
			algorithm, auth_mode, sa[0], sa[1], sa[2], sa[3], sa[4], sa[5]);

		status = _STATS_NO_SUPP_ALG_;

		goto auth_fail;
	}

#if CONFIG_RTW_MACADDR_ACL
	if (rtw_access_ctrl(padapter, sa) == _FALSE) {
		status = _STATS_UNABLE_HANDLE_STA_;
		goto auth_fail;
	}
#endif

	pstat = rtw_get_stainfo(pstapriv, sa);
	if (pstat == NULL) {

		mld = rtw_phl_get_mld_by_addr(phl, phl_role, sa);
		if (mld != NULL) {
			RTW_INFO("%s mld is not empty, sta is under disconnecting, so reject auth for sa "MAC_FMT"",
				__func__, MAC_ARG(sa));
			return _FAIL;
		}

		/* allocate a new one */
		RTW_INFO("going to alloc stainfo for sa="MAC_FMT"\n",  MAC_ARG(sa));

		/* ToDo CONFIG_RTW_MLD: replace sa with MLD address */
		mld = rtw_phl_alloc_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)), padapter->phl_role, sa, DTYPE);
		if (mld == NULL) {
			RTW_INFO(" Exceed the upper limit of supported MLDs...\n");
			status = _STATS_UNABLE_HANDLE_STA_;
			goto auth_fail;
		}
		/* Allocate stainfo on the link where AP receives the auth req */
		pstat = rtw_alloc_stainfo_sw(pstapriv, DTYPE, main_id, padapter_link->wrlink->id, sa);
		if (pstat == NULL) {
			RTW_INFO(" Exceed the upper limit of supported clients...\n");
			rtw_free_mld_stainfo(padapter, mld);
			status = _STATS_UNABLE_HANDLE_STA_;
			goto auth_fail;
		}
		rtw_phl_link_mld_stainfo(mld, pstat->phl_sta);
		pstat->state = WIFI_FW_AUTH_NULL;
		pstat->auth_seq = 0;
		pstat->first_auth_time = rtw_systime_to_ms(rtw_get_current_time());
		/* pstat->flags = 0; */
		/* pstat->capability = 0; */
	} else {
		#if 0
		if (pstat->state & WIFI_FW_LINKING_STATE) { /* sta is under linking */
			u32 auth_t = rtw_systime_to_ms(rtw_get_current_time());

			if ((auth_t - pstat->first_auth_time) < STA_AUTH_TO) {
				RTW_INFO("%s : Reject multiple auth for sa "MAC_FMT" when sta is under linking.\n",
					__func__, MAC_ARG(sa));
				return _FAIL;
			} else {
				pstat->state = WIFI_FW_AUTH_NULL;
				status = _STATS_AUTH_TIMEOUT_;

				RTW_INFO("%s : sa "MAC_FMT" connect %u ms, it's timeout, so free sta_info\n",
					__func__, MAC_ARG(sa), STA_AUTH_TO);
				goto auth_fail;
			}
		} else if (ATOMIC_READ(&pstat->deleting) != 0) { /* sta is under disconnecting */
			status = _STATS_REFUSED_TEMPORARILY_;
			RTW_ERR("%s : Reject auth refused temporarily for sa "MAC_FMT", sta is deleting=%d\n",
				__func__, MAC_ARG(sa), ATOMIC_READ(&pstat->deleting));
			return _FAIL;
		}

		RTW_INFO("STA is still connecting, disconnect it at first.\n");
		rtw_cfg80211_indicate_sta_disassoc(padapter, sa, WLAN_REASON_DISASSOC_STA_HAS_LEFT);
		#endif

		if (ATOMIC_READ(&pstat->deleting) != 0) { /* sta is under disconnecting */
			status = _STATS_REFUSED_TEMPORARILY_;
			RTW_ERR("%s : Reject auth refused temporarily for sa "MAC_FMT", sta is deleting=%d\n",
				__func__, MAC_ARG(sa), ATOMIC_READ(&pstat->deleting));
			return _FAIL;
		}

#ifdef CONFIG_IEEE80211W
		if ((pstat->bpairwise_key_installed != _TRUE && (pstat->flags & WLAN_STA_MFP))
			|| !(pstat->flags & WLAN_STA_MFP))
#endif /* CONFIG_IEEE80211W */
		{

			_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
			if (rtw_is_list_empty(&pstat->asoc_list) == _FALSE) {
				rtw_list_delete(&pstat->asoc_list);
				pstapriv->asoc_list_cnt--;
				RTW_INFO("%s, asoc_list_cnt=%u\n", __func__, pstapriv->asoc_list_cnt);
				#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
				if (pstat->tbtx_enable)
					pstapriv->tbtx_asoc_list_cnt--;
				#endif
				if (pstat->expire_to > 0)
					;/* TODO: STA re_auth within expire_to */
			}
			_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

			if (seq == 1)
				; /* TODO: STA re_auth and auth timeout */

		}
	}

#ifdef CONFIG_IEEE80211W
	if ((pstat->bpairwise_key_installed != _TRUE && (pstat->flags & WLAN_STA_MFP))
		|| !(pstat->flags & WLAN_STA_MFP))
#endif /* CONFIG_IEEE80211W */
	{
		_rtw_spinlock_bh(&pstapriv->auth_list_lock);
		if (rtw_is_list_empty(&pstat->auth_list)) {

			rtw_list_insert_tail(&pstat->auth_list, &pstapriv->auth_list);
			pstapriv->auth_list_cnt++;
		}
		_rtw_spinunlock_bh(&pstapriv->auth_list_lock);
	}

	if (pstat->auth_seq == 0)
		pstat->expire_to = pstapriv->auth_to;

#ifdef CONFIG_IOCTL_CFG80211
	if (GET_CFG80211_REPORT_MGMT(adapter_wdev_data(padapter), IEEE80211_STYPE_AUTH) == _TRUE) {
		if ((algorithm == WLAN_AUTH_SAE) &&
			(auth_mode == dot11AuthAlgrthm_8021X)) {
			pstat->authalg = algorithm;

			rtw_cfg80211_rx_mframe(padapter, precv_frame, NULL);
			return _SUCCESS;
		}
	#ifdef CONFIG_RTW_80211R_AP
		pstat->authalg = algorithm;
		if (algorithm == WLAN_AUTH_FT) {
			rtw_cfg80211_rx_mframe(padapter, precv_frame, "FTTA");
			return _SUCCESS;
		}
	#endif
	}
#endif /* CONFIG_IOCTL_CFG80211 */

	if ((pstat->auth_seq + 1) != seq) {
		RTW_INFO("(1)auth rejected because out of seq [rx_seq=%d, exp_seq=%d]!\n",
			 seq, pstat->auth_seq + 1);
		status = _STATS_OUT_OF_AUTH_SEQ_;
		goto auth_fail;
	}

	if (algorithm == 0 && (auth_mode == 0 || auth_mode == 2 || auth_mode == 3)) {
		if (seq == 1) {
#ifdef CONFIG_IEEE80211W
			if ((pstat->bpairwise_key_installed != _TRUE && (pstat->flags & WLAN_STA_MFP))
				|| !(pstat->flags & WLAN_STA_MFP))
#endif /* CONFIG_IEEE80211W */
			{
				pstat->state &= ~WIFI_FW_AUTH_NULL;
				pstat->state |= WIFI_FW_AUTH_SUCCESS;
				pstat->expire_to = pstapriv->assoc_to;
			}
			pstat->authalg = algorithm;
		} else {
			RTW_INFO("(2)auth rejected because out of seq [rx_seq=%d, exp_seq=%d]!\n",
				 seq, pstat->auth_seq + 1);
			status = _STATS_OUT_OF_AUTH_SEQ_;
			goto auth_fail;
		}
	} else { /* shared system or auto authentication */
		if (seq == 1) {
			/* prepare for the challenging txt... */

			/* get_random_bytes((void *)pstat->chg_txt, 128); */ /* TODO: */
			_rtw_memset((void *)pstat->chg_txt, 78, 128);
#ifdef CONFIG_IEEE80211W
			if ((pstat->bpairwise_key_installed != _TRUE && (pstat->flags & WLAN_STA_MFP))
				|| !(pstat->flags & WLAN_STA_MFP))
#endif /* CONFIG_IEEE80211W */
			{
				pstat->state &= ~WIFI_FW_AUTH_NULL;
				pstat->state |= WIFI_FW_AUTH_STATE;
			}
			pstat->authalg = algorithm;
			pstat->auth_seq = 2;
		} else if (seq == 3) {
			/* checking for challenging txt... */
			RTW_INFO("checking for challenging txt...\n");

			p = rtw_get_ie(pframe + WLAN_HDR_A3_LEN + 4 + _AUTH_IE_OFFSET_ , _CHLGETXT_IE_, (int *)&ie_len,
				len - WLAN_HDR_A3_LEN - _AUTH_IE_OFFSET_ - 4);

			if ((p == NULL) || (ie_len <= 0)) {
				RTW_INFO("auth rejected because challenge failure!(1)\n");
				status = _STATS_CHALLENGE_FAIL_;
				goto auth_fail;
			}

			if (_rtw_memcmp((void *)(p + 2), pstat->chg_txt, 128)) {
#ifdef CONFIG_IEEE80211W
				if ((pstat->bpairwise_key_installed != _TRUE && (pstat->flags & WLAN_STA_MFP))
					|| !(pstat->flags & WLAN_STA_MFP))
#endif /* CONFIG_IEEE80211W */
				{
					pstat->state &= (~WIFI_FW_AUTH_STATE);
					pstat->state |= WIFI_FW_AUTH_SUCCESS;
					/* challenging txt is correct... */
					pstat->expire_to =  pstapriv->assoc_to;
				}
			} else {
				RTW_INFO("auth rejected because challenge failure!\n");
				status = _STATS_CHALLENGE_FAIL_;
				goto auth_fail;
			}
		} else {
			RTW_INFO("(3)auth rejected because out of seq [rx_seq=%d, exp_seq=%d]!\n",
				 seq, pstat->auth_seq + 1);
			status = _STATS_OUT_OF_AUTH_SEQ_;
			goto auth_fail;
		}
	}


	/* Now, we are going to issue_auth... */
	pstat->auth_seq = seq + 1;

#ifdef CONFIG_NATIVEAP_MLME
	issue_auth(padapter, pstat, (unsigned short)(_STATS_SUCCESSFUL_));
#endif

	if ((pstat->state & WIFI_FW_AUTH_SUCCESS) || (pstat->state & WIFI_FW_ASSOC_SUCCESS))
		pstat->auth_seq = 0;


	return _SUCCESS;

auth_fail:
#ifdef CONFIG_NATIVEAP_MLME
	issue_auth(padapter, pstat, (unsigned short)status);
#endif
	/* Free sta_info, phl_sta, mld */
	if (pstat) {
		RTW_INFO("%s : fail, free  sta_info, phl_sta, mld, for sa="MAC_FMT"\n",
			__func__, MAC_ARG(sa));
		rtw_free_stainfo_sw(padapter , pstat);
		mld = rtw_phl_get_mld_by_addr(phl, phl_role, sa);
		rtw_phl_free_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)), mld);
	}
#endif
	return _FAIL;

}

unsigned int OnAuthClient(_adapter *padapter, union recv_frame *precv_frame)
{
	unsigned int	seq, len, status, algthm, offset;
	unsigned char	*p;
	unsigned int	go2asoc = 0;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint pkt_len = precv_frame->u.hdr.len;
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;
	struct link_mlme_ext_info	*lmlmeinfo = &padapter_link->mlmeextpriv.mlmext_info;

	RTW_INFO("%s\n", __FUNCTION__);

#ifdef CONFIG_IOCTL_CFG80211
	if (GET_CFG80211_REPORT_MGMT(adapter_wdev_data(padapter), IEEE80211_STYPE_AUTH) == _TRUE) {
		if (rtw_sec_chk_auth_type(padapter, MLME_AUTHTYPE_SAE)) {
			if (rtw_cached_pmkid(padapter, get_my_bssid(&lmlmeinfo->network)) != -1) {
				RTW_INFO("SAE: PMKSA cache entry found\n");
				goto normal;
			}
			rtw_cfg80211_rx_mframe(padapter, precv_frame, NULL);
			return _SUCCESS;
		}
	}

normal:
#endif /* CONFIG_IOCTL_CFG80211 */

	/* check A1 matches or not */
	if (!rtw_match_self_addr(padapter, get_da(pframe)))
		return _SUCCESS;

	if (!(pmlmeinfo->state & WIFI_FW_AUTH_STATE) || pmlmeext->join_abort)
		return _SUCCESS;

	offset = (GetPrivacy(pframe)) ? 4 : 0;

	algthm	= le16_to_cpu(*(unsigned short *)((SIZE_PTR)pframe + WLAN_HDR_A3_LEN + offset));
	seq	= le16_to_cpu(*(unsigned short *)((SIZE_PTR)pframe + WLAN_HDR_A3_LEN + offset + 2));
	status	= le16_to_cpu(*(unsigned short *)((SIZE_PTR)pframe + WLAN_HDR_A3_LEN + offset + 4));

	if (status != 0) {
		RTW_INFO("clnt auth fail, status: %d\n", status);
		if (status == 13) { /* && pmlmeinfo->auth_algo == dot11AuthAlgrthm_Auto) */
			if (pmlmeinfo->auth_algo == dot11AuthAlgrthm_Shared)
				pmlmeinfo->auth_algo = dot11AuthAlgrthm_Open;
			else
				pmlmeinfo->auth_algo = dot11AuthAlgrthm_Shared;
			/* pmlmeinfo->reauth_count = 0; */
		}

		pmlmeinfo->auth_status = status;
		set_link_timer(pmlmeext, 1);
		goto authclnt_fail;
	}

	if (seq == 2) {
		if (pmlmeinfo->auth_algo == dot11AuthAlgrthm_Shared) {
			/* legendary shared system */
			p = rtw_get_ie(pframe + WLAN_HDR_A3_LEN + _AUTH_IE_OFFSET_, _CHLGETXT_IE_, (int *)&len,
				pkt_len - WLAN_HDR_A3_LEN - _AUTH_IE_OFFSET_);

			if (p == NULL) {
				/* RTW_INFO("marc: no challenge text?\n"); */
				goto authclnt_fail;
			}

			if (len > sizeof(pmlmeinfo->chg_txt)) {
				goto authclnt_fail;
			}

			_rtw_memcpy((void *)(pmlmeinfo->chg_txt), (void *)(p + 2), len);
			pmlmeinfo->auth_seq = 3;
			issue_auth(padapter, NULL, 0);
			set_link_timer(pmlmeext, REAUTH_TO);

			return _SUCCESS;
		} else {
			/* open, or 802.11r FTAA system */
			go2asoc = 1;
		}
	} else if (seq == 4) {
		if (pmlmeinfo->auth_algo == dot11AuthAlgrthm_Shared)
			go2asoc = 1;
		else
			goto authclnt_fail;
	} else {
		/* this is also illegal */
		/* RTW_INFO("marc: clnt auth failed due to illegal seq=%x\n", seq); */
		goto authclnt_fail;
	}

	if (go2asoc) {
#ifdef CONFIG_RTW_80211R
		if (rtw_ft_update_auth_rsp_ies(padapter, pframe, pkt_len))
			return _SUCCESS;
#endif
		RTW_PRINT("auth success, start assoc\n");
		start_clnt_assoc(padapter);
		return _SUCCESS;
	}

authclnt_fail:

	/* pmlmeinfo->state &= ~(WIFI_FW_AUTH_STATE); */

	return _FAIL;

}

static u8 rtw_deny_legacy_sta(_adapter *padapter, struct sta_info *pstat)
{
	struct link_mlme_ext_priv *pmlmeext = &pstat->padapter_link->mlmeextpriv;
	struct registry_priv *pregpriv = &padapter->registrypriv;
	u8 res = _FALSE;

	sta_info_update(padapter, pstat);
	rtw_ap_set_sta_wmode(padapter, pstat);

	if (pmlmeext->cur_wireless_mode == WLAN_MD_11BGN) {
		if ((pregpriv->deny_legacy == WLAN_MD_11BG) &&
			!(pstat->phl_sta->wmode & WLAN_MD_11N))
			/* 2.4G N only */
			res = _TRUE;
		else if ((pregpriv->deny_legacy == WLAN_MD_11B) &&
				 (pstat->phl_sta->wmode == WLAN_MD_11B))
			/* 2.4G G+N */
			res = _TRUE;
	} else if (pmlmeext->cur_wireless_mode == WLAN_MD_11BG) {
		if ((pregpriv->deny_legacy == WLAN_MD_11B) &&
			(pstat->phl_sta->wmode == WLAN_MD_11B))
			/* 2.4G G only */
			res = _TRUE;
	} else if (pmlmeext->cur_wireless_mode == WLAN_MD_11A_AC) {
		if ((pregpriv->deny_legacy == WLAN_MD_11AN) &&
			!(pstat->phl_sta->wmode & WLAN_MD_11AC))
			/* 5G AC only */
			res = _TRUE;
		else if ((pregpriv->deny_legacy == WLAN_MD_11A) &&
				 (pstat->phl_sta->wmode == WLAN_MD_11A))
			/* 5G N+AC */
			res = _TRUE;
	} else if (pmlmeext->cur_wireless_mode == WLAN_MD_11AN) {
		if ((pregpriv->deny_legacy == WLAN_MD_11A) &&
			(pstat->phl_sta->wmode == WLAN_MD_11A))
			/* 5G N only */
			res = _TRUE;
	}

	return res;
}

unsigned int OnAssocReq(_adapter *padapter, union recv_frame *precv_frame)
{
#ifdef CONFIG_AP_MODE
	u16 listen_interval;
	struct rtw_ieee802_11_elems elems;
	struct sta_info *pstat;
	unsigned char		reassoc, *pos;
	int		left;
	unsigned short		status = _STATS_SUCCESSFUL_;
	unsigned short		frame_type, ie_offset = 0;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	WLAN_BSSID_EX	*cur = &(pmlmeinfo->dev_network);
	struct sta_priv *pstapriv = &padapter->stapriv;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint pkt_len = precv_frame->u.hdr.len;
#ifdef CONFIG_P2P
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	u8 p2p_status_code = P2P_STATUS_SUCCESS;
	u8 *p2pie;
	u32 p2pielen = 0;
#endif /* CONFIG_P2P */
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	u8 sta_tbtx_enable = _FALSE;
#endif
	struct registry_priv *pregpriv = &padapter->registrypriv;

#ifdef CONFIG_CONCURRENT_MODE
	if (((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) &&
	    rtw_mi_buddy_check_fwstate(padapter, WIFI_UNDER_LINKING | WIFI_UNDER_SURVEY)) {
		/* don't process assoc request; */
		return _SUCCESS;
	}
#endif /* CONFIG_CONCURRENT_MODE */

	if ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)
		return _FAIL;

	if (rtw_check_invalid_mac_address(get_addr2_ptr(pframe), _FALSE)) {
		RTW_INFO("%s : reject invalid ASSOC-req "MAC_FMT"\n", 
			__func__, MAC_ARG(get_addr2_ptr(pframe)));
		return _FAIL;
	}

	frame_type = get_frame_sub_type(pframe);
	if (frame_type == WIFI_ASSOCREQ) {
		reassoc = 0;
		ie_offset = _ASOCREQ_IE_OFFSET_;
	} else { /* WIFI_REASSOCREQ */
		reassoc = 1;
		ie_offset = _REASOCREQ_IE_OFFSET_;
	}


	if (pkt_len < IEEE80211_3ADDR_LEN + ie_offset) {
		RTW_INFO("handle_assoc(reassoc=%d) - too short payload (len=%lu)"
			 "\n", reassoc, (unsigned long)pkt_len);
		return _FAIL;
	}

	pstat = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
	if (pstat == (struct sta_info *)NULL) {
		status = _RSON_CLS2_;
		goto asoc_class2_error;
	}

#ifdef RTW_PHL_TEST_FPGA
	DBGP(" [%s][%d] force assoc \n", __FUNCTION__, __LINE__);
	pstat->state = WIFI_ASOC_STATE;
	pstat->state |= WIFI_FW_ASSOC_SUCCESS;
	issue_asocrsp(padapter, status, pstat, WIFI_ASSOCRSP);
	return _SUCCESS;
#endif


	RTW_INFO("%s\n", __FUNCTION__);

	if (pstat->authalg == WLAN_AUTH_SAE) {
		/* WPA3-SAE */
		if (((pstat->state) & WIFI_FW_AUTH_NULL)) {
			/* TODO:
			   Queue AssocReq and Proccess
			   by external auth trigger. */
			RTW_INFO("%s: wait external auth trigger\n", __func__);
			return _SUCCESS;
		}
	}

	/* check if this stat has been successfully authenticated/assocated */
	if (!((pstat->state) & WIFI_FW_AUTH_SUCCESS)) {
		if (!((pstat->state) & WIFI_FW_ASSOC_SUCCESS)) {
			status = _RSON_CLS2_;
			goto asoc_class2_error;
		} else {
			pstat->state &= (~WIFI_FW_ASSOC_SUCCESS);
			pstat->state |= WIFI_FW_ASSOC_STATE;
		}
	} else {
		pstat->state &= (~WIFI_FW_AUTH_SUCCESS);
		pstat->state |= WIFI_FW_ASSOC_STATE;
	}

#if 0/* todo:tkip_countermeasures */
	if (hapd->tkip_countermeasures) {
		resp = WLAN_REASON_MICHAEL_MIC_FAILURE;
		goto fail;
	}
#endif
	/*GEORGIA_TODO_MOVE_CODE_TO_DBG_SYS*/
	if (rtw_ap_linking_test_force_asoc_fail()) {
		status = rtw_ap_linking_test_force_asoc_fail();
		RTW_INFO(FUNC_ADPT_FMT" force asoc fail with status:%u\n"
			, FUNC_ADPT_ARG(padapter), status);
		goto OnAssocReqFail;
	}

	/* now parse all ieee802_11 ie to point to elems */
	left = pkt_len - (IEEE80211_3ADDR_LEN + ie_offset);
	pos = pframe + (IEEE80211_3ADDR_LEN + ie_offset);
	if (rtw_ieee802_11_parse_elems(pos, left, &elems, 1) == ParseFailed) {
		RTW_INFO("STA " MAC_FMT " sent invalid association request\n",
			 MAC_ARG(pstat->phl_sta->mac_addr));
		status = _STATS_FAILURE_;
		goto OnAssocReqFail;
	}

	rtw_ap_parse_sta_capability(padapter, pstat, pframe + WLAN_HDR_A3_LEN);

	listen_interval = RTW_GET_LE16(pframe + WLAN_HDR_A3_LEN + 2);
#if 0/* todo: */
	/* check listen_interval */
	if (listen_interval > hapd->conf->max_listen_interval) {
		hostapd_logger(hapd, mgmt->sa, HOSTAPD_MODULE_IEEE80211,
			       HOSTAPD_LEVEL_DEBUG,
			       "Too large Listen Interval (%d)",
			       listen_interval);
		resp = WLAN_STATUS_ASSOC_DENIED_LISTEN_INT_TOO_LARGE;
		goto fail;
	}

	pstat->listen_interval = listen_interval;
#endif

	/* now we should check all the fields... */
	/* checking SSID */
	if (elems.ssid == NULL
		|| elems.ssid_len == 0
		|| elems.ssid_len != cur->Ssid.SsidLength
		|| _rtw_memcmp(elems.ssid, cur->Ssid.Ssid, cur->Ssid.SsidLength) == _FALSE
	) {
		status = _STATS_FAILURE_;
		goto OnAssocReqFail;
	}

	/* (Extended) Supported rates */
	status = rtw_ap_parse_sta_supported_rates(padapter, pstat
		, pframe + WLAN_HDR_A3_LEN + ie_offset, pkt_len - WLAN_HDR_A3_LEN - ie_offset);
	if (status != _STATS_SUCCESSFUL_)
		goto OnAssocReqFail;

	/* check RSN/WPA/WPS */
	status = rtw_ap_parse_sta_security_ie(padapter, pstat, &elems);
	if (status != _STATS_SUCCESSFUL_)
		goto OnAssocReqFail;

	/* check if there is WMM IE & support WWM-PS */
	rtw_ap_parse_sta_wmm_ie(padapter, pstat
		, pframe + WLAN_HDR_A3_LEN + ie_offset, pkt_len - WLAN_HDR_A3_LEN - ie_offset);

#ifdef CONFIG_RTS_FULL_BW
	/*check vendor IE*/
	rtw_parse_sta_vendor_ie_8812(padapter, pstat
		, pframe + WLAN_HDR_A3_LEN + ie_offset, pkt_len - WLAN_HDR_A3_LEN - ie_offset);
#endif/*CONFIG_RTS_FULL_BW*/

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	if (elems.tbtx_cap && elems.tbtx_cap_len != 0) {
		if(rtw_is_tbtx_capabilty(elems.tbtx_cap, elems.tbtx_cap_len)) {
			sta_tbtx_enable = _TRUE;
		}
	}

#endif

	rtw_ap_parse_sta_ht_ie(padapter, pstat, &elems);
	rtw_ap_parse_sta_vht_ie(padapter, pstat, &elems);
	rtw_ap_parse_sta_he_ie(padapter, pstat, &elems);

	if (pregpriv->deny_legacy && rtw_deny_legacy_sta(padapter, pstat)) {
		RTW_INFO("Deny legacy STA " MAC_FMT " association\n",
			 MAC_ARG(pstat->phl_sta->mac_addr));
		status = WLAN_STATUS_UNSPECIFIED_FAILURE;
		goto OnAssocReqFail;
	}

	if (((pstat->flags & WLAN_STA_HT) || (pstat->flags & WLAN_STA_VHT) || (pstat->flags & WLAN_STA_HE)) &&
	    ((pstat->wpa2_pairwise_cipher & WPA_CIPHER_TKIP) ||
	     (pstat->wpa_pairwise_cipher & WPA_CIPHER_TKIP))) {

		RTW_INFO("HT/VHT/HE: " MAC_FMT " TKIP association\n", MAC_ARG(pstat->phl_sta->mac_addr));

		pstat->flags &= ~WLAN_STA_HT;
		pstat->flags &= ~WLAN_STA_VHT;
		pstat->flags &= ~WLAN_STA_HE;
	}

#ifdef CONFIG_P2P
	pstat->is_p2p_device = _FALSE;
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
		p2pie = rtw_get_p2p_ie(pframe + WLAN_HDR_A3_LEN + ie_offset , pkt_len - WLAN_HDR_A3_LEN - ie_offset , NULL, &p2pielen);
		if (p2pie) {
			pstat->is_p2p_device = _TRUE;
			p2p_status_code = (u8)process_assoc_req_p2p_ie(pwdinfo, pframe, pkt_len, pstat);
			if (p2p_status_code > 0) {
				pstat->p2p_status_code = p2p_status_code;
				status = _STATS_CAP_FAIL_;
				goto OnAssocReqFail;
			}
		}
#ifdef CONFIG_WFD
		rtw_process_wfd_ies(padapter, pframe + WLAN_HDR_A3_LEN + ie_offset, pkt_len - WLAN_HDR_A3_LEN - ie_offset, __func__);
#endif
	}
	pstat->p2p_status_code = p2p_status_code;
#endif /* CONFIG_P2P */

	rtw_ap_parse_sta_multi_ap_ie(padapter, pstat, pos, left);

#ifdef CONFIG_RTW_MBO
	rtw_mbo_process_assoc_req(padapter,
		(pframe + WLAN_HDR_A3_LEN + ie_offset),
		(pkt_len - WLAN_HDR_A3_LEN - ie_offset));
#endif

	/* TODO: identify_proprietary_vendor_ie(); */
	/* Realtek proprietary IE */
	/* identify if this is Broadcom sta */
	/* identify if this is ralink sta */
	/* Customer proprietary IE */
#ifdef PRIVATE_R
	if (pmlmepriv->vendor_ie_filter_enable) {
		if (!check_vendor_ie(padapter, pframe + WLAN_HDR_A3_LEN + _ASOCREQ_IE_OFFSET_,
			pkt_len - WLAN_HDR_A3_LEN - _ASOCREQ_IE_OFFSET_)) {
			status = _STATS_UNABLE_HANDLE_STA_;
			goto OnAssocReqFail;
		}
	}
#endif

#ifdef CONFIG_RTW_80211K
	rtw_ap_parse_sta_rm_en_cap(padapter, pstat, &elems);
#endif

#ifdef CONFIG_RTW_MBO
	if (pmlmepriv->mbopriv.enable == _TRUE) {
		if(pmlmepriv->mbopriv.assoc_disallow != 0){
			RTW_INFO("Reject STA " MAC_FMT ":MBO association disallowed\n",
			 MAC_ARG(pstat->phl_sta->mac_addr));
			status = WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA;
			goto OnAssocReqFail;
		}
		rtw_ap_parse_sta_mbo_element(padapter, pstat,
						pframe + WLAN_HDR_A3_LEN + ie_offset,
						pkt_len - WLAN_HDR_A3_LEN - ie_offset);
	}
#endif /*CONFIG_RTW_MBO*/

	/* AID assignment */
	if (pstat->phl_sta->aid > 0)
		RTW_INFO(FUNC_ADPT_FMT" old AID=%d\n", FUNC_ADPT_ARG(padapter), pstat->phl_sta->aid);
	else {
		if (!rtw_aid_alloc(padapter, pstat)) {
			RTW_INFO(FUNC_ADPT_FMT" no room for more AIDs\n", FUNC_ADPT_ARG(padapter));
			status = WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA;
			goto OnAssocReqFail;
		}
		RTW_INFO(FUNC_ADPT_FMT" allocate new AID=%d\n", FUNC_ADPT_ARG(padapter), pstat->phl_sta->aid);
	}

	pstat->state &= (~WIFI_FW_ASSOC_STATE);
	pstat->state |= WIFI_FW_ASSOC_SUCCESS;
	/* RTW_INFO("==================%s, %d,  (%x), bpairwise_key_installed=%d, MAC:"MAC_FMT"\n"
	, __func__, __LINE__, pstat->state, pstat->bpairwise_key_installed, MAC_ARG(pstat->phl_sta->mac_addr)); */
#ifdef CONFIG_IEEE80211W
	if ((pstat->bpairwise_key_installed != _TRUE && (pstat->flags & WLAN_STA_MFP))
		|| !(pstat->flags & WLAN_STA_MFP))
#endif /* CONFIG_IEEE80211W */
	{
		_rtw_spinlock_bh(&pstapriv->auth_list_lock);
		if (!rtw_is_list_empty(&pstat->auth_list)) {
			rtw_list_delete(&pstat->auth_list);
			pstapriv->auth_list_cnt--;
		}
		_rtw_spinunlock_bh(&pstapriv->auth_list_lock);

		_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
		if (rtw_is_list_empty(&pstat->asoc_list)) {
			pstat->expire_to = pstapriv->expire_to;
			rtw_list_insert_tail(&pstat->asoc_list, &pstapriv->asoc_list);
			pstapriv->asoc_list_cnt++;
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
			if (sta_tbtx_enable) {
				pstat->tbtx_enable = _TRUE;
				pstapriv->tbtx_asoc_list_cnt++;
			}
#endif
		}
		_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);
	}

	/* now the station is qualified to join our BSS...	 */
	if (pstat && (pstat->state & WIFI_FW_ASSOC_SUCCESS) && (_STATS_SUCCESSFUL_ == status)) {
#ifdef CONFIG_NATIVEAP_MLME
#ifdef CONFIG_IEEE80211W
		if ((pstat->bpairwise_key_installed != _TRUE && (pstat->flags & WLAN_STA_MFP))
			|| !(pstat->flags & WLAN_STA_MFP))
#endif /* CONFIG_IEEE80211W */
		{
			/* .1 bss_cap_update & sta_info_update */
			bss_cap_update_on_sta_join(padapter, pstat);
			sta_info_update(padapter, pstat);
		}
#ifdef CONFIG_IEEE80211W
		if (pstat->bpairwise_key_installed == _TRUE && (pstat->flags & WLAN_STA_MFP))
			status = _STATS_REFUSED_TEMPORARILY_;
#endif /* CONFIG_IEEE80211W */
#ifdef CONFIG_RTW_MBO
		if (rtw_mbo_ap_assoc_disallow(padapter))
			status = _STATS_UNABLE_HANDLE_STA_;
#endif
#ifdef CONFIG_RTW_80211R_AP
		if ((pstat && (pstat->authalg != WLAN_AUTH_FT)))
#endif
		{
			/* .2 issue assoc rsp before notify station join event. */
			if (frame_type == WIFI_ASSOCREQ)
				issue_asocrsp(padapter, status, pstat, WIFI_ASSOCRSP);
			else
				issue_asocrsp(padapter, status, pstat, WIFI_REASSOCRSP);
		}

#ifdef CONFIG_IOCTL_CFG80211
		_rtw_spinlock_bh(&pstat->lock);
		if (pstat->passoc_req) {
			rtw_mfree(pstat->passoc_req, pstat->assoc_req_len);
			pstat->passoc_req = NULL;
			pstat->assoc_req_len = 0;
		}

		pstat->passoc_req =  rtw_zmalloc(pkt_len);
		if (pstat->passoc_req) {
			_rtw_memcpy(pstat->passoc_req, pframe, pkt_len);
			pstat->assoc_req_len = pkt_len;
		}
		_rtw_spinunlock_bh(&pstat->lock);
#endif /* CONFIG_IOCTL_CFG80211 */
#ifdef CONFIG_IEEE80211W
		if ((pstat->bpairwise_key_installed != _TRUE && (pstat->flags & WLAN_STA_MFP))
			|| !(pstat->flags & WLAN_STA_MFP))
#endif /* CONFIG_IEEE80211W */
		{
			/* .3-(1) report sta add event */
			report_add_sta_event(padapter, pstat->phl_sta->mac_addr);
		}
#ifdef CONFIG_IEEE80211W
		if (pstat->bpairwise_key_installed == _TRUE && (pstat->flags & WLAN_STA_MFP)) {
			RTW_INFO(MAC_FMT"\n", MAC_ARG(pstat->phl_sta->mac_addr));
			issue_action_SA_Query(padapter, pstat->padapter_link,
					pstat->phl_sta->mac_addr, 0, 0, IEEE80211W_RIGHT_KEY);
		}
#endif /* CONFIG_IEEE80211W */
#endif /* CONFIG_NATIVEAP_MLME */
	}

	return _SUCCESS;

asoc_class2_error:

#ifdef CONFIG_NATIVEAP_MLME
	issue_deauth(padapter, (void *)get_addr2_ptr(pframe), status);
#endif

	return _FAIL;

OnAssocReqFail:


#ifdef CONFIG_NATIVEAP_MLME
	/* pstat->phl_sta->aid = 0; */
	if (frame_type == WIFI_ASSOCREQ)
		issue_asocrsp(padapter, status, pstat, WIFI_ASSOCRSP);
	else
		issue_asocrsp(padapter, status, pstat, WIFI_REASSOCRSP);
#endif

	/* Free sta_info, phl_sta, mld */
	if (pstat) {
		struct rtw_phl_mld_t *mld = NULL;
		void *phl = GET_PHL_INFO(adapter_to_dvobj(padapter));
		struct rtw_wifi_role_t *phl_role = padapter->phl_role;

		RTW_INFO("%s : fail, free  sta_info, phl_sta, mld, for sa="MAC_FMT"\n",
			__func__, MAC_ARG(get_addr2_ptr(pframe)));
		rtw_free_stainfo_sw(padapter , pstat);
		mld = rtw_phl_get_mld_by_addr(phl, phl_role, get_addr2_ptr(pframe));
		rtw_phl_free_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)), mld);
	}

#endif /* CONFIG_AP_MODE */

	return _FAIL;

}

#if defined(CONFIG_LAYER2_ROAMING) && defined(CONFIG_RTW_80211K)
void rtw_roam_nb_discover(_adapter *padapter, u8 bfroce)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_info *pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
	u8 nb_req_issue = _FALSE;

	if (!check_fwstate(pmlmepriv, WIFI_ASOC_STATE))
		return;

	if (!rtw_chk_roam_flags(padapter, RTW_ROAM_ACTIVE))
		return;

	psta = rtw_get_stainfo(pstapriv, pmlmeinfo->network.MacAddress);
	if (!psta)
		return;

	if (bfroce || (!pmlmepriv->nb_info.nb_rpt_is_same))
		nb_req_issue = _TRUE;

	if (nb_req_issue && (psta->rm_en_cap[0] & RTW_RRM_NB_RPT_EN))
		rm_add_nb_req(padapter, psta);
}
#endif

void rtw_ie_handler(struct _ADAPTER *padapter, struct _ADAPTER_LINK *padapter_link,
				u8 *ie, u32 ie_len)
{
	int i;
	struct _NDIS_802_11_VARIABLE_IEs *p = NULL;
#ifdef CONFIG_WAPI_SUPPORT
	PNDIS_802_11_VARIABLE_IEs pWapiIE = NULL;
#endif

	if (!ie || (ie_len == 0))
		return;

	for (i = 0; i < ie_len;) {
		p = (struct _NDIS_802_11_VARIABLE_IEs *)(ie + i);
		switch (p->ElementID) {
#ifdef CONFIG_WAPI_SUPPORT
		case _WAPI_IE_:
			pWapiIE = p;
			break;
#endif

		case _VENDOR_SPECIFIC_IE_:
			if (_rtw_memcmp(p->data, WMM_PARA_OUI, 6))	/* WMM */
				WMM_param_handler(padapter, padapter_link, p);
#if defined(CONFIG_P2P) && defined(CONFIG_WFD)
			else if (_rtw_memcmp(p->data, WFD_OUI, 4))		/* WFD */
				rtw_process_wfd_ie(padapter, (u8 *)p, p->Length, __func__);
#endif
			break;
		case _HT_CAPABILITY_IE_:	/* HT caps */
			HT_caps_handler(padapter, padapter_link, p);
#ifdef PRIVATE_R
			HT_caps_handler_infra_ap(padapter, padapter_link, p);
#endif /* PRIVATE_R */
			break;

		case _HT_EXTRA_INFO_IE_:	/* HT info */
			HT_info_handler(padapter, padapter_link, p);
			break;

#ifdef CONFIG_80211AC_VHT
		case EID_VHTCapability:
			VHT_caps_handler(padapter, padapter_link, p);
#ifdef PRIVATE_R
			VHT_caps_handler_infra_ap(padapter, padapter_link, p);
#endif /* PRIVATE_R */
			break;

		case EID_VHTOperation:
			VHT_operation_handler(padapter, padapter_link, p);
			break;
#endif

		case WLAN_EID_EXTENSION:
#ifdef CONFIG_80211AX_HE
			if (p->data[0] == WLAN_EID_EXTENSION_HE_CAPABILITY)
				HE_caps_handler(padapter, padapter_link, p);
			else if (p->data[0] == WLAN_EID_EXTENSION_HE_OPERATION)
				HE_operation_handler(padapter, padapter_link, p);
			else if (p->data[0] == WLAN_EID_EXTENSION_HE_MU_EDCA)
				HE_mu_edca_handler(padapter, padapter_link, p, _TRUE);
			else if (p->data[0] == WLAN_EID_EXT_HE_6G_CAP)
				HE_6g_bandcap_handler(padapter, padapter_link, p);
#endif
			break;
		case _ERPINFO_IE_:
			ERP_IE_handler(padapter, padapter_link, p);
			break;
#ifdef CONFIG_TDLS
		case WLAN_EID_EXT_CAP:
			if (check_ap_tdls_prohibited(p->data, p->Length) == _TRUE)
				padapter->tdlsinfo.ap_prohibited = _TRUE;
			if (check_ap_tdls_ch_switching_prohibited(p->data, p->Length) == _TRUE)
				padapter->tdlsinfo.ch_switch_prohibited = _TRUE;
			break;
#endif /* CONFIG_TDLS */

		case _EID_RRM_EN_CAP_IE_: /* ToDo CONFIG_RTW_MLD: per link? */
			RM_IE_handler(padapter, p);
			break;
#ifdef PRIVATE_R
		/* Infra mode, used to store AP's info , Parse the supported rates from AssocRsp */
		case _SUPPORTEDRATES_IE_:
			Supported_rate_infra_ap(padapter, padapter_link, p);
			break;

		case _EXT_SUPPORTEDRATES_IE_:
			Extended_Supported_rate_infra_ap(padapter, padapter_link, p);
			break;
#endif /* PRIVATE_R */
		default:
			break;
		}

		i += (p->Length + 2);
	}
}

unsigned int OnAssocRsp(_adapter *padapter, union recv_frame *precv_frame)
{
	uint i;
	int res;
	unsigned short status;
	PNDIS_802_11_VARIABLE_IEs pIE = NULL;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	/* WLAN_BSSID_EX 		*cur_network = &(pmlmeinfo->network); */
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint pkt_len = precv_frame->u.hdr.len;
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;
	struct link_mlme_ext_info *lmlmeinfo = &padapter_link->mlmeextpriv.mlmext_info;
#ifdef CONFIG_80211BE_EHT
	struct mlo_priv *pmlopriv = &pmlmepriv->mlopriv;
#endif

	RTW_INFO("%s\n", __FUNCTION__);

	/* check A1 matches or not */
	if (!rtw_match_self_addr(padapter, get_da(pframe)))
		return _SUCCESS;

	if (!(pmlmeinfo->state & (WIFI_FW_AUTH_SUCCESS | WIFI_FW_ASSOC_STATE)) || pmlmeext->join_abort)
		return _SUCCESS;

	if (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS)
		return _SUCCESS;

	cancel_link_timer(pmlmeext); /*_cancel_timer_ex(&pmlmeext->link_timer);*/

	/* status */
	status = le16_to_cpu(*(unsigned short *)(pframe + WLAN_HDR_A3_LEN + 2));
	if (status > 0) {
		RTW_INFO("assoc reject, status code: %d\n", status);
		pmlmeinfo->state = WIFI_FW_NULL_STATE;
		res = -4;
		goto report_assoc_result;
	}
	/* Connection is accepted */
	padapter_link->mlmepriv.is_accepted = _TRUE;

	/* get capabilities */
	lmlmeinfo->capability = le16_to_cpu(*(unsigned short *)(pframe + WLAN_HDR_A3_LEN));

	/* set slot time */
	lmlmeinfo->slotTime = (padapter_link->mlmeextpriv.mlmext_info.capability & BIT(10)) ? 9 : 20;

	/* AID assignment move to rtw_joinbss_update_stainfo */
	res = (int)(le16_to_cpu(*(unsigned short *)(pframe + WLAN_HDR_A3_LEN + 4)) & 0x3fff);

#ifdef RTW_PHL_TEST_FPGA
	res = 1;
#endif

	/* check aid value */
	if (res < 1 || res > 2007) {
		RTW_INFO("assoc reject, aid: %d\n", res);
		pmlmeinfo->state = WIFI_FW_NULL_STATE;
		res = -4;
		goto report_assoc_result;
	}

	/* following are moved to join event callback function */
	/* to handle HT, WMM, rate adaptive, update MAC reg */
	/* for not to handle the synchronous IO in the tasklet */

	rtw_ie_handler(padapter, padapter_link,
				pframe + 6 + WLAN_HDR_A3_LEN,
				pkt_len - 6 - WLAN_HDR_A3_LEN);

#ifdef CONFIG_80211BE_EHT
	if (pmlopriv->mlo_option)
		rtw_eht_ml_ies_handler(padapter, padapter_link,
				pframe + 6 + WLAN_HDR_A3_LEN,
				pkt_len - 6 - WLAN_HDR_A3_LEN);
#endif

#ifdef CONFIG_WAPI_SUPPORT
	rtw_wapi_on_assoc_ok(padapter, pIE);
#endif

	pmlmeinfo->state &= (~WIFI_FW_ASSOC_STATE);
	pmlmeinfo->state |= WIFI_FW_ASSOC_SUCCESS;

	/* Update Basic Rate Table for spec, 2010-12-28 , by thomas */
	UpdateBrateTbl(padapter, lmlmeinfo->network.SupportedRates);

report_assoc_result:
	if (res > 0)
		rtw_buf_update(&pmlmepriv->assoc_rsp, &pmlmepriv->assoc_rsp_len, pframe, pkt_len);
	else
		rtw_buf_free(&pmlmepriv->assoc_rsp, &pmlmepriv->assoc_rsp_len);

	report_join_res(padapter, res, status);

#if defined(CONFIG_LAYER2_ROAMING) && defined(CONFIG_RTW_80211K)
	rtw_roam_nb_discover(padapter, _TRUE);
#endif
	return _SUCCESS;
}

unsigned int OnDeAuth(_adapter *padapter, union recv_frame *precv_frame)
{
	unsigned short	reason;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8 *pframe = precv_frame->u.hdr.rx_data;
	bool active = _FALSE;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P */

	/* check A3 */
	if (!(rtw_match_bssid_ext(padapter, GetAddr3Ptr(pframe))))
		return _SUCCESS;

	RTW_INFO(FUNC_ADPT_FMT" - Start to Disconnect\n", FUNC_ADPT_ARG(padapter));

	reason = le16_to_cpu(*(unsigned short *)(pframe + WLAN_HDR_A3_LEN));

#ifdef CONFIG_AP_MODE
	if (MLME_IS_AP(padapter)) {
		struct sta_info *psta;
		struct sta_priv *pstapriv = &padapter->stapriv;

		/* _rtw_spinlock_bh(&(pstapriv->sta_hash_lock));		 */
		/* rtw_free_stainfo(padapter, psta); */
		/* _rtw_spinunlock_bh(&(pstapriv->sta_hash_lock));		 */

		RTW_PRINT(FUNC_ADPT_FMT" reason=%u, ta=%pM\n"
			, FUNC_ADPT_ARG(padapter), reason, get_addr2_ptr(pframe));

		psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
		if (psta) {
			u8 updated = _FALSE;

			if (ATOMIC_READ(&psta->deleting) == 0) {
				ATOMIC_INC(&psta->deleting);
				RTW_INFO("psta->deleting=%u\n", ATOMIC_READ(&psta->deleting));
			} else {
				return _SUCCESS;
			}

			_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
			if (rtw_is_list_empty(&psta->asoc_list) == _FALSE) {
				rtw_list_delete(&psta->asoc_list);
				pstapriv->asoc_list_cnt--;
				RTW_INFO("%s, asoc_list_cnt=%d\n", __func__, pstapriv->asoc_list_cnt);
				#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
				if (psta->tbtx_enable)
					pstapriv->tbtx_asoc_list_cnt--;
				#endif

#ifdef CONFIG_IEEE80211W
				/* pmf: 4.3.3.2 */
				if (psta->flags & WLAN_STA_MFP) {
					psta->flags &= ~WLAN_STA_MFP;
					reason = _RSON_CLS2_;
					active = _TRUE;
				}
#endif
				updated = ap_free_sta(padapter, psta, active, reason, _TRUE, _FALSE);

			}
			_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

			associated_clients_update(padapter, updated, STA_INFO_UPDATE_ALL);
		}


		return _SUCCESS;
	} else
#endif
	if (!MLME_IS_MESH(padapter)) {
		int	ignore_received_deauth = 0;

		/*	Commented by Albert 20130604 */
		/*	Before sending the auth frame to start the STA/GC mode connection with AP/GO,  */
		/*	we will send the deauth first. */
		/*	However, the Win8.1 with BRCM Wi-Fi will send the deauth with reason code 6 to us after receieving our deauth. */
		/*	Added the following code to avoid this case. */
		if ((pmlmeinfo->state & WIFI_FW_AUTH_STATE) ||
		    (pmlmeinfo->state & WIFI_FW_ASSOC_STATE)) {
			if (reason == WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA)
				ignore_received_deauth = 1;
			else if (WLAN_REASON_PREV_AUTH_NOT_VALID == reason) {
				/* TODO: 802.11r */
				ignore_received_deauth = 1;
			}
		}

#ifdef CONFIG_RTW_WNM
		if (!rtw_wnm_try_btm_roam_imnt(padapter))
			ignore_received_deauth = 1;
#endif

		RTW_PRINT(FUNC_ADPT_FMT" reason=%u, ta=%pM, ignore=%d\n"
			, FUNC_ADPT_ARG(padapter), reason, get_addr2_ptr(pframe), ignore_received_deauth);

		if (0 == ignore_received_deauth) {
			receive_disconnect(padapter, get_addr2_ptr(pframe), reason, _FALSE);
			pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
			pmlmeinfo->disconnect_code = DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DEAUTH;
			pmlmeinfo->wifi_reason_code = reason;
		}
	}
	pmlmepriv->LinkDetectInfo.bBusyTraffic = _FALSE;
	return _SUCCESS;

}

unsigned int OnDisassoc(_adapter *padapter, union recv_frame *precv_frame)
{
	unsigned short	reason;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8 *pframe = precv_frame->u.hdr.rx_data;
	bool active = _FALSE;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P */

	/* check A3 */
	if (!(rtw_match_bssid_ext(padapter, GetAddr3Ptr(pframe))))
		return _SUCCESS;

	RTW_INFO(FUNC_ADPT_FMT" - Start to Disconnect\n", FUNC_ADPT_ARG(padapter));

	reason = le16_to_cpu(*(unsigned short *)(pframe + WLAN_HDR_A3_LEN));

#ifdef CONFIG_AP_MODE
	if (MLME_IS_AP(padapter)) {
		struct sta_info *psta;
		struct sta_priv *pstapriv = &padapter->stapriv;

		/* _rtw_spinlock_bh(&(pstapriv->sta_hash_lock));	 */
		/* rtw_free_stainfo(padapter, psta); */
		/* _rtw_spinunlock_bh(&(pstapriv->sta_hash_lock));		 */

		RTW_PRINT(FUNC_ADPT_FMT" reason=%u, ta=%pM\n"
			, FUNC_ADPT_ARG(padapter), reason, get_addr2_ptr(pframe));

		psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
		if (psta) {
			u8 updated = _FALSE;

			if (ATOMIC_READ(&psta->deleting) == 0) {
				ATOMIC_INC(&psta->deleting);
				RTW_INFO("psta->deleting=%u\n", ATOMIC_READ(&psta->deleting));
			} else {
				return _SUCCESS;
			}

			_rtw_spinlock_bh(&pstapriv->asoc_list_lock);
			if (rtw_is_list_empty(&psta->asoc_list) == _FALSE) {
				rtw_list_delete(&psta->asoc_list);
				pstapriv->asoc_list_cnt--;
				RTW_INFO("%s, asoc_list_cnt=%d\n", __func__, pstapriv->asoc_list_cnt);
				#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
				if (psta->tbtx_enable)
					pstapriv->tbtx_asoc_list_cnt--;
				#endif

#ifdef CONFIG_IEEE80211W
				/* pmf: 4.3.3.1 */
				if (psta->flags & WLAN_STA_MFP) {
					psta->flags &= ~WLAN_STA_MFP;
					reason = _RSON_CLS2_;
					active = _TRUE;
				}
#endif
				updated = ap_free_sta(padapter, psta, active, reason, _TRUE, _FALSE);

			}
			_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

			associated_clients_update(padapter, updated, STA_INFO_UPDATE_ALL);
		}

		return _SUCCESS;
	} else
#endif
	if (!MLME_IS_MESH(padapter)) {
		RTW_PRINT(FUNC_ADPT_FMT" reason=%u, ta=%pM\n"
			, FUNC_ADPT_ARG(padapter), reason, get_addr2_ptr(pframe));

	#ifdef CONFIG_RTW_WNM
		if (rtw_wnm_try_btm_roam_imnt(padapter) > 0)
	#endif
		{
			receive_disconnect(padapter, get_addr2_ptr(pframe), reason, _FALSE);
			pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
			pmlmeinfo->disconnect_code = DISCONNECTION_BY_AP_DUE_TO_RECEIVE_DISASSOC;
			pmlmeinfo->wifi_reason_code = reason;
		}
	}
	pmlmepriv->LinkDetectInfo.bBusyTraffic = _FALSE;
	return _SUCCESS;

}

unsigned int OnAtim(_adapter *padapter, union recv_frame *precv_frame)
{
	RTW_INFO("%s\n", __FUNCTION__);
	return _SUCCESS;
}

unsigned int on_action_spct_ch_switch(_adapter *padapter, struct sta_info *psta, u8 *ies, uint ies_len)
{
	unsigned int ret = _FAIL;
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct mlme_ext_priv *mlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(mlmeext->mlmext_info);

	if (!(pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS)) {
		ret = _SUCCESS;
		goto exit;
	}

	if ((pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE) {
		enum band_type band;
		int ch_switch_mode = -1, ch = -1, ch_switch_cnt = -1;
		int ch_offset = -1;
		u8 bwmode;
		struct ieee80211_info_element *ie;

		RTW_INFO(FUNC_NDEV_FMT" from "MAC_FMT"\n",
			FUNC_NDEV_ARG(padapter->pnetdev), MAC_ARG(psta->phl_sta->mac_addr));

		for_each_ie(ie, ies, ies_len) {
			if (ie->id == WLAN_EID_CHANNEL_SWITCH) {
				ch_switch_mode = ie->data[0];
				ch = ie->data[1];
				ch_switch_cnt = ie->data[2];
				RTW_INFO("ch_switch_mode:%d, ch:%d, ch_switch_cnt:%d\n",
					 ch_switch_mode, ch, ch_switch_cnt);
			} else if (ie->id == WLAN_EID_SECONDARY_CHANNEL_OFFSET) {
				ch_offset = secondary_ch_offset_to_hal_ch_offset(ie->data[0]);
				RTW_INFO("ch_offset:%d\n", ch_offset);
			}
		}

		if (ch == -1)
			return _SUCCESS;

		band = padapter_link->mlmeextpriv.chandef.band;

		if (ch_offset == -1)
			bwmode = padapter_link->mlmeextpriv.chandef.bw;
		else
			bwmode = (ch_offset == CHAN_OFFSET_NO_EXT) ?
				 CHANNEL_WIDTH_20 : CHANNEL_WIDTH_40;

		ch_offset = (ch_offset == -1) ? padapter_link->mlmeextpriv.chandef.offset : ch_offset;

		/* todo:
		 * 1. the decision of channel switching
		 * 2. things after channel switching
		 */

		ret = rtw_set_chbw_cmd(padapter, padapter_link, band, ch, bwmode, ch_offset, 0);
	}

exit:
	return ret;
}

unsigned int on_action_spct(_adapter *padapter, union recv_frame *precv_frame)
{
	unsigned int ret = _FAIL;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint frame_len = precv_frame->u.hdr.len;
	u8 *frame_body = (u8 *)(pframe + sizeof(struct rtw_ieee80211_hdr_3addr));
	u8 category;
	u8 action;

	psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));

	if (!psta)
		goto exit;

	category = frame_body[0];
	if (category != RTW_WLAN_CATEGORY_SPECTRUM_MGMT)
		goto exit;

	action = frame_body[1];

	RTW_INFO(FUNC_ADPT_FMT" action:%u\n", FUNC_ADPT_ARG(padapter), action);

	switch (action) {
	case RTW_WLAN_ACTION_SPCT_MSR_REQ:
	case RTW_WLAN_ACTION_SPCT_MSR_RPRT:
	case RTW_WLAN_ACTION_SPCT_TPC_REQ:
	case RTW_WLAN_ACTION_SPCT_TPC_RPRT:
		break;
	case RTW_WLAN_ACTION_SPCT_CHL_SWITCH:
#ifdef CONFIG_SPCT_CH_SWITCH
		ret = on_action_spct_ch_switch(padapter, psta
				, frame_body + 2, frame_len - (frame_body - pframe) - 2);
#elif defined(CONFIG_ECSA_PHL)
		if (MLME_IS_STA(padapter) && MLME_IS_ASOC(padapter)) {
			process_csa_ie(padapter
				, frame_body + 2, frame_len - (frame_body - pframe) - 2);
		}
#endif
		break;
	default:
		break;
	}

exit:
	return ret;
}

unsigned int OnAction_qos(_adapter *padapter, union recv_frame *precv_frame)
{
	return _SUCCESS;
}

unsigned int OnAction_dls(_adapter *padapter, union recv_frame *precv_frame)
{
	return _SUCCESS;
}

#ifdef CONFIG_RTW_WNM
unsigned int on_action_wnm(_adapter *adapter, union recv_frame *rframe)
{
	unsigned int ret = _FAIL;
	struct sta_info *sta = NULL;
	struct mlme_priv *pmlmepriv = &(adapter->mlmepriv);
	struct sta_priv *stapriv = &(adapter->stapriv);
	u8 *frame = rframe->u.hdr.rx_data;
	u32 frame_len = rframe->u.hdr.len;
	u8 *frame_body = (u8 *)(frame + sizeof(struct rtw_ieee80211_hdr_3addr));
	u32 frame_body_len = frame_len - sizeof(struct rtw_ieee80211_hdr_3addr);	
	u8 category, action;
	int cnt = 0;
	char msg[16];

	sta = rtw_get_stainfo(stapriv, get_addr2_ptr(frame));
	if (!sta)
		goto exit;

	category = frame_body[0];
	if (category != RTW_WLAN_CATEGORY_WNM)
		goto exit;

	action = frame_body[1];

	switch (action) {
#ifdef CONFIG_RTW_80211R
	case RTW_WLAN_ACTION_WNM_BTM_REQ:
		if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) == _TRUE) {
			RTW_INFO("WNM: BSS Transition Management Request recv.\n");
			rtw_wnm_process_btm_req(adapter, frame_body, frame_body_len);
		}
		ret = _SUCCESS;
		break;
#endif		
	case RTW_WLAN_ACTION_WNM_BTM_RSP:
		if ((check_fwstate(pmlmepriv, WIFI_AP_STATE) == _TRUE) &&
			(pmlmepriv->nb_info.features & RTW_WNM_FEATURE_BTM_REQ_EN)) {
			struct btm_rsp_hdr rsp;
			u32 sz;

			RTW_INFO("WNM: BSS Transition Management Response recv.\n");
			sz = rtw_wnm_btm_rsp_candidates_sz_get(adapter,
					frame_body, frame_body_len);
			_rtw_memset(&rsp, 0, sizeof(rsp));

			if (sz > 0)
				rsp.pcandidates = rtw_zmalloc(sz);

			rtw_wnm_process_btm_rsp(adapter, frame_body, frame_body_len, &rsp);
			/* TODO : handle candidates info in rsp.pcandidates for upper-layer services */

			if (0 && rsp.pcandidates && (rsp.candidates_num > 0))
				RTW_INFO_DUMP("pcandidates : ", rsp.pcandidates, sz);

			if ((sz > 0) && (rsp.pcandidates != NULL))
				rtw_mfree(rsp.pcandidates, sz);
		}
		fallthrough;
	default:
		#ifdef CONFIG_RTW_MBO
		if (rtw_mbo_wifi_logo_test(adapter) \
			&& (check_fwstate(pmlmepriv, WIFI_AP_STATE) == _TRUE) \
			&& (action == RTW_WLAN_ACTION_WNM_BTM_QUERY)) {
			struct mbo_attr_info *pmbo_attr = \
				&(pmlmepriv->mbo_attr);

			RTW_INFO("%s: BSS Transition Management query"
				" recv from "MAC_FMT"\n", __func__,
				MAC_ARG(sta->phl_sta->mac_addr));
			pmbo_attr->user_raw.hdr.dialog_token = \
				wnm_btm_dialog_token(frame_body);
			pmbo_attr->user_raw.hdr.req_mode |= \
				PREFERRED_CANDIDATE_LIST_INCLUDED;
			pmbo_attr->user_raw.hdr.validity_interval = 30;
			rtw_wnm_process_btm_query(
				adapter, frame_body, frame_body_len);
			rtw_wnm_issue_btm_req(adapter, sta->phl_sta->mac_addr,
				&pmbo_attr->user_raw.hdr, NULL, 0,
				(u8 *)&pmbo_attr->user_raw.btm_cants,
				pmbo_attr->user_raw.candidate_cnt);
			break;
		}

		if (rtw_mbo_wifi_logo_test(adapter) \
			&& (check_fwstate(pmlmepriv, WIFI_AP_STATE) == _TRUE) \
			&& (action == RTW_WLAN_ACTION_WNM_NOTIF_REQ)) {
			struct mbo_attr_info *pmbo_attr = \
				&(pmlmepriv->mbo_attr);

			RTW_INFO("%s: Notification Request"
				" recv from "MAC_FMT"\n", __func__,
				MAC_ARG(sta->phl_sta->mac_addr));
			rtw_wnm_process_notification_req(adapter,
				frame_body, frame_body_len);
			break;
		}
		#endif /* CONFIG_RTW_MBO */
		#ifdef CONFIG_IOCTL_CFG80211
		cnt += sprintf((msg + cnt), "ACT_WNM %u", action);
		rtw_cfg80211_rx_action(adapter, rframe, msg);
		#endif
		ret = _SUCCESS;
		break;
	}

exit:
	return ret;
}
#endif /* CONFIG_RTW_WNM */

/**
 * rtw_rx_ampdu_size - Get the target RX AMPDU buffer size for the specific @adapter
 * @adapter: the adapter to get target RX AMPDU buffer size
 *
 * Returns: the target RX AMPDU buffer size
 */
u8 rtw_rx_ampdu_size(_adapter *adapter)
{
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	u8 size;
	HT_CAP_AMPDU_FACTOR max_rx_ampdu_factor = 0;

	/* for scan */
	if (!mlmeext_chk_scan_state(&adapter->mlmeextpriv, SCAN_DISABLE)
	    && !mlmeext_chk_scan_state(&adapter->mlmeextpriv, SCAN_COMPLETE)
	    && adapter->mlmeextpriv.sitesurvey_res.rx_ampdu_size != RX_AMPDU_SIZE_INVALID
	   ) {
		size = adapter->mlmeextpriv.sitesurvey_res.rx_ampdu_size;
		goto exit;
	}

	/* default value based on max_rx_ampdu_factor */
	if (adapter->driver_rx_ampdu_factor != 0xFF)
		max_rx_ampdu_factor = (HT_CAP_AMPDU_FACTOR)adapter->driver_rx_ampdu_factor;
	else
		rtw_hal_get_def_var(adapter, adapter_link, HW_VAR_MAX_RX_AMPDU_FACTOR, &max_rx_ampdu_factor);

	/* In Maximum A-MPDU Length Exponent subfield of A-MPDU Parameters field of HT Capabilities element,
		the unit of max_rx_ampdu_factor are octets. 8K, 16K, 32K, 64K is right.
		But the buffer size subfield of Block Ack Parameter Set field in ADDBA action frame indicates
		the number of buffers available for this particular TID. Each buffer is equal to max. size of
		MSDU or AMSDU.
		The size variable means how many MSDUs or AMSDUs, it's not Kbytes.
	*/
	if (MAX_AMPDU_FACTOR_64K == max_rx_ampdu_factor)
		size = 64;
	else if (MAX_AMPDU_FACTOR_32K == max_rx_ampdu_factor)
		size = 32;
	else if (MAX_AMPDU_FACTOR_16K == max_rx_ampdu_factor)
		size = 16;
	else if (MAX_AMPDU_FACTOR_8K == max_rx_ampdu_factor)
		size = 8;
	else
		size = 64;

exit:

	if (size > 127)
		size = 127;

	return size;
}

/**
 * rtw_rx_ampdu_is_accept - Get the permission if RX AMPDU should be set up for the specific @adapter
 * @adapter: the adapter to get the permission if RX AMPDU should be set up
 *
 * Returns: accept or not
 */
bool rtw_rx_ampdu_is_accept(_adapter *adapter)
{
	bool accept;

	if (adapter->fix_rx_ampdu_accept != RX_AMPDU_ACCEPT_INVALID) {
		accept = adapter->fix_rx_ampdu_accept;
		goto exit;
	}

	/* for scan */
	if (!mlmeext_chk_scan_state(&adapter->mlmeextpriv, SCAN_DISABLE)
	    && !mlmeext_chk_scan_state(&adapter->mlmeextpriv, SCAN_COMPLETE)
	    && adapter->mlmeextpriv.sitesurvey_res.rx_ampdu_accept != RX_AMPDU_ACCEPT_INVALID
	   ) {
		accept = adapter->mlmeextpriv.sitesurvey_res.rx_ampdu_accept;
		goto exit;
	}

	/* default value for other cases */
	accept = adapter->mlmeextpriv.mlmext_info.bAcceptAddbaReq;

exit:
	return accept;
}

/**
 * rtw_rx_ampdu_set_size - Set the target RX AMPDU buffer size for the specific @adapter and specific @reason
 * @adapter: the adapter to set target RX AMPDU buffer size
 * @size: the target RX AMPDU buffer size to set
 * @reason: reason for the target RX AMPDU buffer size setting
 *
 * Returns: whether the target RX AMPDU buffer size is changed
 */
bool rtw_rx_ampdu_set_size(_adapter *adapter, u8 size, u8 reason)
{
	bool is_adj = _FALSE;
	struct mlme_ext_priv *mlmeext;
	struct mlme_ext_info *mlmeinfo;

	mlmeext = &adapter->mlmeextpriv;
	mlmeinfo = &mlmeext->mlmext_info;

	if (reason == RX_AMPDU_DRV_FIXED) {
		if (adapter->fix_rx_ampdu_size != size) {
			adapter->fix_rx_ampdu_size = size;
			is_adj = _TRUE;
			RTW_INFO(FUNC_ADPT_FMT" fix_rx_ampdu_size:%u\n", FUNC_ADPT_ARG(adapter), size);
		}
	} else if (reason == RX_AMPDU_DRV_SCAN) {
		struct ss_res *ss = &adapter->mlmeextpriv.sitesurvey_res;

		if (ss->rx_ampdu_size != size) {
			ss->rx_ampdu_size = size;
			is_adj = _TRUE;
			RTW_INFO(FUNC_ADPT_FMT" ss.rx_ampdu_size:%u\n", FUNC_ADPT_ARG(adapter), size);
		}
	}

	return is_adj;
}

/**
 * rtw_rx_ampdu_set_accept - Set the permission if RX AMPDU should be set up for the specific @adapter and specific @reason
 * @adapter: the adapter to set if RX AMPDU should be set up
 * @accept: if RX AMPDU should be set up
 * @reason: reason for the permission if RX AMPDU should be set up
 *
 * Returns: whether the permission if RX AMPDU should be set up is changed
 */
bool rtw_rx_ampdu_set_accept(_adapter *adapter, u8 accept, u8 reason)
{
	bool is_adj = _FALSE;
	struct mlme_ext_priv *mlmeext;
	struct mlme_ext_info *mlmeinfo;

	mlmeext = &adapter->mlmeextpriv;
	mlmeinfo = &mlmeext->mlmext_info;

	if (reason == RX_AMPDU_DRV_FIXED) {
		if (adapter->fix_rx_ampdu_accept != accept) {
			adapter->fix_rx_ampdu_accept = accept;
			is_adj = _TRUE;
			RTW_INFO(FUNC_ADPT_FMT" fix_rx_ampdu_accept:%u\n", FUNC_ADPT_ARG(adapter), accept);
		}
	} else if (reason == RX_AMPDU_DRV_SCAN) {
		if (adapter->mlmeextpriv.sitesurvey_res.rx_ampdu_accept != accept) {
			adapter->mlmeextpriv.sitesurvey_res.rx_ampdu_accept = accept;
			is_adj = _TRUE;
			RTW_INFO(FUNC_ADPT_FMT" ss.rx_ampdu_accept:%u\n", FUNC_ADPT_ARG(adapter), accept);
		}
	}

	return is_adj;
}

/**
 * rx_ampdu_apply_sta_tid - Apply RX AMPDU setting to the specific @sta and @tid
 * @adapter: the adapter to which @sta belongs
 * @sta: the sta to be checked
 * @tid: the tid to be checked
 * @accept: the target permission if RX AMPDU should be set up
 * @size: the target RX AMPDU buffer size
 *
 * Returns:
 * 0: no canceled
 * 1: canceled by no permission
 * 2: canceled by different buffer size
 * 3: canceled by potential mismatched status
 *
 * Blocking function, may sleep
 */
u8 rx_ampdu_apply_sta_tid(_adapter *adapter, struct sta_info *sta, u8 tid, u8 accept, u8 size)
{
	u8 ret = 0;
	struct recv_reorder_ctrl *reorder_ctl = &sta->recvreorder_ctrl[tid];

	if (reorder_ctl->enable == _FALSE) {
		if (reorder_ctl->ampdu_size != RX_AMPDU_SIZE_INVALID) {
			send_delba_sta_tid_wait_ack(adapter, 0, sta, tid, 1);
			ret = 3;
		}
		goto exit;
	}

	if (accept == _FALSE) {
		send_delba_sta_tid_wait_ack(adapter, 0, sta, tid, 0);
		ret = 1;
	} else if (reorder_ctl->ampdu_size != size) {
		send_delba_sta_tid_wait_ack(adapter, 0, sta, tid, 0);
		ret = 2;
	}

exit:
	return ret;
}

u8 rx_ampdu_size_sta_limit(_adapter *adapter, struct sta_info *sta)
{
	u8 sz_limit = 0xFF;

#ifdef CONFIG_80211N_HT
	struct registry_priv *regsty = &adapter->registrypriv;
	struct _ADAPTER_LINK *adapter_link = sta->padapter_link;
	struct link_mlme_priv *mlme = &adapter_link->mlmepriv;
	struct link_mlme_ext_priv *mlmeext = &adapter_link->mlmeextpriv;
	struct link_mlme_ext_info *mlmeinfo = &mlmeext->mlmext_info;
	s8 nss = -1;
	u8 bw = rtw_min(sta->phl_sta->chandef.bw, mlmeext->chandef.bw);

	#ifdef CONFIG_80211AC_VHT
	#ifdef CONFIG_80211AX_HE
	/* CONFIG_80211AX_HE_TODO */
	#endif
	if (is_supported_vht(sta->phl_sta->wmode)) {
		nss = rtw_min(rtw_vht_mcsmap_to_nss(mlme->vhtpriv.vht_mcs_map)
				, rtw_vht_mcsmap_to_nss(sta->vhtpriv.vht_mcs_map));
	} else
	#endif
	if (is_supported_ht(sta->phl_sta->wmode)) {
		nss = rtw_min(rtw_ht_mcsset_to_nss(mlmeinfo->HT_caps.u.HT_cap_element.MCS_rate)
				, rtw_ht_mcsset_to_nss(sta->htpriv.ht_cap.supp_mcs_set));
	}

	if (nss >= 1)
		sz_limit = regsty->rx_ampdu_sz_limit_by_nss_bw[nss - 1][bw];
#endif /* CONFIG_80211N_HT */

	return sz_limit;
}

/**
 * rx_ampdu_apply_sta - Apply RX AMPDU setting to the specific @sta
 * @adapter: the adapter to which @sta belongs
 * @sta: the sta to be checked
 * @accept: the target permission if RX AMPDU should be set up
 * @size: the target RX AMPDU buffer size
 *
 * Returns: number of the RX AMPDU assciation canceled for applying current target setting
 *
 * Blocking function, may sleep
 */
u8 rx_ampdu_apply_sta(_adapter *adapter, struct sta_info *sta, u8 accept, u8 size)
{
	u8 change_cnt = 0;
	int i;

	for (i = 0; i < TID_NUM; i++) {
		if (rx_ampdu_apply_sta_tid(adapter, sta, i, accept, size) != 0)
			change_cnt++;
	}

	return change_cnt;
}

/**
 * rtw_rx_ampdu_apply - Apply the current target RX AMPDU setting for the specific @adapter
 * @adapter: the adapter to be applied
 *
 * Returns: number of the RX AMPDU assciation canceled for applying current target setting
 */
u16 rtw_rx_ampdu_apply(_adapter *adapter)
{
	u16 adj_cnt = 0;
	struct sta_info *sta;
	u8 accept = rtw_rx_ampdu_is_accept(adapter);
	u8 size;

	if (adapter->fix_rx_ampdu_size != RX_AMPDU_SIZE_INVALID)
		size = adapter->fix_rx_ampdu_size;
	else
		size = rtw_rx_ampdu_size(adapter);

	if (MLME_IS_STA(adapter)) {
		sta = rtw_get_stainfo(&adapter->stapriv, get_bssid(&adapter->mlmepriv));
		if (sta) {
			u8 sta_size = size;

			if (adapter->fix_rx_ampdu_size == RX_AMPDU_SIZE_INVALID)
				sta_size = rtw_min(size, rx_ampdu_size_sta_limit(adapter, sta));
			adj_cnt += rx_ampdu_apply_sta(adapter, sta, accept, sta_size);
		}
		/* TODO: TDLS peer */

	} else if (MLME_IS_AP(adapter) || MLME_IS_MESH(adapter)) {
		_list *phead, *plist;
		u8 peer_num = 0;
		char peers[NUM_STA];
		struct sta_priv *pstapriv = &adapter->stapriv;
		int i;

		_rtw_spinlock_bh(&pstapriv->asoc_list_lock);

		phead = &pstapriv->asoc_list;
		plist = get_next(phead);

		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			int stainfo_offset;

			sta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
			plist = get_next(plist);

			stainfo_offset = rtw_stainfo_offset(pstapriv, sta);
			if (stainfo_offset_valid(stainfo_offset))
				peers[peer_num++] = stainfo_offset;
		}

		_rtw_spinunlock_bh(&pstapriv->asoc_list_lock);

		for (i = 0; i < peer_num; i++) {
			sta = rtw_get_stainfo_by_offset(pstapriv, peers[i]);
			if (sta) {
				u8 sta_size = size;

				if (adapter->fix_rx_ampdu_size == RX_AMPDU_SIZE_INVALID)
					sta_size = rtw_min(size, rx_ampdu_size_sta_limit(adapter, sta));
				adj_cnt += rx_ampdu_apply_sta(adapter, sta, accept, sta_size);
			}
		}
	}

	/* TODO: ADHOC */

	return adj_cnt;
}

unsigned int OnAction_back(_adapter *padapter, union recv_frame *precv_frame)
{
	u8 *addr;
	struct sta_info *psta = NULL;
	struct recv_reorder_ctrl *preorder_ctrl;
	unsigned char		*frame_body;
	unsigned char		category, action;
	unsigned short	tid, status, reason_code = 0;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct registry_priv *pregpriv = &padapter->registrypriv;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct _ADAPTER_LINK *padapter_link = NULL;

#ifdef CONFIG_80211N_HT

	RTW_INFO("%s\n", __FUNCTION__);

	/* check RA matches or not	 */
	if (!rtw_match_self_addr(padapter, GetAddr1Ptr(pframe)))
		return _SUCCESS;

#if 0
	/* check A1 matches or not */
	if (!_rtw_memcmp(adapter_mac_addr(padapter), get_da(pframe), ETH_ALEN))
		return _SUCCESS;
#endif

	if ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)
		if (!(pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS))
			return _SUCCESS;

	psta = precv_frame->u.hdr.psta;
	addr = psta->phl_sta->mac_addr;
	padapter_link = psta->padapter_link;

	if (psta == NULL)
		return _SUCCESS;

	frame_body = (unsigned char *)(pframe + sizeof(struct rtw_ieee80211_hdr_3addr));

	category = frame_body[0];
	if (category == RTW_WLAN_CATEGORY_BACK) { /* representing Block Ack */
#ifdef CONFIG_TDLS
		if ((psta->tdls_sta_state & TDLS_LINKED_STATE) &&
		    (psta->htpriv.ht_option == _TRUE
#if CONFIG_IEEE80211_BAND_6GHZ
		     || psta->phl_sta->chandef.band == BAND_ON_6G
#endif
		    ) && (psta->ampdu_priv.ampdu_enable == _TRUE))
			RTW_INFO("Recv [%s] from direc link\n", __FUNCTION__);
		else
#endif /* CONFIG_TDLS */
			if (!padapter_link->mlmeextpriv.mlmext_info.HT_enable
#if CONFIG_IEEE80211_BAND_6GHZ
			    && psta->phl_sta->chandef.band != BAND_ON_6G
#endif
			   )
				return _SUCCESS;

		action = frame_body[1];
		RTW_INFO("%s, action=%d\n", __FUNCTION__, action);
		switch (action) {
		case RTW_WLAN_ACTION_ADDBA_REQ: /* ADDBA request */

			/* process_addba_req(padapter, (u8*)&(pmlmeinfo->ADDBA_req), GetAddr3Ptr(pframe)); */
			_rtw_memcpy(&(pmlmeinfo->ADDBA_req), &(frame_body[2]), sizeof(struct ADDBA_request));
			process_addba_req(padapter, (u8 *)&(pmlmeinfo->ADDBA_req), addr);

			break;

		case RTW_WLAN_ACTION_ADDBA_RESP: /* ADDBA response */

			/* status = frame_body[3] | (frame_body[4] << 8); */ /* endian issue */
			status = RTW_GET_LE16(&frame_body[3]);
			tid = ((frame_body[5] >> 2) & 0x7);
			if (status == 0) {
				/* successful					 */
				RTW_INFO("agg_enable for TID=%d\n", tid);
				psta->ampdu_priv.agg_enable_bitmap |= 1 << tid;
				psta->ampdu_priv.candidate_tid_bitmap &= ~BIT(tid);
				/* amsdu in ampdu */
				if (pregpriv->tx_ampdu_amsdu == 0)
					psta->ampdu_priv.tx_amsdu_enable = _FALSE;
				else if (pregpriv->tx_ampdu_amsdu == 1)
					psta->ampdu_priv.tx_amsdu_enable = _TRUE;
				else {
					if (frame_body[5] & 1)
						psta->ampdu_priv.tx_amsdu_enable = _TRUE;
				}
			} else
				psta->ampdu_priv.agg_enable_bitmap &= ~BIT(tid);

			if (psta->state & WIFI_STA_ALIVE_CHK_STATE) {
				RTW_INFO("%s alive check - rx ADDBA response\n", __func__);
				psta->ampdu_priv.agg_enable_bitmap &= ~BIT(tid);
				psta->expire_to = pstapriv->expire_to;
				psta->state ^= WIFI_STA_ALIVE_CHK_STATE;
			}

			/* RTW_INFO("marc: ADDBA RSP: %x\n", pmlmeinfo->agg_enable_bitmap); */
			break;

		case RTW_WLAN_ACTION_DELBA: /* DELBA */
			if ((frame_body[3] & BIT(3)) == 0) {
				psta->ampdu_priv.agg_enable_bitmap &= ~(1 << ((frame_body[3] >> 4) & 0xf));
				psta->ampdu_priv.candidate_tid_bitmap &= ~(1 << ((frame_body[3] >> 4) & 0xf));

				/* reason_code = frame_body[4] | (frame_body[5] << 8); */
				reason_code = RTW_GET_LE16(&frame_body[4]);
			} else if ((frame_body[3] & BIT(3)) == BIT(3)) {
				tid = (frame_body[3] >> 4) & 0x0F;

				preorder_ctrl = &psta->recvreorder_ctrl[tid];
				preorder_ctrl->enable = _FALSE;
				preorder_ctrl->ampdu_size = RX_AMPDU_SIZE_INVALID;
				rtw_delba_cmd(padapter, addr, tid);
			}

			RTW_INFO("%s(): DELBA: %x(%x)\n", __FUNCTION__, pmlmeinfo->agg_enable_bitmap, reason_code);
			/* todo: how to notify the host while receiving DELETE BA */
			break;

		default:
			break;
		}
	}
#endif /* CONFIG_80211N_HT */
	return _SUCCESS;
}

#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
u32 rtw_build_vendor_ie(_adapter *padapter , unsigned char **pframe , u8 mgmt_frame_tyte)
{
	int vendor_ie_num = 0;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u32 len = 0;

	for (vendor_ie_num = 0 ; vendor_ie_num < WLAN_MAX_VENDOR_IE_NUM ; vendor_ie_num++) {
		if (pmlmepriv->vendor_ielen[vendor_ie_num] > 0 && pmlmepriv->vendor_ie_mask[vendor_ie_num] & mgmt_frame_tyte) {
			_rtw_memcpy(*pframe , pmlmepriv->vendor_ie[vendor_ie_num] , pmlmepriv->vendor_ielen[vendor_ie_num]);
			*pframe +=  pmlmepriv->vendor_ielen[vendor_ie_num];
			len += pmlmepriv->vendor_ielen[vendor_ie_num];
		}
	}

	return len;
}
#endif

#ifdef CONFIG_ECSA_PHL
u8 *build_supported_op_class_ie(_adapter *padapter, u8 *pbuf, int *pktlen)
{
	u8 buf[32];
	int ie_len = 0;

	ie_len = get_supported_op_class(padapter, buf, sizeof(buf));
	pbuf = rtw_set_ie(pbuf, WLAN_EID_SUPPORT_OP_CLASS , ie_len,
			    (unsigned char *)buf, pktlen);
	return pbuf;
}
#endif /* CONFIG_ECSA_PHL */

#ifdef CONFIG_P2P
void issue_probersp_p2p(_adapter *padapter, unsigned char *da)
{
	struct xmit_frame			*pmgntframe;
	struct pkt_attrib			*pattrib;
	unsigned char					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short				*fctrl;
	unsigned char					*mac;
	struct xmit_priv	*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	/* WLAN_BSSID_EX 		*cur_network = &(pmlmeinfo->network); */
	u16					beacon_interval = 100;
	u16					capInfo = 0;
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	u8					wpsie[255] = { 0x00 };
	u32					wpsielen = 0, p2pielen = 0;
#ifdef CONFIG_WFD
	u32					wfdielen = 0;
#endif
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	/* RTW_INFO("%s\n", __FUNCTION__); */

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	if (IS_CCK_RATE(pattrib->rate)) {
		/* force OFDM 6M rate */
		pattrib->rate = MGN_6M;
	}

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	mac = padapter_link->mac_addr;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;
	_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, mac, ETH_ALEN);

	/*	Use the device address for BSSID field.	 */
	_rtw_memcpy(pwlanhdr->addr3, mac, ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(fctrl, WIFI_PROBERSP);

	pattrib->hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = pattrib->hdrlen;
	pframe += pattrib->hdrlen;

	/* timestamp will be inserted by hardware */
	pframe += 8;
	pattrib->pktlen += 8;

	/* beacon interval: 2 bytes */
	_rtw_memcpy(pframe, (unsigned char *) &beacon_interval, 2);
	pframe += 2;
	pattrib->pktlen += 2;

	/*	capability info: 2 bytes */
	/*	ESS and IBSS bits must be 0 (defined in the 3.1.2.1.1 of WiFi Direct Spec) */
	capInfo |= cap_ShortPremble;
	capInfo |= cap_ShortSlot;

	_rtw_memcpy(pframe, (unsigned char *) &capInfo, 2);
	pframe += 2;
	pattrib->pktlen += 2;


	/* SSID */
	pframe = rtw_set_ie(pframe, _SSID_IE_, 7, pwdinfo->p2p_wildcard_ssid, &pattrib->pktlen);

	/* supported rates... */
	/*	Use the OFDM rate in the P2P probe response frame. ( 6(B), 9(B), 12, 18, 24, 36, 48, 54 ) */
	pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_, 8, pwdinfo->support_rate, &pattrib->pktlen);

	/* DS parameter set */
	pframe = rtw_set_ie(pframe, _DSSET_IE_, 1, (unsigned char *)&pwdinfo->listen_channel, &pattrib->pktlen);

#ifdef CONFIG_IOCTL_CFG80211
	if (adapter_wdev_data(padapter)->p2p_enabled) {
		if (pmlmepriv->wps_probe_resp_ie != NULL && pmlmepriv->p2p_probe_resp_ie != NULL) {
			/* WPS IE */
			_rtw_memcpy(pframe, pmlmepriv->wps_probe_resp_ie, pmlmepriv->wps_probe_resp_ie_len);
			pattrib->pktlen += pmlmepriv->wps_probe_resp_ie_len;
			pframe += pmlmepriv->wps_probe_resp_ie_len;

			/* P2P IE */
			_rtw_memcpy(pframe, pmlmepriv->p2p_probe_resp_ie, pmlmepriv->p2p_probe_resp_ie_len);
			pattrib->pktlen += pmlmepriv->p2p_probe_resp_ie_len;
			pframe += pmlmepriv->p2p_probe_resp_ie_len;
		}
	}
#endif /* CONFIG_IOCTL_CFG80211		 */

#ifdef CONFIG_WFD
	wfdielen = rtw_append_probe_resp_wfd_ie(padapter, pframe);
	pframe += wfdielen;
	pattrib->pktlen += wfdielen;
#endif

/* Vendor Specific IE */
#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
	pattrib->pktlen += rtw_build_vendor_ie(padapter , &pframe , WIFI_P2P_PROBERESP_VENDOR_IE_BIT);
#endif

	pattrib->last_txcmdsz = pattrib->pktlen;


	dump_mgntframe(padapter, pmgntframe);

	return;

}

int _issue_probereq_p2p(_adapter *padapter, u8 *da, int wait_ack)
{
	int ret = _FAIL;
	struct xmit_frame		*pmgntframe;
	struct pkt_attrib		*pattrib;
	unsigned char			*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short		*fctrl;
	unsigned char			*mac;
	struct xmit_priv		*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	u8	bc_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	u8					wpsie[255] = { 0x00 }, p2pie[255] = { 0x00 };
	u16					wpsielen = 0, p2pielen = 0;
#ifdef CONFIG_WFD
	u32					wfdielen = 0;
#endif
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);


	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		goto exit;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	if (IS_CCK_RATE(pattrib->rate)) {
		/* force OFDM 6M rate */
		pattrib->rate = MGN_6M;
	}

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	mac = padapter_link->mac_addr;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	if (da) {
		_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, da, ETH_ALEN);
	} else {
		/*	broadcast probe request frame */
		_rtw_memcpy(pwlanhdr->addr1, bc_addr, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, bc_addr, ETH_ALEN);
	}
	_rtw_memcpy(pwlanhdr->addr2, mac, ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_PROBEREQ);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	pframe = rtw_set_ie(pframe, _SSID_IE_, P2P_WILDCARD_SSID_LEN, pwdinfo->p2p_wildcard_ssid, &(pattrib->pktlen));

	/*	Use the OFDM rate in the P2P probe request frame. ( 6(B), 9(B), 12(B), 24(B), 36, 48, 54 ) */
	pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_, 8, pwdinfo->support_rate, &pattrib->pktlen);

#ifdef CONFIG_IOCTL_CFG80211
	if (adapter_wdev_data(padapter)->p2p_enabled) {
		if (pmlmepriv->wps_probe_req_ie != NULL && pmlmepriv->p2p_probe_req_ie != NULL) {
			/* WPS IE */
			_rtw_memcpy(pframe, pmlmepriv->wps_probe_req_ie, pmlmepriv->wps_probe_req_ie_len);
			pattrib->pktlen += pmlmepriv->wps_probe_req_ie_len;
			pframe += pmlmepriv->wps_probe_req_ie_len;

			/* P2P IE */
			_rtw_memcpy(pframe, pmlmepriv->p2p_probe_req_ie, pmlmepriv->p2p_probe_req_ie_len);
			pattrib->pktlen += pmlmepriv->p2p_probe_req_ie_len;
			pframe += pmlmepriv->p2p_probe_req_ie_len;
		}
	}
#endif /* CONFIG_IOCTL_CFG80211 */

#ifdef CONFIG_WFD
	wfdielen = rtw_append_probe_req_wfd_ie(padapter, pframe);
	pframe += wfdielen;
	pattrib->pktlen += wfdielen;
#endif

/* Vendor Specific IE */
#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
	pattrib->pktlen += rtw_build_vendor_ie(padapter , &pframe , WIFI_P2P_PROBEREQ_VENDOR_IE_BIT);
#endif

	pattrib->last_txcmdsz = pattrib->pktlen;


	if (wait_ack)
		ret = dump_mgntframe_and_wait_ack(padapter, pmgntframe);
	else {
		dump_mgntframe(padapter, pmgntframe);
		ret = _SUCCESS;
	}

exit:
	return ret;
}

inline void issue_probereq_p2p(_adapter *adapter, u8 *da)
{
	_issue_probereq_p2p(adapter, da, _FALSE);
}

#endif /* CONFIG_P2P */

s32 rtw_action_public_decache(union recv_frame *rframe, u8 token_offset)
{
	_adapter *adapter = rframe->u.hdr.adapter;
	struct mlme_ext_priv *mlmeext = &(adapter->mlmeextpriv);
	u8 *frame = rframe->u.hdr.rx_data;
	u16 seq_ctrl = ((rframe->u.hdr.attrib.seq_num & 0xffff) << 4) | (rframe->u.hdr.attrib.frag_num & 0xf);
	u8 token = *(rframe->u.hdr.rx_data + sizeof(struct rtw_ieee80211_hdr_3addr) + token_offset);

	if (GetRetry(frame)) {
		if ((seq_ctrl == mlmeext->action_public_rxseq)
		    && (token == mlmeext->action_public_dialog_token)
		   ) {
			RTW_INFO(FUNC_ADPT_FMT" seq_ctrl=0x%x, rxseq=0x%x, token:%d\n",
				FUNC_ADPT_ARG(adapter), seq_ctrl, mlmeext->action_public_rxseq, token);
			return _FAIL;
		}
	}

	/* TODO: per sta seq & token */
	mlmeext->action_public_rxseq = seq_ctrl;
	mlmeext->action_public_dialog_token = token;

	return _SUCCESS;
}

unsigned int on_action_public_p2p(union recv_frame *precv_frame)
{
	_adapter *padapter = precv_frame->u.hdr.adapter;

#ifdef CONFIG_P2P
#if defined(RTW_DEDICATED_P2P_DEVICE) || !RTW_P2P_GROUP_INTERFACE
	if (!adapter_wdev_data(padapter)->p2p_enabled) {
		RTW_INFO(FUNC_ADPT_FMT": failed to receive p2p action frame\n",
			FUNC_ADPT_ARG(padapter));
		return _FAIL;
	} else
#endif
	{
		rtw_cfg80211_rx_p2p_action_public(padapter, precv_frame);
	}
#endif /* CONFIG_P2P */

	return _SUCCESS;
}

unsigned int on_action_public_vendor(union recv_frame *precv_frame)
{
	unsigned int ret = _FAIL;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	u8 *frame_body = pframe + sizeof(struct rtw_ieee80211_hdr_3addr);
	_adapter *adapter = precv_frame->u.hdr.adapter;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	int cnt = 0;
	char msg[64];

	if (_rtw_memcmp(frame_body + 2, P2P_OUI, 4) == _TRUE) {
		if (rtw_action_public_decache(precv_frame, 7) == _FAIL)
			goto exit;

		if (!rtw_hw_chk_wl_func(dvobj, WL_FUNC_MIRACAST))
			rtw_rframe_del_wfd_ie(precv_frame, 8);

		ret = on_action_public_p2p(precv_frame);
	} else if (_rtw_memcmp(frame_body + 2, DPP_OUI, 4) == _TRUE) {
		u8 dpp_type = frame_body[7];

#ifdef CONFIG_IOCTL_CFG80211
		cnt += sprintf((msg + cnt), "DPP(type:%u)", dpp_type);
		rtw_cfg80211_rx_action(adapter, precv_frame, msg);
#endif
	}

exit:
	return ret;
}

unsigned int on_action_public_default(union recv_frame *precv_frame, u8 action)
{
	unsigned int ret = _FAIL;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	u8 *frame_body = pframe + sizeof(struct rtw_ieee80211_hdr_3addr);
	u8 token;
	_adapter *adapter = precv_frame->u.hdr.adapter;
	int cnt = 0;
	char msg[64];

	token = frame_body[2];

	if (rtw_action_public_decache(precv_frame, 2) == _FAIL)
		goto exit;

#ifdef CONFIG_IOCTL_CFG80211
	cnt += sprintf((msg + cnt), "%s(token:%u)", action_public_str(action), token);
	rtw_cfg80211_rx_action(adapter, precv_frame, msg);
#endif

	ret = _SUCCESS;

exit:
	return ret;
}

unsigned int on_action_public(_adapter *padapter, union recv_frame *precv_frame)
{
	unsigned int ret = _FAIL;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint frame_len = precv_frame->u.hdr.len;
	u8 *frame_body = pframe + sizeof(struct rtw_ieee80211_hdr_3addr);
	u8 category, action;

	/* check RA matches or broadcast */
	if (!(_rtw_memcmp(adapter_mac_addr(padapter), GetAddr1Ptr(pframe), ETH_ALEN) ||
		is_broadcast_mac_addr(GetAddr1Ptr(pframe))))
		goto exit;

	category = frame_body[0];
	if (category != RTW_WLAN_CATEGORY_PUBLIC)
		goto exit;

	action = frame_body[1];
	switch (action) {
	case ACT_PUBLIC_BSSCOEXIST:
#ifdef CONFIG_80211N_HT
#ifdef CONFIG_AP_MODE
		/*20/40 BSS Coexistence Management frame is a Public Action frame*/
		if (check_fwstate(&padapter->mlmepriv, WIFI_AP_STATE) == _TRUE)
			rtw_process_public_act_bsscoex(padapter, pframe, frame_len);
#endif /*CONFIG_AP_MODE*/
#endif /*CONFIG_80211N_HT*/
		break;
	case ACT_PUBLIC_VENDOR:
		ret = on_action_public_vendor(precv_frame);
		break;
	default:
		ret = on_action_public_default(precv_frame, action);
		break;
	}

exit:
	return ret;
}

unsigned int OnAction_ft(_adapter *padapter, union recv_frame *precv_frame)
{
#ifdef CONFIG_RTW_80211R
	u32	ret = _FAIL, frame_len = 0;
	u8	action_code = 0, category = 0;
	u8	*pframe = NULL, *pframe_body = NULL, *pie = NULL;
	u8 	tgt_addr[ETH_ALEN], msg[32] = {0};
	u32	ft_ie_len = 0;
	u32 status_code = 0;
	struct mlme_ext_priv *pmlmeext = NULL;
	struct mlme_ext_info *pmlmeinfo = NULL;
	struct mlme_priv *pmlmepriv = NULL;
	struct wlan_network *proam_target = NULL;
	struct ft_roam_info *pft_roam = NULL;
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;
	struct link_mlme_priv *lmlmepriv = &(padapter_link->mlmepriv);

	pmlmeext = &(padapter->mlmeextpriv);
	pmlmeinfo = &(pmlmeext->mlmext_info);
	pmlmepriv = &(padapter->mlmepriv);
	pft_roam = &(pmlmepriv->ft_roam);
	pframe = precv_frame->u.hdr.rx_data;
	frame_len = precv_frame->u.hdr.len;
	pframe_body = pframe + sizeof(struct rtw_ieee80211_hdr_3addr);
	category = pframe_body[0];

	if (category != RTW_WLAN_CATEGORY_FT)
		goto exit;

	action_code = pframe_body[1];
	switch (action_code) {
	case RTW_WLAN_ACTION_FT_RSP:
		RTW_INFO("FT: RTW_WLAN_ACTION_FT_RSP recv.\n");
		if (!_rtw_memcmp(adapter_mac_addr(padapter), &pframe_body[2], ETH_ALEN)) {
			RTW_ERR("FT: Unmatched STA MAC Address "MAC_FMT"\n", MAC_ARG(&pframe_body[2]));
			goto exit;
		}

		status_code = le16_to_cpu(*(u16 *)((SIZE_PTR)pframe +  sizeof(struct rtw_ieee80211_hdr_3addr) + 14));
		if (status_code != 0) {
			RTW_ERR("FT: WLAN ACTION FT RESPONSE fail, status: %d\n", status_code);
			goto exit;
		}

		_rtw_memcpy(tgt_addr, &pframe_body[8], ETH_ALEN);
		if (is_zero_mac_addr(tgt_addr) || is_broadcast_mac_addr(tgt_addr)) {
			RTW_ERR("FT: Invalid Target MAC Address "MAC_FMT"\n", MAC_ARG(tgt_addr));
			goto exit;
		}

		pie = rtw_get_ie(pframe_body, _MDIE_, &ft_ie_len, frame_len);
		if (pie) {
			if (!_rtw_memcmp(&pft_roam->mdid, pie+2, 2)) {
				RTW_ERR("FT: Invalid MDID\n");
				goto exit;
			}
		}

		rtw_ft_set_status(padapter, RTW_FT_REQUESTED_STA);
		_cancel_timer_ex(&pmlmeext->ft_link_timer);

		/*Disconnect current AP*/
		receive_disconnect(padapter, lmlmepriv->cur_network.network.MacAddress, WLAN_REASON_ACTIVE_ROAM, _FALSE);
		pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
		pmlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_FT;
		pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;

		pft_roam->ft_action_len = frame_len;
		_rtw_memcpy(pft_roam->ft_action, pframe, rtw_min(frame_len, RTW_FT_MAX_IE_SZ));
		ret = _SUCCESS;
		break;
	case RTW_WLAN_ACTION_FT_REQ:
	case RTW_WLAN_ACTION_FT_CONF:
	case RTW_WLAN_ACTION_FT_ACK:
	#ifdef CONFIG_IOCTL_CFG80211
		sprintf(msg, "ACT_FT %u", action_code);
		rtw_cfg80211_rx_action(padapter, precv_frame, msg);
	#endif
		break;
	default:
		RTW_ERR("FT: Unsupported FT Action!\n");
		break;
	}

exit:
	return ret;
#else
	return _SUCCESS;
#endif
}

unsigned int OnAction_ht(_adapter *padapter, union recv_frame *precv_frame)
{
	u8 *pframe = precv_frame->u.hdr.rx_data;
	u8 *frame_body = pframe + sizeof(struct rtw_ieee80211_hdr_3addr);
	u8 category, action;

	/* check RA matches or not */
	if (!_rtw_memcmp(adapter_mac_addr(padapter), GetAddr1Ptr(pframe), ETH_ALEN))
		goto exit;

	category = frame_body[0];
	if (category != RTW_WLAN_CATEGORY_HT)
		goto exit;

	action = frame_body[1];
	switch (action) {
	case RTW_WLAN_ACTION_HT_SM_PS:
#ifdef CONFIG_80211N_HT
#ifdef CONFIG_AP_MODE
		if (check_fwstate(&padapter->mlmepriv, WIFI_AP_STATE) == _TRUE)
			rtw_process_ht_action_smps(padapter, get_addr2_ptr(pframe), frame_body[2]);
#endif /*CONFIG_AP_MODE*/
#endif /*CONFIG_80211N_HT*/
		break;
	case RTW_WLAN_ACTION_HT_COMPRESS_BEAMFORMING:
#ifdef CONFIG_BEAMFORMING
		/*RTW_INFO("RTW_WLAN_ACTION_HT_COMPRESS_BEAMFORMING\n");*/
		/*rtw_beamforming_get_report_frame(padapter, precv_frame);*/
#endif /*CONFIG_BEAMFORMING*/
		break;
	default:
		break;
	}

exit:

	return _SUCCESS;
}

#ifdef CONFIG_IEEE80211W
unsigned int OnAction_sa_query(_adapter *padapter, union recv_frame *precv_frame)
{
	u8 *pframe = precv_frame->u.hdr.rx_data;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct sta_info		*psta;
	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
	u16 tid;
	struct _ADAPTER_LINK *padapter_link = precv_frame->u.hdr.adapter_link;
	/* Baron */

	RTW_INFO("OnAction_sa_query\n");

	switch (pframe[WLAN_HDR_A3_LEN + 1]) {
	case 0: /* SA Query req */
		_rtw_memcpy(&tid, &pframe[WLAN_HDR_A3_LEN + 2], sizeof(u16));
		RTW_INFO("OnAction_sa_query request,action=%d, tid=%04x, pframe=%02x-%02x\n"
			, pframe[WLAN_HDR_A3_LEN + 1], tid, pframe[WLAN_HDR_A3_LEN + 2], pframe[WLAN_HDR_A3_LEN + 3]);
		issue_action_SA_Query(padapter, padapter_link,
					get_addr2_ptr(pframe), 1, tid, IEEE80211W_RIGHT_KEY);
		break;

	case 1: /* SA Query rsp */
		psta = rtw_get_stainfo(pstapriv, get_addr2_ptr(pframe));
		if (psta != NULL)
			_cancel_timer_ex(&psta->dot11w_expire_timer);

		_rtw_memcpy(&tid, &pframe[WLAN_HDR_A3_LEN + 2], sizeof(u16));
		RTW_INFO("OnAction_sa_query response,action=%d, tid=%04x, cancel timer\n", pframe[WLAN_HDR_A3_LEN + 1], tid);
		break;
	default:
		break;
	}
	if (0) {
		int pp;
		printk("pattrib->pktlen = %d =>", pattrib->pkt_len);
		for (pp = 0; pp < pattrib->pkt_len; pp++)
			printk(" %02x ", pframe[pp]);
		printk("\n");
	}

	return _SUCCESS;
}
#endif /* CONFIG_IEEE80211W */

unsigned int on_action_rm(_adapter *padapter, union recv_frame *precv_frame)
{
#ifdef CONFIG_RTW_80211K
	return rm_on_action(padapter, precv_frame);
#else
	return _SUCCESS;
#endif  /* CONFIG_RTW_80211K */
}

unsigned int OnAction_wmm(_adapter *padapter, union recv_frame *precv_frame)
{
	return _SUCCESS;
}

unsigned int OnAction_vht(_adapter *padapter, union recv_frame *precv_frame)
{
#ifdef CONFIG_80211AC_VHT
	u8 *pframe = precv_frame->u.hdr.rx_data;
	struct rtw_ieee80211_hdr_3addr *whdr = (struct rtw_ieee80211_hdr_3addr *)pframe;
	u8 *frame_body = pframe + sizeof(struct rtw_ieee80211_hdr_3addr);
	u8 category, action;
	struct sta_info *psta = NULL;

	/* check RA matches or not */
	if (!_rtw_memcmp(adapter_mac_addr(padapter), GetAddr1Ptr(pframe), ETH_ALEN))
		goto exit;

	category = frame_body[0];
	if (category != RTW_WLAN_CATEGORY_VHT)
		goto exit;

	action = frame_body[1];
	switch (action) {
	case RTW_WLAN_ACTION_VHT_COMPRESSED_BEAMFORMING:
#ifdef CONFIG_BEAMFORMING
		/*RTW_INFO("RTW_WLAN_ACTION_VHT_COMPRESSED_BEAMFORMING\n");*/
		/*rtw_beamforming_get_report_frame(padapter, precv_frame);*/
#endif /*CONFIG_BEAMFORMING*/
		break;
	case RTW_WLAN_ACTION_VHT_OPMODE_NOTIFICATION:
		/* CategoryCode(1) + ActionCode(1) + OpModeNotification(1) */
		/* RTW_INFO("RTW_WLAN_ACTION_VHT_OPMODE_NOTIFICATION\n"); */
		psta = rtw_get_stainfo(&padapter->stapriv, whdr->addr2);
		if (psta)
			rtw_process_vht_op_mode_notify(padapter, &frame_body[2], psta);
		break;
	case RTW_WLAN_ACTION_VHT_GROUPID_MANAGEMENT:
#ifdef CONFIG_BEAMFORMING
		rtw_beamforming_get_vht_gid_mgnt_frame(padapter, precv_frame);
#endif /* CONFIG_BEAMFORMING */
		break;
	default:
		break;
	}

exit:
#endif /* CONFIG_80211AC_VHT */

	return _SUCCESS;
}

unsigned int OnAction_he(_adapter *padapter, union recv_frame *precv_frame)
{
#ifdef CONFIG_80211AX_HE
	/* CONFIG_80211AX_HE_TODO */
#endif /* CONFIG_80211AX_HE */

	return _SUCCESS;
}

unsigned int OnAction_protected_he(_adapter *padapter, union recv_frame *precv_frame)
{
#ifdef CONFIG_80211AX_HE
	/* CONFIG_80211AX_HE_TODO */
#endif /* CONFIG_80211AX_HE */

	return _SUCCESS;
}


unsigned int OnAction_p2p(_adapter *padapter, union recv_frame *precv_frame)
{
#ifdef CONFIG_P2P
	u8 *frame_body;
	u8 category, OUI_Subtype, dialogToken = 0;
	u8 *pframe = precv_frame->u.hdr.rx_data;
	uint len = precv_frame->u.hdr.len;
	struct	wifidirect_info	*pwdinfo = &(padapter->wdinfo);

	/* check RA matches or not */
	if (!_rtw_memcmp(adapter_mac_addr(padapter), GetAddr1Ptr(pframe), ETH_ALEN))
		return _SUCCESS;

	frame_body = (unsigned char *)(pframe + sizeof(struct rtw_ieee80211_hdr_3addr));

	category = frame_body[0];
	if (category != RTW_WLAN_CATEGORY_P2P)
		return _SUCCESS;

	if (cpu_to_be32(*((u32 *)(frame_body + 1))) != P2POUI)
		return _SUCCESS;

#ifdef CONFIG_IOCTL_CFG80211
	if (adapter_wdev_data(padapter)->p2p_enabled) {
		rtw_cfg80211_rx_action_p2p(padapter, precv_frame);
		return _SUCCESS;
	}
#endif /* CONFIG_IOCTL_CFG80211 */
#endif /* CONFIG_P2P */

	return _SUCCESS;

}

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
unsigned int OnAction_tbtx_token(_adapter *padapter, union recv_frame *precv_frame)
{
#define TOKEN_REQ 0x00
#define TOKEN_REL 0x01

	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
	u32 xmit_time;
	u8 *src=NULL, *pframe = precv_frame->u.hdr.rx_data;
	u8 tbtx_action_code;
	u8 i, nr_send;
	uint tx_duration = 0;

	if (padapter->tbtx_capability == _FALSE)
		goto exit;

	tbtx_action_code = *(pframe + WLAN_HDR_A3_LEN + 1);


	switch (tbtx_action_code)
	{
		case TOKEN_REQ:
			// parse duration
			tx_duration = le32_to_cpu(*(uint *)(pframe + WLAN_HDR_A3_LEN + 2));
			padapter->tbtx_duration = tx_duration/1000; // Mirocsecond to Millisecond
			ATOMIC_SET(&padapter->tbtx_tx_pause, _FALSE);
			rtw_tx_control_cmd(padapter);
			_set_timer(&pmlmeext->tbtx_xmit_timer, padapter->tbtx_duration);
			ATOMIC_SET(&padapter->tbtx_remove_tx_pause, _FALSE);
			rtw_tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
			break;
#ifdef CONFIG_AP_MODE
		case TOKEN_REL:
			src = get_addr2_ptr(pframe);
			if (!src)
				goto exit;
			psta = rtw_get_stainfo(&padapter->stapriv, src);
			if (!psta)
				goto exit;

			if (ATOMIC_READ(&pstapriv->nr_token_keeper) < 1)
				goto exit;

			for (i=0; i< NR_MAXSTA_INSLOT; i++) {
				if (pstapriv->token_holder[i] == psta) {
					pstapriv->token_holder[i] = NULL;
					//RTW_INFO("macaddr1:" MAC_FMT "\n", MAC_ARG(psta->phl_sta->mac_addr));
					ATOMIC_DEC(&pstapriv->nr_token_keeper);
					break;
				}
			}

			if (ATOMIC_READ(&pstapriv->nr_token_keeper) == 0)
				_set_timer(&pmlmeext->tbtx_token_dispatch_timer, 1);

			break;
#endif
		default:
			RTW_INFO("Undefined Action Code\n");
			goto exit;
			break;
	}

exit:
	return _SUCCESS;
}

void rtw_issue_action_token_rel(_adapter *padapter)
{

	// Todo: 
	// gen token
	/* Token Release Format
		Category code : 	1 Byte
		Action code : 		1 Byte */
	int ret = _FAIL;
	//u16	*fctrl;
	u8	val = 0x01;
	u8	category = RTW_WLAN_CATEGORY_TBTX;
	u8	*pframe;
	struct xmit_frame		*pmgntframe;
	struct pkt_attrib		*pattrib;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	struct	mlme_priv		*pmlmepriv = &padapter->mlmepriv;
	struct xmit_priv		*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK 	*padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_info	*lmlmeinfo = &padapter_link->mlmeextpriv.mlmext_info;
	//struct sta_info			*psta;
	//struct sta_priv			*pstapriv = &padapter->stapriv;
	//struct registry_priv		*pregpriv = &padapter->registrypriv;

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return;

	/*update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgnt_tx_rate(padapter, padapter_link, IEEE80211_OFDM_RATE_24MB); // issue action request using OFDM rate? 20190322 Bruce add
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	//fctrl = &(pwlanhdr->frame_ctl);
	//*(fctrl) = 0;

	_rtw_memcpy(pwlanhdr->addr1, get_my_bssid(&(lmlmeinfo->network)), ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(lmlmeinfo->network)), ETH_ALEN);

	// SetSeqNum??
	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_ACTION);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	pframe = rtw_set_fixed_ie(pframe, 1, &(category), &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 1, &(val), &(pattrib->pktlen));

	pattrib->last_txcmdsz = pattrib->pktlen;

	dump_mgntframe(padapter, pmgntframe);

	//RTW_INFO("%s\n", __func__);

}
#endif

unsigned int OnAction(_adapter *padapter, union recv_frame *precv_frame)
{
	int i;
	unsigned char	category;
	struct action_handler *ptable;
	unsigned char	*frame_body;
	u8 *pframe = precv_frame->u.hdr.rx_data;

	frame_body = (unsigned char *)(pframe + sizeof(struct rtw_ieee80211_hdr_3addr));

	category = frame_body[0];

	for (i = 0; i < sizeof(OnAction_tbl) / sizeof(struct action_handler); i++) {
		ptable = &OnAction_tbl[i];

		if (category == ptable->num)
			ptable->func(padapter, precv_frame);

	}

	return _SUCCESS;

}

unsigned int DoReserved(_adapter *padapter, union recv_frame *precv_frame)
{

	/* RTW_INFO("rcvd mgt frame(%x, %x)\n", (get_frame_sub_type(pframe) >> 4), *(unsigned int *)GetAddr1Ptr(pframe)); */
	return _SUCCESS;
}

struct xmit_frame *_alloc_mgtxmitframe(struct xmit_priv *pxmitpriv, bool once)
{
	struct xmit_frame *pmgntframe;

	if (once)
		pmgntframe = rtw_alloc_xmitframe_once(pxmitpriv);
	else
		pmgntframe = rtw_alloc_xmitframe_ext(pxmitpriv);

	if (pmgntframe == NULL) {
		RTW_INFO(FUNC_ADPT_FMT" alloc xmitframe fail, once:%d\n", FUNC_ADPT_ARG(pxmitpriv->adapter), once);
		goto exit;
	}
#if 0 /*CONFIG_CORE_XMITBUF*/
	struct xmit_buf *pxmitbuf;
	pxmitbuf = rtw_alloc_xmitbuf_ext(pxmitpriv);
	if (pxmitbuf == NULL) {
		RTW_INFO(FUNC_ADPT_FMT" alloc xmitbuf fail\n", FUNC_ADPT_ARG(pxmitpriv->adapter));
		rtw_free_xmitframe(pxmitpriv, pmgntframe);
		pmgntframe = NULL;
		goto exit;
	}

	pmgntframe->frame_tag = MGNT_FRAMETAG;
	pmgntframe->pxmitbuf = pxmitbuf;
	pmgntframe->buf_addr = pxmitbuf->pbuf;
	pxmitbuf->priv_data = pmgntframe;
#else
	pmgntframe->frame_tag = MGNT_FRAMETAG;
#endif

exit:
	return pmgntframe;

}

inline struct xmit_frame *alloc_mgtxmitframe(struct xmit_priv *pxmitpriv)
{
	return _alloc_mgtxmitframe(pxmitpriv, _FALSE);
}

inline struct xmit_frame *alloc_mgtxmitframe_once(struct xmit_priv *pxmitpriv)
{
	return _alloc_mgtxmitframe(pxmitpriv, _TRUE);
}


/****************************************************************************

Following are some TX fuctions for WiFi MLME

*****************************************************************************/

void update_mgnt_tx_rate(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 rate)
{
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);

	pmlmeext->tx_rate = rate;
	/* RTW_INFO("%s(): rate = %x\n",__FUNCTION__, rate); */
}


void update_monitor_frame_attrib(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			struct pkt_attrib *pattrib)
{
	u8	wireless_mode;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	struct sta_info		*psta = NULL;
	struct sta_info		*bmc_sta = NULL;
	struct sta_priv		*pstapriv = &padapter->stapriv;

	psta = rtw_get_stainfo(pstapriv, pattrib->ra);
	bmc_sta = rtw_get_bcmc_stainfo(padapter, padapter_link);

	if (bmc_sta == NULL) {
		RTW_ERR("%s bmc_sta=NULL\n", __func__);
		return;
	}

	pattrib->hdrlen = 24;
	pattrib->nr_frags = 1;
	pattrib->priority = 7;
	pattrib->mac_id = bmc_sta->phl_sta->macid;
	pattrib->adapter_link = padapter_link;

	pattrib->pktlen = 0;

	if (padapter_link->mlmeextpriv.tx_rate == IEEE80211_CCK_RATE_1MB)
		wireless_mode = WLAN_MD_11B;
	else
		wireless_mode = WLAN_MD_11G;


#ifdef CONFIG_80211AC_VHT
	pattrib->rate = MGN_VHT1SS_MCS9;
#else
	pattrib->rate = MGN_MCS7;
#endif

#ifdef CONFIG_80211AX_HE
	/* CONFIG_80211AX_HE_TODO */
#endif

	pattrib->encrypt = _NO_PRIVACY_;
	pattrib->bswenc = _FALSE;

	pattrib->qos_en = _FALSE;
	pattrib->ht_en = 1;
	pattrib->bwmode = CHANNEL_WIDTH_20;
	pattrib->ch_offset = CHAN_OFFSET_NO_EXT;
	pattrib->sgi = _FALSE;

	pattrib->seqnum = pmlmeext->mgnt_seq;

	pattrib->retry_ctrl = _TRUE;

	pattrib->mbssid = 0;
	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;

}


void update_mgntframe_attrib(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
				struct pkt_attrib *pattrib)
{
	u8	wireless_mode;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;
	struct sta_info		*bmc_sta = NULL;

#ifdef CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P_PS_NOA_USE_MACID_SLEEP */

	/* _rtw_memset((u8 *)(pattrib), 0, sizeof(struct pkt_attrib)); */

	bmc_sta = rtw_get_bcmc_stainfo(padapter, padapter_link);
	if (bmc_sta == NULL) {
		RTW_ERR("%s bmc_sta=NULL\n", __func__);
		return;
	}
	pattrib->type = WIFI_MGT_TYPE;
	pattrib->hdrlen = 24;
	pattrib->nr_frags = 1;
	pattrib->priority = 8;
	pattrib->mac_id = bmc_sta->phl_sta->macid;
	pattrib->adapter_link = padapter_link;

#ifdef CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_mi_buddy_check_fwstate(padapter, WIFI_ASOC_STATE))
#endif /* CONFIG_CONCURRENT_MODE */
		if (MLME_IS_GC(padapter)) {
			if (pwdinfo->p2p_ps_mode > P2P_PS_NONE) {
				struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
				struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
				WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);
				struct sta_priv *pstapriv = &padapter->stapriv;
				struct sta_info *psta;

				psta = rtw_get_stainfo(pstapriv, cur_network->MacAddress);
				if (psta) {
					/* use macid sleep during NoA, mgmt frame use ac queue & ap macid */
					pattrib->mac_id = psta->phl_sta->macid;
				} else {
					if (pwdinfo->p2p_ps_state != P2P_PS_DISABLE)
						RTW_ERR("%s , psta was NULL\n", __func__);
				}
			}
		}
#endif /* CONFIG_P2P_PS_NOA_USE_MACID_SLEEP */


	pattrib->pktlen = 0;

	if (IS_CCK_RATE(padapter_link->mlmeextpriv.tx_rate))
		wireless_mode = WLAN_MD_11B;
	else
		wireless_mode = WLAN_MD_11G;
	pattrib->rate = padapter_link->mlmeextpriv.tx_rate;

	pattrib->encrypt = _NO_PRIVACY_;
	pattrib->bswenc = _FALSE;

	pattrib->qos_en = _FALSE;
	pattrib->ht_en = _FALSE;
	pattrib->bwmode = CHANNEL_WIDTH_20;
	pattrib->ch_offset = CHAN_OFFSET_NO_EXT;
	pattrib->sgi = _FALSE;

	pattrib->seqnum = pmlmeext->mgnt_seq;

	pattrib->retry_ctrl = _TRUE;

	pattrib->mbssid = 0;
	pattrib->hw_ssn_sel = pxmitpriv->hw_ssn_seq_no;
}

void update_mgntframe_attrib_addr(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	u8	*pframe;
	struct pkt_attrib	*pattrib = &pmgntframe->attrib;
#if defined(CONFIG_BEAMFORMING) || defined(CONFIG_ANTENNA_DIVERSITY)
	struct sta_info		*sta = NULL;
#endif

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;

	_rtw_memcpy(pattrib->ra, GetAddr1Ptr(pframe), ETH_ALEN);
	_rtw_memcpy(pattrib->ta, get_addr2_ptr(pframe), ETH_ALEN);

#if defined(CONFIG_BEAMFORMING) || defined(CONFIG_ANTENNA_DIVERSITY)
	sta = pattrib->psta;
	if (!sta) {
		sta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);
		pattrib->psta = sta;
	}
#endif /* defined(CONFIG_BEAMFORMING) || defined(CONFIG_ANTENNA_DIVERSITY) */
}

void dump_mgntframe(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
		#if 0 /*CONFIG_CORE_XMITBUF*/
		rtw_free_xmitbuf(&padapter->xmitpriv, pmgntframe->pxmitbuf);
		#endif
		rtw_free_xmitframe(&padapter->xmitpriv, pmgntframe);
		return;
	}

	/*rtw_hal_mgnt_xmit(padapter, pmgntframe);*/
	rtw_mgnt_xmit(padapter, pmgntframe);
}

s32 dump_mgntframe_and_wait(_adapter *padapter, struct xmit_frame *pmgntframe, int timeout_ms)
{
	s32 ret = _FAIL;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	#if 0 /*CONFIG_CORE_XMITBUF*/
	struct xmit_buf *pxmitbuf = pmgntframe->pxmitbuf;
	struct submit_ctx sctx;
	unsigned long sp_flags;

	if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
		rtw_free_xmitbuf(&padapter->xmitpriv, pmgntframe->pxmitbuf);
		rtw_free_xmitframe(&padapter->xmitpriv, pmgntframe);
		return ret;
	}

	rtw_sctx_init(&sctx, timeout_ms);
	pxmitbuf->sctx = &sctx;

	/*ret = rtw_hal_mgnt_xmit(padapter, pmgntframe);*/
	ret = rtw_mgnt_xmit(padapter, pmgntframe);

	if (ret == _SUCCESS)
		ret = rtw_sctx_wait(&sctx, __func__);

	_rtw_spinlock_irq(&pxmitpriv->lock_sctx, &sp_flags);
	pxmitbuf->sctx = NULL;
	_rtw_spinunlock_irq(&pxmitpriv->lock_sctx, &sp_flags);
	#else
	if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
		rtw_free_xmitframe(&padapter->xmitpriv, pmgntframe);
		return ret;
	}
	#endif
	return ret;
}

s32 dump_mgntframe_and_wait_ack_timeout(_adapter *padapter, struct xmit_frame *pmgntframe, int timeout_ms)
{
#ifdef CONFIG_XMIT_ACK
	static u8 seq_no = 0;
	s32 ret = _FAIL;
	#ifdef CONFIG_XMIT_ACK_BY_REL_RPT
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	#else
	struct xmit_priv *pxmitpriv = &(GET_PRIMARY_ADAPTER(padapter))->xmitpriv;
	#endif

	if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
		rtw_free_xmitframe(&padapter->xmitpriv, pmgntframe);
		return -1;
	}

	_rtw_mutex_lock_interruptible(&pxmitpriv->ack_tx_mutex);
	pxmitpriv->ack_tx = _TRUE;
	pxmitpriv->ack_tx_seq_no = seq_no++;
	pmgntframe->ack_report = 1;
	rtw_sctx_init(&(pxmitpriv->ack_tx_ops), timeout_ms);
	/*if (rtw_hal_mgnt_xmit(padapter, pmgntframe) == _SUCCESS)*/
	if (rtw_mgnt_xmit(padapter, pmgntframe) == _SUCCESS)
		ret = rtw_sctx_wait(&(pxmitpriv->ack_tx_ops), __func__);

	pxmitpriv->ack_tx = _FALSE;
	_rtw_mutex_unlock(&pxmitpriv->ack_tx_mutex);

	return ret;
#else /* !CONFIG_XMIT_ACK */
	dump_mgntframe(padapter, pmgntframe);
	rtw_msleep_os(50);
	return _SUCCESS;
#endif /* !CONFIG_XMIT_ACK */
}

s32 dump_mgntframe_and_wait_ack(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	/* In this case, use 500 ms as the default wait_ack timeout */
	return dump_mgntframe_and_wait_ack_timeout(padapter, pmgntframe, 500);
}

#ifdef RTW_PHL_BCN //core ops
s32 rtw_core_issue_beacon(_adapter *padapter, struct xmit_frame *pmgntframe)
{
	struct rtw_wifi_role_t *wrole = padapter->phl_role;
	struct rtw_bcn_info_cmn *bcn_cmn = NULL;
	struct pkt_attrib	*pattrib = &pmgntframe->attrib;
	u8	*pframe = (u8 *)(pmgntframe->buf_addr);
	void *phl = padapter->dvobj->phl;
	struct _ADAPTER_LINK *padapter_link = pattrib->adapter_link;

	if(!wrole)
		return _FAIL;

	if(pattrib->pktlen > MAX_BCN_SIZE)
		return _FAIL;

	bcn_cmn = &padapter_link->wrlink->bcn_cmn;
	_rtw_memcpy(bcn_cmn->bcn_buf, pframe, pattrib->pktlen);
	bcn_cmn->bcn_length = pattrib->pktlen;
	{
		u8 *pie_start, *pie_tim = NULL;
		u32 total_ielen, tim_ielen = 0;

		pie_start = pframe + (sizeof(struct rtw_ieee80211_hdr_3addr) + _FIXED_IE_LENGTH_);
		total_ielen = pattrib->pktlen - (sizeof(struct rtw_ieee80211_hdr_3addr) + _FIXED_IE_LENGTH_);

		pie_tim = rtw_get_ie(pie_start, _TIM_IE_, &tim_ielen, total_ielen);

		if(pie_tim)
			bcn_cmn->ie_offset_tim = (u32)(pie_tim - pie_start) + _FIXED_IE_LENGTH_;
	}

	if(bcn_cmn->bcn_added)
		rtw_phl_cmd_issue_beacon(phl, padapter_link->wrlink, bcn_cmn, PHL_CMD_DIRECTLY, 0);
	else {
		struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
		struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
		WLAN_BSSID_EX *cur_network = &(pmlmeinfo->network);

		_rtw_memcpy(bcn_cmn->bssid, get_my_bssid(cur_network), ETH_ALEN);
		bcn_cmn->bcn_id = padapter->iface_id;
		bcn_cmn->role_idx = wrole->id;
		bcn_cmn->lidx = padapter_link->wrlink->id;
		bcn_cmn->bcn_interval = pmlmeinfo->bcn_interval;
		bcn_cmn->bcn_offload = (BIT(BCN_HW_SEQ) | BIT(BCN_HW_TIM));

		if (pmlmeext->chandef.chan <= 14)
			bcn_cmn->bcn_rate = RTW_DATA_RATE_CCK1;
		else
			bcn_cmn->bcn_rate = RTW_DATA_RATE_OFDM6;

		rtw_phl_cmd_issue_beacon(phl, padapter_link->wrlink, bcn_cmn, PHL_CMD_DIRECTLY, 0);
		bcn_cmn->bcn_added = 1;

		/*
				[todo]
				bcn_id: mbssid-ieee
				bcn_offload: tbd
				bcn_rate, 0=cck 1=ofdm, 2mac can NOT cck
		*/
	}
	return _SUCCESS;
}
#endif

int update_hidden_ssid(u8 *ies, u32 ies_len, u8 hidden_ssid_mode)
{
	u8 *ssid_ie;
	sint ssid_len_ori;
	int len_diff = 0;

	ssid_ie = rtw_get_ie(ies,  WLAN_EID_SSID, &ssid_len_ori, ies_len);

	/* RTW_INFO("%s hidden_ssid_mode:%u, ssid_ie:%p, ssid_len_ori:%d\n", __FUNCTION__, hidden_ssid_mode, ssid_ie, ssid_len_ori); */

	if (ssid_ie && ssid_len_ori > 0) {
		switch (hidden_ssid_mode) {
		case 1: {
			u8 *next_ie = ssid_ie + 2 + ssid_len_ori;
			u32 remain_len = 0;

			remain_len = ies_len - (next_ie - ies);

			ssid_ie[1] = 0;
			_rtw_memcpy(ssid_ie + 2, next_ie, remain_len);
			len_diff -= ssid_len_ori;

			break;
		}
		case 2:
			_rtw_memset(&ssid_ie[2], 0, ssid_len_ori);
			break;
		default:
			break;
		}
	}

	return len_diff;
}

u8 *rtw_build_reduced_nb_rpt(_adapter *padapter, struct pkt_attrib *pattrib, u8 *pframe)
{
	u32 len = 0;
	len = rtw_phl_build_reduced_nb_rpt(padapter->phl_role, pattrib->adapter_link->wrlink, pframe);
	pattrib->pktlen += len;

	return pframe + len;
}

void issue_beacon(_adapter *padapter, int timeout_ms)
{
	struct xmit_frame	*pmgntframe;
	struct pkt_attrib	*pattrib;
	unsigned char	*pframe;
	struct rtw_ieee80211_hdr *pwlanhdr;
	unsigned short *fctrl;
	unsigned int	rate_len;
	struct xmit_priv	*pxmitpriv = &(padapter->xmitpriv);
#if defined(CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME)
	struct mlme_priv *pmlmepriv = &(padapter->mlmepriv);
#endif /* #if defined (CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME) */
	struct mlme_ext_info	*pmlmeinfo = &(padapter->mlmeextpriv.mlmext_info);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*lmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info	*lmlmeinfo = &(lmlmeext->mlmext_info);
	WLAN_BSSID_EX		*cur_network = &(lmlmeinfo->network);
	u8	bc_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#ifdef CONFIG_P2P
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P */


	/* RTW_INFO("%s\n", __FUNCTION__); */

#if 0 /*def CONFIG_BCN_ICF*/
	pmgntframe = rtw_alloc_bcnxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
#else
	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
#endif
	{
		RTW_INFO("%s, alloc mgnt frame fail\n", __FUNCTION__);
		return;
	}
#if defined(CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME)
	_rtw_spinlock_bh(&padapter_link->mlmepriv.bcn_update_lock);
#endif /* #if defined (CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME) */

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

#if 0 /*defined(CONFIG_CONCURRENT_MODE)*/
	if (padapter->hw_port == HW_PORT1)
		pattrib->mbssid = 1;
#endif

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;


	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy(pwlanhdr->addr1, bc_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(cur_network), ETH_ALEN);

	SetSeqNum(pwlanhdr, 0/*pmlmeext->mgnt_seq*/);
	/* pmlmeext->mgnt_seq++; */
	set_frame_sub_type(pframe, WIFI_BEACON);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	if (MLME_IS_AP(padapter) || MLME_IS_MESH(padapter)) {
		/* RTW_INFO("ie len=%d\n", cur_network->IELength); */
#ifdef CONFIG_P2P
		/* for P2P : Primary Device Type & Device Name */
		u32 wpsielen = 0, insert_len = 0;
		u8 *wpsie = NULL;
		wpsie = rtw_get_wps_ie(cur_network->IEs + _FIXED_IE_LENGTH_, cur_network->IELength - _FIXED_IE_LENGTH_, NULL, &wpsielen);

		if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO) && wpsie && wpsielen > 0) {
			uint wps_offset, remainder_ielen;
			u8 *premainder_ie, *pframe_wscie;

			wps_offset = (uint)(wpsie - cur_network->IEs);

			premainder_ie = wpsie + wpsielen;

			remainder_ielen = cur_network->IELength - wps_offset - wpsielen;

#ifdef CONFIG_IOCTL_CFG80211
			if (adapter_wdev_data(padapter)->p2p_enabled) {
				if (pmlmepriv->wps_beacon_ie && pmlmepriv->wps_beacon_ie_len > 0) {
#ifdef PRIVATE_R
					/* if no feature need to show ssid, hide GO ssid in beacon */
					struct registry_priv	*pregpriv = &padapter->registrypriv;
					int len_diff;
					u8 hide_ssid = 0;

					if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO) && pregpriv->go_hidden_ssid_mode == ALL_HIDE_SSID)
						hide_ssid = 1;
#endif
					_rtw_memcpy(pframe, cur_network->IEs, wps_offset);
#ifdef PRIVATE_R
					len_diff = update_hidden_ssid(
								pframe + _BEACON_IE_OFFSET_
								, wps_offset - _BEACON_IE_OFFSET_
								, hide_ssid);
					pframe += len_diff;
					pattrib->pktlen += len_diff;
#endif
					pframe += wps_offset;
					pattrib->pktlen += wps_offset;

					_rtw_memcpy(pframe, pmlmepriv->wps_beacon_ie, pmlmepriv->wps_beacon_ie_len);
					pframe += pmlmepriv->wps_beacon_ie_len;
					pattrib->pktlen += pmlmepriv->wps_beacon_ie_len;

					/* copy remainder_ie to pframe */
					_rtw_memcpy(pframe, premainder_ie, remainder_ielen);
					pframe += remainder_ielen;
					pattrib->pktlen += remainder_ielen;
				} else {
					_rtw_memcpy(pframe, cur_network->IEs, cur_network->IELength);
					pframe += cur_network->IELength;
					pattrib->pktlen += cur_network->IELength;
				}
			}
#endif /* CONFIG_IOCTL_CFG80211 */
		} else
#endif /* CONFIG_P2P */
		{
			int len_diff;
			_rtw_memcpy(pframe, cur_network->IEs, cur_network->IELength);
			len_diff = update_hidden_ssid(
					   pframe + _BEACON_IE_OFFSET_
				   , cur_network->IELength - _BEACON_IE_OFFSET_
					   , pmlmeinfo->hidden_ssid_mode
				   );
			pframe += (cur_network->IELength + len_diff);
			pattrib->pktlen += (cur_network->IELength + len_diff);
		}

		{
			u8 *wps_ie;
			uint wps_ielen;
			u8 sr = 0;
			wps_ie = rtw_get_wps_ie(pmgntframe->buf_addr + TXDESC_OFFSET + sizeof(struct rtw_ieee80211_hdr_3addr) + _BEACON_IE_OFFSET_,
				pattrib->pktlen - sizeof(struct rtw_ieee80211_hdr_3addr) - _BEACON_IE_OFFSET_, NULL, &wps_ielen);
			if (wps_ie && wps_ielen > 0)
				rtw_get_wps_attr_content(wps_ie,  wps_ielen, WPS_ATTR_SELECTED_REGISTRAR, (u8 *)(&sr), NULL);
			if (sr != 0)
				set_fwstate(pmlmepriv, WIFI_UNDER_WPS);
			else
				_clr_fwstate_(pmlmepriv, WIFI_UNDER_WPS);
		}

#ifdef CONFIG_RTW_MBO
		rtw_mbo_build_beacon_ies(padapter, &pframe, pattrib);
#endif
#ifdef CONFIG_P2P
		if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
			u32 len = 0;
#ifdef CONFIG_IOCTL_CFG80211
			if (adapter_wdev_data(padapter)->p2p_enabled) {
				len = pmlmepriv->p2p_beacon_ie_len;
				if (pmlmepriv->p2p_beacon_ie && len > 0) {
					_rtw_memcpy(pframe, pmlmepriv->p2p_beacon_ie, len);
					pframe += len;
					pattrib->pktlen += len;
				}
			}
#endif /* CONFIG_IOCTL_CFG80211 */
#ifdef CONFIG_P2P_PS
			pframe = rtw_append_p2p_go_noa_ie(padapter, pframe,
							  &pattrib->pktlen);
#endif
#ifdef CONFIG_WFD
			len = rtw_append_beacon_wfd_ie(padapter, pframe);
			pframe += len;
			pattrib->pktlen += len;
#endif
		}
#endif /* CONFIG_P2P */

#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
		pattrib->pktlen += rtw_build_vendor_ie(padapter , &pframe , WIFI_BEACON_VENDOR_IE_BIT);
#endif


#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
		if (padapter->tbtx_capability == _TRUE)
			pframe = rtw_set_ie(pframe, _VENDOR_SPECIFIC_IE_, 8, REALTEK_TBTX_IE, &pattrib->pktlen);
#endif

#ifdef CONFIG_80211BE_EHT
		if (pmlmepriv->mlopriv.mlo_option == _TRUE) {
			pframe = rtw_build_reduced_nb_rpt(padapter, pattrib, pframe);
			pframe = rtw_build_ml_ies(padapter, NULL, pframe, pattrib, PACKET_BEACON, NULL, 0, 0);
		}
#endif
		goto _issue_bcn;

	}

	/* below for ad-hoc mode */

	/* timestamp will be inserted by hardware */
	pframe += 8;
	pattrib->pktlen += 8;

	/* beacon interval: 2 bytes */

	_rtw_memcpy(pframe, (unsigned char *)(rtw_get_beacon_interval_from_ie(cur_network->IEs)), 2);

	pframe += 2;
	pattrib->pktlen += 2;

	/* capability info: 2 bytes */

	_rtw_memcpy(pframe, (unsigned char *)(rtw_get_capability_from_ie(cur_network->IEs)), 2);

	pframe += 2;
	pattrib->pktlen += 2;

	/* SSID */
	pframe = rtw_set_ie(pframe, _SSID_IE_, cur_network->Ssid.SsidLength, cur_network->Ssid.Ssid, &pattrib->pktlen);

	/* supported rates... */
	rate_len = rtw_get_rateset_len(cur_network->SupportedRates);
	pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_, ((rate_len > 8) ? 8 : rate_len), cur_network->SupportedRates, &pattrib->pktlen);

	/* DS parameter set */
	pframe = rtw_set_ie(pframe, _DSSET_IE_, 1, (unsigned char *)&(cur_network->Configuration.DSConfig), &pattrib->pktlen);

	/* if( (pmlmeinfo->state&0x03) == WIFI_FW_ADHOC_STATE) */
	{
		u8 erpinfo = 0;
		u32 ATIMWindow;
		/* IBSS Parameter Set... */
		/* ATIMWindow = cur->Configuration.ATIMWindow; */
		ATIMWindow = 0;
		pframe = rtw_set_ie(pframe, _IBSS_PARA_IE_, 2, (unsigned char *)(&ATIMWindow), &pattrib->pktlen);

		/* ERP IE */
		pframe = rtw_set_ie(pframe, _ERPINFO_IE_, 1, &erpinfo, &pattrib->pktlen);
	}


	/* EXTERNDED SUPPORTED RATE */
	if (rate_len > 8)
		pframe = rtw_set_ie(pframe, _EXT_SUPPORTEDRATES_IE_, (rate_len - 8), (cur_network->SupportedRates + 8), &pattrib->pktlen);


	/* todo:HT for adhoc */

_issue_bcn:

#if defined(CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME)
	padapter_link->mlmepriv.update_bcn = _FALSE;

	_rtw_spinunlock_bh(&padapter_link->mlmepriv.bcn_update_lock);
#endif /* #if defined (CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME) */

	if ((pattrib->pktlen + TXDESC_SIZE) > MAX_BEACON_LEN) {
		RTW_ERR("beacon frame too large ,len(%d,%d)\n",
			(pattrib->pktlen + TXDESC_SIZE), MAX_BEACON_LEN);
		rtw_warn_on(1);
		return;
	}

	pattrib->last_txcmdsz = pattrib->pktlen;

	/* RTW_INFO("issue bcn_sz=%d\n", pattrib->last_txcmdsz); */
	
#ifdef RTW_PHL_BCN
	rtw_core_issue_beacon(padapter, pmgntframe);

	#if 0 /*CONFIG_CORE_XMITBUF*/
	rtw_free_xmitbuf(&padapter->xmitpriv, pmgntframe->pxmitbuf);
	#endif
	rtw_free_xmitframe(&padapter->xmitpriv, pmgntframe);
#else
	if (timeout_ms > 0)
		dump_mgntframe_and_wait(padapter, pmgntframe, timeout_ms);
	else
		dump_mgntframe(padapter, pmgntframe);
#endif

}

u8 *rtw_build_probersp_ies(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		u8 *pframe, struct pkt_attrib *pattrib, u8 is_valid_p2p_probereq)
{
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
#if defined(CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME)
	u8 *pwps_ie;
	uint wps_ielen;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
#endif /* #if defined (CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME) */
	WLAN_BSSID_EX *cur_network = &(padapter_link->mlmeextpriv.mlmext_info.network);
	unsigned int	rate_len;
#ifdef CONFIG_P2P
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P */
	u8 *pframe_start = pframe;

#if defined(CONFIG_AP_MODE) && defined (CONFIG_NATIVEAP_MLME)
	if ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) {
		pwps_ie = rtw_get_wps_ie(cur_network->IEs + _FIXED_IE_LENGTH_, cur_network->IELength - _FIXED_IE_LENGTH_, NULL, &wps_ielen);

		/* inerset & update wps_probe_resp_ie */
		if ((pmlmepriv->wps_probe_resp_ie != NULL) && pwps_ie && (wps_ielen > 0)) {
			uint wps_offset, remainder_ielen;
			u8 *premainder_ie;

			wps_offset = (uint)(pwps_ie - cur_network->IEs);

			premainder_ie = pwps_ie + wps_ielen;

			remainder_ielen = cur_network->IELength - wps_offset - wps_ielen;

			_rtw_memcpy(pframe, cur_network->IEs, wps_offset);
			pframe += wps_offset;
			pattrib->pktlen += wps_offset;

			wps_ielen = (uint)pmlmepriv->wps_probe_resp_ie[1];/* to get ie data len */
			if ((wps_offset + wps_ielen + 2) <= MAX_IE_SZ) {
				_rtw_memcpy(pframe, pmlmepriv->wps_probe_resp_ie, wps_ielen + 2);
				pframe += wps_ielen + 2;
				pattrib->pktlen += wps_ielen + 2;
			}

			if ((wps_offset + wps_ielen + 2 + remainder_ielen) <= MAX_IE_SZ) {
				_rtw_memcpy(pframe, premainder_ie, remainder_ielen);
				pframe += remainder_ielen;
				pattrib->pktlen += remainder_ielen;
			}
		} else {
			_rtw_memcpy(pframe, cur_network->IEs, cur_network->IELength);
			pframe += cur_network->IELength;
			pattrib->pktlen += cur_network->IELength;
		}

		/* retrieve SSID IE from cur_network->Ssid */
		{
			u8 *ssid_ie;
			sint ssid_ielen;
			sint ssid_ielen_diff;
			u8 buf[MAX_IE_SZ];
			u8 *ies = pframe_start;

			ssid_ie = rtw_get_ie(ies + _FIXED_IE_LENGTH_, _SSID_IE_, &ssid_ielen,
				     (pframe - ies) - _FIXED_IE_LENGTH_);

			ssid_ielen_diff = cur_network->Ssid.SsidLength - ssid_ielen;

			if (ssid_ie &&  cur_network->Ssid.SsidLength) {
				uint remainder_ielen;
				u8 *remainder_ie;
				remainder_ie = ssid_ie + 2;
				remainder_ielen = (pframe - remainder_ie);

				if (remainder_ielen > MAX_IE_SZ) {
					RTW_WARN(FUNC_ADPT_FMT" remainder_ielen > MAX_IE_SZ\n", FUNC_ADPT_ARG(padapter));
					remainder_ielen = MAX_IE_SZ;
				}

				_rtw_memcpy(buf, remainder_ie, remainder_ielen);
				_rtw_memcpy(remainder_ie + ssid_ielen_diff, buf, remainder_ielen);
				*(ssid_ie + 1) = cur_network->Ssid.SsidLength;
				_rtw_memcpy(ssid_ie + 2, cur_network->Ssid.Ssid, cur_network->Ssid.SsidLength);

				pframe += ssid_ielen_diff;
				pattrib->pktlen += ssid_ielen_diff;
			}
		}

#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
		pattrib->pktlen += rtw_build_vendor_ie(padapter , &pframe , WIFI_PROBERESP_VENDOR_IE_BIT);
#endif
	} else
#endif
	{

		/* timestamp will be inserted by hardware */
		pframe += 8;
		pattrib->pktlen += 8;

		/* beacon interval: 2 bytes */

		_rtw_memcpy(pframe, (unsigned char *)(rtw_get_beacon_interval_from_ie(cur_network->IEs)), 2);

		pframe += 2;
		pattrib->pktlen += 2;

		/* capability info: 2 bytes */

		_rtw_memcpy(pframe, (unsigned char *)(rtw_get_capability_from_ie(cur_network->IEs)), 2);

		pframe += 2;
		pattrib->pktlen += 2;

		/* below for ad-hoc mode */

		/* SSID */
		pframe = rtw_set_ie(pframe, _SSID_IE_, cur_network->Ssid.SsidLength, cur_network->Ssid.Ssid, &pattrib->pktlen);

		/* supported rates... */
		rate_len = rtw_get_rateset_len(cur_network->SupportedRates);
		pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_, ((rate_len > 8) ? 8 : rate_len), cur_network->SupportedRates, &pattrib->pktlen);

		/* DS parameter set */
		pframe = rtw_set_ie(pframe, _DSSET_IE_, 1, (unsigned char *)&(cur_network->Configuration.DSConfig), &pattrib->pktlen);

		if ((pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) {
			u8 erpinfo = 0;
			u32 ATIMWindow;
			/* IBSS Parameter Set... */
			/* ATIMWindow = cur->Configuration.ATIMWindow; */
			ATIMWindow = 0;
			pframe = rtw_set_ie(pframe, _IBSS_PARA_IE_, 2, (unsigned char *)(&ATIMWindow), &pattrib->pktlen);

			/* ERP IE */
			pframe = rtw_set_ie(pframe, _ERPINFO_IE_, 1, &erpinfo, &pattrib->pktlen);
		}


		/* EXTERNDED SUPPORTED RATE */
		if (rate_len > 8)
			pframe = rtw_set_ie(pframe, _EXT_SUPPORTEDRATES_IE_, (rate_len - 8), (cur_network->SupportedRates + 8), &pattrib->pktlen);


		/* todo:HT for adhoc */

	}

#ifdef CONFIG_RTW_MBO
	rtw_mbo_build_probe_rsp_ies(padapter, &pframe, pattrib);
#endif
#ifdef CONFIG_P2P
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)
	    /* IOT issue, When wifi_spec is not set, send probe_resp with P2P IE even if probe_req has no P2P IE */
	    && (is_valid_p2p_probereq || !padapter->registrypriv.wifi_spec)) {
		u32 len = 0;
#ifdef CONFIG_IOCTL_CFG80211
		if (adapter_wdev_data(padapter)->p2p_enabled) {
			/* if pwdinfo->role == P2P_ROLE_DEVICE will call issue_probersp_p2p() */
			len = pmlmepriv->p2p_go_probe_resp_ie_len;
			if (pmlmepriv->p2p_go_probe_resp_ie && len > 0) {
				_rtw_memcpy(pframe, pmlmepriv->p2p_go_probe_resp_ie, len);
				pframe += len;
				pattrib->pktlen += len;
			}
		}
#endif /* CONFIG_IOCTL_CFG80211 */
#ifdef CONFIG_P2P_PS
		pframe = rtw_append_p2p_go_noa_ie(padapter, pframe,
						  &pattrib->pktlen);
#endif
#ifdef CONFIG_WFD
		len = rtw_append_probe_resp_wfd_ie(padapter, pframe);
		pframe += len;
		pattrib->pktlen += len;
#endif
	}
#endif /* CONFIG_P2P */


#ifdef CONFIG_AUTO_AP_MODE
	{
		struct sta_info	*psta;
		struct sta_priv *pstapriv = &padapter->stapriv;

		RTW_INFO("(%s)\n", __FUNCTION__);

		/* check rc station */
		psta = rtw_get_stainfo(pstapriv, da);
		if (psta && psta->isrc && psta->pid > 0) {
			u8 RC_OUI[4] = {0x00, 0xE0, 0x4C, 0x0A};
			u8 RC_INFO[14] = {0};
			/* EID[1] + EID_LEN[1] + RC_OUI[4] + MAC[6] + PairingID[2] + ChannelNum[2] */
			u16 cu_ch = (u16)cur_network->Configuration.DSConfig;

			RTW_INFO("%s, reply rc(pid=0x%x) device "MAC_FMT" in ch=%d\n", __FUNCTION__,
				 psta->pid, MAC_ARG(psta->phl_sta->mac_addr), cu_ch);

			/* append vendor specific ie */
			_rtw_memcpy(RC_INFO, RC_OUI, sizeof(RC_OUI));
			_rtw_memcpy(&RC_INFO[4], mac, ETH_ALEN);
			_rtw_memcpy(&RC_INFO[10], (u8 *)&psta->pid, 2);
			_rtw_memcpy(&RC_INFO[12], (u8 *)&cu_ch, 2);

			pframe = rtw_set_ie(pframe, _VENDOR_SPECIFIC_IE_, sizeof(RC_INFO), RC_INFO, &pattrib->pktlen);
		}
	}
#endif /* CONFIG_AUTO_AP_MODE */
	return pframe;
}

void issue_probersp(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			unsigned char *da, u8 is_valid_p2p_probereq)
{
	struct xmit_frame			*pmgntframe;
	struct pkt_attrib			*pattrib;
	unsigned char					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short				*fctrl;
	unsigned char					*mac, *bssid;
	struct xmit_priv	*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	WLAN_BSSID_EX		*cur_network = &(padapter_link->mlmeextpriv.mlmext_info.network);

	/* RTW_INFO("%s\n", __FUNCTION__); */

	if (da == NULL)
		return;

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL) {
		RTW_INFO("%s, alloc mgnt frame fail\n", __FUNCTION__);
		return;
	}


	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	mac = padapter_link->mac_addr;
	bssid = cur_network->MacAddress;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;
	_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, mac, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, bssid, ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(fctrl, WIFI_PROBERSP);

	pattrib->hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = pattrib->hdrlen;
	pframe += pattrib->hdrlen;


	if (cur_network->IELength > MAX_IE_SZ)
		return;

	pframe = rtw_build_probersp_ies(padapter, padapter_link, pframe, pattrib, is_valid_p2p_probereq);

#ifdef CONFIG_80211BE_EHT
	if (padapter->mlmepriv.mlopriv.mlo_option == _TRUE) {
		pframe = rtw_build_reduced_nb_rpt(padapter, pattrib, pframe);
		pframe = rtw_build_ml_ies(padapter, NULL, pframe, pattrib, PACKET_PROBE_RESPONSE,
					NULL, 0, 0);
	}
#endif

	pattrib->last_txcmdsz = pattrib->pktlen;


	dump_mgntframe(padapter, pmgntframe);

	return;

}

#ifdef CONFIG_80211BE_EHT
void issue_ml_probersp(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, unsigned char *da,
			u8 is_valid_p2p_probereq)
{
	struct xmit_frame			*pmgntframe;
	struct pkt_attrib			*pattrib;
	unsigned char					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short				*fctrl;
	unsigned char					*mac, *bssid;
	struct xmit_priv	*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	WLAN_BSSID_EX		*cur_network = &(padapter_link->mlmeextpriv.mlmext_info.network);
	u8 *primary_ies;
	u32 primary_ies_len = 0;


	/* RTW_INFO("%s\n", __FUNCTION__); */

	if (da == NULL)
		return;

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL) {
		RTW_INFO("%s, alloc mgnt frame fail\n", __FUNCTION__);
		return;
	}


	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	mac = padapter_link->mac_addr;
	bssid = cur_network->MacAddress;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;
	_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, mac, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, bssid, ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(fctrl, WIFI_PROBERSP);

	pattrib->hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = pattrib->hdrlen;
	pframe += pattrib->hdrlen;


	if (cur_network->IELength > MAX_IE_SZ)
		return;

	primary_ies = pframe;
	pframe = rtw_build_probersp_ies(padapter, padapter_link, pframe, pattrib, is_valid_p2p_probereq);
	primary_ies_len = pattrib->pktlen - sizeof(struct rtw_ieee80211_hdr_3addr);

	pframe = rtw_build_reduced_nb_rpt(padapter, pattrib, pframe);

	pframe = rtw_build_ml_ies(padapter, NULL, pframe, pattrib, PACKET_PROBE_RESPONSE_ML,
		primary_ies, primary_ies_len, is_valid_p2p_probereq);

	pattrib->last_txcmdsz = pattrib->pktlen;


	dump_mgntframe(padapter, pmgntframe);

	return;
}
#endif

int _issue_probereq(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		const NDIS_802_11_SSID *pssid, const u8 *da, u8 ch, bool append_wps, int wait_ack)
{
	int ret = _FAIL;
	struct xmit_frame		*pmgntframe;
	struct pkt_attrib		*pattrib;
	unsigned char			*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short		*fctrl;
	unsigned char			*mac;
	unsigned char			bssrate[NumRates];
	struct xmit_priv		*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_priv		*pmlmepriv = &(padapter->mlmepriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	int	bssrate_len = 0;
	u8	bc_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
	struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);
#endif

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		goto exit;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);


	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
	if ((pwdev_priv->random_mac_enabled)
	    && (MLME_IS_STA(padapter))
	    && (check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE) == _FALSE))
		mac = pwdev_priv->pno_mac_addr;
	else
#endif
	mac = padapter_link->mac_addr;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	if (da) {
		/*	unicast probe request frame */
		_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, da, ETH_ALEN);
	} else {
		/*	broadcast probe request frame */
		_rtw_memcpy(pwlanhdr->addr1, bc_addr, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, bc_addr, ETH_ALEN);
	}

	_rtw_memcpy(pwlanhdr->addr2, mac, ETH_ALEN);

#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
	if ((pwdev_priv->random_mac_enabled)
	    && (MLME_IS_STA(padapter))
	    && (check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE) == _FALSE)) {
#ifdef CONFIG_RTW_DEBUG
		RTW_DBG("%s pno_scan_seq_num: %d\n", __func__,
			 pwdev_priv->pno_scan_seq_num);
#endif
		SetSeqNum(pwlanhdr, pwdev_priv->pno_scan_seq_num);
		pattrib->seqnum = pwdev_priv->pno_scan_seq_num;
		pattrib->qos_en = 1;
		pwdev_priv->pno_scan_seq_num++;
	} else
#endif
	{
		SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
		pmlmeext->mgnt_seq++;
	}
	set_frame_sub_type(pframe, WIFI_PROBEREQ);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	if (pssid && !MLME_IS_MESH(padapter))
		pframe = rtw_set_ie(pframe, _SSID_IE_, pssid->SsidLength, pssid->Ssid, &(pattrib->pktlen));
	else
		pframe = rtw_set_ie(pframe, _SSID_IE_, 0, NULL, &(pattrib->pktlen));

	get_rate_set(padapter, padapter_link, bssrate, &bssrate_len);

	if (bssrate_len > 8) {
		pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_ , 8, bssrate, &(pattrib->pktlen));
		pframe = rtw_set_ie(pframe, _EXT_SUPPORTEDRATES_IE_ , (bssrate_len - 8), (bssrate + 8), &(pattrib->pktlen));
	} else
		pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_ , bssrate_len , bssrate, &(pattrib->pktlen));

	if (ch)
		pframe = rtw_set_ie(pframe, _DSSET_IE_, 1, &ch, &pattrib->pktlen);

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(padapter)) {
		if (pssid)
			pframe = rtw_set_ie_mesh_id(pframe, &pattrib->pktlen, pssid->Ssid, pssid->SsidLength);
		else
			pframe = rtw_set_ie_mesh_id(pframe, &pattrib->pktlen, NULL, 0);
	}
#endif

	if (append_wps) {
		/* add wps_ie for wps2.0 */
		if (pmlmepriv->wps_probe_req_ie_len > 0 && pmlmepriv->wps_probe_req_ie) {
			_rtw_memcpy(pframe, pmlmepriv->wps_probe_req_ie, pmlmepriv->wps_probe_req_ie_len);
			pframe += pmlmepriv->wps_probe_req_ie_len;
			pattrib->pktlen += pmlmepriv->wps_probe_req_ie_len;
			/* pmlmepriv->wps_probe_req_ie_len = 0 ; */ /* reset to zero */
		}
	}
#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
	pattrib->pktlen += rtw_build_vendor_ie(padapter , &pframe , WIFI_PROBEREQ_VENDOR_IE_BIT);
#endif

#ifdef CONFIG_RTW_MBO
	rtw_mbo_build_probe_req_ies(padapter, &pframe, pattrib);
#endif
	pattrib->last_txcmdsz = pattrib->pktlen;


	if (wait_ack)
		ret = dump_mgntframe_and_wait_ack(padapter, pmgntframe);
	else {
		dump_mgntframe(padapter, pmgntframe);
		ret = _SUCCESS;
	}

exit:
	return ret;
}

inline void issue_probereq(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			const NDIS_802_11_SSID *pssid, const u8 *da)
{
	_issue_probereq(padapter, padapter_link, pssid, da, 0, 1, _FALSE);
}

/*
 * wait_ms == 0 means that there is no need to wait ack through C2H_CCX_TX_RPT
 * wait_ms > 0 means you want to wait ack through C2H_CCX_TX_RPT, and the value of wait_ms means the interval between each TX
 * try_cnt means the maximal TX count to try
 */
int issue_probereq_ex(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			const NDIS_802_11_SSID *pssid, const u8 *da, u8 ch, bool append_wps,
			int try_cnt, int wait_ms)
{
	int ret = _FAIL;
	int i = 0;
	systime start = rtw_get_current_time();

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	do {
		ret = _issue_probereq(padapter, padapter_link,
					pssid, da, ch, append_wps, wait_ms > 0 ? _TRUE : _FALSE);

		i++;

		if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
			break;

		if (i < try_cnt && wait_ms > 0 && ret == _FAIL)
			rtw_msleep_os(wait_ms);

	} while ((i < try_cnt) && ((ret == _FAIL) || (wait_ms == 0)));

	if (ret != _FAIL) {
		ret = _SUCCESS;
#ifndef DBG_XMIT_ACK
		goto exit;
#endif
	}

	if (try_cnt && wait_ms) {
		if (da)
			RTW_INFO(FUNC_ADPT_FMT" to "MAC_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(padapter), MAC_ARG(da),
				rtw_get_oper_ch(padapter, padapter_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
		else
			RTW_INFO(FUNC_ADPT_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(padapter),
				rtw_get_oper_ch(padapter, padapter_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
	}
exit:
	return ret;
}

/* if psta == NULL, indiate we are station(client) now... */
void issue_auth(_adapter *padapter, struct sta_info *psta, unsigned short status)
{
	struct xmit_frame			*pmgntframe;
	struct pkt_attrib			*pattrib;
	unsigned char					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short				*fctrl;
	unsigned int					val32;
	unsigned short				val16;
	int use_shared_key = 0;
	struct xmit_priv			*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct _ADAPTER_LINK *padapter_link = NULL;
	struct link_mlme_ext_info *lmlmeinfo;

	if (psta) /* AP Mode */
		padapter_link = psta->padapter_link;
	else /* STA Mode */
		/* Send auth req via the primary link */
		padapter_link = GET_PRIMARY_LINK(padapter);

 	lmlmeinfo = &padapter_link->mlmeextpriv.mlmext_info;

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_AUTH);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);


	if (psta) { /* for AP mode */
#ifdef CONFIG_NATIVEAP_MLME
		_rtw_memcpy(pwlanhdr->addr1, psta->phl_sta->mac_addr, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, padapter_link->mac_addr, ETH_ALEN);


		/* setting auth algo number */
		val16 = (u16)psta->authalg;

		if (status != _STATS_SUCCESSFUL_)
			val16 = 0;

		if (val16)	{
			val16 = cpu_to_le16(val16);
			use_shared_key = 1;
		}

		pframe = rtw_set_fixed_ie(pframe, _AUTH_ALGM_NUM_, (unsigned char *)&val16, &(pattrib->pktlen));

		/* setting auth seq number */
		val16 = (u16)psta->auth_seq;
		val16 = cpu_to_le16(val16);
		pframe = rtw_set_fixed_ie(pframe, _AUTH_SEQ_NUM_, (unsigned char *)&val16, &(pattrib->pktlen));

		/* setting status code... */
		val16 = status;
		val16 = cpu_to_le16(val16);
		pframe = rtw_set_fixed_ie(pframe, _STATUS_CODE_, (unsigned char *)&val16, &(pattrib->pktlen));

		/* added challenging text... */
		if ((psta->auth_seq == 2) && (psta->state & WIFI_FW_AUTH_STATE) && (use_shared_key == 1))
			pframe = rtw_set_ie(pframe, _CHLGETXT_IE_, 128, psta->chg_txt, &(pattrib->pktlen));
#endif
	} else {
		_rtw_memcpy(pwlanhdr->addr1, get_my_bssid(&lmlmeinfo->network), ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&lmlmeinfo->network), ETH_ALEN);

#ifdef CONFIG_RTW_80211R
		if (rtw_ft_roam(padapter)) {
			/* 2: 802.11R FTAA */
			val16 = cpu_to_le16(WLAN_AUTH_FT);
		} else
#endif
		{
			/* setting auth algo number */
			val16 = (pmlmeinfo->auth_algo == dot11AuthAlgrthm_Shared) ? 1 : 0;	/* 0:OPEN System, 1:Shared key */
			if (val16) {
				val16 = cpu_to_le16(val16);
				use_shared_key = 1;
			}
		}

		/* RTW_INFO("%s auth_algo= %s auth_seq=%d\n",__FUNCTION__,(pmlmeinfo->auth_algo==0)?"OPEN":"SHARED",pmlmeinfo->auth_seq); */

		/* setting IV for auth seq #3 */
		if ((pmlmeinfo->auth_seq == 3) && (pmlmeinfo->state & WIFI_FW_AUTH_STATE) && (use_shared_key == 1)) {
			/* RTW_INFO("==> iv(%d),key_index(%d)\n",pmlmeinfo->iv,pmlmeinfo->key_index); */
			val32 = ((pmlmeinfo->iv++) | (psecuritypriv->dot11PrivacyKeyIndex << 30));
			val32 = cpu_to_le32(val32);
			pframe = rtw_set_fixed_ie(pframe, 4, (unsigned char *)&val32, &(pattrib->pktlen));

			pattrib->iv_len = 4;
		}

		pframe = rtw_set_fixed_ie(pframe, _AUTH_ALGM_NUM_, (unsigned char *)&val16, &(pattrib->pktlen));

		/* setting auth seq number */
		val16 = pmlmeinfo->auth_seq;
		val16 = cpu_to_le16(val16);
		pframe = rtw_set_fixed_ie(pframe, _AUTH_SEQ_NUM_, (unsigned char *)&val16, &(pattrib->pktlen));


		/* setting status code... */
		val16 = status;
		val16 = cpu_to_le16(val16);
		pframe = rtw_set_fixed_ie(pframe, _STATUS_CODE_, (unsigned char *)&val16, &(pattrib->pktlen));

#ifdef CONFIG_RTW_80211R
		rtw_ft_build_auth_req_ies(padapter, pattrib, &pframe);
#endif

#ifdef CONFIG_80211BE_EHT
		if (padapter->mlmepriv.mlopriv.mlo_option)
			pframe = rtw_build_ml_ies(padapter, NULL, pframe, pattrib, PACKET_AUTH, NULL, 0, 0);
#endif

		/* then checking to see if sending challenging text... */
		if ((pmlmeinfo->auth_seq == 3) && (pmlmeinfo->state & WIFI_FW_AUTH_STATE) && (use_shared_key == 1)) {
			pframe = rtw_set_ie(pframe, _CHLGETXT_IE_, 128, pmlmeinfo->chg_txt, &(pattrib->pktlen));

			SetPrivacy(fctrl);

			pattrib->hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);

			pattrib->encrypt = _WEP40_;

			pattrib->icv_len = 4;

			pattrib->pktlen += pattrib->icv_len;

			pattrib->bswenc = _TRUE;
		}

	}

	pattrib->last_txcmdsz = pattrib->pktlen;

	rtw_wep_encrypt(padapter, (u8 *)pmgntframe);
	RTW_INFO("%s\n", __FUNCTION__);
	dump_mgntframe(padapter, pmgntframe);

	return;
}


void issue_asocrsp(_adapter *padapter, unsigned short status, struct sta_info *pstat, int pkt_type)
{
#ifdef CONFIG_AP_MODE
	struct xmit_frame	*pmgntframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	struct pkt_attrib *pattrib;
	unsigned char	*pbuf, *pframe;
	unsigned short val, ie_status;
	unsigned short *fctrl;
	struct xmit_priv *pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct _ADAPTER_LINK	*padapter_link = pstat->padapter_link;
	struct link_mlme_priv	*pmlmepriv = &(padapter_link->mlmepriv);
	WLAN_BSSID_EX *pnetwork = &padapter_link->mlmeextpriv.mlmext_info.network;
	u8 *ie = pnetwork->IEs;
	uint ie_len = 0;
#ifdef CONFIG_P2P
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
#ifdef CONFIG_WFD
	u32					wfdielen = 0;
#endif
#endif /* CONFIG_P2P */
#ifdef CONFIG_RTW_MBO
	u8 WIFI_ALLIANCE_OUI[] = {0x50, 0x6f, 0x9a};
#endif /* CONFIG_RTW_MBO */
	struct security_priv *psecuritypriv = &(padapter->securitypriv);

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	RTW_INFO("%s\n", __FUNCTION__);

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);


	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy((void *)GetAddr1Ptr(pwlanhdr), pstat->phl_sta->mac_addr, ETH_ALEN);
	_rtw_memcpy((void *)get_addr2_ptr(pwlanhdr), padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy((void *)GetAddr3Ptr(pwlanhdr), get_my_bssid(pnetwork), ETH_ALEN);


	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	if ((pkt_type == WIFI_ASSOCRSP) || (pkt_type == WIFI_REASSOCRSP))
		set_frame_sub_type(pwlanhdr, pkt_type);
	else
		return;

	pattrib->hdrlen = sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen += pattrib->hdrlen;
	pframe += pattrib->hdrlen;

	/* capability */
	val = *(unsigned short *)rtw_get_capability_from_ie(ie);

	pframe = rtw_set_fixed_ie(pframe, _CAPABILITY_ , (unsigned char *)&val, &(pattrib->pktlen));

	ie_status = cpu_to_le16(status);
	pframe = rtw_set_fixed_ie(pframe , _STATUS_CODE_ , (unsigned char *)&ie_status, &(pattrib->pktlen));

	val = cpu_to_le16(pstat->phl_sta->aid | BIT(14) | BIT(15));
	pframe = rtw_set_fixed_ie(pframe, _ASOC_ID_ , (unsigned char *)&val, &(pattrib->pktlen));

	if (pstat->bssratelen <= 8)
		pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_, pstat->bssratelen, pstat->bssrateset, &(pattrib->pktlen));
	else {
		pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_, 8, pstat->bssrateset, &(pattrib->pktlen));
		pframe = rtw_set_ie(pframe, _EXT_SUPPORTEDRATES_IE_, (pstat->bssratelen - 8), pstat->bssrateset + 8, &(pattrib->pktlen));
	}

#ifdef CONFIG_IEEE80211W
	if (status == _STATS_REFUSED_TEMPORARILY_) {
		u8 timeout_itvl[5];
		u32 timeout_interval = 3000;
		/* Association Comeback time */
		timeout_itvl[0] = 0x03;
		timeout_interval = cpu_to_le32(timeout_interval);
		_rtw_memcpy(timeout_itvl + 1, &timeout_interval, 4);
		pframe = rtw_set_ie(pframe, _TIMEOUT_ITVL_IE_, 5, timeout_itvl, &(pattrib->pktlen));
	}
#endif /* CONFIG_IEEE80211W */

#ifdef CONFIG_80211N_HT
	if ((pstat->flags & WLAN_STA_HT) && (pmlmepriv->htpriv.ht_option)) {
		uint ie_len = 0;

		/* FILL HT CAP INFO IE */
		/* p = hostapd_eid_ht_capabilities_info(hapd, p); */
		pbuf = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_CAPABILITY_IE_, &ie_len, (pnetwork->IELength - _BEACON_IE_OFFSET_));
		if (pbuf && ie_len > 0) {
			_rtw_memcpy(pframe, pbuf, ie_len + 2);
			pframe += (ie_len + 2);
			pattrib->pktlen += (ie_len + 2);
		}

		/* FILL HT ADD INFO IE */
		/* p = hostapd_eid_ht_operation(hapd, p); */
		pbuf = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_ADD_INFO_IE_, &ie_len, (pnetwork->IELength - _BEACON_IE_OFFSET_));
		if (pbuf && ie_len > 0) {
			_rtw_memcpy(pframe, pbuf, ie_len + 2);
			pframe += (ie_len + 2);
			pattrib->pktlen += (ie_len + 2);
		}

	}
#endif

	/*adding EXT_CAPAB_IE */
	if (pmlmepriv->ext_capab_ie_len > 0) {
		uint ie_len = 0;

		pbuf = rtw_get_ie(ie + _BEACON_IE_OFFSET_, WLAN_EID_EXT_CAP, &ie_len, (pnetwork->IELength - _BEACON_IE_OFFSET_));
		if (pbuf && ie_len > 0) {
			_rtw_memcpy(pframe, pbuf, ie_len + 2);
			pframe += (ie_len + 2);
			pattrib->pktlen += (ie_len + 2);
		}
	}

#ifdef CONFIG_RTW_80211K
	/* Adding RM capability IE */
	if (padapter->rmpriv.enable == _TRUE) {
		pbuf = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _EID_RRM_EN_CAP_IE_, &ie_len, (pnetwork->IELength - _BEACON_IE_OFFSET_));
		if (pbuf && ie_len > 0) {
			_rtw_memcpy(pframe, pbuf, ie_len + 2);
			pframe += (ie_len + 2);
			pattrib->pktlen += (ie_len + 2);
		}
	}
#endif /* CONFIG_RTW_80211K */

#ifdef CONFIG_RTW_MBO
	if (padapter->mlmepriv.mbopriv.enable == _TRUE) {
		ie_len = 0;
		for (pbuf = ie + _BEACON_IE_OFFSET_; ; pbuf += (ie_len + 2)) {
			pbuf = rtw_get_ie(pbuf, _SSN_IE_1_, &ie_len, (pnetwork->IELength - _BEACON_IE_OFFSET_ - (ie_len + 2)));
			if ((pbuf) && (_rtw_memcmp(pbuf + 2, WIFI_ALLIANCE_OUI, 3)) && (*(pbuf+5) == MBO_OUI_TYPE)) {
				/* find MBO-OCE information element */
				_rtw_memcpy(pframe, pbuf, ie_len + 2);
				pframe += (ie_len + 2);
				pattrib->pktlen += (ie_len + 2);
				break;
			}
			if ((pbuf == NULL) || (ie_len == 0))
				break;
		}
	}
#endif /* CONFIG_RTW_MBO */

#ifdef CONFIG_80211AC_VHT
	if ((pstat->flags & WLAN_STA_VHT) && (pmlmepriv->vhtpriv.vht_option)
	    && (pstat->wpa_pairwise_cipher != WPA_CIPHER_TKIP)
	    && (pstat->wpa2_pairwise_cipher != WPA_CIPHER_TKIP)) {
		u32 ie_len = 0;

		/* FILL VHT CAP IE */
		pbuf = rtw_get_ie(ie + _BEACON_IE_OFFSET_, EID_VHTCapability, &ie_len, (pnetwork->IELength - _BEACON_IE_OFFSET_));
		if (pbuf && ie_len > 0) {
			_rtw_memcpy(pframe, pbuf, ie_len + 2);
			pframe += (ie_len + 2);
			pattrib->pktlen += (ie_len + 2);
		}

		/* FILL VHT OPERATION IE */
		pbuf = rtw_get_ie(ie + _BEACON_IE_OFFSET_, EID_VHTOperation, &ie_len, (pnetwork->IELength - _BEACON_IE_OFFSET_));
		if (pbuf && ie_len > 0) {
			_rtw_memcpy(pframe, pbuf, ie_len + 2);
			pframe += (ie_len + 2);
			pattrib->pktlen += (ie_len + 2);
		}
	}
#endif /* CONFIG_80211AC_VHT */

#ifdef CONFIG_80211AX_HE
	if ((pstat->flags & WLAN_STA_HE) && (pmlmepriv->hepriv.he_option)) {
		u32 ie_len = 0;
		u8 cap;

		/* FILL HE CAP IE */
		cap = WLAN_EID_EXTENSION_HE_CAPABILITY;
		pbuf = rtw_get_ie_ex(ie + _BEACON_IE_OFFSET_, pnetwork->IELength - _BEACON_IE_OFFSET_,
		WLAN_EID_EXTENSION, &cap, 1, NULL, &ie_len);
		if (pbuf && ie_len > 0) {
			_rtw_memcpy(pframe, pbuf, ie_len);
			pframe += ie_len;
			pattrib->pktlen += ie_len;
		}

		/* FILL HE OPERATION IE */
		cap = WLAN_EID_EXTENSION_HE_OPERATION;
		pbuf = rtw_get_ie_ex(ie + _BEACON_IE_OFFSET_, pnetwork->IELength - _BEACON_IE_OFFSET_,
		WLAN_EID_EXTENSION, &cap, 1, NULL, &ie_len);
		if (pbuf && ie_len > 0) {
			_rtw_memcpy(pframe, pbuf, ie_len);
			pframe += ie_len;
			pattrib->pktlen += ie_len;
		}
#if CONFIG_IEEE80211_BAND_6GHZ
		/* FILL HE 6G BAND CAP IE */
		cap = WLAN_EID_EXT_HE_6G_CAP;
		pbuf = rtw_get_ie_ex(ie + _BEACON_IE_OFFSET_, pnetwork->IELength - _BEACON_IE_OFFSET_,
		WLAN_EID_EXTENSION, &cap, 1, NULL, &ie_len);
		if (pbuf && ie_len > 0) {
			_rtw_memcpy(pframe, pbuf, ie_len);
			pframe += ie_len;
			pattrib->pktlen += ie_len;
		}
#endif
	}
#endif /* CONFIG_80211AX_HE */


#ifdef CONFIG_RTW_80211R_AP
	rtw_ft_build_assoc_rsp_ies(padapter, pstat, pattrib, &pframe);
#endif
#ifdef CONFIG_RTW_MBO
	rtw_mbo_build_assoc_rsp_ies(padapter, &pframe, pattrib);
#endif

	/* FILL WMM IE */
	if ((pstat->flags & WLAN_STA_WME) && (pmlmepriv->qospriv.qos_option)) {
		uint ie_len = 0;
		unsigned char WMM_PARA_IE[] = {0x00, 0x50, 0xf2, 0x02, 0x01, 0x01};

		for (pbuf = ie + _BEACON_IE_OFFSET_; ; pbuf += (ie_len + 2)) {
			pbuf = rtw_get_ie(pbuf, _VENDOR_SPECIFIC_IE_, &ie_len, (pnetwork->IELength - _BEACON_IE_OFFSET_ - (ie_len + 2)));
			if (pbuf && _rtw_memcmp(pbuf + 2, WMM_PARA_IE, 6)) {
				_rtw_memcpy(pframe, pbuf, ie_len + 2);
				pframe += (ie_len + 2);
				pattrib->pktlen += (ie_len + 2);

				break;
			}

			if ((pbuf == NULL) || (ie_len == 0))
				break;
		}

	}


	if (pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_REALTEK)
		pframe = rtw_set_ie(pframe, _VENDOR_SPECIFIC_IE_, 6 , REALTEK_96B_IE, &(pattrib->pktlen));

	/* add WPS IE ie for wps 2.0 */
	if (padapter->mlmepriv.wps_assoc_resp_ie && padapter->mlmepriv.wps_assoc_resp_ie_len > 0) {
		_rtw_memcpy(pframe, padapter->mlmepriv.wps_assoc_resp_ie, padapter->mlmepriv.wps_assoc_resp_ie_len);

		pframe += padapter->mlmepriv.wps_assoc_resp_ie_len;
		pattrib->pktlen += padapter->mlmepriv.wps_assoc_resp_ie_len;
	}


	if ((psecuritypriv->auth_type == MLME_AUTHTYPE_SAE) &&
		padapter->mlmepriv.assoc_rsp &&
			padapter->mlmepriv.assoc_rsp_len > 0) {
		_rtw_memcpy(pframe, padapter->mlmepriv.assoc_rsp,
			padapter->mlmepriv.assoc_rsp_len);
		pframe += padapter->mlmepriv.assoc_rsp_len;
		pattrib->pktlen += padapter->mlmepriv.assoc_rsp_len;
	}


#ifdef CONFIG_P2P
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO) && (pstat->is_p2p_device == _TRUE)) {
		u32 len = 0;

		if (padapter->mlmepriv.p2p_assoc_resp_ie && padapter->mlmepriv.p2p_assoc_resp_ie_len > 0) {
			len = padapter->mlmepriv.p2p_assoc_resp_ie_len;
			_rtw_memcpy(pframe, padapter->mlmepriv.p2p_assoc_resp_ie, len);
		} else if (padapter->mlmepriv.assoc_rsp && padapter->mlmepriv.assoc_rsp_len > 0) {
			len = padapter->mlmepriv.assoc_rsp_len;
			_rtw_memcpy(pframe, padapter->mlmepriv.assoc_rsp, len);
		}

		pframe += len;
		pattrib->pktlen += len;
	}

#ifdef CONFIG_WFD
	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO)) {
		wfdielen = rtw_append_assoc_resp_wfd_ie(padapter, pframe);
		pframe += wfdielen;
		pattrib->pktlen += wfdielen;
	}
#endif

#endif /* CONFIG_P2P */

#ifdef CONFIG_RTW_MULTI_AP
	if (padapter->multi_ap && (pstat->flags & WLAN_STA_MULTI_AP))
		pframe = rtw_set_multi_ap_ie_ext(pframe, &pattrib->pktlen, padapter->multi_ap);
#endif

#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
	pattrib->pktlen += rtw_build_vendor_ie(padapter , &pframe , WIFI_ASSOCRESP_VENDOR_IE_BIT);
#endif

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	if (padapter->tbtx_capability == _TRUE)
		pframe = rtw_set_ie(pframe, _VENDOR_SPECIFIC_IE_, 8, REALTEK_TBTX_IE, &pattrib->pktlen);
#endif

	pattrib->last_txcmdsz = pattrib->pktlen;

	dump_mgntframe(padapter, pmgntframe);

#endif
}

static u32 rtw_append_assoc_req_owe_ie(_adapter *adapter, u8 *pbuf)
{
	struct security_priv *sec = &adapter->securitypriv;
	u32 len = 0;

	if (sec == NULL)
		goto exit;

	if (sec->owe_ie_len > 0) {
		len = sec->owe_ie_len;
		_rtw_memcpy(pbuf, sec->owe_ie, len);
	}

exit:
	return len;
}

int rtw_build_assocreq_ies(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			u8 *pframe, struct pkt_attrib *pattrib, u8 is_reassoc)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	struct link_mlme_ext_priv *lmlmeext= &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info *lmlmeinfo = &lmlmeext->mlmext_info;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	unsigned short				val16;
#if CONFIG_DFS
	u16	cap;
#endif
	unsigned char	bssrate[NumRates], sta_bssrate[NumRates];
	int	bssrate_len = 0, sta_bssrate_len = 0;
	unsigned int	i, j, index = 0;
	PNDIS_802_11_VARIABLE_IEs	pIE;
	u8	vs_ie_length = 0;
#ifdef CONFIG_P2P
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	u8					p2pie[255] = { 0x00 };
	u16					p2pielen = 0;
#ifdef CONFIG_WFD
	u32					wfdielen = 0;
#endif
#endif /* CONFIG_P2P */
	int ret = _SUCCESS;

	/* caps */

#if CONFIG_DFS
	_rtw_memcpy(&cap, rtw_get_capability_from_ie(lmlmeinfo->network.IEs), 2);
	cap |= cap_SpecMgmt;
	_rtw_memcpy(pframe, &cap, 2);
#else
	_rtw_memcpy(pframe, rtw_get_capability_from_ie(lmlmeinfo->network.IEs), 2);
#endif

	pframe += 2;
	pattrib->pktlen += 2;

	/* listen interval */
	/* todo: listen interval for power saving */
	val16 = cpu_to_le16(3);
	_rtw_memcpy(pframe , (unsigned char *)&val16, 2);
	pframe += 2;
	pattrib->pktlen += 2;

	/*Construct Current AP Field for Reassoc-Req only*/
	if (is_reassoc == _TRUE) {
		_rtw_memcpy(pframe, get_my_bssid(&(pmlmeinfo->dev_network)), ETH_ALEN);
		pframe += ETH_ALEN;
		pattrib->pktlen += ETH_ALEN;
	}

	/* SSID */
	pframe = rtw_set_ie(pframe, _SSID_IE_, lmlmeinfo->network.Ssid.SsidLength, lmlmeinfo->network.Ssid.Ssid, &(pattrib->pktlen));
#if CONFIG_IEEE80211_BAND_5GHZ && CONFIG_DFS
	/* Dot H */
	if (lmlmeext->chandef.band == BAND_ON_5G) {
		struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
		u8 pow_cap_ele[2] = { 0x00 };

		pow_cap_ele[0] = 13;	/* Minimum transmit power capability */
		pow_cap_ele[1] = 21;	/* Maximum transmit power capability */
		pframe = rtw_set_ie(pframe, EID_PowerCap, 2, pow_cap_ele, &(pattrib->pktlen));

		pframe = rtw_chset_set_spt_chs_ie(&rfctl->chset, pframe, &(pattrib->pktlen));
	}
#endif /* CONFIG_IEEE80211_BAND_5GHZ && CONFIG_DFS */

	/* supported rate & extended supported rate */

#if 1	/* Check if the AP's supported rates are also supported by STA. */
	get_rate_set(padapter, padapter_link, sta_bssrate, &sta_bssrate_len);
	/* RTW_INFO("sta_bssrate_len=%d\n", sta_bssrate_len); */

	if (lmlmeext->chandef.chan == 14) /* for JAPAN, channel 14 can only uses B Mode(CCK) */
		sta_bssrate_len = 4;


	/* for (i = 0; i < sta_bssrate_len; i++) { */
	/*	RTW_INFO("sta_bssrate[%d]=%02X\n", i, sta_bssrate[i]); */
	/* } */

	for (i = 0; i < NDIS_802_11_LENGTH_RATES_EX; i++) {
		if (lmlmeinfo->network.SupportedRates[i] == 0)
			break;
		RTW_INFO("network.SupportedRates[%d]=%02X\n", i, lmlmeinfo->network.SupportedRates[i]);
	}


	for (i = 0; i < NDIS_802_11_LENGTH_RATES_EX; i++) {
		if (lmlmeinfo->network.SupportedRates[i] == 0)
			break;


		/* Check if the AP's supported rates are also supported by STA. */
		for (j = 0; j < sta_bssrate_len; j++) {
			/* Avoid the proprietary data rate (22Mbps) of Handlink WSG-4000 AP */
			if ((lmlmeinfo->network.SupportedRates[i] | IEEE80211_BASIC_RATE_MASK)
			    == (sta_bssrate[j] | IEEE80211_BASIC_RATE_MASK)) {
				/* RTW_INFO("match i = %d, j=%d\n", i, j); */
				break;
			} else {
				/* RTW_INFO("not match: %02X != %02X\n", (lmlmeinfo->network.SupportedRates[i]|IEEE80211_BASIC_RATE_MASK), (sta_bssrate[j]|IEEE80211_BASIC_RATE_MASK)); */
			}
		}

		if (j == sta_bssrate_len) {
			/* the rate is not supported by STA */
			RTW_INFO("%s(): the rate[%d]=%02X is not supported by STA!\n", __FUNCTION__, i, lmlmeinfo->network.SupportedRates[i]);
		} else {
			/* the rate is supported by STA */
			bssrate[index++] = lmlmeinfo->network.SupportedRates[i];
		}
	}

	bssrate_len = index;
	RTW_INFO("bssrate_len = %d\n", bssrate_len);

#else	/* Check if the AP's supported rates are also supported by STA. */
#if 0
	get_rate_set(padapter, bssrate, &bssrate_len);
#else
	for (bssrate_len = 0; bssrate_len < NumRates; bssrate_len++) {
		if (lmlmeinfo->network.SupportedRates[bssrate_len] == 0)
			break;

		if (lmlmeinfo->network.SupportedRates[bssrate_len] == 0x2C) /* Avoid the proprietary data rate (22Mbps) of Handlink WSG-4000 AP */
			break;

		bssrate[bssrate_len] = lmlmeinfo->network.SupportedRates[bssrate_len];
	}
#endif
#endif /* Check if the AP's supported rates are also supported by STA. */

	if ((bssrate_len == 0) && (lmlmeinfo->network.SupportedRates[0] != 0)) {
		#if 0 /*CONFIG_CORE_XMITBUF*/
		rtw_free_xmitbuf(pxmitpriv, pmgntframe->pxmitbuf);
		#endif
		ret = _FAIL;
		goto _exit; /* don't connect to AP if no joint supported rate */
	}


	if (bssrate_len > 8) {
		pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_ , 8, bssrate, &(pattrib->pktlen));
		pframe = rtw_set_ie(pframe, _EXT_SUPPORTEDRATES_IE_ , (bssrate_len - 8), (bssrate + 8), &(pattrib->pktlen));
	} else if (bssrate_len > 0)
		pframe = rtw_set_ie(pframe, _SUPPORTEDRATES_IE_ , bssrate_len , bssrate, &(pattrib->pktlen));
	else
		RTW_INFO("%s: Connect to AP without 11b and 11g data rate!\n", __FUNCTION__);

#ifdef CONFIG_ECSA_PHL
	if (rtw_is_ecsa_enabled(padapter))
		pframe = build_supported_op_class_ie(padapter, pframe, &pattrib->pktlen);
#endif

#ifdef CONFIG_RTW_MBO
	rtw_mbo_build_assoc_req_ies(padapter, &pframe, pattrib);
#endif
#ifdef CONFIG_RTW_80211R
	rtw_ft_build_assoc_req_ies(padapter, is_reassoc, pattrib, &pframe);
#endif
#ifdef CONFIG_RTW_80211K
	if (lmlmeinfo->network.PhyInfo.rm_en_cap[0] /* RM Enabled Capabilities */
		| lmlmeinfo->network.PhyInfo.rm_en_cap[1]
		| lmlmeinfo->network.PhyInfo.rm_en_cap[2]
		| lmlmeinfo->network.PhyInfo.rm_en_cap[3]
		| lmlmeinfo->network.PhyInfo.rm_en_cap[4])
		pframe = rtw_set_ie(pframe, _EID_RRM_EN_CAP_IE_, 5,
				(u8 *)padapter->rmpriv.rm_en_cap_def, &(pattrib->pktlen));
#endif /* CONFIG_RTW_80211K */

	/* vendor specific IE, such as WPA, WMM, WPS */
	for (i = sizeof(NDIS_802_11_FIXED_IEs); i < lmlmeinfo->network.IELength;) {
		pIE = (PNDIS_802_11_VARIABLE_IEs)(lmlmeinfo->network.IEs + i);
		switch (pIE->ElementID) {
		case _VENDOR_SPECIFIC_IE_:
			if ((_rtw_memcmp(pIE->data, RTW_WPA_OUI, 4)) ||
			    (_rtw_memcmp(pIE->data, WMM_OUI, 4)) ||
			    (_rtw_memcmp(pIE->data, WPS_OUI, 4))) {
				vs_ie_length = pIE->Length;
				if ((!padapter->registrypriv.wifi_spec) && (_rtw_memcmp(pIE->data, WPS_OUI, 4))) {
					/* Commented by Kurt 20110629 */
					/* In some older APs, WPS handshake */
					/* would be fail if we append vender extensions informations to AP */

					vs_ie_length = 14;
				}

				pframe = rtw_set_ie(pframe, _VENDOR_SPECIFIC_IE_, vs_ie_length, pIE->data, &(pattrib->pktlen));
			}
			break;

		case EID_WPA2:
#ifdef CONFIG_RTW_80211R
			if ((is_reassoc) && (rtw_ft_roam(padapter))) {
				rtw_ft_update_rsnie(padapter, _TRUE, pattrib, &pframe);
			} else
#endif
			{
				pframe = rtw_set_ie(pframe, EID_WPA2, pIE->Length, pIE->data, &(pattrib->pktlen));
				/* tmp: update rsn's spp related opt. */
				/*rtw_set_spp_amsdu_mode(padapter->registrypriv.amsdu_mode, pframe - (pIE->Length + 2), pIE->Length +2); */

			}
			break;

		case WLAN_EID_RSNX:
			pframe = rtw_set_ie(pframe, WLAN_EID_RSNX, pIE->Length,
					pIE->data, &(pattrib->pktlen));
			break;

#ifdef CONFIG_80211N_HT
		case EID_HTCapability:
			if (pmlmepriv->htpriv.ht_option == _TRUE) {
				if (!(is_ap_in_tkip(padapter_link))) {
					_rtw_memcpy(&(lmlmeinfo->HT_caps), pIE->data, sizeof(struct HT_caps_element));

					lmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info = cpu_to_le16(lmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info);

					#ifdef CONFIG_DBCC_SUPPORT
					if (rtw_phl_mr_is_db(adapter_to_dvobj(padapter)->phl)) {
						RTW_INFO("%s, stay in dual-band set SM_PS\n", __func__);
						SET_HT_CAP_ELE_SM_PS(&(lmlmeinfo->HT_caps.u.HT_cap_element.HT_caps_info), SM_PS_STATIC);
					}
					#endif
					pframe = rtw_set_ie(pframe, EID_HTCapability, pIE->Length , (u8 *)(&(lmlmeinfo->HT_caps)), &(pattrib->pktlen));
				}
			}
			break;
#endif /* CONFIG_80211N_HT */

		case WLAN_EID_EXT_CAP:
			pframe = rtw_set_ie(pframe, WLAN_EID_EXT_CAP, pIE->Length, pIE->data, &(pattrib->pktlen));
			break;

#ifdef CONFIG_80211AC_VHT
		case EID_VHTCapability:
			if (pmlmepriv->vhtpriv.vht_option == _TRUE)
				pframe = rtw_set_ie(pframe, EID_VHTCapability, pIE->Length, pIE->data, &(pattrib->pktlen));
			break;

		case EID_OpModeNotification:
			if (pmlmepriv->vhtpriv.vht_option == _TRUE) {
#if 0
				if (rtw_phl_mr_is_db(adapter_to_dvobj(padapter)->phl)) {
					u8 orig_ie_rx_nss = GET_VHT_OPERATING_MODE_FIELD_RX_NSS(pIE->data);
					if (orig_ie_rx_nss > 0) {
						RTW_INFO("%s, change op_mode_notify_ie(RX NSS:%d->0)\n", __func__, orig_ie_rx_nss);
						/* change op mode notify IE */
						SET_VHT_OPERATING_MODE_FIELD_RX_NSS(pIE->data, 0);
						SET_VHT_OPERATING_MODE_FIELD_RX_NSS_TYPE(pIE->data, 0);

					}

				}
#endif
				pframe = rtw_set_ie(pframe, EID_OpModeNotification, pIE->Length, pIE->data, &(pattrib->pktlen));
			}
			break;
#endif /* CONFIG_80211AC_VHT */
		case WLAN_EID_EXTENSION:
#ifdef CONFIG_80211AX_HE
			if ((pIE->data[0] == WLAN_EID_EXTENSION_HE_CAPABILITY
				|| pIE->data[0] == WLAN_EID_EXT_HE_6G_CAP)
				&& (pmlmepriv->hepriv.he_option == _TRUE))
				pframe = rtw_set_ie(pframe, WLAN_EID_EXTENSION, pIE->Length, pIE->data, &(pattrib->pktlen));
#endif /* CONFIG_80211AX_HE */
			break;
		default:
			break;
		}

		i += (pIE->Length + 2);
	}
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	if (padapter->tbtx_capability == _TRUE)
		pframe = rtw_set_ie(pframe, _VENDOR_SPECIFIC_IE_, 8 , REALTEK_TBTX_IE, &(pattrib->pktlen));
#endif

	if (pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_REALTEK)
		pframe = rtw_set_ie(pframe, _VENDOR_SPECIFIC_IE_, 6 , REALTEK_96B_IE, &(pattrib->pktlen));


#ifdef CONFIG_WAPI_SUPPORT
	rtw_build_assoc_req_wapi_ie(padapter, pframe, pattrib);
#endif


#ifdef CONFIG_P2P

#ifdef CONFIG_IOCTL_CFG80211
	if (adapter_wdev_data(padapter)->p2p_enabled) {
		if (padapter->mlmepriv.p2p_assoc_req_ie && padapter->mlmepriv.p2p_assoc_req_ie_len > 0) {
			_rtw_memcpy(pframe, padapter->mlmepriv.p2p_assoc_req_ie, padapter->mlmepriv.p2p_assoc_req_ie_len);
			pframe += padapter->mlmepriv.p2p_assoc_req_ie_len;
			pattrib->pktlen += padapter->mlmepriv.p2p_assoc_req_ie_len;
		}
	}
#endif /* CONFIG_IOCTL_CFG80211 */

#ifdef CONFIG_WFD
	wfdielen = rtw_append_assoc_req_wfd_ie(padapter, pframe);
	pframe += wfdielen;
	pattrib->pktlen += wfdielen;
#endif
#endif /* CONFIG_P2P */

#ifdef CONFIG_RTW_MULTI_AP
	if (padapter->multi_ap)
		pframe = rtw_set_multi_ap_ie_ext(pframe, &pattrib->pktlen, padapter->multi_ap);
#endif

	/* OWE */
	{
	u32 owe_ie_len;

	owe_ie_len = rtw_append_assoc_req_owe_ie(padapter, pframe);
	pframe += owe_ie_len;
	pattrib->pktlen += owe_ie_len;
	}

#ifdef CONFIG_APPEND_VENDOR_IE_ENABLE
	pattrib->pktlen += rtw_build_vendor_ie(padapter, &pframe ,WIFI_ASSOCREQ_VENDOR_IE_BIT);
#endif

_exit:
	return ret;
}

void _issue_assocreq(_adapter *padapter, u8 is_reassoc)
{
	int ret = _FAIL;
	struct xmit_frame				*pmgntframe;
	struct pkt_attrib				*pattrib;
	unsigned char					*pframe;
	struct rtw_ieee80211_hdr			*pwlanhdr;
	unsigned short				*fctrl;
	struct xmit_priv		*pxmitpriv = &(padapter->xmitpriv);
	/* Send assoc req via the primary link */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
#ifdef CONFIG_80211BE_EHT
	u8 *primary_ies;
	u32 primary_ies_len;
#endif

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		goto exit;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);


	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;
	_rtw_memcpy(pwlanhdr->addr1, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);

	SetSeqNum(pwlanhdr, padapter->mlmeextpriv.mgnt_seq);
	padapter->mlmeextpriv.mgnt_seq++;
	if (is_reassoc == _TRUE)
		set_frame_sub_type(pframe, WIFI_REASSOCREQ);
	else
		set_frame_sub_type(pframe, WIFI_ASSOCREQ);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

#ifdef CONFIG_80211BE_EHT
	primary_ies = pframe;
#endif
	ret = rtw_build_assocreq_ies(padapter, padapter_link, pframe, pattrib, is_reassoc);
	pframe += pattrib->pktlen - sizeof(struct rtw_ieee80211_hdr_3addr);

	if (ret != _SUCCESS) {
		rtw_free_xmitframe(pxmitpriv, pmgntframe);
		goto exit; /* don't connect to AP if no joint supported rate */
	}

#ifdef CONFIG_80211BE_EHT
	primary_ies_len = pframe - primary_ies;
	if (padapter->mlmepriv.mlopriv.mlo_option)
		pframe = rtw_build_ml_ies(padapter, NULL, pframe, pattrib, PACKET_ASSOC_REQUEST,
					primary_ies, primary_ies_len, is_reassoc);
#endif

	pattrib->last_txcmdsz = pattrib->pktlen;
	dump_mgntframe(padapter, pmgntframe);

	ret = _SUCCESS;

exit:
	if (ret == _SUCCESS) {
		rtw_buf_update(&padapter->mlmepriv.assoc_req, &padapter->mlmepriv.assoc_req_len, (u8 *)pwlanhdr, pattrib->pktlen);
	#ifdef CONFIG_RTW_WNM
		if (is_reassoc == _TRUE)
			rtw_wnm_update_reassoc_req_ie(padapter);
	#endif
	} else
		rtw_buf_free(&padapter->mlmepriv.assoc_req, &padapter->mlmepriv.assoc_req_len);

	return;
}

void issue_assocreq(_adapter *padapter)
{
	_issue_assocreq(padapter, _FALSE);
}

void issue_reassocreq(_adapter *padapter)
{
	_issue_assocreq(padapter, _TRUE);
}

/* when wait_ack is ture, this function shoule be called at process context */
static int _issue_nulldata(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			unsigned char *da, unsigned int power_mode, int wait_ack)
{
	int ret = _FAIL;
	struct xmit_frame			*pmgntframe;
	struct pkt_attrib			*pattrib;
	unsigned char					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short				*fctrl;
	struct xmit_priv	*pxmitpriv;
	struct mlme_ext_priv	*pmlmeext;
	struct link_mlme_ext_info	*pmlmeinfo;
	u8 a4_shift;

	/* RTW_INFO("%s:%d\n", __FUNCTION__, power_mode); */

	if (!padapter)
		goto exit;

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	pxmitpriv = &(padapter->xmitpriv);
	pmlmeext = &(padapter->mlmeextpriv);
	pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		goto exit;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);
	pattrib->retry_ctrl = _FALSE;

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	if (MLME_IS_AP(padapter))
		SetFrDs(fctrl);
	else if (MLME_IS_STA(padapter))
		SetToDs(fctrl);
	else if (MLME_IS_MESH(padapter)) {
		SetToDs(fctrl);
		SetFrDs(fctrl);
	}

	if (power_mode)
		SetPwrMgt(fctrl);

	if (get_tofr_ds(fctrl) == 3) {
		_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, da, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr4, padapter_link->mac_addr, ETH_ALEN);
		a4_shift = ETH_ALEN;
		pattrib->hdrlen += ETH_ALEN;
	} else {
		_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);
		a4_shift = 0;
	}

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_DATA_NULL);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr) + a4_shift;
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr) + a4_shift;

	pattrib->last_txcmdsz = pattrib->pktlen;

	if (wait_ack)
		ret = dump_mgntframe_and_wait_ack(padapter, pmgntframe);
	else {
		dump_mgntframe(padapter, pmgntframe);
		ret = _SUCCESS;
	}

exit:
	return ret;
}

/*
 * When wait_ms > 0, this function should be called at process context
 * wait_ms == 0 means that there is no need to wait ack through C2H_CCX_TX_RPT
 * wait_ms > 0 means you want to wait ack through C2H_CCX_TX_RPT, and the value of wait_ms means the interval between each TX
 * try_cnt means the maximal TX count to try
 * da == NULL for station mode
 */
int issue_nulldata(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			unsigned char *da, unsigned int power_mode, int try_cnt, int wait_ms)
{
	int ret = _FAIL;
	int i = 0;
	systime start = rtw_get_current_time();
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	/* da == NULL, assum it's null data for sta to ap */
	if (da == NULL)
		da = get_my_bssid(&(pmlmeinfo->network));

	do {
		ret = _issue_nulldata(padapter, padapter_link,
					da, power_mode, wait_ms > 0 ? _TRUE : _FALSE);

		i++;

		if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
			break;

		if (i < try_cnt && wait_ms > 0 && ret == _FAIL)
			rtw_msleep_os(wait_ms);

	} while ((i < try_cnt) && ((ret == _FAIL) || (wait_ms == 0)));

	if (ret != _FAIL) {
		ret = _SUCCESS;
#ifndef DBG_XMIT_ACK
		goto exit;
#endif
	}

	if (try_cnt && wait_ms) {
		if (da)
			RTW_INFO(FUNC_ADPT_FMT" to "MAC_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(padapter), MAC_ARG(da),
				rtw_get_oper_ch(padapter, padapter_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
		else
			RTW_INFO(FUNC_ADPT_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(padapter),
				rtw_get_oper_ch(padapter, padapter_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
	}
exit:
	return ret;
}

static inline void _ps_announce(_adapter *adapter, struct _ADAPTER_LINK *adapter_link, bool ps)
{
	RTW_INFO(FUNC_ADPT_FMT" issue_null(%d)\n", FUNC_ADPT_ARG(adapter), ps);
	if (MLME_IS_STA(adapter)) {
		if (is_client_associated_to_ap(adapter) == _TRUE) {

			#ifdef CONFIG_XMIT_ACK
			if (adapter_link->wrlink->mstate == MLME_LINKED)
				issue_nulldata(adapter, adapter_link, NULL, ps, 3, 500);
			#else
			if (adapter_link->wrlink->mstate == MLME_LINKED)
				issue_nulldata(adapter, adapter_link, NULL, ps, 1, 0);
			#endif
		}
	}
	#ifdef CONFIG_RTW_MESH
	else if (MLME_IS_MESH(adapter)) {
		rtw_mesh_ps_annc(adapter, ps);
	}
	#endif
}

bool rtw_core_issu_null_data(void *priv, u8 ridx, u8 link_idx, bool ps)
{
	struct dvobj_priv *obj = (struct dvobj_priv *)priv;
	_adapter *iface = NULL;
	struct _ADAPTER_LINK *iface_link = NULL;

	if (ridx >= CONFIG_IFACE_NUMBER) {
		RTW_ERR("%s ridx:%d invalid\n", __func__, ridx);
		rtw_warn_on(1);
		goto _error;
	}

	iface = obj->padapters[ridx];
	if (!rtw_is_adapter_up(iface))
		goto _error;

	iface_link = GET_LINK(iface, link_idx);
	_ps_announce(iface, iface_link, ps);

	return _SUCCESS;
_error:
	return _FAIL;
}


/* when wait_ack is ture, this function shoule be called at process context */
static int _issue_qos_nulldata(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
				unsigned char *da, u16 tid, u8 ps, int wait_ack, u8 om_info)
{
	int ret = _FAIL;
	struct xmit_frame			*pmgntframe;
	struct pkt_attrib			*pattrib;
	unsigned char					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short				*fctrl, *qc;
	struct xmit_priv			*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct link_mlme_ext_info	*pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);
	u8 a4_shift;

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	/* RTW_INFO("%s\n", __FUNCTION__); */

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		goto exit;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	pattrib->hdrlen += 2;
	pattrib->qos_en = _TRUE;
	pattrib->eosp = 1;
	pattrib->ack_policy = 0;
	pattrib->mdata = 0;

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	if (MLME_IS_AP(padapter))
		SetFrDs(fctrl);
	else if (MLME_IS_STA(padapter))
		SetToDs(fctrl);
	else if (MLME_IS_MESH(padapter)) {
		SetToDs(fctrl);
		SetFrDs(fctrl);
	}

	if (ps)
		SetPwrMgt(fctrl);

	if (pattrib->mdata)
		SetMData(fctrl);

	if (get_tofr_ds(fctrl) == 3) {
		_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, da, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr4, padapter_link->mac_addr, ETH_ALEN);
		a4_shift = ETH_ALEN;
		pattrib->hdrlen += ETH_ALEN;
	} else {
		_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
		_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);
		a4_shift = 0;
	}

	qc = (unsigned short *)(pframe + pattrib->hdrlen - 2);

	SetPriority(qc, tid);

	SetEOSP(qc, pattrib->eosp);

	SetAckpolicy(qc, pattrib->ack_policy);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_QOS_DATA_NULL);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr_qos) + a4_shift;
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr_qos) + a4_shift;

#ifdef CONFIG_80211AX_HE
	if (om_info) {
		set_htc_order_bit(fctrl);
		pattrib->rate = MGN_HE1SS_MCS0;
		rtw_he_fill_htc(padapter, pattrib, (u32 *)pframe);
		pframe = pframe + 4;
		pattrib->pktlen = pattrib->pktlen + 4;
	}
#endif

	pattrib->last_txcmdsz = pattrib->pktlen;

	if (wait_ack)
		ret = dump_mgntframe_and_wait_ack(padapter, pmgntframe);
	else {
		dump_mgntframe(padapter, pmgntframe);
		ret = _SUCCESS;
	}

exit:
	return ret;
}

/*
 * when wait_ms >0 , this function should be called at process context
 * wait_ms == 0 means that there is no need to wait ack through C2H_CCX_TX_RPT
 * wait_ms > 0 means you want to wait ack through C2H_CCX_TX_RPT, and the value of wait_ms means the interval between each TX
 * try_cnt means the maximal TX count to try
 * da == NULL for station mode
 */
int issue_qos_nulldata(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		unsigned char *da, u16 tid, u8 ps, int try_cnt, int wait_ms, u8 need_om)
{
	int ret = _FAIL;
	int i = 0;
	systime start = rtw_get_current_time();
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	/* da == NULL, assum it's null data for sta to ap*/
	if (da == NULL)
		da = get_my_bssid(&(pmlmeinfo->network));

	do {
		ret = _issue_qos_nulldata(padapter, padapter_link,
					da, tid, ps, wait_ms > 0 ? _TRUE : _FALSE, need_om);

		i++;

		if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
			break;

		if (i < try_cnt && wait_ms > 0 && ret == _FAIL)
			rtw_msleep_os(wait_ms);

	} while ((i < try_cnt) && ((ret == _FAIL) || (wait_ms == 0)));

	if (ret != _FAIL) {
		ret = _SUCCESS;
#ifndef DBG_XMIT_ACK
		goto exit;
#endif
	}

	if (try_cnt && wait_ms) {
		if (da)
			RTW_INFO(FUNC_ADPT_FMT" to "MAC_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(padapter), MAC_ARG(da),
				rtw_get_oper_ch(padapter, padapter_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
		else
			RTW_INFO(FUNC_ADPT_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(padapter),
				rtw_get_oper_ch(padapter, padapter_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
	}
exit:
	return ret;
}

static int _issue_deauth(_adapter *padapter, unsigned char *da, unsigned short reason, u8 wait_ack, u8 key_type)
{
	struct xmit_frame			*pmgntframe;
	struct pkt_attrib			*pattrib;
	unsigned char					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short				*fctrl;
	struct xmit_priv			*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK	*padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_info	*pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);
	int ret = _FAIL;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P	 */

	/* RTW_INFO("%s to "MAC_FMT"\n", __func__, MAC_ARG(da)); */

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		goto exit;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);
	pattrib->retry_ctrl = _FALSE;
	pattrib->key_type = key_type;
	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_DEAUTH);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	reason = cpu_to_le16(reason);
	pframe = rtw_set_fixed_ie(pframe, _RSON_CODE_ , (unsigned char *)&reason, &(pattrib->pktlen));

	pattrib->last_txcmdsz = pattrib->pktlen;


	if (wait_ack)
		ret = dump_mgntframe_and_wait_ack(padapter, pmgntframe);
	else {
		dump_mgntframe(padapter, pmgntframe);
		ret = _SUCCESS;
	}

exit:
	return ret;
}

int issue_deauth(_adapter *padapter, unsigned char *da, unsigned short reason)
{
	RTW_INFO("%s reason(%u) to "MAC_FMT"\n", __func__, reason, MAC_ARG(da));
	return _issue_deauth(padapter, da, reason, _FALSE, IEEE80211W_RIGHT_KEY);
}

#ifdef CONFIG_IEEE80211W
int issue_deauth_11w(_adapter *padapter, unsigned char *da, unsigned short reason, u8 key_type)
{
	RTW_INFO("%s to "MAC_FMT"\n", __func__, MAC_ARG(da));
	return _issue_deauth(padapter, da, reason, _FALSE, key_type);
}
#endif /* CONFIG_IEEE80211W */

/*
 * wait_ms == 0 means that there is no need to wait ack through C2H_CCX_TX_RPT
 * wait_ms > 0 means you want to wait ack through C2H_CCX_TX_RPT, and the value of wait_ms means the interval between each TX
 * try_cnt means the maximal TX count to try
 */
int issue_deauth_ex(_adapter *padapter, u8 *da, unsigned short reason, int try_cnt,
		    int wait_ms)
{
	int ret = _FAIL;
	int i = 0;
	systime start = rtw_get_current_time();
	/* ToDo CONFIG_RTW_MLDL [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	do {
		ret = _issue_deauth(padapter, da, reason, wait_ms > 0 ? _TRUE : _FALSE, IEEE80211W_RIGHT_KEY);

		i++;

		if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
			break;

		if (i < try_cnt && wait_ms > 0 && ret == _FAIL)
			rtw_msleep_os(wait_ms);

	} while ((i < try_cnt) && ((ret == _FAIL) || (wait_ms == 0)));

	if (ret != _FAIL) {
		ret = _SUCCESS;
#ifndef DBG_XMIT_ACK
		goto exit;
#endif
	}

	if (try_cnt && wait_ms) {
		if (da)
			RTW_INFO(FUNC_ADPT_FMT" to "MAC_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(padapter), MAC_ARG(da),
				rtw_get_oper_ch(padapter, padapter_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
		else
			RTW_INFO(FUNC_ADPT_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(padapter),
				rtw_get_oper_ch(padapter, padapter_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
	}
exit:
	return ret;
}


static int _issue_disassoc(_adapter *padapter, unsigned char *da, unsigned short reason, u8 wait_ack, u8 key_type)
{
	struct xmit_frame			*pmgntframe;
	struct pkt_attrib			*pattrib;
	unsigned char					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short				*fctrl;
	struct xmit_priv			*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK	*padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_info	*pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);
	int ret = _FAIL;
#ifdef CONFIG_P2P
	struct wifidirect_info *pwdinfo = &(padapter->wdinfo);
#endif /* CONFIG_P2P	 */

	/* RTW_INFO("%s to "MAC_FMT"\n", __func__, MAC_ARG(da)); */

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		goto exit;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);
	pattrib->retry_ctrl = _FALSE;
	pattrib->key_type = key_type;
	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy(pwlanhdr->addr1, da, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_DISASSOC);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	reason = cpu_to_le16(reason);
	pframe = rtw_set_fixed_ie(pframe, _RSON_CODE_ , (unsigned char *)&reason, &(pattrib->pktlen));

	pattrib->last_txcmdsz = pattrib->pktlen;


	if (wait_ack)
		ret = dump_mgntframe_and_wait_ack(padapter, pmgntframe);
	else {
		dump_mgntframe(padapter, pmgntframe);
		ret = _SUCCESS;
	}

exit:
	return ret;
}

int issue_disassoc(_adapter *padapter, unsigned char *da, unsigned short reason)
{
	RTW_INFO("%s reason(%u) to "MAC_FMT"\n", __func__, reason, MAC_ARG(da));
	return _issue_disassoc(padapter, da, reason, _FALSE, IEEE80211W_RIGHT_KEY);
}

void issue_action_spct_ch_switch(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			u8 *ra, u8 new_ch, u8 ch_offset)
{
	struct xmit_frame *pmgntframe;
	struct pkt_attrib *pattrib;
	unsigned char				*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short			*fctrl;
	struct xmit_priv			*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	RTW_INFO(FUNC_NDEV_FMT" ra="MAC_FMT", ch:%u, offset:%u\n",
		FUNC_NDEV_ARG(padapter->pnetdev), MAC_ARG(ra), new_ch, ch_offset);

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy(pwlanhdr->addr1, ra, ETH_ALEN); /* RA */
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN); /* TA */
	_rtw_memcpy(pwlanhdr->addr3, ra, ETH_ALEN); /* DA = RA */

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_ACTION);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	/* category, action */
	{
		u8 category, action;
		category = RTW_WLAN_CATEGORY_SPECTRUM_MGMT;
		action = RTW_WLAN_ACTION_SPCT_CHL_SWITCH;

		pframe = rtw_set_fixed_ie(pframe, 1, &(category), &(pattrib->pktlen));
		pframe = rtw_set_fixed_ie(pframe, 1, &(action), &(pattrib->pktlen));
	}

	pframe = rtw_set_ie_ch_switch(pframe, &(pattrib->pktlen), 0, new_ch, 0);
	pframe = rtw_set_ie_secondary_ch_offset(pframe, &(pattrib->pktlen),
			hal_ch_offset_to_secondary_ch_offset(ch_offset));

	pattrib->last_txcmdsz = pattrib->pktlen;

	dump_mgntframe(padapter, pmgntframe);

}

#ifdef CONFIG_IEEE80211W
void issue_action_SA_Query(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			unsigned char *raddr, unsigned char action, unsigned short tid, u8 key_type)
{
	u8	category = RTW_WLAN_CATEGORY_SA_QUERY;
	u16	reason_code;
	struct xmit_frame		*pmgntframe;
	struct pkt_attrib		*pattrib;
	u8					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	u16					*fctrl;
	struct xmit_priv		*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct sta_info		*psta;
	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct registry_priv		*pregpriv = &padapter->registrypriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	RTW_INFO("%s, %04x\n", __FUNCTION__, tid);

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL) {
		RTW_INFO("%s: alloc_mgtxmitframe fail\n", __FUNCTION__);
		return;
	}

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);
	pattrib->key_type = key_type;
	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	if (raddr)
		_rtw_memcpy(pwlanhdr->addr1, raddr, ETH_ALEN);
	else
		_rtw_memcpy(pwlanhdr->addr1, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_ACTION);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	pframe = rtw_set_fixed_ie(pframe, 1, &category, &pattrib->pktlen);
	pframe = rtw_set_fixed_ie(pframe, 1, &action, &pattrib->pktlen);

	switch (action) {
	case 0: /* SA Query req */
		pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)&pmlmeext->sa_query_seq, &pattrib->pktlen);
		pmlmeext->sa_query_seq++;
		/* send sa query request to AP, AP should reply sa query response in 1 second */
		if (pattrib->key_type == IEEE80211W_RIGHT_KEY) {
			psta = rtw_get_stainfo(pstapriv, pwlanhdr->addr1);
			if (psta != NULL) {
				/* RTW_INFO("%s, %d, set dot11w_expire_timer\n", __func__, __LINE__); */
				if (_check_timer_is_active(&psta->dot11w_expire_timer)){
					/* The timer is active, do not set it again */
					rtw_free_xmitframe(&padapter->xmitpriv, pmgntframe);
					return;
				}
				_set_timer(&psta->dot11w_expire_timer, 1000);
			}
		}
		break;

	case 1: /* SA Query rsp */
		/* RTW_INFO("rtw_set_fixed_ie, %04x\n", tid); */
		pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)&tid, &pattrib->pktlen);
		break;
	default:
		break;
	}

	pattrib->last_txcmdsz = pattrib->pktlen;

	dump_mgntframe(padapter, pmgntframe);
}
#endif /* CONFIG_IEEE80211W */

/**
 * issue_action_ba - internal function to TX Block Ack action frame
 * @padapter: the adapter to TX
 * @raddr: receiver address
 * @action: Block Ack Action
 * @tid: tid
 * @size: the announced AMPDU buffer size. used by ADDBA_RESP
 * @paddba_req: used by ADDBA_RESP
 * @status: status/reason code. used by ADDBA_RESP, DELBA
 * @initiator: if we are the initiator of AMPDU association. used by DELBA
 * @wait_ack: used xmit ack
 *
 * Returns:
 * _SUCCESS: No xmit ack is used or acked
 * _FAIL: not acked when using xmit ack
 */
static int issue_action_ba(_adapter *padapter, unsigned char *raddr,
			   unsigned char action, u8 tid, u8 size,
			   struct ADDBA_request *paddba_req, u16 status,
			   u8 initiator, int wait_ack)
{
	int ret = _FAIL;
	u8	category = RTW_WLAN_CATEGORY_BACK;
	u16	start_seq;
	u16	BA_para_set;
	u16	BA_timeout_value;
	u16	BA_starting_seqctrl;
	struct xmit_frame		*pmgntframe;
	struct pkt_attrib		*pattrib;
	u8					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	u16					*fctrl;
	struct xmit_priv		*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK	*padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_info	*lmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);
	struct sta_info		*psta;
	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct registry_priv		*pregpriv = &padapter->registrypriv;

#ifdef CONFIG_80211N_HT

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		goto exit;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	/* _rtw_memcpy(pwlanhdr->addr1, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN); */
	_rtw_memcpy(pwlanhdr->addr1, raddr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(lmlmeinfo->network)), ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_ACTION);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	pframe = rtw_set_fixed_ie(pframe, 1, &(category), &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 1, &(action), &(pattrib->pktlen));

	if (category == 3) {
		switch (action) {
		case RTW_WLAN_ACTION_ADDBA_REQ:
			do {
				pmlmeinfo->dialogToken++;
			} while (pmlmeinfo->dialogToken == 0);
			pframe = rtw_set_fixed_ie(pframe, 1, &(pmlmeinfo->dialogToken), &(pattrib->pktlen));

			BA_para_set = (0x1002 | ((tid & 0xf) << 2)); /* immediate ack & 64 buffer size */

#ifdef CONFIG_TX_AMSDU
			if (padapter->tx_amsdu >= 1) /* TX AMSDU  enabled */
				BA_para_set |= BIT(0);
			else /* TX AMSDU disabled */
				BA_para_set &= ~BIT(0);
#endif
			psta = rtw_get_stainfo(pstapriv, raddr);
			if (psta != NULL) {
				if (psta->flags & WLAN_STA_AMSDU_DISABLE)
					BA_para_set &= ~BIT(0);
			}
			BA_para_set = cpu_to_le16(BA_para_set);
			pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)(&(BA_para_set)), &(pattrib->pktlen));

			/* BA_timeout_value = 0xffff; */ /* max: 65535 TUs(~ 65 ms) */
			BA_timeout_value = 5000;/* ~ 5ms */
			BA_timeout_value = cpu_to_le16(BA_timeout_value);
			pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)(&(BA_timeout_value)), &(pattrib->pktlen));

			/* if ((psta = rtw_get_stainfo(pstapriv, pmlmeinfo->network.MacAddress)) != NULL) */
			psta = rtw_get_stainfo(pstapriv, raddr);
			if (psta != NULL) {
				start_seq = (psta->sta_xmitpriv.txseq_tid[tid & 0x07] & 0xfff) + 1;

				RTW_INFO("BA_starting_seqctrl = %d for TID=%d\n", start_seq, tid & 0x07);

				psta->BA_starting_seqctrl[tid & 0x07] = start_seq;

				BA_starting_seqctrl = start_seq << 4;
				BA_starting_seqctrl = cpu_to_le16(BA_starting_seqctrl);
				pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)(&(BA_starting_seqctrl)), &(pattrib->pktlen));
			}
			break;

		case RTW_WLAN_ACTION_ADDBA_RESP:
			pframe = rtw_set_fixed_ie(pframe, 1, &(paddba_req->dialog_token), &(pattrib->pktlen));
			status = cpu_to_le16(status);
			pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)(&status), &(pattrib->pktlen));

			BA_para_set = le16_to_cpu(paddba_req->BA_para_set);

			BA_para_set &= ~IEEE80211_ADDBA_PARAM_TID_MASK;
			BA_para_set |= (tid << 2) & IEEE80211_ADDBA_PARAM_TID_MASK;

			BA_para_set &= ~RTW_IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK;
			BA_para_set |= (size << 6) & RTW_IEEE80211_ADDBA_PARAM_BUF_SIZE_MASK;

			if (!padapter->registrypriv.wifi_spec) {
				if (pregpriv->rx_ampdu_amsdu == 0) /* disabled */
					BA_para_set &= ~BIT(0);
				else if (pregpriv->rx_ampdu_amsdu == 1) /* enabled */
					BA_para_set |= BIT(0);
			}

			psta = rtw_get_stainfo(pstapriv, raddr);
			if (psta != NULL) {
				if (psta->flags & WLAN_STA_AMSDU_DISABLE)
					BA_para_set &= ~BIT(0);
			}

			BA_para_set = cpu_to_le16(BA_para_set);

			pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)(&(BA_para_set)), &(pattrib->pktlen));
			pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)(&(paddba_req->BA_timeout_value)), &(pattrib->pktlen));
			break;

		case RTW_WLAN_ACTION_DELBA:
			BA_para_set = 0;
			BA_para_set |= (tid << 12) & IEEE80211_DELBA_PARAM_TID_MASK;
			BA_para_set |= (initiator << 11) & IEEE80211_DELBA_PARAM_INITIATOR_MASK;

			BA_para_set = cpu_to_le16(BA_para_set);
			pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)(&(BA_para_set)), &(pattrib->pktlen));
			status = cpu_to_le16(status);
			pframe = rtw_set_fixed_ie(pframe, 2, (unsigned char *)(&(status)), &(pattrib->pktlen));
			break;
		default:
			break;
		}
	}

	pattrib->last_txcmdsz = pattrib->pktlen;

	if (wait_ack)
		ret = dump_mgntframe_and_wait_ack(padapter, pmgntframe);
	else {
		dump_mgntframe(padapter, pmgntframe);
		ret = _SUCCESS;
	}

exit:
#endif /* CONFIG_80211N_HT */
	return ret;
}

/**
 * issue_addba_req - TX ADDBA_REQ
 * @adapter: the adapter to TX
 * @ra: receiver address
 * @tid: tid
 */
inline void issue_addba_req(_adapter *adapter, unsigned char *ra, u8 tid)
{
	issue_action_ba(adapter, ra, RTW_WLAN_ACTION_ADDBA_REQ
			, tid
			, 0 /* unused */
			, NULL /* unused */
			, 0 /* unused */
			, 0 /* unused */
			, _FALSE
		       );
	RTW_INFO(FUNC_ADPT_FMT" ra="MAC_FMT" tid=%u\n"
		 , FUNC_ADPT_ARG(adapter), MAC_ARG(ra), tid);

}

/**
 * issue_addba_rsp - TX ADDBA_RESP
 * @adapter: the adapter to TX
 * @ra: receiver address
 * @tid: tid
 * @status: status code
 * @size: the announced AMPDU buffer size
 * @paddba_req: used by ADDBA_RESP
 */
inline void issue_addba_rsp(_adapter *adapter, unsigned char *ra, u8 tid,
			    u16 status, u8 size,
			    struct ADDBA_request *paddba_req)
{
	issue_action_ba(adapter, ra, RTW_WLAN_ACTION_ADDBA_RESP
			, tid
			, size
			, paddba_req
			, status
			, 0 /* unused */
			, _FALSE
		       );
	RTW_INFO(FUNC_ADPT_FMT" ra="MAC_FMT" status=%u, tid=%u, size=%u\n"
		 , FUNC_ADPT_ARG(adapter), MAC_ARG(ra), status, tid, size);
}

/**
 * issue_addba_rsp_wait_ack - TX ADDBA_RESP and wait ack
 * @adapter: the adapter to TX
 * @ra: receiver address
 * @tid: tid
 * @status: status code
 * @size: the announced AMPDU buffer size
 * @paddba_req: used by ADDBA_RESP
 * @try_cnt: the maximal TX count to try
 * @wait_ms: == 0 means that there is no need to wait ack through C2H_CCX_TX_RPT
 *           > 0 means you want to wait ack through C2H_CCX_TX_RPT, and the value of wait_ms means the interval between each TX
 */
inline u8 issue_addba_rsp_wait_ack(_adapter *adapter, unsigned char *ra, u8 tid,
				   u16 status, u8 size,
				   struct ADDBA_request *paddba_req, int try_cnt,
				   int wait_ms)
{
	int ret = _FAIL;
	int i = 0;
	systime start = rtw_get_current_time();

	if (adapter_is_tx_blocked_by_ch_waiting(adapter))
		goto exit;

	do {
		ret = issue_action_ba(adapter, ra, RTW_WLAN_ACTION_ADDBA_RESP
				      , tid
				      , size
				      , paddba_req
				      , status
				      , 0 /* unused */
				      , _TRUE
				     );

		i++;

		if (RTW_CANNOT_RUN(adapter_to_dvobj(adapter)))
			break;

		if (i < try_cnt && wait_ms > 0 && ret == _FAIL)
			rtw_msleep_os(wait_ms);

	} while ((i < try_cnt) && ((ret == _FAIL) || (wait_ms == 0)));

	if (ret != _FAIL) {
		ret = _SUCCESS;
#ifndef DBG_XMIT_ACK
		/* goto exit; */
#endif
	}

	if (try_cnt && wait_ms) {
		RTW_INFO(FUNC_ADPT_FMT" ra="MAC_FMT" status:=%u tid=%u size:%u%s, %d/%d in %u ms\n"
			, FUNC_ADPT_ARG(adapter), MAC_ARG(ra), status, tid, size
			, ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
	}

exit:
	return ret;
}

/**
 * issue_del_ba - TX DELBA
 * @adapter: the adapter to TX
 * @ra: receiver address
 * @tid: tid
 * @reason: reason code
 * @initiator: if we are the initiator of AMPDU association. used by DELBA
 */
inline void issue_del_ba(_adapter *adapter, unsigned char *ra, u8 tid, u16 reason, u8 initiator)
{
	issue_action_ba(adapter, ra, RTW_WLAN_ACTION_DELBA
			, tid
			, 0 /* unused */
			, NULL /* unused */
			, reason
			, initiator
			, _FALSE
		       );
	RTW_INFO(FUNC_ADPT_FMT" ra="MAC_FMT" reason=%u, tid=%u, initiator=%u\n"
		 , FUNC_ADPT_ARG(adapter), MAC_ARG(ra), reason, tid, initiator);
}

/**
 * issue_del_ba_ex - TX DELBA with xmit ack options
 * @adapter: the adapter to TX
 * @ra: receiver address
 * @tid: tid
 * @reason: reason code
 * @initiator: if we are the initiator of AMPDU association. used by DELBA
 * @try_cnt: the maximal TX count to try
 * @wait_ms: == 0 means that there is no need to wait ack through C2H_CCX_TX_RPT
 *           > 0 means you want to wait ack through C2H_CCX_TX_RPT, and the value of wait_ms means the interval between each TX
 */
int issue_del_ba_ex(_adapter *adapter, unsigned char *ra, u8 tid, u16 reason, u8 initiator
		    , int try_cnt, int wait_ms)
{
	int ret = _FAIL;
	int i = 0;
	systime start = rtw_get_current_time();

	if (adapter_is_tx_blocked_by_ch_waiting(adapter))
		goto exit;

	do {
		ret = issue_action_ba(adapter, ra, RTW_WLAN_ACTION_DELBA
				      , tid
				      , 0 /* unused */
				      , NULL /* unused */
				      , reason
				      , initiator
				      , wait_ms > 0 ? _TRUE : _FALSE
				     );

		i++;

		if (RTW_CANNOT_RUN(adapter_to_dvobj(adapter)))
			break;

		if (i < try_cnt && wait_ms > 0 && ret == _FAIL)
			rtw_msleep_os(wait_ms);

	} while ((i < try_cnt) && ((ret == _FAIL) || (wait_ms == 0)));

	if (ret != _FAIL) {
		ret = _SUCCESS;
#ifndef DBG_XMIT_ACK
		/* goto exit; */
#endif
	}

	if (try_cnt && wait_ms) {
		RTW_INFO(FUNC_ADPT_FMT" ra="MAC_FMT" reason=%u, tid=%u, initiator=%u%s, %d/%d in %u ms\n"
			, FUNC_ADPT_ARG(adapter), MAC_ARG(ra), reason, tid, initiator
			, ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
	}
exit:
	return ret;
}

void issue_action_BSSCoexistPacket(_adapter *padapter)
{
	_list		*plist, *phead;
	unsigned char category, action;
	struct xmit_frame			*pmgntframe;
	struct pkt_attrib			*pattrib;
	unsigned char				*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short			*fctrl;
	struct	wlan_network	*pnetwork = NULL;
	struct xmit_priv			*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK	*padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &padapter_link->mlmepriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);
	_queue		*queue	= &(padapter->mlmepriv.scanned_queue);
	u8 InfoContent[16] = {0};
	u8 ICS[8][15];
#ifdef CONFIG_80211N_HT
	if ((pmlmepriv->num_FortyMHzIntolerant == 0) && (pmlmepriv->num_sta_no_ht == 0))
		return;

	if (_TRUE == pmlmeinfo->bwmode_updated)
		return;

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return;

	RTW_INFO("%s\n", __FUNCTION__);


	category = RTW_WLAN_CATEGORY_PUBLIC;
	action = ACT_PUBLIC_BSSCOEXIST;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy(pwlanhdr->addr1, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_ACTION);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	pframe = rtw_set_fixed_ie(pframe, 1, &(category), &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 1, &(action), &(pattrib->pktlen));

	/* TODO calculate 40Mhz intolerant via ch and ch offset */
	/* if (pmlmepriv->num_FortyMHzIntolerant > 0) */
	{
		u8 iedata = 0;

		iedata |= BIT(2);/* 20 MHz BSS Width Request */
		pframe = rtw_set_ie(pframe, EID_BSSCoexistence,  1, &iedata, &(pattrib->pktlen));
	}

	/*  */
	_rtw_memset(ICS, 0, sizeof(ICS));
	if (pmlmepriv->num_sta_no_ht > 0) {
		int i;

		_rtw_spinlock_bh(&(padapter->mlmepriv.scanned_queue.lock));

		phead = get_list_head(queue);
		plist = get_next(phead);

		while (1) {
			int len;
			u8 *p;
			WLAN_BSSID_EX *pbss_network;

			if (rtw_end_of_queue_search(phead, plist) == _TRUE)
				break;

			pnetwork = LIST_CONTAINOR(plist, struct wlan_network, list);

			plist = get_next(plist);

			pbss_network = (WLAN_BSSID_EX *)&pnetwork->network;

			p = rtw_get_ie(pbss_network->IEs + _FIXED_IE_LENGTH_, _HT_CAPABILITY_IE_, &len, pbss_network->IELength - _FIXED_IE_LENGTH_);
			if ((p == NULL) || (len == 0)) { /* non-HT */
				if ((pbss_network->Configuration.DSConfig <= 0) || (pbss_network->Configuration.DSConfig > 14))
					continue;

				ICS[0][pbss_network->Configuration.DSConfig] = 1;

				if (ICS[0][0] == 0)
					ICS[0][0] = 1;
			}

		}

		_rtw_spinunlock_bh(&(padapter->mlmepriv.scanned_queue.lock));


		for (i = 0; i < 8; i++) {
			if (ICS[i][0] == 1) {
				int j, k = 0;

				InfoContent[k] = i;
				/* SET_BSS_INTOLERANT_ELE_REG_CLASS(InfoContent,i); */
				k++;

				for (j = 1; j <= 14; j++) {
					if (ICS[i][j] == 1) {
						if (k < 16) {
							InfoContent[k] = j; /* channel number */
							/* SET_BSS_INTOLERANT_ELE_CHANNEL(InfoContent+k, j); */
							k++;
						}
					}
				}

				pframe = rtw_set_ie(pframe, EID_BSSIntolerantChlReport, k, InfoContent, &(pattrib->pktlen));

			}

		}


	}


	pattrib->last_txcmdsz = pattrib->pktlen;

	dump_mgntframe(padapter, pmgntframe);
#endif /* CONFIG_80211N_HT */
}

/* Spatial Multiplexing Powersave (SMPS) action frame */
int _issue_action_SM_PS(_adapter *padapter, unsigned char *raddr, u8 NewMimoPsMode, u8 wait_ack)
{

	int ret = _FAIL;
	unsigned char category = RTW_WLAN_CATEGORY_HT;
	u8 action = RTW_WLAN_ACTION_HT_SM_PS;
	u8 sm_power_control = 0;
	struct xmit_frame *pmgntframe;
	struct pkt_attrib *pattrib;
	unsigned char *pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	unsigned short				*fctrl;
	struct xmit_priv			*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] TBD may pass stainfo here to get adapter_link */
	struct _ADAPTER_LINK	*padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_info	*pmlmeinfo = &(padapter_link->mlmeextpriv.mlmext_info);


	if (NewMimoPsMode == SM_PS_DISABLE) {
		sm_power_control = sm_power_control  & ~(BIT(0)); /* SM Power Save Enable = 0 SM Power Save Disable */
	} else if (NewMimoPsMode == SM_PS_STATIC) {
		sm_power_control = sm_power_control | BIT(0);    /* SM Power Save Enable = 1 SM Power Save Enable  */
		sm_power_control = sm_power_control & ~(BIT(1)); /* SM Mode = 0 Static Mode */
	} else if (NewMimoPsMode == SM_PS_DYNAMIC) {
		sm_power_control = sm_power_control | BIT(0); /* SM Power Save Enable = 1 SM Power Save Enable  */
		sm_power_control = sm_power_control | BIT(1); /* SM Mode = 1 Dynamic Mode */
	} else
		return ret;

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		return ret;

	RTW_INFO("%s, sm_power_control=%u, NewMimoPsMode=%u\n", __FUNCTION__ , sm_power_control , NewMimoPsMode);

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		return ret;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy(pwlanhdr->addr1, raddr, ETH_ALEN); /* RA */
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN); /* TA */
	_rtw_memcpy(pwlanhdr->addr3, get_my_bssid(&(pmlmeinfo->network)), ETH_ALEN); /* DA = RA */

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_ACTION);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	/* category, action */
	pframe = rtw_set_fixed_ie(pframe, 1, &(category), &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 1, &(action), &(pattrib->pktlen));

	pframe = rtw_set_fixed_ie(pframe, 1, &(sm_power_control), &(pattrib->pktlen));

	pattrib->last_txcmdsz = pattrib->pktlen;

	if (wait_ack)
		ret = dump_mgntframe_and_wait_ack(padapter, pmgntframe);
	else {
		dump_mgntframe(padapter, pmgntframe);
		ret = _SUCCESS;
	}

	if (ret != _SUCCESS)
		RTW_INFO("%s, ack to\n", __func__);

	return ret;
}

/*
 * wait_ms == 0 means that there is no need to wait ack through C2H_CCX_TX_RPT
 * wait_ms > 0 means you want to wait ack through C2H_CCX_TX_RPT, and the value of wait_ms means the interval between each TX
 * try_cnt means the maximal TX count to try
 */
int issue_action_SM_PS_wait_ack(_adapter *padapter, unsigned char *raddr, u8 NewMimoPsMode, int try_cnt, int wait_ms)
{
	int ret = _FAIL;
	int i = 0;
	systime start = rtw_get_current_time();

	if (adapter_is_tx_blocked_by_ch_waiting(padapter))
		goto exit;

	do {
		ret = _issue_action_SM_PS(padapter, raddr, NewMimoPsMode , wait_ms > 0 ? _TRUE : _FALSE);

		i++;

		if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
			break;

		if (i < try_cnt && wait_ms > 0 && ret == _FAIL)
			rtw_msleep_os(wait_ms);

	} while ((i < try_cnt) && ((ret == _FAIL) || (wait_ms == 0)));

	if (ret != _FAIL) {
		ret = _SUCCESS;
#ifndef DBG_XMIT_ACK
		goto exit;
#endif
	}

	if (try_cnt && wait_ms) {
		if (raddr)
			RTW_INFO(FUNC_ADPT_FMT" to "MAC_FMT", %s , %d/%d in %u ms\n",
				 FUNC_ADPT_ARG(padapter), MAC_ARG(raddr),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
		else
			RTW_INFO(FUNC_ADPT_FMT", %s , %d/%d in %u ms\n",
				 FUNC_ADPT_ARG(padapter),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
	}
exit:

	return ret;
}

int issue_action_SM_PS(_adapter *padapter ,  unsigned char *raddr , u8 NewMimoPsMode)
{
	RTW_INFO("%s to "MAC_FMT"\n", __func__, MAC_ARG(raddr));
	return _issue_action_SM_PS(padapter, raddr, NewMimoPsMode , _FALSE);
}

#ifdef PRIVATE_R

int issue_action_find_remote(_adapter *padapter)
{
	int ret = _FAIL;
	u8		category = RTW_WLAN_CATEGORY_P2P;
	u16	start_seq;
	struct xmit_frame		*pmgntframe;
	struct pkt_attrib		*pattrib;
	u8					*pframe;
	struct rtw_ieee80211_hdr	*pwlanhdr;
	u16					*fctrl;
	struct xmit_priv		*pxmitpriv = &(padapter->xmitpriv);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	u8		bc_addr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8		ROKU_OUI[] = {0x00, 0x0d, 0x4b};
	u8		vendor_spec_subtype = 0x7f;
	u8		p2p_action_subtype = 0x01;
	u8		p2p_action_dialog = 0x03;
	u8		reserved_1 = 0xaa;
	u8		reserved_2[] = {0xab, 0x01, 0x01};
	u8		len = 0;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (alink_is_tx_blocked_by_ch_waiting(padapter_link))
		goto exit;

	pmgntframe = alloc_mgtxmitframe(pxmitpriv);
	if (pmgntframe == NULL)
		goto exit;

	/* update attribute */
	pattrib = &pmgntframe->attrib;
	update_mgntframe_attrib(padapter, padapter_link, pattrib);

	_rtw_memset(pmgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	pframe = (u8 *)(pmgntframe->buf_addr) + TXDESC_OFFSET;
	pwlanhdr = (struct rtw_ieee80211_hdr *)pframe;

	fctrl = &(pwlanhdr->frame_ctl);
	*(fctrl) = 0;

	_rtw_memcpy(pwlanhdr->addr1, bc_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr2, padapter_link->mac_addr, ETH_ALEN);
	_rtw_memcpy(pwlanhdr->addr3, padapter_link->mac_addr, ETH_ALEN);

	SetSeqNum(pwlanhdr, pmlmeext->mgnt_seq);
	pmlmeext->mgnt_seq++;
	set_frame_sub_type(pframe, WIFI_ACTION);

	pframe += sizeof(struct rtw_ieee80211_hdr_3addr);
	pattrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	pframe = rtw_set_fixed_ie(pframe, 1, &category, &(pattrib->pktlen));

	pframe = rtw_set_fixed_ie(pframe, 3, ROKU_OUI, &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 1, &vendor_spec_subtype, &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 1, &p2p_action_subtype, &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, 1, &p2p_action_dialog, &(pattrib->pktlen));

	/* set remote len and MAC address */
	pframe = rtw_set_fixed_ie(pframe, 1, &reserved_1, &(pattrib->pktlen));
	len = pwdinfo->num_of_remote * ETH_ALEN;
	pframe = rtw_set_fixed_ie(pframe, 1, &len, &(pattrib->pktlen));
	pframe = rtw_set_fixed_ie(pframe, len, pwdinfo->remote_mac_address, &(pattrib->pktlen));

	pframe = rtw_set_fixed_ie(pframe, 3, reserved_2, &(pattrib->pktlen));

	pattrib->last_txcmdsz = pattrib->pktlen;
	
	dump_mgntframe(padapter, pmgntframe);
	
	ret = _SUCCESS;

exit:
	return ret;
}
#endif

/**
 * _send_delba_sta_tid - Cancel the AMPDU association for the specific @sta, @tid
 * @adapter: the adapter to which @sta belongs
 * @initiator: if we are the initiator of AMPDU association
 * @sta: the sta to be checked
 * @tid: the tid to be checked
 * @force: cancel and send DELBA even when no AMPDU association is setup
 * @wait_ack: send delba with xmit ack (valid when initiator == 0)
 *
 * Returns:
 * _FAIL if sta is NULL
 * when initiator is 1, always _SUCCESS
 * when initiator is 0, _SUCCESS if DELBA is acked
 */
static unsigned int _send_delba_sta_tid(_adapter *adapter, u8 initiator, struct sta_info *sta, u8 tid
					, u8 force, int wait_ack)
{
	int ret = _SUCCESS;

	if (sta == NULL) {
		ret = _FAIL;
		goto exit;
	}

	if (initiator == 0) {
		/* recipient */
		if (force || sta->recvreorder_ctrl[tid].enable == _TRUE) {
			u8 ampdu_size_bak = sta->recvreorder_ctrl[tid].ampdu_size;

			sta->recvreorder_ctrl[tid].enable = _FALSE;
			sta->recvreorder_ctrl[tid].ampdu_size = RX_AMPDU_SIZE_INVALID;

			if (rtw_del_rx_ampdu_test_trigger_no_tx_fail())
				ret = _FAIL;
			else if (wait_ack)
				ret = issue_del_ba_ex(adapter, sta->phl_sta->mac_addr, tid, 37, initiator, 3, 1);
			else
				issue_del_ba(adapter, sta->phl_sta->mac_addr, tid, 37, initiator);

			if (ret == _FAIL && sta->recvreorder_ctrl[tid].enable == _FALSE)
				sta->recvreorder_ctrl[tid].ampdu_size = ampdu_size_bak;
		}
	} else if (initiator == 1) {
		/* originator */
#ifdef CONFIG_80211N_HT
		if (force || sta->ampdu_priv.agg_enable_bitmap & BIT(tid)) {
			sta->ampdu_priv.agg_enable_bitmap &= ~BIT(tid);
			sta->ampdu_priv.candidate_tid_bitmap &= ~BIT(tid);
			issue_del_ba(adapter, sta->phl_sta->mac_addr, tid, 37, initiator);
		}
#endif
	}

exit:
	return ret;
}

inline unsigned int send_delba_sta_tid(_adapter *adapter, u8 initiator, struct sta_info *sta, u8 tid
				       , u8 force)
{
	return _send_delba_sta_tid(adapter, initiator, sta, tid, force, 0);
}

inline unsigned int send_delba_sta_tid_wait_ack(_adapter *adapter, u8 initiator, struct sta_info *sta, u8 tid
		, u8 force)
{
	return _send_delba_sta_tid(adapter, initiator, sta, tid, force, 1);
}

unsigned int send_delba(_adapter *padapter, u8 initiator, u8 *addr)
{
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta = NULL;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u16 tid;

	if ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)
		if (!(pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS))
			return _SUCCESS;

	psta = rtw_get_stainfo(pstapriv, addr);
	if (psta == NULL)
		return _SUCCESS;

#if 0
	RTW_INFO("%s:%s\n", __func__, (initiator == 0) ? "RX_DIR" : "TX_DIR");
	if (initiator == 1) /* originator */
		RTW_INFO("tx agg_enable_bitmap(0x%08x)\n", psta->ampdu_priv.agg_enable_bitmap);
#endif

	for (tid = 0; tid < TID_NUM; tid++)
		send_delba_sta_tid(padapter, initiator, psta, tid, 0);

	return _SUCCESS;
}

unsigned int send_beacon(_adapter *padapter)
{
#ifdef RTW_PHL_BCN

	/* bypass TX BCN queue because op ch is switching */
	if (check_fwstate(&padapter->mlmepriv, WIFI_OP_CH_SWITCHING))
		return _SUCCESS;

	issue_beacon(padapter, 0);

	/* maybe need some mechanism to check bcn ready here */

	return _SUCCESS;
#else
#if defined(CONFIG_PCI_HCI) && !defined(CONFIG_PCI_BCN_POLLING)

	/* bypass TX BCN queue because op ch is switching */
	if (check_fwstate(&padapter->mlmepriv, WIFI_OP_CH_SWITCHING))
		return _SUCCESS;

	/* RTW_INFO("%s\n", __FUNCTION__); */

	rtw_hal_set_hwreg(padapter, HW_VAR_BCN_VALID, NULL);
	rtw_hal_set_hwreg(padapter, HW_VAR_DL_BCN_SEL, NULL);


	issue_beacon(padapter, 0);

	return _SUCCESS;
#endif

/* CONFIG_PCI_BCN_POLLING is for pci interface beacon polling mode */
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)|| defined(CONFIG_PCI_BCN_POLLING) 
	u8 bxmitok = _FALSE;
	int issue = 0;
	int poll = 0;
	systime start = rtw_get_current_time();

	/* bypass TX BCN queue because op ch is switching */
	if (check_fwstate(&padapter->mlmepriv, WIFI_OP_CH_SWITCHING))
		return _SUCCESS;

	rtw_hal_set_hwreg(padapter, HW_VAR_BCN_VALID, NULL);
	rtw_hal_set_hwreg(padapter, HW_VAR_DL_BCN_SEL, NULL);
	do {
		#if defined(CONFIG_PCI_BCN_POLLING) 
		issue_beacon(padapter, 0);
		#else
		issue_beacon(padapter, 100);
		#endif
		issue++;
		do {
			#if defined(CONFIG_PCI_BCN_POLLING) 
			rtw_msleep_os(1);
			#else
			rtw_yield_os();
			#endif
			rtw_hal_get_hwreg(padapter, HW_VAR_BCN_VALID, (u8 *)(&bxmitok));
			poll++;
		} while ((poll % 10) != 0 && _FALSE == bxmitok && !RTW_CANNOT_RUN(adapter_to_dvobj(padapter)));
		#if defined(CONFIG_PCI_BCN_POLLING) 
		rtw_hal_unmap_beacon_icf(padapter);
		#endif
	} while (bxmitok == _FALSE && (issue < 100) && !RTW_CANNOT_RUN(adapter_to_dvobj(padapter)));

	if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
		return _FAIL;


	if (_FALSE == bxmitok) {
		RTW_INFO("%s fail! %u ms\n", __FUNCTION__, rtw_get_passing_time_ms(start));
		return _FAIL;
	} else {
		u32 passing_time = rtw_get_passing_time_ms(start);

		if (passing_time > 100 || issue > 3)
			RTW_INFO("%s success, issue:%d, poll:%d, %u ms\n", __FUNCTION__, issue, poll, rtw_get_passing_time_ms(start));
		else if (0)
			RTW_INFO("%s success, issue:%d, poll:%d, %u ms\n", __FUNCTION__, issue, poll, rtw_get_passing_time_ms(start));

		return _SUCCESS;
	}

#endif /*defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)*/
#endif /* RTW_PHL_BCN */
}

/****************************************************************************

Following are some utitity fuctions for WiFi MLME

*****************************************************************************/

BOOLEAN IsLegal5GChannel(
	_adapter *adapter,
	u8			channel)
{

	int i = 0;
	u8 Channel_5G[45] = {36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58,
		60, 62, 64, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122,
		124, 126, 128, 130, 132, 134, 136, 138, 140, 149, 151, 153, 155, 157, 159,
			     161, 163, 165
			    };
	for (i = 0; i < sizeof(Channel_5G); i++)
		if (channel == Channel_5G[i])
			return _TRUE;
	return _FALSE;
}

/* collect bss info from Beacon and Probe request/response frames. */
u8 collect_bss_info(_adapter *padapter, union recv_frame *precv_frame, WLAN_BSSID_EX *bssid)
{
	int	i;
	sint len;
	u8	*p;
	u8	rf_path;
	u16	val16, subtype;
	u8	*pframe = precv_frame->u.hdr.rx_data;
	u32	packet_len = precv_frame->u.hdr.len;
	u8 ie_offset;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct registry_priv	*pregistrypriv = &padapter->registrypriv;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
#ifdef CONFIG_80211BE_EHT
	u8 eid_eht_ml = EID_EXT_MULTI_LINK;
#endif

	len = packet_len - sizeof(struct rtw_ieee80211_hdr_3addr);

	if (len > MAX_IE_SZ) {
		/*RTW_INFO("IE too long for survey event\n");*/
		return _FAIL;
	}

	_rtw_memset(bssid, 0, sizeof(WLAN_BSSID_EX));

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

	bssid->Length = sizeof(WLAN_BSSID_EX) - MAX_IE_SZ + len;

	/* below is to copy the information element */
	bssid->IELength = len;
	_rtw_memcpy(bssid->IEs, (pframe + sizeof(struct rtw_ieee80211_hdr_3addr)), bssid->IELength);

	/*RTW_WKARD_CORE_RSSI_V1 - GEORGIA MUST REFINE*/
	bssid->PhyInfo.isValid = precv_frame->u.hdr.attrib.phy_info.is_valid;
	/* bssid->PhyInfo.rssi = precv_frame->u.hdr.attrib.SignalStrength; */ /* 0-100 index. */
	bssid->PhyInfo.SignalQuality = precv_frame->u.hdr.attrib.phy_info.signal_quality;/* in percentage */
	bssid->PhyInfo.SignalStrength = precv_frame->u.hdr.attrib.phy_info.signal_strength;/* in percentage */
	bssid->PhyInfo.rssi =  precv_frame->u.hdr.attrib.phy_info.recv_signal_power;/*dbm*/

	if (precv_frame->u.hdr.attrib.data_rate >= DESC_RATE11M) {
		bssid->PhyInfo.is_cck_rate = 0;
		for (rf_path = 0; rf_path < GET_HAL_RFPATH_NUM(dvobj); rf_path++)
			bssid->PhyInfo.rx_snr[rf_path] =
				precv_frame->u.hdr.attrib.phy_info.rx_snr[rf_path];
	} else
		bssid->PhyInfo.is_cck_rate = 1;


	/* checking SSID */
	p = rtw_get_ie(bssid->IEs + ie_offset, _SSID_IE_, &len, bssid->IELength - ie_offset);
	if (p == NULL) {
		RTW_INFO("marc: cannot find SSID for survey event\n");
		return _FAIL;
	}

	if (*(p + 1)) {
		if (len > NDIS_802_11_LENGTH_SSID) {
			RTW_INFO("%s()-%d: IE too long (%d) for survey event\n", __FUNCTION__, __LINE__, len);
			return _FAIL;
		}
		_rtw_memcpy(bssid->Ssid.Ssid, (p + 2), *(p + 1));
		bssid->Ssid.SsidLength = *(p + 1);
	} else
		bssid->Ssid.SsidLength = 0;

	_rtw_memset(bssid->SupportedRates, 0, NDIS_802_11_LENGTH_RATES_EX);

	/* checking rate info... */
	i = 0;
	p = rtw_get_ie(bssid->IEs + ie_offset, _SUPPORTEDRATES_IE_, &len, bssid->IELength - ie_offset);
	if (p != NULL) {
		if (len > NDIS_802_11_LENGTH_RATES_EX) {
			RTW_INFO("%s()-%d: IE too long (%d) for survey event\n", __FUNCTION__, __LINE__, len);
			return _FAIL;
		}
#ifndef RTW_PHL_TEST_FPGA
		if (rtw_validate_value(_SUPPORTEDRATES_IE_, p+2, len) == _FALSE) {
			rtw_absorb_ssid_ifneed(padapter, bssid, pframe);
			RTW_DBG_DUMP("Invalidated Support Rate IE --", p, len+2);
			return _FAIL;
		}
#endif
		_rtw_memcpy(bssid->SupportedRates, (p + 2), len);
		i = len;
	}

	p = rtw_get_ie(bssid->IEs + ie_offset, _EXT_SUPPORTEDRATES_IE_, &len, bssid->IELength - ie_offset);
	if (p != NULL) {
		if (len > (NDIS_802_11_LENGTH_RATES_EX - i)) {
			RTW_INFO("%s()-%d: IE too long (%d) for survey event\n", __FUNCTION__, __LINE__, len);
			return _FAIL;
		}
#ifndef RTW_PHL_TEST_FPGA
		if ((padapter->registrypriv.wifi_spec == 0) &&
			(rtw_validate_value(_EXT_SUPPORTEDRATES_IE_, p+2, len) == _FALSE)) {
			rtw_absorb_ssid_ifneed(padapter, bssid, pframe);
			RTW_DBG_DUMP("Invalidated EXT Support Rate IE --", p, len+2);
			return _FAIL;
		}
#endif
		_rtw_memcpy(bssid->SupportedRates + i, (p + 2), len);
	}

#ifdef CONFIG_P2P
	if (subtype == WIFI_PROBEREQ) {
		u8 *p2p_ie;
		u32	p2p_ielen;
		/* Set Listion Channel */
		p2p_ie = rtw_get_p2p_ie(bssid->IEs, bssid->IELength, NULL, &p2p_ielen);
		if (p2p_ie) {
			u32	attr_contentlen = 0;
			u8 listen_ch[5] = { 0x00 };

			attr_contentlen = sizeof(listen_ch);
			rtw_get_p2p_attr_content(p2p_ie, p2p_ielen, P2P_ATTR_LISTEN_CH, listen_ch, &attr_contentlen);
			bssid->Configuration.DSConfig = listen_ch[4];
		} else {
			/* use current channel */
			bssid->Configuration.DSConfig = padapter_link->mlmeextpriv.chandef.chan;
			RTW_INFO("%s()-%d: Cannot get p2p_ie. set DSconfig to op_ch(%d)\n", __FUNCTION__, __LINE__, bssid->Configuration.DSConfig);
		}

		/* FIXME */
		bssid->InfrastructureMode = Ndis802_11Infrastructure;
		_rtw_memcpy(bssid->MacAddress, get_addr2_ptr(pframe), ETH_ALEN);
		bssid->Privacy = 1;
		RTW_INFO("%s: returned on line %d ?\n", __func__, __LINE__);
		return _SUCCESS;
	}
#endif /* CONFIG_P2P */

	if (bssid->IELength < 12)
		return _FAIL;

	/* Checking for DSConfig */
	p = rtw_get_ie(bssid->IEs + ie_offset, _DSSET_IE_, &len, bssid->IELength - ie_offset);

	bssid->Configuration.Band = 0;
	bssid->Configuration.DSConfig = 0;
	bssid->Configuration.Length = 0;

	if (p) {
		bssid->Configuration.Band = (*(p + 2) > 14) ? BAND_ON_5G : BAND_ON_24G;
		bssid->Configuration.DSConfig = *(p + 2);
	} else {
		/* In 5G, some ap do not have DSSET IE */
		/* checking HT info for channel */
		p = rtw_get_ie(bssid->IEs + ie_offset, _HT_ADD_INFO_IE_, &len, bssid->IELength - ie_offset);
		if (p) {
			struct HT_info_element *HT_info = (struct HT_info_element *)(p + 2);
			bssid->Configuration.Band = (HT_info->primary_channel > 14) ? BAND_ON_5G : BAND_ON_24G;
			bssid->Configuration.DSConfig = HT_info->primary_channel;
		} else {
#if CONFIG_IEEE80211_BAND_6GHZ
			u8 he_op_eid_ext = WLAN_EID_EXTENSION_HE_OPERATION;
			u32	ielen;
			u8 *he_op_ie;

			he_op_ie = rtw_get_ie_ex(bssid->IEs + ie_offset, bssid->IELength - ie_offset, WLAN_EID_EXTENSION, &he_op_eid_ext, 1, NULL, &ielen);
			if(he_op_ie && GET_HE_OP_PARA_6GHZ_OP_INFO_PRESENT(he_op_ie + 3)) {
				u32 offset = HE_OPER_PARAMS_LEN + HE_OPER_BSS_COLOR_INFO_LEN + HE_OPER_BASIC_MCS_LEN;

				if(GET_HE_OP_PARA_VHT_OP_INFO_PRESENT(he_op_ie + 3))
					offset += HE_OPER_VHT_OPER_INFO_LEN;
				if(GET_HE_OP_PARA_CO_HOSTED_BSS(he_op_ie + 3))
					offset += HE_OPER_MAX_COHOST_BSSID_LEN;
				bssid->Configuration.Band = BAND_ON_6G;
				bssid->Configuration.DSConfig = GET_HE_OP_INFO_PRIMARY_CHAN(he_op_ie + 3 + offset);
			} else
#endif
			{
				/* use current band and channel */
				bssid->Configuration.Band = rtw_get_oper_band(padapter, padapter_link);
				bssid->Configuration.DSConfig = rtw_get_oper_ch(padapter, padapter_link);
			}
		}
	}

	_rtw_memcpy(&bssid->Configuration.BeaconPeriod, rtw_get_beacon_interval_from_ie(bssid->IEs), 2);
	bssid->Configuration.BeaconPeriod = le32_to_cpu(bssid->Configuration.BeaconPeriod);

	val16 = rtw_get_capability((WLAN_BSSID_EX *)bssid);

	if ((val16 & 0x03) == cap_ESS) {
		bssid->InfrastructureMode = Ndis802_11Infrastructure;
		_rtw_memcpy(bssid->MacAddress, get_addr2_ptr(pframe), ETH_ALEN);
	} else if ((val16 & 0x03) == cap_IBSS){
		bssid->InfrastructureMode = Ndis802_11IBSS;
		_rtw_memcpy(bssid->MacAddress, GetAddr3Ptr(pframe), ETH_ALEN);
	} else if ((val16 & 0x03) == 0x00){
		u8 *mesh_id_ie, *mesh_conf_ie;
		sint mesh_id_ie_len, mesh_conf_ie_len;

		mesh_id_ie = rtw_get_ie(bssid->IEs + ie_offset, WLAN_EID_MESH_ID, &mesh_id_ie_len, bssid->IELength - ie_offset);
		mesh_conf_ie = rtw_get_ie(bssid->IEs + ie_offset, WLAN_EID_MESH_CONFIG, &mesh_conf_ie_len, bssid->IELength - ie_offset);
		if (mesh_id_ie || mesh_conf_ie) {
			if (!mesh_id_ie) {
				RTW_INFO("cannot find Mesh ID for survey event\n");
				return _FAIL;
			}
			if (mesh_id_ie_len) {
				if (mesh_id_ie_len > NDIS_802_11_LENGTH_SSID) {
					RTW_INFO("Mesh ID too long (%d) for survey event\n", mesh_id_ie_len);
					return _FAIL;
				}
				_rtw_memcpy(bssid->mesh_id.Ssid, (mesh_id_ie + 2), mesh_id_ie_len);
				bssid->mesh_id.SsidLength = mesh_id_ie_len;
			} else
				bssid->mesh_id.SsidLength = 0;

			if (!mesh_conf_ie) {
				RTW_INFO("cannot find Mesh config for survey event\n");
				return _FAIL;
			}
			if (mesh_conf_ie_len != 7) {
				RTW_INFO("invalid Mesh conf IE len (%d) for survey event\n", mesh_conf_ie_len);
				return _FAIL;
			}

			bssid->InfrastructureMode = Ndis802_11_mesh;
			_rtw_memcpy(bssid->MacAddress, GetAddr3Ptr(pframe), ETH_ALEN);
		} else {
			/* default cases */
			bssid->InfrastructureMode = Ndis802_11IBSS;
			_rtw_memcpy(bssid->MacAddress, GetAddr3Ptr(pframe), ETH_ALEN);
		}
	}

	if (val16 & BIT(4))
		bssid->Privacy = 1;
	else
		bssid->Privacy = 0;

	bssid->Configuration.ATIMWindow = 0;

	/* 20/40 BSS Coexistence check */
	if ((pregistrypriv->wifi_spec == 1) && (_FALSE == pmlmeinfo->bwmode_updated)) {
		struct link_mlme_priv *pmlmepriv = &padapter_link->mlmepriv;
#ifdef CONFIG_80211N_HT
		p = rtw_get_ie(bssid->IEs + ie_offset, _HT_CAPABILITY_IE_, &len, bssid->IELength - ie_offset);
		if (p && len > 0) {
			struct HT_caps_element	*pHT_caps;
			pHT_caps = (struct HT_caps_element *)(p + 2);

			if (pHT_caps->u.HT_cap_element.HT_caps_info & BIT(14))
				pmlmepriv->num_FortyMHzIntolerant++;
		} else
			pmlmepriv->num_sta_no_ht++;
#endif /* CONFIG_80211N_HT */

	}

#if defined(DBG_RX_SIGNAL_DISPLAY_SSID_MONITORED) & 1
	if (strcmp(bssid->Ssid.Ssid, DBG_RX_SIGNAL_DISPLAY_SSID_MONITORED) == 0) {
		RTW_INFO("Receiving %s("MAC_FMT", DSConfig:%u) from ch%u with ss:%3u, sq:%3u, RawRSSI:%d\n"
			, bssid->Ssid.Ssid, MAC_ARG(bssid->MacAddress), bssid->Configuration.DSConfig
			, rtw_get_oper_ch(padapter, padapter_link)
			, bssid->PhyInfo.SignalStrength, bssid->PhyInfo.SignalQuality, bssid->PhyInfo.rssi
			);
	}
#endif

	/* mark bss info receving from nearby channel as SignalQuality 101 */
	if (bssid->Configuration.DSConfig != rtw_get_oper_ch(padapter, padapter_link))
		bssid->PhyInfo.SignalQuality = 101;

#ifdef CONFIG_RTW_80211K
	p = rtw_get_ie(bssid->IEs + ie_offset, _EID_RRM_EN_CAP_IE_, &len, bssid->IELength - ie_offset);
	if (p)
		_rtw_memcpy(bssid->PhyInfo.rm_en_cap, (p + 2), MIN(*(p + 1),
			    sizeof(bssid->PhyInfo.rm_en_cap)));

	/* save freerun counter */
	bssid->PhyInfo.free_cnt = precv_frame->u.hdr.attrib.free_cnt;
#endif

#ifdef CONFIG_80211BE_EHT
	p = rtw_get_ie_ex(bssid->IEs + ie_offset, bssid->IELength - ie_offset, WLAN_EID_EXTENSION,
			&eid_eht_ml, 1, NULL, &len);
	if (p != NULL && len >= EHT_ML_BASIC_MIN_LEN) {
		bssid->is_mld = _TRUE;
		if (GET_ML_ELE_ML_CTRL_LINK_ID_INFO_PRESENT(p + 3))
			bssid->link_id = LE_BITS_TO_1BYTE(p + EHT_ML_BASIC_MIN_LEN, 0, 4);
	}
	else
#endif
	{
		bssid->is_mld = _FALSE;
	}

	return _SUCCESS;
}

void start_create_ibss(_adapter *padapter)
{
	unsigned short	caps;
	u8	val8;
	u8	join_type;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX		*pnetwork = (WLAN_BSSID_EX *)(&(pmlmeinfo->network));
	u8 do_rfk = _FALSE;
	pmlmeext->chandef.chan = (u8)pnetwork->Configuration.DSConfig;
	pmlmeinfo->bcn_interval = get_beacon_interval(pnetwork);

	/* update wireless mode */
	update_wireless_mode(padapter, padapter_link);

	/* udpate capability */
	caps = rtw_get_capability((WLAN_BSSID_EX *)pnetwork);
	update_capinfo(padapter, padapter_link, caps);

	if (caps & cap_IBSS) { /* adhoc master */
		/* set_opmode_cmd(padapter, adhoc); */ /* removed */

		rtw_hal_set_hwreg(padapter, HW_VAR_SEC_CFG, NULL);

		do_rfk = _TRUE;
		/* switch channel */
		set_channel_bwmode(padapter, padapter_link,
				pmlmeext->chandef.chan,
				CHAN_OFFSET_NO_EXT,
				CHANNEL_WIDTH_20,
				do_rfk);

		beacon_timing_control(padapter);

		/* issue beacon */
		if (send_beacon(padapter) == _FAIL) {

			report_join_res(padapter, -1, WLAN_STATUS_UNSPECIFIED_FAILURE);
			padapter->mlmeextpriv.mlmext_info.state = WIFI_FW_NULL_STATE;
		} else {
			rtw_hal_set_hwreg(padapter, HW_VAR_BSSID, padapter->registrypriv.dev_network.MacAddress);
			join_type = 0;
			rtw_hal_set_hwreg(padapter, HW_VAR_MLME_JOIN, (u8 *)(&join_type));

			report_join_res(padapter, 1, WLAN_STATUS_SUCCESS);
			padapter->mlmeextpriv.mlmext_info.state |= WIFI_FW_ASSOC_SUCCESS;
			rtw_indicate_connect(padapter);
		}
	} else {
		RTW_INFO("start_create_ibss, invalid cap:%x\n", caps);
		return;
	}
}

void start_clnt_join(_adapter *padapter)
{
	unsigned short	caps;
	int beacon_timeout;
	u8 ASIX_ID[] = {0x00, 0x0E, 0xC6};
	/* Client join with the primary link */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX		*pnetwork = (WLAN_BSSID_EX *)(&(pmlmeinfo->network));
	u8 lidx;


	/* update wireless mode and capability of the primary adapter_link*/
	update_wireless_mode(padapter, padapter_link);
	caps = rtw_get_capability(pnetwork);
	update_capinfo(padapter, padapter_link, caps);
	if (pnetwork->is_mld) {
		for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
			padapter_link = GET_LINK(padapter, lidx);
			if (!padapter_link->mlmepriv.to_join || lidx == RTW_RLINK_PRIMARY)
				continue;
			update_wireless_mode(padapter, padapter_link);
			caps = rtw_get_capability(&padapter_link->mlmeextpriv.mlmext_info.network);
			update_capinfo(padapter, padapter_link, caps);
		}
	}
	/* check if sta is ASIX peer and fix IOT issue if it is. */
	/*GEORGIA_TODO_FIXIT_HAL_DEP*/
	if (_rtw_memcmp(get_my_bssid(&pmlmeinfo->network) , ASIX_ID , 3)) {
		u8 iot_flag = _TRUE;
		rtw_hal_set_hwreg(padapter, HW_VAR_ASIX_IOT, (u8 *)(&iot_flag));
	}

	if (caps & cap_ESS) {
		rtw_hal_set_hwreg(padapter, HW_VAR_SEC_CFG, NULL);

#ifdef CONFIG_DEAUTH_BEFORE_CONNECT
		/* Because of AP's not receiving deauth before */
		/* AP may: 1)not response auth or 2)deauth us after link is complete */
		/* issue deauth before issuing auth to deal with the situation */

		/*	Commented by Albert 2012/07/21 */
		/*	For the Win8 P2P connection, it will be hard to have a successful connection if this Wi-Fi doesn't connect to it. */
		{
#ifdef CONFIG_P2P
			_queue *queue = &(padapter->mlmepriv.scanned_queue);
			_list	*head = get_list_head(queue);
			_list *pos = get_next(head);
			struct wlan_network *scanned = NULL;
			u8 ie_offset = 0;
			bool has_p2p_ie = _FALSE;

			_rtw_spinlock_bh(&(padapter->mlmepriv.scanned_queue.lock));

			for (pos = get_next(head); !rtw_end_of_queue_search(head, pos); pos = get_next(pos)) {

				scanned = LIST_CONTAINOR(pos, struct wlan_network, list);

				if (_rtw_memcmp(&(scanned->network.Ssid), &(pnetwork->Ssid), sizeof(NDIS_802_11_SSID)) == _TRUE
				    && _rtw_memcmp(scanned->network.MacAddress, pnetwork->MacAddress, sizeof(NDIS_802_11_MAC_ADDRESS)) == _TRUE
				   ) {
					ie_offset = (scanned->network.Reserved[0] == BSS_TYPE_PROB_REQ ? 0 : 12);
					if (rtw_get_p2p_ie(scanned->network.IEs + ie_offset, scanned->network.IELength - ie_offset, NULL, NULL))
						has_p2p_ie = _TRUE;
					break;
				}
			}

			_rtw_spinunlock_bh(&(padapter->mlmepriv.scanned_queue.lock));

			if (scanned == NULL || rtw_end_of_queue_search(head, pos) || has_p2p_ie == _FALSE)
#endif /* CONFIG_P2P */
				/* To avoid connecting to AP fail during resume process, change retry count from 5 to 1 */
				issue_deauth_ex(padapter, pnetwork->MacAddress, WLAN_REASON_DEAUTH_LEAVING, 1, 100);
		}
#endif /* CONFIG_DEAUTH_BEFORE_CONNECT */

		/* here wait for receiving the beacon to start auth */
		/* and enable a timer */
		beacon_timeout = decide_wait_for_beacon_timeout(pmlmeinfo->bcn_interval);
		set_link_timer(&padapter->mlmeextpriv, beacon_timeout);
		/*_set_timer(&padapter->mlmepriv.assoc_timer,
			(REAUTH_TO * REAUTH_LIMIT) + (REASSOC_TO * REASSOC_LIMIT) + beacon_timeout);*/
		set_assoc_timer(&padapter->mlmepriv,
			(REAUTH_TO * REAUTH_LIMIT) + (REASSOC_TO * REASSOC_LIMIT) + beacon_timeout);

#ifdef CONFIG_RTW_80211R
		if (rtw_ft_roam(padapter)) {
			rtw_ft_start_clnt_join(padapter);
		} else
#endif
		{
			rtw_sta_linking_test_set_start();
			padapter->mlmeextpriv.mlmext_info.state = WIFI_FW_AUTH_NULL | WIFI_FW_STATION_STATE;
		}

#ifdef CONFIG_DRV_FAKE_AP
		_set_timer(&adapter_to_dvobj(padapter)->fakeap.bcn_timer, 5);
#endif /* CONFIG_DRV_FAKE_AP */
	} else if (caps & cap_IBSS) { /* adhoc client */
		rtw_hal_set_hwreg(padapter, HW_VAR_SEC_CFG, NULL);

		beacon_timing_control(padapter);

		padapter->mlmeextpriv.mlmext_info.state = WIFI_FW_ADHOC_STATE;

		report_join_res(padapter, 1, WLAN_STATUS_SUCCESS);
	} else {
		/* RTW_INFO("marc: invalid cap:%x\n", caps); */
		return;
	}

}

void start_clnt_auth(_adapter *padapter)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	cancel_link_timer(pmlmeext); /*_cancel_timer_ex(&pmlmeext->link_timer);*/

	pmlmeinfo->state &= (~WIFI_FW_AUTH_NULL);
	pmlmeinfo->state |= WIFI_FW_AUTH_STATE;

	pmlmeinfo->auth_seq = 1;
	pmlmeinfo->reauth_count = 0;
	pmlmeinfo->reassoc_count = 0;
	pmlmeinfo->link_count = 0;
	pmlmeext->retry = 0;

#ifdef CONFIG_RTW_80211R
	if (rtw_ft_roam(padapter)) {
		rtw_ft_set_status(padapter, RTW_FT_AUTHENTICATING_STA);
		RTW_PRINT("start ft auth\n");
	} else
#endif
		RTW_PRINT("start auth\n");

#ifdef CONFIG_IOCTL_CFG80211
	if (rtw_sec_chk_auth_type(padapter, MLME_AUTHTYPE_SAE)) {
		if (rtw_cached_pmkid(padapter, get_my_bssid(&pmlmeinfo->dev_network)) != -1) {
			RTW_INFO("SAE: PMKSA cache entry found\n");
			padapter->securitypriv.auth_alg = WLAN_AUTH_OPEN;
			goto no_external_auth;
		}

		RTW_PRINT("SAE: start external auth\n");
		rtw_cfg80211_external_auth_request(padapter, NULL);
		return;
	}
no_external_auth:
#endif /* CONFIG_IOCTL_CFG80211 */

	issue_auth(padapter, NULL, 0);

	set_link_timer(pmlmeext, REAUTH_TO);

}


void start_clnt_assoc(_adapter *padapter)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	cancel_link_timer(pmlmeext); /*_cancel_timer_ex(&pmlmeext->link_timer);*/

	pmlmeinfo->state &= (~(WIFI_FW_AUTH_NULL | WIFI_FW_AUTH_STATE));
	pmlmeinfo->state |= (WIFI_FW_AUTH_SUCCESS | WIFI_FW_ASSOC_STATE);

#ifdef CONFIG_RTW_80211R
	if (rtw_ft_roam(padapter)
	#ifdef CONFIG_RTW_WNM
		|| rtw_wnm_btm_reassoc_req(padapter)
	#endif
	)
		issue_reassocreq(padapter);
	else
#endif
		issue_assocreq(padapter);

	set_link_timer(pmlmeext, REASSOC_TO);
}

unsigned int receive_disconnect(_adapter *padapter, unsigned char *MacAddr, unsigned short reason, u8 locally_generated)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (!(rtw_match_bssid_ext(padapter, MacAddr)))
		return _SUCCESS;

	RTW_INFO("%s\n", __FUNCTION__);

	if ((pmlmeinfo->state & 0x03) == WIFI_FW_STATION_STATE) {
		if (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) {
			if (report_del_sta_event(padapter, MacAddr, reason, _TRUE, locally_generated) != _FAIL)
				pmlmeinfo->state = WIFI_FW_NULL_STATE;
		} else if (pmlmeinfo->state & WIFI_FW_LINKING_STATE) {
			if (report_join_res(padapter, -2, reason) != _FAIL)
				pmlmeinfo->state = WIFI_FW_NULL_STATE;
		} else
			RTW_INFO(FUNC_ADPT_FMT" - End to Disconnect\n", FUNC_ADPT_ARG(padapter));
#ifdef CONFIG_RTW_80211R
		rtw_ft_roam_status_reset(padapter);
#endif
#ifdef CONFIG_RTW_WNM
		rtw_wnm_reset_btm_state(padapter);
#endif
	}

	return _SUCCESS;
}

/*
 * Return channel index of struct dvobj_priv.rf_ctl.channel_set[] for channel match
 * BSS_EX_OP_BAND(bss) & BSS_EX_OP_CH(bss), or -1 if not found.
 */
static int rtw_hidden_ssid_bss_count(_adapter *adapter, WLAN_BSSID_EX *bss)
{
	struct rtw_chset *chset = adapter_to_chset(adapter);
	int chset_idx;

	if (bss->InfrastructureMode != Ndis802_11Infrastructure)
		return -1;

	if (!hidden_ssid_ap(bss))
		return -1;

	chset_idx = rtw_chset_search_bch(chset, BSS_EX_OP_BAND(bss), BSS_EX_OP_CH(bss));
	if (chset_idx < 0)
		return chset_idx;

	chset->chs[chset_idx].hidden_bss_cnt++;
	return chset_idx;
}

/****************************************************************************

Following are the functions to report events

*****************************************************************************/

void report_survey_event(_adapter *padapter, union recv_frame *precv_frame)
{
	struct cmd_obj *pcmd_obj;
	u8 *pevtcmd;
	u32 cmdsz;
	struct survey_event *psurvey_evt;
	struct rtw_evt_header *evt_hdr;
	struct mlme_ext_priv *pmlmeext;
	struct cmd_priv *pcmdpriv;
	/* u8 *pframe = precv_frame->u.hdr.rx_data; */
	/* uint len = precv_frame->u.hdr.len; */
#ifdef CONFIG_CMD_SCAN
	struct rtw_chset *chset;
	int ch_set_idx = -1;
#endif /* CONFIG_CMD_SCAN */
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);


	if (!padapter)
		return;

	pmlmeext = &padapter->mlmeextpriv;
	pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;

	pcmd_obj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmd_obj == NULL)
		return;
	pcmd_obj->padapter = padapter;

	cmdsz = (sizeof(struct survey_event) + sizeof(struct rtw_evt_header));
	pevtcmd = (u8 *)rtw_zmalloc(cmdsz);
	if (pevtcmd == NULL) {
		rtw_mfree((u8 *)pcmd_obj, sizeof(struct cmd_obj));
		return;
	}

	_rtw_init_listhead(&pcmd_obj->list);

	CMD_OBJ_SET_HWBAND(pcmd_obj, HW_BAND_MAX);
	pcmd_obj->cmdcode = CMD_SET_MLME_EVT;
	pcmd_obj->cmdsz = cmdsz;
	pcmd_obj->parmbuf = pevtcmd;

	pcmd_obj->rsp = NULL;
	pcmd_obj->rspsz  = 0;

	evt_hdr = (struct rtw_evt_header *)(pevtcmd);
	evt_hdr->len = sizeof(struct survey_event);
	evt_hdr->id = EVT_SURVEY;
	evt_hdr->seq = ATOMIC_INC_RETURN(&pmlmeext->event_seq);

	psurvey_evt = (struct survey_event *)(pevtcmd + sizeof(struct rtw_evt_header));

	if (collect_bss_info(padapter, precv_frame, (WLAN_BSSID_EX *)&psurvey_evt->bss) == _FAIL) {
		rtw_mfree((u8 *)pcmd_obj, sizeof(struct cmd_obj));
		rtw_mfree((u8 *)pevtcmd, cmdsz);
		return;
	}

#ifdef CONFIG_CMD_SCAN
	ch_set_idx = rtw_hidden_ssid_bss_count(padapter, &psurvey_evt->bss);

	chset = adapter_to_chset(padapter);
	if ((ch_set_idx >= 0) && (chset->chs[ch_set_idx].hidden_bss_cnt == 1)
	    && pmlmeext->sitesurvey_res.scan_param) {
		struct rtw_phl_scan_param *phl_scan;
		struct phl_module_op_info op_info = {0};
		u8 ch;
		int i;

		phl_scan = pmlmeext->sitesurvey_res.scan_param;
		ch = chset->chs[ch_set_idx].ChannelNum;
		for (i = 0; i < phl_scan->ch_num; i++) {
			if (phl_scan->ch[i].channel != ch)
				continue;
			if (phl_scan->ch[i].type != RTW_PHL_SCAN_PASSIVE)
				break;

			op_info.op_code = FG_REQ_OP_NOTIFY_BCN_RCV;
			op_info.inbuf = &ch;
			op_info.inlen = 1;
			rtw_phl_set_cur_cmd_info(GET_PHL_INFO(adapter_to_dvobj(padapter)),
						padapter_link->wrlink->hw_band,
						&op_info);
			break;
		}
	}
#else /* !CONFIG_CMD_SCAN */
	rtw_hidden_ssid_bss_count(padapter, &psurvey_evt->bss);
#endif /* !CONFIG_CMD_SCAN */

	rtw_enqueue_cmd(pcmdpriv, pcmd_obj);

	pmlmeext->sitesurvey_res.bss_cnt++;

	return;

}

/*
* @acs: aim to trigger channel selection
*/
void report_surveydone_event(_adapter *padapter, bool acs, u8 flags)
{
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	/* Currently only use the primary link to scan */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (flags & RTW_CMDF_DIRECTLY) {
		struct surveydone_event survey_done;

		survey_done.bss_cnt = pmlmeext->sitesurvey_res.bss_cnt;
		survey_done.activate_ch_cnt = pmlmeext->sitesurvey_res.activate_ch_cnt;
		survey_done.acs = acs;
		RTW_INFO("survey done event(%x) wifi band:%d for "ADPT_FMT"-HW_BAND_%d\n",
			survey_done.bss_cnt, padapter->setband, ADPT_ARG(padapter),
			padapter_link->wrlink->hw_band);
		rtw_surveydone_event_callback(padapter, (u8 *)&survey_done);
	} else {

		struct cmd_obj *pcmd_obj;
		u8 *pevtcmd;
		u32 cmdsz;
		struct surveydone_event *psurveydone_evt;
		struct rtw_evt_header *evt_hdr;

		struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;

		pcmd_obj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
		if (pcmd_obj == NULL)
			return;
		pcmd_obj->padapter = padapter;

		cmdsz = (sizeof(struct surveydone_event) + sizeof(struct rtw_evt_header));
		pevtcmd = (u8 *)rtw_zmalloc(cmdsz);
		if (pevtcmd == NULL) {
			rtw_mfree((u8 *)pcmd_obj, sizeof(struct cmd_obj));
			return;
		}

		_rtw_init_listhead(&pcmd_obj->list);

		CMD_OBJ_SET_HWBAND(pcmd_obj, HW_BAND_MAX);
		pcmd_obj->cmdcode = CMD_SET_MLME_EVT;
		pcmd_obj->cmdsz = cmdsz;
		pcmd_obj->parmbuf = pevtcmd;

		pcmd_obj->rsp = NULL;
		pcmd_obj->rspsz  = 0;

		evt_hdr = (struct rtw_evt_header *)(pevtcmd);
		evt_hdr->len = sizeof(struct surveydone_event);
		evt_hdr->id = EVT_SURVEY_DONE;
		evt_hdr->seq = ATOMIC_INC_RETURN(&pmlmeext->event_seq);

		psurveydone_evt = (struct surveydone_event *)(pevtcmd + sizeof(struct rtw_evt_header));
		psurveydone_evt->bss_cnt = pmlmeext->sitesurvey_res.bss_cnt;
		psurveydone_evt->activate_ch_cnt = pmlmeext->sitesurvey_res.activate_ch_cnt;
		psurveydone_evt->acs = acs;

		RTW_INFO("survey done event(%x) wifi band:%d for "ADPT_FMT"- HW_BAND_%d\n",
			psurveydone_evt->bss_cnt, padapter->setband, ADPT_ARG(padapter),
			padapter_link->wrlink->hw_band);

		rtw_enqueue_cmd(pcmdpriv, pcmd_obj);
	}

	return;
}

u32 report_join_res(_adapter *padapter, int aid_res, u16 status)
{
	struct cmd_obj *pcmd_obj;
	u8	*pevtcmd;
	u32 cmdsz;
	struct joinbss_event *pjoinbss_evt;
	struct rtw_evt_header *evt_hdr;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	u32 ret = _FAIL;
#ifdef CONFIG_80211BE_EHT
	struct _ADAPTER_LINK *adapter_link;
	struct wlan_mld_network *pmld_network;
	struct wlan_network *rec_link_network;
	WLAN_BSSID_EX *pnetwork;
	u8 lidx;
#endif

	pcmd_obj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmd_obj == NULL)
		goto exit;
	pcmd_obj->padapter = padapter;

	cmdsz = (sizeof(struct joinbss_event) + sizeof(struct rtw_evt_header));
	pevtcmd = (u8 *)rtw_zmalloc(cmdsz);
	if (pevtcmd == NULL) {
		rtw_mfree((u8 *)pcmd_obj, sizeof(struct cmd_obj));
		goto exit;
	}

	_rtw_init_listhead(&pcmd_obj->list);

	CMD_OBJ_SET_HWBAND(pcmd_obj, HW_BAND_MAX);
	pcmd_obj->cmdcode = CMD_SET_MLME_EVT;
	pcmd_obj->cmdsz = cmdsz;
	pcmd_obj->parmbuf = pevtcmd;

	pcmd_obj->rsp = NULL;
	pcmd_obj->rspsz  = 0;

	evt_hdr = (struct rtw_evt_header *)(pevtcmd);
	evt_hdr->len = sizeof(struct joinbss_event);
	evt_hdr->id = EVT_JOINBSS;
	evt_hdr->seq = ATOMIC_INC_RETURN(&padapter->mlmeextpriv.event_seq);

	pjoinbss_evt = (struct joinbss_event *)(pevtcmd + sizeof(struct rtw_evt_header));
	_rtw_memcpy((unsigned char *)(&(pjoinbss_evt->network.network)), &(pmlmeinfo->network), sizeof(WLAN_BSSID_EX));
	pjoinbss_evt->network.join_res = pjoinbss_evt->network.aid = aid_res;
#ifdef CONFIG_80211BE_EHT
	if (pmlmeinfo->network.is_mld) {
		pmld_network = (struct wlan_mld_network *)rtw_zmalloc(sizeof(struct wlan_mld_network));
		if (!pmld_network) {
			RTW_INFO(FUNC_ADPT_FMT" Failed to allocate mld network\n", FUNC_ADPT_ARG(padapter));
			goto exit;
		}
		_rtw_memset(pmld_network, 0, sizeof(struct wlan_mld_network));
		_rtw_memcpy(pmld_network->mac_addr, padapter->mlmeextpriv.mlmext_info.dev_network.MacAddress, ETH_ALEN);

		/* Link mld network with the primary link */
		rtw_link_mld_network(padapter, pmld_network, &pjoinbss_evt->network);

		for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
			adapter_link = GET_LINK(padapter, lidx);
			if (!adapter_link->mlmepriv.to_join || adapter_link == padapter_link)
				continue;

			pnetwork = &adapter_link->mlmeextpriv.mlmext_info.network;
			rec_link_network = (struct wlan_network *)rtw_zmalloc(sizeof(struct wlan_network));
			if (!rec_link_network) {
				RTW_INFO(FUNC_ADPT_FMT" cannot allocate link network for link_id %u, set adapter_link %d to_join to _FALSE\n"
					, FUNC_ADPT_ARG(padapter), adapter_link->mlmepriv.link_id, adapter_link->wrlink->id);
				adapter_link->mlmepriv.to_join = _FALSE;
				continue;
			}
			_rtw_memcpy(&rec_link_network->network, pnetwork, sizeof(WLAN_BSSID_EX));
			rtw_link_mld_network(padapter, pmld_network, rec_link_network);
		}
	}
#endif

	RTW_INFO("report_join_res(%d, %u)\n", aid_res, status);


	rtw_joinbss_event_prehandle(padapter, (u8 *)&pjoinbss_evt->network, status);


	ret = rtw_enqueue_cmd(pcmdpriv, pcmd_obj);

exit:
	return ret;
}

void report_wmm_edca_update(_adapter *padapter)
{
	struct cmd_obj *pcmd_obj;
	u8 *pevtcmd;
	u32 cmdsz;
	struct wmm_event *pwmm_event;
	struct rtw_evt_header *evt_hdr;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;

	pcmd_obj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmd_obj == NULL)
		return;
	pcmd_obj->padapter = padapter;

	cmdsz = (sizeof(struct wmm_event) + sizeof(struct rtw_evt_header));
	pevtcmd = (u8 *)rtw_zmalloc(cmdsz);
	if (pevtcmd == NULL) {
		rtw_mfree((u8 *)pcmd_obj, sizeof(struct cmd_obj));
		return;
	}

	_rtw_init_listhead(&pcmd_obj->list);

	CMD_OBJ_SET_HWBAND(pcmd_obj, HW_BAND_MAX);
	pcmd_obj->cmdcode = CMD_SET_MLME_EVT;
	pcmd_obj->cmdsz = cmdsz;
	pcmd_obj->parmbuf = pevtcmd;

	pcmd_obj->rsp = NULL;
	pcmd_obj->rspsz  = 0;

	evt_hdr = (struct rtw_evt_header *)(pevtcmd);
	evt_hdr->len = sizeof(struct wmm_event);
	evt_hdr->id = EVT_WMM_UPDATE;
	evt_hdr->seq = ATOMIC_INC_RETURN(&pmlmeext->event_seq);

	pwmm_event = (struct wmm_event *)(pevtcmd + sizeof(struct rtw_evt_header));
	pwmm_event->wmm = 0;

	rtw_enqueue_cmd(pcmdpriv, pcmd_obj);

	return;

}

u32 report_del_sta_event(_adapter *padapter, unsigned char *MacAddr, unsigned short reason, bool enqueue, u8 locally_generated)
{
	struct cmd_obj *pcmd_obj;
	u8 *pevtcmd;
	u32 cmdsz;
	struct sta_info *psta;
	int mac_id = -1;
	struct stadel_event *pdel_sta_evt;
	struct rtw_evt_header *evt_hdr;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	u8 res = _SUCCESS;


#ifdef CONFIG_AP_CMD_DISPR
	if (MLME_IS_AP(padapter)) {
		RTW_INFO("%s : enqueue=%u\n", __func__, enqueue);
		if (enqueue) {
			res = rtw_add_del_sta_cmd_check(padapter, MacAddr, _FALSE);
		} else {
			psta = rtw_get_stainfo(&padapter->stapriv, MacAddr);
			if (!psta || !psta->phl_sta || !psta->phl_sta->mld) {
				res = _FAIL;
			} else {
				rtw_hw_disconnect(padapter, psta);
				res = rtw_free_mld_stainfo(padapter, psta->phl_sta->mld);
			}
		}
		goto exit;
	}
#endif

	/* prepare cmd parameter */
	cmdsz = (sizeof(struct stadel_event) + sizeof(struct rtw_evt_header));
	pevtcmd = (u8 *)rtw_zmalloc(cmdsz);
	if (pevtcmd == NULL) {
		res = _FAIL;
		goto exit;
	}

	evt_hdr = (struct rtw_evt_header *)(pevtcmd);
	evt_hdr->len = sizeof(struct stadel_event);
	evt_hdr->id = EVT_DEL_STA;
	evt_hdr->seq = ATOMIC_INC_RETURN(&pmlmeext->event_seq);

	pdel_sta_evt = (struct stadel_event *)(pevtcmd + sizeof(struct rtw_evt_header));
	_rtw_memcpy((unsigned char *)(&(pdel_sta_evt->macaddr)), MacAddr, ETH_ALEN);
	_rtw_memcpy((unsigned char *)(pdel_sta_evt->rsvd), (unsigned char *)(&reason), 2);
	psta = rtw_get_stainfo(&padapter->stapriv, MacAddr);
	if (psta)
		mac_id = (int)psta->phl_sta->macid;
	else
		mac_id = (-1);
	pdel_sta_evt->mac_id = mac_id;
	pdel_sta_evt->locally_generated = locally_generated;

	if (!enqueue) {
		/* do directly */
		rtw_stadel_event_callback(padapter, (u8 *)pdel_sta_evt);
		rtw_mfree(pevtcmd, cmdsz);
	} else {
		pcmd_obj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
		if (pcmd_obj == NULL) {
			rtw_mfree(pevtcmd, cmdsz);
			res = _FAIL;
			goto exit;
		}
		pcmd_obj->padapter = padapter;

		_rtw_init_listhead(&pcmd_obj->list);
		CMD_OBJ_SET_HWBAND(pcmd_obj, HW_BAND_MAX);
		pcmd_obj->cmdcode = CMD_SET_MLME_EVT;
		pcmd_obj->cmdsz = cmdsz;
		pcmd_obj->parmbuf = pevtcmd;

		pcmd_obj->rsp = NULL;
		pcmd_obj->rspsz  = 0;

		res = rtw_enqueue_cmd(pcmdpriv, pcmd_obj);
	}

exit:

	RTW_INFO(FUNC_ADPT_FMT" "MAC_FMT" mac_id=%d, enqueue:%d, res:%u\n"
		, FUNC_ADPT_ARG(padapter), MAC_ARG(MacAddr), mac_id, enqueue, res);

	return res;
}

void report_add_sta_event(_adapter *padapter, unsigned char *MacAddr)
{
	struct cmd_obj *pcmd_obj;
	u8 *pevtcmd;
	u32 cmdsz;
	struct stassoc_event *padd_sta_evt;
	struct rtw_evt_header *evt_hdr;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;

#ifdef CONFIG_AP_CMD_DISPR
	if (MLME_IS_AP(padapter)){
		rtw_add_del_sta_cmd_check(padapter, MacAddr, _TRUE);
		return;
	}
#endif

	pcmd_obj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmd_obj == NULL)
		return;
	pcmd_obj->padapter = padapter;

	cmdsz = (sizeof(struct stassoc_event) + sizeof(struct rtw_evt_header));
	pevtcmd = (u8 *)rtw_zmalloc(cmdsz);
	if (pevtcmd == NULL) {
		rtw_mfree((u8 *)pcmd_obj, sizeof(struct cmd_obj));
		return;
	}

	_rtw_init_listhead(&pcmd_obj->list);

	CMD_OBJ_SET_HWBAND(pcmd_obj, HW_BAND_MAX);
	pcmd_obj->cmdcode = CMD_SET_MLME_EVT;
	pcmd_obj->cmdsz = cmdsz;
	pcmd_obj->parmbuf = pevtcmd;

	pcmd_obj->rsp = NULL;
	pcmd_obj->rspsz  = 0;

	evt_hdr = (struct rtw_evt_header *)(pevtcmd);
	evt_hdr->len = sizeof(struct stassoc_event);
	evt_hdr->id = EVT_ADD_STA;
	evt_hdr->seq = ATOMIC_INC_RETURN(&pmlmeext->event_seq);

	padd_sta_evt = (struct stassoc_event *)(pevtcmd + sizeof(struct rtw_evt_header));
	_rtw_memcpy((unsigned char *)(&(padd_sta_evt->macaddr)), MacAddr, ETH_ALEN);

	RTW_INFO("report_add_sta_event: add STA\n");

	rtw_enqueue_cmd(pcmdpriv, pcmd_obj);

	return;
}

/****************************************************************************

Following are the event callback functions

*****************************************************************************/

/* for sta/adhoc mode */
void update_sta_info(_adapter *padapter, struct sta_info *psta) {
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	/* ERP */
	VCS_update(padapter, psta);

#ifdef CONFIG_80211N_HT
	/* HT */
	if (pmlmepriv->htpriv.ht_option) {
		psta->htpriv.ht_option = _TRUE;

		psta->ampdu_priv.ampdu_enable = pmlmepriv->ampdu_priv.ampdu_enable;

		psta->ampdu_priv.rx_ampdu_min_spacing = (pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & IEEE80211_HT_CAP_AMPDU_DENSITY) >> 2;

		if (support_short_GI(padapter, padapter_link, &(pmlmeinfo->HT_caps), CHANNEL_WIDTH_20))
			psta->htpriv.sgi_20m = _TRUE;

		if (support_short_GI(padapter, padapter_link, &(pmlmeinfo->HT_caps), CHANNEL_WIDTH_40))
			psta->htpriv.sgi_40m = _TRUE;

		psta->qos_option = _TRUE;

		psta->htpriv.ldpc_cap = pmlmepriv->htpriv.ldpc_cap;
		psta->htpriv.stbc_cap = pmlmepriv->htpriv.stbc_cap;
		psta->htpriv.beamform_cap = pmlmepriv->htpriv.beamform_cap;

		_rtw_memcpy(&psta->htpriv.ht_cap, &pmlmeinfo->HT_caps, sizeof(struct rtw_ieee80211_ht_cap));
		#ifdef CONFIG_BEAMFORMING
		psta->htpriv.beamform_cap = pmlmepriv->htpriv.beamform_cap;
		#endif
	} else
#endif /* CONFIG_80211N_HT */
	{
#ifdef CONFIG_80211N_HT
		psta->htpriv.ht_option = _FALSE;
		psta->ampdu_priv.ampdu_enable = _FALSE;
		psta->ampdu_priv.tx_amsdu_enable = _FALSE;
		psta->htpriv.sgi_20m = _FALSE;
		psta->htpriv.sgi_40m = _FALSE;
#endif /* CONFIG_80211N_HT */
		psta->qos_option = _FALSE;

	}

#ifdef CONFIG_80211N_HT
	psta->htpriv.ch_offset = pmlmeext->chandef.offset;

	psta->ampdu_priv.agg_enable_bitmap = 0x0;/* reset */
	psta->ampdu_priv.candidate_tid_bitmap = 0x0;/* reset */
#endif /* CONFIG_80211N_HT */

	psta->phl_sta->chandef.bw = pmlmeext->chandef.bw;

	/* QoS */
	if (pmlmepriv->qospriv.qos_option)
		psta->qos_option = _TRUE;

#ifdef CONFIG_80211AC_VHT
	_rtw_memcpy(&psta->vhtpriv, &pmlmepriv->vhtpriv, sizeof(struct vht_priv));
	if (psta->vhtpriv.vht_option) {
		/* ToDo: need to API to inform hal_sta->ra_info.is_vht_enable  */
		/* psta->phl_sta->ra_info.is_vht_enable = _TRUE; */
		#ifdef CONFIG_BEAMFORMING
		psta->vhtpriv.beamform_cap = pmlmepriv->vhtpriv.beamform_cap;
		#endif /*CONFIG_BEAMFORMING*/
	}
#endif /* CONFIG_80211AC_VHT */

#ifdef CONFIG_80211AX_HE
	_rtw_memcpy(&psta->hepriv, &pmlmepriv->hepriv, sizeof(struct he_priv));
#endif /* CONFIG_80211AX_HE */

#if CONFIG_IEEE80211_BAND_6GHZ
	if (psta->phl_sta->chandef.band == BAND_ON_6G) {
		psta->ampdu_priv.ampdu_enable = pmlmepriv->ampdu_priv.ampdu_enable;
		/* update mpdu_min_spacing in HE_6g_bandcap_handler */
		psta->ampdu_priv.rx_ampdu_min_spacing = psta->hepriv.mpdu_min_spacing;
	}
#endif

	/* ToDo: need to API to inform hal_sta->ra_info.is_support_sgi  */
	/* psta->phl_sta->ra_info.is_support_sgi = query_ra_short_GI(psta, rtw_get_tx_bw_mode(padapter, psta)); */

	_rtw_spinlock_bh(&psta->lock);
	psta->state = WIFI_ASOC_STATE;
	_rtw_spinunlock_bh(&psta->lock);

}

void update_sta_trx_nss(_adapter *adapter, struct sta_info *psta)
{
	s8 tx_nss, rx_nss;
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;

	/* get adapter tx nss */
	tx_nss = rtw_get_sta_tx_nss(adapter, psta);
	/* get adapter rx nss */
	rx_nss =  rtw_get_sta_rx_nss(adapter, psta);

	/* peer sta tx should referece adapter rx_nss */
	psta->phl_sta->asoc_cap.nss_tx = rx_nss;
	/* peer sta rx should referece adapter tx_nss */
	psta->phl_sta->asoc_cap.nss_rx = tx_nss;

#ifdef CONFIG_DBCC_SUPPORT
	if (rtw_phl_mr_is_db(adapter_to_dvobj(adapter)->phl)) {
		RTW_INFO("%s, stay in dual-band update_sta_trx_nss\n", __func__);
		rtw_ctrl_and_backup_assoc_cap_rx_nss(adapter, psta, 1);
	}
#endif

#ifdef CONFIG_CTRL_TXSS_BY_TP
	rtw_ctrl_txss_update(adapter, psta);
#endif

	RTW_INFO("STA - MAC_ID:%d, Tx - %d SS, Rx - %d SS\n",
			psta->phl_sta->macid, tx_nss, rx_nss);
}

void update_sta_smps_cap(_adapter *adapter, struct sta_info *psta)
{
	struct _ADAPTER_LINK	*adapter_link = psta->padapter_link;
	struct mlme_ext_priv	*pmlmeext = &adapter->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(adapter_link->mlmeextpriv.mlmext_info);

	/*Spatial Multiplexing Power Save*/
	if (check_fwstate(&adapter->mlmepriv, WIFI_AP_STATE) == _TRUE)
		psta->phl_sta->asoc_cap.sm_ps = psta->smps_mode;
	else
		psta->phl_sta->asoc_cap.sm_ps = pmlmeinfo->SM_PS;

	RTW_INFO("STA - MAC_ID:%d, SM_PS %d\n",
		psta->phl_sta->macid, psta->phl_sta->asoc_cap.sm_ps);

	if (psta->phl_sta->asoc_cap.sm_ps == SM_PS_STATIC
		&& psta->phl_sta->asoc_cap.nss_rx != 1) {
		pmlmeext->txss_bk = psta->phl_sta->asoc_cap.nss_rx;
		rtw_phl_sta_assoc_cap_process(psta->phl_sta, _TRUE);
		psta->phl_sta->asoc_cap.nss_rx = 1;
		psta->phl_sta->asoc_cap.stbc_ht_rx = 0;
		psta->phl_sta->asoc_cap.stbc_vht_rx = 0;
		psta->phl_sta->asoc_cap.stbc_he_rx = 0;
	}
	RTW_INFO("STA - MAC_ID:%d, Tx - %d SS, Rx - %d SS\n",
			psta->phl_sta->macid,
			psta->phl_sta->asoc_cap.nss_tx,
			psta->phl_sta->asoc_cap.nss_rx);
}

void update_sta_rate_mask(_adapter *padapter, struct sta_info *psta)
{
	struct registry_priv *registrypriv = &padapter->registrypriv;
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	u8 i, tx_nss;
	u64 tx_ra_bitmap = 0, tmp64=0;

	if (psta == NULL)
		return;

	/* b/g mode ra_bitmap  */
	for (i = 0; i < sizeof(psta->bssrateset); i++) {
		if (psta->bssrateset[i])
			tx_ra_bitmap |= rtw_get_bit_value_from_ieee_value(psta->bssrateset[i] & 0x7f);
	}

#ifdef CONFIG_80211N_HT
	if (registrypriv->ht_enable && is_supported_ht(registrypriv->wireless_mode)) {
		tx_nss = get_phy_tx_nss(padapter, padapter_link);
#ifdef CONFIG_80211AC_VHT
		if (psta->vhtpriv.vht_option) {
			/* AC mode ra_bitmap */
			tx_ra_bitmap |= (rtw_vht_mcs_map_to_bitmap(psta->vhtpriv.vht_mcs_map, tx_nss) << 12);
		} else
#endif /* CONFIG_80211AC_VHT */
		{
			if (psta->htpriv.ht_option) {
				/* n mode ra_bitmap */

				/* Handling SMPS mode for AP MODE only*/
				if (check_fwstate(&padapter->mlmepriv, WIFI_AP_STATE) == _TRUE) {
				/*0:static SMPS, 1:dynamic SMPS, 3:SMPS disabled, 2:reserved*/
					if (psta->smps_mode == SM_PS_STATIC || psta->smps_mode == SM_PS_DYNAMIC)
						/*operate with only one active receive chain // 11n-MCS rate <= MSC7*/
						tx_nss = rtw_min(tx_nss, 1);
				}

				tmp64 = rtw_ht_mcs_set_to_bitmap(psta->htpriv.ht_cap.supp_mcs_set, tx_nss);
				tx_ra_bitmap |= (tmp64 << 12);
			}
		}
	}
#endif /* CONFIG_80211N_HT */
	/* ToDo: Need API to inform hal_sta->ra_info.ramask */
	/* psta->phl_sta->ra_info.ramask = tx_ra_bitmap;*/
	psta->init_rate = get_highest_rate_idx(tx_ra_bitmap) & 0x3f;
}

void update_sta_ra_info(_adapter *padapter, struct sta_info *psta)
{
	update_sta_trx_nss(padapter, psta);
	update_sta_smps_cap(padapter, psta);
	update_sta_rate_mask(padapter, psta);
}

void rtw_mlmeext_disconnect(_adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct recv_info *precvinfo = &padapter->recvinfo;
	u8 self_action = MLME_ACTION_UNKNOWN;
	u8 state_backup = (pmlmeinfo->state & 0x03);
	u8 ASIX_ID[] = {0x00, 0x0E, 0xC6};
	u8 *mac;
	struct sta_info *sta;
	struct _ADAPTER_LINK *padapter_link = NULL;
	u8 lidx;

	if (MLME_IS_AP(padapter))
		self_action = MLME_AP_STOPPED;
	else if (MLME_IS_MESH(padapter))
		self_action = MLME_MESH_STOPPED;
	else if (MLME_IS_STA(padapter))
		self_action = MLME_STA_DISCONNECTED;
	else if (MLME_IS_ADHOC(padapter) || MLME_IS_ADHOC_MASTER(padapter))
		self_action = MLME_ADHOC_STOPPED;
	else {
		RTW_INFO("state:0x%x\n", MLME_STATE(padapter));
		rtw_warn_on(1);
	}

	/* check if sta is ASIX peer and fix IOT issue if it is. */
	if (_rtw_memcmp(get_my_bssid(&pmlmeinfo->dev_network) , ASIX_ID , 3)) {
		u8 iot_flag = _FALSE;
		rtw_hal_set_hwreg(padapter, HW_VAR_ASIX_IOT, (u8 *)(&iot_flag));
	}
	pmlmeinfo->state = WIFI_FW_NULL_STATE;

	/* switch to the 20M Hz mode after disconnect */
	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
		padapter_link->mlmeextpriv.chandef.bw = CHANNEL_WIDTH_20;
		padapter_link->mlmeextpriv.chandef.offset = CHAN_OFFSET_NO_EXT;
	}

#ifdef CONFIG_CTRL_TXSS_BY_TP
	pmlmeext->txss_1ss = _FALSE;
#endif

#ifdef CONFIG_LAYER2_ROAMING
#define RTW_ROAM_DICONNECT_DELAY        20
	if (pmlmepriv->roam_network)
		rtw_msleep_os(RTW_ROAM_DICONNECT_DELAY);
#endif

#ifdef CONFIG_80211D
	if (self_action == MLME_STA_DISCONNECTED)
		rtw_leavebss_update_regulatory(padapter);
#endif

	for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
		padapter_link = GET_LINK(padapter, lidx);
		/* before chanctx_del */
		mac = padapter_link->mlmeextpriv.mlmext_info.network.MacAddress;
		sta = rtw_get_stainfo(&padapter->stapriv, mac);
		if (sta) {
			#ifdef CONFIG_DFS_MASTER
			if (!CHK_MLME_STATE(padapter, WIFI_AP_STATE | WIFI_MESH_STATE | WIFI_OP_CH_SWITCHING)) {
				/* radar detect status no need to check here for AP/MESH or iface under OPCH_SW */
				rtw_dfs_rd_en_dec_on_mlme_act(padapter, padapter_link, self_action, 0);
			}
			#endif
			rtw_hw_disconnect(padapter, sta);
		} else
			RTW_ERR(FUNC_ADPT_FMT ": can't find drv sta info for "
			MAC_FMT " !\n", FUNC_ADPT_ARG(padapter), MAC_ARG(mac));
	}

	cancel_link_timer(pmlmeext); /*_cancel_timer_ex(&pmlmeext->link_timer);*/

	/* pmlmepriv->LinkDetectInfo.TrafficBusyState = _FALSE; */
	pmlmepriv->LinkDetectInfo.TrafficTransitionCount = 0;
	pmlmepriv->LinkDetectInfo.LowPowerTransitionCount = 0;

#ifdef CONFIG_TDLS
	padapter->tdlsinfo.ap_prohibited = _FALSE;

	/* For TDLS channel switch, currently we only allow it to work in wifi logo test mode */
	if (padapter->registrypriv.wifi_spec == 1)
		padapter->tdlsinfo.ch_switch_prohibited = _FALSE;
#endif /* CONFIG_TDLS */

#ifdef PRIVATE_R
	pxmitpriv->tx_bk_drop_cnt = 0;
	pxmitpriv->tx_be_drop_cnt = 0;
	pxmitpriv->tx_vi_drop_cnt = 0;
	pxmitpriv->tx_vo_drop_cnt = 0;
	_rtw_memset(&(precvinfo->rx_vo_pkt_count_per_data_rate), 0, sizeof(precvinfo->rx_vo_pkt_count_per_data_rate));
	precvinfo->rx_vo_pkt_retry_count = 0;
#endif

#ifdef CONFIG_RTS_FULL_BW
	rtw_set_rts_bw(padapter);
#endif/*CONFIG_RTS_FULL_BW*/
}

/* Return 0 for success, otherwise fail. */
int rtw_set_hw_after_join(struct _ADAPTER *a, int join_res)
{
	struct mlme_priv *mlme = &a->mlmepriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(a);
	struct sta_info *sta;
	u8 *mac;
	int err;
	struct _ADAPTER_LINK *alink = NULL;
	u8 lidx;
	err = SUCCESS;


	if (mlme->wpa_phase == _TRUE)
		mlme->wpa_phase = _FALSE;

	if (join_res < 0) {
		err = rtw_hw_connect_abort(a);
		return err;
	}
#ifdef CONFIG_ARP_KEEP_ALIVE_GW
	mlme->bGetGateway = 1;
	mlme->GetGatewayTryCnt = 0;
#endif
	for (lidx = 0; lidx < a->adapter_link_num; lidx++) {
		alink = GET_LINK(a, lidx);
		if (alink->mlmepriv.is_accepted) {
			mac = (u8*)alink->mlmeextpriv.mlmext_info.network.MacAddress;
			sta = rtw_get_stainfo(&a->stapriv, mac);
			if (!sta) {
				RTW_ERR(FUNC_ADPT_FMT ": drv sta_info(" MAC_FMT ") not exist!\n",
					FUNC_ADPT_ARG(a), MAC_ARG(mac));
				return -1;
			}
			sta->phl_sta->wmode = alink->mlmeextpriv.cur_wireless_mode;
		} else {
			mac = (u8*)alink->mlmeextpriv.mlmext_info.network.MacAddress;
			sta = rtw_get_stainfo(&a->stapriv, mac);
			if (sta) /* Remove the unaccepted link sta */
				rtw_hw_connect_remove_sta(a, sta);
		}
	}
	err = rtw_hw_connected(a);

#ifndef CONFIG_STA_CMD_DISPR /* Run in MSG_EVT_CONNECT_END@PHL_FG_MDL_CONNECT */
	rtw_xmit_queue_clear(sta);
#endif /* CONFIG_STA_CMD_DISPR */

	/*reset counter for accumulated false alarm counter*/
	ATOMIC_SET((ATOMIC_T *)&dvobj->fa_cnt_acc, 0);

	return err;
}

/* currently only adhoc mode will go here */
void mlmeext_sta_add_event_callback(_adapter *padapter, struct sta_info *psta)
{
	struct _ADAPTER_LINK	*padapter_link = psta->padapter_link;
	struct link_mlme_ext_priv	*pmlmeext = &(padapter_link->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(padapter->mlmeextpriv.mlmext_info);
	u8	join_type;

	RTW_INFO("%s\n", __FUNCTION__);

	if ((pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) {
		if (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) { /* adhoc master or sta_count>1 */
			/* nothing to do */
		} else { /* adhoc client */

			/* start beacon */
			if (send_beacon(padapter) == _FAIL)
				rtw_warn_on(1);

			pmlmeinfo->state |= WIFI_FW_ASSOC_SUCCESS;
		}

		join_type = 2;
		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_JOIN, (u8 *)(&join_type));
	}

	/* update adhoc sta_info */
	update_sta_info(padapter, psta);

	update_sta_ra_info(padapter, psta);

	/* ToDo: HT for Ad-hoc */
	psta->phl_sta->wmode = rtw_check_network_type(psta->bssrateset, psta->bssratelen, pmlmeext->chandef.chan);
}

void mlmeext_sta_del_event_callback(_adapter *padapter)
{
	if (is_client_associated_to_ap(padapter) || is_IBSS_empty(padapter))
		rtw_mlmeext_disconnect(padapter);
}

/****************************************************************************

Following are the functions for the timer handlers

*****************************************************************************/
void _linked_info_dump(_adapter *padapter)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	if (padapter->bLinkInfoDump) {
		rtw_hal_get_def_var(padapter, padapter_link, HW_DEF_RA_INFO_DUMP, RTW_DBGDUMP);
		/*rtw_hal_set_phydm_var(padapter, HAL_PHYDM_RX_INFO_DUMP, RTW_DBGDUMP, _FALSE);*/
	}
}
/********************************************************************

When station does not receive any packet in MAX_CONTINUAL_NORXPACKET_COUNT*2 seconds,
recipient station will teardown the block ack by issuing DELBA frame.

*********************************************************************/
void rtw_delba_check(_adapter *padapter, struct sta_info *psta, u8 from_timer)
{
	int	i = 0;
	int ret = _SUCCESS;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	/*
		IOT issue,occur Broadcom ap(Buffalo WZR-D1800H,Netgear R6300).
		AP is originator.AP does not transmit unicast packets when STA response its BAR.
		This case probably occur ap issue BAR after AP builds BA.

		Follow 802.11 spec, STA shall maintain an inactivity timer for every negotiated Block Ack setup.
		The inactivity timer is not reset when MPDUs corresponding to other TIDs are received.
	*/
	if (pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_BROADCOM) {
		for (i = 0; i < TID_NUM ; i++) {
			if ((psta->recvreorder_ctrl[i].enable) && 
                        (sta_rx_data_qos_pkts(psta, i) == sta_last_rx_data_qos_pkts(psta, i)) ) {			
					if (_TRUE == rtw_inc_and_chk_continual_no_rx_packet(psta, i)) {					
						/* send a DELBA frame to the peer STA with the Reason Code field set to TIMEOUT */
						if (!from_timer)
							ret = issue_del_ba_ex(padapter, psta->phl_sta->mac_addr, i, 39, 0, 3, 1);
						else
							issue_del_ba(padapter,  psta->phl_sta->mac_addr, i, 39, 0);
						psta->recvreorder_ctrl[i].enable = _FALSE;
						if (ret != _FAIL)
							psta->recvreorder_ctrl[i].ampdu_size = RX_AMPDU_SIZE_INVALID;
						rtw_reset_continual_no_rx_packet(psta, i);
					}				
			} else {
				/* The inactivity timer is reset when MPDUs to the TID is received. */
				rtw_reset_continual_no_rx_packet(psta, i);
			}
		}
	}
}

u8 chk_ap_is_alive(_adapter *padapter, struct sta_info *psta)
{
	u8 ret = _FALSE;
#ifdef DBG_EXPIRATION_CHK
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	RTW_INFO(FUNC_ADPT_FMT" rx:"STA_PKTS_FMT", beacon:%llu, probersp_to_self:%llu"
		/*", probersp_bm:%llu, probersp_uo:%llu, probereq:%llu, BI:%u"*/
		 ", retry:%u\n"
		 , FUNC_ADPT_ARG(padapter)
		 , STA_RX_PKTS_DIFF_ARG(psta)
		, psta->sta_stats.rx_beacon_pkts - psta->sta_stats.last_rx_beacon_pkts
		, psta->sta_stats.rx_probersp_pkts - psta->sta_stats.last_rx_probersp_pkts
		/*, psta->sta_stats.rx_probersp_bm_pkts - psta->sta_stats.last_rx_probersp_bm_pkts
		, psta->sta_stats.rx_probersp_uo_pkts - psta->sta_stats.last_rx_probersp_uo_pkts
		, psta->sta_stats.rx_probereq_pkts - psta->sta_stats.last_rx_probereq_pkts
		 , pmlmeinfo->bcn_interval*/
		 , pmlmeext->retry
		);

	RTW_INFO(FUNC_ADPT_FMT" tx_pkts:%llu, link_count:%u\n", FUNC_ADPT_ARG(padapter)
		 , sta_tx_pkts(psta)
		 , pmlmeinfo->link_count
		);
#endif

	if ((sta_rx_data_pkts(psta) == sta_last_rx_data_pkts(psta))
	    && sta_rx_beacon_pkts(psta) == sta_last_rx_beacon_pkts(psta)
	    && sta_rx_probersp_pkts(psta) == sta_last_rx_probersp_pkts(psta)
	   )
		ret = _FALSE;
	else
		ret = _TRUE;

	sta_update_last_rx_pkts(psta);

	return ret;
}

u8 chk_adhoc_peer_is_alive(struct sta_info *psta)
{
	u8 ret = _TRUE;

#ifdef DBG_EXPIRATION_CHK
	RTW_INFO("sta:"MAC_FMT", rssi:%d, rx:"STA_PKTS_FMT", beacon:%llu, probersp_to_self:%llu"
		/*", probersp_bm:%llu, probersp_uo:%llu, probereq:%llu, BI:%u"*/
		 ", expire_to:%u\n"
		 , MAC_ARG(psta->phl_sta->mac_addr)
		 , 0 /* TODO: psta->phl_sta->hal_sta->rssi_stat.rssi */
		 , STA_RX_PKTS_DIFF_ARG(psta)
		, psta->sta_stats.rx_beacon_pkts - psta->sta_stats.last_rx_beacon_pkts
		, psta->sta_stats.rx_probersp_pkts - psta->sta_stats.last_rx_probersp_pkts
		/*, psta->sta_stats.rx_probersp_bm_pkts - psta->sta_stats.last_rx_probersp_bm_pkts
		, psta->sta_stats.rx_probersp_uo_pkts - psta->sta_stats.last_rx_probersp_uo_pkts
		, psta->sta_stats.rx_probereq_pkts - psta->sta_stats.last_rx_probereq_pkts
		 , pmlmeinfo->bcn_interval*/
		 , psta->expire_to
		);
#endif

	if (sta_rx_data_pkts(psta) == sta_last_rx_data_pkts(psta)
	    && sta_rx_beacon_pkts(psta) == sta_last_rx_beacon_pkts(psta)
	    && sta_rx_probersp_pkts(psta) == sta_last_rx_probersp_pkts(psta))
		ret = _FALSE;

	sta_update_last_rx_pkts(psta);

	return ret;
}

#ifdef CONFIG_TDLS
u8 chk_tdls_peer_sta_is_alive(_adapter *padapter, struct sta_info *psta)
{
	if ((psta->sta_stats.rx_data_pkts == psta->sta_stats.last_rx_data_pkts)
	    && (psta->sta_stats.rx_tdls_disc_rsp_pkts == psta->sta_stats.last_rx_tdls_disc_rsp_pkts))
		return _FALSE;

	return _TRUE;
}

void linked_status_chk_tdls(_adapter *padapter)
{
	struct candidate_pool {
		struct sta_info *psta;
		u8 addr[ETH_ALEN];
	};
	struct sta_priv *pstapriv = &padapter->stapriv;
	u8 ack_chk;
	struct sta_info *psta;
	int i, num_teardown = 0, num_checkalive = 0;
	_list	*plist, *phead;
	struct tdls_txmgmt txmgmt;
	struct candidate_pool checkalive[MAX_ALLOWED_TDLS_STA_NUM];
	struct candidate_pool teardown[MAX_ALLOWED_TDLS_STA_NUM];
	u8 tdls_sta_max = _FALSE;

#define ALIVE_MIN 2
#define ALIVE_MAX 5

	_rtw_memset(&txmgmt, 0x00, sizeof(struct tdls_txmgmt));
	_rtw_memset(checkalive, 0x00, sizeof(checkalive));
	_rtw_memset(teardown, 0x00, sizeof(teardown));

	if ((padapter->tdlsinfo.link_established == _TRUE)) {
		_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
		for (i = 0; i < NUM_STA; i++) {
			phead = &(pstapriv->sta_hash[i]);
			plist = get_next(phead);

			while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
				psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
				plist = get_next(plist);

				if (psta->tdls_sta_state & TDLS_LINKED_STATE) {
					psta->alive_count++;
					if (psta->alive_count >= ALIVE_MIN) {
						if (chk_tdls_peer_sta_is_alive(padapter, psta) == _FALSE) {
							if (psta->alive_count < ALIVE_MAX) {
								_rtw_memcpy(checkalive[num_checkalive].addr, psta->phl_sta->mac_addr, ETH_ALEN);
								checkalive[num_checkalive].psta = psta;
								num_checkalive++;
							} else {
								_rtw_memcpy(teardown[num_teardown].addr, psta->phl_sta->mac_addr, ETH_ALEN);
								teardown[num_teardown].psta = psta;
								num_teardown++;
							}
						} else
							psta->alive_count = 0;
					}
					psta->sta_stats.last_rx_data_pkts = psta->sta_stats.rx_data_pkts;
					psta->sta_stats.last_rx_tdls_disc_rsp_pkts = psta->sta_stats.rx_tdls_disc_rsp_pkts;

					if ((num_checkalive >= MAX_ALLOWED_TDLS_STA_NUM) || (num_teardown >= MAX_ALLOWED_TDLS_STA_NUM)) {
						tdls_sta_max = _TRUE;
						break;
					}
				}
			}

			if (tdls_sta_max == _TRUE)
				break;
		}
		_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

		if (num_checkalive > 0) {
			for (i = 0; i < num_checkalive; i++) {
				_rtw_memcpy(txmgmt.peer, checkalive[i].addr, ETH_ALEN);
				issue_tdls_dis_req(padapter, &txmgmt);
				issue_tdls_dis_req(padapter, &txmgmt);
				issue_tdls_dis_req(padapter, &txmgmt);
			}
		}

		if (num_teardown > 0) {
			for (i = 0; i < num_teardown; i++) {
				RTW_INFO("[%s %d] Send teardown to "MAC_FMT"\n", __FUNCTION__, __LINE__, MAC_ARG(teardown[i].addr));
				txmgmt.status_code = _RSON_TDLS_TEAR_TOOFAR_;
				_rtw_memcpy(txmgmt.peer, teardown[i].addr, ETH_ALEN);
				issue_tdls_teardown(padapter, &txmgmt, _FALSE);
			}
		}
	}

}
#endif /* CONFIG_TDLS */

inline int rtw_get_rx_chk_limit(_adapter *adapter)
{
	return adapter->stapriv.rx_chk_limit;
}

inline void rtw_set_rx_chk_limit(_adapter *adapter, int limit)
{
	adapter->stapriv.rx_chk_limit = limit;
}

/* from_timer == 1 means driver is in LPS */
void linked_status_chk(_adapter *padapter, u8 from_timer)
{
	u32	i;
	struct sta_info		*psta;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct sta_priv		*pstapriv = &padapter->stapriv;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	bool is_p2p_gc = _FALSE;
	/* ToDo CONFIG_RTW_MLD: check retry per link or per MLD? */
	struct _ADAPTER_LINK *padapter_link;
	struct link_mlme_ext_info *lmlmeinfo;
	u8 lidx;

#ifdef CONFIG_RTW_WDS
	rtw_wds_gptr_expire(padapter);
#endif

	if (padapter->registrypriv.mp_mode == _TRUE)
		return;

	if (check_fwstate(pmlmepriv, WIFI_CSA_UPDATE_BEACON))
		return;

	if (is_client_associated_to_ap(padapter)) {
		/* linked infrastructure client mode */

		int tx_chk = _SUCCESS, rx_chk = _SUCCESS;
		int rx_chk_limit;
		int link_count_limit;

		rx_chk_limit = rtw_get_rx_chk_limit(padapter);

#ifdef CONFIG_ARP_KEEP_ALIVE_GW
		if (!from_timer && pmlmepriv->bGetGateway == 1 && pmlmepriv->GetGatewayTryCnt < 3) {
			RTW_INFO("do rtw_gw_addr_query() : %d\n", pmlmepriv->GetGatewayTryCnt);
			pmlmepriv->GetGatewayTryCnt++;
			if (rtw_gw_addr_query(padapter) == 0)
				pmlmepriv->bGetGateway = 0;
			else {
				_rtw_memset(pmlmepriv->gw_ip, 0, 4);
				_rtw_memset(pmlmepriv->gw_mac_addr, 0, ETH_ALEN);
			}
		}
#endif
#ifdef CONFIG_P2P
		is_p2p_gc = rtw_p2p_chk_role(&padapter->wdinfo, P2P_ROLE_CLIENT);
		if (is_p2p_gc) {
			if (!from_timer)
				link_count_limit = 3; /* 8 sec */
			else
				link_count_limit = 15; /* 32 sec */
		} else
#endif /* CONFIG_P2P */
		{
			if (!from_timer)
				link_count_limit = 7; /* 16 sec */
			else
				link_count_limit = 29; /* 60 sec */
		}

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
		if (ATOMIC_READ(&padapter->tdlsinfo.chsw_info.chsw_on) == _TRUE)
			return;
#endif /* CONFIG_TDLS_CH_SW */

#ifdef CONFIG_TDLS_AUTOCHECKALIVE
		linked_status_chk_tdls(padapter);
#endif /* CONFIG_TDLS_AUTOCHECKALIVE */
#endif /* CONFIG_TDLS */


		for (lidx = 0; lidx < padapter->adapter_link_num; lidx++) {
			padapter_link = GET_LINK(padapter, lidx);
			if (!padapter_link->mlmepriv.is_accepted)
				continue;
			lmlmeinfo = &padapter_link->mlmeextpriv.mlmext_info;
			psta = rtw_get_stainfo(pstapriv, lmlmeinfo->network.MacAddress);
			if (psta != NULL) {
#ifdef CONFIG_ISSUE_DELBA_WHEN_NO_TRAFFIC
				/*issue delba when ap does not tx data packet that is Broadcom ap */
				rtw_delba_check(padapter, psta, from_timer);
#endif
				if (chk_ap_is_alive(padapter, psta) == _FALSE)
					rx_chk = _FAIL;

				if (sta_last_tx_pkts(psta) == sta_tx_pkts(psta))
					tx_chk = _FAIL;

#ifdef CONFIG_ACTIVE_KEEP_ALIVE_CHECK
				if (!from_timer && pmlmeext->active_keep_alive_check && (rx_chk == _FAIL || tx_chk == _FAIL)) {
					u8 switch_channel_by_drv = _TRUE;
					struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
					struct rtw_chan_def hw_chdef = {0};
					struct rtw_chan_def union_chdef = {0};
					struct rtw_chan_def bk_chdef = {0};

					if (switch_channel_by_drv) {
						if (rtw_phl_mr_get_chandef(dvobj->phl, padapter->phl_role,
									padapter_link->wrlink, &union_chdef)
										!= RTW_PHL_STATUS_SUCCESS) {
							RTW_ERR("fun:%s line:%d get chandef failed\n", __FUNCTION__, __LINE__);
							rtw_warn_on(1);
						}
						if (padapter_link->mlmeextpriv.chandef.chan != union_chdef.chan)
							goto bypass_active_keep_alive;

						/* switch to correct channel of current network  before issue keep-alive frames */
						rtw_get_oper_chdef(padapter, padapter_link, &hw_chdef);
						if (hw_chdef.chan != padapter_link->mlmeextpriv.chandef.chan) {
							_rtw_memcpy(&bk_chdef, &hw_chdef, sizeof(struct rtw_chan_def));
							set_bch_bwmode(padapter, padapter_link, union_chdef.band,
									 union_chdef.chan, union_chdef.offset,
								    	 union_chdef.bw, _FALSE);
						}
					}

					if (rx_chk != _SUCCESS)
						issue_probereq_ex(padapter, padapter_link, &lmlmeinfo->network.Ssid, psta->phl_sta->mac_addr, 0, 0, 3, 1);

					if ((tx_chk != _SUCCESS && pmlmeinfo->link_count++ == link_count_limit) || rx_chk != _SUCCESS) {
						if (rtw_mi_check_fwstate(padapter, WIFI_UNDER_SURVEY))
							tx_chk = issue_nulldata(padapter, padapter_link, psta->phl_sta->mac_addr, 1, 3, 1);
						else
							tx_chk = issue_nulldata(padapter, padapter_link, psta->phl_sta->mac_addr, 0, 3, 1);
						/* If tx acked, set rx_chk _SUCCESS to reset the retry count
						 * unless it is a P2P GC. If the P2P GO is no longer a P2P GO,
						 * P2P GC tx may still receive acks, but no beacon.
						 */
						if (tx_chk == _SUCCESS && !is_p2p_gc)
							rx_chk = _SUCCESS;
					}

					/* back to the original operation channel */
					if (bk_chdef.chan > 0 && switch_channel_by_drv)
						set_bch_bwmode(padapter, padapter_link,
								bk_chdef.band, bk_chdef.chan, bk_chdef.offset, bk_chdef.bw, _FALSE);

bypass_active_keep_alive:
					;
				} else
#endif /* CONFIG_ACTIVE_KEEP_ALIVE_CHECK */
				{
					if (rx_chk != _SUCCESS) {
						if (pmlmeext->retry == 0) {
#ifdef DBG_EXPIRATION_CHK
							RTW_INFO("issue_probereq to trigger probersp, retry=%d\n", pmlmeext->retry);
#endif
							issue_probereq_ex(padapter, padapter_link, &lmlmeinfo->network.Ssid,
								lmlmeinfo->network.MacAddress, 0, 0, 0, (from_timer ? 0 : 1));
							issue_probereq_ex(padapter, padapter_link, &lmlmeinfo->network.Ssid,
								lmlmeinfo->network.MacAddress, 0, 0, 0, (from_timer ? 0 : 1));
							issue_probereq_ex(padapter, padapter_link, &lmlmeinfo->network.Ssid,
								lmlmeinfo->network.MacAddress, 0, 0, 0, (from_timer ? 0 : 1));
						}
					}

					if (tx_chk != _SUCCESS && pmlmeinfo->link_count++ == link_count_limit) {
						#ifdef DBG_EXPIRATION_CHK
						RTW_INFO("%s issue_nulldata(%d)\n", __FUNCTION__, from_timer ? 1 : 0);
						#endif
						if (from_timer || rtw_mi_check_fwstate(padapter, WIFI_UNDER_SURVEY))
							tx_chk = issue_nulldata(padapter, padapter_link, NULL, 1, 0, 0);
						else
							tx_chk = issue_nulldata(padapter, padapter_link, NULL, 0, 1, 1);
					}
				}

				if (rx_chk == _FAIL) {
					pmlmeext->retry++;
					if (pmlmeext->retry > rx_chk_limit) {
						RTW_PRINT(FUNC_ADPT_FMT" disconnect or roaming\n",
							FUNC_ADPT_ARG(padapter));
						receive_disconnect(padapter,
							lmlmeinfo->network.MacAddress,
							WLAN_REASON_EXPIRATION_CHK, _FALSE);
						pmlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
						pmlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_KEEPALIVE_TIMEOUT;
						pmlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
						return;
					}
				} else
					pmlmeext->retry = 0;

				if (tx_chk == _FAIL)
					pmlmeinfo->link_count %= (link_count_limit + 1);
				else {
					psta->sta_stats.last_tx_pkts = psta->sta_stats.tx_pkts;
					pmlmeinfo->link_count = 0;
				}

			} /* end of if ((psta = rtw_get_stainfo(pstapriv, passoc_res->network.MacAddress)) != NULL) */
		}
	} else if (is_client_associated_to_ibss(padapter)) {
		_list *phead, *plist, dlist;

		_rtw_init_listhead(&dlist);

		_rtw_spinlock_bh(&pstapriv->sta_hash_lock);

		for (i = 0; i < NUM_STA; i++) {

			phead = &(pstapriv->sta_hash[i]);
			plist = get_next(phead);
			while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
				psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
				plist = get_next(plist);

				if (is_broadcast_mac_addr(psta->phl_sta->mac_addr))
					continue;

				if (chk_adhoc_peer_is_alive(psta) || !psta->expire_to)
					psta->expire_to = pstapriv->adhoc_expire_to;
				else
					psta->expire_to--;

				if (psta->expire_to <= 0) {
					rtw_list_delete(&psta->list);
					rtw_list_insert_tail(&psta->list, &dlist);
				}
			}
		}

		_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

		plist = get_next(&dlist);
		while (rtw_end_of_queue_search(&dlist, plist) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, list);
			plist = get_next(plist);
			rtw_list_delete(&psta->list);
			RTW_INFO(FUNC_ADPT_FMT" ibss expire "MAC_FMT"\n"
				, FUNC_ADPT_ARG(padapter), MAC_ARG(psta->phl_sta->mac_addr));
			report_del_sta_event(padapter, psta->phl_sta->mac_addr, WLAN_REASON_EXPIRATION_CHK, from_timer ? _TRUE : _FALSE, _FALSE);
		}
	}

}

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
void rtw_tbtx_xmit_timer_hdl(void *ctx)
{
	_adapter *padapter = (_adapter *)ctx;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;

	if (ATOMIC_READ(&padapter->tbtx_remove_tx_pause) == _TRUE){
		ATOMIC_SET(&padapter->tbtx_tx_pause, _FALSE);
		rtw_tx_control_cmd(padapter);
	}else {
		rtw_issue_action_token_rel(padapter);
		ATOMIC_SET(&padapter->tbtx_tx_pause, _TRUE);
		rtw_tx_control_cmd(padapter);
		_set_timer(&pmlmeext->tbtx_xmit_timer, MAX_TXPAUSE_DURATION);
		ATOMIC_SET(&padapter->tbtx_remove_tx_pause, _TRUE);
	}
}

#ifdef CONFIG_AP_MODE
void rtw_tbtx_token_dispatch_timer_hdl(void *ctx)
{
	_adapter *padapter = (_adapter *)ctx;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	_irqL irqL;
	struct sta_info *psta = NULL;
	struct sta_priv *pstapriv = &padapter->stapriv;
	_list *phead, *plist;
	int i, found = _FALSE;
	u8 nr_send, th_idx = 0;

	_rtw_spinlock_bh(&pstapriv->asoc_list_lock, &irqL);
	RTW_DBG("%s:asoc_cnt: %d\n",__func__, pstapriv->tbtx_asoc_list_cnt);

	// check number of TBTX sta
	if (padapter->stapriv.tbtx_asoc_list_cnt < 2)
		goto exit;

	// dispatch token
	
	nr_send = RTW_DIV_ROUND_UP(pstapriv->tbtx_asoc_list_cnt, NR_TBTX_SLOT);

	phead = &pstapriv->asoc_list;
	plist = get_next(phead);
	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		/* psta is supporting TBTX */
		if ((!psta) || (!psta->tbtx_enable))
			RTW_DBG("sta tbtx_enable is false\n");
		else {
			for (i = 0; i < nr_send; i++) {
				if (pstapriv->last_token_holder == psta) {
					found = _TRUE;
					goto outof_loop;
				}
			}
		}
		plist = get_next(plist);
	}
outof_loop:
	
	RTW_DBG("rtw_tbtx_token_dispatch_timer_hdl()   th_idx=%d,  nr_send=%d, phead=%p, plist=%p, found=%d\n ", th_idx ,  nr_send, phead, plist, found);
	if (!found) {
		plist = get_next(phead);
		while(rtw_end_of_queue_search(phead, plist) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
			if ((!psta) || (!psta->tbtx_enable))
				RTW_DBG("sta tbtx_enable is false\n");
			else {
					pstapriv->token_holder[th_idx] = psta;
					rtw_issue_action_token_req(padapter, pstapriv->token_holder[th_idx++]);
					break;
			}
			plist = get_next(plist);
		}
	}

	for (i=th_idx; i<nr_send;) {
		plist = get_next(plist);
		if (plist == phead)
			plist = get_next(plist);
		psta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		if ((!psta) || (!psta->tbtx_enable))
			RTW_DBG("sta tbtx_enable is false\n");
		else {
			pstapriv->token_holder[th_idx] = psta;
			rtw_issue_action_token_req(padapter, pstapriv->token_holder[th_idx++]);
			i++;
		}
	}
	ATOMIC_SET(&pstapriv->nr_token_keeper, nr_send);
	

exit:
	// set_timer
	_rtw_spinunlock_bh(&pstapriv->asoc_list_lock, &irqL);
	_set_timer(&pmlmeext->tbtx_token_dispatch_timer, TBTX_TX_DURATION); 
}
#endif /* CONFIG_AP_MODE */
#endif /* CONFIG_RTW_TOKEN_BASED_XMIT */

void link_timer_hdl(void *ctx)
{
	_adapter *padapter = (_adapter *)ctx;
	/* static unsigned int		rx_pkt = 0; */
	/* static u64				tx_cnt = 0; */
	/* struct xmit_priv		*pxmitpriv = &(padapter->xmitpriv); */
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	/* struct sta_priv		*pstapriv = &padapter->stapriv; */
#ifdef CONFIG_RTW_80211R
	struct	sta_priv		*pstapriv = &padapter->stapriv;
	struct	sta_info		*psta = NULL;
#endif

	if (rtw_sta_linking_test_force_fail())
		RTW_INFO("rtw_sta_linking_test_force_fail\n");

	if (pmlmeext->join_abort && pmlmeinfo->state != WIFI_FW_NULL_STATE) {
		RTW_INFO(FUNC_ADPT_FMT" join abort\n", FUNC_ADPT_ARG(padapter));
		pmlmeinfo->state = WIFI_FW_NULL_STATE;
		report_join_res(padapter, -4, WLAN_STATUS_UNSPECIFIED_FAILURE);
		goto exit;
	}

	if (pmlmeinfo->state & WIFI_FW_AUTH_NULL) {
		RTW_INFO("link_timer_hdl:no beacon while connecting\n");
		pmlmeinfo->state = WIFI_FW_NULL_STATE;
		report_join_res(padapter, -3, WLAN_STATUS_UNSPECIFIED_FAILURE);
	} else if (pmlmeinfo->state & WIFI_FW_AUTH_STATE) {

#ifdef CONFIG_IOCTL_CFG80211
		if (rtw_sec_chk_auth_type(padapter, MLME_AUTHTYPE_SAE))
			return;
#endif /* CONFIG_IOCTL_CFG80211 */

		/* re-auth timer */
		if (++pmlmeinfo->reauth_count > REAUTH_LIMIT) {
			/* if (pmlmeinfo->auth_algo != dot11AuthAlgrthm_Auto) */
			/* { */
			pmlmeinfo->state = 0;
			if (pmlmeinfo->auth_status) {
				report_join_res(padapter, -1, pmlmeinfo->auth_status);
				pmlmeinfo->auth_status = 0; /* reset */
			} else
				report_join_res(padapter, -1, WLAN_STATUS_UNSPECIFIED_FAILURE);
			return;
			/* } */
			/* else */
			/* { */
			/*	pmlmeinfo->auth_algo = dot11AuthAlgrthm_Shared; */
			/*	pmlmeinfo->reauth_count = 0; */
			/* } */
		}

		RTW_INFO("link_timer_hdl: auth timeout and try again\n");
		pmlmeinfo->auth_seq = 1;
		issue_auth(padapter, NULL, 0);
		set_link_timer(pmlmeext, REAUTH_TO);
	} else if (pmlmeinfo->state & WIFI_FW_ASSOC_STATE) {
		/* re-assoc timer */
		if (++pmlmeinfo->reassoc_count > REASSOC_LIMIT) {
			pmlmeinfo->state = WIFI_FW_NULL_STATE;
#ifdef CONFIG_RTW_80211R
			if (rtw_ft_roam(padapter)) {
				psta = rtw_get_stainfo(pstapriv, pmlmeinfo->dev_network.MacAddress);
				if (psta)
					rtw_free_mld_stainfo(padapter, psta->phl_sta->mld);
			}
#endif
			report_join_res(padapter, -2, WLAN_STATUS_UNSPECIFIED_FAILURE);
			return;
		}

#ifdef CONFIG_RTW_80211R
		if (rtw_ft_roam(padapter)) {
			RTW_INFO("link_timer_hdl: reassoc timeout and try again\n");
			issue_reassocreq(padapter);
		} else
#endif
		{
			RTW_INFO("link_timer_hdl: assoc timeout and try again\n");
			issue_assocreq(padapter);
		}

		set_link_timer(pmlmeext, REASSOC_TO);
	}

exit:
	return;
}

void addba_timer_hdl(void *ctx)
{
	struct sta_info *psta = (struct sta_info *)ctx;

#ifdef CONFIG_80211N_HT
	struct ht_priv	*phtpriv;
	struct ampdu_priv *ampdu_priv;

	if (!psta)
		return;

	phtpriv = &psta->htpriv;
	ampdu_priv = &psta->ampdu_priv;

	if ((phtpriv->ht_option == _TRUE
#if CONFIG_IEEE80211_BAND_6GHZ
	    || psta->phl_sta->chandef.band == BAND_ON_6G
#endif
	    ) && ampdu_priv->ampdu_enable == _TRUE) {
		if (ampdu_priv->candidate_tid_bitmap)
			ampdu_priv->candidate_tid_bitmap = 0x0;

	}
#endif /* CONFIG_80211N_HT */
}

#ifdef CONFIG_IEEE80211W
void report_sta_timeout_event(_adapter *padapter, u8 *MacAddr, unsigned short reason)
{
	struct cmd_obj *pcmd_obj;
	u8 *pevtcmd;
	u32 cmdsz;
	struct sta_info *psta;
	int	mac_id;
	struct stadel_event *pdel_sta_evt;
	struct rtw_evt_header *evt_hdr;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;

	pcmd_obj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmd_obj == NULL)
		return;
	pcmd_obj->padapter = padapter;

	cmdsz = (sizeof(struct stadel_event) + sizeof(struct rtw_evt_header));
	pevtcmd = (u8 *)rtw_zmalloc(cmdsz);
	if (pevtcmd == NULL) {
		rtw_mfree((u8 *)pcmd_obj, sizeof(struct cmd_obj));
		return;
	}

	_rtw_init_listhead(&pcmd_obj->list);

	CMD_OBJ_SET_HWBAND(pcmd_obj, HW_BAND_MAX);
	pcmd_obj->cmdcode = CMD_SET_MLME_EVT;
	pcmd_obj->cmdsz = cmdsz;
	pcmd_obj->parmbuf = pevtcmd;

	pcmd_obj->rsp = NULL;
	pcmd_obj->rspsz  = 0;

	evt_hdr = (struct rtw_evt_header *)(pevtcmd);
	evt_hdr->len = sizeof(struct stadel_event);
	evt_hdr->id = EVT_TIMEOUT_STA;
	evt_hdr->seq = ATOMIC_INC_RETURN(&pmlmeext->event_seq);

	pdel_sta_evt = (struct stadel_event *)(pevtcmd + sizeof(struct rtw_evt_header));
	_rtw_memcpy((unsigned char *)(&(pdel_sta_evt->macaddr)), MacAddr, ETH_ALEN);
	_rtw_memcpy((unsigned char *)(pdel_sta_evt->rsvd), (unsigned char *)(&reason), 2);


	psta = rtw_get_stainfo(&padapter->stapriv, MacAddr);
	if (psta)
		mac_id = (int)psta->phl_sta->macid;
	else
		mac_id = (-1);

	pdel_sta_evt->mac_id = mac_id;

	RTW_INFO("report_del_sta_event: delete STA, mac_id=%d\n", mac_id);

	rtw_enqueue_cmd(pcmdpriv, pcmd_obj);

	return;
}

void clnt_sa_query_timeout(_adapter *padapter)
{
	struct mlme_ext_priv *mlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *mlmeinfo = &(mlmeext->mlmext_info);

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));
	receive_disconnect(padapter, get_my_bssid(&(mlmeinfo->dev_network)), WLAN_REASON_SA_QUERY_TIMEOUT, _FALSE);

	mlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
	mlmeinfo->disconnect_code = DISCONNECTION_BY_DRIVER_DUE_TO_SA_QUERY_TIMEOUT;
	mlmeinfo->wifi_reason_code = WLAN_REASON_UNSPECIFIED;
}

void sa_query_timer_hdl(void *ctx)
{
	struct sta_info *psta = (struct sta_info *)ctx;
	_adapter *padapter = psta->padapter;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	if (MLME_IS_STA(padapter) &&
	    check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
		clnt_sa_query_timeout(padapter);
	else if (check_fwstate(pmlmepriv, WIFI_AP_STATE) == _TRUE)
		report_sta_timeout_event(padapter, psta->phl_sta->mac_addr, WLAN_REASON_PREV_AUTH_NOT_VALID);
}

#endif /* CONFIG_IEEE80211W */

#ifdef PRIVATE_R
void find_remote_timer_hdl(void *ctx)
{
	_adapter *padapter = (_adapter *)ctx;
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	
	rtw_find_remote_wk_cmd(padapter);
	set_find_remote_timer(pmlmeext, 1);
}

#ifdef CONFIG_P2P
void hide_ssid_hdl(void *ctx)
{
	_adapter *padapter = (_adapter *)ctx;
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	rtw_set_bit(MIRACAST, &pregpriv->go_hidden_ssid_mode);
	ATOMIC_SET(&pregpriv->set_hide_ssid_timer, 0);

	/* check if there is other feature show SSID in beacon */
	if (pregpriv->go_hidden_ssid_mode == ALL_HIDE_SSID)
		rtw_hide_ssid_wk_cmd(padapter);
}
#endif
#endif

#ifdef CONFIG_ECSA_PHL
void csa_wait_bcn_timer_hdl(void *ctx)
{
	_adapter *padapter = (_adapter *)ctx;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	_adapter *iface;
	u8 i;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface)
			continue;
		/* Wait beacon timeout, so clear fwstate WIFI_CSA_UPDATE_BEACON */
		if (check_fwstate(&(iface->mlmepriv), WIFI_CSA_UPDATE_BEACON)) {
			clr_fwstate(&(iface->mlmepriv), WIFI_CSA_UPDATE_BEACON);
			RTW_INFO(FUNC_ADPT_FMT" wait beacons more than 70 seconds\n", FUNC_ADPT_ARG(iface));
			return;
		}
	}
}
#endif /* CONFIG_ECSA_PHL */

#ifdef CONFIG_AUTO_AP_MODE
void rtw_auto_ap_rx_msg_dump(_adapter *padapter, union recv_frame *precv_frame, u8 *ehdr_pos)
{
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct sta_info *psta = precv_frame->u.hdr.psta;
	struct ethhdr *ehdr = (struct ethhdr *)ehdr_pos;

	RTW_INFO("eth rx: got eth_type=0x%x\n", ntohs(ehdr->h_proto));

	if (psta && psta->isrc && psta->pid > 0) {
		u16 rx_pid;

		rx_pid = *(u16 *)(ehdr_pos + ETH_HLEN);

		RTW_INFO("eth rx(pid=0x%x): sta("MAC_FMT") pid=0x%x\n",
			 rx_pid, MAC_ARG(psta->phl_sta->mac_addr), psta->pid);

		if (rx_pid == psta->pid) {
			int i;
			u16 len = *(u16 *)(ehdr_pos + ETH_HLEN + 2);
			/* u16 ctrl_type = *(u16 *)(ehdr_pos + ETH_HLEN + 4); */

			/* RTW_INFO("eth, RC: len=0x%x, ctrl_type=0x%x\n", len, ctrl_type);  */
			RTW_INFO("eth, RC: len=0x%x\n", len);

			for (i = 0; i < len; i++)
				RTW_INFO("0x%x\n", *(ehdr_pos + ETH_HLEN + 4 + i));
			/* RTW_INFO("0x%x\n", *(ehdr_pos + ETH_HLEN + 6 + i)); */

			RTW_INFO("eth, RC-end\n");
		}
	}

}

void rtw_start_auto_ap(_adapter *adapter)
{
	RTW_INFO("%s\n", __FUNCTION__);

	rtw_set_802_11_infrastructure_mode(adapter, Ndis802_11APMode, 0);

	rtw_setopmode_cmd(adapter, Ndis802_11APMode, RTW_CMDF_WAIT_ACK);
}

static int rtw_auto_ap_start_beacon(_adapter *adapter)
{
	int ret = 0;
	u8 *pbuf = NULL;
	uint len;
	u8	supportRate[16];
	int	sz = 0, rateLen;
	u8	*ie;
	u8	wireless_mode, oper_channel;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct rtw_chan_def u_chdef = {0};
	u8 ssid[3] = {0}; /* hidden ssid */
	u32 ssid_len = sizeof(ssid);
	struct mlme_priv *pmlmepriv = &(adapter->mlmepriv);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adpater);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE) != _TRUE)
		return -EINVAL;


	len = 128;
	pbuf = rtw_zmalloc(len);
	if (!pbuf)
		return -ENOMEM;


	/* generate beacon */
	ie = pbuf;

	/* timestamp will be inserted by hardware */
	sz += 8;
	ie += sz;

	/* beacon interval : 2bytes */
	*(u16 *)ie = cpu_to_le16((u16)100); /* BCN_INTERVAL=100; */
	sz += 2;
	ie += 2;

	/* capability info */
	*(u16 *)ie = 0;
	*(u16 *)ie |= cpu_to_le16(cap_ESS);
	*(u16 *)ie |= cpu_to_le16(cap_ShortPremble);
	/* *(u16*)ie |= cpu_to_le16(cap_Privacy); */
	sz += 2;
	ie += 2;

	/* SSID */
	ie = rtw_set_ie(ie, _SSID_IE_, ssid_len, ssid, &sz);

	/* Get OP ch */
	if (rtw_mi_check_status(adapter, MI_LINKED)) {
		if (rtw_phl_mr_get_chandef(dvobj->phl, adapter->phl_role,
							adapter_link->wrlink, &u_chdef)
								!= RTW_PHL_STATUS_SUCCESS) {
			RTW_ERR("%s get union chandef failed\n", __func__);
			rtw_warn_on(1);
		}
		oper_channel = u_chdef.chan;
	}
	else {
		oper_channel = adapter_link->mlmeextpriv.chandef.chan;
	}

	/* supported rates */
	wireless_mode = (WIRELESS_11BG_24N & adapter->registrypriv.wireless_mode);
	rtw_set_supported_rate(supportRate, wireless_mode, oper_channel);
	rateLen = rtw_get_rateset_len(supportRate);
	if (rateLen > 8)
		ie = rtw_set_ie(ie, _SUPPORTEDRATES_IE_, 8, supportRate, &sz);
	else
		ie = rtw_set_ie(ie, _SUPPORTEDRATES_IE_, rateLen, supportRate, &sz);

	/* DS parameter set */
	ie = rtw_set_ie(ie, _DSSET_IE_, 1, &oper_channel, &sz);

	/* ext supported rates */
	if (rateLen > 8)
		ie = rtw_set_ie(ie, _EXT_SUPPORTEDRATES_IE_, (rateLen - 8), (supportRate + 8), &sz);

	RTW_INFO("%s, start auto ap beacon sz=%d\n", __FUNCTION__, sz);

	/* lunch ap mode & start to issue beacon */
	if (rtw_check_beacon_data(adapter, pbuf,  sz) == _SUCCESS) {

	} else
		ret = -EINVAL;


	rtw_mfree(pbuf, len);

	return ret;

}
#endif/* CONFIG_AUTO_AP_MODE */

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
u8 tx_control_hdl(_adapter *adapter)
{
	u8 val;

	if(ATOMIC_READ(&adapter->tbtx_tx_pause))
		val = 0xff;
	else
		val = 0x00;

	rtw_hal_set_hwreg(adapter, HW_VAR_TXPAUSE, &val);

	return H2C_SUCCESS;
}
#endif
void rtw_disconnect_ch_switch(_adapter *adapter)
{
	void *phl = adapter_to_dvobj(adapter)->phl;
	int chanctx_num = 0;
	struct rtw_chan_def chan_def = {0};
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);


	if (!MLME_IS_OPCH_SW(adapter)) {
		/* channel status no need to check here for iface under OPCH_SW */

		chanctx_num = rtw_phl_chanctx_del(phl, adapter->phl_role, adapter_link->wrlink, &chan_def);
		RTW_INFO("PHL - CH:%d, BW:%d OFF:%d\n", chan_def.chan, chan_def.bw, chan_def.offset);

		if (chanctx_num && chan_def.chan != 0)
			set_bch_bwmode(adapter, adapter_link,
					chan_def.band, chan_def.chan, chan_def.offset, chan_def.bw, _FALSE);

		rtw_rfctl_update_op_mode(adapter_to_rfctl(adapter), BIT(adapter->iface_id), 0);
	}
}

#ifdef CONFIG_AP_MODE
u8 stop_ap_hdl(_adapter *adapter)
{
#ifdef CONFIG_DFS_MASTER
	bool hb_to_stop[HW_BAND_MAX];
	int i;
#endif

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(adapter));

#ifdef CONFIG_DFS_MASTER
	for (i = HW_BAND_0; i < HW_BAND_MAX; i++)
		hb_to_stop[i] = rtw_iface_is_operate_at_hwband(adapter, i) ? true : false;
#endif

	rtw_set_802_11_infrastructure_mode(adapter, Ndis802_11Infrastructure, RTW_CMDF_DIRECTLY);
	rtw_setopmode_cmd(adapter, Ndis802_11Infrastructure, RTW_CMDF_DIRECTLY);

#ifdef CONFIG_DFS_MASTER
	for (i = HW_BAND_0; i < HW_BAND_MAX; i++)
		if (hb_to_stop[i])
			rtw_dfs_rd_en_dec_update(adapter_to_dvobj(adapter), i);
#endif

#ifdef CONFIG_AP_CMD_DISPR
	rtw_free_bcn_entry(adapter);
#else
	rtw_phl_ap_stop(adapter_to_dvobj(adapter)->phl, adapter->phl_role);
#endif
	rtw_disconnect_ch_switch(adapter);
	return H2C_SUCCESS;
}
#endif

u8 setopmode_hdl(_adapter *padapter, u8 *pbuf)
{
	u8	type;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct setopmode_parm *psetop = (struct setopmode_parm *)pbuf;

	if (psetop->mode == Ndis802_11APMode
		|| psetop->mode == Ndis802_11_mesh
	) {
		pmlmeinfo->state = WIFI_FW_AP_STATE;
		type = _HW_STATE_AP_;
	} else if (psetop->mode == Ndis802_11Infrastructure) {
		pmlmeinfo->state &= ~(BIT(0) | BIT(1)); /* clear state */
		pmlmeinfo->state |= WIFI_FW_STATION_STATE;/* set to 	STATION_STATE */
		type = _HW_STATE_STATION_;
	} else if (psetop->mode == Ndis802_11IBSS)
		type = _HW_STATE_ADHOC_;
	else if (psetop->mode == Ndis802_11Monitor)
		type = _HW_STATE_MONITOR_;
	else
		type = _HW_STATE_NOLINK_;

	if (_FAIL == rtw_hw_iface_type_change(padapter, type)) {
		RTW_ERR("%s - change iface type fails !\n", __func__);
		return H2C_CMD_FAIL;
	}

#ifdef CONFIG_AP_PORT_SWAP
	rtw_hal_set_hwreg(padapter, HW_VAR_PORT_SWITCH, (u8 *)(&type));
#endif

	rtw_hal_set_hwreg(padapter, HW_VAR_SET_OPMODE, (u8 *)(&type));

#ifdef CONFIG_AUTO_AP_MODE
	if (psetop->mode == Ndis802_11APMode)
		rtw_auto_ap_start_beacon(padapter);
#endif

	return H2C_SUCCESS;

}

u8 createbss_hdl(_adapter *padapter, u8 *pbuf)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK	*padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_ext_priv	*lmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*lmlmeinfo = &(lmlmeext->mlmext_info);
	WLAN_BSSID_EX	*lnetwork = (WLAN_BSSID_EX *)(&(lmlmeinfo->network));
	WLAN_BSSID_EX	*pdev_network = &padapter->registrypriv.dev_network;
	struct createbss_parm *parm = (struct createbss_parm *)pbuf;
	u8 ret = H2C_SUCCESS;
	/* u8	initialgain; */

#ifdef CONFIG_AP_MODE
	if ((parm->req_ch == 0 && pmlmeinfo->state == WIFI_FW_AP_STATE)
		|| parm->req_ch != 0
	) {
		rtw_core_ap_prepare(padapter, parm);
		rtw_core_ap_swch_start(padapter, parm);
		rtw_core_ap_start(padapter, parm);
		goto exit;
	}
#endif

	/* below is for ad-hoc master */
	if (parm->adhoc) {
		rtw_warn_on(pdev_network->InfrastructureMode != Ndis802_11IBSS);
		rtw_joinbss_reset(padapter);

		lmlmeext->chandef.bw = CHANNEL_WIDTH_20;
		lmlmeext->chandef.offset = CHAN_OFFSET_NO_EXT;
		lmlmeinfo->ERP_enable = 0;
		lmlmeinfo->WMM_enable = 0;
		lmlmeinfo->HT_enable = 0;
		lmlmeinfo->HT_caps_enable = 0;
		lmlmeinfo->HT_info_enable = 0;
		pmlmeinfo->agg_enable_bitmap = 0;
		pmlmeinfo->candidate_tid_bitmap = 0;

		/* cancel link timer */
		cancel_link_timer(pmlmeext); /*_cancel_timer_ex(&pmlmeext->link_timer);*/

		/* clear CAM */
		flush_all_cam_entry(padapter, PHL_CMD_DIRECTLY, 0);

		pdev_network->Length = get_WLAN_BSSID_EX_sz(pdev_network);
		_rtw_memcpy(lnetwork, pdev_network, FIELD_OFFSET(WLAN_BSSID_EX, IELength));
		lnetwork->IELength = pdev_network->IELength;

		if (lnetwork->IELength > MAX_IE_SZ) {
			ret = H2C_PARAMETERS_ERROR;
			goto ibss_post_hdl;
		}

		_rtw_memcpy(lnetwork->IEs, pdev_network->IEs, lnetwork->IELength);
		start_create_ibss(padapter);
	} else {
		rtw_warn_on(1);
		ret = H2C_PARAMETERS_ERROR;
	}

ibss_post_hdl:
	rtw_create_ibss_post_hdl(padapter, ret);

exit:
	return ret;
}

static void set_hw_before_join(struct _ADAPTER *a)
{
	struct mlme_ext_priv *pmlmeext = &a->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	struct _WLAN_BSSID_EX *pnetwork = &pmlmeinfo->dev_network;
	struct sta_info *sta;

	/* check already connecting to AP or not */
	if (pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS) {
		if (pmlmeinfo->state & WIFI_FW_STATION_STATE)
			issue_deauth_ex(a, pnetwork->MacAddress,
					WLAN_REASON_DEAUTH_LEAVING, 1, 100);
		pmlmeinfo->state = WIFI_FW_NULL_STATE;
		cancel_link_timer(pmlmeext); /*_cancel_timer_ex(&pmlmeext->link_timer);*/

		sta = rtw_get_stainfo(&a->stapriv, pnetwork->MacAddress);
		if (sta)
			rtw_hw_disconnect(a, sta);
		else
			RTW_ERR(FUNC_ADPT_FMT ": can't find drv sta info for "
				MAC_FMT " !\n",
				FUNC_ADPT_ARG(a),
				MAC_ARG(pnetwork->MacAddress));
	}

	/*reset hw before connection if necessary*/

}

void update_join_link_info(struct _ADAPTER *a, struct _ADAPTER_LINK *alink, struct _WLAN_BSSID_EX *pbuf)
{
	struct link_mlme_ext_info *pmlmeinfo = &alink->mlmeextpriv.mlmext_info;
	struct _WLAN_BSSID_EX *pnetwork = &pmlmeinfo->network;
	struct _NDIS_802_11_VARIABLE_IEs *pIE;
	u32 i;
	u8 join_type;

	alink->mlmepriv.num_FortyMHzIntolerant = 0;
	alink->mlmepriv.num_sta_no_ht = 0;
	alink->mlmepriv.ampdu_priv.ampdu_enable = _FALSE;/* reset to disabled */

	pmlmeinfo->ERP_enable = 0;
	pmlmeinfo->WMM_enable = 0;
	pmlmeinfo->HT_enable = 0;
	pmlmeinfo->HT_caps_enable = 0;
	pmlmeinfo->HT_info_enable = 0;
	pmlmeinfo->SM_PS = SM_PS_DISABLE;
	pmlmeinfo->bwmode_updated = _FALSE;
	/* pmlmeinfo->assoc_AP_vendor = HT_IOT_PEER_MAX; */
	pmlmeinfo->VHT_enable = 0;
	pmlmeinfo->HE_enable = 0;
#ifdef PRIVATE_R
	pmlmeinfo->ht_vht_received = 0;
	_rtw_memset(pmlmeinfo->SupportedRates_infra_ap, 0, NDIS_802_11_LENGTH_RATES_EX);
#endif /* PRIVATE_R */
	_rtw_memcpy(pnetwork, pbuf, FIELD_OFFSET(WLAN_BSSID_EX, IELength));
	pnetwork->IELength = pbuf->IELength;
	_rtw_memcpy(pnetwork->IEs, ((WLAN_BSSID_EX *)pbuf)->IEs, pnetwork->IELength);

	pmlmeinfo->bcn_interval = get_beacon_interval(pnetwork);

	/* sizeof(NDIS_802_11_FIXED_IEs)	 */
	for (i = _FIXED_IE_LENGTH_; i < (pnetwork->IELength - 2);) {
		pIE = (struct _NDIS_802_11_VARIABLE_IEs*)(pnetwork->IEs + i);

		switch (pIE->ElementID) {
		case _VENDOR_SPECIFIC_IE_: /* Get WMM IE. */
			if (_rtw_memcmp(pIE->data, WMM_OUI, 4))
				WMM_param_handler(a, alink, pIE);
			break;

#ifdef CONFIG_80211N_HT
		case _HT_CAPABILITY_IE_:	/* Get HT Cap IE. */
			pmlmeinfo->HT_caps_enable = 1;
			break;

		case _HT_EXTRA_INFO_IE_:	/* Get HT Info IE. */
			pmlmeinfo->HT_info_enable = 1;
			break;
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_80211AC_VHT
		case EID_VHTCapability: /* Get VHT Cap IE. */
			pmlmeinfo->VHT_enable = 1;
			break;

		case EID_VHTOperation: /* Get VHT Operation IE. */
			break;
#endif /* CONFIG_80211AC_VHT */

#ifdef CONFIG_80211AX_HE
		case WLAN_EID_EXTENSION:
			if (pIE->data[0] == WLAN_EID_EXTENSION_HE_CAPABILITY)
				pmlmeinfo->HE_enable = 1;
			break;
#endif /* CONFIG_80211AX_HE */

		default:
			break;
		}

		i += (pIE->Length + 2);
	}

	/*get chan/bw/offset info from IEs*/
	rtw_bss_get_chbw(pnetwork, &alink->mlmeextpriv.chandef.band,
			 &alink->mlmeextpriv.chandef.chan, (u8 *)&alink->mlmeextpriv.chandef.bw,
			 (u8 *)&alink->mlmeextpriv.chandef.offset, pmlmeinfo->HT_caps_enable,
			 pmlmeinfo->VHT_enable, pmlmeinfo->HE_enable);

	/*adjust chan/bw/offset with registary and hw cap*/
	rtw_adjust_bchbw(a, alink->mlmeextpriv.chandef.band, alink->mlmeextpriv.chandef.chan,
			(u8 *)&alink->mlmeextpriv.chandef.bw, (u8 *)&alink->mlmeextpriv.chandef.offset);

	RTW_INFO("%s: band=%d, chan=%d, bw=%d, offset=%d\n", __func__,
			alink->mlmeextpriv.chandef.band, alink->mlmeextpriv.chandef.chan,
			alink->mlmeextpriv.chandef.bw, alink->mlmeextpriv.chandef.offset);
}

void update_join_info(struct _ADAPTER *a, struct _WLAN_BSSID_EX *pbuf)
{
	struct mlme_ext_priv *pmlmeext = &a->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	struct _WLAN_BSSID_EX *pnetwork = &pmlmeinfo->dev_network;
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(a);
	struct wlan_mld_network *mld_network = NULL;
	struct wlan_network *link_network = NULL;
	u8 lidx;

	pmlmeinfo->agg_enable_bitmap = 0;
	pmlmeinfo->candidate_tid_bitmap = 0;
	_rtw_memcpy(pnetwork, pbuf, FIELD_OFFSET(WLAN_BSSID_EX, IELength));
	pnetwork->IELength = pbuf->IELength;
	_rtw_memcpy(pnetwork->IEs, ((WLAN_BSSID_EX *)pbuf)->IEs, pnetwork->IELength);

	/* Update the join info for the primary link */
	update_join_link_info(a, alink, pbuf);
#ifdef CONFIG_80211BE_EHT
	if (pbuf->is_mld) {
		mld_network = pbuf->mld_network;
		/* Replace adapter's mlmeinfo->network->MacAddress with MLD MAC Address */
		_rtw_memcpy(pnetwork->MacAddress, mld_network->mac_addr, ETH_ALEN);
		/* Update the join info of the rest desired links, where to_join == _TRUE */
		for (lidx = 0; lidx < a->adapter_link_num; lidx++) {
			alink = GET_LINK(a, lidx);
			if (!alink->mlmepriv.to_join || lidx == RTW_RLINK_PRIMARY)
				continue;
			/* The link_id is assigned at select_and_join_from_scanned_queue() */
			link_network = rtw_get_link_network_by_linkid(mld_network, alink->mlmepriv.link_id);
			if (!link_network) {
				RTW_INFO(FUNC_ADPT_FMT" network is NULL for link_id %u, set adapter_link %d to_join to _FALSE\n"
					, FUNC_ADPT_ARG(a), alink->mlmepriv.link_id, alink->mlmepriv.link_id);
				alink->mlmepriv.to_join = _FALSE;
				continue;
			}
			update_join_link_info(a, alink, &link_network->network);
		}
	}
#endif
}

#if 0
static void set_hw_prepare_connect(_adapter *padapter, struct sta_info *sta)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &pmlmeext->mlmext_info;
	WLAN_BSSID_EX		*pnetwork = &pmlmeinfo->network;

#ifdef CONFIG_ANTENNA_DIVERSITY
	rtw_antenna_select_cmd(padapter, pnetwork->PhyInfo.Optimum_antenna, _FALSE);
#endif
	rtw_hw_prepare_connect(padapter, sta, pnetwork->MacAddress);
}
#endif

/*
join_res = -1 caller is rtw_hw_connect_abort
join_res = 1 caller is rtw_hw_connected
*/
void rtw_join_done_chk_ch(_adapter *adapter, int join_res)
{
#define DUMP_ADAPTERS_STATUS 0

	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	_adapter *iface;
	struct mlme_priv *mlme;
	struct mlme_ext_priv *mlmeext;
	int chanctx_num = 0;
	struct rtw_chan_def u_chdef = {0};
	struct rtw_chan_def new_chdef = {0};
	struct rtw_mr_chctx_info mr_cc_info = {0};
	int i, ret;
	bool is_chctx_add = false;
	struct _ADAPTER_LINK *adapter_link = NULL;
	struct _ADAPTER_LINK *iface_link = NULL;
	struct link_mlme_priv *lmlme;
	struct link_mlme_ext_priv *lmlmeext;
	u8 lidx, iface_lidx;

	#if DUMP_ADAPTERS_STATUS
	RTW_INFO(FUNC_ADPT_FMT" enter\n", FUNC_ADPT_ARG(adapter));
	dump_adapters_status(RTW_DBGDUMP , dvobj);
	#endif


	for (lidx = 0; lidx < adapter->adapter_link_num; lidx++) {
		adapter_link = GET_LINK(adapter, lidx);

		chanctx_num = rtw_phl_mr_get_chanctx_num(dvobj->phl, adapter->phl_role, adapter_link->wrlink);
		if (join_res >= 0 && chanctx_num <= 0) {
			dump_adapters_status(RTW_DBGDUMP , dvobj);
			RTW_ERR("%s join_res:%d, chanctx_num:%d failed\n", __func__, join_res, chanctx_num);
			join_res = -1;
			rtw_warn_on(1);
		}

		if (rtw_phl_mr_get_chandef(dvobj->phl, adapter->phl_role, adapter_link->wrlink, &u_chdef)
							!= RTW_PHL_STATUS_SUCCESS) {
			RTW_ERR("%s get union chandef failed\n", __func__);
			rtw_warn_on(1);
			return;
		}
		if (u_chdef.chan == 0) {
			RTW_ERR("%s chandef.chan == 0\n", __func__);
			rtw_warn_on(1);
			return;
		}

		_rtw_memcpy(&new_chdef, &u_chdef, sizeof(struct rtw_chan_def));
		#ifdef DBG_DUMP_CHAN_SET
		RTW_INFO("[UCH] u_band:%d\n", u_chdef.band);
		RTW_INFO("[UCH] u_chan:%d\n", u_chdef.chan);
		RTW_INFO("[UCH] u_bw:%d\n", u_chdef.bw);
		RTW_INFO("[UCH] u_offset:%d\n", u_chdef.offset);
		#endif
		/* Only the accepted adapter_link join success */
		if (join_res >= 0 && adapter_link->mlmepriv.is_accepted) { /*client join success - restart all ap*/
			for (i = 0; i < dvobj->iface_nums; i++) {
				iface = dvobj->padapters[i];
				mlme = &iface->mlmepriv;
				mlmeext = &iface->mlmeextpriv;

				if (!iface || iface == adapter || !rtw_is_adapter_up(iface))
					continue;

				for (iface_lidx = 0; iface_lidx < iface->adapter_link_num; iface_lidx++) {
					/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
					iface_link = GET_PRIMARY_LINK(iface);
					lmlme = &iface_link->mlmepriv;
					lmlmeext = &iface_link->mlmeextpriv;

					/* ToDo CONFIG_RTW_MLD: should compare the band of links */
					if (!rtw_iface_at_same_hwband(adapter, iface))
						continue;

					if ((MLME_IS_AP(iface) || MLME_IS_MESH(iface))
						&& check_fwstate(mlme, WIFI_ASOC_STATE)
						&& check_fwstate(mlme, WIFI_OP_CH_SWITCHING)
					) {
						u8 ori_band, ori_ch, ori_bw, ori_offset;

						/* handle AP which need to switch ch setting */
						ori_band = lmlmeext->chandef.band;
						ori_ch = lmlmeext->chandef.chan;
						ori_bw = lmlmeext->chandef.bw;
						ori_offset = lmlmeext->chandef.offset;

						/* restore original bw, adjust bw by registry setting on target ch */
						lmlmeext->chandef.bw = lmlme->ori_chandef.bw;
						lmlmeext->chandef.band = new_chdef.band;
						lmlmeext->chandef.chan = new_chdef.chan;

						/*adjust bw according registery and hw cap*/
						rtw_adjust_bchbw(iface, lmlmeext->chandef.band, lmlmeext->chandef.chan,
									(u8 *)&lmlmeext->chandef.bw,
									(u8 *)&lmlmeext->chandef.offset);
						#ifdef DBG_DUMP_CHAN_SET
						RTW_INFO("[IF] if_band:%d\n", lmlmeext->chandef.band);
						RTW_INFO("[IF] if_chan:%d\n", lmlmeext->chandef.chan);
						RTW_INFO("[IF] if_bw:%d\n", lmlmeext->chandef.bw);
						RTW_INFO("[IF] if_offset:%d\n", lmlmeext->chandef.offset);
						#endif
						#ifdef CONFIG_RTW_MESH
						if (MLME_IS_MESH(iface))
							rtw_mesh_adjust_bchbw(	mlmeext->chandef.band,
										mlmeext->chandef.chan,
										&mlmeext->chandef.bw,
										&mlmeext->chandef.offset);
						#endif

						/*adjust offset according to union chan and bw*/
						#if 0
						rtw_chset_sync_chbw(adapter_to_chset(adapter)
								, &mlmeext->chandef.chan, &mlmeext->chandef.bw, &mlmeext->chandef.offset
								, &u_ch, &u_bw, &u_offset, 1, 0);
						#endif

						new_chdef.bw = lmlmeext->chandef.bw;
						rtw_phl_adjust_chandef(dvobj->phl, iface_link->wrlink, &new_chdef);
						lmlmeext->chandef.offset = new_chdef.offset;
						is_chctx_add = rtw_phl_chanctx_add(dvobj->phl, iface->phl_role,
											iface_link->wrlink,
											&new_chdef, &mr_cc_info);
						rtw_hw_update_chan_def(iface, iface_link);

						RTW_INFO(FUNC_ADPT_FMT" ori:%u %u,%u,%u => cur:%i %u,%u,%u => grp:%i %u,%u,%u - chctx_add:%s\n",
								FUNC_ADPT_ARG(iface),
								ori_band, ori_ch, ori_bw, ori_offset,
								lmlmeext->chandef.band, lmlmeext->chandef.chan, lmlmeext->chandef.bw, lmlmeext->chandef.offset,
								new_chdef.band, new_chdef.chan, new_chdef.bw, new_chdef.offset,
								(is_chctx_add) ? "Y" : "N");
						if (is_chctx_add == false) {
							RTW_ERR(FUNC_ADPT_FMT" chctx_add failed\n", FUNC_ADPT_ARG(iface));
							rtw_warn_on(1);
						}

						rtw_ap_update_bss_bchbw(iface, iface_link, &(lmlmeext->mlmext_info.network), &lmlmeext->chandef);

						_rtw_memcpy(&(lmlme->cur_network.network), &(lmlmeext->mlmext_info.network), sizeof(WLAN_BSSID_EX));

						rtw_start_bss_hdl_after_chbw_decided(iface, iface_link);

						{
							#if defined(CONFIG_IOCTL_CFG80211) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0))
							u8 ht_option = 0;

							#ifdef CONFIG_80211N_HT
							ht_option = lmlme->htpriv.ht_option;
							#endif

							rtw_cfg80211_ch_switch_notify(iface
								, &lmlmeext->chandef
								, ht_option, 0);
							#endif
						}

						clr_fwstate(mlme, WIFI_OP_CH_SWITCHING);
						rtw_update_beacon(iface, iface_link, 0xFF, NULL, _TRUE, 0);
						rtw_phl_cmd_stop_beacon(adapter_to_dvobj(adapter)->phl, iface_link->wrlink, _FALSE, PHL_CMD_DIRECTLY, 0);
					}
				}
			}
			/* ToDo CONFIG_RTW_MLD: [DFS] */
			#ifdef CONFIG_DFS_MASTER
			rtw_dfs_rd_en_dec_on_mlme_act(adapter, adapter_link, MLME_STA_CONNECTED, 0);
			#endif

			if (u_chdef.chan != new_chdef.chan) {
				RTW_ERR("[MI] chandef->chan(%d) != u_ch(%d)\n", u_chdef.chan, new_chdef.chan);
				rtw_warn_on(1);
			}
		} else { /*client join failed - restart all ap on ori-chan*/
			for (i = 0; i < dvobj->iface_nums; i++) {
				iface = dvobj->padapters[i];
				mlme = &iface->mlmepriv;
				mlmeext = &iface->mlmeextpriv;

				if (!iface || iface == adapter || !rtw_is_adapter_up(iface))
					continue;

				for (iface_lidx = 0; iface_lidx < iface->adapter_link_num; iface_lidx++) {
					iface_link = GET_LINK(iface, iface_lidx);
					lmlme = &iface_link->mlmepriv;
					lmlmeext = &iface_link->mlmeextpriv;

					/* ToDo CONFIG_RTW_MLD: should compare the band of links */
					if (!rtw_iface_at_same_hwband(adapter, iface))
						continue;

					if ((MLME_IS_AP(iface) || MLME_IS_MESH(iface))
						&& check_fwstate(mlme, WIFI_ASOC_STATE)
						&& check_fwstate(mlme, WIFI_OP_CH_SWITCHING)
					) {
						_rtw_memcpy(&new_chdef, &lmlmeext->chandef, sizeof(struct rtw_chan_def));
						is_chctx_add = rtw_phl_chanctx_add(dvobj->phl, iface->phl_role,
											iface_link->wrlink,
											&new_chdef, &mr_cc_info);
						if (is_chctx_add == _FALSE) {
							RTW_ERR(FUNC_ADPT_FMT "chctx_add failed\n", FUNC_ADPT_ARG(iface));
							rtw_warn_on(1);
						}
						RTW_INFO(FUNC_ADPT_FMT" cur:%u,%u,%u => grp:%u,%u,%u chctx_add:%s\n",
								FUNC_ADPT_ARG(iface),
								lmlmeext->chandef.chan, lmlmeext->chandef.bw, lmlmeext->chandef.offset,
								new_chdef.chan, new_chdef.bw, new_chdef.offset,
								(is_chctx_add) ? "Y" : "N");
						clr_fwstate(mlme, WIFI_OP_CH_SWITCHING);
						rtw_update_beacon(iface,iface_link, 0xFF, NULL, _TRUE, 0);
						rtw_phl_cmd_stop_beacon(adapter_to_dvobj(adapter)->phl, iface_link->wrlink, _FALSE, PHL_CMD_DIRECTLY, 0);
					}
				}
			}

			#ifdef CONFIG_80211D
			rtw_alink_leavebss_update_regulatory(adapter_link);
			#endif

			/* ToDo CONFIG_RTW_MLD: [DFS] */
			#ifdef CONFIG_DFS_MASTER
			rtw_dfs_rd_en_dec_on_mlme_act(adapter, adapter_link, MLME_STA_DISCONNECTED, 0);
			#endif
		}

		RTW_INFO("[MI] union:%u,%u,%u,%u\n", new_chdef.band, new_chdef.chan, new_chdef.bw, new_chdef.offset);
		RTW_INFO("[MI] u chandef:%u,%u,%u,%u\n", u_chdef.band, u_chdef.chan, u_chdef.bw, u_chdef.offset);

		set_bch_bwmode(adapter, adapter_link, new_chdef.band, new_chdef.chan, new_chdef.offset, new_chdef.bw, _FALSE);

		if (join_res >= 0)
			rtw_rfctl_update_op_mode(adapter_to_rfctl(adapter), BIT(adapter->iface_id), 1);
	}

	#if DUMP_ADAPTERS_STATUS
	RTW_INFO(FUNC_ADPT_FMT" exit\n", FUNC_ADPT_ARG(adapter));
	dump_adapters_status(RTW_DBGDUMP , dvobj);
	#endif
}

#if 0
int rtw_chk_start_clnt_join(_adapter *adapter, u8 *ch, u8 *bw, u8 *offset)
{
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	u8 u_ch = 0, cur_ch = 0;
	enum channel_width u_bw = 0, cur_bw = 0;
	enum chan_offset u_offset = 0, cur_offset = 0;
	struct rtw_chan_def new_chdef = {0};
	struct rtw_mr_chctx_info mr_cc_info = {0};
	bool is_chctx_add = false;
	struct mi_state mstate;
	bool connect_allow = _TRUE;

	if (!ch || !bw || !offset) {
		connect_allow = _FALSE;
		rtw_warn_on(1);
		goto _exit;
	}

	new_chdef.chan = cur_ch = pmlmeext->chandef.chan;
	new_chdef.bw = cur_bw = pmlmeext->chandef.bw;
	new_chdef.offset = cur_offset = pmlmeext->chandef.offset;
	new_chdef.band = rtw_get_band_type(new_chdef.chan);

	RTW_INFO(FUNC_ADPT_FMT" CONT - CH:%d, BW:%d OFF:%d\n",
			FUNC_ADPT_ARG(adapter),cur_ch, cur_bw, cur_offset);

	is_chctx_add = rtw_phl_chanctx_add(dvobj->phl, adapter->phl_role,
					&new_chdef, &mr_cc_info);

	if (is_chctx_add == false) {
		rtw_mi_status_no_self(adapter, &mstate);
		RTW_INFO(FUNC_ADPT_FMT" others ld_sta_num:%u, ap_num:%u, mesh_num:%u\n"
				, FUNC_ADPT_ARG(adapter), MSTATE_STA_LD_NUM(&mstate)
				, MSTATE_AP_NUM(&mstate), MSTATE_MESH_NUM(&mstate));

		if (MSTATE_STA_LD_NUM(&mstate) + MSTATE_AP_LD_NUM(&mstate) + MSTATE_MESH_LD_NUM(&mstate) >= 4)
			connect_allow = _FALSE;

		#if 0 /*def CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT*/
		for (i = 0; i < dvobj->iface_nums; i++) {
			iface = dvobj->padapters[i];
			mlme = &iface->mlmepriv;

			if (MLME_IS_STA(iface)
				&& check_fwstate(mlme, WIFI_ASOC_STATE)
				#if defined(CONFIG_P2P)
				&& rtw_p2p_chk_state(&(iface->wdinfo), P2P_STATE_NONE)
				#endif
			) {
				connect_allow = _FALSE;
				break;
			}
		}
		#endif /* CONFIG_CFG80211_ONECHANNEL_UNDER_CONCURRENT */

		if (connect_allow == _FALSE) {
			RTW_INFO(FUNC_ADPT_FMT" connect_allow = _FALSE\n", FUNC_ADPT_ARG(adapter));
			goto _exit;
		}

		rtw_mi_buddy_disconnect(adapter, DISCONNECTION_BY_DRIVER_DUE_TO_EACH_IFACE_CHBW_NOT_SYNC);
		rtw_phl_chanctx_del_no_self(dvobj->phl, adapter->phl_role);

		new_chdef.chan = cur_ch;
		new_chdef.bw = cur_bw;
		new_chdef.offset = cur_offset;
		new_chdef.band = rtw_get_band_type(new_chdef.chan);
		is_chctx_add = rtw_phl_chanctx_add(dvobj->phl, adapter->phl_role,
					&new_chdef, &mr_cc_info);
		if (is_chctx_add == false) {
			RTW_ERR("%s - %d chanctx_add failed\n", __FUNCTION__, __LINE__);
			rtw_warn_on(1);
			connect_allow = _FALSE;
			goto _exit;
		}
	}

	#ifdef CONFIG_DFS_MASTER
	rtw_dfs_rd_en_decision(adapter, MLME_STA_CONNECTING, 0);
	#endif

	*ch = new_chdef.chan;
	*bw = new_chdef.bw;
	*offset = new_chdef.offset;
_exit:
	RTW_INFO(FUNC_ADPT_FMT" chctx_add:%s\n", FUNC_ADPT_ARG(adapter), (is_chctx_add) ? "Y" : "N");
	RTW_INFO("Grouped - CH:%d, BW:%d OFF:%d\n",new_chdef.chan, new_chdef.bw, new_chdef.offset);
	return connect_allow;
}
#endif

static WLAN_BSSID_EX *join_cmd_pre_hdl(_adapter *padapter, WLAN_BSSID_EX *network)
{
	WLAN_BSSID_EX		*psecnetwork = NULL;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
#ifdef CONFIG_80211BE_EHT
	struct _ADAPTER_LINK *adapter_link = NULL;
	struct wlan_mld_network *pmld_network = NULL;
	struct wlan_mld_network *rec_mld_network = NULL;
	struct wlan_network *plink_network;
	struct wlan_network *rec_link_network;
	u8 lidx;
#endif

	pmlmeinfo->assoc_AP_vendor = check_assoc_AP(network->IEs, network->IELength);
	rtw_update_phl_iot(padapter, pmlmeinfo->assoc_AP_vendor);

#ifdef CONFIG_80211BE_EHT
	if (network->is_mld)
		pmld_network = network->mld_network;
#endif

	/* Added by Albert 2009/02/18 */
	/* If the the driver wants to use the bssid to create the connection. */
	/* If not,  we have to copy the connecting AP's MAC address to it so that */
	/* the driver just has the bssid information for PMKIDList searching. */

	if (padapter->mlmepriv.assoc_by_bssid == _FALSE) {
#ifdef CONFIG_80211BE_EHT
		if (pmld_network)
			_rtw_memcpy(&padapter->mlmepriv.assoc_bssid[0], &pmld_network->mac_addr[0], ETH_ALEN);
		else
#endif
			_rtw_memcpy(&padapter->mlmepriv.assoc_bssid[0], &network->MacAddress[0], ETH_ALEN);
	}

#ifdef CONFIG_80211BE_EHT
	if (pmld_network) {
		/* Allocate mld network to avoid race condition issue */
		rec_mld_network = rtw_clone_to_join_mld_network(padapter, pmld_network);
		if (rec_mld_network == NULL)
			goto exit;

		for (lidx = 0; lidx < rec_mld_network->network_num; lidx++) {
			rec_link_network = GET_LINK_NETWORK(rec_mld_network, lidx);
			plink_network = rtw_get_link_network_by_linkid(pmld_network, rec_link_network->network.link_id);
			adapter_link = rtw_get_adapter_link_by_linkid(padapter, rec_link_network->network.link_id);
			if (!adapter_link)
				continue;

			rtw_update_join_priv(padapter, adapter_link, &plink_network->network, &rec_link_network->network);
			if (adapter_link == padapter_link)
				psecnetwork = &rec_link_network->network;
		}
	} else
#endif
	{
		/*
			Modified by Arvin 2015/05/13
			Solution for allocating a new WLAN_BSSID_EX to avoid race condition issue between disconnect and joinbss
		*/
		psecnetwork = (WLAN_BSSID_EX *)rtw_zmalloc(sizeof(WLAN_BSSID_EX));
		if (psecnetwork == NULL)
			goto exit;

		_rtw_memset(psecnetwork, 0, sizeof(WLAN_BSSID_EX));
		_rtw_memcpy(psecnetwork, network, get_WLAN_BSSID_EX_sz(network));
		rtw_update_join_priv(padapter, padapter_link, network, psecnetwork);
	}

exit:
	return psecnetwork;
}

u8 rtw_join_cmd_hdl(_adapter *padapter, u8 *pbuf)
{
#if 0 /*ifndef CONFIG_STA_CMD_DISPR*/
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &pmlmeext->mlmext_info;
	WLAN_BSSID_EX		*pnetwork = &pmlmeinfo->network;
	struct sta_info *sta = NULL;
	u8 u_ch, u_bw, u_offset;

	/* Check pbuf->IELength */
	if (((struct _WLAN_BSSID_EX*)pbuf)->IELength > MAX_IE_SZ)
		return H2C_PARAMETERS_ERROR;

	if (((struct _WLAN_BSSID_EX*)pbuf)->IELength < 2) {
		report_join_res(padapter, (-4), WLAN_STATUS_UNSPECIFIED_FAILURE);
		return H2C_SUCCESS;
	}

	set_hw_before_join(padapter);

	/*update HT/VHT/HE CAP and chan/bw/offset from join_info-pbuf*/
	update_join_info(padapter, (struct _WLAN_BSSID_EX *)pbuf);

	sta = rtw_get_stainfo(&padapter->stapriv, padapter->phl_role->mac_addr);
	rtw_free_stainfo(padapter, sta);
	sta = rtw_alloc_stainfo(&padapter->stapriv, pnetwork->MacAddress, PHL_CMD_DIRECTLY);
	if (sta == NULL) {
		RTW_ERR("alloc sta "MAC_FMT" fail\n", MAC_ARG(pnetwork->MacAddress));
		return H2C_CMD_FAIL;
	}

	/* check channel, bandwidth, offset and switch */
	if (rtw_chk_start_clnt_join(padapter, &u_ch, &u_bw, &u_offset) == _FAIL) {
		report_join_res(padapter, (-4), WLAN_STATUS_UNSPECIFIED_FAILURE);
		return H2C_SUCCESS;
	}

	/*allow for connection*/
	set_hw_prepare_connect(padapter, sta);

	rtw_hw_update_chan_def(padapter);
	set_channel_bwmode(padapter, u_ch, u_offset, u_bw, _TRUE);
	rtw_phl_mr_dump_cur_chandef(adapter_to_dvobj(padapter)->phl, padapter->phl_role);

	/* cancel link timer */
	cancel_link_timer(pmlmeext); /*_cancel_timer_ex(&pmlmeext->link_timer);*/

	/*Client and AD-Hoc client*/
	start_clnt_join(padapter);

	return H2C_SUCCESS;
#else
	struct joinbss_parm	*pparm = (struct joinbss_parm *)pbuf;
	WLAN_BSSID_EX *jnetwork = NULL;
	u8 ret = H2C_SUCCESS;

	if ((pparm->network.IELength > MAX_IE_SZ) || (pparm->network.IELength < 2)) {
		RTW_ERR(FUNC_ADPT_FMT ": invalid IE length(%u)\n",
			 FUNC_ADPT_ARG(padapter), pparm->network.IELength);
		ret = H2C_PARAMETERS_ERROR;
		goto exit;
	}

	jnetwork = join_cmd_pre_hdl(padapter, &pparm->network);
	if (!jnetwork) {
		RTW_ERR(FUNC_ADPT_FMT ": join_cmd_pre_hdl return NULL\n",
			 FUNC_ADPT_ARG(padapter));
		ret = H2C_MEMORY;
		goto exit;
	}

	if (rtw_connect_cmd(padapter, jnetwork) != RTW_PHL_STATUS_SUCCESS)
		ret = H2C_CMD_FAIL;

#ifdef CONFIG_80211BE_EHT
	if (jnetwork->is_mld)
		rtw_free_cloned_mld_network(jnetwork->mld_network);
	else
#endif
		rtw_mfree(jnetwork, sizeof(*jnetwork));

exit:
	if (ret != H2C_SUCCESS)
		report_join_res(padapter, (-4), WLAN_STATUS_UNSPECIFIED_FAILURE);
	return ret;
#endif
}

u8 disconnect_hdl(_adapter *padapter, unsigned char *pbuf)
{
#if CONFIG_DFS
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
#endif
	struct disconnect_parm *param = (struct disconnect_parm *)pbuf;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8 val8;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	WLAN_BSSID_EX		*pnetwork = (WLAN_BSSID_EX *)(&(padapter_link->mlmeextpriv.mlmext_info.network));

	/* For STA, disconnect flow only for conneted case */
	if (MLME_IS_STA(padapter) && !MLME_IS_ASOC(padapter))
		goto exit;

	if (is_client_associated_to_ap(padapter)
		#if CONFIG_DFS
		&& !IS_RADAR_DETECTED(rfctl)
		#ifdef CONFIG_ECSA_PHL
		&& !rtw_mr_is_ecsa_running(padapter)
		#endif
		#endif
	) {
		#ifdef CONFIG_PLATFORM_ROCKCHIPS
		/* To avoid connecting to AP fail during resume process, change retry count from 5 to 1 */
		issue_deauth_ex(padapter, pnetwork->MacAddress, WLAN_REASON_DEAUTH_LEAVING, 1, 100);
		#else
		issue_deauth_ex(padapter, pnetwork->MacAddress, WLAN_REASON_DEAUTH_LEAVING, param->deauth_timeout_ms / 100, 100);
		#endif /* CONFIG_PLATFORM_ROCKCHIPS */
	}

	if (((pmlmeinfo->state & 0x03) == WIFI_FW_ADHOC_STATE) || ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)) {
		/* Stop BCN */
		val8 = 0;
		rtw_hal_set_hwreg(padapter, HW_VAR_BCN_FUNC, (u8 *)(&val8));
	}

	rtw_mlmeext_disconnect(padapter);
	#if 0
	rtw_free_uc_swdec_pending_queue(adapter_to_dvobj(padapter));
	#endif

exit:
	return	H2C_SUCCESS;
}

/**
 * rtw_ps_annc - check and doing ps announcement for all the adapters
 * @adapter: the requesting adapter
 * @ps: power saving or not
 *
 * Returns: 0: no ps announcement is doing. 1: ps announcement is doing
 */
u8 rtw_ps_annc(_adapter *adapter, bool ps)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	_adapter *iface;
	int i;
	u8 ps_anc = 0;
	struct _ADAPTER_LINK *iface_link;
	u8 lidx;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface)
			continue;
		if (!rtw_iface_at_same_hwband(adapter, iface))
			continue;

		if (MLME_IS_STA(iface)) {
			if (is_client_associated_to_ap(iface) == _TRUE) {
				/* TODO: TDLS peers */
				for (lidx = 0; lidx < iface->adapter_link_num; lidx++) {
					iface_link = GET_LINK(iface, lidx);
					if (iface_link->wrlink->mstate == MLME_LINKED
						#ifdef CONFIG_MCC_MODE
						&& !rtw_hw_mcc_chk_inprogress(adapter, iface_link)
						#endif
					)
						issue_nulldata(iface, iface_link, NULL, ps, 3, 500);
				}
				ps_anc = 1;
			}
		#ifdef CONFIG_RTW_MESH
		} else if (MLME_IS_MESH(iface)) {
			if (rtw_mesh_ps_annc(iface, ps))
				ps_anc = 1;
		#endif
		}
	}
	return ps_anc;
}

void rtw_back_opch(_adapter *adapter)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	_rtw_mutex_lock_interruptible(&rfctl->offch_mutex);

	if (rfctl->offch_state != OFFCHS_NONE) {
		rfctl->offch_state = OFFCHS_BACKING_OP;
		rtw_hal_macid_wakeup_all_used(adapter);
		rtw_ps_annc(adapter, 0);

		rfctl->offch_state = OFFCHS_NONE;
		rtw_mi_os_xmit_schedule(adapter);
	}

	_rtw_mutex_unlock(&rfctl->offch_mutex);
}

void rtw_leave_opch(_adapter *adapter)
{
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	_rtw_mutex_lock_interruptible(&rfctl->offch_mutex);

	if (rfctl->offch_state == OFFCHS_NONE) {
		/* prepare to leave operating channel */
		rfctl->offch_state = OFFCHS_LEAVING_OP;

		/* clear HW TX queue */
		rtw_hal_set_hwreg(adapter, HW_VAR_CHECK_TXBUF, 0);

		rtw_hal_macid_sleep_all_used(adapter);

		rtw_ps_annc(adapter, 1);

		rfctl->offch_state = OFFCHS_LEAVE_OP;
	}

	_rtw_mutex_unlock(&rfctl->offch_mutex);
}

u8 setauth_hdl(_adapter *padapter, unsigned char *pbuf)
{
	struct setauth_parm		*pparm = (struct setauth_parm *)pbuf;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (pparm->mode < 4)
		pmlmeinfo->auth_algo = pparm->mode;

	return	H2C_SUCCESS;
}

static u8 amsdu_spp_enable(_adapter *pdapter, enum security_type type)
{
	u8 ret = _FALSE;
	if (pdapter->registrypriv.amsdu_mode == RTW_AMSDU_MODE_SPP) {
		if ( type == _AES_ || type == _CCMP_256_
			|| type == _GCMP_ || type == _GCMP_256_ )
			ret = _SUCCESS;
	}
	return ret;
}

#ifdef CONFIG_CMD_DISP
/*Set WEP key or Group Key*/
u8 setkey_hdl(struct _ADAPTER *a,struct _ADAPTER_LINK *alink, struct setkey_parm *key,
		enum phl_cmd_type cmd_type,  u32 cmd_timeout)
{
	struct mlme_ext_info *info = &a->mlmeextpriv.mlmext_info;
	u8 keytype = 1; /* 1 for Group Key */
	u8 *mac;
	struct sta_info *sta;
	u8 spp = 0;


	if (((info->state & 0x03) == WIFI_FW_AP_STATE)
	    || ((info->state & 0x03) == WIFI_FW_ADHOC_STATE))
		mac = alink->mac_addr;
	else
		mac = get_link_bssid(&alink->mlmepriv);

	sta = rtw_get_stainfo(&a->stapriv, mac);
	if (!sta) {
		RTW_ERR("%s: sta %pM not found\n", __func__, mac);
		goto exit;
	}

	/* main tx key for wep. */
	if (key->set_tx)
		a->mlmeextpriv.mlmext_info.key_index = key->keyid;

	/* Change to Unicast for WEP */
	if (is_wep_enc(key->algorithm))
		keytype = 0;

	#ifdef CONFIG_IEEE80211W
	if (key->algorithm == _BIP_CMAC_128_)
		keytype = 2;
	#endif

	if (amsdu_spp_enable(a, key->algorithm) == _SUCCESS)
		spp = 1;

	RTW_PRINT("%s: set %s key for %pM, kid:%d type:%u algo:%s\n",
		  __func__, is_wep_enc(key->algorithm)?"WEP":"group", mac,
		  key->keyid, keytype, security_type_str(key->algorithm));

	/* ToDo CONFIG_RTW_MLD: HALMAC API */
	rtw_hw_add_key(a, sta, key->keyid, key->algorithm, keytype, key->key, spp, cmd_type, cmd_timeout);

#ifdef CONFIG_LAYER2_ROAMING
	dequeuq_roam_pkt(a);
#endif

exit:
	return H2C_SUCCESS;
}
#else
/*Set WEP key or Group Key*/
u8 setkey_hdl(struct _ADAPTER *a, u8 *pbuf)
{
	struct setkey_parm *key = (struct setkey_parm *)pbuf;
	struct mlme_ext_info *info = &a->mlmeextpriv.mlmext_info;
	u8 keytype = 1; /* 1 for Group Key */
	u8 *mac;
	struct sta_info *sta;
	u8 spp = 0;

	if (((info->state & 0x03) == WIFI_FW_AP_STATE)
	    || ((info->state & 0x03) == WIFI_FW_ADHOC_STATE))
		mac = adapter_mac_addr(a);
	else
		mac = get_bssid(&a->mlmepriv);
	sta = rtw_get_stainfo(&a->stapriv, mac);
	if (!sta) {
		RTW_ERR("%s: sta %pM not found\n", __func__, mac);
		goto exit;
	}

	/* main tx key for wep. */
	if (key->set_tx)
		a->mlmeextpriv.mlmext_info.key_index = key->keyid;

	/* Change to Unicast for WEP */
	if (is_wep_enc(key->algorithm))
		keytype = 0;

	#ifdef CONFIG_IEEE80211W
	if (key->algorithm == _BIP_CMAC_128_)
		keytype = 2;
	#endif

	if (amsdu_spp_enable(a, key->algorithm) == _SUCCESS)
		spp = 1;

	RTW_PRINT("%s: set %s key for %pM, kid:%d type:%u algo:%s\n",
		  __func__, is_wep_enc(key->algorithm)?"WEP":"group", mac,
		  key->keyid, keytype, security_type_str(key->algorithm));

	rtw_hw_add_key(a, sta, key->keyid, key->algorithm, keytype, key->key, spp, PHL_CMD_DIRECTLY, 0);
#ifdef CONFIG_LAYER2_ROAMING
	dequeuq_roam_pkt(a);
#endif

exit:
	return H2C_SUCCESS;
}
#endif

void rtw_ap_wep_pk_setting(_adapter *adapter, struct sta_info *psta)
{
	struct security_priv *psecuritypriv = &(adapter->securitypriv);
	struct set_stakey_parm	sta_pparm;
	sint keyid;

	if (!is_wep_enc(psecuritypriv->dot11PrivacyAlgrthm))
		return;

	for (keyid = 0; keyid < 4; keyid++) {
		if ((psecuritypriv->key_mask & BIT(keyid)) && (keyid == psecuritypriv->dot11PrivacyKeyIndex)) {
			sta_pparm.algorithm = psecuritypriv->dot11PrivacyAlgrthm;
			sta_pparm.keyid = keyid;
			sta_pparm.gk = 0;
			_rtw_memcpy(sta_pparm.key, &(psecuritypriv->dot11DefKey[keyid].skey[0]), 16);
			_rtw_memcpy(sta_pparm.addr, psta->phl_sta->mac_addr, ETH_ALEN);

			RTW_PRINT(FUNC_ADPT_FMT"set WEP - PK with "MAC_FMT" keyid:%u\n"
				, FUNC_ADPT_ARG(adapter), MAC_ARG(psta->phl_sta->mac_addr), keyid);
#ifdef CONFIG_CMD_DISP
			set_stakey_hdl(adapter, &sta_pparm, PHL_CMD_DIRECTLY, 0);
#else /* CONFIG_FSM */
			set_stakey_hdl(adapter, (u8 *)&sta_pparm);
#endif
		}
	}
}

#ifdef CONFIG_CMD_DISP
u8 set_stakey_hdl(struct _ADAPTER *a, struct set_stakey_parm *key,
					enum phl_cmd_type cmd_type,  u32 cmd_timeout)
{
	struct sta_priv *pstapriv = &a->stapriv;
	struct sta_info *sta;
	int err = 0;
	u8 ret = H2C_SUCCESS;
	u8 spp = 0;


	sta = rtw_get_stainfo(pstapriv, key->addr);
	if (!sta) {
		RTW_ERR("%s: sta %pM not found\n", __func__, key->addr);
		ret = H2C_REJECTED;
		goto exit;
	}

	a->mlmeextpriv.mlmext_info.enc_algo = key->algorithm;

	if (key->algorithm == _NO_PRIVACY_) {
		RTW_INFO("%s: del all key for %pM for _NO_PRIVACY_\n",
			 __func__, key->addr);
		err = rtw_hw_del_all_key(a, sta, PHL_CMD_DIRECTLY, 0);
	} else {
		RTW_INFO("%s: set %s key for %pM, kid:%d algo:%s\n",
			 __func__, key->gk?"group":"pairwise",
			 key->addr, key->keyid,
			 security_type_str(key->algorithm));
		if (amsdu_spp_enable(a, key->algorithm) == _SUCCESS)
			spp = 1;
#ifdef CONFIG_WAPI_SUPPORT
		if(key->algorithm == _SMS4_)
			sta->phl_sta->wapi = true;
#endif

		/* ToDo CONFIG_RTW_MLD: HALMAC API
		** pairwise key uses mld addr */
		err = rtw_hw_add_key(a, sta, key->keyid, key->algorithm,
				     key->gk, key->key, spp, PHL_CMD_DIRECTLY, 0);
	}
	if (!(key->gk))
		ATOMIC_INC(&sta->keytrack);	/*CVE-2020-24587*/
	if (err)
		RTW_ERR("%s: FAIL to set %s key for %pM, kid:%d algo:%s !\n",
			__func__, key->gk?"group":"pairwise",
			key->addr, key->keyid,
			security_type_str(key->algorithm));

	ret = H2C_SUCCESS_RSP;

exit:
	return ret;
}
#else /* CONFIG_FSM */
u8 set_stakey_hdl(struct _ADAPTER *a, u8 *pbuf)
{
	struct set_stakey_parm *key = (struct set_stakey_parm *)pbuf;
	struct sta_priv *pstapriv = &a->stapriv;
	struct sta_info *sta;
	int err = 0;
	u8 ret = H2C_SUCCESS;
	u8 spp = 0;


	sta = rtw_get_stainfo(pstapriv, key->addr);
	if (!sta) {
		RTW_ERR("%s: sta %pM not found\n", __func__, key->addr);
		ret = H2C_REJECTED;
		goto exit;
	}

	a->mlmeextpriv.mlmext_info.enc_algo = key->algorithm;

	if (key->algorithm == _NO_PRIVACY_) {
		RTW_INFO("%s: del all key for %pM for _NO_PRIVACY_\n",
			 __func__, key->addr);
		err = rtw_hw_del_all_key(a, sta, PHL_CMD_DIRECTLY, 0);
	} else {
		RTW_INFO("%s: set %s key for %pM, kid:%d algo:%s\n",
			 __func__, key->gk?"group":"pairwise",
			 key->addr, key->keyid,
			 security_type_str(key->algorithm));
		if (amsdu_spp_enable(a, key->algorithm) == _SUCCESS)
			spp = 1;
		err = rtw_hw_add_key(a, sta, key->keyid, key->algorithm,
				     key->gk, key->key, spp, PHL_CMD_DIRECTLY, 0);
	}
	if (!(key->gk))
		ATOMIC_INC(&sta->keytrack);	/*CVE-2020-24587*/

	if (err)
		RTW_ERR("%s: FAIL to set %s key for %pM, kid:%d algo:%s !\n",
			__func__, key->gk?"group":"pairwise",
			key->addr, key->keyid,
			security_type_str(key->algorithm));

	ret = H2C_SUCCESS_RSP;

exit:
	return ret;
}
#endif

u8 add_ba_hdl(_adapter *padapter, unsigned char *pbuf)
{
	struct addBaReq_parm	*pparm = (struct addBaReq_parm *)pbuf;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct sta_info *psta = rtw_get_stainfo(&padapter->stapriv, pparm->addr);
	struct _ADAPTER_LINK *padapter_link;

	if (!psta)
		return	H2C_SUCCESS;

	padapter_link = psta->padapter_link;

#ifdef CONFIG_80211N_HT
	if (((pmlmeinfo->state & WIFI_FW_ASSOC_SUCCESS)
		&& (padapter_link->mlmeextpriv.mlmext_info.HT_enable
#if CONFIG_IEEE80211_BAND_6GHZ
		|| psta->phl_sta->chandef.band == BAND_ON_6G
#endif
		)) || ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)) {
		/* pmlmeinfo->ADDBA_retry_count = 0; */
		/* pmlmeinfo->candidate_tid_bitmap |= (0x1 << pparm->tid);		 */
		/* psta->ampdu_priv.candidate_tid_bitmap |= BIT(pparm->tid); */
		issue_addba_req(padapter, pparm->addr, (u8)pparm->tid);
		_set_timer(&psta->addba_retry_timer, ADDBA_TO);
	}
#ifdef CONFIG_TDLS
	else if ((psta->tdls_sta_state & TDLS_LINKED_STATE) &&
		 (psta->htpriv.ht_option == _TRUE
#if CONFIG_IEEE80211_BAND_6GHZ
		  || psta->phl_sta->chandef.band == BAND_ON_6G
#endif
		 ) && (psta->ampdu_priv.ampdu_enable == _TRUE)) {
		issue_addba_req(padapter, pparm->addr, (u8)pparm->tid);
		_set_timer(&psta->addba_retry_timer, ADDBA_TO);
	}
#endif /* CONFIG */
	else
		psta->ampdu_priv.candidate_tid_bitmap &= ~BIT(pparm->tid);
#endif /* CONFIG_80211N_HT */
	return	H2C_SUCCESS;
}


u8 add_ba_rsp_hdl(_adapter *padapter, unsigned char *pbuf)
{
	struct addBaRsp_parm *pparm = (struct addBaRsp_parm *)pbuf;
	struct recv_reorder_ctrl *preorder_ctrl;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
	u8 ret = _TRUE;

	psta = rtw_get_stainfo(pstapriv, pparm->addr);
	if (!psta)
		goto exit;

	if (pparm->status == 0) {
		rtw_phl_start_rx_ba_session(padapter->dvobj->phl, psta->phl_sta,
					    pparm->preq.dialog_token, 3,
					    pparm->start_seq, 0, pparm->tid,
					    pparm->size);
	}

	preorder_ctrl = &psta->recvreorder_ctrl[pparm->tid];
	ret = issue_addba_rsp_wait_ack(padapter, pparm->addr, pparm->tid,
				       pparm->status, pparm->size,
				       &(pparm->preq), 3, 50);

#ifdef CONFIG_UPDATE_INDICATE_SEQ_WHILE_PROCESS_ADDBA_REQ
	/* status = 0 means accept this addba req, so update indicate seq = start_seq under this compile flag */
	if (pparm->status == 0) {
		preorder_ctrl->indicate_seq = pparm->start_seq;
		#ifdef DBG_RX_SEQ
		RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" tid:%u SN_UPDATE indicate_seq:%d, start_seq:%d\n"
			, FUNC_ADPT_ARG(padapter), preorder_ctrl->tid, preorder_ctrl->indicate_seq, pparm->start_seq);
		#endif
	}
#else
	rtw_set_bit(RTW_RECV_ACK_OR_TIMEOUT, &preorder_ctrl->rec_abba_rsp_ack);
	#ifdef DBG_RX_SEQ
	RTW_INFO("DBG_RX_SEQ "FUNC_ADPT_FMT" tid:%u SN_CLEAR indicate_seq:%d, "
                 "start_seq:%d preorder_ctrl->rec_abba_rsp_ack =%lu "
                 "pparm->preq.dialog_token=%d pparm->preq.ba_p_set=%d\n"
		, FUNC_ADPT_ARG(padapter)
		, preorder_ctrl->tid
		, preorder_ctrl->indicate_seq
		, pparm->start_seq
		, preorder_ctrl->rec_abba_rsp_ack
		, pparm->preq.dialog_token
		, pparm->preq.ba_p_set
		);
	#endif
#endif

	/*
	  * status = 0 means accept this addba req
	  * status = 37 means reject this addba req
	  */
	if (pparm->status == 0) {
		preorder_ctrl->enable = _TRUE;
		preorder_ctrl->ampdu_size = pparm->size;
	} else if (pparm->status == 37)
		preorder_ctrl->enable = _FALSE;

exit:
	return H2C_SUCCESS;
}

u8 delba_hdl(struct _ADAPTER *a, unsigned char *pbuf)
{
	struct addBaReq_parm *parm = (struct addBaReq_parm *)pbuf;
	struct sta_info *sta;


	sta = rtw_get_stainfo(&a->stapriv, parm->addr);
	if (!sta) {
		RTW_WARN(FUNC_ADPT_FMT ": No STA(" MAC_FMT ") for DELBA!\n",
			 FUNC_ADPT_ARG(a), MAC_ARG(parm->addr));
		return	H2C_SUCCESS;
	}

	rtw_phl_stop_rx_ba_session(a->dvobj->phl, sta->phl_sta, parm->tid);

	return	H2C_SUCCESS;
}

u8 chk_bmc_sleepq_cmd(_adapter *padapter)
{
	struct cmd_obj *cmd;
	struct cmd_priv *pcmdpriv = &(adapter_to_dvobj(padapter)->cmdpriv);
	u8 res = _SUCCESS;


	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmd->padapter = padapter;

	init_h2fwcmd_w_parm_no_parm_rsp(cmd, CMD_CHK_BMCSLEEPQ);

	res = rtw_enqueue_cmd(pcmdpriv, cmd);

exit:
	return res;
}

u8 set_tx_beacon_cmd(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 flags)
{
	struct cmd_obj	*cmd;
	struct Tx_Beacon_param	*ptxBeacon_parm;
	struct cmd_priv	*pcmdpriv = &(adapter_to_dvobj(padapter)->cmdpriv);
	struct mlme_ext_info	*pmlmeinfo = &(padapter->mlmeextpriv.mlmext_info);
	struct link_mlme_ext_priv	*lmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*lmlmeinfo = &(lmlmeext->mlmext_info);
	struct submit_ctx sctx;
	u8	res = _SUCCESS;
	int len_diff = 0;

	/*prepare cmd parameter*/
	ptxBeacon_parm = (struct Tx_Beacon_param *)rtw_zmalloc(sizeof(struct Tx_Beacon_param));
	if (ptxBeacon_parm == NULL) {
		res = _FAIL;
		goto exit;
	}

	_rtw_memcpy(&(ptxBeacon_parm->network), &(lmlmeinfo->network), sizeof(WLAN_BSSID_EX));

	len_diff = update_hidden_ssid(
			   ptxBeacon_parm->network.IEs + _BEACON_IE_OFFSET_
		   , ptxBeacon_parm->network.IELength - _BEACON_IE_OFFSET_
			   , pmlmeinfo->hidden_ssid_mode
		   );
	ptxBeacon_parm->network.IELength += len_diff;


	/* need enqueue, prepare cmd_obj and enqueue */
	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL) {
		res = _FAIL;
		rtw_mfree((u8 *)ptxBeacon_parm, sizeof(*ptxBeacon_parm));
		goto exit;
	}
	cmd->padapter = padapter;

	init_h2fwcmd_w_parm_no_rsp(cmd, ptxBeacon_parm, CMD_TX_BEACON);

	if (flags & RTW_CMDF_WAIT_ACK) {
		cmd->sctx = &sctx;
		rtw_sctx_init(&sctx, 10 * 1000);
	}

	res = rtw_enqueue_cmd(pcmdpriv, cmd);

	if (res == _SUCCESS && (flags & RTW_CMDF_WAIT_ACK)) {
		rtw_sctx_wait(&sctx, __func__);
		_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
		if (sctx.status == RTW_SCTX_SUBMITTED)
			cmd->sctx = NULL;
		_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
	}
exit:
	return res;
}

u8 mlme_evt_hdl(_adapter *padapter, unsigned char *pbuf)
{
	struct rtw_evt_header *evt_hdr;
	u8 *peventbuf;
	void (*event_callback)(_adapter *dev, u8 *pbuf);
	struct cmd_priv *pcmd_priv = &adapter_to_dvobj(padapter)->cmdpriv;

	if (pbuf == NULL)
		goto _abort_event_;

	evt_hdr = (struct rtw_evt_header *)pbuf;
	peventbuf = pbuf + sizeof(struct rtw_evt_header);

#ifdef CHECK_EVENT_SEQ
	/* checking event sequence...		 */
	if (evt_hdr->seq != (ATOMIC_READ(&pcmd_priv->event_seq) & 0x7f)) {
		pcmd_priv->event_seq = (evt_hdr->seq + 1) & 0x7f;
		goto _abort_event_;
	}
#endif

	/* checking if event code is valid */
	if (evt_hdr->id >= EVT_ID_MAX) {
		goto _abort_event_;
	}

	/* checking if event size match the event parm size	 */
	if ((wlanevents[evt_hdr->id].parmsize != 0) &&
	    (wlanevents[evt_hdr->id].parmsize != evt_hdr->len)) {

		goto _abort_event_;

	}

	ATOMIC_INC(&pcmd_priv->event_seq);

	if (peventbuf) {
		event_callback = wlanevents[evt_hdr->id].event_callback;
		event_callback(padapter, (u8 *)peventbuf);
		pcmd_priv->evt_done_cnt++;
	}

_abort_event_:
	return H2C_SUCCESS;

}

u8 chk_bmc_sleepq_hdl(_adapter *padapter, unsigned char *pbuf)
{
#ifdef CONFIG_AP_MODE
	struct sta_info *psta_bmc;
	_list	*xmitframe_plist, *xmitframe_phead;
	struct xmit_frame *pxmitframe = NULL;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct sta_priv  *pstapriv = &padapter->stapriv;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	/* for BC/MC Frames */
	psta_bmc = rtw_get_bcmc_stainfo(padapter, padapter_link);
	if (!psta_bmc)
		return H2C_SUCCESS;

	if ((rtw_tim_map_is_set(padapter, pstapriv->tim_bitmap, 0)) && (psta_bmc->sleepq_len > 0)) {
#ifndef CONFIG_PCI_HCI
		rtw_msleep_os(10);/* 10ms, ATIM(HIQ) Windows */
#endif
		/* _rtw_spinlock_bh(&psta_bmc->sleep_q.lock); */
		_rtw_spinlock_bh(&pxmitpriv->lock);

		xmitframe_phead = get_list_head(&psta_bmc->sleep_q);
		xmitframe_plist = get_next(xmitframe_phead);

		while ((rtw_end_of_queue_search(xmitframe_phead, xmitframe_plist)) == _FALSE) {
			pxmitframe = LIST_CONTAINOR(xmitframe_plist, struct xmit_frame, list);

			xmitframe_plist = get_next(xmitframe_plist);

			rtw_list_delete(&pxmitframe->list);

			psta_bmc->sleepq_len--;
			if (psta_bmc->sleepq_len > 0)
				pxmitframe->attrib.mdata = 1;
			else
				pxmitframe->attrib.mdata = 0;

			pxmitframe->attrib.triggered = 1;

#if 0

			if (xmitframe_hiq_filter(pxmitframe) == _TRUE)
				pxmitframe->attrib.qsel = rtw_hal_get_qsel(padapter,QSLT_HIGH_ID);/* HIQ */

			_rtw_spinunlock_bh(&psta_bmc->sleep_q.lock);
			if (rtw_hal_xmit(padapter, pxmitframe) == _TRUE)
				rtw_os_xmit_complete(padapter, pxmitframe);
			_rtw_spinlock_bh(&psta_bmc->sleep_q.lock);
#endif
			rtw_intf_xmitframe_enqueue(padapter, pxmitframe);
		}

		/* _rtw_spinunlock_bh(&psta_bmc->sleep_q.lock); */
		_rtw_spinunlock_bh(&pxmitpriv->lock);

		if (rtw_get_intf_type(padapter) != RTW_HCI_PCIE) {
			/* check hi queue and bmc_sleepq */
			rtw_chk_hi_queue_cmd(padapter);
		}
	}
#endif

	return H2C_SUCCESS;
}

u8 tx_beacon_hdl(_adapter *padapter, unsigned char *pbuf)
{
	/*RTW_INFO(FUNC_ADPT_FMT, FUNC_ADPT_ARG(padapter));*/
	if (send_beacon(padapter) == _FAIL) {
		RTW_INFO("issue_beacon, fail!\n");
		return H2C_PARAMETERS_ERROR;
	}

	/* tx bc/mc frames after update TIM */
	chk_bmc_sleepq_hdl(padapter, NULL);
	return H2C_SUCCESS;
}

/*
* according to channel
* add/remove WLAN_BSSID_EX.IEs's ERP ie
* set WLAN_BSSID_EX.SupportedRates
* update WLAN_BSSID_EX.IEs's Supported Rate and Extended Supported Rate ie
*/
void change_band_update_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			WLAN_BSSID_EX *pnetwork, struct rtw_chan_def *chandef)
{
	u8	network_type, rate_len, total_rate_len, remainder_rate_len;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(padapter);
	u8	erpinfo = 0x4;

	if (chandef->band == BAND_ON_5G ||
	    chandef->band == BAND_ON_6G) {
		network_type = WLAN_MD_11A;
		total_rate_len = IEEE80211_NUM_OFDM_RATESLEN;
		rtw_remove_bcn_ie(padapter, pnetwork, _ERPINFO_IE_);
		#ifdef CONFIG_80211AC_VHT
		/* if channel in 5G band, then add vht ie . */
		if ((pmlmepriv->htpriv.ht_option == _TRUE)
			&& REGSTY_IS_11AC_ENABLE(&padapter->registrypriv)
			&& is_supported_vht(padapter->registrypriv.wireless_mode)
			&& RFCTL_REG_EN_11AC(rfctl)
		) {
			if (pmlmepriv->upper_layer_setting)
				rtw_reattach_vht_ies(padapter, padapter_link, pnetwork);
			else if (REGSTY_IS_11AC_AUTO(&padapter->registrypriv))
				rtw_vht_ies_attach(padapter, padapter_link, pnetwork);
		}
		#endif
		#ifdef CONFIG_80211AX_HE
		/* CONFIG_80211AX_HE_TODO */
		#endif
	} else { /* BAND_ON_24G */
		network_type = 0;
		total_rate_len = 0;
		if (padapter->registrypriv.wireless_mode & WLAN_MD_11B) {
			network_type |= WLAN_MD_11B;
			total_rate_len += IEEE80211_CCK_RATE_LEN;
		}
		if (padapter->registrypriv.wireless_mode & WLAN_MD_11G) {
			network_type |= WLAN_MD_11G;
			total_rate_len += IEEE80211_NUM_OFDM_RATESLEN;
		}
		rtw_add_bcn_ie(padapter, pnetwork, _ERPINFO_IE_, &erpinfo, 1);
		#ifdef CONFIG_80211AC_VHT
		rtw_vht_ies_detach(padapter, padapter_link, pnetwork);
		#endif
		#ifdef CONFIG_80211AX_HE
		/* CONFIG_80211AX_HE_TODO */
		#endif
	}

	rtw_set_supported_rate(pnetwork->SupportedRates, network_type, chandef->chan);

	UpdateBrateTbl(padapter, pnetwork->SupportedRates);

	if (total_rate_len > 8) {
		rate_len = 8;
		remainder_rate_len = total_rate_len - 8;
	} else {
		rate_len = total_rate_len;
		remainder_rate_len = 0;
	}

	rtw_add_bcn_ie(padapter, pnetwork, _SUPPORTEDRATES_IE_, pnetwork->SupportedRates, rate_len);

	if (remainder_rate_len)
		rtw_add_bcn_ie(padapter, pnetwork, _EXT_SUPPORTEDRATES_IE_, (pnetwork->SupportedRates + 8), remainder_rate_len);
	else
		rtw_remove_bcn_ie(padapter, pnetwork, _EXT_SUPPORTEDRATES_IE_);

	pnetwork->Length = get_WLAN_BSSID_EX_sz(pnetwork);
}

void rtw_set_external_auth_status(_adapter *padapter,
	const void *data, int len)
{
#ifdef CONFIG_IOCTL_CFG80211
	struct net_device *dev = padapter->pnetdev;
	struct wiphy *wiphy = adapter_to_wiphy(padapter);
	struct rtw_external_auth_params params;

	/* convert data to external_auth_params */
	params.action = RTW_GET_BE32((u8 *)data);
	_rtw_memcpy(&params.bssid, (u8 *)data + 4, ETH_ALEN);
	_rtw_memcpy(&params.ssid.ssid, (u8 *)data + 10, WLAN_SSID_MAXLEN);
	params.ssid.ssid_len = RTW_GET_BE64((u8 *)data + 42);
	params.key_mgmt_suite = RTW_GET_BE32((u8 *)data + 58);
	params.status = RTW_GET_BE16((u8 *)data + 62);
	_rtw_memcpy(&params.pmkid, (u8 *)data + 64, PMKID_LEN);

	rtw_cfg80211_external_auth_status(wiphy, dev, &params);
#endif /* CONFIG_IOCTL_CFG80211 */
}

u8 rtw_set_chbw_hdl(_adapter *padapter, u8 *pbuf)
{
	struct set_ch_parm *set_ch_parm;
	struct _ADAPTER_LINK *padapter_link;
	struct link_mlme_ext_priv	*pmlmeext;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	if (!pbuf)
		return H2C_PARAMETERS_ERROR;

	set_ch_parm = (struct set_ch_parm *)pbuf;

	padapter_link = GET_LINK(padapter, set_ch_parm->link_idx);
	pmlmeext = &padapter_link->mlmeextpriv;

	RTW_INFO(FUNC_NDEV_FMT" ch:%u, bw:%u, ch_offset:%u\n",
		 FUNC_NDEV_ARG(padapter->pnetdev),
		 set_ch_parm->ch, set_ch_parm->bw, set_ch_parm->ch_offset);

	pmlmeext->chandef.band = set_ch_parm->band;
	pmlmeext->chandef.chan = set_ch_parm->ch;
	pmlmeext->chandef.offset = set_ch_parm->ch_offset;
	pmlmeext->chandef.bw = set_ch_parm->bw;

	set_bch_bwmode(padapter, padapter_link,
				set_ch_parm->band,
				set_ch_parm->ch,
				set_ch_parm->ch_offset,
				set_ch_parm->bw,
				set_ch_parm->do_rfk);
	rtw_rfctl_update_op_mode(dvobj_to_rfctl(dvobj), 0, 0);

	return	H2C_SUCCESS;
}

u8 led_blink_hdl(_adapter *padapter, unsigned char *pbuf)
{
	struct LedBlink_param *ledBlink_param;

	if (!pbuf)
		return H2C_PARAMETERS_ERROR;

	ledBlink_param = (struct LedBlink_param *)pbuf;

#ifdef CONFIG_RTW_LED_HANDLED_BY_CMD_THREAD
	BlinkHandler((PLED_DATA)ledBlink_param->pLed);
#endif

	return	H2C_SUCCESS;
}

u8 tdls_hdl(_adapter *padapter, unsigned char *pbuf)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
#ifdef CONFIG_TDLS
	struct tdls_info *ptdlsinfo = &padapter->tdlsinfo;
#ifdef CONFIG_TDLS_CH_SW
	struct tdls_ch_switch *pchsw_info = &ptdlsinfo->chsw_info;
#endif
	struct TDLSoption_param *TDLSoption;
	struct sta_info *ptdls_sta = NULL;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	struct sta_info *ap_sta = rtw_get_stainfo(&padapter->stapriv, get_my_bssid(&(pmlmeinfo->network)));
	u8 survey_channel, i, min, option;
	struct tdls_txmgmt txmgmt;
	u32 setchtime, resp_sleep = 0, wait_time;
	u8 zaddr[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	u8 ret;
	u8 do_rfk;
	struct sta_priv *pstapriv = &padapter->stapriv;
	void *phl = GET_PHL_INFO(adapter_to_dvobj(padapter));
	struct rtw_wifi_role_t *wrole = padapter->phl_role;
	enum role_type rtype = PHL_RTYPE_TDLS;
	enum rtw_phl_status status;
	u8 sta_band = 0;

	if (!pbuf)
		return H2C_PARAMETERS_ERROR;

	TDLSoption = (struct TDLSoption_param *)pbuf;
	option = TDLSoption->option;

	if (wrole->type != PHL_RTYPE_TDLS) {
		status = rtw_phl_cmd_wrole_change(phl, wrole, padapter_link->wrlink, WR_CHG_TYPE,
					(u8*)&rtype, sizeof(enum role_type), PHL_CMD_DIRECTLY, 0);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			RTW_ERR("%s - change to phl role type = %d fail with error = %d\n", __func__, rtype, status);
			rtw_warn_on(1);
		}
	}

	if (!_rtw_memcmp(TDLSoption->addr, zaddr, ETH_ALEN)) {
		ptdls_sta = rtw_get_stainfo(&(padapter->stapriv), TDLSoption->addr);
		if (ptdls_sta == NULL)
			return H2C_REJECTED;
	}

	/* _rtw_spinlock_bh(&(ptdlsinfo->hdl_lock)); */
	/* RTW_INFO("[%s] option:%d\n", __FUNCTION__, option); */

	switch (option) {
	case TDLS_ESTABLISHED:

		rtw_alloc_stainfo_hw(&(padapter->stapriv), ptdls_sta);

		update_sta_ra_info(padapter, ptdls_sta);

#ifdef CONFIG_80211N_HT
#ifdef CONFIG_80211AC_VHT
#ifdef CONFIG_80211AX_HE
		if (ptdls_sta->hepriv.he_option == _TRUE)
			sta_band = WLAN_MD_11AX;
		else
#endif
		if (ptdls_sta->vhtpriv.vht_option == _TRUE)
			sta_band = WLAN_MD_11AC;
		else
#endif
		if (ptdls_sta->htpriv.ht_option == _TRUE)
			sta_band = WLAN_MD_11N;
#endif
		if (pmlmeext->chandef.chan > 14)
			sta_band |= WLAN_MD_11A;
		else {
			if ((cckratesonly_included(ptdls_sta->bssrateset, ptdls_sta->bssratelen)) == _TRUE)
				sta_band |= WLAN_MD_11B;
			else if ((cckrates_included(ptdls_sta->bssrateset, ptdls_sta->bssratelen)) == _TRUE)
				sta_band |= WLAN_MD_11BG;
			else
				sta_band |= WLAN_MD_11G;
		}
		ptdls_sta->phl_sta->wmode = sta_band & padapter->registrypriv.wireless_mode;

		/* ref: top-half of rtw_hw_connected() */
		rtw_update_phl_sta_cap(padapter, ptdls_sta, &ptdls_sta->phl_sta->asoc_cap);

		status = rtw_phl_cmd_update_media_status(phl, ptdls_sta->phl_sta,
							 ptdls_sta->phl_sta->mac_addr, true,
							 PHL_CMD_DIRECTLY, 0);

		if (padapter->securitypriv.dot11PrivacyAlgrthm != _NO_PRIVACY_)
			rtw_tdls_set_key(padapter, ptdls_sta);

		/* Inform PHL for phl_mr_info_upt */
		rtw_send_tdls_sync_msg(padapter);
		break;
	case TDLS_ISSUE_PTI:
		ptdls_sta->tdls_sta_state |= TDLS_WAIT_PTR_STATE;
		issue_tdls_peer_traffic_indication(padapter, ptdls_sta);
		_set_timer(&ptdls_sta->pti_timer, TDLS_PTI_TIME);
		break;
#ifdef CONFIG_TDLS_CH_SW
	case TDLS_CH_SW_RESP:
		_rtw_memset(&txmgmt, 0x00, sizeof(struct tdls_txmgmt));
		txmgmt.status_code = 0;
		_rtw_memcpy(txmgmt.peer, ptdls_sta->phl_sta->mac_addr, ETH_ALEN);

		if (ap_sta)
			rtw_hal_macid_sleep(padapter, ap_sta->phl_sta->macid);
		issue_nulldata(padapter, NULL, 1, 3, 3);

		RTW_INFO("[TDLS ] issue tdls channel switch response\n");
		ret = issue_tdls_ch_switch_rsp(padapter, &txmgmt, _TRUE);

		/* If we receive TDLS_CH_SW_REQ at off channel which it's target is AP's channel */
		/* then we just switch to AP's channel*/
		if (padapter->mlmeextpriv.chandef.chan == pchsw_info->off_ch_num) {
			rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_CH_SW_END_TO_BASE_CHNL);
			break;
		}

		if (ret == _SUCCESS)
			rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_CH_SW_TO_OFF_CHNL);
		else
			RTW_INFO("[TDLS] issue_tdls_ch_switch_rsp wait ack fail !!!!!!!!!!\n");

		break;
	case TDLS_CH_SW_PREPARE:
		pchsw_info->ch_sw_state |= TDLS_CH_SWITCH_PREPARE_STATE;

		/* to collect IQK info of off-chnl */
		do_rfk = _TRUE;
		set_channel_bwmode(padapter,
				pchsw_info->off_ch_num,
				pchsw_info->ch_offset,
				(pchsw_info->ch_offset) ? CHANNEL_WIDTH_40 : CHANNEL_WIDTH_20,
				do_rfk);

		do_rfk = _FALSE;
		/* switch back to base-chnl */
		set_channel_bwmode(padapter,
				pmlmeext->chandef.chan,
				pmlmeext->chandef.offset,
				pmlmeext->chandef.bw,
				do_rfk);

		rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_CH_SW_START);

		pchsw_info->ch_sw_state &= ~(TDLS_CH_SWITCH_PREPARE_STATE);

		break;
	case TDLS_CH_SW_START:
		rtw_tdls_set_ch_sw_oper_control(padapter, _TRUE);
		break;
	case TDLS_CH_SW_TO_OFF_CHNL:
		if (ap_sta)
			rtw_hal_macid_sleep(padapter, ap_sta->phl_sta->macid);
		issue_nulldata(padapter, NULL, 1, 3, 3);

		if (padapter->registrypriv.wifi_spec == 0) {
		if (!(pchsw_info->ch_sw_state & TDLS_CH_SW_INITIATOR_STATE))
			_set_timer(&ptdls_sta->ch_sw_timer, (u32)(ptdls_sta->ch_switch_timeout) / 1000);
		}

		if (rtw_tdls_do_ch_sw(padapter, ptdls_sta, TDLS_CH_SW_OFF_CHNL, pchsw_info->off_ch_num,
			pchsw_info->ch_offset, (pchsw_info->ch_offset) ? CHANNEL_WIDTH_40 : CHANNEL_WIDTH_20, ptdls_sta->ch_switch_time) == _SUCCESS) {
			pchsw_info->ch_sw_state &= ~(TDLS_PEER_AT_OFF_STATE);
			if (pchsw_info->ch_sw_state & TDLS_CH_SW_INITIATOR_STATE) {
				if (issue_nulldata_to_TDLS_peer_STA(ptdls_sta->padapter, ptdls_sta->phl_sta->mac_addr, 0, 1, 
					(padapter->registrypriv.wifi_spec == 0) ? 3 : 0) == _FAIL)
					rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_CH_SW_TO_BASE_CHNL);
			}
		} else {
			if (!(pchsw_info->ch_sw_state & TDLS_CH_SW_INITIATOR_STATE))
				_cancel_timer_ex(&ptdls_sta->ch_sw_timer);
		}


		break;
	case TDLS_CH_SW_END:
	case TDLS_CH_SW_END_TO_BASE_CHNL:
		rtw_tdls_set_ch_sw_oper_control(padapter, _FALSE);
		_cancel_timer_ex(&ptdls_sta->ch_sw_timer);
		_cancel_timer_ex(&ptdls_sta->stay_on_base_chnl_timer);
		_cancel_timer_ex(&ptdls_sta->ch_sw_monitor_timer);
#if 0
		_rtw_memset(pHalData->tdls_ch_sw_iqk_info_base_chnl, 0x00, sizeof(pHalData->tdls_ch_sw_iqk_info_base_chnl));
		_rtw_memset(pHalData->tdls_ch_sw_iqk_info_off_chnl, 0x00, sizeof(pHalData->tdls_ch_sw_iqk_info_off_chnl));
#endif

		if (option == TDLS_CH_SW_END_TO_BASE_CHNL)
			rtw_tdls_cmd(padapter, ptdls_sta->phl_sta->mac_addr, TDLS_CH_SW_TO_BASE_CHNL);

		break;
	case TDLS_CH_SW_TO_BASE_CHNL_UNSOLICITED:
	case TDLS_CH_SW_TO_BASE_CHNL:
		pchsw_info->ch_sw_state &= ~(TDLS_PEER_AT_OFF_STATE | TDLS_WAIT_CH_RSP_STATE);

		if (option == TDLS_CH_SW_TO_BASE_CHNL_UNSOLICITED) {
			if (ptdls_sta != NULL) {
				/* Send unsolicited channel switch rsp. to peer */
				_rtw_memset(&txmgmt, 0x00, sizeof(struct tdls_txmgmt));
				txmgmt.status_code = 0;
				_rtw_memcpy(txmgmt.peer, ptdls_sta->phl_sta->mac_addr, ETH_ALEN);
				issue_tdls_ch_switch_rsp(padapter, &txmgmt, _FALSE);
			}
		}

		if (rtw_tdls_do_ch_sw(padapter, ptdls_sta, TDLS_CH_SW_BASE_CHNL, pmlmeext->chandef.chan,
			pmlmeext->chandef.offset, pmlmeext->chandef.bw, ptdls_sta->ch_switch_time) == _SUCCESS) {
			if (ap_sta)
				rtw_hal_macid_wakeup(padapter, ap_sta->phl_sta->macid);
			issue_nulldata(padapter, NULL, 0, 3, 3);
			/* set ch sw monitor timer for responder */
			if (!(pchsw_info->ch_sw_state & TDLS_CH_SW_INITIATOR_STATE))
				_set_timer(&ptdls_sta->ch_sw_monitor_timer, TDLS_CH_SW_MONITOR_TIMEOUT);
		}

		break;
#endif
	case TDLS_TEARDOWN_STA:
	case TDLS_TEARDOWN_STA_TOOFAR:
	case TDLS_TEARDOWN_STA_NO_WAIT:
		_rtw_memset(&txmgmt, 0x00, sizeof(struct tdls_txmgmt));
		if (option == TDLS_TEARDOWN_STA_TOOFAR)
			txmgmt.status_code = _RSON_TDLS_TEAR_TOOFAR_;
		else
			txmgmt.status_code = _RSON_TDLS_TEAR_UN_RSN_;
		_rtw_memcpy(txmgmt.peer, ptdls_sta->phl_sta->mac_addr, ETH_ALEN);

		issue_tdls_teardown(padapter, &txmgmt, (option == TDLS_TEARDOWN_STA) ? _TRUE : _FALSE);

		break;
	case TDLS_TEARDOWN_STA_LOCALLY:
	case TDLS_TEARDOWN_STA_LOCALLY_POST:
#ifdef CONFIG_TDLS_CH_SW
		if (_rtw_memcmp(TDLSoption->addr, pchsw_info->addr, ETH_ALEN) == _TRUE) {
			pchsw_info->ch_sw_state &= ~(TDLS_CH_SW_INITIATOR_STATE |
						     TDLS_CH_SWITCH_ON_STATE |
						     TDLS_PEER_AT_OFF_STATE);
			rtw_tdls_set_ch_sw_oper_control(padapter, _FALSE);
			_rtw_memset(pchsw_info->addr, 0x00, ETH_ALEN);
		}
#endif

		if (option == TDLS_TEARDOWN_STA_LOCALLY)
			rtw_tdls_teardown_pre_hdl(padapter, ptdls_sta);

		rtw_tdls_teardown_post_hdl(padapter, ptdls_sta, _FALSE);

		if (ptdlsinfo->tdls_sctx != NULL)
			rtw_sctx_done(&(ptdlsinfo->tdls_sctx));

		break;
	}

	/* _rtw_spinunlock_bh(&(ptdlsinfo->hdl_lock)); */

	return H2C_SUCCESS;
#else
	return H2C_REJECTED;
#endif /* CONFIG_TDLS */

}

u8 run_in_thread_hdl(_adapter *padapter, u8 *pbuf)
{
	struct RunInThread_param *p;


	if (NULL == pbuf)
		return H2C_PARAMETERS_ERROR;
	p = (struct RunInThread_param *)pbuf;

	if (p->func)
		p->func(p->context);

	return H2C_SUCCESS;
}

int rtw_sae_preprocess(_adapter *adapter, const u8 *buf, u32 len, u8 tx)
{
#ifdef CONFIG_IOCTL_CFG80211
	const u8 *frame_body = buf + sizeof(struct rtw_ieee80211_hdr_3addr);
	u16 alg;
	u16 seq;
	u16 status;
	int ret = _FAIL;

	alg = RTW_GET_LE16(frame_body);
	if (alg != WLAN_AUTH_SAE)
		goto exit;

	seq = RTW_GET_LE16(frame_body + 2);
	status = RTW_GET_LE16(frame_body + 4);

	RTW_INFO("RTW_%s:AUTH alg:0x%04x, seq:0x%04x, status:0x%04x, mesg:%s\n",
		(tx == _TRUE) ? "Tx" : "Rx", alg, seq, status,
		(seq == 1) ? "Commit" : "Confirm");

	ret = _SUCCESS;

#ifdef CONFIG_RTW_MESH
	if (MLME_IS_MESH(adapter)) {
		rtw_mesh_sae_check_frames(adapter, buf, len, tx, alg, seq, status);
		goto exit;
	}
#endif

	if (tx && (seq == 2) && (status == 0) && MLME_IS_AP(adapter)) {
		/* queue confirm frame until external auth status update */
		struct sta_priv *pstapriv = &adapter->stapriv;
		struct sta_info	*psta = NULL;

		psta = rtw_get_stainfo(pstapriv, GetAddr1Ptr(buf));
		if (psta) {
			_rtw_spinlock_bh(&psta->lock);
			if (psta->pauth_frame) {
				rtw_mfree(psta->pauth_frame, psta->auth_len);
				psta->pauth_frame = NULL;
				psta->auth_len = 0;
			}

			psta->pauth_frame =  rtw_zmalloc(len);
			if (psta->pauth_frame) {
				_rtw_memcpy(psta->pauth_frame, buf, len);
				psta->auth_len = len;
			}
			_rtw_spinunlock_bh(&psta->lock);

			ret = 2;
		}
	}
exit:
	return ret;
#else
	return _SUCCESS;
#endif /* CONFIG_IOCTL_CFG80211 */
}

char UNKNOWN_EVT[16] = "UNKNOWN_EVT";
char *rtw_evt_name(struct rtw_evt_header *pev)
{
	if (pev->id >= EVT_ID_MAX)
		return UNKNOWN_EVT;

	return wlanevents[pev->id].name;
}

