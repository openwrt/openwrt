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
	FAL_OC_CLOCK_MODE = 0, /* OC clock mode */
	FAL_BC_CLOCK_MODE,     /* BC clock mode */
	FAL_E2ETC_CLOCK_MODE,  /* E2E TC clock mode */
	FAL_P2PTC_CLOCK_MODE   /* P2P TC clock mode */
} fal_ptp_clock_mode_t;

typedef enum {
	FAL_ONE_STEP_MODE = 0, /* one step mode */
	FAL_TWO_STEP_MODE,     /* two step mode */
	FAL_AUTO_MODE	       /* step mode is selected through packet field */
} fal_ptp_step_mode_t;

typedef struct {
	a_bool_t ptp_en;	         /* enable/disable ptp feature */
	fal_ptp_clock_mode_t clock_mode; /* clock mode */
	fal_ptp_step_mode_t step_mode;   /* step mode */
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
	a_int64_t seconds;         /* second field */
	a_int32_t nanoseconds;     /* nano second field */
	a_int32_t fracnanoseconds; /* fraction nano second field */
} fal_ptp_time_t;

typedef enum {
	FAL_RX_DIRECTION = 0,     /* ptp rx direction */
	FAL_TX_DIRECTION          /* ptp tx direction */
} fal_ptp_direction_t;

typedef struct{
	a_uint32_t sequence_id;		/* packet field: sequenceId */
	a_uint64_t clock_identify;	/* packet field: ClockIdentify */
	a_uint32_t port_number;		/* packet field: SourcePortID */
	a_uint32_t msg_type;		/* packet field: message type */
} fal_ptp_pkt_info_t;

typedef enum {
	FAL_GM_PPSIN_MODE = 0,	/* use PSSIN mode to sync nanoseconds */
	FAL_GM_HWPLL_MODE,	/* use HWPLL mode to sync nanoseconds */
	FAL_GM_SWPLL_MODE,	/* use SWPLL mode to sync nanoseconds */
} fal_ptp_grandmaster_ns_sync_mode_t;

typedef enum {  /* maximun frequency offset of rtc_clk from PPS source */
	FAL_GM_MAXFREQ_1PPM = 0,
	FAL_GM_MAXFREQ_10PPM,
	FAL_GM_MAXFREQ_50PPM,
	FAL_GM_MAXFREQ_100PPM,
	FAL_GM_MAXFREQ_150PPM,
	FAL_GM_MAXFREQ_200PPM,    /* 802.3 standard select this value */
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
	fal_ptp_grandmaster_ns_sync_mode_t ns_sync_mode; /* nano second sync mode */
} fal_ptp_grandmaster_mode_t;

typedef struct {
	a_bool_t address_check_en; /* address should matched when regard it as PTP packet */
	a_bool_t ipv6_udp_checksum_recal_en; /* recalculate ipv6 udp checksum on TX direction */
	a_bool_t version_check_en; /* PTP version should matched when regard it as PTP packet */
	a_uint32_t ptp_version;    /* ptp version number to match */
	a_bool_t ipv4_udp_checksum_force_zero_en; /* force ipv4 checksum to 0 on TX direction */
	a_bool_t ipv6_embed_udp_checksum_force_zero_en; /* force RX ipv6 checksum to 0 */
} fal_ptp_security_t;

typedef struct {
	a_bool_t negative_in_latency;	/* ingress latency value is positive or negative */
	a_uint32_t in_latency;		/* ingress latency value */
	a_uint32_t out_phase;		/* adjust the phase of PPS outout signal */
	a_uint32_t out_pulse_width;	/* adjust the pulse width of PPS outout signal */
} fal_ptp_pps_signal_control_t;

typedef struct {
	a_bool_t eg_asym_en;	  /* enable egress asymmetry correction */
	a_bool_t in_asym_en;	  /* enable ingress asymmetry correction */
	a_uint32_t eg_asym_value; /* egress asymmetry correction value */
	a_uint32_t in_asym_value; /* ingress asymmetry correction value */
} fal_ptp_asym_correction_t;

typedef enum {
	FAL_WAVE_FREQ = 0,	/* select wave period as SYNC_CLKO_PTP output */
	FAL_PULSE_10MS,		/* select pulse 10ms as SYNC_CLKO_PTP output */
	FAL_TRIGGER0_GPIO,	/* select trigger0 status as SYNC_CLKO_PTP output */
	FAL_RX_PTP_STATE,	/* select RX PTP state as SYNC_CLKO_PTP output */
} fal_ptp_waveform_type_t;

typedef struct {
	fal_ptp_waveform_type_t waveform_type; /* select the wave output type */
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
	a_uint16_t baud_config;	/* baud_config = (125 * 10^6)/(16)/baud_rate for speed <= 1G */
				/* baud_config = (200 * 10^6)/(16)/baud_rate for speed >= 2.5 G */
	a_uint32_t uart_config_bmp; /* refer to FAL_UART_START_POLARITY_HIGH_EN */
	a_bool_t reset_buf_en;	/* reset TOD UART RX/TX buffer, self clearing */
	a_uint32_t buf_status_bmp; /* refer to FAL_UART_RX_BUFFER_DATA_PRESENT */
	a_uint16_t tx_buf_value;   /* the uart data to transport */
	a_uint16_t rx_buf_value;   /* the uart data received */
} fal_ptp_tod_uart_t;

enum {
	FAL_ENHANCED_TS_ETH_TYPE_EN = 0,     /* timestamp the matched ethernet type */
	FAL_ENHANCED_TS_DMAC_EN,             /* timestamp the matched dst mac */
	FAL_ENHANCED_TS_RESV_DMAC_EN,        /* timestamp the matched ptp dst mac */
	FAL_ENHANCED_TS_IPV4_L4_PROTO_EN,    /* timestamp the matched layer 4 protocol id */
	FAL_ENHANCED_TS_IPV4_DIP_EN,         /* timestamp the matched ipv4 dst addr */
	FAL_ENHANCED_TS_RESV_IPV4_DIP_EN,    /* timestamp the matched ptp ipv4 dst addr */
	FAL_ENHANCED_TS_IPV6_NEXT_HEADER_EN, /* timestamp the matched ipv6 next header field */
	FAL_ENHANCED_TS_IPV6_DIP_EN,         /* timestamp the matched ipv6 dst addr */
	FAL_ENHANCED_TS_RESV_IPV6_DIP_EN,    /* timestamp the matched ptp ipv6 dst addr */
	FAL_ENHANCED_TS_UDP_DPORT_EN,        /* timestamp the matched udp dport number */
	FAL_ENHANCED_TS_RESV_UDP_DPORT_EN,   /* timestamp the matched udp dport number */
	FAL_ENHANCED_TS_Y1731_EN,            /* timestamp the received Y1731 frame */
	FAL_ENHANCED_TS_Y1731_TIMESTAMP_INSERT_EN, /* enable inserting RX Y1731 timestamp */
	FAL_ENHANCED_TS_Y1731_MAC_EN /* TX direction check smac, RX direction check dmac */
};

enum {
	FAL_ENHANCED_TS_ETH_TYPE_STATUS = 0,        /* the MAC type of received packet matches
						      the filter setting */
	FAL_ENHANCED_TS_DMAC_STATUS,                /* the dest MAC of received packet matches
						      the filter setting */
	FAL_ENHANCED_TS_RESV_PRIM_DMAC_STATUS,      /* the dest MAC of received packet matches
						      PTP primary multicast MAC address */
	FAL_ENHANCED_TS_RESV_PDELAY_DMAC_STATUS,    /* the dest MAC of received packet matches
						      PTP peer delay multicast MAC address */
	FAL_ENHANCED_TS_IPV4_L4_PROTO_STATUS,       /* the L4 layer protocol of received packet
						      matched the filter setting */
	FAL_ENHANCED_TS_IPV4_DIP_STATUS,            /* the IPv4 dest address of received packet
						      matched the filter setting */
	FAL_ENHANCED_TS_RESV_IPV4_PRIM_DIP_STATUS,  /* the IPv4 dest address of received packet
						      matches PTP primary multicast ipv4 addr */
	FAL_ENHANCED_TS_RESV_IPV4_PDELAY_DIP_STATUS,/* the IPv4 dest address of received packet
							matches PTP peer delay multicast addr */
	FAL_ENHANCED_TS_IPV6_NEXT_HEADER_STATUS,    /* the next header fielf of receviced packet
						       matches the filter setting */
	FAL_ENHANCED_TS_IPV6_DIP_STATUS,            /* the ipv6 dest addr of received packet
						       matches the filter setting */
	FAL_ENHANCED_TS_RESV_IPV6_PRIM_DIP_STATUS,  /* the ipv6 dest address of received packet
						       matches PTP primary multicast ipv6 addr */
	FAL_ENHANCED_TS_RESV_IPV6_PDELAY_DIP_STATUS,/* the ipv6 dest address of received packet
						       matches PTP peer delay multicast addr */
	FAL_ENHANCED_TS_UDP_DPORT_STATUS,           /* the UDP dest port of received packet
						       matches the filter setting */
	FAL_ENHANCED_TS_RESV_UDP_DPORT_STATUS,      /* the UDP dest port of received packet
						       matches udp dport of ptp event packet */
	FAL_ENHANCED_TS_Y1731_MATCH_STATUS          /* the received frame is Y.1731 OAM frame */
};

typedef struct {
	a_bool_t filt_en;	/* enable/disable filter feature */
	a_uint32_t enhance_ts_conf_bmp; /* refer to FAL_ENHANCED_TS_ETH_TYPE_EN */
	a_uint32_t eth_type;            /* ethernet type value */
	fal_mac_addr_t dmac_addr;       /* dest mac address */
	a_uint32_t ipv4_l4_proto;       /* ipv4 layer 4 protocol field */
	fal_ip4_addr_t ipv4_dip;        /* ipv4 dest ip addr */
	fal_ip6_addr_t ipv6_dip;        /* ipv6 dest ip addr */
	a_uint32_t udp_dport;           /* udp dest port */
	fal_mac_addr_t y1731_mac_addr;	/* TX check smac, RX check dmac */
	a_uint32_t enhance_ts_status_bmp; 	/* refer to FAL_ENHANCED_TS_ETH_TYPE_STATUS */
	a_uint32_t enhance_ts_status_pre_bmp;	/* TX NOT SUPPORT */
	a_uint32_t y1731_identity;	/* save Y1731 identify value */
	a_uint32_t y1731_identity_pre;	/* TX NOT SUPPORT */
	fal_ptp_time_t timestamp;	/* save packet timestamp when matched */
	fal_ptp_time_t timestamp_pre;	/* TX NOT SUPPORT */
} fal_ptp_enhanced_ts_engine_t;

typedef struct {
	a_bool_t trigger_en;            /* enable trigger or not */
	a_bool_t output_force_en;       /* force trigger output a force value */
	int output_force_value;         /* the forced value */
	int patten_select;              /* trigger pattern:
					   0 single rising edge;
					   1 single falling edge;
					   2 trigger pulse;
					   3 trigger periodic waveform;
					   4 toggle mode;
					 */
	int late_operation;             /* if later, trigger immediately */
	int notify;                     /* report the completion of trigger */
	int trigger_effect;             /* write 1 to generate a high pulse when trigger happen */
	fal_ptp_time_t tim;             /* the trigger timestamp */
} fal_ptp_trigger_conf_t;

typedef struct {
	int trigger_finished;           /* trigger finished or not */
	int trigger_active;             /* trigger is active or not */
	int trigger_error;              /* trigger error status
					   0 no error
					   1 trigger time prior to current time
					   2 initial value error for edge trigger
					 */
} fal_ptp_trigger_status_t;

typedef struct {
	fal_ptp_trigger_conf_t trigger_conf;
	fal_ptp_trigger_status_t trigger_status;
} fal_ptp_trigger_t;

typedef struct {
	int status_clear;              /* clear event status register */
	int notify_event;              /* notify event through interrupt */
	int single_multi_select;       /* 1 for single or 0 for multi event capture */
	a_bool_t fall_edge_en;         /* enable falling edge detection */
	a_bool_t rise_edge_en;         /* enable rising edge detection */
} fal_ptp_capture_conf_t;

typedef struct {
	int event_detected;            /* event detected or not */
	int fall_rise_edge_detected;   /* 0 for rising edge, 1 for falling edge detected */
	int single_multi_detected;     /* 0 for single event, 1 for multi event detected */
	int event_missed_cnt;          /* the number of events missed */
	fal_ptp_time_t tim;            /* event timestamp */
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
