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
#define _HAL_TEST_MP_CAL_C_
#include "../../hal_headers.h"
#include "../../../test/mp/phl_test_mp_def.h"

#ifdef CONFIG_HAL_TEST_MP
enum rtw_hal_status rtw_hal_mp_cal_trigger(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	PHL_INFO("%s: cal_type = %d. \n",__FUNCTION__, arg->cal_type);

	switch(arg->cal_type){
	case MP_CAL_CHL_RFK:
		PHL_INFO("MP_CAL_CHL_RFK: cur_phy_idx = %d.\n", mp->cur_phy);
		hal_status = rtw_hal_rf_chl_rfk_trigger(hal_com, mp->cur_phy, false);
		break;
	case MP_CAL_DACK:
		PHL_INFO("MP_CAL_DACK: . \n");
		hal_status = rtw_hal_rf_dack_trigger(mp->hal, false);
		break;
	case MP_CAL_IQK:
		PHL_INFO("MP_CAL_IQK: cur_phy_idx = %d.\n", mp->cur_phy);
		hal_status = rtw_hal_rf_iqk_trigger(mp->hal, mp->cur_phy, false);
		break;
	case MP_CAL_LCK:
		PHL_INFO("MP_CAL_LCK \n");
		hal_status = rtw_hal_rf_lck_trigger(mp->hal);
		break;
	case MP_CAL_DPK:
		PHL_INFO("MP_CAL_DPK: cur_phy_idx = %d.\n", mp->cur_phy);
		hal_status = rtw_hal_rf_dpk_trigger(mp->hal, mp->cur_phy, false);
		break;
	case MP_CAL_TSSI:
		PHL_INFO("MP_CAL_TSSI: cur_phy_idx = %d.\n", mp->cur_phy);
		hal_status = rtw_hal_rf_tssi_trigger(mp->hal, mp->cur_phy);
		break;
	case MP_CAL_GAPK:
		PHL_INFO("MP_CAL_GAPK: cur_phy_idx = %d.\n", mp->cur_phy);
		hal_status = rtw_hal_rf_gapk_trigger(mp->hal, mp->cur_phy, false);
		break;
	default:
		PHL_INFO("Unknown calibration type.\n");
		break;
	}

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_cal_set_capability(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: cal_type = %d, enable = %d\n",
		 __FUNCTION__, arg->cal_type, arg->enable);

	switch(arg->cal_type){
	case MP_CAL_DACK:
		PHL_INFO("MP_CAL_DACK.\n");
		hal_status = rtw_hal_rf_set_capability_dack(mp->hal,
							    arg->enable);
		break;
	case MP_CAL_IQK:
		PHL_INFO("MP_CAL_IQK.\n");
		hal_status = rtw_hal_rf_set_capability_iqk(mp->hal,
							   arg->enable);
		break;
	case MP_CAL_DPK:
		PHL_INFO("MP_CAL_DPK.\n");
		hal_status = rtw_hal_rf_set_capability_dpk(mp->hal,
							   arg->enable);
		break;
	case MP_CAL_DPK_TRACK:
		PHL_INFO("MP_CAL_DPK_TRACK.\n");
		hal_status = rtw_hal_rf_set_capability_dpk_track(mp->hal,
								 arg->enable);
		break;
	case MP_CAL_TSSI:
		PHL_INFO("MP_CAL_TSSI.\n");
		hal_status = rtw_hal_rf_set_capability_tssi(mp->hal,
							    arg->enable);
		break;
	case MP_CAL_GAPK:
		PHL_INFO("MP_CAL_GAPK.\n");
		hal_status = rtw_hal_rf_set_capability_gapk(mp->hal,
							    arg->enable);
		break;
	default:
		PHL_INFO("Unknown calibration type.\n");
		break;
	}

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_cal_get_capability(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: cal_type = %d. \n",__FUNCTION__,arg->cal_type);

	switch(arg->cal_type){
	case MP_CAL_DACK:
		PHL_INFO("MP_CAL_DACK.\n");
		hal_status = rtw_hal_rf_get_capability_dack(mp->hal,
							    &arg->enable);
		break;
	case MP_CAL_IQK:
		PHL_INFO("MP_CAL_IQK.\n");
		hal_status = rtw_hal_rf_get_capability_iqk(mp->hal,
							   &arg->enable);
		break;
	case MP_CAL_DPK:
		PHL_INFO("MP_CAL_DPK.\n");
		hal_status = rtw_hal_rf_get_capability_dpk(mp->hal,
							   &arg->enable);
		break;
	case MP_CAL_DPK_TRACK:
		PHL_INFO("MP_CAL_DPK_TRACK.\n");
		hal_status = rtw_hal_rf_get_capability_dpk_track(mp->hal,
								 &arg->enable);
		break;
	case MP_CAL_TSSI:
		PHL_INFO("MP_CAL_TSSI.\n");
		hal_status = rtw_hal_rf_get_capability_tssi(mp->hal,
							    &arg->enable);
		break;
	case MP_CAL_GAPK:
		PHL_INFO("MP_CAL_GAPK.\n");
		hal_status = rtw_hal_rf_get_capability_gapk(mp->hal,
							    &arg->enable);
		break;
	default:
		PHL_INFO("Unknown calibration type.\n");
		break;
	}

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_cal_get_tssi_de(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	/*hal_status = rtw_hal_rf_get_tssi_de_value(mp, arg);*/
	hal_status = RTW_HAL_STATUS_SUCCESS;
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_cal_set_tssi_de(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	/*hal_status = rtw_hal_rf_set_tssi_de_tx_verify(mp->hal, arg);*/
	hal_status = RTW_HAL_STATUS_SUCCESS;
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_cal_get_txpwr_final_abs(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	/*hal_status = rtw_hal_rf_get_txpwr_final_abs(mp->hal, arg);*/
	hal_status = RTW_HAL_STATUS_SUCCESS;
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_cal_trigger_dpk_tracking(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_rf_trigger_dpk_tracking(mp->hal);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_set_tssi_avg(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_rf_set_tssi_avg(mp->hal, mp->cur_phy, arg->xdbm);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

/* PSD */

enum rtw_hal_status rtw_hal_mp_psd_init(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_rf_psd_init(mp->hal, mp->cur_phy, arg->path,
						arg->iq_path, arg->avg, arg->fft);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}
enum rtw_hal_status rtw_hal_mp_psd_restore(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_rf_psd_restore(mp->hal, mp->cur_phy);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}
enum rtw_hal_status rtw_hal_mp_psd_get_point_data(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_rf_psd_get_point_data(mp->hal, mp->cur_phy,
						arg->point, &arg->buf);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}
enum rtw_hal_status rtw_hal_mp_psd_query(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_rf_psd_query(mp->hal, mp->cur_phy, arg->upoint,
				arg->start_point, arg->stop_point, arg->outbuf);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_event_trigger(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;

	PHL_INFO("%s\n", __FUNCTION__);

	rtw_hal_rf_test_event_trigger(mp->hal, mp->cur_phy,
						arg->event,
						arg->func,
						&arg->buf);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_trigger_watchdog_cal(
	struct mp_context *mp)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_bb_watchdog(hal_info, false);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s rtw_hal_bb_watchdog fail (%x)\n", __FUNCTION__, hal_status);
		goto exit;
	}

	hal_status = rtw_hal_rf_watchdog(hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s rtw_hal_rf_watchdog fail (%x)\n", __FUNCTION__, hal_status);
		goto exit;
	}

	hal_status = rtw_hal_mac_watchdog(hal_info, mp->phl_com);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s rtw_hal_mac_watchdog fail (%x)\n", __FUNCTION__, hal_status);
		goto exit;
	}
exit:
	return hal_status;
}

#endif /* CONFIG_HAL_TEST_MP */
