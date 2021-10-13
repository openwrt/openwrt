/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
#ifndef _CPPE_LOOPBACK_H_
#define _CPPE_LOOPBACK_H_

sw_error_t
cppe_lpbk_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_enable_u *value);

sw_error_t
cppe_lpbk_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_enable_u *value);

sw_error_t
cppe_lpbk_fifo_1_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_fifo_1_ctrl_u *value);

sw_error_t
cppe_lpbk_fifo_1_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_fifo_1_ctrl_u *value);

sw_error_t
cppe_lpbk_fifo_2_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_fifo_2_ctrl_u *value);

sw_error_t
cppe_lpbk_fifo_2_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_fifo_2_ctrl_u *value);

sw_error_t
cppe_lpbk_pps_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_pps_ctrl_u *value);

sw_error_t
cppe_lpbk_pps_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_pps_ctrl_u *value);

sw_error_t
cppe_lpbk_mac_junmo_size_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_mac_junmo_size_u *value);

sw_error_t
cppe_lpbk_mac_junmo_size_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_mac_junmo_size_u *value);

sw_error_t
cppe_lpbk_mib_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_mib_ctrl_u *value);

sw_error_t
cppe_lpbk_mib_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbk_mib_ctrl_u *value);

sw_error_t
cppe_lpbk_mib_uni_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkuni_u *value);

sw_error_t
cppe_lpbk_mib_multi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkmulti_u *value);

sw_error_t
cppe_lpbk_mib_broad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkbroad_u *value);

sw_error_t
cppe_lpbk_mib_pkt64_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkt64_u *value);

sw_error_t
cppe_lpbk_mib_pkt65to127_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkt65to127_u *value);

sw_error_t
cppe_lpbk_mib_pkt128to255_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkt128to255_u *value);

sw_error_t
cppe_lpbk_mib_pkt256to511_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkt256to511_u *value);

sw_error_t
cppe_lpbk_mib_pkt512to1023_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkt512to1023_u *value);

sw_error_t
cppe_lpbk_mib_pkt1024to1518_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkt1024to1518_u *value);

sw_error_t
cppe_lpbk_mib_pkt1519tox_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkt1519tox_u *value);

sw_error_t
cppe_lpbk_mib_toolong_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkttoolong_u *value);

sw_error_t
cppe_lpbk_mib_byte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkbyte_l_u *value);

sw_error_t
cppe_lpbk_mib_byte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkbyte_h_u *value);

sw_error_t
cppe_lpbk_mib_drop_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkdropcounter_u *value);

sw_error_t
cppe_lpbk_mib_tooshort_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkttooshort_u *value);

sw_error_t
cppe_lpbk_mib_pkt14to63_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbkpkt14to63_u *value);

sw_error_t
cppe_lpbk_mib_toolongbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbktoolongbyte_l_u *value);

sw_error_t
cppe_lpbk_mib_toolongbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbktoolongbyte_h_u *value);


sw_error_t
cppe_lpbk_mib_tooshortbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbktooshortbyte_l_u *value);

sw_error_t
cppe_lpbk_mib_tooshortbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpbktooshortbyte_h_u *value);
#endif
