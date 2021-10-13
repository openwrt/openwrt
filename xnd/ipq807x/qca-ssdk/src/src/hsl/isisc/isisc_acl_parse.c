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
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_acl.h"
#include "isisc_acl.h"
#include "isisc_reg.h"
#include "isisc_acl_prv.h"

#define DAH  0x1
#define SAH  0x2
#define TAG  0x4
#define STAG 0x8
#define CTAG 0x10

typedef sw_error_t(*parse_func_t) (fal_acl_rule_t * sw,
                                   hw_filter_t * hw_filter_snap,
                                   a_bool_t * b_care);

static a_bool_t
_isisc_acl_zero_addr(const fal_mac_addr_t addr)
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
_isisc_acl_field_care(fal_acl_field_op_t op, a_uint32_t val, a_uint32_t mask,
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
_isisc_acl_rule_bmac_parse(fal_acl_rule_t * sw,
                          hw_filter_t * hw, a_bool_t * b_care)
{
    a_uint32_t i;

    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));

    FIELD_SET_MASK(MAC_RUL_M4, RULE_TYP, ISISC_MAC_FILTER);

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_DA))
    {
        if (A_TRUE != _isisc_acl_zero_addr(sw->dest_mac_mask))
        {
            *b_care = A_TRUE;
        }

        for (i = 0; i < 6; i++)
        {
            sw->dest_mac_val.uc[i] &= sw->dest_mac_mask.uc[i];
        }

        FIELD_SET(MAC_RUL_V0, DAV_BYTE2, sw->dest_mac_val.uc[2]);
        FIELD_SET(MAC_RUL_V0, DAV_BYTE3, sw->dest_mac_val.uc[3]);
        FIELD_SET(MAC_RUL_V0, DAV_BYTE4, sw->dest_mac_val.uc[4]);
        FIELD_SET(MAC_RUL_V0, DAV_BYTE5, sw->dest_mac_val.uc[5]);
        FIELD_SET(MAC_RUL_V1, DAV_BYTE0, sw->dest_mac_val.uc[0]);
        FIELD_SET(MAC_RUL_V1, DAV_BYTE1, sw->dest_mac_val.uc[1]);

        FIELD_SET_MASK(MAC_RUL_M0, DAM_BYTE2, sw->dest_mac_mask.uc[2]);
        FIELD_SET_MASK(MAC_RUL_M0, DAM_BYTE3, sw->dest_mac_mask.uc[3]);
        FIELD_SET_MASK(MAC_RUL_M0, DAM_BYTE4, sw->dest_mac_mask.uc[4]);
        FIELD_SET_MASK(MAC_RUL_M0, DAM_BYTE5, sw->dest_mac_mask.uc[5]);
        FIELD_SET_MASK(MAC_RUL_M1, DAM_BYTE0, sw->dest_mac_mask.uc[0]);
        FIELD_SET_MASK(MAC_RUL_M1, DAM_BYTE1, sw->dest_mac_mask.uc[1]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_SA))
    {
        if (A_TRUE != _isisc_acl_zero_addr(sw->src_mac_mask))
        {
            *b_care = A_TRUE;
        }

        for (i = 0; i < 6; i++)
        {
            sw->src_mac_val.uc[i] &= sw->src_mac_mask.uc[i];
        }

        FIELD_SET(MAC_RUL_V1, SAV_BYTE4, sw->src_mac_val.uc[4]);
        FIELD_SET(MAC_RUL_V1, SAV_BYTE5, sw->src_mac_val.uc[5]);
        FIELD_SET(MAC_RUL_V2, SAV_BYTE0, sw->src_mac_val.uc[0]);
        FIELD_SET(MAC_RUL_V2, SAV_BYTE1, sw->src_mac_val.uc[1]);
        FIELD_SET(MAC_RUL_V2, SAV_BYTE2, sw->src_mac_val.uc[2]);
        FIELD_SET(MAC_RUL_V2, SAV_BYTE3, sw->src_mac_val.uc[3]);

        FIELD_SET_MASK(MAC_RUL_M1, SAM_BYTE4, sw->src_mac_mask.uc[4]);
        FIELD_SET_MASK(MAC_RUL_M1, SAM_BYTE5, sw->src_mac_mask.uc[5]);
        FIELD_SET_MASK(MAC_RUL_M2, SAM_BYTE0, sw->src_mac_mask.uc[0]);
        FIELD_SET_MASK(MAC_RUL_M2, SAM_BYTE1, sw->src_mac_mask.uc[1]);
        FIELD_SET_MASK(MAC_RUL_M2, SAM_BYTE2, sw->src_mac_mask.uc[2]);
        FIELD_SET_MASK(MAC_RUL_M2, SAM_BYTE3, sw->src_mac_mask.uc[3]);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_ETHTYPE))
    {
        if (0x0 != sw->ethtype_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ethtype_val &= sw->ethtype_mask;
        FIELD_SET(MAC_RUL_V3, ETHTYPV, sw->ethtype_val);
        FIELD_SET_MASK(MAC_RUL_M3, ETHTYPM, sw->ethtype_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_TAGGED))
    {
        if (0x0 != sw->tagged_mask)
        {
            *b_care = A_TRUE;
        }

        sw->tagged_val &= sw->tagged_mask;
        FIELD_SET_MASK(MAC_RUL_M4, TAGGEDV, sw->tagged_val);
        FIELD_SET_MASK(MAC_RUL_M4, TAGGEDM, sw->tagged_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_UP))
    {
        if (0x0 != sw->up_mask)
        {
            *b_care = A_TRUE;
        }

        sw->up_val &= sw->up_mask;
        FIELD_SET(MAC_RUL_V3, VLANPRIV, sw->up_val);
        FIELD_SET_MASK(MAC_RUL_M3, VLANPRIM, sw->up_mask);
    }

    FIELD_SET_MASK(MAC_RUL_M4, VIDMSK, 1);
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
                _isisc_acl_field_care(sw->vid_op, sw->vid_val, sw->vid_mask,
                                     0xfff))
        {
            *b_care = A_TRUE;
        }

        FIELD_SET_MASK(MAC_RUL_M4, VIDMSK, 0);
        if (FAL_ACL_FIELD_MASK == sw->vid_op)
        {
            sw->vid_val &= sw->vid_mask;
            FIELD_SET(MAC_RUL_V3, VLANIDV, sw->vid_val);
            FIELD_SET_MASK(MAC_RUL_M3, VLANIDM, sw->vid_mask);
            FIELD_SET_MASK(MAC_RUL_M4, VIDMSK, 1);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->vid_op)
        {
            FIELD_SET(MAC_RUL_V3, VLANIDV, sw->vid_val);
            FIELD_SET_MASK(MAC_RUL_M3, VLANIDM, sw->vid_mask);
        }
        else if (FAL_ACL_FIELD_LE == sw->vid_op)
        {
            FIELD_SET(MAC_RUL_V3, VLANIDV, 0);
            FIELD_SET_MASK(MAC_RUL_M3, VLANIDM, sw->vid_val);
        }
        else
        {
            FIELD_SET(MAC_RUL_V3, VLANIDV, sw->vid_val);
            FIELD_SET_MASK(MAC_RUL_M3, VLANIDM, 0xfff);
        }
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CFI))
    {
        if (0x0 != sw->cfi_mask)
        {
            *b_care = A_TRUE;
        }

        sw->cfi_val &= sw->cfi_mask;
        FIELD_SET(MAC_RUL_V3, VLANCFIV, sw->cfi_val);
        FIELD_SET_MASK(MAC_RUL_M3, VLANCFIM, sw->cfi_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL))
    {
        FIELD_SET(MAC_RUL_V4, RULE_INV, 1);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_ehmac_parse(fal_acl_rule_t * sw,
                           hw_filter_t * hw, a_bool_t * b_care)
{
    a_uint32_t i;
    a_bool_t da_h = A_FALSE, sa_h = A_FALSE;

    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));

    FIELD_SET_MASK(MAC_RUL_M4, RULE_TYP, ISISC_EHMAC_FILTER);

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_DA))
    {
        for (i = 0; i < 3; i++)
        {
            if (sw->dest_mac_mask.uc[i])
            {
                da_h = A_TRUE;
                break;
            }
        }
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_SA))
    {
        for (i = 0; i < 3; i++)
        {
            if (sw->src_mac_mask.uc[i])
            {
                sa_h = A_TRUE;
                break;
            }
        }
    }

    /* if sa_h and da_h both are true need't process mac address fileds */
    if ((A_TRUE == da_h) && ((A_TRUE == sa_h)))
    {
        da_h = A_FALSE;
        sa_h = A_FALSE;
    }

    if (A_TRUE == da_h)
    {
        FIELD_SET(EHMAC_RUL_V3, DA_EN, 1);

        if (A_TRUE != _isisc_acl_zero_addr(sw->dest_mac_mask))
        {
            *b_care = A_TRUE;
        }

        for (i = 0; i < 6; i++)
        {
            sw->dest_mac_val.uc[i] &= sw->dest_mac_mask.uc[i];
        }

        FIELD_SET(EHMAC_RUL_V0, DAV_BYTE2, sw->dest_mac_val.uc[2]);
        FIELD_SET(EHMAC_RUL_V0, DAV_BYTE3, sw->dest_mac_val.uc[3]);
        FIELD_SET(EHMAC_RUL_V0, DAV_BYTE4, sw->dest_mac_val.uc[4]);
        FIELD_SET(EHMAC_RUL_V0, DAV_BYTE5, sw->dest_mac_val.uc[5]);
        FIELD_SET(EHMAC_RUL_V1, DAV_BYTE0, sw->dest_mac_val.uc[0]);
        FIELD_SET(EHMAC_RUL_V1, DAV_BYTE1, sw->dest_mac_val.uc[1]);

        FIELD_SET_MASK(EHMAC_RUL_M0, DAM_BYTE2, sw->dest_mac_mask.uc[2]);
        FIELD_SET_MASK(EHMAC_RUL_M0, DAM_BYTE3, sw->dest_mac_mask.uc[3]);
        FIELD_SET_MASK(EHMAC_RUL_M0, DAM_BYTE4, sw->dest_mac_mask.uc[4]);
        FIELD_SET_MASK(EHMAC_RUL_M0, DAM_BYTE5, sw->dest_mac_mask.uc[5]);
        FIELD_SET_MASK(EHMAC_RUL_M1, DAM_BYTE0, sw->dest_mac_mask.uc[0]);
        FIELD_SET_MASK(EHMAC_RUL_M1, DAM_BYTE1, sw->dest_mac_mask.uc[1]);

        if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_SA))
        {
            if (A_TRUE != _isisc_acl_zero_addr(sw->src_mac_mask))
            {
                *b_care = A_TRUE;
            }

            for (i = 0; i < 6; i++)
            {
                sw->src_mac_val.uc[i] &= sw->src_mac_mask.uc[i];
            }

            FIELD_SET(EHMAC_RUL_V2, SAV_BYTE3, sw->src_mac_val.uc[3]);
            FIELD_SET(EHMAC_RUL_V1, SAV_BYTE4, sw->src_mac_val.uc[4]);
            FIELD_SET(EHMAC_RUL_V1, SAV_BYTE5, sw->src_mac_val.uc[5]);

            FIELD_SET_MASK(EHMAC_RUL_M2, SAM_BYTE3, sw->src_mac_mask.uc[3]);
            FIELD_SET_MASK(EHMAC_RUL_M1, SAM_BYTE4, sw->src_mac_mask.uc[4]);
            FIELD_SET_MASK(EHMAC_RUL_M1, SAM_BYTE5, sw->src_mac_mask.uc[5]);
        }
    }

    if (A_TRUE == sa_h)
    {
        if (A_TRUE != _isisc_acl_zero_addr(sw->src_mac_mask))
        {
            *b_care = A_TRUE;
        }

        for (i = 0; i < 6; i++)
        {
            sw->src_mac_val.uc[i] &= sw->src_mac_mask.uc[i];
        }

        FIELD_SET(EHMAC_RUL_V0, DAV_BYTE2, sw->src_mac_val.uc[2]);
        FIELD_SET(EHMAC_RUL_V0, DAV_BYTE3, sw->src_mac_val.uc[3]);
        FIELD_SET(EHMAC_RUL_V0, DAV_BYTE4, sw->src_mac_val.uc[4]);
        FIELD_SET(EHMAC_RUL_V0, DAV_BYTE5, sw->src_mac_val.uc[5]);
        FIELD_SET(EHMAC_RUL_V1, DAV_BYTE0, sw->src_mac_val.uc[0]);
        FIELD_SET(EHMAC_RUL_V1, DAV_BYTE1, sw->src_mac_val.uc[1]);

        FIELD_SET_MASK(EHMAC_RUL_M0, DAM_BYTE2, sw->src_mac_mask.uc[2]);
        FIELD_SET_MASK(EHMAC_RUL_M0, DAM_BYTE3, sw->src_mac_mask.uc[3]);
        FIELD_SET_MASK(EHMAC_RUL_M0, DAM_BYTE4, sw->src_mac_mask.uc[4]);
        FIELD_SET_MASK(EHMAC_RUL_M0, DAM_BYTE5, sw->src_mac_mask.uc[5]);
        FIELD_SET_MASK(EHMAC_RUL_M1, DAM_BYTE0, sw->src_mac_mask.uc[0]);
        FIELD_SET_MASK(EHMAC_RUL_M1, DAM_BYTE1, sw->src_mac_mask.uc[1]);

        if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_DA))
        {
            if (A_TRUE != _isisc_acl_zero_addr(sw->dest_mac_mask))
            {
                *b_care = A_TRUE;
            }

            for (i = 0; i < 6; i++)
            {
                sw->dest_mac_val.uc[i] &= sw->dest_mac_mask.uc[i];
            }

            FIELD_SET(EHMAC_RUL_V2, SAV_BYTE3, sw->dest_mac_val.uc[3]);
            FIELD_SET(EHMAC_RUL_V1, SAV_BYTE4, sw->dest_mac_val.uc[4]);
            FIELD_SET(EHMAC_RUL_V1, SAV_BYTE5, sw->dest_mac_val.uc[5]);

            FIELD_SET_MASK(EHMAC_RUL_M2, SAM_BYTE3, sw->dest_mac_mask.uc[3]);
            FIELD_SET_MASK(EHMAC_RUL_M1, SAM_BYTE4, sw->dest_mac_mask.uc[4]);
            FIELD_SET_MASK(EHMAC_RUL_M1, SAM_BYTE5, sw->dest_mac_mask.uc[5]);
        }
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_ETHTYPE))
    {
        if (0x0 != sw->ethtype_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ethtype_val &= sw->ethtype_mask;
        FIELD_SET(EHMAC_RUL_V3, ETHTYPV, sw->ethtype_val);
        FIELD_SET_MASK(EHMAC_RUL_M3, ETHTYPM, sw->ethtype_mask);
    }

    /* Process Stag Fields */
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_STAGGED))
    {
        if (0x0 != sw->stagged_mask)
        {
            *b_care = A_TRUE;
        }

        sw->stagged_val &= sw->stagged_mask;
        FIELD_SET(EHMAC_RUL_V3, STAGGEDV, sw->stagged_val);
        FIELD_SET(EHMAC_RUL_V3, STAGGEDM, sw->stagged_mask);
    }

    FIELD_SET(EHMAC_RUL_V3, SVIDMSK, 1);
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_STAG_VID))
    {
        if ((FAL_ACL_FIELD_MASK != sw->stag_vid_op)
                && (FAL_ACL_FIELD_RANGE != sw->stag_vid_op)
                && (FAL_ACL_FIELD_LE != sw->stag_vid_op)
                && (FAL_ACL_FIELD_GE != sw->stag_vid_op))
        {
            return SW_NOT_SUPPORTED;
        }

        if (A_TRUE ==
                _isisc_acl_field_care(sw->stag_vid_op, sw->stag_vid_val,
                                     sw->stag_vid_mask, 0xfff))
        {
            *b_care = A_TRUE;
        }

        FIELD_SET(EHMAC_RUL_V3, SVIDMSK, 0);
        if (FAL_ACL_FIELD_MASK == sw->stag_vid_op)
        {
            sw->stag_vid_val &= sw->stag_vid_mask;
            FIELD_SET(EHMAC_RUL_V2, STAG_VIDV, sw->stag_vid_val);
            FIELD_SET_MASK(EHMAC_RUL_M2, STAG_VIDM, sw->stag_vid_mask);
            FIELD_SET(EHMAC_RUL_V3, SVIDMSK, 1);

        }
        else if (FAL_ACL_FIELD_RANGE == sw->stag_vid_op)
        {
            FIELD_SET(EHMAC_RUL_V2, STAG_VIDV, sw->stag_vid_val);
            FIELD_SET_MASK(EHMAC_RUL_M2, STAG_VIDM, sw->stag_vid_mask);

        }
        else if (FAL_ACL_FIELD_LE == sw->stag_vid_op)
        {
            FIELD_SET(EHMAC_RUL_V2, STAG_VIDV, 0);
            FIELD_SET_MASK(EHMAC_RUL_M2, STAG_VIDM, sw->stag_vid_val);

        }
        else
        {
            FIELD_SET(EHMAC_RUL_V2, STAG_VIDV, sw->stag_vid_val);
            FIELD_SET_MASK(EHMAC_RUL_M2, STAG_VIDM, 0xfff);
        }
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_STAG_PRI))
    {
        if (0x0 != sw->stag_pri_mask)
        {
            *b_care = A_TRUE;
        }

        sw->stag_pri_val &= sw->stag_pri_mask;
        FIELD_SET(EHMAC_RUL_V2, STAG_PRIV, sw->stag_pri_val);
        FIELD_SET_MASK(EHMAC_RUL_M2, STAG_PRIM, sw->stag_pri_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_STAG_DEI))
    {
        if (0x0 != sw->stag_dei_mask)
        {
            *b_care = A_TRUE;
        }

        sw->stag_dei_val &= sw->stag_dei_mask;
        FIELD_SET(EHMAC_RUL_V2, STAG_DEIV, sw->stag_dei_val);
        FIELD_SET_MASK(EHMAC_RUL_M2, STAG_DEIM, sw->stag_dei_mask);
    }

    /* Process Ctag Fields */
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CTAGGED))
    {
        if (0x0 != sw->ctagged_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ctagged_val &= sw->ctagged_mask;
        FIELD_SET_MASK(EHMAC_RUL_M4, CTAGGEDV, sw->ctagged_val);
        FIELD_SET_MASK(EHMAC_RUL_M4, CTAGGEDM, sw->ctagged_mask);
    }

    FIELD_SET_MASK(EHMAC_RUL_M4, CVIDMSK, 1);
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CTAG_VID))
    {
        if ((FAL_ACL_FIELD_MASK != sw->ctag_vid_op)
                && (FAL_ACL_FIELD_RANGE != sw->ctag_vid_op)
                && (FAL_ACL_FIELD_LE != sw->ctag_vid_op)
                && (FAL_ACL_FIELD_GE != sw->ctag_vid_op))
        {
            return SW_NOT_SUPPORTED;
        }

        if (A_TRUE ==
                _isisc_acl_field_care(sw->ctag_vid_op, sw->ctag_vid_val,
                                     sw->ctag_vid_mask, 0xfff))
        {
            *b_care = A_TRUE;
        }

        FIELD_SET_MASK(EHMAC_RUL_M4, CVIDMSK, 0);
        if (FAL_ACL_FIELD_MASK == sw->ctag_vid_op)
        {
            sw->ctag_vid_val &= sw->ctag_vid_mask;
            FIELD_SET(EHMAC_RUL_V2, CTAG_VIDLV, sw->ctag_vid_val);
            FIELD_SET(EHMAC_RUL_V3, CTAG_VIDHV, (sw->ctag_vid_val >> 8));
            FIELD_SET_MASK(EHMAC_RUL_M2, CTAG_VIDLM, sw->ctag_vid_mask);
            FIELD_SET_MASK(EHMAC_RUL_M3, CTAG_VIDHM, (sw->ctag_vid_mask >> 8));
            FIELD_SET_MASK(EHMAC_RUL_M4, CVIDMSK, 1);

        }
        else if (FAL_ACL_FIELD_RANGE == sw->ctag_vid_op)
        {
            FIELD_SET(EHMAC_RUL_V2, CTAG_VIDLV, sw->ctag_vid_val);
            FIELD_SET(EHMAC_RUL_V3, CTAG_VIDHV, (sw->ctag_vid_val >> 8));
            FIELD_SET_MASK(EHMAC_RUL_M2, CTAG_VIDLM, sw->ctag_vid_mask);
            FIELD_SET_MASK(EHMAC_RUL_M3, CTAG_VIDHM, (sw->ctag_vid_mask >> 8));

        }
        else if (FAL_ACL_FIELD_LE == sw->ctag_vid_op)
        {
            FIELD_SET(EHMAC_RUL_V2, CTAG_VIDLV, 0);
            FIELD_SET(EHMAC_RUL_V3, CTAG_VIDHV, 0);
            FIELD_SET_MASK(EHMAC_RUL_M2, CTAG_VIDLM, sw->ctag_vid_val);
            FIELD_SET_MASK(EHMAC_RUL_M3, CTAG_VIDHM, (sw->ctag_vid_val >> 8));

        }
        else
        {
            FIELD_SET(EHMAC_RUL_V2, CTAG_VIDLV, sw->ctag_vid_val);
            FIELD_SET(EHMAC_RUL_V3, CTAG_VIDHV, (sw->ctag_vid_val >> 8));
            FIELD_SET_MASK(EHMAC_RUL_M2, CTAG_VIDLM, 0xff);
            FIELD_SET_MASK(EHMAC_RUL_M3, CTAG_VIDHM, 0xf);
        }
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CTAG_PRI))
    {
        if (0x0 != sw->ctag_pri_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ctag_pri_val &= sw->ctag_pri_mask;
        FIELD_SET(EHMAC_RUL_V3, CTAG_PRIV, sw->ctag_pri_val);
        FIELD_SET_MASK(EHMAC_RUL_M3, CTAG_PRIM, sw->ctag_pri_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CTAG_CFI))
    {
        if (0x0 != sw->ctag_cfi_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ctag_cfi_val &= sw->ctag_cfi_mask;
        FIELD_SET(EHMAC_RUL_V3, CTAG_CFIV, sw->ctag_cfi_val);
        FIELD_SET_MASK(EHMAC_RUL_M3, CTAG_CFIM, sw->ctag_cfi_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL))
    {
        FIELD_SET(MAC_RUL_V4, RULE_INV, 1);
    }

    return SW_OK;
}

static void
_isisc_acl_rule_mac_preparse(fal_acl_rule_t * sw, a_bool_t * b_mac,
                            a_bool_t * eh_mac)
{
    a_uint32_t bm = 0, i, tmp;

    *b_mac = A_FALSE;
    *eh_mac = A_FALSE;
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_DA))
    {
        for (i = 0; i < 3; i++)
        {
            if (sw->dest_mac_mask.uc[i])
            {
                bm |= DAH;
                break;
            }
        }
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_SA))
    {
        for (i = 0; i < 3; i++)
        {
            if (sw->src_mac_mask.uc[i])
            {
                bm |= SAH;
                break;
            }
        }
    }

    tmp = 0;
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_TAGGED))
    {
        tmp |= ((sw->tagged_mask & 0x1) << 16);
    }
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_UP))
    {
        tmp |= ((sw->up_mask & 0x7) << 13);
    }
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CFI))
    {
        tmp |= ((sw->cfi_mask & 0x1) << 12);
    }
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_VID))
    {
        if (A_TRUE ==
                _isisc_acl_field_care(sw->vid_op, sw->vid_val, sw->vid_mask,
                                     0xfff))
        {
            tmp |= 0xfff;
        }
    }
    if (tmp)
    {
        bm |= TAG;
    }

    tmp = 0;
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_STAGGED))
    {
        tmp |= ((sw->stagged_mask & 0x1) << 16);
    }
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_STAG_PRI))
    {
        tmp |= ((sw->stag_pri_mask & 0x7) << 13);
    }
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_STAG_DEI))
    {
        tmp |= ((sw->stag_dei_mask & 0x1) << 12);
    }
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_STAG_VID))
    {
        if (A_TRUE ==
                _isisc_acl_field_care(sw->stag_vid_op, sw->stag_vid_val,
                                     sw->stag_vid_mask, 0xfff))
        {
            tmp |= 0xfff;
        }
    }
    if (tmp)
    {
        bm |= STAG;
    }

    tmp = 0;
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CTAGGED))
    {
        tmp |= ((sw->ctagged_mask & 0x1) << 16);
    }
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CTAG_PRI))
    {
        tmp |= ((sw->ctag_pri_mask & 0x7) << 13);
    }
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CTAG_CFI))
    {
        tmp |= ((sw->ctag_cfi_mask & 0x1) << 12);
    }
    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_MAC_CTAG_VID))
    {
        if (A_TRUE ==
                _isisc_acl_field_care(sw->ctag_vid_op, sw->ctag_vid_val,
                                     sw->ctag_vid_mask, 0xfff))
        {
            tmp |= 0xfff;
        }
    }
    if (tmp)
    {
        bm |= CTAG;
    }

    if ((bm & CTAG) || (bm & STAG))
    {
        *eh_mac = A_TRUE;
    }

    if ((bm & TAG) || ((bm & DAH) && (bm & SAH)))
    {
        *b_mac = A_TRUE;
    }
}

static sw_error_t
_isisc_acl_rule_ip4_parse(fal_acl_rule_t * sw,
                         hw_filter_t * hw, a_bool_t * b_care)
{
    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));

    FIELD_SET_MASK(MAC_RUL_M4, RULE_TYP, ISISC_IP4_FILTER);

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP_DSCP))
    {
        if (0x0 != sw->ip_dscp_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip_dscp_val &= sw->ip_dscp_mask;
        FIELD_SET(IP4_RUL_V2, IP4DSCPV, sw->ip_dscp_val);
        FIELD_SET_MASK(IP4_RUL_M2, IP4DSCPM, sw->ip_dscp_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP_PROTO))
    {
        if (0x0 != sw->ip_proto_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip_proto_val &= sw->ip_proto_mask;
        FIELD_SET(IP4_RUL_V2, IP4PROTV, sw->ip_proto_val);
        FIELD_SET_MASK(IP4_RUL_M2, IP4PROTM, sw->ip_proto_mask);
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

    if ((FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_L4_SPORT))
            && ((FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_ICMP_TYPE))
                || (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_ICMP_CODE))))
    {
        return SW_BAD_PARAM;
    }

    FIELD_SET_MASK(IP4_RUL_M3, IP4SPORTM_EN, 1);
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
                _isisc_acl_field_care(sw->src_l4port_op, sw->src_l4port_val,
                                     sw->src_l4port_mask, 0xffff))
        {
            if (FAL_ACL_FIELD_MASK == sw->src_l4port_op)
            {
                sw->src_l4port_op = FAL_ACL_FIELD_RANGE;
                sw->src_l4port_val = 0;
                sw->src_l4port_mask = 0xffff;
            }
        }
        *b_care = A_TRUE;

        FIELD_SET_MASK(IP4_RUL_M3, IP4SPORTM_EN, 0);
        if (FAL_ACL_FIELD_MASK == sw->src_l4port_op)
        {
            sw->src_l4port_val &= sw->src_l4port_mask;
            FIELD_SET(IP4_RUL_V3, IP4SPORTV, sw->src_l4port_val);
            FIELD_SET_MASK(IP4_RUL_M3, IP4SPORTM, sw->src_l4port_mask);
            FIELD_SET_MASK(IP4_RUL_M3, IP4SPORTM_EN, 1);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->src_l4port_op)
        {
            FIELD_SET(IP4_RUL_V3, IP4SPORTV, sw->src_l4port_val);
            FIELD_SET_MASK(IP4_RUL_M3, IP4SPORTM, sw->src_l4port_mask);
        }
        else if (FAL_ACL_FIELD_LE == sw->src_l4port_op)
        {
            FIELD_SET(IP4_RUL_V3, IP4SPORTV, 0);
            FIELD_SET_MASK(IP4_RUL_M3, IP4SPORTM, sw->src_l4port_val);
        }
        else
        {
            FIELD_SET(IP4_RUL_V3, IP4SPORTV, sw->src_l4port_val);
            FIELD_SET_MASK(IP4_RUL_M3, IP4SPORTM, 0xffff);
        }
    }

    FIELD_SET_MASK(IP4_RUL_M3, IP4DPORTM_EN, 1);
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
                _isisc_acl_field_care(sw->dest_l4port_op, sw->dest_l4port_val,
                                     sw->dest_l4port_mask, 0xffff))
        {
            if (FAL_ACL_FIELD_MASK == sw->dest_l4port_op)
            {
                sw->dest_l4port_op = FAL_ACL_FIELD_RANGE;
                sw->dest_l4port_val = 0;
                sw->dest_l4port_mask = 0xffff;
            }
        }
        *b_care = A_TRUE;

        FIELD_SET_MASK(IP4_RUL_M3, IP4DPORTM_EN, 0);
        if (FAL_ACL_FIELD_MASK == sw->dest_l4port_op)
        {
            sw->dest_l4port_val &= sw->dest_l4port_mask;
            FIELD_SET(IP4_RUL_V2, IP4DPORTV, sw->dest_l4port_val);
            FIELD_SET_MASK(IP4_RUL_M2, IP4DPORTM, sw->dest_l4port_mask);
            FIELD_SET_MASK(IP4_RUL_M3, IP4DPORTM_EN, 1);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->dest_l4port_op)
        {
            FIELD_SET(IP4_RUL_V2, IP4DPORTV, sw->dest_l4port_val);
            FIELD_SET_MASK(IP4_RUL_M2, IP4DPORTM, sw->dest_l4port_mask);
        }
        else if (FAL_ACL_FIELD_LE == sw->dest_l4port_op)
        {
            FIELD_SET(IP4_RUL_V2, IP4DPORTV, 0);
            FIELD_SET_MASK(IP4_RUL_M2, IP4DPORTM, sw->dest_l4port_val);
        }
        else
        {
            FIELD_SET(IP4_RUL_V2, IP4DPORTV, sw->dest_l4port_val);
            FIELD_SET_MASK(IP4_RUL_M2, IP4DPORTM, 0xffff);
        }
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_ICMP_TYPE))
    {
        if (0x0 != sw->icmp_type_mask)
        {
            *b_care = A_TRUE;
        }
        FIELD_SET(IP4_RUL_V3, ICMP_EN, 1);

        sw->icmp_type_val &= sw->icmp_type_mask;
        FIELD_SET(IP4_RUL_V3, IP4ICMPTYPV, sw->icmp_type_val);
        FIELD_SET_MASK(IP4_RUL_M3, IP4ICMPTYPM, sw->icmp_type_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_ICMP_CODE))
    {
        if (0x0 != sw->icmp_code_mask)
        {
            *b_care = A_TRUE;
        }
        FIELD_SET(IP4_RUL_V3, ICMP_EN, 1);

        sw->icmp_code_val &= sw->icmp_code_mask;
        FIELD_SET(IP4_RUL_V3, IP4ICMPCODEV, sw->icmp_code_val);
        FIELD_SET_MASK(IP4_RUL_M3, IP4ICMPCODEM, sw->icmp_code_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_TCP_FLAG))
    {
        if (0x0 != sw->tcp_flag_mask)
        {
            *b_care = A_TRUE;
        }

        sw->tcp_flag_val &= sw->tcp_flag_mask;
        FIELD_SET(IP4_RUL_V3, IP4TCPFLAGV, sw->tcp_flag_val);
        FIELD_SET_MASK(IP4_RUL_M3, IP4TCPFLAGM, sw->tcp_flag_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_RIPV1))
    {
        if (0x0 != sw->ripv1_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ripv1_val &= sw->ripv1_mask;
        FIELD_SET(IP4_RUL_V3, IP4RIPV, sw->ripv1_val);
        FIELD_SET_MASK(IP4_RUL_M3, IP4RIPM, sw->ripv1_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_DHCPV4))
    {
        if (0x0 != sw->dhcpv4_mask)
        {
            *b_care = A_TRUE;
        }

        sw->dhcpv4_val &= sw->dhcpv4_mask;
        FIELD_SET(IP4_RUL_V3, IP4DHCPV, sw->dhcpv4_val);
        FIELD_SET_MASK(IP4_RUL_M3, IP4DHCPM, sw->dhcpv4_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL))
    {
        FIELD_SET(MAC_RUL_V4, RULE_INV, 1);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_ip6r1_parse(fal_acl_rule_t * sw,
                           hw_filter_t * hw, a_bool_t * b_care)
{
    a_uint32_t i;

    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));

    FIELD_SET_MASK(MAC_RUL_M4, RULE_TYP, ISISC_IP6R1_FILTER);

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

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL))
    {
        FIELD_SET(MAC_RUL_V4, RULE_INV, 1);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_ip6r2_parse(fal_acl_rule_t * sw,
                           hw_filter_t * hw, a_bool_t * b_care)
{
    a_uint32_t i;

    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));

    FIELD_SET_MASK(MAC_RUL_M4, RULE_TYP, ISISC_IP6R2_FILTER);

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

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL))
    {
        FIELD_SET(MAC_RUL_V4, RULE_INV, 1);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_ip6r3_parse(fal_acl_rule_t * sw,
                           hw_filter_t * hw, a_bool_t * b_care)
{
    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));

    FIELD_SET_MASK(MAC_RUL_M4, RULE_TYP, ISISC_IP6R3_FILTER);

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP6_LABEL))
    {
        if (0x0 != sw->ip6_lable_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip6_lable_val &= sw->ip6_lable_mask;
        FIELD_SET(IP6_RUL3_V1, IP6LABEL1V, sw->ip6_lable_val);
        FIELD_SET_MASK(IP6_RUL3_M1, IP6LABEL1M, sw->ip6_lable_mask);

        FIELD_SET(IP6_RUL3_V2, IP6LABEL2V, (sw->ip6_lable_val >> 16));
        FIELD_SET_MASK(IP6_RUL3_M2, IP6LABEL2M, (sw->ip6_lable_mask >> 16));
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP_PROTO))
    {
        if (0x0 != sw->ip_proto_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip_proto_val &= sw->ip_proto_mask;
        FIELD_SET(IP6_RUL3_V0, IP6PROTV, sw->ip_proto_val);
        FIELD_SET_MASK(IP6_RUL3_M0, IP6PROTM, sw->ip_proto_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_IP_DSCP))
    {
        if (0x0 != sw->ip_dscp_mask)
        {
            *b_care = A_TRUE;
        }

        sw->ip_dscp_val &= sw->ip_dscp_mask;
        FIELD_SET(IP6_RUL3_V0, IP6DSCPV, sw->ip_dscp_val);
        FIELD_SET_MASK(IP6_RUL3_M0, IP6DSCPM, sw->ip_dscp_mask);
    }

    if ((FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_L4_SPORT))
            && ((FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_ICMP_TYPE))
                || (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_ICMP_CODE))))
    {
        return SW_BAD_PARAM;
    }

    FIELD_SET_MASK(IP6_RUL3_M3, IP6SPORTM_EN, 1);
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
                _isisc_acl_field_care(sw->src_l4port_op, sw->src_l4port_val,
                                     sw->src_l4port_mask, 0xffff))
        {
            if (FAL_ACL_FIELD_MASK == sw->src_l4port_op)
            {
                sw->src_l4port_op = FAL_ACL_FIELD_RANGE;
                sw->src_l4port_val = 0;
                sw->src_l4port_mask = 0xffff;
            }
        }
        *b_care = A_TRUE;

        FIELD_SET_MASK(IP6_RUL3_M3, IP6SPORTM_EN, 0);
        if (FAL_ACL_FIELD_MASK == sw->src_l4port_op)
        {
            sw->src_l4port_val &= sw->src_l4port_mask;
            FIELD_SET(IP6_RUL3_V3, IP6SPORTV, sw->src_l4port_val);
            FIELD_SET_MASK(IP6_RUL3_M3, IP6SPORTM, sw->src_l4port_mask);
            FIELD_SET_MASK(IP6_RUL3_M3, IP6SPORTM_EN, 1);

        }
        else if (FAL_ACL_FIELD_RANGE == sw->src_l4port_op)
        {
            FIELD_SET(IP6_RUL3_V3, IP6SPORTV, sw->src_l4port_val);
            FIELD_SET_MASK(IP6_RUL3_M3, IP6SPORTM, sw->src_l4port_mask);

        }
        else if (FAL_ACL_FIELD_LE == sw->src_l4port_op)
        {
            FIELD_SET(IP6_RUL3_V3, IP6SPORTV, 0);
            FIELD_SET_MASK(IP6_RUL3_M3, IP6SPORTM, sw->src_l4port_val);

        }
        else
        {
            FIELD_SET(IP6_RUL3_V3, IP6SPORTV, sw->src_l4port_val);
            FIELD_SET_MASK(IP6_RUL3_M3, IP6SPORTM, 0xffff);
        }
    }

    FIELD_SET_MASK(IP6_RUL3_M3, IP6DPORTM_EN, 1);
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
                _isisc_acl_field_care(sw->dest_l4port_op, sw->dest_l4port_val,
                                     sw->dest_l4port_mask, 0xffff))
        {
            if (FAL_ACL_FIELD_MASK == sw->dest_l4port_op)
            {
                sw->dest_l4port_op = FAL_ACL_FIELD_RANGE;
                sw->dest_l4port_val = 0;
                sw->dest_l4port_mask = 0xffff;
            }
        }
        *b_care = A_TRUE;

        FIELD_SET_MASK(IP6_RUL3_M3, IP6DPORTM_EN, 0);
        if (FAL_ACL_FIELD_MASK == sw->dest_l4port_op)
        {
            sw->dest_l4port_val &= sw->dest_l4port_mask;
            FIELD_SET(IP6_RUL3_V2, IP6DPORTV, sw->dest_l4port_val);
            FIELD_SET_MASK(IP6_RUL3_M2, IP6DPORTM, sw->dest_l4port_mask);
            FIELD_SET_MASK(IP6_RUL3_M3, IP6DPORTM_EN, 1);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->dest_l4port_op)
        {
            FIELD_SET(IP6_RUL3_V2, IP6DPORTV, sw->dest_l4port_val);
            FIELD_SET_MASK(IP6_RUL3_M2, IP6DPORTM, sw->dest_l4port_mask);
        }
        else if (FAL_ACL_FIELD_LE == sw->dest_l4port_op)
        {
            FIELD_SET(IP6_RUL3_V2, IP6DPORTV, 0);
            FIELD_SET_MASK(IP6_RUL3_M2, IP6DPORTM, sw->dest_l4port_val);
        }
        else
        {
            FIELD_SET(IP6_RUL3_V2, IP6DPORTV, sw->dest_l4port_val);
            FIELD_SET_MASK(IP6_RUL3_M2, IP6DPORTM, 0xffff);
        }
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_ICMP_TYPE))
    {
        if (0x0 != sw->icmp_type_mask)
        {
            *b_care = A_TRUE;
        }
        FIELD_SET(IP6_RUL3_V3, ICMP6_EN, 1);

        sw->icmp_type_val &= sw->icmp_type_mask;
        FIELD_SET(IP6_RUL3_V3, IP6ICMPTYPV, sw->icmp_type_val);
        FIELD_SET_MASK(IP6_RUL3_M3, IP6ICMPTYPM, sw->icmp_type_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_ICMP_CODE))
    {
        if (0x0 != sw->icmp_code_mask)
        {
            *b_care = A_TRUE;
        }
        FIELD_SET(IP6_RUL3_V3, ICMP6_EN, 1);

        sw->icmp_code_val &= sw->icmp_code_mask;
        FIELD_SET(IP6_RUL3_V3, IP6ICMPCODEV, sw->icmp_code_val);
        FIELD_SET_MASK(IP6_RUL3_M3, IP6ICMPCODEM, sw->icmp_code_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_TCP_FLAG))
    {
        if (0x0 != sw->tcp_flag_mask)
        {
            *b_care = A_TRUE;
        }

        sw->tcp_flag_val &= sw->tcp_flag_mask;
        FIELD_SET(IP6_RUL3_V3, IP6TCPFLAGV, sw->tcp_flag_val);
        FIELD_SET_MASK(IP6_RUL3_M3, IP6TCPFLAGM, sw->tcp_flag_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_DHCPV6))
    {
        if (0x0 != sw->dhcpv6_mask)
        {
            *b_care = A_TRUE;
        }

        sw->dhcpv6_val &= sw->dhcpv6_mask;
        FIELD_SET(IP6_RUL3_V3, IP6DHCPV, sw->dhcpv6_val);
        FIELD_SET_MASK(IP6_RUL3_M3, IP6DHCPM, sw->dhcpv6_mask);
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL))
    {
        FIELD_SET(MAC_RUL_V4, RULE_INV, 1);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_udf_parse(fal_acl_rule_t * sw,
                         hw_filter_t * hw, a_bool_t * b_care)
{
    a_uint32_t i;

    *b_care = A_FALSE;
    aos_mem_zero(&(hw->vlu[0]), sizeof (hw->vlu));
    aos_mem_zero(&(hw->msk[0]), sizeof (hw->msk));

    FIELD_SET_MASK(MAC_RUL_M4, RULE_TYP, ISISC_UDF_FILTER);

    if (!FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_UDF))
    {
        if (FAL_ACL_RULE_UDF == sw->rule_type)
        {
            if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL))
            {
                FIELD_SET(MAC_RUL_V4, RULE_INV, 1);
            }
            *b_care = A_TRUE;
        }
        return SW_OK;
    }

    if (ISISC_MAX_UDF_LENGTH < sw->udf_len)
    {
        return SW_NOT_SUPPORTED;
    }

    *b_care = A_TRUE;
    for (i = 0; i < sw->udf_len; i++)
    {
        hw->vlu[3 - i / 4] |=
            ((sw->udf_mask[i] & sw->udf_val[i]) << (24 - 8 * (i % 4)));
        hw->msk[3 - i / 4] |= ((sw->udf_mask[i]) << (24 - 8 * (i % 4)));
    }

    if (FAL_FIELD_FLG_TST(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL))
    {
        FIELD_SET(MAC_RUL_V4, RULE_INV, 1);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_action_parse(a_uint32_t dev_id, const fal_acl_rule_t * sw,
                       hw_filter_t * hw)
{
    fal_pbmp_t des_pts;

    aos_mem_zero(&(hw->act[0]), sizeof (hw->act));

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_MATCH_TRIGGER_INTR))
    {
        FIELD_SET_ACTION(ACL_RSLT2, TRIGGER_INTR, 1);
    }

    /* FAL_ACL_ACTION_PERMIT need't process */

    if ((FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_RDTCPU))
            && (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_CPYCPU)))
    {
        return SW_BAD_PARAM;
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_RDTCPU))
    {
        FIELD_SET_ACTION(ACL_RSLT2, FWD_CMD, 0x3);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_CPYCPU))
    {
        FIELD_SET_ACTION(ACL_RSLT2, FWD_CMD, 0x1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_DENY))
    {
        FIELD_SET_ACTION(ACL_RSLT2, FWD_CMD, 0x7);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_MIRROR))
    {
        FIELD_SET_ACTION(ACL_RSLT2, MIRR_EN, 1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REDPT))
    {
        FIELD_SET_ACTION(ACL_RSLT2, DES_PORT_EN, 1);

        des_pts = (sw->ports >> 3) & 0xf;
        FIELD_SET_ACTION(ACL_RSLT2, DES_PORT1, des_pts);

        des_pts = sw->ports & 0x7;
        FIELD_SET_ACTION(ACL_RSLT1, DES_PORT0, des_pts);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_UP))
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_QUEUE))
    {
        FIELD_SET_ACTION(ACL_RSLT1, PRI_QU_EN, 1);
        FIELD_SET_ACTION(ACL_RSLT1, PRI_QU, sw->queue);
    }

    if ((FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_MODIFY_VLAN))
            || (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_NEST_VLAN)))
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_DSCP))
    {
        FIELD_SET_ACTION(ACL_RSLT1, DSCPV, sw->dscp);
        FIELD_SET_ACTION(ACL_RSLT1, DSCP_REMAP, 1);
    }

    FIELD_SET_ACTION(ACL_RSLT0, STAGVID, sw->stag_vid);
    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_STAG_VID))
    {
        FIELD_SET_ACTION(ACL_RSLT1, TRANS_SVID_CHG, 1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_STAG_PRI))
    {
        FIELD_SET_ACTION(ACL_RSLT0, STAGPRI, sw->stag_pri);
        FIELD_SET_ACTION(ACL_RSLT1, STAG_PRI_REMAP, 1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_STAG_DEI))
    {
        FIELD_SET_ACTION(ACL_RSLT0, STAGDEI, sw->stag_dei);
        FIELD_SET_ACTION(ACL_RSLT1, STAG_DEI_CHG, 1);
    }

    FIELD_SET_ACTION(ACL_RSLT0, CTAGVID, sw->ctag_vid);
    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_CTAG_VID))
    {
        FIELD_SET_ACTION(ACL_RSLT1, TRANS_CVID_CHG, 1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_CTAG_PRI))
    {
        FIELD_SET_ACTION(ACL_RSLT0, CTAGPRI, sw->ctag_pri);
        FIELD_SET_ACTION(ACL_RSLT1, CTAG_PRI_REMAP, 1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_CTAG_CFI))
    {
        FIELD_SET_ACTION(ACL_RSLT0, CTAGCFI, sw->ctag_cfi);
        FIELD_SET_ACTION(ACL_RSLT1, CTAG_CFI_CHG, 1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_REMARK_LOOKUP_VID))
    {
        FIELD_SET_ACTION(ACL_RSLT1, LOOK_VID_CHG, 1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_POLICER_EN))
    {
        FIELD_SET_ACTION(ACL_RSLT2, POLICER_PTR, sw->policer_ptr);
        FIELD_SET_ACTION(ACL_RSLT2, POLICER_EN, 1);
    }

    if ((FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_ARP_EN))
            && (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_WCMP_EN)))
    {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_ARP_EN))
    {
        FIELD_SET_ACTION(ACL_RSLT1, ARP_PTR, sw->arp_ptr);
        FIELD_SET_ACTION(ACL_RSLT1, ARP_PTR_EN, 1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_WCMP_EN))
    {
        FIELD_SET_ACTION(ACL_RSLT1, ARP_PTR, sw->wcmp_ptr);
        FIELD_SET_ACTION(ACL_RSLT1, WCMP_EN, 1);
        FIELD_SET_ACTION(ACL_RSLT1, ARP_PTR_EN, 1);
    }

    FIELD_SET_ACTION(ACL_RSLT1, FORCE_L3_MODE, 0x0);
    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_POLICY_FORWARD_EN))
    {
        if (FAL_ACL_POLICY_ROUTE == sw->policy_fwd)
        {
            return SW_NOT_SUPPORTED;
        }
        else if (FAL_ACL_POLICY_SNAT == sw->policy_fwd)
        {
            FIELD_SET_ACTION(ACL_RSLT1, FORCE_L3_MODE, 0x1);
        }
        else if (FAL_ACL_POLICY_DNAT == sw->policy_fwd)
        {
            FIELD_SET_ACTION(ACL_RSLT1, FORCE_L3_MODE, 0x2);
        }
        else if (FAL_ACL_POLICY_RESERVE == sw->policy_fwd)
        {
            FIELD_SET_ACTION(ACL_RSLT1, FORCE_L3_MODE, 0x3);
        }
        else
        {
            return SW_BAD_PARAM;
        }
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_BYPASS_EGRESS_TRANS))
    {
        FIELD_SET_ACTION(ACL_RSLT2, EG_BYPASS, 1);
    }

    if (FAL_ACTION_FLG_TST(sw->action_flg, FAL_ACL_ACTION_MATCH_TRIGGER_INTR))
    {
        FIELD_SET_ACTION(ACL_RSLT2, TRIGGER_INTR, 1);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_bmac_reparse(fal_acl_rule_t * sw, const hw_filter_t * hw)
{
    a_uint32_t mask_en;

    /* destnation mac address */
    FIELD_GET(MAC_RUL_V0, DAV_BYTE2, sw->dest_mac_val.uc[2]);
    FIELD_GET(MAC_RUL_V0, DAV_BYTE3, sw->dest_mac_val.uc[3]);
    FIELD_GET(MAC_RUL_V0, DAV_BYTE4, sw->dest_mac_val.uc[4]);
    FIELD_GET(MAC_RUL_V0, DAV_BYTE5, sw->dest_mac_val.uc[5]);
    FIELD_GET(MAC_RUL_V1, DAV_BYTE0, sw->dest_mac_val.uc[0]);
    FIELD_GET(MAC_RUL_V1, DAV_BYTE1, sw->dest_mac_val.uc[1]);

    FIELD_GET_MASK(MAC_RUL_M0, DAM_BYTE2, sw->dest_mac_mask.uc[2]);
    FIELD_GET_MASK(MAC_RUL_M0, DAM_BYTE3, sw->dest_mac_mask.uc[3]);
    FIELD_GET_MASK(MAC_RUL_M0, DAM_BYTE4, sw->dest_mac_mask.uc[4]);
    FIELD_GET_MASK(MAC_RUL_M0, DAM_BYTE5, sw->dest_mac_mask.uc[5]);
    FIELD_GET_MASK(MAC_RUL_M1, DAM_BYTE0, sw->dest_mac_mask.uc[0]);
    FIELD_GET_MASK(MAC_RUL_M1, DAM_BYTE1, sw->dest_mac_mask.uc[1]);
    if (A_FALSE == _isisc_acl_zero_addr(sw->dest_mac_mask))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_DA);
    }

    /* source mac address */
    FIELD_GET(MAC_RUL_V2, SAV_BYTE0, sw->src_mac_val.uc[0]);
    FIELD_GET(MAC_RUL_V2, SAV_BYTE1, sw->src_mac_val.uc[1]);
    FIELD_GET(MAC_RUL_V2, SAV_BYTE2, sw->src_mac_val.uc[2]);
    FIELD_GET(MAC_RUL_V2, SAV_BYTE3, sw->src_mac_val.uc[3]);
    FIELD_GET(MAC_RUL_V1, SAV_BYTE4, sw->src_mac_val.uc[4]);
    FIELD_GET(MAC_RUL_V1, SAV_BYTE5, sw->src_mac_val.uc[5]);

    FIELD_GET_MASK(MAC_RUL_M2, SAM_BYTE0, sw->src_mac_mask.uc[0]);
    FIELD_GET_MASK(MAC_RUL_M2, SAM_BYTE1, sw->src_mac_mask.uc[1]);
    FIELD_GET_MASK(MAC_RUL_M2, SAM_BYTE2, sw->src_mac_mask.uc[2]);
    FIELD_GET_MASK(MAC_RUL_M2, SAM_BYTE3, sw->src_mac_mask.uc[3]);
    FIELD_GET_MASK(MAC_RUL_M1, SAM_BYTE4, sw->src_mac_mask.uc[4]);
    FIELD_GET_MASK(MAC_RUL_M1, SAM_BYTE5, sw->src_mac_mask.uc[5]);
    if (A_FALSE == _isisc_acl_zero_addr(sw->src_mac_mask))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_SA);
    }

    /* ethernet type */
    FIELD_GET(MAC_RUL_V3, ETHTYPV, sw->ethtype_val);
    FIELD_GET_MASK(MAC_RUL_M3, ETHTYPM, sw->ethtype_mask);
    if (0x0 != sw->ethtype_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_ETHTYPE);
    }

    /* packet tagged */
    FIELD_GET_MASK(MAC_RUL_M4, TAGGEDV, sw->tagged_val);
    FIELD_GET_MASK(MAC_RUL_M4, TAGGEDM, sw->tagged_mask);
    if (0x0 != sw->tagged_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_TAGGED);
    }

    /* vlan priority */
    FIELD_GET(MAC_RUL_V3, VLANPRIV, sw->up_val);
    FIELD_GET_MASK(MAC_RUL_M3, VLANPRIM, sw->up_mask);
    if (0x0 != sw->up_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_UP);
    }

    /* vlanid */
    FIELD_GET(MAC_RUL_V3, VLANIDV, sw->vid_val);
    FIELD_GET_MASK(MAC_RUL_M3, VLANIDM, sw->vid_mask);
    FIELD_GET_MASK(MAC_RUL_M4, VIDMSK, mask_en);
    if (mask_en)
    {
        sw->vid_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->vid_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _isisc_acl_field_care(sw->vid_op, (a_uint32_t) sw->vid_val,
                                 (a_uint32_t) sw->vid_mask, 0xfff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_VID);
    }

    /* vlan cfi */
    FIELD_GET(MAC_RUL_V3, VLANCFIV, sw->cfi_val);
    FIELD_GET_MASK(MAC_RUL_M3, VLANCFIM, sw->cfi_mask);
    if (0x0 != sw->cfi_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_CFI);
    }

    FIELD_GET(MAC_RUL_V4, RULE_INV, mask_en);
    if (mask_en)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_ehmac_reparse(fal_acl_rule_t * sw, const hw_filter_t * hw)
{
    a_uint32_t i, mask_en, data;

    FIELD_GET(EHMAC_RUL_V3, DA_EN, data);
    if (data)
    {
        for (i = 2; i < 6; i++)
        {
            sw->dest_mac_val.uc[i] = ((hw->vlu[0]) >> ((5 - i) << 3)) & 0xff;
            sw->dest_mac_mask.uc[i] = ((hw->msk[0]) >> ((5 - i) << 3)) & 0xff;
        }

        for (i = 0; i < 2; i++)
        {
            sw->dest_mac_val.uc[i] = ((hw->vlu[1]) >> ((1 - i) << 3)) & 0xff;
            sw->dest_mac_mask.uc[i] = ((hw->msk[1]) >> ((1 - i) << 3)) & 0xff;
        }

        if (A_FALSE == _isisc_acl_zero_addr(sw->dest_mac_mask))
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_DA);
        }

        FIELD_GET(EHMAC_RUL_V2, SAV_BYTE3, sw->src_mac_val.uc[3]);
        FIELD_GET(EHMAC_RUL_V1, SAV_BYTE4, sw->src_mac_val.uc[4]);
        FIELD_GET(EHMAC_RUL_V1, SAV_BYTE5, sw->src_mac_val.uc[5]);

        FIELD_GET_MASK(EHMAC_RUL_M2, SAM_BYTE3, sw->src_mac_mask.uc[3]);
        FIELD_GET_MASK(EHMAC_RUL_M1, SAM_BYTE4, sw->src_mac_mask.uc[4]);
        FIELD_GET_MASK(EHMAC_RUL_M1, SAM_BYTE5, sw->src_mac_mask.uc[5]);

        if (A_FALSE == _isisc_acl_zero_addr(sw->src_mac_mask))
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_SA);
        }
    }
    else
    {
        for (i = 2; i < 6; i++)
        {
            sw->src_mac_val.uc[i] = ((hw->vlu[0]) >> ((5 - i) << 3)) & 0xff;
            sw->src_mac_mask.uc[i] = ((hw->msk[0]) >> ((5 - i) << 3)) & 0xff;
        }

        for (i = 0; i < 2; i++)
        {
            sw->src_mac_val.uc[i] = ((hw->vlu[1]) >> ((1 - i) << 3)) & 0xff;
            sw->src_mac_mask.uc[i] = ((hw->msk[1]) >> ((1 - i) << 3)) & 0xff;
        }

        if (A_FALSE == _isisc_acl_zero_addr(sw->src_mac_mask))
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_SA);
        }

        FIELD_GET(EHMAC_RUL_V2, SAV_BYTE3, sw->dest_mac_val.uc[3]);
        FIELD_GET(EHMAC_RUL_V1, SAV_BYTE4, sw->dest_mac_val.uc[4]);
        FIELD_GET(EHMAC_RUL_V1, SAV_BYTE5, sw->dest_mac_val.uc[5]);

        FIELD_GET_MASK(EHMAC_RUL_M2, SAM_BYTE3, sw->dest_mac_mask.uc[3]);
        FIELD_GET_MASK(EHMAC_RUL_M1, SAM_BYTE4, sw->dest_mac_mask.uc[4]);
        FIELD_GET_MASK(EHMAC_RUL_M1, SAM_BYTE5, sw->dest_mac_mask.uc[5]);
        if (A_FALSE == _isisc_acl_zero_addr(sw->dest_mac_mask))
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_DA);
        }
    }

    /* ethernet type */
    FIELD_GET(EHMAC_RUL_V3, ETHTYPV, sw->ethtype_val);
    FIELD_GET_MASK(EHMAC_RUL_M3, ETHTYPM, sw->ethtype_mask);
    if (0x0 != sw->ethtype_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_ETHTYPE);
    }

    /* packet stagged */
    FIELD_GET(EHMAC_RUL_V3, STAGGEDV, sw->stagged_val);
    FIELD_GET(EHMAC_RUL_V3, STAGGEDM, sw->stagged_mask);
    if (0x0 != sw->stagged_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_STAGGED);
    }

    /* stag vid */
    FIELD_GET(EHMAC_RUL_V2, STAG_VIDV, sw->stag_vid_val);
    FIELD_GET_MASK(EHMAC_RUL_M2, STAG_VIDM, sw->stag_vid_mask);
    FIELD_GET(EHMAC_RUL_V3, SVIDMSK, mask_en);
    if (mask_en)
    {
        sw->stag_vid_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->stag_vid_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _isisc_acl_field_care(sw->stag_vid_op, (a_uint32_t) sw->stag_vid_val,
                                 (a_uint32_t) sw->stag_vid_mask, 0xfff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_STAG_VID);
    }

    /* stag priority */
    FIELD_GET(EHMAC_RUL_V2, STAG_PRIV, sw->stag_pri_val);
    FIELD_GET_MASK(EHMAC_RUL_M2, STAG_PRIM, sw->stag_pri_mask);
    if (0x0 != sw->stag_pri_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_STAG_PRI);
    }

    /* stag dei */
    FIELD_GET(EHMAC_RUL_V2, STAG_DEIV, sw->stag_dei_val);
    FIELD_GET_MASK(EHMAC_RUL_M2, STAG_DEIM, sw->stag_dei_mask);
    if (0x0 != sw->stag_dei_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_STAG_DEI);
    }

    /* packet ctagged */
    FIELD_GET_MASK(EHMAC_RUL_M4, CTAGGEDV, sw->ctagged_val);
    FIELD_GET_MASK(EHMAC_RUL_M4, CTAGGEDM, sw->ctagged_mask);
    if (0x0 != sw->ctagged_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_CTAGGED);
    }

    /* ctag vid */
    FIELD_GET(EHMAC_RUL_V2, CTAG_VIDLV, sw->ctag_vid_val);
    FIELD_GET(EHMAC_RUL_V3, CTAG_VIDHV, data);
    sw->ctag_vid_val |= (data << 8);
    FIELD_GET_MASK(EHMAC_RUL_M2, CTAG_VIDLM, sw->ctag_vid_mask);
    FIELD_GET_MASK(EHMAC_RUL_M3, CTAG_VIDHM, data);
    sw->ctag_vid_mask |= (data << 8);

    FIELD_GET_MASK(EHMAC_RUL_M4, CVIDMSK, mask_en);
    if (mask_en)
    {
        sw->ctag_vid_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->ctag_vid_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _isisc_acl_field_care(sw->ctag_vid_op, (a_uint32_t) sw->ctag_vid_val,
                                 (a_uint32_t) sw->ctag_vid_mask, 0xfff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_CTAG_VID);
    }

    /* ctag priority */
    FIELD_GET(EHMAC_RUL_V3, CTAG_PRIV, sw->ctag_pri_val);
    FIELD_GET_MASK(EHMAC_RUL_M3, CTAG_PRIM, sw->ctag_pri_mask);
    if (0x0 != sw->ctag_pri_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_CTAG_PRI);
    }

    /* ctag dei */
    FIELD_GET(EHMAC_RUL_V3, CTAG_CFIV, sw->ctag_cfi_val);
    FIELD_GET_MASK(EHMAC_RUL_M3, CTAG_CFIM, sw->ctag_cfi_mask);
    if (0x0 != sw->ctag_cfi_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_MAC_CTAG_CFI);
    }

    FIELD_GET(MAC_RUL_V4, RULE_INV, mask_en);
    if (mask_en)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_ip4_reparse(fal_acl_rule_t * sw, const hw_filter_t * hw)
{
    a_uint32_t mask_en, icmp_en;

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

    FIELD_GET(IP4_RUL_V2, IP4PROTV, sw->ip_proto_val);
    FIELD_GET_MASK(IP4_RUL_M2, IP4PROTM, sw->ip_proto_mask);
    if (0x0 != sw->ip_proto_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP_PROTO);
    }

    FIELD_GET(IP4_RUL_V2, IP4DSCPV, sw->ip_dscp_val);
    FIELD_GET_MASK(IP4_RUL_M2, IP4DSCPM, sw->ip_dscp_mask);
    if (0x0 != sw->ip_dscp_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP_DSCP);
    }

    FIELD_GET(IP4_RUL_V2, IP4DPORTV, sw->dest_l4port_val);
    FIELD_GET_MASK(IP4_RUL_M2, IP4DPORTM, sw->dest_l4port_mask);
    FIELD_GET_MASK(IP4_RUL_M3, IP4DPORTM_EN, mask_en);
    if (mask_en)
    {
        sw->dest_l4port_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->dest_l4port_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _isisc_acl_field_care(sw->dest_l4port_op,
                                 (a_uint32_t) sw->dest_l4port_val,
                                 (a_uint32_t) sw->dest_l4port_mask, 0xffff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_DPORT);
    }
    else if (FAL_ACL_FIELD_RANGE == sw->dest_l4port_op)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_DPORT);
    }

    FIELD_GET(IP4_RUL_V3, ICMP_EN, icmp_en);
    if (icmp_en)
    {
        FIELD_GET(IP4_RUL_V3, IP4ICMPTYPV, sw->icmp_type_val);
        FIELD_GET_MASK(IP4_RUL_M3, IP4ICMPTYPM, sw->icmp_type_mask);
        if (0x0 != sw->icmp_type_mask)
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_ICMP_TYPE);
        }

        FIELD_GET(IP4_RUL_V3, IP4ICMPCODEV, sw->icmp_code_val);
        FIELD_GET_MASK(IP4_RUL_M3, IP4ICMPCODEM, sw->icmp_code_mask);
        if (0x0 != sw->icmp_code_mask)
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_ICMP_CODE);
        }
    }
    else
    {
        FIELD_GET(IP4_RUL_V3, IP4SPORTV, sw->src_l4port_val);
        FIELD_GET_MASK(IP4_RUL_M3, IP4SPORTM, sw->src_l4port_mask);
        FIELD_GET_MASK(IP4_RUL_M3, IP4SPORTM_EN, mask_en);
        if (mask_en)
        {
            sw->src_l4port_op = FAL_ACL_FIELD_MASK;
        }
        else
        {
            sw->src_l4port_op = FAL_ACL_FIELD_RANGE;
        }

        if (A_TRUE ==
                _isisc_acl_field_care(sw->src_l4port_op,
                                     (a_uint32_t) sw->src_l4port_val,
                                     (a_uint32_t) sw->src_l4port_mask, 0xffff))
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_SPORT);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->src_l4port_op)
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_SPORT);
        }
    }

    FIELD_GET(IP4_RUL_V3, IP4TCPFLAGV, sw->tcp_flag_val);
    FIELD_GET_MASK(IP4_RUL_M3, IP4TCPFLAGM, sw->tcp_flag_mask);
    if (0x0 != sw->tcp_flag_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_TCP_FLAG);
    }

    FIELD_GET(IP4_RUL_V3, IP4RIPV, sw->ripv1_val);
    FIELD_GET_MASK(IP4_RUL_M3, IP4RIPM, sw->ripv1_mask);
    if (0x0 != sw->ripv1_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_RIPV1);
    }

    FIELD_GET(IP4_RUL_V3, IP4DHCPV, sw->dhcpv4_val);
    FIELD_GET_MASK(IP4_RUL_M3, IP4DHCPM, sw->dhcpv4_mask);
    if (0x0 != sw->dhcpv4_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_DHCPV4);
    }

    FIELD_GET(MAC_RUL_V4, RULE_INV, mask_en);
    if (mask_en)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_ip6r1_reparse(fal_acl_rule_t * sw, const hw_filter_t * hw)
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

    FIELD_GET(MAC_RUL_V4, RULE_INV, i);
    if (i)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_ip6r2_reparse(fal_acl_rule_t * sw, const hw_filter_t * hw)
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

    FIELD_GET(MAC_RUL_V4, RULE_INV, i);
    if (i)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_ip6r3_reparse(fal_acl_rule_t * sw, const hw_filter_t * hw)
{
    a_uint32_t mask_en, icmp6_en, tmp;

    FIELD_GET(IP6_RUL3_V0, IP6PROTV, sw->ip_proto_val);
    FIELD_GET_MASK(IP6_RUL3_M0, IP6PROTM, sw->ip_proto_mask);
    if (0x0 != sw->ip_proto_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP_PROTO);
    }

    FIELD_GET(IP6_RUL3_V0, IP6DSCPV, sw->ip_dscp_val);
    FIELD_GET_MASK(IP6_RUL3_M0, IP6DSCPM, sw->ip_dscp_mask);
    if (0x0 != sw->ip_dscp_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP_DSCP);
    }

    FIELD_GET(IP6_RUL3_V2, IP6DPORTV, sw->dest_l4port_val);
    FIELD_GET_MASK(IP6_RUL3_M2, IP6DPORTM, sw->dest_l4port_mask);
    FIELD_GET_MASK(IP6_RUL3_M3, IP6DPORTM_EN, mask_en);
    if (mask_en)
    {
        sw->dest_l4port_op = FAL_ACL_FIELD_MASK;
    }
    else
    {
        sw->dest_l4port_op = FAL_ACL_FIELD_RANGE;
    }

    if (A_TRUE ==
            _isisc_acl_field_care(sw->dest_l4port_op,
                                 (a_uint32_t) sw->dest_l4port_val,
                                 (a_uint32_t) sw->dest_l4port_mask, 0xffff))
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_DPORT);
    }
    else if (FAL_ACL_FIELD_RANGE == sw->dest_l4port_op)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_DPORT);
    }

    FIELD_GET(IP6_RUL3_V3, ICMP6_EN, icmp6_en);
    if (icmp6_en)
    {
        FIELD_GET(IP6_RUL3_V3, IP6ICMPTYPV, sw->icmp_type_val);
        FIELD_GET_MASK(IP6_RUL3_M3, IP6ICMPTYPM, sw->icmp_type_mask);
        if (0x0 != sw->icmp_type_mask)
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_ICMP_TYPE);
        }

        FIELD_GET(IP6_RUL3_V3, IP6ICMPCODEV, sw->icmp_code_val);
        FIELD_GET_MASK(IP6_RUL3_M3, IP6ICMPCODEM, sw->icmp_code_mask);
        if (0x0 != sw->icmp_code_mask)
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_ICMP_CODE);
        }
    }
    else
    {
        FIELD_GET(IP6_RUL3_V3, IP6SPORTV, sw->src_l4port_val);
        FIELD_GET_MASK(IP6_RUL3_M3, IP6SPORTM, sw->src_l4port_mask);
        FIELD_GET_MASK(IP6_RUL3_M3, IP6SPORTM_EN, mask_en);
        if (mask_en)
        {
            sw->src_l4port_op = FAL_ACL_FIELD_MASK;
        }
        else
        {
            sw->src_l4port_op = FAL_ACL_FIELD_RANGE;
        }

        if (A_TRUE ==
                _isisc_acl_field_care(sw->src_l4port_op,
                                     (a_uint32_t) sw->src_l4port_val,
                                     (a_uint32_t) sw->src_l4port_mask, 0xffff))
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_SPORT);
        }
        else if (FAL_ACL_FIELD_RANGE == sw->src_l4port_op)
        {
            FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_L4_SPORT);
        }
    }

    FIELD_GET(IP6_RUL3_V1, IP6LABEL1V, sw->ip6_lable_val);
    FIELD_GET_MASK(IP6_RUL3_M1, IP6LABEL1M, sw->ip6_lable_mask);

    FIELD_GET(IP6_RUL3_V2, IP6LABEL2V, tmp);
    sw->ip6_lable_val |= (tmp << 16);
    FIELD_GET_MASK(IP6_RUL3_M2, IP6LABEL2M, tmp);
    sw->ip6_lable_mask |= (tmp << 16);

    if (0x0 != sw->ip6_lable_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_IP6_LABEL);
    }

    FIELD_GET(IP6_RUL3_V3, IP6TCPFLAGV, sw->tcp_flag_val);
    FIELD_GET_MASK(IP6_RUL3_M3, IP6TCPFLAGM, sw->tcp_flag_mask);
    if (0x0 != sw->tcp_flag_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_TCP_FLAG);
    }

    FIELD_GET(IP6_RUL3_V3, IP6DHCPV, sw->dhcpv6_val);
    FIELD_GET_MASK(IP6_RUL3_M3, IP6DHCPM, sw->dhcpv6_mask);
    if (0x0 != sw->dhcpv6_mask)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_DHCPV6);
    }

    FIELD_GET(MAC_RUL_V4, RULE_INV, mask_en);
    if (mask_en)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_udf_reparse(fal_acl_rule_t * sw, const hw_filter_t * hw)
{
    a_uint32_t i;

    FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_UDF);

    /* for ISIS UDF type, length and offset no meanging in rules, just set default value */
    sw->udf_type = FAL_ACL_UDF_TYPE_L2;
    sw->udf_len = 16;
    sw->udf_offset = 0;

    for (i = 0; i < ISISC_MAX_UDF_LENGTH; i++)
    {
        sw->udf_val[i] = ((hw->vlu[3 - i / 4]) >> (24 - 8 * (i % 4))) & 0xff;
        sw->udf_mask[i] = ((hw->msk[3 - i / 4]) >> (24 - 8 * (i % 4))) & 0xff;
    }

    FIELD_GET(MAC_RUL_V4, RULE_INV, i);
    if (i)
    {
        FAL_FIELD_FLG_SET(sw->field_flg, FAL_ACL_FIELD_INVERSE_ALL);
    }

    return SW_OK;
}

static sw_error_t
_isisc_acl_rule_action_reparse(fal_acl_rule_t * sw, const hw_filter_t * hw)
{
    a_uint32_t data;

    sw->action_flg = 0;

    FIELD_GET_ACTION(ACL_RSLT2, DES_PORT_EN, data);
    if (1 == data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REDPT);
        FIELD_GET_ACTION(ACL_RSLT1, DES_PORT0, sw->ports);
        FIELD_GET_ACTION(ACL_RSLT2, DES_PORT1, data);
        sw->ports |= (data << 3);
    }

    FIELD_GET_ACTION(ACL_RSLT2, FWD_CMD, data);
    if (0x7 == data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_DENY);
    }
    else if (0x3 == data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_RDTCPU);
    }
    else if (0x1 == data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_CPYCPU);
    }
    else
    {
        /* need't set permit action */
    }

    FIELD_GET_ACTION(ACL_RSLT2, MIRR_EN, data);
    if (1 == data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_MIRROR);
    }

    FIELD_GET_ACTION(ACL_RSLT1, PRI_QU_EN, data);
    if (1 == data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_QUEUE);
        FIELD_GET_ACTION(ACL_RSLT1, PRI_QU, sw->queue);
    }

    FIELD_GET_ACTION(ACL_RSLT1, DSCP_REMAP, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_DSCP);
        FIELD_GET_ACTION(ACL_RSLT1, DSCPV, sw->dscp);
    }

    FIELD_GET_ACTION(ACL_RSLT0, STAGVID, sw->stag_vid);

    FIELD_GET_ACTION(ACL_RSLT1, TRANS_SVID_CHG, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_STAG_VID);
    }

    FIELD_GET_ACTION(ACL_RSLT1, STAG_PRI_REMAP, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_STAG_PRI);
        FIELD_GET_ACTION(ACL_RSLT0, STAGPRI, sw->stag_pri);
    }

    FIELD_GET_ACTION(ACL_RSLT1, STAG_DEI_CHG, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_STAG_DEI);
        FIELD_GET_ACTION(ACL_RSLT0, STAGDEI, sw->stag_dei);
    }

    FIELD_GET_ACTION(ACL_RSLT0, CTAGVID, sw->ctag_vid);

    FIELD_GET_ACTION(ACL_RSLT1, TRANS_CVID_CHG, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_CTAG_VID);
    }

    FIELD_GET_ACTION(ACL_RSLT1, CTAG_PRI_REMAP, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_CTAG_PRI);
        FIELD_GET_ACTION(ACL_RSLT0, CTAGPRI, sw->ctag_pri);
    }

    FIELD_GET_ACTION(ACL_RSLT1, CTAG_CFI_CHG, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_CTAG_CFI);
        FIELD_GET_ACTION(ACL_RSLT0, CTAGCFI, sw->ctag_cfi);
    }

    FIELD_GET_ACTION(ACL_RSLT1, LOOK_VID_CHG, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_REMARK_LOOKUP_VID);
    }

    FIELD_GET_ACTION(ACL_RSLT2, POLICER_EN, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_POLICER_EN);
        FIELD_GET_ACTION(ACL_RSLT2, POLICER_PTR, sw->policer_ptr);
    }

    FIELD_GET_ACTION(ACL_RSLT1, ARP_PTR_EN, data);
    if (data)
    {
        FIELD_GET_ACTION(ACL_RSLT1, WCMP_EN, data);
        if (data)
        {
            FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_WCMP_EN);
            FIELD_GET_ACTION(ACL_RSLT1, ARP_PTR, sw->wcmp_ptr);
        }
        else
        {
            FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_ARP_EN);
            FIELD_GET_ACTION(ACL_RSLT1, ARP_PTR, sw->arp_ptr);
        }
    }

    FIELD_GET_ACTION(ACL_RSLT1, FORCE_L3_MODE, data);
    if ((0 != data) && (3 != data))
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_POLICY_FORWARD_EN);
        if (0x1 == data)
        {
            sw->policy_fwd = FAL_ACL_POLICY_SNAT;
        }
        else
        {
            sw->policy_fwd = FAL_ACL_POLICY_DNAT;
        }
    }

    FIELD_GET_ACTION(ACL_RSLT2, EG_BYPASS, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_BYPASS_EGRESS_TRANS);
    }

    FIELD_GET_ACTION(ACL_RSLT2, TRIGGER_INTR, data);
    if (data)
    {
        FAL_ACTION_FLG_SET(sw->action_flg, FAL_ACL_ACTION_MATCH_TRIGGER_INTR);
    }

    return SW_OK;
}

sw_error_t
_isisc_acl_rule_sw_to_hw(a_uint32_t dev_id, fal_acl_rule_t * sw,
                        isisc_acl_rule_t * hw_rule_snap, a_uint32_t * idx)
{
    sw_error_t rv;
    a_uint32_t tmp_idx, i, b_rule[7] = { 0 };
    parse_func_t ptr[7] = { NULL };
    a_bool_t b_care, b_mac, eh_mac;

    rv = _isisc_acl_action_parse(dev_id, sw, &(hw_rule_snap[*idx].filter));
    SW_RTN_ON_ERROR(rv);

    ptr[0] = _isisc_acl_rule_udf_parse;
    _isisc_acl_rule_mac_preparse(sw, &b_mac, &eh_mac);

    /* ehmac rule must be parsed bofore mac rule.
       it's important for reparse process */
    if (A_TRUE == eh_mac)
    {
        ptr[1] = _isisc_acl_rule_ehmac_parse;
    }

    if (A_TRUE == b_mac)
    {
        ptr[2] = _isisc_acl_rule_bmac_parse;
    }

    if ((A_FALSE == b_mac) && (A_FALSE == eh_mac))
    {
        ptr[2] = _isisc_acl_rule_bmac_parse;
    }

    if (FAL_ACL_RULE_MAC == sw->rule_type)
    {
    }
    else if (FAL_ACL_RULE_IP4 == sw->rule_type)
    {
        ptr[3] = _isisc_acl_rule_ip4_parse;
    }
    else if (FAL_ACL_RULE_IP6 == sw->rule_type)
    {
        ptr[4] = _isisc_acl_rule_ip6r1_parse;
        ptr[5] = _isisc_acl_rule_ip6r2_parse;
        ptr[6] = _isisc_acl_rule_ip6r3_parse;
    }
    else if (FAL_ACL_RULE_UDF == sw->rule_type)
    {
        ptr[1] = NULL;
        ptr[2] = NULL;
    }
    else
    {
        return SW_NOT_SUPPORTED;
    }

    tmp_idx = *idx;
    for (i = 0; i < 7; i++)
    {
        if (ptr[i])
        {
            if (ISISC_HW_RULE_TMP_CNT <= tmp_idx)
            {
                return SW_NO_RESOURCE;
            }

            rv = ptr[i] (sw, &(hw_rule_snap[tmp_idx].filter), &b_care);
            SW_RTN_ON_ERROR(rv);
            if (A_TRUE == b_care)
            {
                tmp_idx++;
                b_rule[i] = 1;
            }
        }
    }

    if (FAL_ACL_RULE_IP6 == sw->rule_type)
    {
        if ((!b_rule[4]) && (!b_rule[5]) && (!b_rule[6]))
        {
            tmp_idx++;
        }
    }

    if (FAL_ACL_RULE_IP4 == sw->rule_type)
    {
        if (!b_rule[3])
        {
            tmp_idx++;
        }
    }

    if (tmp_idx == *idx)
    {
        /* set type start & end */
        SW_SET_REG_BY_FIELD(MAC_RUL_M4, RULE_VALID, FLT_STARTEND,
                            (hw_rule_snap[*idx].filter.msk[4]));
        (*idx)++;
    }
    else
    {
        if (1 == (tmp_idx - *idx))
        {
            if (FAL_ACL_COMBINED_START == sw->combined)
            {
                SW_SET_REG_BY_FIELD(MAC_RUL_M4, RULE_VALID, FLT_START,
                                    (hw_rule_snap[*idx].filter.msk[4]));
            }
            else if (FAL_ACL_COMBINED_CONTINUE == sw->combined)
            {
                SW_SET_REG_BY_FIELD(MAC_RUL_M4, RULE_VALID, FLT_CONTINUE,
                                    (hw_rule_snap[*idx].filter.msk[4]));
            }
            else if (FAL_ACL_COMBINED_END == sw->combined)
            {
                SW_SET_REG_BY_FIELD(MAC_RUL_M4, RULE_VALID, FLT_END,
                                    (hw_rule_snap[*idx].filter.msk[4]));
            }
            else
            {
                SW_SET_REG_BY_FIELD(MAC_RUL_M4, RULE_VALID, FLT_STARTEND,
                                (hw_rule_snap[*idx].filter.msk[4]));
            }
        }
        else
        {
            for (i = *idx; i < tmp_idx; i++)
            {
                if (i == *idx)
                {
                    SW_SET_REG_BY_FIELD(MAC_RUL_M4, RULE_VALID, FLT_START,
                                        (hw_rule_snap[i].filter.msk[4]));
                }
                else if (i == (tmp_idx - 1))
                {
                    SW_SET_REG_BY_FIELD(MAC_RUL_M4, RULE_VALID, FLT_END,
                                        (hw_rule_snap[i].filter.msk[4]));
                }
                else
                {
                    SW_SET_REG_BY_FIELD(MAC_RUL_M4, RULE_VALID, FLT_CONTINUE,
                                        (hw_rule_snap[i].filter.msk[4]));
                }
                aos_mem_copy(&(hw_rule_snap[i].filter.act[0]),
                             &(hw_rule_snap[*idx].filter.act[0]),
                             sizeof (hw_rule_snap[*idx].filter.act));
            }
        }
        *idx = tmp_idx;
    }

    return SW_OK;
}

sw_error_t
_isisc_acl_rule_hw_to_sw(a_uint32_t dev_id, fal_acl_rule_t * sw,
                        isisc_acl_rule_t * hw_rule_snap, a_uint32_t idx,
                        a_uint32_t ent_nr)
{
    a_bool_t b_mac = A_FALSE, b_ip4 = A_FALSE, b_ip6 = A_FALSE;
    sw_error_t rv;
    a_uint32_t i, flt_typ;
    hw_filter_t *hw;

    rv = _isisc_acl_rule_action_reparse(sw, &(hw_rule_snap[idx].filter));
    SW_RTN_ON_ERROR(rv);

    sw->rule_type = FAL_ACL_RULE_UDF;
    for (i = 0; i < ent_nr; i++)
    {
        hw = &(hw_rule_snap[idx + i].filter);
        FIELD_GET_MASK(MAC_RUL_M4, RULE_TYP, flt_typ);

        if (ISISC_UDF_FILTER == flt_typ)
        {
            rv = _isisc_acl_rule_udf_reparse(sw, hw);
            SW_RTN_ON_ERROR(rv);
        }
        else if (ISISC_MAC_FILTER == flt_typ)
        {
            rv = _isisc_acl_rule_bmac_reparse(sw, hw);
            SW_RTN_ON_ERROR(rv);
            b_mac = A_TRUE;
        }
        else if (ISISC_EHMAC_FILTER == flt_typ)
        {
            rv = _isisc_acl_rule_ehmac_reparse(sw, hw);
            SW_RTN_ON_ERROR(rv);
            b_mac = A_TRUE;
        }
        else if (ISISC_IP4_FILTER == flt_typ)
        {
            rv = _isisc_acl_rule_ip4_reparse(sw, hw);
            SW_RTN_ON_ERROR(rv);
            b_ip4 = A_TRUE;
        }
        else if (ISISC_IP6R1_FILTER == flt_typ)
        {
            rv = _isisc_acl_rule_ip6r1_reparse(sw, hw);
            SW_RTN_ON_ERROR(rv);
            b_ip6 = A_TRUE;
        }
        else if (ISISC_IP6R2_FILTER == flt_typ)
        {
            rv = _isisc_acl_rule_ip6r2_reparse(sw, hw);
            SW_RTN_ON_ERROR(rv);
            b_ip6 = A_TRUE;
        }
        else if (ISISC_IP6R3_FILTER == flt_typ)
        {
            rv = _isisc_acl_rule_ip6r3_reparse(sw, hw);
            SW_RTN_ON_ERROR(rv);
            b_ip6 = A_TRUE;
        }
        else
        {
            /* ignore fill gap filters */
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
