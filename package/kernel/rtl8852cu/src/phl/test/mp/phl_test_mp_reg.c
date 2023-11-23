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
#define _PHL_TEST_MP_REG_C_
#include "../../phl_headers.h"
#include "phl_test_mp_def.h"
#include "../../hal_g6/test/mp/hal_test_mp_api.h"

#ifdef CONFIG_PHL_TEST_MP
static enum rtw_phl_status phl_mp_reg_read_macreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_read_macreg(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_write_macreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_write_macreg(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_read_rfreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_read_rfreg(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_write_rfreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_write_rfreg(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_read_rf_syn(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_read_rf_syn(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_write_rf_syn(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_write_rf_syn(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_read_bbreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_read_bbreg(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_write_bbreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_write_bbreg(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_get_xcap(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_get_xcap(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_set_xcap(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_set_xcap(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_get_xsi(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_get_xsi(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_reg_set_xsi(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_reg_set_xsi(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_reg_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status mp_reg(struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	switch(arg->cmd){
	case MP_REG_CMD_READ_MAC:
		PHL_INFO("%s: CMD = MP_REG_CMD_READ_MAC\n", __FUNCTION__);
		phl_status = phl_mp_reg_read_macreg(mp, arg);
		break;
	case MP_REG_CMD_WRITE_MAC:
		PHL_INFO("%s: CMD = MP_REG_CMD_WRITE_MAC\n", __FUNCTION__);
		phl_status = phl_mp_reg_write_macreg(mp, arg);
		break;
	case MP_REG_CMD_READ_RF:
		PHL_INFO("%s: CMD = MP_REG_CMD_READ_RF\n", __FUNCTION__);
		phl_status = phl_mp_reg_read_rfreg(mp, arg);
		break;
	case MP_REG_CMD_WRITE_RF:
		PHL_INFO("%s: CMD = MP_REG_CMD_WRITE_RF\n", __FUNCTION__);
		phl_status = phl_mp_reg_write_rfreg(mp, arg);
		break;
	case MP_REG_CMD_READ_SYN:
		PHL_INFO("%s: CMD = MP_REG_CMD_READ_SYN\n", __FUNCTION__);
		phl_status = phl_mp_reg_read_rf_syn(mp, arg);
		break;
	case MP_REG_CMD_WRITE_SYN:
		PHL_INFO("%s: CMD = MP_REG_CMD_WRITE_SYN\n", __FUNCTION__);
		phl_status = phl_mp_reg_write_rf_syn(mp, arg);
		break;
	case MP_REG_CMD_READ_BB:
		PHL_INFO("%s: CMD = MP_REG_CMD_READ_BB\n", __FUNCTION__);
		phl_status = phl_mp_reg_read_bbreg(mp, arg);
		break;
	case MP_REG_CMD_WRITE_BB:
		PHL_INFO("%s: CMD = MP_REG_CMD_WRITE_BB\n", __FUNCTION__);
		phl_status = phl_mp_reg_write_bbreg(mp, arg);
		break;
	case MP_REG_CMD_GET_XCAP:
		PHL_INFO("%s: CMD = MP_REG_CMD_GET_XCAP\n", __FUNCTION__);
		phl_status = phl_mp_reg_get_xcap(mp, arg);
		break;
	case MP_REG_CMD_SET_XCAP:
		PHL_INFO("%s: CMD = MP_REG_CMD_SET_XCAP\n", __FUNCTION__);
		phl_status = phl_mp_reg_set_xcap(mp, arg);
		break;
	case MP_REG_CMD_GET_XSI:
		PHL_INFO("%s: CMD = MP_REG_CMD_GET_XSI\n", __FUNCTION__);
		phl_status = phl_mp_reg_get_xsi(mp, arg);
		break;
	case MP_REG_CMD_SET_XSI:
		PHL_INFO("%s: CMD = MP_REG_CMD_SET_XSI\n", __FUNCTION__);
		phl_status = phl_mp_reg_set_xsi(mp, arg);
		break;
	default:
		PHL_WARN("%s: CMD NOT RECOGNIZED\n", __FUNCTION__);
		break;
	}

	return phl_status;
}
#endif /* CONFIG_PHL_TEST_MP */
