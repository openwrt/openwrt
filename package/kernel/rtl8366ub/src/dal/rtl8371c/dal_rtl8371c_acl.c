/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (c) 2025 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided as is and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <dal/rtl8371c/dal_rtl8371c_acl.h>
#include <dal/rtl8371c/dal_rtl8371c_vlan.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>
#include <dal/rtl8371c/rtk_rtl8371c_table_struct.h>
#include <rate.h>

#include <dal/dal_mgmts.h>
#include <osal/lib.h>

#if defined(CONFIG_VIRTUAL_REG_ASICDRV_TEST)
rtl8371c_aclrulesmi rtl8371cVirtualAclRuleTable[RTL8371C_ACLRULENO];
rtl8371c_acl_act_entry_t rtl8371cVirtualAclActTable[RTL8371C_ACLRULENO];
#endif

CONST_T rtksw_uint8 rtl8371C_filter_templateField[RTL8371C_ACLTEMPLATENO][RTL8371C_ACLRULEFIELDNO] = {
    {RTL8371C_ACL_DMAC0,             RTL8371C_ACL_DMAC1,          RTL8371C_ACL_DMAC2,          RTL8371C_ACL_SMAC0,          RTL8371C_ACL_SMAC1,          RTL8371C_ACL_SMAC2,          RTL8371C_ACL_ETHERTYPE,      RTL8371C_ACL_FIELD_SELECT07},
    {RTL8371C_ACL_IP4SIP0,           RTL8371C_ACL_IP4SIP1,        RTL8371C_ACL_IP4DIP0,        RTL8371C_ACL_IP4DIP1,        RTL8371C_ACL_L4SPORT,        RTL8371C_ACL_L4DPORT,        RTL8371C_ACL_FIELD_SELECT02, RTL8371C_ACL_FIELD_SELECT07},
    {RTL8371C_ACL_IP6SIP0WITHIPV4,   RTL8371C_ACL_IP6SIP1WITHIPV4,RTL8371C_ACL_L4SPORT,        RTL8371C_ACL_L4DPORT,        RTL8371C_ACL_FIELD_SELECT05, RTL8371C_ACL_FIELD_SELECT06, RTL8371C_ACL_FIELD_SELECT00, RTL8371C_ACL_FIELD_SELECT01},
    {RTL8371C_ACL_IP6DIP0WITHIPV4,   RTL8371C_ACL_IP6DIP1WITHIPV4,RTL8371C_ACL_L4SPORT,        RTL8371C_ACL_L4DPORT,        RTL8371C_ACL_FIELD_SELECT00, RTL8371C_ACL_FIELD_SELECT03, RTL8371C_ACL_FIELD_SELECT04, RTL8371C_ACL_FIELD_SELECT07},
    {RTL8371C_ACL_VIDRANGE,          RTL8371C_ACL_IPRANGE,        RTL8371C_ACL_PORTRANGE,      RTL8371C_ACL_CTAG,           RTL8371C_ACL_STAG,           RTL8371C_ACL_FIELD_SELECT04, RTL8371C_ACL_FIELD_SELECT03, RTL8371C_ACL_FIELD_SELECT07}
};

CONST_T rtksw_uint8 rtl8371C_filter_advanceCaretagField[RTL8371C_ACLTEMPLATENO][2] = {
    {TRUE,      7},
    {TRUE,      7},
    {FALSE,     0},
    {TRUE,      7},
    {TRUE,      7},
};


CONST_T rtksw_uint8 rtl8371C_filter_fieldTemplateIndex[FILTER_FIELD_END][RTKSW_FILTER_FIELD_USED_MAX] = {
    {0x00, 0x01,0x02},
    {0x03, 0x04,0x05},
    {0x06},
    {0x43},
    {0x44},
    {0x10, 0x11},
    {0x12, 0x13},
    {0x24},
    {0x25},
    {0x35},
    {0x35},
    {0x20, 0x21},
    {0x30, 0x31},
    {0x26},
    {0x27},
    {0x14},
    {0x15},
    {0x16},
    {0x14},
    {0x15},
    {0x14},
    {0x14},
    {0x14},

    {0x40},
    {0x41},
    {0x42},

    {0x26},
    {0x27},
    {0x16},
    {0x35},
    {0x36},
    {0x24},
    {0x25},
    {0x47},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},
    {0},

    {0xFF} /* Pattern Match */
};

CONST_T rtksw_uint8 rtl8371C_filter_fieldSize[FILTER_FIELD_END] = {
    3, 3, 1, 1, 1,
    2, 2, 1, 1, 1, 1, 2, 2, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    8
};

CONST_T rtksw_uint16 rtl8371C_field_selector[RTL8371C_FIELDSEL_FORMAT_NUMBER][2] =
{
    {RTL8371C_FIELDSEL_FORMAT_IPV6, 0},    /* Field Selector 0 */
    {RTL8371C_FIELDSEL_FORMAT_IPV6, 6},    /* Field Selector 1 */
    {RTL8371C_FIELDSEL_FORMAT_IPPAYLOAD, 12}, /* Field Selector 2 */
    {RTL8371C_FIELDSEL_FORMAT_IPV4, 6},       /* Field Selector 3 */
    {RTL8371C_FIELDSEL_FORMAT_IPPAYLOAD, 0},  /* Field Selector 4 */
    {RTL8371C_FIELDSEL_FORMAT_IPV4, 0},       /* Field Selector 5 */
    {RTL8371C_FIELDSEL_FORMAT_IPV4, 8},       /* Field Selector 6 */
    {RTL8371C_FIELDSEL_FORMAT_DEFAULT, 0}     /* Field Selector 7 */
};

/*
    Exchange structure type define with MMI and SMI
*/

static void _rtl8371c_aclRuleStSmi2User( rtl8371c_aclrule *pAclUser, rtl8371c_aclrulesmi *pAclSmi)
{
    rtksw_uint8 *care_ptr, *data_ptr;
    rtksw_uint8 care_tmp, data_tmp;
    rtksw_uint8 care_pmask, data_pmask;
    rtksw_uint32 i;

    care_ptr = (rtksw_uint8*)&pAclSmi->care_bits;
    data_ptr = (rtksw_uint8*)&pAclSmi->data_bits;

    for ( i = 0; i < sizeof(struct rtl8371c_acl_rule_smi_st); i++)
    {
        care_tmp = *(care_ptr + i) ^ (*(data_ptr + i));
        data_tmp = ~(*(care_ptr + i)) & *(data_ptr + i);

        *(care_ptr + i) = care_tmp;
        *(data_ptr + i) = data_tmp;
    }

    pAclUser->data_bits.active_portmsk = ((pAclSmi->data_bits.rule_info >> 8) & 0x00FF);
    pAclUser->data_bits.type = (pAclSmi->data_bits.rule_info & 0x0007);
    pAclUser->data_bits.tag_exist = (pAclSmi->data_bits.rule_info & 0x00F8) >> 3;

    for(i = 0; i < RTL8371C_ACLRULEFIELDNO; i++)
        pAclUser->data_bits.field[i] = pAclSmi->data_bits.field[i];

    pAclUser->valid = pAclSmi->valid;

    pAclUser->care_bits.active_portmsk = ((pAclSmi->care_bits.rule_info >> 8) & 0x00FF);
    pAclUser->care_bits.type = (pAclSmi->care_bits.rule_info & 0x0007);
    pAclUser->care_bits.tag_exist = (pAclSmi->care_bits.rule_info & 0x00F8) >> 3;


    care_pmask = pAclUser->care_bits.active_portmsk & 0xff;
    data_pmask = pAclUser->data_bits.active_portmsk & 0xff;

    for (i = 0; i <= 7; i++)
    {
        if( ((care_pmask & (0x01 << i)) == 0 )&&( (data_pmask & (0x01 << i)) == 0) )
        {
            care_pmask |= (0x01 << i);
            data_pmask |= (0x01 << i);
        }
    }

    pAclUser->care_bits.active_portmsk = care_pmask;
    pAclUser->data_bits.active_portmsk = data_pmask;

    for(i = 0; i < RTL8371C_ACLRULEFIELDNO; i++)
        pAclUser->care_bits.field[i] = pAclSmi->care_bits.field[i];
}


/*
    Exchange structure type define with MMI and SMI
*/

static void _rtl8371c_aclRuleStUser2Smi(rtl8371c_aclrule *pAclUser, rtl8371c_aclrulesmi *pAclSmi)
{
    rtksw_uint8 *care_ptr, *data_ptr;
    rtksw_uint8 care_tmp, data_tmp;
    rtksw_uint8 care_pmask, data_pmask;
    rtksw_uint32 i;

    care_pmask = pAclUser->care_bits.active_portmsk & 0xff;
    data_pmask = pAclUser->data_bits.active_portmsk & 0xff;

    for (i = 0; i <= 7; i++)
    {
        if( (care_pmask & (0x01 << i)) && (data_pmask & (0x01 << i)) )
        {
            care_pmask &= ~(0x01 << i);
            data_pmask &= ~(0x01 << i);
        }
    }
    pAclSmi->data_bits.rule_info = (data_pmask << 8) | ((pAclUser->data_bits.tag_exist & 0x1F) << 3) | (pAclUser->data_bits.type & 0x07);

    for(i = 0;i < RTL8371C_ACLRULEFIELDNO; i++)
        pAclSmi->data_bits.field[i] = pAclUser->data_bits.field[i];

    pAclSmi->valid = pAclUser->valid;

    pAclSmi->care_bits.rule_info = (care_pmask << 8) | ((pAclUser->care_bits.tag_exist & 0x1F) << 3) | (pAclUser->care_bits.type & 0x07);

    for(i = 0; i < RTL8371C_ACLRULEFIELDNO; i++)
        pAclSmi->care_bits.field[i] = pAclUser->care_bits.field[i];

    care_ptr = (rtksw_uint8*)&pAclSmi->care_bits;
    data_ptr = (rtksw_uint8*)&pAclSmi->data_bits;

    for ( i = 0; i < sizeof(struct rtl8371c_acl_rule_smi_st); i++)
    {
        care_tmp = ~(*(care_ptr + i)) | ~(*(data_ptr + i));
        data_tmp = ~(*(care_ptr + i)) | *(data_ptr + i);

        *(care_ptr + i) = care_tmp;
        *(data_ptr + i) = data_tmp;
    }
}

/*
    Exchange structure type define with MMI and SMI
*/
static rtksw_api_ret_t _rtl8371c_aclActStSmi2User(rtksw_uint32 unit, rtl8371c_acl_act_t *pAclUser, rtl8371c_acl_act_entry_t *pAclActEntry)
{
    rtksw_uint32 fieldData;
    rtksw_api_ret_t retVal;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_CACTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->cact = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_CVIDtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->cvidx_cact = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_CACT_EXTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->cact_ext = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_TAG_FMTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->tag_fmt = fieldData;
    
    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_SACTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->sact = fieldData;
    
    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_SVIDtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->svidx_sact = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_METER_LOG_IDXtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;
    
    pAclUser->aclmeteridx = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_FWD_ACTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->fwdact = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_ACLPMSKtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->fwdpmask = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_FWD_EXTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->fwdact_ext = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_PRI_CATtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->priact = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_ACL_PRItf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

     pAclUser->pridx = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_ACL_INTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->aclint = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_GPIO_ENtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->gpio_en = fieldData;

    if ((retVal = table16_field_get(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_GPIO_PINtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    pAclUser->gpio_pin = fieldData;
    return RT_ERR_OK;
}

/*
    Exchange structure type define with MMI and SMI
*/
static rtksw_api_ret_t _rtl8371c_aclActStUser2Smi(rtksw_uint32 unit, rtl8371c_acl_act_t *pAclUser, rtl8371c_acl_act_entry_t *pAclActEntry)
{
    rtksw_uint32 fieldData;
    rtksw_api_ret_t retVal;

    fieldData = pAclUser->cact;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_CACTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->cvidx_cact;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_CVIDtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->cact_ext;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_CACT_EXTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->tag_fmt;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_TAG_FMTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;
    
    fieldData = pAclUser->sact;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_SACTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;
    
    fieldData = pAclUser->svidx_sact;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_SVIDtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->aclmeteridx;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_METER_LOG_IDXtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->fwdact;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_FWD_ACTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->fwdpmask;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_ACLPMSKtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->fwdact_ext;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_FWD_EXTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->priact;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_PRI_CATtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->pridx;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_ACL_PRItf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->aclint;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_ACL_INTtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->gpio_en;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_GPIO_ENtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    fieldData = pAclUser->gpio_pin;
    if ((retVal = table16_field_set(unit, RTL8371C_ACL_ACT_ENTRYt, RTL8371C_ACL_ACT_ENTRY_GPIO_PINtf, &fieldData, (rtksw_uint32 *)pAclActEntry)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_cfg_delAll
 * Description:
 *      Delete all ACL entries from ASIC
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      This function delete all ACL configuration from ASIC.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_cfg_delAll(rtksw_uint32 unit)
{
    rtksw_uint32 i;
    rtksw_uint32 regData;
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    for (i = 0; i < RTL8371C_ACLRULENO; i++)
    {
        regData = 1;
        if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, i, RTL8371C_ACT_CVIDf, &regData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, i, RTL8371C_ACT_SVIDf, &regData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, i, RTL8371C_ACT_PRIORITYf, &regData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, i, RTL8371C_ACT_POLICINGf, &regData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, i, RTL8371C_ACT_FORWARDf, &regData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, i, RTL8371C_ACT_GPIOf, &regData)) != RT_ERR_OK)
            return retVal;

        regData = 0;
        if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, i, RTL8371C_OP_NOTf, &regData)) != RT_ERR_OK)
            return retVal;
    }

    regData = 1;
    if ((retVal = reg16_field_write(unit, RTL8371C_ACL_CTRLr, RTL8371C_TABLE_RSTf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_init
 * Description:
 *      ACL initialization function
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - Pointer pFilter_field or pFilter_cfg point to NULL.
 * Note:
 *      This function enable and intialize ACL function
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_init(rtksw_uint32 unit)
{
    rtksw_filter_template_t aclTemplate;
    rtksw_api_ret_t retVal;
    rtksw_uint32 i;
    rtksw_uint32 j;
    rtksw_port_t port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Delete all rules */
    if ((retVal =  dal_rtl8371c_filter_igrAcl_cfg_delAll(unit)) != RT_ERR_OK)
        return retVal;

    /* Clear all range check */
    for (i = 0; i < RTL8371C_ACLRANGEMAX; i++)
    {
        if ((retVal = dal_rtl8371c_filter_iprange_set(unit, i, IPRANGE_UNUSED, 0, 0)) != RT_ERR_OK)
            return retVal;
        
        if ((retVal = dal_rtl8371c_filter_vidrange_set(unit, i, VIDRANGE_UNUSED, 0, 0)) != RT_ERR_OK)
            return retVal;

        if ((retVal = dal_rtl8371c_filter_portrange_set(unit, i, PORTRANGE_UNUSED, 0, 0)) != RT_ERR_OK)
            return retVal;
    }

    /* Setup Template */
    for(i = 0; i < RTL8371C_ACLTEMPLATENO; i++)
    {
        osal_memset(&aclTemplate, 0x00, sizeof(rtksw_filter_template_t));
        aclTemplate.index = i;
        for(j = 0; j < RTL8371C_ACLRULEFIELDNO; j++)
            aclTemplate.fieldType[j] = (rtksw_filter_field_type_raw_t)rtl8371C_filter_templateField[i][j];

        if ((retVal = dal_rtl8371c_filter_igrAcl_template_set(unit, &aclTemplate)) != RT_ERR_OK)
            return retVal;
    }

    /* Setup Field Select */
    for(i = 0; i < RTL8371C_FIELDSEL_FORMAT_NUMBER; i++)
    {
        if ((retVal = dal_rtl8371c_filter_igrAcl_field_sel_set(unit, i, (rtksw_field_sel_t)rtl8371C_field_selector[i][0], rtl8371C_field_selector[i][1])) != RT_ERR_OK)
            return retVal;
    }

    /* Enable ACL and set unmatch action = permit */
    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((retVal = dal_rtl8371c_filter_igrAcl_state_set(unit, port, RTKSW_ENABLED)) != RT_ERR_OK)
            return retVal;

        if ((retVal = dal_rtl8371c_filter_igrAcl_unmatchAction_set(unit, port, FILTER_UNMATCH_PERMIT)) != RT_ERR_OK)
            return retVal;
    }

#ifdef CONFIG_VIRTUAL_REG_ASICDRV_TEST
    for(i = 0; i < RTL8371C_ACLRULENO; i++)
        osal_memset(&rtl8371cVirtualAclRuleTable[i], 0x00, sizeof(rtl8371c_aclrulesmi));
#endif
    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_field_add
 * Description:
 *      Add comparison rule to an ACL configuration
 * Input:
 *      unit            - Unit ID
 *      pFilter_cfg     - The ACL configuration that this function will add comparison rule
 *      pFilter_field   - The comparison rule that will be added.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NULL_POINTER     - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This function add a comparison rule (*pFilter_field) to an ACL configuration (*pFilter_cfg).
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pFilter_field will be added to linked
 *      list keeped by structure that pFilter_cfg points to.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_field_add(rtksw_uint32 unit, rtksw_filter_cfg_t* pFilter_cfg, rtksw_filter_field_t* pFilter_field)
{
    rtksw_uint32 i;
    rtksw_filter_field_t *tailPtr;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pFilter_cfg || NULL == pFilter_field)
        return RT_ERR_NULL_POINTER;

    if(pFilter_field->fieldType >= FILTER_FIELD_END)
        return RT_ERR_ENTRY_INDEX;


    if(0 == pFilter_field->fieldTemplateNo)
    {
        pFilter_field->fieldTemplateNo = rtl8371C_filter_fieldSize[pFilter_field->fieldType];

        for(i = 0; i < pFilter_field->fieldTemplateNo; i++)
        {
            pFilter_field->fieldTemplateIdx[i] = rtl8371C_filter_fieldTemplateIndex[pFilter_field->fieldType][i];
        }
    }

    if(NULL == pFilter_cfg->fieldHead)
    {
        pFilter_cfg->fieldHead = pFilter_field;
    }
    else
    {
        if (pFilter_cfg->fieldHead->next == NULL)
        {
            pFilter_cfg->fieldHead->next = pFilter_field;
        }
        else
        {
            tailPtr = pFilter_cfg->fieldHead->next;
            while( tailPtr->next != NULL)
            {
                tailPtr = tailPtr->next;
            }
            tailPtr->next = pFilter_field;
        }
    }

    return RT_ERR_OK;
}


static rtksw_api_ret_t _rtksw_filter_igrAcl_writeDataField(rtl8371c_aclrule *aclRule, rtksw_filter_field_t *fieldPtr)
{
    rtksw_uint32 i, tempIdx,fieldIdx, ipValue, ipMask;
    rtksw_uint32 ip6addr[RTL8371C_RTKSW_IPV6_ADDR_WORD_LENGTH];
    rtksw_uint32 ip6mask[RTL8371C_RTKSW_IPV6_ADDR_WORD_LENGTH];

    for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
    {
        tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;

        aclRule[tempIdx].valid = TRUE;
    }

    switch (fieldPtr->fieldType)
    {
    /* use DMAC structure as representative for mac structure */
    case FILTER_FIELD_DMAC:
    case FILTER_FIELD_SMAC:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.mac.value.octet[5 - i*2] | (fieldPtr->filter_pattern_union.mac.value.octet[5 - (i*2 + 1)] << 8);
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.mac.mask.octet[5 - i*2] | (fieldPtr->filter_pattern_union.mac.mask.octet[5 - (i*2 + 1)] << 8);
        }
        break;
    case FILTER_FIELD_ETHERTYPE:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.etherType.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.etherType.mask;
        }
        break;
    case FILTER_FIELD_IPV4_SIP:
    case FILTER_FIELD_IPV4_DIP:

        ipValue = fieldPtr->filter_pattern_union.sip.value;
        ipMask = fieldPtr->filter_pattern_union.sip.mask;

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = (0xFFFF & (ipValue >> (i*16)));
            aclRule[tempIdx].care_bits.field[fieldIdx] = (0xFFFF & (ipMask >> (i*16)));
        }
        break;
    case FILTER_FIELD_IPV4_TOS:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.ipTos.value & 0xFF;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.ipTos.mask  & 0xFF;
        }
        break;
    case FILTER_FIELD_IPV4_PROTOCOL:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.protocol.value & 0xFF;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.protocol.mask  & 0xFF;
        }
        break;
    case FILTER_FIELD_IPV6_SIPV6:
    case FILTER_FIELD_IPV6_DIPV6:
        for(i = 0; i < RTL8371C_RTKSW_IPV6_ADDR_WORD_LENGTH; i++)
        {
            ip6addr[i] = fieldPtr->filter_pattern_union.sipv6.value.addr[i];
            ip6mask[i] = fieldPtr->filter_pattern_union.sipv6.mask.addr[i];
        }

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            if(i < 2)
            {
                aclRule[tempIdx].data_bits.field[fieldIdx] = ((ip6addr[0] & (0xFFFF << (i * 16))) >> (i * 16));
                aclRule[tempIdx].care_bits.field[fieldIdx] = ((ip6mask[0] & (0xFFFF << (i * 16))) >> (i * 16));
            }
        }

        break;
    case FILTER_FIELD_CTAG:
    case FILTER_FIELD_STAG:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.l2tag.pri.value << 13) | (fieldPtr->filter_pattern_union.l2tag.cfi.value << 12) | fieldPtr->filter_pattern_union.l2tag.vid.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.l2tag.pri.mask << 13) | (fieldPtr->filter_pattern_union.l2tag.cfi.mask << 12) | fieldPtr->filter_pattern_union.l2tag.vid.mask;
        }
        break;
    case FILTER_FIELD_IPV4_FLAG:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] &= 0x1FFF;
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.xf.value << 15);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.df.value << 14);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.mf.value << 13);

            aclRule[tempIdx].care_bits.field[fieldIdx] &= 0x1FFF;
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.xf.mask << 15);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.df.mask << 14);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.ipFlag.mf.mask << 13);
        }

        break;
    case FILTER_FIELD_IPV4_OFFSET:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] &= 0xE000;
            aclRule[tempIdx].data_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.inData.value;

            aclRule[tempIdx].care_bits.field[fieldIdx] &= 0xE000;
            aclRule[tempIdx].care_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.inData.mask;
        }

        break;

    case FILTER_FIELD_IPV6_TRAFFIC_CLASS:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;


            aclRule[tempIdx].data_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.inData.value << 4)&0x0FF0;
            aclRule[tempIdx].care_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.inData.mask << 4)&0x0FF0;
        }
        break;
    case FILTER_FIELD_IPV6_NEXT_HEADER:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.inData.value << 8;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.inData.mask << 8;
        }
        break;
    case FILTER_FIELD_TCP_SPORT:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.tcpSrcPort.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.tcpSrcPort.mask;
        }
        break;
    case FILTER_FIELD_TCP_DPORT:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.tcpDstPort.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.tcpDstPort.mask;
        }
        break;
    case FILTER_FIELD_TCP_FLAG:

        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.cwr.value << 7);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.ece.value << 6);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.urg.value << 5);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.ack.value << 4);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.psh.value << 3);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.rst.value << 2);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.syn.value << 1);
            aclRule[tempIdx].data_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.tcpFlag.fin.value;

            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.cwr.mask << 7);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.ece.mask << 6);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.urg.mask << 5);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.ack.mask << 4);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.psh.mask << 3);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.rst.mask << 2);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.tcpFlag.syn.mask << 1);
            aclRule[tempIdx].care_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.tcpFlag.fin.mask;
        }
        break;
    case FILTER_FIELD_UDP_SPORT:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.udpSrcPort.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.udpSrcPort.mask;
        }
        break;
    case FILTER_FIELD_UDP_DPORT:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.udpDstPort.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.udpDstPort.mask;
        }
        break;
    case FILTER_FIELD_ICMP_CODE:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] &= 0xFF00;
            aclRule[tempIdx].data_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.icmpCode.value;
            aclRule[tempIdx].care_bits.field[fieldIdx] &= 0xFF00;
            aclRule[tempIdx].care_bits.field[fieldIdx] |= fieldPtr->filter_pattern_union.icmpCode.mask;
        }
        break;
    case FILTER_FIELD_ICMP_TYPE:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] &= 0x00FF;
            aclRule[tempIdx].data_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.icmpType.value << 8);
            aclRule[tempIdx].care_bits.field[fieldIdx] &= 0x00FF;
            aclRule[tempIdx].care_bits.field[fieldIdx] |= (fieldPtr->filter_pattern_union.icmpType.mask << 8);
        }
        break;
    case FILTER_FIELD_IGMP_TYPE:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.igmpType.value << 8);
            aclRule[tempIdx].care_bits.field[fieldIdx] = (fieldPtr->filter_pattern_union.igmpType.mask << 8);
        }
        break;
    case FILTER_FIELD_PATTERN_MATCH:
        for(i = 0; i < fieldPtr->fieldTemplateNo; i++)
        {
            tempIdx = (fieldPtr->fieldTemplateIdx[i] & 0xF0) >> 4;
            fieldIdx = fieldPtr->fieldTemplateIdx[i] & 0x0F;

            aclRule[tempIdx].data_bits.field[fieldIdx] = ((fieldPtr->filter_pattern_union.pattern.value[i/2] >> (16 * (i%2))) & 0x0000FFFF );
            aclRule[tempIdx].care_bits.field[fieldIdx] = ((fieldPtr->filter_pattern_union.pattern.mask[i/2] >> (16 * (i%2))) & 0x0000FFFF );
        }
        break;
    case FILTER_FIELD_VID_RANGE:
    case FILTER_FIELD_IP_RANGE:
    case FILTER_FIELD_PORT_RANGE:
    default:
        tempIdx = (fieldPtr->fieldTemplateIdx[0] & 0xF0) >> 4;
        fieldIdx = fieldPtr->fieldTemplateIdx[0] & 0x0F;

        aclRule[tempIdx].data_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.inData.value;
        aclRule[tempIdx].care_bits.field[fieldIdx] = fieldPtr->filter_pattern_union.inData.mask;
        break;
    }

    return RT_ERR_OK;
}

static rtksw_api_ret_t _rtl8371c_getAclRule(rtksw_uint32 unit, rtksw_uint32 index, rtl8371c_aclrule *pAclRule)
{
    rtl8371c_aclrulesmi aclRuleSmi;
    ret_t retVal;
    rtksw_uint16* tableAddr;
    rtksw_uint32 i;
    rtl8371c_acl_entry_t aclEntry;

    if(index > RTL8371C_ACLRULEMAX)
        return RT_ERR_OUT_OF_RANGE;

    osal_memset(&aclRuleSmi, 0x00, sizeof(rtl8371c_aclrulesmi));

    /* Read Data Bits */
    osal_memset(&aclEntry, 0x00, sizeof(rtl8371c_acl_entry_t));
    if ((retVal = table16_read(unit, RTL8371C_ACL_RULE_DATAt, index, (rtksw_uint32 *)&aclEntry)) != RT_ERR_OK)
        return retVal;

    tableAddr = (rtksw_uint16 *)&aclRuleSmi.data_bits;
    for(i = 0; i < (RTL8371C_ACLRULETBLEN - 1); i++)
    {
        *tableAddr = aclEntry.entry_data[i];
        tableAddr++;
    }

    aclRuleSmi.valid = aclEntry.entry_data[RTL8371C_ACLRULETBLEN - 1] & 0x1;

    /* Read Care Bits */
    osal_memset(&aclEntry, 0x00, sizeof(rtl8371c_acl_entry_t));
    if ((retVal = table16_read(unit, RTL8371C_ACL_RULE_MASKt, index, (rtksw_uint32 *)&aclEntry)) != RT_ERR_OK)
        return retVal;

    tableAddr = (rtksw_uint16 *)&aclRuleSmi.care_bits;
    for(i = 0; i < (RTL8371C_ACLRULETBLEN - 1); i++)
    {
        *tableAddr = aclEntry.entry_data[i];
        tableAddr++;
    }

#ifdef CONFIG_VIRTUAL_REG_ASICDRV_TEST
    osal_memcpy(&aclRuleSmi, &rtl8371cVirtualAclRuleTable[index], sizeof(rtl8371c_aclrulesmi));
#endif

     _rtl8371c_aclRuleStSmi2User(pAclRule, &aclRuleSmi);

    return RT_ERR_OK;
}

static rtksw_api_ret_t _rtl8371c_setAclAct(rtksw_uint32 unit, rtksw_uint32 index, rtl8371c_acl_act_t* pAclAct)
{
    ret_t retVal;
    rtl8371c_acl_act_entry_t aclActEntry;

    if(index > RTL8371C_ACLRULEMAX)
        return RT_ERR_OUT_OF_RANGE;

    osal_memset(&aclActEntry, 0x00, sizeof(rtl8371c_acl_act_entry_t));
    if ((retVal = _rtl8371c_aclActStUser2Smi(unit, pAclAct, &aclActEntry)) != RT_ERR_OK)
        return retVal;

    if ((retVal = table16_write(unit, RTL8371C_ACL_ACT_ENTRYt, index, (rtksw_uint32 *)&aclActEntry)) != RT_ERR_OK)
        return retVal;

#ifdef CONFIG_VIRTUAL_REG_ASICDRV_TEST
    osal_memcpy(&rtl8371cVirtualAclActTable[index], &aclActEntry, sizeof(rtl8371c_acl_act_entry_t));
#endif

    return RT_ERR_OK;
}

static rtksw_api_ret_t _rtl8371c_setAclRule(rtksw_uint32 unit, rtksw_uint32 index, rtl8371c_aclrule* pAclRule)
{
    rtl8371c_aclrulesmi aclRuleSmi;
    rtksw_uint16* tableAddr;
    rtksw_uint32 i;
    rtksw_api_ret_t retVal;
    rtl8371c_acl_entry_t aclEntry;

    if(index > RTL8371C_ACLRULEMAX)
        return RT_ERR_OUT_OF_RANGE;

    osal_memset(&aclRuleSmi, 0x00, sizeof(rtl8371c_aclrulesmi));
    _rtl8371c_aclRuleStUser2Smi(pAclRule, &aclRuleSmi);

    /* Write valid bit = 0 */
    osal_memset(&aclEntry, 0x00, sizeof(rtl8371c_acl_entry_t));
    if ((retVal = table16_write(unit, RTL8371C_ACL_RULE_DATAt, index, (rtksw_uint32 *)&aclEntry)) != RT_ERR_OK)
        return retVal;

    /* Write Care Bits */
    osal_memset(&aclEntry, 0x00, sizeof(rtl8371c_acl_entry_t));
    tableAddr = (rtksw_uint16*)&aclRuleSmi.care_bits;
    for(i = 0; i < (RTL8371C_ACLRULETBLEN - 1); i++) /* Care bits length = 9 registers. */
    {
        aclEntry.entry_data[i] = (rtksw_uint32)*tableAddr;
        tableAddr++;
    }

    if ((retVal = table16_write(unit, RTL8371C_ACL_RULE_MASKt, index, (rtksw_uint32 *)&aclEntry)) != RT_ERR_OK)
        return retVal;

    /* Write Data Bits */
    osal_memset(&aclEntry, 0x00, sizeof(rtl8371c_acl_entry_t));
    tableAddr = (rtksw_uint16*)&aclRuleSmi.data_bits;
    for(i = 0; i < (RTL8371C_ACLRULETBLEN - 1); i++) /* Data bits length = 9 registers + valid bit. */
    {
        aclEntry.entry_data[i] = (rtksw_uint32)*tableAddr;
        tableAddr++;
    }

    aclEntry.entry_data[RTL8371C_ACLRULETBLEN - 1] = aclRuleSmi.valid;
    if ((retVal = table16_write(unit, RTL8371C_ACL_RULE_DATAt, index, (rtksw_uint32 *)&aclEntry)) != RT_ERR_OK)
        return retVal;

#ifdef CONFIG_VIRTUAL_REG_ASICDRV_TEST
    osal_memcpy(&rtl8371cVirtualAclRuleTable[index], &aclRuleSmi, sizeof(rtl8371c_aclrulesmi));
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_cfg_add
 * Description:
 *      Add an ACL configuration to ASIC
 * Input:
 *      unit            - Unit ID
 *      filter_id       - Start index of ACL configuration.
 *      pFilter_cfg     - The ACL configuration that this function will add comparison rule
 *      pFilter_action  - Action(s) of ACL configuration.
 * Output:
 *      ruleNum - number of rules written in acl table
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_SMI                              - SMI access error
 *      RT_ERR_NULL_POINTER                     - Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 *      RT_ERR_ENTRY_INDEX                      - Invalid filter_id .
 *      RT_ERR_NULL_POINTER                     - Pointer pFilter_action or pFilter_cfg point to NULL.
 *      RT_ERR_FILTER_INACL_ACT_NOT_SUPPORT     - Action is not supported in this chip.
 *      RT_ERR_FILTER_INACL_RULE_NOT_SUPPORT    - Rule is not supported.
 * Note:
 *      This function store pFilter_cfg, pFilter_action into ASIC. The starting
 *      index(es) is filter_id.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_cfg_add(rtksw_uint32 unit, rtksw_filter_id_t filter_id, rtksw_filter_cfg_t* pFilter_cfg, rtksw_filter_action_t* pFilter_action, rtksw_filter_number_t *ruleNum)
{
    rtksw_api_ret_t               retVal;
    rtksw_uint32                  careTagData, careTagMask;
    rtksw_uint32                  i,actType, ruleId;
    rtksw_uint32                  aclActCtrl;
    rtksw_uint32                  cpuPort;
    rtksw_filter_field_t*         fieldPtr;
    rtl8371c_aclrule            aclRule[RTL8371C_ACLTEMPLATENO];
    rtl8371c_aclrule            tempRule;
    rtl8371c_acl_act_t          aclAct;
    rtksw_uint32                  noRulesAdd;
    rtksw_uint32                  portmask;
    rtksw_uint32                  regData;
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(filter_id > RTL8371C_ACLRULEMAX )
        return RT_ERR_ENTRY_INDEX;

    if((NULL == pFilter_cfg) || (NULL == pFilter_action) || (NULL == ruleNum))
        return RT_ERR_NULL_POINTER;

    fieldPtr = pFilter_cfg->fieldHead;

    /* init RULE */
    for(i = 0; i < RTL8371C_ACLTEMPLATENO; i++)
    {
        osal_memset(&aclRule[i], 0, sizeof(rtl8371c_aclrule));

        aclRule[i].data_bits.type= i;
        aclRule[i].care_bits.type= 0x7;
    }

    while(NULL != fieldPtr)
    {
        _rtksw_filter_igrAcl_writeDataField(aclRule, fieldPtr);

        fieldPtr = fieldPtr->next;
    }

    /*set care tag mask in User Defined Field 15*/
    /*Follow care tag should not be used while ACL template and User defined fields are fully control by system designer*/
    /*those advanced packet type care tag is used in default template design structure only*/
    careTagData = 0;
    careTagMask = 0;

    for(i = CARE_TAG_TCP; i < CARE_TAG_END; i++)
    {
        if(pFilter_cfg->careTag.tagType[i].mask)
            careTagMask = careTagMask | (1 << (i-CARE_TAG_TCP));

        if(pFilter_cfg->careTag.tagType[i].value)
            careTagData = careTagData | (1 << (i-CARE_TAG_TCP));
    }

    if(careTagData || careTagMask)
    {
        i = 0;
        while(i < RTL8371C_ACLTEMPLATENO)
        {
            if(aclRule[i].valid == 1 && rtl8371C_filter_advanceCaretagField[i][0] == TRUE)
            {
                aclRule[i].data_bits.field[rtl8371C_filter_advanceCaretagField[i][1]] = careTagData & 0xFFFF;
                aclRule[i].care_bits.field[rtl8371C_filter_advanceCaretagField[i][1]] = careTagMask & 0xFFFF;
                break;
            }
            i++;
        }
        /*none of previous used template containing field 15*/
        if(i == RTL8371C_ACLTEMPLATENO)
        {
            i = 0;
            while(i < RTL8371C_ACLTEMPLATENO)
            {
                if(rtl8371C_filter_advanceCaretagField[i][0] == TRUE)
                {
                    aclRule[i].data_bits.field[rtl8371C_filter_advanceCaretagField[i][1]] = careTagData & 0xFFFF;
                    aclRule[i].care_bits.field[rtl8371C_filter_advanceCaretagField[i][1]] = careTagMask & 0xFFFF;
                    aclRule[i].valid = 1;
                    break;
                }
                i++;
            }
        }
    }

    /*Check rule number*/
    noRulesAdd = 0;
    for(i = 0; i < RTL8371C_ACLTEMPLATENO; i++)
    {
        if(1 == aclRule[i].valid)
        {
            noRulesAdd ++;
        }
    }

    *ruleNum = noRulesAdd;

    if((filter_id + noRulesAdd - 1) > RTL8371C_ACLRULEMAX)
    {
        return RT_ERR_ENTRY_INDEX;
    }

    /*set care tag mask in TAG Indicator*/
    careTagData = 0;
    careTagMask = 0;

    for(i = 0; i <= CARE_TAG_IPV6;i++)
    {
        if(0 == pFilter_cfg->careTag.tagType[i].mask )
        {
            careTagMask &=  ~(1 << i);
        }
        else
        {
            careTagMask |= (1 << i);
            if(0 == pFilter_cfg->careTag.tagType[i].value )
                careTagData &= ~(1 << i);
            else
                careTagData |= (1 << i);
        }
    }

    for(i = 0; i < RTL8371C_ACLTEMPLATENO; i++)
    {
        aclRule[i].data_bits.tag_exist = (careTagData) & ACL_RULE_CARETAG_MASK;
        aclRule[i].care_bits.tag_exist = (careTagMask) & ACL_RULE_CARETAG_MASK;
    }

    RTKSW_CHK_PORTMASK_VALID(unit, &pFilter_cfg->activeport.value);
    RTKSW_CHK_PORTMASK_VALID(unit, &pFilter_cfg->activeport.mask);

    for(i = 0; i < RTL8371C_ACLTEMPLATENO; i++)
    {
        if(TRUE == aclRule[i].valid)
        {
            if(rtksw_switch_portmask_L2P_get(unit, &pFilter_cfg->activeport.value, &portmask) != RT_ERR_OK)
                return RT_ERR_PORT_MASK;

            aclRule[i].data_bits.active_portmsk = portmask;

            if(rtksw_switch_portmask_L2P_get(unit, &pFilter_cfg->activeport.mask, &portmask) != RT_ERR_OK)
                return RT_ERR_PORT_MASK;

            aclRule[i].care_bits.active_portmsk = portmask;
        }
    }

    if(pFilter_cfg->invert >= FILTER_INVERT_END )
        return RT_ERR_INPUT;


    /*Last action gets high priority if actions are the same*/
    osal_memset(&aclAct, 0, sizeof(rtl8371c_acl_act_t));
    aclActCtrl = 0;
    for(actType = 0; actType < FILTER_ENACT_END; actType ++)
    {
        if(pFilter_action->actEnable[actType])
        {
            switch (actType)
            {
            case FILTER_ENACT_CVLAN_INGRESS:
                if(pFilter_action->filterCvlanVid > RTL8371C_VIDMAX)
                    return RT_ERR_INPUT;
                aclAct.cact = FILTER_ENACT_CVLAN_TYPE(actType);
                aclAct.cvidx_cact = pFilter_action->filterCvlanVid;

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_TAGONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_VLANONLY;
                }

                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
            case FILTER_ENACT_CVLAN_EGRESS:
                if(pFilter_action->filterCvlanVid > RTL8371C_VIDMAX)
                    return RT_ERR_INPUT;

                aclAct.cact = FILTER_ENACT_CVLAN_TYPE(actType);
                aclAct.cvidx_cact = pFilter_action->filterCvlanVid;

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_TAGONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_VLANONLY;
                }

                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_CVLAN_SVID:

                aclAct.cact = FILTER_ENACT_CVLAN_TYPE(actType);

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_TAGONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_VLANONLY;
                }

                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_POLICING_1:
                if(pFilter_action->filterPolicingIdx[1] >= ((RTKSW_MAX_METER_ID + 1) + RTL8371C_MAX_LOG_CNT_NUM))
                    return RT_ERR_INPUT;

                aclAct.cact = FILTER_ENACT_CVLAN_TYPE(actType);
                aclAct.cvidx_cact = pFilter_action->filterPolicingIdx[1];

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_TAGONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_VLANONLY;
                }

                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;

            case FILTER_ENACT_SVLAN_INGRESS:
            case FILTER_ENACT_SVLAN_EGRESS:
                aclAct.sact = FILTER_ENACT_SVLAN_TYPE(actType);
                aclAct.svidx_sact = pFilter_action->filterSvlanVid;
                aclActCtrl |= FILTER_ENACT_SVLAN_MASK;
                break;
            case FILTER_ENACT_SVLAN_CVID:

                aclAct.sact = FILTER_ENACT_SVLAN_TYPE(actType);
                aclActCtrl |= FILTER_ENACT_SVLAN_MASK;
                break;
            case FILTER_ENACT_POLICING_2:
                if(pFilter_action->filterPolicingIdx[2] >= ((RTKSW_MAX_METER_ID + 1) + RTL8371C_MAX_LOG_CNT_NUM))
                    return RT_ERR_INPUT;

                aclAct.sact = FILTER_ENACT_SVLAN_TYPE(actType);
                aclAct.svidx_sact = pFilter_action->filterPolicingIdx[2];
                aclActCtrl |= FILTER_ENACT_SVLAN_MASK;
                break;
            case FILTER_ENACT_POLICING_0:
                if(pFilter_action->filterPolicingIdx[0] >= ((RTKSW_MAX_METER_ID + 1) + RTL8371C_MAX_LOG_CNT_NUM))
                    return RT_ERR_INPUT;

                aclAct.aclmeteridx = pFilter_action->filterPolicingIdx[0];
                aclActCtrl |= FILTER_ENACT_POLICING_MASK;
                break;
            case FILTER_ENACT_PRIORITY:
            case FILTER_ENACT_1P_REMARK:
                if(pFilter_action->filterPriority > RTL8371C_ACL_PRIMAX)
                    return RT_ERR_INPUT;

                aclAct.priact = FILTER_ENACT_PRI_TYPE(actType);
                aclAct.pridx = pFilter_action->filterPriority;
                aclActCtrl |= FILTER_ENACT_PRIORITY_MASK;
                break;
            case FILTER_ENACT_DSCP_REMARK:
                if(pFilter_action->filterPriority > RTL8371C_ACL_DSCPMAX)
                    return RT_ERR_INPUT;

                aclAct.priact = FILTER_ENACT_PRI_TYPE(actType);
                aclAct.pridx = pFilter_action->filterPriority;
                aclActCtrl |= FILTER_ENACT_PRIORITY_MASK;
                break;
            case FILTER_ENACT_POLICING_3:
                if(pFilter_action->filterPriority >= ((RTKSW_MAX_METER_ID + 1) + RTL8371C_MAX_LOG_CNT_NUM))
                    return RT_ERR_INPUT;

                aclAct.priact = FILTER_ENACT_PRI_TYPE(actType);
                aclAct.pridx = pFilter_action->filterPolicingIdx[3];
                aclActCtrl |= FILTER_ENACT_PRIORITY_MASK;
                break;
            case FILTER_ENACT_DROP:

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(FILTER_ENACT_REDIRECT);
                aclAct.fwdact_ext = FALSE;

                aclAct.fwdpmask = 0;
                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_REDIRECT:
                RTKSW_CHK_PORTMASK_VALID(unit, &pFilter_action->filterPortmask);

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(actType);
                aclAct.fwdact_ext = FALSE;

                if(rtksw_switch_portmask_L2P_get(unit, &pFilter_action->filterPortmask, &portmask) != RT_ERR_OK)
                    return RT_ERR_PORT_MASK;
                aclAct.fwdpmask = portmask;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;

            case FILTER_ENACT_ADD_DSTPORT:
                RTKSW_CHK_PORTMASK_VALID(unit, &pFilter_action->filterPortmask);

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(actType);
                aclAct.fwdact_ext = FALSE;

                if(rtksw_switch_portmask_L2P_get(unit, &pFilter_action->filterPortmask, &portmask) != RT_ERR_OK)
                    return RT_ERR_PORT_MASK;
                aclAct.fwdpmask = portmask;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_MIRROR:
                RTKSW_CHK_PORTMASK_VALID(unit, &pFilter_action->filterPortmask);

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(actType);
                aclAct.cact_ext = FALSE;

                if(rtksw_switch_portmask_L2P_get(unit, &pFilter_action->filterPortmask, &portmask) != RT_ERR_OK)
                    return RT_ERR_PORT_MASK;
                aclAct.fwdpmask = portmask;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_TRAP_CPU:

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(actType);
                aclAct.fwdact_ext = FALSE;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_COPY_CPU:
                if((retVal = reg16_field_read(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TRAP_PORTf, &cpuPort)) != RT_ERR_OK)
                    return retVal;

                aclAct.fwdact = FILTER_ENACT_FWD_TYPE(FILTER_ENACT_MIRROR);
                aclAct.fwdact_ext = FALSE;

                aclAct.fwdpmask = 1 << cpuPort;
                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;
            case FILTER_ENACT_ISOLATION:
                RTKSW_CHK_PORTMASK_VALID(unit, &pFilter_action->filterPortmask);

                aclAct.fwdact_ext = TRUE;

                if(rtksw_switch_portmask_L2P_get(unit, &pFilter_action->filterPortmask, &portmask) != RT_ERR_OK)
                    return RT_ERR_PORT_MASK;
                aclAct.fwdpmask = portmask;

                aclActCtrl |= FILTER_ENACT_FWD_MASK;
                break;

            case FILTER_ENACT_INTERRUPT:

                aclAct.aclint = TRUE;
                aclActCtrl |= FILTER_ENACT_INTGPIO_MASK;
                break;
            case FILTER_ENACT_GPO:
                if (pFilter_action->filterPin > RTL8371C_ACLGPIOPINNO)
                    return RT_ERR_INPUT;
                aclAct.gpio_en = TRUE;
                aclAct.gpio_pin = pFilter_action->filterPin;
                aclActCtrl |= FILTER_ENACT_INTGPIO_MASK;
                break;
             case FILTER_ENACT_EGRESSCTAG_TAG:

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_VLANONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_TAGONLY;
                }
                aclAct.tag_fmt = FILTER_CTAGFMT_TAG;
                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_EGRESSCTAG_UNTAG:

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_VLANONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_TAGONLY;
                }
                aclAct.tag_fmt = FILTER_CTAGFMT_UNTAG;
                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_EGRESSCTAG_KEEP:

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_VLANONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_TAGONLY;
                }
                aclAct.tag_fmt = FILTER_CTAGFMT_KEEP;
                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
             case FILTER_ENACT_EGRESSCTAG_KEEPAND1PRMK:

                if(aclActCtrl &(FILTER_ENACT_CVLAN_MASK))
                {
                    if(aclAct.cact_ext == FILTER_ENACT_CACTEXT_VLANONLY)
                        aclAct.cact_ext = FILTER_ENACT_CACTEXT_BOTHVLANTAG;
                }
                else
                {
                    aclAct.cact_ext = FILTER_ENACT_CACTEXT_TAGONLY;
                }
                aclAct.tag_fmt = FILTER_CTAGFMT_KEEP1PRMK;
                aclActCtrl |= FILTER_ENACT_CVLAN_MASK;
                break;
           default:
                return RT_ERR_FILTER_INACL_ACT_NOT_SUPPORT;
            }
        }
    }


    /*check if free ACL rules are enough*/
    for(i = filter_id; i < (filter_id + noRulesAdd); i++)
    {
        if((retVal = _rtl8371c_getAclRule(unit, i, &tempRule)) != RT_ERR_OK )
            return retVal;

        if(tempRule.valid == TRUE)
        {
            return RT_ERR_TBL_FULL;
        }
    }

    ruleId = 0;
    for(i = 0; i < RTL8371C_ACLTEMPLATENO; i++)
    {
        if(aclRule[i].valid == TRUE)
        {
            /* write ACL action control */
            regData = (aclActCtrl & ACL_ACT_CVLAN_ENABLE_MASK) ? 1 : 0;
            if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)(filter_id + ruleId), RTL8371C_ACT_CVIDf, &regData)) != RT_ERR_OK)
                return retVal;

            regData = (aclActCtrl & ACL_ACT_SVLAN_ENABLE_MASK) ? 1 : 0;
            if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)(filter_id + ruleId), RTL8371C_ACT_SVIDf, &regData)) != RT_ERR_OK)
                return retVal;

            regData = (aclActCtrl & ACL_ACT_PRIORITY_ENABLE_MASK) ? 1 : 0;
            if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)(filter_id + ruleId), RTL8371C_ACT_PRIORITYf, &regData)) != RT_ERR_OK)
                return retVal;

            regData = (aclActCtrl & ACL_ACT_POLICING_ENABLE_MASK) ? 1 : 0;
            if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)(filter_id + ruleId), RTL8371C_ACT_POLICINGf, &regData)) != RT_ERR_OK)
                return retVal;

            regData = (aclActCtrl & ACL_ACT_FWD_ENABLE_MASK) ? 1 : 0;
            if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)(filter_id + ruleId), RTL8371C_ACT_FORWARDf, &regData)) != RT_ERR_OK)
                return retVal;

            regData = (aclActCtrl & ACL_ACT_INTGPIO_ENABLE_MASK) ? 1 : 0;
            if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)(filter_id + ruleId), RTL8371C_ACT_GPIOf, &regData)) != RT_ERR_OK)
                return retVal;

            /* write ACL action */
            if((retVal = _rtl8371c_setAclAct(unit, filter_id + ruleId, &aclAct)) != RT_ERR_OK )
                return retVal;

            /* write ACL not */
            regData = pFilter_cfg->invert;
            if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)(filter_id + ruleId), RTL8371C_OP_NOTf, &regData)) != RT_ERR_OK)
                return retVal;

            /* write ACL rule */
            if((retVal = _rtl8371c_setAclRule(unit, filter_id + ruleId, &aclRule[i])) != RT_ERR_OK )
                return retVal;

            /* only the first rule will be written with input action control, aclActCtrl of other rules will be zero */
            aclActCtrl = 0;
            osal_memset(&aclAct, 0, sizeof(rtl8371c_acl_act_t));

            ruleId ++;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_cfg_del
 * Description:
 *      Delete an ACL configuration from ASIC
 * Input:
 *      unit        - Unit ID
 *      filter_id   - Start index of ACL configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_FILTER_ENTRYIDX  - Invalid filter_id.
 * Note:
 *      This function delete a group of ACL rules starting from filter_id.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_cfg_del(rtksw_uint32 unit, rtksw_filter_id_t filter_id)
{
    rtl8371c_aclrule initRule;
    rtl8371c_acl_act_t initAct;
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(filter_id > RTL8371C_ACLRULEMAX )
        return RT_ERR_FILTER_ENTRYIDX;

    osal_memset(&initRule, 0, sizeof(rtl8371c_aclrule));
    osal_memset(&initAct, 0, sizeof(rtl8371c_acl_act_t));

    if((retVal = _rtl8371c_setAclRule(unit, filter_id, &initRule)) != RT_ERR_OK)
        return retVal;

    regData = 1;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_CVIDf, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_SVIDf, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_PRIORITYf, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_POLICINGf, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_FORWARDf, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_GPIOf, &regData)) != RT_ERR_OK)
        return retVal;

    if((retVal = _rtl8371c_setAclAct(unit, filter_id, &initAct)) != RT_ERR_OK)
        return retVal;

    regData = 0;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_OP_NOTf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

static rtksw_api_ret_t _rtl8371c_getAclAct(rtksw_uint32 unit, rtksw_uint32 index, rtl8371c_acl_act_t *pAclAct)
{
    rtl8371c_acl_act_entry_t aclActEntry;
    ret_t retVal;

    if(index > RTL8371C_ACLRULEMAX)
        return RT_ERR_OUT_OF_RANGE;

    osal_memset(&aclActEntry, 0x00, sizeof(rtl8371c_acl_act_entry_t));

    if ((retVal = table16_read(unit, RTL8371C_ACL_ACT_ENTRYt, index, (rtksw_uint32 *)&aclActEntry)) != RT_ERR_OK)
        return retVal;

#ifdef CONFIG_VIRTUAL_REG_ASICDRV_TEST
    osal_memcpy(&aclActEntry, &rtl8371cVirtualAclActTable[index], sizeof(rtl8371c_acl_act_entry_t));
#endif

     if ((retVal = _rtl8371c_aclActStSmi2User(unit, pAclAct, &aclActEntry)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_cfg_get
 * Description:
 *      Get one ingress acl configuration from ASIC.
 * Input:
 *      unit            - Unit ID
 *      filter_id       - Start index of ACL configuration.
 * Output:
 *      pFilter_cfg     - buffer pointer of ingress acl data
 *      pFilter_action  - buffer pointer of ingress acl action
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_NULL_POINTER     - Pointer pFilter_action or pFilter_cfg point to NULL.
 *      RT_ERR_FILTER_ENTRYIDX  - Invalid entry index.
 * Note:
 *      This function get configuration from ASIC.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_cfg_get(rtksw_uint32 unit, rtksw_filter_id_t filter_id, rtksw_filter_cfg_raw_t *pFilter_cfg, rtksw_filter_action_t *pAction)
{
    rtksw_api_ret_t               retVal;
    rtksw_uint32                  i;
    rtksw_uint32                  regData;
    rtl8371c_aclrule            aclRule;
    rtl8371c_acl_act_t          aclAct;
    rtksw_uint32                  cpuPort;
    rtksw_uint32                  phyPmask;
    rtksw_filter_template_t       aclTemplate;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pFilter_cfg || NULL == pAction)
        return RT_ERR_NULL_POINTER;

    if(filter_id > RTL8371C_ACLRULEMAX)
        return RT_ERR_ENTRY_INDEX;

    if ((retVal = _rtl8371c_getAclRule(unit, filter_id, &aclRule)) != RT_ERR_OK)
        return retVal;

    /* Check valid */
    if(aclRule.valid == 0)
    {
        pFilter_cfg->valid = RTKSW_DISABLED;
        return RT_ERR_OK;
    }

    phyPmask = aclRule.data_bits.active_portmsk;
    if(rtksw_switch_portmask_P2L_get(unit, phyPmask,&(pFilter_cfg->activeport.value)) != RT_ERR_OK)
        return RT_ERR_FAILED;

    phyPmask = aclRule.care_bits.active_portmsk;
    if(rtksw_switch_portmask_P2L_get(unit, phyPmask,&(pFilter_cfg->activeport.mask)) != RT_ERR_OK)
        return RT_ERR_FAILED;

    for(i = 0; i <= CARE_TAG_IPV6; i++)
    {
        if(aclRule.data_bits.tag_exist & (1 << i))
            pFilter_cfg->careTag.tagType[i].value = 1;
        else
            pFilter_cfg->careTag.tagType[i].value = 0;

        if (aclRule.care_bits.tag_exist & (1 << i))
            pFilter_cfg->careTag.tagType[i].mask = 1;
        else
            pFilter_cfg->careTag.tagType[i].mask = 0;
    }

    if(rtl8371C_filter_advanceCaretagField[aclRule.data_bits.type][0] == TRUE)
    {
        /* Advanced Care tag setting */
        for(i = CARE_TAG_TCP; i < CARE_TAG_END; i++)
        {
            if(aclRule.data_bits.field[rtl8371C_filter_advanceCaretagField[aclRule.data_bits.type][1]] & (0x0001 << (i-CARE_TAG_TCP)) )
                pFilter_cfg->careTag.tagType[i].value = 1;
            else
                pFilter_cfg->careTag.tagType[i].value = 0;

            if(aclRule.care_bits.field[rtl8371C_filter_advanceCaretagField[aclRule.care_bits.type][1]] & (0x0001 << (i-CARE_TAG_TCP)) )
                pFilter_cfg->careTag.tagType[i].mask = 1;
            else
                pFilter_cfg->careTag.tagType[i].mask = 0;
        }
    }

    for(i = 0; i < RTL8371C_ACLRULEFIELDNO; i++)
    {
        pFilter_cfg->careFieldRaw[i] = aclRule.care_bits.field[i];
        pFilter_cfg->dataFieldRaw[i] = aclRule.data_bits.field[i];
    }

    if ((retVal = reg16_array_field_read(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_OP_NOTf, &regData)) != RT_ERR_OK)
        return retVal;

    pFilter_cfg->invert = regData;

    pFilter_cfg->valid = aclRule.valid;

    osal_memset(pAction, 0, sizeof(rtksw_filter_action_t));
    osal_memset(&aclAct, 0, sizeof(rtl8371c_acl_act_t));

    if ((retVal = _rtl8371c_getAclAct(unit, filter_id, &aclAct)) != RT_ERR_OK)
        return retVal;

    /* FILTER_ENACT_FWD_MASK */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_FORWARDf, &regData)) != RT_ERR_OK)
        return retVal;

    if(regData == 1)
    {
        if(TRUE == aclAct.fwdact_ext)
        {
            pAction->actEnable[FILTER_ENACT_ISOLATION] = TRUE;

            phyPmask = aclAct.fwdpmask;
            if(rtksw_switch_portmask_P2L_get(unit, phyPmask,&(pAction->filterPortmask)) != RT_ERR_OK)
                return RT_ERR_FAILED;
        }
        else if(aclAct.fwdact == RTL8371C_ACL_FWD_TRAP)
        {
            pAction->actEnable[FILTER_ENACT_TRAP_CPU] = TRUE;
        }
        else if (aclAct.fwdact == RTL8371C_ACL_FWD_MIRRORFUNTION )
        {
            pAction->actEnable[FILTER_ENACT_MIRROR] = TRUE;

            phyPmask = aclAct.fwdpmask;
            if(rtksw_switch_portmask_P2L_get(unit, phyPmask,&(pAction->filterPortmask)) != RT_ERR_OK)
                return RT_ERR_FAILED;
        }
        else if (aclAct.fwdact == RTL8371C_ACL_FWD_REDIRECT)
        {
            if(aclAct.fwdpmask == 0 )
                pAction->actEnable[FILTER_ENACT_DROP] = TRUE;
            else
            {
                pAction->actEnable[FILTER_ENACT_REDIRECT] = TRUE;

                phyPmask = aclAct.fwdpmask;
                if(rtksw_switch_portmask_P2L_get(unit, phyPmask,&(pAction->filterPortmask)) != RT_ERR_OK)
                    return RT_ERR_FAILED;
            }
        }
        else if (aclAct.fwdact == RTL8371C_ACL_FWD_MIRROR)
        {
            if ((retVal = reg16_field_read(unit, RTL8371C_CPU_TAG_CTRLr, RTL8371C_CPU_TRAP_PORTf, &cpuPort)) != RT_ERR_OK)
                return retVal;
                
            if (aclAct.fwdpmask == (1UL << cpuPort))
                pAction->actEnable[FILTER_ENACT_COPY_CPU] = TRUE;
            else
            {
                pAction->actEnable[FILTER_ENACT_ADD_DSTPORT] = TRUE;

                phyPmask = aclAct.fwdpmask;
                if(rtksw_switch_portmask_P2L_get(unit, phyPmask,&(pAction->filterPortmask)) != RT_ERR_OK)
                    return RT_ERR_FAILED;
            }
        }
        else
        {
            return RT_ERR_FAILED;
        }
    }

    /* FILTER_ENACT_POLICING_MASK */ 
    if ((retVal = reg16_array_field_read(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_POLICINGf, &regData)) != RT_ERR_OK)
        return retVal;

    if(regData == 1)
    {
        pAction->actEnable[FILTER_ENACT_POLICING_0] = TRUE;
        pAction->filterPolicingIdx[0] = aclAct.aclmeteridx;
    }

    /* FILTER_ENACT_PRIORITY_MASK */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_PRIORITYf, &regData)) != RT_ERR_OK)
        return retVal;

    if(regData == 1)
    {
        if(aclAct.priact == FILTER_ENACT_PRI_TYPE(FILTER_ENACT_PRIORITY))
        {
            pAction->actEnable[FILTER_ENACT_PRIORITY] = TRUE;
            pAction->filterPriority = aclAct.pridx;
        }
        else if(aclAct.priact == FILTER_ENACT_PRI_TYPE(FILTER_ENACT_1P_REMARK))
        {
            pAction->actEnable[FILTER_ENACT_1P_REMARK] = TRUE;
            pAction->filterPriority = aclAct.pridx;
        }
        else if(aclAct.priact == FILTER_ENACT_PRI_TYPE(FILTER_ENACT_DSCP_REMARK))
        {
            pAction->actEnable[FILTER_ENACT_DSCP_REMARK] = TRUE;
            pAction->filterPriority = aclAct.pridx;
        }
        else if(aclAct.priact == FILTER_ENACT_PRI_TYPE(FILTER_ENACT_POLICING_3))
        {
            pAction->actEnable[FILTER_ENACT_POLICING_3] = TRUE;
            pAction->filterPolicingIdx[3]  = aclAct.pridx;
        }
    }

    /* FILTER_ENACT_SVLAN_MASK */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_SVIDf, &regData)) != RT_ERR_OK)
        return retVal;

    if(regData == 1)
    {
        if(aclAct.sact == FILTER_ENACT_SVLAN_TYPE(FILTER_ENACT_SVLAN_INGRESS))
        {
            pAction->actEnable[FILTER_ENACT_SVLAN_INGRESS] = TRUE;
            pAction->filterSvlanIdx = aclAct.svidx_sact;
            pAction->filterSvlanVid = aclAct.svidx_sact;
        }
        else if(aclAct.sact == FILTER_ENACT_SVLAN_TYPE(FILTER_ENACT_SVLAN_EGRESS))
        {
            pAction->actEnable[FILTER_ENACT_SVLAN_EGRESS] = TRUE;
            pAction->filterSvlanIdx = aclAct.svidx_sact;
            pAction->filterSvlanVid = aclAct.svidx_sact;
        }
        else if(aclAct.sact == FILTER_ENACT_SVLAN_TYPE(FILTER_ENACT_SVLAN_CVID))
            pAction->actEnable[FILTER_ENACT_SVLAN_CVID] = TRUE;
        else if(aclAct.sact == FILTER_ENACT_SVLAN_TYPE(FILTER_ENACT_POLICING_2))
        {
            pAction->actEnable[FILTER_ENACT_POLICING_2] = TRUE;
            pAction->filterPolicingIdx[2]  = aclAct.svidx_sact;
        }
    }

    /* FILTER_ENACT_CVLAN_MASK */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_CVIDf, &regData)) != RT_ERR_OK)
        return retVal;

    if(regData == 1)
    {
        if(FILTER_ENACT_CACTEXT_TAGONLY == aclAct.cact_ext ||
            FILTER_ENACT_CACTEXT_BOTHVLANTAG == aclAct.cact_ext )
        {
            if(FILTER_CTAGFMT_UNTAG == aclAct.tag_fmt)
            {
                pAction->actEnable[FILTER_ENACT_EGRESSCTAG_UNTAG] = TRUE;
            }
            else if(FILTER_CTAGFMT_TAG == aclAct.tag_fmt)
            {
                pAction->actEnable[FILTER_ENACT_EGRESSCTAG_TAG] = TRUE;
            }
            else if(FILTER_CTAGFMT_KEEP == aclAct.tag_fmt)
            {
                pAction->actEnable[FILTER_ENACT_EGRESSCTAG_KEEP] = TRUE;
            }
             else if(FILTER_CTAGFMT_KEEP1PRMK== aclAct.tag_fmt)
            {
                pAction->actEnable[FILTER_ENACT_EGRESSCTAG_KEEPAND1PRMK] = TRUE;
            }

        }

        if(FILTER_ENACT_CACTEXT_VLANONLY == aclAct.cact_ext ||
            FILTER_ENACT_CACTEXT_BOTHVLANTAG == aclAct.cact_ext )
        {
            if(aclAct.cact == FILTER_ENACT_CVLAN_TYPE(FILTER_ENACT_CVLAN_INGRESS))
            {
                pAction->actEnable[FILTER_ENACT_CVLAN_INGRESS] = TRUE;
                pAction->filterCvlanIdx  = aclAct.cvidx_cact;
                pAction->filterCvlanVid  = aclAct.cvidx_cact;
            }
            else if(aclAct.cact == FILTER_ENACT_CVLAN_TYPE(FILTER_ENACT_CVLAN_EGRESS))
            {
                pAction->actEnable[FILTER_ENACT_CVLAN_EGRESS] = TRUE;
                pAction->filterCvlanIdx  = aclAct.cvidx_cact;
                pAction->filterCvlanVid  = aclAct.cvidx_cact;
            }
            else if(aclAct.cact == FILTER_ENACT_CVLAN_TYPE(FILTER_ENACT_CVLAN_SVID))
            {
                pAction->actEnable[FILTER_ENACT_CVLAN_SVID] = TRUE;
            }
            else if(aclAct.cact == FILTER_ENACT_CVLAN_TYPE(FILTER_ENACT_POLICING_1))
            {
                pAction->actEnable[FILTER_ENACT_POLICING_1] = TRUE;
                pAction->filterPolicingIdx[1]  = aclAct.cvidx_cact;
            }
        }
    }

    /* FILTER_ENACT_INTGPIO_MASK */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_ACL_ACT_CTRLr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)filter_id, RTL8371C_ACT_GPIOf, &regData)) != RT_ERR_OK)
        return retVal;

    if(regData == 1)
    {
        if(TRUE == aclAct.aclint)
        {
            pAction->actEnable[FILTER_ENACT_INTERRUPT] = TRUE;
        }

        if(TRUE == aclAct.gpio_en)
        {
            pAction->actEnable[FILTER_ENACT_GPO] = TRUE;
            pAction->filterPin = aclAct.gpio_pin;
        }
    }

    /* Get field type of RAW data */
    osal_memset(&aclTemplate, 0x00, sizeof(rtksw_filter_template_t));
    aclTemplate.index = aclRule.data_bits.type;
    if ((retVal = dal_rtl8371c_filter_igrAcl_template_get(unit, &aclTemplate))!= RT_ERR_OK)
        return retVal;

    for(i = 0; i < RTL8371C_ACLRULEFIELDNO; i++)
    {
        pFilter_cfg->fieldRawType[i] = aclTemplate.fieldType[i];
    }/* end of for(i...) */

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_unmatchAction_set
 * Description:
 *      Set action to packets when no ACL configuration match
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      action  - Action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function sets action of packets when no ACL configruation matches.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_unmatchAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_unmatch_action_t action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(action >= FILTER_UNMATCH_END)
        return RT_ERR_INPUT;

    if ((retVal = reg16_field_read(unit, RTL8371C_ACL_PORT_UNMATCH_PERMITr, RTL8371C_PMSK_ACTf, &regData)) != RT_ERR_OK)
        return retVal;

    if (action == FILTER_UNMATCH_DROP)
        regData &= ~(0x0001 << rtksw_switch_port_L2P_get(unit, port));
    else
        regData |= (0x0001 << rtksw_switch_port_L2P_get(unit, port));

    if ((retVal = reg16_field_write(unit, RTL8371C_ACL_PORT_UNMATCH_PERMITr, RTL8371C_PMSK_ACTf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_unmatchAction_get
 * Description:
 *      Get action to packets when no ACL configuration match
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pAction - Action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_unmatchAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_unmatch_action_t* pAction)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pAction)
        return RT_ERR_NULL_POINTER;

    /* Check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if ((retVal = reg16_field_read(unit, RTL8371C_ACL_PORT_UNMATCH_PERMITr, RTL8371C_PMSK_ACTf, &regData)) != RT_ERR_OK)
        return retVal;

    if (regData & (0x0001 << rtksw_switch_port_L2P_get(unit, port)))
        *pAction = FILTER_UNMATCH_PERMIT;
    else
        *pAction = FILTER_UNMATCH_DROP;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_state_set
 * Description:
 *      Set state of ingress ACL.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      state   - Ingress ACL state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_state_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_state_t state)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (state >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if ((retVal = reg16_field_read(unit, RTL8371C_ACL_PORT_ENr, RTL8371C_PMSKf, &regData)) != RT_ERR_OK)
        return retVal;

    if (state == RTKSW_ENABLED)
        regData |= (0x0001 << rtksw_switch_port_L2P_get(unit, port));
    else
        regData &= ~(0x0001 << rtksw_switch_port_L2P_get(unit, port));

    if ((retVal = reg16_field_write(unit, RTL8371C_ACL_PORT_ENr, RTL8371C_PMSKf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_state_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pState  - Ingress ACL state.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_state_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_filter_state_t* pState)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pState)
        return RT_ERR_NULL_POINTER;

    /* Check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if ((retVal = reg16_field_read(unit, RTL8371C_ACL_PORT_ENr, RTL8371C_PMSKf, &regData)) != RT_ERR_OK)
        return retVal;

    if (regData & (0x0001 << rtksw_switch_port_L2P_get(unit, port)))
        *pState = RTKSW_ENABLED;
    else
        *pState = RTKSW_DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_template_set
 * Description:
 *      Set template of ingress ACL.
 * Input:
 *      unit        - Unit ID
 *      template    - Ingress ACL template
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function set ACL template.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_template_set(rtksw_uint32 unit, rtksw_filter_template_t *aclTemplate)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 idxField;
    rtksw_uint32 regData;
    rtksw_uint32 regArrayField[] = {RTL8371C_FIELD0_TYPEf, RTL8371C_FIELD1_TYPEf, RTL8371C_FIELD2_TYPEf, RTL8371C_FIELD3_TYPEf, 
                                  RTL8371C_FIELD4_TYPEf, RTL8371C_FIELD5_TYPEf, RTL8371C_FIELD6_TYPEf, RTL8371C_FIELD7_TYPEf};

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (aclTemplate == NULL)
        return RT_ERR_NULL_POINTER;

    if (aclTemplate->index >= RTKSW_MAX_NUM_OF_FILTER_TYPE)
        return RT_ERR_INPUT;

    for(idxField = 0; idxField < RTKSW_MAX_NUM_OF_FILTER_FIELD; idxField++)
    {
        if (aclTemplate->fieldType[idxField] < FILTER_FIELD_RAW_DMAC_15_0 ||
            (aclTemplate->fieldType[idxField] > FILTER_FIELD_RAW_CTAG && aclTemplate->fieldType[idxField] < FILTER_FIELD_RAW_IPV4_SIP_15_0 ) ||
            (aclTemplate->fieldType[idxField] > FILTER_FIELD_RAW_IPV4_DIP_31_16 && aclTemplate->fieldType[idxField] < FILTER_FIELD_RAW_IPV6_SIP_15_0 ) ||
            (aclTemplate->fieldType[idxField] > FILTER_FIELD_RAW_FIELD_VALID && aclTemplate->fieldType[idxField] < FILTER_FIELD_RAW_FIELD_SELECT00 ) ||
             aclTemplate->fieldType[idxField] >= FILTER_FIELD_RAW_END)
        {
            return RT_ERR_INPUT;
        }
    }

    for (idxField = 0; idxField < RTKSW_MAX_NUM_OF_FILTER_FIELD; idxField++)
    {
        regData = (rtksw_uint32)aclTemplate->fieldType[idxField];
        if ((retVal = reg16_array_field_write(unit, RTL8371C_ACL_TEMPLATE_CTRLr, REG_ARRAY_INDEX_NONE, aclTemplate->index, regArrayField[idxField], &regData)) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_template_get
 * Description:
 *      Get template of ingress ACL.
 * Input:
 *      unit        - Unit ID
 *      template    - Ingress ACL template
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This function gets template of ACL.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_template_get(rtksw_uint32 unit, rtksw_filter_template_t *aclTemplate)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 idxField;
    rtksw_uint32 regData;
    rtksw_uint32 regArrayField[] = {RTL8371C_FIELD0_TYPEf, RTL8371C_FIELD1_TYPEf, RTL8371C_FIELD2_TYPEf, RTL8371C_FIELD3_TYPEf, 
                                  RTL8371C_FIELD4_TYPEf, RTL8371C_FIELD5_TYPEf, RTL8371C_FIELD6_TYPEf, RTL8371C_FIELD7_TYPEf};

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (aclTemplate == NULL)
        return RT_ERR_NULL_POINTER;

    if(aclTemplate->index >= RTKSW_MAX_NUM_OF_FILTER_TYPE)
        return RT_ERR_INPUT;

    for (idxField = 0; idxField < RTKSW_MAX_NUM_OF_FILTER_FIELD; idxField++)
    {
        if ((retVal = reg16_array_field_read(unit, RTL8371C_ACL_TEMPLATE_CTRLr, REG_ARRAY_INDEX_NONE, aclTemplate->index, regArrayField[idxField], &regData)) != RT_ERR_OK)
            return retVal;

        aclTemplate->fieldType[idxField] = (rtksw_filter_field_type_raw_t)regData;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_field_sel_set
 * Description:
 *      Set user defined field selectors in HSB
 * Input:
 *      unit        - Unit ID
 *      index       - index of field selector 0-15
 *      format      - Format of field selector
 *      offset      - Retrieving data offset
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      System support 16 user defined field selctors.
 *      Each selector can be enabled or disable.
 *      User can defined retrieving 16-bits in many predefiend
 *      standard l2/l3/l4 payload.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_field_sel_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_field_sel_t format, rtksw_uint32 offset)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(index >= RTL8371C_FIELDSEL_FORMAT_NUMBER)
        return RT_ERR_OUT_OF_RANGE;

    if(format >= FORMAT_END)
        return RT_ERR_OUT_OF_RANGE;

    if(offset > RTL8371C_FIELDSEL_MAX_OFFSET)
        return RT_ERR_OUT_OF_RANGE;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PAR_FIELD00r, REG_ARRAY_INDEX_NONE, index, RTL8371C_FORMATf, (rtksw_uint32 *)&format)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_PAR_FIELD00r, REG_ARRAY_INDEX_NONE, index, RTL8371C_OFFSETf, &offset)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_igrAcl_field_sel_get
 * Description:
 *      Get user defined field selectors in HSB
 * Input:
 *      unit        - Unit ID
 *      index       - index of field selector 0-15
 * Output:
 *      pFormat     - Format of field selector
 *      pOffset     - Retrieving data offset
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      None.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_igrAcl_field_sel_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_field_sel_t *pFormat, rtksw_uint32 *pOffset)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pFormat || NULL == pOffset)
        return RT_ERR_NULL_POINTER;

    if(index >= RTL8371C_FIELDSEL_FORMAT_NUMBER)
        return RT_ERR_OUT_OF_RANGE;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PAR_FIELD00r, REG_ARRAY_INDEX_NONE, index, RTL8371C_FORMATf, &regData)) != RT_ERR_OK)
        return retVal;

    *pFormat = (rtksw_field_sel_t)regData;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_PAR_FIELD00r, REG_ARRAY_INDEX_NONE, index, RTL8371C_OFFSETf, &regData)) != RT_ERR_OK)
        return retVal;

    *pOffset = (rtksw_field_sel_t)regData;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_iprange_set
 * Description:
 *      Set IP Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of IP Range 0-7
 *      type        - IP Range check type, 0:Delete a entry, 1: IPv4_SIP, 2: IPv4_DIP, 3:IPv6_SIP, 4:IPv6_DIP
 *      upperIp     - The upper bound of IP range
 *      lowerIp     - The lower Bound of IP range
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upperIp must be larger or equal than lowerIp.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_iprange_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_iprange_t type, ipaddr_t upperIp, ipaddr_t lowerIp)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(index > RTL8371C_ACLRANGEMAX)
        return RT_ERR_OUT_OF_RANGE;

    if(type >= IPRANGE_END)
        return RT_ERR_OUT_OF_RANGE;

    if(lowerIp > upperIp)
        return RT_ERR_INPUT;

    regData = lowerIp & 0xFFFF;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_LOWER_LSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    regData = (lowerIp >> 16) & 0xFFFF;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_LOWER_HSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    regData = upperIp & 0xFFFF;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_UPPER_LSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    regData = (upperIp >> 16) & 0xFFFF;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_UPPER_HSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_TYPEf, (rtksw_uint32 *)&type)) != RT_ERR_OK)
        return RT_ERR_OK;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_iprange_get
 * Description:
 *      Set IP Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of IP Range 0-7
 * Output:
 *      pType        - IP Range check type, 0:Delete a entry, 1: IPv4_SIP, 2: IPv4_DIP, 3:IPv6_SIP, 4:IPv6_DIP
 *      pUpperIp     - The upper bound of IP range
 *      pLowerIp     - The lower Bound of IP range
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      None.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_iprange_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_iprange_t *pType, ipaddr_t *pUpperIp, ipaddr_t *pLowerIp)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if((NULL == pType) || (NULL == pUpperIp) || (NULL == pLowerIp))
        return RT_ERR_NULL_POINTER;

    if(index > RTL8371C_ACLRANGEMAX)
        return RT_ERR_OUT_OF_RANGE;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_LOWER_LSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;
    
    *pLowerIp = regData & 0xFFFF;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_LOWER_HSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    *pLowerIp |= ((regData << 16) & 0xFFFF0000);

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_UPPER_LSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;

    *pUpperIp = regData & 0xFFFF;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_UPPER_HSBf, &regData)) != RT_ERR_OK)
        return RT_ERR_OK;
    
    *pUpperIp |= ((regData << 16) & 0xFFFF0000);

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_IPr, REG_ARRAY_INDEX_NONE, index, RTL8371C_TYPEf, (rtksw_uint32 *)pType)) != RT_ERR_OK)
        return RT_ERR_OK;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_vidrange_set
 * Description:
 *      Set VID Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of VID Range 0-7
 *      type        - IP Range check type, 0:Delete a entry, 1: CVID, 2: SVID
 *      upperVid    - The upper bound of VID range
 *      lowerVid    - The lower Bound of VID range
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upperVid must be larger or equal than lowerVid.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_vidrange_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_vidrange_t type, rtksw_uint32 upperVid, rtksw_uint32 lowerVid)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index > RTL8371C_ACLRANGEMAX)
        return RT_ERR_OUT_OF_RANGE;

    if (type >= VIDRANGE_END)
        return RT_ERR_OUT_OF_RANGE;

    if (lowerVid > upperVid)
        return RT_ERR_INPUT;

    if ((upperVid > 0xFFF) || (lowerVid > 0xFFF))
        return RT_ERR_OUT_OF_RANGE;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_VIDr, REG_ARRAY_INDEX_NONE, index, RTL8371C_LOWERf, &lowerVid)) != RT_ERR_OK)
        return RT_ERR_OK;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_VIDr, REG_ARRAY_INDEX_NONE, index, RTL8371C_UPPERf, &upperVid)) != RT_ERR_OK)
        return RT_ERR_OK;
   
    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_VIDr, REG_ARRAY_INDEX_NONE, index, RTL8371C_TYPEf, (rtksw_uint32 *)&type)) != RT_ERR_OK)
        return RT_ERR_OK;

    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_vidrange_get
 * Description:
 *      Get VID Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of VID Range 0-7
 * Output:
 *      pType        - IP Range check type, 0:Unused, 1: CVID, 2: SVID
 *      pUpperVid    - The upper bound of VID range
 *      pLowerVid    - The lower Bound of VID range
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      None.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_vidrange_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_vidrange_t *pType, rtksw_uint32 *pUpperVid, rtksw_uint32 *pLowerVid)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index > RTL8371C_ACLRANGEMAX)
        return RT_ERR_OUT_OF_RANGE;

    if ((pType == NULL) || (pUpperVid == NULL) || (pLowerVid == NULL))
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_VIDr, REG_ARRAY_INDEX_NONE, index, RTL8371C_LOWERf, pLowerVid)) != RT_ERR_OK)
        return RT_ERR_OK;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_VIDr, REG_ARRAY_INDEX_NONE, index, RTL8371C_UPPERf, pUpperVid)) != RT_ERR_OK)
        return RT_ERR_OK;
   
    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_VIDr, REG_ARRAY_INDEX_NONE, index, RTL8371C_TYPEf, (rtksw_uint32 *)pType)) != RT_ERR_OK)
        return RT_ERR_OK;
    
    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_portrange_set
 * Description:
 *      Set Port Range check
 * Input:
 *      unit        - Unit ID
 *      index       - index of Port Range 0-7
 *      type        - IP Range check type, 0:Delete a entry, 1: Source Port, 2: Destnation Port
 *      upperPort   - The upper bound of Port range
 *      lowerPort   - The lower Bound of Port range
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upperPort must be larger or equal than lowerPort.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_portrange_set(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_portrange_t type, rtksw_uint32 upperPort, rtksw_uint32 lowerPort)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index > RTL8371C_ACLRANGEMAX)
        return RT_ERR_OUT_OF_RANGE;

    if (type >= PORTRANGE_END)
        return RT_ERR_OUT_OF_RANGE;

    if (lowerPort > upperPort)
        return RT_ERR_INPUT;

    if ((upperPort > 0xFFFF) || (lowerPort > 0xFFFF))
        return RT_ERR_OUT_OF_RANGE;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_PORTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_LOWERf, &lowerPort)) != RT_ERR_OK)
        return RT_ERR_OK;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_PORTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_UPPERf, &upperPort)) != RT_ERR_OK)
        return RT_ERR_OK;
   
    if ((retVal = reg16_array_field_write(unit, RTL8371C_RNG_CHK_PORTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_TYPEf, (rtksw_uint32 *)&type)) != RT_ERR_OK)
        return RT_ERR_OK;
    
    return RT_ERR_OK;
}

/* Function Name:
 *       dal_rtl8371c_filter_portrange_get
 * Description:
 *      Set Port Range check
 * Input:
 *      unit    - Unit ID
 *      index       - index of Port Range 0-7
 * Output:
 *      pType       - IP Range check type, 0:Delete a entry, 1: Source Port, 2: Destnation Port
 *      pUpperPort  - The upper bound of Port range
 *      pLowerPort  - The lower Bound of Port range
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
rtksw_api_ret_t  dal_rtl8371c_filter_portrange_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_filter_portrange_t *pType, rtksw_uint32 *pUpperPort, rtksw_uint32 *pLowerPort)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (index > RTL8371C_ACLRANGEMAX)
        return RT_ERR_OUT_OF_RANGE;

    if ((pType == NULL) || (pUpperPort == NULL) || (pLowerPort == NULL))
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_PORTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_LOWERf, pLowerPort)) != RT_ERR_OK)
        return RT_ERR_OK;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_PORTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_UPPERf, pUpperPort)) != RT_ERR_OK)
        return RT_ERR_OK;
   
    if ((retVal = reg16_array_field_read(unit, RTL8371C_RNG_CHK_PORTr, REG_ARRAY_INDEX_NONE, index, RTL8371C_TYPEf, (rtksw_uint32 *)pType)) != RT_ERR_OK)
        return RT_ERR_OK;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_filter_igrAclPolarity_set
 * Description:
 *      Set ACL Goip control palarity
 * Input:
 *      unit        - Unit ID
 *      polarity    - 1: High, 0: Low
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      none
 */
rtksw_api_ret_t dal_rtl8371c_filter_igrAclPolarity_set(rtksw_uint32 unit, rtksw_uint32 polarity)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(polarity > 1)
        return RT_ERR_OUT_OF_RANGE;

    regData = polarity;
    if ((retVal = reg16_field_write(unit, RTL8371C_ACL_GPIO_POLARr, RTL8371C_ACL_GPIO_POLARITYf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_filter_igrAclPolarity_get
 * Description:
 *      Get ACL Goip control palarity
 * Input:
 *      unit        - Unit ID
 *      pPolarity   - 1: High, 0: Low
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Success
 *      RT_ERR_SMI      - SMI access error
 * Note:
 *      none
 */
rtksw_api_ret_t dal_rtl8371c_filter_igrAclPolarity_get(rtksw_uint32 unit, rtksw_uint32* pPolarity)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pPolarity)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_ACL_GPIO_POLARr, RTL8371C_ACL_GPIO_POLARITYf, &regData)) != RT_ERR_OK)
        return retVal;

    *pPolarity = regData;
    return RT_ERR_OK;
}
