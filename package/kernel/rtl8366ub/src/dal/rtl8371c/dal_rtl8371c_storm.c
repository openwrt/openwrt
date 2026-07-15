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
#include <dal/rtl8371c/dal_rtl8371c_storm.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_rate_stormControlMeterIdx_set
 * Description:
 *      Set the storm control meter index.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id
 *      storm_type  - storm group type
 *      index       - storm control meter index.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlMeterIdx_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_storm_group_t stormType, rtksw_uint32 index)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (index > RTKSW_MAX_METER_ID)
        return RT_ERR_FILTER_METER_ID;

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
			if ((retVal = reg16_array_field_write(unit, RTL8371C_RX_STORM_UNUCAST_METERr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_UNUCAST_MIDXf, &index)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
			if ((retVal = reg16_array_field_write(unit, RTL8371C_RX_STORM_UNMCAST_METERr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_UNMCAST_MIDXf, &index)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_MULTICAST:
			if ((retVal = reg16_array_field_write(unit, RTL8371C_RX_STORM_MCAST_METERr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_MCAST_MIDXf, &index)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_BROADCAST:
			if ((retVal = reg16_array_field_write(unit, RTL8371C_RX_STORM_BCAST_METERr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_BCAST_MIDXf, &index)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_stormControlMeterIdx_get
 * Description:
 *      Get the storm control meter index.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id
 *      storm_type  - storm group type
 * Output:
 *      pIndex     - storm control meter index.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlMeterIdx_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_storm_group_t stormType, rtksw_uint32 *pIndex)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (NULL == pIndex )
        return RT_ERR_NULL_POINTER;

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_RX_STORM_UNUCAST_METERr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_UNUCAST_MIDXf, pIndex)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_RX_STORM_UNMCAST_METERr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_UNMCAST_MIDXf, pIndex)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_MULTICAST:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_RX_STORM_MCAST_METERr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_MCAST_MIDXf, pIndex)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_BROADCAST:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_RX_STORM_BCAST_METERr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_BCAST_MIDXf, pIndex)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_stormControlPortEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id
 *      stormType   - storm group type
 *      enable      - enable status of storm control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlPortEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_storm_group_t stormType, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    phy_port = rtksw_switch_port_L2P_get(unit, port);

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
			if ((retVal = reg16_array_field_write(unit, RTL8371C_RX_STORM_UNUCAST_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_UNUCAST_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
			if ((retVal = reg16_array_field_write(unit, RTL8371C_RX_STORM_UNMCAST_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_UNMCAST_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_MULTICAST:
			if ((retVal = reg16_array_field_write(unit, RTL8371C_RX_STORM_MCAST_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_MCAST_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_BROADCAST:
			if ((retVal = reg16_array_field_write(unit, RTL8371C_RX_STORM_BCAST_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_BCAST_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_stormControlPortEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 *  Input:
 *      unit        - Unit ID
 *      port        - port id
 *      stormType   - storm group type
 * Output:
 *      pEnable     - enable status of storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlPortEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_storm_group_t stormType, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 phy_port;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Port Valid */
    RTKSW_CHK_PORT_VALID(unit, port);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (NULL == pEnable)
        return RT_ERR_ENABLE;
	
    phy_port = rtksw_switch_port_L2P_get(unit, port);

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_RX_STORM_UNUCAST_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_UNUCAST_ENf, pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_RX_STORM_UNMCAST_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_UNMCAST_ENf, pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_MULTICAST:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_RX_STORM_MCAST_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_MCAST_ENf, pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_BROADCAST:
			if ((retVal = reg16_array_field_read(unit, RTL8371C_RX_STORM_BCAST_CTRLr, phy_port, REG_ARRAY_INDEX_NONE, RTL8371C_RX_STORM_BCAST_ENf, pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_storm_bypass_set
 * Description:
 *      Set bypass storm filter control configuration.
 *  Input:
 *      unit        - Unit ID
 *      type        - Bypass storm filter control type.
 *      enable      - Bypass status.
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
 *      This API can set per-port bypass stomr filter control frame type including RMA and igmp.
 *      The bypass frame type is as following:
 *      - BYPASS_BRG_GROUP,
 *      - BYPASS_FD_PAUSE,
 *      - BYPASS_SP_MCAST,
 *      - BYPASS_1X_PAE,
 *      - BYPASS_UNDEF_BRG_04,
 *      - BYPASS_UNDEF_BRG_05,
 *      - BYPASS_UNDEF_BRG_06,
 *      - BYPASS_UNDEF_BRG_07,
 *      - BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - BYPASS_UNDEF_BRG_09,
 *      - BYPASS_UNDEF_BRG_0A,
 *      - BYPASS_UNDEF_BRG_0B,
 *      - BYPASS_UNDEF_BRG_0C,
 *      - BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - BYPASS_8021AB,
 *      - BYPASS_UNDEF_BRG_0F,
 *      - BYPASS_BRG_MNGEMENT,
 *      - BYPASS_UNDEFINED_11,
 *      - BYPASS_UNDEFINED_12,
 *      - BYPASS_UNDEFINED_13,
 *      - BYPASS_UNDEFINED_14,
 *      - BYPASS_UNDEFINED_15,
 *      - BYPASS_UNDEFINED_16,
 *      - BYPASS_UNDEFINED_17,
 *      - BYPASS_UNDEFINED_18,
 *      - BYPASS_UNDEFINED_19,
 *      - BYPASS_UNDEFINED_1A,
 *      - BYPASS_UNDEFINED_1B,
 *      - BYPASS_UNDEFINED_1C,
 *      - BYPASS_UNDEFINED_1D,
 *      - BYPASS_UNDEFINED_1E,
 *      - BYPASS_UNDEFINED_1F,
 *      - BYPASS_GMRP,
 *      - BYPASS_GVRP,
 *      - BYPASS_UNDEF_GARP_22,
 *      - BYPASS_UNDEF_GARP_23,
 *      - BYPASS_UNDEF_GARP_24,
 *      - BYPASS_UNDEF_GARP_25,
 *      - BYPASS_UNDEF_GARP_26,
 *      - BYPASS_UNDEF_GARP_27,
 *      - BYPASS_UNDEF_GARP_28,
 *      - BYPASS_UNDEF_GARP_29,
 *      - BYPASS_UNDEF_GARP_2A,
 *      - BYPASS_UNDEF_GARP_2B,
 *      - BYPASS_UNDEF_GARP_2C,
 *      - BYPASS_UNDEF_GARP_2D,
 *      - BYPASS_UNDEF_GARP_2E,
 *      - BYPASS_UNDEF_GARP_2F,
 *      - BYPASS_IGMP.
 *      - BYPASS_CDP.
 *      - BYPASS_CSSTP.
 *      - BYPASS_LLDP.
 */
rtksw_api_ret_t dal_rtl8371c_storm_bypass_set(rtksw_uint32 unit, rtksw_storm_bypass_t type, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= BYPASS_END)
        return RT_ERR_INPUT;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    switch (type)
    {
        case BYPASS_BRG_GROUP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY00r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_FD_PAUSE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY01r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_SP_MCAST:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY02r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_1X_PAE:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY03r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEF_BRG_04:
        case BYPASS_UNDEF_BRG_05:
        case BYPASS_UNDEF_BRG_06:
        case BYPASS_UNDEF_BRG_07:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY08r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEF_BRG_09:
        case BYPASS_UNDEF_BRG_0A:
        case BYPASS_UNDEF_BRG_0B:
        case BYPASS_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;            
        case BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_8021AB:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY0Er, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEF_BRG_0F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY04r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_BRG_MNGEMENT:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY10r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_11: 
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY11r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_12:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY12r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_13:
        case BYPASS_UNDEFINED_14:
        case BYPASS_UNDEFINED_15:
        case BYPASS_UNDEFINED_16:
        case BYPASS_UNDEFINED_17:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_18:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY18r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_19:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_1A:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_1B:
        case BYPASS_UNDEFINED_1C:
        case BYPASS_UNDEFINED_1D:
        case BYPASS_UNDEFINED_1E:
        case BYPASS_UNDEFINED_1F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY13r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_GMRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY20r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_GVRP:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY21r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEF_GARP_22:
        case BYPASS_UNDEF_GARP_23:
        case BYPASS_UNDEF_GARP_24:
        case BYPASS_UNDEF_GARP_25:
        case BYPASS_UNDEF_GARP_26:
        case BYPASS_UNDEF_GARP_27:
        case BYPASS_UNDEF_GARP_28:
        case BYPASS_UNDEF_GARP_29:
        case BYPASS_UNDEF_GARP_2A:
        case BYPASS_UNDEF_GARP_2B:
        case BYPASS_UNDEF_GARP_2C:
        case BYPASS_UNDEF_GARP_2D:
        case BYPASS_UNDEF_GARP_2E:
        case BYPASS_UNDEF_GARP_2F:
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY22r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_CDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY30r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_CSSTP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY31r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_LLDP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_RMA_ETY32r, RTL8371C_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_IGMP:            
            if ((retVal = reg16_field_write(unit, RTL8371C_IGMP_CTRLr, RTL8371C_IGMP_MLD_DISCARD_STORM_FILTERf, &enable)) != RT_ERR_OK)
                return retVal;
            break;         
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_storm_bypass_get
 * Description:
 *      Get bypass storm filter control configuration.
 *  Input:
 *      unit        - Unit ID
 *      type        - Bypass storm filter control type.
 * Output:
 *      pEnable - Bypass status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can get per-port bypass stomr filter control frame type including RMA and igmp.
 *      The bypass frame type is as following:
 *      - BYPASS_BRG_GROUP,
 *      - BYPASS_FD_PAUSE,
 *      - BYPASS_SP_MCAST,
 *      - BYPASS_1X_PAE,
 *      - BYPASS_UNDEF_BRG_04,
 *      - BYPASS_UNDEF_BRG_05,
 *      - BYPASS_UNDEF_BRG_06,
 *      - BYPASS_UNDEF_BRG_07,
 *      - BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - BYPASS_UNDEF_BRG_09,
 *      - BYPASS_UNDEF_BRG_0A,
 *      - BYPASS_UNDEF_BRG_0B,
 *      - BYPASS_UNDEF_BRG_0C,
 *      - BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - BYPASS_8021AB,
 *      - BYPASS_UNDEF_BRG_0F,
 *      - BYPASS_BRG_MNGEMENT,
 *      - BYPASS_UNDEFINED_11,
 *      - BYPASS_UNDEFINED_12,
 *      - BYPASS_UNDEFINED_13,
 *      - BYPASS_UNDEFINED_14,
 *      - BYPASS_UNDEFINED_15,
 *      - BYPASS_UNDEFINED_16,
 *      - BYPASS_UNDEFINED_17,
 *      - BYPASS_UNDEFINED_18,
 *      - BYPASS_UNDEFINED_19,
 *      - BYPASS_UNDEFINED_1A,
 *      - BYPASS_UNDEFINED_1B,
 *      - BYPASS_UNDEFINED_1C,
 *      - BYPASS_UNDEFINED_1D,
 *      - BYPASS_UNDEFINED_1E,
 *      - BYPASS_UNDEFINED_1F,
 *      - BYPASS_GMRP,
 *      - BYPASS_GVRP,
 *      - BYPASS_UNDEF_GARP_22,
 *      - BYPASS_UNDEF_GARP_23,
 *      - BYPASS_UNDEF_GARP_24,
 *      - BYPASS_UNDEF_GARP_25,
 *      - BYPASS_UNDEF_GARP_26,
 *      - BYPASS_UNDEF_GARP_27,
 *      - BYPASS_UNDEF_GARP_28,
 *      - BYPASS_UNDEF_GARP_29,
 *      - BYPASS_UNDEF_GARP_2A,
 *      - BYPASS_UNDEF_GARP_2B,
 *      - BYPASS_UNDEF_GARP_2C,
 *      - BYPASS_UNDEF_GARP_2D,
 *      - BYPASS_UNDEF_GARP_2E,
 *      - BYPASS_UNDEF_GARP_2F,
 *      - BYPASS_IGMP.
 *      - BYPASS_CDP.
 *      - BYPASS_CSSTP.
 *      - BYPASS_LLDP.
 */
rtksw_api_ret_t dal_rtl8371c_storm_bypass_get(rtksw_uint32 unit, rtksw_storm_bypass_t type, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (type >= BYPASS_END)
        return RT_ERR_INPUT;

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    switch (type)
    {
        case BYPASS_BRG_GROUP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY00r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_FD_PAUSE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY01r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_SP_MCAST:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY02r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_1X_PAE:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY03r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEF_BRG_04:
        case BYPASS_UNDEF_BRG_05:
        case BYPASS_UNDEF_BRG_06:
        case BYPASS_UNDEF_BRG_07:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY08r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEF_BRG_09:
        case BYPASS_UNDEF_BRG_0A:
        case BYPASS_UNDEF_BRG_0B:
        case BYPASS_UNDEF_BRG_0C:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;            
        case BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Dr, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_8021AB:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY0Er, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEF_BRG_0F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY04r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_BRG_MNGEMENT:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY10r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_11: 
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY11r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_12:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY12r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_13:
        case BYPASS_UNDEFINED_14:
        case BYPASS_UNDEFINED_15:
        case BYPASS_UNDEFINED_16:
        case BYPASS_UNDEFINED_17:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_18:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY18r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_19:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_1A:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY1Ar, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEFINED_1B:
        case BYPASS_UNDEFINED_1C:
        case BYPASS_UNDEFINED_1D:
        case BYPASS_UNDEFINED_1E:
        case BYPASS_UNDEFINED_1F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY13r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_GMRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY20r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_GVRP:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY21r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_UNDEF_GARP_22:
        case BYPASS_UNDEF_GARP_23:
        case BYPASS_UNDEF_GARP_24:
        case BYPASS_UNDEF_GARP_25:
        case BYPASS_UNDEF_GARP_26:
        case BYPASS_UNDEF_GARP_27:
        case BYPASS_UNDEF_GARP_28:
        case BYPASS_UNDEF_GARP_29:
        case BYPASS_UNDEF_GARP_2A:
        case BYPASS_UNDEF_GARP_2B:
        case BYPASS_UNDEF_GARP_2C:
        case BYPASS_UNDEF_GARP_2D:
        case BYPASS_UNDEF_GARP_2E:
        case BYPASS_UNDEF_GARP_2F:
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY22r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_CDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY30r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_CSSTP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY31r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_LLDP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_RMA_ETY32r, RTL8371C_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;
        case BYPASS_IGMP:            
            if ((retVal = reg16_field_read(unit, RTL8371C_IGMP_CTRLr, RTL8371C_IGMP_MLD_DISCARD_STORM_FILTERf, pEnable)) != RT_ERR_OK)
                return retVal;
            break;         
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_stormControlExtPortmask_set
 * Description:
 *      Set externsion storm control port mask
 *  Input:
 *      unit        - Unit ID
 *      pPortmask   - port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlExtPortmask_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (NULL == pPortmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = rtksw_switch_portmask_L2P_get(unit, pPortmask, &pmask)) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_field_write(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_EXT_EN_PORTMASKf, &pmask)) != RT_ERR_OK)
		return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_stormControlExtPortmask_get
 * Description:
 *      Set externsion storm control port mask
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pPortmask  - port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlExtPortmask_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);
	
    if (NULL == pPortmask)
    {
        return RT_ERR_NULL_POINTER;
    }
    
	if ((retVal = reg16_field_read(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_EXT_EN_PORTMASKf, &pmask)) != RT_ERR_OK)
		return retVal;

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, pmask, pPortmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_stormControlExtEnable_set
 * Description:
 *      Set externsion storm control state
 *  Input:
 *      unit        - Unit ID
 *      stormType   - storm group type
 *      enable      - externsion storm control state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlExtEnable_set(rtksw_uint32 unit, rtksw_rate_storm_group_t stormType, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (enable >= RTKSW_ENABLE_END)
        return RT_ERR_ENABLE;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
			if ((retVal = reg16_field_write(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_UNKNOWN_UCAST_EXT_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
			if ((retVal = reg16_field_write(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_UNKNOWN_MCAST_EXT_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_MULTICAST:
			if ((retVal = reg16_field_write(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_MCAST_EXT_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_BROADCAST:
			if ((retVal = reg16_field_write(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_BCAST_EXT_ENf, &enable)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_stormControlExtEnable_get
 * Description:
 *      Get externsion storm control state
 *  Input:
 *      unit        - Unit ID
 *      stormType   - storm group type
 * Output:
 *      pEnable     - externsion storm control state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlExtEnable_get(rtksw_uint32 unit, rtksw_rate_storm_group_t stormType, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
			if ((retVal = reg16_field_read(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_UNKNOWN_UCAST_EXT_ENf, pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
			if ((retVal = reg16_field_read(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_UNKNOWN_MCAST_EXT_ENf, pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_MULTICAST:
			if ((retVal = reg16_field_read(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_MCAST_EXT_ENf, pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_BROADCAST:
			if ((retVal = reg16_field_read(unit, RTL8371C_CFG_STORM_EXTr, RTL8371C_STORM_BCAST_EXT_ENf, pEnable)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_stormControlExtMeterIdx_set
 * Description:
 *      Set externsion storm control meter index
 *  Input:
 *      unit        - Unit ID
 *      stormType   - storm group type
 *      index       - externsion storm control state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlExtMeterIdx_set(rtksw_uint32 unit, rtksw_rate_storm_group_t stormType, rtksw_uint32 index)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if (index > RTKSW_MAX_METER_ID)
        return RT_ERR_FILTER_METER_ID;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
			if ((retVal = reg16_field_write(unit, RTL8371C_STORM_EXT_MTRIDX_CFGr, RTL8371C_STORM_UNKNOWN_UCAST_EXT_METERIDf, &index)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
			if ((retVal = reg16_field_write(unit, RTL8371C_STORM_EXT_MTRIDX_CFGr, RTL8371C_STORM_UNKNOWN_MCAST_EXT_METERIDf, &index)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_MULTICAST:
			if ((retVal = reg16_field_write(unit, RTL8371C_STORM_EXT_MTRIDX_CFGr, RTL8371C_STORM_MCAST_EXT_METERIDf, &index)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_BROADCAST:
			if ((retVal = reg16_field_write(unit, RTL8371C_STORM_EXT_MTRIDX_CFGr, RTL8371C_STORM_BCAST_EXT_METERIDf, &index)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_rate_stormControlExtMeterIdx_get
 * Description:
 *      Get externsion storm control meter index
 *  Input:
 *      unit        - Unit ID
 *      stormType   - storm group type
 *      pIndex      - externsion storm control state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
rtksw_api_ret_t dal_rtl8371c_rate_stormControlExtMeterIdx_get(rtksw_uint32 unit, rtksw_rate_storm_group_t stormType, rtksw_uint32 *pIndex)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if (stormType >= STORM_GROUP_END)
        return RT_ERR_SFC_UNKNOWN_GROUP;

    if(NULL == pIndex)
        return RT_ERR_NULL_POINTER;

    switch (stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
			if ((retVal = reg16_field_read(unit, RTL8371C_STORM_EXT_MTRIDX_CFGr, RTL8371C_STORM_UNKNOWN_UCAST_EXT_METERIDf, pIndex)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:
			if ((retVal = reg16_field_read(unit, RTL8371C_STORM_EXT_MTRIDX_CFGr, RTL8371C_STORM_UNKNOWN_MCAST_EXT_METERIDf, pIndex)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_MULTICAST:
			if ((retVal = reg16_field_read(unit, RTL8371C_STORM_EXT_MTRIDX_CFGr, RTL8371C_STORM_MCAST_EXT_METERIDf, pIndex)) != RT_ERR_OK)
				return retVal;
            break;
        case STORM_GROUP_BROADCAST:
			if ((retVal = reg16_field_read(unit, RTL8371C_STORM_EXT_MTRIDX_CFGr, RTL8371C_STORM_BCAST_EXT_METERIDf, pIndex)) != RT_ERR_OK)
				return retVal;
            break;
        default:
            break;
    }

    return RT_ERR_OK;
}



