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

#include <dal/dal_mgmts.h>

static rtksw_init_state_t    init_state[DAL_MGMT_MAX_UNIT] = {RTKSW_INIT_NOT_COMPLETED};

#if defined(RTK_X86_CLE)
pthread_mutex_t api_mutex = PTHREAD_MUTEX_INITIALIZER;
#else
DEFINE_MUTEX(rtksw_api_mutex);
#endif

rtksw_switch_halCtrl_t *halCtrl[DAL_MGMT_MAX_UNIT] = {NULL};


static rtksw_api_ret_t _rtksw_switch_init(rtksw_uint32 unit)
{
    rtksw_int32  retVal;
    switch_chip_t   switchChip;

    RTKSW_CHK_UNIT_ID(unit);

    /* Find device */
    if((halCtrl[unit] = rtksw_hal_find_device(unit)) == NULL)
        return RT_ERR_CHIP_NOT_FOUND;

    /* Attached DAL mapper */
    switchChip = halCtrl[unit]->switch_type;
    if((retVal = dal_mgmt_attachDevice(unit, switchChip)) != RT_ERR_OK)
        return retVal;

    /* Set initial state */
    if((retVal = rtksw_switch_initialState_set(unit, RTKSW_INIT_COMPLETED)) != RT_ERR_OK)
        return retVal;
    
    /* Call initial function */
    if((retVal = RT_MAPPER->switch_init(unit)) != RT_ERR_OK)
        return retVal;
    
    return RT_ERR_OK;
}

/* Function Name:
 *      rtksw_switch_initialState_set
 * Description:
 *      Set initial status
 * Input:
 *      unit    - Unit ID
 *      state   - Initial state;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Initialized
 *      RT_ERR_FAILED   - Uninitialized
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_initialState_set(rtksw_uint32 unit, rtksw_init_state_t state)
{
    if(state >= RTKSW_INIT_STATE_END)
        return RT_ERR_FAILED;

    init_state[unit] = state;
    return RT_ERR_OK;
}

/* Function Name:
 *      rtksw_switch_initialState_get
 * Description:
 *      Get initial status
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RTKSW_INIT_COMPLETED     - Initialized
 *      RTKSW_INIT_NOT_COMPLETED - Uninitialized
 * Note:
 *
 */
rtksw_init_state_t rtksw_switch_initialState_get(rtksw_uint32 unit)
{
    return init_state[unit];
}

/* Function Name:
 *      rtksw_switch_logicalPortCheck
 * Description:
 *      Check logical port ID.
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is correct
 *      RT_ERR_FAILED   - Port ID is not correct
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_logicalPortCheck(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if(halCtrl[unit]->l2p_port[logicalPort] == 0xFF)
        return RT_ERR_FAILED;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtksw_switch_isUtpPort
 * Description:
 *      Check is logical port a UTP port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a UTP port
 *      RT_ERR_FAILED   - Port ID is not a UTP port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isUtpPort(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if(halCtrl[unit]->log_port_type[logicalPort] == UTP_PORT)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      rtksw_switch_isExtPort
 * Description:
 *      Check is logical port a Extension port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a EXT port
 *      RT_ERR_FAILED   - Port ID is not a EXT port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isExtPort(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if(halCtrl[unit]->log_port_type[logicalPort] == EXT_PORT)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      rtksw_switch_isUsxgPort
 * Description:
 *      Check is logical port a USXG port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a USXG port
 *      RT_ERR_FAILED   - Port ID is not a USXG port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isUsxgPort(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if( ((0x01 << logicalPort) & halCtrl[unit]->usxg_logical_portmask) != 0)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      rtksw_switch_isHsgPort
 * Description:
 *      Check is logical port a HSG port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a HSG port
 *      RT_ERR_FAILED   - Port ID is not a HSG port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isHsgPort(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if( ((0x01 << logicalPort) & halCtrl[unit]->hsg_logical_portmask) != 0)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      rtksw_switch_isSgmiiPort
 * Description:
 *      Check is logical port a SGMII port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a SGMII port
 *      RT_ERR_FAILED   - Port ID is not a SGMII port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isSgmiiPort(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if( ((0x01 << logicalPort) & halCtrl[unit]->sg_logical_portmask) != 0)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      rtksw_switch_isUtp2p5gPort
 * Description:
 *      Check is logical port a 2.5G UTP port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a 2.5G UTP port
 *      RT_ERR_FAILED   - Port ID is not a 2.5G UTP port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isUtp2p5gPort(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if( ((0x01 << logicalPort) & halCtrl[unit]->utp_2p5g_logical_portmask) != 0)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      rtksw_switch_isCPUPort
 * Description:
 *      Check is logical port a CPU port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a CPU port
 *      RT_ERR_FAILED   - Port ID is not a CPU port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isCPUPort(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if( ((0x01 << logicalPort) & halCtrl[unit]->valid_cpu_portmask) != 0)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      rtksw_switch_isComboPort
 * Description:
 *      Check is logical port a Combo port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a combo port
 *      RT_ERR_FAILED   - Port ID is not a combo port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isComboPort(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if(halCtrl[unit]->combo_logical_port == logicalPort)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      rtksw_switch_ComboPort_get
 * Description:
 *      Get Combo port ID
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Port ID of combo port
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_ComboPort_get(rtksw_uint32 unit)
{
    return halCtrl[unit]->combo_logical_port;
}

/* Function Name:
 *      rtksw_switch_isPtpPort
 * Description:
 *      Check is logical port a PTP port
 * Input:
 *      logicalPort     - logical port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Port ID is a PTP port
 *      RT_ERR_FAILED   - Port ID is not a PTP port
 *      RT_ERR_NOT_INIT - Not Initialize
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isPtpPort(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return RT_ERR_FAILED;

    if(halCtrl[unit]->ptp_port[logicalPort] == 1)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
}

/* Function Name:
 *      rtksw_switch_port_L2P_get
 * Description:
 *      Get physical port ID
 * Input:
 *      logicalPort       - logical port ID
 * Output:
 *      None
 * Return:
 *      Physical port ID
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_port_L2P_get(rtksw_uint32 unit, rtksw_port_t logicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return UNDEFINE_PHY_PORT;

    if(logicalPort >= RTKSW_SWITCH_PORT_NUM)
        return UNDEFINE_PHY_PORT;

    return (halCtrl[unit]->l2p_port[logicalPort]);
}

/* Function Name:
 *      rtksw_switch_port_P2L_get
 * Description:
 *      Get logical port ID
 * Input:
 *      physicalPort       - physical port ID
 * Output:
 *      None
 * Return:
 *      logical port ID
 * Note:
 *
 */
rtksw_port_t rtksw_switch_port_P2L_get(rtksw_uint32 unit, rtksw_uint32 physicalPort)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return UNDEFINE_PORT;

    if(physicalPort >= RTKSW_SWITCH_PORT_NUM)
        return UNDEFINE_PORT;

    return (halCtrl[unit]->p2l_port[physicalPort]);
}

/* Function Name:
 *      rtksw_switch_isPortMaskValid
 * Description:
 *      Check portmask is valid or not
 * Input:
 *      pPmask       - logical port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - port mask is valid
 *      RT_ERR_FAILED       - port mask is not valid
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isPortMaskValid(rtksw_uint32 unit, rtksw_portmask_t *pPmask)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(NULL == pPmask)
        return RT_ERR_NULL_POINTER;

    if( (pPmask->bits[0] | halCtrl[unit]->valid_portmask) != halCtrl[unit]->valid_portmask )
        return RT_ERR_FAILED;
    else
        return RT_ERR_OK;
}

/* Function Name:
 *      rtksw_switch_isPortMaskUtp
 * Description:
 *      Check all ports in portmask are only UTP port
 * Input:
 *      pPmask       - logical port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Only UTP port in port mask
 *      RT_ERR_FAILED       - Not only UTP port in port mask
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isPortMaskUtp(rtksw_uint32 unit, rtksw_portmask_t *pPmask)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(NULL == pPmask)
        return RT_ERR_NULL_POINTER;

    if( (pPmask->bits[0] | halCtrl[unit]->valid_utp_portmask) != halCtrl[unit]->valid_utp_portmask )
        return RT_ERR_FAILED;
    else
        return RT_ERR_OK;
}

/* Function Name:
 *      rtksw_switch_isPortMaskExt
 * Description:
 *      Check all ports in portmask are only EXT port
 * Input:
 *      pPmask       - logical port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Only EXT port in port mask
 *      RT_ERR_FAILED       - Not only EXT port in port mask
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_isPortMaskExt(rtksw_uint32 unit, rtksw_portmask_t *pPmask)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(NULL == pPmask)
        return RT_ERR_NULL_POINTER;

    if( (pPmask->bits[0] | halCtrl[unit]->valid_ext_portmask) != halCtrl[unit]->valid_ext_portmask )
        return RT_ERR_FAILED;
    else
        return RT_ERR_OK;
}

/* Function Name:
 *      rtksw_switch_portmask_L2P_get
 * Description:
 *      Get physicl portmask from logical portmask
 * Input:
 *      pLogicalPmask       - logical port mask
 * Output:
 *      pPhysicalPortmask   - physical port mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 *      RT_ERR_PORT_MASK    - Error port mask
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_portmask_L2P_get(rtksw_uint32 unit, rtksw_portmask_t *pLogicalPmask, rtksw_uint32 *pPhysicalPortmask)
{
    rtksw_uint32 log_port, phyPort;

    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(NULL == pLogicalPmask)
        return RT_ERR_NULL_POINTER;

    if(NULL == pPhysicalPortmask)
        return RT_ERR_NULL_POINTER;

    if(rtksw_switch_isPortMaskValid(unit, pLogicalPmask) != RT_ERR_OK)
        return RT_ERR_PORT_MASK;

    /* reset physical port mask */
    *pPhysicalPortmask = 0;

    RTKSW_PORTMASK_SCAN((*pLogicalPmask), log_port)
    {
        phyPort = rtksw_switch_port_L2P_get(unit, (rtksw_port_t)log_port);
        if (phyPort == UNDEFINE_PHY_PORT)
            return RT_ERR_PORT_ID;

        *pPhysicalPortmask |= (0x0001 << phyPort);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtksw_switch_portmask_P2L_get
 * Description:
 *      Get logical portmask from physical portmask
 * Input:
 *      physicalPortmask    - physical port mask
 * Output:
 *      pLogicalPmask       - logical port mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 *      RT_ERR_PORT_MASK    - Error port mask
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_portmask_P2L_get(rtksw_uint32 unit, rtksw_uint32 physicalPortmask, rtksw_portmask_t *pLogicalPmask)
{
    rtksw_uint32 log_port, phy_port;

    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_NOT_INIT;

    if(NULL == pLogicalPmask)
        return RT_ERR_NULL_POINTER;

    RTKSW_PORTMASK_CLEAR(*pLogicalPmask);

    for(phy_port = halCtrl[unit]->min_phy_port; phy_port <= halCtrl[unit]->max_phy_port; phy_port++)
    {
        if(physicalPortmask & (0x0001 << phy_port))
        {
            log_port = rtksw_switch_port_P2L_get(unit, phy_port);
            if(log_port != UNDEFINE_PORT)
            {
                RTKSW_PORTMASK_PORT_SET(*pLogicalPmask, log_port);
            }
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtksw_switch_phyPortMask_get
 * Description:
 *      Get physical portmask
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      0x00                - Not Initialize
 *      Other value         - Physical port mask
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_phyPortMask_get(rtksw_uint32 unit)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return 0x00; /* No port in portmask */

    return (halCtrl[unit]->phy_portmask);
}

/* Function Name:
 *      rtksw_switch_logPortMask_get
 * Description:
 *      Get Logical portmask
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NOT_INIT     - Not Initialize
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *
 */
rtksw_api_ret_t rtksw_switch_logPortMask_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return RT_ERR_FAILED;

    if(NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    pPortmask->bits[0] = halCtrl[unit]->valid_portmask;
    return RT_ERR_OK;
}

/* Function Name:
 *      rtksw_switch_init
 * Description:
 *      Set chip to default configuration enviroment
 * Input:
 *      unit                - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can set chip registers to default configuration for different release chip model.
 */
rtksw_api_ret_t rtksw_switch_init(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);

    RTKSW_API_LOCK(unit);
    retVal = _rtksw_switch_init(unit);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_switch_portMaxPktLen_set
 * Description:
 *      Set Max packet length
 * Input:
 *      unit    - Unit ID
 *      port    - Port ID
 *      speed   - Speed
 *      cfgId   - Configuration ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
rtksw_api_ret_t rtksw_switch_portMaxPktLen_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_switch_maxPktLen_linkSpeed_t speed, rtksw_uint32 cfgId)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->switch_portMaxPktLen_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->switch_portMaxPktLen_set(unit, port, speed, cfgId);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_switch_portMaxPktLen_get
 * Description:
 *      Get Max packet length
 * Input:
 *      unit    - Unit ID
 *      port    - Port ID
 *      speed   - Speed
 * Output:
 *      pCfgId  - Configuration ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
rtksw_api_ret_t rtksw_switch_portMaxPktLen_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_switch_maxPktLen_linkSpeed_t speed, rtksw_uint32 *pCfgId)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->switch_portMaxPktLen_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->switch_portMaxPktLen_get(unit, port, speed, pCfgId);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_switch_maxPktLenCfg_set
 * Description:
 *      Set Max packet length configuration
 * Input:
 *      unit    - Unit ID
 *      cfgId   - Configuration ID
 *      pktLen  - Max packet length
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
rtksw_api_ret_t rtksw_switch_maxPktLenCfg_set(rtksw_uint32 unit, rtksw_uint32 cfgId, rtksw_uint32 pktLen)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->switch_maxPktLenCfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->switch_maxPktLenCfg_set(unit, cfgId, pktLen);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_switch_maxPktLenCfg_get
 * Description:
 *      Get Max packet length configuration
 * Input:
 *      unit    - Unit ID
 *      cfgId   - Configuration ID
 *      pPktLen - Max packet length
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 */
rtksw_api_ret_t rtksw_switch_maxPktLenCfg_get(rtksw_uint32 unit, rtksw_uint32 cfgId, rtksw_uint32 *pPktLen)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->switch_maxPktLenCfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->switch_maxPktLenCfg_get(unit, cfgId, pPktLen);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_switch_greenEthernet_set
 * Description:
 *      Set all Ports Green Ethernet state.
 * Input:
 *      unit    - Unit ID
 *      enable - Green Ethernet state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_SMI      - SMI access error
 *      RT_ERR_ENABLE   - Invalid enable input.
 * Note:
 *      This API can set all Ports Green Ethernet state.
 *      The configuration is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtksw_api_ret_t rtksw_switch_greenEthernet_set(rtksw_uint32 unit, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->switch_greenEthernet_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->switch_greenEthernet_set(unit, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_switch_greenEthernet_get
 * Description:
 *      Get all Ports Green Ethernet state.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - Green Ethernet state.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      This API can get Green Ethernet state.
 */
rtksw_api_ret_t rtksw_switch_greenEthernet_get(rtksw_uint32 unit, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->switch_greenEthernet_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->switch_greenEthernet_get(unit, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_switch_maxLogicalPort_get
 * Description:
 *      Get Max logical port ID
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Max logical port
 * Note:
 *      This API can get max logical port
 */
rtksw_port_t rtksw_switch_maxLogicalPort_get(rtksw_uint32 unit)
{
    rtksw_port_t port, maxLogicalPort = 0;

    /* Check initialization state */
    if(rtksw_switch_initialState_get(unit) != RTKSW_INIT_COMPLETED)
    {
        return UNDEFINE_PORT;
    }

    for(port = 0; port < RTKSW_SWITCH_PORT_NUM; port++)
    {
        if( (halCtrl[unit]->log_port_type[port] == UTP_PORT) || (halCtrl[unit]->log_port_type[port] == EXT_PORT) )
            maxLogicalPort = port;
    }

    return maxLogicalPort;
}

/* Function Name:
 *      rtksw_switch_maxMeterId_get
 * Description:
 *      Get Max Meter ID
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      0x00                - Not Initialize
 *      Other value         - Max Meter ID
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_maxMeterId_get(rtksw_uint32 unit)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return 0x00;

    return (halCtrl[unit]->max_meter_id);
}

/* Function Name:
 *      rtksw_switch_maxLutAddrNumber_get
 * Description:
 *      Get Max LUT Address number
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      0x00                - Not Initialize
 *      Other value         - Max LUT Address number
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_maxLutAddrNumber_get(rtksw_uint32 unit)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return 0x00;

    return (halCtrl[unit]->max_lut_addr_num);
}

/* Function Name:
 *      rtksw_switch_isValidTrunkGrpId
 * Description:
 *      Check if trunk group is valid or not
 * Input:
 *      grpId       - Group ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - Trunk Group ID is valid
 *      RT_ERR_LA_TRUNK_ID  - Trunk Group ID is not valid
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_isValidTrunkGrpId(rtksw_uint32 unit, rtksw_uint32 grpId)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return 0x00;

    if( (halCtrl[unit]->trunk_group_mask & (0x01 << grpId) ) != 0)
        return RT_ERR_OK;
    else
        return RT_ERR_LA_TRUNK_ID;

}

/* Function Name:
 *      rtksw_switch_maxBufferPageNum_get
 * Description:
 *      Get number of packet buffer page
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Number of packet buffer page
 * Note:
 *
 */
rtksw_uint32 rtksw_switch_maxBufferPageNum_get(rtksw_uint32 unit)
{
    if(init_state[unit] != RTKSW_INIT_COMPLETED)
        return 0x00;

    return (halCtrl[unit]->packet_buffer_page_num);
}

/* Function Name:
 *      rtksw_switch_chipType_get
 * Description:
 *      Get switch chip type
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      CHIP_END    - Unknown chip type
 *      other       - Switch chip type
 * Note:
 *
 */
switch_chip_t rtksw_switch_chipType_get(rtksw_uint32 unit)
{
    if (halCtrl[unit] == NULL)
        return CHIP_END;

    return halCtrl[unit]->switch_type;
}

/* Function Name:
 *      rtksw_switch_reset
 * Description:
 *      Reset chip.
 * Input:
 *      unit                - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      The API can reset entire chip.
 */
rtksw_api_ret_t rtksw_switch_reset(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->switch_reset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->switch_reset(unit);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}
