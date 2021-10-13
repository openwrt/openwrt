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
#ifndef _ADPT_CPPE_FLOW_
#define _ADPT_CPPE_FLOW_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#ifndef IN_FLOW_MINI
sw_error_t
adpt_cppe_flow_copy_escape_set(a_uint32_t dev_id, a_bool_t enable);

sw_error_t
adpt_cppe_flow_copy_escape_get(a_uint32_t dev_id, a_bool_t *enable);
#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif
