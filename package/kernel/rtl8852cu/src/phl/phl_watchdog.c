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
#define _PHL_WATCHDOG_C_
#include "phl_headers.h"

#ifdef CONFIG_FSM
static void _phl_datapath_watchdog(struct phl_info_t *phl_info)
{
	phl_tx_watchdog(phl_info);
	phl_rx_watchdog(phl_info);
	phl_sta_trx_tfc_upd(phl_info);
}

void rtw_phl_watchdog_callback(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	do {
		_phl_datapath_watchdog(phl_info);
		#ifdef CONFIG_PCI_HCI
		#ifdef RTW_WKARD_DYNAMIC_LTR
		phl_ltr_ctrl_watchdog(phl_info);
		#endif
		#ifdef PCIE_TRX_MIT_EN
		phl_pcie_trx_mit_watchdog(phl_info);
		#endif
		#endif
		phl_mr_watchdog(phl_info);
		rtw_hal_watchdog(phl_info->hal, phl_info->phl_com);
	} while (false);
}
#endif

static void _phl_watchdog_sw(struct phl_info_t *phl)
{
	/* Only sw statistics or sw behavior or trigger FG cmd */
	phl_tx_watchdog(phl);
	phl_rx_watchdog(phl);
	phl_sta_trx_tfc_upd(phl);
}

#ifdef PHL_WATCHDOG_REFINE
static void _phl_watchdog_sw_post(struct phl_info_t *phl)
{
	return;
}
#endif

static void _phl_watchdog_hw(struct phl_info_t *phl)
{
	#ifdef DBG_DUMP_TX_COUNTER
	if (phl->phl_com->dbg_dump_tx)
		rtw_hal_dump_tx_status(phl->hal, phl->phl_com->dbg_dump_tx_bidx);
	#endif

	#ifdef CONFIG_PHL_THERMAL_PROTECT
	phl_thermal_protect_watchdog(phl);
	#endif

	/* I/O, tx behavior, request power, ... */
	#ifdef CONFIG_PCI_HCI
	#ifdef RTW_WKARD_DYNAMIC_LTR
	phl_ltr_ctrl_watchdog(phl);
	#endif
	#ifdef PCIE_TRX_MIT_EN
	phl_pcie_trx_mit_watchdog(phl);
	#endif
	#endif

	phl_mr_watchdog(phl);
	rtw_hal_watchdog(phl->hal, phl->phl_com);
	phl_bcn_watchdog_hw(phl);
}

#ifdef CONFIG_CMD_DISP
static void _phl_watchdog_post_action(struct phl_info_t *phl_info)
{
#if defined(CONFIG_POWER_SAVE) && defined(CONFIG_HW_RADIO_ONOFF_DETECT)
	if (!phl_is_mp_mode(phl_info->phl_com))
		rtw_hal_ps_chk_hw_rf_state(phl_info->phl_com, phl_info->hal);
#endif /* CONFIG_POWER_SAVE */
}

static void _phl_trigger_next_watchdog(struct phl_info_t *phl_info)
{
	struct phl_watchdog *wdog = &(phl_info->wdog);


	if (wdog->state == WD_STATE_STARTED) {
#ifdef PHL_WATCHDOG_REFINE
		_phl_watchdog_sw_post(phl_info);
		if (NULL != wdog->core_post_sw_wdog) {
			wdog->core_post_sw_wdog(phl_to_drvpriv(phl_info));
		}
#endif
		_os_set_timer(phl_to_drvpriv(phl_info), &wdog->wdog_timer, wdog->period);
	}
}

static void _phl_watchdog_hw_done(void *drv_priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)cmd;

	_phl_watchdog_post_action(phl_info);
	_phl_trigger_next_watchdog(phl_info);
}

static enum rtw_phl_status
_phl_watchdog_hw_cmd(struct phl_info_t *phl_info,
                  enum phl_cmd_type cmd_type,
                  u32 cmd_timeout)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		phl_status = phl_watchdog_hw_cmd_hdl(phl_info, RTW_PHL_STATUS_SUCCESS);
		goto _exit;
	}

	/* watchdog dont care hw_band */
	phl_status = phl_cmd_enqueue(phl_info,
	                             HW_BAND_0,
	                             MSG_EVT_HW_WATCHDOG,
	                             (u8 *)phl_info,
	                             0,
	                             _phl_watchdog_hw_done,
	                             cmd_type,
	                             cmd_timeout);

	if (is_cmd_failure(phl_status)) {
		/* Send cmd success, but wait cmd fail*/
	} else if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		phl_status = RTW_PHL_STATUS_FAILURE;
	}


_exit:
	return phl_status;
}

static void _phl_watchdog_sw_done(void *drv_priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)cmd;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	bool set_timer = true;

	if (true == phl_disp_eng_is_fg_empty(phl_info, HW_BAND_MAX)) {

		/* send watchdog cmd to request privilege of I/O */
		psts = _phl_watchdog_hw_cmd(phl_info, PHL_CMD_NO_WAIT, 0);
		if (psts != RTW_PHL_STATUS_FAILURE)
			set_timer = false;
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s: skip watchdog\n",
		          __FUNCTION__);
	}

	if (set_timer)
		_phl_trigger_next_watchdog(phl_info);
}

static enum rtw_phl_status
_phl_watchdog_sw_cmd(struct phl_info_t *phl_info,
                  enum phl_cmd_type cmd_type,
                  u32 cmd_timeout)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		phl_status = phl_watchdog_sw_cmd_hdl(phl_info, RTW_PHL_STATUS_SUCCESS);
		goto _exit;
	}

	/* watchdog dont care hw_band */
	phl_status = phl_cmd_enqueue(phl_info,
	                             HW_BAND_0,
	                             MSG_EVT_SW_WATCHDOG,
	                             (u8 *)phl_info,
	                             0,
	                             _phl_watchdog_sw_done,
	                             cmd_type,
	                             cmd_timeout);

	if (is_cmd_failure(phl_status)) {
		/* Send cmd success, but wait cmd fail*/
	} else if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		phl_status = RTW_PHL_STATUS_FAILURE;
	}


_exit:
	return phl_status;
}
#endif

static void _phl_watchdog_timer_expired(void *context)
{
#ifdef CONFIG_CMD_DISP
	struct phl_info_t *phl_info = (struct phl_info_t *)context;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	bool set_timer = true;

	/* phl sw watchdog */
	_phl_watchdog_sw(phl_info);
	psts = _phl_watchdog_sw_cmd(phl_info, PHL_CMD_NO_WAIT, 0);
	if (psts != RTW_PHL_STATUS_FAILURE)
		set_timer = false;

	if (set_timer)
		_phl_trigger_next_watchdog(phl_info);
#else
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Not support watchdog\n", __FUNCTION__);
#endif
}

enum rtw_phl_status
phl_watchdog_hw_cmd_hdl(struct phl_info_t *phl_info, enum rtw_phl_status psts)
{
	struct phl_watchdog *wdog = &(phl_info->wdog);

	if (false == is_cmd_failure(psts)) {
		_phl_watchdog_hw(phl_info);

		if (NULL != wdog->core_hw_wdog)
			wdog->core_hw_wdog(phl_to_drvpriv(phl_info));
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_watchdog_sw_cmd_hdl(struct phl_info_t *phl_info, enum rtw_phl_status psts)
{
	struct phl_watchdog *wdog = &(phl_info->wdog);

	if (false == is_cmd_failure(psts)) {
		phl_bcn_watchdog_sw(phl_info);
		if (NULL != wdog->core_sw_wdog)
			wdog->core_sw_wdog(phl_to_drvpriv(phl_info));
	}

	return RTW_PHL_STATUS_SUCCESS;
}
#ifdef PHL_WATCHDOG_REFINE
void rtw_phl_watchdog_init(void *phl,
                           u16 period,
                           void (*core_sw_wdog)(void *drv_priv),
                           void (*core_hw_wdog)(void *drv_priv),
                           void (*core_sw_post_wdog)(void *drv_priv))
#else
void rtw_phl_watchdog_init(void *phl,
                           u16 period,
                           void (*core_sw_wdog)(void *drv_priv),
                           void (*core_hw_wdog)(void *drv_priv))
#endif
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_watchdog *wdog = &(phl_info->wdog);

	wdog->state = WD_STATE_INIT;
	wdog->core_sw_wdog = core_sw_wdog;
	wdog->core_hw_wdog = core_hw_wdog;
#ifdef PHL_WATCHDOG_REFINE
	wdog->core_post_sw_wdog = core_sw_post_wdog;
#endif

	if (period > 0)
		wdog->period = period;
	else
		wdog->period = WDOG_PERIOD;

	_os_init_timer(phl_to_drvpriv(phl_info),
	               &wdog->wdog_timer,
	               _phl_watchdog_timer_expired,
	               phl,
	               "phl_watchdog_timer");
}

void rtw_phl_watchdog_deinit(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_watchdog *wdog = &(phl_info->wdog);

	_os_release_timer(phl_to_drvpriv(phl_info), &wdog->wdog_timer);
}

void rtw_phl_watchdog_start(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_watchdog *wdog = &(phl_info->wdog);

	wdog->state = WD_STATE_STARTED;
	_phl_trigger_next_watchdog(phl_info);
}

void rtw_phl_watchdog_stop(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_watchdog *wdog = &(phl_info->wdog);

	PHL_INFO("%s\n", __func__);

	wdog->state = WD_STATE_STOP;

	_os_cancel_timer(phl_to_drvpriv(phl_info), &wdog->wdog_timer);
}
