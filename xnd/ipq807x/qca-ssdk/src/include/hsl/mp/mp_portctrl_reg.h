/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef MAPLE_PORTCTRL_REG_H
#define MAPLE_PORTCTRL_REG_H

/*[register] MAC_CONFIGURATION*/
#define MAC_CONFIGURATION
#define MAC_CONFIGURATION_ADDRESS 0x0
#define MAC_CONFIGURATION_NUM     2
#define MAC_CONFIGURATION_INC     0x100000
#define MAC_CONFIGURATION_DEFAULT 0x0

struct mac_configuration {
	a_uint32_t  preamble_length:2;
	a_uint32_t  rx_enable:1;
	a_uint32_t  tx_enable:1;
	a_uint32_t  deferral_check:1;
	a_uint32_t  back_off_limit:2;
	a_uint32_t  acs:1;
	a_uint32_t  link_status:1;
	a_uint32_t  disable_retry:1;
	a_uint32_t  checksum_offload:1;
	a_uint32_t  duplex:1;
	a_uint32_t  loopback:1;
	a_uint32_t  disable_receive_own:1;
	a_uint32_t  mii_speed:1;
	a_uint32_t  port_select:1;
	a_uint32_t  disable_carrier_sense:1;
	a_uint32_t  ipg:3;
	a_uint32_t  jumbo_frame_enable:1;
	a_uint32_t  frame_burst_enable:1;
	a_uint32_t  jabber_disable:1;
	a_uint32_t  watchdog_disable:1;
	a_uint32_t  tc:1;
	a_uint32_t  crc_strpping:1;
	a_uint32_t  sfterr:1;
	a_uint32_t  twokpe:1;
	a_uint32_t  sarc:3;
	a_uint32_t  reserved:1;

};

union mac_configuration_u {
	a_uint32_t val;
	struct mac_configuration bf;
};

/*[register] MAC_FRAME_FILTER*/
#define MAC_FRAME_FILTER
#define MAC_FRAME_FILTER_ADDRESS 0x4
#define MAC_FRAME_FILTER_NUM     2
#define MAC_FRAME_FILTER_INC     0x100000
#define MAC_FRAME_FILTER_DEFAULT 0x0

struct mac_frame_filter {
	a_uint32_t  promiscuous_mode:1;
	a_uint32_t  hash_unicast:1;
	a_uint32_t  hash_multicast:1;
	a_uint32_t  da_inverse_filtering:1;
	a_uint32_t  pass_multicast:1;
	a_uint32_t  disable_broadcast:1;
	a_uint32_t  pass_control_frame:2;
	a_uint32_t  sa_inverse_filtering:1;
	a_uint32_t  source_addr_filter_enable:1;
	a_uint32_t  perfect_filter:1;
	a_uint32_t  reserved_0:5;
	a_uint32_t  vlan_tag_filter_enable:1;
	a_uint32_t  resereved_1:3;
	a_uint32_t  layer_3_4_filter_enable:1;
	a_uint32_t  drop_non_tcp_udp:1;
	a_uint32_t  reserved_2:9;
	a_uint32_t  receive_all:1;
};

union mac_frame_filter_u {
	a_uint32_t val;
	struct mac_frame_filter bf;
};

/*[register] MAC_FLOW_CTRL*/
#define MAC_FLOW_CTRL
#define MAC_FLOW_CTRL_ADDRESS 0x18
#define MAC_FLOW_CTRL_NUM     2
#define MAC_FLOW_CTRL_INC     0x100000
#define MAC_FLOW_CTRL_DEFAULT 0x0

struct mac_flow_ctrl {
	a_uint32_t  flowctrl_busy:1;
	a_uint32_t  flowctrl_tx_enable:1;
	a_uint32_t  flowctrl_rx_enable:1;
	a_uint32_t  unicast_pause_frame_detect:1;
	a_uint32_t  pause_low_threshold:2;
	a_uint32_t  reserved_0:1;
	a_uint32_t  disable_zero_quanta_pause:1;
	a_uint32_t  reserved_1:8;
	a_uint32_t  pause_time:16;
};

union mac_flow_ctrl_u {
	a_uint32_t val;
	struct mac_flow_ctrl bf;
};

/*[register] LPI_CONTROL_STATUS*/
#define MAC_LPI_CTRL_STATUS
#define MAC_LPI_CTRL_STATUS_ADDRESS 0x30
#define MAC_LPI_CTRL_STATUS_NUM     2
#define MAC_LPI_CTRL_STATUS_INC     0x100000
#define MAC_LPI_CTRL_STATUS_DEFAULT 0x0

struct mac_lpi_ctrl_status {
	a_uint32_t  tx_lpi_entry:1;
	a_uint32_t  tx_lpi_exit:1;
	a_uint32_t  rx_lpi_entry:1;
	a_uint32_t  rx_lpi_exit:1;
	a_uint32_t  reserved_0:4;
	a_uint32_t  tx_lpi_state:1;
	a_uint32_t  rx_lpi_state:1;
	a_uint32_t  reserved_1:6;
	a_uint32_t  lpi_enable:1;
	a_uint32_t  link_status:1;
	a_uint32_t  link_status_enable:1;
	a_uint32_t  lpi_tx_auto_enable:1;
	a_uint32_t  reserved_2:12;
};

union mac_lpi_ctrl_status_u {
	a_uint32_t val;
	struct mac_lpi_ctrl_status bf;
};

/*[register] LPI_TIMER_CONTROL*/
#define MAC_LPI_TIMER_CTRL
#define MAC_LPI_TIMER_CTRL_ADDRESS 0x34
#define MAC_LPI_TIMER_CTRL_NUM     2
#define MAC_LPI_TIMER_CTRL_INC     0x100000
#define MAC_LPI_TIMER_CTRL_DEFAULT 0x0

struct mac_lpi_timer_ctrl {
	a_uint32_t  lpi_tw_timer:16;
	a_uint32_t  lpi_ls_timer:10;
	a_uint32_t  reserved_0:6;
};

union mac_lpi_timer_ctrl_u {
	a_uint32_t val;
	struct mac_lpi_timer_ctrl bf;
};

/*[register] MAX_FRAME_CONTROL*/
#define MAC_MAX_FRAME_CTRL
#define MAC_MAX_FRAME_CTRL_ADDRESS 0xDC
#define MAC_MAX_FRAME_CTRL_NUM     2
#define MAC_MAX_FRAME_CTRL_INC     0x100000
#define MAC_MAX_FRAME_CTRL_DEFAULT 0x0

struct mac_max_frame_ctrl {
	a_uint32_t  max_frame_ctrl:14;
	a_uint32_t  reserved_0:2;
	a_uint32_t  max_frame_ctrl_enable:1;
};

union mac_max_frame_ctrl_u {
	a_uint32_t val;
	struct mac_max_frame_ctrl bf;
};

/*[register] OPERATION_MODE*/
#define MAC_OPERATION_MODE_CTRL
#define MAC_OPERATION_MODE_CTRL_ADDRESS 0x01018
#define MAC_OPERATION_MODE_CTRL_NUM     2
#define MAC_OPERATION_MODE_CTRL_INC     0x100000
#define MAC_OPERATION_MODE_CTRL_DEFAULT 0x0

struct mac_operation_mode_ctrl {
	a_uint32_t  reserved_0:1;
	a_uint32_t  stop_receive:1;
	a_uint32_t  second_frame:1;
	a_uint32_t  receive_threshold_ctrl:2;
	a_uint32_t  drop_gaint_frame:1;
	a_uint32_t  forwad_good_undersize_frame:1;
	a_uint32_t  forward_error_frame:1;
	a_uint32_t  enable_hw_flowctrl:1;
	a_uint32_t  threshold_of_activating:2;
	a_uint32_t  threshold_of_deactivating:2;
	a_uint32_t  stop_transmission_command:1;
	a_uint32_t  transmit_threshold_ctrl:3;
	a_uint32_t  reserved_1:3;
	a_uint32_t  flush_transmit_fifo:1;
	a_uint32_t  transmit_store_and_foward:1;
	a_uint32_t  msb_threshold_of_deactivating:1;
	a_uint32_t  msb_threshold_of_activating:1;
	a_uint32_t  disable_flushing_receiving_frame:1;
	a_uint32_t  receive_store_and_foward:1;
	a_uint32_t  disable_dropping_checking_error_frame:1;
	a_uint32_t  reserved_2:5;
};

union mac_operation_mode_ctrl_u {
	a_uint32_t val;
	struct mac_operation_mode_ctrl bf;
};

#endif

