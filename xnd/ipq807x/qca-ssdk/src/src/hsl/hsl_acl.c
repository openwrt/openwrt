/*
 * Copyright (c) 2012, 2016, The Linux Foundation. All rights reserved.
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


#include "sw_config.h"
#include "aos_head.h"
#include "sw_error.h"
#include "shared_func.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_acl.h"

typedef struct
{
    a_uint32_t pri;
    a_uint32_t addr;
    a_uint32_t size;
    a_uint32_t status;
    a_uint32_t info;
} hsl_acl_blk_t;

typedef struct
{
    a_uint32_t used_blk;
    a_uint32_t total_blk;
    a_uint32_t free_rsc;
    hsl_acl_blk_t *blk_ent;
} hsl_acl_pool_t;

#define MEM_FREE    1
#define MEM_USED    2

static hsl_acl_pool_t acl_pool[SW_MAX_NR_DEV];

static sw_error_t
_hsl_acl_blk_loc(a_uint32_t dev_id, a_uint32_t addr, a_uint32_t * idx);

static sw_error_t
_hsl_acl_blk_comb(a_uint32_t dev_id, a_uint32_t idx, a_uint32_t nr);

static sw_error_t _hsl_acl_free_blk_comb(a_uint32_t dev_id, a_uint32_t idx);

static sw_error_t
_hsl_acl_blk_ent_left_mv(a_uint32_t dev_id, a_uint32_t idx, a_uint32_t offset);

static sw_error_t
_hsl_acl_blk_ent_right_mv(a_uint32_t dev_id, a_uint32_t idx, a_uint32_t offset);

static sw_error_t
_hsl_acl_blk_left_defrag(a_uint32_t dev_id, a_uint32_t p_idx, a_uint32_t t_size,
                         a_bool_t b_must, a_uint32_t * f_idx, a_uint32_t * f_nr,
                         a_uint32_t * f_size);

static sw_error_t
_hsl_acl_blk_right_defrag(a_uint32_t dev_id, a_uint32_t p_idx,
                          a_uint32_t t_size, a_bool_t b_must,
                          a_uint32_t * f_idx, a_uint32_t * f_nr,
                          a_uint32_t * f_size);

static sw_error_t
_hsl_acl_blk_alloc(a_uint32_t dev_id, a_uint32_t free_idx, a_uint32_t pri,
                   a_uint32_t size, a_uint32_t info, a_uint32_t * addr);

static sw_error_t
_hsl_acl_blk_reduce(a_uint32_t dev_id, a_uint32_t idx, a_uint32_t new_size);

static sw_error_t
_hsl_acl_blk_left_enlarge(a_uint32_t dev_id, a_uint32_t idx,
                          a_uint32_t new_size);

static sw_error_t
_hsl_acl_blk_right_enlarge(a_uint32_t dev_id, a_uint32_t idx,
                           a_uint32_t new_size);

static sw_error_t
_hsl_acl_rule_copy(a_uint32_t dev_id, a_uint32_t src_addr, a_uint32_t dest_addr,
                   a_uint32_t size);

static sw_error_t
_hsl_acl_rule_invalid(a_uint32_t dev_id, a_uint32_t addr, a_uint32_t size);

static sw_error_t
_hsl_acl_addr_update(a_uint32_t dev_id, a_uint32_t old_addr,
                     a_uint32_t new_addr, a_uint32_t info);

//#define ACL_POOL_DEBUG
#ifdef ACL_POOL_DEBUG
static void
_hsl_acl_blk_dump(a_uint32_t dev_id, const char *info)
{
    a_uint32_t i;

    aos_printk("\n%s dev_id=%d  free_rsc=%d  total_blk=%d  used_blk=%d",
               info, dev_id, acl_pool[dev_id].free_rsc,
               acl_pool[dev_id].total_blk, acl_pool[dev_id].used_blk);

    for (i = 0; i < acl_pool[dev_id].used_blk; i++)
    {
        aos_printk("\naddr=%d  status = %d  size=%d  list_id=%d  list_pri=%d",
                   acl_pool[dev_id].blk_ent[i].addr,
                   acl_pool[dev_id].blk_ent[i].status,
                   acl_pool[dev_id].blk_ent[i].size,
                   acl_pool[dev_id].blk_ent[i].info,
                   acl_pool[dev_id].blk_ent[i].pri);
    }
    aos_printk("\n");
}
#else
#define _hsl_acl_blk_dump(dev_id, info)
#endif

static sw_error_t
_hsl_acl_blk_loc(a_uint32_t dev_id, a_uint32_t addr, a_uint32_t * idx)
{
    a_uint32_t i;

    for (i = 0; i < acl_pool[dev_id].used_blk; i++)
    {
        if (addr == acl_pool[dev_id].blk_ent[i].addr)
        {
            *idx = i;
            return SW_OK;
        }
    }
    return SW_NOT_FOUND;
}

static sw_error_t
_hsl_acl_blk_comb(a_uint32_t dev_id, a_uint32_t idx, a_uint32_t nr)
{
    sw_error_t rv;
    a_uint32_t i, size;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_comb before combine");

    if ((idx + nr) > acl_pool[dev_id].used_blk)
    {
        return SW_BAD_PARAM;
    }

    if (nr < 2)
    {
        _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_comb after combine");
        return SW_OK;
    }

    size = 0;
    for (i = 0; i < nr; i++)
    {
        size += acl_pool[dev_id].blk_ent[idx + i].size;
    }
    acl_pool[dev_id].blk_ent[idx].size = size;

    rv = _hsl_acl_blk_ent_left_mv(dev_id, idx + nr, nr - 1);
    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_comb after combine");
    return rv;
}

static sw_error_t
_hsl_acl_free_blk_comb(a_uint32_t dev_id, a_uint32_t idx)
{
    sw_error_t rv;
    a_uint32_t first, num;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_free_blk_comb before combine");

    first = idx;
    num = 1;
    if (0 != idx)
    {
        if (MEM_FREE == acl_pool[dev_id].blk_ent[idx - 1].status)
        {
            num++;
            first = idx - 1;
        }
    }

    if ((acl_pool[dev_id].used_blk - 1) != idx)
    {
        if (MEM_FREE == acl_pool[dev_id].blk_ent[idx + 1].status)
        {
            num++;
        }
    }

    rv = _hsl_acl_blk_comb(dev_id, first, num);
    _hsl_acl_blk_dump(dev_id, "_hsl_acl_free_blk_comb after combine");
    return rv;
}

static sw_error_t
_hsl_acl_blk_ent_left_mv(a_uint32_t dev_id, a_uint32_t idx, a_uint32_t offset)
{
    a_uint32_t i;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_ent_left_mv before move");

    if (offset > idx)
    {
        return SW_BAD_PARAM;
    }

    for (i = idx; i < acl_pool[dev_id].used_blk; i++)
    {
        acl_pool[dev_id].blk_ent[i - offset] = acl_pool[dev_id].blk_ent[i];
    }

    acl_pool[dev_id].used_blk -= offset;
    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_ent_left_mv after move");
    return SW_OK;
}

static sw_error_t
_hsl_acl_blk_ent_right_mv(a_uint32_t dev_id, a_uint32_t idx, a_uint32_t offset)
{
    a_uint32_t i, cnt, tmp;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_ent_right_mv before move");

    if (acl_pool[dev_id].total_blk < (acl_pool[dev_id].used_blk + offset))
    {
        return SW_BAD_PARAM;
    }

    /* we support to increase used block number without block moving */
    if (idx > acl_pool[dev_id].used_blk)
    {
        return SW_BAD_PARAM;
    }

    cnt = acl_pool[dev_id].used_blk - idx;
    tmp = acl_pool[dev_id].used_blk - 1;
    for (i = 0; i < cnt; i++)
    {
        acl_pool[dev_id].blk_ent[tmp + offset - i]
            = acl_pool[dev_id].blk_ent[tmp - i];
    }

    acl_pool[dev_id].used_blk += offset;
    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_ent_right_mv after move");
    return SW_OK;
}

static sw_error_t
_hsl_acl_blk_left_defrag(a_uint32_t dev_id, a_uint32_t p_idx, a_uint32_t t_size,
                         a_bool_t b_must, a_uint32_t * f_idx, a_uint32_t * f_nr,
                         a_uint32_t * f_size)
{
    sw_error_t rv;
    a_int32_t idx;
    a_uint32_t i, f_rsc, f_blk, dest_addr;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_left_defrag before defrag");

    f_rsc = 0;
    for (idx = p_idx - 1; idx >= 0; idx--)
    {
        if (MEM_FREE == acl_pool[dev_id].blk_ent[idx].status)
        {
            f_rsc += acl_pool[dev_id].blk_ent[idx].size;
        }

        if (t_size <= f_rsc)
        {
            break;
        }
    }

    if ((f_rsc < t_size) && (A_TRUE == b_must))
    {
        return SW_NO_RESOURCE;
    }

    if (0 == f_rsc)
    {
        *f_idx  = p_idx;
        *f_nr   = 0;
        *f_size = 0;
        _hsl_acl_blk_dump(dev_id, "_hsl_acl_left_defrag after defrag");
        return SW_OK;
    }

    if (idx < 0)
    {
        idx = 0;
    }

    f_blk = 0;
    f_rsc = 0;
    dest_addr = acl_pool[dev_id].blk_ent[idx].addr;
    for (i = idx; i < p_idx; i++)
    {
        if (MEM_FREE == acl_pool[dev_id].blk_ent[i].status)
        {
            f_blk += 1;
            f_rsc += acl_pool[dev_id].blk_ent[i].size;
        }

        if (MEM_USED == acl_pool[dev_id].blk_ent[i].status)
        {
            if (dest_addr != acl_pool[dev_id].blk_ent[i].addr)
            {
                /* update acl rules hardware position */
                rv = _hsl_acl_rule_copy(dev_id,
                                        acl_pool[dev_id].blk_ent[i].addr,
                                        dest_addr,
                                        acl_pool[dev_id].blk_ent[i].size);
                SW_RTN_ON_ERROR(rv);

                rv = _hsl_acl_addr_update(dev_id,
                                          acl_pool[dev_id].blk_ent[i].addr,
                                          dest_addr,
                                          acl_pool[dev_id].blk_ent[i].info);
                SW_RTN_ON_ERROR(rv);

                /* update acl memory block control infomation */
                acl_pool[dev_id].blk_ent[i - f_blk] =
                    acl_pool[dev_id].blk_ent[i];
                acl_pool[dev_id].blk_ent[i - f_blk].addr -= f_rsc;
            }
            dest_addr += acl_pool[dev_id].blk_ent[i].size;
        }
    }

    for (i = p_idx - f_blk; i < p_idx; i++)
    {
        acl_pool[dev_id].blk_ent[i].status = MEM_FREE;
        acl_pool[dev_id].blk_ent[i].addr = dest_addr;
        acl_pool[dev_id].blk_ent[i].size = 0;
        acl_pool[dev_id].blk_ent[i].info = 0;
        acl_pool[dev_id].blk_ent[i].pri  = 0;
    }
    acl_pool[dev_id].blk_ent[p_idx - f_blk].size = f_rsc;

    *f_idx = p_idx - f_blk;
    *f_nr = f_blk;
    *f_size = f_rsc;
    rv = _hsl_acl_rule_invalid(dev_id, acl_pool[dev_id].blk_ent[*f_idx].addr,
                               f_rsc);
    _hsl_acl_blk_dump(dev_id, "_hsl_acl_left_defrag after defrag");
    return rv;
}

static sw_error_t
_hsl_acl_blk_right_defrag(a_uint32_t dev_id, a_uint32_t p_idx,
                          a_uint32_t t_size, a_bool_t b_must,
                          a_uint32_t * f_idx, a_uint32_t * f_nr,
                          a_uint32_t * f_size)
{
    sw_error_t rv;
    a_uint32_t i, cnt;
    a_uint32_t idx, f_rsc, f_blk, dest_addr;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_right_defrag before defrag");

    f_rsc = 0;
    for (idx = p_idx; idx < acl_pool[dev_id].used_blk; idx++)
    {
        if (MEM_FREE == acl_pool[dev_id].blk_ent[idx].status)
        {
            f_rsc += acl_pool[dev_id].blk_ent[idx].size;
        }

        if (t_size <= f_rsc)
        {
            break;
        }
    }

    if ((f_rsc < t_size) && (A_TRUE == b_must))
    {
        return SW_NO_RESOURCE;
    }

    if (0 == f_rsc)
    {
        *f_idx  = p_idx;
        *f_nr   = 0;
        *f_size = 0;
        _hsl_acl_blk_dump(dev_id, "_hsl_acl_right_defrag after defrag");
        return SW_OK;
    }

    if (idx >= acl_pool[dev_id].used_blk)
    {
        idx = acl_pool[dev_id].used_blk - 1;
    }

    f_blk = 0;
    f_rsc = 0;
    dest_addr = acl_pool[dev_id].blk_ent[idx].addr
                + acl_pool[dev_id].blk_ent[idx].size;
    for (cnt = 0; cnt <= (idx -p_idx); cnt++)
    {
        i = idx - cnt;
        if (MEM_FREE == acl_pool[dev_id].blk_ent[i].status)
        {
            f_blk += 1;
            f_rsc += acl_pool[dev_id].blk_ent[i].size;
        }

        if (MEM_USED == acl_pool[dev_id].blk_ent[i].status)
        {
            dest_addr -= acl_pool[dev_id].blk_ent[i].size;

            if (dest_addr != acl_pool[dev_id].blk_ent[i].addr)
            {
                /* update acl rules hardware position */
                rv = _hsl_acl_rule_copy(dev_id,
                                        acl_pool[dev_id].blk_ent[i].addr,
                                        dest_addr,
                                        acl_pool[dev_id].blk_ent[i].size);
                SW_RTN_ON_ERROR(rv);

                rv = _hsl_acl_addr_update(dev_id,
                                          acl_pool[dev_id].blk_ent[i].addr,
                                          dest_addr,
                                          acl_pool[dev_id].blk_ent[i].info);
                SW_RTN_ON_ERROR(rv);

                /* update acl memory block control infomation */
                acl_pool[dev_id].blk_ent[i + f_blk] =
                    acl_pool[dev_id].blk_ent[i];
                acl_pool[dev_id].blk_ent[i + f_blk].addr += f_rsc;
            }
        }
    }

    for (i = p_idx; i < (p_idx + f_blk); i++)
    {
        acl_pool[dev_id].blk_ent[i].status = MEM_FREE;
        acl_pool[dev_id].blk_ent[i].size = 0;
        acl_pool[dev_id].blk_ent[i].addr = dest_addr - f_rsc;
    }
    acl_pool[dev_id].blk_ent[p_idx].size = f_rsc;

    *f_idx = p_idx;
    *f_nr = f_blk;
    *f_size = f_rsc;
    rv = _hsl_acl_rule_invalid(dev_id, acl_pool[dev_id].blk_ent[*f_idx].addr,
                               f_rsc);
    _hsl_acl_blk_dump(dev_id, "_hsl_acl_right_defrag after defrag");
    return rv;
}

static sw_error_t
_hsl_acl_blk_alloc(a_uint32_t dev_id, a_uint32_t free_idx, a_uint32_t pri,
                   a_uint32_t size, a_uint32_t info, a_uint32_t * addr)
{
    sw_error_t rv;
    a_uint32_t i;
    a_bool_t   b_comb = A_FALSE;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_alloc before alloc");

    if (MEM_FREE != acl_pool[dev_id].blk_ent[free_idx].status)
    {
        return SW_BAD_PARAM;
    }

    if (size > acl_pool[dev_id].blk_ent[free_idx].size)
    {
        return SW_NO_RESOURCE;
    }

    if (size != acl_pool[dev_id].blk_ent[free_idx].size)
    {
        b_comb = A_TRUE;
        i  = free_idx + 1;
        rv = _hsl_acl_blk_ent_right_mv(dev_id, i, 1);
        SW_RTN_ON_ERROR(rv);

        acl_pool[dev_id].blk_ent[i].addr =
            acl_pool[dev_id].blk_ent[free_idx].addr + size;
        acl_pool[dev_id].blk_ent[i].size =
            acl_pool[dev_id].blk_ent[free_idx].size - size;
        acl_pool[dev_id].blk_ent[i].status = MEM_FREE;
        acl_pool[dev_id].blk_ent[i].pri    = 0;
        acl_pool[dev_id].blk_ent[i].info   = 0;
    }

    acl_pool[dev_id].blk_ent[free_idx].status = MEM_USED;
    acl_pool[dev_id].blk_ent[free_idx].size = size;
    acl_pool[dev_id].blk_ent[free_idx].pri  = pri;
    acl_pool[dev_id].blk_ent[free_idx].info = info;
    acl_pool[dev_id].free_rsc -= size;

    if (A_TRUE == b_comb)
    {
        /* try to combine neighbor free memory blocks */
        rv = _hsl_acl_free_blk_comb(dev_id, free_idx + 1);
        SW_RTN_ON_ERROR(rv);
    }

    *addr = acl_pool[dev_id].blk_ent[free_idx].addr;
    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_alloc after alloc");
    return SW_OK;
}

static sw_error_t
_hsl_acl_blk_reduce(a_uint32_t dev_id, a_uint32_t idx, a_uint32_t new_size)
{
    sw_error_t rv;
    a_uint32_t addr, old_size;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_reduce before reduce");

    addr = acl_pool[dev_id].blk_ent[idx].addr;
    old_size = acl_pool[dev_id].blk_ent[idx].size;

    rv = _hsl_acl_blk_ent_right_mv(dev_id, idx + 1, 1);
    SW_RTN_ON_ERROR(rv);

    acl_pool[dev_id].blk_ent[idx].size = new_size;
    acl_pool[dev_id].blk_ent[idx + 1].status = MEM_FREE;
    acl_pool[dev_id].blk_ent[idx + 1].addr = addr + new_size;
    acl_pool[dev_id].blk_ent[idx + 1].size = old_size - new_size;
    acl_pool[dev_id].blk_ent[idx + 1].pri  = 0;
    acl_pool[dev_id].blk_ent[idx + 1].info = 0;
    acl_pool[dev_id].free_rsc += (old_size - new_size);

    /* try to combine neighbor free blocks */
    rv = _hsl_acl_free_blk_comb(dev_id, idx + 1);
    SW_RTN_ON_ERROR(rv);

    rv = _hsl_acl_rule_invalid(dev_id, addr + new_size, old_size - new_size);
    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_reduce after reduce");
    return rv;
}

static sw_error_t
_hsl_acl_blk_left_enlarge(a_uint32_t dev_id, a_uint32_t idx,
                          a_uint32_t new_size)
{
    sw_error_t rv;
    a_uint32_t old_size, old_addr, new_addr;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_left_enlarge before enlarge");

    if (0 == idx)
    {
        return SW_BAD_PARAM;
    }

    if (MEM_FREE != acl_pool[dev_id].blk_ent[idx - 1].status)
    {
        return SW_BAD_PARAM;
    }

    old_size = acl_pool[dev_id].blk_ent[idx].size;
    if ((new_size - old_size) > acl_pool[dev_id].blk_ent[idx - 1].size)
    {
        return SW_BAD_PARAM;
    }

    old_addr = acl_pool[dev_id].blk_ent[idx].addr;
    new_addr = old_addr - (new_size - old_size);
    rv = _hsl_acl_rule_copy(dev_id, old_addr, new_addr, old_size);
    SW_RTN_ON_ERROR(rv);

    rv = _hsl_acl_rule_invalid(dev_id, new_addr + old_size, new_size - old_size);
    SW_RTN_ON_ERROR(rv);

    acl_pool[dev_id].blk_ent[idx].size = new_size;
    acl_pool[dev_id].blk_ent[idx].addr = new_addr;
    acl_pool[dev_id].free_rsc         -= (new_size - old_size);
    rv = _hsl_acl_addr_update(dev_id, old_addr, new_addr,
                              acl_pool[dev_id].blk_ent[idx].info);
    SW_RTN_ON_ERROR(rv);

    rv = SW_OK;
    if ((new_size - old_size) == acl_pool[dev_id].blk_ent[idx - 1].size)
    {
        rv = _hsl_acl_blk_ent_left_mv(dev_id, idx, 1);
    }
    else
    {
        acl_pool[dev_id].blk_ent[idx - 1].size -= (new_size - old_size);
    }

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_left_enlarge after enlarge");
    return rv;
}

static sw_error_t
_hsl_acl_blk_right_enlarge(a_uint32_t dev_id, a_uint32_t idx,
                           a_uint32_t new_size)
{
    sw_error_t rv;
    a_uint32_t old_size;

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_right_enlarge before enlarge");

    if ((idx + 1) >= acl_pool[dev_id].used_blk)
    {
        return SW_BAD_PARAM;
    }

    if (MEM_FREE != acl_pool[dev_id].blk_ent[idx + 1].status)
    {
        return SW_BAD_PARAM;
    }

    old_size = acl_pool[dev_id].blk_ent[idx].size;
    if ((new_size - old_size) > acl_pool[dev_id].blk_ent[idx + 1].size)
    {
        return SW_BAD_PARAM;
    }

    acl_pool[dev_id].blk_ent[idx].size = new_size;
    acl_pool[dev_id].free_rsc         -= (new_size - old_size);
    rv = SW_OK;
    if ((new_size - old_size) < acl_pool[dev_id].blk_ent[idx + 1].size)
    {
        acl_pool[dev_id].blk_ent[idx + 1].size -= (new_size - old_size);
        acl_pool[dev_id].blk_ent[idx + 1].addr += (new_size - old_size);
    }
    else
    {
        if ((idx + 2) < acl_pool[dev_id].used_blk)
        {
            rv = _hsl_acl_blk_ent_left_mv(dev_id, idx + 2, 1);
        }
    }

    _hsl_acl_blk_dump(dev_id, "_hsl_acl_blk_right_enlarge after enlarge");
    return rv;
}

static sw_error_t
_hsl_acl_rule_copy(a_uint32_t dev_id, a_uint32_t src_addr, a_uint32_t dest_addr,
                   a_uint32_t size)
{
    hsl_acl_func_t * p_api;
    sw_error_t       rv;

    p_api = hsl_acl_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_api);

    if (NULL == p_api->acl_rule_copy)
        return SW_NOT_SUPPORTED;

    rv = p_api->acl_rule_copy(dev_id, src_addr, dest_addr, size);
    return rv;
}

static sw_error_t
_hsl_acl_rule_invalid(a_uint32_t dev_id, a_uint32_t addr, a_uint32_t size)
{
    hsl_acl_func_t * p_api;
    sw_error_t       rv;

    p_api = hsl_acl_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_api);

    if (NULL == p_api->acl_rule_invalid)
        return SW_NOT_SUPPORTED;

    rv = p_api->acl_rule_invalid(dev_id, addr, size);
    return rv;
}

static sw_error_t
_hsl_acl_addr_update(a_uint32_t dev_id, a_uint32_t old_addr,
                     a_uint32_t new_addr, a_uint32_t info)
{
    hsl_acl_func_t * p_api;
    sw_error_t       rv;

    p_api = hsl_acl_ptr_get(dev_id);
    SW_RTN_ON_NULL(p_api);

    if (NULL == p_api->acl_addr_update)
        return SW_NOT_SUPPORTED;

    rv = p_api->acl_addr_update(dev_id, old_addr, new_addr, info);
    return rv;
}

sw_error_t
hsl_acl_pool_creat(a_uint32_t dev_id, a_uint32_t blk_nr, a_uint32_t rsc_nr)
{
    HSL_DEV_ID_CHECK(dev_id);

    acl_pool[dev_id].blk_ent = aos_mem_alloc(blk_nr * (sizeof (hsl_acl_blk_t)));
    if (NULL == acl_pool[dev_id].blk_ent)
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(acl_pool[dev_id].blk_ent, blk_nr * (sizeof (hsl_acl_blk_t)));

    acl_pool[dev_id].used_blk  = 1;
    acl_pool[dev_id].total_blk = blk_nr;
    acl_pool[dev_id].free_rsc  = rsc_nr;

    acl_pool[dev_id].blk_ent[0].addr = 0;
    acl_pool[dev_id].blk_ent[0].size = rsc_nr;
    acl_pool[dev_id].blk_ent[0].status = MEM_FREE;

    _hsl_acl_blk_dump(dev_id, "hsl_acl_pool_creat after creat");
    return SW_OK;
}

sw_error_t
hsl_acl_pool_destroy(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (NULL == acl_pool[dev_id].blk_ent)
    {
        return SW_FAIL;
    }

    aos_mem_free(acl_pool[dev_id].blk_ent);
    aos_mem_zero(&acl_pool[dev_id], sizeof(acl_pool[dev_id]));
    return SW_OK;
}

sw_error_t
hsl_acl_blk_alloc(a_uint32_t dev_id, a_uint32_t pri, a_uint32_t size,
                  a_uint32_t info, a_uint32_t * addr)
{
    sw_error_t rv;
    a_uint32_t i;
    a_uint32_t blk_nr;
    a_uint32_t p_idx, largest_idx, prev_f_s, largest_f_s;
    a_uint32_t l_idx, l_nr, l_size, r_idx, r_nr, r_size;

    HSL_DEV_ID_CHECK(dev_id);

    _hsl_acl_blk_dump(dev_id, "hsl_acl_blk_alloc before alloc");

    if (0 == size)
    {
        return SW_BAD_PARAM;
    }

    if (size > acl_pool[dev_id].free_rsc)
    {
        return SW_NO_RESOURCE;
    }

    blk_nr = acl_pool[dev_id].used_blk;

    p_idx       = 0;
    prev_f_s    = 0;
    largest_f_s = 0;
    largest_idx = 0;

    for (i = 0; i < blk_nr; i++)
    {
        if (MEM_FREE == acl_pool[dev_id].blk_ent[i].status)
        {
            prev_f_s += acl_pool[dev_id].blk_ent[i].size;
            continue;
        }

        p_idx = i;
        if (pri <= acl_pool[dev_id].blk_ent[i].pri)
        {
            break;
        }
    }

    if (i == blk_nr)
    {
        p_idx = blk_nr;
    }

    for (i = p_idx; i < blk_nr; i++)
    {
        if (MEM_FREE == acl_pool[dev_id].blk_ent[i].status)
        {
            if (largest_f_s < acl_pool[dev_id].blk_ent[i].size)
            {
                largest_idx = i;
                largest_f_s = acl_pool[dev_id].blk_ent[i].size;
            }
            continue;
        }

        if (pri != acl_pool[dev_id].blk_ent[i].pri)
        {
            break;
        }
    }

    if (largest_f_s >= size)
    {
        rv = _hsl_acl_blk_alloc(dev_id, largest_idx, pri, size, info,
                                addr);

    }
    else if (prev_f_s >= size)
    {
        rv = _hsl_acl_blk_left_defrag(dev_id, p_idx, size, A_TRUE, &l_idx,
                                      &l_nr, &l_size);
        SW_RTN_ON_ERROR(rv);

        rv = _hsl_acl_blk_comb(dev_id, l_idx, l_nr);
        SW_RTN_ON_ERROR(rv);

        rv = _hsl_acl_blk_alloc(dev_id, l_idx, pri, size, info, addr);
    }
    else if ((acl_pool[dev_id].free_rsc - prev_f_s) >= size)
    {
        rv = _hsl_acl_blk_right_defrag(dev_id, p_idx, size, A_TRUE, &r_idx,
                                       &r_nr, &r_size);
        SW_RTN_ON_ERROR(rv);

        rv = _hsl_acl_blk_comb(dev_id, r_idx, r_nr);
        SW_RTN_ON_ERROR(rv);

        rv = _hsl_acl_blk_alloc(dev_id, r_idx, pri, size, info, addr);
    }
    else
    {
        rv = _hsl_acl_blk_left_defrag(dev_id, p_idx, size, A_FALSE, &l_idx,
                                      &l_nr, &l_size);
        SW_RTN_ON_ERROR(rv);

        rv = _hsl_acl_blk_right_defrag(dev_id, p_idx, size, A_FALSE, &r_idx,
                                       &r_nr, &r_size);
        SW_RTN_ON_ERROR(rv);

        rv = _hsl_acl_blk_comb(dev_id, l_idx, (l_nr + r_nr));
        SW_RTN_ON_ERROR(rv);

        rv = _hsl_acl_blk_alloc(dev_id, l_idx, pri, size, info, addr);
    }

    _hsl_acl_blk_dump(dev_id, "hsl_acl_blk_alloc after alloc");
    return rv;
}

sw_error_t
hsl_acl_blk_free(a_uint32_t dev_id, a_uint32_t addr)
{
    sw_error_t rv;
    a_uint32_t idx;

    HSL_DEV_ID_CHECK(dev_id);

    _hsl_acl_blk_dump(dev_id, "hsl_acl_blk_free before free");

    rv = _hsl_acl_blk_loc(dev_id, addr, &idx);
    SW_RTN_ON_ERROR(rv);

    acl_pool[dev_id].blk_ent[idx].status = MEM_FREE;
    acl_pool[dev_id].blk_ent[idx].pri  = 0;
    acl_pool[dev_id].blk_ent[idx].info = 0;
    acl_pool[dev_id].free_rsc += acl_pool[dev_id].blk_ent[idx].size;

    rv = _hsl_acl_rule_invalid(dev_id, addr, acl_pool[dev_id].blk_ent[idx].size);
    SW_RTN_ON_ERROR(rv);

    rv = _hsl_acl_free_blk_comb(dev_id, idx);
    SW_RTN_ON_ERROR(rv);

    _hsl_acl_blk_dump(dev_id, "hsl_acl_blk_free after free");
    return SW_OK;
}

sw_error_t
hsl_acl_blk_resize(a_uint32_t dev_id, a_uint32_t addr, a_uint32_t new_size)
{
    sw_error_t rv;
    a_uint32_t idx, l_idx, l_nr, l_size, r_idx, r_nr, r_size, old_size;

    HSL_DEV_ID_CHECK(dev_id);

    _hsl_acl_blk_dump(dev_id, "hsl_acl_blk_resize before resize");

    rv = _hsl_acl_blk_loc(dev_id, addr, &idx);
    SW_RTN_ON_ERROR(rv);

    if (MEM_USED != acl_pool[dev_id].blk_ent[idx].status)
    {
        return SW_BAD_PARAM;
    }

    old_size = acl_pool[dev_id].blk_ent[idx].size;
    if (new_size == old_size)
    {
        return SW_OK;
    }

    if (0 == new_size)
    {
        rv = hsl_acl_blk_free(dev_id, addr);
        return rv;
    }

    /* reduce acl memory block size */
    if (new_size < old_size)
    {
        rv = _hsl_acl_blk_reduce(dev_id, idx, new_size);
        _hsl_acl_blk_dump(dev_id, "hsl_acl_blk_resize after resize");
        return rv;
    }

    /* enlarge acl memory block size */
    if (acl_pool[dev_id].free_rsc < (new_size - old_size))
    {
        return SW_NO_RESOURCE;
    }

    rv = _hsl_acl_blk_left_defrag(dev_id, idx, new_size - old_size,
                                  A_FALSE, &l_idx, &l_nr, &l_size);
    SW_RTN_ON_ERROR(rv);

    rv = _hsl_acl_blk_comb(dev_id, l_idx, l_nr);
    SW_RTN_ON_ERROR(rv);

    rv = _hsl_acl_blk_loc(dev_id, addr, &idx);
    SW_RTN_ON_ERROR(rv);

    if (l_size >= (new_size - old_size))
    {
        rv = _hsl_acl_blk_left_enlarge(dev_id, idx, new_size);
        _hsl_acl_blk_dump(dev_id, "hsl_acl_blk_resize after resize");
        return rv;
    }

    if (idx >= (acl_pool[dev_id].used_blk - 1))
    {
        return SW_NO_RESOURCE;
    }
    rv = _hsl_acl_blk_right_defrag(dev_id, idx + 1, new_size - old_size,
                                   A_FALSE, &r_idx, &r_nr, &r_size);
    SW_RTN_ON_ERROR(rv);

    rv = _hsl_acl_blk_comb(dev_id, r_idx, r_nr);
    SW_RTN_ON_ERROR(rv);

    rv = _hsl_acl_blk_loc(dev_id, addr, &idx);
    SW_RTN_ON_ERROR(rv);

    if (r_size >= (new_size - old_size))
    {
        rv = _hsl_acl_blk_right_enlarge(dev_id, idx, new_size);
        _hsl_acl_blk_dump(dev_id, "hsl_acl_blk_resize after resize");
        return rv;
    }

    rv = _hsl_acl_blk_right_enlarge(dev_id, idx, old_size + r_size);
    SW_RTN_ON_ERROR(rv);

    rv = _hsl_acl_blk_left_enlarge(dev_id, idx, new_size);
    _hsl_acl_blk_dump(dev_id, "hsl_acl_blk_resize after resize");
    return rv;
}

sw_error_t
hsl_acl_free_rsc_get(a_uint32_t dev_id, a_uint32_t * free_rsc)
{
    HSL_DEV_ID_CHECK(dev_id);

    * free_rsc = acl_pool[dev_id].free_rsc;
    return SW_OK;
}
