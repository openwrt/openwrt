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
#ifndef __PHL_SCAN_H__
#define __PHL_SCAN_H__

/* Header file for application to invoke scan service */
#define PHL_SSID_LEN 32

#ifdef CONFIG_PHL_CMD_SCAN
enum _CMD_SCAN_STATE {
	CMD_SCAN_INIT = 0,
	CMD_SCAN_ACQUIRE = BIT0,
	CMD_SCAN_STARTED = BIT1,
	CMD_SCAN_DF_IO = BIT2, /* Disable Function : IO */
	CMD_SCAN_END = BIT3, /* End/ Cancel (Abort isn't included) */
};
/**   Scan Flag Format (1Byte):
 *    7                    4                    0
 *    +--------------------+--------------------+
 *    | Flags of HW_BAND_1 | Flags of HW_BAND_0 |
 *    +--------------------+--------------------+
**/

#define SET_SCAN_FLAG(_status, _band_idx, _flags) \
	((_status) |= ((_flags) << (_band_idx*4)))
#define TEST_SCAN_FLAG(_status, _band_idx, _flags)\
	(((_status) & ((_flags) << (_band_idx*4)))==((_flags) << (_band_idx*4)))
#define TEST_SCAN_FLAGS(_status, _flags)\
	((((_status) & (_flags))==(_flags)) || \
	(((_status) & ((_flags) << 4))==((_flags) << 4)))
#endif

struct rtw_phl_ssid {
	u32 ssid_len;
	u8 ssid[PHL_SSID_LEN];
};

enum phl_scan_mode {
	NULL_MODE,  /* End of Scanning */
	NORMAL_SCAN_MODE, /* OFF CHANNEL : non-operation channel*/
	BACKOP_MODE,
	P2P_SCAN_MODE,
	P2P_LISTEN_MODE,
	MAX_MODE,
};

/* Scan type including active and passive scan. */
enum rtw_phl_scan_type {
	RTW_PHL_SCAN_PASSIVE,
	RTW_PHL_SCAN_ACTIVE,
	RTW_PHL_SCAN_MIX,
};

enum phl_ext_act_scan_state {
	EXT_ACT_SCAN_DISABLE,
	EXT_ACT_SCAN_ENABLE,
	EXT_ACT_SCAN_TRIGGER,
	EXT_ACT_SCAN_DONE,
};

enum scan_bkop_sel {
	SCAN_BKOP_SEL_AUTO,
	SCAN_BKOP_SEL_MANUAL
};

enum scan_bkop_mode {
	SCAN_BKOP_NONE,
	SCAN_BKOP_CNT,
	SCAN_BKOP_TIMER
};

struct phl_scan_channel {
	_os_list list;

	enum band_type band;
	u16 channel; /* channel number */
	u16 duration; /* 0: use default, otherwise: channel scan time */
	enum channel_width bw; /* 0: use default 20Mhz */
	enum chan_offset offset;
	enum rtw_phl_scan_type type; /* active scan: 1, passive scan: 0 */
	u8 scan_mode; /* according to phl_scan_mode */
	u8 ext_act_scan; /* according to phl_ext_act_scan_state */
	u32 start_t; /* start time */
	u32 remain_t;
	u32 pass_t;
	u32 last_t;
#ifdef CONFIG_RTW_ACS
	u8 acs_idx; /* idx of phl_acs_chnl_tbl */
#endif
};

struct scan_bkop_link_info {
	struct rtw_wifi_role_link_t *rlink;
	/* every 'back_op.off_ch_dur_ms' go back to op ch
	 * back_op.off_ch_dur_ms have to large than ch->duration
	 * 0 : not specify
	*/
	u16 ch_dur_ms; /* op ch stay time; 0 : use default value */
	u16 off_ch_dur_ms;
	u16 off_ch_ext_dur_ms; /* extend when off_ch_tx (MGNT_TX) */
};

struct scan_backop_para {
	/* back op parameters */
	enum scan_bkop_sel sel; /* op channel selecton */
	/* core decided rlink first */
	struct scan_bkop_link_info link_info_list[MAX_WIFI_ROLE_NUMBER];
	u8 link_info_num;
	enum scan_bkop_mode mode;
	u8 ch_intv; /* every ch_intv go back to op ch */

	/* every 'back_op.off_ch_dur_ms' go back to op ch
	 * back_op.off_ch_dur_ms have to large than ch->duration
	 * 0 : not specify
	*/
	u16 ch_dur_ms; /* op ch stay time; 0 : use default value */
	u16 off_ch_dur_ms;
	u16 off_ch_ext_dur_ms; /* extend when off_ch_tx (MGNT_TX) */
};

struct cmd_scan_ctrl {

	u8 fltr_mode; /* backup filter mode before off channel */
	u8 repeat; /* init value = param->repeat */
	bool is_cckphy;
	u32 token;

	struct rtw_wifi_role_t *wrole; /* scan_timer would use wrole->hw_band*/
	_os_timer scan_timer; /* init in phl_cmd_scan.acquired */

	struct phl_scan_channel back_op_ch[MAX_WIFI_ROLE_NUMBER];
	struct phl_queue chlist;
	struct phl_scan_channel *scan_ch;
	int ch_idx; /* current scaned cahnnel index, init vaule = (-1) */
	u32 last_opch_add_time; /* used for timer */
};

enum scan_result {
	SCAN_REQ_ABORT, /* abort a non-started(queued) scan */
	SCAN_REQ_CANCEL, /* cancel a started scan */
	SCAN_REQ_COMPLETE /* scan complete */
};

enum scan_cmd_opt {
	SCAN_CMD_OPT_DB = BIT0,
};

#ifdef CONFIG_PHL_SCANOFLD
enum scan_mode {
	SCAN_MD_DRV,
	SCAN_MD_FW
};
#endif

#define SCAN_SSID_AMOUNT 9 /* for WEXT_CSCAN_AMOUNT 9 */
#define SCAN_CH_AMOUNT (14+37)
struct rtw_phl_scan_param {
	u32 max_scan_time;
	#ifdef CONFIG_PHL_CMD_SCAN_BKOP_TIME
	u32 max_listen_time;
	#endif
	u32 ext_act_scan_period;
	struct rtw_phl_ssid ssid[SCAN_SSID_AMOUNT];
	u8 ssid_num;

	/* create for computing scan time */
	u32 enqueue_time;
	u32 start_time;
	u32 end_time;
	u32 total_scan_time;

	struct phl_scan_channel *ch;
	u8 ch_num;
	u32 ch_sz;
	u8 repeat;  /* 255 means scan forever until cancel */

	struct scan_backop_para back_op;

	/* core layer handler */
	struct rtw_phl_scan_ops *ops;
	void *priv; /* ops private */

	struct rtw_wifi_role_t *wifi_role;
	u32 probe_t; /* probe timer */

#ifdef CONFIG_PHL_CMD_SCAN
	struct cmd_scan_ctrl sctrl[MAX_BAND_NUM];
	u8 sctrl_num;
	u8 opt;
	u8 state;
#endif

	//u32 token; /* unique id, generated by rtw_phl_cmd_scan_request() */
	const char *name;
	enum scan_result result;

#ifdef CONFIG_PHL_SCANOFLD
	/* scan offlod */
	enum scan_mode mode;
	u8 chkpt_time;
#endif

#ifdef CONFIG_RTW_ACS
	bool acs;
	bool nhm_include_cca;
#endif
};

struct rtw_phl_scan_ops {
	int (*scan_issue_pbreq)(void *priv, struct rtw_phl_scan_param *param, u8 sctrl_idx);
	u8 (*scan_issue_null_data)(void *priv, u8 ridx, bool ps);
	int (*scan_start)(void *priv, struct rtw_phl_scan_param *param);
	int (*scan_ch_ready)(void *priv, struct rtw_phl_scan_param *param, u8 sctrl_idx);
	bool (*scan_ch_cancel)(void *priv, struct rtw_phl_scan_param *param, u8 sctrl_idx);
	int (*scan_probe)(void *priv, struct rtw_phl_scan_param *param, u8 sctrl_idx);
	int (*scan_off_ch_tx)(void *priv,
		struct rtw_phl_scan_param *param, void *data);
	int (*scan_complete)(void *priv, struct rtw_phl_scan_param *param);
};

/* CMD_DISP SCAN*/
enum rtw_phl_status rtw_phl_cmd_scan_request(void *phl,
	struct rtw_phl_scan_param *param);
enum rtw_phl_status rtw_phl_cmd_scan_cancel(void *phl,
				struct rtw_phl_scan_param *param);
int rtw_phl_cmd_scan_inprogress(void *phl, u8 band_idx);

u8 phl_cmd_chk_ext_act_scan(struct rtw_phl_scan_param *param, u8 sctrl_idx);
u8 phl_cmd_scan_ctrl(struct rtw_phl_scan_param *param, u8 band_idx,
		     struct cmd_scan_ctrl **sctrl);
#endif /* __PHL_SCAN_H__ */

