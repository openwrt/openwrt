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
#define _PHL_MCC_C_
#include "phl_headers.h"

#ifdef CONFIG_MCC_SUPPORT
#include "phl_mcc.h"
void _mcc_dump_state(enum rtw_phl_mcc_state *state)
{
	if (MCC_NONE == *state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_state(): MCC_NONE\n");
	} else if (MCC_CFG_EN_INFO == *state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_state(): MCC_CFG_EN_INFO\n");
	} else if (MCC_TRIGGER_FW_EN == *state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_state(): MCC_TRIGGER_FW_EN\n");
	} else if (MCC_FW_EN_FAIL == *state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_state(): MCC_FW_EN_FAIL\n");
	} else if (MCC_RUNING == *state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_state(): MCC_RUNING\n");
	} else if (MCC_TRIGGER_FW_DIS == *state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_state(): MCC_TRIGGER_FW_DIS\n");
	} else if (MCC_FW_DIS_FAIL == *state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_state(): MCC_FW_DIS_FAIL\n");
	} else if (MCC_STOP == *state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_state(): MCC_STOP\n");
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_dump_state(): Undefined state(%d)\n",
			*state);
	}
}

void _mcc_dump_mode(enum rtw_phl_tdmra_wmode *mode)
{
	if (RTW_PHL_TDMRA_AP_CLIENT_WMODE == *mode) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_mode(): RTW_PHL_TDMRA_AP_CLIENT_WMODE\n");
	} else if (RTW_PHL_TDMRA_2CLIENTS_WMODE == *mode) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_mode(): RTW_PHL_TDMRA_2CLIENTS_WMODE\n");
	} else if (RTW_PHL_TDMRA_AP_WMODE == *mode) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_mode(): RTW_PHL_TDMRA_AP_WMODE\n");
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_dump_mode(): Undefined mode(%d)\n",
			*mode);
	}
}

void _mcc_dump_sync_tsf_info(struct rtw_phl_mcc_sync_tsf_info *info)
{
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_sync_tsf_info(): sync_en(%d), source macid(%d), target macid(%d), offset(%d)\n",
		info->sync_en, info->source, info->target, info->offset);
}

void _mcc_dump_dur_info(struct rtw_phl_mcc_dur_info *dur_i)
{
	struct rtw_phl_mcc_dur_lim_info *dur_l = NULL;

	dur_l = &dur_i->dur_limit;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_dump_role_info(): dur(%d), dur lim info: enable(%d), tag(%d), max_dur(%d)\n",
		dur_i->dur, dur_l->enable, dur_l->tag, dur_l->max_dur);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< _mcc_dump_role_info(): max_toa(%d), max_tob(%d)\n",
		dur_l->max_toa, dur_l->max_tob);
}

void _mcc_dump_role_info(struct rtw_phl_mcc_role *mrole)
{
	struct rtw_phl_mcc_policy_info *policy = &mrole->policy;
	u8 i = 0;

	policy = &mrole->policy;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_dump_role_info(): wrole id(%d), rlink id(%d) type(%d), macid(%d), bcn_intvl(%d)\n",
		mrole->wrole->id, mrole->rlink->id, mrole->wrole->type, mrole->macid,
		mrole->bcn_intvl);

	for (i = 0; i < PHL_MACID_MAX_ARRAY_NUM; i++) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_role_info(): macid_map[%d]= 0x%08X\n",
			i, mrole->used_macid.bitmap[i]);
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_role_info(): chan(%d), center_ch(%d), bw(%d), offset(%d)\n",
		mrole->chandef->chan, mrole->chandef->center_ch,
		mrole->chandef->bw, mrole->chandef->offset);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_role_info(): group(%d), c2h_rpt(%d), tx_null_early(%d)\n",
		mrole->group, policy->c2h_rpt, policy->tx_null_early);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_role_info(): dis_tx_null(%d), in_curr_ch(%d), dis_sw_retry(%d)\n",
		policy->dis_tx_null, policy->in_curr_ch, policy->dis_sw_retry);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_role_info(): sw_retry_count(%d), rfk_chk(%d)\n",
		policy->sw_retry_count, policy->rfk_chk);
	_mcc_dump_dur_info(&policy->dur_info);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< _mcc_dump_role_info(): courtesy_en(%d), courtesy_num(%d), courtesy_target(0x%x)\n",
		policy->courtesy_en, policy->courtesy_num,
		policy->courtesy_target);
}

void _mcc_dump_pattern(struct rtw_phl_mcc_pattern *m_pattern)
{
	struct rtw_phl_mcc_courtesy *courtesy_i = &m_pattern->courtesy_i;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_dump_pattern(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d), bcns_offset(%d), calc_fail(%d), d_r_d_a_spacing_max(%d), c_en(%d)\n",
		m_pattern->tob_r, m_pattern->toa_r, m_pattern->tob_a,
		m_pattern->toa_a, m_pattern->bcns_offset, m_pattern->calc_fail,
		m_pattern->d_r_d_a_spacing_max, courtesy_i->c_en);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_pattern(): slot_num(%d), bt_slot_num(%d)\n",
		m_pattern->slot_num, m_pattern->bt_slot_num);
	if (courtesy_i->c_en) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< _mcc_dump_pattern(): c_en(%d), c_num(%d), src_role->macid(0x%x), tgt_role->macid(0x%x)\n",
			courtesy_i->c_en, courtesy_i->c_num,
			courtesy_i->src_role->macid,
			courtesy_i->tgt_role->macid);
	}
}

void _mcc_dump_ref_role_info(struct rtw_phl_mcc_en_info *info)
{
	struct rtw_phl_mcc_role *ref_role = NULL;

	ref_role = get_ref_role(info);

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_ref_role_info(): mrole idx(%d), wrole id(%d), rlink id(%d), macid(%d) chan(%d), bw(%d), offset(%d)\n",
		info->ref_role_idx, ref_role->wrole->id, ref_role->rlink->id,
		ref_role->macid,
		ref_role->chandef->chan, ref_role->chandef->bw,
		ref_role->chandef->offset);
}

void _mcc_dump_en_info(struct rtw_phl_mcc_en_info *info)
{
	struct rtw_phl_mcc_role *m_role = NULL;
	u8 midx = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_en_info(): mrole_map(0x%x), role_num(%d), mcc_intvl(%d), Start tsf(0x%08X %08X)\n",
		info->mrole_map, info->mrole_num, info->mcc_intvl,
		info->tsf_high, info->tsf_low);
	_mcc_dump_ref_role_info(info);
	_mcc_dump_sync_tsf_info(&info->sync_tsf_info);
	_mcc_dump_pattern(&info->m_pattern);
	for (midx = 0; midx < MCC_ROLE_NUM; midx++) {
		if (!(info->mrole_map & BIT(midx)))
			continue;
		m_role = &info->mcc_role[midx];
		_mcc_dump_role_info(m_role);
	}
}

void _mcc_dump_bt_ino(struct rtw_phl_mcc_bt_info *bt_info)
{
	u8 seg_num = BT_SEG_NUM;

	if (bt_info->bt_seg_num > seg_num)
		return;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_dump_bt_ino(): bt_dur(%d), bt_seg_num(%d), bt_seg[0](%d), bt_seg[1](%d), add_bt_role(%d)\n",
		bt_info->bt_dur, bt_info->bt_seg_num, bt_info->bt_seg[0],
		bt_info->bt_seg[1], bt_info->add_bt_role);
}

void _mcc_dump_mcc_info(struct phl_mcc_info *minfo)
{
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_dump_mcc_info():\n");
	_mcc_dump_mode(&minfo->mcc_mode);
	_mcc_dump_state(&minfo->state);
	_mcc_dump_bt_ino(&minfo->bt_info);
	_mcc_dump_en_info(&minfo->en_info);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< _mcc_dump_mcc_info():\n");
}

void _mcc_set_state(struct phl_mcc_info *minfo, enum rtw_phl_mcc_state state)
{
	PHL_TRACE(COMP_PHL_MCC, _PHL_ALWAYS_, "_mcc_set_state(): Set from (%d) to (%d)\n",
		minfo->state, state);
	minfo->state = state;
	_mcc_dump_state(&minfo->state);
}

struct rtw_phl_mcc_role *
_mcc_get_mrole_by_wrole(struct phl_mcc_info *minfo,
				struct rtw_wifi_role_t *wrole)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_role *m_role = NULL;
	u8 midx = 0;

	for (midx = 0; midx < MCC_ROLE_NUM; midx++) {
		if (!(en_info->mrole_map & BIT(midx)))
			continue;
		m_role = &en_info->mcc_role[midx];
		if (m_role->wrole == wrole) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_get_mrole_by_wrole(): Get mrole in mrole_idx(%d), wrole->type(%d), wrole->id(%d)\n",
				midx, wrole->type, wrole->id);
			return m_role;
		}
	}
	return NULL;
}

u8
_mcc_get_mrole_idx_by_wrole(struct phl_mcc_info *minfo,
				struct rtw_wifi_role_t *wrole, u8 *idx)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_role *m_role = NULL;
	u8 midx = 0;

	for (midx = 0; midx < MCC_ROLE_NUM; midx++) {
		if (!(en_info->mrole_map & BIT(midx)))
			continue;
		m_role = &en_info->mcc_role[midx];
		if (m_role->wrole == wrole) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_get_mrole_idx_by_wrole(): Get mrole in mrole_idx(%d)\n",
				midx);
			*idx = midx;
			status = RTW_PHL_STATUS_SUCCESS;
			break;
		}
	}
	return status;
}

struct rtw_phl_mcc_role *
_mcc_get_mrole_by_category(struct rtw_phl_mcc_en_info *en_info,
			enum _mcc_role_cat category)
{
	struct rtw_phl_mcc_role *m_role = NULL;
	u8 midx = 0;

	for (midx = 0; midx < MCC_ROLE_NUM; midx++) {
		if (!(en_info->mrole_map & BIT(midx)))
			continue;
		m_role = &en_info->mcc_role[midx];
		if (MCC_ROLE_AP_CAT == category) {
			if (rtw_phl_role_is_ap_category(m_role->wrole))
				return m_role;
		} else if (MCC_ROLE_CLIENT_CAT == category) {
			if (rtw_phl_role_is_client_category(m_role->wrole))
				return m_role;
		} else {
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_get_mrole_by_category(): Undefined category(%d)\n",
				category);
			break;
		}
	}
	return NULL;
}

enum rtw_phl_status _mcc_transfer_mode(struct phl_info_t *phl,
				struct phl_mcc_info *minfo, u8 role_map)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_wifi_role_t *wrole = NULL;
	u8 ridx = 0, ap_num = 0, client_num = 0;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (!(role_map & BIT(ridx)))
			continue;
		wrole = phl_get_wrole_by_ridx(phl, ridx);
		if (wrole == NULL) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_transfer_mode(): get wrole fail, role_idx(%d)\n",
				ridx);
			goto exit;
		}
		if (rtw_phl_role_is_client_category(wrole)) {
			client_num++;
		} else if (rtw_phl_role_is_ap_category(wrole)) {
			ap_num++;
		} else {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_transfer_mode(): undefined category, role->type(%d), ridx(%d), shall check code flow\n",
				wrole->type, ridx);
			goto exit;
		}
	}
	if ((client_num + ap_num > MAX_MCC_GROUP_ROLE)){
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_transfer_mode(): client_num(%d) + ap_num(%d) is illegal num, please check code flow\n",
			client_num, ap_num);
		goto exit;
	}
	if (ap_num == 1 && client_num == 1) {
		minfo->mcc_mode = RTW_PHL_TDMRA_AP_CLIENT_WMODE;
	} else if (ap_num == 0 && client_num == 2) {
		minfo->mcc_mode = RTW_PHL_TDMRA_2CLIENTS_WMODE;
	} else if (ap_num == 1 && client_num == 0) {
		minfo->mcc_mode = RTW_PHL_TDMRA_AP_WMODE;
	} else {
		minfo->mcc_mode = RTW_PHL_TDMRA_UNKNOWN_WMODE;
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_transfer_mode(): Undefined mode, please check code flow\n");
		goto exit;
	}
	_mcc_dump_mode(&minfo->mcc_mode);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

enum rtw_phl_status _mcc_get_role_map(struct phl_info_t *phl,
				struct hw_band_ctl_t *band_ctrl, u8 *role_map)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl);
	struct rtw_chan_ctx *chanctx = NULL;
	_os_list *chan_ctx_list = &band_ctrl->chan_ctx_queue.queue;

	*role_map = 0;
	_os_spinlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
	phl_list_for_loop(chanctx, struct rtw_chan_ctx, chan_ctx_list, list) {
		*role_map |= chanctx->role_map;
	}
	_os_spinunlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_get_role_map(): role_map(%d)\n",
		*role_map);
	status = RTW_PHL_STATUS_SUCCESS;
	return status;
}

void _mcc_set_fw_log_info(struct phl_info_t *phl, u8 hw_band,
			bool en_fw_mcc_log, u8 fw_mcc_log_lv)
{
	struct phl_mcc_info *minfo = get_mcc_info(phl, hw_band);

	if (minfo->fw_log_i.en_fw_mcc_log != en_fw_mcc_log) {
		minfo->fw_log_i.en_fw_mcc_log = en_fw_mcc_log;
		minfo->fw_log_i.update = true;
	}
	if (minfo->fw_log_i.fw_mcc_log_lv != fw_mcc_log_lv) {
		minfo->fw_log_i.fw_mcc_log_lv = fw_mcc_log_lv;
		minfo->fw_log_i.update = true;
	}
}

void _mcc_up_fw_log_setting(struct phl_info_t *phl, struct phl_mcc_info *minfo)
{
	struct phl_mcc_fw_log_info *fw_log_i = &minfo->fw_log_i;

	if (fw_log_i->update) {
		rtw_hal_en_fw_log(phl->hal, FL_COMP_MCC,
					fw_log_i->en_fw_mcc_log);
		fw_log_i->update = false;
	}
}

void _mcc_set_unspecific_dur(struct phl_mcc_info *minfo)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_role *m_role = NULL;
	u8 midx = 0;

	for (midx = 0; midx < MCC_ROLE_NUM; midx++) {
		if (!(en_info->mrole_map & BIT(midx)))
			continue;
		m_role = &en_info->mcc_role[midx];
		m_role->policy.dur_info.dur = MCC_DUR_NONSPECIFIC;
	}
}

void _mcc_fill_dur_lim_info(struct phl_info_t *phl,
				struct rtw_phl_mcc_role *mrole,
				struct phl_mcc_dur_lim_req_info *dur_req)
{
	struct rtw_phl_mcc_dur_info *dur_i = &mrole->policy.dur_info;
	struct rtw_phl_mcc_dur_lim_info *dur_lim = &dur_i->dur_limit;
	u64 tsf_lim = 0;
	u32 max_toa = 0, max_tob = 0, max_dur = 0;
	u32 bcn_intvl = mrole->bcn_intvl * TU;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_dur_lim_info(): dur_req: tag(%d), enable(%d), start_t_h(0x%08x), start_t_l(0x%08x), dur(%d), intvl(%d)\n",
		dur_req->tag, dur_req->enable, dur_req->start_t_h,
		dur_req->start_t_l, dur_req->dur, dur_req->intvl);
	dur_lim->enable = false;
	if (!dur_req->enable) {
		goto exit;
	}
	if (bcn_intvl != dur_req->intvl) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_dur_lim_info(): not support bcn_intvl(%d) != dur_req.intvl(%d), please check code\n",
			bcn_intvl, dur_req->intvl);
		goto exit;
	}
	/*Assume bcn allocat in TSF % BcnInvtal = 0*/
	tsf_lim = dur_req->start_t_h;
	tsf_lim = tsf_lim << 32;
	tsf_lim |= dur_req->start_t_l;
	max_toa = (u16)_os_modular64(tsf_lim, mrole->bcn_intvl * TU);
	if (max_toa >= (mrole->bcn_intvl * TU - dur_req->dur) ||
		max_toa == 0) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_dur_lim_info(): not support bcn allocate in limited slot, please check code\n");
		goto exit;
	}
	max_dur = dur_req->intvl - dur_req->dur;
	max_toa = max_toa / TU;
	max_dur = max_dur / TU;
	max_tob = max_dur - max_toa;

	dur_lim->max_toa = (u16)max_toa;
	dur_lim->max_tob = (u16)max_tob;
	dur_lim->max_dur = (u16)max_dur;
	dur_lim->tag = dur_req->tag;
	dur_lim->enable = true;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_dur_lim_info(): dur_lim_info: enable(%d), tag(%d), max_toa(%d), max_tob(%d), max_dur(%d)\n",
			dur_lim->enable, dur_lim->tag, dur_lim->max_toa,
			dur_lim->max_tob, dur_lim->max_dur);
	return;
}

void _mcc_fill_default_policy(struct phl_info_t *phl,
				struct rtw_phl_mcc_role *mrole)
{
	struct rtw_phl_mcc_policy_info *policy = &mrole->policy;
	struct phl_mcc_dur_lim_req_info dur_req = {0};

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_default_policy(): set mcc policy by default setting\n");
	policy->c2h_rpt = RTW_MCC_RPT_ALL;
	policy->tx_null_early = 3;
	policy->dis_tx_null = rtw_phl_role_is_client_category(mrole->wrole) ? 0 : 1;
	policy->in_curr_ch = 0;
	policy->dis_sw_retry = 1;
	policy->sw_retry_count = 0;
	policy->dur_info.dur = rtw_phl_role_is_client_category(mrole->wrole) ?
					DEFAULT_CLIENT_DUR : DEFAULT_AP_DUR;

	phl_mr_coex_query_role_time_slot_lim(phl, mrole->wrole, mrole->rlink, &dur_req);
	_mcc_fill_dur_lim_info(phl, mrole, &dur_req);

	policy->rfk_chk = rtw_hal_check_ch_rfk(phl->hal, &mrole->rlink->chandef);
	if (false == policy->rfk_chk) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_default_policy(): No rfk, it will degrade perormance, please check code\n");
	}
}

void _mcc_fill_mcc_role_policy_info(struct phl_info_t *phl,
                                    struct rtw_wifi_role_t *wrole,
                                    struct rtw_wifi_role_link_t *rlink,
                                    struct rtw_phl_mcc_role *mrole)
{
	struct phl_com_mcc_info *com_info = phl_to_com_mcc_info(phl);
	struct rtw_phl_mcc_policy_info *policy = &mrole->policy;
	struct rtw_phl_mcc_setting_info param = {0};
	struct phl_mcc_info *minfo = NULL;
	u8 hw_band = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, ">>> _mcc_fill_mcc_role_policy_info()\n");

	hw_band = rlink->hw_band;
	minfo = get_mcc_info(phl, hw_band);

	_mcc_fill_default_policy(phl, mrole);
	if (NULL == com_info->ops.mcc_get_setting)
		goto exit;
	param.wrole = wrole;
	param.role_map = minfo->role_map;
	param.tx_null_early = NONSPECIFIC_SETTING;
	param.dur = NONSPECIFIC_SETTING;
	if (!com_info->ops.mcc_get_setting(com_info->ops.priv, &param)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_mcc_role_policy_info(): mcc_get_setting from core layer fail\n");
		goto exit;
	}
	if (NONSPECIFIC_SETTING != param.tx_null_early) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_mcc_role_policy_info(): Core layer change tx_null_early from %d to %d\n",
			policy->tx_null_early, param.tx_null_early);
		policy->tx_null_early = param.tx_null_early;
	}
	if (NONSPECIFIC_SETTING != param.dur) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_mcc_role_policy_info(): Core layer change dur from %d to %d\n",
			policy->dur_info.dur, param.dur);
		policy->dur_info.dur = param.dur;
	}
	_mcc_set_fw_log_info(phl, hw_band, param.en_fw_mcc_log,
				param.fw_mcc_log_lv);
exit:
	return;
}

void _mcc_fill_macid_bitmap_by_role(struct phl_info_t *phl,
					struct rtw_phl_mcc_role *mrole)
{
	struct macid_ctl_t *mc = phl_to_mac_ctrl(phl);
	struct rtw_phl_mcc_macid_bitmap *used_macid = &mrole->used_macid;
	u8 i = 0, max_map_idx = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_fill_macid_bitmap_by_role()\n");
	for (i = 0; i < PHL_MACID_MAX_ARRAY_NUM; i++) {
		if ((mc->wifi_role_usedmap[mrole->wrole->id][i] != 0) &&
							(max_map_idx <= i)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_macid_bitmap_by_role(): macid_map[%d]=0x%08x\n",
				i, mc->wifi_role_usedmap[mrole->wrole->id][i]);
			max_map_idx = i;
		}
	}
	used_macid->bitmap = &mc->wifi_role_usedmap[mrole->wrole->id][0];
	used_macid->len = (max_map_idx + 1) * sizeof(u32);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_macid_bitmap_by_role(): bitmap->len(%d), max_map_idx(%d)\n",
		used_macid->len, max_map_idx);
}

enum rtw_phl_status
_mcc_fill_mcc_role_basic_info(struct phl_info_t *phl,
                              struct rtw_wifi_role_t *wrole,
                              struct rtw_wifi_role_link_t *rlink,
                              struct rtw_phl_mcc_role *mrole)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_stainfo_t *sta = rtw_phl_get_stainfo_self(phl, rlink);

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_fill_mcc_role_basic_info()\n");
	if (sta == NULL) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_mcc_role_basic_info(): transfer mcc mode failed\n");
		goto exit;
	}
	mrole->wrole = wrole;
	mrole->macid = sta->macid;
	mrole->rlink = rlink;

	mrole->bcn_intvl = phl_role_get_bcn_intvl(phl, wrole, rlink);
	if (mrole->bcn_intvl == 0) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_mcc_role_basic_info(): mrole->bcn_intvl ==0, please check code of core layer.\n");
		goto exit;
	}

	mrole->chandef = &rlink->chandef;

	_mcc_fill_macid_bitmap_by_role(phl, mrole);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

enum rtw_phl_status
_mcc_fill_ref_role_info(struct phl_info_t *phl,
                        struct rtw_phl_mcc_en_info *en_info,
                        struct rtw_wifi_role_t *wrole,
                        struct rtw_wifi_role_link_t *rlink
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_mcc_role *mrole = NULL;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_fill_ref_role_info()\n");
	mrole = &en_info->mcc_role[REF_ROLE_IDX];

	status = _mcc_fill_mcc_role_basic_info(phl, wrole,rlink, mrole);
	if (RTW_PHL_STATUS_SUCCESS != status) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_ref_role_info(): set basic info failed\n");
		goto exit;
	}

	_mcc_fill_mcc_role_policy_info(phl, wrole, rlink, mrole);

	en_info->mrole_map |= BIT(REF_ROLE_IDX);
	en_info->mrole_num++;
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_ref_role_info(): status(%d), wrole id(%d), wrole->type(%d), rlink id (%d) Fill mrole(%d) Info\n",
			status, wrole->id, wrole->type, rlink->id, REF_ROLE_IDX);

	return status;
}

enum rtw_phl_status
_mcc_fill_role_info(struct phl_info_t *phl,
                    struct rtw_phl_mcc_en_info *en_info,
                    u8 role_map,
                    struct rtw_wifi_role_t *cur_role,
                    struct rtw_wifi_role_link_t *cur_rlink
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_mcc_role *mrole = NULL;
	u8 ridx = 0, mcc_idx = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_fill_role_info()\n");

	if (RTW_PHL_STATUS_SUCCESS != _mcc_fill_ref_role_info(phl,
	                                                      en_info,
	                                                      cur_role,
	                                                      cur_rlink)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_role_info(): set ref role info failed\n");
		goto exit;
	}

	mcc_idx = en_info->mrole_num;
	role_map &= ~(BIT(cur_role->id));
	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (!(role_map & BIT(ridx)))
			continue;
		wrole = phl_get_wrole_by_ridx(phl, ridx);
		if (wrole == NULL) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_role_info(): get wrole fail, role_idx(%d)\n",
				ridx);
			goto exit;
		}
		if (mcc_idx >= MCC_ROLE_NUM) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_role_info(): mcc_idx(%d) >= MCC_ROLE_NUM(%d)\n",
				mcc_idx, MCC_ROLE_NUM);
			goto exit;
		}
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_role_info(): wrole(%d), wrole->type(%d), Fill mrole(%d) Info\n",
			ridx, wrole->type, mcc_idx);
		mrole = &en_info->mcc_role[mcc_idx];

		rlink = phl_get_rlink_by_hw_band(wrole, cur_rlink->hw_band);
		status = _mcc_fill_mcc_role_basic_info(phl, wrole, rlink, mrole);
		if (RTW_PHL_STATUS_SUCCESS != status) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_role_info(): set basic info failed\n");
			goto exit;
		}
		_mcc_fill_mcc_role_policy_info(phl, wrole, rlink, mrole);

		en_info->mrole_map |= BIT(mcc_idx);
		en_info->mrole_num++;
		mcc_idx ++;
	}
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< _mcc_fill_role_info(): status(%d), role_map(0x%x), mcc_role_map(0x%x)\n",
		status, role_map, en_info->mrole_map);
	return status;
}

void _mcc_fill_coex_mode(struct phl_info_t *phl, struct phl_mcc_info *minfo)
{
	/* if get from core or ....
	else*/
	minfo->coex_mode = RTW_PHL_MCC_COEX_MODE_BT_MASTER;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_coex_mode(): Set default mode(%d)\n",
		minfo->coex_mode);
}

void _mcc_fill_bt_dur(struct phl_info_t *phl, enum phl_band_idx band_idx,
			struct phl_mcc_info *minfo)
{
	minfo->bt_info.bt_dur = (u16)rtw_hal_get_btc_req_slot(phl->hal, band_idx);
	minfo->bt_info.bt_seg_num = 1;
	minfo->bt_info.bt_seg[0] = minfo->bt_info.bt_dur;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_bt_dur(): minfo->bt_info.bt_dur(%d)\n",
		minfo->bt_info.bt_dur);

}

/*
 * fill slot info
 * @m_pattern: pattern info
 * @bt_role: True: BT Role; False: Wifi Role
 * @dur: time slot
 * @mrole: mcc role info
 */
void _mcc_fill_slot_info(struct rtw_phl_mcc_pattern *m_pattern, bool bt_role,
				u16 dur, struct rtw_phl_mcc_role *mrole)
{
	if (m_pattern->slot_num >= SLOT_NUM) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_slot_info(): Fail, m_pattern->slot_num(%d) >= SLOT_NUM(%d)\n",
		m_pattern->slot_num, SLOT_NUM);
		return;
	}
	if (bt_role) {
		m_pattern->slot_order[m_pattern->slot_num].bt_role = true;
		m_pattern->slot_order[m_pattern->slot_num].slot = dur;
		m_pattern->slot_order[m_pattern->slot_num].mrole = NULL;
		m_pattern->bt_slot_num++;
	} else {
		m_pattern->slot_order[m_pattern->slot_num].bt_role = false;
		m_pattern->slot_order[m_pattern->slot_num].slot = dur;
		m_pattern->slot_order[m_pattern->slot_num].mrole = mrole;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "m_pattern->slot_num(): Fill slot, Idx(%d), dur(%d), is BT slot(%d)\n",
		m_pattern->slot_num, dur, bt_role);
	m_pattern->slot_num++;
}

void _mcc_reset_minfo(struct phl_info_t *phl, struct phl_mcc_info *minfo,
				enum _mcc_minfo_reset_type reset_type)
{
	void *priv = phl_to_drvpriv(phl);

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_reset_minfo\n");
	if (reset_type == MINFO_RESET_ALL) {
		_os_mem_set(priv, minfo, 0, sizeof(struct phl_mcc_info));
		return;
	}
	if (reset_type & MINFO_RESET_EN_INFO)
		_os_mem_set(priv, &minfo->en_info, 0,
				sizeof(struct rtw_phl_mcc_en_info));
	if (reset_type & MINFO_RESET_MODE)
		_os_mem_set(priv, &minfo->mcc_mode, 0,
				sizeof(enum rtw_phl_tdmra_wmode));
	if (reset_type & MINFO_RESET_ROLE_MAP)
		_os_mem_set(priv, &minfo->role_map, 0,
				sizeof(minfo->role_map));
	if (reset_type & MINFO_RESET_STATE)
		_os_mem_set(priv, &minfo->state, 0,
				sizeof(enum rtw_phl_mcc_state));
	if (reset_type & MINFO_RESET_COEX_MODE)
		_os_mem_set(priv, &minfo->coex_mode, 0,
				sizeof(enum rtw_phl_mcc_coex_mode));
	if (reset_type & MINFO_RESET_BT_INFO)
		_os_mem_set(priv, &minfo->bt_info, 0,
				sizeof(struct rtw_phl_mcc_bt_info));
	if (reset_type & MINFO_RESET_PATTERN_INFO)
		_os_mem_set(priv, &minfo->en_info.m_pattern, 0,
				sizeof(struct rtw_phl_mcc_pattern));
}

void _mcc_clean_noa(struct phl_info_t *phl, struct rtw_phl_mcc_en_info *en_info)
{
	struct phl_com_mcc_info *com_info = phl_to_com_mcc_info(phl);
	struct rtw_phl_mcc_noa param = {0};

	if (com_info == NULL) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_clean_noa(): Get mcc common info failed\n");
	} else if (com_info->ops.mcc_update_noa) {
		struct rtw_phl_mcc_role *ap_role = NULL;
		ap_role = _mcc_get_mrole_by_category(en_info, MCC_ROLE_AP_CAT);
		if (NULL == ap_role) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_clean_noa(): Get AP role fail\n");
			goto exit;
		}
		param.wrole = ap_role->wrole;
		param.rlink = ap_role->rlink;

		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_clean_noa()\n");
		com_info->ops.mcc_update_noa(com_info->ops.priv, &param);
	}
exit:
	return;
}

bool _tdmra_calc_noa_2wrole(struct phl_info_t *phl, struct phl_mcc_info *minfo,
				struct rtw_phl_mcc_noa *param)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_bt_info *bt = &minfo->bt_info;
	struct rtw_phl_mcc_role *role_ref = get_ref_role(en_info);
	struct rtw_phl_mcc_role *role_ano = (role_ref == &en_info->mcc_role[0])
				? &en_info->mcc_role[1] : &en_info->mcc_role[0];
	u16 d_r = role_ref->policy.dur_info.dur;
	u16 d_a = role_ano->policy.dur_info.dur;
	u64 mcc_start = 0, noa_start = 0;
	bool ret = false;

	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, ">>> _tdmra_calc_noa_2wrole()\n");
	mcc_start = en_info->tsf_high;
	mcc_start = mcc_start << 32;
	mcc_start |= en_info->tsf_low;
	if (rtw_phl_role_is_ap_category(role_ref->wrole)){
		/*calculate end time of GO*/
		noa_start = mcc_start + (d_r * TU);
		param->dur = en_info->mcc_intvl - d_r;
		param->wrole = role_ref->wrole;
		param->rlink = role_ref->rlink;
	} else {
		u32 tsf_ref_h = 0, tsf_ref_l = 0, tsf_ano_h = 0, tsf_ano_l = 0;
		u64 tsf_ref = 0, tsf_ano = 0;
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_tdmra_calc_noa_2wrole(): AP Role isn't ref role, we need to get 2 port tsf\n");
		_mcc_dump_bt_ino(bt);
		if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mcc_get_2ports_tsf(
				phl->hal, role_ref->group, role_ref->macid,
				role_ano->macid, &tsf_ref_h, &tsf_ref_l,
				&tsf_ano_h, &tsf_ano_l)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_tdmra_calc_noa_2wrole(): Get 2 port tsf failed\n");
			goto exit;
		}
		tsf_ref = tsf_ref_h;
		tsf_ref = tsf_ref << 32;
		tsf_ref |= tsf_ref_l;
		tsf_ano = tsf_ano_h;
		tsf_ano = tsf_ano << 32;
		tsf_ano |= tsf_ano_l;
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_tdmra_calc_noa_2wrole(): tsf_ref: 0x%08X %08x, tsf_ano: 0x%08x %08x\n",
			(u32)(tsf_ref >> 32), (u32)tsf_ref,
			(u32)(tsf_ano >> 32), (u32)tsf_ano);
		/*calculate end time of GO*/
		noa_start = mcc_start + (en_info->mcc_intvl * TU);
		if (bt->add_bt_role) {
			if(bt->bt_seg_num == 1) {
				noa_start -= (bt->bt_seg[0] * TU);
			} else if (bt->bt_seg_num == 2) {
				noa_start -= (bt->bt_seg[1] * TU);
			} else {
				PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_tdmra_calc_noa_2wrole(): error bt_seg_num(%d), please check code flow\n",
					bt->bt_seg_num);
				goto exit;
			}
		}
		noa_start = noa_start - tsf_ref + tsf_ano;
		param->dur = en_info->mcc_intvl - d_a;
		param->wrole = role_ano->wrole;
		param->rlink = role_ano->rlink;
	}
	param->start_t_h = noa_start >> 32;
	param->start_t_l = (u32)noa_start;
	param->cnt = 255;
	param->interval = en_info->mcc_intvl;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_tdmra_calc_noa_2wrole(): IsGORef(%d), mcc_start(0x%08x %08x)\n",
		rtw_phl_role_is_ap_category(role_ref->wrole),
		(u32)(mcc_start >> 32), (u32)mcc_start);
	ret = true;
exit:
	return ret;
}

bool _tdmra_calc_noa_1wrole(struct phl_info_t *phl, struct phl_mcc_info *minfo,
				struct rtw_phl_mcc_noa *param)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_role *role_ref = get_ref_role(en_info);
	u16 d_r = role_ref->policy.dur_info.dur;
	u64 mcc_start = 0, noa_start = 0;
	bool ret = false;

	mcc_start = en_info->tsf_high;
	mcc_start = mcc_start << 32;
	mcc_start |= en_info->tsf_low;
	/*calculate end time of GO*/
	noa_start = mcc_start + (d_r * TU);
	param->dur = en_info->mcc_intvl - d_r;
	param->wrole = role_ref->wrole;
	param->rlink = role_ref->rlink;
	param->start_t_h = noa_start >> 32;
	param->start_t_l = (u32)noa_start;
	param->cnt = 255;
	param->interval = en_info->mcc_intvl;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_tdmra_calc_noa_1wrole(): IsGORef(%d), mcc_start(0x%08x %08x)\n",
		rtw_phl_role_is_ap_category(role_ref->wrole),
		(u32)(mcc_start >> 32), (u32)mcc_start);
	ret = true;
	return ret;
}


void _mcc_up_noa(struct phl_info_t *phl, struct phl_mcc_info *minfo)

{
	struct phl_com_mcc_info *com_info = phl_to_com_mcc_info(phl);
	struct rtw_phl_mcc_noa param = {0};

	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, ">>> _mcc_up_noa()\n");
	if (com_info == NULL) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_up_noa(): Get mcc common info failed\n");
		goto exit;
	}
	if (!com_info->ops.mcc_update_noa)
		goto exit;
	if (RTW_PHL_TDMRA_AP_CLIENT_WMODE == minfo->mcc_mode) {
		if (false == _tdmra_calc_noa_2wrole(phl, minfo, &param))
			goto exit;
	} else if (RTW_PHL_TDMRA_AP_WMODE == minfo->mcc_mode) {
		if (false == _tdmra_calc_noa_1wrole(phl, minfo, &param))
			goto exit;
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_up_noa(): error wmode\n");
		_mcc_dump_mode(&minfo->mcc_mode);
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_up_noa(): NOA_start(0x%08x %08x), dur(%d), cnt(%d), interval(%d)\n",
		param.start_t_h, param.start_t_l, param.dur, param.cnt,
		param.interval);
	com_info->ops.mcc_update_noa(com_info->ops.priv, &param);
exit:
	return;
}

bool _mcc_adjust_dur_for_2g_mcc_2role_bt(struct phl_mcc_info *minfo)
{
	struct rtw_phl_mcc_bt_info *bt_info = &minfo->bt_info;
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_role *m_role1 = &en_info->mcc_role[0];
	struct rtw_phl_mcc_role *m_role2 = &en_info->mcc_role[1];
	u16 d1 = m_role1->policy.dur_info.dur;
	u16 d2 = m_role2->policy.dur_info.dur;
	u16 d1_max = (m_role1->policy.dur_info.dur_limit.enable) ?
		m_role1->policy.dur_info.dur_limit.max_dur : en_info->mcc_intvl;
	u16 d2_max = (m_role2->policy.dur_info.dur_limit.enable) ?
		m_role2->policy.dur_info.dur_limit.max_dur : en_info->mcc_intvl;
	u16 d1_min = rtw_phl_role_is_ap_category(m_role1->wrole) ?
					MIN_AP_DUR : MIN_CLIENT_DUR;
	u16 d2_min = rtw_phl_role_is_ap_category(m_role2->wrole) ?
					MIN_AP_DUR : MIN_CLIENT_DUR;
	u16 wifi_dur = 0, bt_dur = bt_info->bt_dur;
	u16 i = 0;
	bool adjust_ok = false;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_adjust_dur_for_2g_mcc_2role_bt(): mcc_intvl(%d), bt_dur(%d), d1(%d), d2(%d), d1_min(%d), d1_max(%d), d2_min(%d), d2_max(%d)\n",
		en_info->mcc_intvl, bt_dur, d1, d2, d1_min, d1_max, d2_min, d2_max);
	for (i = 0; i < en_info->mcc_intvl; i++) {
		wifi_dur = en_info->mcc_intvl - bt_dur;
		d1 = ((d1 * 100 / (d1 + d2)) * wifi_dur) / 100;
		if (d1 < d1_min) {
			d1 = d1_min;
		} else if (d1 > d1_max) {
			d1 = d1_max;
		}
		d2 = wifi_dur - d1;
		if (d2 < d2_min) {
			d2 = d2_min;
			d1 = wifi_dur - d2;
		} else if (d2 > d2_max) {
			d2 = d2_max;
			d1 = wifi_dur - d2;
		}
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_adjust_dur_for_2g_mcc_2role_bt(): Loop bt_dur(%d), d1(%d), d2(%d)\n",
				bt_dur, d1, d2);
		if ((d1 >= d1_min) && (d1 <= d1_max) &&
			(d2 >= d2_min) && (d2 <= d2_max)) {
			m_role1->policy.dur_info.dur = d1;
			m_role2->policy.dur_info.dur = d2;
			bt_info->bt_dur = bt_dur;
			en_info->m_pattern.d_r_d_a_spacing_max = bt_info->bt_dur;
			adjust_ok = true;
			break;
		}
		bt_dur--;
	}
	if (adjust_ok == false){
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_adjust_dur_for_2g_mcc_2role_bt(): Adjust fail\n");
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_adjust_dur_for_2g_mcc_2role_bt(): Adjust ok, d1(%d), d2(%d), bt dur(%d), d_r_d_a_spacing_max(%d)\n",
			m_role1->policy.dur_info.dur,
			m_role2->policy.dur_info.dur,
			bt_info->bt_dur, en_info->m_pattern.d_r_d_a_spacing_max);
	}
	return adjust_ok;
}

void _mcc_adjust_dur_for_2_band_mcc_2role_bt(struct phl_mcc_info *minfo,
	struct rtw_phl_mcc_role *role_2g, struct rtw_phl_mcc_role *role_non_2g)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	enum rtw_phl_mcc_coex_mode *coex_mode = &minfo->coex_mode;
	struct rtw_phl_mcc_policy_info *plcy_2g = &role_2g->policy;
	struct rtw_phl_mcc_policy_info *plcy_non2g = &role_non_2g->policy;
	u16 *bt_dur = &minfo->bt_info.bt_dur;
	u16 dur_2g = 0, dur_non2g = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_adjust_dur_for_2_band_mcc_2role_bt(): coex_mode(%d) 2G_Dur(%d), 5G_Dur(%d), BT_Dur(%d)\n",
		*coex_mode, plcy_2g->dur_info.dur, plcy_non2g->dur_info.dur,
		*bt_dur);
	if (plcy_non2g->dur_info.dur >= *bt_dur) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_adjust_dur_for_2_band_mcc_2role_bt(): 5G_Dur(%d) >= BT_Dur(%d), no need to adjust 5G duration for BT\n",
			plcy_non2g->dur_info.dur, *bt_dur);
		goto exit;
	}
	if (plcy_non2g->dur_info.dur_limit.enable &&
		plcy_non2g->dur_info.dur_limit.max_dur < *bt_dur) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_adjust_dur_for_2_band_mcc_2role_bt(): dur_limit.max_dur(%d) < bt_dur(%d), We can't adjust 5G duration(%d) for BT\n",
			plcy_non2g->dur_info.dur_limit.max_dur,
			*bt_dur, plcy_non2g->dur_info.dur);
		goto exit;
	}
	if (*coex_mode == RTW_PHL_MCC_COEX_MODE_BT_MASTER) {
		dur_non2g = *bt_dur;
		dur_2g = en_info->mcc_intvl - dur_non2g;
		if (plcy_2g->dur_info.dur_limit.enable &&
			plcy_2g->dur_info.dur_limit.max_dur < dur_2g) {
			dur_2g = plcy_2g->dur_info.dur_limit.max_dur;
			dur_non2g = en_info->mcc_intvl - dur_2g;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_adjust_dur_for_2_band_mcc_2role_bt(): plcy_2g->max_dur(%d) < dur_2g(%d), We can adjust some 5G duration for BT\n",
				plcy_2g->dur_info.dur_limit.max_dur, dur_2g);
		}
		plcy_non2g->dur_info.dur = dur_non2g;
		plcy_2g->dur_info.dur = dur_2g;
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_adjust_dur_for_2_band_mcc_2role_bt(): coex_mode == RTW_PHL_MCC_COEX_MODE_BT_MASTER, we adjust 5G duration for BT\n");
	} else if (*coex_mode == RTW_PHL_MCC_COEX_MODE_WIFI_MASTER) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_adjust_dur_for_2_band_mcc_2role_bt(): coex_mode == RTW_PHL_MCC_COEX_MODE_WIFI_MASTER, we don't adjust 5G duration for BT\n");
		goto exit;
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_adjust_dur_for_2_band_mcc_2role_bt(): coex_mode(%d), Undefined mode, ignore bt duration\n",
			*coex_mode);
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_adjust_dur_for_2_band_mcc_2role_bt(): After adjust, 2G_Dur(%d), 5G_Dur(%d), BT_Dur(%d)\n",
		plcy_2g->dur_info.dur, plcy_non2g->dur_info.dur,
		*bt_dur);
exit:
	return;
}

bool _mcc_need_to_seg_bt_dur(struct phl_mcc_info *minfo)
{
/* Not ready for implementation*/
	return false;
#if 0
	bool seg = false;
	struct rtw_phl_mcc_en_info *info = &minfo->en_info;
	struct rtw_phl_mcc_dur_lim_info *limit_i = NULL;
	u8 i = 0;

	if (minfo->mcc_mode != RTW_PHL_TDMRA_2CLIENTS_WMODE)
		goto exit;
	if (minfo->bt_info.bt_dur < BT_DUR_SEG_TH || BT_SEG_NUM < 2)
		goto exit;
	for (i = 0; i < MCC_ROLE_NUM; i++) {
		limit_i = &minfo->en_info.mcc_role[i].policy.dur_info.dur_limit;
		if (limit_i->enable) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_need_to_seg_bt_dur(): Can't seg bt slot when wifi slot with limitation\n");
			goto exit;
		}
	}
	seg = true;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_need_to_seg_bt_dur(): seg(%d)\n",
		seg);
	return seg;
#endif
}

/*
 * 2 wifi slot req + bt slot req
 */
void _mcc_discision_dur_for_2g_mcc_2role_bt(struct phl_mcc_info *minfo)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_role *m_role1 = &en_info->mcc_role[0];
	struct rtw_phl_mcc_role *m_role2 = &en_info->mcc_role[1];
	u16 d1 = 0, d2 = 0;

	/* Segment bt slot, don't support in current code*/
	if (_mcc_need_to_seg_bt_dur(minfo)) {
		/*2 wifi slot + 2bt slot*/
		d1 = m_role1->policy.dur_info.dur;
		d2 = m_role2->policy.dur_info.dur;
		en_info->mcc_intvl = WORSECASE_INTVL;
		_mcc_adjust_dur_for_2g_mcc_2role_bt(minfo);
		minfo->bt_info.bt_seg_num = 2;
		minfo->bt_info.bt_seg[0] = minfo->bt_info.bt_dur / 2;
		minfo->bt_info.bt_seg[1] = minfo->bt_info.bt_dur -
						minfo->bt_info.bt_seg[0];
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_discision_dur_for_2g_mcc_2role_bt(): Change D1(%d), D2(%d) to D1(%d), D2(%d), bt_seg[0](%d), bt_seg[1](%d)\n",
			d1, d2, m_role1->policy.dur_info.dur,
			m_role2->policy.dur_info.dur,
			minfo->bt_info.bt_seg[0], minfo->bt_info.bt_seg[1]);
	} else {
	/*2 wifi slot + 1bt slot*/
		if (minfo->bt_info.bt_dur > BT_DUR_MAX_2WS) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_discision_dur_for_2g_mcc_2role_bt(): bt req slot(%d) > BT_DUR_MAX_2WS(%d)\n",
				minfo->bt_info.bt_dur, BT_DUR_MAX_2WS);
			minfo->bt_info.bt_dur = BT_DUR_MAX_2WS;
			minfo->bt_info.bt_seg[0] = BT_DUR_MAX_2WS;
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_discision_dur_for_2g_mcc_2role_bt(): set bt rq slot to (%d)\n",
				minfo->bt_info.bt_dur);
		}
		_mcc_adjust_dur_for_2g_mcc_2role_bt(minfo);
	}
}

bool _mcc_discision_duration_for_2role_bt_v2(struct phl_mcc_info *minfo)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_bt_info *bt_info = &minfo->bt_info;
	struct rtw_phl_mcc_role *m_role1 = &en_info->mcc_role[0];
	struct rtw_phl_mcc_role *m_role2 = &en_info->mcc_role[1];
	bool add_extra_bt_role = false;

	if (bt_info->bt_dur == 0)
		goto exit;
	if (m_role1->chandef->band == BAND_ON_24G &&
		m_role2->chandef->band == BAND_ON_24G) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_discision_duration_for_2role_bt_v2(): Not support , We will ignore Bt slot\n");
#if 0
		if (_mcc_adjust_dur_for_2g_mcc_2role_bt(minfo)) {
			add_extra_bt_role = true;
		} else {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_discision_duration_for_2role_bt(): Adjust dur fail, We will ignore Bt slot\n");
		}
#endif
		goto exit;
	}
	if (m_role1->chandef->band != BAND_ON_24G &&
		m_role2->chandef->band != BAND_ON_24G) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_discision_duration_for_2role_bt_v2(): All 5G, Don't care BT duration\n");
		goto exit;
	}
	if (m_role1->chandef->band == BAND_ON_24G)
		_mcc_adjust_dur_for_2_band_mcc_2role_bt(minfo, m_role1, m_role2);
	else
		_mcc_adjust_dur_for_2_band_mcc_2role_bt(minfo, m_role2, m_role1);
exit:
	return add_extra_bt_role;
}

bool _mcc_discision_duration_for_2role_bt(struct phl_mcc_info *minfo)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_role *m_role1 = &en_info->mcc_role[0];
	struct rtw_phl_mcc_role *m_role2 = &en_info->mcc_role[1];
	bool add_extra_bt_role = false;

	if (minfo->bt_info.bt_dur == 0)
		goto exit;
	if (m_role1->chandef->band == BAND_ON_24G &&
		m_role2->chandef->band == BAND_ON_24G) {
		_mcc_discision_dur_for_2g_mcc_2role_bt(minfo);
		add_extra_bt_role = true;
		goto exit;
	}
	if (m_role1->chandef->band != BAND_ON_24G &&
		m_role2->chandef->band != BAND_ON_24G) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_discision_duration_for_2role_bt(): All 5G, Don't care BT duration\n");
		goto exit;
	}
	if (m_role1->chandef->band == BAND_ON_24G)
		_mcc_adjust_dur_for_2_band_mcc_2role_bt(minfo, m_role1, m_role2);
	else
		_mcc_adjust_dur_for_2_band_mcc_2role_bt(minfo, m_role2, m_role1);

exit:
	return add_extra_bt_role;
}

enum rtw_phl_status
_mcc_calculate_start_tsf(struct phl_info_t *phl,
                         struct rtw_phl_mcc_en_info *en_info)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_mcc_role *ref_role = get_ref_role(en_info);
	struct mr_coex_info *mrcx_i = get_mr_coex_i(phl, ref_role->rlink->hw_band);
	u32 tsf_h = 0, tsf_l = 0;
	u64 tsf = 0, start_tsf = 0;
	u8 i = 0, max_loop = 10, calc_done = 0;
	u16 offset = 0;
	u64 min_trig_t = LONG_TRIGGER_MCC_TIME;

	if (MR_COEX_TRIG_BY_LINKING == mrcx_i->coex_trig) {
		if (rtw_phl_role_is_ap_category(ref_role->wrole))
			min_trig_t = SHORT_TRIGGER_MCC_TIME;
	} else if (MR_COEX_TRIG_BY_BT == mrcx_i->coex_trig) {
		min_trig_t = SHORT_TRIGGER_MCC_TIME;
	}
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_get_tsf(phl->hal,
	                                              ref_role->rlink->hw_band,
	                                              ref_role->rlink->hw_port,
	                                              &tsf_h,
	                                              &tsf_l)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_start_tsf(): Get tsf fail\n");
		goto exit;
	}

	tsf = tsf_h;
	tsf = tsf << 32;
	tsf |= tsf_l;
	/*calculate the value between current TSF and TBTT*/
	phl_calc_offset_from_tbtt(phl, ref_role->wrole, ref_role->rlink, tsf, &offset);

	start_tsf = tsf + (ref_role->bcn_intvl * TU);
	start_tsf = start_tsf - ((offset + en_info->m_pattern.tob_r) * TU);
	for (i = 0; i < max_loop; i++) {
		if (start_tsf < (tsf + (min_trig_t * TU))) {
			start_tsf += (ref_role->bcn_intvl * TU);
		} else {
			calc_done = 1;
			break;
		}
	}
	if (!calc_done) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calculate_start_tsf(): Calcculate start tsf fail, please check code flow\n");
		goto exit;
	}
	en_info->tsf_high = start_tsf >> 32;
	en_info->tsf_low = (u32)start_tsf;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_start_tsf(): start_tsf(0x%08x %08x), cur_tsf(0x%08x %08x), ref_role->bcn_intvl(%d), ref_role->duration(%d)\n",
		(u32)(start_tsf >> 32), (u32)start_tsf, (u32)(tsf >> 32),
		(u32)tsf, ref_role->bcn_intvl, ref_role->policy.dur_info.dur);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

void _mcc_set_2_clients_worsecase_default_pattern(struct rtw_phl_mcc_pattern *m_pattern,
					u16 dur_ref)
{
	m_pattern->toa_r = CLIENTS_WORSECASE_REF_TOA;
	m_pattern->tob_r = dur_ref - m_pattern->toa_r;
	_mcc_fill_slot_info(m_pattern, false,
				m_pattern->role_ref->policy.dur_info.dur,
				m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, false,
				m_pattern->role_ano->policy.dur_info.dur,
				m_pattern->role_ano);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_set_2_clients_worsecase_default_pattern(): tob_r(%d), toa_r(%d)\n",
		m_pattern->tob_r, m_pattern->toa_r);
}

/*
 * get bcn offset for 2 wifi slot and 1 bt slot for pattern1
 * Pattern:
 * |      Dur_r     |     Dur_a      | Bt slot |
 *          bcn                bcn
 * | tob_r | toa_r|tob_a | toa_a| Bt slot |
 */
s16 _mcc_get_offset_for_2_wslot_1_btslot_p1(s16 dur_r,
				s16 tob_r, s16 tob_a)
{
	return dur_r - tob_r + tob_a;
}

/*
 * get bcn offset for 2 wifi slot and 1 bt slot for pattern2
 * Pattern:
 * |      Dur_r     | Bt slot |     Dur_a      |
 *          bcn                            bcn
 * | tob_r | toa_r| Bt slot |tob_a | toa_a|
 */
s16 _mcc_get_offset_for_2_wslot_1_btslot_p2(s16 dur_r, s16 bt_dur,
				s16 tob_r, s16 tob_a)
{
	return dur_r - tob_r + bt_dur + tob_a;
}

void _mcc_get_offset_range_for_2wslot_1btslot_p1(s16 ref_dur, s16 ano_dur,
				s16 *bcn_min, s16 *bcn_max)
{
	s16 min1 = 0, max1 = 0;

	*bcn_min = _mcc_get_offset_for_2_wslot_1_btslot_p1(ref_dur, EARLY_RX_BCN_T,
							EARLY_RX_BCN_T);
	*bcn_max = _mcc_get_offset_for_2_wslot_1_btslot_p1(ref_dur, EARLY_RX_BCN_T,
							ano_dur - MIN_RX_BCN_T);
	min1 = _mcc_get_offset_for_2_wslot_1_btslot_p1(ref_dur,
					ref_dur - MIN_RX_BCN_T, EARLY_RX_BCN_T);
	max1 = _mcc_get_offset_for_2_wslot_1_btslot_p1(ref_dur,
				ref_dur - MIN_RX_BCN_T, ano_dur - MIN_RX_BCN_T);
	if (min1 < *bcn_min)
		*bcn_min = min1;
	if (max1 > *bcn_max)
		*bcn_max = max1;
	PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_get_offset_range_for_2wslot_1btslot_p1(): min(%d), max(%d)\n",
		*bcn_min, *bcn_max);
}

void _mcc_get_offset_range_for_2wslot_1btslot_p2(s16 ref_dur, s16 ano_dur,
				s16 bt_dur, s16 *bcn_min, s16 *bcn_max)
{
	s16 min1 = 0, max1 = 0;

	*bcn_min = _mcc_get_offset_for_2_wslot_1_btslot_p2(ref_dur, bt_dur,
						EARLY_RX_BCN_T, EARLY_RX_BCN_T);
	*bcn_max = _mcc_get_offset_for_2_wslot_1_btslot_p2(ref_dur, bt_dur,
					EARLY_RX_BCN_T, ano_dur - MIN_RX_BCN_T);
	min1 = _mcc_get_offset_for_2_wslot_1_btslot_p2(ref_dur, bt_dur,
					ref_dur - MIN_RX_BCN_T, EARLY_RX_BCN_T);

	min1 = _mcc_get_offset_for_2_wslot_1_btslot_p2(ref_dur, bt_dur,
				ref_dur - MIN_RX_BCN_T, ano_dur - MIN_RX_BCN_T);
	if (min1 < *bcn_min)
		*bcn_min = min1;
	if (max1 > *bcn_max)
		*bcn_max = max1;
	PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_get_offset_range_for_2wslot_1btslot_p2(): min(%d), max(%d)\n",
		*bcn_min, *bcn_max);
}

s16 _mcc_get_offset_for_2_clients_worsecase(s16 ref_dur, s16 ano_dur,
				u16 ref_bcn_intvl, s16 toa_ref, s16 tob_ano)
{
	return toa_ref + tob_ano + ref_dur + ano_dur - (2 * ref_bcn_intvl);
}

void _mcc_get_offset_range_for_2_clients_worsecase(s16 ref_dur, s16 ano_dur,
				u16 ref_bcn_intvl, s16 *bcn_min, s16 *bcn_max)
{
	s16 min1 = 0, min2 = 0, max1 = 0, max2 = 0;
	min1 = _mcc_get_offset_for_2_clients_worsecase(ref_dur, ano_dur,
				ref_bcn_intvl, MIN_RX_BCN_T, EARLY_RX_BCN_T);
	max1 = _mcc_get_offset_for_2_clients_worsecase(ref_dur, ano_dur,
				ref_bcn_intvl, MIN_RX_BCN_T, ano_dur -
				MIN_RX_BCN_T);
	min2 = _mcc_get_offset_for_2_clients_worsecase(ref_dur, ano_dur,
				ref_bcn_intvl, ref_dur - EARLY_RX_BCN_T,
				EARLY_RX_BCN_T);
	max2 = _mcc_get_offset_for_2_clients_worsecase(ref_dur, ano_dur,
				ref_bcn_intvl, ref_dur -EARLY_RX_BCN_T,
				ano_dur - MIN_RX_BCN_T);
	if (min1 < min2)
		*bcn_min = min1;
	else
		*bcn_min = min2;
	if (max1 > max2)
		*bcn_max = max1;
	else
		*bcn_max = max2;
}

/*
 * copy from _mcc_calc_2wslot_1btslot_nego_p1
 * | Wifi slot1 | Bt slot | Wifi slot2 |
 */
bool _mcc_calc_2wslot_1btslot_nego_p2(struct rtw_phl_mcc_dur_info *ref_dur,
			struct rtw_phl_mcc_dur_info *ano_dur, s16 bt_dur,
			s16 offset, struct rtw_phl_mcc_pattern *m_pattern)
{
	bool cal_ok = false;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0;

	if ((ref_dur->dur_limit.enable) && (ano_dur->dur_limit.enable)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2wslot_1btslot_nego_p2(): not support, both enable slot limitation\n");
		/*todo*/
		goto exit;
	}
	if (ref_dur->dur_limit.enable) {
		tob_r = ref_dur->dur / 2;
		toa_r = ref_dur->dur - tob_r;
		if (tob_r > ref_dur->dur_limit.max_tob) {
			tob_r = ref_dur->dur_limit.max_tob;
			toa_r = ref_dur->dur - tob_r;
		}
		if (toa_r > ref_dur->dur_limit.max_toa) {
			toa_r = ref_dur->dur_limit.max_toa;
			tob_r = ref_dur->dur - toa_r;
		}
		if ((tob_r > ref_dur->dur_limit.max_tob) ||
			(toa_r > ref_dur->dur_limit.max_toa)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2wslot_1btslot_nego_p2(): After adjust tob_r(%d) > max_tob(%d) or toa_r(%d) > max_toa(%d)\n",
				tob_r, ref_dur->dur_limit.max_tob,
				toa_r, ref_dur->dur_limit.max_toa);
			goto exit;
		}
		tob_a = offset - toa_r - bt_dur;
		toa_a = ano_dur->dur - tob_a;
		if (tob_a <= 0 || toa_a <= 0) {
			m_pattern->courtesy_i.c_en= true;
			m_pattern->courtesy_i.c_num = 3;
			m_pattern->courtesy_i.src_role = m_pattern->role_ref;
			m_pattern->courtesy_i.tgt_role = m_pattern->role_ano;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2wslot_1btslot_nego_p2(): Limi by ref dur, courtesy_info: c_en(%d), c_num(%d), src_role->macid(0x%x), tgt_role->macid(0x%x)\n",
				m_pattern->courtesy_i.c_en,
				m_pattern->courtesy_i.c_num,
				m_pattern->courtesy_i.src_role->macid,
				m_pattern->courtesy_i.tgt_role->macid);
		}
		goto fill_pattern;
	} else if (ano_dur->dur_limit.enable) {
		tob_a = ano_dur->dur / 2;
		toa_a = ano_dur->dur - tob_a;
		if (tob_a > ano_dur->dur_limit.max_tob) {
			tob_a = ano_dur->dur_limit.max_tob;
			toa_a = ano_dur->dur - tob_a;
		}
		if (toa_a > ano_dur->dur_limit.max_toa) {
			toa_a = ano_dur->dur_limit.max_toa;
			tob_a = ano_dur->dur - toa_a;
		}
		if ((tob_a > ano_dur->dur_limit.max_tob) ||
			(toa_a > ano_dur->dur_limit.max_toa)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2wslot_1btslot_nego_p2(): After adjust tob_r(%d) > max_tob(%d) or toa_r(%d) > max_toa(%d)\n",
				tob_a, ano_dur->dur_limit.max_tob,
				toa_a, ano_dur->dur_limit.max_toa);
			goto exit;
		}
		toa_r = offset - tob_a - bt_dur;
		tob_r = ref_dur->dur - toa_r;
		if (toa_r <= 0 || tob_r <= 0) {
			m_pattern->courtesy_i.c_en= true;
			m_pattern->courtesy_i.c_num = 3;
			m_pattern->courtesy_i.src_role = m_pattern->role_ano;
			m_pattern->courtesy_i.tgt_role = m_pattern->role_ref;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2wslot_1btslot_nego_p2(): Limi by ano dur, courtesy_info: c_en(%d), c_num(%d), src_role->macid(0x%x), tgt_role->macid(0x%x)\n",
				m_pattern->courtesy_i.c_en,
				m_pattern->courtesy_i.c_num,
				m_pattern->courtesy_i.src_role->macid,
				m_pattern->courtesy_i.tgt_role->macid);
		}
		goto fill_pattern;
	} else {
		tob_r = ref_dur->dur / 2;
		toa_r = ref_dur->dur - tob_r;
		tob_a = offset - toa_r - bt_dur;
		toa_a = ano_dur->dur - tob_a;
		if (tob_a <= 0 || toa_a <= 0) {
			m_pattern->courtesy_i.c_en= true;
			m_pattern->courtesy_i.c_num = 3;
			m_pattern->courtesy_i.src_role = m_pattern->role_ref;
			m_pattern->courtesy_i.tgt_role = m_pattern->role_ano;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2wslot_1btslot_nego_p2(): Limi by ref dur, courtesy_info: c_en(%d), c_num(%d), src_role->macid(0x%x), tgt_role->macid(0x%x)\n",
				m_pattern->courtesy_i.c_en,
				m_pattern->courtesy_i.c_num,
				m_pattern->courtesy_i.src_role->macid,
				m_pattern->courtesy_i.tgt_role->macid);
		}
		goto fill_pattern;
	}
fill_pattern:
	cal_ok = true;
	m_pattern->tob_r = tob_r;
	m_pattern->toa_r = toa_r;
	m_pattern->tob_a = tob_a;
	m_pattern->toa_a = toa_a;
	_mcc_fill_slot_info(m_pattern, false, ref_dur->dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, true, bt_dur, NULL);
	_mcc_fill_slot_info(m_pattern, false, ano_dur->dur, m_pattern->role_ano);
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2wslot_1btslot_nego_p2(): calc nego patter ok(%d)\n",
		cal_ok);
	return cal_ok;
}

/*
 * copy from _mcc_calc_2_wrole_nego_pattern
 * |   Wifi slot1   |   Wifi slot2   | Bt slot |
 * |      Dur_r     |     Dur_a      | Bt slot |
 *          bcn                bcn
 * | tob_r | toa_r|tob_a | toa_a| Bt slot |
 */
bool _mcc_calc_2wslot_1btslot_nego_p1(struct rtw_phl_mcc_dur_info *ref_dur,
			struct rtw_phl_mcc_dur_info *ano_dur, s16 offset,
			s16 bt_dur, struct rtw_phl_mcc_pattern *m_pattern)
{
	bool cal_ok = false;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0;

	if ((ref_dur->dur_limit.enable) && (ano_dur->dur_limit.enable)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2wslot_1btslot_nego_p1(): not support, both enable slot limitation\n");
		/*todo*/
		goto exit;
	}
	if (ref_dur->dur_limit.enable) {
		tob_r = ref_dur->dur / 2;
		toa_r = ref_dur->dur - tob_r;
		if (tob_r > ref_dur->dur_limit.max_tob) {
			tob_r = ref_dur->dur_limit.max_tob;
			toa_r = ref_dur->dur - tob_r;
		}
		if (toa_r > ref_dur->dur_limit.max_toa) {
			toa_r = ref_dur->dur_limit.max_toa;
			tob_r = ref_dur->dur - toa_r;
		}
		if ((tob_r > ref_dur->dur_limit.max_tob) ||
			(toa_r > ref_dur->dur_limit.max_toa)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2wslot_1btslot_nego_p1(): After adjust tob_r(%d) > max_tob(%d) or toa_r(%d) > max_toa(%d)\n",
				tob_r, ref_dur->dur_limit.max_tob,
				toa_r, ref_dur->dur_limit.max_toa);
			goto exit;
		}
		tob_a = offset - toa_r;
		toa_a = ano_dur->dur - tob_a;
		if (tob_a <= 0 || toa_a <= 0) {
			m_pattern->courtesy_i.c_en= true;
			m_pattern->courtesy_i.c_num = 3;
			m_pattern->courtesy_i.src_role = m_pattern->role_ref;
			m_pattern->courtesy_i.tgt_role = m_pattern->role_ano;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2wslot_1btslot_nego_p1(): Limi by ref dur, courtesy_info: c_en(%d), c_num(%d), src_role->macid(0x%x), tgt_role->macid(0x%x)\n",
				m_pattern->courtesy_i.c_en,
				m_pattern->courtesy_i.c_num,
				m_pattern->courtesy_i.src_role->macid,
				m_pattern->courtesy_i.tgt_role->macid);
		}
		goto fill_pattern;
	} else if (ano_dur->dur_limit.enable) {
		tob_a = ano_dur->dur / 2;
		toa_a = ano_dur->dur - tob_a;
		if (tob_a > ano_dur->dur_limit.max_tob) {
			tob_a = ano_dur->dur_limit.max_tob;
			toa_a = ano_dur->dur - tob_a;
		}
		if (toa_a > ano_dur->dur_limit.max_toa) {
			toa_a = ano_dur->dur_limit.max_toa;
			tob_a = ano_dur->dur - toa_a;
		}
		if ((tob_a > ano_dur->dur_limit.max_tob) ||
			(toa_a > ano_dur->dur_limit.max_toa)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2wslot_1btslot_nego_p1(): After adjust tob_r(%d) > max_tob(%d) or toa_r(%d) > max_toa(%d)\n",
				tob_a, ano_dur->dur_limit.max_tob,
				toa_a, ano_dur->dur_limit.max_toa);
			goto exit;
		}
		toa_r = offset - tob_a;
		tob_r = ref_dur->dur - toa_r;
		if (toa_r <= 0 || tob_r <= 0) {
			m_pattern->courtesy_i.c_en= true;
			m_pattern->courtesy_i.c_num = 3;
			m_pattern->courtesy_i.src_role = m_pattern->role_ano;
			m_pattern->courtesy_i.tgt_role = m_pattern->role_ref;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2wslot_1btslot_nego_p1(): Limi by ano dur, courtesy_info: c_en(%d), c_num(%d), src_role->macid(0x%x), tgt_role->macid(0x%x)\n",
				m_pattern->courtesy_i.c_en,
				m_pattern->courtesy_i.c_num,
				m_pattern->courtesy_i.src_role->macid,
				m_pattern->courtesy_i.tgt_role->macid);
		}
		goto fill_pattern;
	} else {
		tob_r = ref_dur->dur / 2;
		toa_r = ref_dur->dur - tob_r;
		tob_a = offset - toa_r;
		toa_a = ano_dur->dur - tob_a;
		if (tob_a <= 0 || toa_a <= 0) {
			m_pattern->courtesy_i.c_en= true;
			m_pattern->courtesy_i.c_num = 3;
			m_pattern->courtesy_i.src_role = m_pattern->role_ref;
			m_pattern->courtesy_i.tgt_role = m_pattern->role_ano;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2wslot_1btslot_nego_p1(): courtesy_info: c_en(%d), c_num(%d), src_role->macid(0x%x), tgt_role->macid(0x%x)\n",
				m_pattern->courtesy_i.c_en,
				m_pattern->courtesy_i.c_num,
				m_pattern->courtesy_i.src_role->macid,
				m_pattern->courtesy_i.tgt_role->macid);
		}
		goto fill_pattern;
	}
fill_pattern:
	cal_ok = true;
	m_pattern->tob_r = tob_r;
	m_pattern->toa_r = toa_r;
	m_pattern->tob_a = tob_a;
	m_pattern->toa_a = toa_a;
	_mcc_fill_slot_info(m_pattern, false, ref_dur->dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, false, ano_dur->dur, m_pattern->role_ano);
	_mcc_fill_slot_info(m_pattern, true, bt_dur, NULL);
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2wslot_1btslot_nego_p1(): calc nego patter ok(%d)\n",
		cal_ok);
	return cal_ok;
}

/*
 * copy from _mcc_calculate_2_clients_pattern
 * Calculate pattern2 for 2 wifi slot and 1 bt slot
 * Pattern:
 * |   Wifi slot1   | Bt slot |   Wifi slot2   |
 * |      Dur_r     | Bt slot |     Dur_a      |
 *          bcn                            bcn
 * | tob_r | toa_r| Bt slot |tob_a | toa_a|
 * @offset: The offset of Bcns
 * @m_pattern: pattern info
 */
enum rtw_phl_status _mcc_calculate_2wslot_1btslot_pattern2(
				struct rtw_phl_mcc_dur_info *ref_dur,
				struct rtw_phl_mcc_dur_info *ano_dur,
				u16 offset, s16 bt_dur,
				struct rtw_phl_mcc_pattern *m_pattern)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	u16 mcc_intvl = ref_dur->dur + ano_dur->dur;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0, tob_r_cand = 0;
	s16 d_r = ref_dur->dur, d_a = ano_dur->dur, bcns_offset = offset;
	s16 sum = 0, sum_last = 0;
	s16 tob_r_l = ref_dur->dur_limit.max_tob;
	s16 toa_r_l = ref_dur->dur_limit.max_toa;
	s16 tob_a_l = ano_dur->dur_limit.max_tob;
	s16 toa_a_l = ano_dur->dur_limit.max_toa;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern2(): ref_dur(%d), ano_dur(%d), bt_dur(%d), bcns offset(%d)\n",
		d_r, d_a, bt_dur, bcns_offset);
	for (tob_r = EARLY_RX_BCN_T; tob_r < mcc_intvl; tob_r++) {
		toa_r = d_r - tob_r;
		if (toa_r < MIN_RX_BCN_T) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern2(): break loop, by toa_r(%d) < MIN_RX_BCN_T(%d)\n",
				toa_r, MIN_RX_BCN_T);
			break;
		}
		if (ref_dur->dur_limit.enable) {
			if (tob_r > tob_r_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern2(): tob_r(%d) > tob_r_l(%d), Break loop\n",
					tob_r, tob_r_l);
				break;
			} else if (toa_r > toa_r_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern2(): toa_r(%d) > toa_r_l(%d), continue next loop\n",
					toa_r, toa_r_l);
				continue;
			}
		}
		tob_a = bcns_offset - toa_r - bt_dur;
		if (tob_a < EARLY_RX_BCN_T)
			continue;
		toa_a = d_a - tob_a;
		if (toa_a < MIN_RX_BCN_T){
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern2(): break loop, by toa_a(%d) < MIN_RX_BCN_T(%d)\n",
				toa_a, MIN_RX_BCN_T);
			break;
		}
		if (ano_dur->dur_limit.enable) {
			if (tob_a > tob_a_l || toa_a > toa_a_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern2(): tob_a(%d) > tob_a_l(%d) || toa_a(%d) > toa_a_l(%d), continue next loop\n",
					tob_a, tob_a_l, toa_a, toa_a_l);
				continue;
			}
		}
		sum = ((tob_r - toa_r) * (tob_r - toa_r)) +
			((tob_r - tob_a) * (tob_r - tob_a)) +
			((tob_r - toa_a) * (tob_r - toa_a)) +
			((toa_r - tob_a) * (toa_r - tob_a)) +
			((toa_r - toa_a) * (toa_r - toa_a)) +
			((tob_a - toa_a) * (tob_a - toa_a));
		tob_r_cand = tob_r;
		if (sum_last !=0 && sum > sum_last) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern2(): Find the optimal pattern, by get minSum\n");
			break;
		}
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern2(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d), sum_last(%d), sum(%d)\n",
			tob_r, toa_r, tob_a, toa_a, sum_last, sum);
		sum_last = sum;
	}
	if (0 == tob_r_cand) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2wslot_1btslot_pattern2(): Can't found suitable pattern, goto calc_nego_pattern\n");
		goto exit;
	}
	tob_r = tob_r_cand;
	toa_r = d_r - tob_r;
	tob_a = bcns_offset - toa_r;
	toa_a = d_a - tob_a;
	m_pattern->tob_r = tob_r;
	m_pattern->toa_r = toa_r;
	m_pattern->tob_a = tob_a;
	m_pattern->toa_a = toa_a;
	/*Update slot order*/
	_mcc_fill_slot_info(m_pattern, false, ref_dur->dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, true, bt_dur, NULL);
	_mcc_fill_slot_info(m_pattern, false, ano_dur->dur, m_pattern->role_ano);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern2(): status(%d), tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d)\n",
		status, m_pattern->tob_r, m_pattern->toa_r, m_pattern->tob_a,
		m_pattern->toa_a);
	return status;
}

/*
 * copy from _mcc_calculate_2_clients_pattern
 * Calculate pattern1 for 2 wifi slot and 1 bt slot
 * Pattern:
 * |   Wifi slot1   |   Wifi slot2   | Bt slot |
 * |      Dur_r     |     Dur_a      | Bt slot |
 *          bcn                bcn
 * | tob_r | toa_r|tob_a | toa_a| Bt slot |
 * @offset: The offset of Bcns
 * @m_pattern: pattern info
 */
enum rtw_phl_status _mcc_calculate_2wslot_1btslot_pattern1(
			struct rtw_phl_mcc_dur_info *ref_dur,
			struct rtw_phl_mcc_dur_info *ano_dur,
			u16 offset, s16 bt_dur,
			struct rtw_phl_mcc_pattern *m_pattern)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	u16 mcc_intvl = ref_dur->dur + ano_dur->dur;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0, tob_r_cand = 0;
	s16 d_r = ref_dur->dur, d_a = ano_dur->dur, bcns_offset = offset;
	s16 sum = 0, sum_last = 0;
	s16 tob_r_l = ref_dur->dur_limit.max_tob;
	s16 toa_r_l = ref_dur->dur_limit.max_toa;
	s16 tob_a_l = ano_dur->dur_limit.max_tob;
	s16 toa_a_l = ano_dur->dur_limit.max_toa;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern1(): ref_dur(%d), ano_dur(%d), bcns offset(%d)\n",
		d_r, d_a, bcns_offset);
	for (tob_r = EARLY_RX_BCN_T; tob_r < mcc_intvl; tob_r++) {
		toa_r = d_r - tob_r;
		if (toa_r < MIN_RX_BCN_T) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern1(): break loop, by toa_r(%d) < MIN_RX_BCN_T(%d)\n",
				toa_r, MIN_RX_BCN_T);
			break;
		}
		if (ref_dur->dur_limit.enable) {
			if (tob_r > tob_r_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern1(): tob_r(%d) > tob_r_l(%d), Break loop\n",
					tob_r, tob_r_l);
				break;
			} else if (toa_r > toa_r_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern1(): toa_r(%d) > toa_r_l(%d), continue next loop\n",
					toa_r, toa_r_l);
				continue;
			}
		}
		tob_a = bcns_offset - toa_r;
		if (tob_a < EARLY_RX_BCN_T)
			continue;
		toa_a = d_a - tob_a;
		if (toa_a < MIN_RX_BCN_T){
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern1(): break loop, by toa_a(%d) < MIN_RX_BCN_T(%d)\n",
				toa_a, MIN_RX_BCN_T);
			break;
		}
		if (ano_dur->dur_limit.enable) {
			if (tob_a > tob_a_l || toa_a > toa_a_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern1(): tob_a(%d) > tob_a_l(%d) || toa_a(%d) > toa_a_l(%d), continue next loop\n",
					tob_a, tob_a_l, toa_a, toa_a_l);
				continue;
			}
		}
		sum = ((tob_r - toa_r) * (tob_r - toa_r)) +
			((tob_r - tob_a) * (tob_r - tob_a)) +
			((tob_r - toa_a) * (tob_r - toa_a)) +
			((toa_r - tob_a) * (toa_r - tob_a)) +
			((toa_r - toa_a) * (toa_r - toa_a)) +
			((tob_a - toa_a) * (tob_a - toa_a));
		tob_r_cand = tob_r;
		if (sum_last !=0 && sum > sum_last) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern1(): Find the optimal pattern, by get minSum\n");
			break;
		}
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern1(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d), sum_last(%d), sum(%d)\n",
			tob_r, toa_r, tob_a, toa_a, sum_last, sum);
		sum_last = sum;
	}
	if (0 == tob_r_cand) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2wslot_1btslot_pattern1(): Can't found suitable pattern, goto calc_nego_pattern\n");
		goto exit;
	}
	tob_r = tob_r_cand;
	toa_r = d_r - tob_r;
	tob_a = bcns_offset - toa_r;
	toa_a = d_a - tob_a;
	m_pattern->tob_r = tob_r;
	m_pattern->toa_r = toa_r;
	m_pattern->tob_a = tob_a;
	m_pattern->toa_a = toa_a;
	_mcc_fill_slot_info(m_pattern, false, ref_dur->dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, false, ano_dur->dur, m_pattern->role_ano);
	_mcc_fill_slot_info(m_pattern, true, bt_dur, NULL);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2wslot_1btslot_pattern1(): status(%d), tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d)\n",
		status, m_pattern->tob_r, m_pattern->toa_r, m_pattern->tob_a,
		m_pattern->toa_a);
	return status;
}

/*
 * Calculate pattern for 2 wifi slot and 1 bt slot
 */
enum rtw_phl_status _mcc_calculate_2wslot_1btslot_pattern(
			struct rtw_phl_mcc_dur_info *ref_dur,
			struct rtw_phl_mcc_dur_info *ano_dur,
			s16 bt_dur, struct rtw_phl_mcc_pattern *m_pattern)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	s16 offset_min = 0, offset_max = 0;
	u16 offset = m_pattern->bcns_offset;

	_mcc_get_offset_range_for_2wslot_1btslot_p1(ref_dur->dur, ano_dur->dur,
						&offset_min, &offset_max);
	if (offset >= offset_min && offset <= offset_max) {
		if (RTW_PHL_STATUS_SUCCESS !=
			_mcc_calculate_2wslot_1btslot_pattern1(ref_dur, ano_dur,
						offset, bt_dur, m_pattern)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2wslot_1btslot_pattern(): Pattern1, Can't found suitable pattern, goto calc_nego_pattern\n");
			goto calc_nego_pattern;
		} else {
			status = RTW_PHL_STATUS_SUCCESS;
			goto exit;
		}
	}
	_mcc_get_offset_range_for_2wslot_1btslot_p2(ref_dur->dur, ano_dur->dur,
					bt_dur, &offset_min, &offset_max);
	if (offset >= offset_min && offset <= offset_max) {
		if (RTW_PHL_STATUS_SUCCESS !=
			_mcc_calculate_2wslot_1btslot_pattern2(ref_dur, ano_dur,
						offset, bt_dur, m_pattern)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2wslot_1btslot_pattern(): Pattern2, Can't found suitable pattern, goto calc_nego_pattern\n");
			goto calc_nego_pattern;
		} else {
			status = RTW_PHL_STATUS_SUCCESS;
			goto exit;
		}
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2wslot_1btslot_pattern(): worsecase, goto calc_nego_pattern\n");
		goto calc_nego_pattern;
	}
calc_nego_pattern:
	if (_mcc_calc_2wslot_1btslot_nego_p1(ref_dur,
					ano_dur, offset, bt_dur, m_pattern)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2wslot_1btslot_pattern(): calc_nego_pattern1 ok\n");
	} else if (_mcc_calc_2wslot_1btslot_nego_p2(ref_dur,
					ano_dur, offset, bt_dur, m_pattern)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2wslot_1btslot_pattern(): calc_nego_pattern2 ok\n");
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calculate_2wslot_1btslot_pattern(): calc_nego_pattern fail\n");
		goto exit;
	}
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

/*
 * |Wifi1 slot| Wifi2 slot|
 * <              150TU               ><              150TU               >
 * |      Dur_r     |     Dur_a      |      Dur_r     |     Dur_a      |
 *          bcn_r                                                     bcn_a
 * | tob_r | toa_r|                                        |tob_a | toa_a|
 */
enum rtw_phl_status _mcc_calc_2_clients_worsecase_pattern(u16 ref_dur,
				u16 ano_dur, u16 offset, u16 ref_bcn_intvl,
				struct rtw_phl_mcc_pattern *m_pattern)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	u16 mcc_intvl = ref_dur + ano_dur;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0;
	s16 d_r = ref_dur, d_a = ano_dur, bcns_offset = offset;
	s16 sum = 0, sum_last = 0, offset_min = 0, offset_max = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_worsecase_pattern(): ref_dur(%d), ano_dur(%d), bcns offset(%d), ref_bcn_intvl(%d)\n",
		ref_dur, ano_dur, offset, ref_bcn_intvl);
	if (ref_bcn_intvl != HANDLE_BCN_INTVL) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2_clients_worsecase_pattern(): ref_bcn_intvl(%d) != HANDLE_BCN_INTVL(%d), now, we can't calculate the pattern\n",
			ref_bcn_intvl, HANDLE_BCN_INTVL);
		goto exit;
	}
	_mcc_get_offset_range_for_2_clients_worsecase(d_r, d_a, ref_bcn_intvl,
						&offset_min, &offset_max);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_worsecase_pattern(): we can calculate the range of bcn offset is %d~%d\n",
			offset_min, offset_max);
	if ((bcns_offset >= offset_min) && (bcns_offset <= offset_max))
		goto calc;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_worsecase_pattern(): transform bcn offset from %d to %d\n",
			bcns_offset, ref_bcn_intvl - bcns_offset);
	/*bcn offfset = 85, we can transform to -15*/
	bcns_offset = ref_bcn_intvl - bcns_offset;
	if (bcns_offset >= offset_min && offset_min <=offset_max) {
		goto calc;
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calculate_2_clients_worsecase_pattern(): bcn offset out of range, we can't calculate it\n");
		goto exit;
	}
calc:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_worsecase_pattern(): Start calculate\n");
	for (tob_r = EARLY_RX_BCN_T; tob_r < mcc_intvl; tob_r++) {
		toa_r = d_r - tob_r;
		if (toa_r < MIN_RX_BCN_T) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_worsecase_pattern(): Find the optimal pattern, by toa_r(%d) < MIN_RX_BCN_T(%d)\n",
				toa_r, MIN_RX_BCN_T);
			break;
		}
		tob_a = bcns_offset + 2 * ref_bcn_intvl - toa_r - mcc_intvl;
		if (tob_a < EARLY_RX_BCN_T)
			continue;
		toa_a = d_a - tob_a;
		if (toa_a < MIN_RX_BCN_T){
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_worsecase_pattern(): Find the optimal pattern, by toa_a(%d) < MIN_RX_BCN_T(%d)\n",
				toa_a, MIN_RX_BCN_T);
			break;
		}
		sum = ((tob_r - toa_r) * (tob_r - toa_r)) +
			((tob_r - tob_a) * (tob_r - tob_a)) +
			((tob_r - toa_a) * (tob_r - toa_a)) +
			((toa_r - tob_a) * (toa_r - tob_a)) +
			((toa_r - toa_a) * (toa_r - toa_a)) +
			((tob_a - toa_a) * (tob_a - toa_a));
		if (sum_last !=0 && sum > sum_last) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_worsecase_pattern(): Find the optimal pattern, by get minSum\n");
			break;
		}
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_worsecase_pattern(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d), sum_last(%d), sum(%d)\n",
			tob_r, toa_r, tob_a, toa_a, sum_last, sum);
		sum_last = sum;

	}
	tob_r = tob_r - 1;
	toa_r = d_r - tob_r;
	tob_a = bcns_offset + 2 * ref_bcn_intvl - toa_r - mcc_intvl;
	toa_a = d_a - tob_a;
	m_pattern->tob_r = (u8)tob_r;
	m_pattern->toa_r = (u8)toa_r;
	m_pattern->tob_a = (u8)tob_a;
	m_pattern->toa_a = (u8)toa_a;
	_mcc_fill_slot_info(m_pattern, false, ref_dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, false, ano_dur, m_pattern->role_ano);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_worsecase_pattern(): Result, tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d)\n",
			tob_r, toa_r, tob_a, toa_a);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	if (status != RTW_PHL_STATUS_SUCCESS)
		m_pattern->calc_fail = true;
	return status;
}

#if 0
void _mcc_fill_bt_slot(struct phl_mcc_info *minfo,
				struct rtw_phl_mcc_dur_info *ref_dur,
				struct rtw_phl_mcc_dur_info *ano_dur)
{
	struct rtw_phl_mcc_bt_info *bt_info = &minfo->bt_info;
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_pattern *m_pattern = &en_info->m_pattern;
	u8 i = 0;
	s16 spacing = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_fill_bt_slot()\n");
	if (false == bt_info->add_bt_role)
		goto exit;
	spacing = m_pattern->bcns_offset - m_pattern->toa_r - m_pattern->tob_a;
	if (0 < spacing) {
		if (bt_info->bt_dur < spacing) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_bt_slot(): bt_info->bt_dur(%d) < spacing(%d), adjust BT dur to (%d), please check code\n",
				bt_info->bt_dur, spacing, spacing);
			bt_info->bt_dur = spacing;
			bt_info->bt_seg[0] = bt_info->bt_dur;
			bt_info->bt_seg_num = 1;
			goto exit;
		}
		bt_info->bt_seg[0] = spacing;
		bt_info->bt_seg[1] = bt_info->bt_dur - bt_info->bt_seg[0];
		bt_info->bt_seg_num = 2;
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_bt_slot(): Segment Bt dur to Seg1(%d), Seg2(%d)\n",
			bt_info->bt_seg[0], bt_info->bt_seg[1]);
	} else if (0 == spacing){
		bt_info->bt_seg[0] = bt_info->bt_dur;
		bt_info->bt_seg_num = 1;
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_bt_slot(): spacing(%d) < 0, please check code,\n",
			spacing);
	}
exit:
	_mcc_dump_bt_ino(bt_info);
	return;
}
#endif

/*
 * |Wifi1 slot|Wifi2 slot|
 */
bool _mcc_calc_2_wrole_nego_pattern(struct rtw_phl_mcc_dur_info *ref_dur,
				struct rtw_phl_mcc_dur_info *ano_dur, s16 offset,
				struct rtw_phl_mcc_pattern *m_pattern)
{
	bool cal_ok = false;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0;

	if ((!ref_dur->dur_limit.enable) && (!ano_dur->dur_limit.enable)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2_wrole_nego_pattern(): not support, both not enable slot limitation\n");
		/*todo*/
		goto exit;
	}
	if ((ref_dur->dur_limit.enable) && (ano_dur->dur_limit.enable)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2_wrole_nego_pattern(): not support, both enable slot limitation\n");
		/*todo*/
		goto exit;
	}
	if ((ref_dur->dur_limit.enable) && (!ano_dur->dur_limit.enable)) {
		tob_r = ref_dur->dur / 2;
		toa_r = ref_dur->dur - tob_r;
		if (tob_r > ref_dur->dur_limit.max_tob) {
			tob_r = ref_dur->dur_limit.max_tob;
			toa_r = ref_dur->dur - tob_r;
		}
		if (toa_r > ref_dur->dur_limit.max_toa) {
			toa_r = ref_dur->dur_limit.max_toa;
			tob_r = ref_dur->dur - toa_r;
		}
		if ((tob_r > ref_dur->dur_limit.max_tob) ||
			(toa_r > ref_dur->dur_limit.max_toa)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2_wrole_nego_pattern(): After adjust tob_r(%d) > max_tob(%d) or toa_r(%d) > max_toa(%d)\n",
				tob_r, ref_dur->dur_limit.max_tob,
				toa_r, ref_dur->dur_limit.max_toa);
			goto exit;
		}
		tob_a = offset - toa_r;
		toa_a = ano_dur->dur - tob_a;
		if (tob_a <= 0 || toa_a <= 0) {
			m_pattern->courtesy_i.c_en= true;
			m_pattern->courtesy_i.c_num = 3;
			m_pattern->courtesy_i.src_role = m_pattern->role_ref;
			m_pattern->courtesy_i.tgt_role = m_pattern->role_ano;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2_wrole_nego_pattern(): Limi by ref dur, courtesy_info: c_en(%d), c_num(%d), src_role->macid(0x%x), tgt_role->macid(0x%x)\n",
				m_pattern->courtesy_i.c_en,
				m_pattern->courtesy_i.c_num,
				m_pattern->courtesy_i.src_role->macid,
				m_pattern->courtesy_i.tgt_role->macid);
		}
		goto fill_pattern;
	}
	if ((!ref_dur->dur_limit.enable) && (ano_dur->dur_limit.enable)) {
		tob_a = ano_dur->dur / 2;
		toa_a = ano_dur->dur - tob_a;
		if (tob_a > ano_dur->dur_limit.max_tob) {
			tob_a = ano_dur->dur_limit.max_tob;
			toa_a = ano_dur->dur - tob_a;
		}
		if (toa_a > ano_dur->dur_limit.max_toa) {
			toa_a = ano_dur->dur_limit.max_toa;
			tob_a = ano_dur->dur - toa_a;
		}
		if ((tob_a > ano_dur->dur_limit.max_tob) ||
			(toa_a > ano_dur->dur_limit.max_toa)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2_wrole_nego_pattern(): After adjust tob_r(%d) > max_tob(%d) or toa_r(%d) > max_toa(%d)\n",
				tob_a, ano_dur->dur_limit.max_tob,
				toa_a, ano_dur->dur_limit.max_toa);
			goto exit;
		}
		toa_r = offset - tob_a;
		tob_r = ref_dur->dur - toa_r;
		if (toa_r <= 0 || tob_r <= 0) {
			m_pattern->courtesy_i.c_en= true;
			m_pattern->courtesy_i.c_num = 3;
			m_pattern->courtesy_i.src_role = m_pattern->role_ano;
			m_pattern->courtesy_i.tgt_role = m_pattern->role_ref;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2_wrole_nego_pattern(): Limi by ano dur, courtesy_info: c_en(%d), c_num(%d), src_role->macid(0x%x), tgt_role->macid(0x%x)\n",
				m_pattern->courtesy_i.c_en,
				m_pattern->courtesy_i.c_num,
				m_pattern->courtesy_i.src_role->macid,
				m_pattern->courtesy_i.tgt_role->macid);
		}
		goto fill_pattern;
	}
fill_pattern:
	cal_ok = true;
	m_pattern->tob_r = tob_r;
	m_pattern->toa_r = toa_r;
	m_pattern->tob_a = tob_a;
	m_pattern->toa_a = toa_a;
	_mcc_fill_slot_info(m_pattern, false, ref_dur->dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, false, ano_dur->dur, m_pattern->role_ano);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wrole_nego_pattern(): calc nego patter\n");
exit:
	return cal_ok;
}

#if 0
/**
 * Calculate the optimal pattern for 2wrole MCC with limitation of time slot v2
 * @ref_dur: Duration info of reference ch
 * @ano_dur: Duration info of another ch
 * @offset: The offset between beacon of ref_role and beacon of ano_role
 * @m_pattern: mcc pattern.
 **/
enum rtw_phl_status _mcc_calculate_2_wrole_pattern_v2(
				struct rtw_phl_mcc_dur_info *ref_dur,
				struct rtw_phl_mcc_dur_info *ano_dur,
				u16 offset,
				struct rtw_phl_mcc_pattern *m_pattern)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0;
	s16 tob_r_cand = 0, toa_r_cand = 0, tob_a_cand = 0, toa_a_cand = 0;
	s16 d_r = ref_dur->dur, d_a = ano_dur->dur, bcns_offset = offset;
	s16 sum = 0, sum_last = 0;
	s16 tob_r_min = rtw_phl_role_is_ap_category(m_pattern->role_ref->wrole) ?
			EARLY_TX_BCN_T : EARLY_RX_BCN_T;
	s16 toa_r_min = rtw_phl_role_is_ap_category(m_pattern->role_ref->wrole) ?
			MIN_TX_BCN_T : MIN_RX_BCN_T;
	s16 tob_a_min = rtw_phl_role_is_ap_category(m_pattern->role_ano->wrole) ?
			EARLY_TX_BCN_T : EARLY_RX_BCN_T;
	s16 toa_a_min = rtw_phl_role_is_ap_category(m_pattern->role_ano->wrole) ?
			MIN_TX_BCN_T : MIN_RX_BCN_T;
	s16 min_bcns_offset = toa_r_min + tob_a_min;
	s16 i = 0, cnt = 0;
	bool bdry_r = false, bdry_a = false; /*reach boundary edge*/

	if ((bcns_offset > (m_pattern->role_ref->bcn_intvl - min_bcns_offset))
		|| (bcns_offset < min_bcns_offset)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2_wrole_pattern_v2(): bcns_offset(%d) > max_offset(%d) || bcns_offset < min_offset(%d), goto calc_nego_pattern\n",
			bcns_offset,
			(m_pattern->role_ref->bcn_intvl - min_bcns_offset),
			min_bcns_offset);
		goto calc_nego_pattern;
	}
	cnt = bcns_offset - toa_r_min - tob_a_min;
	toa_r = toa_r_min;
	tob_a = tob_a_min;
	for (i = 0; i < cnt; i++) {
		if ((true == bdry_r) && (true == bdry_a)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2_wrole_pattern_v2(): braek loop by (true == bdry_r) && (true == bdry_a)\n");
			break;
		}
		if (true == bdry_r)
			goto calc_ano;
		if (i > 0)
			toa_r++;
		tob_r = d_r - toa_r;
		if (tob_r < tob_r_min) {
			bdry_r = true;
			goto calc_ano;
		}
		if (!ref_dur->dur_limit.enable)
			goto check_conflict1;
		if ((tob_r > ref_dur->dur_limit.max_tob) ||
			(toa_r > ref_dur->dur_limit.max_toa)) {
			goto calc_ano;
		}
check_conflict1:
		if ((toa_r + tob_a_cand) > bcns_offset) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2_wrole_pattern_v2(): braek loop by conflict(toa_r(%d) + tob_a_cand(%d)) > bcns_offset(%d)\n",
				toa_r, tob_a_cand, bcns_offset);
			break;
		}
		toa_r_cand = toa_r;
calc_ano:
		if (true == bdry_a)
			continue;
		if (i > 0)
			tob_a++;
		toa_a = d_a - tob_a;
		if (toa_a < toa_a_min) {
			bdry_a = true;
			continue;
		}
		if (!ano_dur->dur_limit.enable)
			goto check_conflict2;
		if ((tob_a > ano_dur->dur_limit.max_tob) ||
			(toa_a > ano_dur->dur_limit.max_toa)) {
			continue;
		}
check_conflict2:
		if ((bcns_offset - (tob_a + toa_r_cand)) >
			(m_pattern->d_r_d_a_spacing_max)) {
			continue;
		}
		if ((tob_a + toa_r_cand) > bcns_offset) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2_wrole_pattern_v2(): braek loop by conflict(tob_a(%d) + toa_r_cand(%d)) > bcns_offset(%d)\n",
				tob_a, toa_r_cand, bcns_offset);
			break;
		}
		tob_a_cand = tob_a;
/*calculate candidate result*/
		tob_r_cand = d_r - toa_r_cand;
		toa_a_cand = d_a - tob_a_cand;
		sum = ((tob_r_cand - toa_r_cand) * (tob_r_cand - toa_r_cand)) +
		      ((tob_r_cand - tob_a_cand) * (tob_r_cand - tob_a_cand)) +
		      ((tob_r_cand - toa_a_cand) * (tob_r_cand - toa_a_cand)) +
		      ((toa_r_cand - tob_a_cand) * (toa_r_cand - tob_a_cand)) +
		      ((toa_r_cand - toa_a_cand) * (toa_r_cand - toa_a_cand)) +
		      ((tob_a_cand - toa_a_cand) * (tob_a_cand - toa_a_cand));
		if (sum_last !=0 && sum > sum_last) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern_v2(): Find the optimal pattern, by get minSum\n");
			break;
		}
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern_v2(): tob_r_cand(%d), toa_r_cand(%d), tob_a_cand(%d), toa_a_cand(%d), sum_last(%d), sum(%d)\n",
			tob_r_cand, toa_r_cand, tob_a_cand, toa_a_cand,
			sum_last, sum);
		sum_last = sum;
	}
	if ((0 == toa_r_cand) || (0 == tob_a_cand)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2_wrole_pattern_v2(): Can't found suitable pattern, goto calc_nego_pattern\n");
		goto calc_nego_pattern;
	}
	m_pattern->tob_r = tob_r_cand;
	m_pattern->toa_r = toa_r_cand;
	m_pattern->tob_a = tob_a_cand;
	m_pattern->toa_a = toa_a_cand;
	_mcc_fill_slot_info(m_pattern, false, ref_dur->dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, false, ano_dur->dur, m_pattern->role_ano);
	status = RTW_PHL_STATUS_SUCCESS;
	goto exit;
calc_nego_pattern:
	if (_mcc_calc_2_wrole_nego_pattern(ref_dur, ano_dur, bcns_offset,
						m_pattern)) {
		status = RTW_PHL_STATUS_SUCCESS;
	}
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern_v2(): m_pattern: tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d)\n",
		m_pattern->tob_r, m_pattern->toa_r, m_pattern->tob_a,
		m_pattern->toa_a);
	return status;
}
#endif

#if 0
/**
 * Copy from _mcc_calc_2_clients_worsecase_pattern and add limitation of time slot
 * worsecase: TDMA interval is 150 TU
 * Calculate the optimal pattern for 2wifi slot with limitation of time slot for worsecase
 * @ref_dur: Duration info of reference slot
 * @ano_dur: Duration info of another slot
 * @offset: The offset between beacon of ref_role and beacon of ano_role
 * @ref_bcn_intvl: Bcn interval of reference role
 * @m_pattern: mcc pattern.
 **/
enum rtw_phl_status _mcc_calc_2_wifi_slot_worsecase_pattern(
			struct rtw_phl_mcc_dur_info *ref_dur,
			struct rtw_phl_mcc_dur_info *ano_dur, u16 offset,
			u16 ref_bcn_intvl, struct rtw_phl_mcc_pattern *m_pattern)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	u16 mcc_intvl = ref_dur->dur + ano_dur->dur;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0, tob_r_cand = 0;

	s16 d_r = ref_dur->dur, d_a = ano_dur->dur, bcns_offset = offset;
	s16 sum = 0, sum_last = 0, offset_min = 0, offset_max = 0;
	s16 tob_r_l = ref_dur->dur_limit.max_tob;
	s16 toa_r_l = ref_dur->dur_limit.max_toa;
	s16 tob_a_l = ano_dur->dur_limit.max_tob;
	s16 toa_a_l = ano_dur->dur_limit.max_toa;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): ref_dur(%d), ano_dur(%d), bcns offset(%d), ref_bcn_intvl(%d)\n",
		d_r, d_a, offset, ref_bcn_intvl);
	if (ref_bcn_intvl != HANDLE_BCN_INTVL) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): ref_bcn_intvl(%d) != HANDLE_BCN_INTVL(%d), now, we can't calculate the pattern\n",
			ref_bcn_intvl, HANDLE_BCN_INTVL);
		goto exit;
	}
	_mcc_get_offset_range_for_2_clients_worsecase(d_r, d_a, ref_bcn_intvl,
						&offset_min, &offset_max);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): we can calculate the range of bcn offset is %d~%d\n",
			offset_min, offset_max);
	if ((bcns_offset >= offset_min) && (bcns_offset <= offset_max))
		goto calc;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): transform bcn offset from %d to %d\n",
			bcns_offset, ref_bcn_intvl - bcns_offset);
	/*bcn offfset = 85, we can transform to -15*/
	bcns_offset = ref_bcn_intvl - bcns_offset;
	if (bcns_offset >= offset_min && offset_min <=offset_max) {
		goto calc;
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): bcn offset out of range, we can't calculate it\n");
		goto exit;
	}
calc:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): Start calculate\n");
	for (tob_r = EARLY_RX_BCN_T; tob_r < mcc_intvl; tob_r++) {
		toa_r = d_r - tob_r;
		if (toa_r < MIN_RX_BCN_T) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): Break loop, by toa_r(%d) < MIN_RX_BCN_T(%d)\n",
				toa_r, MIN_RX_BCN_T);
			break;
		}
		if (ref_dur->dur_limit.enable) {
			if (toa_r > toa_r_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): toa_r(%d) > toa_r_l(%d), continue next loop\n",
					toa_r, toa_r_l);
				continue;
			}
			if (tob_r > tob_r_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): Break loop, tob_r(%d) > tob_r_l(%d)\n",
					tob_r, tob_r_l);
				break;
			}
		}
		tob_a = bcns_offset + 2 * ref_bcn_intvl - toa_r - mcc_intvl;
		if (tob_a < EARLY_RX_BCN_T)
			continue;
		toa_a = d_a - tob_a;
		if (toa_a < MIN_RX_BCN_T){
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): Break loop, by toa_a(%d) < MIN_RX_BCN_T(%d)\n",
				toa_a, MIN_RX_BCN_T);
			break;
		}
		if (ano_dur->dur_limit.enable) {
			if (tob_a > tob_a_l || toa_a > toa_a_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): tob_a(%d) > tob_a_l(%d) || toa_a(%d) > toa_a_l(%d), continue next loop\n",
					tob_a, tob_a_l, toa_a, toa_a_l);
				continue;
			}
		}
		sum = ((tob_r - toa_r) * (tob_r - toa_r)) +
			((tob_r - tob_a) * (tob_r - tob_a)) +
			((tob_r - toa_a) * (tob_r - toa_a)) +
			((toa_r - tob_a) * (toa_r - tob_a)) +
			((toa_r - toa_a) * (toa_r - toa_a)) +
			((tob_a - toa_a) * (tob_a - toa_a));
		tob_r_cand = tob_r;
		if (sum_last !=0 && sum > sum_last) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): Find the optimal pattern, by get minSum\n");
			break;
		}
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d), sum_last(%d), sum(%d)\n",
			tob_r, toa_r, tob_a, toa_a, sum_last, sum);
		sum_last = sum;

	}
	if (0 == tob_r_cand) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): Can't found suitable pattern, goto calc_nego_pattern\n");
		goto calc_nego_pattern;
	}
	tob_r = tob_r_cand;
	toa_r = d_r - tob_r;
	tob_a = bcns_offset + 2 * ref_bcn_intvl - toa_r - mcc_intvl;
	toa_a = d_a - tob_a;
	m_pattern->tob_r = tob_r;
	m_pattern->toa_r = toa_r;
	m_pattern->tob_a = tob_a;
	m_pattern->toa_a = toa_a;
	_mcc_fill_slot_info(m_pattern, false, ref_dur->dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, false, ano_dur->dur, m_pattern->role_ano);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): Result, tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d)\n",
			tob_r, toa_r, tob_a, toa_a);
	status = RTW_PHL_STATUS_SUCCESS;
	goto exit;
calc_nego_pattern:
	if (_mcc_calc_2_wrole_nego_pattern(ref_dur, ano_dur, bcns_offset,
					m_pattern))
		status = RTW_PHL_STATUS_SUCCESS;
exit:
	if (status != RTW_PHL_STATUS_SUCCESS)
		m_pattern->calc_fail = true;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calc_2_wifi_slot_worsecase_pattern(): status(%d)\n",
		status);
	return status;
}
#endif

/*
 * Calculate the optimal pattern for 2wrole MCC with limitation of time slot
 * @ref_dur: Duration of reference ch
 * @ano_dur: Duration of another ch
 * @offset: The offset between beacon of client1 and beacon of client2
 * @m_pattern: mcc pattern.
 * |          Wifi1 slot        |            Wifi2 slot        |          Wifi1 slot        |             Wifi2 slot        |
 * <tob_r> Bcn_r <toa_r>                                   <tob_r> Bcn_r <toa_r>
 *                                  <tob_a> Bcn_a <toa_a>                                  <tob_a> Bcn_a <toa_a>
 *	         <       bcns_offset       >
 */
enum rtw_phl_status _mcc_calculate_2_wrole_pattern(
				struct rtw_phl_mcc_dur_info *ref_dur,
				struct rtw_phl_mcc_dur_info *ano_dur,
				u16 offset,
				struct rtw_phl_mcc_pattern *m_pattern)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	u16 mcc_intvl = ref_dur->dur + ano_dur->dur;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0, tob_r_cand = 0;
	s16 d_r = ref_dur->dur, d_a = ano_dur->dur, bcns_offset = offset;
	s16 sum = 0, sum_last = 0;
	s16 tob_r_l = ref_dur->dur_limit.max_tob;
	s16 toa_r_l = ref_dur->dur_limit.max_toa;
	s16 tob_a_l = ano_dur->dur_limit.max_tob;
	s16 toa_a_l = ano_dur->dur_limit.max_toa;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern(): ref_dur(%d), ano_dur(%d), bcns offset(%d)\n",
		d_r, d_a, bcns_offset);
	for (tob_r = EARLY_RX_BCN_T; tob_r < mcc_intvl; tob_r++) {
		toa_r = d_r - tob_r;
		if (toa_r < MIN_RX_BCN_T) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern(): break loop, by toa_r(%d) < MIN_RX_BCN_T(%d)\n",
				toa_r, MIN_RX_BCN_T);
			break;
		}
		if (ref_dur->dur_limit.enable) {
			if (tob_r > tob_r_l || toa_r > toa_r_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern(): tob_r(%d) > tob_r_l(%d) || toa_r(%d) > toa_r_l(%d), continue next loop\n",
					tob_r, tob_r_l, toa_r, toa_r_l);
				continue;
			}
		}
		tob_a = bcns_offset - toa_r;
		if (tob_a < EARLY_RX_BCN_T)
			continue;
		toa_a = d_a - tob_a;
		if (toa_a < MIN_RX_BCN_T){
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern(): break loop, by toa_a(%d) < MIN_RX_BCN_T(%d)\n",
				toa_a, MIN_RX_BCN_T);
			break;
		}
		if (ano_dur->dur_limit.enable) {
			if (tob_a > tob_a_l || toa_a > toa_a_l) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern(): tob_a(%d) > tob_a_l(%d) || toa_a(%d) > toa_a_l(%d), continue next loop\n",
					tob_a, tob_a_l, toa_a, toa_a_l);
				continue;
			}
		}
		sum = ((tob_r - toa_r) * (tob_r - toa_r)) +
			((tob_r - tob_a) * (tob_r - tob_a)) +
			((tob_r - toa_a) * (tob_r - toa_a)) +
			((toa_r - tob_a) * (toa_r - tob_a)) +
			((toa_r - toa_a) * (toa_r - toa_a)) +
			((tob_a - toa_a) * (tob_a - toa_a));
		tob_r_cand = tob_r;
		if (sum_last !=0 && sum > sum_last) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern(): Find the optimal pattern, by get minSum\n");
			break;
		}
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_wrole_pattern(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d), sum_last(%d), sum(%d)\n",
			tob_r, toa_r, tob_a, toa_a, sum_last, sum);
		sum_last = sum;
	}
	if (0 == tob_r_cand) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_calculate_2_wrole_pattern(): Can't found suitable pattern, goto calc_nego_pattern\n");
		goto calc_nego_pattern;
	}
	tob_r = tob_r_cand;
	toa_r = d_r - tob_r;
	tob_a = bcns_offset - toa_r;
	toa_a = d_a - tob_a;
	m_pattern->tob_r = tob_r;
	m_pattern->toa_r = toa_r;
	m_pattern->tob_a = tob_a;
	m_pattern->toa_a = toa_a;
	_mcc_fill_slot_info(m_pattern, false, ref_dur->dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, false, ano_dur->dur, m_pattern->role_ano);
	status = RTW_PHL_STATUS_SUCCESS;
	goto exit;
calc_nego_pattern:
	if (_mcc_calc_2_wrole_nego_pattern(ref_dur, ano_dur, bcns_offset,
					m_pattern))
		status = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_pattern(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d)\n",
			m_pattern->tob_r, m_pattern->toa_r, m_pattern->tob_a,
			m_pattern->toa_a);
	return status;
}

/*
 * Calculate the optimal pattern for client+client MCC
 * @ref_dur: Duration of reference ch
 * @ano_dur: Duration of another ch
 * @offset: The offset between beacon of client1 and beacon of client2
 * @m_pattern: mcc pattern.
 * |          Wifi1 slot        |            Wifi2 slot        |          Wifi1 slot        |             Wifi2 slot        |
 * <tob_r> Bcn_r <toa_r>                                   <tob_r> Bcn_r <toa_r>
 *                                  <tob_a> Bcn_a <toa_a>                                  <tob_a> Bcn_a <toa_a>
 *	         <       bcns_offset       >
 */
void _mcc_calculate_2_clients_pattern(u16 ref_dur, u16 ano_dur, u16 offset,
					struct rtw_phl_mcc_pattern *m_pattern)
{
	u16 mcc_intvl = ref_dur + ano_dur;
	s16 tob_r = 0, toa_r = 0, tob_a = 0, toa_a = 0;
	s16 d_r = ref_dur, d_a = ano_dur, bcns_offset = offset;
	s16 sum = 0, sum_last = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_pattern(): ref_dur(%d), ano_dur(%d), bcns offset(%d)\n",
		ref_dur, ano_dur, offset);
	for (tob_r = EARLY_RX_BCN_T; tob_r < mcc_intvl; tob_r++) {
		toa_r = d_r - tob_r;
		if (toa_r < MIN_RX_BCN_T) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_pattern(): Find the optimal pattern, by toa_r(%d) < MIN_RX_BCN_T(%d)\n",
				toa_r, MIN_RX_BCN_T);
			break;
		}
		tob_a = bcns_offset - toa_r;
		if (tob_a < EARLY_RX_BCN_T)
			continue;
		toa_a = d_a - tob_a;
		if (toa_a < MIN_RX_BCN_T){
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_pattern(): Find the optimal pattern, by toa_a(%d) < MIN_RX_BCN_T(%d)\n",
				toa_a, MIN_RX_BCN_T);
			break;
		}
		sum = ((tob_r - toa_r) * (tob_r - toa_r)) +
			((tob_r - tob_a) * (tob_r - tob_a)) +
			((tob_r - toa_a) * (tob_r - toa_a)) +
			((toa_r - tob_a) * (toa_r - tob_a)) +
			((toa_r - toa_a) * (toa_r - toa_a)) +
			((tob_a - toa_a) * (tob_a - toa_a));
		if (sum_last !=0 && sum > sum_last) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_pattern(): Find the optimal pattern, by get minSum\n");
			break;
		}
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_pattern(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d), sum_last(%d), sum(%d)\n",
			tob_r, toa_r, tob_a, toa_a, sum_last, sum);
		sum_last = sum;
	}
	tob_r = tob_r - 1;
	toa_r = d_r - tob_r;
	tob_a = bcns_offset - toa_r;
	toa_a = d_a - tob_a;
	m_pattern->tob_r = (u8)tob_r;
	m_pattern->toa_r = (u8)toa_r;
	m_pattern->tob_a = (u8)tob_a;
	m_pattern->toa_a = (u8)toa_a;
	_mcc_fill_slot_info(m_pattern, false, ref_dur, m_pattern->role_ref);
	_mcc_fill_slot_info(m_pattern, false, ano_dur, m_pattern->role_ano);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_calculate_2_clients_pattern(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d)\n",
			tob_r, toa_r, tob_a, toa_a);
}

/*
 * Fill patten info for 2wifi slot req + bt slot req
 * @minfo: enable mcc info
 * @role_ref: reference wifi slot req
 * @role_ano: another wifi slot req
 */
void _mcc_fill_2_wrole_bt_pattern(struct phl_mcc_info *minfo,
	struct rtw_phl_mcc_role *role_ref, struct rtw_phl_mcc_role *role_ano)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_pattern *m_pattern = &en_info->m_pattern;
	struct rtw_phl_mcc_dur_info dur_ref = role_ref->policy.dur_info;
	struct rtw_phl_mcc_dur_info dur_ano = role_ano->policy.dur_info;
#if 0
	u16 dr_max = (dur_ref.dur_limit.enable) ?
			dur_ref.dur_limit.max_dur : en_info->mcc_intvl;
	u16 da_max = (dur_ano.dur_limit.enable) ?
			dur_ano.dur_limit.max_dur : en_info->mcc_intvl;


	if (minfo->bt_info.bt_seg_num == 2) {
		/*temporary disable this case, we can't handle it in GC(NOA)+STA*/
		minfo->bt_info.add_bt_role = true;
		if (((dur_ref.dur + minfo->bt_info.bt_seg[0]) <= dr_max) &&
			((dur_ano.dur + minfo->bt_info.bt_seg[1]) <= da_max)) {
			dur_ref.dur += minfo->bt_info.bt_seg[0];
			dur_ano.dur += minfo->bt_info.bt_seg[1];
		} else if ((dur_ref.dur + minfo->bt_info.bt_dur) <= dr_max) {
			dur_ref.dur += minfo->bt_info.bt_dur;
		} else if ((dur_ano.dur + minfo->bt_info.bt_dur) <= da_max) {
			dur_ano.dur += minfo->bt_info.bt_dur;
		} else {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_2_wrole_bt_pattern(): Fail to combine wifi slot and bt slot to single slot, we use default worsecase pattern \n");
			_mcc_set_2_clients_worsecase_default_pattern(m_pattern, dur_ref.dur);
			goto exit;
		}
		if (RTW_PHL_STATUS_SUCCESS !=
				_mcc_calc_2_wifi_slot_worsecase_pattern(
						&dur_ref, &dur_ano,
						m_pattern->bcns_offset,
						role_ref->bcn_intvl,
						m_pattern)) {
			_mcc_set_2_clients_worsecase_default_pattern(m_pattern, dur_ref.dur);
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_2_wrole_bt_pattern(): _mcc_calc_2_wifi_slot_worsecase_pattern fail, we use default worsecase pattern\n");
		}
		dur_ref = role_ref->policy.dur_info;
		dur_ano = role_ano->policy.dur_info;
		if (m_pattern->tob_r > (dur_ref.dur - MIN_RX_BCN_T)) {
			role_ref->policy.protect_bcn = true;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_2_wrole_bt_pattern(): bcn will in bt dur, we set protect_bcn = true for role_ref\n");
		}
		if (m_pattern->tob_a > (dur_ano.dur - MIN_RX_BCN_T)) {
			role_ano->policy.protect_bcn = true;
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_2_wrole_bt_pattern(): bcn will in bt dur, we set protect_bcn = true for role_ano\n");
		}
	} else
#endif
	if (minfo->bt_info.bt_seg_num == 1) {
		minfo->bt_info.add_bt_role = true;
		if (RTW_PHL_STATUS_SUCCESS !=
			_mcc_calculate_2wslot_1btslot_pattern(&dur_ref,
				&dur_ano, minfo->bt_info.bt_dur, m_pattern)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_2_wrole_bt_pattern(): _mcc_calculate_2wslot_1btslot_pattern fail, we use default worsecase pattern\n");
			_mcc_set_2_clients_worsecase_default_pattern(m_pattern, dur_ref.dur);
		}
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_2_wrole_bt_pattern(): error bt_seg_num(%d)\n",
			minfo->bt_info.bt_seg_num);
	}
#if 0
exit:
#endif
	return;
}

bool _mcc_fill_2wrole_pattern_with_limitation(struct phl_mcc_info *minfo,
	struct rtw_phl_mcc_role *role_ref, struct rtw_phl_mcc_role *role_ano)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_dur_info *dur_i_r = &role_ref->policy.dur_info;
	struct rtw_phl_mcc_dur_info *dur_i_a = &role_ano->policy.dur_info;
	struct rtw_phl_mcc_policy_info *policy_i = NULL;
	struct rtw_phl_mcc_courtesy *courtesy_i = &en_info->m_pattern.courtesy_i;
	bool ret = false;

	if (!(dur_i_r->dur_limit.enable || dur_i_a->dur_limit.enable))
		goto exit;
	if (dur_i_r->dur_limit.enable && dur_i_a->dur_limit.enable) {
		/*implement in phase????*/
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_2wrole_pattern_with_limitation(): Not support for all enable limitation, tag_r(%d), tag_a(%d)\n",
			dur_i_r->dur_limit.tag, dur_i_a->dur_limit.tag);
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_2wrole_pattern_with_limitation(): we ignore the limitation of time slot, it will degrade performance\n");
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_fill_2wrole_pattern_with_limitation\n");
	if (_mcc_discision_duration_for_2role_bt(minfo)) {
		_mcc_fill_2_wrole_bt_pattern(minfo, role_ref,  role_ano);
	} else if (RTW_PHL_STATUS_SUCCESS != _mcc_calculate_2_wrole_pattern(
					dur_i_r, dur_i_a,
					en_info->m_pattern.bcns_offset,
					&en_info->m_pattern)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_2wrole_pattern_with_limitation(): Calc pattern fail with limitation of time slot, we ignore the limitation of time slot, it will degrade performance\n");
		goto exit;
	}
	/*_mcc_fill_bt_slot(minfo, dur_i_r, dur_i_a);*/
	if (true == courtesy_i->c_en) {
		policy_i = &courtesy_i->src_role->policy;
		policy_i->courtesy_en = true;
		policy_i->courtesy_num = courtesy_i->c_num;
		policy_i->courtesy_target = (u8)courtesy_i->tgt_role->macid;
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_2wrole_pattern_with_limitation(): Enable courtesy function, time slot of macid:0x%x replace by time slot of macid:0x%x, courtesy_num(%d)\n",
			courtesy_i->src_role->macid, policy_i->courtesy_target,
			policy_i->courtesy_num);
	}
	ret =true;
exit:
	return ret;
}

void _mcc_fill_2_clients_pattern(struct phl_mcc_info *minfo, u8 worsecase,
	struct rtw_phl_mcc_role *role_ref, struct rtw_phl_mcc_role *role_ano)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;

	if (!worsecase) {
		_mcc_calculate_2_clients_pattern(role_ref->policy.dur_info.dur,
						role_ano->policy.dur_info.dur,
						en_info->m_pattern.bcns_offset,
						&en_info->m_pattern);
		goto exit;
	}
	if (RTW_PHL_STATUS_SUCCESS != _mcc_calc_2_clients_worsecase_pattern(
					role_ref->policy.dur_info.dur,
					role_ano->policy.dur_info.dur,
					en_info->m_pattern.bcns_offset,
					role_ref->bcn_intvl,
					&en_info->m_pattern)) {
		_mcc_set_2_clients_worsecase_default_pattern(&en_info->m_pattern,
						role_ref->policy.dur_info.dur);
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_info_for_2_clients_mode(): _mcc_calc_2_clients_worsecase_pattern fail, we use default worsecase pattern\n");
	}
exit:
	return;
}

enum rtw_phl_status _mcc_get_2_clients_bcn_offset(struct phl_info_t *phl,
			u16 *offset, struct rtw_phl_mcc_role *role_ref,
			struct rtw_phl_mcc_role *role_ano)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	u32 tsf_ref_h = 0, tsf_ref_l = 0, tsf_ano_h = 0, tsf_ano_l = 0;
	u64 tsf_ref = 0, tsf_ano = 0;
	u16 ofst_r = 0, ofst_a = 0;

	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mcc_get_2ports_tsf(phl->hal,
			role_ref->group, role_ref->macid, role_ano->macid,
			&tsf_ref_h, &tsf_ref_l, &tsf_ano_h, &tsf_ano_l)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_get_2_clients_bcn_offset(): Get tsf failed\n");
		goto exit;
	}
	tsf_ref = tsf_ref_h;
	tsf_ref = tsf_ref << 32;
	tsf_ref |= tsf_ref_l;
	tsf_ano = tsf_ano_h;
	tsf_ano = tsf_ano << 32;
	tsf_ano |= tsf_ano_l;
	/*calculate the value between current TSF and TBTT*/
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_get_2_clients_bcn_offset(): role_ref calc_offset_from_tbtt\n");
	phl_calc_offset_from_tbtt(phl, role_ref->wrole, role_ref->rlink, tsf_ref, &ofst_r);

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_get_2_clients_bcn_offset(): role_ano calc_offset_from_tbtt\n");
	phl_calc_offset_from_tbtt(phl, role_ano->wrole, role_ano->rlink, tsf_ano, &ofst_a);

	if (ofst_r < ofst_a)
		ofst_r += role_ref->bcn_intvl;
	*offset = ofst_r - ofst_a;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_get_2_clients_bcn_offset(): bcn offset(%d)\n",
		*offset);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

/**
* set defalut pattern for ap slot+sta slot
 * |       Wifi1 slot           |          Wifi2 slot         |       Wifi1 slot           |          Wifi2 slot         |
 * <tob_r> Bcn_r <toa_r>                                  <tob_r> Bcn_r <toa_r>
 *                                  <tob_a> Bcn_a <toa_a>                                 <tob_a> Bcn_a <toa_a>
 *               <         bcns_offset         >
 **/

void _mcc_set_ap_client_default_pattern(struct phl_mcc_info *minfo,
					u16 *bcns_offet)
{
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_pattern *m_pattern = &en_info->m_pattern;
	struct rtw_phl_mcc_role *role_ref = get_ref_role(en_info);
	struct rtw_phl_mcc_role *role_ano = (role_ref == &en_info->mcc_role[0])
				? &en_info->mcc_role[1] : &en_info->mcc_role[0];

	*bcns_offet = (u8)(en_info->mcc_intvl / 2);
	m_pattern->toa_r= role_ref->policy.dur_info.dur / 2;
	m_pattern->tob_r = role_ref->policy.dur_info.dur - m_pattern->toa_r;
	m_pattern->tob_a = (u8)(*bcns_offet - m_pattern->toa_r);
	m_pattern->toa_a = role_ano->policy.dur_info.dur - m_pattern->tob_a;
	_mcc_fill_slot_info(m_pattern, false, role_ref->policy.dur_info.dur, role_ref);
	_mcc_fill_slot_info(m_pattern, false, role_ano->policy.dur_info.dur, role_ano);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_set_ap_client_default_pattern(): tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d)\n",
		m_pattern->tob_r, m_pattern->toa_r, m_pattern->tob_a, m_pattern->toa_a);
}

void _mcc_set_worsecase_dur_for_2_clients_mode(
				struct rtw_phl_mcc_dur_info *dur_i_1,
				struct rtw_phl_mcc_dur_info *dur_i_2,
				u16 *mcc_intvl)
{
	/*need to get from core layer for worsecase??*/
	if (dur_i_1->dur > dur_i_2->dur) {
		*mcc_intvl = WORSECASE_INTVL;
		dur_i_1->dur = CLIENTS_WORSECASE_LARGE_DUR;
		dur_i_2->dur = (*mcc_intvl - dur_i_1->dur);
	} else {
		*mcc_intvl = WORSECASE_INTVL;
		dur_i_2->dur = CLIENTS_WORSECASE_LARGE_DUR;
		dur_i_1->dur = (*mcc_intvl - dur_i_2->dur);
	}
}

void _mcc_set_dur_for_2_clients_mode(
				struct rtw_phl_mcc_dur_info *dur_i_1,
				struct rtw_phl_mcc_dur_info *dur_i_2,
				u16 *mcc_intvl)
{
	u16 *dur1 = &dur_i_1->dur, *dur2 = &dur_i_2->dur;

	if (*dur1 == MCC_DUR_NONSPECIFIC) {
		*dur1 = (*mcc_intvl - (*dur2));
	} else {
		*dur2 = (*mcc_intvl - (*dur1));
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_set_dur_for_2_clients_mode(): Original dur1(%d), dur2(%d)\n",
		*dur1, *dur2);
	if (*dur1 < MIN_CLIENT_DUR) {
		*dur1 = MIN_CLIENT_DUR;
		*dur2 = (*mcc_intvl - (*dur1));
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_set_dur_for_2_clients_mode(): Core specific unsuitable duration, we adjust dur to dur1(%d) and dur2(%d)\n",
			*dur1, *dur2);
	} else if (*dur2 < MIN_CLIENT_DUR) {
		*dur2 = MIN_CLIENT_DUR;
		*dur1 = (*mcc_intvl - (*dur2));
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_set_dur_for_2_clients_mode(): Core specific unsuitable duration, we adjust dur to dur1(%d) and dur2(%d)\n",
			*dur1, *dur2);
	}
	if ((dur_i_1->dur_limit.enable) && (dur_i_1->dur_limit.max_dur != 0) &&
		(*dur1 > dur_i_1->dur_limit.max_dur)) {
		*dur1 = dur_i_1->dur_limit.max_dur;
		*dur2 = (*mcc_intvl - (*dur1));
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_set_dur_for_2_clients_mode(): dur1 > max_dur(%d), we adjust dur to dur1(%d) and dur2(%d)\n",
			dur_i_1->dur_limit.max_dur, *dur1, *dur2);
	}
	if ((dur_i_2->dur_limit.enable) && (dur_i_2->dur_limit.max_dur != 0) &&
		(*dur2 > dur_i_2->dur_limit.max_dur)) {
		*dur2 = dur_i_2->dur_limit.max_dur;
		*dur1 = (*mcc_intvl - (*dur2));
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_set_dur_for_2_clients_mode(): dur2 > max_dur(%d), we adjust dur to dur1(%d) and dur2(%d)\n",
			dur_i_2->dur_limit.max_dur, *dur1, *dur2);
	}
}

enum rtw_phl_status _mcc_fill_info_for_2_clients_mode(struct phl_info_t *phl,
						struct phl_mcc_info *minfo)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_role *role_ref = get_ref_role(en_info);
	struct rtw_phl_mcc_role *role_ano = (role_ref == &en_info->mcc_role[0])
				? &en_info->mcc_role[1] : &en_info->mcc_role[0];
	u16 bcns_offset = 0;
	bool worsecase = false;

	en_info->mcc_intvl = role_ref->bcn_intvl;
	if (RTW_PHL_STATUS_SUCCESS != _mcc_get_2_clients_bcn_offset(phl,
					&bcns_offset, role_ref, role_ano)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_info_for_2_clients_mode(): Get bcn offset fail\n");
		goto exit;
	}
	en_info->m_pattern.role_ref = role_ref;
	en_info->m_pattern.role_ano = role_ano;
	en_info->m_pattern.bcns_offset = bcns_offset;
	if ((bcns_offset < MIN_BCNS_OFFSET) ||
		(bcns_offset > (role_ref->bcn_intvl - MIN_BCNS_OFFSET))) {
		worsecase = true;
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_fill_info_for_2_clients_mode(): worsecase, bcns_offset(%d) < %d or bcns_offset > %d\n",
			bcns_offset, MIN_BCNS_OFFSET,
			(role_ref->bcn_intvl - MIN_BCNS_OFFSET));
	}
	_mcc_set_dur_for_2_clients_mode(&role_ref->policy.dur_info,
					&role_ano->policy.dur_info,
					&en_info->mcc_intvl);
	if (_mcc_fill_2wrole_pattern_with_limitation(minfo, role_ref, role_ano)) {
		/*wifi slot, bt slot, and only 1 wifi slot with limitation*/
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_info_for_2_clients_mode(): _mcc_fill_2wrole_pattern_with_limitation\n");
	} else if (_mcc_discision_duration_for_2role_bt(minfo)) {
		_mcc_fill_2_wrole_bt_pattern(minfo, role_ref, role_ano);
	} else {
	/*We only adjust dur for all wifi slot in worsecase*/
		if (worsecase) {
			_mcc_set_worsecase_dur_for_2_clients_mode(
				&role_ref->policy.dur_info,
				&role_ano->policy.dur_info, &en_info->mcc_intvl);
		}
		_mcc_fill_2_clients_pattern(minfo, worsecase, role_ref, role_ano);
	}
	if (RTW_PHL_STATUS_SUCCESS != _mcc_calculate_start_tsf(phl, en_info)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_info_for_2_clients_mode(): calculate start tsf fail\n");
		goto exit;
	}
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

void _mcc_set_dur_for_ap_client_mode(u16 *ap_dur, u16 *client_dur, u16 mcc_intvl)
{
	if (*ap_dur == MCC_DUR_NONSPECIFIC)
		*ap_dur = mcc_intvl - *client_dur;
	if (*ap_dur < MIN_AP_DUR) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_set_dur_for_ap_client_mode(): ap_dur(%d) < MIN_AP_DUR(%d), set ap_dur = MIN_AP_DUR\n",
			*ap_dur, MIN_AP_DUR);
		*ap_dur = MIN_AP_DUR;
	} else if (*ap_dur > (mcc_intvl - MIN_CLIENT_DUR)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "_mcc_set_dur_for_ap_client_mode(): ap_dur(%d) < MAX_AP_DUR(%d), set ap_dur = MAX_AP_DUR\n",
			*ap_dur, (mcc_intvl - MIN_CLIENT_DUR));
		*ap_dur = mcc_intvl - MIN_CLIENT_DUR;
	}
	*client_dur = mcc_intvl - *ap_dur;
}

enum rtw_phl_status _mcc_fill_info_for_ap_client_mode(
			struct phl_info_t *phl, struct phl_mcc_info *minfo)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_sync_tsf_info *sync_info = &en_info->sync_tsf_info;
	struct rtw_phl_mcc_role *ap_role = NULL;
	struct rtw_phl_mcc_role *client_role = NULL;
	struct rtw_phl_mcc_role *role_ref = get_ref_role(en_info);
	struct rtw_phl_mcc_role *role_ano = (role_ref == &en_info->mcc_role[0])
				? &en_info->mcc_role[1] : &en_info->mcc_role[0];

	ap_role = _mcc_get_mrole_by_category(en_info, MCC_ROLE_AP_CAT);
	client_role = _mcc_get_mrole_by_category(en_info, MCC_ROLE_CLIENT_CAT);
	if (ap_role == NULL || client_role == NULL) {
		_mcc_dump_en_info(en_info);
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_info_for_ap_client_mode(): (ap_role == NULL || client_role == NULL)\n");
		goto exit;
	}
	en_info->m_pattern.role_ref = role_ref;
	en_info->m_pattern.role_ano = role_ano;
	en_info->mcc_intvl = ap_role->bcn_intvl;
	_mcc_set_dur_for_ap_client_mode(&ap_role->policy.dur_info.dur,
					&client_role->policy.dur_info.dur,
					en_info->mcc_intvl);
	// TODO: Random offset should be used here.
	if (_mcc_discision_duration_for_2role_bt(minfo)) {
		en_info->m_pattern.bcns_offset = AP_CLIENT_OFFSET;
		_mcc_fill_2_wrole_bt_pattern(minfo, role_ref, role_ano);
	} else {
		_mcc_set_ap_client_default_pattern(minfo,
					&en_info->m_pattern.bcns_offset);
	}

	if (RTW_PHL_STATUS_SUCCESS != rtw_phl_tbtt_sync(phl,
	                                                client_role->wrole,
	                                                client_role->rlink,
	                                                ap_role->wrole,
	                                                ap_role->rlink,
	                                                en_info->m_pattern.bcns_offset,
	                                                true,
	                                                &sync_info->offset)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_info_for_ap_client_mode(): Sync tsf fail\n");
		goto exit;
	}

	sync_info->source = client_role->macid;
	sync_info->target = ap_role->macid;
	sync_info->sync_en = true;
	if (RTW_PHL_STATUS_SUCCESS != _mcc_calculate_start_tsf(phl, en_info)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_info_for_ap_client_mode(): calculate start tsf fail\n");
		goto exit;
	}
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

/*
 * |         Wifi slot          |          BT slot         |
 * <tob_r> Bcn_r <toa_r>
**/
enum rtw_phl_status _mcc_fill_info_for_ap_bt_mode(
			struct phl_info_t *phl, struct phl_mcc_info *minfo)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
	struct rtw_phl_mcc_role *role_ref = get_ref_role(en_info);
	struct rtw_phl_mcc_pattern *m_pattern = &en_info->m_pattern;

	minfo->bt_info.add_bt_role = true;
	en_info->mcc_intvl = role_ref->bcn_intvl;
	role_ref->policy.dur_info.dur = (u8)(en_info->mcc_intvl -
							minfo->bt_info.bt_dur);
	m_pattern->role_ref = role_ref;
	m_pattern->toa_r= role_ref->policy.dur_info.dur / 2;
	m_pattern->tob_r = role_ref->policy.dur_info.dur - m_pattern->toa_r;
	_mcc_fill_slot_info(m_pattern, false, role_ref->policy.dur_info.dur, role_ref);
	_mcc_fill_slot_info(m_pattern, true, minfo->bt_info.bt_dur, NULL);
	if (RTW_PHL_STATUS_SUCCESS != _mcc_calculate_start_tsf(phl, en_info)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_fill_info_for_ap_bt_mode(): calculate start tsf fail\n");
		goto exit;
	}
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

enum rtw_phl_status _mcc_pkt_offload_for_client(struct phl_info_t *phl, u8 macid)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	struct rtw_pkt_ofld_null_info null_info = {0};
	void *d = phl_to_drvpriv(phl);
	u32 null_token = 0;

	phl_sta = rtw_phl_get_stainfo_by_macid(phl, macid);
	if (phl_sta == NULL) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_pkt_offload_for_client(): get sta fail, macid(%d)\n",
			macid);
		goto exit;
	}
	if (NOT_USED != phl_pkt_ofld_get_id(phl, macid,
						PKT_TYPE_NULL_DATA)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_pkt_offload_for_client(): macid(%d), we had already offload NULL Pkt\n",
			macid);
		status = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	}
	_os_mem_cpy(d, &(null_info.a1[0]), &(phl_sta->mac_addr[0]),
		MAC_ADDRESS_LENGTH);
	_os_mem_cpy(d,&(null_info.a2[0]), &(phl_sta->wrole->mac_addr[0]),
		MAC_ADDRESS_LENGTH);
	_os_mem_cpy(d, &(null_info.a3[0]), &(phl_sta->mac_addr[0]),
		MAC_ADDRESS_LENGTH);
	if (RTW_PHL_STATUS_SUCCESS != rtw_phl_pkt_ofld_request(phl, macid,
						PKT_TYPE_NULL_DATA, &null_token, &null_info, __func__)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_pkt_offload_for_client(): Pkt offload fail, macid(%d)\n",
			macid);
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_pkt_offload_for_client(): offload ok, macid(%d), null_token(%d)\n",
		macid, null_token);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}


enum rtw_phl_status _mcc_pkt_offload(struct phl_info_t *phl,
					struct rtw_phl_mcc_en_info *info)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_mcc_role *mcc_role = NULL;
	u8 midx = 0;

	for (midx = 0; midx < MCC_ROLE_NUM; midx++) {
		if (!(info->mrole_map & BIT(midx)))
			continue;
		mcc_role = &info->mcc_role[midx];
		if (rtw_phl_role_is_client_category(mcc_role->wrole)) {
			if (RTW_PHL_STATUS_SUCCESS !=
				_mcc_pkt_offload_for_client(phl,
							(u8)mcc_role->macid)) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_pkt_offload_for_client(): mcc_role index(%d)\n",
					midx);
				goto exit;
			}
		}
	}
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

enum rtw_phl_status _mcc_update_2_clients_pattern(struct phl_info_t *phl,
				struct phl_mcc_info *ori_minfo,
				struct phl_mcc_info *new_minfo)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (RTW_PHL_STATUS_SUCCESS != _mcc_fill_info_for_2_clients_mode(phl,
								new_minfo)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_2_clients_pattern(): fill info fail for 2clients mode\n");
		goto error;
	}
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mcc_change_pattern(phl->hal,
					&ori_minfo->en_info, &new_minfo->en_info,
					&new_minfo->bt_info)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_2_clients_pattern(): set duration fail\n");
		goto error;
	}
	status = RTW_PHL_STATUS_SUCCESS;
	goto exit;
error:
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_2_clients_pattern(): Update fail\n");
exit:
	return status;
}

enum rtw_phl_status _mcc_update_ap_client_pattern(struct phl_info_t *phl,
		struct phl_mcc_info *ori_minfo, struct phl_mcc_info *new_minfo)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (RTW_PHL_STATUS_SUCCESS != _mcc_fill_info_for_ap_client_mode(phl,
								new_minfo)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_ap_client_pattern(): fill info fail for ap client mode\n");
		goto error;
	}
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mcc_change_pattern(phl->hal,
					&ori_minfo->en_info, &new_minfo->en_info,
					&new_minfo->bt_info)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_ap_client_pattern(): set duration fail\n");
		goto error;
	}
	if ((new_minfo->en_info.sync_tsf_info.sync_en) &&
		(new_minfo->en_info.sync_tsf_info.offset !=
		ori_minfo->en_info.sync_tsf_info.offset)) {
		if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mcc_sync_enable(phl->hal,
							&new_minfo->en_info)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_ap_client_pattern(): set tsf sync fail\n");
			goto error;
		}
	}
	_mcc_up_noa(phl, new_minfo);
	status = RTW_PHL_STATUS_SUCCESS;
	goto exit;
error:
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_ap_client_pattern(): Update fail\n");
exit:
	return status;
}

enum rtw_phl_status _mcc_update_ap_bt_pattern(struct phl_info_t *phl,
		struct phl_mcc_info *ori_minfo, struct phl_mcc_info *new_minfo)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (RTW_PHL_STATUS_SUCCESS != _mcc_fill_info_for_ap_bt_mode(phl,
								new_minfo)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_ap_bt_pattern(): fill info fail for ap_bt\n");
		goto error;
	}
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mcc_change_pattern(phl->hal,
					&ori_minfo->en_info, &new_minfo->en_info,
					&new_minfo->bt_info)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_ap_bt_pattern(): set duration fail\n");
		goto error;
	}
	_mcc_up_noa(phl, new_minfo);
	status = RTW_PHL_STATUS_SUCCESS;
	goto exit;
error:
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_update_ap_bt_pattern(): Update fail\n");
exit:
	return status;
}

enum rtw_phl_status _mcc_duration_change(struct phl_info_t *phl,
		struct phl_mcc_info *minfo, struct phl_mcc_info *new_minfo)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (RTW_PHL_TDMRA_AP_CLIENT_WMODE == minfo->mcc_mode) {
		if (RTW_PHL_STATUS_SUCCESS != _mcc_update_ap_client_pattern(
					phl, minfo, new_minfo)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_duration_change(): update ap_client fail\n");
			goto exit;
		}
	} else if (RTW_PHL_TDMRA_2CLIENTS_WMODE == minfo->mcc_mode) {
		if (RTW_PHL_STATUS_SUCCESS != _mcc_update_2_clients_pattern(
							phl, minfo, new_minfo)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_duration_change(): update ap_client fail\n");
			goto exit;
		}
	} else if (RTW_PHL_TDMRA_AP_WMODE == minfo->mcc_mode) {
		if (RTW_PHL_STATUS_SUCCESS != _mcc_update_ap_bt_pattern(
							phl, minfo, new_minfo)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_duration_change(): update ap_bt fail\n");
			goto exit;
		}
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_duration_change(): Undefined mcc_mode(%d)\n",
			minfo->mcc_mode);
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_duration_change(): Update success\n");
	_os_mem_cpy(phl_to_drvpriv(phl), minfo, new_minfo,
			sizeof(struct phl_mcc_info));
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

void _mcc_2_clients_tracking(struct phl_info_t *phl,
				struct phl_mcc_info *minfo
)
{
	struct rtw_phl_mcc_en_info *en_info = NULL;
	struct rtw_phl_mcc_pattern *m_pattern = NULL;
	struct rtw_phl_mcc_role *role_ref = NULL;
	struct rtw_phl_mcc_role *role_ano = NULL;
	struct phl_mcc_info new_minfo = {0};
	u16 bcns_offset = 0, diff = 0, tol = 0;/*tolerance*/
	bool negative_sign = false;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> _mcc_2_clients_tracking\n");
	_os_mem_cpy(phl_to_drvpriv(phl), &new_minfo, minfo,
			sizeof(struct phl_mcc_info));
	en_info = &new_minfo.en_info;
	m_pattern = &en_info->m_pattern;
	role_ref = get_ref_role(en_info);
	role_ano = (role_ref == &en_info->mcc_role[0]) ? &en_info->mcc_role[1] :
							&en_info->mcc_role[0];
	if (RTW_PHL_STATUS_SUCCESS != _mcc_get_2_clients_bcn_offset(phl,
					&bcns_offset, role_ref, role_ano)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_2_clients_tracking(): Get bcn offset fail\n");
		goto exit;
	}
	if (bcns_offset > m_pattern->bcns_offset) {
		diff = bcns_offset - m_pattern->bcns_offset;
	} else {
		diff = m_pattern->bcns_offset - bcns_offset;
		negative_sign = true;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_2_clients_tracking(): old bcns_offset(%d), new bcns_offset(%d)\n",
		m_pattern->bcns_offset, bcns_offset);
	_mcc_dump_pattern(m_pattern);
	_mcc_dump_ref_role_info(en_info);
	if (en_info->m_pattern.courtesy_i.c_en) {
		tol = CLIENTS_TRACKING_COURTESY_TH;
		goto decision;
	}
	if (en_info->mcc_intvl == WORSECASE_INTVL) {
		tol = CLIENTS_TRACKING_WORSECASE_TH;
		goto decision;
	}
	if (negative_sign) {
		if (m_pattern->tob_a <= EARLY_RX_BCN_T) {
			tol = CLIENTS_TRACKING_CRITICAL_POINT_TH;
		} else if (m_pattern->tob_a >= (2 * EARLY_RX_BCN_T)){
			tol = m_pattern->tob_a - ((3 * EARLY_RX_BCN_T) / 2);
		} else {
			tol = CLIENTS_TRACKING_TH;
		}
	} else {
		if (m_pattern->toa_a <= MIN_RX_BCN_T) {
			tol = CLIENTS_TRACKING_CRITICAL_POINT_TH;
		} else if (m_pattern->toa_a >= (2 * MIN_RX_BCN_T)){
			tol = m_pattern->toa_a - ((3 * MIN_RX_BCN_T) / 2);
		} else {
			tol = CLIENTS_TRACKING_TH;
		}
	}
decision:
	if (diff < tol)
		goto exit;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_2_clients_tracking(): Need to update new 2clients pattern, negative_sign(%d), diff(%d), tolerance(%d), mcc_intvl(%d)\n",
		negative_sign, diff, tol, en_info->mcc_intvl);
	_mcc_reset_minfo(phl, &new_minfo, MINFO_RESET_BT_INFO |
						MINFO_RESET_PATTERN_INFO);
	/*fill original bt slot*/
	_mcc_fill_bt_dur(phl, role_ref->rlink->hw_band, &new_minfo);

	/*get original wifi time slot*/
	_mcc_fill_mcc_role_policy_info(phl, role_ref->wrole, role_ref->rlink, role_ref);
	_mcc_fill_mcc_role_policy_info(phl, role_ano->wrole, role_ano->rlink, role_ano);

	if (RTW_PHL_STATUS_SUCCESS != _mcc_update_2_clients_pattern(phl,
							minfo, &new_minfo)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_2_clients_tracking(): update 2clients fail\n");
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_2_clients_tracking(): update new pattern ok\n");
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_2_clients_tracking(): old pattern:\n");
	_mcc_dump_pattern(&en_info->m_pattern);
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_mcc_2_clients_tracking(): new pattern:\n");
	_mcc_dump_pattern(&new_minfo.en_info.m_pattern);
	_os_mem_cpy(phl_to_drvpriv(phl), minfo, &new_minfo,
			sizeof(struct phl_mcc_info));
exit:
	return;
}

enum rtw_phl_status
rtw_phl_mcc_ap_bt_coex_enable(struct phl_info_t *phl,
                              struct rtw_wifi_role_t *cur_role,
                              struct rtw_wifi_role_link_t *cur_rlink)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_mcc_info *minfo = NULL;
	struct rtw_phl_mcc_en_info *en_info = NULL;
	struct hw_band_ctl_t *band_ctrl = NULL;
	u8 hw_band = 0;

	hw_band = cur_rlink->hw_band;
	band_ctrl = get_band_ctrl(phl, hw_band);

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_mcc_ap_bt_coex_enable(): cur_role->type(%d)\n",
		cur_role->type);
	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_ap_bt_coex_enable(): mcc is not init, please check code\n");
		goto exit;
	}
	minfo = get_mcc_info(phl, hw_band);
	en_info = &minfo->en_info;
	if (MCC_NONE != minfo->state && MCC_STOP != minfo->state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_ap_bt_coex_enable(): (MCC_NONE != minfo->state || MCC_STOP != minfo->state(%d)), please check code flow\n",
			minfo->state);
		_mcc_dump_state(&minfo->state);
		goto exit;
	}
	_mcc_set_state(minfo, MCC_CFG_EN_INFO);
	_mcc_reset_minfo(phl, minfo, (MINFO_RESET_EN_INFO | MINFO_RESET_MODE |
				MINFO_RESET_ROLE_MAP | MINFO_RESET_COEX_MODE |
				MINFO_RESET_BT_INFO));
	if (RTW_PHL_STATUS_SUCCESS != _mcc_get_role_map(phl, band_ctrl,
							&minfo->role_map)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_ap_bt_coex_enable(): Get role map failed\n");
		goto _cfg_info_fail;
	}
	if (RTW_PHL_STATUS_SUCCESS != _mcc_transfer_mode(phl, minfo,
							minfo->role_map)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_ap_bt_coex_enable(): transfer mcc mode failed\n");
		goto _cfg_info_fail;
	}
	if (RTW_PHL_TDMRA_AP_WMODE != minfo->mcc_mode) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_ap_bt_coex_enable(): error  wmode\n");
		_mcc_dump_mode(&minfo->mcc_mode);
		goto _cfg_info_fail;
	}

	if (RTW_PHL_STATUS_SUCCESS != _mcc_fill_role_info(phl,
	                                                  en_info,
	                                                  minfo->role_map,
	                                                  cur_role,
	                                                  cur_rlink)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_ap_bt_coex_enable(): fill role info failed\n");
		goto _cfg_info_fail;
	}

	_mcc_fill_coex_mode(phl, minfo);
	_mcc_fill_bt_dur(phl, cur_rlink->hw_band, minfo);
	if (RTW_PHL_STATUS_SUCCESS != _mcc_get_mrole_idx_by_wrole(minfo,
					cur_role, &en_info->ref_role_idx)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_ap_bt_coex_enable(): fill ref_role idx failed\n");
		goto _cfg_info_fail;
	}
	if (RTW_PHL_STATUS_SUCCESS != _mcc_fill_info_for_ap_bt_mode(phl, minfo)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_ap_bt_coex_enable(): fill ref_role idx failed\n");
		goto _cfg_info_fail;
	}
	_mcc_set_state(minfo, MCC_TRIGGER_FW_EN);
	if (rtw_hal_mcc_enable(phl->hal, en_info, &minfo->bt_info,
				minfo->mcc_mode) != RTW_HAL_STATUS_SUCCESS) {
		_mcc_set_state(minfo, MCC_FW_EN_FAIL);
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_ap_bt_coex_enable(): Enable FW mcc Fail\n");
		goto exit;
	}
	_mcc_set_state(minfo, MCC_RUNING);
	PHL_TRACE(COMP_PHL_MCC, _PHL_ALWAYS_, "rtw_phl_mcc_ap_bt_coex_enable(): Enable FW mcc ok\n");
	_mcc_up_noa(phl, minfo);
	_mcc_dump_mcc_info(minfo);
	_mcc_up_fw_log_setting(phl, minfo);
	status = RTW_PHL_STATUS_SUCCESS;
	goto exit;
_cfg_info_fail:
	_mcc_set_state(minfo, MCC_NONE);
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_ALWAYS_, "<<< rtw_phl_mcc_ap_bt_coex_enable():status(%d)\n",
		status);
	return status;
}

enum rtw_phl_status rtw_phl_mcc_go_bt_coex_disable(struct phl_info_t *phl,
				struct rtw_wifi_role_t *spec_role)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_mcc_info *minfo = NULL;
	struct rtw_phl_mcc_en_info *en_info = NULL;
	struct rtw_phl_mcc_role *m_role = NULL;
	u8 hw_band = 0;

	if (spec_role->rlink_num > 1) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: MLD not support\n", __func__);
		goto exit;
	}

	hw_band = spec_role->rlink[spec_role->rlink_num-1].hw_band;

	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_go_bt_coex_disable(): mcc is not init, please check code\n");
		goto exit;
	}
	minfo = get_mcc_info(phl, hw_band);
	en_info = &minfo->en_info;
	if (MCC_RUNING != minfo->state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_go_bt_coex_disable(): MCC_RUNING != m_info->state, please check code flow\n");
		_mcc_dump_state(&minfo->state);
		goto exit;
	}
	if (NULL == (m_role = _mcc_get_mrole_by_wrole(minfo, spec_role))) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_go_bt_coex_disable(): Can't get mrole, wrole id(%d), please check code flow\n",
			spec_role->id);
		goto exit;
	}
	_mcc_set_state(minfo, MCC_TRIGGER_FW_DIS);
	if (rtw_hal_mcc_disable(phl->hal, m_role->group, m_role->macid,
				minfo->mcc_mode) != RTW_HAL_STATUS_SUCCESS) {
		status = RTW_PHL_STATUS_FAILURE;
		_mcc_set_state(minfo, MCC_FW_DIS_FAIL);
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_,"rtw_phl_mcc_go_bt_coex_disable(): Disable FW mcc Fail\n");
		goto exit;
	}

	rtw_hal_sync_cur_ch(phl->hal, hw_band, spec_role->rlink[spec_role->rlink_num-1].chandef);

	_mcc_set_state(minfo, MCC_STOP);
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_go_bt_coex_disable(): Disable FW mcc ok\n");
	_mcc_clean_noa(phl, en_info);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_ALWAYS_, "<<< rtw_phl_mcc_go_bt_coex_disable(): status(%d)\n",
		status);
	return status;
}

void rtw_phl_mcc_watchdog(struct phl_info_t *phl, u8 band_idx)
{
	struct phl_mcc_info *minfo = NULL;

	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_watchdog(): mcc is not init, please check code\n");
		goto exit;
	}
	minfo = get_mcc_info(phl, band_idx);
	if (MCC_RUNING != minfo->state)
		goto exit;

	if (RTW_PHL_TDMRA_2CLIENTS_WMODE == minfo->mcc_mode)
		_mcc_2_clients_tracking(phl, minfo);

exit:
	return;
}

enum rtw_phl_status rtw_phl_mcc_duration_change(struct phl_info_t *phl,
			struct phl_tdmra_dur_change_info *info)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_mcc_info new_minfo = {0};
	struct rtw_phl_mcc_role *spec_mrole = NULL;
	struct phl_mcc_info *minfo = get_mcc_info(phl, info->hw_band);

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_mcc_duration_change\n");
	_os_mem_cpy(phl_to_drvpriv(phl), &new_minfo, minfo,
			sizeof(struct phl_mcc_info));
	_mcc_set_unspecific_dur(&new_minfo);
	spec_mrole = _mcc_get_mrole_by_wrole(&new_minfo, info->role);
	if (NULL == spec_mrole) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_duration_change(): Can't get mrole by wrole(%d), please check code flow\n",
			info->role->id);
		goto exit;
	}
	spec_mrole->policy.dur_info.dur = info->dur;
	if (RTW_PHL_STATUS_SUCCESS != _mcc_duration_change(phl, minfo,
								&new_minfo)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_duration_change(): Change fail\n");
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_duration_change(): Change success\n");
	status = RTW_PHL_STATUS_SUCCESS;
	_mcc_dump_mcc_info(&new_minfo);
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< rtw_phl_mcc_duration_change\n");
	return status;
}

enum rtw_phl_status rtw_phl_mcc_bt_duration_change(struct phl_info_t *phl,
				struct phl_tdmra_dur_change_info *info)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_mcc_info *minfo = get_mcc_info(phl, info->hw_band);
	struct phl_mcc_info new_minfo = {0};
	struct rtw_phl_mcc_en_info *en_info = NULL;
	struct rtw_phl_mcc_role *mrole = NULL;
	u8 midx = 0;
	bool exist_2g = false;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_mcc_bt_duration_change(): dur(%d)\n",
		info->dur);
	if (info->dur == minfo->bt_info.bt_dur) {
		status = RTW_PHL_STATUS_SUCCESS;
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_phl_mcc_bt_duration_change(): no change of bt slot(%d), skip it\n",
			info->dur);
		goto exit;
	}
	_os_mem_cpy(phl_to_drvpriv(phl), &new_minfo, minfo,
			sizeof(struct phl_mcc_info));
	en_info = &new_minfo.en_info;
	_mcc_reset_minfo(phl, &new_minfo, MINFO_RESET_BT_INFO |
						MINFO_RESET_PATTERN_INFO);
	_mcc_fill_bt_dur(phl, info->hw_band, &new_minfo);
	/*fill original wifi time slot*/
	for (midx = 0; midx < en_info->mrole_num; midx++) {
		if (!(en_info->mrole_map & BIT(midx)))
			continue;

		mrole = &en_info->mcc_role[midx];
		_mcc_fill_mcc_role_policy_info(phl, mrole->wrole, mrole->rlink, mrole);

		if (mrole->chandef->band == BAND_ON_24G)
			exist_2g = true;
	}
	if (false == exist_2g) {
		status = RTW_PHL_STATUS_SUCCESS;
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_phl_mcc_bt_duration_change(): All 5G, Don't care BT duration\n");
		goto exit;
	}
	if (RTW_PHL_STATUS_SUCCESS != _mcc_duration_change(phl, minfo,
								&new_minfo)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_bt_duration_change(): Change fail\n");
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_bt_duration_change(): Change success\n");
	status = RTW_PHL_STATUS_SUCCESS;
	_mcc_dump_mcc_info(&new_minfo);
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< rtw_phl_mcc_bt_duration_change(): status(%d)\n",
		status);
	return status;
}

enum rtw_phl_status rtw_phl_mcc_dur_lim_change(struct phl_info_t *phl,
				struct rtw_wifi_role_t *wrole,
				struct phl_mcc_dur_lim_req_info *lim_req)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_mcc_info *minfo = NULL;
	struct phl_mcc_info new_minfo = {0};
	struct rtw_phl_mcc_en_info *en_info = NULL;
	struct rtw_phl_mcc_role *spec_mrole = NULL;
	struct rtw_phl_mcc_role *mrole = NULL;
	u8 midx = 0, hw_band = 0;

	if (wrole->rlink_num > 1) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: MLD not support\n", __func__);
		goto exit;
	}

	hw_band = wrole->rlink[wrole->rlink_num-1].hw_band;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_mcc_dur_lim_change()\n");
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "_mcc_fill_dur_lim_info(): dur_req: tag(%d), enable(%d), start_t_h(0x%08x), start_t_l(0x%08x), dur(%d), intvl(%d)\n",
		lim_req->tag, lim_req->enable, lim_req->start_t_h,
		lim_req->start_t_l, lim_req->dur, lim_req->intvl);

	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_dur_lim_change(): mcc is not init, please check code\n");
		goto exit;
	}
	minfo = get_mcc_info(phl, hw_band);
	if (MCC_RUNING != minfo->state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_dur_lim_change(): MCC_RUNING != minfo->state, , please check code flow\n");
		_mcc_dump_state(&minfo->state);
		goto exit;
	}
	_os_mem_cpy(phl_to_drvpriv(phl), &new_minfo, minfo,
			sizeof(struct phl_mcc_info));
	spec_mrole = _mcc_get_mrole_by_wrole(&new_minfo, wrole);
	if (NULL == spec_mrole) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_dur_lim_change(): Can't get mrole by wrole(%d), please check code flow\n",
			wrole->id);
		goto exit;
	}
	en_info = &new_minfo.en_info;
	/*fill original bt slot*/
	_mcc_reset_minfo(phl, &new_minfo, MINFO_RESET_BT_INFO |
						MINFO_RESET_PATTERN_INFO);
	_mcc_fill_bt_dur(phl, hw_band, &new_minfo);
	/*fill original wifi time slot*/
	for (midx = 0; midx < en_info->mrole_num; midx++) {
		if (!(en_info->mrole_map & BIT(midx)))
			continue;

		mrole = &en_info->mcc_role[midx];
		_mcc_fill_mcc_role_policy_info(phl, mrole->wrole, mrole->rlink, mrole);
	}
	_mcc_fill_dur_lim_info(phl, spec_mrole, lim_req);
	if (RTW_PHL_STATUS_SUCCESS != _mcc_duration_change(phl, minfo,
								&new_minfo)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_dur_lim_change(): Change fail\n");
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_dur_lim_change(): Change success\n");
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

void rtw_phl_mcc_sta_entry_change(struct phl_info_t *phl,
					struct rtw_wifi_role_t *wrole)
{
	struct phl_mcc_info *minfo = NULL;
	struct rtw_phl_mcc_role *mrole = NULL;
	u8 hw_band = 0;

	if (wrole->rlink_num > 1) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: MLD not support\n", __func__);
		goto exit;
	}

	hw_band = wrole->rlink[wrole->rlink_num-1].hw_band;

	if (!is_mcc_init(phl)) {
		goto exit;
	}
	minfo = get_mcc_info(phl, hw_band);
	if (MCC_RUNING != minfo->state) {
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, ">>> rtw_phl_mcc_sta_entry_change\n");
	minfo = get_mcc_info(phl, hw_band);
	if (NULL == (mrole = _mcc_get_mrole_by_wrole(minfo, wrole))) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_sta_entry_change(): Can't get mrole, wrole id(%d), please check code flow\n",
			wrole->id);
		goto exit;
	}
	_mcc_fill_macid_bitmap_by_role(phl, mrole);
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mcc_update_macid_bitmap(
					phl->hal, mrole->group,
					mrole->macid, &mrole->used_macid)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_sta_entry_change(): Update macid map fail\n");
		goto exit;
	}
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_notify_mcc_macid(phl->hal,
	                                                       mrole,
	                                                       minfo->mcc_mode)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_sta_entry_change(): Notify macid map fail\n");
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_sta_entry_change(): Update macid map ok\n");
	_mcc_dump_mcc_info(minfo);
exit:
	return;
}

void phl_mcc_ap_client_notify(struct phl_info_t *phl,
			struct rtw_wifi_role_t *wrole, enum link_state state)
{
	struct phl_mcc_info *minfo = NULL;
	struct rtw_phl_mcc_role *mrole = NULL;
	u8 hw_band = 0;

	if (wrole->rlink_num > 1) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: MLD not support\n", __func__);
		goto exit;
	}

	hw_band = wrole->rlink[wrole->rlink_num-1].hw_band;

	if (state != PHL_ClIENT_JOINING &&
		state != PHL_ClIENT_LEFT)
		goto exit;
	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s(): mcc is not init, please check code\n",
			__func__);
		goto exit;
	}
	minfo = get_mcc_info(phl, hw_band);
	if (MCC_RUNING != minfo->state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s(): MCC_RUNING != minfo->state\n",
			__func__);
		_mcc_dump_state(&minfo->state);
		goto exit;
	}
	if (NULL == (mrole = _mcc_get_mrole_by_wrole(minfo, wrole))) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s(): Can't get mrole, wrole id(%d), please check code flow\n",
			__func__, wrole->id);
		goto exit;
	}
	_mcc_fill_macid_bitmap_by_role(phl, mrole);
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_mcc_update_macid_bitmap(
					phl->hal, mrole->group,
					mrole->macid, &mrole->used_macid)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s(): Update macid map fail\n",
			__func__);
		goto exit;
	}
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_notify_mcc_macid(phl->hal,
	                                                       mrole,
	                                                       minfo->mcc_mode)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s(): Notify macid map fail\n",
			__func__);
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s(): Update macid map ok\n",
		__func__);
	_mcc_dump_mcc_info(minfo);
exit:
	return;
}

bool rtw_phl_mcc_inprogress(struct phl_info_t *phl, u8 band_idx)
{
	bool ret = false;
	struct phl_mcc_info *minfo = NULL;

	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_inprogress(): mcc is not init, please check code\n");
		goto exit;
	}
	minfo = get_mcc_info(phl, band_idx);
	if (MCC_TRIGGER_FW_EN == minfo->state || MCC_RUNING == minfo->state ||
		MCC_TRIGGER_FW_DIS == minfo->state ||
		MCC_FW_DIS_FAIL == minfo->state) {
		ret = true;
	}
exit:
	return ret;
}

static bool _is_mcc_sts_err(struct phl_mcc_info *minfo)
{
	bool ret = false;

	if (MCC_FW_DIS_FAIL == minfo->state ||
		MCC_FW_EN_FAIL == minfo->state) {
		ret = true;
	}
	return ret;
}

static enum rtw_phl_status _mcc_reset(struct phl_info_t *phl,
					struct phl_mcc_info *minfo)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_mcc_en_info *en_info = NULL;

	FUNCIN();
	en_info = &minfo->en_info;

	/* Reset mcc */
	rtw_hal_mcc_reset(phl->hal, en_info->group, minfo->mcc_mode);
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_,
		"%s(): reset mcc group(%d) complete\n",
		__func__, en_info->group);
	/* Reset state */
	_mcc_set_state(minfo, MCC_NONE);

	if (minfo->mcc_mode == RTW_PHL_TDMRA_AP_CLIENT_WMODE ||
		minfo->mcc_mode == RTW_PHL_TDMRA_AP_WMODE)
		_mcc_clean_noa(phl, en_info);
	status = RTW_PHL_STATUS_SUCCESS;
	return status;
}

static enum rtw_phl_status _mcc_recovery(struct phl_info_t *phl,
					struct phl_mcc_info *minfo)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;

	if (_is_mcc_sts_err(minfo)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_,
			"%s: err status detected, try to reset mcc.\n",
			__func__);
		_mcc_reset(phl, minfo);
	}

	return status;
}

/* Enable Fw MCC
 * @cur_role: the role in the current ch.
 */
enum rtw_phl_status
rtw_phl_mcc_enable(struct phl_info_t *phl,
                   struct rtw_wifi_role_t *cur_role,
                   struct rtw_wifi_role_link_t *cur_rlink
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_mcc_info *minfo = NULL;
	struct rtw_phl_mcc_en_info *en_info = NULL;
	struct hw_band_ctl_t *band_ctrl = NULL;
	u8 role_map = 0;
	u8 hw_band = 0;

	hw_band = cur_rlink->hw_band;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_mcc_enable(): cur_role->type(%d)\n",
		cur_role->type);
	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): mcc is not init, please check code\n");
		goto exit;
	}
	band_ctrl = get_band_ctrl(phl, hw_band);
	minfo = get_mcc_info(phl, hw_band);
	en_info = &minfo->en_info;
	if (MCC_NONE != minfo->state && MCC_STOP != minfo->state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): (MCC_NONE != minfo->state || MCC_STOP != minfo->state(%d)), please check code flow\n",
			minfo->state);
		_mcc_dump_state(&minfo->state);
		goto exit;
	}
	_mcc_set_state(minfo, MCC_CFG_EN_INFO);
	if (RTW_PHL_STATUS_SUCCESS != _mcc_get_role_map(phl, band_ctrl,
							&role_map)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): Get role map failed\n");
		goto _cfg_info_fail;
	}
	_mcc_reset_minfo(phl, minfo, (MINFO_RESET_EN_INFO | MINFO_RESET_MODE |
				MINFO_RESET_ROLE_MAP | MINFO_RESET_COEX_MODE |
				MINFO_RESET_BT_INFO));
	minfo->role_map = role_map;
	if (RTW_PHL_STATUS_SUCCESS != _mcc_transfer_mode(phl, minfo,
							role_map)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): transfer mcc mode failed\n");
		goto _cfg_info_fail;
	}

	if (RTW_PHL_STATUS_SUCCESS != _mcc_fill_role_info(phl,
	                                                  en_info,
	                                                  role_map,
	                                                  cur_role,
	                                                  cur_rlink)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): fill role info failed\n");
		goto _cfg_info_fail;
	}

	_mcc_fill_coex_mode(phl, minfo);
	_mcc_fill_bt_dur(phl, cur_rlink->hw_band, minfo);
	if (RTW_PHL_STATUS_SUCCESS != _mcc_get_mrole_idx_by_wrole(minfo,
					cur_role, &en_info->ref_role_idx)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): fill ref_role idx failed\n");
		goto _cfg_info_fail;
	}
	if (minfo->mcc_mode == RTW_PHL_TDMRA_AP_CLIENT_WMODE) {
		if (RTW_PHL_STATUS_SUCCESS !=
			_mcc_fill_info_for_ap_client_mode(phl, minfo)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): fill info failed for ap_client mode\n");
			goto _cfg_info_fail;
		}
	} else if (minfo->mcc_mode == RTW_PHL_TDMRA_2CLIENTS_WMODE){
		if (RTW_PHL_STATUS_SUCCESS !=
			_mcc_fill_info_for_2_clients_mode(phl, minfo)) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): fill info failed for 2clients mode\n");
			goto _cfg_info_fail;
		}
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): Undefined mcc_mode(%d)\n",
			minfo->mcc_mode);
		goto _cfg_info_fail;
	}
	_mcc_dump_mcc_info(minfo);
	if (RTW_PHL_STATUS_SUCCESS != _mcc_pkt_offload(phl, en_info)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): pkt offload Fail\n");
		goto _cfg_info_fail;
	}
	_mcc_set_state(minfo, MCC_TRIGGER_FW_EN);
	if (rtw_hal_mcc_enable(phl->hal, en_info, &minfo->bt_info,
				minfo->mcc_mode) != RTW_HAL_STATUS_SUCCESS) {
		_mcc_set_state(minfo, MCC_FW_EN_FAIL);
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_enable(): Enable FW mcc Fail\n");
		goto exit;
	}
	_mcc_set_state(minfo, MCC_RUNING);
	PHL_TRACE(COMP_PHL_MCC, _PHL_ALWAYS_, "rtw_phl_mcc_enable(): Enable FW mcc ok\n");
	if (minfo->mcc_mode == RTW_PHL_TDMRA_AP_CLIENT_WMODE)
		_mcc_up_noa(phl, minfo);
	_mcc_dump_mcc_info(minfo);
	_mcc_up_fw_log_setting(phl, minfo);
	status = RTW_PHL_STATUS_SUCCESS;
	goto exit;
_cfg_info_fail:
	_mcc_set_state(minfo, MCC_NONE);
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "<<< rtw_phl_mcc_enable():status(%d)\n",
		status);
	return status;
}

/*
 * Stop fw mcc
 * @ spec_role: You want to fw switch ch to the specific ch of the role when fw stop mcc
 */
enum rtw_phl_status
rtw_phl_mcc_disable(struct phl_info_t *phl,
                    struct rtw_wifi_role_t *spec_role,
                    struct rtw_wifi_role_link_t *spec_rlink
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_mcc_info *minfo = NULL;
	struct rtw_phl_mcc_en_info *en_info = NULL;
	struct rtw_phl_mcc_role *m_role = NULL;
	u8 hw_band = 0;

	hw_band = spec_rlink->hw_band;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_mcc_disable()\n");
	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_disable(): mcc is not init, please check code\n");
		goto exit;
	}
	minfo = get_mcc_info(phl, hw_band);
	en_info = &minfo->en_info;
	if (MCC_RUNING != minfo->state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_disable(): MCC_RUNING != m_info->state, please check code flow\n");
		_mcc_dump_state(&minfo->state);
		goto exit;
	}
	if (NULL == (m_role = _mcc_get_mrole_by_wrole(minfo, spec_role))) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_disable(): Can't get mrole, wrole id(%d), please check code flow\n",
			spec_role->id);
		goto exit;
	}
	_mcc_set_state(minfo, MCC_TRIGGER_FW_DIS);
	if (rtw_hal_mcc_disable(phl->hal, m_role->group, m_role->macid,
				minfo->mcc_mode) != RTW_HAL_STATUS_SUCCESS) {
		status = RTW_PHL_STATUS_FAILURE;
		_mcc_set_state(minfo, MCC_FW_DIS_FAIL);
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_,"rtw_phl_mcc_disable(): Disable FW mcc Fail\n");
		goto exit;
	}

	rtw_hal_sync_cur_ch(phl->hal, hw_band, spec_rlink->chandef);

	_mcc_set_state(minfo, MCC_STOP);
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_disable(): Disable FW mcc ok\n");
	if (minfo->mcc_mode == RTW_PHL_TDMRA_AP_CLIENT_WMODE)
		_mcc_clean_noa(phl, en_info);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}


enum rtw_phl_status rtw_phl_tdmra_duration_change(struct phl_info_t *phl,
			struct phl_tdmra_dur_change_info *info)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_mcc_info *minfo = NULL;
	struct rtw_phl_mcc_en_info *en_info = NULL;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_tdmra_duration_change()\n");
	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_tdmra_duration_change(): mcc is not init, please check code\n");
		goto exit;
	}
	minfo = get_mcc_info(phl, info->hw_band);
	en_info = &minfo->en_info;
	if (MCC_RUNING != minfo->state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_tdmra_duration_change(): MCC_RUNING != m_info->state, please check code flow\n");
		_mcc_dump_state(&minfo->state);
		goto exit;
	}
	if (true == info->bt_role) {
		status = rtw_phl_mcc_bt_duration_change(phl, info);
	} else {
		status = rtw_phl_mcc_duration_change(phl, info);
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< rtw_phl_tdmra_duration_change(): status(%d)\n",
		status);
exit:
	return status;
}

enum rtw_phl_status
rtw_phl_tdmra_enable(struct phl_info_t *phl,
                     struct rtw_wifi_role_t *cur_role,
                     struct rtw_wifi_role_link_t *cur_rlink
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct hw_band_ctl_t *band_ctrl = NULL;
	struct mr_info *cur_info = NULL;
	u8 chanctx_num = 0;
	u8 hw_band = 0;

	hw_band = cur_rlink->hw_band;
	band_ctrl = get_band_ctrl(phl, hw_band);
	cur_info = &band_ctrl->cur_info;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_tdmra_enable\n");
	chanctx_num = (u8)phl_mr_get_chanctx_num(phl, band_ctrl);
	if (2 == chanctx_num) {
		status = rtw_phl_mcc_enable(phl, cur_role, cur_rlink);
	} else if (1 == chanctx_num) {
		if ((1 == cur_info->ap_num || 1 == cur_info->p2p_go_num) &&
		   (cur_info->ld_sta_num == 0 || cur_info->lg_sta_num == 0)) {
			status = rtw_phl_mcc_ap_bt_coex_enable(phl, cur_role, cur_rlink);
		} else {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_tdmra_enable(): Not support this type\n");
			PHL_DUMP_MR_EX(phl);
		}
	} else {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_tdmra_enable(): Not support for chanctx_num(%d)\n",
			chanctx_num);
	}
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< rtw_phl_tdmra_enable(): status(%d)\n",
		status);
	return status;
}

/*
 * Stop tdmra
 * @ spec_role: You want to fw switch ch to the specific ch of the role when fw stop tdma
 */
enum rtw_phl_status
rtw_phl_tdmra_disable(struct phl_info_t *phl,
                      struct rtw_wifi_role_t *spec_role,
                      struct rtw_wifi_role_link_t *spec_rlink
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_mcc_info *minfo = NULL;
	struct rtw_phl_mcc_en_info *en_info = NULL;
	struct rtw_phl_mcc_role *m_role = NULL;
	u8 hw_band = 0;

	hw_band = spec_rlink->hw_band;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_tdmra_disable()\n");
	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_tdmra_disable(): mcc is not init, please check code\n");
		goto exit;
	}
	minfo = get_mcc_info(phl, hw_band);
	en_info = &minfo->en_info;
	if (MCC_RUNING != minfo->state) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_tdmra_disable(): MCC_RUNING != m_info->state, please check code flow\n");
		_mcc_dump_state(&minfo->state);
		goto exit;
	}
	if (NULL == (m_role = _mcc_get_mrole_by_wrole(minfo, spec_role))) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_tdmra_disable(): Can't get mrole, wrole id(%d), please check code flow\n",
			spec_role->id);
		#ifdef RTW_WKARD_TDMRA_AUTO_GET_STAY_ROLE
		m_role = get_ref_role(en_info);
		if (m_role == NULL) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s(): Can't get mrole from ref_mrole, please check code flow\n",
				__func__);
			goto exit;
		} else {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s(): Workaround get mrore from ref_mrole\n",
				__func__);
		}
		#else
		goto exit;
		#endif /* RTW_WKARD_TDMRA_AUTO_GET_STAY_ROLE */
	}
	_mcc_set_state(minfo, MCC_TRIGGER_FW_DIS);
	if (rtw_hal_mcc_disable(phl->hal, m_role->group, m_role->macid,
				minfo->mcc_mode) != RTW_HAL_STATUS_SUCCESS) {
		status = RTW_PHL_STATUS_FAILURE;
		_mcc_set_state(minfo, MCC_FW_DIS_FAIL);
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_,"rtw_phl_tdmra_disable(): Disable FW mcc Fail\n");
		goto exit;
	}
	rtw_hal_sync_cur_ch(phl->hal, hw_band, *m_role->chandef);
	_mcc_set_state(minfo, MCC_STOP);
	PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_tdmra_disable(): Disable FW mcc ok\n");
	if (minfo->mcc_mode == RTW_PHL_TDMRA_AP_CLIENT_WMODE ||
		minfo->mcc_mode == RTW_PHL_TDMRA_AP_WMODE)
		_mcc_clean_noa(phl, en_info);
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "<<< rtw_phl_tdmra_disable(): status(%d)\n",
		status);
	return status;
}

enum rtw_phl_status rtw_phl_tdmra_recovery(struct phl_info_t *phl,
					u8 band_idx)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_SUCCESS;
	struct phl_mcc_info *minfo = NULL;

	if (!is_mcc_init(phl)) {
		goto _exit;
	}
	minfo = get_mcc_info(phl, band_idx);
	if (minfo->state == MCC_RUNING) {
		struct rtw_phl_mcc_en_info *en_info = &minfo->en_info;
		struct rtw_phl_mcc_role *role_ref = get_ref_role(en_info);

		sts = rtw_phl_tdmra_disable(phl, role_ref->wrole, role_ref->rlink);
		if (sts != RTW_PHL_STATUS_SUCCESS)
			goto _exit;
		sts = rtw_phl_tdmra_enable(phl, role_ref->wrole, role_ref->rlink);
	} else {
		sts = _mcc_recovery(phl, minfo);
	}
_exit:
	return sts;
}

enum rtw_phl_status rtw_phl_mcc_init_ops(struct phl_info_t *phl, struct rtw_phl_mcc_ops *ops)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_com_mcc_info *com_info = NULL;

	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_init_ops(): mcc is not init, please check code\n");
		goto exit;
	}
	com_info = phl_to_com_mcc_info(phl);
	com_info->ops.priv = ops->priv;
	com_info->ops.mcc_update_noa = ops->mcc_update_noa;
	com_info->ops.mcc_get_setting = ops->mcc_get_setting;
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_phl_mcc_init_ops(): init ok\n");
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

enum rtw_phl_status rtw_phl_mcc_init(struct phl_info_t *phl)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_com_t *phl_com = phl->phl_com;
	void *drv = phl_to_drvpriv(phl);
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = NULL;
	u8 band_idx = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_mcc_init()\n");
	set_mcc_init_state(phl, false);
	if (mr_ctl->com_mcc == NULL) {
		mr_ctl->com_mcc = _os_mem_alloc(drv, sizeof(struct phl_com_mcc_info));
		if (mr_ctl->com_mcc != NULL) {
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_phl_mcc_init(): Allocate phl_com_mcc_info\n");
		} else {
			PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_init(): Allocate phl_com_mcc_info Fail\n");
			goto deinit;
		}
	}
	for (band_idx = 0; band_idx < MAX_BAND_NUM; band_idx++) {
		band_ctrl = &mr_ctl->band_ctrl[band_idx];
		if (band_ctrl->mcc_info == NULL) {
			band_ctrl->mcc_info = _os_mem_alloc(drv,
						sizeof(struct phl_mcc_info));
			if (band_ctrl->mcc_info != NULL) {
				PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_phl_mcc_init(): Allocate mcc_info for HW Band(%d)\n",
					band_idx);
			} else {
				PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_init(): Allocate mcc_info fail for HW Band(%d)\n",
					band_idx);
				goto deinit;
			}
		}
	}
	set_mcc_init_state(phl, true);
	status = RTW_PHL_STATUS_SUCCESS;
	goto exit;
deinit:
	rtw_phl_mcc_deinit(phl);
exit:
	return status;
}

void rtw_phl_mcc_deinit(struct phl_info_t *phl)
{
	struct rtw_phl_com_t *phl_com = phl->phl_com;
	void *drv = phl_to_drvpriv(phl);
	struct phl_com_mcc_info *com_info = phl_to_com_mcc_info(phl);
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = NULL;
	u8 band_idx = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, ">>> rtw_phl_mcc_deinit()\n");
	if (com_info != NULL) {
		_os_mem_free(drv, com_info, sizeof(struct phl_com_mcc_info));
		PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_phl_mcc_deinit(): Free phl_com_mcc_info\n");
	}
	for (band_idx = 0; band_idx < MAX_BAND_NUM; band_idx++) {
		band_ctrl = &mr_ctl->band_ctrl[band_idx];
		if (band_ctrl->mcc_info != NULL) {
			_os_mem_free(drv, band_ctrl->mcc_info,
						sizeof(struct phl_mcc_info));
			PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_phl_mcc_deinit(): Free phl_mcc_info, HwBand(%d)\n",
				band_idx);
		}
	}
	set_mcc_init_state(phl, false);
}

bool rtw_phl_mcc_get_dbg_info(struct phl_info_t *phl, u8 band_idx,
				enum rtw_phl_mcc_dbg_type type, void *info)
{
	bool get_ok = false;
	struct phl_mcc_info *minfo = NULL;
	struct rtw_phl_mcc_en_info *en_info = NULL;

	if (!is_mcc_init(phl)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "rtw_phl_mcc_get_dbg_info(): mcc is not init, please check code\n");
		goto exit;
	}
	minfo = get_mcc_info(phl, band_idx);
	en_info = &minfo->en_info;

	if (MCC_DBG_STATE == type) {
		*(enum rtw_phl_mcc_state *)info = minfo->state;
	} else if (MCC_DBG_OP_MODE == type) {
		*(enum rtw_phl_tdmra_wmode *)info = minfo->mcc_mode;
	} else if (MCC_DBG_COEX_MODE == type) {
		*(enum rtw_phl_mcc_coex_mode *)info = minfo->coex_mode;
	} else if (MCC_DBG_BT_INFO == type) {
		_os_mem_cpy(phl_to_drvpriv(phl), info, &minfo->bt_info,
				sizeof(struct rtw_phl_mcc_bt_info));
	} else if (MCC_DBG_EN_INFO == type) {
		_os_mem_cpy(phl_to_drvpriv(phl), info, en_info,
				sizeof(struct rtw_phl_mcc_en_info));
	} else {
		goto exit;
	}
	get_ok = true;

exit:
	return get_ok;
}


#endif /* CONFIG_MCC_SUPPORT */
