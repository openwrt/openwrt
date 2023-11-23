/******************************************************************************
 *
 * Copyright(c) 2022 Realtek Corporation.
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
#include "phl_headers.h"
#include "phl_scan.h"

#ifdef CONFIG_PHL_SCANOFLD

static enum rtw_phl_status
_scanofld_add_chnl(struct phl_info_t *phl_info,
		   struct rtw_wifi_role_link_t *rlink,
		   struct rtw_phl_scan_param *param)
{
	u8 ch_num = 0, i = 0;
	struct phl_scan_channel *ch = NULL;
	struct cmd_scan_ctrl *sctrl = NULL;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	void *d = phl_to_drvpriv(phl_info);
	u8 sctrl_idx = 0;
	struct scan_ofld_ch_info ch_info = {0};
	_os_list *node = NULL;
	struct rtw_chan_def chan_def = {0};

	sctrl_idx = phl_cmd_scan_ctrl(param, rlink->hw_band, &sctrl);
	if(sctrl == NULL) {
		PHL_ERR("%s: find sctrl failed\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}

	ch_num = (u8)sctrl->chlist.cnt;

	if (pq_get_front(d, &sctrl->chlist, &node, _ps) == false)
		return RTW_PHL_STATUS_SUCCESS;
	do {
		ch = (struct phl_scan_channel*)node;

		_os_mem_set(d, &ch_info, 0, sizeof(struct scan_ofld_ch_info));
		_os_mem_set(d, &chan_def, 0, sizeof(struct rtw_chan_def));
		ch_info.band =  chan_def.band = ch->band;
		ch_info.bw = chan_def.bw = ch->bw;
		chan_def.offset = ch->offset;
		chan_def.chan = (u8)ch->channel;
		ch_info.center_chan = rtw_phl_get_center_ch(&chan_def);
		ch_info.chan = (u8)ch->channel;
		ch_info.period = (u8)ch->duration;
		/* extend first, if not transforming to active, this channel will also be cancelled by probing */
		if (ch->type == RTW_PHL_SCAN_PASSIVE &&
		    param->ops->scan_ch_cancel) {
			ch_info.period += (u8)param->ext_act_scan_period;
		}
		ch_info.tx_null = ch->scan_mode == BACKOP_MODE ? true : false;
		ch_info.enter_notify = true;
		ch_info.chkpt_time = param->chkpt_time;

		if (rlink->mstate == MLME_LINKED &&
		    rlink->chandef.chan == ch->channel)
			ch_info.tx_data_pause = false;
		else
			ch_info.tx_data_pause = true;

		/* driver process probe request */
		ch_info.tx_pkt = false;

		hstatus = rtw_hal_scan_ofld_add_ch(phl_info->hal,
		 rlink->hw_band, &ch_info, i == (ch_num - 1) ? true : false);

		if (hstatus != RTW_HAL_STATUS_SUCCESS) {
			PHL_INFO("%s failed \n", __func__);
			return RTW_PHL_STATUS_FAILURE;
		}

		i++;

		if (!pq_get_next(d, &sctrl->chlist, node, &node, _ps))
			break;

	} while (1);


	PHL_INFO("%s succeeded \n", __func__);

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_scanofld_start(struct phl_info_t *phl_info, struct rtw_wifi_role_link_t *rlink,
                u16 mac_id, struct rtw_phl_scan_param *param)
{
	struct scan_ofld_info cfg = {0};
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct rtw_chan_def chdef = {0};

	/* add channel */
	pstatus = _scanofld_add_chnl(phl_info, rlink, param);

	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		return pstatus;

	rtw_hal_notification(phl_info->hal, MSG_EVT_SCANOFLD_START, rlink->hw_band);

	/* trigger fw to start scan */
	cfg.operation = SCAN_OFLD_OP_START;
	cfg.mode = SCAN_OFLD_MD_ONCE;
	cfg.end_notify = true;
	cfg.tgt_mode = true;
	rtw_hal_get_cur_chdef(phl_info->hal, rlink->hw_band, &chdef);
	_os_mem_cpy(phl_to_drvpriv(phl_info), &cfg.tgt_chandef, &chdef,
		    sizeof(struct rtw_chan_def));

	if (cfg.tgt_chandef.chan == 0)
		cfg.tgt_mode = false;

	hstatus = rtw_hal_scan_ofld(phl_info->hal, mac_id, rlink->hw_band,
	                            rlink->hw_port, &cfg);

	return hstatus == RTW_HAL_STATUS_SUCCESS ?
			RTW_PHL_STATUS_SUCCESS : RTW_PHL_STATUS_FAILURE;
}

static void
_scanofld_stop(struct phl_info_t *phl_info, struct rtw_wifi_role_link_t *rlink,
               struct rtw_phl_scan_param *param)
{
	struct scan_ofld_info cfg = {0};
	struct rtw_phl_stainfo_t *sta = rtw_phl_get_stainfo_self(phl_info, rlink);

	cfg.operation = SCAN_OFLD_OP_STOP;
	rtw_hal_scan_ofld(phl_info->hal, sta->macid, rlink->hw_band,
			  rlink->hw_port, &cfg);
	rtw_hal_notification(phl_info->hal, MSG_EVT_SCANOFLD_END, rlink->hw_band);
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

void phl_cmd_scanofld_end(void *drv, void *_param, u8 band_idx)
{
	struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param *)_param;
	struct rtw_phl_com_t *phl_com = param->wifi_role->phl_com;
	struct phl_info_t *phl_info = phl_com->phl_priv;
	void *d = phlcom_to_drvpriv(phl_com);
	struct cmd_scan_ctrl *sctrl = NULL;
	bool indicate = true;
	u8 sctrl_idx = 0xff;

	PHL_INFO("%s \n", __func__);

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
	_os_release_timer(d, &sctrl->scan_timer);

	if(TEST_SCAN_FLAG(param->state, band_idx, CMD_SCAN_STARTED) &&
	   !TEST_SCAN_FLAG(param->state, band_idx, CMD_SCAN_DF_IO) ) {
	        _scanofld_stop(phl_info, &sctrl->wrole->rlink[RTW_RLINK_PRIMARY],
			       param);
		rtw_hal_com_scan_restore_tx_lifetime(phl_info->hal,
						     band_idx);
		rtw_hal_scan_set_rxfltr_by_mode(phl_info->hal,
						band_idx,
						false, &sctrl->fltr_mode);

		rtw_hal_notification(phl_info->hal, MSG_EVT_SCAN_END,
				     band_idx);
	}

error:
	if (indicate) {
		rtw_hal_scan_ofld_cfg_en(phl_info->hal, false);
		_cmd_scan_req_deinit(phl_info, param);
		param->sctrl_num = 0;
	}

	PHL_INFO("[cmd_scan][%d]:: sctrl_idx[%d] param->state(0x%X), result(0x%X)\n",
	         band_idx, sctrl_idx, param->state, param->result);

	if (indicate && param->ops->scan_complete)
		param->ops->scan_complete(param->priv, param);

}

#define DEF_PROBE_PERIOD 20
/*
 * Return value :
 * 	false : cancel cur channel.
 *	true : keep scan on cur channel.
 */
static bool _scanofld_handle_probing(void *d,
				     struct rtw_phl_scan_param *param,
				     u8 sctrl_idx)
{
	struct cmd_scan_ctrl *sctrl = &param->sctrl[sctrl_idx];
	struct phl_scan_channel *scan_ch = sctrl->scan_ch;
	u32 dur = 0, diff_t = 0;
	bool check_cancel = true;

	/* no probe handler */
	if (!param->ops->scan_probe)
		return false;

	if (scan_ch == NULL)
		return false;

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

	return true;
}
static void
_cmd_scanofld_start(struct phl_info_t *phl_info,
		    struct cmd_scan_ctrl *sctrl,
		    u8 band_idx)
{
	rtw_hal_scan_set_rxfltr_by_mode(phl_info->hal, band_idx, true,
					&sctrl->fltr_mode);
	rtw_hal_com_scan_set_tx_lifetime(phl_info->hal, band_idx);
	rtw_hal_scan_ofld_cfg_en(phl_info->hal, true);
	rtw_hal_notification(phl_info->hal, MSG_EVT_SCAN_START,
                             band_idx);
}

static void
_scanofld_get_scan_ch_info(struct phl_info_t *phl_info,
			   struct rtw_phl_scan_param *param,
			   enum band_type band,
			   u8 chnl,
			   struct phl_scan_channel *scan_ch)
{
	u8 i = 0;

	for (i = 0; i < param->ch_num; i++) {
		if (param->ch[i].band == band && param->ch[i].channel == chnl) {
			scan_ch = &param->ch[i];
			return;
		}
	}

	scan_ch = NULL;
}

enum phl_mdl_ret_code
phl_cmd_scanofld_hdl_internal_evt(void* dispr,
			       void* priv,
			       void* _msg)
{
	struct phl_msg *msg = (struct phl_msg *)_msg;
	struct rtw_phl_scan_param *param = (struct rtw_phl_scan_param*)priv;
	struct rtw_wifi_role_t *wifi_role = param->wifi_role;
	struct rtw_phl_com_t *phl_com = wifi_role->phl_com;
	struct phl_info_t *phl_info = phl_com->phl_priv;
	void *d = phlcom_to_drvpriv(phl_com);
	u32 diff_time = 0;
	struct phl_msg nextmsg = {0};
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct rtw_scanofld_rsp *rsp = NULL;
	struct phl_scan_channel *scan_ch = NULL;
	u8 band_idx = 0xff, sctrl_idx = 0xff;
	struct cmd_scan_ctrl *sctrl = NULL;
	u8 i = 0;

	diff_time = phl_get_passing_time_ms(param->enqueue_time);

	if (param->max_scan_time && diff_time >= param->max_scan_time) {
		PHL_WARN("%s:: Timeout! %d > max_time %d\n",
				 __func__, diff_time, param->max_scan_time);
	}
	else {
		PHL_INFO("%s:: TimeIntvl: %u \n", __func__, diff_time);
	}

	phl_dispr_get_idx(dispr, &band_idx);
	sctrl_idx = phl_cmd_scan_ctrl(param, band_idx, &sctrl);
	if(sctrl == NULL) {
		PHL_ERR("%s: find sctrl failed\n", __func__);
		return MDL_RET_FAIL;
	}

	sta = rtw_phl_get_stainfo_self(phl_info,
				       &sctrl->wrole->rlink[RTW_RLINK_PRIMARY]);

	SET_MSG_MDL_ID_FIELD(nextmsg.msg_id, PHL_FG_MDL_SCAN);
	nextmsg.band_idx = band_idx;

	PHL_INFO("%s phy_idx %u \n", __func__, band_idx);
	switch(MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_SCAN_START:

		_cmd_scanofld_start(phl_info, sctrl, band_idx);
		/* [scan start notify] */
		if (!TEST_SCAN_FLAGS(param->state, CMD_SCAN_STARTED)) {
			if (param->ops->scan_start)
				param->ops->scan_start(param->priv, param);
		}

		SET_SCAN_FLAG(param->state, band_idx, CMD_SCAN_STARTED);

		pstatus = _scanofld_start(phl_info,
					  &sctrl->wrole->rlink[RTW_RLINK_PRIMARY],
					  sta->macid, param);

		if (pstatus != RTW_PHL_STATUS_SUCCESS) {
			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SCAN_END);
			nextmsg.rsvd[0].ptr = (u8*)sctrl->wrole;
			pstatus = phl_disp_eng_send_msg(phl_info, &nextmsg,
							&attr, NULL);
			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				return MDL_RET_FAIL;
		}
		break;
	case MSG_EVT_SCANOFLD_SWITCH:
		rsp = (struct rtw_scanofld_rsp *)msg->inbuf;
		scan_ch = NULL;
		for (i = 0; i < param->ch_num; i++) {
			if (param->ch[i].band == rsp->band &&
			    param->ch[i].channel == rsp->pri_ch) {
				scan_ch = &param->ch[i];
				break;
			}
		}
		sctrl->scan_ch = scan_ch;
		if (sctrl->scan_ch != NULL) {
			if (!BAND_6GHZ(scan_ch->band)) {
				if ((scan_ch->scan_mode != BACKOP_MODE) &&
				    (scan_ch->type == RTW_PHL_SCAN_ACTIVE)) {
					if (param->ops->scan_issue_pbreq)
						param->ops->scan_issue_pbreq(param->priv, param, sctrl_idx);
				}
			}
			if (param->ops->scan_probe) {
				scan_ch->start_t = _os_get_cur_time_ms();
				scan_ch->last_t = 0;
				scan_ch->pass_t = 0;
				scan_ch->remain_t = scan_ch->duration;
			}
			if (param->ops->scan_ch_ready)
				param->ops->scan_ch_ready(param->priv, param, sctrl_idx);
		}
		break;
	case MSG_EVT_SCANOFLD_CHKPT_TIMER:
		rsp = (struct rtw_scanofld_rsp *)msg->inbuf;
		if (!_scanofld_handle_probing(d, param, band_idx)) {
			PHL_INFO("%s leave cur chnl \n", __func__);
			rtw_hal_scan_ofld_leave_cur_chnl(phl_info->hal, band_idx,
							 rsp->band, rsp->pri_ch);
		}
		break;
	case MSG_EVT_SCANOFLD_END:
		PHL_INFO("%s: MSG_EVT_SCANOFLD_END \n", __func__);
		/* scan finished */
		SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SCAN_END);
		nextmsg.rsvd[0].ptr = (u8*)sctrl->wrole;
		pstatus = phl_disp_eng_send_msg(phl_info, &nextmsg, &attr, NULL);
		if (pstatus != RTW_PHL_STATUS_SUCCESS)
			PHL_ERR("%s :: [MSG_EVT_SCANOFLD_END] dispr_send_msg failed\n", __func__);
		break;

	case MSG_EVT_SCAN_END:
		PHL_INFO("%s: MSG_EVT_SCAN_END \n", __func__);
		pstatus = phl_disp_eng_free_token(phl_info, band_idx, &sctrl->token);
		if(pstatus == RTW_PHL_STATUS_SUCCESS) {
			param->result = SCAN_REQ_COMPLETE;
			phl_cmd_scanofld_end(d, param, band_idx);
		} else {
			PHL_WARN("%s :: [SCAN_END] Abort occurred, skip!\n", __func__);
		}
		break;
	default:
			/* unknown state */
		break;
	}

	return MDL_RET_SUCCESS;
}
#endif
