/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef _HALBB_PLCP_TX_L_ENDIAN_H_
#define _HALBB_PLCP_TX_L_ENDIAN_H_
/*@--------------------------[Define] ---------------------------------------*/
#define DL_STA_LIST_MAX_NUM    8


/*@--------------------------[Enum]------------------------------------------*/



/*@--------------------------[Structure]-------------------------------------*/
#if 0
struct cr_address_t {
	u8 address_0;
	u8 address_1;
	u8 address_2;
	u8 address_3;

	u8 bitmask_0;
	u8 bitmask_1;
	u8 bitmask_2;
	u8 bitmask_3;
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
	u8 dropping_flag: 1; //0
	u8 txbf: 1;
	u8 coding: 1;
	u8 nsts: 3;
	u8 ps160: 1;
	u8 rsvd0: 1;

	u8 mac_id;
	u8 ru_position;

	u8 vip_flag: 1; //dont care
	u8 pwr_boost_factor: 5; //dont care
	u8 rsvd1: 2;
	// DW1
	u8 tx_length_0;
	u8 tx_length_1;
	u8 tx_length_2;
	u8 tx_length_3;
	// DW2
	struct ru_rate_entry ru_rate;
	struct rura_report ru_ra_report;
	u8 aid12_0;

	u8 aid12_1: 3;
	u8 rsvd2: 5;
};

struct dl_rua_output {
	// DW0
	u8 ru2su_flag: 1;
	u8 ppdu_bw: 2;  //set
	u8 group_tx_pwr_0: 5;

	u8 group_tx_pwr_1: 4;
	u8 stbc: 1;
	u8 gi_ltf: 3;

	u8 doppler: 1;
	u8 n_ltf_and_ma: 3;
	u8 sta_list_num: 4; //set

	u8 grp_mode: 1;
	u8 rsvd0: 6;
	u8 fixed_mode: 1; //set 1
	// DW1
	u8 group_id;

	u8 ch20_with_data_0;

	u8 ch20_with_data_1;

	u8 pri_txsb: 5;
	u8 ru_grp_ntx: 3;
	// DW2 (new added)
	u8 ul_dl: 2;
	u8 ppdu_type_comp_mode: 2;
	u8 usig_spat_reuse: 4;

	u8 usig_ltf_symb: 3;
	u8 usig_nss: 4;
	u8 usig_bf: 1;

	u8 usig_spat_gi_ltf: 2;
	u8 usig_disregard_ndp: 2;
	u8 usig_ldpc_extra_symb_seg: 1;
	u8 usig_prefec: 2;
	u8 usig_pe_disambiguity: 1;

	u8 usig_disregard: 4;
	u8 rsvd1: 4;
	// DW3
	struct dl_ru_output_sta_entry  dl_output_sta_list[8];
};

//sig output
struct sigb_compute_output {
	// DW0
	u8 sta_0_idx: 3;
	u8 sta_1_idx: 3;
	u8 sta_2_idx_0: 2;

	u8 sta_2_idx_1: 1;
	u8 sta_3_idx: 3;
	u8 sta_4_idx: 3;
	u8 sta_5_idx_0: 1;

	u8 sta_5_idx_1: 2;
	u8 sta_6_idx: 3;
	u8 sta_7_idx: 3;

	u8 rsvd0;
	// DW1
	u8 hw_sigb_content_channelone_len;

	u8 hw_sigb_content_channeltwo_len;

	u8 hw_sigb_symbolnum: 6;
	u8 hw_sigb_content_channeltwo_offset_0: 2; //have to +1

	u8 hw_sigb_content_channeltwo_offset_1: 3; //have to +1
	u8 ru2su_flag: 1;
	u8 sigb_dcm: 1;
	u8 sigb_mcs: 3;
	// DW2 (new added)
	u8 HW_EHTSIG_1st80_content_channelone_len_0;

	u8 HW_EHTSIG_1st80_content_channelone_len_1: 2;
	u8 HW_EHTSIG_1st80_content_channeltwo_len_0: 6;

	u8 HW_EHTSIG_1st80_content_channeltwo_len_1: 4;
	u8 rsvd1: 4;

	u8 rsvd2;
	// DW3 (new added)
	u8 HW_EHTSIG_2nd80_content_channelone_len_0;

	u8 HW_EHTSIG_2nd80_content_channelone_len_1: 2;
	u8 HW_EHTSIG_2nd80_content_channeltwo_len_0: 6;

	u8 HW_EHTSIG_2nd80_content_channeltwo_len_1: 4;
	u8 rsvd3: 4;

	u8 rsvd4;
	// DW4 (new added)
	u8 HW_EHTSIG_3rd80_content_channelone_len_0;

	u8 HW_EHTSIG_3rd80_content_channelone_len_1: 2;
	u8 HW_EHTSIG_3rd80_content_channeltwo_len_0: 6;

	u8 HW_EHTSIG_3rd80_content_channeltwo_len_1: 4;
	u8 rsvd5: 4;

	u8 rsvd6;
	// DW5 (new added)
	u8 HW_EHTSIG_4th80_content_channelone_len_0;

	u8 HW_EHTSIG_4th80_content_channelone_len_1: 2;
	u8 HW_EHTSIG_4th80_content_channeltwo_len_0: 6;

	u8 HW_EHTSIG_4th80_content_channeltwo_len_1: 4;
	u8 rsvd7: 4;

	u8 rsvd8;
};

struct bb_h2c_sig_info {
	u8 force_sigb_rate;
	u8 force_sigb_mcs;
	u8 force_sigb_dcm;
	u8 rsvd;
	struct dl_rua_output dl_rua_out;
	struct sigb_compute_output  sigb_output;
	struct cr_address_t n_sym_hesigb_ehtsig[156];
};

#else

struct cr_address_t {
	u8 address_0;
	u8 address_1;
	u8 address_2;
	u8 address_3;
};
// EHT SIG & SIGB
struct ehtsig_sigb_usr_info {
	u8 coding;
	u8 nsts;
	u8 ru_position;
	u8 dcm;

	u8 mcs;
	u8 ps160;
	u8 rsvd0;
	u8 rsvd1;
};

struct ehtsig_sigb_info {
	u8 ppdu_bw;
	u8 sta_list_num;
	u8 ch20_with_data_0;
	u8 ch20_with_data_1;

	u8 pri_txsb;
	u8 ul_dl;// 2 bit
	u8 ppdu_type_comp_mode;// 2 bit
	u8 usig_spat_reuse;// 4 bit

	u8 usig_ltf_symb;// 3 bit
	u8 usig_nss;// 4 bit
	u8 usig_bf;// 1 bit
	u8 usig_spat_gi_ltf;// 2 bit

	u8 usig_disregard_ndp;// 2 bit
	u8 usig_ldpc_extra_symb_seg;// 1 bit
	u8 usig_prefec;// 2 bit
	u8 usig_pe_disambiguity;// 1 bit

	u8 usig_disregard;// 4 bit
	u8 rsvd0;
	u8 rsvd1;
	u8 rsvd2;

	struct ehtsig_sigb_usr_info usr_info[DL_STA_LIST_MAX_NUM];
};

struct bb_h2c_ehtsig_sigb {
	u8 ehtsig_sigb; // (True): EHT-SIG / (False): SIG-B
	u8 ehtsig_sigb_mcs;
	u8 rsvd2;
	u8 rsvd3;
	struct ehtsig_sigb_info ehtsig_sigb_i;
	struct cr_address_t ehtsig_sigb_cr[80];
	struct cr_address_t n_sym_sig_ch1[16];
	struct cr_address_t n_sym_sig_ch2[16];
};
/*@--------------------------[Prptotype]-------------------------------------*/
#endif
#endif
