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
#ifndef __PHL_SOUND_H__
#define __PHL_SOUND_H__

#ifdef CONFIG_PHL_CMD_BF

#define MAX_SND_GRP_NUM 4
#define MAX_SND_HE_BFRP_NUM 2
#define MAX_SND_HE_BFRP_USER_NUM 4
#define MAX_SND_VHT_BFRP_NUM 3

#define SND_PROC_DEFAULT_PERIOD 200 /* ms */
#define SND_PROC_DEFAULT_TIMEOUT 10 /* ms */

/* MAX_NUM_STA_SND_GRP :phl sounding design limit = 4, hw/fw support maximum 8 STA in a sounding frame exchange*/
#define MAX_NUM_STA_SND_GRP 4

enum snd_type {
	PHL_SND_TYPE_INVALID,
	PHL_SND_TYPE_VHT_HW,
	PHL_SND_TYPE_VHT_SW,
	PHL_SND_TYPE_HE_HW,
	PHL_SND_TYPE_HE_SW
};

enum snd_fb_type {
	PHL_SND_FB_TYPE_SU,
	PHL_SND_FB_TYPE_MU,
	PHL_SND_FB_TYPE_CQI
};

enum snd_status {
	PHL_SND_STS_PENDING,
	PHL_SND_STS_ON_PROC,
	PHL_SND_STS_SUCCESS,
	PHL_SND_STS_FAILURE
};

enum snd_grp_tier {
	PHL_SND_GRP_TIER_0, /* TIER_0 Group will free sounding resource in state SND_PROC_TERMINATE or group removed*/
	PHL_SND_GRP_TIER_1  /* TIER_1 Group will free sounding resource in next SND_PROC_IDLE*/
};

enum snd_test_flag {
	PHL_SND_TEST_F_NONE = 0, /* default value */
	PHL_SND_TEST_F_ONE_TIME = BIT(0), /* Test mode : only sounding one time */
	PHL_SND_TEST_F_FIX_STA = BIT(1), /* forced SND STAs (skip grouping and check TP) */
	PHL_SND_TEST_F_GRP_SND_PARA = BIT(2), /* Fixed SND STA's Feedback Type or BW from snd->fix_param */
	PHL_SND_TEST_F_GRP_EN_BF_FIX = BIT(3), /* Enable grp->en_fix_mode in grouping function */
	PHL_SND_TEST_F_PASS_STS_CHK = BIT(4) /* by pass sounding status check when post config */
};

struct npda_dialog_token {
	u8 reserved:1;
	u8 he:1;
	u8 token:6;
};

struct vht_ndpa_sta_info {
	u16 aid12:12;
#define VHT_NDPA_FB_TYPE_SU 0
#define VHT_NDPA_FB_TYPE_MU 1
	u16 feedback_type:1;
	u16 nc:3;
};

struct he_ndpa_sta_info {
	u32 aid:11;
	u32 bw:14;
	u32 fb_ng:2;
	u32 disambiguation:1;
	u32 cb:1;
	u32 nc:3;
};

struct phl_snd_sta {
	u8 valid;
	u16 macid;
	enum channel_width bw; /* Sounding BW */
	enum snd_fb_type snd_fb_t; /* Sounding feedback type : SU/MU/CQI */
	u32 npda_sta_info; /* VHT/HE NDPA STA info*/
	enum snd_status snd_sts;
	/* Query resource in SND PROC */
	void *bf_entry;/* HAL BF Entry for sounding */
};


struct phl_snd_grp {
	u8 gidx;
	enum snd_type snd_type;
	enum snd_grp_tier grp_tier;
	u8 wrole_idx;
	u8 band;
	u8 num_sta;
	struct phl_snd_sta sta[MAX_NUM_STA_SND_GRP];
	enum snd_status snd_sts;
	u8 en_fix_mode; /* post confg forced mode setting */
	/**
	 * en_swap_mode : Only TIER_0 MU group support. When swap mode is enable,
	 * each MU Entry will use two CSI MU buffer. Therefore, it cannan skip Disable
	 * MU in Preconfig when next round sounding
	 **/
	u8 en_swap_mode;
	u8 skip_post_cfg;/* 1: SKIP ALL; BIT1:Skip Group, BIT2:Skip GID, BIT3:Skip STA */

	struct snd_cmd_bfer snd_cmd;

};


/* for whole phl snd fsm module fixed mode, only worked if snd_param->test_flag != 0 */
struct phl_snd_fix_param {
	u8 en_fix_gidx;
	u8 en_fix_fb_type;
	u8 en_fix_sta;
	u8 en_fix_snd_bw;
	u8 grp_idx;
	enum snd_fb_type snd_fb_type;
	u16 sta_macid[MAX_NUM_STA_SND_GRP];
	enum channel_width bw[MAX_NUM_STA_SND_GRP];
	u8 f_ru_tbl_20[MAX_SND_HE_BFRP_USER_NUM][MAX_SND_HE_BFRP_USER_NUM];
	u8 f_ru_tbl_80[MAX_SND_HE_BFRP_USER_NUM][MAX_SND_HE_BFRP_USER_NUM];
};

struct phl_sound_param {
	void *m_wrole;
	struct phl_snd_grp snd_grp[MAX_SND_GRP_NUM];
	u32 grp_used_map;
	u8 cur_proc_grp_idx;
	u8 pre_proc_grp_idx;
	u8 snd_func_grp_num;
	u8 snd_dialog_token;
	u8 snd_proc_timeout_ms;
	u32 proc_start_time;
	u8 snd_proc_period;
	bool bypass_snd_sts_chk;
	u32 test_flag;
	struct phl_snd_fix_param fix_param;
	u8 snd_fail_counter;
};
struct phl_snd_ops
{
	enum rtw_phl_status (*snd_send_ndpa)(void *drv_priv,
                                             struct rtw_wifi_role_link_t *rlink,
                                             u8 *snd_dialog_tkn,
                                             u32 *ndpa_sta,
                                             enum channel_width snd_bw);
};

struct phl_sound_obj {
	struct phl_sound_param snd_param;
	struct phl_snd_ops ops;

	struct phl_info_t *phl_info;
	void *iface;

	u8 snd_in_progress;
	u8 is_terminated;

	_os_lock snd_lock;
	/* snd test */
	u8 wrole_idx;

	/* snd cmd disp related */
	u8 msg_busy;

	_os_lock cmd_lock;
	_os_timer snd_timer;
	u32 msg_hdl;

};


/* phl sounding intern api*/
enum rtw_phl_status phl_snd_func_snd_init(struct phl_info_t *phl_info);

enum rtw_phl_status phl_snd_func_pre_config(struct phl_info_t *phl_info);

enum rtw_phl_status phl_snd_init_snd_grp(struct phl_info_t *phl_info);

/* phl sounding extern api*/
enum rtw_phl_status
rtw_phl_sound_start(void *phl, u8 wrole_idx, u8 st_dlg_tkn, u8 period, u8 test_flag);

enum rtw_phl_status
rtw_phl_sound_abort(void *phl);

enum rtw_phl_status
rtw_phl_sound_down_ev(void *phl);

void rtw_phl_snd_fix_tx_he_mu(struct phl_info_t *phl_info, u8 gid, bool en);

/* snd func grp */
struct phl_snd_grp *
phl_snd_get_grp_byidx(struct phl_info_t *phl_info, u8 gidx);

void
phl_snd_func_remove_grp_all(struct phl_info_t *phl_info);

enum rtw_phl_status
phl_snd_func_grouping(struct phl_info_t *phl_info, u8 wroleidx);

/* snd proc resource */
enum rtw_phl_status
phl_snd_proc_get_res(
	struct phl_info_t *phl_info, struct phl_snd_grp *grp, u8 *nsta);

enum rtw_phl_status
phl_snd_proc_release_res(struct phl_info_t *phl_info, struct phl_snd_grp *grp);

/* snd proc precfg */
enum rtw_phl_status
phl_snd_proc_precfg(struct phl_info_t *phl_info, struct phl_snd_grp *grp);


/* snd proc busy cmd to fw */
enum rtw_phl_status
phl_snd_proc_start_sounding_fw(struct phl_info_t *phl_info,
			       struct phl_snd_grp *grp);

/* snd proc postcfg */
enum rtw_phl_status
phl_snd_proc_postcfg(struct phl_info_t *phl_info, struct phl_snd_grp *grp);

enum rtw_phl_status
phl_snd_proc_chk_condition(struct phl_info_t *phl_info, struct phl_snd_grp *grp);

void
phl_snd_proc_chk_prev_grp(struct phl_info_t *phl_info,
			  struct phl_snd_grp *grp);

enum rtw_phl_status
phl_snd_polling_pri_sta_sts(struct phl_info_t *phl_info,
			    struct phl_snd_grp *grp);
void
phl_snd_mac_ctrl(struct phl_info_t *phl_info,
		 enum phl_band_idx band, u8 ctrl);

enum rtw_phl_status
rtw_phl_snd_add_grp(void *phl,
                    struct rtw_wifi_role_link_t *rlink,
                    u8 gidx,
                    u16 *macid,
                    u8 num_sta,
                    bool he,
                    bool mu);

enum rtw_phl_status
rtw_phl_sound_start_ex(void *phl, u8 wrole_idx, u8 st_dlg_tkn, u8 period, u8 test_flag);


#else
#define phl_snd_func_snd_init(_phl_info) RTW_PHL_STATUS_SUCCESS
#define phl_snd_func_pre_config(_phl_info) RTW_PHL_STATUS_SUCCESS
#define phl_snd_init_snd_grp(_phl_info) RTW_PHL_STATUS_SUCCESS
#define phl_snd_get_grp_byidx(_phl_info, _gidx) NULL
#define phl_snd_func_remove_grp_all(_phl_info)
#define phl_snd_func_grouping(_phl_info, _wroleidx) RTW_PHL_STATUS_SUCCESS
#define phl_snd_proc_get_res(_phl_info, _grp, _nsta) RTW_PHL_STATUS_SUCCESS
#define phl_snd_proc_release_res(_phl_info, _grp) RTW_PHL_STATUS_SUCCESS
#define phl_snd_proc_precfg(_phl_info, _grp) RTW_PHL_STATUS_SUCCESS
#define phl_snd_proc_start_sounding_fw(_phl_info, _grp) RTW_PHL_STATUS_SUCCESS
#define phl_snd_proc_postcfg(_phl_info, _grp) RTW_PHL_STATUS_SUCCESS
#define phl_snd_proc_chk_condition(_phl_info, _grp) RTW_PHL_STATUS_SUCCESS
#define phl_snd_proc_chk_prev_grp(_phl_info, _grp)
#define phl_snd_polling_pri_sta_sts(_phl_info, _grp) RTW_PHL_STATUS_SUCCESS
#define phl_snd_mac_ctrl(_phl_info, _band, _ctrl)
#endif
#endif
