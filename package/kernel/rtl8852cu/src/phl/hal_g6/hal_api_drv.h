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
#ifndef _HAL_API_DRV_H_
#define _HAL_API_DRV_H_

u32 rtw_hal_mac_coex_init(struct rtw_hal_com_t *hal_com, u8 pta_mode, u8 direction);
u32 rtw_hal_mac_coex_reg_read(struct rtw_hal_com_t *hal_com, u32 offset, u32 *value);
u32 rtw_hal_mac_coex_reg_write(struct rtw_hal_com_t *hal_com, u32 offset, u32 value);
u32 rtw_hal_mac_set_scoreboard(struct rtw_hal_com_t *hal_com, u32 *value);
u32 rtw_hal_mac_get_scoreboard(struct rtw_hal_com_t *hal_com, u32 *value);
u32 rtw_hal_mac_set_grant(struct rtw_hal_com_t *hal_com, u8 *value);
u32 rtw_hal_mac_get_grant(struct rtw_hal_com_t *hal_com, u8 *value);
u32 rtw_hal_mac_set_polluted(struct rtw_hal_com_t *hal_com, u8 band, u8 tx_val, u8 rx_val);
u32 rtw_hal_mac_set_tx_time(struct rtw_hal_com_t *hal_com, u8 is_btc,
			    u8 is_resume, u8 macid, u32 tx_time);
u32 rtw_hal_mac_get_tx_time(struct rtw_hal_com_t *hal_com, u8 macid, u32 *tx_time);
u32 rtw_hal_mac_set_tx_retry_limit(struct rtw_hal_com_t *hal_com, u8 is_btc,
				   u8 is_resume, u8 macid, u8 tx_retry);
u32 rtw_hal_mac_set_rsc_cfg(struct rtw_hal_com_t *hal_com, u8 rrsr_cfg, u8 band);
u32 rtw_hal_mac_set_rrsr_ref_rate_sel(struct rtw_hal_com_t *hal_com, bool ref_rate_sel, u8 band);
u32 rtw_hal_mac_get_tx_retry_limit(struct rtw_hal_com_t *hal_com, u8 macid,
				   u8 *tx_retry);
u32 rtw_hal_mac_get_bt_polt_cnt(struct rtw_hal_com_t *hal_com, u8 band,
				u16 *cnt);
u32 rtw_hal_mac_set_coex_ctrl(struct rtw_hal_com_t *hal_com, u32 val);
u32 rtw_hal_mac_get_coex_ctrl(struct rtw_hal_com_t *hal_com, u32 *val);

u32 rtw_hal_mac_send_h2c(struct rtw_hal_com_t *hal_com,
	struct rtw_g6_h2c_hdr *hdr, u32 *pvalue);

u32 rtw_hal_read_rf_reg(struct rtw_hal_com_t *hal_com,
					enum rf_path path, u32 addr, u32 mask);

bool rtw_hal_write_rf_reg(struct rtw_hal_com_t *hal_com,
					enum rf_path path, u32 addr, u32 mask, u32 data);

u8 rtw_hal_ex_cn_report(struct rtw_hal_com_t *hal_com);

u8 rtw_hal_ex_evm_1ss_report(struct rtw_hal_com_t *hal_com);

u8 rtw_hal_ex_evm_max_report(struct rtw_hal_com_t *hal_com);

u8 rtw_hal_ex_evm_min_report(struct rtw_hal_com_t *hal_com);

u32 rtw_hal_bb_read_cr(struct rtw_hal_com_t *hal_com, u32 addr, u32 mask);

bool rtw_hal_bb_write_cr(struct rtw_hal_com_t *hal_com, u32 addr, u32 mask,
			 u32 data);

#ifdef CONFIG_PHL_DFS
u32 rtw_hal_mac_cfg_dfs_rpt(struct rtw_hal_com_t *hal_com, struct hal_mac_dfs_rpt_cfg *conf);
#endif

u32 rtw_hal_mac_write_msk_pwr_reg(
	struct rtw_hal_com_t *hal_com, u8 band, u32 offset, u32 mask, u32 val);

u32 rtw_hal_mac_set_pwr_reg(struct rtw_hal_com_t *hal_com, u8 band, u32 offset, u32 val);

u32 rtw_hal_mac_get_pwr_reg(struct rtw_hal_com_t *hal_com, u8 band, u32 offset, u32 *val);

enum rtw_hal_status
rtw_hal_mac_get_log_efuse_size(struct rtw_hal_com_t *hal_com, u32 *val,
						bool is_limited);

enum rtw_hal_status
rtw_hal_mac_get_efuse_a_die_size(struct rtw_hal_com_t *hal_com, u8 *val);

enum rtw_hal_status
rtw_hal_mac_read_log_efuse_map(struct rtw_hal_com_t *hal_com, u8 *map,
						bool is_limited);

enum rtw_hal_status
rtw_hal_mac_write_log_efuse_map(struct rtw_hal_com_t *hal_com,
						u8 *map,
						u32 map_size,
						u8 *mask,
						u32 mask_size,
						u8 *map_version,
						u8 *mask_version,
						u8 version_length,
						u8 part,
						bool is_limited,
						u8 a_die_size,
						u32 hci_to_a_die_offset,
						u32 a_die_start_offset);

enum rtw_hal_status
rtw_hal_mac_read_hidden_rpt(struct rtw_hal_com_t *hal_com);

enum rtw_hal_status
rtw_hal_mac_check_efuse_autoload(struct rtw_hal_com_t *hal_com, u8 *autoload);

enum rtw_hal_status
rtw_hal_mac_get_efuse_avl(struct rtw_hal_com_t *hal_com, u32 *val);

enum rtw_hal_status
rtw_hal_mac_get_efuse_size(struct rtw_hal_com_t *hal_com, u32 *val);

enum rtw_hal_status
rtw_hal_mac_get_efuse_mask_size(struct rtw_hal_com_t *hal_com, u32 *val,
						bool is_limited);

enum rtw_hal_status
rtw_hal_mac_get_efuse_info(struct rtw_hal_com_t *hal_com,
	u8 *efuse_map, enum rtw_efuse_info info_type, void *value,
	u8 size, u8 map_valid);

enum rtw_hal_status
rtw_hal_mac_read_phy_efuse(struct rtw_hal_com_t *hal_com,
	u32 addr, u32 size, u8 *value);

enum rtw_hal_status
rtw_hal_mac_read_bt_phy_efuse(struct rtw_hal_com_t *hal_com,
	u32 addr, u32 size, u8 *value);

enum rtw_hal_status
rtw_hal_mac_get_xcap(struct rtw_hal_com_t *hal_com, u8 sc_xo, u32 *value);

enum rtw_hal_status
rtw_hal_mac_set_xcap(struct rtw_hal_com_t *hal_com, u8 sc_xo, u32 value);

enum rtw_hal_status
rtw_hal_mac_get_xsi(struct rtw_hal_com_t *hal_com, u8 offset, u8 *val);

enum rtw_hal_status
rtw_hal_mac_set_xsi(struct rtw_hal_com_t *hal_com, u8 offset, u8 val);

enum rtw_hal_status
rtw_hal_bb_get_efuse_info(struct rtw_hal_com_t *hal_com,
	u8 *efuse_map, enum rtw_efuse_info info_type, void *value,
	u8 size, u8 map_valid);

enum rtw_hal_status
rtw_hal_rf_get_efuse_info(struct rtw_hal_com_t *hal_com,
	u8 *efuse_map, enum rtw_efuse_info info_type, void *value,
	u8 size, u8 map_valid);

void
rtw_hal_rf_set_power_table_switch(struct rtw_hal_com_t *hal_com,
				enum phl_phy_idx phy_idx,
				u8 pwrbyrate_type, u8 pwrlmt_type);

void rtw_hal_bb_set_tx_pow_per_path_lmt(struct rtw_hal_com_t *hal_com,
					s16 pwr_lmt_a, s16 pwr_lmt_b);

void
rtw_hal_bb_set_tx_pow_ref(struct rtw_hal_com_t *hal_com,
			  enum phl_phy_idx phy_idx);

enum rtw_hal_status rtw_hal_rf_read_pwr_table(
	struct rtw_hal_com_t *hal_com, u8 rf_path, u16 rate,
	u8 bandwidth, u8 channel, u8 offset, u8 dcm,
	u8 beamforming, s16 *get_item);

enum rtw_hal_status rtw_hal_rf_wlan_tx_power_control(
	struct rtw_hal_com_t *hal_com,
	enum phl_phy_idx phy, enum phl_pwr_ctrl pwr_ctrl_idx, u32 tx_power_val, bool enable);

enum rtw_hal_status rtw_hal_rf_wl_tx_power_control(
	struct rtw_hal_com_t *hal_com,
	u32 tx_power_val);

enum rtw_hal_status rtw_hal_rf_ctrl_dbcc(struct rtw_hal_com_t *hal_com,
	bool dbcc_en);

void rtw_hal_rf_tssi_scan_ch(struct rtw_hal_com_t *hal_com,
		enum phl_phy_idx phy_idx, enum rf_path path);

enum rtw_hal_status
rtw_hal_btc_get_efuse_info(struct rtw_hal_com_t *hal_com,
	u8 *efuse_map, enum rtw_efuse_info info_type, void *value,
	u8 size, u8 map_valid);

enum rtw_hal_status rtw_hal_efuse_get_info(struct rtw_hal_com_t *hal_com,
					enum rtw_efuse_info info_type, void *value, u8 size);
enum rtw_hal_status rtw_hal_notify_switch_band(void *hinfo,
			enum band_type band, enum phl_phy_idx phy_idx);

enum rtw_hal_status rtw_hal_reset(struct rtw_hal_com_t *hal_com,
			enum phl_phy_idx phy_idx, u8 band_idx, bool reset);

#ifndef CONFIG_BTCOEX
#define rtw_hal_btc_wl_rfk_ntfy(hal_com, phy_idx, rfk_type, rfk_process) 0
#else
u8 rtw_hal_btc_wl_rfk_ntfy(struct rtw_hal_com_t *hal_com, u8 phy_idx, u8 rfk_type, u8 rfk_process);
#endif
void rtw_hal_btc_power_on_ntfy(void *hinfo);
void rtw_hal_btc_power_off_ntfy(void *hinfo);

enum rtw_hal_status
rtw_hal_tx_pause(struct rtw_hal_com_t *hal_com,
			u8 band_idx, bool tx_pause, enum tx_pause_rson rson);

enum rtw_hal_status
rtw_hal_mac_set_macid_pause(struct rtw_hal_com_t *hal_com,
                            u16 macid, bool pause);

enum rtw_hal_status
rtw_hal_mac_set_macid_grp_pause(struct rtw_hal_com_t *hal_com,
                            u32 *macid_arr, u8 macid_arr_sz, bool pause);

enum rtw_hal_status
rtw_hal_mac_set_macid_pkt_drop(struct rtw_hal_com_t *hal_com,
                               u16 macid, u8 sel, u8 band, u8 port, u8 mbssid);
enum rtw_hal_status
rtw_hal_mac_data_drop_once(struct rtw_hal_com_t *hal_com,
                                  struct rtw_phl_stainfo_t *sta);
u32
rtw_hal_mac_lamode_trig(struct rtw_hal_com_t *hal_com, u8 trig);

enum rtw_hal_status
rtw_hal_mac_lamode_cfg_buf(struct rtw_hal_com_t *hal_com, u8 buf_sel,
				u32 *addr_start, u32 *addr_end);

enum rtw_hal_status
rtw_hal_mac_lamode_cfg(struct rtw_hal_com_t *hal_com, u8 func_en,
				u8 restart_en, u8 timeout_en, u8 timeout_val,
				u8 data_loss_imr, u8 la_tgr_tu_sel, u8 tgr_time_val);

enum rtw_hal_status
rtw_hal_mac_get_lamode_st(struct rtw_hal_com_t *hal_com, u8 *la_state,
				u16 *la_finish_addr, bool *la_round_up,
				bool *la_loss_data);

void
rtw_hal_mac_get_buffer_data(struct rtw_hal_com_t *hal_com, u32 strt_addr,
				u8 *buf, u32 len, u32 dbg_path);

enum rtw_hal_status
rtw_hal_fw_log_cfg(void *halcom, u8 op, u8 type, u32 value);

enum rtw_hal_status
rtw_hal_mac_tx_path_map_cfg(struct rtw_hal_com_t *hal_com,
                            struct hal_txmap_cfg *txmap_cfg);

/* HALBB APIs for HW TX */

enum rtw_hal_status
rtw_hal_bb_set_plcp_tx(struct rtw_hal_com_t *hal_com,
						struct mp_plcp_param_t *plcp_tx_struct,
						struct mp_usr_plcp_gen_in *plcp_usr_info,
						enum phl_phy_idx plcp_phy_idx,
						u8 *plcp_sts);

enum rtw_hal_status
rtw_hal_bb_set_pmac_cont_tx(struct rtw_hal_com_t *hal_com, u8 enable, u8 is_cck,
							enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_set_pmac_carrier_suppression_tx(struct rtw_hal_com_t *hal_com, u8 enable, u8 is_cck,
							enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_set_pmac_packet_tx(struct rtw_hal_com_t *hal_com, u8 enable,
							u8 is_cck, u16 tx_cnt ,u16 period, u16 tx_time, u8 cck_lbk_en,
							enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_set_pmac_fw_trigger_tx(struct rtw_hal_com_t *hal_com, u8 enable,
							u8 is_cck, u16 tx_cnt, u8 tx_duty,
							enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_set_dpd_bypass(struct rtw_hal_com_t *hal_com, bool pdp_bypass,
						enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_set_power(struct rtw_hal_com_t *hal_com, s16 power_dbm,
					enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_get_power(struct rtw_hal_com_t *hal_com, s16 *power_dbm,
					enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_rf_set_ch_bw(struct rtw_hal_com_t *hal_com,
                     enum phl_phy_idx phy,
                     u8 center_ch,
                     enum band_type band,
                     enum channel_width bw);

enum rtw_hal_status
rtw_hal_rf_chl_rfk_trigger(struct rtw_hal_com_t *hal_com,
                           u8 phy_idx,
                           u8 force);

enum rtw_hal_status
rtw_hal_bb_ctrl_btg(struct rtw_hal_com_t *hal_com, bool btg);

enum rtw_hal_status
rtw_hal_bb_ctrl_btc_preagc(struct rtw_hal_com_t *hal_com, bool bt_en);

enum rtw_hal_status
rtw_hal_bb_cfg_rx_path(struct rtw_hal_com_t *hal_com, u8 rx_path, u8 phy_idx);

enum rtw_hal_status
rtw_hal_bb_cfg_tx_path(struct rtw_hal_com_t *hal_com, u8 tx_path, u8 phy_idx);

/* mode: 0 = tmac, 1 = pmac */
enum rtw_hal_status
rtw_hal_bb_tx_mode_switch(struct rtw_hal_com_t *hal_com,
						enum phl_phy_idx phy_idx,
						u8 mode);
/* HALBB APIs for HW TX END*/
/*@--------------------------[Prptotype]-------------------------------------*/
/**
 * rtw_hal_bb_get_txsc input arguments:
 * @hal_com: hal com info
 * @pri_ch: Spec-defined primary channel index
 * @central_ch: Spec-defined central channel index
 * @cbw: Channel BW
 * @dbw: Data BW
 */
u8 rtw_hal_bb_get_txsc(struct rtw_hal_com_t *hal_com, u8 pri_ch,
		u8 central_ch, enum channel_width cbw, enum channel_width dbw);

void rtw_hal_bb_nhm_mntr_result(struct rtw_hal_com_t *hal_com, void *rpt, enum phl_phy_idx phy_idx);

bool rtw_hal_query_regulation(void *phl, struct rtw_regulation_info *info);

enum rtw_hal_status
rtw_hal_mac_set_tpu_mode(struct rtw_hal_com_t *hal_com,
			 enum rtw_tpu_op_mode op_mode_new, u8 band);
enum rtw_hal_status
rtw_hal_mac_write_pwr_limit_rua_reg(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_write_pwr_limit_reg(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_write_pwr_by_rate_reg(struct rtw_hal_com_t *hal_com, u8 band);

enum rtw_hal_status
rtw_hal_mac_get_efuse_bt_avl(struct rtw_hal_com_t *hal_com, u32 *val);

enum rtw_hal_status
rtw_hal_mac_get_efuse_bt_size(struct rtw_hal_com_t *hal_com, u32 *val);

enum rtw_hal_status
rtw_hal_mac_get_efuse_bt_mask_size(struct rtw_hal_com_t *hal_com, u32 *val);

enum rtw_hal_status
rtw_hal_mac_get_log_efuse_bt_size(struct rtw_hal_com_t *hal_com, u32 *val);

enum rtw_hal_status
rtw_hal_mac_read_log_efuse_bt_map(struct rtw_hal_com_t *hal_com, u8 *map);

enum rtw_hal_status
rtw_hal_mac_write_log_efuse_bt_map(struct rtw_hal_com_t *hal_com,
                            u8 *map,
                            u32 map_size,
                            u8 *mask,
                            u32 mask_size);


enum rtw_hal_status
rtw_hal_bb_backup_info(struct rtw_hal_com_t *hal_com, u8 cur_phy_idx);

enum rtw_hal_status
rtw_hal_bb_restore_info(struct rtw_hal_com_t *hal_com, u8 cur_phy_idx);

enum rtw_hal_status
rtw_hal_bb_tssi_bb_reset(struct rtw_hal_com_t *hal_com);

enum rtw_hal_status
rtw_hal_bb_query_rainfo(void *hal, struct rtw_hal_stainfo_t *hal_sta,
			struct rtw_phl_rainfo *phl_rainfo);
enum rtw_hal_status rtw_hal_bb_ctrl_rx_cca(struct rtw_hal_com_t *hal_com,
	bool cca_en, enum phl_phy_idx phy_idx);
bool rtw_hal_bb_query_cck_en(struct rtw_hal_com_t *hal_com, enum phl_phy_idx phy_idx);

enum rtw_hal_status rtw_hal_bb_ctrl_cck_en(struct rtw_hal_com_t *hal_com,
	bool cca_en, enum phl_phy_idx phy_idx);
enum rtw_hal_status
rtw_hal_mac_set_pwr_ofst_mode(struct rtw_hal_com_t *hal_com, u8 band, s8 pwr_ofst);
enum rtw_hal_status
rtw_hal_mac_write_pwr_ofst_mode(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_set_pwr_ofst_bw(struct rtw_hal_com_t *hal_com, u8 band, s8 pwr_ofst);
enum rtw_hal_status
rtw_hal_mac_write_pwr_ofst_bw(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_write_pwr_ref_reg(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_write_pwr_limit_en(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_set_pwr_lmt_en_val(struct rtw_hal_com_t *hal_com, u8 band, bool en_val);
bool
rtw_hal_mac_get_pwr_lmt_en_val(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_write_pwr_limit_rua_reg(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_write_pwr_limit_reg(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_write_pwr_by_rate_reg(struct rtw_hal_com_t *hal_com, u8 band);
enum rtw_hal_status
rtw_hal_mac_read_efuse_bt_hidden(struct rtw_hal_com_t *hal_com, u32 addr, u32 size,  u8 *val);

enum rtw_hal_status
rtw_hal_mac_write_efuse_bt_hidden(struct rtw_hal_com_t *hal_com, u32 addr, u8 val);

enum rtw_hal_status
rtw_hal_mac_ax_init_bf_role(struct rtw_hal_com_t *hal_com, u8 bf_role, u8 band);

enum rtw_hal_status
rtw_hal_mac_ax_deinit_bfee(struct rtw_hal_com_t *hal_com, u8 band);

void rtw_hal_bb_gpio_setting_all(struct rtw_hal_com_t *hal_com, u8 rfe_idx);

void rtw_hal_bb_gpio_setting(struct rtw_hal_com_t *hal_com, u8 gpio_idx, u8 path,
			bool inv, u8 src);

enum rtw_hal_status
rtw_hal_mac_set_gpio_func(struct rtw_hal_com_t *hal_com, u8 func, s8 gpio_cfg);

enum rtw_phl_status
rtw_hal_cmd_notify(struct rtw_phl_com_t *phl_com,
                   enum phl_msg_evt_id event,
                   void *hal_cmd,
                   u8 hw_idx);

enum rtw_hal_status rtw_hal_mac_add_cmd_ofld(struct rtw_hal_com_t *hal_com, struct rtw_mac_cmd *cmd);
enum rtw_hal_status rtw_hal_mac_cmd_ofld(struct rtw_hal_com_t *hal_com);
#ifdef CONFIG_FW_IO_OFLD_SUPPORT
void
rtw_hal_bb_fwofld_cfgcr_start(struct rtw_hal_com_t *hal_com);
void
rtw_hal_bb_fwofld_cfgcr_end(struct rtw_hal_com_t *hal_com);

#endif

void rtw_hal_bb_env_rpt(struct rtw_hal_com_t *hal_com, struct rtw_env_report *env_rpt,
		     enum phl_phy_idx phy_indx);

void rtw_hal_bb_set_pow_patten_sharp(struct rtw_hal_com_t *hal_com, u8 channel, u8 is_cck, u8 sharp_id, enum phl_phy_idx phy_idx);

#if defined(CONFIG_PHL_DRV_HAS_NVM) || defined(RTW_FLASH_98D)
/* For BB to get paramater values */
enum rtw_hal_status
rtw_hal_flash_get_info(struct rtw_hal_com_t *hal_com,
		       enum rtw_efuse_info info_type,
		       void *value,
		       u8 size);
#endif /* CONFIG_PHL_DRV_HAS_NVM || RTW_FLASH_98D */

enum rtw_hal_status
rtw_hal_rf_syn_config(struct rtw_hal_com_t *hal_com,
                      u8 syn_id,
                      enum phl_phy_idx phy_idx,
                      u8 path,
                      bool turn_on);

#endif /*_HAL_API_DRV_H_*/
