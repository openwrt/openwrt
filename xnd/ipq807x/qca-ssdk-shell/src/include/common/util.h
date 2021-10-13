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



#ifndef _UTIL_H_
#define _UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#define LL_IN_ORDER  0x1
#define LL_FIX_NDNR  0x2

    typedef enum {
        LL_CMP_EQUAL = 0,
        LL_CMP_GREATER = 1,
        LL_CMP_SMALLER = 2
    }
                     ll_cmp_rslt_t;

    typedef ll_cmp_rslt_t(*ll_nd_cmp) (void *src, void *dest);

    typedef void (*ll_nd_dump) (void *data);

    typedef struct _sll_node_t
    {
        struct _sll_node_t *next;
        void *data;
    } sll_node_t;

    typedef struct
    {
        sll_node_t *fst_nd;
        a_uint32_t nd_nr;
        a_uint32_t flag;
        ll_nd_cmp nd_cmp;
        ll_nd_dump nd_dump;
        sll_node_t *free_nd;
    } sll_head_t;

    sll_head_t *sll_creat(ll_nd_cmp cmp_func, ll_nd_dump dump_func,
                          a_uint32_t flag, a_uint32_t nd_nr);

    void sll_destroy(sll_head_t * sll);

    void sll_lock(sll_head_t * sll);

    void sll_unlock(sll_head_t * sll);

    void *sll_nd_find(const sll_head_t * sll, void *data,
                      a_uint32_t * iterator);

    void *sll_nd_next(const sll_head_t * sll, a_uint32_t * iterator);

    sw_error_t sll_nd_insert(sll_head_t * sll, void *data);

    sw_error_t sll_nd_delete(sll_head_t * sll, void *data);

    typedef struct
    {
        a_uint32_t id_ptr;
        a_uint32_t id_nr;
        a_uint32_t id_min;
        a_uint32_t id_size;
        void *id_pool;
    } sid_pool_t;

    sid_pool_t *sid_pool_creat(a_uint32_t id_nr, a_uint32_t min_id);

    void sid_pool_destroy(sid_pool_t * pool);

    sw_error_t sid_pool_id_alloc(sid_pool_t * pool, a_uint32_t * id);

    sw_error_t sid_pool_id_free(sid_pool_t * pool, a_uint32_t id);

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _UTIL_H_ */
