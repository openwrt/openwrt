/******************************************************************************
 *
 * Copyright(c) 2021 Realtek Corporation.
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
#define _PHL_PS_C_
#include "phl_headers.h"
#ifdef CONFIG_POWER_SAVE
const char *phl_ps_op_mode_to_str(u8 op_mode)
{
	switch (op_mode) {
	case PS_OP_MODE_DISABLED:
		return "Disabled";
	case PS_OP_MODE_FORCE_ENABLED:
		return "Force Enabled";
	case PS_OP_MODE_AUTO:
		return "Auto";
	default:
		return "-";
	}
}

const char *phl_ps_ps_mode_to_str(u8 ps_mode)
{
	switch (ps_mode) {
	case PS_MODE_IPS:
		return "IPS";
	case PS_MODE_LPS:
		return "LPS";
	default:
		return "NONE";
	}
}

#define case_pwr_lvl(src) \
	case PS_PWR_LVL_##src: return #src
const char *phl_ps_pwr_lvl_to_str(u8 pwr_lvl)
{
	switch (pwr_lvl) {
	case_pwr_lvl(PWROFF);
	case_pwr_lvl(PWR_GATED);
	case_pwr_lvl(CLK_GATED);
	case_pwr_lvl(RF_OFF);
	case_pwr_lvl(PWRON);
	case_pwr_lvl(MAX);
	default:
		return "Undefined";
	}
}

u8 phl_ps_judge_pwr_lvl(u8 ps_cap, u8 ps_mode, u8 ps_en)
{
	if (!ps_en)
		return PS_PWR_LVL_PWRON;

	if (ps_mode == PS_MODE_IPS) {
		if (ps_cap & PS_CAP_PWR_OFF)
			return PS_PWR_LVL_PWROFF;
		else if (ps_cap & PS_CAP_PWR_GATED)
			return PS_PWR_LVL_PWR_GATED;
		else if (ps_cap & PS_CAP_CLK_GATED)
			return PS_PWR_LVL_CLK_GATED;
		else if (ps_cap & PS_CAP_RF_OFF)
			return PS_PWR_LVL_RF_OFF;
		else if (ps_cap & PS_CAP_PWRON)
			return PS_PWR_LVL_PWRON;
		else
			return PS_PWR_LVL_PWROFF; /* ips default support power off */
	} else if (ps_mode == PS_MODE_LPS) {
		if (ps_cap & PS_CAP_PWR_GATED)
			return PS_PWR_LVL_PWR_GATED;
		else if (ps_cap & PS_CAP_CLK_GATED)
			return PS_PWR_LVL_CLK_GATED;
		else if (ps_cap & PS_CAP_RF_OFF)
			return PS_PWR_LVL_RF_OFF;
		else if (ps_cap & PS_CAP_PWRON)
			return PS_PWR_LVL_PWRON;
		else
			return PS_PWR_LVL_PWRON; /* lps default support protocol */
	} else {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS], %s(): unknown ps mode!\n", __func__);
	}

	return PS_PWR_LVL_PWRON;
}

static void _ps_ntfy_before_pwr_cfg(struct phl_info_t *phl_info, u8 ps_mode,
			u8 cur_pwr_lvl, u8 req_pwr_lvl)
{
	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS], %s(): \n", __func__);

	if (cur_pwr_lvl == PS_PWR_LVL_PWRON) { /* enter ps */
		if (req_pwr_lvl == PS_PWR_LVL_PWROFF) {
			#ifdef CONFIG_BTCOEX
			rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_WL_OFF);
			#endif
			#if defined(CONFIG_PCI_HCI) && defined(RTW_WKARD_DYNAMIC_LTR)
			phl_ltr_sw_ctrl_ntfy(phl_info->phl_com, false);
			#endif
		} else if (req_pwr_lvl <= PS_PWR_LVL_RF_OFF) {
			#ifdef CONFIG_BTCOEX
			rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_FW_CTRL);
			#endif
			#if defined(CONFIG_PCI_HCI) && defined(RTW_WKARD_DYNAMIC_LTR)
			if (req_pwr_lvl == PS_PWR_LVL_PWR_GATED)
				phl_ltr_sw_ctrl_ntfy(phl_info->phl_com, false);
			#endif
		}
	}
}

static void _ps_ntfy_after_pwr_cfg(struct phl_info_t *phl_info, u8 ps_mode,
			u8 cur_pwr_lvl, u8 req_pwr_lvl, u8 cfg_ok)
{
	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS], %s(): \n", __func__);

	if (cur_pwr_lvl > req_pwr_lvl) { /* enter ps */
		if (!cfg_ok) { /* fail */
			if (req_pwr_lvl == PS_PWR_LVL_PWROFF) { /* driver ips */
				#ifdef CONFIG_BTCOEX
				rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_WL_ON);
				#endif
				#if defined(CONFIG_PCI_HCI) && defined(RTW_WKARD_DYNAMIC_LTR)
				phl_ltr_sw_ctrl_ntfy(phl_info->phl_com, true);
				#endif
			} else { /* fw control */
				#ifdef CONFIG_BTCOEX
				if (ps_mode == PS_MODE_LPS)
					rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_LPS_WL_ON);
				else
					rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_WL_ON);
				#endif
				#if defined(CONFIG_PCI_HCI) && defined(RTW_WKARD_DYNAMIC_LTR)
				phl_ltr_sw_ctrl_ntfy(phl_info->phl_com, true);
				#endif
			}
		}
	} else { /* leave ps */
		if (cfg_ok) { /* ok */
			if (cur_pwr_lvl == PS_PWR_LVL_PWROFF) { /* driver ips */
				if (req_pwr_lvl == PS_PWR_LVL_PWRON) {
					#ifdef CONFIG_BTCOEX
					rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_WL_ON);
					#endif
					#if defined(CONFIG_PCI_HCI) && defined(RTW_WKARD_DYNAMIC_LTR)
					phl_ltr_sw_ctrl_ntfy(phl_info->phl_com, true);
					#endif
				}
			} else { /* fw control */
				if (req_pwr_lvl == PS_PWR_LVL_PWRON) {
					#ifdef CONFIG_BTCOEX
					if (ps_mode == PS_MODE_LPS)
						rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_LPS_WL_ON);
					else
						rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_WL_ON);
					#endif
					#if defined(CONFIG_PCI_HCI) && defined(RTW_WKARD_DYNAMIC_LTR)
					phl_ltr_sw_ctrl_ntfy(phl_info->phl_com, true);
					#endif
				}
			}
		}
	}
}


enum rtw_phl_status
_phl_ps_ctrl_datapath(struct phl_info_t *phl_info, bool pause)
{
	struct phl_data_ctl_t ctl = {0};
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	ctl.id = PHL_MDL_POWER_MGNT;
	ctl.cmd = (pause == true ? PHL_DATA_CTL_SW_TX_PAUSE : PHL_DATA_CTL_SW_TX_RESUME);
	status = phl_data_ctrler(phl_info, &ctl, NULL);
	if (status != RTW_PHL_STATUS_SUCCESS)
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): control tx fail (pause %d).\n", __func__, pause);
	ctl.cmd = (pause == true ? PHL_DATA_CTL_SW_RX_PAUSE : PHL_DATA_CTL_SW_RX_RESUME);
	status = phl_data_ctrler(phl_info, &ctl, NULL);
	if (status != RTW_PHL_STATUS_SUCCESS)
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): control rx fail (pause %d).\n", __func__, pause);

	return status;
}

enum rtw_phl_status
phl_ps_lps_cfg_int(struct phl_info_t *phl_info, enum phl_ps_lps_int_cfg_step step)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_info->phl_com);
	struct rtw_phl_evt_ops *evt_ops = NULL;

	evt_ops = &phl_info->phl_com->evt_ops;

	if (step == LPS_LEAVE_CFG_INT_MAX)
		return RTW_PHL_STATUS_SUCCESS;

	/* check whether to config imr during lps */
	if (!hal_spec->lps_cfg_int)
		return RTW_PHL_STATUS_SUCCESS;

	switch (step) {
	case LPS_ENTER_CFG_INT_PRE_PHASE:
		/**
		 * 1. disable imr
		 * 2. stop datapath
		 */
		#ifdef CONFIG_SYNC_INTERRUPT
		evt_ops->set_interrupt_caps(phlcom_to_drvpriv(phl_info->phl_com), false);
		#else
		rtw_hal_disable_interrupt(phl_info->phl_com, phl_info->hal);
		#endif /* CONFIG_SYNC_INTERRUPT */

		_phl_ps_ctrl_datapath(phl_info, true);
		break;
	case LPS_ENTER_CFG_INT_POST_PHASE:
		/**
		 * 1. set imr using during low power
		 * 2. start datapath
		 * 3. enable imr
		 */
		rtw_hal_init_int_default_value(phl_info->phl_com, phl_info->hal, INT_SET_OPT_LPS_START);

		_phl_ps_ctrl_datapath(phl_info, false);

		#ifdef CONFIG_SYNC_INTERRUPT
		evt_ops->set_interrupt_caps(phlcom_to_drvpriv(phl_info->phl_com), true);
		#else
		rtw_hal_enable_interrupt(phl_info->phl_com, phl_info->hal);
		#endif /* CONFIG_SYNC_INTERRUPT */
		break;
	case LPS_LEAVE_CFG_INT_PRE_PHASE:
		/**
		 * 1. disable imr
		 * 2. stop datapath
		 */
		#ifdef CONFIG_SYNC_INTERRUPT
		evt_ops->set_interrupt_caps(phlcom_to_drvpriv(phl_info->phl_com), false);
		#else
		rtw_hal_disable_interrupt(phl_info->phl_com, phl_info->hal);
		#endif /* CONFIG_SYNC_INTERRUPT */

		_phl_ps_ctrl_datapath(phl_info, true);
		break;
	case LPS_LEAVE_CFG_INT_POST_PHASE:
		/**
		 * 1. set imr using after low power
		 * 2. start datapath
		 * 3. enable imr
		 */
		rtw_hal_init_int_default_value(phl_info->phl_com, phl_info->hal, INT_SET_OPT_LPS_STOP);

		_phl_ps_ctrl_datapath(phl_info, false);

		#ifdef CONFIG_SYNC_INTERRUPT
		evt_ops->set_interrupt_caps(phlcom_to_drvpriv(phl_info->phl_com), true);
		#else
		rtw_hal_enable_interrupt(phl_info->phl_com, phl_info->hal);
		#endif /* CONFIG_SYNC_INTERRUPT */
		break;
	default:
		PHL_ERR("%s(): unknown step!\n", __func__);
		break;
	}

	return status;
}

static enum phl_ps_lps_int_cfg_step
_judge_lps_cfg_int_step(u8 cur_pwr_lvl, u8 req_pwr_lvl, bool pre_phase)
{
	switch (cur_pwr_lvl) {
	case PS_PWR_LVL_PWRON: /* enter low power */
		if (req_pwr_lvl == PS_PWR_LVL_CLK_GATED || req_pwr_lvl == PS_PWR_LVL_PWR_GATED)
			return (pre_phase ? LPS_ENTER_CFG_INT_PRE_PHASE : LPS_ENTER_CFG_INT_POST_PHASE);
		break;
	case PS_PWR_LVL_CLK_GATED: /* leave low power */
	case PS_PWR_LVL_PWR_GATED:
		if (req_pwr_lvl == PS_PWR_LVL_PWRON)
			return (pre_phase ? LPS_LEAVE_CFG_INT_PRE_PHASE : LPS_LEAVE_CFG_INT_POST_PHASE);
		break;
	default:
		break;
	}

	return LPS_LEAVE_CFG_INT_MAX;
}

enum rtw_phl_status
phl_ps_cfg_pwr_lvl(struct phl_info_t *phl_info, u8 ps_mode, u8 cur_pwr_lvl, u8 req_pwr_lvl)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): from %s to %s.\n",
			__func__, phl_ps_pwr_lvl_to_str(cur_pwr_lvl), phl_ps_pwr_lvl_to_str(req_pwr_lvl));

	if (cur_pwr_lvl == req_pwr_lvl) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS], %s(): pwr lvl is not change!\n", __func__);
		return RTW_HAL_STATUS_SUCCESS;
	}

	_ps_ntfy_before_pwr_cfg(phl_info, ps_mode, cur_pwr_lvl, req_pwr_lvl);

	if (ps_mode == PS_MODE_LPS)
		phl_ps_lps_cfg_int(phl_info, _judge_lps_cfg_int_step(cur_pwr_lvl, req_pwr_lvl, true));

	hstatus = rtw_hal_ps_pwr_lvl_cfg(phl_info->phl_com, phl_info->hal,
				req_pwr_lvl);

	if (ps_mode == PS_MODE_LPS)
		phl_ps_lps_cfg_int(phl_info, _judge_lps_cfg_int_step(cur_pwr_lvl, req_pwr_lvl, false));

	_ps_ntfy_after_pwr_cfg(phl_info, ps_mode, cur_pwr_lvl, req_pwr_lvl,
						   (hstatus == RTW_HAL_STATUS_SUCCESS ? true : false));

	return (hstatus == RTW_HAL_STATUS_SUCCESS) ? RTW_PHL_STATUS_SUCCESS : RTW_PHL_STATUS_FAILURE;
}


static enum rtw_phl_status _ps_ntfy_before_lps_proto_cfg(
	struct phl_info_t *phl_info, u8 lps_en, u16 macid)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): \n", __func__);

	if (lps_en) { /* enter lps */
		#ifdef CONFIG_BTCOEX
		rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_LPS_WL_ON);
		#endif
		if (RTW_HAL_STATUS_SUCCESS !=
			rtw_hal_bb_lps_info_update(phl_info->hal, macid))
			return RTW_PHL_STATUS_FAILURE;
	}
	return status;
}

static void
_ps_ntfy_after_lps_proto_cfg(struct phl_info_t *phl_info, u8 lps_en, u8 cfg_ok)
{
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): \n", __func__);

	if (lps_en) { /* enter lps */
		if (!cfg_ok) { /* fail */
			#ifdef CONFIG_BTCOEX
			rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_WL_ON);
			#endif
		}
	} else { /* leave lps */
		if (cfg_ok) { /* ok */
			#ifdef CONFIG_BTCOEX
			rtw_hal_btc_radio_state_ntfy(phl_info->hal, BTC_RFCTRL_WL_ON);
			#endif
		}
	}
}

static void
_phl_lps_role_config_tbtt_agg(struct phl_info_t *phl_info,
                              struct rtw_wifi_role_t *cur_wrole,
                              u32 tbtt_agg_val)
{
	u8 role_idx, idx = 0;
	struct rtw_wifi_role_t *wrole;
	u32 tbtt_agg = tbtt_agg_val;

	if (cur_wrole == NULL) {
		PHL_ERR("%s cur role is NULL\n", __func__);
		return;
	}

	for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
		wrole = phl_get_wrole_by_ridx(phl_info, role_idx);
		if(wrole == NULL || !wrole->active)
			continue;

		if (wrole == cur_wrole)
			continue;

		PHL_INFO("%s role %d config tbtt agg = %d\n",
		         __func__, role_idx, tbtt_agg_val);

		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rtw_hal_role_cfg_ex(phl_info->hal, get_rlink(wrole, idx),
			                    PCFG_TBTT_AGG, &tbtt_agg);
		}
	}
}

static void
_phl_ips_role_config_tbtt_agg(struct phl_info_t *phl_info, u32 tbtt_agg_val)
{
	u8 role_idx, idx = 0;
	struct rtw_wifi_role_t *wrole;
	u32 tbtt_agg = tbtt_agg_val;

	for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
		wrole = phl_get_wrole_by_ridx(phl_info, role_idx);
		if (wrole == NULL || !wrole->active)
			continue;

		PHL_INFO("%s role %d config tbtt agg = %d\n", __func__, role_idx,
				 tbtt_agg_val);

		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rtw_hal_role_cfg_ex(phl_info->hal, get_rlink(wrole, idx),
			                    PCFG_TBTT_AGG, &tbtt_agg);
		}
	}
}

enum rtw_phl_status
phl_ps_ips_cfg(struct phl_info_t *phl_info, struct ps_cfg *cfg, u8 en)
{
	u32 tbtt_agg = en ? 0 : RTW_MAC_TBTT_AGG_DEF;
	struct rtw_hal_ips_info ips_info = {0};

	/* avoid waking up at each TBTT under disconnected standby */
	_phl_ips_role_config_tbtt_agg(phl_info, tbtt_agg);

	ips_info.en = en;
	ips_info.macid = cfg->macid;

	return rtw_hal_ps_ips_cfg(phl_info->hal, &ips_info);
}

enum rtw_phl_status
phl_ps_lps_cfg(struct phl_info_t *phl_info, struct ps_cfg *cfg, u8 en)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct rtw_hal_lps_info lps_info;
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;

	sta = rtw_phl_get_stainfo_by_macid(phl_info, cfg->macid);
	if (sta != NULL) {
		wrole = sta->wrole;
	} else {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS], %s(): cannot get sta!\n", __func__);
	}

	status = phl_snd_cmd_ntfy_ps(phl_info, wrole, en);
	if (RTW_PHL_STATUS_SUCCESS != status) {
		status = RTW_PHL_STATUS_FAILURE;
		return status;
	}

	if (en) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): enter lps, macid %d.\n", __func__, cfg->macid);
		_phl_lps_role_config_tbtt_agg(phl_info, wrole, 0);
	} else {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): leave lps, macid %d.\n", __func__, cfg->macid);
		_phl_lps_role_config_tbtt_agg(phl_info, wrole, RTW_MAC_TBTT_AGG_DEF);
	}

	lps_info.en = en;
	lps_info.macid = cfg->macid;
	lps_info.listen_bcn_mode = cfg->listen_bcn_mode;
	lps_info.awake_interval = cfg->awake_interval;
	lps_info.smart_ps_mode = cfg->smart_ps_mode;
	lps_info.bcnnohit_en = cfg->bcnnohit_en;

	if (RTW_PHL_STATUS_SUCCESS !=
		_ps_ntfy_before_lps_proto_cfg(phl_info, en, lps_info.macid)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): ntfy before lps fail.\n", __func__);
		status = RTW_PHL_STATUS_FAILURE;
		goto exit;
	}

	if (rtw_hal_ps_lps_cfg(phl_info->hal, &lps_info) != RTW_HAL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config lps fail.\n", __func__);
		rtw_hal_notification(phl_info->hal, MSG_EVT_DBG_TX_DUMP, HW_PHY_0);
		status = RTW_PHL_STATUS_FAILURE;
	}
exit:
	_ps_ntfy_after_lps_proto_cfg(phl_info, en, (status == RTW_PHL_STATUS_SUCCESS ? true : false));

	return status;
}

static enum rtw_phl_status _lps_enter_proto_cfg(struct phl_info_t *phl_info, struct ps_cfg *cfg)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_pkt_ofld_null_info null_info = {0};
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	void *d = phl_to_drvpriv(phl_info);

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): \n", __func__);

#ifdef CONFIG_PHL_PS_FW_DBG
	rtw_hal_en_fw_log(phl_info->hal, FL_COMP_PS, true);
#endif

	phl_sta = rtw_phl_get_stainfo_by_macid(phl_info, cfg->macid);
	if (phl_sta == NULL)
		return RTW_PHL_STATUS_FAILURE;

	_os_mem_cpy(d, &(null_info.a1[0]), &(phl_sta->mac_addr[0]),
		MAC_ADDRESS_LENGTH);

	_os_mem_cpy(d,&(null_info.a2[0]), &(phl_sta->wrole->mac_addr[0]),
			MAC_ADDRESS_LENGTH);

	_os_mem_cpy(d, &(null_info.a3[0]), &(phl_sta->mac_addr[0]),
			MAC_ADDRESS_LENGTH);

	status = rtw_phl_pkt_ofld_request(phl_info, cfg->macid,
				PKT_TYPE_NULL_DATA, cfg->token, &null_info, __func__);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): add null pkt ofld fail!\n", __func__);
		return status;
	}

	status = phl_ps_lps_cfg(phl_info, cfg, true);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config lps fail!\n", __func__);
		return status;
	}

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status _lps_leave_proto_cfg(struct phl_info_t *phl_info, struct ps_cfg *cfg)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): \n", __func__);

	status = phl_ps_lps_cfg(phl_info, cfg, false);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config lps fail!\n", __func__);
		return status;
	}

	status = rtw_phl_pkt_ofld_cancel(phl_info, cfg->macid,
					PKT_TYPE_NULL_DATA, cfg->token);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): del null pkt ofld fail!\n", __func__);
		return status;
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_ps_lps_proto_cfg(struct phl_info_t *phl_info, struct ps_cfg *cfg, bool lps_en)
{
	if (lps_en)
		return _lps_enter_proto_cfg(phl_info, cfg);
	else
		return _lps_leave_proto_cfg(phl_info, cfg);
}

enum rtw_phl_status phl_ps_lps_enter(struct phl_info_t *phl_info, struct ps_cfg *cfg)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (cfg->proto_cfg) {
		status = phl_ps_lps_proto_cfg(phl_info, cfg, true);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config lps protocol fail!\n", __func__);
			return status;
		}
	}

	if (cfg->pwr_cfg) {
		status = phl_ps_cfg_pwr_lvl(phl_info, cfg->ps_mode, cfg->cur_pwr_lvl, cfg->pwr_lvl);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config lps pwr lvl fail!\n", __func__);
			return status;
		}
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_ps_lps_leave(struct phl_info_t *phl_info, struct ps_cfg *cfg)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (cfg->pwr_cfg) {
		status = phl_ps_cfg_pwr_lvl(phl_info, cfg->ps_mode, cfg->cur_pwr_lvl, cfg->pwr_lvl);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config lps pwr lvl fail!\n", __func__);
			return status;
		}
	}

	if (cfg->proto_cfg) {
		status = phl_ps_lps_proto_cfg(phl_info, cfg, false);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config lps protocol fail!\n", __func__);
			return status;
		}
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_ps_ips_proto_cfg(struct phl_info_t *phl_info, struct ps_cfg *cfg, bool ips_en)
{
	/* ips protocol config */
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): \n", __func__);

#ifdef CONFIG_PHL_PS_FW_DBG
	rtw_hal_en_fw_log(phl_info->hal, FL_COMP_PS, (ips_en ? true : false));
#endif

	return phl_ps_ips_cfg(phl_info, cfg, ips_en);
}

enum rtw_phl_status phl_ps_ips_enter(struct phl_info_t *phl_info, struct ps_cfg *cfg)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (cfg->proto_cfg) {
		status = phl_ps_ips_proto_cfg(phl_info, cfg, true);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config ips protocol fail!\n", __func__);
			return status;
		}
	}

	if (cfg->pwr_cfg) {
		status = phl_ps_cfg_pwr_lvl(phl_info, cfg->ps_mode, cfg->cur_pwr_lvl, cfg->pwr_lvl);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config ips pwr lvl fail!\n", __func__);
			return status;
		}
	}

	return status;
}

enum rtw_phl_status phl_ps_ips_leave(struct phl_info_t *phl_info, struct ps_cfg *cfg)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if (cfg->pwr_cfg) {
		status = phl_ps_cfg_pwr_lvl(phl_info, cfg->ps_mode, cfg->cur_pwr_lvl, cfg->pwr_lvl);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config ips pwr lvl fail!\n", __func__);
			return status;
		}
	}

	if (cfg->proto_cfg) {
		status = phl_ps_ips_proto_cfg(phl_info, cfg, false);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS], %s(): config ips protocol fail!\n", __func__);
			return status;
		}
	}

	return status;
}

static void _ps_ntfy(struct phl_info_t *phl_info, struct ps_cfg *cfg, enum phl_msg_evt_id event)
{
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;

	sta = rtw_phl_get_stainfo_by_macid(phl_info, cfg->macid);
	if (sta != NULL)
		rlink = sta->rlink;
	else
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS], %s(): cannot get sta!\n", __func__);

	if (rlink != NULL)
		rtw_hal_notification(phl_info->hal, event, rlink->hw_band);
}

enum rtw_phl_status
phl_ps_enter_ps(struct phl_info_t *phl_info, struct ps_cfg *cfg)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): ps mode(%s), pwr lvl(%s), macid(%d), proto_cfg(%d), pwr_cfg(%d)\n",
			__func__, phl_ps_ps_mode_to_str(cfg->ps_mode), phl_ps_pwr_lvl_to_str(cfg->pwr_lvl),
			cfg->macid, cfg->proto_cfg, cfg->pwr_cfg);

	if (cfg->ps_mode == PS_MODE_LPS) {
		status = phl_ps_lps_enter(phl_info, cfg);
		if (status == RTW_PHL_STATUS_SUCCESS)
			_ps_ntfy(phl_info, cfg, MSG_EVT_PS_LPS_ENTER);
	} else if (cfg->ps_mode == PS_MODE_IPS) {
		status = phl_ps_ips_enter(phl_info, cfg);
	}

	return status;
}

enum rtw_phl_status
phl_ps_leave_ps(struct phl_info_t *phl_info, struct ps_cfg *cfg)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS], %s(): current ps mode(%s), pwr lvl(%s), macid(%d), proto_cfg(%d), pwr_cfg(%d)\n",
			__func__, phl_ps_ps_mode_to_str(cfg->ps_mode), phl_ps_pwr_lvl_to_str(cfg->pwr_lvl),
			cfg->macid, cfg->proto_cfg, cfg->pwr_cfg);

	if (cfg->ps_mode == PS_MODE_LPS) {
		status = phl_ps_lps_leave(phl_info, cfg);
		if (status == RTW_PHL_STATUS_SUCCESS)
			_ps_ntfy(phl_info, cfg, MSG_EVT_PS_LPS_LEAVE);
	} else if (cfg->ps_mode == PS_MODE_IPS) {
		status = phl_ps_ips_leave(phl_info, cfg);
	}

	return status;
}
#endif
