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



#include "sw.h"
#include "sw_ioctl.h"
#include "fal_ptp.h"
#include "fal_uk_if.h"

sw_error_t
fal_ptp_config_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_config_t *config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_CONFIG_SET, dev_id, port_id,
                    config);
    return rv;
}

sw_error_t
fal_ptp_config_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_config_t *config)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_CONFIG_GET, dev_id, port_id,
                    config);
    return rv;
}

sw_error_t
fal_ptp_reference_clock_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_reference_clock_t ref_clock)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_REFERENCE_CLOCK_SET, dev_id, port_id,
                    ref_clock);
    return rv;
}

sw_error_t
fal_ptp_reference_clock_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_reference_clock_t *ref_clock)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_REFERENCE_CLOCK_GET, dev_id, port_id,
                    ref_clock);
    return rv;
}

sw_error_t
fal_ptp_rx_timestamp_mode_set(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_rx_timestamp_mode_t ts_mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RX_TIMESTAMP_MODE_SET, dev_id, port_id,
                    ts_mode);
    return rv;
}

sw_error_t
fal_ptp_rx_timestamp_mode_get(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_rx_timestamp_mode_t *ts_mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RX_TIMESTAMP_MODE_GET, dev_id, port_id,
                    ts_mode);
    return rv;
}

sw_error_t
fal_ptp_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_direction_t direction,
		fal_ptp_pkt_info_t *pkt_info,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_TIMESTAMP_GET, dev_id, port_id,
                    direction, pkt_info, time);
    return rv;
}

sw_error_t
fal_ptp_pkt_timestamp_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_PKT_TIMESTAMP_SET, dev_id, port_id,
                    time);
    return rv;
}

sw_error_t
fal_ptp_pkt_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_PKT_TIMESTAMP_GET, dev_id, port_id,
                    time);
    return rv;
}

sw_error_t
fal_ptp_grandmaster_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_grandmaster_mode_t *gm_mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_GRANDMASTER_MODE_SET, dev_id, port_id,
                    gm_mode);
    return rv;
}

sw_error_t
fal_ptp_grandmaster_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_grandmaster_mode_t *gm_mode)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_GRANDMASTER_MODE_GET, dev_id, port_id,
                    gm_mode);
    return rv;
}

sw_error_t
fal_ptp_rtc_time_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RTC_TIME_GET, dev_id, port_id,
                    time);
    return rv;
}

sw_error_t
fal_ptp_rtc_time_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RTC_TIME_SET, dev_id, port_id,
                    time);
    return rv;
}

sw_error_t
fal_ptp_rtc_time_clear(a_uint32_t dev_id, a_uint32_t port_id)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RTC_TIME_CLEAR, dev_id, port_id);
    return rv;
}

sw_error_t
fal_ptp_rtc_adjtime_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RTC_ADJTIME_SET, dev_id, port_id,
                    time);
    return rv;
}

sw_error_t
fal_ptp_rtc_adjfreq_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RTC_ADJFREQ_SET, dev_id, port_id,
                    time);
    return rv;
}

sw_error_t
fal_ptp_rtc_adjfreq_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RTC_ADJFREQ_GET, dev_id, port_id,
                    time);
    return rv;
}

sw_error_t
fal_ptp_link_delay_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_LINK_DELAY_SET, dev_id, port_id,
                    time);
    return rv;
}

sw_error_t
fal_ptp_link_delay_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_LINK_DELAY_GET, dev_id, port_id,
                    time);
    return rv;
}

sw_error_t
fal_ptp_security_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_security_t *sec)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_SECURITY_SET, dev_id, port_id,
                    sec);
    return rv;
}

sw_error_t
fal_ptp_security_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_security_t *sec)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_SECURITY_GET, dev_id, port_id,
                    sec);
    return rv;
}

sw_error_t
fal_ptp_pps_signal_control_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_pps_signal_control_t *sig_control)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_PPS_SIGNAL_CONTROL_SET, dev_id, port_id,
                    sig_control);
    return rv;
}

sw_error_t
fal_ptp_pps_signal_control_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_pps_signal_control_t *sig_control)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_PPS_SIGNAL_CONTROL_GET, dev_id, port_id,
                    sig_control);
    return rv;
}

sw_error_t
fal_ptp_rx_crc_recalc_enable(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t status)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RX_CRC_RECALC_SET, dev_id, port_id,
                    status);
    return rv;
}

sw_error_t
fal_ptp_rx_crc_recalc_status_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t *status)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RX_CRC_RECALC_GET, dev_id, port_id,
                    status);
    return rv;
}

sw_error_t
fal_ptp_asym_correction_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_asym_correction_t *asym_cf)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_ASYM_CORRECTION_SET, dev_id, port_id,
                    asym_cf);
    return rv;
}

sw_error_t
fal_ptp_asym_correction_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_asym_correction_t* asym_cf)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_ASYM_CORRECTION_GET, dev_id, port_id,
                    asym_cf);
    return rv;
}

sw_error_t
fal_ptp_output_waveform_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_output_waveform_t *waveform)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_OUTPUT_WAVEFORM_SET, dev_id, port_id,
                    waveform);
    return rv;
}

sw_error_t
fal_ptp_output_waveform_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_output_waveform_t *waveform)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_OUTPUT_WAVEFORM_GET, dev_id, port_id,
                    waveform);
    return rv;
}

sw_error_t
fal_ptp_rtc_time_snapshot_enable(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t status)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RTC_TIME_SNAPSHOT_SET, dev_id, port_id,
                    status);
    return rv;
}

sw_error_t
fal_ptp_rtc_time_snapshot_status_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t *status)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_RTC_TIME_SNAPSHOT_GET, dev_id, port_id,
                    status);
    return rv;
}

sw_error_t
fal_ptp_increment_sync_from_clock_enable(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t status)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_INCREMENT_SYNC_FROM_CLOCK_SET, dev_id, port_id,
                    status);
    return rv;
}

sw_error_t
fal_ptp_increment_sync_from_clock_status_get(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_INCREMENT_SYNC_FROM_CLOCK_GET, dev_id, port_id,
                    status);
    return rv;
}

sw_error_t
fal_ptp_tod_uart_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_tod_uart_t *tod_uart)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_TOD_UART_SET, dev_id, port_id,
                    tod_uart);
    return rv;
}

sw_error_t
fal_ptp_tod_uart_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_tod_uart_t *tod_uart)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_TOD_UART_GET, dev_id, port_id,
                    tod_uart);
    return rv;
}

sw_error_t
fal_ptp_enhanced_timestamp_engine_set(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_ENHANCED_TIMESTAMP_ENGINE_SET, dev_id, port_id,
                    direction, ts_engine);
    return rv;
}

sw_error_t
fal_ptp_enhanced_timestamp_engine_get(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_ENHANCED_TIMESTAMP_ENGINE_GET, dev_id, port_id,
                    direction, ts_engine);
    return rv;
}

sw_error_t
fal_ptp_trigger_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_TRIGGER_SET, dev_id, port_id,
                    trigger_id, triger);
    return rv;
}

sw_error_t
fal_ptp_trigger_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_TRIGGER_GET, dev_id, port_id,
                    trigger_id, triger);
    return rv;
}

sw_error_t
fal_ptp_capture_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_CAPTURE_SET, dev_id, port_id,
                    capture_id, capture);
    return rv;
}

sw_error_t
fal_ptp_capture_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_CAPTURE_GET, dev_id, port_id,
                    capture_id, capture);
    return rv;
}

sw_error_t
fal_ptp_interrupt_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_interrupt_t *interrupt)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_INTERRUPT_SET, dev_id, port_id,
                    interrupt);
    return rv;
}

sw_error_t
fal_ptp_interrupt_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_interrupt_t *interrupt)
{
    sw_error_t rv;

    rv = sw_uk_exec(SW_API_PTP_INTERRUPT_GET, dev_id, port_id,
                    interrupt);
    return rv;
}


