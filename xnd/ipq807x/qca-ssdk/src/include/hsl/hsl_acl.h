/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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



#ifndef _HSL_ACL_H_
#define _HSL_ACL_H_

#ifdef __cplusplus
extern "c" {
#endif

    sw_error_t
    hsl_acl_pool_creat(a_uint32_t dev_id, a_uint32_t blk_nr, a_uint32_t rsc_nr);

    sw_error_t
    hsl_acl_pool_destroy(a_uint32_t dev_id);

    sw_error_t
    hsl_acl_blk_alloc(a_uint32_t dev_id, a_uint32_t pri, a_uint32_t size,
                      a_uint32_t info, a_uint32_t * addr);

    sw_error_t
    hsl_acl_blk_free(a_uint32_t dev_id, a_uint32_t addr);

    sw_error_t
    hsl_acl_blk_resize(a_uint32_t dev_id, a_uint32_t addr, a_uint32_t new_size);

    sw_error_t
    hsl_acl_free_rsc_get(a_uint32_t dev_id, a_uint32_t * free_rsc);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /*_HSL_ACL_H_ */
