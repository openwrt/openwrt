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
#define _HAL_TEST_MP_EFUSE_C_
#include "../../hal_headers.h"
#include "../../../test/mp/phl_test_mp_def.h"

#ifdef CONFIG_HAL_TEST_MP

/* WIFI EFUSE */
enum rtw_hal_status rtw_hal_mp_efuse_wifi_shadow_read(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: offset = 0x%x\n", __FUNCTION__, arg->io_offset);

	hal_status = rtw_hal_efuse_shadow_read(mp->hal,
										   arg->io_type,
										   arg->io_offset,
										   &arg->io_value,
										   true);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: value = 0x%x\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_wifi_shadow_write(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: offset = 0x%x\n", __FUNCTION__, arg->io_offset);
	PHL_INFO("%s: value = 0x%x\n", __FUNCTION__, arg->io_value);

	hal_status = rtw_hal_efuse_shadow_write(mp->hal,
											arg->io_type,
											arg->io_offset,
											arg->io_value,
											true);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_wifi_shadow_update(
	struct mp_context *mp)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	/* PG limit part of logical efuse map in MP*/
	hal_status = rtw_hal_efuse_shadow_update(mp->hal, true);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_wifi_shadow_reload(
	struct mp_context *mp)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	/* Reload limit part of logical efuse map in MP*/
	hal_status = rtw_hal_efuse_shadow_load(mp->hal, true);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_offset_mask(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_get_offset_mask(mp->hal,
						   arg->io_offset,
						   (u8 *)&arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: wifi offset mask = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_usage(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_get_usage(mp->hal, (u32 *)&arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: wifi usage(bytes/percentage) = %d\n",
		 __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_log_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_get_logical_size(mp->hal, &arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: wifi logical size = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_get_size(mp->hal, &arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: wifi size = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_get_avl_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_get_avl(mp->hal, &arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: available size = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_get_shadowmap_from_val(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_get_shadowmap_from(mp->hal, (u8*)&arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: val=%d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_autoload_status(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_mac_check_efuse_autoload(mp->hal, &arg->autoload);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: autoload status = %d\n", __FUNCTION__, arg->autoload);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_shadow2buf(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);
	if (arg->poutbuf != NULL && arg->buf_len != 0)
		hal_status = rtw_hal_efuse_shadow2buf(mp->hal, arg->poutbuf, arg->buf_len);
	else
		PHL_INFO("%s: buf null, buf len = %d\n", __FUNCTION__, arg->buf_len);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_file_map_load(
	struct mp_context *mp, char *pfilepath)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	if (pfilepath)
		hal_status = rtw_hal_efuse_file_map_load(mp->hal, pfilepath, true);
	else 
		PHL_INFO("%s: pfilepath null\n", __FUNCTION__);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_file_mask_load(
	struct mp_context *mp, char *pfilepath)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	if (pfilepath)
		hal_status = rtw_hal_efuse_file_mask_load(mp->hal, pfilepath, true);
	else
		PHL_INFO("%s: pfilepath null\n", __FUNCTION__);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_get_info(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_get_info(hal_info->hal_com,
					    arg->io_type,
					    arg->poutbuf,
					    (u8)arg->buf_len);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

/* BT EFUSE */
enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow_read(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: offset = 0x%x\n", __FUNCTION__, arg->io_offset);

	hal_status = rtw_hal_efuse_bt_shadow_read(mp->hal,
										   arg->io_type,
										   arg->io_offset,
										   &arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: value = 0x%x\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow_write(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: offset = 0x%x\n", __FUNCTION__, arg->io_offset);
	PHL_INFO("%s: value = 0x%x\n", __FUNCTION__, arg->io_value);

	hal_status = rtw_hal_efuse_bt_shadow_write(mp->hal,
											arg->io_type,
											arg->io_offset,
											arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow_update(
	struct mp_context *mp)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_bt_shadow_update(mp->hal);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow_reload(
	struct mp_context *mp)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_bt_shadow_load(mp->hal);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_get_offset_mask(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_bt_get_offset_mask(mp->hal,
						   arg->io_offset,
						   (u8 *)&arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: bt offset mask = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_get_usage(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_bt_get_usage(mp->hal, (u32 *)&arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: bt usage(bytes/percentage) = %d\n",
		 __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_get_log_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_bt_get_logical_size(mp->hal, &arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: bt logical size = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_get_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_bt_get_size(mp->hal, &arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: bt size = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_shadow2buf(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);
	if (arg->poutbuf != NULL && arg->buf_len != 0)
		hal_status = rtw_hal_efuse_bt_shadow2buf(mp->hal, arg->poutbuf, arg->buf_len);
	else
		PHL_INFO("%s: buf null, buf len = %d\n", __FUNCTION__, arg->buf_len);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_get_avl_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_bt_get_avl(mp->hal, &arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: bt available size = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_file_map_load(
	struct mp_context *mp, char *pfilepath)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	if (pfilepath)
		hal_status = rtw_hal_efuse_bt_file_map_load(mp->hal, pfilepath);
	else
		PHL_INFO("%s: pfilepath null\n", __FUNCTION__);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_file_mask_load(
	struct mp_context *mp, char *pfilepath)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	if (pfilepath)
		hal_status = rtw_hal_efuse_bt_file_mask_load(mp->hal, pfilepath);
	else
		PHL_INFO("%s: pfilepath null\n", __FUNCTION__);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_read_hidden(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	u8 value = (u8)arg->io_value;

	PHL_INFO("%s\n", __FUNCTION__);
	PHL_INFO("%s: offset = %x size = %x\n", __FUNCTION__, arg->io_offset, arg->io_type);

	hal_status = rtw_hal_efuse_bt_read_hidden(mp->hal, arg->io_offset, arg->io_type,
						&value);
	arg->io_value = value;

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: io_value = %d\n", __FUNCTION__, arg->io_value);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_write_hidden(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);
	PHL_INFO("%s: offset = %x value = %x\n", __FUNCTION__, arg->io_offset, arg->io_value);

	hal_status = rtw_hal_efuse_bt_write_hidden(mp->hal,arg->io_offset,
							(u8)arg->io_value);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_read_phy_map(
	struct mp_context *mp, struct mp_efuse_arg *arg, u8 type)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);
	PHL_INFO("%s: offset = %x size = %x\n", __FUNCTION__, arg->io_offset, arg->buf_len);

	hal_status = rtw_hal_efuse_read_phy_efuse(mp->hal, arg->io_offset, arg->buf_len,
						arg->poutbuf, type);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_renew(
	struct mp_context *mp, struct mp_efuse_arg *arg, u8 type)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_renew(mp->hal, type);

	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_wifi_get_mask_buf(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	u32 masklen = 0;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_get_mask_buf(mp->hal,
					arg->poutbuf, &masklen);

	arg->buf_len = (u16)masklen;
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_efuse_bt_get_mask_buf(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	u32 masklen = 0;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_efuse_bt_get_mask_buf(mp->hal,
					arg->poutbuf, &masklen);
	arg->buf_len = (u16)masklen;
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

#endif /* CONFIG_HAL_TEST_MP */
