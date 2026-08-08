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

#ifndef __DAL_RTL8371C_STAT_H__
#define __DAL_RTL8371C_STAT_H__

#include <stat.h>

#define RTL8371C_MIB_PORT_OFFSET                (0x60)
#define RTL8371C_MIB_LEARNENTRYDISCARD_OFFSET   (0x320)

#define RTL8371C_MAX_LOG_CNT_NUM                (16)
#define RTL8371C_MIB_MAX_LOG_CNT_IDX            (RTL8371C_MAX_LOG_CNT_NUM - 1)
#define RTL8371C_MIB_LOG_CNT_OFFSET             (0x300)
#define RTL8371C_MIB_MAX_LOG_MODE_IDX           (8-1)

typedef enum RTL8371C_MIBCOUNTER_E{

    /* RX */
    ifInOctets = 0,

    dot3StatsFCSErrors,
    dot3StatsSymbolErrors,
    dot3InPauseFrames,
    dot3ControlInUnknownOpcodes,

    etherStatsFragments,
    etherStatsJabbers,
    ifInUcastPkts,
    etherStatsDropEvents,

    ifInMulticastPkts,
    ifInBroadcastPkts,

    /* ether RX */
    rx_etherStatsOctets,

    rx_etherStatsUnderSizePkts,
    rx_etherOversizeStats,
    rx_etherStatsPkts64Octets,
    rx_etherStatsPkts65to127Octets,
    rx_etherStatsPkts128to255Octets,
    rx_etherStatsPkts256to511Octets,
    rx_etherStatsPkts512to1023Octets,
    rx_etherStatsPkts1024to1518Octets,

    /* ether TX */
    tx_etherStatsOctets,

    tx_etherStatsUnderSizePkts,
    tx_etherOversizeStats,
    tx_etherStatsPkts64Octets,
    tx_etherStatsPkts65to127Octets,
    tx_etherStatsPkts128to255Octets,
    tx_etherStatsPkts256to511Octets,
    tx_etherStatsPkts512to1023Octets,
    tx_etherStatsPkts1024to1518Octets,

    /* TX */
    ifOutOctets,

    dot3StatsSingleCollisionFrames,
    dot3StatMultipleCollisionFrames,
    dot3sDeferredTransmissions,
    dot3StatsLateCollisions,
    etherStatsCollisions,
    dot3StatsExcessiveCollisions,
    dot3OutPauseFrames,
    ifOutDiscards,

    /* ALE */
    dot1dTpPortInDiscards,
    ifOutUcastPkts,
    ifOutMulticastPkts,
    ifOutBroadcastPkts,
    outOampduPkts,
    inOampduPkts,

    /*Device only */
    dot1dTpLearnedEntryDiscards,
    RTL8371C_MIBS_NUMBER,

}RTL8371C_MIBCOUNTER;

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
extern rtksw_api_ret_t dal_rtl8371c_stat_global_reset(rtksw_uint32 unit);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_port_reset(rtksw_uint32 unit, rtksw_port_t port);

/* Function Name:
 *      dal_rtl8371c_stat_queueManage_reset
 * Description:
 *      Reset queue manage MIB counter.
 *  Input:
 *      unit        - Unit ID
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
extern rtksw_api_ret_t dal_rtl8371c_stat_queueManage_reset(rtksw_uint32 unit);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_global_get(rtksw_uint32 unit, rtksw_stat_global_type_t cntr_idx, rtksw_stat_counter_t *pCntr);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_global_getAll(rtksw_uint32 unit, rtksw_stat_global_cntr_t *pGlobal_cntrs);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_port_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_stat_port_type_t cntr_idx, rtksw_stat_counter_t *pCntr);

/* Function Name:
 *      dal_rtl8371c_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id.
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
extern rtksw_api_ret_t dal_rtl8371c_stat_port_getAll(rtksw_uint32 unit, rtksw_port_t port, rtksw_stat_port_cntr_t *pPort_cntrs);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_logging_counterCfg_set(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_logging_counter_mode_t mode, rtksw_logging_counter_type_t type);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_logging_counterCfg_get(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_logging_counter_mode_t *pMode, rtksw_logging_counter_type_t *pType);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_logging_counter_reset(rtksw_uint32 unit, rtksw_uint32 idx);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_logging_counter_get(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_uint32 *pCnt);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_lengthMode_set(rtksw_uint32 unit, rtksw_stat_lengthMode_t txMode, rtksw_stat_lengthMode_t rxMode);

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
extern rtksw_api_ret_t dal_rtl8371c_stat_lengthMode_get(rtksw_uint32 unit, rtksw_stat_lengthMode_t *pTxMode, rtksw_stat_lengthMode_t *pRxMode);

#endif /* __DAL_RTL8371C_STAT_H__ */



