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
#ifndef __HALBB_CMN_RPT_H__
#define __HALBB_CMN_RPT_H__

/*@--------------------------[Define] ---------------------------------------*/
#define RSSI_MA_H 4 /*moving average factor for RSSI: 2^4=16 */
#define RSSI_MA_M 3
#define RSSI_MA_L 2
#define RSSI_MA_UL 1

#define BB_HIST_SIZE		12
#define BB_HIST_TH_SIZE	(BB_HIST_SIZE - 1)

#define POP_HIST_SIZE 4

/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/
struct bb_rssi_mu_acc_info { /*all in U(8,1)*/
	/*acc value*/
	u32 rssi_t_avg_acc;
	u32 rssi_t_acc[HALBB_MAX_PATH];  /*VHT, HE*/
};

struct bb_pkt_cnt_bcn_info {
	u8		pkt_cnt_beacon;
	u8		beacon_cnt_in_period; /*@beacon cnt within watchdog period*/
	u16		beacon_phy_rate;
};

struct bb_rssi_su_acc_info { /*all in U(8,1)*/
	/*acc value*/
	u32 rssi_cck_avg_acc;
	u32 rssi_cck_acc[HALBB_MAX_PATH];
	u32 rssi_ofdm_avg_acc;
	u32 rssi_ofdm_acc[HALBB_MAX_PATH]; /*L-OFDM*/
	u32 rssi_t_avg_acc;
	u32 rssi_t_acc[HALBB_MAX_PATH];  /*HT, VHT, HE*/
};

struct bb_pkt_cnt_cap_info {
	u32 pkt_cnt_ldpc; /*pkt_cnt_ofdm = pkt_cnt_ldpc + pkt_cnt_bcc*/
	u32 pkt_cnt_bcc;
	u32 pkt_cnt_stbc;
	u32 pkt_cnt_subf;
	u32 pkt_cnt_mubf;
};

struct bb_physts_hist_info {
	u16 evm_1ss[BB_HIST_SIZE];
	u16 evm_min_hist[BB_HIST_SIZE];
	u16 evm_max_hist[BB_HIST_SIZE];
	u16 snr_avg_hist[BB_HIST_SIZE];
	u16 cn_avg_hist[BB_HIST_SIZE];
	u16 cfo_avg_hist[BB_HIST_SIZE]; /*ABS(cfo) 0~256 Khz*/
};

struct bb_physts_hist_th_info {
	u8 evm_hist_th[BB_HIST_TH_SIZE]; /*threshold*/
	u8 cn_hist_th[BB_HIST_TH_SIZE]; /*threshold*/
	u8 cfo_hist_th[BB_HIST_TH_SIZE]; /*threshold*/
};

struct bb_physts_acc_info {
	u32 evm_1ss;	/*U(8,2)*/ /*only for 1SS & L-OFDM*/
	u32 evm_min_acc; /*U(8,2)*/ /*only for >= 2SS*/
	u32 evm_max_acc; /*U(8,2)*/ /*only for >= 2SS*/
	u32 snr_avg_acc; /*U(6,0)*/
	u32 snr_per_path_acc[HALBB_MAX_PATH]; /*U(6,0)*/
	u32 cn_avg_acc;  /*U(7,1)*/
	s32 cfo_avg_acc; /*U(8,2)*/
};

struct bb_physts_pop_info {
	u16 pop_hist_cck[POP_HIST_SIZE]; /*U(8,0) pop_idx histogram*/
	u16 pop_hist_ofdm[POP_HIST_SIZE]; /*U(8,0) pop_idx histogram*/
};

struct bb_cmn_rpt_info {
	bool				is_cck_rate;
	u8				consec_idle_prd_su; /*consecutive idle period*/
	u8				consec_idle_prd_mu;
	struct bb_rate_info		bb_rate_i;
	struct bb_pkt_cnt_bcn_info	bb_pkt_cnt_bcn_i; /*beacon info*/
	struct bb_pkt_cnt_cap_info	bb_pkt_cnt_all_i; /*capibility info*/
	struct bb_pkt_cnt_su_info	bb_pkt_cnt_su_i; /*Packet count*/
	struct bb_rssi_su_acc_info	bb_rssi_su_acc_i; /*acc RSSI*/
	struct bb_rssi_su_avg_info	bb_rssi_su_avg_i; /*avg RSSI*/
	struct bb_physts_hist_th_info	bb_physts_hist_th_i;
	struct bb_physts_hist_info	bb_physts_hist_i; /*phy-sts histogram*/
	struct bb_physts_acc_info 	bb_physts_acc_i; /*acc phy-sts*/
	struct bb_physts_avg_info 	bb_physts_avg_i; /*avg phy-sts*/
	struct bb_physts_pop_info 	bb_physts_pop_i; /*pop info*/
	/*[MU]*/
	struct bb_pkt_cnt_mu_info	bb_pkt_cnt_mu_i; /*Packet count*/
	struct bb_rssi_mu_acc_info	bb_rssi_mu_acc_i; /*acc RSSI*/
	struct bb_rssi_mu_avg_info	bb_rssi_mu_avg_i; /*avg RSSI*/
	struct bb_pkt_cnt_su_store_info bb_pkt_cnt_su_store_i;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_print_hist_2_buf_u8(struct bb_info *bb, u8 *val, u16 len, char *buf,
			    u16 buf_size);
void halbb_print_hist_2_buf(struct bb_info *bb, u16 *val, u16 len, char *buf,
			    u16 buf_size);
u16 halbb_get_plurality_rx_rate_mu(struct bb_info *bb);
u16 halbb_get_plurality_rx_rate_su(struct bb_info *bb);
void halbb_basic_dbg_07_hist_su(struct bb_info *bb);
void halbb_show_rssi_and_rate_distribution_mu(struct bb_info *bb);
void halbb_show_rssi_and_rate_distribution_su(struct bb_info *bb);
void halbb_rx_pkt_cnt_rpt_reset(struct bb_info *bb);
void halbb_cmn_rpt(struct bb_info *bb, struct physts_rxd *desc, u32 physts_bitmap);
void halbb_cmn_info_rpt_store_data(struct bb_info *bb);
void halbb_cmn_info_rpt_reset(struct bb_info *bb);
void halbb_cmn_rpt_init(struct bb_info *bb);
#endif
