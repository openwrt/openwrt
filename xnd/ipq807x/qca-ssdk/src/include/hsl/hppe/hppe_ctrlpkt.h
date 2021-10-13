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
#ifndef _HPPE_CTRLPKT_H_
#define _HPPE_CTRLPKT_H_

#define ETHERTYPE_CTRL_MAX_ENTRY	4
#define RFDB_TBL_MAX_ENTRY	32
#define APP_CTRL_MAX_ENTRY	32


sw_error_t
hppe_ethertype_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ethertype_ctrl_u *value);

sw_error_t
hppe_ethertype_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ethertype_ctrl_u *value);

sw_error_t
hppe_app_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union app_ctrl_u *value);

sw_error_t
hppe_app_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union app_ctrl_u *value);

sw_error_t
hppe_ethertype_ctrl_ethertype_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ethertype_ctrl_ethertype_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ethertype_ctrl_ethertype_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ethertype_ctrl_ethertype_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_portbitmap_include_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_portbitmap_include_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_portbitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_portbitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_rfdb_index_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_rfdb_index_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_protocol_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_protocol_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_in_stg_byp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_in_stg_byp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_l2_sec_byp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_l2_sec_byp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_protocol_include_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_protocol_include_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_ethertype_include_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_ethertype_include_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_sg_byp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_sg_byp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_rfdb_include_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_rfdb_include_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_ethertype_index_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_ethertype_index_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_app_ctrl_in_vlan_fltr_byp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_app_ctrl_in_vlan_fltr_byp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

