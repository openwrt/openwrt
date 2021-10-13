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
#ifndef _ADPT_MP_UNIPHY_H_
#define _ADPT_MP_UNIPHY_H_

#ifdef __cplusplus
extern "C" {
#endif                         /* __cplusplus */

void
adpt_mp_gcc_uniphy_port_reset(a_uint32_t dev_id, a_uint32_t port_id);

sw_error_t
adpt_mp_gcc_uniphy_port_clock_set(a_uint32_t dev_id, a_uint32_t port_id,
	a_bool_t enable);

sw_error_t
adpt_mp_uniphy_adapter_port_reset(a_uint32_t dev_id, a_uint32_t port_id);

sw_error_t
adpt_mp_uniphy_mode_configure(a_uint32_t dev_id, a_uint32_t index, a_uint32_t mode);

#ifdef __cplusplus
}
#endif                         /* __cplusplus */
#endif
