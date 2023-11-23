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
#define _RTW_MP_C_
#include <drv_types.h>
#ifdef PLATFORM_FREEBSD
	#include <sys/unistd.h>		/* for RFHIGHPID */
#endif


#ifdef CONFIG_MP_VHT_HW_TX_MODE
#define CEILING_POS(X) ((X - (int)(X)) > 0 ? (int)(X + 1) : (int)(X))
#define CEILING_NEG(X) ((X - (int)(X)) < 0 ? (int)(X - 1) : (int)(X))
#define ceil(X) (((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X))

int rtfloor(float x)
{
	int i = x - 2;
	while
	(++i <= x - 1)
		;
	return i;
}
#endif

#ifdef CONFIG_MP_INCLUDED
bool rtw_mp_is_cck_rate(u16 rate)
{
	return ((rate & 0x1ff) <= _11M_RATE_) ? true : false;
}

static void _init_mp_priv_(struct mp_priv *pmp_priv)
{
	WLAN_BSSID_EX *pnetwork;

	_rtw_memset(pmp_priv, 0, sizeof(struct mp_priv));

	pmp_priv->mode = MP_OFF;

	pmp_priv->band = 0;
	pmp_priv->channel = 1;
	pmp_priv->bandwidth = CHANNEL_WIDTH_20;
	pmp_priv->prime_channel_offset = CHAN_OFFSET_NO_EXT;
	//pmp_priv->rateidx = RATE_1M;
	pmp_priv->txpoweridx = 0;
	pmp_priv->antenna_tx = MP_ANTENNA_A;
	pmp_priv->antenna_rx = MP_ANTENNA_A;
	pmp_priv->antenna_trx = MP_ANTENNA_A;

	pmp_priv->check_mp_pkt = 0;

	pmp_priv->tx_pktcount = 0;

	pmp_priv->rx_bssidpktcount = 0;
	pmp_priv->rx_pktcount = 0;
	pmp_priv->rx_crcerrpktcount = 0;

	pmp_priv->network_macaddr[0] = 0x00;
	pmp_priv->network_macaddr[1] = 0xE0;
	pmp_priv->network_macaddr[2] = 0x4C;
	pmp_priv->network_macaddr[3] = 0x87;
	pmp_priv->network_macaddr[4] = 0x66;
	pmp_priv->network_macaddr[5] = 0x55;

	pmp_priv->bSetRxBssid = _FALSE;
	pmp_priv->bRTWSmbCfg = _FALSE;
	pmp_priv->bloopback = _FALSE;

	pmp_priv->bloadefusemap = _FALSE;
	pmp_priv->brx_filter_beacon = _FALSE;
	pmp_priv->mplink_brx = _FALSE;
	pmp_priv->mp_keep_btc_mode = BTC_MODE_MAX;

	pnetwork = &pmp_priv->mp_network.network;
	_rtw_memcpy(pnetwork->MacAddress, pmp_priv->network_macaddr, ETH_ALEN);

	pnetwork->Ssid.SsidLength = 8;
	_rtw_memcpy(pnetwork->Ssid.Ssid, "mp_871x", pnetwork->Ssid.SsidLength);

	pmp_priv->tx.payload = MP_TX_Payload_default_random;
#ifdef CONFIG_80211N_HT
	pmp_priv->tx.attrib.ht_en = 1;
#endif

	pmp_priv->mpt_ctx.mpt_rate_index = 1;

}


static void mp_init_xmit_attrib(struct mp_tx *pmptx, _adapter *padapter)
{
	struct pkt_attrib *pattrib;

	/* init xmitframe attribute */
	pattrib = &pmptx->attrib;
	_rtw_memset(pattrib, 0, sizeof(struct pkt_attrib));
	_rtw_memset(pmptx->desc, 0, TXDESC_SIZE);

	pattrib->ether_type = 0x8712;
#if 0
	_rtw_memcpy(pattrib->src, adapter_mac_addr(padapter), ETH_ALEN);
	_rtw_memcpy(pattrib->ta, pattrib->src, ETH_ALEN);
#endif
	_rtw_memset(pattrib->dst, 0xFF, ETH_ALEN);

	/*	pattrib->dhcp_pkt = 0;
	 *	pattrib->pktlen = 0; */
	pattrib->ack_policy = 0;
	/*	pattrib->pkt_hdrlen = ETH_HLEN; */
	pattrib->hdrlen = WLAN_HDR_A3_LEN;
	pattrib->subtype = WIFI_DATA;
	pattrib->priority = 0;
	/*pattrib->qsel = pattrib->priority;*/
	/*	do_queue_select(padapter, pattrib); */
	pattrib->nr_frags = 1;
	pattrib->encrypt = 0;
	pattrib->bswenc = _FALSE;
	pattrib->qos_en = _FALSE;

	pattrib->pktlen = 1500;
}

s32 init_mp_priv(_adapter *padapter)
{
	struct mp_priv *pmppriv = &padapter->mppriv;

	_init_mp_priv_(pmppriv);
	pmppriv->papdater = padapter;
	if (0) //(IS_HARDWARE_TYPE_8822C(padapter))
		pmppriv->mp_dm = 1;/* default enable dpk tracking */
	else
		pmppriv->mp_dm = 0;

	pmppriv->tx.stop = 1;
	pmppriv->bSetTxPower = 0;		/*for  manually set tx power*/
	pmppriv->bTxBufCkFail = _FALSE;
	pmppriv->pktInterval = 100;
	pmppriv->pktLength = 1000;
	pmppriv->bprocess_mp_mode = _FALSE;
	pmppriv->rtw_mp_tx_method = RTW_MP_PMACT_TX;
	pmppriv->rtw_mp_tx_state = 0;
	pmppriv->rtw_mp_cur_phy = 0;
	pmppriv->rtw_mp_pmact_patt_idx = 0;
	pmppriv->rtw_mp_pmact_ppdu_type = 0;
	pmppriv->rtw_mp_dbcc = 0;
	pmppriv->pre_refcw_cck_pwridxa = 0;
	pmppriv->pre_refcw_cck_pwridxb = 0;
	pmppriv->pre_refcw_ofdm_pwridxa = 0;
	pmppriv->pre_refcw_ofdm_pwridxb = 0;
	pmppriv->rtw_mp_data_bandwidth = CHANNEL_WIDTH_20;
	pmppriv->rtw_mp_tx_time = 0;
	pmppriv->rtw_mp_trxsc = 0;
	pmppriv->rtw_mp_stbc = 0;
	pmppriv->rtw_mp_he_sigb = 0;
	pmppriv->rtw_mp_he_sigb_dcm = 0;
	pmppriv->rtw_mp_plcp_gi = 1;
	pmppriv->rtw_mp_plcp_ltf = 0;
	pmppriv->rtw_mp_plcp_tx_time = 0;
	pmppriv->rtw_mp_plcp_tx_mode = 0;
	pmppriv->rtw_mp_plcp_tx_user = 1;
	pmppriv->rtw_mp_he_er_su_ru_106_en = 0;
	pmppriv->rtw_mp_ru_tone = MP_RU_TONE_26;

	pmppriv->mp_plcp_useridx = 0;
	pmppriv->mp_plcp_user[0].plcp_mcs = 0;
	pmppriv->mp_plcp_user[0].aid = 0;
	pmppriv->mp_plcp_user[0].coding = 0;
	pmppriv->mp_plcp_user[0].dcm = 0;
	pmppriv->mp_plcp_user[0].plcp_txlen = 1000;
	pmppriv->mp_plcp_user[0].ru_alloc = 0;
	pmppriv->mp_plcp_user[0].txbf = 0;
	pmppriv->mp_plcp_user[0].pwr_boost_db = 0;

	pmppriv->btc_path = BTC_MODE_WL;

	_rtw_memset(pmppriv->st_giltf, 0, sizeof(struct rtw_mp_giltf_data)*5);

	mp_init_xmit_attrib(&pmppriv->tx, padapter);
#if 0
	switch (GET_HAL_RFPATH(padapter)) {
	case RF_1T1R:
		pmppriv->antenna_tx = ANTENNA_A;
		pmppriv->antenna_rx = ANTENNA_A;
		break;
	case RF_1T2R:
	default:
		pmppriv->antenna_tx = ANTENNA_A;
		pmppriv->antenna_rx = ANTENNA_AB;
		break;
	case RF_2T2R:
		pmppriv->antenna_tx = ANTENNA_AB;
		pmppriv->antenna_rx = ANTENNA_AB;
		break;
	case RF_2T4R:
		pmppriv->antenna_tx = ANTENNA_BC;
		pmppriv->antenna_rx = ANTENNA_ABCD;
		break;
	}
#endif

	return _SUCCESS;
}

void free_mp_priv(struct mp_priv *pmp_priv)
{
	if (pmp_priv->pallocated_mp_xmitframe_buf) {
		rtw_mfree(pmp_priv->pallocated_mp_xmitframe_buf, 0);
		pmp_priv->pallocated_mp_xmitframe_buf = NULL;
	}
	pmp_priv->pmp_xmtframe_buf = NULL;
}

#if 0
static void PHY_IQCalibrate_default(
		_adapter *adapter,
		BOOLEAN	bReCovery
)
{
	RTW_INFO("%s\n", __func__);
}

static void PHY_LCCalibrate_default(
		_adapter *adapter
)
{
	RTW_INFO("%s\n", __func__);
}

static void PHY_SetRFPathSwitch_default(
		_adapter *adapter,
		BOOLEAN		bMain
)
{
	RTW_INFO("%s\n", __func__);
}
#endif

void mpt_InitHWConfig(_adapter *adapter)
{
#ifdef CONFIG_RTL8822B
	if (IS_HARDWARE_TYPE_8822B(adapter)) {
		u32 tmp_reg = 0;

		rtw_write16(adapter, REG_RXFLTMAP1_8822B, 0x2000);
		/* fixed wifi can't 2.4g tx suggest by Szuyitasi 20160504 */
		phy_set_bb_reg(adapter, 0x70, bMaskByte3, 0x0e);
		RTW_INFO(" 0x73 = 0x%x\n", phy_query_bb_reg(adapter, 0x70, bMaskByte3));
		phy_set_bb_reg(adapter, 0x1704, bMaskDWord, 0x0000ff00);
		RTW_INFO(" 0x1704 = 0x%x\n", phy_query_bb_reg(adapter, 0x1704, bMaskDWord));
		phy_set_bb_reg(adapter, 0x1700, bMaskDWord, 0xc00f0038);
		RTW_INFO(" 0x1700 = 0x%x\n", phy_query_bb_reg(adapter, 0x1700, bMaskDWord));
	}
#endif /* CONFIG_RTL8822B */
#ifdef CONFIG_RTL8821C
	if (IS_HARDWARE_TYPE_8821C(adapter))
		rtw_write16(adapter, REG_RXFLTMAP1_8821C, 0x2000);
#endif /* CONFIG_RTL8821C */
#if defined(CONFIG_RTL8822C)
	if( IS_HARDWARE_TYPE_8822C(adapter)) {
		rtw_write16(adapter, REG_RXFLTMAP1_8822C, 0x2000);
		/* 0x7D8[31] : time out enable when cca is not assert
			0x60D[7:0] : time out value (Unit : us)*/
		rtw_write8(adapter, 0x7db, 0xc0);
		RTW_INFO(" 0x7d8 = 0x%x\n", rtw_read8(adapter, 0x7d8));
		rtw_write8(adapter, 0x60d, 0x0c);
		RTW_INFO(" 0x60d = 0x%x\n", rtw_read8(adapter, 0x60d));
		phy_set_bb_reg(adapter, 0x1c44, BIT10, 0x1);
		RTW_INFO(" 0x1c44 = 0x%x\n", phy_query_bb_reg(adapter, 0x1c44, bMaskDWord));
	}
#endif
#if defined(CONFIG_RTL8814B)
	if(IS_HARDWARE_TYPE_8814B(adapter))
		rtw_write16(adapter, REG_RXFLTMAP1_8814B, 0x2000);
#endif

}

static void PHY_IQCalibrate(_adapter *padapter, u8 bReCovery)
{
	//halrf_iqk_trigger(adapter_to_phydm(padapter), bReCovery);
}

static void PHY_LCCalibrate(_adapter *padapter)
{
	//halrf_lck_trigger(adapter_to_phydm(padapter));
}

static u8 PHY_QueryRFPathSwitch(_adapter *padapter)
{
	u8 bmain = 0;


	return bmain;
}

static void  PHY_SetRFPathSwitch(_adapter *padapter , BOOLEAN bMain) {

}


static void phy_switch_rf_path_set(_adapter *padapter , u8 *prf_set_State) {
#ifdef CONFIG_RTL8821C
	struct dm_struct *phydm = adapter_to_phydm(padapter);

	if (IS_HARDWARE_TYPE_8821C(padapter)) {
		config_phydm_set_ant_path(phydm, *prf_set_State, phydm->current_ant_num_8821c);
		/* Do IQK when switching to BTG/WLG, requested by RF Binson */
		if (*prf_set_State == SWITCH_TO_BTG || *prf_set_State == SWITCH_TO_WLG)
			PHY_IQCalibrate(padapter, FALSE);
	}
#endif

}


s32
MPT_InitializeAdapter(
		_adapter *adapter,
		u8				Channel
)
{
	s32		rtStatus = _SUCCESS;
	PMPT_CONTEXT	pMptCtx = &adapter->mppriv.mpt_ctx;
	/*u32		ledsetting;*/

	pMptCtx->bMptDrvUnload = _FALSE;
	pMptCtx->bMassProdTest = _FALSE;
	pMptCtx->bMptIndexEven = _TRUE;	/* default gain index is -6.0db */
	pMptCtx->h2cReqNum = 0x0;
	/* init for BT MP */
	mpt_InitHWConfig(adapter);

	pMptCtx->bMptWorkItemInProgress = _FALSE;
	pMptCtx->CurrMptAct = NULL;
	pMptCtx->mpt_rf_path = RF_PATH_A;
	/* ------------------------------------------------------------------------- */
	/* Don't accept any packets */
	//rtw_write32(adapter, REG_RCR, 0);

	/* ledsetting = rtw_read32(adapter, REG_LEDCFG0); */
	/* rtw_write32(adapter, REG_LEDCFG0, ledsetting & ~LED0DIS); */

	/* rtw_write32(adapter, REG_LEDCFG0, 0x08080); */
	/*ledsetting = rtw_read32(adapter, REG_LEDCFG0);*/


	PHY_LCCalibrate(adapter);
	PHY_IQCalibrate(adapter, _FALSE);
	/* dm_check_txpowertracking(adapter_to_phydm(adapter));	*/ /* trigger thermal meter */

	PHY_SetRFPathSwitch(adapter, 1/*pHalData->bDefaultAntenna*/); /* default use Main */
#if 0
	pMptCtx->backup0xc50 = (u8)phy_query_bb_reg(adapter, rOFDM0_XAAGCCore1, bMaskByte0);
	pMptCtx->backup0xc58 = (u8)phy_query_bb_reg(adapter, rOFDM0_XBAGCCore1, bMaskByte0);
	pMptCtx->backup0xc30 = (u8)phy_query_bb_reg(adapter, rOFDM0_RxDetector1, bMaskByte0);
	pMptCtx->backup0x52_RF_A = (u8)phy_query_rf_reg(adapter, RF_PATH_A, RF_0x52, 0x000F0);
	pMptCtx->backup0x52_RF_B = (u8)phy_query_rf_reg(adapter, RF_PATH_B, RF_0x52, 0x000F0);
#endif	
	return	rtStatus;
}

/*-----------------------------------------------------------------------------
 * Function:	MPT_DeInitAdapter()
 *
 * Overview:	Extra DeInitialization for Mass Production Test.
 *
 * Input:		_adapter *adapter
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	05/08/2007	MHC		Create Version 0.
 *	05/18/2007	MHC		Add normal driver MPHalt code.
 *
 *---------------------------------------------------------------------------*/
void
MPT_DeInitAdapter(
		_adapter *adapter
)
{
	PMPT_CONTEXT		pMptCtx = &adapter->mppriv.mpt_ctx;
	pMptCtx->bMptDrvUnload = _TRUE;
}

static u8 mpt_ProStartTest(_adapter *padapter)
{
	PMPT_CONTEXT pMptCtx = &padapter->mppriv.mpt_ctx;

	pMptCtx->bMassProdTest = _TRUE;
	pMptCtx->is_start_cont_tx = _FALSE;
	pMptCtx->bCckContTx = _FALSE;
	pMptCtx->bOfdmContTx = _FALSE;
	pMptCtx->bSingleCarrier = _FALSE;
	pMptCtx->is_carrier_suppression = _FALSE;
	pMptCtx->is_single_tone = _FALSE;
	pMptCtx->HWTxmode = PACKETS_TX;

	return _SUCCESS;
}

void rtw_mp_cal_trigger(_adapter *padapter, u8 cal_tye)
{
	struct rtw_mp_cal_arg	*mp_cal_arg = NULL;
	struct mp_priv *pmppriv = &padapter->mppriv;

	mp_cal_arg = _rtw_malloc(sizeof(struct rtw_mp_cal_arg));
	if (mp_cal_arg)
		_rtw_memset((void *)mp_cal_arg, 0, sizeof(struct rtw_mp_cal_arg));
	else {
		RTW_ERR("%s,malloc fail ! ", __func__);
		return;
	}
	mp_cal_arg->cal_type = cal_tye;

	if (cal_tye == RTW_MP_CAL_DPK_TRACK) {
		if (rtw_mp_phl_calibration(padapter,
					mp_cal_arg,
					RTW_MP_CAL_CMD_TRIGGER_DPK_TRACKING))
			RTW_INFO("%s ok\n", __func__);
		else
			RTW_INFO("%s fail\n", __func__);
	} else {
		if (rtw_mp_phl_calibration(padapter,
					mp_cal_arg,
					RTW_MP_CAL_CMD_TRIGGER_CAL))
			RTW_INFO("%s ok\n", __func__);
		else
			RTW_INFO("%s fail\n", __func__);
	}
	if (mp_cal_arg)
		_rtw_mfree(mp_cal_arg, sizeof(struct rtw_mp_cal_arg));

	return;
}

void rtw_mp_cal_capab(_adapter *padapter, u8 cal_tye, u8 benable)
{
	struct rtw_mp_cal_arg	*mp_cal_arg = NULL;
	struct mp_priv *pmppriv = &padapter->mppriv;

	mp_cal_arg = _rtw_malloc(sizeof(struct rtw_mp_cal_arg));
	if (mp_cal_arg)
		_rtw_memset((void *)mp_cal_arg, 0, sizeof(struct rtw_mp_cal_arg));
	else {
		RTW_ERR("%s,malloc fail ! ", __func__);
		return;
	}

	mp_cal_arg->cal_type = cal_tye;
	mp_cal_arg->enable = benable;

	if (rtw_mp_phl_calibration(padapter, mp_cal_arg, RTW_MP_CAL_CMD_SET_CAPABILITY_CAL))
		RTW_INFO("%s ok\n", __func__);
	else
		RTW_INFO("%s fail\n", __func__);

	if (mp_cal_arg)
		_rtw_mfree(mp_cal_arg, sizeof(struct rtw_mp_cal_arg));

	return;
}

/*
 * General use
 */
s32 SetPowerTracking(_adapter *padapter, u8 enable)
{

	//hal_mpt_SetPowerTracking(padapter, enable);
	return 0;
}

void GetPowerTracking(_adapter *padapter, u8 *enable)
{
	//hal_mpt_GetPowerTracking(padapter, enable);
	return;
}

void rtw_mp_trigger_iqk(_adapter *padapter)
{
	rtw_mp_cal_trigger(padapter, RTW_MP_CAL_IQK);
}

void rtw_mp_trigger_lck(_adapter *padapter)
{
	rtw_mp_cal_trigger(padapter, RTW_MP_CAL_LCK);
}

void rtw_mp_trigger_dpk(_adapter *padapter)
{
	rtw_mp_cal_trigger(padapter, RTW_MP_CAL_DPK);
}

void rtw_mp_trigger_tssi(_adapter *padapter)
{
	rtw_mp_cal_trigger(padapter, RTW_MP_CAL_TSSI);
}

void rtw_mp_trigger_ch_rfk(_adapter *padapter)
{
	rtw_mp_cal_trigger(padapter, RTW_MP_CAL_CHL_RFK);
}

void rtw_mp_trigger_dack(_adapter *padapter)
{
	rtw_mp_cal_trigger(padapter, RTW_MP_CAL_DACK);
}

static void init_mp_data(_adapter *padapter)
{
#if 0	
	struct dm_struct *phydm = adapter_to_phydm(padapter);

	/*disable BCN*/
	v8 = rtw_read8(padapter, REG_BCN_CTRL);
	v8 &= ~EN_BCN_FUNCTION;
	rtw_write8(padapter, REG_BCN_CTRL, v8);

	phydm->rf_calibrate_info.txpowertrack_control = _FALSE;
#endif	
}

u32 mp_join(_adapter *padapter, u8 mode)
{
	WLAN_BSSID_EX bssid;
	struct sta_info *psta;
	u32 length;
	s32 res = _SUCCESS;
	u8 i = 0;
	struct mp_priv *pmppriv = &padapter->mppriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct wlan_network *tgt_network = &pmlmepriv->dev_cur_network;
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	WLAN_BSSID_EX		*pnetwork = (WLAN_BSSID_EX *)(&(pmlmeinfo->dev_network));
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct rtw_phl_mld_t *pmld = NULL;
	void *phl = GET_PHL_INFO(adapter_to_dvobj(padapter));

	/* 1. initialize a new WLAN_BSSID_EX */
	_rtw_memset(&bssid, 0, sizeof(WLAN_BSSID_EX));
	RTW_INFO("%s ,pmppriv->network_macaddr=%x %x %x %x %x %x\n", __func__,
		pmppriv->network_macaddr[0], pmppriv->network_macaddr[1], pmppriv->network_macaddr[2], pmppriv->network_macaddr[3], pmppriv->network_macaddr[4],
		 pmppriv->network_macaddr[5]);
	_rtw_memcpy(bssid.MacAddress, pmppriv->network_macaddr, ETH_ALEN);

	if (mode == WIFI_FW_ADHOC_STATE) {
		bssid.Ssid.SsidLength = strlen("mp_pseudo_adhoc");
		_rtw_memcpy(bssid.Ssid.Ssid, (u8 *)"mp_pseudo_adhoc", bssid.Ssid.SsidLength);
		bssid.InfrastructureMode = Ndis802_11IBSS;
		bssid.IELength = 0;
		bssid.Configuration.DSConfig = pmppriv->channel;

	} else if (mode == WIFI_FW_STATION_STATE) {
		bssid.Ssid.SsidLength = strlen("mp_pseudo_STATION");
		_rtw_memcpy(bssid.Ssid.Ssid, (u8 *)"mp_pseudo_STATION", bssid.Ssid.SsidLength);
		bssid.InfrastructureMode = Ndis802_11Infrastructure;
		bssid.IELength = 0;
	}

	length = get_WLAN_BSSID_EX_sz(&bssid);
	if (length % 4)
		bssid.Length = ((length >> 2) + 1) << 2; /* round up to multiple of 4 bytes. */
	else
		bssid.Length = length;

	_rtw_spinlock_bh(&pmlmepriv->lock);

	if (check_fwstate(pmlmepriv, WIFI_MP_STATE) == _TRUE)
		goto end_of_mp_start_test;

	/* init mp_start_test status */
	for (i = 0; i < dvobj->iface_nums; i++) {
		_adapter *iface = NULL;
		struct mlme_priv *buddy_mlmepriv;

		iface = dvobj->padapters[i];
		buddy_mlmepriv = &iface->mlmepriv;

		if (iface == NULL)
			continue;
		if (rtw_is_adapter_up(iface) == _FALSE)
			continue;
		if (check_fwstate(buddy_mlmepriv, WIFI_ASOC_STATE) == _TRUE) {
#ifdef CONFIG_STA_CMD_DISPR
			rtw_disassoc_cmd(iface, 500, RTW_CMDF_WAIT_ACK);
#else /* !CONFIG_STA_CMD_DISPR */
			rtw_disassoc_cmd(iface, 500, 0);
			rtw_free_assoc_resources_cmd(iface, _TRUE, RTW_CMDF_WAIT_ACK);
#endif /* !CONFIG_STA_CMD_DISPR */
			rtw_indicate_disconnect(iface, 0, _FALSE);
		}
	}

	pmppriv->prev_fw_state = get_fwstate(pmlmepriv);
	/*pmlmepriv->fw_state = WIFI_MP_STATE;*/
	init_fwstate(pmlmepriv, WIFI_MP_STATE);
	set_fwstate(pmlmepriv, WIFI_UNDER_LINKING);

	/* 3 2. create a new psta for mp driver */
	/* clear psta in the cur_network, if any */
	psta = rtw_get_stainfo(&padapter->stapriv, tgt_network->network.MacAddress);
	if (psta)
		rtw_free_mld_stainfo(padapter, psta->phl_sta->mld);
	/* ToDo CONFIG_RTW_MLD: MLD MAC Address */
	pmld = rtw_phl_alloc_mld(GET_PHL_INFO(adapter_to_dvobj(padapter)), padapter->phl_role, bssid.MacAddress, DTYPE);
	if (pmld == NULL) {
		init_fwstate(pmlmepriv, pmppriv->prev_fw_state);
		res = _FAIL;
		goto end_of_mp_start_test;
	}
	/* main_id is don't care for self */
	psta = rtw_alloc_stainfo(&padapter->stapriv, bssid.MacAddress, DTYPE, 0, padapter_link->wrlink->id, PHL_CMD_DIRECTLY);
	if (psta == NULL) {
		/*pmlmepriv->fw_state = pmppriv->prev_fw_state;*/
		init_fwstate(pmlmepriv, pmppriv->prev_fw_state);
		res = _FAIL;
		goto end_of_mp_start_test;
	}
	if (mode == WIFI_FW_ADHOC_STATE)
		set_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE);
	else
		set_fwstate(pmlmepriv, WIFI_STATION_STATE);
	/* 3 3. join psudo AdHoc */
	tgt_network->join_res = 1;
	tgt_network->aid = psta->phl_sta->aid = 1;

	_rtw_memcpy(&padapter->registrypriv.dev_network, &bssid, length);
	rtw_update_registrypriv_dev_network(padapter);
	_rtw_memcpy(&tgt_network->network, &padapter->registrypriv.dev_network, padapter->registrypriv.dev_network.Length);
	_rtw_memcpy(pnetwork, &padapter->registrypriv.dev_network, padapter->registrypriv.dev_network.Length);

	rtw_indicate_connect(padapter);
	_clr_fwstate_(pmlmepriv, WIFI_UNDER_LINKING);
	set_fwstate(pmlmepriv, WIFI_ASOC_STATE);

end_of_mp_start_test:

	_rtw_spinunlock_bh(&pmlmepriv->lock);

	if (1) { /* (res == _SUCCESS) */
		/* set MSR to WIFI_FW_ADHOC_STATE */
		if (mode == WIFI_FW_ADHOC_STATE) {
			/* set msr to WIFI_FW_ADHOC_STATE */
			pmlmeinfo->state = WIFI_FW_ADHOC_STATE;
			rtw_hal_set_hwreg(padapter, HW_VAR_BSSID, padapter->registrypriv.dev_network.MacAddress);
			pmlmeinfo->state |= WIFI_FW_ASSOC_SUCCESS;
		} else {
			RTW_INFO("%s , pmppriv->network_macaddr =%x %x %x %x %x %x\n", __func__,
				pmppriv->network_macaddr[0], pmppriv->network_macaddr[1], pmppriv->network_macaddr[2], pmppriv->network_macaddr[3], pmppriv->network_macaddr[4],
				pmppriv->network_macaddr[5]);

			rtw_hal_set_hwreg(padapter, HW_VAR_BSSID, pmppriv->network_macaddr);
		}
	}

	return res;
}
/* This function initializes the DUT to the MP test mode */
s32 mp_start_test(_adapter *padapter)
{
	struct mp_priv *pmppriv = &padapter->mppriv;
	s32 res = _SUCCESS;

	padapter->registrypriv.mp_mode = 1;
	init_mp_priv(padapter);

	init_mp_data(padapter);
#if 0
#ifdef CONFIG_PCI_HCI
	hal = GET_PHL_COM(adapter_to_dvobj(padapter));
	hal->pci_backdoor_ctrl = 0;
	rtw_pci_aspm_config(padapter);
#endif


	/* 3 0. update mp_priv */
	switch (GET_HAL_RFPATH(padapter)) {
		case RF_1T1R:
			pmppriv->antenna_tx = ANTENNA_A;
			pmppriv->antenna_rx = ANTENNA_A;
			break;
		case RF_1T2R:
		default:
			pmppriv->antenna_tx = ANTENNA_A;
			pmppriv->antenna_rx = ANTENNA_AB;
			break;
		case RF_2T2R:
			pmppriv->antenna_tx = ANTENNA_AB;
			pmppriv->antenna_rx = ANTENNA_AB;
			break;
		case RF_2T4R:
			pmppriv->antenna_tx = ANTENNA_AB;
			pmppriv->antenna_rx = ANTENNA_ABCD;
			break;
	}
#endif
	mpt_ProStartTest(padapter);

	mp_join(padapter, WIFI_FW_ADHOC_STATE);

	return res;
}
/* ------------------------------------------------------------------------------
 * This function change the DUT from the MP test mode into normal mode */
void mp_stop_test(_adapter *padapter)
{
	struct mp_priv *pmppriv = &padapter->mppriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct wlan_network *tgt_network = &pmlmepriv->dev_cur_network;
	struct sta_info *psta;
#ifdef CONFIG_PCI_HCI
	struct registry_priv  *registry_par = &padapter->registrypriv;
#endif

	if (pmppriv->mode == MP_ON) {
		pmppriv->bSetTxPower = 0;
		_rtw_spinlock_bh(&pmlmepriv->lock);
		if (check_fwstate(pmlmepriv, WIFI_MP_STATE) == _FALSE)
			goto end_of_mp_stop_test;

		/* 3 1. disconnect psudo AdHoc */
		rtw_indicate_disconnect(padapter, 0, _FALSE);

		/* 3 2. clear psta used in mp test mode.
		*	rtw_free_assoc_resources(padapter, _TRUE); */
		psta = rtw_get_stainfo(&padapter->stapriv, tgt_network->network.MacAddress);
		if (psta)
			rtw_free_mld_stainfo(padapter, psta->phl_sta->mld);

		/* 3 3. return to normal state (default:station mode) */
		/*pmlmepriv->fw_state = pmppriv->prev_fw_state; */ /* WIFI_STATION_STATE;*/
		init_fwstate(pmlmepriv, pmppriv->prev_fw_state);

		/* flush the cur_network */
		_rtw_memset(tgt_network, 0, sizeof(struct wlan_network));

		_clr_fwstate_(pmlmepriv, WIFI_MP_STATE);

end_of_mp_stop_test:

		_rtw_spinunlock_bh(&pmlmepriv->lock);

#if 0//def CONFIG_PCI_HCI
		hal = GET_PHL_COM(adapter_to_dvobj(padapter));
		hal->pci_backdoor_ctrl = registry_par->pci_aspm_config;
		rtw_pci_aspm_config(padapter);
#endif
	}
}

void SetChannel(_adapter *padapter)
{
#ifdef CONFIG_MP_INCLUDED
	rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_SET_CH_BW);
#else
	//hal_mpt_SetChannel(adapter);
#endif	
}

void SetBandwidth(_adapter *padapter)
{
#ifdef CONFIG_MP_INCLUDED
	rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_SET_CH_BW);
#else	
	//hal_mpt_SetBandwidth(adapter);
#endif
}

void SetAntenna(_adapter *padapter)
{
	mp_ant_path mp_trx_path = padapter->mppriv.antenna_trx;
	u8 cfg_rf_path = RF_PATH_A;

	switch (mp_trx_path) {
	case MP_ANTENNA_A:
		cfg_rf_path = RF_PATH_A;
		break;
	case MP_ANTENNA_B:
		cfg_rf_path = RF_PATH_B;
		break;
	case MP_ANTENNA_C:
		cfg_rf_path = RF_PATH_C;
		break;
	case MP_ANTENNA_D:
		cfg_rf_path = RF_PATH_D;
		break;
	case MP_ANTENNA_AB:
		cfg_rf_path = RF_PATH_AB;
		break;
	case MP_ANTENNA_AC:
		cfg_rf_path = RF_PATH_AC;
		break;
	case MP_ANTENNA_AD:
		cfg_rf_path = RF_PATH_AD;
		break;
	case MP_ANTENNA_BC:
		cfg_rf_path = RF_PATH_BC;
		break;
	case MP_ANTENNA_BD:
		cfg_rf_path = RF_PATH_BD;
		break;
	case MP_ANTENNA_CD:
		cfg_rf_path = RF_PATH_CD;
		break;
	case MP_ANTENNA_ABC:
		cfg_rf_path = RF_PATH_ABC;
		break;
	case MP_ANTENNA_BCD:
		cfg_rf_path = RF_PATH_BCD;
		break;
	case MP_ANTENNA_ABD:
		cfg_rf_path = RF_PATH_ABD;
		break;
	case MP_ANTENNA_ACD:
		cfg_rf_path = RF_PATH_ACD;
		break;
	case MP_ANTENNA_ABCD:
		cfg_rf_path = RF_PATH_ABCD;
		break;
	default:
		RTW_INFO("%s ,default Antenna Path A\n", __func__);
		break;
	}

	padapter->mppriv.antenna_trx = cfg_rf_path;
	rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_SET_RF_TXRX_PATH);
}

int rtw_mp_txpoweridx(_adapter *adapter)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	struct mp_priv *pmppriv = &adapter->mppriv;
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	u8 tx_nss = get_phy_tx_nss(adapter, adapter_link);
	u8 i = 0;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.txpwr_index = pmppriv->txpoweridx * TX_POWER_CODE_WORD_BASE;

	for (i = 0; i < tx_nss; i++) {
		ptxpwr_arg.rfpath = i;
		ptxpwr_arg.is_cck = rtw_mp_is_cck_rate(pmppriv->rateidx);

		rtw_mp_phl_txpower(adapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_SET_TXPWR_INDEX);
	}
	return _TRUE;
}

s16 rtw_mp_get_pwr_refcw(_adapter *adapter, u8 rfpath, u8 is_cck)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	struct mp_priv *pmppriv = &adapter->mppriv;
	s16 txpwr_refcw_idx;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));
	ptxpwr_arg.rfpath = rfpath;
	ptxpwr_arg.is_cck = is_cck;

	rtw_mp_phl_txpower(adapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_GET_TXPWR_REF_CW);
	txpwr_refcw_idx = ptxpwr_arg.txpwr_index / TX_POWER_CODE_WORD_BASE;

	RTW_INFO("%s ,pwr ref cw: %d\n", __func__, txpwr_refcw_idx);
	return txpwr_refcw_idx;
}

u16 rtw_mp_get_pwr_ref(_adapter *adapter, u8 rfpath)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	struct mp_priv *pmppriv = &adapter->mppriv;
	s16 txpwr_ref;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));
	ptxpwr_arg.rfpath = rfpath;
	ptxpwr_arg.is_cck = rtw_mp_is_cck_rate(pmppriv->rateidx);

	rtw_mp_phl_txpower(adapter,  &ptxpwr_arg, RTW_MP_TXPWR_CMD_GET_TXPWR_REF);
	txpwr_ref = ptxpwr_arg.txpwr_ref / TX_POWER_BASE;

	RTW_INFO("%s ,pwr ref: %d\n", __func__, txpwr_ref);
	return txpwr_ref;
}

u16 rtw_mp_get_pwrtab_dbm(_adapter *adapter, u8 rfpath)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	struct mp_priv *pmppriv = &adapter->mppriv;
	s16 txpwr_dbm;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));
	ptxpwr_arg.rfpath = rfpath;
	ptxpwr_arg.is_cck = rtw_mp_is_cck_rate(pmppriv->rateidx);
	ptxpwr_arg.rate = pmppriv->rateidx;
	ptxpwr_arg.dcm = 0;
	ptxpwr_arg.offset = 0;
	ptxpwr_arg.bandwidth = pmppriv->bandwidth;
	ptxpwr_arg.beamforming = 0;
	ptxpwr_arg.channel = pmppriv->channel;

	rtw_mp_phl_txpower(adapter,  &ptxpwr_arg, RTW_MP_TXPWR_CMD_READ_PWR_TABLE);
	txpwr_dbm = ptxpwr_arg.table_item / TX_POWER_BASE;

	RTW_INFO("%s ,read PwrTab dbm: %d\n", __func__, txpwr_dbm);
	return txpwr_dbm;
}


u16 rtw_mp_txpower_dbm(_adapter *adapter, u8 rf_path)
{
	struct mp_priv *pmppriv = &adapter->mppriv;
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	u16 tmp_power_ref = 0;
	u16 agc_cw_val = 0;
	s16 pre_pwr_refcw_idx = 0;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.txpwr = pmppriv->txpowerdbm;
	ptxpwr_arg.is_cck = rtw_mp_is_cck_rate(pmppriv->rateidx);
	ptxpwr_arg.rfpath = rf_path;

	rtw_mp_phl_txpower(adapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_SET_TXPWR);

	if (pmppriv->pre_refcw_cck_pwridxa == 0 || pmppriv->pre_refcw_cck_pwridxb == 0) {
		pmppriv->pre_refcw_cck_pwridxa = rtw_mp_get_pwr_refcw(adapter, RF_PATH_A, 1);
		pmppriv->pre_refcw_cck_pwridxb = rtw_mp_get_pwr_refcw(adapter, RF_PATH_B, 1);
	}
	if (pmppriv->pre_refcw_ofdm_pwridxa == 0 || pmppriv->pre_refcw_ofdm_pwridxb == 0) {
		pmppriv->pre_refcw_ofdm_pwridxa = rtw_mp_get_pwr_refcw(adapter, RF_PATH_A, 0);
		pmppriv->pre_refcw_ofdm_pwridxb = rtw_mp_get_pwr_refcw(adapter, RF_PATH_B, 0);
	}

	if (rf_path == RF_PATH_A && ptxpwr_arg.is_cck == true)
		pre_pwr_refcw_idx = pmppriv->pre_refcw_cck_pwridxa;
	else if (rf_path == RF_PATH_B && ptxpwr_arg.is_cck == true)
		pre_pwr_refcw_idx = pmppriv->pre_refcw_cck_pwridxb;
	else if (rf_path == RF_PATH_A && ptxpwr_arg.is_cck == false)
		pre_pwr_refcw_idx = pmppriv->pre_refcw_ofdm_pwridxa;
	else if (rf_path == RF_PATH_B && ptxpwr_arg.is_cck == false)
		pre_pwr_refcw_idx = pmppriv->pre_refcw_ofdm_pwridxb;

	tmp_power_ref = rtw_mp_get_pwr_ref(adapter, rf_path);
	RTW_INFO("%s () tmp_power_ref: %d !! pre_pwr_refcw_idx:%d, pwr offset %d\n",
							__func__, tmp_power_ref, pre_pwr_refcw_idx , pmppriv->txpoweridx);
	agc_cw_val = pmppriv->txpowerdbm - tmp_power_ref + pre_pwr_refcw_idx + pmppriv->txpoweridx;

	ptxpwr_arg.txpwr_index = (pre_pwr_refcw_idx + pmppriv->txpoweridx) * TX_POWER_CODE_WORD_BASE;
	rtw_mp_phl_txpower(adapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_SET_TXPWR_INDEX);

	return agc_cw_val;
}

void SetDataRate(_adapter *padapter)
{

	rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_SET_RATE_IDX);

	return;
}

void SetTxAGCOffset(_adapter *adapter, u32 ulTxAGCOffset)
{
	u32 TxAGCOffset_B, TxAGCOffset_C, TxAGCOffset_D, tmpAGC;

	TxAGCOffset_B = (ulTxAGCOffset & 0x000000ff);
	TxAGCOffset_C = ((ulTxAGCOffset & 0x0000ff00) >> 8);
	TxAGCOffset_D = ((ulTxAGCOffset & 0x00ff0000) >> 16);

	tmpAGC = (TxAGCOffset_D << 8 | TxAGCOffset_C << 4 | TxAGCOffset_B);
//	write_bbreg(adapter, rFPGA0_TxGainStage,
//		    (bXBTxAGC | bXCTxAGC | bXDTxAGC), tmpAGC);
}

void MP_PHY_SetRFPathSwitch(_adapter *adapter , BOOLEAN bMain)
{
	//PHY_SetRFPathSwitch(adapter, bMain);
	return;
}

void mp_phy_switch_rf_path_set(_adapter *adapter , u8 *pstate)
{

	//phy_switch_rf_path_set(adapter, pstate);
	return;

}

u8 MP_PHY_QueryRFPathSwitch(_adapter *adapter)
{
	//return PHY_QueryRFPathSwitch(adapter);
	return 0;
}

s32 SetThermalMeter(_adapter *adapter, u8 target_ther)
{
	//return hal_mpt_SetThermalMeter(adapter, target_ther);
	return 0;
}

#if 0
static void TriggerRFThermalMeter(_adapter *adapter)
{
	hal_mpt_TriggerRFThermalMeter(adapter);
}

static u8 ReadRFThermalMeter(_adapter *adapter)
{
	return hal_mpt_ReadRFThermalMeter(adapter);
}
#endif

void GetUuid(_adapter *adapter, u32 *uuid)
{
	struct rtw_mp_config_arg pmp_arg;
	u16 i = 0;

	_rtw_memset((void *)&pmp_arg, 0, sizeof(struct rtw_mp_config_arg));

	pmp_arg.mp_class = RTW_MP_CLASS_CONFIG;
	pmp_arg.cmd = RTW_MP_CONFIG_CMD_GET_UUID;
	RTW_INFO("%s, id: %d !!!\n", __func__, pmp_arg.cmd);

	rtw_mp_set_phl_cmd(adapter, (void*)&pmp_arg, sizeof(struct rtw_mp_config_arg));

	while (i != 100) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(adapter, (void*)&pmp_arg, sizeof(struct rtw_mp_config_arg));

		if (pmp_arg.cmd_ok && pmp_arg.status == RTW_PHL_STATUS_SUCCESS) {
			*uuid = pmp_arg.uuid;
			RTW_INFO("%s, SET CMD OK, uuid = %d\n", __func__, pmp_arg.uuid);
			break;
		} else {
			if (i > 100) {
				RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__, pmp_arg.status);
				break;
			}
			i++;
			rtw_msleep_os(10);
		}
	}
}

void GetThermalMeter(_adapter *adapter, u8 rfpath ,u8 *value)
{
	struct mp_priv	*pmppriv = &adapter->mppriv;
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	u16 i = 0;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.mp_class = RTW_MP_CLASS_TXPWR;
	ptxpwr_arg.cmd = RTW_MP_TXPWR_CMD_GET_THERMAL;
	ptxpwr_arg.rfpath = rfpath;

	rtw_mp_set_phl_cmd(adapter, (void*)&ptxpwr_arg, sizeof(struct rtw_mp_txpwr_arg));

	while (i != 100) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(adapter, (void*)&ptxpwr_arg, sizeof(struct rtw_mp_txpwr_arg));

		if (ptxpwr_arg.cmd_ok && ptxpwr_arg.status == RTW_PHL_STATUS_SUCCESS) {
			*value = ptxpwr_arg.thermal;
			RTW_INFO("%s, SET CMD OK, thermal = %d\n", __func__, ptxpwr_arg.thermal);
			break;
		} else {
			if (i > 100) {
				RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__, ptxpwr_arg.status);
					break;
				}
			i++;
			rtw_msleep_os(10);
		}
	}
}

void rtw_mp_singlecarrier_tx(_adapter *adapter, u8 bstart)
{
	RTW_INFO("Not Supported SetSingleCarrierTx \n");
}

void rtw_mp_singletone_tx(_adapter *adapter, u8 bstart)
{
	struct mp_priv *pmp_priv = &adapter->mppriv;

	rtw_mp_txpwr_level(adapter);
	rtw_phl_mp_tx_cmd(adapter, RTW_MP_TX_SINGLE_TONE, pmp_priv->rtw_mp_tx_method, bstart);
}

void rtw_mp_carriersuppr_tx(_adapter *adapter, u8 bstart)
{
	struct mp_priv *pmp_priv = &adapter->mppriv;

	rtw_mp_txpwr_level(adapter);
	rtw_phl_mp_tx_cmd(adapter, RTW_MP_TX_CCK_Carrier_Suppression, pmp_priv->rtw_mp_tx_method, bstart);
}

void rtw_mp_continuous_tx(_adapter *adapter, u8 bstart)
{
	struct mp_priv *pmp_priv = &adapter->mppriv;

	rtw_mp_txpwr_level(adapter);
	rtw_phl_mp_tx_cmd(adapter, RTW_MP_TX_CONTINUOUS, pmp_priv->rtw_mp_tx_method, bstart);
}

void rtw_mp_txpwr_level(_adapter *adapter)
{
	struct mp_priv *pmp_priv = &adapter->mppriv;
/*
	if (pmp_priv->bSetTxPower == 0)
		rtw_hal_set_tx_power_level(adapter, pmp_priv->channel);
*/

	return;
}

/* ------------------------------------------------------------------------------ */
static void dump_mpframe(_adapter *padapter, struct xmit_frame *pmpframe)
{
	/*rtw_hal_mgnt_xmit(padapter, pmpframe);*/
	rtw_mgnt_xmit(padapter, pmpframe);
}

static struct xmit_frame *alloc_mp_xmitframe(struct xmit_priv *pxmitpriv)
{
#if 0 /*CONFIG_CORE_XMITBUF*/
	struct xmit_frame	*pmpframe;
	struct xmit_buf	*pxmitbuf;

	pmpframe = rtw_alloc_xmitframe(pxmitpriv);
	if (pmpframe == NULL)
		return NULL;

	pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
	if (pxmitbuf == NULL) {
		rtw_free_xmitframe(pxmitpriv, pmpframe);
		return NULL;
	}

	pmpframe->frame_tag = MP_FRAMETAG;

	pmpframe->pxmitbuf = pxmitbuf;

	pmpframe->buf_addr = pxmitbuf->pbuf;

	pxmitbuf->priv_data = pmpframe;
#else
	struct xmit_frame *pmpframe;

	pmpframe = rtw_alloc_xmitframe(pxmitpriv, 0);
	if (pmpframe == NULL)
		return NULL;

	pmpframe->frame_tag = MP_FRAMETAG;
#endif

	return pmpframe;

}

static thread_return mp_xmit_packet_thread(thread_context context)
{
	struct xmit_frame	*pxmitframe;
	struct mp_tx		*pmptx;
	struct mp_priv	*pmp_priv;
	struct xmit_priv	*pxmitpriv;
	_adapter *padapter;

	pmp_priv = (struct mp_priv *)context;
	pmptx = &pmp_priv->tx;
	padapter = pmp_priv->papdater;
	pxmitpriv = &(padapter->xmitpriv);

	rtw_thread_enter("RTW_MP_THREAD");

	RTW_INFO("%s:pkTx Start\n", __func__);
	while (1) {
		pxmitframe = alloc_mp_xmitframe(pxmitpriv);

		if (pxmitframe == NULL) {
			if (pmptx->stop ||
			    RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
				goto exit;
			else {
				rtw_usleep_os(10);
				continue;
			}
		}
		_rtw_memcpy((u8 *)(pxmitframe->buf_addr + TXDESC_OFFSET), pmptx->buf, pmptx->write_size);
		_rtw_memcpy(&(pxmitframe->attrib), &(pmptx->attrib), sizeof(struct pkt_attrib));


		rtw_usleep_os(padapter->mppriv.pktInterval);
		dump_mpframe(padapter, pxmitframe);

		pmptx->sended++;
		pmp_priv->tx_pktcount++;

		if (pmptx->stop ||
		    RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
			goto exit;
		if ((pmptx->count != 0) &&
		    (pmptx->count == pmptx->sended))
			goto exit;

		flush_signals_thread();
	}

exit:
	/* RTW_INFO("%s:pkTx Exit\n", __func__); */
	rtw_mfree(pmptx->pallocated_buf, pmptx->buf_size);
	pmptx->pallocated_buf = NULL;
	pmptx->stop = 1;

	rtw_thread_exit(NULL);
	return 0;
}

void fill_txdesc_for_mp(_adapter *padapter, u8 *ptxdesc)
{
	struct mp_priv *pmp_priv = &padapter->mppriv;
	_rtw_memcpy(ptxdesc, pmp_priv->tx.desc, TXDESC_SIZE);
}

static void Rtw_MPSetMacTxEDCA(_adapter *padapter)
{
#if 0
	rtw_write32(padapter, 0x508 , 0x00a422); /* Disable EDCA BE Txop for MP pkt tx adjust Packet interval */
	/* RTW_INFO("%s:write 0x508~~~~~~ 0x%x\n", __func__,rtw_read32(padapter, 0x508)); */
	phy_set_mac_reg(padapter, 0x458 , bMaskDWord , 0x0);
	/*RTW_INFO("%s()!!!!! 0x460 = 0x%x\n" ,__func__, phy_query_bb_reg(padapter, 0x460, bMaskDWord));*/
	phy_set_mac_reg(padapter, 0x460 , bMaskLWord , 0x0); /* fast EDCA queue packet interval & time out value*/
	/*phy_set_mac_reg(padapter, ODM_EDCA_VO_PARAM ,bMaskLWord , 0x431C);*/
	/*phy_set_mac_reg(padapter, ODM_EDCA_BE_PARAM ,bMaskLWord , 0x431C);*/
	/*phy_set_mac_reg(padapter, ODM_EDCA_BK_PARAM ,bMaskLWord , 0x431C);*/
	RTW_INFO("%s()!!!!! 0x460 = 0x%x\n" , __func__, phy_query_bb_reg(padapter, 0x460, bMaskDWord));
#endif
}

u8 rtw_phl_mp_tx_cmd(_adapter *padapter, enum rtw_mp_tx_cmd cmdid,
							enum rtw_mp_tx_method tx_method, boolean bstart)
{
	struct mp_priv	*pmppriv = &padapter->mppriv;
	struct pkt_attrib *pattrib = &pmppriv->tx.attrib;
	struct rtw_mp_tx_arg tx_arg;
	u16 i = 0;
	u8 user_idx = pmppriv->mp_plcp_useridx;

	_rtw_memset((void *)&tx_arg, 0, sizeof(struct rtw_mp_tx_arg));
	tx_arg.mp_class = RTW_MP_CLASS_TX;
	tx_arg.tx_method = tx_method;
	tx_arg.cmd = cmdid;
	tx_arg.cmd_ok = 0;
	tx_arg.tx_ok = 0;
	tx_arg.data_rate = pmppriv->rateidx;
	tx_arg.is_cck = rtw_mp_is_cck_rate(pmppriv->rateidx);
	tx_arg.start_tx = bstart;
	tx_arg.tx_path = pmppriv->antenna_trx;
	tx_arg.gi = pmppriv->rtw_mp_plcp_gi;
	tx_arg.period = pmppriv->pktInterval;
	tx_arg.plcp_usr_idx = user_idx;
	tx_arg.stbc = pmppriv->rtw_mp_stbc;
	tx_arg.tx_state = 0;

	switch (cmdid) {
		case RTW_MP_TX_PACKETS:
			RTW_INFO("%s,SET MP_TX_PACKETS tx_method %d\n", __func__, tx_arg.tx_method);
			if (tx_method == RTW_MP_PMACT_TX) {
				tx_arg.tx_mode = RTW_MP_PMAC_PKTS_TX;
				tx_arg.tx_cnt = pmppriv->tx.count;
				tx_arg.tx_time = pmppriv->rtw_mp_tx_time;

			} else if (tx_method == RTW_MP_FW_PMACT_TX) {
				tx_arg.tx_mode = RTW_MP_PMAC_FW_TRIG_TX;
				tx_arg.tx_cnt = 0;
				tx_arg.tx_time = 60;
			}

			break;
		case RTW_MP_TX_CONTINUOUS:
			RTW_INFO("%s,SET MP_TX_CONTINUOUS\n", __func__);
			tx_arg.tx_mode = RTW_MP_PMAC_CONT_TX;
			break;
		case RTW_MP_TX_SINGLE_TONE:
			RTW_INFO("%s,SET TX_OFDM_Single_Tone\n", __func__);
			tx_arg.tx_mode = RTW_MP_PMAC_OFDM_SINGLE_TONE_TX;
			break;
		case RTW_MP_TX_CCK_Carrier_Suppression:
			RTW_INFO("%s,SET TX_CCK_Carrier_Suppression\n", __func__);
			tx_arg.tx_mode = RTW_MP_PMAC_CCK_CARRIER_SIPPRESSION_TX;
			break;
		case RTW_MP_TX_CONFIG_PLCP_PATTERN:
			RTW_INFO("%s,SET MP_TX_CONFIG_PLCP_PATTERN\n", __func__);
			tx_arg.plcp_case_id = pmppriv->rtw_mp_pmact_patt_idx;
			tx_arg.plcp_ppdu_type = pmppriv->rtw_mp_pmact_ppdu_type;
			break;
		case RTW_MP_TX_CONFIG_PLCP_COMMON_INFO:
			tx_arg.dbw = pmppriv->rtw_mp_data_bandwidth;
			tx_arg.long_preamble_en = pmppriv->preamble;
			tx_arg.stbc = pmppriv->rtw_mp_stbc;
			tx_arg.gi = pmppriv->rtw_mp_plcp_gi;
			tx_arg.he_ltf_type = pmppriv->rtw_mp_plcp_ltf;
			tx_arg.he_mcs_sigb = pmppriv->rtw_mp_he_sigb;
			tx_arg.he_dcm_sigb = pmppriv->rtw_mp_he_sigb_dcm;
			tx_arg.bss_color = 0;
			tx_arg.he_er_u106ru_en = pmppriv->rtw_mp_he_er_su_ru_106_en;
			if (pmppriv->rtw_mp_tx_method == RTW_MP_FW_PMACT_TX) {
				pmppriv->rtw_mp_plcp_tx_time = 5484;
				pmppriv->rtw_mp_plcp_tx_mode = 1;
			}
			tx_arg.max_tx_time_0p4us = pmppriv->rtw_mp_plcp_tx_time;
			tx_arg.mode = pmppriv->rtw_mp_plcp_tx_mode;
			tx_arg.ppdu_type = pmppriv->rtw_mp_pmact_ppdu_type;
			tx_arg.cbw = pmppriv->bandwidth;
			tx_arg.txsc = pmppriv->rtw_mp_trxsc;
			tx_arg.n_user = pmppriv->rtw_mp_plcp_tx_user; 

			RTW_INFO("%s,SET TX_CONFIG_PLCP_COMMON_INFO\n", __func__);
			RTW_INFO("%s=============================\n", __func__);
			RTW_INFO("%s dbw = %d\n", __func__, tx_arg.dbw);
			RTW_INFO("%s long_preamble_en = %d\n", __func__, tx_arg.long_preamble_en);
			RTW_INFO("%s stbc = %d\n", __func__, tx_arg.stbc);
			RTW_INFO("%s gi = %d\n", __func__, tx_arg.gi);
			RTW_INFO("%s tb_l_len = %d\n", __func__, tx_arg.tb_l_len);
			RTW_INFO("%s he_ltf_type = %d\n", __func__, tx_arg.he_ltf_type);
			RTW_INFO("%s he_mcs_sigb = %d\n", __func__, tx_arg.he_mcs_sigb);
			RTW_INFO("%s he_dcm_sigb = %d\n", __func__, tx_arg.he_dcm_sigb);
			RTW_INFO("%s max_tx_time_0p4us = %d\n", __func__, tx_arg.max_tx_time_0p4us);
			RTW_INFO("%s bss_color = %d\n", __func__, tx_arg.bss_color);
			RTW_INFO("%s he_er_u106ru_en = %d\n", __func__, tx_arg.he_er_u106ru_en);
			RTW_INFO("%s mode = %d\n", __func__, tx_arg.mode);
			RTW_INFO("%s ppdu_type = %d\n", __func__, tx_arg.ppdu_type);
			RTW_INFO("%s cbw = %d\n", __func__, tx_arg.cbw);
			RTW_INFO("%s txsc = %d\n", __func__, tx_arg.txsc);
			RTW_INFO("%s n_user = %d\n", __func__, tx_arg.n_user);
			RTW_INFO("%s=============================\n", __func__);
			break;
		case RTW_MP_TX_CMD_PHY_OK:
			/*TBD. API not ready*/
			RTW_INFO("%s,SET MP_TX_CMD_PHY_OK\n", __func__);
			break;
		case RTW_MP_TX_CONFIG_PLCP_USER_INFO:
			tx_arg.plcp_usr_idx = user_idx;
			tx_arg.mcs = pmppriv->mp_plcp_user[user_idx].plcp_mcs;
			tx_arg.fec = pmppriv->mp_plcp_user[user_idx].coding;
			tx_arg.dcm = pmppriv->mp_plcp_user[user_idx].dcm;
			tx_arg.aid = pmppriv->mp_plcp_user[user_idx].aid;
			tx_arg.scrambler_seed = (rtw_random32() % 127) + 1;
			tx_arg.random_init_seed = (rtw_random32() % 127) + 1;
			tx_arg.apep = pmppriv->mp_plcp_user[user_idx].plcp_txlen;
			tx_arg.ru_alloc = pmppriv->mp_plcp_user[user_idx].ru_alloc;
			tx_arg.nss = pmppriv->mp_plcp_user[user_idx].plcp_nss + 1;
			tx_arg.pwr_boost_db = pmppriv->mp_plcp_user[user_idx].pwr_boost_db;
			tx_arg.fec = pmppriv->mp_plcp_user[user_idx].coding;

			RTW_INFO("%s,SET MP_TX_CONFIG_PLCP_USER_INFO\n", __func__);
			RTW_INFO("%s plcp_usr_idx = %d\n", __func__, tx_arg.plcp_usr_idx);
			RTW_INFO("%s mcs = %d\n", __func__, tx_arg.mcs);
			RTW_INFO("%s fec = %d\n", __func__, tx_arg.fec);
			RTW_INFO("%s dcm = %d\n", __func__, tx_arg.dcm);
			RTW_INFO("%s ascrambler_seed = %d\n", __func__, tx_arg.scrambler_seed);
			RTW_INFO("%s random_init_seed = %d\n", __func__, tx_arg.random_init_seed);
			RTW_INFO("%s apep = %d\n", __func__, tx_arg.apep);
			RTW_INFO("%s ru_alloc = %d\n", __func__, tx_arg.ru_alloc);
			RTW_INFO("%s nss = %d\n", __func__, tx_arg.nss);
			break;
		case RTW_MP_TX_MODE_SWITCH:
			if (tx_method == RTW_MP_TMACT_TX)
				tx_arg.tx_mode = 0;/* mode: 0 = tmac, 1 = pmac */
			else
				tx_arg.tx_mode = 1;/* mode: 0 = tmac, 1 = pmac */
			RTW_INFO("%s,SET MP_TX_MODE_SWITCH\n", __func__);
			break;
		case RTW_MP_TX_CHECK_TX_IDLE:
			RTW_INFO("%s,GET RTW_MP_TX_CHECK_TX_IDLE !\n", __func__);
			break;
		default:
				RTW_INFO("%s,SET MP_TX_MODE None\n", __func__);
			return 0;
	}

	rtw_mp_set_phl_cmd(padapter, (void*)&tx_arg, sizeof(struct rtw_mp_tx_arg));
	while (i != 100) {
		rtw_mp_get_phl_cmd(padapter, (void*)&tx_arg, sizeof(struct rtw_mp_tx_arg));
		if (tx_arg.cmd_ok && tx_arg.status == RTW_PHL_STATUS_SUCCESS) {
			RTW_INFO("%s,SET CMD OK\n", __func__);
			if (cmdid == RTW_MP_TX_CMD_PHY_OK) {
				RTW_INFO("%s, Get Tx Rpt OK CNT:%d\n", __func__, tx_arg.tx_ok);
				padapter->mppriv.tx.sended = tx_arg.tx_ok;
			} else if (cmdid == RTW_MP_TX_CHECK_TX_IDLE){
				RTW_INFO("%s, Get Tx state:%d\n", __func__, tx_arg.tx_state);
				pmppriv->rtw_mp_tx_state = tx_arg.tx_state;
			}
			break;
		} else {
			if (i > 100) {
				RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__, tx_arg.status);
				break;
			}
			i++;
			rtw_msleep_os(10);
		}
	}
	return (tx_arg.cmd_ok);
}


static thread_return mp_xmit_phl_packet_thread(thread_context context)
{

	struct mp_priv	*pmppriv = (struct mp_priv *)context;
	struct mp_tx	*pmptx = &pmppriv->tx;
	_adapter *padapter = pmppriv->papdater;
	struct pkt_attrib *pattrib = &pmppriv->tx.attrib;
	struct dvobj_priv	*dvobj = adapter_to_dvobj(padapter);
	struct rtw_trx_test_param test_param = {0};
	u16 i = 0;

	RTW_INFO("%s: Start !!\n", __func__);
	rtw_phl_trx_default_param(GET_PHL_INFO(dvobj), &test_param);

	test_param.tx_cap.f_rate = (u16)pmppriv->rateidx;
	test_param.tx_payload_size = pattrib->pktlen;
	test_param.tx_cap.f_gi_ltf = 0;
	test_param.tx_cap.f_stbc = 0;
	test_param.tx_cap.f_ldpc = 0;
	RTW_INFO("%s: Start !! tx method %d\n", __func__, pmppriv->rtw_mp_tx_method);
	while (1) {
		if (pmppriv->rtw_mp_tx_method == RTW_MP_TMACT_TX) {
			/*/if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_PACKETS, pmppriv->rtw_mp_tx_method, _TRUE)) {*/
			if (rtw_phl_trx_testsuite(GET_PHL_INFO(dvobj), &test_param) == 0)
				RTW_DBG("%s: rtw_phl_trx_testsuite!!\n", __func__);
			else
				rtw_usleep_os(10);
		}
				pmptx->sended++;
				pmppriv->tx_pktcount++;
		rtw_usleep_os(padapter->mppriv.pktInterval);

		if (pmptx->stop || RTW_CANNOT_RUN(adapter_to_dvobj(padapter)))
			goto exit;
		if ((pmptx->count != 0) && (pmptx->count == pmptx->sended))
			goto exit;
		flush_signals_thread();
	}
exit:
	pmptx->stop = 1;

	RTW_INFO("%s: Exit !!\n", __func__);
	rtw_thread_exit(NULL);
	return 0;
}

static void rtw_get_tx_idle(_adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	u8 j;

	pmp_priv->rtw_mp_tx_state = 0;
	for (j = 0; j < 10 ; j++) {
		if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_CHECK_TX_IDLE,
							pmp_priv->rtw_mp_tx_method, _FALSE))
		if (pmp_priv->rtw_mp_tx_state == 1)
			break;
		rtw_msleep_os(1);
	}
	return;
}

static void rtw_dpd_bypass(_adapter *padapter, u8 phy_idx)
{
	u32 data32 = 0;
	u32 dpd_bypass_val = 0;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct mp_priv *pmp_priv = &padapter->mppriv;

	if (pmp_priv->rtw_mp_tx_method != RTW_MP_TMACT_TX)
		rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_MODE_SWITCH, RTW_MP_TMACT_TX, _FALSE);

	if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_PACKETS, RTW_MP_SW_TX, _TRUE))
		RTW_INFO("%s: RTW_MP_SW_TX!!\n", __func__);
	else
		RTW_INFO("%s: RTW_MP_SW_TX fail!!\n", __func__);

	if (pmp_priv->rtw_mp_tx_method != RTW_MP_TMACT_TX)
		rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_MODE_SWITCH, pmp_priv->rtw_mp_tx_method, _FALSE);

	if (phy_idx == 0) {
		data32 = rtw_phl_read32(dvobj->phl, 0xD6F0);
		RTW_INFO("%s:Phy0 read 0xD6F0 = 0x%x !!\n", __func__, data32);
		dpd_bypass_val = ((data32 & 0x4000000) >> 26);
		RTW_INFO("%s: dpd_bypass_val = 0x%x !!\n", __func__, dpd_bypass_val);
	} else if (phy_idx == 1) {
		data32 = rtw_phl_read32(dvobj->phl, 0xF6F0);
		RTW_INFO("%s:Phy1 read 0xF6F0 = 0x%x !!\n", __func__, data32);
		dpd_bypass_val = ((data32 & 0x4000000) >> 26);
		RTW_INFO("%s: dpd_bypass_val = 0x%x !!\n", __func__, dpd_bypass_val);
	}

}

static void rtw_pretx_trkdpk(_adapter *padapter)
{
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct mp_priv *pmp_priv = &padapter->mppriv;
	u8 tx_nss = get_phy_tx_nss(padapter, padapter_link);
	u8 rfpath_i = 0;
	u8 tssi_mode = pmp_priv->tssi_mode;
	u32 bk_tx_stop = pmp_priv->tx.stop;
	u32 bk_tx_count = pmp_priv->tx.count;
	s16 bk_txpwr = pmp_priv->txpowerdbm;

	if (pmp_priv->tssi_mode == RTW_MP_TSSI_ON && bk_txpwr > 17) {
		pmp_priv->txpowerdbm = 17;
		for (rfpath_i = 0 ; rfpath_i < tx_nss; rfpath_i ++)
			rtw_mp_txpower_dbm(padapter, rfpath_i);
	}

	pmp_priv->tx.stop = 0;
	pmp_priv->tx.count = 0;
	pmp_priv->rtw_mp_tx_time = 5484;

	if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_PACKETS, RTW_MP_PMACT_TX, _TRUE))
		RTW_INFO("%s: RTW_MP_TX_PACKETS!!\n", __func__);
	rtw_msleep_os(20);

	if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_PACKETS, RTW_MP_PMACT_TX, _FALSE))
		RTW_INFO("%s: RTW_MP_TX_PACKETS Done!!\n", __func__);

	rtw_mp_cal_trigger(padapter, RTW_MP_CAL_DPK);

	/*End Workaround to trackiing DPK */
	pmp_priv->tx.stop = bk_tx_stop;
	pmp_priv->tx.count = bk_tx_count;
	pmp_priv->rtw_mp_tx_time = 0;

	if (pmp_priv->tssi_mode == RTW_MP_TSSI_ON && bk_txpwr > 17) {
		pmp_priv->txpowerdbm = bk_txpwr;
		for (rfpath_i = 0 ; rfpath_i < tx_nss; rfpath_i ++)
			rtw_mp_txpower_dbm(padapter, rfpath_i);
	}

}

void rtw_set_phl_packet_tx(_adapter *padapter, u8 bStart)
{
	struct mp_priv *pmp_priv;
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	u8 rfpath_i = 0;
	u8 tx_nss = get_phy_tx_nss(padapter, padapter_link);
	pmp_priv = &padapter->mppriv;


	for (rfpath_i = 0 ; rfpath_i < tx_nss; rfpath_i ++)
		rtw_mp_txpower_dbm(padapter, rfpath_i);

	RTW_INFO("%s: PACKET TX tx method %d!!\n", __func__, pmp_priv->rtw_mp_tx_method);

		if (bStart) {
			RTW_INFO("%s: !! tx method %d\n", __func__, pmp_priv->rtw_mp_tx_method);
			pmp_priv->tx.sended = 0;
			pmp_priv->tx.stop = 0;
			pmp_priv->tx_pktcount = 0;
			if (pmp_priv->rtw_mp_tx_method == RTW_MP_PMACT_TX ||
					pmp_priv->rtw_mp_tx_method == RTW_MP_FW_PMACT_TX) {
					RTW_INFO("%s: PLCP_USER_INFO & PLCP_COMMON_INFO!!\n", __func__);
					if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_CONFIG_PLCP_USER_INFO,
											pmp_priv->rtw_mp_tx_method, _TRUE) == true)
						RTW_INFO("%s: PLCP_USER_INFO done!!\n", __func__);
					if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_CONFIG_PLCP_COMMON_INFO,
												pmp_priv->rtw_mp_tx_method, _TRUE) == true)
						RTW_INFO("%s: RTW_MP_TX_CONFIG_PLCP_COMMON_INFO done!!\n", __func__);
					rtw_get_tx_idle(padapter);
					rtw_pretx_trkdpk(padapter);
			}
			rtw_dpd_bypass(padapter, pmp_priv->rtw_mp_cur_phy);
			rtw_get_tx_idle(padapter);

			if (pmp_priv->rtw_mp_tx_method == RTW_MP_FW_PMACT_TX) {
				if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_PACKETS, pmp_priv->rtw_mp_tx_method, _TRUE))
					RTW_INFO("%s: RTW_MP_FW_PMACT_TX!!\n", __func__);
				else
					RTW_INFO("%s: RTW_MP_FW_PMACT_TX fail!!\n", __func__);
			} else if (pmp_priv->rtw_mp_tx_method == RTW_MP_PMACT_TX) {
				if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_PACKETS, pmp_priv->rtw_mp_tx_method, _TRUE))
					RTW_DBG("%s: RTW_MP_TX_PACKETS!!\n", __func__);
			} else {
				pmp_priv->tx.PktTxThread = rtw_thread_start(
										mp_xmit_phl_packet_thread, pmp_priv, "RTW_MP_Tx_THREAD");
				if (pmp_priv->tx.PktTxThread == NULL)
					RTW_ERR("Create PktTx Thread Fail !!!!!\n");
			}
	}
		if (!bStart) {
			if (pmp_priv->rtw_mp_tx_method == RTW_MP_FW_PMACT_TX) {
				if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_PACKETS, pmp_priv->rtw_mp_tx_method, false)) {
					RTW_INFO("%s: RTW_MP_FW_TMACT_TX Stop TX!!\n", __func__);
					return;
				}
			} else if (pmp_priv->rtw_mp_tx_method == RTW_MP_PMACT_TX) {
				if (rtw_phl_mp_tx_cmd(padapter, RTW_MP_TX_PACKETS, pmp_priv->rtw_mp_tx_method, false)) {
					RTW_INFO("%s: RTW_MP_PMACT_TX Stop TX!!\n", __func__);
					return;
				}
			}
		}
}

void rtw_mp_set_packet_tx(_adapter *padapter)
{
	u8 *ptr, *pkt_start, *pkt_end;
	u32 pkt_size = 0, i = 0, idx = 0, tmp_idx = 0;
	struct rtw_ieee80211_hdr *hdr;
	u8 payload;
	s32 bmcast;
	struct pkt_attrib *pattrib;
	struct mp_priv *pmp_priv;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	pmp_priv = &padapter->mppriv;

	if (pmp_priv->tx.stop)
		return;
	pmp_priv->tx.sended = 0;
	pmp_priv->tx.stop = 0;
	pmp_priv->tx_pktcount = 0;

	/* 3 1. update_attrib() */
	pattrib = &pmp_priv->tx.attrib;
	_rtw_memcpy(pattrib->src, adapter_mac_addr(padapter), ETH_ALEN);
	_rtw_memcpy(pattrib->ta, pattrib->src, ETH_ALEN);
	_rtw_memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
	bmcast = IS_MCAST(pattrib->ra);
	if (bmcast)
		pattrib->psta = rtw_get_bcmc_stainfo(padapter, padapter_link);
	else
		pattrib->psta = rtw_get_stainfo(&padapter->stapriv, get_bssid(&padapter->mlmepriv));

	if (pattrib->psta == NULL) {
		RTW_INFO("%s:psta = NULL !!\n", __func__);
		return;
	}

	pattrib->mac_id = pattrib->psta->phl_sta->macid;
	pattrib->mbssid = 0;

	pattrib->last_txcmdsz = pattrib->hdrlen + pattrib->pktlen;

	/* 3 2. allocate xmit buffer */
	pkt_size = pattrib->last_txcmdsz;

	if (pmp_priv->tx.pallocated_buf)
		rtw_mfree(pmp_priv->tx.pallocated_buf, pmp_priv->tx.buf_size);
	pmp_priv->tx.write_size = pkt_size;
	pmp_priv->tx.buf_size = pkt_size + SZ_ALIGN_XMITFRAME_EXT;
	pmp_priv->tx.pallocated_buf = rtw_zmalloc(pmp_priv->tx.buf_size);
	if (pmp_priv->tx.pallocated_buf == NULL) {
		RTW_INFO("%s: malloc(%d) fail!!\n", __func__, pmp_priv->tx.buf_size);
		return;
	}
	pmp_priv->tx.buf = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pmp_priv->tx.pallocated_buf), SZ_ALIGN_XMITFRAME_EXT);
	ptr = pmp_priv->tx.buf;

	_rtw_memset(pmp_priv->tx.desc, 0, TXDESC_SIZE);
	pkt_start = ptr;
	pkt_end = pkt_start + pkt_size;

	/* 3 3. init TX descriptor */
#if defined(CONFIG_RTL8822B)
	if (IS_HARDWARE_TYPE_8822B(padapter))
		rtl8822b_prepare_mp_txdesc(padapter, pmp_priv);
#endif /* CONFIG_RTL8822B */

#if defined(CONFIG_RTL8822C)
	if (IS_HARDWARE_TYPE_8822C(padapter))
		rtl8822c_prepare_mp_txdesc(padapter, pmp_priv);
#endif /* CONFIG_RTL8822C */

#if defined(CONFIG_RTL8821C)
	if (IS_HARDWARE_TYPE_8821C(padapter))
		rtl8821c_prepare_mp_txdesc(padapter, pmp_priv);
#endif /* CONFIG_RTL8821C */

#if defined(CONFIG_RTL8814B)
	if (IS_HARDWARE_TYPE_8814B(padapter))
		rtl8814b_prepare_mp_txdesc(padapter, pmp_priv);
#endif /* CONFIG_RTL8814B */

	/* 3 4. make wlan header, make_wlanhdr() */
	hdr = (struct rtw_ieee80211_hdr *)pkt_start;
	set_frame_sub_type(&hdr->frame_ctl, pattrib->subtype);

	_rtw_memcpy(hdr->addr1, pattrib->dst, ETH_ALEN); /* DA */
	_rtw_memcpy(hdr->addr2, pattrib->src, ETH_ALEN); /* SA */
	_rtw_memcpy(hdr->addr3, get_bssid(&padapter->mlmepriv), ETH_ALEN); /* RA, BSSID */

	/* 3 5. make payload */
	ptr = pkt_start + pattrib->hdrlen;

	if (pmp_priv->mplink_btx == _TRUE) {
		_rtw_memcpy(ptr, pmp_priv->mplink_buf, pkt_end - ptr);
	} else {
		switch (pmp_priv->tx.payload) {
		case MP_TX_Payload_00:
			RTW_INFO("MP packet tx 0x00 payload!\n");
			payload = 0x00;
			_rtw_memset(ptr, 0x00, pkt_end - ptr);
			break;
		case MP_TX_Payload_5a:
			RTW_INFO("MP packet tx 0x5a payload!\n");
			payload = 0x5a;
			_rtw_memset(ptr, 0x5a, pkt_end - ptr);
			break;
		case MP_TX_Payload_a5:
			RTW_INFO("MP packet tx 0xa5 payload!\n");
			payload = 0xa5;
			_rtw_memset(ptr, 0xa5, pkt_end - ptr);
			break;
		case MP_TX_Payload_ff:
			RTW_INFO("MP packet tx 0xff payload!\n");
			payload = 0xff;
			_rtw_memset(ptr, 0xff, pkt_end - ptr);
			break;
		case MP_TX_Payload_prbs9:
			RTW_INFO("MP packet tx PRBS9 payload!\n");
			while (idx <= pkt_end - ptr) {
				int start = 0x02;
				int a = start;

				for (i = 0;; i++) {
						int newbit = (((a >> 8) ^ (a >> 4)) & 1);
						a = ((a << 1) | newbit) & 0x1ff;
						RTW_DBG("%x ", a);
						ptr[idx + i] = a;

						if (a == start) {
							RTW_INFO("payload repetition period is %d , end %d\n", i , idx);
							tmp_idx += i;
							break;
						}
						if (idx + i >= (pkt_end - ptr)) {
							tmp_idx += (idx + i);
							RTW_INFO(" repetition period payload end curr ptr %d\n", idx + i);
							break;
						}
				}
				idx = tmp_idx;
			}
			break;
		case MP_TX_Payload_default_random:
			RTW_INFO("MP packet tx default random payload!\n");
			for (i = 0; i < pkt_end - ptr; i++)
				ptr[i] = rtw_random32() % 0xFF;
			break;
		default:
			RTW_INFO("Config payload type default use 0x%x\n!", pmp_priv->tx.payload);
			_rtw_memset(ptr, pmp_priv->tx.payload, pkt_end - ptr);
			break;
		}
	}
	/* 3 6. start thread */
#ifdef PLATFORM_LINUX
	pmp_priv->tx.PktTxThread = rtw_thread_start(mp_xmit_packet_thread, pmp_priv, "RTW_MP_THREAD");
	if (pmp_priv->tx.PktTxThread == NULL)
		RTW_ERR("Create PktTx Thread Fail !!!!!\n");
#endif
#ifdef PLATFORM_FREEBSD
	{
		struct proc *p;
		struct thread *td;
		pmp_priv->tx.PktTxThread = kproc_kthread_add(mp_xmit_packet_thread, pmp_priv,
			&p, &td, RFHIGHPID, 0, "MPXmitThread", "MPXmitThread");

		if (pmp_priv->tx.PktTxThread < 0)
			RTW_INFO("Create PktTx Thread Fail !!!!!\n");
	}
#endif

	Rtw_MPSetMacTxEDCA(padapter);
	return;
}

void rtw_mp_reset_phy_count(_adapter *adapter)
{
	RTW_INFO("%s()!!!\n", __func__);
	rtw_mp_phl_config_arg(adapter, RTW_MP_CONFIG_CMD_SET_RESET_PHY_COUNT);
	rtw_mp_phl_config_arg(adapter, RTW_MP_CONFIG_CMD_SET_RESET_MAC_COUNT);
	rtw_mp_phl_config_arg(adapter, RTW_MP_CONFIG_CMD_SET_RESET_DRV_COUNT);
}

struct psd_init_regs {
	/* 3 wire */
	int reg_88c;
	int reg_c00;
	int reg_e00;
	int reg_1800;
	int reg_1a00;
	/* cck */
	int reg_800;
	int reg_808;
};

static int rtw_mp_psd_init(_adapter *padapter, struct psd_init_regs *regs)
{
	u8 rf_type = GET_HAL_RFPATH(adapter_to_dvobj(padapter));
#if 0
	switch (rf_type) {
	/* 1R */
	case RF_1T1R:
		if (rtw_hw_chk_proto_cap(padapter, PROTO_CAP_11AC)) {
			/* 11AC 1R PSD Setting 3wire & cck off */
			regs->reg_c00 = rtw_read32(padapter, 0xC00);
			phy_set_bb_reg(padapter, 0xC00, 0x3, 0x00);
			regs->reg_808 = rtw_read32(padapter, 0x808);
			phy_set_bb_reg(padapter, 0x808, 0x10000000, 0x0);
		} else {
			/* 11N 3-wire off 1 */
			regs->reg_88c = rtw_read32(padapter, 0x88C);
			phy_set_bb_reg(padapter, 0x88C, 0x300000, 0x3);
			/* 11N CCK off */
			regs->reg_800 = rtw_read32(padapter, 0x800);
			phy_set_bb_reg(padapter, 0x800, 0x1000000, 0x0);
		}
	break;

	/* 2R */
	case RF_1T2R:
	case RF_2T2R:
		if (rtw_hw_chk_proto_cap(padapter, PROTO_CAP_11AC)) {
			/* 11AC 2R PSD Setting 3wire & cck off */
			regs->reg_c00 = rtw_read32(padapter, 0xC00);
			regs->reg_e00 = rtw_read32(padapter, 0xE00);
			phy_set_bb_reg(padapter, 0xC00, 0x3, 0x00);
			phy_set_bb_reg(padapter, 0xE00, 0x3, 0x00);
			regs->reg_808 = rtw_read32(padapter, 0x808);
			phy_set_bb_reg(padapter, 0x808, 0x10000000, 0x0);
		} else {
			/* 11N 3-wire off 2 */
			regs->reg_88c = rtw_read32(padapter, 0x88C);
			phy_set_bb_reg(padapter, 0x88C, 0xF00000, 0xF);
			/* 11N CCK off */
			regs->reg_800 = rtw_read32(padapter, 0x800);
			phy_set_bb_reg(padapter, 0x800, 0x1000000, 0x0);
		}
	break;

	/* 3R */
	case RF_2T3R:
	case RF_3T3R:
		if (rtw_hw_chk_proto_cap(padapter, PROTO_CAP_11AC)) {
			/* 11AC 3R PSD Setting 3wire & cck off */
			regs->reg_c00 = rtw_read32(padapter, 0xC00);
			regs->reg_e00 = rtw_read32(padapter, 0xE00);
			regs->reg_1800 = rtw_read32(padapter, 0x1800);
			phy_set_bb_reg(padapter, 0xC00, 0x3, 0x00);
			phy_set_bb_reg(padapter, 0xE00, 0x3, 0x00);
			phy_set_bb_reg(padapter, 0x1800, 0x3, 0x00);
			regs->reg_808 = rtw_read32(padapter, 0x808);
			phy_set_bb_reg(padapter, 0x808, 0x10000000, 0x0);
		} else {
			RTW_ERR("%s: 11n don't support 3R\n", __func__);
			return -1;
		}
		break;

	/* 4R */
	case RF_2T4R:
	case RF_3T4R:
	case RF_4T4R:
		if (rtw_hw_chk_proto_cap(padapter, PROTO_CAP_11AC)) {
			/* 11AC 4R PSD Setting 3wire & cck off */
			regs->reg_c00 = rtw_read32(padapter, 0xC00);
			regs->reg_e00 = rtw_read32(padapter, 0xE00);
			regs->reg_1800 = rtw_read32(padapter, 0x1800);
			regs->reg_1a00 = rtw_read32(padapter, 0x1A00);
			phy_set_bb_reg(padapter, 0xC00, 0x3, 0x00);
			phy_set_bb_reg(padapter, 0xE00, 0x3, 0x00);
			phy_set_bb_reg(padapter, 0x1800, 0x3, 0x00);
			phy_set_bb_reg(padapter, 0x1A00, 0x3, 0x00);
			regs->reg_808 = rtw_read32(padapter, 0x808);
			phy_set_bb_reg(padapter, 0x808, 0x10000000, 0x0);
		} else {
			RTW_ERR("%s: 11n don't support 4R\n", __func__);
			return -1;
		}
		break;

	default:
		RTW_ERR("%s: unknown %d rf type\n", __func__,
			GET_HAL_RFPATH(adapter_to_dvobj(padapter)));
		return -1;
	}

	/* Set PSD points, 0=128, 1=256, 2=512, 3=1024 */
	if (rtw_hw_chk_proto_cap(padapter, PROTO_CAP_11AC))
		phy_set_bb_reg(padapter, 0x910, 0xC000, 3);
	else
		phy_set_bb_reg(padapter, 0x808, 0xC000, 3);
#endif
	RTW_INFO("%s: set %d rf type done\n", __func__,
		GET_HAL_RFPATH(adapter_to_dvobj(padapter)));
	return 0;
}

static int rtw_mp_psd_close(_adapter *padapter, struct psd_init_regs *regs)
{
	u8 rf_type = GET_HAL_RFPATH(adapter_to_dvobj(padapter));

#if 0
	if (!rtw_hw_chk_proto_cap(padapter, PROTO_CAP_11AC)) {
		/* 11n 3wire restore */
		rtw_write32(padapter, 0x88C, regs->reg_88c);
		/* 11n cck restore */
		rtw_write32(padapter, 0x800, regs->reg_800);
		RTW_INFO("%s: restore %d rf type\n", __func__, rf_type);
		return 0;
	}

	/* 11ac 3wire restore */
	switch (rf_type) {
	case RF_1T1R:
		rtw_write32(padapter, 0xC00, regs->reg_c00);
		break;
	case RF_1T2R:
	case RF_2T2R:
		rtw_write32(padapter, 0xC00, regs->reg_c00);
		rtw_write32(padapter, 0xE00, regs->reg_e00);
		break;
	case RF_2T3R:
	case RF_3T3R:
		rtw_write32(padapter, 0xC00, regs->reg_c00);
		rtw_write32(padapter, 0xE00, regs->reg_e00);
		rtw_write32(padapter, 0x1800, regs->reg_1800);
		break;
	case RF_2T4R:
	case RF_3T4R:
	case RF_4T4R:
		rtw_write32(padapter, 0xC00, regs->reg_c00);
		rtw_write32(padapter, 0xE00, regs->reg_e00);
		rtw_write32(padapter, 0x1800, regs->reg_1800);
		rtw_write32(padapter, 0x1A00, regs->reg_1a00);
		break;
	default:
		RTW_WARN("%s: unknown %d rf type\n", __func__, rf_type);
		break;
	}

	/* 11ac cck restore */
	rtw_write32(padapter, 0x808, regs->reg_808);
#endif	
	RTW_INFO("%s: restore %d rf type done\n", __func__, rf_type);
	return 0;
}

/* reg 0x808[9:0]: FFT data x
 * reg 0x808[22]:  0  -->  1  to get 1 FFT data y
 * reg 0x8B4[15:0]: FFT data y report */
static u32 rtw_GetPSDData(_adapter *adapter, u32 point)
{
	u32 psd_val = 0;
#if 0
#if defined(CONFIG_RTL8822B) || defined(CONFIG_RTL8821C) || defined(CONFIG_RTL8822C)
	u16 psd_reg = 0x910;
	u16 psd_regL = 0xF44;
#else
	u16 psd_reg = 0x808;
	u16 psd_regL = 0x8B4;
#endif

	psd_val = rtw_read32(adapter, psd_reg);

	psd_val &= 0xFFBFFC00;
	psd_val |= point;

	rtw_write32(adapter, psd_reg, psd_val);
	rtw_mdelay_os(1);
	psd_val |= 0x00400000;

	rtw_write32(adapter, psd_reg, psd_val);
	rtw_mdelay_os(1);

	psd_val = rtw_read32(adapter, psd_regL);
#if defined(CONFIG_RTL8821C)
	psd_val = (psd_val & 0x00FFFFFF) / 32;
#else
	psd_val &= 0x0000FFFF;
#endif
#endif
	return psd_val;
}

u8 rtw_mp_phl_psd_cmd(_adapter *padapter, struct rtw_mp_cal_arg	*psd_arg, u8 cmdid)
{
	struct mp_priv	*pmppriv = &padapter->mppriv;
	u16 i = 0;
	u32 cmd_size = sizeof(struct rtw_mp_cal_arg);

	psd_arg->mp_class = RTW_MP_CLASS_CAL;
	psd_arg->cmd = cmdid;

	rtw_mp_set_phl_cmd(padapter, (void*)psd_arg, cmd_size);
	while (i <= 100) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(padapter, (void*)psd_arg, cmd_size);
		if (psd_arg->cmd_ok && psd_arg->status == RTW_PHL_STATUS_SUCCESS) {
			RTW_INFO("%s,SET CMD OK\n", __func__);
			return psd_arg->cmd_ok;
		} else {
			if (i > 100) {
				RTW_DBG("%s,GET CMD FAIL !!! status %d\n", __func__, psd_arg->status);
				break;
			}
			i++;
		}
	}
	return 0;
}

/*
 * pts	start_point_min		stop_point_max
 * 128	64			64 + 128 = 192
 * 256	128			128 + 256 = 384
 * 512	256			256 + 512 = 768
 * 1024	512			512 + 1024 = 1536
 *
 */
u32 mp_query_psd(_adapter *adapter, u8 *data)
{
	struct mp_priv *pmppriv = &adapter->mppriv;
	struct rtw_mp_cal_arg	*my_psd_arg = NULL;

	u32 i, psd_pts = 0, psd_start = 0, psd_stop = 0;
	u32 fft = 0, avg = 0, iq_path = 0;
	u32 psd_data = 0;

	my_psd_arg = _rtw_malloc(sizeof(struct rtw_mp_cal_arg));

	if (my_psd_arg)
		_rtw_memset((void *)my_psd_arg, 0, sizeof(struct rtw_mp_cal_arg));
	else {
		RTW_ERR("%s() psd_arg malloc fail !\n", __func__);
		return 0;
	}

	if (sscanf(data, "fft=%d,avg=%d,iq=%d", &fft, &avg, &iq_path) == 3) {

		my_psd_arg->path = pmppriv->antenna_trx;
		my_psd_arg->iq_path = iq_path;
		my_psd_arg->avg = avg;
		my_psd_arg->fft = fft;

		RTW_INFO("%s: PSD RF_Path:%d fft=%d,avg=%d,iq_path=%d !\n",
			__func__, pmppriv->antenna_trx, fft, avg, iq_path);

		if (rtw_mp_phl_psd_cmd(adapter, my_psd_arg, RTW_MP_CAL_CMD_PSD_INIT))
			RTW_INFO("PSD_INIT CMD OK!\n");
		else
			RTW_INFO("PSD_INIT CMD FAIL!\n");
	} else if (strncmp(data, "restore", 7) == 0) {

		my_psd_arg->path = pmppriv->antenna_trx;

		if (rtw_mp_phl_psd_cmd(adapter, my_psd_arg, RTW_MP_CAL_CMD_PSD_RESTORE))
			RTW_INFO("PSD_RESTORE CMD OK!\n");
		else
			RTW_INFO("PSD_RESTORE CMD FAIL!\n");
	} else {
		if (sscanf(data, "pts=%d,start=%d,stop=%d",
			&psd_pts, &psd_start, &psd_stop) == 3) {
			my_psd_arg->upoint = psd_pts;
			my_psd_arg->start_point = psd_start;
			my_psd_arg->stop_point = psd_stop;
		} else {
			my_psd_arg->upoint = 1280;
			my_psd_arg->start_point = 640;
			my_psd_arg->stop_point = 960;
			RTW_INFO("Default PSD setting!\n");
		}

		if (rtw_mp_phl_psd_cmd(adapter, my_psd_arg, RTW_MP_CAL_CMD_PSD_QUERY))
			RTW_INFO("PSD_QUERY CMD OK!\n");
		else
			RTW_INFO("PSD_QUERY CMD FAIL!\n");

		data[0] = '\0';
		i = 0;
		while (i < 320) {
			sprintf(data, "%s%x ", data, (my_psd_arg->outbuf[i]));
			i++;
		}
	}

	if (my_psd_arg)
		_rtw_mfree(my_psd_arg, sizeof(struct rtw_mp_cal_arg));

	return strlen(data) + 1;
}

u8
mpt_to_mgnt_rate(
		u32	MptRateIdx
)
{
	/* Mapped to MGN_XXX defined in MgntGen.h */
	switch (MptRateIdx) {
	/* CCK rate. */
	case	MPT_RATE_1M:
		return MGN_1M;
	case	MPT_RATE_2M:
		return MGN_2M;
	case	MPT_RATE_55M:
		return MGN_5_5M;
	case	MPT_RATE_11M:
		return MGN_11M;

	/* OFDM rate. */
	case	MPT_RATE_6M:
		return MGN_6M;
	case	MPT_RATE_9M:
		return MGN_9M;
	case	MPT_RATE_12M:
		return MGN_12M;
	case	MPT_RATE_18M:
		return MGN_18M;
	case	MPT_RATE_24M:
		return MGN_24M;
	case	MPT_RATE_36M:
		return MGN_36M;
	case	MPT_RATE_48M:
		return MGN_48M;
	case	MPT_RATE_54M:
		return MGN_54M;

	/* HT rate. */
	case	MPT_RATE_MCS0:
		return MGN_MCS0;
	case	MPT_RATE_MCS1:
		return MGN_MCS1;
	case	MPT_RATE_MCS2:
		return MGN_MCS2;
	case	MPT_RATE_MCS3:
		return MGN_MCS3;
	case	MPT_RATE_MCS4:
		return MGN_MCS4;
	case	MPT_RATE_MCS5:
		return MGN_MCS5;
	case	MPT_RATE_MCS6:
		return MGN_MCS6;
	case	MPT_RATE_MCS7:
		return MGN_MCS7;
	case	MPT_RATE_MCS8:
		return MGN_MCS8;
	case	MPT_RATE_MCS9:
		return MGN_MCS9;
	case	MPT_RATE_MCS10:
		return MGN_MCS10;
	case	MPT_RATE_MCS11:
		return MGN_MCS11;
	case	MPT_RATE_MCS12:
		return MGN_MCS12;
	case	MPT_RATE_MCS13:
		return MGN_MCS13;
	case	MPT_RATE_MCS14:
		return MGN_MCS14;
	case	MPT_RATE_MCS15:
		return MGN_MCS15;
	case	MPT_RATE_MCS16:
		return MGN_MCS16;
	case	MPT_RATE_MCS17:
		return MGN_MCS17;
	case	MPT_RATE_MCS18:
		return MGN_MCS18;
	case	MPT_RATE_MCS19:
		return MGN_MCS19;
	case	MPT_RATE_MCS20:
		return MGN_MCS20;
	case	MPT_RATE_MCS21:
		return MGN_MCS21;
	case	MPT_RATE_MCS22:
		return MGN_MCS22;
	case	MPT_RATE_MCS23:
		return MGN_MCS23;
	case	MPT_RATE_MCS24:
		return MGN_MCS24;
	case	MPT_RATE_MCS25:
		return MGN_MCS25;
	case	MPT_RATE_MCS26:
		return MGN_MCS26;
	case	MPT_RATE_MCS27:
		return MGN_MCS27;
	case	MPT_RATE_MCS28:
		return MGN_MCS28;
	case	MPT_RATE_MCS29:
		return MGN_MCS29;
	case	MPT_RATE_MCS30:
		return MGN_MCS30;
	case	MPT_RATE_MCS31:
		return MGN_MCS31;

	/* VHT rate. */
	case	MPT_RATE_VHT1SS_MCS0:
		return MGN_VHT1SS_MCS0;
	case	MPT_RATE_VHT1SS_MCS1:
		return MGN_VHT1SS_MCS1;
	case	MPT_RATE_VHT1SS_MCS2:
		return MGN_VHT1SS_MCS2;
	case	MPT_RATE_VHT1SS_MCS3:
		return MGN_VHT1SS_MCS3;
	case	MPT_RATE_VHT1SS_MCS4:
		return MGN_VHT1SS_MCS4;
	case	MPT_RATE_VHT1SS_MCS5:
		return MGN_VHT1SS_MCS5;
	case	MPT_RATE_VHT1SS_MCS6:
		return MGN_VHT1SS_MCS6;
	case	MPT_RATE_VHT1SS_MCS7:
		return MGN_VHT1SS_MCS7;
	case	MPT_RATE_VHT1SS_MCS8:
		return MGN_VHT1SS_MCS8;
	case	MPT_RATE_VHT1SS_MCS9:
		return MGN_VHT1SS_MCS9;
	case	MPT_RATE_VHT2SS_MCS0:
		return MGN_VHT2SS_MCS0;
	case	MPT_RATE_VHT2SS_MCS1:
		return MGN_VHT2SS_MCS1;
	case	MPT_RATE_VHT2SS_MCS2:
		return MGN_VHT2SS_MCS2;
	case	MPT_RATE_VHT2SS_MCS3:
		return MGN_VHT2SS_MCS3;
	case	MPT_RATE_VHT2SS_MCS4:
		return MGN_VHT2SS_MCS4;
	case	MPT_RATE_VHT2SS_MCS5:
		return MGN_VHT2SS_MCS5;
	case	MPT_RATE_VHT2SS_MCS6:
		return MGN_VHT2SS_MCS6;
	case	MPT_RATE_VHT2SS_MCS7:
		return MGN_VHT2SS_MCS7;
	case	MPT_RATE_VHT2SS_MCS8:
		return MGN_VHT2SS_MCS8;
	case	MPT_RATE_VHT2SS_MCS9:
		return MGN_VHT2SS_MCS9;
	case	MPT_RATE_VHT3SS_MCS0:
		return MGN_VHT3SS_MCS0;
	case	MPT_RATE_VHT3SS_MCS1:
		return MGN_VHT3SS_MCS1;
	case	MPT_RATE_VHT3SS_MCS2:
		return MGN_VHT3SS_MCS2;
	case	MPT_RATE_VHT3SS_MCS3:
		return MGN_VHT3SS_MCS3;
	case	MPT_RATE_VHT3SS_MCS4:
		return MGN_VHT3SS_MCS4;
	case	MPT_RATE_VHT3SS_MCS5:
		return MGN_VHT3SS_MCS5;
	case	MPT_RATE_VHT3SS_MCS6:
		return MGN_VHT3SS_MCS6;
	case	MPT_RATE_VHT3SS_MCS7:
		return MGN_VHT3SS_MCS7;
	case	MPT_RATE_VHT3SS_MCS8:
		return MGN_VHT3SS_MCS8;
	case	MPT_RATE_VHT3SS_MCS9:
		return MGN_VHT3SS_MCS9;
	case	MPT_RATE_VHT4SS_MCS0:
		return MGN_VHT4SS_MCS0;
	case	MPT_RATE_VHT4SS_MCS1:
		return MGN_VHT4SS_MCS1;
	case	MPT_RATE_VHT4SS_MCS2:
		return MGN_VHT4SS_MCS2;
	case	MPT_RATE_VHT4SS_MCS3:
		return MGN_VHT4SS_MCS3;
	case	MPT_RATE_VHT4SS_MCS4:
		return MGN_VHT4SS_MCS4;
	case	MPT_RATE_VHT4SS_MCS5:
		return MGN_VHT4SS_MCS5;
	case	MPT_RATE_VHT4SS_MCS6:
		return MGN_VHT4SS_MCS6;
	case	MPT_RATE_VHT4SS_MCS7:
		return MGN_VHT4SS_MCS7;
	case	MPT_RATE_VHT4SS_MCS8:
		return MGN_VHT4SS_MCS8;
	case	MPT_RATE_VHT4SS_MCS9:
		return MGN_VHT4SS_MCS9;

	case	MPT_RATE_LAST:	/* fully automatiMGN_VHT2SS_MCS1;	 */
	default:
		RTW_INFO("<===mpt_to_mgnt_rate(), Invalid Rate: %d!!\n", MptRateIdx);
		return 0x0;
	}
}


u8 rtw_mp_hwrate2mptrate(u8 rate)
{
	u8	ret_rate = MGN_1M;

	switch (rate) {
	case DESC_RATE1M:
		ret_rate = MPT_RATE_1M;
		break;
	case DESC_RATE2M:
		ret_rate = MPT_RATE_2M;
		break;
	case DESC_RATE5_5M:
		ret_rate = MPT_RATE_55M;
		break;
	case DESC_RATE11M:
		ret_rate = MPT_RATE_11M;
		break;
	case DESC_RATE6M:
		ret_rate = MPT_RATE_6M;
		break;
	case DESC_RATE9M:
		ret_rate = MPT_RATE_9M;
		break;
	case DESC_RATE12M:
		ret_rate = MPT_RATE_12M;
		break;
	case DESC_RATE18M:
		ret_rate = MPT_RATE_18M;
		break;
	case DESC_RATE24M:
		ret_rate = MPT_RATE_24M;
		break;
	case DESC_RATE36M:
		ret_rate = MPT_RATE_36M;
		break;
	case DESC_RATE48M:
		ret_rate = MPT_RATE_48M;
		break;
	case DESC_RATE54M:
		ret_rate = MPT_RATE_54M;
		break;
	case DESC_RATEMCS0:
		ret_rate = MPT_RATE_MCS0;
		break;
	case DESC_RATEMCS1:
		ret_rate = MPT_RATE_MCS1;
		break;
	case DESC_RATEMCS2:
		ret_rate = MPT_RATE_MCS2;
		break;
	case DESC_RATEMCS3:
		ret_rate = MPT_RATE_MCS3;
		break;
	case DESC_RATEMCS4:
		ret_rate = MPT_RATE_MCS4;
		break;
	case DESC_RATEMCS5:
		ret_rate = MPT_RATE_MCS5;
		break;
	case DESC_RATEMCS6:
		ret_rate = MPT_RATE_MCS6;
		break;
	case DESC_RATEMCS7:
		ret_rate = MPT_RATE_MCS7;
		break;
	case DESC_RATEMCS8:
		ret_rate = MPT_RATE_MCS8;
		break;
	case DESC_RATEMCS9:
		ret_rate = MPT_RATE_MCS9;
		break;
	case DESC_RATEMCS10:
		ret_rate = MPT_RATE_MCS10;
		break;
	case DESC_RATEMCS11:
		ret_rate = MPT_RATE_MCS11;
		break;
	case DESC_RATEMCS12:
		ret_rate = MPT_RATE_MCS12;
		break;
	case DESC_RATEMCS13:
		ret_rate = MPT_RATE_MCS13;
		break;
	case DESC_RATEMCS14:
		ret_rate = MPT_RATE_MCS14;
		break;
	case DESC_RATEMCS15:
		ret_rate = MPT_RATE_MCS15;
		break;
	case DESC_RATEMCS16:
		ret_rate = MPT_RATE_MCS16;
		break;
	case DESC_RATEMCS17:
		ret_rate = MPT_RATE_MCS17;
		break;
	case DESC_RATEMCS18:
		ret_rate = MPT_RATE_MCS18;
		break;
	case DESC_RATEMCS19:
		ret_rate = MPT_RATE_MCS19;
		break;
	case DESC_RATEMCS20:
		ret_rate = MPT_RATE_MCS20;
		break;
	case DESC_RATEMCS21:
		ret_rate = MPT_RATE_MCS21;
		break;
	case DESC_RATEMCS22:
		ret_rate = MPT_RATE_MCS22;
		break;
	case DESC_RATEMCS23:
		ret_rate = MPT_RATE_MCS23;
		break;
	case DESC_RATEMCS24:
		ret_rate = MPT_RATE_MCS24;
		break;
	case DESC_RATEMCS25:
		ret_rate = MPT_RATE_MCS25;
		break;
	case DESC_RATEMCS26:
		ret_rate = MPT_RATE_MCS26;
		break;
	case DESC_RATEMCS27:
		ret_rate = MPT_RATE_MCS27;
		break;
	case DESC_RATEMCS28:
		ret_rate = MPT_RATE_MCS28;
		break;
	case DESC_RATEMCS29:
		ret_rate = MPT_RATE_MCS29;
		break;
	case DESC_RATEMCS30:
		ret_rate = MPT_RATE_MCS30;
		break;
	case DESC_RATEMCS31:
		ret_rate = MPT_RATE_MCS31;
		break;
	case DESC_RATEVHTSS1MCS0:
		ret_rate = MPT_RATE_VHT1SS_MCS0;
		break;
	case DESC_RATEVHTSS1MCS1:
		ret_rate = MPT_RATE_VHT1SS_MCS1;
		break;
	case DESC_RATEVHTSS1MCS2:
		ret_rate = MPT_RATE_VHT1SS_MCS2;
		break;
	case DESC_RATEVHTSS1MCS3:
		ret_rate = MPT_RATE_VHT1SS_MCS3;
		break;
	case DESC_RATEVHTSS1MCS4:
		ret_rate = MPT_RATE_VHT1SS_MCS4;
		break;
	case DESC_RATEVHTSS1MCS5:
		ret_rate = MPT_RATE_VHT1SS_MCS5;
		break;
	case DESC_RATEVHTSS1MCS6:
		ret_rate = MPT_RATE_VHT1SS_MCS6;
		break;
	case DESC_RATEVHTSS1MCS7:
		ret_rate = MPT_RATE_VHT1SS_MCS7;
		break;
	case DESC_RATEVHTSS1MCS8:
		ret_rate = MPT_RATE_VHT1SS_MCS8;
		break;
	case DESC_RATEVHTSS1MCS9:
		ret_rate = MPT_RATE_VHT1SS_MCS9;
		break;
	case DESC_RATEVHTSS2MCS0:
		ret_rate = MPT_RATE_VHT2SS_MCS0;
		break;
	case DESC_RATEVHTSS2MCS1:
		ret_rate = MPT_RATE_VHT2SS_MCS1;
		break;
	case DESC_RATEVHTSS2MCS2:
		ret_rate = MPT_RATE_VHT2SS_MCS2;
		break;
	case DESC_RATEVHTSS2MCS3:
		ret_rate = MPT_RATE_VHT2SS_MCS3;
		break;
	case DESC_RATEVHTSS2MCS4:
		ret_rate = MPT_RATE_VHT2SS_MCS4;
		break;
	case DESC_RATEVHTSS2MCS5:
		ret_rate = MPT_RATE_VHT2SS_MCS5;
		break;
	case DESC_RATEVHTSS2MCS6:
		ret_rate = MPT_RATE_VHT2SS_MCS6;
		break;
	case DESC_RATEVHTSS2MCS7:
		ret_rate = MPT_RATE_VHT2SS_MCS7;
		break;
	case DESC_RATEVHTSS2MCS8:
		ret_rate = MPT_RATE_VHT2SS_MCS8;
		break;
	case DESC_RATEVHTSS2MCS9:
		ret_rate = MPT_RATE_VHT2SS_MCS9;
		break;
	case DESC_RATEVHTSS3MCS0:
		ret_rate = MPT_RATE_VHT3SS_MCS0;
		break;
	case DESC_RATEVHTSS3MCS1:
		ret_rate = MPT_RATE_VHT3SS_MCS1;
		break;
	case DESC_RATEVHTSS3MCS2:
		ret_rate = MPT_RATE_VHT3SS_MCS2;
		break;
	case DESC_RATEVHTSS3MCS3:
		ret_rate = MPT_RATE_VHT3SS_MCS3;
		break;
	case DESC_RATEVHTSS3MCS4:
		ret_rate = MPT_RATE_VHT3SS_MCS4;
		break;
	case DESC_RATEVHTSS3MCS5:
		ret_rate = MPT_RATE_VHT3SS_MCS5;
		break;
	case DESC_RATEVHTSS3MCS6:
		ret_rate = MPT_RATE_VHT3SS_MCS6;
		break;
	case DESC_RATEVHTSS3MCS7:
		ret_rate = MPT_RATE_VHT3SS_MCS7;
		break;
	case DESC_RATEVHTSS3MCS8:
		ret_rate = MPT_RATE_VHT3SS_MCS8;
		break;
	case DESC_RATEVHTSS3MCS9:
		ret_rate = MPT_RATE_VHT3SS_MCS9;
		break;
	case DESC_RATEVHTSS4MCS0:
		ret_rate = MPT_RATE_VHT4SS_MCS0;
		break;
	case DESC_RATEVHTSS4MCS1:
		ret_rate = MPT_RATE_VHT4SS_MCS1;
		break;
	case DESC_RATEVHTSS4MCS2:
		ret_rate = MPT_RATE_VHT4SS_MCS2;
		break;
	case DESC_RATEVHTSS4MCS3:
		ret_rate = MPT_RATE_VHT4SS_MCS3;
		break;
	case DESC_RATEVHTSS4MCS4:
		ret_rate = MPT_RATE_VHT4SS_MCS4;
		break;
	case DESC_RATEVHTSS4MCS5:
		ret_rate = MPT_RATE_VHT4SS_MCS5;
		break;
	case DESC_RATEVHTSS4MCS6:
		ret_rate = MPT_RATE_VHT4SS_MCS6;
		break;
	case DESC_RATEVHTSS4MCS7:
		ret_rate = MPT_RATE_VHT4SS_MCS7;
		break;
	case DESC_RATEVHTSS4MCS8:
		ret_rate = MPT_RATE_VHT4SS_MCS8;
		break;
	case DESC_RATEVHTSS4MCS9:
		ret_rate = MPT_RATE_VHT4SS_MCS9;
		break;

	default:
		RTW_INFO("hwrate_to_mrate(): Non supported Rate [%x]!!!\n", rate);
		break;
	}
	return ret_rate;
}

static u16 srate2herate(u8 *str)
{

	RTW_INFO("%s(): %s!!!\n", __func__, str);
	if (!strcmp(str, "1M")) return HRATE_CCK1;
	else if (!strcmp(str, "2M")) return HRATE_CCK2;
	else if (!strcmp(str, "5.5M")) return HRATE_CCK5_5;
	else if (!strcmp(str, "11M")) return HRATE_CCK11;
	else if (!strcmp(str, "6M")) return HRATE_OFDM6;
	else if (!strcmp(str, "9M")) return HRATE_OFDM9;
	else if (!strcmp(str, "12M")) return HRATE_OFDM12;
	else if (!strcmp(str, "18M")) return HRATE_OFDM18;
	else if (!strcmp(str, "24M")) return HRATE_OFDM24;
	else if (!strcmp(str, "36M")) return HRATE_OFDM36;
	else if (!strcmp(str, "48M")) return HRATE_OFDM48;
	else if (!strcmp(str, "54M")) return HRATE_OFDM54;
	else if (!strcmp(str, "HTMCS0")) return HRATE_MCS0;
	else if (!strcmp(str, "HTMCS1")) return HRATE_MCS1;
	else if (!strcmp(str, "HTMCS2")) return HRATE_MCS2;
	else if (!strcmp(str, "HTMCS3")) return HRATE_MCS3;
	else if (!strcmp(str, "HTMCS4")) return HRATE_MCS4;
	else if (!strcmp(str, "HTMCS5")) return HRATE_MCS5;
	else if (!strcmp(str, "HTMCS6")) return HRATE_MCS6;
	else if (!strcmp(str, "HTMCS7")) return HRATE_MCS7;
	else if (!strcmp(str, "HTMCS8")) return HRATE_MCS8;
	else if (!strcmp(str, "HTMCS9")) return HRATE_MCS9;
	else if (!strcmp(str, "HTMCS10")) return HRATE_MCS10;
	else if (!strcmp(str, "HTMCS11")) return HRATE_MCS11;
	else if (!strcmp(str, "HTMCS12")) return HRATE_MCS12;
	else if (!strcmp(str, "HTMCS13")) return HRATE_MCS13;
	else if (!strcmp(str, "HTMCS14")) return HRATE_MCS14;
	else if (!strcmp(str, "HTMCS15")) return HRATE_MCS15;
	else if (!strcmp(str, "HTMCS16")) return HRATE_MCS16;
	else if (!strcmp(str, "HTMCS17")) return HRATE_MCS17;
	else if (!strcmp(str, "HTMCS18")) return HRATE_MCS18;
	else if (!strcmp(str, "HTMCS19")) return HRATE_MCS19;
	else if (!strcmp(str, "HTMCS20")) return HRATE_MCS20;
	else if (!strcmp(str, "HTMCS21")) return HRATE_MCS21;
	else if (!strcmp(str, "HTMCS22")) return HRATE_MCS22;
	else if (!strcmp(str, "HTMCS23")) return HRATE_MCS23;
	else if (!strcmp(str, "HTMCS24")) return HRATE_MCS24;
	else if (!strcmp(str, "HTMCS25")) return HRATE_MCS25;
	else if (!strcmp(str, "HTMCS26")) return HRATE_MCS26;
	else if (!strcmp(str, "HTMCS27")) return HRATE_MCS27;
	else if (!strcmp(str, "HTMCS28")) return HRATE_MCS28;
	else if (!strcmp(str, "HTMCS29")) return HRATE_MCS29;
	else if (!strcmp(str, "HTMCS30")) return HRATE_MCS30;
	else if (!strcmp(str, "HTMCS31")) return HRATE_MCS31;
	else if (!strcmp(str, "VHT1MCS0")) return HRATE_VHT_NSS1_MCS0;
	else if (!strcmp(str, "VHT1MCS1")) return HRATE_VHT_NSS1_MCS1;
	else if (!strcmp(str, "VHT1MCS2")) return HRATE_VHT_NSS1_MCS2;
	else if (!strcmp(str, "VHT1MCS3")) return HRATE_VHT_NSS1_MCS3;
	else if (!strcmp(str, "VHT1MCS4")) return HRATE_VHT_NSS1_MCS4;
	else if (!strcmp(str, "VHT1MCS5")) return HRATE_VHT_NSS1_MCS5;
	else if (!strcmp(str, "VHT1MCS6")) return HRATE_VHT_NSS1_MCS6;
	else if (!strcmp(str, "VHT1MCS7")) return HRATE_VHT_NSS1_MCS7;
	else if (!strcmp(str, "VHT1MCS8")) return HRATE_VHT_NSS1_MCS8;
	else if (!strcmp(str, "VHT1MCS9")) return HRATE_VHT_NSS1_MCS9;
	else if (!strcmp(str, "VHT2MCS0")) return HRATE_VHT_NSS2_MCS0;
	else if (!strcmp(str, "VHT2MCS1")) return HRATE_VHT_NSS2_MCS1;
	else if (!strcmp(str, "VHT2MCS2")) return HRATE_VHT_NSS2_MCS2;
	else if (!strcmp(str, "VHT2MCS3")) return HRATE_VHT_NSS2_MCS3;
	else if (!strcmp(str, "VHT2MCS4")) return HRATE_VHT_NSS2_MCS4;
	else if (!strcmp(str, "VHT2MCS5")) return HRATE_VHT_NSS2_MCS5;
	else if (!strcmp(str, "VHT2MCS6")) return HRATE_VHT_NSS2_MCS6;
	else if (!strcmp(str, "VHT2MCS7")) return HRATE_VHT_NSS2_MCS7;
	else if (!strcmp(str, "VHT2MCS8")) return HRATE_VHT_NSS2_MCS8;
	else if (!strcmp(str, "VHT2MCS9")) return HRATE_VHT_NSS2_MCS9;
	else if (!strcmp(str, "VHT3MCS0")) return HRATE_VHT_NSS3_MCS0;
	else if (!strcmp(str, "VHT3MCS1")) return HRATE_VHT_NSS3_MCS1;
	else if (!strcmp(str, "VHT3MCS2")) return HRATE_VHT_NSS3_MCS2;
	else if (!strcmp(str, "VHT3MCS3")) return HRATE_VHT_NSS3_MCS3;
	else if (!strcmp(str, "VHT3MCS4")) return HRATE_VHT_NSS3_MCS4;
	else if (!strcmp(str, "VHT3MCS5")) return HRATE_VHT_NSS3_MCS5;
	else if (!strcmp(str, "VHT3MCS6")) return HRATE_VHT_NSS3_MCS6;
	else if (!strcmp(str, "VHT3MCS7")) return HRATE_VHT_NSS3_MCS7;
	else if (!strcmp(str, "VHT3MCS8")) return HRATE_VHT_NSS3_MCS8;
	else if (!strcmp(str, "VHT3MCS9")) return HRATE_VHT_NSS3_MCS9;
	else if (!strcmp(str, "VHT4MCS0")) return HRATE_VHT_NSS4_MCS0;
	else if (!strcmp(str, "VHT4MCS1")) return HRATE_VHT_NSS4_MCS1;
	else if (!strcmp(str, "VHT4MCS2")) return HRATE_VHT_NSS4_MCS2;
	else if (!strcmp(str, "VHT4MCS3")) return HRATE_VHT_NSS4_MCS3;
	else if (!strcmp(str, "VHT4MCS4")) return HRATE_VHT_NSS4_MCS4;
	else if (!strcmp(str, "VHT4MCS5")) return HRATE_VHT_NSS4_MCS5;
	else if (!strcmp(str, "VHT4MCS6")) return HRATE_VHT_NSS4_MCS6;
	else if (!strcmp(str, "VHT4MCS7")) return HRATE_VHT_NSS4_MCS7;
	else if (!strcmp(str, "VHT4MCS8")) return HRATE_VHT_NSS4_MCS8;
	else if (!strcmp(str, "VHT4MCS9")) return HRATE_VHT_NSS4_MCS9;
	else if (!strcmp(str, "HE1MCS0")) return HRATE_HE_NSS1_MCS0;
	else if (!strcmp(str, "HE1MCS1")) return HRATE_HE_NSS1_MCS1;
	else if (!strcmp(str, "HE1MCS2")) return HRATE_HE_NSS1_MCS2;
	else if (!strcmp(str, "HE1MCS3")) return HRATE_HE_NSS1_MCS3;
	else if (!strcmp(str, "HE1MCS4")) return HRATE_HE_NSS1_MCS4;
	else if (!strcmp(str, "HE1MCS5")) return HRATE_HE_NSS1_MCS5;
	else if (!strcmp(str, "HE1MCS6")) return HRATE_HE_NSS1_MCS6;
	else if (!strcmp(str, "HE1MCS7")) return HRATE_HE_NSS1_MCS7;
	else if (!strcmp(str, "HE1MCS8")) return HRATE_HE_NSS1_MCS8;
	else if (!strcmp(str, "HE1MCS9")) return HRATE_HE_NSS1_MCS9;
	else if (!strcmp(str, "HE1MCS10")) return HRATE_HE_NSS1_MCS10;
	else if (!strcmp(str, "HE1MCS11")) return HRATE_HE_NSS1_MCS11;
	else if (!strcmp(str, "HE2MCS0")) return HRATE_HE_NSS2_MCS0;
	else if (!strcmp(str, "HE2MCS1")) return HRATE_HE_NSS2_MCS1;
	else if (!strcmp(str, "HE2MCS2")) return HRATE_HE_NSS2_MCS2;
	else if (!strcmp(str, "HE2MCS3")) return HRATE_HE_NSS2_MCS3;
	else if (!strcmp(str, "HE2MCS4")) return HRATE_HE_NSS2_MCS4;
	else if (!strcmp(str, "HE2MCS5")) return HRATE_HE_NSS2_MCS5;
	else if (!strcmp(str, "HE2MCS6")) return HRATE_HE_NSS2_MCS6;
	else if (!strcmp(str, "HE2MCS7")) return HRATE_HE_NSS2_MCS7;
	else if (!strcmp(str, "HE2MCS8")) return HRATE_HE_NSS2_MCS8;
	else if (!strcmp(str, "HE2MCS9")) return HRATE_HE_NSS2_MCS9;
	else if (!strcmp(str, "HE2MCS10")) return HRATE_HE_NSS2_MCS10;
	else if (!strcmp(str, "HE2MCS11")) return HRATE_HE_NSS2_MCS11;
	else if (!strcmp(str, "HE3MCS0")) return HRATE_HE_NSS3_MCS0;
	else if (!strcmp(str, "HE3MCS1")) return HRATE_HE_NSS3_MCS1;
	else if (!strcmp(str, "HE3MCS2")) return HRATE_HE_NSS3_MCS2;
	else if (!strcmp(str, "HE3MCS3")) return HRATE_HE_NSS3_MCS3;
	else if (!strcmp(str, "HE3MCS4")) return HRATE_HE_NSS3_MCS4;
	else if (!strcmp(str, "HE3MCS5")) return HRATE_HE_NSS3_MCS5;
	else if (!strcmp(str, "HE3MCS6")) return HRATE_HE_NSS3_MCS6;
	else if (!strcmp(str, "HE3MCS7")) return HRATE_HE_NSS3_MCS7;
	else if (!strcmp(str, "HE3MCS8")) return HRATE_HE_NSS3_MCS8;
	else if (!strcmp(str, "HE3MCS9")) return HRATE_HE_NSS3_MCS9;
	else if (!strcmp(str, "HE3MCS10")) return HRATE_HE_NSS3_MCS10;
	else if (!strcmp(str, "HE3MCS11")) return HRATE_HE_NSS3_MCS11;
	else if (!strcmp(str, "HE4MCS0")) return HRATE_HE_NSS4_MCS0;
	else if (!strcmp(str, "HE4MCS1")) return HRATE_HE_NSS4_MCS1;
	else if (!strcmp(str, "HE4MCS2")) return HRATE_HE_NSS4_MCS2;
	else if (!strcmp(str, "HE4MCS3")) return HRATE_HE_NSS4_MCS3;
	else if (!strcmp(str, "HE4MCS4")) return HRATE_HE_NSS4_MCS4;
	else if (!strcmp(str, "HE4MCS5")) return HRATE_HE_NSS4_MCS5;
	else if (!strcmp(str, "HE4MCS6")) return HRATE_HE_NSS4_MCS6;
	else if (!strcmp(str, "HE4MCS7")) return HRATE_HE_NSS4_MCS7;
	else if (!strcmp(str, "HE4MCS8")) return HRATE_HE_NSS4_MCS8;
	else if (!strcmp(str, "HE4MCS9")) return HRATE_HE_NSS4_MCS9;
	else if (!strcmp(str, "HE4MCS10")) return HRATE_HE_NSS4_MCS10;
	else if (!strcmp(str, "HE4MCS11")) return HRATE_HE_NSS4_MCS11;
	return 0;
}

#define MP_RATE_STR_NUM 132
#define MP_RATE_STR_LEN 12
const char rtw_mp_rateidx_arrdef[MP_RATE_STR_NUM][MP_RATE_STR_LEN] = {
	"1M", "2M", "5.5M", "11M", "6M", "9M", "12M", "18M", "24M", "36M", "48M", "54M",
	"HTMCS0", "HTMCS1", "HTMCS2", "HTMCS3", "HTMCS4", "HTMCS5", "HTMCS6", "HTMCS7",
	"HTMCS8", "HTMCS9", "HTMCS10", "HTMCS11", "HTMCS12", "HTMCS13", "HTMCS14", "HTMCS15",
	"HTMCS16", "HTMCS17", "HTMCS18", "HTMCS19", "HTMCS20", "HTMCS21", "HTMCS22", "HTMCS23",
	"HTMCS24", "HTMCS25", "HTMCS26", "HTMCS27", "HTMCS28", "HTMCS29", "HTMCS30", "HTMCS31",
	"VHT1MCS0", "VHT1MCS1", "VHT1MCS2", "VHT1MCS3", "VHT1MCS4",
	"VHT1MCS5", "VHT1MCS6", "VHT1MCS7", "VHT1MCS8", "VHT1MCS9",
	"VHT2MCS0", "VHT2MCS1", "VHT2MCS2", "VHT2MCS3", "VHT2MCS4",
	"VHT2MCS5", "VHT2MCS6", "VHT2MCS7", "VHT2MCS8", "VHT2MCS9",
	"VHT3MCS0", "VHT3MCS1", "VHT3MCS2", "VHT3MCS3", "VHT3MCS4",
	"VHT3MCS5", "VHT3MCS6", "VHT3MCS7", "VHT3MCS8", "VHT3MCS9",
	"VHT4MCS0", "VHT4MCS1", "VHT4MCS2", "VHT4MCS3", "VHT4MCS4",
	"VHT4MCS5", "VHT4MCS6", "VHT4MCS7", "VHT4MCS8", "VHT4MCS9",
	"HE1MCS0", "HE1MCS1", "HE1MCS2", "HE1MCS3", "HE1MCS4", "HE1MCS5",
	"HE1MCS6", "HE1MCS7", "HE1MCS8", "HE1MCS9", "HE1MCS10", "HE1MCS11",
	"HE2MCS0", "HE2MCS1", "HE2MCS2", "HE2MCS3", "HE2MCS4", "HE2MCS5",
	"HE2MCS6", "HE2MCS7", "HE2MCS8", "HE2MCS9", "HE2MCS10", "HE2MCS11",
	"HE3MCS0", "HE3MCS1", "HE3MCS2", "HE3MCS3", "HE3MCS4", "HE3MCS5",
	"HE3MCS6", "HE3MCS7", "HE3MCS8", "HE3MCS9", "HE3MCS10", "HE3MCS11",
	"HE4MCS0", "HE4MCS1", "HE4MCS2", "HE4MCS3", "HE4MCS4", "HE4MCS5",
	"HE4MCS6", "HE4MCS7", "HE4MCS8", "HE4MCS9", "HE4MCS10", "HE4MCS11"};

u16 rtw_mp_rate_parse(_adapter *adapter, u8 *target_str)
{
	struct mp_priv	*pmppriv = &adapter->mppriv;

	u16 i = 0, mcs_idx_base = 0;
	u16 hrateidx = 0;
	u8 plcp_nss = 0, ppdu_type = 0;
	u8 user_idx = pmppriv->mp_plcp_useridx;

#if 1/*def CONFIG_80211AX_HE*/
	hrateidx = srate2herate(target_str);

	if (MP_IS_1T_HRATE(hrateidx))
		plcp_nss = MP_NSS1;
	else if (MP_IS_2T_HRATE(hrateidx))
		plcp_nss = MP_NSS2;
	else if (MP_IS_3T_HRATE(hrateidx))
		plcp_nss = MP_NSS3;
	else if (MP_IS_4T_HRATE(hrateidx))
		plcp_nss = MP_NSS4;
	RTW_INFO("%s , PLCP NSS idx = %d\n", __func__ , plcp_nss);

	if (MP_IS_CCK_HRATE(hrateidx))
		ppdu_type = RTW_MP_TYPE_CCK;
	else if (MP_IS_OFDM_HRATE(hrateidx)) {
		ppdu_type = RTW_MP_TYPE_LEGACY;
		mcs_idx_base = 4;
	} else if (MP_IS_HT_HRATE(hrateidx)) {
		ppdu_type = RTW_MP_TYPE_HT_MF;
		mcs_idx_base = 12;
	} else if (MP_IS_VHT_HRATE(hrateidx)) {
		if (plcp_nss == MP_NSS1)
			mcs_idx_base = 44;
		else if (plcp_nss == MP_NSS2)
			mcs_idx_base = 44 + 10;
		else if (plcp_nss == MP_NSS3)
			mcs_idx_base = 44 + 20;
		else if (plcp_nss == MP_NSS4)
			mcs_idx_base = 44 + 30;

		ppdu_type = RTW_MP_TYPE_VHT;
	} else if (MP_IS_HE_HRATE(hrateidx)) {
		if (plcp_nss == MP_NSS1)
			mcs_idx_base = 84;
		else if (plcp_nss == MP_NSS2)
			mcs_idx_base = 84 + 12;
		else if (plcp_nss == MP_NSS3)
			mcs_idx_base = 84 + 24;
		else if (plcp_nss == MP_NSS4)
			mcs_idx_base = 84+ 36;

		ppdu_type = RTW_MP_TYPE_HE_SU;
	}

	pmppriv->mp_plcp_user[user_idx].plcp_nss = plcp_nss;
	pmppriv->rtw_mp_pmact_ppdu_type = ppdu_type;
	rtw_update_giltf(adapter);
	rtw_mp_update_coding(adapter);

		for (i = 0; i <= MP_RATE_STR_NUM; i++) {
		if (strcmp(target_str, rtw_mp_rateidx_arrdef[i+ mcs_idx_base]) == 0) {
				pmppriv->mp_plcp_user[user_idx].plcp_mcs = i;
				RTW_INFO("%s , PLCP MCS = %d\n", __func__ , pmppriv->mp_plcp_user[user_idx].plcp_mcs);
					break;
				}
		}

	if (1) {
		printk("%s ,please input a Data RATE String as:", __func__);
		for (i = 0; i <= MP_RATE_STR_NUM; i++) {
			printk("%s ", rtw_mp_rateidx_arrdef[i]);
			if (i % 10 == 0)
				printk("\n");
			}
	}
	return hrateidx;
#else
	for (i = 0; i <= MP_RATE_STR_NUM; i++) {
		if (strcmp(targetStr, rtw_mp_rateidx_arrdef[i]) == 0) {
			RTW_INFO("%s , index = %d\n", __func__ , i);
			return i;
		}
	}

	printk("%s ,please input a Data RATE String as:", __func__);
	for (i = 0; i <= MP_RATE_STR_NUM; i++) {
		printk("%s ", rtw_mp_rateidx_arrdef[i]);
		if (i % 10 == 0)
			printk("\n");
	}
	return _FAIL;
#endif
}

u8 rtw_mp_mode_check(_adapter *adapter)
{
	_adapter *primary_adapter = GET_PRIMARY_ADAPTER(adapter);

	if (primary_adapter->registrypriv.mp_mode == 1 || primary_adapter->mppriv.bprocess_mp_mode == _TRUE)
		return _TRUE;
	else
		return _FALSE;
}

bool rtw_is_mp_tssitrk_on(_adapter *adapter)
{
	_adapter *primary_adapter = GET_PRIMARY_ADAPTER(adapter);

	return primary_adapter->mppriv.tssitrk_on;
}

u32 mpt_ProQueryCalTxPower(
	_adapter *adapter,
	u8		RfPath
)
{
	PMPT_CONTEXT		pMptCtx = &(adapter->mppriv.mpt_ctx);
#if 0
	u32			TxPower = 1;
	struct txpwr_idx_comp tic;
	u8 mgn_rate = mpt_to_mgnt_rate(pMptCtx->mpt_rate_index);
	RATE_SECTION rs = mgn_rate_to_rs(mgn_rate);

	TxPower = rtw_hal_get_tx_power_index(adapter, RfPath, rs, mgn_rate
		, pHalData->current_channel_bw, pHalData->current_band_type, pHalData->current_channel, &tic);

	dump_tx_power_index_inline(RTW_DBGDUMP, RfPath
		, pHalData->current_channel_bw, pHalData->current_channel
		, mgn_rate, TxPower, &tic);

	adapter->mppriv.txpoweridx = (u8)TxPower;
	if (RfPath == RF_PATH_A)
		pMptCtx->TxPwrLevel[RF_PATH_A] = (u8)TxPower;
	else if (RfPath == RF_PATH_B)
		pMptCtx->TxPwrLevel[RF_PATH_B] = (u8)TxPower;
	else if (RfPath == RF_PATH_C)
		pMptCtx->TxPwrLevel[RF_PATH_C] = (u8)TxPower;
	else if (RfPath == RF_PATH_D)
		pMptCtx->TxPwrLevel[RF_PATH_D] = (u8)TxPower;
	hal_mpt_SetTxPower(adapter);
		return TxPower;
#endif
	return 0;
}

u32 mpt_get_tx_power_finalabs_val(_adapter *padapter, u8 rf_path)
{
	PMPT_CONTEXT		pMptCtx = &(padapter->mppriv.mpt_ctx);

	u8 mgn_rate = mpt_to_mgnt_rate(pMptCtx->mpt_rate_index);
	u32 powerdbm = 0;

#ifdef CONFIG_80211AX_HE
	struct mp_priv	*pmppriv = &padapter->mppriv;
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	u16 i = 0;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.mp_class = RTW_MP_CLASS_TXPWR;
	ptxpwr_arg.cmd = RTW_MP_TXPWR_CMD_READ_PWR_TABLE;

	ptxpwr_arg.channel = pmppriv->channel;
	ptxpwr_arg.bandwidth = pmppriv->bandwidth;
	ptxpwr_arg.rate = pmppriv->rateidx;
	ptxpwr_arg.rfpath = pmppriv->antenna_tx;
	ptxpwr_arg.beamforming = 0;
	ptxpwr_arg.dcm = 0;
	ptxpwr_arg.offset = 0;

	rtw_mp_set_phl_cmd(padapter, (void*)&ptxpwr_arg, sizeof(struct rtw_mp_txpwr_arg));

	while (i != 100) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(padapter, (void*)&ptxpwr_arg, sizeof(struct rtw_mp_txpwr_arg));
		if (ptxpwr_arg.cmd_ok && ptxpwr_arg.status == RTW_PHL_STATUS_SUCCESS) {
			powerdbm = ptxpwr_arg.table_item;
			RTW_INFO("%s,SET CMD OK\n", __func__);
			break;
		} else {
			if (i > 100) {
				RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__, ptxpwr_arg.status);
					break;
				}
			i++;
		}
	}
#else
/*
	powerdbm = phy_get_tx_power_final_absolute_value(padapter, rf_path, mgn_rate, pHalData->current_channel_bw, pHalData->current_channel);
	RTW_INFO("bw=%d, ch=%d, rateid=%d, TSSI Power(dBm):%d\n",
		pHalData->current_channel_bw, pHalData->current_channel, mgn_rate ,powerdbm);
*/
#endif
	return powerdbm;
}

u8 rtw_mpt_set_power_limit_en(_adapter *padapter, bool en_val)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	u16 i = 0;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.mp_class = RTW_MP_CLASS_TXPWR;
	ptxpwr_arg.cmd = RTW_MP_TXPWR_CMD_SET_PWR_LMT_EN;

	ptxpwr_arg.pwr_lmt_en = en_val;

	rtw_mp_set_phl_cmd(padapter, (void*)&ptxpwr_arg, sizeof(struct rtw_mp_txpwr_arg));

	while (i != 10) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(padapter, (void*)&ptxpwr_arg, sizeof(struct rtw_mp_txpwr_arg));
		if (ptxpwr_arg.cmd_ok && ptxpwr_arg.status == RTW_PHL_STATUS_SUCCESS) {
			RTW_INFO("%s,SET CMD OK\n", __func__);
			return ptxpwr_arg.cmd_ok;
		} else {
			if (i > 10) {
				RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__, ptxpwr_arg.status);
				break;
			}
			i++;
		}
	}

	return 0;
}

bool rtw_mpt_get_power_limit_en(_adapter *padapter)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	u16 i = 0;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.mp_class = RTW_MP_CLASS_TXPWR;
	ptxpwr_arg.cmd = RTW_MP_TXPWR_CMD_GET_PWR_LMT_EN;
	ptxpwr_arg.pwr_lmt_en = _FALSE;

	rtw_mp_set_phl_cmd(padapter, (void*)&ptxpwr_arg, sizeof(struct rtw_mp_txpwr_arg));

	while (i != 10) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(padapter, (void*)&ptxpwr_arg, sizeof(struct rtw_mp_txpwr_arg));
		if (ptxpwr_arg.cmd_ok && ptxpwr_arg.status == RTW_PHL_STATUS_SUCCESS) {
			RTW_INFO("%s, CMD OK\n", __func__);
			return ptxpwr_arg.pwr_lmt_en;
		} else {
			if (i > 10) {
				RTW_INFO("%s,GET CMD FAIL !!! status %d\n",
					__func__, ptxpwr_arg.status);
				break;
			}
			i++;
		}
	}

	return ptxpwr_arg.pwr_lmt_en;
}

void rtw_mp_get_phl_cmd(_adapter *padapter, void* buf, u32 buflen)
{
	struct dvobj_priv	*dvobj = adapter_to_dvobj(padapter);
	struct rtw_mp_test_cmdbuf *cmdbuf = NULL;

	if (buflen > RTW_MAX_TEST_CMD_BUF) {
		RTW_ERR("%s, buflen %d over CMD BUF Size !\n", __func__, buflen);
		return;
	}

	cmdbuf = _rtw_malloc(sizeof(struct rtw_mp_test_cmdbuf));

	if (cmdbuf) {
		_rtw_memset((void *)cmdbuf, 0, sizeof(struct rtw_mp_test_cmdbuf));
		cmdbuf->type = 0;
		cmdbuf->len = buflen;
		_rtw_memcpy((void *)cmdbuf->buf, buf, buflen);
		rtw_phl_test_submodule_get_rpt(dvobj->phl_com, (void *)cmdbuf, sizeof(struct rtw_mp_test_cmdbuf));
		_rtw_memcpy((void *)buf, (void *)cmdbuf->buf, buflen);
	}
	if (cmdbuf)
		_rtw_mfree(cmdbuf, sizeof(struct rtw_mp_test_cmdbuf));
}

void rtw_mp_set_phl_cmd(_adapter *padapter, void* buf, u32 buflen)
{
	struct dvobj_priv	*dvobj = adapter_to_dvobj(padapter);
	struct rtw_mp_test_cmdbuf *cmdbuf = NULL;

	if (buflen > RTW_MAX_TEST_CMD_BUF) {
		RTW_ERR("%s, buflen %d over CMD BUF Size !\n", __func__, buflen);
		return;
	}

	cmdbuf = _rtw_malloc(sizeof(struct rtw_mp_test_cmdbuf));

	if (cmdbuf) {
		_rtw_memset((void *)cmdbuf, 0, sizeof(struct rtw_mp_test_cmdbuf));

		cmdbuf->type = 0;
		cmdbuf->len = buflen;
		_rtw_memcpy((void *)cmdbuf->buf, buf, buflen);
		rtw_phl_test_submodule_cmd_process(dvobj->phl_com, (void*)cmdbuf, sizeof(struct rtw_mp_test_cmdbuf));
	}
	if (cmdbuf)
		_rtw_mfree(cmdbuf, sizeof(struct rtw_mp_test_cmdbuf));
}

u8 rtw_mp_phl_txpower(_adapter *padapter, struct rtw_mp_txpwr_arg	*ptxpwr_arg, u8 cmdid)
{
	struct mp_priv	*pmppriv = &padapter->mppriv;
	u16 i = 0;

	ptxpwr_arg->mp_class = RTW_MP_CLASS_TXPWR;
	ptxpwr_arg->cmd = cmdid;

	rtw_mp_set_phl_cmd(padapter, (void*)ptxpwr_arg,  sizeof(struct rtw_mp_txpwr_arg));

	while (i <= 10) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(padapter, (void*)ptxpwr_arg,  sizeof(struct rtw_mp_txpwr_arg));
		if (ptxpwr_arg->cmd_ok && ptxpwr_arg->status == RTW_PHL_STATUS_SUCCESS) {
			RTW_INFO("%s,SET CMD OK\n", __func__);
			break;
		} else {
			if (i > 10) {
				RTW_DBG("%s,GET CMD FAIL !!! status %d\n", __func__, ptxpwr_arg->status);
				break;
			}
			i++;
		}
	}

	return ptxpwr_arg->cmd_ok;
}

bool rtw_mp_phl_config_arg(_adapter *padapter, enum rtw_mp_config_cmdid cmdid)
{
	struct mp_priv	*pmppriv = &padapter->mppriv;
	struct rtw_mp_config_arg pmp_arg;
	u16 i = 0;

	_rtw_memset((void *)&pmp_arg, 0, sizeof(struct rtw_mp_config_arg));

	pmp_arg.mp_class = RTW_MP_CLASS_CONFIG;
	pmp_arg.cmd = cmdid;
	RTW_INFO("%s, id: %d !!!\n", __func__, cmdid);

	switch (cmdid) {
	case RTW_MP_CONFIG_CMD_SET_CH_BW:
		if (pmppriv->channel > 6000) {
			pmppriv->channel = pmppriv->channel - 6000;
			pmp_arg.channel = pmppriv->channel;
			pmp_arg.band = 2;
		} else {
			pmp_arg.channel = pmppriv->channel;

			if (pmppriv->channel >= 36 && pmppriv->band < 2 )
				pmp_arg.band = 1;
			else if (pmppriv->channel <= 14 && pmppriv->band < 2)
				pmp_arg.band = 0;
			else
				pmp_arg.band = pmppriv->band;

		}
		pmp_arg.bandwidth = pmppriv->bandwidth;
		pmp_arg.sc_idx = pmppriv->prime_channel_offset;
		break;
	case RTW_MP_CONFIG_CMD_SET_RATE_IDX:
		pmp_arg.rate_idx= pmppriv->rateidx;
		break;
	case RTW_MP_CONFIG_CMD_SET_RF_TXRX_PATH:
		pmp_arg.ant_tx = pmppriv->antenna_trx;
		pmp_arg.ant_rx = pmppriv->antenna_trx;
		break;
	case RTW_MP_CONFIG_CMD_STOP_DUT:
	case RTW_MP_CONFIG_CMD_START_DUT:
	case RTW_MP_CONFIG_CMD_SET_PHY_INDEX:
		pmp_arg.cur_phy = pmppriv->rtw_mp_cur_phy;
		break;
	case RTW_MP_CONFIG_CMD_SET_RESET_PHY_COUNT:
	case RTW_MP_CONFIG_CMD_SET_RESET_MAC_COUNT:
	case RTW_MP_CONFIG_CMD_SET_RESET_DRV_COUNT:
		pmp_arg.cur_phy = pmppriv->rtw_mp_cur_phy;
		break;
	case RTW_MP_CONFIG_CMD_SWITCH_BT_PATH:
		pmp_arg.btc_mode = pmppriv->btc_path;
		break;
	default:
		RTW_INFO("%s, No CMD case match !!!\n", __func__);
		return _FALSE;
	}

	rtw_mp_set_phl_cmd(padapter, (void*)&pmp_arg,  sizeof(struct rtw_mp_config_arg));
	while (1) {
		rtw_mp_get_phl_cmd(padapter, (void*)&pmp_arg,  sizeof(struct rtw_mp_config_arg));
		if (pmp_arg.cmd_ok && pmp_arg.status == RTW_PHL_STATUS_SUCCESS) {
			RTW_INFO("%s, %d GET CMD OK !!!\n", __func__, cmdid);
			break;
		} else {
			i++;
			rtw_msleep_os(10);
			if (i == 100) {
				RTW_INFO("%s, %d GET CMD FAIL !!!\n", __func__, cmdid);
				break;
			}
		}
	}
	return pmp_arg.cmd_ok;
}

void rtw_mp_phl_rx_physts(_adapter *padapter, struct rtw_mp_rx_arg *rx_arg, bool bstart)
{
	struct mp_priv *pmppriv = &padapter->mppriv;
	u8 i = 0;

	rx_arg->mp_class = RTW_MP_CLASS_RX;
	rx_arg->cmd = RTW_MP_RX_CMD_GET_PHYSTS;
	rx_arg->enable = bstart;

	rx_arg->cmd_ok = _FALSE;
	rtw_mp_set_phl_cmd(padapter, (void*)rx_arg,  sizeof(struct rtw_mp_rx_arg));

	while (i != 10) {
			rtw_msleep_os(100);
			rtw_mp_get_phl_cmd(padapter, (void*)rx_arg,  sizeof(struct rtw_mp_rx_arg));
			if (rx_arg->cmd_ok && rx_arg->status == RTW_PHL_STATUS_SUCCESS) {
				RTW_INFO("%s,GET CMD RX OK\n", __func__);
				break;
			} else {
				if (i > 10) {
					RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__,rx_arg->status);
					break;
				}
				i++;
			}
	}
}

void rtw_mp_phl_rx_rssi(_adapter *padapter, struct rtw_mp_rx_arg *rx_arg)
{
	struct mp_priv *pmppriv = &padapter->mppriv;
	u8 i = 0;

	rx_arg->mp_class = RTW_MP_CLASS_RX;
	rx_arg->cmd = RTW_MP_RX_CMD_GET_RSSI;

	rx_arg->cmd_ok = _FALSE;
	rtw_mp_set_phl_cmd(padapter, (void*)rx_arg,  sizeof(struct rtw_mp_rx_arg));

	while (i != 10) {
			rtw_msleep_os(100);
			rtw_mp_get_phl_cmd(padapter, (void*)rx_arg,  sizeof(struct rtw_mp_rx_arg));
			if (rx_arg->cmd_ok && rx_arg->status == RTW_PHL_STATUS_SUCCESS) {
				RTW_INFO("%s,Path%d rssi:%d GET CMD RX OK\n", __func__, rx_arg->rf_path, rx_arg->rssi);
				break;
			} else {
				if (i > 10) {
					RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__,rx_arg->status);
					break;
				}
				i++;
			}
	}
}

void rtw_mp_phl_rx_gain_offset(_adapter *padapter, struct rtw_mp_rx_arg *rx_arg, u8 path_num)
{
	struct mp_priv *pmppriv = &padapter->mppriv;
	u8 all_path_num = path_num;
	u8 rf_path = 0;

	rx_arg->mp_class = RTW_MP_CLASS_RX;
	rx_arg->cmd = RTW_MP_RX_CMD_SET_GAIN_OFFSET;

	if (path_num > 1) {
		path_num = 0;
		all_path_num -= 1;
	}

	for (rf_path = path_num; rf_path <= all_path_num; rf_path++) {
		u16 i = 0;

		rx_arg->rf_path = rf_path;
		rx_arg->cmd_ok = _FALSE;
		rtw_mp_set_phl_cmd(padapter, (void*)rx_arg,  sizeof(struct rtw_mp_rx_arg));

		while (i != 10) {
				rtw_msleep_os(100);
				rtw_mp_get_phl_cmd(padapter, (void*)rx_arg,  sizeof(struct rtw_mp_rx_arg));
				if (rx_arg->cmd_ok && rx_arg->status == RTW_PHL_STATUS_SUCCESS) {
					RTW_INFO("%s,rf path %d GET CMD RX OK\n", __func__, rx_arg->rf_path);
					break;
				} else {
					if (i > 10) {
						RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__,rx_arg->status);
						break;
					}
					i++;
				}
		}
	}
}


void rtw_mp_phl_query_rx(_adapter *padapter, struct rtw_mp_rx_arg *rx_arg ,u8 rx_qurey_type)
{
	struct mp_priv *pmppriv = &padapter->mppriv;
	u8 rxcmd[2] = {0, 0};
	u8 cmd_idx = 0;

	rx_arg->mp_class = RTW_MP_CLASS_RX;
	rx_arg->rx_ok = 0;
	rx_arg->rx_err = 0;

	if (rx_qurey_type == 0) {
		rxcmd[0] = RTW_MP_RX_CMD_PHY_CRC_OK;
		rxcmd[1] = RTW_MP_RX_CMD_PHY_CRC_ERR;
	} else if (rx_qurey_type == 1) {
		rxcmd[0] = RTW_MP_RX_CMD_MAC_CRC_OK;
		rxcmd[1] = RTW_MP_RX_CMD_MAC_CRC_ERR;
	} else if (rx_qurey_type == 2) {
		rxcmd[0] = RTW_MP_RX_CMD_DRV_CRC_OK;
		rxcmd[1] = RTW_MP_RX_CMD_DRV_CRC_ERR;
	}

	for (cmd_idx = 0; cmd_idx < 2 ; cmd_idx++) {
		u16 i = 0;

		rx_arg->cmd = rxcmd[cmd_idx];
		rx_arg->cmd_ok = _FALSE;
		rtw_mp_set_phl_cmd(padapter, (void*)rx_arg,  sizeof(struct rtw_mp_rx_arg));

		while (i != 10) {
				rtw_msleep_os(100);
				rtw_mp_get_phl_cmd(padapter, (void*)rx_arg,  sizeof(struct rtw_mp_rx_arg));
				if (rx_arg->cmd_ok && rx_arg->status == RTW_PHL_STATUS_SUCCESS) {
					RTW_INFO("%s,GET CMD RX OK:%d ,RX ERR:%d\n", __func__, rx_arg->rx_ok, rx_arg->rx_err);
					break;
				} else {
					if (i > 10) {
						RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__,rx_arg->status);
						break;
					}
					i++;
				}
		}
	}
}

void rtw_mp_set_crystal_cap(_adapter *padapter, u32 xcapvalue)
{
	struct rtw_mp_reg_arg cfg_arg;
	u8 sc_xo_idx = 1;
	u8 i = 0;

	cfg_arg.mp_class = RTW_MP_CLASS_REG;
	cfg_arg.cmd = RTW_MP_REG_CMD_SET_XCAP;

	for (i <=0 ; i <= sc_xo_idx; i++) {
		cfg_arg.cmd_ok = 0;
		cfg_arg.sc_xo = i;
		cfg_arg.io_value = xcapvalue;
		rtw_mp_set_phl_cmd(padapter, (void*)&cfg_arg,  sizeof(struct rtw_mp_reg_arg));

		while (i != 10) {
				rtw_msleep_os(10);
				rtw_mp_get_phl_cmd(padapter, (void*)&cfg_arg,  sizeof(struct rtw_mp_reg_arg));
				if (cfg_arg.cmd_ok && cfg_arg.status == RTW_PHL_STATUS_SUCCESS) {
					RTW_INFO("%s,SET CMD TX OK\n", __func__);
					break;
				} else {
					if (i > 10) {
						RTW_INFO("%s,GET CMD FAIL !!! status %d\n", __func__, cfg_arg.status);
						break;
				}
				i++;
			}
		}
	}

}

u8 rtw_mp_phl_calibration(_adapter *padapter, struct rtw_mp_cal_arg	*pcal_arg, u8 cmdid)
{
	struct mp_priv	*pmppriv = &padapter->mppriv;
	u16 i = 0;
	u32 cmd_size = sizeof(struct rtw_mp_cal_arg);

	pcal_arg->mp_class = RTW_MP_CLASS_CAL;
	pcal_arg->cmd = cmdid;

	rtw_mp_set_phl_cmd(padapter, (void*)pcal_arg, cmd_size);

	while (i <= 10) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(padapter, (void*)pcal_arg, cmd_size);
		if (pcal_arg->cmd_ok && pcal_arg->status == RTW_PHL_STATUS_SUCCESS) {
			RTW_INFO("%s,SET CMD OK\n", __func__);
			break;
		} else {
			if (i > 10) {
				RTW_DBG("%s,GET CMD FAIL !!! status %d\n", __func__, pcal_arg->status);
				break;
			}
			i++;
		}
	}

	return pcal_arg->cmd_ok;
}

u8 rtw_mp_phl_reg(_adapter *padapter, struct rtw_mp_reg_arg	*reg_arg, u8 cmdid)
{
	struct mp_priv	*pmppriv = &padapter->mppriv;
	u16 i = 0;
	u32 cmd_size = sizeof(struct rtw_mp_reg_arg);

	reg_arg->mp_class = RTW_MP_CLASS_REG;
	reg_arg->cmd = cmdid;

	rtw_mp_set_phl_cmd(padapter, (void*)reg_arg, cmd_size);

	while (i <= 10) {
		rtw_msleep_os(10);
		rtw_mp_get_phl_cmd(padapter, (void*)reg_arg, cmd_size);
		if (reg_arg->cmd_ok && reg_arg->status == RTW_PHL_STATUS_SUCCESS) {
			RTW_INFO("%s,SET CMD OK\n", __func__);
			break;
		} else {
			if (i > 10) {
				RTW_DBG("%s,GET CMD FAIL !!! status %d\n", __func__, reg_arg->status);
				break;
			}
			i++;
		}
	}

	return reg_arg->cmd_ok;
}


u8 rtw_update_giltf(_adapter *padapter)
{
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 idx = 0;
	u8 ppdu_type = pmp_priv->rtw_mp_pmact_ppdu_type;
	u8 user_idx = pmp_priv->mp_plcp_useridx;

	RTW_INFO("%s: ppdu_type=%s\n", __func__ , PPDU_TYPE_STR(pmp_priv->rtw_mp_pmact_ppdu_type));
	if (ppdu_type == RTW_MP_TYPE_HT_MF || ppdu_type == RTW_MP_TYPE_HT_GF
										|| ppdu_type == RTW_MP_TYPE_VHT) {
		pmp_priv->st_giltf[0].gi = 0;
		pmp_priv->st_giltf[0].ltf = 0;
		strcpy(pmp_priv->st_giltf[0].type_str, "0.4");
		pmp_priv->st_giltf[1].gi = 1;
		pmp_priv->st_giltf[1].ltf = 0;
		strcpy(pmp_priv->st_giltf[1].type_str, "0.8");
		idx = 1;
	} else if (ppdu_type > RTW_MP_TYPE_VHT) {
		if (ppdu_type == RTW_MP_TYPE_HE_SU || ppdu_type == RTW_MP_TYPE_HE_ER_SU) {

			if (!pmp_priv->mp_plcp_user[user_idx].dcm && !pmp_priv->rtw_mp_stbc) {
				pmp_priv->st_giltf[0].gi = 1;
				pmp_priv->st_giltf[0].ltf = 0;
				strcpy(pmp_priv->st_giltf[0].type_str, "1x 0.8");
				pmp_priv->st_giltf[1].gi = 1;
				pmp_priv->st_giltf[1].ltf = 1;
				strcpy(pmp_priv->st_giltf[1].type_str, "2x 0.8");
				pmp_priv->st_giltf[2].gi = 2;
				pmp_priv->st_giltf[2].ltf = 1;
				strcpy(pmp_priv->st_giltf[2].type_str, "2x 1.6");
				pmp_priv->st_giltf[3].gi = 3;
				pmp_priv->st_giltf[3].ltf = 2;
				strcpy(pmp_priv->st_giltf[3].type_str, "4x 3.2");
				pmp_priv->st_giltf[4].gi = 1;
				pmp_priv->st_giltf[4].ltf = 2;
				strcpy(pmp_priv->st_giltf[4].type_str, "4x 0.8");
				idx = 4;
			} else {
				pmp_priv->st_giltf[0].gi = 1;
				pmp_priv->st_giltf[0].ltf = 0;
				strcpy(pmp_priv->st_giltf[0].type_str, "1x 0.8");
				pmp_priv->st_giltf[1].gi = 1;
				pmp_priv->st_giltf[1].ltf = 1;
				strcpy(pmp_priv->st_giltf[1].type_str, "2x 0.8");
				pmp_priv->st_giltf[2].gi = 2;
				pmp_priv->st_giltf[2].ltf = 1;
				strcpy(pmp_priv->st_giltf[2].type_str, "2x 1.6");
				pmp_priv->st_giltf[3].gi = 3;
				pmp_priv->st_giltf[3].ltf = 2;
				strcpy(pmp_priv->st_giltf[3].type_str, "4x 3.2");
				idx = 3;
			}
		}else if (ppdu_type == RTW_MP_TYPE_HE_MU_OFDMA) {
			pmp_priv->st_giltf[0].gi = 1;
			pmp_priv->st_giltf[0].ltf = 1;
			strcpy(pmp_priv->st_giltf[0].type_str, "2x 0.8");
			pmp_priv->st_giltf[1].gi = 2;
			pmp_priv->st_giltf[1].ltf = 1;
			strcpy(pmp_priv->st_giltf[1].type_str, "2x 1.6");
			pmp_priv->st_giltf[2].gi = 1;
			pmp_priv->st_giltf[2].ltf = 2;
			strcpy(pmp_priv->st_giltf[2].type_str, "4x 0.8");
			pmp_priv->st_giltf[3].gi = 3;
			pmp_priv->st_giltf[3].ltf = 2;
			strcpy(pmp_priv->st_giltf[3].type_str, "4x 3.2");
			idx = 3;
		} else if (ppdu_type == RTW_MP_TYPE_HE_TB) {
				pmp_priv->st_giltf[0].gi = 2;
				pmp_priv->st_giltf[0].ltf = 0;
				strcpy(pmp_priv->st_giltf[0].type_str, "1x 1.6");
				pmp_priv->st_giltf[1].gi = 2;
				pmp_priv->st_giltf[1].ltf = 1;
				strcpy(pmp_priv->st_giltf[1].type_str, "2x 1.6");
				pmp_priv->st_giltf[2].gi = 3;
				pmp_priv->st_giltf[2].ltf = 2;
				strcpy(pmp_priv->st_giltf[2].type_str, "4x 3.2");
				idx = 2;
			}
		}
	return idx;
}

void rtw_mp_update_coding(_adapter *padapter)
{
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 ppdu_type = pmp_priv->rtw_mp_pmact_ppdu_type;
	u8 user_idx = pmp_priv->mp_plcp_useridx;

	if (ppdu_type == RTW_MP_TYPE_HE_SU || pmp_priv->bandwidth >= CHANNEL_WIDTH_40 ||
		pmp_priv->rtw_mp_ru_tone >= MP_RU_TONE_484) {

		RTW_INFO("%s, PPDU HE SU , over 40M, RU Tone over 484\n", __func__);
		pmp_priv->mp_plcp_user[user_idx].coding = 1;/* 1 LDPC, 0 BCC */
	} else if (ppdu_type == RTW_MP_TYPE_HE_MU_OFDMA) {

		RTW_INFO("%s, PPDU HE MU\n", __func__);
		pmp_priv->mp_plcp_user[user_idx].coding = 0;/* 1 LDPC, 0 BCC */
	}

	RTW_INFO("%s, coding: %s\n", __func__, (pmp_priv->mp_plcp_user[user_idx].coding?"LDPC":"BCC"));
}

u8 rtw_mp_update_ru_tone(_adapter *padapter)
{
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 user_idx = pmp_priv->mp_plcp_useridx;
	u8 cur_coding = pmp_priv->mp_plcp_user[user_idx].coding;
	u8 ruidx = MP_RU_TONE_26;
	u8 i = 0 , j = 0;

	if (cur_coding == 0) {/* 1 LDPC, 0 BCC */
		ruidx = MP_RU_TONE_242;
		if (pmp_priv->rtw_mp_ru_tone < MP_RU_TONE_242)
			pmp_priv->rtw_mp_ru_tone = MP_RU_TONE_242;
	} else {
		ruidx = MP_RU_TONE_26;
	}

	for (i = 0; i <= 5; i++) {
		pmp_priv->ru_tone_sel_list[i] = ruidx++;
		if (ruidx > MP_RU_TONE_966)
			break;
	}
	return i;
}

u8 rtw_mp_update_ru_alloc(_adapter *padapter)
{
	struct mp_priv *pmp_priv = (struct mp_priv *)&padapter->mppriv;
	u8 cur_tone = pmp_priv->rtw_mp_ru_tone;
	u8 i = 0;
	u8 alloc_start = 0, alloc_end = 0;
	u8 user_idx = pmp_priv->mp_plcp_useridx;

	switch (cur_tone) {
	case MP_RU_TONE_26:
					alloc_start = 0;
					alloc_end = 9;
					if (pmp_priv->bandwidth == CHANNEL_WIDTH_40)
						alloc_end = 18;

					if (pmp_priv->bandwidth == CHANNEL_WIDTH_80)
						alloc_end = 37;
					break;
	case MP_RU_TONE_52:
					alloc_start = 37;
					alloc_end = 41;
					if (pmp_priv->bandwidth == CHANNEL_WIDTH_40)
						alloc_end = 45;

					if (pmp_priv->bandwidth == CHANNEL_WIDTH_80)
						alloc_end = 53;

					break;
	case MP_RU_TONE_106:
					alloc_start = 53;
					alloc_end = 55;

					if (pmp_priv->bandwidth == CHANNEL_WIDTH_40)
						alloc_end = 57;

					if (pmp_priv->bandwidth == CHANNEL_WIDTH_80)
						alloc_end = 61;

					break;
	case MP_RU_TONE_242:
					alloc_start = 61;
					alloc_end = 61;
					if (pmp_priv->bandwidth == CHANNEL_WIDTH_40)
						alloc_end = 62;

					if (pmp_priv->bandwidth == CHANNEL_WIDTH_80)
						alloc_end = 64;

					break;
	case MP_RU_TONE_484:
					alloc_start = 65;
					alloc_end = 66;
					break;
	case MP_RU_TONE_966:
					alloc_start = 67;
					alloc_end = 67;
					break;
	};

	if (pmp_priv->mp_plcp_user[user_idx].ru_alloc < alloc_start ||
			pmp_priv->mp_plcp_user[user_idx].ru_alloc > alloc_end)
		pmp_priv->mp_plcp_user[user_idx].ru_alloc = alloc_start;

	for (i = 0; i < 68; i++) {
		if (alloc_start + i > alloc_end)
			break;
		pmp_priv->ru_alloc_list[i] = alloc_start + i;
		RTW_INFO("%s, Ru alloc:%d\n", __func__, pmp_priv->ru_alloc_list[i]);

	}
	return i;
}

u32 rtw_mp_get_tssi_de(_adapter *padapter, u8 rf_path)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	struct mp_priv *pmppriv = &padapter->mppriv;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.rfpath = rf_path;

	if (rtw_mp_phl_txpower(padapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_GET_TSSI))
		return ptxpwr_arg.tssi;
	else
		return 0;
}

s32 rtw_mp_get_online_tssi_de(_adapter *padapter, s32 out_pwr, s32 tgdbm, u8 rf_path)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	struct mp_priv *pmppriv = &padapter->mppriv;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.dbm = tgdbm;
	ptxpwr_arg.pout = out_pwr;
	ptxpwr_arg.rfpath = rf_path;

	if (rtw_mp_phl_txpower(padapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_GET_ONLINE_TSSI_DE))
		return ptxpwr_arg.online_tssi_de;
	else
		return 0;
}

u8 rtw_mp_set_tsside2verify(_adapter *padapter, u32 tssi_de, u8 rf_path)
{
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	struct mp_priv *pmppriv = &padapter->mppriv;
	u8 tx_nss = get_phy_tx_nss(padapter, padapter_link);
	u8 i = 0;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.tssi = tssi_de;
	ptxpwr_arg.rfpath = rf_path;

	if (rtw_mp_phl_txpower(padapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_SET_TSSI))
		return true;
	else
		return false;
}

u8 rtw_mp_set_tssi_offset(_adapter *padapter, u32 tssi_offset, u8 rf_path)
{
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	struct mp_priv *pmppriv = &padapter->mppriv;
	u8 tx_nss = get_phy_tx_nss(padapter, padapter_link);
	u8 i = 0;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.tssi_de_offset = tssi_offset;
	ptxpwr_arg.rfpath = rf_path;

	if (rtw_mp_phl_txpower(padapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_SET_TSSI_OFFSET))
		return true;
	else
		return false;
}

u8 rtw_mp_set_tssi_pwrtrk(_adapter *padapter, u8 tssi_state)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	ptxpwr_arg.txpwr_track_status = tssi_state;

	if (rtw_mp_phl_txpower(padapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_SET_PWR_TRACK_STATUS))
		return true;
	else
		return false;
}

u8 rtw_mp_get_tssi_pwrtrk(_adapter *padapter)
{
	struct rtw_mp_txpwr_arg	ptxpwr_arg;
	u8	tssi_state = RTW_MP_TSSI_OFF;

	_rtw_memset((void *)&ptxpwr_arg, 0, sizeof(struct rtw_mp_txpwr_arg));

	if (rtw_mp_phl_txpower(padapter, &ptxpwr_arg, RTW_MP_TXPWR_CMD_GET_PWR_TRACK_STATUS)) {
		tssi_state = ptxpwr_arg.txpwr_track_status;
		RTW_INFO("%s, get tssi_state = %d\n", __func__, tssi_state);
	} else
		RTW_INFO("%s, rtw_mp_phl_txpower fail\n", __func__);

	return tssi_state;
}

#endif
