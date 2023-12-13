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
#ifndef __HALBB_ANT_DIV_H__
#define __HALBB_ANT_DIV_H__

/*@--------------------------[Define] ---------------------------------------*/
#define EVM_BASED_ANTDIV 0
#define CN_BASED_ANTDIV 1

#define TP_MAX_DOMINATION 0
#define TP_HIGHEST_DOMINATION 1
#define TP_AVG_DOMINATION 2

#define	ANTDIV_INIT	0xff
#define	MAIN_ANT	1		/*@ant A or ant Main   or S1*/
#define	AUX_ANT		2		/*@AntB or ant Aux   or S0*/
#define	MAX_ANT		3		/* @3 for AP using*/

#define ANT1_2G 0
/* @= ANT2_5G for 8723D  BTG S1 RX S0S1 diversity for 8723D, TX fixed at S1 */
#define ANT2_2G 1
/* @= ANT1_5G for 8723D  BTG S0  RX S0S1 diversity for 8723D, TX fixed at S1 */

#define ANTDIV_MAX_STA_NUM PHL_MAX_STA_NUM
#define ANTDIV_RSSI_TH_HIGH	30
#define ANTDIV_RSSI_TH_LOW	20
#define ANTDIV_PERIOD	1
#define ANTDIV_TRAINING_NUM 6
#define ANTDIV_NOTRAINING_NUM 3

#define FORCE_RSSI_DIFF 10

#define ANTDIV_DELAY 1
#define ANTDIV_INTVL 10

#define ANTDIV_DEC_TP_HIGH 100
#define ANTDIV_DEC_TP_LOW 5
#define ANTDIV_DEC_EVM 4
#define ANTDIV_DEC_EVM_1SS 8
#define TP_LOWER_BOUND 1

/*parameter for 1ss rssi-based antdiv*/
#define ANTDIV_PERIOD_1SS	1
#define ANTDIV_TRAINING_NUM_1SS 2
#define ANTDIV_DELAY_1SS 1
#define ANTDIV_INTVL_1SS 10
#define ANTDIV_DEC_RSSI 3
#define ANTDIV_FIXANT_RSSI 2

/* @Antenna Diversty Control type */
#define	ODM_AUTO_ANT		0
#define	ODM_FIX_MAIN_ANT	1
#define	ODM_FIX_AUX_ANT		2

#define ANTDIV_ON	1
#define ANTDIV_OFF	0

#define ANT_PATH_A	0
#define ANT_PATH_B	1
#define ANT_PATH_AB	2

/*@--------------------------[Enum]------------------------------------------*/
enum bb_antdiv_mode_t {
	AUTO_ANT	= 0,
	FIX_MAIN_ANT	= 1,
	FIX_AUX_ANT	= 2,
};

enum bb_tp_method_t {
	TP_MAX	= 0,
	TP_HIGHEST	= 1,
	TP_AVG	= 2,
};

enum bb_evm_method_t {
	EVM_LINEAR_AVG	= 0,
	EVM_DB_AVG	= 1,
};

enum bb_antdiv_method_t {
	EVM_BASED	= 0,
	CN_BASED	= 1,
};


/*@--------------------------[Structure]-------------------------------------*/

struct bb_antdiv_rssi_info { /*all in U(8,1)*/
	/*acc value*/
	u32 rssi_cck_avg_acc;
	u32 rssi_ofdm_avg_acc;
	u32 rssi_t_avg_acc;
	u8 rssi_cck_avg;
	u8 rssi_ofdm_avg;
	u8 rssi_t_avg;
	u8 rssi_final;
	u16 pkt_cnt_t;
	u16 pkt_cnt_cck;
	u16 pkt_cnt_ofdm;

	u32 main_rssi_cck_avg_acc;
	u32 main_rssi_ofdm_avg_acc;
	u32 main_rssi_t_avg_acc;
	u8 main_rssi_cck_avg;
	u8 main_rssi_ofdm_avg;
	u8 main_rssi_t_avg;
	u8 main_rssi_final;
	u16 main_pkt_cnt_t;
	u16 main_pkt_cnt_cck;
	u16 main_pkt_cnt_ofdm;

	u32 aux_rssi_cck_avg_acc;
	u32 aux_rssi_ofdm_avg_acc;
	u32 aux_rssi_t_avg_acc;
	u8 aux_rssi_cck_avg;
	u8 aux_rssi_ofdm_avg;
	u8 aux_rssi_t_avg;
	u8 aux_rssi_final;
	u16 aux_pkt_cnt_t;
	u16 aux_pkt_cnt_cck;
	u16 aux_pkt_cnt_ofdm;

	u8 rssi_diff;
	bool no_change_flag;
};

struct bb_antdiv_cn_info {
	u32 main_cn_avg_acc;  /*U(7,1)*/
	u32 aux_cn_avg_acc;  /*U(7,1)*/
};

struct bb_antdiv_evm_info {
	u32 main_evm_1ss;/*U(8,2)*/ /*only for 1SS & L-OFDM*/
	u32 main_evm_min_acc; /*U(8,2)*/ /*only for >= 2SS*/
	u32 main_evm_max_acc; /*U(8,2)*/ /*only for >= 2SS*/
	u32 aux_evm_1ss;/*U(8,2)*/ /*only for 1SS & L-OFDM*/
	u32 aux_evm_min_acc; /*U(8,2)*/ /*only for >= 2SS*/
	u32 aux_evm_max_acc; /*U(8,2)*/ /*only for >= 2SS*/
	u8 evm_diff;
	bool no_change_flag;
};

struct bb_antdiv_rate_info {
	/*====[Phy rate counter main ant]=============================================*/
	u16		main_pkt_cnt_cck;
	u16		main_pkt_cnt_ofdm; /*L-OFDM*/
	u16		main_pkt_cnt_t; /*HT, VHT, HE = pkt_cnt_1ss + pkt_cnt_2ss*/
	u16		main_pkt_cnt_1ss; /*HT, VHT, HE*/
	u16		main_pkt_cnt_2ss; /*HT, VHT, HE*/
	/*Legacy*/
	u16		main_pkt_cnt_legacy[LEGACY_RATE_NUM];
	/*HT*/
	u16		main_pkt_cnt_ht[HT_RATE_NUM];
	/*VHT*/
	u16		main_pkt_cnt_vht[VHT_RATE_NUM];
	/*HE*/
	u16		main_pkt_cnt_he[HE_RATE_NUM];
	
	u16		main_max_cnt;
	u16		main_max_idx;
	/*====[Phy rate counter] aux ant=============================================*/
	u16		aux_pkt_cnt_cck;
	u16		aux_pkt_cnt_ofdm; /*L-OFDM*/
	u16		aux_pkt_cnt_t; /*HT, VHT, HE = pkt_cnt_1ss + pkt_cnt_2ss*/
	u16		aux_pkt_cnt_1ss; /*HT, VHT, HE*/
	u16		aux_pkt_cnt_2ss; /*HT, VHT, HE*/
	/*Legacy*/
	u16		aux_pkt_cnt_legacy[LEGACY_RATE_NUM];
	/*HT*/
	u16		aux_pkt_cnt_ht[HT_RATE_NUM];
	/*VHT*/
	u16		aux_pkt_cnt_vht[VHT_RATE_NUM];
	/*HE*/
	u16		aux_pkt_cnt_he[HE_RATE_NUM];

	u16		aux_max_cnt;
	u16		aux_max_idx;
	u16		main_cnt_all;
	u16		aux_cnt_all;

	u64		main_tp;
	u64		aux_tp;
	u64		tp_diff;

	bool no_change_flag;
	bool main_ht_pkt_not_zero;
	bool main_vht_pkt_not_zero;
	bool main_he_pkt_not_zero;
	bool aux_ht_pkt_not_zero;
	bool aux_vht_pkt_not_zero;
	bool aux_he_pkt_not_zero;
};

struct bb_antdiv_cr_info {
	u32 path0_r_ant_train_en;			
	u32 path0_r_ant_train_en_m;			
	u32 path0_r_tx_ant_sel;				
	u32 path0_r_tx_ant_sel_m;			
	u32 path0_r_rfe_buf_en;				
	u32 path0_r_rfe_buf_en_m;			
	u32 path0_r_lnaon_agc;				
	u32 path0_r_lnaon_agc_m;			
	u32 path0_r_trsw_bit_bt;			
	u32 path0_r_trsw_bit_bt_m;			
	u32 path0_r_trsw_s;					
	u32 path0_r_trsw_s_m;				
	u32 path0_r_trsw_o;					
	u32 path0_r_trsw_o_m;				
	u32 path0_r_trswb_o;
	u32 path0_r_trswb_o_m;				
	u32 path0_r_bt_force_antidx;		
	u32 path0_r_bt_force_antidx_m;		
	u32 path0_r_bt_force_antidx_en;		
	u32 path0_r_bt_force_antidx_en_m;	
	u32 path0_r_ant_module_rfe_opt;		
	u32 path0_r_ant_module_rfe_opt_m;	
	u32 path0_r_rfsw_tr;
	u32 path0_r_rfsw_tr_m;
	u32 path0_r_antsel;
	u32 path0_r_antsel_m;				
	u32 path0_r_rfsw_ant_31_0;			
	u32 path0_r_rfsw_ant_31_0_m;		
	u32 path0_r_rfsw_ant_63_32;		
	u32 path0_r_rfsw_ant_63_32_m;		
	u32 path0_r_rfsw_ant_95_64;		
	u32 path0_r_rfsw_ant_95_64_m;		
	u32 path0_r_rfsw_ant_127_96;		
	u32 path0_r_rfsw_ant_127_96_m;		
};

struct bb_antdiv_info {
	struct bb_antdiv_cr_info bb_antdiv_cr_i;
	/* For CN cacluation */
	struct bb_antdiv_cn_info bb_cn_i;
	/* For EVM cacluation */
	struct bb_antdiv_evm_info bb_evm_i;
	/* For MCS cacluation */
	struct bb_antdiv_rate_info bb_rate_i;
	/* For RSSI */
	struct bb_antdiv_rssi_info bb_rssi_i;
	
	enum	bb_antdiv_mode_t antdiv_mode;
	enum	bb_antdiv_method_t antdiv_method;
	enum	bb_antdiv_mode_t pre_antdiv_mode;
	enum	bb_tp_method_t tp_decision_method;
	enum	bb_evm_method_t evm_decision_method;

	/* Training state & period related*/
	u8	antdiv_wd_cnt;
	u8	antdiv_training_state_cnt;
	u32	antdiv_intvl;
	u32	antdiv_delay;
	u32	antdiv_train_num;
	u8	antdiv_period;
	u8	tp_lb;
	u8	rssi_pre;
	u8	antdiv_notrain_cnt;
	u8	antdiv_notrain_num;
	u8	fixant_rssi_diff;

	/* antenna setting */
	u8	pre_target_ant;
	u8	training_ant;
	u8	target_ant;
	u8	target_ant_cn;
	u8	target_ant_evm;
	u8	target_ant_tp;
	u8	target_ant_rssi;

	/* Decision*/
	u16 tp_diff_th_high;
	u16 tp_diff_th_low;
	u8 evm_diff_th;
	u8 rssi_diff_th;
	
	/*Phy-sts related */
	bool get_stats;
	bool antdiv_use_ctrl_frame;
	struct halbb_timer_info antdiv_timer_i;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_antdiv_io_en(struct bb_info *bb);
void halbb_antdiv_timer_init(struct bb_info *bb);
void halbb_cr_cfg_antdiv_init(struct bb_info *bb);
void halbb_antdiv_reg_init(struct bb_info *bb);
void halbb_antdiv_init(struct bb_info *bb);
void halbb_antdiv_reset(struct bb_info *bb);
void halbb_antdiv_reset_training_stat(struct bb_info *bb);
void halbb_antdiv_set_ant(struct bb_info *bb, u8 ant);
void halbb_antdiv_get_highest_mcs(struct bb_info *bb);
void halbb_antdiv_get_evm_target_ant(struct bb_info *bb);
void halbb_antdiv_training_state(struct bb_info *bb);
void halbb_antdiv_decision_state(struct bb_info *bb);
void halbb_antdiv_1ss_decision_state(struct bb_info *bb);
void halbb_evm_based_antdiv(struct bb_info *bb);
void halbb_antenna_diversity(struct bb_info *bb);
void halbb_antdiv_phy_sts(struct bb_info *bb, u32 physts_bitmap,
		       struct physts_rxd *desc);
void halbb_antdiv_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			      char *output, u32 *_out_len);
#endif

