/******************************************************************************
 *
 * Copyright(c)2019 Realtek Corporation.
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
#ifndef _HAL_FW_H_
#define _HAL_FW_H_

enum rtw_hal_status
rtw_hal_download_fw(struct rtw_phl_com_t *phl_com, void *hal);

enum rtw_hal_status
rtw_hal_redownload_fw(struct rtw_phl_com_t *phl_com, void *hal);

void rtw_hal_fw_dbg_dump(void *hal);

void hal_fw_en_basic_log(struct rtw_hal_com_t *hal_com);

#endif /* _HAL_FW_H_ */

