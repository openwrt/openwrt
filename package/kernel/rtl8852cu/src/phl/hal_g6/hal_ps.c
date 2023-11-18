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
#define _HAL_PS_C_
#include "hal_headers.h"
#ifdef CONFIG_POWER_SAVE
#define case_pwr_state(src) \
	case PS_PWR_STATE_##src: return #src
const char *hal_ps_pwr_state_to_str(u8 pwr_state)
{
	switch (pwr_state) {
	case_pwr_state(ACTIVE);
	case_pwr_state(BAND0_RFON);
	case_pwr_state(BAND1_RFON);
	case_pwr_state(BAND0_RFOFF);
	case_pwr_state(BAND1_RFOFF);
	case_pwr_state(CLK_GATED);
	case_pwr_state(PWR_GATED);
	case_pwr_state(MAX);
	default:
		return "Undefined";
	}
}

#define PS_PROTOCAL_LEAVE_TOLERANCE 50 /* ms */
#define PWR_LVL_CHANGE_TOLERANCE 50 /* ms */
#define MAX_CHK_PWR_STATE_CHANGE_CNT 5

static enum rtw_hal_status
_hal_ps_lps_chk_leave(struct hal_info_t *hal_info, u16 macid)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	u32 mac_sts = 0;
	u32 priv_mac_sts = 0;
	u32 start_time = _os_get_cur_time_ms();
	u32 pass_time = 0;

	do {
		status = rtw_hal_mac_lps_chk_leave(hal_info, macid, &mac_sts);
		if (status == RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_,
				"[HALPS], %s(): pass time = %d ms.\n",
				__func__, pass_time);
			break;
		}

		if (mac_sts != priv_mac_sts) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_,
				"[HALPS], %s(): mac status %u.\n",
				__func__, mac_sts);
			priv_mac_sts = mac_sts;
		}

		pass_time = phl_get_passing_time_ms(start_time);
		if (pass_time > PS_PROTOCAL_LEAVE_TOLERANCE)
			break;

		_os_sleep_us(hal_to_drvpriv(hal_info), 50);

	} while (1);

#ifdef CONFIG_PHL_PS_FW_DBG
	rtw_hal_fw_dbg_dump(hal_info);
#endif
	if (status != RTW_HAL_STATUS_SUCCESS)
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[HALPS], %s(): polling timeout!\n", __func__);

	return status;
}

static enum rtw_hal_status
_hal_ps_lps_cfg(struct hal_info_t *hal_info,
			struct rtw_hal_lps_info *lps_info)
{
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_,
		"[HALPS], %s(): mode(%d), listen bcn mode(%d), awake interval(%d), smart_ps_mode(%d), bcnnohit(%d).\n",
		__func__, lps_info->en, lps_info->listen_bcn_mode,
		lps_info->awake_interval, lps_info->smart_ps_mode,
		lps_info->bcnnohit_en);

	return rtw_hal_mac_lps_cfg(hal_info, lps_info);
}

static enum rtw_hal_status
_hal_ps_pwr_state_chk(struct hal_info_t *hal_info, u8 req_pwr_state)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	u32 mac_sts = 0;
	u32 priv_mac_sts = 0;
	u32 start_time = _os_get_cur_time_ms();
	u32 pass_time = 0;

	do {
		status = rtw_hal_mac_chk_pwr_state(hal_info, req_pwr_state, &mac_sts);
		if (status == RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_,
				"[HALPS], %s(): pass time = %d ms.\n",
				__func__, pass_time);
			break;
		}

		if (mac_sts != priv_mac_sts) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_,
				"[HALPS], %s(): mac status %u.\n",
				__func__, mac_sts);
			priv_mac_sts = mac_sts;
		}

		pass_time = phl_get_passing_time_ms(start_time);
		if (pass_time > PWR_LVL_CHANGE_TOLERANCE)
			break;

		_os_sleep_us(hal_to_drvpriv(hal_info), 50);

	} while (1);

#ifdef CONFIG_PHL_PS_FW_DBG
	rtw_hal_fw_dbg_dump(hal_info);
#endif

	if (status != RTW_HAL_STATUS_SUCCESS)
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[HALPS], %s(): polling timeout!\n", __func__);

	return status;
}

static enum rtw_hal_status
_hal_ps_cfg_pwr_state(struct hal_info_t *hal_info, u8 req_pwr_state)
{
#if (HAL_LPS_SKIP_HW_CFG == 0)
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	u8 cnt = 0;

	for (cnt = 0; cnt < MAX_CHK_PWR_STATE_CHANGE_CNT; cnt++) {

		/* rpwm */
		status = rtw_hal_mac_req_pwr_state(hal_info, req_pwr_state);
		if (status != RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_ERR_,
				"[HALPS], %s(): set pwr state %s fail!\n",
				__func__, hal_ps_pwr_state_to_str(req_pwr_state));
			return RTW_HAL_STATUS_FAILURE;
		}

		/* cpwm */
		status = _hal_ps_pwr_state_chk(hal_info, req_pwr_state);
		if (status == RTW_HAL_STATUS_SUCCESS)
			break;
	}

	if (cnt < MAX_CHK_PWR_STATE_CHANGE_CNT) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_,
				"[HALPS], %s(): chk pwr state %s success, cnt %d.\n",
				__func__, hal_ps_pwr_state_to_str(req_pwr_state), cnt);
		return RTW_HAL_STATUS_SUCCESS;
	} else {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_,
				"[HALPS], %s(): chk pwr state %s fail!\n",
				__func__, hal_ps_pwr_state_to_str(req_pwr_state));
		return RTW_HAL_STATUS_FAILURE;
	}
#else
	return RTW_HAL_STATUS_SUCCESS;
#endif
}

static enum rtw_hal_status
_hal_ps_cfg_pwr_on(struct rtw_phl_com_t *phl_com,
	struct hal_info_t *hal_info)
{
	struct rtw_phl_evt_ops *ops = &phl_com->evt_ops;
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;

	if (hal_info->hal_com->is_hal_init) {
		#if (HAL_LPS_SKIP_HW_CFG == 0)
		status = _hal_ps_cfg_pwr_state(hal_info, PS_PWR_STATE_ACTIVE);
		#else
		status = RTW_HAL_STATUS_SUCCESS;
		#endif
	} else {
		#if (HAL_IPS_SKIP_HW_CFG == 0)
		if (ops->set_rf_state(phlcom_to_drvpriv(phl_com), RTW_RF_ON) == true)
			status = RTW_HAL_STATUS_SUCCESS;
		else
			status = RTW_HAL_STATUS_FAILURE;
		#else
			status = RTW_HAL_STATUS_SUCCESS;
		#endif
	}

	return status;
}

static enum rtw_hal_status
_hal_ps_cfg_pwr_off(struct rtw_phl_com_t *phl_com,
	struct hal_info_t *hal_info)
{
	struct rtw_phl_evt_ops *ops = &phl_com->evt_ops;

	#if (HAL_IPS_SKIP_HW_CFG == 0)
	if (ops->set_rf_state(phlcom_to_drvpriv(phl_com), RTW_RF_OFF) == true)
		return RTW_HAL_STATUS_SUCCESS;
	else
		return RTW_HAL_STATUS_FAILURE;
	#else
		return RTW_HAL_STATUS_SUCCESS;
	#endif
}

static enum rtw_hal_status
_hal_ps_pwr_lvl_cfg(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal_info,
			u8 req_pwr_lvl)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;

	switch (req_pwr_lvl) {
	case PS_PWR_LVL_PWROFF:
		status = _hal_ps_cfg_pwr_off(phl_com, hal_info);
		break;
	case PS_PWR_LVL_PWR_GATED:
		status = _hal_ps_cfg_pwr_state(hal_info, PS_PWR_STATE_PWR_GATED);
		break;
	case PS_PWR_LVL_CLK_GATED:
		status = _hal_ps_cfg_pwr_state(hal_info, PS_PWR_STATE_CLK_GATED);
		break;
	case PS_PWR_LVL_RF_OFF:
		status = _hal_ps_cfg_pwr_state(hal_info, PS_PWR_STATE_BAND0_RFOFF);
		break;
	case PS_PWR_LVL_PWRON:
		status = _hal_ps_cfg_pwr_on(phl_com, hal_info);
		break;
	default:
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[HALPS], %s(): undefined pwr lvl!\n", __func__);
		break;
	}

	return status;
}

void _hal_ps_proc_hw_rf_state_done(void* priv, struct phl_msg* msg)
{
	struct rtw_phl_com_t *phl_com = (struct rtw_phl_com_t *)priv;

	if (msg->inbuf && msg->inlen) {
		_os_kmem_free(phlcom_to_drvpriv(phl_com), msg->inbuf, msg->inlen);
	}
}

/**
 * configured requested power level
 * return success if configure power level ok
 * @phl_com: see rtw_phl_com_t
 * @hal: see hal_info_t
 * @req_pwr_lvl: target power level to configured
 */
enum rtw_hal_status
rtw_hal_ps_pwr_lvl_cfg(struct rtw_phl_com_t *phl_com, void *hal,
			u8 req_pwr_lvl)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	status = _hal_ps_pwr_lvl_cfg(phl_com, hal_info, req_pwr_lvl);
	if (status != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[HALPS], %s(): pwr lvl cfg fail!\n", __func__);
	}

	return status;
}

/**
 * configure the legacy power save (protocol)
 * return configure lps fail or not
 * @hal: see hal_info_t
 * @lps: see rtw_hal_lps_info
 */
enum rtw_hal_status
rtw_hal_ps_lps_cfg(void *hal, struct rtw_hal_lps_info *lps_info)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	status = _hal_ps_lps_cfg(hal_info, lps_info);

	if (status == RTW_HAL_STATUS_SUCCESS) {
		if (lps_info->en == false)
			status = _hal_ps_lps_chk_leave(hal_info, lps_info->macid);
	}

	return status;
}

enum rtw_hal_status rtw_hal_ps_pwr_req(struct rtw_phl_com_t *phl_com, u8 src, bool pwr_req)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	status = phl_ps_hal_pwr_req(phl_com, src, pwr_req);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[HALPS], %s(): fail (pwr_req %d).\n", __func__, pwr_req);
		return RTW_HAL_STATUS_FAILURE;
	}

	return RTW_HAL_STATUS_SUCCESS;
}

static enum rtw_hal_status
_hal_ps_ips_chk_leave(struct hal_info_t *hal_info, u16 macid)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	u32 start_time = _os_get_cur_time_ms();
	u32 pass_time = 0;

	do {
		status = rtw_hal_mac_ips_chk_leave(hal_info, macid);
		if (status == RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_,
				"[HALPS], %s(): pass time = %d ms.\n",
				__func__, pass_time);
			break;
		}

		pass_time = phl_get_passing_time_ms(start_time);
		if (pass_time > PS_PROTOCAL_LEAVE_TOLERANCE)
			break;

		_os_sleep_us(hal_to_drvpriv(hal_info), 50);

	} while (1);

#ifdef CONFIG_PHL_PS_FW_DBG
	rtw_hal_fw_dbg_dump(hal_info);
#endif
	if (status != RTW_HAL_STATUS_SUCCESS)
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[HALPS], %s(): polling timeout!\n", __func__);

	return status;
}

enum rtw_hal_status rtw_hal_ps_ips_cfg(void *hal,
	struct rtw_hal_ips_info *ips_info)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	hstatus = rtw_hal_mac_ips_cfg(hal, ips_info->macid, ips_info->en);

	if (hstatus == RTW_HAL_STATUS_SUCCESS && !ips_info->en)
		hstatus = _hal_ps_ips_chk_leave(hal_info, ips_info->macid);

	return hstatus;
}

#ifdef CONFIG_HW_RADIO_ONOFF_DETECT
static void
_hal_ps_ntfy_hw_rf_state(struct rtw_phl_com_t *phl_com,
			enum rtw_rf_state rf_state)
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	void *d = phlcom_to_drvpriv(phl_com);
	enum rtw_rf_state *rf_ntfy = NULL;

	rf_ntfy = (enum rtw_rf_state *)_os_kmem_alloc(d, sizeof(*rf_ntfy));
	if (rf_ntfy == NULL) {
		PHL_ERR("[HALPS], %s(): alloc for ntfy fail.\n", __func__);
		return;
	}

	*rf_ntfy = rf_state;
	msg.inbuf = (u8 *)rf_ntfy;
	msg.inlen = sizeof(*rf_ntfy);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_HW_RF_CHG);
	attr.completion.completion = _hal_ps_proc_hw_rf_state_done;
	attr.completion.priv = phl_com;
	if (rtw_phl_msg_hub_hal_send(phl_com, &attr, &msg) !=
		RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("[HALPS], %s(): send msg failed\n", __func__);
		_os_kmem_free(d, rf_ntfy, sizeof(*rf_ntfy));
	}
}

void
rtw_hal_ps_chk_hw_rf_state(struct rtw_phl_com_t *phl_com, void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	enum rtw_rf_state rf_state = RTW_RF_ON;
	u8 val = 0;

	hstatus = rtw_hal_mac_get_wl_dis_val(hal_info, &val);
	if (hstatus != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[HALPS], %s(): get wl dis val fail, status: %d\n",
			  __func__, hstatus);
		return;
	}

	/* get new rf state */
	if (val == 1) {
		rf_state = RTW_RF_ON;
	} else if (val == 0) {
		rf_state = RTW_RF_OFF;
	} else {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[HALPS], %s(): wl_dis is invalid value: %d\n",
			  __func__, val);
		return;
	}

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[HALPS], %s(): rf state = %d\n",
		  __func__, rf_state);
	_hal_ps_ntfy_hw_rf_state(phl_com, rf_state);
}
#endif
void rtw_hal_ps_notify_wake(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	rtw_hal_mac_ps_notify_wake(hal_info);
}

enum rtw_hal_status
rtw_hal_lps_bcn_tracking_cfg(void *hal,
	struct rtw_bcn_tracking_cfg *cfg, struct rtw_bcn_tracking_cfg *cfg_cur)
{
	return rtw_hal_mac_lps_bcn_tracking_cfg(hal, cfg, cfg_cur);
}

void rtw_hal_lps_pvb_wait_rx(void *hal, u16 macid, bool pvb_wait_rx)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	rtw_hal_mac_lps_pvb_wait_rx(hal_info, macid, pvb_wait_rx);
}


#endif /* CONFIG_POWER_SAVE */
