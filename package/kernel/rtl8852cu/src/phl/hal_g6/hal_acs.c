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
#define _HAL_ACS_C_
#include "hal_headers.h"

#ifdef CONFIG_RTW_ACS
void rtw_hal_acs_mntr_trigger(void *hal, enum phl_band_idx band_idx, struct acs_mntr_parm *parm)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	rtw_hal_bb_acs_mntr_trigger(hal_info, band_idx, parm);
}

enum rtw_hal_status rtw_hal_acs_mntr_result(void *hal,
					enum phl_band_idx band_idx, struct acs_mntr_rpt *rpt)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_bb_acs_mntr_result(hal_info, band_idx, rpt);
}
#endif /* CONFIG_RTW_ACS */