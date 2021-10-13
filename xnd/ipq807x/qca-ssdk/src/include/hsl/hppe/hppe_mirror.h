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
#ifndef _HPPE_MIRROR_H_
#define _HPPE_MIRROR_H_

#define PORT_MIRROR_MAX_ENTRY	8

sw_error_t
hppe_mirror_analyzer_get(
		a_uint32_t dev_id,
		union mirror_analyzer_u *value);

sw_error_t
hppe_mirror_analyzer_set(
		a_uint32_t dev_id,
		union mirror_analyzer_u *value);

sw_error_t
hppe_port_mirror_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_mirror_u *value);

sw_error_t
hppe_port_mirror_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_mirror_u *value);

sw_error_t
hppe_mirror_analyzer_in_analyzer_port_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_mirror_analyzer_in_analyzer_port_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_mirror_analyzer_eg_analyzer_port_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_mirror_analyzer_eg_analyzer_port_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_port_mirror_in_mirr_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_mirror_in_mirr_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_mirror_eg_mirr_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_mirror_eg_mirr_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

