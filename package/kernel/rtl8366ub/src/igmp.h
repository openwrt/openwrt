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

#ifndef __RTKSW_API_IGMP_H__
#define __RTKSW_API_IGMP_H__

/*
 * Data Type Declaration
 */
typedef enum rtksw_igmp_type_e
{
    IGMP_IPV4 = 0,
    IGMP_PPPOE_IPV4,
    IGMP_MLD,
    IGMP_PPPOE_MLD,
    IGMP_TYPE_END
} rtksw_igmp_type_t;

typedef enum rtksw_trap_igmp_action_e
{
    IGMP_ACTION_FORWARD = 0,
    IGMP_ACTION_TRAP2CPU,
    IGMP_ACTION_DROP,
    IGMP_ACTION_ASIC,
    IGMP_ACTION_END
} rtksw_igmp_action_t;

typedef enum rtksw_igmp_protocol_e
{
    PROTOCOL_IGMPv1 = 0,
    PROTOCOL_IGMPv2,
    PROTOCOL_IGMPv3,
    PROTOCOL_MLDv1,
    PROTOCOL_MLDv2,
    PROTOCOL_END
} rtksw_igmp_protocol_t;

typedef enum rtksw_igmp_tableFullAction_e
{
    IGMP_TABLE_FULL_FORWARD = 0,
    IGMP_TABLE_FULL_DROP,
    IGMP_TABLE_FULL_TRAP,
    IGMP_TABLE_FULL_OP_END
}rtksw_igmp_tableFullAction_t;

typedef enum rtksw_igmp_checksumErrorAction_e
{
    IGMP_CRC_ERR_DROP = 0,
    IGMP_CRC_ERR_TRAP,
    IGMP_CRC_ERR_FORWARD,
    IGMP_CRC_ERR_OP_END
}rtksw_igmp_checksumErrorAction_t;

typedef enum rtksw_igmp_bypassGroup_e
{
    IGMP_BYPASS_224_0_0_X = 0,
    IGMP_BYPASS_224_0_1_X,
    IGMP_BYPASS_239_255_255_X,
    IGMP_BYPASS_IPV6_00XX,
    IGMP_BYPASS_GROUP_END
}rtksw_igmp_bypassGroup_t;


typedef struct rtksw_igmp_dynamicRouterPort_s
{
    rtksw_enable_t    dynamicRouterPort0Valid;
    rtksw_port_t      dynamicRouterPort0;
    rtksw_uint32      dynamicRouterPort0Timer;
    rtksw_enable_t    dynamicRouterPort1Valid;
    rtksw_port_t      dynamicRouterPort1;
    rtksw_uint32      dynamicRouterPort1Timer;

}rtksw_igmp_dynamicRouterPort_t;

typedef struct rtksw_igmp_rxPktEnable_s
{
    rtksw_enable_t rxQuery;
    rtksw_enable_t rxReport;
    rtksw_enable_t rxLeave;
    rtksw_enable_t rxMRP;
    rtksw_enable_t rxMcast;
}rtksw_igmp_rxPktEnable_t;

typedef struct rtksw_igmp_groupInfo_s
{
    rtksw_enable_t    valid;
    rtksw_portmask_t  member;
    rtksw_uint32      timer[RTKSW_PORT_MAX];
    rtksw_uint32      reportSuppFlag;
}rtksw_igmp_groupInfo_t;

typedef enum rtksw_igmp_ReportLeaveFwdAct_e
{
    IGMP_REPORT_LEAVE_TO_ROUTER = 0,
    IGMP_REPORT_LEAVE_TO_ALLPORT,
    IGMP_REPORT_LEAVE_TO_ROUTER_PORT_ADV,
    IGMP_REPORT_LEAVE_ACT_END
}rtksw_igmp_ReportLeaveFwdAct_t;

/* Function Name:
 *      rtksw_igmp_init
 * Description:
 *      This API enables H/W IGMP and set a default initial configuration.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API enables H/W IGMP and set a default initial configuration.
 */
extern rtksw_api_ret_t rtksw_igmp_init(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_igmp_state_set
 * Description:
 *      This API set H/W IGMP state.
 * Input:
 *      unit        - Unit ID
 *      enabled     - H/W IGMP state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      This API set H/W IGMP state.
 */
extern rtksw_api_ret_t rtksw_igmp_state_set(rtksw_uint32 unit, rtksw_enable_t enabled);

/* Function Name:
 *      rtksw_igmp_state_get
 * Description:
 *      This API get H/W IGMP state.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnabled    - H/W IGMP state
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      This API set current H/W IGMP state.
 */
extern rtksw_api_ret_t rtksw_igmp_state_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled);

/* Function Name:
 *      rtksw_igmp_static_router_port_set
 * Description:
 *      Configure static router port
 * Input:
 *      unit        - Unit ID
 *      pPortmask   - Static Port mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *      This API set static router port
 */
extern rtksw_api_ret_t rtksw_igmp_static_router_port_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_igmp_static_router_port_get
 * Description:
 *      Get static router port
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pPortmask   - Static port mask
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *      This API get static router port
 */
extern rtksw_api_ret_t rtksw_igmp_static_router_port_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_igmp_protocol_set
 * Description:
 *      set IGMP/MLD protocol action
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      protocol    - IGMP/MLD protocol
 *      action      - Per-port and per-protocol IGMP action seeting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *      This API set IGMP/MLD protocol action
 */
extern rtksw_api_ret_t rtksw_igmp_protocol_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_igmp_protocol_t protocol, rtksw_igmp_action_t action);

/* Function Name:
 *      rtksw_igmp_protocol_get
 * Description:
 *      set IGMP/MLD protocol action
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      protocol    - IGMP/MLD protocol
 * Output:
 *      pAction     - Per-port and per-protocol IGMP action seeting
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *      This API set IGMP/MLD protocol action
 */
extern rtksw_api_ret_t rtksw_igmp_protocol_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_igmp_protocol_t protocol, rtksw_igmp_action_t *pAction);

/* Function Name:
 *      rtksw_igmp_fastLeave_set
 * Description:
 *      set IGMP/MLD FastLeave state
 * Input:
 *      unit        - Unit ID
 *      state       - RTKSW_ENABLED: Enable FastLeave, RTKSW_DISABLED: disable FastLeave
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API set IGMP/MLD FastLeave state
 */
extern rtksw_api_ret_t rtksw_igmp_fastLeave_set(rtksw_uint32 unit, rtksw_enable_t state);

/* Function Name:
 *      rtksw_igmp_fastLeave_get
 * Description:
 *      get IGMP/MLD FastLeave state
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pState      - RTKSW_ENABLED: Enable FastLeave, RTKSW_DISABLED: disable FastLeave
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_NULL_POINTER    - NULL pointer
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API get IGMP/MLD FastLeave state
 */
extern rtksw_api_ret_t rtksw_igmp_fastLeave_get(rtksw_uint32 unit, rtksw_enable_t *pState);

/* Function Name:
 *      rtksw_igmp_maxGroup_set
 * Description:
 *      Set per port multicast group learning limit.
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      group       - The number of multicast group learning limit.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_PORT_ID         - Error Port ID
 *      RT_ERR_OUT_OF_RANGE    - parameter out of range
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API set per port multicast group learning limit.
 */
extern rtksw_api_ret_t rtksw_igmp_maxGroup_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 group);

/* Function Name:
 *      rtksw_igmp_maxGroup_get
 * Description:
 *      Get per port multicast group learning limit.
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pGroup      - The number of multicast group learning limit.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_PORT_ID         - Error Port ID
 *      RT_ERR_NULL_POINTER    - Null pointer
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API get per port multicast group learning limit.
 */
extern rtksw_api_ret_t rtksw_igmp_maxGroup_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 *pGroup);

/* Function Name:
 *      rtksw_igmp_currentGroup_get
 * Description:
 *      Get per port multicast group learning count.
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pGroup      - The number of multicast group learning count.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_PORT_ID         - Error Port ID
 *      RT_ERR_NULL_POINTER    - Null pointer
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API get per port multicast group learning count.
 */
extern rtksw_api_ret_t rtksw_igmp_currentGroup_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_uint32 *pGroup);

/* Function Name:
 *      rtksw_igmp_tableFullAction_set
 * Description:
 *      set IGMP/MLD Table Full Action
 * Input:
 *      unit        - Unit ID
 *      action      - Table Full Action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_tableFullAction_set(rtksw_uint32 unit, rtksw_igmp_tableFullAction_t action);

/* Function Name:
 *      rtksw_igmp_tableFullAction_get
 * Description:
 *      get IGMP/MLD Table Full Action
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pAction     - Table Full Action
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_NULL_POINTER    - Null pointer
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_tableFullAction_get(rtksw_uint32 unit, rtksw_igmp_tableFullAction_t *pAction);

/* Function Name:
 *      rtksw_igmp_checksumErrorAction_set
 * Description:
 *      set IGMP/MLD Checksum Error Action
 * Input:
 *      unit        - Unit ID
 *      action      - Checksum error Action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_checksumErrorAction_set(rtksw_uint32 unit, rtksw_igmp_checksumErrorAction_t action);


/* Function Name:
 *      rtksw_igmp_checksumErrorAction_get
 * Description:
 *      get IGMP/MLD Checksum Error Action
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pAction     - Checksum error Action
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_NULL_POINTER    - Null pointer
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_checksumErrorAction_get(rtksw_uint32 unit, rtksw_igmp_checksumErrorAction_t *pAction);

/* Function Name:
 *      rtksw_igmp_leaveTimer_set
 * Description:
 *      set IGMP/MLD Leave timer
 * Input:
 *      unit        - Unit ID
 *      timer       - Leave timer
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_leaveTimer_set(rtksw_uint32 unit, rtksw_uint32 timer);

/* Function Name:
 *      rtksw_igmp_leaveTimer_get
 * Description:
 *      get IGMP/MLD Leave timer
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pTimer      - Leave Timer.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_NULL_POINTER    - Null pointer
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_leaveTimer_get(rtksw_uint32 unit, rtksw_uint32 *pTimer);

/* Function Name:
 *      rtksw_igmp_queryInterval_set
 * Description:
 *      set IGMP/MLD Query Interval
 * Input:
 *      unit        - Unit ID
 *      interval    - Query Interval
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_queryInterval_set(rtksw_uint32 unit, rtksw_uint32 interval);

/* Function Name:
 *      rtksw_igmp_queryInterval_get
 * Description:
 *      get IGMP/MLD Query Interval
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pInterval   - Query Interval
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_NULL_POINTER    - Null pointer
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_queryInterval_get(rtksw_uint32 unit, rtksw_uint32 *pInterval);

/* Function Name:
 *      rtksw_igmp_robustness_set
 * Description:
 *      set IGMP/MLD Robustness value
 * Input:
 *      unit        - Unit ID
 *      robustness  - Robustness value
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_robustness_set(rtksw_uint32 unit, rtksw_uint32 robustness);

/* Function Name:
 *      rtksw_igmp_robustness_get
 * Description:
 *      get IGMP/MLD Robustness value
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pRobustness - Robustness value.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_NULL_POINTER    - Null pointer
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 */
extern rtksw_api_ret_t rtksw_igmp_robustness_get(rtksw_uint32 unit, rtksw_uint32 *pRobustness);

/* Function Name:
 *      rtksw_igmp_dynamicRouterRortAllow_set
 * Description:
 *      Configure dynamic router port allow option
 * Input:
 *      unit        - Unit ID
 *      pPortmask   - Dynamic Port allow mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_dynamicRouterPortAllow_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_igmp_dynamicRouterRortAllow_get
 * Description:
 *      Get dynamic router port allow option
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pPortmask   - Dynamic Port allow mask
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_NULL_POINTER    - Null pointer
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_dynamicRouterPortAllow_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_igmp_dynamicRouterPort_get
 * Description:
 *      Get dynamic router port
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pDynamicRouterPort    - Dynamic Router Port
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_NULL_POINTER    - Null pointer
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_MASK       - Error parameter
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_dynamicRouterPort_get(rtksw_uint32 unit, rtksw_igmp_dynamicRouterPort_t *pDynamicRouterPort);

/* Function Name:
 *      rtksw_igmp_suppressionEnable_set
 * Description:
 *      Configure IGMPv1/v2 & MLDv1 Report/Leave/Done suppression
 * Input:
 *      unit                - Unit ID
 *      reportSuppression   - Report suppression
 *      leaveSuppression    - Leave suppression
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_suppressionEnable_set(rtksw_uint32 unit, rtksw_enable_t reportSuppression, rtksw_enable_t leaveSuppression);

/* Function Name:
 *      rtksw_igmp_suppressionEnable_get
 * Description:
 *      Get IGMPv1/v2 & MLDv1 Report/Leave/Done suppression
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pReportSuppression  - Report suppression
 *      pLeaveSuppression   - Leave suppression
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_NULL_POINTER    - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_suppressionEnable_get(rtksw_uint32 unit, rtksw_enable_t *pReportSuppression, rtksw_enable_t *pLeaveSuppression);

/* Function Name:
 *      rtksw_igmp_portRxPktEnable_set
 * Description:
 *      Configure IGMP/MLD RX Packet configuration
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 *      pRxCfg      - RX Packet Configuration
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_NULL_POINTER    - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_portRxPktEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_igmp_rxPktEnable_t *pRxCfg);

/* Function Name:
 *      rtksw_igmp_portRxPktEnable_get
 * Description:
 *      Get IGMP/MLD RX Packet configuration
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID
 * Output:
 *      pRxCfg      - RX Packet Configuration.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_NULL_POINTER    - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_portRxPktEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_igmp_rxPktEnable_t *pRxCfg);

/* Function Name:
 *      rtksw_igmp_groupInfo_get
 * Description:
 *      Get IGMP/MLD Group database
 * Input:
 *      unit        - Unit ID
 *      indes       - Index (0~255)
 * Output:
 *      pGroup      - Group database information.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_NULL_POINTER    - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_groupInfo_get(rtksw_uint32 unit, rtksw_uint32 index, rtksw_igmp_groupInfo_t *pGroup);

/* Function Name:
 *      rtksw_igmp_ReportLeaveFwdAction_set
 * Description:
 *      Set Report Leave packet forwarding action
 * Input:
 *      unit        - Unit ID
 *      action      - Action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_ReportLeaveFwdAction_set(rtksw_uint32 unit, rtksw_igmp_ReportLeaveFwdAct_t action);

/* Function Name:
 *      rtksw_igmp_ReportLeaveFwdAction_get
 * Description:
 *      Get Report Leave packet forwarding action
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pAction     - Action.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_NULL_POINTER    - Null Pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_ReportLeaveFwdAction_get(rtksw_uint32 unit, rtksw_igmp_ReportLeaveFwdAct_t *pAction);

/* Function Name:
 *      rtksw_igmp_dropLeaveZeroEnable_set
 * Description:
 *      Set the function of droppping Leave packet with group IP = 0.0.0.0
 * Input:
 *      unit        - Unit ID
 *      enabled     - Action 1: drop, 0:pass
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_dropLeaveZeroEnable_set(rtksw_uint32 unit, rtksw_enable_t enabled);

/* Function Name:
 *      rtksw_igmp_dropLeaveZeroEnable_get
 * Description:
 *      Get the function of droppping Leave packet with group IP = 0.0.0.0
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnabled.   - Action 1: drop, 0:pass
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_NULL_POINTER    - Null Pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_dropLeaveZeroEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled);

/* Function Name:
 *      rtksw_igmp_bypassGroupRange_set
 * Description:
 *      Set Bypass group
 * Input:
 *      unit        - Unit ID
 *      group       - bypassed group
 *      enabled     - enabled 1: Bypassed, 0: not bypass
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_bypassGroupRange_set(rtksw_uint32 unit, rtksw_igmp_bypassGroup_t group, rtksw_enable_t enabled);

/* Function Name:
 *      rtksw_igmp_bypassGroupRange_get
 * Description:
 *      get Bypass group
 * Input:
 *      unit        - Unit ID
 *      group       - bypassed group
 * Output:
 *      pEnable     - enabled 1: Bypassed, 0: not bypass
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error Input
 *      RT_ERR_NULL_POINTER    - Null Pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_igmp_bypassGroupRange_get(rtksw_uint32 unit, rtksw_igmp_bypassGroup_t group, rtksw_enable_t *pEnable);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_igmp_init(void)                                                     rtksw_igmp_init(0)
#define rtk_igmp_state_set(enabled)                                             rtksw_igmp_state_set(0, enabled)
#define rtk_igmp_state_get(pEnabled)                                            rtksw_igmp_state_get(0, pEnabled)
#define rtk_igmp_static_router_port_set(pPortmask)                              rtksw_igmp_static_router_port_set(0, pPortmask)
#define rtk_igmp_static_router_port_get(pPortmask)                              rtksw_igmp_static_router_port_get(0, pPortmask)
#define rtk_igmp_protocol_set(port, protocol, action)                           rtksw_igmp_protocol_set(0, port, protocol, action)
#define rtk_igmp_protocol_get(port, protocol, pAction)                          rtksw_igmp_protocol_get(0, port, protocol, pAction)
#define rtk_igmp_fastLeave_set(state)                                           rtksw_igmp_fastLeave_set(0, state)
#define rtk_igmp_fastLeave_get(pState)                                          rtksw_igmp_fastLeave_get(0, pState)
#define rtk_igmp_maxGroup_set(port, group)                                      rtksw_igmp_maxGroup_set(0, port, group)
#define rtk_igmp_maxGroup_get(port, pGroup)                                     rtksw_igmp_maxGroup_get(0, port, pGroup)
#define rtk_igmp_currentGroup_get(port, pGroup)                                 rtksw_igmp_currentGroup_get(0, port, pGroup)
#define rtk_igmp_tableFullAction_set(action)                                    rtksw_igmp_tableFullAction_set(0, action)
#define rtk_igmp_tableFullAction_get(pAction)                                   rtksw_igmp_tableFullAction_get(0, pAction)
#define rtk_igmp_checksumErrorAction_set(action)                                rtksw_igmp_checksumErrorAction_set(0, action)
#define rtk_igmp_checksumErrorAction_get(pAction)                               rtksw_igmp_checksumErrorAction_get(0, pAction)
#define rtk_igmp_leaveTimer_set(timer)                                          rtksw_igmp_leaveTimer_set(0, timer)
#define rtk_igmp_leaveTimer_get(pTimer)                                         rtksw_igmp_leaveTimer_get(0, pTimer)
#define rtk_igmp_queryInterval_set(interval)                                    rtksw_igmp_queryInterval_set(0, interval)
#define rtk_igmp_queryInterval_get(pInterval)                                   rtksw_igmp_queryInterval_get(0, pInterval)
#define rtk_igmp_robustness_set(robustness)                                     rtksw_igmp_robustness_set(0, robustness)
#define rtk_igmp_robustness_get(pRobustness)                                    rtksw_igmp_robustness_get(0, pRobustness)
#define rtk_igmp_dynamicRouterPortAllow_set(pPortmask)                          rtksw_igmp_dynamicRouterPortAllow_set(0, pPortmask)
#define rtk_igmp_dynamicRouterPortAllow_get(pPortmask)                          rtksw_igmp_dynamicRouterPortAllow_get(0, pPortmask)
#define rtk_igmp_dynamicRouterPort_get(pDynamicRouterPort)                      rtksw_igmp_dynamicRouterPort_get(0, pDynamicRouterPort)
#define rtk_igmp_suppressionEnable_set(reportSuppression, leaveSuppression)     rtksw_igmp_suppressionEnable_set(0, reportSuppression, leaveSuppression)
#define rtk_igmp_suppressionEnable_get(pReportSuppression, pLeaveSuppression)   rtksw_igmp_suppressionEnable_get(0, pReportSuppression, pLeaveSuppression)
#define rtk_igmp_portRxPktEnable_set(port, pRxCfg)                              rtksw_igmp_portRxPktEnable_set(0, port, pRxCfg)
#define rtk_igmp_portRxPktEnable_get(port, pRxCfg)                              rtksw_igmp_portRxPktEnable_get(0, port, pRxCfg)
#define rtk_igmp_groupInfo_get(index, pGroup)                                   rtksw_igmp_groupInfo_get(0, index, pGroup)
#define rtk_igmp_ReportLeaveFwdAction_set(action)                               rtksw_igmp_ReportLeaveFwdAction_set(0, action)
#define rtk_igmp_ReportLeaveFwdAction_get(pAction)                              rtksw_igmp_ReportLeaveFwdAction_get(0, pAction)
#define rtk_igmp_dropLeaveZeroEnable_set(enabled)                               rtksw_igmp_dropLeaveZeroEnable_set(0, enabled)
#define rtk_igmp_dropLeaveZeroEnable_get(pEnabled)                              rtksw_igmp_dropLeaveZeroEnable_get(0, pEnabled)
#define rtk_igmp_bypassGroupRange_set(group, enabled)                           rtksw_igmp_bypassGroupRange_set(0, group, enabled)
#define rtk_igmp_bypassGroupRange_get(group, pEnable)                           rtksw_igmp_bypassGroupRange_get(0, group, pEnable)

#define rtk_igmp_type_t                     rtksw_igmp_type_t
#define rtk_igmp_action_t                   rtksw_igmp_action_t
#define rtk_igmp_protocol_t                 rtksw_igmp_protocol_t
#define rtk_igmp_tableFullAction_t          rtksw_igmp_tableFullAction_t
#define rtk_igmp_checksumErrorAction_t      rtksw_igmp_checksumErrorAction_t
#define rtk_igmp_bypassGroup_t              rtksw_igmp_bypassGroup_t
#define rtk_igmp_dynamicRouterPort_t        rtksw_igmp_dynamicRouterPort_t
#define rtk_igmp_rxPktEnable_t              rtksw_igmp_rxPktEnable_t
#define rtk_igmp_groupInfo_t                rtksw_igmp_groupInfo_t
#define rtk_igmp_ReportLeaveFwdAct_t        rtksw_igmp_ReportLeaveFwdAct_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_IGMP_H__ */
