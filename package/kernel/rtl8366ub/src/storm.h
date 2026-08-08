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

#ifndef __RTKSW_API_STORM_H__
#define __RTKSW_API_STORM_H__

typedef enum rtksw_rate_storm_group_e
{
    STORM_GROUP_UNKNOWN_UNICAST = 0,
    STORM_GROUP_UNKNOWN_MULTICAST,
    STORM_GROUP_MULTICAST,
    STORM_GROUP_BROADCAST,
    STORM_GROUP_END
} rtksw_rate_storm_group_t;

typedef enum rtksw_storm_bypass_e
{
    BYPASS_BRG_GROUP = 0,
    BYPASS_FD_PAUSE,
    BYPASS_SP_MCAST,
    BYPASS_1X_PAE,
    BYPASS_UNDEF_BRG_04,
    BYPASS_UNDEF_BRG_05,
    BYPASS_UNDEF_BRG_06,
    BYPASS_UNDEF_BRG_07,
    BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS,
    BYPASS_UNDEF_BRG_09,
    BYPASS_UNDEF_BRG_0A,
    BYPASS_UNDEF_BRG_0B,
    BYPASS_UNDEF_BRG_0C,
    BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS,
    BYPASS_8021AB,
    BYPASS_UNDEF_BRG_0F,
    BYPASS_BRG_MNGEMENT,
    BYPASS_UNDEFINED_11,
    BYPASS_UNDEFINED_12,
    BYPASS_UNDEFINED_13,
    BYPASS_UNDEFINED_14,
    BYPASS_UNDEFINED_15,
    BYPASS_UNDEFINED_16,
    BYPASS_UNDEFINED_17,
    BYPASS_UNDEFINED_18,
    BYPASS_UNDEFINED_19,
    BYPASS_UNDEFINED_1A,
    BYPASS_UNDEFINED_1B,
    BYPASS_UNDEFINED_1C,
    BYPASS_UNDEFINED_1D,
    BYPASS_UNDEFINED_1E,
    BYPASS_UNDEFINED_1F,
    BYPASS_GMRP,
    BYPASS_GVRP,
    BYPASS_UNDEF_GARP_22,
    BYPASS_UNDEF_GARP_23,
    BYPASS_UNDEF_GARP_24,
    BYPASS_UNDEF_GARP_25,
    BYPASS_UNDEF_GARP_26,
    BYPASS_UNDEF_GARP_27,
    BYPASS_UNDEF_GARP_28,
    BYPASS_UNDEF_GARP_29,
    BYPASS_UNDEF_GARP_2A,
    BYPASS_UNDEF_GARP_2B,
    BYPASS_UNDEF_GARP_2C,
    BYPASS_UNDEF_GARP_2D,
    BYPASS_UNDEF_GARP_2E,
    BYPASS_UNDEF_GARP_2F,
    BYPASS_IGMP,
    BYPASS_CDP,
    BYPASS_CSSTP,
    BYPASS_LLDP,
    BYPASS_END,
}rtksw_storm_bypass_t;

/* Function Name:
 *      rtksw_rate_stormControlMeterIdx_set
 * Description:
 *      Set the storm control meter index.
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_rate_stormControlMeterIdx_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_storm_group_t stormType, rtksw_uint32 index);

/* Function Name:
 *      rtksw_rate_stormControlMeterIdx_get
 * Description:
 *      Get the storm control meter index.
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_rate_stormControlMeterIdx_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_storm_group_t stormType, rtksw_uint32 *pIndex);

/* Function Name:
 *      rtksw_rate_stormControlPortEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_rate_stormControlPortEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_storm_group_t stormType, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_rate_stormControlPortEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_rate_stormControlPortEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_rate_storm_group_t stormType, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_storm_bypass_set
 * Description:
 *      Set bypass storm filter control configuration.
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_storm_bypass_set(rtksw_uint32 unit, rtksw_storm_bypass_t type, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_storm_bypass_get
 * Description:
 *      Get bypass storm filter control configuration.
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_storm_bypass_get(rtksw_uint32 unit, rtksw_storm_bypass_t type, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_rate_stormControlExtPortmask_set
 * Description:
 *      Set externsion storm control port mask
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_rate_stormControlExtPortmask_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_rate_stormControlExtPortmask_get
 * Description:
 *      Set externsion storm control port mask
 * Input:
 *      unit        - unit ID
 * Output:
 *      pPortmask   - port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_rate_stormControlExtPortmask_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_rate_stormControlExtEnable_set
 * Description:
 *      Set externsion storm control state
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_rate_stormControlExtEnable_set(rtksw_uint32 unit, rtksw_rate_storm_group_t stormType, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_rate_stormControlExtEnable_get
 * Description:
 *      Get externsion storm control state
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_rate_stormControlExtEnable_get(rtksw_uint32 unit, rtksw_rate_storm_group_t stormType, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_rate_stormControlExtMeterIdx_set
 * Description:
 *      Set externsion storm control meter index
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_rate_stormControlExtMeterIdx_set(rtksw_uint32 unit, rtksw_rate_storm_group_t stormType, rtksw_uint32 index);

/* Function Name:
 *      rtksw_rate_stormControlExtMeterIdx_get
 * Description:
 *      Get externsion storm control meter index
 * Input:
 *      unit        - unit ID
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
extern rtksw_api_ret_t rtksw_rate_stormControlExtMeterIdx_get(rtksw_uint32 unit, rtksw_rate_storm_group_t stormType, rtksw_uint32 *pIndex);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_rate_stormControlMeterIdx_set(port, stormType, index)           rtksw_rate_stormControlMeterIdx_set(0, port, stormType, index)
#define rtk_rate_stormControlMeterIdx_get(port, stormType, pIndex)          rtksw_rate_stormControlMeterIdx_get(0, port, stormType, pIndex)
#define rtk_rate_stormControlPortEnable_set(port, stormType, enable)        rtksw_rate_stormControlPortEnable_set(0, port, stormType, enable)
#define rtk_rate_stormControlPortEnable_get(port, stormType, pEnable)       rtksw_rate_stormControlPortEnable_get(0, port, stormType, pEnable)
#define rtk_storm_bypass_set(type, enable)                                  rtksw_storm_bypass_set(0, type, enable)
#define rtk_storm_bypass_get(type, pEnable)                                 rtksw_storm_bypass_get(0, type, pEnable)
#define rtk_rate_stormControlExtPortmask_set(pPortmask)                     rtksw_rate_stormControlExtPortmask_set(0, pPortmask)
#define rtk_rate_stormControlExtPortmask_get(pPortmask)                     rtksw_rate_stormControlExtPortmask_get(0, pPortmask)
#define rtk_rate_stormControlExtEnable_set(stormType, enable)               rtksw_rate_stormControlExtEnable_set(0, stormType, enable)
#define rtk_rate_stormControlExtEnable_get(stormType, pEnable)              rtksw_rate_stormControlExtEnable_get(0, stormType, pEnable)
#define rtk_rate_stormControlExtMeterIdx_set(stormType, index)              rtksw_rate_stormControlExtMeterIdx_set(0, stormType, index)
#define rtk_rate_stormControlExtMeterIdx_get(stormType, pIndex)             rtksw_rate_stormControlExtMeterIdx_get(0, stormType, pIndex)

#define rtk_rate_storm_group_t          rtksw_rate_storm_group_t
#define rtk_storm_bypass_t              rtksw_storm_bypass_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_STORM_H__ */
