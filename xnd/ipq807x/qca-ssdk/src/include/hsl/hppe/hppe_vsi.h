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
#ifndef _HPPE_VSI_H_
#define _HPPE_VSI_H_

#define VSI_TBL_MAX_ENTRY	32
#define VLAN_CNT_TBL_MAX_ENTRY	32
#define EG_VSI_COUNTER_TBL_MAX_ENTRY	32
#define PRE_L2_CNT_TBL_MAX_ENTRY	32

sw_error_t
hppe_vsi_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vsi_tbl_u *value);

sw_error_t
hppe_vsi_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vsi_tbl_u *value);

sw_error_t
hppe_vsi_tbl_umc_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vsi_tbl_umc_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vsi_tbl_station_move_lrn_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vsi_tbl_station_move_lrn_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vsi_tbl_new_addr_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vsi_tbl_new_addr_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vsi_tbl_uuc_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vsi_tbl_uuc_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vsi_tbl_member_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vsi_tbl_member_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vsi_tbl_new_addr_lrn_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vsi_tbl_new_addr_lrn_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vsi_tbl_bc_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vsi_tbl_bc_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vsi_tbl_station_move_fwd_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vsi_tbl_station_move_fwd_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vlan_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_cnt_tbl_u *value);

sw_error_t
hppe_vlan_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vlan_cnt_tbl_u *value);

sw_error_t
hppe_vlan_cnt_tbl_rx_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_vlan_cnt_tbl_rx_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_vlan_cnt_tbl_rx_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vlan_cnt_tbl_rx_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_vsi_counter_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vsi_counter_tbl_u *value);

sw_error_t
hppe_eg_vsi_counter_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_vsi_counter_tbl_u *value);


sw_error_t
hppe_eg_vsi_counter_tbl_tx_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_eg_vsi_counter_tbl_tx_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_eg_vsi_counter_tbl_tx_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_vsi_counter_tbl_tx_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pre_l2_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pre_l2_cnt_tbl_u *value);

sw_error_t
hppe_pre_l2_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pre_l2_cnt_tbl_u *value);

sw_error_t
hppe_pre_l2_cnt_tbl_rx_drop_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_pre_l2_cnt_tbl_rx_drop_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_pre_l2_cnt_tbl_rx_byte_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_pre_l2_cnt_tbl_rx_byte_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_pre_l2_cnt_tbl_rx_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pre_l2_cnt_tbl_rx_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pre_l2_cnt_tbl_rx_drop_pkt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pre_l2_cnt_tbl_rx_drop_pkt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

