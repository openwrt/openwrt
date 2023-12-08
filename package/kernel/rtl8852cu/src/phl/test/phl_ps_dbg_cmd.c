/******************************************************************************
 *
 * Copyright(c) 2019 - 2022 Realtek Corporation.
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
#define _PHL_PS_DBG_CMD_C_
#include "../phl_headers.h"
#ifdef CONFIG_POWER_SAVE
struct phl_ps_cmd_info {
	char name[16];
	u8 id;
};

enum PHL_PS_CMD_ID {
	PHL_PS_HELP,
	PHL_PS_SHOW,
	PHL_PS_FORCE_PS,
	PHL_PS_STOP_PS,
	PHL_PS_SET_PS_MODE,
	PHL_PS_SET_PS_CAP
};

struct phl_ps_cmd_info phl_ps_cmd_i[] = {
	{"-h", PHL_PS_HELP},
	{"show", PHL_PS_SHOW},
	{"force", PHL_PS_FORCE_PS},
	{"stop_ps", PHL_PS_STOP_PS},
	{"ps_mode", PHL_PS_SET_PS_MODE},
	{"ps_cap", PHL_PS_SET_PS_CAP}
};

/* echo phl ps show */
static void _phl_ps_cmd_show(struct phl_info_t *phl_info, u32 *used, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len)
{
	if (phl_info == NULL)
		return;

	phl_ps_dbg_dump(phl_info, used, input, input_num, output, out_len);
}

/* debug to disable power saving */
static void _phl_ps_cmd_stop_ps(struct phl_info_t *phl_info, u32 *used, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len)
{
	if (phl_info == NULL)
		return;

	phl_ps_dbg_stop_ps(phl_info, used, input, input_num, output, out_len);
}

static void _phl_ps_cmd_ps_mode(struct phl_info_t *phl_info, u32 *used, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len)
{
	if (phl_info == NULL)
		return;

	phl_ps_dbg_ps_op_mode(phl_info, used, input, input_num, output, out_len);
}

static void _phl_ps_cmd_ps_cap(struct phl_info_t *phl_info, u32 *used, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len)
{
	if (phl_info == NULL)
		return;

	phl_ps_dbg_ps_cap(phl_info, used, input, input_num, output, out_len);
}
static void _phl_ps_cmd_force_ps(struct phl_info_t *phl_info, u32 *used, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len)
{
	if (phl_info == NULL)
		return;

	phl_ps_dbg_force_ps(phl_info, used, input, input_num, output, out_len);
}

void phl_ps_cmd_parser(struct phl_info_t *phl_info, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len)
{
	u32 used = 0;
	u8 id = 0;
	u32 i;
	u32 array_size = sizeof(phl_ps_cmd_i) / sizeof(struct phl_ps_cmd_info);

	PS_CNSL(out_len, used, output + used, out_len - used, "\n");

	/* Parsing Cmd ID */
	if (input_num) {
		for (i = 0; i < array_size; i++) {
			if (!_os_strcmp(phl_ps_cmd_i[i].name, input[1])) {
				id = phl_ps_cmd_i[i].id;
				break;
			}
		}
	}

	switch (id) {
	case PHL_PS_FORCE_PS:
		_phl_ps_cmd_force_ps(phl_info, &used, input, input_num,
					output, out_len);
		break;
	case PHL_PS_SHOW:
		_phl_ps_cmd_show(phl_info, &used, input, input_num,
					output, out_len);
		break;
	case PHL_PS_STOP_PS:
		_phl_ps_cmd_stop_ps(phl_info, &used, input, input_num,
					output, out_len);
		break;
	case PHL_PS_SET_PS_MODE:
		_phl_ps_cmd_ps_mode(phl_info, &used, input, input_num,
					output, out_len);
		break;
	case PHL_PS_SET_PS_CAP:
		_phl_ps_cmd_ps_cap(phl_info, &used, input, input_num,
					output, out_len);
		break;
	default:
		PS_CNSL(out_len, used, output + used, out_len - used,
			 "command not supported !!\n");

		/* fall through */
	case PHL_PS_HELP:
		PS_CNSL(out_len, used, output + used, out_len - used,
			 "PS cmd ==>\n");

		for (i = 0; i < array_size - 1; i++)
			PS_CNSL(out_len, used, output + used, out_len - used,
				 " %s\n", phl_ps_cmd_i[i + 1].name);
		break;
	}
}

void phl_ps_dbg_dump(struct phl_info_t *phl_info, u32 *used,
	char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len)
{
	struct phl_module_op_info op_info = {0};
	struct phl_cmd_ps_basic_info info = {0};
	struct rtw_ps_cap_t *ps_cap = NULL;

	PS_CNSL(out_len, *used, output + *used, out_len - *used,
		"========== Basic Info ==========\n");

	op_info.op_code = PS_MDL_OP_BASIC_INFO;
	op_info.inbuf = (u8 *)&info;
#ifdef CONFIG_CMD_DISP
	if (phl_disp_eng_query_bk_module_info(phl_info, HW_BAND_0,
			PHL_MDL_POWER_MGNT, &op_info) != RTW_PHL_STATUS_SUCCESS)
		return;

	PS_CNSL(out_len, *used, output + *used, out_len - *used,
		"ps_mode: %s, cur_pwr_lvl: %s\
		\nap_active: %s, gc_active: %s\
		\ntx traffic lvl: %s, rx traffic lvl: %s\n",
		phl_ps_ps_mode_to_str(info.ps_mode), phl_ps_pwr_lvl_to_str(info.cur_pwr_lvl),
		(info.ap_active == true ? "yes" : "no"), (info.gc_active == true ? "yes" : "no"),
		phl_tfc_lvl_to_str(phl_info->phl_com->phl_stats.tx_traffic.lvl), phl_tfc_lvl_to_str(phl_info->phl_com->phl_stats.rx_traffic.lvl));
	PS_CNSL(out_len, *used, output + *used, out_len - *used,
		"Cur BcnTimeout: %d, cand BcnTimeout: %d\n",
		info.bcn_tracking_i.cur_tracking.bcn_timeout,
		info.bcn_tracking_i.cand_tracking.bcn_timeout);
	if (info.sta != NULL) {
		PS_CNSL(out_len, *used, output + *used, out_len - *used,
				"chnl: %d, rssi: %d, rssi_bcn: %d\n",
				info.sta->chandef.chan, rtw_hal_get_sta_rssi(info.sta), phl_get_min_rssi_bcn(phl_info));
	}

	ps_cap = _get_ps_cap(phl_info);

	PS_CNSL(out_len, *used, output + *used, out_len - *used,
		"========== Advanced Info ==========\n");

	PS_CNSL(out_len, *used, output + *used, out_len - *used,
		"last enter reason: %s\
		\nlast leave reason: %s\
		\nreject all pwr req: %s\
		\nbtc req pwr: %s\
		\nruntime stop reason: %d\
		\nrecovery count: %d\n",
		info.enter_rson,
		info.leave_rson,
		(info.rej_pwr_req == true ? "yes" : "no"),
		(info.btc_req_pwr == true ? "yes" : "no"),
		info.rt_stop_rson,
		info.recy_cnt);

	if (ps_cap->defer_para.defer_rson & PS_DEFER_PING_PKT) {
		PS_CNSL(out_len, *used, output + *used, out_len - *used,
		        "tx ping pass time(ms): %d\n", (int)phl_get_passing_time_ms(info.last_tx_ping_time));
	}
	if (ps_cap->defer_para.defer_rson & PS_DEFER_DHCP_PKT) {
		PS_CNSL(out_len, *used, output + *used, out_len - *used,
		        "tx dhcp pass time(ms): %d\n", (int)phl_get_passing_time_ms(info.last_tx_dhcp_time));
	}

	PS_CNSL(out_len, *used, output + *used, out_len - *used,
		"========== Capability ==========\n");

	PS_CNSL(out_len, *used, output + *used, out_len - *used,
		"init_rf_state: %s, init_rt_stop_rson: 0x%x, leave_fail_act: 0x%x\
		\nlps: %s, lps_cap: %s, lps_pause_tx: %d\
		\nawake_interval: %d, listen_bcn_mode: %d, smart_ps_mode: %d, bcnnohit: %d\
		\nrssi_enter_threshold: %d, rssi_leave_threshold: %d, rssi_diff_threshold: %d\
		\ndefer_rson: 0x%x, lps_ping_defer_time: %d(ms), lps_dhcp_defer_time: %d(ms)\
		\nlps_adv_cap: pvb_wait_rx (%s)\
		\nips: %s, ips_cap: %s\
		\nwowlan lps: %s, wowlan lps_cap: %s\
		\nwowlan lps awake_interval: %d, wowlan lps listen_bcn_mode: %d, wowlan lps smart_ps_mode: %d\n",
		(ps_cap->init_rf_state ? "off" : "on"), ps_cap->init_rt_stop_rson, ps_cap->leave_fail_act,
		phl_ps_op_mode_to_str(ps_cap->lps_en), phl_ps_pwr_lvl_to_str(phl_ps_judge_pwr_lvl(ps_cap->lps_cap, PS_MODE_LPS, true)), ps_cap->lps_pause_tx,
		ps_cap->lps_awake_interval, ps_cap->lps_listen_bcn_mode, ps_cap->lps_smart_ps_mode, ps_cap->lps_bcnnohit_en,
		ps_cap->lps_rssi_enter_threshold, ps_cap->lps_rssi_leave_threshold, ps_cap->lps_rssi_diff_threshold,
		ps_cap->defer_para.defer_rson, (int)ps_cap->defer_para.lps_ping_defer_time, (int)ps_cap->defer_para.lps_dhcp_defer_time,
	        ((ps_cap->lps_adv_cap & RTW_LPS_ADV_PVB_W_RX) ? "on" : "off"),
		phl_ps_op_mode_to_str(ps_cap->ips_en), phl_ps_pwr_lvl_to_str(phl_ps_judge_pwr_lvl(ps_cap->ips_cap, PS_MODE_IPS, true)),
		phl_ps_op_mode_to_str(ps_cap->lps_wow_en), phl_ps_pwr_lvl_to_str(phl_ps_judge_pwr_lvl(ps_cap->lps_wow_cap, PS_MODE_LPS, true)),
		ps_cap->lps_wow_awake_interval, ps_cap->lps_wow_listen_bcn_mode, ps_cap->lps_wow_smart_ps_mode);

#else
	PS_CNSL(out_len, *used, output + *used, out_len - *used,
		"Not support.\n");
#endif
}

void phl_ps_dbg_stop_ps(struct phl_info_t *phl_info, u32 *used,
	char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len)
{
	u32 stop = 0;
	bool ps_en = false;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;

	do {
		if (input_num < 3){
			PS_CNSL(out_len, *used, output + *used, out_len - *used,
					 "[DBG] echo phl ps stop_ps <0/1>\n");
			break;
		}

		if (!_get_hex_from_string(input[2], &stop))
			break;

		if ((phl_com->dev_sw_cap.ps_cap.ips_en != PS_OP_MODE_AUTO) &&
			(phl_com->dev_sw_cap.ps_cap.lps_en != PS_OP_MODE_AUTO)) {
				PS_CNSL(out_len, *used, output + *used, out_len - *used,
					 "Either ips or lps should be set PS_OP_MODE_AUTO\n");
				break;
		}

		if (stop)
			ps_en = false;
		else
			ps_en = true;

		rtw_phl_ps_set_rt_cap((void*)phl_info, HW_BAND_0, ps_en, PS_RT_DEBUG);

	} while (0);
}

static void _ps_dbg_cmd_done(void *priv, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;

	if (msg->inbuf && msg->inlen) {
		_os_mem_free(phl_to_drvpriv(phl_info),
			msg->inbuf, msg->inlen);
	}

}

void rtw_phl_dbg_ps_op_mode(void *phl, u8 band_idx, u8 ps_mode, u8 ps_op_mode)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	struct ps_mdl_dbg_info *dbg_info = NULL;

	dbg_info = (struct ps_mdl_dbg_info *)_os_mem_alloc(phl_to_drvpriv(phl_info),
				sizeof(struct ps_mdl_dbg_info));
	if (dbg_info == NULL) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to alloc memory.\n", __func__);
		return;
	}

	if (ps_mode == PS_MODE_LPS)
		dbg_info->op = SET_LPS_MODE;
	else if (ps_mode == PS_MODE_IPS)
		dbg_info->op = SET_IPS_MODE;
	else
		goto cmd_fail;

	dbg_info->val = ps_op_mode;

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_PS_DBG_CMD);
	msg.band_idx = band_idx;
	msg.inbuf = (u8*)dbg_info;
	msg.inlen = sizeof(*dbg_info);
	attr.completion.completion = _ps_dbg_cmd_done;
	attr.completion.priv = phl_info;

	if (phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL) !=
				RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to notify batter change.\n", __func__);
		goto cmd_fail;
	}

	return;

cmd_fail:
	_os_mem_free(phl_to_drvpriv(phl_info), dbg_info, sizeof(dbg_info));
}

void phl_ps_dbg_ps_op_mode(struct phl_info_t *phl_info, u32 *used,
	char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len)
{
	u32 op_mode = 0;
	u8 ps_mode = PS_MODE_NONE;

	do {
		if (input_num < 4) {
			PS_CNSL(out_len, *used, output + *used, out_len - *used,
					 "[DBG] echo phl ps_mode  <ips/lps> <0/1/2>\n");
			break;
		}

		if (!_os_strcmp(input[2], "lps"))
			ps_mode = PS_MODE_LPS;
		else if (!_os_strcmp(input[2], "ips"))
			ps_mode = PS_MODE_IPS;
		else
			break;

		if (!_get_hex_from_string(input[3], &op_mode))
			break;

		rtw_phl_dbg_ps_op_mode((void*)phl_info, HW_BAND_0, ps_mode, (u8)op_mode);

	} while (0);
}

void rtw_phl_dbg_ps_cap(void *phl, u8 band_idx, u8 ps_mode, u8 ps_cap)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	struct ps_mdl_dbg_info *dbg_info = NULL;

	dbg_info = (struct ps_mdl_dbg_info *)_os_mem_alloc(phl_to_drvpriv(phl_info),
				sizeof(struct ps_mdl_dbg_info));
	if (dbg_info == NULL) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to alloc memory.\n", __func__);
		return;
	}

	if (ps_mode == PS_MODE_LPS)
		dbg_info->op = SET_LPS_CAP;
	else
		goto cmd_fail;

	dbg_info->val = ps_cap;

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_PS_DBG_CMD);
	msg.band_idx = band_idx;
	msg.inbuf = (u8*)dbg_info;
	msg.inlen = sizeof(*dbg_info);
	attr.completion.completion = _ps_dbg_cmd_done;
	attr.completion.priv = phl_info;

	if (phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL) !=
				RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to notify batter change.\n", __func__);
		goto cmd_fail;
	}

	return;

cmd_fail:
	_os_mem_free(phl_to_drvpriv(phl_info), dbg_info, sizeof(dbg_info));
}

void phl_ps_dbg_ps_cap(struct phl_info_t *phl_info, u32 *used,
	char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len)
{
	u8 ps_mode = PS_MODE_NONE, ps_cap = 0;

	do {
		if (input_num < 4) {
			PS_CNSL(out_len, *used, output + *used, out_len - *used,
					 "[DBG] echo phl ps ps_cap <lps> <rfoff/cg/pg>\n");
			break;
		}

		/* Now, only support lps cap*/
		if (!_os_strcmp(input[2], "lps"))
			ps_mode = PS_MODE_LPS;
		else
			break;

		if (!_os_strcmp(input[3], "rfoff"))
			ps_cap = PS_CAP_PWRON | PS_CAP_RF_OFF;
		else if (!_os_strcmp(input[3], "cg"))
			ps_cap = PS_CAP_PWRON | PS_CAP_RF_OFF | PS_CAP_CLK_GATED;
		else if (!_os_strcmp(input[3], "pg"))
			ps_cap = PS_CAP_PWRON | PS_CAP_RF_OFF | PS_CAP_CLK_GATED | PS_CAP_PWR_GATED;
		else
			break;

		rtw_phl_dbg_ps_cap((void*)phl_info, HW_BAND_0, ps_mode, ps_cap);

	} while (0);
}

static void
_ps_dbg_force_ps(struct phl_info_t *phl_info, u8 ps_mode, bool enter)
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	struct ps_mdl_dbg_info *dbg_info = NULL;

	dbg_info = (struct ps_mdl_dbg_info *)_os_mem_alloc(phl_to_drvpriv(phl_info),
				sizeof(struct ps_mdl_dbg_info));
	if (dbg_info == NULL) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to alloc memory.\n", __func__);
		return;
	}

	if (ps_mode == PS_MODE_LPS)
		dbg_info->op = (enter ? FORCE_LPS_ENTER : FORCE_LPS_LEAVE);
	else if (ps_mode == PS_MODE_IPS)
		dbg_info->op = (enter ? FORCE_IPS_ENTER : FORCE_IPS_LEAVE);
	else
		goto end;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): set %s %s\n", __func__,
			phl_ps_ps_mode_to_str(ps_mode), (enter ? "enter" : "leave"));

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_PS_DBG_CMD);
	msg.band_idx = HW_BAND_0;
	msg.inbuf = (u8*)dbg_info;
	msg.inlen = sizeof(struct ps_mdl_dbg_info);
	attr.completion.completion = _ps_dbg_cmd_done;
	attr.completion.priv = phl_info;

	if (phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL) !=
				RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to send ps dbg cmd.\n", __func__);
		goto end;
	}

	return;

end:
	_os_mem_free(phl_to_drvpriv(phl_info), dbg_info, sizeof(struct ps_mdl_dbg_info));
}

void phl_ps_dbg_force_ps(struct phl_info_t *phl_info, u32 *used,
	char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len)
{
	u32 enter = 0;
	u8 ps_mode = PS_MODE_NONE;

	do {
		if (input_num < 4){
			PS_CNSL(out_len, *used, output + *used, out_len - *used,
					 "[DBG] echo phl ps force <ips/lps> <0/1>\n");
			break;
		}

		if (!_os_strcmp(input[2], "lps")) {
			ps_mode = PS_MODE_LPS;
		} else if (!_os_strcmp(input[2], "ips")) {
			ps_mode = PS_MODE_IPS;
		} else {
			break;
		}

		if (!_get_hex_from_string(input[3], &enter))
			break;

		_ps_dbg_force_ps(phl_info, ps_mode, (enter ? true : false));

	} while (0);
}

#endif /* CONFIG_POWER_SAVE */
