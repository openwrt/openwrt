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
#include <dal/rtl8371c/dal_rtl8371c_eee.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>
#include <dal/rtl8371c/dal_rtl8371c_port.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_eee_portEnable_set
 * Description:
 *      Set enable status of EEE function.
 * Input:
 *      unit    - Unit ID
 *      port    - port id.
 *      enable  - enable EEE status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 *      RT_ERR_ENABLE - Invalid enable input.
 * Note:
 *      This API can set EEE function to the specific port.
 *      The configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtksw_api_ret_t dal_rtl8371c_eee_portEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phyData;
    rtksw_uint32 regData;
    rtksw_uint32 fieldData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port is UTP port */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;
    
    if ((retVal = reg16_array_read(unit, RTL8371C_MAC_FORCE_MODE_CTRL0r, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, &regData)) != RT_ERR_OK)
        return retVal;

    fieldData = (enable == RTKSW_ENABLED) ? 1 : 0;
    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL0r, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_EEE_EN_100Mf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL0r, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_EEE_EN_500Mf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL0r, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_EEE_EN_1000Mf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL0r, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_EEE_2P5G_LITEf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_set(unit, RTL8371C_MAC_FORCE_MODE_CTRL0r, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_CFG_FORCE_EEE_EN_2P5Gf, &fieldData, &regData)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_write(unit, RTL8371C_MAC_FORCE_MODE_CTRL0r, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, &regData)) != RT_ERR_OK)
        return retVal;

    /* 0xA5D0[2:1] */
    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA5D0, &phyData)) != RT_ERR_OK)
        return retVal;

    if (enable == RTKSW_ENABLED)
        phyData |= (0x0003 << 1);
    else
        phyData &= ~(0x0003 << 1);

    if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xA5D0, phyData)) != RT_ERR_OK)
        return retVal;

    /* 0xA4A2[9] */
    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA4A2, &phyData)) != RT_ERR_OK)
        return retVal;

    if (enable == RTKSW_ENABLED)
        phyData |= (0x0001 << 9);
    else
        phyData &= ~(0x0001 << 9);

    if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xA4A2, phyData)) != RT_ERR_OK)
        return retVal;

    /* 0xA428[7] */
    if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA428, &phyData)) != RT_ERR_OK)
        return retVal;

    if (enable == RTKSW_ENABLED)
        phyData |= (0x0001 << 7);
    else
        phyData &= ~(0x0001 << 7);

    if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xA428, phyData)) != RT_ERR_OK)
        return retVal;

    if (rtksw_switch_isUtp2p5gPort(unit, port) == RT_ERR_OK)
    {
        /* 0xA6D4[0] */
        if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA6D4, &phyData)) != RT_ERR_OK)
            return retVal;

        if (enable == RTKSW_ENABLED)
            phyData |= 0x0001;
        else
            phyData &= ~0x0001;

        if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xA6D4, phyData)) != RT_ERR_OK)
            return retVal;

        /* 0xA6D8[4] */
        if ((retVal = dal_rtl8371c_port_phyOCPReg_get(unit, port, 0xA6D8, &phyData)) != RT_ERR_OK)
            return retVal;

        if (enable == RTKSW_ENABLED)
            phyData |= (0x0001 << 4);
        else
            phyData &= ~(0x0001 << 4);

        if ((retVal = dal_rtl8371c_port_phyOCPReg_set(unit, port, 0xA6D8, phyData)) != RT_ERR_OK)
            return retVal;
    }

    /* Restart auto-Nego*/
    if ((retVal = dal_rtl8371c_port_phyReg_get(unit, port, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
        return retVal;

    phyData = phyData | (1 << 9);
    if ((retVal = dal_rtl8371c_port_phyReg_set(unit, port, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_eee_portEnable_get
 * Description:
 *      Get enable status of EEE function
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable - Back pressure status.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 * Note:
 *      This API can get EEE function to the specific port.
 *      The configuration of the port is as following:
 *      - DISABLE
 *      - ENABLE
 */
rtksw_api_ret_t dal_rtl8371c_eee_portEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 regData;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check port is UTP port */
    RTKSW_CHK_PORT_IS_UTP(unit, port);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_read(unit, RTL8371C_MAC_FORCE_MODE_CTRL0r, rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, &regData)) != RT_ERR_OK)
        return retVal;

    *pEnable = (regData & 0x001f) ? RTKSW_ENABLED : RTKSW_DISABLED;
    return RT_ERR_OK;
}

