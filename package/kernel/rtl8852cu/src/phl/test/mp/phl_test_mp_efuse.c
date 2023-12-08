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
#define _PHL_TEST_MP_EFUSE_C_
#include "../../phl_headers.h"
#include "phl_test_mp_def.h"
#include "../../hal_g6/test/mp/hal_test_mp_api.h"

#ifdef CONFIG_PHL_TEST_MP
static enum rtw_phl_status phl_mp_efuse_wifi_shadow_read(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_wifi_shadow_read(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_wifi_shadow_write(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_wifi_shadow_write(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_wifi_shadow_update(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_wifi_shadow_update(mp);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_wifi_shadow_reload(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_wifi_shadow_reload(mp);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_wifi_get_offset_mask(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_wifi_get_offset_mask(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_wifi_get_usage(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_wifi_get_usage(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_wifi_get_log_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_wifi_get_log_size(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_wifi_get_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_wifi_get_size(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_get_avl_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_get_avl_size(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_get_shadowmap_from_val(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_get_shadowmap_from_val(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_autoload_status(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_autoload_status(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_shadow_map2buf(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_shadow2buf(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_file_map_load(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	if (_os_strlen(arg->pfile_path) > 0)
		hal_status = rtw_hal_mp_efuse_file_map_load(mp, (char*)arg->pfile_path);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_file_mask_load(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	if (_os_strlen(arg->pfile_path) > 0)
		hal_status = rtw_hal_mp_efuse_file_mask_load(mp, (char*)arg->pfile_path);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_get_info(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_get_info(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

/*BT Efuses*/
static enum rtw_phl_status phl_mp_efuse_bt_shadow_read(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_shadow_read(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_shadow_write(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_shadow_write(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_shadow_update(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_shadow_update(mp);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_shadow_reload(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_shadow_reload(mp);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_get_offset_mask(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_get_offset_mask(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_get_usage(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_get_usage(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_get_log_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_get_log_size(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_get_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_get_size(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_get_avl_size(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_get_avl_size(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_shadow_map2buf(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_shadow2buf(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_file_map_load(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	if (_os_strlen(arg->pfile_path) > 0)
		hal_status = rtw_hal_mp_efuse_bt_file_map_load(mp, (char*)arg->pfile_path);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_file_mask_load(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	if (_os_strlen(arg->pfile_path) > 0)
		hal_status = rtw_hal_mp_efuse_bt_file_mask_load(mp, (char*)arg->pfile_path);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_read_hidden(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_read_hidden(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_write_hidden(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_write_hidden(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_raw_read(
	struct mp_context *mp, struct mp_efuse_arg *arg, u8 type)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_read_phy_map(mp, arg, type);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_renew(
	struct mp_context *mp, struct mp_efuse_arg *arg, u8 type)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_renew(mp, arg, type);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_wifi_get_mask_buf(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_wifi_get_mask_buf(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_efuse_bt_get_mask_buf(
	struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_efuse_bt_get_mask_buf(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_efuse_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status mp_efuse(struct mp_context *mp, struct mp_efuse_arg *arg)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	switch(arg->cmd) {
	case MP_EFUSE_CMD_WIFI_READ:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_READ\n", __FUNCTION__);
		phl_status = phl_mp_efuse_wifi_shadow_read(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_WRITE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_WRITE\n", __FUNCTION__);
		phl_status = phl_mp_efuse_wifi_shadow_write(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_UPDATE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_UPDATE\n", __FUNCTION__);
		phl_status = phl_mp_efuse_wifi_shadow_update(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_UPDATE_MAP:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_UPDATE_MAP\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_wifi_shadow_reload(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_GET_OFFSET_MASK:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_GET_OFFSET_MASK\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_wifi_get_offset_mask(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_GET_USAGE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_GET_USAGE\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_wifi_get_usage(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_GET_LOG_SIZE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_GET_LOG_SIZE\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_wifi_get_log_size(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_GET_SIZE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_GET_SIZE\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_wifi_get_size(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_GET_AVL_SIZE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_GET_AVL_SIZE\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_get_avl_size(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_GET_MAP_FROM:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_GET_MAP_FROM\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_get_shadowmap_from_val(mp, arg);
		break;
	case MP_EFUSE_CMD_AUTOLOAD_STATUS:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_AUTOLOAD_STATUS\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_autoload_status(mp, arg);
		break;
	case MP_EFUSE_CMD_SHADOW_MAP2BUF:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_SHADOW_MAP2BUF\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_shadow_map2buf(mp, arg);
		break;
	case MP_EFUSE_CMD_FILE_MAP_LOAD:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_FILE_MAP_LOAD\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_file_map_load(mp, arg);
		break;
	case MP_EFUSE_CMD_FILE_MASK_LOAD:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_FILE_MASK_LOAD\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_file_mask_load(mp, arg);
		break;
	case MP_EFUSE_CMD_GET_INFO:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_GET_INFO\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_get_info(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_READ:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_READ\n", __FUNCTION__);
		phl_status = phl_mp_efuse_bt_shadow_read(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_WRITE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_WRITE\n", __FUNCTION__);
		phl_status = phl_mp_efuse_bt_shadow_write(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_UPDATE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_UPDATE\n", __FUNCTION__);
		phl_status = phl_mp_efuse_bt_shadow_update(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_UPDATE_MAP:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_UPDATE_MAP\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_shadow_reload(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_GET_OFFSET_MASK:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_GET_OFFSET_MASK\n",
			__FUNCTION__);
		phl_status = phl_mp_efuse_bt_get_offset_mask(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_GET_USAGE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_GET_USAGE\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_get_usage(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_GET_LOG_SIZE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_GET_LOG_SIZE\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_get_log_size(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_GET_SIZE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_GET_SIZE\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_get_size(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_GET_AVL_SIZE:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_GET_AVL_SIZE\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_get_avl_size(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_SHADOW_MAP2BUF:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_SHADOW_MAP2BUF\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_shadow_map2buf(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_FILE_MAP_LOAD:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_FILE_MAP_LOAD\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_file_map_load(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_FILE_MASK_LOAD:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_FILE_MASK_LOAD\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_file_mask_load(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_READ_HIDDEN:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_READ_HIDDEN\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_read_hidden(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_WRITE_HIDDEN:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_WRITE_HIDDEN\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_write_hidden(mp, arg);
		break;
	case MP_EFUSE_CMD_WIFI_GET_PHY_MAP:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_GET_PHY_MAP\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_raw_read (mp, arg, PHL_MP_EFUSE_WIFI);
		break;
	case MP_EFUSE_CMD_BT_GET_PHY_MAP:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_BT_GET_PHY_MAP\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_raw_read(mp, arg, PHL_MP_EFUSE_BT);
		break;
	case MP_EFUSE_CMD_WIFI_SET_RENEW:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_SET_RENEW\n",
		 __FUNCTION__);
		phl_status = phl_mp_efuse_renew(mp, arg, PHL_MP_EFUSE_WIFI);
		break;
	case MP_EFUSE_CMD_WIFI_GET_MASK_BUF:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_GET_MASK_BUF\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_wifi_get_mask_buf(mp, arg);
		break;
	case MP_EFUSE_CMD_BT_GET_MASK_BUF:
		PHL_INFO("%s: CMD = MP_EFUSE_CMD_WIFI_GET_MASK_BUF\n",
			 __FUNCTION__);
		phl_status = phl_mp_efuse_bt_get_mask_buf(mp, arg);
		break;
	default:
		PHL_INFO("%s: UNKNOWN CMD = %d\n", __FUNCTION__, arg->cmd);
		break;
	}
	return phl_status;
}
#endif /* CONFIG_PHL_TEST_MP */
