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



typedef struct
{
    a_uint8_t  status;
    a_uint8_t  list_id;
    a_uint8_t  list_pri;
    a_uint8_t  rule_nr;
    fal_pbmp_t bind_pts;
} isisc_acl_list_t;


typedef struct
{
    a_uint32_t  vlu[5];
    a_uint32_t  msk[5];
    a_uint32_t  act[3];
} hw_filter_t;


typedef struct
{
    a_uint8_t   status;
    a_uint8_t   list_id;
    a_uint8_t   list_pri;
    a_uint8_t   rule_id;
    hw_filter_t filter;
    a_uint32_t  src_flt_dis; /* src filter disabled */
} isisc_acl_rule_t;


#define ENT_USED                0x1
#define ENT_TMP                 0x2
#define ENT_DEACTIVE            0x4

#define FLT_START               0x0
#define FLT_CONTINUE            0x1
#define FLT_END                 0x2
#define FLT_STARTEND            0x3


#define ISISC_MAC_FILTER         1
#define ISISC_IP4_FILTER         2
#define ISISC_IP6R1_FILTER       3
#define ISISC_IP6R2_FILTER       4
#define ISISC_IP6R3_FILTER       5
#define ISISC_UDF_FILTER         6
#define ISISC_EHMAC_FILTER       7


#define ISISC_MAX_UDF_OFFSET     31
#define ISISC_MAX_UDF_LENGTH     16


#define ISISC_FILTER_VLU_OP      0x0
#define ISISC_FILTER_MSK_OP      0x1
#define ISISC_FILTER_ACT_OP      0x2



//#define ISISC_MAX_FILTER         8
#define ISISC_MAX_FILTER         96
#define ISISC_RULE_FUNC_ADDR     0x0400
#define ISISC_HW_RULE_TMP_CNT    (ISISC_MAX_FILTER + 4)

#define ISISC_MAX_LIST_ID        255
#define ISISC_MAX_LIST_PRI       255

#define ISISC_UDF_MAX_LENGTH     15
#define ISISC_UDF_MAX_OFFSET     31

#define WIN_RULE_CTL0_ADDR      0x218
#define WIN_RULE_CTL1_ADDR      0x234


#define ISISC_FILTER_VLU_ADDR    0x58000
#define ISISC_FILTER_MSK_ADDR    0x59000
#define ISISC_FILTER_ACT_ADDR    0x5a000


#define FIELD_SET(reg, field, val) \
    SW_REG_SET_BY_FIELD_U32(hw->vlu[reg], val, reg##_##field##_BOFFSET, \
                            reg##_##field##_BLEN)

#define FIELD_GET(reg, field, val) \
    SW_FIELD_GET_BY_REG_U32(hw->vlu[reg], val, reg##_##field##_BOFFSET, \
                            reg##_##field##_BLEN)

#define FIELD_SET_MASK(reg, field, val) \
    SW_REG_SET_BY_FIELD_U32(hw->msk[reg-5], val, reg##_##field##_BOFFSET, \
                            reg##_##field##_BLEN)

#define FIELD_GET_MASK(reg, field, val) \
    SW_FIELD_GET_BY_REG_U32(hw->msk[reg-5], val, reg##_##field##_BOFFSET, \
                            reg##_##field##_BLEN)

#define FIELD_SET_ACTION(reg, field, val) \
    SW_REG_SET_BY_FIELD_U32(hw->act[reg-10], val, reg##_##field##_BOFFSET, \
                            reg##_##field##_BLEN)

#define FIELD_GET_ACTION(reg, field, val) \
    SW_FIELD_GET_BY_REG_U32(hw->act[reg-10], val, reg##_##field##_BOFFSET, \
                            reg##_##field##_BLEN)

sw_error_t
_isisc_acl_rule_sw_to_hw(a_uint32_t dev_id, fal_acl_rule_t * sw, isisc_acl_rule_t * hw_filter_snap, a_uint32_t * idx);


sw_error_t
_isisc_acl_rule_hw_to_sw(a_uint32_t dev_id, fal_acl_rule_t * sw, isisc_acl_rule_t * hw_filter_snap, a_uint32_t idx, a_uint32_t ent_nr);


