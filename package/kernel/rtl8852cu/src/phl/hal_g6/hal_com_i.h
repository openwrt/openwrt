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
#ifndef _HAL_COM_I_H_
#define _HAL_COM_I_H_

enum rtw_hal_status rtw_hal_ppdu_sts_cfg(void *hal, u8 band_idx, bool en);
enum rtw_hal_status rtw_hal_ppdu_sts_init(void *hal, u8 band_idx,
			bool en, u8 appen_info, u8 filter);

enum phl_band_idx rtw_hal_phy_idx_to_hw_band(enum phl_phy_idx p_idx);

enum phl_phy_idx rtw_hal_hw_band_to_phy_idx(enum phl_band_idx band_idx);

void rtw_hal_init_hw_band_info(void *hal, enum phl_band_idx band_idx);
#endif /* _HAL_COM_I_H_ */
