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
#define _PHL_TWT_C_
#include "phl_headers.h"

#ifdef CONFIG_PHL_TWT
#include "phl_twt.h"
void _twt_transfer_config_state(enum phl_twt_action action,
				enum twt_config_state *state)
{
	if (PHL_TWT_ACTION_FREE == action)
		*state = twt_config_state_free;
	else if (PHL_TWT_ACTION_ALLOC == action)
		*state = twt_config_state_idle;
	else if (PHL_TWT_ACTION_ENABLE == action)
		*state = twt_config_state_enable;
	else if (PHL_TWT_ACTION_DISABLE == action)
		*state = twt_config_state_idle;
	else if (PHL_TWT_ACTION_UP_ERROR == action)
		*state = twt_config_state_error;
}

/*
 * Calculate map of macid
 * @map_offset: number of offset for wait_macid_map
 * @macid_map: map of macid
 * Ex: macid_map = 0x80(bit7), offset = 2, macid 71(7 + 2*32) wait announce
 */
void _twt_calc_macid_map_info(u16 macid, u8 *map_offset, u32 *macid_map)
{
	*map_offset = (u8)(macid / 32);
	*macid_map = BIT(macid % 32);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_calc_macid_map_info(): macid:%d, map_offset:%d, macid_map:0x%x\n",
		macid, *map_offset, *macid_map);
}

u32 _twt_calc_intvl(u8 exp, u16 mantissa)
{
	u32 intvl = 0;

	intvl = mantissa * (1 << exp);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_calc_intvl(): exp:%u, mantissa:%u, intvl=%u\n",
		exp, mantissa, intvl);
	return intvl;
}

u32 _twt_calc_wakeup_dur(u8 dur, enum rtw_phl_wake_dur_unit dur_unit)
{
	u32 dur_t = 0;

	if (RTW_PHL_WAKE_256US == dur_unit)
		dur_t = dur * 256;
	else if (RTW_PHL_WAKE_1TU == dur_unit)
		dur_t = dur * 1024;
	return dur_t;
}

enum rtw_phl_status _twt_fill_individual_twt_para_set(
				struct rtw_phl_indiv_twt_para_set *para,
				bool ndp_paging, u8 *buf, u8 *length)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_req_type_indiv *req_type = &para->req_type;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_fill_individual_twt_para_set(): twt_request(%d), twt_setup_cmd(%d), trigger(%d), implicit(%d), flow_type(%d), twt_flow_id(%d), twt_wake_int_exp(%d), twt_protection(%d)\n",
		req_type->twt_request, req_type->twt_setup_cmd,
		req_type->trigger, req_type->implicit,
		req_type->flow_type, req_type->twt_flow_id,
		req_type->twt_wake_int_exp, req_type->twt_protection);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_fill_individual_twt_para_set(): target_wake_t_h(0x%08x), target_wake_t_l(0x%08x), nom_min_twt_wake_dur(%d), twt_wake_int_mantissa(%d), twt_channel(%d)\n",
		para->target_wake_t_h, para->target_wake_t_l,
		para->nom_min_twt_wake_dur, para->twt_wake_int_mantissa,
		para->twt_channel);
	*length = 0;
	/*Request Type*/
	SET_TWT_REQ_TYPE_TWT_REQUEST(buf, req_type->twt_request);
	SET_TWT_REQ_TYPE_TWT_SETUP_COMMAND(buf, req_type->twt_setup_cmd);
	SET_TWT_REQ_TYPE_TRIGGER(buf, req_type->trigger);
	SET_TWT_REQ_TYPE_IMPLICIT(buf, req_type->implicit);
	SET_TWT_REQ_TYPE_FLOW_TYPE(buf, req_type->flow_type);
	SET_TWT_REQ_TYPE_TWT_FLOW_IDENTIFER(buf, req_type->twt_flow_id);
	SET_TWT_REQ_TYPE_TWT_WAKE_INTERVAL_EXPONENT(buf,
					req_type->twt_wake_int_exp);
	SET_TWT_REQ_TYPE_TWT_PROTECTION(buf, req_type->twt_protection);
	*length += REQUEST_TYPE_LENGTH;
	if (RTW_PHL_TWT_GROUPING == req_type->twt_setup_cmd) {
		/*TODO*/
	} else {
		SET_TWT_TARGET_WAKE_TIME_L(buf, para->target_wake_t_l);
		SET_TWT_TARGET_WAKE_TIME_H(buf, para->target_wake_t_h);
		*length += TARGET_WAKE_TIME_LENGTH;
	}
	SET_TWT_NOMINAL_MINIMUM_TWT_WAKE_DURATION(buf, *length,
						para->nom_min_twt_wake_dur);
	*length += NOMINAL_MINIMUM_TWT_WAKE_DURATION_LENGTH;

	SET_TWT_TWT_WAKE_INTERVAL_MANTISSA(buf, *length,
					para->twt_wake_int_mantissa);
	*length += TWT_WAKE_INTERVAL_MANTISSA_LENGTH;
	SET_TWT_TWT_CHANNEL(buf, *length, para->twt_channel);
	*length += TWT_CHANNEL_LENGTH;
	if (true == ndp_paging) {
		/*TODO*/
	}
	pstatus = RTW_PHL_STATUS_SUCCESS;
	return pstatus;
}

enum rtw_phl_status _twt_parse_individual_twt_para(u8 *twt_ele, u16 length,
				struct rtw_phl_twt_element *element)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_indiv_twt_para_set *para = &element->info.i_twt_para_set;
	struct rtw_phl_req_type_indiv *req_type = &para->req_type;

	u8 *next_buf = twt_ele + ELEM_ID_LEN + ELEM_LEN_LEN + CONTROL_LENGTH;
	req_type->twt_request = GET_TWT_REQ_TYPE_TWT_REQUEST(next_buf);
	req_type->twt_setup_cmd = GET_TWT_REQ_TYPE_TWT_SETUP_COMMAND(next_buf);
	req_type->trigger = GET_TWT_REQ_TYPE_TRIGGER(next_buf);
	req_type->implicit = GET_TWT_REQ_TYPE_IMPLICIT(next_buf);
	req_type->flow_type = GET_TWT_REQ_TYPE_FLOW_TYPE(next_buf);
	req_type->twt_flow_id = GET_TWT_REQ_TYPE_TWT_FLOW_IDENTIFER(next_buf);
	req_type->twt_wake_int_exp =
			GET_TWT_REQ_TYPE_TWT_WAKE_INTERVAL_EXPONENT(next_buf);
	req_type->twt_protection = GET_TWT_REQ_TYPE_TWT_PROTECTION(next_buf);
	next_buf += REQUEST_TYPE_LENGTH;
	if (RTW_PHL_TWT_GROUPING == req_type->twt_setup_cmd) {
		//Todo
	} else {
		para->target_wake_t_l = GET_TWT_TARGET_WAKE_TIME_L(next_buf);
		para->target_wake_t_h = GET_TWT_TARGET_WAKE_TIME_H(next_buf);
		next_buf += TARGET_WAKE_TIME_LENGTH;
	}
	para->nom_min_twt_wake_dur =
			GET_TWT_NOMINAL_MINIMUM_TWT_WAKE_DURATION(next_buf);
	next_buf += NOMINAL_MIN_TWT_WAKE_DURATION_LENGTH;
	para->twt_wake_int_mantissa =
				GET_TWT_TWT_WAKE_INTERVAL_MANTISSA(next_buf);
	next_buf += TWT_WAKE_INTERVAL_MANTISSA_LENGTH;
	para->twt_channel = GET_TWT_TWT_CHANNEL(next_buf);
	next_buf += TWT_CHANNEL_LENGTH;
	if (element->twt_ctrl.ndp_paging_indic) {
		/*TODO*/
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_parse_individual_twt_para(): twt_request:%d, twt_setup_cmd:%d, trigger:%d, implicit:%d, flow_type:%d, twt_flow_id:%d, twt_wake_int_exp:%d, twt_protection:%d\n",
		req_type->twt_request, req_type->twt_setup_cmd,
		req_type->trigger, req_type->implicit,
		req_type->flow_type, req_type->twt_flow_id,
		req_type->twt_wake_int_exp, req_type->twt_protection);

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_parse_individual_twt_para(): target_wake_t_h:0x%08X, target_wake_t_l:0x%08X, nom_min_twt_wake_dur:%d, twt_wake_int_mantissa:%d, twt_channel:%d\n",
		para->target_wake_t_h, para->target_wake_t_l,
		para->nom_min_twt_wake_dur, para->twt_wake_int_mantissa,
		para->twt_channel);
	pstatus = RTW_PHL_STATUS_SUCCESS;
	return pstatus;
}

enum rtw_phl_status _twt_announce_info_enqueue(struct phl_info_t *phl_info,
				struct phl_queue *twt_annc_q,
				struct _twt_announce_info *twt_annc)
{
	void *drv = phl_to_drvpriv(phl_info);

	if (!twt_annc)
		return RTW_PHL_STATUS_FAILURE;
	_os_spinlock(drv, &twt_annc_q->lock, _bh, NULL);
	list_add_tail(&twt_annc->list, &twt_annc_q->queue);
	twt_annc_q->cnt++;
	_os_spinunlock(drv, &twt_annc_q->lock, _bh, NULL);
	return RTW_PHL_STATUS_SUCCESS;
}

struct _twt_announce_info * _twt_announce_info_dequeue(
				struct phl_info_t *phl_info,
				struct phl_queue *twt_annc_q)
{
	struct _twt_announce_info *twt_annc = NULL;
	void *drv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv, &twt_annc_q->lock, _bh, NULL);
	if (list_empty(&twt_annc_q->queue)) {
		twt_annc = NULL;
	} else {
		twt_annc = list_first_entry(&twt_annc_q->queue,
					struct _twt_announce_info, list);
		list_del(&twt_annc->list);
		twt_annc_q->cnt--;
	}
	_os_spinunlock(drv, &twt_annc_q->lock, _bh, NULL);
	return twt_annc;
}

enum rtw_phl_status _twt_sta_announce(struct phl_info_t *phl_info,
				struct phl_queue *annc_queue, u16 macid)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl_info);
	struct _twt_announce_info *info = NULL;
	_os_list *annc_list = &annc_queue->queue;
	u8 offset = 0;
	u32 macid_map = 0;

	_twt_calc_macid_map_info(macid, &offset, &macid_map);
	_os_spinlock(drv, &annc_queue->lock, _bh, NULL);
	phl_list_for_loop(info, struct _twt_announce_info, annc_list, list) {
		if (NULL == info)
			break;
		if (info->map_offset != offset)
			continue;
		if (!(info->wait_macid_map & macid_map))
			continue;
		hstatus = rtw_hal_twt_sta_announce(phl_info->hal, (u8)macid);
		if (RTW_HAL_STATUS_SUCCESS == hstatus) {
			info->wait_macid_map &= (~macid_map);
			pstatus = RTW_PHL_STATUS_SUCCESS;
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_sta_announce(): rtw_hal_twt_sta_announce success, macid:%d, map_offset:%d, wait_macid_map:0x%x\n",
				macid, info->map_offset, info->wait_macid_map);
		} else {
			PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "_twt_sta_announce(): rtw_hal_twt_sta_announce fail, macid:%d, map_offset:%d, wait_macid_map:0x%x\n",
				macid, info->map_offset, info->wait_macid_map);
		}
		break;
	}
	_os_spinunlock(drv, &annc_queue->lock, _bh, NULL);
	return pstatus;
}

enum rtw_phl_status _twt_set_sta_announce_state(struct phl_info_t *phl_info,
				struct phl_queue *annc_q, u16 macid,
				enum phl_wait_annc_type type)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl_info);
	struct _twt_announce_info *info = NULL;
	_os_list *annc_list = &annc_q->queue;
	u8 offset = 0;
	u32 macid_map = 0;
	u8 bset = false;

	_twt_calc_macid_map_info(macid, &offset, &macid_map);
	_os_spinlock(drv, &annc_q->lock, _bh, NULL);
	phl_list_for_loop(info, struct _twt_announce_info, annc_list, list) {
		if (NULL == info)
			break;
		if (info->map_offset != offset)
			continue;
		if (RTW_PHL_TWT_WAIT_ANNC_ENABLE == type) {
			info->wait_macid_map |= macid_map;
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_set_sta_announce_state(): set macid:%d to wait annc state, map_offset:%d, wait_macid_map:0x%x\n",
				macid, info->map_offset, info->wait_macid_map);
		} else if (RTW_PHL_TWT_WAIT_ANNC_DISABLE == type) {
			info->wait_macid_map &= (~macid_map);
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_set_sta_announce_state(): set macid:%d to annc state, map_offset:%d, wait_macid_map:0x%x\n",
				macid, info->map_offset, info->wait_macid_map);
		} else {
			PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "_twt_set_sta_announce_state(): Unknown type:%d\n",
				type);
			break;
		}
		pstatus = RTW_PHL_STATUS_SUCCESS;
		bset = true;
		break;
	}
	_os_spinunlock(drv, &annc_q->lock, _bh, NULL);
	if (true == bset)
		goto exit;
	if (RTW_PHL_TWT_WAIT_ANNC_ENABLE == type) {
		info = _os_mem_alloc(drv, sizeof(struct _twt_announce_info));
		if (NULL == info) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "_twt_sta_wait_announce(): Fail to alloc new annc info\n");
		} else {
			info->map_offset = offset;
			info->wait_macid_map = macid_map;
			_twt_announce_info_enqueue(phl_info, annc_q, info);
			pstatus = RTW_PHL_STATUS_SUCCESS;
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_set_sta_announce_state(): add new Q and set macid:%d to annc state, map_offset:%d, wait_macid_map:0x%x\n",
				macid, info->map_offset, info->wait_macid_map);
		}
	} else if (RTW_PHL_TWT_WAIT_ANNC_DISABLE == type) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "_twt_set_sta_announce_state(): macid:%d is not in wait annc state\n",
				macid);
	} else {
		/*nothing*/
	}
exit:
	return pstatus;
}

enum rtw_phl_status rtw_phl_twt_handle_c2h_wait_annc(struct phl_info_t *phl,
				u8 *buf)
{
	struct rtw_phl_twt_wait_anno_rpt *wait_anno = NULL;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_twt_info *phl_twt_info = get_twt_info(phl);
	struct phl_queue *annc_q = &phl_twt_info->twt_annc_queue;
	u8 i;
	bool error = false;

	if (buf == NULL) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_handle_c2h_wait_annc(): buf == NULL!\n");
		return RTW_PHL_STATUS_FAILURE;
	}

	wait_anno = (struct rtw_phl_twt_wait_anno_rpt *)buf;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "[%s]wait_case = %d, macid = %d %d %d\n",
							__func__,
							wait_anno->wait_case,
							wait_anno->macid[0],
							wait_anno->macid[1],
							wait_anno->macid[2]);

	for (i = 0; i < RTW_PHL_TWT_WAIT_ANNO_STA_NUM; i++) {
		if (IGNORE_MACID == wait_anno->macid[i])
			continue;
		pstatus = _twt_set_sta_announce_state(phl, annc_q,
						wait_anno->macid[i],
						wait_anno->wait_case);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_handle_c2h_wait_annc(): pstatus:%d, macid: %d, fail to set sta wait announce state\n",
				pstatus, wait_anno->macid[i]);
			error = true;
		}
	}
	if (true == error)
		pstatus = RTW_PHL_STATUS_FAILURE;
	return pstatus;
}

/*
struct rtw_twt_sta_info *
_twt_get_twt_sta(
	struct phl_info_t *phl_info,
	struct phl_queue *sta_queue,
	struct rtw_phl_stainfo_t *phl_sta,
	u8 id
)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_twt_sta_info *twt_sta = NULL, *ret_twt_sta = NULL;
	_os_list *sta_list = &sta_queue->queue;
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> _twt_get_twt_sta()\n");
	_os_spinlock(drv, &sta_queue->lock, _bh, NULL);
	phl_list_for_loop(twt_sta, struct rtw_twt_sta_info, sta_list, list) {
		if (twt_sta == NULL)
			break;
		if (phl_sta != twt_sta->phl_sta)
			continue;
		if (DELETE_ALL != id && id != twt_sta->id)
			continue;
		ret_twt_sta = twt_sta;
	}
	_os_spinunlock(drv, &sta_queue->lock, _bh, NULL);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== _twt_get_twt_sta()\n");
	return ret_twt_sta;
}
*/

void _twt_fill_config_info_indiv(struct rtw_phl_twt_info *twt_info,
				struct rtw_phl_indiv_twt_para_set *para_set)
{
	struct rtw_phl_req_type_indiv *req_type = &para_set->req_type;

	twt_info->trigger = req_type->trigger;
	twt_info->flow_type = req_type->flow_type;
	twt_info->implicit_lastbcast = req_type->implicit;
	twt_info->twt_protection = req_type->twt_protection;
	twt_info->twt_wake_int_exp = req_type->twt_wake_int_exp;
	twt_info->twt_wake_int_mantissa = para_set->twt_wake_int_mantissa;
	twt_info->nom_min_twt_wake_dur = para_set->nom_min_twt_wake_dur;
	twt_info->target_wake_time_h = para_set->target_wake_t_h;
	twt_info->target_wake_time_l = para_set->target_wake_t_l;
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_fill_config_info_indiv(): twt_info: trigger:%d, flow_type:%d, implicit_lastbcast:%d, twt_protection:%d, twt_wake_int_exp:%d, twt_wake_int_mantissa:%d, nom_min_twt_wake_dur:%d, target_wake_time_h:0x%08X, target_wake_time_l:0x%08X\n",
		twt_info->trigger, twt_info->flow_type,
		twt_info->implicit_lastbcast, twt_info->twt_protection,
		twt_info->twt_wake_int_exp, twt_info->twt_wake_int_mantissa,
		twt_info->nom_min_twt_wake_dur, twt_info->target_wake_time_h,
		twt_info->target_wake_time_l);
}

void _twt_fill_config_info(struct rtw_phl_twt_info *twt_info,
				struct rtw_phl_twt_setup_info *setup_info)
{
	struct rtw_phl_twt_element *twt_ele = &setup_info->twt_element;
	struct rtw_phl_twt_control *twt_ctrl = &twt_ele->twt_ctrl;

	twt_info->responder_pm_mode = twt_ctrl->responder_pm_mode;
	twt_info->nego_type = twt_ctrl->nego_type;
	twt_info->wake_dur_unit = twt_ctrl->wake_dur_unit;
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_fill_config_info(): twt_info: responder_pm_mode:%d, nego_type:%d, wake_dur_unit:%d\n",
		twt_info->responder_pm_mode, twt_info->nego_type,
		twt_info->wake_dur_unit);
	if (RTW_PHL_INDIV_TWT == twt_info->nego_type) {
		_twt_fill_config_info_indiv(twt_info,
						&twt_ele->info.i_twt_para_set);
	} else {
		/*todo*/
	}
}

void _twt_reset_config_info(struct phl_info_t *phl,
				struct phl_twt_config *config)
{
	config->role = NULL;
	_os_mem_set(phl_to_drvpriv(phl), &config->twt_info, 0,
			sizeof(struct rtw_phl_twt_info));
}

u8 _twt_compare_twt_para(struct rtw_phl_twt_info *twt_info,
				struct rtw_phl_twt_setup_info *twt_setup)
{
	u8 ret = false;
	u64 twt1 = 0, twt2 = 0, diff_t = 0;
	u32 intvl = 0;
	struct rtw_phl_twt_info cmp_info = {0};

	_twt_fill_config_info(&cmp_info, twt_setup);
	do {
		if (cmp_info.responder_pm_mode != twt_info->responder_pm_mode)
			break;
		if (cmp_info.nego_type != twt_info->nego_type)
			break;
		if (cmp_info.trigger != twt_info->trigger)
			break;
		if (cmp_info.flow_type != twt_info->flow_type)
			break;
		if (cmp_info.implicit_lastbcast != twt_info->implicit_lastbcast)
			break;
		if (cmp_info.twt_protection != twt_info->twt_protection)
			break;
		if ((_twt_calc_wakeup_dur(cmp_info.nom_min_twt_wake_dur
			, cmp_info.wake_dur_unit)) !=
			(_twt_calc_wakeup_dur(twt_info->nom_min_twt_wake_dur,
			twt_info->wake_dur_unit)))
			break;
		if ((_twt_calc_intvl(cmp_info.twt_wake_int_exp,
			cmp_info.twt_wake_int_mantissa)) !=
			(_twt_calc_intvl(twt_info->twt_wake_int_exp,
			twt_info->twt_wake_int_mantissa)))
			break;
		/*compare target wake time*/
		intvl = _twt_calc_intvl(twt_info->twt_wake_int_exp,
					twt_info->twt_wake_int_mantissa);
		twt1 = cmp_info.target_wake_time_h;
		twt1 = twt1 << 32;
		twt1 |= cmp_info.target_wake_time_l;
		twt2 = twt_info->target_wake_time_h;
		twt2 = twt2 << 32;
		twt2 |= twt_info->target_wake_time_l;
		if (twt1 > twt2) {
			/*cmp_info target_wake_time > twt_info target_wake_time*/
			diff_t = _os_minus64(twt1, twt2);
		} else {
			diff_t = _os_minus64(twt2, twt1);
		}
		if (_os_modular64(diff_t, intvl) != 0)
			break;
		ret = true;
	} while(false);
	return ret;
}

u8 _twt_is_same_config(struct phl_twt_config *config,
				struct _twt_compare *compare_info)
{
	bool found = false;

	do {
		if (config->role != compare_info->role)
			break;
		if (!(twt_config_state_idle == config->state ||
			twt_config_state_enable == config->state))
			break;
		if (_twt_compare_twt_para(&config->twt_info,
				&compare_info->twt_setup))
			found = true;
	} while(false);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== _twt_is_same_config(): found(%d)\n",
		found);
	return found;
}

void _twt_dump_twt_cfg_info(struct phl_twt_cfg_info *twt_cfg_i)
{
	struct phl_twt_config *config = NULL;
	u8 i = 0;

	config = (struct phl_twt_config *)twt_cfg_i->twt_cfg_ring;

	for (i = 0; i < twt_cfg_i->twt_cfg_num; i++) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_DEBUG_, "_twt_dump_twt_cfg_info(): loop i(%d), cfg id(%d), state(%d)\n",
			i, config[i].idx, config[i].state);
	}
}

enum rtw_phl_status _twt_operate_twt_config(struct phl_info_t *phl_info,
		struct phl_twt_cfg_info *twt_cfg_i, enum phl_operate_config_type type,
		u8 *para, struct phl_twt_config **ret_config)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_twt_config *config =
			(struct phl_twt_config *)twt_cfg_i->twt_cfg_ring;
	u8 i = 0;

	PHL_TRACE(COMP_PHL_TWT, _PHL_DEBUG_, "==> _twt_operate_twt_config(): type(%d)\n",
		type);
	if (type == PHL_GET_CONFIG_BY_ID) {
		if (*para >= twt_cfg_i->twt_cfg_num) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_operate_twt_config(): get cfg by id(%d) fail, out of range(%d)\n",
				*para, twt_cfg_i->twt_cfg_num);
			goto exit;
		}
		if (twt_config_state_free == config[*para].state){
			PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_operate_twt_config(): get cfg by id(%d) fail, cfg state is in twt_config_state_free\n",
				*para);
			goto exit;
		}
		*ret_config = &config[*para];
		pstatus = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	} else if (type == PHL_FREE_CONFIG) {
		if (*para >= twt_cfg_i->twt_cfg_num) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_operate_twt_config(): free cfg by id(%d) fail, out of range(%d)\n",
				*para, twt_cfg_i->twt_cfg_num);
			goto exit;
		}
		if (twt_config_state_free == config[*para].state){
			PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_operate_twt_config(): free cfg by id(%d) fail, cfg state is in twt_config_state_free\n",
				*para);
			goto exit;
		}
		_twt_transfer_config_state(PHL_TWT_ACTION_FREE, &config[*para].state);
		pstatus = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	} else if (type == PHL_GET_HEAD_CONFIG) {
		*ret_config = config;
		pstatus = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	} else if (type == PHL_GET_NEXT_CONFIG) {
		u8 next_id = 0;
		if (*para >= twt_cfg_i->twt_cfg_num) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_operate_twt_config(): get cfg by id(%d) fail, out of range(%d)\n",
				*para, twt_cfg_i->twt_cfg_num);
			goto exit;
		}
		next_id = *para + 1;
		if (next_id == twt_cfg_i->twt_cfg_num)
			next_id = 0;
		*ret_config = &config[next_id];
		pstatus = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	}
	for (i = 0; i < twt_cfg_i->twt_cfg_num; i++) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_DEBUG_, "_twt_operate_twt_config(): loop i(%d), cfg id(%d), state(%d)\n",
		i, config[i].idx, config[i].state);
		if (type == PHL_GET_NEW_CONFIG) {
			if (twt_config_state_free != config[i].state)
				continue;
			_twt_reset_config_info(phl_info, &config[i]);
			_twt_transfer_config_state(PHL_TWT_ACTION_ALLOC,
							&config[i].state);
			config[i].twt_info.twt_id = config[i].idx;
			*ret_config = &config[i];
			pstatus = RTW_PHL_STATUS_SUCCESS;
			break;
		} else if (type == PHL_GET_CONFIG_BY_ROLE) {
			if (twt_config_state_free == config[i].state)
				continue;
			if ((struct rtw_wifi_role_t *)para != config[i].role)
				continue;
			*ret_config = &config[i];
			pstatus = RTW_PHL_STATUS_SUCCESS;
			break;
		} else if (type == PHL_GET_CONFIG_BY_PARA) {
			if (twt_config_state_free == config[i].state)
				continue;
			if (!_twt_is_same_config(&config[i],
						(struct _twt_compare *)para))
				continue;
			*ret_config = &config[i];
			pstatus = RTW_PHL_STATUS_SUCCESS;
			break;
		}
	}
exit:
	_twt_dump_twt_cfg_info(twt_cfg_i);
	PHL_TRACE(COMP_PHL_TWT, _PHL_DEBUG_, "<== _twt_operate_twt_config(): pstatus:%d\n",
		pstatus);
	return pstatus;
}

enum rtw_phl_status _twt_sta_update(void *hal, u16 macid, u8 twt_id,
				enum rtw_phl_twt_sta_action action)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	hstatus = rtw_hal_twt_sta_update(hal, (u8)macid, twt_id, action);
	if (hstatus != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "twt sta update fail: hstatus:%d, macid:%d, twt_id:%d, action:%d\n",
			hstatus, macid, twt_id, action);
	} else {
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "twt sta update ok: macid:%d, twt_id:%d, action:%d\n",
			macid, twt_id, action);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	}
	return pstatus;
}

enum rtw_phl_status _twt_all_sta_update(struct phl_info_t *phl_info,
				u8 config_id, struct phl_queue *sta_queue,
				enum rtw_phl_twt_sta_action action)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl_info);
	_os_list *sta_list = &sta_queue->queue;
	struct rtw_twt_sta_info *psta = NULL;

	_os_spinlock(drv, &sta_queue->lock, _bh, NULL);
	phl_list_for_loop(psta, struct rtw_twt_sta_info, sta_list, list) {
		if (NULL == psta)
			break;
		pstatus = _twt_sta_update(phl_info->hal, psta->phl_sta->macid,
						config_id, action);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;
	}
	_os_spinunlock(drv, &sta_queue->lock, _bh, NULL);
	return pstatus;
}

struct rtw_twt_sta_info * _twt_get_sta_info(struct phl_info_t *phl_info,
		struct phl_queue *sta_queue, struct rtw_phl_stainfo_t *phl_sta)
{
	void *drv = phl_to_drvpriv(phl_info);
	_os_list *sta_list = &sta_queue->queue;
	struct rtw_twt_sta_info *psta = NULL, *ret_sta = NULL;

	_os_spinlock(drv, &sta_queue->lock, _bh, NULL);
	phl_list_for_loop(psta, struct rtw_twt_sta_info, sta_list, list) {
		if (NULL == psta)
			break;
		if (phl_sta == psta->phl_sta) {
			ret_sta = psta;
			break;
		}
	}
	_os_spinunlock(drv, &sta_queue->lock, _bh, NULL);
	return ret_sta;
}

enum rtw_phl_status _twt_sta_enqueue(struct phl_info_t *phl_info,
		struct phl_queue *sta_q, struct rtw_twt_sta_info *psta)
{
	void *drv = phl_to_drvpriv(phl_info);

	if (!psta)
		return RTW_PHL_STATUS_FAILURE;
	_os_spinlock(drv, &sta_q->lock, _bh, NULL);
	list_add_tail(&psta->list, &sta_q->queue);
	sta_q->cnt++;
	_os_spinunlock(drv, &sta_q->lock, _bh, NULL);
	return RTW_PHL_STATUS_SUCCESS;
}

struct rtw_twt_sta_info * _twt_sta_dequeue(struct phl_info_t *phl_info,
				struct phl_queue *sta_q, u16 *cnt)
{
	struct rtw_twt_sta_info *psta = NULL;
	void *drv = phl_to_drvpriv(phl_info);

	_os_spinlock(drv, &sta_q->lock, _bh, NULL);
	if (list_empty(&sta_q->queue)) {
		psta = NULL;
	} else {
		psta = list_first_entry(&sta_q->queue,
					struct rtw_twt_sta_info, list);
		list_del(&psta->list);
		sta_q->cnt--;
		*cnt = (u16)sta_q->cnt;
	}
	_os_spinunlock(drv, &sta_q->lock, _bh, NULL);
	return psta;
}

/*
 * Delete all sta entry from queue
 * @sta_queue: twt sta Q
 */
void _twt_delete_all_sta(struct phl_info_t *phl_info,
				struct phl_queue *sta_queue)
{
	struct rtw_twt_sta_info *twt_sta;
	void *drv = phl_to_drvpriv(phl_info);
	u16 cnt;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> _twt_delete_all_sta()\n");
	do {
		twt_sta = _twt_sta_dequeue(phl_info, sta_queue, &cnt);
		if (NULL != twt_sta)
			_os_mem_free(drv, twt_sta,
					sizeof(struct rtw_twt_sta_info));
	} while (twt_sta != NULL);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== _twt_delete_all_sta()\n");
}

/*
 * Delete twt sta entry by specific sta and id from queue
 * @sta_queue: twt sta Q
 * @phl_sta: specific sta
 * @id: specific twt folw id/broadcast twt id or delete all
 * @cnt: total num of sta entery in Q
 */
enum rtw_phl_status _twt_delete_sta(struct phl_info_t *phl_info,
			struct phl_queue *sta_q,
			struct rtw_phl_stainfo_t *phl_sta, u8 id, u16 *cnt)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_twt_sta_info *twt_sta, *f_sta;
	void *drv = phl_to_drvpriv(phl_info);

	f_sta = _twt_sta_dequeue(phl_info, sta_q, cnt);
	twt_sta = f_sta;
	do {
		if (twt_sta == NULL)
			break;
		if ((phl_sta == twt_sta->phl_sta) &&
			(DELETE_ALL == id || id == twt_sta->id)) {
			_os_mem_free(drv, twt_sta,
					sizeof(struct rtw_twt_sta_info));
			pstatus = RTW_PHL_STATUS_SUCCESS;
			break;
		}
		_twt_sta_enqueue(phl_info, sta_q, twt_sta);
		twt_sta = _twt_sta_dequeue(phl_info, sta_q, cnt);
		if (NULL != twt_sta && twt_sta == f_sta) {
			_twt_sta_enqueue(phl_info, sta_q, twt_sta);
			break;
		}
	} while (true);
	return pstatus;
}

/*
 * Does sta exist in twt sta entry by specific sta and id
 * @sta_queue: twt sta Q
 * @phl_sta: specific sta
 * @id: specific twt folw id/broadcast twt id or delete all
 */
enum rtw_phl_status _twt_sta_exist(struct phl_info_t *phl_info,
			struct phl_queue *sta_q,
			struct rtw_phl_stainfo_t *phl_sta, u8 id)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_twt_sta_info *twt_sta = NULL;
	void *drv = phl_to_drvpriv(phl_info);
	_os_list *q_list = &sta_q->queue;

	_os_spinlock(drv, &sta_q->lock, _bh, NULL);
	phl_list_for_loop(twt_sta, struct rtw_twt_sta_info, q_list, list) {
		if (NULL == twt_sta)
			break;
		if ((phl_sta == twt_sta->phl_sta) &&
			(DELETE_ALL == id || id == twt_sta->id)) {
			pstatus = RTW_PHL_STATUS_SUCCESS;
			break;
		}
	}
	_os_spinunlock(drv, &sta_q->lock, _bh, NULL);
	return pstatus;
}


enum rtw_phl_status _twt_add_sta(struct phl_info_t *phl_info,
			struct rtw_phl_stainfo_t *phl_sta,
			struct phl_queue *sta_q, u8 id)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_twt_sta_info *twt_sta;
	void *drv = phl_to_drvpriv(phl_info);

	twt_sta = _os_mem_alloc(drv, sizeof(struct rtw_twt_sta_info));
	if (NULL == twt_sta) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "_phl_twt_add_sta(): alloc rtw_twt_sta_info failed\n");
	} else {
		twt_sta->phl_sta = phl_sta;
		twt_sta->id = id;
		_twt_sta_enqueue(phl_info, sta_q, twt_sta);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	}
	return pstatus;
}

enum rtw_phl_status _twt_delete_sta_info(struct phl_info_t *phl_info,
			struct rtw_phl_stainfo_t *phl_sta,
			u8 ignore_type, enum rtw_phl_nego_type nego_type,
			u8 id, u8 *bitmap)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_twt_info *phl_twt_info = get_twt_info(phl_info);
	struct phl_twt_cfg_info *twt_cfg_i = &phl_twt_info->twt_cfg_info;
	struct phl_twt_config *config = NULL, *f_config = NULL;
	enum rtw_phl_nego_type type = nego_type;
	bool delete_error = false;
	u16 cnt;
	u8 delete_id = ignore_type ? DELETE_ALL : id;

	*bitmap = 0;
	if (RTW_PHL_MANAGE_BCAST_TWT == nego_type)
		type = RTW_PHL_BCAST_TWT;
	if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl_info, twt_cfg_i,
				PHL_GET_HEAD_CONFIG, NULL, &config)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_delete_sta_info(): Fail to get first allocate config\n");
		goto exit;
	}
	f_config = config;
	do {
		PHL_TRACE(COMP_PHL_TWT, _PHL_DEBUG_, "_twt_delete_sta_info(): while loop, twt_id:%d\n",
			config->twt_info.twt_id);
		if (twt_config_state_free == config->state)
			goto next_cfg;
		if (config->role != phl_sta->wrole)
			goto next_cfg;
		if (false == ignore_type && config->twt_info.nego_type != type)
			goto next_cfg;
		if (RTW_PHL_STATUS_SUCCESS != _twt_sta_exist(phl_info,
							&config->twt_sta_queue,
							phl_sta, delete_id))
			goto next_cfg;
		if (RTW_PHL_STATUS_SUCCESS != _twt_sta_update(phl_info->hal,
							phl_sta->macid,
							config->twt_info.twt_id,
							TWT_STA_DEL_MACID)) {
			delete_error = true;
			goto next_cfg;
		}
		if (RTW_PHL_STATUS_SUCCESS != _twt_delete_sta(phl_info,
							&config->twt_sta_queue,
							phl_sta, delete_id,
							&cnt)) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_delete_sta_info(): Fail to delete sta from twt_sta Q, macid(0x%x), delete_id(%d)\n",
				phl_sta->macid, delete_id);
			delete_error = true;
			goto next_cfg;
		}
		if (0 == cnt)
			*bitmap |= (1 << config->twt_info.twt_id);
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_delete_sta_info(): Delete sta success, config id = %d, twt_sta_queue cnt:%d, bitmap:0x%X\n",
			config->twt_info.twt_id, cnt, *bitmap);
		if (DELETE_ALL != delete_id)
			break;
next_cfg:
		if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl_info,
						twt_cfg_i, PHL_GET_NEXT_CONFIG,
						(u8 *)&config->idx, &config)) {
			delete_error = true;
			break;
		}
	} while (config != f_config);
	if (false == delete_error)
		pstatus = RTW_PHL_STATUS_SUCCESS;
	else
		pstatus = RTW_PHL_STATUS_FAILURE;
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_delete_sta_info(): pstatus:%d, nego_type = %d, id:%d, bitmap:0x%x\n",
		pstatus, nego_type, id, *bitmap);
	return pstatus;
}

enum rtw_phl_status
_twt_info_update(struct phl_info_t *phl_info,
                 struct phl_twt_config *config,
                 enum phl_twt_action action)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	enum rtw_phl_twt_cfg_action config_action;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> _twt_info_update()\n");

	if (PHL_TWT_ACTION_ENABLE == action) {
		config_action = TWT_CFG_ADD;
	} else if (PHL_TWT_ACTION_DISABLE == action) {
		config_action = TWT_CFG_DELETE;
	} else {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_info_update(): Unexpected action:%d\n",
			action);
		goto exit;
	}

	hstatus = rtw_hal_twt_info_update(phl_info,
	                                  config->twt_info,
	                                  config->rlink,
	                                  config_action);
	if (hstatus == RTW_HAL_STATUS_SUCCESS) {
		_twt_transfer_config_state(action, &config->state);
		pstatus = RTW_PHL_STATUS_SUCCESS;
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "_twt_info_update(): update ok\n");
	} else {
		_twt_transfer_config_state(PHL_TWT_ACTION_UP_ERROR,
								&config->state);
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_info_update(): update fail, hstatus:%d\n",
			hstatus);
	}
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<==_twt_info_update(): hstatus:%d, twt_id:%d, action:%d\n",
		pstatus, config->twt_info.twt_id, action);
	return pstatus;
}

/*
void
_twt_free_config(
	void *phl,
	struct phl_twt_config *config
)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	_twt_info_update(phl_info->hal, config, PHL_TWT_ACTION_DISABLE);
	_twt_delete_all_sta(phl_info, &config->twt_sta_queue);
	_twt_reset_config_info(config);
	_twt_transfer_config_state(PHL_TWT_ACTION_FREE, &config->state);
}
*/

bool _twt_exist_same_twt_config(struct phl_info_t *phl,
		struct phl_twt_cfg_info *twt_cfg_i, struct rtw_wifi_role_t *role,
		struct rtw_phl_twt_setup_info setup_info,
		struct phl_twt_config **ret_config)
{
	bool exist = false;
	struct _twt_compare compare_info = {0};
	struct phl_twt_config *config = NULL;

	compare_info.role = role;
	compare_info.twt_setup = setup_info;
	if (RTW_PHL_STATUS_SUCCESS == _twt_operate_twt_config(phl, twt_cfg_i,
		PHL_GET_CONFIG_BY_PARA, (u8 *)&compare_info, &config)) {
		*ret_config = config;
		exist = true;
	}
	return exist;
}

/*
u8
_twt_get_new_config_entry(
	struct phl_info_t *phl,
	struct phl_queue *twt_queue,
	struct phl_twt_config **ret_config
)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_twt_config *config = NULL;
	u8 bget = false;
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> _twt_get_new_config_entry()\n");
	if (RTW_PHL_STATUS_SUCCESS == _twt_operate_twt_config(phl, twt_queue,
				PHL_GET_NEW_CONFIG, NULL, &config)) {
		*ret_config = config;
		bget = true;
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== _twt_get_new_config_entry(): bget:%d\n",
		bget);
	return bget;
}
*/

bool _twt_new_config_is_available(struct phl_info_t *phl_i)
{
	struct phl_twt_info *phl_twt_info = get_twt_info(phl_i);
	struct phl_twt_cfg_info *twt_cfg_i = &phl_twt_info->twt_cfg_info;
	struct phl_twt_config *config = NULL;
	u8 available = false;

	if (RTW_PHL_STATUS_SUCCESS == _twt_operate_twt_config(phl_i, twt_cfg_i,
				PHL_GET_NEW_CONFIG, NULL, &config)) {
		_twt_operate_twt_config(phl_i, twt_cfg_i, PHL_FREE_CONFIG,
					&config->twt_info.twt_id, NULL);
		available = true;
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_new_config_is_available(): bavailable:%d\n", available);
	return available;
}

/*
 * Whether the twt flow id of sta exist in any twt config entry.
 * @phl_sta: the specific sta
 * @role: specific role for search twt config entry
 * @id: twt flow id
 * Note: for sta mode.
*/
u8 _twt_flow_id_exist(void *phl, struct rtw_phl_stainfo_t *phl_sta,
				struct rtw_wifi_role_t *role, u8 id)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = get_twt_info(phl_info);
	struct phl_twt_cfg_info *twt_cfg_i = &phl_twt_info->twt_cfg_info;
	struct phl_twt_config *config = NULL, *f_config = NULL;
	struct rtw_twt_sta_info *twt_sta = NULL;
	bool exist = false;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> _twt_flow_id_exist()\n");
	if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl, twt_cfg_i,
				PHL_GET_HEAD_CONFIG, NULL, &config)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_flow_id_exist(): Fail to get first allocate config\n");
		goto exit;
	}
	f_config = config;
	do {
		PHL_TRACE(COMP_PHL_TWT, _PHL_DEBUG_, "_twt_flow_id_exist(): while loop\n");
		if (twt_config_state_free == config->state)
			goto next_cfg;
		if (config->role != phl_sta->wrole ||
			RTW_PHL_INDIV_TWT != config->twt_info.nego_type)
			goto next_cfg;
		twt_sta = _twt_get_sta_info(phl_info, &config->twt_sta_queue,
						phl_sta);
		if (NULL != twt_sta && id == twt_sta->id) {
			exist = true;
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_flow_id_exist(): exist the twt_flow_id:%d\n",
				id);
			break;
		}
next_cfg:
		if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl,
						twt_cfg_i, PHL_GET_NEXT_CONFIG,
						(u8 *)&config->idx, &config))
			break;
	} while(config != f_config);
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== _twt_flow_id_exist(): twt flow id:%d, bexist:%d\n",
		id, exist);
	return exist;
}

enum rtw_phl_status _twt_accept_bcast_by_sta(struct phl_info_t *phl,
			struct rtw_phl_twt_setup_info *setup_info,
			struct rtw_phl_stainfo_t *phl_sta, u8 *config_id)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

	/*TODO*/
	PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "==> _twt_accept_bcast_by_sta(): not support, todo\n");
	pstatus = RTW_PHL_STATUS_FAILURE;
	return pstatus;
}

enum rtw_phl_status _twt_accept_indiv_by_sta(struct phl_info_t *phl,
			struct rtw_phl_twt_setup_info *setup_info,
			struct rtw_phl_stainfo_t *phl_sta, u8 *config_id)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_twt_element *twt_ele = &setup_info->twt_element;
	struct rtw_phl_twt_control *twt_ctrl = &twt_ele->twt_ctrl;
	struct rtw_phl_indiv_twt_para_set *para = &twt_ele->info.i_twt_para_set;
	struct rtw_phl_req_type_indiv *req_type = &para->req_type;
	u8 bitmap = 0, id = 0, i = 0;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> _twt_accept_indiv_by_sta()\n");
	if (_twt_flow_id_exist(phl, phl_sta, phl_sta->wrole,
				req_type->twt_flow_id)) {
		pstatus = _twt_delete_sta_info(phl, phl_sta, false,
						twt_ctrl->nego_type,
						req_type->twt_flow_id, &bitmap);
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_accept_indiv_by_sta(): twt flow id(%d) exist, first, delete twt sta, pstatus:%d, bitmap:0x%x\n",
			req_type->twt_flow_id, pstatus, bitmap);
		if (RTW_PHL_STATUS_SUCCESS == pstatus && bitmap != 0) {
			id = 0;
			do {
				i = ((bitmap >> id) & BIT0);
				if (i != 0) {
					bitmap &= ~(BIT(id));
					break;
				}
				id++;
			} while (true);
			pstatus = rtw_phl_twt_free_twt_config(phl, id);
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_accept_indiv_by_sta():sta Q is empty in twt config entry(%d), we free it, pstatus:%d \n",
				id, pstatus);
			if (bitmap !=0) {
				PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "_twt_accept_indiv_by_sta():  TWT config entry bitmap(0x%x) != 0, some twt config entry not free. please check code\n",
						bitmap);
			}
		}
	}
	pstatus = rtw_phl_twt_alloc_twt_config(phl,
	                                       phl_sta->rlink,
	                                       *setup_info,
	                                       true,
	                                       &id);
	if (RTW_PHL_STATUS_SUCCESS == pstatus) {
		pstatus = rtw_phl_twt_add_sta_info(phl, phl_sta, id,
							req_type->twt_flow_id);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			rtw_phl_twt_free_twt_config(phl, id);
		} else {
			*config_id = id;
		}
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "_twt_accept_indiv_by_sta(): pstatus:%d, config_id:%d\n",
		pstatus, *config_id);
	return pstatus;
}

/*
 * Initialize twt
 */
enum rtw_phl_status phl_twt_init(void *phl)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	struct phl_twt_info *phl_twt_i = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config = NULL;
	u16 len = 0;
	u8 i = 0;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> phl_twt_init()\n");
	if (NULL == phl_info->phl_twt_info) {
		phl_info->phl_twt_info = _os_mem_alloc(drv,
					sizeof(struct phl_twt_info));
		if (NULL == phl_info->phl_twt_info) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "phl_twt_init(): Failed to allocate phl_twt_info\n");
			goto exit;
		}
		_os_mem_set(phl_to_drvpriv(phl_info), phl_info->phl_twt_info,
				0, sizeof(struct phl_twt_info));
	} else {
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "phl_twt_init(): Duplicate init1, please check code\n");
	}
	phl_twt_i = get_twt_info(phl_info);
	twt_cfg_i = &phl_twt_i->twt_cfg_info;
	if (NULL == twt_cfg_i->twt_cfg_ring) {
		twt_cfg_i->twt_cfg_num = MAX_NUM_HW_TWT_CONFIG;
		len = sizeof(struct phl_twt_config) * twt_cfg_i->twt_cfg_num;
		twt_cfg_i->twt_cfg_ring = _os_mem_alloc(drv, len);
		if (NULL == twt_cfg_i->twt_cfg_ring) {
			twt_cfg_i->twt_cfg_num = 0;
			PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "phl_twt_init(): Failed to allocate twt_cfg_ring\n");
			goto exit;
		}
		config = (struct phl_twt_config *)twt_cfg_i->twt_cfg_ring;
		for (i = 0; i < twt_cfg_i->twt_cfg_num; i++) {
			_os_mem_set(phl_to_drvpriv(phl_info), config, 0,
					sizeof(struct phl_twt_config));
			config->idx = i;
			pq_init(drv, &config->twt_sta_queue);
			config++;
		}
	} else {
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "phl_twt_init(): Duplicate init2, please check code\n");
	}
	/* init for twt_annc_queue */
	pq_init(drv, &phl_twt_i->twt_annc_queue);
	pstatus = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "<== phl_twt_init(): pstatus:%d\n",
		pstatus);
	return pstatus;
}

/*
 * Deinitialize twt
 */
void phl_twt_deinit(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	struct phl_twt_info *phl_twt_i = get_twt_info(phl_info);
	struct phl_twt_config *config = NULL;
	struct _twt_announce_info *annc_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	u8 i = 0;
	u16 len = 0;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> phl_twt_deinit()\n");
	if (NULL == phl_twt_i)
		goto exit;
	twt_cfg_i = &phl_twt_i->twt_cfg_info;
	if (NULL == twt_cfg_i->twt_cfg_ring)
		goto free_twt_info;
	config = (struct phl_twt_config *)(twt_cfg_i->twt_cfg_ring);
	for (i = 0; i < twt_cfg_i->twt_cfg_num; i++) {
		if (config->twt_sta_queue.cnt > 0) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "rtw_phl_twt_deinit(): config_id: %d, twt_sta_queue.cnt(%d) >0, force delete all\n",
				config->idx ,config->twt_sta_queue.cnt);
			_twt_delete_all_sta(phl_info, &config->twt_sta_queue);
		}
		pq_deinit(drv, &config->twt_sta_queue);
		config++;
	}
	len = sizeof(struct phl_twt_config) * twt_cfg_i->twt_cfg_num;
	_os_mem_free(drv, twt_cfg_i->twt_cfg_ring, len);
	do {
		annc_info = _twt_announce_info_dequeue(phl_info,
						&phl_twt_i->twt_annc_queue);
		if (NULL == annc_info)
			break;
		_os_mem_free(drv, annc_info, sizeof(struct _twt_announce_info));
	} while(true);
	pq_deinit(drv, &phl_twt_i->twt_annc_queue);
free_twt_info:
	_os_mem_free(drv, phl_info->phl_twt_info, sizeof(struct phl_twt_info));
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_deinit()\n");
}

/*
 * Allocate new twt config
 * @role: the user of twt config
 * @setup_info: twt setup info
 * @benable: whether to enable the twt config to fw,
 * if benable is equal to false, only allocate twt config entry
 * @id: Output the id of twt confi entry
 */
enum rtw_phl_status
rtw_phl_twt_alloc_twt_config(void *phl,
                             struct rtw_wifi_role_link_t *rlink,
                             struct rtw_phl_twt_setup_info setup_info,
                             u8 benable,
                             u8 *id)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config = NULL;
	bool alloc = false;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_alloc_twt_config()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_alloc_twt_config(): twt_sup == false\n");
		return pstatus;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_alloc_twt_config(): twt_init == false\n");
		return pstatus;
	}
	phl_twt_info = get_twt_info(phl_info);
	twt_cfg_i = &phl_twt_info->twt_cfg_info;
/*	if (true == _twt_exist_same_twt_config(phl_info, twt_cfg_i, role,
						setup_info, &config)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "[TWT]alloc from existing config, id = %d\n",
			config->twt_info.twt_id);
		alloc = true;
	} else */{
		if (RTW_PHL_STATUS_SUCCESS == _twt_operate_twt_config(phl_info,
				twt_cfg_i, PHL_GET_NEW_CONFIG, NULL, &config)) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "[TWT]alloc from new config, id = %d\n",
				config->twt_info.twt_id);
			alloc = true;
		} else {
			PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "[TWT]fail to alloc new config\n");
			pstatus = RTW_PHL_STATUS_RESOURCE;
		}
	}
	if (true == alloc) {
		*id = config->twt_info.twt_id;
		config->rlink = rlink;
		_twt_fill_config_info(&config->twt_info, &setup_info);
		if (benable) {
			pstatus = _twt_info_update(phl_info->hal, config,
							PHL_TWT_ACTION_ENABLE);
			if (RTW_PHL_STATUS_SUCCESS != pstatus) {
				/*todo*/
			}
		} else {
			pstatus = RTW_PHL_STATUS_SUCCESS;
		}
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_alloc_twt_config(): pstatus:%d\n",
		pstatus);
	return pstatus;
}

/*
 * Free twt config entry by specific config ID
 * @id: id of twt config entry
 */
enum rtw_phl_status rtw_phl_twt_free_twt_config(void *phl, u8 id)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config = NULL;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_free_twt_config()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_free_twt_config(): twt_sup == false\n");
		return pstatus;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_free_twt_config(): twt_init == false\n");
		return pstatus;
	}
	phl_twt_info = get_twt_info(phl_info);
	twt_cfg_i = &phl_twt_info->twt_cfg_info;
	if (RTW_PHL_STATUS_SUCCESS == _twt_operate_twt_config(phl, twt_cfg_i,
							PHL_GET_CONFIG_BY_ID,
							&id, &config)) {
		_twt_info_update(phl_info->hal, config, PHL_TWT_ACTION_DISABLE);
		_twt_delete_all_sta(phl_info, &config->twt_sta_queue);
		_twt_operate_twt_config(phl, twt_cfg_i, PHL_FREE_CONFIG, &id,
									NULL);
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "[TWT]Free twt config success, id = %d\n",
			id);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_free_twt_config(): pstatus:%d\n",
		pstatus);
	return pstatus;
}

/*
 * Enable twt config by specific config id
 * @id: id of twt confi entry
 */
enum rtw_phl_status rtw_phl_twt_enable_twt_config(void *phl, u8 id)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config = NULL;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_enable_twt_config()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_enable_twt_config(): twt_sup == false\n");
		return pstatus;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_enable_twt_config(): twt_init == false\n");
		return pstatus;
	}
	phl_twt_info = get_twt_info(phl_info);
	twt_cfg_i = &phl_twt_info->twt_cfg_info;
	if (RTW_PHL_STATUS_SUCCESS == _twt_operate_twt_config(phl, twt_cfg_i,
					PHL_GET_CONFIG_BY_ID, &id, &config)) {
		pstatus = _twt_info_update(phl_info->hal, config,
							PHL_TWT_ACTION_ENABLE);
		if (RTW_PHL_STATUS_SUCCESS == pstatus) {
			_twt_all_sta_update(phl_info, id, &config->twt_sta_queue,
						TWT_STA_ADD_MACID);
		}
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_enable_twt_config(): pstatus:%d, id = %d\n",
		pstatus, id);
	return pstatus;
}

/*
 * Free all twt config by specific role
 * @role: specific role for search twt config entry
 */
enum rtw_phl_status rtw_phl_twt_free_all_twt_by_role(void *phl,
				struct rtw_wifi_role_t *role)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config;
	u8 id;
	bool free = false;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_free_all_twt_by_role()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_free_all_twt_by_role(): twt_sup == false\n");
		return pstatus;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_free_all_twt_by_role(): twt_init == false\n");
		return pstatus;
	}
	phl_twt_info = get_twt_info(phl_info);
	twt_cfg_i = &phl_twt_info->twt_cfg_info;
	do {
		pstatus = _twt_operate_twt_config(phl, twt_cfg_i,
				PHL_GET_CONFIG_BY_ROLE, (u8 *)role, &config);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			break;
		id = config->twt_info.twt_id;
		_twt_info_update(phl_info->hal, config, PHL_TWT_ACTION_DISABLE);
		_twt_delete_all_sta(phl_info, &config->twt_sta_queue);
		_twt_operate_twt_config(phl, twt_cfg_i, PHL_FREE_CONFIG, &id,
									NULL);
		free = true;
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "Free twt config success, id = %d\n",
			id);
	} while(true);
	if (true == free)
		pstatus = RTW_PHL_STATUS_SUCCESS;
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_free_all_twt_by_role(): pstatus:%d\n",
		pstatus);
	return pstatus;
}

/*
 * Pause all twt config by specific role
 * @role: specific role for search twt config entry
 */
enum rtw_phl_status rtw_phl_twt_disable_all_twt_by_role(void *phl,
				struct rtw_wifi_role_t *role)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config = NULL, *f_config = NULL;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_disable_all_twt_by_role()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_disable_all_twt_by_role(): twt_sup == false\n");
		goto exit;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_disable_all_twt_by_role(): twt_init == false\n");
		goto exit;
	}
	phl_twt_info = get_twt_info(phl_info);
	twt_cfg_i = &phl_twt_info->twt_cfg_info;
	if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl, twt_cfg_i,
				PHL_GET_HEAD_CONFIG, NULL, &config)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_disable_all_twt_by_role(): Fail to get first allocate config\n");
		goto exit;
	}
	f_config = config;
	do {
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_disable_all_twt_by_role(): while loop, twt_id:%d\n",
			config->twt_info.twt_id);
		if (twt_config_state_free == config->state)
			goto next_cfg;
		if (config->role == role)
			_twt_info_update(phl_info->hal, config,
					PHL_TWT_ACTION_DISABLE);
next_cfg:
		if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl,
						twt_cfg_i, PHL_GET_NEXT_CONFIG,
						(u8 *)&config->idx, &config))
			goto exit;
	} while(config != f_config);
	pstatus = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_disable_all_twt_by_role(): pstatus:%d\n",
		pstatus);
	return pstatus;
}

/*
 * Enable all twt config by specific role
 * @role: specific role for search twt config entry
 */
enum rtw_phl_status rtw_phl_twt_enable_all_twt_by_role(void *phl,
				struct rtw_wifi_role_t *role)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config = NULL, *f_config = NULL;
	bool error = false;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_enable_all_twt_by_role()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_enable_all_twt_by_role(): twt_sup == false\n");
		goto exit;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_enable_all_twt_by_role(): twt_init == false\n");
		goto exit;
	}
	phl_twt_info = get_twt_info(phl_info);
	twt_cfg_i = &phl_twt_info->twt_cfg_info;
	if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl, twt_cfg_i,
				PHL_GET_HEAD_CONFIG, NULL, &config)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_enable_all_twt_by_role(): Fail to get first allocate config\n");
		goto exit;
	}
	f_config = config;
	do {
		PHL_TRACE(COMP_PHL_TWT, _PHL_DEBUG_, "rtw_phl_twt_enable_all_twt_by_role(): while loop, twt_id:%d\n",
			config->twt_info.twt_id);
		if (twt_config_state_free == config->state)
			goto next_cfg;
		if (config->role != role)
			goto next_cfg;
		pstatus = _twt_info_update(phl_info->hal, config,
						PHL_TWT_ACTION_ENABLE);
		if (RTW_PHL_STATUS_SUCCESS == pstatus) {
			pstatus = _twt_all_sta_update(phl_info,
							config->twt_info.twt_id,
							&config->twt_sta_queue,
							TWT_STA_ADD_MACID);
			if (RTW_PHL_STATUS_SUCCESS != pstatus) {
				error = true;
				PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "rtw_phl_twt_enable_all_twt_by_role(): Fail to update all twt sta, twt_id:%d\n",
						config->twt_info.twt_id);
			}
		} else {
			error = true;
			PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "rtw_phl_twt_enable_all_twt_by_role(): Fail to enable twt config, twt_id:%d\n",
				config->twt_info.twt_id);
		}
next_cfg:
		if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl,
						twt_cfg_i, PHL_GET_NEXT_CONFIG,
						(u8 *)&config->idx, &config)) {
			error = true;
			break;
		}
	} while(config != f_config);
	if (true == error)
		pstatus = RTW_PHL_STATUS_FAILURE;
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_enable_all_twt_by_role(): pstatus:%d\n",
		pstatus);
	return pstatus;
}

/*
 *  Add twt sta to specifi twt conig entry
 * @phl_sta: sta entry that you wnat to add in specifi twt conig entry
 * @config_id: id of target twt config entry
 * @id: twt flow id/ broadcast twt id
 */
enum rtw_phl_status rtw_phl_twt_add_sta_info(void *phl,
			struct rtw_phl_stainfo_t *phl_sta, u8 config_id, u8 id)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config = NULL;
	struct phl_queue *sta_q = NULL;
	u16 cnt = 0;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_add_sta_info()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_add_sta_info(): twt_sup == false\n");
		goto fail;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_add_sta_info(): twt_init == false\n");
		goto fail;
	}
	phl_twt_info = get_twt_info(phl_info);
	twt_cfg_i = &phl_twt_info->twt_cfg_info;
	if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl, twt_cfg_i,
							PHL_GET_CONFIG_BY_ID,
							&config_id, &config)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "Fail to get TWT config by id(%d)\n",
			config_id);
		goto fail;
	}
	sta_q = &config->twt_sta_queue;
	if (RTW_PHL_STATUS_SUCCESS != _twt_add_sta(phl, phl_sta, sta_q, id)) {
		goto fail;
	}
	if (RTW_PHL_STATUS_SUCCESS != _twt_sta_update(phl_info->hal,
				phl_sta->macid, config_id, TWT_STA_ADD_MACID)) {
		_twt_delete_sta(phl, sta_q, phl_sta, id, &cnt);
		goto fail;
	}
	pstatus = RTW_PHL_STATUS_SUCCESS;
fail:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_add_sta_info(): pstatus:%d\n",
		pstatus);
	return pstatus;
}

/*
 *  Remove all twt sta from twt config entry by specific sta entry
 * @phl_sta: sta entry that you wnat to remove
 * @bitmap: Output the bitmap. Indicate the statue that twt sta don't exist in the twt config entry that twt sta removed from it.
 * ex: Bitmap=10: We remove the twt sta from id 1, id 3 and other id of twt config entry,
 * but after remove, there are no twt sta existing in the twt config entry of id 1 and id 3.
 * ex: Bitmap=0: We remove the twt sta, after remove, there are at least one twt sta existing in the twt config entry that twt sta removed from it.
 */
enum rtw_phl_status rtw_phl_twt_delete_all_sta_info(void *phl,
				struct rtw_phl_stainfo_t *phl_sta, u8 *bitmap)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_delete_all_sta_info()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_delete_all_sta_info(): twt_sup == false\n");
		return pstatus;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_delete_all_sta_info(): twt_init == false\n");
		return pstatus;
	}
	pstatus = _twt_delete_sta_info(phl, phl_sta, true, 0, 0, bitmap);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_delete_all_sta_info(): pstatus:%d, bitmap:0x%x\n",
		pstatus, *bitmap);
	return pstatus;
}

/*
 * Remove twt sta when tx/rx twt teardown frame
 * @phl_sta: sta entry that you wnat to remove
 * @twt_flow: twt flow field info
 * @bitmap: Output the bitmap. Indicate the statue that twt sta don't exist in the twt config entry that twt sta removed from it.
 * ex: Bitmap=10: We remove the twt sta from id 1, id 3 and other id of twt config entry,
 * but after remove, there are no twt sta existing in the twt config entry of id 1 and id 3.
 * ex: Bitmap=0: We remove the twt sta, after remove, there are at least one twt sta existing in the twt config entry that twt sta removed from it.
 */
enum rtw_phl_status rtw_phl_twt_teardown_sta(void *phl,
			struct rtw_phl_stainfo_t *phl_sta,
			struct rtw_phl_twt_flow_field *twt_flow, u8 *bitmap)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	u8 id = 0;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_teardown_sta()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_teardown_sta(): twt_sup == false\n");
		return pstatus;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_teardown_sta(): twt_init == false\n");
		return pstatus;
	}
	if (RTW_PHL_INDIV_TWT == twt_flow->nego_type ||
		RTW_PHL_WAKE_TBTT_INR == twt_flow->nego_type) {
		if (twt_flow->info.twt_flow01.teardown_all)
			id = DELETE_ALL;
		else
			id = twt_flow->info.twt_flow01.twt_flow_id;
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} else if (RTW_PHL_BCAST_TWT == twt_flow->nego_type ) {
		/*Todo*/
	} else if (RTW_PHL_MANAGE_BCAST_TWT == twt_flow->nego_type) {
		if (twt_flow->info.twt_flow3.teardown_all)
			id = DELETE_ALL;
		else
			id = twt_flow->info.twt_flow3.bcast_twt_id;
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} else {
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "rtw_phl_twt_teardown_sta(): Unknown nego_type:%d\n",
			twt_flow->nego_type);
	}
	if (RTW_PHL_STATUS_SUCCESS == pstatus) {
		pstatus = _twt_delete_sta_info(phl, phl_sta, false,
					twt_flow->nego_type, id, bitmap);
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_teardown_sta(): pstatus:%d, twt_flow->nego_type:%d, id:%d, bitmap:0x%x\n",
		pstatus, twt_flow->nego_type, id, *bitmap);
	return pstatus;
}

/*
 * Assign new flow id for twt setup of sta.
 * @phl_sta: the specific sta
 * @role: specific role for search twt config entry
 * @id: Output: twt flow id
 * Note: for sta mode.
 */
enum rtw_phl_status rtw_phl_twt_get_new_flow_id(void *phl,
				struct rtw_phl_stainfo_t *phl_sta, u8 *id)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_RESOURCE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config = NULL, *f_config = NULL;
	struct rtw_twt_sta_info *twt_sta = NULL;
	u8 use_map = 0, unuse_map = 0;
	u8 i = 0;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_get_new_flow_id()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_get_new_flow_id(): twt_sup == false\n");
		goto exit;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_get_new_flow_id(): twt_init == false\n");
		goto exit;
	}
	phl_twt_info = get_twt_info(phl_info);
	twt_cfg_i = &phl_twt_info->twt_cfg_info;
	if (false == _twt_new_config_is_available(phl_info))
		goto exit;
	if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl, twt_cfg_i,
				PHL_GET_HEAD_CONFIG, NULL, &config)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_get_new_flow_id(): Fail to get first allocate config\n");
		goto exit;
	}
	f_config = config;
	do {
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_get_new_flow_id(): while loop\n");
		if (twt_config_state_free == config->state)
			goto next_cfg;
		if (config->role != phl_sta->wrole ||
			RTW_PHL_INDIV_TWT != config->twt_info.nego_type)
			goto next_cfg;
		twt_sta = _twt_get_sta_info(phl_info, &config->twt_sta_queue,
						phl_sta);
		if (NULL != twt_sta) {
			use_map |= (1 << twt_sta->id);
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_get_new_flow_id(): config_ID:%d, get match sta, twt_sta->id:%d\n",
				config->twt_info.twt_id, twt_sta->id);
		}
next_cfg:
		if (RTW_PHL_STATUS_SUCCESS != _twt_operate_twt_config(phl,
						twt_cfg_i, PHL_GET_NEXT_CONFIG,
						(u8 *)&config->idx, &config))
			goto exit;
	} while(config != f_config);
	unuse_map = (~use_map) & 0xFF;
	i = 0;
	while ((unuse_map >> i) > 0) {
		if ((unuse_map >> i) & BIT0) {
			*id = i;
			pstatus = RTW_PHL_STATUS_SUCCESS;
			break;
		}
		i++;
	}
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_get_new_flow_id(): pstatus:%d, use_map:%d, unuse_map:%d, new_flow_id:%d\n",
		pstatus, use_map, unuse_map, *id);
	return pstatus;
}

static void _phl_twt_get_target_wake_time_done(void *priv, u8 *param,
				u32 param_len, enum rtw_phl_status sts)
{
	if (param) {
		_os_kmem_free(priv, param, param_len);
		param = NULL;
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "%s\n", __func__);
	}
}

enum rtw_phl_status phl_twt_get_target_wake_time(struct phl_info_t *phl,
						u8 *param)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct rtw_phl_twt_get_twt_i *get_twt_i =
					(struct rtw_phl_twt_get_twt_i *)param;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_twt_cfg_info *twt_cfg_i = NULL;
	struct phl_twt_config *config = NULL;
	u32 c_tsf_l = 0, c_tsf_h = 0, intvl = 0;
	u64 cur_tsf = 0, tgt_tsf = 0, ref_tsf = 0, dif_tsf = 0;

	if (false == twt_sup(phl)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s: twt_sup == false\n",
			__func__);
		goto exit;
	}
	if (false == twt_init(phl)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s: twt_init == false\n",
			__func__);
		goto exit;
	}
	phl_twt_info = get_twt_info(phl);
	twt_cfg_i = &phl_twt_info->twt_cfg_info;
	hstatus = rtw_hal_get_tsf(phl->hal,
			get_twt_i->wrole->rlink[RTW_RLINK_PRIMARY].hw_band,
			get_twt_i->wrole->rlink[RTW_RLINK_PRIMARY].hw_port,
			&c_tsf_h, &c_tsf_l);
	if (RTW_HAL_STATUS_FAILURE == hstatus) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "%s: Fail to get tsf, hstatus:%d, port:%d\n",
			__func__, hstatus, get_twt_i->wrole->rlink[RTW_RLINK_PRIMARY].hw_port);
		goto exit;
	}
	cur_tsf = c_tsf_h;
	cur_tsf = cur_tsf << 32;
	cur_tsf |= c_tsf_l;
	if (IGNORE_CFG_ID == get_twt_i->id) {
		tgt_tsf = _os_add64(cur_tsf, get_twt_i->offset * 1000);
	} else {
		pstatus = _twt_operate_twt_config(phl, twt_cfg_i,
				PHL_GET_CONFIG_BY_ID, &get_twt_i->id, &config);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "%s: Fail to get twt entry by id, pstatus:%d, id:%d\n",
				__func__, pstatus, get_twt_i->id);
			goto exit;
		}
		ref_tsf = config->twt_info.target_wake_time_h;
		ref_tsf = ref_tsf << 32;
		ref_tsf |= config->twt_info.target_wake_time_l;
		intvl = _twt_calc_intvl(config->twt_info.twt_wake_int_exp,
					config->twt_info.twt_wake_int_mantissa);
		tgt_tsf = _os_add64(cur_tsf, get_twt_i->offset * 1000);
		dif_tsf = _os_minus64(tgt_tsf, ref_tsf);
		tgt_tsf = _os_minus64(tgt_tsf, _os_modular64(dif_tsf, intvl));
		tgt_tsf = _os_add64(tgt_tsf, intvl);
	}
	*(get_twt_i->tsf_h) = (u32)(tgt_tsf >> 32);
	*(get_twt_i->tsf_l) = (u32)(tgt_tsf);
	pstatus = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "%s: pstatus(%d), port:%d, twt_id:%d, offset:0x%08x, tsf_h: 0x%08X, tsf_l: 0x%08X\n",
		__func__, pstatus, get_twt_i->wrole->rlink[RTW_RLINK_PRIMARY].hw_port, get_twt_i->id,
		get_twt_i->offset, *(get_twt_i->tsf_h), *(get_twt_i->tsf_l));

	return pstatus;
}

/*
 * get target wake time, only handle PHL_CMD_DIRECTLY or PHL_CMD_WAIT of cmd_type
 * @wrole: Specific wrole
 * @id: reference id of twt configuration
 * @offset: unit: ms. An amount of time that you will start TWT from now
 * @tsf_h: return high 4-byte value of target wake time
 * @tsf_l: return low 4-byte value of target wake time
 */
enum rtw_phl_status
rtw_phl_twt_get_target_wake_time(void *phl,
			struct rtw_phl_twt_get_twt_i *get_twt_i,
			enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_i = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_i);
	struct rtw_phl_twt_get_twt_i *para = NULL;
	u32 para_len = 0;

	if (PHL_CMD_DIRECTLY != cmd_type && PHL_CMD_WAIT != cmd_type) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s error cmd_type(%d) \n",
			__func__, cmd_type);
		goto _exit;
	}
	para_len = sizeof(struct rtw_phl_twt_get_twt_i);
	para = _os_kmem_alloc(drv_priv, para_len);
	if (para == NULL) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s - alloc param failed!\n",
			__func__);
		goto _exit;
	}
	para->wrole = get_twt_i->wrole;
	para->id = get_twt_i->id;
	para->offset = get_twt_i->offset;
	para->tsf_h = get_twt_i->tsf_h;
	para->tsf_l = get_twt_i->tsf_l;
	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = phl_twt_get_target_wake_time(phl_i, (u8 *)para);
		_os_kmem_free(drv_priv, para, para_len);
		goto _exit;
	}
	psts = phl_cmd_enqueue(phl_i,
				para->wrole->rlink[RTW_RLINK_PRIMARY].hw_band,
				MSG_EVT_TWT_GET_TWT,
				(u8 *)para,
				para_len,
				_phl_twt_get_target_wake_time_done,
				cmd_type,
				cmd_timeout);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(drv_priv, para, para_len);
		psts = RTW_PHL_STATUS_FAILURE;
	}
_exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s psts(%d)\n", __func__, psts);
	return psts;
}

/*
 * Fill twt element
 * @twt_ele: twt element info
 * @buf: fill memory
 * @len: the length of twt element
 */
enum rtw_phl_status rtw_phl_twt_fill_twt_element(
			struct rtw_phl_twt_element *twt_ele, u8 *buf, u8 *len)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	u8 twt_para_length = 0;
	struct rtw_phl_twt_control *twt_ctrl = NULL;

	if (twt_ele == NULL || buf == NULL || len == NULL) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_fill_twt_element(): twt_ele or buf or len = NULL\n");
		return pstatus;
	}
	twt_ctrl = &twt_ele->twt_ctrl;
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_fill_twt_element(): twt_ctrl: ndp_paging_indic(%d), responder_pm_mode(%d), nego_type(%d), twt_info_frame_disable(%d), wake_dur_unit(%d)\n",
		twt_ctrl->ndp_paging_indic, twt_ctrl->responder_pm_mode,
		twt_ctrl->nego_type, twt_ctrl->twt_info_frame_disable,
		twt_ctrl->wake_dur_unit);
	*len = 0;
/*Control filed*/
	SET_TWT_CONTROL_NDP_PAGING_INDICATOR(buf, twt_ctrl->ndp_paging_indic);
	SET_TWT_CONTROL_RESPONDER_PM_MODE(buf, twt_ctrl->responder_pm_mode);
	SET_TWT_CONTROL_NEGOTIATION_TYPE(buf, twt_ctrl->nego_type);
	SET_TWT_CONTROL_TWT_INFORMATION_FRAME_DISABLE(buf,
					twt_ctrl->twt_info_frame_disable);
	SET_TWT_CONTROL_WAKE_DURATION_UNIT(buf, twt_ctrl->wake_dur_unit);
	*len += CONTROL_LENGTH;
/*TWT Parameter Information*/
	if (RTW_PHL_INDIV_TWT == twt_ctrl->nego_type) {
		pstatus = _twt_fill_individual_twt_para_set(
					&twt_ele->info.i_twt_para_set,
					twt_ctrl->ndp_paging_indic,
					buf + *len, &twt_para_length);
		*len += twt_para_length;
	} else {
		/*todo*/
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_fill_twt_element(): not support, todo, twt_ctrl->nego_type(%d)\n",
			twt_ctrl->nego_type);
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_fill_twt_element()\n");
	return pstatus;
}

/*
 * Fill twt flow field of TWT teardown frame
 * @twt_flow: twt flow field info
 * @buf: fill memory
 * @length: the length of twt flow field
 */
enum rtw_phl_status rtw_phl_twt_fill_flow_field(
		struct rtw_phl_twt_flow_field *twt_flow, u8 *buf, u16 *length)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_fill_flow_field()\n");
	if (twt_flow == NULL || buf == NULL || length == NULL) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_fill_flow_field(): twt_flow or buf or length = NULL\n");
		return pstatus;
	}
	*length = 0;
	if (RTW_PHL_INDIV_TWT == twt_flow->nego_type ||
		RTW_PHL_WAKE_TBTT_INR == twt_flow->nego_type) {
		struct rtw_phl_twt_flow_type01 *flow_info =
						&twt_flow->info.twt_flow01;
		SET_TWT_FLOW_ID(buf, flow_info->twt_flow_id);
		SET_NEGOTIATION_TYPE(buf, twt_flow->nego_type);
		SET_TEARDOWN_ALL_TWT(buf, flow_info->teardown_all);
		*length = TWT_FLOW_FIELD_LENGTH;
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} else if (RTW_PHL_BCAST_TWT  == twt_flow->nego_type) {
		SET_NEGOTIATION_TYPE(buf, twt_flow->nego_type);
		*length = TWT_FLOW_FIELD_LENGTH;
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} else if (RTW_PHL_MANAGE_BCAST_TWT == twt_flow->nego_type) {
		struct rtw_phl_twt_flow_type3 *flow_info =
						&twt_flow->info.twt_flow3;
		SET_BROADCAST_TWT_ID(buf, flow_info->bcast_twt_id);
		SET_NEGOTIATION_TYPE(buf, twt_flow->nego_type);
		SET_TEARDOWN_ALL_TWT(buf, flow_info->teardown_all);
		*length = TWT_FLOW_FIELD_LENGTH;
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} else {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_fill_flow_field(): Unknown type(%d)\n",
			twt_flow->nego_type);
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_fill_flow_field()\n");
	return pstatus;
}

/*
 * Parse twt element from pkt
 * @twt_ele: the address of twt elemant
 * @length: length of pkt
 * @twt_element: Parse info
 */
enum rtw_phl_status rtw_phl_twt_parse_element(u8 *twt_ele, u16 length,
				struct rtw_phl_twt_element *twt_element)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_twt_control *twt_ctrl = NULL;
	u8 ele_len = 0, ele_id = 0;
	u8 *next_buf = twt_ele;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_parse_element()\n");
	if (twt_ele == NULL || twt_element == NULL) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_parse_element(): twt_ele or twt_element = NULL\n");
		return pstatus;
	}
	twt_ctrl = &twt_element->twt_ctrl;
	if (length < (MIN_TWT_ELE_LEN + ELEM_ID_LEN + ELEM_LEN_LEN)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "rtw_phl_twt_parse_element(): error buffer length(%d) < %d\n",
			length, (MIN_TWT_ELE_LEN + ELEM_ID_LEN + ELEM_LEN_LEN));
		goto exit;
	}
	ele_id = GET_ELE_ID(next_buf);
	next_buf += ELEM_ID_LEN;
	ele_len = GET_ELE_LEN(next_buf);
	next_buf += ELEM_LEN_LEN;
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_parse_element(): ele_id:%d, ele_len:%d, length:%d\n",
		ele_id, ele_len, length);
	if (ele_len < MIN_TWT_ELE_LEN) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_WARNING_, "rtw_phl_twt_parse_element(): error ele length(%d) < %d\n",
			ele_len, MIN_TWT_ELE_LEN);
		goto exit;
	}
	twt_ctrl->ndp_paging_indic =
				GET_TWT_CONTROL_NDP_PAGING_INDICATOR(next_buf);
	twt_ctrl->responder_pm_mode =
				GET_TWT_CONTROL_RESPONDER_PM_MODE(next_buf);
	twt_ctrl->nego_type = GET_TWT_CONTROL_NEGOTIATION_TYPE(next_buf);
	twt_ctrl->twt_info_frame_disable =
			GET_TWT_CONTROL_TWT_INFORMATION_FRAME_DISABLE(next_buf);
	twt_ctrl->wake_dur_unit = GET_TWT_CONTROL_WAKE_DURATION_UNIT(next_buf);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_parse_element(): twt_ctrl: ndp_paging_indic(%d), responder_pm_mode(%d), nego_type(%d), twt_info_frame_disable(%d), wake_dur_unit(%d)\n",
		twt_ctrl->ndp_paging_indic, twt_ctrl->responder_pm_mode,
		twt_ctrl->nego_type, twt_ctrl->twt_info_frame_disable,
		twt_ctrl->wake_dur_unit);
	if (RTW_PHL_INDIV_TWT == twt_ctrl->nego_type) {
		pstatus = _twt_parse_individual_twt_para(twt_ele, length,
							twt_element);
	} else {
		/*todo*/
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_parse_element(): not support, todo, twt_ctrl->nego_type(%d)\n",
			twt_ctrl->nego_type);
	}
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_parse_element(): pstatus(%d)\n",
		pstatus);
	return pstatus;
}

/*
 * Parse twt setup info from pkt
 * @pkt: the address of Category of twt setup info frame
 * @length: length of pkt
 * @twt_setup_info: Parse info
 */
enum rtw_phl_status rtw_phl_twt_parse_setup_info(u8 *pkt, u16 length,
				struct rtw_phl_twt_setup_info *setup_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	u8 *twt_ele = NULL;
	u16 ele_length = length - TOKEN_OFFSET- TOKEN_LENGTH;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_parse_setup_info()\n");
	if (pkt == NULL || setup_info == NULL) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_parse_setup_info(): pkt or setup_info = NULL\n");
		return pstatus;
	}
	twt_ele = pkt + TOKEN_OFFSET + TOKEN_LENGTH;
	setup_info->dialog_token = GET_DIALOG_TOKEN(pkt + TOKEN_OFFSET);
	pstatus = rtw_phl_twt_parse_element(twt_ele, ele_length,
						&setup_info->twt_element);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_parse_setup_info(): pstatus(%d)\n",
		pstatus);
	return pstatus;
}

/*
 * Parse twt twt flow field from twt teardown frame
 * @pkt: the address of twt flow field
 * @length: length of pkt
 * @twt_flow_info: Parse info
 */
enum rtw_phl_status rtw_phl_twt_parse_flow_field(u8 *pkt, u16 length,
				struct rtw_phl_twt_flow_field *twt_flow)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_parse_flow_field()\n");
	if (pkt == NULL || twt_flow == NULL) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_parse_flow_field(): pkt or twt_flow = NULL\n");
		return pstatus;
	}
	twt_flow->nego_type = GET_NEGOTIATION_TYPE(pkt);
	if (RTW_PHL_INDIV_TWT == twt_flow->nego_type ||
		RTW_PHL_WAKE_TBTT_INR == twt_flow->nego_type) {
		struct rtw_phl_twt_flow_type01 *flow_info =
						&twt_flow->info.twt_flow01;
		flow_info->twt_flow_id = GET_TWT_FLOW_ID(pkt);
		flow_info->teardown_all = GET_TEARDOWN_ALL_TWT(pkt);
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_parse_flow_field(): nego_type:%d, twt_flow_id:%d, teardown_all:%d\n",
			twt_flow->nego_type, flow_info->twt_flow_id,
			flow_info->teardown_all);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} else if (RTW_PHL_BCAST_TWT == twt_flow->nego_type) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_parse_flow_field(): not support, todo, twt_flow->nego_type(%d)\n",
			twt_flow->nego_type);
	} else if (RTW_PHL_MANAGE_BCAST_TWT == twt_flow->nego_type) {
		struct rtw_phl_twt_flow_type3 *flow_info =
						&twt_flow->info.twt_flow3;
		flow_info->bcast_twt_id = GET_BROADCAST_TWT_ID(pkt);
		flow_info->teardown_all = GET_TEARDOWN_ALL_TWT(pkt);
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_parse_flow_field(): nego_type:%d, bcast_twt_id:%d, teardown_all:%d\n",
			twt_flow->nego_type, flow_info->bcast_twt_id,
			flow_info->teardown_all);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} else {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_parse_flow_field(): Unknown type, twt_flow->nego_type(%d)\n",
			twt_flow->nego_type);
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_parse_flow_field(): pstatus(%d)\n",
		pstatus);
	return pstatus;
}

/*
 * Tell fw which macid is announced in awake state
 * @macid: macid of sta that is in awake state
 */
enum rtw_phl_status rtw_phl_twt_sta_announce_to_fw(void *phl,
				u16 macid)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_twt_info *phl_twt_info = NULL;
	struct phl_queue *annc_q = NULL;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> rtw_phl_twt_sta_announce_to_fw()\n");
	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_sta_announce_to_fw(): twt_sup == false\n");
		return pstatus;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_sta_announce_to_fw(): twt_init == false\n");
		return pstatus;
	}
	phl_twt_info = get_twt_info(phl_info);
	annc_q = &phl_twt_info->twt_annc_queue;
	pstatus = _twt_sta_announce(phl_info, annc_q, macid);
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== rtw_phl_twt_sta_announce_to_fw(): pstatus:%d, macid: %d\n",
		pstatus, macid);
	return pstatus;
}

#if 0
/*
 * Handle twt c2h
 * @c: c2h content
 */
enum rtw_phl_status rtw_phl_twt_handle_c2h(void *phl_com, void *c)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_com_t *phl_com_info= (struct rtw_phl_com_t *)phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t*)phl_com_info->phl_priv;
	struct rtw_c2h_info *c2h = (struct rtw_c2h_info *)c;

	if (false == twt_sup(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_handle_c2h(): twt_sup == false\n");
		return pstatus;
	}
	if (false == twt_init(phl_info)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "rtw_phl_twt_handle_c2h(): twt_init == false\n");
		return pstatus;
	}
	if (C2H_FUN_WAIT_ANNC == c2h->c2h_func) {
		pstatus = _twt_handle_c2h_wait_annc(phl_info, c2h->content);
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "rtw_phl_twt_handle_c2h(): pstatus:%d cat:%d, class:%d, func:%d, len:%d, content:0x%x\n",
		pstatus, c2h->c2h_cat, c2h->c2h_class, c2h->c2h_func,
		c2h->content_len, *(c2h->content));
	return pstatus;
}
#endif

static void _phl_twt_sta_accept_twt_done(void *priv, u8 *param,
				u32 param_len, enum rtw_phl_status sts)
{
	if (param) {
		struct rtw_phl_twt_sta_accept_i *accept_i =
				(struct rtw_phl_twt_sta_accept_i *)param;

		if (accept_i->accept_done) {
			accept_i->accept_done(priv, accept_i->sta,
						&accept_i->setup_info, sts);
		}
		_os_kmem_free(priv, param, param_len);
		param = NULL;
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "%s\n", __func__);
	}
}

enum rtw_phl_status phl_twt_accept_for_sta_mode(struct phl_info_t *phl,
			u8 *param)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_twt_sta_accept_i *accept_i =
				(struct rtw_phl_twt_sta_accept_i *)param;
	struct rtw_phl_stainfo_t *sta = accept_i->sta;
	struct rtw_phl_twt_setup_info *setup_i = &accept_i->setup_info;
	struct rtw_phl_twt_element *element = &setup_i->twt_element;
	struct rtw_phl_twt_control *twt_ctrl =&element->twt_ctrl;
	u8 cfg_id = 0;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> %s()\n",
		__func__);
	if (false == twt_sup(phl)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s: twt_sup == false\n",
			__func__);
		return pstatus;
	}
	if (false == twt_init(phl)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s: twt_init == false\n",
			__func__);
		return pstatus;
	}
	if (RTW_PHL_INDIV_TWT == twt_ctrl->nego_type) {
		pstatus = _twt_accept_indiv_by_sta(phl, setup_i, sta, &cfg_id);
	} else {
		pstatus = _twt_accept_bcast_by_sta(phl, setup_i, sta, &cfg_id);
	}
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== %s: pstatus:%d, config_id:%d\n",
		__func__, pstatus, cfg_id);
	return pstatus;
}

/*
 * Handle sta to config twt when sta accept the twt agreement
 * @phl_sta: sta entry that you wnat to config twt
 * @setup_info: twt setup info
 * @id: Output the id of twt confi entry
 * Note: for sta mode
 */
enum rtw_phl_status rtw_phl_twt_accept_for_sta_mode(void *phl,
				struct rtw_phl_twt_sta_accept_i *accept_i)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_phl_twt_sta_accept_i *para = NULL;
	u32 para_len = 0;

	para_len = sizeof(struct rtw_phl_twt_sta_accept_i);
	para = _os_kmem_alloc(drv_priv, para_len);
	if (para == NULL) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s - alloc param failed!\n",
			__func__);
		goto _exit;
	}
	para->accept_done = accept_i->accept_done;
	para->sta = accept_i->sta;
	_os_mem_cpy(drv_priv, &para->setup_info, &accept_i->setup_info,
		sizeof(struct rtw_phl_twt_setup_info));
	psts = phl_cmd_enqueue(phl_info,
				para->sta->rlink->hw_band,
				MSG_EVT_TWT_STA_ACCEPT,
				(u8 *)para,
				para_len,
				_phl_twt_sta_accept_twt_done,
				PHL_CMD_NO_WAIT,
				0);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(drv_priv, para, para_len);
		psts = RTW_PHL_STATUS_FAILURE;
	}
_exit:
	return psts;
}

static void _phl_twt_sta_teardown_done(void *priv, u8 *param,
				u32 param_len, enum rtw_phl_status sts)
{
	if (param) {
		struct rtw_phl_twt_sta_teardown_i *teardown_i =
				(struct rtw_phl_twt_sta_teardown_i *)param;

		if (teardown_i->teardown_done) {
			teardown_i->teardown_done(priv, teardown_i->sta, sts);
		}
		_os_kmem_free(priv, param, param_len);
		param = NULL;
		PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "%s\n", __func__);
	}
}

enum rtw_phl_status phl_twt_teardown_for_sta_mode(struct phl_info_t *phl,
				u8 *param)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_twt_sta_teardown_i *teardown_i =
				(struct rtw_phl_twt_sta_teardown_i *)param;
	u8 bitmap =0; /*bitmap of empty config of twt*/
	u8 i = 0;

	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "==> %s()\n",
		__func__);
	if (false == twt_sup(phl)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s: twt_sup == false\n",
			__func__);
		goto exit;
	}
	if (false == twt_init(phl)) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s: twt_init == false\n",
			__func__);
		goto exit;
	}
	pstatus = rtw_phl_twt_teardown_sta(phl, teardown_i->sta,
						&teardown_i->twt_flow, &bitmap);
	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		goto exit;
	if (bitmap == 0)
		goto exit;
	i = 0;
	do {
		if (((bitmap >> i) & BIT0)) {
			pstatus = rtw_phl_twt_free_twt_config(phl, i);
			PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "%s: sta Q is empty in twt config entry(%d), we free it, pstatus:%d \n",
				__func__, i, pstatus);
		}
		i++;
	} while ((bitmap >> i) != 0);
exit:
	PHL_TRACE(COMP_PHL_TWT, _PHL_INFO_, "<== %s: pstatus(%d)\n",
		__func__, pstatus);
	return pstatus;
}

/*
 * Handle sta to disable twt when sta tx/rx twt teardown frame
 * @struct teardown_i:
 * @phl_sta: sta entry that you wnat to config twt
 * @twt_flow: twt flow field info
 * @teardown_done: callback function after process cmd done
 * Note: for sta mode.
 */
enum rtw_phl_status rtw_phl_twt_teardown_for_sta_mode(void *phl,
			struct rtw_phl_twt_sta_teardown_i *teardown_i)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_twt_sta_teardown_i *para = NULL;
	u32 para_len = 0;

	para_len = sizeof(struct rtw_phl_twt_sta_teardown_i);
	para = _os_kmem_alloc(drv_priv, para_len);
	if (para == NULL) {
		PHL_TRACE(COMP_PHL_TWT, _PHL_ERR_, "%s - alloc param failed!\n", __func__);
		goto _exit;
	}
	para->teardown_done = teardown_i->teardown_done;
	para->sta = teardown_i->sta;
	_os_mem_cpy(drv_priv, &para->twt_flow, &teardown_i->twt_flow,
			sizeof(struct rtw_phl_twt_flow_field));
	psts = phl_cmd_enqueue(phl_info,
				para->sta->rlink->hw_band,
				MSG_EVT_TWT_STA_TEARDOWN,
				(u8 *)para,
				para_len,
				_phl_twt_sta_teardown_done,
				PHL_CMD_NO_WAIT,
				0);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(drv_priv, para, para_len);
		psts = RTW_PHL_STATUS_FAILURE;
	}
_exit:
	return psts;
}

#endif /* CONFIG_PHL_TWT */
