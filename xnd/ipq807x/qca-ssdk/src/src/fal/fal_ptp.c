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


/**
 * @defgroup fal_ptp FAL_PTP
 * @{
 */
#include "sw.h"
#include "fal_ptp.h"
#include "hsl_api.h"
#include "adpt.h"

#include <linux/kernel.h>
#include <linux/module.h>

sw_error_t
_fal_ptp_security_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_security_t *sec)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_security_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_security_set(dev_id, port_id, sec);
	return rv;
}
sw_error_t
_fal_ptp_link_delay_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_link_delay_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_link_delay_set(dev_id, port_id, time);
	return rv;
}
sw_error_t
_fal_ptp_rx_crc_recalc_status_get(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rx_crc_recalc_status_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rx_crc_recalc_status_get(dev_id, port_id, status);
	return rv;
}
sw_error_t
_fal_ptp_tod_uart_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_tod_uart_t *tod_uart)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_tod_uart_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_tod_uart_set(dev_id, port_id, tod_uart);
	return rv;
}
sw_error_t
_fal_ptp_enhanced_timestamp_engine_get(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_enhanced_timestamp_engine_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_enhanced_timestamp_engine_get(dev_id, port_id, direction, ts_engine);
	return rv;
}
sw_error_t
_fal_ptp_pps_signal_control_set(a_uint32_t dev_id,
		a_uint32_t port_id,
		fal_ptp_pps_signal_control_t *sig_control)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_pps_signal_control_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_pps_signal_control_set(dev_id, port_id, sig_control);
	return rv;
}
sw_error_t
_fal_ptp_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_direction_t direction,
		fal_ptp_pkt_info_t *pkt_info,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_timestamp_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_timestamp_get(dev_id, port_id, direction, pkt_info, time);
	return rv;
}
sw_error_t
_fal_ptp_asym_correction_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_asym_correction_t* asym_cf)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_asym_correction_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_asym_correction_get(dev_id, port_id, asym_cf);
	return rv;
}
sw_error_t
_fal_ptp_rtc_time_snapshot_status_get(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rtc_time_snapshot_status_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rtc_time_snapshot_status_get(dev_id, port_id, status);
	return rv;
}
sw_error_t
_fal_ptp_capture_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_capture_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_capture_set(dev_id, port_id, capture_id, capture);
	return rv;
}
sw_error_t
_fal_ptp_rtc_adjfreq_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rtc_adjfreq_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rtc_adjfreq_set(dev_id, port_id, time);
	return rv;
}
sw_error_t
_fal_ptp_asym_correction_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_asym_correction_t *asym_cf)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_asym_correction_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_asym_correction_set(dev_id, port_id, asym_cf);
	return rv;
}
sw_error_t
_fal_ptp_pkt_timestamp_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_pkt_timestamp_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_pkt_timestamp_set(dev_id, port_id, time);
	return rv;
}
sw_error_t
_fal_ptp_rtc_time_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rtc_time_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rtc_time_get(dev_id, port_id, time);
	return rv;
}
sw_error_t
_fal_ptp_rtc_time_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rtc_time_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rtc_time_set(dev_id, port_id, time);
	return rv;
}
sw_error_t
_fal_ptp_pkt_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_pkt_timestamp_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_pkt_timestamp_get(dev_id, port_id, time);
	return rv;
}
sw_error_t
_fal_ptp_interrupt_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_interrupt_t *interrupt)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_interrupt_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_interrupt_set(dev_id, port_id, interrupt);
	return rv;
}
sw_error_t
_fal_ptp_trigger_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_trigger_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_trigger_set(dev_id, port_id, trigger_id, triger);
	return rv;
}
sw_error_t
_fal_ptp_pps_signal_control_get(a_uint32_t dev_id,
		a_uint32_t port_id,
		fal_ptp_pps_signal_control_t *sig_control)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_pps_signal_control_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_pps_signal_control_get(dev_id, port_id, sig_control);
	return rv;
}
sw_error_t
_fal_ptp_capture_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_capture_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_capture_get(dev_id, port_id, capture_id, capture);
	return rv;
}
sw_error_t
_fal_ptp_rx_crc_recalc_enable(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t status)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rx_crc_recalc_enable)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rx_crc_recalc_enable(dev_id, port_id, status);
	return rv;
}
sw_error_t
_fal_ptp_security_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_security_t *sec)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_security_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_security_get(dev_id, port_id, sec);
	return rv;
}
sw_error_t
_fal_ptp_increment_sync_from_clock_status_get(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_increment_sync_from_clock_status_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_increment_sync_from_clock_status_get(dev_id, port_id, status);
	return rv;
}
sw_error_t
_fal_ptp_tod_uart_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_tod_uart_t *tod_uart)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_tod_uart_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_tod_uart_get(dev_id, port_id, tod_uart);
	return rv;
}
sw_error_t
_fal_ptp_enhanced_timestamp_engine_set(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_enhanced_timestamp_engine_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_enhanced_timestamp_engine_set(dev_id, port_id, direction, ts_engine);
	return rv;
}
sw_error_t
_fal_ptp_rtc_time_clear(a_uint32_t dev_id, a_uint32_t port_id)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rtc_time_clear)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rtc_time_clear(dev_id, port_id);
	return rv;
}
sw_error_t
_fal_ptp_reference_clock_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_reference_clock_t ref_clock)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_reference_clock_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_reference_clock_set(dev_id, port_id, ref_clock);
	return rv;
}
sw_error_t
_fal_ptp_output_waveform_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_output_waveform_t *waveform)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_output_waveform_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_output_waveform_set(dev_id, port_id, waveform);
	return rv;
}
sw_error_t
_fal_ptp_rx_timestamp_mode_set(a_uint32_t dev_id,
		a_uint32_t port_id,
		fal_ptp_rx_timestamp_mode_t ts_mode)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rx_timestamp_mode_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rx_timestamp_mode_set(dev_id, port_id, ts_mode);
	return rv;
}
sw_error_t
_fal_ptp_grandmaster_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_grandmaster_mode_t *gm_mode)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_grandmaster_mode_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_grandmaster_mode_set(dev_id, port_id, gm_mode);
	return rv;
}
sw_error_t
_fal_ptp_config_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_config_t *config)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_config_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_config_set(dev_id, port_id, config);
	return rv;
}
sw_error_t
_fal_ptp_trigger_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_trigger_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_trigger_get(dev_id, port_id, trigger_id, triger);
	return rv;
}
sw_error_t
_fal_ptp_rtc_adjfreq_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rtc_adjfreq_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rtc_adjfreq_get(dev_id, port_id, time);
	return rv;
}
sw_error_t
_fal_ptp_grandmaster_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_grandmaster_mode_t *gm_mode)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_grandmaster_mode_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_grandmaster_mode_get(dev_id, port_id, gm_mode);
	return rv;
}
sw_error_t
_fal_ptp_rx_timestamp_mode_get(a_uint32_t dev_id,
		a_uint32_t port_id,
		fal_ptp_rx_timestamp_mode_t *ts_mode)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rx_timestamp_mode_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rx_timestamp_mode_get(dev_id, port_id, ts_mode);
	return rv;
}
sw_error_t
_fal_ptp_rtc_adjtime_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rtc_adjtime_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rtc_adjtime_set(dev_id, port_id, time);
	return rv;
}
sw_error_t
_fal_ptp_link_delay_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_link_delay_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_link_delay_get(dev_id, port_id, time);
	return rv;
}
sw_error_t
_fal_ptp_increment_sync_from_clock_enable(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t status)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_increment_sync_from_clock_enable)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_increment_sync_from_clock_enable(dev_id, port_id, status);
	return rv;
}
sw_error_t
_fal_ptp_config_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_config_t *config)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_config_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_config_get(dev_id, port_id, config);
	return rv;
}
sw_error_t
_fal_ptp_output_waveform_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_output_waveform_t *waveform)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_output_waveform_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_output_waveform_get(dev_id, port_id, waveform);
	return rv;
}
sw_error_t
_fal_ptp_interrupt_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_interrupt_t *interrupt)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_interrupt_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_interrupt_get(dev_id, port_id, interrupt);
	return rv;
}
sw_error_t
_fal_ptp_rtc_time_snapshot_enable(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t status)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_rtc_time_snapshot_enable)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_rtc_time_snapshot_enable(dev_id, port_id, status);
	return rv;
}
sw_error_t
_fal_ptp_reference_clock_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_reference_clock_t *ref_clock)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_ptp_reference_clock_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_ptp_reference_clock_get(dev_id, port_id, ref_clock);
	return rv;
}

sw_error_t
fal_ptp_security_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_security_t *sec)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_security_set(dev_id, port_id, sec);
	FAL_API_UNLOCK;
	return rv;
}
	sw_error_t
fal_ptp_link_delay_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_link_delay_set(dev_id, port_id, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rx_crc_recalc_status_get(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rx_crc_recalc_status_get(dev_id, port_id, status);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_tod_uart_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_tod_uart_t *tod_uart)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_tod_uart_set(dev_id, port_id, tod_uart);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_enhanced_timestamp_engine_get(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_enhanced_timestamp_engine_get(dev_id, port_id, direction, ts_engine);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_pps_signal_control_set(a_uint32_t dev_id,
		a_uint32_t port_id,
		fal_ptp_pps_signal_control_t *sig_control)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_pps_signal_control_set(dev_id, port_id, sig_control);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_direction_t direction,
		fal_ptp_pkt_info_t *pkt_info,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_timestamp_get(dev_id, port_id, direction, pkt_info, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_asym_correction_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_asym_correction_t* asym_cf)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_asym_correction_get(dev_id, port_id, asym_cf);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rtc_time_snapshot_status_get(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rtc_time_snapshot_status_get(dev_id, port_id, status);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_capture_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_capture_set(dev_id, port_id, capture_id, capture);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rtc_adjfreq_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rtc_adjfreq_set(dev_id, port_id, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_asym_correction_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_asym_correction_t *asym_cf)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_asym_correction_set(dev_id, port_id, asym_cf);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_pkt_timestamp_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_pkt_timestamp_set(dev_id, port_id, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rtc_time_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rtc_time_get(dev_id, port_id, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rtc_time_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rtc_time_set(dev_id, port_id, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_pkt_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_pkt_timestamp_get(dev_id, port_id, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_interrupt_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_interrupt_t *interrupt)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_interrupt_set(dev_id, port_id, interrupt);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_trigger_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_trigger_set(dev_id, port_id, trigger_id, triger);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_pps_signal_control_get(a_uint32_t dev_id,
		a_uint32_t port_id,
		fal_ptp_pps_signal_control_t *sig_control)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_pps_signal_control_get(dev_id, port_id, sig_control);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_capture_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_capture_get(dev_id, port_id, capture_id, capture);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rx_crc_recalc_enable(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t status)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rx_crc_recalc_enable(dev_id, port_id, status);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_security_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_security_t *sec)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_security_get(dev_id, port_id, sec);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_increment_sync_from_clock_status_get(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_increment_sync_from_clock_status_get(dev_id, port_id, status);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_tod_uart_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_tod_uart_t *tod_uart)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_tod_uart_get(dev_id, port_id, tod_uart);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_enhanced_timestamp_engine_set(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_enhanced_timestamp_engine_set(dev_id, port_id, direction, ts_engine);
	FAL_API_UNLOCK;
	return rv;
}
	sw_error_t
fal_ptp_rtc_time_clear(a_uint32_t dev_id, a_uint32_t port_id)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rtc_time_clear(dev_id, port_id);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_reference_clock_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_reference_clock_t ref_clock)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_reference_clock_set(dev_id, port_id, ref_clock);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_output_waveform_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_output_waveform_t *waveform)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_output_waveform_set(dev_id, port_id, waveform);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rx_timestamp_mode_set(a_uint32_t dev_id,
		a_uint32_t port_id,
		fal_ptp_rx_timestamp_mode_t ts_mode)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rx_timestamp_mode_set(dev_id, port_id, ts_mode);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_grandmaster_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_grandmaster_mode_t *gm_mode)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_grandmaster_mode_set(dev_id, port_id, gm_mode);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_config_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_config_t *config)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_config_set(dev_id, port_id, config);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_trigger_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_trigger_get(dev_id, port_id, trigger_id, triger);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rtc_adjfreq_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rtc_adjfreq_get(dev_id, port_id, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_grandmaster_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_grandmaster_mode_t *gm_mode)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_grandmaster_mode_get(dev_id, port_id, gm_mode);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rx_timestamp_mode_get(a_uint32_t dev_id,
		a_uint32_t port_id,
		fal_ptp_rx_timestamp_mode_t *ts_mode)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rx_timestamp_mode_get(dev_id, port_id, ts_mode);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rtc_adjtime_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rtc_adjtime_set(dev_id, port_id, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_link_delay_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_link_delay_get(dev_id, port_id, time);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_increment_sync_from_clock_enable(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t status)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_increment_sync_from_clock_enable(dev_id, port_id, status);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_config_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_config_t *config)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_config_get(dev_id, port_id, config);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_output_waveform_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_output_waveform_t *waveform)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_output_waveform_get(dev_id, port_id, waveform);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_interrupt_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_interrupt_t *interrupt)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_interrupt_get(dev_id, port_id, interrupt);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_rtc_time_snapshot_enable(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t status)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_rtc_time_snapshot_enable(dev_id, port_id, status);
	FAL_API_UNLOCK;
	return rv;
}
sw_error_t
fal_ptp_reference_clock_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_reference_clock_t *ref_clock)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_ptp_reference_clock_get(dev_id, port_id, ref_clock);
	FAL_API_UNLOCK;
	return rv;
}

EXPORT_SYMBOL(fal_ptp_config_set);
EXPORT_SYMBOL(fal_ptp_config_get);
EXPORT_SYMBOL(fal_ptp_reference_clock_set);
EXPORT_SYMBOL(fal_ptp_reference_clock_get);
EXPORT_SYMBOL(fal_ptp_rx_timestamp_mode_set);
EXPORT_SYMBOL(fal_ptp_rx_timestamp_mode_get);
EXPORT_SYMBOL(fal_ptp_timestamp_get);
EXPORT_SYMBOL(fal_ptp_pkt_timestamp_set);
EXPORT_SYMBOL(fal_ptp_pkt_timestamp_get);
EXPORT_SYMBOL(fal_ptp_grandmaster_mode_set);
EXPORT_SYMBOL(fal_ptp_grandmaster_mode_get);
EXPORT_SYMBOL(fal_ptp_rtc_time_get);
EXPORT_SYMBOL(fal_ptp_rtc_time_set);
EXPORT_SYMBOL(fal_ptp_rtc_time_clear);
EXPORT_SYMBOL(fal_ptp_rtc_adjtime_set);
EXPORT_SYMBOL(fal_ptp_rtc_adjfreq_set);
EXPORT_SYMBOL(fal_ptp_rtc_adjfreq_get);
EXPORT_SYMBOL(fal_ptp_link_delay_set);
EXPORT_SYMBOL(fal_ptp_link_delay_get);
EXPORT_SYMBOL(fal_ptp_security_set);
EXPORT_SYMBOL(fal_ptp_security_get);
EXPORT_SYMBOL(fal_ptp_pps_signal_control_set);
EXPORT_SYMBOL(fal_ptp_pps_signal_control_get);
EXPORT_SYMBOL(fal_ptp_rx_crc_recalc_enable);
EXPORT_SYMBOL(fal_ptp_rx_crc_recalc_status_get);
EXPORT_SYMBOL(fal_ptp_asym_correction_set);
EXPORT_SYMBOL(fal_ptp_asym_correction_get);
EXPORT_SYMBOL(fal_ptp_output_waveform_set);
EXPORT_SYMBOL(fal_ptp_output_waveform_get);
EXPORT_SYMBOL(fal_ptp_rtc_time_snapshot_enable);
EXPORT_SYMBOL(fal_ptp_rtc_time_snapshot_status_get);
EXPORT_SYMBOL(fal_ptp_increment_sync_from_clock_enable);
EXPORT_SYMBOL(fal_ptp_increment_sync_from_clock_status_get);
EXPORT_SYMBOL(fal_ptp_tod_uart_set);
EXPORT_SYMBOL(fal_ptp_tod_uart_get);
EXPORT_SYMBOL(fal_ptp_enhanced_timestamp_engine_set);
EXPORT_SYMBOL(fal_ptp_enhanced_timestamp_engine_get);
EXPORT_SYMBOL(fal_ptp_trigger_set);
EXPORT_SYMBOL(fal_ptp_trigger_get);
EXPORT_SYMBOL(fal_ptp_capture_set);
EXPORT_SYMBOL(fal_ptp_capture_get);
EXPORT_SYMBOL(fal_ptp_interrupt_set);
EXPORT_SYMBOL(fal_ptp_interrupt_get);

/**
 * @}
 */
