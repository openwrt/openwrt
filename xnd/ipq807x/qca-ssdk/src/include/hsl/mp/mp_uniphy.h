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

/**
 * @defgroup
 * @{
 */

#ifndef _MP_UNIPHY_H_
#define _MP_UNIPHY_H_

#define UNIPHY_CLK_DIV_25M                              1
#define UNIPHY_CLK_DIV_50M                              0
#define UNIPHY_CLK_DRV_1                                1

sw_error_t
mp_uniphy_clock_output_control_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uniphy_clock_output_control_u *value);
sw_error_t
mp_uniphy_clock_output_control_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uniphy_clock_output_control_u *value);
#endif

