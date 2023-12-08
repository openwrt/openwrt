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
#ifndef _PHL_TWT_H_
#define _PHL_TWT_H_

#ifdef CONFIG_PHL_TWT
#define MIN_TWT_ELE_LEN 15
#define MAX_NUM_HW_TWT_CONFIG 4
#define CONTROL_LENGTH 1
#define REQUEST_TYPE_LENGTH 2
#define NOMINAL_MINIMUM_TWT_WAKE_DURATION_LENGTH 1
#define TARGET_WAKE_TIME_LENGTH 8
#define NOMINAL_MIN_TWT_WAKE_DURATION_LENGTH 1
#define TWT_WAKE_INTERVAL_MANTISSA_LENGTH 2
#define TWT_CHANNEL_LENGTH 1
#define TWT_FLOW_FIELD_LENGTH 1
#define TOKEN_OFFSET 2
#define TOKEN_LENGTH 1
#define ELEM_ID_LEN 1 /* Length of element id*/
#define ELEM_LEN_LEN 1 /* Length of element Length*/
#define C2H_FUN_WAIT_ANNC 0x0

#define get_twt_info(_phl) (struct phl_twt_info *)(_phl->phl_twt_info)
#define set_config_state(_state, set_state) (_state = set_state)
#define twt_init(_phl) (NULL != (get_twt_info(_phl)))
#define twt_sup(_phl) (((struct phl_info_t *)_phl)->phl_com->dev_cap.twt_sup)

enum phl_operate_config_type {
	PHL_OP_TYPE_NONE = 0,
	PHL_GET_NEW_CONFIG,
	PHL_FREE_CONFIG,
	PHL_GET_CONFIG_BY_ID,
	PHL_GET_CONFIG_BY_ROLE,
	PHL_GET_CONFIG_BY_PARA,
	PHL_GET_NEXT_CONFIG,
	PHL_GET_HEAD_CONFIG
};

enum phl_twt_action {
	PHL_TWT_ACTION_NONE = 0,
	PHL_TWT_ACTION_FREE,
	PHL_TWT_ACTION_ALLOC,
	PHL_TWT_ACTION_ENABLE,
	PHL_TWT_ACTION_DISABLE,
	PHL_TWT_ACTION_UP_ERROR
};

enum twt_config_state {
	twt_config_state_free = 0,
	twt_config_state_idle,
	twt_config_state_enable,
	twt_config_state_error,
	twt_config_state_unknown
};

struct phl_twt_config{
	u8 idx;
	enum twt_config_state state;
	struct rtw_wifi_role_t *role;
	struct rtw_wifi_role_link_t *rlink;
	struct rtw_phl_twt_info twt_info;
	struct phl_queue twt_sta_queue; /*struct rtw_twt_sta_info*/
};

struct phl_twt_cfg_info {
	u8 twt_cfg_num;
	void *twt_cfg_ring; /*struct phl_twt_config*/
};

struct phl_twt_info {
	struct phl_twt_cfg_info twt_cfg_info;
	struct phl_queue twt_annc_queue;/*struct _twt_announce_info*/
};

struct _twt_announce_info{
	_os_list list;
	u8 map_offset;
	u32 wait_macid_map;
};

struct _twt_compare {
	struct rtw_phl_twt_setup_info twt_setup;
	struct rtw_wifi_role_t *role;
};

struct _twt_teardown {
	struct rtw_phl_stainfo_t *phl_sta;
	enum rtw_phl_nego_type nego_type;
	u8 id;
};


/* TWT element */
/*Control*/
#define SET_TWT_CONTROL_NDP_PAGING_INDICATOR(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 0, 1, _val)
#define SET_TWT_CONTROL_RESPONDER_PM_MODE(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 1, 1, _val)
#define SET_TWT_CONTROL_NEGOTIATION_TYPE(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 2, 2, _val)
#define SET_TWT_CONTROL_TWT_INFORMATION_FRAME_DISABLE(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 4, 1, _val)
#define SET_TWT_CONTROL_WAKE_DURATION_UNIT(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 5, 1, _val)

/*Individual TWT Parameter Set field*/
/*Request Type*/
#define SET_TWT_REQ_TYPE_TWT_REQUEST(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 0, 1, _val)
#define SET_TWT_REQ_TYPE_TWT_SETUP_COMMAND(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 1, 3, _val)
#define SET_TWT_REQ_TYPE_TRIGGER(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 4, 1, _val)
#define SET_TWT_REQ_TYPE_IMPLICIT(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 5, 1, _val)
#define SET_TWT_REQ_TYPE_FLOW_TYPE(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 6, 1, _val)
#define SET_TWT_REQ_TYPE_TWT_FLOW_IDENTIFER(_ele_start, _val) \
	SET_BITS_TO_LE_2BYTE(_ele_start, 7, 3, _val)
#define SET_TWT_REQ_TYPE_TWT_WAKE_INTERVAL_EXPONENT(_ele_start, _val) \
	SET_BITS_TO_LE_2BYTE(_ele_start, 10, 5, _val)
#define SET_TWT_REQ_TYPE_TWT_PROTECTION(_ele_start, _val) \
	SET_BITS_TO_LE_2BYTE(_ele_start, 15, 1, _val)

#define SET_TWT_TARGET_WAKE_TIME_L(_ele_start, _val) \
	SET_BITS_TO_LE_4BYTE((_ele_start) + 2, 0, 32, _val)

#define SET_TWT_TARGET_WAKE_TIME_H(_ele_start, _val) \
	SET_BITS_TO_LE_4BYTE((_ele_start) + 6, 0, 32, _val)
/*twt group assignment*/


#define SET_TWT_NOMINAL_MINIMUM_TWT_WAKE_DURATION(_ele_start, _offset, _val) \
	SET_BITS_TO_LE_1BYTE((_ele_start) + _offset, 0, 8, _val)
#define SET_TWT_TWT_WAKE_INTERVAL_MANTISSA(_ele_start, _offset, _val) \
	SET_BITS_TO_LE_2BYTE((_ele_start) + _offset, 0, 16, _val)
#define SET_TWT_TWT_CHANNEL(_ele_start, _offset, _val) \
	SET_BITS_TO_LE_1BYTE((_ele_start) + _offset, 0, 8, _val)


/*Broadcast TWT Parameter Set field*/


/*TWT FLOW field*/
#define SET_TWT_FLOW_ID(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 0, 3, _val)
#define SET_NEGOTIATION_TYPE(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 5, 2, _val)
#define SET_TEARDOWN_ALL_TWT(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 7, 1, _val)

#define SET_BROADCAST_TWT_ID(_ele_start, _val) \
	SET_BITS_TO_LE_1BYTE(_ele_start, 0, 5, _val)

/*Control*/
/*Bit0*/
#define GET_TWT_CONTROL_NDP_PAGING_INDICATOR(_buf) \
	LE_BITS_TO_1BYTE(_buf, 0, 1)
/*Bit1*/
#define GET_TWT_CONTROL_RESPONDER_PM_MODE(_buf) \
	LE_BITS_TO_1BYTE(_buf, 1, 1)
/*Bit2-3*/
#define GET_TWT_CONTROL_NEGOTIATION_TYPE(_buf) \
	LE_BITS_TO_1BYTE(_buf, 2, 2)
/*Bit4*/
#define GET_TWT_CONTROL_TWT_INFORMATION_FRAME_DISABLE(_buf) \
	LE_BITS_TO_1BYTE(_buf, 4, 1)
/*Bit5*/
#define GET_TWT_CONTROL_WAKE_DURATION_UNIT(_buf) \
	LE_BITS_TO_1BYTE(_buf, 5, 1)

/*Request Type*/
/*Bit0*/
#define GET_TWT_REQ_TYPE_TWT_REQUEST(_buf) \
	LE_BITS_TO_2BYTE(_buf, 0, 1)
/*Bit1-3*/
#define GET_TWT_REQ_TYPE_TWT_SETUP_COMMAND(_buf) \
	LE_BITS_TO_2BYTE(_buf, 1, 3)
/*Bit4*/
#define GET_TWT_REQ_TYPE_TRIGGER(_buf) \
	LE_BITS_TO_2BYTE(_buf, 4, 1)
/*Bit5*/
#define GET_TWT_REQ_TYPE_IMPLICIT(_buf) \
	LE_BITS_TO_2BYTE(_buf, 5, 1)
/*Bit6*/
#define GET_TWT_REQ_TYPE_FLOW_TYPE(_buf) \
	LE_BITS_TO_2BYTE(_buf, 6, 1)
/*Bit7-9*/
#define GET_TWT_REQ_TYPE_TWT_FLOW_IDENTIFER(_buf) \
	LE_BITS_TO_2BYTE(_buf, 7, 3)
/*Bit10-14*/
#define GET_TWT_REQ_TYPE_TWT_WAKE_INTERVAL_EXPONENT(_buf) \
	LE_BITS_TO_2BYTE(_buf, 10, 5)
/*Bit15*/
#define GET_TWT_REQ_TYPE_TWT_PROTECTION(_buf) \
	LE_BITS_TO_2BYTE(_buf, 15, 1)

#define GET_TWT_TARGET_WAKE_TIME_L(_buf) \
	LE_BITS_TO_4BYTE(_buf, 0, 32)
#define GET_TWT_TARGET_WAKE_TIME_H(_buf) \
	LE_BITS_TO_4BYTE((_buf) + 4, 0, 32)
#define GET_TWT_NOMINAL_MINIMUM_TWT_WAKE_DURATION(_buf) \
	LE_BITS_TO_1BYTE(_buf, 0 , 8);
#define GET_TWT_TWT_WAKE_INTERVAL_MANTISSA(_buf) \
	LE_BITS_TO_2BYTE(_buf, 0, 16);
#define GET_TWT_TWT_CHANNEL(_buf) \
	LE_BITS_TO_1BYTE(_buf, 0 ,8);

/*TWT FLOW field*/
/*Bit0-2*/
#define GET_TWT_FLOW_ID(_buf) \
	LE_BITS_TO_1BYTE(_buf, 0, 3)
/*Bit5-6*/
#define GET_NEGOTIATION_TYPE(_buf) \
	LE_BITS_TO_1BYTE(_buf, 5 , 2);
/*Bit7*/
#define GET_TEARDOWN_ALL_TWT(_buf) \
	LE_BITS_TO_1BYTE(_buf, 7, 1)
/*Bit0-4*/
#define GET_BROADCAST_TWT_ID(_buf) \
	LE_BITS_TO_1BYTE(_buf, 0, 5)


#define GET_ELE_ID(_buf) \
	LE_BITS_TO_1BYTE(_buf, 0, 8);
#define GET_ELE_LEN(_buf) \
	LE_BITS_TO_1BYTE(_buf, 0, 8);
#define GET_DIALOG_TOKEN(_buf) \
	LE_BITS_TO_1BYTE(_buf, 0, 8);


enum rtw_phl_status phl_twt_init(void *phl);

void phl_twt_deinit(void *phl);

enum rtw_phl_status
rtw_phl_twt_disable_all_twt_by_role(void *phl, struct rtw_wifi_role_t *role);

enum rtw_phl_status
rtw_phl_twt_alloc_twt_config(void *phl,
                             struct rtw_wifi_role_link_t *rlink,
                             struct rtw_phl_twt_setup_info setup_info,
                             u8 benable,
                             u8 *id);

enum rtw_phl_status rtw_phl_twt_free_twt_config(void *phl, u8 id);

enum rtw_phl_status rtw_phl_twt_add_sta_info(void *phl, struct rtw_phl_stainfo_t *phl_sta,
			u8 config_id, u8 id);

enum rtw_phl_status phl_twt_get_target_wake_time(struct phl_info_t *phl,
						u8 *param);

enum rtw_phl_status phl_twt_accept_for_sta_mode(struct phl_info_t *phl,
			u8 *param);

enum rtw_phl_status phl_twt_teardown_for_sta_mode(struct phl_info_t *phl,
				u8 *param);

enum rtw_phl_status rtw_phl_twt_handle_c2h_wait_annc(struct phl_info_t *phl,
				u8 *buf);
#else

#define phl_twt_init(_phl) RTW_PHL_STATUS_SUCCESS
#define phl_twt_deinit(_phl)

#endif /* CONFIG_PHL_TWT */
#endif /*_PHL_TWT_H_*/
