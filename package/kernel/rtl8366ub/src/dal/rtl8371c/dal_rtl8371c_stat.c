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
#include <dal/rtl8371c/dal_rtl8371c_stat.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

#define MIB_NOT_SUPPORT     (0xFFFF)
static rtksw_api_ret_t _get_asic_mib_idx(rtksw_stat_port_type_t cnt_idx, RTL8371C_MIBCOUNTER *pMib_idx)
{
    RTL8371C_MIBCOUNTER mib_asic_idx[STAT_PORT_CNTR_END]=
    {
        ifInOctets,                         /* STAT_IfInOctets */
        dot3StatsFCSErrors,                 /* STAT_Dot3StatsFCSErrors */
        dot3StatsSymbolErrors,              /* STAT_Dot3StatsSymbolErrors */
        dot3InPauseFrames,                  /* STAT_Dot3InPauseFrames */
        dot3ControlInUnknownOpcodes,        /* STAT_Dot3ControlInUnknownOpcodes */
        etherStatsFragments,                /* STAT_EtherStatsFragments */
        etherStatsJabbers,                  /* STAT_EtherStatsJabbers */
        ifInUcastPkts,                      /* STAT_IfInUcastPkts */
        etherStatsDropEvents,               /* STAT_EtherStatsDropEvents */
        rx_etherStatsOctets,                /* STAT_EtherStatsOctets */
        rx_etherStatsUnderSizePkts,         /* STAT_EtherStatsUnderSizePkts */
        rx_etherOversizeStats,              /* STAT_EtherOversizeStats */
        rx_etherStatsPkts64Octets,          /* STAT_EtherStatsPkts64Octets */
        rx_etherStatsPkts65to127Octets,     /* STAT_EtherStatsPkts65to127Octets */
        rx_etherStatsPkts128to255Octets,    /* STAT_EtherStatsPkts128to255Octets */
        rx_etherStatsPkts256to511Octets,    /* STAT_EtherStatsPkts256to511Octets */
        rx_etherStatsPkts512to1023Octets,   /* STAT_EtherStatsPkts512to1023Octets */
        rx_etherStatsPkts1024to1518Octets,  /* STAT_EtherStatsPkts1024to1518Octets */
        ifInMulticastPkts,                  /* STAT_EtherStatsMulticastPkts */
        ifInBroadcastPkts,                  /* STAT_EtherStatsBroadcastPkts */
        ifOutOctets,                        /* STAT_IfOutOctets */
        dot3StatsSingleCollisionFrames,     /* STAT_Dot3StatsSingleCollisionFrames */
        dot3StatMultipleCollisionFrames,    /* STAT_Dot3StatsMultipleCollisionFrames */
        dot3sDeferredTransmissions,         /* STAT_Dot3StatsDeferredTransmissions */
        dot3StatsLateCollisions,            /* STAT_Dot3StatsLateCollisions */
        etherStatsCollisions,               /* STAT_EtherStatsCollisions */
        dot3StatsExcessiveCollisions,       /* STAT_Dot3StatsExcessiveCollisions */
        dot3OutPauseFrames,                 /* STAT_Dot3OutPauseFrames */
        MIB_NOT_SUPPORT,                    /* STAT_Dot1dBasePortDelayExceededDiscards */
        dot1dTpPortInDiscards,              /* STAT_Dot1dTpPortInDiscards */
        ifOutUcastPkts,                     /* STAT_IfOutUcastPkts */
        ifOutMulticastPkts,                 /* STAT_IfOutMulticastPkts */
        ifOutBroadcastPkts,                 /* STAT_IfOutBroadcastPkts */
        outOampduPkts,                      /* STAT_OutOampduPkts */
        inOampduPkts,                       /* STAT_InOampduPkts */
        MIB_NOT_SUPPORT,                    /* STAT_PktgenPkts */
        MIB_NOT_SUPPORT,                    /* STAT_InMldChecksumError */
        MIB_NOT_SUPPORT,                    /* STAT_InIgmpChecksumError */
        MIB_NOT_SUPPORT,                    /* STAT_InMldSpecificQuery */
        MIB_NOT_SUPPORT,                    /* STAT_InMldGeneralQuery */
        MIB_NOT_SUPPORT,                    /* STAT_InIgmpSpecificQuery */
        MIB_NOT_SUPPORT,                    /* STAT_InIgmpGeneralQuery */
        MIB_NOT_SUPPORT,                    /* STAT_InMldLeaves */
        MIB_NOT_SUPPORT,                    /* STAT_InIgmpInterfaceLeaves */
        MIB_NOT_SUPPORT,                    /* STAT_InIgmpJoinsSuccess */
        MIB_NOT_SUPPORT,                    /* STAT_InIgmpJoinsFail */
        MIB_NOT_SUPPORT,                    /* STAT_InMldJoinsSuccess */
        MIB_NOT_SUPPORT,                    /* STAT_InMldJoinsFail */
        MIB_NOT_SUPPORT,                    /* STAT_InReportSuppressionDrop */
        MIB_NOT_SUPPORT,                    /* STAT_InLeaveSuppressionDrop */
        MIB_NOT_SUPPORT,                    /* STAT_OutIgmpReports */
        MIB_NOT_SUPPORT,                    /* STAT_OutIgmpLeaves */
        MIB_NOT_SUPPORT,                    /* STAT_OutIgmpGeneralQuery */
        MIB_NOT_SUPPORT,                    /* STAT_OutIgmpSpecificQuery */
        MIB_NOT_SUPPORT,                    /* STAT_OutMldReports */
        MIB_NOT_SUPPORT,                    /* STAT_OutMldLeaves */
        MIB_NOT_SUPPORT,                    /* STAT_OutMldGeneralQuery */
        MIB_NOT_SUPPORT,                    /* STAT_OutMldSpecificQuery */
        MIB_NOT_SUPPORT,                    /* STAT_InKnownMulticastPkts */
        ifInMulticastPkts,                  /* STAT_IfInMulticastPkts */
        ifInBroadcastPkts,                  /* STAT_IfInBroadcastPkts */
        ifOutDiscards                       /* STAT_IfOutDiscards */
    };

    if(cnt_idx >= STAT_PORT_CNTR_END)
        return RT_ERR_STAT_INVALID_PORT_CNTR;

    if(mib_asic_idx[cnt_idx] == MIB_NOT_SUPPORT)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    *pMib_idx = mib_asic_idx[cnt_idx];
    return RT_ERR_OK;
}

static rtksw_api_ret_t _rtl8371c_getMIBsCounter(rtksw_uint32 unit, rtksw_uint32 port, RTL8371C_MIBCOUNTER mibIdx, rtksw_uint64* pCounter)
{
    ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 mibAddr;
    rtksw_uint32 mibOff=0;
    rtksw_uint32 mibRegAddr[4] = {RTL8371C_MIB_DATA_0r, RTL8371C_MIB_DATA_1r, RTL8371C_MIB_DATA_2r, RTL8371C_MIB_DATA_3r};

    /* address offset to MIBs counter */
    CONST rtksw_uint16 mibLength[RTL8371C_MIBS_NUMBER]= {
        4,2,2,2,2,2,2,2,2,2,2,
        4,2,2,2,2,2,2,2,2,
        4,2,2,2,2,2,2,2,2,
        4,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};

    rtksw_uint16 i;
    rtksw_uint64 mibCounter;
    rtksw_uint32 tmpAddr;
    rtksw_int32  off_idx;

    if(mibIdx >= RTL8371C_MIBS_NUMBER)
        return RT_ERR_STAT_INVALID_CNTR;

    if(dot1dTpLearnedEntryDiscards == mibIdx)
    {
        mibAddr = RTL8371C_MIB_LEARNENTRYDISCARD_OFFSET;
    }
    else
    {
        i = 0;
        mibOff = RTL8371C_MIB_PORT_OFFSET * port;

        while(i < mibIdx)
        {
            mibOff += mibLength[i];
            i++;
        }

        mibAddr = mibOff;
    }

    /*writing access counter address first*/
    /*This address is SRAM address, and SRAM address = MIB register address >> 2*/
    /*then ASIC will prepare 64bits counter wait for being retrived*/
    /*Write Mib related address to access control register*/
    tmpAddr = (mibAddr >> 2);        
    if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ADRr, RTL8371C_ADDRf, &tmpAddr)) != RT_ERR_OK)
        return retVal;    

    /* polling busy flag */
    i = 100;
    while(i > 0)
    {
        /*read MIB control register*/
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_BUSY_FLAGf, &regData)) != RT_ERR_OK)
            return retVal;            

        if(regData  == 0)
        {
            break;
        }

        i--;
    }

    if(regData != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    /* Read MIB */
    mibCounter = 0;    
    i = mibLength[mibIdx];
    if(4 == i)
        off_idx = 3;
    else
        off_idx = (mibOff + 1) % 4;

    while(i)
    {
        if ((retVal = reg16_field_read(unit, mibRegAddr[off_idx], RTL8371C_DATAf, &regData)) != RT_ERR_OK)
            return retVal;

        mibCounter = (mibCounter << 16) | (regData & 0xFFFF);

        off_idx--;
        i--;
    }
 
    *pCounter = mibCounter;
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_stat_global_reset
 * Description:
 *      Reset global MIB counter.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Reset MIB counter of ports. API will use global reset while port mask is all-ports.
 */
rtksw_api_ret_t dal_rtl8371c_stat_global_reset(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 fieldData;
    rtksw_int32 i;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_read(unit, RTL8371C_MIB_CMD_REGr, &regData)) != RT_ERR_OK)
        return retVal;

    fieldData = 1;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_GLOBAL_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_QM_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT7_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT6_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT5_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT4_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT3_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT2_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT1_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_set(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT0_RESETf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_write(unit, RTL8371C_MIB_CMD_REGr, &regData)) != RT_ERR_OK)
        return retVal;

    /* polling reset flag */
    i = 100;
    while(i > 0)
    {
        /*read MIB control register*/
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_RESET_FLAGf, &regData)) != RT_ERR_OK)
            return retVal;            

        if(regData  == 0)
        {
            break;
        }

        i--;
    }

    if(regData != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_port_reset
 * Description:
 *      Reset per port MIB counter by port.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_stat_port_reset(rtksw_uint32 unit, rtksw_port_t port)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 phyPort;
    rtksw_int32 i;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

   regData = RTKSW_ENABLED;

    switch (phyPort)
    {
        case 0:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT0_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 1:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT1_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 2:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT2_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 3:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT3_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 4:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT4_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 5:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT5_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 6:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT6_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 7:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_PORT7_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_QOS_INT_PRIORITY;
    }

    /* polling reset flag */
    i = 100;
    while(i > 0)
    {
        /*read MIB control register*/
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_RESET_FLAGf, &regData)) != RT_ERR_OK)
            return retVal;            

        if(regData  == 0)
        {
            break;
        }

        i--;
    }

    if(regData != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_queueManage_reset
 * Description:
 *      Reset queue manage MIB counter.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_stat_queueManage_reset(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint32 i;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);
	
	regData = RTKSW_ENABLED;
    if ((retVal = reg16_field_write(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_QM_RESETf, &regData)) != RT_ERR_OK)
        return retVal;

    /* polling reset flag */
    i = 100;
    while(i > 0)
    {
        /*read MIB control register*/
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_RESET_FLAGf, &regData)) != RT_ERR_OK)
            return retVal;            

        if(regData  == 0)
        {
            break;
        }

        i--;
    }

    if(regData != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_global_get
 * Description:
 *      Get global MIB counter
 *  Input:
 *      unit        - Unit ID
 *      cntr_idx    - global counter index.
 * Output:
 *      pCntr - global counter value.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Get global MIB counter by index definition.
 */
rtksw_api_ret_t dal_rtl8371c_stat_global_get(rtksw_uint32 unit, rtksw_stat_global_type_t cntr_idx, rtksw_stat_counter_t *pCntr)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pCntr)
        return RT_ERR_NULL_POINTER;

    if (cntr_idx!=RTKSW_DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX)
        return RT_ERR_STAT_INVALID_GLOBAL_CNTR;

    if ((retVal = _rtl8371c_getMIBsCounter(unit, 0, dot1dTpLearnedEntryDiscards, pCntr)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_stat_global_getAll
 * Description:
 *      Get all global MIB counter
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pGlobal_cntrs - global counter structure.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Get all global MIB counter by index definition.
 */
rtksw_api_ret_t dal_rtl8371c_stat_global_getAll(rtksw_uint32 unit, rtksw_stat_global_cntr_t *pGlobal_cntrs)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pGlobal_cntrs)
        return RT_ERR_NULL_POINTER;

    if ((retVal = _rtl8371c_getMIBsCounter(unit, 0, dot1dTpLearnedEntryDiscards, &pGlobal_cntrs->dot1dTpLearnedEntryDiscards)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_stat_port_get
 * Description:
 *      Get per port MIB counter by index
 *  Input:
 *      unit        - Unit ID
 *      port        - port id.
 *      cntr_idx    - port counter index.
 * Output:
 *      pCntr - MIB retrived counter.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Get per port MIB counter by index definition.
 */
rtksw_api_ret_t dal_rtl8371c_stat_port_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_stat_port_type_t cntr_idx, rtksw_stat_counter_t *pCntr)
{
    rtksw_api_ret_t       retVal;
    RTL8371C_MIBCOUNTER mib_idx;
    rtksw_stat_counter_t  second_cnt;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pCntr)
        return RT_ERR_NULL_POINTER;

    /* Check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (cntr_idx>=STAT_PORT_CNTR_END)
        return RT_ERR_STAT_INVALID_PORT_CNTR;

    if((retVal = _get_asic_mib_idx(cntr_idx, &mib_idx)) != RT_ERR_OK)
        return retVal;
    
    if(mib_idx == MIB_NOT_SUPPORT)
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((retVal = _rtl8371c_getMIBsCounter(unit, rtksw_switch_port_L2P_get(unit, port), mib_idx, pCntr)) != RT_ERR_OK)
        return retVal;

    if(cntr_idx == STAT_EtherStatsMulticastPkts)
    {
        if((retVal = _get_asic_mib_idx(STAT_IfOutMulticastPkts, &mib_idx)) != RT_ERR_OK)
            return retVal;

        if((retVal = _rtl8371c_getMIBsCounter(unit, rtksw_switch_port_L2P_get(unit, port), mib_idx, &second_cnt)) != RT_ERR_OK)
            return retVal;

        *pCntr += second_cnt;
    }

    if(cntr_idx == STAT_EtherStatsBroadcastPkts)
    {
        if((retVal = _get_asic_mib_idx(STAT_IfOutBroadcastPkts, &mib_idx)) != RT_ERR_OK)
            return retVal;

        if((retVal = _rtl8371c_getMIBsCounter(unit, rtksw_switch_port_L2P_get(unit, port), mib_idx, &second_cnt)) != RT_ERR_OK)
            return retVal;

        *pCntr += second_cnt;
    }

    if ((cntr_idx >= STAT_EtherStatsOctets) && (cntr_idx <= STAT_EtherStatsPkts1024to1518Octets))
    {
        /* read TX part counter */
        mib_idx = tx_etherStatsOctets + (cntr_idx - STAT_EtherStatsOctets);
        if((retVal = _rtl8371c_getMIBsCounter(unit, rtksw_switch_port_L2P_get(unit, port), mib_idx, &second_cnt)) != RT_ERR_OK)
            return retVal;

        *pCntr += second_cnt;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 *  Input:
 *      unit        - Unit ID
 *      port - port id.
 * Output:
 *      pPort_cntrs - buffer pointer of counter value.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Get all MIB counters of one port.
 */
rtksw_api_ret_t dal_rtl8371c_stat_port_getAll(rtksw_uint32 unit, rtksw_port_t port, rtksw_stat_port_cntr_t *pPort_cntrs)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 mibIndex;
    rtksw_uint64 mibCounter;
    rtksw_uint32 *accessPtr;
    /* address offset to MIBs counter */
    CONST_T rtksw_uint16 mibLength[STAT_PORT_CNTR_END]= {
        2,1,1,1,1,1,1,1,1,
        2,1,1,1,1,1,1,1,1,1,1,
        2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pPort_cntrs)
        return RT_ERR_NULL_POINTER;

    /* Check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    accessPtr = (rtksw_uint32*)pPort_cntrs;
    for (mibIndex=0;mibIndex<STAT_PORT_CNTR_END;mibIndex++)
    {
        if ((retVal = dal_rtl8371c_stat_port_get(unit, port, mibIndex, &mibCounter)) != RT_ERR_OK)
        {
            if (retVal == RT_ERR_CHIP_NOT_SUPPORTED)
                mibCounter = 0;
            else
                return retVal;
        }

        if (2 == mibLength[mibIndex])
            *(rtksw_uint64*)accessPtr = mibCounter;
        else if (1 == mibLength[mibIndex])
            *accessPtr = mibCounter;
        else
            return RT_ERR_FAILED;

        accessPtr+=mibLength[mibIndex];
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_logging_counterCfg_set
 * Description:
 *      Set the type and mode of Logging Counter
 *  Input:
 *      unit        - Unit ID
 *      idx         - The index of Logging Counter. Should be even number only.(0,2,4,6,8.....14)
 *      mode        - 32 bits or 64 bits mode
 *      type        - Packet counter or byte counter
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_OUT_OF_RANGE - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Set the type and mode of Logging Counter.
 */
rtksw_api_ret_t dal_rtl8371c_stat_logging_counterCfg_set(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_logging_counter_mode_t mode, rtksw_logging_counter_type_t type)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(idx > RTL8371C_MIB_MAX_LOG_CNT_IDX)
        return RT_ERR_OUT_OF_RANGE;

    if((idx % 2) == 1)
        return RT_ERR_INPUT;

    if(mode >= LOGGING_MODE_END)
        return RT_ERR_OUT_OF_RANGE;

    if(type >= LOGGING_TYPE_END)
        return RT_ERR_OUT_OF_RANGE;

    switch (idx/2)
    {
        case 0:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER0_MODEf, &mode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER0_TYPEf, &type)) != RT_ERR_OK)
                return retVal;
            break;
        case 1:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER1_MODEf, &mode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER1_TYPEf, &type)) != RT_ERR_OK)
                return retVal;
            break;
        case 2:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER2_MODEf, &mode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER2_TYPEf, &type)) != RT_ERR_OK)
                return retVal;
            break;
        case 3:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER3_MODEf, &mode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER3_TYPEf, &type)) != RT_ERR_OK)
                return retVal;
            break;
        case 4:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER4_MODEf, &mode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER4_TYPEf, &type)) != RT_ERR_OK)
                return retVal;
            break;
        case 5:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER5_MODEf, &mode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER5_TYPEf, &type)) != RT_ERR_OK)
                return retVal;
            break;
        case 6:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER6_MODEf, &mode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER6_TYPEf, &type)) != RT_ERR_OK)
                return retVal;
            break;
        case 7:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER7_MODEf, &mode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER7_TYPEf, &type)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_QOS_INT_PRIORITY;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_logging_counterCfg_get
 * Description:
 *      Get the type and mode of Logging Counter
 *  Input:
 *      unit        - Unit ID
 *      idx         - The index of Logging Counter. Should be even number only.(0,2,4,6,8.....14)
 * Output:
 *      pMode   - 32 bits or 64 bits mode
 *      pType   - Packet counter or byte counter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_OUT_OF_RANGE - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - NULL Pointer
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Get the type and mode of Logging Counter.
 */
rtksw_api_ret_t dal_rtl8371c_stat_logging_counterCfg_get(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_logging_counter_mode_t *pMode, rtksw_logging_counter_type_t *pType)
{
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(idx > RTL8371C_MIB_MAX_LOG_CNT_IDX)
        return RT_ERR_OUT_OF_RANGE;

    if((idx % 2) == 1)
        return RT_ERR_INPUT;

    if(pMode == NULL)
        return RT_ERR_NULL_POINTER;

    if(pType == NULL)
        return RT_ERR_NULL_POINTER;

    switch (idx/2)
    {
        case 0:
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER0_MODEf, pMode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER0_TYPEf, pType)) != RT_ERR_OK)
                return retVal;
            break;
        case 1:
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER1_MODEf, pMode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER1_TYPEf, pType)) != RT_ERR_OK)
                return retVal;
            break;
        case 2:
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER2_MODEf, pMode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER2_TYPEf, pType)) != RT_ERR_OK)
                return retVal;
            break;
        case 3:
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER3_MODEf, pMode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER3_TYPEf, pType)) != RT_ERR_OK)
                return retVal;
            break;
        case 4:
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER4_MODEf, pMode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER4_TYPEf, pType)) != RT_ERR_OK)
                return retVal;
            break;
        case 5:
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER5_MODEf, pMode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER5_TYPEf, pType)) != RT_ERR_OK)
                return retVal;
            break;
        case 6:
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER6_MODEf, pMode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER6_TYPEf, pType)) != RT_ERR_OK)
                return retVal;
            break;
        case 7:
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_CNTSELr, RTL8371C_COUNTER7_MODEf, pMode)) != RT_ERR_OK)
                return retVal;            
            if ((retVal = reg16_field_read(unit, RTL8371C_MIB_ACL_TYPESELr, RTL8371C_COUNTER7_TYPEf, pType)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_QOS_INT_PRIORITY;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_logging_counter_reset
 * Description:
 *      Reset Logging Counter
 *  Input:
 *      unit        - Unit ID
 *      idx         - The index of Logging Counter. (0~15)
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_OUT_OF_RANGE - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Reset Logging Counter.
 */
rtksw_api_ret_t dal_rtl8371c_stat_logging_counter_reset(rtksw_uint32 unit, rtksw_uint32 idx)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(idx > RTL8371C_MIB_MAX_LOG_CNT_IDX)
        return RT_ERR_OUT_OF_RANGE;

    regData = RTKSW_ENABLED;
    
    switch (idx)
    {
        case 0:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER0_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 1:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER1_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 2:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER2_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 3:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER3_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 4:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER4_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 5:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER5_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 6:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER6_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 7:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER7_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 8:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER8_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 9:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER9_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 10:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER10_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 11:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER11_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 12:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER12_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 13:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER13_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 14:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER14_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        case 15:
            if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RESET_CMDr, RTL8371C_COUNTER15_RESETf, &regData)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_QOS_INT_PRIORITY;
    }


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_logging_counter_get
 * Description:
 *      Get Logging Counter
 *  Input:
 *      unit        - Unit ID
 *      idx         - The index of Logging Counter. (0~15)
 * Output:
 *      pCnt    - Logging counter value
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_OUT_OF_RANGE - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      Get Logging Counter.
 */
rtksw_api_ret_t dal_rtl8371c_stat_logging_counter_get(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_uint32 *pCnt)
{
    rtksw_api_ret_t   retVal;
    rtksw_uint32 regData, regData2;
    rtksw_uint32 mibAddr;
    rtksw_uint16 i;
    rtksw_uint64 mibCounter;
    rtksw_uint32 tmpAddr;
    rtksw_uint32 mibData[4];    

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pCnt)
        return RT_ERR_NULL_POINTER;

    if(idx > RTL8371C_MIB_MAX_LOG_CNT_IDX)
        return RT_ERR_OUT_OF_RANGE;

    mibAddr = RTL8371C_MIB_LOG_CNT_OFFSET + ((idx / 2) * 4);

    tmpAddr = (mibAddr >> 2);         
    if ((retVal = reg16_field_write(unit, RTL8371C_MIB_ADRr, RTL8371C_ADDRf, &tmpAddr)) != RT_ERR_OK)
        return retVal;    

    /* polling busy flag */
    i = 100;
    while(i > 0)
    {
        /*read MIB control register*/
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_BUSY_FLAGf, &regData)) != RT_ERR_OK)
            return retVal;            
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_CMD_REGr, RTL8371C_RESET_FLAGf, &regData2)) != RT_ERR_OK)
            return retVal;

        if(regData  == 0)
        {
            break;
        }

        i--;
    }

    if(regData != 0)
        return RT_ERR_BUSYWAIT_TIMEOUT;

    if(regData2 != 0)
        return RT_ERR_STAT_CNTR_FAIL;

    mibCounter = 0;
    if((idx % 2) == 1)
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_DATA_2r, RTL8371C_DATAf, &mibData[2])) != RT_ERR_OK)
            return retVal;
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_DATA_3r, RTL8371C_DATAf, &mibData[3])) != RT_ERR_OK)
            return retVal;
        mibCounter = ((mibData[3]&0xFFFF) << 16) | (mibData[2]&0xFFFF);		
    }
    else
    {
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_DATA_0r, RTL8371C_DATAf, &mibData[0])) != RT_ERR_OK)
            return retVal;
        if ((retVal = reg16_field_read(unit, RTL8371C_MIB_DATA_1r, RTL8371C_DATAf, &mibData[1])) != RT_ERR_OK)
            return retVal;
        mibCounter = ((mibData[1]&0xFFFF) << 16) | (mibData[0]&0xFFFF);				
    }   
    *pCnt = mibCounter;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_lengthMode_set
 * Description:
 *      Set Legnth mode.
 *  Input:
 *      unit        - Unit ID
 *      txMode      - The length counting mode
 *      rxMode      - The length counting mode
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_stat_lengthMode_set(rtksw_uint32 unit, rtksw_stat_lengthMode_t txMode, rtksw_stat_lengthMode_t rxMode)
{
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(txMode >= LENGTH_MODE_END)
        return RT_ERR_INPUT;

    if(rxMode >= LENGTH_MODE_END)
        return RT_ERR_INPUT;

    if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RMON_LEN_CTRLr, RTL8371C_TX_LENGTH_CTRLf, &txMode)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_write(unit, RTL8371C_MIB_RMON_LEN_CTRLr, RTL8371C_RX_LENGTH_CTRLf, &rxMode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_stat_lengthMode_get
 * Description:
 *      Get Legnth mode.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pTxMode       - The length counting mode
 *      pRxMode       - The length counting mode
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 */
rtksw_api_ret_t dal_rtl8371c_stat_lengthMode_get(rtksw_uint32 unit, rtksw_stat_lengthMode_t *pTxMode, rtksw_stat_lengthMode_t *pRxMode)
{
    rtksw_api_ret_t   retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pTxMode)
        return RT_ERR_NULL_POINTER;

    if(NULL == pRxMode)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_MIB_RMON_LEN_CTRLr, RTL8371C_TX_LENGTH_CTRLf, pTxMode)) != RT_ERR_OK)
        return retVal;
    if ((retVal = reg16_field_read(unit, RTL8371C_MIB_RMON_LEN_CTRLr, RTL8371C_RX_LENGTH_CTRLf, pRxMode)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}




