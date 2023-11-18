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
#define _HAL_DBCC_C_
#include "hal_headers.h"

#ifdef CONFIG_DBCC_SUPPORT
enum phl_phy_idx
hal_dbcc_cck_phyidx_decision(struct hal_info_t *hal_info,
					     struct rtw_phl_com_t *phl_com,
					     bool dbcc_en)
{
	enum phl_phy_idx phy_idx = HW_PHY_0;
	struct mr_ctl_t *mr_ctl = &phl_com->mr_ctrl;
	struct hw_band_ctl_t *band0_ctrl = &(mr_ctl->band_ctrl[HW_BAND_0]);
	/*struct hw_band_ctl_t *band1_ctrl = &(mr_ctl->band_ctrl[HW_BAND_1]);*/

	if (dbcc_en == false)
		goto _exit;

	#ifdef CONFIG_DBCC_FORCE
	if (is_frc_dbcc_mode(phl_com))
		phy_idx = phl_com->dev_cap.dbcc_force_cck_phyidx;
	else
	#endif
	if (phl_is_mp_mode(phl_com)) {
        	phy_idx = (hal_info->hal_com->band[HW_BAND_0].cur_chandef.band ==
		           BAND_ON_24G)
		          ? HW_PHY_0 : HW_PHY_1;
	} else {
		phy_idx = (band0_ctrl->chctx_band_map & BIT(CC_BAND_24G)) ? HW_PHY_0 : HW_PHY_1;
	}

_exit:
	mr_ctl->cck_phyidx = phy_idx;
	PHL_INFO("%s => mr_ctl->cck_phyidx:%d\n", __func__, mr_ctl->cck_phyidx);
	return phy_idx;
}

enum rtw_hal_status
rtw_hal_dbcc_trx_ctrl(void *hal, struct rtw_phl_com_t *phl_com,
				enum phl_band_idx band_idx, bool pause)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	hsts = rtw_hal_mac_dbcc_trx_ctrl(hal_info, band_idx, pause);
	if (hsts != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("%s band_%d , pause:%d failed\n", __func__, band_idx, pause);
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return hsts;
}

enum rtw_hal_status
rtw_hal_dbcc_pre_cfg(void *hal, struct rtw_phl_com_t *phl_com, bool dbcc_en)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	if (!phl_com->dev_cap.dbcc_sup)
		goto exit_func;

	hal_com->dbcc_en = (dbcc_en) ? true : false;
	if (dbcc_en) { /*dbcc enable*/
		rtw_hal_init_hw_band_info(hal, HW_BAND_1);
		hsts = rtw_hal_reset(hal_com, HW_PHY_0, HW_BAND_0, true);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s - EN hal reset - HW_PHY_0, HW_BAND_0 failed\n", __func__);
			goto exit_func;
		}

		hsts = rtw_hal_mac_dbcc_pre_cfg(phl_com, hal_info, dbcc_en);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ASSERT("%s - EN mac_dbcc_pre_cfg failed\n", __func__);
			hal_com->dbcc_en = false;
			goto exit_func;
		}

		hsts = rtw_hal_ppdu_sts_init(hal, HW_BAND_1, true,
					       HAL_PPDU_MAC_INFO | HAL_PPDU_PLCP | HAL_PPDU_RX_CNT,
					       HAL_PPDU_HAS_CRC_OK); /*HAL_PPDU_HAS_A1M - 52b,HAL_PPDU_HAS_DMA_OK-52c */
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			hal_com->dbcc_en = false;
			PHL_ASSERT("%s - EN HW_BAND_1 ppdu_sts_init failed\n", __func__);
			goto exit_func;
		}
		phl_com->ppdu_sts_info.en_ppdu_sts[HW_BAND_1] = true;

		rtw_hal_mac_dbcc_trx_ctrl(hal_info, HW_BAND_1, true);

		hsts = rtw_hal_reset(hal_com, HW_PHY_1, HW_BAND_1, true);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s - EN hal reset - HW_PHY_1, HW_BAND_1 failed\n", __func__);
			goto exit_func;
		}

		/*phy_dbcc_pre_cfg*/
		hsts = rtw_hal_phy_dbcc_pre_cfg(hal_info, phl_com, dbcc_en);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ASSERT("%s - EN phy_dbcc_pre_cfg failed\n", __func__);
			hal_com->dbcc_en = false;
			goto exit_func;
		}
	}
	else { /*dbcc disable*/
		hsts = rtw_hal_reset(hal_com, HW_PHY_0, HW_BAND_0, true);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s - DIS hal reset - HW_PHY_0, HW_BAND_0 failed\n", __func__);
			goto exit_func;
		}

		hsts = rtw_hal_reset(hal_com, HW_PHY_1, HW_BAND_1, true);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s - DIS hal reset - HW_PHY_1, HW_BAND_1 failed\n", __func__);
			goto exit_func;
		}

		hsts = rtw_hal_mac_dbcc_pre_cfg(phl_com, hal_info, dbcc_en);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ASSERT("%s - DIS mac_dbcc_pre_cfg failed\n", __func__);
			hal_com->dbcc_en = true;
			goto exit_func;
		}
		/*phy_dbcc_pre_cfg*/
		hsts = rtw_hal_phy_dbcc_pre_cfg(hal_info, phl_com, dbcc_en);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ASSERT("%s - DIS phy_dbcc_pre_cfg failed\n", __func__);
			hal_com->dbcc_en = true;
			goto exit_func;
		}
	}

	PHL_INFO("%s-%s done and success\n", __func__, (dbcc_en) ? "EN" : "DIS");

exit_func:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return hsts;
}

enum rtw_hal_status
rtw_hal_dbcc_cfg(void *hal, struct rtw_phl_com_t *phl_com, bool dbcc_en)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	if (!phl_com->dev_cap.dbcc_sup)
		goto exit_func;

	if (dbcc_en) { /*dbcc enable*/
		/*mac_dbcc_cfg*/
		hsts = rtw_hal_mac_dbcc_cfg(phl_com, hal_info, dbcc_en);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ASSERT("%s - EN mac_dbcc_cfg failed\n", __func__);
			hal_com->dbcc_en = false;
			goto exit_func;
		}

		/*bb,rf_dbcc_cfg*/
		hsts = rtw_hal_phy_dbcc_cfg(hal_info, phl_com, dbcc_en);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ASSERT("%s - EN phy_dbcc_cfg failed\n", __func__);
			hal_com->dbcc_en = false;
			goto exit_func;
		}
	}
	else { /*dbcc disable*/
		hsts = rtw_hal_ppdu_sts_init(hal, HW_BAND_1, false, 0, 0);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			hal_com->dbcc_en = false;
			PHL_ASSERT("%s - DIS HW_BAND_1 ppdu_sts_deinit failed\n", __func__);
			goto exit_func;
		}
		phl_com->ppdu_sts_info.en_ppdu_sts[HW_BAND_1] = false;

		/*mac_dbcc_cfg*/
		hsts = rtw_hal_mac_dbcc_cfg(phl_com, hal_info, dbcc_en);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ASSERT("%s - DIS mac_dbcc_cfg failed\n", __func__);
			hal_com->dbcc_en = false;
			goto exit_func;
		}

		/*bb,rf_dbcc_cfg*/
		hsts = rtw_hal_phy_dbcc_cfg(hal_info, phl_com, dbcc_en);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ASSERT("%s - DIS phy_dbcc_cfg failed\n", __func__);
			hal_com->dbcc_en = false;
			goto exit_func;
		}
		hsts = rtw_hal_reset(hal_com, HW_PHY_0, HW_BAND_0, false);
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s - DIS hal reset - HW_PHY_0, HW_BAND_0 failed\n", __func__);
			goto exit_func;
		}
	}

	PHL_INFO("%s-%s done and success\n", __func__, (dbcc_en) ? "EN" : "DIS");

exit_func:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return hsts;
}

enum rtw_hal_status
rtw_hal_dbcc_reset_hal(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	hsts = rtw_hal_reset(hal_com, HW_PHY_0, HW_BAND_0, false);
	if (hsts != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s - hal reset - HW_PHY_0, HW_BAND_0 failed\n", __func__);
		_os_warn_on(1);
		goto exit_func;
	}
	hsts = rtw_hal_reset(hal_com, HW_PHY_1, HW_BAND_1, false);
	if (hsts != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s - hal reset - HW_PHY_1, HW_BAND_1 failed\n", __func__);
		_os_warn_on(1);
		goto exit_func;
	}

	hsts = RTW_HAL_STATUS_SUCCESS;

exit_func:
	return hsts;
}

enum rtw_hal_status
rtw_hal_dbcc_band_switch_hdl(void *hal, enum phl_band_idx band_idx)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	PHL_INFO("[assoc_sta_cnt] all:%d, B0:%d, B1:%d\n",
			hal_com->assoc_sta_cnt,
			hal_com->band[HW_BAND_0].assoc_sta_cnt,
			hal_com->band[HW_BAND_1].assoc_sta_cnt);

	hal_com->band[HW_BAND_0].assoc_sta_cnt =
		hal_com->band[HW_BAND_1].assoc_sta_cnt;
	hal_com->band[HW_BAND_1].assoc_sta_cnt = 0;

	PHL_INFO("[assoc_sta_cnt] all:%d, B0:%d, B1:%d\n",
			hal_com->assoc_sta_cnt,
			hal_com->band[HW_BAND_0].assoc_sta_cnt,
			hal_com->band[HW_BAND_1].assoc_sta_cnt);
	return RTW_HAL_STATUS_SUCCESS;
}

void rtw_hal_dbcc_hci_ctrl(void *hal, enum phl_band_idx band_idx, u8 pause)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	/* Stop DMA + Disable all of txch dma (Reset B1 TXBD idx to 0) */
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mac_dbcc_hci_ctrl(hal_info, band_idx, pause))
		PHL_ERR("%s failure \n", __func__);
}


void
hal_dbcc_cfg_phy_map(struct hal_info_t *hal, enum phl_band_idx band_idx)
{
	enum phl_phy_idx phy_idx = rtw_hal_hw_band_to_phy_idx(band_idx);
	enum phl_phy_idx cck_phy_idx = HW_PHY_MAX;

	rtw_hal_reset(hal->hal_com, phy_idx, band_idx, true);

	cck_phy_idx = hal_dbcc_cck_phyidx_decision(hal, hal->phl_com,
	                                           hal->hal_com->dbcc_en);
	PHL_INFO("%s: cck_phy_idx(%d)\n", __func__, cck_phy_idx);
	rtw_hal_bb_cfg_dbcc_phy_map(hal, cck_phy_idx);

	rtw_hal_reset(hal->hal_com, phy_idx, band_idx, false);
}
#endif /*CONFIG_DBCC_SUPPORT*/
