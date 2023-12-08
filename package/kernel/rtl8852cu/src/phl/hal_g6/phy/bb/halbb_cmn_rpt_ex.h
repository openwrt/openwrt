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
#ifndef __HALBB_CMN_RPT_EX_H__
#define __HALBB_CMN_RPT_EX_H__

/*@--------------------------[Define] ---------------------------------------*/

/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/
struct bb_rssi_mu_avg_info { /*all in U(8,1)*/
	u8 rssi_t_avg;
	u8 rssi_t[HALBB_MAX_PATH];  /*VHT, HE*/
};

struct bb_rssi_su_avg_info { /*all in U(8,1)*/
	u8 rssi_cck_avg;
	u8 rssi_cck[HALBB_MAX_PATH];
	u8 rssi_ofdm_avg;
	u8 rssi_ofdm[HALBB_MAX_PATH]; /*L-OFDM*/
	u8 rssi_t_avg;
	u8 rssi_t[HALBB_MAX_PATH];  /*HT, VHT, HE*/
};

struct bb_physts_avg_info {
	u8 evm_1ss; /*U(8,2) 0~63*/
	u8 evm_min; /*U(8,2) 0~63*/
	u8 evm_max; /*U(8,2) 0~63*/
	u8 snr_avg; /*U(6,0) 0~63*/
	u8 snr_per_path_avg[HALBB_MAX_PATH]; /*U(6,0)*/
	u8 cn_avg;  /*U(7,1) 0~63*/
	s16 cfo_avg; /*U(16,2) 0~512*/
};

struct bb_pkt_cnt_mu_info {
	/*====[Phy rate counter]=============================================*/
	u16		pkt_cnt_all; /*VHT, HE = pkt_cnt_1ss + pkt_cnt_2ss*/
	u16		gi_ltf_cnt[RTW_GILTF_MAX];
	u16		pkt_cnt_1ss; /*VHT, HE*/
	u16		pkt_cnt_2ss; /*VHT, HE*/
	u16		pkt_cnt_sc20[LOW_BW_RATE_NUM]; /*@20M SC*/
	bool		sc20_occur;
	/*VHT*/
	u16		pkt_cnt_vht[VHT_RATE_NUM];
	u16		pkt_cnt_sc40[LOW_BW_RATE_NUM]; /*@40M SC*/
	bool		vht_pkt_not_zero;
	bool		sc40_occur;
	u16		pkt_cnt_sc80[LOW_BW_RATE_NUM];
	bool		sc80_occur;
	/*HE*/
	u16		pkt_cnt_he[HE_RATE_NUM];
	bool		he_pkt_not_zero;
};

struct bb_pkt_cnt_su_store_info {
	bool		ht_pkt_not_zero;
	bool		vht_pkt_not_zero;
	bool		he_pkt_not_zero;
};

struct bb_pkt_cnt_su_info {
	/*====[Phy rate counter]=============================================*/
	u16		pkt_cnt_all; /*CCK + OFDM + HT, VHT, HE*/
	u16		gi_ltf_cnt[RTW_GILTF_MAX];
	u16		pkt_cnt_cck;
	u16		pkt_cnt_ofdm; /*L-OFDM*/
	u16		pkt_cnt_t; /*HT, VHT, HE = pkt_cnt_1ss + pkt_cnt_2ss*/
	u16		pkt_cnt_1ss; /*HT, VHT, HE*/
	u16		pkt_cnt_2ss; /*HT, VHT, HE*/
	/*Legacy*/
	u16		pkt_cnt_legacy[LEGACY_RATE_NUM];
	/*HT*/
	u16		pkt_cnt_ht[HT_RATE_NUM];
	u16		pkt_cnt_sc20[LOW_BW_RATE_NUM]; /*@20M SC*/
	bool		ht_pkt_not_zero;
	bool		sc20_occur;
	/*VHT*/
	u16		pkt_cnt_vht[VHT_RATE_NUM];
	u16		pkt_cnt_sc40[LOW_BW_RATE_NUM]; /*@40M SC*/
	bool		vht_pkt_not_zero;
	bool		sc40_occur;
	/*HE*/
	u16		pkt_cnt_he[HE_RATE_NUM];
	u16		pkt_cnt_sc80[LOW_BW_RATE_NUM]; /*@80M SC*/
	bool		he_pkt_not_zero;
	bool		sc80_occur;
	/*EHT*/
	u16		pkt_cnt_eht[EHT_RATE_NUM];
	bool		eht_pkt_not_zero;
	/*non_data packet*/
	u16		pkt_cnt_legacy_non_data[LEGACY_RATE_NUM];
	u16		pkt_cnt_else_non_data;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_get_rx_pkt_cnt_rpt_su(struct bb_info *bb, struct bb_pkt_cnt_su_info *pkt_cnt_rpt, enum phl_phy_idx phy_idx);

#endif
