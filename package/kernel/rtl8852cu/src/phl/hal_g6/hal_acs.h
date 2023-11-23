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
#ifndef _HAL_ACS_H_
#define _HAL_ACS_H_

void rtw_hal_acs_mntr_trigger(void *hal, enum phl_band_idx band_idx, struct acs_mntr_parm *parm);
enum rtw_hal_status rtw_hal_acs_mntr_result(void *hal,
					enum phl_band_idx band_idx, struct acs_mntr_rpt *rpt);

#endif