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
#define _PHL_CMD_DISPR_CONTROLLER_C_
#include "phl_headers.h"

#ifdef CONFIG_CMD_DISP

struct cmd_controller {
	struct phl_info_t *phl_info;
	u32 mon_mdl_id; /* monitor module id */
	bool mon_mdl;
};


/* Each dispr has a controller.
 * A dispr controller is designed for phl instance to interact with dispr modules that are belonged to a specific hw band,
 * phl instance can perform follwing actions via dedicated controller:
 * 1. allow (phl status/non-dispr phl modules) to monitor & drop msg
 * 2. allow dispr modules, that are belonged to same dispr, to sequentially communicate with phl instance & call phl api,
 *    and also allow (phl status/non-dispr phl modules) to notify dispr by hw band.
 * *Note*
 * 1. when cmd dispatch engine is in solo thread mode (each dispr has its own dedicated thread).
 *    phl instance might receive msg from different dispr simutaneously and
 *    currently using semaphore (dispr_ctrl_sema) to prevent multi-thread condition.
 * 2. when cmd dispatch engine is in share thread mode, msg from different dispr would pass to controller sequentially.

 * PS:
 * phl instance: means phl_info_t, which include phl mgnt status & non-dispr phl modules
 * dispr modules: all existing background & foreground modules.
 * non-dispr phl module : Data path (TX/Rx), etc
 * phl mgnt status : stop/surprise remove/cannot io
*/

enum phl_mdl_ret_code
_dispr_resume_io_ctrl(struct phl_info_t *phl_info,
                      struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	do {
		ctl.cmd = PHL_DATA_CTL_SW_TX_RESUME;
		ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
		sts = phl_data_ctrler(phl_info, &ctl, msg);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
			          "[DATA_CTRL] SW Tx paused by module(0x%x) resume fail by module(%d) with event(%d)\n",
			          phl_info->pause_tx_id, ctl.id,
			          MSG_EVT_ID_FIELD(msg->msg_id));
			ret = MDL_RET_FAIL;
			break;
		}

		ctl.cmd = PHL_DATA_CTL_SW_RX_RESUME;
		ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
		sts = phl_data_ctrler(phl_info, &ctl, msg);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
			          "[DATA_CTRL] SW Rx paused by module(0x%x) resume fail by module(%d) with event(%d)\n",
			          phl_info->pause_rx_id, ctl.id,
			          MSG_EVT_ID_FIELD(msg->msg_id));
			ret = MDL_RET_FAIL;
			break;
		}

		ret = MDL_RET_SUCCESS;
	} while (false);

	return ret;
}


enum phl_mdl_ret_code
_dispr_cannot_io_ctrl(struct phl_info_t *phl_info,
                      struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_TRX_SW_PAUSE;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW T/Rx pause fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_hw_trx_rst_resume_ctrl(struct phl_info_t *phl_info,
                              struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_HW_TRX_RST_RESUME;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] HW T/Rx reset and resume fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_hw_trx_pause_ctrl(struct phl_info_t *phl_info,
                         struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_HW_TRX_PAUSE;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] HW T/Rx pause fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_sw_tx_resume_ctrl(struct phl_info_t *phl_info,
                         struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};
	enum phl_module_id mdl_id = MSG_MDL_ID_FIELD(msg->msg_id);

	/* tx pause by MDL_POWER_MGNT, tx resume by MDL_TX/MDL_RX */
	if (mdl_id == PHL_MDL_TX || mdl_id == PHL_MDL_RX)
		ctl.id = PHL_MDL_POWER_MGNT;
	else
		ctl.id = mdl_id;
	ctl.cmd = PHL_DATA_CTL_SW_TX_RESUME;

	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW Tx paused by module(0x%x) resume fail by module(%d) with event(%d)\n",
		          phl_info->pause_tx_id, ctl.id,
		          MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_sw_rx_resume_ctrl(struct phl_info_t *phl_info,
                         struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};
	enum phl_module_id mdl_id = MSG_MDL_ID_FIELD(msg->msg_id);

	/* rx pause by MDL_POWER_MGNT, rx resume by MDL_TX/MDL_RX */
	if (mdl_id == PHL_MDL_TX || mdl_id == PHL_MDL_RX)
		ctl.id = PHL_MDL_POWER_MGNT;
	else
		ctl.id = mdl_id;
	ctl.cmd = PHL_DATA_CTL_SW_RX_RESUME;

	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW Rx paused by module(0x%x) resume fail by module(%d) with event(%d)\n",
		          phl_info->pause_rx_id, ctl.id,
		          MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_sw_tx_pause_ctrl(struct phl_info_t *phl_info,
                        struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_SW_TX_PAUSE;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW Tx pause fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_sw_rx_pause_ctrl(struct phl_info_t *phl_info,
                        struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_SW_RX_PAUSE;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW Rx pause fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_sw_tx_reset_ctrl(struct phl_info_t *phl_info,
                        struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_SW_TX_RESET;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW Tx reset fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_sw_rx_reset_ctrl(struct phl_info_t *phl_info,
                        struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_SW_RX_RESET;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW Rx reset fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_trx_sw_pause_ctrl(struct phl_info_t *phl_info,
                         struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_TRX_SW_PAUSE;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW T/Rx pause fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_trx_sw_resume_ctrl(struct phl_info_t *phl_info,
                          struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_TRX_SW_RESUME;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW T/Rx resume fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_trx_pause_w_rst_ctrl(struct phl_info_t *phl_info,
                            struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_TRX_PAUSE_W_RST;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW T/Rx pause with reset fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_trx_resume_w_rst_ctrl(struct phl_info_t *phl_info,
                             struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.cmd = PHL_DATA_CTL_TRX_RESUME_W_RST;
	ctl.id = MSG_MDL_ID_FIELD(msg->msg_id);
	sts = phl_data_ctrler(phl_info, &ctl, msg);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "[DATA_CTRL] SW T/Rx resume with reset fail by module(%d) with event(%d)\n",
		          ctl.id, MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

enum phl_mdl_ret_code
_dispr_mdl_stop_ctrl(struct phl_info_t *phl_info,
	struct cmd_controller *cmd_ctrl)
{
	if (cmd_ctrl->mon_mdl_id != 0) {
		cmd_ctrl->mon_mdl = true;
		return MDL_RET_PENDING;
	} else {
		return MDL_RET_SUCCESS;
	}
}

enum phl_mdl_ret_code
_dispr_ctrl_init(void *phl,
                 void *dispr,
                 void **priv)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct cmd_controller *cmd_ctrl = NULL;
	void *drv = phl_to_drvpriv(phl_info);

	PHL_INFO("%s(): \n", __func__);

	(*priv) = NULL;
	cmd_ctrl = (struct cmd_controller *)_os_mem_alloc(drv,
		sizeof(struct cmd_controller));

	if (cmd_ctrl == NULL) {
		PHL_ERR(" %s, alloc fail\n",__FUNCTION__);
		return MDL_RET_FAIL;
	}

	cmd_ctrl->phl_info = phl_info;
	(*priv) = (void*)cmd_ctrl;
	return MDL_RET_SUCCESS;
}

void _dispr_ctrl_deinit(void *dispr, void *priv)
{
	struct cmd_controller *cmd_ctrl = (struct cmd_controller *)priv;
	void *drv = phl_to_drvpriv(cmd_ctrl->phl_info);

	_os_mem_free(drv, cmd_ctrl, sizeof(struct cmd_controller));

	PHL_INFO("%s(): \n", __func__);
}

enum phl_mdl_ret_code _dispr_ctrl_start(void *dispr, void *priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;
	struct cmd_controller *cmd_ctrl = (struct cmd_controller *)priv;
	struct phl_info_t *phl_info = (struct phl_info_t *)cmd_ctrl->phl_info;
	void *drv = phl_to_drvpriv(cmd_ctrl->phl_info);

	PHL_INFO("%s(): \n", __func__);

	_os_mem_set(drv, cmd_ctrl, 0, sizeof(struct cmd_controller));
	cmd_ctrl->phl_info = phl_info;

	return ret;
}

enum phl_mdl_ret_code _dispr_ctrl_stop(void *dispr, void *priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;

	PHL_INFO("%s(): \n", __func__);

	return ret;
}

static void _fail_hdlr(void *dispr, struct phl_info_t *phl_info, struct phl_msg *msg)
{

}

static enum phl_mdl_ret_code
_ctrler_msg_hdlr(struct phl_info_t *phl_info, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_DEV_RESUME_IO:
		ret = _dispr_resume_io_ctrl(phl_info, msg);
		break;
	case MSG_EVT_DEV_CANNOT_IO:
		ret = _dispr_cannot_io_ctrl(phl_info, msg);
		break;
	default:
		break;
	}

	return ret;
}

static enum phl_mdl_ret_code
_tx_msg_hdlr(struct phl_info_t *phl_info, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_TRX_PWR_REQ:
		/* ps module handle MSG_EVT_TRX_PWR_REQ itself */
	default:
		break;
	}

	return ret;
}

static enum phl_mdl_ret_code
_rx_msg_hdlr(struct phl_info_t *phl_info, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_TRX_PWR_REQ:
		/* ps module handle MSG_EVT_TRX_PWR_REQ itself */
	default:
		break;
	}

	return ret;
}

static void _mon_msg_hdlr(struct cmd_controller *cmd_ctrl,
	struct phl_msg *msg)
{
	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_SER_L0_RESET:
	case MSG_EVT_SER_M1_PAUSE_TRX:
	case MSG_EVT_SER_M3_DO_RECOV:
		cmd_ctrl->mon_mdl_id = PHL_MDL_SER;
		break;
	case MSG_EVT_SER_M5_READY:
		cmd_ctrl->mon_mdl_id = 0;
		if (cmd_ctrl->mon_mdl)
			phl_disp_eng_clr_pending_msg(cmd_ctrl->phl_info, msg->band_idx);
		cmd_ctrl->mon_mdl = false;
		break;
	}
}

static enum phl_mdl_ret_code _internal_msg_hdlr(void *dispr,
	struct cmd_controller *cmd_ctrl, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	struct phl_info_t *phl_info = cmd_ctrl->phl_info;

	/* sender is controller itself */
	if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_PHY_MGNT) {
		ret = _ctrler_msg_hdlr(phl_info, msg);
	} else if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_TX) {
		ret = _tx_msg_hdlr(phl_info, msg);
	} else if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_RX) {
		ret = _rx_msg_hdlr(phl_info, msg);
	}

	return ret;
}

static enum phl_mdl_ret_code
_external_msg_hdlr(void *dispr,
	struct cmd_controller *cmd_ctrl, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = cmd_ctrl->phl_info;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;

	if (phl_com->dev_state & RTW_DEV_RESUMING) {
		PHL_WARN("Controller: MDL_ID(%d)-EVT_ID(%d) is sent during RTW_DEV_RESUMING!\n",
		         MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));
	}

	if (phl_com->dev_state & RTW_DEV_SURPRISE_REMOVAL) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "Controller: msg fail due to invalid device state\n");
		return MDL_RET_FAIL;
	}

	_mon_msg_hdlr(cmd_ctrl, msg);

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_HW_TRX_RST_RESUME:
		ret = _dispr_hw_trx_rst_resume_ctrl(phl_info, msg);
		break;
	case MSG_EVT_HW_TRX_PAUSE:
		ret = _dispr_hw_trx_pause_ctrl(phl_info, msg);
		break;
	case MSG_EVT_SW_TX_RESUME:
		ret = _dispr_sw_tx_resume_ctrl(phl_info, msg);
		break;
	case MSG_EVT_SW_RX_RESUME:
		ret = _dispr_sw_rx_resume_ctrl(phl_info, msg);
		break;
	case MSG_EVT_SW_TX_PAUSE:
		ret = _dispr_sw_tx_pause_ctrl(phl_info, msg);
		break;
	case MSG_EVT_SW_RX_PAUSE:
		ret = _dispr_sw_rx_pause_ctrl(phl_info, msg);
		break;
	case MSG_EVT_SW_TX_RESET:
		ret = _dispr_sw_tx_reset_ctrl(phl_info, msg);
		break;
	case MSG_EVT_SW_RX_RESET:
		ret = _dispr_sw_rx_reset_ctrl(phl_info, msg);
		break;
	case MSG_EVT_TRX_SW_PAUSE:
		ret = _dispr_trx_sw_pause_ctrl(phl_info, msg);
		break;
	case MSG_EVT_TRX_SW_RESUME:
		ret = _dispr_trx_sw_resume_ctrl(phl_info, msg);
		break;
	case MSG_EVT_TRX_PAUSE_W_RST:
		ret = _dispr_trx_pause_w_rst_ctrl(phl_info, msg);
		break;
	case MSG_EVT_TRX_RESUME_W_RST:
		ret = _dispr_trx_resume_w_rst_ctrl(phl_info, msg);
		break;
	case MSG_EVT_MDL_CHECK_STOP:
		ret = _dispr_mdl_stop_ctrl(phl_info, cmd_ctrl);
		break;
	default:
		break;
	}

	return ret;
}
enum phl_mdl_ret_code
_dispr_ctrl_msg_hdlr(void *dispr,
                     void *priv,
                     struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	struct cmd_controller *cmd_ctrl = (struct cmd_controller *)priv;

	FUNCIN();
	if (IS_MSG_FAIL(msg->msg_id)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
		          "%s: cmd dispatcher notify cmd failure: 0x%x.\n",
		          __FUNCTION__, msg->msg_id);
		_fail_hdlr(dispr, cmd_ctrl->phl_info, msg);
		FUNCOUT();
		return MDL_RET_FAIL;
	}

	if (IS_DISPR_CTRL(MSG_MDL_ID_FIELD(msg->msg_id)))
		ret = _internal_msg_hdlr(dispr, cmd_ctrl, msg);
	else
		ret = _external_msg_hdlr(dispr, cmd_ctrl, msg);
	FUNCOUT();
	return ret;
}

enum phl_mdl_ret_code
_dispr_ctrl_set_info(void *dispr,
                     void *priv,
                     struct phl_module_op_info *info)
{
	PHL_INFO("%s(): \n", __func__);

	return MDL_RET_IGNORE;
}

enum phl_mdl_ret_code
_dispr_ctrl_query_info(void *dispr, void *priv,
			   struct phl_module_op_info *info)
{
	PHL_INFO("%s(): \n", __func__);

	return MDL_RET_IGNORE;
}

void dispr_ctrl_hook_ops(void *dispr, struct phl_bk_module_ops *ops)
{
	if (ops == NULL)
		return;
	ops->init 	= _dispr_ctrl_init;
	ops->deinit 	= _dispr_ctrl_deinit;
	ops->start 	= _dispr_ctrl_start;
	ops->stop 	= _dispr_ctrl_stop;
	ops->msg_hdlr 	= _dispr_ctrl_msg_hdlr;
	ops->set_info 	= _dispr_ctrl_set_info;
	ops->query_info = _dispr_ctrl_query_info;
}
#endif

