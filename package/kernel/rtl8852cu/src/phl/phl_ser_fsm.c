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
 *****************************************************************************/
#include "phl_headers.h"

#ifdef CONFIG_FSM
/*
 * SER stands for System Error Recovery
 */

#define SER_FW_TIMEOUT 1000 /* ms */
#define SER_POLLING_INTERVAL 10 /* ms */
#define SER_USB_POLLING_INTERVAL_IDL 1000 /* ms */
#define SER_USB_POLLING_INTERVAL_ACT 10 /* ms */

#define SER_POLL_IO_TIMES 200
#define SER_USB_POLL_IO_TIMES 300
#define SER_POLL_BULK_TIMES 100

#define SER_L0 0x00000001
#define SER_L1 0x00000002
#define SER_L2 0x00000004

struct ser_obj {
	struct fsm_main *fsm;
	struct phl_info_t *phl_info;
	struct fsm_obj *fsm_obj;

	int poll_io_times;
	int poll_bulk_times;
	bool trigger_l2_reset;
	bool trigger_l1_reset;
	bool dynamicThredRunning;
	_os_lock state_lock;
};

enum SER_STATE_ST {
	SER_ST_IDLE,
	SER_ST_L1_PAUSE_TRX,
	SER_ST_L1_DO_HCI,
	SER_ST_L1_RESUME_TRX,
	SER_ST_L2
};

enum SER_EV_ID {
	SER_EV_L1_START,
	//SER_EV_L2_START,
	//SER_EV_M1_PAUSE_TRX,
	SER_EV_M3_DO_RECOVERY,
	SER_EV_M5_READY,
	SER_EV_M9_L2_RESET,
	SER_EV_FW_TIMER_EXPIRE,
	SER_EV_POLL_IO_EXPIRE,
	SER_EV_POLL_BULK_EXPIRE,
	SER_EV_POLL_USB_INT_EXPIRE,
	SER_EV_CHK_SER_EVENT,
	SER_EV_MAX
};

static int ser_idle_st_hdl(void *obj, u16 event, void *param);
static int ser_usb_idle_st_hdl(void *obj, u16 event, void *param);
static int ser_pci_l1_pause_trx_st_hdl(void *obj, u16 event, void *param);
static int ser_usb_l1_pause_trx_st_hdl(void *obj, u16 event, void *param);
static int ser_sdio_l1_pause_trx_st_hdl(void *obj, u16 event, void *param);
static int ser_pci_l1_do_hci_st_hdl(void *obj, u16 event, void *param);
static int ser_usb_l1_do_hci_st_hdl(void *obj, u16 event, void *param);
static int ser_sdio_l1_do_hci_st_hdl(void *obj, u16 event, void *param);
static int ser_l1_resume_trx_st_hdl(void *obj, u16 event, void *param);
static int ser_l2_st_hdl(void *obj, u16 event, void *param);

/* PCIE STATE table */
static struct fsm_state_ent ser_pci_state_tbl[] = {
	ST_ENT(SER_ST_IDLE, ser_idle_st_hdl),
	ST_ENT(SER_ST_L1_PAUSE_TRX, ser_pci_l1_pause_trx_st_hdl),
	ST_ENT(SER_ST_L1_DO_HCI, ser_pci_l1_do_hci_st_hdl),
	ST_ENT(SER_ST_L1_RESUME_TRX, ser_l1_resume_trx_st_hdl),
	ST_ENT(SER_ST_L2, ser_l2_st_hdl)
};

/* USB STATE table */
static struct fsm_state_ent ser_usb_state_tbl[] = {
	ST_ENT(SER_ST_IDLE, ser_usb_idle_st_hdl),
	ST_ENT(SER_ST_L1_PAUSE_TRX, ser_usb_l1_pause_trx_st_hdl),
	ST_ENT(SER_ST_L1_DO_HCI, ser_usb_l1_do_hci_st_hdl),
	ST_ENT(SER_ST_L1_RESUME_TRX, ser_l1_resume_trx_st_hdl),
	ST_ENT(SER_ST_L2, ser_l2_st_hdl)
};

/* SDIO STATE table */
static struct fsm_state_ent ser_sdio_state_tbl[] = {
	ST_ENT(SER_ST_IDLE, ser_idle_st_hdl),
	ST_ENT(SER_ST_L1_PAUSE_TRX, ser_sdio_l1_pause_trx_st_hdl),
	ST_ENT(SER_ST_L1_DO_HCI, ser_sdio_l1_do_hci_st_hdl),
	ST_ENT(SER_ST_L1_RESUME_TRX, ser_l1_resume_trx_st_hdl),
	ST_ENT(SER_ST_L2, ser_l2_st_hdl)
};


/* EVENT table */
static struct fsm_event_ent ser_event_tbl[] = {
	EV_ENT(SER_EV_L1_START),
	//EV_ENT(SER_EV_L2_START),
	//EV_ENT(SER_EV_M1_PAUSE_TRX),
	EV_ENT(SER_EV_M3_DO_RECOVERY),
	EV_ENT(SER_EV_M5_READY),
	EV_ENT(SER_EV_M9_L2_RESET),
	EV_ENT(SER_EV_FW_TIMER_EXPIRE),
	EV_ENT(SER_EV_POLL_IO_EXPIRE),
	EV_ENT(SER_EV_POLL_BULK_EXPIRE),
	EV_DBG(SER_EV_POLL_USB_INT_EXPIRE),
	EV_ENT(SER_EV_CHK_SER_EVENT),
	EV_ENT(SER_EV_MAX) /* EV_MAX for fsm safety checking */
};

static enum rtw_phl_status _ser_event_notify(void *phl, u8* p_ntfy)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct ser_obj *pser = phl_info->ser_obj;
	struct phl_msg msg = {0};
	enum RTW_PHL_SER_NOTIFY_EVENT notify = RTW_PHL_SER_L2_RESET;
	u32 err = 0;

	notify = rtw_hal_ser_get_error_status(pser->phl_info->hal, &err);

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


/*
 * SER sub function
 */
void ser_pcie_pause_dma_io(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
}

int ser_polling_io_state(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
	return 0; /* success */
}

void ser_return_all_tcb_and_mem(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
}

void ser_send_l0_handle_method_event(struct ser_obj *pser, enum RTW_PHL_SER_RCVY_STEP event)
{
	FSM_DBG(pser->fsm, "%s: %s with event = 0x%04X\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__, event);

	/*
	* Default use notify only RTW_PHL_SER_L0_CFG_NOTIFY
	* if need do l0 debug, set RTW_PHL_SER_L0_CFG_HANDSHAKE when init.
	* 1. after receive L0 notify within h2c, dump some Crs for debug.
	* 2. set RTW_PHL_SER_L0_RCVY_EN
	*/
	phl_ser_event_to_fw(pser->phl_info, event);
}

void ser_send_l0_do_rcvy_event(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);

	/*Default use notify only RTW_PHL_SER_L0_CFG_NOTIFY*/
	phl_ser_event_to_fw(pser->phl_info, RTW_PHL_SER_L0_RCVY_EN);
}


void ser_send_m2_event(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);

	phl_ser_event_to_fw(pser->phl_info, RTW_PHL_SER_L1_DISABLE_EN);
}

void ser_send_m4_event(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
	phl_ser_event_to_fw(pser->phl_info, RTW_PHL_SER_L1_RCVY_EN);
}

void ser_usb_trx_disable_cr(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
}

int ser_usb_wait_all_bulk_complete(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
	return 0; /* success */
}

void ser_pci_clear_rw_pointer(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
}

void ser_pci_assert_dessert_dma_reset(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
}

void ser_pci_reset_bdram(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
}

void ser_pci_enable_dma_io(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
}

void ser_usb_enable_bulk_IO(struct ser_obj *pser)
{
	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);
}
void ser_resume_trx_process(struct ser_obj *pser, u8 type)
{
	struct phl_hci_trx_ops *ops = pser->phl_info->hci_trx_ops;

	FSM_DBG(pser->fsm, "%s: %s\n",
		phl_fsm_obj_name(pser->fsm_obj), __func__);

	ops->trx_resume(pser->phl_info, type);
}

#ifdef RTW_WKARD_SER_USB_POLLING_EVENT
extern bool rtw_phl_recognize_interrupt(void *phl);
extern enum rtw_phl_status rtw_phl_interrupt_handler(void *phl);
static void ser_usb_chk_int_event(struct ser_obj *pser)
{
	if (true == rtw_phl_recognize_interrupt(pser->phl_info))
		rtw_phl_interrupt_handler(pser->phl_info);
}
#endif

/*
 * SER state handler
 */
u8 rtw_phl_ser_inprogress(void *phl)
{
	return phl_ser_inprogress(phl);
}

void rtw_phl_ser_clear_status(void *phl, u32 serstatus)
{
	 phl_ser_clear_status(phl, serstatus);
}

/*
 * ser idle handler
 */
static int ser_idle_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;
	u8 notify;

	switch (event) {
	case FSM_EV_STATE_IN:
		break;

	case SER_EV_CHK_SER_EVENT:
		if (true == rtw_hal_recognize_halt_c2h_interrupt(pser->phl_info->hal)) {
			_ser_event_notify(pser->phl_info, &notify);
			if ((notify == RTW_PHL_SER_L0_RESET) || (notify == RTW_PHL_SER_L2_RESET))
				rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
		}
		else {
			rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
		}

		break;

	case SER_EV_L1_START:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L1_PAUSE_TRX);
		break;
#if 0
	case SER_EV_L2_START:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		break;
#endif

	case SER_EV_M9_L2_RESET:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		break;

	case FSM_EV_STATE_OUT:
		break;

	default:
		break;
	}
	return 0;
}

static int ser_usb_idle_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;

	switch (event) {
	case FSM_EV_SWITCH_IN:
#if 1
		/* Disable L0 Reset Notify from FW to driver */
		ser_send_l0_handle_method_event(pser, RTW_PHL_SER_L0_CFG_DIS_NOTIFY);
#endif
		/* fallthrough */
	case FSM_EV_STATE_IN:

#if defined(RTW_WKARD_SER_USB_POLLING_EVENT) && !defined(CONFIG_PHL_CMD_SER)
		phl_fsm_set_alarm_ext(pser->fsm_obj,
			SER_USB_POLLING_INTERVAL_IDL, SER_EV_POLL_USB_INT_EXPIRE, 1, NULL);
#endif

		break;

#ifdef RTW_WKARD_SER_USB_POLLING_EVENT
	case SER_EV_POLL_USB_INT_EXPIRE:
		ser_usb_chk_int_event(pser);

		phl_fsm_set_alarm_ext(pser->fsm_obj,
			SER_USB_POLLING_INTERVAL_IDL, SER_EV_POLL_USB_INT_EXPIRE, 1, NULL);
		break;
#endif

	case SER_EV_L1_START:
#ifndef RTW_WKARD_SER_USB_DISABLE_L1_RCVY_FLOW
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L1_PAUSE_TRX);
#endif
		break;

#if 0
	case SER_EV_L2_START:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		break;
#endif

	case SER_EV_M9_L2_RESET:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm_ext(pser->fsm_obj, 1);
		break;

	default:
		break;
	}
	return 0;
}

static int ser_pci_l1_pause_trx_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;
	struct phl_hci_trx_ops *ops = pser->phl_info->hci_trx_ops;
	struct phl_msg msg = {0};
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_SER);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_SER_L1);

	switch (event) {
	case FSM_EV_STATE_IN:

		phl_ser_set_status(pser, SER_L1);

		//send msg to core
		phl_msg_hub_send(pser->phl_info, NULL, &msg);

		ops->req_tx_stop(pser->phl_info);
		rtw_phl_tx_req_notify(pser->phl_info);

		if(rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_RCVY_STEP_1)
			!= RTW_HAL_STATUS_SUCCESS){
			ser_resume_trx_process(pser, PHL_CTRL_TX);
			rtw_phl_tx_req_notify(pser->phl_info);
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
			break;
		}

		ops->req_rx_stop(pser->phl_info);
		rtw_phl_start_rx_process(pser->phl_info);

		if (false == ops->is_tx_pause(pser->phl_info) ||
			false == ops->is_rx_pause(pser->phl_info)) {
			/* pci: polling fail; wait for a while */
			phl_fsm_set_alarm(pser->fsm_obj,
				SER_POLLING_INTERVAL, SER_EV_POLL_IO_EXPIRE);

			/* prevent infinite polling */
			pser->poll_io_times = SER_POLL_IO_TIMES;
			break;
		}

		ops->trx_reset(pser->phl_info, PHL_CTRL_TX | PHL_CTRL_RX);

		/* pci: send M2 event */
		ser_send_m2_event(pser);

		/* pci: wait M3 */
		phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
			SER_EV_FW_TIMER_EXPIRE);
		break;

	case SER_EV_M3_DO_RECOVERY:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L1_DO_HCI);
		break;

	case SER_EV_FW_TIMER_EXPIRE:
		/*phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);*/
		status = rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_SER_RCVY_STEP_2);

		ser_resume_trx_process(pser, PHL_CTRL_RX);
		rtw_phl_start_rx_process(pser->phl_info);
		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);

		if (status != RTW_HAL_STATUS_SUCCESS) {
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		} else {
			phl_ser_clear_status(pser, SER_L1);
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		}

		break;

	case SER_EV_M9_L2_RESET:

		ser_resume_trx_process(pser, PHL_CTRL_RX);
		rtw_phl_start_rx_process(pser->phl_info);
		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);

		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);

		break;

	case SER_EV_POLL_IO_EXPIRE:

		if (pser->poll_io_times-- <= 0) {
#ifdef RTW_WKARD_SER_L1_EXPIRE

			ops->trx_reset(pser->phl_info, PHL_CTRL_TX | PHL_CTRL_RX);

			/* pci: send M2 event */
			ser_send_m2_event(pser);

			/* pci: wait M3 */
			phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
				SER_EV_FW_TIMER_EXPIRE);

#else
			ser_resume_trx_process(pser, PHL_CTRL_RX);
			rtw_phl_start_rx_process(pser->phl_info);
			ser_resume_trx_process(pser, PHL_CTRL_TX);
			rtw_phl_tx_req_notify(pser->phl_info);
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
#endif
			break;
		}

		/*send msg to core*/
		/*phl_msg_hub_send(pser->phl_info, &msg);*/

		if (false == ops->is_tx_pause(pser->phl_info) ||
			false == ops->is_rx_pause(pser->phl_info) ) {
			/* pci: polling fail; wait for a while */
			phl_fsm_set_alarm(pser->fsm_obj,
				SER_POLLING_INTERVAL, SER_EV_POLL_IO_EXPIRE);
			break;
		}
/*
		if(rtw_hal_lv1_rcvy(hal_info, RTW_PHL_SER_LV1_RCVY_STEP_1) != RTW_HAL_STATUS_SUCCESS){
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
			break;
		}
*/
		ops->trx_reset(pser->phl_info, PHL_CTRL_TX | PHL_CTRL_RX);

		/* pci: send M2 event */
		ser_send_m2_event(pser);

		/* pci: wait M3 */
		phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
			SER_EV_FW_TIMER_EXPIRE);
		break;

	case FSM_EV_CANCEL:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pser->fsm_obj);
		break;

	default:
		break;
	}
	return 0;
}

static void ser_usb_l1_pause_trx_done(struct ser_obj *pser)
{
	rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_RCVY_STEP_1);

	ser_send_m2_event(pser);
}

static int ser_usb_l1_pause_trx_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;
	struct phl_hci_trx_ops *ops = pser->phl_info->hci_trx_ops;
	struct phl_msg msg = {0};
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	switch (event) {
	case FSM_EV_STATE_IN:
		phl_ser_set_status(pser, SER_L1);

		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_SER);
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_SER_L1);
		phl_msg_hub_send(pser->phl_info, NULL, &msg);

		ops->req_tx_stop(pser->phl_info);
		rtw_phl_tx_req_notify(pser->phl_info);

#if 0
		/* not request pause PHL Rx in USB data path */
		ops->req_rx_stop(pser->phl_info);
		rtw_phl_start_rx_process(pser->phl_info);
#endif

		if (false == ops->is_tx_pause(pser->phl_info)
#if 0
			|| false == ops->is_rx_pause(pser->phl_info)
#endif
			) {
			phl_fsm_set_alarm(pser->fsm_obj,
				SER_USB_POLLING_INTERVAL_ACT, SER_EV_POLL_IO_EXPIRE);

			pser->poll_io_times = SER_USB_POLL_IO_TIMES;
			break;
		}

		ser_usb_l1_pause_trx_done(pser);

#ifdef RTW_WKARD_SER_USB_POLLING_EVENT
		phl_fsm_set_alarm_ext(pser->fsm_obj,
			SER_USB_POLLING_INTERVAL_ACT, SER_EV_POLL_USB_INT_EXPIRE, 1, NULL);
#endif

		phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
			SER_EV_FW_TIMER_EXPIRE);

		break;

#ifdef RTW_WKARD_SER_USB_POLLING_EVENT
	case SER_EV_POLL_USB_INT_EXPIRE:
		ser_usb_chk_int_event(pser);

		phl_fsm_set_alarm_ext(pser->fsm_obj,
			SER_USB_POLLING_INTERVAL_ACT, SER_EV_POLL_USB_INT_EXPIRE, 1, NULL);
		break;
#endif

	case SER_EV_M3_DO_RECOVERY:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L1_DO_HCI);
		break;

	case SER_EV_FW_TIMER_EXPIRE:
		status = rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_SER_RCVY_STEP_2);
		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);

		if (status != RTW_HAL_STATUS_SUCCESS)
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		else
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);

		break;

	case SER_EV_M9_L2_RESET:
		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		break;

	case SER_EV_POLL_IO_EXPIRE:

		if (pser->poll_io_times-- <= 0) {
#ifdef RTW_WKARD_SER_L1_EXPIRE
			ser_usb_l1_pause_trx_done(pser);

			phl_fsm_set_alarm_ext(pser->fsm_obj,
				SER_USB_POLLING_INTERVAL_ACT, SER_EV_POLL_USB_INT_EXPIRE, 1, NULL);

			phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
				SER_EV_FW_TIMER_EXPIRE);
#else
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
#endif
			break;
		}

		if (false == ops->is_tx_pause(pser->phl_info)
#if 0
			|| false == ops->is_rx_pause(pser->phl_info)
#endif
			) {
			phl_fsm_set_alarm(pser->fsm_obj,
				SER_USB_POLLING_INTERVAL_ACT, SER_EV_POLL_IO_EXPIRE);
			break;
		}

		ser_usb_l1_pause_trx_done(pser);

		phl_fsm_set_alarm_ext(pser->fsm_obj,
			SER_USB_POLLING_INTERVAL_ACT, SER_EV_POLL_USB_INT_EXPIRE, 1, NULL);

		phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
			SER_EV_FW_TIMER_EXPIRE);

		break;

	case FSM_EV_CANCEL:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pser->fsm_obj);
		phl_fsm_cancel_alarm_ext(pser->fsm_obj, 1);
		break;

	default:
		break;
	}
	return 0;
}

static int ser_sdio_l1_pause_trx_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;
	struct phl_hci_trx_ops *ops = pser->phl_info->hci_trx_ops;
	struct phl_msg msg = {0};
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	switch (event) {
	case FSM_EV_STATE_IN:
		phl_ser_set_status(pser, BIT1);

		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_SER);
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_SER_L1);
		phl_msg_hub_send(pser->phl_info, NULL, &msg);

		ops->req_tx_stop(pser->phl_info);
		rtw_phl_tx_req_notify(pser->phl_info);

		ops->req_rx_stop(pser->phl_info);
		rtw_phl_start_rx_process(pser->phl_info);

		if (false == ops->is_tx_pause(pser->phl_info)
			|| false == ops->is_rx_pause(pser->phl_info)) {
			phl_fsm_set_alarm(pser->fsm_obj,
				SER_POLLING_INTERVAL, SER_EV_POLL_IO_EXPIRE);

			pser->poll_io_times = SER_POLL_IO_TIMES;
			break;
		}

		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_DIS_HCI_INT);

		if (rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_RCVY_STEP_1)
			!= RTW_HAL_STATUS_SUCCESS) {
			rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
			break;
		}

		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_SER_HANDSHAKE_MODE);

		ser_send_m2_event(pser);

		phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
			SER_EV_FW_TIMER_EXPIRE);

		break;

	case SER_EV_CHK_SER_EVENT:
		if (true == rtw_hal_recognize_halt_c2h_interrupt(pser->phl_info->hal))
			_ser_event_notify(pser->phl_info, NULL);

		break;

	case SER_EV_M3_DO_RECOVERY:
		rtw_hal_clear_interrupt(pser->phl_info->hal);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L1_DO_HCI);
		break;

	case SER_EV_FW_TIMER_EXPIRE:
		status = rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_SER_RCVY_STEP_2);

		ser_resume_trx_process(pser, PHL_CTRL_RX);
		rtw_phl_start_rx_process(pser->phl_info);
		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);

		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);

		if (status != RTW_HAL_STATUS_SUCCESS) {
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		} else {
			phl_ser_clear_status(pser, BIT1);
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		}

		break;

	case SER_EV_M9_L2_RESET:
		ser_resume_trx_process(pser, PHL_CTRL_RX);
		rtw_phl_start_rx_process(pser->phl_info);
		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);

		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);

		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		break;

	case SER_EV_POLL_IO_EXPIRE:

		if (pser->poll_io_times-- <= 0) {
#ifdef RTW_WKARD_SER_L1_EXPIRE

			if (rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_RCVY_STEP_1)
				!= RTW_HAL_STATUS_SUCCESS) {
				rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
				phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
				break;
			}

			rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_SER_HANDSHAKE_MODE);

			ser_send_m2_event(pser);

			phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
				SER_EV_FW_TIMER_EXPIRE);

#else
			ser_resume_trx_process(pser, PHL_CTRL_RX);
			rtw_phl_start_rx_process(pser->phl_info);
			ser_resume_trx_process(pser, PHL_CTRL_TX);
			rtw_phl_tx_req_notify(pser->phl_info);

			rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);

			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
#endif
			break;
		}

		if (false == ops->is_tx_pause(pser->phl_info) ||
			false == ops->is_rx_pause(pser->phl_info)) {
			phl_fsm_set_alarm(pser->fsm_obj,
				SER_POLLING_INTERVAL, SER_EV_POLL_IO_EXPIRE);
			break;
		}

		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_DIS_HCI_INT);

		if (rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_RCVY_STEP_1)
			!= RTW_HAL_STATUS_SUCCESS) {
			rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
			break;
		}

		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_SER_HANDSHAKE_MODE);

		ser_send_m2_event(pser);

		phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
			SER_EV_FW_TIMER_EXPIRE);
		break;

	case FSM_EV_CANCEL:
		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pser->fsm_obj);
		break;

	default:
		break;
	}
	return 0;
}

static int ser_pci_l1_do_hci_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;

	switch (event) {
	case FSM_EV_STATE_IN:

		if(rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_SER_RCVY_STEP_2)
			!= RTW_HAL_STATUS_SUCCESS){
			ser_resume_trx_process(pser, PHL_CTRL_RX);
			rtw_phl_start_rx_process(pser->phl_info);
			ser_resume_trx_process(pser, PHL_CTRL_TX);
			rtw_phl_tx_req_notify(pser->phl_info);
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
			break;
		}
		ser_resume_trx_process(pser, PHL_CTRL_RX);
		rtw_phl_start_rx_process(pser->phl_info);

		/* pci: send M4 event */
		ser_send_m4_event(pser);

		/* pci: set timeout to wait M5 */
		phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
			SER_EV_FW_TIMER_EXPIRE);
		break;

	case SER_EV_M5_READY:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L1_RESUME_TRX);
		break;


	case SER_EV_FW_TIMER_EXPIRE:
		/* PHL_INFO("ser_pci_l1_do_hci_st_hdl():	SER_EV_FW_TIMER_EXPIRE \n"); */
		/* _ser_event_notify(pser->phl_info, NULL); */
		/* phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2); */
		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);
		phl_ser_clear_status(pser, SER_L1);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case SER_EV_M9_L2_RESET:

		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);

		break;

	case FSM_EV_CANCEL:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pser->fsm_obj);
		break;

	default:
		break;
	}
	return 0;
}

static int ser_usb_l1_do_hci_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;

	switch (event) {
	case FSM_EV_STATE_IN:
		if (rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_SER_RCVY_STEP_2)
			!= RTW_HAL_STATUS_SUCCESS) {
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
			break;
		}
		ser_resume_trx_process(pser, PHL_CTRL_RX);
		rtw_phl_start_rx_process(pser->phl_info);

		ser_send_m4_event(pser);

#ifdef RTW_WKARD_SER_USB_POLLING_EVENT
		phl_fsm_set_alarm_ext(pser->fsm_obj,
			SER_USB_POLLING_INTERVAL_ACT, SER_EV_POLL_USB_INT_EXPIRE, 1, NULL);
#endif

		phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
			SER_EV_FW_TIMER_EXPIRE);
		break;

#ifdef RTW_WKARD_SER_USB_POLLING_EVENT
	case SER_EV_POLL_USB_INT_EXPIRE:
		ser_usb_chk_int_event(pser);

		phl_fsm_set_alarm_ext(pser->fsm_obj,
			SER_USB_POLLING_INTERVAL_ACT, SER_EV_POLL_USB_INT_EXPIRE, 1, NULL);
		break;
#endif

	case SER_EV_M5_READY:
	case SER_EV_FW_TIMER_EXPIRE:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L1_RESUME_TRX);
		break;

	case SER_EV_M9_L2_RESET:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		break;

	case FSM_EV_CANCEL:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pser->fsm_obj);
		phl_fsm_cancel_alarm_ext(pser->fsm_obj, 1);
		break;

	default:
		break;
	}
	return 0;
}

static int ser_sdio_l1_do_hci_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;

	switch (event) {
	case FSM_EV_STATE_IN:

		if (rtw_hal_lv1_rcvy(pser->phl_info->hal, RTW_PHL_SER_LV1_SER_RCVY_STEP_2)
			!= RTW_HAL_STATUS_SUCCESS) {
			rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
			phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
			break;
		}
		ser_resume_trx_process(pser, PHL_CTRL_RX);
		rtw_phl_start_rx_process(pser->phl_info);

		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_SER_HANDSHAKE_MODE);

		ser_send_m4_event(pser);

		phl_fsm_set_alarm(pser->fsm_obj, SER_FW_TIMEOUT,
			SER_EV_FW_TIMER_EXPIRE);
		break;

	case SER_EV_CHK_SER_EVENT:
		if (true == rtw_hal_recognize_halt_c2h_interrupt(pser->phl_info->hal))
			_ser_event_notify(pser->phl_info, NULL);

		break;

	case SER_EV_M5_READY:
		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L1_RESUME_TRX);
		break;

	case SER_EV_FW_TIMER_EXPIRE:
		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);
		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case SER_EV_M9_L2_RESET:
		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_L2);
		break;

	case FSM_EV_CANCEL:
		rtw_hal_config_interrupt(pser->phl_info->hal, RTW_PHL_EN_HCI_INT);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pser->fsm_obj);
		break;

	default:
		break;
	}
	return 0;
}

static int ser_l1_resume_trx_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;


	switch (event) {
	case FSM_EV_STATE_IN:

		/* TODO resume TRX process */
		ser_resume_trx_process(pser, PHL_CTRL_TX);
		rtw_phl_tx_req_notify(pser->phl_info);
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_CANCEL:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pser->fsm_obj);
		phl_ser_clear_status(pser, SER_L1);
		break;

	default:
		break;
	}
	return 0;
}

static int ser_l2_st_hdl(void *obj, u16 event, void *param)
{
	struct ser_obj *pser = (struct ser_obj *)obj;
	struct phl_msg msg = {0};

	switch (event) {
	case FSM_EV_STATE_IN:

		/*To avoid fw watchdog intr trigger periodically*/
		rtw_hal_ser_reset_wdt_intr(pser->phl_info->hal);

		/* TODO do something */
		phl_ser_clear_status(pser, SER_L1);
		phl_ser_set_status(pser, SER_L2);

		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_SER);
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_SER_L2);
		phl_msg_hub_send(pser->phl_info, NULL, &msg);

		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_TIMER_EXPIRE:
		break;

	case FSM_EV_CANCEL:
		phl_fsm_state_goto(pser->fsm_obj, SER_ST_IDLE);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pser->fsm_obj);
		break;

	default:
		break;
	}
	return 0;
}

static void ser_dump_obj(void *obj, char *s, int *sz)
{
	/* nothing to do for now */
}

static void ser_dump_fsm(void *fsm, char *s, int *sz)
{
	/* nothing to do for now */
}

/* For EXTERNAL application to create a ser FSM */
/* @root: FSM root structure
 * @phl_info: private data structure to invoke hal/phl function
 *
 * return
 * fsm_main: FSM main structure (Do NOT expose)
 */
struct fsm_main *phl_ser_new_fsm(struct fsm_root *root,
	struct phl_info_t *phl_info)
{
	void *d = phl_to_drvpriv(phl_info);
	struct fsm_main *fsm = NULL;
	struct rtw_phl_fsm_tb tb;

	_os_mem_set(d, &tb, 0, sizeof(tb));

	if (phl_info->phl_com->hci_type == RTW_HCI_PCIE)
		tb.state_tbl = ser_pci_state_tbl;
	else if (phl_info->phl_com->hci_type == RTW_HCI_USB)
		tb.state_tbl = ser_usb_state_tbl;
	else if (phl_info->phl_com->hci_type == RTW_HCI_SDIO)
		tb.state_tbl = ser_sdio_state_tbl;
	else
		return NULL;

	tb.max_state = sizeof(ser_pci_state_tbl)/sizeof(ser_pci_state_tbl[0]);
	tb.max_event = sizeof(ser_event_tbl)/sizeof(ser_event_tbl[0]);
	tb.evt_tbl = ser_event_tbl;
	tb.dump_obj = ser_dump_obj;
	tb.dump_obj = ser_dump_fsm;
	tb.dbg_level = FSM_DBG_INFO;
	tb.evt_level = FSM_DBG_INFO;

	fsm = phl_fsm_init_fsm(root, "ser", phl_info, &tb);

	return fsm;
}

/* For EXTERNAL application to destory ser fsm */
/* @fsm: see fsm_main
 */
void phl_ser_destory_fsm(struct fsm_main *fsm)
{
	if (fsm == NULL)
		return;

	/* deinit fsm local variable if has */

	/* call FSM Framewro to deinit fsm */
	phl_fsm_deinit_fsm(fsm);
}

/* For EXTERNAL application to create SER object */
/* @fsm: FSM main structure which created by phl_ser_new_fsm()
 * @phl_info: private data structure to invoke hal/phl function
 *
 * return
 * ser_obj: structure of SER object (Do NOT expose)
 */
struct ser_obj *phl_ser_new_obj(struct fsm_main *fsm,
	struct phl_info_t *phl_info)
{
	void *d = phl_to_drvpriv(phl_info);
	struct fsm_obj *obj;
	struct ser_obj *pser;


	pser = phl_fsm_new_obj(fsm, (void **)&obj, sizeof(*pser));

	if (pser == NULL) {
		/* TODO free fsm; currently will be freed in deinit process */
		FSM_ERR(fsm, "ser: malloc obj fail\n");
		return NULL;
	}
	pser->fsm = fsm;
	pser->fsm_obj = obj;
	pser->phl_info = phl_info;

	/* init local use variable */
	_os_spinlock_init(d, &pser->state_lock);

	return pser;
}

/* For EXTERNAL application to destory ser object */
/* @pser: local created command object
 *
 */
void phl_ser_destory_obj(struct ser_obj *pser)
{
	void *d;

	if (pser == NULL)
		return;

	d = phl_to_drvpriv(pser->phl_info);

	/* deinit obj local variable if has */
	_os_spinlock_free(d, &pser->state_lock);

	/* inform FSM framewory to recycle fsm_obj */
	phl_fsm_destory_obj(pser->fsm_obj);
}

/* For EXTERNAL application to stop ser service (expose) */
/* @pser: ser job will be cancelled
 */
enum rtw_phl_status phl_ser_cancel(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct ser_obj *pser = phl_info->ser_obj;

	return phl_fsm_gen_msg(phl, pser->fsm_obj, NULL, 0, FSM_EV_CANCEL);
}

/* For EXTERNAL interrupt handler to send event into ser fsm (expose) */
enum rtw_phl_status phl_ser_event_to_fw(void *phl, u32 err)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_hal_status status;

	status = rtw_hal_ser_set_error_status(phl_info->hal ,err);
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "phl_ser_event_to_fw err %d, status 0x%x\n", err, status);
	return status;
}

/* For EXTERNAL interrupt handler and dump fw ple (expose) */
enum rtw_phl_status rtw_phl_ser_dump_ple_buffer(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "rtw_phl_ser_dump_ple_buffer\n");

	rtw_hal_dump_fw_rsvd_ple(phl_info->hal);

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL interrupt handler and dump fw ple (expose) */
enum rtw_phl_status phl_fw_watchdog_timeout_notify(void *phl)
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

/* @phl: phl_info_t
 * @notify: event to ser fsm
 */

#ifndef CONFIG_PHL_CMD_SER
enum rtw_phl_status phl_ser_send_msg(void *phl,
	enum RTW_PHL_SER_NOTIFY_EVENT notify)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct ser_obj *pser = phl_info->ser_obj;
	u16 event;

	switch (notify) {
	case RTW_PHL_SER_PAUSE_TRX: /* M1 */
		event = SER_EV_L1_START;
		break;
	case RTW_PHL_SER_DO_RECOVERY: /* M3 */
		event = SER_EV_M3_DO_RECOVERY;
		break;
	case RTW_PHL_SER_READY: /* M5 */
		event = SER_EV_M5_READY;
		break;
	case RTW_PHL_SER_L2_RESET: /* M9 */
		event = SER_EV_M9_L2_RESET;
		break;
	case RTW_PHL_SER_EVENT_CHK:
		event = SER_EV_CHK_SER_EVENT;
		break;
	case RTW_PHL_SER_L0_RESET:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "phl_ser_send_msg(): Unsupported case:%d, please check it\n",
				notify);
		return RTW_PHL_STATUS_FAILURE;
	default:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "phl_ser_send_msg(): Unrecognize case:%d, please check it\n",
				notify);
		return RTW_PHL_STATUS_FAILURE;
	}
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "phl_ser_send_msg  event  %d\n", event);

	return phl_fsm_gen_msg(phl, pser->fsm_obj, NULL, 0, event);
}


/* For EXTERNAL application to do L2 reset (expose) */
/* @pser: ser job will be cancelled
 */
u8 phl_ser_inprogress(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct ser_obj *pser = phl_info->ser_obj;
	u8 reset_type = 0;
	void *d = phl_to_drvpriv(phl_info);

	_os_spinlock(d, &pser->state_lock, _bh, NULL);

	if(pser->trigger_l1_reset == true) {
		reset_type |= SER_L1;
		//pser->trigger_l2_reset = false;
	}
	if(pser->trigger_l2_reset == true) {
		reset_type |= SER_L2;
	}

	_os_spinunlock(d, &pser->state_lock, _bh, NULL);
	return reset_type;
}
#endif

/* For EXTERNAL application to do L2 reset (expose) */
/* @pser: ser job will be cancelled
 */
void phl_ser_clear_status(struct ser_obj *pser, u32 serstatus)
{
	void *d = phl_to_drvpriv(pser->phl_info);

	_os_spinlock(d, &pser->state_lock, _bh, NULL);

	if(serstatus & SER_L1) {
		pser->trigger_l1_reset = false;
	}
	if(serstatus & SER_L2) {
		pser->trigger_l2_reset = false;
	}

	_os_spinunlock(d, &pser->state_lock, _bh, NULL);
}

void phl_ser_set_status(struct ser_obj *pser, u32 serstatus)
{
	void *d = phl_to_drvpriv(pser->phl_info);

	_os_spinlock(d, &pser->state_lock, _bh, NULL);

	if(serstatus & SER_L1) {
		pser->trigger_l1_reset = true;
	}
	if(serstatus & SER_L2) {
		pser->trigger_l2_reset = true;
	}

	_os_spinunlock(d, &pser->state_lock, _bh, NULL);
}


/* For EXTERNAL application notify from upper layer*/
void phl_ser_notify_from_upper_watchdog_status(void *phl, bool inprogress)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct ser_obj *pser = phl_info->ser_obj;

	pser->dynamicThredRunning = inprogress;
}

void rtw_phl_notify_watchdog_status(void *phl, bool inprogress)
{
	phl_ser_notify_from_upper_watchdog_status(phl, inprogress);
}
#endif /*CONFIG_FSM*/

