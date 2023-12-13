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
#ifndef _HAL_MCC_H_
#define _HAL_MCC_H_
/* MCC definition for private usage */
#ifdef CONFIG_MCC_SUPPORT
enum rtw_hal_status rtw_hal_mcc_update_macid_bitmap(void *hal, u8 group,
			u16 macid, struct rtw_phl_mcc_macid_bitmap *info);

enum rtw_hal_status rtw_hal_mcc_sync_enable(void *hal,
					struct rtw_phl_mcc_en_info *info);
#endif
#endif /*_HAL_MCC_H_*/
