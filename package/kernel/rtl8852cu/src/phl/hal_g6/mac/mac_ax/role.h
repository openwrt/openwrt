/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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

#ifndef _MAC_AX_ROLE_H_
#define _MAC_AX_ROLE_H_

#include "../type.h"
#include "fwcmd.h"
#include "addr_cam.h"
#include "security_cam.h"
#include "hw.h"
#include "trxcfg.h"

/*--------------------Define -------------------------------------------*/
#define CCTL_NTX_PATH_EN	3
#define CCTL_PATH_MAP_B		1
#define CCTL_PATH_MAP_C		2
#define CCTL_PATH_MAP_D		3
#define CCTRL_NC		1
#define CCTRL_NR		1
#define CCTRL_CB		1
#define CCTRL_CSI_PARA_EN	1
#define CCTRL_CSI_PARA_EN_MSK	1

/*--------------------Define MACRO--------------------------------------*/
#define MAC_AX_ACTUAL_WMM_BAND BIT(1)
#define MAC_AX_ACTUAL_WMM_DRV_WMM BIT(0)

/*--------------------Define Enum---------------------------------------*/
/*--------------------Define Struct-------------------------------------*/
/*--------------------Function Prototype--------------------------------*/

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief role_tbl_init
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 role_tbl_init(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief role_tbl_exit
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 role_tbl_exit(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief role_info_valid
 *
 * @param *adapter
 * @param *info
 * @param change_role
 * @return Please Place Description here.
 * @retval u32
 */
u32 role_info_valid(struct mac_ax_adapter *adapter,
		    struct mac_ax_role_info *info,
		    enum mac_ax_role_opmode op);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_add_role
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_add_role(struct mac_ax_adapter *adapter, struct mac_ax_role_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_remove_role
 *
 * @param *adapter
 * @param macid
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_remove_role(struct mac_ax_adapter *adapter, u8 macid);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_remove_role_by_band
 *
 * @param *adapter
 * @param band
 * @param sw
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_remove_role_by_band(struct mac_ax_adapter *adapter, u8 band, u8 sw);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_change_role
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_change_role(struct mac_ax_adapter *adapter,
		    struct mac_ax_role_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_role_srch
 *
 * @param *adapter
 * @param macid
 * @return Please Place Description here.
 * @retval  mac_role_tbl
 */
struct mac_role_tbl *mac_role_srch(struct mac_ax_adapter *adapter,
				   u8 macid);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief role_srch_no_lock
 *
 * @param *adapter
 * @param macid
 * @return Please Place Description here.
 * @retval  mac_role_tbl
 */
struct mac_role_tbl *role_srch_no_lock(struct mac_ax_adapter *adapter,
				       u8 macid);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_role_srch_by_addr_cam
 *
 * @param *adapter
 * @param addr_cam_idx
 * @return Please Place Description here.
 * @retval  mac_role_tbl
 */
struct mac_role_tbl *mac_role_srch_by_addr_cam(struct mac_ax_adapter *adapter,
					       u16 addr_cam_idx);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_role_srch_by_bssid
 *
 * @param *adapter
 * @param bssid_cam_idx
 * @return Please Place Description here.
 * @retval  mac_role_tbl
 */
struct mac_role_tbl *mac_role_srch_by_bssid(struct mac_ax_adapter *adapter,
					    u8 bssid_cam_idx);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_get_macaddr
 *
 * @param *adapter
 * @param *macaddr
 * @param role_idx
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_get_macaddr(struct mac_ax_adapter *adapter,
		    struct mac_ax_macaddr *macaddr,
		    u8 role_idx);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_set_slot_time
 *
 * @param *adapter
 * @param mac_ax_slot_time
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_set_slot_time(struct mac_ax_adapter *adapter, enum mac_ax_slot_time);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_h2c_join_info
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval  u32
 */

static u32 mac_h2c_join_info(struct mac_ax_adapter *adapter,
			     struct mac_ax_role_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */

/**
 * @brief mac_fw_role_maintain
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval  u32
 */
static u32 mac_fw_role_maintain(struct mac_ax_adapter *adapter,
				struct mac_ax_role_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Association
 * @{
 * @addtogroup Role_Related
 * @{
 */
/**
 * @brief mac_role_sync
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_role_sync(struct mac_ax_adapter *adapter, struct mac_ax_role_info *info);
/**
 * @}
 * @}
 */
#endif
