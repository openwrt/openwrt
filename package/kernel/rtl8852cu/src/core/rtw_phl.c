/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
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
#define _RTW_PHL_C_
#include <drv_types.h>


/***************** export API to osdep/core*****************/

static const char *const _band_cap_str[] = {
	/* BIT0 */"2G",
	/* BIT1 */"5G",
	/* BIT2 */"6G",	
};

static const char *const _bw_cap_str[] = {
	/* BIT0 */"20M",
	/* BIT1 */"40M",
	/* BIT2 */"80M",
	/* BIT3 */"160M",
	/* BIT4 */"80_80M",
	/* BIT5 */"5M",
	/* BIT6 */"10M",
};

static const char *const _proto_cap_str[] = {
	/* BIT0 */"b",
	/* BIT1 */"g",
	/* BIT2 */"n",
	/* BIT3 */"ac",
};

static const char *const _wl_func_str[] = {
	/* BIT0 */"P2P",
	/* BIT1 */"MIRACAST",
	/* BIT2 */"TDLS",
	/* BIT3 */"FTM",
};

static const char *const hw_cap_str = "[HW-CAP]";
void rtw_hw_dump_hal_spec(void *sel, struct dvobj_priv *dvobj)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(dvobj);
	_adapter *padapter = dvobj_get_primary_adapter(dvobj);
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	int i;

	RTW_PRINT_SEL(sel, "%s ic_name:%s\n", hw_cap_str, hal_spec->ic_name);
	RTW_PRINT_SEL(sel, "%s macid_num:%u\n", hw_cap_str, hal_spec->macid_num);
	RTW_PRINT_SEL(sel, "%s sec_cap:0x%02x\n", hw_cap_str, hal_spec->sec_cap);
	RTW_PRINT_SEL(sel, "%s sec_cam_ent_num:%u\n", hw_cap_str, hal_spec->sec_cam_ent_num);

	RTW_PRINT_SEL(sel, "%s rfpath_num_2g:%u\n", hw_cap_str, hal_spec->rfpath_num_2g);
	RTW_PRINT_SEL(sel, "%s rfpath_num_5g:%u\n", hw_cap_str, hal_spec->rfpath_num_5g);
	RTW_PRINT_SEL(sel, "%s rf_reg_path_num:%u\n", hw_cap_str, hal_spec->rf_reg_path_num);
	RTW_PRINT_SEL(sel, "%s max_tx_cnt:%u\n", hw_cap_str, hal_spec->max_tx_cnt);
	if (padapter_link && padapter_link->wrlink) {
		RTW_PRINT_SEL(sel, "%s tx_nss_num:%u\n", hw_cap_str, get_phy_tx_nss(padapter, padapter_link));
		RTW_PRINT_SEL(sel, "%s rx_nss_num:%u\n", hw_cap_str, get_phy_rx_nss(padapter, padapter_link));
	}

	RTW_PRINT_SEL(sel, "%s band_cap:", hw_cap_str);
	for (i = 0; i < BAND_CAP_BIT_NUM; i++) {
		if (((dvobj->phl_com->dev_cap.band_sup) >> i) & BIT0 && _band_cap_str[i])
			_RTW_PRINT_SEL(sel, "%s ", _band_cap_str[i]);
	}
	_RTW_PRINT_SEL(sel, "\n");

	RTW_PRINT_SEL(sel, "%s bw_cap:", hw_cap_str);
	for (i = 0; i < BW_CAP_BIT_NUM; i++) {
		if (((dvobj->phl_com->dev_cap.bw_sup) >> i) & BIT0 && _bw_cap_str[i])
			_RTW_PRINT_SEL(sel, "%s ", _bw_cap_str[i]);
	}
	_RTW_PRINT_SEL(sel, "\n");

	RTW_PRINT_SEL(sel, "%s proto_cap:", hw_cap_str);
	for (i = 0; i < PROTO_CAP_BIT_NUM; i++) {
		if (((hal_spec->proto_cap) >> i) & BIT0 && _proto_cap_str[i])
			_RTW_PRINT_SEL(sel, "%s ", _proto_cap_str[i]);
	}
	_RTW_PRINT_SEL(sel, "\n");

#if 0 /*GEORGIA_TODO_FIXIT*/
	RTW_PRINT_SEL(sel, "%s txgi_max:%u\n", hw_cap_str, hal_spec->txgi_max);
	RTW_PRINT_SEL(sel, "%s txgi_pdbm:%u\n", hw_cap_str, hal_spec->txgi_pdbm);
#endif
	RTW_PRINT_SEL(sel, "%s wl_func:", hw_cap_str);
	for (i = 0; i < WL_FUNC_BIT_NUM; i++) {
		if (((hal_spec->wl_func) >> i) & BIT0 && _wl_func_str[i])
			_RTW_PRINT_SEL(sel, "%s ", _wl_func_str[i]);
	}
	_RTW_PRINT_SEL(sel, "\n");
	
#if 0 /*GEORGIA_TODO_FIXIT*/

	RTW_PRINT_SEL(sel, "%s pg_txpwr_saddr:0x%X\n", hw_cap_str, hal_spec->pg_txpwr_saddr);
	RTW_PRINT_SEL(sel, "%s pg_txgi_diff_factor:%u\n", hw_cap_str, hal_spec->pg_txgi_diff_factor);
#endif
}

void rtw_dump_phl_sta_info(void *sel, struct sta_info *sta)
{	
	struct rtw_phl_stainfo_t *phl_sta = sta->phl_sta;

	RTW_PRINT_SEL(sel, "[PHL STA]- role-idx: %d\n", phl_sta->wrole->id);

	RTW_PRINT_SEL(sel, "[PHL STA]- mac_addr:"MAC_FMT"\n", MAC_ARG(phl_sta->mac_addr));
	RTW_PRINT_SEL(sel, "[PHL STA]- aid: %d\n", phl_sta->aid);
	RTW_PRINT_SEL(sel, "[PHL STA]- macid: %d\n", phl_sta->macid);

	RTW_PRINT_SEL(sel, "[PHL STA]- wifi_band: %d\n", phl_sta->chandef.band);
	RTW_PRINT_SEL(sel, "[PHL STA]- bw: %d\n", phl_sta->chandef.bw);
	RTW_PRINT_SEL(sel, "[PHL STA]- chan: %d\n", phl_sta->chandef.chan);
	RTW_PRINT_SEL(sel, "[PHL STA]- offset: %d\n", phl_sta->chandef.offset);
}

inline bool rtw_hw_chk_band_cap(struct dvobj_priv *dvobj, u8 cap)
{
	return dvobj->phl_com->dev_cap.band_sup & cap;
}

inline bool rtw_hw_chk_bw_cap(struct dvobj_priv *dvobj, u8 cap)
{
	return dvobj->phl_com->dev_cap.bw_sup & cap;
}

inline bool rtw_hw_chk_proto_cap(struct dvobj_priv *dvobj, u8 cap)
{
	return GET_HAL_SPEC(dvobj)->proto_cap & cap;
}

inline bool rtw_hw_chk_wl_func(struct dvobj_priv *dvobj, u8 func)
{
	return GET_HAL_SPEC(dvobj)->wl_func & func;
}

inline bool rtw_hw_is_band_support(struct dvobj_priv *dvobj, u8 band)
{
	return (dvobj->phl_com->dev_cap.band_sup & band_to_band_cap(band));
}

inline bool rtw_hw_is_bw_support(struct dvobj_priv *dvobj, u8 bw)
{
	return (dvobj->phl_com->dev_cap.bw_sup & ch_width_to_bw_cap(bw));
}

inline bool rtw_hw_is_wireless_mode_support(struct dvobj_priv *dvobj, u8 mode)
{
	u8 proto_cap = GET_HAL_SPEC(dvobj)->proto_cap;

	if (mode == WLAN_MD_11B)
		if ((proto_cap & PROTO_CAP_11B) && rtw_hw_chk_band_cap(dvobj, BAND_CAP_2G))
			return 1;

	if (mode == WLAN_MD_11G)
		if ((proto_cap & PROTO_CAP_11G) && rtw_hw_chk_band_cap(dvobj, BAND_CAP_2G))
			return 1;

	if (mode == WLAN_MD_11A)
		if ((proto_cap & PROTO_CAP_11G) && rtw_hw_chk_band_cap(dvobj, BAND_CAP_5G))
			return 1;

	#ifdef CONFIG_80211N_HT
	if (mode == WLAN_MD_11N)
		if (proto_cap & PROTO_CAP_11N)
			return 1;
	#endif

	#ifdef CONFIG_80211AC_VHT
	if (mode == WLAN_MD_11AC)
		if ((proto_cap & PROTO_CAP_11AC) && rtw_hw_chk_band_cap(dvobj, BAND_CAP_5G))
			return 1;
	#endif

	#ifdef CONFIG_80211AX_HE
	if (mode == WLAN_MD_11AX)
		if (proto_cap & PROTO_CAP_11AX)
			return 1;
	#endif
	return 0;
}


inline u8 rtw_hw_get_wireless_mode(struct dvobj_priv *dvobj)
{
	u8 proto_cap = GET_HAL_SPEC(dvobj)->proto_cap;
	u8 wireless_mode = 0;

	if(proto_cap & PROTO_CAP_11B)
		wireless_mode |= WLAN_MD_11B;

	if(proto_cap & PROTO_CAP_11G)
		wireless_mode |= WLAN_MD_11G;

	if(rtw_hw_chk_band_cap(dvobj, BAND_CAP_5G))
		wireless_mode |= WLAN_MD_11A;

	#ifdef CONFIG_80211N_HT
	if(proto_cap & PROTO_CAP_11N)
		wireless_mode |= WLAN_MD_11N;
	#endif

	#ifdef CONFIG_80211AC_VHT
	if(proto_cap & PROTO_CAP_11AC) 
		wireless_mode |= WLAN_MD_11AC;
	#endif

	#ifdef CONFIG_80211AX_HE
	if(proto_cap & PROTO_CAP_11AX) {
			wireless_mode |= WLAN_MD_11AX;
	}
	#endif
	
	return wireless_mode;
}

inline u8 rtw_hw_get_band_cap(struct dvobj_priv *dvobj)
{
	return dvobj->phl_com->dev_cap.band_sup;
}

inline bool rtw_hw_is_mimo_support(_adapter *adapter)
{
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);

	if ((get_phy_tx_nss(adapter, adapter_link) == 1) &&
		(get_phy_rx_nss(adapter, adapter_link) == 1))
		return 0;
	return 1;
}

/*
* rtw_hw_largest_bw - starting from in_bw, get largest bw supported by HAL
* @adapter:
* @in_bw: starting bw, value of enum channel_width
*
* Returns: value of enum channel_width
*/
u8 rtw_hw_largest_bw(struct dvobj_priv *dvobj, u8 in_bw)
{
	for (; in_bw > CHANNEL_WIDTH_20; in_bw--) {
		if (rtw_hw_is_bw_support(dvobj, in_bw))
			break;
	}

	if (!rtw_hw_is_bw_support(dvobj, in_bw))
		rtw_warn_on(1);

	return in_bw;
}

u8 rtw_hw_get_mac_addr(struct dvobj_priv *dvobj, u8 *hw_mac_addr)
{
	if (rtw_phl_get_mac_addr_efuse(dvobj->phl, hw_mac_addr) != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s failed\n", __func__);
		return _FAIL;
	}
	return _SUCCESS;
}

u8 rtw_core_deregister_phl_msg(struct dvobj_priv *dvobj)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

	psts = rtw_phl_msg_hub_deregister_recver(dvobj->phl, MSG_RECV_CORE);
	if(psts	== RTW_PHL_STATUS_FAILURE) {
		RTW_ERR("%s failed\n", __func__);
		return _FAIL;
	}
	return _SUCCESS;
}

void rtw_hw_deinit(struct dvobj_priv *dvobj)
{
	if (dvobj->phl) {
		rtw_phl_trx_free(dvobj->phl);
		rtw_core_deregister_phl_msg(dvobj);
		rtw_phl_watchdog_deinit(dvobj->phl);
		rtw_phl_deinit(dvobj->phl);
	}

	#ifdef DBG_PHL_MEM_ALLOC
	RTW_INFO("[PHL-MEM] %s PHL memory :%d\n", __func__,
					ATOMIC_READ(&(dvobj->phl_mem)));
	#endif
}


#if 0 /*GEORGIA_TODO_FIXIT*/
void rtw_hw_intf_cfg(struct dvobj_priv *dvobj, struct hal_com_t *hal_com)
{
	struct hci_info_st hci_info;

	#ifdef CONFIG_PCI_HCI
	if (dvobj->interface_type == RTW_HCI_PCIE) {
		PPCI_DATA pci = dvobj_to_pci(dvobj);
		//hci_info.
	}
	#endif

	#ifdef CONFIG_USB_HCI
	if (dvobj->interface_type == RTW_HCI_USB) {
		PUSB_DATA usb = dvobj_to_usb(dvobj);
		#if 0
		u8 usb_speed; /* 1.1, 2.0 or 3.0 */
		u16 usb_bulkout_size;
		u8 nr_endpoint; /*MAX_ENDPOINT_NUM*/

		/* Bulk In , Out Pipe information */
		int RtInPipe[MAX_BULKIN_NUM];
		u8 RtNumInPipes;
		int RtOutPipe[MAX_BULKOUT_NUM];
		u8 RtNumOutPipes;
		#endif
		//hci_info
	}
	#endif

	#ifdef CONFIG_SDIO_HCI
	if (dvobj->interface_type == RTW_HCI_SDIO) {
		struct sdio_data *sdio = dvobj_to_sdio(dvobj);

		hci_info.clock = sdio->clock;
		hci_info.timing = sdio->timing;
		hci_info.sd3_bus_mode = sdio->sd3_bus_mode;
		hci_info.block_sz = sdio->block_transfer_len;
		hci_info.align_sz = sdio->block_transfer_len;
	}
	#endif

	rtw_hal_intf_config(hal_com, &hci_info);
}
#endif

static void _hw_ic_info_cfg(struct dvobj_priv *dvobj, struct rtw_ic_info *ic_info)
{
	_rtw_memset(ic_info, 0,sizeof(struct rtw_ic_info));

	ic_info->ic_id = dvobj->ic_id;
	ic_info->hci_type = dvobj->interface_type;

	#ifdef CONFIG_PCI_HCI
	if (dvobj->interface_type == RTW_HCI_PCIE) {
		PPCI_DATA pci = dvobj_to_pci(dvobj);

	}
	#endif

	#ifdef CONFIG_USB_HCI
	if (dvobj->interface_type == RTW_HCI_USB) {
		PUSB_DATA usb = dvobj_to_usb(dvobj);

		ic_info->usb_info.usb_speed = usb->usb_speed;
		ic_info->usb_info.usb_bulkout_size = usb->usb_bulkout_size;
		ic_info->usb_info.inep_num = usb->RtNumInPipes;
		ic_info->usb_info.outep_num = usb->RtNumOutPipes;
	}
	#endif

	#ifdef CONFIG_SDIO_HCI
	if (dvobj->interface_type == RTW_HCI_SDIO) {
		struct sdio_data *sdio = dvobj_to_sdio(dvobj);

		ic_info->sdio_info.clock = sdio->clock;
		ic_info->sdio_info.timing = sdio->timing;
		ic_info->sdio_info.sd3_bus_mode = sdio->sd3_bus_mode;
		ic_info->sdio_info.io_align_sz = 4;
		ic_info->sdio_info.block_sz = sdio->block_transfer_len;
		ic_info->sdio_info.tx_align_sz = sdio->block_transfer_len;
		ic_info->sdio_info.tx_512_by_byte_mode =
				(sdio->max_byte_size >= 512) ? true : false;
	}
	#endif
}

u8 rtw_backup_and_get_final_ss(_adapter *adapter, struct sta_info *sta, u8 chg_ss)
{
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);
	u8 final_ss = 0;

	if (chg_ss) {
		final_ss = chg_ss;
		rtw_ctrl_and_backup_assoc_cap_rx_nss(adapter, sta, final_ss);
	} else {
		sta->phl_sta->asoc_cap.nss_rx = pmlmeext->txss_bk;
		rtw_phl_sta_assoc_cap_process(sta->phl_sta, _FALSE);
		final_ss = sta->phl_sta->asoc_cap.nss_rx;
	}
	//RTW_INFO("%s, adapter=0x%08x, sta=0x%08x\n", __func__, adapter, sta);
	RTW_INFO("%s, final_ss=%d, txss_bk=%d, nss_rx=%d", __func__, final_ss, pmlmeext->txss_bk, sta->phl_sta->asoc_cap.nss_rx);
	return final_ss;
}

void rtw_ctrl_and_backup_assoc_cap_rx_nss(_adapter *adapter, struct sta_info *sta, u8 rx_nss)
{
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);


	pmlmeext->txss_bk = sta->phl_sta->asoc_cap.nss_rx;
	rtw_phl_sta_assoc_cap_process(sta->phl_sta, _TRUE);

	if (rx_nss == 1) {
		sta->phl_sta->asoc_cap.nss_rx = 1;
		sta->phl_sta->asoc_cap.stbc_ht_rx = 0;
		sta->phl_sta->asoc_cap.stbc_vht_rx = 0;
		sta->phl_sta->asoc_cap.stbc_he_rx = 0;
	} else {
		sta->phl_sta->asoc_cap.nss_rx = rx_nss;
	}
}

#ifdef CONFIG_DBCC_SUPPORT
static void _dbcc_proto_go(_adapter *adapter, u8 dbcc_en)
{
	struct sta_priv *stapriv = NULL;
	struct sta_info *sta = NULL;
	int i, stainfo_offset;
	_list	*plist, *phead;
	u8 chk_num = 0;
	u8 chk_list[NUM_STA];
	struct _ADAPTER_LINK *a_link = GET_PRIMARY_LINK(adapter);
	struct link_mlme_priv *mlmepriv = &(a_link->mlmepriv);
	struct link_mlme_ext_priv *mlmeext = &(a_link->mlmeextpriv);
	struct link_mlme_ext_info *mlmeinfo = &(mlmeext->mlmext_info);
	WLAN_BSSID_EX *network = &(mlmeinfo->network);
	struct HT_caps_element *ht_caps;
	u8 *p, *ie = network->IEs;
	u32 len = 0;

	stapriv = &adapter->stapriv;

	if (!MLME_IS_GO(adapter))
		return;

	if (!mlmepriv->htpriv.ht_option)
			return;

	if (!mlmeinfo->HT_caps_enable)
			return;

	_rtw_spinlock_bh(&stapriv->asoc_list_lock);

	phead = &stapriv->asoc_list;
	plist = get_next(phead);

	while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
		sta = LIST_CONTAINOR(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		stainfo_offset = rtw_stainfo_offset(stapriv, sta);
		if (stainfo_offset_valid(stainfo_offset))
			chk_list[chk_num++] = stainfo_offset;
		continue;
	}

	_rtw_spinunlock_bh(&stapriv->asoc_list_lock);


	/* issue SMPS */
	if (chk_num) {
		for (i = 0; i < chk_num; i++) {
			sta = rtw_get_stainfo_by_offset(stapriv, chk_list[i]);
			if (dbcc_en)
				rtw_ssmps_enter(adapter, sta);
			else
				rtw_ssmps_leave(adapter, sta);
		}
	}

	/* update BCN */
	p = rtw_get_ie(ie + _BEACON_IE_OFFSET_, _HT_CAPABILITY_IE_, &len, (network->IELength - _BEACON_IE_OFFSET_));
	if (p && len > 0) {
		ht_caps = (struct HT_caps_element *)(p + 2);
		RTW_INFO("[DBCC] orig ht caps info = 0x%02x\n", ht_caps->u.HT_cap_element.HT_caps_info);

		if (dbcc_en)
			SET_HT_CAP_ELE_SM_PS(&(ht_caps->u.HT_cap_element.HT_caps_info), SM_PS_STATIC);
		else
			SET_HT_CAP_ELE_SM_PS(&(ht_caps->u.HT_cap_element.HT_caps_info), SM_PS_DISABLE);

		RTW_INFO("[DBCC] modify ht caps info = 0x%02x\n", ht_caps->u.HT_cap_element.HT_caps_info);
		rtw_update_beacon(adapter, a_link, _HT_CAPABILITY_IE_, NULL, _TRUE, RTW_CMDF_DIRECTLY);
	}
}

static u8 _rtw_dbcc_proto_hdl(void *drv_priv, u8 wr_idx, void *data)
{
	struct dvobj_priv *dvobj = (struct dvobj_priv *)drv_priv;
	_adapter *adapter;
	struct mlme_priv *pmlmepriv = NULL;
	struct sta_priv *pstapriv = NULL;
	struct sta_info *psta = NULL;
	struct dbcc_proto_msg *dbcc_proto = NULL;
	struct _ADAPTER_LINK *alink = NULL;
	u8 rst = _FAIL;
	u8 final_ss = 0;

	adapter = dvobj->padapters[wr_idx];
	if (!rtw_is_adapter_up(adapter)) {
		RTW_ERR("if%d mac_addr : "MAC_FMT" not active\n", wr_idx, MAC_ARG(adapter_mac_addr(adapter)));
		goto _exit;
	}

	if (!rtw_hw_is_mimo_support(adapter)) {
		RTW_ERR("if%d mac_addr : "MAC_FMT" not support mimo\n", wr_idx, MAC_ARG(adapter_mac_addr(adapter)));
		goto _exit;
	}

	alink = GET_PRIMARY_LINK(adapter);
	pmlmepriv = &adapter->mlmepriv;
	dbcc_proto = (struct dbcc_proto_msg *)data;

	if (dbcc_proto->dbcc_en) {
		if (dbcc_proto->wr->id == wr_idx)
			goto _exit;
	}

	if (is_client_associated_to_ap(adapter)) {
	/*if ((MLME_IS_STA(adapter) || MLME_IS_GC(adapter)) && MLME_IS_ASOC(adapter)) {*/
		RTW_INFO(ADPT_FMT" associated to AP\n", ADPT_ARG(adapter));
		/*if (!pmlmeext->ssmps_en)*/
		pstapriv = &adapter->stapriv;
		psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
		if (psta == NULL) {
			RTW_ERR(ADPT_FMT" sta == NULL\n", ADPT_ARG(adapter));
			rtw_warn_on(1);
			goto _exit;
		}
		/* N mode - smps*/

		if (psta->phl_sta->wmode & WLAN_MD_11AX) {
			if (dbcc_proto->dbcc_en)
				final_ss = rtw_backup_and_get_final_ss(adapter, psta, 1);
			else
				final_ss = rtw_backup_and_get_final_ss(adapter, psta, 0);
			rtw_he_om_ctrl_trx_ss(adapter, alink, psta, final_ss, _TRUE);
		} else if (psta->phl_sta->wmode & WLAN_MD_11AC) {
			if (dbcc_proto->dbcc_en)
				final_ss = rtw_backup_and_get_final_ss(adapter, psta, 1);
			else
				final_ss = rtw_backup_and_get_final_ss(adapter, psta, 0);
			rtw_vht_op_mode_ctrl_rx_nss(adapter, alink, psta, final_ss, _TRUE);
		} else if (psta->phl_sta->wmode & WLAN_MD_11N) {
			if (dbcc_proto->dbcc_en)
				rtw_ssmps_enter(adapter, psta);
			else
				rtw_ssmps_leave(adapter, psta);
		}
	}
	else if (MLME_IS_AP(adapter) || MLME_IS_GO(adapter) || MLME_IS_ADHOC_MASTER(adapter)) {
		RTW_INFO(ADPT_FMT" is AP\n", ADPT_ARG(adapter));
		if (MLME_IS_GO(adapter)) {
			_dbcc_proto_go(adapter, dbcc_proto->dbcc_en);
		}
		else if (MLME_IS_AP(adapter)) {
			/* Rx cap chang - Protocol*/

			/* re-send beacon*/
			if (dbcc_proto->dbcc_en == _FALSE) {
				RTW_INFO(ADPT_FMT" send_beacon\n", ADPT_ARG(adapter));
				if(send_beacon(adapter) == _FAIL)
					RTW_ERR(ADPT_FMT" send_beacon, fail!\n", ADPT_ARG(adapter));
			}
		}
		else {
			RTW_INFO(ADPT_FMT" not support dbcc yet\n", ADPT_ARG(adapter));
		}
	}
	rst = _SUCCESS;

_exit:
	return rst;
}

u8 core_dbcc_protocol_hdl(void *drv, enum phl_band_idx band_idx,
					struct dbcc_proto_msg *dbcc_proto)
{
	struct dvobj_priv *dvobj = (struct dvobj_priv *)drv;
	struct rtw_wifi_role_t *wrole = NULL;
	_adapter *adapter = NULL;

	wrole = dbcc_proto->wr;
	if (wrole == NULL) {
		rtw_warn_on(1);
		RTW_ERR("%s role == NULL\n", __func__);
		return _FAIL;
	}
	adapter = dvobj->padapters[wrole->id];


	if (dbcc_proto->dbcc_en) {
		RTW_INFO(FUNC_ADPT_FMT " DBCC enable ......\n",
				FUNC_ADPT_ARG(adapter));
		/* loop adapter
		* 1. change Rx cap
		* issue_smps ,issue_action_omi , issue_action_om - issue(wait_ack)
		* monitor rx-rate if change to 1ss (wait_ack)
		* 2. change Tx cap
		* Reregister RA to 1x1 - general mdl
		*/
		rtw_phl_mr_process_by_band(dvobj->phl, HW_BAND_0, dbcc_proto,
							_rtw_dbcc_proto_hdl);
		rtw_phl_mr_dbcc_enable(dvobj->phl, band_idx, wrole);
	}
	else { /*dbcc disable*/
		RTW_INFO(FUNC_ADPT_FMT " DBCC disable ......\n",
				FUNC_ADPT_ARG(adapter));
		/* loop adapter
		* 1. change Rx cap
		* issue_smps ,issue_action_omi , issue_action_om - issue(wait_ack)
		* monitor rx-rate if change to 2ss (wait_ack)
		* 2. change Tx cap
		* Reregister RA to 2x2 - general mdl
		*/
		rtw_phl_mr_process_by_band(dvobj->phl, HW_BAND_0, dbcc_proto,
							_rtw_dbcc_proto_hdl);
		rtw_phl_mr_dbcc_disable(dvobj->phl, band_idx, wrole);
	}
	return _SUCCESS;
}
#endif /*CONFIG_DBCC_SUPPORT*/

#ifdef CONFIG_DFS_MASTER
static void phl_radar_detect_msg_hdl(struct dvobj_priv *dvobj, struct phl_msg *msg)
{
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);

	if (rfctl->radar_detect_enabled) {
		/* TODO: PHL info for specific band and range */
		enum phl_band_idx band_idx = HW_BAND_0;
		u8 radar_cch = 0;
		enum channel_width radar_bw = CHANNEL_WIDTH_MAX;

		band_idx = rfctl->radar_detect_hwband;
		radar_cch = rfctl->radar_detect_cch[band_idx];
		radar_bw = rfctl->radar_detect_bw[band_idx];
		rtw_dfs_rd_hdl(dvobj, band_idx, radar_cch, radar_bw);
	}
}
#endif /* CONFIG_DFS_MASTER */

void core_handler_phl_msg(void *drv_priv, struct phl_msg *msg)
{
	struct dvobj_priv *dvobj = (struct dvobj_priv *)drv_priv;
	u8 mdl_id = MSG_MDL_ID_FIELD(msg->msg_id);
	u16 evt_id = MSG_EVT_ID_FIELD(msg->msg_id);

	switch(mdl_id) {
	case PHL_MDL_RX:
	{
		_adapter *iface;
		u8 i = 0;

		if (evt_id == MSG_EVT_BCN_RESEND) {
			for (i = 0; i < dvobj->iface_nums; i++) {
				iface = dvobj->padapters[i];
				if(!rtw_is_adapter_up(iface))
					continue;
				/*
				if (!rtw_iface_at_same_hwband(padapter, iface))
					continue;
				*/
				if(MLME_IS_MESH(iface)
					|| MLME_IS_AP(iface)
					|| MLME_IS_ADHOC_MASTER(iface)) {
					if (send_beacon(iface) == _FAIL)
						RTW_ERR(ADPT_FMT" issue_beacon, fail!\n",
									ADPT_ARG(iface));
				}
			}
		}
		#ifdef CONFIG_XMIT_ACK_BY_CCX_RPT
		else if (evt_id == MSG_EVT_CCX_REPORT_TX_OK || evt_id == MSG_EVT_CCX_REPORT_TX_FAIL) {
			struct rtw_tx_pkt_rpt *tx_rpt = NULL;
			struct xmit_priv *pxmitpriv;

			tx_rpt = (struct rtw_tx_pkt_rpt *)msg->inbuf;
			if (tx_rpt == NULL) {
				RTW_ERR("%s get tx_rpt failed\n", __func__);
				rtw_warn_on(1);
				break;
			}

			iface = dvobj_get_primary_adapter(dvobj);
			pxmitpriv = &iface->xmitpriv;

			if ((pxmitpriv->ack_tx_seq_no & 0x0F) != tx_rpt->seq_no) {
				RTW_ERR("%s seq_no-(%d)%d:%d missmatch\n", __func__,
					pxmitpriv->ack_tx_seq_no,
					(pxmitpriv->ack_tx_seq_no & 0x0F),
					tx_rpt->seq_no);
				RTW_INFO("[XMIT_ACK] ack_tx_seq_no:%d\n", pxmitpriv->ack_tx_seq_no);
				RTW_INFO("[XMIT_ACK] mac_id:%d seq_no:%d\n", tx_rpt->macid, tx_rpt->seq_no);
			}

			if (evt_id == MSG_EVT_CCX_REPORT_TX_OK)
				rtw_ack_tx_done(&iface->xmitpriv, RTW_SCTX_DONE_SUCCESS);
			else if (evt_id == MSG_EVT_CCX_REPORT_TX_FAIL)
				rtw_ack_tx_done(&iface->xmitpriv, RTW_SCTX_DONE_CCX_PKT_FAIL);
		}
		#endif
		#ifdef CONFIG_DFS_MASTER
		else if (evt_id == MSG_EVT_DFS_RD_IS_DETECTING)
			phl_radar_detect_msg_hdl(dvobj, msg);
		#endif
	}
	break;
	case PHL_MDL_SER:
	{
		if (evt_id == MSG_EVT_SER_L2)
			RTW_ERR("RECV PHL MSG_EVT_SER_L2\n");
	}
	break;
	case PHL_MDL_WOW:
	{
		/*if (evt_id == MSG_EVT_WOW_WAKE_RSN)
			RTW_INFO("[wow] wake rsn : %d\n", msg->inbuf[0]);*/
	}
	break;
	case PHL_MDL_MRC:
	{

	}
	break;
	default:
		RTW_ERR("%s mdl_id :%d not support\n", __func__, mdl_id);
		break;
	}
}

u8 rtw_core_register_phl_msg(struct dvobj_priv *dvobj)
{
	struct phl_msg_receiver ctx = {0};
	u8 imr[] = {PHL_MDL_RX, PHL_MDL_SER, PHL_MDL_WOW, PHL_MDL_MRC};
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

	ctx.incoming_evt_notify = core_handler_phl_msg;
	ctx.priv = (void*)dvobj;

	psts = rtw_phl_msg_hub_register_recver(dvobj->phl, &ctx, MSG_RECV_CORE);
	if(psts	== RTW_PHL_STATUS_FAILURE) {
		RTW_ERR("phl_msg_hub_register failed\n");
		return _FAIL;
	}

	psts = rtw_phl_msg_hub_update_recver_mask(dvobj->phl,
					MSG_RECV_CORE, imr, sizeof(imr), false);
	if(psts	== RTW_PHL_STATUS_FAILURE) {
		RTW_ERR("phl_msg_hub_update_recver_mask failed\n");
		return _FAIL;
	}
	return _SUCCESS;
}

/*RTW_WKARD_CORE_RSSI_V1*/
s8 rtw_phl_rssi_to_dbm(u8 rssi)
{
	return rssi - PHL_MAX_RSSI;
}


#ifdef CONFIG_MCC_MODE
u8 rtw_hw_mcc_chk_inprogress(struct _ADAPTER *a, struct _ADAPTER_LINK *adapter_link)
{
	struct dvobj_priv *d;
	void *phl;
	u8 ret = _FALSE;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		goto exit;

	ret = rtw_phl_mr_coex_query_inprogress(phl, adapter_link->wrlink->hw_band, RTW_MR_COEX_CHK_INPROGRESS);

exit:
	return ret;
}

#ifdef CONFIG_P2P_PS
static int _mcc_update_noa(void *priv, struct rtw_phl_mcc_noa *param)
{
	struct dvobj_priv *dvobj = (struct dvobj_priv *) priv;
	struct rtw_wifi_role_t *wrole = NULL;
	struct _ADAPTER *a = NULL;
	struct wifidirect_info *wdinfo;
	u8 id = 0;
	u8 ret = _SUCCESS;
	u8 noa_en = 0;
#ifdef CONFIG_PHL_P2PPS
	struct rtw_phl_noa_desc desc= {0};
#endif
	struct _ADAPTER_LINK *alink;

	wrole = param->wrole;
	if (wrole == NULL) {
		RTW_ERR("%s wrole is NULL\n", __func__);
		ret = _FAIL;
		goto exit;
	}

	id = wrole->id;
	if (id >= CONFIG_IFACE_NUMBER) {
		RTW_ERR("%s error id (%d)\n", __func__, id);
		ret = _FAIL;
		goto exit;
	}

	a = dvobj->padapters[id];
	if (a == NULL) {
		RTW_ERR("%s adapter(%d) is NULL\n", __func__, id);
		ret = _FAIL;
		goto exit;
	}

	/* by pass non-GO case */
	if (!MLME_IS_GO(a))
		goto exit;

	wdinfo = &a->wdinfo;
	RTW_INFO(FUNC_ADPT_FMT":(%d)\n", FUNC_ADPT_ARG(a), id);
	RTW_INFO("start_t_h=0x%02x,start_t_l=0x%02x\n", param->start_t_h, param->start_t_l);
	RTW_INFO("dur=0x%d,cnt=0x%d,interval=0x%d\n", param->dur, param->cnt, param->interval);
	/* cnt will be 0, if disable NoA */
	if (param->cnt == 0)
		noa_en = _FALSE;
	else
		noa_en = _TRUE;

#ifdef CONFIG_PHL_P2PPS
	/* enable TSF32 toggle */
	desc.tag = P2PPS_TRIG_MCC;
	if (noa_en)
		desc.enable = _TRUE;
	else
		desc.enable = _FALSE;
	desc.duration = param->dur * NET80211_TU_TO_US;
	desc.interval = param->interval * NET80211_TU_TO_US;
	desc.start_t_h = param->start_t_h;
	desc.start_t_l = param->start_t_l;
	desc.count = param->cnt;
	desc.w_role = param->wrole;
	desc.rlink = param->rlink;
	if (rtw_phl_p2pps_noa_update(dvobj->phl, &desc) != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s rtw_phl_p2pps_noa_update fail\n", __func__);
		ret = _FAIL;
		goto exit;
	}
#endif

	/* update NoA IE */
	wdinfo->noa_index = wdinfo->noa_index + 1;
	if (noa_en)
		wdinfo->noa_num = 1;
	else
		wdinfo->noa_num = 0;
	wdinfo->noa_count[0] = param->cnt;
	wdinfo->noa_duration[0] =param->dur * NET80211_TU_TO_US;
	wdinfo->noa_interval[0] = param->interval * NET80211_TU_TO_US;
	wdinfo->noa_start_time[0] = param->start_t_l;

	alink = GET_PRIMARY_LINK(a);
	rtw_update_beacon(a, alink, _VENDOR_SPECIFIC_IE_, P2P_OUI, _TRUE, RTW_CMDF_DIRECTLY);
exit:
	return ret;
}
#endif

/* default setting  */
static int _mcc_get_setting(void *priv, struct rtw_phl_mcc_setting_info *param)
{
	struct dvobj_priv *dvobj = (struct dvobj_priv *) priv;
	struct rtw_wifi_role_t *wrole = NULL;
	struct _ADAPTER *a = NULL;
	struct wifidirect_info *wdinfo;
	u8 id = 0;
	u8 ret = _SUCCESS;

	wrole = param->wrole;
	if (wrole == NULL) {
		RTW_ERR("%s wrole is NULL\n", __func__);
		ret = _FAIL;
		goto exit;
	}

	id = wrole->id;
	if (id >= CONFIG_IFACE_NUMBER) {
		RTW_ERR("%s error id (%d)\n", __func__, id);
		ret = _FAIL;
		goto exit;
	}

	a = dvobj->padapters[id];
	if (a == NULL) {
		RTW_ERR("%s adapter(%d) is NULL\n", __func__, id);
		ret = _FAIL;
		goto exit;
	}

	if (MLME_IS_GO(a) || MLME_IS_GC(a))
		param->dur = 50;
	else
		param->dur = 50;

	if (MLME_IS_STA(a) || MLME_IS_GC(a))
		param->tx_null_early = 5;
	else
		param->tx_null_early = NONSPECIFIC_SETTING;

	RTW_INFO("%s: adapter(%d) dur=%d, tx_null_early=%d\n", __func__, id, param->dur, param->tx_null_early);

exit:
	return ret;
}

struct rtw_phl_mcc_ops rtw_mcc_ops = {
	.priv = NULL,
	.mcc_update_noa = _mcc_update_noa,
	.mcc_get_setting = _mcc_get_setting,
};
#endif

struct rtw_phl_mr_ops rtw_mr_ops = {
/*
#ifdef CONFIG_PHL_P2PPS
	.phl_mr_update_noa = &dummy_p2p_noa;
#endif
*/
#ifdef CONFIG_MCC_MODE
	.mcc_ops = &rtw_mcc_ops,
#endif

#ifdef CONFIG_DBCC_SUPPORT
	.dbcc_protocol_hdl = core_dbcc_protocol_hdl,
#endif
};

void rtw_core_register_mr_config(struct dvobj_priv *dvobj)
{
#ifdef CONFIG_MCC_MODE
	rtw_mr_ops.mcc_ops->priv = (void *)dvobj;
#endif
	rtw_phl_mr_ops_init(dvobj->phl, &rtw_mr_ops);
}

#ifdef CONFIG_ECSA_PHL
static void rtw_core_set_ecsa_ops(struct dvobj_priv *d)
{
	struct rtw_phl_ecsa_ops ops = {0};

	ops.priv = (void *)d;
	ops.update_beacon = rtw_ecsa_update_beacon;
	ops.update_chan_info = rtw_ecsa_mr_update_chan_info_by_role;
	ops.check_ecsa_allow = rtw_ap_check_ecsa_allow;
	ops.ecsa_complete = rtw_ecsa_complete;
	ops.check_tx_resume_allow = rtw_ecsa_check_tx_resume_allow;
	rtw_phl_ecsa_init_ops(GET_PHL_INFO(d), &ops);
}
#endif

static void rtw_core_tx_power_tbl_loaded(void *drv_priv, bool target_loaded, bool limit_loaded)
{
	struct dvobj_priv * dvobj = drv_priv;

	RTW_INFO("%s target=%d limit=%d\n", __func__, target_loaded, limit_loaded);

	if (target_loaded)
		;
	if (limit_loaded)
		rtw_txpwr_update_cur_lmt_regs(dvobj, true);
}

static void rtw_core_set_phl_ops(struct dvobj_priv *dvobj)
{
	struct rtw_phl_evt_ops *evt_ops;

	evt_ops = &(dvobj->phl_com->evt_ops);

	evt_ops->rx_process = rtw_core_rx_process;
	evt_ops->tx_recycle = rtw_core_tx_recycle;
#ifdef CONFIG_WOWLAN
	evt_ops->indicate_wake_rsn = rtw_core_wow_handle_wake_up_rsn;
#ifdef CONFIG_GTK_OL
	evt_ops->wow_handle_sec_info_update = rtw_update_gtk_ofld_info;
#endif
#endif /* CONFIG_WOWLAN */
	#ifdef CONFIG_RTW_IPS
	evt_ops->set_rf_state = rtw_core_set_ips_state;
	#endif
	evt_ops->issue_null_data = rtw_core_issu_null_data;
	#ifdef CONFIG_PCI_HCI
	evt_ops->os_query_local_buf = rtw_os_query_local_buf;
	evt_ops->os_return_local_buf = rtw_os_return_local_buf;
	#endif

	evt_ops->tx_power_tbl_loaded = rtw_core_tx_power_tbl_loaded;

	#if 0 /*TBD*/
	rtw_phl_snd_init_ops_send_ndpa(dvobj->phl,
				       rtw_core_beamform_send_ndpa);
	evt_ops->ap_ps_sta_ps_change = rtw_core_ap_sta_ps_chg;
	#endif
}

u8 rtw_hw_init(struct dvobj_priv *dvobj)
{
	u8 rst = _FAIL;
	enum rtw_phl_status phl_status;
	struct rtw_ic_info ic_info;

#ifdef DBG_PHL_MEM_ALLOC
	ATOMIC_SET(&dvobj->phl_mem, 0);
#endif

	_hw_ic_info_cfg(dvobj, &ic_info);
	phl_status = rtw_phl_init(dvobj, &(dvobj->phl), &ic_info);

	if ((phl_status != RTW_PHL_STATUS_SUCCESS) || (dvobj->phl == NULL)) {
		RTW_ERR("%s - rtw_phl_init failed status(%d), dvobj->phl(%p)\n",
			__func__, phl_status, dvobj->phl);
		goto _free_hal;
	}

	dvobj->phl_com = rtw_phl_get_com(dvobj->phl);

	/*init sw cap from registary*/
	rtw_core_update_default_setting(dvobj);

	/* sw & hw cap*/
	rtw_phl_cap_pre_config(dvobj->phl);

	#ifdef CONFIG_RX_PSTS_PER_PKT
	rtw_phl_init_ppdu_sts_para(dvobj->phl_com,
		_TRUE, _FALSE,
		RTW_PHL_PSTS_FLTR_MGNT | RTW_PHL_PSTS_FLTR_DATA /*| RTW_PHL_PSTS_FLTR_CTRL*/
		);
	#endif
	/*init datapath section*/
	rtw_phl_trx_alloc(dvobj->phl);
	rtw_core_set_phl_ops(dvobj);

	rtw_core_register_phl_msg(dvobj);

	/* load wifi feature or capability from efuse*/
	phl_status = rtw_phl_preload(dvobj->phl);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s - rtw_phl_preload from efuse failed\n", __func__);
		goto _free_hal;
	}

	rtw_phl_final_cap_decision(dvobj->phl);

	/* after final cap decision */
	rtw_core_register_mr_config(dvobj);
#ifdef CONFIG_P2P_PS
	rtw_core_register_p2pps_ops(dvobj);
#endif
	#ifdef CONFIG_ECSA_PHL
	rtw_core_set_ecsa_ops(dvobj);
	#endif

	rtw_dump_rfe_type(dvobj);

	rtw_phl_watchdog_init(dvobj->phl,
				0,
				rtw_core_watchdog_sw_hdlr,
				rtw_core_watchdog_hw_hdlr,
				rtw_core_watchdog_sw_post_hdlr);

	rst = _SUCCESS;
	return rst;

_free_hal :
	rtw_hw_deinit(dvobj);
	return rst;
}

u8 rtw_hw_start(struct dvobj_priv *dvobj)
{
	if (dev_is_hw_start(dvobj))
		return _FAIL;

	if (rtw_phl_start(GET_PHL_INFO(dvobj)) != RTW_PHL_STATUS_SUCCESS)
		return _FAIL;

	#ifdef CONFIG_PCI_HCI
	//intr init flag
	dvobj_to_pci(dvobj)->irq_enabled = 1;
#ifdef RTW_WKARD_RESET_INT
	rtw_phl_disable_interrupt(dvobj->phl);
	rtw_phl_enable_interrupt(dvobj->phl);
#endif
	#endif
	#ifdef CONFIG_CMD_GENERAL
	rtw_phl_watchdog_start(dvobj->phl);
	#endif

	dev_set_hw_start(dvobj);

	return _SUCCESS;
}
void rtw_hw_stop(struct dvobj_priv *dvobj)
{
	if (!dev_is_hw_start(dvobj))
		return;

	#ifdef CONFIG_CMD_GENERAL
	rtw_phl_watchdog_stop(dvobj->phl);
	#endif
	rtw_phl_stop(GET_PHL_INFO(dvobj));

	#ifdef CONFIG_PCI_HCI
	//intr init flag
	dvobj_to_pci(dvobj)->irq_enabled = 0;
	#endif

	dev_clr_hw_start(dvobj);
}

bool rtw_hw_get_init_completed(struct dvobj_priv *dvobj)
{
	return rtw_phl_is_init_completed(GET_PHL_INFO(dvobj));
}

bool rtw_hw_is_init_completed(struct dvobj_priv *dvobj)
{
	return (rtw_phl_is_init_completed(GET_PHL_INFO(dvobj))) ? _TRUE : _FALSE;
}

#define NSS_VALID(nss) (nss > 0)
void rtw_hw_cap_init(struct dvobj_priv *dvobj)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(dvobj);
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);
	struct phy_cap_t *phy_cap = phl_com->phy_cap;
	struct registry_priv  *regpriv =
		&(dvobj_get_primary_adapter(dvobj)->registrypriv);

#ifdef DIRTY_FOR_WORK
	dvobj->phl_com->rf_path_num = hal_spec->rf_reg_path_num; /*GET_HAL_RFPATH_NUM*/
	dvobj->phl_com->rf_type = RF_2T2R; /*GET_HAL_RFPATH*/

	dvobj->cam_ctl.sec_cap = hal_spec->sec_cap;
	dvobj->cam_ctl.num = rtw_min(hal_spec->sec_cam_ent_num, SEC_CAM_ENT_NUM_SW_LIMIT);
#endif
}


/*
 * _ch_offset_drv2phl() - Convert driver channel offset to PHL type
 * @ch_offset:	channel offset, ref: HAL_PRIME_CHNL_OFFSET_*
 *
 * Return PHL channel offset type "enum chan_offset"
 */
static enum chan_offset _ch_offset_drv2phl(u8 ch_offset)
{
	if (ch_offset == CHAN_OFFSET_UPPER)
		return CHAN_OFFSET_UPPER;
	if (ch_offset == CHAN_OFFSET_LOWER)
		return CHAN_OFFSET_LOWER;

	return CHAN_OFFSET_NO_EXT;
}

/*
 * rtw_hw_set_ch_bw() - Set channel, bandwidth and channel offset
 * @a:		pointer of struct _ADAPTER
 * @band:	wifi band
 * @ch:		channel
 * @bw:		bandwidth
 * @offset:	channel offset, ref: HAL_PRIME_CHNL_OFFSET_*
 *
 * Set channel, bandwidth and channel offset.
 *
 * Return 0 for success, otherwise fail
 */
int rtw_hw_set_ch_bw(struct _ADAPTER *a, struct _ADAPTER_LINK *alink,
			enum band_type band, u8 ch, enum channel_width bw, u8 offset, u8 do_rfk)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct dvobj_priv *dvobj = adapter_to_dvobj(a);
	int err = 0;
	struct rtw_chan_def chdef = {0};
	enum phl_cmd_type cmd_type = PHL_CMD_DIRECTLY;
	u32 cmd_timeout = 0;

#ifdef CONFIG_MCC_MODE
	if (rtw_hw_mcc_chk_inprogress(a, alink)) {
		RTW_WARN("under mcc, skip ch setting\n");
		return err;
	}
#endif

	chdef.band = band;
	chdef.chan = ch;
	chdef.bw = bw;
	chdef.offset = offset;

	_rtw_mutex_lock_interruptible(&dvobj->setch_mutex);
#ifdef DBG_CONFIG_CMD_DISP
	if (a->cmd_type == 0xFF) {
		cmd_type = PHL_CMD_DIRECTLY;
		cmd_timeout = 0;
	}
	else {
		cmd_type = a->cmd_type;
		cmd_timeout = a->cmd_timeout;
	}
#endif
	status = rtw_phl_cmd_set_ch_bw(a->phl_role, alink->wrlink,
					&chdef, do_rfk, cmd_type, cmd_timeout);

	if (status == RTW_PHL_STATUS_SUCCESS) {
		if (a->bNotifyChannelChange)
			RTW_INFO("[%s] band = %d, ch = %d, offset = %d, bwmode = %d, success\n",
				__FUNCTION__, band, ch, offset, bw);

	} else {
		err = -1;
		RTW_ERR("%s: set band(%u) ch(%u) bw(%u) offset(%u) FAIL!\n",
			__func__, band, ch, bw, offset);
	}

	_rtw_mutex_unlock(&dvobj->setch_mutex);

	return err;
}

void rtw_hw_update_chan_def(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	struct link_mlme_ext_priv *mlmeext = &(adapter_link->mlmeextpriv);
	struct rtw_wifi_role_link_t *rlink = adapter_link->wrlink;
	struct rtw_phl_stainfo_t *phl_sta_self = NULL;

	/*update chan_def*/
	rlink->chandef.band = mlmeext->chandef.band;
	rlink->chandef.chan = mlmeext->chandef.chan;
	rlink->chandef.bw = mlmeext->chandef.bw;
	rlink->chandef.offset = mlmeext->chandef.offset;
	rlink->chandef.center_ch = rtw_phl_get_center_ch(&mlmeext->chandef);
	/* ToDo: 80+80 BW & 160 BW */

	phl_sta_self = rtw_phl_get_stainfo_self(adapter_to_dvobj(adapter)->phl, rlink);
	_rtw_memcpy(&phl_sta_self->chandef, &rlink->chandef, sizeof(struct rtw_chan_def));
}

static void _dump_phl_role_info(struct rtw_wifi_role_t *wrole)
{
	u8 lidx;
	RTW_INFO("[WROLE]- role-idx: %d\n", wrole->id);

	RTW_INFO("[WROLE]- type: %d\n", wrole->type);
	RTW_INFO("[WROLE]- mstate: %d\n", wrole->mstate);
	RTW_INFO("[WROLE]- mac_addr:"MAC_FMT"\n", MAC_ARG(wrole->mac_addr));
	for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
		RTW_INFO("- [WRLINK]- link-idx: %d\n", wrole->rlink[lidx].id);
		RTW_INFO("- [WRLINK]- hw_band: %d\n", wrole->rlink[lidx].hw_band);
		RTW_INFO("- [WRLINK]- hw_port: %d\n", wrole->rlink[lidx].hw_port);
		RTW_INFO("- [WRLINK]- hw_wmm: %d\n", wrole->rlink[lidx].hw_wmm);
		RTW_INFO("- [WRLINK]- band: %d\n", wrole->rlink[lidx].chandef.band);
		RTW_INFO("- [WRLINK]- chan: %d\n", wrole->rlink[lidx].chandef.chan);
		RTW_INFO("- [WRLINK]- bw: %d\n", wrole->rlink[lidx].chandef.bw);
		RTW_INFO("- [WRLINK]- offset: %d\n",wrole->rlink[lidx].chandef.offset);
	}
	// Freddie ToDo: MBSSID
}

void rtw_collect_adapter_link_mac_addr(_adapter *adapter, u8 **mac)
{
	struct _ADAPTER_LINK *adapter_link;
	u8 lidx;

	for (lidx = 0; lidx < adapter->adapter_link_num; lidx++) {
		adapter_link = GET_LINK(adapter, lidx);
		mac[lidx] = adapter_link->mac_addr;
	}
}

u8 rtw_hw_iface_init(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	u8 phl_role_idx = INVALID_WIFI_ROLE_IDX;
	u8 rst = _FAIL;
	int chctx_num = 0;
	struct _ADAPTER_LINK *adapter_link = NULL;
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 *adapter_link_mac_addr[RTW_RLINK_MAX] = {NULL};
	int lidx;
#if defined(CONFIG_RTW_IPS) || defined(CONFIG_RTW_LPS)
	bool ps_allow = _FALSE;

	rtw_phl_ps_set_rt_cap(GET_PHL_INFO(dvobj), HW_BAND_0, ps_allow, PS_RT_CORE_INIT);
#endif
	// Freddie ToDo: For AP mode, net type should be set to net device already.

	rtw_collect_adapter_link_mac_addr(adapter, adapter_link_mac_addr);


	/* will allocate phl self sta info */
	phl_role_idx = rtw_phl_wifi_role_alloc(GET_PHL_INFO(dvobj),
			adapter_mac_addr(adapter),
			adapter_link_mac_addr,
			PHL_RTYPE_STATION,
			adapter->iface_id, &(adapter->phl_role),
			DTYPE,
			_FALSE);

	if ((phl_role_idx == INVALID_WIFI_ROLE_IDX) ||
		(adapter->phl_role == NULL)) {
		RTW_ERR("rtw_phl_wifi_role_alloc failed\n");
		rtw_warn_on(1);
		goto _error;
	}
	for (lidx = 0; lidx < adapter->adapter_link_num; lidx++) {
		adapter_link = GET_LINK(adapter, lidx);
		rlink = &(adapter->phl_role->rlink[lidx]);
		if (rlink == NULL) {
			RTW_ERR("%s rlink == NULL\n", __func__);
			goto _error;
		}
		adapter_link->wrlink = rlink;
	}

	/*init default value*/
	#ifdef DBG_CONFIG_CMD_DISP
	adapter->cmd_type = 0xFF;
	adapter->cmd_timeout = 0;
	#endif
	for (lidx = 0; lidx < adapter->adapter_link_num; lidx++) {
		adapter_link = GET_LINK(adapter, lidx);
		rtw_hw_update_chan_def(adapter, adapter_link);
		chctx_num = rtw_phl_mr_get_chanctx_num(GET_PHL_INFO(dvobj), adapter->phl_role, adapter_link->wrlink);
		if (chctx_num == 0) {
			if (rtw_phl_cmd_set_ch_bw(adapter->phl_role, adapter_link->wrlink,
							&(adapter_link->wrlink->chandef),
							_FALSE, PHL_CMD_WAIT, 0)
							!= RTW_PHL_STATUS_SUCCESS) {
				RTW_ERR("%s init ch failed\n", __func__);
			}
		}
	}

	_dump_phl_role_info(adapter->phl_role);

	/* init self staion info after wifi role alloc */
	rst = rtw_init_self_stainfo(adapter, PHL_CMD_WAIT);

#if defined (CONFIG_PCI_HCI) && defined (CONFIG_PCIE_TRX_MIT_FIX)
	rtw_pcie_trx_mit_cmd(adapter, 0, 0,
			     PCIE_RX_INT_MIT_TIMER, 0, 1);
#endif
#if defined(CONFIG_RTW_IPS) || defined(CONFIG_RTW_LPS)
	ps_allow = _TRUE;
	rtw_phl_ps_set_rt_cap(GET_PHL_INFO(dvobj), HW_BAND_0, ps_allow, PS_RT_CORE_INIT);
#endif

#ifdef CONFIG_HW_RTS
	/* Disable HW CTS2self */
	rtw_phl_hw_cts2self_cfg(GET_PHL_INFO(dvobj), HW_BAND_0, 0, 0, 0);
#endif

	return rst;

_error:
	return rst;
}

u8 rtw_hw_iface_type_change(_adapter *adapter, u8 iface_type)
{
	void *phl = GET_PHL_INFO(adapter_to_dvobj(adapter));
#ifdef CONFIG_WIFI_MONITOR
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(adapter_to_dvobj(adapter));
#endif
	struct rtw_wifi_role_t *wrole = adapter->phl_role;
	enum role_type rtype = PHL_RTYPE_NONE;
	enum rtw_phl_status status;
	struct sta_info *sta = NULL;
	struct _ADAPTER_LINK *adapter_link = NULL;
	struct rtw_phl_mld_t *mld = NULL;
	u8 lidx;

	if (wrole == NULL) {
		RTW_ERR("%s - wrole = NULL\n", __func__);
		rtw_warn_on(1);
		return _FAIL;
	}

	switch (iface_type) {
	case _HW_STATE_ADHOC_:
		rtype = PHL_RTYPE_ADHOC;
		break;
	case _HW_STATE_STATION_:
		rtype = PHL_RTYPE_STATION;
		break;
	case _HW_STATE_AP_:
		rtype = PHL_RTYPE_AP;
		break;
	case _HW_STATE_MONITOR_:
		rtype = PHL_RTYPE_MONITOR;
		break;
	case _HW_STATE_NOLINK_:
	default:
		/* TBD */
		break;
	}

	status = rtw_phl_cmd_wrole_change(phl, wrole, NULL,
				WR_CHG_TYPE, (u8*)&rtype, sizeof(enum role_type),
				PHL_CMD_DIRECTLY, 0);

	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s - change to phl role type = %d fail with error = %d\n",
			__func__, rtype, status);
		rtw_warn_on(1);
		return _FAIL;
	}

#ifdef CONFIG_WIFI_MONITOR
	if (rtype == PHL_RTYPE_MONITOR) {
		phl_com->append_fcs = false; /* This need to check again by yiwei*/
		rtw_phl_enter_mon_mode(phl, wrole);
	} else {
		phl_com->append_fcs = true; /* This need to check again by yiwei*/
		rtw_phl_leave_mon_mode(phl, wrole);
	}
#endif

	/* AP allocates self-station and changes broadcast-station before hostapd adds key */
	if (rtype == PHL_RTYPE_AP) {
		mld = rtw_phl_get_mld_by_addr(GET_PHL_INFO(adapter_to_dvobj(adapter)), wrole, adapter_mac_addr(adapter));
		if (mld == NULL) {
			mld = rtw_phl_alloc_mld(GET_PHL_INFO(adapter_to_dvobj(adapter)), wrole, adapter_mac_addr(adapter), DTYPE);
			if (mld == NULL) {
				RTW_ERR("%s - allocate AP self-mld failed\n", __func__);
				rtw_warn_on(1);
				return _FAIL;
			}
		}
		for (lidx = 0; lidx < adapter->adapter_link_num; lidx++) {
			adapter_link = GET_LINK(adapter, lidx);
			sta = rtw_get_stainfo(&adapter->stapriv, adapter_link->mac_addr);
			if (sta == NULL) {
				/* main_id is don't care for self sta */
				sta = rtw_alloc_stainfo(&adapter->stapriv, adapter_link->mac_addr, DTYPE, 0, lidx, PHL_CMD_DIRECTLY);
				if (sta == NULL) {
					RTW_ERR("%s - allocate AP self-stations failed\n", __func__);
					rtw_warn_on(1);
					return _FAIL;
				}
			}
		}
	}

	RTW_INFO("%s - change to type = %d success !\n", __func__, iface_type);

	return _SUCCESS;
}

void rtw_hw_iface_deinit(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
#if defined(CONFIG_RTW_IPS) || defined(CONFIG_RTW_LPS)
	bool ps_allow = _FALSE;

	rtw_phl_ps_set_rt_cap(GET_PHL_INFO(dvobj), HW_BAND_0, ps_allow, PS_RT_CORE_INIT);
#endif
	if (adapter->phl_role) {
		rtw_free_self_stainfo(adapter);
		rtw_phl_wifi_role_free(GET_PHL_INFO(dvobj), adapter->phl_role->id);
		adapter->phl_role = NULL;
	}
#if defined(CONFIG_RTW_IPS) || defined(CONFIG_RTW_LPS)
	ps_allow = _TRUE;
	rtw_phl_ps_set_rt_cap(GET_PHL_INFO(dvobj), HW_BAND_0, ps_allow, PS_RT_CORE_INIT);
#endif
}

/*
 * _sec_algo_drv2phl() - Convert security algorithm to PHL's definition
 * @drv_algo:		security algorithm
 * @phl_algo:		security algorithm for PHL, ref to enum rtw_enc_algo
 * @phl_key_len:	key length
 *
 * Convert driver's security algorithm defintion to PHL's type.
 *
 */
static void _sec_algo_drv2phl(enum security_type drv_algo,
			      u8 *algo, u8 *key_len)
{
	u8 phl_algo = RTW_ENC_NONE;
	u8 phl_key_len = 0;

	switch(drv_algo) {
	case _NO_PRIVACY_:
		phl_algo = RTW_ENC_NONE;
		phl_key_len = 0;
		break;
	case _WEP40_:
		phl_algo = RTW_ENC_WEP40;
		phl_key_len = 5;
		break;
	case _TKIP_:
	case _TKIP_WTMIC_:
		phl_algo = RTW_ENC_TKIP;
		phl_key_len = 16;
		break;
	case _AES_:
		phl_algo = RTW_ENC_CCMP;
		phl_key_len = 16;
		break;
	case _WEP104_:
		phl_algo = RTW_ENC_WEP104;
		phl_key_len = 13;
		break;
	case _SMS4_:
		phl_algo = RTW_ENC_WAPI;
		phl_key_len = 32;
		break;
	case _GCMP_:
		phl_algo = RTW_ENC_GCMP;
		phl_key_len = 16;
		break;
	case _CCMP_256_:
		phl_algo = RTW_ENC_CCMP256;
		phl_key_len = 32;
		break;
	case _GCMP_256_:
		phl_algo = RTW_ENC_GCMP256;
		phl_key_len = 32;
		break;
#ifdef CONFIG_IEEE80211W
	case _BIP_CMAC_128_:
		phl_algo = RTW_ENC_BIP_CCMP128;
		phl_key_len = 16;
		break;
#endif /* CONFIG_IEEE80211W */
	default:
		RTW_ERR("%s: No rule to covert drv algo(0x%x) to phl!!\n",
			__func__, drv_algo);
		phl_algo = RTW_ENC_MAX;
		phl_key_len = 0;
		break;
	}

	if(algo)
		*algo = phl_algo;
	if(key_len)
		*key_len = phl_key_len;
}

/*
 * _sec_algo_phl2drv() - Convert security algorithm to core layer definition
 * @drv_algo:		security algorithm for core layer, ref to enum security_type
 * @phl_algo:		security algorithm for PHL, ref to enum rtw_enc_algo
 * @drv_key_len:	key length
 *
 * Convert PHL's security algorithm defintion to core layer definition.
 *
 */
static void _sec_algo_phl2drv(enum rtw_enc_algo phl_algo,
			      u8 *algo, u8 *key_len)
{
	u8 drv_algo = RTW_ENC_NONE;
	u8 drv_key_len = 0;

	switch(phl_algo) {
	case RTW_ENC_NONE:
		drv_algo = _NO_PRIVACY_;
		drv_key_len = 0;
		break;
	case RTW_ENC_WEP40:
		drv_algo = _WEP40_;
		drv_key_len = 5;
		break;
	case RTW_ENC_TKIP:
		/* drv_algo = _TKIP_WTMIC_ */
		drv_algo = _TKIP_;
		drv_key_len = 16;
		break;
	case RTW_ENC_CCMP:
		drv_algo = _AES_;
		drv_key_len = 16;
		break;
	case RTW_ENC_WEP104:
		drv_algo = _WEP104_;
		drv_key_len = 13;
		break;
	case RTW_ENC_WAPI:
		drv_algo = _SMS4_;
		drv_key_len = 32;
		break;
	case RTW_ENC_GCMP:
		drv_algo = _GCMP_;
		drv_key_len = 16;
		break;
	case RTW_ENC_CCMP256:
		drv_algo = _CCMP_256_;
		drv_key_len = 32;
		break;
	case RTW_ENC_GCMP256:
		drv_algo = _GCMP_256_;
		drv_key_len = 32;
		break;
#ifdef CONFIG_IEEE80211W
	case RTW_ENC_BIP_CCMP128:
		drv_algo = _BIP_CMAC_128_;
		drv_key_len = 16;
		break;
#endif /* CONFIG_IEEE80211W */
	default:
		RTW_ERR("%s: No rule to covert phl algo(0x%x) to drv!!\n",
			__func__, phl_algo);
		drv_algo = _SEC_TYPE_MAX_;
		drv_key_len = 0;
		break;
	}

	if(algo)
		*algo = drv_algo;
	if(key_len)
		*key_len = drv_key_len;
}

u8 rtw_sec_algo_drv2phl(enum security_type drv_algo)
{
	u8 algo = 0;

	_sec_algo_drv2phl(drv_algo, &algo, NULL);
	return algo;
}

u8 rtw_sec_algo_phl2drv(enum rtw_enc_algo phl_algo)
{
	u8 algo = 0;

	_sec_algo_phl2drv(phl_algo, &algo, NULL);
	return algo;
}

static int rtw_hw_chk_sec_mode(struct _ADAPTER *a, struct sta_info *sta,
			enum phl_cmd_type cmd_type,  u32 cmd_timeout)
{
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	u8 sec_mode = 0;
	struct security_priv *psecuritypriv = &a->securitypriv;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);

	if (!phl)
		return _FAIL;

	sec_mode = rtw_phl_trans_sec_mode(
		rtw_sec_algo_drv2phl(psecuritypriv->dot11PrivacyAlgrthm),
		rtw_sec_algo_drv2phl(psecuritypriv->dot118021XGrpPrivacy));

	RTW_INFO("After phl trans_sec_mode = %d\n", sec_mode);

	if (sec_mode != sta->phl_sta->sec_mode) {
		RTW_INFO("%s: original sec_mode =%d update sec mode to %d.\n",
			__func__, sta->phl_sta->sec_mode, sec_mode);
		status = rtw_phl_cmd_change_stainfo(phl, sta->phl_sta, STA_CHG_SEC_MODE,
				&sec_mode, sizeof(u8), cmd_type, cmd_timeout);
	/* To Do: check the return status */
	} else {
		RTW_INFO("%s: sec mode remains the same. skip update.\n", __func__);
	}
	return _SUCCESS;
}

/*
 * rtw_hw_add_key() - Add security key
 * @a:		pointer of struct _ADAPTER
 * @sta:	pointer of struct sta_info
 * @keyid:	key index
 * @keyalgo:	key algorithm
 * @keytype:	0: unicast / 1: multicast / 2: bip (ref: enum SEC_CAM_KEY_TYPE)
 * @key:	key content
 * @spp:	spp mode
 *
 * Add security key.
 *
 * Return 0 for success, otherwise fail.
 */
int rtw_hw_add_key(struct _ADAPTER *a, struct sta_info *sta,
		u8 keyid, enum security_type keyalgo, u8 keytype, u8 *key,
		u8 spp, enum phl_cmd_type cmd_type,  u32 cmd_timeout)
{
	struct dvobj_priv *d;
	void *phl;
	struct phl_sec_param_h crypt = {0};
	enum rtw_phl_status status;


	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		return -1;

	if (rtw_hw_chk_sec_mode(a, sta, cmd_type, cmd_timeout) == _FAIL)
		return -1;

	crypt.keyid = keyid;
	crypt.key_type= keytype;
	crypt.spp = spp;
	_sec_algo_drv2phl(keyalgo, &crypt.enc_type, &crypt.key_len);

	/* delete key before adding key */
	rtw_phl_cmd_del_key(phl, sta->phl_sta, &crypt, cmd_type, cmd_timeout);
	status = rtw_phl_cmd_add_key(phl, sta->phl_sta, &crypt, key, cmd_type, cmd_timeout);
	if (status != RTW_PHL_STATUS_SUCCESS)
		return -1;

	return 0;
}

/*
 * rtw_hw_del_key() - Delete security key
 * @a:		pointer of struct _ADAPTER
 * @sta:	pointer of struct sta_info
 * @keyid:	key index
 * @keytype:	0: unicast / 1: multicast / 2: bip (ref: enum SEC_CAM_KEY_TYPE)
 *
 * Delete security key by macid, keyid and keytype.
 *
 * Return 0 for success, otherwise fail.
 */
int rtw_hw_del_key(struct _ADAPTER *a, struct sta_info *sta,
		u8 keyid, u8 keytype, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	struct dvobj_priv *d;
	void *phl;
	struct phl_sec_param_h crypt = {0};
	enum rtw_phl_status status;


	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		return -1;

	crypt.keyid = keyid;
	crypt.key_type= keytype;

	status = rtw_phl_cmd_del_key(phl, sta->phl_sta, &crypt, cmd_type, cmd_timeout);
	if (status != RTW_PHL_STATUS_SUCCESS)
		return -1;

	return 0;
}

/*
 * rtw_hw_del_all_key() - Delete all security key for this STA
 * @a:		pointer of struct _ADAPTER
 * @sta:	pointer of struct sta_info
 *
 * Delete all security keys belong to this STA.
 *
 * Return 0 for success, otherwise fail.
 */
int rtw_hw_del_all_key(struct _ADAPTER *a, struct sta_info *sta,
			enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	struct dvobj_priv *d;
	void *phl;
	u8 keyid;
	u8 keytype;
	struct phl_sec_param_h crypt = {0};
	enum rtw_phl_status status;


	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		return -1;

	/* Delete Group and Pairwise key */
	for (keytype = 0; keytype < 2; keytype++) {
		for (keyid = 0; keyid < 4; keyid++) {
			crypt.keyid = keyid;
			crypt.key_type = keytype;
			rtw_phl_cmd_del_key(phl, sta->phl_sta, &crypt, cmd_type, cmd_timeout);
		}
	}

	/* Delete BIP key */
	crypt.key_type = 2;
	for (keyid = 4; keyid <= BIP_MAX_KEYID; keyid++) {
		crypt.keyid = keyid;
		rtw_phl_cmd_del_key(phl, sta->phl_sta, &crypt, cmd_type, cmd_timeout);
	}

	return 0;
}
#ifndef CONFIG_AP_CMD_DISPR
int rtw_hw_start_bss_network(struct _ADAPTER *a)
{
	/* some hw related ap settings */
	if (rtw_phl_ap_started(adapter_to_dvobj(a)->phl, a->phl_role) !=
		RTW_PHL_STATUS_SUCCESS)
		return _FAIL;

	return _SUCCESS;
}
#endif

#if 0
/* connect */
int rtw_hw_prepare_connect(struct _ADAPTER *a, struct sta_info *sta, u8 *target_addr)
{
	/*adapter->phl_role.mac_addr*/
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;


	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);

	status = rtw_phl_connect_prepare(phl, a->phl_role, target_addr);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s: Fail to setup hardware for connecting!(%d)\n",
			__func__, status);
		return -1;
	}
	/* Todo: Enable TSF update */
	/* Todo: Set support short preamble or not by beacon capability */
	/* Todo: Set slot time */

	return 0;
}
#endif

int rtw_hw_connect_remove_sta(struct _ADAPTER *a, struct sta_info *sta)
{
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		return -1;

	rtw_hw_del_all_key(a, sta, PHL_CMD_DIRECTLY, 0);

	status = rtw_phl_cmd_update_media_status(phl, sta->phl_sta, NULL, false,
							PHL_CMD_DIRECTLY, 0);
	if (status != RTW_PHL_STATUS_SUCCESS)
		return -1;

	rtw_phl_chanctx_del(adapter_to_dvobj(a)->phl, a->phl_role, sta->padapter_link->wrlink, NULL);

	/* free connecting AP sta info */
	rtw_free_stainfo(a, sta);

	return 0;
}


/* Handle connect fail case */
int rtw_hw_connect_abort(struct _ADAPTER *a)
{
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	struct _ADAPTER_LINK *alink = NULL;
	struct sta_info *sta = NULL;
	u8 lidx;


	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		return -1;

	for (lidx = 0; lidx < a->adapter_link_num; lidx++) {
		alink = GET_LINK(a, lidx);
		if (!alink->mlmepriv.to_join)
			continue;
		sta = rtw_get_stainfo(&(a->stapriv), alink->mlmeextpriv.mlmext_info.network.MacAddress);
		if (!sta) {
			RTW_ERR(FUNC_ADPT_FMT ": drv sta_info(" MAC_FMT ") not exist!\n",
				FUNC_ADPT_ARG(a), MAC_ARG(sta->phl_sta->mac_addr));
			return -1;
		}
		rtw_hw_del_all_key(a, sta, PHL_CMD_DIRECTLY, 0);
		status = rtw_phl_cmd_update_media_status(phl, sta->phl_sta, NULL, false,
								PHL_CMD_DIRECTLY, 0);
		if (status != RTW_PHL_STATUS_SUCCESS)
			return -1;
	}

#ifndef CONFIG_STA_CMD_DISPR
	/*
	 * In CONFIG_STA_CMD_DISPR case, connect abort hw setting has been moved
	 * to MSG_EVT_DISCONNECT@PHL_FG_MDL_CONNECT .
	 */

	/* disconnect hw setting */
	rtw_phl_disconnect(phl, a->phl_role);

	/* delete sta channel ctx */
	for (lidx = 0; lidx < a->adapter_link_num; lidx++) {
		alink = GET_LINK(a, lidx);
		if (!alink->mlmepriv.to_join)
			continue;
		rtw_phl_chanctx_del(adapter_to_dvobj(a)->phl, a->phl_role, alink->wrlink, NULL);
	}
	/* restore orig union ch */
	rtw_join_done_chk_ch(a, -1);

	/* free connecting AP sta info */
	rtw_free_mld_stainfo(a, sta->phl_sta->mld);
	rtw_init_self_stainfo(a, PHL_CMD_DIRECTLY);
#endif /* !CONFIG_STA_CMD_DISPR */

	return 0;
}

#ifdef RTW_WKARD_UPDATE_PHL_ROLE_CAP
/**
 * rtw_update_phl_cap_by_rgstry() - Update cap & protocol_cap of phl_role
 * @a:		struct _ADAPTER*
 *
 * Update cap & protocol_cap of a->phl_role by registry/driver parameters.
 *
 */
void rtw_update_phl_cap_by_rgstry(struct _ADAPTER *a, struct _ADAPTER_LINK *alink)
{
	struct registry_priv *rgstry = &a->registrypriv;
	struct role_link_cap_t *cap = &alink->wrlink->cap;
	struct protocol_cap_t *prtcl = &alink->wrlink->protocol_cap;

	/* LDPC */
	prtcl->ht_ldpc &= (TEST_FLAG(rgstry->ldpc_cap, BIT4) ? 1 : 0);
	cap->tx_ht_ldpc &= (TEST_FLAG(rgstry->ldpc_cap, BIT5) ? 1 : 0);
	prtcl->vht_ldpc &= (TEST_FLAG(rgstry->ldpc_cap, BIT0) ? 1 : 0);
	cap->tx_vht_ldpc &= (TEST_FLAG(rgstry->ldpc_cap, BIT1) ? 1 : 0);
	prtcl->he_ldpc &= (TEST_FLAG(rgstry->ldpc_cap, BIT2) ? 1 : 0);
	cap->tx_he_ldpc &= (TEST_FLAG(rgstry->ldpc_cap, BIT3) ? 1 : 0);
}
#endif /* RTW_WKARD_UPDATE_PHL_ROLE_CAP */

static void _dump_phl_sta_asoc_cap(struct sta_info *sta)
{
	struct rtw_phl_stainfo_t *phl_sta = sta->phl_sta;
	struct protocol_cap_t *asoc_cap = &phl_sta->asoc_cap;
#define _loc_dbg_func	RTW_DBG
#define _loc_dbg(f)     _loc_dbg_func(#f ": %u\n", asoc_cap->f)


	_loc_dbg_func("[PHL STA ASOC CAP]- mac_addr: " MAC_FMT "\n",
		      MAC_ARG(phl_sta->mac_addr));
	_loc_dbg(ht_ldpc);
	_loc_dbg(vht_ldpc);
	_loc_dbg(he_ldpc);
	_loc_dbg(stbc_ht_rx);
	_loc_dbg(stbc_vht_rx);
	_loc_dbg(stbc_he_rx);
	_loc_dbg(vht_su_bfmr);
	_loc_dbg(vht_su_bfme);
	_loc_dbg(vht_mu_bfmr);
	_loc_dbg(vht_mu_bfme);
	_loc_dbg(bfme_sts);
	_loc_dbg(num_snd_dim);
	_loc_dbg_func("[PHL STA ASOC CAP]- end\n");
}

#ifdef CONFIG_80211N_HT
#ifdef CONFIG_80211AC_VHT
static void update_phl_sta_cap_vht(struct _ADAPTER *a, struct sta_info *sta,
			           struct protocol_cap_t *cap)
{
	struct vht_priv *vht;


	vht = &sta->vhtpriv;

	if (cap->ampdu_len_exp < vht->ampdu_len)
		cap->ampdu_len_exp = vht->ampdu_len;
	if (cap->max_amsdu_len < vht->max_mpdu_len)
		cap->max_amsdu_len = vht->max_mpdu_len;

	cap->sgi_80 = (vht->sgi_80m == _TRUE) ? 1 : 0;
	cap->sgi_160 = (vht->sgi_160m == _TRUE) ? 1 : 0;

	_rtw_memcpy(cap->vht_rx_mcs, vht->vht_mcs_map, 2);
	/* Todo: cap->vht_tx_mcs[2]; */
	if (vht->op_present)
		_rtw_memcpy(cap->vht_basic_mcs, &vht->vht_op[3], 2);
}
#endif /* CONFIG_80211AC_VHT */
static void update_phl_sta_cap_ht(struct _ADAPTER *a, struct sta_info *sta,
			          struct protocol_cap_t *cap)
{
	struct link_mlme_ext_info *info;
	struct ht_priv *ht;
	struct ampdu_priv *ampdu_priv;

	info = &sta->padapter_link->mlmeextpriv.mlmext_info;
	ht = &sta->htpriv;
	ampdu_priv = &sta->ampdu_priv;

	/* Set to 64, ref to _hal_update_cctrl_tbl() */
	cap->num_ampdu = 64;

	cap->ampdu_density = ampdu_priv->rx_ampdu_min_spacing;
	cap->ampdu_len_exp = GET_HT_CAP_ELE_MAX_AMPDU_LEN_EXP(&ht->ht_cap);
	cap->amsdu_in_ampdu = 1;
	cap->max_amsdu_len = GET_HT_CAP_ELE_MAX_AMSDU_LENGTH(&ht->ht_cap);

	/* move to update_sta_smps_cap  */
	/*GET_HT_CAP_ELE_SM_PS(&info->HT_caps.u.HT_cap_element.HT_caps_info);*/
	/* cap->sm_ps = info->SM_PS; */

	cap->sgi_20 = (ht->sgi_20m == _TRUE) ? 1 : 0;
	cap->sgi_40 = (ht->sgi_40m == _TRUE) ? 1 : 0;

	_rtw_memcpy(cap->ht_rx_mcs, ht->ht_cap.supp_mcs_set, 4);
	/* Todo: cap->ht_tx_mcs[4]; */
	if (info->HT_info_enable)
		_rtw_memcpy(cap->ht_basic_mcs, info->HT_info.MCS_rate, 4);
}
#endif /* CONFIG_80211N_HT */

void rtw_update_phl_sta_cap(struct _ADAPTER *a, struct sta_info *sta,
			    struct protocol_cap_t *cap)
{
	struct link_mlme_ext_info *info;

	info = &sta->padapter_link->mlmeextpriv.mlmext_info;

	/* MAC related */
	/* update beacon interval */
	cap->bcn_interval = info->bcn_interval;
#if 0
	cap->num_ampdu;		/* HT, VHT, HE */
	cap->ampdu_density:3;	/* HT, VHT, HE */
	cap->ampdu_len_exp;	/* HT, VHT, HE */
	cap->amsdu_in_ampdu:1;	/* HT, VHT, HE */
	cap->max_amsdu_len:2;	/* HT, VHT, HE */
	cap->htc_rx:1;
	cap->sm_ps:2;		/* HT */
	cap->trig_padding:2;
	cap->twt:6;
	cap->all_ack:1;
	cap->a_ctrl:3;
	cap->ops:1;
	cap->ht_vht_trig_rx:1;
#endif
	cap->short_slot = (info->slotTime == SHORT_SLOT_TIME) ? 1 : 0;
	cap->preamble = (info->preamble_mode == PREAMBLE_SHORT) ? 1 : 0;
#if 0
	cap->sgi_20:1;		/* HT */
	cap->sgi_40:1;		/* HT */
	cap->sgi_80:1;		/* VHT */
	cap->sgi_160:1		/* VHT, HE */
	/* update in WMMOnAssocRsp() @ core/rtw_wlan_util.c */
	struct rtw_edca_param edca[4]; 	/* Access Category, 0:BE, 1:BK, 2:VI, 3:VO */
	u8 mu_qos_info;
	/* update in HE_mu_edca_handler() @ core/rtw_he.c */
	struct rtw_mu_edca_param mu_edca[4];	/* HE */

	/* BB related */
	cap->ht_ldpc:1;		/* HT, HT_caps_handler() */
	cap->vht_ldpc:1;	/* VHT, VHT_caps_handler() */
	cap->he_ldpc:1;		/* HE, HE_phy_caps_handler() */
	cap->sgi:1;
	cap->su_bfmr:1;
	cap->su_bfme:1;
	cap->mu_bfmr:1;
	cap->mu_bfme:1;
	cap->bfme_sts:3;
	cap->num_snd_dim:3;
#endif
	_rtw_memset(cap->supported_rates, 0, 12);
	_rtw_memcpy(cap->supported_rates, sta->bssrateset,
		    sta->bssratelen < 12 ? sta->bssratelen : 12);
#if 0
	cap->ht_rx_mcs[4];	/* HT */
	cap->ht_tx_mcs[4];	/* HT */
	cap->ht_basic_mcs[4];	/* Basic rate of HT */
	cap->vht_rx_mcs[2];	/* VHT */
	cap->vht_tx_mcs[2];	/* VHT */
	cap->vht_basic_mcs[2];	/* Basic rate of VHT */
#endif
#if 0
	/* HE done */
	cap->he_rx_mcs[2];
	cap->he_tx_mcs[2];
	cap->he_basic_mcs[2];	/* Basic rate of HE */
	cap->stbc_ht_rx:2;	/* HT_caps_handler() */
	cap->stbc_vht_rx:3;	/* VHT_caps_handler() */
	cap->stbc_he_rx:1;	/* HE_phy_caps_handler() */
	cap->stbc_tx:1;
	cap->ltf_gi;
	cap->doppler_tx:1;
	cap->doppler_rx:1;
	cap->dcm_max_const_tx:2;
	cap->dcm_max_nss_tx:1;
	cap->dcm_max_const_rx:2;
	cap->dcm_max_nss_rx:1;
	cap->partial_bw_su_in_mu:1;
	cap->bfme_sts_greater_80mhz:3;
	cap->num_snd_dim_greater_80mhz:3;
	cap->stbc_tx_greater_80mhz:1;
	cap->stbc_rx_greater_80mhz:1;
	cap->ng_16_su_fb:1;
	cap->ng_16_mu_fb:1;
	cap->cb_sz_su_fb:1;
	cap->cb_sz_mu_fb:1;
	cap->trig_su_bfm_fb:1;
	cap->trig_mu_bfm_fb:1;
	cap->trig_cqi_fb:1;
	cap->partial_bw_su_er:1;
	cap->pkt_padding:2;
	cap->ppe_th[24];
	cap->pwr_bst_factor:1;
	cap->max_nc:3;
	cap->dcm_max_ru:2;
	cap->long_sigb_symbol:1;
	cap->non_trig_cqi_fb:1;
	cap->tx_1024q_ru:1;
	cap->rx_1024q_ru:1;
	cap->fbw_su_using_mu_cmprs_sigb:1;
	cap->fbw_su_using_mu_non_cmprs_sigb:1;
	cap->er_su:1;
	cap->tb_pe:3;
	cap->txop_du_rts_th;
#endif

#ifdef CONFIG_80211N_HT
	if (sta->htpriv.ht_option) {
		update_phl_sta_cap_ht(a, sta, cap);
#ifdef CONFIG_80211AC_VHT
		if (sta->vhtpriv.vht_option)
			update_phl_sta_cap_vht(a, sta, cap);;
#endif /* CONFIG_80211AC_VHT */
	}
#endif /* CONFIG_80211N_HT */
}

/**
 * rtw_update_phl_sta_edca() - Update PHL STA EDCA parameter
 * @a:		struct _ADAPTER *
 * @ac:		Access Category, ref: enum rtw_ac
 * @param:	AIFS:BIT[7:0], CWMIN:BIT[11:8], CWMAX:BIT[15:12],
 *		TXOP:BIT[31:16]
 *		ref: struct rtw_edca_param
 *
 * Update WMM EDCA parameter set to PHL STA protocol capability.
 */
void rtw_update_phl_sta_edca(struct _ADAPTER *a, struct _ADAPTER_LINK *alink, enum rtw_ac ac, u32 param)
{
	struct sta_info *sta;
	struct protocol_cap_t *cap;
	u8 *mac;


	mac = (u8*)alink->mlmeextpriv.mlmext_info.network.MacAddress;
	sta = rtw_get_stainfo(&a->stapriv, mac);
	if (!sta) {
		RTW_ERR(FUNC_ADPT_FMT ": drv sta_info(" MAC_FMT ") not exist!\n",
			FUNC_ADPT_ARG(a), MAC_ARG(mac));
		return;
	}
	cap = &sta->phl_sta->asoc_cap;

	cap->edca[ac].param = param;
}

/**
 * rtw_hw_set_edca() - setup WMM EDCA parameter
 * @a:		struct _ADAPTER *
 * @ac:		Access Category, 0:BE, 1:BK, 2:VI, 3:VO
 * @param:	AIFS:BIT[7:0], CWMIN:BIT[11:8], CWMAX:BIT[15:12],
 *		TXOP:BIT[31:16]
 *
 * Setup WMM EDCA parameter set.
 *
 * Return 0 for SUCCESS, otherwise fail.
 */
int rtw_hw_set_edca(struct _ADAPTER *a, struct _ADAPTER_LINK *alink, u8 ac, u32 param)
{
	struct dvobj_priv *d;
	void *phl;
	struct rtw_edca_param edca = {0};
	enum rtw_phl_status status;


	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		return -1;

	edca.ac = ac;
	edca.param = param;

	status = rtw_phl_cmd_wrole_change(phl, a->phl_role, alink->wrlink,
				WR_CHG_EDCA_PARAM, (u8*)&edca, sizeof(struct rtw_edca_param),
				PHL_CMD_DIRECTLY, 0);

	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s: fail to set edca parameter, ac(%u), "
			"param(0x%08x)\n",
			__func__, ac, param);
		return -1;
	}

	return 0;
}

int rtw_hw_connected(struct _ADAPTER *a)
{
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	struct security_priv *psecuritypriv = &a->securitypriv;
	struct _ADAPTER_LINK *alink = NULL;
	struct sta_info *sta = NULL;
	u8 lidx;


	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		return -1;

	for (lidx = 0; lidx < a->adapter_link_num; lidx++) {
		alink = GET_LINK(a, lidx);
		if (!alink->mlmepriv.is_accepted)
			continue;
		sta = rtw_get_stainfo(&a->stapriv, alink->mlmeextpriv.mlmext_info.network.MacAddress);
		if (!sta)
			return -1;
		rtw_update_phl_sta_cap(a, sta, &sta->phl_sta->asoc_cap);
		_dump_phl_sta_asoc_cap(sta);

#ifdef CONFIG_STA_MULTIPLE_BSSID
		/*use addr cam mask 0x1F to receive byte0~byte4 the same BSSID address == STA_CHG_MBSSID*/
		if (alink->mlmeextpriv.mlmext_info.network.is_mbssid) {
			sta->phl_sta->addr_sel = 3; /*MAC_AX_BSSID_MSK*/
			sta->phl_sta->addr_msk = 0x1F; /*MAC_AX_BYTE5*/
		}
#endif

		status = rtw_phl_cmd_update_media_status(phl, sta->phl_sta,
					sta->phl_sta->mac_addr, true,
					PHL_CMD_DIRECTLY, 0);
		if (status != RTW_PHL_STATUS_SUCCESS)
			return -1;
		rtw_dump_phl_sta_info(RTW_DBGDUMP, sta);

		/* Todo: udpate capability: short preamble, slot time */
		update_capinfo(a, alink, alink->mlmeextpriv.mlmext_info.capability);

		WMMOnAssocRsp(a, alink);
	}

	/* Todo: update IOT-releated issue */
#if 0
	update_IOT_info(a);
#endif
	/* Todo: RTS full bandwidth setting */
#if 0
#ifdef CONFIG_RTS_FULL_BW
	rtw_set_rts_bw(a);
#endif /* CONFIG_RTS_FULL_BW */
#endif
	/* Todo: Basic rate setting */
#if 0
	rtw_hal_set_hwreg(a, HW_VAR_BASIC_RATE, cur_network->SupportedRates);
#endif

	/* Todo: HT: AMPDU factor, min space, max time and related parameters */
#if 0
#ifdef CONFIG_80211N_HT
	HTOnAssocRsp(a);
#endif /* CONFIG_80211N_HT */
#endif
	/* Todo: VHT */
#if 0
#ifdef CONFIG_80211AC_VHT
	VHTOnAssocRsp(a);
#endif
#endif
	/* Todo: Set Data rate and RA */
#if 0
	set_sta_rate(a, psta);
#endif
	/* Todo: Firmware media status report */
#if 0
	rtw_sta_media_status_rpt(a, psta, 1);
#endif
	/* Todo: IC specific hardware setting */
#if 0
	join_type = 2;
	rtw_hal_set_hwreg(a, HW_VAR_MLME_JOIN, (u8 *)(&join_type));
#endif
	if ((a->mlmeextpriv.mlmext_info.state & 0x03) == WIFI_FW_STATION_STATE) {
		/* Todo: Correct TSF */
#if 0
		correct_TSF(a, MLME_STA_CONNECTED);
#endif
	}

	/* Todo: btcoex connect event notify */
#if 0
	rtw_btcoex_connect_notify(a, join_type);
#endif
	/* Todo: Beamforming setting */
#if 0
	beamforming_wk_cmd(a, BEAMFORMING_CTRL_ENTER, (u8 *)psta, sizeof(struct sta_info), 0);
#endif

	rtw_join_done_chk_ch(a, 1);
	status = rtw_phl_connected(phl, a->connect_bidx, a->phl_role, sta->phl_sta);

	if (status != RTW_PHL_STATUS_SUCCESS)
			return -1;

#ifdef CONFIG_80211AX_HE
	for (lidx = 0; lidx < a->adapter_link_num; lidx++) {
		alink = GET_LINK(a, lidx);
		if (!alink->mlmepriv.is_accepted)
			continue;
		rtw_he_init_om_info(a, alink);
	}
#endif
	ATOMIC_SET(&a->need_tsf_sync_done, _TRUE);
	return 0;
}

int rtw_hw_disconnect(struct _ADAPTER *a, struct sta_info *sta)
{
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	int tid;
	u8 is_ap_self = _FALSE;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		return -1;

	if (MLME_IS_AP(a) &&
		_rtw_memcmp(a->phl_role->mac_addr, sta->phl_sta->mac_addr, ETH_ALEN))
		is_ap_self = _TRUE;

	/* Check and reset setting related to rx ampdu resources of PHL. */
	for (tid = 0; tid < TID_NUM; tid++) {
		if(sta->recvreorder_ctrl[tid].enable == _TRUE) {
			sta->recvreorder_ctrl[tid].enable =_FALSE;
			rtw_phl_stop_rx_ba_session(phl, sta->phl_sta, tid);
			RTW_INFO(FUNC_ADPT_FMT"stop process tid %d \n",
				FUNC_ADPT_ARG(a), tid);
		}
	}

	/*reset sec setting and clean all connection setting*/
	rtw_hw_del_all_key(a, sta, PHL_CMD_DIRECTLY, 0);

	if (is_ap_self == _FALSE) {
		status = rtw_phl_cmd_update_media_status(phl, sta->phl_sta, NULL, false,
						PHL_CMD_DIRECTLY, 0);
		if (status != RTW_PHL_STATUS_SUCCESS)
			return -1;

		rtw_dump_phl_sta_info(RTW_DBGDUMP, sta);
	}

	return 0;
}

int rtw_hw_connected_apmode(struct _ADAPTER *a, struct sta_info *sta)
{
	struct dvobj_priv *d;
	void *phl;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	if (!phl)
		return -1;

	rtw_ap_set_sta_wmode(a, sta);
	update_sta_ra_info(a, sta);
	rtw_update_phl_sta_cap(a, sta, &sta->phl_sta->asoc_cap);

	if (RTW_PHL_STATUS_SUCCESS != rtw_phl_cmd_update_media_status(
		phl, sta->phl_sta, sta->phl_sta->mac_addr, true,
		PHL_CMD_DIRECTLY, 0))
		return -1;

	rtw_dump_phl_sta_info(RTW_DBGDUMP, sta);

	return 0;
}

u8 rtw_hal_get_def_var(struct _ADAPTER *a, struct _ADAPTER_LINK *alink,
				enum _HAL_DEF_VARIABLE def_var, void *val)
{
	struct rtw_wifi_role_t *wrole = a->phl_role;
	struct protocol_cap_t *proto_cap = &(alink->wrlink->protocol_cap);

	switch (def_var) {
	case HAL_DEF_IS_SUPPORT_ANT_DIV:
		*(u8*)val = _FALSE;
		break;
	case HAL_DEF_DBG_DUMP_RXPKT:
		*(u8*)val = 0;
		break;
	case HAL_DEF_BEAMFORMER_CAP:
		*(u8*)val = proto_cap->num_snd_dim;
		break;
	case HAL_DEF_BEAMFORMEE_CAP:
		*(u8*)val = proto_cap->bfme_sts;
		break;
	case HW_VAR_MAX_RX_AMPDU_FACTOR:
		/* HT only */
		*(enum _HT_CAP_AMPDU_FACTOR*)val = MAX_AMPDU_FACTOR_64K;
		break;
	case HW_DEF_RA_INFO_DUMP:
		/* do nothing */
		break;
	case HAL_DEF_DBG_DUMP_TXPKT:
		*(u8*)val = 0;
		break;
	case HAL_DEF_TX_PAGE_SIZE:
		/* would be removed later */
		break;
	case HW_VAR_BEST_AMPDU_DENSITY:
		*(u8*)val = 0;
		break;
	default:
		break;
	}

	return 0;
}

#ifdef RTW_DETECT_HANG
#define HANG_DETECT_THR 3
#define MAC_RX_FULL_DROP_CHK_MAX_NUM 100
void rtw_is_rxff_hang(_adapter *padapter, struct rxff_hang_info *prxff_hang_info)
{
	struct dvobj_priv *pdvobjpriv = padapter->dvobj;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(padapter);
	struct rtw_wifi_role_link_t *rlink = adapter_link->wrlink;
	void *phl = GET_PHL_INFO(pdvobjpriv);
	u16 mac_rx_full_drop_cnt = 0;
	u8 chk_cnt = 0;
	enum rtw_phl_status psts;

	if (prxff_hang_info->rx_cnt != prxff_hang_info->last_rx_cnt) {
		prxff_hang_info->last_rx_cnt = prxff_hang_info->rx_cnt;
		prxff_hang_info->rx_ff_hang_cnt = 0;
		prxff_hang_info->dbg_is_rxff_hang = _FALSE;
		return;
	}

	psts = rtw_phl_cmd_set_reset_rx_cnt(phl, rlink->hw_band,
					    PHL_CMD_DIRECTLY, 0);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		RTW_WARN("%s: reset rx dbg cnt failed\n", __func__);
		return;
	}

	do {
		psts = rtw_phl_cmd_get_rx_cnt_by_idx(phl,rlink->hw_band,
						     RXCNT_FULLDRP_PKT,
						     &mac_rx_full_drop_cnt,
						     PHL_CMD_DIRECTLY, 0);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			RTW_WARN("%s: get rx full drop cnt failed\n", __func__);
			continue;
		}

		if (mac_rx_full_drop_cnt) {
			RTW_INFO("%s: mac rx full drop w/o HCI rx\n", __func__);
			prxff_hang_info->rx_ff_hang_cnt++;
			break;
		}
	} while (++chk_cnt < MAC_RX_FULL_DROP_CHK_MAX_NUM);

	if (prxff_hang_info->rx_ff_hang_cnt >= HANG_DETECT_THR)
		prxff_hang_info->dbg_is_rxff_hang = _TRUE;
}

void rtw_is_fw_hang(_adapter *padapter, struct fw_hang_info *pfw_hang_info)
{
	struct dvobj_priv *pdvobjpriv = padapter->dvobj;
	void *phl = GET_PHL_INFO(pdvobjpriv);
	enum rtw_fw_status fw_sts;

	fw_sts = rtw_phl_get_fw_status(phl);

	if (fw_sts == RTW_FW_STATUS_NOFW) {
		pfw_hang_info->dbg_is_fw_gone = _TRUE;
		pfw_hang_info->dbg_is_fw_hang = _FALSE;
	} else {
		pfw_hang_info->dbg_is_fw_gone = _FALSE;

		if (fw_sts == RTW_FW_STATUS_ASSERT ||
		    fw_sts == RTW_FW_STATUS_EXCEP ||
		    fw_sts == RTW_FW_STATUS_RXI300 ||
		    fw_sts == RTW_FW_STATUS_HANG)
			pfw_hang_info->dbg_is_fw_hang = _TRUE;
		else
			pfw_hang_info->dbg_is_fw_hang = _FALSE;
	}
}

void rtw_is_hang_check(_adapter *padapter)
{
	u32 start_time = rtw_get_current_time();
	struct dvobj_priv *pdvobjpriv = padapter->dvobj;
	struct debug_priv *pdbgpriv = &pdvobjpriv->drv_dbg;
	struct hang_info *phang_info = &pdbgpriv->dbg_hang_info;
	/* struct fw_hang_info *pfw_hang_info = &phang_info->dbg_fw_hang_info; */
	struct rxff_hang_info *prxff_hang_info = &phang_info->dbg_rxff_hang_info;
	struct fw_hang_info *pfw_hang_info = &phang_info->dbg_fw_hang_info;
	u8 is_fw_in_ps_mode = _FALSE;
	u8 is_fw_ps_awake = _TRUE;

	if (phang_info->is_stop)
		return;

	if (rtw_hw_get_init_completed(pdvobjpriv) && (!is_fw_in_ps_mode) &&
	    is_fw_ps_awake) {
		phang_info->enter_cnt++;

		rtw_is_rxff_hang(padapter, prxff_hang_info);
		rtw_is_fw_hang(padapter, pfw_hang_info);
	}
}
#endif /* RTW_DETECT_HANG */

static u8 target_drv2phl(enum _HT_IOT_PEER drv)
{
	switch (drv) {
	case HT_IOT_PEER_UNKNOWN:
	default:
		return 0;
	}

	return 0;
}

void rtw_update_phl_iot(struct _ADAPTER *a, enum _HT_IOT_PEER peer)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(a);
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);
	u32 iot;


	iot = IOT_ID(target_drv2phl(peer));
	phl_com->id.iot_id[a->phl_role->id] = iot;
	RTW_INFO(FUNC_ADPT_FMT ": iot = 0x%08x\n", FUNC_ADPT_ARG(a), iot);
}

#ifdef CONFIG_RTW_ACS
u8 rtw_acs_get_clm_ratio(struct _ADAPTER *a, enum band_type band, u8 ch)
{
	struct dvobj_priv *d = adapter_to_dvobj(a);
	struct rtw_acs_info_parm parm = {0};
	enum rtw_phl_status rtn;

	parm.idx = rtw_phl_get_acs_chnl_tbl_idx(d->phl, band, ch);
	rtn = rtw_phl_get_acs_info(d->phl, &parm);

	if (rtn == RTW_PHL_STATUS_SUCCESS)
		return parm.rpt.clm_ratio;

	return 0;
}

u8 rtw_acs_get_nhm_ratio(struct _ADAPTER *a, enum band_type band, u8 ch)
{
	struct dvobj_priv *d = adapter_to_dvobj(a);
	struct rtw_acs_info_parm parm = {0};
	enum rtw_phl_status rtn;

	parm.idx = rtw_phl_get_acs_chnl_tbl_idx(d->phl, band, ch);
	rtn = rtw_phl_get_acs_info(d->phl, &parm);

	if (rtn == RTW_PHL_STATUS_SUCCESS)
		return parm.rpt.nhm_ratio;

	return 0;
}

s8 rtw_acs_get_noise_dbm(struct _ADAPTER *a, enum band_type band, u8 ch)
{
	struct dvobj_priv *d = adapter_to_dvobj(a);
	struct rtw_acs_info_parm parm = {0};
	enum rtw_phl_status rtn;

	parm.idx = rtw_phl_get_acs_chnl_tbl_idx(d->phl, band, ch);
	rtn = rtw_phl_get_acs_info(d->phl, &parm);

	if (rtn == RTW_PHL_STATUS_SUCCESS)
		return parm.rpt.nhm_pwr;

	return -110;
}

int rtw_acs_get_report(struct _ADAPTER *a, enum band_type band, u8 ch, struct rtw_acs_info_parm *rpt)
{
	struct dvobj_priv *d = adapter_to_dvobj(a);
	enum rtw_phl_status rtn;

	rpt->idx = rtw_phl_get_acs_chnl_tbl_idx(d->phl, band, ch);
	rtn = rtw_phl_get_acs_info(d->phl, rpt);

	if (rtn != RTW_PHL_STATUS_SUCCESS)
		return -1;

	return 0;
}

#endif /* CONFIG_RTW_ACS */

void rtw_dump_env_rpt(struct _ADAPTER *a, void *sel)
{
	struct dvobj_priv *d = adapter_to_dvobj(a);
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(d);
	void *phl = GET_PHL_INFO(d);
	struct rtw_env_report rpt;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(a);

	rtw_phl_get_env_rpt(phl, &rpt, alink->wrlink->hw_band);

	RTW_PRINT_SEL(sel, "clm_ratio:%d (%%)\n", rpt.nhm_cca_ratio);
	RTW_PRINT_SEL(sel, "nhm_ratio:%d (%%)\n", rpt.nhm_ratio);
}

void rtw_dump_rfe_type(struct dvobj_priv *d)
{
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(d);

	RTW_INFO("RFE Type: %d\n", phl_com->dev_cap.rfe_type);
}

#ifdef CONFIG_WOWLAN
static u8 _cfg_keep_alive_info(struct _ADAPTER *a, u8 enable)
{
	struct rtw_keep_alive_info info;
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	u8 check_period = 5;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);

	_rtw_memset(&info, 0, sizeof(struct rtw_keep_alive_info));

	info.keep_alive_en = enable;
	info.keep_alive_period = check_period;
#ifdef CONFIG_ARP_KEEP_ALIVE
	/* Require arp_en = 1 in _cfg_arp_ofld_info() */
	info.keep_alive_pkt_type = PKT_TYPE_ARP_RSP;
#else
	info.keep_alive_pkt_type = PKT_TYPE_NULL_DATA;
#endif

	RTW_INFO("%s: keep_alive_en=%d, keep_alive_period=%d, keep_alive_pkt_type=%d\n",
		 __func__, info.keep_alive_en, info.keep_alive_period,
		 info.keep_alive_pkt_type);

	status = rtw_phl_cfg_keep_alive_info(phl, &info);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_INFO("%s fail(%d)\n", __func__, status);
		return _FAIL;
	}

	return _SUCCESS;
}

static u8 _cfg_disc_det_info(struct _ADAPTER *a, u8 enable)
{
	struct wow_priv *wowpriv = adapter_to_wowlan(a);
	struct rtw_disc_det_info *wow_disc = &wowpriv->wow_disc;
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	struct registry_priv *registry_par;
	u8 check_period = 100, trypkt_num = 5;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	registry_par = &a->registrypriv;

	wow_disc->disc_det_en = enable;

	/* wake up event includes deauth wake up */
	if (registry_par->wakeup_event & BIT(2))
		wow_disc->disc_wake_en = _TRUE;
	else
		wow_disc->disc_wake_en = _FALSE;
	wow_disc->try_pkt_count = trypkt_num;
	wow_disc->check_period = check_period;

	wow_disc->cnt_bcn_lost_en = 0;
	wow_disc->cnt_bcn_lost_limit = 0;

	status = rtw_phl_cfg_disc_det_info(phl, wow_disc);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_INFO("%s fail(%d)\n", __func__, status);
		return _FAIL;
	}

	return _SUCCESS;
}

static u8 _cfg_arp_ofld_info(struct _ADAPTER *a)
{
	struct rtw_arp_ofld_info info;
	struct dvobj_priv *d;
	void *phl;
	struct mlme_ext_priv *pmlmeext = &(a->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;
	struct mlme_priv *pmlmepriv = &(a->mlmepriv);

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	_rtw_memset(&info, 0, sizeof(struct rtw_arp_ofld_info));

	info.arp_en = 1;

	if (info.arp_en) {
		/* Sender IP address */
		_rtw_memcpy(info.arp_ofld_content.host_ipv4_addr,
			    pmlmeinfo->ip_addr, IPV4_ADDRESS_LENGTH);

#ifdef CONFIG_ARP_KEEP_ALIVE
		/*
		 * Driver only needs to fill in the Target IP address and Target
		 * MAC address when ARP keepalive is enabled. In general, when
		 * the FW receives an ARP request, it will use the Sender IP
		 * address and Sender MAC address of the ARP request as the
		 * destination IP address and destination MAC address of the ARP
		 * response.
		 */
#ifdef CONFIG_ARP_KEEP_ALIVE_GW
		if (!is_zero_mac_addr(pmlmepriv->gw_mac_addr)) {
			RTW_INFO("%s: gw arp keepalive enabled\n", __func__);

			/* Gateway MAC address of A3 */
			_rtw_memcpy(info.arp_ofld_content.a3,
				    pmlmepriv->gw_mac_addr, MAC_ADDRESS_LENGTH);

			/* Gateway Target IP address */
			_rtw_memcpy(info.arp_ofld_content.remote_ipv4_addr,
				    pmlmepriv->gw_ip, IPV4_ADDRESS_LENGTH);

			/* Gateway Target MAC address */
			_rtw_memcpy(info.arp_ofld_content.remote_mac_addr,
				    pmlmepriv->gw_mac_addr, MAC_ADDRESS_LENGTH);
		} else
#endif /* CONFIG_ARP_KEEP_ALIVE_GW */
		{
			/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
			struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(a);
			struct rtw_phl_stainfo_t *psta = NULL;

			RTW_INFO("%s: ap arp keepalive enabled\n", __func__);

			psta = rtw_phl_get_stainfo_self(phl, alink->wrlink);

			/* AP MAC address of A3 */
			_rtw_memcpy(info.arp_ofld_content.a3,
				    psta->mac_addr, MAC_ADDRESS_LENGTH);

			/* STA Target IP address */
			_rtw_memcpy(info.arp_ofld_content.remote_ipv4_addr,
				    pmlmeinfo->ip_addr, IPV4_ADDRESS_LENGTH);

			/* STA Target MAC address */
			_rtw_memcpy(info.arp_ofld_content.remote_mac_addr,
				    alink->mac_addr, MAC_ADDRESS_LENGTH);
		}
#endif /* CONFIG_ARP_KEEP_ALIVE */
	}

	rtw_phl_cfg_arp_ofld_info(phl, &info);

	return _SUCCESS;
}

static u8 _cfg_ndp_ofld_info(struct _ADAPTER *a)
{
	struct rtw_ndp_ofld_info info;
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);

	_rtw_memset(&info, 0, sizeof(struct rtw_ndp_ofld_info));

	rtw_phl_cfg_ndp_ofld_info(phl, &info);

	return _SUCCESS;
}

#ifdef CONFIG_GTK_OL
static u8 _cfg_gtk_ofld_info(struct _ADAPTER *a)
{
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	struct rtw_gtk_ofld_info gtk_ofld_info = {0};
	struct rtw_gtk_ofld_content *gtk_ofld_content = NULL;
	struct security_priv *securitypriv = &a->securitypriv;
	struct sta_info *sta = NULL;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(a);

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	sta = rtw_get_stainfo(&a->stapriv, get_bssid(&a->mlmepriv));
	gtk_ofld_content = &gtk_ofld_info.gtk_ofld_content;

	if (securitypriv->binstallKCK_KEK) {
		gtk_ofld_info.gtk_en = _TRUE;

		gtk_ofld_info.akmtype_byte3 = securitypriv->rsn_akm_suite_type;

		gtk_ofld_content->kck_len = RTW_KCK_LEN;
		_rtw_memcpy(gtk_ofld_content->kck, sta->kck, RTW_KCK_LEN);

		gtk_ofld_content->kek_len = RTW_KEK_LEN;
		_rtw_memcpy(gtk_ofld_content->kek, sta->kek, RTW_KEK_LEN);

		if (securitypriv->dot11PrivacyAlgrthm == _TKIP_) {
			gtk_ofld_info.tkip_en = _TRUE;
			/* The driver offloads the Tx MIC key here, which is
			 * actually the Rx MIC key, but the driver definition is
			 * the opposite of the correct definition.
			 */
			_rtw_memcpy(gtk_ofld_content->rxmickey,
				    sta->dot11tkiptxmickey.skey, RTW_TKIP_MIC_LEN);
		}

		_rtw_memcpy(gtk_ofld_content->replay_cnt, sta->replay_ctr,
			    RTW_REPLAY_CTR_LEN);
	}

#ifdef CONFIG_IEEE80211W
	if (SEC_IS_BIP_KEY_INSTALLED(&alink->securitypriv)) {
		gtk_ofld_info.ieee80211w_en = 1;
		RTW_PUT_LE32(gtk_ofld_content->igtk_keyid,
			     alink->securitypriv.dot11wBIPKeyid);
		RTW_PUT_LE64(gtk_ofld_content->ipn,
			     alink->securitypriv.dot11wBIPrxpn.val);
		_rtw_memcpy(gtk_ofld_content->igtk[0],
			    alink->securitypriv.dot11wBIPKey[4].skey, RTW_IGTK_LEN);
		_rtw_memcpy(gtk_ofld_content->igtk[1],
			    alink->securitypriv.dot11wBIPKey[5].skey, RTW_IGTK_LEN);
		gtk_ofld_content->igtk_len = RTW_IGTK_LEN;

		_rtw_memcpy(gtk_ofld_content->psk,
			    sta->dot118021x_UncstKey.skey, RTW_PTK_LEN);
		gtk_ofld_content->psk_len = RTW_PTK_LEN;
	}
#endif

	rtw_phl_cfg_gtk_ofld_info(phl, &gtk_ofld_info);

	return _SUCCESS;
}
#endif

static u8 _cfg_realwow_info(struct _ADAPTER *a)
{
	struct rtw_realwow_info info;
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);

	/* default disable */
	_rtw_memset(&info, 0, sizeof(struct rtw_realwow_info));
	status = rtw_phl_cfg_realwow_info(phl, &info);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_INFO("%s fail(%d)\n", __func__, status);
		return _FAIL;
	}

	return _SUCCESS;
}

static u8 _cfg_wow_wake(struct _ADAPTER *a, u8 wow_en)
{
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	struct rtw_wow_wake_info wow_wake_event = {0};
	struct security_priv *securitypriv;
	struct registry_priv  *registry_par = &a->registrypriv;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(a);

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	securitypriv = &a->securitypriv;

	wow_wake_event.wow_en = _TRUE;
	/* wake up by magic packet */
	if (registry_par->wakeup_event & BIT(0))
		wow_wake_event.magic_pkt_en = _TRUE;
	else
		wow_wake_event.magic_pkt_en = _FALSE;
	/* wake up by deauth packet */
	if (registry_par->wakeup_event & BIT(2))
		wow_wake_event.deauth_wakeup = _TRUE;
	else
		wow_wake_event.deauth_wakeup = _FALSE;
	/* wake up by pattern match packet */
	if (registry_par->wakeup_event & (BIT(1) | BIT(3))) {
		wow_wake_event.pattern_match_en = _TRUE;

		rtw_wow_pattern_clean(a, RTW_DEFAULT_PATTERN);

		if (registry_par->wakeup_event & BIT(1))
			rtw_set_default_pattern(a);

		if (!(registry_par->wakeup_event & BIT(3)))
			rtw_wow_pattern_clean(a, RTW_CUSTOMIZED_PATTERN);
	} else {
		wow_wake_event.pattern_match_en = _FALSE;
	}
	/* wake up by ptk rekey */
	if (registry_par->wakeup_event & BIT(4))
		wow_wake_event.rekey_wakeup = _TRUE;
	else
		wow_wake_event.rekey_wakeup = _FALSE;

	wow_wake_event.pairwise_sec_algo = rtw_sec_algo_drv2phl(securitypriv->dot11PrivacyAlgrthm);
	wow_wake_event.group_sec_algo = rtw_sec_algo_drv2phl(securitypriv->dot118021XGrpPrivacy);
#ifdef CONFIG_IEEE80211W
	if (SEC_IS_BIP_KEY_INSTALLED(&alink->securitypriv))
		wow_wake_event.bip_sec_algo = rtw_sec_algo_drv2phl(securitypriv->dot11wCipher);
#endif

	rtw_construct_remote_control_info(a, &wow_wake_event.remote_wake_ctrl_info);

	status = rtw_phl_cfg_wow_wake(phl, &wow_wake_event);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_INFO("%s fail(%d)\n", __func__, status);
		return _FAIL;
	}

	return _SUCCESS;
}

static u8 _cfg_wow_gpio(struct _ADAPTER *a)
{
	struct rtw_wow_wake_info info = {0};
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	struct wow_priv *wowpriv = adapter_to_wowlan(a);
	struct rtw_wow_gpio_info *wow_gpio = &wowpriv->wow_gpio;
	struct registry_priv  *registry_par = &a->registrypriv;
	struct rtw_dev2hst_gpio_info *d2h_gpio_info = &wow_gpio->d2h_gpio_info;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
#ifdef CONFIG_GPIO_WAKEUP
	d2h_gpio_info->dev2hst_gpio_en = _TRUE;

	/* ToDo: fw/halmac do not support so far
	pwrctrlpriv->hst2dev_high_active = HIGH_ACTIVE_HST2DEV;
	*/
#ifdef CONFIG_RTW_ONE_PIN_GPIO
	wow_gpio->dev2hst_gpio_mode = RTW_AX_SW_IO_MODE_INPUT;
	status = rtw_phl_cfg_wow_set_sw_gpio_mode(phl, gpio);
#else
	#ifdef CONFIG_WAKEUP_GPIO_INPUT_MODE
	wow_gpio->dev2hst_gpio_mode = RTW_AX_SW_IO_MODE_OUTPUT_OD;
	d2h_gpio_info->gpio_output_input = _TRUE;
	#else
	wow_gpio->dev2hst_gpio_mode = RTW_AX_SW_IO_MODE_OUTPUT_PP;
	d2h_gpio_info->gpio_output_input = _FALSE;
	#endif /*CONFIG_WAKEUP_GPIO_INPUT_MODE*/
	/* switch GPIO to open-drain or push-pull */
	status = rtw_phl_cfg_wow_set_sw_gpio_mode(phl, wow_gpio);
	/*default low active, gpio_active and dev2hst_high is the same thing
	, but two halmac implementation. FW and halmac need to refine */
	status = rtw_phl_cfg_wow_sw_gpio_ctrl(phl, wow_gpio);
	RTW_INFO("%s: set GPIO_%d %d as default. status=%d\n",
		 __func__, WAKEUP_GPIO_IDX, wow_gpio->dev2hst_high, status);
#endif /* CONFIG_RTW_ONE_PIN_GPIO */

	/* SDIO inband wake sdio_wakeup_enable
	d2h_gpio_info->data_pin_wakeup = info->data_pin_wakeup;
	*/
	/* two halmac implementation. FW and halmac need to refine */
	wow_gpio->dev2hst_gpio = WAKEUP_GPIO_IDX;
	d2h_gpio_info->gpio_num = WAKEUP_GPIO_IDX;

	status = rtw_phl_cfg_gpio_wake_pulse(phl, wow_gpio);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_INFO("%s fail(%d)\n", __func__, status);
		return _FAIL;
	}
#endif /* CONFIG_GPIO_WAKEUP */
	return _SUCCESS;
}

static u8 _wow_cfg(struct _ADAPTER *a, u8 wow_en)
{
	struct dvobj_priv *d;
	void *phl;
	struct rtw_phl_stainfo_t *phl_sta;
	enum rtw_phl_status status;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);

	if (!_cfg_keep_alive_info(a, wow_en))
		return _FAIL;

	if(!_cfg_disc_det_info(a, wow_en))
		return _FAIL;

	if (!_cfg_arp_ofld_info(a))
		return _FAIL;

	if (!_cfg_ndp_ofld_info(a))
		return _FAIL;

#ifdef CONFIG_GTK_OL
	if (!_cfg_gtk_ofld_info(a))
		return _FAIL;
#endif

	if (!_cfg_realwow_info(a))
		return _FAIL;

	if (!_cfg_wow_wake(a, wow_en))
		return _FAIL;

	if(!_cfg_wow_gpio(a))
		return _FAIL;

	return _SUCCESS;
}

#ifdef CONFIG_PNO_SUPPORT
static u8 _cfg_nlo_info(struct _ADAPTER *a)
{
	struct dvobj_priv *d;
	struct wow_priv *wowpriv;
	void *phl;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);
	wowpriv = adapter_to_wowlan(a);

	rtw_phl_cfg_nlo_info(phl, &wowpriv->wow_nlo);

	return _SUCCESS;
}

static u8 _cfg_wow_nlo_wake(struct _ADAPTER *a)
{
	struct dvobj_priv *d;
	void *phl;
	enum rtw_phl_status status;
	struct rtw_wow_wake_info wow_wake_event = {0};
	struct registry_priv  *registry_par = &a->registrypriv;

	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);

	wow_wake_event.wow_en = _TRUE;
	/* wake up by magic packet */
	if (registry_par->wakeup_event & BIT(0))
		wow_wake_event.magic_pkt_en = _TRUE;
	else
		wow_wake_event.magic_pkt_en = _FALSE;

	status = rtw_phl_cfg_wow_wake(phl, &wow_wake_event);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_INFO("%s fail(%d)\n", __func__, status);
		return _FAIL;
	}

	return _SUCCESS;
}

static u8 _wow_nlo_cfg(struct _ADAPTER *a)
{
	if (!_cfg_nlo_info(a))
		return _FAIL;

	if (!_cfg_wow_nlo_wake(a))
		return _FAIL;

	if(!_cfg_wow_gpio(a))
		return _FAIL;

	return _SUCCESS;
}
#endif

u8 rtw_hw_wow(struct _ADAPTER *a, u8 wow_en)
{
	struct dvobj_priv *d;
	void *phl;
	struct pwrctrl_priv *pwrpriv;
	struct rtw_phl_stainfo_t *phl_sta;
	enum rtw_phl_status status;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(a);

	pwrpriv = adapter_to_pwrctl(a);
	d = adapter_to_dvobj(a);
	phl = GET_PHL_INFO(d);

#ifdef CONFIG_PNO_SUPPORT
	if (pwrpriv->wowlan_pno_enable) {
		if (!_wow_nlo_cfg(a))
			return _FAIL;
	} else
#endif
	{
		if (!_wow_cfg(a, wow_en))
			return _FAIL;
	}

	phl_sta = rtw_phl_get_stainfo_self(phl, alink->wrlink);

	if (wow_en) {
#ifdef CONFIG_WRC_WOW_MAGIC
		rtw_cfg_wrc_wol_magic(a, _TRUE);
#endif
		status = rtw_phl_suspend(phl, phl_sta, wow_en);
	} else {
		status = rtw_phl_resume(phl, phl_sta, &wow_en);
#ifdef CONFIG_WRC_WOW_MAGIC
		rtw_cfg_wrc_wol_magic(a, _FALSE);
#endif
	}

	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR("%s wow %s fail(status: %d)\n", __func__, wow_en ? "enable" : "disable", status);
		return _FAIL;
	}

	return _SUCCESS;
}
#endif /* CONFIG_WOWLAN */

static u32 rtw_tx_sts_total(u32 *tx_sts, u8 num)
{
	u32 ret = 0;
	int i = 0;

	for (i = 0; i < num; i++)
		ret += tx_sts[i];
	return ret;
}

int rtw_get_sta_tx_stat(_adapter *adapter, struct sta_info *psta)
{
#if defined(CONFIG_USB_HCI) || defined(CONFIG_PCI_HCI)
	struct stainfo_stats	*pstats = NULL;

	u32 tx_retry_cnt[PHL_AC_QUEUE_TOTAL] = {0};
	u32 tx_fail_cnt[PHL_AC_QUEUE_TOTAL] = {0};
	u32 tx_ok_cnt[PHL_AC_QUEUE_TOTAL] = {0};

	rtw_phl_get_tx_ra_retry_rpt(GET_PHL_INFO(adapter_to_dvobj(adapter)),
				    psta->phl_sta, tx_retry_cnt,
				    PHL_AC_QUEUE_TOTAL, 1);
	rtw_phl_get_tx_fail_rpt(GET_PHL_INFO(adapter_to_dvobj(adapter)), psta->phl_sta,
		tx_fail_cnt, PHL_AC_QUEUE_TOTAL);
	rtw_phl_get_tx_ok_rpt(GET_PHL_INFO(adapter_to_dvobj(adapter)), psta->phl_sta,
		tx_ok_cnt, PHL_AC_QUEUE_TOTAL);
	pstats = &psta->sta_stats;
	pstats->tx_retry_cnt = rtw_tx_sts_total(tx_retry_cnt, PHL_AC_QUEUE_TOTAL);
	pstats->tx_fail_cnt = rtw_tx_sts_total(tx_fail_cnt, PHL_AC_QUEUE_TOTAL);
	pstats->tx_ok_cnt =  rtw_tx_sts_total(tx_ok_cnt, PHL_AC_QUEUE_TOTAL);
	pstats->total_tx_retry_cnt += pstats->tx_retry_cnt;

	pstats->tx_fail_cnt_sum += pstats->tx_fail_cnt;
	pstats->tx_retry_cnt_sum += pstats->tx_retry_cnt;
#else
	RTW_INFO("%s() not support\n", __func__);
#endif

	return 0;
}

static enum rtw_edcca_mode rtw_edcca_mode_to_phl(enum rtw_edcca_mode_t mode)
{
	switch (mode) {
	case RTW_EDCCA_NORM:
		return RTW_EDCCA_NORMAL;
	case RTW_EDCCA_ADAPT:
		return RTW_EDCCA_ETSI;
	case RTW_EDCCA_CS:
		return RTW_EDCCA_JP;
	default:
		return RTW_EDCCA_MAX;
	}
}

void rtw_edcca_hal_update(struct dvobj_priv *dvobj)
{
	void *phl = GET_PHL_INFO(dvobj);
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);
	enum phl_band_idx band_idx = HW_BAND_0; /* TODO: DBCC */
	struct rtw_chan_def chdef;
	int chctx_num;
	enum band_type band;
	u8 mode = RTW_EDCCA_NORM;
	enum rtw_edcca_mode phl_mode = rtw_edcca_mode_to_phl(mode);

	chctx_num = rtw_phl_mr_get_chandef_by_hwband(phl, band_idx, &chdef);
	if (chctx_num > 1) {
		RTW_WARN("%s can't handle MCC case\n", __func__);
		goto exit;
	}

	if (chdef.chan != 0) {
		band = chdef.band;
		rfctl->last_edcca_mode_op_band = band;
	} else if (rfctl->last_edcca_mode_op_band != BAND_MAX)
		band = rfctl->last_edcca_mode_op_band;
	else {
		rtw_phl_get_cur_hal_chdef_by_hwband(phl, band_idx, &chdef);
		band = chdef.band;
	}

	mode = rtw_get_edcca_mode(dvobj, band);
	/*
	* may get band not existing in current channel plan
	* then edcca mode RTW_EDCCA_MODE_NUM is got
	* this is not a real problem because this band is not used for TX
	* change to RTW_EDCCA_NORM to avoid warning calltrace below
	*/
	if (mode == RTW_EDCCA_MODE_NUM)
		mode = RTW_EDCCA_NORM;

	phl_mode = rtw_edcca_mode_to_phl(mode);
	if (phl_mode == RTW_EDCCA_MAX) {
		RTW_WARN("%s can't get valid phl mode from %s(%d)\n", __func__, rtw_edcca_mode_str(mode), mode);
		rtw_warn_on(1);
		return;
	}

exit:
	if (rtw_phl_get_edcca_mode(phl) != phl_mode)
		rtw_phl_set_edcca_mode(phl, phl_mode);
}

#if CONFIG_TXPWR_LIMIT
enum txpwr_lmt_reg_exc_match rtw_txpwr_hal_lmt_reg_exc_search(struct dvobj_priv* dvobj, const char *country, u8 domain, const char **reg_name)
{
	u8 hal_match = rtw_phl_ext_reg_codemap_search(GET_PHL_INFO(dvobj), domain, country, reg_name);

	if (hal_match & RTW_PHL_EXT_REG_MATCH_COUNTRY)
		return TXPWR_LMT_REG_EXC_MATCH_COUNTRY;
	if (hal_match & RTW_PHL_EXT_REG_MATCH_DOMAIN)
		return TXPWR_LMT_REG_EXC_MATCH_DOMAIN;
	return TXPWR_LMT_REG_EXC_MATCH_NONE;
}

bool rtw_txpwr_hal_lmt_reg_search(struct dvobj_priv* dvobj, enum band_type band, const char *name)
{
	int hal_regu = rtw_phl_get_pw_lmt_regu_type_from_str(GET_PHL_INFO(dvobj), name);

	if (hal_regu == -1)
		return false;

	if (strcmp(name, txpwr_lmt_str(TXPWR_LMT_NONE)) == 0
		|| strcmp(name, txpwr_lmt_str(TXPWR_LMT_WW)) == 0)
		return true;

	return rtw_phl_pw_lmt_regu_tbl_exist(GET_PHL_INFO(dvobj), band, hal_regu);
}

void rtw_txpwr_hal_set_current_lmt_regs_by_name(struct dvobj_priv* dvobj, char *names_of_band[], int names_len_of_band[])
{
	struct txpwr_regu_info_t hal_conf;
	enum band_type band;
	int hal_regu, hal_none_regu = rtw_phl_get_pw_lmt_regu_type_from_str(GET_PHL_INFO(dvobj), "NONE");
	char *name;
	u8 *regu;
	u8 regu_num;

	_rtw_memset(&hal_conf, 0, sizeof(hal_conf));

	hal_conf.force = true;
	for (band = 0; band < BAND_MAX; band++) {
		if (!names_of_band[band] || !names_len_of_band[band])
			continue;

		regu_num = 0;
		ustrs_for_each_str(names_of_band[band], names_len_of_band[band], name)
				regu_num++;
		if (!regu_num)
			continue;

		regu = rtw_malloc(regu_num);
		if (!regu)
			break;

		if (band == BAND_ON_24G) {
			hal_conf.regu_2g = regu;
			hal_conf.regu_2g_len = regu_num;
		} else if (band == BAND_ON_5G) {
			hal_conf.regu_5g = regu;
			hal_conf.regu_5g_len = regu_num;
		} else if (band == BAND_ON_6G) {
			hal_conf.regu_6g = regu;
			hal_conf.regu_6g_len = regu_num;
		} else {
			rtw_mfree(regu, regu_num);
			continue;
		}

		regu_num = 0;
		ustrs_for_each_str(names_of_band[band], names_len_of_band[band], name) {
			if (rtw_txpwr_hal_lmt_reg_search(dvobj, band, name)) {
				hal_regu = rtw_phl_get_pw_lmt_regu_type_from_str(GET_PHL_INFO(dvobj), name);
				regu[regu_num++] = hal_regu >= 0 ? hal_regu : hal_none_regu;
			} else
				regu[regu_num++] = hal_none_regu;
		}
	}

	rtw_phl_cmd_set_pw_lmt_regu(GET_PHL_INFO(dvobj), &hal_conf, true, PHL_CMD_DIRECTLY, 0);

	if (hal_conf.regu_2g)
		rtw_mfree(hal_conf.regu_2g, hal_conf.regu_2g_len);
	if (hal_conf.regu_5g)
		rtw_mfree(hal_conf.regu_5g, hal_conf.regu_5g_len);
	if (hal_conf.regu_6g)
		rtw_mfree(hal_conf.regu_6g, hal_conf.regu_6g_len);
}

void rtw_txpwr_hal_get_current_lmt_regs_name(struct dvobj_priv* dvobj, char *names_of_band[], int names_len_of_band[])
{
	struct txpwr_regu_info_t *hal_info;
	enum band_type band;
	u8 *reg;
	u8 reg_len;
	int i;
	const char *hal_name;

	for (band = 0; band < BAND_MAX; band++) {
		names_of_band[band] = NULL;
		names_len_of_band[band] = 0;
	}

	hal_info = rtw_phl_get_pw_lmt_regu_info(GET_PHL_INFO(dvobj));
	if (!hal_info) {
		RTW_ERR("%s rtw_phl_get_pw_lmt_regu_info return NULL\n", __func__);
		return;
	}

	for (band = 0; band < BAND_MAX; band++) {
		if (band == BAND_ON_24G) {
			reg = hal_info->regu_2g;
			reg_len = hal_info->regu_2g_len;
		} else if (band == BAND_ON_5G) {
			reg = hal_info->regu_5g;
			reg_len = hal_info->regu_5g_len;
		} else if (band == BAND_ON_6G) {
			reg = hal_info->regu_6g;
			reg_len = hal_info->regu_6g_len;
		} else
			continue;

		for (i = 0; i < reg_len; i++) {
			hal_name = rtw_phl_get_pw_lmt_regu_str_from_type(GET_PHL_INFO(dvobj), reg[i]);
			if (hal_name)
				ustrs_add(&names_of_band[band], &names_len_of_band[band], hal_name);
		}
	}

	rtw_phl_free_pw_lmt_regu_info(GET_PHL_INFO(dvobj), hal_info);
}
#endif

#ifdef CONFIG_DFS_MASTER
void rtw_dfs_hal_radar_detect_disable(struct dvobj_priv *dvobj, u8 band_idx)
{
	rtw_phl_cmd_dfs_rd_disable(GET_PHL_INFO(dvobj), band_idx, PHL_CMD_DIRECTLY, 0);
}

void rtw_dfs_hal_radar_detect_enable(struct dvobj_priv *dvobj, u8 band_idx, bool cac, u32 rd_freq_hi, u32 rd_freq_lo)
{
	rtw_phl_cmd_dfs_rd_enable_with_sp_freq_range(GET_PHL_INFO(dvobj), band_idx, cac, rd_freq_hi, rd_freq_lo, PHL_CMD_DIRECTLY, 0);
}

void rtw_dfs_hal_set_cac_status(struct dvobj_priv *dvobj, u8 band_idx, bool cac)
{
	rtw_phl_cmd_dfs_rd_set_cac_status(GET_PHL_INFO(dvobj), band_idx, cac, PHL_CMD_DIRECTLY, 0);
}

void rtw_dfs_hal_csa_mg_tx_pause(struct dvobj_priv *dvobj, u8 band_idx, bool pause)
{
	rtw_phl_cmd_dfs_csa_mg_tx_pause(GET_PHL_INFO(dvobj), band_idx, pause, PHL_CMD_DIRECTLY, 0);
}

static const enum dfs_regd_t _rtw_dfs_regd_to_phl[RTW_DFS_REGD_NUM] = {
	/* elements not listed here will get DFS_REGD_UNKNOWN(0) */
	[RTW_DFS_REGD_NONE]	= DFS_REGD_UNKNOWN,
	[RTW_DFS_REGD_FCC]	= DFS_REGD_FCC,
	[RTW_DFS_REGD_MKK]	= DFS_REGD_JAP,
	[RTW_DFS_REGD_ETSI]	= DFS_REGD_ETSI,
	[RTW_DFS_REGD_KCC]	= DFS_REGD_KCC,
};

#define rtw_dfs_regd_to_phl(region) (((region) >= RTW_DFS_REGD_NUM) ? _rtw_dfs_regd_to_phl[RTW_DFS_REGD_NONE] : _rtw_dfs_regd_to_phl[(region)])

bool rtw_dfs_hal_region_supported(struct dvobj_priv* dvobj, enum rtw_dfs_regd domain)
{
	return rtw_dfs_regd_to_phl(domain) != DFS_REGD_UNKNOWN;
}

void rtw_dfs_hal_update_region(struct dvobj_priv *dvobj, u8 band_idx, enum rtw_dfs_regd domain)
{
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);

	rtw_phl_cmd_dfs_change_domain(GET_PHL_INFO(dvobj), band_idx, rtw_dfs_regd_to_phl(domain), PHL_CMD_DIRECTLY, 0);
}

u8 rtw_dfs_hal_radar_detect_polling_int_ms(struct dvobj_priv *dvobj)
{
	return 100;
}
#endif /* CONFIG_DFS_MASTER */

bool rtw_txpwr_hal_get_pwr_lmt_en(struct dvobj_priv *dvobj)
{
	return rtw_phl_get_pwr_lmt_en(GET_PHL_INFO(dvobj), HW_BAND_0);
}

bool rtw_txpwr_hal_get_ext_info(struct dvobj_priv *dvobj, struct tx_power_ext_info *info)
{
	void *phl_info = GET_PHL_INFO(dvobj);
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);
	u8 band_idx = HW_BAND_0;
	struct phy_sw_cap_t *sw_cap = &phl_com->phy_sw_cap[band_idx];

	if (!rtw_hw_is_init_completed(dvobj))
		return false;

	SET_TXPWR_PARAM_STATUS(&info->by_rate
		, phl_com->dev_cap.pwrbyrate_off == RTW_PW_BY_RATE_ON
		, phl_com->dev_cap.pwrbyrate_off == RTW_PW_BY_RATE_ON
		, sw_cap->rf_txpwr_byrate_info.para_src == RTW_PARA_SRC_EXTNAL);

	SET_TXPWR_PARAM_STATUS(&info->lmt
		, rtw_phl_get_pwr_lmt_en(phl_info, band_idx)
		, rtw_phl_get_pwr_lmt_en(phl_info, band_idx)
		, sw_cap->rf_txpwrlmt_info.para_src == RTW_PARA_SRC_EXTNAL);

#ifdef CONFIG_80211AX_HE
	SET_TXPWR_PARAM_STATUS(&info->lmt_ru
		, rtw_phl_get_pwr_lmt_en(phl_info, band_idx)
		, rtw_phl_get_pwr_lmt_en(phl_info, band_idx)
		, sw_cap->rf_txpwrlmt_ru_info.para_src == RTW_PARA_SRC_EXTNAL);
#endif

#if CONFIG_IEEE80211_BAND_6GHZ
	SET_TXPWR_PARAM_STATUS(&info->lmt_6g
		, rtw_phl_get_pwr_lmt_en(phl_info, band_idx)
		, rtw_phl_get_pwr_lmt_en(phl_info, band_idx)
		, sw_cap->rf_txpwrlmt_6g_info.para_src == RTW_PARA_SRC_EXTNAL);

	SET_TXPWR_PARAM_STATUS(&info->lmt_ru_6g
		, rtw_phl_get_pwr_lmt_en(phl_info, band_idx)
		, rtw_phl_get_pwr_lmt_en(phl_info, band_idx)
		, sw_cap->rf_txpwrlmt_ru_6g_info.para_src == RTW_PARA_SRC_EXTNAL);
#endif

	return true;
}

void rtw_txpwr_hal_update_pwr(struct dvobj_priv *dvobj, enum phl_band_idx band_idx)
{
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);
	struct txpwr_ctl_param args;
	int i;

	txpwr_ctl_param_init(&args);
	args.force_write_txpwr = true;
	args.constraint_mb = rfctl->tpc_mode == TPC_MODE_MANUAL ? rfctl->tpc_manual_constraint : 0;

	for (i = HW_BAND_0; i < HW_BAND_MAX; i++) {
		if (band_idx < HW_BAND_MAX && band_idx != i)
			continue;
		args.band_idx = i;
		rtw_phl_cmd_txpwr_ctl(GET_PHL_INFO(dvobj), &args, PHL_CMD_DIRECTLY, 0);
	}
}

inline u8 get_phy_tx_nss(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	u8 txss = 0;

	if (adapter_link->wrlink)
		txss = GET_PHY_TX_NSS_BY_BAND(adapter, adapter_link->wrlink->hw_band);
	else
		rtw_warn_on(1);

	return txss;
}

inline u8 get_phy_rx_nss(_adapter *adapter, struct _ADAPTER_LINK *adapter_link)
{
	u8 rxss = 0;

	if (adapter_link->wrlink)
		rxss = GET_PHY_RX_NSS_BY_BAND(adapter, adapter_link->wrlink->hw_band);
	else
		rtw_warn_on(1);

	return rxss;
}
