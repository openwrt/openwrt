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
#ifndef _FAL_PTP_H_
#define _FAL_PTP_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "common/sw.h"
#include "fal/fal_type.h"

typedef enum {
	FAL_OC_CLOCK_MODE = 0,
	FAL_BC_CLOCK_MODE,
	FAL_E2ETC_CLOCK_MODE,
	FAL_P2PTC_CLOCK_MODE
} fal_ptp_clock_mode_t;

typedef enum {
	FAL_ONE_STEP_MODE = 0,
	FAL_TWO_STEP_MODE,
	FAL_AUTO_MODE	/* ONE/TWO will be auto selected through packet field */
} fal_ptp_step_mode_t;

typedef struct {
	a_bool_t ptp_en;	/* enable/disable ptp feature */
	fal_ptp_clock_mode_t clock_mode;
	fal_ptp_step_mode_t step_mode;
} fal_ptp_config_t;

typedef enum {
	FAL_REF_CLOCK_LOCAL = 0,/* use local clock as reference clock */
	FAL_REF_CLOCK_SYNCE,	/* use synce clock as reference clock */
	FAL_REF_CLOCK_EXTERNAL,	/* use external clock as reference clock */
} fal_ptp_reference_clock_t;

typedef enum {
	FAL_RX_TS_MDIO = 0,	/* timestamp will be saved to MDIO register */
	FAL_RX_TS_EMBED,	/* timestamp will be saved to packet PTP header */
} fal_ptp_rx_timestamp_mode_t;

typedef struct{
	a_int64_t seconds;
	a_int32_t nanoseconds;
	a_int32_t fracnanoseconds;
} fal_ptp_time_t;

typedef enum {
	FAL_RX_DIRECTION = 0,
	FAL_TX_DIRECTION
} fal_ptp_direction_t;

typedef struct{
	a_uint32_t sequence_id;		/* packet field: sequenceId */
	a_uint64_t clock_identify;	/* packet field: ClockIdentify */
	a_uint32_t port_number;		/* packet field: SourcePortID */
	a_uint32_t msg_type;		/* packet field: messageId */
} fal_ptp_pkt_info_t;

typedef enum {
	FAL_GM_PPSIN_MODE = 0,	/* use PSSIN mode to sync nanoseconds */
	FAL_GM_HWPLL_MODE,	/* use HWPLL mode to sync nanoseconds */
	FAL_GM_SWPLL_MODE,	/* use SWPLL mode to sync nanoseconds */
} fal_ptp_grandmaster_ns_sync_mode_t;

typedef enum {
	FAL_GM_MAXFREQ_1PPM = 0,
	FAL_GM_MAXFREQ_10PPM,
	FAL_GM_MAXFREQ_50PPM,
	FAL_GM_MAXFREQ_100PPM,
	FAL_GM_MAXFREQ_150PPM,
	FAL_GM_MAXFREQ_200PPM,
	FAL_GM_MAXFREQ_250PPM,
	FAL_GM_MAXFREQ_300PPM,
	FAL_GM_MAXFREQ_0PPM
} fal_ptp_grandmaster_maxfreq_offset_t;

typedef struct {
	a_bool_t grandmaster_mode_en;	/* enable/disable grandmaster mode */
	a_bool_t grandmaster_second_sync_en; /* GPS receiver second sync feature */
	fal_ptp_grandmaster_maxfreq_offset_t freq_offset; /* for HWPLL */
	a_bool_t right_shift_in_kp; /* for HWPLL, left or right shift */
	a_uint32_t kp_value; /* for HWPLL, proportional part coefficient */
	a_bool_t right_shift_in_ki; /* for HWPLL, left or right shift */
	a_uint32_t ki_value; /* for HWPLL, integral part coefficient */
	fal_ptp_grandmaster_ns_sync_mode_t ns_sync_mode;
} fal_ptp_grandmaster_mode_t;

typedef struct {
	a_bool_t address_check_en;	/* address matched when regard it as PTP packet */
	a_bool_t ipv6_udp_checksum_recal_en;
	a_bool_t version_check_en;	/* PTP version matched when regard it as PTP packet */
	a_uint32_t ptp_version;
	a_bool_t ipv4_udp_checksum_force_zero_en;
	a_bool_t ipv6_embed_udp_checksum_force_zero_en;
} fal_ptp_security_t;

typedef struct {
	a_bool_t negative_in_latency;	/* in latency value is positive or negative */
	a_uint32_t in_latency;		/* in latency */
	a_uint32_t out_phase;		/* adjust the phase of PPS outout signal */
	a_uint32_t out_pulse_width;	/* adjust the pulse width of PPS outout signal */
} fal_ptp_pps_signal_control_t;

typedef struct {
	a_bool_t eg_asym_en;	/* enable egress asymmetry correction */
	a_bool_t in_asym_en;	/* enable ingress asymmetry correction */
	a_uint32_t eg_asym_value;
	a_uint32_t in_asym_value;
} fal_ptp_asym_correction_t;

typedef enum {
	FAL_WAVE_FREQ = 0,	/* select wave period as SYNC_CLKO_PTP output */
	FAL_PULSE_10MS,		/* select pulse 10ms as SYNC_CLKO_PTP output */
	FAL_TRIGGER0_GPIO,	/* select trigger0 status as SYNC_CLKO_PTP output */
	FAL_RX_PTP_STATE,	/* select RX PTP state as SYNC_CLKO_PTP output */
} fal_ptp_waveform_type_t;

typedef struct {
	fal_ptp_waveform_type_t waveform_type;
	a_bool_t wave_align_pps_out_en;	/* for FAL_WAVE_FREQ, enable wave align pps output */
	a_uint64_t wave_period;		/* for FAL_WAVE_FREQ, wave period */
} fal_ptp_output_waveform_t;

enum {
	FAL_UART_START_POLARITY_HIGH_EN = 0,	/* polarity of start bit is low/high */
	FAL_UART_MSB_FIRST_EN,			/* output LSB/MSB first */
	FAL_UART_PARITY_CHECK_EN,		/* parity check is enable/disable */
	FAL_UART_AUTO_TOD_OUT_EN,		/* enable output TOD via UART automatically */
	FAL_UART_AUTO_TOD_IN_EN			/* enable input TOD via UART automatically */
};

enum {
	FAL_UART_RX_BUFFER_DATA_PRESENT = 0,	/* RX BUFFER has data */
	FAL_UART_RX_BUFFER_FULL,		/* RX BUFFER is full */
	FAL_UART_RX_BUFFER_ALMOST_FULL,		/* RX BUFFER is full-1 */
	FAL_UART_RX_BUFFER_HALF_FULL,		/* RX BUFFER is half full */
	FAL_UART_RX_BUFFER_ALMOST_EMPTY,	/* RX BUFFER is 1 */
	FAL_UART_TX_BUFFER_FULL,		/* TX BUFFER is full */
	FAL_UART_TX_BUFFER_ALMOST_FULL,		/* TX BUFFER is full-1 */
	FAL_UART_TX_BUFFER_HALF_FULL,		/* TX BUFFER is half full */
	FAL_UART_TX_BUFFER_ALMOST_EMPTY,	/* TX BUFFER is 1 */
};

typedef struct {
	a_uint16_t baud_config;	/* baud_config = baud_rate/(125 * 10^6)/(16) */
	a_uint32_t uart_config_bmp; /* refer to FAL_UART_START_POLARITY_HIGH_EN */
	a_bool_t reset_buf_en;	/* reset TOD UART RX/TX buffer, self clearing */
	a_uint32_t buf_status_bmp; /* refer to FAL_UART_RX_BUFFER_DATA_PRESENT */
	a_uint16_t tx_buf_value;
	a_uint16_t rx_buf_value;
} fal_ptp_tod_uart_t;

enum {
	FAL_ENHANCED_TS_ETH_TYPE_EN = 0,
	FAL_ENHANCED_TS_DMAC_EN,
	FAL_ENHANCED_TS_RESV_DMAC_EN,
	FAL_ENHANCED_TS_IPV4_L4_PROTO_EN,
	FAL_ENHANCED_TS_IPV4_DIP_EN,
	FAL_ENHANCED_TS_RESV_IPV4_DIP_EN,
	FAL_ENHANCED_TS_IPV6_NEXT_HEADER_EN,
	FAL_ENHANCED_TS_IPV6_DIP_EN,
	FAL_ENHANCED_TS_RESV_IPV6_DIP_EN,
	FAL_ENHANCED_TS_UDP_DPORT_EN,
	FAL_ENHANCED_TS_RESV_UDP_DPORT_EN,
	FAL_ENHANCED_TS_Y1731_EN,
	FAL_ENHANCED_TS_Y1731_TIMESTAMP_INSERT_EN,
	FAL_ENHANCED_TS_Y1731_MAC_EN /* TX direction check smac, RX direction check dmac */
};

enum {
	FAL_ENHANCED_TS_ETH_TYPE_STATUS = 0,
	FAL_ENHANCED_TS_DMAC_STATUS,
	FAL_ENHANCED_TS_RESV_PRIM_DMAC_STATUS,
	FAL_ENHANCED_TS_RESV_PDELAY_DMAC_STATUS,
	FAL_ENHANCED_TS_IPV4_L4_PROTO_STATUS,
	FAL_ENHANCED_TS_IPV4_DIP_STATUS,
	FAL_ENHANCED_TS_RESV_IPV4_PRIM_DIP_STATUS,
	FAL_ENHANCED_TS_RESV_IPV4_PDELAY_DIP_STATUS,
	FAL_ENHANCED_TS_IPV6_NEXT_HEADER_STATUS,
	FAL_ENHANCED_TS_IPV6_DIP_STATUS,
	FAL_ENHANCED_TS_RESV_IPV6_PRIM_DIP_STATUS,
	FAL_ENHANCED_TS_RESV_IPV6_PDELAY_DIP_STATUS,
	FAL_ENHANCED_TS_UDP_DPORT_STATUS,
	FAL_ENHANCED_TS_RESV_UDP_DPORT_STATUS,
	FAL_ENHANCED_TS_Y1731_MATCH_STATUS
};

typedef struct {
	a_bool_t filt_en;	/* enable/disable filter feature */
	a_uint32_t enhance_ts_conf_bmp; /* refer to FAL_ENHANCED_TS_ETH_TYPE_EN */
	a_uint32_t eth_type;
	fal_mac_addr_t dmac_addr;
	a_uint32_t ipv4_l4_proto;
	fal_ip4_addr_t ipv4_dip;
	fal_ip6_addr_t ipv6_dip;
	a_uint32_t udp_dport;
	fal_mac_addr_t y1731_mac_addr;	/* TX direction check smac, RX direction check dmac */
	a_uint32_t enhance_ts_status_bmp; 	/* refer to FAL_ENHANCED_TS_ETH_TYPE_STATUS */
	a_uint32_t enhance_ts_status_pre_bmp;	/* TX NOT SUPPORT */
	a_uint32_t y1731_identity;	/* save Y1731 identify value */
	a_uint32_t y1731_identity_pre;	/* TX NOT SUPPORT */
	fal_ptp_time_t timestamp;	/* save packet timestamp when matched */
	fal_ptp_time_t timestamp_pre;	/* TX NOT SUPPORT */
} fal_ptp_enhanced_ts_engine_t;

typedef struct {
	a_bool_t trigger_en;
	a_bool_t output_force_en;
	int output_force_value;
	int patten_select;
	int late_operation;
	int notify;
	int trigger_effect;
	fal_ptp_time_t tim;
} fal_ptp_trigger_conf_t;

typedef struct {
	int trigger_finished;
	int trigger_active;
	int trigger_error;
} fal_ptp_trigger_status_t;

typedef struct {
	fal_ptp_trigger_conf_t trigger_conf;
	fal_ptp_trigger_status_t trigger_status;
} fal_ptp_trigger_t;

typedef struct {
	int status_clear;
	int notify_event;
	int single_multi_select;
	a_bool_t fall_edge_en;
	a_bool_t rise_edge_en;
} fal_ptp_capture_conf_t;

typedef struct {
	int event_detected;
	int fall_rise_edge_detected;
	int single_multi_detected;
	int event_missed_cnt;
	fal_ptp_time_t tim;
} fal_ptp_capture_status_t;

typedef struct {
	fal_ptp_capture_conf_t capture_conf;
	fal_ptp_capture_status_t capture_status;
} fal_ptp_capture_t;

enum {
	FAL_PTP_INTR_EXPAND = 0,/* expand interrupt, FAL_PTP_INTR_TX_GTSE ~ FAL_PTP_INTR_CAP1 */
	FAL_PTP_INTR_RX,	/* ptp pkt rx interrupt */
	FAL_PTP_INTR_TX,	/* ptp pkt tx interrupt */
	FAL_PTP_INTR_TX_GTSE,	/* gtse pkt tx interrupt */
	FAL_PTP_INTR_RX_GTSE,	/* gtse pkt rx interrupt */
	FAL_PTP_INTR_TX_BUF,	/* uart tod tx buffer half full interrupt */
	FAL_PTP_INTR_RX_BUF,	/* uart tod rx buffer half full interrupt */
	FAL_PTP_INTR_PPS_OUT,	/* PPS output interrupt */
	FAL_PTP_INTR_PPS_IN,	/* PPS input interrupt */
	FAL_PTP_INTR_10MS,	/* each 10ms interrupt */
	FAL_PTP_INTR_TRIG0,	/* trigger0 interrupt */
	FAL_PTP_INTR_TRIG1,	/* trigger1 interrupt */
	FAL_PTP_INTR_CAP0,	/* capture0 interrupt */
	FAL_PTP_INTR_CAP1	/* capture1 interrupt */
};

typedef struct {
	a_uint32_t intr_mask;
	a_uint32_t intr_status;
} fal_ptp_interrupt_t;

sw_error_t
fal_ptp_config_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_config_t *config);

sw_error_t
fal_ptp_config_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_config_t *config);

sw_error_t
fal_ptp_reference_clock_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_reference_clock_t ref_clock);

sw_error_t
fal_ptp_reference_clock_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_reference_clock_t *ref_clock);

sw_error_t
fal_ptp_rx_timestamp_mode_set(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_rx_timestamp_mode_t ts_mode);

sw_error_t
fal_ptp_rx_timestamp_mode_get(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_rx_timestamp_mode_t *ts_mode);

sw_error_t
fal_ptp_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_direction_t direction,
		fal_ptp_pkt_info_t *pkt_info,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_pkt_timestamp_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_pkt_timestamp_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_grandmaster_mode_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_grandmaster_mode_t *gm_mode);

sw_error_t
fal_ptp_grandmaster_mode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_grandmaster_mode_t *gm_mode);

sw_error_t
fal_ptp_rtc_time_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_rtc_time_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_rtc_time_clear(a_uint32_t dev_id, a_uint32_t port_id);

sw_error_t
fal_ptp_rtc_adjtime_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_rtc_adjfreq_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_rtc_adjfreq_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_link_delay_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_link_delay_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_time_t *time);

sw_error_t
fal_ptp_security_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_security_t *sec);

sw_error_t
fal_ptp_security_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_security_t *sec);

sw_error_t
fal_ptp_pps_signal_control_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_pps_signal_control_t *sig_control);

sw_error_t
fal_ptp_pps_signal_control_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_pps_signal_control_t *sig_control);

sw_error_t
fal_ptp_rx_crc_recalc_enable(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t status);

sw_error_t
fal_ptp_rx_crc_recalc_status_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t *status);

sw_error_t
fal_ptp_asym_correction_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_asym_correction_t *asym_cf);

sw_error_t
fal_ptp_asym_correction_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_asym_correction_t* asym_cf);

sw_error_t
fal_ptp_output_waveform_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_output_waveform_t *waveform);

sw_error_t
fal_ptp_output_waveform_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_output_waveform_t *waveform);

sw_error_t
fal_ptp_rtc_time_snapshot_enable(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t status);

sw_error_t
fal_ptp_rtc_time_snapshot_status_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_bool_t *status);

sw_error_t
fal_ptp_increment_sync_from_clock_enable(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t status);

sw_error_t
fal_ptp_increment_sync_from_clock_status_get(a_uint32_t dev_id,
		a_uint32_t port_id, a_bool_t *status);

sw_error_t
fal_ptp_tod_uart_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_tod_uart_t *tod_uart);

sw_error_t
fal_ptp_tod_uart_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_tod_uart_t *tod_uart);

sw_error_t
fal_ptp_enhanced_timestamp_engine_set(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine);

sw_error_t
fal_ptp_enhanced_timestamp_engine_get(a_uint32_t dev_id,
		a_uint32_t port_id, fal_ptp_direction_t direction,
		fal_ptp_enhanced_ts_engine_t *ts_engine);

sw_error_t
fal_ptp_trigger_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger);

sw_error_t
fal_ptp_trigger_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t trigger_id, fal_ptp_trigger_t *triger);

sw_error_t
fal_ptp_capture_set(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture);

sw_error_t
fal_ptp_capture_get(a_uint32_t dev_id, a_uint32_t port_id,
		a_uint32_t capture_id, fal_ptp_capture_t *capture);

sw_error_t
fal_ptp_interrupt_set(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_interrupt_t *interrupt);

sw_error_t
fal_ptp_interrupt_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_ptp_interrupt_t *interrupt);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _FAL_PTP_H_ */
/**
 * @}
 */
