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
#include "../../phl_headers.h"
#include "phl_test_mp_def.h"
#include "../../hal_g6/test/mp/hal_test_mp_api.h"

#ifdef CONFIG_PHL_TEST_MP
static enum rtw_phl_status phl_mp_txpwr_read_table(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_read_table(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_get_pwrtrack(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_get_pwrtrack(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_set_pwrtrack(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_set_pwrtrack(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_set_pwr(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_set_pwr(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_get_pwr(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_get_pwr(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_get_txinfo_pwr(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_get_txinfo_pwr(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_ctrl_rf_mode(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_ctrl_rf_mode(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_get_pwr_idx(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_get_pwr_idx(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_set_pwr_idx(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_set_pwr_idx(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_get_thermal(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_get_thermal(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_set_tssi(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_set_tssi(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_get_tssi(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_get_tssi(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_get_online_tssi_de(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_get_online_tssi_de(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_set_pwr_lmt_en(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_set_pwr_lmt_en(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_get_pwr_lmt_en(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_get_pwr_lmt_en(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_set_tssi_offset(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_set_tssi_offset(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_get_pwr_ref(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_get_pwr_ref(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_txpwr_get_pwr_ref_cw(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_txpwr_get_pwr_ref_cw(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_mp_set_tx_pow_patten_sharp(
	struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;

	rtw_hal_mp_set_tx_pow_patten_sharp(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_txpwr_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status mp_txpwr(struct mp_context *mp, struct mp_txpwr_arg *arg)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	switch(arg->cmd){
	case MP_TXPWR_CMD_READ_PWR_TABLE:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_READ_PWR_TABLE\n",
			__FUNCTION__);
		phl_status = phl_mp_txpwr_read_table(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_PWR_TRACK_STATUS:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_PWR_TRACK_STATUS\n",
			 __FUNCTION__);
		phl_status = phl_mp_txpwr_get_pwrtrack(mp, arg);
		break;
	case MP_TXPWR_CMD_SET_PWR_TRACK_STATUS:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_SET_PWR_TRACK_STATUS\n",
			 __FUNCTION__);
		phl_status = phl_mp_txpwr_set_pwrtrack(mp, arg);
		break;
	case MP_TXPWR_CMD_SET_TXPWR:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_SET_TXPWR\n", __FUNCTION__);
		phl_status = phl_mp_txpwr_set_pwr(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_TXPWR:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_TXPWR\n", __FUNCTION__);
		phl_status = phl_mp_txpwr_get_pwr(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_TXPWR_INDEX:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_TXPWR_INDEX\n",
			 __FUNCTION__);
		phl_status = phl_mp_txpwr_get_pwr_idx(mp, arg);
		break;
	case MP_TXPWR_CMD_SET_TXPWR_INDEX:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_SET_TXPWR_INDEX\n",
			 __FUNCTION__);
		phl_status = phl_mp_txpwr_set_pwr_idx(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_THERMAL:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_THERMAL\n", __FUNCTION__);
		phl_status = phl_mp_txpwr_get_thermal(mp, arg);
		break;
	case MP_TXPWR_CMD_SET_TSSI:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_SET_TSSI\n", __FUNCTION__);
		phl_status = phl_mp_txpwr_set_tssi(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_TSSI:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_TSSI\n", __FUNCTION__);
		phl_status = phl_mp_txpwr_get_tssi(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_TXPWR_REF:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_TXPWR_REF\n",
			 __FUNCTION__);
		phl_status = phl_mp_txpwr_get_pwr_ref(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_TXPWR_REF_CW:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_TXPWR_REF_CW\n",
			 __FUNCTION__);
		phl_status = phl_mp_txpwr_get_pwr_ref_cw(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_TXINFOPWR:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_TXINFOPWR\n", __FUNCTION__);
		phl_status = phl_mp_txpwr_get_txinfo_pwr(mp, arg);
		break;
	case MP_TXPWR_CMD_SET_RFMODE:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_SET_RFMODE\n", __FUNCTION__);
		phl_status = phl_mp_txpwr_ctrl_rf_mode(mp, arg);
		break;
	case MP_TXPWR_CMD_SET_TSSI_OFFSET:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_SET_TSSI\n", __FUNCTION__);
		phl_status = phl_mp_txpwr_set_tssi_offset(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_ONLINE_TSSI_DE:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_ONLINE_TSSI_DE\n", __FUNCTION__);
		phl_status = phl_mp_txpwr_get_online_tssi_de(mp, arg);
		break;
	case MP_TXPWR_CMD_SET_PWR_LMT_EN:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_SET_PWR_LMT_EN\n", __FUNCTION__);
		phl_status = phl_mp_set_pwr_lmt_en(mp, arg);
		break;
	case MP_TXPWR_CMD_GET_PWR_LMT_EN:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_GET_PWR_LMT_EN\n", __FUNCTION__);
		phl_status = phl_mp_get_pwr_lmt_en(mp, arg);
		break;
	case MP_TXPWR_CMD_SET_TX_POW_PATTERN_SHARP:
		PHL_INFO("%s: CMD = MP_TXPWR_CMD_SET_TX_POW_PATTERN_SHARP\n", __FUNCTION__);
		phl_status = phl_mp_set_tx_pow_patten_sharp(mp, arg);
		break;
	default:
		break;
	}

	return phl_status;
}
#endif /* CONFIG_PHL_TEST_MP */
