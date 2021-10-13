/*
 * Copyright (c) 2015, 2017-2021, The Linux Foundation. All rights reserved.
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
/*qca808x_start*/
#ifndef _HSL_PHY_H_
#define _HSL_PHY_H_

#ifdef __cplusplus
extern "C" {
#endif				/* __cplusplus */

#include "fal.h"
#include <linux/version.h>

	/** Phy function reset type */
	typedef enum {
		PHY_FIFO_RESET = 0,	/**< Phy fifo reset */
	} hsl_phy_function_reset_t;

	typedef sw_error_t(*hsl_phy_init) (a_uint32_t dev_id,
					   a_uint32_t phy_id);
	typedef sw_error_t(*hsl_phy_hibernation_set) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      a_bool_t enable);
	typedef sw_error_t(*hsl_phy_hibernation_get) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      a_bool_t * enable);
	typedef sw_error_t(*hsl_phy_speed_get) (a_uint32_t dev_id,
						a_uint32_t phy_id,
						fal_port_speed_t * speed);
	typedef sw_error_t(*hsl_phy_speed_set) (a_uint32_t dev_id,
						a_uint32_t phy_id,
						fal_port_speed_t speed);
	typedef sw_error_t(*hsl_phy_duplex_get) (a_uint32_t dev_id,
						 a_uint32_t phy_id,
						 fal_port_duplex_t * duplex);
	typedef sw_error_t(*hsl_phy_duplex_set) (a_uint32_t dev_id,
						 a_uint32_t phy_id,
						 fal_port_duplex_t duplex);
	typedef sw_error_t(*hsl_phy_autoneg_enable_set) (a_uint32_t dev_id,
							 a_uint32_t phy_id);
	typedef sw_error_t(*hsl_phy_autoneg_enable_get) (a_uint32_t dev_id,
							 a_uint32_t phy_id,
							 a_bool_t * enable);
	typedef sw_error_t(*hsl_phy_restart_autoneg) (a_uint32_t dev_id,
						      a_uint32_t phy_id);
	typedef a_bool_t(*hsl_phy_autoneg_status_get) (a_uint32_t dev_id,
							 a_uint32_t phy_id);
	typedef sw_error_t(*hsl_phy_powersave_set) (a_uint32_t dev_id,
						    a_uint32_t phy_id,
						    a_bool_t enable);
	typedef sw_error_t(*hsl_phy_powersave_get) (a_uint32_t dev_id,
						    a_uint32_t phy_id,
						    a_bool_t * enable);
	typedef sw_error_t(*hsl_phy_cdt) (a_uint32_t dev_id, a_uint32_t phy_id,
					  a_uint32_t mdi_pair,
					  fal_cable_status_t * cable_status,
					  a_uint32_t * cable_len);
	typedef a_bool_t(*hsl_phy_link_status_get) (a_uint32_t dev_id,
						      a_uint32_t phy_id);
	typedef sw_error_t(*hsl_phy_get_ability) (a_uint32_t dev_id,
						  a_uint32_t phy_id,
						  a_uint32_t * ability);
	typedef sw_error_t(*hsl_phy_mdix_set) (a_uint32_t dev_id,
					       a_uint32_t phy_id,
					       fal_port_mdix_mode_t mode);
	typedef sw_error_t(*hsl_phy_mdix_get) (a_uint32_t dev_id,
					       a_uint32_t phy_id,
					       fal_port_mdix_mode_t * mode);
	typedef sw_error_t(*hsl_phy_mdix_status_get) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      fal_port_mdix_status_t *
						      mode);
	typedef sw_error_t(*hsl_phy_8023az_set) (a_uint32_t dev_id,
						 a_uint32_t phy_id,
						 a_bool_t enable);
	typedef sw_error_t(*hsl_phy_8023az_get) (a_uint32_t dev_id,
						 a_uint32_t phy_id,
						 a_bool_t * enable);
	typedef sw_error_t(*hsl_phy_local_loopback_set) (a_uint32_t dev_id,
							 a_uint32_t phy_id,
							 a_bool_t enable);
	typedef sw_error_t(*hsl_phy_local_loopback_get) (a_uint32_t dev_id,
							 a_uint32_t phy_id,
							 a_bool_t * enable);
	typedef sw_error_t(*hsl_phy_remote_loopback_set) (a_uint32_t dev_id,
							  a_uint32_t phy_id,
							  a_bool_t enable);
	typedef sw_error_t(*hsl_phy_remote_loopback_get) (a_uint32_t dev_id,
							  a_uint32_t phy_id,
							  a_bool_t * enable);
	typedef sw_error_t(*hsl_phy_master_set) (a_uint32_t dev_id,
						 a_uint32_t phy_id,
						 fal_port_master_t master);
	typedef sw_error_t(*hsl_phy_master_get) (a_uint32_t dev_id,
						 a_uint32_t phy_id,
						 fal_port_master_t * master);
/*qca808x_end*/
	typedef sw_error_t(*hsl_phy_combo_prefer_medium_set) (a_uint32_t dev_id,
							      a_uint32_t phy_id,
							      fal_port_medium_t
							      phy_medium);
	typedef sw_error_t(*hsl_phy_combo_prefer_medium_get) (a_uint32_t dev_id,
							      a_uint32_t phy_id,
							      fal_port_medium_t
							      * phy_medium);
	typedef sw_error_t(*hsl_phy_combo_medium_status_get) (a_uint32_t dev_id,
							      a_uint32_t phy_id,
							      fal_port_medium_t
							      * phy_medium);
	typedef sw_error_t(*hsl_phy_combo_fiber_mode_set) (a_uint32_t dev_id,
							   a_uint32_t phy_id,
							   fal_port_fiber_mode_t
							   fiber_mode);
	typedef sw_error_t(*hsl_phy_combo_fiber_mode_get) (a_uint32_t dev_id,
							   a_uint32_t phy_id,
							   fal_port_fiber_mode_t
							   * fiber_mode);
	typedef sw_error_t (*hsl_phy_function_reset) (a_uint32_t dev_id,
							   a_uint32_t phy_id,
							   hsl_phy_function_reset_t
							   phy_reset_type);
/*qca808x_start*/
	typedef sw_error_t(*hsl_phy_reset) (a_uint32_t dev_id,
					    a_uint32_t phy_id);
	typedef sw_error_t(*hsl_phy_reset_status_get) (a_uint32_t dev_id,
						       a_uint32_t phy_id,
						       fal_port_reset_status_t *
						       status);
	typedef sw_error_t(*hsl_phy_power_off) (a_uint32_t dev_id,
					    a_uint32_t phy_id);
	typedef sw_error_t(*hsl_phy_power_on) (a_uint32_t dev_id,
					    a_uint32_t phy_id);
	typedef sw_error_t(*hsl_phy_id_get) (a_uint32_t dev_id,
					     a_uint32_t phy_id,
					     a_uint32_t *phy_chip_id);
	typedef sw_error_t(*hsl_phy_autoneg_adv_set) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      a_uint32_t autoneg);
	typedef sw_error_t(*hsl_phy_autoneg_adv_get) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      a_uint32_t * autoneg);
	typedef sw_error_t(*hsl_phy_reg_write) (a_uint32_t dev_id,
						a_uint32_t phy_addr,
						a_uint32_t reg,
						a_uint16_t reg_val);
	typedef a_uint16_t(*hsl_phy_reg_read) (a_uint32_t dev_id,
					       a_uint32_t phy_addr,
					       a_uint32_t reg);
	typedef sw_error_t(*hsl_phy_debug_write) (a_uint32_t dev_id,
						  a_uint32_t phy_id,
						  a_uint16_t reg_id,
						  a_uint16_t reg_val);
	typedef a_uint16_t(*hsl_phy_debug_read) (a_uint32_t dev_id,
						 a_uint32_t phy_id,
						 a_uint16_t reg_id);
	typedef sw_error_t(*hsl_phy_mmd_write) (a_uint32_t dev_id,
						a_uint32_t phy_id,
						a_uint16_t mmd_num,
						a_uint16_t reg_id,
						a_uint16_t reg_val);
	typedef a_uint16_t(*hsl_phy_mmd_read) (a_uint32_t dev_id,
					       a_uint32_t phy_id,
					       a_uint16_t mmd_num,
					       a_uint16_t reg_id);

	typedef sw_error_t(*hsl_phy_magic_frame_mac_set) (a_uint32_t dev_id,
							  a_uint32_t phy_id,
							  fal_mac_addr_t * mac);

	typedef sw_error_t(*hsl_phy_magic_frame_mac_get) (a_uint32_t dev_id,
							  a_uint32_t phy_id,
							  fal_mac_addr_t * mac);
	typedef sw_error_t(*hsl_phy_wol_status_set) (a_uint32_t dev_id,
							 a_uint32_t phy_id,
							 a_bool_t enable);
	typedef sw_error_t(*hsl_phy_wol_status_get) (a_uint32_t dev_id,
							 a_uint32_t phy_id,
							 a_bool_t * enable);
	typedef sw_error_t(*hsl_phy_interface_mode_set) (a_uint32_t dev_id,
							 a_uint32_t phy_id,
							 fal_port_interface_mode_t
							 interface_mode);
	typedef sw_error_t(*hsl_phy_interface_mode_get) (a_uint32_t dev_id,
							 a_uint32_t phy_id,
							 fal_port_interface_mode_t
							 * interface_mode);
	typedef sw_error_t(*hsl_phy_interface_mode_status_get) (a_uint32_t dev_id,
							 a_uint32_t phy_id,
							 fal_port_interface_mode_t
							 * interface_mode);
	typedef sw_error_t(*hsl_phy_intr_mask_set) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      a_uint32_t mask);
	typedef sw_error_t(*hsl_phy_intr_mask_get) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      a_uint32_t * mask);
	typedef sw_error_t(*hsl_phy_intr_status_get) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      a_uint32_t * status);
	typedef sw_error_t(*hsl_phy_counter_set) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      a_bool_t enable);
	typedef sw_error_t(*hsl_phy_counter_get) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      a_bool_t * enable);
	typedef sw_error_t(*hsl_phy_counter_show) (a_uint32_t dev_id,
						      a_uint32_t phy_id,
						      fal_port_counter_info_t * counter_info);
	typedef sw_error_t(*hsl_phy_serdes_reset) (a_uint32_t dev_id);

	typedef sw_error_t(*hsl_phy_get_status) (a_uint32_t dev_id,
				a_uint32_t phy_id, struct port_phy_status *phy_status);

	typedef sw_error_t(*hsl_phy_eee_adv_set) (a_uint32_t dev_id,
					      a_uint32_t phy_id,
					      a_uint32_t adv);
	typedef sw_error_t(*hsl_phy_eee_adv_get) (a_uint32_t dev_id,
					      a_uint32_t phy_id,
					      a_uint32_t * adv);
	typedef sw_error_t(*hsl_phy_eee_partner_adv_get) (a_uint32_t dev_id,
					      a_uint32_t phy_id,
					      a_uint32_t * adv);
	typedef sw_error_t(*hsl_phy_eee_cap_get) (a_uint32_t dev_id,
					      a_uint32_t phy_id,
					      a_uint32_t * cap);
	typedef sw_error_t(*hsl_phy_eee_status_get) (a_uint32_t dev_id,
					      a_uint32_t phy_id,
					      a_uint32_t * status);
/*qca808x_end*/
	typedef sw_error_t(*hsl_phy_led_ctrl_pattern_set) (a_uint32_t dev_id,
					      a_uint32_t phy_id,
					      led_ctrl_pattern_t * pattern);
	typedef sw_error_t(*hsl_phy_led_ctrl_pattern_get) (a_uint32_t dev_id,
					      a_uint32_t phy_id,
					      led_ctrl_pattern_t * pattern);
	typedef sw_error_t(*hsl_phy_led_ctrl_source_set) (a_uint32_t dev_id,
					      a_uint32_t phy_id,
					      a_uint32_t source_id,
					      led_ctrl_pattern_t * pattern);
	typedef sw_error_t(*hsl_phy_ptp_security_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_security_t *sec);

	typedef sw_error_t(*hsl_phy_ptp_link_delay_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_rx_crc_recalc_status_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_bool_t *status);

	typedef sw_error_t(*hsl_phy_ptp_tod_uart_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_tod_uart_t *tod_uart);

	typedef sw_error_t(*hsl_phy_ptp_enhanced_timestamp_engine_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_direction_t direction,
				fal_ptp_enhanced_ts_engine_t *ts_engine);

	typedef sw_error_t(*hsl_phy_ptp_pps_signal_control_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_pps_signal_control_t *sig_control);

	typedef sw_error_t(*hsl_phy_ptp_timestamp_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_direction_t direction,
				fal_ptp_pkt_info_t *pkt_info, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_asym_correction_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_asym_correction_t* asym_cf);

	typedef sw_error_t(*hsl_phy_ptp_rtc_time_snapshot_status_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_bool_t *status);

	typedef sw_error_t(*hsl_phy_ptp_capture_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_uint32_t capture_id,
				fal_ptp_capture_t *capture);

	typedef sw_error_t(*hsl_phy_ptp_rtc_adjfreq_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_asym_correction_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_asym_correction_t *asym_cf);

	typedef sw_error_t(*hsl_phy_ptp_pkt_timestamp_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_rtc_time_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_rtc_time_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_pkt_timestamp_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_interrupt_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_interrupt_t *interrupt);

	typedef sw_error_t(*hsl_phy_ptp_trigger_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_uint32_t trigger_id,
				fal_ptp_trigger_t *triger);

	typedef sw_error_t(*hsl_phy_ptp_pps_signal_control_get) (a_uint32_t dev_id,
				a_uint32_t phy_id,
				fal_ptp_pps_signal_control_t *sig_control);

	typedef sw_error_t(*hsl_phy_ptp_capture_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_uint32_t capture_id,
				fal_ptp_capture_t *capture);

	typedef sw_error_t(*hsl_phy_ptp_rx_crc_recalc_enable) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_bool_t status);

	typedef sw_error_t(*hsl_phy_ptp_security_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_security_t *sec);

	typedef sw_error_t(*hsl_phy_ptp_increment_sync_from_clock_status_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_bool_t *status);

	typedef sw_error_t(*hsl_phy_ptp_tod_uart_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_tod_uart_t *tod_uart);

	typedef sw_error_t(*hsl_phy_ptp_enhanced_timestamp_engine_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_direction_t direction,
				fal_ptp_enhanced_ts_engine_t *ts_engine);

	typedef sw_error_t(*hsl_phy_ptp_rtc_time_clear) (a_uint32_t dev_id,
				a_uint32_t phy_id);

	typedef sw_error_t(*hsl_phy_ptp_reference_clock_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_reference_clock_t ref_clock);

	typedef sw_error_t(*hsl_phy_ptp_output_waveform_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_output_waveform_t *waveform);

	typedef sw_error_t(*hsl_phy_ptp_rx_timestamp_mode_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_rx_timestamp_mode_t ts_mode);

	typedef sw_error_t(*hsl_phy_ptp_grandmaster_mode_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_grandmaster_mode_t *gm_mode);

	typedef sw_error_t(*hsl_phy_ptp_config_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_config_t *config);

	typedef sw_error_t(*hsl_phy_ptp_trigger_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_uint32_t trigger_id,
				fal_ptp_trigger_t *triger);

	typedef sw_error_t(*hsl_phy_ptp_rtc_adjfreq_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_grandmaster_mode_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_grandmaster_mode_t *gm_mode);

	typedef sw_error_t(*hsl_phy_ptp_rx_timestamp_mode_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_rx_timestamp_mode_t *ts_mode);

	typedef sw_error_t(*hsl_phy_ptp_rtc_adjtime_set) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_link_delay_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_time_t *time);

	typedef sw_error_t(*hsl_phy_ptp_increment_sync_from_clock_enable) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_bool_t status);

	typedef sw_error_t(*hsl_phy_ptp_config_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_config_t *config);

	typedef sw_error_t(*hsl_phy_ptp_output_waveform_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_output_waveform_t *waveform);

	typedef sw_error_t(*hsl_phy_ptp_interrupt_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_interrupt_t *interrupt);

	typedef sw_error_t(*hsl_phy_ptp_rtc_time_snapshot_enable) (a_uint32_t dev_id,
				a_uint32_t phy_id, a_bool_t status);

	typedef sw_error_t(*hsl_phy_ptp_reference_clock_get) (a_uint32_t dev_id,
				a_uint32_t phy_id, fal_ptp_reference_clock_t *ref_clock);

	typedef struct hsl_phy_ptp_ops_s {
		hsl_phy_ptp_security_set phy_ptp_security_set;
		hsl_phy_ptp_link_delay_set phy_ptp_link_delay_set;
		hsl_phy_ptp_rx_crc_recalc_status_get phy_ptp_rx_crc_recalc_status_get;
		hsl_phy_ptp_tod_uart_set phy_ptp_tod_uart_set;
		hsl_phy_ptp_enhanced_timestamp_engine_get phy_ptp_enhanced_timestamp_engine_get;
		hsl_phy_ptp_pps_signal_control_set phy_ptp_pps_signal_control_set;
		hsl_phy_ptp_timestamp_get phy_ptp_timestamp_get;
		hsl_phy_ptp_asym_correction_get phy_ptp_asym_correction_get;
		hsl_phy_ptp_rtc_time_snapshot_status_get phy_ptp_rtc_time_snapshot_status_get;
		hsl_phy_ptp_capture_set phy_ptp_capture_set;
		hsl_phy_ptp_rtc_adjfreq_set phy_ptp_rtc_adjfreq_set;
		hsl_phy_ptp_asym_correction_set phy_ptp_asym_correction_set;
		hsl_phy_ptp_pkt_timestamp_set phy_ptp_pkt_timestamp_set;
		hsl_phy_ptp_rtc_time_get phy_ptp_rtc_time_get;
		hsl_phy_ptp_rtc_time_set phy_ptp_rtc_time_set;
		hsl_phy_ptp_pkt_timestamp_get phy_ptp_pkt_timestamp_get;
		hsl_phy_ptp_interrupt_set phy_ptp_interrupt_set;
		hsl_phy_ptp_trigger_set phy_ptp_trigger_set;
		hsl_phy_ptp_pps_signal_control_get phy_ptp_pps_signal_control_get;
		hsl_phy_ptp_capture_get phy_ptp_capture_get;
		hsl_phy_ptp_rx_crc_recalc_enable phy_ptp_rx_crc_recalc_enable;
		hsl_phy_ptp_security_get phy_ptp_security_get;
		hsl_phy_ptp_increment_sync_from_clock_status_get \
			phy_ptp_increment_sync_from_clock_status_get;
		hsl_phy_ptp_tod_uart_get phy_ptp_tod_uart_get;
		hsl_phy_ptp_enhanced_timestamp_engine_set phy_ptp_enhanced_timestamp_engine_set;
		hsl_phy_ptp_rtc_time_clear phy_ptp_rtc_time_clear;
		hsl_phy_ptp_reference_clock_set phy_ptp_reference_clock_set;
		hsl_phy_ptp_output_waveform_set phy_ptp_output_waveform_set;
		hsl_phy_ptp_rx_timestamp_mode_set phy_ptp_rx_timestamp_mode_set;
		hsl_phy_ptp_grandmaster_mode_set phy_ptp_grandmaster_mode_set;
		hsl_phy_ptp_config_set phy_ptp_config_set;
		hsl_phy_ptp_trigger_get phy_ptp_trigger_get;
		hsl_phy_ptp_rtc_adjfreq_get phy_ptp_rtc_adjfreq_get;
		hsl_phy_ptp_grandmaster_mode_get phy_ptp_grandmaster_mode_get;
		hsl_phy_ptp_rx_timestamp_mode_get phy_ptp_rx_timestamp_mode_get;
		hsl_phy_ptp_rtc_adjtime_set phy_ptp_rtc_adjtime_set;
		hsl_phy_ptp_link_delay_get phy_ptp_link_delay_get;
		hsl_phy_ptp_increment_sync_from_clock_enable \
			phy_ptp_increment_sync_from_clock_enable;
		hsl_phy_ptp_config_get phy_ptp_config_get;
		hsl_phy_ptp_output_waveform_get phy_ptp_output_waveform_get;
		hsl_phy_ptp_interrupt_get phy_ptp_interrupt_get;
		hsl_phy_ptp_rtc_time_snapshot_enable phy_ptp_rtc_time_snapshot_enable;
		hsl_phy_ptp_reference_clock_get phy_ptp_reference_clock_get;
	} hsl_phy_ptp_ops_t;

/*qca808x_start*/
	typedef struct hsl_phy_ops_s {

		hsl_phy_init phy_init;
		hsl_phy_hibernation_set phy_hibernation_set;
		hsl_phy_hibernation_get phy_hibernation_get;
		hsl_phy_speed_get phy_speed_get;
		hsl_phy_speed_set phy_speed_set;
		hsl_phy_duplex_get phy_duplex_get;
		hsl_phy_duplex_set phy_duplex_set;
		hsl_phy_autoneg_enable_set phy_autoneg_enable_set;
		hsl_phy_autoneg_enable_get phy_autoneg_enable_get;
		hsl_phy_restart_autoneg phy_restart_autoneg;
		hsl_phy_autoneg_status_get phy_autoneg_status_get;
		hsl_phy_autoneg_adv_set phy_autoneg_adv_set;
		hsl_phy_autoneg_adv_get phy_autoneg_adv_get;
		hsl_phy_powersave_set phy_powersave_set;
		hsl_phy_powersave_get phy_powersave_get;
		hsl_phy_cdt phy_cdt;
		hsl_phy_link_status_get phy_link_status_get;
		hsl_phy_get_ability phy_get_ability;
		hsl_phy_mdix_set phy_mdix_set;
		hsl_phy_mdix_get phy_mdix_get;
		hsl_phy_mdix_status_get phy_mdix_status_get;
		hsl_phy_8023az_set phy_8023az_set;
		hsl_phy_8023az_get phy_8023az_get;
		hsl_phy_local_loopback_set phy_local_loopback_set;
		hsl_phy_local_loopback_get phy_local_loopback_get;
		hsl_phy_remote_loopback_set phy_remote_loopback_set;
		hsl_phy_remote_loopback_get phy_remote_loopback_get;
		hsl_phy_master_set phy_master_set;
		hsl_phy_master_get phy_master_get;
/*qca808x_end*/
		hsl_phy_combo_prefer_medium_set phy_combo_prefer_medium_set;
		hsl_phy_combo_prefer_medium_get phy_combo_prefer_medium_get;
		hsl_phy_combo_medium_status_get phy_combo_medium_status_get;
		hsl_phy_combo_fiber_mode_set phy_combo_fiber_mode_set;
		hsl_phy_combo_fiber_mode_get phy_combo_fiber_mode_get;
		hsl_phy_function_reset phy_function_reset;
/*qca808x_start*/
		hsl_phy_reset phy_reset;
		hsl_phy_power_off phy_power_off;
		hsl_phy_power_on phy_power_on;
		hsl_phy_reset_status_get phy_reset_status_get;
		hsl_phy_id_get phy_id_get;
		hsl_phy_reg_write phy_reg_write;
		hsl_phy_reg_read phy_reg_read;
		hsl_phy_debug_write phy_debug_write;
		hsl_phy_debug_read phy_debug_read;
		hsl_phy_mmd_write phy_mmd_write;
		hsl_phy_mmd_read phy_mmd_read;
		hsl_phy_magic_frame_mac_set phy_magic_frame_mac_set;
		hsl_phy_magic_frame_mac_get phy_magic_frame_mac_get;
		hsl_phy_wol_status_set phy_wol_status_set;
		hsl_phy_wol_status_get phy_wol_status_get;
		hsl_phy_interface_mode_set phy_interface_mode_set;
		hsl_phy_interface_mode_get phy_interface_mode_get;
		hsl_phy_interface_mode_status_get phy_interface_mode_status_get;
		hsl_phy_intr_mask_set  phy_intr_mask_set;
		hsl_phy_intr_mask_get  phy_intr_mask_get;
		hsl_phy_intr_status_get  phy_intr_status_get;
		hsl_phy_counter_set  phy_counter_set;
		hsl_phy_counter_get  phy_counter_get;
		hsl_phy_counter_show  phy_counter_show;
		hsl_phy_serdes_reset phy_serdes_reset;
		hsl_phy_get_status phy_get_status;
		hsl_phy_eee_adv_set phy_eee_adv_set;
		hsl_phy_eee_adv_get phy_eee_adv_get;
		hsl_phy_eee_partner_adv_get phy_eee_partner_adv_get;
		hsl_phy_eee_cap_get phy_eee_cap_get;
		hsl_phy_eee_status_get phy_eee_status_get;
/*qca808x_end*/
		hsl_phy_led_ctrl_pattern_set phy_led_ctrl_pattern_set;
		hsl_phy_led_ctrl_pattern_get phy_led_ctrl_pattern_get;
		hsl_phy_led_ctrl_source_set phy_led_ctrl_source_set;
		hsl_phy_ptp_ops_t phy_ptp_ops;
/*qca808x_start*/
	} hsl_phy_ops_t;

typedef struct phy_driver_instance {
	a_uint32_t phy_type;
	a_uint32_t port_bmp[SW_MAX_NR_DEV];
	hsl_phy_ops_t *phy_ops;
	int (*init)(a_uint32_t dev_id, a_uint32_t portbmp);
	void (*exit)(a_uint32_t dev_id, a_uint32_t portbmp);
} phy_driver_instance_t;

typedef enum
{
/*qca808x_end*/
	F1_PHY_CHIP = 0,
	F2_PHY_CHIP,
	MALIBU_PHY_CHIP,
	AQUANTIA_PHY_CHIP,
	QCA803X_PHY_CHIP,
	SFP_PHY_CHIP,
	MPGE_PHY_CHIP,
/*qca808x_start*/
	QCA808X_PHY_CHIP,
	MAX_PHY_CHIP,
} phy_type_t;

#define PHY_INVALID_DAC        0

typedef struct {
	a_uint8_t mdac;
	a_uint8_t edac;
} phy_dac_t;

typedef struct {
	a_uint32_t phy_address[SW_MAX_NR_PORT];
	a_uint32_t phy_type[SW_MAX_NR_PORT];
	/* fake mdio address is used to register the phy device,
	 * when the phy is not accessed by the MDIO bus.
	 * */
	a_uint32_t phy_mdio_fake_address[SW_MAX_NR_PORT];
	a_uint8_t phy_access_type[SW_MAX_NR_PORT];
	a_bool_t phy_c45[SW_MAX_NR_PORT];
	a_bool_t phy_combo[SW_MAX_NR_PORT];
	a_uint32_t phy_reset_gpio[SW_MAX_NR_PORT];
	phy_dac_t phy_dac[SW_MAX_NR_PORT];
} phy_info_t;
/*qca808x_end*/
#define MALIBU5PORT_PHY         0x004DD0B1
#define MALIBU2PORT_PHY         0x004DD0B2
#define QCA8030_PHY             0x004DD076
#define QCA8033_PHY             0x004DD074
#define QCA8035_PHY             0x004DD072
/*qca808x_start*/
#define QCA8081_PHY_V1_1        0x004DD101
#define INVALID_PHY_ID          0xFFFFFFFF

/*qca808x_end*/
#define F1V1_PHY                0x004DD033
#define F1V2_PHY                0x004DD034
#define F1V3_PHY                0x004DD035
#define F1V4_PHY                0x004DD036
#define F2V1_PHY                0x004DD042
#define AQUANTIA_PHY_107        0x03a1b4e2
#define AQUANTIA_PHY_108        0x03a1b4f2
#define AQUANTIA_PHY_109        0x03a1b502
#define AQUANTIA_PHY_111        0x03a1b610
#define AQUANTIA_PHY_111B0      0x03a1b612
#define AQUANTIA_PHY_112        0x03a1b660
#define AQUANTIA_PHY_113C_A0    0x31c31C10
#define AQUANTIA_PHY_113C_A1    0x31c31C11
#define AQUANTIA_PHY_112C       0x03a1b792

#define PHY_805XV2              0x004DD082
#define PHY_805XV1              0x004DD081
/*qca808x_start*/
#define SFP_PHY                 0xaaaabbbb
/*qca808x_end*/
#define MP_GEPHY                0x004DD0C0
#define SFP_PHY_MASK            0xffffffff

#define CABLE_PAIR_A            0
#define CABLE_PAIR_B            1
#define CABLE_PAIR_C            2
#define CABLE_PAIR_D            3
/*qca808x_start*/
#define PHY_MDIO_ACCESS         0
#define PHY_I2C_ACCESS          1

#define INVALID_PHY_ADDR        0xff
#define MAX_PHY_ADDR            0x1f
#define QCA8072_PHY_NUM         0x2

#define PHY_INVALID_DATA 0xffff

#define PHY_RTN_ON_READ_ERROR(phy_data) \
    do { if (phy_data == PHY_INVALID_DATA) return(SW_READ_ERROR); } while(0);

#define PHY_RTN_ON_ERROR(rv) \
    do { if (rv != SW_OK) return(rv); } while(0);

sw_error_t
hsl_phy_api_ops_register(phy_type_t phy_type, hsl_phy_ops_t * phy_api_ops);

sw_error_t
hsl_phy_api_ops_unregister(phy_type_t phy_type, hsl_phy_ops_t * phy_api_ops);

hsl_phy_ops_t *hsl_phy_api_ops_get(a_uint32_t dev_id, a_uint32_t port_id);

sw_error_t phy_api_ops_init(phy_type_t phy_type);

int ssdk_phy_driver_init(a_uint32_t dev_id, ssdk_init_cfg *cfg);

int qca_ssdk_phy_info_init(a_uint32_t dev_id);

void qca_ssdk_port_bmp_init(a_uint32_t dev_id);
/*qca808x_end*/
void hsl_phy_address_init(a_uint32_t dev_id, a_uint32_t i,
				a_uint32_t value);
/*qca808x_start*/
a_uint32_t
hsl_phyid_get(a_uint32_t dev_id, a_uint32_t port_id, ssdk_init_cfg *cfg);

a_uint32_t
qca_ssdk_port_to_phy_addr(a_uint32_t dev_id, a_uint32_t port_id);
/*qca808x_end*/
a_uint32_t
qca_ssdk_port_to_phy_mdio_fake_addr(a_uint32_t dev_id, a_uint32_t port_id);

a_uint32_t
qca_ssdk_phy_mdio_fake_addr_to_port(a_uint32_t dev_id, a_uint32_t phy_addr);

void qca_ssdk_phy_mdio_fake_address_set(a_uint32_t dev_id, a_uint32_t i,
				a_uint32_t value);
/*qca808x_start*/
void qca_ssdk_port_bmp_set(a_uint32_t dev_id, a_uint32_t value);

a_uint32_t qca_ssdk_port_bmp_get(a_uint32_t dev_id);
/*qca808x_end*/
a_uint32_t qca_ssdk_phy_type_port_bmp_get(a_uint32_t dev_id,
				phy_type_t phy_type);
/*qca808x_start*/
a_uint32_t
qca_ssdk_phy_addr_to_port(a_uint32_t dev_id, a_uint32_t phy_addr);
/*qca808x_end*/
void
hsl_port_phy_c45_capability_set(a_uint32_t dev_id, a_uint32_t port_id,
			a_bool_t enable);

a_bool_t
hsl_port_phy_combo_capability_get(a_uint32_t dev_id, a_uint32_t port_id);

void
hsl_port_phy_combo_capability_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t enable);
/*qca808x_start*/
a_uint8_t
hsl_port_phy_access_type_get(a_uint32_t dev_id, a_uint32_t port_id);

void
hsl_port_phy_access_type_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint8_t access_type);
/*qca808x_end*/
sw_error_t
hsl_port_phy_serdes_reset(a_uint32_t dev_id);

sw_error_t
hsl_port_phy_mode_set(a_uint32_t dev_id, fal_port_interface_mode_t mode);
phy_type_t hsl_phy_type_get(a_uint32_t dev_id, a_uint32_t port_id);

a_uint32_t
hsl_port_phyid_get(a_uint32_t dev_id, fal_port_t port_id);

a_uint32_t hsl_port_phy_reset_gpio_get(a_uint32_t dev_id, a_uint32_t port_id);

void hsl_port_phy_reset_gpio_set(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t phy_reset_gpio);

void hsl_port_phy_gpio_reset(a_uint32_t dev_id, a_uint32_t port_id);

void
hsl_port_phy_dac_get(a_uint32_t dev_id, a_uint32_t port_id,
	phy_dac_t *phy_dac);

void
hsl_port_phy_dac_set(a_uint32_t dev_id, a_uint32_t port_id,
	phy_dac_t phy_dac);
/*qca808x_start*/
sw_error_t ssdk_phy_driver_cleanup(void);
/*qca808x_end*/
sw_error_t
hsl_phydriver_update(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t mode);

void
qca_ssdk_phy_address_set(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t phy_addr);

sw_error_t
hsl_port_phy_hw_init(a_uint32_t dev_id, a_uint32_t port_id);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0))
sw_error_t
hsl_port_phydev_adv_update(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t autoadv);
#endif
/*qca808x_start*/
#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _HSL_PHY_H_ */
/*qca808x_end*/
