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
#include <dal/rtl8371c/dal_rtl8371c_leaky.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_leaky_vlan_set
 * Description:
 *      Set VLAN leaky.
 * Input:
 *      unit    - Unit ID
 *      type    - Packet type for VLAN leaky.
 *      enable  - Leaky status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      This API can set VLAN leaky for RMA ,IGMP/MLD, CDP, CSSTP, and LLDP packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 *      - LEAKY_CDP,
 *      - LEAKY_CSSTP,
 *      - LEAKY_LLDP.
 */
rtksw_api_ret_t dal_rtl8371c_leaky_vlan_set(rtksw_uint32 unit, rtksw_leaky_type_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= LEAKY_END)
        return RT_ERR_INPUT;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    switch (type)
    {
        case LEAKY_BRG_GROUP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY00r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_FD_PAUSE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY01r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_SP_MCAST:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY02r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_1X_PAE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY03r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY08r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;            
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_8021AB:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Er, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_0F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_BRG_MNGEMENT:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY10r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_11: 
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY11r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_12:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY12r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_18:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY18r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_19:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_1A:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_GMRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY20r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_GVRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY21r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY22r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_CDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY30r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_CSSTP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY31r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_LLDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY32r, RTL8371C_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_IGMP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_IGMP_CTRLr, RTL8371C_IGMP_MLD_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_IPMULTICAST:            
            RTKSW_SCAN_ALL_LOG_PORT(unit, port)
            {           
                if ((retVal = reg16_array_field_write(unit, RTL8371C_IPMUL_NO_VLAN_EGRESSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_IPMUL_VLAN_LEAKYf, &enable)) != RT_ERR_OK)
                    return retVal;
            }
            break;          
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_leaky_vlan_get
 * Description:
 *      Get VLAN leaky.
 * Input:
 *      unit    - Unit ID
 *      type    - Packet type for VLAN leaky.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get VLAN leaky status for RMA ,IGMP/MLD, CDP, CSSTP, and LLDP  packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 *      - LEAKY_CDP,
 *      - LEAKY_CSSTP,
 *      - LEAKY_LLDP.
 */
rtksw_api_ret_t dal_rtl8371c_leaky_vlan_get(rtksw_uint32 unit, rtksw_leaky_type_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 port,tmp;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= LEAKY_END)
        return RT_ERR_INPUT;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    switch (type)
    {
        case LEAKY_BRG_GROUP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY00r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_FD_PAUSE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY01r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_SP_MCAST:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY02r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_1X_PAE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY03r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY08r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;            
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_8021AB:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Er, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_0F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_VLAN_LEAKYf,pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_BRG_MNGEMENT:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY10r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_11: 
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY11r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_12:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY12r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_18:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY18r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_19:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_1A:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_GMRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY20r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_GVRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY21r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY22r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_CDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY30r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_CSSTP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY31r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_LLDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY32r, RTL8371C_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_IGMP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_IGMP_CTRLr, RTL8371C_IGMP_MLD_VLAN_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_IPMULTICAST:            
            RTKSW_SCAN_ALL_LOG_PORT(unit, port)
            {           
                if ((retVal = reg16_array_field_read(unit, RTL8371C_IPMUL_NO_VLAN_EGRESSr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_IPMUL_VLAN_LEAKYf, &tmp)) != RT_ERR_OK)
                    return retVal;
            if (port>0&&(tmp!=*pEnable))
                return RT_ERR_FAILED;
            *pEnable = tmp;				
            }
            break;          
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_leaky_portIsolation_set
 * Description:
 *      Set port isolation leaky.
 * Input:
 *      unit    - Unit ID
 *      type    - Packet type for port isolation leaky.
 *      enable  - Leaky status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      This API can set port isolation leaky for RMA ,IGMP/MLD, CDP, CSSTP, and LLDP  packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 *      - LEAKY_CDP,
 *      - LEAKY_CSSTP,
 *      - LEAKY_LLDP.
 */
rtksw_api_ret_t dal_rtl8371c_leaky_portIsolation_set(rtksw_uint32 unit, rtksw_leaky_type_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= LEAKY_END)
        return RT_ERR_INPUT;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    switch (type)
    {
        case LEAKY_BRG_GROUP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY00r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_FD_PAUSE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY01r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_SP_MCAST:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY02r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_1X_PAE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY03r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY08r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;            
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_8021AB:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Er, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_0F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_BRG_MNGEMENT:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY10r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_11: 
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY11r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_12:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY12r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_18:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY18r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_19:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_1A:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_GMRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY20r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_GVRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY21r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY22r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_CDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY30r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_CSSTP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY31r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_LLDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY32r, RTL8371C_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_IGMP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_IGMP_CTRLr, RTL8371C_IGMP_MLD_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_IPMULTICAST:        
            RTKSW_SCAN_ALL_LOG_PORT(unit, port)
            {           
                if ((retVal = reg16_array_field_write(unit, RTL8371C_IPMUL_NO_PORTISOr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_IPMUL_PORTISO_LEAKYf, &enable)) != RT_ERR_OK)
                    return retVal;
            }
            break;          
        default:
            break;
    }


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_leaky_portIsolation_get
 * Description:
 *      Get port isolation leaky.
 * Input:
 *      unit    - Unit ID
 *      type    - Packet type for port isolation leaky.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get port isolation leaky status for RMA ,IGMP/MLD, CDP, CSSTP, and LLDP  packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_IPMULTICAST.
 *      - LEAKY_CDP,
 *      - LEAKY_CSSTP,
 *      - LEAKY_LLDP.
 */
rtksw_api_ret_t dal_rtl8371c_leaky_portIsolation_get(rtksw_uint32 unit, rtksw_leaky_type_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 port, tmp;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= LEAKY_END)
        return RT_ERR_INPUT;

    switch (type)
    {
        case LEAKY_BRG_GROUP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY00r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_FD_PAUSE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY01r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_SP_MCAST:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY02r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_1X_PAE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY03r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_04:
        case LEAKY_UNDEF_BRG_05:
        case LEAKY_UNDEF_BRG_06:
        case LEAKY_UNDEF_BRG_07:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY08r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_09:
        case LEAKY_UNDEF_BRG_0A:
        case LEAKY_UNDEF_BRG_0B:
        case LEAKY_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;            
        case LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_8021AB:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Er, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_BRG_0F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_PORTISO_LEAKYf,pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_BRG_MNGEMENT:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY10r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_11: 
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY11r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_12:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY12r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_13:
        case LEAKY_UNDEFINED_14:
        case LEAKY_UNDEFINED_15:
        case LEAKY_UNDEFINED_16:
        case LEAKY_UNDEFINED_17:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_18:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY18r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_19:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_1A:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEFINED_1B:
        case LEAKY_UNDEFINED_1C:
        case LEAKY_UNDEFINED_1D:
        case LEAKY_UNDEFINED_1E:
        case LEAKY_UNDEFINED_1F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_GMRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY20r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_GVRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY21r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_UNDEF_GARP_22:
        case LEAKY_UNDEF_GARP_23:
        case LEAKY_UNDEF_GARP_24:
        case LEAKY_UNDEF_GARP_25:
        case LEAKY_UNDEF_GARP_26:
        case LEAKY_UNDEF_GARP_27:
        case LEAKY_UNDEF_GARP_28:
        case LEAKY_UNDEF_GARP_29:
        case LEAKY_UNDEF_GARP_2A:
        case LEAKY_UNDEF_GARP_2B:
        case LEAKY_UNDEF_GARP_2C:
        case LEAKY_UNDEF_GARP_2D:
        case LEAKY_UNDEF_GARP_2E:
        case LEAKY_UNDEF_GARP_2F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY22r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_CDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY30r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_CSSTP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY31r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_LLDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY32r, RTL8371C_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_IGMP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_IGMP_CTRLr, RTL8371C_IGMP_MLD_PORTISO_LEAKYf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case LEAKY_IPMULTICAST:            
            RTKSW_SCAN_ALL_LOG_PORT(unit, port)
            {           
                if ((retVal = reg16_array_field_read(unit, RTL8371C_IPMUL_NO_PORTISOr, (rtksw_uint32)rtksw_switch_port_L2P_get(unit, port), REG_ARRAY_INDEX_NONE, RTL8371C_IPMUL_PORTISO_LEAKYf, &tmp)) != RT_ERR_OK)
                    return retVal;
                if (port>0&&(tmp!=*pEnable))
                    return RT_ERR_FAILED;
                *pEnable = tmp;				
            }
            break;          
        default:
            break;
    }

    return RT_ERR_OK;
}


