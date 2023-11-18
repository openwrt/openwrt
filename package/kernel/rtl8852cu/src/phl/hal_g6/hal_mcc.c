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
#define _HAL_MCC_C_
#include "hal_headers.h"
#include "hal_mcc.h"

#ifdef CONFIG_MCC_SUPPORT
#define _mcc_fill_slot_bt_coex(_dbg_hal_i, _en) ((struct rtw_phl_mcc_dbg_hal_info *)_dbg_hal_i)->btc_in_group = _en;

void _mcc_update_slot_dbg_info(struct rtw_phl_mcc_dbg_hal_info *dbg_hal_i,
				bool bt_role, u16 macid, u16 dur)
{
	struct rtw_phl_mcc_dbg_slot_info *dbg_slot_i = NULL;
	u8 idx = 0;

	for (idx = 0; idx < dbg_hal_i->slot_num; idx++) {
		dbg_slot_i = &dbg_hal_i->dbg_slot_i[idx];
		if (dbg_slot_i->bt_role) {
			if (bt_role) {
				dbg_slot_i->dur = dur;
				break;
			}
		} else if (false == bt_role && dbg_slot_i->macid == macid) {
			dbg_slot_i->dur = dur;
			break;
		}
	}
}

void _mcc_update_dbg_info(struct rtw_phl_mcc_en_info *info,
			struct rtw_phl_mcc_bt_info *bt_info)
{
	u8 idx = 0;

	if (bt_info->bt_dur > 0) {
		_mcc_fill_slot_bt_coex(&info->dbg_hal_i, true);
	} else {
		_mcc_fill_slot_bt_coex(&info->dbg_hal_i, false);
	}
	for (idx = 0; idx < info->mrole_num; idx++) {
		_mcc_update_slot_dbg_info(&info->dbg_hal_i, false,
					info->mcc_role[idx].macid,
					info->mcc_role[idx].policy.dur_info.dur);
	}
}

void _mcc_fill_slot_dbg_info(struct rtw_phl_mcc_dbg_hal_info *dbg_hal_i,
				struct rtw_phl_mcc_role *mrole)
{
	struct rtw_phl_mcc_dbg_slot_info *dbg_slot_i = NULL;
	if (dbg_hal_i->slot_num >= SLOT_NUM) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_hal_dbg_info(): Fill fail, dbg_hal_i->slot_num(%d) >= SLOT_NUM(%d), please check code\n",
			dbg_hal_i->slot_num, SLOT_NUM);
		goto exit;
	}
	dbg_slot_i = &dbg_hal_i->dbg_slot_i[dbg_hal_i->slot_num];
	dbg_slot_i->bt_role = mrole->bt_role ? true : false;
	dbg_slot_i->dur = mrole->policy.dur_info.dur;
	if (false == dbg_slot_i->bt_role) {
		dbg_slot_i->ch = mrole->chandef->chan;
		dbg_slot_i->macid = mrole->macid;
	}
	dbg_hal_i->slot_num++;
exit:
	return;
}

enum rtw_hal_status _mcc_add_bt_role(struct hal_info_t *hal, u8 group,
			struct rtw_phl_mcc_slot_info *slot_i,
			struct rtw_phl_mcc_dbg_hal_info *dbg_hal_i)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_phl_mcc_role mrole = {0};

	mrole.policy.dur_info.dur = (u8)slot_i->slot;
	mrole.bt_role = true;
	mrole.group = group;
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_add_bt_role(): add bt role\n");
	_mcc_fill_slot_dbg_info(dbg_hal_i, &mrole);
	status = rtw_hal_mac_add_mcc(hal, &mrole);
	return status;
}

enum rtw_hal_status _mcc_add_wifi_role(struct hal_info_t *hal,
				u8 group, struct rtw_phl_mcc_slot_info *slot_i,
				struct rtw_phl_mcc_dbg_hal_info *dbg_hal_i)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_phl_mcc_role *mrole = slot_i->mrole;

	if (NULL == mrole) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_add_wifi_role(): Fail, NULL == mrole, Please check code\n");
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_add_wifi_role(): add wifi role\n");
	mrole->group = group;
	_mcc_fill_slot_dbg_info(dbg_hal_i, mrole);
	status = rtw_hal_mac_add_mcc(hal, mrole);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_add_wifi_role(): Add mcc failed\n");
		goto exit;
	}
	status = rtw_hal_mcc_update_macid_bitmap(hal, group,
						(u8)mrole->macid,
						&mrole->used_macid);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_add_wifi_role(): Add MACID bitmap failed\n");
		goto exit;
	}
exit:
	return status;
}

enum rtw_hal_status _mcc_fill_role_setting(struct hal_info_t *hal,
			struct rtw_phl_mcc_en_info *info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_phl_mcc_slot_info *slot_i = info->m_pattern.slot_order;
	u8 i = 0;

	if (info->m_pattern.slot_num > SLOT_NUM) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_role_setting(): Fial, slot_num(%d) > SLOT_NUM(%d), please check code\n",
			info->m_pattern.slot_num, SLOT_NUM);
		goto exit;
	} else if (info->m_pattern.slot_num < MIN_TDMRA_SLOT_NUM) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_role_setting(): Fial, slot_num(%d) < MIN_TDMRA_SLOT_NUM(%d), please check code\n",
			info->m_pattern.slot_num, MIN_TDMRA_SLOT_NUM);
		goto exit;
	}
	for (i = 0; i < info->m_pattern.slot_num; i++) {
		if (!slot_i[i].bt_role) {
			if (RTW_HAL_STATUS_SUCCESS!= _mcc_add_wifi_role(hal,
				info->group, &slot_i[i], &info->dbg_hal_i)) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_role_setting(): Add wifi role failed\n");
				goto exit;
			}
		} else {
			if (RTW_HAL_STATUS_SUCCESS != _mcc_add_bt_role(hal,
				info->group, &slot_i[i], &info->dbg_hal_i)) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_role_setting(): Add bt role failed\n");
				goto exit;
			}
		}
	}
	status = RTW_HAL_STATUS_SUCCESS;
exit:
	return status;
}

enum rtw_hal_status _mcc_replace_pattern(struct hal_info_t *hal,
				struct rtw_phl_mcc_en_info *ori_info,
				struct rtw_phl_mcc_en_info *new_info,
				struct rtw_phl_mcc_bt_info *new_bt_info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_phl_mcc_role *ref_role =
				&new_info->mcc_role[new_info->ref_role_idx];
	bool btc_in_group = false;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_replace_pattern\n");
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mac_get_mcc_group(hal, &new_info->group)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_replace_pattern(): Allocate group fail\n");
		goto exit;
	}
	_os_mem_set(hal_to_drvpriv(hal), &new_info->dbg_hal_i, 0,
			sizeof(struct rtw_phl_mcc_dbg_hal_info));
	status = _mcc_fill_role_setting(hal, new_info);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_replace_pattern(): Fail fill role setting\n");
		goto exit;
	}
	if (new_info->sync_tsf_info.sync_en) {
		status = rtw_hal_mcc_sync_enable(hal, new_info);
		if (status != RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_replace_pattern(): Sync enable failed\n");
			goto exit;
		}
	}
	if (new_bt_info->bt_dur > 0) {
		btc_in_group = true;
		_mcc_fill_slot_bt_coex(&new_info->dbg_hal_i, true);
	} else {
		_mcc_fill_slot_bt_coex(&new_info->dbg_hal_i, false);
	}
	status = rtw_hal_mac_start_mcc(hal, new_info->group,
					(u8)ref_role->macid, new_info->tsf_high,
					new_info->tsf_low, btc_in_group, 1, ori_info->group);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_replace_pattern(): Start MCC failed\n");
		goto exit;
	}
	status = RTW_HAL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< _mcc_replace_pattern(): Ststus(%d)\n",
		status);
	return status;
}

void
_mcc_check_start_t(struct rtw_phl_mcc_en_info *ori_info,
			struct rtw_phl_mcc_en_info *new_info)
{
	u64 new_tsf = 0, ori_tsf = 0;

	new_tsf = new_info->tsf_high;
	new_tsf = new_tsf << 32;
	new_tsf |= new_info->tsf_low;
	ori_tsf = ori_info->tsf_high;
	ori_tsf = ori_tsf << 32;
	ori_tsf |= ori_info->tsf_low;
	if (new_tsf < ori_tsf) {
		u64 cnt = 0;

		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "%s: new_tsf(0x%08x %08x) < ori_tsf(0x%08x %08x), we shall extend the new start tsf\n",
			__FUNCTION__, new_info->tsf_high, new_info->tsf_low,
			ori_info->tsf_high, ori_info->tsf_low);
		cnt = _os_division64((ori_tsf - new_tsf),
				     (new_info->mcc_intvl * TU));
		new_tsf = new_tsf + ((cnt + 1) * (new_info->mcc_intvl * TU));
		new_info->tsf_high = (u32)(new_tsf >> 32);
		new_info->tsf_low = (u32)new_tsf;
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "%s: extended new_tsf(0x%08x %08x)\n",
			__FUNCTION__, new_info->tsf_high, new_info->tsf_low);
	}
}

enum rtw_hal_status rtw_hal_mcc_get_2ports_tsf(void *hal, u8 group,
			u16 macid_x, u16 macid_y, u32 *tsf_x_h, u32 *tsf_x_l,
			u32 *tsf_y_h, u32 *tsf_y_l)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_hal_mac_mcc_request_tsf(hal, group, (u8)macid_x, (u8)macid_y);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_get_2ports_tsf(): Req tsf failed\n");
		goto exit;
	}
	status = rtw_hal_mac_get_mcc_tsf_rpt(hal, group, tsf_x_h, tsf_x_l,
						tsf_y_h, tsf_y_l);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_get_2ports_tsf(): Get tsf rpt failed\n");
		goto exit;
	}
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_hal_mcc_get_2ports_tsf(): Ststus(%d)\n",
		status);
	return status;
}

enum rtw_hal_status rtw_hal_notify_mcc_macid(void *hal,
                                             struct rtw_phl_mcc_role *mrole,
                                             enum rtw_phl_tdmra_wmode wmode)
{
	enum rtw_hal_status sts = RTW_HAL_STATUS_FAILURE;

	if (wmode == RTW_PHL_TDMRA_AP_CLIENT_WMODE ||
	    wmode == RTW_PHL_TDMRA_2CLIENTS_WMODE) {
		if (sts != rtw_hal_bb_upd_mcc_macid(hal, mrole)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s: Notify BB MCC MACID bitmap failed\n",
			          __FUNCTION__);
			goto exit;

		}
	}
	sts = RTW_HAL_STATUS_SUCCESS;
exit:
	return sts;
}

enum rtw_hal_status rtw_hal_mcc_update_macid_bitmap(void *hal, u8 group,
			u16 macid, struct rtw_phl_mcc_macid_bitmap *info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_hal_mac_mcc_macid_bitmap(hal, group, (u8)macid,
						(u8 *)info->bitmap, info->len);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_update_macid_bitmap(): Add MACID bitmap failed\n");
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_hal_mcc_update_macid_bitmap(): Ststus(%d)\n",
		status);
	return status;
}

enum rtw_hal_status rtw_hal_mcc_sync_enable(void *hal,
					struct rtw_phl_mcc_en_info *info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_phl_mcc_role *ref_role = &info->mcc_role[info->ref_role_idx];

	if (info->sync_tsf_info.sync_en) {
		status = rtw_hal_mac_mcc_sync_enable(hal, ref_role->group,
						(u8)info->sync_tsf_info.source,
						(u8)info->sync_tsf_info.target,
						(u8)info->sync_tsf_info.offset);
		if (status != RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_sync_enable(): Sync enable failed\n");
			goto exit;
		}
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_sync_enable(): sync_en == false, please check code\n");
		goto exit;
	}
	status = RTW_HAL_STATUS_SUCCESS;
exit:
	return status;
}

enum rtw_hal_status _mcc_set_duration(void *hal,
					struct rtw_phl_mcc_en_info *info,
					struct rtw_phl_mcc_bt_info *bt_info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_hal_mac_set_duration(hal, info, bt_info);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_set_duration(): set duration failed\n");
	} else {
		_mcc_update_dbg_info(info, bt_info);
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_set_duration(): Ststus(%d)\n",
		status);
	return status;
}

enum rtw_hal_status rtw_hal_mcc_change_pattern(void *hal,
				struct rtw_phl_mcc_en_info *ori_info,
				struct rtw_phl_mcc_en_info *new_info,
				struct rtw_phl_mcc_bt_info *new_bt_info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	_mcc_check_start_t(ori_info, new_info);
	/* mcc_set_duration only can change 2 wifi slot,
	otherwise we shall create new pattern and replace previous pattern*/
	if (new_info->m_pattern.slot_num == 2 &&
	    new_info->m_pattern.bt_slot_num == 0 &&
	    ori_info->m_pattern.slot_num == 2 &&
	    ori_info->m_pattern.bt_slot_num == 0) {
		status = _mcc_set_duration(hal, new_info, new_bt_info);
	} else {
		status = _mcc_replace_pattern(hal, ori_info, new_info,
						new_bt_info);
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_hal_mcc_change_pattern(): status(%d)\n",
		status);
	return status;
}

enum rtw_hal_status rtw_hal_mcc_reset(void *hal, u8 group,
					enum rtw_phl_tdmra_wmode wmode)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	status = rtw_hal_mac_reset_mcc_group(hal, group);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_,
			"%s(): reset group(%d) failed\n",
			__func__, group);
		goto exit;
	}
	if (wmode == RTW_PHL_TDMRA_AP_CLIENT_WMODE ||
		wmode == RTW_PHL_TDMRA_2CLIENTS_WMODE) {
		rtw_hal_bb_mcc_stop(hal);
	}

	status = RTW_HAL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s(): status(%d)\n",
		__func__, status);
	return status;
}

enum rtw_hal_status rtw_hal_mcc_disable(void *hal, u8 group, u16 macid,
					enum rtw_phl_tdmra_wmode wmode)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_i = (struct hal_info_t *)hal;
	struct rtw_phl_stainfo_t *sta = NULL;

	sta = rtw_phl_get_stainfo_by_macid(hal_i->phl_com->phl_priv, macid);
	status = rtw_hal_mac_stop_mcc(hal, group, (u8)macid);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_disable(): Stop mcc failed\n");
		goto exit;
	}
	status = rtw_hal_mac_del_mcc_group(hal, group);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_disable(): Delete group failed\n");
		goto exit;
	}
	if (wmode == RTW_PHL_TDMRA_AP_CLIENT_WMODE ||
		wmode == RTW_PHL_TDMRA_2CLIENTS_WMODE) {
		rtw_hal_bb_mcc_stop(hal);
	}

#ifdef RTW_WKARD_HALRF_MCC
	rtw_hal_rf_dpk_switch(hal, true);
#endif /* RTW_WKARD_HALRF_MCC */
	rtw_hal_notification(hal, MSG_EVT_MCC_STOP, sta->rlink->hw_band);
	status = RTW_HAL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_hal_mcc_disable(): Ststus(%d)\n",
		status);
	return status;
}

enum rtw_hal_status rtw_hal_mcc_enable(void *hal, struct rtw_phl_mcc_en_info *info,
					struct rtw_phl_mcc_bt_info *bt_info,
					enum rtw_phl_tdmra_wmode wmode)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_phl_mcc_role *ref_role = &info->mcc_role[info->ref_role_idx];
	bool btc_in_group = false;

	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mac_get_mcc_group(hal, &info->group)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_enable(): Allocate group fail\n");
		goto exit;
	}
#ifdef RTW_WKARD_HALRF_MCC
	rtw_hal_rf_dpk_switch(hal, false);
#endif /* RTW_WKARD_HALRF_MCC */
	if (RTW_HAL_STATUS_SUCCESS != _mcc_fill_role_setting(hal, info)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_enable(): Fail fill role setting\n");
		goto exit;
	}
/*fill start setting*/
	if (info->sync_tsf_info.sync_en) {
		status = rtw_hal_mcc_sync_enable(hal, info);
		if (status != RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_enable(): Sync enable failed\n");
			goto exit;
		}
	}
	if (bt_info->bt_dur > 0) {
		btc_in_group = true;
		_mcc_fill_slot_bt_coex(&info->dbg_hal_i, true);
	} else {
		_mcc_fill_slot_bt_coex(&info->dbg_hal_i, false);
	}
	status = rtw_hal_mac_start_mcc(hal, info->group,
					(u8)ref_role->macid, info->tsf_high,
					info->tsf_low, btc_in_group, 0, 0);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_enable(): Start MCC failed\n");
		goto exit;
	}

	if (wmode == RTW_PHL_TDMRA_AP_CLIENT_WMODE ||
		wmode == RTW_PHL_TDMRA_2CLIENTS_WMODE) {
		status = rtw_hal_bb_mcc_start(hal, &info->mcc_role[0],
		                              &info->mcc_role[1]);
		if (status != RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_hal_mcc_enable(): Notify MCC start failed\n");
		}
	}
	rtw_hal_notification(hal, MSG_EVT_MCC_START, ref_role->rlink->hw_band);
	status = RTW_HAL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_hal_mcc_enable(): Ststus(%d)\n",
		status);
	return status;
}
#endif /* CONFIG_MCC_SUPPORT */
