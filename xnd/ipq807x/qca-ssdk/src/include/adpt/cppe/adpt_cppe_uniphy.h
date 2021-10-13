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
#ifndef _ADPT_CPPE_UNIPHYH_
#define _ADPT_CPPE_UNIPHYH_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

sw_error_t
__adpt_cppe_uniphy_channel_selection_set(a_uint32_t dev_id,
	a_uint32_t ch0_selection, a_uint32_t ch4_selection);

void
__adpt_hppe_gcc_uniphy_xpcs_reset(a_uint32_t dev_id, a_uint32_t uniphy_index,
		a_bool_t enable);
sw_error_t
__adpt_hppe_uniphy_calibrate(a_uint32_t dev_id, a_uint32_t uniphy_index);

void
__adpt_cppe_gcc_uniphy_software_reset(a_uint32_t dev_id,
		a_uint32_t uniphy_index);
sw_error_t
__adpt_cppe_uniphy_mode_set(a_uint32_t dev_id,
		a_uint32_t uniphy_index, a_uint32_t mode);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif
