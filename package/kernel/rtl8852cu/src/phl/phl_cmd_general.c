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
#define _PHL_CMD_GENERAL_C_
#include "phl_headers.h"

#ifdef CONFIG_CMD_DISP

enum phl_cmd_sts {
	PHL_CMD_SUBMITTED = -1,
	PHL_CMD_DONE_SUCCESS = 0,
	PHL_CMD_DONE_TIMEOUT,
	PHL_CMD_DONE_CMD_ERROR,
	PHL_CMD_DONE_CMD_DROP,
	PHL_CMD_DONE_CANNOT_IO,
	PHL_CMD_DONE_UNKNOWN,
};

struct phl_sync {
	u32 submit_time;
	u32 timeout_ms; /* 0: wait forever, >0: up to ms waiting */
	enum phl_cmd_sts status; /* status for operation */
	_os_event done;
};

struct phl_cmd_sync {
	enum phl_msg_evt_id evt_id;
	struct phl_sync sync;
	_os_lock lock;
};

struct phl_cmd_obj {
	enum phl_msg_evt_id evt_id; /* u8 id */
	u8 *buf;
	u32 buf_len;
	bool no_io;
	void (*cmd_complete)(void* priv, u8 *buf, u32 buf_len, enum rtw_phl_status status);
	/*cmd sync*/
	bool is_cmd_wait;
	_os_atomic ref_cnt;
	struct phl_cmd_sync cmd_sync;
};


#define DBG_CMD_SYNC

#ifdef DBG_CMD_SYNC
static void _phl_cmd_sync_dump(struct phl_cmd_sync *cmd_sync, const char *caller)
{
	PHL_INFO("[CMD_SYNC] %s\n", caller);
	PHL_INFO("[CMD_SYNC] evt_id:%d status:%d\n", cmd_sync->evt_id, cmd_sync->sync.status);
	PHL_INFO("[CMD_SYNC] take:%d ms\n", phl_get_passing_time_ms(cmd_sync->sync.submit_time));
}
#endif

static void _phl_cmd_sync_init(struct phl_info_t *phl_info,
		enum phl_msg_evt_id evt_id,
		struct phl_cmd_sync *cmd_sync, u32 timeout_ms)
{
	void *drv = phl_to_drvpriv(phl_info);

	_os_spinlock_init(drv, &cmd_sync->lock);
	cmd_sync->evt_id = evt_id;
	cmd_sync->sync.timeout_ms = timeout_ms;
	cmd_sync->sync.submit_time = _os_get_cur_time_ms();
	_os_event_init(drv, &(cmd_sync->sync.done));
	cmd_sync->sync.status = PHL_CMD_SUBMITTED;
}

static void _phl_cmd_sync_deinit(struct phl_info_t *phl_info,
			struct phl_cmd_sync *cmd_sync)
{
	#ifdef DBG_CMD_SYNC
	_phl_cmd_sync_dump(cmd_sync, __func__);
	#endif
	_os_event_free(phl_to_drvpriv(phl_info), &(cmd_sync->sync.done));
	_os_spinlock_free(phl_to_drvpriv(phl_info), &cmd_sync->lock);
}

inline static enum rtw_phl_status _cmd_stat_2_phl_stat(enum phl_cmd_sts status)
{
	if (status == PHL_CMD_DONE_TIMEOUT)
		return RTW_PHL_STATUS_CMD_TIMEOUT;
	else if(status == PHL_CMD_DONE_CANNOT_IO)
		return RTW_PHL_STATUS_CMD_CANNOT_IO;
	else if (status == PHL_CMD_DONE_CMD_ERROR)
		return RTW_PHL_STATUS_CMD_ERROR;
	else if (status == PHL_CMD_DONE_CMD_DROP)
		return RTW_PHL_STATUS_CMD_DROP;
	else
		return RTW_PHL_STATUS_CMD_SUCCESS;
}

static enum rtw_phl_status
_phl_cmd_wait(struct phl_info_t *phl_info, struct phl_cmd_sync *cmd_sync)
{
	void *drv = phl_to_drvpriv(phl_info);
	u32 cmd_wait_ms = cmd_sync->sync.timeout_ms;/*0: wait forever, >0: up to ms waiting*/

	#ifdef DBG_CMD_SYNC
	PHL_INFO("evt_id:%d %s in...............\n", cmd_sync->evt_id, __func__);
	#endif

	if (_os_event_wait(drv, &cmd_sync->sync.done, cmd_wait_ms) == 0) {
		_os_spinlock(drv, &cmd_sync->lock, _bh, NULL);
		cmd_sync->sync.status = PHL_CMD_DONE_TIMEOUT;
		_os_spinunlock(drv, &cmd_sync->lock, _bh, NULL);
		PHL_ERR("%s evt_id:%d timeout\n", __func__, cmd_sync->evt_id);
	}
	#ifdef DBG_CMD_SYNC
	PHL_INFO("evt_id:%d %s out...............\n", cmd_sync->evt_id, __func__);
	_phl_cmd_sync_dump(cmd_sync, __func__);
	#endif
	return _cmd_stat_2_phl_stat(cmd_sync->sync.status);
}

static bool _phl_cmd_chk_wating_status(enum phl_cmd_sts status)
{
	switch (status) {
	case PHL_CMD_SUBMITTED:
		/* fall through */
	case PHL_CMD_DONE_UNKNOWN:
		return true;
	default:
		return false;
	}
}

static void _phl_cmd_done(struct phl_info_t *phl_info,
		struct phl_cmd_sync *cmd_sync, enum phl_cmd_sts status)
{
	void *drv = phl_to_drvpriv(phl_info);

	if (!cmd_sync) {
		PHL_ERR("%s cmd_sync is NULL\n", __func__);
		_os_warn_on(1);
		return;
	}
	#ifdef DBG_CMD_SYNC
	PHL_INFO("evt_id:%d %s in...............\n", cmd_sync->evt_id, __func__);
	#endif
	_os_spinlock(drv, &cmd_sync->lock, _bh, NULL);
	if (_phl_cmd_chk_wating_status(cmd_sync->sync.status)) {
		PHL_INFO("%s status:%d\n", __func__, status);
		cmd_sync->sync.status = status;
	}
	_os_spinunlock(drv, &cmd_sync->lock, _bh, NULL);

	_os_event_set(drv, &cmd_sync->sync.done);
	#ifdef DBG_CMD_SYNC
	PHL_INFO("evt_id:%d %s out...............\n", cmd_sync->evt_id, __func__);
	_phl_cmd_sync_dump(cmd_sync, __func__);
	#endif
}
/********************************************************/
static enum rtw_phl_status
_phl_cmd_general_pre_phase_msg_hdlr(struct phl_info_t *phl_info, void *dispr,
				    struct phl_msg *msg)
{
	enum phl_msg_evt_id evt_id = MSG_EVT_ID_FIELD(msg->msg_id);
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_cmd_obj *phl_cmd = NULL;

	phl_cmd = (struct phl_cmd_obj *)msg->inbuf;

	switch (evt_id) {
	case MSG_EVT_DBG_RX_DUMP:
		/* Do Nothing */
		psts = RTW_PHL_STATUS_SUCCESS;
	break;
	case MSG_EVT_DBG_TX_DUMP:
		/* Do Nothing */
		psts = RTW_PHL_STATUS_SUCCESS;
	break;
	case MSG_EVT_NONE:
		/* fall through */
	default:
		psts = RTW_PHL_STATUS_SUCCESS;
		break;
	}

	return psts;
}

static enum rtw_phl_status
_phl_cmd_general_post_phase_msg_hdlr(struct phl_info_t *phl_info, void *dispr,
				     struct phl_msg *msg)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	enum phl_msg_evt_id evt_id = MSG_EVT_ID_FIELD(msg->msg_id);
	struct phl_cmd_obj *phl_cmd = NULL;

	phl_cmd = (struct phl_cmd_obj *)msg->inbuf;

	switch (evt_id) {
#ifdef CONFIG_PHL_P2PPS
	case MSG_EVT_NOA_UP:
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_cmd_general_post_phase_msg_hdlr: MSG_EVT_NOA_UP\n");
		psts = phl_noa_update(phl_info,
				(struct rtw_phl_noa_desc *)phl_cmd->buf);
	break;

	case MSG_EVT_NOA_DISABLE:
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_cmd_general_post_phase_msg_hdlr: MSG_EVT_NOA_DISABLE\n");
		psts = phl_cmd_noa_disable_hdl(phl_info, phl_cmd->buf);
	break;
#endif /* CONFIG_PHL_P2PPS */

	case MSG_EVT_CHG_OP_CH_DEF_START:
		psts = phl_cmd_chg_op_chdef_start_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_SWCH_START:
		psts = phl_cmd_set_ch_bw_hdl(phl_info, phl_cmd->buf);
	break;

	#if defined(CONFIG_PCI_HCI) && defined(PCIE_TRX_MIT_EN)
	case MSG_EVT_PCIE_TRX_MIT:
		psts = phl_evt_pcie_trx_mit_hdlr(phl_info, phl_cmd->buf);
	break;
	#endif
	case MSG_EVT_DBG_TX_DUMP:
	{
		struct hal_mac_dbg_dump_cfg cfg = {0};

		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "===> %s : MSG_EVT_DBG_TX_DUMP\n", __func__);
		RTW_DUMP_HAL_CR(phl_info, MSG_EVT_DBG_TX_DUMP, msg->band_idx);
		cfg.tx_flow_dbg = 1;
		rtw_hal_dbg_status_dump(phl_info->hal, &cfg);
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<=== %s : MSG_EVT_DBG_TX_DUMP\n", __func__);
		psts = RTW_PHL_STATUS_SUCCESS;
	}
	break;
	case MSG_EVT_DBG_RX_DUMP:
	{
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "===> %s : MSG_EVT_DBG_RX_DUMP\n", __func__);
		RTW_DUMP_HAL_CR(phl_info, MSG_EVT_DBG_RX_DUMP, msg->band_idx);
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<=== %s : MSG_EVT_DBG_RX_DUMP\n", __func__);
		psts = RTW_PHL_STATUS_SUCCESS;
	}
	break;
	case MSG_EVT_SW_WATCHDOG:
		if (IS_MSG_FAIL(msg->msg_id))
			psts = RTW_PHL_STATUS_FAILURE;
		else if (IS_MSG_CANCEL(msg->msg_id))
			psts = RTW_PHL_STATUS_FAILURE;
		else
			psts = RTW_PHL_STATUS_SUCCESS;
		psts = phl_watchdog_sw_cmd_hdl(phl_info, psts);
	break;
	case MSG_EVT_HW_WATCHDOG:
	{
		if (IS_MSG_CANNOT_IO(msg->msg_id))
			psts = RTW_PHL_STATUS_CANNOT_IO;
		else if (IS_MSG_FAIL(msg->msg_id))
			psts = RTW_PHL_STATUS_FAILURE;
		else if (IS_MSG_CANCEL(msg->msg_id))
			psts = RTW_PHL_STATUS_FAILURE;
		else
			psts = RTW_PHL_STATUS_SUCCESS;
		psts = phl_watchdog_hw_cmd_hdl(phl_info, psts);
	}
	break;

#if defined(CONFIG_USB_HCI)
	case MSG_EVT_FORCE_USB_SW:
		psts = phl_force_usb_switch(phl_info, *(u32*)(phl_cmd->buf));
	break;
	case MSG_EVT_GET_USB_SPEED:
		psts = phl_get_cur_usb_speed(phl_info, (u32*)(phl_cmd->buf));
	break;
	case MSG_EVT_GET_USB_SW_ABILITY:
		psts = phl_get_usb_support_ability(phl_info, (u32*)(phl_cmd->buf));
	break;
#endif
	case MSG_EVT_HWSEQ_GET_HW_SEQUENCE:
		psts = phl_cmd_get_hwseq_hdl(phl_info, phl_cmd->buf);
	break;
	case MSG_EVT_CFG_AMPDU:
		psts = phl_cmd_cfg_ampdu_hdl(phl_info, phl_cmd->buf);
	break;
	case MSG_EVT_CFG_AMSDU_TX:
		psts = phl_cmd_cfg_amsdu_tx_hdl(phl_info, phl_cmd->buf);
	break;
	case MSG_EVT_UPDT_EXT_TXPWR_LMT:
		psts = phl_cmd_updt_ext_txpwr_lmt(phl_info, phl_cmd->buf);
	break;
	case MSG_EVT_DFS_PAUSE_TX:
		psts = phl_cmd_dfs_tx_pause_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_ROLE_RECOVER:
		psts = phl_role_recover(phl_info);
	break;

	case MSG_EVT_ROLE_SUSPEND:
	{
		enum phl_role_susp_rsn *rsn = (enum phl_role_susp_rsn *)phl_cmd->buf;

		psts = phl_role_suspend(phl_info, *rsn);
	}
	break;

#if defined(CONFIG_PCI_HCI)
	case MSG_EVT_HAL_SET_L2_LEAVE:
		if (rtw_hal_set_l2_leave(phl_info->hal) == RTW_HAL_STATUS_SUCCESS)
			psts = RTW_PHL_STATUS_SUCCESS;
	break;
#endif

	case MSG_EVT_GET_TX_PWR_DBM:
	{
		if (IS_MSG_CANNOT_IO(msg->msg_id))
			psts = RTW_PHL_STATUS_CANNOT_IO;
		else if (IS_MSG_FAIL(msg->msg_id))
			psts = RTW_PHL_STATUS_FAILURE;
		else if (IS_MSG_CANCEL(msg->msg_id))
			psts = RTW_PHL_STATUS_FAILURE;
		else
			psts = rtw_phl_get_txinfo_pwr((void*)phl_info, (s16*)(phl_cmd->buf));
	}
	break;

	case MSG_EVT_NOTIFY_HAL:
		psts = phl_notify_cmd_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_ISSUE_BCN:
#ifdef RTW_PHL_BCN
		psts = phl_cmd_issue_bcn_hdl(phl_info, phl_cmd->buf);
#endif
	break;
	case MSG_EVT_STOP_BCN:
#ifdef RTW_PHL_BCN
		psts = phl_cmd_stop_bcn_hdl(phl_info, phl_cmd->buf);
#endif
	break;

	case MSG_EVT_SEC_KEY:
		psts = phl_cmd_set_key_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_ROLE_START:
		psts = phl_wifi_role_start_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_ROLE_CHANGE:
		psts = phl_wifi_role_chg_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_ROLE_STOP:
		psts = phl_wifi_role_stop_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_STA_INFO_CTRL:
		psts = phl_cmd_alloc_stainfo_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_STA_MEDIA_STATUS_UPT:
		psts = phl_update_media_status_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_CFG_CHINFO:
#ifdef CONFIG_PHL_CHANNEL_INFO
		psts = phl_cmd_cfg_chinfo_hdl(phl_info, phl_cmd->buf);
#endif
	break;

	case MSG_EVT_STA_CHG_STAINFO:
		psts = phl_cmd_change_stainfo_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_TPE_INFO_UPDATE:
		psts = phl_cmd_tpe_update_hdl(phl_info, phl_cmd->buf);
	break;

#ifdef CONFIG_PHL_TWT
	case MSG_EVT_TWT_STA_ACCEPT:
		psts = phl_twt_accept_for_sta_mode(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_TWT_STA_TEARDOWN:
		psts = phl_twt_teardown_for_sta_mode(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_TWT_GET_TWT:
		psts = phl_twt_get_target_wake_time(phl_info, phl_cmd->buf);
	break;
#endif

	case MSG_EVT_GET_CUR_TSF:
		psts = phl_cmd_get_cur_tsf_hdl(phl_info,
			(struct rtw_phl_port_tsf *)phl_cmd->buf);
	break;

	case MSG_EVT_SET_MACID_PAUSE:
		psts = phl_cmd_set_macid_pause_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_SET_MACID_PKT_DROP:
		psts = phl_cmd_set_macid_pkt_drop_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_GT3_SETUP:
		psts = phl_cmd_cfg_gt3_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_HW_SEQ_SETUP:
		psts = phl_cmd_cfg_hw_seq_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_LIFETIME_SETUP:
		psts = phl_cmd_cfg_lifetime_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_POWER_OFFSET_SETUP:
		psts = phl_cmd_cfg_power_offset_hdl(phl_info, phl_cmd->buf);
	break;

	case MSG_EVT_SET_UL_FIXINFO:
		psts = phl_set_fw_ul_fixinfo_hdl(phl_info,
			(struct rtw_phl_ax_ul_fixinfo *)phl_cmd->buf);
	break;

#ifdef CONFIG_PHL_DFS
	case MSG_EVT_DFS_RD_SETUP:
		psts = phl_cmd_dfs_rd_ctl_hdl(phl_info, phl_cmd->buf);
	break;
#endif

	case MSG_EVT_RX_DBG_CNT_GET_BY_IDX:
		psts = phl_cmd_get_rx_cnt_by_idx_hdl(phl_info, phl_cmd->buf);
		break;
	case MSG_EVT_RX_DBG_CNT_RESET:
		psts = phl_cmd_set_reset_rx_cnt_hdl(phl_info, phl_cmd->buf);
		break;
#ifdef CONFIG_PHL_USB_RX_AGGREGATION
	case MSG_EVT_USB_RX_AGG_CFG:
		psts = phl_cmd_usb_rx_agg_cfg_hdl(phl_info, phl_cmd->buf);
		break;
#endif
	case MSG_EVT_TXPWR_SETUP:
		psts = phl_cmd_txpwr_ctl_hdl(phl_info, phl_cmd->buf);
		break;

	case MSG_EVT_HW_CTS2SELF:
		psts = phl_cmd_cfg_hw_cts2self_hdl(phl_info, phl_cmd->buf);
		break;

	case MSG_EVT_SET_STA_SEC_IV:
		psts = phl_cmd_set_seciv_hdl(phl_info,
			phl_cmd->buf);
	break;

	default:
		psts = RTW_PHL_STATUS_SUCCESS;
	break;
	}

	return psts;
}

static enum phl_mdl_ret_code _phl_cmd_general_init(void *phl, void *dispr,
						   void **priv)
{
	*priv = phl;
	return MDL_RET_SUCCESS;
}

static void _phl_cmd_general_deinit(void *dispr, void *priv)
{

}

static enum phl_mdl_ret_code _phl_cmd_general_start(void *dispr, void *priv)
{
	u8 dispr_idx = 0;

	if (RTW_PHL_STATUS_SUCCESS != phl_dispr_get_idx(dispr, &dispr_idx))
		return MDL_RET_FAIL;

	#if defined(CONFIG_PCI_HCI) && defined(PCIE_TRX_MIT_EN)
	{
		struct phl_info_t *phl_info = (struct phl_info_t *)priv;

		if (RTW_PHL_STATUS_SUCCESS !=
		phl_pcie_trx_mit_start(phl_info, dispr_idx))
			return MDL_RET_FAIL;
	}
	#endif

	return MDL_RET_SUCCESS;
}

static void _stop_operation_on_general(void *phl)
{
	rtw_phl_watchdog_stop(phl);
}

static enum phl_mdl_ret_code _phl_cmd_general_stop(void *dispr, void *priv)
{
	_stop_operation_on_general(priv);
	return MDL_RET_SUCCESS;
}

static void _fail_evt_hdlr(void *dispr, void *priv, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	u16 evt_id = MSG_EVT_ID_FIELD(msg->msg_id);
	u8 idx = 0;
#ifdef CONFIG_POWER_SAVE
	struct phl_module_op_info op_info = {0};
#endif

	phl_dispr_get_idx(dispr, &idx);

	switch (evt_id) {
	case MSG_EVT_HW_WATCHDOG:
		/* watchdog do not need to handle fail case */
		PHL_DBG("%s do simple watchdog!\n", __func__);
		rtw_hal_simple_watchdog(phl_info->hal, false);
		break;
	default:
#ifdef CONFIG_POWER_SAVE
		op_info.op_code = PS_MDL_OP_CANCEL_PWR_REQ;
		op_info.inbuf = (u8 *)&evt_id;
		phl_disp_eng_set_bk_module_info(phl_info, idx,
				PHL_MDL_POWER_MGNT, &op_info);
#endif
		break;
	}
}

static enum phl_mdl_ret_code _phl_cmd_general_msg_hdlr(void *dispr, void *priv,
						       struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (IS_MSG_FAIL(msg->msg_id)) {

		PHL_INFO("%s :: MDL_ID(%d)_FAIL - MSG_EVT_ID=%d \n", __func__,
			 MSG_MDL_ID_FIELD(msg->msg_id),
			 MSG_EVT_ID_FIELD(msg->msg_id));

		_fail_evt_hdlr(dispr, priv, msg);

		return MDL_RET_FAIL;
	}

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_GENERAL)
		return MDL_RET_IGNORE;

	/*
	 * GENERAL is optional module, msg pass through mandatory module,
	 * optional module, and wifi role(protocol). So msg shall be handled
	 * at post phase to make sure that wifi role is at valid state
	 * if the msg is relative to wifi role(protocol)
	 */
	if (IS_MSG_IN_PRE_PHASE(msg->msg_id))
		status =
		    _phl_cmd_general_pre_phase_msg_hdlr(phl_info, dispr, msg);
	else
		status =
		    _phl_cmd_general_post_phase_msg_hdlr(phl_info, dispr, msg);

	if (status != RTW_PHL_STATUS_SUCCESS)
		return MDL_RET_FAIL;

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_phl_cmd_general_set_info(void *dispr, void *priv,
			  struct phl_module_op_info *info)
{
	return MDL_RET_SUCCESS;
}

enum rtw_phl_status
phl_cmd_get_cur_cmdinfo(struct phl_info_t *phl_info,
						u8 band_idx,
						struct phl_msg *msg,
						u8 **cmd,
						u32 *cmd_len)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_cmd_obj *phl_cmd = NULL;

	phl_cmd = (struct phl_cmd_obj *)msg->inbuf;
	if (NULL != phl_cmd) {
		*cmd = phl_cmd->buf;
		*cmd_len = phl_cmd->buf_len;
		psts = RTW_PHL_STATUS_SUCCESS;
	}

	return psts;
}

static enum phl_mdl_ret_code
_phl_cmd_general_query_info(void *dispr, void *priv,
			    struct phl_module_op_info *info)
{
	return MDL_RET_SUCCESS;
}

enum rtw_phl_status phl_register_cmd_general(struct phl_info_t *phl_info)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_cmd_dispatch_engine *disp_eng = &(phl_info->disp_eng);
	struct phl_bk_module_ops bk_ops;
	u8 band_idx = 0;

	bk_ops.init = _phl_cmd_general_init;
	bk_ops.deinit = _phl_cmd_general_deinit;
	bk_ops.start = _phl_cmd_general_start;
	bk_ops.stop = _phl_cmd_general_stop;
	bk_ops.msg_hdlr = _phl_cmd_general_msg_hdlr;
	bk_ops.set_info = _phl_cmd_general_set_info;
	bk_ops.query_info = _phl_cmd_general_query_info;

	for (band_idx = 0; band_idx < disp_eng->phy_num; band_idx++) {
		status = phl_disp_eng_register_module(phl_info, band_idx,
						 PHL_MDL_GENERAL, &bk_ops);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR(
			    "%s register MDL_GENRAL in cmd disp failed :%d\n",
			    __func__, band_idx + 1);
			break;
		}
	}

	return status;
}

static enum rtw_phl_status
_phl_cmd_obj_free(struct phl_info_t *phl_info, struct phl_cmd_obj *phl_cmd)
{
	void *drv = phl_to_drvpriv(phl_info);

	if(phl_cmd == NULL) {
		PHL_ERR("%s phl_cmd is NULL\n", __func__);
		_os_warn_on(1);
		return RTW_PHL_STATUS_FAILURE;
	}

	if (phl_cmd->is_cmd_wait == true)
		_phl_cmd_sync_deinit(phl_info, &phl_cmd->cmd_sync);
	_os_kmem_free(drv, phl_cmd, sizeof(struct phl_cmd_obj));
	phl_cmd = NULL;
	return RTW_PHL_STATUS_SUCCESS;
}

static void _phl_cmd_complete(void *priv, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	void *drv = phl_to_drvpriv(phl_info);
	struct phl_cmd_obj *phl_cmd = (struct phl_cmd_obj *)msg->inbuf;
	enum phl_cmd_sts csts = PHL_CMD_DONE_UNKNOWN;
	enum rtw_phl_status pstst = RTW_PHL_STATUS_SUCCESS;

	PHL_DBG("%s evt_id:%d\n", __func__, phl_cmd->evt_id);

	if (IS_MSG_CANNOT_IO(msg->msg_id))
		csts = PHL_CMD_DONE_CANNOT_IO;
	else if (IS_MSG_FAIL(msg->msg_id))
		csts = PHL_CMD_DONE_CMD_ERROR;
	else if (IS_MSG_CANCEL(msg->msg_id))
		csts = PHL_CMD_DONE_CMD_DROP;
	else
		csts = PHL_CMD_DONE_SUCCESS;

	if (phl_cmd->is_cmd_wait)
		_phl_cmd_done(phl_info, &phl_cmd->cmd_sync, csts);

	pstst = _cmd_stat_2_phl_stat(csts);
	if (phl_cmd->cmd_complete)
		phl_cmd->cmd_complete(drv, phl_cmd->buf, phl_cmd->buf_len, pstst);

	if (phl_cmd->is_cmd_wait) {
		#define PHL_MAX_SCHEDULE_TIMEOUT 100000
		u32 try_cnt = 0;
		u32 start = _os_get_cur_time_ms();

		do {
			if (_os_atomic_read(drv, &(phl_cmd->ref_cnt)) == 1)
				break;
			_os_sleep_ms(drv, 10);
			try_cnt++;
			if (try_cnt == 50)
				PHL_ERR("F-%s, L-%d polling is_cmd_wait to false\n",
							__FUNCTION__, __LINE__);
		} while (phl_get_passing_time_ms(start) < PHL_MAX_SCHEDULE_TIMEOUT);
	}

	_phl_cmd_obj_free(phl_info, phl_cmd);
}

enum rtw_phl_status
phl_cmd_enqueue(struct phl_info_t *phl_info,
                enum phl_band_idx band_idx,
                enum phl_msg_evt_id evt_id,
                u8 *cmd_buf,
                u32 cmd_len,
                void (*cmd_complete)(void* priv, u8 *buf, u32 buf_len, enum rtw_phl_status status),
                enum phl_cmd_type cmd_type,
                u32 cmd_timeout)
{
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_cmd_obj *phl_cmd = NULL;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	phl_cmd = _os_kmem_alloc(drv, sizeof(struct phl_cmd_obj));
	if (phl_cmd == NULL) {
		PHL_ERR("%s: alloc phl_cmd failed!\n", __func__);
		psts = RTW_PHL_STATUS_RESOURCE;
		goto _exit;
	}
	phl_cmd->evt_id = evt_id;
	phl_cmd->buf = cmd_buf;
	phl_cmd->buf_len = cmd_len;
	phl_cmd->cmd_complete = cmd_complete;

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_GENERAL);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, evt_id);
	msg.inbuf = (u8 *)phl_cmd;
	msg.inlen = sizeof(struct phl_cmd_obj);
	msg.band_idx = band_idx;
	attr.completion.completion = _phl_cmd_complete;
	attr.completion.priv = phl_info;

	if (cmd_type == PHL_CMD_WAIT) {
		phl_cmd->is_cmd_wait = true;
		_os_atomic_set(drv, &phl_cmd->ref_cnt, 0);
		_phl_cmd_sync_init(phl_info, evt_id, &phl_cmd->cmd_sync, cmd_timeout);
	}

	psts = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if (psts == RTW_PHL_STATUS_SUCCESS) {
		if (cmd_type == PHL_CMD_WAIT) {
			psts = _phl_cmd_wait(phl_info, &phl_cmd->cmd_sync);
			/*ref_cnt++ for cmd wait done*/
			_os_atomic_inc(drv, &phl_cmd->ref_cnt);
			/* Check wait cmd status */
			if (psts == RTW_PHL_STATUS_CMD_SUCCESS)
				psts = RTW_PHL_STATUS_SUCCESS;
		} else {
			psts = RTW_PHL_STATUS_SUCCESS;
		}
	} else {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s: evt_id(%d)\n", __func__, evt_id);
		_phl_cmd_obj_free(phl_info, phl_cmd);
	}

_exit:
	return psts;
}

enum rtw_phl_status
rtw_phl_cmd_enqueue(void *phl,
                    enum phl_band_idx band_idx,
                    enum phl_msg_evt_id evt_id,
                    u8 *cmd_buf,
                    u32 cmd_len,
                    void (*core_cmd_complete)(void *priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status),
                    enum phl_cmd_type cmd_type,
                    u32 cmd_timeout)
{
	return phl_cmd_enqueue((struct phl_info_t *)phl,
	                       band_idx,
	                       evt_id,
	                       cmd_buf,
	                       cmd_len,
	                       core_cmd_complete,
	                       cmd_type,
	                       cmd_timeout);
}

#endif /*CONFIG_CMD_DISP*/

