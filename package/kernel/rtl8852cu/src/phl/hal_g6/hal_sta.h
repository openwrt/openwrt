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
#ifndef _HAL_STA_H_
#define _HAL_STA_H_

struct rtw_hw_cfg_tab {
	u8 dummy;
};

enum rtw_hal_status
rtw_hal_restore_sta_entry(struct rtw_phl_com_t* phl_com, void *hal,
                          struct rtw_phl_stainfo_t *sta, bool is_connect);

enum rtw_hal_status
rtw_hal_init_tmp_entry(void *hal, u8 tmp_entry_num, u8 std_entry_num);

#endif  /*_HAL_STA_H_*/
