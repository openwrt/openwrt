/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
#ifndef __PHL_BTC_DEF_H__
#define __PHL_BTC_DEF_H__

#define RTW_PHL_BTC_CTRL_BUF 40
#define RTW_PHL_BTC_FWINFO_BUF 1800


/*****************************************
 * Please contact the member of BTC submodule if you
 * need to revise this section
 *****************************************/
#define RTW_BTINFO_MAXLEN 10
#define RTW_SCBD_MAXLEN 10

#define BTC_C2H_CAT 2 /* C2H Category for OutSrc */
#define BTC_CLASS_MIN 0x10
#define BTC_CLASS_FEV 0x12
#define BTC_CLASS_MAX 0x17

enum {
	BTC_FEV_REPORT = 0,
	BTC_FEV_BT_INFO = 1,
	BTC_FEV_BT_SCBD = 2,
	BTC_FEV_BT_REG = 3
};
/*****************************************/

enum {
	BTC_HMSG_TMR_EN = 0x0,
	BTC_HMSG_BT_REG_READBACK = 0x1,
	BTC_HMSG_SET_BT_REQ_SLOT = 0x2,
	BTC_HMSG_FW_EV = 0x3,
	BTC_HMSG_BT_LINK_CHG = 0x4,
	BTC_HMSG_SET_BT_REQ_STBC = 0x5,
	BTC_HMSG_MAX
};

enum {
	BTC_CTRL_TYPE_MANUAL,
	BTC_CTRL_TYPE_EN_REPORT,
	BTC_CTRL_TYPE_SET_SLOTS,
	BTC_CTRL_TYPE_SET_MREGS,
	BTC_CTRL_TYPE_SET_TDMA,
	BTC_CTRL_TYPE_SET_1SLOT,
	BTC_CTRL_TYPE_SET_POLICY,
	BTC_CTRL_TYPE_SET_GPIO_DBG,
	BTC_CTRL_TYPE_SEND_HUB_MSG,
	BTC_CTRL_TYPE_TEST,
	BTC_CTRL_TYPE_MAX
};

enum {
	BTC_TIMER_PERIODIC,
	BTC_TIMER_WL_SPECPKT,
	BTC_TIMER_WL_RFKTO,
	BTC_TIMER_BT_A2DPPLAY,
	BTC_TIMER_MAX
};

enum RTW_PHL_BTC_CTRL_TYPE {
	PHL_BTC_CTRL_MANUAL,
	PHL_BTC_CTRL_MAX
};

enum {
	PHL_BTC_CNTFY_BTINFO,
	PHL_BTC_CNTFY_MAX
};

enum PHL_BTC_NTFY_REASON {
	PHL_BTC_NTFY_RSN_PERIOTIC
};

enum RTW_PHL_BTC_NOTIFY {
	PHL_BTC_NTFY_SCAN_START,
	PHL_BTC_NTFY_SCAN_STOP,
	PHL_BTC_NTFY_COEX_INFO,
	PHL_BTC_NTFY_ROLE_INFO,
	PHL_BTC_NTFY_CTRL,
	PHL_BTC_NTFY_FWINFO,
	PHL_BTC_NTFY_RADIO_STATE,
	PHL_BTC_NTFY_WLSTA,
	PHL_BTC_NTFY_PACKET_EVT,
	PHL_BTC_NTFY_TIMER,
	PHL_BTC_NTFY_MAX
};

enum {
	BTC_MODE_NORMAL,
	BTC_MODE_WL,
	BTC_MODE_BT,
	BTC_MODE_WLOFF,
	BTC_MODE_COTX,
	BTC_MODE_MAX
};

enum {
	BTC_RFCTRL_WL_OFF,
	BTC_RFCTRL_WL_ON,
	BTC_RFCTRL_LPS_WL_ON,
	BTC_RFCTRL_FW_CTRL,
	BTC_RFCTRL_MAX
};

struct rtw_phl_btc_ops {
	void (*print)(const char *msg);
};

struct rtw_phl_btc_coex_info_param {
	u8 query_type;
};

struct rtw_phl_btc_role_info_param {
	u8 role_id;
	enum role_state rstate;
};

struct rtw_phl_btc_wl_sta_param {
	u8 role_id;
	u16 mac_id;
	u8 reason;
};

struct rtw_phl_btc_pkt_param {
	u8 role_id;
	u8 pkt_evt_type;
};

struct rtw_phl_btc_ctrl_param {
	u8 type;
	u16 len;
	u8 buf[RTW_PHL_BTC_CTRL_BUF];
};

struct rtw_phl_btc_fwinfo_param {
	u8 c2h_class;
	u8 c2h_func;
};

struct rtw_phl_btc_radio_state_param {
	u8 rf_on;
};

struct rtw_phl_btc_tmr_param {
	void *timer;
};

struct rtw_phl_btc_ntfy {
	enum RTW_PHL_BTC_NOTIFY notify;
	struct rtw_phl_btc_ops *ops;

	/* real parameter for different notifications */
	union {
		struct rtw_phl_btc_coex_info_param cinfo;
		struct rtw_phl_btc_role_info_param rinfo;
		struct rtw_phl_btc_ctrl_param ctrl;
		struct rtw_phl_btc_radio_state_param rfst;
		struct rtw_phl_btc_fwinfo_param finfo;
		struct rtw_phl_btc_wl_sta_param wsta;
		struct rtw_phl_btc_pkt_param pkt;
		struct rtw_phl_btc_tmr_param tmr;
	} u;

	void *priv;
	int (*ntfy_cb)(void *priv, enum RTW_PHL_BTC_NOTIFY ntfy,
		struct rtw_phl_btc_ntfy *info);
};

#endif /* __PHL_BTC_DEF_H__ */
