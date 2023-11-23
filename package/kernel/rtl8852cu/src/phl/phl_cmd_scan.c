/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
 * Author: vincent_fann@realtek.com
 *
 *****************************************************************************/
#define _PHL_CMD_SCAN_C_
#include "phl_headers.h"
#include "phl_scan.h"
#include "phl_scanofld.h"

#define param_to_phlcom(_param)        (_param->wifi_role->phl_com)
#define wrole_to_phl(_wrole)           ((struct phl_info_t *)(_wrole->phl_com->phl_priv))
#define sctrl_to_rlink(_sctrl, _idx)   (&(_sctrl->wrole->rlink[_idx]))

#ifdef CONFIG_PHL_CMD_SCAN

#define DEF_PROBE_PERIOD 20

static void _cmd_scan_req_deinit(struct phl_info_t *phl_info,
                                 struct rtw_phl_scan_param *param);
static void _cmd_scan_timer(void *context);

#define DBG_SCAN_CHAN_DUMP


u8 phl_cmd_scan_ctrl(struct rtw_phl_scan_param *param, u8 band_idx,
		  struct cmd_scan_ctrl **sctrl)
{
	struct cmd_scan_ctrl *_sctrl = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 sctrl_idx = 0;

	for(sctrl_idx = 0; sctrl_idx < param->sctrl_num; sctrl_idx++) {
		_sctrl = &param->sctrl[sctrl_idx];
		rlink = sctrl_to_rlink(_sctrl, RTW_RLINK_PRIMARY);

		if(rlink->hw_band == band_idx)
			break;
	}

	if(sctrl_idx == param->sctrl_num) {
		PHL_ASSERT("%s: sctrl doesn't support this band_idx(%d)\n", __func__, band_idx);
		_sctrl = NULL;
	}

	*sctrl = _sctrl;
	return sctrl_idx;
}

#ifdef DBG_SCAN_CHAN_DUMP
static void
_cmd_estimated_swch_seq(void *drv, struct rtw_phl_scan_param *param, u8 op_num, u8 sctrl_idx)
{
	struct cmd_scan_ctrl *sctrl = &param->sctrl[sctrl_idx];
	u8 chidx = 0;
	u8 opidx = 0;
	u8 total_ch_num = 0;
	struct phl_scan_channel *ch;
	_os_list *node = NULL;

	if ((param->back_op.mode == SCAN_BKOP_CNT) &&
		(param->back_op.ch_intv == 0)) {
		PHL_ERR("%s bkop_cnt == 0\n", __func__);
		_os_warn_on(1);
		return;
	}

	if (pq_get_front(drv, &sctrl->chlist, &node, _ps) == false)
		return;

	/*swicth channel sequence by cmd_scan's estimated */
	PHL_INFO("%s:: Estimated channel sequence:\n", __func__);

	if (param->back_op.mode == SCAN_BKOP_CNT) {
		PHL_INFO("[SCAN_BKOP_CNT]:: sctrl[%d]\n", sctrl_idx);

		do {
			ch = (struct phl_scan_channel*)node;
			PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, "%3d, ", ch->channel);
			total_ch_num++;
			if(!((chidx + 1) % param->back_op.ch_intv)) {
				if (op_num) {
					for(opidx = 0; opidx < op_num; opidx++) {
						PHL_DATA(COMP_PHL_DBG, _PHL_INFO_,
							"[%3d], ", sctrl->back_op_ch[opidx].channel);
						total_ch_num++;
					}
				}
			}
			if(!((chidx + 1) % (param->back_op.ch_intv * 2)))
				PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, "\n");
			chidx++;
		} while(pq_get_next(drv, &sctrl->chlist, node, &node, _ps));
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, "\n");

	}
	#ifdef CONFIG_PHL_CMD_SCAN_BKOP_TIME
	else if (param->back_op.mode == SCAN_BKOP_TIMER) {
		u16 ch_dur = 0;

		PHL_INFO("[SCAN_BKOP_TIMER]:: sctrl_idx[%d]\n", sctrl_idx);
		do {
			ch = (struct phl_scan_channel*)node;
			PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, "%3d, ", ch->channel);
			total_ch_num++;
			ch_dur = (op_num) ? param->back_op.off_ch_dur_ms : ch->duration;
			PHL_INFO("\t%3d, dur:%d(ms)\n", ch->channel, ch_dur);
			chidx++;
		} while(pq_get_next(drv, &sctrl->chlist, node, &node, _ps));
		if (op_num) {
			for(opidx = 0; opidx < op_num; opidx++) {
				total_ch_num++;
				PHL_INFO("\t[%3d], dur:%d(ms)\n",
					sctrl->back_op_ch[opidx].channel, sctrl->back_op_ch[opidx].duration);
			}
		}
		PHL_INFO("max_listen_time:%d (ms)\n", param->max_listen_time);
		PHL_INFO("op_ch_dur_ms:%d, off_ch_dur_ms:%d, off_ch_ext_dur_ms:%d (ms)",
			param->back_op.ch_dur_ms,
			param->back_op.off_ch_dur_ms,
			param->back_op.off_ch_ext_dur_ms);
	}
	#endif
	else if (param->back_op.mode == SCAN_BKOP_NONE) {
		PHL_INFO("[SCAN_BKOP_NONE]:: sctrl_idx[%d]\n", sctrl_idx);
		do {
			ch = (struct phl_scan_channel*)node;
			PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, "%3d, ", ch->channel);
			total_ch_num++;
			if(!((chidx + 1) % 6))
				PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, "\n");
			chidx++;
		} while(pq_get_next(drv, &sctrl->chlist, node, &node, _ps));
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, "\n");
	}

	PHL_INFO("Scan chan num:%d , Total num:%d, repeat:%d\n",
		param->ch_num, total_ch_num, param->repeat);
	PHL_INFO("--------\n");
}
#endif /*DBG_SCAN_CHAN_DUMP*/

#ifdef CONFIG_RTW_ACS
static void
_cmd_scan_acs_mntr_trigger(struct phl_info_t *phl_info, enum phl_band_idx band_idx,
                           struct rtw_phl_scan_param *param,
			   u8 sctrl_idx)
{
	struct phl_acs_parm parm = {0};
	struct phl_scan_channel *scan_ch = param->sctrl[sctrl_idx].scan_ch;

	if (!param->acs)
		return;

	parm.idx = scan_ch->acs_idx;
	parm.monitor_time = ACS_ENV_MNTR_TIME(scan_ch->duration);
	parm.nhm_include_cca = param->nhm_include_cca;

	phl_acs_mntr_trigger(phl_info, band_idx, &parm);
}

static void
_cmd_scan_acs_mntr_result(struct phl_info_t *phl_info, enum phl_band_idx band_idx,
			struct rtw_phl_scan_param *param,
			  u8 sctrl_idx)
{
	struct phl_acs_parm parm = {0};
	struct phl_scan_channel *scan_ch = param->sctrl[sctrl_idx].scan_ch;

	if (!param->acs || param->sctrl[sctrl_idx].ch_idx < 0)
		return;

	parm.idx = scan_ch->acs_idx;

	phl_acs_mntr_result(phl_info, band_idx, &parm);
}
#endif /* CONFIG_RTW_ACS */

static void
_cmd_scan_update_chlist(void *drv, struct rtw_phl_scan_param *param,
			u8 sctrl_idx, bool is_cckphy)
{
	u8 idx = 0;
	enum band_type sel_band = BAND_MAX;
	struct cmd_scan_ctrl *sctrl = NULL;

	INIT_LIST_HEAD(&param->sctrl[sctrl_idx].chlist.queue);

	if(param->sctrl_num > 1) {

		sctrl = &param->sctrl[sctrl_idx];

		if(is_cckphy)
			sel_band = BAND_ON_24G;
		else
			sel_band = BAND_ON_5G;

		sctrl->is_cckphy = is_cckphy;

		PHL_INFO("%s[%d]:: is_cckphy:%d, sel_band:%d\n",
			__func__, sctrl_idx, is_cckphy, sel_band);


		/* 1,2,3,[36],4,5,6,[36],7,8,9,[36],10,11*/
		/* 36,[36],40,44,48,[36],52,56,60,[36]...*/
		/* --->  1,2,3,4,5,6,7,8,9,10,11*/
		/* --->  [36],[36],[36],36,[36],40,44,48,[36]*/
		for(idx = 0; idx < param->ch_num; idx++) {
			if(param->ch[idx].band == sel_band) {
				INIT_LIST_HEAD(&param->ch[idx].list);
				pq_push(drv, &sctrl->chlist, &param->ch[idx].list, _tail, _ps);
			}
		}
	}
	else {
		param->sctrl[0].is_cckphy = is_cckphy;
		for(idx = 0; idx < param->ch_num; idx++) {
			INIT_LIST_HEAD(&param->ch[idx].list);
			pq_push(drv, &param->sctrl[0].chlist, &param->ch[idx].list, _tail, _ps);
		}
	}
}

/*
 * Insert op channel list
 * Ex.	ch_intvl =3
 * ch_idx : -1, 0, 1, 2,   3, 4, 5, 6
 *					   ^		  ^
 *					   op0~op5	  op0~op5
 *
 * => 0,1,2,[op0],[op1],3,4,5,[op0],[op1],6,7,8,[op0],[op1]
 */

static inline void
_cmd_scan_enqueue_opch(void *drv, struct cmd_scan_ctrl *sctrl)
{
	u8 idx = 0;

	for(idx = 0; idx < MAX_WIFI_ROLE_NUMBER; idx ++) {
		if(sctrl->back_op_ch[idx].channel)
			pq_push(drv, &sctrl->chlist, &sctrl->back_op_ch[idx].list, _first, _ps);
		else
			break;
	}
}
static struct phl_scan_channel *_cmd_scan_select_chnl(
	void *drv, struct rtw_phl_scan_param *param, u8 band_idx)
{
	struct phl_scan_channel *scan_ch = NULL;
	struct cmd_scan_ctrl *sctrl = NULL;
	_os_list* obj = NULL;
	bool back_op_is_required = false;
	u8 sctrl_idx = 0;

	sctrl_idx = phl_cmd_scan_ctrl(param, band_idx, &sctrl);
	if(sctrl == NULL) {
		PHL_ERR("%s: find sctrl failed\n", __func__);
		return NULL;
	}
	back_op_is_required = (sctrl->back_op_ch[0].channel)? true:false;


next_ch:
	if(pq_pop(drv, &sctrl->chlist, &obj, _first, _ps)) {
		scan_ch = (struct phl_scan_channel*)obj;

		if(scan_ch->scan_mode == NORMAL_SCAN_MODE) {
			sctrl->ch_idx++;
			/* 1- enable,  2- BK_CNT mode, 3- prev is non-op, 4- ch_intvl's turn */
			if (back_op_is_required && param->back_op.mode == SCAN_BKOP_CNT) {
				if(!((sctrl->ch_idx + 1) % param->back_op.ch_intv)) {
					_cmd_scan_enqueue_opch(drv, sctrl);
				}
			} else if (back_op_is_required && param->back_op.mode == SCAN_BKOP_TIMER) {

				if ((phl_get_passing_time_ms(param->start_time) -
				     sctrl->last_opch_add_time) >
					param->back_op.off_ch_dur_ms) {
					sctrl->last_opch_add_time =
					        phl_get_passing_time_ms(param->start_time);
					_cmd_scan_enqueue_opch(drv, sctrl);
				}
			}
		}
		#ifdef CONFIG_PHL_CMD_SCAN_BKOP_TIME
		else if (scan_ch->scan_mode == P2P_LISTEN_MODE) {
			if (back_op_is_required && param->back_op.mode == SCAN_BKOP_TIMER) {
				scan_ch->duration = param->back_op.off_ch_dur_ms;
				_cmd_scan_enqueue_opch(drv, sctrl);
			}
			sctrl->ch_idx++;
		}
		#endif
		sctrl->scan_ch = scan_ch;
	}
	else if(sctrl->repeat > 0) {
		_cmd_scan_update_chlist(drv, param, band_idx,
		                        sctrl->is_cckphy);
		sctrl->ch_idx = 0;
		/* 255 means loop forever */
		if (sctrl->repeat != 255)
			sctrl->repeat--;
		goto next_ch;
	}
	else {
		return NULL;
	}

	PHL_INFO("%s:[%d] repeat[%d] ch_idx=[%d], remain=[%d], ch_number=%d, scan_mode= %s\n", __func__,
		 band_idx, sctrl->repeat, sctrl->ch_idx, sctrl->chlist.cnt, sctrl->scan_ch->channel,
		 (sctrl->scan_ch->scan_mode == BACKOP_MODE)? "OP_CH": "Non-OP");

	return sctrl->scan_ch;
}

/* Notification complete */
void _cmd_scan_timer_notify_cb(void *priv, struct phl_msg *msg)
{
	struct cmd_scan_ctrl *sctrl = (struct cmd_scan_ctrl *)priv;

	if (IS_MSG_CANNOT_IO(msg->msg_id)) {
		PHL_ERR("%s: LISTEN_STATE_EXPIRE failure by CANNOT IO\n", __func__);
		return;
	}

	if(IS_MSG_CLR_SNDR(msg->msg_id)) {
		/* Last event occured MSG_STATUS_PENDING */
		PHL_ERR("%s: LISTEN_STATE_EXPIRE pending Occurred!\n", __func__);
		_cmd_scan_timer((void *)sctrl);
	}
	else if (IS_MSG_CANCEL(msg->msg_id)) {
		PHL_ERR("%s: LISTEN_STATE_EXPIRE abort Occurred!\n", __func__);
		return;
	}
}

static void _cmd_scan_timer(void *context)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct cmd_scan_ctrl *sctrl = (struct cmd_scan_ctrl *)context;
	struct phl_info_t *phl_info = wrole_to_phl(sctrl->wrole);
	struct rtw_wifi_role_link_t *rlink = sctrl_to_rlink(sctrl, RTW_RLINK_PRIMARY);
	u8 band_idx = rlink->hw_band;

	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	PHL_INFO("%s: band_idx=%d\n", __func__, band_idx);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_SCAN);
/** When listen state of each channel entry expired,
 * timer callback send MSG_EVT_LISTEN_STATE_EXPIRE for additional condition check
 * If nothing specitail occured, then send MSG_EVT_SWCH_START to proceed for the rest of channel list
 * therefore, additional process delay for MSG_EVT_LISTEN_STATE_EXPIRE would prolong listen period
 * */
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_LISTEN_STATE_EXPIRE);
	msg.band_idx = band_idx;

	attr.completion.completion = _cmd_scan_timer_notify_cb;
	attr.completion.priv = sctrl;

	phl_status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if(phl_status != RTW_PHL_STATUS_SUCCESS)
		PHL_ERR("%s: [SCAN_TIMER] phl_disp_eng_send_msg failed(%X) !\n", __func__, phl_status);
}

/* Notification complete */
void _cmd_swch_done_notify_cb(
	void *drv, struct phl_msg *msg)
{
	if (msg->inbuf) {
		_os_mem_free(drv, msg->inbuf, msg->inlen);
	}
}

enum rtw_phl_status _cmd_swch_done_notify(
	void *dispr, void *drv, struct rtw_phl_scan_param *param, u8 sctrl_idx)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct cmd_scan_ctrl *sctrl = &param->sctrl[sctrl_idx];
	struct phl_info_t *phl_info = wrole_to_phl(sctrl->wrole);
	struct phl_scan_channel *scan_ch = sctrl->scan_ch;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	u8 *info = NULL;
	u8 band_idx = 0;

	info = _os_mem_alloc(drv, sizeof(struct phl_scan_channel));
	if (info == NULL) {
		PHL_ERR("%s: [SWCH_DONE] alloc buffer failed!\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	_os_mem_cpy(drv, info, scan_ch, sizeof(*scan_ch));

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_SCAN);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_SWCH_DONE);

	attr.opt = MSG_OPT_CLR_SNDR_MSG_IF_PENDING;
	attr.completion.completion = _cmd_swch_done_notify_cb;
	attr.completion.priv = drv;

	msg.inbuf = info;
	msg.inlen = sizeof(*scan_ch);
	msg.rsvd[0].ptr = (u8*)sctrl->wrole;
	msg.rsvd[1].ptr = (u8*)&sctrl->wrole->rlink[RTW_RLINK_PRIMARY];
	phl_dispr_get_idx(dispr, &band_idx);
	msg.band_idx = band_idx;
	pstatus = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if(pstatus != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: [SWCH_DONE] phl_disp_eng_send_msg failed!\n", __func__);
		_os_mem_free(drv, info, sizeof(struct phl_scan_channel));
	}
	return pstatus;
}

void _cmd_scan_end(
	void *drv, struct rtw_phl_scan_param *param,
	u8 band_idx)
{
	struct rtw_phl_com_t *phl_com = param_to_phlcom(param);
	struct phl_info_t *phl_info = phl_com->phl_priv;
	struct cmd_scan_ctrl *sctrl = NULL;
	bool indicate = true;
	u8 sctrl_idx = 0xff;

	PHL_INFO("_cmd_scan_end \n");
	#ifdef CONFIG_PHL_SCANOFLD
	if (param->mode == SCAN_MD_FW) {
		phl_cmd_scanofld_end(drv, param, band_idx);
		return;
	}
	#endif

	if((param->sctrl_num > 1) &&
	   !TEST_SCAN_FLAGS(param->state, CMD_SCAN_END)) {
	   	indicate = false;
	}
	SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_END);

	param->end_time = _os_get_cur_time_ms();

	/* dump scan time */
	param->total_scan_time =
		phl_get_passing_time_ms(param->enqueue_time);

	sctrl_idx = phl_cmd_scan_ctrl(param, band_idx, &sctrl);
	if(sctrl == NULL) {
		PHL_ERR("%s: find sctrl failed\n", __func__);
		goto error;
	}

	sctrl->scan_ch = NULL;

	/* acquire state */
	_os_cancel_timer(drv, &sctrl->scan_timer);
	_os_release_timer(drv, &sctrl->scan_timer);

	if(TEST_SCAN_FLAG(param->state, band_idx, CMD_SCAN_STARTED) &&
	   !TEST_SCAN_FLAG(param->state, band_idx, CMD_SCAN_DF_IO) )
	{
		rtw_hal_com_scan_restore_tx_lifetime(phl_info->hal, band_idx);

		rtw_hal_scan_set_rxfltr_by_mode(phl_info->hal, band_idx,
						false, &sctrl->fltr_mode);
		rtw_hal_scan_pause_tx_fifo(phl_info->hal, band_idx, false);

		rtw_hal_notification(phl_info->hal, MSG_EVT_SCAN_END, band_idx);
	}
error:
	if (indicate) {
		_cmd_scan_req_deinit(phl_info, param);
		param->sctrl_num = 0;
	}

	PHL_INFO("[cmd_scan][%d]:: sctrl_idx[%d] param->state(0x%X), result(0x%X)\n",
	         band_idx, sctrl_idx, param->state, param->result);

	if (indicate && param->ops->scan_complete)
		param->ops->scan_complete(param->priv, param);
}

/* Notification complete */
void _cmd_abort_notify_cb(
	void *drv, struct phl_msg *msg)
{
	struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param *)msg->inbuf;

	if (IS_MSG_CANNOT_IO(msg->msg_id))
		SET_SCAN_FLAG(param->state, msg->band_idx, CMD_SCAN_DF_IO);

	_cmd_scan_end(drv, param, msg->band_idx);
}

void _cmd_abort_notify(void *dispr, void *drv,
	struct rtw_phl_scan_param *param, bool abort)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_com_t *phl_com = param_to_phlcom(param);
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	struct phl_info_t *phl = (struct phl_info_t *) phl_com->phl_priv;
	struct cmd_scan_ctrl *sctrl = NULL;
	bool indicate = true;
	u8 band_idx = 0xff, sctrl_idx = 0xff;

	phl_dispr_get_idx(dispr, &band_idx);

	if(TEST_SCAN_FLAG(param->state, band_idx, CMD_SCAN_ACQUIRE))
	{
		param->result = SCAN_REQ_CANCEL;

		sctrl_idx = phl_cmd_scan_ctrl(param, band_idx, &sctrl);
		if(sctrl == NULL) {
			PHL_ERR("%s: find sctrl failed\n", __func__);
			_cmd_abort_notify_cb(drv, &msg);
			return;
		}

		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_SCAN);
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_SCAN_END);

		if(abort)
			attr.opt = MSG_OPT_SEND_IN_ABORT;
		attr.completion.completion = _cmd_abort_notify_cb;
		attr.completion.priv = drv;

		msg.inbuf = (u8*)param;    /* for _cmd_abort_notify_cb */
		msg.rsvd[0].ptr = (u8*)sctrl->wrole;
		msg.band_idx = band_idx;

		pstatus = phl_disp_eng_send_msg(phl, &msg, &attr, NULL);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			/* (1) dispr_stop
			   (2) idle msg empty .*/

			PHL_ERR("%s :: [Abort] dispr_send_msg failed (0x%X)\n",
				__func__, pstatus);

			if(pstatus == RTW_PHL_STATUS_UNEXPECTED_ERROR ||
			   TEST_STATUS_FLAG(phl_com->dev_state, RTW_DEV_SURPRISE_REMOVAL)) {
				/* clean sw resource only */
				/* (1) driver is going to unload */
				/* (2) Supprise remove */
				SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_DF_IO);
			}
			_cmd_abort_notify_cb(drv, &msg);
		}
	}
	else {
		if((param->sctrl_num > 1) &&
		   !TEST_SCAN_FLAGS(param->state, CMD_SCAN_END)) {
			indicate = false;
		}

		SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_END);

		param->result = SCAN_REQ_ABORT;

		if(indicate) {
			_cmd_scan_req_deinit(phl, param);
			param->sctrl_num = 0;
		}

		if (indicate && param->ops->scan_complete)
			param->ops->scan_complete(param->priv, param);
	}
}

enum phl_mdl_ret_code _cmd_scan_fail_ev_hdlr(
	void* dispr, void* priv, struct phl_msg* msg)
{
	struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param*)priv;
	struct rtw_phl_com_t *phl_com = param_to_phlcom(param);
	struct phl_info_t *phl_info = phl_com->phl_priv;
	void *d = phlcom_to_drvpriv(phl_com);
	u8 band_idx = 0xff, sctrl_idx = 0xff;
	struct phl_msg nextmsg = {0};
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct cmd_scan_ctrl *sctrl = NULL;

	SET_MSG_MDL_ID_FIELD(nextmsg.msg_id, PHL_FG_MDL_SCAN);
	phl_dispr_get_idx(dispr, &band_idx);
	sctrl_idx = phl_cmd_scan_ctrl(param, band_idx, &sctrl);
	if(sctrl == NULL) {
		PHL_ERR("%s: find sctrl failed\n", __func__);
		return MDL_RET_FAIL;
	}

	nextmsg.band_idx = band_idx;
	switch(MSG_EVT_ID_FIELD(msg->msg_id)) {
		case MSG_EVT_SCAN_START:
			/* fall through */
		case MSG_EVT_LISTEN_STATE_EXPIRE:
			/* fall through */
		case MSG_EVT_SWCH_START:
			/* fall through */
		case MSG_EVT_SWCH_DONE:
			PHL_INFO("[%d]SCAN_START/SWCH_START/SWCH_DONE:: failed/timeout handler \n", band_idx);

			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SCAN_END);
			nextmsg.rsvd[0].ptr = (u8*)sctrl->wrole;

			pstatus = phl_disp_eng_send_msg(phl_info, &nextmsg, &attr, NULL);
			if(pstatus != RTW_PHL_STATUS_SUCCESS)
				PHL_ERR("%s :: [SWCH_START] phl_disp_eng_send_msg failed\n", __func__);
		break;

		case MSG_EVT_SCAN_END:
			// free token
			// release timer
			PHL_INFO("[%d]MSG_EVT_SCAN_END:: failed/timeout handler \n", band_idx);
			pstatus = phl_disp_eng_free_token(phl_info, band_idx, &sctrl->token);
			if (pstatus == RTW_PHL_STATUS_SUCCESS) {
				if (IS_MSG_CANNOT_IO(msg->msg_id))
					SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_DF_IO);
				_cmd_scan_end(d, param, band_idx);
			}
		break;

		default:
			PHL_INFO("%s, unknown state : %d \n", __func__,
				MSG_EVT_ID_FIELD(msg->msg_id));
			/* unknown state */
		break;
	}

	return MDL_RET_SUCCESS;
}

enum phl_mdl_ret_code _cmd_scan_hdl_external_evt(
	void* dispr, void* priv, struct phl_msg* msg)
{
	PHL_DBG("%s :: From others MDL =%d , EVT_ID=%d\n", __func__,
		 MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));
	return MDL_RET_IGNORE;
}

u8 phl_cmd_chk_ext_act_scan(struct rtw_phl_scan_param *param, u8 sctrl_idx)
{
	struct cmd_scan_ctrl *sctrl = &param->sctrl[sctrl_idx];
	/** suppose to query the time of last recieved beacon in current channel here
	 * then change state to EXT_ACT_SCAN_TRIGGER if needed
	 * but, PHL does not store bss list at the moment, therefore,
	 * core layer use set_info (FG_REQ_OP_NOTIFY_BCN_RCV) to notify scan module incoming bcn
	 * and change state to EXT_ACT_SCAN_TRIGGER accordingly.
	*/
	if (sctrl->scan_ch->type == RTW_PHL_SCAN_PASSIVE &&
	    sctrl->scan_ch->ext_act_scan == EXT_ACT_SCAN_TRIGGER) {
		if (param->ops->scan_issue_pbreq)
			param->ops->scan_issue_pbreq(param->priv, param, sctrl_idx);
		sctrl->scan_ch->ext_act_scan = EXT_ACT_SCAN_DONE;
		return true;
	}
	return false;
}

/*
 * Return value :
 * 	false : cancel cur channel.
 *	true : keep scan on cur channel.
 */
static bool _handle_probing(void *d, struct rtw_phl_scan_param *param, u8 sctrl_idx)
{
	struct cmd_scan_ctrl *sctrl = &param->sctrl[sctrl_idx];
	struct phl_scan_channel *scan_ch = sctrl->scan_ch;
	u32 dur = 0, diff_t = 0, probe_t = 0;
	bool check_cancel = true;

	scan_ch->pass_t = phl_get_passing_time_ms(scan_ch->start_t);
	if (scan_ch->pass_t >= scan_ch->last_t)
		diff_t = scan_ch->pass_t - scan_ch->last_t;
	PHL_INFO("[probing], band %d, ch %d, pass_t %d, last_t %d, diff_t %d, duration %d\n",
			scan_ch->band, scan_ch->channel, scan_ch->pass_t,
			scan_ch->last_t, diff_t, scan_ch->duration);

	/* DFS, tx probe and extend scan period */
	if (phl_cmd_chk_ext_act_scan(param, sctrl_idx)) {
		scan_ch->duration += (u16)param->ext_act_scan_period;
		check_cancel = false;
		PHL_INFO("[probing], DFS extend period to %d !\n",
				scan_ch->duration);
	}

	dur = scan_ch->duration;
	if (scan_ch->pass_t >= dur) {
		PHL_INFO("[probing], expired, channel end !!\n");
		return false;
	}

	scan_ch->remain_t = (u32)(dur - scan_ch->pass_t);
	PHL_INFO("[probing], remain_t %d \n", scan_ch->remain_t);

	if (diff_t >= DEF_PROBE_PERIOD) {
		scan_ch->last_t = scan_ch->pass_t;

		if (param->ops->scan_ch_cancel && check_cancel) {
			if (param->ops->scan_ch_cancel(param->priv, param, sctrl_idx)) {
				PHL_INFO("[probing], channel canceled !\n");
				return false;
			}
		}

		/* core probe handler */
		param->ops->scan_probe(param->priv, param, sctrl_idx);
	}

	if (scan_ch->remain_t <= param->probe_t)
		probe_t = scan_ch->remain_t;
	else
		probe_t = param->probe_t;

	_os_set_timer(d, &sctrl->scan_timer, probe_t);

	return true;
}

void
_cmd_scan_start(struct phl_info_t *phl_info,
		struct cmd_scan_ctrl *sctrl,
		u8 band_idx)
{
	rtw_hal_scan_pause_tx_fifo(phl_info->hal, band_idx, true);
	rtw_hal_scan_set_rxfltr_by_mode(phl_info->hal, band_idx,
					true, &sctrl->fltr_mode);
	rtw_hal_com_scan_set_tx_lifetime(phl_info->hal, band_idx);
	rtw_hal_notification(phl_info->hal, MSG_EVT_SCAN_START, band_idx);
}

enum phl_mdl_ret_code _cmd_scan_hdl_internal_evt(
	void* dispr, void* priv, struct phl_msg* msg)
{
	struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param*)priv;
	struct rtw_phl_com_t *phl_com = param_to_phlcom(param);
	struct phl_info_t *phl_info = phl_com->phl_priv;
	void *d = phlcom_to_drvpriv(phl_com);
	u32 diff_time = 0, probe_t = 0;
	struct cmd_scan_ctrl *sctrl = NULL;
	struct phl_msg nextmsg = {0};
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	u8 band_idx = 0xff, sctrl_idx = 0xff;
	struct phl_scan_channel *scan_ch = NULL;
	bool tx_pause = true;
	struct rtw_chan_def chdef = {0};

	diff_time = phl_get_passing_time_ms(param->enqueue_time);

	if (param->max_scan_time && diff_time >= param->max_scan_time) {
		PHL_WARN("%s:: Timeout! %d > max_time %d\n",
				 __func__, diff_time, param->max_scan_time);

		/* Abort scan request */
		/* Based on [CN3AXSW-552]
		 * ex. max_scan_time = 4sec
		 * Usb dongle would abort scan_req in 29~33th chnl
		 * If chnllist insert op-chnl, scan_req would be aborted in 21~23th chnl.
		 * It means that usb dongle always can't do fully scan.
		 * So, abort scan_req or not, depend on core layer.
		*/
		_cmd_scan_fail_ev_hdlr(dispr, priv, msg);
		return MDL_RET_FAIL;
	}
	else {
		PHL_INFO("\t[cmd_scan]:: TimeIntvl: %u \n", diff_time);
	}

	#ifdef CONFIG_PHL_CMD_SCAN_BKOP_TIME
	if (param->max_listen_time && diff_time >= param->max_listen_time) {
		PHL_WARN("%s:: Timeout! %d > max_listen_time %d\n",
				 __func__, diff_time, param->max_listen_time);
		#if 0
		_cmd_scan_fail_ev_hdlr(dispr, priv, msg);
		return MDL_RET_FAIL;
		#endif
	}
	#endif

	phl_dispr_get_idx(dispr, &band_idx);
	sctrl_idx = phl_cmd_scan_ctrl(param, band_idx, &sctrl);
	if(sctrl == NULL) {
		PHL_ERR("%s: find sctrl failed\n", __func__);
		return MDL_RET_FAIL;
	}

	SET_MSG_MDL_ID_FIELD(nextmsg.msg_id, PHL_FG_MDL_SCAN);
	nextmsg.band_idx = band_idx;

	switch(MSG_EVT_ID_FIELD(msg->msg_id)) {
		case MSG_EVT_SCAN_START:
			_cmd_scan_start(phl_info, sctrl, band_idx);

			PHL_INFO("[%d]MSG_EVT_SCAN_START \n", band_idx);
			/* [scan start notify] */
			if (!TEST_SCAN_FLAGS(param->state, CMD_SCAN_STARTED)) {
				if (param->ops->scan_start)
					param->ops->scan_start(param->priv, param);
			}
			SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_STARTED);

			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SWCH_START);
			nextmsg.rsvd[0].ptr = (u8*)sctrl->wrole;
			pstatus = phl_disp_eng_send_msg(phl_info, &nextmsg, &attr, NULL);
			if(pstatus != RTW_PHL_STATUS_SUCCESS)
				PHL_ERR("%s :: [SCAN_START] phl_disp_eng_send_msg failed\n", __func__);
		break;
		case MSG_EVT_LISTEN_STATE_EXPIRE:
			if (!param->ops->scan_probe) {
				PHL_INFO("[%d]MSG_EVT_LISTEN_STATE_EXPIRE \n", band_idx);
				if (phl_cmd_chk_ext_act_scan(param, sctrl_idx)) {
					_os_set_timer(d, &sctrl->scan_timer,
					              param->ext_act_scan_period);
					PHL_INFO("%s :: extend listen state of ch %d by %d ms, and reset timer\n",
						__func__, sctrl->scan_ch->channel, param->ext_act_scan_period);
					break;
				}
			} else {
				if (_handle_probing(d, param, sctrl_idx))
					break;
			}

			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SWCH_START);
			nextmsg.rsvd[0].ptr = (u8*)sctrl->wrole;

			pstatus = phl_disp_eng_send_msg(phl_info, &nextmsg, &attr, NULL);
			if(pstatus != RTW_PHL_STATUS_SUCCESS)
				PHL_ERR("%s :: [LISTEN_STATE_EXPIRE] dispr_send_msg failed\n", __func__);

		break;

		case MSG_EVT_SWCH_START:
			/*	ycx++
				ycx > length(yclist) ? SCAN_EV_END : switch channel */

			PHL_INFO("[%d]MSG_EVT_SWCH_START \n", band_idx);

			/* For the first time, param->scan_ch would be NULL */
			/* Current channel scan_mode */
			if (sctrl->scan_ch && sctrl->scan_ch->scan_mode == BACKOP_MODE) {
				tx_pause = false;
			}

			#ifdef CONFIG_RTW_ACS
			_cmd_scan_acs_mntr_result(phl_info, band_idx, param, sctrl_idx);
			#endif

			scan_ch = _cmd_scan_select_chnl(d, param, band_idx);
			if (scan_ch == NULL) {
				/* no more channel, we are done */
				SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SCAN_END);
				nextmsg.rsvd[0].ptr = (u8*)sctrl->wrole;
				pstatus = phl_disp_eng_send_msg(phl_info, &nextmsg, &attr, NULL);
				if(pstatus != RTW_PHL_STATUS_SUCCESS)
					PHL_ERR("%s :: [SWCH_START] dispr_send_msg failed\n", __func__);
				break;
			}

			/* Next channel scan_mode */
			if ((scan_ch->scan_mode != BACKOP_MODE) && !tx_pause) {
				/* Tx pause */
				rtw_hal_scan_pause_tx_fifo(phl_info->hal, band_idx, true);
				tx_pause = true;
			}
			chdef.band = scan_ch->band;
			chdef.chan = (u8)scan_ch->channel;
			chdef.bw = scan_ch->bw;
			chdef.offset = scan_ch->offset;

			phl_set_ch_bw(phl_info, band_idx, &chdef, false);

			#ifdef CONFIG_RTW_ACS
			_cmd_scan_acs_mntr_trigger(phl_info, band_idx, param, sctrl_idx);
			#endif

			if (!BAND_6GHZ(scan_ch->band) &&
			    (scan_ch->scan_mode != BACKOP_MODE) &&
			    (scan_ch->type == RTW_PHL_SCAN_ACTIVE)) {
				/* Notify RF to do tssi backup */
				rtw_hal_notification(phl_info->hal, MSG_EVT_SWCH_START, band_idx);
				if (param->ops->scan_issue_pbreq)
					param->ops->scan_issue_pbreq(param->priv, param, sctrl_idx);
			}

			if (param->ops->scan_probe) {
				scan_ch->start_t = _os_get_cur_time_ms();
				scan_ch->last_t = 0;
				scan_ch->pass_t = 0;
				scan_ch->remain_t = scan_ch->duration;
				if (scan_ch->remain_t >= param->probe_t)
					probe_t = param->probe_t;
				else
					probe_t = scan_ch->remain_t;
			} else {
				probe_t = scan_ch->duration;
			}

			_os_set_timer(d, &sctrl->scan_timer, probe_t);

			if ((scan_ch->scan_mode == BACKOP_MODE) && tx_pause) {
				/* Tx un-pause */
				rtw_hal_scan_pause_tx_fifo(phl_info->hal, band_idx, false);
			}
			#ifdef DBG_SCAN_CHAN_DUMP
			PHL_INFO("[SCAN] band:%d chan:%d bw:%d offset:%d duration:%d (ms)\n",
				scan_ch->band, scan_ch->channel, scan_ch->bw,
					scan_ch->offset, scan_ch->duration);
			#endif

			pstatus = _cmd_swch_done_notify(dispr, d, param, sctrl_idx);
		break;

		case MSG_EVT_SWCH_DONE:
			if (param->ops->scan_ch_ready)
				param->ops->scan_ch_ready(param->priv, param, sctrl_idx);

			PHL_INFO("\tMSG_EVT_SWCH_DONE :: duration=%d\n", sctrl->scan_ch->duration);
		break;

		case MSG_EVT_SCAN_END:
			PHL_INFO("[%d]MSG_EVT_SCAN_END \n", band_idx);
			pstatus = phl_disp_eng_free_token(phl_info, band_idx, &sctrl->token);
			if(pstatus == RTW_PHL_STATUS_SUCCESS) {

				param->result = SCAN_REQ_COMPLETE;
				_cmd_scan_end(d, param, band_idx);
			}
			else
				PHL_WARN("%s :: [SCAN_END] Abort occurred, skip!\n", __func__);

		break;

		default:
			/* unknown state */
		break;
	}

	return MDL_RET_SUCCESS;
}


enum phl_mdl_ret_code _phl_cmd_scan_req_acquired(
	void* dispr, void* priv)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param*)priv;
	struct rtw_phl_com_t *phl_com = param_to_phlcom(param);
	void *d = phlcom_to_drvpriv(phl_com);
	u32 diff_time = 0;
	struct phl_info_t *phl_info = phl_com->phl_priv;
	struct cmd_scan_ctrl *sctrl = NULL;
	u8 band_idx = 0xff, sctrl_idx = 0xff;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	FUNCIN();

	if(!param->start_time)
		param->start_time = _os_get_cur_time_ms();

	/* check max scan time */
	if (param->max_scan_time > 0) {
		diff_time = phl_get_passing_time_ms(param->enqueue_time);

		if (diff_time >= param->max_scan_time) {
			PHL_WARN("%s:: Timeout! %u > max_time %d\n",
				 __func__, diff_time, param->max_scan_time);
			goto error;
		}
	}

	phl_dispr_get_idx(dispr, &band_idx);
	sctrl_idx = phl_cmd_scan_ctrl(param, band_idx, &sctrl);
	if(sctrl == NULL) {
		PHL_ERR("%s: find sctrl failed\n", __func__);
		goto error;
	}

	if (param->probe_t == 0)
		param->probe_t = DEF_PROBE_PERIOD;

	_os_init_timer(d, &sctrl->scan_timer, _cmd_scan_timer,
	               sctrl, "phl_cmd_scan_req_timer");

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_SCAN);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_SCAN_START);
	msg.rsvd[0].ptr = (u8*)sctrl->wrole;
	msg.band_idx = band_idx;
	pstatus = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);

	if(pstatus != RTW_PHL_STATUS_SUCCESS) {
		_os_release_timer(d, &sctrl->scan_timer);
		// take care another fg cmd
		goto error;
	}
	else {
		SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_ACQUIRE);
		return MDL_RET_SUCCESS;
	}

error:
	_cmd_abort_notify(dispr, d, param, false);
	return MDL_RET_FAIL;
}

enum phl_mdl_ret_code _phl_cmd_scan_req_abort(
	void* dispr, void* priv)
{
	struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param*)priv;
	struct rtw_phl_com_t *phl_com = param_to_phlcom(param);
	void *d = phlcom_to_drvpriv(phl_com);

	PHL_INFO("_phl_cmd_scan_req_abort \n");
	_cmd_abort_notify(dispr, d, param, true);
	return MDL_RET_SUCCESS;
}

enum phl_mdl_ret_code _phl_cmd_scan_req_ev_hdlr(
	void* dispr, void* priv,
	struct phl_msg* msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param*)priv;

	if(IS_MSG_FAIL(msg->msg_id)) {
		PHL_INFO("%s :: MSG(%d)_FAIL - EVT_ID=%d \n", __func__,
			 MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));

		_cmd_scan_fail_ev_hdlr(dispr, priv, msg);
		return MDL_RET_FAIL;
	}

	switch(MSG_MDL_ID_FIELD(msg->msg_id)) {
		case PHL_FG_MDL_SCAN:

			#ifdef CONFIG_PHL_SCANOFLD
			/* Scan offload */
			if (param->mode == SCAN_MD_FW) {
				ret = phl_cmd_scanofld_hdl_internal_evt(dispr,
									priv,
									(void *)msg);
			} else
			#endif
			{
				ret = _cmd_scan_hdl_internal_evt(dispr, priv, msg);
			}
			break;

		default:
			ret = _cmd_scan_hdl_external_evt(dispr, priv, msg);
			break;
	}
	return ret;
}

enum phl_mdl_ret_code _phl_cmd_scan_req_set_info(
	void* dispr, void* priv, struct phl_module_op_info* info)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	u8 band_idx = 0;

	phl_dispr_get_idx(dispr, &band_idx);

	switch(info->op_code) {
		case FG_REQ_OP_NOTIFY_BCN_RCV:
		{
			struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param*)priv;
			struct cmd_scan_ctrl *sctrl = NULL;
			struct phl_scan_channel *scan_ch = NULL;
			u16 channel = 0;
			u8 sctrl_idx = 0;

			/* this workaround might have race condition with background thread*/
			channel = *(u8*)info->inbuf;

			sctrl_idx = phl_cmd_scan_ctrl(param, band_idx, &sctrl);
			if(sctrl == NULL) {
				PHL_ERR("%s[%d]: find sctrl failed\n", __func__, band_idx);
				return ret;
			}

			scan_ch = sctrl->scan_ch;

			if (scan_ch &&
			    scan_ch->channel == channel &&
			    scan_ch->ext_act_scan == EXT_ACT_SCAN_ENABLE) {
				scan_ch->ext_act_scan = EXT_ACT_SCAN_TRIGGER;
				PHL_INFO("%s[%d] :: channel %d extend for active scan\n", __func__, band_idx, channel);
			}
			if (scan_ch &&
			    scan_ch->channel != channel)
				PHL_DBG("%s[%d] :: sctrl[%d] channel %d mismatch from listen channel %d\n",
				        __func__, band_idx, sctrl_idx, channel, scan_ch->channel);
			ret = MDL_RET_SUCCESS;
		}
			break;
		default:
			break;
	}

	/* PHL_INFO(" %s :: info->op_code=%d \n", __func__, info->op_code); */
	return ret;
}

enum phl_mdl_ret_code _phl_cmd_scan_req_query_info(
	void* dispr, void* priv, struct phl_module_op_info* info)
{
	struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param*)priv;
	struct rtw_phl_com_t *phl_com = param_to_phlcom(param);
	void *d = phlcom_to_drvpriv(phl_com);
	struct cmd_scan_ctrl *sctrl = NULL;
	u8 ucInfo = 0;
	void* pInfo = NULL;
	u8 band_idx = 0, sctrl_idx = 0;

	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	phl_dispr_get_idx(dispr, &band_idx);
	sctrl_idx = phl_cmd_scan_ctrl(param, band_idx, &sctrl);
	if(sctrl == NULL) {
		PHL_ERR("%s: find sctrl failed\n", __func__);
		return MDL_RET_FAIL;
	}

	/* PHL_INFO(" %s :: info->op_code=%d \n", __func__, info->op_code); */

	switch(info->op_code) {
		case FG_REQ_OP_GET_ROLE:
			info->outbuf = (u8*)sctrl->wrole;
			ret = MDL_RET_SUCCESS;
			break;
		case FG_REQ_OP_GET_ROLE_LINK:
			info->outbuf = (u8*)&sctrl->wrole->rlink[RTW_RLINK_PRIMARY];
			ret = MDL_RET_SUCCESS;
			break;

		case FG_REQ_OP_GET_MDL_ID:
			ucInfo= PHL_FG_MDL_SCAN;
			pInfo = (void*) &ucInfo;
			info->outlen=1;
			_os_mem_cpy(d, (void*)info->outbuf, pInfo, info->outlen);
			ret = MDL_RET_SUCCESS;
			break;
		default:
			break;
	}

	return ret;
}

static struct rtw_wifi_role_t *
_phl_cmd_scan_select_wrole(struct phl_info_t *phl_info,
                           struct rtw_phl_scan_param *param, u8 sctrl_idx)
{
	struct rtw_wifi_role_link_t *rlink = NULL;

	if(sctrl_idx == 0) {
		return param->wifi_role; /* sctrl[0]: original band_idx */
	} else {
		u8 band_idx = HW_BAND_1;

		rlink = &(param->wifi_role->rlink[RTW_RLINK_PRIMARY]);
		if(rlink->hw_band == HW_BAND_1)
			band_idx = HW_BAND_0;
		rlink = phl_mr_get_first_rlink_by_band(phl_info, band_idx);
		if (rlink != NULL)
			return rlink->wrole;
		else
			return NULL;
	}
}

#ifdef CONFIG_PHL_CMD_SCAN_BKOP_TIME
static void
_cmd_scan_update_chparam(void *drv, struct rtw_phl_scan_param *param, u8 sctrl_idx)
{
	struct cmd_scan_ctrl *sctrl = &param->sctrl[sctrl_idx];
	//u8 idx = 0;
	u16 scan_section_ms = 0;
	u16 total_scan_ms = 0;
	struct phl_scan_channel *ch;
	_os_list *node = NULL;

	/*for(idx = 0; idx < param->ch_num; idx++) {
		if (param->ch[idx].scan_mode == P2P_LISTEN_MODE) {
			param->max_listen_time = param->ch[idx].duration;
			total_scan_ms = param->ch[idx].duration;
			break;
		}
	}*/

	if (pq_get_front(drv, &sctrl->chlist, &node, _ps) == false) {
		PHL_ERR("%s get sctrl->chlist failed\n", __func__);
		return;
	}

	do {
		ch = (struct phl_scan_channel*)node;
		if (ch->scan_mode == P2P_LISTEN_MODE) {
			param->max_listen_time = ch->duration;
			total_scan_ms = ch->duration;
			break;
	}
	} while(pq_get_next(drv, &sctrl->chlist, node, &node, _ps));

	scan_section_ms = param->back_op.ch_dur_ms + param->back_op.off_ch_dur_ms;
	if (scan_section_ms)
		sctrl->repeat = total_scan_ms / scan_section_ms;
}
#endif

static u16
_assign_opch_duration(struct scan_backop_para *bkop_para, u8 band_idx,
                      struct rtw_chan_def *chdef)
{
	u16 max_dur = 0;
	u8 i = 0;
	struct scan_bkop_link_info *link_info = NULL;

	/* SCAN_BKOP_SEL_AUTO */
	if (bkop_para->sel == SCAN_BKOP_SEL_AUTO)
		return bkop_para->ch_dur_ms;

	/* SCAN_BKOP_SEL_MANUAL, get the max duration in the same channel and band_idx from the role_link list
	 * e.g. link_info_num = 2, chdef->ch = 6, band_idx = 0,
	 * (1) (a) phy 0, ch6, dur=40 (b) phy 0, ch6 dur=60, then return 60
	 * (2) (a) phy 0, ch6, dur=30 (b) phy 1, ch6 dur=80, then return 30
	 * (3) (a) phy 0, ch6, dur=50 (b) phy 0, ch11 dur=70, then return 50
	 */
	for (i = 0; i < bkop_para->link_info_num; i++) {
		link_info = &bkop_para->link_info_list[i];
		if (link_info->rlink->hw_band == band_idx) {
			if (link_info->rlink->chandef.band == chdef->band &&
			    link_info->rlink->chandef.chan == chdef->chan) {
					if (link_info->ch_dur_ms > max_dur)
						max_dur = link_info->ch_dur_ms;
			}
		}
	}

	return max_dur == 0 ? bkop_para->ch_dur_ms : max_dur;
}

static u8
_assign_opch_list(struct phl_info_t *phl_info,
                  struct rtw_phl_scan_param *param,
                  u8 band_idx, struct cmd_scan_ctrl *sctrl)
{
	u8 op_num = 0, i = 0;
	struct rtw_chan_def chdef_list[MAX_WIFI_ROLE_NUMBER] = {0};
	struct rtw_wifi_role_link_t *rlink_list[MAX_WIFI_ROLE_NUMBER] = {0};
	u8 rlink_num = 0;

	if (param->back_op.sel == SCAN_BKOP_SEL_AUTO) {
		op_num = rtw_phl_mr_get_opch_list(phl_info,
		                                  band_idx,
		                                  chdef_list,
		                                  (u8)MAX_WIFI_ROLE_NUMBER,
		                                  NULL,
		                                  0);
	} else if (param->back_op.sel == SCAN_BKOP_SEL_MANUAL) {
		/* list all role link */
		rlink_num = param->back_op.link_info_num;
		for (i = 0; i < rlink_num; i++)
			rlink_list[i] = param->back_op.link_info_list[i].rlink;

		op_num = rtw_phl_mr_get_opch_list(phl_info,
		                                  band_idx,
		                                  chdef_list,
		                                  (u8)MAX_WIFI_ROLE_NUMBER,
		                                  rlink_list,
		                                  rlink_num);
	}

	for (i = 0; i < op_num; i++) {
		if (chdef_list[i].chan) {
			INIT_LIST_HEAD(&sctrl->back_op_ch[i].list);
			sctrl->back_op_ch[i].band = chdef_list[i].band;
			sctrl->back_op_ch[i].channel = chdef_list[i].chan;
			sctrl->back_op_ch[i].bw = chdef_list[i].bw;
			sctrl->back_op_ch[i].offset = chdef_list[i].offset;
			sctrl->back_op_ch[i].duration =
				_assign_opch_duration(&param->back_op, band_idx,
				                      &(chdef_list[i]));
			sctrl->back_op_ch[i].scan_mode = BACKOP_MODE;
		}
	}

	return op_num;
}

static enum rtw_phl_status
_cmd_scan_req_init(struct phl_info_t *phl_info,
		       struct rtw_phl_scan_param *param)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_chan_def chdef_list[MAX_WIFI_ROLE_NUMBER] = {0};
	u8 sctrl_idx = 0;
	struct cmd_scan_ctrl *sctrl = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 band_idx = 0, op_num = 0;
	bool is_cckphy = true;

	param->enqueue_time = _os_get_cur_time_ms();
	param->start_time = 0;
	param->state = 0;
	for(sctrl_idx = 0; sctrl_idx < param->sctrl_num; sctrl_idx++) {
		_os_mem_set(drv, &chdef_list, 0, sizeof(struct rtw_chan_def) * MAX_WIFI_ROLE_NUMBER);

		sctrl = &param->sctrl[sctrl_idx];
		sctrl->repeat = param->repeat;
		sctrl->ch_idx = -1;
		sctrl->last_opch_add_time = 0;
		pq_init(drv, &sctrl->chlist);

		sctrl->wrole = _phl_cmd_scan_select_wrole(phl_info, param, sctrl_idx);
		if(sctrl->wrole == NULL) {
			PHL_ERR("%s sctrl->wrole == NULL\n", __func__);
			_os_warn_on(1);
			return RTW_PHL_STATUS_FAILURE;
		}

		rlink = sctrl_to_rlink(sctrl, RTW_RLINK_PRIMARY);
		band_idx = rlink->hw_band;

		SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_INIT);

		#ifdef CONFIG_DBCC_SUPPORT
		is_cckphy = phl_mr_is_cckphy(phl_info, band_idx);
		#endif
		_cmd_scan_update_chlist(drv, param, sctrl_idx, is_cckphy);

		if (param->back_op.mode != SCAN_BKOP_NONE) {
			op_num = _assign_opch_list(phl_info, param, band_idx, sctrl);
		#ifdef CONFIG_PHL_CMD_SCAN_BKOP_TIME
			if (op_num && param->back_op.mode == SCAN_BKOP_TIMER)
				_cmd_scan_update_chparam(drv, param, sctrl_idx);
		#endif
		}

	#ifdef DBG_SCAN_CHAN_DUMP
	/* debug information*/
		_cmd_estimated_swch_seq(drv, param, op_num, sctrl_idx);
	#endif
	}

	return RTW_PHL_STATUS_SUCCESS;
}

static void
_cmd_scan_req_deinit(struct phl_info_t *phl_info,
                     struct rtw_phl_scan_param *param)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct cmd_scan_ctrl *sctrl = NULL;
	u8 sctrl_idx = 0;

	for(sctrl_idx = 0; sctrl_idx < param->sctrl_num; sctrl_idx++) {
		sctrl = &param->sctrl[sctrl_idx];
		pq_deinit(drv, &sctrl->chlist);
		sctrl->scan_ch = NULL;
	}
}

enum rtw_phl_status
_phl_cmd_scan_req_submit(struct phl_info_t *phl_info,
		       struct rtw_phl_scan_param *param)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_cmd_token_req fgreq[HW_BAND_MAX] = {0};
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 sctrl_idx = 0, band_idx = 0xff;

	for(sctrl_idx = 0; sctrl_idx < param->sctrl_num; sctrl_idx++) {
	/* Fill foreground command request */
		fgreq[sctrl_idx].module_id= PHL_FG_MDL_SCAN;
		fgreq[sctrl_idx].priv = param;

		fgreq[sctrl_idx].role = param->sctrl[sctrl_idx].wrole;

		fgreq[sctrl_idx].acquired = _phl_cmd_scan_req_acquired;
		fgreq[sctrl_idx].abort = _phl_cmd_scan_req_abort;
		fgreq[sctrl_idx].msg_hdlr = _phl_cmd_scan_req_ev_hdlr;
		fgreq[sctrl_idx].set_info = _phl_cmd_scan_req_set_info;
		fgreq[sctrl_idx].query_info = _phl_cmd_scan_req_query_info;

		rlink = sctrl_to_rlink((&param->sctrl[sctrl_idx]), RTW_RLINK_PRIMARY);
		band_idx = rlink->hw_band;

		if(param->sctrl[sctrl_idx].chlist.cnt) {
			/* cmd_dispatcher would copy whole phl_cmd_token_req */
			pstatus = phl_disp_eng_add_token_req(phl_info, band_idx, &fgreq[sctrl_idx], &param->sctrl[sctrl_idx].token);
}
		else {
			/*if chlist have no element */
			pstatus = RTW_PHL_STATUS_SUCCESS;
			SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_END);
		}

		if((pstatus == RTW_PHL_STATUS_SUCCESS) ||
		   (pstatus == RTW_PHL_STATUS_PENDING)) {
			//if(sctrl_idx == 0) param->token = param->sctrl[0].token;
		}
		else {
			if(sctrl_idx == 0) {
				PHL_ERR("%s: 1st cmd_scan failed!\n", __func__);
				return pstatus;
			}
			else {
				/* 2nd FG cmd failed */
				SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_END);
			}
		}

	}
	return RTW_PHL_STATUS_SUCCESS;
}

static u8
_cmd_scan_dbcc_policy(struct phl_info_t *phl_info,
                      struct rtw_phl_scan_param *param)
{
	u8 band_idx = 0, valid_role = 0;
	u8 sctrl_num = 1;

	/* case1: DBCC_en and SCAN_CMD_OPT_DB */
	if(param->opt == SCAN_CMD_OPT_DB) {
		for (band_idx = 0; band_idx < HW_BAND_MAX; band_idx++) {
			if(phl_mr_get_role_by_bandidx(phl_info, band_idx))
				valid_role = true;
			else
				valid_role = false;

		}
		if(valid_role)
			sctrl_num = band_idx; /* MAX_BAND_NUM */
	}
	/* case2:
	 *   - DBCC_en
	 *   - scan_req without SCAN_CMD_OPT_DB
	 *   - only a cck-phy module
	 *   - wrole->hw_band != cckphy
	 *   - chlist have 2G
	 */

	 return sctrl_num;
}

/* For EXTERNAL application to request scan (expose) */
/* @pscan: scan object
 * @pbuf: scan parameter, will be freed by caller after retrun
 */
enum rtw_phl_status rtw_phl_cmd_scan_request(void *phl,
	struct rtw_phl_scan_param *param)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);

	//Debug log
	if(param->wifi_role->rlink[RTW_RLINK_PRIMARY].hw_band == HW_BAND_1)
		PHL_INFO("rtw_phl_cmd_scan_request:: HW_BAND_1!\n");

#ifdef CONFIG_DBCC_SUPPORT
	if(rtw_phl_mr_is_db(phl_info))
		param->sctrl_num =_cmd_scan_dbcc_policy(phl_info, param);
	else
#endif
		param->sctrl_num = 1;

	PHL_INFO("[cmd_scan]:: param->sctrl_num = %d\n", param->sctrl_num);

	_os_mem_set(drv, param->sctrl, 0, MAX_BAND_NUM * sizeof(struct cmd_scan_ctrl));

	pstatus = _cmd_scan_req_init(phl_info, param);
	if(pstatus != RTW_PHL_STATUS_SUCCESS) {
		goto error;
	}

	/* cmd_dispatcher would copy whole phl_cmd_token_req */
	pstatus = _phl_cmd_scan_req_submit(phl_info, param);
	if((pstatus != RTW_PHL_STATUS_SUCCESS) &&
	   (pstatus != RTW_PHL_STATUS_PENDING)) {
		goto error;
	}

	return RTW_PHL_STATUS_SUCCESS;

error:
	if(param->sctrl) {
		_cmd_scan_req_deinit(phl_info, param);
		param->sctrl_num = 0;
	}

	return pstatus;
}

enum rtw_phl_status rtw_phl_cmd_scan_cancel(void *phl,
				struct rtw_phl_scan_param *param)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct cmd_scan_ctrl *sctrl = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 sctrl_idx = 0;

	for(sctrl_idx = 0; sctrl_idx < param->sctrl_num; sctrl_idx++) {
		sctrl = &param->sctrl[sctrl_idx];
		rlink = sctrl_to_rlink(sctrl, RTW_RLINK_PRIMARY);
		pstatus = phl_disp_eng_cancel_token_req(phl_info, rlink->hw_band, &sctrl->token);
		sctrl->token = 0;
	}
	//param->token = 0;
	return pstatus;
}

/**   Scan return format: (1Byte or 2Byte)
 *    0             1             2
 *    +-------------+-------------+
 *    |     MDL     |  sctrl_num  |
 *    +-------------+-------------+
**/
int rtw_phl_cmd_scan_inprogress(void *phl, u8 band_idx)
{
	struct phl_module_op_info op_info = {0};
	u8	mdl = 0;

	op_info.op_code = FG_REQ_OP_GET_MDL_ID;
	op_info.outbuf = (u8*)&mdl;
	op_info.outlen = 4;

	if(phl_disp_eng_query_cur_cmd_info(phl, band_idx,
				&op_info)== RTW_PHL_STATUS_SUCCESS ) {

		if(mdl == PHL_FG_MDL_SCAN)
			return true;
	}

	return false;
}

static void _cmd_scan_send_msg_done(void *priv, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;

	if (msg->inbuf && msg->inlen) {
		_os_kmem_free(phl_to_drvpriv(phl_info), msg->inbuf, msg->inlen);
	}
}

void rtw_phl_cmd_scan_send_msg(void *phl, u16 evt_id, u8 band_idx, u8 *buf,
			       u32 len)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	u8 *ptr = NULL;

	ptr = _os_kmem_alloc(phl_to_drvpriv(phl_info), len);
	if (ptr == NULL) {
		PHL_ERR("%s(): allocate ptr fail.\n", __func__);
		return;
	}

	_os_mem_cpy(phl_to_drvpriv(phl_info), ptr, buf, len);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_SCAN);
	attr.completion.completion = _cmd_scan_send_msg_done;
	attr.completion.priv = (void *)phl_info;
	msg.band_idx = band_idx;
	msg.inbuf = (u8 *)ptr;
	msg.inlen = len;

	SET_MSG_EVT_ID_FIELD(msg.msg_id, evt_id);

	pstatus = phl_disp_eng_send_msg(phl, &msg, &attr, NULL);
	if (RTW_PHL_STATUS_SUCCESS != pstatus) {
		_cmd_scan_send_msg_done(phl, &msg);
		PHL_ERR("%s sending msg failed \n", __func__);
	}
}

#endif /* CONFIG_PHL_CMD_SCAN */
