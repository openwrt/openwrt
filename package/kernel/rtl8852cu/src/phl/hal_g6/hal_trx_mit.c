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
#define _HAL_TRX_MIT_C_
#include "hal_headers.h"

static void
_hal_trx_mit_timer_convert(u32 timer, u8 *mul_ptr,
			   enum mac_ax_trx_mitigation_timer_unit *unit_ptr)
{
	/* timer = mul * unit */

	const enum mac_ax_trx_mitigation_timer_unit UNIT_ARR[] = {
	    MAC_AX_MIT_64US, MAC_AX_MIT_128US, MAC_AX_MIT_256US,
	    MAC_AX_MIT_512US};
	const u8 UNIT_EXP_ARR[] = {6, 7, 8, 9}; /* 2^exp = unit */
	const u8 UNIT_ARR_LEN = 4;

	const u8 MUL_MAX = 0xff; /* 8 bits for mul */

	u32 timer_ = 0;
	u8 idx;

	for (idx = 0; idx < UNIT_ARR_LEN; idx++) {

		timer_ = timer >> UNIT_EXP_ARR[idx];

		if (timer_ <= MUL_MAX)
			break;
	}

	if (timer_ > MUL_MAX)
		*mul_ptr = MUL_MAX;
	else
		*mul_ptr = (u8)timer_;

	if (idx < UNIT_ARR_LEN)
		*unit_ptr = UNIT_ARR[idx];
	else
		*unit_ptr = UNIT_ARR[UNIT_ARR_LEN - 1];
}

enum rtw_hal_status rtw_hal_pcie_trx_mit(void *hal, u32 tx_timer, u8 tx_counter,
					 u32 rx_timer, u8 rx_counter)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	struct mac_ax_pcie_trx_mitigation mit_info;
	struct mac_ax_txdma_ch_map txch_map;
	struct mac_ax_rxdma_ch_map rxch_map;

	/* tx */

	txch_map.ch0 = MAC_AX_PCIE_DISABLE;
	txch_map.ch1 = MAC_AX_PCIE_DISABLE;
	txch_map.ch2 = MAC_AX_PCIE_DISABLE;
	txch_map.ch3 = MAC_AX_PCIE_DISABLE;
	txch_map.ch4 = MAC_AX_PCIE_DISABLE;
	txch_map.ch5 = MAC_AX_PCIE_DISABLE;
	txch_map.ch6 = MAC_AX_PCIE_DISABLE;
	txch_map.ch7 = MAC_AX_PCIE_DISABLE;
	txch_map.ch8 = MAC_AX_PCIE_DISABLE;
	txch_map.ch9 = MAC_AX_PCIE_DISABLE;
	txch_map.ch10 = MAC_AX_PCIE_DISABLE;
	txch_map.ch11 = MAC_AX_PCIE_DISABLE;
	txch_map.ch12 = MAC_AX_PCIE_DISABLE;

	mit_info.txch_map = &txch_map;
	mit_info.tx_counter = tx_counter;

	_hal_trx_mit_timer_convert(tx_timer, &(mit_info.tx_timer),
				   &(mit_info.tx_timer_unit));

	PHL_INFO(
	    "%s :: mit_info.tx_timer == %d, mit_info.tx_timer_unit == %d\n",
	    __func__, mit_info.tx_timer, mit_info.tx_timer_unit);

	/* tx - END */

	/* rx */

	rxch_map.rxq = MAC_AX_PCIE_ENABLE;
	rxch_map.rpq = MAC_AX_PCIE_ENABLE;

	mit_info.rxch_map = &rxch_map;
	mit_info.rx_counter = rx_counter;

	_hal_trx_mit_timer_convert(rx_timer, &(mit_info.rx_timer),
				   &(mit_info.rx_timer_unit));

	PHL_INFO(
	    "%s :: mit_info.rx_timer == %d, mit_info.rx_timer_unit == %d\n",
	    __func__, mit_info.rx_timer, mit_info.rx_timer_unit);

	/* rx - END */

	if (rtw_hal_mac_pcie_trx_mit(hal_info, &mit_info) !=
	    RTW_HAL_STATUS_SUCCESS) {

		PHL_INFO(
		    "%s :: failed to config pcie trx interrupt mitigation\n",
		    __func__);
		return RTW_HAL_STATUS_FAILURE;
	}

	return RTW_HAL_STATUS_SUCCESS;
}
