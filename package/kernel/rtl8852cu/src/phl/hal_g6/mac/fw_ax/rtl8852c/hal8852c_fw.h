/******************************************************************************
 *
 * Copyright(c) 2012 - 2020 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/
#ifdef PHL_FEATURE_AP

#if defined(MAC_8852C_SUPPORT)
#ifdef MAC_FW_8852C_U1
#ifdef MAC_FW_CATEGORY_AP
extern u32 array_length_8852c_u1_ap;
extern u8 array_8852c_u1_ap[402600];
#endif /* MAC_FW_CATEGORY_AP */

#endif /* MAC_FW_8852C_U1 */
#ifdef MAC_FW_8852C_U2
#ifdef MAC_FW_CATEGORY_AP
extern u32 array_length_8852c_u2_ap;
extern u8 array_8852c_u2_ap[361120];
#endif /* MAC_FW_CATEGORY_AP */

#endif /* MAC_FW_8852C_U2 */
#endif /* #if MAC_XXXX_SUPPORT */
#endif /* PHL_FEATURE_AP */
#ifdef PHL_FEATURE_NIC

#if defined(MAC_8852C_SUPPORT)
#ifdef MAC_FW_8852C_U1
#ifdef MAC_FW_CATEGORY_NIC
extern u32 array_length_8852c_u1_nic;
extern u8 array_8852c_u1_nic[323480];
#endif /* MAC_FW_CATEGORY_NIC */

#ifdef MAC_FW_CATEGORY_WOWLAN
extern u32 array_length_8852c_u1_wowlan;
extern u8 array_8852c_u1_wowlan[254224];
#endif /* MAC_FW_CATEGORY_WOWLAN */

#endif /* MAC_FW_8852C_U1 */
#ifdef MAC_FW_8852C_U2
#ifdef MAC_FW_CATEGORY_NIC
extern u32 array_length_8852c_u2_nic;
extern u8 array_8852c_u2_nic[286136];
#endif /* MAC_FW_CATEGORY_NIC */

#ifdef MAC_FW_CATEGORY_WOWLAN
extern u32 array_length_8852c_u2_wowlan;
extern u8 array_8852c_u2_wowlan[213744];
#endif /* MAC_FW_CATEGORY_WOWLAN */

#endif /* MAC_FW_8852C_U2 */
#endif /* #if MAC_XXXX_SUPPORT */
#endif /* PHL_FEATURE_NIC */
