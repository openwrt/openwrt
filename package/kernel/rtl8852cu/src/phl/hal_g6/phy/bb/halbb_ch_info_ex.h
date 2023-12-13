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
#ifndef __HALBB_CH_INFO_EX_H__
#define __HALBB_CH_INFO_EX_H__

/*@--------------------------[Define] ---------------------------------------*/
#define CH_DESI_OPT_NUM 4 /*CH info decimation number 1/2/4/16 */
#define CH_INFO_BW_NUM 4 /*20/40/80/160*/
#define CH_INFO_RXD_LEN 8 /*len of a ch_info */

/*@--------------------------[Enum]------------------------------------------*/
enum bb_ch_info_en_t {
	CH_INFO_DISABLE = 0,
	CH_INFO_FROM_PHY_STS = BIT0, /*Report CH info form phy-status*/
	CH_INFO_FROM_CH_STS = BIT1 /*Report CH info form CH_info-status*/
};

enum bb_ch_info_t {
	BB_CH_INFO_SUCCESS = 0, /*Get ch_info segment success*/
	BB_CH_INFO_LAST_SEG, /*Get ch_info segment success, and which is the last segment*/
	BB_CH_INFO_FAIL, /*Get ch_info segment fail*/
};

enum bb_ch_mode_t {
	BB_CH_LEGACY_CH = 0,
	BB_CH_MIMO_CH = 1
};

/*@--------------------------[Structure]-------------------------------------*/

struct bb_ch_rpt_size_info {
	u8 data_byte; /* 1&2 Byte, 1Byte:S(8,4), 2Byte:S(16,12)*/
	u8 n_c; /*column of channel matrix, RX path number*/
	u8 n_r; /*row of channel matrix, TX path number*/
	u16 ch_info_rpt_len[CH_INFO_BW_NUM]; /*20,40,80; Unit: Byte; ch_info rpt length calculated by current tone decimatin configuration*/
	u16 ch_info_rpt_len_he[CH_INFO_BW_NUM]; /*HE 20,40,80; Byte; ch_info rpt length calculated by current tone decimatin configuration*/
	u16 ch_info_tone_num_lgcy[CH_INFO_BW_NUM]; /*20,40,80;*/
	u16 ch_info_tone_num[CH_INFO_BW_NUM]; /*20,40,80; Byte; current tone decimatin configuration*/
	u16 ch_info_tone_num_he[CH_INFO_BW_NUM]; /*HE 20,40,80; Byte; current tone decimatin configuration*/
	u16 per_tone_ch_rpt_size; /*Per tone ch_info size*/
	u16 tone_num_lgcy[CH_INFO_BW_NUM][CH_DESI_OPT_NUM];
	u16 tone_num[CH_INFO_BW_NUM][CH_DESI_OPT_NUM]; /*look up table for tone number under different tone decimatin CR configuration*/
	u16 tone_num_he[CH_INFO_BW_NUM][CH_DESI_OPT_NUM];/*look up table for HE tone number under different tone decimatin CR configuration*/
	bool valid_size_physts_lgcy[CH_INFO_BW_NUM]; /*<1024 Byte*/
	bool valid_size_physts_ht[CH_INFO_BW_NUM]; /*<1024 Byte*/
	bool valid_size_physts_vht[CH_INFO_BW_NUM]; /*<1024 Byte*/
	bool valid_size_physts_he[CH_INFO_BW_NUM]; /*<1024 Byte*/
};

struct bb_ch_info_cr_cfg_info {
	bool	ch_i_data_src; /*0~1: CH-estimation, CH-smoothing*/
	bool	ch_i_cmprs; /*0~1: 8/16 bit, 0:S(8,4), 1:S(16,12)*/
	u8	ch_i_grp_num; /*[Lgcy/HT/VHT] 0~3: decimation to 1/1, 1/2, 1/4, 1/16*/
	u8	ch_i_grp_num_he; /*[HE]0~3: decimation to 1/1, 1/2, 1/4, 1/16*/
	u8	ch_i_blk_start_idx; /*1~10*/
	u8	ch_i_blk_end_idx; /*1~10*/
	u32	ch_i_ele_bitmap; /*Channel matrix size, ex: 0x303:2X2, 0x1: 1X1*/
	enum bb_ch_mode_t ch_i_type; /*0~1: Legacy-CH, MIMO-CH*/
	u8	ch_i_seg_len; /*0~3: 12/28/60/124 (8byte)*/
};

struct bb_ch_info_buf_cfg_info {
	u8	ch_i_blk_start_idx; /*1~10*/
	u8	ch_i_blk_end_idx; /*1~10*/
	u8	ch_i_seg_len; /*0~3: 12/28/60/124 (8byte)*/
};

struct bb_ch_rpt_hdr_info {
	u16 total_len_l; /*header(16byte) + Raw data length(Unit: byte)*/
	#if (PLATFOM_IS_LITTLE_ENDIAN)
	u8 total_len_m:1;
	u8 total_seg_num:7;
	#else
	u8 total_seg_num:7;
	u8 total_len_m:1;
	#endif

	u8 avg_noise_pow; /*U(8,1)*/
	#if (PLATFOM_IS_LITTLE_ENDIAN)
	u8 is_pkt_end:1;
	u8 set_valid:1;
	u8 n_rx:3;
	u8 n_sts:3;
	#else
	u8 n_sts:3;
	u8 n_rx:3;
	u8 set_valid:1;
	u8 is_pkt_end:1;
	#endif
	u8 segment_size; /*unit (8Byte)*/
	u8 sts0_evm;
	u8 seq_num;
};

struct bb_phy_info_rpt {
	u8	rssi[2];
	u16	rsvd_0;
	u8	rssi_avg;
	#if (PLATFOM_IS_LITTLE_ENDIAN)
	u8	rxsc:4;
	u8	sts1_evm_l:4;
	u8	sts1_evm_m:4; /*(sts1_evm_m << 4 | sts1_evm_l): U(8,2)*/
	u8	rsvd_1:4;
	#else
	u8	rsvd_1:4;
	u8	sts1_evm_m:4;
	u8	sts1_evm_l:4;
	u8	rxsc:4;
	#endif
	u8	rsvd_2;
};

struct bb_ch_info_drv_rpt {
	u32 raw_data_len;
	u8 seg_idx_curr;
	bool get_ch_rpt_success;
};

enum bb_ch_info_event_type {
	CH_RPT_TRIG_ONCE = 0,
	CH_RPT_TRIG_ALWAYS= 1
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
bool halbb_ch_info_wait_from_physts(struct bb_info *bb, u32 dly, u32 dly_max,
				    u16 bitmap, bool valid_rpt_only);
void halbb_ch_info_cfg_mu_buff_cr(struct bb_info *bb, bool en);
bool halbb_cfg_ch_info_cr(struct bb_info *bb, struct bb_ch_info_cr_cfg_info *cfg);
void halbb_ch_info_size_query(struct bb_info *bb, struct bb_ch_rpt_size_info *exp_rpt_size, enum phl_phy_idx phy_idx);
void halbb_ch_info_physts_en(struct bb_info *bb, bool en,
			     u16 bitmap, enum phl_phy_idx phy_idx);
void halbb_ch_info_status_en(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx);
void halbb_ch_trig_select(struct bb_info *bb, u8 event);
void halbb_ch_info_close_powersaving(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx);
u8 halbb_ch_info_ack_verify(struct bb_info *bb, u16 *addr, u8 datasize, u16 len);

enum bb_ch_info_t halbb_ch_info_parsing(struct bb_info *bb, u8 *addr, u32 len,
					struct physts_rxd *desc,
					u8 *rpt_buf,
					struct bb_ch_rpt_hdr_info *hdr,
					struct bb_phy_info_rpt *phy_info,
					struct bb_ch_info_drv_rpt *drv);

#endif
