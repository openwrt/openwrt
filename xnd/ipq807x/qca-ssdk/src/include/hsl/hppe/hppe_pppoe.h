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
#ifndef _HPPE_PPPOE_H_
#define _HPPE_PPPOE_H_

#define PPPOE_SESSION_MAX_ENTRY	16
#define PPPOE_SESSION_EXT_MAX_ENTRY	16
#define PPPOE_SESSION_EXT1_MAX_ENTRY	16

sw_error_t
hppe_pppoe_session_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pppoe_session_u *value);

sw_error_t
hppe_pppoe_session_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pppoe_session_u *value);

sw_error_t
hppe_pppoe_session_ext_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pppoe_session_ext_u *value);

sw_error_t
hppe_pppoe_session_ext_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pppoe_session_ext_u *value);

sw_error_t
hppe_pppoe_session_ext1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pppoe_session_ext1_u *value);

sw_error_t
hppe_pppoe_session_ext1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pppoe_session_ext1_u *value);

sw_error_t
hppe_pppoe_session_session_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pppoe_session_session_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pppoe_session_l3_if_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pppoe_session_l3_if_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pppoe_session_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pppoe_session_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pppoe_session_ext_uc_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pppoe_session_ext_uc_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pppoe_session_ext_mc_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pppoe_session_ext_mc_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pppoe_session_ext_smac_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pppoe_session_ext_smac_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pppoe_session_ext_l3_if_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pppoe_session_ext_l3_if_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pppoe_session_ext_smac_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pppoe_session_ext_smac_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pppoe_session_ext1_smac_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pppoe_session_ext1_smac_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);


#endif

