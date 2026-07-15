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
#include <dal/rtl8371c/dal_rtl8371c_trunk.h>
#include <dal/reg.h>
#include <dal/rtl8371c/rtk_rtl8371c_reg_struct.h>

#include <dal/dal_mgmts.h>

/* Function Name:
 *      dal_rtl8371c_trunk_port_set
 * Description:
 *      Set trunking group available port mask
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trunk_port_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_portmask_t *pTrunk_member_portmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmsk, phyPort, trkPortNum;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(NULL == pTrunk_member_portmask)
        return RT_ERR_NULL_POINTER;

    RTKSW_CHK_PORTMASK_VALID(unit, pTrunk_member_portmask);

    if((retVal = rtksw_switch_portmask_L2P_get(unit, pTrunk_member_portmask, &pmsk)) != RT_ERR_OK)
    {
        return retVal;
    }

	trkPortNum = 0;
	RTKSW_SCAN_ALL_PHY_PORTMASK(unit, phyPort)
	{
        if ((pmsk&(1<<phyPort))!=0)
            trkPortNum++;
	}
	
    if (trkPortNum>RTL8371C_MAX_TRKPORT_NUM)
        return RT_ERR_PORT_MASK;    

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_MBR_CTRLr, REG_ARRAY_INDEX_NONE, trk_gid, RTL8371C_TRK_PMSKf, &pmsk)) != RT_ERR_OK)
        return retVal;     

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_port_get
 * Description:
 *      Get trunking group available port mask
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trunk_port_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_portmask_t *pTrunk_member_portmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmsk;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_MBR_CTRLr, REG_ARRAY_INDEX_NONE, trk_gid, RTL8371C_TRK_PMSKf, &pmsk)) != RT_ERR_OK)
        return retVal; 

    if((retVal = rtksw_switch_portmask_P2L_get(unit, pmsk, pTrunk_member_portmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_distributionAlgorithm_set
 * Description:
 *      Set port trunking hash select sources
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trunk_distributionAlgorithm_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_uint32 algo_bitmask)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if (algo_bitmask >= 128)
        return RT_ERR_LA_HASHMASK;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASH_CTRLr, REG_ARRAY_INDEX_NONE, trk_gid, RTL8371C_HASH_MSKf, &algo_bitmask)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_distributionAlgorithm_get
 * Description:
 *      Get port trunking hash select sources
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trunk_distributionAlgorithm_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_uint32 *pAlgo_bitmask)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(NULL == pAlgo_bitmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASH_CTRLr, REG_ARRAY_INDEX_NONE, trk_gid, RTL8371C_HASH_MSKf, pAlgo_bitmask)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_trafficSeparate_set
 * Description:
 *      Set the traffic separation setting of a trunk group from the specified device.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trunk_trafficSeparate_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_separateType_t separateType)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 enabled;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(separateType >= SEPARATE_END)
        return RT_ERR_INPUT;

    enabled = (separateType == SEPARATE_FLOOD) ? RTKSW_ENABLED : RTKSW_DISABLED;
    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_CFGr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_PORT_TRUNK_FLOODf, &enabled)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_trafficSeparate_get
 * Description:
 *      Get the traffic separation setting of a trunk group from the specified device.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trunk_trafficSeparate_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_separateType_t *pSeparateType)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 enabled;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(NULL == pSeparateType)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_CFGr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_PORT_TRUNK_FLOODf, &enabled)) != RT_ERR_OK)
        return retVal; 

    *pSeparateType = (enabled == RTKSW_ENABLED) ? SEPARATE_FLOOD : SEPARATE_NONE;
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_distributionMode_set
 * Description:
 *      Set the trunk mode to the specified device.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trunk_distributionMode_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_mode_t mode)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(mode >= TRUNK_MODE_END)
        return RT_ERR_INPUT;
    
    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_CFGr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_PORT_TRUNK_DUMBf, &mode)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_distributionMode_get
 * Description:
 *      Get the trunk mode from the specified device.
 *  Input:
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
rtksw_api_ret_t dal_rtl8371c_trunk_distributionMode_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_mode_t *pMode)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(NULL == pMode)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_CFGr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_PORT_TRUNK_DUMBf, pMode)) != RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_trafficPause_set
 * Description:
 *      Set the traffic pause setting of a trunk group.
 *  Input:
 *      unit            - Unit ID
 *      trk_gid         - trunk group id
 *      enable          - traffic pause state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID - invalid trunk ID
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_trunk_trafficPause_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_enable_t enable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(enable >= RTKSW_ENABLE_END)
        return RT_ERR_INPUT;

    switch (trk_gid)
    {
        case TRUNK_GROUP0:
            if ((retVal = reg16_field_write(unit, RTL8371C_TRK_FCr, RTL8371C_EN_FLOWCTRL_TG0f, &enable)) != RT_ERR_OK)
                return retVal; 
            break;
        case TRUNK_GROUP1:
            if ((retVal = reg16_field_write(unit, RTL8371C_TRK_FCr, RTL8371C_EN_FLOWCTRL_TG1f, &enable)) != RT_ERR_OK)
                return retVal; 
            break;
        default:
            return RT_ERR_LA_TRUNK_ID;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_trafficPause_get
 * Description:
 *      Get the traffic pause setting of a trunk group.
 *  Input:
 *      unit            - Unit ID
 *      trk_gid         - trunk group id
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
rtksw_api_ret_t dal_rtl8371c_trunk_trafficPause_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_enable_t *pEnable)
{
    rtksw_api_ret_t retVal;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(NULL == pEnable)
        return RT_ERR_NULL_POINTER;

    switch (trk_gid)
    {
        case TRUNK_GROUP0:
            if ((retVal = reg16_field_read(unit, RTL8371C_TRK_FCr, RTL8371C_EN_FLOWCTRL_TG0f, pEnable)) != RT_ERR_OK)
                return retVal; 
            break;
        case TRUNK_GROUP1:
            if ((retVal = reg16_field_read(unit, RTL8371C_TRK_FCr, RTL8371C_EN_FLOWCTRL_TG1f, pEnable)) != RT_ERR_OK)
                return retVal; 
            break;
        default:
            return RT_ERR_LA_TRUNK_ID;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_hashMappingTable_set
 * Description:
 *      Set hash value to port array in the trunk group id from the specified device.
 *  Input:
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
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_trunk_hashMappingTable_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_hashVal2Port_t *pHash2Port_array)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 hashPort[RTKSW_MAX_NUM_OF_TRUNK_HASH_VAL];
    rtksw_uint32 i;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(NULL == pHash2Port_array)
        return RT_ERR_NULL_POINTER;

    for(i = 0; i < RTKSW_MAX_NUM_OF_TRUNK_HASH_VAL; i++)
    {
        hashPort[i] = rtksw_switch_port_L2P_get(unit, pHash2Port_array->value[i]);
        if (hashPort[i] == UNDEFINE_PHY_PORT)
            return RT_ERR_PORT_ID;
    }

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH0f, &hashPort[0])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH1f, &hashPort[1])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH2f, &hashPort[2])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH3f, &hashPort[3])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH4f, &hashPort[4])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH5f, &hashPort[5])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH6f, &hashPort[6])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH7f, &hashPort[7])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH8f, &hashPort[8])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH9f, &hashPort[9])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH10f, &hashPort[10])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH11f, &hashPort[11])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH12f, &hashPort[12])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH13f, &hashPort[13])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH14f, &hashPort[14])) != RT_ERR_OK)
        return retVal;

    if ((retVal = reg16_array_field_write(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH15f, &hashPort[15])) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_hashMappingTable_get
 * Description:
 *      Get hash value to port array in the trunk group id from the specified device.
 *  Input:
 *      unit        - Unit ID
 *      trk_gid     - trunk group id
 * Output:
 *      pHash2Port_array - pointer buffer of ports associate with the hash value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
rtksw_api_ret_t dal_rtl8371c_trunk_hashMappingTable_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_hashVal2Port_t *pHash2Port_array)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 hashPort[RTKSW_MAX_NUM_OF_TRUNK_HASH_VAL];
    rtksw_uint32 i;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    /* Check Trunk Group Valid */
    RTKSW_CHK_TRUNK_GROUP_VALID(unit, trk_gid);

    if(NULL == pHash2Port_array)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH0f, &hashPort[0])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH1f, &hashPort[1])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH2f, &hashPort[2])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH3f, &hashPort[3])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH4f, &hashPort[4])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH5f, &hashPort[5])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH6f, &hashPort[6])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH7f, &hashPort[7])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH8f, &hashPort[8])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH9f, &hashPort[9])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH10f, &hashPort[10])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH11f, &hashPort[11])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH12f, &hashPort[12])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH13f, &hashPort[13])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH14f, &hashPort[14])) != RT_ERR_OK)
        return retVal;
    
    if ((retVal = reg16_array_field_read(unit, RTL8371C_TRK_HASHr, REG_ARRAY_INDEX_NONE, (rtksw_uint32)trk_gid, RTL8371C_HASH15f, &hashPort[15])) != RT_ERR_OK)
        return retVal;
    
    for(i = 0; i < RTKSW_MAX_NUM_OF_TRUNK_HASH_VAL; i++)
    {
        pHash2Port_array->value[i] = rtksw_switch_port_P2L_get(unit, hashPort[i]);
        if (pHash2Port_array->value[i] == UNDEFINE_PORT)
            return RT_ERR_PORT_ID;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8371c_trunk_portQueueEmpty_get
 * Description:
 *      Get the port mask which all queues are empty.
 *  Input:
 *      unit        - Unit ID
 * Output:
 *      pEmpty_portmask   - pointer empty port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
rtksw_api_ret_t dal_rtl8371c_trunk_portQueueEmpty_get(rtksw_uint32 unit, rtksw_portmask_t *pEmpty_portmask)
{
    rtksw_api_ret_t retVal;
    rtksw_uint32 pmask;

    /* Check initialization state */
    RTKSW_CHK_INIT_STATE(unit);

    if(NULL == pEmpty_portmask)
        return RT_ERR_NULL_POINTER;

    if ((retVal = reg16_field_read(unit, RTL8371C_EPMSK_EMPTYr, RTL8371C_PORT_MASKf, &pmask)) != RT_ERR_OK)
        return retVal; 

    if ((retVal = rtksw_switch_portmask_P2L_get(unit, pmask, pEmpty_portmask)) != RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}



