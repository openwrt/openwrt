
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
#define _PHL_CMD_SER_C_
#include "phl_headers.h"
#include "phl_api.h"

#define CMD_SER_L0 0x00000001
#define CMD_SER_L1 0x00000002
#define CMD_SER_L2 0x00000004

enum _CMD_SER_EVENT_SOURCE {
	CMD_SER_SRC_UNKNOWN = 0,
	CMD_SER_SRC_INT = BIT0, // ser event from interrupt
	CMD_SER_SRC_POLL = BIT1, // ser event by polling io
	CMD_SER_SRC_INT_NOTIFY = BIT2, // ser event from interrupt and ser state from mac
};

enum _CMD_SER_TIMER_STATE {
	CMD_SER_NOT_OCCUR = 0,
	CMD_SER_M1 = BIT0, //POLL_IO
	CMD_SER_M2 = BIT1, //POLL_FW
	CMD_SER_M3 = BIT2,
	CMD_SER_M4 = BIT3,
	CMD_SER_M5 = BIT4,
	CMD_SER_M9 = BIT5,
};

#define CMD_SER_FW_TIMEOUT 1000 /* ms */
#define CMD_SER_POLLING_INTERVAL 10 /* ms */
#define CMD_SER_USB_POLLING_INTERVAL_IDL 1000 /* ms */
#define CMD_SER_USB_POLLING_INTERVAL_ACT 10 /* ms */

#define CMD_SER_POLL_IO_TIMES 200
#define CMD_SER_USB_POLL_IO_TIMES 300

#define CMD_SER_LOG_SIZE 10

struct sts_l2 {
	_os_list list;
	u8 idx;
	u8 ser_log;
};

struct cmd_ser {
	struct phl_info_t *phl_info;
	void* dispr;
	u8 state;
	_os_lock _lock;

	u8 evtsrc;
	int poll_cnt;
	_os_timer poll_timer;

	/* L2 log :
	//    If L2 triggered, set ser_log = state-of-cmd_ser
	*/
	struct phl_queue stslist;
	struct sts_l2 stsl2[CMD_SER_LOG_SIZE];
	u8 bserl2;
	u8 (*ser_l2_hdlr)(void *drv);
};

static void _ser_int_ntfy_ctrl(struct phl_info_t *phl_info,
                               enum rtw_phl_config_int int_type)
{
	if (RTW_PHL_CLR_HCI_INT == int_type)
		rtw_hal_clear_interrupt(phl_info->hal);
	else
		rtw_hal_config_interrupt(phl_info->hal, int_type);
}

static enum rtw_phl_status
_ser_event_notify(void *phl, u8 *p_ntfy)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum RTW_PHL_SER_NOTIFY_EVENT notify = RTW_PHL_SER_L2_RESET;
	struct phl_msg msg = {0};
	u32 err = 0;

	notify = rtw_hal_ser_get_error_status(phl_info->hal, &err);

	if (p_ntfy != NULL)
		*p_ntfy = notify;

	phl_info->phl_com->phl_stats.ser_event[notify]++;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "_ser_event_notify, error 0x%x, notify 0x%x\n", err, notify);

	if (notify == RTW_PHL_SER_L0_RESET) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "_ser_event_notify, hit L0 Reset\n");
		return RTW_PHL_STATUS_SUCCESS;
	}

	if (notify == RTW_PHL_SER_LOG_ONLY) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "_ser_event_notify, RTW_PHL_SER_LOG_ONLY\n");
		return RTW_PHL_STATUS_SUCCESS;
	}

	if (notify == RTW_PHL_SER_DUMP_FW_LOG) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "_ser_event_notify, RTW_PHL_SER_DUMP_FW_LOG\n");

		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_PHY_MGNT);
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_DUMP_PLE_BUFFER);
		phl_msg_hub_send(phl_info, NULL, &msg);

		return RTW_PHL_STATUS_SUCCESS;
	}

	return phl_ser_send_msg(phl, notify);
}

void _ser_dump_stsl2(struct cmd_ser *cser)
{
	u8 idx =0;

	for (idx = 0; idx < CMD_SER_LOG_SIZE; idx++) {
		if (cser->stsl2[idx].ser_log || cser->stsl2[idx].idx >= CMD_SER_LOG_SIZE) {
			PHL_ERR("%s :: [%d] %d - ser_log = 0x%X \n", __func__,
			        idx, cser->stsl2[idx].idx, cser->stsl2[idx].ser_log);
		}
	}
}

void _ser_reset_status(struct cmd_ser *cser)
{
	void *drv = phl_to_drvpriv(cser->phl_info);

	_os_spinlock(drv, &cser->_lock, _bh, NULL);
	cser->state = CMD_SER_NOT_OCCUR;
	_os_spinunlock(drv, &cser->_lock, _bh, NULL);

	if (CMD_SER_SRC_POLL == cser->evtsrc) {
		cser->poll_cnt = 0;
		_os_set_timer(drv,
		              &cser->poll_timer,
		              CMD_SER_USB_POLLING_INTERVAL_IDL);
	} else if (CMD_SER_SRC_INT_NOTIFY == cser->evtsrc) {
		_ser_int_ntfy_ctrl(cser->phl_info, RTW_PHL_EN_HCI_INT);
	}
}

void _ser_set_status(struct cmd_ser *cser, u8 serstatus)
{
	void *drv = phl_to_drvpriv(cser->phl_info);

	_os_spinlock(drv, &cser->_lock, _bh, NULL);
	cser->state |= serstatus;
	_os_spinunlock(drv, &cser->_lock, _bh, NULL);
}

void _ser_clear_status(struct cmd_ser *cser, u8 serstatus)
{
	void *drv = phl_to_drvpriv(cser->phl_info);

	_os_spinlock(drv, &cser->_lock, _bh, NULL);
	cser->state &= ~(serstatus);
	_os_spinunlock(drv, &cser->_lock, _bh, NULL);
}

static void _ser_l1_notify(struct cmd_ser *cser)
{
	struct phl_msg nextmsg = {0};

	SET_MSG_MDL_ID_FIELD(nextmsg.msg_id, PHL_MDL_SER);
	SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SER_L1);
	phl_msg_hub_send(cser->phl_info, NULL, &nextmsg);
}

static void _ser_l2_notify(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	void *drv = phl_to_drvpriv(phl_info);
	struct sts_l2 *stsl2 = NULL;
	struct phl_msg nextmsg = {0};
	_os_list* obj = NULL;

	rtw_hal_ser_reset_wdt_intr(phl_info->hal);

	if (pq_pop(drv, &cser->stslist, &obj, _first, _bh)) {
		stsl2 = (struct sts_l2*)obj;

		/* Rotate stslist : 0~ (CMD_SER_LOG_SIZE-1) are unused index */
		stsl2->idx+= CMD_SER_LOG_SIZE;
		stsl2->ser_log = cser->state;
		pq_push(drv, &cser->stslist, &stsl2->list, _tail, _bh);
	}
	_ser_dump_stsl2(cser);

	/* L2 can't be rescued, bserl2 wouldn't reset. */
	/* comment out: wait for new ser flow to handle L2 */
	cser->bserl2 = true;

	if (cser->ser_l2_hdlr)
		cser->ser_l2_hdlr(phl_to_drvpriv(phl_info));

	phl_disp_eng_clr_pending_msg(cser->phl_info, HW_BAND_0);
	phl_disp_eng_clr_pending_msg(cser->phl_info, HW_BAND_1);

	SET_MSG_MDL_ID_FIELD(nextmsg.msg_id, PHL_MDL_SER);
	SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SER_L2);
	phl_msg_hub_send(cser->phl_info, NULL, &nextmsg);
}

static void _ser_m2_notify(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	if (CMD_SER_SRC_INT_NOTIFY == cser->evtsrc)
		_ser_int_ntfy_ctrl(phl_info, RTW_PHL_SER_HANDSHAKE_MODE);

	_ser_clear_status(cser, CMD_SER_M1);
	_ser_set_status(cser, CMD_SER_M2);

	/* send M2 event to fw */
	status = rtw_hal_ser_set_error_status(phl_info->hal, RTW_PHL_SER_L1_DISABLE_EN);
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "_ser_m2_notify:: RTW_PHL_SER_L1_DISABLE_EN, status 0x%x\n", status);
}

static void _ser_m3_m5_waiting(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	void *drv = phl_to_drvpriv(phl_info);
	int poll_cnt = 0, intvl = CMD_SER_FW_TIMEOUT;

	if (cser->evtsrc == CMD_SER_SRC_POLL) {
		/* CMD_SER_POLLING_INTERVAL = CMD_SER_FW_TIMEOUT/ CMD_SER_USB_POLLING_INTERVAL_ACT */
		poll_cnt = CMD_SER_POLLING_INTERVAL;
		intvl = CMD_SER_USB_POLLING_INTERVAL_ACT;
	}

	cser->poll_cnt = poll_cnt;
	/* wait M3 or M5 */
	_os_set_timer(drv, &cser->poll_timer, intvl);
}

static void _ser_m4_notify(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	if (CMD_SER_SRC_INT_NOTIFY == cser->evtsrc)
		_ser_int_ntfy_ctrl(phl_info, RTW_PHL_SER_HANDSHAKE_MODE);

	_ser_clear_status(cser, CMD_SER_M3);
	_ser_set_status(cser, CMD_SER_M4);

	/* send M4 event */
	status = rtw_hal_ser_set_error_status(phl_info->hal, RTW_PHL_SER_L1_RCVY_EN);
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "_ser_m4_notify:: RTW_PHL_SER_L1_RCVY_EN, status 0x%x\n", status);
}

static void _ser_poll_timer_cb(void *priv)
{
	struct cmd_ser *cser = (struct cmd_ser *)priv;
	struct phl_msg nextmsg = {0};
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;

	SET_MSG_MDL_ID_FIELD(nextmsg.msg_id, PHL_MDL_SER);

	if (TEST_STATUS_FLAG(cser->state, CMD_SER_M1)) {
		SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SER_IO_TIMER_EXPIRE);
	} else if (TEST_STATUS_FLAG(cser->state, CMD_SER_M2)) {
		if (cser->poll_cnt > 0) /* polling mode */
			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SER_POLLING_CHK);
		else
			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SER_FW_TIMER_EXPIRE);
	} else if (TEST_STATUS_FLAG(cser->state, CMD_SER_M4)) {
		if (cser->poll_cnt > 0) /* polling mode */
			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SER_POLLING_CHK);
		else
			SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SER_FW_TIMER_EXPIRE);
	} else {
		SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, MSG_EVT_SER_POLLING_CHK);
	}

	nextmsg.band_idx = HW_BAND_0;
	nextmsg.rsvd[0].value = cser->state;

	if (MSG_EVT_ID_FIELD(nextmsg.msg_id)) {
		PHL_DBG("%s :: nextmsg->msg_id= 0x%X\n", __func__, MSG_EVT_ID_FIELD(nextmsg.msg_id));
		pstatus = phl_disp_eng_send_msg(cser->phl_info, &nextmsg, &attr, NULL);
		if (pstatus != RTW_PHL_STATUS_SUCCESS)
			PHL_ERR("%s :: [SER_TIMER_CB] dispr_send_msg failed\n", __func__);
	}
}

static void _ser_m1_pause_trx(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.id = PHL_MDL_SER;

	ctl.cmd = PHL_DATA_CTL_SW_TX_PAUSE;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): pause sw tx failure\n", __func__);
		goto err;
	}

	ctl.cmd = PHL_DATA_CTL_HW_TRX_PAUSE;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): pause hw trx failure\n", __func__);
		goto err;
	}

	ctl.cmd = PHL_DATA_CTL_SW_RX_PAUSE;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
#ifdef RTW_WKARD_SER_L1_EXPIRE
	if (RTW_PHL_STATUS_SUCCESS != sts && RTW_PHL_STATUS_CMD_TIMEOUT != sts) {
		PHL_WARN("%s(): pause sw rx failure\n", __func__);
		goto err;
	}
#else
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): pause sw rx failure\n", __func__);
		goto err;
	}
#endif

	ctl.cmd = PHL_DATA_CTL_SW_TX_RESET;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): reset sw tx failure\n", __func__);
		goto err;
	}

	ctl.cmd = PHL_DATA_CTL_SW_RX_RESET;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): reset sw rx failure\n", __func__);
		goto err;
	}

	_ser_m2_notify(cser);
	_ser_m3_m5_waiting(cser);

	return;
err:
	_ser_l2_notify(cser);
	_ser_reset_status(cser);

	return;
}

static void _ser_m9_pause_trx(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.id = PHL_MDL_SER;

	ctl.cmd = PHL_DATA_CTL_SW_TX_PAUSE;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): pause sw tx failure\n", __func__);
	}

	ctl.cmd = PHL_DATA_CTL_SW_RX_PAUSE;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
#ifdef RTW_WKARD_SER_L1_EXPIRE
	if (RTW_PHL_STATUS_SUCCESS != sts && RTW_PHL_STATUS_CMD_TIMEOUT != sts) {
		PHL_WARN("%s(): pause sw rx failure\n", __func__);
	}
#else
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): pause sw rx failure\n", __func__);
	}
#endif

	ctl.cmd = PHL_DATA_CTL_SW_TX_RESET;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): reset sw tx failure\n", __func__);
	}

	ctl.cmd = PHL_DATA_CTL_SW_RX_RESET;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): reset sw rx failure\n", __func__);
	}

	return;
}


static void _ser_m3_reset_hw_trx(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	ctl.id = PHL_MDL_SER;

	ctl.cmd = PHL_DATA_CTL_HW_TRX_RST_RESUME;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_WARN("%s(): resume hw trx failure\n", __func__);
		goto err;
	}

	_ser_m4_notify(cser);
	_ser_m3_m5_waiting(cser);

	return;
err:
	_ser_l2_notify(cser);
	_ser_reset_status(cser);

	return;
}

enum phl_mdl_ret_code
_ser_fail_ev_hdlr(void *dispr, void *priv, struct phl_msg *msg)
{
	struct cmd_ser *cser = (struct cmd_ser *)priv;
	struct phl_info_t *phl_info = cser->phl_info;
	void *drv = phl_to_drvpriv(phl_info);

	PHL_INFO("%s :: [MSG_FAIL] MDL =%d , EVT_ID=%d\n", __func__,
	         MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));

	if (cser->evtsrc != CMD_SER_SRC_POLL)
		return MDL_RET_SUCCESS;

	_os_set_timer(drv, &cser->poll_timer, CMD_SER_USB_POLLING_INTERVAL_IDL);

	return MDL_RET_SUCCESS;
}

enum phl_mdl_ret_code
_ser_hdl_external_evt(void *dispr, void *priv, struct phl_msg *msg)
{
	struct cmd_ser *cser = (struct cmd_ser *)priv;

	/**
	 * 1. SER inprogress: pending msg from others module
	 * 2. SER recovery fail: clr pending event from MDL_SER & msg return failed from others module
	 * 3. SER recovery done: clr pending event & msg return ignor from others module
	 * 4. SER NOT OCCUR: MDL_RET_IGNORE
	 */
	if (cser->bserl2) {
		/* allow MSG_EVT_DBG_L2_DIAGNOSE when ser L2 occured */
		if (MSG_EVT_ID_FIELD(msg->msg_id) == MSG_EVT_DBG_L2_DIAGNOSE)
			return MDL_RET_IGNORE;
		PHL_ERR("%s: L2 Occured!! From others MDL=%d, EVT_ID=%d\n", __func__,
		MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));
		return MDL_RET_FAIL;
	} else if (cser->state) { /* non-CMD_SER_NOT_OCCUR */
		PHL_WARN("%s: Within SER!! From others MDL=%d, EVT_ID=%d\n", __func__,
		MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));
		return MDL_RET_PENDING;
	}

	return MDL_RET_IGNORE;
}

static void _ser_msg_hdl_polling_chk(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	void *drv = phl_to_drvpriv(phl_info);

	if (CMD_SER_SRC_POLL != cser->evtsrc)
		return;

	if (true == rtw_hal_recognize_interrupt(phl_info->hal)) {
		rtw_phl_interrupt_handler(phl_info);
	} else {
		if (cser->poll_cnt > 0) {
			/* wait for m3/m5, polling 10*10 */
			cser->poll_cnt--;
			_os_set_timer(drv, &cser->poll_timer, CMD_SER_USB_POLLING_INTERVAL_ACT); /* 10ms */
		} else {
			/* no ser occur, set next polling timer */
			_os_set_timer(drv, &cser->poll_timer, CMD_SER_USB_POLLING_INTERVAL_IDL); /* 1000ms */
		}
	}
}

static enum rtw_phl_status
_ser_msg_hdl_evt_chk(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

	if (CMD_SER_SRC_INT == cser->evtsrc) {
		psts = _ser_event_notify(phl_info, NULL);
	} else if (CMD_SER_SRC_POLL == cser->evtsrc) {
		psts = _ser_event_notify(phl_info, NULL);
	} else if (CMD_SER_SRC_INT_NOTIFY == cser->evtsrc) {
		u8 notify = RTW_PHL_SER_L0_RESET;

		/* disable interrupt notification and read ser value */
		if (true == rtw_hal_recognize_halt_c2h_interrupt(phl_info->hal)) {
			psts = _ser_event_notify(phl_info, &notify);

			if ((notify == RTW_PHL_SER_L0_RESET) || (notify == RTW_PHL_SER_L2_RESET))
				_ser_int_ntfy_ctrl(phl_info, RTW_PHL_EN_HCI_INT);
		} else {
			_ser_int_ntfy_ctrl(phl_info, RTW_PHL_EN_HCI_INT);
		}
	}

	return psts;
}

static void _ser_msg_hdl_m1(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	void *drv = phl_to_drvpriv(phl_info);

	if (CMD_SER_SRC_POLL == cser->evtsrc)
		_os_cancel_timer(drv, &cser->poll_timer);
	else if (CMD_SER_SRC_INT_NOTIFY == cser->evtsrc)
		_ser_int_ntfy_ctrl(phl_info, RTW_PHL_DIS_HCI_INT);

	_ser_set_status(cser, CMD_SER_M1);
	_ser_l1_notify(cser);

	rtw_hal_ser_int_cfg(phl_info->hal, phl_info->phl_com, RTW_PHL_SER_M1_PRE_CFG);

	_ser_m1_pause_trx(cser);

	rtw_hal_ser_int_cfg(phl_info->hal, phl_info->phl_com, RTW_PHL_SER_M1_POST_CFG);
}

static void _ser_msg_hdl_fw_expire(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	void *drv = phl_to_drvpriv(phl_info);

	if (CMD_SER_SRC_POLL == cser->evtsrc) {
		_ser_set_status(cser, CMD_SER_M9);

		_os_set_timer(drv,
		              &cser->poll_timer,
		              CMD_SER_USB_POLLING_INTERVAL_IDL);
	}

	cser->bserl2 = true;
}

static void _ser_msg_hdl_m3(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	void *drv = phl_to_drvpriv(phl_info);

	if (CMD_SER_SRC_INT_NOTIFY == cser->evtsrc)
		_ser_int_ntfy_ctrl(phl_info, RTW_PHL_CLR_HCI_INT);

	_os_cancel_timer(drv, &cser->poll_timer);
	_ser_clear_status(cser, CMD_SER_M2);
	_ser_set_status(cser, CMD_SER_M3);

	_ser_m3_reset_hw_trx(cser);
}

static void _ser_msg_hdl_m5(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};

	_os_cancel_timer(drv, &cser->poll_timer);
	_ser_clear_status(cser, CMD_SER_M4);
	_ser_set_status(cser, CMD_SER_M5);

	ctl.id = PHL_MDL_SER;

	ctl.cmd = PHL_DATA_CTL_SW_RX_RESUME;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts)
		PHL_WARN("%s(): resume sw rx failure\n", __func__);

	ctl.cmd = PHL_DATA_CTL_SW_TX_RESUME;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts)
		PHL_WARN("%s(): resume sw tx failure\n", __func__);

	rtw_hal_ser_int_cfg(phl_info->hal, phl_info->phl_com, RTW_PHL_SER_M5_CFG);

	_ser_reset_status(cser);

	phl_disp_eng_clr_pending_msg(cser->phl_info, HW_BAND_0);
	phl_disp_eng_clr_pending_msg(cser->phl_info, HW_BAND_1);
}

static void _ser_msg_hdl_m9(struct cmd_ser *cser)
{
	struct phl_info_t *phl_info = cser->phl_info;
	void *drv = phl_to_drvpriv(phl_info);

	/* dump FW status for debug*/
	rtw_hal_fw_dbg_dump(phl_info->hal);

	_os_cancel_timer(drv, &cser->poll_timer);
	_ser_set_status(cser, CMD_SER_M9);

	_ser_m9_pause_trx(cser);

	_ser_l2_notify(cser);
	_ser_reset_status(cser);
}

static void _ser_msg_hdl_l2_reset_done(struct cmd_ser *cser)
{
	/* reset bserl2 to false after L2 done */
	if (cser->bserl2 == true)
		cser->bserl2 = false;
	else
		PHL_WARN("Ser L2 state not set!\n");
}

enum phl_mdl_ret_code
_ser_hdl_internal_evt(void *dispr, void *priv, struct phl_msg *msg)
{
	struct cmd_ser *cser = (struct cmd_ser *)priv;
	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_SER_POLLING_CHK:
		PHL_DBG("MSG_EVT_SER_POLLING_CHK\n");
		_ser_msg_hdl_polling_chk(cser);
		break;

	case MSG_EVT_SER_EVENT_CHK:
		PHL_INFO("MSG_EVT_SER_EVENT_CHK\n");
		_ser_msg_hdl_evt_chk(cser);
		break;

	case MSG_EVT_SER_M1_PAUSE_TRX:
		PHL_WARN("MSG_EVT_SER_M1_PAUSE_TRX\n");
		_ser_msg_hdl_m1(cser);
		break;

	case MSG_EVT_SER_IO_TIMER_EXPIRE:
		PHL_INFO("MSG_EVT_SER_IO_TIMER_EXPIRE\n");
		break;

	case MSG_EVT_SER_FW_TIMER_EXPIRE:
		PHL_WARN("MSG_EVT_SER_FW_TIMER_EXPIRE (state %d), do nothing and wait FW WDT\n",
		         cser->state);
		_ser_msg_hdl_fw_expire(cser);
		break;

	case MSG_EVT_SER_M3_DO_RECOV:
		PHL_INFO("MSG_EVT_SER_M3_DO_RECOV\n");
		_ser_msg_hdl_m3(cser);
		break;

	case MSG_EVT_SER_M5_READY:
		PHL_INFO("MSG_EVT_SER_M5_READY\n");
		_ser_msg_hdl_m5(cser);
		break;

	case MSG_EVT_SER_M9_L2_RESET:
		PHL_WARN("MSG_EVT_SER_M9_L2_RESET\n");
		_ser_msg_hdl_m9(cser);
		break;
	case MSG_EVT_SER_L2_RESET_DONE:
		PHL_WARN("MSG_EVT_SER_L2_RESET_DONE\n");
		_ser_msg_hdl_l2_reset_done(cser);
		break;
	}

	return ret;
}

static enum phl_mdl_ret_code
_phl_ser_mdl_init(void *phl, void *dispr, void **priv)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	struct cmd_ser *cser = NULL;
	u8	idx = 0;

	FUNCIN();
	if (priv == NULL)
		return MDL_RET_FAIL;

	(*priv) = NULL;
	cser = (struct cmd_ser *)_os_mem_alloc(drv, sizeof(struct cmd_ser));
	if (cser == NULL) {
		PHL_ERR(" %s, alloc fail\n",__FUNCTION__);
		return MDL_RET_FAIL;
	}

	_os_mem_set(drv, cser, 0, sizeof(struct cmd_ser));
	_os_spinlock_init(drv, &cser->_lock);
	_os_init_timer(drv,
	               &cser->poll_timer,
	               _ser_poll_timer_cb,
	               cser,
	               "cmd_ser_poll_timer");

	pq_init(drv, &cser->stslist);
	for (idx =0; idx < CMD_SER_LOG_SIZE; idx++) {
		INIT_LIST_HEAD(&cser->stsl2[idx].list);
		cser->stsl2[idx].idx = idx;
		pq_push(drv, &cser->stslist, &cser->stsl2[idx].list, _tail, _bh);
	}

	cser->phl_info = phl_info;
	cser->dispr = dispr;
	(*priv) = (void*)cser;

	if (phl_info->phl_com->hci_type == RTW_HCI_PCIE)
		cser->evtsrc = CMD_SER_SRC_INT;
	else if (phl_info->phl_com->hci_type == RTW_HCI_USB)
		cser->evtsrc = CMD_SER_SRC_POLL;
	else if (phl_info->phl_com->hci_type == RTW_HCI_SDIO)
		cser->evtsrc = CMD_SER_SRC_INT_NOTIFY;

	PHL_INFO("%s:: cser->evtsrc = %d\n", __func__, cser->evtsrc);
	FUNCOUT();

	return MDL_RET_SUCCESS;
}

static void _phl_ser_mdl_deinit(void *dispr, void *priv)
{
	struct cmd_ser *cser = (struct cmd_ser *)priv;
	void *drv = phl_to_drvpriv(cser->phl_info);

	FUNCIN();

	_os_cancel_timer(drv, &cser->poll_timer);
	_os_release_timer(drv, &cser->poll_timer);
	pq_deinit(drv, &cser->stslist);
	_os_spinlock_free(drv, &cser->_lock);
	_os_mem_free(drv, cser, sizeof(struct cmd_ser));
	PHL_INFO(" %s\n", __FUNCTION__);
}

static enum phl_mdl_ret_code
_phl_ser_mdl_start(void *dispr, void *priv)
{
	struct cmd_ser *cser = (struct cmd_ser *)priv;
	struct phl_info_t *phl_info = cser->phl_info;

	if (CMD_SER_SRC_POLL == cser->evtsrc) {
		/* Disable L0 Reset Notify from FW to driver */
		rtw_hal_ser_set_error_status(phl_info->hal, RTW_PHL_SER_L0_CFG_DIS_NOTIFY);
	}

	_ser_reset_status(cser);

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_phl_ser_mdl_stop(void *dispr, void *priv)
{
	struct cmd_ser *cser = (struct cmd_ser *)priv;
	void *drv = phl_to_drvpriv(cser->phl_info);

	_os_cancel_timer(drv, &cser->poll_timer);

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_phl_ser_mdl_msg_hdlr(void *dispr,
                      void *priv,
                      struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;

	if (IS_MSG_FAIL(msg->msg_id)) {
		PHL_INFO("%s :: MSG(%d)_FAIL - EVT_ID=%d \n", __func__,
		         MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));

		return _ser_fail_ev_hdlr(dispr, priv, msg);
	}

	switch (MSG_MDL_ID_FIELD(msg->msg_id)) {
		case PHL_MDL_SER:
			if (IS_MSG_IN_PRE_PHASE(msg->msg_id))
				ret = _ser_hdl_internal_evt(dispr, priv, msg);
			break;

		default:
			ret = _ser_hdl_external_evt(dispr, priv, msg);
			break;
	}

	return ret;
}

static enum phl_mdl_ret_code
_phl_ser_mdl_set_info(void *dispr,
                      void *priv,
                      struct phl_module_op_info *info)
{
	struct cmd_ser *cser = (struct cmd_ser *)priv;
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	/* PHL_INFO(" %s :: info->op_code=%d \n", __func__, info->op_code); */

	switch (info->op_code) {
		case BK_MODL_OP_INPUT_CMD:
			if (info->inbuf) {
				cser->ser_l2_hdlr=(u8 (*)(void *))info->inbuf;
			}
			ret = MDL_RET_SUCCESS;
			break;
	}

	return ret;
}

static enum phl_mdl_ret_code
_phl_ser_mdl_query_info(void *dispr,
                        void *priv,
                        struct phl_module_op_info *info)
{
	struct cmd_ser *cser = (struct cmd_ser *)priv;
	void *drv = phl_to_drvpriv(cser->phl_info);
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	/* PHL_INFO(" %s :: info->op_code=%d \n", __func__, info->op_code); */

	switch (info->op_code) {
		case BK_MODL_OP_STATE:
			_os_mem_cpy(drv, (void*)info->inbuf, &cser->state, 1);
			ret = MDL_RET_SUCCESS;
			break;
	}
	return ret;
}

static struct phl_bk_module_ops ser_ops= {
	.init = _phl_ser_mdl_init,
	.deinit = _phl_ser_mdl_deinit,
	.start = _phl_ser_mdl_start,
	.stop = _phl_ser_mdl_stop,
	.msg_hdlr = _phl_ser_mdl_msg_hdlr,
	.set_info = _phl_ser_mdl_set_info,
	.query_info = _phl_ser_mdl_query_info,
};

enum rtw_phl_status
phl_register_ser_module(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	phl_status = phl_disp_eng_register_module(phl_info,
	                                          HW_BAND_0,
	                                          PHL_MDL_SER,
						  &ser_ops);
	if (RTW_PHL_STATUS_SUCCESS != phl_status) {
		PHL_ERR("%s register SER module in cmd disp failed! \n", __func__);
	}

	return phl_status;
}

#ifdef CONFIG_PHL_CMD_SER
u8 phl_ser_inprogress(void *phl)
{
	struct phl_module_op_info op_info = {0};
	u8 state = 0;

	op_info.op_code = BK_MODL_OP_STATE;
	op_info.inbuf = (u8*)&state;
	op_info.inlen = 1;

	if (rtw_phl_query_bk_module_info(phl,
	                                 HW_BAND_0,
	                                 PHL_MDL_SER,
	                                 &op_info) == RTW_PHL_STATUS_SUCCESS) {
		if (state) /* non-CMD_SER_NOT_OCCUR */
			return true;
	}
	return false;
}

enum rtw_phl_status
phl_ser_send_msg(void *phl, enum RTW_PHL_SER_NOTIFY_EVENT notify)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_msg nextmsg = {0};
	struct phl_msg_attribute attr = {0};
	u16 event = 0;

	switch (notify) {
	case RTW_PHL_SER_PAUSE_TRX: /* M1 */
		event = MSG_EVT_SER_M1_PAUSE_TRX;
		break;
	case RTW_PHL_SER_DO_RECOVERY: /* M3 */
		event = MSG_EVT_SER_M3_DO_RECOV;
		break;
	case RTW_PHL_SER_READY: /* M5 */
		event = MSG_EVT_SER_M5_READY;
		break;
	case RTW_PHL_SER_L2_RESET: /* M9 */
		event = MSG_EVT_SER_M9_L2_RESET;
		break;
	case RTW_PHL_SER_EVENT_CHK:
		event = MSG_EVT_SER_EVENT_CHK;
		break;
	case RTW_PHL_SER_L2_RESET_DONE:
		event = MSG_EVT_SER_L2_RESET_DONE;
		break;
	case RTW_PHL_SER_L0_RESET:
	default:
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "phl_ser_send_msg(): unsupported case %d\n",
		          notify);
		return RTW_PHL_STATUS_FAILURE;
	}
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "phl_ser_send_msg(): event %d\n", event);

	SET_MSG_MDL_ID_FIELD(nextmsg.msg_id, PHL_MDL_SER);
	SET_MSG_EVT_ID_FIELD(nextmsg.msg_id, event);
	nextmsg.band_idx = HW_BAND_0;

	phl_status = phl_disp_eng_send_msg(phl,
					       &nextmsg,
					       &attr,
					       NULL);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("[CMD_SER] send_msg_to_dispr fail! (%d)\n", event);
	}

	return phl_status;
}
#endif

#ifndef CONFIG_FSM
/* The same as phl_fw_watchdog_timeout_notify of fsm-ser */
enum rtw_phl_status
phl_fw_watchdog_timeout_notify(void *phl)
{
	enum RTW_PHL_SER_NOTIFY_EVENT notify = RTW_PHL_SER_L2_RESET;
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "phl_fw_watchdog_timeout_notify triggle L2 Reset !!!\n");

	return phl_ser_send_msg(phl, notify);
}

enum rtw_phl_status rtw_phl_ser_l2_notify(struct rtw_phl_com_t *phl_com)
{
	enum RTW_PHL_SER_NOTIFY_EVENT notify = RTW_PHL_SER_L2_RESET;
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "rtw_phl_ser_l2_notify triggle L2 Reset !!!\n");

	return phl_ser_send_msg(phl_com->phl_priv, notify);
}

enum rtw_phl_status rtw_phl_ser_l2_done_notify(struct rtw_phl_com_t *phl_com)
{
	enum RTW_PHL_SER_NOTIFY_EVENT notify = RTW_PHL_SER_L2_RESET_DONE;
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "rtw_phl_ser_l2_done_notify restore L2 Reset !!!\n");

	return phl_ser_send_msg(phl_com->phl_priv, notify);
}

/* The same as rtw_phl_ser_dump_ple_buffer of fsm-ser */
enum rtw_phl_status
rtw_phl_ser_dump_ple_buffer(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "rtw_phl_ser_dump_ple_buffer\n");

	rtw_hal_dump_fw_rsvd_ple(phl_info->hal);

	return RTW_PHL_STATUS_SUCCESS;
}
#endif /*#ifndef CONFIG_FSM*/

