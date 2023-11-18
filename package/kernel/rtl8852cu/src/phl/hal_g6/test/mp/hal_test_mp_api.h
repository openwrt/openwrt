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
#ifndef _HAL_TEST_MP_H_
#define _HAL_TEST_MP_H_

#ifdef CONFIG_HAL_TEST_MP
enum rtw_hal_status rtw_hal_mp_cal_trigger(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_cal_set_capability(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_cal_get_capability(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_cal_get_tssi_de(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_cal_set_tssi_de(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_cal_get_txpwr_final_abs(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_cal_trigger_dpk_tracking(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_set_tssi_avg(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_event_trigger(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_trigger_watchdog_cal(
	struct mp_context *mp);
/* PSD */
enum rtw_hal_status rtw_hal_mp_psd_init(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_psd_restore(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_psd_get_point_data(
	struct mp_context *mp, struct mp_cal_arg *arg);
enum rtw_hal_status rtw_hal_mp_psd_query(
	struct mp_context *mp, struct mp_cal_arg *arg);

enum rtw_hal_status rtw_hal_mp_config_start_dut(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_get_dev_info(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_set_phy_idx(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_get_bw_mode(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_set_trx_path(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_reset_phy_cnt(
	struct mp_context *mp);
enum rtw_hal_status rtw_hal_mp_config_reset_mac_cnt(
	struct mp_context *mp);
enum rtw_hal_status rtw_hal_mp_config_reset_drv_cnt(
	struct mp_context *mp);
enum rtw_hal_status rtw_hal_mp_config_set_modulation(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_get_modulation(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_set_trx_mode(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_set_rate(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_get_mac_addr(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_set_mac_addr(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_set_ch_bw(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_config_swith_btc_path(
	struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_trigger_fw_conflict(struct mp_context *mp, struct mp_config_arg *arg);
enum rtw_hal_status rtw_hal_mp_bb_loop_bck(struct mp_context *mp, struct mp_tx_arg *arg);
enum rtw_hal_status rtw_hal_mp_ic_hw_setting_init(
	struct mp_context *mp);
u32 rtw_hal_get_uuid(struct mp_context *mp);
void rtw_hal_set_regulation(
	struct mp_context *mp,struct mp_config_arg *arg);
void rtw_hal_set_bt_uart_en(struct mp_context *mp, struct mp_config_arg *arg);
/* WIFI EFUSE */
enum rtw_hal_status rtw_hal_mp_efuse_wifi_shadow_read(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_wifi_shadow_write(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_wifi_shadow_update(
	struct mp_context *mp);
enum rtw_hal_status rtw_hal_mp_efuse_wifi_shadow_reload(
	struct mp_context *mp);
enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_offset_mask(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_usage(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_log_size(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_size(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_get_avl_size(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_get_shadowmap_from_val(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_autoload_status(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_shadow2buf(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_file_map_load(
	struct mp_context *mp, char *pfilepath);
enum rtw_hal_status rtw_hal_mp_efuse_file_mask_load(
	struct mp_context *mp, char *pfilepath);
enum rtw_hal_status rtw_hal_mp_efuse_get_info(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_mask_buf(
	struct mp_context *mp, struct mp_efuse_arg *arg);

/* BT EFUSE */
enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow_read(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow_write(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow_update(
	struct mp_context *mp);
enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow_reload(
	struct mp_context *mp);
enum rtw_hal_status rtw_hal_mp_efuse_bt_get_offset_mask(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_get_usage(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_get_log_size(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_get_size(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_get_avl_size(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow2buf(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_file_map_load(
	struct mp_context *mp, char *pfilepath);
enum rtw_hal_status rtw_hal_mp_efuse_bt_file_mask_load(
	struct mp_context *mp, char *pfilepath);
enum rtw_hal_status rtw_hal_mp_efuse_bt_read_hidden(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_write_hidden(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_bt_get_mask_buf(
	struct mp_context *mp, struct mp_efuse_arg *arg);
enum rtw_hal_status rtw_hal_mp_efuse_read_phy_map(
	struct mp_context *mp, struct mp_efuse_arg *arg, u8 type);
enum rtw_hal_status rtw_hal_mp_efuse_renew(
	struct mp_context *mp, struct mp_efuse_arg *arg, u8 type);

enum rtw_hal_status rtw_hal_mp_reg_read_macreg(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_write_macreg(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_read_rfreg(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_write_rfreg(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_read_rf_syn(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_write_rf_syn(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_read_bbreg(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_write_bbreg(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_get_xcap(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_set_xcap(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_get_xsi(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_reg_set_xsi(
	struct mp_context *mp, struct mp_reg_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_phy_crc_ok(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_phy_crc_err(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_mac_crc_ok(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_mac_crc_err(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_drv_crc_ok(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_drv_crc_err(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_get_rssi(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_get_rssi_ex(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_get_rxevm(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_trigger_rxevm(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_rx_set_gain_offset(
	struct mp_context *mp, struct mp_rx_arg *arg);
enum rtw_hal_status rtw_hal_mp_tx_plcp_gen(
	struct mp_context *mp, struct mp_tx_arg *arg,struct mp_plcp_param_t *plcp_tx_struct);
enum rtw_hal_status rtw_hal_mp_tx_pmac_packet(
	struct mp_context *mp, struct mp_tx_arg *arg);
enum rtw_hal_status rtw_hal_mp_tx_pmac_continuous(
	struct mp_context *mp, struct mp_tx_arg *arg);
enum rtw_hal_status rtw_hal_mp_tx_pmac_fw_trigger(
	struct mp_context *mp, struct mp_tx_arg *arg);
enum rtw_hal_status rtw_hal_mp_tx_single_tone(
	struct mp_context *mp, struct mp_tx_arg *arg);
enum rtw_hal_status rtw_hal_mp_tx_carrier_suppression(
	struct mp_context *mp, struct mp_tx_arg *arg);
enum rtw_hal_status rtw_hal_mp_tx_phy_ok_cnt(
	struct mp_context *mp, struct mp_tx_arg *arg);
void rtw_hal_mp_check_tx_idle(
	struct mp_context *mp, struct mp_tx_arg *arg);
enum rtw_hal_status rtw_hal_mp_set_dpd_bypass(
	struct mp_context *mp, struct mp_tx_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_read_table(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_get_pwrtrack(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_set_pwrtrack(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_set_pwr(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_get_pwr(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_get_pwr_idx(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_set_pwr_idx(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_get_thermal(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_set_tssi(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_get_tssi(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_get_online_tssi_de(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_set_pwr_lmt_en(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_get_pwr_lmt_en(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_set_tssi_offset(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_get_pwr_ref(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_get_pwr_ref_cw(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_get_txinfo_pwr(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
enum rtw_hal_status rtw_hal_mp_txpwr_ctrl_rf_mode(
	struct mp_context *mp, struct mp_txpwr_arg *arg);
void rtw_hal_mp_set_tx_pow_patten_sharp(
	struct mp_context *mp, struct mp_txpwr_arg *arg);

void rtw_hal_mp_cfg(struct rtw_phl_com_t *phl_com, void *hal);
#endif /* CONFIG_HAL_TEST_MP */

#endif /* _HAL_TEST_MP_H_ */
