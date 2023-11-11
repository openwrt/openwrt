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
#ifndef _HAL_API_BB_H_
#define _HAL_API_BB_H_
void rtw_hal_bb_bb_reset_cmn(struct hal_info_t *hal_info,bool en, enum phl_phy_idx phy_idx);
void rtw_hal_bb_dfs_en(struct hal_info_t *hal_info, bool en);
void rtw_hal_bb_tssi_cont_en(struct hal_info_t *hal_info, bool en, enum rf_path path);
void rtw_hal_bb_adc_en(struct hal_info_t *hal_info,bool en, enum phl_phy_idx phy_idx);
void rtw_hal_bb_reset_en(struct hal_info_t *hal_info,bool en, enum phl_phy_idx phy_idx);

bool rtw_hal_bb_proc_cmd(struct hal_info_t *hal_info,
					struct rtw_proc_cmd *incmd,
					char *output, u32 out_len);

enum rtw_hal_status rtw_hal_bb_watchdog(struct hal_info_t *hal_info, u8 is_lps);
enum rtw_hal_status rtw_hal_bb_simple_watchdog(struct hal_info_t *hal_info, u8 io_en);

void rtw_hal_bb_fw_edcca(struct hal_info_t *hal_info);

void rtw_hal_bb_reset(struct hal_info_t *hal_info);

void rtw_hal_bb_dm_init(struct hal_info_t *hal_info);

void rtw_hal_bb_dm_deinit(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal_info);

enum rtw_hal_status rtw_hal_bb_ctrl_rx_cca(struct rtw_hal_com_t *hal_com,
	bool cca_en, enum phl_phy_idx phy_idx);

#ifdef CONFIG_DBCC_SUPPORT
enum rtw_hal_status rtw_hal_bb_ctrl_dbcc(struct hal_info_t *hal_info, bool dbcc_en);

enum rtw_hal_status
rtw_hal_bb_cfg_dbcc_phy_map(struct hal_info_t *hal_info,
                            enum phl_phy_idx phy_idx);

enum rtw_hal_status rtw_hal_bb_cfg_dbcc(struct hal_info_t *hal_info,
				struct rtw_phl_com_t *phl_com, bool dbcc_en);

enum rtw_hal_status
rtw_hal_phy_dbcc_pre_cfg(struct hal_info_t *hal_info,
			struct rtw_phl_com_t *phl_com, bool dbcc_en);

enum rtw_hal_status
rtw_hal_phy_dbcc_cfg(struct hal_info_t *hal_info,
			struct rtw_phl_com_t *phl_com, bool dbcc_en);

void rtw_hal_dbcc_init_bb_reg(struct hal_info_t *hal_info);
#endif /* CONFIG_DBCC_SUPPORT */

u32 rtw_hal_bb_init(struct rtw_phl_com_t *phl_com,
			struct hal_info_t *hal_info);

void rtw_hal_bb_deinit(struct rtw_phl_com_t *phl_com,
			struct hal_info_t *hal_info);

void rtw_hal_init_bb_reg(struct hal_info_t *hal_info);

u8 rtw_hal_ex_cn_report(struct rtw_hal_com_t *hal_com);

u8 rtw_hal_ex_evm_1ss_report(struct rtw_hal_com_t *hal_com);

u8 rtw_hal_ex_evm_max_report(struct rtw_hal_com_t *hal_com);

u8 rtw_hal_ex_evm_min_report(struct rtw_hal_com_t *hal_com);

u32 rtw_hal_read_rf_reg(struct rtw_hal_com_t *hal_com,
			enum rf_path path, u32 addr, u32 mask);

bool rtw_hal_write_rf_reg(struct rtw_hal_com_t *hal_com,
				enum rf_path path, u32 addr, u32 mask, u32 data);

u32 rtw_hal_read_bb_reg(struct rtw_hal_com_t *hal_com,
						u32 addr, u32 mask);

bool rtw_hal_write_bb_reg(struct rtw_hal_com_t *hal_com,
			  			  u32 addr, u32 mask, u32 data);

enum rtw_hal_status
rtw_hal_bb_stainfo_init(struct hal_info_t *hal_info,
				struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_bb_stainfo_deinit(struct hal_info_t *hal_info,
				struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_bb_stainfo_add(struct hal_info_t *hal_info,
				struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_bb_stainfo_delete(struct hal_info_t *hal_info,
				struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_bb_lps_info_update(void *hal, u16 macid);
void rtw_hal_bb_media_status_update(struct hal_info_t *hal_info,
			struct rtw_phl_stainfo_t *sta, bool is_connected);
enum rtw_hal_status
rtw_hal_bb_upt_ramask(struct hal_info_t *hal_info,
				struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_bb_ra_update(struct hal_info_t *hal_info,
				struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_bb_ra_register(struct hal_info_t *hal_info,
				struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_bb_ra_deregister(struct hal_info_t *hal_info,
				struct rtw_phl_stainfo_t *sta);
u8
rtw_hal_bb_get_arfr_idx(struct hal_info_t *hal_info,
				struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_bb_set_ch_bw(struct hal_info_t *hal_info,
					enum phl_phy_idx phy_idx,
					u8 pri_ch,
					u8 central_ch_seg0,
		      		u8 central_ch_seg1,
					enum band_type band,
					enum channel_width bw);
#ifdef CONFIG_FW_IO_OFLD_SUPPORT
bool
rtw_hal_bb_fw_delay(struct hal_info_t *hal_info, u32 val);
#endif

#ifdef CONFIG_PHL_CUSTOM_FEATURE
enum rtw_hal_status
rtw_hal_bb_set_pop_en(struct hal_info_t *hal_info,
                      bool en,
                      enum phl_phy_idx phy_idx);

bool
rtw_hal_bb_query_pop_en(struct hal_info_t *hal_info,
                        enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_set_pkt_detect_thold(struct hal_info_t *hal_info,
				enum phl_band_idx band_idx, u32 bound);

u8
rtw_hal_bb_query_pkt_detect_thold(struct hal_info_t *hal_info,
                                  bool get_en_info,
                                  enum phl_phy_idx phy_idx);
#endif
#ifdef CONFIG_RTW_ACS
void rtw_hal_bb_acs_mntr_trigger(struct hal_info_t *hal_info,
				enum phl_band_idx band_idx, struct acs_mntr_parm *parm);
enum rtw_hal_status rtw_hal_bb_acs_mntr_result(struct hal_info_t *hal_info,
					enum phl_band_idx band_idx, struct acs_mntr_rpt *rpt);
#endif /* CONFIG_RTW_ACS */
#ifdef CONFIG_PHL_DFS
enum rtw_hal_status
rtw_hal_bb_dfs_rpt_cfg(struct hal_info_t *hal_info, bool dfs_en);
bool
rtw_hal_bb_radar_detect(struct hal_info_t *hal_info,
			struct hal_dfs_rpt *hal_dfs);
#endif

#ifdef CONFIG_PHL_CHANNEL_INFO
enum rtw_hal_status
rtw_hal_bb_ch_info_parsing(struct hal_info_t *hal_info,
	u8 *addr, struct rtw_r_meta_data *mdata,
	u8 *rpt_buf, struct ch_rpt_hdr_info *ch_hdr_rpt,
	struct phy_info_rpt *phy_rpt, struct ch_info_drv_rpt *drv);

void
rtw_hal_bb_chan_info_cfg(struct hal_info_t *hal_info, struct chinfo_bbcr_cfg *bbcr);

void
rtw_hal_bb_ch_info_status_en(struct hal_info_t *hal_info, bool en, enum phl_phy_idx phy_idx);

void
rtw_hal_bb_ch_info_physts_en(struct hal_info_t *hal_info, bool en,
					enum phl_phy_idx phy_idx, enum wlan_mode wmode, bool ack_mode);

enum rtw_hal_status
rtw_hal_bb_ch_info_decision(struct hal_info_t *hal_info, enum phl_phy_idx phy_idx,
								enum channel_width bw, enum wlan_mode wmode,
								bool *valid_ch_info_physts);

void
rtw_hal_bb_ch_info_get_physts(struct hal_info_t *hal_info,
							u8 *rpt_buf, struct ch_rpt_hdr_info *ch_hdr_rpt,
							struct phy_info_rpt *phy_rpt, struct ch_info_drv_rpt *drv);

void
rtw_hal_bb_ch_trig_select(struct hal_info_t *hal_info, u8 event);
#endif /* CONFIG_PHL_CHANNEL_INFO */

enum rtw_hal_status rtw_hal_bb_get_rx_ok(struct hal_info_t *hal_info, u8 cur_phy_idx, u32 *rx_ok);

enum rtw_hal_status rtw_hal_bb_get_rx_crc(struct hal_info_t *hal_info, u8 cur_phy_idx, u32 *rx_crc_err);

enum rtw_hal_status rtw_hal_bb_set_reset_cnt(void *hal);

enum rtw_hal_status rtw_hal_bb_set_pwr_index(void *hal, u16 pwr_idx, enum rf_path tx_path, bool is_cck);

enum rtw_hal_status rtw_hal_bb_get_pwr_index(void *hal, u16 *pwr_idx, enum rf_path tx_path, bool is_cck);

enum rtw_hal_status
rtw_hal_bb_parse_phy_sts(void *hal, void *ppdu_sts,
			 struct rtw_phl_rx_pkt *phl_rx, u8 is_su);

enum rtw_hal_status rtw_hal_bb_get_tx_ok(void *hal, u8 cur_phy_idx, u32 *tx_ok);
enum rtw_hal_status rtw_hal_bb_dump_tx_sts(struct hal_info_t *hal_info, bool en, u8 phy_idx);

enum rtw_hal_status rtw_hal_bb_get_txpwr_ref(struct hal_info_t *hal_info, u8 is_cck, u8 tx_path, s16 *txpwr_ref);

enum rtw_hal_status rtw_hal_bb_get_rssi(struct hal_info_t *hal_info, enum rf_path rx_path, u8 *rssi);

enum rtw_hal_status rtw_hal_bb_get_rssi_ex(struct hal_info_t *hal_info, s8 *rssi_ex,
						enum rf_path rx_path, u8 nss, u8 cur_phy_idx);

enum rtw_hal_status rtw_hal_bb_get_rxevm(struct hal_info_t *hal_info, u8 user, u8 strm, u8 rxevm_table, u8 *rx_evm);

enum rtw_hal_status rtw_hal_bb_trigger_rxevm(struct hal_info_t *hal_info,
                                             u32 *phy_user0_rxevm,
                                             u32 *phy_user1_rxevm,
                                             u32 *phy_user2_rxevm,
                                             u32 *phy_user3_rxevm,
                                             u8 rx_phy_idx);

enum rtw_hal_status rtw_hal_bb_set_txsc(struct hal_info_t *hal_info, u8 txsc,
										enum phl_phy_idx phy_idx);
u32 rtw_hal_bb_process_c2h(void *hal, struct rtw_c2h_info *c2h, struct c2h_evt_msg *c2h_msg);


enum rtw_hal_status
rtw_hal_bb_get_txinfo_power(struct hal_info_t *hal_info,
				s16	 *txinfo_power_dbm);

enum rtw_hal_status
rtw_hal_bb_ctrl_rf_mode(struct hal_info_t *hal_info,
					enum phl_rf_mode rf_mode);

enum rtw_hal_status
rtw_hal_bb_set_sta_id(struct hal_info_t *hal_info,
	u16 staid, enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_set_bss_color(struct hal_info_t *hal_info,
	u8 bsscolor, enum phl_phy_idx phy_idx);

#ifdef RTW_WKARD_DEF_CMACTBL_CFG
enum rtw_hal_status
rtw_hal_bb_trx_path_cfg(struct hal_info_t *hal_info,
	enum rf_path tx, u8 tx_nss, enum rf_path rx, u8 rx_nss);

u16 rtw_hal_bb_cfg_cmac_tx_ant(struct hal_info_t *hal_info,
	enum rf_path tx_path);
#endif

enum rtw_hal_status
rtw_hal_bb_cfg_config_cmac_tbl(struct hal_info_t *hal_i,
			struct rtw_phl_stainfo_t *phl_sta,
			struct rtw_hal_mac_ax_cctl_info *cctrl,
			struct rtw_hal_mac_ax_cctl_info *cctl_info_mask);

#ifdef RTW_WKARD_DYNAMIC_BFEE_CAP
void rtw_hal_bb_dcr_en(struct hal_info_t *hal_info, bool en);
bool rtw_hal_bb_csi_rsp(struct hal_info_t *hal_info);
#endif

void rtw_hal_bb_notification(struct hal_info_t *hal_info,
                             enum phl_msg_evt_id event,
                             enum phl_phy_idx phy_idx);

void rtw_hal_bb_cmd_notification(struct hal_info_t *hal_info,
                             void *hal_cmd,
                             enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_set_gain_offset(struct hal_info_t *hal_info, s8 rx_gain_offset,
				enum rf_path rx_path, enum phl_phy_idx phy_idx, u8 iscck);

void rtw_hal_bb_get_efuse_init(struct rtw_hal_com_t *hal_com);

enum rtw_hal_status
rtw_hal_bb_ic_hw_setting_init(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_bb_loop_bck_en(struct hal_info_t *hal_info, u8 enable, u8 is_dgt_mode,
						enum rf_path tx_path, enum rf_path rx_path,
		   				enum channel_width bw, enum phl_phy_idx phy_idx,
						u8 is_cck);

bool rtw_hal_bb_check_tx_idle(struct hal_info_t *hal_info, enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_bb_query_rainfo(void *hal, struct rtw_hal_stainfo_t *hal_sta,
			struct rtw_phl_rainfo *phl_rainfo);

enum rtw_hal_status
rtw_hal_bb_query_txsts_rpt(struct hal_info_t *hal_info,
				u16 macid0, u16 macid1);

enum rtw_hal_status
rtw_hal_bb_set_tb_pwr_ofst(struct hal_info_t *hal_info,
			s16 ofst, enum phl_phy_idx phy_idx);

u8 rtw_hal_bb_drv_info_dbm2rssi(struct hal_info_t *hal_info,
				 u16 dbm, enum channel_width bw);

#ifdef CONFIG_MCC_SUPPORT
enum rtw_hal_status
rtw_hal_bb_upd_mcc_macid(struct hal_info_t *hal_info,
                         struct rtw_phl_mcc_role *mrole);

void
rtw_hal_bb_mcc_stop(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_bb_mcc_start(struct hal_info_t *hal_info,
                     struct rtw_phl_mcc_role *m_role1,
                     struct rtw_phl_mcc_role *m_role2);
#endif
#endif /*_HAL_API_BB_H_*/
