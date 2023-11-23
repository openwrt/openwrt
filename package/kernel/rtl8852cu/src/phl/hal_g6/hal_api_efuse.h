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
#ifndef _HAL_API_EFUSE_H_
#define _HAL_API_EFUSE_H_

/* efuse exported API */
/* WIFI EFUSE */
enum rtw_hal_status rtw_hal_efuse_init(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal_info);
void rtw_hal_efuse_deinit(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal_info);
void rtw_hal_efuse_process(struct rtw_phl_com_t *phl_com,
                           struct hal_info_t *hal_info,
                           char *ic_name
);
enum rtw_hal_status
rtw_hal_efuse_shadow_load(struct hal_info_t *hal_info, bool is_limit);
enum rtw_hal_status
rtw_hal_efuse_shadow_read(struct hal_info_t *hal_info, u8 byte_count,
						  u16 offset, u32 *value, bool is_limit);
enum rtw_hal_status
rtw_hal_efuse_shadow_write(struct hal_info_t *hal_info, u8 byte_count,
						   u16 offset, u32 value, bool is_limit);
enum rtw_hal_status
rtw_hal_efuse_shadow_update(struct hal_info_t *hal_info, bool is_limit);
enum rtw_hal_status rtw_hal_efuse_shadow2buf(struct hal_info_t *hal_info,
	u8 *pbuf, u16 buflen);
enum rtw_hal_status rtw_hal_efuse_file_map_load(struct hal_info_t *hal_info,
	char *file_path, u8 is_limit);
enum rtw_hal_status rtw_hal_efuse_file_mask_load(struct hal_info_t *hal_info,
	char *file_path, u8 is_limit);
enum rtw_hal_status rtw_hal_efuse_get_usage(struct hal_info_t *hal_info,
	u32 *usage);
enum rtw_hal_status rtw_hal_efuse_get_logical_size(struct hal_info_t *hal_info,
	u32 *size);
enum rtw_hal_status rtw_hal_efuse_get_size(struct hal_info_t *hal_info,
	u32 *size);
enum rtw_hal_status rtw_hal_efuse_get_avl(struct hal_info_t *hal_info,
	u32 *size);
enum rtw_hal_status rtw_hal_efuse_get_shadowmap_from(struct hal_info_t *hal_info,
	u8 *val);
enum rtw_hal_status rtw_hal_efuse_get_offset_mask(struct hal_info_t *hal_info,
	u16 offset, u8 *mask);
enum rtw_hal_status rtw_hal_efuse_get_mask_buf(struct hal_info_t *hal_info,
	u8 *mask, u32 *buflen);


/* BT EFUSE */
enum rtw_hal_status rtw_hal_efuse_bt_shadow_load(struct hal_info_t *hal_info);

enum rtw_hal_status rtw_hal_efuse_bt_shadow_read(struct hal_info_t *hal_info, u8 byte_count,
						  u16 offset, u32 *value);

enum rtw_hal_status rtw_hal_efuse_bt_shadow_write(struct hal_info_t *hal_info, u8 byte_count,
						   u16 offset, u32 value);

enum rtw_hal_status rtw_hal_efuse_bt_shadow_update(struct hal_info_t *hal_info);

enum rtw_hal_status rtw_hal_efuse_bt_shadow2buf(struct hal_info_t *hal_info,
	u8 *pbuf, u16 buflen);

enum rtw_hal_status rtw_hal_efuse_bt_file_map_load(
	struct hal_info_t *hal_info, char *file_path);

enum rtw_hal_status rtw_hal_efuse_bt_file_mask_load(
	struct hal_info_t *hal_info, char *file_path);

enum rtw_hal_status rtw_hal_efuse_bt_get_usage(struct hal_info_t *hal_info,
	u32 *usage);

enum rtw_hal_status rtw_hal_efuse_bt_get_logical_size(struct hal_info_t *hal_info,
	u32 *size);

enum rtw_hal_status rtw_hal_efuse_bt_get_size(struct hal_info_t *hal_info,
	u32 *size);

enum rtw_hal_status rtw_hal_efuse_bt_get_avl(struct hal_info_t *hal_info,
	u32 *size);

enum rtw_hal_status rtw_hal_efuse_bt_get_offset_mask(struct hal_info_t *hal_info,
	u16 offset, u8 *mask);

enum rtw_hal_status rtw_hal_efuse_bt_get_mask_buf(struct hal_info_t *hal_info,
	u8 *mask, u32 *buflen);

enum rtw_hal_status rtw_hal_efuse_bt_read_hidden(
	struct hal_info_t *hal_info, u32 addr, u32 size, u8 *val);

enum rtw_hal_status rtw_hal_efuse_bt_write_hidden(
	struct hal_info_t *hal_info, u32 addr, u8 val);

enum rtw_hal_status rtw_hal_efuse_read_phy_efuse(
	struct hal_info_t *hal_info, u32 addr, u32 size, u8 *data, u8 type);

enum rtw_hal_status rtw_hal_efuse_renew(
	struct hal_info_t *hal_info, u8 type);

bool rtw_hal_efuse_proc_cmd(
	struct hal_info_t *hal_info,
	struct rtw_proc_cmd *incmd,
	char *output,
	u32 out_len
	);

#endif /* _HAL_API_EFUSE_H_ */
