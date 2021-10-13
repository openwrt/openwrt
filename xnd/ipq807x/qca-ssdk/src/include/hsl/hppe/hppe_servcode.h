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
#ifndef _HPPE_SERVCODE_H_
#define _HPPE_SERVCODE_H_

#define IN_L2_SERVICE_TBL_MAX_ENTRY	256
#define EG_SERVICE_TBL_MAX_ENTRY	256

sw_error_t
hppe_service_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union service_tbl_u *value);

sw_error_t
hppe_service_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union service_tbl_u *value);

sw_error_t
hppe_service_tbl_rx_counting_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_service_tbl_rx_counting_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_service_tbl_bypass_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_service_tbl_bypass_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_in_l2_service_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_l2_service_tbl_u *value);

sw_error_t
hppe_in_l2_service_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union in_l2_service_tbl_u *value);

sw_error_t
hppe_in_l2_service_tbl_direction_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_in_l2_service_tbl_direction_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_in_l2_service_tbl_rx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_in_l2_service_tbl_rx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_in_l2_service_tbl_tx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_in_l2_service_tbl_tx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_in_l2_service_tbl_bypass_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_in_l2_service_tbl_bypass_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_in_l2_service_tbl_dst_port_id_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_in_l2_service_tbl_dst_port_id_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_in_l2_service_tbl_dst_port_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_in_l2_service_tbl_dst_port_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

		sw_error_t
hppe_eg_service_tbl_next_service_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_service_tbl_next_service_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_service_tbl_tx_counting_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_service_tbl_tx_counting_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_service_tbl_field_update_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_service_tbl_field_update_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_service_tbl_offset_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_service_tbl_offset_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_service_tbl_hw_services_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_eg_service_tbl_hw_services_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_eg_service_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_service_tbl_u *value);

sw_error_t
hppe_eg_service_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union eg_service_tbl_u *value);


#endif

