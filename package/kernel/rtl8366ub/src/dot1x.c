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
#include <dot1x.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      rtksw_dot1x_unauthPacketOper_set
 * Description:
 *      Set 802.1x unauth action configuration.
 * Input:
 *      unit            - Unit ID
 *      port            - Port id.
 *      unauth_action   - 802.1X unauth action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      This API can set 802.1x unauth action configuration.
 *      The unauth action is as following:
 *      - DOT1X_ACTION_DROP
 *      - DOT1X_ACTION_TRAP2CPU
 *      - DOT1X_ACTION_GUESTVLAN
 */
rtksw_api_ret_t rtksw_dot1x_unauthPacketOper_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_dot1x_unauth_action_t unauth_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_unauthPacketOper_set)
        return RT_ERR_DRIVER_NOT_FOUND; 
	
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_unauthPacketOper_set(unit, port, unauth_action);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_unauthPacketOper_get
 * Description:
 *      Get 802.1x unauth action configuration.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pUnauth_action - 802.1X unauth action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get 802.1x unauth action configuration.
 *      The unauth action is as following:
 *      - DOT1X_ACTION_DROP
 *      - DOT1X_ACTION_TRAP2CPU
 *      - DOT1X_ACTION_GUESTVLAN
 */
rtksw_api_ret_t rtksw_dot1x_unauthPacketOper_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_dot1x_unauth_action_t *pUnauth_action)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_unauthPacketOper_get)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_unauthPacketOper_get(unit, port, pUnauth_action);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_eapolFrame2CpuEnable_set
 * Description:
 *      Set 802.1x EAPOL packet trap to CPU configuration
 * Input:
 *      unit    - Unit ID
 *      enable  - The status of 802.1x EAPOL packet.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      To support 802.1x authentication functionality, EAPOL frame (ether type = 0x888E) has to
 *      be trapped to CPU.
 *      The status of EAPOL frame trap to CPU is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_dot1x_eapolFrame2CpuEnable_set(rtksw_uint32 unit, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_eapolFrame2CpuEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_eapolFrame2CpuEnable_set(unit, enable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_eapolFrame2CpuEnable_get
 * Description:
 *      Get 802.1x EAPOL packet trap to CPU configuration
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - The status of 802.1x EAPOL packet.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      To support 802.1x authentication functionality, EAPOL frame (ether type = 0x888E) has to
 *      be trapped to CPU.
 *      The status of EAPOL frame trap to CPU is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_dot1x_eapolFrame2CpuEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_eapolFrame2CpuEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_eapolFrame2CpuEnable_get(unit, pEnable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_portBasedEnable_set
 * Description:
 *      Set 802.1x port-based enable configuration
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      enable  - The status of 802.1x port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_ENABLE               - Invalid enable input.
 *      RT_ERR_DOT1X_PORTBASEDPNEN  - 802.1X port-based enable error
 * Note:
 *      The API can update the port-based port enable register content. If a port is 802.1x
 *      port based network access control "enabled", it should be authenticated so packets
 *      from that port won't be dropped or trapped to CPU.
 *      The status of 802.1x port-based network access control is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_dot1x_portBasedEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->dot1x_portBasedEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND; 
	
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_portBasedEnable_set(unit, port, enable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_portBasedEnable_get
 * Description:
 *      Get 802.1x port-based enable configuration
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable - The status of 802.1x port.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get the 802.1x port-based port status.
 */
rtksw_api_ret_t rtksw_dot1x_portBasedEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_portBasedEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_portBasedEnable_get(unit, port, pEnable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_portBasedAuthStatus_set
 * Description:
 *      Set 802.1x port-based auth. port configuration
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      port_auth   - The status of 802.1x port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *     RT_ERR_DOT1X_PORTBASEDAUTH   - 802.1X port-based auth error
 * Note:
 *      The authenticated status of 802.1x port-based network access control is as following:
 *      - UNAUTH
 *      - AUTH
 */
rtksw_api_ret_t rtksw_dot1x_portBasedAuthStatus_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_dot1x_auth_status_t port_auth)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_portBasedAuthStatus_set)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_portBasedAuthStatus_set(unit, port, port_auth);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_portBasedAuthStatus_get
 * Description:
 *      Get 802.1x port-based auth. port configuration
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pPort_auth - The status of 802.1x port.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get 802.1x port-based port auth.information.
 */
rtksw_api_ret_t rtksw_dot1x_portBasedAuthStatus_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_dot1x_auth_status_t *pPort_auth)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_portBasedAuthStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_portBasedAuthStatus_get(unit, port, pPort_auth);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_portBasedDirection_set
 * Description:
 *      Set 802.1x port-based operational direction configuration
 * Input:
 *      unit            - Unit ID
 *      port            - Port id.
 *      port_direction  - Operation direction
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_DOT1X_PORTBASEDOPDIR - 802.1X port-based operation direction error
 * Note:
 *      The operate controlled direction of 802.1x port-based network access control is as following:
 *      - BOTH
 *      - IN
 */
rtksw_api_ret_t rtksw_dot1x_portBasedDirection_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_dot1x_direction_t port_direction)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_portBasedDirection_set)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_portBasedDirection_set(unit, port, port_direction);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_portBasedDirection_get
 * Description:
 *      Get 802.1X port-based operational direction configuration
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pPort_direction - Operation direction
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get 802.1x port-based operational direction information.
 */
rtksw_api_ret_t rtksw_dot1x_portBasedDirection_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_dot1x_direction_t *pPort_direction)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_portBasedDirection_get)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_portBasedDirection_get(unit, port, pPort_direction);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_macBasedEnable_set
 * Description:
 *      Set 802.1x mac-based port enable configuration
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      enable - The status of 802.1x port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_ENABLE               - Invalid enable input.
 *      RT_ERR_DOT1X_MACBASEDPNEN   - 802.1X mac-based enable error
 * Note:
 *      If a port is 802.1x MAC based network access control "enabled", the incoming packets should
 *       be authenticated so packets from that port won't be dropped or trapped to CPU.
 *      The status of 802.1x MAC-based network access control is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_dot1x_macBasedEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_macBasedEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_macBasedEnable_set(unit, port, enable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_macBasedEnable_get
 * Description:
 *      Get 802.1x mac-based port enable configuration
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pEnable - The status of 802.1x port.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      If a port is 802.1x MAC based network access control "enabled", the incoming packets should
 *      be authenticated so packets from that port wont be dropped or trapped to CPU.
 *      The status of 802.1x MAC-based network access control is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_dot1x_macBasedEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_macBasedEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_macBasedEnable_get(unit, port, pEnable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_macBasedAuthMac_add
 * Description:
 *      Add an authenticated MAC to ASIC
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pAuth_mac   - The authenticated MAC.
 *      fid         - filtering database.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_ENABLE               - Invalid enable input.
 *      RT_ERR_DOT1X_MACBASEDPNEN   - 802.1X mac-based enable error
 * Note:
 *      The API can add a 802.1x authenticated MAC address to port. If the MAC does not exist in LUT,
 *      user can't add this MAC to auth status.
 */
rtksw_api_ret_t rtksw_dot1x_macBasedAuthMac_add(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_t *pAuth_mac, rtksw_fid_t fid)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_macBasedAuthMac_add)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_macBasedAuthMac_add(unit, port, pAuth_mac, fid);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_macBasedAuthMac_del
 * Description:
 *      Delete an authenticated MAC to ASIC
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pAuth_mac   - The authenticated MAC.
 *      fid         - filtering database.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_MAC          - Invalid MAC address.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can delete a 802.1x authenticated MAC address to port. It only change the auth status of
 *      the MAC and won't delete it from LUT.
 */
rtksw_api_ret_t rtksw_dot1x_macBasedAuthMac_del(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_t *pAuth_mac, rtksw_fid_t fid)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_macBasedAuthMac_del)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_macBasedAuthMac_del(unit, port, pAuth_mac, fid);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_macBasedDirection_set
 * Description:
 *      Set 802.1x mac-based operational direction configuration
 * Input:
 *      unit            - Unit ID
 *      mac_direction   - Operation direction
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameter.
 *      RT_ERR_DOT1X_MACBASEDOPDIR  - 802.1X mac-based operation direction error
 * Note:
 *      The operate controlled direction of 802.1x mac-based network access control is as following:
 *      - BOTH
 *      - IN
 */
rtksw_api_ret_t rtksw_dot1x_macBasedDirection_set(rtksw_uint32 unit, rtksw_dot1x_direction_t mac_direction)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_macBasedDirection_set)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_macBasedDirection_set(unit, mac_direction);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_macBasedDirection_get
 * Description:
 *      Get 802.1x mac-based operational direction configuration
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 * Output:
 *      pMac_direction - Operation direction
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get 802.1x mac-based operational direction information.
 */
rtksw_api_ret_t rtksw_dot1x_macBasedDirection_get(rtksw_uint32 unit, rtksw_dot1x_direction_t *pMac_direction)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_macBasedDirection_get)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_macBasedDirection_get(unit, pMac_direction);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      Set 802.1x guest VLAN configuration
 * Description:
 *      Set 802.1x mac-based operational direction configuration
 * Input:
 *      unit    - Unit ID
 *      vid     - 802.1x guest VLAN ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The operate controlled 802.1x guest VLAN
 */
rtksw_api_ret_t rtksw_dot1x_guestVlan_set(rtksw_uint32 unit, rtksw_vlan_t vid)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_guestVlan_set)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_guestVlan_set(unit, vid);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_guestVlan_get
 * Description:
 *      Get 802.1x guest VLAN configuration
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pVid    - 802.1x guest VLAN ID
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get 802.1x guest VLAN information.
 */
rtksw_api_ret_t rtksw_dot1x_guestVlan_get(rtksw_uint32 unit, rtksw_vlan_t *pVid)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_guestVlan_get)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_guestVlan_get(unit, pVid);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_guestVlan2Auth_set
 * Description:
 *      Set 802.1x guest VLAN to auth host configuration
 * Input:
 *      unit    - Unit ID
 *      enable  - The status of guest VLAN to auth host.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The operational direction of 802.1x guest VLAN to auth host control is as following:
 *      - RTKSW_ENABLED
 *      - RTKSW_DISABLED
 */
rtksw_api_ret_t rtksw_dot1x_guestVlan2Auth_set(rtksw_uint32 unit, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_guestVlan2Auth_set)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_guestVlan2Auth_set(unit, enable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_dot1x_guestVlan2Auth_get
 * Description:
 *      Get 802.1x guest VLAN to auth host configuration
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - The status of guest VLAN to auth host.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The API can get 802.1x guest VLAN to auth host information.
 */
rtksw_api_ret_t rtksw_dot1x_guestVlan2Auth_get(rtksw_uint32 unit, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);

    if (NULL == RT_MAPPER->dot1x_guestVlan2Auth_get)
        return RT_ERR_DRIVER_NOT_FOUND; 
	    
    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->dot1x_guestVlan2Auth_get(unit, pEnable);    
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


