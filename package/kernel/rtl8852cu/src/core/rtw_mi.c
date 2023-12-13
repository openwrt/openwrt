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
#define _RTW_MI_C_

#include <drv_types.h>

/* Find union about ch, bw, ch_offset of all linked/linking interfaces */
static int _rtw_mi_get_ch_setting_union(struct dvobj_priv *dvobj, _adapter *adapter, u8 band_idx, u8 ifbmp, enum band_type *band, u8 *ch, u8 *bw, u8 *offset)
{
	_adapter *iface;
	struct _ADAPTER_LINK *iface_link;
	struct link_mlme_ext_priv *mlmeext;
	int i;
	enum band_type band_ret = BAND_MAX;
	u8 ch_ret = 0;
	u8 bw_ret = CHANNEL_WIDTH_20;
	u8 offset_ret = CHAN_OFFSET_NO_EXT;
	int num = 0;

	if (band)
		*band = 0;
	if (ch)
		*ch = 0;
	if (bw)
		*bw = CHANNEL_WIDTH_20;
	if (offset)
		*offset = CHAN_OFFSET_NO_EXT;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface || !(ifbmp & BIT(iface->iface_id)))
			continue;

		if (adapter) {
			if (!rtw_iface_at_same_hwband(adapter, iface))
				continue;
		} else if (band_idx < HW_BAND_MAX){
			if (!rtw_iface_is_operate_at_hwband(iface, band_idx))
				continue;
		}

		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		iface_link = GET_PRIMARY_LINK(iface);
		mlmeext = &iface_link->mlmeextpriv;

		if (!check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE | WIFI_UNDER_LINKING))
			continue;

		if (check_fwstate(&iface->mlmepriv, WIFI_OP_CH_SWITCHING))
			continue;

		if (num == 0) {
			band_ret = mlmeext->chandef.band;
			ch_ret = mlmeext->chandef.chan;
			bw_ret = mlmeext->chandef.bw;
			offset_ret = mlmeext->chandef.offset;
			num++;
			continue;
		}

		if (band_ret != mlmeext->chandef.band || ch_ret != mlmeext->chandef.chan) {
			num = 0;
			break;
		}

		if (bw_ret < mlmeext->chandef.bw) {
			bw_ret = mlmeext->chandef.bw;
			offset_ret = mlmeext->chandef.offset;
		} else if (bw_ret == mlmeext->chandef.bw && offset_ret != mlmeext->chandef.offset) {
			num = 0;
			break;
		}

		num++;
	}

	if (num) {
		if (band)
			*band = band_ret;
		if (ch)
			*ch = ch_ret;
		if (bw)
			*bw = bw_ret;
		if (offset)
			*offset = offset_ret;
	}

	return num;
}

int rtw_mi_get_ch_setting_union_by_ifbmp(_adapter *adapter, u8 ifbmp, enum band_type *band, u8 *ch, u8 *bw, u8 *offset)
{
	return _rtw_mi_get_ch_setting_union(adapter_to_dvobj(adapter), adapter, HW_BAND_MAX, ifbmp, band, ch, bw, offset);
}

inline int rtw_mi_get_ch_setting_union(_adapter *adapter, enum band_type *band, u8 *ch, u8 *bw, u8 *offset)
{
	return rtw_mi_get_ch_setting_union_by_ifbmp(adapter, 0xFF, band, ch, bw, offset);
}

inline int rtw_mi_get_ch_setting_union_no_self(_adapter *adapter, enum band_type *band, u8 *ch, u8 *bw, u8 *offset)
{
	return rtw_mi_get_ch_setting_union_by_ifbmp(adapter, 0xFF & ~BIT(adapter->iface_id), band, ch, bw, offset);
}

int rtw_mi_get_bch_setting_union_by_hwband(struct dvobj_priv *dvobj, u8 band_idx
	, enum band_type *band, u8 *ch, u8 *bw, u8 *offset)
{
	return _rtw_mi_get_ch_setting_union(dvobj, NULL, band_idx, 0xFF, band, ch, bw, offset);
}

int rtw_mi_get_bch_setting_union_by_hwband_ifbmp(struct dvobj_priv *dvobj, u8 band_idx, u8 ifbmp
	, enum band_type *band, u8 *ch, u8 *bw, u8 *offset)
{
	return _rtw_mi_get_ch_setting_union(dvobj, NULL, band_idx, ifbmp, band, ch, bw, offset);
}

static void _rtw_mi_status(struct dvobj_priv *dvobj, _adapter *adapter, u8 band_idx, u8 ifbmp, struct mi_state *mstate)
{
	_adapter *iface;
	int i;

	_rtw_memset(mstate, 0, sizeof(struct mi_state));

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface || !(ifbmp & BIT(iface->iface_id)))
			continue;

		if (adapter) {
			if (!rtw_iface_at_same_hwband(adapter, iface))
				continue;
		} else if (band_idx < HW_BAND_MAX){
			if (!rtw_iface_is_operate_at_hwband(iface, band_idx))
				continue;
		}

		if (MLME_IS_STA(iface)) {
			MSTATE_STA_NUM(mstate)++;
			if (check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE) == _TRUE) {
				MSTATE_STA_LD_NUM(mstate)++;

				#ifdef CONFIG_TDLS
				if (iface->tdlsinfo.link_established == _TRUE)
					MSTATE_TDLS_LD_NUM(mstate)++;
				#endif
				#ifdef CONFIG_P2P
				if (MLME_IS_GC(iface))
					MSTATE_P2P_GC_NUM(mstate)++;
				#endif
			}
			if (check_fwstate(&iface->mlmepriv, WIFI_UNDER_LINKING) == _TRUE)
				MSTATE_STA_LG_NUM(mstate)++;

#ifdef CONFIG_AP_MODE
		} else if (check_fwstate(&iface->mlmepriv, WIFI_AP_STATE) == _TRUE ) {
			if (check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE) == _TRUE) {
				MSTATE_AP_NUM(mstate)++;
				if (iface->stapriv.asoc_sta_count >= 2)
					MSTATE_AP_LD_NUM(mstate)++;
				#ifdef CONFIG_P2P
				if (MLME_IS_GO(iface))
					MSTATE_P2P_GO_NUM(mstate)++;
				#endif
			} else
				MSTATE_AP_STARTING_NUM(mstate)++;
#endif

		} else if (check_fwstate(&iface->mlmepriv, WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE) == _TRUE
			&& check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE) == _TRUE
		) {
			MSTATE_ADHOC_NUM(mstate)++;
			if (iface->stapriv.asoc_sta_count > 2)
				MSTATE_ADHOC_LD_NUM(mstate)++;

#ifdef CONFIG_RTW_MESH
		} else if (check_fwstate(&iface->mlmepriv, WIFI_MESH_STATE) == _TRUE
			&& check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE) == _TRUE
		) {
			MSTATE_MESH_NUM(mstate)++;
			if (iface->stapriv.asoc_sta_count > 2)
				MSTATE_MESH_LD_NUM(mstate)++;
#endif

		}

		if (check_fwstate(&iface->mlmepriv, WIFI_UNDER_WPS) == _TRUE)
			MSTATE_WPS_NUM(mstate)++;

		if (check_fwstate(&iface->mlmepriv, WIFI_UNDER_SURVEY) == _TRUE) {
			MSTATE_SCAN_NUM(mstate)++;

			if (mlmeext_scan_state(&iface->mlmeextpriv) != SCAN_DISABLE
				&& mlmeext_scan_state(&iface->mlmeextpriv) != SCAN_BACK_OP)
				MSTATE_SCAN_ENTER_NUM(mstate)++;
		}

#ifdef CONFIG_IOCTL_CFG80211
		if (rtw_cfg80211_get_is_mgmt_tx(iface))
			MSTATE_MGMT_TX_NUM(mstate)++;
		#ifdef CONFIG_P2P
		if (rtw_cfg80211_get_is_roch(iface) == _TRUE)
			MSTATE_ROCH_NUM(mstate)++;
		#endif
#endif /* CONFIG_IOCTL_CFG80211 */
#ifdef CONFIG_P2P
		if (MLME_IS_PD(iface))
			MSTATE_P2P_DV_NUM(mstate)++;
#endif
	}
}

void rtw_mi_status_by_ifbmp(_adapter *adapter, u8 ifbmp, struct mi_state *mstate)
{
	_rtw_mi_status(adapter_to_dvobj(adapter), adapter, HW_BAND_MAX, ifbmp, mstate);
}

inline void rtw_mi_status(_adapter *adapter, struct mi_state *mstate)
{
	rtw_mi_status_by_ifbmp(adapter, 0xFF, mstate);
}

inline void rtw_mi_status_no_self(_adapter *adapter, struct mi_state *mstate)
{
	rtw_mi_status_by_ifbmp(adapter, 0xFF & ~BIT(adapter->iface_id), mstate);
}

inline void rtw_mi_status_no_others(_adapter *adapter, struct mi_state *mstate)
{
	rtw_mi_status_by_ifbmp(adapter, BIT(adapter->iface_id), mstate);
}

void rtw_mi_status_by_hwband(struct dvobj_priv *dvobj, u8 band_idx, struct mi_state *mstate)
{
	_rtw_mi_status(dvobj, NULL, band_idx, 0xFF, mstate);
}

void rtw_mi_status_by_hwband_ifbmp(struct dvobj_priv *dvobj, u8 band_idx, u8 ifbmp, struct mi_state *mstate)
{
	_rtw_mi_status(dvobj, NULL, band_idx, ifbmp, mstate);
}

/* For now, not handle union_ch/bw/offset */
inline void rtw_mi_status_merge(struct mi_state *d, struct mi_state *a)
{
	d->sta_num += a->sta_num;
	d->ld_sta_num += a->ld_sta_num;
	d->lg_sta_num += a->lg_sta_num;
#ifdef CONFIG_TDLS
	d->ld_tdls_num += a->ld_tdls_num;
#endif
#ifdef CONFIG_AP_MODE
	d->ap_num += a->ap_num;
	d->ld_ap_num += a->ld_ap_num;
#endif
	d->adhoc_num += a->adhoc_num;
	d->ld_adhoc_num += a->ld_adhoc_num;
#ifdef CONFIG_RTW_MESH
	d->mesh_num += a->mesh_num;
	d->ld_mesh_num += a->ld_mesh_num;
#endif
	d->scan_num += a->scan_num;
	d->scan_enter_num += a->scan_enter_num;
	d->uwps_num += a->uwps_num;
#ifdef CONFIG_IOCTL_CFG80211
	#ifdef CONFIG_P2P
	d->roch_num += a->roch_num;
	#endif
	d->mgmt_tx_num += a->mgmt_tx_num;
#endif
}

void dump_mi_status(void *sel, struct dvobj_priv *dvobj)
{
	u8 i;
	struct mi_state *iface_state;

	RTW_PRINT_SEL(sel, "== dvobj-iface_state ==\n");

	for (i = 0; i < MAX_BAND_NUM; i++) {
		RTW_PRINT_SEL(sel, "== Band-%d iface_state ==\n", i);
		iface_state = &dvobj->iface_state[i];
		RTW_PRINT_SEL(sel, "sta_num:%d\n", MSTATE_STA_NUM(iface_state));
		RTW_PRINT_SEL(sel, "linking_sta_num:%d\n", MSTATE_STA_LG_NUM(iface_state));
		RTW_PRINT_SEL(sel, "linked_sta_num:%d\n", MSTATE_STA_LD_NUM(iface_state));
#ifdef CONFIG_TDLS
		RTW_PRINT_SEL(sel, "linked_tdls_num:%d\n", MSTATE_TDLS_LD_NUM(iface_state));
#endif
#ifdef CONFIG_AP_MODE
		RTW_PRINT_SEL(sel, "ap_num:%d\n", MSTATE_AP_NUM(iface_state));
		RTW_PRINT_SEL(sel, "starting_ap_num:%d\n", MSTATE_AP_STARTING_NUM(iface_state));
		RTW_PRINT_SEL(sel, "linked_ap_num:%d\n", MSTATE_AP_LD_NUM(iface_state));
#endif
		RTW_PRINT_SEL(sel, "adhoc_num:%d\n", MSTATE_ADHOC_NUM(iface_state));
		RTW_PRINT_SEL(sel, "linked_adhoc_num:%d\n", MSTATE_ADHOC_LD_NUM(iface_state));
#ifdef CONFIG_RTW_MESH
		RTW_PRINT_SEL(sel, "mesh_num:%d\n", MSTATE_MESH_NUM(iface_state));
		RTW_PRINT_SEL(sel, "linked_mesh_num:%d\n", MSTATE_MESH_LD_NUM(iface_state));
#endif
#ifdef CONFIG_P2P
		RTW_PRINT_SEL(sel, "p2p_device_num:%d\n", MSTATE_P2P_DV_NUM(iface_state));
		RTW_PRINT_SEL(sel, "p2p_gc_num:%d\n", MSTATE_P2P_GC_NUM(iface_state));
		RTW_PRINT_SEL(sel, "p2p_go_num:%d\n", MSTATE_P2P_GO_NUM(iface_state));
#endif
		RTW_PRINT_SEL(sel, "scan_num:%d\n", MSTATE_SCAN_NUM(iface_state));
		RTW_PRINT_SEL(sel, "under_wps_num:%d\n", MSTATE_WPS_NUM(iface_state));
#if defined(CONFIG_IOCTL_CFG80211)
		#if defined(CONFIG_P2P)
		RTW_PRINT_SEL(sel, "roch_num:%d\n", MSTATE_ROCH_NUM(iface_state));
		#endif
		RTW_PRINT_SEL(sel, "mgmt_tx_num:%d\n", MSTATE_MGMT_TX_NUM(iface_state));
#endif
	}
	RTW_PRINT_SEL(sel, "================\n\n");
}

void dump_dvobj_mi_status(void *sel, const char *fun_name, _adapter *adapter)
{
	RTW_INFO("\n[ %s ] call %s\n", fun_name, __func__);
	dump_mi_status(sel, adapter_to_dvobj(adapter));
}

inline void rtw_mi_update_iface_status(struct mlme_priv *pmlmepriv, sint state)
{
	_adapter *adapter = container_of(pmlmepriv, _adapter, mlmepriv);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct mi_state *iface_state = NULL;
	struct mi_state tmp_mstate;
	u8 hw_band_idx = 0;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);

	if (state == WIFI_MONITOR_STATE
		|| state == 0xFFFFFFFF
	)
		return;

	if (0)
		RTW_INFO("%s => will change or clean state to 0x%08x\n", __func__, state);

	if (rtw_is_adapter_up(adapter))
		hw_band_idx = adapter_link->wrlink->hw_band;

	iface_state = &dvobj->iface_state[hw_band_idx];

	rtw_mi_status(adapter, &tmp_mstate);
	_rtw_memcpy(iface_state, &tmp_mstate, sizeof(struct mi_state));

#ifdef DBG_IFACE_STATUS
	DBG_IFACE_STATUS_DUMP(adapter);
#endif
}
u8 rtw_mi_check_status(_adapter *adapter, u8 type)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct mi_state *iface_state;
	u8 ret = _FALSE;
	struct mr_query_info info = {0};
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);

	if (!rtw_is_adapter_up(adapter)) {
		RTW_ERR("%s-"ADPT_FMT" !rtw_is_adapter_up\n", __func__, ADPT_ARG(adapter));
		return ret;
	}
#ifdef DBG_IFACE_STATUS
	DBG_IFACE_STATUS_DUMP(adapter);
	RTW_INFO("%s-"ADPT_FMT" check type:%d\n", __func__, ADPT_ARG(adapter), type);
#endif
	iface_state = &dvobj->iface_state[adapter_link->wrlink->hw_band];
	rtw_phl_mr_query_info(dvobj->phl, adapter->phl_role, adapter_link->wrlink, &info);
#if 0
	if (info.cur_info.ld_sta_num != MSTATE_STA_LD_NUM(iface_state))
		RTW_ERR("ld_sta_num (%d:%d)\n", info.cur_info.ld_sta_num, MSTATE_STA_LD_NUM(iface_state));
	if (info.cur_info.ap_num != MSTATE_AP_NUM(iface_state))
		RTW_ERR("ap_num (%d:%d)\n", info.cur_info.ap_num, MSTATE_AP_NUM(iface_state));
	if (info.cur_info.ld_ap_num != MSTATE_AP_LD_NUM(iface_state))
		RTW_ERR("ld_ap_num (%d:%d)\n", info.cur_info.ld_ap_num, MSTATE_AP_LD_NUM(iface_state));
#endif
	switch (type) {
	case MI_LINKED:
		/*if (MSTATE_STA_LD_NUM(iface_state) || MSTATE_AP_NUM(iface_state) || MSTATE_ADHOC_NUM(iface_state) || MSTATE_MESH_NUM(iface_state))*/ /*check_fwstate(&iface->mlmepriv, WIFI_ASOC_STATE)*/
		if (info.cur_info.ld_sta_num || info.cur_info.ap_num) /*info.cur_info.adhoc_num || info.cur_info.mesh_num*/
			ret = _TRUE;
		break;
	case MI_ASSOC:
		/*if (MSTATE_STA_LD_NUM(iface_state) || MSTATE_AP_LD_NUM(iface_state) || MSTATE_ADHOC_LD_NUM(iface_state) || MSTATE_MESH_LD_NUM(iface_state))*/
		if (info.cur_info.ld_sta_num || info.cur_info.ld_ap_num) /*info.cur_info.ld_adhoc_num || info.cur_info.ld_mesh_num*/

		ret = _TRUE;
		break;
	case MI_AP_MODE:
		/*if (MSTATE_AP_NUM(iface_state))*/
		if (info.cur_info.ap_num)
			ret = _TRUE;
		break;

	case MI_AP_ASSOC:
		/*if (MSTATE_AP_LD_NUM(iface_state))*/
		if (info.cur_info.ld_ap_num)
			ret = _TRUE;
		break;
	case MI_STA_LINKED:
		/*if (MSTATE_STA_LD_NUM(iface_state))*/
		if (info.cur_info.ld_sta_num)
			ret = _TRUE;
		break;
	case MI_STA_LINKING:
		/*if (MSTATE_STA_LG_NUM(iface_state))*/
		if (info.cur_info.lg_sta_num)
			ret = _TRUE;
		break;
#if 0
	case MI_UNDER_WPS:
		if (MSTATE_WPS_NUM(iface_state))
			ret = _TRUE;
		break;

	case MI_ADHOC:
		if (MSTATE_ADHOC_NUM(iface_state))
			ret = _TRUE;
		break;
	case MI_ADHOC_ASSOC:
		if (MSTATE_ADHOC_LD_NUM(iface_state))
			ret = _TRUE;
		break;

	#ifdef CONFIG_RTW_MESH
	case MI_MESH:
		if (MSTATE_MESH_NUM(iface_state))
			ret = _TRUE;
		break;
	case MI_MESH_ASSOC:
		if (MSTATE_MESH_LD_NUM(iface_state))
			ret = _TRUE;
		break;
	#endif

	case MI_STA_NOLINK: /* this is misleading, but not used now */
		if (MSTATE_STA_NUM(iface_state) && (!(MSTATE_STA_LD_NUM(iface_state) || MSTATE_STA_LG_NUM(iface_state))))
			ret = _TRUE;
		break;
#endif
	default:
		break;
	}
	return ret;
}

/*
* return value : 0 is failed or have not interface meet condition
* return value : !0 is success or interface numbers which meet condition
* return value of ops_func must be _TRUE or _FALSE
*/
static u8 _rtw_mi_process(_adapter *padapter, bool exclude_self,
		  void *data, u8(*ops_func)(_adapter *padapter, void *data))
{
	int i;
	_adapter *iface;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	u8 ret = 0;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if ((iface) && rtw_is_adapter_up(iface)) {

			if ((exclude_self) && (iface == padapter))
				continue;

			if (!rtw_iface_at_same_hwband(padapter, iface))
				continue;

			if (ops_func)
				if (_TRUE == ops_func(iface, data))
					ret++;
		}
	}
	return ret;
}

static u8 _rtw_mi_process_by_hwband_ifbmp(struct dvobj_priv *dvobj
	, _adapter *adapter, u8 band_idx, u8 ifbmp
	, void *data, u8(*ops_func)(_adapter *adapter, void *data))
{
	int i;
	_adapter *iface;
	u8 ret = 0;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface || !rtw_is_adapter_up(iface) || !(ifbmp & BIT(iface->iface_id)))
			continue;

		if (adapter) {
			if (!rtw_iface_at_same_hwband(adapter, iface))
				continue;
		} else if (band_idx < HW_BAND_MAX){
			if (!rtw_iface_is_operate_at_hwband(iface, band_idx))
				continue;
		}

		if (ops_func)
			if (_TRUE == ops_func(iface, data))
				ret++;
	}
	return ret;
}

static u8 _rtw_mi_process_without_schk(_adapter *padapter, bool exclude_self,
		  void *data, u8(*ops_func)(_adapter *padapter, void *data))
{
	int i;
	_adapter *iface;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	u8 ret = 0;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (iface) {
			if ((exclude_self) && (iface == padapter))
				continue;

			if (ops_func)
				if (ops_func(iface, data) == _TRUE)
					ret++;
		}
	}
	return ret;
}

static u8 _rtw_mi_netif_caroff_qstop(_adapter *padapter, void *data)
{
	struct net_device *pnetdev = padapter->pnetdev;

	rtw_netif_carrier_off(pnetdev);
	rtw_netif_stop_queue(pnetdev);
	return _TRUE;
}
u8 rtw_mi_netif_caroff_qstop(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_netif_caroff_qstop);
}
u8 rtw_mi_buddy_netif_caroff_qstop(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_netif_caroff_qstop);
}

static u8 _rtw_mi_disconnect(_adapter *adapter, void *data)
{
	struct mlme_priv *mlme = &adapter->mlmepriv;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	struct link_mlme_ext_priv *lmlmeext = &adapter_link->mlmeextpriv;
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;
	struct mlme_ext_info *mlmeinfo = &(mlmeext->mlmext_info);

	if ((MLME_IS_AP(adapter) || MLME_IS_MESH(adapter))
			&& check_fwstate(mlme, WIFI_ASOC_STATE)) {
		#ifdef CONFIG_SPCT_CH_SWITCH
		if (1)
		rtw_ap_inform_ch_switch(adapter, adapter_link,
				lmlmeext->chandef.chan, lmlmeext->chandef.offset);
		else
		#endif
		#ifdef CONFIG_STA_CMD_DISPR
			rtw_phl_cmd_stop_beacon(adapter_to_dvobj(adapter)->phl, adapter_link->wrlink, _TRUE, PHL_CMD_NO_WAIT, 0);
		#else
			rtw_phl_cmd_stop_beacon(adapter_to_dvobj(adapter)->phl, adapter->phl_role, _TRUE, PHL_CMD_DIRECTLY, 0);
		#endif
		rtw_sta_flush(adapter, _TRUE);

		set_fwstate(mlme, WIFI_OP_CH_SWITCHING);
		rtw_phl_chanctx_del(adapter_to_dvobj(adapter)->phl, adapter->phl_role, adapter_link->wrlink, NULL);

	} else if (MLME_IS_STA(adapter)
			&& check_fwstate(mlme, WIFI_ASOC_STATE)) {
		Disconnect_type disc_code = DISCONNECTION_NOT_YET_OCCUR;

		if (data)
			disc_code = *(int *)data;

		rtw_disassoc_cmd(adapter, 500, RTW_CMDF_DIRECTLY);
#ifndef CONFIG_STA_CMD_DISPR
		rtw_free_assoc_resources(adapter, _TRUE);
#endif /* !CONFIG_STA_CMD_DISPR */
		rtw_indicate_disconnect(adapter, 0, _FALSE);

		if (disc_code == DISCONNECTION_BY_DRIVER_DUE_TO_EACH_IFACE_CHBW_NOT_SYNC) {
			mlmeinfo->disconnect_occurred_time = rtw_systime_to_ms(rtw_get_current_time());
			mlmeinfo->disconnect_code = disc_code;
			mlmeinfo->wifi_reason_code = WLAN_REASON_DEAUTH_LEAVING;
		}
	}
	return _TRUE;
}

u8 rtw_mi_disconnect(_adapter *adapter)
{
	return _rtw_mi_process(adapter, _FALSE, NULL, _rtw_mi_disconnect);
}
u8 rtw_mi_buddy_disconnect(_adapter *adapter, Disconnect_type disc_code)
{
	return _rtw_mi_process(adapter, _TRUE, (void *)&disc_code,
			       _rtw_mi_disconnect);
	/*_phl_mr_process_by_band(phl_info, wifi_role, _TRUE, NULL, _rtw_mi_disconnect);*/
}

#ifdef	PHL_MR_PROC_CMD
static u8 _rtw_dump_mac_addr(void *drv_priv, u8 wr_idx, void *data)
{
	_adapter *adapter;
	struct dvobj_priv *dvobj = (struct dvobj_priv *)drv_priv;

	adapter = dvobj->padapters[wr_idx];
	if (rtw_is_adapter_up(adapter))
		RTW_INFO("if%d mac_addr : "MAC_FMT"\n", wr_idx, MAC_ARG(adapter_mac_addr(adapter)));
	return _SUCCESS;
}

u8 rtw_mi_dump_mac_addr(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	return rtw_phl_mr_process_by_band(dvobj->phl, HW_BAND_0, NULL,
							_rtw_dump_mac_addr);
}
#endif

static u8 _rtw_mi_netif_caron_qstart(_adapter *padapter, void *data)
{
	struct net_device *pnetdev = padapter->pnetdev;

	rtw_netif_carrier_on(pnetdev);
	rtw_netif_start_queue(pnetdev);
	return _TRUE;
}
u8 rtw_mi_netif_caron_qstart(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_netif_caron_qstart);
}
u8 rtw_mi_buddy_netif_caron_qstart(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_netif_caron_qstart);
}

static u8 _rtw_mi_netif_stop_queue(_adapter *padapter, void *data)
{
	struct net_device *pnetdev = padapter->pnetdev;

	rtw_netif_stop_queue(pnetdev);
	return _TRUE;
}
u8 rtw_mi_netif_stop_queue(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_netif_stop_queue);
}
u8 rtw_mi_buddy_netif_stop_queue(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_netif_stop_queue);
}

static u8 _rtw_mi_netif_wake_queue(_adapter *padapter, void *data)
{
	struct net_device *pnetdev = padapter->pnetdev;

	if (pnetdev)
		rtw_netif_wake_queue(pnetdev);
	return _TRUE;
}
u8 rtw_mi_netif_wake_queue(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_netif_wake_queue);
}
u8 rtw_mi_buddy_netif_wake_queue(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_netif_wake_queue);
}

static u8 _rtw_mi_netif_carrier_on(_adapter *padapter, void *data)
{
	struct net_device *pnetdev = padapter->pnetdev;

	if (pnetdev)
		rtw_netif_carrier_on(pnetdev);
	return _TRUE;
}
u8 rtw_mi_netif_carrier_on(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_netif_carrier_on);
}
u8 rtw_mi_buddy_netif_carrier_on(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_netif_carrier_on);
}

static u8 _rtw_mi_netif_carrier_off(_adapter *padapter, void *data)
{
	struct net_device *pnetdev = padapter->pnetdev;

	if (pnetdev)
		rtw_netif_carrier_off(pnetdev);
	return _TRUE;
}
u8 rtw_mi_netif_carrier_off(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_netif_carrier_off);
}
u8 rtw_mi_buddy_netif_carrier_off(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_netif_carrier_off);
}

static u8 _rtw_mi_scan_abort(_adapter *adapter, void *data)
{
	bool bwait = *(bool *)data;

	if (bwait)
		rtw_scan_abort(adapter, 0);
	else
		rtw_scan_abort_no_wait(adapter);

	return _TRUE;
}
void rtw_mi_scan_abort(_adapter *adapter, bool bwait)
{
	bool in_data = bwait;

	_rtw_mi_process(adapter, _FALSE, &in_data, _rtw_mi_scan_abort);

}
void rtw_mi_buddy_scan_abort(_adapter *adapter, bool bwait)
{
	bool in_data = bwait;

	_rtw_mi_process(adapter, _TRUE, &in_data, _rtw_mi_scan_abort);
}

static u8 _rtw_mi_cancel_all_timer(_adapter *adapter, void *data)
{
	rtw_cancel_all_timer(adapter);
	return _TRUE;
}
void rtw_mi_cancel_all_timer(_adapter *adapter)
{
	_rtw_mi_process(adapter, _FALSE, NULL, _rtw_mi_cancel_all_timer);
}
void rtw_mi_buddy_cancel_all_timer(_adapter *adapter)
{
	_rtw_mi_process(adapter, _TRUE, NULL, _rtw_mi_cancel_all_timer);
}

static u8 _rtw_mi_reset_drv_sw(_adapter *adapter, void *data)
{
	rtw_reset_drv_sw(adapter);
	return _TRUE;
}
void rtw_mi_reset_drv_sw(_adapter *adapter)
{
	_rtw_mi_process_without_schk(adapter, _FALSE, NULL, _rtw_mi_reset_drv_sw);
}
void rtw_mi_buddy_reset_drv_sw(_adapter *adapter)
{
	_rtw_mi_process_without_schk(adapter, _TRUE, NULL, _rtw_mi_reset_drv_sw);
}

u8 rtw_mi_hal_iface_init(_adapter *padapter)
{
	int i;
	_adapter *iface;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	u8 ret = _SUCCESS;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (iface && iface->netif_up) {
			ret = rtw_hw_iface_init(iface);
			if (ret == _FAIL)
				break;
		}
	}
	return ret;
}


static u8 _rtw_mi_suspend_free_assoc_resource(_adapter *padapter, void *data)
{
	return rtw_suspend_free_assoc_resource(padapter);
}
void rtw_mi_suspend_free_assoc_resource(_adapter *adapter)
{
	_rtw_mi_process(adapter, _FALSE, NULL, _rtw_mi_suspend_free_assoc_resource);
}
void rtw_mi_buddy_suspend_free_assoc_resource(_adapter *adapter)
{
	_rtw_mi_process(adapter, _TRUE, NULL, _rtw_mi_suspend_free_assoc_resource);
}

static u8 _rtw_mi_is_scan_deny(_adapter *adapter, void *data)
{
	return rtw_is_scan_deny(adapter);
}

u8 rtw_mi_is_scan_deny(_adapter *adapter)
{
	return _rtw_mi_process(adapter, _FALSE, NULL, _rtw_mi_is_scan_deny);

}
u8 rtw_mi_buddy_is_scan_deny(_adapter *adapter)
{
	return _rtw_mi_process(adapter, _TRUE, NULL, _rtw_mi_is_scan_deny);
}

#ifdef CONFIG_SET_SCAN_DENY_TIMER
static u8 _rtw_mi_set_scan_deny(_adapter *adapter, void *data)
{
	u32 ms = *(u32 *)data;

	rtw_set_scan_deny(adapter, ms);
	return _TRUE;
}
void rtw_mi_set_scan_deny(_adapter *adapter, u32 ms)
{
	u32 in_data = ms;

	_rtw_mi_process(adapter, _FALSE, &in_data, _rtw_mi_set_scan_deny);
}
void rtw_mi_buddy_set_scan_deny(_adapter *adapter, u32 ms)
{
	u32 in_data = ms;

	_rtw_mi_process(adapter, _TRUE, &in_data, _rtw_mi_set_scan_deny);
}
#endif /*CONFIG_SET_SCAN_DENY_TIMER*/

static u8 _rtw_mi_beacon_update(_adapter *padapter, void *data)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	if (!MLME_IS_STA(padapter)
	    && check_fwstate(&padapter->mlmepriv, WIFI_ASOC_STATE) == _TRUE) {
		RTW_INFO(ADPT_FMT" - update_beacon\n", ADPT_ARG(padapter));
		rtw_update_beacon(padapter, padapter_link,
				0xFF, NULL, _TRUE, 0);
	}
	return _TRUE;
}

void rtw_mi_beacon_update(_adapter *padapter)
{
	_rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_beacon_update);
}

void rtw_mi_buddy_beacon_update(_adapter *padapter)
{
	_rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_beacon_update);
}

u8 _rtw_mi_busy_traffic_check(_adapter *padapter, void *data)
{
	return padapter->mlmepriv.LinkDetectInfo.bBusyTraffic;
}

u8 rtw_mi_busy_traffic_check(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_busy_traffic_check);
}
u8 rtw_mi_buddy_busy_traffic_check(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_busy_traffic_check);
}
static u8 _rtw_mi_check_mlmeinfo_state(_adapter *padapter, void *data)
{
	u32 state = *(u32 *)data;
	struct mlme_ext_priv *mlmeext = &padapter->mlmeextpriv;

	/*if (mlmeext_msr(mlmeext) == state)*/
	if (check_mlmeinfo_state(mlmeext, state))
		return _TRUE;
	else
		return _FALSE;
}

u8 rtw_mi_check_mlmeinfo_state(_adapter *padapter, u32 state)
{
	u32 in_data = state;

	return _rtw_mi_process(padapter, _FALSE, &in_data, _rtw_mi_check_mlmeinfo_state);
}

u8 rtw_mi_buddy_check_mlmeinfo_state(_adapter *padapter, u32 state)
{
	u32 in_data = state;

	return _rtw_mi_process(padapter, _TRUE, &in_data, _rtw_mi_check_mlmeinfo_state);
}

/*#define DBG_DUMP_FW_STATE*/
#ifdef DBG_DUMP_FW_STATE
static void rtw_dbg_dump_fwstate(_adapter *padapter, sint state)
{
	u8 buf[32] = {0};

	if (state & WIFI_FW_NULL_STATE) {
		_rtw_memset(buf, 0, 32);
		sprintf(buf, "WIFI_FW_NULL_STATE");
		RTW_INFO(FUNC_ADPT_FMT"fwstate-%s\n", FUNC_ADPT_ARG(padapter), buf);
	}

	if (state & WIFI_ASOC_STATE) {
		_rtw_memset(buf, 0, 32);
		sprintf(buf, "WIFI_ASOC_STATE");
		RTW_INFO(FUNC_ADPT_FMT"fwstate-%s\n", FUNC_ADPT_ARG(padapter), buf);
	}

	if (state & WIFI_UNDER_LINKING) {
		_rtw_memset(buf, 0, 32);
		sprintf(buf, "WIFI_UNDER_LINKING");
		RTW_INFO(FUNC_ADPT_FMT"fwstate-%s\n", FUNC_ADPT_ARG(padapter), buf);
	}

	if (state & WIFI_UNDER_SURVEY) {
		_rtw_memset(buf, 0, 32);
		sprintf(buf, "WIFI_UNDER_SURVEY");
		RTW_INFO(FUNC_ADPT_FMT"fwstate-%s\n", FUNC_ADPT_ARG(padapter), buf);
	}
}
#endif

static u8 _rtw_mi_check_fwstate(_adapter *padapter, void *data)
{
	u8 ret = _FALSE;

	sint state = *(sint *)data;

	if ((state == WIFI_FW_NULL_STATE) &&
	    (padapter->mlmepriv.fw_state == WIFI_FW_NULL_STATE))
		ret = _TRUE;
	else if (_TRUE == check_fwstate(&padapter->mlmepriv, state))
		ret = _TRUE;
#ifdef DBG_DUMP_FW_STATE
	if (ret)
		rtw_dbg_dump_fwstate(padapter, state);
#endif
	return ret;
}

u8 rtw_mi_check_fwstate(_adapter *padapter, sint state)
{
	sint in_data = state;

	return _rtw_mi_process(padapter, _FALSE, &in_data, _rtw_mi_check_fwstate);
}

u8 rtw_mi_buddy_check_fwstate(_adapter *padapter, sint state)
{
	sint in_data = state;

	return _rtw_mi_process(padapter, _TRUE, &in_data, _rtw_mi_check_fwstate);
}

u8 rtw_mi_check_fwstate_by_hwband(struct dvobj_priv *dvobj, u8 band_idx, sint state)
{
	sint in_data = state;

	return _rtw_mi_process_by_hwband_ifbmp(dvobj, NULL, band_idx, 0xFF, &in_data, _rtw_mi_check_fwstate);
}

static u8 _rtw_mi_traffic_statistics(_adapter *padapter , void *data)
{
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(padapter);

	/* Tx */
	pdvobjpriv->traffic_stat.tx_bytes += padapter->xmitpriv.tx_bytes;
	pdvobjpriv->traffic_stat.tx_pkts += padapter->xmitpriv.tx_pkts;
	pdvobjpriv->traffic_stat.tx_drop += padapter->xmitpriv.tx_drop;

	/* Rx */
	pdvobjpriv->traffic_stat.rx_bytes += padapter->recvinfo.rx_bytes;
	pdvobjpriv->traffic_stat.rx_pkts += padapter->recvinfo.rx_pkts;
	pdvobjpriv->traffic_stat.rx_drop += padapter->recvinfo.rx_drop;
	return _TRUE;
}
u8 rtw_mi_traffic_statistics(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_traffic_statistics);
}

static u8 _rtw_mi_check_miracast_enabled(_adapter *padapter , void *data)
{
	return is_miracast_enabled(padapter);
}
u8 rtw_mi_check_miracast_enabled(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_check_miracast_enabled);
}

#if 0 /*def CONFIG_XMIT_THREAD_MODE*/
static u8 _rtw_mi_check_pending_xmitbuf(_adapter *padapter , void *data)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	return check_pending_xmitbuf(pxmitpriv);
}
u8 rtw_mi_check_pending_xmitbuf(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_check_pending_xmitbuf);
}
u8 rtw_mi_buddy_check_pending_xmitbuf(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_check_pending_xmitbuf);
}
#endif

static void _rtw_mi_adapter_reset(_adapter *padapter , u8 exclude_self)
{
	int i;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);

	for (i = 0; i < dvobj->iface_nums; i++) {
		if (dvobj->padapters[i]) {
			if ((exclude_self) && (dvobj->padapters[i] == padapter))
				continue;
			dvobj->padapters[i] = NULL;
		}
	}
}

void rtw_mi_adapter_reset(_adapter *padapter)
{
	_rtw_mi_adapter_reset(padapter, _FALSE);
}

void rtw_mi_buddy_adapter_reset(_adapter *padapter)
{
	_rtw_mi_adapter_reset(padapter, _TRUE);
}

static u8 _rtw_mi_dynamic_check_handlder(struct _ADAPTER *a, void *data)
{
	rtw_iface_dynamic_check_handlder(a);
	return _TRUE;
}

u8 rtw_mi_dynamic_check_handlder(struct _ADAPTER *a)
{
	return _rtw_mi_process(a, _FALSE, NULL, _rtw_mi_dynamic_check_handlder);
}

#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
static u8 _rtw_mi_dynamic_check_timer_handlder(_adapter *adapter, void *data)
{
	rtw_iface_dynamic_check_timer_handlder(adapter);
	return _TRUE;
}
u8 rtw_mi_dynamic_check_timer_handlder(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_dynamic_check_timer_handlder);
}
u8 rtw_mi_buddy_dynamic_check_timer_handlder(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_dynamic_check_timer_handlder);
}
#endif

static u8 _rtw_mi_dynamic_chk_wk_hdl(_adapter *adapter, void *data)
{
	rtw_iface_dynamic_chk_wk_hdl(adapter);
	return _TRUE;
}
u8 rtw_mi_dynamic_chk_wk_hdl(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_dynamic_chk_wk_hdl);
}
u8 rtw_mi_buddy_dynamic_chk_wk_hdl(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_dynamic_chk_wk_hdl);
}

static u8 _rtw_mi_os_xmit_schedule(_adapter *adapter, void *data)
{
	rtw_os_xmit_schedule(adapter);
	return _TRUE;
}
u8 rtw_mi_os_xmit_schedule(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_os_xmit_schedule);
}
u8 rtw_mi_buddy_os_xmit_schedule(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_os_xmit_schedule);
}

static u8 _rtw_mi_report_survey_event(_adapter *adapter, void *data)
{
	union recv_frame *precv_frame = (union recv_frame *)data;

	report_survey_event(adapter, precv_frame);
	return _TRUE;
}
u8 rtw_mi_report_survey_event(_adapter *padapter, union recv_frame *precv_frame)
{
	return _rtw_mi_process(padapter, _FALSE, precv_frame, _rtw_mi_report_survey_event);
}
u8 rtw_mi_buddy_report_survey_event(_adapter *padapter, union recv_frame *precv_frame)
{
	return _rtw_mi_process(padapter, _TRUE, precv_frame, _rtw_mi_report_survey_event);
}

static u8 _rtw_mi_sreset_adapter_hdl(_adapter *adapter, void *data)
{
	u8 bstart = *(u8 *)data;

	if (bstart)
		sreset_start_adapter(adapter);
	else
		sreset_stop_adapter(adapter);
	return _TRUE;
}
u8 rtw_mi_sreset_adapter_hdl(_adapter *padapter, u8 bstart)
{
	u8 in_data = bstart;

	return _rtw_mi_process(padapter, _FALSE, &in_data, _rtw_mi_sreset_adapter_hdl);
}

#if defined(DBG_CONFIG_ERROR_RESET) && defined(CONFIG_CONCURRENT_MODE)
void rtw_mi_ap_info_restore(_adapter *adapter)
{
	int i;
	_adapter *iface;
	struct mlme_priv *pmlmepriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (iface) {
			pmlmepriv = &iface->mlmepriv;

			if (MLME_IS_AP(iface) || MLME_IS_MESH(iface)) {
				RTW_INFO(FUNC_ADPT_FMT" %s\n", FUNC_ADPT_ARG(iface), MLME_IS_AP(iface) ? "AP" : "MESH");
				rtw_iface_bcmc_sec_cam_map_restore(iface);
			}
		}
	}
}
#endif /*#if defined(DBG_CONFIG_ERROR_RESET) && defined(CONFIG_CONCURRENT_MODE)*/

u8 rtw_mi_buddy_sreset_adapter_hdl(_adapter *padapter, u8 bstart)
{
	u8 in_data = bstart;

	return _rtw_mi_process(padapter, _TRUE, &in_data, _rtw_mi_sreset_adapter_hdl);
}
static u8 _rtw_mi_tx_beacon_hdl(_adapter *adapter, void *data)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	if ((MLME_IS_AP(adapter) || MLME_IS_MESH(adapter))
		&& check_fwstate(&adapter->mlmepriv, WIFI_ASOC_STATE) == _TRUE
	) {
		adapter_link->mlmepriv.update_bcn = _TRUE;
#ifndef CONFIG_INTERRUPT_BASED_TXBCN
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI) || defined(CONFIG_PCI_BCN_POLLING)
		tx_beacon_hdl(adapter, NULL);
#endif
#endif
	}
	return _TRUE;
}
u8 rtw_mi_tx_beacon_hdl(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_tx_beacon_hdl);
}
u8 rtw_mi_buddy_tx_beacon_hdl(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_sreset_adapter_hdl);
}

static u8 _rtw_mi_set_tx_beacon_cmd(_adapter *adapter, void *data)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	struct link_mlme_priv *pmlmepriv = &adapter_link->mlmepriv;

	if (MLME_IS_AP(adapter) || MLME_IS_MESH(adapter)) {
		if (pmlmepriv->update_bcn == _TRUE)
			set_tx_beacon_cmd(adapter, adapter_link, 0);
	}
	return _TRUE;
}
u8 rtw_mi_set_tx_beacon_cmd(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_set_tx_beacon_cmd);
}
u8 rtw_mi_buddy_set_tx_beacon_cmd(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_set_tx_beacon_cmd);
}

#ifdef CONFIG_P2P
static u8 _rtw_mi_stay_in_p2p_mode(_adapter *adapter, void *data)
{
	struct wifidirect_info *pwdinfo = &(adapter->wdinfo);

	if (rtw_p2p_chk_role(pwdinfo, P2P_ROLE_CLIENT) ||
	    rtw_p2p_chk_role(pwdinfo, P2P_ROLE_GO))
		return _TRUE;
	return _FALSE;
}
u8 rtw_mi_stay_in_p2p_mode(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _FALSE, NULL, _rtw_mi_stay_in_p2p_mode);
}
u8 rtw_mi_buddy_stay_in_p2p_mode(_adapter *padapter)
{
	return _rtw_mi_process(padapter, _TRUE, NULL, _rtw_mi_stay_in_p2p_mode);
}
#endif /*CONFIG_P2P*/

_adapter *rtw_get_iface_by_id(_adapter *padapter, u8 iface_id)
{
	_adapter *iface = NULL;
	struct dvobj_priv *dvobj;

	if ((padapter == NULL) || (iface_id >= CONFIG_IFACE_NUMBER)) {
		rtw_warn_on(1);
		return iface;
	}
	dvobj = adapter_to_dvobj(padapter);

	dvobj = adapter_to_dvobj(padapter);
	return dvobj->padapters[iface_id];
}

_adapter *rtw_get_iface_by_macddr(_adapter *padapter, const u8 *mac_addr)
{
	int i;
	_adapter *iface = NULL;
	u8 bmatch = _FALSE;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
	struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);
	#endif
	u8 *iface_mac_addr;

#ifdef RTW_PHL_TEST_FPGA
	return dvobj->padapters[0];
#endif

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface)
			continue;

#if defined(CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI) || defined(CONFIG_RTW_SCAN_RAND)
		if (pwdev_priv->random_mac_enabled) {
			iface_mac_addr = adapter_pno_mac_addr(iface);
		} else
#endif /* CONFIG_RTW_SCAN_RAND */
			iface_mac_addr = adapter_mac_addr(iface);

		if (rtw_match_self_addr(iface, mac_addr))
		{
			bmatch = _TRUE;
			break;
		}
	}
	if (bmatch)
		return iface;
	else
		return NULL;
}


/*#define CONFIG_SKB_ALLOCATED*/
#define DBG_SKB_PROCESS
#ifdef DBG_SKB_PROCESS
void rtw_dbg_skb_process(_adapter *padapter, union recv_frame *precvframe, union recv_frame *pcloneframe)
{
	struct sk_buff *pkt_copy, *pkt_org;

	pkt_org = precvframe->u.hdr.pkt;
	pkt_copy = pcloneframe->u.hdr.pkt;
	/*
		RTW_INFO("%s ===== ORG SKB =====\n", __func__);
		RTW_INFO(" SKB head(%p)\n", pkt_org->head);
		RTW_INFO(" SKB data(%p)\n", pkt_org->data);
		RTW_INFO(" SKB tail(%p)\n", pkt_org->tail);
		RTW_INFO(" SKB end(%p)\n", pkt_org->end);

		RTW_INFO(" recv frame head(%p)\n", precvframe->u.hdr.rx_head);
		RTW_INFO(" recv frame data(%p)\n", precvframe->u.hdr.rx_data);
		RTW_INFO(" recv frame tail(%p)\n", precvframe->u.hdr.rx_tail);
		RTW_INFO(" recv frame end(%p)\n", precvframe->u.hdr.rx_end);

		RTW_INFO("%s ===== COPY SKB =====\n", __func__);
		RTW_INFO(" SKB head(%p)\n", pkt_copy->head);
		RTW_INFO(" SKB data(%p)\n", pkt_copy->data);
		RTW_INFO(" SKB tail(%p)\n", pkt_copy->tail);
		RTW_INFO(" SKB end(%p)\n", pkt_copy->end);

		RTW_INFO(" recv frame head(%p)\n", pcloneframe->u.hdr.rx_head);
		RTW_INFO(" recv frame data(%p)\n", pcloneframe->u.hdr.rx_data);
		RTW_INFO(" recv frame tail(%p)\n", pcloneframe->u.hdr.rx_tail);
		RTW_INFO(" recv frame end(%p)\n", pcloneframe->u.hdr.rx_end);
	*/
	/*
		RTW_INFO("%s => recv_frame adapter(%p,%p)\n", __func__, precvframe->u.hdr.adapter, pcloneframe->u.hdr.adapter);
		RTW_INFO("%s => recv_frame dev(%p,%p)\n", __func__, pkt_org->dev , pkt_copy->dev);
		RTW_INFO("%s => recv_frame len(%d,%d)\n", __func__, precvframe->u.hdr.len, pcloneframe->u.hdr.len);
	*/
	if (precvframe->u.hdr.len != pcloneframe->u.hdr.len)
		RTW_INFO("%s [WARN]  recv_frame length(%d:%d) compare failed\n", __func__, precvframe->u.hdr.len, pcloneframe->u.hdr.len);

	if (_rtw_memcmp(&precvframe->u.hdr.attrib, &pcloneframe->u.hdr.attrib, sizeof(struct rx_pkt_attrib)) == _FALSE)
		RTW_INFO("%s [WARN]  recv_frame attrib compare failed\n", __func__);

	if (_rtw_memcmp(precvframe->u.hdr.rx_data, pcloneframe->u.hdr.rx_data, precvframe->u.hdr.len) == _FALSE)
		RTW_INFO("%s [WARN]  recv_frame rx_data compare failed\n", __func__);

}
#endif

static s32 _rtw_mi_buddy_clone_bcmc_packet(_adapter *adapter,
		union recv_frame *precvframe, union recv_frame *pcloneframe)
{
	s32 ret = _SUCCESS;
#ifdef CONFIG_SKB_ALLOCATED
	u8 *pbuf = precvframe->u.hdr.rx_data;
#endif
	struct rx_pkt_attrib *pattrib = NULL;

	if (pcloneframe) {
		pcloneframe->u.hdr.adapter = adapter;
		/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
		pcloneframe->u.hdr.adapter_link = GET_PRIMARY_LINK(adapter);

		_rtw_init_listhead(&pcloneframe->u.hdr.list);
		pcloneframe->u.hdr.precvbuf = NULL;	/*can't access the precvbuf for new arch.*/
		pcloneframe->u.hdr.len = 0;

		_rtw_memcpy(&pcloneframe->u.hdr.attrib, &precvframe->u.hdr.attrib, sizeof(struct rx_pkt_attrib));

		pattrib = &pcloneframe->u.hdr.attrib;
#ifdef CONFIG_SKB_ALLOCATED
		if (rtw_os_alloc_recvframe(adapter, pcloneframe, pbuf, NULL) == _SUCCESS)
#else
		if (rtw_os_recvframe_duplicate_skb(adapter, pcloneframe, precvframe->u.hdr.pkt) == _SUCCESS)
#endif
		{
#ifdef CONFIG_SKB_ALLOCATED
			recvframe_put(pcloneframe, pattrib->pkt_len);
#endif

#ifdef DBG_SKB_PROCESS
			rtw_dbg_skb_process(adapter, precvframe, pcloneframe);
#endif
			validate_recv_frame(adapter, pcloneframe);

		} else {
			ret = _FAIL;
			RTW_ERR("%s()-%d: rtw_os_alloc_recvframe() failed!\n", __func__, __LINE__);
		}
	}
	return ret;
}


void rtw_mi_buddy_clone_bcmc_packet(_adapter *padapter,
					union recv_frame *precvframe)
{
	int i;
	s32 ret = _SUCCESS;
	_adapter *iface = NULL;
	union recv_frame *pcloneframe = NULL;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct recv_priv *precvpriv = &dvobj->recvpriv;
	_queue *pfree_recv_queue = &precvpriv->free_recv_queue;
	u8 *fhead = get_recvframe_data(precvframe);
	u8 type = GetFrameType(fhead);

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface || iface == padapter)
			continue;
		if (rtw_is_adapter_up(iface) == _FALSE || iface->registered == 0)
			continue;
		#if 0
		if (!rtw_iface_at_same_hwband(padapter, iface))
			continue;
		#endif

		if (type == WIFI_DATA_TYPE && !adapter_allow_bmc_data_rx(iface))
			continue;

		pcloneframe = rtw_alloc_recvframe(pfree_recv_queue);
		if (pcloneframe) {
			ret = _rtw_mi_buddy_clone_bcmc_packet(iface, precvframe, pcloneframe);
			if (ret == _FAIL)
				RTW_ERR("_rtw_mi_buddy_clone_bcmc_packet failed!\n");

			rtw_free_recvframe(pcloneframe);
		} else {
			RTW_ERR("%s rtw_alloc_recvframe failed\n", __func__);
			rtw_warn_on(1);
		}
	}
}

u8 rtw_mi_get_ifbmp_by_hwband(struct dvobj_priv *dvobj, u8 band_idx)
{
	int i;
	_adapter *iface;
	u8 ifbmp = 0;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface)
			continue;
		if (!rtw_iface_is_operate_at_hwband(iface, band_idx))
			continue;
		ifbmp |= BIT(i);
	}

	return ifbmp;
}

_adapter *rtw_mi_get_iface_by_hwband(struct dvobj_priv *dvobj, u8 band_idx)
{
	u8 ifbmp = rtw_mi_get_ifbmp_by_hwband(dvobj, band_idx);

	if (ifbmp) {
		int i;

		for (i = 0; i < dvobj->iface_nums; i++) {
			if ((ifbmp & BIT(i)) && dvobj->padapters[i])
				return dvobj->padapters[i];
		}
	}
	return NULL;
}

static u8 _rtw_mi_get_ld_sta_ifbmp_by_hwband(struct dvobj_priv *dvobj, _adapter *adapter, u8 band_idx)
{
	int i;
	_adapter *iface;
	u8 ifbmp = 0;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface)
			continue;
		if (adapter) {
			if (!rtw_iface_at_same_hwband(adapter, iface))
				continue;
		} else if (band_idx < HW_BAND_MAX){
			if (!rtw_iface_is_operate_at_hwband(iface, band_idx))
				continue;
		}

		if (MLME_IS_STA(iface) && MLME_IS_ASOC(iface))
			ifbmp |= BIT(i);
	}

	return ifbmp;
}

u8 rtw_mi_get_ld_sta_ifbmp(_adapter *adapter)
{
	return _rtw_mi_get_ld_sta_ifbmp_by_hwband(adapter_to_dvobj(adapter), adapter, HW_BAND_MAX);
}

u8 rtw_mi_get_ld_sta_ifbmp_by_hwband(struct dvobj_priv *dvobj, u8 band_idx)
{
	return _rtw_mi_get_ld_sta_ifbmp_by_hwband(dvobj, NULL, band_idx);
}

static u8 _rtw_mi_get_ap_mesh_ifbmp_by_hwband(struct dvobj_priv *dvobj, _adapter *adapter, u8 band_idx)
{
	int i;
	_adapter *iface;
	u8 ifbmp = 0;

	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (!iface)
			continue;
		if (adapter) {
			if (!rtw_iface_at_same_hwband(adapter, iface))
				continue;
		} else if (band_idx < HW_BAND_MAX){
			if (!rtw_iface_is_operate_at_hwband(iface, band_idx))
				continue;
		}

		if (CHK_MLME_STATE(iface, WIFI_AP_STATE | WIFI_MESH_STATE)
			&& MLME_IS_ASOC(iface))
			ifbmp |= BIT(i);
	}

	return ifbmp;
}

u8 rtw_mi_get_ap_mesh_ifbmp(_adapter *adapter)
{
	return _rtw_mi_get_ap_mesh_ifbmp_by_hwband(adapter_to_dvobj(adapter), adapter, HW_BAND_MAX);
}

u8 rtw_mi_get_ap_mesh_ifbmp_by_hwband(struct dvobj_priv *dvobj, u8 band_idx)
{
	return _rtw_mi_get_ap_mesh_ifbmp_by_hwband(dvobj, NULL, band_idx);
}

_adapter *rtw_mi_get_ap_mesh_iface_by_hwband(struct dvobj_priv *dvobj, u8 band_idx)
{
	u8 ifbmp = rtw_mi_get_ap_mesh_ifbmp_by_hwband(dvobj, band_idx);

	if (ifbmp) {
		int i;

		for (i = 0; i < dvobj->iface_nums; i++) {
			if ((ifbmp & BIT(i)) && dvobj->padapters[i])
				return dvobj->padapters[i];
		}
	}
	return NULL;
}

bool rtw_iface_is_operate_at_hwband(_adapter *adapter, u8 band_idx)
{
	struct _ADAPTER_LINK *alink;
	u8 lidx;

	if (rtw_is_adapter_up(adapter)) {
		for (lidx = 0; lidx < adapter->adapter_link_num; lidx++) {
			alink = GET_LINK(adapter, lidx);
			if (alink->wrlink->hw_band == band_idx)
				return true;
		}
	}

	return false;
}

bool rtw_iface_at_same_hwband(_adapter *adapter, _adapter *iface)
{
	bool rst = _TRUE;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	struct _ADAPTER_LINK *iface_link = GET_PRIMARY_LINK(iface);

	#ifdef CONFIG_DBCC_SUPPORT
	if (rtw_is_adapter_up(adapter) && rtw_is_adapter_up(iface))
		rst = (adapter_link->wrlink->hw_band == iface_link->wrlink->hw_band) ? _TRUE : _FALSE;
	#endif
	return rst;
}

u8 rtw_mi_get_hw_port(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	if (rtw_is_adapter_up(adapter))
		return adapter_link->wrlink->hw_port;
	return 0xFF;
}
