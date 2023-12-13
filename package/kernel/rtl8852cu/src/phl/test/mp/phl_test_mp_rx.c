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
#define _PHL_TEST_MP_RX_C_
#include "../../phl_headers.h"
#include "phl_test_mp_def.h"
#include "../../hal_g6/test/mp/hal_test_mp_api.h"

#ifdef CONFIG_PHL_TEST_MP
static enum rtw_phl_status phl_mp_rx_phy_crc_ok(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_phy_crc_ok(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_phy_crc_err(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_phy_crc_err(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_mac_crc_ok(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_mac_crc_ok(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_mac_crc_err(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_mac_crc_err(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_drv_crc_ok(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_drv_crc_ok(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_drv_crc_err(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_drv_crc_err(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_get_rssi(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_get_rssi(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_get_rxevm(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_get_rxevm(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_get_physts(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	if(arg->enable) {
		mp->rx_physts = true;
	}
	else {
		mp->rx_physts = false;
	}
	/* Record the result */
	arg->cmd_ok = true;
	arg->status = RTW_HAL_STATUS_SUCCESS;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_mp_rx_get_rssi_ex(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_get_rssi_ex(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_trigger_rxevm(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_trigger_rxevm(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_rx_set_gain_offset(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_rx_set_gain_offset(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_rx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status mp_rx(struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	switch(arg->cmd){
	case MP_RX_CMD_PHY_CRC_OK:
		PHL_INFO("%s: CMD = MP_RX_CMD_PHY_CRC_OK\n", __FUNCTION__);
		phl_status = phl_mp_rx_phy_crc_ok(mp, arg);
		break;
	case MP_RX_CMD_PHY_CRC_ERR:
		PHL_INFO("%s: CMD = MP_RX_CMD_PHY_CRC_ERR\n", __FUNCTION__);
		phl_status = phl_mp_rx_phy_crc_err(mp, arg);
		break;
	case MP_RX_CMD_MAC_CRC_OK:
		PHL_INFO("%s: CMD = MP_RX_CMD_MAC_CRC_OK\n", __FUNCTION__);
		phl_status = phl_mp_rx_mac_crc_ok(mp, arg);
		break;
	case MP_RX_CMD_MAC_CRC_ERR:
		PHL_INFO("%s: CMD = MP_RX_CMD_MAC_CRC_ERR\n", __FUNCTION__);
		phl_status = phl_mp_rx_mac_crc_err(mp, arg);
		break;
	case MP_RX_CMD_DRV_CRC_OK:
		PHL_INFO("%s: CMD = MP_RX_CMD_DRV_CRC_OK\n", __FUNCTION__);
		phl_status = phl_mp_rx_drv_crc_ok(mp, arg);
		break;
	case MP_RX_CMD_DRV_CRC_ERR:
		PHL_INFO("%s: CMD = MP_RX_CMD_DRV_CRC_ERR\n", __FUNCTION__);
		phl_status = phl_mp_rx_drv_crc_err(mp, arg);
		break;
	case MP_RX_CMD_GET_RSSI:
		PHL_INFO("%s: CMD = MP_RX_CMD_GET_RSSI\n", __FUNCTION__);
		phl_status = phl_mp_rx_get_rssi(mp, arg);
		break;
	case MP_RX_CMD_GET_RXEVM:
		PHL_INFO("%s: CMD = MP_RX_CMD_GET_RXEVM\n", __FUNCTION__);
		phl_status = phl_mp_rx_get_rxevm(mp, arg);
		break;
	case MP_RX_CMD_GET_PHYSTS:
		PHL_INFO("%s: CMD = MP_RX_CMD_GET_PHYSTS\n", __FUNCTION__);
		phl_status = phl_mp_rx_get_physts(mp, arg);
		break;
	case MP_RX_CMD_TRIGGER_RXEVM:
		PHL_INFO("%s: CMD = MP_RX_CMD_TRIGGER_RXEVM\n", __FUNCTION__);
		phl_status = phl_mp_rx_trigger_rxevm(mp, arg);
		break;
	case MP_RX_CMD_SET_GAIN_OFFSET:
		PHL_INFO("%s: CMD = MP_RX_CMD_SET_GAIN_OFFSET\n", __FUNCTION__);
		phl_status = phl_mp_rx_set_gain_offset(mp, arg);
		break;
	case MP_RX_CMD_GET_RSSI_EX:
		PHL_INFO("%s: CMD = MP_RX_CMD_GET_RSSI\n", __FUNCTION__);
		phl_status = phl_mp_rx_get_rssi_ex(mp, arg);
		break;
	default:
		PHL_WARN("%s: CMD NOT RECOGNIZED\n", __FUNCTION__);
		break;
	}

	return phl_status;
}
#endif /* CONFIG_PHL_TEST_MP */
