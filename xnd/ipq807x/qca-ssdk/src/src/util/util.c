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



#include "sw.h"
#include "util.h"


static sll_node_t *
sll_nd_new(sll_head_t * sll)
{
    sll_node_t *new_nd;

    if (LL_FIX_NDNR & sll->flag)
    {
        if (NULL == sll->free_nd)
        {
            new_nd = NULL;
        }
        else
        {
            new_nd = sll->free_nd;
            sll->free_nd = sll->free_nd->next;
        }
    }
    else
    {
        new_nd = aos_mem_alloc(sizeof (sll_node_t));
    }

    if (NULL != new_nd)
    {
        aos_mem_zero(new_nd, sizeof (sll_node_t));
    }

    return new_nd;
}

static void
sll_nd_free(sll_head_t * sll, sll_node_t * nd)
{
    if (LL_FIX_NDNR & sll->flag)
    {
        nd->next = sll->free_nd;
        sll->free_nd = nd;
    }
    else
    {
        aos_mem_free(nd);
    }
    return;
}

//#define SLL_DENUG
#ifdef SLL_DENUG
static void
sll_nd_dump(sll_head_t * sll, const char * info)
{
    sll_node_t * curr;

    if (NULL == sll->nd_dump)
    {
        return;
    }

    aos_printk("\n%s    node number = %d\n", info, sll->nd_nr);
    curr = sll->fst_nd;
    while (NULL != curr)
    {
        sll->nd_dump(curr->data);
        curr = curr->next;
    }
}
#else
#define sll_nd_dump(sll, info)
#endif

sll_head_t *
sll_creat(ll_nd_cmp cmp_func, ll_nd_dump dump_func, a_uint32_t flag, a_uint32_t nd_nr)
{
    sll_head_t *head;
    sll_node_t *node;
    a_uint32_t i, size;

    if (flag & LL_FIX_NDNR)
    {
        size = sizeof (sll_head_t) + sizeof (sll_node_t) * nd_nr;
    }
    else
    {
        size = sizeof (sll_head_t);
    }

    head = aos_mem_alloc(size);
    if (NULL == head)
    {
        return NULL;
    }
    aos_mem_zero(head, size);

    head->fst_nd  = NULL;
    head->nd_nr   = 0;
    head->flag    = flag;
    head->nd_cmp  = cmp_func;
    head->nd_dump = dump_func;
    head->free_nd = NULL;

    if (flag & LL_FIX_NDNR)
    {
        node = (sll_node_t *) ((a_uint8_t *) head + sizeof (sll_head_t));
        for (i = 0; i < nd_nr; i++)
        {
            node[i].next = head->free_nd;
            head->free_nd = &(node[i]);
        }
    }

    return head;
}

void
sll_destroy(sll_head_t * sll)
{
    aos_mem_free(sll);
    return;
}

void
sll_lock(sll_head_t * sll)
{
    return;
}

void
sll_unlock(sll_head_t * sll)
{
    return;
}

void *
sll_nd_find(const sll_head_t *sll, void *data, a_ulong_t *iterator)
{
    sll_node_t *node;
    ll_cmp_rslt_t rslt;

    node = sll->fst_nd;
    while (NULL != node)
    {
        rslt = sll->nd_cmp(node->data, data);
        if (LL_CMP_EQUAL == rslt)
        {
            *iterator = (a_ulong_t)node;
            return node->data;
        }

        if ((LL_CMP_GREATER == rslt) && (sll->flag & LL_IN_ORDER))
        {
            return NULL;
        }
        node = node->next;
    }

    return NULL;
}

void *
sll_nd_next(const sll_head_t *sll, a_ulong_t *iterator)
{
    sll_node_t *curr = NULL;

    if (0 == *iterator)
    {
        if (NULL != sll->fst_nd)
        {
            curr = sll->fst_nd;
        }
    }
    else
    {
        curr = ((sll_node_t *)(*iterator))->next;
    }

    if (NULL == curr)
    {
        return NULL;
    }
    else
    {
        *iterator = (a_ulong_t)curr;
        return curr->data;
    }
}

sw_error_t
sll_nd_insert(sll_head_t * sll, void *data)
{
    sll_node_t *node = NULL;
    sll_node_t *curr = NULL;
    sll_node_t *prev;
    ll_cmp_rslt_t rslt;

    sll_nd_dump(sll, "sll_nd_insert before insert");

    node = sll_nd_new(sll);
    if (NULL == node)
    {
        return SW_NO_RESOURCE;
    }
    node->data = data;

    if ((NULL == sll->fst_nd)
            || (0 == (sll->flag & LL_IN_ORDER)))
    {
        node->next = sll->fst_nd;
        sll->fst_nd = node;
        sll->nd_nr++;
        sll_nd_dump(sll, "sll_nd_insert after insert");
        return SW_OK;
    }

    curr = sll->fst_nd;
    prev = NULL;
    while (NULL != curr)
    {
        rslt = sll->nd_cmp(curr->data, data);

        if (LL_CMP_EQUAL == rslt)
        {
            sll_nd_free(sll, node);
            return SW_ALREADY_EXIST;
        }

        if (LL_CMP_GREATER == rslt)
        {
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if (NULL == prev)
    {
        node->next = sll->fst_nd;
        sll->fst_nd = node;
    }
    else
    {
        prev->next = node;
        node->next = curr;
    }
    sll->nd_nr++;

    sll_nd_dump(sll, "sll_nd_insert after insert");
    return SW_OK;
}

sw_error_t
sll_nd_delete(sll_head_t * sll, void *data)
{
    sll_node_t *curr;
    sll_node_t *prev = NULL;
    ll_cmp_rslt_t rslt;

    sll_nd_dump(sll, "sll_nd_delete before delete");

    curr = sll->fst_nd;
    while (NULL != curr)
    {
        rslt = sll->nd_cmp(curr->data, data);
        if (LL_CMP_EQUAL == rslt)
        {
            if (NULL != prev)
            {
                prev->next = curr->next;
            }
            else
            {
                sll->fst_nd = curr->next;
            }
            sll->nd_nr--;
            sll_nd_free(sll, curr);

            sll_nd_dump(sll, "sll_nd_delete after delete");
            return SW_OK;
        }

        if ((LL_CMP_GREATER == rslt) && (sll->flag & LL_IN_ORDER))
        {
            return SW_NOT_FOUND;
        }

        prev = curr;
        curr = curr->next;
    }

    return SW_NOT_FOUND;
}


#define ID_SIZE_U8     0x1
#define ID_SIZE_U16    0x2
#define ID_SIZE_U32    0x4

#define ID_NR_IN_U8    256
#define ID_NR_IN_U16   65536

sid_pool_t *
sid_pool_creat(a_uint32_t id_nr, a_uint32_t min_id)
{
    sid_pool_t * pool;
    a_uint32_t   size;
    a_uint32_t   id_size;
    a_uint32_t   i;

    if (0 == id_nr)
    {
        return NULL;
    }

    pool = aos_mem_alloc(sizeof(sid_pool_t));
    if (NULL == pool)
    {
        return NULL;
    }
    aos_mem_zero(pool, sizeof(sid_pool_t));

    pool->id_min     = min_id;
    pool->id_nr      = id_nr;
    pool->id_ptr     = 0;

    if (ID_NR_IN_U8 >= id_nr)
    {
        size = ((id_nr + 3) >> 2) << 2;
        id_size = ID_SIZE_U8;
    }
    else if (ID_NR_IN_U16 >= id_nr)
    {
        size = ((id_nr + 1) >> 1) << 2;
        id_size = ID_SIZE_U16;
    }
    else
    {
        size = id_nr << 2;
        id_size = ID_SIZE_U32;
    }
    pool->id_size = id_size;

    pool->id_pool = aos_mem_alloc(size);
    if (NULL == pool->id_pool)
    {
        aos_mem_free(pool);
        return NULL;
    }
    aos_mem_zero(pool->id_pool, size);

    if (ID_SIZE_U8 == id_size)
    {
        a_uint8_t *p_id;

        p_id = pool->id_pool;
        for (i = 0; i < id_nr; i++)
        {
            p_id[i] = i;
        }
    }
    else if (ID_SIZE_U16 == id_size)
    {
        a_uint16_t *p_id;

        p_id = pool->id_pool;
        for (i = 0; i < id_nr; i++)
        {
            p_id[i] = i;
        }
    }
    else
    {
        a_uint32_t *p_id;

        p_id = pool->id_pool;
        for (i = 0; i < id_nr; i++)
        {
            p_id[i] = i;
        }
    }

    return pool;
}

void
sid_pool_destroy(sid_pool_t * pool)
{
    aos_mem_free(pool->id_pool);
    aos_mem_free(pool);
    return;
}

sw_error_t
sid_pool_id_alloc(sid_pool_t * pool, a_uint32_t * id)
{
    if (pool->id_nr <= pool->id_ptr)
    {
        return SW_NO_RESOURCE;
    }

    if (ID_SIZE_U8 == pool->id_size)
    {
        a_uint8_t *p_id;

        p_id = pool->id_pool;
        *id = p_id[pool->id_ptr] + pool->id_min;
    }
    else if (ID_SIZE_U16 == pool->id_size)
    {
        a_uint16_t *p_id;

        p_id = pool->id_pool;
        *id = p_id[pool->id_ptr] + pool->id_min;
    }
    else
    {
        a_uint32_t *p_id;

        p_id = pool->id_pool;
        *id = p_id[pool->id_ptr] + pool->id_min;
    }

    pool->id_ptr++;
    return SW_OK;
}

sw_error_t
sid_pool_id_free(sid_pool_t * pool, a_uint32_t id)
{
    if (0 == pool->id_ptr)
    {
        return SW_FAIL;
    }

    pool->id_ptr--;
    if (ID_SIZE_U8 == pool->id_size)
    {
        a_uint8_t *p_id;

        if (ID_NR_IN_U8 < (id - id - pool->id_min))
        {
            return SW_FAIL;
        }

        p_id = pool->id_pool;
        p_id[pool->id_ptr] = id - pool->id_min;
    }
    else if (ID_SIZE_U16 == pool->id_size)
    {
        a_uint16_t *p_id;

        if (ID_NR_IN_U16 < (id - id - pool->id_min))
        {
            return SW_FAIL;
        }

        p_id = pool->id_pool;
        p_id[pool->id_ptr] = id - pool->id_min;
    }
    else
    {
        a_uint32_t *p_id;

        p_id = pool->id_pool;
        p_id[pool->id_ptr] = id - pool->id_min;
    }

    return SW_OK;
}
