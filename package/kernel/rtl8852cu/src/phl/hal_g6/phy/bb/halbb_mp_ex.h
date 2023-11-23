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
#ifndef __HALBB_MP_EX_H__
#define __HALBB_MP_EX_H__

/*@--------------------------[Define] ---------------------------------------*/
#define MAX_USER_NUM 4

/*@--------------------------[Enum]------------------------------------------*/
 
/*@--------------------------[Structure]-------------------------------------*/
struct bb_rpt_cr_info {
	u32 cnt_ccktxon;
	u32 cnt_ccktxon_m;
	u32 cnt_ofdmtxon;
	u32 cnt_ofdmtxon_m;
	u32 cnt_cck_crc32ok_p0;	
	u32 cnt_cck_crc32ok_p0_m;	
	u32 cnt_cck_crc32ok_p1;		
	u32 cnt_cck_crc32ok_p1_m;	
	u32 cnt_l_crc_ok;		
	u32 cnt_l_crc_ok_m;		
	u32 cnt_ht_crc_ok;		
	u32 cnt_ht_crc_ok_m; 
	u32 cnt_vht_crc_ok;		
	u32 cnt_vht_crc_ok_m;	
	u32 cnt_he_crc_ok;		
	u32 cnt_he_crc_ok_m; 
	u32 cnt_cck_crc32fail_p0;	
	u32 cnt_cck_crc32fail_p0_m;	
	u32 cnt_cck_crc32fail_p1;	
	u32 cnt_cck_crc32fail_p1_m;
	u32 cnt_l_crc_err;	
	u32 cnt_l_crc_err_m;
	u32 cnt_ht_crc_err;	
	u32 cnt_ht_crc_err_m;
	u32 cnt_vht_crc_err;
	u32 cnt_vht_crc_err_m;
	u32 cnt_he_crc_err;
	u32 cnt_he_crc_err_m;
	u32 rst_all_cnt; 
	u32 rst_all_cnt_m;	
	u32 phy_sts_bitmap_he_mu;	
	u32 phy_sts_bitmap_he_mu_m;	
	u32 phy_sts_bitmap_vht_mu;	
	u32 phy_sts_bitmap_vht_mu_m;
	u32 phy_sts_bitmap_cck;		
	u32 phy_sts_bitmap_cck_m;	
	u32 phy_sts_bitmap_legacy;	
	u32 phy_sts_bitmap_legacy_m;
	u32 phy_sts_bitmap_ht;		
	u32 phy_sts_bitmap_ht_m; 
	u32 phy_sts_bitmap_vht;		
	u32 phy_sts_bitmap_vht_m;	
	u32 phy_sts_bitmap_he;		
	u32 phy_sts_bitmap_he_m; 
	u32 rpt_tone_evm_idx;		
	u32 rpt_tone_evm_idx_m;		
	u32 dbg_port_ref_clk_en; 
	u32 dbg_port_ref_clk_en_m;	
	u32 dbg_port_en; 		
	u32 dbg_port_en_m;			
	u32 dbg_port_ip_sel; 	
	u32 dbg_port_ip_sel_m;		
	u32 dbg_port_sel;			
	u32 dbg_port_sel_m;			
	u32 dbg32_d;			
	u32 dbg32_d_m;				
	u32 phy_sts_bitmap_trigbase;
	u32 phy_sts_bitmap_trigbase_m;
	u32 sts_keeper_en;			
	u32 sts_keeper_en_m; 	
	u32 sts_keeper_trig_cond;	
	u32 sts_keeper_trig_cond_m;
	u32 sts_dbg_sel; 		
	u32 sts_dbg_sel_m;			
	u32 sts_keeper_read;	
	u32 sts_keeper_read_m;		
	u32 sts_keeper_addr; 	
	u32 sts_keeper_addr_m;		
	u32 sts_keeper_data; 	
	u32 sts_keeper_data_m;		
	u32 pw_dbm_rx0;
	u32 pw_dbm_rx0_m;
	u32 path0_rssi_at_agc_rdy;
	u32 path0_rssi_at_agc_rdy_m;
	u32 path1_rssi_at_agc_rdy;
	u32 path1_rssi_at_agc_rdy_m;
	u32 sts_user_sel;
	u32 sts_user_sel_m;
	u32 path1_g_lna6;
	u32 path1_g_lna6_m;
};

struct bb_rpt_info {
	struct bb_rpt_cr_info bb_rpt_cr_i;
};

struct rxevm_usr {
	u8 rxevm_ss_0;
	u8 rxevm_ss_1;
	u8 rxevm_ss_2;
	u8 rxevm_ss_3;
};

struct rxevm_info {
	struct rxevm_usr rxevm_user[MAX_USER_NUM];
};

struct rxevm_physts {
	// Seg0/1
	struct rxevm_info rxevm_seg[2];
};

struct rssi_i {
	s32 rssi[4];
};

struct rssi_physts {
	// Seg0/1
	struct rssi_i rssi_seg[2];
};

struct mp_physts_rslt_0 {
	u8 rx_path_en_cck;
	u8 cfo_avg_cck;
	u8 evm_avg_cck;
	u8 avg_idle_noise_pwr_cck;
	u8 pop_idx_cck;
};

struct mp_physts_rslt_1 {
	u8 rx_path_en;
	s16 cfo_avg; /*S(12,2), -512~+511.75 kHz*/
	u8 evm_max;
	u8 evm_min;
	u8 snr_avg;
	u8 cn_avg;
	u8 avg_idle_noise_pwr;
	u8 pop_idx;
	u8 rxsc;
	u8 ch_idx;
	enum channel_width bw_idx;
	bool is_su; /*if (not MU && not OFDMA), is_su = 1*/
	bool is_ldpc;
	bool is_ndp;
	bool is_stbc;
	bool grant_bt;
	bool is_awgn;
	bool is_bf;
};

struct mp_physts_rslt_basic {
	struct mp_physts_rslt_0		mp_physts_rslt_basic_0_i;
	struct mp_physts_rslt_1		mp_physts_rslt_basic_1_i;

};


struct bb_mp_psts {
	u32 ie_bitmap;
	struct mp_physts_rslt_basic	mp_physts_rslt_basic_i;
	struct mp_physts_rslt_0		mp_physts_rslt_0_i;
	struct mp_physts_rslt_1		mp_physts_rslt_1_i;
};



struct halbb_mp {
	/*Tx ok count, statistics used in Mass Production Test.*/
	u64 tx_phy_ok_cnt;
	/*Rx CRC32 ok/error count, statistics used in Mass Production Test.*/
	u64 rx_phy_crc_ok_cnt;
	u64 rx_phy_crc_err_cnt;
	/*The Value of IO operation is depend of MptActType.*/
	u32 io_ok_value;
	u32 io_err_value;
};

/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_mp_bt_cfg(struct bb_info *bb, bool bt_connect);
u16 halbb_mp_get_tx_ok(struct bb_info *bb, u32 rate_index,
			enum phl_phy_idx phy_idx);
u32 halbb_mp_get_rx_crc_ok(struct bb_info *bb, enum phl_phy_idx phy_idx);
u32 halbb_mp_get_rx_crc_err(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_mp_cnt_reset(struct bb_info *bb);
void halbb_mp_reset_cnt(struct bb_info *bb);
u8 halbb_mp_get_rxevm(struct bb_info *bb, u8 user, u8 strm, bool rxevm_table);
struct rxevm_physts halbb_mp_get_rxevm_physts(struct bb_info *bb,
					      enum phl_phy_idx phy_idx);
//u16 halbb_mp_get_pwdb_diff(struct bb_info *bb, enum rf_path path);
u8 halbb_mp_get_rssi(struct bb_info *bb, enum rf_path path);
struct rssi_physts halbb_get_mp_rssi_physts(struct bb_info *bb, enum rf_path path, enum phl_phy_idx phy_idx);
void halbb_mp_get_psts(struct bb_info *bb , struct bb_mp_psts *bb_mp_physts);
void halbb_cvrt_2_mp(struct bb_info *bb);
u16 halbb_mp_get_rpl(struct bb_info *bb, enum rf_path path, enum phl_phy_idx phy_idx);
u32 halbb_mp_get_dc_lvl(struct bb_info *bb, enum rf_path path, bool i_ch, enum phl_phy_idx phy_idx);
u16 halbb_mp_get_pwdbm(struct bb_info *bb, enum rf_path path, enum phl_phy_idx phy_idx);
u16 halbb_mp_get_cfo(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_mp_dbg(struct bb_info *bb, char input[][16], u32 *_used,
		  char *output, u32 *_out_len);
void halbb_cr_cfg_mp_init(struct bb_info *bb);
void halbb_dbg_port_sel(struct bb_info *bb, u16 dbg_port_sel, u8 dbg_port_ip_sel,
			bool dbg_port_ref_clk_en, bool dbg_port_en);
#endif
