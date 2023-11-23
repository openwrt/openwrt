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

#ifndef _HALBB_OUTSRC_DEF_H_
#define _HALBB_OUTSRC_DEF_H_
/*  ============================================================
 			   define
    ============================================================
*/
#define N_USER	4
/*  ============================================================
 			Enumeration
    ============================================================
 */
enum mlo0_mode {
	MLO0_0_PATH  = 0x0,
	MLO0_1_PATH  = 0x1,
	MLO0_2_PATH  = 0x2,
	MLO0_3_PATH  = 0x3,
};

enum mlo1_mode {
	MLO1_0_PATH  = 0x0,
	MLO1_1_PATH  = 0x10,
	MLO1_2_PATH  = 0x20,
	MLO1_3_PATH  = 0x30,
};

enum bb_mlo_mode_info {
	MLO_0_PLUS_2  = (MLO1_0_PATH | MLO0_2_PATH),
	MLO_1_PLUS_1  = (MLO1_1_PATH | MLO0_1_PATH),
	MLO_2_PLUS_0  = (MLO1_2_PATH | MLO0_0_PATH),
	MLO_2_PLUS_2  = (MLO1_2_PATH | MLO0_2_PATH)
};

enum halbb_dbcc_mode_type {
	BB_DBCC_NOT_SUPPORT   = 0,
	BB_DBCC_MLO_0_PLUS_2  = (MLO1_0_PATH | MLO0_2_PATH),
	BB_DBCC_MLO_1_PLUS_1  = (MLO1_1_PATH | MLO0_1_PATH),
	BB_DBCC_MLO_2_PLUS_0  = (MLO1_2_PATH | MLO0_0_PATH),
	BB_DBCC_MLO_2_PLUS_2  = (MLO1_2_PATH | MLO0_2_PATH),
	BB_DBCC_SINGLE_MAC    = 0xff
};

enum plcp_sts {
	PLCP_SUCCESS = 0,
	LENGTH_EXCEED,
	CCK_INVALID,
	OFDM_INVALID,
	HT_INVALID,
	VHT_INVALID,
	HE_INVALID,
	EHT_INVALID,
	SPEC_INVALID
};

enum plcp_dbw {
	DBW20 = 0,
	DBW40,
	DBW80,
	DBW160,
	DBW320
};

enum ru_sizes_list {
	RU26 		= 0,
	RU52,
	RU106,
	RU242,
	RU484,
	RU996,
	RU996X2,
	HESIGB,
	RU996X4,
	RU52_26,
	RU106_26,
	RU484_242,
	RU996_484,
	RU996_484_242,
	RU996X2_484,
	RU996X3,
	RU996X3_484,
	RU_SIZE_NUM
};

enum packet_format_t{
	B_MODE_FMT 	= 0, 
	LEGACY_FMT,
	HT_MF_FMT,
	HT_GF_FMT,
	VHT_FMT,
	HE_SU_FMT,
	HE_ER_SU_FMT,
	HE_MU_FMT,
	HE_TB_FMT,
	EHT_MU_SU_FMT,
	EHT_MU_MU_FMT, // MU-MIMO
	EHT_MU_RU_FMT, // DL MU-OFDMA
	EHT_MU_ERSU_FMT,
	EHT_TB_FMT
};
/*  ============================================================
 			   structure
    ============================================================
*/
#if 0
struct cr_address_t {
	u32 address;
	u32 bitmask;
};

struct ru_rate_entry {
	u8 dcm: 1;
	u8 ss: 3;
	u8 mcs: 4;
};

struct rura_report {
	u8 rate_table_col_idx: 6;
	u8 partial_allocation_flag: 1;
	u8 rate_change_flag: 1;
};

struct dl_ru_output_sta_entry {
	// DW0
	u32 dropping_flag: 1; //0
	u32 txbf: 1;
	u32 coding: 1;
	u32 nsts: 3;
	u32 ps160: 1;
	u32 rsvd0: 1;

	u32 mac_id: 8;
	u32 ru_position: 8;

	u32 vip_flag: 1; //dont care
	u32 pwr_boost_factor: 5; //dont care
	u32 rsvd1: 2;
	// DW1
	u32 tx_length;
	// DW2
	struct ru_rate_entry ru_rate;
	struct rura_report ru_ra_report;
	u32 aid12: 11;
	u32 rsvd2: 5;
};

struct dl_rua_output {
	// DW0
	u32 ru2su_flag: 1;
	u32 ppdu_bw: 2;  //set
	u32 group_tx_pwr: 9;
	u32 stbc: 1;
	u32 gi_ltf: 3;

	u32 doppler: 1;
	u32 n_ltf_and_ma: 3;
	u32 sta_list_num: 4; //set

	u32 grp_mode: 1;
	u32 rsvd0: 6;
	u32 fixed_mode: 1; //set 1
	// DW1
	u32 group_id: 8;
	u32 ch20_with_data: 16;
	u32 pri_txsb: 5;
	u32 ru_grp_ntx: 3;
	// DW2 (new added)
	u32 ul_dl: 2;
	u32 ppdu_type_comp_mode: 2;
	u32 usig_spat_reuse: 4;
	u32 usig_spat_gi_ltf: 2;
	u32 usig_ltf_symb: 3;
	u32 usig_nss: 4;
	u32 usig_bf: 1;
	u32 usig_disregard_ndp: 2;
	u32 usig_ldpc_extra_symb_seg: 1;
	u32 usig_prefec: 2;
	u32 usig_pe_disambiguity: 1;
	u32 usig_disregard: 4;
	u32 rsvd1: 4;
	// DW3
	struct dl_ru_output_sta_entry  dl_output_sta_list[DL_STA_LIST_MAX_NUM];
};

//sig output
struct sigb_compute_output {
	// DW0
	u32 sta_0_idx: 3;
	u32 sta_1_idx: 3;
	u32 sta_2_idx: 3;
	u32 sta_3_idx: 3;
	u32 sta_4_idx: 3;
	u32 sta_5_idx: 3;
	u32 sta_6_idx: 3;
	u32 sta_7_idx: 3;
	u32 rsvd: 8;
	// DW1
	u32 hw_sigb_content_channelone_len: 8;
	u32 hw_sigb_content_channeltwo_len: 8;
	u32 hw_sigb_symbolnum: 6;
	u32 hw_sigb_content_channeltwo_offset: 5; //have to +1
	u32 ru2su_flag: 1;
	u32 sigb_dcm: 1;
	u32 sigb_mcs: 3;
	// DW2 (new added)
	u32 HW_EHTSIG_1st80_content_channelone_len: 10;
	u32 HW_EHTSIG_1st80_content_channeltwo_len: 10;
	u32 rsvd1: 12;
	// DW3 (new added)
	u32 HW_EHTSIG_2nd80_content_channelone_len: 10;
	u32 HW_EHTSIG_2nd80_content_channeltwo_len: 10;
	u32 rsvd2: 12;
	// DW4 (new added)
	u32 HW_EHTSIG_3rd80_content_channelone_len: 10;
	u32 HW_EHTSIG_3rd80_content_channeltwo_len: 10;
	u32 rsvd3: 12;
	// DW5 (new added)
	u32 HW_EHTSIG_4th80_content_channelone_len: 10;
	u32 HW_EHTSIG_4th80_content_channeltwo_len: 10;
	u32 rsvd4: 12;
};

struct bb_h2c_sig_info {
	u8 force_sigb_rate;
	u8 force_sigb_mcs;
	u8 force_sigb_dcm;
	u8 rsvd;
	struct dl_rua_output dl_rua_out;
	struct sigb_compute_output  sigb_output;
	struct cr_address_t n_sym_sigb_ch1[16];
	struct cr_address_t n_sym_sigb_ch2[16];
};
#endif

//========== [Outer-Input] ==========//
struct usr_plcp_gen_in {
	u32 mcs : 6;
	u32 mpdu_len : 14;
	u32 n_mpdu : 9;
	u32 fec : 1;
	u32 dcm : 1;
	u32 rsvd0 : 1;
	u32 aid : 12;
	u32 scrambler_seed : 8; // rand (1~255)
	u32 random_init_seed : 8; // rand (1~255)
	u32 rsvd1 : 4;
	u32 apep : 22;
	u32 ru_alloc : 8;
	u32 rsvd2 : 2;
	u32 nss : 4;
	u32 txbf : 1;
	u32 pwr_boost_db : 5;
	u32 ru_size : 8;
	u32 ru_idx : 8;
	u32 rsvd3 : 6;
};

struct halbb_plcp_info {
	u32 source_gen_mode : 2;
	u32 locked_clk : 1;
	u32 dyn_bw : 1;
	u32 ndp_en : 1;
	u32 long_preamble_en : 1; //bmode
	u32 stbc : 1;
	u32 gi : 2; //0:0.4,1:0.8,2:1.6,3:3.2
	u32 tb_l_len : 12;
	u32 tb_ru_tot_sts_max : 3;
	u32 vht_txop_not_allowed : 1;
	u32 tb_disam : 1;
	u32 doppler : 2;
	u32 he_ltf_type : 2;//0:1x,1:2x,2:4x
	u32 rsvd1 : 2;

	u32 ht_l_len : 12;
	u32 preamble_puncture : 1;
	u32 he_mcs_sigb : 3;//0~5
	u32 he_dcm_sigb : 1;
	u32 he_sigb_compress_en : 1;
	u32 max_tx_time_0p4us : 14;

	u32 ul_flag : 1;
	u32 tb_ldpc_extra : 1;
	u32 bss_color : 6;
	u32 sr : 4;
	u32 beamchange_en : 1;
	u32 he_er_u106ru_en : 1;
	u32 ul_srp1 : 4;
	u32 ul_srp2 : 4;
	u32 ul_srp3 : 4;
	u32 ul_srp4 : 4;
	u32 mode : 2;

	u32 group_id : 6;	
	u32 ppdu_type : 4;//0: bmode,1:Legacy,2:HT_MF,3:HT_GF,4:VHT,5:HE_SU,6:HE_ER_SU,7:HE_MU,8:HE_TB
	u32 txop : 7;
	u32 tb_strt_sts : 3;
	u32 tb_pre_fec_padding_factor : 2;
	u32 cbw : 2;
	u32 txsc : 4;
	u32 tb_mumimo_mode_en : 1;
	u32 dbw : 3; //0:BW20, 1:BW40, 2:BW80, 3:BW160/BW80+80, 4:BW320

	u32 nominal_t_pe : 2; // def = 2
	u32 ness : 2; // def = 0
	u32 rsvd2 : 4;
	u32 n_user : 8;
	u32 tb_rsvd : 9;//def = 0
	u32 rsvd3 : 7;

	u32 punc_pattern : 8;
	u32 eht_mcs_sig : 8; // [TBD] Export
	u32 txsb : 8; // [TBD] Export
	u32 rsvd4 : 8;

	struct usr_plcp_gen_in usr[N_USER];
};

/*  ============================================================
 		     Function Prototype
    ============================================================
*/

struct bb_info;

enum plcp_sts halbb_plcp_gen(struct bb_info *bb, struct halbb_plcp_info *in,
		    struct usr_plcp_gen_in *user, enum phl_phy_idx phy_idx);

#endif
