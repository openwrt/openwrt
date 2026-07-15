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
#include <vlan.h>
#include <rate.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      rtksw_vlan_init
 * Description:
 *      Initialize VLAN.
 * Input:
 *      unit        - Unit ID
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *      VLAN is disabled by default. User has to call this API to enable VLAN before
 *      using it. And It will set a default VLAN(vid 1) including all ports and set
 *      all ports PVID to the default VLAN.
 */
rtksw_api_ret_t rtksw_vlan_init(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_init(unit);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_set
 * Description:
 *      Set a VLAN entry.
 * Input:
 *      unit        - Unit ID
 *      vid         - VLAN ID to configure.
 *      pVlanCfg    - VLAN Configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameters.
 *      RT_ERR_L2_FID               - Invalid FID.
 *      RT_ERR_VLAN_PORT_MBR_EXIST  - Invalid member port mask.
 *      RT_ERR_VLAN_VID             - Invalid VID parameter.
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_set(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_set(unit, vid, pVlanCfg);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_get
 * Description:
 *      Get a VLAN entry.
 * Input:
 *      unit        - Unit ID
 *      vid         - VLAN ID to configure.
 * Output:
 *      pVlanCfg    - VLAN Configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_get(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_vlan_cfg_t *pVlanCfg)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_get(unit, vid, pVlanCfg);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_egrFilterEnable_set
 * Description:
 *      Set VLAN egress filter.
 * Input:
 *      unit        - Unit ID
 *      egrFilter   - Egress filtering
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_ENABLE       - Invalid input parameters.
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_egrFilterEnable_set(rtksw_uint32 unit, rtksw_enable_t egrFilter)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_egrFilterEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_egrFilterEnable_set(unit, egrFilter);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_egrFilterEnable_get
 * Description:
 *      Get VLAN egress filter.
 * Input:
 *      unit        - Unit ID
 *      pEgrFilter - Egress filtering
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - NULL Pointer.
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_egrFilterEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEgrFilter)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_egrFilterEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_egrFilterEnable_get(unit, pEgrFilter);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_mbrCfg_set
 * Description:
 *      Set a VLAN Member Configuration entry by index.
 * Input:
 *      unit        - Unit ID
 *      idx         - Index of VLAN Member Configuration.
 *      pMbrcfg     - VLAN member Configuration.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *     Set a VLAN Member Configuration entry by index.
 */
rtksw_api_ret_t rtksw_vlan_mbrCfg_set(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_vlan_mbrcfg_t *pMbrcfg)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_mbrCfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_mbrCfg_set(unit, idx, pMbrcfg);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_mbrCfg_get
 * Description:
 *      Get a VLAN Member Configuration entry by index.
 * Input:
 *      unit        - Unit ID
 *      idx         - Index of VLAN Member Configuration.
 * Output:
 *      pMbrcfg     - VLAN member Configuration.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *     Get a VLAN Member Configuration entry by index.
 */
rtksw_api_ret_t rtksw_vlan_mbrCfg_get(rtksw_uint32 unit, rtksw_uint32 idx, rtksw_vlan_mbrcfg_t *pMbrcfg)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_mbrCfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_mbrCfg_get(unit, idx, pMbrcfg);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *     rtksw_vlan_portPvid_set
 * Description:
 *      Set port to specified VLAN ID(PVID).
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pvid        - Specified VLAN ID.
 *      priority    - 802.1p priority for the PVID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_VLAN_PRIORITY        - Invalid priority.
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - VLAN entry not found.
 *      RT_ERR_VLAN_VID             - Invalid VID parameter.
 * Note:
 *       The API is used for Port-based VLAN. The untagged frame received from the
 *       port will be classified to the specified VLAN and assigned to the specified priority.
 */
rtksw_api_ret_t rtksw_vlan_portPvid_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t pvid, rtksw_pri_t priority)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_portPvid_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_portPvid_set(unit, port, pvid, priority);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_portPvid_get
 * Description:
 *      Get VLAN ID(PVID) on specified port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pPvid       - Specified VLAN ID.
 *      pPriority   - 802.1p priority for the PVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can get the PVID and 802.1p priority for the PVID of Port-based VLAN.
 */
rtksw_api_ret_t rtksw_vlan_portPvid_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_t *pPvid, rtksw_pri_t *pPriority)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_portPvid_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_portPvid_get(unit, port, pPvid, pPriority);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_portIgrFilterEnable_set
 * Description:
 *      Set VLAN ingress for each port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      igr_filter  - VLAN ingress function enable status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      The status of vlan ingress filter is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 *      While VLAN function is enabled, ASIC will decide VLAN ID for each received frame and get belonged member
 *      ports from VLAN table. If received port is not belonged to VLAN member ports, ASIC will drop received frame if VLAN ingress function is enabled.
 */
rtksw_api_ret_t rtksw_vlan_portIgrFilterEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t igr_filter)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_portIgrFilterEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_portIgrFilterEnable_set(unit, port, igr_filter);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_portIgrFilterEnable_get
 * Description:
 *      Get VLAN Ingress Filter
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pIgr_filter - VLAN ingress function enable status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can Get the VLAN ingress filter status.
 *     The status of vlan ingress filter is as following:
 *     - RTKSW_DISABLED
 *     - RTKSW_ENABLED
 */
rtksw_api_ret_t rtksw_vlan_portIgrFilterEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pIgr_filter)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_portIgrFilterEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_portIgrFilterEnable_get(unit, port, pIgr_filter);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_portAcceptFrameType_set
 * Description:
 *      Set VLAN accept_frame_type
 * Input:
 *      unit                - Unit ID
 *      port                - Port id.
 *      accept_frame_type   - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                       - OK
 *      RT_ERR_FAILED                   - Failed
 *      RT_ERR_SMI                      - SMI access error
 *      RT_ERR_PORT_ID                  - Invalid port number.
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE   - Invalid frame type.
 * Note:
 *      The API is used for checking 802.1Q tagged frames.
 *      The accept frame type as following:
 *      - RTKSW_ACCEPT_FRAME_TYPE_ALL
 *      - RTKSW_ACCEPT_FRAME_TYPE_TAG_ONLY
 *      - RTKSW_ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtksw_api_ret_t rtksw_vlan_portAcceptFrameType_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_acceptFrameType_t accept_frame_type)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_portAcceptFrameType_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_portAcceptFrameType_set(unit, port, accept_frame_type);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_portAcceptFrameType_get
 * Description:
 *      Get VLAN accept_frame_type
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pAccept_frame_type - accept frame type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     The API can Get the VLAN ingress filter.
 *     The accept frame type as following:
 *     - RTKSW_ACCEPT_FRAME_TYPE_ALL
 *     - RTKSW_ACCEPT_FRAME_TYPE_TAG_ONLY
 *     - RTKSW_ACCEPT_FRAME_TYPE_UNTAG_ONLY
 */
rtksw_api_ret_t rtksw_vlan_portAcceptFrameType_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_acceptFrameType_t *pAccept_frame_type)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_portAcceptFrameType_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_portAcceptFrameType_get(unit, port, pAccept_frame_type);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_protoAndPortBasedVlan_add
 * Description:
 *      Add the protocol-and-port-based vlan to the specified port of device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      pInfo       - Protocol and port based VLAN configuration information.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_VLAN_VID         - Invalid VID parameter.
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 *      RT_ERR_TBL_FULL         - Table is full.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The incoming packet which match the protocol-and-port-based vlan will use the configure vid for ingress pipeline
 *      The frame type is shown in the following:
 *      - RTKSW_FRAME_TYPE_ETHERNET
 *      - RTKSW_FRAME_TYPE_RFC1042
 *      - RTKSW_FRAME_TYPE_LLCOTHER
 */
rtksw_api_ret_t rtksw_vlan_protoAndPortBasedVlan_add(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_protoAndPortInfo_t *pInfo)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_protoAndPortBasedVlan_add)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_protoAndPortBasedVlan_add(unit, port, pInfo);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_protoAndPortBasedVlan_get
 * Description:
 *      Get the protocol-and-port-based vlan to the specified port of device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      proto_type  - protocol-and-port-based vlan protocol type.
 *      frame_type  - protocol-and-port-based vlan frame type.
 * Output:
 *      pInfo - Protocol and port based VLAN configuration information.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 *      RT_ERR_TBL_FULL         - Table is full.
 * Note:
 *     The incoming packet which match the protocol-and-port-based vlan will use the configure vid for ingress pipeline
 *     The frame type is shown in the following:
 *      - RTKSW_FRAME_TYPE_ETHERNET
 *      - RTKSW_FRAME_TYPE_RFC1042
 *      - RTKSW_FRAME_TYPE_LLCOTHER
 */
rtksw_api_ret_t rtksw_vlan_protoAndPortBasedVlan_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_proto_type_t proto_type, rtksw_vlan_protoVlan_frameType_t frame_type, rtksw_vlan_protoAndPortInfo_t *pInfo)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_protoAndPortBasedVlan_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_protoAndPortBasedVlan_get(unit, port, proto_type, frame_type, pInfo);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_protoAndPortBasedVlan_del
 * Description:
 *      Delete the protocol-and-port-based vlan from the specified port of device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      proto_type  - protocol-and-port-based vlan protocol type.
 *      frame_type  - protocol-and-port-based vlan frame type.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 *      RT_ERR_TBL_FULL         - Table is full.
 * Note:
 *     The incoming packet which match the protocol-and-port-based vlan will use the configure vid for ingress pipeline
 *     The frame type is shown in the following:
 *      - RTKSW_FRAME_TYPE_ETHERNET
 *      - RTKSW_FRAME_TYPE_RFC1042
 *      - RTKSW_FRAME_TYPE_LLCOTHER
 */
rtksw_api_ret_t rtksw_vlan_protoAndPortBasedVlan_del(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_proto_type_t proto_type, rtksw_vlan_protoVlan_frameType_t frame_type)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_protoAndPortBasedVlan_del)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_protoAndPortBasedVlan_del(unit, port, proto_type, frame_type);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_protoAndPortBasedVlan_delAll
 * Description:
 *     Delete all protocol-and-port-based vlans from the specified port of device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *     The incoming packet which match the protocol-and-port-based vlan will use the configure vid for ingress pipeline
 *     Delete all flow table protocol-and-port-based vlan entries.
 */
rtksw_api_ret_t rtksw_vlan_protoAndPortBasedVlan_delAll(rtksw_uint32 unit, rtksw_port_t port)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_protoAndPortBasedVlan_delAll)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_protoAndPortBasedVlan_delAll(unit, port);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_tagMode_set
 * Description:
 *      Set CVLAN egress tag mode
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      tag_mode    - The egress tag mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The API can set Egress tag mode. There are 4 mode for egress tag:
 *      - RTKSW_VLAN_TAG_MODE_ORIGINAL,
 *      - RTKSW_VLAN_TAG_MODE_KEEP_FORMAT,
 *      - RTKSW_VLAN_TAG_MODE_PRI.
 *      - RTKSW_VLAN_TAG_MODE_REAL_KEEP_FORMAT,
 */
rtksw_api_ret_t rtksw_vlan_tagMode_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_tagMode_t tag_mode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_tagMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_tagMode_set(unit, port, tag_mode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_tagMode_get
 * Description:
 *      Get CVLAN egress tag mode
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pTag_mode   - The egress tag mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get Egress tag mode. There are 4 mode for egress tag:
 *      - RTKSW_VLAN_TAG_MODE_ORIGINAL,
 *      - RTKSW_VLAN_TAG_MODE_KEEP_FORMAT,
 *      - RTKSW_VLAN_TAG_MODE_PRI.
 *      - RTKSW_VLAN_TAG_MODE_REAL_KEEP_FORMAT,
 */
rtksw_api_ret_t rtksw_vlan_tagMode_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_vlan_tagMode_t *pTag_mode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_tagMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_tagMode_get(unit, port, pTag_mode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_transparent_set
 * Description:
 *      Set VLAN transparent mode
 * Input:
 *      unit            - Unit ID
 *      egr_port        - Egress Port id.
 *      pIgr_pmask      - Ingress Port Mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtksw_api_ret_t rtksw_vlan_transparent_set(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_transparent_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_transparent_set(unit, egr_port, pIgr_pmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_transparent_get
 * Description:
 *      Get VLAN transparent mode
 * Input:
 *      unit            - Unit ID
 *      egr_port        - Egress Port id.
 * Output:
 *      pIgr_pmask      - Ingress Port Mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtksw_api_ret_t rtksw_vlan_transparent_get(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_transparent_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_transparent_get(unit, egr_port, pIgr_pmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_keep_set
 * Description:
 *      Set VLAN egress keep mode
 * Input:
 *      unit            - Unit ID
 *      egr_port        - Egress Port id.
 *      pIgr_pmask      - Ingress Port Mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtksw_api_ret_t rtksw_vlan_keep_set(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_keep_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_keep_set(unit, egr_port, pIgr_pmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_keep_get
 * Description:
 *      Get VLAN egress keep mode
 * Input:
 *      unit            - Unit ID
 *      egr_port        - Egress Port id.
 * Output:
 *      pIgr_pmask      - Ingress Port Mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      None.
 */
rtksw_api_ret_t rtksw_vlan_keep_get(rtksw_uint32 unit, rtksw_port_t egr_port, rtksw_portmask_t *pIgr_pmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_keep_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_keep_get(unit, egr_port, pIgr_pmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device
 * Input:
 *      unit        - Unit ID
 *      vid         - Specified VLAN ID.
 *      stg         - spanning tree group instance.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_MSTI         - Invalid msti parameter
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *      The API can set spanning tree group instance of the vlan to the specified device.
 */
rtksw_api_ret_t rtksw_vlan_stg_set(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_stp_msti_id_t stg)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_stg_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_stg_set(unit, vid, stg);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_stg_get
 * Description:
 *      Get spanning tree group instance of the vlan to the specified device
 * Input:
 *      unit        - Unit ID
 *      vid         - Specified VLAN ID.
 * Output:
 *      pStg        - spanning tree group instance.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 * Note:
 *      The API can get spanning tree group instance of the vlan to the specified device.
 */
rtksw_api_ret_t rtksw_vlan_stg_get(rtksw_uint32 unit, rtksw_vlan_t vid, rtksw_stp_msti_id_t *pStg)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_stg_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_stg_get(unit, vid, pStg);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_portFid_set
 * Description:
 *      Set port-based filtering database
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      enable      - ebable port-based FID
 *      fid         - Specified filtering database.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_L2_FID - Invalid fid.
 *      RT_ERR_INPUT - Invalid input parameter.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can set port-based filtering database. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
rtksw_api_ret_t rtksw_vlan_portFid_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable, rtksw_fid_t fid)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_portFid_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_portFid_set(unit, port, enable, fid);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_portFid_get
 * Description:
 *      Get port-based filtering database
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pEnable     - ebable port-based FID
 *      pFid        - Specified filtering database.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can get port-based filtering database status. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
rtksw_api_ret_t rtksw_vlan_portFid_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable, rtksw_fid_t *pFid)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_portFid_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_portFid_get(unit, port, pEnable, pFid);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_UntagDscpPriorityEnable_set
 * Description:
 *      Set Untag DSCP priority assign
 * Input:
 *      unit        - Unit ID
 *      enable      - state of Untag DSCP priority assign
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_ENABLE          - Invalid input parameters.
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_UntagDscpPriorityEnable_set(rtksw_uint32 unit, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_UntagDscpPriorityEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_UntagDscpPriorityEnable_set(unit, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_UntagDscpPriorityEnable_get
 * Description:
 *      Get Untag DSCP priority assign
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnable - state of Untag DSCP priority assign
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_NULL_POINTER    - Null pointer
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_UntagDscpPriorityEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_UntagDscpPriorityEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_UntagDscpPriorityEnable_get(unit, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_stp_mstpState_set
 * Description:
 *      Configure spanning tree state per each port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id
 *      msti        - Multiple spanning tree instance.
 *      stp_state   - Spanning tree state for msti
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_MSTI         - Invalid msti parameter.
 *      RT_ERR_MSTP_STATE   - Invalid STP state.
 * Note:
 *      System supports per-port multiple spanning tree state for each msti.
 *      There are four states supported by ASIC.
 *      - RTKSW_STP_STATE_DISABLED
 *      - RTKSW_STP_STATE_BLOCKING
 *      - RTKSW_STP_STATE_LEARNING
 *      - RTKSW_STP_STATE_FORWARDING
 */
rtksw_api_ret_t rtksw_stp_mstpState_set(rtksw_uint32 unit, rtksw_stp_msti_id_t msti, rtksw_port_t port, rtksw_stp_state_t stp_state)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->stp_mstpState_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->stp_mstpState_set(unit, msti, port, stp_state);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_stp_mstpState_get
 * Description:
 *      Get spanning tree state per each port.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      msti        - Multiple spanning tree instance.
 * Output:
 *      pStp_state  - Spanning tree state for msti
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_MSTI         - Invalid msti parameter.
 * Note:
 *      System supports per-port multiple spanning tree state for each msti.
 *      There are four states supported by ASIC.
 *      - RTKSW_STP_STATE_DISABLED
 *      - RTKSW_STP_STATE_BLOCKING
 *      - RTKSW_STP_STATE_LEARNING
 *      - RTKSW_STP_STATE_FORWARDING
 */
rtksw_api_ret_t rtksw_stp_mstpState_get(rtksw_uint32 unit, rtksw_stp_msti_id_t msti, rtksw_port_t port, rtksw_stp_state_t *pStp_state)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->stp_mstpState_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->stp_mstpState_get(unit, msti, port, pStp_state);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_reservedVidAction_set
 * Description:
 *      Set Action of VLAN ID = 0 & 4095 tagged packet
 * Input:
 *      unit            - Unit ID
 *      action_vid0     - Action for VID 0.
 *      action_vid4095  - Action for VID 4095.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_reservedVidAction_set(rtksw_uint32 unit, rtksw_vlan_resVidAction_t action_vid0, rtksw_vlan_resVidAction_t action_vid4095)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_reservedVidAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_reservedVidAction_set(unit, action_vid0, action_vid4095);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_reservedVidAction_get
 * Description:
 *      Get Action of VLAN ID = 0 & 4095 tagged packet
 * Input:
 *      unit                - Unit ID
 *      pAction_vid0        - Action for VID 0.
 *      pAction_vid4095     - Action for VID 4095.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - NULL Pointer
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_reservedVidAction_get(rtksw_uint32 unit, rtksw_vlan_resVidAction_t *pAction_vid0, rtksw_vlan_resVidAction_t *pAction_vid4095)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_reservedVidAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_reservedVidAction_get(unit, pAction_vid0, pAction_vid4095);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_realKeepRemarkEnable_set
 * Description:
 *      Set Real keep 1p remarking feature
 * Input:
 *      unit        - Unit ID
 *      enabled     - State of 1p remarking at real keep packet
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_realKeepRemarkEnable_set(rtksw_uint32 unit, rtksw_enable_t enabled)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_realKeepRemarkEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_realKeepRemarkEnable_set(unit, enabled);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_realKeepRemarkEnable_get
 * Description:
 *      Get Real keep 1p remarking feature
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnabled     - State of 1p remarking at real keep packet
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_realKeepRemarkEnable_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_realKeepRemarkEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_realKeepRemarkEnable_get(unit, pEnabled);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_vlan_reset
 * Description:
 *      Reset VLAN
 * Input:
 *      unit        - Unit ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *
 */
rtksw_api_ret_t rtksw_vlan_reset(rtksw_uint32 unit)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->vlan_reset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->vlan_reset(unit);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

