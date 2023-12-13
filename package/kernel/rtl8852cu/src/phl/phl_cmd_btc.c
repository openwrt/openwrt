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
#define _PHL_CMD_BTC_C_
#include "phl_headers.h"

#ifdef CONFIG_BTCOEX
#ifdef CONFIG_PHL_CMD_BTC
static void _fail_hdlr(void *phl, struct phl_msg *msg)
{
}

static void _hdl_tmr(void *phl, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_btc_timer(phl_info->hal, (void *)msg->inbuf);
}

static void _hdl_role_notify(void *phl, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_role_cmd *rcmd = NULL;
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (msg->inbuf && (msg->inlen == sizeof(struct rtw_role_cmd))) {
		rcmd  = (struct rtw_role_cmd *)msg->inbuf;
		wrole = rcmd->wrole;
		/* we only handle PHL_LINK_UP_NOA in _hdl_role_notify
		     and handle the others in FG modules
		  */
		if (rcmd->lstate != PHL_LINK_UP_NOA)
			return;
		/* Notify all links in a role */
		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rlink = get_rlink(wrole, idx);

			sta = rtw_phl_get_stainfo_self(phl_info, rlink);

			rtw_hal_btc_update_role_info_ntfy(phl_info->hal,
			                                  rcmd->wrole->id,
			                                  wrole,
			                                  rlink,
			                                  sta,
			                                  rcmd->lstate);
		}
	} else {
		PHL_ERR("%s: invalid msg, buf = %p, len = %d\n",
			__func__, msg->inbuf, msg->inlen);
	}
}

static void _hdl_pkt_evt_notify(void *phl, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum phl_pkt_evt_type *pkt_evt_type = NULL;
	u8 *cmd = NULL;
	u32 cmd_len = 0;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;

	pstatus = phl_cmd_get_cur_cmdinfo(phl_info, msg->band_idx,
					msg, &cmd, &cmd_len);
	if (pstatus != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: get cmd info fail, status = %d\n",
			__func__, pstatus);
		return;
	}

	pkt_evt_type = (enum phl_pkt_evt_type *)cmd;
	rtw_hal_btc_packet_event_ntfy(phl_info->hal, *pkt_evt_type);
}

static enum phl_mdl_ret_code _btc_cmd_init(void *phl, void *dispr,
						  void **priv)
{
	PHL_INFO("[BTCCMD], %s(): \n", __func__);

	*priv = phl;
	return MDL_RET_SUCCESS;
}

static void _btc_cmd_deinit(void *dispr, void *priv)
{
	PHL_INFO("[BTCCMD], %s(): \n", __func__);
}

static enum phl_mdl_ret_code _btc_cmd_start(void *dispr, void *priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;

	PHL_INFO("[BTCCMD], %s(): \n", __func__);

	return ret;
}

static enum phl_mdl_ret_code _btc_cmd_stop(void *dispr, void *priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;

	PHL_INFO("[BTCCMD], %s(): \n", __func__);

	return ret;
}

static enum phl_mdl_ret_code
_btc_internal_pre_msg_hdlr(struct phl_info_t *phl_info,
                           void *dispr,
                           struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum phl_msg_evt_id evt_id = MSG_EVT_ID_FIELD(msg->msg_id);

	/*PHL_INFO("[BTCCMD], msg->band_idx = %d,  msg->msg_id = 0x%x\n",
		msg->band_idx, msg->msg_id);*/

	switch(evt_id) {
	case MSG_EVT_BTC_REQ_BT_SLOT:
		PHL_INFO("[BTCCMD], MSG_EVT_BTC_REQ_BT_SLOT \n");
		ret = MDL_RET_SUCCESS;
		break;

	default:
		break;
	}

	return ret;
}

static enum phl_mdl_ret_code
_btc_internal_post_msg_hdlr(struct phl_info_t *phl_info,
                            void *dispr,
                            struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum phl_msg_evt_id evt_id = MSG_EVT_ID_FIELD(msg->msg_id);

	switch(evt_id) {
	case MSG_EVT_BTC_TMR:
		PHL_DBG("[BTCCMD], MSG_EVT_BTC_TMR \n");
		_hdl_tmr(phl_info, msg);
		ret = MDL_RET_SUCCESS;
		break;

	case MSG_EVT_BTC_FWEVNT:
		PHL_DBG("[BTCCMD], MSG_EVT_BTC_FWEVNT \n");
		rtw_hal_btc_fwinfo_ntfy(phl_info->hal);
		ret = MDL_RET_SUCCESS;
		break;

	default:
		break;
	}

	return ret;
}

static enum phl_mdl_ret_code
_btc_internal_msg_hdlr(struct phl_info_t *phl_info,
                       void *dispr,
                       struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if (IS_MSG_IN_PRE_PHASE(msg->msg_id))
		ret = _btc_internal_pre_msg_hdlr(phl_info, dispr, msg);
	else
		ret = _btc_internal_post_msg_hdlr(phl_info, dispr, msg);

	return ret;
}

static enum phl_mdl_ret_code
_btc_pre_handle_connect_start(struct phl_info_t *phl, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->inbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
						sta, PHL_EN_LINK_START);
	}
#ifdef CONFIG_DBCC_SUPPORT
	/* Be going to enable DBCC */
	/* Disable mechanism right now */
	if (phl_mr_is_trigger_dbcc(phl)) {
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
						sta, PHL_EN_DBCC_START);
	}
#endif /* CONFIG_DBCC_SUPPORT */
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_pre_handle_disconnect_prepare(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
						sta, PHL_DIS_LINK_START);
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_pre_handle_disconnect_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT &&
		MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_BTC, _PHL_INFO_, "%s: Msg_band(%d)\n",
		__FUNCTION__, msg->band_idx);
#ifdef CONFIG_DBCC_SUPPORT
	/* Be going to disable DBCC */
	if (phl_mr_is_trigger_dbcc(phl)) {
		struct rtw_wifi_role_link_t *rlink = NULL;
		enum phl_band_idx oth_band = (msg->band_idx == HW_BAND_0)
						? HW_BAND_1 : HW_BAND_0;

		/* Disable mechanism right now */
		rlink = phl_mr_get_first_rlink_by_band(phl, oth_band);
		if (rlink != NULL) {
			struct rtw_phl_stainfo_t *sta = NULL;

			sta = rtw_phl_get_stainfo_self(phl, rlink);
			rtw_hal_btc_update_role_info_ntfy(phl->hal,
						rlink->wrole->id, rlink->wrole,
						rlink, sta, PHL_DIS_DBCC_START);
		}
	}
#endif /* CONFIG_DBCC_SUPPORT */
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_pre_handle_ap_start_prepare(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct phl_module_op_info op_info = {0};
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	op_info.op_code = FG_REQ_OP_GET_ROLE;
	if (phl_disp_eng_query_cur_cmd_info(phl, msg->band_idx, &op_info)
						!= RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_BTC, _PHL_WARNING_, "Query wifi role fail!\n");
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)op_info.outbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
						sta, PHL_EN_LINK_START);
	}
#ifdef CONFIG_DBCC_SUPPORT
	/* Be going to enable DBCC */
	/* Disable mechanism right now */
	if (phl_mr_is_trigger_dbcc(phl)) {
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
						sta, PHL_EN_DBCC_START);
	}
#endif /* CONFIG_DBCC_SUPPORT */
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_pre_handle_ap_stop_prepare(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
						sta, PHL_DIS_LINK_START);
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_pre_handle_ap_stop_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_BTC, _PHL_INFO_, "%s: Msg_band(%d)\n",
		__FUNCTION__, msg->band_idx);
#ifdef CONFIG_DBCC_SUPPORT
	/* Be going to disable DBCC */
	if (phl_mr_is_trigger_dbcc(phl)) {
		struct rtw_wifi_role_link_t *rlink = NULL;
		enum phl_band_idx oth_band = (msg->band_idx == HW_BAND_0)
						? HW_BAND_1 : HW_BAND_0;

		/* Disable mechanism right now */
		rlink = phl_mr_get_first_rlink_by_band(phl, oth_band);
		if (rlink != NULL) {
			struct rtw_phl_stainfo_t *sta = NULL;

			sta = rtw_phl_get_stainfo_self(phl, rlink);
			rtw_hal_btc_update_role_info_ntfy(phl->hal,
						rlink->wrole->id, rlink->wrole,
						rlink, sta, PHL_DIS_DBCC_START);
		}
	}
#endif /* CONFIG_DBCC_SUPPORT */
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_post_handle_swch_done(struct phl_info_t *phl,
						struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, rlink->wrole->id);
	sta = rtw_phl_get_stainfo_self(phl, rlink);
	rtw_hal_btc_update_role_info_ntfy(phl->hal, rlink->wrole->id, rlink->wrole,
					rlink, sta, PHL_LINK_STARTED);
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}


static enum phl_mdl_ret_code
_btc_post_handle_connect_done(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct phl_module_op_info op_info = {0};
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	op_info.op_code = FG_REQ_OP_GET_ROLE;
	if(phl_disp_eng_query_cur_cmd_info(phl, msg->band_idx, &op_info)
		!= RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "Query wifi role fail!\n");
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)op_info.outbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
						sta, PHL_EN_LINK_DONE);
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_post_handle_disconnect_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT &&
		MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->inbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
						sta, PHL_DIS_LINK_DONE);
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_post_handle_ap_start_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
					sta, PHL_EN_LINK_DONE);
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_post_handle_ap_stop_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->inbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
					sta, PHL_DIS_LINK_DONE);
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_post_handle_ecsa_swch_done(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_ECSA ) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[1].ptr;
	sta = rtw_phl_get_stainfo_self(phl, rlink);
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	rtw_hal_btc_update_role_info_ntfy(phl->hal, role->id, role, rlink,
						sta, PHL_LINK_CHG_CH);
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

#ifdef CONFIG_DBCC_SUPPORT
static enum phl_mdl_ret_code
_btc_post_handle_dbcc_enable(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_MRC) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d)\n",
		__FUNCTION__, msg->band_idx);
	/* resume mechanism for band0 */
	rlink = phl_mr_get_first_rlink_by_band(phl, HW_BAND_0);
	if (rlink != NULL) {
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, rlink->wrole->id,
						rlink->wrole, rlink,
						sta, PHL_EN_DBCC_DONE);
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_btc_post_handle_dbcc_disable(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_MRC) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d)\n",
		__FUNCTION__, msg->band_idx);
	/* resume mechanism for band0 */
	rlink = phl_mr_get_first_rlink_by_band(phl, HW_BAND_0);
	if (rlink != NULL) {
		sta = rtw_phl_get_stainfo_self(phl, rlink);
		rtw_hal_btc_update_role_info_ntfy(phl->hal, rlink->wrole->id,
						rlink->wrole, rlink,
						sta, PHL_DIS_DBCC_DONE);
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

#endif /* CONFIG_DBCC_SUPPORT */

static enum phl_mdl_ret_code
_btc_external_pre_msg_hdlr(struct phl_info_t *phl_info,
                           void *dispr,
                           struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum phl_msg_evt_id evt_id = MSG_EVT_ID_FIELD(msg->msg_id);
	enum band_type band = BAND_ON_5G;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	enum phl_phy_idx phy_idx = HW_PHY_0;

	switch(evt_id) {
	case MSG_EVT_SCAN_START:
		if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_SCAN)
			break;
		if (msg->band_idx == HW_BAND_1)
			phy_idx = HW_PHY_1;
		PHL_INFO("[BTCCMD], MSG_EVT_SCAN_START \n");
		band = hal_com->band[msg->band_idx].cur_chandef.band;
		rtw_hal_btc_scan_start_ntfy(phl_info->hal, phy_idx, band);
		ret = MDL_RET_SUCCESS;
		break;
	case MSG_EVT_CONNECT_START:
		PHL_INFO("[BTCCMD], MSG_EVT_CONNECT_START \n");
		ret = _btc_pre_handle_connect_start(phl_info, msg);
		break;
	case MSG_EVT_DISCONNECT_PREPARE:
		ret = _btc_pre_handle_disconnect_prepare(phl_info, msg);
		break;
	case MSG_EVT_DISCONNECT_END:
		ret = _btc_pre_handle_disconnect_end(phl_info, msg);
		break;
	case MSG_EVT_AP_START_PREPARE:
		ret = _btc_pre_handle_ap_start_prepare(phl_info, msg);
		break;
	case MSG_EVT_AP_STOP_PREPARE:
		ret = _btc_pre_handle_ap_stop_prepare(phl_info, msg);
		break;
	case MSG_EVT_AP_STOP_END:
		ret = _btc_pre_handle_ap_stop_end(phl_info, msg);
		break;
	case MSG_EVT_PKT_EVT_NTFY:
		PHL_INFO("[BTCCMD], MSG_EVT_PKT_EVT_NTFY \n");
		_hdl_pkt_evt_notify(phl_info, msg);
		ret = MDL_RET_SUCCESS;
		break;
	default:
		PHL_TRACE(COMP_PHL_BTC, _PHL_INFO_, "%s: MDL(%d), EVT(%d), Not handle event in pre-phase\n",
			__FUNCTION__, MSG_MDL_ID_FIELD(msg->msg_id),
			MSG_EVT_ID_FIELD(msg->msg_id));
		break;
	}
	if (ret == MDL_RET_FAIL) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			"%s: MDL(%d), EVT(%d), handle event failed\n",
			__FUNCTION__, MSG_MDL_ID_FIELD(msg->msg_id),
			MSG_EVT_ID_FIELD(msg->msg_id));
	}
	return ret;
}

static enum phl_mdl_ret_code
_btc_external_post_msg_hdlr(struct phl_info_t *phl_info,
                            void *dispr,
                            struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum phl_msg_evt_id evt_id = MSG_EVT_ID_FIELD(msg->msg_id);
	struct hal_info_t *hal_info = (struct hal_info_t *)phl_info->hal;
	enum phl_phy_idx phy_idx = HW_PHY_0;

	switch(evt_id) {
	case MSG_EVT_SCAN_END:
		if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_SCAN)
			break;
		if (msg->band_idx == HW_BAND_1)
			phy_idx = HW_PHY_1;
		PHL_DBG("[BTCCMD], MSG_EVT_SCAN_END \n");
		rtw_hal_btc_scan_finish_ntfy(hal_info, phy_idx);
		ret = MDL_RET_SUCCESS;
		break;
	case MSG_EVT_SWCH_DONE:
		ret = _btc_post_handle_swch_done(phl_info, msg);
		break;
	case MSG_EVT_CONNECT_END:
		ret = _btc_post_handle_connect_done(phl_info, msg);
		break;
	case MSG_EVT_DISCONNECT_END:
		ret = _btc_post_handle_disconnect_end(phl_info, msg);
		break;
	case MSG_EVT_AP_START_END:
		ret = _btc_post_handle_ap_start_end(phl_info, msg);
		break;
	case MSG_EVT_AP_STOP_END:
		ret = _btc_post_handle_ap_stop_end(phl_info, msg);
		break;
	case MSG_EVT_ECSA_SWITCH_DONE:
		ret = _btc_post_handle_ecsa_swch_done(phl_info, msg);
		break;
#ifdef CONFIG_DBCC_SUPPORT
	case MSG_EVT_DBCC_ENABLE:
		ret = _btc_post_handle_dbcc_enable(phl_info, msg);
		break;
	case MSG_EVT_DBCC_DISABLE:
		ret = _btc_post_handle_dbcc_disable(phl_info, msg);
		break;
#endif /* CONFIG_DBCC_SUPPORT */
	case MSG_EVT_ROLE_NTFY:
		if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_MRC)
			break;
		PHL_DBG("[BTCCMD], MSG_EVT_ROLE_NTFY \n");
		_hdl_role_notify(phl_info, msg);
		ret = MDL_RET_SUCCESS;
		break;
	case MSG_EVT_BTC_TMR:
		PHL_DBG("[BTCCMD], MSG_EVT_BTC_TMR \n");
		_hdl_tmr(phl_info, msg);
		ret = MDL_RET_SUCCESS;
		break;
	case MSG_EVT_BTC_FWEVNT:
		PHL_DBG("[BTCCMD], MSG_EVT_BTC_FWEVNT \n");
		rtw_hal_btc_fwinfo_ntfy(phl_info->hal);
		ret = MDL_RET_SUCCESS;
		break;
	default:
		PHL_TRACE(COMP_PHL_BTC, _PHL_INFO_, "%s: MDL(%d), EVT(%d), Not handle event in post-phase\n",
			__FUNCTION__, MSG_MDL_ID_FIELD(msg->msg_id),
			MSG_EVT_ID_FIELD(msg->msg_id));
		break;
	}
	if (ret == MDL_RET_FAIL) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			"%s: MDL(%d), EVT(%d), handle event failed\n",
			__FUNCTION__, MSG_MDL_ID_FIELD(msg->msg_id),
			MSG_EVT_ID_FIELD(msg->msg_id));
	}
	return ret;
}

static enum phl_mdl_ret_code
_btc_external_msg_hdlr(struct phl_info_t *phl_info,
                       void *dispr,
                       struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if (IS_MSG_IN_PRE_PHASE(msg->msg_id))
		ret = _btc_external_pre_msg_hdlr(phl_info, dispr, msg);
	else
		ret = _btc_external_post_msg_hdlr(phl_info, dispr, msg);

	return ret;
}

static enum phl_mdl_ret_code
_btc_msg_hdlr(void *dispr, void *priv, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;

	FUNCIN();

	if (IS_MSG_FAIL(msg->msg_id)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
			  "%s: cmd dispatcher notify cmd failure: 0x%x.\n",
			   __FUNCTION__, msg->msg_id);
		_fail_hdlr(phl_info, msg);
		FUNCOUT();
		return MDL_RET_FAIL;
	}

	if (IS_PRIVATE_MSG(msg->msg_id)) {
		FUNCOUT();
		return ret;
	}

	switch(MSG_MDL_ID_FIELD(msg->msg_id)) {
		case PHL_MDL_BTC:
			ret = _btc_internal_msg_hdlr(phl_info, dispr, msg);
			break;

		default:
			ret = _btc_external_msg_hdlr(phl_info, dispr, msg);
			break;
	}

	FUNCOUT();
	return ret;
}

static enum phl_mdl_ret_code
_btc_set_info(void *dispr, void *priv,
			 struct phl_module_op_info *info)
{
	PHL_INFO("[BTCCMD], %s(): \n", __func__);

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_btc_query_info(void *dispr, void *priv,
			   struct phl_module_op_info *info)
{
	PHL_INFO("[BTCCMD], %s(): \n", __func__);

	return MDL_RET_SUCCESS;
}

static void _btc_set_stbc(struct phl_info_t *phl_info, u8 *buf)
{
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);

	hal_com->btc_ctrl.disable_rx_stbc = buf[0];
	PHL_INFO("[BTCCMD], %s(): disable_rx_stbc(%d) \n",
			__func__, hal_com->btc_ctrl.disable_rx_stbc);
}

enum rtw_phl_status phl_register_btc_module(struct phl_info_t *phl_info)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_cmd_dispatch_engine *disp_eng = &(phl_info->disp_eng);
	struct phl_bk_module_ops bk_ops = {0};
	u8 i = 0;

	PHL_INFO("[BTCCMD], %s(): \n", __func__);

	bk_ops.init = _btc_cmd_init;
	bk_ops.deinit = _btc_cmd_deinit;
	bk_ops.start = _btc_cmd_start;
	bk_ops.stop = _btc_cmd_stop;
	bk_ops.msg_hdlr = _btc_msg_hdlr;
	bk_ops.set_info = _btc_set_info;
	bk_ops.query_info = _btc_query_info;

	for (i = 0; i < disp_eng->phy_num; i++) {
		sts = phl_disp_eng_register_module(phl_info, i, PHL_MDL_BTC,
						&bk_ops);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
				"%s register PHL_MDL_MR_COEX in cmd disp failed :%d\n",
				__FUNCTION__, i);
			break;
		}
	}
	return sts;
}

bool rtw_phl_btc_send_cmd(struct rtw_phl_com_t *phl_com,
			enum phl_band_idx hw_band, u8 *buf, u32 len, u16 ev_id)
{
	struct phl_info_t *phl_info = phl_com->phl_priv;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	msg.inbuf = buf;
	msg.inlen = len;
	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_BTC);
	msg.band_idx = hw_band;
	switch (ev_id) {
	case BTC_HMSG_TMR_EN:
		SET_MSG_EVT_ID_FIELD(msg.msg_id,
			MSG_EVT_BTC_TMR);
		break;
	case BTC_HMSG_SET_BT_REQ_SLOT:
		SET_MSG_EVT_ID_FIELD(msg.msg_id,
			MSG_EVT_BTC_REQ_BT_SLOT);
		break;
	case BTC_HMSG_FW_EV:
		SET_MSG_EVT_ID_FIELD(msg.msg_id,
			MSG_EVT_BTC_FWEVNT);
		break;
	case BTC_HMSG_SET_BT_REQ_STBC:
		_btc_set_stbc(phl_info, buf);
		return true;
	default:
		PHL_ERR("%s: Unknown msg !\n", __func__);
		return false;
	}

	if (!TEST_STATUS_FLAG(phl_com->dev_state, RTW_DEV_WORKING)) {
		PHL_ERR("%s: phl_com->dev_state is wrong(%d)\n", __func__,
				phl_com->dev_state);
		return false;
	}

	if (phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL) !=
				RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: [BTC] dispr_send_msg failed !\n", __func__);
		return false;
	}

	return true;
}
#endif /*CONFIG_PHL_CMD_BTC*/

#ifndef CONFIG_FSM
int rtw_phl_btc_notify(void *phl, enum RTW_PHL_BTC_NOTIFY notify,
				struct rtw_phl_btc_ntfy *ntfy)
{
	PHL_ERR("CMD_BTC not support :%s\n", __func__);
	return 0;
}
void rtw_phl_btc_role_notify(void *phl, u8 role_id, enum role_state rstate)
{
	struct rtw_phl_btc_ntfy ntfy = {0};
	struct rtw_phl_btc_role_info_param *prinfo = &ntfy.u.rinfo;

	prinfo->role_id = role_id;
	prinfo->rstate = rstate;

	ntfy.notify = PHL_BTC_NTFY_ROLE_INFO;
	ntfy.ops = NULL;
	ntfy.priv = NULL;
	ntfy.ntfy_cb = NULL;

	rtw_phl_btc_notify(phl, ntfy.notify, &ntfy);
}

void rtw_phl_btc_hub_msg_hdl(void *phl, struct phl_msg *msg)
{
}
#endif

#else

enum rtw_phl_status phl_register_btc_module(struct phl_info_t *phl_info)
{
	return RTW_PHL_STATUS_SUCCESS;
}


bool rtw_phl_btc_send_cmd(struct rtw_phl_com_t *phl_com,
			enum phl_band_idx hw_band, u8 *buf, u32 len, u16 ev_id)
{
	return 0;
}

#endif /*CONFIG_BTCOEX*/

