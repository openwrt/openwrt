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
#define _HAL_TEST_MP_TX_C_
#include "../../hal_headers.h"
#include "../../../test/mp/phl_test_mp_def.h"

#ifdef CONFIG_HAL_TEST_MP
enum rtw_hal_status rtw_hal_mp_tx_plcp_gen(
	struct mp_context *mp, struct mp_tx_arg *arg, struct mp_plcp_param_t *plcp_tx_struct)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	hal_status = rtw_hal_bb_set_plcp_tx(hal_info->hal_com,
										plcp_tx_struct,
										mp->usr,
										mp->cur_phy,
										&arg->plcp_sts);
	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, mp->cur_phy);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_tx_pmac_packet(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, mp->cur_phy);
	PHL_INFO("%s: start tx: %d\n", __FUNCTION__, arg->start_tx);
	PHL_INFO("%s: is cck: %d\n", __FUNCTION__, arg->is_cck);
	PHL_INFO("%s: tx count: %d\n", __FUNCTION__, arg->tx_cnt);
	PHL_INFO("%s: period: %d\n", __FUNCTION__, arg->period);
	PHL_INFO("%s: tx time: %d\n", __FUNCTION__, arg->tx_time);

	hal_status = rtw_hal_bb_set_pmac_packet_tx(hal_info->hal_com,
						   arg->start_tx,
						   arg->is_cck,
						   arg->tx_cnt,
						   arg->period,
						   arg->tx_time,
						   arg->cck_lbk_en,
						   mp->cur_phy);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_tx_pmac_continuous(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, mp->cur_phy);
	PHL_INFO("%s: start tx: %d\n", __FUNCTION__, arg->start_tx);
	PHL_INFO("%s: is cck: %d\n", __FUNCTION__, arg->is_cck);

	hal_status = rtw_hal_bb_set_pmac_cont_tx(hal_info->hal_com,
						 arg->start_tx,
						 arg->is_cck,
						 mp->cur_phy);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_tx_pmac_fw_trigger(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, mp->cur_phy);
	PHL_INFO("%s: start tx: %d\n", __FUNCTION__, arg->start_tx);
	PHL_INFO("%s: is cck: %d\n", __FUNCTION__, arg->is_cck);
	PHL_INFO("%s: tx count: %d\n", __FUNCTION__, arg->tx_cnt);
	PHL_INFO("%s: tx duty: %d\n", __FUNCTION__, arg->tx_time);

	hal_status = rtw_hal_bb_set_pmac_fw_trigger_tx(hal_info->hal_com,
						   arg->start_tx,
						   arg->is_cck,
						   arg->tx_cnt,
						   (u8)arg->tx_time,
						   mp->cur_phy);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}



enum rtw_hal_status rtw_hal_mp_tx_single_tone(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: start tx: %d\n", __FUNCTION__, arg->start_tx);
	PHL_INFO("%s: rf path: %d\n", __FUNCTION__, arg->tx_path);

	hal_status = rtw_hal_rf_set_singletone_tx(mp->hal,
						  arg->start_tx,
						  arg->tx_path);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_tx_carrier_suppression(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_bb_set_pmac_carrier_suppression_tx
					(hal_info->hal_com,
					arg->start_tx,
					arg->is_cck,
					mp->cur_phy);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_tx_phy_ok_cnt(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);
	/*
	hal_status = rtw_hal_bb_get_tx_ok(phl_info->hal,
					mp->cur_phy, &arg->tx_ok);
	*/
	hal_status = RTW_HAL_STATUS_SUCCESS;
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: phy ok cnt = %d\n", __FUNCTION__, arg->tx_ok);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_set_dpd_bypass(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, mp->cur_phy);
	PHL_INFO("%s: dpd_bypass: %d\n", __FUNCTION__, arg->dpd_bypass);

	hal_status = rtw_hal_bb_set_dpd_bypass(hal_info->hal_com,
						   arg->dpd_bypass,
						   mp->cur_phy);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

void rtw_hal_mp_check_tx_idle(struct mp_context *mp, struct mp_tx_arg *arg)
{
	arg->tx_state = rtw_hal_bb_check_tx_idle(mp->hal, mp->cur_phy);
}


enum rtw_hal_status rtw_hal_mp_bb_loop_bck(struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	u8 tx_path;
	u8 rx_path;

	PHL_INFO("%s\n", __FUNCTION__);

	if (arg->tx_path == RF_PATH_A) {
		tx_path = RF_PATH_A;
		rx_path = RF_PATH_B;
	} else {
		tx_path = RF_PATH_B;
		rx_path = RF_PATH_A;
	}

	hal_status = rtw_hal_bb_loop_bck_en(mp->hal, arg->enable, arg->is_dgt,
										tx_path, rx_path, arg->dbw, mp->cur_phy, arg->is_cck);

	return hal_status;
}


#endif /* CONFIG_HAL_TEST_MP */
