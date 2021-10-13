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

#ifndef MP_MIB_REG_H
#define MP_MIB_REG_H

/*[register] MMC_CONTROL*/
#define MMC_CONTROL
#define MMC_CONTROL_ADDRESS 0x100
#define MMC_CONTROL_NUM     2
#define MMC_CONTROL_INC     0x100000
#define MMC_CONTROL_DEFAULT 0x0

struct mmc_control {
	a_uint32_t  cntrst:1;
	a_uint32_t  cntstopro:1;
	a_uint32_t  rstonrd:1;
	a_uint32_t  cntfreez:1;
	a_uint32_t  cntprst:1;
	a_uint32_t  cntprstlvl:1;
	a_uint32_t  _reserved1:2;
	a_uint32_t  ucdbc:8;
	a_uint32_t  _reserved2:23;
};

union mmc_control_u {
	a_uint32_t val;
	struct mmc_control bf;
};

/*[register] TX_OCTET_COUNT_GOOD_BAD*/
#define TX_OCTET_COUNT_GOOD_BAD
#define TX_OCTET_COUNT_GOOD_BAD_ADDRESS 0x114
#define TX_OCTET_COUNT_GOOD_BAD_NUM     2
#define TX_OCTET_COUNT_GOOD_BAD_INC     0x100000
#define TX_OCTET_COUNT_GOOD_BAD_DEFAULT 0x0

struct tx_octet_count_good_bad {
	a_uint32_t  txoctgb:32;
};

union tx_octet_count_good_bad_u {
	a_uint32_t val;
	struct tx_octet_count_good_bad bf;
};

/*[register] TX_FRAME_COUNT_GOOD_BAD*/
#define TX_FRAME_COUNT_GOOD_BAD
#define TX_FRAME_COUNT_GOOD_BAD_ADDRESS 0x118
#define TX_FRAME_COUNT_GOOD_BAD_NUM     2
#define TX_FRAME_COUNT_GOOD_BAD_INC     0x100000
#define TX_FRAME_COUNT_GOOD_BAD_DEFAULT 0x0

struct tx_frame_count_good_bad {
	a_uint32_t  txfrmgb:32;
};

union tx_frame_count_good_bad_u {
	a_uint32_t val;
	struct tx_frame_count_good_bad bf;
};

/*[register] TX_BROADCAST_FRAMES_GOOD*/
#define TX_BROADCAST_FRAMES_GOOD
#define TX_BROADCAST_FRAMES_GOOD_ADDRESS 0x11c
#define TX_BROADCAST_FRAMES_GOOD_NUM     2
#define TX_BROADCAST_FRAMES_GOOD_INC     0x100000
#define TX_BROADCAST_FRAMES_GOOD_DEFAULT 0x0

struct tx_broadcast_frames_good {
	a_uint32_t  txbcastg:32;
};

union tx_broadcast_frames_good_u {
	a_uint32_t val;
	struct tx_broadcast_frames_good bf;
};

/*[register] TX_MULTICAST_FRAMES_GOOD*/
#define TX_MULTICAST_FRAMES_GOOD
#define TX_MULTICAST_FRAMES_GOOD_ADDRESS 0x120
#define TX_MULTICAST_FRAMES_GOOD_NUM     2
#define TX_MULTICAST_FRAMES_GOOD_INC     0x100000
#define TX_MULTICAST_FRAMES_GOOD_DEFAULT 0x0

struct tx_multicast_frames_good {
	a_uint32_t  txmcastg:32;
};

union tx_multicast_frames_good_u {
	a_uint32_t val;
	struct tx_multicast_frames_good bf;
};

/*[register] TX_64OCTETS_FRAMES_GOOD_BAD*/
#define TX_64OCTETS_FRAMES_GOOD_BAD
#define TX_64OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x124
#define TX_64OCTETS_FRAMES_GOOD_BAD_NUM     2
#define TX_64OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define TX_64OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct tx_64octets_frames_good_bad {
	a_uint32_t  tx64octgb:32;
};

union tx_64octets_frames_good_bad_u {
	a_uint32_t val;
	struct tx_64octets_frames_good_bad bf;
};

/*[register] TX_65TO127OCTETS_FRAMES_GOOD_BAD*/
#define TX_65TO127OCTETS_FRAMES_GOOD_BAD
#define TX_65TO127OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x128
#define TX_65TO127OCTETS_FRAMES_GOOD_BAD_NUM     2
#define TX_65TO127OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define TX_65TO127OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct tx_65to127octets_frames_good_bad {
	a_uint32_t  tx65_127octgb:32;
};

union tx_65to127octets_frames_good_bad_u {
	a_uint32_t val;
	struct tx_65to127octets_frames_good_bad bf;
};

/*[register] TX_128TO255OCTETS_FRAMES_GOOD_BAD*/
#define TX_128TO255OCTETS_FRAMES_GOOD_BAD
#define TX_128TO255OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x12c
#define TX_128TO255OCTETS_FRAMES_GOOD_BAD_NUM     2
#define TX_128TO255OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define TX_128TO255OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct tx_128to255octets_frames_good_bad {
	a_uint32_t  tx128_255octgb:32;
};

union tx_128to255octets_frames_good_bad_u {
	a_uint32_t val;
	struct tx_128to255octets_frames_good_bad bf;
};

/*[register] TX_256TO511OCTETS_FRAMES_GOOD_BAD*/
#define TX_256TO511OCTETS_FRAMES_GOOD_BAD
#define TX_256TO511OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x130
#define TX_256TO511OCTETS_FRAMES_GOOD_BAD_NUM     2
#define TX_256TO511OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define TX_256TO511OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct tx_256to511octets_frames_good_bad {
	a_uint32_t  tx256_511octgb:32;
};

union tx_256to511octets_frames_good_bad_u {
	a_uint32_t val;
	struct tx_256to511octets_frames_good_bad bf;
};

/*[register] TX_512TO1023OCTETS_FRAMES_GOOD_BAD*/
#define TX_512TO1023OCTETS_FRAMES_GOOD_BAD
#define TX_512TO1023OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x134
#define TX_512TO1023OCTETS_FRAMES_GOOD_BAD_NUM     2
#define TX_512TO1023OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define TX_512TO1023OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct tx_512to1023octets_frames_good_bad {
	a_uint32_t  tx512_1023octgb:32;
};

union tx_512to1023octets_frames_good_bad_u {
	a_uint32_t val;
	struct tx_512to1023octets_frames_good_bad bf;
};

/*[register] TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD*/
#define TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD
#define TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_ADDRESS 0x138
#define TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_NUM     2
#define TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct tx_1024tomaxoctets_frames_good_bad {
	a_uint32_t  tx1024_maxoctgb:32;
};

union tx_1024tomaxoctets_frames_good_bad_u {
	a_uint32_t val;
	struct tx_1024tomaxoctets_frames_good_bad bf;
};

/*[register] TX_UNICAST_FRAMES_GOOD_BAD*/
#define TX_UNICAST_FRAMES_GOOD_BAD
#define TX_UNICAST_FRAMES_GOOD_BAD_ADDRESS 0x13c
#define TX_UNICAST_FRAMES_GOOD_BAD_NUM     2
#define TX_UNICAST_FRAMES_GOOD_BAD_INC     0x100000
#define TX_UNICAST_FRAMES_GOOD_BAD_DEFAULT 0x0

struct tx_unicast_frames_good_bad {
	a_uint32_t  txucastgb:32;
};

union tx_unicast_frames_good_bad_u {
	a_uint32_t val;
	struct tx_unicast_frames_good_bad bf;
};

/*[register] TX_MULTICAST_FRAMES_GOOD_BAD*/
#define TX_MULTICAST_FRAMES_GOOD_BAD
#define TX_MULTICAST_FRAMES_GOOD_BAD_ADDRESS 0x140
#define TX_MULTICAST_FRAMES_GOOD_BAD_NUM     2
#define TX_MULTICAST_FRAMES_GOOD_BAD_INC     0x100000
#define TX_MULTICAST_FRAMES_GOOD_BAD_DEFAULT 0x0

struct tx_multicast_frames_good_bad {
	a_uint32_t  txmcastgb:32;
};

union tx_multicast_frames_good_bad_u {
	a_uint32_t val;
	struct tx_multicast_frames_good_bad bf;
};

/*[register] TX_BROADCAST_FRAMES_GOOD_BAD*/
#define TX_BROADCAST_FRAMES_GOOD_BAD
#define TX_BROADCAST_FRAMES_GOOD_BAD_ADDRESS 0x144
#define TX_BROADCAST_FRAMES_GOOD_BAD_NUM     2
#define TX_BROADCAST_FRAMES_GOOD_BAD_INC     0x100000
#define TX_BROADCAST_FRAMES_GOOD_BAD_DEFAULT 0x0

struct tx_broadcast_frames_good_bad {
	a_uint32_t  txbcastgb:32;
};

union tx_broadcast_frames_good_bad_u {
	a_uint32_t val;
	struct tx_broadcast_frames_good_bad bf;
};

/*[register] TX_UNDERFLOW_ERROR_FRAMES*/
#define TX_UNDERFLOW_ERROR_FRAMES
#define TX_UNDERFLOW_ERROR_FRAMES_ADDRESS 0x148
#define TX_UNDERFLOW_ERROR_FRAMES_NUM     2
#define TX_UNDERFLOW_ERROR_FRAMES_INC     0x100000
#define TX_UNDERFLOW_ERROR_FRAMES_DEFAULT 0x0

struct tx_underflow_error_frames {
	a_uint32_t  txundrflw:32;
};

union tx_underflow_error_frames_u {
	a_uint32_t val;
	struct tx_underflow_error_frames bf;
};

/*[register] TX_SINGLE_COLLISION_GOOD_FRAMES*/
#define TX_SINGLE_COLLISION_GOOD_FRAMES
#define TX_SINGLE_COLLISION_GOOD_FRAMES_ADDRESS 0x14c
#define TX_SINGLE_COLLISION_GOOD_FRAMES_NUM     2
#define TX_SINGLE_COLLISION_GOOD_FRAMES_INC     0x100000
#define TX_SINGLE_COLLISION_GOOD_FRAMES_DEFAULT 0x0

struct tx_single_collision_good_frames {
	a_uint32_t  txsnglcolg:32;
};

union tx_single_collision_good_frames_u {
	a_uint32_t val;
	struct tx_single_collision_good_frames bf;
};

/*[register] TX_MULTIPLE_COLLISION_GOOD_FRAMES*/
#define TX_MULTIPLE_COLLISION_GOOD_FRAMES
#define TX_MULTIPLE_COLLISION_GOOD_FRAMES_ADDRESS 0x150
#define TX_MULTIPLE_COLLISION_GOOD_FRAMES_NUM     2
#define TX_MULTIPLE_COLLISION_GOOD_FRAMES_INC     0x100000
#define TX_MULTIPLE_COLLISION_GOOD_FRAMES_DEFAULT 0x0

struct tx_multiple_collision_good_frames {
	a_uint32_t  txmultcolg:32;
};

union tx_multiple_collision_good_frames_u {
	a_uint32_t val;
	struct tx_multiple_collision_good_frames bf;
};

/*[register] TX_DEFERRED_FRAMES*/
#define TX_DEFERRED_FRAMES
#define TX_DEFERRED_FRAMES_ADDRESS 0x154
#define TX_DEFERRED_FRAMES_NUM     2
#define TX_DEFERRED_FRAMES_INC     0x100000
#define TX_DEFERRED_FRAMES_DEFAULT 0x0

struct tx_deferred_frames {
	a_uint32_t  txdefrd:32;
};

union tx_deferred_frames_u {
	a_uint32_t val;
	struct tx_deferred_frames bf;
};

/*[register] TX_LATE_COLLISION_FRAMES*/
#define TX_LATE_COLLISION_FRAMES
#define TX_LATE_COLLISION_FRAMES_ADDRESS 0x158
#define TX_LATE_COLLISION_FRAMES_NUM     2
#define TX_LATE_COLLISION_FRAMES_INC     0x100000
#define TX_LATE_COLLISION_FRAMES_DEFAULT 0x0

struct tx_late_collision_frames {
	a_uint32_t  txlatecol:32;
};

union tx_late_collision_frames_u {
	a_uint32_t val;
	struct tx_late_collision_frames bf;
};

/*[register] TX_EXCESSIVE_COLLISION_FRAMES*/
#define TX_EXCESSIVE_COLLISION_FRAMES
#define TX_EXCESSIVE_COLLISION_FRAMES_ADDRESS 0x15c
#define TX_EXCESSIVE_COLLISION_FRAMES_NUM     2
#define TX_EXCESSIVE_COLLISION_FRAMES_INC     0x100000
#define TX_EXCESSIVE_COLLISION_FRAMES_DEFAULT 0x0

struct tx_excessive_collision_frames {
	a_uint32_t  txexscol:32;
};

union tx_excessive_collision_frames_u {
	a_uint32_t val;
	struct tx_excessive_collision_frames bf;
};

/*[register] TX_CARRIER_ERROR_FRAMES*/
#define TX_CARRIER_ERROR_FRAMES
#define TX_CARRIER_ERROR_FRAMES_ADDRESS 0x160
#define TX_CARRIER_ERROR_FRAMES_NUM     2
#define TX_CARRIER_ERROR_FRAMES_INC     0x100000
#define TX_CARRIER_ERROR_FRAMES_DEFAULT 0x0

struct tx_carrier_error_frames {
	a_uint32_t  txexscol:32;
};

union tx_carrier_error_frames_u {
	a_uint32_t val;
	struct tx_carrier_error_frames bf;
};

/*[register] TX_OCTET_COUNT_GOOD*/
#define TX_OCTET_COUNT_GOOD
#define TX_OCTET_COUNT_GOOD_ADDRESS 0x164
#define TX_OCTET_COUNT_GOOD_NUM     2
#define TX_OCTET_COUNT_GOOD_INC     0x100000
#define TX_OCTET_COUNT_GOOD_DEFAULT 0x0

struct tx_octet_count_good {
	a_uint32_t  txoctg:32;
};

union tx_octet_count_good_u {
	a_uint32_t val;
	struct tx_octet_count_good bf;
};

/*[register] TX_FRAME_COUNT_GOOD*/
#define TX_FRAME_COUNT_GOOD
#define TX_FRAME_COUNT_GOOD_ADDRESS 0x168
#define TX_FRAME_COUNT_GOOD_NUM     2
#define TX_FRAME_COUNT_GOOD_INC     0x100000
#define TX_FRAME_COUNT_GOOD_DEFAULT 0x0

struct tx_frame_count_good {
	a_uint32_t  txfrmg:32;
};

union tx_frame_count_good_u {
	a_uint32_t val;
	struct tx_frame_count_good bf;
};

/*[register] TX_EXCESSIVE_DEFERRAL_ERROR*/
#define TX_EXCESSIVE_DEFERRAL_ERROR
#define TX_EXCESSIVE_DEFERRAL_ERROR_ADDRESS 0x16c
#define TX_EXCESSIVE_DEFERRAL_ERROR_NUM     2
#define TX_EXCESSIVE_DEFERRAL_ERROR_INC     0x100000
#define TX_EXCESSIVE_DEFERRAL_ERRORD_DEFAULT 0x0

struct tx_excessive_deferral_error {
	a_uint32_t  txexsdef:32;
};

union tx_excessive_deferral_error_u {
	a_uint32_t val;
	struct tx_excessive_deferral_error bf;
};

/*[register] TX_PAUSE_FRAMES*/
#define TX_PAUSE_FRAMES
#define TX_PAUSE_FRAMES_ADDRESS 0x170
#define TX_PAUSE_FRAMES_NUM     2
#define TX_PAUSE_FRAMES_INC     0x100000
#define TX_PAUSE_FRAMES_DEFAULT 0x0

struct tx_pause_frames {
	a_uint32_t  txpauseg:32;
};

union tx_pause_frames_u {
	a_uint32_t val;
	struct tx_pause_frames bf;
};

/*[register] TX_VLAN_FRAMES_GOOD*/
#define TX_VLAN_FRAMES_GOOD
#define TX_VLAN_FRAMES_GOOD_ADDRESS 0x174
#define TX_VLAN_FRAMES_GOOD_NUM     2
#define TX_VLAN_FRAMES_GOOD_INC     0x100000
#define TX_VLAN_FRAMES_GOOD_DEFAULT 0x0

struct tx_vlan_frames_good {
	a_uint32_t  txvlang:32;
};

union tx_vlan_frames_good_u {
	a_uint32_t val;
	struct tx_vlan_frames_good bf;
};

/*[register] TX_OSIZE_FRAMES_GOOD*/
#define TX_OSIZE_FRAMES_GOOD
#define TX_OSIZE_FRAMES_GOOD_ADDRESS 0x178
#define TX_OSIZE_FRAMES_GOOD_NUM     2
#define TX_OSIZE_FRAMES_GOOD_INC     0x100000
#define TX_OSIZE_FRAMES_GOOD_DEFAULT 0x0

struct tx_osize_frames_good {
	a_uint32_t  txosize:32;
};

union tx_osize_frames_good_u {
	a_uint32_t val;
	struct tx_osize_frames_good bf;
};

/*[register] RX_FRAME_COUNT_GOOD_BAD*/
#define RX_FRAME_COUNT_GOOD_BAD
#define RX_FRAME_COUNT_GOOD_BAD_ADDRESS 0x180
#define RX_FRAME_COUNT_GOOD_BAD_NUM     2
#define RX_FRAME_COUNT_GOOD_BAD_INC     0x100000
#define RX_FRAME_COUNT_GOOD_BAD_DEFAULT 0x0

struct rx_frame_count_good_bad {
	a_uint32_t  rxfrmgb:32;
};

union rx_frame_count_good_bad_u {
	a_uint32_t val;
	struct rx_frame_count_good_bad bf;
};

/*[register] RX_OCTET_COUNT_GOOD_BAD*/
#define RX_OCTET_COUNT_GOOD_BAD
#define RX_OCTET_COUNT_GOOD_BAD_ADDRESS 0x184
#define RX_OCTET_COUNT_GOOD_BAD_NUM     2
#define RX_OCTET_COUNT_GOOD_BAD_INC     0x100000
#define RX_OCTET_COUNT_GOOD_BAD_DEFAULT 0x0

struct rx_octet_count_good_bad {
	a_uint32_t  rxoctgb:32;
};

union rx_octet_count_good_bad_u {
	a_uint32_t val;
	struct rx_octet_count_good_bad bf;
};

/*[register] RX_OCTET_COUNT_GOOD*/
#define RX_OCTET_COUNT_GOOD
#define RX_OCTET_COUNT_GOOD_ADDRESS 0x188
#define RX_OCTET_COUNT_GOOD_NUM     2
#define RX_OCTET_COUNT_GOOD_INC     0x100000
#define RX_OCTET_COUNT_GOOD_DEFAULT 0x0

struct rx_octet_count_good {
	a_uint32_t  rxoctg:32;
};

union rx_octet_count_good_u {
	a_uint32_t val;
	struct rx_octet_count_good bf;
};

/*[register] RX_BROADCAST_FRAMES_GOOD*/
#define RX_BROADCAST_FRAMES_GOOD
#define RX_BROADCAST_FRAMES_GOOD_ADDRESS 0x18c
#define RX_BROADCAST_FRAMES_GOOD_NUM     2
#define RX_BROADCAST_FRAMES_GOOD_INC     0x100000
#define RX_BROADCAST_FRAMES_GOOD_DEFAULT 0x0

struct rx_broadcast_frames_good {
	a_uint32_t  rxbcastg:32;
};

union rx_broadcast_frames_good_u {
	a_uint32_t val;
	struct rx_broadcast_frames_good bf;
};

/*[register] RX_MULTICAST_FRAMES_GOOD*/
#define RX_MULTICAST_FRAMES_GOOD
#define RX_MULTICAST_FRAMES_GOOD_ADDRESS 0x190
#define RX_MULTICAST_FRAMES_GOOD_NUM     2
#define RX_MULTICAST_FRAMES_GOOD_INC     0x100000
#define RX_MULTICAST_FRAMES_GOOD_DEFAULT 0x0

struct rx_multicast_frames_good {
	a_uint32_t  rxmcastg:32;
};

union rx_multicast_frames_good_u {
	a_uint32_t val;
	struct rx_multicast_frames_good bf;
};

/*[register] RX_CRC_ERROR_FRAMES*/
#define RX_CRC_ERROR_FRAMES
#define RX_CRC_ERROR_FRAMES_ADDRESS 0x194
#define RX_CRC_ERROR_FRAMES_NUM     2
#define RX_CRC_ERROR_FRAMES_INC     0x100000
#define RX_CRC_ERROR_FRAMES_DEFAULT 0x0

struct rx_crc_error_frames {
	a_uint32_t  rxcrcer:32;
};

union rx_crc_error_frames_u {
	a_uint32_t val;
	struct rx_crc_error_frames bf;
};

/*[register] RX_ALIGNMENT_ERROR_FRAMES*/
#define RX_ALIGNMENT_ERROR_FRAMES
#define RX_ALIGNMENT_ERROR_FRAMES_ADDRESS 0x198
#define RX_ALIGNMENT_ERROR_FRAMES_NUM     2
#define RX_ALIGNMENT_ERROR_FRAMES_INC     0x100000
#define RX_ALIGNMENT_ERROR_FRAMES_DEFAULT 0x0

struct rx_alignment_error_frames {
	a_uint32_t  rxalgnerr:32;
};

union rx_alignment_error_frames_u {
	a_uint32_t val;
	struct rx_alignment_error_frames bf;
};

/*[register] RX_RUNT_ERROR_FRAMES*/
#define RX_RUNT_ERROR_FRAMES
#define RX_RUNT_ERROR_FRAMES_ADDRESS 0x19c
#define RX_RUNT_ERROR_FRAMES_NUM     2
#define RX_RUNT_ERROR_FRAMES_INC     0x100000
#define RX_RUNT_ERROR_FRAMES_DEFAULT 0x0

struct rx_runt_error_frames {
	a_uint32_t  rxrunter:32;
};

union rx_runt_error_frames_u {
	a_uint32_t val;
	struct rx_runt_error_frames bf;
};

/*[register] RX_JABBER_ERROR_FRAMES*/
#define RX_JABBER_ERROR_FRAMES
#define RX_JABBER_ERROR_FRAMES_ADDRESS 0x1a0
#define RX_JABBER_ERROR_FRAMES_NUM     2
#define RX_JABBER_ERROR_FRAMES_INC     0x100000
#define RX_JABBER_ERROR_FRAMES_DEFAULT 0x0

struct rx_jabber_error_frames {
	a_uint32_t  rxjaberer:32;
};

union rx_jabber_error_frames_u {
	a_uint32_t val;
	struct rx_jabber_error_frames bf;
};

/*[register] RX_UNDERSIZE_FRAMES_GOOD*/
#define RX_UNDERSIZE_FRAMES_GOOD
#define RX_UNDERSIZE_FRAMES_GOOD_ADDRESS 0x1a4
#define RX_UNDERSIZE_FRAMES_GOOD_NUM     2
#define RX_UNDERSIZE_FRAMES_GOOD_INC     0x100000
#define RX_UNDERSIZE_FRAMES_GOOD_DEFAULT 0x0

struct rx_undersize_frames_good {
	a_uint32_t  rxusizeg:32;
};

union rx_undersize_frames_good_u {
	a_uint32_t val;
	struct rx_undersize_frames_good bf;
};

/*[register] RX_OVERSIZE_FRAMES_GOOD*/
#define RX_OVERSIZE_FRAMES_GOOD
#define RX_OVERSIZE_FRAMES_GOOD_ADDRESS 0x1a8
#define RX_OVERSIZE_FRAMES_GOOD_NUM     2
#define RX_OVERSIZE_FRAMES_GOOD_INC     0x100000
#define RX_OVERSIZE_FRAMES_GOOD_DEFAULT 0x0

struct rx_oversize_frames_good {
	a_uint32_t  rxosizeg:32;
};

union rx_oversize_frames_good_u {
	a_uint32_t val;
	struct rx_oversize_frames_good bf;
};

/*[register] RX_64OCTETS_FRAMES_GOOD_BAD*/
#define RX_64OCTETS_FRAMES_GOOD_BAD
#define RX_64OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x1ac
#define RX_64OCTETS_FRAMES_GOOD_BAD_NUM     2
#define RX_64OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define RX_64OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct rx_64octets_frames_good_bad {
	a_uint32_t  rx64octgb:32;
};

union rx_64octets_frames_good_bad_u {
	a_uint32_t val;
	struct rx_64octets_frames_good_bad bf;
};

/*[register] RX_65TO127OCTETS_FRAMES_GOOD_BAD*/
#define RX_65TO127OCTETS_FRAMES_GOOD_BAD
#define RX_65TO127OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x1b0
#define RX_65TO127OCTETS_FRAMES_GOOD_BAD_NUM     2
#define RX_65TO127OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define RX_65TO127OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct rx_65to127octets_frames_good_bad {
	a_uint32_t  rx65_127octgb:32;
};

union rx_65to127octets_frames_good_bad_u {
	a_uint32_t val;
	struct rx_65to127octets_frames_good_bad bf;
};

/*[register] RX_128TO255OCTETS_FRAMES_GOOD_BAD*/
#define RX_128TO255OCTETS_FRAMES_GOOD_BAD
#define RX_128TO255OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x1b4
#define RX_128TO255OCTETS_FRAMES_GOOD_BAD_NUM     2
#define RX_128TO255OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define RX_128TO255OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct rx_128to255octets_frames_good_bad {
	a_uint32_t  rx128_255octgb:32;
};

union rx_128to255octets_frames_good_bad_u {
	a_uint32_t val;
	struct rx_128to255octets_frames_good_bad bf;
};

/*[register] RX_256TO511OCTETS_FRAMES_GOOD_BAD*/
#define RX_256TO511OCTETS_FRAMES_GOOD_BAD
#define RX_256TO511OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x1b8
#define RX_256TO511OCTETS_FRAMES_GOOD_BAD_NUM     2
#define RX_256TO511OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define RX_256TO511OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct rx_256to511octets_frames_good_bad {
	a_uint32_t  rx256_511octgb:32;
};

union rx_256to511octets_frames_good_bad_u {
	a_uint32_t val;
	struct rx_256to511octets_frames_good_bad bf;
};

/*[register] RX_512TO1023OCTETS_FRAMES_GOOD_BAD*/
#define RX_512TO1023OCTETS_FRAMES_GOOD_BAD
#define RX_512TO1023OCTETS_FRAMES_GOOD_BAD_ADDRESS 0x1bc
#define RX_512TO1023OCTETS_FRAMES_GOOD_BAD_NUM     2
#define RX_512TO1023OCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define RX_512TO1023OCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct rx_512to1023octets_frames_good_bad {
	a_uint32_t  rx512_1023octgb:32;
};

union rx_512to1023octets_frames_good_bad_u {
	a_uint32_t val;
	struct rx_512to1023octets_frames_good_bad bf;
};

/*[register] RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD*/
#define RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD
#define RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_ADDRESS 0x1c0
#define RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_NUM     2
#define RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_INC     0x100000
#define RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_DEFAULT 0x0

struct rx_1024tomaxoctets_frames_good_bad {
	a_uint32_t  rx1024_maxgboct:32;
};

union rx_1024tomaxoctets_frames_good_bad_u {
	a_uint32_t val;
	struct rx_1024tomaxoctets_frames_good_bad bf;
};

/*[register] RX_UNICAST_FRAMES_GOOD*/
#define RX_UNICAST_FRAMES_GOOD
#define RX_UNICAST_FRAMES_GOOD_ADDRESS 0x1c4
#define RX_UNICAST_FRAMES_GOOD_NUM     2
#define RX_UNICAST_FRAMES_GOOD_INC     0x100000
#define RX_UNICAST_FRAMES_GOOD_DEFAULT 0x0

struct rx_unicast_frames_good {
	a_uint32_t  rxucastg:32;
};

union rx_unicast_frames_good_u {
	a_uint32_t val;
	struct rx_unicast_frames_good bf;
};

/*[register] RX_LENGTH_ERROR_FRAMES*/
#define RX_LENGTH_ERROR_FRAMES
#define RX_LENGTH_ERROR_FRAMES_ADDRESS 0x1c8
#define RX_LENGTH_ERROR_FRAMES_NUM     2
#define RX_LENGTH_ERROR_FRAMES_INC     0x100000
#define RX_LENGTH_ERROR_FRAMES_DEFAULT 0x0

struct rx_length_error_frames {
	a_uint32_t  rxlenerr:32;
};

union rx_length_error_frames_u {
	a_uint32_t val;
	struct rx_length_error_frames bf;
};

/*[register] RX_OUTOFRANGE_FRAMES*/
#define RX_OUTOFRANGE_FRAMES
#define RX_OUTOFRANGE_FRAMES_ADDRESS 0x1cc
#define RX_OUTOFRANGE_FRAMES_NUM     2
#define RX_OUTOFRANGE_FRAMES_INC     0x100000
#define RX_OUTOFRANGE_FRAMES_DEFAULT 0x0

struct rx_outofrange_frames {
	a_uint32_t  rxorange:32;
};

union rx_outofrange_frames_u {
	a_uint32_t val;
	struct rx_outofrange_frames bf;
};

/*[register] RX_PAUSE_FRAMES*/
#define RX_PAUSE_FRAMES
#define RX_PAUSE_FRAMES_ADDRESS 0x1d0
#define RX_PAUSE_FRAMES_NUM     2
#define RX_PAUSE_FRAMES_INC     0x100000
#define RX_PAUSE_FRAMES_DEFAULT 0x0

struct rx_pause_frames {
	a_uint32_t  rxpause:32;
};

union rx_pause_frames_u {
	a_uint32_t val;
	struct rx_pause_frames bf;
};

/*[register] RX_FIFOOVERFW_FRAMES*/
#define RX_FIFOOVERFW_FRAMES
#define RX_FIFOOVERFW_FRAMES_ADDRESS 0x1d4
#define RX_FIFOOVERFW_FRAMES_NUM     2
#define RX_FIFOOVERFW_FRAMES_INC     0x100000
#define RX_FIFOOVERFW_FRAMES_DEFAULT 0x0

struct rx_fifo_over_flow_frames {
	a_uint32_t  rxfovf:32;
};

union rx_fifo_over_flow_frames_u {
	a_uint32_t val;
	struct rx_fifo_over_flow_frames bf;
};

/*[register] RX_VLAN_FRAMES_GOOD_BAD*/
#define RX_VLAN_FRAMES_GOOD_BAD
#define RX_VLAN_FRAMES_GOOD_BAD_ADDRESS 0x1d8
#define RX_VLAN_FRAMES_GOOD_BAD_NUM     2
#define RX_VLAN_FRAMES_GOOD_BAD_INC     0x100000
#define RX_VLAN_FRAMES_GOOD_BAD_DEFAULT 0x0

struct rx_vlan_frames_good_bad {
	a_uint32_t  rxvlangb:32;
};

union rx_vlan_frames_good_bad_u {
	a_uint32_t val;
	struct rx_vlan_frames_good_bad bf;
};

/*[register] RX_WATCHDOG_ERROR_FRAMES*/
#define RX_WATCHDOG_ERROR_FRAMES
#define RX_WATCHDOG_ERROR_FRAMES_ADDRESS 0x1dc
#define RX_WATCHDOG_ERROR_FRAMES_NUM     2
#define RX_WATCHDOG_ERROR_FRAMES_INC     0x100000
#define RX_WATCHDOG_ERROR_FRAMES_DEFAULT 0x0

struct rx_watchdog_error_frames {
	a_uint32_t  rxwdogerr:32;
};

union rx_watchdog_error_frames_u {
	a_uint32_t val;
	struct rx_watchdog_error_frames bf;
};

/*[register] GMAC0_RX_RECEIVE_ERROR_FRAMES*/
#define RX_RECEIVE_ERROR_FRAMES
#define RX_RECEIVE_ERROR_FRAMES_ADDRESS 0x1e0
#define RX_RECEIVE_ERROR_FRAMES_NUM     2
#define RX_RECEIVE_ERROR_FRAMES_INC     0x100000
#define RX_RECEIVE_ERROR_FRAMES_DEFAULT 0x0

struct rx_receive_error_frames {
	a_uint32_t  rxrcverr:32;
};

union rx_receive_error_frames_u {
	a_uint32_t val;
	struct rx_receive_error_frames bf;
};

/*[register] RX_CONTROL_FRAMES_GOOD*/
#define RX_CONTROL_FRAMES_GOOD
#define RX_CONTROL_FRAMES_GOOD_ADDRESS 0x1e4
#define RX_CONTROL_FRAMES_GOOD_NUM     2
#define RX_CONTROL_FRAMES_GOOD_INC     0x100000
#define RX_CONTROL_FRAMES_GOOD_DEFAULT 0x0

struct rx_control_frames_good {
	a_uint32_t  rxctlg:32;
};

union rx_control_frames_good_u {
	a_uint32_t val;
	struct rx_control_frames_good bf;
};
#endif
