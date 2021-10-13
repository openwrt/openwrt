/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
#ifndef _HPPE_XGMACMIB_H_
#define _HPPE_XGMACMIB_H_

#define MMC_CONTROL_MAX_ENTRY	2
#define MMC_RECEIVE_INTERRUPT_MAX_ENTRY	2
#define MMC_TRANSMIT_INTERRUPT_MAX_ENTRY	2
#define MMC_RECEIVE_INTERRUPT_ENABLE_MAX_ENTRY	2
#define MMC_TRANSMIT_INTERRUPT_ENABLE_MAX_ENTRY	2
#define TX_OCTET_COUNT_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_OCTET_COUNT_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_FRAME_COUNT_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_FRAME_COUNT_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_BROADCAST_FRAMES_GOOD_LOW_MAX_ENTRY	2
#define TX_BROADCAST_FRAMES_GOOD_HIGH_MAX_ENTRY	 2
#define TX_MULTICAST_FRAMES_GOOD_LOW_MAX_ENTRY	2
#define TX_MULTICAST_FRAMES_GOOD_HIGH_MAX_ENTRY	2
#define TX_64OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_64OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_65TO127OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_65TO127OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_128TO255OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_128TO255OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_256TO511OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_256TO511OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_512TO1023OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_512TO1023OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_UNICAST_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_UNICAST_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_MULTICAST_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_MULTICAST_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_BROADCAST_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define TX_BROADCAST_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define TX_UNDERFLOW_ERROR_FRAMES_LOW_MAX_ENTRY	2
#define TX_UNDERFLOW_ERROR_FRAMES_HIGH_MAX_ENTRY	2
#define TX_OCTET_COUNT_GOOD_LOW_MAX_ENTRY	 2
#define TX_OCTET_COUNT_GOOD_HIGH_MAX_ENTRY	2
#define TX_FRAME_COUNT_GOOD_LOW_MAX_ENTRY	 2
#define TX_FRAME_COUNT_GOOD_HIGH_MAX_ENTRY	2
#define TX_PAUSE_FRAMES_LOW_MAX_ENTRY	 2
#define TX_PAUSE_FRAMES_HIGH_MAX_ENTRY	2
#define TX_VLAN_FRAMES_GOOD_LOW_MAX_ENTRY	2
#define TX_VLAN_FRAMES_GOOD_HIGH_MAX_ENTRY	2
#define TX_LPI_USEC_CNTR_MAX_ENTRY	2
#define TX_LPI_TRAN_CNTR_MAX_ENTRY	2
#define RX_FRAME_COUNT_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_FRAME_COUNT_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_OCTET_COUNT_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_OCTET_COUNT_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_OCTET_COUNT_GOOD_LOW_MAX_ENTRY	2
#define RX_OCTET_COUNT_GOOD_HIGH_MAX_ENTRY	2
#define RX_BROADCAST_FRAMES_GOOD_LOW_MAX_ENTRY	2
#define RX_BROADCAST_FRAMES_GOOD_HIGH_MAX_ENTRY	2
#define RX_MULTICAST_FRAMES_GOOD_LOW_MAX_ENTRY	2
#define RX_MULTICAST_FRAMES_GOOD_HIGH_MAX_ENTRY	2
#define RX_CRC_ERROR_FRAMES_LOW_MAX_ENTRY	2
#define RX_CRC_ERROR_FRAMES_HIGH_MAX_ENTRY	2
#define RX_RUNT_ERROR_FRAMES_MAX_ENTRY	2
#define RX_JABBER_ERROR_FRAMES_MAX_ENTRY	2
#define RX_UNDERSIZE_FRAMES_GOOD_MAX_ENTRY	2
#define RX_OVERSIZE_FRAMES_GOOD_MAX_ENTRY	2
#define RX_64OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_64OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_65TO127OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_65TO127OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_128TO255OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_128TO255OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_256TO511OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_256TO511OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_512TO1023OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_512TO1023OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_UNICAST_FRAMES_GOOD_LOW_MAX_ENTRY	2
#define RX_UNICAST_FRAMES_GOOD_HIGH_MAX_ENTRY	2
#define RX_LENGTH_ERROR_FRAMES_LOW_MAX_ENTRY	2
#define RX_LENGTH_ERROR_FRAMES_HIGH_MAX_ENTRY	2
#define RX_OUTOFRANGE_FRAMES_LOW_MAX_ENTRY	2
#define RX_OUTOFRANGE_FRAMES_HIGH_MAX_ENTRY	2
#define RX_PAUSE_FRAMES_LOW_MAX_ENTRY	2
#define RX_PAUSE_FRAMES_HIGH_MAX_ENTRY	2
#define RX_FIFOOVERFLOW_FRAMES_LOW_MAX_ENTRY	2
#define RX_FIFOOVERFLOW_FRAMES_HIGH_MAX_ENTRY	2
#define RX_VLAN_FRAMES_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_VLAN_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_WATCHDOG_ERROR_FRAMES_MAX_ENTRY	2
#define RX_LPI_USEC_CNTR_MAX_ENTRY	2
#define RX_LPI_TRAN_CNTR_MAX_ENTRY	2
#define RX_DISCARD_FRAME_COUNT_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_DISCARD_FRAME_COUNT_GOOD_BAD_HIGH_MAX_ENTRY	2
#define RX_DISCARD_OCTET_COUNT_GOOD_BAD_LOW_MAX_ENTRY	2
#define RX_DISCARD_OCTET_COUNT_GOOD_BAD_HIGH_MAX_ENTRY	2

sw_error_t
hppe_mmc_control_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_control_u *value);

sw_error_t
hppe_mmc_control_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_control_u *value);

sw_error_t
hppe_tx_octet_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_bad_low_u *value);

sw_error_t
hppe_tx_octet_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_bad_low_u *value);

sw_error_t
hppe_tx_octet_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_bad_high_u *value);

sw_error_t
hppe_tx_octet_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_bad_high_u *value);

sw_error_t
hppe_tx_frame_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_bad_low_u *value);

sw_error_t
hppe_tx_frame_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_bad_low_u *value);

sw_error_t
hppe_tx_frame_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_bad_high_u *value);

sw_error_t
hppe_tx_frame_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_bad_high_u *value);

sw_error_t
hppe_tx_broadcast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_low_u *value);

sw_error_t
hppe_tx_broadcast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_low_u *value);

sw_error_t
hppe_tx_broadcast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_high_u *value);

sw_error_t
hppe_tx_broadcast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_high_u *value);

sw_error_t
hppe_tx_multicast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_low_u *value);

sw_error_t
hppe_tx_multicast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_low_u *value);

sw_error_t
hppe_tx_multicast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_high_u *value);

sw_error_t
hppe_tx_multicast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_high_u *value);

sw_error_t
hppe_tx_64octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_64octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_64octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_64octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_64octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_64octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_64octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_64octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_65to127octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_65to127octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_65to127octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_65to127octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_65to127octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_65to127octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_65to127octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_65to127octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_128to255octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_128to255octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_128to255octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_128to255octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_128to255octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_128to255octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_128to255octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_128to255octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_256to511octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_256to511octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_256to511octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_256to511octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_256to511octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_256to511octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_256to511octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_256to511octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_512to1023octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_512to1023octets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_512to1023octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_512to1023octets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_1024tomaxoctets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_1024tomaxoctets_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_1024tomaxoctets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_1024tomaxoctets_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_unicast_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_unicast_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_unicast_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_unicast_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_unicast_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_unicast_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_unicast_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_unicast_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_multicast_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_multicast_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_multicast_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_multicast_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_broadcast_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_broadcast_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_bad_low_u *value);

sw_error_t
hppe_tx_broadcast_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_broadcast_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_bad_high_u *value);

sw_error_t
hppe_tx_underflow_error_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_underflow_error_frames_low_u *value);

sw_error_t
hppe_tx_underflow_error_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_underflow_error_frames_low_u *value);

sw_error_t
hppe_tx_underflow_error_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_underflow_error_frames_high_u *value);

sw_error_t
hppe_tx_underflow_error_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_underflow_error_frames_high_u *value);

sw_error_t
hppe_tx_octet_count_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_low_u *value);

sw_error_t
hppe_tx_octet_count_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_low_u *value);

sw_error_t
hppe_tx_octet_count_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_high_u *value);

sw_error_t
hppe_tx_octet_count_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_high_u *value);

sw_error_t
hppe_tx_frame_count_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_low_u *value);

sw_error_t
hppe_tx_frame_count_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_low_u *value);

sw_error_t
hppe_tx_frame_count_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_high_u *value);

sw_error_t
hppe_tx_frame_count_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_high_u *value);

sw_error_t
hppe_tx_pause_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_pause_frames_low_u *value);

sw_error_t
hppe_tx_pause_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_pause_frames_low_u *value);

sw_error_t
hppe_tx_pause_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_pause_frames_high_u *value);

sw_error_t
hppe_tx_pause_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_pause_frames_high_u *value);

sw_error_t
hppe_tx_vlan_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_vlan_frames_good_low_u *value);

sw_error_t
hppe_tx_vlan_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_vlan_frames_good_low_u *value);

sw_error_t
hppe_tx_vlan_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_vlan_frames_good_high_u *value);

sw_error_t
hppe_tx_vlan_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_vlan_frames_good_high_u *value);

sw_error_t
hppe_tx_lpi_usec_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_lpi_usec_cntr_u *value);

sw_error_t
hppe_tx_lpi_usec_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_lpi_usec_cntr_u *value);

sw_error_t
hppe_tx_lpi_tran_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_lpi_tran_cntr_u *value);

sw_error_t
hppe_tx_lpi_tran_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_lpi_tran_cntr_u *value);

sw_error_t
hppe_rx_frame_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_frame_count_good_bad_low_u *value);

sw_error_t
hppe_rx_frame_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_frame_count_good_bad_low_u *value);

sw_error_t
hppe_rx_frame_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_frame_count_good_bad_high_u *value);

sw_error_t
hppe_rx_frame_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_frame_count_good_bad_high_u *value);

sw_error_t
hppe_rx_octet_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_bad_low_u *value);

sw_error_t
hppe_rx_octet_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_bad_low_u *value);

sw_error_t
hppe_rx_octet_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_bad_high_u *value);

sw_error_t
hppe_rx_octet_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_bad_high_u *value);

sw_error_t
hppe_rx_octet_count_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_low_u *value);

sw_error_t
hppe_rx_octet_count_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_low_u *value);

sw_error_t
hppe_rx_octet_count_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_high_u *value);

sw_error_t
hppe_rx_octet_count_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_high_u *value);

sw_error_t
hppe_rx_broadcast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_broadcast_frames_good_low_u *value);

sw_error_t
hppe_rx_broadcast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_broadcast_frames_good_low_u *value);

sw_error_t
hppe_rx_broadcast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_broadcast_frames_good_high_u *value);

sw_error_t
hppe_rx_broadcast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_broadcast_frames_good_high_u *value);

sw_error_t
hppe_rx_multicast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_multicast_frames_good_low_u *value);

sw_error_t
hppe_rx_multicast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_multicast_frames_good_low_u *value);

sw_error_t
hppe_rx_multicast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_multicast_frames_good_high_u *value);

sw_error_t
hppe_rx_multicast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_multicast_frames_good_high_u *value);

sw_error_t
hppe_rx_crc_error_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_low_u *value);

sw_error_t
hppe_rx_crc_error_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_low_u *value);

sw_error_t
hppe_rx_crc_error_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_high_u *value);

sw_error_t
hppe_rx_crc_error_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_high_u *value);

sw_error_t
hppe_rx_runt_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_runt_error_frames_u *value);

sw_error_t
hppe_rx_runt_error_frames_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_runt_error_frames_u *value);

sw_error_t
hppe_rx_jabber_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_jabber_error_frames_u *value);

sw_error_t
hppe_rx_jabber_error_frames_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_jabber_error_frames_u *value);

sw_error_t
hppe_rx_undersize_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_undersize_frames_good_u *value);

sw_error_t
hppe_rx_undersize_frames_good_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_undersize_frames_good_u *value);

sw_error_t
hppe_rx_oversize_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_oversize_frames_good_u *value);

sw_error_t
hppe_rx_oversize_frames_good_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_oversize_frames_good_u *value);

sw_error_t
hppe_rx_64octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_64octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_64octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_64octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_64octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_64octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_64octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_64octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_65to127octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_65to127octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_65to127octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_65to127octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_65to127octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_65to127octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_65to127octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_65to127octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_128to255octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_128to255octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_128to255octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_128to255octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_128to255octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_128to255octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_128to255octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_128to255octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_256to511octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_256to511octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_256to511octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_256to511octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_256to511octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_256to511octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_256to511octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_256to511octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_512to1023octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_512to1023octets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_512to1023octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_512to1023octets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_1024tomaxoctets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_1024tomaxoctets_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_1024tomaxoctets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_1024tomaxoctets_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_unicast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_unicast_frames_good_low_u *value);

sw_error_t
hppe_rx_unicast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_unicast_frames_good_low_u *value);

sw_error_t
hppe_rx_unicast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_unicast_frames_good_high_u *value);

sw_error_t
hppe_rx_unicast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_unicast_frames_good_high_u *value);

sw_error_t
hppe_rx_length_error_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_length_error_frames_low_u *value);

sw_error_t
hppe_rx_length_error_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_length_error_frames_low_u *value);

sw_error_t
hppe_rx_length_error_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_length_error_frames_high_u *value);

sw_error_t
hppe_rx_length_error_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_length_error_frames_high_u *value);

sw_error_t
hppe_rx_outofrange_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_outofrange_frames_low_u *value);

sw_error_t
hppe_rx_outofrange_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_outofrange_frames_low_u *value);

sw_error_t
hppe_rx_outofrange_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_outofrange_frames_high_u *value);

sw_error_t
hppe_rx_outofrange_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_outofrange_frames_high_u *value);

sw_error_t
hppe_rx_pause_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_pause_frames_low_u *value);

sw_error_t
hppe_rx_pause_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_pause_frames_low_u *value);

sw_error_t
hppe_rx_pause_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_pause_frames_high_u *value);

sw_error_t
hppe_rx_pause_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_pause_frames_high_u *value);

sw_error_t
hppe_rx_fifooverflow_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifooverflow_frames_low_u *value);

sw_error_t
hppe_rx_fifooverflow_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifooverflow_frames_low_u *value);

sw_error_t
hppe_rx_fifooverflow_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifooverflow_frames_high_u *value);

sw_error_t
hppe_rx_fifooverflow_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifooverflow_frames_high_u *value);

sw_error_t
hppe_rx_vlan_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_vlan_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_vlan_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_vlan_frames_good_bad_low_u *value);

sw_error_t
hppe_rx_vlan_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_vlan_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_vlan_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_vlan_frames_good_bad_high_u *value);

sw_error_t
hppe_rx_watchdog_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_watchdog_error_frames_u *value);

sw_error_t
hppe_rx_watchdog_error_frames_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_watchdog_error_frames_u *value);

sw_error_t
hppe_rx_lpi_usec_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_lpi_usec_cntr_u *value);

sw_error_t
hppe_rx_lpi_usec_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_lpi_usec_cntr_u *value);

sw_error_t
hppe_rx_lpi_tran_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_lpi_tran_cntr_u *value);

sw_error_t
hppe_rx_lpi_tran_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_lpi_tran_cntr_u *value);

sw_error_t
hppe_rx_discard_frame_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_frame_count_good_bad_low_u *value);

sw_error_t
hppe_rx_discard_frame_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_frame_count_good_bad_low_u *value);

sw_error_t
hppe_rx_discard_frame_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_frame_count_good_bad_high_u *value);

sw_error_t
hppe_rx_discard_frame_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_frame_count_good_bad_high_u *value);

sw_error_t
hppe_rx_discard_octet_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_octet_count_good_bad_low_u *value);

sw_error_t
hppe_rx_discard_octet_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_octet_count_good_bad_low_u *value);

sw_error_t
hppe_rx_discard_octet_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_octet_count_good_bad_high_u *value);

sw_error_t
hppe_rx_discard_octet_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_octet_count_good_bad_high_u *value);


sw_error_t
hppe_mmc_control_cntrst_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_control_cntrst_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_control_rstonrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_control_rstonrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_control_cntstopro_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_control_cntstopro_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_control_mct_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_control_mct_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_control_pr_mmc_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_control_pr_mmc_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_control_cntprst_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_control_cntprst_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_control_mcf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_control_mcf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_octet_count_good_bad_low_txoctgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_octet_count_good_bad_low_txoctgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_octet_count_good_bad_high_txoctgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_octet_count_good_bad_high_txoctgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_frame_count_good_bad_low_txfrmgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_frame_count_good_bad_low_txfrmgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_frame_count_good_bad_high_txfrmgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_frame_count_good_bad_high_txfrmgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_broadcast_frames_good_low_txbcastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_broadcast_frames_good_low_txbcastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_broadcast_frames_good_high_txbcastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_broadcast_frames_good_high_txbcastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_multicast_frames_good_low_txmcastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_multicast_frames_good_low_txmcastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_multicast_frames_good_high_txmcastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_multicast_frames_good_high_txmcastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_64octets_frames_good_bad_low_tx64octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_64octets_frames_good_bad_low_tx64octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_64octets_frames_good_bad_high_tx64octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_64octets_frames_good_bad_high_tx64octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_65to127octets_frames_good_bad_low_tx65_127octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_65to127octets_frames_good_bad_low_tx65_127octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_65to127octets_frames_good_bad_high_tx65_127octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_65to127octets_frames_good_bad_high_tx65_127octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_128to255octets_frames_good_bad_low_tx128_255octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_128to255octets_frames_good_bad_low_tx128_255octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_128to255octets_frames_good_bad_high_tx128_255octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_128to255octets_frames_good_bad_high_tx128_255octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_256to511octets_frames_good_bad_low_tx256_511octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_256to511octets_frames_good_bad_low_tx256_511octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_256to511octets_frames_good_bad_high_tx256_511octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_256to511octets_frames_good_bad_high_tx256_511octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_low_tx512_1023octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_low_tx512_1023octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_high_tx512_1023octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_high_tx512_1023octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_low_tx1024_maxoctgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_low_tx1024_maxoctgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_high_tx1024_maxoctgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_high_tx1024_maxoctgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_unicast_frames_good_bad_low_txucastgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_unicast_frames_good_bad_low_txucastgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_unicast_frames_good_bad_high_txucastgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_unicast_frames_good_bad_high_txucastgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_multicast_frames_good_bad_low_txmcastgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_multicast_frames_good_bad_low_txmcastgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_multicast_frames_good_bad_high_txmcastgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_multicast_frames_good_bad_high_txmcastgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_broadcast_frames_good_bad_low_txbcastgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_broadcast_frames_good_bad_low_txbcastgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_broadcast_frames_good_bad_high_txbcastgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_broadcast_frames_good_bad_high_txbcastgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_underflow_error_frames_low_txundrflwlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_underflow_error_frames_low_txundrflwlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_underflow_error_frames_high_txundrflwhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_underflow_error_frames_high_txundrflwhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_octet_count_good_low_txoctglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_octet_count_good_low_txoctglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_octet_count_good_high_txoctghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_octet_count_good_high_txoctghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_frame_count_good_low_txfrmglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_frame_count_good_low_txfrmglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_frame_count_good_high_txfrmghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_frame_count_good_high_txfrmghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_pause_frames_low_txpauseglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_pause_frames_low_txpauseglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_pause_frames_high_txpauseghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_pause_frames_high_txpauseghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_vlan_frames_good_low_txvlanglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_vlan_frames_good_low_txvlanglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_vlan_frames_good_high_txvlanghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_vlan_frames_good_high_txvlanghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_lpi_usec_cntr_txlpiusc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_lpi_usec_cntr_txlpiusc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tx_lpi_tran_cntr_txlpitrc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tx_lpi_tran_cntr_txlpitrc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_frame_count_good_bad_low_rxfrmgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_frame_count_good_bad_low_rxfrmgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_frame_count_good_bad_high_rxfrmgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_frame_count_good_bad_high_rxfrmgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_octet_count_good_bad_low_rxoctgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_octet_count_good_bad_low_rxoctgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_octet_count_good_bad_high_rxoctgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_octet_count_good_bad_high_rxoctgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_octet_count_good_low_rxoctglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_octet_count_good_low_rxoctglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_octet_count_good_high_rxoctghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_octet_count_good_high_rxoctghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_broadcast_frames_good_low_rxbcastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_broadcast_frames_good_low_rxbcastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_broadcast_frames_good_high_rxbcastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_broadcast_frames_good_high_rxbcastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_multicast_frames_good_low_rxmcastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_multicast_frames_good_low_rxmcastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_multicast_frames_good_high_rxmcastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_multicast_frames_good_high_rxmcastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_crc_error_frames_low_rxcrcerlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_crc_error_frames_low_rxcrcerlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_crc_error_frames_high_rxcrcerhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_crc_error_frames_high_rxcrcerhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_runt_error_frames_rxrunter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_runt_error_frames_rxrunter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_jabber_error_frames_rxjaberer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_jabber_error_frames_rxjaberer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_undersize_frames_good_rxusizeg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_undersize_frames_good_rxusizeg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_oversize_frames_good_rxosizeg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_oversize_frames_good_rxosizeg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_64octets_frames_good_bad_low_rx64octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_64octets_frames_good_bad_low_rx64octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_64octets_frames_good_bad_high_rx64octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_64octets_frames_good_bad_high_rx64octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_65to127octets_frames_good_bad_low_rx65_127octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_65to127octets_frames_good_bad_low_rx65_127octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_65to127octets_frames_good_bad_high_rx65_127octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_65to127octets_frames_good_bad_high_rx65_127octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_128to255octets_frames_good_bad_low_rx128_255octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_128to255octets_frames_good_bad_low_rx128_255octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_128to255octets_frames_good_bad_high_rx128_255octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_128to255octets_frames_good_bad_high_rx128_255octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_256to511octets_frames_good_bad_low_rx256_511octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_256to511octets_frames_good_bad_low_rx256_511octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_256to511octets_frames_good_bad_high_rx256_511octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_256to511octets_frames_good_bad_high_rx256_511octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_low_rx512_1023octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_low_rx512_1023octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_high_rx512_1023octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_high_rx512_1023octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_low_rx1024_maxgboctlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_low_rx1024_maxgboctlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_high_rx1024_maxgbocthi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_high_rx1024_maxgbocthi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_unicast_frames_good_low_rxucastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_unicast_frames_good_low_rxucastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_unicast_frames_good_high_rxucastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_unicast_frames_good_high_rxucastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_length_error_frames_low_rxlenerrlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_length_error_frames_low_rxlenerrlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_length_error_frames_high_rxlenerrhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_length_error_frames_high_rxlenerrhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_outofrange_frames_low_rxorangelo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_outofrange_frames_low_rxorangelo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_outofrange_frames_high_rxorangehi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_outofrange_frames_high_rxorangehi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_pause_frames_low_rxpauselo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_pause_frames_low_rxpauselo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_pause_frames_high_rxpausehi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_pause_frames_high_rxpausehi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_fifooverflow_frames_low_rxfovflo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_fifooverflow_frames_low_rxfovflo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_fifooverflow_frames_high_rxfovfhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_fifooverflow_frames_high_rxfovfhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_vlan_frames_good_bad_low_rxvlangblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_vlan_frames_good_bad_low_rxvlangblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_vlan_frames_good_bad_high_rxvlangbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_vlan_frames_good_bad_high_rxvlangbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_watchdog_error_frames_rxwdogerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_watchdog_error_frames_rxwdogerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_lpi_usec_cntr_rxlpiusc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_lpi_usec_cntr_rxlpiusc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_lpi_tran_cntr_rxlpitrc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_lpi_tran_cntr_rxlpitrc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_discard_frame_count_good_bad_low_rxdfcntgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_discard_frame_count_good_bad_low_rxdfcntgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_discard_frame_count_good_bad_high_rxdfcntgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_discard_frame_count_good_bad_high_rxdfcntgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_discard_octet_count_good_bad_low_rxdocntgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_discard_octet_count_good_bad_low_rxdocntgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rx_discard_octet_count_good_bad_high_rxdocntgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_discard_octet_count_good_bad_high_rxdocntgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

