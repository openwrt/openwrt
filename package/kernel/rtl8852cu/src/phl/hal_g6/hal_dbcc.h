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
#ifndef _HAL_DBCC_H_
#define _HAL_DBCC_H_

enum phl_phy_idx
hal_dbcc_cck_phyidx_decision(struct hal_info_t *hal_info,
					     struct rtw_phl_com_t *phl_com,
					     bool dbcc_en);

void
hal_dbcc_cfg_phy_map(struct hal_info_t *hal, enum phl_band_idx band_idx);

#endif /* _HAL_DBCC_H_ */

