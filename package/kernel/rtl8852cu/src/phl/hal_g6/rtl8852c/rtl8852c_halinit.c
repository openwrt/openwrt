/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#define _RTL8852C_HALINIT_C_
#include "rtl8852c_hal.h"

void init_hal_spec_8852c(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal)
{
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_com);
	struct rtw_hal_com_t *hal_com = hal->hal_com;
	struct protocol_cap_t *hw_proto_cap = hal_com->proto_hw_cap;
	struct dev_cap_t *dev_hw_cap = &hal_com->dev_hw_cap;

	hal_spec->ic_name = "rtl8852c";
	hal_spec->macid_num = hal_mac_get_macid_num(hal);
	hal_com->dev_hw_cap.macid_num = hal_mac_get_macid_num(hal);
	/* hal_spec->sec_cam_ent_num follow halmac setting */
	hal_spec->sec_cap = SEC_CAP_CHK_BMC;

	hal_spec->rfpath_num_2g = 2;
	hal_spec->rfpath_num_5g = 2;
	hal_spec->rf_reg_path_num = 2;
	hal_com->phy_hw_cap[0].rx_num = 2;
	hal_com->phy_hw_cap[0].tx_num = 2;
	hal_com->phy_hw_cap[0].rx_path_num = 2;
	hal_com->phy_hw_cap[0].tx_path_num = 2;
	hal_com->phy_hw_cap[1].rx_num = 2;
	hal_com->phy_hw_cap[1].tx_num = 2;
	hal_com->phy_hw_cap[1].rx_path_num = 2;
	hal_com->phy_hw_cap[1].tx_path_num = 2;
	hal_com->phy_hw_cap[0].hw_rts_time_th = 0;
	hal_com->phy_hw_cap[1].hw_rts_time_th = 0;
	hal_com->phy_hw_cap[0].hw_rts_len_th = 0;
	hal_com->phy_hw_cap[1].hw_rts_len_th = 0;
	hal_spec->max_tx_cnt = 2;
	dev_hw_cap->band_sup = BAND_CAP_2G | BAND_CAP_5G | BAND_CAP_6G;
	dev_hw_cap->bw_sup = BW_CAP_20M | BW_CAP_40M | BW_CAP_80M | BW_CAP_160M;
	hal_spec->port_num = 5;
	hal_spec->wmm_num = 2;

	hal_spec->proto_cap = PROTO_CAP_11B | PROTO_CAP_11G | PROTO_CAP_11N |
				PROTO_CAP_11AC | PROTO_CAP_11AX;

	hal_spec->wl_func = 0
				| WL_FUNC_P2P
				| WL_FUNC_MIRACAST
				| WL_FUNC_TDLS
				;

	hal_spec->max_csi_buf_su_nr = 4;
	hal_spec->max_csi_buf_mu_nr = 6;
	hal_spec->max_bf_ent_nr = 16;
	hal_spec->max_su_sta_nr = 16;
	hal_spec->max_mu_sta_nr = 6;

	hal_spec->max_std_entry_num = 8;
	hal_spec->max_tmp_entry_num = 8;
#ifdef RTW_WKARD_PHY_CAP
	/* HE */
	hw_proto_cap[0].he_su_bfme = 1;
	hw_proto_cap[0].he_su_bfmr = 1;
	hw_proto_cap[0].he_mu_bfme = 1;
	hw_proto_cap[0].he_mu_bfmr = 1;

	hw_proto_cap[1].he_su_bfme = 1;
	hw_proto_cap[1].he_su_bfmr = 0;
	hw_proto_cap[1].he_mu_bfme = 1;
	hw_proto_cap[1].he_mu_bfmr = 0;

	hw_proto_cap[0].trig_cqi_fb = 1;
	hw_proto_cap[0].non_trig_cqi_fb = 1;
	hw_proto_cap[1].trig_cqi_fb = 1;
	hw_proto_cap[1].non_trig_cqi_fb = 1;

	/* VHT */
	hw_proto_cap[0].vht_su_bfmr = 1;
	hw_proto_cap[0].vht_su_bfme = 1;
	hw_proto_cap[0].vht_mu_bfmr = 1;
	hw_proto_cap[0].vht_mu_bfme = 1;

	hw_proto_cap[1].vht_su_bfmr = 0;
	hw_proto_cap[1].vht_su_bfme = 1;
	hw_proto_cap[1].vht_mu_bfmr = 0;
	hw_proto_cap[1].vht_mu_bfme = 1;

	/* HT */
	hw_proto_cap[0].ht_su_bfmr = 1;
	hw_proto_cap[0].ht_su_bfme = 1;

	hw_proto_cap[1].ht_su_bfmr = 0;
	hw_proto_cap[1].ht_su_bfme = 1;

	/* STBC Tx*/
	hw_proto_cap[0].stbc_ht_tx = 1;
	hw_proto_cap[1].stbc_ht_tx = 0;
	hw_proto_cap[0].stbc_vht_tx = 1;
	hw_proto_cap[1].stbc_vht_tx = 0;
	hw_proto_cap[0].stbc_he_tx = 1;
	hw_proto_cap[1].stbc_he_tx = 0;
	hw_proto_cap[0].stbc_tx_greater_80mhz = 0;
	hw_proto_cap[1].stbc_tx_greater_80mhz = 0;

	/* STBC Rx*/
	hw_proto_cap[0].stbc_ht_rx = 1;
	hw_proto_cap[1].stbc_ht_rx = 1;
	hw_proto_cap[0].stbc_vht_rx = 1;
	hw_proto_cap[1].stbc_vht_rx = 1;
	hw_proto_cap[0].stbc_he_rx = 1;
	hw_proto_cap[1].stbc_he_rx = 1;
	hw_proto_cap[0].stbc_rx_greater_80mhz = 0;
	hw_proto_cap[1].stbc_rx_greater_80mhz = 0;
#endif

	/*get mac capability*/
	phl_com->dev_cap.hw_sup_flags = HW_SUP_DBCC |
			HW_SUP_AMSDU |
			HW_SUP_TCP_TX_CHKSUM |
			HW_SUP_TXPKT_CONVR;

	phl_com->dev_cap.hw_sup_flags |= HW_SUP_TCP_RX_CHKSUM;

	phl_com->dev_cap.hw_sup_flags |= HW_SUP_OFDMA | HW_SUP_CHAN_INFO;
	phl_com->dev_cap.hw_sup_flags |= HW_SUP_TSSI | HW_SUP_TANK_K;


#ifdef RTW_WKARD_LAMODE
	hal_com->dev_hw_cap.la_mode = true;/*TODO : get info from halbb*/
#endif

#ifdef CONFIG_DBCC_SUPPORT
	if (phl_com->dev_cap.hw_sup_flags & HW_SUP_DBCC)
		hal_com->dev_hw_cap.dbcc_sup = true;/*get info from efuse*/
#endif
	hal_com->dev_hw_cap.hw_hdr_conv = true;
	hal_com->dev_hw_cap.hw_amsdu = true;
	hal_com->dev_hw_cap.tx_mu_ru = true;

	hal_com->dev_hw_cap.sec_cap.hw_form_hdr = true;
	hal_com->dev_hw_cap.sec_cap.hw_tx_search_key = true;
	hal_com->dev_hw_cap.sec_cap.hw_sec_iv = true;

#ifdef CONFIG_MCC_SUPPORT
	hal_com->dev_hw_cap.mcc_sup = true;
#endif /* CONFIG_MCC_SUPPORT */

	hal_com->dev_hw_cap.sta_ulru = RTW_HW_CAP_ULRU_ENABLE;
#ifdef RTW_WKARD_BB_DISABLE_STA_2G40M_ULOFDMA
	hal_com->dev_hw_cap.sta_ulru_2g40mhz = RTW_HW_CAP_ULRU_DISABLE;
#endif

#ifdef CONFIG_PHL_TWT
	hal_com->dev_hw_cap.twt_sup = RTW_PHL_TWT_REQ_SUP | RTW_PHL_TWT_RSP_SUP;
#endif /* CONFIG_PHL_TWT */

	hal_com->dev_hw_cap.ps_cap.ips_cap = PS_CAP_PWR_OFF |
		PS_CAP_PWRON | PS_CAP_RF_OFF | PS_CAP_CLK_GATED | PS_CAP_PWR_GATED;
	hal_com->dev_hw_cap.ps_cap.ips_wow_cap =
		PS_CAP_PWRON | PS_CAP_RF_OFF | PS_CAP_CLK_GATED | PS_CAP_PWR_GATED;
	hal_com->dev_hw_cap.ps_cap.lps_cap =
		PS_CAP_PWRON | PS_CAP_RF_OFF | PS_CAP_CLK_GATED |
		(hal_com->cv == CAV ? 0 : PS_CAP_PWR_GATED);
	hal_com->dev_hw_cap.ps_cap.lps_wow_cap =
		PS_CAP_PWRON | PS_CAP_RF_OFF | PS_CAP_CLK_GATED | PS_CAP_PWR_GATED;
	hal_com->dev_hw_cap.ps_cap.bcn_tracking = true;
	hal_com->dev_hw_cap.hw_stype_cap = EFUSE_HW_STYPE_NONE_8852C;
	hal_com->dev_hw_cap.wl_func_cap = EFUSE_WL_FUNC_NONE;

	hal_com->dev_hw_cap.sgi_160_sup = true;
	hal_com->dev_hw_cap.rpq_agg_num = 0;

	hal_com->dev_hw_cap.max_link_num = RTW_ONE_LINK;
	hal_com->dev_hw_cap.max_mld_num = 0;

	hal_com->dev_hw_cap.drv_info_sup = RTW_DEV_CAP_ENABLE;
	hal_com->dev_hw_cap.bfee_rx_ndp_sts = 7;
}


void init_default_value_8852c(struct hal_info_t *hal)
{

	struct rtw_hal_com_t *hal_com = hal->hal_com;
	struct rtw_chan_def *chandef = NULL;
	u8 bid = 0;

	for (bid = 0; bid < HW_BAND_MAX; bid++) {

		chandef = &(hal_com->band[bid].cur_chandef);
		chandef->bw = CHANNEL_WIDTH_MAX;
		chandef->chan = 0;
		chandef->offset = CHAN_OFFSET_NO_EXT;
	}
}

u32 _hal_cfg_rom_fw_8852c(enum rtw_fw_type fw_type, struct rtw_fw_info_t *fw_info,
			  char *ic_name)
{
	char *hal_phy_folder = FW_FILE_CONFIG_PATH;
	char *filename_postfix = "";

	switch (fw_type) {
	case RTW_FW_NIC:
		filename_postfix = FW_FILE_NIC_POSTFIX;
		break;
	case RTW_FW_WOWLAN:
		filename_postfix = FW_FILE_WOWLAN_POSTFIX;
		break;
	case RTW_FW_SPIC:
		filename_postfix = FW_FILE_SPIC_POSTFIX;
		break;
	case RTW_FW_AP:
		filename_postfix = FW_FILE_AP_POSTFIX;
		break;
	default:
		break;
	}

	_os_snprintf(fw_info->rom_path, MAX_PATH_LEN, "%s%s%s%s", hal_phy_folder,
		     ic_name, _os_path_sep, "rtl8852cfw_rom.bin");

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s : %s\n", __func__, fw_info->rom_path);

	fw_info->rom_size = _os_read_file(fw_info->rom_path,
					  fw_info->rom_buff,
					  RTW_MAX_FW_SIZE);
	if (!fw_info->rom_size)
		return RTW_HAL_STATUS_FAILURE;

	return RTW_HAL_STATUS_SUCCESS;
}

u32 _hal_cfg_intnal_fw_8852c(struct rtw_phl_com_t *phl_com, enum rtw_fw_type fw_type, struct rtw_fw_info_t *fw_info)
{
	/* any related to fw from header can be defined here */
	return RTW_HAL_STATUS_SUCCESS;
}

static u32 _hal_read_fw_8852c(enum rtw_fw_type fw_type,
	u8 *tgt_buf, u32 *tgt_buf_size, char *ic_name)
{
	char *hal_phy_folder = FW_FILE_CONFIG_PATH;
	char *filename_postfix = "";
	char path[256] = {0};

	if (tgt_buf == NULL)
		return RTW_HAL_STATUS_FAILURE;

	switch (fw_type) {
	case RTW_FW_NIC:
		filename_postfix = FW_FILE_NIC_POSTFIX;
		break;
	case RTW_FW_WOWLAN:
		filename_postfix = FW_FILE_WOWLAN_POSTFIX;
		break;
	case RTW_FW_SPIC:
		filename_postfix = FW_FILE_SPIC_POSTFIX;
		break;
	case RTW_FW_AP:
		filename_postfix = FW_FILE_AP_POSTFIX;
		break;
	default:
		return RTW_HAL_STATUS_FAILURE;
	}

	_os_snprintf(path, MAX_PATH_LEN, "%s%s%s%s%s%s", hal_phy_folder,
		     ic_name, _os_path_sep, "RTL8852Cfw", filename_postfix, ".bin");

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s : %s\n", __func__, path);

	if (0 == *tgt_buf_size) {
		*tgt_buf_size = _os_read_file(path, tgt_buf, RTW_MAX_FW_SIZE);
		/* reading fw file failed */
		if (0 == *tgt_buf_size)
			return RTW_HAL_STATUS_FAILURE;
	}

	return RTW_HAL_STATUS_SUCCESS;
}

static u32 _hal_cfg_extnal_fw_8852c(struct rtw_phl_com_t *phl_com,
	enum rtw_fw_type fw_type, struct rtw_fw_info_t *fw_info, char *ic_name)
{
	void *d = phlcom_to_drvpriv(phl_com);
	u8 *tgt_buf = NULL;
	u32 tgt_buf_size = 0;

	switch (fw_type) {
	case RTW_FW_NIC:
		_hal_read_fw_8852c(RTW_FW_NIC, fw_info->buf,
			&fw_info->buf_size, ic_name);
		/* preload wowlan fw */
		_hal_read_fw_8852c(RTW_FW_WOWLAN, fw_info->wow_buf,
			&fw_info->wow_buf_size, ic_name);
		/* target is still NIC */
		tgt_buf = fw_info->buf;
		tgt_buf_size = fw_info->buf_size;
		break;
	case RTW_FW_WOWLAN:
		_hal_read_fw_8852c(RTW_FW_WOWLAN, fw_info->wow_buf,
			&fw_info->wow_buf_size, ic_name);
		tgt_buf = fw_info->wow_buf;
		tgt_buf_size = fw_info->wow_buf_size;
		break;
	case RTW_FW_SPIC:
		_hal_read_fw_8852c(RTW_FW_SPIC, fw_info->buf,
			&fw_info->buf_size, ic_name);
		tgt_buf = fw_info->buf;
		tgt_buf_size = fw_info->buf_size;
		break;
	case RTW_FW_AP:
		_hal_read_fw_8852c(RTW_FW_AP, fw_info->buf,
			&fw_info->buf_size, ic_name);
		tgt_buf = fw_info->buf;
		tgt_buf_size = fw_info->buf_size;
		break;
	default:
		return RTW_HAL_STATUS_FAILURE;
	}

	/* fw has been downloaded */
	if (tgt_buf && (0 != tgt_buf_size)) {
		_os_mem_cpy(d, fw_info->ram_buff, tgt_buf,
						tgt_buf_size);
		fw_info->ram_size = tgt_buf_size;
		return RTW_HAL_STATUS_SUCCESS;
	}  else {
		fw_info->ram_size = 0;
		return _hal_read_fw_8852c(fw_type, fw_info->ram_buff,
			&fw_info->ram_size, ic_name);
	}

}

enum rtw_hal_status hal_cfg_fw_8852c(struct rtw_phl_com_t *phl_com,
				     struct hal_info_t *hal,
				     char *ic_name,
				     enum rtw_fw_type fw_type)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct rtw_fw_info_t *fw_info = &phl_com->fw_info;
	struct rtw_fw_cap_t *fw_cap = &phl_com->dev_cap.fw_cap;

	FUNCIN();

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s : fw_src %d.\n", __func__,
		fw_cap->fw_src);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s : dlram_en %d.\n", __func__,
		fw_cap->dlram_en);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s : dlrom_en %d.\n", __func__,
		fw_cap->dlrom_en);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s : fw_type %d.\n", __func__,
		fw_type);

	fw_info->fw_en = false;

	if (!fw_cap->dlram_en)
		return hstatus;

	/* Should handle fw src from header */
	if (fw_cap->dlrom_en) {
		if (RTW_HAL_STATUS_SUCCESS !=_hal_cfg_rom_fw_8852c(fw_type, fw_info,
			ic_name))
			goto init_fw_fail;
	} else {
		if(fw_info->rom_buff) {
			_os_mem_free(halcom_to_drvpriv(hal->hal_com), fw_info->rom_buff, RTW_MAX_FW_SIZE);
			fw_info->rom_buff = NULL;
		}
	}


	/* RAM */
	if (fw_cap->fw_src == RTW_FW_SRC_EXTNAL) {
		fw_info->fw_src = RTW_FW_SRC_EXTNAL;
		if (fw_info->ram_buff == NULL) {
			fw_info->ram_buff = _os_mem_alloc(halcom_to_drvpriv(hal->hal_com), RTW_MAX_FW_SIZE);
			if (fw_info->ram_buff == NULL) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s : Allocate ram_buff fail.\n", __func__);
				goto init_fw_fail;
			}
		}
		if (RTW_HAL_STATUS_SUCCESS != _hal_cfg_extnal_fw_8852c(phl_com, fw_type,
			fw_info, ic_name))
			goto init_fw_fail;
	} else if (fw_cap->fw_src == RTW_FW_SRC_INTNAL) {
		fw_info->fw_src = RTW_FW_SRC_INTNAL;
		if (fw_info->ram_buff) {
			_os_mem_free(halcom_to_drvpriv(hal->hal_com), fw_info->ram_buff, RTW_MAX_FW_SIZE);
			fw_info->ram_buff = NULL;
		}
		if (RTW_HAL_STATUS_SUCCESS != _hal_cfg_intnal_fw_8852c(phl_com, fw_type,
			fw_info))
			goto init_fw_fail;
	} else {
		goto init_fw_fail;
	}

	fw_info->fw_type = fw_type;
	fw_info->fw_en = true;
	fw_info->dlram_en = fw_cap->dlram_en;
	fw_info->dlrom_en = fw_cap->dlrom_en;

	/* fw_en, dlram_en, dlrom_en, ram_buff, ram_size, rom_buff, rom_size are ready here. */

	hstatus = RTW_HAL_STATUS_SUCCESS;

init_fw_fail:
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s : fw_en %d.\n", __func__, fw_info->fw_en);
	return hstatus;
}

enum rf_path
hal_get_path_from_ant_num_8852c(u8 antnum){
	enum rf_path ret = RF_PATH_B;

	switch (antnum) {
		default:
			break;
		case 1:
			ret = RF_PATH_B;
			break;
		case 2:
			ret = RF_PATH_AB;
			break;
		case 3:
			ret = RF_PATH_ABC;
			break;
	}
	return ret;
}

enum rtw_hal_status hal_get_efuse_8852c(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal,
					struct hal_init_info_t *init_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	FUNCIN();

	hal_status = rtw_hal_mac_hal_fast_init(phl_com, hal, init_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		goto hal_fast_init_fail;

	rtw_hal_efuse_process(phl_com, hal, init_info->ic_name);

	hal_status = rtw_hal_mac_power_switch(phl_com, hal, 0);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		goto hal_power_off_fail;

	FUNCOUT();

	return RTW_HAL_STATUS_SUCCESS;

hal_power_off_fail:
hal_fast_init_fail:
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "==> %s : hal get efuse fail\n", __func__);
	return hal_status;
}

enum rtw_hal_status hal_start_8852c(struct rtw_phl_com_t *phl_com,
				   struct hal_info_t *hal,
				   struct hal_init_info_t *init_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct phy_cap_t *phy_cap = phl_com->phy_cap;
	u8 val = 0;

	/* Read phy parameter files */
	rtw_hal_dl_all_para_file(phl_com, init_info->ic_name, hal);

	hal_status = rtw_hal_mac_hal_init(phl_com, hal, init_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		goto hal_init_fail;

#ifdef PHL_FEATURE_AP
	rtw_hal_set_rxfltr_opt_by_mode(hal, HW_BAND_0, RX_FLTR_OPT_MODE_AP_NORMAL);
#else /* PHL_FEATURE_AP */
	rtw_hal_set_rxfltr_opt_by_mode(hal, HW_BAND_0, RX_FLTR_OPT_MODE_STA_NORMAL);
#endif /* PHL_FEATURE_AP */
	rtw_hal_set_rxfltr_type_by_mode(hal, HW_BAND_0, RX_FLTR_TYPE_MODE_HAL_INIT);
	/* MAC Suggested : 11264 Byte */
	rtw_hal_mac_set_rxfltr_mpdu_size(hal->hal_com, HW_BAND_0, 0x2c00);
	rtw_hal_mac_set_hw_rts_th(hal, HW_BAND_0,
				  phy_cap[HW_BAND_0].hw_rts_time_th,
				  phy_cap[HW_BAND_0].hw_rts_len_th);
	/*update phy cap of tx agg info */
	rtw_hal_mac_init_txagg_num(hal);
	/* tmp entry init */
	hal_status = rtw_hal_init_tmp_entry(hal, phl_com->hal_spec.max_tmp_entry_num,
	                                     phl_com->hal_spec.max_std_entry_num);
	if (hal_status != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("rtw_hal_init_tmp_entry fail!\n");
		goto hal_init_fail;
	}
	if (hal->hal_com->dbcc_en == true) {
		rtw_hal_set_rxfltr_opt_by_mode(hal, HW_BAND_1, RX_FLTR_OPT_MODE_STA_NORMAL);
		rtw_hal_set_rxfltr_type_by_mode(hal, HW_BAND_1, RX_FLTR_TYPE_MODE_HAL_INIT);
		rtw_hal_mac_set_rxfltr_mpdu_size(hal->hal_com, HW_BAND_1, 0x2c00);
		rtw_hal_mac_set_hw_rts_th(hal, HW_BAND_1,
					  phy_cap[HW_BAND_1].hw_rts_time_th,
					  phy_cap[HW_BAND_1].hw_rts_len_th);
	}

#ifdef CONFIG_BTCOEX
	/* power on config for btc */
	rtw_hal_btc_power_on_ntfy(hal);
#endif

#ifndef PHL_FEATURE_AP
	/* EFUSE config */
	rtw_hal_efuse_process(phl_com, hal, init_info->ic_name);
#endif
	/*update final cap of txagg info*/
	rtw_hal_final_cap_decision(phl_com, hal);

	/*[Pre-config BB/RF] BBRST / RFC reset */
	rtw_hal_mac_enable_bb_rf(hal, 0);
	rtw_hal_mac_enable_bb_rf(hal, 1);

	/* load parameters or config mac, phy, btc, ... */
#ifdef USE_TRUE_PHY
	rtw_hal_init_bb_reg(hal);
	rtw_hal_init_rf_reg(phl_com, hal);
#ifdef CONFIG_DBCC_SUPPORT
	if (hal->hal_com->dbcc_en == true)
		rtw_hal_phy_dbcc_cfg(hal, phl_com, true);
#endif
#endif

#ifdef CONFIG_BTCOEX
	/* After mac/bb/rf initialized, set btc config */
	rtw_hal_btc_init_coex_cfg_ntfy(hal);
#endif
	/* start watchdog/dm */
	rtw_hal_bb_dm_init(hal);
	rtw_hal_rf_dm_init(hal);

	hal_status = rtw_hal_mac_get_append_fcs(hal, &val);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		goto hal_init_fail;
	else
		phl_com->append_fcs = val;

	hal_status = rtw_hal_mac_get_acpt_icv_err(hal, &val);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		goto hal_init_fail;
	else
		phl_com->accept_icv_err = val;

#ifdef RTW_WKARD_HW_MGNT_GCMP_256_DISABLE
	rtw_hal_mac_config_hw_mgnt_sec(hal, false);
#endif

	/* Enable PPDU STS in default for BAND-0 for phy status */
	PHL_INFO("==> Default ENABLE RX_PPDU_STS for Band0\n");
	hal_status = rtw_hal_ppdu_sts_init(hal, HW_BAND_0, true,
					     HAL_PPDU_MAC_INFO | HAL_PPDU_PLCP | HAL_PPDU_RX_CNT,
					     HAL_PPDU_HAS_CRC_OK | HAL_PPDU_HAS_DMA_OK);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		goto hal_init_fail;
	phl_com->ppdu_sts_info.en_ppdu_sts[HW_BAND_0] = true;

#ifdef CONFIG_DBCC_SUPPORT
	/*Enable PPDU STS in force-dbcc for BAND-1 for phy status */
	if (hal->hal_com->dbcc_en == true) {
		PHL_INFO("==> Default ENABLE RX_PPDU_STS for Band1\n");
		hal_status = rtw_hal_ppdu_sts_init(hal, HW_BAND_1, true,
					     HAL_PPDU_MAC_INFO | HAL_PPDU_PLCP | HAL_PPDU_RX_CNT,
					     HAL_PPDU_HAS_CRC_OK | HAL_PPDU_HAS_DMA_OK);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			goto hal_init_fail;
		phl_com->ppdu_sts_info.en_ppdu_sts[HW_BAND_1] = true;
	}
#endif

	if (RTW_DEV_CAP_ENABLE == phl_com->dev_cap.drv_info_sup) {
		/* Enable DrvInfo for per pkt rssi */
		hal_status = rtw_hal_mac_drvinfo_cfg(hal, true, HW_BAND_0);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			goto hal_init_fail;
	}

	hal_status = rtw_hal_hdr_conv_cfg(hal, phl_com->dev_cap.hw_hdr_conv);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		goto hal_init_fail;

#ifdef CONFIG_RTW_TX_HW_AMSDU_SW_MERGE_MODE
	val = 1;	// for 8852C sw merge mode testing
#else
	val = 0;
#endif

	hal_status = rtw_hal_mac_enable_hwamsdu(hal, phl_com->dev_cap.hw_amsdu, 4, val, 0);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		goto hal_init_fail;

	/* Enable FW basic logs */
	hal_fw_en_basic_log(hal->hal_com);

	return RTW_HAL_STATUS_SUCCESS;

hal_init_fail:
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "==> %s : hal init fail\n", __func__);
	return hal_status;
}

enum rtw_hal_status hal_stop_8852c(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

#ifdef CONFIG_BTCOEX
	/* power off config for btc */
	rtw_hal_btc_power_off_ntfy(hal);
#endif
	hal_status = rtw_hal_mac_hal_deinit(phl_com, hal);
	rtw_hal_bb_dm_deinit(phl_com, hal);

	return hal_status;
}

#ifdef CONFIG_WOWLAN
enum rtw_hal_status
hal_wow_init_8852c(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal_info,
                   struct rtw_phl_stainfo_t *sta,
                   struct hal_init_info_t *init_info)
{
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	bool linked = sta->rlink->mstate == MLME_LINKED ? true : false;

	hal_status = hal_ops->hal_cfg_fw(phl_com, hal_info, init_info->ic_name,
	                                 RTW_FW_WOWLAN);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: cfg fw fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}

	rtw_phl_disable_interrupt_sync(phl_com);

	hal_status = rtw_hal_redownload_fw(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: redownload fw fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}

	rtw_phl_enable_interrupt_sync(phl_com);

	hal_status = rtw_hal_restore_sta_entry(phl_com, hal_info, sta, linked);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: update sta entry fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}

#ifdef RTW_WKARD_HW_MGNT_GCMP_256_DISABLE
	rtw_hal_mac_config_hw_mgnt_sec(hal_info, true);
#endif

exit:
	return hal_status;
}

enum rtw_hal_status
hal_wow_deinit_8852c(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal_info,
                     struct rtw_phl_stainfo_t *sta,
                     struct hal_init_info_t *init_info)
{
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	bool linked = sta->rlink->mstate == MLME_LINKED ? true : false;
	/* AOAC Report */

	hal_status = hal_ops->hal_cfg_fw(phl_com, hal_info, init_info->ic_name,
	                                 RTW_FW_NIC);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: cfg fw fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}

	rtw_phl_disable_interrupt_sync(phl_com);

	hal_status = rtw_hal_redownload_fw(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: redownload fw fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}

	rtw_phl_enable_interrupt_sync(phl_com);

	hal_status = rtw_hal_restore_sta_entry(phl_com, hal_info, sta, linked);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: update sta entry fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}

#ifdef RTW_WKARD_HW_MGNT_GCMP_256_DISABLE
	rtw_hal_mac_config_hw_mgnt_sec(hal_info, false);
#endif

exit:
	return hal_status;
}
#endif /* CONFIG_WOWLAN */

#ifdef RTW_PHL_BCN //fill 8852c bcn ops

enum rtw_hal_status hal_config_beacon_8852c(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal, struct rtw_bcn_entry *bcn_entry)
{
	if(hal_mac_ax_config_beacon(hal, bcn_entry) == RTW_HAL_STATUS_FAILURE)
		return RTW_HAL_STATUS_FAILURE;

	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status hal_update_beacon_8852c(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal, struct rtw_bcn_entry *bcn_entry)
{
	if(hal_mac_ax_send_beacon(hal, bcn_entry) == RTW_HAL_STATUS_FAILURE)
		return RTW_HAL_STATUS_FAILURE;

	return RTW_HAL_STATUS_SUCCESS;
}
#endif /* RTW_PHL_BCN */

enum rtw_hal_status
hal_mp_init_8852c(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal_info,
					struct hal_init_info_t *init_info)
{
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	enum rtw_fw_type fw_type = RTW_FW_MAX;

#ifdef PHL_FEATURE_NIC
	fw_type = RTW_FW_NIC;
#elif defined(PHL_FEATURE_AP)
	fw_type = RTW_FW_AP;
#else
	fw_type  = RTW_FW_MAX;
#endif

	hal_status = hal_ops->hal_cfg_fw(phl_com, hal_info, init_info->ic_name, fw_type);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: cfg fw fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}

	hal_status = rtw_hal_redownload_fw(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: redownload fw fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}
exit:
	return hal_status;
}

enum rtw_hal_status
hal_mp_deinit_8852c(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal_info,
					struct hal_init_info_t *init_info)
{
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	enum rtw_fw_type fw_type = RTW_FW_MAX;

#ifdef PHL_FEATURE_NIC
	fw_type = RTW_FW_NIC;
#elif defined(PHL_FEATURE_AP)
	fw_type = RTW_FW_AP;
#else
	fw_type  = RTW_FW_MAX;
#endif

	hal_status = hal_ops->hal_cfg_fw(phl_com, hal_info, init_info->ic_name, fw_type);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: cfg fw fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}

	hal_status = rtw_hal_redownload_fw(phl_com, hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s: redownload fw fail(%d)!!\n", __func__, hal_status);
		goto exit;
	}

exit:
	return hal_status;
}
