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

#ifndef _MAC_AX_SEC_CAM_H_
#define _MAC_AX_SEC_CAM_H_

#include "../mac_def.h"
#include "role.h"
#include "fwcmd.h"
#include "addr_cam.h"

/*--------------------Define ----------------------------------------*/
#ifdef PHL_FEATURE_AP
	#define SEC_CAM_ENTRY_NUM	0x80
#else
	#define SEC_CAM_ENTRY_NUM	0x10
#endif

#define ADDRCAM_VALID BIT0
#define ADDRCAM_SEC_MODE_SH 16
#define ADDRCAM_SEC_ENT0_KEYID_SH 18
#define ADDRCAM_SEC_ENT0_KEYID_SH 18

#define ADDRCAM_KEYID_MSK 0x3
#define ADDRCAM_SECMODE_MSK 0x3
#define ADDRCAM_MACID_MSK 0xFF
#define ADDRCAM_KEY_VALID_MSK 0xFF
#define ADDRCAM_KEY_CAM_IDX_MSK 0xFF

#define ADDRCAM_VALID_BIT_OFFSET 0x00
#define ADDRCAM_MACID_OFFSET 0x18
#define ADDRCAM_SECMODE_OFFSET 0x1C
#define ADDRCAM_KEY_VALID_OFFSET 0x20
#define ADDRCAM_KEY_IDX_OFFSET 0x21

#define SEC_CAM_ENTRY_SIZE	0x20
#define ADDR_CAM_ENTRY_SIZE	0x40
#define MACNOKEYINDEX		0xFF

#define DEFAULT_KEYID		0x0
#define DEFAULT_KEYTYPE		0x0

#define MACID_ENTRY_NUM		0x80
#define KEYNUM_PER_MACID	0x07

/*--------------------DSecurity cam type declaration-----------------*/

/**
 * @struct sec_cam_entry_t
 * @brief sec_cam_entry_t
 *
 * @var sec_cam_entry_t::valid
 * Please Place Description here.
 * @var sec_cam_entry_t::mac_id
 * Please Place Description here.
 * @var sec_cam_entry_t::key_id
 * Please Place Description here.
 * @var sec_cam_entry_t::key_type
 * Please Place Description here.
 * @var sec_cam_entry_t::sec_cam_info
 * Please Place Description here.
 */
struct sec_cam_entry_t {
	u8 valid;
	u8 mac_id;
	u8 key_id;
	u8 key_type;
	struct mac_ax_sec_cam_info *sec_cam_info;
};

struct dctl_secinfo_entry_t {
	u16 aes_iv_l;
	u32 aes_iv_h;
	u8 sec_keyid;
	u8 wapi_ctrl;
	u8 sec_ent_valid;
	u8 sec_ent_keyid[7];
	u8 sec_ent[7];
};

/**
 * @struct sec_cam_table_t
 * @brief sec_cam_table_t
 *
 * @var sec_cam_table_t::sec_cam_entry
 * Please Place Description here.
 * @var sec_cam_table_t::next_cam_storage_idx
 * Please Place Description here.
 */
struct sec_cam_table_t {
	struct sec_cam_entry_t *sec_cam_entry[128];
	u8 next_cam_storage_idx;
};

struct dctl_sec_info_t {
	struct dctl_secinfo_entry_t *dctl_secinfo_entry[MACID_ENTRY_NUM];
};

/**
 * @enum SEC_FUNCTION_TYPE
 *
 * @brief SEC_FUNCTION_TYPE
 *
 * @var SEC_FUNCTION_TYPE::SEC_TX_ENC
 * Please Place Description here.
 * @var SEC_FUNCTION_TYPE::SEC_RX_ENC
 * Please Place Description here.
 * @var SEC_FUNCTION_TYPE::SEC_BC_ENC
 * Please Place Description here.
 * @var SEC_FUNCTION_TYPE::SEC_MC_ENC
 * Please Place Description here.
 * @var SEC_FUNCTION_TYPE::SEC_UC_MGNT_ENC
 * Please Place Description here.
 * @var SEC_FUNCTION_TYPE::SEC_BMC_MGNT_ENC
 * Please Place Description here.
 */
enum SEC_FUNCTION_TYPE {
	SEC_TX_ENC = 0,
	SEC_RX_ENC = 1,
	SEC_BC_ENC = 2,
	SEC_MC_ENC = 3,
	SEC_UC_MGNT_ENC  = 4,
	SEC_BMC_MGNT_ENC = 5,
};

enum SEC_IV_UPD_TYPE {
	SEC_IV_UPD_TYPE_NONE = 0,
	SEC_IV_UPD_TYPE_WRITE = 1,
	SEC_IV_UPD_TYPE_READ = 2
};

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
u32 disconnect_flush_key(struct mac_ax_adapter *adapter,
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

u32 sec_info_tbl_init(struct mac_ax_adapter *adapter);
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

u32 free_sec_info_tbl(struct mac_ax_adapter *adapter);
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

u32 fill_sec_cam_info(struct mac_ax_adapter *adapter,
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

u32 mac_sta_add_key(struct mac_ax_adapter *adapter,
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

u32 mac_sta_del_key(struct mac_ax_adapter *adapter,
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

u32 mac_sta_search_key_idx(struct mac_ax_adapter *adapter,
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

u32 mac_sta_hw_security_support(struct mac_ax_adapter *adapter,
				u8 hw_security_support_type, u8 enable);
/**
 * @}
 * @}
 */

u32 refresh_security_cam_info(struct mac_ax_adapter *adapter,
			      u8 mac_id);
/**
 * @}
 * @}
 */

u32 mac_wowlan_secinfo(struct mac_ax_adapter *adapter,
		       struct mac_ax_sec_iv_info *sec_iv_info);

#endif
