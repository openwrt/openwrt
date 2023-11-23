/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#ifndef __RTW_SCAN_H_
#define __RTW_SCAN_H_

/*rtw_mlme.h*/
void rtw_drv_scan_by_self(_adapter *padapter, u8 reason);
void rtw_scan_abort_no_wait(_adapter *adapter);
u32 rtw_scan_abort(_adapter *adapter, u32 timeout_ms);

void rtw_survey_event_callback(_adapter *adapter, u8 *pbuf);
void rtw_surveydone_event_callback(_adapter *adapter, u8 *pbuf);

enum {
	SS_DENY_MP_MODE,
	SS_DENY_RSON_SCANING,
	SS_DENY_BLOCK_SCAN,
	SS_DENY_BY_DRV,
	SS_DENY_SELF_AP_UNDER_WPS,
	SS_DENY_SELF_AP_UNDER_LINKING,
	SS_DENY_SELF_AP_UNDER_SURVEY,
	/*SS_DENY_SELF_STA_UNDER_WPS,*/
	SS_DENY_SELF_STA_UNDER_LINKING,
	SS_DENY_SELF_STA_UNDER_SURVEY,
	SS_DENY_BUDDY_UNDER_LINK_WPS,
	SS_DENY_BUDDY_UNDER_SURVEY,
	SS_DENY_BUSY_TRAFFIC,
	SS_ALLOW,
#ifdef DBG_LA_MODE
	SS_DENY_LA_MODE,
#endif
	SS_DENY_ADAPTIVITY,
};
u8 _rtw_sitesurvey_condition_check(const char *caller, _adapter *adapter, bool check_sc_interval);
#define rtw_sitesurvey_condition_check(adapter, check_sc_interval) _rtw_sitesurvey_condition_check(__func__, adapter, check_sc_interval)

#ifdef CONFIG_SET_SCAN_DENY_TIMER
bool rtw_is_scan_deny(_adapter *adapter);
void rtw_clear_scan_deny(_adapter *adapter);
void rtw_set_scan_deny_timer_hdl(void *ctx);
void rtw_set_scan_deny(_adapter *adapter, u32 ms);
#else
#define rtw_is_scan_deny(adapter) _FALSE
#define rtw_clear_scan_deny(adapter) do {} while (0)
#define rtw_set_scan_deny(adapter, ms) do {} while (0)
#endif

#ifdef CONFIG_RTW_ACS
u8 rtw_set_acs_sitesurvey(_adapter *adapter);
#endif

/*rtw_mlme_ext.h*/
void sitesurvey_set_offch_state(_adapter *adapter, u8 scan_state);

/*const char *scan_state_str(u8 state);*/
#define mlmeext_scan_state(mlmeext) ((mlmeext)->sitesurvey_res.state)
/*#define mlmeext_scan_state_str(mlmeext) scan_state_str((mlmeext)->sitesurvey_res.state)*/
#define mlmeext_chk_scan_state(mlmeext, _state) ((mlmeext)->sitesurvey_res.state == (_state))
#define mlmeext_set_scan_state(mlmeext, _state) \
	do { \
		((mlmeext)->sitesurvey_res.state = (_state)); \
		((mlmeext)->sitesurvey_res.next_state = (_state)); \
		rtw_mi_update_iface_status(&((container_of(mlmeext, _adapter, mlmeextpriv)->mlmepriv)), 0); \
		/* RTW_INFO("set_scan_state:%s\n", scan_state_str(_state)); */ \
		sitesurvey_set_offch_state(container_of(mlmeext, _adapter, mlmeextpriv), _state); \
	} while (0)

#if 0
#define mlmeext_scan_next_state(mlmeext) ((mlmeext)->sitesurvey_res.next_state)
#define mlmeext_set_scan_next_state(mlmeext, _state) \
	do { \
		((mlmeext)->sitesurvey_res.next_state = (_state)); \
		/* RTW_INFO("set_scan_next_state:%s\n", scan_state_str(_state)); */ \
	} while (0)
#endif
#ifdef CONFIG_SCAN_BACKOP
#define mlmeext_scan_backop_flags(mlmeext) ((mlmeext)->sitesurvey_res.backop_flags)
#define mlmeext_chk_scan_backop_flags(mlmeext, flags) ((mlmeext)->sitesurvey_res.backop_flags & (flags))
#define mlmeext_assign_scan_backop_flags(mlmeext, flags) \
	do { \
		((mlmeext)->sitesurvey_res.backop_flags = (flags)); \
		RTW_INFO("assign_scan_backop_flags:0x%02x\n", (mlmeext)->sitesurvey_res.backop_flags); \
	} while (0)

#define mlmeext_scan_backop_flags_sta(mlmeext) ((mlmeext)->sitesurvey_res.backop_flags_sta)
#define mlmeext_chk_scan_backop_flags_sta(mlmeext, flags) ((mlmeext)->sitesurvey_res.backop_flags_sta & (flags))
#define mlmeext_assign_scan_backop_flags_sta(mlmeext, flags) \
	do { \
		((mlmeext)->sitesurvey_res.backop_flags_sta = (flags)); \
	} while (0)
#else
#define mlmeext_scan_backop_flags(mlmeext) (0)
#define mlmeext_chk_scan_backop_flags(mlmeext, flags) (0)
#define mlmeext_assign_scan_backop_flags(mlmeext, flags) do {} while (0)

#define mlmeext_scan_backop_flags_sta(mlmeext) (0)
#define mlmeext_chk_scan_backop_flags_sta(mlmeext, flags) (0)
#define mlmeext_assign_scan_backop_flags_sta(mlmeext, flags) do {} while (0)
#endif /* CONFIG_SCAN_BACKOP */

#if defined(CONFIG_SCAN_BACKOP) && defined(CONFIG_AP_MODE)
#define mlmeext_scan_backop_flags_ap(mlmeext) ((mlmeext)->sitesurvey_res.backop_flags_ap)
#define mlmeext_chk_scan_backop_flags_ap(mlmeext, flags) ((mlmeext)->sitesurvey_res.backop_flags_ap & (flags))
#define mlmeext_assign_scan_backop_flags_ap(mlmeext, flags) \
	do { \
		((mlmeext)->sitesurvey_res.backop_flags_ap = (flags)); \
	} while (0)
#else
#define mlmeext_scan_backop_flags_ap(mlmeext) (0)
#define mlmeext_chk_scan_backop_flags_ap(mlmeext, flags) (0)
#define mlmeext_assign_scan_backop_flags_ap(mlmeext, flags) do {} while (0)
#endif /* defined(CONFIG_SCAN_BACKOP) && defined(CONFIG_AP_MODE) */

#if defined(CONFIG_SCAN_BACKOP) && defined(CONFIG_RTW_MESH)
#define mlmeext_scan_backop_flags_mesh(mlmeext) ((mlmeext)->sitesurvey_res.backop_flags_mesh)
#define mlmeext_chk_scan_backop_flags_mesh(mlmeext, flags) ((mlmeext)->sitesurvey_res.backop_flags_mesh & (flags))
#define mlmeext_assign_scan_backop_flags_mesh(mlmeext, flags) \
	do { \
		((mlmeext)->sitesurvey_res.backop_flags_mesh = (flags)); \
	} while (0)
#else
#define mlmeext_scan_backop_flags_mesh(mlmeext) (0)
#define mlmeext_chk_scan_backop_flags_mesh(mlmeext, flags) (0)
#define mlmeext_assign_scan_backop_flags_mesh(mlmeext, flags) do {} while (0)
#endif /* defined(CONFIG_SCAN_BACKOP) && defined(CONFIG_RTW_MESH) */

#if 0
void survey_timer_hdl(void *ctx);
#define set_survey_timer(mlmeext, ms) \
	do { \
		/*RTW_INFO("%s set_survey_timer(%p, %d)\n", __FUNCTION__, (mlmeext), (ms));*/ \
		_set_timer(&(mlmeext)->survey_timer, (ms)); \
	} while (0)
#define cancel_survey_timer(mlmeext) \
	do { \
		/*RTW_INFO("%s cancel_survey_timer(%p)\n", __FUNCTION__, (mlmeext));*/ \
		_cancel_timer_ex(&(mlmeext)->survey_timer); \
	} while (0)
#endif

enum SCAN_STATE {
	SCAN_DISABLE = 0,
	SCAN_START = 1,
	SCAN_PS_ANNC_WAIT = 2,
	SCAN_ENTER = 3,
	SCAN_PROCESS = 4,

	/* backop */
	SCAN_BACKING_OP = 5,
	SCAN_BACK_OP = 6,
	SCAN_LEAVING_OP = 7,
	SCAN_LEAVE_OP = 8,

	/* SW antenna diversity (before linked) */
	SCAN_SW_ANTDIV_BL = 9,

	/* legacy p2p */
	SCAN_TO_P2P_LISTEN = 10,
	SCAN_P2P_LISTEN = 11,

	SCAN_COMPLETE = 12,
	SCAN_STATE_MAX,
};


enum ss_backop_flag {
	SS_BACKOP_EN = BIT0, /* backop when linked */
	SS_BACKOP_EN_NL = BIT1, /* backop even when no linked */

	SS_BACKOP_PS_ANNC = BIT4,
	SS_BACKOP_TX_RESUME = BIT5,
};

#define RTW_SSID_SCAN_AMOUNT 9 /* for WEXT_CSCAN_AMOUNT 9 */
#define RTW_CHANNEL_SCAN_AMOUNT MAX_CHANNEL_NUM

struct ss_res {
	u8 state;
	u8 next_state; /* will set to state on next cmd hdl */
	int bss_cnt;
	u8 activate_ch_cnt;
	#ifdef CONFIG_CMD_SCAN
	struct rtw_phl_scan_param *scan_param;
	#endif
	struct submit_ctx sctx;

	u16 scan_ch_ms;

	u8 rx_ampdu_accept;
	u8 rx_ampdu_size;
#if 0
	int channel_idx;
	u8 force_ssid_scan;
	int scan_mode;
	u8 igi_scan;
	u8 igi_before_scan; /* used for restoring IGI value without enable DIG & FA_CNT */
#endif

#ifdef CONFIG_SCAN_BACKOP
	u8 backop_flags_sta; /* policy for station mode*/
	#ifdef CONFIG_AP_MODE
	u8 backop_flags_ap; /* policy for ap mode */
	#endif
	#ifdef CONFIG_RTW_MESH
	u8 backop_flags_mesh; /* policy for mesh mode */
	#endif
	u8 backop_flags; /* per backop runtime decision */
	#if 0
	u8 scan_cnt;
	#endif
	u8 scan_cnt_max;
	systime backop_time; /* the start time of backop */
	u16 backop_ms;
#endif
#if defined(CONFIG_ANTENNA_DIVERSITY) || defined(DBG_SCAN_SW_ANTDIV_BL)
	u8 is_sw_antdiv_bl_scan;
#endif
	u8 ssid_num;
	u8 ch_num;
	NDIS_802_11_SSID ssid[RTW_SSID_SCAN_AMOUNT];
	struct rtw_ieee80211_channel ch[RTW_CHANNEL_SCAN_AMOUNT];

	u32 token; 	/* 0: use to identify caller */
	u16 duration;	/* 0: use default */
	u8 igi;		/* 0: use defalut */
	u8 bw;		/* 0: use default */

	bool acs; /* aim to trigger channel selection when scan done */
};

enum rtw_scan_type {
	RTW_SCAN_NORMAL,
	RTW_SCAN_P2P,
	RTW_SCAN_RRM
};

struct sitesurvey_parm {
	enum rtw_phl_scan_type scan_mode;	/* active: 1, passive: 0 */
	/* sint bsslimit;	// 1 ~ 48 */
	u8 ssid_num;
	u8 ch_num;
	NDIS_802_11_SSID ssid[RTW_SSID_SCAN_AMOUNT];
	struct rtw_ieee80211_channel ch[RTW_CHANNEL_SCAN_AMOUNT];

	u32 rrm_token;	/* 80211k use it to identify caller */
	u16 duration;	/* 0: use default, otherwise: channel scan time */
	u8 bw;		/* 0: use default */

	bool acs; /* aim to trigger channel selection when scan done */

	enum rtw_scan_type scan_type;
};

void rtw_init_sitesurvey_parm(_adapter *padapter, struct sitesurvey_parm *pparm);
u8 rtw_sitesurvey_cmd(_adapter *padapter, struct sitesurvey_parm *pparm);
#ifndef CONFIG_CMD_SCAN
u32 rtw_site_survey_fsm(_adapter *padapter, struct cmd_obj *pcmd);
#endif

#ifdef CONFIG_FSM
u8 sitesurvey_cmd_hdl(_adapter *padapter, u8 *pbuf);
void rtw_survey_cmd_callback(_adapter  *padapter, struct cmd_obj *pcmd);
#endif

#ifdef CONFIG_IOCTL_CFG80211
u8 rtw_phl_remain_on_ch_cmd(_adapter *padapter, u64 cookie, struct wireless_dev *wdev,
	struct ieee80211_channel *ch, u8 ch_type, unsigned int duration,
	struct back_op_param *bkop_parm, u8 is_p2p);
#endif

#endif /* __RTW_SCAN_H_ */
