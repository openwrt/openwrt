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

#ifndef _RTL8367C_TYPES_H_
#define _RTL8367C_TYPES_H_

#if defined(RTK_X86_CLE)
#include <stdio.h>
#endif

typedef unsigned long long      rtksw_uint64;
typedef long long               rtksw_int64;
typedef unsigned int            rtksw_uint32;
typedef int                     rtksw_int32;
typedef unsigned short          rtksw_uint16;
typedef short                   rtksw_int16;
typedef unsigned char           rtksw_uint8;
typedef char                    rtksw_int8;

#define CONST_T     const

#define RTKSW_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST    1

#define RTKSW_MAX_NUM_OF_PORT                         8
#define RTKSW_PORT_ID_MAX                             (RTKSW_MAX_NUM_OF_PORT-1)
#define RTKSW_PHY_ID_MAX                              (RTKSW_MAX_NUM_OF_PORT-4)
#define RTKSW_MAX_PORT_MASK                           0xFF

#define RTKSW_WHOLE_SYSTEM                            0xFF

typedef struct rtksw_portmask_s
{
    rtksw_uint32  bits[RTKSW_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST];
} rtksw_portmask_t;

typedef enum rtksw_enable_e
{
    RTKSW_DISABLED = 0,
    RTKSW_ENABLED,
    RTKSW_ENABLE_END
} rtksw_enable_t;

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN      6
#endif

/* ethernet address type */
typedef struct  rtksw_mac_s
{
    rtksw_uint8 octet[ETHER_ADDR_LEN];
} rtksw_mac_t;

typedef rtksw_uint32  rtksw_pri_t;      /* priority vlaue */
typedef rtksw_uint32  rtksw_qid_t;      /* queue id type */
typedef rtksw_uint32  rtksw_data_t;
typedef rtksw_uint32  rtksw_dscp_t;     /* dscp vlaue */
typedef rtksw_uint32  rtksw_fid_t;      /* filter id type */
typedef rtksw_uint32  rtksw_vlan_t;     /* vlan id type */
typedef rtksw_uint32  rtksw_mac_cnt_t;  /* MAC count type  */
typedef rtksw_uint32  rtksw_meter_id_t; /* meter id type  */
typedef rtksw_uint32  rtksw_rate_t;     /* rate type  */

typedef enum rtksw_port_e
{
    UTP_PORT0 = 0,
    UTP_PORT1,
    UTP_PORT2,
    UTP_PORT3,
    UTP_PORT4,
    UTP_PORT5,
    UTP_PORT6,
    UTP_PORT7,

    EXT_PORT0 = 16,
    EXT_PORT1,
    EXT_PORT2,

    UNDEFINE_PORT = 30,
    RTKSW_PORT_MAX = 31
} rtksw_port_t;


#ifndef _RTL_TYPES_H

#if 0
typedef unsigned long long      uint64;
typedef long long               int64;
typedef unsigned int            uint32;
typedef int                     int32;
typedef unsigned short          uint16;
typedef short                   int16;
typedef unsigned char           uint8;
typedef char                    int8;
#endif

typedef rtksw_uint32                  ipaddr_t;
typedef rtksw_uint32                  memaddr;

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN      6
#endif

typedef struct ether_addr_s {
    rtksw_uint8 octet[ETHER_ADDR_LEN];
} ether_addr_t;

#if defined(RTK_X86_CLE)
#ifdef __KERNEL__
#define rtlglue_printf printk
#else
#define rtlglue_printf printf
#endif
#define PRINT           rtlglue_printf
#endif /* defined(RTK_X86_CLE) */

#endif /*_RTL_TYPES_H*/


/* type abstraction */
#ifdef EMBEDDED_SUPPORT

typedef rtksw_int16                   rtksw_api_ret_t;
typedef rtksw_int16                   ret_t;
typedef rtksw_uint32                  rtksw_u_long;

#else

typedef rtksw_int32                   rtksw_api_ret_t;
typedef rtksw_int32                   ret_t;
typedef rtksw_uint64                  rtksw_u_long_t;

#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define CONST           const

#if defined(CONFIG_COMPATIBLE_V1_4)

#define rtk_uint64  rtksw_uint64
#define rtk_int64   rtksw_int64
#define rtk_uint32  rtksw_uint32
#define rtk_int32   rtksw_int32
#define rtk_uint16  rtksw_uint16
#define rtk_int16   rtksw_int16
#define rtk_uint8   rtksw_uint8
#define rtk_int8    rtksw_int8

#define RTK_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST     RTKSW_TOTAL_NUM_OF_WORD_FOR_1BIT_PORT_LIST
#define RTK_MAX_NUM_OF_PORT                          RTKSW_MAX_NUM_OF_PORT
#define RTK_PORT_ID_MAX                              RTKSW_PORT_ID_MAX
#define RTK_PHY_ID_MAX                               RTKSW_PHY_ID_MAX
#define RTK_MAX_PORT_MASK                            RTKSW_MAX_PORT_MASK
#define RTK_MAX_PORT_MASK                            RTKSW_MAX_PORT_MASK

#define RTK_WHOLE_SYSTEM    RTKSW_WHOLE_SYSTEM

#define DISABLED        RTKSW_DISABLED
#define ENABLED         RTKSW_ENABLED
#define RTK_ENABLE_END  RTKSW_ENABLE_END

#define rtk_portmask_t  rtksw_portmask_t
#define rtk_enable_t    rtksw_enable_t
#define rtk_mac_t       rtksw_mac_t
#define rtk_port_t      rtksw_port_t
#define rtk_api_ret_t   rtksw_api_ret_t
#define rtk_u_long_t    rtksw_u_long_t

#define rtk_pri_t       rtksw_pri_t
#define rtk_qid_t       rtksw_qid_t   
#define rtk_data_t      rtksw_data_t
#define rtk_dscp_t      rtksw_dscp_t    
#define rtk_fid_t       rtksw_fid_t      
#define rtk_vlan_t      rtksw_vlan_t     
#define rtk_mac_cnt_t   rtksw_mac_cnt_t 
#define rtk_meter_id_t  rtksw_meter_id_t
#define rtk_rate_t      rtksw_rate_t    

#endif /* CONFIG_COMPATIBLE_V1_4 */

#endif /* _RTL8367C_TYPES_H_ */
