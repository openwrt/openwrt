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
#ifndef __HALBB_8852C_API_H__
#define __HALBB_8852C_API_H__
#ifdef BB_8852C_SUPPORT
#include "../halbb_api.h"

/*@--------------------------[Define]-------------------------------------*/
#define INVALID_RF_DATA 0xffffffff
#define BB_PATH_MAX_8852C 2
#define HE_N_USER_MAX_8852C 8

#define GAIN_HIDE_EFUSE_START_ADDR_8852C 0x585

#define GAIN_HIDE_EFUSE_A_2G_8852C 0x5BB
#define GAIN_HIDE_EFUSE_A_5GL_8852C 0x5BA
#define GAIN_HIDE_EFUSE_A_5GM_8852C 0x5B9
#define GAIN_HIDE_EFUSE_A_5GH_8852C 0x5B8
#define GAIN_HIDE_EFUSE_A_6GH_0_8852C 0x5B7
#define GAIN_HIDE_EFUSE_A_6GH_1_8852C 0x5B6
#define GAIN_HIDE_EFUSE_A_6GH_2_8852C 0x5B5
#define GAIN_HIDE_EFUSE_A_6GH_3_8852C 0x5B4
#define GAIN_HIDE_EFUSE_A_6GH_4_8852C 0x5B3
#define GAIN_HIDE_EFUSE_A_6GH_5_8852C 0x5B2
#define GAIN_HIDE_EFUSE_A_6GH_6_8852C 0x5B1
#define GAIN_HIDE_EFUSE_A_6GH_7_8852C 0x5B0
#define GAIN_HIDE_EFUSE_B_2G_8852C 0x590
#define GAIN_HIDE_EFUSE_B_5GL_8852C 0x58F
#define GAIN_HIDE_EFUSE_B_5GM_8852C 0x58E
#define GAIN_HIDE_EFUSE_B_5GH_8852C 0x58D
#define GAIN_HIDE_EFUSE_B_6GH_0_8852C 0x58C
#define GAIN_HIDE_EFUSE_B_6GH_1_8852C 0x58B
#define GAIN_HIDE_EFUSE_B_6GH_2_8852C 0x58A
#define GAIN_HIDE_EFUSE_B_6GH_3_8852C 0x589
#define GAIN_HIDE_EFUSE_B_6GH_4_8852C 0x588
#define GAIN_HIDE_EFUSE_B_6GH_5_8852C 0x587
#define GAIN_HIDE_EFUSE_B_6GH_6_8852C 0x586
#define GAIN_HIDE_EFUSE_B_6GH_7_8852C 0x585

/*@--------------------------[Enum]------------------------------------------*/
/*@--------------------------[Structure]-------------------------------------*/
struct bb_h2c_lps_info_8852c {
	u8 central_ch;
	u8 pri_ch;
	u8 bw;
	u8 band_type;
};
/*@--------------------------[Prptotype]-------------------------------------*/
struct bb_info;
u8 halbb_ex_cn_report_8852c(struct bb_info *bb);

u8 halbb_ex_evm_1ss_report_8852c(struct bb_info *bb);
u8 halbb_ex_evm_max_report_8852c(struct bb_info *bb);
u8 halbb_ex_evm_min_report_8852c(struct bb_info *bb);

bool halbb_set_vht_mu_user_idx_8852c(struct bb_info *bb, bool en, u8 idx,
				     enum phl_phy_idx phy_idx);
bool halbb_set_pwr_ul_tb_ofst_8852c(struct bb_info *bb,
				      s8 pw_ofst, enum phl_phy_idx phy_idx);
bool halbb_lps_info_8852c(struct bb_info *bb, u16 mac_id);
void halbb_digital_cfo_comp_8852c(struct bb_info *bb, s32 curr_cfo);

void halbb_tx_triangular_shap_cfg_8852c(struct bb_info *bb, u8 shape_idx,
					  enum phl_phy_idx phy_idx);
void halbb_set_digital_pwr_comp_8852c(struct bb_info *bb, bool en,
				      enum phl_phy_idx phy_idx);
void halbb_tx_dfir_shap_cck_8852c(struct bb_info *bb, u8 ch, u8 shape_idx,
				  enum phl_phy_idx phy_idx);
bool halbb_ctrl_bw_ch_8852c(struct bb_info *bb, u8 pri_ch, u8 central_ch,
			      enum channel_width bw, enum band_type band,
			      enum phl_phy_idx phy_idx);
void halbb_1r_cca_cr_cfg_8852c(struct bb_info *bb, enum rf_path rx_path);
bool halbb_ctrl_rx_path_8852c(struct bb_info *bb, enum rf_path rx_path, bool dbcc_en);
bool halbb_ctrl_tx_path_pmac_8852c(struct bb_info *bb, enum rf_path tx_path, bool dbcc_en);
bool halbb_ctrl_tx_path_tmac_8852c(struct bb_info *bb, enum rf_path tx_path_1sts, bool dbcc_en);
bool halbb_ctrl_tx_path_8852c(struct bb_info *bb, enum rf_path tx_path);
void halbb_tssi_ctrl_set_dbw_table_8852c(struct bb_info *bb);
#if 0
bool halbb_cfg_rx_path_8852c(struct bb_info *bb, enum rf_path rx_path,
			       u8 rx_nss, enum phl_phy_idx phy_idx);

bool halbb_cfg_tx_path_8852c(struct bb_info *bb, enum rf_path tx_path,
			       u8 tx_nss,enum phl_phy_idx phy_idx);
#endif

u32 halbb_phy0_to_phy1_ofst_8852c(struct bb_info *bb, u32 addr);

void halbb_gpio_ctrl_dump_8852c(struct bb_info *bb);

void halbb_gpio_rfm_8852c(struct bb_info *bb, enum bb_path path,
			    enum bb_rfe_src_sel src, bool dis_tx_gnt_wl,
			    bool active_tx_opt, bool act_bt_en, u8 rfm_output_val);

void halbb_gpio_trsw_table_8852c(struct bb_info *bb, enum bb_path path,
				   bool path_en, bool trsw_tx,
				   bool trsw_rx, bool trsw, bool trsw_b);

void halbb_gpio_setting_8852c(struct bb_info *bb, u8 gpio_idx,
			        enum bb_path path, bool inv,
			        enum bb_rfe_src_sel src);

void halbb_gpio_setting_all_8852c(struct bb_info *bb, u8 rfe_idx);

void halbb_gpio_setting_init_8852c(struct bb_info *bb);

void halbb_bb_reset_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx);

u32 halbb_read_rf_reg_8852c(struct bb_info *bb, enum rf_path path, u32 reg_addr,
			    u32 bit_mask);

bool halbb_write_rf_reg_8852c(struct bb_info *bb, enum rf_path path, u32 reg_addr,
			      u32 bit_mask, u32 data);

bool halbb_spur_location_8852c(struct bb_info *bb, u8 central_ch,
                               enum channel_width bw, enum band_type band,
			       u32 *intf);
bool halbb_spur_location_for_CSI_8852c(struct bb_info *bb, u8 central_ch,
                               enum channel_width bw, enum band_type band,
			       u32 *intf);

bool halbb_ctrl_bw_8852c(struct bb_info *bb, u8 pri_ch, enum channel_width bw,
			 enum phl_phy_idx phy_idx);

bool halbb_query_cck_en_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx,
			      u8 func_idx);

void halbb_ctrl_cck_en_8852c(struct bb_info *bb, bool cck_en,
			     enum phl_phy_idx phy_idx, u8 func_idx);

u16 halbb_cfg_cmac_tx_ant_8852c(struct bb_info *bb, enum rf_path tx_path);

void halbb_ctrl_trx_path_8852c(struct bb_info *bb, enum rf_path tx_path,
			       u8 tx_nss, enum rf_path rx_path, u8 rx_nss);
#if 0
void halbb_cfg_trx_path_8852c(struct bb_info *bb, enum rf_path tx_path,
				u8 tx_nss, enum rf_path rx_path, u8 rx_nss,
				enum phl_phy_idx phy_idx, bool tmac_en);
#endif

void halbb_dfs_en_8852c(struct bb_info *bb, bool en);

void halbb_adc_en_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx);

void halbb_tssi_cont_en_8852c(struct bb_info *bb, bool en, enum rf_path path);

void halbb_bb_reset_en_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx);

void halbb_fix_rx_htmcs0_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx);

void halbb_tssi_bb_reset_8852c(struct bb_info *bb);

bool halbb_ctrl_sco_cck_8852c(struct bb_info *bb, u8 pri_ch);

bool halbb_adc_cfg_8852c(struct bb_info *bb, enum channel_width bw,
			 enum rf_path path);

void halbb_ctrl_rf_mode_8852c(struct bb_info *bb,  enum phl_rf_mode mode);

bool halbb_ctrl_ch2_80p80_8852c(struct bb_info *bb, u8 central_ch);

#ifdef HALBB_DBCC_SUPPORT
void halbb_ctrl_dbcc_8852c(struct bb_info *bb, bool dbcc_en);

void halbb_cfg_dbcc_cck_phy_map_8852c(struct bb_info *bb, bool dbcc_en,
				      enum phl_phy_idx cck_phy_map);

void halbb_cfg_dbcc_8852c(struct bb_info *bb, struct bb_dbcc_cfg_info *cfg);
#endif

void halbb_ctrl_rx_cca_8852c(struct bb_info *bb, bool cca_en, enum phl_phy_idx phy_idx);

void halbb_ctrl_ofdm_en_8852c(struct bb_info *bb, bool ofdm_en,
			      enum phl_phy_idx phy_idx);
bool halbb_set_txpwr_dbm_8852c(struct bb_info *bb, s16 power_dbm,
			       enum phl_phy_idx phy_idx);

void halbb_reset_bb_hw_cnt_8852c(struct bb_info *bb);

s16 halbb_get_txpwr_dbm_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx);

s16 halbb_get_txinfo_txpwr_dbm_8852c(struct bb_info *bb);

bool halbb_set_cck_txpwr_idx_8852c(struct bb_info *bb, u16 power_idx,
				   enum rf_path tx_path);
u16 halbb_get_cck_txpwr_idx_8852c(struct bb_info *bb, enum rf_path tx_path);

s16 halbb_get_cck_ref_dbm_8852c(struct bb_info *bb, enum rf_path tx_path);

bool halbb_set_ofdm_txpwr_idx_8852c(struct bb_info *bb, u16 power_idx,
				    enum rf_path tx_path);

u16 halbb_get_ofdm_txpwr_idx_8852c(struct bb_info *bb, enum rf_path tx_path);

s16 halbb_get_ofdm_ref_dbm_8852c(struct bb_info *bb, enum rf_path tx_path);

void halbb_backup_info_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx);

void halbb_restore_info_8852c(struct bb_info *bb, enum phl_phy_idx phy_idx);

bool halbb_cfg_lbk_8852c(struct bb_info *bb, bool lbk_en, bool is_dgt_lbk,
			 enum rf_path tx_path, enum rf_path rx_path,
			 enum channel_width bw, enum phl_phy_idx phy_idx);

bool halbb_cfg_lbk_cck_8852c(struct bb_info *bb, bool lbk_en, bool is_dgt_lbk,
			     enum rf_path tx_path, enum rf_path rx_path,
			     enum channel_width bw, enum phl_phy_idx phy_idx);

void halbb_ctrl_btg_8852c(struct bb_info *bb, bool btg);

void halbb_ctrl_btc_preagc_8852c(struct bb_info *bb, bool bt_en);

bool halbb_set_txsc_8852c(struct bb_info *bb, u8 txsc, enum phl_phy_idx phy_idx);

bool halbb_set_bss_color_8852c(struct bb_info *bb, u8 bss_color,
			       enum phl_phy_idx phy_idx);

bool halbb_set_sta_id_8852c(struct bb_info *bb, u16 sta_id,
			    enum phl_phy_idx phy_idx);

void halbb_set_igi_8852c(struct bb_info *bb, u8 lna_idx, bool tia_idx,
			u8 rxbb_idx, enum rf_path path);
void halbb_set_tx_pow_per_path_lmt_8852c(struct bb_info *bb, s16 pwr_lmt_a, s16 pwr_lmt_b);
void halbb_set_tx_pow_ref_8852c(struct bb_info *bb, s16 pw_dbm_ofdm, /*s(9,2)*/
				  s16 pw_dbm_cck, s8 ofst,
				  u8 base_cw_0db, u16 tssi_16dBm_cw,
				  u16 *ofdm_cw, u16 *cck_cw,
				  enum phl_phy_idx phy_idx);
void halbb_dump_bb_reg_8852c(struct bb_info *bb, u32 *_used, char *output,
			     u32 *_out_len, bool dump_2_buff, enum bb_frc_phy_dump_reg frc_phy_dump);
void halbb_dbgport_dump_all_8852c(struct bb_info *bb, u32 *_used, char *output,
				    u32 *_out_len);
void halbb_physts_brk_fail_pkt_rpt_8852c(struct bb_info *bb, bool enable,
					   enum phl_phy_idx phy_idx);

bool halbb_rf_write_bb_reg_8852c(struct bb_info *bb, u32 addr, u32 mask, u32 data);
void halbb_pre_agc_en_8852c(struct bb_info *bb, bool enable);
void halbb_agc_fix_gain_8852c(struct bb_info *bb, bool enable, u8 lna_idx,
			      u8 tia_idx, u8 rxbb_idx, enum rf_path path);
void halbb_agc_tia_shrink_8852c(struct bb_info *bb, bool shrink_en,
				bool shrink_init, enum rf_path path);
void halbb_agc_step_en_8852c(struct bb_info *bb, bool pre_pd_agc_en,
			     bool linear_agc_en, bool post_pd_agc_en,
			     bool nlgc_agc_en, enum rf_path path);
void halbb_get_normal_efuse_init_8852c(struct bb_info *bb);
void halbb_get_hidden_efuse_init_8852c(struct bb_info *bb);
void halbb_set_hidden_efuse_8852c(struct bb_info *bb, u8 central_ch, enum band_type band_type, enum rf_path path);
void halbb_set_normal_efuse_8852c(struct bb_info *bb, u8 central_ch, enum band_type band_type, enum rf_path path);
void halbb_set_gain_cr_init_8852c(struct bb_info *bb);
void halbb_set_gain_error_8852c(struct bb_info *bb, u8 central_ch, enum band_type band, enum rf_path path);
void halbb_normal_efuse_verify_8852c(struct bb_info *bb, s8 rx_gain_offset,
				       enum rf_path rx_path,
				       enum phl_phy_idx phy_idx);
void halbb_normal_efuse_verify_cck_8852c(struct bb_info *bb, s8 rx_gain_offset,
					   enum rf_path rx_path,
					   enum phl_phy_idx phy_idx);
void halbb_show_gain_err_8852c(struct bb_info *bb, char input[][16],
			       u32 *_used, char *output, u32 *_out_len);
void halbb_rx_op1db_dbg_8852c(struct bb_info *bb, char input[][16],
			      u32 *_used, char *output, u32 *_out_len);
void halbb_clk_en_8852c(struct bb_info *bb, bool en, enum phl_phy_idx phy_idx);
void halbb_pwr_en_8852c(struct bb_info *bb, bool en);
#endif
#endif
