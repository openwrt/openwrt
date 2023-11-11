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
#ifndef __HALBB_STATISTICS_H__
#define __HALBB_STATISTICS_H__

/*@--------------------------[Define] ---------------------------------------*/
#define CHK_HANG_L_SIG_TH             3
#define HANG_RECOVERY		      true // Disable auto-recovery mechanism for 52A CBV
#define HANG_LIMIT		      1

/*@--------------------------[Enum]------------------------------------------*/
enum stat_type_sel {
	STATE_PROBE_RESP	= 1,
	STATE_BEACON		= 2,
	STATE_ACTION		= 3,
	STATE_BFRP		= 4,
	STATE_NDPA		= 5,
	STATE_BA		= 6,
	STATE_RTS		= 7,
	STATE_CTS		= 8,
	STATE_ACK		= 9,
	STATE_DATA		= 10,
	STATE_NULL		= 11,
	STATE_QOS		= 12,
};

enum stat_mac_type {
	TYPE_PROBE_RESP		= 0x05,
	TYPE_BEACON		= 0x08,
	TYPE_ACTION		= 0x0d,
	TYPE_BFRP		= 0x14,
	TYPE_NDPA		= 0x15,
	TYPE_BA			= 0x19,
	TYPE_RTS		= 0x1b,
	TYPE_CTS		= 0x1c,
	TYPE_ACK		= 0x1d,
	TYPE_DATA		= 0x20,
	TYPE_NULL		= 0x24,
	TYPE_QOS		= 0x28,
};

/*@--------------------------[Structure]-------------------------------------*/
struct bb_usr_set_info {
	u16		ofdm2_rate_idx;
	u16		ht2_rate_idx;
	u16		vht2_rate_idx;
	u16		he2_rate_idx;
	u16		eht2_rate_idx;
	enum stat_mac_type stat_mac_type_i;
	enum stat_type_sel stat_type_sel_i;
};

struct bb_cca_info {
	u32		cnt_ofdm_cca;
	u32		cnt_cck_cca;
	u32		cnt_cca_all;
	u32		cnt_cck_spoofing;
	u32		cnt_ofdm_spoofing;
	u32		cnt_cca_spoofing_all;
	u32		pop_cnt;
};

struct bb_crc_info {
	u32		cnt_ampdu_miss;
	u32		cnt_ampdu_crc_error;
	u32		cnt_ampdu_crc_ok;
	u32		cnt_cck_crc32_error;
	u32		cnt_cck_crc32_ok;
	u32		cnt_ofdm_crc32_error;
	u32		cnt_ofdm_crc32_ok;
	u32		cnt_ht_crc32_error;
	u32		cnt_ht_crc32_ok;
	u32		cnt_vht_crc32_error;
	u32		cnt_vht_crc32_ok;
	u32		cnt_he_crc32_ok;
	u32		cnt_he_crc32_error;
	u32		cnt_eht_crc32_ok;
	u32		cnt_eht_crc32_error;
	u32		cnt_crc32_error_all;
	u32		cnt_crc32_ok_all;
};

struct bb_crc2_info {
	u32		cnt_ofdm2_crc32_error;
	u32		cnt_ofdm2_crc32_ok;
	u8		ofdm2_pcr;
	u32		cnt_ht2_crc32_error;
	u32		cnt_ht2_crc32_ok;
	u8		ht2_pcr;
	u32		cnt_vht2_crc32_error;
	u32		cnt_vht2_crc32_ok;
	u8		vht2_pcr;
	u32		cnt_he2_crc32_error;
	u32		cnt_he2_crc32_ok;
	u8		he2_pcr;
	u32		cnt_eht2_crc32_ok;
	u32		cnt_eht2_crc32_error;
	u8		eht2_pcr;
	u32		cnt_ofdm3_crc32_error;
	u32		cnt_ofdm3_crc32_ok;
};

struct bb_cck_fa_info {
	u32		sfd_gg_cnt;
	u32		sig_gg_cnt;
	u32		cnt_cck_crc_16;
};

struct bb_legacy_fa_info {
	u32		cnt_lsig_brk_s_th;
	u32		cnt_lsig_brk_l_th;
	u32		cnt_parity_fail;
	u32		cnt_rate_illegal;	
	u32		cnt_sb_search_fail;
};

struct bb_ht_fa_info {
	u32		cnt_crc8_fail;
	u32		cnt_crc8_fail_s_th;
	u32		cnt_crc8_fail_l_th;
	u32		cnt_mcs_fail;
};

struct bb_vht_fa_info {
	u32		cnt_crc8_fail_vhta;
	/*u32		cnt_crc8_fail_vhtb; removed at RXD*/
	u32		cnt_mcs_fail_vht;
};

struct bb_he_fa_info {
	u32		cnt_crc4_fail_hea_su;
	u32		cnt_crc4_fail_hea_ersu;
	u32		cnt_crc4_fail_hea_mu;
	u32		cnt_crc4_fail_heb_ch1_mu;
	u32		cnt_crc4_fail_heb_ch2_mu;
	u32		cnt_mcs_fail_he_bcc;
	u32		cnt_mcs_fail_he;
	u32		cnt_mcs_fail_he_dcm;
};

struct bb_fa_info {
	u32		cnt_total_brk;
	u32		cnt_cck_fail;
	u32		cnt_ofdm_fail;
	u32		cnt_fail_all;
	struct bb_cck_fa_info		bb_cck_fa_i;
	struct bb_legacy_fa_info	bb_legacy_fa_i;
	struct bb_ht_fa_info		bb_ht_fa_i;
	struct bb_vht_fa_info		bb_vht_fa_i;
	struct bb_he_fa_info		bb_he_fa_i;
};

struct bb_tx_cnt_info {
	u32		cck_mac_txen;
	u32		cck_phy_txon;
	u32		ofdm_mac_txen;
	u32		ofdm_phy_txon;
};

struct bb_stat_cr_info {
	u32 cck_cca;	    
	u32 cck_cca_m;		
	u32 cck_crc16fail;	
	u32 cck_crc16fail_m;
	u32 cck_crc32ok;	
	u32 cck_crc32ok_m;
	u32 cck_crc32fail;
	u32 cck_crc32fail_m;
	u32 cca_spoofing;		
	u32 cca_spoofing_m;		
	u32 lsig_brk_s_th;		
	u32 lsig_brk_s_th_m;	
	u32 lsig_brk_l_th;		
	u32 lsig_brk_l_th_m;
	u32 htsig_crc8_err_s_th;	
	u32 htsig_crc8_err_s_th_m;	
	u32 htsig_crc8_err_l_th;
	u32 htsig_crc8_err_l_th_m;	
	u32 brk;			
	u32 brk_m;
	u32 brk_sel;		
	u32 brk_sel_m;		
	u32 rxl_err_parity;	
	u32 rxl_err_parity_m;	
	u32 rxl_err_rate;		
	u32 rxl_err_rate_m;		
	u32 ht_err_crc8;	    
	u32 ht_err_crc8_m;		
	u32 vht_err_siga_crc8;	
	u32 vht_err_siga_crc8_m;	
	u32 ht_not_support_mcs;	
	u32 ht_not_support_mcs_m;
	u32 vht_not_support_mcs;	
	u32 vht_not_support_mcs_m;		
	u32 err_during_bt_tx;	
	u32 err_during_bt_tx_m;	
	u32 err_during_bt_rx;	
	u32 err_during_bt_rx_m;	
	u32 edge_murx_nsts0;	
	u32 edge_murx_nsts0_m;	
	u32 search_fail;		
	u32 search_fail_m;	
	u32 ofdm_cca;		
	u32 ofdm_cca_m;		
	u32 ofdm_cca_s20;	
	u32 ofdm_cca_s20_m;
	u32 ofdm_cca_s40;	
	u32 ofdm_cca_s40_m;	
	u32 ofdm_cca_s80;	
	u32 ofdm_cca_s80_m;
	u32 ccktxen;
	u32 ccktxen_m;
	u32 ccktxon;
	u32 ccktxon_m;
	u32 ofdmtxon;		
	u32 ofdmtxon_m;		
	u32 ofdmtxen;		
	u32 ofdmtxen_m;		
	u32 drop_trig;		
	u32 drop_trig_m;	
	u32 pop_trig;		
	u32 pop_trig_m;		
	u32 tx_conflict;		
	u32 tx_conflict_m;	
	u32 wmac_rstb;		
	u32 wmac_rstb_m;		
	u32 en_tb_ppdu_fix_gain;
	u32 en_tb_ppdu_fix_gain_m;
	u32 en_tb_cca_pw_th;
	u32 en_tb_cca_pw_th_m;
	u32 eht_crc_ok;
	u32 eht_crc_ok_m;
	u32 eht_crc_err;
	u32 eht_crc_err_m;
	u32 he_crc_ok;
	u32 he_crc_ok_m;
	u32 he_crc_err;
	u32 he_crc_err_m;
	u32 vht_crc_ok;
	u32 vht_crc_ok_m;
	u32 vht_crc_err;
	u32 vht_crc_err_m;
	u32 ht_crc_ok;
	u32 ht_crc_ok_m;
	u32 ht_crc_err;
	u32 ht_crc_err_m;
	u32 l_crc_ok;
	u32 l_crc_ok_m;
	u32 l_crc_err;
	u32 l_crc_err_m;
	u32 eht_crc_ok2;
	u32 eht_crc_ok2_m;
	u32 eht_crc_err2;
	u32 eht_crc_err2_m;
	u32 he_crc_ok2;
	u32 he_crc_ok2_m;
	u32 he_crc_err2;
	u32 he_crc_err2_m;
	u32 vht_crc_ok2;
	u32 vht_crc_ok2_m;
	u32 vht_crc_err2;
	u32 vht_crc_err2_m;
	u32 ht_crc_ok2;
	u32 ht_crc_ok2_m;
	u32 ht_crc_err2;
	u32 ht_crc_err2_m;
	u32 l_crc_ok2;
	u32 l_crc_ok2_m;
	u32 l_crc_err2;
	u32 l_crc_err2_m;
	u32 l_crc_ok3;
	u32 l_crc_ok3_m;
	u32 l_crc_err3;
	u32 l_crc_err3_m;
	u32 ampdu_rxon;
	u32 ampdu_rxon_m;
	u32 ampdu_miss;
	u32 ampdu_miss_m;
	u32 ampdu_crc_ok;
	u32 ampdu_crc_ok_m;
	u32 ampdu_crc_err;
	u32 ampdu_crc_err_m;
	u32 hesu_err_sig_a_crc4;
	u32 hesu_err_sig_a_crc4_m;
	u32 heersu_err_sig_a_crc4;
	u32 heersu_err_sig_a_crc4_m;
	u32 hemu_err_sig_a_crc4;
	u32 hemu_err_sig_a_crc4_m;
	u32 hemu_err_sigb_ch1_comm_crc4;
	u32 hemu_err_sigb_ch1_comm_crc4_m;
	u32 hemu_err_sigb_ch2_comm_crc4;
	u32 hemu_err_sigb_ch2_comm_crc4_m;
	u32 he_u0_err_bcc_mcs;
	u32 he_u0_err_bcc_mcs_m;
	u32 he_u0_err_mcs;
	u32 he_u0_err_mcs_m;
	u32 he_u0_err_dcm_mcs;
	u32 he_u0_err_dcm_mcs_m;
	u32 r1b_rx_rpt_rst;
	u32 r1b_rx_rpt_rst_m;
	u32 r1b_rr_sel;
	u32 r1b_rr_sel_m;
	u32 rst_all_cnt;
	u32 rst_all_cnt_m;
	u32 enable_all_cnt;
	u32 enable_all_cnt_m;
	u32 enable_ofdm;
	u32 enable_ofdm_m;
	u32 enable_cck;
	u32 enable_cck_m;
	u32 r1b_rx_dis_cca;
	u32 r1b_rx_dis_cca_m;
	u32 intf_r_rate;
	u32 intf_r_rate_m;
	u32 intf_r_mcs;
	u32 intf_r_mcs_m;
	u32 intf_r_vht_mcs;
	u32 intf_r_vht_mcs_m;
	u32 intf_r_he_mcs;
	u32 intf_r_he_mcs_m;
	u32 intf_r_eht_mcs;
	u32 intf_r_eht_mcs_m;
	u32 intf_r_vht_nss;
	u32 intf_r_vht_nss_m;
	u32 intf_r_he_nss;
	u32 intf_r_he_nss_m;
	u32 intf_r_eht_nss;
	u32 intf_r_eht_nss_m;
	u32 intf_r_mac_hdr_type;
	u32 intf_r_mac_hdr_type_m;
	u32 intf_r_pkt_type;
	u32 intf_r_pkt_type_m;
	u32 dbcc;
	u32 dbcc_m;
	u32 dbcc_2p4g_band_sel;
	u32 dbcc_2p4g_band_sel_m;
	u32 cnt_pop_trig;
	u32 cnt_pop_trig_m;
	u32 max_cnt_pop;
	u32 max_cnt_pop_m;
};

struct bb_stat_info {
	struct bb_stat_cr_info bb_stat_cr_i;
	u32		cnt_bw_usc;
	u32		cnt_bw_lsc;
	u32		time_fa_all;
	u32		dbg_port0;
	u32		chk_hang_cnt;
	u8		chk_hang_limit;
	bool	hang_recovery_en;
	bool	cnt_reset_en;
	bool		cck_block_enable;
	bool		ofdm_block_enable;
	struct bb_tx_cnt_info		bb_tx_cnt_i;
	struct bb_cca_info		bb_cca_i;
	struct bb_crc_info		bb_crc_i;
	struct bb_crc2_info		bb_crc2_i;
	struct bb_fa_info		bb_fa_i;
	struct bb_usr_set_info		bb_usr_set_i;
};

struct bb_info;
/*@--------------------------[Prptotype]-------------------------------------*/

void halbb_chk_hang(struct bb_info *bb);
void halbb_print_cnt3(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_print_cnt2(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_print_cnt(struct bb_info *bb, bool cck_enable, enum phl_phy_idx phy_idx, enum phl_phy_idx phy_idx_2);
void halbb_cnt_reg_reset(struct bb_info *bb);
void halbb_set_crc32_cnt2_rate(struct bb_info *bb, u16 rate_idx);
void halbb_set_crc32_cnt3_format(struct bb_info *bb, u8 usr_type_sel);
void halbb_crc32_cnt_dbg(struct bb_info *bb, char input[][16], u32 *_used, 
			 char *output, u32 *_out_len);
void halbb_cck_cnt_statistics(struct bb_info *bb);
void halbb_ofdm_cnt_statistics(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_statistics_reset(struct bb_info *bb);
void halbb_statistics(struct bb_info *bb);
void halbb_statistics_init(struct bb_info *bb);
void halbb_cr_cfg_stat_init(struct bb_info *bb);
void halbb_pmac_statistics_io_en(struct bb_info *bb);
void halbb_pmac_statistics(struct bb_info *bb);
#endif

