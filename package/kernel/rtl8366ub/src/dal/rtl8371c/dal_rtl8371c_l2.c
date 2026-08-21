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
#include <dal/rtl8371c/dal_rtl8371c_l2.h>
#include <dal/rtl8371c/dal_rtl8371c_vlan.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>
#include <osal/lib.h>

/*
 * Macro Declaration
 */
rtksw_uint32 l2TableDataWriteReg[RTL8371C_LUT_ENTRY_LEN] =
{
    RTL8371C_ACCESS_DATA_W00r,
    RTL8371C_ACCESS_DATA_W01r,
    RTL8371C_ACCESS_DATA_W02r,
    RTL8371C_ACCESS_DATA_W03r,
    RTL8371C_ACCESS_DATA_W04r,
};

rtksw_uint32 l2TableDataReadReg[RTL8371C_LUT_ENTRY_LEN] =
{
    RTL8371C_ACCESS_DATA_R00r,
    RTL8371C_ACCESS_DATA_R01r,
    RTL8371C_ACCESS_DATA_R02r,
    RTL8371C_ACCESS_DATA_R03r,
    RTL8371C_ACCESS_DATA_R04r,
};

static void _rtl8371c_fdbStUser2Smi( rtl8371c_luttb *pLutSt, rtl8371c_l2_entry_t *pEntry)
{
    /* L3 lookup */
    if(pLutSt->l3lookup)
    {
        pEntry->entry_data[0] = (pLutSt->sip & 0x0000FFFF);

        pEntry->entry_data[1] = (pLutSt->sip & 0xFFFF0000) >> 16;

        pEntry->entry_data[2] = (pLutSt->dip & 0x0000FFFF);

        pEntry->entry_data[3] = (pLutSt->dip & 0x0FFF0000) >> 16;
        pEntry->entry_data[3] |= (pLutSt->l3lookup & 0x0001) << 12;
        pEntry->entry_data[3] |= (pLutSt->mbr & 0x0003) << 14;

        pEntry->entry_data[4] |= (pLutSt->mbr & 0x00FC) >> 2;
        pEntry->entry_data[4] |= (pLutSt->nosalearn & 0x0001) << 6;
    }
    else if(pLutSt->mac.octet[0] & 0x01) /*Multicast L2 Lookup*/
    {
        pEntry->entry_data[0] |= pLutSt->mac.octet[5];
        pEntry->entry_data[0] |= pLutSt->mac.octet[4] << 8;

        pEntry->entry_data[1] |= pLutSt->mac.octet[3];
        pEntry->entry_data[1] |= pLutSt->mac.octet[2] << 8;

        pEntry->entry_data[2] |= pLutSt->mac.octet[1];
        pEntry->entry_data[2] |= pLutSt->mac.octet[0] << 8;

        pEntry->entry_data[3] |= pLutSt->cvid_fid;
        pEntry->entry_data[3] |= (pLutSt->l3lookup & 0x0001) << 12;
        pEntry->entry_data[3] |= (pLutSt->ivl_svl & 0x0001) << 13;
        pEntry->entry_data[3] |= (pLutSt->mbr & 0x0003) << 14;

        pEntry->entry_data[4] |= (pLutSt->mbr & 0x00FC) >> 2;
        pEntry->entry_data[4] |= (pLutSt->nosalearn & 0x0001) << 6;
    }
    else /*Asic auto-learning*/
    {
        pEntry->entry_data[0] |= pLutSt->mac.octet[5];
        pEntry->entry_data[0] |= pLutSt->mac.octet[4] << 8;

        pEntry->entry_data[1] |= pLutSt->mac.octet[3];
        pEntry->entry_data[1] |= pLutSt->mac.octet[2] << 8;

        pEntry->entry_data[2] |= pLutSt->mac.octet[1];
        pEntry->entry_data[2] |= pLutSt->mac.octet[0] << 8;

        pEntry->entry_data[3] |= pLutSt->cvid_fid;
        pEntry->entry_data[3] |= (pLutSt->l3lookup & 0x0001) << 12;
        pEntry->entry_data[3] |= (pLutSt->ivl_svl & 0x0001) << 13;
        pEntry->entry_data[3] |= (pLutSt->spa & 0x0003) << 14;

        pEntry->entry_data[4] |= (pLutSt->spa & 0x0004) >> 2;
        pEntry->entry_data[4] |= (pLutSt->age & 0x0007) << 1;
        pEntry->entry_data[4] |= (pLutSt->nosalearn & 0x0001) << 6;
    }
}

static void _rtl8371c_fdbStSmi2User( rtl8371c_luttb *pLutSt, rtl8371c_l2_entry_t *pEntry)
{
    /*L3 lookup*/
    if(pEntry->entry_data[3] & 0x1000)
    {
        pLutSt->sip             = pEntry->entry_data[0] | (pEntry->entry_data[1] << 16);
        pLutSt->dip             = 0xE0000000 | pEntry->entry_data[2] | ((pEntry->entry_data[3] & 0x0FFF) << 16);
        pLutSt->mbr             = ((pEntry->entry_data[4] & 0x003F) << 2) | ((pEntry->entry_data[3] & 0xC000) >> 14);
        pLutSt->l3lookup        = (pEntry->entry_data[3] & 0x1000) >> 12;
        pLutSt->nosalearn       = (pEntry->entry_data[4] & 0x0040) >> 6;
    }
    else if(pEntry->entry_data[2] & 0x0100) /*Multicast L2 Lookup*/
    {
        pLutSt->mac.octet[0]    = (pEntry->entry_data[2] & 0xFF00) >> 8;
        pLutSt->mac.octet[1]    = (pEntry->entry_data[2] & 0x00FF);
        pLutSt->mac.octet[2]    = (pEntry->entry_data[1] & 0xFF00) >> 8;
        pLutSt->mac.octet[3]    = (pEntry->entry_data[1] & 0x00FF);
        pLutSt->mac.octet[4]    = (pEntry->entry_data[0] & 0xFF00) >> 8;
        pLutSt->mac.octet[5]    = (pEntry->entry_data[0] & 0x00FF);

        pLutSt->cvid_fid        = pEntry->entry_data[3] & 0x0FFF;
        pLutSt->mbr             = ((pEntry->entry_data[4] & 0x003F) << 2) | ((pEntry->entry_data[3] & 0xC000) >> 14);

        pLutSt->l3lookup        = (pEntry->entry_data[3] & 0x1000) >> 12;
        pLutSt->ivl_svl         = (pEntry->entry_data[3] & 0x2000) >> 13;
        pLutSt->nosalearn       = (pEntry->entry_data[4] & 0x0040) >> 6;
    }
    else /*Asic auto-learning*/
    {
        pLutSt->mac.octet[0]    = (pEntry->entry_data[2] & 0xFF00) >> 8;
        pLutSt->mac.octet[1]    = (pEntry->entry_data[2] & 0x00FF);
        pLutSt->mac.octet[2]    = (pEntry->entry_data[1] & 0xFF00) >> 8;
        pLutSt->mac.octet[3]    = (pEntry->entry_data[1] & 0x00FF);
        pLutSt->mac.octet[4]    = (pEntry->entry_data[0] & 0xFF00) >> 8;
        pLutSt->mac.octet[5]    = (pEntry->entry_data[0] & 0x00FF);

        pLutSt->cvid_fid        = pEntry->entry_data[3] & 0x0FFF;

        pLutSt->spa             = ((pEntry->entry_data[4] & 0x0001) << 2) | ((pEntry->entry_data[3] & 0xC000) >> 14);
        pLutSt->age             = (pEntry->entry_data[4] & 0x000E) >> 1;

        pLutSt->l3lookup        = (pEntry->entry_data[3] & 0x1000) >> 12;
        pLutSt->ivl_svl         = (pEntry->entry_data[3] & 0x2000) >> 13;
        pLutSt->nosalearn       = (pEntry->entry_data[4] & 0x0040) >> 6;
    }
}

static rtksw_api_ret_t _rtl8371c_getL2LookupTb(rtksw_uint32 unit, rtksw_uint32 method, rtl8371c_luttb *pL2Table)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 fieldData;
    rtksw_uint32 i;
    rtl8371c_l2_entry_t l2Entry;
    rtksw_uint32 busyCounter;

    if(pL2Table->wait_time == 0)
        busyCounter = RTL8371C_LUT_BUSY_CHECK_NO;
    else
        busyCounter = pL2Table->wait_time;

    /* Check Busy status before read */
    while(busyCounter)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &regData)) != RT_ERR_OK)
            return retVal;

        pL2Table->lookup_busy = regData;
        if(!regData)
            break;

        busyCounter--;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /* Prepare read Command */
    if ((retVal = reg16_read(unit, RTL8371C_ACS_CMDr, &regData)) != RT_ERR_OK)
        return retVal;

    /* Set table type */
    fieldData = 4;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_TABLE_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    /*set command type -- 0b0 read*/
    fieldData = 0;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_COMMAND_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    /* Set Read Method */
    fieldData = (rtksw_uint32)method;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_ACCESS_METHODf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    switch(method)
    {
        case RTL8371C_LUTREADMETHOD_ADDRESS:
        case RTL8371C_LUTREADMETHOD_NEXT_ADDRESS:
        case RTL8371C_LUTREADMETHOD_NEXT_L2UC:
        case RTL8371C_LUTREADMETHOD_NEXT_L2MC:
        case RTL8371C_LUTREADMETHOD_NEXT_L3MC:
        case RTL8371C_LUTREADMETHOD_NEXT_L2L3MC:
            fieldData = pL2Table->address;
            if ((retVal = reg16_field_write(unit, RTL8371C_ACS_ADRr, RTL8371C_ADDRESSf, &fieldData)) != RT_ERR_OK)
                return retVal;
            break;
        case RTL8371C_LUTREADMETHOD_MAC:
            osal_memset(&l2Entry, 0x00, sizeof(rtl8371c_l2_entry_t));
            _rtl8371c_fdbStUser2Smi(pL2Table, &l2Entry);

            for (i = 0 ; i < RTL8371C_LUT_ENTRY_LEN ; i++)
            {
                if ((retVal = reg16_write(unit, l2TableDataWriteReg[i], &l2Entry.entry_data[i])) != RT_ERR_OK)
                    return retVal;
            }
            break;
        case RTL8371C_LUTREADMETHOD_NEXT_L2UCSPA:
            fieldData = pL2Table->address;
            if ((retVal = reg16_field_write(unit, RTL8371C_ACS_ADRr, RTL8371C_ADDRESSf, &fieldData)) != RT_ERR_OK)
                return retVal;

            fieldData = (rtksw_uint32)pL2Table->spa;
            if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_SPAf, &fieldData, &regData)) != RT_ERR_OK)
                return retVal;

            break;
        default:
            return RT_ERR_INPUT;
    }

    
    /* Issue command */
    fieldData = 1;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_write(unit, RTL8371C_ACS_CMDr, &regData)) != RT_ERR_OK)
        return retVal;

    /* Wait until reading finishes */
    if(pL2Table->wait_time == 0)
        busyCounter = RTL8371C_LUT_BUSY_CHECK_NO;
    else
        busyCounter = pL2Table->wait_time;

    while(busyCounter)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &regData)) != RT_ERR_OK)
            return retVal;

        pL2Table->lookup_busy = regData;
        if(!regData)
            break;

        busyCounter--;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    if ((retVal = reg16_field_read(unit, RTL8371C_ACS_L2_ADRr, RTL8371C_HIT_STATUSf, &regData)) != RT_ERR_OK)
        return retVal;

    pL2Table->lookup_hit = regData;
    if(!pL2Table->lookup_hit)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    /*Read access address*/
    if ((retVal = reg16_field_read(unit, RTL8371C_ACS_L2_ADRr, RTL8371C_ADDRESSf, &regData)) != RT_ERR_OK)
        return retVal;

    pL2Table->address = (regData & 0x7ff);

    if ((retVal = reg16_field_read(unit, RTL8371C_ACS_L2_ADRr, RTL8371C_TYPEf, &regData)) != RT_ERR_OK)
        return retVal;
    
    pL2Table->address |= (regData << 11);
    pL2Table->lookup_busy = 0;

    /*read L2 entry */
    osal_memset(&l2Entry, 0x00, sizeof(rtl8371c_l2_entry_t));
    for (i = 0 ; i < RTL8371C_LUT_ENTRY_LEN ; i++)
    {
        if ((retVal = reg16_read(unit, l2TableDataReadReg[i], &l2Entry.entry_data[i])) != RT_ERR_OK)
            return retVal;
    }

    _rtl8371c_fdbStSmi2User(pL2Table, &l2Entry);
    return RT_ERR_OK;
}

static rtksw_api_ret_t _rtl8371c_setL2LookupTb(rtksw_uint32 unit, rtl8371c_luttb *pL2Table)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 fieldData;
    rtksw_uint32 i;
    rtl8371c_l2_entry_t l2Entry;
    rtksw_uint32 busyCounter;

    osal_memset(&l2Entry, 0x00, sizeof(rtl8371c_l2_entry_t));
    _rtl8371c_fdbStUser2Smi(pL2Table, &l2Entry);

    if(pL2Table->wait_time == 0)
        busyCounter = RTL8371C_LUT_BUSY_CHECK_NO;
    else
        busyCounter = pL2Table->wait_time;

    /* Check Busy status before write */
    while(busyCounter)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &regData)) != RT_ERR_OK)
            return retVal;

        pL2Table->lookup_busy = regData;
        if(!regData)
            break;

        busyCounter--;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /*Write table data to indirect data register */
    for (i = 0 ; i < RTL8371C_LUT_ENTRY_LEN ; i++)
    {
        if ((retVal = reg16_write(unit, l2TableDataWriteReg[i], &l2Entry.entry_data[i])) != RT_ERR_OK)
            return retVal;
    }

    /* Prepare Write Command */
    if ((retVal = reg16_read(unit, RTL8371C_ACS_CMDr, &regData)) != RT_ERR_OK)
        return retVal;

    /* Set table type */
    fieldData = 4;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_TABLE_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    /*set command type -- 0b1 write*/
    fieldData = 1;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_COMMAND_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    
    /* Issue command */
    fieldData = 1;
    if ((retVal = reg16_field_set(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_write(unit, RTL8371C_ACS_CMDr, &regData)) != RT_ERR_OK)
        return retVal;

    /* Wait until writing finishes */
    if(pL2Table->wait_time == 0)
        busyCounter = RTL8371C_LUT_BUSY_CHECK_NO;
    else
        busyCounter = pL2Table->wait_time;

    while(busyCounter)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_ACS_CMDr, RTL8371C_TLB_EXECUTEf, &regData)) != RT_ERR_OK)
            return retVal;

        pL2Table->lookup_busy = regData;
        if(!regData)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }

    /* Read access status */
    if ((retVal = reg16_field_read(unit, RTL8371C_ACS_L2_ADRr, RTL8371C_HIT_STATUSf, &regData)) != RT_ERR_OK)
        return retVal;

    pL2Table->lookup_hit = regData;
    if(!pL2Table->lookup_hit)
        return RT_ERR_FAILED;

    /*Read access address*/
    if ((retVal = reg16_field_read(unit, RTL8371C_ACS_L2_ADRr, RTL8371C_ADDRESSf, &regData)) != RT_ERR_OK)
        return retVal;

    pL2Table->address = (regData & 0x7ff);

    if ((retVal = reg16_field_read(unit, RTL8371C_ACS_L2_ADRr, RTL8371C_TYPEf, &regData)) != RT_ERR_OK)
        return retVal;
    
    pL2Table->address |= (regData << 11);
    pL2Table->lookup_busy = 0;

    return RT_ERR_OK;
}

static rtksw_api_ret_t _rtl8371c_getLutIPMCGroup(rtksw_uint32 unit, rtksw_uint32 index, ipaddr_t *pGroup_addr, rtksw_uint32 *pPmask, rtksw_uint32 *pValid)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    if(index > RTL8371C_LUT_IPMCGRP_TABLE_MAX)
        return RT_ERR_INPUT;

    if (NULL == pGroup_addr)
        return RT_ERR_NULL_POINTER;

    if (NULL == pPmask)
        return RT_ERR_NULL_POINTER;

    /* Group address */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_IPMC_GROUP_ETYr, REG_ARRAY_INDEX_NONE, index, RTL8371C_IPMC_ENTRY0_27_16f, &regData)) != RT_ERR_OK)
        return retVal;

    *pGroup_addr = (((regData & 0x00000FFF) << 16) | 0xE0000000);

    if ((retVal = reg16_array_field_read(unit, RTL8371C_IPMC_GROUP_ETYr, REG_ARRAY_INDEX_NONE, index, RTL8371C_IPMC_ENTRY0_15_0f, &regData)) != RT_ERR_OK)
        return retVal;

    *pGroup_addr |= (regData & 0x0000FFFF);

    /* portmask */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_IPMC_GROUP_PMSKr, REG_ARRAY_INDEX_NONE, index, RTL8371C_PMASKf, &regData)) != RT_ERR_OK)
        return retVal;

    *pPmask = regData;

    /* valid */
    if ((retVal = reg16_array_field_read(unit, RTL8371C_IPMC_GROUP_VALIDr, REG_ARRAY_INDEX_NONE, index, RTL8371C_VALIDf, &regData)) != RT_ERR_OK)
        return retVal;

    *pValid = regData;

    return RT_ERR_OK;
}

static rtksw_api_ret_t _rtl8371c_setLutIPMCGroup(rtksw_uint32 unit, rtksw_uint32 index, ipaddr_t group_addr, rtksw_uint32 pmask, rtksw_uint32 valid)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    if(index > RTL8371C_LUT_IPMCGRP_TABLE_MAX)
        return RT_ERR_INPUT;

    if ((group_addr & 0xF0000000) != 0xE0000000)    /* not in 224.0.0.0 ~ 239.255.255.255 */
        return RT_ERR_INPUT;

    /* Group Address */
    regData = ((group_addr & 0x0FFFFFFF) >> 16);
    if ((retVal = reg16_array_field_write(unit, RTL8371C_IPMC_GROUP_ETYr, REG_ARRAY_INDEX_NONE, index, RTL8371C_IPMC_ENTRY0_27_16f, &regData)) != RT_ERR_OK)
        return retVal;

    regData = (group_addr & 0x0000FFFF);
    if ((retVal = reg16_array_field_write(unit, RTL8371C_IPMC_GROUP_ETYr, REG_ARRAY_INDEX_NONE, index, RTL8371C_IPMC_ENTRY0_15_0f, &regData)) != RT_ERR_OK)
        return retVal;

    /* portmask */
    if ((retVal = reg16_array_field_write(unit, RTL8371C_IPMC_GROUP_PMSKr, REG_ARRAY_INDEX_NONE, index, RTL8371C_PMASKf, &pmask)) != RT_ERR_OK)
        return retVal;

    /* valid */
    if ((retVal = reg16_array_field_write(unit, RTL8371C_IPMC_GROUP_VALIDr, REG_ARRAY_INDEX_NONE, index, RTL8371C_VALIDf, &valid)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *      Initialize l2 module before calling any l2 APIs.
 */
rtksw_api_ret_t dal_rtl8371c_l2_init(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 fieldData;
    rtksw_uint32 port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = dal_rtl8371c_l2_ipMcastAddrLookup_set(unit, LOOKUP_MAC)) != RT_ERR_OK)
        return retVal;

    /*Enable CAM Usage*/
    fieldData = 0;
    if ((retVal = reg16_field_write(unit, RTL8371C_LUT_CFGr, RTL8371C_BCAM_DISABLEf, &fieldData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = dal_rtl8371c_l2_aging_set(unit, 300)) != RT_ERR_OK)
        return retVal;

    RTKSW_SCAN_ALL_LOG_PORT(unit, port)
    {
        if ((retVal = dal_rtl8371c_l2_limitLearningCnt_set(unit, port, rtksw_switch_maxLutAddrNumber_get(unit))) != RT_ERR_OK)
            return retVal;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_addr_add
 * Description:
 *      Add LUT unicast entry.
 * Input:
 *      unit        - Unit ID
 *      pMac        - 6 bytes unicast(I/G bit is 0) mac address to be written into LUT.
 *      pL2_data    - Unicast entry parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_MAC              - Invalid MAC address.
 *      RT_ERR_L2_FID           - Invalid FID .
 *      RT_ERR_L2_INDEXTBL_FULL - hashed index is full of entries.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      If the unicast mac address already existed in LUT, it will udpate the status of the entry.
 *      Otherwise, it will find an empty or asic auto learned entry to write. If all the entries
 *      with the same hash value can't be replaced, ASIC will return a RT_ERR_L2_INDEXTBL_FULL error.
 */
rtksw_api_ret_t dal_rtl8371c_l2_addr_add(rtksw_uint32 unit, rtksw_mac_t *pMac, rtksw_l2_ucastAddr_t *pL2_data)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 method;
    rtl8371c_luttb l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* must be unicast address */
    if ((pMac == NULL) || (pMac->octet[0] & 0x1))
        return RT_ERR_MAC;

    if(pL2_data == NULL)
        return RT_ERR_MAC;

    RTKSW_CHK_PORT_VALID(unit, pL2_data->port);

    if (pL2_data->ivl >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if (pL2_data->cvid > RTL8371C_VIDMAX)
        return RT_ERR_L2_VID;

    if (pL2_data->fid > RTL8371C_FIDMAX)
        return RT_ERR_L2_FID;

    if (pL2_data->is_static>= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));

    /* fill key (MAC,FID) to get L2 entry */
    osal_memcpy(l2Table.mac.octet, pMac->octet, ETHER_ADDR_LEN);
    l2Table.ivl_svl     = pL2_data->ivl;
    l2Table.cvid_fid    = (pL2_data->ivl ? pL2_data->cvid : pL2_data->fid);
    method = RTL8371C_LUTREADMETHOD_MAC;
    retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table);
    if (RT_ERR_OK == retVal )
    {
        osal_memcpy(l2Table.mac.octet, pMac->octet, ETHER_ADDR_LEN);
        l2Table.ivl_svl     = pL2_data->ivl;
        l2Table.cvid_fid    = (pL2_data->ivl ? pL2_data->cvid : pL2_data->fid);
        l2Table.spa         = rtksw_switch_port_L2P_get(unit, pL2_data->port);
        l2Table.nosalearn   = pL2_data->is_static;
        l2Table.l3lookup    = 0;
        l2Table.age         = 6;
        if((retVal = _rtl8371c_setL2LookupTb(unit, &l2Table)) != RT_ERR_OK)
            return retVal;

        pL2_data->address = l2Table.address;
        return RT_ERR_OK;
    }
    else if (RT_ERR_L2_ENTRY_NOTFOUND == retVal )
    {
        osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));
        osal_memcpy(l2Table.mac.octet, pMac->octet, ETHER_ADDR_LEN);
        l2Table.ivl_svl     = pL2_data->ivl;
        l2Table.cvid_fid    = (pL2_data->ivl ? pL2_data->cvid : pL2_data->fid);
        l2Table.spa         = rtksw_switch_port_L2P_get(unit, pL2_data->port);
        l2Table.nosalearn   = pL2_data->is_static;
        l2Table.l3lookup    = 0;
        l2Table.age         = 6;

        if ((retVal = _rtl8371c_setL2LookupTb(unit, &l2Table)) != RT_ERR_OK)
            return retVal;

        pL2_data->address = l2Table.address;

        method = RTL8371C_LUTREADMETHOD_MAC;
        retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table);
        if (RT_ERR_L2_ENTRY_NOTFOUND == retVal )
            return RT_ERR_L2_INDEXTBL_FULL;
        else
            return retVal;
    }
    else
        return retVal;

}

/* Function Name:
 *      dal_rtl8371c_l2_addr_get
 * Description:
 *      Get LUT unicast entry.
 * Input:
 *      unit    - Unit ID
 *      pMac    - 6 bytes unicast(I/G bit is 0) mac address to be written into LUT.
 * Output:
 *      pL2_data - Unicast entry parameter
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
rtksw_api_ret_t dal_rtl8371c_l2_addr_get(rtksw_uint32 unit, rtksw_mac_t *pMac, rtksw_l2_ucastAddr_t *pL2_data)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 method;
    rtl8371c_luttb l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* must be unicast address */
    if ((pMac == NULL) || (pMac->octet[0] & 0x1))
        return RT_ERR_MAC;

    if (pL2_data->ivl >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if (pL2_data->ivl == 1)
    {
        if (pL2_data->cvid > RTL8371C_VIDMAX)
            return RT_ERR_L2_VID;
    }
    else
    {
        if (pL2_data->fid > RTL8371C_FIDMAX)
            return RT_ERR_L2_FID;
    }

    osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));

    osal_memcpy(l2Table.mac.octet, pMac->octet, ETHER_ADDR_LEN);
    l2Table.ivl_svl     = pL2_data->ivl;
    l2Table.cvid_fid    = (pL2_data->ivl ? pL2_data->cvid : pL2_data->fid);
    method = RTL8371C_LUTREADMETHOD_MAC;

    if ((retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table)) != RT_ERR_OK)
        return retVal;

    osal_memcpy(pL2_data->mac.octet, pMac->octet,ETHER_ADDR_LEN);
    pL2_data->port      = rtksw_switch_port_P2L_get(unit, l2Table.spa);
    pL2_data->ivl       = l2Table.ivl_svl;

    if (pL2_data->ivl == 1)
        pL2_data->cvid      = l2Table.cvid_fid;
    else
        pL2_data->fid       = l2Table.cvid_fid;

    pL2_data->is_static = l2Table.nosalearn;
    pL2_data->address   = l2Table.address;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_addr_next_get
 * Description:
 *      Get Next LUT unicast entry.
 * Input:
 *      unit            - Unit ID
 *      read_method     - The reading method.
 *      port            - The port number if the read_metohd is READMETHOD_NEXT_L2UCSPA
 *      pAddress        - The Address ID
 * Output:
 *      pL2_data - Unicast entry parameter
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      Get the next unicast entry after the current entry pointed by pAddress.
 *      The address of next entry is returned by pAddress. User can use (address + 1)
 *      as pAddress to call this API again for dumping all entries is LUT.
 */
rtksw_api_ret_t dal_rtl8371c_l2_addr_next_get(rtksw_uint32 unit, rtksw_l2_read_method_t read_method, rtksw_port_t port, rtksw_uint32 *pAddress, rtksw_l2_ucastAddr_t *pL2_data)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32      method;
    rtl8371c_luttb  l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Error Checking */
    if ((pL2_data == NULL) || (pAddress == NULL))
        return RT_ERR_MAC;

    if(read_method == READMETHOD_NEXT_L2UC)
        method = RTL8371C_LUTREADMETHOD_NEXT_L2UC;
    else if(read_method == READMETHOD_NEXT_L2UCSPA)
        method = RTL8371C_LUTREADMETHOD_NEXT_L2UCSPA;
    else
        return RT_ERR_INPUT;

    if(read_method == READMETHOD_NEXT_L2UCSPA)
    {
        /* Check Port Valid */
        RTKSW_CHK_PORT_VALID(unit, port);
    }

    if(*pAddress > (rtksw_switch_maxLutAddrNumber_get(unit) - 1))
        return RT_ERR_L2_L2UNI_PARAM;

    osal_memset(pL2_data, 0, sizeof(rtksw_l2_ucastAddr_t));
    osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));
    l2Table.address = *pAddress;

    if(read_method == READMETHOD_NEXT_L2UCSPA)
        l2Table.spa = rtksw_switch_port_L2P_get(unit, port);

    if ((retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table)) != RT_ERR_OK)
        return retVal;

    if(l2Table.address < *pAddress)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memcpy(pL2_data->mac.octet, l2Table.mac.octet, ETHER_ADDR_LEN);
    pL2_data->port      = rtksw_switch_port_P2L_get(unit, l2Table.spa);
    pL2_data->ivl       = l2Table.ivl_svl;

    if (pL2_data->ivl == 1)
        pL2_data->cvid      = l2Table.cvid_fid;
    else
        pL2_data->fid       = l2Table.cvid_fid;

    pL2_data->is_static = l2Table.nosalearn;
    pL2_data->address   = l2Table.address;

    *pAddress = l2Table.address;

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8371c_l2_addr_del
 * Description:
 *      Delete LUT unicast entry.
 * Input:
 *      unit    - Unit ID
 *      pMac    - 6 bytes unicast(I/G bit is 0) mac address to be written into LUT.
 *      fid     - Filtering database
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
rtksw_api_ret_t dal_rtl8371c_l2_addr_del(rtksw_uint32 unit, rtksw_mac_t *pMac, rtksw_l2_ucastAddr_t *pL2_data)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 method;
    rtl8371c_luttb l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* must be unicast address */
    if ((pMac == NULL) || (pMac->octet[0] & 0x1))
        return RT_ERR_MAC;

    if (pL2_data->ivl >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if (pL2_data->ivl == 1)
    {
        if (pL2_data->cvid > RTL8371C_VIDMAX)
            return RT_ERR_L2_VID;
    }
    else
    {
        if (pL2_data->fid > RTL8371C_FIDMAX)
            return RT_ERR_L2_FID;
    }

    osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));

    /* fill key (MAC,FID) to get L2 entry */
    osal_memcpy(l2Table.mac.octet, pMac->octet, ETHER_ADDR_LEN);
    l2Table.ivl_svl     = pL2_data->ivl;
    l2Table.cvid_fid    = (pL2_data->ivl ? pL2_data->cvid : pL2_data->fid);
    method = RTL8371C_LUTREADMETHOD_MAC;
    retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table);
    if (RT_ERR_OK ==  retVal)
    {
        osal_memcpy(l2Table.mac.octet, pMac->octet, ETHER_ADDR_LEN);
        l2Table.ivl_svl     = pL2_data->ivl;
        l2Table.cvid_fid    = (pL2_data->ivl ? pL2_data->cvid : pL2_data->fid);
        l2Table.spa         = 0;
        l2Table.nosalearn   = 0;
        l2Table.age         = 0;
        if((retVal = _rtl8371c_setL2LookupTb(unit, &l2Table)) != RT_ERR_OK)
            return retVal;

        pL2_data->address = l2Table.address;
        return RT_ERR_OK;
    }
    else
        return retVal;
}

/* Function Name:
 *      dal_rtl8371c_l2_mcastAddr_add
 * Description:
 *      Add LUT multicast entry.
 * Input:
 *      unit        - Unit ID
 *      pMcastAddr  - L2 multicast entry structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_MAC              - Invalid MAC address.
 *      RT_ERR_L2_FID           - Invalid FID .
 *      RT_ERR_L2_VID           - Invalid VID .
 *      RT_ERR_L2_INDEXTBL_FULL - hashed index is full of entries.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      If the multicast mac address already existed in the LUT, it will udpate the
 *      port mask of the entry. Otherwise, it will find an empty or asic auto learned
 *      entry to write. If all the entries with the same hash value can't be replaced,
 *      ASIC will return a RT_ERR_L2_INDEXTBL_FULL error.
 */
rtksw_api_ret_t dal_rtl8371c_l2_mcastAddr_add(rtksw_uint32 unit, rtksw_l2_mcastAddr_t *pMcastAddr)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32      method;
    rtl8371c_luttb  l2Table;
    rtksw_uint32      pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pMcastAddr)
        return RT_ERR_NULL_POINTER;

    /* must be L2 multicast address */
    if( (pMcastAddr->mac.octet[0] & 0x01) != 0x01)
        return RT_ERR_MAC;

    RTKSW_CHK_PORTMASK_VALID(unit, &pMcastAddr->portmask);

    if(pMcastAddr->ivl == 1)
    {
        if (pMcastAddr->vid > RTL8371C_VIDMAX)
            return RT_ERR_L2_VID;
    }
    else if(pMcastAddr->ivl == 0)
    {
        if (pMcastAddr->fid > RTL8371C_FIDMAX)
            return RT_ERR_L2_FID;
    }
    else
        return RT_ERR_INPUT;

    /* Get physical port mask */
    if ((retVal = rtksw_switch_portmask_L2P_get(unit, &pMcastAddr->portmask, &pmask)) != RT_ERR_OK)
        return retVal;

    osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));

    /* fill key (MAC,FID) to get L2 entry */
    osal_memcpy(l2Table.mac.octet, pMcastAddr->mac.octet, ETHER_ADDR_LEN);
    l2Table.ivl_svl     = pMcastAddr->ivl;

    if(pMcastAddr->ivl)
        l2Table.cvid_fid    = pMcastAddr->vid;
    else
        l2Table.cvid_fid    = pMcastAddr->fid;

    method = RTL8371C_LUTREADMETHOD_MAC;
    retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table);
    if (RT_ERR_OK == retVal)
    {
        osal_memcpy(l2Table.mac.octet, pMcastAddr->mac.octet, ETHER_ADDR_LEN);
        l2Table.ivl_svl     = pMcastAddr->ivl;

        if(pMcastAddr->ivl)
            l2Table.cvid_fid    = pMcastAddr->vid;
        else
            l2Table.cvid_fid    = pMcastAddr->fid;

        l2Table.mbr         = pmask;
        l2Table.nosalearn   = 1;
        l2Table.l3lookup    = 0;
        if((retVal = _rtl8371c_setL2LookupTb(unit, &l2Table)) != RT_ERR_OK)
            return retVal;

        pMcastAddr->address = l2Table.address;
        return RT_ERR_OK;
    }
    else if (RT_ERR_L2_ENTRY_NOTFOUND == retVal)
    {
        osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));
        osal_memcpy(l2Table.mac.octet, pMcastAddr->mac.octet, ETHER_ADDR_LEN);
        l2Table.ivl_svl     = pMcastAddr->ivl;
        if(pMcastAddr->ivl)
            l2Table.cvid_fid    = pMcastAddr->vid;
        else
            l2Table.cvid_fid    = pMcastAddr->fid;

        l2Table.mbr         = pmask;
        l2Table.nosalearn   = 1;
        l2Table.l3lookup    = 0;
        if ((retVal = _rtl8371c_setL2LookupTb(unit, &l2Table)) != RT_ERR_OK)
            return retVal;

        pMcastAddr->address = l2Table.address;

        method = RTL8371C_LUTREADMETHOD_MAC;
        retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table);
        if (RT_ERR_L2_ENTRY_NOTFOUND == retVal)
            return     RT_ERR_L2_INDEXTBL_FULL;
        else
            return retVal;
    }
    else
        return retVal;

}

/* Function Name:
 *      dal_rtl8371c_l2_mcastAddr_get
 * Description:
 *      Get LUT multicast entry.
 * Input:
 *      unit        - Unit ID
 *      pMcastAddr  - L2 multicast entry structure
 * Output:
 *      pMcastAddr  - L2 multicast entry structure
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_VID               - Invalid VID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      If the multicast mac address existed in the LUT, it will return the port where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
rtksw_api_ret_t dal_rtl8371c_l2_mcastAddr_get(rtksw_uint32 unit, rtksw_l2_mcastAddr_t *pMcastAddr)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 method;
    rtl8371c_luttb l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pMcastAddr)
        return RT_ERR_NULL_POINTER;

    /* must be L2 multicast address */
    if( (pMcastAddr->mac.octet[0] & 0x01) != 0x01)
        return RT_ERR_MAC;

    if(pMcastAddr->ivl == 1)
    {
        if (pMcastAddr->vid > RTL8371C_VIDMAX)
            return RT_ERR_L2_VID;
    }
    else if(pMcastAddr->ivl == 0)
    {
        if (pMcastAddr->fid > RTL8371C_FIDMAX)
            return RT_ERR_L2_FID;
    }
    else
        return RT_ERR_INPUT;

    osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));
    osal_memcpy(l2Table.mac.octet, pMcastAddr->mac.octet, ETHER_ADDR_LEN);
    l2Table.ivl_svl     = pMcastAddr->ivl;

    if(pMcastAddr->ivl)
        l2Table.cvid_fid    = pMcastAddr->vid;
    else
        l2Table.cvid_fid    = pMcastAddr->fid;

    method = RTL8371C_LUTREADMETHOD_MAC;

    if ((retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table)) != RT_ERR_OK)
        return retVal;

    pMcastAddr->address     = l2Table.address;

    /* Get Logical port mask */
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, l2Table.mbr, &pMcastAddr->portmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_mcastAddr_next_get
 * Description:
 *      Get Next L2 Multicast entry.
 * Input:
 *      unit            - Unit ID
 *      pAddress        - The Address ID
 * Output:
 *      pMcastAddr  - L2 multicast entry structure
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      Get the next L2 multicast entry after the current entry pointed by pAddress.
 *      The address of next entry is returned by pAddress. User can use (address + 1)
 *      as pAddress to call this API again for dumping all multicast entries is LUT.
 */
rtksw_api_ret_t dal_rtl8371c_l2_mcastAddr_next_get(rtksw_uint32 unit, rtksw_uint32 *pAddress, rtksw_l2_mcastAddr_t *pMcastAddr)
{
    rtksw_api_ret_t   retVal;
    rtl8371c_luttb  l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Error Checking */
    if ((pAddress == NULL) || (pMcastAddr == NULL))
        return RT_ERR_INPUT;

    if(*pAddress > (rtksw_switch_maxLutAddrNumber_get(unit) - 1) )
        return RT_ERR_L2_L2UNI_PARAM;

    osal_memset(pMcastAddr, 0, sizeof(rtksw_l2_mcastAddr_t));
    osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));
    l2Table.address = *pAddress;

    if ((retVal = _rtl8371c_getL2LookupTb(unit, RTL8371C_LUTREADMETHOD_NEXT_L2MC, &l2Table)) != RT_ERR_OK)
        return retVal;

    if(l2Table.address < *pAddress)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    osal_memcpy(pMcastAddr->mac.octet, l2Table.mac.octet, ETHER_ADDR_LEN);
    pMcastAddr->ivl     = l2Table.ivl_svl;

    if(pMcastAddr->ivl)
        pMcastAddr->vid = l2Table.cvid_fid;
    else
        pMcastAddr->fid = l2Table.cvid_fid;

    pMcastAddr->address     = l2Table.address;

    /* Get Logical port mask */
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, l2Table.mbr, &pMcastAddr->portmask)) != RT_ERR_OK)
        return retVal;

    *pAddress = l2Table.address;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_mcastAddr_del
 * Description:
 *      Delete LUT multicast entry.
 * Input:
 *      unit        - Unit ID
 *      pMcastAddr  - L2 multicast entry structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_VID               - Invalid VID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
rtksw_api_ret_t dal_rtl8371c_l2_mcastAddr_del(rtksw_uint32 unit, rtksw_l2_mcastAddr_t *pMcastAddr)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 method;
    rtl8371c_luttb l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pMcastAddr)
        return RT_ERR_NULL_POINTER;

    /* must be L2 multicast address */
    if( (pMcastAddr->mac.octet[0] & 0x01) != 0x01)
        return RT_ERR_MAC;

    if(pMcastAddr->ivl == 1)
    {
        if (pMcastAddr->vid > RTL8371C_VIDMAX)
            return RT_ERR_L2_VID;
    }
    else if(pMcastAddr->ivl == 0)
    {
        if (pMcastAddr->fid > RTL8371C_FIDMAX)
            return RT_ERR_L2_FID;
    }
    else
        return RT_ERR_INPUT;

    osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));

    /* fill key (MAC,FID) to get L2 entry */
    osal_memcpy(l2Table.mac.octet, pMcastAddr->mac.octet, ETHER_ADDR_LEN);
    l2Table.ivl_svl     = pMcastAddr->ivl;

    if(pMcastAddr->ivl)
        l2Table.cvid_fid    = pMcastAddr->vid;
    else
        l2Table.cvid_fid    = pMcastAddr->fid;

    method = RTL8371C_LUTREADMETHOD_MAC;
    retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table);
    if (RT_ERR_OK == retVal)
    {
        osal_memcpy(l2Table.mac.octet, pMcastAddr->mac.octet, ETHER_ADDR_LEN);
        l2Table.ivl_svl     = pMcastAddr->ivl;

        if(pMcastAddr->ivl)
            l2Table.cvid_fid    = pMcastAddr->vid;
        else
            l2Table.cvid_fid    = pMcastAddr->fid;

        l2Table.mbr         = 0;
        l2Table.nosalearn   = 0;
        l2Table.l3lookup    = 0;
        if((retVal = _rtl8371c_setL2LookupTb(unit, &l2Table)) != RT_ERR_OK)
            return retVal;

        pMcastAddr->address = l2Table.address;
        return RT_ERR_OK;
    }
    else
        return retVal;
}

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddr_add
 * Description:
 *      Add Lut IP multicast entry
 * Input:
 *      unit            - Unit ID
 *      pIpMcastAddr    - IP Multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_L2_INDEXTBL_FULL - hashed index is full of entries.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      System supports L2 entry with IP multicast DIP/SIP to forward IP multicasting frame as user
 *      desired. If this function is enabled, then system will be looked up L2 IP multicast entry to
 *      forward IP multicast frame directly without flooding.
 */
rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddr_add(rtksw_uint32 unit, rtksw_l2_ipMcastAddr_t *pIpMcastAddr)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 method;
    rtl8371c_luttb l2Table;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pIpMcastAddr)
        return RT_ERR_NULL_POINTER;

    /* check port mask */
    RTKSW_CHK_PORTMASK_VALID(unit, &pIpMcastAddr->portmask);

    if( (pIpMcastAddr->dip & 0xF0000000) != 0xE0000000)
        return RT_ERR_INPUT;

    /* Get Physical port mask */
    if ((retVal = rtksw_switch_portmask_L2P_get(unit, &pIpMcastAddr->portmask, &pmask)) != RT_ERR_OK)
        return retVal;

    osal_memset(&l2Table, 0x00, sizeof(rtl8371c_luttb));
    l2Table.sip = pIpMcastAddr->sip;
    l2Table.dip = pIpMcastAddr->dip;
    l2Table.l3lookup = 1;
    method = RTL8371C_LUTREADMETHOD_MAC;
    retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table);
    if (RT_ERR_OK == retVal)
    {
        l2Table.sip = pIpMcastAddr->sip;
        l2Table.dip = pIpMcastAddr->dip;
        l2Table.mbr = pmask;
        l2Table.nosalearn = 1;
        l2Table.l3lookup = 1;
        if((retVal = _rtl8371c_setL2LookupTb(unit, &l2Table)) != RT_ERR_OK)
            return retVal;

        pIpMcastAddr->address = l2Table.address;
        return RT_ERR_OK;
    }
    else if (RT_ERR_L2_ENTRY_NOTFOUND == retVal)
    {
        osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));
        l2Table.sip = pIpMcastAddr->sip;
        l2Table.dip = pIpMcastAddr->dip;
        l2Table.mbr = pmask;
        l2Table.nosalearn = 1;
        l2Table.l3lookup = 1;
        if ((retVal = _rtl8371c_setL2LookupTb(unit, &l2Table)) != RT_ERR_OK)
            return retVal;

        pIpMcastAddr->address = l2Table.address;

        method = RTL8371C_LUTREADMETHOD_MAC;
        retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table);
        if (RT_ERR_L2_ENTRY_NOTFOUND == retVal)
            return     RT_ERR_L2_INDEXTBL_FULL;
        else
            return retVal;

    }
    else
        return retVal;

}

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddr_get
 * Description:
 *      Get LUT IP multicast entry.
 * Input:
 *      unit            - Unit ID
 *      pIpMcastAddr    - IP Multicast entry
 * Output:
 *      pIpMcastAddr    - IP Multicast entry
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      The API can get Lut table of IP multicast entry.
 */
rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddr_get(rtksw_uint32 unit, rtksw_l2_ipMcastAddr_t *pIpMcastAddr)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 method;
    rtl8371c_luttb l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pIpMcastAddr)
        return RT_ERR_NULL_POINTER;

    if( (pIpMcastAddr->dip & 0xF0000000) != 0xE0000000)
        return RT_ERR_INPUT;

    osal_memset(&l2Table, 0x00, sizeof(rtl8371c_luttb));
    l2Table.sip = pIpMcastAddr->sip;
    l2Table.dip = pIpMcastAddr->dip;
    l2Table.l3lookup = 1;
    method = RTL8371C_LUTREADMETHOD_MAC;
    if ((retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table)) != RT_ERR_OK)
        return retVal;

    /* Get Logical port mask */
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, l2Table.mbr, &pIpMcastAddr->portmask)) != RT_ERR_OK)
        return retVal;

    pIpMcastAddr->address       = l2Table.address;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddr_next_get
 * Description:
 *      Get Next IP Multicast entry.
 * Input:
 *      unit            - Unit ID
 *      pAddress        - The Address ID
 * Output:
 *      pIpMcastAddr    - IP Multicast entry
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      Get the next IP multicast entry after the current entry pointed by pAddress.
 *      The address of next entry is returned by pAddress. User can use (address + 1)
 *      as pAddress to call this API again for dumping all IP multicast entries is LUT.
 */
rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddr_next_get(rtksw_uint32 unit, rtksw_uint32 *pAddress, rtksw_l2_ipMcastAddr_t *pIpMcastAddr)
{
    rtksw_api_ret_t   retVal;
    rtl8371c_luttb  l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Error Checking */
    if ((pAddress == NULL) || (pIpMcastAddr == NULL) )
        return RT_ERR_INPUT;

    if(*pAddress > (rtksw_switch_maxLutAddrNumber_get(unit) - 1) )
        return RT_ERR_L2_L2UNI_PARAM;

    osal_memset(pIpMcastAddr, 0, sizeof(rtksw_l2_ipMcastAddr_t));
    osal_memset(&l2Table, 0, sizeof(rtl8371c_luttb));
    l2Table.address = *pAddress;

    if ((retVal = _rtl8371c_getL2LookupTb(unit, RTL8371C_LUTREADMETHOD_NEXT_L3MC, &l2Table)) != RT_ERR_OK)
        return retVal;

    if(l2Table.address < *pAddress)
        return RT_ERR_L2_ENTRY_NOTFOUND;

    pIpMcastAddr->sip = l2Table.sip;
    pIpMcastAddr->dip = l2Table.dip;

    /* Get Logical port mask */
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, l2Table.mbr, &pIpMcastAddr->portmask)) != RT_ERR_OK)
        return retVal;

    pIpMcastAddr->address       = l2Table.address;
    *pAddress = l2Table.address;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddr_del
 * Description:
 *      Delete a ip multicast address entry from the specified device.
 * Input:
 *      unit            - Unit ID
 *      pIpMcastAddr    - IP Multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      The API can delete a IP multicast address entry from the specified device.
 */
rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddr_del(rtksw_uint32 unit, rtksw_l2_ipMcastAddr_t *pIpMcastAddr)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 method;
    rtl8371c_luttb l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Error Checking */
    if (pIpMcastAddr == NULL)
        return RT_ERR_INPUT;

    if( (pIpMcastAddr->dip & 0xF0000000) != 0xE0000000)
        return RT_ERR_INPUT;

    osal_memset(&l2Table, 0x00, sizeof(rtl8371c_luttb));
    l2Table.sip = pIpMcastAddr->sip;
    l2Table.dip = pIpMcastAddr->dip;
    l2Table.l3lookup = 1;
    method = RTL8371C_LUTREADMETHOD_MAC;
    retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table);
    if (RT_ERR_OK == retVal)
    {
        l2Table.sip = pIpMcastAddr->sip;
        l2Table.dip = pIpMcastAddr->dip;
        l2Table.mbr = 0;
        l2Table.nosalearn = 0;
        l2Table.l3lookup = 1;
        if((retVal = _rtl8371c_setL2LookupTb(unit, &l2Table)) != RT_ERR_OK)
            return retVal;

        pIpMcastAddr->address = l2Table.address;
        return RT_ERR_OK;
    }
    else
        return retVal;
}

/* Function Name:
 *      dal_rtl8371c_l2_ucastAddr_flush
 * Description:
 *      Flush L2 mac address by type in the specified device (both dynamic and static).
 * Input:
 *      unit    - Unit ID
 *      pConfig - flush configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      flushByVid          - 1: Flush by VID, 0: Don't flush by VID
 *      vid                 - VID (0 ~ 4095)
 *      flushByFid          - 1: Flush by FID, 0: Don't flush by FID
 *      fid                 - FID (0 ~ 15)
 *      flushByPort         - 1: Flush by Port, 0: Don't flush by Port
 *      port                - Port ID
 *      flushByMac          - Not Supported
 *      ucastAddr           - Not Supported
 *      flushStaticAddr     - 1: Flush both Static and Dynamic entries, 0: Flush only Dynamic entries
 *      flushAddrOnAllPorts - 1: Flush VID-matched entries at all ports, 0: Flush VID-matched entries per port.
 */
rtksw_api_ret_t dal_rtl8371c_l2_ucastAddr_flush(rtksw_uint32 unit, rtksw_l2_flushCfg_t *pConfig)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32 phyPort;
    rtksw_uint32 regData;
    rtksw_uint32 fieldData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(pConfig == NULL)
        return RT_ERR_NULL_POINTER;

    if(pConfig->flushByVid >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if(pConfig->flushByFid >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if(pConfig->flushByPort >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if(pConfig->flushByMac >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if(pConfig->flushStaticAddr >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if(pConfig->flushAddrOnAllPorts >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if(pConfig->vid > RTL8371C_VIDMAX)
        return RT_ERR_VLAN_VID;

    if(pConfig->fid > RTL8371C_FIDMAX)
        return RT_ERR_INPUT;

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, pConfig->port);

    phyPort = rtksw_switch_port_L2P_get(unit, pConfig->port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if(pConfig->flushByVid == RTKSW_ENABLED)
    {
        /* Flush Mode & Type */
        if ((retVal = reg16_read(unit, RTL8371C_L2_FLUSH_MODEr, &regData)) != RT_ERR_OK)
            return retVal;

        fieldData = RTL8371C_FLUSHMDOE_VID;
        if ((retVal = reg16_field_set(unit, RTL8371C_L2_FLUSH_MODEr, RTL8371C_LUT_FLUSH_MODEf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        fieldData = (pConfig->flushStaticAddr == RTKSW_ENABLED) ? RTL8371C_FLUSHTYPE_BOTH : RTL8371C_FLUSHTYPE_DYNAMIC;
        if ((retVal = reg16_field_set(unit, RTL8371C_L2_FLUSH_MODEr, RTL8371C_LUT_FLUSH_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_write(unit, RTL8371C_L2_FLUSH_MODEr, &regData)) != RT_ERR_OK)
            return retVal;

        /* Flush VID */
        fieldData = pConfig->vid;
        if ((retVal = reg16_field_write(unit, RTL8371C_L2_FLUSH_XIDr, RTL8371C_LUT_FLUSH_VIDf, &fieldData)) != RT_ERR_OK)
            return retVal;

        /* Flush Portmask */
        if(pConfig->flushAddrOnAllPorts == RTKSW_ENABLED)
            fieldData = RTKSW_PHY_PORTMASK_ALL;
        else if(pConfig->flushByPort == RTKSW_ENABLED)
            fieldData = (1 << phyPort) & 0xff;
        else
            return RT_ERR_INPUT;

        if ((retVal = reg16_field_write(unit, RTL8371C_L2_FLUSH_CMDr, RTL8371C_PORTMASKf, &fieldData)) != RT_ERR_OK)
            return retVal;
    }
    else if(pConfig->flushByFid == RTKSW_ENABLED)
    {
        /* Flush Mode & Type */
        if ((retVal = reg16_read(unit, RTL8371C_L2_FLUSH_MODEr, &regData)) != RT_ERR_OK)
            return retVal;

        fieldData = RTL8371C_FLUSHMDOE_FID;
        if ((retVal = reg16_field_set(unit, RTL8371C_L2_FLUSH_MODEr, RTL8371C_LUT_FLUSH_MODEf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        fieldData = (pConfig->flushStaticAddr == RTKSW_ENABLED) ? RTL8371C_FLUSHTYPE_BOTH : RTL8371C_FLUSHTYPE_DYNAMIC;
        if ((retVal = reg16_field_set(unit, RTL8371C_L2_FLUSH_MODEr, RTL8371C_LUT_FLUSH_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_write(unit, RTL8371C_L2_FLUSH_MODEr, &regData)) != RT_ERR_OK)
            return retVal;

        /* Flush FID */
        fieldData = pConfig->fid;
        if ((retVal = reg16_field_write(unit, RTL8371C_L2_FLUSH_XIDr, RTL8371C_LUT_FLUSH_FIDf, &fieldData)) != RT_ERR_OK)
            return retVal;

        /* Flush Portmask */
        if(pConfig->flushAddrOnAllPorts == RTKSW_ENABLED)
            fieldData = RTKSW_PHY_PORTMASK_ALL;
        else if(pConfig->flushByPort == RTKSW_ENABLED)
            fieldData = (1 << phyPort) & 0xff;
        else
            return RT_ERR_INPUT;

        if ((retVal = reg16_field_write(unit, RTL8371C_L2_FLUSH_CMDr, RTL8371C_PORTMASKf, &fieldData)) != RT_ERR_OK)
            return retVal;
    }
    else if(pConfig->flushByPort == RTKSW_ENABLED)
    {
        /* Flush Mode & Type */
        if ((retVal = reg16_read(unit, RTL8371C_L2_FLUSH_MODEr, &regData)) != RT_ERR_OK)
            return retVal;

        fieldData = RTL8371C_FLUSHMDOE_PORT;
        if ((retVal = reg16_field_set(unit, RTL8371C_L2_FLUSH_MODEr, RTL8371C_LUT_FLUSH_MODEf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        fieldData = (pConfig->flushStaticAddr == RTKSW_ENABLED) ? RTL8371C_FLUSHTYPE_BOTH : RTL8371C_FLUSHTYPE_DYNAMIC;
        if ((retVal = reg16_field_set(unit, RTL8371C_L2_FLUSH_MODEr, RTL8371C_LUT_FLUSH_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
            return retVal;

        if ((retVal = reg16_write(unit, RTL8371C_L2_FLUSH_MODEr, &regData)) != RT_ERR_OK)
            return retVal;

        /* Flush Portmask */
        fieldData = (1 << phyPort) & 0xff;
        if ((retVal = reg16_field_write(unit, RTL8371C_L2_FLUSH_CMDr, RTL8371C_PORTMASKf, &fieldData)) != RT_ERR_OK)
            return retVal;
    }
    else if(pConfig->flushByMac == RTKSW_ENABLED)
    {
        /* Should use API "rtksw_l2_addr_del" to remove a specified entry*/
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    else
        return RT_ERR_INPUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_table_clear
 * Description:
 *      Flush all static & dynamic entries in LUT.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_l2_table_clear(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 fieldData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    fieldData = 1;
    if ((retVal = reg16_field_write(unit, RTL8371C_L2_FLUSH_ALLr, RTL8371C_FLUSH_ALLf, &fieldData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_table_clearStatus_get
 * Description:
 *      Get table clear status
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pStatus - Clear status, 1:Busy, 0:finish
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_l2_table_clearStatus_get(rtksw_uint32 unit, rtksw_l2_clearStatus_t *pStatus)
{
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pStatus)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_L2_FLUSH_ALLr, RTL8371C_FLUSH_ALLf, pStatus)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      enable  - link down flush status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The status of flush linkdown port address is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t dal_rtl8371c_l2_flushLinkDownPortAddrEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 fieldData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (port != RTKSW_WHOLE_SYSTEM)
        return RT_ERR_PORT_ID;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    fieldData = (enable == RTKSW_ENABLED) ? 0 : 1;
    if ((retVal = reg16_field_write(unit, RTL8371C_LUT_CFGr, RTL8371C_LINKDOWN_AGEOUTf, &fieldData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable - link down flush status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The status of flush linkdown port address is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t dal_rtl8371c_l2_flushLinkDownPortAddrEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 fieldData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (port != RTKSW_WHOLE_SYSTEM)
        return RT_ERR_PORT_ID;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_LUT_CFGr, RTL8371C_LINKDOWN_AGEOUTf, &fieldData)) != RT_ERR_OK)
        return retVal;

    *pEnable = (fieldData == 1) ? RTKSW_DISABLED : RTKSW_ENABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_agingEnable_set
 * Description:
 *      Set L2 LUT aging status per port setting.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      enable  - Aging status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can be used to set L2 LUT aging status per port.
 */
rtksw_api_ret_t dal_rtl8371c_l2_agingEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 fieldData;
    rtksw_uint32 physicalPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if ((retVal = reg16_field_read(unit, RTL8371C_DIS_AGEr, RTL8371C_PORTMASKf, &fieldData)) != RT_ERR_OK)
        return retVal;

    physicalPort = rtksw_switch_port_L2P_get(unit, port);
    if (enable == RTKSW_ENABLED)
        fieldData &= ~(0x0001 << physicalPort);
    else
        fieldData |= (0x0001 << physicalPort);

    if ((retVal = reg16_field_write(unit, RTL8371C_DIS_AGEr, RTL8371C_PORTMASKf, &fieldData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_agingEnable_get
 * Description:
 *      Get L2 LUT aging status per port setting.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable - Aging status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can be used to get L2 LUT aging function per port.
 */
rtksw_api_ret_t dal_rtl8371c_l2_agingEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 fieldData;
    rtksw_uint32 physicalPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_DIS_AGEr, RTL8371C_PORTMASKf, &fieldData)) != RT_ERR_OK)
        return retVal;

    physicalPort = rtksw_switch_port_L2P_get(unit, port);
    *pEnable = (fieldData & (0x0001 << physicalPort)) ? RTKSW_DISABLED : RTKSW_ENABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitLearningCnt_set
 * Description:
 *      Set per-Port auto learning limit number
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      mac_cnt - Auto learning entries limit number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_LIMITED_L2ENTRY_NUM  - Invalid auto learning limit number
 * Note:
 *      The API can set per-port ASIC auto learning limit number from 0(disable learning)
 *      to 2112.
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitLearningCnt_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_cnt_t mac_cnt)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (mac_cnt > rtksw_switch_maxLutAddrNumber_get(unit))
        return RT_ERR_LIMITED_L2ENTRY_NUM;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_L2_LRN_PORT_LMTr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_LIMIT_NUMBERf, (rtksw_uint32 *)&mac_cnt)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitLearningCnt_get
 * Description:
 *      Get per-Port auto learning limit number
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pMac_cnt - Auto learning entries limit number
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get per-port ASIC auto learning limit number.
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitLearningCnt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_cnt_t *pMac_cnt)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pMac_cnt)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_L2_LRN_PORT_LMTr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_LIMIT_NUMBERf, &regData)) != RT_ERR_OK)
        return retVal;

    *pMac_cnt = (rtksw_mac_cnt_t)((regData > rtksw_switch_maxLutAddrNumber_get(unit)) ? rtksw_switch_maxLutAddrNumber_get(unit) : regData);
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCnt_set
 * Description:
 *      Set System auto learning limit number
 * Input:
 *      unit    - Unit ID
 *      mac_cnt - Auto learning entries limit number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_LIMITED_L2ENTRY_NUM  - Invalid auto learning limit number
 * Note:
 *      The API can set system ASIC auto learning limit number from 0(disable learning)
 *      to 2112.
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCnt_set(rtksw_uint32 unit, rtksw_mac_cnt_t mac_cnt)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (mac_cnt > rtksw_switch_maxLutAddrNumber_get(unit))
        return RT_ERR_LIMITED_L2ENTRY_NUM;

    if ((retVal = reg16_field_write(unit, RTL8371C_LRN_SYS_LMTr, RTL8371C_LIMIT_NUMBERf, (rtksw_uint32 *)&mac_cnt)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCnt_get
 * Description:
 *      Get System auto learning limit number
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pMac_cnt - Auto learning entries limit number
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get system ASIC auto learning limit number.
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCnt_get(rtksw_uint32 unit, rtksw_mac_cnt_t *pMac_cnt)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pMac_cnt)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_LRN_SYS_LMTr, RTL8371C_LIMIT_NUMBERf, (rtksw_uint32 *)pMac_cnt)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitLearningCntAction_set
 * Description:
 *      Configure auto learn over limit number action.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      action  - Auto learning entries limit number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_NOT_ALLOWED  - Invalid learn over action
 * Note:
 *      The API can set SA unknown packet action while auto learn limit number is over
 *      The action symbol as following:
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_DROP,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU,
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitLearningCntAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_l2_limitLearnCntAction_t action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (port != RTKSW_WHOLE_SYSTEM)
        return RT_ERR_PORT_ID;

    if ( RTKSW_LIMIT_LEARN_CNT_ACTION_DROP == action )
        regData = 1;
    else if ( RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD == action )
        regData = 0;
    else if ( RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU == action )
        regData = 2;
    else
        return RT_ERR_NOT_ALLOWED;

    if ((retVal = reg16_field_write(unit, RTL8371C_L2_LRN_PORT_CONSTRT_ACTr, RTL8371C_LRN_ACTf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitLearningCntAction_get
 * Description:
 *      Get auto learn over limit number action.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pAction - Learn over action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get SA unknown packet action while auto learn limit number is over
 *      The action symbol as following:
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_DROP,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU,
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitLearningCntAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_l2_limitLearnCntAction_t *pAction)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (port != RTKSW_WHOLE_SYSTEM)
        return RT_ERR_PORT_ID;

    if(NULL == pAction)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_L2_LRN_PORT_CONSTRT_ACTr, RTL8371C_LRN_ACTf, &regData)) != RT_ERR_OK)
        return retVal;

    if ( 1 == regData )
        *pAction = RTKSW_LIMIT_LEARN_CNT_ACTION_DROP;
    else if ( 0 == regData )
        *pAction = RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD;
    else if ( 2 == regData )
        *pAction = RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU;
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCntAction_set
 * Description:
 *      Configure system auto learn over limit number action.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      action  - Auto learning entries limit number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_NOT_ALLOWED  - Invalid learn over action
 * Note:
 *      The API can set SA unknown packet action while auto learn limit number is over
 *      The action symbol as following:
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_DROP,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU,
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCntAction_set(rtksw_uint32 unit, rtksw_l2_limitLearnCntAction_t action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ( RTKSW_LIMIT_LEARN_CNT_ACTION_DROP == action )
        regData = 1;
    else if ( RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD == action )
        regData = 0;
    else if ( RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU == action )
        regData = 2;
    else
        return RT_ERR_NOT_ALLOWED;

    if ((retVal = reg16_field_write(unit, RTL8371C_LRN_SYS_LMT_CTRLr, RTL8371C_LUT_SYSTEM_LEARN_OVER_ACTf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCntAction_get
 * Description:
 *      Get system auto learn over limit number action.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pAction - Learn over action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get SA unknown packet action while auto learn limit number is over
 *      The action symbol as following:
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_DROP,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU,
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCntAction_get(rtksw_uint32 unit, rtksw_l2_limitLearnCntAction_t *pAction)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pAction)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_LRN_SYS_LMT_CTRLr, RTL8371C_LUT_SYSTEM_LEARN_OVER_ACTf, &regData)) != RT_ERR_OK)
        return retVal;

    if ( 1 == regData )
        *pAction = RTKSW_LIMIT_LEARN_CNT_ACTION_DROP;
    else if ( 0 == regData )
        *pAction = RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD;
    else if ( 2 == regData )
        *pAction = RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU;
    else
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCntPortMask_set
 * Description:
 *      Configure system auto learn portmask
 * Input:
 *      unit        - Unit ID
 *      pPortmask   - Port Mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK    - Invalid port mask.
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCntPortMask_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    /* Check port mask */
    RTKSW_CHK_PORTMASK_VALID(unit, pPortmask);

    if ((retVal = rtksw_switch_portmask_L2P_get(unit, pPortmask, &pmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_LRN_SYS_LMT_CTRLr, RTL8371C_LUT_SYSTEM_LEARN_PMASKf, &pmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCntPortMask_get
 * Description:
 *      get system auto learn portmask
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pPortmask - Port Mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - Null pointer.
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCntPortMask_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_LRN_SYS_LMT_CTRLr, RTL8371C_LUT_SYSTEM_LEARN_PMASKf, &pmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, pmask, pPortmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_learningCnt_get
 * Description:
 *      Get per-Port current auto learning number
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pMac_cnt - ASIC auto learning entries number
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get per-port ASIC auto learning number
 */
rtksw_api_ret_t dal_rtl8371c_l2_learningCnt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_cnt_t *pMac_cnt)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pMac_cnt)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_L2_LRN_PORT_CONSTRT_CNTr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_LRN_CNTf, (rtksw_uint32 *)pMac_cnt)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_floodPortMask_set
 * Description:
 *      Set flooding portmask
 * Input:
 *      unit            - Unit ID
 *      type            - flooding type.
 *      pFlood_portmask - flooding porkmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can set the flooding mask.
 *      The flooding type is as following:
 *      - FLOOD_UNKNOWNDA
 *      - FLOOD_UNKNOWNMC
 *      - FLOOD_BC
 */
rtksw_api_ret_t dal_rtl8371c_l2_floodPortMask_set(rtksw_uint32 unit, rtksw_l2_flood_type_t floood_type, rtksw_portmask_t *pFlood_portmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (floood_type >= FLOOD_END)
        return RT_ERR_INPUT;

    /* check port valid */
    RTKSW_CHK_PORTMASK_VALID(unit, pFlood_portmask);

    /* Get Physical port mask */
    if ((retVal = rtksw_switch_portmask_L2P_get(unit, pFlood_portmask, &pmask))!=RT_ERR_OK)
        return retVal;

    switch (floood_type)
    {
        case FLOOD_UNKNOWNDA:
            if ((retVal = reg16_field_write(unit, RTL8371C_UNKN_UC_FLD_PMSKr, RTL8371C_PORT_MASKf, &pmask)) != RT_ERR_OK)
                return retVal;
            break;
        case FLOOD_UNKNOWNMC:
            if ((retVal = reg16_field_write(unit, RTL8371C_UNKN_MC_FLD_PMSKr, RTL8371C_PORT_MASKf, &pmask)) != RT_ERR_OK)
                return retVal;
            break;
        case FLOOD_BC:
            if ((retVal = reg16_field_write(unit, RTL8371C_UNKN_BC_FLD_PMSKr, RTL8371C_PORT_MASKf, &pmask)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_floodPortMask_get
 * Description:
 *      Get flooding portmask
 * Input:
 *      unit    - Unit ID
 *      type    - flooding type.
 * Output:
 *      pFlood_portmask - flooding porkmask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get the flooding mask.
 *      The flooding type is as following:
 *      - FLOOD_UNKNOWNDA
 *      - FLOOD_UNKNOWNMC
 *      - FLOOD_BC
 */
rtksw_api_ret_t dal_rtl8371c_l2_floodPortMask_get(rtksw_uint32 unit, rtksw_l2_flood_type_t floood_type, rtksw_portmask_t *pFlood_portmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (floood_type >= FLOOD_END)
        return RT_ERR_INPUT;

    if(NULL == pFlood_portmask)
        return RT_ERR_NULL_POINTER;

    switch (floood_type)
    {
        case FLOOD_UNKNOWNDA:
            if ((retVal = reg16_field_read(unit, RTL8371C_UNKN_UC_FLD_PMSKr, RTL8371C_PORT_MASKf, &pmask)) != RT_ERR_OK)
                return retVal;
            break;
        case FLOOD_UNKNOWNMC:
            if ((retVal = reg16_field_read(unit, RTL8371C_UNKN_MC_FLD_PMSKr, RTL8371C_PORT_MASKf, &pmask)) != RT_ERR_OK)
                return retVal;
            break;
        case FLOOD_BC:
            if ((retVal = reg16_field_read(unit, RTL8371C_UNKN_BC_FLD_PMSKr, RTL8371C_PORT_MASKf, &pmask)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            break;
    }

    /* Get Logical port mask */
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, pmask, pFlood_portmask))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_localPktPermit_set
 * Description:
 *      Set permittion of frames if source port and destination port are the same.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      permit  - permittion status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid permit value.
 * Note:
 *      This API is setted to permit frame if its source port is equal to destination port.
 */
rtksw_api_ret_t dal_rtl8371c_l2_localPktPermit_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t permit)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (permit >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_SOURCE_PORT_PERMITr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_SRC_PERMIT_ENf, (rtksw_uint32 *)&permit)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_localPktPermit_get
 * Description:
 *      Get permittion of frames if source port and destination port are the same.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pPermit - permittion status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API is to get permittion status for frames if its source port is equal to destination port.
 */
rtksw_api_ret_t dal_rtl8371c_l2_localPktPermit_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pPermit)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pPermit)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_SOURCE_PORT_PERMITr, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_SRC_PERMIT_ENf, (rtksw_uint32 *)pPermit)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_aging_set
 * Description:
 *      Set LUT agging out speed
 * Input:
 *      unit        - Unit ID
 *      aging_time  - Agging out time.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can set LUT agging out period for each entry and the range is from 45s to 458s.
 */
rtksw_api_ret_t dal_rtl8371c_l2_aging_set(rtksw_uint32 unit, rtksw_l2_age_time_t aging_time)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    if(aging_time > RTL8371C_LUTAGINGTIMER_MAX)
        return RT_ERR_OUT_OF_RANGE;

    regData = (rtksw_uint32)(aging_time * 10);
    if ((retVal = reg16_field_write(unit, RTL8371C_AGE_TIMER_CTRLr, RTL8371C_AGE_TIMERf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_aging_get
 * Description:
 *      Get LUT agging out time
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - Aging status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get LUT agging out period for each entry.
 */
rtksw_api_ret_t dal_rtl8371c_l2_aging_get(rtksw_uint32 unit, rtksw_l2_age_time_t *pAging_time)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    if ((retVal = reg16_field_read(unit, RTL8371C_AGE_TIMER_CTRLr, RTL8371C_AGE_TIMERf, &regData)) != RT_ERR_OK)
        return retVal;

    *pAging_time = regData / 10;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddrLookup_set
 * Description:
 *      Set Lut IP multicast lookup function
 * Input:
 *      unit    - Unit ID
 *      type    - Lookup type for IPMC packet.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *      LOOKUP_MAC      - Lookup by MAC address
 *      LOOKUP_IP       - Lookup by IP address
 */
rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddrLookup_set(rtksw_uint32 unit, rtksw_l2_ipmc_lookup_type_t type)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(type == LOOKUP_MAC)
        regData = 0;
    else if(type == LOOKUP_IP)
        regData = 1;
    else
        return RT_ERR_INPUT;
    
    if ((retVal = reg16_field_write(unit, RTL8371C_LUT_CFGr, RTL8371C_LUT_IPMC_HASHf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddrLookup_get
 * Description:
 *      Get Lut IP multicast lookup function
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pType - Lookup type for IPMC packet.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddrLookup_get(rtksw_uint32 unit, rtksw_l2_ipmc_lookup_type_t *pType)
{
    rtksw_api_ret_t       retVal;
    rtksw_uint32          regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pType)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_LUT_CFGr, RTL8371C_LUT_IPMC_HASHf, &regData)) != RT_ERR_OK)
        return retVal;

    if(regData == 1)
        *pType = LOOKUP_IP;
    else
        *pType = LOOKUP_MAC;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastGroupEntry_add
 * Description:
 *      Add an IP Multicast entry to group table
 * Input:
 *      unit        - Unit ID
 *      ip_addr     - IP address
 *      vid         - VLAN ID
 *      pPortmask   - portmask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 *      RT_ERR_TBL_FULL    - Table Full
 * Note:
 *      Add an entry to IP Multicast Group table.
 */
rtksw_api_ret_t dal_rtl8371c_l2_ipMcastGroupEntry_add(rtksw_uint32 unit, ipaddr_t ip_addr, rtksw_uint32 vid, rtksw_portmask_t *pPortmask)
{
    rtksw_uint32      empty_idx = 0xFFFF;
    rtksw_int32       index;
    ipaddr_t        group_addr;
    rtksw_uint32      pmask;
    rtksw_uint32      valid;
    rtksw_uint32      physicalPortmask;
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(vid)//remove warning
    {
    }

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    if((ip_addr & 0xF0000000) != 0xE0000000)
        return RT_ERR_INPUT;

    /* Get Physical port mask */
    if ((retVal = rtksw_switch_portmask_L2P_get(unit, pPortmask, &physicalPortmask))!=RT_ERR_OK)
        return retVal;

    for(index = 0; index <= RTL8371C_LUT_IPMCGRP_TABLE_MAX; index++)
    {
        if ((retVal = _rtl8371c_getLutIPMCGroup(unit, (rtksw_uint32)index, &group_addr, &pmask, &valid))!=RT_ERR_OK)
            return retVal;

        if( (valid == RTKSW_ENABLED) && (group_addr == ip_addr))
        {
            if(pmask != physicalPortmask)
            {
                pmask = physicalPortmask;
                if ((retVal = _rtl8371c_setLutIPMCGroup(unit, index, ip_addr, pmask, valid))!=RT_ERR_OK)
                    return retVal;
            }

            return RT_ERR_OK;
        }

        if( (valid == RTKSW_DISABLED) && (empty_idx == 0xFFFF) ) /* Unused */
            empty_idx = (rtksw_uint32)index;
    }

    if(empty_idx == 0xFFFF)
        return RT_ERR_TBL_FULL;

    pmask = physicalPortmask;
    if ((retVal = _rtl8371c_setLutIPMCGroup(unit, empty_idx, ip_addr, pmask, RTKSW_ENABLED))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastGroupEntry_del
 * Description:
 *      Delete an entry from IP Multicast group table
 * Input:
 *      unit        - Unit ID
 *      ip_addr     - IP address
 *      vid         - VLAN ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 *      RT_ERR_TBL_FULL    - Table Full
 * Note:
 *      Delete an entry from IP Multicast group table.
 */
rtksw_api_ret_t dal_rtl8371c_l2_ipMcastGroupEntry_del(rtksw_uint32 unit, ipaddr_t ip_addr, rtksw_uint32 vid)
{
    rtksw_int32       index;
    ipaddr_t        group_addr;
    rtksw_uint32      pmask;
    rtksw_uint32      valid;
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(vid)//remove warning
    {
    }

    if((ip_addr & 0xF0000000) != 0xE0000000)
        return RT_ERR_INPUT;

    for(index = 0; index <= RTL8371C_LUT_IPMCGRP_TABLE_MAX; index++)
    {
        if ((retVal = _rtl8371c_getLutIPMCGroup(unit, (rtksw_uint32)index, &group_addr, &pmask, &valid))!=RT_ERR_OK)
            return retVal;

        if( (valid == RTKSW_ENABLED) && (group_addr == ip_addr) )
        {
            group_addr = 0xE0000000;
            pmask = 0;
            if ((retVal = _rtl8371c_setLutIPMCGroup(unit, index, group_addr, pmask, RTKSW_DISABLED))!=RT_ERR_OK)
                return retVal;

            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastGroupEntry_get
 * Description:
 *      get an entry from IP Multicast group table
 * Input:
 *      unit        - Unit ID
 *      ip_addr     - IP address
 *      vid         - VLAN ID
 * Output:
 *      pPortmask   - member port mask
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 *      RT_ERR_TBL_FULL    - Table Full
 * Note:
 *      Delete an entry from IP Multicast group table.
 */
rtksw_api_ret_t dal_rtl8371c_l2_ipMcastGroupEntry_get(rtksw_uint32 unit, ipaddr_t ip_addr, rtksw_uint32 vid, rtksw_portmask_t *pPortmask)
{
    rtksw_int32       index;
    ipaddr_t        group_addr;
    rtksw_uint32      valid;
    rtksw_uint32      pmask;
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(vid)//remove warning
    {
    }

    if((ip_addr & 0xF0000000) != 0xE0000000)
        return RT_ERR_INPUT;

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    for(index = 0; index <= RTL8371C_LUT_IPMCGRP_TABLE_MAX; index++)
    {
        if ((retVal = _rtl8371c_getLutIPMCGroup(unit, (rtksw_uint32)index, &group_addr, &pmask, &valid))!=RT_ERR_OK)
            return retVal;

        if( (valid == RTKSW_ENABLED) && (group_addr == ip_addr) )
        {
            if ((retVal = rtksw_switch_portmask_P2L_get(unit, pmask, pPortmask))!=RT_ERR_OK)
                return retVal;

            return RT_ERR_OK;
        }
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_rtl8371c_l2_entry_get
 * Description:
 *      Get LUT unicast entry.
 * Input:
 *      unit        - Unit ID
 *      pL2_entry   - Index field in the structure.
 * Output:
 *      pL2_entry - other fields such as MAC, port, age...
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This API is used to get address by index from 0~2111.
 */
rtksw_api_ret_t dal_rtl8371c_l2_entry_get(rtksw_uint32 unit, rtksw_l2_addr_table_t *pL2_entry)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 method;
    rtl8371c_luttb l2Table;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (pL2_entry->index >= rtksw_switch_maxLutAddrNumber_get(unit))
        return RT_ERR_INPUT;

    osal_memset(&l2Table, 0x00, sizeof(rtl8371c_luttb));
    l2Table.address= pL2_entry->index;
    method = RTL8371C_LUTREADMETHOD_ADDRESS;
    if ((retVal = _rtl8371c_getL2LookupTb(unit, method, &l2Table)) != RT_ERR_OK)
        return retVal;

    if ((pL2_entry->index>0x800)&&(l2Table.lookup_hit==0))
         return RT_ERR_L2_EMPTY_ENTRY;

    if(l2Table.l3lookup)
    {
        osal_memset(&pL2_entry->mac, 0, sizeof(rtksw_mac_t));
        pL2_entry->is_ipmul  = l2Table.l3lookup;
        pL2_entry->sip       = l2Table.sip;
        pL2_entry->dip       = l2Table.dip;
        pL2_entry->is_static = l2Table.nosalearn;

        /* Get Logical port mask */
        if ((retVal = rtksw_switch_portmask_P2L_get(unit, l2Table.mbr, &(pL2_entry->portmask)))!=RT_ERR_OK)
            return retVal;

        pL2_entry->fid       = 0;
        pL2_entry->age       = 0;
        pL2_entry->sa_block  = 0;
        pL2_entry->is_ipvidmul = 0;
        pL2_entry->l3_vid      = 0;

    }
    else if(l2Table.mac.octet[0]&0x01)
    {
        osal_memset(&pL2_entry->sip, 0, sizeof(ipaddr_t));
        osal_memset(&pL2_entry->dip, 0, sizeof(ipaddr_t));
        pL2_entry->mac.octet[0] = l2Table.mac.octet[0];
        pL2_entry->mac.octet[1] = l2Table.mac.octet[1];
        pL2_entry->mac.octet[2] = l2Table.mac.octet[2];
        pL2_entry->mac.octet[3] = l2Table.mac.octet[3];
        pL2_entry->mac.octet[4] = l2Table.mac.octet[4];
        pL2_entry->mac.octet[5] = l2Table.mac.octet[5];
        pL2_entry->is_ipmul  = l2Table.l3lookup;
        pL2_entry->is_static = l2Table.nosalearn;

        /* Get Logical port mask */
        if ((retVal = rtksw_switch_portmask_P2L_get(unit, l2Table.mbr, &(pL2_entry->portmask)))!=RT_ERR_OK)
            return retVal;

        pL2_entry->ivl       = l2Table.ivl_svl;
        if(l2Table.ivl_svl == 1) /* IVL */
        {
            pL2_entry->cvid      = l2Table.cvid_fid;
            pL2_entry->fid       = 0;
        }
        else /* SVL*/
        {
            pL2_entry->cvid      = 0;
            pL2_entry->fid       = l2Table.cvid_fid;
        }
        pL2_entry->age       = 0;
        pL2_entry->is_ipvidmul = 0;
        pL2_entry->l3_vid      = 0;
    }
    else if((l2Table.age != 0)||(l2Table.nosalearn == 1))
    {
        osal_memset(&pL2_entry->sip, 0, sizeof(ipaddr_t));
        osal_memset(&pL2_entry->dip, 0, sizeof(ipaddr_t));
        pL2_entry->mac.octet[0] = l2Table.mac.octet[0];
        pL2_entry->mac.octet[1] = l2Table.mac.octet[1];
        pL2_entry->mac.octet[2] = l2Table.mac.octet[2];
        pL2_entry->mac.octet[3] = l2Table.mac.octet[3];
        pL2_entry->mac.octet[4] = l2Table.mac.octet[4];
        pL2_entry->mac.octet[5] = l2Table.mac.octet[5];
        pL2_entry->is_ipmul  = l2Table.l3lookup;
        pL2_entry->is_static = l2Table.nosalearn;

        /* Get Logical port mask */
        if ((retVal = rtksw_switch_portmask_P2L_get(unit, 1<<(l2Table.spa), &(pL2_entry->portmask)))!=RT_ERR_OK)
            return retVal;

        pL2_entry->ivl       = l2Table.ivl_svl;
        if(l2Table.ivl_svl == 1) /* IVL */
        {
            pL2_entry->cvid      = l2Table.cvid_fid;
            pL2_entry->fid       = 0;
        }
        else /* SVL*/
        {
            pL2_entry->cvid      = 0;
            pL2_entry->fid       = l2Table.cvid_fid;
        }

        pL2_entry->age       = l2Table.age;
        pL2_entry->is_ipvidmul = 0;
        pL2_entry->l3_vid      = 0;
    }
    else
       return RT_ERR_L2_EMPTY_ENTRY;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_lookupHitIsolationAction_set
 * Description:
 *      Set action of lookup hit & isolation.
 * Input:
 *      unit        - Unit ID
 *      action      - The action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      This API is used to configure the action of packet which is lookup hit
 *      in L2 table but the destination port/portmask are not in the port isolation
 *      group.
 */
rtksw_api_ret_t dal_rtl8371c_l2_lookupHitIsolationAction_set(rtksw_uint32 unit, rtksw_l2_lookupHitIsolationAction_t action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    switch (action)
    {
        case L2_LOOKUPHIT_ISOACTION_NOP:
            regData = 0;
            break;
        case L2_LOOKUPHIT_ISOACTION_UNKNOWN:
            regData = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((retVal = reg16_field_write(unit, RTL8371C_CFG_LOOKUP_HIT_ISO_ACTr, RTL8371C_OPTIONf, &regData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_l2_lookupHitIsolationAction_get
 * Description:
 *      Get action of lookup hit & isolation.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pAction         - The action
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_NULL_POINTER         - Null pointer
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      This API is used to get the action of packet which is lookup hit
 *      in L2 table but the destination port/portmask are not in the port isolation
 *      group.
 */
rtksw_api_ret_t dal_rtl8371c_l2_lookupHitIsolationAction_get(rtksw_uint32 unit, rtksw_l2_lookupHitIsolationAction_t *pAction)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (pAction == NULL)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_CFG_LOOKUP_HIT_ISO_ACTr, RTL8371C_OPTIONf, &regData)) != RT_ERR_OK)
        return retVal;

    switch (regData)
    {
        case 0:
            *pAction = L2_LOOKUPHIT_ISOACTION_NOP;
            break;
        case 1:
            *pAction = L2_LOOKUPHIT_ISOACTION_UNKNOWN;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
