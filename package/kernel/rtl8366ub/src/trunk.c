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
#include <trunk.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      rtksw_trunk_port_set
 * Description:
 *      Set trunking group available port mask
 * Input:
 *      unit                    - Unit ID
 *      trk_gid                 - trunk group id
 *      pTrunk_member_portmask  - Logic trunking member port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_LA_TRUNK_ID  - Invalid trunking group
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      The API can set port trunking group port mask. Each port trunking group has max 4 ports.
 *      If enabled port mask has less than 2 ports available setting, then this trunking group function is disabled.
 */
rtksw_api_ret_t rtksw_trunk_port_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_portmask_t *pTrunk_member_portmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_port_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_port_set(unit, trk_gid, pTrunk_member_portmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_port_get
 * Description:
 *      Get trunking group available port mask
 * Input:
 *      unit        - Unit ID
 *      trk_gid     - trunk group id
 * Output:
 *      pTrunk_member_portmask - Logic trunking member port mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_LA_TRUNK_ID  - Invalid trunking group
 * Note:
 *      The API can get 2 port trunking group.
 */
rtksw_api_ret_t rtksw_trunk_port_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_portmask_t *pTrunk_member_portmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_port_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_port_get(unit, trk_gid, pTrunk_member_portmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_distributionAlgorithm_set
 * Description:
 *      Set port trunking hash select sources
 * Input:
 *      unit            - Unit ID
 *      trk_gid         - trunk group id
 *      algo_bitmask    - Bitmask of the distribution algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_LA_TRUNK_ID  - Invalid trunking group
 *      RT_ERR_LA_HASHMASK  - Hash algorithm selection error.
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 * Note:
 *      The API can set port trunking hash algorithm sources.
 *      7 bits mask for link aggregation group0 hash parameter selection {DIP, SIP, DMAC, SMAC, SPA}
 *      - 0b0000001: SPA
 *      - 0b0000010: SMAC
 *      - 0b0000100: DMAC
 *      - 0b0001000: SIP
 *      - 0b0010000: DIP
 *      - 0b0100000: TCP/UDP Source Port
 *      - 0b1000000: TCP/UDP Destination Port
 *      Example:
 *      - 0b0000011: SMAC & SPA
 *      - Note that it could be an arbitrary combination or independent set
 */
rtksw_api_ret_t rtksw_trunk_distributionAlgorithm_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_uint32 algo_bitmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_distributionAlgorithm_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_distributionAlgorithm_set(unit, trk_gid, algo_bitmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_distributionAlgorithm_get
 * Description:
 *      Get port trunking hash select sources
 * Input:
 *      unit        - Unit ID
 *      trk_gid     - trunk group id
 * Output:
 *      pAlgo_bitmask -  Bitmask of the distribution algorithm
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_LA_TRUNK_ID  - Invalid trunking group
 * Note:
 *      The API can get port trunking hash algorithm sources.
 */
rtksw_api_ret_t rtksw_trunk_distributionAlgorithm_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_uint32 *pAlgo_bitmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_distributionAlgorithm_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_distributionAlgorithm_get(unit, trk_gid, pAlgo_bitmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_trafficSeparate_set
 * Description:
 *      Set the traffic separation setting of a trunk group from the specified device.
 * Input:
 *      unit            - Unit ID
 *      trk_gid         - trunk group id
 *      separateType    - traffic separation setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_LA_TRUNK_ID - invalid trunk ID
 *      RT_ERR_LA_HASHMASK - invalid hash mask
 * Note:
 *      SEPARATE_NONE: disable traffic separation
 *      SEPARATE_FLOOD: trunk MSB link up port is dedicated to TX flooding (L2 lookup miss) traffic
 */
rtksw_api_ret_t rtksw_trunk_trafficSeparate_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_separateType_t separateType)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_trafficSeparate_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_trafficSeparate_set(unit, trk_gid, separateType);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_trafficSeparate_get
 * Description:
 *      Get the traffic separation setting of a trunk group from the specified device.
 * Input:
 *      unit            - Unit ID
 *      trk_gid         - trunk group id
 * Output:
 *      pSeparateType   - pointer separated traffic type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      SEPARATE_NONE: disable traffic separation
 *      SEPARATE_FLOOD: trunk MSB link up port is dedicated to TX flooding (L2 lookup miss) traffic
 */
rtksw_api_ret_t rtksw_trunk_trafficSeparate_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_separateType_t *pSeparateType)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_trafficSeparate_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_trafficSeparate_get(unit, trk_gid, pSeparateType);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_mode_set
 * Description:
 *      Set the trunk mode to the specified device.
 * Input:
 *      unit        - Unit ID
 *      mode        - trunk mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      The enum of the trunk mode as following
 *      - TRUNK_MODE_NORMAL
 *      - TRUNK_MODE_DUMB
 */
rtksw_api_ret_t rtksw_trunk_mode_set(rtksw_uint32 unit, rtksw_trunk_mode_t mode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_mode_set(unit, mode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_mode_get
 * Description:
 *      Get the trunk mode from the specified device.
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pMode - pointer buffer of trunk mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The enum of the trunk mode as following
 *      - TRUNK_MODE_NORMAL
 *      - TRUNK_MODE_DUMB
 */
rtksw_api_ret_t rtksw_trunk_mode_get(rtksw_uint32 unit, rtksw_trunk_mode_t *pMode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_mode_get(unit, pMode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_distributionMode_set
 * Description:
 *      Set the trunk mode to the specified device.
 * Input:
 *      unit        - Unit ID
 *      trk_gid     - trunk group id
 *      mode        - trunk mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      The enum of the trunk mode as following
 *      - TRUNK_MODE_NORMAL
 *      - TRUNK_MODE_DUMB
 */
rtksw_api_ret_t rtksw_trunk_distributionMode_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_mode_t mode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_distributionMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_distributionMode_set(unit, trk_gid, mode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_distributionMode_get
 * Description:
 *      Get the trunk mode from the specified device.
 * Input:
 *      unit        - Unit ID
 *      trk_gid     - trunk group id
 * Output:
 *      pMode - pointer buffer of trunk mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The enum of the trunk mode as following
 *      - TRUNK_MODE_NORMAL
 *      - TRUNK_MODE_DUMB
 */
rtksw_api_ret_t rtksw_trunk_distributionMode_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_mode_t *pMode)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_distributionMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_distributionMode_get(unit, trk_gid, pMode);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_trafficPause_set
 * Description:
 *      Set the traffic pause setting of a trunk group.
 * Input:
 *      unit         - Unit ID
 *      trk_gid      - trunk group id
 *      enable       - traffic pause state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID - invalid trunk ID
 * Note:
 *      None.
 */
rtksw_api_ret_t rtksw_trunk_trafficPause_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_trafficPause_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_trafficPause_set(unit, trk_gid, enable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_trafficPause_get
 * Description:
 *      Get the traffic pause setting of a trunk group.
 * Input:
 *      unit           - Unit ID
 *      trk_gid        - trunk group id
 * Output:
 *      pEnable        - pointer of traffic pause state.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
rtksw_api_ret_t rtksw_trunk_trafficPause_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_trafficPause_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_trafficPause_get(unit, trk_gid, pEnable);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_hashMappingTable_set
 * Description:
 *      Set hash value to port array in the trunk group id from the specified device.
 * Input:
 *      unit                - Unit ID
 *      trk_gid             - trunk group id
 *      pHash2Port_array    - ports associate with the hash value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_LA_TRUNK_ID        - invalid trunk ID
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_LA_TRUNK_NOT_EXIST - the trunk doesn't exist
 *      RT_ERR_LA_NOT_MEMBER_PORT - the port is not a member port of the trunk
 *      RT_ERR_LA_CPUPORT         - CPU port can not be aggregated port
 * Note:
 *      Trunk group 0 & 1 shares the same hash mapping table.
 *      Trunk group 2 uses a independent table.
 */
rtksw_api_ret_t rtksw_trunk_hashMappingTable_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_hashVal2Port_t *pHash2Port_array)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_hashMappingTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_hashMappingTable_set(unit, trk_gid, pHash2Port_array);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_hashMappingTable_get
 * Description:
 *      Get hash value to port array in the trunk group id from the specified device.
 * Input:
 *      unit            - Unit ID
 *      trk_gid         - trunk group id
 * Output:
 *      pHash2Port_array - pointer buffer of ports associate with the hash value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Trunk group 0 & 1 shares the same hash mapping table.
 *      Trunk group 2 uses a independent table.
 */
rtksw_api_ret_t rtksw_trunk_hashMappingTable_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_hashVal2Port_t *pHash2Port_array)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_hashMappingTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_hashMappingTable_get(unit, trk_gid, pHash2Port_array);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}

/* Function Name:
 *      rtksw_trunk_portQueueEmpty_get
 * Description:
 *      Get the port mask which all queues are empty.
 * Input:
 *      unit            - Unit ID
 * Output:
 *      pEmpty_portmask   - pointer empty port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
rtksw_api_ret_t rtksw_trunk_portQueueEmpty_get(rtksw_uint32 unit, rtksw_portmask_t *pEmpty_portmask)
{
    rtksw_api_ret_t retVal;

    RTKSW_CHK_UNIT_ID(unit);
    RTKSW_CHK_RT_MAPPER(unit);
	
    if (NULL == RT_MAPPER->trunk_portQueueEmpty_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTKSW_API_LOCK(unit);
    retVal = RT_MAPPER->trunk_portQueueEmpty_get(unit, pEmpty_portmask);
    RTKSW_API_UNLOCK(unit);

    return retVal;
}


