/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#define _HAL_THERMAL_C_
#include "hal_headers.h"

#ifdef CONFIG_PHL_THERMAL_PROTECT

enum rtw_hal_status
rtw_hal_thermal_protect_cfg_tx_ampdu(
	void *hal,
	struct rtw_phl_stainfo_t *sta,
	u8 ratio)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
	u8 tx_time = 0;
	u16 num_ampdu = 0;

	if (64 == sta->asoc_cap.num_ampdu)
		tx_time = 0xA5;
	else if (128 == sta->asoc_cap.num_ampdu)
		tx_time = 0xA5;
	else if (256 == sta->asoc_cap.num_ampdu)
		tx_time = 0xA5;

	if(sta->asoc_cap.num_ampdu_bk == 0)
		sta->asoc_cap.num_ampdu_bk = sta->asoc_cap.num_ampdu;

	sta->asoc_cap.num_ampdu = sta->asoc_cap.num_ampdu_bk * ratio / 100;
	num_ampdu = sta->asoc_cap.num_ampdu;

	if(num_ampdu == 0)
		num_ampdu = 1;

	hsts = rtw_hal_mac_set_hw_ampdu_cfg(hal_info, 0, num_ampdu, tx_time);

	PHL_INFO("%s: bk_num_ampdu = %d num_ampdu = %d, tx_time = %x\n",
		 __FUNCTION__, sta->asoc_cap.num_ampdu_bk, num_ampdu, tx_time);

	if (RTW_HAL_STATUS_SUCCESS != hsts)
		goto out;

out:
	return hsts;
}

bool rtw_hal_check_thermal_protect(
	struct rtw_phl_com_t *phl_com,
	void *hal
)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum halrf_thermal_status status = HALRF_THERMAL_STATUS_BELOW_THRESHOLD;
	bool action_changed = false;

	status = rtw_hal_rf_get_ther_protected_threshold(hal_info);

	PHL_INFO("%s: Cur action = %x\n", __FUNCTION__, phl_com->thermal_protect_action);
	PHL_INFO("%s: status = %x\n", __FUNCTION__, status);

	if (status == HALRF_THERMAL_STATUS_ABOVE_THRESHOLD) {
		if (phl_com->thermal_protect_action < PHL_THERMAL_PROTECT_ACTION_LEVEL_MAX) {
			if (phl_com->thermal_protect_action == PHL_THERMAL_PROTECT_ACTION_NONE)
				phl_com->drv_mode = RTW_DRV_MODE_HIGH_THERMAL;
			phl_com->thermal_protect_action++;
			action_changed = true;
		}
	} else if (status == HALRF_THERMAL_STATUS_BELOW_THRESHOLD) {
		if (phl_com->thermal_protect_action > PHL_THERMAL_PROTECT_ACTION_NONE) {
			if (phl_com->thermal_protect_action == PHL_THERMAL_PROTECT_ACTION_LEVEL1)
				phl_com->drv_mode = RTW_DRV_MODE_NORMAL;
			phl_com->thermal_protect_action--;
			action_changed = true;
		}
	} else {
		/* Do nothing */
	}
	if(action_changed)
		PHL_INFO("%s: Next action = %x\n", __FUNCTION__, phl_com->thermal_protect_action);
	return action_changed;
}

#endif /* CONFIG_PHL_THERMAL_PROTECT */


enum rtw_hal_status
rtw_hal_thermal_protect_cfg_tx_duty(
	void *hal,
	u16 tx_duty_interval,
	u8 ratio)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
	u16 pause_duration = 0, tx_duration = 0;

	tx_duration = tx_duty_interval * ratio / 100;
	pause_duration = tx_duty_interval - tx_duration;
	PHL_INFO("%s: tx duty interval = %d tx duration = %d, pause duration = %d\n",
		 __FUNCTION__, tx_duty_interval, tx_duration, pause_duration);

	hsts = rtw_hal_mac_set_tx_duty(hal_info, pause_duration, tx_duration);

	return hsts;
}

enum rtw_hal_status
rtw_hal_thermal_protect_stop_tx_duty(
	void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: Stop tx duty!\n", __FUNCTION__);

	hsts = rtw_hal_mac_stop_tx_duty(hal_info);

	return hsts;
}