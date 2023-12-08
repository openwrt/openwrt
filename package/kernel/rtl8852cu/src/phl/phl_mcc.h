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
#ifndef _PHL_MCC_H_
#define _PHL_MCC_H_
/* MCC definition for private usage */

#define phl_to_com_mcc_info(_phl) ((struct phl_com_mcc_info *)(phl_to_mr_ctrl(_phl)->com_mcc))
#define get_mcc_info(_phl, _band) ((struct phl_mcc_info *)((get_band_ctrl(_phl, _band)->mcc_info)))
#define set_mcc_init_state(_phl, _state) (((struct mr_ctl_t *)phl_to_mr_ctrl(_phl))->init_mcc = _state)
#define is_mcc_init(_phl) (((struct mr_ctl_t *)phl_to_mr_ctrl(_phl))->init_mcc == true)
#define get_ref_role(_en_info) ((struct rtw_phl_mcc_role *)&(_en_info->mcc_role[_en_info->ref_role_idx]))

#define EARLY_TX_BCN_T 10
#define MIN_TX_BCN_T 10
#define EARLY_RX_BCN_T 5
#define MIN_RX_BCN_T 10
#define MIN_GO_STA_OFFSET_T 15
#define MIN_CLIENT_DUR (EARLY_RX_BCN_T + MIN_RX_BCN_T)
#define MIN_AP_DUR (EARLY_TX_BCN_T + MIN_GO_STA_OFFSET_T - EARLY_RX_BCN_T)
#define MIN_BCNS_OFFSET (EARLY_RX_BCN_T + MIN_RX_BCN_T)
#define MAX_MCC_GROUP_ROLE 2
#define DEFAULT_AP_DUR 60
#define DEFAULT_CLIENT_DUR 40
#define MCC_DUR_NONSPECIFIC 0xff
#define CLIENTS_WORSECASE_REF_TOA 30
#define CLIENTS_WORSECASE_SMALL_DUR 60
#define CLIENTS_WORSECASE_LARGE_DUR 90
#define WORSECASE_INTVL 150
#define LONG_TRIGGER_MCC_TIME 300/*TU*/
#define SHORT_TRIGGER_MCC_TIME 100/*TU*/
#define CLIENTS_TRACKING_TH 3
#define CLIENTS_TRACKING_WORSECASE_TH 3
#define CLIENTS_TRACKING_COURTESY_TH 3
#define CLIENTS_TRACKING_CRITICAL_POINT_TH 2
#define HANDLE_BCN_INTVL 100
#define BT_DUR_SEG_TH 20
#define AP_CLIENT_OFFSET 40
#define REF_ROLE_IDX 0
#define BT_DUR_MAX_2WS 33 /*The max bt slot for 2wifi slot and 1 bt slot*/

enum _mcc_minfo_reset_type {
	MINFO_RESET_EN_INFO = BIT(0),
	MINFO_RESET_MODE = BIT(1),
	MINFO_RESET_ROLE_MAP = BIT(2),
	MINFO_RESET_STATE = BIT(3),
	MINFO_RESET_COEX_MODE = BIT(4),
	MINFO_RESET_BT_INFO = BIT(5),
	MINFO_RESET_PATTERN_INFO = BIT(6),
	MINFO_RESET_ALL = 0xFF
};

enum _mcc_role_cat {
	MCC_ROLE_NONE = 0,
	MCC_ROLE_AP_CAT,
	MCC_ROLE_CLIENT_CAT
};

struct phl_mcc_fw_log_info {
	bool en_fw_mcc_log;
	u8 fw_mcc_log_lv;/* fw mcc log level */
	bool update; /*if update = true, we need to update setting to fw.*/
};

struct phl_mcc_info {
	struct rtw_phl_mcc_en_info en_info;
	enum rtw_phl_tdmra_wmode mcc_mode;
	u8 role_map; /*the wifi role map in operating mcc */
	enum rtw_phl_mcc_state state;
	enum rtw_phl_mcc_coex_mode coex_mode;
	struct rtw_phl_mcc_bt_info bt_info;
	struct phl_mcc_fw_log_info fw_log_i;
};



#endif /*_PHL_MCC_H_*/
