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
#ifndef __HALBB_CH_INFO_H__
#define __HALBB_CH_INFO_H__

#ifdef HALBB_CH_INFO_SUPPORT
/*@--------------------------[Define] ---------------------------------------*/
#define TEST_CH_INFO_MAX_SEG	4
#define TEST_CH_SEG_LEN	(12 * 8) /*Byte*/
/*@--------------------------[Enum]------------------------------------------*/
enum bb_ch_info_state_type {
	CH_RPT_START_TO_WAIT = 0,
	CH_RPT_GETTED = 1,
	CH_RPT_ALWAYS_ON = 2
};

enum bb_ch_info_ack_rxsc_t{
	RXSC_FULLBW = 0,
	RXSC_20 = 1,
	RXSC_DUPILCATE_40 = 9,
	RXSC_DUPILCATE_80 = 13
};

/*@--------------------------[Structure]-------------------------------------*/
struct bb_ch_info_physts_info {
	enum bb_ch_info_state_type ch_info_state;
	bool get_ch_rpt_success;
	u16 force_bitmap_type; /*force setting*/
	u16 bitmap_type_rpt; /*report*/
	bool valid_ch_info_only_en;
	bool filter_rxsc_en;
	u8 filter_rxsc_tgrt_idx;
	bool bitmap_type_auto_en;
	u8 rxsc;
	u8 n_rx;
	u8 n_sts;
	u16 ch_info_len;
	u8 evm_1_sts;
	u8 evm_2_sts;
	u8 avg_idle_noise_pwr;
	bool is_ch_info_len_valid;
	enum rtw_gi_ltf gi_ltf;
	u16 data_rate;
	enum bb_ch_info_event_type ch_info_event;
};

struct bb_ch_info_raw_info {
	s16 *octet;
	u32 ch_info_buf_len; /*Byte*/
};

struct bb_ch_info_cr_info {
	u32 ch_info_en_0;
	u32 ele_bitmap;
	u32 ch_info_type;
	u32 ch_info_type_m;
	u32 seg_len;
	u32 seg_len_m;
	u32 ch_info_off_powersaving;
};

struct bb_ch_rpt_info {
	u8 seg_idx_pre;
	u32 csi_raw_data_total_len;  /*Raw data length(Unit: byte) = total_len - 16*/
	u32 total_len_remnant;
	u16 ch_rpt_hdr_len;
	u16 phy_info_len;
	bool skip_ch_info; /*wait for seg_0*/
	u32 raw_data_len_acc;
	u8 *test_buf;
	u8 *test_buf_curr;
	bool print_en;
	enum bb_ch_info_en_t ch_info_data_mode;
	struct bb_ch_info_cr_cfg_info bb_ch_info_cr_cfg_i;
	struct bb_ch_info_cr_cfg_info bb_ch_info_cur_cr_cfg_i;
	struct bb_ch_info_cr_info bb_ch_info_cr_i; /*CR callback table*/
	struct bb_ch_rpt_size_info bb_ch_rpt_size_i;
	struct bb_ch_info_raw_info bb_ch_info_raw_i; /*save dumped ch-info data*/
	struct bb_ch_info_physts_info bb_ch_info_physts_i;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
bool halbb_ch_info_valid_chk_8852a(struct bb_info *bb, struct physts_rxd *desc);
void halbb_ch_info_buf_rls(struct bb_info *bb);
bool halbb_ch_info_buf_alloc(struct bb_info *bb);
void halbb_ch_info_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		       char *output, u32 *_out_len);
void halbb_ch_info_deinit(struct bb_info *bb);
void halbb_ch_info_init(struct bb_info *bb);
void halbb_cr_cfg_ch_info_init(struct bb_info *bb);
void halbb_ch_info_modify_ack_rxsc(struct bb_info *bb, struct physts_rxd *desc, u32 csi_raw_data_total_len,  u8 *rxsc);
#endif
#endif