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
#define _HAL_CHAN_C_
#include "hal_headers.h"

enum rtw_hal_status rtw_hal_set_ch_bw(void *hal, u8 band_idx,
		struct rtw_chan_def *chdef, bool do_rfk, bool rd_enabled, bool frc_switch)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_chan_def *cur_chdef = &(hal_com->band[band_idx].cur_chandef);
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	u8 center_ch = 0;
	u8 central_ch_seg1 = 0;
	enum band_type change_band;
	enum phl_phy_idx phy_idx = HW_PHY_0;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	enum channel_width tmp_bw = chdef->bw;
	#ifdef CONFIG_PHL_NARROW_BW
	struct rtw_phl_com_t *phl_com = hal_info->phl_com;
	struct dev_cap_t *dev_cap = &phl_com->dev_cap;

	if (dev_cap->nb_config == CHANNEL_WIDTH_10)
		tmp_bw = CHANNEL_WIDTH_10;
	else if (dev_cap->nb_config == CHANNEL_WIDTH_5)
		tmp_bw = CHANNEL_WIDTH_5;
	#endif /*CONFIG_PHL_NARROW_BW*/


	phy_idx = rtw_hal_hw_band_to_phy_idx(band_idx);

	if ((frc_switch) ||
	    (chdef->band != cur_chdef->band) ||
	    (chdef->chan != cur_chdef->chan) ||
	    (tmp_bw != cur_chdef->bw) ||
	    (chdef->offset != cur_chdef->offset)) {

		status = rtw_hal_reset(hal_com, phy_idx, band_idx, true);
		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_reset en - failed\n", __func__);
			_os_warn_on(1);
		}
		/* if central channel changed, reset BB & MAC */
		center_ch = rtw_phl_get_center_ch(chdef);
		PHL_INFO("Using band:%d central channel %u for primary channel %u BW %u\n",
		         chdef->band, center_ch, chdef->chan, chdef->bw);

		change_band = chdef->band;

		status = rtw_hal_mac_set_bw(hal_info, band_idx, chdef->chan,
					      center_ch, central_ch_seg1, change_band,
					      tmp_bw);
		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_mac_set_bw - failed\n", __func__);
			return status;
		}


		if(tmp_bw == CHANNEL_WIDTH_80_80 && central_ch_seg1 == 0) {
			PHL_ERR("%s mising info for 80+80M configuration\n", __func__);
			return RTW_HAL_STATUS_FAILURE;
		}
		status = rtw_hal_bb_set_ch_bw(hal_info, phy_idx, chdef->chan,
					      center_ch, central_ch_seg1, change_band,
					      tmp_bw);
		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_bb_set_ch_bw - failed\n", __func__);
			return status;
		}

		status = rtw_hal_rf_set_ch_bw(hal_com,
		                              phy_idx,
		                              center_ch,
		                              change_band,
		                              tmp_bw);

		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_rf_set_ch_bw - failed\n", __func__);
			return status;
		}

		cur_chdef->chan = chdef->chan;
		cur_chdef->bw = chdef->bw;
		cur_chdef->offset = chdef->offset;
		cur_chdef->center_ch = center_ch;

		if ((frc_switch) || (cur_chdef->band != change_band)) {
			cur_chdef->band = change_band;
			rtw_hal_notify_switch_band(hal, change_band, phy_idx);
		}

		status = rtw_hal_rf_set_power(hal_info, phy_idx, PWR_LIMIT);

		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_rf_set_power - failed\n", __func__);
			return status;
		}

		status = rtw_hal_rf_set_power(hal_info, phy_idx, PWR_LIMIT_RU);

		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_rf_set_power - failed\n", __func__);
			return status;
		}

		PHL_INFO("%s phy_idx:%d, band:%d, ch:%d, bw:%d, offset:%d\n",
			__func__, phy_idx, chdef->band, chdef->chan, tmp_bw, chdef->offset);

		status = rtw_hal_reset(hal_com, phy_idx, band_idx, false);
		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_reset dis- failed\n", __func__);
			_os_warn_on(1);
		}

		/*PHL_DUMP_CHAN_DEF_EX(chandef);*/
	}
	if (do_rfk) {
		#ifdef CONFIG_PHL_DFS
		if (rd_enabled)
			rtw_hal_bb_dfs_rpt_cfg(hal_info, false);
		#endif

		status = rtw_hal_rf_chl_rfk_trigger(hal_com, phy_idx, true);
		if (status != RTW_HAL_STATUS_SUCCESS)
			PHL_ERR("rtw_hal_rf_chl_rfk_trigger fail!\n");

		#ifdef CONFIG_PHL_DFS
		if (rd_enabled)
			rtw_hal_bb_dfs_rpt_cfg(hal_info, true);
		#endif
	}
	PHL_INFO("%s: Switch chdef done, do_rfk:%s, frc_switch(%d)\n", __func__,
		(do_rfk) ?"Y" : "N", frc_switch);
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return status;
}

u8 rtw_hal_get_cur_ch(void *hal, u8 band_idx)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_chan_def *chandef = &(hal_com->band[band_idx].cur_chandef);

	return chandef->chan;
}

void rtw_hal_get_cur_chdef(void *hal, u8 band_idx,
				struct rtw_chan_def *cur_chandef)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_chan_def *chandef = &(hal_com->band[band_idx].cur_chandef);
	void *drv = hal_com->drv_priv;

	_os_mem_cpy(drv, cur_chandef, chandef, sizeof(struct rtw_chan_def));
}

void rtw_hal_sync_cur_ch(void *hal, u8 band_idx, struct rtw_chan_def chandef)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_chan_def *cur_chandef = &(hal_com->band[band_idx].cur_chandef);

	PHL_INFO("%s: Sync Ori-cur ch to src ch\n", __FUNCTION__);
	PHL_INFO("%s: Src ch: Bnad(%d), ch(%d), bw(%d), ofst(%d), cntr_ch(%d)\n",
		 __FUNCTION__, chandef.band, chandef.chan, chandef.bw, chandef.offset,
		 chandef.center_ch);
	PHL_INFO("%s: Ori-cur ch: Bnad(%d), ch(%d), bw(%d), ofst(%d), cntr_ch(%d)\n",
		 __FUNCTION__, cur_chandef->band, cur_chandef->chan,
		 cur_chandef->bw, cur_chandef->offset, cur_chandef->center_ch);
	_os_mem_cpy(hal_to_drvpriv(hal_info), cur_chandef, &chandef,
		    sizeof(struct rtw_chan_def));
}
