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

#ifndef __DAL_rtl8371C_L2_H__
#define __DAL_rtl8371C_L2_H__

#include <l2.h>

/*
 * Data Type Declaration
 */
#if 0
#define RTKSW_MAX_NUM_OF_LEARN_LIMIT                  (rtksw_switch_maxLutAddrNumber_get())

#define RTKSW_MAC_ADDR_LEN                            6
#define RTKSW_MAX_LUT_ADDRESS                         (RTKSW_MAX_NUM_OF_LEARN_LIMIT)
#define RTKSW_MAX_LUT_ADDR_ID                         (RTKSW_MAX_LUT_ADDRESS - 1)
#endif

#define RTL8371C_LUT_IPMCGRP_TABLE_MAX  (0x3F)

#define RTL8371C_LUT_BUSY_CHECK_NO      (10)

#define RTL8371C_LUT_TABLE_SIZE         (5)

#define RTL8371C_LUTREADMETHOD_MAC            0
#define RTL8371C_LUTREADMETHOD_ADDRESS        1
#define RTL8371C_LUTREADMETHOD_NEXT_ADDRESS   2
#define RTL8371C_LUTREADMETHOD_NEXT_L2UC      3
#define RTL8371C_LUTREADMETHOD_NEXT_L2MC      4
#define RTL8371C_LUTREADMETHOD_NEXT_L3MC      5
#define RTL8371C_LUTREADMETHOD_NEXT_L2L3MC    6
#define RTL8371C_LUTREADMETHOD_NEXT_L2UCSPA   7


#define RTL8371C_FLUSHMDOE_PORT      0
#define RTL8371C_FLUSHMDOE_VID       1
#define RTL8371C_FLUSHMDOE_FID       2
#define RTL8371C_FLUSHMDOE_END       3

#define RTL8371C_FLUSHTYPE_DYNAMIC   0
#define RTL8371C_FLUSHTYPE_BOTH      1
#define RTL8371C_FLUSHTYPE_END       2

#define RTL8371C_LUTAGINGTIMER_MAX   (0xFFFF/10)

#define RTL8371C_LUT_ENTRY_LEN      5

typedef struct LUTTABLE{

    ipaddr_t sip;
    ipaddr_t dip;
    ether_addr_t mac;
    rtksw_uint16 ivl_svl;
    rtksw_uint16 cvid_fid;
    rtksw_uint16 nosalearn;
    rtksw_uint16 mbr;
    rtksw_uint16 spa;
    rtksw_uint16 age;
    rtksw_uint16 l3lookup;

    rtksw_uint16 lookup_hit;
    rtksw_uint16 lookup_busy;
    rtksw_uint16 address;
    rtksw_uint16 wait_time;

}rtl8371c_luttb;

typedef struct rtl8371c_l2_entry_s
{
    rtksw_uint32 entry_data[RTL8371C_LUT_ENTRY_LEN];
}rtl8371c_l2_entry_t;

/* Function Name:
 *      dal_rtl8371c_l2_init
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
extern rtksw_api_ret_t dal_rtl8371c_l2_init(rtksw_uint32 unit);

/* Function Name:
 *      dal_rtl8371c_l2_addr_add
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
extern rtksw_api_ret_t dal_rtl8371c_l2_addr_add(rtksw_uint32 unit, rtksw_mac_t *pMac, rtksw_l2_ucastAddr_t *pL2_data);

/* Function Name:
 *      dal_rtl8371c_l2_addr_get
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
extern rtksw_api_ret_t dal_rtl8371c_l2_addr_get(rtksw_uint32 unit, rtksw_mac_t *pMac, rtksw_l2_ucastAddr_t *pL2_data);

/* Function Name:
 *      dal_rtl8371c_l2_addr_next_get
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
extern rtksw_api_ret_t dal_rtl8371c_l2_addr_next_get(rtksw_uint32 unit, rtksw_l2_read_method_t read_method, rtksw_port_t port, rtksw_uint32 *pAddress, rtksw_l2_ucastAddr_t *pL2_data);

/* Function Name:
 *      dal_rtl8371c_l2_addr_del
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
extern rtksw_api_ret_t dal_rtl8371c_l2_addr_del(rtksw_uint32 unit, rtksw_mac_t *pMac, rtksw_l2_ucastAddr_t *pL2_data);

/* Function Name:
 *      dal_rtl8371c_l2_mcastAddr_add
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
extern rtksw_api_ret_t dal_rtl8371c_l2_mcastAddr_add(rtksw_uint32 unit, rtksw_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      dal_rtl8371c_l2_mcastAddr_get
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
extern rtksw_api_ret_t dal_rtl8371c_l2_mcastAddr_get(rtksw_uint32 unit, rtksw_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      dal_rtl8371c_l2_mcastAddr_next_get
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
extern rtksw_api_ret_t dal_rtl8371c_l2_mcastAddr_next_get(rtksw_uint32 unit, rtksw_uint32 *pAddress, rtksw_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      dal_rtl8371c_l2_mcastAddr_del
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
extern rtksw_api_ret_t dal_rtl8371c_l2_mcastAddr_del(rtksw_uint32 unit, rtksw_l2_mcastAddr_t *pMcastAddr);

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddr_add
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
extern rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddr_add(rtksw_uint32 unit, rtksw_l2_ipMcastAddr_t *pIpMcastAddr);

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddr_get
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
extern rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddr_get(rtksw_uint32 unit, rtksw_l2_ipMcastAddr_t *pIpMcastAddr);

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddr_next_get
 * Description:
 *      Get Next IP Multicast entry.
 * Input:
 *      unit        - Unit ID
 *      pAddress    - The Address ID
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
extern rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddr_next_get(rtksw_uint32 unit, rtksw_uint32 *pAddress, rtksw_l2_ipMcastAddr_t *pIpMcastAddr);

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddr_del
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
extern rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddr_del(rtksw_uint32 unit, rtksw_l2_ipMcastAddr_t *pIpMcastAddr);

/* Function Name:
 *      dal_rtl8371c_l2_ucastAddr_flush
 * Description:
 *      Flush L2 mac address by type in the specified device (both dynamic and static).
 * Input:
 *      unit    - Unit ID
 *      pConfig - flush configuration
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
extern rtksw_api_ret_t dal_rtl8371c_l2_ucastAddr_flush(rtksw_uint32 unit, rtksw_l2_flushCfg_t *pConfig);

/* Function Name:
 *      dal_rtl8371c_l2_table_clear
 * Description:
 *      Flush all static & dynamic entries in LUT.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
extern rtksw_api_ret_t dal_rtl8371c_l2_table_clear(rtksw_uint32 unit);

/* Function Name:
 *      dal_rtl8371c_l2_table_clearStatus_get
 * Description:
 *      Get table clear status
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pStatus - Clear status, 1:Busy, 0:finish
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 * Note:
 *
 */
extern rtksw_api_ret_t dal_rtl8371c_l2_table_clearStatus_get(rtksw_uint32 unit, rtksw_l2_clearStatus_t *pStatus);

/* Function Name:
 *      dal_rtl8371c_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      enable  - link down flush status
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
extern rtksw_api_ret_t dal_rtl8371c_l2_flushLinkDownPortAddrEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      dal_rtl8371c_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration of the specified device.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
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
extern rtksw_api_ret_t dal_rtl8371c_l2_flushLinkDownPortAddrEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_l2_agingEnable_set
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
extern rtksw_api_ret_t dal_rtl8371c_l2_agingEnable_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t enable);

/* Function Name:
 *      dal_rtl8371c_l2_agingEnable_get
 * Description:
 *      Get L2 LUT aging status per port setting.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
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
extern rtksw_api_ret_t dal_rtl8371c_l2_agingEnable_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pEnable);

/* Function Name:
 *      dal_rtl8371c_l2_limitLearningCnt_set
 * Description:
 *      Set per-Port auto learning limit number
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      mac_cnt - Auto learning entries limit number
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
 *      to 8k.
 */
extern rtksw_api_ret_t dal_rtl8371c_l2_limitLearningCnt_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_cnt_t mac_cnt);

/* Function Name:
 *      dal_rtl8371c_l2_limitLearningCnt_get
 * Description:
 *      Get per-Port auto learning limit number
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
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
extern rtksw_api_ret_t dal_rtl8371c_l2_limitLearningCnt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_cnt_t *pMac_cnt);

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCnt_set
 * Description:
 *      Set System auto learning limit number
 * Input:
 *      unit    - Unit ID
 *      mac_cnt - Auto learning entries limit number
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
extern rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCnt_set(rtksw_uint32 unit, rtksw_mac_cnt_t mac_cnt);

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCnt_get
 * Description:
 *      Get System auto learning limit number
 * Input:
 *      unit    - Unit ID
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
extern rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCnt_get(rtksw_uint32 unit, rtksw_mac_cnt_t *pMac_cnt);

/* Function Name:
 *      dal_rtl8371c_l2_limitLearningCntAction_set
 * Description:
 *      Configure auto learn over limit number action.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      action  - Auto learning entries limit number
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
extern rtksw_api_ret_t dal_rtl8371c_l2_limitLearningCntAction_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_l2_limitLearnCntAction_t action);

/* Function Name:
 *      dal_rtl8371c_l2_limitLearningCntAction_get
 * Description:
 *      Get auto learn over limit number action.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
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
extern rtksw_api_ret_t dal_rtl8371c_l2_limitLearningCntAction_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_l2_limitLearnCntAction_t *pAction);

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCntAction_set
 * Description:
 *      Configure system auto learn over limit number action.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      action  - Auto learning entries limit number
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
extern rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCntAction_set(rtksw_uint32 unit, rtksw_l2_limitLearnCntAction_t action);

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCntAction_get
 * Description:
 *      Get system auto learn over limit number action.
 * Input:
 *      unit    - Unit ID
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
extern rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCntAction_get(rtksw_uint32 unit, rtksw_l2_limitLearnCntAction_t *pAction);

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCntPortMask_set
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
extern rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCntPortMask_set(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      dal_rtl8371c_l2_limitSystemLearningCntPortMask_get
 * Description:
 *      get system auto learn portmask
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pPortmask - Port Mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_NULL_POINTER - Null pointer.
 * Note:
 *
 */
extern rtksw_api_ret_t dal_rtl8371c_l2_limitSystemLearningCntPortMask_get(rtksw_uint32 unit, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      dal_rtl8371c_l2_learningCnt_get
 * Description:
 *      Get per-Port current auto learning number
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
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
extern rtksw_api_ret_t dal_rtl8371c_l2_learningCnt_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_mac_cnt_t *pMac_cnt);

/* Function Name:
 *      dal_rtl8371c_l2_floodPortMask_set
 * Description:
 *      Set flooding portmask
 * Input:
 *      unit            - Unit ID
 *      type            - flooding type.
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
extern rtksw_api_ret_t dal_rtl8371c_l2_floodPortMask_set(rtksw_uint32 unit, rtksw_l2_flood_type_t floood_type, rtksw_portmask_t *pFlood_portmask);

/* Function Name:
 *      dal_rtl8371c_l2_floodPortMask_get
 * Description:
 *      Get flooding portmask
 * Input:
 *      unit    - Unit ID
 *      type    - flooding type.
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
extern rtksw_api_ret_t dal_rtl8371c_l2_floodPortMask_get(rtksw_uint32 unit, rtksw_l2_flood_type_t floood_type, rtksw_portmask_t *pFlood_portmask);

/* Function Name:
 *      dal_rtl8371c_l2_localPktPermit_set
 * Description:
 *      Set permittion of frames if source port and destination port are the same.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
 *      permit  - permittion status
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
extern rtksw_api_ret_t dal_rtl8371c_l2_localPktPermit_set(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t permit);

/* Function Name:
 *      dal_rtl8371c_l2_localPktPermit_get
 * Description:
 *      Get permittion of frames if source port and destination port are the same.
 * Input:
 *      unit    - Unit ID
 *      port    - Port id.
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
extern rtksw_api_ret_t dal_rtl8371c_l2_localPktPermit_get(rtksw_uint32 unit, rtksw_port_t port, rtksw_enable_t *pPermit);

/* Function Name:
 *      dal_rtl8371c_l2_aging_set
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
 *      The API can set LUT agging out period for each entry and the range is from 14s to 800s.
 */
extern rtksw_api_ret_t dal_rtl8371c_l2_aging_set(rtksw_uint32 unit, rtksw_l2_age_time_t aging_time);

/* Function Name:
 *      dal_rtl8371c_l2_aging_get
 * Description:
 *      Get LUT agging out time
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pEnable - Aging status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *      The API can get LUT agging out period for each entry.
 */
extern rtksw_api_ret_t dal_rtl8371c_l2_aging_get(rtksw_uint32 unit, rtksw_l2_age_time_t *pAging_time);

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddrLookup_set
 * Description:
 *      Set Lut IP multicast lookup function
 * Input:
 *      unit    - Unit ID
 *      type    - Lookup type for IPMC packet.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *      This API can work with rtksw_l2_ipMcastAddrLookupException_add.
 *      If users set the lookup type to DIP, the group in exception table
 *      will be lookup by DIP+SIP
 *      If users set the lookup type to DIP+SIP, the group in exception table
 *      will be lookup by only DIP
 */
extern rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddrLookup_set(rtksw_uint32 unit, rtksw_l2_ipmc_lookup_type_t type);

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastAddrLookup_get
 * Description:
 *      Get Lut IP multicast lookup function
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pType - Lookup type for IPMC packet.
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - Failed
 *      RT_ERR_SMI         - SMI access error
 * Note:
 *      None.
 */
extern rtksw_api_ret_t dal_rtl8371c_l2_ipMcastAddrLookup_get(rtksw_uint32 unit, rtksw_l2_ipmc_lookup_type_t *pType);

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastGroupEntry_add
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
extern rtksw_api_ret_t dal_rtl8371c_l2_ipMcastGroupEntry_add(rtksw_uint32 unit, ipaddr_t ip_addr, rtksw_uint32 vid, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastGroupEntry_del
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
extern rtksw_api_ret_t dal_rtl8371c_l2_ipMcastGroupEntry_del(rtksw_uint32 unit, ipaddr_t ip_addr, rtksw_uint32 vid);

/* Function Name:
 *      dal_rtl8371c_l2_ipMcastGroupEntry_get
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
extern rtksw_api_ret_t dal_rtl8371c_l2_ipMcastGroupEntry_get(rtksw_uint32 unit, ipaddr_t ip_addr, rtksw_uint32 vid, rtksw_portmask_t *pPortmask);

/* Function Name:
 *      dal_rtl8371c_l2_entry_get
 * Description:
 *      Get LUT unicast entry.
 * Input:
 *      unit        - Unit ID
 *      pL2_entry   - Index field in the structure.
 * Output:
 *      pL2_entry - other fields such as MAC, port, age...
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This API is used to get address by index from 0~2111.
 */
extern rtksw_api_ret_t dal_rtl8371c_l2_entry_get(rtksw_uint32 unit, rtksw_l2_addr_table_t *pL2_entry);

/* Function Name:
 *      dal_rtl8371c_l2_lookupHitIsolationAction_set
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
extern rtksw_api_ret_t dal_rtl8371c_l2_lookupHitIsolationAction_set(rtksw_uint32 unit, rtksw_l2_lookupHitIsolationAction_t action);

/* Function Name:
 *      dal_rtl8371c_l2_lookupHitIsolationAction_get
 * Description:
 *      Get action of lookup hit & isolation.
 * Input:
 *      unit    - Unit ID
 * Output:
 *      pAction         - The action
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
extern rtksw_api_ret_t dal_rtl8371c_l2_lookupHitIsolationAction_get(rtksw_uint32 unit, rtksw_l2_lookupHitIsolationAction_t *pAction);


#endif /* __DAL_rtl8371C_L2_H__ */

