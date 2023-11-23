/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _HAL_EFUSE_H_
#define _HAL_EFUSE_H_

#define MAX_EFUSE_FILE_VERSION_LENGTH 6
#define EFUSE_MASK_FILE_LEN 96
#define MAX_EFUSE_MAP_LEN 1536

enum EFUSE_STATUS_FLAGS {
	EFUSE_STATUS_PROCESS = BIT0,
	EFUSE_STATUS_MAP_FILE_LOADED = BIT1,
	EFUSE_STATUS_MASK_FILE_LOADED = BIT2,
	EFUSE_STATUS_BT_MAP_FILE_LOADED = BIT3,
	EFUSE_STATUS_BT_MASK_FILE_LOADED = BIT4
};

enum EFUSE_SHADOW_MAP_STATUS {
	DEFAULT_MAP = 0,
	HW_LOG_MAP = 1,
	FILE_MAP = 2,
	EFUSE_UNKNOWN,
};

#define SHADOWMAP_FROM2STR(status)\
(status == DEFAULT_MAP) ? "DEFAULT" :\
(status == HW_LOG_MAP) ? "HW_LOG_EFUSE" :\
(status == FILE_MAP) ? "FILE_EFUSE" :\
"UNknow"

/*
 *	@phl_com
 *	@hal_com
 *	@shadow_map: Buffer pointer for limited size logical map
 *	@mask: Buffer pointer for limited size mask read from mask file
 *	@map_version: Buffer pointer for map version read from map file
 *	@mask_version: Buffer pointer for mask version read from mask file
 *	@log_efuse_size: Limited logical map size
 *	@mask_size: Limited mask size
 *	@version_len: Length of verion field in map/mask
 *	@status: Efuse status
 *	@is_map_valid: Flag to check autoload status
 *	@reserved
 */
struct efuse_t {
	struct rtw_phl_com_t *phl_com;
	struct rtw_hal_com_t *hal_com;
	u8 *shadow_map;
	u8 *mask;
	u8 *map_version;
	u8 *mask_version;
	u32 log_efuse_size;
	u32 mask_size;
	u32 limit_efuse_size;
	u32 limit_mask_size;
	u8 efuse_a_die_size;
	u32 hci_to_a_die_offset;
	u32 a_die_start_offset;
	u8 version_len;
	u8 status;
	u8 is_map_valid;
	u8 reserved;
	u8 map_from_status;
	/* BT*/
	u8 *bt_shadow_map;
	u8 *bt_mask;
	u32 bt_log_efuse_size;
	u32 bt_efuse_size;
	u32 bt_mask_size;
};

#endif /* _HAL_EFUSE_H_ */
