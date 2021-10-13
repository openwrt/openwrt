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

#ifndef _MP_MIB_H_
#define _MP_MIB_H_

#define MMC_CONTROL_MAX_ENTRY	2
#define TX_OCTET_COUNT_GOOD_BAD_MAX_ENTRY	2
#define TX_FRAME_COUNT_GOOD_BAD_MAX_ENTRY	2
#define TX_BROADCAST_FRAMES_GOOD_MAX_ENTRY	2
#define TX_MULTICAST_FRAMES_GOOD_MAX_ENTRY	2
#define TX_64OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define TX_65TO127OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define TX_128TO255OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define TX_256TO511OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define TX_512TO1023OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define TX_UNICAST_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define TX_MULTICAST_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define TX_BROADCAST_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define TX_UNDERFERROR_FRAMES_MAX_ENTRY	2
#define TX_SINGLE_COLLISION_GOOD_FRAMES_MAX_ENTRY	2
#define TX_MULTIPLE_COLLISION_GOOD_FRAMES_MAX_ENTRY	2
#define TX_DEFERRED_FRAMES_MAX_ENTRY	2
#define TX_LATE_COLLISION_FRAMES_MAX_ENTRY	2
#define TX_EXCESSIVE_COLLISION_FRAMES_MAX_ENTRY	2
#define TX_CARRIER_ERROR_FRAMES_MAX_ENTRY	2
#define TX_OCTET_COUNT_GOOD_MAX_ENTRY	2
#define TX_FRAME_COUNT_GOOD_MAX_ENTRY	2
#define TX_PAUSE_FRAMES_MAX_ENTRY	2
#define TX_VLAN_FRAMES_GOOD_MAX_ENTRY	2
#define TX_OSIZE_FRAMES_GOOD_MAX_ENTRY 2
#define RX_FRAME_COUNT_GOOD_BAD_MAX_ENTRY	2
#define RX_OCTET_COUNT_GOOD_BAD_MAX_ENTRY	2
#define RX_OCTET_COUNT_GOOD_MAX_ENTRY	2
#define RX_BROADCAST_FRAMES_GOOD_MAX_ENTRY	2
#define RX_MULTICAST_FRAMES_GOOD_MAX_ENTRY	2
#define RX_CRC_ERROR_FRAMES_MAX_ENTRY	2
#define RX_ALIGNMENT_ERROR_FRAMES_MAX_ENTRY	2
#define RX_RUNT_ERROR_FRAMES_MAX_ENTRY	2
#define RX_JABBER_ERROR_FRAMES_MAX_ENTRY	2
#define RX_UNDERSIZE_FRAMES_GOOD_MAX_ENTRY	2
#define RX_OVERSIZE_FRAMES_GOOD_MAX_ENTRY	2
#define RX_64OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define RX_65TO127OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define RX_128TO255OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define RX_256TO511OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define RX_512TO1023OCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define RX_UNICAST_FRAMES_GOOD_MAX_ENTRY	2
#define RX_LENGTH_ERROR_FRAMES_MAX_ENTRY	2
#define RX_OUTOFRANGE_FRAMES_MAX_ENTRY	2
#define RX_PAUSE_FRAMES_MAX_ENTRY	2
#define RX_FIFO_OVER_FLOW_FRAMES_MAX_ENTRY	2
#define RX_VLAN_FRAMES_GOOD_BAD_MAX_ENTRY	2
#define RX_WATCHDOG_ERROR_FRAMES_MAX_ENTRY	2
#define RX_RECEIVE_ERROR_FRAMES_MAX_ENTRY	2
#define RX_CONTROL_FRAMES_GOOD_MAX_ENTRY	2

sw_error_t
mp_mmc_control_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_control_u *value);

sw_error_t
mp_mmc_control_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_control_u *value);

sw_error_t
mp_tx_octet_count_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_bad_u *value);

sw_error_t
mp_tx_frame_count_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_bad_u *value);

sw_error_t
mp_tx_broadcast_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_u *value);

sw_error_t
mp_tx_multicast_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_u *value);

sw_error_t
mp_tx_64octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_64octets_frames_good_bad_u *value);

sw_error_t
mp_tx_65to127octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_65to127octets_frames_good_bad_u *value);

sw_error_t
mp_tx_128to255octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_128to255octets_frames_good_bad_u *value);

sw_error_t
mp_tx_256to511octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_256to511octets_frames_good_bad_u *value);

sw_error_t
mp_tx_512to1023octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_512to1023octets_frames_good_bad_u *value);

sw_error_t
mp_tx_1024tomaxoctets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_1024tomaxoctets_frames_good_bad_u *value);

sw_error_t
mp_tx_unicast_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_unicast_frames_good_bad_u *value);

sw_error_t
mp_tx_multicast_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_bad_u *value);

sw_error_t
mp_tx_broadcast_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_bad_u *value);

sw_error_t
mp_tx_underflow_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_underflow_error_frames_u *value);

sw_error_t
mp_tx_single_col_good_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_single_collision_good_frames_u *value);


sw_error_t
mp_t_multi_col_good_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multiple_collision_good_frames_u *value);

sw_error_t
mp_tx_defer_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_deferred_frames_u *value);

sw_error_t
mp_tx_late_col_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_late_collision_frames_u *value);

sw_error_t
mp_tx_excessive_col_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_excessive_collision_frames_u *value);

sw_error_t
mp_tx_carrier_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_carrier_error_frames_u *value);

sw_error_t
mp_tx_octet_count_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_u *value);

sw_error_t
mp_tx_frame_count_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_u *value);

sw_error_t
mp_tx_pause_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_pause_frames_u *value);

sw_error_t
mp_tx_vlan_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_vlan_frames_good_u *value);

sw_error_t
mp_tx_osize_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_osize_frames_good_u *value);

sw_error_t
mp_rx_frame_count_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_frame_count_good_bad_u *value);

sw_error_t
mp_rx_octet_count_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_bad_u *value);

sw_error_t
mp_rx_octet_count_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_u *value);

sw_error_t
mp_rx_broadcast_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_broadcast_frames_good_u *value);

sw_error_t
mp_rx_multicast_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_multicast_frames_good_u *value);

sw_error_t
mp_rx_crc_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_u *value);

sw_error_t
mp_rx_alignment_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_u *value);

sw_error_t
mp_rx_runt_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_runt_error_frames_u *value);

sw_error_t
mp_rx_jabber_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_jabber_error_frames_u *value);

sw_error_t
mp_rx_undersize_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_undersize_frames_good_u *value);

sw_error_t
mp_rx_oversize_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_oversize_frames_good_u *value);

sw_error_t
mp_rx_64octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_64octets_frames_good_bad_u *value);

sw_error_t
mp_rx_65to127octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_65to127octets_frames_good_bad_u *value);

sw_error_t
mp_rx_128to255octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_128to255octets_frames_good_bad_u *value);

sw_error_t
mp_rx_256to511octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_256to511octets_frames_good_bad_u *value);

sw_error_t
mp_rx_512to1023octets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_512to1023octets_frames_good_bad_u *value);

sw_error_t
mp_rx_1024tomaxoctets_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_1024tomaxoctets_frames_good_bad_u *value);

sw_error_t
mp_rx_unicast_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_unicast_frames_good_u *value);

sw_error_t
mp_rx_length_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_length_error_frames_u *value);

sw_error_t
mp_rx_outofrange_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_outofrange_frames_u *value);

sw_error_t
mp_rx_pause_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_pause_frames_u *value);

sw_error_t
mp_rx_fifo_over_flow_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifo_over_flow_frames_u *value);

sw_error_t
mp_rx_vlan_frames_good_bad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_vlan_frames_good_bad_u *value);

sw_error_t
mp_rx_watchdog_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_watchdog_error_frames_u *value);

sw_error_t
mp_rx_receive_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_receive_error_frames_u *value);

sw_error_t
mp_rx_control_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_control_frames_good_u *value);
#endif

