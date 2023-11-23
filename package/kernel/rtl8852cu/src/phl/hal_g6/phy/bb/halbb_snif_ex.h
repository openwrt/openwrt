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
#ifndef __HALBB_SNIF_EX_H__
#define __HALBB_SNIF_EX_H__


/*@--------------------------[Define] ---------------------------------------*/

/*@--------------------------[Enum]------------------------------------------*/

/*@--------------------------[Structure]-------------------------------------*/
/* === [Legacy] ========================*/
struct plcp_hdr_l_sig_info {
	/*B0*/
	u8 rate:4;
	u8 rsvd_bit:1;
	u8 len0_l:3;
	/*B1*/
	u8 len0_m;
	/*B2*/
	u8 len1:1;
	u8 parity_chk:1;
	u8 rsvd_0:6;
};

/* === [HT] ============================*/

struct plcp_hdr_ht_sig_1_info {
	/*B0*/
	u8 mcs:7;
	u8 cbw:1;
	/*B1*/
	u8 ht_len_l;
	/*B2*/
	u8 ht_len_m;
};

struct plcp_hdr_ht_sig_2_info {
	/*B0*/
	u8 smoothing:1;
	u8 not_sounding:1;
	u8 rsvd_bit:1;
	u8 aggre:1;
	u8 stbc:2;
	u8 fec_coding:1;
	u8 sgi:1;
	/*B1*/
	u8 num_ext_ss:2;
	u8 rsvd_0:6;
};
/* === [VHT] ===========================*/

struct plcp_hdr_vht_sig_a1_info {
	/*B0*/
	u8 bw:2;
	u8 rsvd_bit:1;
	u8 stbc:1;
	u8 g_id_l:4;
	/*B1*/
	u8 g_id_m:2;
	u8 nsts_paid_l:6;
	/*B2*/
	u8 nsts_paid_m:6;
	u8 txop_ps_not_allow:1;
	u8 rsvd_bit_1:1;
};

struct plcp_hdr_vht_sig_a1_ext_su_info {
	/*B0*/
	u8 su_sts:3;
	u8 paid_l:5;
	/*B1*/
	u8 paid_m:4;
	u8 rsvd_0:4;
};

struct plcp_hdr_vht_sig_a1_ext_mu_info {
	/*B0*/
	u8 mu_sts_0:3;
	u8 mu_sts_1:3;
	u8 mu_sts_2_l:2;
	/*B1*/
	u8 mu_sts_2_m:1;
	u8 mu_sts_3:3;
	u8 rsvd_0:4;
};

struct plcp_hdr_vht_sig_a2_info {
	/*B0*/
	u8 sgi:1;
	u8 sgi_disamb:1;
	u8 su_mu_0_coding:1;
	u8 ldpc_ext_symbol:1;
	u8 mcs_coding_l:4;
	/*B1*/
	u8 mcs_coding_m:1;
	u8 rsvd_bit:1;
	u8 rsvd_0:6;
};

struct plcp_hdr_vht_sig_a2_ext_su_info {
	/*B0*/
	u8 su_vht_mcs:4;
	u8 bf:1;
	u8 rsvd_0:3;
};

struct plcp_hdr_vht_sig_a2_ext_mu_info {
	/*B0*/
	u8 mu_coding_1:1;
	u8 mu_coding_2:1;
	u8 mu_coding_3:1;
	u8 rsvd_0:5;
};

struct plcp_hdr_vht_sig_b_info {
	u32 sigb_len_l;
	u8 vht_mcs; //mu
};

/* === [HE] ============================*/

struct plcp_hdr_he_sig_a1_su_info {
	/*B0*/
	u8 fmt:1;
	u8 beam_chg:1;
	u8 ul_dl:1;
	u8 he_mcs:4;
	u8 dcm:1;
	/*B1*/
	u8 bss_color:6;
	u8 rsvd_1:1;
	u8 spatial_reuse_l:1;
	/*B2*/
	u8 spatial_reuse_m:3;
	u8 bw:2;
	u8 gi_ltf_size:2;
	u8 nsts_mid_period_l:1;
	/*B3*/
	u8 nsts_mid_period_m:2;
	u8 rsvd_2:6;
};

struct plcp_hdr_he_sig_a2_su_info {
	/*B0*/
	u8 txop:7;
	u8 coding:1;
	/*B1*/
	u8 ldpc_ext_sym:1;
	u8 stbc:1;
	u8 bf:1;
	u8 pre_fec_fac:2;
	u8 pe_dis:1;
	u8 rsvd_1:1;
	u8 doppler:1;
};

struct plcp_hdr_he_sig_a1_mu_info {
	/*B0*/
	u8 ul_dl:1;
	u8 he_sig_b_mcs:3;
	u8 he_sig_b_dcm:1;
	u8 bss_color_l:3;
	/*B1*/
	u8 bss_color:3;
	u8 spatial_reuse:4;
	u8 bw_l:1;
	/*B2*/
	u8 bw_m:2;
	u8 num_sig_b_sym_or_mu_user:4;
	u8 sig_b_comp:1;
	u8 gi_ltf_size_l:1;
	/*B3*/
	u8 gi_ltf_size_m:1;
	u8 doppler:1;
	u8 rsvd_1:6;
};

struct plcp_hdr_he_sig_a2_mu_info {
	/*B0*/
	u8 txop:7;
	u8 rsvd_1:1;
	/*B1*/
	u8 num_sym_and_mid_period:3;
	u8 ldpc_ext_sym:1;
	u8 stbc:1;
	u8 pre_fec_fac:2;
	u8 pe_dis:1;
};

struct plcp_hdr_he_sig_a1_tb_info {
	/*B0*/
	u8 fmt:1;
	u8 bss_color:6;
	u8 spatial_reuse_1_l:1;
	/*B1*/
	u8 spatial_reuse_1_m:3;
	u8 spatial_reuse_2:4;
	u8 spatial_reuse_3_l:1;
	/*B2*/
	u8 spatial_reuse_3_m:3;
	u8 spatial_reuse_4:4;
	u8 rsvd_1:1;
	/*B3*/
	u8 bw:2;
	u8 rsvd_2:6;
};

struct plcp_hdr_he_sig_a2_tb_info {
	/*B0*/
	u8 txop:7;
	u8 rsvd_1:1;
	/*B1*/
	u8 rsvd_2;
};

struct bb_snif_info {
	bool snif_mode_en;
	u8 snif_rate_mode; /*0~3: Legacy/HT/VHT/HE*/
	u16 snif_rate;
	enum bb_physts_bitmap_t snif_ie_bitmap_type;
	/*LGCY/HT/VHT/HE*/
	u32* snif_l_sig;
	u16 snif_l_sig_len; /*24 Bit*/
	/*HT/VHT/HE*/
	u32* snif_sig_a1; 
	u16 snif_sig_a1_len; /*24/24/26*/
	u32* snif_sig_a2;
	u16 snif_sig_a2_len; /*24/24/26*/
	/*VHT/HE*/
	u32* snif_sig_b;
	u16 snif_sig_b_len; /*max29/N*/
	struct plcp_hdr_vht_sig_b_info plcp_hdr_vht_sig_b_i;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_sniffer_mode_en(struct bb_info *bb, bool en);


#endif