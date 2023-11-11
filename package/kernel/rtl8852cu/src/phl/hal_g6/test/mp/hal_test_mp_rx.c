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
#define _HAL_TEST_MP_RX_C_
#include "../../hal_headers.h"
#include "../../../test/mp/phl_test_mp_def.h"

#ifdef CONFIG_HAL_TEST_MP
enum rtw_hal_status rtw_hal_mp_rx_phy_crc_ok(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, arg->rx_phy_idx);

	hal_status = rtw_hal_bb_get_rx_ok(mp->hal,
				arg->rx_phy_idx, &arg->rx_ok);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: crc ok count = %d\n", __FUNCTION__, arg->rx_ok);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_phy_crc_err(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, arg->rx_phy_idx);

	hal_status = rtw_hal_bb_get_rx_crc(mp->hal, arg->rx_phy_idx, &arg->rx_err);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: crc error count = %d\n", __FUNCTION__, arg->rx_err);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_mac_crc_ok(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_mac_get_rx_cnt(mp->hal, arg->rx_phy_idx, MAC_AX_RX_CRC_OK, &arg->rx_ok);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: mac crc OK count = %d\n", __FUNCTION__, arg->rx_ok);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_mac_crc_err(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_mac_get_rx_cnt(mp->hal, arg->rx_phy_idx, MAC_AX_RX_CRC_FAIL, &arg->rx_err);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: mac crc error count = %d\n", __FUNCTION__, arg->rx_err);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_drv_crc_ok(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_trx_stat *trx_stat = &hal_com->trx_stat;

	PHL_INFO("%s\n", __FUNCTION__);

	arg->rx_ok = trx_stat->rx_ok_cnt;

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: drv ok count = %d\n", __FUNCTION__, arg->rx_ok);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_drv_crc_err(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_trx_stat *trx_stat = &hal_com->trx_stat;

	PHL_INFO("%s\n", __FUNCTION__);

	arg->rx_err = trx_stat->rx_err_cnt;

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: drv err count = %d\n", __FUNCTION__, arg->rx_err);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_get_rssi(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: rx_path: %d\n", __FUNCTION__, arg->rx_path);

	hal_status = rtw_hal_bb_get_rssi(mp->hal, arg->rx_path, &arg->rssi);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: rssi = %d\n", __FUNCTION__, arg->rssi);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_get_rssi_ex(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: rx_path: %d\n", __FUNCTION__, arg->rx_path);

	hal_status = rtw_hal_bb_get_rssi_ex(mp->hal, arg->rssi_ex,
					arg->rx_path, arg->strm, arg->rx_phy_idx);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_get_rxevm(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, mp->cur_phy);

	hal_status = rtw_hal_bb_get_rxevm(mp->hal, arg->user, arg->strm, arg->rxevm_table, &arg->rx_evm);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: rx_evm = %d\n", __FUNCTION__, arg->rx_evm);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_trigger_rxevm(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, arg->rx_phy_idx);

	hal_status = rtw_hal_bb_trigger_rxevm(mp->hal, &arg->phy_user0_rxevm,
	                                               &arg->phy_user1_rxevm,
	                                               &arg->phy_user2_rxevm,
	                                               &arg->phy_user3_rxevm,
	                                               arg->rx_phy_idx);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	PHL_INFO("%s: phy_user0_rxevm = %d\n", __FUNCTION__, arg->phy_user0_rxevm);
	PHL_INFO("%s: phy_user1_rxevm = %d\n", __FUNCTION__, arg->phy_user1_rxevm);
	PHL_INFO("%s: phy_user2_rxevm = %d\n", __FUNCTION__, arg->phy_user2_rxevm);
	PHL_INFO("%s: phy_user3_rxevm = %d\n", __FUNCTION__, arg->phy_user3_rxevm);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_rx_set_gain_offset(
	struct mp_context *mp, struct mp_rx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: phy index: %d\n", __FUNCTION__, mp->cur_phy);

	PHL_INFO("%s: offset: %d\n", __FUNCTION__, arg->offset);
	PHL_INFO("%s: rf_path: %d\n", __FUNCTION__, arg->rf_path);

	hal_status = rtw_hal_bb_set_gain_offset(mp->hal, arg->offset, arg->rf_path, mp->cur_phy, arg->iscck);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

#endif /* CONFIG_HAL_TEST_MP */
