/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup
 * @{
 */
#ifndef _SFP_H_
#define _SFP_H_


sw_error_t
sfp_eeprom_data_get(a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t i2c_slave,
		a_uint32_t offset, a_uint8_t *buf, a_uint32_t counter);

sw_error_t
sfp_eeprom_data_set(a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t i2c_slave,
		a_uint32_t offset, a_uint8_t *buf, a_uint32_t counter);

sw_error_t
sfp_dev_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_dev_type_u *value);

sw_error_t
sfp_dev_type_ext_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_dev_type_ext_u *value);

sw_error_t
sfp_dev_connector_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_dev_connector_type_u *value);

sw_error_t
sfp_transc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_transc_u *value);

sw_error_t
sfp_encoding_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_encoding_u *value);

sw_error_t
sfp_br_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_br_u *value);

sw_error_t
sfp_rate_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_rate_u *value);

sw_error_t
sfp_link_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_link_len_u *value);

sw_error_t
sfp_vendor_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_vendor_u *value);

sw_error_t
sfp_laser_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_laser_u *value);

sw_error_t
sfp_base_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_base_u *value);

sw_error_t
sfp_option_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_option_u *value);

sw_error_t
sfp_rate_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_rate_ctrl_u *value);

sw_error_t
sfp_vendor_ext_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_vendor_ext_u *value);

sw_error_t
sfp_enhanced_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_enhanced_u *value);

sw_error_t
sfp_ext_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_ext_u *value);

sw_error_t
sfp_dev_type_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_dev_type_ext_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_dev_connector_type_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_sonet_ccode_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_fiber_ch_tech_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_sonet_ccode_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_fiber_ch_speed_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_fiber_ch_tech_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_cable_tech_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_fiber_ch_link_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_unallocated_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_fiber_chan_tm_media_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_escon_ccode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_infiniband_ccode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_eth_ccode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_transc_eth_10g_ccode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_encoding_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_br_bit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_rate_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_link_len_om3_mode_1m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_link_len_single_mode_100m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_link_len_om2_mode_10m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_link_len_copper_mode_1m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_link_len_om1_mode_10m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_link_len_single_mode_km_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_vendor_rev_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint8_t *value);

sw_error_t
sfp_vendor_name_get(
		a_uint32_t dev_id, a_uint32_t index,
		a_uint8_t *value);

sw_error_t
sfp_vendor_oui_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint8_t *value);

sw_error_t
sfp_vendor_pn_get(
		a_uint32_t dev_id, a_uint32_t index,
		a_uint8_t *value);

sw_error_t
sfp_laser_wavelength_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_base_check_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_linear_recv_output_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_pwr_level_declar_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_unallocated_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_unallocated_3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_loss_signal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_rate_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_unallocated_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_loss_invert_signal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_tx_disable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_cool_transc_declar_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_option_tx_fault_signal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_rate_ctrl_upper_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_rate_ctrl_lower_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_vendor_ext_date_code_get(
		a_uint32_t dev_id, a_uint32_t index,
		a_uint8_t *value);

sw_error_t
sfp_vendor_ext_sn_get(
		a_uint32_t dev_id, a_uint32_t index,
		a_uint8_t *value);

sw_error_t
sfp_enhanced_diag_mon_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_rx_los_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_cmpl_feature_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_tx_disable_ctrl_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_alarm_warning_flag_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_addr_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_unallocated_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_soft_rate_sel_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_external_cal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_internal_cal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_re_pwr_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_soft_rate_ctrl_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_app_sel_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_legacy_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_tx_fault_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_enhanced_unallocated_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_ext_check_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_threshold_u *value);

sw_error_t
sfp_diag_cal_const_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_cal_const_u *value);

sw_error_t
sfp_diag_dmi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_dmi_u *value);

sw_error_t
sfp_diag_realtime_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_realtime_u *value);

sw_error_t
sfp_diag_optional_ctrl_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_optional_ctrl_status_u *value);

sw_error_t
sfp_diag_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_flag_u *value);

sw_error_t
sfp_diag_extended_ctrl_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_extended_ctrl_status_u *value);

sw_error_t
sfp_diag_threshold_rx_pwr_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_rx_pwr_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_temp_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_vol_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_tx_pwr_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_bias_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_bias_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_vol_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_bias_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_temp_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_rx_pwr_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_vol_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_tx_pwr_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_bias_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_temp_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_tx_pwr_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_vol_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_temp_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_rx_pwr_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_threshold_tx_pwr_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_cal_const_rx_pwr_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_cal_const_t_slope_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_cal_const_rx_pwr_3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_cal_const_rx_pwr_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_cal_const_tx_i_slope_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_cal_const_v_slope_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_cal_const_tx_pwr_slope_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_cal_const_rx_pwr_4_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_cal_const_rx_pwr_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_dmi_check_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_realtime_vcc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_realtime_tx_pwr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_realtime_tx_bias_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_realtime_rx_pwr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_realtime_tmp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_optional_ctrl_status_rs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_optional_ctrl_status_tx_fault_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_optional_ctrl_status_rx_los_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_optional_ctrl_status_data_ready_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_optional_ctrl_status_soft_rate_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_optional_ctrl_status_soft_tx_disable_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_optional_ctrl_status_rate_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_optional_ctrl_status_tx_disable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_rx_pwr_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_rx_pwr_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tx_bias_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tmp_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tmp_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tx_pwr_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_vcc_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_vcc_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_rx_pwr_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_unallocated_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tx_bias_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_vcc_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tx_pwr_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_vcc_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tmp_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tx_bias_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tx_bias_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tmp_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tx_pwr_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_rx_pwr_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_flag_tx_pwr_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_extended_ctrl_status_unallocated_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_extended_ctrl_status_pwr_level_op_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_extended_ctrl_status_soft_rs_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

sw_error_t
sfp_diag_extended_ctrl_status_pwr_level_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		unsigned int *value);

#endif
