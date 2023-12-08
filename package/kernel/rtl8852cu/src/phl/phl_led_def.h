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
#ifndef _PHL_LED_DEF_H_
#define _PHL_LED_DEF_H_

#define PHL_RADIO_ON_OFF_NOT_READY

enum rtw_led_ctrl_mode {
	RTW_LED_CTRL_NOT_SUPPORT,
	RTW_LED_CTRL_HW_TRX_MODE,
	RTW_LED_CTRL_HW_TX_MODE,
	RTW_LED_CTRL_HW_RX_MODE,
	RTW_LED_CTRL_SW_PP_MODE,
	RTW_LED_CTRL_SW_OD_MODE,
};

enum rtw_led_id { RTW_LED_ID_0, RTW_LED_ID_1, RTW_LED_ID_LENGTH };

#define RTW_LED_TIMER_LENGTH (RTW_LED_ID_LENGTH + 1)

/*
 * led_event here is not integrated with msg_evt_id due to the following reason:
 *
 * (a) led_event is used for mapping LED events with LED actions, and
 *     the mapping can be configured in core layer according to the
 *     customized LED table.
 *
 * (b) LED module inside uses led_event as the index of led action
 *     arrays, and hence it would be inappropriate to directly replace
 *     led_event with msg_evt_id which is not continuous and does not
 *     start from zero.
 */
enum rtw_led_event {
	RTW_LED_EVENT_PHL_START,
	RTW_LED_EVENT_PHL_STOP,
	RTW_LED_EVENT_SW_RF_ON,
	RTW_LED_EVENT_SW_RF_OFF,
	RTW_LED_EVENT_SCAN_START,
	RTW_LED_EVENT_LINK_START,
	RTW_LED_EVENT_LINKED,
	RTW_LED_EVENT_NO_LINK,
	RTW_LED_EVENT_LINKED_CIPHER,
	RTW_LED_EVENT_LINKED_NOCIPHER,
	RTW_LED_EVENT_LINKED_24G,
	RTW_LED_EVENT_LINKED_5G,
	RTW_LED_EVENT_WPS,
	RTW_LED_EVENT_LENGTH
};

enum rtw_led_state {
	RTW_LED_STATE_IGNORE = BIT0,
	RTW_LED_STATE_SW_RF_ON = BIT1,
};

enum rtw_led_action {
	RTW_LED_ACTION_LOW,
	RTW_LED_ACTION_HIGH,
	RTW_LED_ACTION_HW_TRX,
	RTW_LED_ACTION_TOGGLE,
};

enum rtw_led_opt {
	RTW_LED_OPT_LOW = 0,
	RTW_LED_OPT_HIGH = 1,
	RTW_LED_OPT_UNKNOWN = 2
};

struct rtw_led_toggle_args_t {
	enum rtw_led_opt start_opt;
	u32 start_delay;
	u32 loop;
	u8 intervals_idx;
};

struct rtw_led_action_args_t {
	enum rtw_led_id led_id;
	enum rtw_led_action led_action;
	struct rtw_led_toggle_args_t toggle_args;
};

struct rtw_led_intervals_t {
	u32 *interval_arr;
	u8 len;
};

#endif /*_PHL_LED_DEF_H_*/
