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
#ifndef _HAL_EFUSE_EXPORT_H_
#define _HAL_EFUSE_EXPORT_H_

enum HAL_EFUSE_CMD_ID {
	HAL_EFUSE_HELP,
	HAL_EFUSE_WIFI_DUMP_MAP,
	HAL_EFUSE_WIFI_LOGIC_DUMP_MAP
};

static const struct hal_cmd_info hal_efuse_cmd_i[] = {
	{"-h", HAL_EFUSE_HELP},
	{"dump_map", HAL_EFUSE_WIFI_DUMP_MAP},
	{"dump_hw_map", HAL_EFUSE_WIFI_LOGIC_DUMP_MAP},
};

/* efuse exported API */
u32
rtw_efuse_init(struct rtw_phl_com_t *phl_com, struct rtw_hal_com_t *hal_com,
			   void **efuse);

void rtw_efuse_deinit(struct rtw_hal_com_t *hal_com, void *efuse);

enum rtw_hal_status
rtw_efuse_get_info(void *efuse, enum rtw_efuse_info info_type, void *value,
				   u8 size);

void rtw_efuse_process(void *efuse, char *ic_name);

bool rtw_efuse_is_processed(void *efuse);

enum rtw_hal_status
rtw_efuse_logicmap_buf_load(void *efuse, u8* buf, bool is_limit);

enum rtw_hal_status
rtw_efuse_shadow_load(void *efuse, bool is_limit);

enum rtw_hal_status
rtw_efuse_shadow_read(void *efuse, u8 byte_count, u16 offset, u32 *value,
					  bool is_limit);

enum rtw_hal_status
rtw_efuse_shadow_write(void *efuse, u8 byte_count, u16 offset, u32 value,
					   bool is_limit);

enum rtw_hal_status
rtw_efuse_shadow_update(void *efuse, bool is_limit);

enum rtw_hal_status
rtw_efuse_shadow2buf(void *efuse, u8 *destbuf, u16 buflen);

enum rtw_hal_status
rtw_efuse_file_map_load(void *efuse, char *file_path, u8 is_limit);

enum rtw_hal_status
rtw_efuse_file_mask_load(void *efuse, char *file_path, u8 is_limit);

enum rtw_hal_status
rtw_efuse_get_usage(void *efuse, u32 *usage);

enum rtw_hal_status
rtw_efuse_get_logical_size(void *efuse, u32 *size, bool is_limited);

enum rtw_hal_status
rtw_efuse_get_size(void *efuse, u32 *size);

enum rtw_hal_status
rtw_efuse_get_avl(void *efuse, u32 *size);

enum rtw_hal_status
rtw_efuse_get_shadowmap_from(void *efuse, u8 *val);

char*
rtw_efuse_get_shadowmap_from_to_str(void *efuse);

enum rtw_hal_status
rtw_efuse_get_offset_mask(void *efuse, u16 offset, u8 *mask);

enum rtw_hal_status
rtw_efuse_get_mask_buf(void *efuse, u8 *destbuf, u32 *buflen);

/* BT EFUSE */
enum rtw_hal_status
rtw_efuse_bt_shadow_load(void *efuse);

enum rtw_hal_status
rtw_efuse_bt_shadow_read(void *efuse, u8 byte_count, u16 offset, u32 *value);

enum rtw_hal_status
rtw_efuse_bt_shadow_write(void *efuse, u8 byte_count, u16 offset, u32 value);

enum rtw_hal_status
rtw_efuse_bt_shadow_update(void *efuse);

enum rtw_hal_status
rtw_efuse_bt_shadow2buf(void *efuse, u8 *destbuf, u16 buflen);

enum rtw_hal_status
rtw_efuse_bt_file_map_load(void *efuse, char *file_path);

enum rtw_hal_status
rtw_efuse_bt_file_mask_load(void *efuse, char *file_path);

enum rtw_hal_status
rtw_efuse_bt_get_usage(void *efuse, u32 *usage);

enum rtw_hal_status
rtw_efuse_bt_get_logical_size(void *efuse, u32 *size);

enum rtw_hal_status
rtw_efuse_bt_get_size(void *efuse, u32 *size);

enum rtw_hal_status
rtw_efuse_bt_get_avl(void *efuse, u32 *size);

enum rtw_hal_status
rtw_efuse_bt_get_offset_mask(void *efuse, u16 offset, u8 *mask);

enum rtw_hal_status
rtw_efuse_bt_get_mask_buf(void *efuse, u8 *destbuf, u32 *buflen);

enum rtw_hal_status
rtw_efuse_bt_read_hidden(void *efuse, u32 addr, u32 size, u8 *val);

enum rtw_hal_status
rtw_efuse_bt_write_hidden(void *efuse, u32 addr, u8 val);

enum rtw_hal_status
rtw_efuse_read_phy_wifi(void *efuse, u32 addr, u32 size, u8 *data, u8 type);

enum rtw_hal_status
rtw_efuse_renew(void *efuse, u8 type);

#endif /* _HAL_EFUSE_EXPORT_H_ */
