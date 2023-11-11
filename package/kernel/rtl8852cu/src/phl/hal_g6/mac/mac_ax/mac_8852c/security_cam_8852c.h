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

#ifndef _MAC_AX_SEC_CAM_8852C_H_
#define _MAC_AX_SEC_CAM_8852C_H_

#include "../../mac_def.h"
//#include "role.h"
#include "../fwcmd.h"
//#include "addr_cam.h"
#include "../security_cam.h"
#include "tblupd_8852c.h"

/*--------------------Define ----------------------------------------*/

#define SEC_DEFAULT_MODE 0x2
#define SEC_MODE_SH 16
#define SEC_ENT0_KEYID_SH 18
#define KEYID_MSK 0x3
#define SEC_MODE_MSK 0x3
#define KEY_VALID_MSK 0xFF
#define KEY_CAM_IDX_MSK 0xFF
#define AES_IV_H_MSK 0xFFFFFFFF
#define AES_IV_L_MSK 0xFFFF
#define KEY_CAM_IDX_SH 0x8
#define DTBL_DWORD5_KEY_NUM 0x3
#define DTBL_DWORD6_KEY_NUM 0x7

#define DCTRL_TBL_SEC_IV0_SH 0
#define DCTRL_TBL_SEC_IV0_MSK 0xff
#define DCTRL_TBL_SEC_IV1_SH 8
#define DCTRL_TBL_SEC_IV1_MSK 0xff
#define DCTRL_TBL_SEC_IV2_SH 0
#define DCTRL_TBL_SEC_IV2_MSK 0xff
#define DCTRL_TBL_SEC_IV3_SH 8
#define DCTRL_TBL_SEC_IV3_MSK 0xff
#define DCTRL_TBL_SEC_IV4_SH 16
#define DCTRL_TBL_SEC_IV4_MSK 0xff
#define DCTRL_TBL_SEC_IV5_SH 24
#define DCTRL_TBL_SEC_IV5_MSK 0xff
#define DCTRL_TBL_SIZE_8852C 0x20
#define DMAC_TBL_IV_BYTE_MSK 0xff

/*--------------------Funciton declaration----------------------------*/
/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup Security
 * @{
 */

/**
 * @brief disconnect_flush_key
 *
 * @param *adapter
 * @param *role
 * @return Please Place Description here.
 * @retval u32
 */
u32 disconnect_flush_key_8852c(struct mac_ax_adapter *adapter,
			       struct mac_role_tbl *role);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup Security
 * @{
 */

/**
 * @brief sec_info_tbl_init
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */

u32 sec_info_tbl_init_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup Security
 * @{
 */

/**
 * @brief free_sec_info_tbl
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */

u32 free_sec_info_tbl_8852c(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup Security
 * @{
 */

/**
 * @brief fill_sec_cam_info
 *
 * @param *adapter
 * @param *s_info
 * @param *sec_info
 * @return Please Place Description here.
 * @retval u32
 */

u32 fill_sec_cam_info_8852c(struct mac_ax_adapter *adapter,
			    struct mac_ax_sec_cam_info *s_info,
			    struct fwcmd_seccam_info *sec_info,
			    u8 clear);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup Security
 * @{
 */

/**
 * @brief mac_sta_add_key
 *
 * @param *adapter
 * @param *sec_cam_content
 * @param mac_id
 * @param key_id
 * @param key_type
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_sta_add_key_8852c(struct mac_ax_adapter *adapter,
			  struct mac_ax_sec_cam_info *sec_cam_content,
			  u8 mac_id,
			  u8 key_id,
			  u8 key_type);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup Security
 * @{
 */

/**
 * @brief mac_sta_del_key
 *
 * @param *adapter
 * @param mac_id
 * @param key_id
 * @param key_type
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_sta_del_key_8852c(struct mac_ax_adapter *adapter,
			  u8 mac_id,
			  u8 key_id,
			  u8 key_type);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup Security
 * @{
 */

/**
 * @brief mac_sta_search_key_idx
 *
 * @param *adapter
 * @param mac_id
 * @param key_id
 * @param key_type
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_sta_search_key_idx_8852c(struct mac_ax_adapter *adapter,
				 u8 mac_id, u8 key_id, u8 key_type);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Basic_TRX
 * @{
 * @addtogroup Security
 * @{
 */

/**
 * @brief mac_sta_hw_security_support
 *
 * @param *adapter
 * @param hw_security_support_type
 * @param enable
 * @return Please Place Description here.
 * @retval u32
 */

u32 mac_sta_hw_security_support_8852c(struct mac_ax_adapter *adapter,
				      u8 hw_security_support_type, u8 enable);
/**
 * @}
 * @}
 */

u32 mac_wowlan_secinfo_8852c(struct mac_ax_adapter *adapter,
			     struct mac_ax_sec_iv_info *sec_iv_info);

/**
 * @}
 * @}
 */
#endif
