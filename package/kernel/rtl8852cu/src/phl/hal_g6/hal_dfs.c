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
#define _HAL_DFS_C_
#include "hal_headers.h"

#ifdef CONFIG_PHL_DFS

enum rtw_hal_status
rtw_hal_radar_detect_cfg(void *hal, bool dfs_enable)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	PHL_INFO("====>%s dfs_en:%d ============\n", __func__, dfs_enable);

	rtw_hal_bb_dfs_rpt_cfg(hal_info, dfs_enable);

	return RTW_HAL_STATUS_SUCCESS;
}

#endif

