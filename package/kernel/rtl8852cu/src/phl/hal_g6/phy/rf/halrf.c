/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#include "halrf_precomp.h"

#if 1

enum rtw_hal_status halrf_chl_rfk_trigger(void *rf_void,
			   enum phl_phy_idx phy_idx,
			   bool force)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	u32 start_time, finish_time;
	bool is_reload;

	start_time = _os_get_cur_time_us();
	rf->is_chl_rfk = true;
#if 0
	//if (rf->phl_com->dev_state & RTW_DEV_IN_DFS_CAC_PERIOD) {
	if (rf->phl_com->dev_state & BIT4) {
		//CAC time, only do RX dck
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_CHLK, RFK_START);
		halrf_tmac_tx_pause(rf, phy_idx, true);

		/*[RX dck]*/
		halrf_rx_dck_trigger(rf, phy_idx, true);
		
		halrf_tmac_tx_pause(rf, phy_idx, false);
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_CHLK, RFK_STOP);

		//halrf_fw_ntfy(rf, phy_idx);

		finish_time = _os_get_cur_time_us();
		RF_DBG(rf, DBG_RF_RFK, "[RX_DCK] halrf_chl_rfk_trigger only do RxDCK processing time = %d.%dms\n",
			HALRF_ABS(finish_time, start_time) / 1000,
			HALRF_ABS(finish_time, start_time) % 1000);
		
		return RTW_HAL_STATUS_SUCCESS;
	}
#endif



	halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_CHLK, RFK_START);
	halrf_tmac_tx_pause(rf, phy_idx, true);

	//if (rf->phl_com->dev_state & RTW_DEV_IN_DFS_CAC_PERIOD) {
	if (rf->phl_com->dev_state & BIT4) {
		/*[RX dck]*/
		halrf_rx_dck_trigger(rf, phy_idx, true);
	} else {
		/*DBCC check*/
		is_reload = halrf_chlk_reload_check_dbcc(rf, phy_idx);
		/*get mcc info*/
		halrf_mcc_get_ch_info(rf, phy_idx);

		/*[RX Gain K]*/
		halrf_do_rx_gain_k(rf, phy_idx);

		/*[TX GAP K]*/
		halrf_gapk_trigger(rf, phy_idx, true);
		
		/*[RX dck]*/
		halrf_rx_dck_trigger(rf, phy_idx, true);

		/*[LOK, IQK]*/
		halrf_iqk_trigger(rf, phy_idx, force);

		/*[TSSI Trk]*/
		halrf_tssi_trigger(rf, phy_idx, true);

		/*[DPK]*/
		halrf_dpk_trigger(rf, phy_idx, force);

		if(!is_reload)
			halrf_chlk_backup_dbcc(rf, phy_idx);

		halrf_fw_ntfy(rf, phy_idx);
	}
	halrf_tmac_tx_pause(rf, phy_idx, false);
	halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_CHLK, RFK_STOP);
	rf->is_chl_rfk = false;

	finish_time = _os_get_cur_time_us();
	rf->rfk_total_time = finish_time - start_time;

	RF_DBG(rf, DBG_RF_RFK, "[RFK] halrf_chl_rfk_trigger processing time = %d.%dms\n",
		HALRF_ABS(finish_time, start_time) / 1000,
		HALRF_ABS(finish_time, start_time) % 1000);
	return RTW_HAL_STATUS_SUCCESS;
}

void halrf_dack_recover(void *rf_void,
			u8 offset,
			enum rf_path path,
			u32 val,
			bool reload)
{

	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	#if 0
	if (phl_is_mp_mode(rf->phl_com) &
	    (hal_i->is_con_tx ||
	    hal_i->is_single_tone ||
	    hal_i->is_carrier_suppresion))
			return;

	if (hal_i->rfk_forbidden)
		return;
	#endif

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		if (hal_i->cv == CAV)
			halrf_dack_recover_8852a(rf, offset, path, val, reload);
		break;
#endif
		default:
		break;
	}
}

enum rtw_hal_status halrf_dack_trigger(void *rf_void, bool force)
{

	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_dack_info *dack = &rf->dack;
	u32 start_time, finish_time;

	#if 0
	if (phl_is_mp_mode(rf->phl_com) &
	    (hal_i->is_con_tx ||
	    hal_i->is_single_tone ||
	    hal_i->is_carrier_suppresion))
			return;

	if (hal_i->rfk_forbidden)
		return;
	#endif

	if (!(rf->support_ability & HAL_RF_DACK))
		return RTW_HAL_STATUS_SUCCESS;
#if 0
	halrf_btc_rfk_ntfy(rf, (BIT(HW_PHY_0) << 4), RF_BTC_DACK, RFK_START);
#endif
	start_time = _os_get_cur_time_us();
	halrf_reset_io_count(rf);

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		if (hal_i->cv == CAV)
			halrf_dac_cal_8852a(rf, force);
		else
			halrf_dac_cal_8852a_b(rf, force);			
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_dac_cal_8852b(rf, force);			
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_dac_cal_8852c(rf, force);			
		break;
#endif
#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
		halrf_dac_cal_8832br(rf, force);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
		halrf_dac_cal_8192xb(rf, force);
		break;
#endif
#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		halrf_dac_cal_8730a(rf, force);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_dac_cal_8852bp(rf, force); 		
			break;
#endif
#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_dac_cal_8851b(rf, force); 		
			break;
#endif
		default:
			break;
	}
	finish_time = _os_get_cur_time_us();
#if 0
	halrf_btc_rfk_ntfy(rf, (BIT(HW_PHY_0) << 4), RF_BTC_DACK, RFK_STOP);	
#endif
	dack->dack_time = HALRF_ABS(finish_time, start_time) / 1000;
	RF_DBG(rf, DBG_RF_RFK, "[DACK] DACK processing time = %d.%dms\n",
		HALRF_ABS(finish_time, start_time) / 1000,
		HALRF_ABS(finish_time, start_time) % 1000);

	RF_DBG(rf, DBG_RF_FW, "[FW]DACK w=%d r=%d, fw_w=%d, h2c=%d\n",
		rf->w_count, rf->r_count, rf->fw_w_count, rf->sw_trigger_count);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_rx_dck_trigger(void *rf_void,
						enum phl_phy_idx phy_idx, bool is_afe)
{

	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	u32 start_time, finish_time;
	u32 i;

	if (!(rf->chlk_map & HAL_RF_RXDCK))
		return RTW_HAL_STATUS_SUCCESS;


	if (!(rf->support_ability & HAL_RF_RXDCK))
		return RTW_HAL_STATUS_SUCCESS;

#if 0
	for (i = 0; i < 2000; i++)
		halrf_delay_us(rf, 10); /*delay 20ms*/
#endif
	rf->rfk_is_processing = true;

	start_time = _os_get_cur_time_us();
	if(!rf->is_chl_rfk) {
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_RXDCK, RFK_START);
		halrf_tmac_tx_pause(rf, phy_idx, true);
	}
	if (phl_is_mp_mode(rf->phl_com))	
		halrf_mutex_lock(rf, &rf->rf_lock);

	halrf_reset_io_count(rf);

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_rx_dck_8852a(rf, phy_idx, is_afe);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_rx_dck_8852b(rf, phy_idx, false);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_rx_dck_8852c(rf, phy_idx, false);
		break;
#endif
#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_rx_dck_8851b(rf, phy_idx, false);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_rx_dck_8832br(rf, phy_idx, false);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_rx_dck_8192xb(rf, phy_idx, true);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		if (!rf->is_chl_rfk) {
			for (i = 0; i < 2000; i++)
				halrf_delay_us(rf, 10); /*delay 20ms*/
		}
		halrf_rx_dck_8852bp(rf, phy_idx, true);
		break;
#endif
		default:
		break;
	}

	rf->rfk_is_processing = false;

	finish_time = _os_get_cur_time_us();
	if(!rf->is_chl_rfk) {
		halrf_tmac_tx_pause(rf, phy_idx, false);
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_RXDCK, RFK_STOP);
	}
	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] RX_DCK processing time = %d.%dms\n",
		HALRF_ABS(finish_time, start_time) / 1000,
		HALRF_ABS(finish_time, start_time) % 1000);
	RF_DBG(rf, DBG_RF_FW, "[FW]RX_DCK w=%d r=%d, fw_w=%d, h2c=%d\n",
		rf->w_count, rf->r_count, rf->fw_w_count, rf->sw_trigger_count);
	rx_dck->rxdck_time = HALRF_ABS(finish_time, start_time) / 1000;
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_unlock(rf, &rf->rf_lock);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_thermal_rx_dck_trigger(void *rf_void,
						enum phl_phy_idx phy_idx, bool is_afe)
{

	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	u32 start_time, finish_time;
	u32 i;

	if (!(rf->chlk_map & HAL_RF_RXDCK))
		return RTW_HAL_STATUS_SUCCESS;


	if (!(rf->support_ability & HAL_RF_RXDCK))
		return RTW_HAL_STATUS_SUCCESS;

#if 0
	for (i = 0; i < 2000; i++)
		halrf_delay_us(rf, 10); /*delay 20ms*/
#endif
	rf->rfk_is_processing = true;

	start_time = _os_get_cur_time_us();

	halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_RXDCK, RFK_START);
	halrf_tmac_tx_pause(rf, phy_idx, true);

	if (phl_is_mp_mode(rf->phl_com))	
		halrf_mutex_lock(rf, &rf->rf_lock);

	halrf_reset_io_count(rf);

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		for (i = 0; i < 2000; i++)
			halrf_delay_us(rf, 10); /*delay 20ms*/
		halrf_thermal_rx_dck_8852c(rf, phy_idx, false);
		break;
#endif
	default:
		break;
	}

	rf->rfk_is_processing = false;

	finish_time = _os_get_cur_time_us();

	halrf_tmac_tx_pause(rf, phy_idx, false);
	halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_RXDCK, RFK_STOP);

	RF_DBG(rf, DBG_RF_RXDCK, "[RX_DCK] RX_DCK (thermal) processing time = %d.%dms\n",
		HALRF_ABS(finish_time, start_time) / 1000,
		HALRF_ABS(finish_time, start_time) % 1000);
	RF_DBG(rf, DBG_RF_FW, "[FW]RX_DCK w=%d r=%d, fw_w=%d, h2c=%d\n",
		rf->w_count, rf->r_count, rf->fw_w_count, rf->sw_trigger_count);
	rx_dck->rxdck_time = HALRF_ABS(finish_time, start_time) / 1000;
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_unlock(rf, &rf->rf_lock);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_rx_dck_tracking(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	if (!(rf->support_ability & HAL_RF_RXDCK_TRACK) || rf->rfk_is_processing ||
		rf->is_watchdog_stop || rf->psd.psd_progress || !(rx_dck->is_rxdck_track_en))
		return RTW_HAL_STATUS_SUCCESS;

	halrf_watchdog_stop(rf, true); /*avoid race condition*/

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_rxdck_track_8852c(rf);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_rxdck_track_8852bp(rf);
		break;
#endif
		default:
		break;
	}

	halrf_watchdog_stop(rf, false);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_iqk_trigger(void *rf_void,
		       enum phl_phy_idx phy_idx,
		       bool force)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_iqk_info *iqk_info = &rf->iqk;
	u32 start_time, finish_time;

	if (!(rf->chlk_map & HAL_RF_IQK))
		return RTW_HAL_STATUS_SUCCESS;

	if (!(rf->support_ability & HAL_RF_IQK))
		return RTW_HAL_STATUS_SUCCESS;
	rf->rfk_is_processing = true;
	start_time = _os_get_cur_time_us();
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_lock(rf, &rf->rf_lock);

	halrf_reset_io_count(rf);

	if(!rf->is_chl_rfk) {
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_IQK, RFK_START);
		halrf_tmac_tx_pause(rf, phy_idx, true);
	}
	halrf_iqk_init(rf);
	halrf_iqk(rf, phy_idx, force);

	rf->rfk_is_processing = false;
	finish_time = _os_get_cur_time_us();
	if(!rf->is_chl_rfk) {
		halrf_tmac_tx_pause(rf, phy_idx, false);
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_IQK, RFK_STOP);
	}
	RF_DBG(rf, DBG_RF_IQK, "[IQK] IQK processing time = %d.%dms\n",
		HALRF_ABS(finish_time, start_time) / 1000,
		HALRF_ABS(finish_time, start_time) % 1000);
	RF_DBG(rf, DBG_RF_FW, "[FW]IQK w=%d r=%d, fw_w=%d, h2c=%d\n",
		rf->w_count, rf->r_count, rf->fw_w_count, rf->sw_trigger_count);
	iqk_info->time = HALRF_ABS(finish_time, start_time) / 1000;
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_unlock(rf, &rf->rf_lock);
	return RTW_HAL_STATUS_SUCCESS;
}

void halrf_lck_trigger(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	#if 0
	if (phl_is_mp_mode(rf->phl_com) &
	    (hal_i->is_con_tx ||
	    hal_i->is_single_tone ||
	    hal_i->is_carrier_suppresion))
			return;

	if (hal_i->rfk_forbidden)
		return;
	#endif

	if (!(rf->support_ability & HAL_RF_LCK))
		return;
		
	
	rf->lck_times++;	/*Total LCK Times*/

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_lck_8852c(rf);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_lck_8832br(rf);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
		halrf_lck_8192xb(rf);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_lck_8852bp(rf);
		break;
#endif
#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		halrf_lck_8730a(rf);
		break;
#endif

		default:
			break;
		}
}

void halrf_lck_tracking(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if (rf->rfk_is_processing || rf->is_watchdog_stop)
		return;

	if (!(rf->support_ability & HAL_RF_LCK))
		return;
		

	rf->is_watchdog_stop = true; /*avoid race condition*/
	
	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_lck_tracking_8852c(rf);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_lck_tracking_8832br(rf);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
		halrf_lck_tracking_8192xb(rf);
		break;
#endif
#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		halrf_lck_tracking_8730a(rf);
		break;
#endif

		default:
			break;
		}
	rf->is_watchdog_stop = false;
}

void halrf_op5k_init(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_op5k_info *op5k = &rf->op5k_info;

	op5k->record_bw = CHANNEL_WIDTH_MAX;
}

void halrf_op5k_trigger(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if (!(rf->support_ability & HAL_RF_OP5K))
		return;
	
	switch (hal_i->chip_id) {
#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_op5k_trigger_8832br(rf);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_op5k_trigger_8192xb(rf);
		break;
#endif
		default:
			break;
	}
}

void halrf_op5k_trigger_by_bw(void *rf_void, enum channel_width bw)
{
#if 0
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_op5k_info *op5k = &rf->op5k_info;

	if (!(rf->support_ability & HAL_RF_OP5K))
		return;
	
	RF_DBG(rf, DBG_RF_OP5K, "=======> %s bw = %d\n", __func__, bw);

	if (op5k->record_bw == bw) {
		RF_DBG(rf, DBG_RF_OP5K,
			"=======> %s op5k->record_bw == bw(%d) Return!!!\n",
			__func__, bw);
		return;
	}
	
	switch (hal_i->chip_id) {
#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_op5k_trigger_8832br(rf);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_op5k_trigger_8192xb(rf);
		break;
#endif
		default:
			break;
	}

	op5k->record_bw = bw;
#endif
}

void halrf_op5k_tracking(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if (rf->rfk_is_processing || rf->is_watchdog_stop)
		return;

	if (!(rf->support_ability & HAL_RF_OP5K_TRACK))
		return;

	rf->is_watchdog_stop = true; /*avoid race condition*/
	
	switch (hal_i->chip_id) {
#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_op5k_tracking_8832br(rf);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_op5k_tracking_8192xb(rf);
		break;
#endif
		default:
			break;
	}

	rf->is_watchdog_stop = false;
}

enum rtw_hal_status halrf_dpk_trigger(void *rf_void,
		       enum phl_phy_idx phy_idx,
		       bool force)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_dpk_info *dpk = &rf->dpk;

	u32 start_time, finish_time;

	if (!(rf->chlk_map & HAL_RF_DPK))
		return RTW_HAL_STATUS_SUCCESS;

	if (!(rf->support_ability & HAL_RF_DPK))
		return RTW_HAL_STATUS_SUCCESS;
	if(!rf->is_chl_rfk) {
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_DPK, RFK_START);
		halrf_tmac_tx_pause(rf, phy_idx, true);
	}
	rf->rfk_is_processing = true;

	start_time = _os_get_cur_time_us();
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_lock(rf, &rf->rf_lock);

	halrf_reset_io_count(rf);

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_dpk_8852a(rf, phy_idx, force);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_dpk_8852b(rf, phy_idx, force);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_dpk_8852c(rf, phy_idx, force);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_dpk_8852bp(rf, phy_idx, force);
		break;
#endif
#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_dpk_8851b(rf, phy_idx, force);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_dpk_8832br(rf, phy_idx, force);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_dpk_8192xb(rf, phy_idx, force);
		break;
#endif
#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		halrf_dpk_8730a(rf, phy_idx, force);
		break;
#endif

		default:
		break;
	}

	rf->rfk_is_processing = false;

	finish_time = _os_get_cur_time_us();
	if(!rf->is_chl_rfk) {
		halrf_tmac_tx_pause(rf, phy_idx, false);
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_DPK, RFK_STOP);
	}
	RF_DBG(rf, DBG_RF_DPK, "[DPK] DPK processing time = %d.%dms\n",
		HALRF_ABS(finish_time, start_time) / 1000,
		HALRF_ABS(finish_time, start_time) % 1000);
	RF_DBG(rf, DBG_RF_FW, "[FW]DPK w=%d r=%d, fw_w=%d, h2c=%d\n",
		rf->w_count, rf->r_count, rf->fw_w_count, rf->sw_trigger_count);
	dpk->dpk_time = HALRF_ABS(finish_time, start_time) / 1000;
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_unlock(rf, &rf->rf_lock);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_dpk_tracking(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_dpk_info *dpk = &rf->dpk;

	if (!(rf->support_ability & HAL_RF_DPK_TRACK) || rf->rfk_is_processing ||
		rf->is_watchdog_stop || rf->psd.psd_progress || !(dpk->is_dpk_track_en))
		return RTW_HAL_STATUS_SUCCESS;

	halrf_watchdog_stop(rf, true); /*avoid race condition*/

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_dpk_track_8852a(rf);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_dpk_track_8852b(rf);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_dpk_track_8852c(rf);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_dpk_track_8852bp(rf);
		break;
#endif
#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_dpk_track_8851b(rf);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_dpk_track_8832br(rf);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_dpk_track_8192xb(rf);
		break;
#endif
#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		halrf_dpk_track_8730a(rf);
		break;
#endif

		default:
		break;
	}

	halrf_watchdog_stop(rf, false);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_tssi_tracking(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if ((!(rf->support_ability & HAL_RF_TX_PWR_TRACK)) || (!(rf->support_ability & HAL_RF_TSSI_TRK))
		|| rf->rfk_is_processing || rf->is_watchdog_stop || rf->psd.psd_progress)
		return RTW_HAL_STATUS_SUCCESS;

	halrf_watchdog_stop(rf, true); /*avoid race condition*/

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_tssi_tracking_8852a(rf);
			break;
#endif
		default:
			break;
	}

	halrf_watchdog_stop(rf, false);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_tssi_tracking_clean(void *rf_void, s16 power_dbm)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if ((!(rf->support_ability & HAL_RF_TX_PWR_TRACK)) || (!(rf->support_ability & HAL_RF_TSSI_TRK))
		|| rf->rfk_is_processing || rf->is_watchdog_stop || rf->psd.psd_progress)
		return RTW_HAL_STATUS_SUCCESS;

	halrf_watchdog_stop(rf, true); /*avoid race condition*/

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_tssi_tracking_clean_8852ab(rf, power_dbm);
			break;
#endif
		default:
			break;
	}

	halrf_watchdog_stop(rf, false);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_tssi_ant_open(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if ((!(rf->support_ability & HAL_RF_TX_PWR_TRACK)) || (!(rf->support_ability & HAL_RF_TSSI_TRK))
		|| rf->rfk_is_processing || rf->is_watchdog_stop || rf->psd.psd_progress)
		return RTW_HAL_STATUS_SUCCESS;

	halrf_watchdog_stop(rf, true); /*avoid race condition*/

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_tssi_ant_open_8852a(rf);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_tssi_ant_open_8852b(rf);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_tssi_ant_open_8852c(rf);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_tssi_ant_open_8832br(rf);
			break;
#endif

#ifdef RF_8730A_SUPPORT
		case CHIP_WIFI6_8730A:
			halrf_tssi_ant_open_8730a(rf);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_tssi_ant_open_8852bp(rf);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_tssi_ant_open_8851b(rf);
			break;
#endif


		default:
			break;
	}

	halrf_watchdog_stop(rf, false);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_tssi_trigger(void *rf_void, enum phl_phy_idx phy_idx, bool hwtx_en)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_tssi_info *tssi_info = &rf->tssi;
	u32 phy_tmp;
	u32 start_time, finish_time;


	if (!(rf->chlk_map & HAL_RF_TX_PWR_TRACK))
		return RTW_HAL_STATUS_SUCCESS;

	start_time = _os_get_cur_time_us();
	
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_lock(rf, &rf->rf_lock);
	if (rf->hal_com->dbcc_en)
		phy_tmp = phy_idx;
	else
		phy_tmp = HW_PHY_0;

	if (phl_is_mp_mode(rf->phl_com)) {
		if (tssi_info->tssi_type[phy_tmp] == TSSI_OFF ) {
			rf->support_ability = rf->support_ability & ~HAL_RF_TX_PWR_TRACK;
		}
	}

	halrf_reset_io_count(rf);

	if (!(rf->support_ability & HAL_RF_TX_PWR_TRACK)) {
		switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
			case CHIP_WIFI6_8852A:
				halrf_tssi_disable_8852a(rf, phy_idx);
			break;
#endif
#ifdef RF_8852B_SUPPORT
			case CHIP_WIFI6_8852B:
				halrf_tssi_disable_8852b(rf, phy_idx);
			break;
#endif
#ifdef RF_8852C_SUPPORT
			case CHIP_WIFI6_8852C:
				halrf_tssi_disable_8852c(rf, phy_idx);
			break;
#endif
#ifdef RF_8832BR_SUPPORT
			case CHIP_WIFI6_8832BR:
				halrf_tssi_disable_8832br(rf, phy_idx);
			break;
#endif
#ifdef RF_8192XB_SUPPORT
			case CHIP_WIFI6_8192XB:
				halrf_tssi_disable_8192xb(rf, phy_idx);
			break;
#endif
#ifdef RF_8852BP_SUPPORT
			case CHIP_WIFI6_8852BP:
				halrf_tssi_disable_8852bp(rf, phy_idx);
			break;
#endif
#ifdef RF_8851B_SUPPORT
			case CHIP_WIFI6_8851B:
				halrf_tssi_disable_8851b(rf, phy_idx);
			break;
#endif
#ifdef RF_8730A_SUPPORT
			case CHIP_WIFI6_8730A:
				halrf_tssi_disable_8730a(rf, phy_idx);
			break;
#endif

			default:
			break;
		}
		if (phl_is_mp_mode(rf->phl_com))
			halrf_mutex_unlock(rf, &rf->rf_lock);
		return RTW_HAL_STATUS_SUCCESS;
	}

	rf->rfk_is_processing = true;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_do_tssi_8852a(rf, phy_idx, hwtx_en);
		break;
#endif
#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_do_tssi_8852b(rf, phy_idx, hwtx_en);
		break;
#endif
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_do_tssi_8852c(rf, phy_idx, hwtx_en);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_do_tssi_8832br(rf, phy_idx);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_do_tssi_8192xb(rf, phy_idx);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_do_tssi_8852bp(rf, phy_idx, hwtx_en);
		break;
#endif
#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_do_tssi_8851b(rf, phy_idx, hwtx_en);
		break;
#endif
#ifdef RF_8730A_SUPPORT
		case CHIP_WIFI6_8730A:
			halrf_do_tssi_8730a(rf, phy_idx);
		break;
#endif

		default:
		break;
	}

	rf->rfk_is_processing = false;

	finish_time = _os_get_cur_time_us();

	tssi_info->tssi_total_time = finish_time - start_time;

	RF_DBG(rf, DBG_RF_RFK, "[TSSI] %s processing time = %d.%dms\n",
		__func__,
		HALRF_ABS(finish_time, start_time) / 1000,
		HALRF_ABS(finish_time, start_time) % 1000);
	RF_DBG(rf, DBG_RF_FW, "[FW]TSSI w=%d r=%d, fw_w=%d, h2c=%d\n",
		rf->w_count, rf->r_count, rf->fw_w_count, rf->sw_trigger_count);
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_unlock(rf, &rf->rf_lock);
	return RTW_HAL_STATUS_SUCCESS;
}

void halrf_do_tssi_scan(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	u32 start_time, finish_time;

	if (!(rf->support_ability & HAL_RF_TX_PWR_TRACK))
		return;

	start_time = _os_get_cur_time_us();

	rf->rfk_is_processing = true;
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_do_tssi_scan_8852a(rf, phy_idx);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_do_tssi_scan_8852b(rf, phy_idx);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_do_tssi_scan_8852c(rf, phy_idx);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_do_tssi_scan_8832br(rf, phy_idx);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_do_tssi_scan_8192xb(rf, phy_idx);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_do_tssi_scan_8852bp(rf, phy_idx);
		break;
#endif
#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_do_tssi_scan_8851b(rf, phy_idx);
		break;
#endif

	default:
		break;
	}
	rf->rfk_is_processing = false;

	finish_time = _os_get_cur_time_us();

	RF_DBG(rf, DBG_RF_RFK, "[TSSI] %s processing time = %d.%dms\n",
		__func__,
		HALRF_ABS(finish_time, start_time) / 1000,
		HALRF_ABS(finish_time, start_time) % 1000);
}

void halrf_tssi_set_avg(void *rf_void,
					enum phl_phy_idx phy_idx, bool enable)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if (!(rf->support_ability & HAL_RF_TX_PWR_TRACK))
		return;

	rf->rfk_is_processing = true;
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_tssi_set_avg_8852ab(rf, phy_idx, enable);
		break;
#endif
	default:
		break;
	}
	rf->rfk_is_processing = false;
}

void halrf_tssi_default_txagc(void *rf_void,
					enum phl_phy_idx phy_idx, bool enable)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if (!(rf->support_ability & HAL_RF_TX_PWR_TRACK))
		return;

	rf->rfk_is_processing = true;
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_tssi_default_txagc_8852ab(rf, phy_idx, enable);
		break;
#endif

#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_tssi_default_txagc_8852b(rf, phy_idx, enable);
		break;
#endif

#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_tssi_default_txagc_8852c(rf, phy_idx, enable);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_tssi_default_txagc_8832br(rf, phy_idx, enable);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_tssi_default_txagc_8192xb(rf, phy_idx, enable);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_tssi_default_txagc_8852bp(rf, phy_idx, enable);
		break;
#endif

#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_tssi_default_txagc_8851b(rf, phy_idx, enable);
		break;
#endif

	default:
		break;
	}
	rf->rfk_is_processing = false;
}

void halrf_tssi_set_efuse_to_de(void *rf_void,
					enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if (!(rf->support_ability & HAL_RF_TX_PWR_TRACK))
		return;

	halrf_write_fwofld_start(rf);		/*FW Offload Start*/

	rf->rfk_is_processing = true;
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_tssi_set_efuse_to_de_8852ab(rf, phy_idx);
		break;
#endif

#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_tssi_set_efuse_to_de_8852b(rf, phy_idx);
		break;
#endif

#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_tssi_set_efuse_to_de_8852c(rf, phy_idx);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_tssi_set_efuse_to_de_8832br(rf, phy_idx);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_tssi_set_efuse_to_de_8192xb(rf, phy_idx);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_tssi_set_efuse_to_de_8852bp(rf, phy_idx);
		break;
#endif

#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_tssi_set_efuse_to_de_8851b(rf, phy_idx);
		break;
#endif

	default:
		break;
	}
	rf->rfk_is_processing = false;

	halrf_write_fwofld_end(rf);		/*FW Offload End*/
}

void halrf_tssi_scan_ch(void *rf_void, enum rf_path path)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if (!(rf->support_ability & HAL_RF_TX_PWR_TRACK))
		return;

	halrf_write_fwofld_start(rf);		/*FW Offload Start*/

	rf->rfk_is_processing = true;
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_tssi_scan_ch_8852ab(rf, path);
		break;
#endif

#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_tssi_scan_ch_8852b(rf, path);
		break;
#endif

#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_tssi_scan_ch_8852c(rf, path);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_tssi_scan_ch_8832br(rf, path);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_tssi_scan_ch_8192xb(rf, path);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_tssi_scan_ch_8852bp(rf, path);
		break;
#endif

#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_tssi_scan_ch_8851b(rf, path);
		break;
#endif


	default:
		break;
	}
	rf->rfk_is_processing = false;

	halrf_write_fwofld_end(rf);		/*FW Offload End*/
}

void halrf_tssi_scan_setting_ch(void *rf_void,
	enum phl_phy_idx phy_idx, enum rf_path path)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if (!(rf->support_ability & HAL_RF_TX_PWR_TRACK))
		return;

	halrf_write_fwofld_start(rf);		/*FW Offload Start*/

	rf->rfk_is_processing = true;
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_tssi_scan_ch_setting_8852ab(rf, phy_idx, path);
		break;
#endif

#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_tssi_scan_ch_setting_8852b(rf, phy_idx, path);
		break;
#endif

#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_tssi_scan_ch_setting_8852c(rf, phy_idx, path);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_tssi_scan_ch_setting_8832br(rf, phy_idx, path);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_tssi_scan_ch_setting_8192xb(rf, phy_idx, path);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_tssi_scan_ch_setting_8852bp(rf, phy_idx, path);
		break;
#endif

#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_tssi_scan_ch_setting_8851b(rf, phy_idx, path);
		break;
#endif

	default:
		break;
	}
	rf->rfk_is_processing = false;

	halrf_write_fwofld_end(rf);		/*FW Offload End*/
}

u32 halrf_tssi_get_current_power(void *rf_void, enum rf_path path)
{
	struct rf_info *rf = (struct rf_info *)rf_void;

	if (path == RF_PATH_A)
		return halrf_rreg(rf, 0x1c78, 0x1ff);
	else
		return halrf_rreg(rf, 0x3c78, 0x1ff);
}

void halrf_hw_tx(void *rf_void, u8 path, u16 cnt, s16 dbm, u32 rate, u8 bw,
				bool enable)

{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_tssi_hw_tx_8852a(rf, HW_PHY_0, path, cnt, dbm, HT_MF_FMT, 0, enable);
		break;
#endif

#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_tssi_hw_tx_8852b(rf, HW_PHY_0, path, cnt, dbm, HT_MF_FMT, 0, enable);
		break;
#endif

#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_tssi_hw_tx_8852c(rf, HW_PHY_0, path, cnt, dbm, HT_MF_FMT, 0, enable);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_tssi_hw_tx_8832br(rf, HW_PHY_0, path, cnt, dbm, HT_MF_FMT, 0, enable);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_tssi_hw_tx_8192xb(rf, HW_PHY_0, path, cnt, dbm, HT_MF_FMT, 0, enable);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_tssi_hw_tx_8852bp(rf, HW_PHY_0, path, cnt, dbm, HT_MF_FMT, 0, enable);
		break;
#endif

#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_tssi_hw_tx_8851b(rf, HW_PHY_0, path, cnt, dbm, HT_MF_FMT, 0, enable);
		break;
#endif

	default:
		break;
	}
}

void halrf_kfree_get_info(void *rf_void, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)

{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_kfree_get_info_8852a(rf, input, _used,
				output, _out_len);
		break;
#endif

#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_kfree_get_info_8852b(rf, input, _used,
				output, _out_len);
		break;
#endif

#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_kfree_get_info_8852c(rf, input, _used,
				output, _out_len);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_kfree_get_info_8832br(rf, input, _used,
				output, _out_len);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_kfree_get_info_8192xb(rf, input, _used,
				output, _out_len);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_kfree_get_info_8852bp(rf, input, _used,
				output, _out_len);
		break;
#endif

#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_kfree_get_info_8851b(rf, input, _used,
				output, _out_len);
		break;
#endif


	default:
		break;
	}
}

void halrf_txgapk_init(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		//halrf_txgapk_init_8852a(rf);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_txgapk_init_8852b(rf);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_txgapk_init_8852c(rf);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_txgapk_init_8832br(rf);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_txgapk_init_8192xb(rf);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_txgapk_init_8852bp(rf);
		break;
#endif


	default:
		break;
	}
}

enum rtw_hal_status halrf_gapk_trigger(void *rf_void,
			enum phl_phy_idx phy_idx,
			bool force)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_gapk_info *txgapk_info = &rf->gapk;

	u32 start_time, finish_time;

	if (!(rf->chlk_map & HAL_RF_TXGAPK))
		return RTW_HAL_STATUS_SUCCESS;
	if (!(rf->support_ability & HAL_RF_TXGAPK))
		return RTW_HAL_STATUS_SUCCESS;

//#ifdef HALRF_CONFIG_FW_IO_OFLD_SUPPORT
//	if (rf->phl_com->dev_cap.fw_cap.offload_cap & BIT(0))
//		return RTW_HAL_STATUS_SUCCESS;
//#endif

#ifdef HALRF_CONFIG_FW_DBCC_OFLD_SUPPORT
	return RTW_HAL_STATUS_SUCCESS;
#endif
	if(!rf->is_chl_rfk) {
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_TXGAPK, RFK_START);
		halrf_tmac_tx_pause(rf, phy_idx, true);
	}
	rf->rfk_is_processing = true;
	
	start_time = _os_get_cur_time_us();

	if(!txgapk_info->is_gapk_init)
		halrf_txgapk_init(rf);
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_lock(rf, &rf->rf_lock);

	halrf_reset_io_count(rf);
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_do_txgapk_8852a(rf, phy_idx);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_do_txgapk_8852b(rf, phy_idx);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_do_txgapk_8852c(rf, phy_idx);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_do_txgapk_8832br(rf, phy_idx);
		break;				
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_do_txgapk_8192xb(rf, phy_idx);
		break;				
#endif

#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		halrf_do_txgapk_8730a(rf, phy_idx);
		break;				
#endif

#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_do_txgapk_8852bp(rf, phy_idx);
		break;
#endif

	default:
		break;
	}

	rf->rfk_is_processing = false;

	finish_time = _os_get_cur_time_us();
	if(!rf->is_chl_rfk) {
		halrf_tmac_tx_pause(rf, phy_idx, false);
		halrf_btc_rfk_ntfy(rf, (BIT(phy_idx) << 4), RF_BTC_TXGAPK, RFK_STOP);
	}
	txgapk_info->txgapk_time = HALRF_ABS(finish_time, start_time) / 1000;
	
	RF_DBG(rf, DBG_RF_RFK, "[TXGAPK] %s processing time = %d.%dms\n",
		__func__,
		HALRF_ABS(finish_time, start_time) / 1000,
		HALRF_ABS(finish_time, start_time) % 1000);
	RF_DBG(rf, DBG_RF_FW, "[FW]gapk w=%d r=%d, fw_w=%d, h2c=%d\n",
		rf->w_count, rf->r_count, rf->fw_w_count, rf->sw_trigger_count);
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_unlock(rf, &rf->rf_lock);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status halrf_gapk_enable(void *rf_void,
			enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	rf->rfk_is_processing = true;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_txgapk_enable_8852a(rf, phy_idx);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_txgapk_enable_8852b(rf, phy_idx);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
	//	halrf_txgapk_enable_8852c(rf, phy_idx);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		//halrf_txgapk_enable_8832br(rf, phy_idx);
		//it needs to use trigger cmd
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		//halrf_txgapk_enable_8192xb(rf, phy_idx);
		//it needs to use trigger cmd
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_txgapk_enable_8852bp(rf, phy_idx);
		break;
#endif


		default:
		break;
	}

	rf->rfk_is_processing = false;

	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status halrf_gapk_disable(void *rf_void,
			enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	rf->rfk_is_processing = true;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_txgapk_write_table_default_8852a(rf, phy_idx);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_txgapk_write_table_default_8852b(rf, phy_idx);
		break;
#endif
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
		halrf_txgapk_write_table_default_8852c(rf, phy_idx);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
		halrf_txgapk_write_table_default_8832br(rf, phy_idx);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_txgapk_write_table_default_8192xb(rf, phy_idx);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_txgapk_write_table_default_8852bp(rf, phy_idx);
		break;
#endif

	default:
		break;
	}

	rf->rfk_is_processing = false;

	return RTW_HAL_STATUS_SUCCESS;
}


void halrf_rck_trigger(void *rf_void,
			enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	u8 path = 0;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		for (path = 0; path < 2; path++)
			halrf_rck_8852a(rf, path);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		for (path = 0; path < 2; path++)
			halrf_rck_8852b(rf, path);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		for (path = 0; path < 2; path++)
			halrf_rck_8852c(rf, path);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		for (path = 0; path < 2; path++)
			halrf_rck_8852bp(rf, path);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		for (path = 0; path < 2; path++)
			halrf_rck_8832br(rf, path);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		for (path = 0; path < 2; path++)
			halrf_rck_8192xb(rf, path);
		break;
#endif
#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		halrf_rck_8730a(rf);
		break;
#endif

	default:
		break;
	}
}

void halrf_gapk_save_tx_gain(struct rf_info *rf)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		//halrf_gapk_save_tx_gain_8852a(rf);
		break;
#endif
		default:
		break;
	}
}

void halrf_gapk_reload_tx_gain(struct rf_info *rf)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		//halrf_gapk_reload_tx_gain_8852a(rf);
		break;
#endif
		default:
		break;
	}
}



void halrf_lo_test(void *rf_void, bool is_on, enum rf_path path)
{

	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
#if 0
	if (hal_i->is_mp_mode &&
	    hal_i->is_con_tx &&
	    hal_i->is_single_tone &&
	    hal_i->is_carrier_suppresion)
			return;

	if (hal_i->rfk_forbidden)
		return;
#endif

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_lo_test_8852a(rf, is_on, path);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_lo_test_8852b(rf, is_on, path);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_lo_test_8852c(rf, is_on, path);
		break;
#endif
		default:
		break;
	}
}

void halrf_iqk_onoff(void *rf_void, bool is_enable)
{
	/*signal go throughput iqk or not*/	
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	u8 path = 0x0;
	
	if (!(rf->support_ability & HAL_RF_DPK))
			return;
	
	rf->rfk_is_processing = true;
	
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_iqk_onoff_8852ab(rf, is_enable);
		break;
#endif
#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_iqk_onoff_8852b(rf, is_enable);
		break;
#endif
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_iqk_onoff_8852c(rf, is_enable);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_iqk_onoff_8832br(rf, is_enable);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_iqk_onoff_8192xb(rf, is_enable);
		break;
#endif
		default:
			break;
	}
	rf->rfk_is_processing = false;
	
}

void halrf_dpk_onoff(void *rf_void, bool is_enable)
{
	/*signal go throughput dpk or not*/
		struct rf_info *rf = (struct rf_info *)rf_void;
		struct rtw_hal_com_t *hal_i = rf->hal_com;
		struct halrf_dpk_info *dpk = &rf->dpk;

		u8 path = 0;
#if 0
		if (hal_i->is_mp_mode &&
			hal_i->is_con_tx &&
			hal_i->is_single_tone &&
			hal_i->is_carrier_suppresion)
				return;
	
		if (hal_i->rfk_forbidden)
			return;
#endif
		if (!(rf->support_ability & HAL_RF_DPK))
				return;

		rf->rfk_is_processing = true;

		dpk->is_dpk_enable = is_enable;

		switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			for (path = 0; path < 2; path++)
				halrf_dpk_onoff_8852a(rf, path, false);
			break;
#endif
#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			for (path = 0; path < 2; path++)
				halrf_dpk_onoff_8852b(rf, path, false);
			break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		for (path = 0; path < 2; path++)
			halrf_dpk_onoff_8852c(rf, path, false);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		for (path = 0; path < 2; path++)
			halrf_dpk_onoff_8852bp(rf, path, false);
		break;
#endif
#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		for (path = 0; path < 2; path++)
			halrf_dpk_onoff_8851b(rf, path, false);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		for (path = 0; path < 2; path++)
			halrf_dpk_onoff_8832br(rf, path, false);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		for (path = 0; path < 2; path++)
			halrf_dpk_onoff_8192xb(rf, path, false);
		break;
#endif

			default:
			break;
		}
		rf->rfk_is_processing = false;
}

void halrf_dpk_track_onoff(void *rf_void, bool is_enable)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_dpk_info *dpk = &rf->dpk;

	dpk->is_dpk_track_en = is_enable;
}

void halrf_dpk_switch(void *rf_void, bool is_enable)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_pwr_info *pwr = &rf->pwr_info;

	if (is_enable) {
		halrf_dpk_onoff(rf, true);
		halrf_dpk_trigger(rf, HW_PHY_0, true);
		halrf_dpk_track_onoff(rf, true);
		/*halrf_wl_tx_power_control(rf, 0xffffeeee);*/
		pwr->dpk_mcc_power = 0;
		halrf_set_power(rf, HW_PHY_0,
			(PWR_BY_RATE | PWR_LIMIT | PWR_LIMIT_RU));
	} else {
		/*halrf_wl_tx_power_control(rf, 0xffff8034);*/
		pwr->dpk_mcc_power = -12;	/*-3dB*/
		halrf_set_power(rf, HW_PHY_0,
			(PWR_BY_RATE | PWR_LIMIT | PWR_LIMIT_RU));
		halrf_dpk_track_onoff(rf, false);
		halrf_dpk_onoff(rf, false);
	}
}

void halrf_dpk_init(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_dpk_info *dpk = &rf->dpk;

	u8 path, idx;

	for (path = 0; path < KPATH; path++) {
		dpk->cur_idx[path] = 0;
		for (idx = 0; idx < DPK_BKUP_NUM; idx++) {
			dpk->bp[path][idx].band = 0;
			dpk->bp[path][idx].bw = 0;
			dpk->bp[path][idx].ch = 0;
			dpk->bp[path][idx].path_ok = 0;
			dpk->bp[path][idx].txagc_dpk = 0;
			dpk->bp[path][idx].ther_dpk = 0;
			dpk->bp[path][idx].gs = 0;
			dpk->bp[path][idx].pwsf = 0;
		}
		dpk->max_dpk_txagc[path] = 0x3F;
	}

	dpk->is_dpk_enable = true;
	dpk->is_dpk_track_en = true;
	dpk->is_dpk_reload_en = false;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_dpk_init_8852a(rf);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_dpk_init_8852bp(rf);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_dpk_init_8852b(rf);
		break;
#endif
#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		halrf_dpk_init_8730a(rf);
		break;
#endif

	default:
		break;
	}
}

void halrf_rx_dck_onoff(void *rf_void, bool is_enable)
{
	/*signal go throughput dpk or not*/
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
#if 0
	if (hal_i->is_mp_mode &&
		hal_i->is_con_tx &&
		hal_i->is_single_tone &&
		hal_i->is_carrier_suppresion)
			return;

	if (hal_i->rfk_forbidden)
		return;
#endif
	if (!(rf->support_ability & HAL_RF_RXDCK))
			return;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_rx_dck_onoff_8852a(rf, is_enable);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_rx_dck_onoff_8852b(rf, is_enable);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_rx_dck_onoff_8852c(rf, is_enable);
		break;
#endif
#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_rx_dck_onoff_8851b(rf, is_enable);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_rx_dck_onoff_8832br(rf, is_enable);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_rx_dck_onoff_8192xb(rf, is_enable);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_rx_dck_onoff_8852bp(rf, is_enable);
		break;
#endif
	default:
		break;
	}
}

void halrf_rx_dck_track_onoff(void *rf_void, bool is_enable)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	rx_dck->is_rxdck_track_en = is_enable;
}

void halrf_rx_dck_init(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_rx_dck_info *rx_dck = &rf->rx_dck;

	rx_dck->is_rxdck_track_en = true;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		if (halrf_rrf(rf, RF_PATH_A, 0x95, BIT(3)) == 0x1) /*if auto DCK enabled*/
			rx_dck->is_auto_res = true;
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		if (halrf_rrf(rf, RF_PATH_A, 0x95, BIT(3)) == 0x1) /*if auto DCK enabled*/
			rx_dck->is_auto_res = true;
		break;
#endif
	default:
		break;
	}
}

void halrf_gapk_onoff(void *rf_void, bool is_enable)
{
	/*parameter to default or not*/

}

void halrf_dack_onoff(void *rf_void, bool is_enable)
{
	/*parameter to default or not*/

}

void halrf_tssi_onoff(void *rf_void, bool is_enable)
{
	/*parameter to default or not*/

}

bool halrf_get_iqk_onoff(void *rf_void)
{
	return 1;
}

bool halrf_get_dpk_onoff(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_dpk_info *dpk = &rf->dpk;

	return dpk->is_dpk_enable;
}

bool halrf_get_dpk_track_onoff(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_dpk_info *dpk = &rf->dpk;

	return dpk->is_dpk_track_en;
}

bool halrf_get_gapk_onoff(void *rf_void)
{
	return 1;
}
bool halrf_get_dack_onoff(void *rf_void)
{
	return 1;
}
bool halrf_get_tssi_onoff(void *rf_void)
{
	return 1;
}

u8 halrf_get_thermal(void *rf_void, u8 rf_path)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_get_thermal_8852a(rf, rf_path);
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		return halrf_get_thermal_8852b(rf, rf_path);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		return halrf_get_thermal_8852c(rf, rf_path);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		return halrf_get_thermal_8832br(rf, rf_path);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		return halrf_get_thermal_8192xb(rf, rf_path);
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		return halrf_get_thermal_8852bp(rf, rf_path);
#endif

#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A)
		return halrf_get_thermal_8730a(rf, rf_path);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		return halrf_get_thermal_8851b(rf, rf_path);
#endif
	return 0;
}

u32 halrf_get_tssi_de(void *rf_void, enum phl_phy_idx phy_idx, u8 path)
{
	return 0;
}

s32 halrf_get_online_tssi_de(void *rf_void, enum phl_phy_idx phy_idx, u8 path, s32 dbm, s32 puot)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	s32 tmp = 0;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			tmp = halrf_get_online_tssi_de_8852a(rf, phy_idx, path, dbm, puot);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			tmp = halrf_get_online_tssi_de_8852b(rf, phy_idx, path, dbm, puot);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			tmp = halrf_get_online_tssi_de_8852c(rf, phy_idx, path, dbm, puot);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			tmp = halrf_get_online_tssi_de_8832br(rf, phy_idx, path, dbm, puot);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			tmp = halrf_get_online_tssi_de_8192xb(rf, phy_idx, path, dbm, puot);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			tmp = halrf_get_online_tssi_de_8852bp(rf, phy_idx, path, dbm, puot);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			tmp = halrf_get_online_tssi_de_8851b(rf, phy_idx, path, dbm, puot);
			break;
#endif
		default:
			break;
	}

	return tmp;
}

void halrf_tssi_enable(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_tssi_enable_8852a(rf, phy_idx);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_tssi_enable_8852b(rf, phy_idx);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_tssi_enable_8852c(rf, phy_idx);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_tssi_enable_8832br(rf, phy_idx);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_tssi_enable_8192xb(rf, phy_idx);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_tssi_enable_8852bp(rf, phy_idx);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_tssi_enable_8851b(rf, phy_idx);
			break;
#endif

		default:
			break;
	}
}

void halrf_tssi_disable(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_tssi_disable_8852a(rf, phy_idx);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_tssi_disable_8852b(rf, phy_idx);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_tssi_disable_8852c(rf, phy_idx);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_tssi_disable_8832br(rf, phy_idx);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_tssi_disable_8192xb(rf, phy_idx);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_tssi_disable_8852bp(rf, phy_idx);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_tssi_disable_8851b(rf, phy_idx);
			break;
#endif

		default:
			break;
	}
}

void halrf_tssi_slope_onoff(void *rf_void, u8 tssi_slope_type)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_tssi_slope_onoff_8852c(rf, tssi_slope_type);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_tssi_slope_onoff_8832br(rf, tssi_slope_type);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			/*halrf_tssi_slope_onoff_8192xb(rf, tssi_slope_type);*/
			break;
#endif

		default:
			break;
	}
}

/*MP sloep K*/
void halrf_tssi_do_slope(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_tssi_do_slope_8852c(rf);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_tssi_do_slope_8832br(rf);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			/*halrf_tssi_do_slope_8192xb(rf);*/
			break;
#endif

		default:
			break;
	}
}

/*MP sloep K*/
u32 halrf_tssi_get_cw(void *rf_void, enum rf_path path)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			return halrf_tssi_get_cw_8852c(rf, path);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			return halrf_tssi_get_cw_8832br(rf, path);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			/*return halrf_tssi_get_cw_8192xb(rf, path);*/
			break;
#endif

		default:
			break;
	}

	return 0;
}

/*MP sloep K*/
void halrf_tssi_slope_apply(void *rf_void,
	u32 gain_first, u32 gain_second,
	u32 cw_first, u32 cw_second, enum rf_path path,
	u32* gain_diff, u32* cw_diff)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_tssi_slope_apply_8852c(rf,
						gain_first, gain_second,
						cw_first, cw_second, path,
						gain_diff, cw_diff);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_tssi_slope_apply_8832br(rf,
						gain_first, gain_second,
						cw_first, cw_second, path,
						gain_diff, cw_diff);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			/*halrf_tssi_slope_apply_8192xb(rf,
						gain_first, gain_second,
						cw_first, cw_second, path,
						gain_diff, cw_diff);*/
			break;
#endif

		default:
			break;
	}
}

/*MP sloep K*/
void halrf_tssi_current_slope_apply(void *rf_void,
	u32 gain_diff, u32 cw_diff, enum rf_path path)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_tssi_current_slope_apply_8852c(rf,
						gain_diff, cw_diff, path);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_tssi_current_slope_apply_8832br(rf,
						gain_diff, cw_diff, path);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			/*halrf_tssi_current_slope_apply_8192xb(rf,
						gain_diff, cw_diff, path);*/
			break;
#endif

		default:
			break;
	}
}

/*MP sloep K*/
void halrf_tssi_finish_slope(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_tssi_finish_slope_8852c(rf);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_tssi_finish_slope_8832br(rf);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			/*halrf_tssi_finish_slope_8192xb(rf);*/
			break;
#endif

		default:
			break;
	}
}

void halrf_tssi_init(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	u8 i;

	for (i = 0; i < MAX_RF_PATH; i++)
		rf->is_tssi_mode[i] = false;
}

s8 halrf_get_ther_protected_threshold(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			return halrf_get_ther_protected_threshold_8852a(rf);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			return halrf_get_ther_protected_threshold_8852b(rf);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			return halrf_get_ther_protected_threshold_8852c(rf);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			return halrf_get_ther_protected_threshold_8852bp(rf);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			return halrf_get_ther_protected_threshold_8851b(rf);
			break;
#endif

		default:
			break;
	}

	return 0;
}

void halrf_set_tssi_de_for_tx_verify(void *rf_void, enum phl_phy_idx phy_idx, u32 tssi_de, u8 path)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_set_tssi_de_for_tx_verify_8852a(rf, phy_idx, tssi_de, path);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_set_tssi_de_for_tx_verify_8852b(rf, phy_idx, tssi_de, path);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_set_tssi_de_for_tx_verify_8852c(rf, phy_idx, tssi_de, path);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_set_tssi_de_for_tx_verify_8832br(rf, phy_idx, tssi_de, path);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_set_tssi_de_for_tx_verify_8192xb(rf, phy_idx, tssi_de, path);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_set_tssi_de_for_tx_verify_8852bp(rf, phy_idx, tssi_de, path);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_set_tssi_de_for_tx_verify_8851b(rf, phy_idx, tssi_de, path);
			break;
#endif
		default:
			break;
	}
}

void halrf_tssi_cck_by_pass(void *rf_void,
			enum phl_phy_idx phy, bool by_pass_en)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	
	switch (hal_i->chip_id) {
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_tssi_cck_by_pass_8192xb(rf, phy, by_pass_en);
		break;
#endif
		default:
		break;
	}
}

u32 halrf_tssi_get_final(void *rf_void, enum phl_phy_idx phy_idx, u8 path)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			return halrf_tssi_get_final_8852ab(rf, path);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			return halrf_tssi_get_final_8852b(rf, path);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			return halrf_tssi_get_final_8852c(rf, path);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			return halrf_tssi_get_final_8832br(rf, path);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			return halrf_tssi_get_final_8192xb(rf, path);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			return halrf_tssi_get_final_8852bp(rf, path);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			return halrf_tssi_get_final_8851b(rf, path);
			break;
#endif

		default:
			break;
	}

	return 0;
}

void halrf_set_tssi_de_offset(void *rf_void, enum phl_phy_idx phy_idx, u32 tssi_de_offset, u8 path)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_set_tssi_de_offset_8852a(rf, phy_idx, tssi_de_offset, path);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_set_tssi_de_offset_8852b(rf, phy_idx, tssi_de_offset, path);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_set_tssi_de_offset_8852c(rf, phy_idx, tssi_de_offset, path);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_set_tssi_de_offset_8832br(rf, phy_idx, tssi_de_offset, path);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_set_tssi_de_offset_8192xb(rf, phy_idx, tssi_de_offset, path);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_set_tssi_de_offset_8852bp(rf, phy_idx, tssi_de_offset, path);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_set_tssi_de_offset_8851b(rf, phy_idx, tssi_de_offset, path);
			break;
#endif

		default:
			break;
	}
}

void halrf_set_tssi_avg_mp(void *rf_void,
					enum phl_phy_idx phy_idx, s32 xdbm)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	
	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_set_tssi_avg_mp_8852a(rf, phy_idx, xdbm);
			break;
#endif
		default:
		break;
	}
}

void halrf_set_rx_gain_offset_for_rx_verify(void *rf_void,
					enum phl_phy_idx phy,
					s8 rx_gain_offset, u8 path)
{
#if 0
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_set_rx_gain_offset_for_rx_verify_8852a(rf, phy, rx_gain_offset, path);
			break;
#endif
			default:
			break;
	}
#endif
}

void halrf_set_power_track(void *rf_void, enum phl_phy_idx phy_idx, u8 value)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_tssi_info *tssi_info = &rf->tssi;
	u32 phy_tmp;

	if (rf->hal_com->dbcc_en)
		phy_tmp = phy_idx;
	else
		phy_tmp = HW_PHY_0;

	tssi_info->tssi_type[phy_tmp] = value;

	if (value == TSSI_OFF) {
		halrf_tssi_trigger(rf, phy_idx, true);
		rf->support_ability = rf->support_ability & ~HAL_RF_TX_PWR_TRACK;
	} else if (value == TSSI_ON) {
		rf->support_ability = rf->support_ability | HAL_RF_TX_PWR_TRACK;
		halrf_tssi_trigger(rf, phy_idx, true);
	} else if (value == TSSI_CAL) {
		rf->support_ability = rf->support_ability | HAL_RF_TX_PWR_TRACK;
		halrf_tssi_trigger(rf, phy_idx, true);
	} else
		rf->support_ability = rf->support_ability & ~HAL_RF_TX_PWR_TRACK;

	
}

u8 halrf_get_power_track(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_tssi_info *tssi_info = &rf->tssi;

	return tssi_info->tssi_type[HW_PHY_0];
}

u8 halrf_get_power_track_phy(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_tssi_info *tssi_info = &rf->tssi;
	u32 phy_tmp;

	if (rf->hal_com->dbcc_en)
		phy_tmp = phy_idx;
	else
		phy_tmp = HW_PHY_0;

	return tssi_info->tssi_type[phy_tmp];
}

void halrf_tssi_get_efuse_ex(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_tssi_get_efuse_8852a(rf, phy_idx);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_tssi_get_efuse_8852b(rf, phy_idx);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_tssi_get_efuse_8852c(rf, phy_idx);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_tssi_get_efuse_8832br(rf, phy_idx);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_tssi_get_efuse_8192xb(rf, phy_idx);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_tssi_get_efuse_8852bp(rf, phy_idx);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_tssi_get_efuse_8851b(rf, phy_idx);
			break;
#endif

#ifdef RF_8730A_SUPPORT
		case CHIP_WIFI6_8730A:
			halrf_tssi_get_efuse_8730a(rf, phy_idx);
			break;
#endif

		default:
			break;
	}
	halrf_get_efuse_power_table_switch(rf, phy_idx);
}

bool halrf_tssi_check_efuse_data(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			return halrf_tssi_check_efuse_data_8852a(rf, phy_idx);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			return halrf_tssi_check_efuse_data_8852b(rf, phy_idx);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			return halrf_tssi_check_efuse_data_8852c(rf, phy_idx);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			return halrf_tssi_check_efuse_data_8832br(rf, phy_idx);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			return halrf_tssi_check_efuse_data_8192xb(rf, phy_idx);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			return halrf_tssi_check_efuse_data_8852bp(rf, phy_idx);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			return halrf_tssi_check_efuse_data_8851b(rf, phy_idx);
			break;
#endif

		default:
			break;
	}
	return false;
}

void halrf_set_ref_power_to_struct(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_set_ref_power_to_struct_8852a(rf, phy_idx);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_set_ref_power_to_struct_8852b(rf, phy_idx);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_set_ref_power_to_struct_8852c(rf, phy_idx);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_set_ref_power_to_struct_8832br(rf, phy_idx);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_set_ref_power_to_struct_8192xb(rf, phy_idx);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_set_ref_power_to_struct_8852bp(rf, phy_idx);
			break;
#endif

#ifdef RF_8730A_SUPPORT
		case CHIP_WIFI6_8730A:
			halrf_set_ref_power_to_struct_8730a(rf, phy_idx);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_set_ref_power_to_struct_8851b(rf, phy_idx);
			break;
#endif

		default:
			break;
	}
}

void halrf_set_regulation_init(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_pwr_info *pwr = &rf->pwr_info;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			pwr->regulation_idx = 0xff;
			break;
#endif
			default:
			break;
	}
}

void halrf_bf_config_rf(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_bf_config_rf_8852a(rf);
			break;
#endif
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_bf_config_rf_8852c(rf);
			break;
#endif
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_bf_config_rf_8192xb(rf);
			break;
#endif
#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_bf_config_rf_8832br(rf);
			break;
#endif
			default:
			break;
		}
}

bool halrf_get_efuse_info(void *rf_void, u8 *efuse_map,
		       enum rtw_efuse_info id, void *value, u32 length,
		       u8 autoload_status)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_get_efuse_info_8852a(rf, efuse_map, id, value, length,
					autoload_status);
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		return halrf_get_efuse_info_8852b(rf, efuse_map, id, value, length,
					autoload_status);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		return halrf_get_efuse_info_8852c(rf, efuse_map, id, value, length,
					autoload_status);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		return halrf_get_efuse_info_8832br(rf, efuse_map, id, value, length,
					autoload_status);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		return halrf_get_efuse_info_8192xb(rf, efuse_map, id, value, length,
					autoload_status);
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		return halrf_get_efuse_info_8852bp(rf, efuse_map, id, value, length,
					autoload_status);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		return halrf_get_efuse_info_8851b(rf, efuse_map, id, value, length,
					autoload_status);
#endif

	return 0;
}

void halrf_get_efuse_rx_gain_k(void *rf_void, enum phl_phy_idx phy_idx)
{
#if 0
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_get_efuse_rx_gain_k_8852a(rf, phy_idx);
#endif
#endif

}

void halrf_get_efuse_trim(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_get_efuse_trim_8852a(rf, phy_idx);
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		halrf_get_efuse_trim_8852b(rf, phy_idx);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		halrf_get_efuse_trim_8852c(rf, phy_idx);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		halrf_get_efuse_trim_8832br(rf, phy_idx);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		halrf_get_efuse_trim_8192xb(rf, phy_idx);
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		halrf_get_efuse_trim_8852bp(rf, phy_idx);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		halrf_get_efuse_trim_8851b(rf, phy_idx);
#endif

#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A)
		halrf_get_efuse_trim_8730a(rf, phy_idx);
#endif

}

void halrf_do_rx_gain_k(void *rf_void, enum phl_phy_idx phy_idx)
{
#if 0
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

	if (!(rf->support_ability & HAL_RF_RXGAINK))
		return;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_do_rx_gain_k_8852a(rf, phy_idx);
#endif
#endif
}

bool halrf_set_dbcc(void *rf_void, bool dbcc_en)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_set_dbcc_8852a(rf, dbcc_en);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		return halrf_set_dbcc_8852c(rf, dbcc_en);
#endif

	return 0;
}

void halrf_wlan_tx_power_control(void *rf_void, enum phl_phy_idx phy,
	enum phl_pwr_ctrl pwr_ctrl_idx, u32 tx_power_val, bool enable)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

	switch (hal_com->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_wlan_tx_power_control_8852a(rf,
				phy, pwr_ctrl_idx, tx_power_val, enable);
			break;
#endif
#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_wlan_tx_power_control_8852b(rf,
				phy, pwr_ctrl_idx, tx_power_val, enable);
			break;
#endif
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_wlan_tx_power_control_8852c(rf,
				phy, pwr_ctrl_idx, tx_power_val, enable);
			break;
#endif
#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_wlan_tx_power_control_8852bp(rf,
				phy, pwr_ctrl_idx, tx_power_val, enable);
			break;
#endif
#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_wlan_tx_power_control_8851b(rf,
				phy, pwr_ctrl_idx, tx_power_val, enable);
			break;
#endif
#ifdef RF_8730A_SUPPORT
		case CHIP_WIFI6_8730A:
			halrf_wlan_tx_power_control_8730a(rf,
				phy, pwr_ctrl_idx, tx_power_val, enable);
			break;
#endif
		default:
			break;
	}
}

bool halrf_wl_tx_power_control(void *rf_void, u32 tx_power_val)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool rtn = false;

	switch (hal_com->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			rtn = halrf_wl_tx_power_control_8852a(rf, tx_power_val);
			break;
#endif
#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			rtn = halrf_wl_tx_power_control_8852b(rf, tx_power_val);
			break;
#endif
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			rtn = halrf_wl_tx_power_control_8852c(rf, tx_power_val);
			break;
#endif
#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			rtn = halrf_wl_tx_power_control_8852bp(rf, tx_power_val);
			break;
#endif
#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			rtn = halrf_wl_tx_power_control_8851b(rf, tx_power_val);
			break;
#endif
#ifdef RF_8730A_SUPPORT
		case CHIP_WIFI6_8730A:
			rtn = halrf_wl_tx_power_control_8730a(rf, tx_power_val);
			break;
#endif

		default:
			break;
	}

	return rtn;
}

void halrf_watchdog(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;

	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_lock(rf, &rf->rf_lock);
	rf->cur_ther_s0 = halrf_get_thermal(rf, RF_PATH_A);
	rf->cur_ther_s1 = halrf_get_thermal(rf, RF_PATH_B);
	halrf_tssi_ant_open(rf);
	halrf_tssi_tracking(rf);
	halrf_dpk_tracking(rf);
	halrf_iqk_tracking(rf);
	halrf_lck_tracking(rf);
	halrf_rx_dck_tracking(rf);
	/*halrf_op5k_tracking(rf);*/
	/*halrf_set_tpe_control(rf);*/
	if (phl_is_mp_mode(rf->phl_com))
		halrf_mutex_unlock(rf, &rf->rf_lock);
}

u8 halrf_get_default_rfe_type(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_get_default_rfe_type_8852a(rf);
#endif
	return 1;
}

u8 halrf_get_default_xtal(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_get_default_xtal_8852a(rf);
#endif
	return 0x3f;
}

bool halrf_iqk_get_ther_rek(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_iqk_get_ther_rek_8852ab(rf);
#endif
#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		return halrf_iqk_get_ther_rek_8852b(rf);
#endif
#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		return halrf_iqk_get_ther_rek_8852c(rf);
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A)
		return halrf_iqk_get_ther_rek_8730a(rf);
#endif

	return false;
}

enum rtw_hal_status halrf_iqk_tracking(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	struct halrf_iqk_info *iqk = &rf->iqk;

	if (!(rf->support_ability & HAL_RF_IQK) || rf->rfk_is_processing ||
		rf->is_watchdog_stop)
		return RTW_HAL_STATUS_SUCCESS;

	halrf_watchdog_stop(rf, true); /*avoid race condition*/

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_iqk_track_8852ab(rf);
#if 0
		if (halrf_iqk_get_ther_rek(rf))
			halrf_iqk_trigger(rf, 0x0, false);
#endif
	break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_iqk_track_8852b(rf);
#if 0
		if (halrf_iqk_get_ther_rek(rf))
			halrf_iqk_trigger(rf, 0x0, false);
#endif
	break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_iqk_track_8852c(rf);
#if 0
		if (halrf_iqk_get_ther_rek(rf))
			halrf_iqk_trigger(rf, 0x0, false);
#endif
		break;
#endif

#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		halrf_iqk_track_8730a(rf);
#if 0
		if (halrf_iqk_get_ther_rek(rf))
			halrf_iqk_trigger(rf, 0x0, false);
#endif

	break;
#endif

	default:
	break;
	}

	halrf_watchdog_stop(rf, false);

	return RTW_HAL_STATUS_SUCCESS;
}

void halrf_psd_init(void *rf_void, enum phl_phy_idx phy,
			u8 path, u8 iq_path, u32 avg, u32 fft)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_psd_init_8852a(rf, phy, path, iq_path, avg, fft);
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		halrf_psd_init_8852b(rf, phy, path, iq_path, avg, fft);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		halrf_psd_init_8852c(rf, phy, path, iq_path, avg, fft);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		halrf_psd_init_8832br(rf, phy, path, iq_path, avg, fft);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		halrf_psd_init_8192xb(rf, phy, path, iq_path, avg, fft);
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		halrf_psd_init_8852bp(rf, phy, path, iq_path, avg, fft);
#endif

#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A)
		halrf_psd_init_8730a(rf, phy, path, iq_path, avg, fft);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		halrf_psd_init_8851b(rf, phy, path, iq_path, avg, fft);
#endif

}

void halrf_psd_restore(void *rf_void, enum phl_phy_idx phy)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_psd_restore_8852a(rf, phy);
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		halrf_psd_restore_8852b(rf, phy);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		halrf_psd_restore_8852c(rf, phy);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		halrf_psd_restore_8832br(rf, phy);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		halrf_psd_restore_8192xb(rf, phy);
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		halrf_psd_restore_8852bp(rf, phy);
#endif

#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A)
		halrf_psd_restore_8730a(rf, phy);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		halrf_psd_restore_8851b(rf, phy);
#endif

}

u32 halrf_psd_get_point_data(void *rf_void, enum phl_phy_idx phy, s32 point)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_psd_get_point_data_8852a(rf, phy, point);
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		return halrf_psd_get_point_data_8852b(rf, phy, point);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		return halrf_psd_get_point_data_8852c(rf, phy, point);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		return halrf_psd_get_point_data_8832br(rf, phy, point);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		return halrf_psd_get_point_data_8192xb(rf, phy, point);
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		return halrf_psd_get_point_data_8852bp(rf, phy, point);
#endif

#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A)
		return halrf_psd_get_point_data_8730a(rf, phy, point);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		return halrf_psd_get_point_data_8851b(rf, phy, point);
#endif

	return 0;
}

void halrf_psd_query(void *rf_void, enum phl_phy_idx phy,
			u32 point, u32 start_point, u32 stop_point, u32 *outbuf)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_psd_query_8852a(rf, phy, point, start_point, stop_point, outbuf);
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		halrf_psd_query_8852b(rf, phy, point, start_point, stop_point, outbuf);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		halrf_psd_query_8852c(rf, phy, point, start_point, stop_point, outbuf);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		halrf_psd_query_8832br(rf, phy, point, start_point, stop_point, outbuf);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		halrf_psd_query_8192xb(rf, phy, point, start_point, stop_point, outbuf);
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		halrf_psd_query_8852bp(rf, phy, point, start_point, stop_point, outbuf);
#endif

#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A)
		halrf_psd_query_8730a(rf, phy, point, start_point, stop_point, outbuf);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		halrf_psd_query_8851b(rf, phy, point, start_point, stop_point, outbuf);
#endif

}

void halrf_set_fix_power_to_struct(void *rf_void,
					enum phl_phy_idx phy, s8 dbm)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_pwr_info *pwr = &rf->pwr_info;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	pwr->fix_power[RF_PATH_A] = true;
	pwr->fix_power_dbm[RF_PATH_A] = dbm * 2;

	pwr->fix_power[RF_PATH_B] = true;
	pwr->fix_power_dbm[RF_PATH_B] = dbm * 2;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_set_fix_power_to_struct_8852ab(rf, phy, dbm);
		break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_set_fix_power_to_struct_8852b(rf, phy, dbm);
		break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_set_fix_power_to_struct_8852c(rf, phy, dbm);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_set_fix_power_to_struct_8832br(rf, phy, dbm);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_set_fix_power_to_struct_8192xb(rf, phy, dbm);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_set_fix_power_to_struct_8852bp(rf, phy, dbm);
		break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_set_fix_power_to_struct_8851b(rf, phy, dbm);
		break;
#endif

		default:
		break;
	}
}

void halrf_pwr_by_rate_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len, enum phl_phy_idx phy)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	u32 used = *_used;
	u32 out_len = *_out_len;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_pwr_by_rate_info_8852a(rf, input, &used,
				output, &out_len, phy);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_pwr_by_rate_info_8852b(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_pwr_by_rate_info_8852c(rf, input, &used,
				output, &out_len, phy);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_pwr_by_rate_info_8832br(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_pwr_by_rate_info_8192xb(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_pwr_by_rate_info_8852bp(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_pwr_by_rate_info_8851b(rf, input, &used,
				output, &out_len);
			break;
#endif

		default:
			break;
	}

	*_used = used;
	*_out_len = out_len;
}

void halrf_pwr_limit_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len, enum phl_phy_idx phy)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	u32 used = *_used;
	u32 out_len = *_out_len;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_pwr_limit_info_8852a(rf, input, &used,
				output, &out_len, phy);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_pwr_limit_info_8852b(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_pwr_limit_info_8852c(rf, input, &used,
				output, &out_len, phy);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_pwr_limit_info_8832br(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_pwr_limit_info_8192xb(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_pwr_limit_info_8852bp(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_pwr_limit_info_8851b(rf, input, &used,
				output, &out_len);
			break;
#endif

		default:
			break;
	}

	*_used = used;
	*_out_len = out_len;
}

void halrf_pwr_limit_ru_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len, enum phl_phy_idx phy)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	u32 used = *_used;
	u32 out_len = *_out_len;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_pwr_limit_ru_info_8852a(rf, input, &used,
				output, &out_len, phy);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_pwr_limit_ru_info_8852b(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_pwr_limit_ru_info_8852c(rf, input, &used,
				output, &out_len, phy);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_pwr_limit_ru_info_8832br(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_pwr_limit_ru_info_8192xb(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_pwr_limit_ru_info_8852bp(rf, input, &used,
				output, &out_len);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_pwr_limit_ru_info_8851b(rf, input, &used,
				output, &out_len);
			break;
#endif

		default:
			break;
	}
}

void halrf_get_tssi_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_get_tssi_info_8852a(rf, input, _used,
				output, _out_len);
			break;
#endif

#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_get_tssi_info_8852b(rf, input, _used,
				output, _out_len);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_get_tssi_info_8852c(rf, input, _used,
				output, _out_len);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_get_tssi_info_8832br(rf, input, _used,
				output, _out_len);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_get_tssi_info_8192xb(rf, input, _used,
				output, _out_len);
			break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_get_tssi_info_8852bp(rf, input, _used,
				output, _out_len);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_get_tssi_info_8851b(rf, input, _used,
				output, _out_len);
			break;
#endif

		default:
			break;
	}
}

void halrf_get_tssi_trk_info(struct rf_info *rf, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	u32 used = *_used;
	u32 out_len = *_out_len;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
		case CHIP_WIFI6_8852A:
			halrf_get_tssi_trk_info_8852a(rf, input, &used,
				output, &out_len);
			break;
#endif
		default:
			break;
	}
}

void halrf_tssi_backup_txagc(struct rf_info *rf, enum phl_phy_idx phy, bool enable)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_tssi_backup_txagc_8852b(rf, phy, enable);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			//halrf_tssi_backup_txagc_8852c(rf, phy, enable);
			break;
#endif

		default:
			break;
	}
}

void halrf_set_tx_shape(struct rf_info *rf, u8 tx_shape_idx)
{
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_set_tx_shape_8852b(rf, tx_shape_idx);
			break;
#endif

#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_set_tx_shape_8852c(rf, tx_shape_idx);
			break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_set_tx_shape_8832br(rf, tx_shape_idx);
			break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_set_tx_shape_8192xb(rf, tx_shape_idx);
			break;
#endif
#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_set_tx_shape_8852bp(rf, tx_shape_idx);
			break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_set_tx_shape_8851b(rf, tx_shape_idx);
			break;
#endif

		default:
			break;
	}
}


void halrf_ctl_bw(void *rf_void, enum phl_phy_idx phy, enum channel_width bw)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:	
				halrf_ctrl_bw_8852b(rf, bw);
			break;
#endif
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
				halrf_ctrl_bw_8852c(rf, phy, bw);
			break;
#endif
#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
				halrf_ctrl_bw_8832br(rf, phy, bw);
			break;
#endif
#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
				halrf_ctrl_bw_8192xb(rf, phy, bw);
			break;
#endif
#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
				halrf_ctrl_bw_8852bp(rf, phy, bw);
		break;
#endif
#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:	
				halrf_ctrl_bw_8851b(rf, bw);
			break;
#endif

		default:
		break;
	}
	return;

}

void halrf_ctl_ch(void *rf_void,  enum phl_phy_idx phy, u8 central_ch, enum band_type band)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852B_SUPPORT
		case CHIP_WIFI6_8852B:
			halrf_ctrl_ch_8852b(rf, central_ch);
		break;
#endif
#ifdef RF_8852C_SUPPORT
		case CHIP_WIFI6_8852C:
			halrf_ctrl_ch_8852c(rf, phy, central_ch, band);
		break;
#endif

#ifdef RF_8832BR_SUPPORT
		case CHIP_WIFI6_8832BR:
			halrf_ctrl_ch_8832br(rf, phy, central_ch, band);
		break;
#endif

#ifdef RF_8192XB_SUPPORT
		case CHIP_WIFI6_8192XB:
			halrf_ctrl_ch_8192xb(rf, phy, central_ch, band);
		break;
#endif

#ifdef RF_8852BP_SUPPORT
		case CHIP_WIFI6_8852BP:
			halrf_ctrl_ch_8852bp(rf, phy, central_ch, band);
		break;
#endif

#ifdef RF_8851B_SUPPORT
		case CHIP_WIFI6_8851B:
			halrf_ctrl_ch_8851b(rf, central_ch);
		break;
#endif

		default:
		break;
	}
	return;
}

void halrf_rxbb_bw(void *rf_void, enum phl_phy_idx phy, enum channel_width bw)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_rxbb_bw_8852a(rf, phy, bw);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_rxbb_bw_8852b(rf, phy, bw);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_rxbb_bw_8852c(rf, phy, bw);
		break;
#endif
#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		halrf_rxbb_bw_8852bp(rf, phy, bw);
		break;
#endif
#ifdef RF_8832BR_SUPPORT
	case CHIP_WIFI6_8832BR:
		halrf_rxbb_bw_8832br(rf, phy, bw);
		break;
#endif
#ifdef RF_8192XB_SUPPORT
	case CHIP_WIFI6_8192XB:
		halrf_rxbb_bw_8192xb(rf, phy, bw);
		break;
#endif
#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		halrf_rxbb_bw_8851b(rf, phy, bw);
		break;
#endif

	default:
		break;
	}
	return;
}

void halrf_config_radio_to_fw(void *rf_void)
{

	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_config_8852a_radio_to_fw(rf);
		break;
#endif
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_config_8852b_radio_to_fw(rf);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_config_8852c_radio_to_fw(rf);
		break;
#endif
		default:
		break;
	}
}

void halrf_rfc_reg_backup(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_rfc_reg_backup_8852a(rf);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_rfc_reg_backup_8852c(rf);
		break;
#endif
		default:
		break;
	}
}

bool halrf_rfc_reg_check_fail(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	bool fail = false;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		fail = halrf_rfc_reg_check_fail_8852a(rf);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		fail = halrf_rfc_reg_check_fail_8852c(rf);
		break;
#endif
		default:
		break;
	}
	return fail;
}

void halrf_rfk_reg_backup(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		halrf_rfk_reg_backup_8852a(rf);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_rfk_reg_backup_8852c(rf);
		break;
#endif
		default:
		break;
	}
}

void halrf_rfk_reg_reload(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_rfk_reg_reload_8852c(rf);
		break;
#endif
		default:
		break;
	}
}

bool halrf_rfk_reg_check_fail(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	bool fail = false;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		fail = halrf_rfk_reg_check_fail_8852a(rf);
		break;
#endif
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		fail = halrf_rfk_reg_check_fail_8852c(rf);
		break;
#endif
		default:
		break;
	}
	return fail;
}

bool halrf_dack_reg_check_fail(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;
	bool fail = false;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		fail = halrf_dack_reg_check_fail_8852a(rf);
		break;
#endif
		default:
		break;
	}
	return fail;
}

bool halrf_rfk_chl_thermal(void *rf_void,
			u8 chl_idx, u8 ther_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool fail = false;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		fail = halrf_rfk_chl_thermal_8852a(rf, chl_idx, ther_idx);
#endif
	return fail;

}

void halrf_rfk_recovery_chl_thermal(void *rf_void, u8 chl_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_rfk_recovery_chl_thermal_8852a(rf, chl_idx);
#endif
}

u8 halrf_fcs_get_thermal_index(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	u8 idx = 0;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		idx = halrf_thermal_index_cal_8852a(rf);
#endif
	return idx;
}

void halrf_disconnect_notify(void *rf_void, struct rtw_chan_def *chandef )
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool fail = false;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		 halrf_disconnect_notify_8852a(rf, chandef);
#endif
#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		 halrf_disconnect_notify_8852b(rf, chandef);
#endif
#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		 halrf_disconnect_notify_8852c(rf, chandef);
#endif
	return;

}

bool  halrf_check_mcc_ch(void *rf_void, struct rtw_chan_def *chandef )
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	bool fail = false;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		fail = halrf_check_mcc_ch_8852a(rf, chandef);
#endif
#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		fail = halrf_check_mcc_ch_8852b(rf, chandef);
#endif
#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		fail = halrf_check_mcc_ch_8852c(rf, chandef);
#endif
	return fail;

}

void  halrf_fw_ntfy(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_fw_ntfy_8852a(rf, phy_idx);
#endif
#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		halrf_fw_ntfy_8852b(rf, phy_idx);
#endif
#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		halrf_fw_ntfy_8852c(rf, phy_idx);
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A)
		halrf_fw_ntfy_8730a(rf, phy_idx);
#endif

	return;
}

void halrf_set_regulation_from_driver(void *rf_void,
		u8 regulation_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_set_regulation_from_driver_8852a(rf, regulation_idx);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		halrf_set_regulation_from_driver_8852c(rf, regulation_idx);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		halrf_set_regulation_from_driver_8832br(rf, regulation_idx);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		halrf_set_regulation_from_driver_8192xb(rf, regulation_idx);
#endif
	return;
}

u32 halrf_get_nctl_reg_ver(struct rf_info *rf)
{
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_get_8852a_nctl_reg_ver();
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		return halrf_get_8852b_nctl_reg_ver();
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		return halrf_get_8852c_nctl_reg_ver();
#endif
#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		return halrf_get_8832br_nctl_reg_ver();
#endif
#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		return halrf_get_8192xb_nctl_reg_ver();
#endif
#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		return halrf_get_8852bp_nctl_reg_ver();
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		return halrf_get_8851b_nctl_reg_ver();
#endif
	return 0;
}

u32 halrf_get_radio_reg_ver(struct rf_info *rf)
{
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_get_8852a_radio_reg_ver();
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		return halrf_get_8852b_radio_reg_ver();
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		return halrf_get_8852c_radio_reg_ver();
#endif

	return 0;
}

u32 halrf_get_radio_ver_from_reg(struct rf_info *rf)
{
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_rrf(rf, RF_PATH_A, 0x67, 0xfff);
#endif
#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		return halrf_rrf(rf, RF_PATH_A, 0x9F, 0xfff);
#endif
#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		return halrf_rrf(rf, RF_PATH_A, 0xFE, 0xfff);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		return halrf_rrf(rf, RF_PATH_A, 0x9F, 0xfff);
#endif
#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		return halrf_rrf(rf, RF_PATH_A, 0xFE, 0xfff);
#endif
#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		return halrf_rrf(rf, RF_PATH_A, 0xFE, 0xfff);
#endif
#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		return halrf_rrf(rf, RF_PATH_A, 0x9F, 0xfff);
#endif
	return 0;
}

void halrf_config_nctl_reg(struct rf_info *rf)
{
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_config_8852a_nctl_reg(rf);
#endif
#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		halrf_config_8852b_nctl_reg(rf);
#endif
#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		halrf_config_8852c_nctl_reg(rf);
#endif
#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		halrf_config_8832br_nctl_reg(rf);
#endif
#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		halrf_config_8192xb_nctl_reg(rf);
#endif
#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		halrf_config_8852bp_nctl_reg(rf);
#endif
#ifdef RF_8730A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8730A)
		halrf_config_8730a_nctl_reg(rf);
#endif
#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		halrf_config_8851b_nctl_reg(rf);
#endif

	return;
}

void halrf_set_gpio(void *rf_void, enum phl_phy_idx phy, u8 band)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_set_gpio_8852a(rf, phy, band);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		halrf_set_gpio_8852c(rf, phy);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		halrf_set_gpio_8192xb(rf, phy);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		halrf_set_gpio_8851b(rf, phy);
#endif

}

bool halrf_mac_set_pwr_reg(void *rf_void, enum phl_phy_idx phy,
	u32 addr, u32 mask, u32 val)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	u32 result = 0;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		result = halrf_mac_set_pwr_reg_8852a(rf, phy, addr, mask, val);
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		result = halrf_mac_set_pwr_reg_8852b(rf, phy, addr, mask, val);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		result = halrf_mac_set_pwr_reg_8852c(rf, phy, addr, mask, val);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		result = halrf_mac_set_pwr_reg_8832br(rf, phy, addr, mask, val);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		result = halrf_mac_set_pwr_reg_8192xb(rf, phy, addr, mask, val);
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		result = halrf_mac_set_pwr_reg_8852bp(rf, phy, addr, mask, val);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		result = halrf_mac_set_pwr_reg_8851b(rf, phy, addr, mask, val);
#endif

	if (result == 0)	/*MAC: MACSUCCESS == 0*/
		return true;
	else
		return false;
}

u32 halrf_mac_get_pwr_reg(void *rf_void, enum phl_phy_idx phy,
	u32 addr, u32 mask)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;
	
#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_mac_get_pwr_reg_8852a(rf, phy, addr, mask);
#endif

#ifdef RF_8852B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852B)
		return halrf_mac_get_pwr_reg_8852b(rf, phy, addr, mask);
#endif

#ifdef RF_8852C_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852C)
		return halrf_mac_get_pwr_reg_8852c(rf, phy, addr, mask);
#endif

#ifdef RF_8832BR_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8832BR)
		return halrf_mac_get_pwr_reg_8832br(rf, phy, addr, mask);
#endif

#ifdef RF_8192XB_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8192XB)
		return halrf_mac_get_pwr_reg_8192xb(rf, phy, addr, mask);
#endif

#ifdef RF_8852BP_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852BP)
		return halrf_mac_get_pwr_reg_8852bp(rf, phy, addr, mask);
#endif

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		return halrf_mac_get_pwr_reg_8851b(rf, phy, addr, mask);
#endif

	return 0;
}

bool halrf_check_efem(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		return halrf_check_efem_8852a(rf, phy_idx);
#endif
	return false;
}

void halrf_2g_rxant(void *rf_void, enum halrf_ant ant)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8852A_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8852A)
		halrf_2g_rxant_8852a(rf, ant);
#endif
}

s8 halrf_xtal_tracking_offset(void *rf_void, enum phl_phy_idx phy_idx)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	if (!(rf->support_ability & HAL_RF_XTAL_TRACK))
		return 0;

	switch (hal_i->chip_id) {
#ifdef RF_8852A_SUPPORT
	case CHIP_WIFI6_8852A:
		return halrf_xtal_tracking_offset_8852a(rf, phy_idx);
	break;
#endif

#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		return halrf_xtal_tracking_offset_8852b(rf, phy_idx);
	break;
#endif

#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		return halrf_xtal_tracking_offset_8852c(rf, phy_idx);
	break;
#endif

#ifdef RF_8852BP_SUPPORT
	case CHIP_WIFI6_8852BP:
		return halrf_xtal_tracking_offset_8852bp(rf, phy_idx);
	break;
#endif

#ifdef RF_8851B_SUPPORT
	case CHIP_WIFI6_8851B:
		return halrf_xtal_tracking_offset_8851b(rf, phy_idx);
	break;
#endif

#ifdef RF_8730A_SUPPORT
	case CHIP_WIFI6_8730A:
		return halrf_xtal_tracking_offset_8730a(rf, phy_idx);
	break;
#endif

	default:
	break;
	}

	return 0;
}

void halrf_set_mp_regulation(void *rf_void, enum phl_phy_idx phy, u8 regulation)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_pwr_info *pwr = &rf->pwr_info;

	pwr->mp_regulation = regulation;
}

void halrf_rfe_ant_num_chk(void *rf_void)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852B_SUPPORT
	case CHIP_WIFI6_8852B:
		halrf_rfe_ant_num_chk_8852b(rf);
	break;
#endif
	default:
	break;
	}
}

void halrf_syn1_onoff(void *rf_void, enum phl_phy_idx phy, u8 path, bool syn1_turn_on) {

	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_syn1_onoff_8852c(rf, phy, path, syn1_turn_on);
	break;
#endif

	default:
	break;
	}
	return ;
}

void halrf_adie_pow_ctrl(void *rf_void, bool rf_off, bool others_off)
{

	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_adie_pow_ctrl_8852c(rf, rf_off, others_off);
	break;
#endif

	default:
	break;
	}
}

void halrf_afe_pow_ctrl(void *rf_void, bool adda_off, bool pll_off)
{

	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_i = rf->hal_com;

	switch (hal_i->chip_id) {
#ifdef RF_8852C_SUPPORT
	case CHIP_WIFI6_8852C:
		halrf_afe_pow_ctrl_8852c(rf, adda_off, pll_off);
	break;
#endif

	default:
	break;
	}
}

u32 halrf_get_iqk_times(void *rf_void) 
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct halrf_iqk_info *iqk_info = &rf->iqk;

	return iqk_info->iqk_times;
}

u32 halrf_get_lck_times(void *rf_void) 
{
	struct rf_info *rf = (struct rf_info *)rf_void;

	return rf->lck_times;
}

u32 halrf_get_extra_para_ver_from_reg(struct rf_info *rf)
{
#ifdef RF_8832BR_SUPPORT
	if (rf->ic_type == RF_RTL8832BR)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0x00ff);
#endif

#ifdef RF_8852C_SUPPORT
	if (rf->ic_type == RF_RTL8852C)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0x00ff);
#endif

#ifdef RF_8192XB_SUPPORT
	if (rf->ic_type == RF_RTL8192XB)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0x00ff);
#endif

#ifdef RF_8832CRVU_SUPPORT
	if (rf->ic_type == RF_RTL8832CR_VU)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0x00ff);
#endif
	
#ifdef RF_8832BRVT_SUPPORT
	if (rf->ic_type == RF_RTL8832BR_VT)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0x00ff);
#endif
	return 0;
}

u32 halrf_get_rfe_type_ver_from_reg(struct rf_info *rf)
{
#ifdef RF_8832BR_SUPPORT
	if (rf->ic_type == RF_RTL8832BR)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xff00);
#endif

#ifdef RF_8852C_SUPPORT
	if (rf->ic_type == RF_RTL8852C)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xff00);
#endif

#ifdef RF_8192XB_SUPPORT
	if (rf->ic_type == RF_RTL8192XB)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xff00);
#endif

#ifdef RF_8832CRVU_SUPPORT
	if (rf->ic_type == RF_RTL8832CR_VU)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xff00);
#endif
	
#ifdef RF_8832BRVT_SUPPORT
	if (rf->ic_type == RF_RTL8832BR_VT)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xff00);
#endif
	return 0;
}

u32 halrf_get_fem_id_from_reg(struct rf_info *rf)
{
#ifdef RF_8832BR_SUPPORT
	if (rf->ic_type == RF_RTL8832BR)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xf0000);
#endif

#ifdef RF_8852C_SUPPORT
	if (rf->ic_type == RF_RTL8852C)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xf0000);
#endif

#ifdef RF_8192XB_SUPPORT
	if (rf->ic_type == RF_RTL8192XB)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xf0000);
#endif

#ifdef RF_8832CRVU_SUPPORT
	if (rf->ic_type == RF_RTL8832CR_VU)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xf0000);
#endif
	
#ifdef RF_8832BRVT_SUPPORT
	if (rf->ic_type == RF_RTL8832BR_VT)
		return halrf_rrf(rf, RF_PATH_A, 0xFD, 0xf0000);
#endif
	return 0;
}

void halrf_set_pwr_lmt_main_or_aux(void *rf_void, u8 ant)
{
	struct rf_info *rf = (struct rf_info *)rf_void;
	struct rtw_hal_com_t *hal_com = rf->hal_com;

#ifdef RF_8851B_SUPPORT
	if (hal_com->chip_id == CHIP_WIFI6_8851B)
		halrf_set_pwr_lmt_main_or_aux_8851b(rf, ant);
#endif

}

#endif

