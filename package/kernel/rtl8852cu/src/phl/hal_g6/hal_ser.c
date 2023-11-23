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
#define _HAL_SER_C_
#include "hal_headers.h"

enum rtw_hal_status rtw_hal_ser_ctrl(void *hal, enum rtw_hal_ser_rsn rsn, bool en)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	hstatus = rtw_hal_mac_ser_ctrl(hal_info, rsn, en);

	return hstatus;
}

u32
rtw_hal_ser_get_error_status(void *hal, u32 *err)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum RTW_PHL_SER_NOTIFY_EVENT notify = RTW_PHL_SER_L2_RESET;

	rtw_hal_mac_ser_get_error_status(hal_info, err);

	if ((*err == MAC_AX_ERR_L1_ERR_DMAC) || (*err == MAC_AX_ERR_L0_PROMOTE_TO_L1)) {
		notify = RTW_PHL_SER_PAUSE_TRX;
	} else if (*err == MAC_AX_ERR_L1_RESET_DISABLE_DMAC_DONE) {
		notify = RTW_PHL_SER_DO_RECOVERY;
	} else if (*err == MAC_AX_ERR_L1_RESET_RECOVERY_DONE) {
		notify = RTW_PHL_SER_READY;
	} else if (*err < MAC_AX_ERR_L0_PROMOTE_TO_L1) {
		notify = RTW_PHL_SER_L0_RESET;
	} else if (*err == MAC_AX_DUMP_SHAREBUFF_INDICATOR) {
		notify = RTW_PHL_SER_DUMP_FW_LOG;
	} else if (*err == MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WCMAC) {
		notify = RTW_PHL_SER_LOG_ONLY;
	} else if ((*err == MAC_AX_ERR_L1_PROMOTE_TO_L2) ||
			  ((*err >= MAC_AX_ERR_L2_ERR_AH_DMA) && (*err <= MAC_AX_GET_ERR_MAX))) {
		notify = RTW_PHL_SER_L2_RESET;
	}

	return notify;
}

enum rtw_hal_status rtw_hal_ser_set_error_status(void *hal, u32 err)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_ser_set_error_status(hal_info, err);
}

bool rtw_hal_ser_chk_ser_l1(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_ser_chk_ser_l1(hal_info);
}

enum rtw_hal_status rtw_hal_trigger_cmac_err(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_trigger_cmac_err(hal_info);
}

enum rtw_hal_status rtw_hal_trigger_dmac_err(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_trigger_dmac_err(hal_info);
}

enum rtw_hal_status rtw_hal_lv1_rcvy(void *hal, u32 step)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "===> rtw_hal_lv1_rcvy step %d\n", step);
	status = rtw_hal_mac_lv1_rcvy(hal_info, step);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<=== rtw_hal_lv1_rcvy step %d, status 0x%x\n", step, status);

	return status;
}

void rtw_hal_dump_fw_rsvd_ple(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u32 mac_err;
	mac_err = rtw_hal_mac_dump_fw_rsvd_ple(hal_info);
}

void
rtw_hal_ser_reset_wdt_intr(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u32 mac_err;
	mac_err = rtw_hal_mac_ser_reset_wdt_intr(hal_info);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "rtw_hal_ser_reset_wdt_intr status 0x%x\n",mac_err);
}

void rtw_hal_ser_int_cfg(void *hal, struct rtw_phl_com_t *phl_com,
						 enum RTW_PHL_SER_CFG_STEP step)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_com->evt_ops;
#endif /* CONFIG_SYNC_INTERRUPT */
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_com);

	/* check whether to config imr during ser */
	if (!hal_spec->ser_cfg_int)
		return;

	switch (step) {
	case RTW_PHL_SER_M1_PRE_CFG:
		/**
		 * 1. disable imr
		 * 2. set imr used during ser
		 */
		#ifdef CONFIG_SYNC_INTERRUPT
		evt_ops->set_interrupt_caps(phlcom_to_drvpriv(phl_com), false);
		#else
		if (hal_ops->disable_interrupt)
			hal_ops->disable_interrupt(hal);
		#endif /* CONFIG_SYNC_INTERRUPT */

		if (hal_ops->init_int_default_value)
			hal_ops->init_int_default_value(hal, INT_SET_OPT_SER_START);
		break;
	case RTW_PHL_SER_M1_POST_CFG:
		/**
		 * 1. enable interrupt
		 */
		#ifdef CONFIG_SYNC_INTERRUPT
		evt_ops->set_interrupt_caps(phlcom_to_drvpriv(phl_com), true);
		#else
		if (hal_ops->enable_interrupt)
			hal_ops->enable_interrupt(hal);
		#endif /* CONFIG_SYNC_INTERRUPT */
		break;
	case RTW_PHL_SER_M5_CFG:
		/**
		 * 1. disable interrupt
		 * 2. set imr used after ser
		 * 3. enable interrupt
		 */
		#ifdef CONFIG_SYNC_INTERRUPT
		evt_ops->set_interrupt_caps(phlcom_to_drvpriv(phl_com), false);
		#else
		if (hal_ops->disable_interrupt)
			hal_ops->disable_interrupt(hal);
		#endif /* CONFIG_SYNC_INTERRUPT */

		if (hal_ops->init_int_default_value)
			hal_ops->init_int_default_value(hal, INT_SET_OPT_SER_DONE);

		#ifdef CONFIG_SYNC_INTERRUPT
		evt_ops->set_interrupt_caps(phlcom_to_drvpriv(phl_com), true);
		#else
		if (hal_ops->enable_interrupt)
			hal_ops->enable_interrupt(hal);
		#endif /* CONFIG_SYNC_INTERRUPT */
		break;
	default:
		PHL_ERR("%s(): unknown step!\n", __func__);
	}
}
