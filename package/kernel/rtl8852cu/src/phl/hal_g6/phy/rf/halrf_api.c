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

 
#include "halrf_precomp.h"

u8 halrf_kpath(struct rf_info *rf, enum phl_phy_idx phy_idx) {

	RF_DBG(rf, DBG_RF_RFK, "[RFK]dbcc_en: %x,  PHY%d\n", rf->hal_com->dbcc_en, phy_idx);

	if (!rf->hal_com->dbcc_en) {
		return RF_AB;
	} else {
		if (phy_idx == HW_PHY_0)
			return RF_A;
		else
			return RF_B;
	}
}

u32 phlrf_psd_log2base(struct rf_info *rf, u32 val)
{
	u32 j;
	u32 tmp, tmp2, val_integerd_b = 0, tindex, shiftcount = 0;
	u32 result, val_fractiond_b = 0;
	u32 table_fraction[21] = {
		0, 432, 332, 274, 232, 200, 174, 151, 132, 115,
		100, 86, 74, 62, 51, 42, 32, 23, 15, 7, 0};

	if (val == 0)
		return 0;

	tmp = val;

	while (1) {
		if (tmp == 1)
			break;

		tmp = (tmp >> 1);
		shiftcount++;
	}

	val_integerd_b = shiftcount + 1;

	tmp2 = 1;
	for (j = 1; j <= val_integerd_b; j++)
		tmp2 = tmp2 * 2;

	tmp = (val * 100) / tmp2;
	tindex = tmp / 5;

	if (tindex > 20)
		tindex = 20;

	val_fractiond_b = table_fraction[tindex];

	result = val_integerd_b * 100 - val_fractiond_b;

	return result;
}

void phlrf_rf_lna_setting(struct rf_info *rf, enum phlrf_lna_set type)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
			case CHIP_WIFI6_8852A:
				break;
#endif
			default:
				break;
		}

}

void halrf_bkp(struct rf_info *rf, u32 *bp_reg, u32 *bp, u32 reg_num)
{
	u32 i;

	for (i = 0; i < reg_num; i++)
		bp[i] = halrf_rreg(rf, bp_reg[i], MASKDWORD);
}

void halrf_bkprf(struct rf_info *rf, u32 *bp_reg, u32 bp[][4], u32 reg_num, u32 path_num)
{
	u32 i, j;

	for (i = 0; i < reg_num; i++) {
		for (j = 0; j < path_num; j++)
			bp[i][j] = halrf_rrf(rf, j, bp_reg[i], MASKRF);
	}
}

void halrf_reload_bkp(struct rf_info *rf, u32 *bp_reg, u32 *bp, u32 reg_num)
{
	u32 i;

	for (i = 0; i < reg_num; i++)
		halrf_wreg(rf, bp_reg[i], MASKDWORD, bp[i]);
}

void halrf_reload_bkprf(struct rf_info *rf,
		       u32 *bp_reg,
		       u32 bp[][4],
		       u32 reg_num,
		       u8 path_num)
{
	u32 i, path;

	for (i = 0; i < reg_num; i++) {
		for (path = 0; path < path_num; path++)
			halrf_wrf(rf, (enum rf_path)path, bp_reg[i],
				       MASKRF, bp[i][path]);
	}
}

void halrf_wait_rx_mode(struct rf_info *rf, u8 kpath)
{
	u8 path, rf_mode = 0;
	u16 count = 0;

	for (path = 0; path < 4; path++) {
		if (kpath & BIT(path)) {
			rf_mode = (u8)halrf_rrf(rf, path, 0x00, MASKRFMODE);

			while (rf_mode == 2 && count < 2500) {
				rf_mode = (u8)halrf_rrf(rf, path, 0x00, MASKRFMODE);
				halrf_delay_us(rf, 2);
				count++;
			}
			RF_DBG(rf, DBG_RF_RFK,
			       "[RFK] Wait S%d to Rx mode!! (count = %d)\n", path, count);
		}
	}
}

void halrf_tmac_tx_pause(struct rf_info *rf, enum phl_phy_idx band_idx, bool is_pause)
{
	halrf_tx_pause(rf, band_idx, is_pause, PAUSE_RSON_RFK);

	RF_DBG(rf, DBG_RF_RFK,"[RFK] Band%d Tx Pause %s!!\n",
	       band_idx, is_pause ? "on" : "off");

	if (is_pause)
		halrf_wait_rx_mode(rf, halrf_kpath(rf, band_idx));
}

void halrf_trigger_thermal(struct rf_info *rf)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_trigger_thermal_8852a(rf, RF_PATH_A);
			halrf_trigger_thermal_8852a(rf, RF_PATH_B);
			break;
#endif
		default:
			break;
	}
}

u8 halrf_only_get_thermal(struct rf_info *rf, enum rf_path path)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			return halrf_only_get_thermal_8852a(rf, path);
			break;
#endif
		default:
			break;
	}

	return 0;
}

void halrf_btc_rfk_ntfy(struct rf_info *rf, u8 phy_map, enum halrf_rfk_type type,
			enum halrf_rfk_process process)
{
	u32 cnt = 0;
	u8 band;
	/*idx : use BIT mask for RF path PATH A: 1, PATH B:2, PATH AB:3*/

	band = rf->hal_com->band[(phy_map & 0x30) >> 5].cur_chandef.band;

	phy_map = (band << 6) | phy_map;

	RF_DBG(rf, DBG_RF_RFK, "[RFK] RFK notify (%s / PHY%d / K_type = %d / path_idx = %d / process = %s)\n",
		band == 0 ? "2G" : (band == 1 ? "5G" : "6G"), (phy_map & 0x30) >> 5, type,
		phy_map & 0xf, process == 0 ? "RFK_STOP" : (process == 1 ? "RFK_START" :
		(process == 2 ? "ONE-SHOT_START" : "ONE-SHOT_STOP")));
#if 1
	if (process == RFK_START && rf->is_bt_iqk_timeout == false) {
		while (halrf_btc_ntfy(rf, phy_map, type, process) == 0 && cnt < 2500) {
			halrf_delay_us(rf, 40);
			cnt++;
		}
		if (cnt == 2500) {
			RF_DBG(rf, DBG_RF_RFK, "[RFK] Wait BT IQK timeout!!!!\n");
			rf->is_bt_iqk_timeout = true;
		}
	} else
		halrf_btc_ntfy(rf, phy_map, type, process);
#endif
}

void halrf_fcs_init(struct rf_info *rf)
{
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_fcs_init_8852a(rf);
#endif
}

void halrf_fast_chl_sw_backup(struct rf_info *rf, u8 chl_index, u8 t_index)
{
	u32 t[2];

	t[0] = chl_index;
	t[1] = t_index;

	halrf_fill_h2c_cmd(rf, 8, FWCMD_H2C_BACKUP_RFK, 0xa, H2CB_TYPE_DATA, t);
	RF_DBG(rf, DBG_RF_RFK, "FWCMD_H2C_BACKUP_RFK chl=%d t=%d\n", chl_index, t_index);
}

void halrf_fast_chl_sw_reload(struct rf_info *rf, u8 chl_index, u8 t_index)
{
	u32 t[2];

	t[0] = chl_index;
	t[1] = t_index;

	halrf_fill_h2c_cmd(rf, 8, FWCMD_H2C_RELOAD_RFK, 0xa, H2CB_TYPE_DATA, t);
	RF_DBG(rf, DBG_RF_RFK, "FWCMD_H2C_RELOAD_RFK chl=%d t=%d\n", chl_index, t_index);
}

void  halrf_quick_check_rf(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_quick_check_rfrx_8852a(rf);
#endif
#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		halrf_quick_checkrf_8852b(rf);
#endif
#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		halrf_quick_checkrf_8852c(rf);
#endif
#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		halrf_quick_checkrf_8851b(rf);
#endif
}

bool halrf_check_if_dowatchdog(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;

	return rf->is_watchdog_stop;
}

void  halrf_watchdog_stop(struct rf_info *rf, bool is_stop) {
	if(is_stop)
		rf->is_watchdog_stop = true;
	else
		rf->is_watchdog_stop = false;
	RF_DBG(rf, DBG_RF_RFK, "is_watchdog_stop=%d\n", rf->is_watchdog_stop);
}

void halrf_wifi_event_notify(void *rf_void,
			enum phl_msg_evt_id event, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_pwr_info *pwr = &rf->pwr_info;

	switch (event) {
		case MSG_EVT_SCAN_START:
			halrf_tssi_default_txagc(rf, phy_idx, true);
			halrf_tssi_set_avg(rf, phy_idx, true);
			halrf_dpk_track_onoff(rf, false);
			halrf_rx_dck_track_onoff(rf, false);
		break;
		case MSG_EVT_SCAN_END:
			halrf_tssi_default_txagc(rf, phy_idx, false);
			halrf_tssi_set_avg(rf, phy_idx, false);
			halrf_dpk_track_onoff(rf, true);
			halrf_rx_dck_track_onoff(rf, true);
		break;
		case MSG_EVT_DBG_RX_DUMP:
			halrf_quick_check_rf(rf);
		break;
		case MSG_EVT_DBG_TX_DUMP:
			halrf_quick_check_rf(rf);
		break;
		case MSG_EVT_SWCH_START:
			halrf_tssi_backup_txagc(rf, phy_idx, true);
		break;
		case MSG_EVT_MCC_START:
			halrf_watchdog_stop(rf, true);
		break;
		case MSG_EVT_MCC_STOP:
			halrf_watchdog_stop(rf, false);
		break;
#if 0
		case MSG_EVT_HAL_INIT_OK:
			halrf_set_power(rf, HW_PHY_0, PWR_BY_RATE);
			halrf_tssi_trigger(rf, HW_PHY_0, false);
		break;
		case MSG_EVT_SET_PWR_LIMIT_LOW:
			pwr->power_limit_6g_type = PW_LMT_6G_LOW;
			halrf_config_power_limit_6g(rf, HW_PHY_0);
			halrf_config_power_limit_ru_6g(rf, HW_PHY_0);
			if (rf->hal_com->dbcc_en)
				halrf_set_power(rf, HW_PHY_1, (PWR_LIMIT & PWR_LIMIT_RU));
			halrf_set_power(rf, HW_PHY_0, (PWR_LIMIT & PWR_LIMIT_RU));
		break;
		case MSG_EVT_SET_PWR_LIMIT_STD:
			pwr->power_limit_6g_type = PW_LMT_6G_STD;
			halrf_config_power_limit_6g(rf, HW_PHY_0);
			halrf_config_power_limit_ru_6g(rf, HW_PHY_0);
			if (rf->hal_com->dbcc_en)
				halrf_set_power(rf, HW_PHY_1, (PWR_LIMIT & PWR_LIMIT_RU));
			halrf_set_power(rf, HW_PHY_0, (PWR_LIMIT & PWR_LIMIT_RU));
		break;
#endif
		default:
		break;
	}
}

void halrf_write_fwofld_start(struct rf_info *rf)
{
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	bool fw_ofld = rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0);

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	fw_ofld = 1;
#endif

	rf->fw_ofld_enable = true;

	RF_DBG(rf, DBG_RF_FW, "======> %s   fw_ofld=%d   rf->fw_ofld_enable=%d\n",
		__func__, fw_ofld, rf->fw_ofld_enable);
#endif
}

void halrf_write_fwofld_trigger(struct rf_info *rf)
{
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	struct rtw_mac_cmd cmd = {0};
	bool fw_ofld = rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0);
	u32 rtn;
	u32 i;

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	fw_ofld = 1;
#endif

	if (rf->fw_ofld_enable == false)
		return;

	RF_DBG(rf, DBG_RF_FW,
		"[FW]write count in h2c=%d\n",rf->fw_w_count - rf->pre_fw_w_count);

	if ((rf->fw_w_count - rf->pre_fw_w_count) == 0) {
		RF_DBG(rf, DBG_RF_FW,
		"[FW]SKIP h2c trigger, os delay %d us\n",rf->fw_delay_us_count);

		for (i = 0; i < rf->fw_delay_us_count; i++)
			halrf_os_delay_us(rf, 1);	
		
		rf->fw_delay_us_count = 0;
		return;
	}


	cmd.type = RTW_MAC_DELAY_OFLD;
	cmd.lc = 1;
	cmd.value = 1;

	if (fw_ofld) {
		rtn = halrf_mac_add_cmd_ofld(rf, &cmd);
		if (rtn) {
			RF_WARNING("======>%s return fail error code = %d !!!\n",
				__func__, rtn);
		}
	}
	rf->fw_ofld_start = false;
	rf->sw_trigger_count++;
	rf->pre_fw_w_count = rf->fw_w_count;

	rf->fw_delay_us_count = 0;
#endif
}

void halrf_write_fwofld_end(struct rf_info *rf)
{
#if defined(HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT) || defined(HALRF_CONFIG_FW_IO_OFLD_SUPPORT)
	bool fw_ofld = rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0);

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	fw_ofld = 1;
#endif

	if (fw_ofld) {
		if (rf->fw_ofld_start == true)
			halrf_write_fwofld_trigger(rf);
		rf->fw_ofld_enable = false;
		
	}
#endif
}

void halrf_ctrl_bw_ch(void *rf_void, enum phl_phy_idx phy, u8 central_ch,
				enum band_type band, enum channel_width bw)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	u32 start_time, finish_time;

	start_time = _os_get_cur_time_us();

	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_lock(rf, &rf->rf_lock);
	halrf_ctl_ch(rf, phy, central_ch, band);
	halrf_ctl_bw(rf, phy, bw);
	halrf_rxbb_bw(rf, phy, bw);
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_unlock(rf, &rf->rf_lock);

	finish_time = _os_get_cur_time_us();
	rf->set_ch_bw_time = HALRF_ABS(finish_time, start_time) / 1000;	
}

u32 halrf_test_event_trigger(void *rf_void,
		enum phl_phy_idx phy, enum halrf_event_idx idx, enum halrf_event_func func) {

	struct rf_info *rf = (struct rf_info *)rf_void;

	switch (idx) {
		case RF_EVENT_PWR_TRK:
			if (func == RF_EVENT_OFF)
				halrf_tssi_disable(rf, phy);
			else if (func == RF_EVENT_ON)
				halrf_tssi_enable(rf, phy);
			else if (func == RF_EVENT_TRIGGER)
				halrf_tssi_trigger(rf, phy, true);
		break;

		case RF_EVENT_IQK:
			if (func == RF_EVENT_OFF)
				halrf_iqk_onoff(rf, true);
			else if (func == RF_EVENT_ON)
				halrf_iqk_onoff(rf, false);
			else if (func == RF_EVENT_TRIGGER) {
				halrf_nbiqk_enable(rf, false); 		
				halrf_iqk_trigger(rf, phy, false);
			}
		break;

		case RF_EVENT_DPK:
			if (func == RF_EVENT_OFF)
				halrf_dpk_onoff(rf, false);
			else if (func == RF_EVENT_ON)
				halrf_dpk_onoff(rf, true);
			else if (func == RF_EVENT_TRIGGER)
				halrf_dpk_trigger(rf, phy, false);
		break;

		case RF_EVENT_TXGAPK:
			if (func == RF_EVENT_OFF)
				halrf_gapk_disable(rf, phy);
			else if (func == RF_EVENT_ON)
				halrf_gapk_enable(rf, phy);
			else if (func == RF_EVENT_TRIGGER)
				halrf_gapk_trigger(rf, phy, true);
		break;

		case RF_EVENT_DACK:
			if (func == RF_EVENT_OFF)
				halrf_dack_onoff(rf, false);
			else if (func == RF_EVENT_ON)
				halrf_dack_onoff(rf, true);
			else if (func == RF_EVENT_TRIGGER)
				halrf_dack_trigger(rf, true);
		break;

		default:
		break;
	}
	return 0;
}

void halrf_mcc_info_init(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_mcc_info *mcc_info = &rf->mcc_info;
	u8 idx;

	if(!mcc_info->is_init) {
		RF_DBG(rf, DBG_RF_RFK, "[MCC info]======> %s \n", __func__);

		mcc_info->is_init = true;
	
		for (idx = 0; idx < 2; idx++) { //channel
			mcc_info->ch[idx] = 0;
			mcc_info->band[idx] = 0;
		}
		mcc_info->table_idx = 0;		
	}
}

void halrf_mcc_get_ch_info(void *rf_void, enum phl_phy_idx phy) 
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_mcc_info *mcc_info = &rf->mcc_info;
	u8 idx;
	u8 get_empty_table = false;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	halrf_mcc_info_init(rf, phy);
#if 0	
	//get channel info
	for  (idx = 0;  idx < 2; idx++) {
		if (mcc_info->ch[idx] == 0) {
			get_empty_table = true;
			break;
		}
	}

	if (false == get_empty_table) {
		idx = mcc_info->table_idx + 1;
		if (idx > 1) {
			idx = 0;
		}			
	}	

	mcc_info->table_idx = idx;
#endif
	idx = mcc_info->table_idx;
	mcc_info->ch[idx] = rf->hal_com->band[phy].cur_chandef.center_ch;
#ifdef RF_8852C_SUPPORT
	if (hal_i->chip_id == CHIP_WIFI6_8852C)
		mcc_info->band[idx] = rf->hal_com->band[phy].cur_chandef.band;
#endif	
}

void halrf_chlk_backup_dbcc(struct rf_info *rf, enum phl_phy_idx phy) 
{
	struct halrf_dbcc_info *dbcc_info = &rf->dbcc_info;
	u8 kpath, idx;

	RF_DBG(rf, DBG_RF_RFK, "[DBCC]======> %s \n", __func__);
	kpath = halrf_kpath(rf, phy);
	idx = dbcc_info->table_idx;

	if (kpath & BIT(0)) {
		rf->kip_table[idx][0] = halrf_rreg(rf, 0x8104, MASKDWORD);
		rf->kip_table[idx][1] = halrf_rreg(rf, 0x8154, MASKDWORD);
		RF_DBG(rf, DBG_RF_RFK, "[DBCC]S0 backup idx=%d\n", idx);
	}
	if (kpath & BIT(1)) {
		rf->kip_table[idx][2] = halrf_rreg(rf, 0x8204, MASKDWORD);
		rf->kip_table[idx][3] = halrf_rreg(rf, 0x8254, MASKDWORD);
		RF_DBG(rf, DBG_RF_RFK, "[DBCC]S1 backup idx=%d\n", idx);
	}
	RF_DBG(rf, DBG_RF_RFK, "[DBCC]kip_table00=0x%x,kip_table01=0x%x,kip_table02=0x%x,kip_table03=0x%x\n",
		rf->kip_table[0][0],rf->kip_table[0][1],
		rf->kip_table[0][2],rf->kip_table[0][3]);
	RF_DBG(rf, DBG_RF_RFK, "[DBCC]kip_table10=0x%x,kip_table11=0x%x,kip_table12=0x%x,kip_table13=0x%x\n",
		rf->kip_table[1][0],rf->kip_table[1][1],
		rf->kip_table[1][2],rf->kip_table[1][3]);
}

void halrf_chlk_reload_dbcc(struct rf_info *rf, enum phl_phy_idx phy, u8 idx) 
{
	struct halrf_dbcc_info *dbcc_info = &rf->dbcc_info;
	u8 kpath;

	RF_DBG(rf, DBG_RF_RFK, "[DBCC]======> %s \n", __func__);
	kpath = halrf_kpath(rf, phy);

	if (kpath & BIT(0)) {
		halrf_wreg(rf, 0x8104, MASKDWORD, rf->kip_table[idx][0]);
		halrf_wreg(rf, 0x8154, MASKDWORD, rf->kip_table[idx][1]);
		RF_DBG(rf, DBG_RF_RFK, "[DBCC]S0 reload idx=%x\n", idx);
	}

	if (kpath & BIT(1)) {
		halrf_wreg(rf, 0x8204, MASKDWORD, rf->kip_table[idx][2]);
		halrf_wreg(rf, 0x8254, MASKDWORD, rf->kip_table[idx][3]);
		RF_DBG(rf, DBG_RF_RFK, "[DBCC]S1 reload idx=%x\n", idx);
	}
}


bool halrf_chlk_reload_check_dbcc(struct rf_info *rf, enum phl_phy_idx phy) 
{
	struct halrf_dbcc_info *dbcc_info = &rf->dbcc_info;
	struct halrf_mcc_info *mcc_info = &rf->mcc_info;
	u8 path, i, j, idx, kpath, kch, kband;
	bool reload = false;

	RF_DBG(rf, DBG_RF_RFK, "[DBCC]======> %s \n", __func__);
	kpath = halrf_kpath(rf, phy);
	kch = rf->hal_com->band[phy].cur_chandef.center_ch;
	kband = rf->hal_com->band[phy].cur_chandef.band;
	idx = dbcc_info->table_idx;
	RF_DBG(rf, DBG_RF_RFK, "[DBCC]dbcc_en=%d  prek_is_dbcc=%d\n", rf->hal_com->dbcc_en, dbcc_info->prek_is_dbcc);
	if (rf->hal_com->dbcc_en || dbcc_info->prek_is_dbcc) {
		//try reload
		for(i = 0; i < 2; i++) {
			if (kpath == RF_AB) {
				if (kch == dbcc_info->ch[i][1] && kband == dbcc_info->band[i][1] &&
					kch == dbcc_info->ch[i][0] && kband == dbcc_info->band[i][0]) {
					idx = i;
					reload = true;
				}
			} else {
				if (kpath == RF_A)
					path = 0;
				else
					path = 1;
				if (kch == dbcc_info->ch[i][path] && kband == dbcc_info->band[i][path]) {
					idx = i;
					reload = true;
				}
			}
		}
		if (reload) {
			halrf_chlk_reload_dbcc(rf, phy, idx);
			rf->chlk_map = 0xffffffff & (~HAL_RF_IQK) & (~HAL_RF_DPK);
			RF_DBG(rf, DBG_RF_RFK, "[DBCC]reload kpath=%d, index=%d\n", kpath, idx);
			RF_DBG(rf, DBG_RF_RFK, "[DBCC]table0 S0 ch=%5d S1 ch=%5d\n", dbcc_info->ch[0][0], dbcc_info->ch[0][1]);
			RF_DBG(rf, DBG_RF_RFK, "[DBCC]table1 S0 ch=%5d S1 ch=%5d\n", dbcc_info->ch[1][0], dbcc_info->ch[1][1]);
			RF_DBG(rf, DBG_RF_RFK, "[DBCC]table0 S0 band =%5d S1 band =%5d\n", dbcc_info->band[0][0], dbcc_info->band[0][1]);
			RF_DBG(rf, DBG_RF_RFK, "[DBCC]table1 S0 band =%5d S1 band =%5d\n", dbcc_info->band[1][0], dbcc_info->band[1][1]);
			return reload;
		}
	}
	//force K
	for  (i = 0;  i < 2; i++)
		if (dbcc_info->ch[i][0] == 0 && dbcc_info->ch[i][1] == 0)
			break;
	if (i < 2) {
		idx = i;
	} else {
		for  (j = 0;  j < 2; j++)
			if (dbcc_info->ch[j][0] != dbcc_info->ch[j][1] ||
				dbcc_info->band[j][0] != dbcc_info->band[j][1])
				break;
		if (j == 2) {
			idx++;
			if (idx > 1)
				idx = 0;
		} else {
			idx = j;
		}
	}
	rf->chlk_map = 0xffffffff;
	dbcc_info->prek_is_dbcc = rf->hal_com->dbcc_en;
	dbcc_info->table_idx = idx;
	mcc_info->table_idx = idx;
	for (path = 0; path < 2; path++) {
		if (kpath & BIT(path)) {
			dbcc_info->ch[idx][path] = kch;
			dbcc_info->band[idx][path] = kband;
		}
	}
	RF_DBG(rf, DBG_RF_RFK, "[DBCC]foreK kpath=%d, index=%d\n", kpath, idx);
	RF_DBG(rf, DBG_RF_RFK, "[DBCC]ch00=%d ch01=%d ch10=%d ch11=%d\n",
		dbcc_info->ch[0][0], dbcc_info->ch[0][1], dbcc_info->ch[1][0], dbcc_info->ch[1][1]);
	RF_DBG(rf, DBG_RF_RFK, "[DBCC]band00=%d band01=%d band10=%d band10=%d\n",
		dbcc_info->band[0][0], dbcc_info->band[0][1], dbcc_info->band[1][0], dbcc_info->band[1][1]);

	return reload;
}

void halrf_reset_io_count(struct rf_info *rf)
{
	rf->w_count = 0;
	rf->r_count = 0;
	rf->fw_w_count = 0;
	rf->fw_r_count = 0;
	rf->sw_trigger_count = 0;
	rf->pre_fw_w_count = 0;
}
