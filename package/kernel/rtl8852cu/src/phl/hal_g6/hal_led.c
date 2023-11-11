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
#define _HAL_LED_C_
#include "hal_headers.h"

#define HAL_LED_GET_CTRL_MODE(m)                                               \
	((m) == RTW_LED_CTRL_SW_PP_MODE                                        \
	     ? MAC_AX_LED_MODE_SW_CTRL_PP                                      \
	     : ((m) == RTW_LED_CTRL_SW_OD_MODE                                 \
		    ? MAC_AX_LED_MODE_SW_CTRL_OD                               \
		    : ((m) == RTW_LED_CTRL_HW_TX_MODE                          \
			   ? MAC_AX_LED_MODE_TX_ON                             \
			   : ((m) == RTW_LED_CTRL_HW_RX_MODE                   \
				  ? MAC_AX_LED_MODE_RX_ON                      \
				  : MAC_AX_LED_MODE_TRX_ON))))

#define HAL_LED_GET_ID(i)                                                      \
	((i) == RTW_LED_ID_0 ? 0 : ((i) == RTW_LED_ID_1 ? 1 : 0))

enum rtw_hal_status rtw_hal_led_set_ctrl_mode(void *hal, enum rtw_led_id led_id,
					      enum rtw_led_ctrl_mode ctrl_mode)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
		  "%s: led_id == %d, ctrl_mode == %d\n", __func__, led_id,
		  ctrl_mode);

	if (ctrl_mode == RTW_LED_CTRL_NOT_SUPPORT)
		return RTW_HAL_STATUS_SUCCESS;

	return rtw_hal_mac_led_set_ctrl_mode(
	    hal_info, HAL_LED_GET_CTRL_MODE(ctrl_mode), HAL_LED_GET_ID(led_id));
}

enum rtw_hal_status rtw_hal_led_control(void *hal, enum rtw_led_id led_id,
					u8 high)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	PHL_TRACE(COMP_PHL_LED, _PHL_INFO_, "===> hal_led_ctrl()\n");

	if (high) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
			  "hal_led_ctrl() : LED HIGH\n");
		status =
		    rtw_hal_mac_led_ctrl(hal_info, 1, HAL_LED_GET_ID(led_id));
	} else {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
			  "hal_led_ctrl() : LED LOW\n");
		status =
		    rtw_hal_mac_led_ctrl(hal_info, 0, HAL_LED_GET_ID(led_id));
	}

	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_LED, _PHL_INFO_,
			  "hal_led_ctrl() : rtw_hal_mac_led_ctrl() failed\n");
	}

	return status;
}
