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
#ifndef __HALBB_DBG_CNSL_OUT_EX_H__
#define __HALBB_DBG_CNSL_OUT_EX_H__

/*@--------------------------[Define] ---------------------------------------*/

struct halbb_statistic_exp_t {
	u16 tx_rate;
	u8 tx_per;
	#if 1
	struct bb_pkt_cnt_su_info bb_pkt_cnt_exp;
	#else
	u16 pkt_cnt_cck;
	u16 pkt_cnt_ofdm;
	u16 pkt_cnt_1ss;
	u16 pkt_cnt_2ss;
	bool ht_pkt_not_zero;
	bool low_bw_20_occur;
	bool vht_pkt_not_zero;
	bool low_bw_40_occur;
	u16 num_qry_legacy_pkt[12];
	u16 num_qry_ht_pkt[16];
	u16 num_qry_pkt_sc_20m[24];
	u16 num_qry_vht_pkt[24];
	u16 num_qry_pkt_sc_40m[24];
	#endif
	#if 1
	struct bb_rssi_su_avg_info bb_rssi_su_avg_exp;
	#else
	u8 rssi_cck_avg[2];
	u8 rssi_ofdm_avg[2];
	u8 rssi_1ss_avg[2];
	u8 rssi_2ss_avg[2];
	#endif
	//struct bb_rssi_su_acc_info bb_rssi_su_acc_exp;
	//struct bb_physts_avg_info avg_info;
	u8 evm_1ss; /*1ss rate*/
	u8 evm_max; /*2ss rate*/
	u8 evm_min; /*2ss rate*/
	u8 snr_avg;
	s8 edcca_fb_pwdb; /*full bandwidth edcca_pwdb, unit: dBm, value = -128 when Tx*/
};

/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
void halbb_statistic_exp(struct bb_info *bb_0,
			 struct halbb_statistic_exp_t *exp,
			 enum phl_phy_idx phy_idx);
void halbb_statistic_reset(struct bb_info *bb);
void halbb_statistic_exp_en(struct bb_info *bb, bool en);

#endif