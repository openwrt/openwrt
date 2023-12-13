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
#define _HAL_CAM_C_
#include "hal_headers.h"

/**
 * This function calls halmac api to configure security cam
 * @hal: see struct hal_info_t
 * @sta: sta info of specified address cam entry
 * @type: encryption type of this key
 * @ext_key: whether 256bit key or not
 * @spp: spp mode
 * @keyid: key index
 * @keytype: specify unicast or multicast or bip key type
 * @keybuf: key buffer length according to ext_key
 * return enum RTW_HAL_STATUS
 */
enum rtw_hal_status
rtw_hal_set_key(void *hal, struct rtw_phl_stainfo_t *sta, u8 type, u8 ext_key, u8 spp,
				u8 keyid, u8 keytype, u8 *keybuf)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	u8 macid = (u8)sta->macid;

	if (keybuf == NULL) {
		/* delete key */
		hal_status = rtw_hal_mac_delete_key(hal_info, macid, type, ext_key,
					spp, keyid, keytype);
	} else {
		/* add key */
		if (ext_key) {
			hal_status = rtw_hal_mac_add_key(hal_info, macid, type, ext_key,
					spp, keyid, keytype, keybuf);
			hal_status = rtw_hal_mac_add_key(hal_info, macid, type, ext_key, spp,
					keyid, keytype, (keybuf+16));
		} else {
			hal_status = rtw_hal_mac_add_key(hal_info, macid, type, 0,
					spp, keyid, keytype, keybuf);
		}
	}

	return hal_status;
}

u32
rtw_hal_search_key_idx(void *hal, struct rtw_phl_stainfo_t *sta,
						u8 keyid, u8 keytype)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u32 sec_cam_idx = 0;
	u8 macid = (u8)sta->macid;

	sec_cam_idx = rtw_hal_mac_search_key_idx(hal_info,
						 macid,
						 keyid,
						 keytype);

	return sec_cam_idx;
}