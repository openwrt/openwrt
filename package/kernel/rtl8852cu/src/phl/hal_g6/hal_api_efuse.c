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
#define _HAL_API_EFUSE_C_
#include "hal_headers.h"
#include "efuse/hal_efuse_export.h"

/*WIFI Efuse*/
enum rtw_hal_status
rtw_hal_efuse_shadow_load(struct hal_info_t *hal_info, bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_shadow_load(hal_info->efuse, is_limit);

	return status;
}

enum rtw_hal_status
rtw_hal_efuse_shadow_update(struct hal_info_t *hal_info, bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_EFUSE_PG_FAIL;

	status = rtw_efuse_shadow_update(hal_info->efuse, is_limit);

	return status;
}

enum rtw_hal_status
rtw_hal_efuse_shadow_read(struct hal_info_t *hal_info, u8 byte_count,
						  u16 offset, u32 *value, bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_shadow_read(hal_info->efuse, byte_count, offset, value,
								   is_limit);

	return status;
}

enum rtw_hal_status
rtw_hal_efuse_shadow_write(struct hal_info_t *hal_info, u8 byte_count,
						   u16 offset, u32 value, bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_shadow_write(hal_info->efuse, byte_count, offset, value,
									is_limit);
	return status;
}

enum rtw_hal_status
rtw_hal_efuse_shadow2buf(struct hal_info_t *hal_info, u8 *pbuf, u16 buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;

	status = rtw_efuse_shadow2buf(hal_info->efuse, pbuf, buflen);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_file_map_load(
	struct hal_info_t *hal_info, char *file_path, u8 is_limit)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_efuse_file_map_load(hal_info->efuse, file_path ,is_limit);

	return hal_status;
}

enum rtw_hal_status rtw_hal_efuse_file_mask_load(
	struct hal_info_t *hal_info, char *file_path, u8 is_limit)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_efuse_file_mask_load(hal_info->efuse, file_path, is_limit);

	return hal_status;
}

/* usage = used percentage(1 Byte) + used bytes(2 Bytes) */
enum rtw_hal_status rtw_hal_efuse_get_usage(struct hal_info_t *hal_info,
	u32 *usage)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_get_usage(hal_info->efuse, usage);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_get_logical_size(struct hal_info_t *hal_info,
	u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_get_logical_size(hal_info->efuse, size, true);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_get_size(struct hal_info_t *hal_info,
	u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_get_size(hal_info->efuse, size);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_get_avl(struct hal_info_t *hal_info,
	u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_get_avl(hal_info->efuse, size);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_get_shadowmap_from(struct hal_info_t *hal_info,
	u8 *val)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_get_shadowmap_from(hal_info->efuse, val);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_get_offset_mask(struct hal_info_t *hal_info,
	u16 offset, u8 *mask)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_get_offset_mask(hal_info->efuse, offset, mask);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_get_mask_buf(struct hal_info_t *hal_info, 
							u8 *destbuf, u32 *buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_get_mask_buf(hal_info->efuse, destbuf, buflen);

	return status;
}

enum rtw_hal_status
rtw_hal_efuse_get_info(struct rtw_hal_com_t *hal_com,
		       enum rtw_efuse_info info_type,
		       void *value,
		       u8 size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = hal_com->hal_priv;

	status = rtw_efuse_get_info(hal_info->efuse, info_type, value, size);

	return status;
}

/* API export to PHL : rtw_hal_get_efuse_info */
enum rtw_hal_status
rtw_hal_get_efuse_info(void *hal,
		       enum rtw_efuse_info info_type,
		       void *value,
		       u8 size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	status = rtw_efuse_get_info(hal_info->efuse, info_type, value, size);

	return status;
}

void rtw_hal_efuse_process(struct rtw_phl_com_t *phl_com,
                           struct hal_info_t *hal_info,
                           char *ic_name
)
{
	if(rtw_efuse_is_processed(hal_info->efuse) == true) {
		PHL_INFO("%s EFUSE module is already initialized.\n", __FUNCTION__);
		return;
	}

#ifdef CONFIG_PHL_FW_DUMP_EFUSE
	rtw_phl_fw_dump_efuse_precfg(phl_com);
#endif

	rtw_efuse_process(hal_info->efuse, ic_name);

#ifdef CONFIG_PHL_FW_DUMP_EFUSE
	rtw_phl_fw_dump_efuse_postcfg(phl_com);
#endif
}

enum rtw_hal_status rtw_hal_efuse_init(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal_info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	status = rtw_efuse_init(phl_com, hal_com, &(hal_info->efuse));

	return status;
}

void rtw_hal_efuse_deinit(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal_info)
{
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	rtw_efuse_deinit(hal_com, hal_info->efuse);
}


enum rtw_hal_status
rtw_hal_efuse_bt_shadow_load(struct hal_info_t *hal_info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_bt_shadow_load(hal_info->efuse);

	return status;
}

enum rtw_hal_status
rtw_hal_efuse_bt_shadow_update(struct hal_info_t *hal_info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_EFUSE_PG_FAIL;

	status = rtw_efuse_bt_shadow_update(hal_info->efuse);

	return status;
}

enum rtw_hal_status
rtw_hal_efuse_bt_shadow_read(struct hal_info_t *hal_info, u8 byte_count,
						  u16 offset, u32 *value)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_bt_shadow_read(hal_info->efuse, byte_count, offset, value);

	return status;
}

enum rtw_hal_status
rtw_hal_efuse_bt_shadow_write(struct hal_info_t *hal_info, u8 byte_count,
						   u16 offset, u32 value)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_bt_shadow_write(hal_info->efuse, byte_count, offset, value);
	return status;
}


enum rtw_hal_status
rtw_hal_efuse_bt_shadow2buf(struct hal_info_t *hal_info, u8 *pbuf, u16 buflen)
{
   enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;

   status = rtw_efuse_bt_shadow2buf(hal_info->efuse, pbuf, buflen);

   return status;
}

enum rtw_hal_status rtw_hal_efuse_bt_file_map_load(
	struct hal_info_t *hal_info, char *file_path)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_efuse_bt_file_map_load(hal_info->efuse, file_path);

	return hal_status;
}


enum rtw_hal_status rtw_hal_efuse_bt_file_mask_load(
	struct hal_info_t *hal_info, char *file_path)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_efuse_bt_file_mask_load(hal_info->efuse, file_path);

	return hal_status;
}

/* usage = used percentage(1 Byte) + used bytes(2 Bytes) */
enum rtw_hal_status rtw_hal_efuse_bt_get_usage(struct hal_info_t *hal_info,
	u32 *usage)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_get_usage(hal_info->efuse, usage);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_bt_get_logical_size(struct hal_info_t *hal_info,
	u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_bt_get_logical_size(hal_info->efuse, size);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_bt_get_size(struct hal_info_t *hal_info,
	u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_bt_get_size(hal_info->efuse, size);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_bt_get_avl(struct hal_info_t *hal_info,
	u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_bt_get_avl(hal_info->efuse, size);

	return status;
}


enum rtw_hal_status rtw_hal_efuse_bt_get_offset_mask(struct hal_info_t *hal_info,
	u16 offset, u8 *mask)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_bt_get_offset_mask(hal_info->efuse, offset, mask);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_bt_get_mask_buf(struct hal_info_t *hal_info,
							u8 *destbuf, u32 *buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_bt_get_mask_buf(hal_info->efuse, destbuf, buflen);

	return status;
}
enum rtw_hal_status rtw_hal_efuse_bt_read_hidden(
	struct hal_info_t *hal_info, u32 addr, u32 size, u8 *val)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_efuse_bt_read_hidden(hal_info->efuse, addr, size, val);

	return hal_status;
}

enum rtw_hal_status rtw_hal_efuse_bt_write_hidden(
	struct hal_info_t *hal_info, u32 addr, u8 val)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_efuse_bt_write_hidden(hal_info->efuse, addr, val);

	return hal_status;
}

enum rtw_hal_status rtw_hal_efuse_read_phy_efuse(
	struct hal_info_t *hal_info, u32 addr, u32 size, u8 *data, u8 type)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_read_phy_wifi(hal_info->efuse, addr, size, data, type);

	return status;
}

enum rtw_hal_status rtw_hal_efuse_renew(
	struct hal_info_t *hal_info, u8 type)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_efuse_renew(hal_info->efuse, type);

	return status;
}

void hal_efuse_dump_wifi_map(
	struct hal_info_t *hal_info,
	u32 *used_len,
	char *output,
	u32 *out_len
	)
{
	void *d = hal_info->hal_com->drv_priv;
	u32 _used_len = *used_len;
	u32 _out_len = *out_len;
	u32 map_len = 0;
	u32 i = 0, j = 0;
	u8 *buf = NULL;

	rtw_efuse_get_logical_size(hal_info->efuse, &map_len, true);

	if(map_len == 0){
		PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
				 _out_len - _used_len, "map length is 0!\n");
		goto exit;
	}

	buf = _os_mem_alloc(d, map_len);

	if(buf == NULL){
		PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
				 _out_len - _used_len, "Allocate buffer fail!\n");
		goto exit;
	}
	rtw_efuse_shadow2buf(hal_info->efuse, buf, (u16)map_len);

	PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len, _out_len - _used_len,
			 "EFUSE Wifi shadow map from %s\n\n",
			 rtw_efuse_get_shadowmap_from_to_str(hal_info->efuse));

	PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len, _out_len - _used_len,
			 "%14s%-6s%-6s%-6s%-6s%-6s%-6s%-6s%-6s\n",
			 " ", "00", "02", "04", "06", "08", "0A", "0C", "0E");
	PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len, _out_len - _used_len,
			 "%14s%s\n",
			 " ", "================================================\n");
	for(i = 0; i < map_len; i+=16){
		PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
				 _out_len - _used_len, "0x%08X%4s", i, "");
		for(j = 0; j<16;j++){
			if((i+j) >= map_len)
				break;
			PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
					 _out_len - _used_len, "%02X ", buf[i+j]);
		}
		PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
				 _out_len - _used_len, "\n");
	}
	if(buf)
		_os_mem_free(d, buf, map_len);
exit:
	*used_len = _used_len;
	*out_len = _out_len;
}

void hal_efuse_dump_wifi_logic_map(
	struct hal_info_t *hal_info,
	u32 *used_len,
	char *output,
	u32 *out_len
	)
{
	void *d = hal_info->hal_com->drv_priv;
	u32 _used_len = *used_len;
	u32 _out_len = *out_len;
	u32 map_len = 0;
	u32 i = 0, j = 0;
	u8 *data = NULL;

	rtw_efuse_get_logical_size(hal_info->efuse, &map_len, true);

	if(map_len == 0){
		PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
				 _out_len - _used_len, "map length is 0!\n");
		goto exit;
	}

	data = _os_mem_alloc(d, map_len);

	if(data == NULL){
		PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
				 _out_len - _used_len, "Allocate buffer fail!\n");
		goto exit;
	}

	rtw_efuse_logicmap_buf_load(hal_info->efuse, data, false);

	PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len, _out_len - _used_len,
			 "EFUSE Wifi map from HW\n");

	PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len, _out_len - _used_len,
			 "%14s%-6s%-6s%-6s%-6s%-6s%-6s%-6s%-6s\n",
			 " ", "00", "02", "04", "06", "08", "0A", "0C", "0E");
	PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len, _out_len - _used_len,
			 "%14s%s\n",
			 " ", "================================================\n");
	for(i = 0; i < map_len; i+=16){
		PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
				 _out_len - _used_len, "0x%08X%4s", i, "");
		for(j = 0; j<16;j++){
			if((i+j) >= map_len)
				break;
			PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
					 _out_len - _used_len, "%02X ", data[i+j]);
		}
		PHL_DBG_OUTBUF(_out_len, _used_len, output + _used_len,
				 _out_len - _used_len, "\n");
	}
	if(data)
		_os_mem_free(d, data, map_len);
exit:
	*used_len = _used_len;
	*out_len = _out_len;
}

void hal_efuse_cmd_parser(
	struct hal_info_t *hal_info,
	char input[][MAX_ARGV],
	u32 input_num,
	char *output,
	u32 out_len
	)
{
	u32 hal_cmd_ary_size = sizeof(hal_efuse_cmd_i) / sizeof(struct hal_cmd_info);
	u32 i = 0;
	u8 id = 0;
	u32 used = 0;

	PHL_DBG_OUTBUF(out_len, used, output + used, out_len - used, "\n");

	if (hal_cmd_ary_size == 0)
		return;

	/* Parsing Cmd ID */
	if (input_num) {
		for (i = 0; i < hal_cmd_ary_size; i++) {
			if (_os_strcmp(hal_efuse_cmd_i[i].name, input[0]) == 0) {
				id = hal_efuse_cmd_i[i].id;
				PHL_INFO("enter EFUSE cmd %s\n", hal_efuse_cmd_i[i].name);
				break;
			}
		}
		if (i == hal_cmd_ary_size) {
			PHL_DBG_OUTBUF(out_len, used, output + used,
					 out_len - used,
					 "EFUSE command not found!\n");
			return;
		}
	}

	switch (id) {
		case HAL_EFUSE_HELP:
			PHL_DBG_OUTBUF(out_len, used, output + used,
					 out_len - used,
					 "EFUSE cmd =>\n");
			for (i = 0; i < hal_cmd_ary_size - 1; i++)
				PHL_DBG_OUTBUF(out_len, used, output + used,
						 out_len - used," %s\n", hal_efuse_cmd_i[i].name);
			break;
		case HAL_EFUSE_WIFI_DUMP_MAP:
			hal_efuse_dump_wifi_map(hal_info, &used, output, &out_len);
			break;
		case HAL_EFUSE_WIFI_LOGIC_DUMP_MAP:
			hal_efuse_dump_wifi_logic_map(hal_info, &used, output, &out_len);
			break;
		default:
			PHL_DBG_OUTBUF(out_len, used, output + used,
					 out_len - used,"Do not support this command\n");
			break;
	}
}

s32 hal_efuse_cmd(
	struct hal_info_t *hal_info,
	char *input,
	char *output,
	u32 out_len
	)
{
	char *token;
	u32 argc = 0;
	char argv[MAX_ARGC][MAX_ARGV];

	do {
		token = _os_strsep(&input, ", ");
		if (token) {
			if (_os_strlen((u8*)token) <= MAX_ARGV)
				_os_strcpy(argv[argc], token);

			argc++;
		} else {
			break;
		}
	} while (argc < MAX_ARGC);

	hal_efuse_cmd_parser(hal_info, argv, argc, output, out_len);

	return 0;
}

bool rtw_hal_efuse_proc_cmd(
	struct hal_info_t *hal_info,
	struct rtw_proc_cmd *incmd,
	char *output,
	u32 out_len
	)
{
	if(incmd->in_type == RTW_ARG_TYPE_BUF)
		hal_efuse_cmd(hal_info, incmd->in.buf, output, out_len);
	else if(incmd->in_type == RTW_ARG_TYPE_ARRAY){
		hal_efuse_cmd_parser(hal_info, incmd->in.vector,
					incmd->in_cnt_len, output, out_len);
	}

	return true;
}