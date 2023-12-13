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

#ifndef __HALBB_PHYSTS_EX_H__
#define __HALBB_PHYSTS_EX_H__

enum bb_physts_bitmap_t {
	TD_SEARCH_FAIL	= 0,
	BRK_BY_TX_PKT	= 1, /*R2T in EHT ICs*/
	CCA_SPOOF	= 2,
	OFDM_BRK	= 3,
	CCK_BRK		= 4,
	DL_MU_SPOOFING	= 5,
	HE_MU		= 6,
	VHT_MU		= 7,
	UL_TB_SPOOFING	= 8,
	RSVD_9		= 9,
	TRIG_BASE_PPDU	= 10,
	CCK_PKT		= 11,
	LEGACY_OFDM_PKT = 12,
	HT_PKT		= 13,
	VHT_PKT		= 14,
	HE_PKT		= 15,
	EHT_PKT		= 16, /*EHT ICs only*/
	PHYSTS_BITMAP_NUM
};

struct physts_rxd_user {
	u8		macid;
	u8		is_data: 1;
	u8		is_ctrl:1;
	u8		is_mgnt:1;
	u8		is_bcn:1;
	u8		rsvd_0:4;
};

struct physts_rxd {
	u8		is_su:1;
	u8		user_num:2;
	u8		is_to_self:1;
	u8		gi_ltf:4; /*enum rtw_gi_ltf*/
	u16		data_rate;
	u8		macid_su;
	//u8		ppdu_cnt;
	enum phl_phy_idx phy_idx;
	struct physts_rxd_user user_i[4];
};

struct physts_result {
	bool			physts_rpt_valid; /* @if physts_rpt_valid is false, please ignore the parsing result in this structure*/
	u8			rssi_avg;
	u8			rssi[4];	/* u(8,1) RSSI in 0~100 index */
	enum bb_physts_bitmap_t ie_map_type;
	u8			ch_idx;		/* channel number---*/
	enum band_type		band;
	enum channel_width	rx_bw;
	u8			rxsc;		/* sub-channel---*/
	u8 			n_rx;
	u8 			n_sts;
	u8			is_mu_pkt;	/* is MU packet or not---bool*/
	u8			is_bf;		/* BF packet---bool*/
	bool			is_pkt_with_data;
	u8			snr_fd_avg;	/* fd, u(8,0), OFDM,        fd_snr_avg(phy-sts), limited by FD DFIR output wordlength*/
	u8			snr_fd[4];	/* fd, u(8,0), OFDM,        fd_snr_avg(phy-sts) + td_rssi_diff[i]*/
	u8			snr_td_avg;	/* td, u(8,0), OFDM + CCK,  td_rssi_avg(phy-sts) - MA(rx_idle_pwer)*/
	u8			snr_td[4];	/* td, u(8,0), OFDM + CCK,  td_rssi[i](phy-sts)  - MA(rx_idle_pwer), limited by DIG in 52A B-cut*/
	u8			is_su ;
	u8			is_ldpc;
	u8			is_stbc;
	u8			evm_1_sts;
	u8			evm_2_sts;
	u8			avg_idle_noise_pwr;
	bool			is_ch_info_len_valid;
	s16			cfo;
	u16			ch_info_len;
	s16			*ch_info_addr;
	u8			ie8_modify_rxsc;
	bool			snif_rpt_valid;
	struct bb_snif_info	*bb_snif_i;
};

struct bb_info;

u8 halbb_drv_info_rssi_parsing(struct bb_info *bb, u16 rssi_in,
			       enum channel_width bw);

bool halbb_physts_parsing(struct bb_info *bb,
			      u8 *addr,
			      u16 physts_total_length,
			      struct physts_rxd *desc,
			      struct physts_result *bb_rpt);

#endif
