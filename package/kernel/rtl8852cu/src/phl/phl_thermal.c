/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#define _PHL_THERMAL_C_
#include "phl_headers.h"

#ifdef CONFIG_PHL_THERMAL_PROTECT

static void _phl_thermal_protect_disable_all_txop(
	struct phl_info_t *phl_info,
	bool disable)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_mld_t *mld = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct rtw_edca_param edca = {0};
	u8 i = 0;
	u8 idx = 0;

	for (i = 0; i < MAX_WIFI_ROLE_NUMBER; i++) {
		if (mr_ctl->role_map & BIT(i)) {
			wrole = phl_get_wrole_by_ridx(phl_info, i);
			if(wrole){
				if(wrole->mstate == MLME_LINKED)
					break;
			}
			wrole = NULL;
			continue;
		}
	}

	if(wrole == NULL)
		return;

	mld = rtw_phl_get_mld_self(phl_info, wrole);
	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);
		sta = mld->phl_sta[idx];

		if(sta == NULL)
			return;

		for(i = 0; i < 4;i++){
			edca.ac = i;
			edca.param = sta->asoc_cap.edca[edca.ac].param;
			if(disable)
				edca.param &= 0x0000FFFF;

			if(rtw_hal_set_edca(phl_info->hal, rlink, edca.ac, edca.param)
			   != RTW_HAL_STATUS_SUCCESS)
				PHL_ERR("%s Config edca fail\n", __FUNCTION__);
		}
	}
}

static void _phl_thermal_protect_reduce_ampdu_num(
	struct phl_info_t *phl_info,
	u8 ratio)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_phl_mld_t *mld = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 i = 0;
	u8 idx = 0;

	for (i = 0; i < MAX_WIFI_ROLE_NUMBER; i++) {
		if (mr_ctl->role_map & BIT(i)) {
			wrole = phl_get_wrole_by_ridx(phl_info, i);
			if(wrole){
				if(wrole->mstate == MLME_LINKED)
					break;
			}
			wrole = NULL;
			continue;
		}
	}

	if(wrole == NULL)
		return;

	mld = rtw_phl_get_mld_self(phl_info, wrole);
	for (idx = 0; idx < wrole->rlink_num; idx++) {
		sta = mld->phl_sta[idx];

		if(sta == NULL)
			return;

		if(ratio != 0){
			if(rtw_hal_thermal_protect_cfg_tx_ampdu(phl_info->hal, sta, ratio)
			   != RTW_HAL_STATUS_SUCCESS)
				PHL_ERR("%s Thermal protect cfg tx ampdu fail\n", __FUNCTION__);
		}
		else{
			if(sta->asoc_cap.num_ampdu_bk != 0){
				sta->asoc_cap.num_ampdu = sta->asoc_cap.num_ampdu_bk;
				sta->asoc_cap.num_ampdu_bk = 0;
			}
			if(rtw_hal_cfg_tx_ampdu(phl_info->hal, sta) !=
			   RTW_HAL_STATUS_SUCCESS)
				PHL_ERR("%s Thermal protect restore tx ampdu fail\n", __FUNCTION__);
		}
	}
}

void phl_thermal_protect_watchdog(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	bool action_changed = false;
	u8 min_tx_duty = phl_com->dev_cap.min_tx_duty;
	u8 next_tx_duty = THERMAL_NO_TX_DUTY_CTRL;
	u8 duty_interval = 1;

	if (min_tx_duty == THERMAL_NO_TX_DUTY_CTRL)
		return;

	if(phl_com->drv_mode != RTW_DRV_MODE_NORMAL &&
	   phl_com->drv_mode != RTW_DRV_MODE_HIGH_THERMAL)
		return;

	action_changed = rtw_hal_check_thermal_protect(phl_com, phl_info->hal);

	if(action_changed == false)
		return;

	duty_interval = (THERMAL_NO_TX_DUTY_CTRL - min_tx_duty) / PHL_THERMAL_PROTECT_ACTION_LEVEL_MAX;
	if (duty_interval == 0)
		duty_interval = 1;

	if (phl_com->thermal_protect_action == PHL_THERMAL_PROTECT_ACTION_NONE) {
		phl_thermal_protect_stop_tx_duty(phl_info);
	} else {
		next_tx_duty = THERMAL_TX_DUTY_CTRL_DURATION - (duty_interval * (u8)phl_com->thermal_protect_action);
		if (next_tx_duty >= min_tx_duty)
			phl_thermal_protect_cfg_tx_duty(phl_info,
							THERMAL_TX_DUTY_CTRL_DURATION,
							next_tx_duty);
	}
}

#endif /* CONFIG_PHL_THERMAL_PROTECT */

void phl_thermal_protect_cfg_tx_duty(
	struct phl_info_t *phl_info,
	u16 tx_duty_interval,
	u8 ratio)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;

	hal_status = rtw_hal_thermal_protect_cfg_tx_duty(phl_info->hal,
							 tx_duty_interval,
							 ratio);
	if(hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("%s Thermal protect cfg tx duty fail\n", __FUNCTION__);
}

void phl_thermal_protect_stop_tx_duty(struct phl_info_t *phl_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;

	hal_status = rtw_hal_thermal_protect_stop_tx_duty(phl_info->hal);
	if(hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("%s Thermal protect stop tx duty fail\n", __FUNCTION__);
}