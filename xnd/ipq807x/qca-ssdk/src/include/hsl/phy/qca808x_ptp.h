/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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

#ifndef _QCA808X_PTP_H_
#define _QCA808X_PTP_H_

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

#define PTP_DEV_ID	0

sw_error_t
qca808x_phy_ptp_security_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_security_t *sec);

sw_error_t
qca808x_phy_ptp_link_delay_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_rx_crc_recalc_status_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t *status);

sw_error_t
qca808x_phy_ptp_tod_uart_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_tod_uart_t *tod_uart);

sw_error_t
qca808x_phy_ptp_enhanced_timestamp_engine_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine);

sw_error_t
qca808x_phy_ptp_pps_signal_control_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_pps_signal_control_t *sig_control);

sw_error_t
qca808x_phy_ptp_timestamp_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_direction_t direction,
		fal_ptp_pkt_info_t *pkt_info, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_asym_correction_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_asym_correction_t* asym_cf);

sw_error_t
qca808x_phy_ptp_rtc_time_snapshot_status_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t *status);

sw_error_t
qca808x_phy_ptp_capture_set(a_uint32_t dev_id,
		a_uint32_t phy_id, a_uint32_t capture_id,
		fal_ptp_capture_t *capture);

sw_error_t
qca808x_phy_ptp_rtc_adjfreq_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_asym_correction_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_asym_correction_t *asym_cf);

sw_error_t
qca808x_phy_ptp_pkt_timestamp_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_rtc_time_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_rtc_time_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_pkt_timestamp_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_interrupt_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_interrupt_t *interrupt);

sw_error_t
qca808x_phy_ptp_trigger_set(a_uint32_t dev_id,
		a_uint32_t phy_id, a_uint32_t trigger_id,
		fal_ptp_trigger_t *triger);

sw_error_t
qca808x_phy_ptp_pps_signal_control_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_pps_signal_control_t *sig_control);

sw_error_t
qca808x_phy_ptp_capture_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_uint32_t capture_id,
		fal_ptp_capture_t *capture);

sw_error_t
qca808x_phy_ptp_rx_crc_recalc_enable(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t status);

sw_error_t
qca808x_phy_ptp_security_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_security_t *sec);

sw_error_t
qca808x_phy_ptp_increment_sync_from_clock_status_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t *status);

sw_error_t
qca808x_phy_ptp_tod_uart_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_tod_uart_t *tod_uart);

sw_error_t
qca808x_phy_ptp_enhanced_timestamp_engine_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine);

sw_error_t
qca808x_phy_ptp_rtc_time_clear(a_uint32_t dev_id,
		a_uint32_t phy_id);

sw_error_t
qca808x_phy_ptp_reference_clock_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_reference_clock_t ref_clock);

sw_error_t
qca808x_phy_ptp_output_waveform_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_output_waveform_t *waveform);

sw_error_t
qca808x_phy_ptp_rx_timestamp_mode_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_rx_timestamp_mode_t ts_mode);

sw_error_t
qca808x_phy_ptp_grandmaster_mode_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_grandmaster_mode_t *gm_mode);

sw_error_t
qca808x_phy_ptp_config_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_config_t *config);

sw_error_t
qca808x_phy_ptp_trigger_get(a_uint32_t dev_id,
		a_uint32_t phy_id, a_uint32_t trigger_id,
		fal_ptp_trigger_t *triger);

sw_error_t
qca808x_phy_ptp_rtc_adjfreq_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_grandmaster_mode_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_grandmaster_mode_t *gm_mode);

sw_error_t
qca808x_phy_ptp_rx_timestamp_mode_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_rx_timestamp_mode_t *ts_mode);

sw_error_t
qca808x_phy_ptp_rtc_adjtime_set(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_link_delay_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_time_t *time);

sw_error_t
qca808x_phy_ptp_increment_sync_from_clock_enable(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t status);

sw_error_t
qca808x_phy_ptp_config_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_config_t *config);

sw_error_t
qca808x_phy_ptp_output_waveform_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_output_waveform_t *waveform);

sw_error_t
qca808x_phy_ptp_interrupt_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_interrupt_t *interrupt);

sw_error_t
qca808x_phy_ptp_rtc_time_snapshot_enable(a_uint32_t dev_id,
		a_uint32_t phy_id, a_bool_t status);

sw_error_t
qca808x_phy_ptp_reference_clock_get(a_uint32_t dev_id,
		a_uint32_t phy_id, fal_ptp_reference_clock_t *ref_clock);

void qca808x_phy_ptp_api_ops_init(hsl_phy_ptp_ops_t *phy_ptp_ops);

#if defined(IN_LINUX_STD_PTP)
void qca808x_ptp_gm_gps_seconds_sync_enable(a_uint32_t dev_id,
		a_uint32_t phy_addr, a_bool_t en);

a_bool_t qca808x_ptp_gm_gps_seconds_sync_status_get(a_uint32_t dev_id,
		a_uint32_t phy_addr);

void qca808x_ptp_clock_mode_config(a_uint32_t dev_id,
		a_uint32_t phy_addr, a_uint16_t clock_mode, a_uint16_t step_mode);

void qca808x_ptp_stat_get(void);

void qca808x_ptp_stat_set(void);
#endif
#ifdef __cplusplus
}
#endif				/* __cplusplus */
#endif				/* _qca808x_PTP_H_ */
