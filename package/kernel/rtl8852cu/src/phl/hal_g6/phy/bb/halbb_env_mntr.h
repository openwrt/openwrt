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
#ifndef __HALBB_ENV_MNTR_H__
#define __HALBB_ENV_MNTR_H__

/*--------------------------[Define] ---------------------------------------*/
#define ENV_MNTR_FAIL_BYTE		0xff
#define ENV_MNTR_FAIL_WORD		0xffff
#define ENV_MNTR_FAIL_DWORD	0xffffffff
#define MAX_ENV_MNTR_TIME		8	/*second*/
#define MS_TO_4US_RATIO		250
/*NHM*/
#define RSSI_2_NHM_TH(rssi)	((rssi) << 1) /*NHM_threshold = u(8,1)*/
#define NHM_TH_2_RSSI(th)		(th >> 1)
#define NHM_PWR_OFST		20
#define NHM_NOISE_F_TH		60	/*60/2 = 30 = -80 dBm*/
#define NHM_160M_NOISE_F_TH	70	/*70/2 = 35 = -75 dBm*/
#define NHM_WA_TH			109	/*109 = -1 dBm*/
#define NHM_WA_PWR		26 /*26 - 110 = -84dBm, only for 52A*/
/*FAHM*/
#define RSSI_2_FAHM_TH(rssi)	((rssi) << 1) /*FAHM_threshold = u(8,1)*/
#define FAHM_TH_2_RSSI(th)		(th >> 1)
#define FAHM_PWR_OFST		20
#define FAHM_WA_TH		109	/*109 = -1 dBm*/
#define FAHM_INCLU_FA		BIT(0)
#define FAHM_INCLU_CRC_OK		BIT(1)
#define FAHM_INCLU_CRC_ERR		BIT(2)
#define FAHM_TH_NUM		11	/*threshold number of FAHM*/
#define FAHM_RPT_NUM		12
/*--------------------------[Enum]------------------------------------------*/
enum ccx_unit {
	CCX_04_US		= 0,	/*4us*/
	CCX_08_US		= 1,	/*8us*/
	CCX_16_US		= 2,	/*16us*/
	CCX_32_US		= 3	/*32us*/
};

enum fahm_application {
	FAHM_INIT		= 0,
	FAHM_BACKGROUND		= 1, /*IEEE 11K for background*/
	FAHM_DIG		= 2,
	FAHM_DBG_11K		= 3, /*IEEE 11K for dbg cmd*/
	FAHM_DBG_MANUAL		= 4 /*fahm_th[0] & th_ofst is manual*/
};

enum env_mntr_sel {
	NHM_SEL			= BIT(0),
	CLM_SEL			= BIT(1),
	IFS_CLM_SEL		= BIT(2),
	EDCCA_CLM_SEL		= BIT(3),
	FAHM_SEL		= BIT(4),
	CCX_SEL_EXCLU_NHM_FAHM	= 0xe,
	CCX_SEL_EXCLU_FAHM	= 0xf,
	CCX_SEL			= 0x1f
};

/*--------------------------[Structure]-------------------------------------*/

struct bb_env_mntr_cr_info {
	u32 ccx_en;
	u32 ccx_en_m;
	u32 ccx_trig_opt;
	u32 ccx_trig_opt_m;
	u32 ccx_trig;
	u32 ccx_trig_m;
	u32 ccx_edcca_opt;
	u32 ccx_edcca_opt_m;
	u32 ccx_source_sel;
	u32 ccx_source_sel_m;
	u32 clm_unit_idx;
	u32 clm_unit_idx_m;
	u32 clm_en;
	u32 clm_en_m;
	u32 clm_opt;
	u32 clm_opt_m;
	u32 clm_period;
	u32 clm_period_m;
	u32 clm_dbg_sel;
	u32 clm_dbg_sel_m;
	u32 clm_nav_en;
	u32 clm_nav_en_m;
	u32 clm_rssi_th_en;
	u32 clm_rssi_th_en_m;
	u32 clm_rssi_th;
	u32 clm_rssi_th_m;
	u32 clm_cnt;
	u32 clm_cnt_m;
	u32 clm_rdy;
	u32 clm_rdy_m;
	u32 edcca_clm_period;
	u32 edcca_clm_period_m;
	u32 edcca_clm_unit_idx;
	u32 edcca_clm_unit_idx_m;
	u32 edcca_clm_en;
	u32 edcca_clm_en_m;
	u32 edcca_clm_cnt;
	u32 edcca_clm_cnt_m;
	u32 edcca_clm_rdy;
	u32 edcca_clm_rdy_m;
	u32 nhm_en;
	u32 nhm_en_m;
	u32 nhm_method_sel;
	u32 nhm_method_sel_m;
	u32 nhm_period;
	u32 nhm_period_m;
	u32 nhm_unit_idx;
	u32 nhm_unit_idx_m;
	u32 nhm_inclu_cca;
	u32 nhm_inclu_cca_m;
	u32 nhm_nav_en;
	u32 nhm_nav_en_m;
	u32 nhm_rssi_th_en;
	u32 nhm_rssi_th_en_m;
	u32 nhm_rssi_th;
	u32 nhm_rssi_th_m;
	u32 nhm_th0;
	u32 nhm_th0_m;
	u32 nhm_th1;
	u32 nhm_th1_m;
	u32 nhm_th2;
	u32 nhm_th2_m;
	u32 nhm_th3;
	u32 nhm_th3_m;
	u32 nhm_th4;
	u32 nhm_th4_m;
	u32 nhm_th5;
	u32 nhm_th5_m;
	u32 nhm_th6;
	u32 nhm_th6_m;
	u32 nhm_th7;
	u32 nhm_th7_m;
	u32 nhm_th8;
	u32 nhm_th8_m;
	u32 nhm_th9;
	u32 nhm_th9_m;
	u32 nhm_th10;
	u32 nhm_th10_m;
	u32 nhm_cnt0;
	u32 nhm_cnt0_m;
	u32 nhm_cnt1;
	u32 nhm_cnt1_m;
	u32 nhm_cnt2;
	u32 nhm_cnt2_m;
	u32 nhm_cnt3;
	u32 nhm_cnt3_m;
	u32 nhm_cnt4;
	u32 nhm_cnt4_m;
	u32 nhm_cnt5;
	u32 nhm_cnt5_m;
	u32 nhm_cnt6;
	u32 nhm_cnt6_m;
	u32 nhm_cnt7;
	u32 nhm_cnt7_m;
	u32 nhm_cnt8;
	u32 nhm_cnt8_m;
	u32 nhm_cnt9;
	u32 nhm_cnt9_m;
	u32 nhm_cnt10;
	u32 nhm_cnt10_m;
	u32 nhm_cnt11;
	u32 nhm_cnt11_m;
	u32 nhm_cca_cnt;
	u32 nhm_cca_cnt_m;
	u32 nhm_tx_cnt;
	u32 nhm_tx_cnt_m;
	u32 nhm_idle_cnt;
	u32 nhm_idle_cnt_m;
	u32 nhm_rdy;
	u32 nhm_rdy_m;
	u32 fahm_en;
	u32 fahm_en_m;
	u32 fahm_ofdm_en;
	u32 fahm_ofdm_en_m;
	u32 fahm_cck_en;
	u32 fahm_cck_en_m;
	u32 fahm_numer_opt;
	u32 fahm_numer_opt_m;
	u32 fahm_denom_opt;
	u32 fahm_denom_opt_m;
	u32 fahm_dis_count_each_mpdu;
	u32 fahm_dis_count_each_mpdu_m;
	u32 fahm_period;
	u32 fahm_period_m;
	u32 fahm_unit_idx;
	u32 fahm_unit_idx_m;
	u32 fahm_method_sel;
	u32 fahm_method_sel_m;
	u32 fahm_th0;
	u32 fahm_th0_m;
	u32 fahm_th1;
	u32 fahm_th1_m;
	u32 fahm_th2;
	u32 fahm_th2_m;
	u32 fahm_th3;
	u32 fahm_th3_m;
	u32 fahm_th4;
	u32 fahm_th4_m;
	u32 fahm_th5;
	u32 fahm_th5_m;
	u32 fahm_th6;
	u32 fahm_th6_m;
	u32 fahm_th7;
	u32 fahm_th7_m;
	u32 fahm_th8;
	u32 fahm_th8_m;
	u32 fahm_th9;
	u32 fahm_th9_m;
	u32 fahm_th10;
	u32 fahm_th10_m;
	u32 fahm_cnt0;
	u32 fahm_cnt0_m;
	u32 fahm_cnt1;
	u32 fahm_cnt1_m;
	u32 fahm_cnt2;
	u32 fahm_cnt2_m;
	u32 fahm_cnt3;
	u32 fahm_cnt3_m;
	u32 fahm_cnt4;
	u32 fahm_cnt4_m;
	u32 fahm_cnt5;
	u32 fahm_cnt5_m;
	u32 fahm_cnt6;
	u32 fahm_cnt6_m;
	u32 fahm_cnt7;
	u32 fahm_cnt7_m;
	u32 fahm_cnt8;
	u32 fahm_cnt8_m;
	u32 fahm_cnt9;
	u32 fahm_cnt9_m;
	u32 fahm_cnt10;
	u32 fahm_cnt10_m;
	u32 fahm_cnt11;
	u32 fahm_cnt11_m;
	u32 fahm_denom_cnt;
	u32 fahm_denom_cnt_m;
	u32 fahm_rdy;
	u32 fahm_rdy_m;
	u32 ifs_clm_en;
	u32 ifs_clm_en_m;
	u32 ifs_clm_clr;
	u32 ifs_clm_clr_m;
	u32 ifs_clm_period;
	u32 ifs_clm_period_m;
	u32 ifs_clm_unit_idx;
	u32 ifs_clm_unit_idx_m;
	u32 ifs_t1_en;
	u32 ifs_t1_en_m;
	u32 ifs_t2_en;
	u32 ifs_t2_en_m;
	u32 ifs_t3_en;
	u32 ifs_t3_en_m;
	u32 ifs_t4_en;
	u32 ifs_t4_en_m;
	u32 ifs_t5_en;
	u32 ifs_t5_en_m;
	u32 ifs_t1_th_l;
	u32 ifs_t1_th_l_m;
	u32 ifs_t2_th_l;
	u32 ifs_t2_th_l_m;
	u32 ifs_t3_th_l;
	u32 ifs_t3_th_l_m;
	u32 ifs_t4_th_l;
	u32 ifs_t4_th_l_m;
	u32 ifs_t5_th_l;
	u32 ifs_t5_th_l_m;
	u32 ifs_t1_th_h;
	u32 ifs_t1_th_h_m;
	u32 ifs_t2_th_h;
	u32 ifs_t2_th_h_m;
	u32 ifs_t3_th_h;
	u32 ifs_t3_th_h_m;
	u32 ifs_t4_th_h;
	u32 ifs_t4_th_h_m;
	u32 ifs_t5_th_h;
	u32 ifs_t5_th_h_m;
	u32 ifs_clm_tx_cnt;
	u32 ifs_clm_tx_cnt_m;
	u32 ifs_clm_edcca_exclu_cca;
	u32 ifs_clm_edcca_exclu_cca_m;
	u32 ifs_clm_cckcca_exclu_fa;
	u32 ifs_clm_cckcca_exclu_fa_m;
	u32 ifs_clm_ofdmcca_exclu_fa;
	u32 ifs_clm_ofdmcca_exclu_fa_m;
	u32 ifs_clm_cck_fa;
	u32 ifs_clm_cck_fa_m;
	u32 ifs_clm_ofdm_fa;
	u32 ifs_clm_ofdm_fa_m;
	u32 ifs_clm_t1_his;
	u32 ifs_clm_t1_his_m;
	u32 ifs_clm_t2_his;
	u32 ifs_clm_t2_his_m;
	u32 ifs_clm_t3_his;
	u32 ifs_clm_t3_his_m;
	u32 ifs_clm_t4_his;
	u32 ifs_clm_t4_his_m;
	u32 ifs_clm_t5_his;
	u32 ifs_clm_t5_his_m;
	u32 ifs_clm_t1_avg;
	u32 ifs_clm_t1_avg_m;
	u32 ifs_clm_t2_avg;
	u32 ifs_clm_t2_avg_m;
	u32 ifs_clm_t3_avg;
	u32 ifs_clm_t3_avg_m;
	u32 ifs_clm_t4_avg;
	u32 ifs_clm_t4_avg_m;
	u32 ifs_clm_t5_avg;
	u32 ifs_clm_t5_avg_m;
	u32 ifs_clm_t1_cca;
	u32 ifs_clm_t1_cca_m;
	u32 ifs_clm_t2_cca;
	u32 ifs_clm_t2_cca_m;
	u32 ifs_clm_t3_cca;
	u32 ifs_clm_t3_cca_m;
	u32 ifs_clm_t4_cca;
	u32 ifs_clm_t4_cca_m;
	u32 ifs_clm_t5_cca;
	u32 ifs_clm_t5_cca_m;
	u32 ifs_total_cnt;
	u32 ifs_total_cnt_m;
	u32 ifs_clm_rdy;
	u32 ifs_clm_rdy_m;
	u32 ifs_clm_cca_opt;
	u32 ifs_clm_cca_opt_m;
};

struct fahm_trig_report {
	u8			fahm_rpt_stamp;
};

struct fahm_para_info {
	enum halbb_racing_lv		fahm_rac_lv;
	u16				fahm_mntr_time;	/*0~2097ms*/
	enum fahm_application		fahm_app;
	u8				fahm_manual_th_ofst;
	u8				fahm_manual_th0; /*dbg manual mode*/
	u8				fahm_numer_opt;
	u8				fahm_denom_opt;
};

struct fahm_report {
	u8			fahm_rpt_stamp;
	bool			fahm_rpt_result;
	u8			fahm_rpt[FAHM_RPT_NUM]; /*percent*/
	u8			fahm_ratio; /*percent*/
	u8			fahm_denom_ratio; /*percent*/
	u16			fahm_permil; /*permil*/
	u16			fahm_denom_permil; /*permil*/
	u8			fahm_pwr; /*110+x(dBm), unit = 1dB*/
	u8			fahm_pwr_0p5; /*110+x/2(dBm), unit = 0.5dB*/
};

struct bb_env_mntr_info {
	struct bb_env_mntr_cr_info	bb_env_mntr_cr_i;
	/*sw ctrl*/
	u32				ccx_trigger_time;
	u8				ccx_rpt_stamp;
	u8				ccx_watchdog_result;
	bool				ccx_ongoing;
	u8				ccx_rac_lv;
	bool				ccx_manual_ctrl;
	bool				ccx_ext_loss_update;
	s8				ccx_ext_loss_pre; /* S(8.1)*/
	s8				ccx_ext_loss; /* S(8.1)*/
	u16				clm_mntr_time; /*0~2097 ms*/
	enum clm_application		clm_app;
	u16				nhm_mntr_time; /*0~2097 ms*/
	enum nhm_application		nhm_app;
	u16				ifs_clm_mntr_time; /*0~2097 ms*/
	enum ifs_clm_application	ifs_clm_app;
	u32				fahm_trigger_time;
	u8 				fahm_rpt_stamp;
	bool				fahm_watchdog_result;
	bool				fahm_ongoing;
	u8				fahm_rac_lv;
	bool				fahm_manual_ctrl;
	u16				fahm_mntr_time; /*0~2097 ms*/
	enum fahm_application		fahm_app;
	u16				edcca_clm_mntr_time; /*0~2097 ms*/
	enum edcca_clm_application	edcca_clm_app;
	/*hw ctrl*/
	u16				ccx_period;
	u8				ccx_unit_idx;
	u8				ccx_edcca_opt;
	enum clm_opt_input		clm_input_opt;
	bool				clm_nav_en;
	bool				clm_rssi_th_en;
	u8				clm_rssi_th;
	enum nhm_opt_cca_all		nhm_include_cca;
	u8				nhm_th[NHM_TH_NUM];
	bool				nhm_nav_en;
	bool				nhm_rssi_th_en;
	u8				nhm_rssi_th;
	u16				ifs_clm_th_l[BE_IFS_CLM_NUM];
	u16				ifs_clm_th_h[BE_IFS_CLM_NUM];
	u16				fahm_period;
	u8				fahm_numer_opt;
	u8				fahm_denom_opt;
	u8				fahm_th[FAHM_TH_NUM];
	/*hw report*/
	u16			clm_result; /*sample cnt*/
	u16			nhm_result[NHM_RPT_NUM]; /*sample cnt*/
	u16			nhm_tx_cnt; /*sample cnt*/
	u16			nhm_cca_cnt; /*sample cnt*/
	u16			nhm_idle_cnt; /*sample cnt*/
	u16			ifs_clm_tx; /*sample cnt*/
	u16			ifs_clm_edcca_excl_cca; /*sample cnt*/
	u16			ifs_clm_ofdmfa; /*sample cnt*/
	u16			ifs_clm_ofdmcca_excl_fa; /*sample cnt*/
	u16			ifs_clm_cckfa; /*sample cnt*/
	u16			ifs_clm_cckcca_excl_fa; /*sample cnt*/
	u16			ifs_clm_total_ifs; /*cnt*/
	u16			ifs_clm_his[BE_IFS_CLM_NUM]; /*cnt*/
	u16			ifs_clm_avg[BE_IFS_CLM_NUM]; /*sample cnt*/
	u16			ifs_clm_cca[BE_IFS_CLM_NUM]; /*sample cnt*/
	u16			fahm_result[FAHM_RPT_NUM]; /*sample cnt*/
	u16			fahm_denom_result; /*sample cnt*/
	u16			edcca_clm_result; /*sample cnt*/
	/*sw report*/
	u8			clm_ratio; /*percent*/
	u16			nhm_sw_result[NHM_RPT_NUM]; /*sample cnt*/
	u8			nhm_wgt[NHM_RPT_NUM]; /*dBm+110*/
	u8			nhm_rpt[NHM_RPT_NUM]; /*percent*/
	u8			nhm_tx_ratio; /*percent*/
	u8			nhm_cca_ratio; /*percent*/
	u8			nhm_idle_ratio; /*percent*/
	u8			nhm_ratio; /*percent*/
	u16			nhm_result_sum; /*sample cnt*/
	u8			nhm_pwr; /*110+x => dBm, unit = 1dB*/
	u8			nhm_pwr_0p5; /*110+x/2 => dBm, unit = 0.5dB*/
	u8			ifs_clm_tx_ratio; /*percent*/
	u8			ifs_clm_edcca_excl_cca_ratio; /*percent*/
	u8			ifs_clm_cck_fa_ratio; /*percent*/
	u8			ifs_clm_ofdm_fa_ratio; /*percent*/
	u8			ifs_clm_cck_cca_excl_fa_ratio; /*percent*/
	u8			ifs_clm_ofdm_cca_excl_fa_ratio; /*percent*/
	u16			ifs_clm_cck_fa_permil; /*permil*/
	u16			ifs_clm_ofdm_fa_permil; /*permil*/
	u32			ifs_clm_ifs_avg[BE_IFS_CLM_NUM]; /*us*/
	u32			ifs_clm_cca_avg[BE_IFS_CLM_NUM]; /*us*/
	u16			fahm_sw_result[FAHM_RPT_NUM]; /*sample cnt*/
	u8			fahm_wgt[FAHM_RPT_NUM]; /*dBm+110*/
	u8			fahm_rpt[FAHM_RPT_NUM]; /*percent*/
	u16			fahm_result_sum; /*sample cnt*/
	u8			fahm_ratio; /*percent*/
	u8			fahm_denom_ratio; /*percent*/
	u16			fahm_permil; /*permil*/
	u16			fahm_denom_permil; /*permil*/
	u8			fahm_pwr; /*110+x => dBm, unit = 1dB*/
	u8			fahm_pwr_0p5; /*110+x/2 => dBm, unit = 0.5dB*/
	u8			edcca_clm_ratio; /*percent*/
	u8			edcca_noise_bg; /*dBm+110, only for 52A*/
	struct env_mntr_rpt	env_mntr_rpt_bg;
	struct ccx_para_info	ccx_para_info_bg;
	struct fahm_report	fahm_report_bg;
	struct fahm_para_info	fahm_para_info_bg;
	u16			idle_pwr_physts; /*RSSI u(16,3) Idle time pwr from physts*/
};

struct bb_c2h_nhm_info {
	u8			nhm_rpt[NHM_RPT_NUM]; /*percent*/
	u8			nhm_tx_ratio; /*percent*/
	u8			nhm_cca_ratio; /*percent*/
	u8			nhm_idle_ratio; /*percent*/
	u8			nhm_ratio; /*percent*/
	u8			nhm_pwr_0p5; /*110+x/2 => dBm, unit = 0.5dB*/
	u8 			phy_idx;
};

/*--------------------------[Prptotype]-------------------------------------*/
struct bb_info;

u32 halbb_ccx_idx_cnt_2_us(struct bb_info *bb, u16 idx_cnt);
#ifdef CLM_SUPPORT
void halbb_clm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len);
#endif
#ifdef NHM_SUPPORT
void halbb_nhm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		   char *output, u32 *_out_len);
#endif
#ifdef IFS_CLM_SUPPORT
void halbb_ifs_clm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len);
#endif
#ifdef FAHM_SUPPORT
bool halbb_fahm_trigger(struct bb_info *bb, struct fahm_para_info *para,
			struct fahm_trig_report *trig_rpt);
bool halbb_fahm_result(struct bb_info *bb, struct fahm_report *rpt);
void halbb_fahm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		    char *output, u32 *_out_len);
#endif
#ifdef EDCCA_CLM_SUPPORT
void halbb_edcca_clm_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			 char *output, u32 *_out_len);
#endif
u32 halbb_env_mntr_get_fw_result_c2h(struct bb_info *bb, u8 *c2h);
void halbb_env_mntr_log(struct bb_info *bb, u32 dbg_comp);
void halbb_idle_time_pwr_physts(struct bb_info *bb, struct physts_rxd *desc,
				bool is_cck_rate);
void halbb_env_mntr(struct bb_info *bb);
void halbb_env_mntr_init(struct bb_info *bb);
void halbb_env_mntr_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			char *output, u32 *_out_len);
void halbb_cr_cfg_env_mntr_init(struct bb_info *bb);
#endif
