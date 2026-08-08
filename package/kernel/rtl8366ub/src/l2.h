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

#ifndef __RTKSW_API_L2_H__
#define __RTKSW_API_L2_H__


/*
 * Data Type Declaration
 */
#define RTKSW_MAX_NUM_OF_LEARN_LIMIT                  (rtksw_switch_maxLutAddrNumber_get(unit))

#define RTKSW_MAC_ADDR_LEN                            6
#define RTKSW_MAX_LUT_ADDRESS                         (RTKSW_MAX_NUM_OF_LEARN_LIMIT)
#define RTKSW_MAX_LUT_ADDR_ID                         (RTKSW_MAX_LUT_ADDRESS - 1)

typedef rtksw_uint32 rtksw_l2_age_time_t;

typedef enum rtksw_l2_flood_type_e
{
    FLOOD_UNKNOWNDA = 0,
    FLOOD_UNKNOWNMC,
    FLOOD_BC,
    FLOOD_END
} rtksw_l2_flood_type_t;

typedef rtksw_uint32 rtksw_l2_flushItem_t;

typedef enum rtksw_l2_flushType_e
{
    FLUSH_TYPE_BY_PORT = 0,       /* physical port       */
    FLUSH_TYPE_BY_PORT_VID,       /* physical port + VID */
    FLUSH_TYPE_BY_PORT_FID,       /* physical port + FID */
    FLUSH_TYPE_END
} rtksw_l2_flushType_t;

typedef struct rtksw_l2_flushCfg_s
{
    rtksw_enable_t    flushByVid;
    rtksw_vlan_t      vid;
    rtksw_enable_t    flushByFid;
    rtksw_uint32      fid;
    rtksw_enable_t    flushByPort;
    rtksw_port_t      port;
    rtksw_enable_t    flushByMac;
    rtksw_mac_t       ucastAddr;
    rtksw_enable_t    flushStaticAddr;
    rtksw_enable_t    flushAddrOnAllPorts; /* this is used when flushByVid */
} rtksw_l2_flushCfg_t;

typedef enum rtksw_l2_read_method_e{

    READMETHOD_MAC = 0,
    READMETHOD_ADDRESS,
    READMETHOD_NEXT_ADDRESS,
    READMETHOD_NEXT_L2UC,
    READMETHOD_NEXT_L2MC,
    READMETHOD_NEXT_L3MC,
    READMETHOD_NEXT_L2L3MC,
    READMETHOD_NEXT_L2UCSPA,
    READMETHOD_END
}rtksw_l2_read_method_t;

/* l2 limit learning count action */
typedef enum rtksw_l2_limitLearnCntAction_e
{
    RTKSW_LIMIT_LEARN_CNT_ACTION_DROP = 0,
    RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD,
    RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU,
    RTKSW_LIMIT_LEARN_CNT_ACTION_END
} rtksw_l2_limitLearnCntAction_t;

typedef enum rtksw_l2_ipmc_lookup_type_e
{
    LOOKUP_MAC = 0,
    LOOKUP_IP,
    LOOKUP_IP_VID,
    LOOKUP_END
} rtksw_l2_ipmc_lookup_type_t;

/* l2 address table - unicast data structure */
typedef struct rtksw_l2_ucastAddr_s
{
    rtksw_mac_t       mac;
    rtksw_uint32      ivl;
    rtksw_uint32      cvid;
    rtksw_uint32      fid;
    rtksw_uint32      efid;
    rtksw_uint32      port;
    rtksw_uint32      sa_block;
    rtksw_uint32      da_block;
    rtksw_uint32      auth;
    rtksw_uint32      is_static;
    rtksw_uint32      priority;
    rtksw_uint32      sa_pri_en;
    rtksw_uint32      fwd_pri_en;
    rtksw_uint32      address;
}rtksw_l2_ucastAddr_t;

/* l2 address table - multicast data structure */
typedef struct rtksw_l2_mcastAddr_s
{
    rtksw_uint32      vid;
    rtksw_mac_t       mac;
    rtksw_uint32      fid;
    rtksw_portmask_t  portmask;
    rtksw_uint32      ivl;
    rtksw_uint32      priority;
    rtksw_uint32      fwd_pri_en;
    rtksw_uint32      igmp_asic;
    rtksw_uint32      igmp_index;
    rtksw_uint32      address;
}rtksw_l2_mcastAddr_t;

/* l2 address table - ip multicast data structure */
typedef struct rtksw_l2_ipMcastAddr_s
{
    ipaddr_t        dip;
    ipaddr_t        sip;
    rtksw_portmask_t  portmask;
    rtksw_uint32      priority;
    rtksw_uint32      fwd_pri_en;
    rtksw_uint32      igmp_asic;
    rtksw_uint32      igmp_index;
    rtksw_uint32      address;
}rtksw_l2_ipMcastAddr_t;

/* l2 address table - ip VID multicast data structure */
typedef struct rtksw_l2_ipVidMcastAddr_s
{
    ipaddr_t        dip;
    ipaddr_t        sip;
    rtksw_uint32      vid;
    rtksw_portmask_t  portmask;
    rtksw_uint32      address;
}rtksw_l2_ipVidMcastAddr_t;

typedef struct rtksw_l2_addr_table_s
{
    rtksw_uint32  index;
    ipaddr_t    sip;
    ipaddr_t    dip;
    rtksw_mac_t   mac;
    rtksw_uint32  sa_block;
    rtksw_uint32  auth;
    rtksw_portmask_t  portmask;
    rtksw_uint32  age;
    rtksw_uint32  ivl;
    rtksw_uint32  cvid;
    rtksw_uint32  fid;
    rtksw_uint32  is_ipmul;
    rtksw_uint32  is_static;
    rtksw_uint32  is_ipvidmul;
    rtksw_uint32  l3_vid;
}rtksw_l2_addr_table_t;

typedef enum rtksw_l2_clearStatus_e
{
    L2_CLEAR_STATE_FINISH = 0,
    L2_CLEAR_STATE_BUSY,
    L2_CLEAR_STATE_END
}rtksw_l2_clearStatus_t;

typedef enum rtksw_l2_lookupHitIsolationAction_e
{
    L2_LOOKUPHIT_ISOACTION_NOP = 0,
    L2_LOOKUPHIT_ISOACTION_UNKNOWN,
    L2_LOOKUPHIT_ISOACTION_END
}rtksw_l2_lookupHitIsolationAction_t;

/* Function Name:
 *      rtksw_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *      Initialize l2 module before calling any l2 APIs.
 */
extern rtksw_api_ret_t rtksw_l2_init(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_l2_addr_add
 * Description:
 *      Add LUT unicast entry.
 * Input:
 *      unit        - Unit ID
 *      pMac        - 6 bytes unicast(I/G bit is 0) mac address to be written into LUT.
 *      pL2_data    - Unicast entry parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_MAC              - Invalid MAC address.
 *      RT_ERR_L2_FID           - Invalid FID .
 *      RT_ERR_L2_INDEXTBL_FULL - hashed index is full of entries.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      If the unicast mac address already existed in LUT, it will udpate the status of the entry.
 *      Otherwise, it will find an empty or asic auto learned entry to write. If all the entries
 *      with the same hash value can't be replaced, ASIC will return a RT_ERR_L2_INDEXTBL_FULL error.
 */
extern rtksw_api_ret_t rtksw_l2_addr_add(rtksw_uint32 unit, rtksw_mac_t *pMac, rtksw_l2_ucastAddr_t *pL2_data);

/* Function Name:
 *      rtksw_l2_addr_get
 * Description:
 *      Get LUT unicast entry.
 * Input:
 *      unit    - Unit ID
 *      pMac    - 6 bytes unicast(I/G bit is 0) mac address to be written into LUT.
 * Output:
 *      pL2_data - Unicast entry parameter
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
extern rtksw_api_ret_t rtksw_l2_addr_get(rtksw_uint32 unit, rtksw_mac_t *pMac, rtksw_l2_ucastAddr_t *pL2_data);

/* Function Name:
 *      rtksw_l2_addr_next_get
 * Description:
 *      Get Next LUT unicast entry.
 * Input:
 *      unit            - Unit ID
 *      read_method     - The reading method.
 *      port            - The port number if the read_metohd is READMETHOD_NEXT_L2UCSPA
 *      pAddress        - The Address ID
 * Output:
 *      pL2_data - Unicast entry parameter
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      Get the next unicast entry after the current entry pointed by pAddress.
 *      The address of next entry is returned by pAddress. User can use (address + 1)
 *      as pAddress to call this API again for dumping all entries is LUT.
 */
extern rtksw_api_ret_t rtksw_l2_addr_next_get(rtksw_uint32 unit, rtksw_l2_read_method_t read_method, rtksw_port_t port, rtksw_uint32 *pAddress, rtksw_l2_ucastAddr_t *pL2_data);

/* Function Name:
 *      rtksw_l2_addr_del
 * Description:
 *      Delete LUT unicast entry.
 * Input:
 *      unit    - Unit ID
 *      pMac    - 6 bytes unicast(I/G bit is 0) mac address to be written into LUT.
 *      fid     - Filtering database
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
extern rtksw_api_ret_t rtksw_l2_addr_del(rtksw_uint32 unit, rtksw_mac_t *pMac, rtksw_l2_ucastAddr_t *pL2_data);

/* Function Name:
 *      rtksw_l2_mcastAddr_add
 * Description:
 *      Add LUT multicast entry.
 * Input:
 *      unit        - Unit ID
 *      pMcastAddr  - L2 multicast entry structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_MAC              - Invalid MAC address.
 *      RT_ERR_L2_FID           - Invalid FID .
 *      RT_ERR_L2_VID           - Invalid VID .
 *      RT_ERR_L2_INDEXTBL_FULL - hashed index is full of entries.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      If the multicast mac address already existed in the LUT, it will udpate the
 *      port mask of the entry. Otherwise, it will find an empty or asic auto learned
 *      entry to write. If all the entries with the same hash value can't be replaced,
 *      ASIC will return a RT_ERR_L2_INDEXTBL_FULL error.
 */
extern rtksw_api_ret_t rtksw_l2_mcastAddr_add(rtksw_uint32 unit, rtksw_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      rtksw_l2_mcastAddr_get
 * Description:
 *      Get LUT multicast entry.
 * Input:
 *      unit        - Unit ID
 *      pMcastAddr  - L2 multicast entry structure
 * Output:
 *      pMcastAddr  - L2 multicast entry structure
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_VID               - Invalid VID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      If the multicast mac address existed in the LUT, it will return the port where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
extern rtksw_api_ret_t rtksw_l2_mcastAddr_get(rtksw_uint32 unit, rtksw_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      rtksw_l2_mcastAddr_next_get
 * Description:
 *      Get Next L2 Multicast entry.
 * Input:
 *      unit        - Unit ID
 *      pAddress    - The Address ID
 * Output:
 *      pMcastAddr  - L2 multicast entry structure
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      Get the next L2 multicast entry after the current entry pointed by pAddress.
 *      The address of next entry is returned by pAddress. User can use (address + 1)
 *      as pAddress to call this API again for dumping all multicast entries is LUT.
 */
extern rtksw_api_ret_t rtksw_l2_mcastAddr_next_get(rtksw_uint32 unit, rtksw_uint32 *pAddress, rtksw_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      rtksw_l2_mcastAddr_del
 * Description:
 *      Delete LUT multicast entry.
 * Input:
 *      unit        - Unit ID
 *      pMcastAddr  - L2 multicast entry structure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_MAC                  - Invalid MAC address.
 *      RT_ERR_L2_FID               - Invalid FID .
 *      RT_ERR_L2_VID               - Invalid VID .
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
extern rtksw_api_ret_t rtksw_l2_mcastAddr_del(rtksw_uint32 unit, rtksw_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      rtksw_l2_ipMcastAddr_add
 * Description:
 *      Add Lut IP multicast entry
 * Input:
 *      unit            - Unit ID
 *      pIpMcastAddr    - IP Multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_L2_INDEXTBL_FULL - hashed index is full of entries.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      System supports L2 entry with IP multicast DIP/SIP to forward IP multicasting frame as user
 *      desired. If this function is enabled, then system will be looked up L2 IP multicast entry to
 *      forward IP multicast frame directly without flooding.
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastAddr_add(rtksw_uint32 unit, rtksw_l2_ipMcastAddr_t *pIpMcastAddr);

/* Function Name:
 *      rtksw_l2_ipMcastAddr_get
 * Description:
 *      Get LUT IP multicast entry.
 * Input:
 *      unit            - Unit ID
 *      pIpMcastAddr    - IP Multicast entry
 * Output:
 *      pIpMcastAddr    - IP Multicast entry
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      The API can get Lut table of IP multicast entry.
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastAddr_get(rtksw_uint32 unit, rtksw_l2_ipMcastAddr_t *pIpMcastAddr);

/* Function Name:
 *      rtksw_l2_ipMcastAddr_next_get
 * Description:
 *      Get Next IP Multicast entry.
 * Input:
 *      unit            - Unit ID
 *      pAddress        - The Address ID
 * Output:
 *      pIpMcastAddr    - IP Multicast entry
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      Get the next IP multicast entry after the current entry pointed by pAddress.
 *      The address of next entry is returned by pAddress. User can use (address + 1)
 *      as pAddress to call this API again for dumping all IP multicast entries is LUT.
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastAddr_next_get(rtksw_uint32 unit, rtksw_uint32 *pAddress, rtksw_l2_ipMcastAddr_t *pIpMcastAddr);

/* Function Name:
 *      rtksw_l2_ipMcastAddr_del
 * Description:
 *      Delete a ip multicast address entry from the specified device.
 * Input:
 *      unit            - Unit ID
 *      pIpMcastAddr    - IP Multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      The API can delete a IP multicast address entry from the specified device.
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastAddr_del(rtksw_uint32 unit, rtksw_l2_ipMcastAddr_t *pIpMcastAddr);

/* Function Name:
 *      rtksw_l2_ipVidMcastAddr_add
 * Description:
 *      Add Lut IP multicast+VID entry
 * Input:
 *      unit                - Unit ID
 *      pIpVidMcastAddr     - IP & VID multicast Entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_L2_INDEXTBL_FULL - hashed index is full of entries.
 *      RT_ERR_PORT_MASK        - Invalid portmask.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_l2_ipVidMcastAddr_add(rtksw_uint32 unit, rtksw_l2_ipVidMcastAddr_t *pIpVidMcastAddr);

/* Function Name:
 *      rtksw_l2_ipVidMcastAddr_get
 * Description:
 *      Get LUT IP multicast+VID entry.
 * Input:
 *      unit            - Unit ID
 *      pIpVidMcastAddr - IP & VID multicast Entry
 * Output:
 *      pIpVidMcastAddr - IP & VID multicast Entry
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_l2_ipVidMcastAddr_get(rtksw_uint32 unit, rtksw_l2_ipVidMcastAddr_t *pIpVidMcastAddr);

/* Function Name:
 *      rtksw_l2_ipVidMcastAddr_next_get
 * Description:
 *      Get Next IP Multicast+VID entry.
 * Input:
 *      unit        - Unit ID
 *      pAddress    - The Address ID
 * Output:
 *      pIpVidMcastAddr - IP & VID multicast Entry
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      Get the next IP multicast entry after the current entry pointed by pAddress.
 *      The address of next entry is returned by pAddress. User can use (address + 1)
 *      as pAddress to call this API again for dumping all IP multicast entries is LUT.
 */
extern rtksw_api_ret_t rtksw_l2_ipVidMcastAddr_next_get(rtksw_uint32 unit, rtksw_uint32 *pAddress, rtksw_l2_ipVidMcastAddr_t *pIpVidMcastAddr);

/* Function Name:
 *      rtksw_l2_ipVidMcastAddr_del
 * Description:
 *      Delete a ip multicast+VID address entry from the specified device.
 * Input:
 *      unit            - Unit ID
 *      pIpVidMcastAddr - IP & VID multicast Entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_L2_ENTRY_NOTFOUND    - No such LUT entry.
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_l2_ipVidMcastAddr_del(rtksw_uint32 unit, rtksw_l2_ipVidMcastAddr_t *pIpVidMcastAddr);

/* Function Name:
 *      rtksw_l2_ucastAddr_flush
 * Description:
 *      Flush L2 mac address by type in the specified device (both dynamic and static).
 * Input:
 *      unit        - Unit ID
 *      pConfig     - flush configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      flushByVid          - 1: Flush by VID, 0: Don't flush by VID
 *      vid                 - VID (0 ~ 4095)
 *      flushByFid          - 1: Flush by FID, 0: Don't flush by FID
 *      fid                 - FID (0 ~ 15)
 *      flushByPort         - 1: Flush by Port, 0: Don't flush by Port
 *      port                - Port ID
 *      flushByMac          - Not Supported
 *      ucastAddr           - Not Supported
 *      flushStaticAddr     - 1: Flush both Static and Dynamic entries, 0: Flush only Dynamic entries
 *      flushAddrOnAllPorts - 1: Flush VID-matched entries at all ports, 0: Flush VID-matched entries per port.
 */
extern rtksw_api_ret_t rtksw_l2_ucastAddr_flush(rtksw_uint32 unit, rtksw_l2_flushCfg_t *pConfig);

/* Function Name:
 *      rtksw_l2_table_clear
 * Description:
 *      Flush all static & dynamic entries in LUT.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_l2_table_clear(rtksw_uint32 unit);

/* Function Name:
 *      rtksw_l2_table_clearStatus_get
 * Description:
 *      Get table clear status
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pStatus     - Clear status, 1:Busy, 0:finish
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_l2_table_clearStatus_get(rtksw_uint32 unit, rtksw_l2_clearStatus_t *pStatus);

/* Function Name:
 *      rtksw_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      enable      - link down flush status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      The status of flush linkdown port address is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
extern rtksw_api_ret_t rtksw_l2_flushLinkDownPortAddrEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pEnable - link down flush status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The status of flush linkdown port address is as following:
 *      - RTKSW_DISABLED
 *      - RTKSW_ENABLED
 */
extern rtksw_api_ret_t rtksw_l2_flushLinkDownPortAddrEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_l2_agingEnable_set
 * Description:
 *      Set L2 LUT aging status per port setting.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      enable  - Aging status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid enable input.
 * Note:
 *      This API can be used to set L2 LUT aging status per port.
 */
extern rtksw_api_ret_t rtksw_l2_agingEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      rtksw_l2_agingEnable_get
 * Description:
 *      Get L2 LUT aging status per port setting.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pEnable - Aging status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can be used to get L2 LUT aging function per port.
 */
extern rtksw_api_ret_t rtksw_l2_agingEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      rtksw_l2_limitLearningCnt_set
 * Description:
 *      Set per-Port auto learning limit number
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      mac_cnt     - Auto learning entries limit number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_PORT_ID              - Invalid port number.
 *      RT_ERR_LIMITED_L2ENTRY_NUM  - Invalid auto learning limit number
 * Note:
 *      The API can set per-port ASIC auto learning limit number from 0(disable learning)
 *      to 2112.
 */
extern rtksw_api_ret_t rtksw_l2_limitLearningCnt_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_cnt_t mac_cnt);

/* Function Name:
 *      rtksw_l2_limitLearningCnt_get
 * Description:
 *      Get per-Port auto learning limit number
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pMac_cnt - Auto learning entries limit number
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get per-port ASIC auto learning limit number.
 */
extern rtksw_api_ret_t rtksw_l2_limitLearningCnt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_cnt_t *pMac_cnt);

/* Function Name:
 *      rtksw_l2_limitSystemLearningCnt_set
 * Description:
 *      Set System auto learning limit number
 * Input:
 *      unit        - Unit ID
 *      mac_cnt     - Auto learning entries limit number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_LIMITED_L2ENTRY_NUM  - Invalid auto learning limit number
 * Note:
 *      The API can set system ASIC auto learning limit number from 0(disable learning)
 *      to 2112.
 */
extern rtksw_api_ret_t rtksw_l2_limitSystemLearningCnt_set(rtksw_uint32 unit, rtksw_mac_cnt_t mac_cnt);

/* Function Name:
 *      rtksw_l2_limitSystemLearningCnt_get
 * Description:
 *      Get System auto learning limit number
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pMac_cnt - Auto learning entries limit number
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get system ASIC auto learning limit number.
 */
extern rtksw_api_ret_t rtksw_l2_limitSystemLearningCnt_get(rtksw_uint32 unit, rtksw_mac_cnt_t *pMac_cnt);

/* Function Name:
 *      rtksw_l2_limitLearningCntAction_set
 * Description:
 *      Configure auto learn over limit number action.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      action      - Auto learning entries limit number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_NOT_ALLOWED  - Invalid learn over action
 * Note:
 *      The API can set SA unknown packet action while auto learn limit number is over
 *      The action symbol as following:
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_DROP,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU,
 */
extern rtksw_api_ret_t rtksw_l2_limitLearningCntAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_l2_limitLearnCntAction_t action);

/* Function Name:
 *      rtksw_l2_limitLearningCntAction_get
 * Description:
 *      Get auto learn over limit number action.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pAction - Learn over action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get SA unknown packet action while auto learn limit number is over
 *      The action symbol as following:
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_DROP,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU,
 */
extern rtksw_api_ret_t rtksw_l2_limitLearningCntAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_l2_limitLearnCntAction_t *pAction);

/* Function Name:
 *      rtksw_l2_limitSystemLearningCntAction_set
 * Description:
 *      Configure system auto learn over limit number action.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      action      - Auto learning entries limit number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_NOT_ALLOWED  - Invalid learn over action
 * Note:
 *      The API can set SA unknown packet action while auto learn limit number is over
 *      The action symbol as following:
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_DROP,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU,
 */
extern rtksw_api_ret_t rtksw_l2_limitSystemLearningCntAction_set(rtksw_uint32 unit, rtksw_l2_limitLearnCntAction_t action);

/* Function Name:
 *      rtksw_l2_limitSystemLearningCntAction_get
 * Description:
 *      Get system auto learn over limit number action.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pAction - Learn over action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get SA unknown packet action while auto learn limit number is over
 *      The action symbol as following:
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_DROP,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD,
 *      - RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU,
 */
extern rtksw_api_ret_t rtksw_l2_limitSystemLearningCntAction_get(rtksw_uint32 unit, rtksw_l2_limitLearnCntAction_t *pAction);

/* Function Name:
 *      rtksw_l2_limitSystemLearningCntPortMask_set
 * Description:
 *      Configure system auto learn portmask
 * Input:
 *      unit        - Unit ID
 *      pPortmask   - Port Mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK    - Invalid port mask.
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_l2_limitSystemLearningCntPortMask_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_l2_limitSystemLearningCntPortMask_get
 * Description:
 *      get system auto learn portmask
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pPortmask   - Port Mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - Null pointer.
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_l2_limitSystemLearningCntPortMask_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_l2_learningCnt_get
 * Description:
 *      Get per-Port current auto learning number
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pMac_cnt - ASIC auto learning entries number
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get per-port ASIC auto learning number
 */
extern rtksw_api_ret_t rtksw_l2_learningCnt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_cnt_t *pMac_cnt);

/* Function Name:
 *      rtksw_l2_floodPortMask_set
 * Description:
 *      Set flooding portmask
 * Input:
 *      unit        - Unit ID
 *      type        - flooding type.
 *      pFlood_portmask - flooding porkmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_MASK    - Invalid portmask.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This API can set the flooding mask.
 *      The flooding type is as following:
 *      - FLOOD_UNKNOWNDA
 *      - FLOOD_UNKNOWNMC
 *      - FLOOD_BC
 */
extern rtksw_api_ret_t rtksw_l2_floodPortMask_set(rtksw_uint32 unit, rtksw_l2_flood_type_t floood_type, rtksw_portmask_t *pFlood_portmask);

/* Function Name:
 *      rtksw_l2_floodPortMask_get
 * Description:
 *      Get flooding portmask
 * Input:
 *      unit        - Unit ID
 *      type        - flooding type.
 * Output:
 *      pFlood_portmask - flooding porkmask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API can get the flooding mask.
 *      The flooding type is as following:
 *      - FLOOD_UNKNOWNDA
 *      - FLOOD_UNKNOWNMC
 *      - FLOOD_BC
 */
extern rtksw_api_ret_t rtksw_l2_floodPortMask_get(rtksw_uint32 unit, rtksw_l2_flood_type_t floood_type, rtksw_portmask_t *pFlood_portmask);

/* Function Name:
 *      rtksw_l2_localPktPermit_set
 * Description:
 *      Set permittion of frames if source port and destination port are the same.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 *      permit      - permittion status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 *      RT_ERR_ENABLE       - Invalid permit value.
 * Note:
 *      This API is setted to permit frame if its source port is equal to destination port.
 */
extern rtksw_api_ret_t rtksw_l2_localPktPermit_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t permit);

/* Function Name:
 *      rtksw_l2_localPktPermit_get
 * Description:
 *      Get permittion of frames if source port and destination port are the same.
 * Input:
 *      unit        - Unit ID
 *      port        - Port id.
 * Output:
 *      pPermit - permittion status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      This API is to get permittion status for frames if its source port is equal to destination port.
 */
extern rtksw_api_ret_t rtksw_l2_localPktPermit_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pPermit);

/* Function Name:
 *      rtksw_l2_aging_set
 * Description:
 *      Set LUT agging out speed
 * Input:
 *      unit        - Unit ID
 *      aging_time  - Agging out time.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_OUT_OF_RANGE     - input out of range.
 * Note:
 *      The API can set LUT agging out period for each entry and the range is from 45s to 458s.
 */
extern rtksw_api_ret_t rtksw_l2_aging_set(rtksw_uint32 unit, rtksw_l2_age_time_t aging_time);

/* Function Name:
 *      rtksw_l2_aging_get
 * Description:
 *      Get LUT agging out time
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnable     - Aging status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get LUT agging out period for each entry.
 */
extern rtksw_api_ret_t rtksw_l2_aging_get(rtksw_uint32 unit, rtksw_l2_age_time_t *pAging_time);

/* Function Name:
 *      rtksw_l2_ipMcastAddrLookup_set
 * Description:
 *      Set Lut IP multicast lookup function
 * Input:
 *      unit        - Unit ID
 *      type        - Lookup type for IPMC packet.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *      LOOKUP_MAC      - Lookup by MAC address
 *      LOOKUP_IP       - Lookup by IP address
 *      LOOKUP_IP_VID   - Lookup by IP address & VLAN ID
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastAddrLookup_set(rtksw_uint32 unit, rtksw_l2_ipmc_lookup_type_t type);

/* Function Name:
 *      rtksw_l2_ipMcastAddrLookup_get
 * Description:
 *      Get Lut IP multicast lookup function
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pType - Lookup type for IPMC packet.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *      None.
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastAddrLookup_get(rtksw_uint32 unit, rtksw_l2_ipmc_lookup_type_t *pType);

/* Function Name:
 *      rtksw_l2_ipMcastForwardRouterPort_set
 * Description:
 *      Set IPMC packet forward to rounter port also or not
 * Input:
 *      unit        - Unit ID
 *      enabled     - 1: Inlcude router port, 0, exclude router port
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastForwardRouterPort_set(rtksw_uint32 unit, rtksw_enable_t enabled);

/* Function Name:
 *      rtksw_l2_ipMcastForwardRouterPort_get
 * Description:
 *      Get IPMC packet forward to rounter port also or not
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pEnabled    - 1: Inlcude router port, 0, exclude router port
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastForwardRouterPort_get(rtksw_uint32 unit, rtksw_enable_t *pEnabled);

/* Function Name:
 *      rtksw_l2_ipMcastGroupEntry_add
 * Description:
 *      Add an IP Multicast entry to group table
 * Input:
 *      unit        - Unit ID
 *      ip_addr     - IP address
 *      vid         - VLAN ID
 *      pPortmask   - portmask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 *      RT_ERR_TBL_FULL    - Table Full
 * Note:
 *      Add an entry to IP Multicast Group table.
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastGroupEntry_add(rtksw_uint32 unit, ipaddr_t ip_addr, rtksw_uint32 vid, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_l2_ipMcastGroupEntry_del
 * Description:
 *      Delete an entry from IP Multicast group table
 * Input:
 *      unit        - Unit ID
 *      ip_addr     - IP address
 *      vid         - VLAN ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 *      RT_ERR_TBL_FULL    - Table Full
 * Note:
 *      Delete an entry from IP Multicast group table.
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastGroupEntry_del(rtksw_uint32 unit, ipaddr_t ip_addr, rtksw_uint32 vid);

/* Function Name:
 *      rtksw_l2_ipMcastGroupEntry_get
 * Description:
 *      get an entry from IP Multicast group table
 * Input:
 *      unit        - Unit ID
 *      ip_addr     - IP address
 *      vid         - VLAN ID
 * Output:
 *      pPortmask   - member port mask
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 *      RT_ERR_TBL_FULL    - Table Full
 * Note:
 *      Delete an entry from IP Multicast group table.
 */
extern rtksw_api_ret_t rtksw_l2_ipMcastGroupEntry_get(rtksw_uint32 unit, ipaddr_t ip_addr, rtksw_uint32 vid, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      rtksw_l2_entry_get
 * Description:
 *      Get LUT unicast entry.
 * Input:
 *      unit        - Unit ID
 *      pL2_entry   - Index field in the structure.
 * Output:
 *      pL2_entry   - other fields such as MAC, port, age...
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This API is used to get address by index from 0~2111.
 */
extern rtksw_api_ret_t rtksw_l2_entry_get(rtksw_uint32 unit, rtksw_l2_addr_table_t *pL2_entry);

/* Function Name:
 *      rtksw_l2_lookupHitIsolationAction_set
 * Description:
 *      Set action of lookup hit & isolation.
 * Input:
 *      unit        - Unit ID
 *      action      - The action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      This API is used to configure the action of packet which is lookup hit
 *      in L2 table but the destination port/portmask are not in the port isolation
 *      group.
 */
extern rtksw_api_ret_t rtksw_l2_lookupHitIsolationAction_set(rtksw_uint32 unit, rtksw_l2_lookupHitIsolationAction_t action);

/* Function Name:
 *      rtksw_l2_lookupHitIsolationAction_get
 * Description:
 *      Get action of lookup hit & isolation.
 * Input:
 *      unit        - Unit ID
 * Output:
 *      pAction     - The action
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_NULL_POINTER         - Null pointer
 *      RT_ERR_SMI                  - SMI access error
 *      RT_ERR_INPUT                - Invalid input parameters.
 * Note:
 *      This API is used to get the action of packet which is lookup hit
 *      in L2 table but the destination port/portmask are not in the port isolation
 *      group.
 */
extern rtksw_api_ret_t rtksw_l2_lookupHitIsolationAction_get(rtksw_uint32 unit, rtksw_l2_lookupHitIsolationAction_t *pAction);

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_l2_init(void)                                           rtksw_l2_init(0)
#define rtk_l2_addr_add(pMac, pL2_data)                             rtksw_l2_addr_add(0, pMac, pL2_data)
#define rtk_l2_addr_get(pMac, pL2_data)                             rtksw_l2_addr_get(0, pMac, pL2_data)
#define rtk_l2_addr_next_get(read_method, port, pAddress, pL2_data) rtksw_l2_addr_next_get(0, read_method, port, pAddress, pL2_data)
#define rtk_l2_addr_del(pMac, pL2_data)                             rtksw_l2_addr_del(0, pMac, pL2_data)
#define rtk_l2_mcastAddr_add(pMcastAddr)                            rtksw_l2_mcastAddr_add(0, pMcastAddr)
#define rtk_l2_mcastAddr_get(pMcastAddr)                            rtksw_l2_mcastAddr_get(0, pMcastAddr)
#define rtk_l2_mcastAddr_next_get(pAddress, pMcastAddr)             rtksw_l2_mcastAddr_next_get(0, pAddress, pMcastAddr)
#define rtk_l2_mcastAddr_del(pMcastAddr)                            rtksw_l2_mcastAddr_del(0, pMcastAddr)
#define rtk_l2_ipMcastAddr_add(pIpMcastAddr)                        rtksw_l2_ipMcastAddr_add(0, pIpMcastAddr)
#define rtk_l2_ipMcastAddr_get(pIpMcastAddr)                        rtksw_l2_ipMcastAddr_get(0, pIpMcastAddr)
#define rtk_l2_ipMcastAddr_next_get(pAddress, pIpMcastAddr)         rtksw_l2_ipMcastAddr_next_get(0, pAddress, pIpMcastAddr)
#define rtk_l2_ipMcastAddr_del(pIpMcastAddr)                        rtksw_l2_ipMcastAddr_del(0, pIpMcastAddr)
#define rtk_l2_ipVidMcastAddr_add(pIpVidMcastAddr)                  rtksw_l2_ipVidMcastAddr_add(0, pIpVidMcastAddr)
#define rtk_l2_ipVidMcastAddr_get(pIpVidMcastAddr)                  rtksw_l2_ipVidMcastAddr_get(0, pIpVidMcastAddr)
#define rtk_l2_ipVidMcastAddr_next_get(pAddress, pIpVidMcastAddr)   rtksw_l2_ipVidMcastAddr_next_get(0, pAddress, pIpVidMcastAddr)
#define rtk_l2_ipVidMcastAddr_del(pIpVidMcastAddr)                  rtksw_l2_ipVidMcastAddr_del(0, pIpVidMcastAddr)
#define rtk_l2_ucastAddr_flush(pConfig)                             rtksw_l2_ucastAddr_flush(0, pConfig)
#define rtk_l2_table_clear(void)                                    rtksw_l2_table_clear(0)
#define rtk_l2_table_clearStatus_get(pStatus)                       rtksw_l2_table_clearStatus_get(0, pStatus)
#define rtk_l2_flushLinkDownPortAddrEnable_set(port, enable)        rtksw_l2_flushLinkDownPortAddrEnable_set(0, port, enable)
#define rtk_l2_flushLinkDownPortAddrEnable_get(port, pEnable)       rtksw_l2_flushLinkDownPortAddrEnable_get(0, port, pEnable)
#define rtk_l2_agingEnable_set(port, enable)                        rtksw_l2_agingEnable_set(0, port, enable)
#define rtk_l2_agingEnable_get(port, pEnable)                       rtksw_l2_agingEnable_get(0, port, pEnable)
#define rtk_l2_limitLearningCnt_set(port, mac_cnt)                  rtksw_l2_limitLearningCnt_set(0, port, mac_cnt)
#define rtk_l2_limitLearningCnt_get(port, pMac_cnt)                 rtksw_l2_limitLearningCnt_get(0, port, pMac_cnt)
#define rtk_l2_limitSystemLearningCnt_set(mac_cnt)                  rtksw_l2_limitSystemLearningCnt_set(0, mac_cnt)
#define rtk_l2_limitSystemLearningCnt_get(pMac_cnt)                 rtksw_l2_limitSystemLearningCnt_get(0, pMac_cnt)
#define rtk_l2_limitLearningCntAction_set(port, action)             rtksw_l2_limitLearningCntAction_set(0, port, action)
#define rtk_l2_limitLearningCntAction_get(port, pAction)            rtksw_l2_limitLearningCntAction_get(0, port, pAction)
#define rtk_l2_limitSystemLearningCntAction_set(action)             rtksw_l2_limitSystemLearningCntAction_set(0, action)
#define rtk_l2_limitSystemLearningCntAction_get(pAction)            rtksw_l2_limitSystemLearningCntAction_get(0, pAction)
#define rtk_l2_limitSystemLearningCntPortMask_set(pPortmask)        rtksw_l2_limitSystemLearningCntPortMask_set(0, pPortmask)
#define rtk_l2_limitSystemLearningCntPortMask_get(pPortmask)        rtksw_l2_limitSystemLearningCntPortMask_get(0, pPortmask)
#define rtk_l2_learningCnt_get(port, pMac_cnt)                      rtksw_l2_learningCnt_get(0, port, pMac_cnt)
#define rtk_l2_floodPortMask_set(floood_type, pFlood_portmask)      rtksw_l2_floodPortMask_set(0, floood_type, pFlood_portmask)
#define rtk_l2_floodPortMask_get(floood_type, pFlood_portmask)      rtksw_l2_floodPortMask_get(0, floood_type, pFlood_portmask)
#define rtk_l2_localPktPermit_set(port, permit)                     rtksw_l2_localPktPermit_set(0, port, permit)
#define rtk_l2_localPktPermit_get(port, pPermit)                    rtksw_l2_localPktPermit_get(0, port, pPermit)
#define rtk_l2_aging_set(aging_time)                                rtksw_l2_aging_set(0, aging_time)
#define rtk_l2_aging_get(pAging_time)                               rtksw_l2_aging_get(0, pAging_time)
#define rtk_l2_ipMcastAddrLookup_set(type)                          rtksw_l2_ipMcastAddrLookup_set(0, type)
#define rtk_l2_ipMcastAddrLookup_get(pType)                         rtksw_l2_ipMcastAddrLookup_get(0, pType)
#define rtk_l2_ipMcastForwardRouterPort_set(enabled)                rtksw_l2_ipMcastForwardRouterPort_set(0, enabled)
#define rtk_l2_ipMcastForwardRouterPort_get(pEnabled)               rtksw_l2_ipMcastForwardRouterPort_get(0, pEnabled)
#define rtk_l2_ipMcastGroupEntry_add(ip_addr, vid, pPortmask)       rtksw_l2_ipMcastGroupEntry_add(0, ip_addr, vid, pPortmask)
#define rtk_l2_ipMcastGroupEntry_del(ip_addr, vid)                  rtksw_l2_ipMcastGroupEntry_del(0, ip_addr, vid)
#define rtk_l2_ipMcastGroupEntry_get(ip_addr, vid, pPortmask)       rtksw_l2_ipMcastGroupEntry_get(0, ip_addr, vid, pPortmask)
#define rtk_l2_entry_get(pL2_entry)                                 rtksw_l2_entry_get(0, pL2_entry)
#define rtk_l2_lookupHitIsolationAction_set(action)                 rtksw_l2_lookupHitIsolationAction_set(0, action)
#define rtk_l2_lookupHitIsolationAction_get(pAction)                rtksw_l2_lookupHitIsolationAction_get(0, pAction)

#define RTK_MAC_ADDR_LEN        RTKSW_MAC_ADDR_LEN   
#define RTK_MAX_LUT_ADDRESS     RTKSW_MAX_LUT_ADDRESS
#define RTK_MAX_LUT_ADDR_ID     RTKSW_MAX_LUT_ADDR_ID

#define LIMIT_LEARN_CNT_ACTION_DROP     RTKSW_LIMIT_LEARN_CNT_ACTION_DROP
#define LIMIT_LEARN_CNT_ACTION_FORWARD  RTKSW_LIMIT_LEARN_CNT_ACTION_FORWARD
#define LIMIT_LEARN_CNT_ACTION_TO_CPU   RTKSW_LIMIT_LEARN_CNT_ACTION_TO_CPU

#define rtk_l2_flood_type_t                 rtksw_l2_flood_type_t
#define rtk_l2_flushItem_t                  rtksw_l2_flushItem_t
#define rtk_l2_flushType_t                  rtksw_l2_flushType_t
#define rtk_l2_flushCfg_t                   rtksw_l2_flushCfg_t
#define rtk_l2_read_method_t                rtksw_l2_read_method_t
#define rtk_l2_limitLearnCntAction_t        rtksw_l2_limitLearnCntAction_t
#define rtk_l2_ipmc_lookup_type_t           rtksw_l2_ipmc_lookup_type_t
#define rtk_l2_ucastAddr_t                  rtksw_l2_ucastAddr_t
#define rtk_l2_mcastAddr_t                  rtksw_l2_mcastAddr_t
#define rtk_l2_ipMcastAddr_t                rtksw_l2_ipMcastAddr_t
#define rtk_l2_ipVidMcastAddr_t             rtksw_l2_ipVidMcastAddr_t
#define rtk_l2_addr_table_t                 rtksw_l2_addr_table_t
#define rtk_l2_clearStatus_t                rtksw_l2_clearStatus_t
#define rtk_l2_lookupHitIsolationAction_t   rtksw_l2_lookupHitIsolationAction_t

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* __RTKSW_API_L2_H__ */

