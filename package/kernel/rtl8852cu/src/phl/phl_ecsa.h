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
#ifndef _PHL_ECSA_H_
#define _PHL_ECSA_H_

#define ECSA_UPDATE_BCN_BEFORE_TBTT 25 /* unit: TU */
#define ECSA_UPDATE_BCN_BEFORE_TBTT_US (ECSA_UPDATE_BCN_BEFORE_TBTT * TU)

#define ECSA_DEFAULT_CHANNEL_SWITCH_COUNT 10
#define ECSA_SWITCH_TIME_AFTER_LAST_COUNT_DOWN (10 * TU)

#define IS_ECSA_TYPE_AP(_ecsa_ctrl) (_ecsa_ctrl->ecsa_param.ecsa_type == ECSA_TYPE_AP)
#define IS_ECSA_TYPE_STA(_ecsa_ctrl) (_ecsa_ctrl->ecsa_param.ecsa_type == ECSA_TYPE_STA)

enum phl_ecsa_state{
	ECSA_STATE_NONE = 0,
	ECSA_STATE_WAIT_DELAY = 1,
	ECSA_STATE_START,
	ECSA_STATE_UPDATE_FIRST_BCN_DONE,
	ECSA_STATE_COUNT_DOWN,
	ECSA_STATE_SWITCH,
	ECSA_STATE_DONE
};

struct phl_ecsa_ctrl_t{
	enum phl_ecsa_state state;
	struct rtw_phl_com_t *phl_com;
	struct rtw_wifi_role_t *role;
	struct rtw_wifi_role_link_t *rlink;
	struct rtw_phl_ecsa_param ecsa_param;
	struct rtw_phl_ecsa_ops ops;
	_os_timer timer;
	_os_lock lock;
	u32 expected_tbtt_ms;
	u32 req_hdl;
};

#ifdef CONFIG_PHL_ECSA
enum rtw_phl_status phl_ecsa_ctrl_init(struct phl_info_t *phl_info);
void phl_ecsa_ctrl_deinit(struct phl_info_t *phl_info);
#else
#define phl_ecsa_ctrl_init(_phl_info) RTW_PHL_STATUS_SUCCESS
#define phl_ecsa_ctrl_deinit(_phl_info)
#endif /* CONFIG_PHL_ECSA */

#endif /*_PHL_ECSA_H_*/
