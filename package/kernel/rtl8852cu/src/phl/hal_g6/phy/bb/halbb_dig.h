/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#ifndef __HALBB_DIG_H__
#define __HALBB_DIG_H__

#define DIG_VERSION "5.0"
//#define DIG_DBCC_DEV_TMP
/*@--------------------------[Define] ---------------------------------------*/
#define BB_LNA_SIZE	7
#define BB_TIA_SIZE	2

#define IGI_RSSI_TH_NUM 5
#define FA_TH_NUM 4
#define RSSI_MAX 110
#define RSSI_MIN 0
#ifdef DIG_DBCC_DEV_TMP
#define IGI_NOLINK (38 + 20)
#else
#define IGI_NOLINK 38
#endif
#define LNA_IDX_MAX 6
#define LNA_IDX_MIN 0
#define TIA_IDX_MAX 1
#define TIA_IDX_MIN 0
#define RXB_IDX_MAX 31
#define RXB_IDX_MIN 0
#define LNA6_GAIN 24
#define LNA5_GAIN 16
#define LNA4_GAIN 8
#define LNA3_GAIN 0
#define LNA2_GAIN (-8)
#define LNA1_GAIN (-16)
#define LNA0_GAIN (-24)
#define TIA1_GAIN_A 20
#define TIA0_GAIN_A 12
#define TIA1_GAIN_G 24
#define TIA0_GAIN_G 16
#define IGI_OFFSET_MAX 25 /* IGI window size */
#define	IGI_MAX_PERFORMANCE_MODE 0x5a
#define	IGI_MAX_BALANCE_MODE 0x3e
#define PD_TH_MAX_RSSI 70 /*  -40dBm */
#define PD_TH_MIN_RSSI 8  /* -102dBm */
#define PD_TH_BW80_CMP_VAL 6
#define PD_TH_BW40_CMP_VAL 3
#define PD_TH_BW20_CMP_VAL 0
#define PD_TH_SB_FLTR_CMP_VAL 7
#define DIG_CCX_WD_TRIGTIME 1900
#define IGI_EDCCA_GAP_LIMIT 35
#ifdef HALBB_DIG_TDMA_SUPPORT
#define IGI_MAX_AT_STATE_L	0x26
#define WACHDOG_PERIOD_IN_MS	2000
#define H_STATE_NUM_MAX		20
#define L_STATE_NUM_MAX		10
#endif
#define DIG_RECORD_NUM		6
#define DIG_LIMIT_PERIOD	60 /*60 sec*/

#ifdef HALBB_DBG_TRACE_SUPPORT
#define BB_DIG_DBG(bb, lv, fmt, ...)\
	do {\
		if(bb->dbg_component & DBG_DIG && bb->bb_dig_i.dbg_lv >= lv) {\
			_os_dbgdump("[BB][%d]" fmt, bb->bb_phy_idx, ##__VA_ARGS__);\
		}\
	} while (0)
#else
#define BB_DIG_DBG(bb, lv, fmt, ...)
#endif

/*@--------------------------[Enum]------------------------------------------*/
enum dig_noisy_level {
	DIG_NOISY_LV0		= 0,	/*FA free*/
	DIG_NOISY_LV1		= 1,
	DIG_NOISY_LV2		= 2,
	DIG_NOISY_LV3		= 3,
	DIG_NOISY_LV_MAX	= 4
};

#ifdef HALBB_DIG_TDMA_SUPPORT
enum dig_tdma_state {
	DIG_TDMA_LOW	= 0,
	DIG_TDMA_HIGH	= 1
};
#endif

enum dig_dbg_level {
	DIG_DBG_LV0	= 0,
	DIG_DBG_LV1	= 1,
	DIG_DBG_LV2	= 2
};
/*@--------------------------[Structure]-------------------------------------*/
struct bb_dig_cr_info {
	u32 path0_ib_pbk;
	u32 path0_ib_pbk_m;
	u32 path0_ib_pkpwr;
	u32 path0_ib_pkpwr_m;
	u32 path1_ib_pbk;
	u32 path1_ib_pbk_m;
	u32 path1_ib_pkpwr;
	u32 path1_ib_pkpwr_m;
	u32 path0_lna_init_idx;
	u32 path0_lna_init_idx_m;
	u32 path1_lna_init_idx;
	u32 path1_lna_init_idx_m;
	u32 path0_tia_init_idx;
	u32 path0_tia_init_idx_m;
	u32 path1_tia_init_idx;
	u32 path1_tia_init_idx_m;
	u32 path0_rxb_init_idx;
	u32 path0_rxb_init_idx_m;
	u32 path1_rxb_init_idx;
	u32 path1_rxb_init_idx_m;
	u32 seg0r_pd_spatial_reuse_en_a;
	u32 seg0r_pd_spatial_reuse_en_a_m;
	u32 seg0r_pd_lower_bound_a;
	u32 seg0r_pd_lower_bound_a_m;
	u32 path0_p20_follow_by_pagcugc_en_a;
	u32 path0_s20_follow_by_pagcugc_en_a;
	u32 path1_p20_follow_by_pagcugc_en_a;
	u32 path1_s20_follow_by_pagcugc_en_a;
	u32 path0_p20_follow_by_pagcugc_en_a_m;
	u32 path0_s20_follow_by_pagcugc_en_a_m;
	u32 path1_p20_follow_by_pagcugc_en_a_m;
	u32 path1_s20_follow_by_pagcugc_en_a_m;
	u32 path0_lna_err_g0_a;
	u32 path0_lna_err_g0_a_m;
	u32 path0_lna_err_g0_g;
	u32 path0_lna_err_g0_g_m;
	u32 path0_lna_err_g1_a;
	u32 path0_lna_err_g1_a_m;
	u32 path0_lna_err_g1_g;
	u32 path0_lna_err_g1_g_m;
	u32 path0_lna_err_g2_a;
	u32 path0_lna_err_g2_a_m;
	u32 path0_lna_err_g2_g;
	u32 path0_lna_err_g2_g_m;
	u32 path0_lna_err_g3_a;
	u32 path0_lna_err_g3_a_m;
	u32 path0_lna_err_g3_g;
	u32 path0_lna_err_g3_g_m;
	u32 path0_lna_err_g4_a;
	u32 path0_lna_err_g4_a_m;
	u32 path0_lna_err_g4_g;
	u32 path0_lna_err_g4_g_m;
	u32 path0_lna_err_g5_a;
	u32 path0_lna_err_g5_a_m;
	u32 path0_lna_err_g5_g;
	u32 path0_lna_err_g5_g_m;
	u32 path0_lna_err_g6_a;
	u32 path0_lna_err_g6_a_m;
	u32 path0_lna_err_g6_g;
	u32 path0_lna_err_g6_g_m;
	u32 path0_tia_err_g0_a;
	u32 path0_tia_err_g0_a_m;
	u32 path0_tia_err_g0_g;
	u32 path0_tia_err_g0_g_m;
	u32 path0_tia_err_g1_a;
	u32 path0_tia_err_g1_a_m;
	u32 path0_tia_err_g1_g;
	u32 path0_tia_err_g1_g_m;
	u32 path1_lna_err_g0_a;
	u32 path1_lna_err_g0_a_m;
	u32 path1_lna_err_g0_g;
	u32 path1_lna_err_g0_g_m;
	u32 path1_lna_err_g1_a;
	u32 path1_lna_err_g1_a_m;
	u32 path1_lna_err_g1_g;
	u32 path1_lna_err_g1_g_m;
	u32 path1_lna_err_g2_a;
	u32 path1_lna_err_g2_a_m;
	u32 path1_lna_err_g2_g;
	u32 path1_lna_err_g2_g_m;
	u32 path1_lna_err_g3_a;
	u32 path1_lna_err_g3_a_m;
	u32 path1_lna_err_g3_g;
	u32 path1_lna_err_g3_g_m;
	u32 path1_lna_err_g4_a;
	u32 path1_lna_err_g4_a_m;
	u32 path1_lna_err_g4_g;
	u32 path1_lna_err_g4_g_m;
	u32 path1_lna_err_g5_a;
	u32 path1_lna_err_g5_a_m;
	u32 path1_lna_err_g5_g;
	u32 path1_lna_err_g5_g_m;
	u32 path1_lna_err_g6_a;
	u32 path1_lna_err_g6_a_m;
	u32 path1_lna_err_g6_g;
	u32 path1_lna_err_g6_g_m;
	u32 path1_tia_err_g0_a;
	u32 path1_tia_err_g0_a_m;
	u32 path1_tia_err_g0_g;
	u32 path1_tia_err_g0_g_m;
	u32 path1_tia_err_g1_a;
	u32 path1_tia_err_g1_a_m;
	u32 path1_tia_err_g1_g;
	u32 path1_tia_err_g1_g_m;
	u32 cca_rssi_lmt_en_a;
	u32 cca_rssi_lmt_en_a_m;
	u32 rssi_nocca_low_th_a;
	u32 rssi_nocca_low_th_a_m;
	u32 path0_dig_mode_en_a;
	u32 path0_dig_mode_en_a_m;
	u32 path0_igi_for_dig_a;
	u32 path0_igi_for_dig_a_m;
	u32 path0_backoff_wb_gain_a;
	u32 path0_backoff_wb_gain_a_m;
	u32 path1_dig_mode_en_a;
	u32 path1_dig_mode_en_a_m;
	u32 path1_igi_for_dig_a;
	u32 path1_igi_for_dig_a_m;
	u32 path1_backoff_wb_gain_a;
	u32 path1_backoff_wb_gain_a_m;
};

struct agc_gaincode_set {
	u8 lna_idx;
	u8 tia_idx;
	u8 rxb_idx;
};

struct bb_dig_fa_info {
	u16 fa_r_cck_onesec;
	u16 fa_r_ofdm_onesec;
	u16 fa_r_onesec;	/* overall fa_ratio */
};

struct bb_dig_op_para_unit {
	bool			dyn_pd_th_en;
	u8			igi_rssi_th[IGI_RSSI_TH_NUM];
	u16			fa_th[FA_TH_NUM];	/* permil */
};

#ifdef HALBB_DIG_DAMPING_CHK
struct bb_dig_record_info {
	u8		igi_bitmap; /*@Don't add any new parameter before this*/
	u8		igi_history[DIG_RECORD_NUM];
	u32		fa_history[DIG_RECORD_NUM];
	bool		damping_lock_en;
	u8		damping_limit_val; /*@Limit IGI_dyn_min*/
	u32		limit_time;
	u8		limit_rssi; /*s(8,1)*/
};
#endif

/* struct for state unit, i.e., L/H */
struct bb_dig_op_unit {
#ifdef HALBB_DIG_TDMA_SUPPORT
	enum dig_tdma_state	state_identifier; /* L/H */
#endif
	struct agc_gaincode_set cur_gaincode;
	enum dig_noisy_level	cur_noisy_lv;
	struct agc_gaincode_set force_gaincode;
	struct bb_dig_op_para_unit dig_op_para;
	u16			fa_r_acc;	/* acced one shot fa_ratio */
	u16			fa_r_avg;	/* acced one shot fa_ratio */
	u8			fa_valid_state_cnt;
	u8			state_num_lmt;
	u8			passed_state_cnt;
	u8			igi_fa_rssi; /*final IGI calaulated by FA & RSSI*/
	u8			fa_rssi_ofst;
	u8			abs_igi_max;
	u8			abs_igi_min;
	u8			dyn_igi_max;
	u8			dyn_igi_min;
	u8			pd_low_th_ofst;	/* pd low safe cca region */
	bool			sdagc_follow_pagc_en;
};

struct bb_dig_info {
	bool init_dig_cr_success;
	enum dig_op_mode 	dig_mode;
	enum dig_op_mode 	pre_dig_mode;
	struct bb_dig_cr_info	bb_dig_cr_i;
	struct agc_gaincode_set max_gaincode;
	u8			igi_rssi; //rssi_min
	u8			ib_pbk;
	s8			ib_pkpwr;
	s8			lna_gain_a[BB_LNA_SIZE];
	s8			lna_gain_g[BB_LNA_SIZE];
	s8			*lna_gain;
	s8			tia_gain_a[BB_TIA_SIZE];
	s8			tia_gain_g[BB_TIA_SIZE];
	s8			*tia_gain;
	s8			le_igi_ofst; /* low end mode IGI offset */
	struct bb_dig_op_unit	*p_cur_dig_unit;
	struct bb_dig_op_unit 	dig_state_h_i; /* high state */
#ifdef HALBB_DIG_TDMA_SUPPORT
	struct bb_dig_op_unit	dig_state_l_i; /* low state */
	bool			gaincode_update_en;
	u16			tdma_passed_time_acc; /* check if 1sec reach */
	u8			tdma_timestamp_pre;
	u8			tdma_timestamp_cur;
	struct halbb_timer_info dig_timer_i;
#endif
#ifdef HALBB_ENV_MNTR_SUPPORT
	u8 			fahm_timestamp;
	struct fahm_para_info 	fahm_para_i;
	bool			fahm_is_triggered;
#endif
	struct bb_dig_fa_info 	dig_fa_i;
	enum dig_dbg_level	dbg_lv;
	u32 rvrt_val[DIG_PAUSE_INFO_SIZE];	/*[Pause fucntion] must set to u32*/
	u16 igi_pause_cnt; /*consective pause counter*/
	bool			need_update;
	u16			dig_hold_cnt;
#ifdef HALBB_DIG_DAMPING_CHK
	struct bb_dig_record_info bb_dig_record_i;
	u8			rls_rssi_diff_th; /*s(8,1)*/
	bool dig_dl_en; /*@damping limit function enable*/
#endif
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
#ifdef HALBB_DIG_TDMA_SUPPORT
void halbb_dig_timercheck_watchdog(struct bb_info*);
void halbb_tdmadig_io_en(struct bb_info *bb);
void halbb_dig_timer_init(struct bb_info *bb);
#endif

void halbb_dig_lps(struct bb_info *bb);
void halbb_dig_cfg_bbcr(struct bb_info *bb, u8 igi_new);
u8 halbb_dig_igi_by_ofst(struct bb_info *bb, u8 igi_pre, s8 ofst);
void halbb_dig(struct bb_info *bb);
void halbb_dig_init(struct bb_info *bb);
void halbb_dig_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len);
void halbb_cr_cfg_dig_init(struct bb_info *bb);

void* halbb_get_dig_fa_statistic(struct bb_info *bb);
void halbb_set_dig_pause_val(struct bb_info *bb, u32 *val_buf, u8 val_len);
#ifdef HALBB_DIG_MCC_SUPPORT
void Halbb_init_mccdm(struct bb_info *bb);
void halbb_mccdm_switch(struct bb_info *bb);
u32 halbb_c2h_mccdm_check(struct bb_info *bb, u16 len, u8 *c2h);
#endif
u8 halbb_get_lna_idx(struct bb_info *bb, enum rf_path path);
u8 halbb_get_tia_idx(struct bb_info *bb, enum rf_path path);
u8 halbb_get_rxb_idx(struct bb_info *bb, enum rf_path path);
#endif
