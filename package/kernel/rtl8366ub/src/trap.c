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
#include <trap.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      rtksw_trap_unknownUnicastPktAction_set
 * Description:
 *      Set unknown unicast packet action configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - ingress port ID for unknown unicast packet
 *      ucast_action    - Unknown unicast action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 *          - UCAST_ACTION_FLOODING
 */
rtksw_api_ret_t rtksw_trap_unknownUnicastPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t ucast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unknownUnicastPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unknownUnicastPktAction_set(unit, port, ucast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_unknownUnicastPktAction_get
 * Description:
 *      Get unknown unicast packet action configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - ingress port ID for unknown unicast packet
 * Output:
 *      pUcast_action   - Unknown unicast action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 *      RT_ERR_NULL_POINTER        - Null pointer
 * Note:
 *      This API can get unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 *          - UCAST_ACTION_FLOODING
 */
rtksw_api_ret_t rtksw_trap_unknownUnicastPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t *pUcast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unknownUnicastPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unknownUnicastPktAction_get(unit, port, pUcast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_unknownMacPktAction_set
 * Description:
 *      Set unknown source MAC packet action configuration.
 * Input:
 *      unit            - Unit ID
 *      ucast_action    - Unknown source MAC action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtksw_api_ret_t rtksw_trap_unknownMacPktAction_set(rtksw_uint32 unit, rtksw_trap_ucast_action_t ucast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unknownMacPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unknownMacPktAction_set(unit, ucast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_unknownMacPktAction_get
 * Description:
 *      Get unknown source MAC packet action configuration.
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pUcast_action   - Unknown source MAC action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NULL_POINTER        - Null Pointer.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *
 */
rtksw_api_ret_t rtksw_trap_unknownMacPktAction_get(rtksw_uint32 unit, rtksw_trap_ucast_action_t *pUcast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unknownMacPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unknownMacPktAction_get(unit, pUcast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_unmatchMacPktAction_set
 * Description:
 *      Set unmatch source MAC packet action configuration.
 * Input:
 *      unit            - Unit ID
 *      ucast_action    - Unmatch source MAC action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtksw_api_ret_t rtksw_trap_unmatchMacPktAction_set(rtksw_uint32 unit, rtksw_trap_ucast_action_t ucast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unmatchMacPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unmatchMacPktAction_set(unit, ucast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_unmatchMacPktAction_get
 * Description:
 *      Get unmatch source MAC packet action configuration.
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pUcast_action   - Unmatch source MAC action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtksw_api_ret_t rtksw_trap_unmatchMacPktAction_get(rtksw_uint32 unit, rtksw_trap_ucast_action_t *pUcast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unmatchMacPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unmatchMacPktAction_get(unit, pUcast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_unmatchMacMoving_set
 * Description:
 *      Set unmatch source MAC packet moving state.
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID.
 *      enable      - RTKSW_ENABLED: allow SA moving, DISABLE: don't allow SA moving.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 */
rtksw_api_ret_t rtksw_trap_unmatchMacMoving_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unmatchMacMoving_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unmatchMacMoving_set(unit, port, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_unmatchMacMoving_get
 * Description:
 *      Set unmatch source MAC packet moving state.
 * Input:
 *      unit        - Unit ID
 *      port        - Port ID.
 * Output:
 *      pEnable     - RTKSW_ENABLED: allow SA moving, DISABLE: don't allow SA moving.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 */
rtksw_api_ret_t rtksw_trap_unmatchMacMoving_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unmatchMacMoving_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unmatchMacMoving_get(unit, port, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_unknownMcastPktAction_set
 * Description:
 *      Set behavior of unknown multicast
 * Input:
 *      unit            - Unit ID
 *      port            - Port id.
 *      type            - unknown multicast packet type.
 *      mcast_action    - unknown multicast action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID         - Invalid port number.
 *      RT_ERR_NOT_ALLOWED     - Invalid action.
 *      RT_ERR_INPUT         - Invalid input parameters.
 * Note:
 *      When receives an unknown multicast packet, switch may trap, drop or flood this packet
 *      (1) The unknown multicast packet type is as following:
 *          - MCAST_L2
 *          - MCAST_IPV4
 *          - MCAST_IPV6
 *      (2) The unknown multicast action is as following:
 *          - MCAST_ACTION_FORWARD
 *          - MCAST_ACTION_DROP
 *          - MCAST_ACTION_TRAP2CPU
 */
rtksw_api_ret_t rtksw_trap_unknownMcastPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_mcast_type_t type, rtksw_trap_mcast_action_t mcast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unknownMcastPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unknownMcastPktAction_set(unit, port, type, mcast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_unknownMcastPktAction_get
 * Description:
 *      Get behavior of unknown multicast
 * Input:
 *      unit        - Unit ID
 *      type        - unknown multicast packet type.
 * Output:
 *      pMcast_action - unknown multicast action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_PORT_ID             - Invalid port number.
 *      RT_ERR_NOT_ALLOWED         - Invalid operation.
 *      RT_ERR_INPUT             - Invalid input parameters.
 * Note:
 *      When receives an unknown multicast packet, switch may trap, drop or flood this packet
 *      (1) The unknown multicast packet type is as following:
 *          - MCAST_L2
 *          - MCAST_IPV4
 *          - MCAST_IPV6
 *      (2) The unknown multicast action is as following:
 *          - MCAST_ACTION_FORWARD
 *          - MCAST_ACTION_DROP
 *          - MCAST_ACTION_TRAP2CPU
 */
rtksw_api_ret_t rtksw_trap_unknownMcastPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mcast_type_t type, rtksw_trap_mcast_action_t *pMcast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_unknownMcastPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_unknownMcastPktAction_get(unit, port, type, pMcast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_lldpEnable_set
 * Description:
 *      Set LLDP enable.
 * Input:
 *      unit        - Unit ID
 *      enabled     - LLDP enable, 0: follow RMA, 1: use LLDP action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT             - Invalid input parameters.
 * Note:
 *      - DMAC                                                 Assignment
 *      - 01:80:c2:00:00:0e ethertype = 0x88CC    LLDP
 *      - 01:80:c2:00:00:03 ethertype = 0x88CC
 *      - 01:80:c2:00:00:00 ethertype = 0x88CC

 */
rtksw_api_ret_t rtksw_trap_lldpEnable_set(rtksw_uint32 unit, rtksw_enable_t enabled)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_lldpEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_lldpEnable_set(unit, enabled);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_lldpEnable_get
 * Description:
 *      Get LLDP status.
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pEnabled - LLDP enable, 0: follow RMA, 1: use LLDP action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT         - Invalid input parameters.
 * Note:
 *      LLDP is as following definition.
 *      - DMAC                                                 Assignment
 *      - 01:80:c2:00:00:0e ethertype = 0x88CC    LLDP
 *      - 01:80:c2:00:00:03 ethertype = 0x88CC
 *      - 01:80:c2:00:00:00 ethertype = 0x88CC
 */
rtksw_api_ret_t rtksw_trap_lldpEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_lldpEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_lldpEnable_get(unit, pEnabled);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_reasonTrapToCpuPriority_set
 * Description:
 *      Set priority value of a packet that trapped to CPU port according to specific reason.
 * Input:
 *      unit            - Unit ID
 *      type            - reason that trap to CPU port.
 *      priority        - internal priority that is going to be set for specific trap reason.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      Currently the trap reason that supported are listed as follows:
 *      - TRAP_REASON_RMA
 *      - TRAP_REASON_OAM
 *      - TRAP_REASON_1XUNAUTH
 *      - TRAP_REASON_VLANSTACK
 *      - TRAP_REASON_UNKNOWNMC
 *      - TRAP_REASON_IGMPMLD
 */
rtksw_api_ret_t rtksw_trap_reasonTrapToCpuPriority_set(rtksw_uint32 unit, rtksw_trap_reason_type_t type, rtksw_pri_t priority)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_reasonTrapToCpuPriority_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_reasonTrapToCpuPriority_set(unit, type, priority);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


/* Function Name:
 *      rtksw_trap_reasonTrapToCpuPriority_get
 * Description:
 *      Get priority value of a packet that trapped to CPU port according to specific reason.
 * Input:
 *      unit        - Unit ID
 *      type        - reason that trap to CPU port.
 * Output:
 *      pPriority   - configured internal priority for such reason.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      Currently the trap reason that supported are listed as follows:
 *      - TRAP_REASON_RMA
 *      - TRAP_REASON_OAM
 *      - TRAP_REASON_1XUNAUTH
 *      - TRAP_REASON_VLANSTACK
 *      - TRAP_REASON_UNKNOWNMC
 *      - TRAP_REASON_IGMPMLD
 */
rtksw_api_ret_t rtksw_trap_reasonTrapToCpuPriority_get(rtksw_uint32 unit, rtksw_trap_reason_type_t type, rtksw_pri_t *pPriority)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_reasonTrapToCpuPriority_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_reasonTrapToCpuPriority_get(unit, type, pPriority);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}



/* Function Name:
 *      rtksw_trap_rmaAction_set
 * Description:
 *      Set Reserved multicast address action configuration.
 * Input:
 *      unit        - Unit ID
 *      type        - rma type.
 *      rma_action  - RMA action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *
 *      There are 48 types of Reserved Multicast Address frame for application usage.
 *      (1)They are as following definition.
 *      - TRAP_BRG_GROUP,
 *      - TRAP_FD_PAUSE,
 *      - TRAP_SP_MCAST,
 *      - TRAP_1X_PAE,
 *      - TRAP_UNDEF_BRG_04,
 *      - TRAP_UNDEF_BRG_05,
 *      - TRAP_UNDEF_BRG_06,
 *      - TRAP_UNDEF_BRG_07,
 *      - TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - TRAP_UNDEF_BRG_09,
 *      - TRAP_UNDEF_BRG_0A,
 *      - TRAP_UNDEF_BRG_0B,
 *      - TRAP_UNDEF_BRG_0C,
 *      - TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - TRAP_8021AB,
 *      - TRAP_UNDEF_BRG_0F,
 *      - TRAP_BRG_MNGEMENT,
 *      - TRAP_UNDEFINED_11,
 *      - TRAP_UNDEFINED_12,
 *      - TRAP_UNDEFINED_13,
 *      - TRAP_UNDEFINED_14,
 *      - TRAP_UNDEFINED_15,
 *      - TRAP_UNDEFINED_16,
 *      - TRAP_UNDEFINED_17,
 *      - TRAP_UNDEFINED_18,
 *      - TRAP_UNDEFINED_19,
 *      - TRAP_UNDEFINED_1A,
 *      - TRAP_UNDEFINED_1B,
 *      - TRAP_UNDEFINED_1C,
 *      - TRAP_UNDEFINED_1D,
 *      - TRAP_UNDEFINED_1E,
 *      - TRAP_UNDEFINED_1F,
 *      - TRAP_GMRP,
 *      - TRAP_GVRP,
 *      - TRAP_UNDEF_GARP_22,
 *      - TRAP_UNDEF_GARP_23,
 *      - TRAP_UNDEF_GARP_24,
 *      - TRAP_UNDEF_GARP_25,
 *      - TRAP_UNDEF_GARP_26,
 *      - TRAP_UNDEF_GARP_27,
 *      - TRAP_UNDEF_GARP_28,
 *      - TRAP_UNDEF_GARP_29,
 *      - TRAP_UNDEF_GARP_2A,
 *      - TRAP_UNDEF_GARP_2B,
 *      - TRAP_UNDEF_GARP_2C,
 *      - TRAP_UNDEF_GARP_2D,
 *      - TRAP_UNDEF_GARP_2E,
 *      - TRAP_UNDEF_GARP_2F,
 *      - TRAP_CDP.
 *      - TRAP_CSSTP.
 *      - TRAP_LLDP.
 *      (2) The RMA action is as following:
 *      - RMA_ACTION_FORWARD
 *      - RMA_ACTION_TRAP2CPU
 *      - RMA_ACTION_DROP
 *      - RMA_ACTION_FORWARD_EXCLUDE_CPU
 */
rtksw_api_ret_t rtksw_trap_rmaAction_set(rtksw_uint32 unit, rtksw_trap_type_t type, rtksw_trap_rma_action_t rma_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_rmaAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_rmaAction_set(unit, type, rma_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_rmaAction_get
 * Description:
 *      Get Reserved multicast address action configuration.
 * Input:
 *      unit        - Unit ID
 *      type        - rma type.
 * Output:
 *      pRma_action - RMA action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      There are 48 types of Reserved Multicast Address frame for application usage.
 *      (1)They are as following definition.
 *      - TRAP_BRG_GROUP,
 *      - TRAP_FD_PAUSE,
 *      - TRAP_SP_MCAST,
 *      - TRAP_1X_PAE,
 *      - TRAP_UNDEF_BRG_04,
 *      - TRAP_UNDEF_BRG_05,
 *      - TRAP_UNDEF_BRG_06,
 *      - TRAP_UNDEF_BRG_07,
 *      - TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - TRAP_UNDEF_BRG_09,
 *      - TRAP_UNDEF_BRG_0A,
 *      - TRAP_UNDEF_BRG_0B,
 *      - TRAP_UNDEF_BRG_0C,
 *      - TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - TRAP_8021AB,
 *      - TRAP_UNDEF_BRG_0F,
 *      - TRAP_BRG_MNGEMENT,
 *      - TRAP_UNDEFINED_11,
 *      - TRAP_UNDEFINED_12,
 *      - TRAP_UNDEFINED_13,
 *      - TRAP_UNDEFINED_14,
 *      - TRAP_UNDEFINED_15,
 *      - TRAP_UNDEFINED_16,
 *      - TRAP_UNDEFINED_17,
 *      - TRAP_UNDEFINED_18,
 *      - TRAP_UNDEFINED_19,
 *      - TRAP_UNDEFINED_1A,
 *      - TRAP_UNDEFINED_1B,
 *      - TRAP_UNDEFINED_1C,
 *      - TRAP_UNDEFINED_1D,
 *      - TRAP_UNDEFINED_1E,
 *      - TRAP_UNDEFINED_1F,
 *      - TRAP_GMRP,
 *      - TRAP_GVRP,
 *      - TRAP_UNDEF_GARP_22,
 *      - TRAP_UNDEF_GARP_23,
 *      - TRAP_UNDEF_GARP_24,
 *      - TRAP_UNDEF_GARP_25,
 *      - TRAP_UNDEF_GARP_26,
 *      - TRAP_UNDEF_GARP_27,
 *      - TRAP_UNDEF_GARP_28,
 *      - TRAP_UNDEF_GARP_29,
 *      - TRAP_UNDEF_GARP_2A,
 *      - TRAP_UNDEF_GARP_2B,
 *      - TRAP_UNDEF_GARP_2C,
 *      - TRAP_UNDEF_GARP_2D,
 *      - TRAP_UNDEF_GARP_2E,
 *      - TRAP_UNDEF_GARP_2F,
 *      - TRAP_CDP.
 *      - TRAP_CSSTP.
 *      - TRAP_LLDP.
 *      (2) The RMA action is as following:
 *      - RMA_ACTION_FORWARD
 *      - RMA_ACTION_TRAP2CPU
 *      - RMA_ACTION_DROP
 *      - RMA_ACTION_FORWARD_EXCLUDE_CPU
 */
rtksw_api_ret_t rtksw_trap_rmaAction_get(rtksw_uint32 unit, rtksw_trap_type_t type, rtksw_trap_rma_action_t *pRma_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_rmaAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_rmaAction_get(unit, type, pRma_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_rmaKeepFormat_set
 * Description:
 *      Set Reserved multicast address keep format configuration.
 * Input:
 *      unit        - Unit ID
 *      type        - rma type.
 *      enable      - enable keep format.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid IFG parameter
 * Note:
 *
 *      There are 48 types of Reserved Multicast Address frame for application usage.
 *      They are as following definition.
 *      - TRAP_BRG_GROUP,
 *      - TRAP_FD_PAUSE,
 *      - TRAP_SP_MCAST,
 *      - TRAP_1X_PAE,
 *      - TRAP_UNDEF_BRG_04,
 *      - TRAP_UNDEF_BRG_05,
 *      - TRAP_UNDEF_BRG_06,
 *      - TRAP_UNDEF_BRG_07,
 *      - TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - TRAP_UNDEF_BRG_09,
 *      - TRAP_UNDEF_BRG_0A,
 *      - TRAP_UNDEF_BRG_0B,
 *      - TRAP_UNDEF_BRG_0C,
 *      - TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - TRAP_8021AB,
 *      - TRAP_UNDEF_BRG_0F,
 *      - TRAP_BRG_MNGEMENT,
 *      - TRAP_UNDEFINED_11,
 *      - TRAP_UNDEFINED_12,
 *      - TRAP_UNDEFINED_13,
 *      - TRAP_UNDEFINED_14,
 *      - TRAP_UNDEFINED_15,
 *      - TRAP_UNDEFINED_16,
 *      - TRAP_UNDEFINED_17,
 *      - TRAP_UNDEFINED_18,
 *      - TRAP_UNDEFINED_19,
 *      - TRAP_UNDEFINED_1A,
 *      - TRAP_UNDEFINED_1B,
 *      - TRAP_UNDEFINED_1C,
 *      - TRAP_UNDEFINED_1D,
 *      - TRAP_UNDEFINED_1E,
 *      - TRAP_UNDEFINED_1F,
 *      - TRAP_GMRP,
 *      - TRAP_GVRP,
 *      - TRAP_UNDEF_GARP_22,
 *      - TRAP_UNDEF_GARP_23,
 *      - TRAP_UNDEF_GARP_24,
 *      - TRAP_UNDEF_GARP_25,
 *      - TRAP_UNDEF_GARP_26,
 *      - TRAP_UNDEF_GARP_27,
 *      - TRAP_UNDEF_GARP_28,
 *      - TRAP_UNDEF_GARP_29,
 *      - TRAP_UNDEF_GARP_2A,
 *      - TRAP_UNDEF_GARP_2B,
 *      - TRAP_UNDEF_GARP_2C,
 *      - TRAP_UNDEF_GARP_2D,
 *      - TRAP_UNDEF_GARP_2E,
 *      - TRAP_UNDEF_GARP_2F,
 *      - TRAP_CDP.
 *      - TRAP_CSSTP.
 *      - TRAP_LLDP.
 */
rtksw_api_ret_t rtksw_trap_rmaKeepFormat_set(rtksw_uint32 unit, rtksw_trap_type_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_rmaKeepFormat_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_rmaKeepFormat_set(unit, type, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_rmaKeepFormat_get
 * Description:
 *      Get Reserved multicast address action configuration.
 * Input:
 *      unit        - Unit ID
 *      type        - rma type.
 * Output:
 *      pEnable     - keep format status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      There are 48 types of Reserved Multicast Address frame for application usage.
 *      They are as following definition.
 *      - TRAP_BRG_GROUP,
 *      - TRAP_FD_PAUSE,
 *      - TRAP_SP_MCAST,
 *      - TRAP_1X_PAE,
 *      - TRAP_UNDEF_BRG_04,
 *      - TRAP_UNDEF_BRG_05,
 *      - TRAP_UNDEF_BRG_06,
 *      - TRAP_UNDEF_BRG_07,
 *      - TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - TRAP_UNDEF_BRG_09,
 *      - TRAP_UNDEF_BRG_0A,
 *      - TRAP_UNDEF_BRG_0B,
 *      - TRAP_UNDEF_BRG_0C,
 *      - TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - TRAP_8021AB,
 *      - TRAP_UNDEF_BRG_0F,
 *      - TRAP_BRG_MNGEMENT,
 *      - TRAP_UNDEFINED_11,
 *      - TRAP_UNDEFINED_12,
 *      - TRAP_UNDEFINED_13,
 *      - TRAP_UNDEFINED_14,
 *      - TRAP_UNDEFINED_15,
 *      - TRAP_UNDEFINED_16,
 *      - TRAP_UNDEFINED_17,
 *      - TRAP_UNDEFINED_18,
 *      - TRAP_UNDEFINED_19,
 *      - TRAP_UNDEFINED_1A,
 *      - TRAP_UNDEFINED_1B,
 *      - TRAP_UNDEFINED_1C,
 *      - TRAP_UNDEFINED_1D,
 *      - TRAP_UNDEFINED_1E,
 *      - TRAP_UNDEFINED_1F,
 *      - TRAP_GMRP,
 *      - TRAP_GVRP,
 *      - TRAP_UNDEF_GARP_22,
 *      - TRAP_UNDEF_GARP_23,
 *      - TRAP_UNDEF_GARP_24,
 *      - TRAP_UNDEF_GARP_25,
 *      - TRAP_UNDEF_GARP_26,
 *      - TRAP_UNDEF_GARP_27,
 *      - TRAP_UNDEF_GARP_28,
 *      - TRAP_UNDEF_GARP_29,
 *      - TRAP_UNDEF_GARP_2A,
 *      - TRAP_UNDEF_GARP_2B,
 *      - TRAP_UNDEF_GARP_2C,
 *      - TRAP_UNDEF_GARP_2D,
 *      - TRAP_UNDEF_GARP_2E,
 *      - TRAP_UNDEF_GARP_2F,
 *      - TRAP_CDP.
 *      - TRAP_CSSTP.
 *      - TRAP_LLDP.
 */
rtksw_api_ret_t rtksw_trap_rmaKeepFormat_get(rtksw_uint32 unit, rtksw_trap_type_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_rmaKeepFormat_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_rmaKeepFormat_get(unit, type, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_portUnknownMacPktAction_set
 * Description:
 *      Set unknown source MAC packet action configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - Port ID.
 *      ucast_action    - Unknown source MAC action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtksw_api_ret_t rtksw_trap_portUnknownMacPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t ucast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_portUnknownMacPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_portUnknownMacPktAction_set(unit, port, ucast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_portUnknownMacPktAction_get
 * Description:
 *      Get unknown source MAC packet action configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - Port ID.
 * Output:
 *      pUcast_action   - Unknown source MAC action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NULL_POINTER        - Null Pointer.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *
 */
rtksw_api_ret_t rtksw_trap_portUnknownMacPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t *pUcast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_portUnknownMacPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_portUnknownMacPktAction_get(unit, port, pUcast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_portUnmatchMacPktAction_set
 * Description:
 *      Set unmatch source MAC packet action configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - Port ID
 *      ucast_action    - Unmatch source MAC action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtksw_api_ret_t rtksw_trap_portUnmatchMacPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t ucast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_portUnmatchMacPktAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_portUnmatchMacPktAction_set(unit, port, ucast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trap_portUnmatchMacPktAction_get
 * Description:
 *      Get unmatch source MAC packet action configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - Port ID
 * Output:
 *      pUcast_action   - Unmatch source MAC action.
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_SMI                 - SMI access error
 *      RT_ERR_NOT_ALLOWED         - Invalid action.
 *      RT_ERR_INPUT               - Invalid input parameters.
 * Note:
 *      This API can set unknown unicast packet action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtksw_api_ret_t rtksw_trap_portUnmatchMacPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t *pUcast_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->trap_portUnmatchMacPktAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trap_portUnmatchMacPktAction_get(unit, port, pUcast_action);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


