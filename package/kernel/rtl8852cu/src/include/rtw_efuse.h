/******************************************************************************
 *
 * Copyright(c) 2007 - 2020 Realtek Corporation.
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
#if defined(CONFIG_MP_INCLUDED)
#ifndef __RTW_EFUSE_H__
#define __RTW_EFUSE_H__
#include <drv_types.h>
#include <rtw_mp.h>

#define RTW_MAX_EFUSE_MAP_LEN 2048

enum rtw_efuse_type {
	RTW_EFUSE_WIFI = 0,
	RTW_EFUSE_BT,
	RTW_EFUSE_NONE,
};

/* PHL efuse command */
enum rtw_efuse_phl_cmdid {
	RTW_EFUSE_CMD_WIFI_READ = 0,
	RTW_EFUSE_CMD_WIFI_WRITE = 1,
	RTW_EFUSE_CMD_WIFI_UPDATE = 2,
	RTW_EFUSE_CMD_WIFI_UPDATE_MAP = 3,
	RTW_EFUSE_CMD_WIFI_GET_OFFSET_MASK = 4,
	RTW_EFUSE_CMD_WIFI_GET_USAGE = 5,
	RTW_EFUSE_CMD_BT_READ = 6,
	RTW_EFUSE_CMD_BT_WRITE = 7,
	RTW_EFUSE_CMD_BT_UPDATE = 8,
	RTW_EFUSE_CMD_BT_UPDATE_MAP = 9,
	RTW_EFUSE_CMD_BT_GET_OFFSET_MASK = 10,
	RTW_EFUSE_CMD_BT_GET_USAGE = 11,
	RTW_EFUSE_CMD_WIFI_GET_LOG_SIZE = 12,
	RTW_EFUSE_CMD_WIFI_GET_SIZE = 13,
	RTW_EFUSE_CMD_WIFI_GET_AVL_SIZE = 14,
	RTW_EFUSE_CMD_AUTOLOAD_STATUS = 15,
	RTW_EFUSE_CMD_SHADOW_MAP2BUF = 16,
	RTW_EFUSE_CMD_FILE_MAP_LOAD = 17,
	RTW_EFUSE_CMD_FILE_MASK_LOAD = 18,
	RTW_MP_EFUSE_CMD_GET_INFO = 19,
	/* BT */
	RTW_EFUSE_CMD_BT_GET_LOG_SIZE = 20,
	RTW_EFUSE_CMD_BT_GET_SIZE = 21,
	RTW_EFUSE_CMD_BT_GET_AVL_SIZE = 22,
	RTW_EFUSE_CMD_BT_SHADOW_MAP2BUF = 23,
	RTW_EFUSE_CMD_BT_FILE_MAP_LOAD = 24,
	RTW_EFUSE_CMD_BT_FILE_MASK_LOAD = 25,
	RTW_EFUSE_CMD_BT_READ_HIDDEN = 26,
	RTW_EFUSE_CMD_BT_WRITE_HIDDEN = 27,
	RTW_MP_EFUSE_CMD_WIFI_GET_MAP_FROM =28,
	RTW_EFUSE_CMD_WIFI_GET_PHY_MAP = 29,
	RTW_EFUSE_CMD_BT_GET_PHY_MAP = 30,
	RTW_MP_EFUSE_CMD_WIFI_SET_RENEW = 31,
	RTW_EFUSE_CMD_WIFI_GET_MASK_BUF = 32,
	RTW_EFUSE_CMD_BT_GET_MASK_BUF = 33,
	RTW_EFUSE_CMD_MAX,
};

struct rtw_efuse_phl_arg {
	u8 mp_class;
	u8 cmd;
	u8 cmd_ok;
	u8 status;
	u8 io_type;
	u16 io_offset;
	u32 io_value;
	u8 autoload;
	u8 pfile_path[200];
	u16 buf_len;
	u8 poutbuf[1536];
};

enum RTW_EFUSE_MAP_STATUS {
	RTW_DEFAULT_MAP = 0,
	RTW_HW_LOG_MAP = 1,
	RTW_FILE_MAP = 2,
	RTW_EFUSE_UNKNOWN,
};

#define RTW_EFUSE_FROM2STR(status)\
(status == RTW_DEFAULT_MAP) ? "DEFAULT" :\
(status == RTW_HW_LOG_MAP) ? "HW_LOG_EFUSE" :\
(status == RTW_FILE_MAP) ? "FILE_EFUSE" :\
"UNknow"

s8 rtw_efuse_get_map_from(_adapter *padapter);

u32 rtw_efuse_get_map_size(_adapter *padapter , u16 *size , enum rtw_efuse_phl_cmdid cmdid);

u32 rtw_efuse_get_available_size(_adapter *padapter , u16 *size, u8 efuse_type);

u8 rtw_efuse_map_read(_adapter * adapter, u16 addr, u16 cnts, u8 *data, u8 efuse_type);

u8 rtw_efuse_map_write(_adapter * adapter, u16 addr, u16 cnts, u8 *data, u8 efuse_type, u8 bpg);

int rtw_ioctl_efuse_get(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra);

int rtw_ioctl_efuse_set(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wdata, char *extra);

int rtw_ioctl_efuse_file_map_load(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra);

int rtw_ioctl_efuse_file_mask_load(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra);

int rtw_ioctl_efuse_bt_file_map_load(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra);

int rtw_ioctl_efuse_bt_file_mask_load(struct net_device *dev,
				struct iw_request_info *info,
				union iwreq_data *wrqu, char *extra);

u8 rtw_efuse_raw_map_read(_adapter * adapter, u16 addr,
	            u16 cnts, u8 *data, u8 efuse_type);

u8 rtw_efuse_bt_write_raw_hidden(_adapter * adapter, u16 addr,
				u16 cnts, u8 *data);
#endif

#endif /*#if defined(CONFIG_MP_INCLUDED)*/
