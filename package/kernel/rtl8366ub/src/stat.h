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

#ifndef __RTKSW_API_STAT_H__
#define __RTKSW_API_STAT_H__

/*
 * Data Type Declaration
 */
typedef rtksw_u_long_t rtksw_stat_counter_t;

/* global statistic counter structure */
typedef struct rtksw_stat_global_cntr_s
{
    rtksw_uint64 dot1dTpLearnedEntryDiscards;
}rtksw_stat_global_cntr_t;

typedef enum rtksw_stat_global_type_e
{
    RTKSW_DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX = 58,
    RTKSW_MIB_GLOBAL_CNTR_END
}rtksw_stat_global_type_t;

/* port statistic counter structure */
typedef struct rtksw_stat_port_cntr_s
{
    rtksw_uint64 ifInOctets;
    rtksw_uint32 dot3StatsFCSErrors;
    rtksw_uint32 dot3StatsSymbolErrors;
    rtksw_uint32 dot3InPauseFrames;
    rtksw_uint32 dot3ControlInUnknownOpcodes;
    rtksw_uint32 etherStatsFragments;
    rtksw_uint32 etherStatsJabbers;
    rtksw_uint32 ifInUcastPkts;
    rtksw_uint32 etherStatsDropEvents;
    rtksw_uint64 etherStatsOctets;
    rtksw_uint32 etherStatsUndersizePkts;
    rtksw_uint32 etherStatsOversizePkts;
    rtksw_uint32 etherStatsPkts64Octets;
    rtksw_uint32 etherStatsPkts65to127Octets;
    rtksw_uint32 etherStatsPkts128to255Octets;
    rtksw_uint32 etherStatsPkts256to511Octets;
    rtksw_uint32 etherStatsPkts512to1023Octets;
    rtksw_uint32 etherStatsPkts1024toMaxOctets;
    rtksw_uint32 etherStatsMcastPkts;
    rtksw_uint32 etherStatsBcastPkts;
    rtksw_uint64 ifOutOctets;
    rtksw_uint32 dot3StatsSingleCollisionFrames;
    rtksw_uint32 dot3StatsMultipleCollisionFrames;
    rtksw_uint32 dot3StatsDeferredTransmissions;
    rtksw_uint32 dot3StatsLateCollisions;
    rtksw_uint32 etherStatsCollisions;
    rtksw_uint32 dot3StatsExcessiveCollisions;
    rtksw_uint32 dot3OutPauseFrames;
    rtksw_uint32 dot1dBasePortDelayExceededDiscards;
    rtksw_uint32 dot1dTpPortInDiscards;
    rtksw_uint32 ifOutUcastPkts;
    rtksw_uint32 ifOutMulticastPkts;
    rtksw_uint32 ifOutBrocastPkts;
    rtksw_uint32 outOampduPkts;
    rtksw_uint32 inOampduPkts;
    rtksw_uint32 pktgenPkts;
    rtksw_uint32 inMldChecksumError;
    rtksw_uint32 inIgmpChecksumError;
    rtksw_uint32 inMldSpecificQuery;
    rtksw_uint32 inMldGeneralQuery;
    rtksw_uint32 inIgmpSpecificQuery;
    rtksw_uint32 inIgmpGeneralQuery;
    rtksw_uint32 inMldLeaves;
    rtksw_uint32 inIgmpLeaves;
    rtksw_uint32 inIgmpJoinsSuccess;
    rtksw_uint32 inIgmpJoinsFail;
    rtksw_uint32 inMldJoinsSuccess;
    rtksw_uint32 inMldJoinsFail;
    rtksw_uint32 inReportSuppressionDrop;
    rtksw_uint32 inLeaveSuppressionDrop;
    rtksw_uint32 outIgmpReports;
    rtksw_uint32 outIgmpLeaves;
    rtksw_uint32 outIgmpGeneralQuery;
    rtksw_uint32 outIgmpSpecificQuery;
    rtksw_uint32 outMldReports;
    rtksw_uint32 outMldLeaves;
    rtksw_uint32 outMldGeneralQuery;
    rtksw_uint32 outMldSpecificQuery;
    rtksw_uint32 inKnownMulticastPkts;
    rtksw_uint32 ifInMulticastPkts;
    rtksw_uint32 ifInBroadcastPkts;
    rtksw_uint32 ifOutDiscards;
}rtksw_stat_port_cntr_t;

/* port statistic counter index */
typedef enum rtksw_stat_port_type_e
{
    STAT_IfInOctets = 0,
    STAT_Dot3StatsFCSErrors,
    STAT_Dot3StatsSymbolErrors,
    STAT_Dot3InPauseFrames,
    STAT_Dot3ControlInUnknownOpcodes,
    STAT_EtherStatsFragments,
    STAT_EtherStatsJabbers,
    STAT_IfInUcastPkts,
    STAT_EtherStatsDropEvents,
    STAT_EtherStatsOctets,
    STAT_EtherStatsUnderSizePkts,
    STAT_EtherOversizeStats,
    STAT_EtherStatsPkts64Octets,
    STAT_EtherStatsPkts65to127Octets,
    STAT_EtherStatsPkts128to255Octets,
    STAT_EtherStatsPkts256to511Octets,
    STAT_EtherStatsPkts512to1023Octets,
    STAT_EtherStatsPkts1024to1518Octets,
    STAT_EtherStatsMulticastPkts,
    STAT_EtherStatsBroadcastPkts,
    STAT_IfOutOctets,
    STAT_Dot3StatsSingleCollisionFrames,
    STAT_Dot3StatsMultipleCollisionFrames,
    STAT_Dot3StatsDeferredTransmissions,
    STAT_Dot3StatsLateCollisions,
    STAT_EtherStatsCollisions,
    STAT_Dot3StatsExcessiveCollisions,
    STAT_Dot3OutPauseFrames,
    STAT_Dot1dBasePortDelayExceededDiscards,
    STAT_Dot1dTpPortInDiscards,
    STAT_IfOutUcastPkts,
    STAT_IfOutMulticastPkts,
    STAT_IfOutBroadcastPkts,
    STAT_OutOampduPkts,
    STAT_InOampduPkts,
    STAT_PktgenPkts,
    STAT_InMldChecksumError,
    STAT_InIgmpChecksumError,
    STAT_InMldSpecificQuery,
    STAT_InMldGeneralQuery,
    STAT_InIgmpSpecificQuery,
    STAT_InIgmpGeneralQuery,
    STAT_InMldLeaves,
    STAT_InIgmpInterfaceLeaves,
    STAT_InIgmpJoinsSuccess,
    STAT_InIgmpJoinsFail,
    STAT_InMldJoinsSuccess,
    STAT_InMldJoinsFail,
    STAT_InReportSuppressionDrop,
    STAT_InLeaveSuppressionDrop,
    STAT_OutIgmpReports,
    STAT_OutIgmpLeaves,
    STAT_OutIgmpGeneralQuery,
    STAT_OutIgmpSpecificQuery,
    STAT_OutMldReports,
    STAT_OutMldLeaves,
    STAT_OutMldGeneralQuery,
    STAT_OutMldSpecificQuery,
    STAT_InKnownMulticastPkts,
    STAT_IfInMulticastPkts,
    STAT_IfInBroadcastPkts,
    STAT_IfOutDiscards,
    STAT_PORT_CNTR_END
}rtksw_stat_port_type_t;

typedef enum rtksw_logging_counter_mode_e
{
    LOGGING_MODE_32BIT = 0,
    LOGGING_MODE_64BIT,
    LOGGING_MODE_END
}rtksw_logging_counter_mode_t;

typedef enum rtksw_logging_counter_type_e
{
    LOGGING_TYPE_PACKET = 0,
    LOGGING_TYPE_BYTE,
    LOGGING_TYPE_END
}rtksw_logging_counter_type_t;

typedef enum rtksw_stat_lengthMode_e
{
    LENGTH_MODE_EXC_TAG = 0,
    LENGTH_MODE_INC_TAG,
    LENGTH_MODE_END
}rtksw_stat_lengthMode_t;



/* Function Name:
 *      rtksw_stat_global_reset
 * Description:
 *      Reset global MIB counter.
 * Input:
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
extern rtksw_api_ret_t rtksw_stat_global_reset(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_stat_port_reset
 * Description:
 *      Reset per port MIB counter by port.
 * Input:
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
extern rtksw_api_ret_t rtksw_stat_port_reset(rtksw_uint32 unit, rtksw_port_t port);

/* Function Name:
 *      rtksw_stat_queueManage_reset
 * Description:
 *      Reset queue manage MIB counter.
 * Input:
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
extern rtksw_api_ret_t rtksw_stat_queueManage_reset(rtksw_uint32 unit);


/* Function Name:
 *      rtksw_stat_global_get
 * Description:
 *      Get global MIB counter
 * Input:
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
extern rtksw_api_ret_t rtksw_stat_global_get(rtksw_uint32 unit, rtksw_stat_global_type_t cntr_idx, rtksw_stat_counter_t *pCntr);

/* Function Name:
 *      rtksw_stat_global_getAll
 * Description:
 *      Get all global MIB counter
 * Input:
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
extern rtksw_api_ret_t rtksw_stat_global_getAll(rtksw_uint32 unit, rtksw_stat_global_cntr_t *pGlobal_cntrs);

/* Function Name:
 *      rtksw_stat_port_get
 * Description:
 *      Get per port MIB counter by index
 * Input:
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
extern rtksw_api_ret_t rtksw_stat_port_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_stat_port_type_t cntr_idx, rtksw_stat_counter_t *pCntr);

/* Function Name:
 *      rtksw_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
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
extern rtksw_api_ret_t rtksw_stat_port_getAll(rtksw_uint32 unit, rtksw_port_t port, rtksw_stat_port_cntr_t *pPort_cntrs);

/* Function Name:
 *      rtksw_stat_logging_counterCfg_set
 * Description:
 *      Set the type and mode of Logging Counter
 * Input:
 *      unit    - Unit ID
 *      idx     - The index of Logging Counter. Should be even number only.(0,2,4,6,8.....30)
 *      mode    - 32 bits or 64 bits mode
 *      type    - Packet counter or byte counter
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
extern rtksw_api_ret_t rtksw_stat_logging_counterCfg_set(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_logging_counter_mode_t mode, rtksw_logging_counter_type_t type);

/* Function Name:
 *      rtksw_stat_logging_counterCfg_get
 * Description:
 *      Get the type and mode of Logging Counter
 * Input:
 *      unit    - Unit ID
 *      idx     - The index of Logging Counter. Should be even number only.(0,2,4,6,8.....30)
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
extern rtksw_api_ret_t rtksw_stat_logging_counterCfg_get(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_logging_counter_mode_t *pMode, rtksw_logging_counter_type_t *pType);


/* Function Name:
 *      rtksw_stat_logging_counter_reset
 * Description:
 *      Reset Logging Counter
 * Input:
 *      unit        - Unit ID
 *      idx         - The index of Logging Counter. (0~31)
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
extern rtksw_api_ret_t rtksw_stat_logging_counter_reset(rtksw_uint32 unit, rtksw_uint32 idx);

/* Function Name:
 *      rtksw_stat_logging_counter_get
 * Description:
 *      Get Logging Counter
 * Input:
 *      unit    - Unit ID
 *      idx     - The index of Logging Counter. (0~31)
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
extern rtksw_api_ret_t rtksw_stat_logging_counter_get(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_uint32 *pCnt);

/* Function Name:
 *      rtksw_stat_lengthMode_set
 * Description:
 *      Set Legnth mode.
 * Input:
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
extern rtksw_api_ret_t rtksw_stat_lengthMode_set(rtksw_uint32 unit, rtksw_stat_lengthMode_t txMode, rtksw_stat_lengthMode_t rxMode);

/* Function Name:
 *      rtksw_stat_lengthMode_get
 * Description:
 *      Get Legnth mode.
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pTxMode         - The length counting mode
 *      pRxMode         - The length counting mode
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - Out of range.
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_stat_lengthMode_get(rtksw_uint32 unit, rtksw_stat_lengthMode_t *pTxMode, rtksw_stat_lengthMode_t *pRxMode);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_stat_global_reset(void)                         rtksw_stat_global_reset(0)
#define rtk_stat_port_reset(port)                           rtksw_stat_port_reset(0, port)
#define rtk_stat_queueManage_reset(void)                    rtksw_stat_queueManage_reset(0)
#define rtk_stat_global_get(cntr_idx, pCntr)                rtksw_stat_global_get(0, cntr_idx, pCntr)
#define rtk_stat_global_getAll(pGlobal_cntrs)               rtksw_stat_global_getAll(0, pGlobal_cntrs)
#define rtk_stat_port_get(port, cntr_idx, pCntr)            rtksw_stat_port_get(0, port, cntr_idx, pCntr)
#define rtk_stat_port_getAll(port, pPort_cntrs)             rtksw_stat_port_getAll(0, port, pPort_cntrs)
#define rtk_stat_logging_counterCfg_set(idx, mode, type)    rtksw_stat_logging_counterCfg_set(0, idx, mode, type)
#define rtk_stat_logging_counterCfg_get(idx, pMode, pType)  rtksw_stat_logging_counterCfg_get(0, idx, pMode, pType)
#define rtk_stat_logging_counter_reset(idx)                 rtksw_stat_logging_counter_reset(0, idx)
#define rtk_stat_logging_counter_get(idx, pCnt)             rtksw_stat_logging_counter_get(0, idx, pCnt)
#define rtk_stat_lengthMode_set(txMode, rxMode)             rtksw_stat_lengthMode_set(0, txMode, rxMode)
#define rtk_stat_lengthMode_get(pTxMode, pRxMode)           rtksw_stat_lengthMode_get(0, pTxMode, pRxMode)

#define rtk_stat_counter_t          rtksw_stat_counter_t
#define rtk_stat_global_cntr_t      rtksw_stat_global_cntr_t
#define rtk_stat_global_type_t      rtksw_stat_global_type_t
#define rtk_stat_port_cntr_t        rtksw_stat_port_cntr_t
#define rtk_stat_port_type_t        rtksw_stat_port_type_t
#define rtk_logging_counter_mode_t  rtksw_logging_counter_mode_t
#define rtk_logging_counter_type_t  rtksw_logging_counter_type_t
#define rtk_stat_lengthMode_t       rtksw_stat_lengthMode_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_STAT_H__ */

