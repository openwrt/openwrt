/*
 * Copyright (c) 2012, 2016, 2018, The Linux Foundation. All rights reserved.
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
 * @defgroup isisc_acl ISISC_ACL
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_acl.h"
#include "isisc_acl.h"
#include "isisc_reg.h"
#include "isisc_acl_prv.h"

//#define ISISC_ACL_DEBUG
//#define ISISC_SW_ENTRY
#define ISISC_HW_ENTRY

static isisc_acl_list_t *sw_list_ent[SW_MAX_NR_DEV];
static isisc_acl_rule_t *sw_rule_ent[SW_MAX_NR_DEV];

static isisc_acl_rule_t *sw_rule_tmp[SW_MAX_NR_DEV];
static isisc_acl_rule_t *hw_rule_tmp[SW_MAX_NR_DEV];
#ifdef ISISC_SW_ENTRY
static a_uint8_t *sw_filter_mem = NULL;
#endif

static sw_error_t
_isisc_filter_valid_set(a_uint32_t dev_id, a_uint32_t flt_idx, a_uint32_t flag);

static sw_error_t
_isisc_filter_ports_bind(a_uint32_t dev_id, a_uint32_t flt_idx,
                        a_uint32_t ports);

#ifdef ISISC_SW_ENTRY
static sw_error_t
_isisc_filter_write(a_uint32_t dev_id, a_uint32_t reg[], a_uint32_t flt_idx,
                   a_uint32_t op);

static sw_error_t
_isisc_filter_read(a_uint32_t dev_id, a_uint32_t reg[], a_uint32_t flt_idx,
                  a_uint32_t op);
#endif

static sw_error_t
_isisc_filter_down_to_hw(a_uint32_t dev_id, hw_filter_t * filter,
                        a_uint32_t flt_idx);

static sw_error_t
_isisc_filter_up_to_sw(a_uint32_t dev_id, hw_filter_t * filter,
                      a_uint32_t flt_idx);

static sw_error_t
_isisc_acl_rule_src_filter_sts_set(a_uint32_t dev_id,
                                  a_uint32_t rule_id, a_bool_t enable);

static void
_isisc_acl_list_dump(a_uint32_t dev_id)
{
    a_uint32_t i;
    isisc_acl_list_t *sw_list;

    aos_printk("\ndev_id=%d  list control infomation:", dev_id);
    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        sw_list = &(sw_list_ent[dev_id][i]);
        if (ENT_USED & sw_list->status)
        {
            aos_printk
            ("\nlist_id=%02d  list_pri=%02d  rule_nr=%02d  [pts_map]:0x%02x  idx=%02d ",
             sw_list->list_id, sw_list->list_pri, sw_list->rule_nr,
             sw_list->bind_pts, i);
        }
    }
    aos_printk("\n");
}

static void
_isisc_acl_sw_rule_dump(char *info, isisc_acl_rule_t * sw_rule)
{
#ifdef ISISC_ACL_DEBUG
    a_uint32_t flt_idx, i;

    aos_printk("\n%s", info);
    for (flt_idx = 0; flt_idx < ISISC_MAX_FILTER; flt_idx++)
    {
        aos_printk("\n%d software filter:", flt_idx);
        aos_printk("\nact:");
        for (i = 0; i < 3; i++)
        {
            aos_printk("%08x  ", sw_rule[flt_idx].filter.act[i]);
        }

        aos_printk("\nvlu:");
        for (i = 0; i < 5; i++)
        {
            aos_printk("%08x  ", sw_rule[flt_idx].filter.vlu[i]);
        }

        aos_printk("\nmsk:");
        for (i = 0; i < 5; i++)
        {
            aos_printk("%08x  ", sw_rule[flt_idx].filter.msk[i]);
        }

        aos_printk("\nctl:status[%02d] list_id[%02d] rule_id[%02d]",
                   sw_rule[flt_idx].status,
                   sw_rule[flt_idx].list_id, sw_rule[flt_idx].rule_id);

        aos_printk("\n\n");
    }
#else
    return;
#endif
}

static isisc_acl_list_t *
_isisc_acl_list_loc(a_uint32_t dev_id, a_uint32_t list_id)
{
    a_uint32_t i;

    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        if ((ENT_USED & sw_list_ent[dev_id][i].status)
                && (list_id == sw_list_ent[dev_id][i].list_id))
        {
            return &(sw_list_ent[dev_id][i]);
        }
    }
    return NULL;
}

static sw_error_t
_isisc_filter_valid_set(a_uint32_t dev_id, a_uint32_t flt_idx, a_uint32_t flag)
{
#ifdef ISISC_SW_ENTRY
    hw_filter_t filter;

    _isisc_filter_up_to_sw(dev_id, &filter, flt_idx);

    filter.msk[4] &= 0xfffffff8;
    filter.msk[4] |= (flag & 0x7);

    _isisc_filter_down_to_hw(dev_id, &filter, flt_idx);

    return SW_OK;
#else
#ifdef ISISC_HW_ENTRY
    hw_filter_t filter;

    filter = sw_rule_ent[dev_id][flt_idx].filter;

    filter.msk[4] &= 0xfffffff8;
    filter.msk[4] |= (flag & 0x7);

    _isisc_filter_down_to_hw(dev_id, &filter, flt_idx);
    return SW_OK;
#else
    sw_error_t rv;
    a_uint32_t addr, data = 0;

    /* read filter mask at first */
    addr = ISISC_RULE_FUNC_ADDR;
    data = (flt_idx & 0x7f) | (0x1 << 8) | (0x1 << 10) | (0x1 << 31);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* get filter mask and modify it */
    addr = ISISC_RULE_FUNC_ADDR + 20;
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data &= 0xfffffff8;
    data |= (flag & 0x7);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* write back filter mask */
    addr = ISISC_RULE_FUNC_ADDR;
    data = (flt_idx & 0x7f) | (0x1 << 8) | (0x1 << 31);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
#endif
#endif
}

static sw_error_t
_isisc_filter_ports_bind(a_uint32_t dev_id, a_uint32_t flt_idx, a_uint32_t ports)
{
#ifdef ISISC_SW_ENTRY
    hw_filter_t filter;

    _isisc_filter_up_to_sw(dev_id, &filter, flt_idx);

    filter.vlu[4] &= 0xffffff80;
    filter.vlu[4] |= (ports & 0x7f);

    _isisc_filter_down_to_hw(dev_id, &filter, flt_idx);

    return SW_OK;
#else
#ifdef ISISC_HW_ENTRY
    hw_filter_t filter;

    filter = sw_rule_ent[dev_id][flt_idx].filter;

    filter.vlu[4] &= 0xffffff80;
    filter.vlu[4] |= (ports & 0x7f);

    _isisc_filter_down_to_hw(dev_id, &filter, flt_idx);

    return SW_OK;
#else
    sw_error_t rv;
    a_uint32_t addr, data;

    /* read filter value at first */
    addr = ISISC_RULE_FUNC_ADDR;
    data = (flt_idx & 0x7f) | (0x1 << 10) | (0x1 << 31);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* get filter value and modify it */
    addr = ISISC_RULE_FUNC_ADDR + 20;
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data &= 0xffffff80;
    data |= (ports & 0x7f);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* write back filter value */
    addr = ISISC_RULE_FUNC_ADDR;
    data = (flt_idx & 0x7f) | (0x1 << 31);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
#endif
#endif
}

#ifdef ISISC_SW_ENTRY
static sw_error_t
_isisc_filter_write(a_uint32_t dev_id, a_uint32_t reg[], a_uint32_t flt_idx,
                   a_uint32_t op)
{
    a_uint32_t i, addr, data, idx = 6;
    sw_error_t rv;

    if (ISISC_FILTER_ACT_OP == op)
    {
        idx = 4;
    }

    for (i = 1; i < idx; i++)
    {
        addr = ISISC_RULE_FUNC_ADDR + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(reg[i - 1])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    addr = ISISC_RULE_FUNC_ADDR;
    data = (flt_idx & 0x7f) | (op << 8) | (0x1 << 31);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_isisc_filter_read(a_uint32_t dev_id, a_uint32_t reg[], a_uint32_t flt_idx,
                  a_uint32_t op)
{
    a_uint32_t i, addr, data, idx = 6;
    sw_error_t rv;

    addr = ISISC_RULE_FUNC_ADDR;
    data = (flt_idx & 0x7f) | (op << 8) | (0x1 << 10) | (0x1 << 31);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (ISISC_FILTER_ACT_OP == op)
    {
        idx = 4;
    }

    for (i = 1; i < idx; i++)
    {
        addr = ISISC_RULE_FUNC_ADDR + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(reg[i - 1])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}
#endif

static sw_error_t
_isisc_filter_down_to_hw(a_uint32_t dev_id, hw_filter_t * filter,
                        a_uint32_t flt_idx)
{
#ifdef ISISC_SW_ENTRY
    a_uint8_t *tbl = sw_filter_mem + sizeof (hw_filter_t) * flt_idx;

    aos_mem_copy(tbl, filter, sizeof (hw_filter_t));
#else
#ifdef ISISC_HW_ENTRY
    sw_error_t rv;
    a_uint32_t i, base, addr;

    base = ISISC_FILTER_ACT_ADDR + (flt_idx << 4);
    for (i = 0; i < 3; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(filter->act[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    base = ISISC_FILTER_VLU_ADDR + (flt_idx << 5);
    for (i = 0; i < 5; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(filter->vlu[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    base = ISISC_FILTER_MSK_ADDR + (flt_idx << 5);
    for (i = 0; i < 5; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(filter->msk[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
#else
    sw_error_t rv;

    rv = _isisc_filter_write(dev_id, &(filter->act[0]), flt_idx,
                            ISISC_FILTER_ACT_OP);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_filter_write(dev_id, &(filter->vlu[0]), flt_idx,
                            ISISC_FILTER_VLU_OP);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_filter_write(dev_id, &(filter->msk[0]), flt_idx,
                            ISISC_FILTER_MSK_OP);
    SW_RTN_ON_ERROR(rv);
#endif
#endif

    return SW_OK;
}

static sw_error_t
_isisc_filter_up_to_sw(a_uint32_t dev_id, hw_filter_t * filter,
                      a_uint32_t flt_idx)
{
#ifdef ISISC_SW_ENTRY
    a_uint8_t *tbl = sw_filter_mem + sizeof (hw_filter_t) * flt_idx;

    aos_mem_copy(filter, tbl, sizeof (hw_filter_t));
#else
#ifdef ISISC_HW_ENTRY
    sw_error_t rv;
    a_uint32_t i, base, addr;

    base = ISISC_FILTER_VLU_ADDR + (flt_idx << 5);
    for (i = 0; i < 5; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(filter->vlu[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    base = ISISC_FILTER_MSK_ADDR + (flt_idx << 5);
    for (i = 0; i < 5; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(filter->msk[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    base = ISISC_FILTER_ACT_ADDR + (flt_idx << 4);
    for (i = 0; i < 3; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(filter->act[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
#else
    sw_error_t rv;

    rv = _isisc_filter_read(dev_id, &(filter->vlu[0]), flt_idx,
                           ISISC_FILTER_VLU_OP);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_filter_read(dev_id, &(filter->msk[0]), flt_idx,
                           ISISC_FILTER_MSK_OP);
    SW_RTN_ON_ERROR(rv);

    rv = _isisc_filter_read(dev_id, &(filter->act[0]), flt_idx,
                           ISISC_FILTER_ACT_OP);
    SW_RTN_ON_ERROR(rv);
#endif
#endif

    return SW_OK;
}

static sw_error_t
_isisc_acl_list_insert(a_uint32_t dev_id, a_uint32_t * src_idx,
                      a_uint32_t * dst_idx, isisc_acl_rule_t * src_rule,
                      isisc_acl_rule_t * dst_rule)
{
    a_uint32_t i, data, rule_id, list_id, list_pri;

    rule_id = 0;
    list_id = src_rule[*src_idx].list_id;
    list_pri = src_rule[*src_idx].list_pri;

    for (i = *src_idx; i < ISISC_MAX_FILTER; i++)
    {
        if (!(ENT_USED & src_rule[i].status))
        {
            continue; // was: break;
        }

        if (src_rule[i].list_id != list_id)
        {
            break;
        }

        SW_GET_FIELD_BY_REG(MAC_RUL_M4, RULE_TYP, data,
                            src_rule[i].filter.msk[4]);
        if (!data)
        {
            continue;
        }

        if (ISISC_MAX_FILTER <= *dst_idx)
        {
            return SW_NO_RESOURCE;
        }

        if (ENT_USED & dst_rule[*dst_idx].status)
        {
            return SW_NO_RESOURCE;
        }

        SW_GET_FIELD_BY_REG(MAC_RUL_M4, RULE_VALID, data,
                            src_rule[i].filter.msk[4]);
        if ((FLT_START == data) && (*dst_idx % 2))
        {
            if (*src_idx != i)
            {
                dst_rule[*dst_idx].src_flt_dis = src_rule[i].src_flt_dis;
                dst_rule[*dst_idx].list_id = list_id;
                dst_rule[*dst_idx].list_pri = list_pri;
                dst_rule[*dst_idx].rule_id = rule_id - 1;
                dst_rule[*dst_idx].status |= ENT_USED;
            }

            (*dst_idx)++;
            if (ISISC_MAX_FILTER <= *dst_idx)
            {
                return SW_NO_RESOURCE;
            }

            if (ENT_USED & dst_rule[*dst_idx].status)
            {
                return SW_NO_RESOURCE;
            }
        }

        aos_mem_copy(&(dst_rule[*dst_idx].filter), &(src_rule[i].filter),
                     sizeof (hw_filter_t));
        dst_rule[*dst_idx].src_flt_dis = src_rule[i].src_flt_dis;
        dst_rule[*dst_idx].list_id = list_id;
        dst_rule[*dst_idx].list_pri = list_pri;
        dst_rule[*dst_idx].rule_id = rule_id;
        dst_rule[*dst_idx].status |= ENT_USED;
        if (ENT_DEACTIVE & src_rule[i].status)
        {
            dst_rule[*dst_idx].status |= ENT_DEACTIVE;
        }
        (*dst_idx)++;

        if ((FLT_END == data) && (*dst_idx % 2))
        {
            if (ISISC_MAX_FILTER > *dst_idx)
            {
                dst_rule[*dst_idx].src_flt_dis = src_rule[i].src_flt_dis;
                dst_rule[*dst_idx].list_id = list_id;
                dst_rule[*dst_idx].list_pri = list_pri;
                dst_rule[*dst_idx].rule_id = rule_id;
                dst_rule[*dst_idx].status |= ENT_USED;
                (*dst_idx)++;
            }
        }

        if ((FLT_END == data) || (FLT_STARTEND == data))
        {
            rule_id++;
        }
    }

    *src_idx = i;
    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_alloc(a_uint32_t dev_id, isisc_acl_list_t * sw_list,
                     a_uint32_t filter_nr)
{
    a_uint32_t free_flt_nr, load_idx, begin_idx, start_idx, end_idx, i;
    a_uint32_t largest_nr, largest_idx;
    sw_error_t rv;

    /* calculate the proper location, [start_idx, end_idx) */
    start_idx = 0;
    end_idx = ISISC_MAX_FILTER;
    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        if (ENT_USED & sw_rule_ent[dev_id][i].status)
        {
            if (sw_rule_ent[dev_id][i].list_pri < sw_list->list_pri)
            {
                start_idx = i + 1;
            }
            else if (sw_rule_ent[dev_id][i].list_pri > sw_list->list_pri)
            {
                end_idx = i;
                break;
            }
        }
    }

    /* find the larget free filters block */
    largest_nr = 0;
    largest_idx = 0;
    free_flt_nr = 0;
    begin_idx = start_idx;
    for (i = start_idx; i < end_idx; i++)
    {
        if (!(ENT_USED & sw_rule_ent[dev_id][i].status))
        {
            free_flt_nr++;
        }
        else
        {
            if (free_flt_nr > largest_nr)
            {
                largest_nr = free_flt_nr;
                largest_idx = begin_idx;
            }
            free_flt_nr = 0;
            begin_idx = i + 1;
        }
    }

    if (free_flt_nr > largest_nr)
    {
        largest_nr = free_flt_nr;
        largest_idx = begin_idx;
    }

    if ((!largest_nr) || ((largest_nr + 1) < filter_nr))
    {
        return SW_NO_RESOURCE;
    }

    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        if (ENT_USED & sw_rule_ent[dev_id][i].status)
        {
            aos_mem_copy(&(sw_rule_tmp[dev_id][i]), &(sw_rule_ent[dev_id][i]),
                         sizeof (isisc_acl_rule_t));
        }
    }

    begin_idx = 0;
    load_idx = largest_idx;
    rv = _isisc_acl_list_insert(dev_id, &begin_idx, &load_idx,
                               hw_rule_tmp[dev_id], sw_rule_tmp[dev_id]);
    return rv;
}

static sw_error_t
_isisc_acl_rule_reorder(a_uint32_t dev_id, isisc_acl_list_t * sw_list)
{
    a_uint32_t i, src_idx, dst_idx;
    sw_error_t rv;

    dst_idx = 0;
    for (i = 0; i < ISISC_MAX_FILTER;)
    {
        if (ENT_USED & sw_rule_ent[dev_id][i].status)
        {
            if (sw_rule_ent[dev_id][i].list_pri <= sw_list->list_pri)
            {
                rv = _isisc_acl_list_insert(dev_id, &i, &dst_idx,
                                           sw_rule_ent[dev_id],
                                           sw_rule_tmp[dev_id]);
                SW_RTN_ON_ERROR(rv);
            }
            else
            {
                break;
            }
        }
        else
        {
            i++;
        }
    }

    src_idx = 0;
    rv = _isisc_acl_list_insert(dev_id, &src_idx, &dst_idx, hw_rule_tmp[dev_id],
                               sw_rule_tmp[dev_id]);
    SW_RTN_ON_ERROR(rv);

    for (; i < ISISC_MAX_FILTER;)
    {
        if (ENT_USED & sw_rule_ent[dev_id][i].status)
        {
            rv = _isisc_acl_list_insert(dev_id, &i, &dst_idx,
                                       sw_rule_ent[dev_id],
                                       sw_rule_tmp[dev_id]);
            SW_RTN_ON_ERROR(rv);
        }
        else
        {
            i++;
        }
    }

    return SW_OK;
}

static void
_isisc_acl_rule_sync(a_uint32_t dev_id, a_uint32_t flt_idx, a_uint32_t flt_nr)
{
    a_uint32_t i, data;

    for (i = flt_idx; i < (flt_idx + flt_nr); i++)
    {
        if (aos_mem_cmp
                (&(sw_rule_ent[dev_id][i]), &(sw_rule_tmp[dev_id][i]),
                 sizeof (isisc_acl_rule_t)))
        {
            SW_GET_FIELD_BY_REG(MAC_RUL_M4, RULE_TYP, data,
                                sw_rule_tmp[dev_id][i].filter.msk[4]);
            if (data)
            {
                _isisc_filter_down_to_hw(dev_id,
                                        &(sw_rule_tmp[dev_id][i].filter), i);
            }
            else
            {
                _isisc_filter_valid_set(dev_id, i, 0);
            }

            aos_mem_copy(&(sw_rule_ent[dev_id][i]), &(sw_rule_tmp[dev_id][i]),
                         sizeof (isisc_acl_rule_t));
            _isisc_acl_rule_src_filter_sts_set(dev_id, i,
                                              !sw_rule_tmp[dev_id][i].src_flt_dis);
        }
    }
}

static sw_error_t
_isisc_acl_list_creat(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t list_pri)
{
    a_uint32_t i, loc = ISISC_MAX_FILTER;
    isisc_acl_list_t *sw_list;

    HSL_DEV_ID_CHECK(dev_id);

    if ((ISISC_MAX_LIST_ID < list_id) || (ISISC_MAX_LIST_PRI < list_pri))
    {
        return SW_NOT_SUPPORTED;
    }

    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        sw_list = &(sw_list_ent[dev_id][i]);
        if (ENT_USED & sw_list->status)
        {
            if (list_id == sw_list->list_id)
            {
                return SW_ALREADY_EXIST;
            }
        }
        else
        {
            loc = i;
        }
    }

    if (ISISC_MAX_FILTER == loc)
    {
        return SW_NO_RESOURCE;
    }

    sw_list = &(sw_list_ent[dev_id][loc]);
    aos_mem_zero(sw_list, sizeof (isisc_acl_list_t));
    sw_list->list_id = list_id;
    sw_list->list_pri = list_pri;
    sw_list->status |= ENT_USED;
    return SW_OK;
}

static sw_error_t
_isisc_acl_list_destroy(a_uint32_t dev_id, a_uint32_t list_id)
{
    isisc_acl_list_t *sw_list;

    HSL_DEV_ID_CHECK(dev_id);

    if (ISISC_MAX_LIST_ID < list_id)
    {
        return SW_NOT_SUPPORTED;
    }

    sw_list = _isisc_acl_list_loc(dev_id, list_id);
    if (NULL == sw_list)
    {
        return SW_NOT_FOUND;
    }

    if (0 != sw_list->bind_pts)
    {
        return SW_NOT_SUPPORTED;
    }

    if (0 != sw_list->rule_nr)
    {
        return SW_NOT_SUPPORTED;
    }

    aos_mem_zero(sw_list, sizeof (isisc_acl_list_t));
    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_add(a_uint32_t dev_id, a_uint32_t list_id,
                   a_uint32_t rule_id, a_uint32_t rule_nr,
                   fal_acl_rule_t * rule)
{
    sw_error_t rv;
    isisc_acl_list_t *sw_list;
    isisc_acl_rule_t *sw_rule;
    a_uint32_t i, free_flt_nr, old_flt_nr, old_flt_idx, new_flt_nr, bind_pts;

    HSL_DEV_ID_CHECK(dev_id);

    if (ISISC_MAX_LIST_ID < list_id)
    {
        return SW_NOT_SUPPORTED;
    }

    if ((0 == rule_nr) || (NULL == rule))
    {
        return SW_BAD_PARAM;
    }

    sw_list = _isisc_acl_list_loc(dev_id, list_id);
    if (NULL == sw_list)
    {
        return SW_NOT_FOUND;
    }

    if (rule_id != sw_list->rule_nr)
    {
        return SW_BAD_PARAM;
    }

    old_flt_idx = 0;
    old_flt_nr = 0;
    free_flt_nr = 0;
    aos_mem_zero(hw_rule_tmp[dev_id],
                 ISISC_HW_RULE_TMP_CNT * sizeof (isisc_acl_rule_t));
    aos_mem_zero(sw_rule_tmp[dev_id],
                 ISISC_MAX_FILTER * sizeof (isisc_acl_rule_t));
    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        sw_rule = &(sw_rule_ent[dev_id][i]);
        if (ENT_USED & sw_rule->status)
        {
            if (sw_rule->list_id == sw_list->list_id)
            {
                aos_mem_copy(&(hw_rule_tmp[dev_id][old_flt_nr]), sw_rule,
                             sizeof (isisc_acl_rule_t));
                if (!old_flt_nr)
                {
                    old_flt_idx = i;
                }
                old_flt_nr++;
            }
        }
        else
        {
            free_flt_nr++;
        }
    }

    if (!free_flt_nr)
    {
        return SW_NO_RESOURCE;
    }

    /* parse rule entry and alloc rule resource */
    new_flt_nr = old_flt_nr;
    for (i = 0; i < rule_nr; i++)
    {
        rv = _isisc_acl_rule_sw_to_hw(dev_id, &rule[i], hw_rule_tmp[dev_id],
                                     &new_flt_nr);
        SW_RTN_ON_ERROR(rv);
    }

    if (free_flt_nr < (new_flt_nr - old_flt_nr))
    {
        return SW_NO_RESOURCE;
    }

    for (i = old_flt_nr; i < new_flt_nr; i++)
    {
        hw_rule_tmp[dev_id][i].status |= ENT_USED;
        hw_rule_tmp[dev_id][i].list_id = sw_list->list_id;
        hw_rule_tmp[dev_id][i].list_pri = sw_list->list_pri;
        bind_pts = sw_list->bind_pts;
        SW_SET_REG_BY_FIELD(MAC_RUL_V4, SRC_PT, bind_pts,
                            (hw_rule_tmp[dev_id][i].filter.vlu[4]));
    }

    for (i = 0; i < old_flt_nr; i++)
    {
        sw_rule = &(sw_rule_ent[dev_id][old_flt_idx + i]);
        sw_rule->status &= (~ENT_USED);
        sw_rule->status |= (ENT_TMP);
    }

    rv = _isisc_acl_rule_alloc(dev_id, sw_list, new_flt_nr);
    if (SW_OK != rv)
    {
        aos_mem_zero(sw_rule_tmp[dev_id],
                     ISISC_MAX_FILTER * sizeof (isisc_acl_rule_t));
        rv = _isisc_acl_rule_reorder(dev_id, sw_list);
    }

    for (i = 0; i < old_flt_nr; i++)
    {
        sw_rule = &(sw_rule_ent[dev_id][i + old_flt_idx]);
        sw_rule->status |= (ENT_USED);
        sw_rule->status &= (~ENT_TMP);
    }
    SW_RTN_ON_ERROR(rv);

    _isisc_acl_rule_sync(dev_id, 0, ISISC_MAX_FILTER);
    sw_list->rule_nr += rule_nr;

    _isisc_acl_sw_rule_dump("sw rule after add", sw_rule_ent[dev_id]);
    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_delete(a_uint32_t dev_id, a_uint32_t list_id,
                      a_uint32_t rule_id, a_uint32_t rule_nr)
{
    isisc_acl_rule_t *sw_rule;
    isisc_acl_list_t *sw_list;
    a_uint32_t i, flt_idx = 0, src_idx, dst_idx, del_nr = 0, flt_nr = 0;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (ISISC_MAX_LIST_ID < list_id)
    {
        return SW_NOT_SUPPORTED;
    }

    sw_list = _isisc_acl_list_loc(dev_id, list_id);
    if (NULL == sw_list)
    {
        return SW_NOT_FOUND;
    }

    if (sw_list->rule_nr < (rule_id + rule_nr))
    {
        return SW_BAD_PARAM;
    }

    aos_mem_zero(hw_rule_tmp[dev_id],
                 ISISC_HW_RULE_TMP_CNT * sizeof (isisc_acl_rule_t));
    aos_mem_zero(sw_rule_tmp[dev_id],
                 ISISC_MAX_FILTER * sizeof (isisc_acl_rule_t));

    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        sw_rule = &(sw_rule_ent[dev_id][i]);
        if ((ENT_USED & sw_rule->status) && (sw_rule->list_id == list_id))
        {
            if (!flt_nr)
            {
                flt_idx = i;
            }

            if ((sw_rule->rule_id >= rule_id)
                    && (sw_rule->rule_id < (rule_id + rule_nr)))
            {
                del_nr++;
            }
            else
            {
                aos_mem_copy(&(hw_rule_tmp[dev_id][flt_idx + flt_nr]), sw_rule,
                             sizeof (isisc_acl_rule_t));
            }
            flt_nr++;
        }
    }

    if (!del_nr)
    {
        return SW_NOT_FOUND;
    }

    _isisc_acl_sw_rule_dump("hw rule before del", hw_rule_tmp[dev_id]);

    for (i = 0; i < flt_nr; i++)
    {
        sw_rule = &(hw_rule_tmp[dev_id][flt_idx + i]);
        if (ENT_USED & sw_rule->status)
        {
            break;
        }
    }

    if (i != flt_nr)
    {
        src_idx = flt_idx + i;
        dst_idx = flt_idx;
        rv = _isisc_acl_list_insert(dev_id, &src_idx, &dst_idx,
                                   hw_rule_tmp[dev_id], sw_rule_tmp[dev_id]);
        SW_RTN_ON_ERROR(rv);
    }

    _isisc_acl_rule_sync(dev_id, flt_idx, flt_nr);
    sw_list->rule_nr -= rule_nr;

    _isisc_acl_sw_rule_dump("sw rule after del", sw_rule_ent[dev_id]);
    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_query(a_uint32_t dev_id, a_uint32_t list_id,
                     a_uint32_t rule_id, fal_acl_rule_t * rule)
{
    sw_error_t rv;
    isisc_acl_rule_t *sw_rule;
    a_uint32_t flt_nr = 0, i;

    HSL_DEV_ID_CHECK(dev_id);

    if (ISISC_MAX_LIST_ID < list_id)
    {
        return SW_NOT_SUPPORTED;
    }

    aos_mem_zero(hw_rule_tmp[dev_id],
                 ISISC_HW_RULE_TMP_CNT * sizeof (isisc_acl_rule_t));
    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        sw_rule = &(sw_rule_ent[dev_id][i]);
        if (ENT_USED & sw_rule->status)
        {
            if ((sw_rule->list_id == list_id) && (sw_rule->rule_id == rule_id))
            {
                aos_mem_copy(&(hw_rule_tmp[dev_id][flt_nr]), sw_rule,
                             sizeof (isisc_acl_rule_t));
                flt_nr++;
            }
        }
    }

    if (!flt_nr)
    {
        return SW_NOT_FOUND;
    }

    aos_mem_zero(rule, sizeof (fal_acl_rule_t));
    rv = _isisc_acl_rule_hw_to_sw(dev_id, rule, hw_rule_tmp[dev_id], 0, flt_nr);
    return rv;
}

static sw_error_t
_isisc_acl_rule_bind(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t ports)
{
    sw_error_t rv;
    a_uint32_t i;
    isisc_acl_rule_t *sw_rule;

    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        sw_rule = &(sw_rule_ent[dev_id][i]);

        if ((ENT_USED & sw_rule->status)
                && (list_id == sw_rule->list_id)
                && (!(ENT_DEACTIVE & sw_rule->status)))
        {
            rv = _isisc_filter_ports_bind(dev_id, i, ports);
            SW_RTN_ON_ERROR(rv);

            SW_SET_REG_BY_FIELD(MAC_RUL_V4, SRC_PT, ports,
                                (sw_rule->filter.vlu[4]));
        }
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_list_bind(a_uint32_t dev_id, a_uint32_t list_id,
                    fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                    a_uint32_t obj_idx)
{
    sw_error_t rv;
    a_uint32_t ports;
    isisc_acl_list_t *sw_list;

    HSL_DEV_ID_CHECK(dev_id);

    if (ISISC_MAX_LIST_ID < list_id)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_ACL_DIREC_IN != direc)
    {
        return SW_NOT_SUPPORTED;
    }

    sw_list = _isisc_acl_list_loc(dev_id, list_id);
    if (NULL == sw_list)
    {
        return SW_NOT_FOUND;
    }

    if (FAL_ACL_BIND_PORT == obj_t)
    {
        ports = (sw_list->bind_pts) | (0x1 << obj_idx);
    }
    else if (FAL_ACL_BIND_PORTBITMAP == obj_t)
    {
        ports = (sw_list->bind_pts) | obj_idx;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    rv = _isisc_acl_rule_bind(dev_id, list_id, ports);
    SW_RTN_ON_ERROR(rv);

    sw_list->bind_pts = ports;
    return SW_OK;
}

static sw_error_t
_isisc_acl_list_unbind(a_uint32_t dev_id, a_uint32_t list_id,
                      fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                      a_uint32_t obj_idx)
{
    sw_error_t rv;
    a_uint32_t ports;
    isisc_acl_list_t *sw_list;

    HSL_DEV_ID_CHECK(dev_id);

    if (ISISC_MAX_LIST_ID < list_id)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_ACL_DIREC_IN != direc)
    {
        return SW_NOT_SUPPORTED;
    }

    sw_list = _isisc_acl_list_loc(dev_id, list_id);
    if (NULL == sw_list)
    {
        return SW_NOT_FOUND;
    }

    if (FAL_ACL_BIND_PORT == obj_t)
    {
        ports = (sw_list->bind_pts) & (~(0x1UL << obj_idx));
    }
    else if (FAL_ACL_BIND_PORTBITMAP == obj_t)
    {
        ports = (sw_list->bind_pts) & (~obj_idx);
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    rv = _isisc_acl_rule_bind(dev_id, list_id, ports);
    SW_RTN_ON_ERROR(rv);

    sw_list->bind_pts = ports;
    return SW_OK;
}

static sw_error_t
_isisc_acl_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable)
    {
        val = 1;
    }
    else if (A_FALSE == enable)
    {
        val = 0;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, MOD_ENABLE, 0, ACL_EN, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isisc_acl_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, MOD_ENABLE, 0, ACL_EN, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (val)
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_port_udf_profile_set(a_uint32_t dev_id, fal_port_t port_id,
                               fal_acl_udf_type_t udf_type, a_uint32_t offset,
                               a_uint32_t length)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (ISISC_UDF_MAX_OFFSET < offset)
    {
        return SW_BAD_PARAM;
    }

    if (ISISC_UDF_MAX_OFFSET < length)
    {
        return SW_BAD_PARAM;
    }

    if ((FAL_ACL_UDF_TYPE_L2_SNAP == udf_type)
            || (FAL_ACL_UDF_TYPE_L3_PLUS == udf_type))
    {
        HSL_REG_ENTRY_GET(rv, dev_id, WIN_RULE_CTL1, port_id,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    }
    else
    {
        HSL_REG_ENTRY_GET(rv, dev_id, WIN_RULE_CTL0, port_id,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    }
    SW_RTN_ON_ERROR(rv);

    switch (udf_type)
    {
        case FAL_ACL_UDF_TYPE_L2:
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL0, L2_OFFSET, offset, reg);
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL0, L2_LENGTH, length, reg);
            break;
        case FAL_ACL_UDF_TYPE_L3:
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL0, L3_OFFSET, offset, reg);
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL0, L3_LENGTH, length, reg);
            break;
        case FAL_ACL_UDF_TYPE_L4:
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL0, L4_OFFSET, offset, reg);
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL0, L4_LENGTH, length, reg);
            break;
        case FAL_ACL_UDF_TYPE_L2_SNAP:
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL1, L2S_OFFSET, offset, reg);
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL1, L2S_LENGTH, length, reg);
            break;
        case FAL_ACL_UDF_TYPE_L3_PLUS:
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL1, L3P_OFFSET, offset, reg);
            SW_SET_REG_BY_FIELD(WIN_RULE_CTL1, L3P_LENGTH, length, reg);
            break;
        default:
            return SW_BAD_PARAM;
    }

    if ((FAL_ACL_UDF_TYPE_L2_SNAP == udf_type)
            || (FAL_ACL_UDF_TYPE_L3_PLUS == udf_type))
    {
        HSL_REG_ENTRY_SET(rv, dev_id, WIN_RULE_CTL1, port_id,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    }
    else
    {
        HSL_REG_ENTRY_SET(rv, dev_id, WIN_RULE_CTL0, port_id,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    }

    return rv;
}

static sw_error_t
_isisc_acl_port_udf_profile_get(a_uint32_t dev_id, fal_port_t port_id,
                               fal_acl_udf_type_t udf_type, a_uint32_t * offset,
                               a_uint32_t * length)
{
    sw_error_t rv;
    a_uint32_t reg = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if ((FAL_ACL_UDF_TYPE_L2_SNAP == udf_type)
            || (FAL_ACL_UDF_TYPE_L3_PLUS == udf_type))
    {
        HSL_REG_ENTRY_GET(rv, dev_id, WIN_RULE_CTL1, port_id,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    }
    else
    {
        HSL_REG_ENTRY_GET(rv, dev_id, WIN_RULE_CTL0, port_id,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    }
    SW_RTN_ON_ERROR(rv);

    switch (udf_type)
    {
        case FAL_ACL_UDF_TYPE_L2:
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL0, L2_OFFSET, (*offset), reg);
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL0, L2_LENGTH, (*length), reg);
            break;
        case FAL_ACL_UDF_TYPE_L3:
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL0, L3_OFFSET, (*offset), reg);
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL0, L3_LENGTH, (*length), reg);
            break;
        case FAL_ACL_UDF_TYPE_L4:
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL0, L4_OFFSET, (*offset), reg);
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL0, L4_LENGTH, (*length), reg);
            break;
        case FAL_ACL_UDF_TYPE_L2_SNAP:
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL1, L2S_OFFSET, (*offset), reg);
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL1, L2S_LENGTH, (*length), reg);
            break;
        case FAL_ACL_UDF_TYPE_L3_PLUS:
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL1, L3P_OFFSET, (*offset), reg);
            SW_GET_FIELD_BY_REG(WIN_RULE_CTL1, L3P_LENGTH, (*length), reg);
            break;
        default:
            return SW_BAD_PARAM;
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_active(a_uint32_t dev_id, a_uint32_t list_id,
                      a_uint32_t rule_id, a_uint32_t rule_nr, a_bool_t active)
{
    sw_error_t rv;
    a_uint32_t i, ports;
    isisc_acl_list_t *sw_list;
    isisc_acl_rule_t *sw_rule;

    HSL_DEV_ID_CHECK(dev_id);

    sw_list = _isisc_acl_list_loc(dev_id, list_id);
    if (NULL == sw_list)
    {
        return SW_NOT_FOUND;
    }

    if (sw_list->rule_nr < (rule_id + rule_nr))
    {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == active)
    {
        ports = (sw_list->bind_pts);
    }
    else
    {
        ports = 0;
    }

    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        sw_rule = &(sw_rule_ent[dev_id][i]);

        if ((ENT_USED & sw_rule->status)
                && (list_id == sw_rule->list_id)
                && (rule_id <= sw_rule->rule_id)
                && ((rule_id + rule_nr) > sw_rule->rule_id))
        {
            rv = _isisc_filter_ports_bind(dev_id, i, ports);
            SW_RTN_ON_ERROR(rv);

            SW_SET_REG_BY_FIELD(MAC_RUL_V4, SRC_PT, ports,
                                (sw_rule->filter.vlu[4]));

            if (A_TRUE == active)
            {
                sw_rule->status &= (~ENT_DEACTIVE);
            }
            else
            {
                sw_rule->status |= (ENT_DEACTIVE);
            }
        }
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_src_filter_sts_set(a_uint32_t dev_id,
                                  a_uint32_t rule_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t reg = 0, regIdx;
    isisc_acl_rule_t *sw_rule;

    HSL_DEV_ID_CHECK(dev_id);

    sw_rule = &sw_rule_ent[dev_id][rule_id];
    if (!(ENT_USED & sw_rule->status))
    {
        return SW_NOT_FOUND;
    }

    regIdx = rule_id >> 5;

    HSL_REG_ENTRY_GET(rv, dev_id, ACL_FWD_SRC_FILTER_CTL0, regIdx,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable)
    {
        reg |= (0x1 << (rule_id & 0x1F));
        sw_rule->src_flt_dis = 0;
    }
    else if (A_FALSE == enable)
    {
        reg &= ~(0x1 << (rule_id & 0x1F));
        sw_rule->src_flt_dis = 1;
    }
    else
    {
        return SW_BAD_PARAM;
    }

    HSL_REG_ENTRY_SET(rv, dev_id, ACL_FWD_SRC_FILTER_CTL0, regIdx,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));

    return rv;
}

static sw_error_t
_isisc_acl_rule_src_filter_sts_get(a_uint32_t dev_id,
                                  a_uint32_t rule_id, a_bool_t* enable)
{
    sw_error_t rv;
    a_uint32_t reg = 0, regIdx;
    isisc_acl_rule_t *sw_rule;

    HSL_DEV_ID_CHECK(dev_id);

    sw_rule = &sw_rule_ent[dev_id][rule_id];
    if (!(ENT_USED & sw_rule->status))
    {
        return SW_NOT_FOUND;
    }

    regIdx = rule_id >> 5;

    HSL_REG_ENTRY_GET(rv, dev_id, ACL_FWD_SRC_FILTER_CTL0, regIdx,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (reg & (0x1 << (rule_id & 0x1F)))
    {
        *enable = A_TRUE;
    }
    else
    {
        *enable = A_FALSE;
    }

    return rv;
}


HSL_LOCAL sw_error_t
isisc_acl_list_dump(a_uint32_t dev_id)
{
    _isisc_acl_list_dump(dev_id);
    return SW_OK;
}

HSL_LOCAL sw_error_t
isisc_acl_rule_dump(a_uint32_t dev_id)
{
    a_uint32_t flt_idx, i;
    sw_error_t rv;
    hw_filter_t filter;

    aos_printk("\nisisc_acl_rule_dump:\n");

    for (flt_idx = 0; flt_idx < ISISC_MAX_FILTER; flt_idx++)
    {
        aos_mem_zero(&filter, sizeof (hw_filter_t));

        rv = _isisc_filter_up_to_sw(dev_id, &filter, flt_idx);
        if (SW_OK != rv)
        {
            continue;
        }

        aos_printk("\n%d filter dump:", flt_idx);

        aos_printk("\nhardware content:");
        aos_printk("\nact:");
        for (i = 0; i < (sizeof (filter.act) / sizeof (a_uint32_t)); i++)
        {
            aos_printk("%08x  ", filter.act[i]);
        }

        aos_printk("\nvlu:");
        for (i = 0; i < (sizeof (filter.vlu) / sizeof (a_uint32_t)); i++)
        {
            aos_printk("%08x  ", filter.vlu[i]);
        }

        aos_printk("\nmsk:");
        for (i = 0; i < (sizeof (filter.msk) / sizeof (a_uint32_t)); i++)
        {
            aos_printk("%08x  ", filter.msk[i]);
        }

        aos_printk("\nsoftware content:");
        aos_printk("\nact:");
        for (i = 0; i < (sizeof (filter.act) / sizeof (a_uint32_t)); i++)
        {
            aos_printk("%08x  ", sw_rule_ent[dev_id][flt_idx].filter.act[i]);
        }

        aos_printk("\nvlu:");
        for (i = 0; i < (sizeof (filter.vlu) / sizeof (a_uint32_t)); i++)
        {
            aos_printk("%08x  ", sw_rule_ent[dev_id][flt_idx].filter.vlu[i]);
        }

        aos_printk("\nmsk:");
        for (i = 0; i < (sizeof (filter.msk) / sizeof (a_uint32_t)); i++)
        {
            aos_printk("%08x  ", sw_rule_ent[dev_id][flt_idx].filter.msk[i]);
        }

        aos_printk("\nctl:status[%02d] list_id[%02d] rule_id[%02d] src_flt_dis[%02d]",
                   sw_rule_ent[dev_id][flt_idx].status,
                   sw_rule_ent[dev_id][flt_idx].list_id,
                   sw_rule_ent[dev_id][flt_idx].rule_id,
                   sw_rule_ent[dev_id][flt_idx].src_flt_dis);

        aos_printk("\n\n");
    }

    return SW_OK;
}

sw_error_t
isisc_acl_reset(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

    aos_mem_zero(sw_list_ent[dev_id],
                 ISISC_MAX_FILTER * sizeof (isisc_acl_list_t));

    aos_mem_zero(sw_rule_ent[dev_id],
                 ISISC_MAX_FILTER * sizeof (isisc_acl_rule_t));

    return SW_OK;
}

/**
 * @brief Creat an acl list
 * @details  Comments:
  *     If the value of list_pri is more small then the priority is more high,
 *     that means the list could be first matched.
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @param[in] list_pri acl list priority
 * @return SW_OK or error code
 */
sw_error_t
isisc_acl_list_creat(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t list_pri)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_list_creat(dev_id, list_id, list_pri);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Destroy an acl list
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_acl_list_destroy(a_uint32_t dev_id, a_uint32_t list_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_list_destroy(dev_id, list_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add one rule or more rules to an existing acl list
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @param[in] rule_id first rule id of this adding operation in list
 * @param[in] rule_nr rule number of this adding operation
 * @param[in] rule rules content of this adding operation
 * @return SW_OK or error code
 */
sw_error_t
isisc_acl_rule_add(a_uint32_t dev_id, a_uint32_t list_id,
                  a_uint32_t rule_id, a_uint32_t rule_nr, fal_acl_rule_t * rule)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_rule_add(dev_id, list_id, rule_id, rule_nr, rule);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete one rule or more rules from an existing acl list
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @param[in] rule_id first rule id of this deleteing operation in list
 * @param[in] rule_nr rule number of this deleteing operation
 * @return SW_OK or error code
 */
sw_error_t
isisc_acl_rule_delete(a_uint32_t dev_id, a_uint32_t list_id,
                     a_uint32_t rule_id, a_uint32_t rule_nr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_rule_delete(dev_id, list_id, rule_id, rule_nr);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Query one particular rule in a particular acl list
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @param[in] rule_id first rule id of this deleteing operation in list
 * @param[out] rule rule content of this operation
 * @return SW_OK or error code
 */
sw_error_t
isisc_acl_rule_query(a_uint32_t dev_id, a_uint32_t list_id,
                    a_uint32_t rule_id, fal_acl_rule_t * rule)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_rule_query(dev_id, list_id, rule_id, rule);
    HSL_API_UNLOCK;
    return rv;
}

a_uint32_t
isisc_acl_rule_get_offset(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id)
{
    a_uint32_t i, pos=0;
    isisc_acl_rule_t *sw_rule;

    for (i = 0; i < ISISC_MAX_FILTER; i++)
    {
        sw_rule = &(sw_rule_ent[0][i]);

        if ((ENT_USED & sw_rule->status)
                && (list_id == sw_rule->list_id) && (sw_rule->rule_id == rule_id)
                && (!(ENT_DEACTIVE & sw_rule->status)))
        {
            pos = i;
            break;

        }
    }

    return pos;
}


sw_error_t
isisc_acl_rule_sync_multi_portmap(a_uint32_t dev_id, a_uint32_t pos, a_uint32_t *act)
{

    HSL_DEV_ID_CHECK(dev_id);

    if (ISISC_MAX_LIST_ID < pos)
    {
        return SW_NOT_SUPPORTED;
    }

    sw_rule_ent[dev_id][pos].filter.act[1] = act[1];
    sw_rule_ent[dev_id][pos].filter.act[2] = act[2];

    sw_rule_tmp[dev_id][pos].filter.act[1] = act[1];
    sw_rule_tmp[dev_id][pos].filter.act[2] = act[2];

    hw_rule_tmp[dev_id][pos].filter.act[1] = act[1];
    hw_rule_tmp[dev_id][pos].filter.act[2] = act[2];


    return SW_OK;
}

/**
 * @brief Bind an acl list to a particular object
 * @details  Comments:
 *    If obj_t equals FAL_ACL_BIND_PORT then obj_idx means port id
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @param[in] direc direction of this binding operation
 * @param[in] obj_t object type of this binding operation
 * @param[in] obj_idx object index of this binding operation
 * @return SW_OK or error code
 */
sw_error_t
isisc_acl_list_bind(a_uint32_t dev_id, a_uint32_t list_id,
                   fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                   a_uint32_t obj_idx)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_list_bind(dev_id, list_id, direc, obj_t, obj_idx);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Unbind an acl list from a particular object
 * @details  Comments:
  *    If obj_t equals FAL_ACL_BIND_PORT then obj_idx means port id
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @param[in] direc direction of this unbinding operation
 * @param[in] obj_t object type of this unbinding operation
 * @param[in] obj_idx object index of this unbinding operation
 * @return SW_OK or error code
 */
sw_error_t
isisc_acl_list_unbind(a_uint32_t dev_id, a_uint32_t list_id,
                     fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                     a_uint32_t obj_idx)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_list_unbind(dev_id, list_id, direc, obj_t, obj_idx);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working status of ACL engine on a particular device
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
sw_error_t
isisc_acl_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get working status of ACL engine on a particular device
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_acl_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set user define fields profile on a particular port
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] udf_type udf type
 * @param[in] offset udf offset
 * @param[in] length udf length
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_acl_port_udf_profile_set(a_uint32_t dev_id, fal_port_t port_id,
                              fal_acl_udf_type_t udf_type, a_uint32_t offset,
                              a_uint32_t length)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_port_udf_profile_set(dev_id, port_id, udf_type, offset,
                                        length);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get user define fields profile on a particular port
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] udf_type udf type
 * @param[out] offset udf offset
 * @param[out] length udf length
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_acl_port_udf_profile_get(a_uint32_t dev_id, fal_port_t port_id,
                              fal_acl_udf_type_t udf_type, a_uint32_t * offset,
                              a_uint32_t * length)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_port_udf_profile_get(dev_id, port_id, udf_type, offset,
                                        length);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Active one or more rules in an existing acl list
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @param[in] rule_id first rule id of this deactive operation in list
 * @param[in] rule_nr rule number of this deactive operation
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_acl_rule_active(a_uint32_t dev_id, a_uint32_t list_id,
                     a_uint32_t rule_id, a_uint32_t rule_nr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_rule_active(dev_id, list_id, rule_id, rule_nr, A_TRUE);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Deactive one or more rules in an existing acl list
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @param[in] rule_id first rule id of this deactive operation in list
 * @param[in] rule_nr rule number of this deactive operation
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_acl_rule_deactive(a_uint32_t dev_id, a_uint32_t list_id,
                       a_uint32_t rule_id, a_uint32_t rule_nr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_rule_active(dev_id, list_id, rule_id, rule_nr, A_FALSE);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief enable acl forward source filter of one rule.
 * @param[in] dev_id device id
 * @param[in] rule_id first rule id of this deactive operation in list
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_acl_rule_src_filter_sts_set(a_uint32_t dev_id,
                                 a_uint32_t rule_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_rule_src_filter_sts_set(dev_id, rule_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief get the status of acl forward source filter of one rule.
 * @param[in] dev_id device id
 * @param[in] rule_id first rule id of this deactive operation in list
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isisc_acl_rule_src_filter_sts_get(a_uint32_t dev_id,
                                 a_uint32_t rule_id, a_bool_t* enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isisc_acl_rule_src_filter_sts_get(dev_id, rule_id, enable);
    HSL_API_UNLOCK;
    return rv;
}



sw_error_t
isisc_acl_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

    sw_list_ent[dev_id] =
        (isisc_acl_list_t *) aos_mem_alloc(ISISC_MAX_FILTER *
                                          sizeof (isisc_acl_list_t));
    if (NULL == sw_list_ent[dev_id])
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(sw_list_ent[dev_id],
                 ISISC_MAX_FILTER * sizeof (isisc_acl_list_t));

    sw_rule_ent[dev_id] =
        (isisc_acl_rule_t *) aos_mem_alloc(ISISC_MAX_FILTER *
                                          sizeof (isisc_acl_rule_t));
    if (NULL == sw_rule_ent[dev_id])
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(sw_rule_ent[dev_id],
                 ISISC_MAX_FILTER * sizeof (isisc_acl_rule_t));

    hw_rule_tmp[dev_id] =
        (isisc_acl_rule_t *) aos_mem_alloc(ISISC_HW_RULE_TMP_CNT *
                                          sizeof (isisc_acl_rule_t));
    if (NULL == hw_rule_tmp[dev_id])
    {
        return SW_NO_RESOURCE;
    }

    sw_rule_tmp[dev_id] =
        (isisc_acl_rule_t *) aos_mem_alloc(ISISC_MAX_FILTER *
                                          sizeof (isisc_acl_rule_t));
    if (NULL == sw_rule_tmp[dev_id])
    {
        return SW_NO_RESOURCE;
    }
#ifdef ISISC_SW_ENTRY
    sw_filter_mem = aos_mem_alloc(ISISC_MAX_FILTER * sizeof (hw_filter_t));
    if (NULL == sw_filter_mem)
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(sw_filter_mem, ISISC_MAX_FILTER * sizeof (hw_filter_t));
#endif

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->acl_list_creat = isisc_acl_list_creat;
        p_api->acl_list_destroy = isisc_acl_list_destroy;
        p_api->acl_list_bind = isisc_acl_list_bind;
        p_api->acl_list_unbind = isisc_acl_list_unbind;
        p_api->acl_rule_add = isisc_acl_rule_add;
        p_api->acl_rule_delete = isisc_acl_rule_delete;
        p_api->acl_rule_query = isisc_acl_rule_query;
        p_api->acl_status_set = isisc_acl_status_set;
        p_api->acl_status_get = isisc_acl_status_get;
        p_api->acl_list_dump = isisc_acl_list_dump;
        p_api->acl_rule_dump = isisc_acl_rule_dump;
        p_api->acl_port_udf_profile_set = isisc_acl_port_udf_profile_set;
        p_api->acl_port_udf_profile_get = isisc_acl_port_udf_profile_get;
        p_api->acl_rule_active = isisc_acl_rule_active;
        p_api->acl_rule_deactive = isisc_acl_rule_deactive;
        p_api->acl_rule_src_filter_sts_set = isisc_acl_rule_src_filter_sts_set;
        p_api->acl_rule_src_filter_sts_get = isisc_acl_rule_src_filter_sts_get;
        p_api->acl_rule_get_offset = isisc_acl_rule_get_offset;
        p_api->acl_rule_sync_multi_portmap = isisc_acl_rule_sync_multi_portmap;
    }
#endif

    return SW_OK;
}

sw_error_t
isisc_acl_cleanup(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (NULL != sw_list_ent[dev_id])
    {
        aos_mem_free(sw_list_ent[dev_id]);
    }

    if (NULL != sw_rule_ent[dev_id])
    {
        aos_mem_free(sw_rule_ent[dev_id]);
    }

    if (NULL != hw_rule_tmp[dev_id])
    {
        aos_mem_free(hw_rule_tmp[dev_id]);
    }

    if (NULL != sw_rule_tmp[dev_id])
    {
        aos_mem_free(sw_rule_tmp[dev_id]);
    }
#ifdef DESS_SW_ENTRY
    if (NULL != sw_filter_mem)
    {
        aos_mem_free(sw_filter_mem);
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

