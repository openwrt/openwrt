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
#ifndef __HALBB_CFO_TRK_H__
#define __HALBB_CFO_TRK_H__

/*@--------------------------[Define] ---------------------------------------*/

#define		CFO_TRK_TH_SIZE		4
#define		CFO_TRK_TH_4		30 /* @kHz disable CFO_Track threshold*/
#define		CFO_TRK_TH_3		20 /* @kHz disable CFO_Track threshold*/
#define		CFO_TRK_TH_2		10 /* @kHz disable CFO_Track threshold*/
#define		CFO_TRK_TH_1		3 /* @kHz disable CFO_Track threshold*/
#define		CFO_STEP_1		1 /* @kHz disable CFO_Track threshold*/
#define		CFO_STEP_2		1 /* @kHz disable CFO_Track threshold*/
#define		CFO_STEP_3		3 /* @kHz disable CFO_Track threshold*/
#define		CFO_STEP_4		3 /* @kHz disable CFO_Track threshold*/
#define		CFO_TRK_ENABLE_TH	3 /* @kHz enable CFO_Track threshold*/
#define		CFO_TRK_STOP_TH		3 /* @kHz disable CFO_Track threshold*/
#define		NO_PKT_RETURN_TH	10
#define		CFO_VALID_BOUNDARY	50

#define		CFO_SW_COMP_FINE_TUNE	5 /* @kHz expected CFO Comp. per Xcap ofst*/ /*0.8ppm~1ppm per Xcap ofst*/
#define		DIGI_CFO_COMP_LIMIT 5 /* @kHz enable digital CFO comp threshold*/

#define		DIGI_CFO_COMP_LIMIT	5 /* @kHz enable digital CFO comp threshold*/

#define		CFO_RECORD_NUM		6
#define		CFO_LIMIT_PERIOD	60 /*60 sec*/

#define		SC_XO			1 /* xcap setting output value */
#define		SC_XI			0 /* xcap setting input value */

#define		STA_CFO_TOLERANCE_2G	30 /* kHz */
#define		STA_CFO_TOLERANCE_5G	80 /* kHz */

#define		CFO_HW_RPT_2_KHZ(val) (((val) << 1) + ((val) >> 1))
#define		CFO_PERIOD_CNT	15
#define		CFO_TP_UPPER	100 /*MHz*/
#define		CFO_TP_LOWER	50 /*MHz*/
#define		CFO_COMP_PERIOD	250 /*ms*/
#define		CFO_TF_CNT_TH 300
/*@--------------------------[Enum]------------------------------------------*/

enum bb_cfo_trk_src_t {
	CFO_SRC_FD = 0,
	CFO_SRC_PREAMBLE = 1
};

enum bb_cfo_trk_st_t {
	CFO_STATE_0 = 0,
	CFO_STATE_1 = 1,
	CFO_STATE_2 =2
};

enum bb_cfo_trk_acc_mode_t {
	CFO_ACC_MODE_0 = 0,   /*disable ul_ofdma cfo acc after 30s acc*/
	CFO_ACC_MODE_1 = 1,   /*enable ul_ofdma cfo acc after 30s acc*/
};
enum multi_sta_cfo_mode_t {
	PKT_BASED_AVG_MODE	= 0,
	ENTRY_BASED_AVG_MODE	= 1,
	TP_BASED_AVG_MODE	= 2,
};

#ifdef BB_DYN_CFO_TRK_LOP
enum bb_dctl_state_t {
	DCTL_SNR = 0,
	DCTL_LINK = 1,
	DCTL_NUM
};
#endif

/*@--------------------------[Structure]-------------------------------------*/

#ifdef BB_DYN_CFO_TRK_LOP
struct bb_cfo_trk_lop_cr_info {
	u8 dctl_data; /*data tracking loop filter bandwidth selection for 3rd step*/
	u8 dctl_pilot; /*pilot tracking loop filter bandwidth selection for 3rd step*/
};

struct bb_dyn_cfo_trk_lop_info {
	bool			dyn_cfo_trk_loop_en;
	enum bb_dctl_state_t	dyn_cfo_trk_loop_state;
	u16			dctl_snr_th_l;
	u16			dctl_snr_th_h;
	u8			dctl_hold_cnt;
	struct bb_cfo_trk_lop_cr_info  bb_cfo_trk_lop_cr_i[DCTL_NUM];
};
#endif

struct bb_cfo_trk_cr_info {
	u32 r_cfo_comp_seg0_312p5khz;
	u32 r_cfo_comp_seg0_312p5khz_m;
	u32 r_cfo_comp_seg0_312p5khz_1;
	u32 r_cfo_comp_seg0_312p5khz_1_m;
	u32 r_cfo_comp_seg0_312p5khz_2;
	u32 r_cfo_comp_seg0_312p5khz_2_m;
	u32 r_cfo_comp_seg0_312p5khz_3;
	u32 r_cfo_comp_seg0_312p5khz_3_m;
	u32 r_cfo_comp_seg0_vld;
	u32 r_cfo_comp_seg0_vld_m;
	u32 r_cfo_wgting;
	u32 r_cfo_wgting_m;
};

struct bb_cfo_rc_info {
	u8		step_history[CFO_RECORD_NUM];
	u8		step_bitmap;
	bool		damping_lock_en;
	bool		force_damping_step;
	u32		limit_time;
};

struct bb_cfo_diver_info {
	bool		divergence_lock_en;
	u32		limit_time;
};

struct bb_cfo_trk_info {
	struct	bb_cfo_trk_cr_info	bb_cfo_trk_cr_i;
#ifdef HALBB_CFO_DAMPING_CHK
	struct	bb_cfo_rc_info		bb_cfo_rc_i;
#endif
	struct	bb_cfo_diver_info	bb_cfo_div_i;
	bool		cfo_trig_by_timer_en;
	bool		is_adjust;	/*@already modify crystal cap*/
	u8		cfo_th[CFO_TRK_TH_SIZE]; /*u(8,2)*/
	u8		step[CFO_TRK_TH_SIZE];
	u8		cfo_th_en;
	u8		cfo_th_stop; /*u(8,2)*/
	s8		x_cap_ofst;
	u8		crystal_cap;
	u8		def_x_cap;
	u8		x_cap_ub;
	u8		x_cap_lb;
	s32		cfo_avg_pre; /*S(12,2), -512~+511.75 kHz*/
	u32		cfo_pkt_cnt;
	u8		no_pkt_cnt;
	u32		rvrt_val; /*all rvrt_val for pause API must set to u32*/
	u8		tb_tx_comp_cfo_th; /*u(8,2)*/
	u8		sw_comp_fine_tune; /*u(8,2)*/
	u8		bb_cfo_trk_cnt;
	u8		sta_cfo_tolerance;
	bool		man_cfo_tol;
	bool		cfo_dyn_acc_en;
	bool		cfo_trk_by_data_en;
	u8		cfo_period_cnt;
	u32		cfo_tf_cnt_th;
	u32		cfo_tf_cnt_pre;
	enum bb_cfo_trk_src_t		cfo_src;
	enum bb_cfo_trk_st_t		bb_cfo_trk_state;
	enum bb_cfo_trk_acc_mode_t bb_cfo_trk_acc_mode;
	enum multi_sta_cfo_mode_t	multi_sta_cfo_mode;
	#ifdef BB_DYN_CFO_TRK_LOP
	struct bb_dyn_cfo_trk_lop_info bb_dyn_cfo_trk_lop_i;
	#endif
	struct halbb_timer_info cfo_timer_i;
};


struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
#ifdef BB_DYN_CFO_TRK_LOP
void halbb_dyn_cfo_trk_loop_en(struct bb_info *bb, bool en);
void halbb_cfo_trk_loop_cr_cfg(struct bb_info *bb, enum bb_dctl_state_t state);
void halbb_dyn_cfo_trk_loop(struct bb_info *bb);
void halbb_dyn_cfo_trk_loop_init(struct bb_info *bb);
#endif
void halbb_cfo_trk_init(struct bb_info *bb);
void halbb_set_crystal_cap(struct bb_info *bb, u8 crystal_cap);
void halbb_set_cfo_pause_val(struct bb_info *bb, u32 *val_buf, u8 val_len);
void halbb_cfo_acc_io_en(struct bb_info *bb);
void halbb_cfo_acc_timer_init(struct bb_info *bb);
void halbb_cfo_dm(struct bb_info *bb);
void halbb_cfo_watchdog(struct bb_info *bb);
void halbb_parsing_cfo(struct bb_info *bb, u32 physts_bitmap,
		       struct physts_rxd *desc);
void halbb_cfo_trk_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len);
void halbb_cr_cfg_cfo_trk_init(struct bb_info *bb);
void halbb_cfo_ul_ofdma_acc_enable(struct bb_info *bb);
void halbb_cfo_ul_ofdma_acc_disable(struct bb_info *bb);
void halbb_cfo_diver_init(struct bb_info *bb);
#endif
