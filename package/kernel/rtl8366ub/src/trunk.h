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

#ifndef __RTKSW_API_TRUNK_H__
#define __RTKSW_API_TRUNK_H__

/*
 * Data Type Declaration
 */
#define    RTKSW_TRUNK_DPORT_HASH_MASK     0x40
#define    RTKSW_TRUNK_SPORT_HASH_MASK     0x20
#define    RTKSW_TRUNK_DIP_HASH_MASK       0x10
#define    RTKSW_TRUNK_SIP_HASH_MASK       0x8
#define    RTKSW_TRUNK_DMAC_HASH_MASK      0x4
#define    RTKSW_TRUNK_SMAC_HASH_MASK      0x2
#define    RTKSW_TRUNK_SPA_HASH_MASK       0x1


#define RTKSW_MAX_NUM_OF_TRUNK_HASH_VAL               16

typedef struct  rtksw_trunk_hashVal2Port_s
{
    rtksw_uint8 value[RTKSW_MAX_NUM_OF_TRUNK_HASH_VAL];
} rtksw_trunk_hashVal2Port_t;

typedef enum rtksw_trunk_group_e
{
    TRUNK_GROUP0 = 0,
    TRUNK_GROUP1,
    TRUNK_GROUP2,
    TRUNK_GROUP3,
    TRUNK_GROUP_END
} rtksw_trunk_group_t;

typedef enum rtksw_trunk_separateType_e
{
    SEPARATE_NONE = 0,
    SEPARATE_FLOOD,
    SEPARATE_END

} rtksw_trunk_separateType_t;

typedef enum rtksw_trunk_mode_e
{
    TRUNK_MODE_NORMAL = 0,
    TRUNK_MODE_DUMB,
    TRUNK_MODE_END
} rtksw_trunk_mode_t;

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
extern rtksw_api_ret_t rtksw_trunk_port_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_portmask_t *pTrunk_member_portmask);

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
extern rtksw_api_ret_t rtksw_trunk_port_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_portmask_t *pTrunk_member_portmask);

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
extern rtksw_api_ret_t rtksw_trunk_distributionAlgorithm_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_uint32 algo_bitmask);

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
extern rtksw_api_ret_t rtksw_trunk_distributionAlgorithm_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_uint32 *pAlgo_bitmask);

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
extern rtksw_api_ret_t rtksw_trunk_trafficSeparate_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_separateType_t separateType);

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
extern rtksw_api_ret_t rtksw_trunk_trafficSeparate_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_separateType_t *pSeparateType);

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
extern rtksw_api_ret_t rtksw_trunk_mode_set(rtksw_uint32 unit, rtksw_trunk_mode_t mode);

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
extern rtksw_api_ret_t rtksw_trunk_mode_get(rtksw_uint32 unit, rtksw_trunk_mode_t *pMode);

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
extern rtksw_api_ret_t rtksw_trunk_distributionMode_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_mode_t mode);

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
extern rtksw_api_ret_t rtksw_trunk_distributionMode_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_mode_t *pMode);

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
extern rtksw_api_ret_t rtksw_trunk_trafficPause_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_enable_t enable);

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
extern rtksw_api_ret_t rtksw_trunk_trafficPause_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_enable_t *pEnable);

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
extern rtksw_api_ret_t rtksw_trunk_hashMappingTable_set(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_hashVal2Port_t *pHash2Port_array);

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
extern rtksw_api_ret_t rtksw_trunk_hashMappingTable_get(rtksw_uint32 unit, rtksw_trunk_group_t trk_gid, rtksw_trunk_hashVal2Port_t *pHash2Port_array);

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
extern rtksw_api_ret_t rtksw_trunk_portQueueEmpty_get(rtksw_uint32 unit, rtksw_portmask_t *pEmpty_portmask);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_trunk_port_set(trk_gid, pTrunk_member_portmask)         rtksw_trunk_port_set(0, trk_gid, pTrunk_member_portmask)
#define rtk_trunk_port_get(trk_gid, pTrunk_member_portmask)         rtksw_trunk_port_get(0, trk_gid, pTrunk_member_portmask)
#define rtk_trunk_distributionAlgorithm_set(trk_gid, algo_bitmask)  rtksw_trunk_distributionAlgorithm_set(0, trk_gid, algo_bitmask)
#define rtk_trunk_distributionAlgorithm_get(trk_gid, pAlgo_bitmask) rtksw_trunk_distributionAlgorithm_get(0, trk_gid, pAlgo_bitmask)
#define rtk_trunk_trafficSeparate_set(trk_gid, separateType)        rtksw_trunk_trafficSeparate_set(0, trk_gid, separateType)
#define rtk_trunk_trafficSeparate_get(trk_gid, pSeparateType)       rtksw_trunk_trafficSeparate_get(0, trk_gid, pSeparateType)
#define rtk_trunk_mode_set(mode)                                    rtksw_trunk_mode_set(0, mode)
#define rtk_trunk_mode_get(pMode)                                   rtksw_trunk_mode_get(0, pMode)
#define rtk_trunk_distributionMode_set(trk_gid, mode)               rtksw_trunk_distributionMode_set(0, trk_gid, mode)
#define rtk_trunk_distributionMode_get(trk_gid, pMode)              rtksw_trunk_distributionMode_get(0, trk_gid, pMode)
#define rtk_trunk_trafficPause_set(trk_gid, enable)                 rtksw_trunk_trafficPause_set(0, trk_gid, enable)
#define rtk_trunk_trafficPause_get(trk_gid, pEnable)                rtksw_trunk_trafficPause_get(0, trk_gid, pEnable)
#define rtk_trunk_hashMappingTable_set(trk_gid, pHash2Port_array)   rtksw_trunk_hashMappingTable_set(0, trk_gid, pHash2Port_array)
#define rtk_trunk_hashMappingTable_get(trk_gid, pHash2Port_array)   rtksw_trunk_hashMappingTable_get(0, trk_gid, pHash2Port_array)
#define rtk_trunk_portQueueEmpty_get(pEmpty_portmask)               rtksw_trunk_portQueueEmpty_get(0, pEmpty_portmask)

#define RTK_TRUNK_DPORT_HASH_MASK       RTKSW_TRUNK_DPORT_HASH_MASK
#define RTK_TRUNK_SPORT_HASH_MASK       RTKSW_TRUNK_SPORT_HASH_MASK
#define RTK_TRUNK_DIP_HASH_MASK         RTKSW_TRUNK_DIP_HASH_MASK  
#define RTK_TRUNK_SIP_HASH_MASK         RTKSW_TRUNK_SIP_HASH_MASK  
#define RTK_TRUNK_DMAC_HASH_MASK        RTKSW_TRUNK_DMAC_HASH_MASK 
#define RTK_TRUNK_SMAC_HASH_MASK        RTKSW_TRUNK_SMAC_HASH_MASK 
#define RTK_TRUNK_SPA_HASH_MASK         RTKSW_TRUNK_SPA_HASH_MASK  

#define RTK_MAX_NUM_OF_TRUNK_HASH_VAL   RTKSW_MAX_NUM_OF_TRUNK_HASH_VAL

#define rtk_trunk_hashVal2Port_t        rtksw_trunk_hashVal2Port_t
#define rtk_trunk_group_t               rtksw_trunk_group_t
#define rtk_trunk_separateType_t        rtksw_trunk_separateType_t
#define rtk_trunk_mode_t                rtksw_trunk_mode_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_TRUNK_H__ */
