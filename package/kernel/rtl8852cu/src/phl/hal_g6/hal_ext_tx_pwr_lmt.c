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
#define _HAL_EXT_TX_PWR_LMT_C_
#include "hal_headers.h"

enum rtw_hal_status
rtw_hal_set_power_limit(void *hal, u8 band_idx)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	enum phl_phy_idx phy_idx = HW_PHY_0;

	if (band_idx == 1) {
		if (hal_info->hal_com->dbcc_en) {
			phy_idx = HW_PHY_1;
		} else {
			PHL_WARN("%s: band_idx(%d) dbcc_en(%d)\n", __func__,
				 band_idx, hal_info->hal_com->dbcc_en);
			return status;
		}
	}
	status = rtw_hal_rf_set_power(hal_info, phy_idx, PWR_LIMIT);
	return status;
}

s8 rtw_hal_get_power_limit(void *hal, enum phl_phy_idx phy,
	u16 rate, u8 bandwidth, u8 beamforming, u8 tx_num, u8 channel)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_rf_get_power_limit(hal_info, phy, rate, bandwidth,
					  beamforming, tx_num, channel);
}

bool
rtw_hal_get_ext_pwr_lmt_en(void *hal,
		u8 hw_band)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_tpu_info *tpu = &(hal_com->band[hw_band].rtw_tpu_i);

	PHL_INFO("%s: hw_band(%d), en(%d)\n", __func__, hw_band, tpu->ext_pwr_lmt_en);

	return tpu->ext_pwr_lmt_en;
}

void
rtw_hal_set_ext_pwr_lmt_en(void *hal,
		u8 hw_band, bool enable)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_tpu_info *tpu = &(hal_com->band[hw_band].rtw_tpu_i);

	PHL_INFO("%s: hw_band(%d), en(%d)\n", __func__, hw_band, enable);

	tpu->ext_pwr_lmt_en = enable;
}

void
rtw_hal_enable_ext_pwr_lmt(void *hal, u8 hw_band,
		struct rtw_phl_ext_pwr_lmt_info *ext_pwr_lmt_info)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_tpu_info *tpu = &(hal_com->band[hw_band].rtw_tpu_i);
	enum phl_phy_idx phy_idx = HW_PHY_0;

	PHL_INFO("%s\n", __func__);

	if (hw_band == 1)
		phy_idx = HW_PHY_1;


	/* Fill external tx power limit into hal_com->band[hw_band].rtw_tpu_i.ext_pwr_lmt_i */
	_os_mem_cpy(hal_com->drv_priv, &(tpu->ext_pwr_lmt_i),
		    ext_pwr_lmt_info, sizeof(struct rtw_phl_ext_pwr_lmt_info));

	/**
	 * Update extension(or external) tx power limit to corresponding field of halrf power limit table (as below)
	 *   2g=> rf->pwr_info.tx_pwr_limit_2g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num]
	 *   5g=> rf->pwr_info.tx_pwr_limit_5g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num]
	 *   6g=> rf->pwr_info.tx_pwr_limit_6g[PW_LMT_REGU_EXT_PWR][bw][rate][bf][ch][tx_num]
	 */
	rtw_hal_rf_update_ext_pwr_lmt_table(hal_info, phy_idx);

	/* enable external tx power limit mechanism */
	tpu->ext_pwr_lmt_en = true;
}

