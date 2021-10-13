/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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



#ifndef _API_ACCESS_H
#define _API_ACCESS_H

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

    sw_api_func_t *
    sw_api_func_find(a_uint32_t api_id);

    sw_api_param_t *
    sw_api_param_find(a_uint32_t api_id);

    a_uint32_t
    sw_api_param_nums(a_uint32_t api_id);

    sw_error_t
    sw_api_get(sw_api_t *sw_api);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _API_ACCESS_H */
