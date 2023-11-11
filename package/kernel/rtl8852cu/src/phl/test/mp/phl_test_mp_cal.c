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
#define _PHL_TEST_MP_CAL_C_
#include "../../phl_headers.h"
#include "phl_test_mp_def.h"
#include "../../hal_g6/test/mp/hal_test_mp_api.h"

#ifdef CONFIG_PHL_TEST_MP
static enum rtw_phl_status phl_mp_cal_trigger(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_cal_trigger(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_set_capability(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_cal_set_capability(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_get_capability(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_cal_get_capability(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("enable = %d. \n", arg->enable);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_get_tssi_de(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_cal_get_tssi_de(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_set_tssi_de(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_cal_set_tssi_de(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_get_txpwr_final_abs(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_cal_get_txpwr_final_abs(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_trigger_dpk_tracking(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_cal_trigger_dpk_tracking(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_set_tssi_avg(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_set_tssi_avg(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

/* PSD */
static enum rtw_phl_status phl_mp_cal_psd_init(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_psd_init(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_psd_restore(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_psd_restore(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_psd_get_point_data(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_psd_get_point_data(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_psd_query(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_psd_query(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_cal_event_trigger(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_event_trigger(mp, arg);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_trigger_watchdog_cal(
	struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_trigger_watchdog_cal(mp);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_cal_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status mp_cal(struct mp_context *mp, struct mp_cal_arg *arg)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	switch(arg->cmd){
	case MP_CAL_CMD_TRIGGER_CAL:
		PHL_INFO("%s: CMD = MP_CAL_CMD_TRIGGER_CAL\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_trigger(mp, arg);
		break;
	case MP_CAL_CMD_SET_CAPABILITY_CAL:
		PHL_INFO("%s: CMD = MP_CAL_CMD_SET_CAPABILITY_CAL\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_set_capability(mp, arg);
		break;
	case MP_CAL_CMD_GET_CAPABILITY_CAL:
		PHL_INFO("%s: CMD = MP_CAL_CMD_GET_CAPABILITY_CAL\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_get_capability(mp, arg);
		break;
	case MP_CAL_CMD_GET_TSSI_DE_VALUE:
		PHL_INFO("%s: CMD = MP_CAL_CMD_GET_TSSI_DE_VALUE\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_get_tssi_de(mp, arg);
		break;
	case MP_CAL_CMD_SET_TSSI_DE_TX_VERIFY:
		PHL_INFO("%s: CMD = MP_CAL_CMD_SET_TSST_DE_TX_VERIFY\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_set_tssi_de(mp, arg);
		break;
	case MP_CAL_CMD_GET_TXPWR_FINAL_ABS:
		PHL_INFO("%s: CMD = MP_CAL_CMD_GET_TXPWR_FINAL_ABS\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_get_txpwr_final_abs(mp, arg);
		break;
	case MP_CAL_CMD_TRIGGER_DPK_TRACKING:
		PHL_INFO("%s: CMD = MP_CAL_CMD_TRIGGER_DPK_TRACKING\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_trigger_dpk_tracking(mp, arg);
		break;
	case MP_CAL_CMD_SET_TSSI_AVG:
		PHL_INFO("%s: CMD = MP_CAL_CMD_SET_TSSI_AVG\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_set_tssi_avg(mp, arg);
		break;
	case MP_CAL_CMD_PSD_INIT:
		PHL_INFO("%s: CMD = MP_CAL_CMD_PSD_INIT\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_psd_init(mp, arg);
		break;
	case MP_CAL_CMD_PSD_RESTORE:
		PHL_INFO("%s: CMD = MP_CAL_CMD_PSD_RESTORE\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_psd_restore(mp, arg);
		break;
	case MP_CAL_CMD_PSD_GET_POINT_DATA:
		PHL_INFO("%s: CMD = MP_CAL_CMD_PSD_GET_POINT_DATA\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_psd_get_point_data(mp, arg);
		break;
	case MP_CAL_CMD_PSD_QUERY:
		PHL_INFO("%s: CMD = MP_CAL_CMD_PSD_QUERY\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_psd_query(mp, arg);
		break;
	case MP_CAL_CMD_EVENT_TRIGGER:
		PHL_INFO("%s: CMD = MP_CAL_CMD_EVENT_TRIGGER\n",
			 __FUNCTION__);
		phl_status = phl_mp_cal_event_trigger(mp, arg);
		break;
	case MP_CAL_CMD_TRIGGER_WATCHDOG_CAL:
		PHL_INFO("%s: CMD = MP_CAL_CMD_TRIGGER_WATCHDOG_CAL\n",
			 __FUNCTION__);
		phl_status = phl_mp_trigger_watchdog_cal(mp, arg);
		break;
	default:
		PHL_WARN("%s: CMD NOT RECOGNIZED\n", __FUNCTION__);
		break;
	}
 	return phl_status;
}
#endif /* CONFIG_PHL_TEST_MP */
