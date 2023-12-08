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

#ifndef _HALBB_PLCP_GEN_H_
#define _HALBB_PLCP_GEN_H_

/*  ============================================================
 			   define
    ============================================================
*/
#define N_USER	4

/*  ============================================================
 			   structure
    ============================================================
*/


struct plcp_mcs_table_in_t {
	u8 spec_idx : 3;
	u8 mcs : 5;
	u8 nss : 4;		
	u8 bw : 3;//0:BW20, 1:BW40, 2:BW80, 3:BW160 4:BW320 /*enum channel_width*/
	u8 rsvd0 : 1;
	u8 ru_size : 5; //0:RU26, 1:RU52, 2:RU106, 3:RU242, 4:RU484, 5:RU996, 6:RU996x2, 7:hesigb, 8:RU996x4, 9:RU52_26, 10:RU106_26, 11:RU484_242, 12:RU996_484, ..., 16:RU996X3_484
	u8 rsvd1 : 3;
	bool dcm;
	bool fec;
};

struct plcp_mcs_table_out_t {
	u32 n_dbps : 19;
	u32 rsvd0 : 13;
	u32 he_n_dbps_short : 17;
	u32 rsvd1 : 15;
	u32 n_cbps : 19;
	u32 rsvd2 : 13;
	u32 n_es : 4;
	u32 valid : 1;
	u32 code_rate : 2;
	u32 nss : 3;
	u32 he_n_cbps_short : 17;
	u32 rsvd3 : 4;
	bool dcm;
	bool fec;
};
//========== [Par] ==========//
struct com_pre_fec_par {
	u16 n_sym_init : 11;
	u16 spec_idx : 3;
	u16 pre_fec_padding_factor_init : 3;
	u16 ndp_en : 1;
	u16 preamble_0p4us : 16;
	u8 m_stbc : 2;
	u8 stbc : 1;
	u8 doppler_mode : 2;
	u8 gi : 2;
	u32 t_sym_0p4us : 6;
	u32 t_ltf_sym_0p4us : 6;
	u32 n_ltf_sym : 4;
	u32 n_sts_max : 4;
	u32 n_ma : 6;
	u32 m_ma : 5;
	u32 tb_trig : 1;
	u32 n_hesigb_sym : 8;	
	u32 n_usr_refine : 8;
	u32 tb_trig_t_pe : 3;
	u32 tb_ldpc_extra : 1;
	u32 rsvd1 : 12;
};

struct usr_pre_fec_par {
	u32 n_excess : 15;
	u32 pre_fec_padding_factor_init : 3;
	u32 n_sym_init : 11;
	u32 rsvd0 : 3;
	u32 n_dbps_last_init : 17;
	u32 n_mpdu_refine : 10;
	u32 rsvd1 : 5;
	u32 n_cbps_last_init : 18;
	u32 mpdu_length_byte_refine : 14;
	u32 apep_refine : 22;
	u32 ru_size_refine : 5;
	u32 rsvd3 : 5;
};

struct plcp_tx_pre_fec_padding_setting_par_t {
	struct com_pre_fec_par com;
	struct plcp_mcs_table_out_t usr_mcs_out[4];
	struct usr_pre_fec_par usr[N_USER];
};
//========== [Input] ==========//
struct usr_pre_fec_in {
	u8 ru_size_idx : 5;
	u8 rsvd0 : 3;
	u8 nss : 4;
	u8 rsvd1 : 4;
	u8 mcs : 6;
	u8 rsvd2 : 2;
	u32 apep : 22;
	u32 n_mpdu : 10;
	u16 mpdu_length_byte : 14;
	u16 rsvd4 : 2;
	bool dcm;
	bool fec;
};

struct plcp_tx_pre_fec_padding_setting_in_t {
	u8 format_idx : 4;
	u8 stbc : 1;
	u8 he_dcm_sigb : 1;
	u8 doppler_mode : 2; //0: diable ,1:MA10, 2:MA20
	u16 n_hesigb_sym : 11; // Shared para. with n_ehtsig_sym
	u16 he_mcs_sigb : 3;
	u16 nominal_t_pe : 2;

	u8 dbw : 3; //0:BW20, 1:BW40, 2:BW80, 3:BW160 4:BW320 /*enum channel_width*/
	u8 rsvd0 : 5;

	u16 gi : 2; //0.4,0.8,1.6,3.2
	u16 ltf_type : 2; //1x, 2x, 4x
	u16 ness : 2;
	u16 rsvd1 : 10;

	u32 mode_idx : 2; //0:apep, 1:max_tx_time, 2:n_mpdu,mpdu_len, 3:tb_trigger_mode
	u32 max_tx_time_0p4us : 14;
	u32 n_user : 8;

	u32 ndp : 1; 
	u32 he_er_u106ru_en : 1; //done
	u32 rsvd2 : 6;
	u32 tb_l_len : 12;
	u32 tb_ru_tot_sts_max : 3;
	u32 tb_disam : 1;
	u32 tb_ldpc_extra : 1;
	u32 tb_pre_fec_padding_factor : 2;
	u32 ht_l_len : 12;
	u32 rsvd3 : 10;
	struct usr_pre_fec_in usr[N_USER];
};


//========== [Output] ==========//
struct usr_pre_fec_out {
	u32 nss : 4;
	u32 nsts : 4;
	u32 mpdu_length_byte : 14;
	u32 n_mpdu : 10;
	u32 eof_padding_length : 32;
	u32 apep_len : 22;
	u32 ru_size : 8;
	u32 mcs_valid : 1;
	u32 rsvd1 : 1;

	u32 ru_idx : 8;
	u32 fec : 1;
	u32 dcm : 1;
	u32 rsvd2 : 22;
};
struct plcp_tx_pre_fec_padding_setting_out_t {
	u32 pre_fec_padding_factor : 2; // 0:4, 1:1, 2:2, 3:3
	u32 n_sym : 11;
	u32 ldpc_extra : 1;
	u32 n_sym_ehtsig : 5;
	u32 punc_ch_info : 5;
	u32 ppdu_type_comp_mode : 2;
	u32 rsvd : 2;
	u32 t_pe : 3; //0: 0us, 1:4us, 2:8us, 3:12us, 4:16us, 5:20us
	u32 valid : 1;
	u16 l_len : 12;
	u16 disamb : 1;
	u16 n_ltf : 3;
	u32 tx_time_0p4us;
	u32 stbc : 1;
	u32 doppler_en : 1;
	u32 midamble : 2;
	u32 n_usr : 8;
	u32 ndp : 1;
	u32 gi : 2;
	u32 n_sym_hesigb : 6;
	u32 plcp_valid : 8;
	u32 rvsd0 : 3;
	struct usr_pre_fec_out usr[N_USER];
};

/*  ============================================================
 			Enumeration
    ============================================================
*/
enum spec_list {
	SPEC_B_MODE	= 0,
	SPEC_LEGACY	= 1,
	SPEC_HT		= 2,
	SPEC_VHT	= 3,
	SPEC_HE		= 4,
	SPEC_EHT	= 5
};

enum fec_t {
	BCC 		= 0,
	LDPC
};

enum coding_rate_t{
	R12 		= 0,
	R23,
	R34,
	R56
};

/*  ============================================================
 		     Function Prototype
    ============================================================
*/

struct bb_info;

u32 halbb_ceil(u32 numerator, u32 denominator);
u32 halbb_mod(u32 numerator, u32 denominator);
u32 halbb_min(u32 val_1, u32 val_2);
u32 halbb_max(u32 val_1, u32 val_2);
void halbb_set_bit(u8 strt, u8 len, u32 in, u32* out);
enum plcp_sts halbb_tx_plcp_cal(struct bb_info *bb,
	const struct plcp_tx_pre_fec_padding_setting_in_t *in,
	struct plcp_tx_pre_fec_padding_setting_out_t *out);


#endif

