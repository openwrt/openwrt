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

#include <drv_types.h>
#include <rtw_sreset.h>

void sreset_init_value(_adapter *padapter)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	HAL_DATA_TYPE	*pHalData = GET_PHL_COM(dvobj);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;

	_rtw_mutex_init(&psrtpriv->silentreset_mutex);
	psrtpriv->silent_reset_inprogress = _FALSE;
	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;
	psrtpriv->last_tx_time = 0;
	psrtpriv->last_tx_complete_time = 0;
#endif
}
void sreset_reset_value(_adapter *padapter)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	HAL_DATA_TYPE	*pHalData = GET_PHL_COM(dvobj);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;

	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;
	psrtpriv->last_tx_time = 0;
	psrtpriv->last_tx_complete_time = 0;
#endif
}

u8 sreset_get_wifi_status(_adapter *padapter)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	HAL_DATA_TYPE	*pHalData = GET_PHL_COM(dvobj);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;
	u8 status = WIFI_STATUS_SUCCESS;
	u32 val32 = 0;

	if (psrtpriv->silent_reset_inprogress == _TRUE)
		return status;
	#if 0 /*GEORGIA_TODO_REDEFINE_IO*/
	val32 = rtw_read32(padapter, REG_TXDMA_STATUS);
	#else
	val32 = rtw_hal_get_dma_statu(padapter);
	#endif
	if (val32 == 0xeaeaeaea)
		psrtpriv->Wifi_Error_Status = WIFI_IF_NOT_EXIST;
	else if (val32 != 0) {
		RTW_INFO("txdmastatu(%x)\n", val32);
		psrtpriv->Wifi_Error_Status = WIFI_MAC_TXDMA_ERROR;
	}

	if (WIFI_STATUS_SUCCESS != psrtpriv->Wifi_Error_Status) {
		RTW_INFO("==>%s error_status(0x%x)\n", __FUNCTION__, psrtpriv->Wifi_Error_Status);
		status = (psrtpriv->Wifi_Error_Status & (~(USB_READ_PORT_FAIL | USB_WRITE_PORT_FAIL)));
	}
	RTW_INFO("==> %s wifi_status(0x%x)\n", __FUNCTION__, status);

	/* status restore */
	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;

	return status;
#else
	return WIFI_STATUS_SUCCESS;
#endif
}

void sreset_set_wifi_error_status(_adapter *padapter, u32 status)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	HAL_DATA_TYPE	*pHalData = GET_PHL_COM(dvobj);
	pHalData->srestpriv.Wifi_Error_Status = status;
#endif
}

void sreset_set_trigger_point(_adapter *padapter, s32 tgp)
{
#if defined(DBG_CONFIG_ERROR_DETECT)
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	HAL_DATA_TYPE	*pHalData = GET_PHL_COM(dvobj);
	pHalData->srestpriv.dbg_trigger_point = tgp;
#endif
}

bool sreset_inprogress(_adapter *padapter)
{
#if defined(DBG_CONFIG_ERROR_RESET)
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	HAL_DATA_TYPE	*pHalData = GET_PHL_COM(dvobj);
	return pHalData->srestpriv.silent_reset_inprogress;
#else
	return _FALSE;
#endif
}

void sreset_restore_security_station(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
	struct mlme_priv *mlmepriv = &padapter->mlmepriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta;
	struct mlme_ext_info	*pmlmeinfo = &padapter->mlmeextpriv.mlmext_info;

	rtw_hal_set_hwreg(padapter, HW_VAR_SEC_CFG, NULL);

	if ((padapter->securitypriv.dot11PrivacyAlgrthm == _TKIP_) ||
	    (padapter->securitypriv.dot11PrivacyAlgrthm == _AES_)) {
		psta = rtw_get_stainfo(pstapriv, get_link_bssid(&padapter_link->mlmepriv));
		if (psta == NULL) {
			/* DEBUG_ERR( ("Set wpa_set_encryption: Obtain Sta_info fail\n")); */
		} else {
			/* pairwise key */
			rtw_setstakey_cmd(padapter, psta, UNICAST_KEY, _FALSE);
			/* group key */
			rtw_set_key(padapter, padapter_link, padapter_link->securitypriv.dot118021XGrpKeyid, 0, _FALSE);
		}
	}
}

void sreset_restore_network_station(_adapter *padapter)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *mlmepriv = &padapter_link->mlmepriv;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8 do_rfk = _FALSE;

	rtw_setopmode_cmd(padapter, Ndis802_11Infrastructure, RTW_CMDF_DIRECTLY);

	{
		u8 threshold;
#ifdef CONFIG_USB_HCI
		/* TH=1 => means that invalidate usb rx aggregation */
		/* TH=0 => means that validate usb rx aggregation, use init value. */
#ifdef CONFIG_80211N_HT
		if (mlmepriv->htpriv.ht_option) {
			if (padapter->registrypriv.wifi_spec == 1)
				threshold = 1;
			else
				threshold = 0;
			rtw_hal_set_hwreg(padapter, HW_VAR_RXDMA_AGG_PG_TH, (u8 *)(&threshold));
		} else {
			threshold = 1;
			rtw_hal_set_hwreg(padapter, HW_VAR_RXDMA_AGG_PG_TH, (u8 *)(&threshold));
		}
#endif /* CONFIG_80211N_HT */
#endif
	}

	do_rfk = _TRUE;
	set_channel_bwmode(padapter,
			padapter_link,
			pmlmeext->chandef.chan,
			pmlmeext->chandef.offset,
			pmlmeext->chandef.bw,
			do_rfk);

	rtw_hal_set_hwreg(padapter, HW_VAR_BSSID, pmlmeinfo->network.MacAddress);

	{
		u8	join_type = 0;

		rtw_hal_set_hwreg(padapter, HW_VAR_MLME_JOIN, (u8 *)(&join_type));
	}

	rtw_set_hw_after_join(padapter, 1);
	/* restore Sequence No. */
	rtw_hal_set_hwreg(padapter, HW_VAR_RESTORE_HW_SEQ, 0);

	sreset_restore_security_station(padapter, padapter_link);
}


void sreset_restore_network_status(_adapter *padapter)
{
	struct mlme_priv *mlmepriv = &padapter->mlmepriv;

	if (MLME_IS_STA(padapter)) {
		RTW_INFO(FUNC_ADPT_FMT" fwstate:0x%08x - WIFI_STATION_STATE\n", FUNC_ADPT_ARG(padapter), get_fwstate(mlmepriv));
		sreset_restore_network_station(padapter);
	} else if (MLME_IS_AP(padapter) || MLME_IS_MESH(padapter)) {
		RTW_INFO(FUNC_ADPT_FMT" %s\n", FUNC_ADPT_ARG(padapter), MLME_IS_AP(padapter) ? "AP" : "MESH");
		rtw_ap_restore_network(padapter);
	} else if (MLME_IS_ADHOC(padapter))
		RTW_INFO(FUNC_ADPT_FMT" fwstate:0x%08x - WIFI_ADHOC_STATE\n", FUNC_ADPT_ARG(padapter), get_fwstate(mlmepriv));
	else
		RTW_INFO(FUNC_ADPT_FMT" fwstate:0x%08x - ???\n", FUNC_ADPT_ARG(padapter), get_fwstate(mlmepriv));
}

void sreset_stop_adapter(_adapter *padapter)
{
	struct mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

	if (padapter == NULL)
		return;

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

	rtw_netif_stop_queue(padapter->pnetdev);

	rtw_cancel_all_timer(padapter);

	/* TODO: OS and HCI independent */
#if defined(CONFIG_USB_HCI)
	rtw_tasklet_kill(&pxmitpriv->xmit_tasklet);
#endif

	if (check_fwstate(pmlmepriv, WIFI_UNDER_SURVEY))
		rtw_scan_abort(padapter, 0);

	if (check_fwstate(pmlmepriv, WIFI_UNDER_LINKING)) {
		rtw_set_to_roam(padapter, 0);
		rtw_join_timeout_handler(padapter);
	}

}

void sreset_start_adapter(_adapter *padapter)
{
	struct mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

	if (padapter == NULL)
		return;

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

	if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE))
		sreset_restore_network_status(padapter);

	/* TODO: OS and HCI independent */
#if defined(CONFIG_USB_HCI)
	rtw_tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
#endif
	#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
	if (is_primary_adapter(padapter))
		_set_timer(&adapter_to_dvobj(padapter)->dynamic_chk_timer, 2000);
	#endif
	rtw_netif_wake_queue(padapter->pnetdev);
}

void sreset_reset(_adapter *padapter)
{
#ifdef DBG_CONFIG_ERROR_RESET	
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	systime start = rtw_get_current_time();
	struct dvobj_priv *psdpriv = padapter->dvobj;
	HAL_DATA_TYPE	*pHalData = GET_PHL_COM(psdpriv);
	struct sreset_priv *psrtpriv = &pHalData->srestpriv;

	struct debug_priv *pdbgpriv = &psdpriv->drv_dbg;

	RTW_INFO("%s\n", __FUNCTION__);

	psrtpriv->Wifi_Error_Status = WIFI_STATUS_SUCCESS;

	_enter_pwrlock(&pwrpriv->lock);

	psrtpriv->silent_reset_inprogress = _TRUE;
	pwrpriv->change_rfpwrstate = rf_off;

	rtw_mi_sreset_adapter_hdl(padapter, _FALSE);/*sreset_stop_adapter*/
#ifdef CONFIG_CONCURRENT_MODE
	rtw_mi_ap_info_restore(padapter);
#endif
	rtw_mi_sreset_adapter_hdl(padapter, _TRUE);/*sreset_start_adapter*/

	psrtpriv->silent_reset_inprogress = _FALSE;

	_exit_pwrlock(&pwrpriv->lock);

	RTW_INFO("%s done in %d ms\n", __FUNCTION__, rtw_get_passing_time_ms(start));
	pdbgpriv->dbg_sreset_cnt++;

	psrtpriv->self_dect_fw = _FALSE;
	psrtpriv->rx_cnt = 0;
#endif
}
