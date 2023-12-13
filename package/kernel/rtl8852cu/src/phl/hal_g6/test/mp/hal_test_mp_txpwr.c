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
#define _PHL_TEST_MP_TXPWR_C_
#include "../../hal_headers.h"
#include "../../../test/mp/phl_test_mp_def.h"

#ifdef CONFIG_HAL_TEST_MP
enum rtw_hal_status rtw_hal_mp_txpwr_read_table(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	PHL_INFO("%s: rf path = %d\n", __FUNCTION__, arg->rfpath);
	PHL_INFO("%s: rate = %d\n", __FUNCTION__, arg->rate);
	PHL_INFO("%s: bandwidth = %d\n", __FUNCTION__, arg->bandwidth);
	PHL_INFO("%s: channel = %d\n", __FUNCTION__, arg->channel);

	hal_status = rtw_hal_rf_read_pwr_table(hal_info->hal_com, arg->rfpath,
					       arg->rate, arg->bandwidth,
					       arg->channel, arg->offset,
					       arg->dcm, arg->beamforming,
					       &arg->table_item);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_get_pwrtrack(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_rf_get_pwrtrack(mp->hal, &arg->txpwr_track_status, arg->cur_phy);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: pwr track status = %d\n", __FUNCTION__,
		 arg->txpwr_track_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_set_pwrtrack(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: pwr track status = %d phy = %d\n", __FUNCTION__,
	                                                arg->txpwr_track_status,
	                                                arg->cur_phy);

	hal_status = rtw_hal_rf_set_pwrtrack(mp->hal, arg->cur_phy, arg->txpwr_track_status);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_set_pwr(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s: tx power = %d\n", __FUNCTION__, arg->txpwr);
	PHL_INFO("%s: phy index = %d\n", __FUNCTION__, arg->cur_phy);

	hal_status = rtw_hal_bb_set_power(hal_info->hal_com,
									  arg->txpwr, arg->cur_phy);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_get_pwr(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s: phy index = %d\n", __FUNCTION__, arg->cur_phy);

	/* Call hal API */
	hal_status = rtw_hal_bb_get_power(hal_info->hal_com,
									  &arg->txpwr, arg->cur_phy);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: tx power = %d\n", __FUNCTION__, arg->txpwr);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_get_pwr_idx(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: tx path = %d\n", __FUNCTION__, arg->tx_path);
	PHL_INFO("%s: is ofdm = %d\n", __FUNCTION__, arg->is_cck);

	hal_status = rtw_hal_bb_get_pwr_index(mp->hal,
					      &arg->txpwr_index,
					      arg->tx_path, arg->is_cck);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: tx power index = %d\n", __FUNCTION__, arg->txpwr_index);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_set_pwr_idx(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: tx path = %d\n", __FUNCTION__, arg->tx_path);
	PHL_INFO("%s: is ofdm = %d\n", __FUNCTION__, arg->is_cck);

	hal_status = rtw_hal_bb_set_pwr_index(mp->hal,
					      arg->txpwr_index,
					      arg->tx_path, arg->is_cck);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: tx power index = %d\n", __FUNCTION__, arg->txpwr_index);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_get_thermal(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: rf path = %d\n", __FUNCTION__, arg->rfpath);

	hal_status = rtw_hal_rf_get_thermal(mp->hal, arg->rfpath, &arg->thermal);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: thermal = %d\n", __FUNCTION__, arg->thermal);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_set_tssi(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: rf path = %d\n", __FUNCTION__, arg->rfpath);
	PHL_INFO("%s: tssi = %d\n", __FUNCTION__, arg->tssi);

	hal_status = rtw_hal_rf_set_tssi(mp->hal, arg->cur_phy, arg->rfpath, arg->tssi);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_set_tssi_offset(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: rf path = %d\n", __FUNCTION__, arg->rfpath);
	PHL_INFO("%s: tssi_de_offset = %d\n", __FUNCTION__, arg->tssi_de_offset);

	hal_status = rtw_hal_rf_set_tssi_offset(mp->hal, arg->cur_phy,arg->tssi_de_offset,arg->rfpath);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_get_tssi(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: rf path = %d\n", __FUNCTION__, arg->rfpath);

	hal_status = rtw_hal_rf_get_tssi(mp->hal, arg->cur_phy, arg->rfpath, &arg->tssi);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: tssi = %d\n", __FUNCTION__, arg->tssi);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_get_online_tssi_de(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: rf path = %d\n", __FUNCTION__, arg->rfpath);
	PHL_INFO("%s: dbm = %d\n", __FUNCTION__, arg->dbm);
	PHL_INFO("%s: pout = %d\n", __FUNCTION__, arg->pout);

	hal_status = rtw_hal_rf_get_online_tssi_de(mp->hal, arg->cur_phy, arg->rfpath, arg->dbm, arg->pout, &arg->online_tssi_de);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: online tssi de = %d\n", __FUNCTION__, arg->online_tssi_de);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_set_pwr_lmt_en(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = mp->hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	PHL_INFO("%s: arg pwr_lmt_en = %d\n", __FUNCTION__, arg->pwr_lmt_en);

	hal_status = rtw_hal_mac_set_pwr_lmt_en_val(hal_com, arg->cur_phy, arg->pwr_lmt_en);
	PHL_INFO("%s: set_pwr_lmt_en_val status = %d\n", __FUNCTION__, hal_status);

	if (hal_status == RTW_HAL_STATUS_FAILURE)
		return hal_status;
	hal_status = rtw_hal_mac_write_pwr_limit_en(hal_com, arg->cur_phy);
	PHL_INFO("%s:write_pwr_limit_en status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_get_pwr_lmt_en(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	struct hal_info_t *hal_info = mp->hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	PHL_INFO("%s: \n", __FUNCTION__);
	arg->pwr_lmt_en = rtw_hal_mac_get_pwr_lmt_en_val(hal_com, arg->cur_phy);

	PHL_INFO("%s: pwr_lmt_en = %d\n", __FUNCTION__, arg->pwr_lmt_en);

	return RTW_HAL_STATUS_SUCCESS;
}


enum rtw_hal_status rtw_hal_mp_txpwr_get_pwr_ref(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: is cck=%d, tx path = %d\n", __FUNCTION__,
		 arg->is_cck, arg->tx_path);

	/* Call hal API */
	hal_status = rtw_hal_bb_get_txpwr_ref(mp->hal,
					      arg->is_cck,
					      arg->tx_path,
					      &arg->txpwr_ref);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: txpwr_ref = %d\n", __FUNCTION__, arg->txpwr_ref);

	return hal_status;
}
enum rtw_hal_status rtw_hal_mp_txpwr_get_pwr_ref_cw(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: is cck=%d, tx path = %d\n", __FUNCTION__,
		 arg->is_cck, arg->tx_path);

	/* Call hal API */
	hal_status = rtw_hal_bb_get_pwr_index(mp->hal,
					      &arg->txpwr_index,
					      arg->tx_path,
						  arg->is_cck
					      );
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: txpwr_ref = %d\n", __FUNCTION__, arg->txpwr_index);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_get_txinfo_pwr(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	/* Call hal API */
	hal_status = rtw_hal_bb_get_txinfo_power(mp->hal,
									  &arg->txpwr);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: txinfo power = %d\n", __FUNCTION__, arg->txpwr);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_txpwr_ctrl_rf_mode(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	/* Call hal API */
	hal_status = rtw_hal_bb_ctrl_rf_mode(mp->hal,
									  arg->rf_mode);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: rf mode = %d\n", __FUNCTION__, arg->rf_mode);

	return hal_status;
}

void rtw_hal_mp_set_tx_pow_patten_sharp(struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	rtw_hal_bb_set_pow_patten_sharp(hal_info->hal_com, arg->channel, arg->is_cck, arg->sharp_id, arg->cur_phy);
}


#endif /* CONFIG_HAL_TEST_MP */
