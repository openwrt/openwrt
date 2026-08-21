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
#include <dal/rtl8371c/dal_rtl8371c_rldp.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_rldp_config_set
 * Description:
 *      Set RLDP module configuration
 *  Input:
 *      unit        - Unit ID
 *      pConfig     - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_rldp_config_set(rtksw_uint32 unit, rtksw_rldp_config_t *pConfig)
{
    rtksw_api_ret_t retVal;
    rtksw_uint16 *magic;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (pConfig->rldp_enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    if (pConfig->trigger_mode >= RTKSW_RLDP_TRIGGER_END)
        return RT_ERR_INPUT;

    if (pConfig->compare_type >= RTKSW_RLDP_CMPTYPE_END)
        return RT_ERR_INPUT;

    if (pConfig->num_check >= RTKSW_RLDP_NUM_MAX)
        return RT_ERR_INPUT;

    if (pConfig->interval_check >= RTKSW_RLDP_INTERVAL_MAX)
        return RT_ERR_INPUT;

    if (pConfig->num_loop >= RTKSW_RLDP_NUM_MAX)
        return RT_ERR_INPUT;

    if (pConfig->interval_loop >= RTKSW_RLDP_INTERVAL_MAX)
        return RT_ERR_INPUT;

    regData = pConfig->rldp_enable ? 0 : 1;
    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_REG0r, RTL8371C_RLDP_DISABLEf, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_REG0r, RTL8371C_RLDP_TRIGGER_MODEf, (rtksw_uint32 *)&(pConfig->trigger_mode))) != RT_ERR_OK)
        return retVal;

    magic = (rtksw_uint16*)&pConfig->magic;
    regData = *magic;
    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_SEED_0r, RTL8371C_RLDP_MAGIC_NUMBER_0f, &regData)) != RT_ERR_OK)
        return retVal;
    magic++;
    regData = *magic;
    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_SEED_1r, RTL8371C_RLDP_MAGIC_NUMBER_1f, &regData)) != RT_ERR_OK)
        return retVal;
    magic++;
    regData = *magic;
    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_SEED_2r, RTL8371C_RLDP_MAGIC_NUMBER_2f, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_REG0r, RTL8371C_RLDP_COMP_IDf, (rtksw_uint32 *)&(pConfig->compare_type))) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_REG1r, RTL8371C_RLDP_RETRY_COUNT_CHKSTATEf, &(pConfig->num_check))) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_REG3r, RTL8371C_RLDP_RETRY_PERIOD_CHKSTATEf, &(pConfig->interval_check))) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_REG1r, RTL8371C_RLDP_RETRY_COUNT_LOOPSTATEf, &(pConfig->num_loop))) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_REG2r, RTL8371C_RLDP_RETRY_PERIOD_LOOPSTATEf, &(pConfig->interval_loop))) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rldp_config_get
 * Description:
 *      Get RLDP module configuration
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pConfig - configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_rldp_config_get(rtksw_uint32 unit, rtksw_rldp_config_t *pConfig)
{
    rtksw_api_ret_t retVal;
    rtksw_uint16 *magic;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_REG0r, RTL8371C_RLDP_DISABLEf, &regData)) != RT_ERR_OK)
        return retVal;

    pConfig->rldp_enable = regData ? 0 : 1;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_REG0r, RTL8371C_RLDP_TRIGGER_MODEf, (rtksw_uint32 *)&(pConfig->trigger_mode))) != RT_ERR_OK)
        return retVal;

    magic = (rtksw_uint16*)&pConfig->magic;
    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_SEED_0r, RTL8371C_RLDP_MAGIC_NUMBER_0f, &regData)) != RT_ERR_OK)
        return retVal;
    *magic = regData;
    magic++;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_SEED_1r, RTL8371C_RLDP_MAGIC_NUMBER_1f, &regData)) != RT_ERR_OK)
        return retVal;
    *magic = regData;
    magic++;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_SEED_2r, RTL8371C_RLDP_MAGIC_NUMBER_2f, &regData)) != RT_ERR_OK)
        return retVal;
    *magic = regData;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_REG0r, RTL8371C_RLDP_COMP_IDf, (rtksw_uint32 *)&(pConfig->compare_type))) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_REG1r, RTL8371C_RLDP_RETRY_COUNT_CHKSTATEf, &(pConfig->num_check))) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_REG3r, RTL8371C_RLDP_RETRY_PERIOD_CHKSTATEf, &(pConfig->interval_check))) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_REG1r, RTL8371C_RLDP_RETRY_COUNT_LOOPSTATEf, &(pConfig->num_loop))) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_REG2r, RTL8371C_RLDP_RETRY_PERIOD_LOOPSTATEf, &(pConfig->interval_loop))) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_rldp_portConfig_set
 * Description:
 *      Set per port RLDP module configuration
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be configured
 *      pPortConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_rldp_portConfig_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portConfig_t *pPortConfig)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 portmask;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (pPortConfig->tx_enable>= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    phy_port = rtksw_switch_port_L2P_get(unit, port);
    if (phy_port == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;


    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_REG4r, RTL8371C_RLDP_TX_PMSKf, &portmask)) != RT_ERR_OK)
        return retVal;

    if (pPortConfig->tx_enable)
    {
         portmask |=(1<<phy_port);
    }
    else
    {
         portmask &= ~(1<<phy_port);
    }

    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_REG4r, RTL8371C_RLDP_TX_PMSKf, &portmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;

}

/* Function Name:
 *      dal_rtl8371c_rldp_portConfig_get
 * Description:
 *      Get per port RLDP module configuration
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be get
 * Output:
 *      pPortConfig - per port configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_rldp_portConfig_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portConfig_t *pPortConfig)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 portmask;
    rtksw_portmask_t logicalPmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_REG4r, RTL8371C_RLDP_TX_PMSKf, &portmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, portmask, &logicalPmask)) != RT_ERR_OK)
        return retVal;

    if (logicalPmask.bits[0] & (1<<port))
    {
         pPortConfig->tx_enable = RTKSW_ENABLED;
    }
    else
    {
         pPortConfig->tx_enable = RTKSW_DISABLED;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_rtl8371c_rldp_status_get
 * Description:
 *      Get RLDP module status
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pStatus - status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_rldp_status_get(rtksw_uint32 unit, rtksw_rldp_status_t *pStatus)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;
    rtksw_uint16 *accessPtr;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    accessPtr = (rtksw_uint16*)&pStatus->id;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_RAND_0r, RTL8371C_RLDP_RANDOM_NUMBER_0f, &regData)) != RT_ERR_OK)
        return retVal;
    *accessPtr = regData;
    accessPtr++;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_RAND_1r, RTL8371C_RLDP_RANDOM_NUMBER_1f, &regData)) != RT_ERR_OK)
        return retVal;
    *accessPtr = regData;
    accessPtr++;

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_RAND_2r, RTL8371C_RLDP_RANDOM_NUMBER_2f, &regData)) != RT_ERR_OK)
        return retVal;
    *accessPtr = regData;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rldp_portStatus_get
 * Description:
 *      Get RLDP module status
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be get
 * Output:
 *      pPortStatus - per port status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_rldp_portStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portStatus_t *pPortStatus)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 portmask;
    rtksw_portmask_t logicalPmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_LOOP_STS2r, RTL8371C_RLDP_LOOPSTATUS_INDICATORf, &portmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, portmask, &logicalPmask)) != RT_ERR_OK)
        return retVal;

    if (logicalPmask.bits[0] & (1<<port))
    {
         pPortStatus->loop_status = RTKSW_RLDP_LOOPSTS_LOOPING;
    }
    else
    {
         pPortStatus->loop_status  = RTKSW_RLDP_LOOPSTS_NONE;
    }

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_LOOP_STS0r, RTL8371C_RLDP_LOOPED_INDICATORf, &portmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, portmask, &logicalPmask)) != RT_ERR_OK)
        return retVal;

    if (logicalPmask.bits[0] & (1<<port))
    {
         pPortStatus->loop_enter = RTKSW_RLDP_LOOPSTS_LOOPING;
    }
    else
    {
         pPortStatus->loop_enter  = RTKSW_RLDP_LOOPSTS_NONE;
    }

    if ((retVal = reg16_field_read(unit, RTL8371C_RLDP_LOOP_STS1r, RTL8371C_RLDP_RELEASED_INDICATORf, &portmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, portmask, &logicalPmask)) != RT_ERR_OK)
        return retVal;

    if (logicalPmask.bits[0] & (1<<port))
    {
         pPortStatus->loop_leave = RTKSW_RLDP_LOOPSTS_LOOPING;
    }
    else
    {
         pPortStatus->loop_leave  = RTKSW_RLDP_LOOPSTS_NONE;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_rldp_portStatus_clear
 * Description:
 *      Clear RLDP module status
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be clear
 *      pPortStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Clear operation effect loop_enter and loop_leave only, other field in
 *      the structure are don't care
 */
rtksw_api_ret_t dal_rtl8371c_rldp_portStatus_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rldp_portStatus_t *pPortStatus)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmsk;
    rtksw_uint32 phyPort;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    phyPort = rtksw_switch_port_L2P_get(unit, port);
    if (phyPort == UNDEFINE_PHY_PORT)
        return RT_ERR_PORT_ID;

    pmsk = (pPortStatus->loop_enter) << phyPort;
    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_LOOP_STS0r, RTL8371C_RLDP_LOOPED_INDICATORf, &pmsk)) != RT_ERR_OK)
        return retVal;

    pmsk = (pPortStatus->loop_leave) << phyPort;
    if ((retVal = reg16_field_write(unit, RTL8371C_RLDP_LOOP_STS1r, RTL8371C_RLDP_RELEASED_INDICATORf, &pmsk)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl8371c_rldp_portLoopPair_get
 * Description:
 *      Get RLDP port loop pairs
 *  Input:
 *      unit        - Unit ID
 *      port        - port number to be get
 * Output:
 *      pPortmask - per port related loop ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_rldp_portLoopPair_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmsk;
    rtksw_uint32 loopedPair;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if ((retVal = reg16_array_field_read(unit, RTL8371C_RLDP_LOOP_PORTr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_RLDP_LOOP_PORT_01f, &loopedPair)) != RT_ERR_OK)
        return retVal;

    pmsk = 1 << loopedPair;
    if ((retVal = rtksw_switch_portmask_P2L_get(unit, pmsk, pPortmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

