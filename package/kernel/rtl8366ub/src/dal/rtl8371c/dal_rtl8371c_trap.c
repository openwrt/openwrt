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
#include <dal/rtl8371c/dal_rtl8371c_trap.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_trap_unknownUnicastPktAction_set
 * Description:
 *      Set unknown unicast packet action configuration.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_unknownUnicastPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t ucast_action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (ucast_action >= UCAST_ACTION_COPY28051)
        return RT_ERR_INPUT;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_L2_UNKNOWN_DA_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, &ucast_action)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_unknownUnicastPktAction_get
 * Description:
 *      Get unknown unicast packet action configuration.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_unknownUnicastPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t *pUcast_action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (NULL == pUcast_action)
        return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_L2_UNKNOWN_DA_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, pUcast_action)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_unmatchMacMoving_set
 * Description:
 *      Set unmatch source MAC packet moving state.
 *  Input:
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

rtksw_api_ret_t dal_rtl8371c_trap_unmatchMacMoving_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;
    rtksw_uint32 portmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = reg16_field_read(unit, RTL8371C_SA_MOVE_PMASKr, RTL8371C_PORTMASKf, &portmask)) != RT_ERR_OK)
        return retVal; 	

    if (enable == RTKSW_ENABLED)
        portmask &= (0xFF & ~(1 << phyPort));
    else
        portmask |= (0xFF & (1 << phyPort));

    if ((retVal = reg16_field_write(unit, RTL8371C_SA_MOVE_PMASKr, RTL8371C_PORTMASKf, &portmask)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_unmatchMacMoving_get
 * Description:
 *      Set unmatch source MAC packet moving state.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_unmatchMacMoving_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;
    rtksw_uint32 portmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = reg16_field_read(unit, RTL8371C_SA_MOVE_PMASKr, RTL8371C_PORTMASKf, &portmask)) != RT_ERR_OK)
        return retVal;

    *pEnable = ((portmask & (1<<phyPort)) != 0) ? RTKSW_DISABLED : RTKSW_ENABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_unknownMcastPktAction_set
 * Description:
 *      Set behavior of unknown multicast
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_unknownMcastPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_mcast_type_t type, rtksw_trap_mcast_action_t mcast_action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rawAction;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (type >= MCAST_END)
        return RT_ERR_INPUT;

    if (mcast_action >= MCAST_ACTION_END)
        return RT_ERR_INPUT;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    switch (type)
    {
        case MCAST_L2:
            if (MCAST_ACTION_ROUTER_PORT == mcast_action)
                return RT_ERR_INPUT;
            else if(MCAST_ACTION_DROP_EX_RMA == mcast_action)
                rawAction = RTL8371C_L2_UNKOWN_MULTICAST_DROP_EXCLUDE_RMA;
            else
                rawAction = (rtksw_uint32)mcast_action;
			
			if ((retVal = reg16_array_field_write(unit, RTL8371C_L2_PORT_MC_LM_ACTr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTf, &rawAction)) != RT_ERR_OK)
				return retVal; 

            break;
        case MCAST_IPV4:
            if ((MCAST_ACTION_DROP_EX_RMA == mcast_action) || (MCAST_ACTION_ROUTER_PORT == mcast_action))
                return RT_ERR_INPUT;
            else
                rawAction = (rtksw_uint32)mcast_action;

			if ((retVal = reg16_array_field_write(unit, RTL8371C_IPV4_PORT_MC_LM_ACTr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTf, &rawAction)) != RT_ERR_OK)
				return retVal;

            break;
        case MCAST_IPV6:
            if ((MCAST_ACTION_DROP_EX_RMA == mcast_action) || (MCAST_ACTION_ROUTER_PORT == mcast_action))
                return RT_ERR_INPUT;
            else
                rawAction = (rtksw_uint32)mcast_action;

			if ((retVal = reg16_array_field_write(unit, RTL8371C_IPV6_PORT_MC_LM_ACTr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTf, &rawAction)) != RT_ERR_OK)
				return retVal;

            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_unknownMcastPktAction_get
 * Description:
 *      Get behavior of unknown multicast
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_unknownMcastPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mcast_type_t type, rtksw_trap_mcast_action_t *pMcast_action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 rawAction;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (type >= MCAST_END)
        return RT_ERR_INPUT;

    if(NULL == pMcast_action)
        return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    switch (type)
    {
        case MCAST_L2:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_L2_PORT_MC_LM_ACTr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTf, &rawAction)) != RT_ERR_OK)
				return retVal;

            if(RTL8371C_L2_UNKOWN_MULTICAST_DROP_EXCLUDE_RMA == rawAction)
                *pMcast_action = MCAST_ACTION_DROP_EX_RMA;
            else
                *pMcast_action = (rtksw_trap_mcast_action_t)rawAction;

            break;
        case MCAST_IPV4:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_IPV4_PORT_MC_LM_ACTr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTf, &rawAction)) != RT_ERR_OK)
				return retVal;

            *pMcast_action = (rtksw_trap_mcast_action_t)rawAction;
            break;
        case MCAST_IPV6:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_IPV6_PORT_MC_LM_ACTr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTf, &rawAction)) != RT_ERR_OK)
				return retVal;			

            *pMcast_action = (rtksw_trap_mcast_action_t)rawAction;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_lldpEnable_set
 * Description:
 *      Set LLDP enable.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_lldpEnable_set(rtksw_uint32 unit, rtksw_enable_t enabled)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (enabled >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    if ((retVal = reg16_field_write(unit, RTL8371C_RMA_LLDP_ENr, RTL8371C_ENABLEf, &enabled)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_lldpEnable_get
 * Description:
 *      Get LLDP status.
 *  Input:
 *      unit        - Unit ID
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
rtksw_api_ret_t dal_rtl8371c_trap_lldpEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled)
{
    rtksw_api_ret_t retVal;
	
    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pEnabled)
        return RT_ERR_NULL_POINTER;
	
    if ((retVal = reg16_field_read(unit, RTL8371C_RMA_LLDP_ENr, RTL8371C_ENABLEf, pEnabled)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_reasonTrapToCpuPriority_set
 * Description:
 *      Set priority value of a packet that trapped to CPU port according to specific reason.
 *  Input:
 *      unit        - Unit ID
 *      type        - reason that trap to CPU port.
 *      priority    - internal priority that is going to be set for specific trap reason.
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
 *      - TRAP_REASON_1XUNAUTH
 *      - TRAP_REASON_VLANSTACK
 *      - TRAP_REASON_UNKNOWNMC
 *      - TRAP_REASON_IGMPMLD
 */
rtksw_api_ret_t dal_rtl8371c_trap_reasonTrapToCpuPriority_set(rtksw_uint32 unit, rtksw_trap_reason_type_t type, rtksw_pri_t priority)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= TRAP_REASON_END)
        return RT_ERR_INPUT;

    if (type == TRAP_REASON_OAM)
        return RT_ERR_INPUT;

    if (priority > RTL8371C_TRAP_PRIMAX)
        return  RT_ERR_QOS_INT_PRIORITY;

    switch (type)
    {
        case TRAP_REASON_RMA:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY00r, RTL8371C_TRAP_PRIORITYf, &priority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_1XUNAUTH:
            if ((retVal = reg16_field_write(unit, RTL8371C_PRI_TRAPr, RTL8371C_DOT1X_PRIORTYf, &priority)) != RT_ERR_OK)
                return retVal;			
            break;
        case TRAP_REASON_VLANSTACK:
            if ((retVal = reg16_field_write(unit, RTL8371C_PRI_TRAPr, RTL8371C_SVLAN_PRIOIRTYf, &priority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_UNKNOWNMC:
            if ((retVal = reg16_field_write(unit, RTL8371C_PRI_TRAPr, RTL8371C_UNKNOWN_MC_PRIf, &priority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_IGMPMLD:
            if ((retVal = reg16_field_write(unit, RTL8371C_PRI_TRAP2r, RTL8371C_IGMPMLD_PRIORTYf, &priority)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_reasonTrapToCpuPriority_get
 * Description:
 *      Get priority value of a packet that trapped to CPU port according to specific reason.
 *  Input:
 *      unit        - Unit ID
 *      type        - reason that trap to CPU port.
 * Output:
 *      pPriority - configured internal priority for such reason.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      Currently the trap reason that supported are listed as follows:
 *      - TRAP_REASON_RMA
 *      - TRAP_REASON_1XUNAUTH
 *      - TRAP_REASON_VLANSTACK
 *      - TRAP_REASON_UNKNOWNMC
 *      - TRAP_REASON_IGMPMLD
 */
rtksw_api_ret_t dal_rtl8371c_trap_reasonTrapToCpuPriority_get(rtksw_uint32 unit, rtksw_trap_reason_type_t type, rtksw_pri_t *pPriority)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= TRAP_REASON_END)
        return RT_ERR_INPUT;

    if (type == TRAP_REASON_OAM)
        return RT_ERR_INPUT;

    if(NULL == pPriority)
        return RT_ERR_NULL_POINTER;

    switch (type)
    {
        case TRAP_REASON_RMA:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY00r, RTL8371C_TRAP_PRIORITYf, pPriority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_1XUNAUTH:
            if ((retVal = reg16_field_read(unit, RTL8371C_PRI_TRAPr, RTL8371C_DOT1X_PRIORTYf, pPriority)) != RT_ERR_OK)
                return retVal;			
            break;
        case TRAP_REASON_VLANSTACK:
            if ((retVal = reg16_field_read(unit, RTL8371C_PRI_TRAPr, RTL8371C_SVLAN_PRIOIRTYf, pPriority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_UNKNOWNMC:
            if ((retVal = reg16_field_read(unit, RTL8371C_PRI_TRAPr, RTL8371C_UNKNOWN_MC_PRIf, pPriority)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_REASON_IGMPMLD:
            if ((retVal = reg16_field_read(unit, RTL8371C_PRI_TRAP2r, RTL8371C_IGMPMLD_PRIORTYf, pPriority)) != RT_ERR_OK)
                return retVal;
            break;
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_rmaAction_set
 * Description:
 *      Set Reserved multicast address action configuration.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_rmaAction_set(rtksw_uint32 unit, rtksw_trap_type_t type, rtksw_trap_rma_action_t rma_action)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= TRAP_END)
        return RT_ERR_INPUT;

    if (rma_action >= RMA_ACTION_END)
        return RT_ERR_RMA_ACTION;

    switch (type)
    {
        case TRAP_BRG_GROUP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY00r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_FD_PAUSE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY01r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_SP_MCAST:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY02r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_1X_PAE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY03r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_04:
        case TRAP_UNDEF_BRG_05:
        case TRAP_UNDEF_BRG_06:
        case TRAP_UNDEF_BRG_07:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY08r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_09:
        case TRAP_UNDEF_BRG_0A:
        case TRAP_UNDEF_BRG_0B:
        case TRAP_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;            
        case TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_8021AB:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Er, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_0F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_BRG_MNGEMENT:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY10r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_11: 
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY11r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_12:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY12r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_13:
        case TRAP_UNDEFINED_14:
        case TRAP_UNDEFINED_15:
        case TRAP_UNDEFINED_16:
        case TRAP_UNDEFINED_17:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_18:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY18r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_19:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_1A:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_1B:
        case TRAP_UNDEFINED_1C:
        case TRAP_UNDEFINED_1D:
        case TRAP_UNDEFINED_1E:
        case TRAP_UNDEFINED_1F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_GMRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY20r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_GVRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY21r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_GARP_22:
        case TRAP_UNDEF_GARP_23:
        case TRAP_UNDEF_GARP_24:
        case TRAP_UNDEF_GARP_25:
        case TRAP_UNDEF_GARP_26:
        case TRAP_UNDEF_GARP_27:
        case TRAP_UNDEF_GARP_28:
        case TRAP_UNDEF_GARP_29:
        case TRAP_UNDEF_GARP_2A:
        case TRAP_UNDEF_GARP_2B:
        case TRAP_UNDEF_GARP_2C:
        case TRAP_UNDEF_GARP_2D:
        case TRAP_UNDEF_GARP_2E:
        case TRAP_UNDEF_GARP_2F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY22r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_CDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY30r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_CSSTP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY31r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_LLDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY32r, RTL8371C_OPERATIONf, &rma_action)) != RT_ERR_OK)
                return retVal;
            break;         
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_rmaAction_get
 * Description:
 *      Get Reserved multicast address action configuration.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_rmaAction_get(rtksw_uint32 unit, rtksw_trap_type_t type, rtksw_trap_rma_action_t *pRma_action)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= TRAP_END)
        return RT_ERR_INPUT;

    if(NULL == pRma_action)
        return RT_ERR_NULL_POINTER;

    switch (type)
    {
        case TRAP_BRG_GROUP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY00r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_FD_PAUSE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY01r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_SP_MCAST:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY02r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_1X_PAE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY03r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_04:
        case TRAP_UNDEF_BRG_05:
        case TRAP_UNDEF_BRG_06:
        case TRAP_UNDEF_BRG_07:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY08r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_09:
        case TRAP_UNDEF_BRG_0A:
        case TRAP_UNDEF_BRG_0B:
        case TRAP_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;            
        case TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_8021AB:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Er, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_0F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_BRG_MNGEMENT:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY10r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_11: 
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY11r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_12:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY12r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_13:
        case TRAP_UNDEFINED_14:
        case TRAP_UNDEFINED_15:
        case TRAP_UNDEFINED_16:
        case TRAP_UNDEFINED_17:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_18:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY18r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_19:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_1A:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_1B:
        case TRAP_UNDEFINED_1C:
        case TRAP_UNDEFINED_1D:
        case TRAP_UNDEFINED_1E:
        case TRAP_UNDEFINED_1F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_GMRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY20r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_GVRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY21r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_GARP_22:
        case TRAP_UNDEF_GARP_23:
        case TRAP_UNDEF_GARP_24:
        case TRAP_UNDEF_GARP_25:
        case TRAP_UNDEF_GARP_26:
        case TRAP_UNDEF_GARP_27:
        case TRAP_UNDEF_GARP_28:
        case TRAP_UNDEF_GARP_29:
        case TRAP_UNDEF_GARP_2A:
        case TRAP_UNDEF_GARP_2B:
        case TRAP_UNDEF_GARP_2C:
        case TRAP_UNDEF_GARP_2D:
        case TRAP_UNDEF_GARP_2E:
        case TRAP_UNDEF_GARP_2F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY22r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_CDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY30r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_CSSTP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY31r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_LLDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY32r, RTL8371C_OPERATIONf, pRma_action)) != RT_ERR_OK)
                return retVal;
            break;         
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_rmaKeepFormat_set
 * Description:
 *      Set Reserved multicast address keep format configuration.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_rmaKeepFormat_set(rtksw_uint32 unit, rtksw_trap_type_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= TRAP_END)
        return RT_ERR_INPUT;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    switch (type)
    {
        case TRAP_BRG_GROUP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY00r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_FD_PAUSE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY01r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_SP_MCAST:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY02r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_1X_PAE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY03r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_04:
        case TRAP_UNDEF_BRG_05:
        case TRAP_UNDEF_BRG_06:
        case TRAP_UNDEF_BRG_07:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY08r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_09:
        case TRAP_UNDEF_BRG_0A:
        case TRAP_UNDEF_BRG_0B:
        case TRAP_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;            
        case TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_8021AB:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Er, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_0F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_BRG_MNGEMENT:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY10r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_11: 
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY11r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_12:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY12r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_13:
        case TRAP_UNDEFINED_14:
        case TRAP_UNDEFINED_15:
        case TRAP_UNDEFINED_16:
        case TRAP_UNDEFINED_17:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_18:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY18r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_19:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_1A:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_1B:
        case TRAP_UNDEFINED_1C:
        case TRAP_UNDEFINED_1D:
        case TRAP_UNDEFINED_1E:
        case TRAP_UNDEFINED_1F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_GMRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY20r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_GVRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY21r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_GARP_22:
        case TRAP_UNDEF_GARP_23:
        case TRAP_UNDEF_GARP_24:
        case TRAP_UNDEF_GARP_25:
        case TRAP_UNDEF_GARP_26:
        case TRAP_UNDEF_GARP_27:
        case TRAP_UNDEF_GARP_28:
        case TRAP_UNDEF_GARP_29:
        case TRAP_UNDEF_GARP_2A:
        case TRAP_UNDEF_GARP_2B:
        case TRAP_UNDEF_GARP_2C:
        case TRAP_UNDEF_GARP_2D:
        case TRAP_UNDEF_GARP_2E:
        case TRAP_UNDEF_GARP_2F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY22r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_CDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY30r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_CSSTP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY31r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_LLDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY32r, RTL8371C_KEEP_FORMATf, &enable)) != RT_ERR_OK)
                return retVal;
            break;         
        default:
            break;
    }
	
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_rmaKeepFormat_get
 * Description:
 *      Get Reserved multicast address action configuration.
 *  Input:
 *      unit        - Unit ID
 *      type        - rma type.
 * Output:
 *      pEnable - keep format status.
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
rtksw_api_ret_t dal_rtl8371c_trap_rmaKeepFormat_get(rtksw_uint32 unit, rtksw_trap_type_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= TRAP_END)
        return RT_ERR_INPUT;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    switch (type)
    {
        case TRAP_BRG_GROUP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY00r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_FD_PAUSE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY01r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_SP_MCAST:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY02r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_1X_PAE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY03r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_04:
        case TRAP_UNDEF_BRG_05:
        case TRAP_UNDEF_BRG_06:
        case TRAP_UNDEF_BRG_07:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY08r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_09:
        case TRAP_UNDEF_BRG_0A:
        case TRAP_UNDEF_BRG_0B:
        case TRAP_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;            
        case TRAP_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_8021AB:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Er, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_BRG_0F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_BRG_MNGEMENT:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY10r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_11: 
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY11r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_12:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY12r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_13:
        case TRAP_UNDEFINED_14:
        case TRAP_UNDEFINED_15:
        case TRAP_UNDEFINED_16:
        case TRAP_UNDEFINED_17:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_18:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY18r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_19:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_1A:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEFINED_1B:
        case TRAP_UNDEFINED_1C:
        case TRAP_UNDEFINED_1D:
        case TRAP_UNDEFINED_1E:
        case TRAP_UNDEFINED_1F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_GMRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY20r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_GVRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY21r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_UNDEF_GARP_22:
        case TRAP_UNDEF_GARP_23:
        case TRAP_UNDEF_GARP_24:
        case TRAP_UNDEF_GARP_25:
        case TRAP_UNDEF_GARP_26:
        case TRAP_UNDEF_GARP_27:
        case TRAP_UNDEF_GARP_28:
        case TRAP_UNDEF_GARP_29:
        case TRAP_UNDEF_GARP_2A:
        case TRAP_UNDEF_GARP_2B:
        case TRAP_UNDEF_GARP_2C:
        case TRAP_UNDEF_GARP_2D:
        case TRAP_UNDEF_GARP_2E:
        case TRAP_UNDEF_GARP_2F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY22r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_CDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY30r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_CSSTP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY31r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case TRAP_LLDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY32r, RTL8371C_KEEP_FORMATf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;         
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_portUnknownMacPktAction_set
 * Description:
 *      Set unknown source MAC packet action configuration.
 *  Input:
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
 *      This API can set unknown source MAC action configuration.
 *      The unknown unicast action is as following:
 *          - UCAST_ACTION_FORWARD_PMASK
 *          - UCAST_ACTION_DROP
 *          - UCAST_ACTION_TRAP2CPU
 */
rtksw_api_ret_t dal_rtl8371c_trap_portUnknownMacPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t ucast_action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (ucast_action >= UCAST_ACTION_FLOODING)
        return RT_ERR_INPUT;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
    {
        return RT_ERR_PORT_ID;
    }

	if ((retVal = reg16_array_field_write(unit, RTL8371C_L2_NEWSA_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, &ucast_action)) != RT_ERR_OK)
		return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_portUnknownMacPktAction_get
 * Description:
 *      Get unknown source MAC packet action configuration.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trap_portUnknownMacPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t *pUcast_action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pUcast_action)
        return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_L2_NEWSA_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, pUcast_action)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_portUnmatchMacPktAction_set
 * Description:
 *      Set unmatch source MAC packet action configuration.
 *  Input:
 *      unit            - Unit ID
 *      port            - Port ID.
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
rtksw_api_ret_t dal_rtl8371c_trap_portUnmatchMacPktAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t ucast_action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (ucast_action >= UCAST_ACTION_FLOODING)
        return RT_ERR_INPUT;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;
	
    if ((retVal = reg16_array_field_write(unit, RTL8371C_L2_UNMATCH_SA_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, &ucast_action)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trap_portUnmatchMacPktAction_get
 * Description:
 *      Get unmatch source MAC packet action configuration.
 *  Input:
 *      unit            - Unit ID
 *      port            - Port ID.
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
rtksw_api_ret_t dal_rtl8371c_trap_portUnmatchMacPktAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_trap_ucast_action_t *pUcast_action)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* check port valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if(NULL == pUcast_action)
        return RT_ERR_NULL_POINTER;

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_L2_UNMATCH_SA_CTRLr, phyPort, REG_ARRAY_INDEX_NONE, RTL8371C_ACTIONf, pUcast_action)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


