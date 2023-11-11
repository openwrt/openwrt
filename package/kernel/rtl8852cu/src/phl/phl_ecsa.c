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
#define _PHL_ECSA_C_
#include "phl_headers.h"

#ifdef CONFIG_PHL_ECSA
void
_phl_ecsa_dump_param(
	struct rtw_phl_ecsa_param *param
)
{
	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: Channel %d\n", __FUNCTION__,
		  param->ch);
	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: Op class %d\n", __FUNCTION__,
		  param->op_class);
	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: Count %d\n", __FUNCTION__,
		  param->count);
	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: Mode %d\n", __FUNCTION__,
		  param->mode);
	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: Delay time %d\n", __FUNCTION__,
		  param->delay_start_ms);
	PHL_DUMP_CHAN_DEF(&(param->new_chan_def));
}

enum rtw_phl_status
_phl_ecsa_tx_pause(
	struct phl_ecsa_ctrl_t *ecsa_ctrl
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_com_t *phl_com = ecsa_ctrl->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct rtw_wifi_role_link_t *rlink = ecsa_ctrl->rlink;

	/* Pause SW Tx */
	rtw_phl_tx_stop(phl_info);
	rtw_phl_tx_req_notify(phl_info);

	/* Disable hw tx all  */
	if (rtw_hal_dfs_pause_tx(phl_info->hal, rlink->hw_band, true,  PAUSE_RSON_DFS) ==
	    RTW_HAL_STATUS_SUCCESS) {
		status = RTW_PHL_STATUS_SUCCESS;
		PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "[ECSA] hw tx pause OK\n");
	} else {
		status = RTW_PHL_STATUS_FAILURE;
		PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "[ECSA] hw tx pause fail\n");
	}

	return status;
}

enum rtw_phl_status
_phl_ecsa_tx_resume(
	struct phl_ecsa_ctrl_t *ecsa_ctrl
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_com_t *phl_com = ecsa_ctrl->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct rtw_wifi_role_link_t *rlink = ecsa_ctrl->rlink;

	/* Enable hw tx all  */
	if (rtw_hal_dfs_pause_tx(phl_info->hal, rlink->hw_band, false, PAUSE_RSON_DFS) ==
	    RTW_HAL_STATUS_SUCCESS) {
		status = RTW_PHL_STATUS_SUCCESS;
		PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "[ECSA] hw tx unpause OK\n");
	} else {
		status = RTW_PHL_STATUS_FAILURE;
		PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "[ECSA] hw tx unpause fail\n");
	}

	rtw_phl_tx_resume(phl_info);

	return status;
}

u32
_phl_ecsa_calculate_next_timer_ap(
	struct phl_ecsa_ctrl_t *ecsa_ctrl
)
{
	struct rtw_phl_com_t *phl_com = ecsa_ctrl->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct rtw_bcn_info_cmn *bcn_cmn = NULL;
	u32 tsf_h = 0, tsf_l = 0;
	u64 tsf = 0;
	u32 beacon_period_us = 0, timeslot_us = 0, next_timeslot_us = 0;
	u32 current_time_ms = _os_get_cur_time_ms();
	struct rtw_wifi_role_link_t *rlink = ecsa_ctrl->rlink;

	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_get_tsf(phl_info->hal,
	                                              rlink->hw_band,
	                                              rlink->hw_port,
	                                              &tsf_h,
	                                              &tsf_l)) {
		PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "_phl_ecsa_timer_callback(): Get tsf fail\n");
		return 0;
	}
	tsf = tsf_h;
	tsf = tsf << 32;
	tsf |= tsf_l;

	bcn_cmn = &rlink->bcn_cmn;
	beacon_period_us = bcn_cmn->bcn_interval * TU;

	timeslot_us = (u32)_os_modular64(tsf, beacon_period_us);

	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: CurTimeMs = %d State = %x timeslot = %d\n",
		  __FUNCTION__, current_time_ms, ecsa_ctrl->state, timeslot_us);

	if(ecsa_ctrl->state == ECSA_STATE_START){
		next_timeslot_us = beacon_period_us - timeslot_us + (2 * TU);
	}
	/* To make sure first ECSA IE show in Beacon */
	else if(ecsa_ctrl->state == ECSA_STATE_UPDATE_FIRST_BCN_DONE){
		next_timeslot_us = (beacon_period_us - timeslot_us -
				    ECSA_UPDATE_BCN_BEFORE_TBTT_US);
		ecsa_ctrl->expected_tbtt_ms = current_time_ms +
				  (beacon_period_us - timeslot_us)/1000;
	}
	else if(ecsa_ctrl->state == ECSA_STATE_COUNT_DOWN){
		if(ecsa_ctrl->ecsa_param.count == 1){
			next_timeslot_us = (beacon_period_us - timeslot_us) +
					ECSA_SWITCH_TIME_AFTER_LAST_COUNT_DOWN;
		}
		else{
			next_timeslot_us = (beacon_period_us - timeslot_us) +
					(beacon_period_us - ECSA_UPDATE_BCN_BEFORE_TBTT_US);

			ecsa_ctrl->expected_tbtt_ms = current_time_ms +
					(2 * beacon_period_us - timeslot_us)/1000;
		}
	}

	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: Expected tbtt %d!\n", __FUNCTION__, ecsa_ctrl->expected_tbtt_ms);
	return next_timeslot_us/1000;
}

u32
_phl_ecsa_calculate_next_timer_sta(
	struct phl_ecsa_ctrl_t *ecsa_ctrl
)
{
	struct rtw_wifi_role_link_t *rlink = ecsa_ctrl->rlink;
	struct rtw_phl_com_t *phl_com = ecsa_ctrl->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct rtw_phl_stainfo_t *sta = NULL;
	u32 beacon_period_us = 0, next_timeslot = 0;
	u32 current_time_ms = 0;

	current_time_ms = _os_get_cur_time_ms();
	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: CurTimeMs = %d State = %x\n",
		  __FUNCTION__, current_time_ms, ecsa_ctrl->state);

	sta = rtw_phl_get_stainfo_self(phl_info, rlink);
	if(sta == NULL){
		PHL_TRACE(COMP_PHL_ECSA, _PHL_ERR_, "%s: Get sta info fail!\n",
			  __FUNCTION__);
		return 0;
	}

	beacon_period_us = sta->asoc_cap.bcn_interval * TU;

	if(ecsa_ctrl->state == ECSA_STATE_START){
		next_timeslot = 0;
	}
	else if(ecsa_ctrl->state == ECSA_STATE_COUNT_DOWN){
		u8 count = ecsa_ctrl->ecsa_param.count;
		next_timeslot = (beacon_period_us * count) / 1000; /* ms */
	}
	else if(ecsa_ctrl->state == ECSA_STATE_SWITCH){
		next_timeslot = 1000; /* 1s */
	}

	return next_timeslot;
}

void
_phl_ecsa_calculate_next_timer(
	struct phl_ecsa_ctrl_t *ecsa_ctrl
)
{
	struct rtw_phl_com_t *phl_com = ecsa_ctrl->phl_com;
	void *d = phlcom_to_drvpriv(phl_com);
	u32 next_timeslot = 0; /* ms */

	if(IS_ECSA_TYPE_AP(ecsa_ctrl))
		next_timeslot = _phl_ecsa_calculate_next_timer_ap(ecsa_ctrl);

	if(IS_ECSA_TYPE_STA(ecsa_ctrl))
		next_timeslot = _phl_ecsa_calculate_next_timer_sta(ecsa_ctrl);;
	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: Next time slot %d!\n", __FUNCTION__, next_timeslot);
	_os_set_timer(d, &ecsa_ctrl->timer, next_timeslot);

}

void _phl_ecsa_state_change_ap(
	struct phl_ecsa_ctrl_t *ecsa_ctrl
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_com_t *phl_com = ecsa_ctrl->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	void *d = phlcom_to_drvpriv(phl_com);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_ECSA);

	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: CurTimeMs = %d State = %x\n",
		  __FUNCTION__, _os_get_cur_time_ms(), ecsa_ctrl->state);

	/* Protect ECSA state change to prevent timer callback racing */
	_os_spinlock(d, &(ecsa_ctrl->lock), _bh, NULL);

	if(ecsa_ctrl->state == ECSA_STATE_WAIT_DELAY){
		status = rtw_phl_ecsa_cmd_request(phl_info, ecsa_ctrl->role);
		if(status != RTW_PHL_STATUS_SUCCESS){
			PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
				  "%s: ECSA command fail!\n", __FUNCTION__);
			ecsa_ctrl->state = ECSA_STATE_NONE;
		}
		else{
			ecsa_ctrl->state = ECSA_STATE_START;
		}

	}
	else if(ecsa_ctrl->state == ECSA_STATE_START){
		ecsa_ctrl->state = ECSA_STATE_UPDATE_FIRST_BCN_DONE;
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ECSA_UPDATE_FIRST_BCN_DONE);

		status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
		if(status != RTW_PHL_STATUS_SUCCESS)
			PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: Send msg fail!\n", __FUNCTION__);
	}
	/* To make sure first ECSA IE show in Beacon */
	else if(ecsa_ctrl->state == ECSA_STATE_UPDATE_FIRST_BCN_DONE){
		ecsa_ctrl->state = ECSA_STATE_COUNT_DOWN;
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ECSA_COUNT_DOWN);

		status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
		if(status != RTW_PHL_STATUS_SUCCESS)
			PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: Send msg fail!\n", __FUNCTION__);
	}
	else if(ecsa_ctrl->state == ECSA_STATE_COUNT_DOWN){
		if(ecsa_ctrl->ecsa_param.count == 1){
			ecsa_ctrl->state = ECSA_STATE_SWITCH;
			SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ECSA_SWITCH_START);
			msg.rsvd[0].ptr = (u8*)ecsa_ctrl->role;
			msg.rsvd[1].ptr = (u8*)ecsa_ctrl->rlink;
			status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
			if(status != RTW_PHL_STATUS_SUCCESS)
				PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: Send msg fail!\n", __FUNCTION__);
		}
		else{
			SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ECSA_COUNT_DOWN);

			status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
			if(status != RTW_PHL_STATUS_SUCCESS)
				PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: Send msg fail!\n", __FUNCTION__);
		}
	}
	_os_spinunlock(d, &(ecsa_ctrl->lock), _bh, NULL);
}

void _phl_ecsa_state_change_sta(
	struct phl_ecsa_ctrl_t *ecsa_ctrl
)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_com_t *phl_com = ecsa_ctrl->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	void *d = phlcom_to_drvpriv(phl_com);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_ECSA);

	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: CurTimeMs = %d State = %x\n",
		  __FUNCTION__, _os_get_cur_time_ms(), ecsa_ctrl->state);

	/* Protect ECSA state change to prevent timer callback racing */
	_os_spinlock(d, &(ecsa_ctrl->lock), _bh, NULL);

	if(ecsa_ctrl->state == ECSA_STATE_WAIT_DELAY){
		status = rtw_phl_ecsa_cmd_request(phl_info, ecsa_ctrl->role);
		if(status != RTW_PHL_STATUS_SUCCESS){
			PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
				  "%s: ECSA command fail!\n", __FUNCTION__);
			ecsa_ctrl->state = ECSA_STATE_NONE;
		}
		else{
			ecsa_ctrl->state = ECSA_STATE_START;
		}
	}
	else if(ecsa_ctrl->state == ECSA_STATE_START){
		ecsa_ctrl->state = ECSA_STATE_COUNT_DOWN;
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ECSA_COUNT_DOWN);

		status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
		if(status != RTW_PHL_STATUS_SUCCESS)
			PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: Send msg fail!\n", __FUNCTION__);
	}
	else if(ecsa_ctrl->state == ECSA_STATE_COUNT_DOWN){
		ecsa_ctrl->state = ECSA_STATE_SWITCH;
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ECSA_SWITCH_START);
		msg.rsvd[0].ptr = (u8*)ecsa_ctrl->role;
		msg.rsvd[1].ptr = (u8*)ecsa_ctrl->rlink;
		status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
		if(status != RTW_PHL_STATUS_SUCCESS)
			PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: Send msg fail!\n", __FUNCTION__);
	}
	else if(ecsa_ctrl->state == ECSA_STATE_SWITCH){
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ECSA_CHECK_TX_RESUME);
		status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
		if(status != RTW_PHL_STATUS_SUCCESS)
			PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: Send msg fail!\n", __FUNCTION__);
	}
	_os_spinunlock(d, &(ecsa_ctrl->lock), _bh, NULL);
}

void
_phl_ecsa_timer_callback(
	void *context
	)
{
	struct phl_ecsa_ctrl_t *ecsa_ctrl = (struct phl_ecsa_ctrl_t *)context;

	if(IS_ECSA_TYPE_AP(ecsa_ctrl))
		_phl_ecsa_state_change_ap(ecsa_ctrl);

	if(IS_ECSA_TYPE_STA(ecsa_ctrl))
		_phl_ecsa_state_change_sta(ecsa_ctrl);
}

void
_phl_ecsa_cmd_abort_hdlr(
	void* dispr,
	void* priv,
	bool abort
)
{
	struct phl_ecsa_ctrl_t *ecsa_ctrl = (struct phl_ecsa_ctrl_t *)priv;
	struct rtw_wifi_role_t *wifi_role = ecsa_ctrl->role;
	struct rtw_phl_com_t *phl_com = wifi_role->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct rtw_phl_ecsa_ops *ops = &ecsa_ctrl->ops;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	void *d = phlcom_to_drvpriv(phl_com);

	_os_cancel_timer(d, &ecsa_ctrl->timer);

	/* ECSA AP abort handle */
	if(IS_ECSA_TYPE_AP(ecsa_ctrl) &&
	   ecsa_ctrl->ecsa_param.flag != 0){
		ecsa_ctrl->state = ECSA_STATE_NONE;
		CLEAR_STATUS_FLAG(ecsa_ctrl->ecsa_param.flag,
				  ECSA_PARAM_FLAG_APPEND_BCN);
		CLEAR_STATUS_FLAG(ecsa_ctrl->ecsa_param.flag,
				  ECSA_PARAM_FLAG_APPEND_PROBERSP);
		/* Update Bcn */
		if(ops->update_beacon)
			ops->update_beacon(ops->priv, wifi_role, ecsa_ctrl->rlink);
	}

	/* ECSA STA abort handle */
	if(IS_ECSA_TYPE_STA(ecsa_ctrl)){
		if(ecsa_ctrl->ecsa_param.mode == true)
			_phl_ecsa_tx_resume(ecsa_ctrl);

		if(ops->ecsa_complete)
			ops->ecsa_complete(ops->priv, wifi_role);
	}

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_ECSA);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ECSA_DONE);

	if(abort)
		attr.opt = MSG_OPT_SEND_IN_ABORT;

	pstatus = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);

	if(pstatus != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s:[ECSA] dispr_send_msg failed (0x%X)\n",
			__FUNCTION__, pstatus);
	}
}

enum phl_mdl_ret_code
_phl_ecsa_cmd_acquired(
	void* dispr,
	void* priv)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_ecsa_ctrl_t *ecsa_ctrl = (struct phl_ecsa_ctrl_t *)priv;
	struct rtw_wifi_role_t *wifi_role = ecsa_ctrl->role;
	struct rtw_phl_com_t *phl_com = wifi_role->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_ECSA);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ECSA_START);

	status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if(status != RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: Send msg fail!\n", __FUNCTION__);
		goto exit;
	}

	ret = MDL_RET_SUCCESS;
exit:
	return ret;
}

enum phl_mdl_ret_code
_phl_ecsa_cmd_abort(
	void* dispr,
	void* priv)
{
	_phl_ecsa_cmd_abort_hdlr(dispr, priv, true);
	return MDL_RET_SUCCESS;
}

enum phl_mdl_ret_code
_phl_ecsa_cmd_msg_hdlr(
	void* dispr,
	void* priv,
	struct phl_msg* msg)
{
	struct phl_ecsa_ctrl_t *ecsa_ctrl = (struct phl_ecsa_ctrl_t *)priv;
	struct rtw_wifi_role_t *wifi_role = ecsa_ctrl->role;
	struct rtw_wifi_role_link_t *rlink = ecsa_ctrl->rlink;
	u8 hw_band = rlink->hw_band;
	struct rtw_phl_com_t *phl_com = wifi_role->phl_com;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	void *d = phlcom_to_drvpriv(phl_com);
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_msg nextmsg = {0};
	struct phl_msg_attribute attr = {0};
	struct rtw_phl_ecsa_ops *ops = &ecsa_ctrl->ops;
	u32 current_time_ms = _os_get_cur_time_ms();
	struct rtw_bcn_info_cmn *bcn_cmn = &rlink->bcn_cmn;
	u32 beacon_period_ms = bcn_cmn->bcn_interval * TU / 1000;
	u8 countdown_n = 1;
	struct rtw_chan_def chdef_to_switch = {0};

	if(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_ECSA) {
		return MDL_RET_IGNORE;
	}

	if(IS_MSG_FAIL(msg->msg_id)) {
		_phl_ecsa_cmd_abort_hdlr(dispr, priv, false);
		status = phl_disp_eng_free_token(phl_info,
		                                 hw_band,
		                                 &ecsa_ctrl->req_hdl);
		if(status != RTW_PHL_STATUS_SUCCESS)
			PHL_WARN("%s: Free token fail!\n", __FUNCTION__);
		return MDL_RET_SUCCESS;
	}

	SET_MSG_MDL_ID_FIELD(nextmsg.msg_id, PHL_FG_MDL_ECSA);

	switch(MSG_EVT_ID_FIELD(msg->msg_id)){
		case MSG_EVT_ECSA_START:
			PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: MSG_EVT_ECSA_START\n", __FUNCTION__);
			if(IS_ECSA_TYPE_AP(ecsa_ctrl)){
				SET_STATUS_FLAG(ecsa_ctrl->ecsa_param.flag,
						ECSA_PARAM_FLAG_APPEND_BCN);
				/* Update Bcn */
				if(ops->update_beacon)
					ops->update_beacon(ops->priv, wifi_role, ecsa_ctrl->rlink);
			}

			if(IS_ECSA_TYPE_STA(ecsa_ctrl) &&
			   ecsa_ctrl->ecsa_param.mode == true){
				_phl_ecsa_tx_pause(ecsa_ctrl);
			}
			_phl_ecsa_calculate_next_timer(ecsa_ctrl);
			break;
		case MSG_EVT_ECSA_UPDATE_FIRST_BCN_DONE:
			PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: MSG_EVT_ECSA_UPDATE_FIRST_BCN_DONE\n", __FUNCTION__);
			SET_STATUS_FLAG(ecsa_ctrl->ecsa_param.flag,
					ECSA_PARAM_FLAG_APPEND_PROBERSP);
			_phl_ecsa_calculate_next_timer(ecsa_ctrl);
			break;
		case MSG_EVT_ECSA_COUNT_DOWN:
			PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: MSG_EVT_ECSA_COUNT_DOWN\n", __FUNCTION__);

			/* Count down mode of STA ECSA only calculate the switch time */
			if(IS_ECSA_TYPE_STA(ecsa_ctrl)){
				_phl_ecsa_calculate_next_timer(ecsa_ctrl);
				break;
			}

			/* Count down mode of AP ECSA calculate the update beacon time */
			if(ecsa_ctrl->expected_tbtt_ms > current_time_ms){
				countdown_n = 1;
			}
			else{
				/*
				 * There may be delay time during msg delivery,
				 * calulate the actual countdown value
				 */
				countdown_n = (u8)((current_time_ms-(ecsa_ctrl->expected_tbtt_ms))%beacon_period_ms+1);

			}

			PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: count down %d\n", __FUNCTION__, countdown_n);

			if(ecsa_ctrl->ecsa_param.count > countdown_n){
				ecsa_ctrl->ecsa_param.count -= countdown_n;
				/* Update Bcn */
				if(ops->update_beacon)
					ops->update_beacon(ops->priv, wifi_role, ecsa_ctrl->rlink);

				_phl_ecsa_calculate_next_timer(ecsa_ctrl);
			}
			else{
				/*
				 * If the countdown value is less than 1,
				 * we have to switch channel immediately
				 */
				ecsa_ctrl->ecsa_param.count = 0;
				ecsa_ctrl->state = ECSA_STATE_SWITCH;
				SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_ECSA_SWITCH_START);
				nextmsg.rsvd[0].ptr = (u8*)ecsa_ctrl->role;
				nextmsg.rsvd[1].ptr = (u8*)ecsa_ctrl->rlink;

				status = phl_disp_eng_send_msg(phl_info,
							       &nextmsg,
							       &attr,
							       NULL);
				if(status != RTW_PHL_STATUS_SUCCESS)
					PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: Send msg fail!\n", __FUNCTION__);
			}
			break;
		case MSG_EVT_ECSA_SWITCH_START:
			PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: MSG_EVT_ECSA_SWITCH_START\n", __FUNCTION__);

			/* Update channel info */
			if(ops->update_chan_info){
				ops->update_chan_info(ops->priv,
				                      wifi_role,
				                      ecsa_ctrl->rlink,
				                      ecsa_ctrl->ecsa_param.new_chan_def);

				PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: update_chan_info done!\n", __FUNCTION__);
			}
			else{
				PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: update_chan_info is NULL!\n", __FUNCTION__);
			}

			/* AP mode ECSA should update beacon to remove ECSA IE and update the channel info */
			if(IS_ECSA_TYPE_AP(ecsa_ctrl)){
				CLEAR_STATUS_FLAG(ecsa_ctrl->ecsa_param.flag,
						  ECSA_PARAM_FLAG_APPEND_BCN);
				CLEAR_STATUS_FLAG(ecsa_ctrl->ecsa_param.flag,
						  ECSA_PARAM_FLAG_APPEND_PROBERSP);
				/* Update Bcn */
				if(ops->update_beacon)
					ops->update_beacon(ops->priv, wifi_role, ecsa_ctrl->rlink);
			}

			/*
			 * We should use chandef of the chanctx to switch,
			 * the bw may not be same as the ECSA operating class
			 * because of the SCC mode with different bandwidth.
			 */
			if (rlink->chanctx != NULL) {
				_os_mem_cpy(d, &chdef_to_switch, &(rlink->chanctx->chan_def),
				            sizeof(struct rtw_chan_def));
				if(rlink->chanctx->chan_def.chan !=
				   ecsa_ctrl->ecsa_param.new_chan_def.chan)
					PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
				                  "%s: channel is not same as ECSA parameter!\n",
				                  __FUNCTION__);
			} else {
				_os_mem_cpy(d, &chdef_to_switch, &(ecsa_ctrl->ecsa_param.new_chan_def),
					    sizeof(struct rtw_chan_def));
				PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
				  	  "%s: chanctx of role is NULL use ECSA parameter!\n",
					  __FUNCTION__);
			}

			/* Switch channel */
			phl_set_ch_bw(phl_info, hw_band, &chdef_to_switch, true);

			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_ECSA_SWITCH_DONE);
			nextmsg.rsvd[0].ptr =  (u8*)ecsa_ctrl->role;
			nextmsg.rsvd[1].ptr = (u8*)ecsa_ctrl->rlink;

			status = phl_disp_eng_send_msg(phl_info,
						       &nextmsg,
						       &attr,
						       NULL);
			if(status != RTW_PHL_STATUS_SUCCESS)
				PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
					  "%s: Send msg fail!\n", __FUNCTION__);
			break;
		case MSG_EVT_ECSA_SWITCH_DONE:
			PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: MSG_EVT_ECSA_SWITCH_DONE\n", __FUNCTION__);
			if(IS_ECSA_TYPE_STA(ecsa_ctrl) &&
			   ecsa_ctrl->ecsa_param.mode == true){
				SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_ECSA_CHECK_TX_RESUME);
				status = phl_disp_eng_send_msg(phl_info, &nextmsg, &attr, NULL);
				if(status != RTW_PHL_STATUS_SUCCESS)
					PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
						"%s: Send msg fail!\n", __FUNCTION__);
				break;
			}

			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_ECSA_DONE);
			status = phl_disp_eng_send_msg(phl_info,
						       &nextmsg,
						       &attr,
						       NULL);
			if(status != RTW_PHL_STATUS_SUCCESS)
				PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
					  "%s: Send msg fail!\n", __FUNCTION__);
			break;
		case MSG_EVT_ECSA_CHECK_TX_RESUME:
			PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: MSG_EVT_ECSA_CHECK_TX_RESUME\n", __FUNCTION__);
			if(IS_ECSA_TYPE_STA(ecsa_ctrl) &&
			   ecsa_ctrl->ecsa_param.mode == true){
				/*
				 * TODO: If driver support DFS-slave with radar
				 * detection, ECSA should tx un-pause directly
				 * and the tx pause should be handled by DFS-slave.
				 */
				if(ops->check_tx_resume_allow){
					if(!ops->check_tx_resume_allow(ops->priv, wifi_role)){
						PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  			  "%s: Keep Tx pause...\n", __FUNCTION__);
						_phl_ecsa_calculate_next_timer(ecsa_ctrl);
						break;
					}
				}
				PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  	  "%s: Tx resume!\n", __FUNCTION__);
				_phl_ecsa_tx_resume(ecsa_ctrl);
			}

			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_ECSA_DONE);
			status = phl_disp_eng_send_msg(phl_info, &nextmsg, &attr, NULL);
			if(status != RTW_PHL_STATUS_SUCCESS)
				PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
					  "%s: Send msg fail!\n", __FUNCTION__);

			break;
		case MSG_EVT_ECSA_DONE:
			PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_,
				  "%s: MSG_EVT_ECSA_DONE\n", __FUNCTION__);
			ecsa_ctrl->state = ECSA_STATE_NONE;

			if(ops->ecsa_complete){
				ops->ecsa_complete(ops->priv, wifi_role);
			}
			else{
				PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
			  "%s: ecsa_complete is NULL!\n", __FUNCTION__);
			}

			status = phl_disp_eng_free_token(phl_info,
			                                 hw_band,
			                                 &ecsa_ctrl->req_hdl);
			if(status != RTW_PHL_STATUS_SUCCESS)
				PHL_WARN("%s: Free token fail!\n", __FUNCTION__);
			break;
		default:
			break;
	}
	return ret;
}

enum phl_mdl_ret_code
_phl_ecsa_cmd_set_info(
	void* dispr,
	void* priv,
	struct phl_module_op_info* info)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;

	/* PHL_INFO(" %s :: info->op_code=%d \n", __func__, info->op_code); */
	return ret;
}

enum phl_mdl_ret_code
_phl_ecsa_cmd_query_info(
	void* dispr,
	void* priv,
	struct phl_module_op_info* info)
{
	struct phl_ecsa_ctrl_t *ecsa_ctrl = (struct phl_ecsa_ctrl_t *)priv;
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	/* PHL_INFO(" %s :: info->op_code=%d \n", __func__, info->op_code); */

	switch(info->op_code) {
		case FG_REQ_OP_GET_ROLE:
			info->outbuf = (u8*)ecsa_ctrl->role;
			ret = MDL_RET_SUCCESS;
			break;

		case FG_REQ_OP_GET_ROLE_LINK:
			info->outbuf = (u8*)ecsa_ctrl->rlink;
			ret = MDL_RET_SUCCESS;
			break;

		default:
			break;
	}

	return ret;
}

enum rtw_phl_status
rtw_phl_ecsa_cmd_request(
	void *phl,
	struct rtw_wifi_role_t *role
	)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_ecsa_ctrl_t *ecsa_ctrl =
		(struct phl_ecsa_ctrl_t *)phl_info->ecsa_ctrl;
	struct phl_cmd_token_req req={0};

	if(ecsa_ctrl == NULL)
		goto exit;

	/* Fill foreground command request */
	req.module_id= PHL_FG_MDL_ECSA;
	req.priv = ecsa_ctrl;
	req.role = role;

	req.acquired = _phl_ecsa_cmd_acquired;
	req.abort = _phl_ecsa_cmd_abort;
	req.msg_hdlr = _phl_ecsa_cmd_msg_hdlr;
	req.set_info = _phl_ecsa_cmd_set_info;
	req.query_info = _phl_ecsa_cmd_query_info;

	status = phl_disp_eng_add_token_req(phl,
	                                    ecsa_ctrl->rlink->hw_band,
	                                    &req,
	                                    &ecsa_ctrl->req_hdl);
	if((status != RTW_PHL_STATUS_SUCCESS) &&
	   (status != RTW_PHL_STATUS_PENDING))
		goto exit;

	status = RTW_PHL_STATUS_SUCCESS;

exit:
	return status;
}

enum rtw_phl_status
rtw_phl_ecsa_start(void *phl,
                   struct rtw_wifi_role_t *role,
                   struct rtw_wifi_role_link_t *rlink,
                   struct rtw_phl_ecsa_param *param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *d = phlcom_to_drvpriv(phl_info->phl_com);
	struct phl_ecsa_ctrl_t *ecsa_ctrl =
		(struct phl_ecsa_ctrl_t *)phl_info->ecsa_ctrl;
	struct rtw_phl_ecsa_param *ecsa_param = &(ecsa_ctrl->ecsa_param);

	if(ecsa_ctrl == NULL)
		return RTW_PHL_STATUS_FAILURE;
	if(ecsa_ctrl->state != ECSA_STATE_NONE){
		PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_, "%s: ECSA already started!\n",
			  __FUNCTION__);
		return RTW_PHL_STATUS_FAILURE;
	}

	ecsa_ctrl->role = role;
	ecsa_ctrl->rlink = rlink;

	_os_mem_cpy(d, ecsa_param, param, sizeof(struct rtw_phl_ecsa_param));
	_phl_ecsa_dump_param(ecsa_param);
	ecsa_ctrl->state = ECSA_STATE_WAIT_DELAY;
	PHL_TRACE(COMP_PHL_ECSA, _PHL_INFO_, "%s: ECSA start after %dms !\n",
		  __FUNCTION__, ecsa_ctrl->ecsa_param.delay_start_ms);
	_os_set_timer(d, &ecsa_ctrl->timer, ecsa_ctrl->ecsa_param.delay_start_ms);

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
rtw_phl_ecsa_cancel(
	void *phl,
	struct rtw_wifi_role_t *role
	)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *d = phlcom_to_drvpriv(phl_info->phl_com);
	struct phl_ecsa_ctrl_t *ecsa_ctrl =
		(struct phl_ecsa_ctrl_t *)phl_info->ecsa_ctrl;

	PHL_INFO("%s >> \n", __func__);

	if(ecsa_ctrl == NULL){
		status = RTW_PHL_STATUS_FAILURE;
		goto exit;
	}

	if(ecsa_ctrl->state == ECSA_STATE_NONE) {
		PHL_INFO("%s, skip (already ECSA_STATE_NONE)\n", __func__);
		goto exit;
	}

	_os_cancel_timer(d, &ecsa_ctrl->timer);

	_os_spinlock(d, &(ecsa_ctrl->lock), _bh, NULL);
	if(ecsa_ctrl->state > ECSA_STATE_WAIT_DELAY){
		status = phl_disp_eng_cancel_token_req(phl_info,
		                                       ecsa_ctrl->rlink->hw_band,
		                                       &ecsa_ctrl->req_hdl);

		if(status != RTW_PHL_STATUS_SUCCESS){
			PHL_TRACE(COMP_PHL_ECSA, _PHL_WARNING_,
				"%s: ECSA cancel req fail!\n", __FUNCTION__);
		}

	}
	else{
		ecsa_ctrl->state = ECSA_STATE_NONE;
	}
	_os_spinunlock(d, &(ecsa_ctrl->lock), _bh, NULL);

exit:
	PHL_INFO("%s << (%d)\n", __func__, status);
	return status;
}

enum rtw_phl_status
rtw_phl_ecsa_get_param(
	void *phl,
	struct rtw_phl_ecsa_param **param
	)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_ecsa_ctrl_t *ecsa_ctrl =
		(struct phl_ecsa_ctrl_t *)phl_info->ecsa_ctrl;

	if(ecsa_ctrl == NULL)
		goto exit;

	*param = &ecsa_ctrl->ecsa_param;
exit:
	return status;
}

enum rtw_phl_status
phl_ecsa_ctrl_init(
	struct phl_info_t *phl_info
	)
{
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_ecsa_ctrl_t *ecsa_ctrl = NULL;

	ecsa_ctrl = _os_mem_alloc(drv_priv, sizeof(struct phl_ecsa_ctrl_t));
	if (ecsa_ctrl == NULL) {
		phl_info->ecsa_ctrl = NULL;
		return RTW_PHL_STATUS_FAILURE;
	}

	phl_info->ecsa_ctrl = ecsa_ctrl;

	/* set default value */
	ecsa_ctrl->state = ECSA_STATE_NONE;
	ecsa_ctrl->phl_com = phl_info->phl_com;
	ecsa_ctrl->role = NULL;
	ecsa_ctrl->rlink = NULL;
	ecsa_ctrl->expected_tbtt_ms = 0;

	_os_init_timer(drv_priv, &ecsa_ctrl->timer, _phl_ecsa_timer_callback,
		       ecsa_ctrl, "phl_ecsa_timer");

	_os_spinlock_init(drv_priv, &(ecsa_ctrl->lock));

	return RTW_PHL_STATUS_SUCCESS;
}

void
phl_ecsa_ctrl_deinit(
	struct phl_info_t *phl_info
	)
{
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct phl_ecsa_ctrl_t *ecsa_ctrl =
	    (struct phl_ecsa_ctrl_t *)(phl_info->ecsa_ctrl);

	if (ecsa_ctrl == NULL)
		return;
	_os_spinlock_free(drv_priv, &(ecsa_ctrl->lock));
	_os_release_timer(drv_priv, &ecsa_ctrl->timer);
	_os_mem_free(drv_priv, ecsa_ctrl, sizeof(struct phl_ecsa_ctrl_t));

	phl_info->ecsa_ctrl = NULL;
}

enum rtw_phl_status
rtw_phl_ecsa_init_ops(
	void *phl,
	struct rtw_phl_ecsa_ops *ops)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_ecsa_ctrl_t *ecsa_ctrl =
		(struct phl_ecsa_ctrl_t *)phl_info->ecsa_ctrl;

	if(ecsa_ctrl == NULL)
		goto exit;

	ecsa_ctrl->ops.priv = ops->priv;
	ecsa_ctrl->ops.update_beacon = ops->update_beacon;
	ecsa_ctrl->ops.update_chan_info = ops->update_chan_info;
	ecsa_ctrl->ops.check_ecsa_allow = ops->check_ecsa_allow;
	ecsa_ctrl->ops.ecsa_complete = ops->ecsa_complete;
	ecsa_ctrl->ops.check_tx_resume_allow = ops->check_tx_resume_allow;
	status = RTW_PHL_STATUS_SUCCESS;
exit:
	return status;
}

#ifdef CONFIG_PHL_ECSA_EXTEND_OPTION
void
rtw_phl_ecsa_extend_option_hdlr(
	u32 extend_option,
	struct rtw_wifi_role_link_t *ap_rlink,
	struct rtw_phl_ecsa_param *param
)
{
	if ((extend_option & ECSA_EX_OPTION_FORCE_BW20) &&
		(param->new_chan_def.bw != CHANNEL_WIDTH_20)) {
		/* force 20M mode, set attributes accordingly */
		param->new_chan_def.bw = CHANNEL_WIDTH_20;
		param->new_chan_def.center_ch = param->new_chan_def.chan;
		param->new_chan_def.offset = CHAN_OFFSET_NO_EXT;
		param->op_class = rtw_phl_get_operating_class(param->new_chan_def);
	}

	/* bandwidth and offset of new_chan_def is same as AP mode */
	if (extend_option & ECSA_EX_OPTION_USE_AP_CHANDEF) {
		param->new_chan_def.bw = ap_rlink->chandef.bw;
		param->new_chan_def.offset = ap_rlink->chandef.offset;
		param->op_class = rtw_phl_get_operating_class(param->new_chan_def);
	}
}
#endif

bool
rtw_phl_ecsa_check_allow(
	void *phl,
	struct rtw_wifi_role_t *role,
	struct rtw_chan_def chan_def,
	enum phl_ecsa_start_reason reason,
#ifdef CONFIG_PHL_ECSA_EXTEND_OPTION
	u32 *extend_option,
#endif
	u32 *delay_start_ms
)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_ecsa_ctrl_t *ecsa_ctrl =
		(struct phl_ecsa_ctrl_t *)phl_info->ecsa_ctrl;
	struct rtw_phl_ecsa_ops *ops = &(ecsa_ctrl->ops);
	bool ecsa_allow = false;

	if(ops->check_ecsa_allow)
		ecsa_allow = ops->check_ecsa_allow(ops->priv,
						   role,
						   chan_def,
						   reason,
#ifdef CONFIG_PHL_ECSA_EXTEND_OPTION
						   extend_option,
#endif
						   delay_start_ms);
	return ecsa_allow;
}
#endif /* CONFIG_PHL_ECSA */