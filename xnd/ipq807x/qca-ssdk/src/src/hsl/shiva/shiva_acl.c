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


/**
 * @defgroup shiva_acl SHIVA_ACL
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_acl.h"
#include "shiva_acl.h"
#include "shiva_reg.h"

//#define SHIVA_ACL_DEBUG
//#define SHIVA_SW_ENTRY
//#define SHIVA_ENTRY_DUMP

typedef struct
{
    a_uint32_t list_id;
    a_uint32_t list_pri;
    a_uint32_t addr;
    a_uint32_t size;
    a_uint32_t status;
    fal_pbmp_t bind_pts;
} shiva_acl_list_t;

typedef struct
{
    a_uint32_t slct[6];
    a_uint32_t vlu[5];
    a_uint32_t msk[5];
    a_uint32_t typ;
    a_uint32_t len;
    a_uint32_t act[3];
} shiva_acl_hw_rule_t;

static shiva_acl_list_t *list_ent[SW_MAX_NR_DEV];
static shiva_acl_hw_rule_t *hw_rule_ent;

static a_uint32_t filter[SW_MAX_NR_DEV];
static a_uint32_t filter_snap[SW_MAX_NR_DEV];

#define SHIVA_MAX_LIST 32
#define SHIVA_MAX_RULE 32

#define ENT_FREE        0x1
#define ENT_USED        0x2

#define SHIVA_RULE_VLU_ADDR  0x58400
#define SHIVA_RULE_MSK_ADDR  0x58c00
#define SHIVA_RULE_LEN_ADDR  0x58818
#define SHIVA_RULE_TYP_ADDR  0x5881c
#define SHIVA_RULE_ACT_ADDR  0x58000
#define SHIVA_RULE_SLCT_ADDR 0x58800

#define SHIVA_MAC_FILTER       1
#define SHIVA_IP4_FILTER       2
#define SHIVA_IP6R1_FILTER     3
#define SHIVA_IP6R2_FILTER     4
#define SHIVA_IP6R3_FILTER     5
#define SHIVA_UDF_FILTER       6

#ifdef SHIVA_SW_ENTRY
static char *flt_vlu_mem = NULL;
static char *flt_msk_mem = NULL;
static char *flt_typ_mem = NULL;
static char *flt_len_mem = NULL;
static char *act_mem = NULL;
static char *slct_mem = NULL;
#endif

static a_bool_t _shiva_acl_zero_addr(const fal_mac_addr_t addr);

static a_bool_t
_shiva_acl_field_care(fal_acl_field_op_t op, a_uint32_t val, a_uint32_t mask,
                      a_uint32_t chkvlu);

static sw_error_t
_shiva_acl_list_loc(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t * idx);

static sw_error_t
_shiva_acl_filter_map_get(const shiva_acl_hw_rule_t * rule,
                          a_uint32_t flt_idx[], a_uint32_t * flt_nr);

static sw_error_t
_shiva_acl_rule_mac_parse(fal_acl_rule_t * sw,
                          shiva_acl_hw_rule_t * hw, a_bool_t * b_care);

static sw_error_t
_shiva_acl_rule_ip4_parse(fal_acl_rule_t * sw,
                          shiva_acl_hw_rule_t * hw, a_bool_t * b_care);

static sw_error_t
_shiva_acl_rule_ip6r1_parse(fal_acl_rule_t * sw,
                            shiva_acl_hw_rule_t * hw, a_bool_t * b_care);

static sw_error_t
_shiva_acl_rule_ip6r2_parse(fal_acl_rule_t * sw,
                            shiva_acl_hw_rule_t * hw, a_bool_t * b_care);

static sw_error_t
_shiva_acl_rule_ip6r3_parse(fal_acl_rule_t * sw,
                            shiva_acl_hw_rule_t * hw, a_bool_t * b_care);

static sw_error_t
_shiva_acl_rule_udf_parse(fal_acl_rule_t * sw,
                          shiva_acl_hw_rule_t * hw, a_bool_t * b_care);

static sw_error_t
_shiva_acl_action_parse(a_uint32_t dev_id, const fal_acl_rule_t * sw,
                        shiva_acl_hw_rule_t * hw);

static sw_error_t
_shiva_acl_rule_mac_reparse(fal_acl_rule_t * sw,
                            const shiva_acl_hw_rule_t * hw);

static sw_error_t
_shiva_acl_rule_ip4_reparse(fal_acl_rule_t * sw,
                            const shiva_acl_hw_rule_t * hw);

static sw_error_t
_shiva_acl_rule_ip6r1_reparse(fal_acl_rule_t * sw,
                              const shiva_acl_hw_rule_t * hw);

static sw_error_t
_shiva_acl_rule_ip6r2_reparse(fal_acl_rule_t * sw,
                              const shiva_acl_hw_rule_t * hw);

static sw_error_t
_shiva_acl_rule_ip6r3_reparse(fal_acl_rule_t * sw,
                              const shiva_acl_hw_rule_t * hw);

static sw_error_t
_shiva_acl_rule_action_reparse(fal_acl_rule_t * sw,
                               const shiva_acl_hw_rule_t * hw);

static sw_error_t
_shiva_acl_filter_alloc(a_uint32_t dev_id, a_uint32_t * idx);

static void
_shiva_acl_filter_free(a_uint32_t dev_id, a_uint32_t idx);

static void
_shiva_acl_filter_snap(a_uint32_t dev_id);

static void
_shiva_acl_filter_commit(a_uint32_t dev_id);

static sw_error_t
_shiva_acl_slct_update(shiva_acl_hw_rule_t * hw, a_uint32_t offset,
                       a_uint32_t flt_idx);

static sw_error_t
_shiva_acl_filter_write(a_uint32_t dev_id, const shiva_acl_hw_rule_t * rule,
                        a_uint32_t flt_idx);

static sw_error_t
_shiva_acl_action_write(a_uint32_t dev_id, const shiva_acl_hw_rule_t * rule,
                        a_uint32_t act_idx);

static sw_error_t
_shiva_acl_slct_write(a_uint32_t dev_id, const shiva_acl_hw_rule_t * rule,
                      a_uint32_t slct_idx);

static sw_error_t
_shiva_acl_filter_read(a_uint32_t dev_id, shiva_acl_hw_rule_t * rule,
                       a_uint32_t flt_idx);

static sw_error_t
_shiva_acl_action_read(a_uint32_t dev_id, shiva_acl_hw_rule_t * rule,
                       a_uint32_t act_idx);

static sw_error_t
_shiva_acl_slct_read(a_uint32_t dev_id, shiva_acl_hw_rule_t * rule,
                     a_uint32_t slct_idx);

static sw_error_t
_shiva_acl_rule_set(a_uint32_t dev_id, a_uint32_t base_addr,
                    const shiva_acl_hw_rule_t * hw_rule_ent,
                    a_uint32_t rule_nr);

static sw_error_t
_shiva_acl_rule_get(a_uint32_t dev_id, shiva_acl_hw_rule_t * rule,
                    a_uint32_t * ent_idx, a_uint32_t rule_idx);

static sw_error_t
_shiva_acl_rule_sw_to_hw(a_uint32_t dev_id, fal_acl_rule_t * sw,
                         shiva_acl_hw_rule_t * hw, a_uint32_t * idx);

static sw_error_t
_shiva_acl_rule_hw_to_sw(fal_acl_rule_t * sw, const shiva_acl_hw_rule_t * hw,
                         a_uint32_t ent_idx, a_uint32_t ent_nr);

static sw_error_t
_shiva_acl_rule_copy(a_uint32_t dev_id, a_uint32_t src_slct_idx,
                     a_uint32_t dst_slct_idx, a_uint32_t size);

static sw_error_t
_shiva_acl_rule_invalid(a_uint32_t dev_id, a_uint32_t rule_idx,
                        a_uint32_t size);

static sw_error_t
_shiva_acl_rule_valid(a_uint32_t dev_id, a_uint32_t rule_idx, a_uint32_t size,
                      a_uint32_t flag);

static sw_error_t
_shiva_acl_addr_update(a_uint32_t dev_id, a_uint32_t old_addr,
                       a_uint32_t new_addr, a_uint32_t list_id);

static sw_error_t
_shiva_acl_rule_bind(a_uint32_t dev_id, a_uint32_t rule_idx, a_uint32_t ports);

#ifdef SHIVA_ACL_DEBUG
static void
_shiva_acl_list_dump(a_uint32_t dev_id)
{
    a_uint32_t i;

    aos_printk("\ndev_id=%d  list control infomation", dev_id);
    for (i = 0; i < SHIVA_MAX_LIST; i++)
    {
        if (ENT_USED == list_ent[dev_id][i].status)
        {
            aos_printk("\nlist_id=%d  list_pri=%d  addr=%d  size=%d  idx=%d ",
                       list_ent[dev_id][i].list_id,
                       list_ent[dev_id][i].list_pri,
                       list_ent[dev_id][i].addr, list_ent[dev_id][i].size, i);
        }
    }
    aos_printk("\n");
}
#else
#define _shiva_acl_list_dump(dev_id)
#endif

static a_bool_t
_shiva_acl_zero_addr(const fal_mac_addr_t addr)
{
    a_uint32_t i;

    for (i = 0; i < 6; i++)
    {
        if (addr.uc[i])
        {
            return A_FALSE;
        }
    }
    return A_TRUE;
}

static a_bool_t
_shiva_acl_field_care(fal_acl_field_op_t op, a_uint32_t val, a_uint32_t mask,
                      a_uint32_t chkvlu)
{
    if (FAL_ACL_FIELD_MASK == op)
    {
        if (0 == mask)
            return A_FALSE;
    }
    else if (FAL_ACL_FIELD_RANGE == op)
    {
        if ((0 == val) && (chkvlu == mask))
            return A_FALSE;
    }
    else if (FAL_ACL_FIELD_LE == op)
    {
        if (chkvlu == val)
            return A_FALSE;
    }
    else if (FAL_ACL_FIELD_GE == op)
    {
        if (0 == val)
            return A_FALSE;
    }
    else if (FAL_ACL_FIELD_NE == op)
    {
        return A_TRUE;
    }

    return A_TRUE;
}

static sw_error_t
_shiva_acl_list_loc(a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t * idx)
{
    a_uint32_t i;

    for (i = 0; i < SHIVA_MAX_LIST; i++)
    {
        if ((ENT_USED == list_ent[dev_id][i].status)
                && (list_id == list_ent[dev_id][i].list_id))
        {
            *idx = i;
            return SW_OK;
        }
    }
    return SW_NOT_FOUND;
}

static sw_error_t
_shiva_acl_filter_map_get(const shiva_acl_hw_rule_t * rule,
                          a_uint32_t flt_idx[], a_uint32_t * flt_nr)
{
    a_uint32_t flt_en, idx, i = 0;

    SW_GET_FIELD_BY_REG(RUL_SLCT0, ADDR0_EN, flt_en, (rule->slct[0]));
    if (flt_en)
    {
        SW_GET_FIELD_BY_REG(RUL_SLCT1, ADDR0, idx, (rule->slct[1]));
        flt_idx[i] = idx;
        i++;
    }

    SW_GET_FIELD_BY_REG(RUL_SLCT0, ADDR1_EN, flt_en, (rule->slct[0]));
    if (flt_en)
    {
        SW_GET_FIELD_BY_REG(RUL_SLCT2, ADDR1, idx, (rule->slct[2]));
        flt_idx[i] = idx;
        i++;
    }

    SW_GET_FIELD_BY_REG(RUL_SLCT0, ADDR2_EN, flt_en, (rule->slct[0]));
    if (flt_en)
    {
        SW_GET_FIELD_BY_REG(RUL_SLCT3, ADDR2, idx, (rule->slct[3]));
        flt_idx[i] = idx;
        i++;
    }

    SW_GET_FIELD_BY_REG(RUL_SLCT0, ADDR3_EN, flt_en, (rule->slct[0]));
    if (flt_en)
    {
        SW_GET_FIELD_BY_REG(RUL_SLCT4, ADDR3, idx, (rule->slct[4]));
        flt_idx[i] = idx;
        i++;
    }

    *flt_nr = i;
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_mac_parse(fal_acl_rule_t * sw,
                          shiva_acl_hw_rule_t * hw, a_bool_t * b_care)
{
    a_uint32_t i, len = 14;

    *b_care = A_FALSE;

    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));
    aos_mem_zero(&(hw->typ), sizeof (hw->typ));
    aos_mem_zero(&(hw->len), sizeof (hw->len));

    SW_SET_REG_BY_FIELD(RUL_SLCT7, RULE_TYP, SHIVA_MAC_FILTER, hw->typ);

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_DA))
    {
        if (A_TRUE != _shiva_acl_zero_addr(sw->dest_mac_mask))
        {
            *b_care = A_TRUE;
        }

        for (i = 0; i < 6; i++)
        {
            sw->dest_mac_val.uc[i] &= sw->dest_mac_mask.uc[i];
        }

        SW_SET_REG_BY_FIELD(MAC_RUL_V0, DAV_BYTE2, sw->dest_mac_val.uc[2],
                            hw->vlu[0]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V0, DAV_BYTE3, sw->dest_mac_val.uc[3],
                            hw->vlu[0]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V0, DAV_BYTE4, sw->dest_mac_val.uc[4],
                            hw->vlu[0]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V0, DAV_BYTE5, sw->dest_mac_val.uc[5],
                            hw->vlu[0]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V1, DAV_BYTE0, sw->dest_mac_val.uc[0],
                            hw->vlu[1]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V1, DAV_BYTE1, sw->dest_mac_val.uc[1],
                            hw->vlu[1]);

        SW_SET_REG_BY_FIELD(MAC_RUL_M0, DAM_BYTE2, sw->dest_mac_mask.uc[2],
                            hw->msk[0]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M0, DAM_BYTE3, sw->dest_mac_mask.uc[3],
                            hw->msk[0]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M0, DAM_BYTE4, sw->dest_mac_mask.uc[4],
                            hw->msk[0]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M0, DAM_BYTE5, sw->dest_mac_mask.uc[5],
                            hw->msk[0]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M1, DAM_BYTE0, sw->dest_mac_mask.uc[0],
                            hw->msk[1]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M1, DAM_BYTE1, sw->dest_mac_mask.uc[1],
                            hw->msk[1]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_SA))
    {
        if (A_TRUE != _shiva_acl_zero_addr(sw->src_mac_mask))
        {
            *b_care = A_TRUE;
        }

        for (i = 0; i < 6; i++)
        {
            sw->src_mac_val.uc[i] &= sw->src_mac_mask.uc[i];
        }

        SW_SET_REG_BY_FIELD(MAC_RUL_V1, SAV_BYTE4, sw->src_mac_val.uc[4],
                            hw->vlu[1]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V1, SAV_BYTE5, sw->src_mac_val.uc[5],
                            hw->vlu[1]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V2, SAV_BYTE0, sw->src_mac_val.uc[0],
                            hw->vlu[2]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V2, SAV_BYTE1, sw->src_mac_val.uc[1],
                            hw->vlu[2]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V2, SAV_BYTE2, sw->src_mac_val.uc[2],
                            hw->vlu[2]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V2, SAV_BYTE3, sw->src_mac_val.uc[3],
                            hw->vlu[2]);

        SW_SET_REG_BY_FIELD(MAC_RUL_M1, SAM_BYTE4, sw->src_mac_mask.uc[4],
                            hw->msk[1]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M1, SAM_BYTE5, sw->src_mac_mask.uc[5],
                            hw->msk[1]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M2, SAM_BYTE0, sw->src_mac_mask.uc[0],
                            hw->msk[2]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M2, SAM_BYTE1, sw->src_mac_mask.uc[1],
                            hw->msk[2]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M2, SAM_BYTE2, sw->src_mac_mask.uc[2],
                            hw->msk[2]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M2, SAM_BYTE3, sw->src_mac_mask.uc[3],
                            hw->msk[2]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_ETHTYPE))
    {
        if (0x0 != sw->ethtype_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ethtype_val &= sw->ethtype_mask;
        SW_SET_REG_BY_FIELD(MAC_RUL_V3, ETHTYPV, sw->ethtype_val, hw->vlu[3]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M3, ETHTYPM, sw->ethtype_mask, hw->msk[3]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_TAGGED))
    {
        if (0x0 != sw->tagged_mask)
        {
            *b_care = A_TRUE;
        }

        sw->tagged_val &= sw->tagged_mask;
        SW_SET_REG_BY_FIELD(MAC_RUL_V4, TAGGEDV, sw->tagged_val, hw->vlu[4]);
        SW_SET_REG_BY_FIELD(MAC_RUL_V4, TAGGEDM, sw->tagged_mask, hw->vlu[4]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_UP))
    {
        if (0x0 != sw->up_mask)
        {
            *b_care = A_TRUE;
        }

        sw->up_val &= sw->up_mask;
        SW_SET_REG_BY_FIELD(MAC_RUL_V3, VLANPRIV, sw->up_val, hw->vlu[3]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M3, VLANPRIM, sw->up_mask, hw->msk[3]);
    }

    SW_SET_REG_BY_FIELD(MAC_RUL_V4, VIDMSK, 1, hw->vlu[4]);
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_VID))
    {
        if ((FAL_ACL_FIELD_MASK != sw->vid_op)
                && (FAL_ACL_FIELD_RANGE != sw->vid_op)
                && (FAL_ACL_FIELD_LE != sw->vid_op)
                && (FAL_ACL_FIELD_GE != sw->vid_op))
        {
            return SW_NOT_SUPPORTED;
        }

        if (A_TRUE ==
                _shiva_acl_field_care(sw->vid_op, sw->vid_val, sw->vid_mask,
                                      0xfff))
        {
            *b_care = A_TRUE;
        }

        SW_SET_REG_BY_FIELD(MAC_RUL_V4, VIDMSK, 0, hw->vlu[4]);
        if (FAL_ACL_FIELD_MASK == sw->vid_op)
        {
            sw->vid_val &= sw->vid_mask;
            SW_SET_REG_BY_FIELD(MAC_RUL_V3, VLANIDV, sw->vid_val, hw->vlu[3]);
            SW_SET_REG_BY_FIELD(MAC_RUL_M3, VLANIDM, sw->vid_mask, hw->msk[3]);
            SW_SET_REG_BY_FIELD(MAC_RUL_V4, VIDMSK, 1, hw->vlu[4]);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->vid_op)
        {
            SW_SET_REG_BY_FIELD(MAC_RUL_V3, VLANIDV, sw->vid_val, hw->vlu[3]);
            SW_SET_REG_BY_FIELD(MAC_RUL_M3, VLANIDM, sw->vid_mask, hw->msk[3]);
        }
        else if (FAL_ACL_FIELD_LE == sw->vid_op)
        {
            SW_SET_REG_BY_FIELD(MAC_RUL_V3, VLANIDV, 0, hw->vlu[3]);
            SW_SET_REG_BY_FIELD(MAC_RUL_M3, VLANIDM, sw->vid_val, hw->msk[3]);
        }
        else
        {
            SW_SET_REG_BY_FIELD(MAC_RUL_V3, VLANIDV, sw->vid_val, hw->vlu[3]);
            SW_SET_REG_BY_FIELD(MAC_RUL_M3, VLANIDM, 0xfff, hw->msk[3]);
        }
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CFI))
    {
        if (0x0 != sw->cfi_mask)
        {
            *b_care = A_TRUE;
        }

        sw->cfi_val &= sw->cfi_mask;
        SW_SET_REG_BY_FIELD(MAC_RUL_V3, VLANCFIV, sw->cfi_val, hw->vlu[3]);
        SW_SET_REG_BY_FIELD(MAC_RUL_M3, VLANCFIM, sw->cfi_mask, hw->msk[3]);
    }

    SW_SET_REG_BY_FIELD(RUL_SLCT6, RULE_LEN, len, hw->len);

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_ip4_parse(fal_acl_rule_t * sw,
                          shiva_acl_hw_rule_t * hw, a_bool_t * b_care)
{
    a_uint32_t len = 34;

    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));
    aos_mem_zero(&(hw->typ), sizeof (hw->typ));
    aos_mem_zero(&(hw->len), sizeof (hw->len));

    SW_SET_REG_BY_FIELD(RUL_SLCT7, RULE_TYP, SHIVA_IP4_FILTER, hw->typ);

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP_DSCP))
    {
        if (0x0 != sw->ip_dscp_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip_dscp_val &= sw->ip_dscp_mask;
        SW_SET_REG_BY_FIELD(IP4_RUL_V2, IP4DSCPV, sw->ip_dscp_val, hw->vlu[2]);
        SW_SET_REG_BY_FIELD(IP4_RUL_M2, IP4DSCPM, sw->ip_dscp_mask, hw->msk[2]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP_PROTO))
    {
        if (0x0 != sw->ip_proto_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip_proto_val &= sw->ip_proto_mask;
        SW_SET_REG_BY_FIELD(IP4_RUL_V2, IP4PROTV, sw->ip_proto_val, hw->vlu[2]);
        SW_SET_REG_BY_FIELD(IP4_RUL_M2, IP4PROTM, sw->ip_proto_mask,
                            hw->msk[2]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP4_SIP))
    {
        if (0x0 != sw->src_ip4_mask)
        {
            *b_care = A_TRUE;
        }
        sw->src_ip4_val &= sw->src_ip4_mask;
        hw->vlu[1] = sw->src_ip4_val;
        hw->msk[1] = sw->src_ip4_mask;
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP4_DIP))
    {
        if (0x0 != sw->dest_ip4_mask)
        {
            *b_care = A_TRUE;
        }
        sw->dest_ip4_val &= sw->dest_ip4_mask;
        hw->vlu[0] = sw->dest_ip4_val;
        hw->msk[0] = sw->dest_ip4_mask;
    }

    SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4SPORTM_EN, 1, hw->msk[3]);
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_L4_SPORT))
    {
        if ((FAL_ACL_FIELD_MASK != sw->src_l4port_op)
                && (FAL_ACL_FIELD_RANGE != sw->src_l4port_op)
                && (FAL_ACL_FIELD_LE != sw->src_l4port_op)
                && (FAL_ACL_FIELD_GE != sw->src_l4port_op))
        {
            return SW_NOT_SUPPORTED;
        }

        if (A_FALSE ==
                _shiva_acl_field_care(sw->src_l4port_op, sw->src_l4port_val,
                                      sw->src_l4port_mask, 0xffff))
        {
            if (FAL_ACL_FIELD_MASK == sw->src_l4port_op)
            {
                sw->src_l4port_op   = FAL_ACL_FIELD_RANGE;
                sw->src_l4port_val  = 0;
                sw->src_l4port_mask = 0xffff;
            }
        }
        *b_care = A_TRUE;
        len = 38;

        SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4SPORTM_EN, 0, hw->msk[3]);
        if (FAL_ACL_FIELD_MASK == sw->src_l4port_op)
        {
            sw->src_l4port_val &= sw->src_l4port_mask;
            SW_SET_REG_BY_FIELD(IP4_RUL_V3, IP4SPORTV, sw->src_l4port_val,
                                hw->vlu[3]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4SPORTM, sw->src_l4port_mask,
                                hw->msk[3]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4SPORTM_EN, 1, hw->msk[3]);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->src_l4port_op)
        {
            SW_SET_REG_BY_FIELD(IP4_RUL_V3, IP4SPORTV, sw->src_l4port_val,
                                hw->vlu[3]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4SPORTM, sw->src_l4port_mask,
                                hw->msk[3]);
        }
        else if (FAL_ACL_FIELD_LE == sw->src_l4port_op)
        {
            SW_SET_REG_BY_FIELD(IP4_RUL_V3, IP4SPORTV, 0, hw->vlu[3]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4SPORTM, sw->src_l4port_val,
                                hw->msk[3]);
        }
        else
        {
            SW_SET_REG_BY_FIELD(IP4_RUL_V3, IP4SPORTV, sw->src_l4port_val,
                                hw->vlu[3]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4SPORTM, 0xffff, hw->msk[3]);
        }
    }

    SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4DPORTM_EN, 1, hw->msk[3]);
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_L4_DPORT))
    {
        if ((FAL_ACL_FIELD_MASK != sw->dest_l4port_op)
                && (FAL_ACL_FIELD_RANGE != sw->dest_l4port_op)
                && (FAL_ACL_FIELD_LE != sw->dest_l4port_op)
                && (FAL_ACL_FIELD_GE != sw->dest_l4port_op))
        {
            return SW_NOT_SUPPORTED;
        }

        if (A_FALSE ==
                _shiva_acl_field_care(sw->dest_l4port_op, sw->dest_l4port_val,
                                      sw->dest_l4port_mask, 0xffff))
        {
            if (FAL_ACL_FIELD_MASK == sw->dest_l4port_op)
            {
                sw->dest_l4port_op   = FAL_ACL_FIELD_RANGE;
                sw->dest_l4port_val  = 0;
                sw->dest_l4port_mask = 0xffff;
            }
        }
        *b_care = A_TRUE;
        len = 38;

        SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4DPORTM_EN, 0, hw->msk[3]);
        if (FAL_ACL_FIELD_MASK == sw->dest_l4port_op)
        {
            sw->dest_l4port_val &= sw->dest_l4port_mask;
            SW_SET_REG_BY_FIELD(IP4_RUL_V2, IP4DPORTV, sw->dest_l4port_val,
                                hw->vlu[2]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M2, IP4DPORTM, sw->dest_l4port_mask,
                                hw->msk[2]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M3, IP4DPORTM_EN, 1, hw->msk[3]);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->dest_l4port_op)
        {
            SW_SET_REG_BY_FIELD(IP4_RUL_V2, IP4DPORTV, sw->dest_l4port_val,
                                hw->vlu[2]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M2, IP4DPORTM, sw->dest_l4port_mask,
                                hw->msk[2]);
        }
        else if (FAL_ACL_FIELD_LE == sw->dest_l4port_op)
        {
            SW_SET_REG_BY_FIELD(IP4_RUL_V2, IP4DPORTV, 0, hw->vlu[2]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M2, IP4DPORTM, sw->dest_l4port_val,
                                hw->msk[2]);
        }
        else
        {
            SW_SET_REG_BY_FIELD(IP4_RUL_V2, IP4DPORTV, sw->dest_l4port_val,
                                hw->vlu[2]);
            SW_SET_REG_BY_FIELD(IP4_RUL_M2, IP4DPORTM, 0xffff, hw->msk[2]);
        }
    }

    SW_SET_REG_BY_FIELD(RUL_SLCT6, RULE_LEN, len, hw->len);

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_ip6r1_parse(fal_acl_rule_t * sw,
                            shiva_acl_hw_rule_t * hw, a_bool_t * b_care)
{
    a_uint32_t i, len = 54;

    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));
    aos_mem_zero(&(hw->typ), sizeof (hw->typ));
    aos_mem_zero(&(hw->len), sizeof (hw->len));

    SW_SET_REG_BY_FIELD(RUL_SLCT7, RULE_TYP, SHIVA_IP6R1_FILTER, hw->typ);

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP6_DIP))
    {
        for (i = 0; i < 4; i++)
        {
            if (0x0 != sw->dest_ip6_mask.ul[i])
            {
                *b_care = A_TRUE;
            }

            sw->dest_ip6_val.ul[3 - i] &= sw->dest_ip6_mask.ul[3 - i];
            hw->vlu[i] = sw->dest_ip6_val.ul[3 - i];
            hw->msk[i] = sw->dest_ip6_mask.ul[3 - i];
        }
    }

    SW_SET_REG_BY_FIELD(RUL_SLCT6, RULE_LEN, len, hw->len);

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_ip6r2_parse(fal_acl_rule_t * sw,
                            shiva_acl_hw_rule_t * hw, a_bool_t * b_care)
{
    a_uint32_t i, len = 54;

    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));
    aos_mem_zero(&(hw->typ), sizeof (hw->typ));
    aos_mem_zero(&(hw->len), sizeof (hw->len));

    SW_SET_REG_BY_FIELD(RUL_SLCT7, RULE_TYP, SHIVA_IP6R2_FILTER, hw->typ);

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP6_SIP))
    {
        for (i = 0; i < 4; i++)
        {
            if (0x0 != sw->src_ip6_mask.ul[i])
            {
                *b_care = A_TRUE;
            }

            sw->src_ip6_val.ul[3 - i] &= sw->src_ip6_mask.ul[3 - i];
            hw->vlu[i] = sw->src_ip6_val.ul[3 - i];
            hw->msk[i] = sw->src_ip6_mask.ul[3 - i];
        }
    }

    SW_SET_REG_BY_FIELD(RUL_SLCT6, RULE_LEN, len, hw->len);

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_ip6r3_parse(fal_acl_rule_t * sw,
                            shiva_acl_hw_rule_t * hw, a_bool_t * b_care)
{
    a_uint32_t len = 54;

    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));
    aos_mem_zero(&(hw->typ), sizeof (hw->typ));
    aos_mem_zero(&(hw->len), sizeof (hw->len));

    SW_SET_REG_BY_FIELD(RUL_SLCT7, RULE_TYP, SHIVA_IP6R3_FILTER, hw->typ);

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP6_LABEL))
    {
        if (0x0 != sw->ip6_lable_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip6_lable_val &= sw->ip6_lable_mask;
        SW_SET_REG_BY_FIELD(IP6_RUL3_V1, IP6LABEL1V, sw->ip6_lable_val,
                            hw->vlu[1]);
        SW_SET_REG_BY_FIELD(IP6_RUL3_M1, IP6LABEL1M, sw->ip6_lable_mask,
                            hw->msk[1]);

        SW_SET_REG_BY_FIELD(IP6_RUL3_V2, IP6LABEL2V, (sw->ip6_lable_val >> 16),
                            hw->vlu[2]);
        SW_SET_REG_BY_FIELD(IP6_RUL3_M2, IP6LABEL2M, (sw->ip6_lable_mask >> 16),
                            hw->msk[2]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP_PROTO))
    {
        if (0x0 != sw->ip_proto_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip_proto_val &= sw->ip_proto_mask;
        SW_SET_REG_BY_FIELD(IP6_RUL3_V0, IP6PROTV, sw->ip_proto_val,
                            hw->vlu[0]);
        SW_SET_REG_BY_FIELD(IP6_RUL3_M0, IP6PROTM, sw->ip_proto_mask,
                            hw->msk[0]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP_DSCP))
    {
        if (0x0 != sw->ip_dscp_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip_dscp_val &= sw->ip_dscp_mask;
        SW_SET_REG_BY_FIELD(IP6_RUL3_V0, IP6DSCPV, sw->ip_dscp_val, hw->vlu[0]);
        SW_SET_REG_BY_FIELD(IP6_RUL3_M0, IP6DSCPM, sw->ip_dscp_mask,
                            hw->msk[0]);
    }

    SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6SPORTM_EN, 1, hw->msk[3]);
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_L4_SPORT))
    {
        if ((FAL_ACL_FIELD_MASK != sw->src_l4port_op)
                && (FAL_ACL_FIELD_RANGE != sw->src_l4port_op)
                && (FAL_ACL_FIELD_LE != sw->src_l4port_op)
                && (FAL_ACL_FIELD_GE != sw->src_l4port_op))
        {
            return SW_NOT_SUPPORTED;
        }

        if (A_FALSE ==
                _shiva_acl_field_care(sw->src_l4port_op, sw->src_l4port_val,
                                      sw->src_l4port_mask, 0xffff))
        {
            if (FAL_ACL_FIELD_MASK == sw->src_l4port_op)
            {
                sw->src_l4port_op   = FAL_ACL_FIELD_RANGE;
                sw->src_l4port_val  = 0;
                sw->src_l4port_mask = 0xffff;
            }
        }
        *b_care = A_TRUE;
        len = 58;

        SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6SPORTM_EN, 0, hw->msk[3]);
        if (FAL_ACL_FIELD_MASK == sw->src_l4port_op)
        {
            sw->src_l4port_val &= sw->src_l4port_mask;
            SW_SET_REG_BY_FIELD(IP6_RUL3_V3, IP6SPORTV, sw->src_l4port_val,
                                hw->vlu[3]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6SPORTM, sw->src_l4port_mask,
                                hw->msk[3]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6SPORTM_EN, 1, hw->msk[3]);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->src_l4port_op)
        {
            SW_SET_REG_BY_FIELD(IP6_RUL3_V3, IP6SPORTV, sw->src_l4port_val,
                                hw->vlu[3]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6SPORTM, sw->src_l4port_mask,
                                hw->msk[3]);
        }
        else if (FAL_ACL_FIELD_LE == sw->src_l4port_op)
        {
            SW_SET_REG_BY_FIELD(IP6_RUL3_V3, IP6SPORTV, 0, hw->vlu[3]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6SPORTM, sw->src_l4port_val,
                                hw->msk[3]);
        }
        else
        {
            SW_SET_REG_BY_FIELD(IP6_RUL3_V3, IP6SPORTV, sw->src_l4port_val,
                                hw->vlu[3]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6SPORTM, 0xffff, hw->msk[3]);
        }
    }

    SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6DPORTM_EN, 1, hw->msk[3]);
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_L4_DPORT))
    {
        if ((FAL_ACL_FIELD_MASK != sw->dest_l4port_op)
                && (FAL_ACL_FIELD_RANGE != sw->dest_l4port_op)
                && (FAL_ACL_FIELD_LE != sw->dest_l4port_op)
                && (FAL_ACL_FIELD_GE != sw->dest_l4port_op))
        {
            return SW_NOT_SUPPORTED;
        }

        if (A_FALSE ==
                _shiva_acl_field_care(sw->dest_l4port_op, sw->dest_l4port_val,
                                      sw->dest_l4port_mask, 0xffff))
        {
            if (FAL_ACL_FIELD_MASK == sw->dest_l4port_op)
            {
                sw->dest_l4port_op   = FAL_ACL_FIELD_RANGE;
                sw->dest_l4port_val  = 0;
                sw->dest_l4port_mask = 0xffff;
            }
        }
        *b_care = A_TRUE;
        len = 58;

        SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6DPORTM_EN, 0, hw->msk[3]);
        if (FAL_ACL_FIELD_MASK == sw->dest_l4port_op)
        {
            sw->dest_l4port_val &= sw->dest_l4port_mask;
            SW_SET_REG_BY_FIELD(IP6_RUL3_V2, IP6DPORTV, sw->dest_l4port_val,
                                hw->vlu[2]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M2, IP6DPORTM, sw->dest_l4port_mask,
                                hw->msk[2]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M3, IP6DPORTM_EN, 1, hw->msk[3]);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->dest_l4port_op)
        {
            SW_SET_REG_BY_FIELD(IP6_RUL3_V2, IP6DPORTV, sw->dest_l4port_val,
                                hw->vlu[2]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M2, IP6DPORTM, sw->dest_l4port_mask,
                                hw->msk[2]);
        }
        else if (FAL_ACL_FIELD_LE == sw->dest_l4port_op)
        {
            SW_SET_REG_BY_FIELD(IP6_RUL3_V2, IP6DPORTV, 0, hw->vlu[2]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M2, IP6DPORTM, sw->dest_l4port_val,
                                hw->msk[2]);
        }
        else
        {
            SW_SET_REG_BY_FIELD(IP6_RUL3_V2, IP6DPORTV, sw->dest_l4port_val,
                                hw->vlu[2]);
            SW_SET_REG_BY_FIELD(IP6_RUL3_M2, IP6DPORTM, 0xffff, hw->msk[2]);
        }
    }

    SW_SET_REG_BY_FIELD(RUL_SLCT6, RULE_LEN, len, hw->len);

    return SW_OK;
}

#define SHIVA_MAX_UDF_OFFSET 127
#define SHIVA_MAX_UDF_LENGTH 16

static sw_error_t
_shiva_acl_rule_udf_parse(fal_acl_rule_t * sw,
                          shiva_acl_hw_rule_t * hw, a_bool_t * b_care)
{
    a_uint32_t i, len = 0;

    if (FAL_ACL_UDF_TYPE_BUTT <= sw->udf_type)
    {
        return SW_BAD_PARAM;
    }

    if (SHIVA_MAX_UDF_OFFSET < sw->udf_offset)
    {
        return SW_NOT_SUPPORTED;
    }

    if (sw->udf_offset % 2)
    {
        return SW_NOT_SUPPORTED;
    }

    if (SHIVA_MAX_UDF_LENGTH < sw->udf_len)
    {
        return SW_NOT_SUPPORTED;
    }

    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));
    aos_mem_zero(&(hw->typ), sizeof (hw->typ));
    aos_mem_zero(&(hw->len), sizeof (hw->len));

    *b_care = A_FALSE;

    SW_SET_REG_BY_FIELD(RUL_SLCT7, RULE_TYP, SHIVA_UDF_FILTER, hw->typ);

    if (!FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_UDF))
    {
        return SW_OK;
    }

    *b_care = A_TRUE;
    if (FAL_ACL_UDF_TYPE_L2 == sw->udf_type)
    {
        SW_SET_REG_BY_FIELD(UDF_RUL_V4, LAYER_TYP, 0, hw->vlu[4]);
        len = sw->udf_offset + sw->udf_len;
    }
    else
    {
        SW_SET_REG_BY_FIELD(UDF_RUL_V4, LAYER_TYP, 1, hw->vlu[4]);
        len = 14 + sw->udf_offset + sw->udf_len;
    }
    SW_SET_REG_BY_FIELD(RUL_SLCT6, RULE_LEN, len, hw->len);

    SW_SET_REG_BY_FIELD(UDF_RUL_V4, LAYER_OFFSET, sw->udf_offset, hw->vlu[4]);

    for (i = 0; i < sw->udf_len; i++)
    {
        hw->vlu[3 - i / 4] |= ((sw->udf_mask[i] & sw->udf_val[i]) << (24 - 8 * (i % 4)));
        hw->msk[3 - i / 4] |= ((sw->udf_mask[i]) << (24 - 8 * i));
    }

    return SW_OK;
}

static sw_error_t
_shiva_acl_action_parse(a_uint32_t dev_id, const fal_acl_rule_t * sw,
                        shiva_acl_hw_rule_t * hw)
{
    aos_mem_zero(&(hw->act[0]), sizeof (hw->act));

    if ((FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_MODIFY_VLAN))
            && (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_NEST_VLAN)))
    {
        return SW_NOT_SUPPORTED;
    }

    /* FAL_ACL_ACTION_PERMIT need't process */

    /* we should ignore any other action flags when DENY bit is settd. */
    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_DENY))
    {
        SW_SET_REG_BY_FIELD(ACL_RSLT1, DES_PORT_EN, 1, hw->act[1]);
        SW_SET_REG_BY_FIELD(ACL_RSLT1, PORT_MEM, 0, hw->act[1]);
        return SW_OK;
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_RDTCPU))
    {
        SW_SET_REG_BY_FIELD(ACL_RSLT2, RDTCPU, 1, hw->act[2]);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_CPYCPU))
    {
        SW_SET_REG_BY_FIELD(ACL_RSLT2, CPYCPU, 1, hw->act[2]);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_MIRROR))
    {
        SW_SET_REG_BY_FIELD(ACL_RSLT1, MIRR_EN, 1, hw->act[1]);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REDPT))
    {
        SW_SET_REG_BY_FIELD(ACL_RSLT1, DES_PORT_EN, 1, hw->act[1]);
        SW_SET_REG_BY_FIELD(ACL_RSLT1, PORT_MEM, sw->ports, hw->act[1]);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_UP))
    {
        SW_SET_REG_BY_FIELD(ACL_RSLT1, REMARK_DOT1P, 1, hw->act[1]);
        SW_SET_REG_BY_FIELD(ACL_RSLT1, DOT1P, sw->up, hw->act[1]);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_QUEUE))
    {
        SW_SET_REG_BY_FIELD(ACL_RSLT1, REMARK_PRI_QU, 1, hw->act[1]);
        SW_SET_REG_BY_FIELD(ACL_RSLT1, PRI_QU, sw->queue, hw->act[1]);
    }

    if ((FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_MODIFY_VLAN))
            || (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_NEST_VLAN)))
    {

        SW_SET_REG_BY_FIELD(ACL_RSLT1, CHG_VID_EN, 1, hw->act[1]);
        SW_SET_REG_BY_FIELD(ACL_RSLT1, VID, sw->vid, hw->act[1]);
        SW_SET_REG_BY_FIELD(ACL_RSLT1, STAG_CHG_EN, 1, hw->act[1]);
        if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_MODIFY_VLAN))
        {
            SW_SET_REG_BY_FIELD(ACL_RSLT1, STAG_CHG_EN, 0, hw->act[1]);

            if (!FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REDPT))
            {
                SW_SET_REG_BY_FIELD(ACL_RSLT1, VID_MEM_EN, 1, hw->act[1]);
                SW_SET_REG_BY_FIELD(ACL_RSLT1, PORT_MEM, sw->ports, hw->act[1]);
            }
        }
    }

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_mac_reparse(fal_acl_rule_t * sw,
                            const shiva_acl_hw_rule_t * hw)
{
    a_uint32_t mask_en;

    /* destnation mac address */
    SW_GET_FIELD_BY_REG(MAC_RUL_V0, DAV_BYTE2, sw->dest_mac_val.uc[2],
                        hw->vlu[0]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V0, DAV_BYTE3, sw->dest_mac_val.uc[3],
                        hw->vlu[0]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V0, DAV_BYTE4, sw->dest_mac_val.uc[4],
                        hw->vlu[0]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V0, DAV_BYTE5, sw->dest_mac_val.uc[5],
                        hw->vlu[0]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V1, DAV_BYTE0, sw->dest_mac_val.uc[0],
                        hw->vlu[1]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V1, DAV_BYTE1, sw->dest_mac_val.uc[1],
                        hw->vlu[1]);

    SW_GET_FIELD_BY_REG(MAC_RUL_M0, DAM_BYTE2, sw->dest_mac_mask.uc[2],
                        hw->msk[0]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M0, DAM_BYTE3, sw->dest_mac_mask.uc[3],
                        hw->msk[0]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M0, DAM_BYTE4, sw->dest_mac_mask.uc[4],
                        hw->msk[0]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M0, DAM_BYTE5, sw->dest_mac_mask.uc[5],
                        hw->msk[0]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M1, DAM_BYTE0, sw->dest_mac_mask.uc[0],
                        hw->msk[1]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M1, DAM_BYTE1, sw->dest_mac_mask.uc[1],
                        hw->msk[1]);
    if (A_FALSE == _shiva_acl_zero_addr(sw->dest_mac_mask))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_DA);
    }

    /* source mac address */
    SW_GET_FIELD_BY_REG(MAC_RUL_V2, SAV_BYTE0, sw->src_mac_val.uc[0],
                        hw->vlu[2]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V2, SAV_BYTE1, sw->src_mac_val.uc[1],
                        hw->vlu[2]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V2, SAV_BYTE2, sw->src_mac_val.uc[2],
                        hw->vlu[2]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V2, SAV_BYTE3, sw->src_mac_val.uc[3],
                        hw->vlu[2]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V1, SAV_BYTE4, sw->src_mac_val.uc[4],
                        hw->vlu[1]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V1, SAV_BYTE5, sw->src_mac_val.uc[5],
                        hw->vlu[1]);

    SW_GET_FIELD_BY_REG(MAC_RUL_M2, SAM_BYTE0, sw->src_mac_mask.uc[0],
                        hw->msk[2]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M2, SAM_BYTE1, sw->src_mac_mask.uc[1],
                        hw->msk[2]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M2, SAM_BYTE2, sw->src_mac_mask.uc[2],
                        hw->msk[2]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M2, SAM_BYTE3, sw->src_mac_mask.uc[3],
                        hw->msk[2]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M1, SAM_BYTE4, sw->src_mac_mask.uc[4],
                        hw->msk[1]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M1, SAM_BYTE5, sw->src_mac_mask.uc[5],
                        hw->msk[1]);
    if (A_FALSE == _shiva_acl_zero_addr(sw->src_mac_mask))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_SA);
    }

    /* ethernet type */
    SW_GET_FIELD_BY_REG(MAC_RUL_V3, ETHTYPV, sw->ethtype_val, hw->vlu[3]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M3, ETHTYPM, sw->ethtype_mask, hw->msk[3]);
    if (0x0 != sw->ethtype_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_ETHTYPE);
    }

    /* packet tagged */
    SW_GET_FIELD_BY_REG(MAC_RUL_V4, TAGGEDV, sw->tagged_val, hw->vlu[4]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V4, TAGGEDM, sw->tagged_mask, hw->vlu[4]);
    if (0x0 != sw->tagged_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_TAGGED);
    }

    /* vlan priority */
    SW_GET_FIELD_BY_REG(MAC_RUL_V3, VLANPRIV, sw->up_val, hw->vlu[3]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M3, VLANPRIM, sw->up_mask, hw->msk[3]);
    if (0x0 != sw->up_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_UP);
    }

    /* vlanid */
    SW_GET_FIELD_BY_REG(MAC_RUL_V3, VLANIDV, sw->vid_val, hw->vlu[3]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M3, VLANIDM, sw->vid_mask, hw->msk[3]);
    SW_GET_FIELD_BY_REG(MAC_RUL_V4, VIDMSK, mask_en, hw->vlu[4]);
    if (mask_en)
    {
        sw->vid_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->vid_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _shiva_acl_field_care(sw->vid_op, (a_uint32_t) sw->vid_val,
                                  (a_uint32_t) sw->vid_mask, 0xfff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_VID);
    }

    /* vlan cfi */
    SW_GET_FIELD_BY_REG(MAC_RUL_V3, VLANCFIV, sw->cfi_val, hw->vlu[3]);
    SW_GET_FIELD_BY_REG(MAC_RUL_M3, VLANCFIM, sw->cfi_mask, hw->msk[3]);
    if (0x0 != sw->cfi_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_CFI);
    }

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_ip4_reparse(fal_acl_rule_t * sw,
                            const shiva_acl_hw_rule_t * hw)
{
    a_uint32_t mask_en;

    sw->dest_ip4_val = hw->vlu[0];
    sw->dest_ip4_mask = hw->msk[0];
    if (0x0 != sw->dest_ip4_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP4_DIP);
    }

    sw->src_ip4_val = hw->vlu[1];
    sw->src_ip4_mask = hw->msk[1];
    if (0x0 != sw->src_ip4_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP4_SIP);
    }

    SW_GET_FIELD_BY_REG(IP4_RUL_V2, IP4PROTV, sw->ip_proto_val, hw->vlu[2]);
    SW_GET_FIELD_BY_REG(IP4_RUL_M2, IP4PROTM, sw->ip_proto_mask, hw->msk[2]);
    if (0x0 != sw->ip_proto_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP_PROTO);
    }

    SW_GET_FIELD_BY_REG(IP4_RUL_V2, IP4DSCPV, sw->ip_dscp_val, hw->vlu[2]);
    SW_GET_FIELD_BY_REG(IP4_RUL_M2, IP4DSCPM, sw->ip_dscp_mask, hw->msk[2]);
    if (0x0 != sw->ip_dscp_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP_DSCP);
    }

    SW_GET_FIELD_BY_REG(IP4_RUL_V2, IP4DPORTV, sw->dest_l4port_val, hw->vlu[2]);
    SW_GET_FIELD_BY_REG(IP4_RUL_M2, IP4DPORTM, sw->dest_l4port_mask,
                        hw->msk[2]);
    SW_GET_FIELD_BY_REG(IP4_RUL_M3, IP4DPORTM_EN, mask_en, hw->msk[3]);
    if (mask_en)
    {
        sw->dest_l4port_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->dest_l4port_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _shiva_acl_field_care(sw->dest_l4port_op,
                                  (a_uint32_t) sw->dest_l4port_val,
                                  (a_uint32_t) sw->dest_l4port_mask, 0xffff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_DPORT);
    }
    else if (FAL_ACL_FIELD_RANGE == sw->dest_l4port_op)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_DPORT);
    }

    SW_GET_FIELD_BY_REG(IP4_RUL_V3, IP4SPORTV, sw->src_l4port_val, hw->vlu[3]);
    SW_GET_FIELD_BY_REG(IP4_RUL_M3, IP4SPORTM, sw->src_l4port_mask, hw->msk[3]);
    SW_GET_FIELD_BY_REG(IP4_RUL_M3, IP4SPORTM_EN, mask_en, hw->msk[3]);
    if (mask_en)
    {
        sw->src_l4port_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->src_l4port_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _shiva_acl_field_care(sw->src_l4port_op,
                                  (a_uint32_t) sw->src_l4port_val,
                                  (a_uint32_t) sw->src_l4port_mask, 0xffff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_SPORT);
    }
    else if (FAL_ACL_FIELD_RANGE == sw->src_l4port_op)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_SPORT);
    }

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_ip6r1_reparse(fal_acl_rule_t * sw,
                              const shiva_acl_hw_rule_t * hw)
{
    a_uint32_t i;

    for (i = 0; i < 4; i++)
    {
        sw->dest_ip6_val.ul[i] = hw->vlu[3 - i];
        sw->dest_ip6_mask.ul[i] = hw->msk[3 - i];
        if (0x0 != sw->dest_ip6_mask.ul[i])
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP6_DIP);
        }
    }
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_ip6r2_reparse(fal_acl_rule_t * sw,
                              const shiva_acl_hw_rule_t * hw)
{
    a_uint32_t i;

    for (i = 0; i < 4; i++)
    {
        sw->src_ip6_val.ul[i] = hw->vlu[3 - i];
        sw->src_ip6_mask.ul[i] = hw->msk[3 - i];
        if (0x0 != sw->src_ip6_mask.ul[i])
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP6_SIP);
        }
    }
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_ip6r3_reparse(fal_acl_rule_t * sw,
                              const shiva_acl_hw_rule_t * hw)
{
    a_uint32_t mask_en;
    a_uint32_t tmp;

    SW_GET_FIELD_BY_REG(IP6_RUL3_V0, IP6PROTV, sw->ip_proto_val, hw->vlu[0]);
    SW_GET_FIELD_BY_REG(IP6_RUL3_M0, IP6PROTM, sw->ip_proto_mask, hw->msk[0]);
    if (0x0 != sw->ip_proto_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP_PROTO);
    }

    SW_GET_FIELD_BY_REG(IP6_RUL3_V0, IP6DSCPV, sw->ip_dscp_val, hw->vlu[0]);
    SW_GET_FIELD_BY_REG(IP6_RUL3_M0, IP6DSCPM, sw->ip_dscp_mask, hw->msk[0]);
    if (0x0 != sw->ip_dscp_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP_DSCP);
    }

    SW_GET_FIELD_BY_REG(IP6_RUL3_V2, IP6DPORTV, sw->dest_l4port_val,
                        hw->vlu[2]);
    SW_GET_FIELD_BY_REG(IP6_RUL3_M2, IP6DPORTM, sw->dest_l4port_mask,
                        hw->msk[2]);
    SW_GET_FIELD_BY_REG(IP6_RUL3_M3, IP6DPORTM_EN, mask_en, hw->msk[3]);
    if (mask_en)
    {
        sw->dest_l4port_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->dest_l4port_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _shiva_acl_field_care(sw->dest_l4port_op,
                                  (a_uint32_t) sw->dest_l4port_val,
                                  (a_uint32_t) sw->dest_l4port_mask, 0xffff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_DPORT);
    }
    else if (FAL_ACL_FIELD_RANGE == sw->dest_l4port_op)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_DPORT);
    }

    SW_GET_FIELD_BY_REG(IP6_RUL3_V3, IP6SPORTV, sw->src_l4port_val, hw->vlu[3]);
    SW_GET_FIELD_BY_REG(IP6_RUL3_M3, IP6SPORTM, sw->src_l4port_mask,
                        hw->msk[3]);
    SW_GET_FIELD_BY_REG(IP6_RUL3_M3, IP6SPORTM_EN, mask_en, hw->msk[3]);
    if (mask_en)
    {
        sw->src_l4port_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->src_l4port_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _shiva_acl_field_care(sw->src_l4port_op,
                                  (a_uint32_t) sw->src_l4port_val,
                                  (a_uint32_t) sw->src_l4port_mask, 0xffff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_SPORT);
    }
    else if (FAL_ACL_FIELD_RANGE == sw->src_l4port_op)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_SPORT);
    }

    SW_GET_FIELD_BY_REG(IP6_RUL3_V1, IP6LABEL1V, sw->ip6_lable_val, hw->vlu[1]);
    SW_GET_FIELD_BY_REG(IP6_RUL3_M1, IP6LABEL1M, sw->ip6_lable_mask,
                        hw->msk[1]);

    SW_GET_FIELD_BY_REG(IP6_RUL3_V2, IP6LABEL2V, tmp, hw->vlu[2]);
    sw->ip6_lable_val |= (tmp << 16);
    SW_GET_FIELD_BY_REG(IP6_RUL3_M2, IP6LABEL2M, tmp, hw->msk[2]);
    sw->ip6_lable_mask |= (tmp << 16);

    if (0x0 != sw->ip6_lable_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP6_LABEL);
    }

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_udf_reparse(fal_acl_rule_t * sw,
                            const shiva_acl_hw_rule_t * hw)
{
    a_uint32_t i, tmp;

    FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_UDF);

    SW_GET_FIELD_BY_REG(UDF_RUL_V4, LAYER_OFFSET, tmp, hw->vlu[4]);
    sw->udf_offset = tmp;

    SW_GET_FIELD_BY_REG(UDF_RUL_V4, LAYER_TYP, tmp, hw->vlu[4]);
    if (tmp)
    {
        sw->udf_type = FAL_ACL_UDF_TYPE_L3;
        sw->udf_len  = hw->len - sw->udf_offset - 14;
    }
    else
    {
        sw->udf_type = FAL_ACL_UDF_TYPE_L2;
        sw->udf_len  = hw->len - sw->udf_offset;
    }

    if (SHIVA_MAX_UDF_LENGTH < sw->udf_len)
    {
        return SW_READ_ERROR;
    }

    for (i = 0; i < sw->udf_len; i++)
    {
        sw->udf_val[i]  = ((hw->vlu[3 - i / 4]) >> (24 - 8 * (i % 4))) & 0xff;
        sw->udf_mask[i] = ((hw->msk[3 - i / 4]) >> (24 - 8 * (i % 4))) & 0xff;
    }

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_action_reparse(fal_acl_rule_t * sw,
                               const shiva_acl_hw_rule_t * hw)
{
    a_uint32_t data;

    sw->match_cnt = hw->act[0];

    sw->action_flg = 0;
    SW_GET_FIELD_BY_REG(ACL_RSLT1, DES_PORT_EN, data, (hw->act[1]));
    if (1 == data)
    {
        SW_GET_FIELD_BY_REG(ACL_RSLT1, PORT_MEM, data, (hw->act[1]));
        sw->ports = data;

        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REDPT);
    }

    SW_GET_FIELD_BY_REG(ACL_RSLT2, RDTCPU, data, (hw->act[2]));
    if (1 == data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_RDTCPU);
    }

    SW_GET_FIELD_BY_REG(ACL_RSLT2, CPYCPU, data, (hw->act[2]));
    if (1 == data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_CPYCPU);
    }

    SW_GET_FIELD_BY_REG(ACL_RSLT1, MIRR_EN, data, (hw->act[1]));
    if (1 == data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_MIRROR);
    }

    SW_GET_FIELD_BY_REG(ACL_RSLT1, REMARK_DOT1P, data, (hw->act[1]));
    if (1 == data)
    {
        SW_GET_FIELD_BY_REG(ACL_RSLT1, DOT1P, data, (hw->act[1]));
        sw->up = data & 0x7;

        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_UP);
    }

    SW_GET_FIELD_BY_REG(ACL_RSLT1, REMARK_PRI_QU, data, (hw->act[1]));
    if (1 == data)
    {
        SW_GET_FIELD_BY_REG(ACL_RSLT1, PRI_QU, data, (hw->act[1]));
        sw->queue = data & 0x3;

        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_QUEUE);
    }

    SW_GET_FIELD_BY_REG(ACL_RSLT1, CHG_VID_EN, data, (hw->act[1]));
    if (1 == data)
    {
        SW_GET_FIELD_BY_REG(ACL_RSLT1, STAG_CHG_EN, data, (hw->act[1]));
        if (1 == data)
        {
            FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_NEST_VLAN);
        }
        else
        {
            FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_MODIFY_VLAN);
            SW_GET_FIELD_BY_REG(ACL_RSLT1, PORT_MEM, data, (hw->act[1]));
            sw->ports = data;
        }
    }

    SW_GET_FIELD_BY_REG(ACL_RSLT1, VID, data, (hw->act[1]));
    sw->vid = data & 0xfff;

    return SW_OK;
}

static sw_error_t
_shiva_acl_filter_alloc(a_uint32_t dev_id, a_uint32_t * idx)
{
    a_uint32_t i;

    for (i = 0; i < SHIVA_MAX_RULE; i++)
    {
        if (0 == (filter_snap[dev_id] & (0x1UL << i)))
        {
            filter_snap[dev_id] |= (0x1UL << i);
            *idx = i;
            return SW_OK;
        }
    }
    return SW_NO_RESOURCE;
}

static void
_shiva_acl_filter_free(a_uint32_t dev_id, a_uint32_t idx)
{
    filter_snap[dev_id] &= (~(0x1UL << idx));
}

static void
_shiva_acl_filter_snap(a_uint32_t dev_id)
{
    filter_snap[dev_id] = filter[dev_id];
    return;
}

static void
_shiva_acl_filter_commit(a_uint32_t dev_id)
{
    filter[dev_id] = filter_snap[dev_id];
    return;
}

static sw_error_t
_shiva_acl_slct_update(shiva_acl_hw_rule_t * hw, a_uint32_t offset,
                       a_uint32_t flt_idx)
{
    switch (offset)
    {
        case 0:
            SW_SET_REG_BY_FIELD(RUL_SLCT0, ADDR0_EN, 1, hw->slct[0]);
            SW_SET_REG_BY_FIELD(RUL_SLCT1, ADDR0, flt_idx, hw->slct[1]);
            break;

        case 1:
            SW_SET_REG_BY_FIELD(RUL_SLCT0, ADDR1_EN, 1, hw->slct[0]);
            SW_SET_REG_BY_FIELD(RUL_SLCT2, ADDR1, flt_idx, hw->slct[2]);
            break;

        case 2:
            SW_SET_REG_BY_FIELD(RUL_SLCT0, ADDR2_EN, 1, hw->slct[0]);
            SW_SET_REG_BY_FIELD(RUL_SLCT3, ADDR2, flt_idx, hw->slct[3]);
            break;

        case 3:
            SW_SET_REG_BY_FIELD(RUL_SLCT0, ADDR3_EN, 1, hw->slct[0]);
            SW_SET_REG_BY_FIELD(RUL_SLCT4, ADDR3, flt_idx, hw->slct[4]);
            break;

        default:
            return SW_FAIL;
    }
    return SW_OK;
}

static sw_error_t
_shiva_acl_filter_write(a_uint32_t dev_id, const shiva_acl_hw_rule_t * rule,
                        a_uint32_t flt_idx)
{
#ifdef SHIVA_SW_ENTRY
    char *memaddr;

    memaddr = flt_vlu_mem + (flt_idx << 5);
    aos_mem_copy(memaddr, (char *) &(rule->vlu[0]), sizeof(rule->vlu));

    memaddr = flt_msk_mem + (flt_idx << 5);
    aos_mem_copy(memaddr, (char *) &(rule->msk[0]), sizeof(rule->vlu));

    memaddr = flt_typ_mem + (flt_idx << 5);
    aos_mem_copy(memaddr, (char *) &(rule->typ), sizeof(rule->typ));

    memaddr = flt_len_mem + (flt_idx << 5);
    aos_mem_copy(memaddr, (char *) &(rule->len), sizeof(rule->len));

#else
    sw_error_t rv;
    a_uint32_t i, base, addr;

    /* set filter value */
    base = SHIVA_RULE_VLU_ADDR + (flt_idx << 5);
    for (i = 0; i < (sizeof(rule->vlu) / sizeof(a_uint32_t)); i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(rule->vlu[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    /* set filter mask */
    base = SHIVA_RULE_MSK_ADDR + (flt_idx << 5);
    for (i = 0; i < (sizeof(rule->msk) / sizeof(a_uint32_t)); i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(rule->msk[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    /* set filter type */
    addr = SHIVA_RULE_TYP_ADDR + (flt_idx << 5);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(rule->typ)), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* set filter length */
    addr = SHIVA_RULE_LEN_ADDR + (flt_idx << 5);
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(rule->len)), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
#endif

#ifdef SHIVA_ENTRY_DUMP
    a_uint32_t j;
    aos_printk("\n_shiva_acl_filter_write flt_idx = %d type = %d len = %d\n",
               flt_idx, rule->typ, rule->len);
    for (j = 0; j < (sizeof(rule->vlu) / sizeof(a_uint32_t)); j++)
    {
        aos_printk("%08x  ", rule->vlu[j]);
    }
    aos_printk("\n");
    for (j = 0; j < (sizeof(rule->msk) / sizeof(a_uint32_t)); j++)
    {
        aos_printk("%08x  ", rule->msk[j]);
    }
#endif

    return SW_OK;
}

static sw_error_t
_shiva_acl_action_write(a_uint32_t dev_id, const shiva_acl_hw_rule_t * rule,
                        a_uint32_t act_idx)
{
#ifdef SHIVA_SW_ENTRY
    char *memaddr;

    memaddr = act_mem + (act_idx << 5);
    aos_mem_copy(memaddr, (char *) &(rule->act[0]), sizeof(rule->act));

#else
    sw_error_t rv;
    a_uint32_t i, base, addr;

    /* set rule action */
    base = SHIVA_RULE_ACT_ADDR + (act_idx << 5);
    for (i = 0; i < (sizeof(rule->act) / sizeof(a_uint32_t)); i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(rule->act[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
#endif

#ifdef SHIVA_ENTRY_DUMP
    a_uint32_t j;
    aos_printk("\n_shiva_acl_action_write act_idx = %d\n", act_idx);
    for (j = 0; j < (sizeof(rule->act) / sizeof(a_uint32_t)); j++)
    {
        aos_printk("%08x  ", rule->act[j]);
    }
#endif

    return SW_OK;
}

static sw_error_t
_shiva_acl_slct_write(a_uint32_t dev_id, const shiva_acl_hw_rule_t * rule,
                      a_uint32_t slct_idx)
{
#ifdef SHIVA_SW_ENTRY
    char *memaddr;

    memaddr = slct_mem + (slct_idx << 5);
    aos_mem_copy(memaddr, (char *) &(rule->slct[0]), sizeof(rule->slct));

#else
    sw_error_t rv;
    a_uint32_t base, addr;
    a_uint32_t i;

    base = SHIVA_RULE_SLCT_ADDR + (slct_idx << 5);

    /* set filter address and source ports bitmap*/
    for (i = 1; i < (sizeof(rule->slct) / sizeof(a_uint32_t)); i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(rule->slct[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    /* set filter enable */
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, base, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(rule->slct[0])),
                          sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
#endif

#ifdef SHIVA_ENTRY_DUMP
    a_uint32_t j;
    aos_printk("\n_shiva_acl_slct_write slct_idx = %d\n", slct_idx);
    for (j = 0; j < (sizeof(rule->slct) / sizeof(a_uint32_t)); j++)
    {
        aos_printk("%08x  ", rule->slct[j]);
    }
#endif

    return SW_OK;
}

static sw_error_t
_shiva_acl_filter_read(a_uint32_t dev_id, shiva_acl_hw_rule_t * rule,
                       a_uint32_t flt_idx)
{
#ifdef SHIVA_SW_ENTRY
    char *memaddr;

    memaddr = flt_vlu_mem + (flt_idx << 5);
    aos_mem_copy((char *) &(rule->vlu[0]), memaddr, 20);

    memaddr = flt_msk_mem + (flt_idx << 5);
    aos_mem_copy((char *) &(rule->msk[0]), memaddr, 20);

    memaddr = flt_typ_mem + (flt_idx << 5);
    aos_mem_copy((char *) &(rule->typ), memaddr, 4);

    memaddr = flt_len_mem + (flt_idx << 5);
    aos_mem_copy((char *) &(rule->len), memaddr, 4);

#else
    sw_error_t rv;
    a_uint32_t i, base, addr;

    /* get filter value */
    base = SHIVA_RULE_VLU_ADDR + (flt_idx << 5);
    for (i = 0; i < (sizeof(rule->vlu) / sizeof(a_uint32_t)); i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(rule->vlu[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    /* get filter mask */
    base = SHIVA_RULE_MSK_ADDR + (flt_idx << 5);
    for (i = 0; i < (sizeof(rule->msk) / sizeof(a_uint32_t)); i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(rule->msk[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    /* get filter type */
    addr = SHIVA_RULE_TYP_ADDR + (flt_idx << 5);
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(rule->typ)), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* get filter length */
    addr = SHIVA_RULE_LEN_ADDR + (flt_idx << 5);
    HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(rule->len)), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
#endif

#ifdef SHIVA_ENTRY_DUMP
    a_uint32_t j;
    aos_printk("\n_shiva_acl_filter_read flt_idx = %d type = %d len = %d \n",
               flt_idx, rule->typ, rule->len);
    for (j = 0; j < (sizeof(rule->vlu) / sizeof(a_uint32_t)); j++)
    {
        aos_printk("%08x  ", rule->vlu[j]);
    }
    aos_printk("\n");
    for (j = 0; j < (sizeof(rule->msk) / sizeof(a_uint32_t)); j++)
    {
        aos_printk("%08x  ", rule->msk[j]);
    }
#endif

    return SW_OK;
}

static sw_error_t
_shiva_acl_action_read(a_uint32_t dev_id, shiva_acl_hw_rule_t * rule,
                       a_uint32_t act_idx)
{
#ifdef SHIVA_SW_ENTRY
    char *memaddr;

    memaddr = act_mem + (act_idx << 5);
    aos_mem_copy((char *) &(rule->act[0]), memaddr, sizeof(rule->act));

#else
    sw_error_t rv;
    a_uint32_t i, base, addr;

    /* get rule action */
    base = SHIVA_RULE_ACT_ADDR + (act_idx << 5);
    for (i = 0; i < (sizeof(rule->act) / sizeof(a_uint32_t)); i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(rule->act[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
#endif

#ifdef SHIVA_ENTRY_DUMP
    a_uint32_t j;
    aos_printk("\n_shiva_acl_action_read act_idx = %d    ", act_idx);
    for (j = 0; j < (sizeof(rule->act) / sizeof(a_uint32_t)); j++)
    {
        aos_printk("%08x  ", rule->act[j]);
    }
#endif

    return SW_OK;
}

static sw_error_t
_shiva_acl_slct_read(a_uint32_t dev_id, shiva_acl_hw_rule_t * rule,
                     a_uint32_t slct_idx)
{
#ifdef SHIVA_SW_ENTRY
    char *memaddr;

    memaddr = slct_mem + (slct_idx << 5);
    aos_mem_copy((char *) &(rule->slct[0]), memaddr, sizeof(rule->slct));

#else
    sw_error_t rv;
    a_uint32_t i, base, addr;

    base = SHIVA_RULE_SLCT_ADDR + (slct_idx << 5);

    /* get filter address and enable and source ports bitmap */
    for (i = 0; i < (sizeof(rule->slct) / sizeof(a_uint32_t)); i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(rule->slct[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
#endif

#ifdef SHIVA_ENTRY_DUMP
    a_uint32_t j;
    aos_printk("\n_shiva_acl_slct_read slct_idx = %d\n", slct_idx);
    for (j = 0; j < (sizeof(rule->slct) / sizeof(a_uint32_t)); j++)
    {
        aos_printk("%08x  ", rule->slct[j]);
    }
#endif

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_set(a_uint32_t dev_id, a_uint32_t base_addr,
                    const shiva_acl_hw_rule_t * rule, a_uint32_t rule_nr)
{
    sw_error_t rv;
    a_uint32_t ent_idx, tmp_ent_idx;
    a_uint32_t i, flt_nr, flt_idx[4] = {0};
    a_uint32_t act_idx, slct_idx;

    act_idx = base_addr;
    slct_idx = base_addr;
    ent_idx = 0;
    for (i = 0; i < rule_nr; i++)
    {
        tmp_ent_idx = ent_idx;

        rv = _shiva_acl_filter_map_get(&rule[ent_idx], flt_idx, &flt_nr);
        SW_RTN_ON_ERROR(rv);

        if (!flt_nr)
        {
            return SW_FAIL;
        }

        for (i = 0; i < flt_nr; i++)
        {
            rv = _shiva_acl_filter_write(dev_id, &(rule[ent_idx]), flt_idx[i]);
            ent_idx++;
        }

        rv = _shiva_acl_action_write(dev_id, &(rule[tmp_ent_idx]), act_idx);
        SW_RTN_ON_ERROR(rv);

        rv = _shiva_acl_slct_write(dev_id, &(rule[tmp_ent_idx]), slct_idx);
        SW_RTN_ON_ERROR(rv);

        act_idx++;
        slct_idx++;
    }
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_get(a_uint32_t dev_id, shiva_acl_hw_rule_t * rule,
                    a_uint32_t * ent_idx, a_uint32_t rule_idx)
{
    sw_error_t rv;
    a_uint32_t i = 0, tmp_idx = 0, flt_nr = 0, flt_idx[4] = {0};

    tmp_idx = *ent_idx;

    rv = _shiva_acl_slct_read(dev_id, &rule[tmp_idx], rule_idx);
    SW_RTN_ON_ERROR(rv);

    rv = _shiva_acl_action_read(dev_id, &rule[tmp_idx], rule_idx);
    SW_RTN_ON_ERROR(rv);

    rv = _shiva_acl_filter_map_get(&rule[tmp_idx], flt_idx, &flt_nr);
    SW_RTN_ON_ERROR(rv);

    for (i = 0; i < flt_nr; i++)
    {
        rv = _shiva_acl_filter_read(dev_id, &rule[tmp_idx], flt_idx[i]);
        SW_RTN_ON_ERROR(rv);

        tmp_idx++;
    }

    *ent_idx = tmp_idx;
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_sw_to_hw(a_uint32_t dev_id, fal_acl_rule_t * sw,
                         shiva_acl_hw_rule_t * hw, a_uint32_t * idx)
{
    sw_error_t rv;
    a_bool_t b_care;
    a_bool_t b_valid = A_FALSE;
    a_uint32_t tmp_idx;

    tmp_idx = *idx;
    if (FAL_ACL_RULE_MAC == sw->rule_type)
    {
        rv = _shiva_acl_rule_udf_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        if (A_TRUE == b_care)
        {
            tmp_idx++;
        }

        rv = _shiva_acl_rule_mac_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        tmp_idx++;
    }
    else if (FAL_ACL_RULE_IP4 == sw->rule_type)
    {
        rv = _shiva_acl_rule_udf_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        if (A_TRUE == b_care)
        {
            tmp_idx++;
        }

        rv = _shiva_acl_rule_mac_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        if (A_TRUE == b_care)
        {
            tmp_idx++;
        }

        rv = _shiva_acl_rule_ip4_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        tmp_idx++;
    }
    else if (FAL_ACL_RULE_IP6 == sw->rule_type)
    {
        rv = _shiva_acl_rule_udf_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        if (A_TRUE == b_care)
        {
            tmp_idx++;
        }

        rv = _shiva_acl_rule_mac_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        if (A_TRUE == b_care)
        {
            tmp_idx++;
        }

        rv = _shiva_acl_rule_ip6r1_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        if (A_TRUE == b_care)
        {
            tmp_idx++;
            b_valid = A_TRUE;
        }

        rv = _shiva_acl_rule_ip6r2_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        if (A_TRUE == b_care)
        {
            tmp_idx++;
            b_valid = A_TRUE;
        }

        rv = _shiva_acl_rule_ip6r3_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        if ((A_TRUE == b_care) || (A_FALSE == b_valid))
        {
            tmp_idx++;
        }
        else
        {
            hw[tmp_idx - 1].len = hw[tmp_idx].len;
        }
    }
    else if (FAL_ACL_RULE_UDF == sw->rule_type)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_UDF);
        rv = _shiva_acl_rule_udf_parse(sw, &hw[tmp_idx], &b_care);
        SW_RTN_ON_ERROR(rv);
        tmp_idx++;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    rv = _shiva_acl_action_parse(dev_id, sw, &(hw_rule_ent[*idx]));
    SW_RTN_ON_ERROR(rv);

    *idx = tmp_idx;
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_hw_to_sw(fal_acl_rule_t * sw, const shiva_acl_hw_rule_t * hw,
                         a_uint32_t ent_idx, a_uint32_t ent_nr)
{
    sw_error_t rv;
    a_uint32_t i, flt_typ;
    a_bool_t b_mac = A_FALSE, b_ip4 = A_FALSE, b_ip6 = A_FALSE;

    rv = _shiva_acl_rule_action_reparse(sw, &hw[ent_idx]);
    SW_RTN_ON_ERROR(rv);

    sw->rule_type = FAL_ACL_RULE_UDF;
    for (i = 0; i < ent_nr; i++)
    {
        SW_GET_FIELD_BY_REG(RUL_SLCT7, RULE_TYP, flt_typ, hw[ent_idx + i].typ);

        if (SHIVA_UDF_FILTER == flt_typ)
        {
            rv = _shiva_acl_rule_udf_reparse(sw, &hw[ent_idx + i]);
            SW_RTN_ON_ERROR(rv);
        }
        else if (SHIVA_MAC_FILTER == flt_typ)
        {
            rv = _shiva_acl_rule_mac_reparse(sw, &hw[ent_idx + i]);
            SW_RTN_ON_ERROR(rv);
            b_mac = A_TRUE;
        }
        else if (SHIVA_IP4_FILTER == flt_typ)
        {
            rv = _shiva_acl_rule_ip4_reparse(sw, &hw[ent_idx + i]);
            SW_RTN_ON_ERROR(rv);
            b_ip4 = A_TRUE;
        }
        else if (SHIVA_IP6R1_FILTER == flt_typ)
        {
            rv = _shiva_acl_rule_ip6r1_reparse(sw, &hw[ent_idx + i]);
            SW_RTN_ON_ERROR(rv);
            b_ip6 = A_TRUE;
        }
        else if (SHIVA_IP6R2_FILTER == flt_typ)
        {
            rv = _shiva_acl_rule_ip6r2_reparse(sw, &hw[ent_idx + i]);
            SW_RTN_ON_ERROR(rv);
            b_ip6 = A_TRUE;
        }
        else if (SHIVA_IP6R3_FILTER == flt_typ)
        {
            rv = _shiva_acl_rule_ip6r3_reparse(sw, &hw[ent_idx + i]);
            SW_RTN_ON_ERROR(rv);
            b_ip6 = A_TRUE;
        }
        else
        {
            return SW_FAIL;
        }
    }

    if (A_TRUE == b_mac)
    {
        sw->rule_type = FAL_ACL_RULE_MAC;
    }

    if (A_TRUE == b_ip4)
    {
        sw->rule_type = FAL_ACL_RULE_IP4;
    }

    if (A_TRUE == b_ip6)
    {
        sw->rule_type = FAL_ACL_RULE_IP6;
    }

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_copy(a_uint32_t dev_id, a_uint32_t src_slct_idx,
                     a_uint32_t dst_slct_idx, a_uint32_t size)
{
    sw_error_t rv;
    a_uint32_t i;
    a_int32_t step, src_idx, dst_idx;
    shiva_acl_hw_rule_t rule;

    if (dst_slct_idx <= src_slct_idx)
    {
        src_idx = src_slct_idx & 0x7fffffff;
        dst_idx = dst_slct_idx & 0x7fffffff;
        step = 1;
    }
    else
    {
        src_idx = (src_slct_idx + size - 1) & 0x7fffffff;
        dst_idx = (dst_slct_idx + size - 1) & 0x7fffffff;
        step = -1;
    }

    aos_mem_zero(&rule, sizeof (shiva_acl_hw_rule_t));
    for (i = 0; i < size; i++)
    {
        rv = _shiva_acl_rule_invalid(dev_id, (a_uint32_t) dst_idx, 1);
        SW_RTN_ON_ERROR(rv);

        rv = _shiva_acl_action_read(dev_id, &rule, (a_uint32_t) src_idx);
        SW_RTN_ON_ERROR(rv);

        rv = _shiva_acl_action_write(dev_id, &rule, (a_uint32_t) dst_idx);
        SW_RTN_ON_ERROR(rv);

        rv = _shiva_acl_slct_read(dev_id, &rule, (a_uint32_t) src_idx);
        SW_RTN_ON_ERROR(rv);

        rv = _shiva_acl_slct_write(dev_id, &rule, (a_uint32_t) dst_idx);
        SW_RTN_ON_ERROR(rv);

        rv = _shiva_acl_rule_invalid(dev_id, (a_uint32_t) src_idx, 1);
        SW_RTN_ON_ERROR(rv);

        src_idx += step;
        dst_idx += step;
    }

    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_invalid(a_uint32_t dev_id, a_uint32_t rule_idx,
                        a_uint32_t size)
{
    sw_error_t rv;
    a_uint32_t base, flag, i;

    flag = 0;
    for (i = 0; i < size; i++)
    {
        base = SHIVA_RULE_SLCT_ADDR + ((rule_idx + i) << 5);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, base, sizeof (a_uint32_t),
                              (a_uint8_t *) (&flag), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_valid(a_uint32_t dev_id, a_uint32_t rule_idx, a_uint32_t size,
                      a_uint32_t flag)
{
    sw_error_t rv;
    a_uint32_t base, i;

    for (i = 0; i < size; i++)
    {
        base = SHIVA_RULE_SLCT_ADDR + ((rule_idx + i) << 5);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, base, sizeof (a_uint32_t),
                              (a_uint8_t *) (&flag), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }
    return SW_OK;
}

static sw_error_t
_shiva_acl_addr_update(a_uint32_t dev_id, a_uint32_t old_addr,
                       a_uint32_t new_addr, a_uint32_t list_id)
{
    sw_error_t rv;
    a_uint32_t idx;

    rv = _shiva_acl_list_loc(dev_id, list_id, &idx);
    SW_RTN_ON_ERROR(rv);

    if (old_addr != list_ent[dev_id][idx].addr)
    {
        return SW_FAIL;
    }

    list_ent[dev_id][idx].addr = new_addr;
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_bind(a_uint32_t dev_id, a_uint32_t rule_idx, a_uint32_t ports)
{
    sw_error_t rv;
    shiva_acl_hw_rule_t rule;

    aos_mem_zero(&rule, sizeof (shiva_acl_hw_rule_t));

    rv = _shiva_acl_slct_read(dev_id, &rule, rule_idx);
    SW_RTN_ON_ERROR(rv);

    rv = _shiva_acl_rule_invalid(dev_id, rule_idx, 1);
    SW_RTN_ON_ERROR(rv);

    SW_SET_REG_BY_FIELD(RUL_SLCT5, SRC_PT, ports, rule.slct[5]);

    rv = _shiva_acl_slct_write(dev_id, &rule, rule_idx);
    SW_RTN_ON_ERROR(rv);

    rv = _shiva_acl_rule_valid(dev_id, rule_idx, 1, rule.slct[0]);
    return rv;
}

static sw_error_t
_shiva_acl_list_creat(a_uint32_t dev_id, a_uint32_t list_id,
                      a_uint32_t list_pri)
{
    a_uint32_t i, loc = SHIVA_MAX_LIST;

    HSL_DEV_ID_CHECK(dev_id);

    for (i = 0; i < SHIVA_MAX_LIST; i++)
    {
        if ((ENT_USED == list_ent[dev_id][i].status)
                && (list_id == list_ent[dev_id][i].list_id))
        {
            return SW_ALREADY_EXIST;
        }

        if (ENT_FREE == list_ent[dev_id][i].status)
        {
            loc = i;
        }
    }

    if (SHIVA_MAX_LIST == loc)
    {
        return SW_NO_RESOURCE;
    }

    aos_mem_zero(&(list_ent[dev_id][loc]), sizeof (shiva_acl_list_t));
    list_ent[dev_id][loc].list_id = list_id;
    list_ent[dev_id][loc].list_pri = list_pri;
    list_ent[dev_id][loc].status = ENT_USED;
    _shiva_acl_list_dump(dev_id);
    return SW_OK;
}

static sw_error_t
_shiva_acl_list_destroy(a_uint32_t dev_id, a_uint32_t list_id)
{
    a_uint32_t list_idx;

    HSL_DEV_ID_CHECK(dev_id);

    for (list_idx = 0; list_idx < SHIVA_MAX_LIST; list_idx++)
    {
        if ((ENT_USED == list_ent[dev_id][list_idx].status)
                && (list_id == list_ent[dev_id][list_idx].list_id))
        {
            break;
        }
    }

    if (list_idx >= SHIVA_MAX_LIST)
    {
        return SW_NOT_FOUND;
    }

    if (0 != list_ent[dev_id][list_idx].bind_pts)
    {
        return SW_NOT_SUPPORTED;
    }

    if (0 != list_ent[dev_id][list_idx].size)
    {
        return SW_NOT_SUPPORTED;
    }

    aos_mem_zero(&(list_ent[dev_id][list_idx]), sizeof (shiva_acl_list_t));
    list_ent[dev_id][list_idx].status = ENT_FREE;
    _shiva_acl_list_dump(dev_id);
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_add(a_uint32_t dev_id, a_uint32_t list_id,
                    a_uint32_t rule_id, a_uint32_t rule_nr,
                    fal_acl_rule_t * rule)
{
    sw_error_t rv;
    a_uint32_t hsl_f_rsc, list_new_size, list_addr;
    a_uint32_t list_pri, list_idx, load_addr, bind_pts;

    HSL_DEV_ID_CHECK(dev_id);

    if ((0 == rule_nr) || (NULL == rule))
    {
        return SW_BAD_PARAM;
    }

    rv = hsl_acl_free_rsc_get(dev_id, &hsl_f_rsc);
    SW_RTN_ON_ERROR(rv);
    if (hsl_f_rsc < rule_nr)
    {
        return SW_NO_RESOURCE;
    }

    rv = _shiva_acl_list_loc(dev_id, list_id, &list_idx);
    SW_RTN_ON_ERROR(rv);

    if (rule_id != list_ent[dev_id][list_idx].size)
    {
        return SW_ALREADY_EXIST;
    }
    bind_pts = list_ent[dev_id][list_idx].bind_pts;

    _shiva_acl_filter_snap(dev_id);

    /* parse rule entry and alloc rule resource */
    {
        a_uint32_t i, j;
        a_uint32_t ent_idx, tmp_ent_idx, flt_idx;

        aos_mem_zero(hw_rule_ent,
                     SHIVA_MAX_RULE * sizeof (shiva_acl_hw_rule_t));

        ent_idx = 0;
        for (i = 0; i < rule_nr; i++)
        {
            tmp_ent_idx = ent_idx;
            rv = _shiva_acl_rule_sw_to_hw(dev_id, &rule[i],
                                          &hw_rule_ent[ent_idx], &ent_idx);
            SW_RTN_ON_ERROR(rv);

            if (4 < (ent_idx - tmp_ent_idx))
            {
                return SW_NOT_SUPPORTED;
            }

            for (j = tmp_ent_idx; j < ent_idx; j++)
            {
                rv = _shiva_acl_filter_alloc(dev_id, &flt_idx);
                SW_RTN_ON_ERROR(rv);

                rv = _shiva_acl_slct_update(&hw_rule_ent[tmp_ent_idx],
                                            j - tmp_ent_idx, flt_idx);
                SW_RTN_ON_ERROR(rv);
            }
            SW_SET_REG_BY_FIELD(RUL_SLCT5, SRC_PT, bind_pts,
                                hw_rule_ent[tmp_ent_idx].slct[5]);
        }
    }

    /* alloc hardware select entry resource */
    if (0 == list_ent[dev_id][list_idx].size)
    {
        list_new_size = rule_nr;
        list_pri = list_ent[dev_id][list_idx].list_pri;

        rv = hsl_acl_blk_alloc(dev_id, list_pri, list_new_size, list_id,
                               &list_addr);
        SW_RTN_ON_ERROR(rv);

        load_addr = list_addr;
    }
    else
    {
        list_new_size = list_ent[dev_id][list_idx].size + rule_nr;
        list_addr = list_ent[dev_id][list_idx].addr;

        rv = hsl_acl_blk_resize(dev_id, list_addr, list_new_size);
        SW_RTN_ON_ERROR(rv);

        /* must be careful resize opration maybe change list base address */
        list_addr = list_ent[dev_id][list_idx].addr;
        load_addr = list_ent[dev_id][list_idx].size + list_addr;
    }

    /* load acl rule to hardware */
    rv = _shiva_acl_rule_set(dev_id, load_addr, hw_rule_ent, rule_nr);
    if (SW_OK != rv)
    {
        (void) hsl_acl_blk_resize(dev_id, list_addr,
                                  list_ent[dev_id][list_idx].size);
        return rv;
    }

    /* update software list control information */
    list_ent[dev_id][list_idx].size = list_new_size;
    list_ent[dev_id][list_idx].addr = list_addr;

    /* update hardware acl rule resource information */
    _shiva_acl_filter_commit(dev_id);
    _shiva_acl_list_dump(dev_id);
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_delete(a_uint32_t dev_id, a_uint32_t list_id,
                       a_uint32_t rule_id, a_uint32_t rule_nr)
{
    sw_error_t rv;
    a_uint32_t flt_idx[4] = {0};
    a_uint32_t i, j, flt_nr;
    a_uint32_t list_idx = 0, addr, size, rule_idx, cnt;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _shiva_acl_list_loc(dev_id, list_id, &list_idx);
    SW_RTN_ON_ERROR(rv);

    if (0 == rule_nr)
    {
        return SW_BAD_PARAM;
    }

    if ((rule_id + rule_nr) > list_ent[dev_id][list_idx].size)
    {
        return SW_NOT_FOUND;
    }

    _shiva_acl_filter_snap(dev_id);

    /* free hardware filter resource */
    addr = list_ent[dev_id][list_idx].addr + rule_id;
    for (i = 0; i < rule_nr; i++)
    {
        rv = _shiva_acl_slct_read(dev_id, &hw_rule_ent[0], i + addr);
        SW_RTN_ON_ERROR(rv);

        rv = _shiva_acl_filter_map_get(&hw_rule_ent[0], flt_idx, &flt_nr);
        SW_RTN_ON_ERROR(rv);

        for (j = 0; j < flt_nr; j++)
        {
            _shiva_acl_filter_free(dev_id, flt_idx[j]);
        }
    }

    cnt = list_ent[dev_id][list_idx].size - (rule_id + rule_nr);
    rule_idx = list_ent[dev_id][list_idx].addr + (rule_id + rule_nr);
    rv = _shiva_acl_rule_copy(dev_id, rule_idx, rule_idx - rule_nr, cnt);
    SW_RTN_ON_ERROR(rv);

    addr = list_ent[dev_id][list_idx].addr;
    size = list_ent[dev_id][list_idx].size;
    rv = hsl_acl_blk_resize(dev_id, addr, size - rule_nr);
    SW_RTN_ON_ERROR(rv);

    list_ent[dev_id][list_idx].size -= rule_nr;
    _shiva_acl_filter_commit(dev_id);
    _shiva_acl_list_dump(dev_id);
    return SW_OK;
}

static sw_error_t
_shiva_acl_rule_query(a_uint32_t dev_id, a_uint32_t list_id,
                      a_uint32_t rule_id, fal_acl_rule_t * rule)
{
    sw_error_t rv;
    a_uint32_t list_idx, ent_idx, tmp_ent_idx, rule_idx;

    HSL_DEV_ID_CHECK(dev_id);

    rv = _shiva_acl_list_loc(dev_id, list_id, &list_idx);
    SW_RTN_ON_ERROR(rv);

    if (rule_id >= list_ent[dev_id][list_idx].size)
    {
        return SW_NOT_FOUND;
    }

    aos_mem_zero(rule, sizeof (fal_acl_rule_t));

    ent_idx = 0;
    tmp_ent_idx = 0;
    rule_idx = list_ent[dev_id][list_idx].addr + rule_id;
    rv = _shiva_acl_rule_get(dev_id, hw_rule_ent, &tmp_ent_idx, rule_idx);
    SW_RTN_ON_ERROR(rv);

    rv = _shiva_acl_rule_hw_to_sw(rule, hw_rule_ent, ent_idx,
                                  tmp_ent_idx - ent_idx);
    return rv;
}

static sw_error_t
_shiva_acl_list_bind(a_uint32_t dev_id, a_uint32_t list_id,
                     fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                     a_uint32_t obj_idx)
{
    sw_error_t rv;
    a_uint32_t i, list_idx, rule_idx, base, ports;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_ACL_DIREC_IN != direc)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_ACL_BIND_PORT != obj_t)
    {
        return SW_NOT_SUPPORTED;
    }

    rv = _shiva_acl_list_loc(dev_id, list_id, &list_idx);
    SW_RTN_ON_ERROR(rv);

    if (list_ent[dev_id][list_idx].bind_pts & (0x1 << obj_idx))
    {
        return SW_ALREADY_EXIST;
    }

    base = list_ent[dev_id][list_idx].addr;
    ports = list_ent[dev_id][list_idx].bind_pts | (0x1 << obj_idx);
    for (i = 0; i < list_ent[dev_id][list_idx].size; i++)
    {
        rule_idx = base + i;
        rv = _shiva_acl_rule_bind(dev_id, rule_idx, ports);
        SW_RTN_ON_ERROR(rv);
    }

    list_ent[dev_id][list_idx].bind_pts = ports;
    return SW_OK;
}

static sw_error_t
_shiva_acl_list_unbind(a_uint32_t dev_id, a_uint32_t list_id,
                       fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                       a_uint32_t obj_idx)
{
    sw_error_t rv;
    a_uint32_t i, list_idx, rule_idx, base, ports;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_ACL_DIREC_IN != direc)
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_ACL_BIND_PORT != obj_t)
    {
        return SW_NOT_SUPPORTED;
    }

    rv = _shiva_acl_list_loc(dev_id, list_id, &list_idx);
    SW_RTN_ON_ERROR(rv);

    if (!(list_ent[dev_id][list_idx].bind_pts & (0x1 << obj_idx)))
    {
        return SW_NOT_FOUND;
    }

    base = list_ent[dev_id][list_idx].addr;
    ports = list_ent[dev_id][list_idx].bind_pts & (~(0x1UL << obj_idx));
    for (i = 0; i < list_ent[dev_id][list_idx].size; i++)
    {
        rule_idx = base + i;
        rv = _shiva_acl_rule_bind(dev_id, rule_idx, ports);
        SW_RTN_ON_ERROR(rv);
    }

    list_ent[dev_id][list_idx].bind_pts = ports;
    return SW_OK;
}

static sw_error_t
_shiva_acl_status_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, ACL_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_shiva_acl_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val = 0;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, QM_CTL, 0, ACL_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
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

HSL_LOCAL sw_error_t
shiva_acl_list_dump(a_uint32_t dev_id)
{
    a_uint32_t idx;

    aos_printk("\nshiva_acl_list_dump:\n");
    for (idx = 0; idx < SHIVA_MAX_LIST; idx++)
    {
        if (ENT_USED == list_ent[dev_id][idx].status)
        {
            aos_printk
            ("\n[id]:%02d  [pri]:%02d  [size]:%02d  [addr]:%02d  [pts_map]:0x%02x",
             list_ent[dev_id][idx].list_id, list_ent[dev_id][idx].list_pri,
             list_ent[dev_id][idx].size, list_ent[dev_id][idx].addr,
             list_ent[dev_id][idx].bind_pts);
        }
    }
    aos_printk("\n");

    return SW_OK;
}

HSL_LOCAL sw_error_t
shiva_acl_rule_dump(a_uint32_t dev_id)
{
    a_uint32_t slt_idx, flt_nr, i, j;
    a_uint32_t flt_idx[4] = {0};
    sw_error_t rv;
    shiva_acl_hw_rule_t rule;

    aos_printk("\nshiva_acl_rule_dump:\n");

    aos_printk("\nfilter_bitmap:0x%x", filter[dev_id]);
    for (slt_idx = 0; slt_idx < SHIVA_MAX_RULE; slt_idx++)
    {
        aos_mem_zero(&rule, sizeof (shiva_acl_hw_rule_t));

        rv = _shiva_acl_slct_read(dev_id, &rule, slt_idx);
        if (SW_OK != rv)
        {
            continue;
        }

        rv = _shiva_acl_filter_map_get(&rule, flt_idx, &flt_nr);
        if (SW_OK != rv)
        {
            continue;
        }

        aos_printk("\nslct_idx=%d  ", slt_idx);
        for (i = 0; i < flt_nr; i++)
        {
            aos_printk("flt%d_idx=%d  ", i, flt_idx[i]);
        }

        aos_printk("\nslt:");
        for (i = 0; i < (sizeof(rule.slct)/sizeof(a_uint32_t)); i++)
        {
            aos_printk("%08x  ", rule.slct[i]);
        }

        if (flt_nr)
        {
            rv = _shiva_acl_action_read(dev_id, &rule, slt_idx);
            if (SW_OK != rv)
            {
                continue;
            }

            aos_printk("\nact:");
            for (i = 0; i < (sizeof(rule.act)/sizeof(a_uint32_t)); i++)
            {
                aos_printk("%08x  ", rule.act[i]);
            }

            for (i = 0; i < flt_nr; i++)
            {
                rv = _shiva_acl_filter_read(dev_id, &rule, flt_idx[i]);
                if (SW_OK != rv)
                {
                    continue;
                }

                aos_printk("\ntyp:%08x  length:%08x", rule.typ, rule.len);
                aos_printk("\nvlu:");
                for (j = 0; j < (sizeof(rule.vlu)/sizeof(a_uint32_t)); j++)
                {
                    aos_printk("%08x  ", rule.vlu[j]);
                }

                aos_printk("\nmsk:");
                for (j = 0; j < (sizeof(rule.msk)/sizeof(a_uint32_t)); j++)
                {
                    aos_printk("%08x  ", rule.msk[j]);
                }
                aos_printk("\n");
            }
        }
        aos_printk("\n");
    }

    return SW_OK;
}

sw_error_t
shiva_acl_reset(a_uint32_t dev_id)
{
    sw_error_t rv;
    a_uint32_t i;

    HSL_DEV_ID_CHECK(dev_id);

    aos_mem_zero(hw_rule_ent,
                 (SHIVA_MAX_RULE + 3) * sizeof (shiva_acl_hw_rule_t));

    aos_mem_zero(list_ent[dev_id],
                 SHIVA_MAX_LIST * sizeof (shiva_acl_list_t));

    for (i = 0; i < SHIVA_MAX_LIST; i++)
    {
        list_ent[dev_id][i].status = ENT_FREE;
    }

    filter[dev_id]      = 0;
    filter_snap[dev_id] = 0;

    rv = hsl_acl_pool_destroy(dev_id);
    SW_RTN_ON_ERROR(rv);

    rv = hsl_acl_pool_creat(dev_id, SHIVA_MAX_LIST, SHIVA_MAX_RULE);
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

/**
 * @brief Creat an acl list
 * @details  Comments:
  *     If the priority of a list is more small then the priority is more high,
 *     that means the list could be first matched.
 * @param[in] dev_id device id
 * @param[in] list_id acl list id
 * @param[in] list_pri acl list priority
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_acl_list_creat(a_uint32_t dev_id, a_uint32_t list_id,
                     a_uint32_t list_pri)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_acl_list_creat(dev_id, list_id, list_pri);
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
shiva_acl_list_destroy(a_uint32_t dev_id, a_uint32_t list_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_acl_list_destroy(dev_id, list_id);
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
HSL_LOCAL sw_error_t
shiva_acl_rule_add(a_uint32_t dev_id, a_uint32_t list_id,
                   a_uint32_t rule_id, a_uint32_t rule_nr,
                   fal_acl_rule_t * rule)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_acl_rule_add(dev_id, list_id, rule_id, rule_nr, rule);
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
HSL_LOCAL sw_error_t
shiva_acl_rule_delete(a_uint32_t dev_id, a_uint32_t list_id,
                      a_uint32_t rule_id, a_uint32_t rule_nr)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_acl_rule_delete(dev_id, list_id, rule_id, rule_nr);
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
HSL_LOCAL sw_error_t
shiva_acl_rule_query(a_uint32_t dev_id, a_uint32_t list_id,
                     a_uint32_t rule_id, fal_acl_rule_t * rule)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_acl_rule_query(dev_id, list_id, rule_id, rule);
    HSL_API_UNLOCK;
    return rv;
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
HSL_LOCAL sw_error_t
shiva_acl_list_bind(a_uint32_t dev_id, a_uint32_t list_id,
                    fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                    a_uint32_t obj_idx)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_acl_list_bind(dev_id, list_id, direc, obj_t, obj_idx);
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
HSL_LOCAL sw_error_t
shiva_acl_list_unbind(a_uint32_t dev_id, a_uint32_t list_id,
                      fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                      a_uint32_t obj_idx)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_acl_list_unbind(dev_id, list_id, direc, obj_t, obj_idx);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set working status of ACL engine on a particular device
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_acl_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_acl_status_set(dev_id, enable);
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
shiva_acl_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _shiva_acl_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
shiva_acl_init(a_uint32_t dev_id)
{
    static a_bool_t b_hw_rule = A_FALSE;
    hsl_acl_func_t *acl_func;
    shiva_acl_hw_rule_t rule;
    sw_error_t rv;
    a_uint32_t i;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == b_hw_rule)
    {
        hw_rule_ent = (shiva_acl_hw_rule_t *)
                      aos_mem_alloc((SHIVA_MAX_RULE +
                                     3) * sizeof (shiva_acl_hw_rule_t));
        if (NULL == hw_rule_ent)
        {
            return SW_NO_RESOURCE;
        }
        aos_mem_zero(hw_rule_ent,
                     (SHIVA_MAX_RULE + 3) * sizeof (shiva_acl_hw_rule_t));
        b_hw_rule = A_TRUE;
    }

    list_ent[dev_id] = (shiva_acl_list_t *)
                       aos_mem_alloc(SHIVA_MAX_LIST * sizeof (shiva_acl_list_t));
    if (NULL == list_ent[dev_id])
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(list_ent[dev_id],
                 SHIVA_MAX_LIST * sizeof (shiva_acl_list_t));

    for (i = 0; i < SHIVA_MAX_LIST; i++)
    {
        list_ent[dev_id][i].status = ENT_FREE;
    }

    filter[dev_id] = 0;
    filter_snap[dev_id] = 0;

    rv = hsl_acl_pool_creat(dev_id, SHIVA_MAX_LIST, SHIVA_MAX_RULE);
    SW_RTN_ON_ERROR(rv);

    acl_func = hsl_acl_ptr_get(dev_id);
    SW_RTN_ON_NULL(acl_func);

    acl_func->acl_rule_copy = _shiva_acl_rule_copy;
    acl_func->acl_rule_invalid = _shiva_acl_rule_invalid;
    acl_func->acl_addr_update = _shiva_acl_addr_update;

    /* zero acl hardware memory */
    aos_mem_zero(&rule, sizeof (shiva_acl_hw_rule_t));
    for (i = 0; i < SHIVA_MAX_RULE; i++)
    {
        rv = _shiva_acl_slct_write(dev_id, &rule, i);
        SW_RTN_ON_ERROR(rv);
    }

#ifdef SHIVA_SW_ENTRY
    flt_vlu_mem = aos_mem_alloc(SHIVA_MAX_RULE * 32);
    if (NULL == flt_vlu_mem)
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(flt_vlu_mem, SHIVA_MAX_RULE * 32);

    flt_msk_mem = aos_mem_alloc(SHIVA_MAX_RULE * 32);
    if (NULL == flt_msk_mem)
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(flt_msk_mem, SHIVA_MAX_RULE * 32);

    flt_typ_mem = aos_mem_alloc(SHIVA_MAX_RULE * 4);
    if (NULL == flt_typ_mem)
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(flt_typ_mem, SHIVA_MAX_RULE * 4);

    flt_len_mem = aos_mem_alloc(SHIVA_MAX_RULE * 4);
    if (NULL == flt_len_mem)
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(flt_len_mem, SHIVA_MAX_RULE * 4);

    act_mem = aos_mem_alloc(SHIVA_MAX_RULE * 32);
    if (NULL == act_mem)
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(act_mem, SHIVA_MAX_RULE * 32);

    slct_mem = aos_mem_alloc(SHIVA_MAX_RULE * 32);
    if (NULL == slct_mem)
    {
        return SW_NO_RESOURCE;
    }
    aos_mem_zero(slct_mem, SHIVA_MAX_RULE * 32);
#endif

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->acl_list_creat = shiva_acl_list_creat;
        p_api->acl_list_destroy = shiva_acl_list_destroy;
        p_api->acl_list_bind = shiva_acl_list_bind;
        p_api->acl_list_unbind = shiva_acl_list_unbind;
        p_api->acl_rule_add = shiva_acl_rule_add;
        p_api->acl_rule_delete = shiva_acl_rule_delete;
        p_api->acl_rule_query = shiva_acl_rule_query;
        p_api->acl_status_set = shiva_acl_status_set;
        p_api->acl_status_get = shiva_acl_status_get;
        p_api->acl_list_dump = shiva_acl_list_dump;
        p_api->acl_rule_dump = shiva_acl_rule_dump;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */

