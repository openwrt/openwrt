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
/*
The purpose of hal_efuse.c

Provide efuse operations.
a. efuse init function
b. efuse shadow map read/write/update
c. efuse information query, map size/used bytes...

*/

#define _HAL_EFUSE_C_
#include "../hal_headers.h"
#include "hal_efuse.h"
#include "hal_efuse_export.h"

#define get_hci_to_a_die_offset(log_size, limit_log_size) (log_size - limit_log_size)
#define get_a_die_start_offset(limit_log_size, a_die_size) (limit_log_size - a_die_size)

/* WIFI EFUSE API */
void efuse_shadow_read_one_byte(struct efuse_t *efuse, u16 offset, u8 *value)
{
	*value = efuse->shadow_map[offset];
}

void efuse_shadow_read_two_byte(struct efuse_t *efuse, u16 offset, u16 *value)
{
	*value = efuse->shadow_map[offset];
	*value |= efuse->shadow_map[offset+1] << 8;
}

void efuse_shadow_read_four_byte(struct efuse_t *efuse, u16 offset, u32 *value)
{
	*value = efuse->shadow_map[offset];
	*value |= efuse->shadow_map[offset+1] << 8;
	*value |= efuse->shadow_map[offset+2] << 16;
	*value |= efuse->shadow_map[offset+3] << 24;
}

void efuse_shadow_write_one_byte(struct efuse_t *efuse, u16 offset, u16 value)
{
	efuse->shadow_map[offset] = (u8)(value&0x00FF);
}

void efuse_shadow_write_two_byte(struct efuse_t *efuse, u16 offset, u16 value)
{
	efuse->shadow_map[offset] = (u8)(value&0x00FF);
	efuse->shadow_map[offset+1] = (u8)((value&0xFF00) >> 8);
}

void efuse_shadow_write_four_byte(struct efuse_t *efuse, u16 offset, u32 value)
{
	efuse->shadow_map[offset] = (u8)(value&0x000000FF);
	efuse->shadow_map[offset+1] = (u8)((value&0x0000FF00) >> 8);
	efuse->shadow_map[offset+2] = (u8)((value&0x00FF0000) >> 16);
	efuse->shadow_map[offset+3] = (u8)((value&0xFF000000) >> 24);
}

u32 efuse_check_autoload(struct efuse_t *efuse)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mac_check_efuse_autoload(efuse->hal_com,
												  &efuse->is_map_valid);
	if (efuse->is_map_valid != true)
		efuse->map_from_status = DEFAULT_MAP;

	return hal_status;
}

u32 efuse_hidden_handle(struct efuse_t *efuse)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mac_read_hidden_rpt(efuse->hal_com);

	return hal_status;
}

enum rtw_hal_status efuse_set_hw_cap(struct efuse_t *efuse)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_com_t *hal_com = efuse->hal_com;
	u8 pkg_type = 0xFF;
	u8 rfe_type = 0xFF;
	u8 xcap = 0xFF;
	u8 domain = 0xFF;
	u8 domain_6g = 0xFF;
	u8 rf_board_opt = 0xFF;

	status = rtw_efuse_get_info(efuse, EFUSE_INFO_RF_PKG_TYPE, &pkg_type,
				    sizeof(pkg_type));

	if(status != RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("%s: Get pkg type fail! Status(%x)\n", __FUNCTION__, status);
	}

	status = rtw_efuse_get_info(efuse, EFUSE_INFO_RF_RFE, &rfe_type,
				    sizeof(rfe_type));

	if(status != RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("%s: Get rfe type fail! Status(%x)\n", __FUNCTION__, status);
	}

	status = rtw_efuse_get_info(efuse, EFUSE_INFO_RF_XTAL, &xcap,
				    sizeof(xcap));

	if(status != RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("%s: Get xcap fail! Status(%x)\n", __FUNCTION__, status);
	}

	status = rtw_efuse_get_info(efuse, EFUSE_INFO_RF_CHAN_PLAN, &domain,
				    sizeof(domain));

	if(status != RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("%s: Get domain fail! Status(%x)\n", __FUNCTION__, status);
	}

	status = rtw_efuse_get_info(efuse, EFUSE_INFO_RF_CHAN_PLAN_6GHZ,
					&domain_6g, sizeof(domain_6g));

	if(status != RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("%s: Get 6ghz domain fail! Status(%x)\n", __FUNCTION__, status);
	}

	status = rtw_efuse_get_info(efuse, EFUSE_INFO_RF_BOARD_OPTION, &rf_board_opt,
				sizeof(rf_board_opt));

	if(status != RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("%s: Get domain fail! Status(%x)\n", __FUNCTION__, status);
	}

	hal_com->dev_hw_cap.pkg_type = pkg_type;
	hal_com->dev_hw_cap.rfe_type = rfe_type;
	hal_com->dev_hw_cap.xcap = xcap;
	hal_com->dev_hw_cap.domain = domain;
	hal_com->dev_hw_cap.domain_6g = domain_6g;
	hal_com->dev_hw_cap.rf_board_opt = rf_board_opt;

	return status;
}

/*
 * This function is used for backward compatible by calling RF-API.
 * ex:
 *     In 1T2R setting, backward to 1T1R by checking rfe_type
 *     RFE_43(efuse_x02CA=0x2B): 1T2R(Tx diversity + Rx MRC)
 *     RFE_41(efuse_x02CA=0x29): 1T1R(Tx/Rx@pathB)
 *
 */
void efuse_compatible_chk(struct efuse_t *efuse)
{
	rtw_hal_rf_rfe_ant_num_chk(efuse->hal_com);
}

enum rtw_hal_status rtw_efuse_logicmap_buf_load(void *efuse, u8* buf, bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_read_log_efuse_map(efuse_info->hal_com,
						buf,
						is_limit);
	return status;
}

enum rtw_hal_status rtw_efuse_shadow_load(void *efuse, bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_read_log_efuse_map(efuse_info->hal_com,
											efuse_info->shadow_map,
											is_limit);

	if (efuse_info->is_map_valid == true && status == RTW_HAL_STATUS_SUCCESS)
		efuse_info->map_from_status = HW_LOG_MAP;

	return status;
}

enum rtw_hal_status rtw_efuse_shadow_file_load(void *efuse, char *ic_name, bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;

#ifdef CONFIG_EFUSE_CONFIG_FILE
	struct efuse_t *efuse_info = efuse;

	if (efuse_info->is_map_valid != true ||
		rtw_hal_rf_check_efuse_data(efuse_info->hal_com, HW_PHY_0) != true) {

		if (rtw_hal_efuse_shadow_file_load(efuse_info->hal_com ,
						ic_name, is_limit) == RTW_HAL_STATUS_SUCCESS) {

			if (rtw_hal_rf_check_efuse_data(efuse_info->hal_com, HW_PHY_0) == true) {
				efuse_info->is_map_valid = true;
				PHL_INFO(" %s() hal_rf check file efuse is_map_valid.\n", __FUNCTION__);
			} else {
				status = RTW_HAL_STATUS_FAILURE;
				PHL_WARN(" %s() efuse Power invalid !\n", __FUNCTION__);
			}
		} else {
			PHL_WARN("%s: efuse shadow_file_load fail!\n", __FUNCTION__);
			status = RTW_HAL_STATUS_FAILURE;
		}
	}
#endif
	return status;
}

enum rtw_hal_status rtw_efuse_shadow_update(void *efuse, bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_EFUSE_PG_FAIL;
	enum rtw_hal_status reload_status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	u32 map_size = 0, mask_size = 0;

	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == false) {
		PHL_WARN("%s: efuse map not load yet!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_UNINIT;
		goto exit;
	}

	/* Load efuse mask file before PG */
	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_MASK_FILE_LOADED) == false) {
		PHL_WARN("%s: efuse mask not load yet!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_PG_FAIL;
		goto exit;
	}

	/*
	 * If driver does not load external map file,
	 * assign the map version with the mask version for the version check.
	 */
	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_MAP_FILE_LOADED) == false) {
		_os_mem_cpy(efuse_info->hal_com->drv_priv, efuse_info->map_version,
					efuse_info->mask_version, efuse_info->version_len);
	}

	if(is_limit) {
		map_size = efuse_info->limit_efuse_size;
		mask_size = efuse_info->limit_mask_size;
	}
	else {
		map_size = efuse_info->log_efuse_size;
		mask_size = efuse_info->mask_size;
	}

	status = rtw_hal_mac_write_log_efuse_map(efuse_info->hal_com,
							efuse_info->shadow_map,
							map_size,
							efuse_info->mask,
							mask_size,
							efuse_info->map_version,
							efuse_info->mask_version,
							efuse_info->version_len,
							0,
							is_limit,
							efuse_info->efuse_a_die_size,
							efuse_info->hci_to_a_die_offset,
							efuse_info->a_die_start_offset);

	if(status != RTW_HAL_STATUS_SUCCESS)
		PHL_WARN("%s: PG Fail!\n", __FUNCTION__);

	/* Reload shadow map after PG */
	reload_status = rtw_hal_mac_read_log_efuse_map(efuse_info->hal_com,
								efuse_info->shadow_map,
								is_limit);
	if(reload_status != RTW_HAL_STATUS_SUCCESS)
		PHL_WARN("%s: Reload shadow map Fail!\n", __FUNCTION__);

	/*
	 * Clear the load external map file flag and map version
	 * after reloading the map.
	 */
	if(TEST_STATUS_FLAG(efuse_info->status,
						EFUSE_STATUS_MAP_FILE_LOADED) == true) {
		CLEAR_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_MAP_FILE_LOADED);
	}

	_os_mem_set(efuse_info->hal_com->drv_priv,
				efuse_info->map_version,
				0,
				efuse_info->version_len);
exit:
	return status;
}

enum rtw_hal_status
rtw_efuse_shadow_read(void *efuse, u8 byte_count, u16 offset, u32 *value,
					  bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	u32	efuse_size = 0;

	if(is_limit)
		efuse_size = efuse_info->limit_efuse_size;
	else
		efuse_size = efuse_info->log_efuse_size;

	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == false) {
		PHL_WARN("%s: efuse map not load yet!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_UNINIT;
		goto exit;
	}

	if((u32)(offset+byte_count) > efuse_size) {
		PHL_WARN("%s: Invalid offset!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_IVALID_OFFSET;
		goto exit;
	}

	if (efuse_info->efuse_a_die_size != 0 &&
		offset >= efuse_info->a_die_start_offset &&
		is_limit)
		offset += (u16)efuse_info->hci_to_a_die_offset;

	if (byte_count == 1)
		efuse_shadow_read_one_byte(efuse_info, offset, (u8 *)value);
	else if (byte_count == 2)
		efuse_shadow_read_two_byte(efuse_info, offset, (u16 *)value);
	else if (byte_count == 4)
		efuse_shadow_read_four_byte(efuse_info, offset, (u32 *)value);

	status = RTW_HAL_STATUS_SUCCESS;
exit:
	return status;
}

enum rtw_hal_status
rtw_efuse_shadow_write(void *efuse, u8 byte_count, u16 offset, u32 value,
					   bool is_limit)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	u32	efuse_size = 0;

	if(is_limit)
		efuse_size = efuse_info->limit_efuse_size;
	else
		efuse_size = efuse_info->log_efuse_size;

	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == false) {
		PHL_WARN("%s: efuse map not load yet!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_UNINIT;
		goto exit;
	}

	if((u32)(offset+byte_count) > efuse_size) {
		PHL_WARN("%s: Invalid offset!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_IVALID_OFFSET;
		goto exit;
	}

	if (efuse_info->efuse_a_die_size != 0 &&
		offset >= efuse_info->a_die_start_offset &&
		is_limit)
		offset += (u16)efuse_info->hci_to_a_die_offset;

	if (byte_count == 1)
		efuse_shadow_write_one_byte(efuse_info, offset, (u8)value);
	else if (byte_count == 2)
		efuse_shadow_write_two_byte(efuse_info, offset, (u16)value);
	else if (byte_count == 4)
		efuse_shadow_write_four_byte(efuse_info, offset, (u32)value);

	status = RTW_HAL_STATUS_SUCCESS;
exit:
	return status;
}


/*
 * This API is used for original mp dll command.
 * usage = used percentage(1 Byte) + used bytes(2 Bytes)
 */
enum rtw_hal_status rtw_efuse_get_usage(void *efuse, u32 *usage)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	u32 avl_size = 0;
	u16 used_byte = 0;
	u32 total_size = 0;
	u8 used_percentage = 0;

	status = rtw_hal_mac_get_efuse_avl(efuse_info->hal_com, &avl_size);
	status = rtw_hal_mac_get_efuse_size(efuse_info->hal_com, &total_size);

	if(total_size != 0){
		used_byte = (u16)(total_size - avl_size);
		used_percentage = (u8)(used_byte*100/total_size);
		*usage = (used_percentage<<16)|(used_byte);
	}

	return status;
}

enum rtw_hal_status rtw_efuse_shadow2buf(void *efuse, u8 *destbuf, u16 buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct efuse_t *efuse_info = efuse;

	_os_mem_cpy(efuse_info->hal_com->drv_priv, (void *)destbuf,
				(void *)efuse_info->shadow_map , buflen);

	return status;
}

enum rtw_hal_status
efuse_map_buf2shadow(struct efuse_t *efuse, u8 *srcbuf, u16 buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;

	_os_mem_cpy(efuse->hal_com->drv_priv, (void *)efuse->shadow_map,
				(void *)srcbuf, buflen);
	SET_STATUS_FLAG(efuse->status, EFUSE_STATUS_MAP_FILE_LOADED);

	return status;
}

enum rtw_hal_status
efuse_file_map2version(struct efuse_t *efuse, u8 *srcbuf, u16 buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;

	_os_mem_cpy(efuse->hal_com->drv_priv, (void *)efuse->map_version,
				(void *)srcbuf, buflen);
	debug_dump_data(efuse->map_version, efuse->version_len,
					"[HAL EFUSE] map version =");

	return status;
}

enum rtw_hal_status
efuse_file_mask2buf(struct efuse_t *efuse, u8 *srcbuf, u16 buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	_os_mem_cpy(efuse->hal_com->drv_priv, (void *)efuse->mask, (void *)srcbuf,
				buflen);
	SET_STATUS_FLAG(efuse->status, EFUSE_STATUS_MASK_FILE_LOADED);
	status = RTW_HAL_STATUS_SUCCESS;

	debug_dump_data(efuse->mask, efuse->mask_size, "[HAL EFUSE] mask data =");

	return status;
}

enum rtw_hal_status
efuse_file_mask2version(struct efuse_t *efuse, u8 *srcbuf, u16 buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	if (buflen <= efuse->version_len) {
		_os_mem_cpy(efuse->hal_com->drv_priv, (void *)efuse->mask_version,
					(void *)srcbuf, buflen);
		debug_dump_data(efuse->mask_version, efuse->version_len,
						"[HAL EFUSE] mask ver =");
		status = RTW_HAL_STATUS_SUCCESS;
	}

	return status;
}

static u16 efuse_file_open(void *d, char *filepath, u8 *buf, u32 buflen)
{
	char *ptmpbuf = NULL, *ptr;
	u32 bufsize = 5120;
	u8 val8 = 0;
	u32 file_size = 0, i = 0, j= 0, tmp = 0;
	int err = 0;

	ptmpbuf = _os_mem_alloc(d, bufsize);
	if (ptmpbuf == NULL) {
		PHL_INFO("%s, alloc buf FAIL!!\n", __FUNCTION__);
		return 0;
	}

	file_size = _os_read_file(filepath, (u8*)ptmpbuf, bufsize);
	if (file_size > 0) {
		if (file_size <= 90) {
			_os_mem_free(d, (void*)ptmpbuf, bufsize);
			PHL_INFO("%s, filepatch %s, size=%d not over 90, FAIL!!\n",
								__FUNCTION__, filepath, file_size);
			return 0;
		}
		i = 0;
		j = 0;
		ptr = ptmpbuf;
		while ((j < buflen) && (i < file_size)) {
			if (ptmpbuf[i] == '\0')
				break;
			ptr = _os_strpbrk((char *)&ptmpbuf[i], " \t\n\r");
			if (ptr) {
				if (ptr == &ptmpbuf[i]) {
					i++;
					continue;
				}
				/* Add string terminating null */
				*ptr = 0;
			} else {
				ptr = &ptmpbuf[file_size-1];
			}

			err = _os_sscanf((char *)&ptmpbuf[i], "%x", &tmp);

			if (err != 1) {
				PHL_INFO("Something wrong to parse efuse file, string=%s\n", &ptmpbuf[i]);
			} else {
				val8 = (u8)tmp;
				buf[j] = val8;
				PHL_INFO("i=%d, j=%d, 0x%02x\n", i, j, buf[j]);
				j++;
			}
			i = (u32)(ptr - ptmpbuf + 1);
		}
	}
	if (ptmpbuf)
		_os_mem_free(d, (void*)ptmpbuf, bufsize);
	PHL_INFO("%s, filepatch %s, size=%d, done\n", __FUNCTION__, filepath, file_size);
	return (u16)j;
}

enum rtw_hal_status
rtw_efuse_file_map_load(void *efuse, char *file_path, u8 is_limit)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	void *d = efuse_info->hal_com->drv_priv;
	u8 *mapbuf = NULL;
	u16 data_len = 0;
	u32 map_sz = 0, full_map_sz = 0;
	u32 offset;

	if (is_limit)
		map_sz = efuse_info->limit_efuse_size;
	else
		map_sz = efuse_info->log_efuse_size;

	if (map_sz == 0) {
		map_sz = MAX_EFUSE_MAP_LEN;
		full_map_sz = MAX_EFUSE_MAP_LEN + MAX_EFUSE_FILE_VERSION_LENGTH;
	} else
		full_map_sz = map_sz + efuse_info->version_len;

	if (file_path && full_map_sz != 0) {
		mapbuf= _os_mem_alloc(d, full_map_sz);
		if (mapbuf == NULL) {
			PHL_INFO("%s, alloc buf FAIL!!\n", __FUNCTION__);
			goto exit;
		}
		_os_mem_set(d, mapbuf, 0xFF, full_map_sz);

		data_len = efuse_file_open(d, file_path, mapbuf, full_map_sz);

		if ((data_len > map_sz) && (data_len <= full_map_sz)) {
			efuse_file_map2version(efuse_info, mapbuf + map_sz,
								   efuse_info->version_len);

			if (data_len > map_sz)
				data_len -= efuse_info->version_len;

			PHL_INFO("%s , File eFuse map to shadow len %d\n", __FUNCTION__, data_len);
			hal_status = efuse_map_buf2shadow(efuse_info, mapbuf, data_len);

			if (efuse_info->efuse_a_die_size != 0 && is_limit) {
				for (offset = efuse_info->a_die_start_offset;
					offset < (efuse_info->a_die_start_offset + (u32)efuse_info->efuse_a_die_size);
					offset++)
					efuse_info->shadow_map[offset + efuse_info->hci_to_a_die_offset] =
						mapbuf[offset];
			}

			efuse_info->map_from_status = FILE_MAP;
		} else {
			PHL_INFO("Error No Map Version !, File Map Data Len %d not over 1536.\n", data_len);
			goto exit;
		}
	}
exit:
	if (mapbuf)
		_os_mem_free(d, (void*)mapbuf, full_map_sz);

	return hal_status;
}

enum rtw_hal_status
rtw_efuse_file_mask_load(void *efuse, char *file_path, u8 is_limit)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	void *d = efuse_info->hal_com->drv_priv;
	u8 *maskbuf = NULL;
	u16 data_len = 0;
	u32 mask_sz = 0, full_mask_sz = 0;

	if (is_limit)
		mask_sz = efuse_info->limit_mask_size;
	else
		mask_sz = efuse_info->mask_size;

	if (mask_sz == 0) {
		mask_sz = EFUSE_MASK_FILE_LEN;
		full_mask_sz = EFUSE_MASK_FILE_LEN + MAX_EFUSE_FILE_VERSION_LENGTH;
	} else
		full_mask_sz = mask_sz + efuse_info->version_len;

	if (file_path) {
		maskbuf= _os_mem_alloc(d, full_mask_sz);
		if (maskbuf == NULL) {
			PHL_INFO("%s, alloc buf FAIL!!\n", __FUNCTION__);
			goto exit;
		}
		_os_mem_set(d, maskbuf, 0xFF, full_mask_sz);

		data_len = efuse_file_open(d, file_path, maskbuf, full_mask_sz);

		if ((data_len > mask_sz) && (data_len <= full_mask_sz)) {
			efuse_file_mask2version(efuse_info, maskbuf + mask_sz,
									efuse_info->version_len);

			if (data_len > mask_sz)
				data_len -= efuse_info->version_len;

			PHL_INFO("Mask File data 2 buf len %d\n", data_len);
			hal_status = efuse_file_mask2buf(efuse_info, maskbuf, data_len);

		} else {
			PHL_INFO("Error No Map Version !, File Map Data Len %d not over 102.\n", data_len);
			goto exit;
		}
	}
exit:
	if (maskbuf)
		_os_mem_free(d, (void*)maskbuf, full_mask_sz);

	return hal_status;
}

enum rtw_hal_status rtw_efuse_get_logical_size(void *efuse, u32 *size,
											   bool is_limited)
{
	struct efuse_t *efuse_info = efuse;

	if(is_limited == true)
		*size = efuse_info->limit_efuse_size;
	else
		*size = efuse_info->log_efuse_size;

	PHL_INFO("%s: size = %d\n", __FUNCTION__, *size);
	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_efuse_get_size(void *efuse, u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_get_efuse_size(efuse_info->hal_com, size);

	PHL_INFO("%s: size = %d\n", __FUNCTION__, *size);
	return status;
}

enum rtw_hal_status rtw_efuse_get_avl(void *efuse, u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_get_efuse_avl(efuse_info->hal_com, size);

	/* [TEMP] Set avl size for testing */
	/* size = 512; */
	PHL_INFO("%s: size = %d\n", __FUNCTION__, *size);
	return status;
}

enum rtw_hal_status rtw_efuse_get_offset_mask(void *efuse, u16 offset, u8 *mask)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct efuse_t *efuse_info = efuse;

	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == false)
		return RTW_HAL_STATUS_EFUSE_UNINIT;

	if(offset >= efuse_info->limit_mask_size)
		return RTW_HAL_STATUS_EFUSE_IVALID_OFFSET;

	*mask = efuse_info->mask[offset];
	PHL_INFO("%s: offset = %x mask = %x\n", __FUNCTION__, offset, *mask);
	return status;
}

enum rtw_hal_status rtw_efuse_get_mask_buf(void *efuse, u8 *destbuf, u32 *buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	if (efuse_info->mask_size > 0) {
		_os_mem_cpy(efuse_info->hal_com->drv_priv, (void *)destbuf,
				(void *)efuse_info->mask , efuse_info->mask_size);
		*buflen = efuse_info->mask_size;

		PHL_INFO("%s: size = %d\n", __FUNCTION__, efuse_info->mask_size);
		status = RTW_HAL_STATUS_SUCCESS;
	}

	return status;
}

enum rtw_hal_status rtw_efuse_get_shadowmap_from(void *efuse, u8 *val)
{
	struct efuse_t *efuse_info = efuse;

	*val = efuse_info->map_from_status;

	PHL_INFO("%s:get efuse %s\n", __FUNCTION__, SHADOWMAP_FROM2STR(*val));
	return RTW_HAL_STATUS_SUCCESS;
}

char* rtw_efuse_get_shadowmap_from_to_str(void *efuse)
{
	struct efuse_t *efuse_info = efuse;

	return SHADOWMAP_FROM2STR(efuse_info->map_from_status);
}


enum rtw_hal_status rtw_efuse_get_info(void *efuse,
									   enum rtw_efuse_info info_type,
									   void *value,
									   u8 size)
{
	struct efuse_t *efuse_info = efuse;
	struct rtw_hal_com_t *hal_com = efuse_info->hal_com;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == false)
		return RTW_HAL_STATUS_EFUSE_UNINIT;

	if(info_type <= EFUSE_INFO_MAC_MAX)
		hal_status = rtw_hal_mac_get_efuse_info(hal_com,
												efuse_info->shadow_map,
												info_type,
												value,
												size,
												efuse_info->is_map_valid);
	else if (info_type <= EFUSE_INFO_BB_MAX)
		hal_status = rtw_hal_bb_get_efuse_info(hal_com,
											   efuse_info->shadow_map,
											   info_type,
											   value,
											   size,
											   efuse_info->is_map_valid);
	else if (info_type <= EFUSE_INFO_RF_MAX)
		hal_status = rtw_hal_rf_get_efuse_info(hal_com,
											   efuse_info->shadow_map,
											   info_type,
											   value,
											   size,
											   efuse_info->is_map_valid);
	else
		hal_status = rtw_hal_btc_get_efuse_info(hal_com,
												efuse_info->shadow_map,
												info_type,
												value,
												size,
												efuse_info->is_map_valid);

	return hal_status;
}

void rtw_efuse_process(void *efuse, char *ic_name)
{
	struct efuse_t *efuse_info = (struct efuse_t *)efuse;

	if(efuse_info == NULL) {
		PHL_ERR("%s efuse_info is NULL\n",__FUNCTION__);
		return;
	}

	efuse_check_autoload(efuse_info);
	/* Load wifi full map to shadow map */
	rtw_efuse_shadow_load(efuse_info, false);

	SET_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS);

	rtw_efuse_shadow_file_load(efuse_info, ic_name, true);

	debug_dump_data(efuse_info->shadow_map, efuse_info->log_efuse_size,
					"Logical EFUSE MAP:");
	efuse_hidden_handle(efuse_info);

	/*
	 * We can set the hw cap after we got the shadow map.
	 * The efuse get info API will check the efuse is processed or not.
	 */
	efuse_set_hw_cap(efuse_info);

	efuse_compatible_chk(efuse_info);

	if (RTW_DRV_MODE_EQC == efuse_info->phl_com->drv_mode) {
		rtw_hal_rf_get_default_rfe_type(efuse_info->hal_com);
		rtw_hal_rf_get_default_xtal(efuse_info->hal_com);
		PHL_WARN("%s: Use default RFE type(0x%x) / XTAL(0x%x) configuration for EQC mode\n",
			 __FUNCTION__,
			 efuse_info->hal_com->dev_hw_cap.rfe_type,
			 efuse_info->hal_com->dev_hw_cap.xcap);
	}
}

bool rtw_efuse_is_processed(void *efuse)
{
	struct efuse_t *efuse_info = (struct efuse_t *)efuse;

	if (TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == true)
		return true;
	else
		return false;
}

u32 rtw_efuse_init(struct rtw_phl_com_t *phl_com,
				   struct rtw_hal_com_t *hal_com, void **efuse)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = NULL;

	efuse_info = _os_mem_alloc(hal_com->drv_priv, sizeof(struct efuse_t));

	if(efuse_info == NULL) {
		hal_status = RTW_HAL_STATUS_RESOURCE;
		goto error_efuse_init;
	}

	/* Allocate shadow map memory */
	hal_status = rtw_hal_mac_get_log_efuse_size(hal_com,
						    &(efuse_info->log_efuse_size),
						    false);

	if(hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s Get full logical efuse map size fail!\n",__FUNCTION__);
		goto error_efuse_shadow_init;
	}

	efuse_info->shadow_map = _os_mem_alloc(hal_com->drv_priv,
					       efuse_info->log_efuse_size);

	if(efuse_info->shadow_map == NULL) {
		hal_status = RTW_HAL_STATUS_RESOURCE;
		PHL_ERR("%s Allocate shadow efuse map fail!\n", __FUNCTION__);
		goto error_efuse_shadow_init;
	}

	hal_status = rtw_hal_mac_get_log_efuse_size(hal_com,
						    &(efuse_info->limit_efuse_size),
						    true);

	if(hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s Get limited logical efuse map size fail!\n", __FUNCTION__);
		goto error_efuse_shadow_init;
	}

	hal_status = rtw_hal_mac_get_efuse_a_die_size(hal_com,
						    &(efuse_info->efuse_a_die_size));

	if(hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s Get efuse a die size fail!\n", __FUNCTION__);
		goto error_efuse_shadow_init;
	}

	efuse_info->hci_to_a_die_offset = get_hci_to_a_die_offset(efuse_info->log_efuse_size,
										efuse_info->limit_efuse_size);

	efuse_info->a_die_start_offset = get_a_die_start_offset(efuse_info->limit_efuse_size,
										efuse_info->efuse_a_die_size);

	/* Allocate mask memory */
	hal_status = rtw_hal_mac_get_efuse_mask_size(hal_com,
						     &(efuse_info->mask_size),
						     false);

	if(hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s Get full efuse mask size fail!\n", __FUNCTION__);
		goto error_efuse_mask_init;
	}

	efuse_info->mask = _os_mem_alloc(hal_com->drv_priv,
					 efuse_info->mask_size);

	if(efuse_info->mask == NULL) {
		hal_status = RTW_HAL_STATUS_RESOURCE;
		PHL_ERR("%s Allocate efuse mask fail!\n", __FUNCTION__);
		goto error_efuse_mask_init;
	}

	hal_status = rtw_hal_mac_get_efuse_mask_size(hal_com,
						     &(efuse_info->limit_mask_size),
						     true);

	if(hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s Get limited efuse mask size fail!\n", __FUNCTION__);
		goto error_efuse_mask_init;
	}

	efuse_info->version_len = rtw_hal_mac_get_efuse_ver_len(hal_com);

	efuse_info->map_version = _os_mem_alloc(hal_com->drv_priv,
						efuse_info->version_len);

	if(efuse_info->map_version == NULL) {
		hal_status = RTW_HAL_STATUS_RESOURCE;
		goto error_map_version_init;
	}

	efuse_info->mask_version = _os_mem_alloc(hal_com->drv_priv,
						 efuse_info->version_len);
	if(efuse_info->mask_version == NULL) {
		hal_status = RTW_HAL_STATUS_RESOURCE;
		goto error_mask_version_init;
	}

	/* Allocate bt shadow map memory */
	hal_status = rtw_hal_mac_get_log_efuse_bt_size(hal_com,
						&(efuse_info->bt_log_efuse_size));

	if(hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s Get full logical efuse map size fail!\n",__FUNCTION__);
		goto error_efuse_bt_shadow_init;
	}

	efuse_info->bt_shadow_map = _os_mem_alloc(hal_com->drv_priv,
					       efuse_info->bt_log_efuse_size);

	if(efuse_info->bt_shadow_map == NULL) {
		hal_status = RTW_HAL_STATUS_RESOURCE;
		PHL_ERR("%s Allocate shadow efuse map fail!\n", __FUNCTION__);
		goto error_efuse_bt_shadow_init;
	}

	/* Allocate mask memory */
	hal_status = rtw_hal_mac_get_efuse_bt_mask_size(hal_com,
						     &(efuse_info->bt_mask_size));

	if(hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s Get full efuse mask size fail!\n", __FUNCTION__);
		goto error_efuse_bt_mask_init;
	}

	efuse_info->bt_mask = _os_mem_alloc(hal_com->drv_priv,
					 efuse_info->bt_mask_size);

	if(efuse_info->bt_mask == NULL) {
		hal_status = RTW_HAL_STATUS_RESOURCE;
		PHL_ERR("%s Allocate efuse mask fail!\n", __FUNCTION__);
		goto error_efuse_bt_mask_init;
	}


	efuse_info->phl_com = phl_com;
	efuse_info->hal_com = hal_com;

	*efuse = efuse_info;

	hal_status = RTW_HAL_STATUS_SUCCESS;
	return hal_status;

error_efuse_bt_mask_init:
	_os_mem_free(hal_com->drv_priv, efuse_info->bt_shadow_map,
		     efuse_info->bt_log_efuse_size);

error_efuse_bt_shadow_init:
	_os_mem_free(hal_com->drv_priv, efuse_info->mask_version,
		efuse_info->version_len);

error_mask_version_init:
	_os_mem_free(hal_com->drv_priv, efuse_info->map_version,
		     efuse_info->version_len);

error_map_version_init:
	_os_mem_free(hal_com->drv_priv, efuse_info->mask,
		     efuse_info->mask_size);

error_efuse_mask_init:
	_os_mem_free(hal_com->drv_priv, efuse_info->shadow_map,
		     efuse_info->log_efuse_size);

error_efuse_shadow_init:
	_os_mem_free(hal_com->drv_priv, efuse_info, sizeof(struct efuse_t));

error_efuse_init:
	return hal_status;
}

void rtw_efuse_deinit(struct rtw_hal_com_t *hal_com, void *efuse)
{
	struct efuse_t *efuse_info = efuse;

	if(efuse_info->bt_mask) {
		_os_mem_free(hal_com->drv_priv, efuse_info->bt_mask,
					 efuse_info->bt_mask_size);
		efuse_info->bt_mask = NULL;
	}

	if(efuse_info->bt_shadow_map) {
		_os_mem_free(hal_com->drv_priv, efuse_info->bt_shadow_map,
					 efuse_info->bt_log_efuse_size);
		efuse_info->bt_shadow_map = NULL;
	}

	if(efuse_info->mask) {
		_os_mem_free(hal_com->drv_priv, efuse_info->mask,
					 efuse_info->mask_size);
		efuse_info->mask = NULL;
	}
	if(efuse_info->map_version) {
		_os_mem_free(hal_com->drv_priv, efuse_info->map_version,
					 efuse_info->version_len);
		efuse_info->map_version = NULL;
	}
	if(efuse_info->mask_version) {
		 _os_mem_free(hal_com->drv_priv, efuse_info->mask_version,
		 				efuse_info->version_len);
		 efuse_info->mask_version = NULL;
	}
	if(efuse_info->shadow_map) {
		_os_mem_free(hal_com->drv_priv, efuse_info->shadow_map,
					 efuse_info->log_efuse_size);
		efuse_info->shadow_map = NULL;
	}

	if (efuse_info) {
		_os_mem_free(hal_com->drv_priv, efuse_info, sizeof(struct efuse_t));
		efuse_info = NULL;
	}
}

/* BT EFUSE API */
void efuse_bt_shadow_read_one_byte(struct efuse_t *efuse, u16 offset, u8 *value)
{
   *value = efuse->bt_shadow_map[offset];
}

void efuse_bt_shadow_read_two_byte(struct efuse_t *efuse, u16 offset, u16 *value)
{
   *value = efuse->bt_shadow_map[offset];
   *value |= efuse->bt_shadow_map[offset+1] << 8;
}

void efuse_bt_shadow_read_four_byte(struct efuse_t *efuse, u16 offset, u32 *value)
{
   *value = efuse->bt_shadow_map[offset];
   *value |= efuse->bt_shadow_map[offset+1] << 8;
   *value |= efuse->bt_shadow_map[offset+2] << 16;
   *value |= efuse->bt_shadow_map[offset+3] << 24;
}

void efuse_bt_shadow_write_one_byte(struct efuse_t *efuse, u16 offset, u16 value)
{
   efuse->bt_shadow_map[offset] = (u8)(value&0x00FF);
}

void efuse_bt_shadow_write_two_byte(struct efuse_t *efuse, u16 offset, u16 value)
{
   efuse->bt_shadow_map[offset] = (u8)(value&0x00FF);
   efuse->bt_shadow_map[offset+1] = (u8)((value&0xFF00) >> 8);
}

void efuse_bt_shadow_write_four_byte(struct efuse_t *efuse, u16 offset, u32 value)
{
   efuse->bt_shadow_map[offset] = (u8)(value&0x000000FF);
   efuse->bt_shadow_map[offset+1] = (u8)((value&0x0000FF00) >> 8);
   efuse->bt_shadow_map[offset+2] = (u8)((value&0x00FF0000) >> 16);
   efuse->bt_shadow_map[offset+3] = (u8)((value&0xFF000000) >> 24);
}

enum rtw_hal_status rtw_efuse_bt_shadow_load(void *efuse)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_read_log_efuse_bt_map(efuse_info->hal_com,
											efuse_info->bt_shadow_map);

	return status;
}

enum rtw_hal_status rtw_efuse_bt_shadow_update(void *efuse)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_EFUSE_PG_FAIL;
	enum rtw_hal_status reload_status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	u32 map_size = 0, mask_size = 0;

	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == false) {
		PHL_WARN("%s: efuse map not load yet!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_UNINIT;
		goto exit;
	}

	/* Load efuse mask file before PG */
	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_BT_MASK_FILE_LOADED) == false) {
		PHL_WARN("%s: efuse mask not load yet!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_PG_FAIL;
		goto exit;
	}

	map_size = efuse_info->bt_log_efuse_size;
	mask_size = efuse_info->bt_mask_size;

	status = rtw_hal_mac_write_log_efuse_bt_map(efuse_info->hal_com,
							efuse_info->bt_shadow_map,
							map_size,
							efuse_info->bt_mask,
							mask_size);

	if(status != RTW_HAL_STATUS_SUCCESS)
		PHL_WARN("%s: BT PG Fail!\n", __FUNCTION__);

	/* Reload shadow map after PG */
	reload_status = rtw_hal_mac_read_log_efuse_bt_map(efuse_info->hal_com,
								efuse_info->bt_shadow_map);
	if(reload_status != RTW_HAL_STATUS_SUCCESS)
		PHL_WARN("%s: Reload bt shadow map Fail!\n", __FUNCTION__);

	/*
	 * Clear the load external map file flag and map version
	 * after reloading the map.
	 */
	if(TEST_STATUS_FLAG(efuse_info->status,
				EFUSE_STATUS_BT_MAP_FILE_LOADED) == true) {
		CLEAR_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_BT_MAP_FILE_LOADED);
	}

exit:
	return status;
}

enum rtw_hal_status
rtw_efuse_bt_shadow_read(void *efuse, u8 byte_count, u16 offset, u32 *value)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	u32	efuse_bt_size = 0;

	efuse_bt_size = efuse_info->bt_log_efuse_size;

	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == false) {
		PHL_WARN("%s: efuse map not load yet!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_UNINIT;
		goto exit;
	}

	if((u32)(offset+byte_count) > efuse_bt_size) {
		PHL_WARN("%s: Invalid offset!\n", __FUNCTION__);
		status = RTW_HAL_STATUS_EFUSE_IVALID_OFFSET;
		goto exit;
	}

	if (byte_count == 1)
		efuse_bt_shadow_read_one_byte(efuse_info, offset, (u8 *)value);
	else if (byte_count == 2)
		efuse_bt_shadow_read_two_byte(efuse_info, offset, (u16 *)value);
	else if (byte_count == 4)
		efuse_bt_shadow_read_four_byte(efuse_info, offset, (u32 *)value);

	status = RTW_HAL_STATUS_SUCCESS;
exit:
	return status;
}

enum rtw_hal_status
rtw_efuse_bt_shadow_write(void *efuse, u8 byte_count, u16 offset, u32 value)
{
  enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
  struct efuse_t *efuse_info = efuse;
  u32 efuse_bt_size = 0;

  efuse_bt_size = efuse_info->bt_log_efuse_size;

  if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == false) {
	  PHL_WARN("%s: efuse map not load yet!\n", __FUNCTION__);
	  status = RTW_HAL_STATUS_EFUSE_UNINIT;
	  goto exit;
  }

  if((u32)(offset+byte_count) > efuse_bt_size) {
	  PHL_WARN("%s: Invalid offset!\n", __FUNCTION__);
	  status = RTW_HAL_STATUS_EFUSE_IVALID_OFFSET;
	  goto exit;
  }

  if (byte_count == 1)
	  efuse_bt_shadow_write_one_byte(efuse_info, offset, (u8)value);
  else if (byte_count == 2)
	  efuse_bt_shadow_write_two_byte(efuse_info, offset, (u16)value);
  else if (byte_count == 4)
	  efuse_bt_shadow_write_four_byte(efuse_info, offset, (u32)value);

  status = RTW_HAL_STATUS_SUCCESS;
exit:
  return status;
}

/*
 * This API is used for original mp dll command.
 * usage = used percentage(1 Byte) + used bytes(2 Bytes)
 */
enum rtw_hal_status rtw_efuse_bt_get_usage(void *efuse, u32 *usage)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	u32 avl_size = 0;
	u16 used_byte = 0;
	u32 total_size = 0;
	u8 used_percentage = 0;

	status = rtw_hal_mac_get_efuse_bt_avl(efuse_info->hal_com, &avl_size);
	status = rtw_hal_mac_get_efuse_bt_size(efuse_info->hal_com, &total_size);

	if(total_size != 0){
		used_byte = (u16)(total_size - avl_size);
		used_percentage = (u8)(used_byte*100/total_size);
		*usage = (used_percentage<<16)|(used_byte);
	}

	return status;
}

enum rtw_hal_status rtw_efuse_bt_shadow2buf(void *efuse, u8 *destbuf, u16 buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct efuse_t *efuse_info = efuse;

	_os_mem_cpy(efuse_info->hal_com->drv_priv, (void *)destbuf,
				(void *)efuse_info->bt_shadow_map , buflen);

	return status;
}

enum rtw_hal_status
efuse_bt_map_buf2shadow(struct efuse_t *efuse, u8 *srcbuf, u16 buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	_os_mem_cpy(efuse->hal_com->drv_priv, (void *)efuse->bt_shadow_map,
				(void *)srcbuf, buflen);
	SET_STATUS_FLAG(efuse->status, EFUSE_STATUS_BT_MAP_FILE_LOADED);
	status = RTW_HAL_STATUS_SUCCESS;

	return status;
}

enum rtw_hal_status
efuse_bt_file_mask2buf(struct efuse_t *efuse, u8 *srcbuf, u16 buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	_os_mem_cpy(efuse->hal_com->drv_priv, (void *)efuse->bt_mask, (void *)srcbuf,
				buflen);
	SET_STATUS_FLAG(efuse->status, EFUSE_STATUS_BT_MASK_FILE_LOADED);
	status = RTW_HAL_STATUS_SUCCESS;

	debug_dump_data(efuse->bt_mask, efuse->bt_mask_size, "[HAL EFUSE] bt mask data =");

	return status;
}

enum rtw_hal_status
rtw_efuse_bt_file_map_load(void *efuse, char *file_path)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	void *d = efuse_info->hal_com->drv_priv;
	u8 *mapbuf = NULL;
	u16 data_len = 0;
	u32 bt_map_sz = 0;

	bt_map_sz = efuse_info->bt_log_efuse_size;

	if (bt_map_sz == 0)
		bt_map_sz = MAX_EFUSE_MAP_LEN;

	if (file_path) {
		mapbuf= _os_mem_alloc(d, bt_map_sz);
		if (mapbuf == NULL) {
			PHL_INFO("%s, alloc buf FAIL!!\n", __FUNCTION__);
			goto exit;
		}
		_os_mem_set(d, mapbuf, 0xFF, bt_map_sz);

		data_len = efuse_file_open(d, file_path, mapbuf, bt_map_sz);

		if (data_len <= bt_map_sz && data_len > 0) {
			PHL_INFO("%s , File eFuse bt map to shadow len %d\n", __FUNCTION__, data_len);
			hal_status = efuse_bt_map_buf2shadow(efuse_info, mapbuf, data_len);

		} else {
			PHL_INFO("Error No bt Map Version !, File Map Data Len %d not over 1024.\n", data_len);
			goto exit;
		}
	}
exit:
	if (mapbuf)
		_os_mem_free(d, (void*)mapbuf, bt_map_sz);

	return hal_status;
}


enum rtw_hal_status
rtw_efuse_bt_file_mask_load(void *efuse, char *file_path)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;
	void *d = efuse_info->hal_com->drv_priv;
	u8 *maskbuf = NULL;
	u16 data_len = 0;
	u32 bt_mask_sz = 0;

	bt_mask_sz = efuse_info->bt_mask_size;

	if (bt_mask_sz == 0)
		goto exit;

	if (file_path) {
		maskbuf= _os_mem_alloc(d, bt_mask_sz);
		if (maskbuf == NULL) {
			PHL_INFO("%s, alloc buf FAIL!!\n", __FUNCTION__);
			goto exit;
		}
		_os_mem_set(d, maskbuf, 0xFF, bt_mask_sz);

		data_len = efuse_file_open(d, file_path, maskbuf, bt_mask_sz);

		if (data_len <= bt_mask_sz && data_len > 0) {
			PHL_INFO("Mask File data 2 buf len %d\n", data_len);
			hal_status = efuse_bt_file_mask2buf(efuse_info, maskbuf, data_len);

		} else {
			PHL_INFO("Error No Map Version !, File Map Data Len %d not over 96.\n", data_len);
			goto exit;
		}
	}
exit:
	if (maskbuf)
		_os_mem_free(d, (void*)maskbuf, bt_mask_sz);

	return hal_status;
}

enum rtw_hal_status rtw_efuse_bt_get_logical_size(void *efuse, u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_get_log_efuse_bt_size(efuse_info->hal_com, size);

	PHL_INFO("%s: size = %d\n", __FUNCTION__, *size);
	return status;
}

enum rtw_hal_status rtw_efuse_bt_get_size(void *efuse, u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_get_efuse_bt_size(efuse_info->hal_com, size);

	PHL_INFO("%s: size = %d\n", __FUNCTION__, *size);
	return status;
}

enum rtw_hal_status rtw_efuse_bt_get_avl(void *efuse, u32 *size)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_get_efuse_bt_avl(efuse_info->hal_com, size);

	PHL_INFO("%s: size = %d\n", __FUNCTION__, *size);
	return status;
}

enum rtw_hal_status rtw_efuse_bt_get_offset_mask(void *efuse, u16 offset, u8 *mask)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	if(TEST_STATUS_FLAG(efuse_info->status, EFUSE_STATUS_PROCESS) == false)
		return RTW_HAL_STATUS_EFUSE_UNINIT;

	if(offset >= efuse_info->bt_mask_size)
		return RTW_HAL_STATUS_EFUSE_IVALID_OFFSET;

	*mask = efuse_info->bt_mask[offset];
	PHL_INFO("%s: bt offset = %x mask = %x\n", __FUNCTION__, offset, *mask);
	return status;
}

enum rtw_hal_status rtw_efuse_bt_get_mask_buf(void *efuse, u8 *destbuf, u32 *buflen)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	if (efuse_info->bt_mask_size > 0) {
		_os_mem_cpy(efuse_info->hal_com->drv_priv, (void *)destbuf,
				(void *)efuse_info->bt_mask , efuse_info->bt_mask_size);
		*buflen = efuse_info->bt_mask_size;

		PHL_INFO("%s: size = %d\n", __FUNCTION__, efuse_info->bt_mask_size);
		status = RTW_HAL_STATUS_SUCCESS;
	}
	return status;
}

enum rtw_hal_status rtw_efuse_bt_read_hidden(void *efuse, u32 addr, u32 size, u8 *val)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_read_efuse_bt_hidden(efuse_info->hal_com, addr, size, val);

	if(status == RTW_HAL_STATUS_SUCCESS)
	{
		PHL_INFO("%s: bt hidden read ok!\n", __FUNCTION__);
	}else{
		PHL_INFO("%s: bt hidden read fail!\n", __FUNCTION__);
	}

	return status;
}

enum rtw_hal_status rtw_efuse_bt_write_hidden(void *efuse, u32 addr, u8 val)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	status = rtw_hal_mac_write_efuse_bt_hidden(efuse_info->hal_com, addr, val);

	if(status == RTW_HAL_STATUS_SUCCESS)
	{
		PHL_INFO("%s: bt hidden PG ok!\n", __FUNCTION__);
	}else{
		PHL_INFO("%s: bt hidden PG fail!\n", __FUNCTION__);
	}

	return status;
}

enum rtw_hal_status rtw_efuse_read_phy_wifi(void *efuse, u32 addr, u32 size, u8 *data, u8 type)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct efuse_t *efuse_info = efuse;

	if(type == HAL_MP_EFUSE_WIFI)
		status = rtw_hal_mac_read_phy_efuse(efuse_info->hal_com, addr, size, data);
	else if(type == HAL_MP_EFUSE_BT)
		status = rtw_hal_mac_read_bt_phy_efuse(efuse_info->hal_com, addr, size, data);

	if(status == RTW_HAL_STATUS_SUCCESS)
	{
		PHL_INFO("%s: real raw read ok!\n", __FUNCTION__);
	}else{
		PHL_INFO("%s: real raw read fail!\n", __FUNCTION__);
	}

	return status;
}

enum rtw_hal_status rtw_efuse_renew(void *efuse, u8 type)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct efuse_t *efuse_info = efuse;

	if (type == HAL_MP_EFUSE_WIFI) {
		rtw_hal_bb_get_efuse_init(efuse_info->hal_com);
		rtw_hal_rf_get_efuse_ex(efuse_info->hal_com, HW_PHY_MAX);
		PHL_INFO("%s: hal efuse renew done\n", __FUNCTION__);

	} else if (type == HAL_MP_EFUSE_BT) {
		PHL_INFO("%s: Not ready\n", __FUNCTION__);
	}

	return status;
}
