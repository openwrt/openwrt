/******************************************************************************
 *
 * Copyright(c) 2022 Realtek Corporation.
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
#define _PHL_CMD_PS_C_
#include "phl_headers.h"
#ifdef CONFIG_POWER_SAVE
/* structure of a power request */
struct pwr_req {
	_os_list list;
	u16 evt_id;
};

struct rt_ps {
	enum phl_ps_rt_rson rt_rson;
	bool ps_allow;
};

/**
 * +----------------------+-------------------+-----------------------+
 * |       ps_mode        |      ps_state     |        pwr_lvl        |
 * +----------------------+-------------------+-----------------------+
 * |     PS_MODE_LPS      |  PS_STATE_LEAVED  |  PS_PWR_LVL_PWRON     |
 * |                      +-------------------+-----------------------+
 * |                      |  PS_STATE_PROTO   |  PS_PWR_LVL_PWRON     |
 * |                      +-------------------+-----------------------+
 * |                      |  PS_STATE_ENTERED |  PS_PWR_LVL_PWR_GATED |
 * |                      |                   |  PS_PWR_LVL_CLK_GATED |
 * |                      |                   |  PS_PWR_LVL_RF_OFF    |
 * +----------------------+-------------------+-----------------------+
 * |     PS_MODE_IPS      |  PS_STATE_LEAVED  |  PS_PWR_LVL_PWRON     |
 * |                      +-------------------+-----------------------+
 * |                      |  PS_STATE_ENTERED |  PS_PWR_LVL_PWROFF    |
 * |                      |                   |  PS_PWR_LVL_PWR_GATED |
 * |                      |                   |  PS_PWR_LVL_CLK_GATED |
 * |                      |                   |  PS_PWR_LVL_RF_OFF    |
 * +----------------------+-------------------+-----------------------+
 */

enum {
	PS_STATE_NONE = 0,
	PS_STATE_LEAVED,
	/**
	 * lps: protocol only
	 * ips: won't in this state
	 */
	PS_STATE_PROTO,
	/**
	 * lps: protocol + power
	 * ips: power
	 */
	PS_STATE_ENTERED
};

static const char *_ps_state_to_str(u8 ps_state)
{
	switch (ps_state) {
	case PS_STATE_ENTERED:
		return "PS_ENTERED";
	case PS_STATE_PROTO:
		return "PS_PROTOCOL";
	case PS_STATE_LEAVED:
		return "PS_LEAVED";
	default:
		return "NONE";
	}
}

struct _ps_mr_info {
	bool ap_active;
	bool gc_active;
};

struct _ps_recovery_info {
	bool in_recovery;
	bool wdg_leave;
	u8 recy_cnt;
};

struct _ps_time_info {
	/* last tx pint time */
	u32 last_tx_ping_time;
	/* reserved for dhcp */
	u32 last_tx_dhcp_time;
};

struct _lps_adv_cfg {
	bool pvb_wait_rx;
};

#define case_defer_rson(src) \
		case PS_DEFER_##src: return #src
const char *_defer_rson_to_str(u8 defer_rson)
{
	switch (defer_rson) {
	case_defer_rson(DHCP_PKT);
	case_defer_rson(PING_PKT);
	default:
		return "Undefined";
	}
}

#define CMD_PS_TIMER_PERIOD 100
#define MAX_PWE_REQ_NUM 16
struct cmd_ps {
	struct phl_info_t *phl_info;
	void *dispr;
	_os_timer ps_timer;

	struct phl_queue req_busy_q;
	struct phl_queue req_idle_q;
	struct pwr_req req_pool[MAX_PWE_REQ_NUM];

	bool rej_pwr_req; /* reject all pwr request */
	bool btc_req_pwr; /* btc request pwr or not */
	bool stop_datapath;

	/* current state */
	u8 cur_pwr_lvl;
	u8 ps_state;
	u8 ps_mode;
	char enter_rson[MAX_CMD_PS_RSON_LENGTH];
	char leave_rson[MAX_CMD_PS_RSON_LENGTH];
	struct bcn_tracking_info bcn_tracking_i;
	struct rtw_phl_stainfo_t *sta; /* sta entering/leaving ps */
	u16 macid;

	/* lps */
	u32 null_token;
	bool wdg_leave_ps;

	struct _ps_time_info time_info;

	/* refs. enum "phl_ps_rt_rson" */
	enum phl_ps_rt_rson rt_stop_rson;

	struct _ps_mr_info mr_info;
	struct _lps_adv_cfg cur_adv_cfg;

	/* rssi */
	u8 rssi_bcn_min;

	struct _ps_recovery_info recy_info;
};

/**
 * determine leave lps or not
 * return true if rssi variation reach threshold
 * @ps: see cmd_ps
 */
static bool _chk_rssi_diff_reach_thld(struct cmd_ps *ps)
{
	struct phl_info_t *phl_info = ps->phl_info;
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);
	u8 cur_rssi_bcn_min = 0;
	u8 *rssi_bcn_min = &ps->rssi_bcn_min;
	bool leave_ps = false;

	cur_rssi_bcn_min = phl_get_min_rssi_bcn(phl_info);

	do {
		if (*rssi_bcn_min == 0 || *rssi_bcn_min == 0xFF) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): (criteria = %d, cur = %d) criteria invalid, set criteria to cur\n",
					__func__, *rssi_bcn_min, cur_rssi_bcn_min);
			*rssi_bcn_min = cur_rssi_bcn_min; /* update with current_rssi */
			break;
		}

		if (DIFF(*rssi_bcn_min, cur_rssi_bcn_min) < ps_cap->lps_rssi_diff_threshold) {
			PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): (criteria = %d, cur = %d) RSSI diff < %d, do nothing\n",
					__func__, *rssi_bcn_min, cur_rssi_bcn_min, ps_cap->lps_rssi_diff_threshold);
			break;
		}

		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): leave ps and update criteria from %d to %d\n", __func__, *rssi_bcn_min, cur_rssi_bcn_min);

		leave_ps = true;
		*rssi_bcn_min = cur_rssi_bcn_min;
	} while (0);

	return leave_ps;
}

/**
 * determine leave lps or not
 * return true if beacon offset changed
 * @ps: see cmd_ps
 */
bool _chk_bcn_offset_changed(struct cmd_ps *ps)
{
	struct phl_info_t *phl_info = ps->phl_info;
	u8 ridx = MAX_WIFI_ROLE_NUMBER;
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_bcn_offset *b_ofst_i = NULL;
	bool leave_ps = false;
	u8 lidx = 0;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		wrole = phl_get_wrole_by_ridx(phl_info, ridx);
		if (wrole == NULL)
			continue;

		if (rtw_phl_role_is_client_category(wrole) && wrole->mstate == MLME_LINKED) {
			for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
				b_ofst_i = phl_get_sta_bcn_offset_info(phl_info, &wrole->rlink[lidx]);

				if (b_ofst_i->conf_lvl >= CONF_LVL_MID &&
					b_ofst_i->offset != b_ofst_i->cr_tbtt_shift) {
					PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): update ridx(%d) bcn offset from %d to %d TU\n",
								__func__, ridx, b_ofst_i->cr_tbtt_shift, b_ofst_i->offset);
					leave_ps = true;
					goto _exit;
				}
			}
		}
	}

_exit:
	return leave_ps;
}

/**
 * check whether to leave/enter lps
 * return true if ps need to change to the target state
 * @ps: see cmd_ps
 * @mac_id: macid of corresponding sta
 * @cur_state: currently lps state
 * @target_state: the target ps state
 */
static bool
_lps_state_judge_changed(struct cmd_ps *ps, u16 macid, u8 cur_state, u8 target_state)
{
	struct phl_info_t *phl_info = ps->phl_info;
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);
	struct rtw_stats *phl_stats = &phl_info->phl_com->phl_stats;
	struct rtw_phl_stainfo_t *sta = NULL;
	bool change_state = false;
	u8 rssi = 0;

	sta = rtw_phl_get_stainfo_by_macid(phl_info, macid);
	if (sta == NULL) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): cannot get sta of macid %d.\n", __func__, macid);
		return false;
	}

	rssi = rtw_hal_get_sta_rssi(sta);

	if (target_state == PS_STATE_ENTERED) {
		if (cur_state == PS_STATE_LEAVED || cur_state == PS_STATE_PROTO) {
			if (rssi > ps_cap->lps_rssi_enter_threshold &&
				phl_stats->tx_traffic.lvl == RTW_TFC_IDLE &&
				phl_stats->rx_traffic.lvl == RTW_TFC_IDLE) {
				change_state = true;
			}
		}
	} else {
		if (cur_state == PS_STATE_ENTERED || cur_state == PS_STATE_PROTO) {
			if (_chk_rssi_diff_reach_thld(ps) ||
				_chk_bcn_offset_changed(ps) ||
				rssi < ps_cap->lps_rssi_leave_threshold ||
				phl_stats->tx_traffic.lvl != RTW_TFC_IDLE ||
				phl_stats->rx_traffic.lvl != RTW_TFC_IDLE) {
				change_state = true;
			}
		}
	}

	if (change_state) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): state %s -> %s, Tx(%s), Rx(%s), RSSI(%d)\n",
				  __func__, _ps_state_to_str(cur_state), _ps_state_to_str(target_state),
				  phl_tfc_lvl_to_str(phl_stats->tx_traffic.lvl),
				  phl_tfc_lvl_to_str(phl_stats->rx_traffic.lvl), rssi);
	}

	return change_state;
}

/**
 * check whether to enter lps
 * return true if related defer rson is set and check related time info
 * @ps: see cmd_ps
 */
static bool
_ps_chk_defer(struct cmd_ps *ps)
{
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);
	u32 pass_time = 0;

	if (!ps_cap->defer_para.defer_rson)
		return false;

	if (ps_cap->defer_para.defer_rson & PS_DEFER_PING_PKT) {
		pass_time = phl_get_passing_time_ms(ps->time_info.last_tx_ping_time);
		if (pass_time < ps_cap->defer_para.lps_ping_defer_time)
			return true;
	}

	if (ps_cap->defer_para.defer_rson & PS_DEFER_DHCP_PKT) {
		pass_time = phl_get_passing_time_ms(ps->time_info.last_tx_dhcp_time);
		if (pass_time < ps_cap->defer_para.lps_dhcp_defer_time)
			return true;
	}

	return false;
}

static void _set_ps_rson(struct cmd_ps *ps, u8 enter, char *rson)
{
	if (enter) {
		_os_mem_set(phl_to_drvpriv(ps->phl_info), ps->enter_rson, 0, MAX_CMD_PS_RSON_LENGTH);
		_os_mem_cpy(phl_to_drvpriv(ps->phl_info), ps->enter_rson, rson, MAX_CMD_PS_RSON_LENGTH);
	} else {
		_os_mem_set(phl_to_drvpriv(ps->phl_info), ps->leave_rson, 0, MAX_CMD_PS_RSON_LENGTH);
		_os_mem_cpy(phl_to_drvpriv(ps->phl_info), ps->leave_rson, rson, MAX_CMD_PS_RSON_LENGTH);
	}
}

static void _ps_cofig_pvb_wait_rx(struct cmd_ps *ps, bool pvb_wait_rx)
{
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);

	if (!(ps_cap->lps_adv_cap & RTW_LPS_ADV_PVB_W_RX))
		return;

	if (ps->ps_mode == PS_MODE_LPS && ps->ps_state != PS_STATE_LEAVED) {
		if (ps->cur_adv_cfg.pvb_wait_rx == pvb_wait_rx) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): Already %s PVB wait for rx mode, skip config\n",
			          __func__, (pvb_wait_rx) ? "Enter" : "Leave");
			return;
		}

		rtw_hal_lps_pvb_wait_rx(ps->phl_info->hal, ps->macid,
		                        pvb_wait_rx);
		ps->cur_adv_cfg.pvb_wait_rx = pvb_wait_rx;
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): %s PVB wait for rx mode\n",
		          __func__, (pvb_wait_rx) ? "Enter" : "Leave");
	}

	return;
}

/**
 * Leave power saving
 * return RTW_PHL_STATUS_SUCCESS if leave ps ok
 * @ps: see cmd_ps
 * @leave_proto: whether to leave protocol
 * @rson: the reason of leaving power saving
 */
enum rtw_phl_status _leave_ps(struct cmd_ps *ps, bool leave_proto, char *rson)
{
	struct phl_info_t *phl_info = ps->phl_info;
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct ps_cfg cfg = {0};

	if (ps->ps_state == PS_STATE_LEAVED) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): not in power saving.\n", __func__);
		return RTW_PHL_STATUS_SUCCESS;
	}

	cfg.cur_pwr_lvl = ps->cur_pwr_lvl;

	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): leave %s (macid %d).\n",
			  __func__, phl_ps_ps_mode_to_str(ps->ps_mode), ps->macid);

	if (ps->ps_mode == PS_MODE_LPS) {
		if (!leave_proto) {
			if (ps->ps_state == PS_STATE_PROTO) {
				PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): in lps protocal only.\n", __func__);
				return RTW_PHL_STATUS_SUCCESS;
			}
			cfg.pwr_cfg = true;
			cfg.proto_cfg = false;
		} else {
			if (ps->ps_state == PS_STATE_PROTO) {
				cfg.pwr_cfg = false;
				cfg.proto_cfg = true;
			} else {
				cfg.pwr_cfg = true;
				cfg.proto_cfg = true;
			}
		}
		cfg.macid = ps->macid;
		cfg.token = &ps->null_token;
		cfg.pwr_lvl = PS_PWR_LVL_PWRON;
		cfg.ps_mode = ps->ps_mode;
	} else if (ps->ps_mode == PS_MODE_IPS) {
		cfg.macid = ps->macid;
		cfg.pwr_lvl = PS_PWR_LVL_PWRON;
		cfg.proto_cfg = (cfg.cur_pwr_lvl == PS_PWR_LVL_PWROFF) ? false : true;
		cfg.pwr_cfg = true;
		cfg.ps_mode = ps->ps_mode;
	} else {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): unknown ps mode!\n", __func__);
	}

	_set_ps_rson(ps, false, rson);

	if (cfg.proto_cfg)
		_ps_cofig_pvb_wait_rx(ps, false);

	status = phl_ps_leave_ps(phl_info, &cfg);

	if (status == RTW_PHL_STATUS_SUCCESS) {
		ps->cur_pwr_lvl = cfg.pwr_lvl;
		if (ps->ps_mode == PS_MODE_LPS) {
			ps->ps_mode = (leave_proto != true) ? PS_MODE_LPS : PS_MODE_NONE;
			ps->ps_state = (leave_proto != true) ? PS_STATE_PROTO : PS_STATE_LEAVED;
			ps->macid = (leave_proto != true) ? ps->macid : PS_MACID_NONE;
		} else {
			ps->ps_mode = PS_MODE_NONE;
			ps->ps_state = PS_STATE_LEAVED;
			ps->macid = PS_MACID_NONE;
		}
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): macid %d leave ps success, reason(%s).\n", __func__, cfg.macid, rson);
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): ps mode(%s), pwr lvl(%s), ps state(%s)\n",
				__func__, phl_ps_ps_mode_to_str(ps->ps_mode),
				phl_ps_pwr_lvl_to_str(ps->cur_pwr_lvl), _ps_state_to_str(ps->ps_state));
	} else {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): leave ps fail! reason(%s).\n", __func__, rson);
	}

	return status;
}


static void _cfg_bcn_tracking(struct cmd_ps *ps, bool reset)
{
	struct rtw_bcn_tracking_cfg *cfg_new = NULL, *cfg_cur = NULL;
	struct rtw_bcn_tracking_cfg *cfg_cand = NULL;
	struct rtw_bcn_tracking_cfg cfg = {0};

	if (_get_ps_cap(ps->phl_info)->bcn_tracking == false)
		goto _exit;
	if (ps->ps_mode != PS_MODE_LPS)
		goto _exit;
	if (reset)
		_os_mem_set(phl_to_drvpriv(ps->phl_info), &ps->bcn_tracking_i,
			    0, sizeof(struct bcn_tracking_info));
	cfg_new = &ps->sta->bcn_i.cfg;
	cfg_cur = &ps->bcn_tracking_i.cur_tracking;
	cfg_cand = &ps->bcn_tracking_i.cand_tracking;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): cfg_new: macid(%d), bcn_timeout(%d)\n",
		__func__, cfg_new->macid, cfg_new->bcn_timeout);
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): cfg_cur: macid(%d), bcn_timeout(%d)\n",
		__func__, cfg_cur->macid, cfg_cur->bcn_timeout);
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): cfg_cand: macid(%d), bcn_timeout(%d)\n",
		__func__, cfg_cand->macid, cfg_cand->bcn_timeout);
	if (cfg_new->bcn_timeout == cfg_cur->bcn_timeout) {
		/* renew cfg_cand */
		if (cfg_cand->bcn_timeout > 0) {
			cfg_cand->macid = cfg_new->bcn_timeout;
			cfg_cand->bcn_timeout = cfg_new->bcn_timeout;
		}
		goto _exit;
	}
	if (cfg_new->bcn_timeout > cfg_cur->bcn_timeout) {
		cfg.bcn_timeout = cfg_new->bcn_timeout;
		goto _cfg;
	}
	/* the following is cfg_new->bcn_timeout < cfg_cur->bcn_timeout */
	if (cfg_cand->bcn_timeout == 0) {
		/* up cfg_cand */
		cfg_cand->bcn_timeout = cfg_new->bcn_timeout;
		goto _exit;
	}
	if (cfg_new->bcn_timeout < cfg_cand->bcn_timeout) {
		cfg.bcn_timeout = (cfg_cand->bcn_timeout +
				   cfg_cur->bcn_timeout) / 2;
	} else {
		cfg.bcn_timeout = (cfg_new->bcn_timeout +
				   cfg_cur->bcn_timeout) / 2;
	}
_cfg:
	cfg.macid = ps->macid;
	cfg.bcn_timeout += 1; /* round up */
	/* reset cfg_cand */
	cfg_cand->bcn_timeout = 0;
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): Need to change setting: macid(%d), bcn_timeout(%d)\n",
		__func__, cfg.macid, cfg.bcn_timeout);
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_lps_bcn_tracking_cfg(
					ps->phl_info->hal, &cfg, cfg_cur)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): cfg bcn tracking fail.\n", __func__);
	} else {
		cfg_cur->bcn_timeout = cfg.bcn_timeout;
		cfg_cur->macid = cfg.macid;
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): cfg bcn tracking ok.\n", __func__);
	}
_exit:
	return;
}

/**
 * Enter power saving
 * return RTW_PHL_STATUS_SUCCESS if enter ps ok
 * @ps: see cmd_ps
 * @ps_mode: target ps mode to enter
 * @macid: target macid to enter lps
 * @rson: the reason of entering power saving
 */
enum rtw_phl_status _enter_ps(struct cmd_ps *ps, u8 ps_mode, u16 macid, bool sw_only, char *rson)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct ps_cfg cfg = {0};
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);

	if (ps->ps_state == PS_STATE_ENTERED) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): already in power saving.\n", __func__);
		return RTW_PHL_STATUS_SUCCESS;
	}

	cfg.cur_pwr_lvl = ps->cur_pwr_lvl;

	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): enter %s (macid %d).\n",
			  __func__, phl_ps_ps_mode_to_str(ps_mode), macid);

	if (ps_mode == PS_MODE_LPS) {
		cfg.proto_cfg = (ps->ps_state == PS_STATE_PROTO) ? false : true;
		cfg.pwr_cfg = true;
		cfg.macid = macid;
		cfg.token = &ps->null_token;
		cfg.pwr_lvl = phl_ps_judge_pwr_lvl(ps_cap->lps_cap, ps_mode, true);
		cfg.ps_mode = ps_mode;
		cfg.awake_interval = ps_cap->lps_awake_interval;
		cfg.listen_bcn_mode = ps_cap->lps_listen_bcn_mode;
		cfg.smart_ps_mode = ps_cap->lps_smart_ps_mode;
		cfg.bcnnohit_en = ps_cap->lps_bcnnohit_en;
	} else if (ps_mode == PS_MODE_IPS) {
		cfg.macid = macid;
		if (macid == PS_MACID_NONE)
			cfg.pwr_lvl = PS_PWR_LVL_PWROFF;
		else
			cfg.pwr_lvl = phl_ps_judge_pwr_lvl(ps_cap->ips_cap, ps_mode, true);
		cfg.pwr_cfg = true;
		cfg.proto_cfg = (cfg.pwr_lvl == PS_PWR_LVL_PWROFF) ? false : true;
		cfg.ps_mode = ps_mode;
	} else {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): unknown ps mode!\n", __func__);
	}

	_set_ps_rson(ps, true, rson);

	if (!sw_only)
		status = phl_ps_enter_ps(ps->phl_info, &cfg);
	else
		status = RTW_PHL_STATUS_SUCCESS;

	if (status == RTW_PHL_STATUS_SUCCESS) {
		ps->cur_pwr_lvl = cfg.pwr_lvl;
		ps->ps_mode = cfg.ps_mode;
		ps->macid = cfg.macid;
		ps->ps_state = PS_STATE_ENTERED;

		if (cfg.proto_cfg)
			_ps_cofig_pvb_wait_rx(ps, true);

		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): macid %d enter ps success, reason(%s), sw_only(%d).\n", __func__, ps->macid, rson, sw_only);
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): ps mode(%s), pwr lvl(%s), ps state(%s)\n",
					__func__, phl_ps_ps_mode_to_str(ps->ps_mode),
					phl_ps_pwr_lvl_to_str(ps->cur_pwr_lvl), _ps_state_to_str(ps->ps_state));
		_cfg_bcn_tracking(ps, true);
	} else {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): enter ps fail! reason(%s).\n", __func__, rson);
	}

	return status;
}

static bool _pop_idle_req(struct cmd_ps *ps, struct pwr_req **req)
{
	void *d = phl_to_drvpriv(ps->phl_info);
	_os_list *new_req = NULL;
	bool ret = false;

	(*req) = NULL;
	if (pq_pop(d, &(ps->req_idle_q), &new_req, _first, _bh)) {
		(*req) = (struct pwr_req *)new_req;
		ret = true;
	} else {
		ret = false;
	}

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): remaining idle req cnt %d.\n", __func__, ps->req_idle_q.cnt);

	return ret;
}

static void _push_idle_req(struct cmd_ps *ps, struct pwr_req *req)
{
	void *d = phl_to_drvpriv(ps->phl_info);

	pq_push(d, &(ps->req_idle_q), &(req->list), _tail, _bh);

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): remaining idle req cnt %d.\n", __func__, ps->req_idle_q.cnt);
}

static bool _pop_busy_req(struct cmd_ps *ps, struct pwr_req **req)
{
	void *d = phl_to_drvpriv(ps->phl_info);
	_os_list *new_req = NULL;
	bool ret = false;

	(*req) = NULL;
	if (pq_pop(d, &(ps->req_busy_q), &new_req, _tail, _bh)) {
		(*req) = (struct pwr_req *)new_req;
		ret = true;
	} else {
		ret = false;
	}

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): current busy req cnt %d.\n", __func__, ps->req_busy_q.cnt);

	return ret;
}

static void _push_busy_req(struct cmd_ps *ps, struct pwr_req *req)
{
	void *d = phl_to_drvpriv(ps->phl_info);

	pq_push(d, &(ps->req_busy_q), &(req->list), _tail, _bh);

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): current busy req cnt %d.\n", __func__, ps->req_busy_q.cnt);
}

static void _cancel_pwr_req(struct cmd_ps *ps, u16 evt_id)
{
	struct pwr_req *req = NULL;

	if (!_pop_busy_req(ps, &req)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): busy queue is empty.\n", __func__);
		return;
	}

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): evt_id %d\n", __func__, req->evt_id);

	if (req->evt_id != evt_id && MSG_EVT_PHY_IDLE != evt_id)
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): evt_id mismatch.\n", __func__);

	req->evt_id = MSG_EVT_NONE;

	_push_idle_req(ps, req);
}

static enum rtw_phl_status _add_pwr_req(struct cmd_ps *ps, u16 evt_id)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct pwr_req *req = NULL;

	if (!_pop_idle_req(ps, &req)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): idle queue is empty.\n", __func__);
		return RTW_PHL_STATUS_RESOURCE;
	}

	req->evt_id = evt_id;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): evt_id %d\n", __func__, evt_id);

	_push_busy_req(ps, req);

	return status;
}

static void _init_pwr_req_q(struct cmd_ps *ps)
{
	void *d = phl_to_drvpriv(ps->phl_info);
	u8 i = 0;

	pq_init(d, &ps->req_busy_q);
	pq_init(d, &ps->req_idle_q);

	_os_mem_set(d, ps->req_pool, 0,
		    sizeof(struct pwr_req) * MAX_PWE_REQ_NUM);

	for (i = 0; i < MAX_PWE_REQ_NUM; i++) {
		pq_push(d, &(ps->req_idle_q),
			&(ps->req_pool[i].list), _tail, _bh);
	}
}

static void _reset_pwr_req_q(struct cmd_ps *ps)
{
	void *d = phl_to_drvpriv(ps->phl_info);
	u8 i = 0;

	pq_reset(d, &ps->req_busy_q, _bh);
	pq_reset(d, &ps->req_idle_q, _bh);

	_os_mem_set(d, ps->req_pool, 0,
		    sizeof(struct pwr_req) * MAX_PWE_REQ_NUM);

	for (i = 0; i < MAX_PWE_REQ_NUM; i++) {
		pq_push(d, &(ps->req_idle_q),
			&(ps->req_pool[i].list), _tail, _bh);
	}
}

static void _deinit_pwr_req_q(struct cmd_ps *ps)
{
	void *d = phl_to_drvpriv(ps->phl_info);

	pq_deinit(d, &ps->req_busy_q);
	pq_deinit(d, &ps->req_idle_q);
}

static void _init_ps_dflt_sw_cap(struct cmd_ps *ps)
{
	struct rtw_ps_cap_t *sw_cap = _get_ps_sw_cap(ps->phl_info);

	sw_cap->init_rf_state = RTW_RF_ON;
	sw_cap->init_rt_stop_rson = PS_RT_RSON_NONE;
	sw_cap->leave_fail_act = PS_LEAVE_FAIL_ACT_NONE;

	sw_cap->ips_en = PS_OP_MODE_DISABLED;
	sw_cap->ips_cap = 0;
	sw_cap->ips_wow_en = PS_OP_MODE_DISABLED;
	sw_cap->ips_wow_cap = 0;
	sw_cap->bcn_tracking = false;
	sw_cap->lps_en = PS_OP_MODE_DISABLED;
	sw_cap->lps_cap = 0;
	sw_cap->lps_awake_interval = 0;
	sw_cap->lps_listen_bcn_mode = RTW_LPS_RLBM_MIN;
	sw_cap->lps_smart_ps_mode = RTW_LPS_TRX_PWR0;
	sw_cap->lps_bcnnohit_en = PS_OP_MODE_DISABLED;
	sw_cap->lps_rssi_enter_threshold = 40;
	sw_cap->lps_rssi_leave_threshold = 35;
	sw_cap->lps_rssi_diff_threshold = 5;
	sw_cap->defer_para.defer_rson = PS_DEFER_RSON_NONE;
	sw_cap->defer_para.lps_ping_defer_time = 0;
	sw_cap->defer_para.lps_dhcp_defer_time = 0;
	sw_cap->lps_adv_cap = RTW_LPS_ADV_NONE;

	sw_cap->lps_wow_en = PS_OP_MODE_DISABLED;
	sw_cap->lps_wow_cap = 0;
	sw_cap->lps_wow_awake_interval = 0;
	sw_cap->lps_wow_listen_bcn_mode = RTW_LPS_RLBM_MAX;
	sw_cap->lps_wow_smart_ps_mode = RTW_LPS_TRX_PWR0;
	sw_cap->lps_wow_bcnnohit_en = PS_OP_MODE_DISABLED;
}

static void _update_init_rf_state(struct cmd_ps *ps)
{
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);
	struct rtw_ps_cap_t *ps_sw_cap = _get_ps_sw_cap(ps->phl_info);

	ps_cap->init_rf_state = ps_sw_cap->init_rf_state;
}

static void _cmd_ps_timer_cb(void *context)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct cmd_ps *ps = (struct cmd_ps *)context;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	PHL_DBG("[PS_CMD], %s(): \n", __func__);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_PS_PERIOD_CHK);
	msg.band_idx = HW_BAND_0;

	pstatus = phl_disp_eng_send_msg(ps->phl_info, &msg, &attr, NULL);
	if (pstatus != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to send MSG_EVT_PS_PERIOD_CHK.\n", __func__);
	}
}

static void _ps_common_info_init(struct phl_info_t *phl_info)
{
	struct phl_ps_info *ps_info = &phl_info->ps_info;

	if (!ps_info->init) {
		PHL_INFO("[PS_CMD], %s(): \n", __func__);

		_os_atomic_set(phl_to_drvpriv(phl_info),
					   &phl_info->ps_info.tx_ntfy,
					   0);
		ps_info->init = true;
	}
}

static void _ps_common_info_deinit(struct phl_info_t *phl_info)
{
	struct phl_ps_info *ps_info = &phl_info->ps_info;

	if (ps_info->init) {
		PHL_INFO("[PS_CMD], %s(): \n", __func__);

		_os_atomic_set(phl_to_drvpriv(phl_info),
				   	   &phl_info->ps_info.tx_ntfy,
					   0);
		ps_info->init = false;
	}
}

static enum phl_mdl_ret_code _ps_mdl_init(void *phl, void *dispr, void **priv)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct cmd_ps *ps = NULL;

	PHL_INFO("[PS_CMD], %s(): \n", __func__);

	if (priv == NULL)
		return MDL_RET_FAIL;

	(*priv) = NULL;
	ps = (struct cmd_ps *)_os_mem_alloc(phl_to_drvpriv(phl_info),
					       sizeof(struct cmd_ps));
	if (ps == NULL) {
		PHL_ERR("[PS_CMD], %s(): allocate cmd ps fail.\n", __func__);
		return MDL_RET_FAIL;
	}

	_os_mem_set(phl_to_drvpriv(phl_info), ps, 0, sizeof(struct cmd_ps));

	ps->phl_info = phl_info;
	ps->dispr = dispr;
	(*priv) = (void *)ps;
	ps->cur_pwr_lvl = PS_PWR_LVL_PWRON;
	ps->ps_state = PS_STATE_LEAVED;
	ps->ps_mode = PS_MODE_NONE;
	ps->rej_pwr_req = false;
	ps->rt_stop_rson = PS_RT_RSON_NONE;
	ps->time_info.last_tx_ping_time = _os_get_cur_time_ms();
	ps->time_info.last_tx_dhcp_time = _os_get_cur_time_ms();
	ps->recy_info.in_recovery = false;
	ps->recy_info.wdg_leave = false;
	ps->recy_info.recy_cnt = 0;

	_os_init_timer(phl_to_drvpriv(phl_info),
	               &ps->ps_timer,
	               _cmd_ps_timer_cb,
	               ps,
	               "phl_cmd_ps_timer");

	_init_pwr_req_q(ps);

	_init_ps_dflt_sw_cap(ps);

	_ps_common_info_init(phl_info);

	return MDL_RET_SUCCESS;
}

static void _ps_mdl_deinit(void *dispr, void *priv)
{
	struct cmd_ps *ps = (struct cmd_ps *)priv;

	PHL_INFO("[PS_CMD], %s(): \n", __func__);

	_os_release_timer(phl_to_drvpriv(ps->phl_info), &ps->ps_timer);

	_deinit_pwr_req_q(ps);

	_ps_common_info_deinit(ps->phl_info);

	if (ps != NULL)
		_os_mem_free(phl_to_drvpriv(ps->phl_info), ps, sizeof(struct cmd_ps));
}

static void _dump_ps_cap(struct cmd_ps *ps)
{
	struct rtw_ps_cap_t *ps_cap = NULL;

	ps_cap = _get_ps_cap(ps->phl_info);

	PHL_INFO("[PS_CMD], %s(): \n", __func__);
	PHL_INFO("[PS_CMD], init_rf_state: %d\n", ps_cap->init_rf_state);
	PHL_INFO("[PS_CMD], init_rt_stop_rson: 0x%x\n", ps_cap->init_rt_stop_rson);
	PHL_INFO("[PS_CMD], leave_fail_act: 0x%x\n", ps_cap->leave_fail_act);
	PHL_INFO("[PS_CMD], ips_en: %d\n", ps_cap->ips_en);
	PHL_INFO("[PS_CMD], ips_cap: %d\n", ps_cap->ips_cap);
	PHL_INFO("[PS_CMD], ips_wow_en: %d\n", ps_cap->ips_wow_en);
	PHL_INFO("[PS_CMD], ips_wow_cap: %d\n", ps_cap->ips_wow_cap);
	PHL_INFO("[PS_CMD], lps_en: %d\n", ps_cap->lps_en);
	PHL_INFO("[PS_CMD], lps_cap: %d\n", ps_cap->lps_cap);
	PHL_INFO("[PS_CMD], lps_awake_interval: %d\n", ps_cap->lps_awake_interval);
	PHL_INFO("[PS_CMD], lps_listen_bcn_mode: %d\n", ps_cap->lps_listen_bcn_mode);
	PHL_INFO("[PS_CMD], lps_smart_ps_mode: %d\n", ps_cap->lps_smart_ps_mode);
	PHL_INFO("[PS_CMD], lps_bcnnohit_en: %d\n", ps_cap->lps_bcnnohit_en);
	PHL_INFO("[PS_CMD], lps_rssi_enter_threshold: %d\n", ps_cap->lps_rssi_enter_threshold);
	PHL_INFO("[PS_CMD], lps_rssi_leave_threshold: %d\n", ps_cap->lps_rssi_leave_threshold);
	PHL_INFO("[PS_CMD], lps_rssi_diff_threshold: %d\n", ps_cap->lps_rssi_diff_threshold);
	PHL_INFO("[PS_CMD], defer_rson: 0x%x\n", ps_cap->defer_para.defer_rson);
	PHL_INFO("[PS_CMD], lps_ping_defer_time: %d\n", ps_cap->defer_para.lps_ping_defer_time);
	PHL_INFO("[PS_CMD], lps_dhcp_defer_time: %d\n", ps_cap->defer_para.lps_dhcp_defer_time);
	PHL_INFO("[PS_CMD], lps_wow_en: %d\n", ps_cap->lps_wow_en);
	PHL_INFO("[PS_CMD], lps_wow_cap: %d\n", ps_cap->lps_wow_cap);
	PHL_INFO("[PS_CMD], lps_wow_awake_interval: %d\n", ps_cap->lps_wow_awake_interval);
	PHL_INFO("[PS_CMD], lps_wow_listen_bcn_mode: %d\n", ps_cap->lps_wow_listen_bcn_mode);
	PHL_INFO("[PS_CMD], lps_wow_smart_ps_mode: %d\n", ps_cap->lps_wow_smart_ps_mode);
	PHL_INFO("[PS_CMD], lps_wow_bcnnohit_en: %d\n", ps_cap->lps_wow_bcnnohit_en);
	PHL_INFO("[PS_CMD], lps_pause_tx: %d\n", ps_cap->lps_pause_tx);
}

static void _leave_success_hdlr(struct cmd_ps *ps)
{
	struct phl_data_ctl_t ctl = {0};

	if (ps->stop_datapath) {
		/* resume tx datapath */
		ctl.id = PHL_MDL_POWER_MGNT;
		ctl.cmd = PHL_DATA_CTL_SW_TX_RESUME;
		phl_data_ctrler(ps->phl_info, &ctl, NULL);
		ps->stop_datapath = false;
	}
}

static enum phl_mdl_ret_code _leave_fail_hdlr(struct cmd_ps *ps)
{
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;

	PHL_WARN("[PS_CMD], %s(): action 0x%x\n", __func__, ps_cap->leave_fail_act);

	/* reject all power operation */
	if (ps_cap->leave_fail_act & PS_LEAVE_FAIL_ACT_REJ_PWR)
		ps->rej_pwr_req = true;

	/* try recovery flow if due to SER L1 */
	if (ps_cap->leave_fail_act & PS_LEAVE_FAIL_SER_L1_RECOVERY) {
		if (rtw_hal_ser_chk_ser_l1(ps->phl_info->hal)) {
			PHL_WARN("[PS_CMD], %s(): start to pend cmd...\n", __func__);
			ps->recy_info.in_recovery = true;
			ps->recy_info.recy_cnt++;
			ret = MDL_RET_PENDING;
		}
	}

	/* L2 should be the last one */
	if (ps_cap->leave_fail_act & PS_LEAVE_FAIL_ACT_L2)
		phl_ser_send_msg(ps->phl_info, RTW_PHL_SER_L2_RESET);

	return ret;
}

static enum phl_mdl_ret_code _ps_mdl_start(void *dispr, void *priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;
	struct cmd_ps *ps = (struct cmd_ps *)priv;
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);
	u8 idx = 0;

	ps->rt_stop_rson = ps_cap->init_rt_stop_rson;

	_reset_pwr_req_q(ps);

	_update_init_rf_state(ps);

	phl_dispr_get_idx(dispr, &idx);

	if (idx == 0) {
		_dump_ps_cap(ps);
		PHL_INFO("[PS_CMD], %s(): init rf state %d, reject pwr req %d\n",
				__func__, ps_cap->init_rf_state, ps->rej_pwr_req);
		if (ps_cap->init_rf_state == RTW_RF_OFF) {
			ps->rej_pwr_req = true;
			_enter_ps(ps, PS_MODE_IPS, PS_MACID_NONE, true, "mdl start with rf off");
		}
		_os_set_timer(phl_to_drvpriv(ps->phl_info), &ps->ps_timer, CMD_PS_TIMER_PERIOD);
	}
	return ret;
}

static enum phl_mdl_ret_code _ps_mdl_stop(void *dispr, void *priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;
	struct cmd_ps *ps = (struct cmd_ps *)priv;

	if (ps->ps_state == PS_STATE_ENTERED) {
		PHL_WARN("[PS_CMD], %s(): module stop in power saving!\n", __func__);
		/*_leave_ps(ps, true, "mdl stop");*/
	}

	_os_cancel_timer(phl_to_drvpriv(ps->phl_info), &ps->ps_timer);

	PHL_INFO("[PS_CMD], %s(): \n", __func__);

	return ret;
}

static bool _chk_role_all_no_link(struct cmd_ps *ps)
{
	struct phl_info_t *phl_info = ps->phl_info;
	u8 role_idx = 0;
	bool ret = true;

	for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
		if (phl_info->phl_com->wifi_roles[role_idx].active == false)
			continue;
		if (phl_info->phl_com->wifi_roles[role_idx].mstate != MLME_NO_LINK) {
			ret = false;
			break;
		}
	}

	return ret;
}

static struct rtw_wifi_role_t *_get_role_of_ps_permitted(struct cmd_ps *ps, u8 target_mode)
{
	struct phl_info_t *phl_info = ps->phl_info;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	u8 role_idx = 0;

	for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
		if (phl_com->wifi_roles[role_idx].active == false)
			continue;
		if (phl_com->wifi_roles[role_idx].type == PHL_RTYPE_STATION) {
			if (target_mode == PS_MODE_LPS &&
				phl_com->wifi_roles[role_idx].mstate == MLME_LINKED)
				return &(phl_com->wifi_roles[role_idx]);
			else if (target_mode == PS_MODE_IPS &&
					 phl_com->wifi_roles[role_idx].mstate == MLME_NO_LINK)
				return &(phl_com->wifi_roles[role_idx]);
		}
	}

	return NULL;
}

/**
 * go through all wifi roles and check whether input
 * ps mode desired is allowed with existing wroles
 * @ps: see cmd_ps
 * @target_mode: the desired ps mode (lps or ips)
 * @macid: target macid to enter lps
 */
static bool _chk_wrole_with_ps_mode(struct cmd_ps *ps,
                                    u8 target_mode,
                                    u16 *macid)
{
	bool ret = false;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_phl_stainfo_t *sta = NULL;

	if (ps->mr_info.ap_active || ps->mr_info.gc_active) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): ap %d or gc %d, is active.\n", __func__,
				ps->mr_info.ap_active, ps->mr_info.gc_active);
		return false;
	}

	if (target_mode == PS_MODE_IPS) {
		if (_chk_role_all_no_link(ps)) {
			role = _get_role_of_ps_permitted(ps, PS_MODE_IPS);
			if (role == NULL) {
				PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): there is no suitable role to enter ips.\n", __func__);
				return false;
			}
			sta = rtw_phl_get_stainfo_self(ps->phl_info, &role->rlink[RTW_RLINK_PRIMARY]);
			if (sta == NULL) {
				PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): cannot get sta info.\n", __func__);
				return false;
			}
			PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): role id to enter ips (%d).\n", __func__, role->id);
			ps->sta = sta;
			*macid = sta->macid;
			ret = true;
		}
	} else if (target_mode == PS_MODE_LPS) {
		role = _get_role_of_ps_permitted(ps, PS_MODE_LPS);
		if (role == NULL) {
			PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): there is no suitable role to enter lps.\n", __func__);
			return false;
		}

		/* TODO: Get rlink with HW_BAND */
		sta = rtw_phl_get_stainfo_self(ps->phl_info, &role->rlink[RTW_RLINK_PRIMARY]);
		if (sta == NULL) {
			PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): cannot get sta info.\n", __func__);
			return false;
		}
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): role id to enter lps (%d).\n", __func__, role->id);
		ps->sta = sta;
		*macid = sta->macid;
		ret = true;
	}

	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): decide enter ps(%s), target mode(%s).\n",
				__func__, (ret) ? "Yes" : "No", phl_ps_ps_mode_to_str(target_mode));

	return ret;
}

static enum rtw_phl_status _stop_datapath(struct cmd_ps *ps)
{
	struct phl_data_ctl_t ctl = {0};

	/* stop tx datapath */
	ctl.id = PHL_MDL_POWER_MGNT;
	ctl.cmd = PHL_DATA_CTL_SW_TX_PAUSE;

	if (phl_data_ctrler(ps->phl_info, &ctl, NULL) == RTW_PHL_STATUS_SUCCESS) {
		ps->stop_datapath = true;
		return RTW_PHL_STATUS_SUCCESS;
	}

	return RTW_PHL_STATUS_FAILURE;
}

static bool _is_datapath_active(struct cmd_ps *ps)
{
	struct phl_info_t *phl_info = ps->phl_info;

	return (_os_atomic_read(phl_to_drvpriv(phl_info), &phl_info->phl_sw_tx_sts)
			!= PHL_TX_STATUS_SW_PAUSE) ? true : false;
}

/**
 * check current capability of power saving
 * return able to enter ps or not
 * @ps: see cmd_ps
 * @mode: the target ps mode to be check
 */
static bool _chk_ps_cap(struct cmd_ps *ps, u8 mode)
{
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);

	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): mode(%s), lps_en(%d), ips_en(%d), runtime stop reason(0x%x).\n",
			__func__, phl_ps_ps_mode_to_str(mode), ps_cap->lps_en, ps_cap->ips_en, ps->rt_stop_rson);

	switch (mode) {
	case PS_MODE_LPS:
		if (ps_cap->lps_en == PS_OP_MODE_DISABLED) {
			return false;
		} else if (ps_cap->lps_en == PS_OP_MODE_FORCE_ENABLED) {
			/* force enable */
			return true;
		} else if (ps_cap->lps_en == PS_OP_MODE_AUTO) {
			if (ps->rt_stop_rson == PS_RT_RSON_NONE)
				return true;
		}
		break;
	case PS_MODE_IPS:
		if (ps_cap->ips_en == PS_OP_MODE_DISABLED) {
			return false;
		} else if (ps_cap->ips_en == PS_OP_MODE_FORCE_ENABLED) {
			/* force enable */
			return true;
		} else if (ps_cap->ips_en == PS_OP_MODE_AUTO) {
			if (ps->rt_stop_rson == PS_RT_RSON_NONE)
				return true;
		}
		break;
	default:
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): unknown ps mode.\n", __func__);
		return false;
	}

	return false;
}

/**
 * check the condition of ips
 * return whether to enter ips or not
 * @ps: see cmd_ps
 */
static bool _chk_ips_enter(struct cmd_ps *ps, u16 *macid)
{
	if (TEST_STATUS_FLAG(ps->phl_info->phl_com->dev_state, RTW_DEV_RESUMING)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): resume in progress.\n", __func__);
		return false;
	}

	if (ps->rej_pwr_req == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
		return false;
	}

	if (!_chk_ps_cap(ps, PS_MODE_IPS)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): ips is not allowed.\n", __func__);
		return false;
	}

	if (!phl_disp_eng_is_fg_empty(ps->phl_info, HW_BAND_0)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): fg exist.\n", __func__);
		return false;
	}

	if (ps->req_busy_q.cnt) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): req q is not empty.\n", __func__);
		return false;
	}

	if (ps->btc_req_pwr) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): btc req pwr.\n", __func__);
		return false;
	}

	if (!_chk_wrole_with_ps_mode(ps, PS_MODE_IPS, macid)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): no need to enter ips.\n", __func__);
		return false;
	}

	return true;
}

/**
 * check the condition of lps
 * return whether to enter lps or not
 * @ps: see cmd_ps
 */
static bool _chk_lps_enter(struct cmd_ps *ps, u16 *macid)
{
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);

	/* check enter lps or not */

	if (TEST_STATUS_FLAG(ps->phl_info->phl_com->dev_state, RTW_DEV_RESUMING)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): resume in progress.\n", __func__);
		return false;
	}

	if (ps->rej_pwr_req == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
		return false;
	}

	/* check capability */
	if (!_chk_ps_cap(ps, PS_MODE_LPS)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): lps is not allowed.\n", __func__);
		return false;
	}

	/* check fg module */
	if (!phl_disp_eng_is_fg_empty(ps->phl_info, HW_BAND_0)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): fg exist.\n", __func__);
		return false;
	}

	/* ref cnt */
	if (ps->req_busy_q.cnt) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): req q is not empty.\n", __func__);
		return false;
	}

	/* btc */
	if (ps->btc_req_pwr) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): btc req pwr.\n", __func__);
		return false;
	}

	if (ps->wdg_leave_ps == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): just leave ps in watchdog prephase.\n", __func__);
		return false;
	}

	if (_ps_chk_defer(ps))
		return false;

	/* check wifi role */
	if (!_chk_wrole_with_ps_mode(ps, PS_MODE_LPS, macid)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): no need to enter lps.\n", __func__);
		return false;
	}

	/* lps */
	if (_lps_state_judge_changed(ps, *macid, ps->ps_state, PS_STATE_ENTERED)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): lps state changed, going to enter...\n", __func__);
		/* check data path stop or not */
		if (ps_cap->lps_pause_tx) {
			if (!_is_datapath_active(ps)) {
				return true;
			} else {
				if (_stop_datapath(ps) == RTW_PHL_STATUS_SUCCESS)
					return true;
			}
		} else {
			return true;
		}
	}

	return false;
}

static void _ps_watchdog_info_dump(struct cmd_ps *ps)
{
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "========== CMD PS Info ========== \n");
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "ps mode(%s), pwr lvl(%s), ps state(%s)\n",
			phl_ps_ps_mode_to_str(ps->ps_mode),
			phl_ps_pwr_lvl_to_str(ps->cur_pwr_lvl),
			_ps_state_to_str(ps->ps_state));
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "req idle cnt(%d), req busy cnt(%d)\n",
			ps->req_idle_q.cnt, ps->req_busy_q.cnt);
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "ap active(%s), gc active(%s)\n",
			(ps->mr_info.ap_active ? "yes" : "no"), (ps->mr_info.gc_active ? "yes" : "no"));
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "reject all pwr req(%s), btc req pwr(%s), runtime stop reason(0x%x)\n",
			(ps->rej_pwr_req ? "yes" : "no"), (ps->btc_req_pwr ? "yes" : "no"), ps->rt_stop_rson);
	if (ps_cap->defer_para.defer_rson & PS_DEFER_PING_PKT) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "tx ping pass time(ms): %d\n",
			phl_get_passing_time_ms(ps->time_info.last_tx_ping_time));
	}
	if (ps_cap->defer_para.defer_rson & PS_DEFER_DHCP_PKT) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "tx dhcp pass time(ms): %d\n",
			phl_get_passing_time_ms(ps->time_info.last_tx_dhcp_time));
	}
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "========== CMD PS Info ========== \n");
}

static bool _lps_judge_wdg_leave(struct cmd_ps *ps)
{
	if (ps->recy_info.wdg_leave) {
		ps->recy_info.wdg_leave = false;
		return true;
	}

	return false;
}

/**
 * pre-phase handler of watchdog
 * will check whether to leave lps or not
 * @ps: see cmd_ps
 */
static enum phl_mdl_ret_code
_ps_watchdog_pre_hdlr(struct cmd_ps *ps)
{
	/* check leave lps or not */

	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): pwr lvl(%s).\n", __func__, phl_ps_pwr_lvl_to_str(ps->cur_pwr_lvl));

	ps->wdg_leave_ps = false;

	if (ps->rej_pwr_req == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
		return MDL_RET_CANNOT_IO;
	}

	if (ps->ps_mode == PS_MODE_IPS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): under inactive ps.\n", __func__);
		return MDL_RET_CANNOT_IO;
	}

	if (ps->ps_state == PS_STATE_LEAVED) {
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): not in power saving.\n", __func__);
		return MDL_RET_SUCCESS;
	} else {
		if (_lps_state_judge_changed(ps, ps->macid, ps->ps_state, PS_STATE_LEAVED) ||
		    _lps_judge_wdg_leave(ps)) {
			PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): lps state changed, going to leave...\n", __func__);
			if (_leave_ps(ps, true, "watchdog") == RTW_PHL_STATUS_SUCCESS) {
				ps->wdg_leave_ps = true;
				_leave_success_hdlr(ps);
				return MDL_RET_SUCCESS;
			} else {
				PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): going to L2 reset!\n", __func__);
				return _leave_fail_hdlr(ps);
			}
		} else {
			_cfg_bcn_tracking(ps, false);
		}
		/**
		 * if _lps_state_judge_changed decide not to leave lps and currrent lps
		 * state is in protocol only, i/o operation is allowable.
		 */
		if (ps->ps_state == PS_STATE_PROTO)
			return MDL_RET_SUCCESS;
		else
			return MDL_RET_CANNOT_IO;
	}
}

/**
 * post-phase handler of watchdog
 * will check whether to enter lps or not
 * @ps: see cmd_ps
 */
static enum phl_mdl_ret_code
_ps_watchdog_post_hdlr(struct cmd_ps *ps)
{
	u16 macid = PS_MACID_NONE;

	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): pwr lvl(%s).\n", __func__, phl_ps_pwr_lvl_to_str(ps->cur_pwr_lvl));

	if (_chk_lps_enter(ps, &macid))
		_enter_ps(ps, PS_MODE_LPS, macid, false, "watchdog");
	else if (_chk_ips_enter(ps, &macid))
		_enter_ps(ps, PS_MODE_IPS, macid, false, "watchdog");

	if (_chk_ps_cap(ps, PS_MODE_IPS) == true || _chk_ps_cap(ps, PS_MODE_LPS) == true)
		_ps_watchdog_info_dump(ps);

	return MDL_RET_SUCCESS;
}

/**
 * leave ps in pre-phase, caller need to check defer time is set
 * @ps: see cmd_ps
 * @msg: see phl_msg
 * @rson: see phl_ps_defer_rson
 */
static enum phl_mdl_ret_code _ps_leave_defer_rson(struct cmd_ps *ps, struct phl_msg *msg, u8 rson)
{
	if (ps->rej_pwr_req == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
		return MDL_RET_CANNOT_IO;
	}

	if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		if (_leave_ps(ps, true, (char *)_defer_rson_to_str(rson)) == RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): reason %s\n", __func__, (char *)_defer_rson_to_str(rson));
			_leave_success_hdlr(ps);
			return MDL_RET_SUCCESS;
		} else {
			_leave_fail_hdlr(ps);
		}
	} else {
		return MDL_RET_SUCCESS;
	}

	return MDL_RET_CANNOT_IO;
}

/**
 * update related time info, caller need to check defer time is set
 * @ps: see cmd_ps
 * @rson: see phl_ps_defer_rson
 */
static void _ps_update_defer_time_cmn(struct cmd_ps *ps, u8 rson)
{
	switch (rson) {
	case PS_DEFER_PING_PKT:
		ps->time_info.last_tx_ping_time = _os_get_cur_time_ms();
		break;
	default:
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): unknown ps defer rson.\n", __func__);
	}
}

static enum phl_mdl_ret_code _ps_ping_pkt_hdlr(struct cmd_ps *ps,
                                               struct phl_msg *msg,
                                               enum phl_pkt_evt_type type)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);

	switch (type) {
	case PKT_EVT_TX_PING_REQ:
		if ((ps_cap->defer_para.defer_rson & PS_DEFER_PING_PKT)) {
			_ps_update_defer_time_cmn(ps, PS_DEFER_PING_PKT);
			ret = _ps_leave_defer_rson(ps, msg, PS_DEFER_PING_PKT);
		}
		break;
	case PKT_EVT_RX_PING_RSP:
		ret = MDL_RET_SUCCESS;
		break;
	default:
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): unknown ps evt type(%d)\n",
		          __func__, type);
		break;
	}

	return ret;
}

static enum phl_mdl_ret_code _ps_add_pwr_req_cmn(struct cmd_ps *ps, struct phl_msg *msg, char *rson)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;

	if (ps->rej_pwr_req == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
		return MDL_RET_CANNOT_IO;
	}

	if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		if (_leave_ps(ps, true, rson) == RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): reason %s\n", __func__, rson);
			_add_pwr_req(ps, MSG_EVT_ID_FIELD(msg->msg_id));
			_leave_success_hdlr(ps);
			ret = MDL_RET_SUCCESS;
		} else {
			ret = _leave_fail_hdlr(ps);
		}
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

static enum phl_mdl_ret_code _ps_cancel_pwr_req_cmn(struct cmd_ps *ps, struct phl_msg *msg, char *rson)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;
	u16 macid = PS_MACID_NONE;

	if (!IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): reason %s\n", __func__, rson);
		_cancel_pwr_req(ps, MSG_EVT_ID_FIELD(msg->msg_id));
		if (_chk_ips_enter(ps, &macid)) {
			_enter_ps(ps, PS_MODE_IPS, macid, false, rson);
		}
		ret = MDL_RET_SUCCESS;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

#define case_pkt_evt(src) \
	case PKT_EVT_##src: return #src
const char *_ps_pkt_evt_to_str(u8 pkt_evt)
{
	switch (pkt_evt) {
	case_pkt_evt(DHCP);
	case_pkt_evt(ARP);
	case_pkt_evt(TX_PING_REQ);
	case_pkt_evt(RX_PING_RSP);
	case_pkt_evt(EAPOL_START);
	case_pkt_evt(EAPOL_END);
	case_pkt_evt(MAX);
	default:
		return "Undefined";
	}
}

static enum phl_mdl_ret_code
_ps_pkt_evt_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;
	enum phl_pkt_evt_type *pkt_evt_type = NULL;
	u8 *cmd = NULL;
	u32 cmd_len = 0;

	pstatus = phl_cmd_get_cur_cmdinfo(ps->phl_info,
			msg->band_idx, msg, &cmd, &cmd_len);
	if (pstatus != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: get cmd info fail!\n", __func__);
		return MDL_RET_FAIL;
	}

	pkt_evt_type = (enum phl_pkt_evt_type *)cmd;

	switch (*pkt_evt_type) {
	case PKT_EVT_TX_PING_REQ:
	case PKT_EVT_RX_PING_RSP:
		ret = _ps_ping_pkt_hdlr(ps, msg, *pkt_evt_type);
		break;
	case PKT_EVT_EAPOL_START:
		ret = _ps_add_pwr_req_cmn(ps, msg, (char *)_ps_pkt_evt_to_str(*pkt_evt_type));
		break;
	case PKT_EVT_EAPOL_END:
		ret = _ps_cancel_pwr_req_cmn(ps, msg, (char *)_ps_pkt_evt_to_str(*pkt_evt_type));
		break;
	default:
		ret = MDL_RET_SUCCESS;
		break;
	}

	return ret;
}

static enum phl_mdl_ret_code _phy_on_msg_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;

	if (ps->rej_pwr_req == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
		return MDL_RET_CANNOT_IO;
	}

	if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], MSG_EVT_PHY_ON\n");
		if (_leave_ps(ps, true, "phy on") == RTW_PHL_STATUS_SUCCESS) {
			_add_pwr_req(ps, MSG_EVT_ID_FIELD(msg->msg_id));
			_leave_success_hdlr(ps);
			ret = MDL_RET_SUCCESS;
		} else {
			ret = _leave_fail_hdlr(ps);
		}
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

static enum phl_mdl_ret_code _tx_pwr_req_msg_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;

	if (ps->rej_pwr_req == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
		return MDL_RET_CANNOT_IO;
	}

	if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], MSG_EVT_TRX_PWR_REQ\n");
		if (_leave_ps(ps, false, "tx req") == RTW_PHL_STATUS_SUCCESS) {
			_leave_success_hdlr(ps);
			ret = MDL_RET_SUCCESS;
		} else {
			ret = _leave_fail_hdlr(ps);
		}
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

static enum phl_mdl_ret_code _phy_idle_msg_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;
	u16 macid = PS_MACID_NONE;

	if (!IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], MSG_EVT_PHY_IDLE\n");
		_cancel_pwr_req(ps, MSG_EVT_ID_FIELD(msg->msg_id));
		/* check enter ips or not */
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): try enter ips.\n", __func__);
		if (_chk_ips_enter(ps, &macid)) {
			_enter_ps(ps, PS_MODE_IPS, macid, false, "phy idle");
		}
		ret = MDL_RET_SUCCESS;
	} else {
		ret = MDL_RET_SUCCESS;
	}

	return ret;
}

/**
 * pre-phase handler of msg
 * leave ps and return corresponding status
 * @ps: see cmd_ps
 * @evt_id: evt id of msg
 */
static enum phl_mdl_ret_code
_ext_msg_pre_hdlr(struct cmd_ps *ps, u16 evt_id)
{
	if (ps->rej_pwr_req == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
		return MDL_RET_CANNOT_IO;
	}

	/* power request */
	if (_leave_ps(ps, true, "ext msg req") == RTW_PHL_STATUS_SUCCESS) {
		_add_pwr_req(ps, evt_id);
		_leave_success_hdlr(ps);
		return MDL_RET_SUCCESS;
	} else {
		return _leave_fail_hdlr(ps);
	}
}

/**
 * post-phase handler of msg
 * cancel power req and chk enter ips or not
 * @ps: see cmd_ps
 * @evt_id: evt id of msg
 */
static enum phl_mdl_ret_code
_ext_msg_post_hdlr(struct cmd_ps *ps, u16 evt_id)
{
	u16 macid = PS_MACID_NONE;

	/* cancel power request */
	_cancel_pwr_req(ps, evt_id);

	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): try enter ips.\n", __func__);
	if (_chk_ips_enter(ps, &macid)) {
		_enter_ps(ps, PS_MODE_IPS, macid, false, "ext msg done");
	}

	return MDL_RET_SUCCESS;
}

static void _ps_mr_info_upt(struct cmd_ps *ps, struct rtw_wifi_role_t *role)
{
	struct phl_info_t *phl_info = ps->phl_info;
	struct rtw_wifi_role_t *wr = NULL;
	u8 role_idx = 0;

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): id %d, type %d, mstate %d\n", __func__,
				role->id, role->type, role->mstate);
	#ifdef RTW_WKARD_PHL_NTFY_MEDIA_STS
	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): is_gc %d\n", __func__, role->is_gc);
	#endif

	_os_mem_set(phl_to_drvpriv(phl_info), &ps->mr_info, 0, sizeof(struct _ps_mr_info));

	for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
		wr = &(phl_info->phl_com->wifi_roles[role_idx]);
		if (wr->active == false)
			continue;
		#ifdef RTW_WKARD_PHL_NTFY_MEDIA_STS
		if (wr->type == PHL_RTYPE_STATION) {
			if (wr->is_gc == true && wr->mstate != MLME_NO_LINK)
				ps->mr_info.gc_active = true;
		}
		#endif
		if (rtw_phl_role_is_ap_category(wr) ||
			wr->type == PHL_RTYPE_MESH)
			ps->mr_info.ap_active = (wr->mstate == MLME_NO_LINK) ? false : true;
	}

	PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): gc_active %d, ap_active %d\n", __func__,
				ps->mr_info.gc_active, ps->mr_info.ap_active);
}

static bool _is_ignored_mrc_evt(u16 evt_id)
{
	return false;
}

static enum phl_mdl_ret_code
_mrc_mdl_msg_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;
	struct rtw_role_cmd *rcmd = NULL;
	struct rtw_wifi_role_t *role = NULL;

	if (_is_ignored_mrc_evt(MSG_EVT_ID_FIELD(msg->msg_id)))
		return MDL_RET_SUCCESS;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_ROLE_NTFY:
		if (!IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			if (msg->inbuf && (msg->inlen == sizeof(struct rtw_role_cmd))) {
				rcmd  = (struct rtw_role_cmd *)msg->inbuf;
				role = rcmd->wrole;
				_ps_mr_info_upt(ps, role);
			}
		}
		ret = MDL_RET_SUCCESS;
		break;
	default:
		if (ps->cur_pwr_lvl != PS_PWR_LVL_PWRON) {
			PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): MDL_ID(%d)-EVT_ID(%d) get cannot I/O!\n", __func__,
		         MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));
			ret = MDL_RET_CANNOT_IO;
		} else {
			ret = MDL_RET_SUCCESS;
		}
		break;
	}

	return ret;
}

static bool _is_ignored_ser_evt(struct cmd_ps *ps, struct phl_msg *msg)
{
	u16 evt_id = MSG_EVT_ID_FIELD(msg->msg_id);
	u8 state = 0;

	if (MSG_EVT_SER_M9_L2_RESET == evt_id) {
		return false;
	} else if (MSG_EVT_SER_POLLING_CHK == evt_id) {
		state = (u8)msg->rsvd[0].value;
		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): EVT_SER_POLLING_CHK - state(0x%X)\n",
		          __func__, state);
		if (!state) /* CMD_SER_NOT_OCCUR */
			return false;
	}
	return true;
}

static enum phl_mdl_ret_code
_ser_mdl_msg_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;

	if (_is_ignored_ser_evt(ps, msg))
		return MDL_RET_SUCCESS;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_SER_M9_L2_RESET:
		ret = MDL_RET_SUCCESS;
		break;
	default:
		if (ps->cur_pwr_lvl != PS_PWR_LVL_PWRON) {
			PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): MDL_ID(%d)-EVT_ID(%d) get cannot I/O!\n",
			         __func__,
			         MSG_MDL_ID_FIELD(msg->msg_id),
			         MSG_EVT_ID_FIELD(msg->msg_id));
			ret = MDL_RET_CANNOT_IO;
		} else {
			ret = MDL_RET_SUCCESS;
		}
		break;
	}

	return ret;
}

static bool _is_ignored_general_evt(u16 evt_id)
{
	bool ret = false;

	switch (evt_id) {
	case MSG_EVT_SW_WATCHDOG:
	case MSG_EVT_GET_USB_SW_ABILITY:
	case MSG_EVT_GET_USB_SPEED:
		ret = true;
		break;
	default:
		ret = false;
		break;
	}

	return ret;
}

#ifdef RTW_WKARD_LINUX_CMD_WKARD
static enum phl_mdl_ret_code
_linux_cmd_wkard_hdlr(struct cmd_ps *ps)
{
	if (ps->rej_pwr_req == true) {
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
		return MDL_RET_CANNOT_IO;
	}

	/* power request */
	if (_leave_ps(ps, true, "linux cmd wkard") == RTW_PHL_STATUS_SUCCESS) {
		_leave_success_hdlr(ps);
		return MDL_RET_SUCCESS;
	} else {
		return _leave_fail_hdlr(ps);
	}
}
#endif /* RTW_WKARD_LINUX_CMD_WKARD */

static enum phl_mdl_ret_code
_general_mdl_msg_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;

	if (_is_ignored_general_evt(MSG_EVT_ID_FIELD(msg->msg_id)))
		return MDL_RET_SUCCESS;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_TSF_SYNC_DONE:
	case MSG_EVT_CHG_OP_CH_DEF_START:
	case MSG_EVT_CHG_OP_CH_DEF_END:
	case MSG_EVT_SWCH_START:
	case MSG_EVT_PCIE_TRX_MIT:
	case MSG_EVT_FORCE_USB_SW:
	case MSG_EVT_GET_USB_SPEED:
	case MSG_EVT_GET_USB_SW_ABILITY:
	case MSG_EVT_HWSEQ_GET_HW_SEQUENCE:
	case MSG_EVT_CFG_AMPDU:
	case MSG_EVT_CFG_AMSDU_TX:
	case MSG_EVT_DFS_PAUSE_TX:
	case MSG_EVT_ROLE_RECOVER:
	case MSG_EVT_ROLE_SUSPEND:
	case MSG_EVT_HAL_SET_L2_LEAVE:
	case MSG_EVT_NOTIFY_HAL:
	case MSG_EVT_ISSUE_BCN:
	case MSG_EVT_STOP_BCN:
	case MSG_EVT_SEC_KEY:
	case MSG_EVT_ROLE_START:
	case MSG_EVT_ROLE_CHANGE:
	case MSG_EVT_ROLE_STOP:
	case MSG_EVT_STA_INFO_CTRL:
	case MSG_EVT_STA_MEDIA_STATUS_UPT:
	case MSG_EVT_CFG_CHINFO:
	case MSG_EVT_STA_CHG_STAINFO:
	case MSG_EVT_TWT_STA_ACCEPT:
	case MSG_EVT_TWT_STA_TEARDOWN:
	case MSG_EVT_TWT_GET_TWT:
	case MSG_EVT_GET_CUR_TSF:
	case MSG_EVT_DFS_RD_IS_DETECTING:
	case MSG_EVT_DFS_RD_SETUP:
	case MSG_EVT_TXPWR_SETUP:
#ifdef CONFIG_PHL_P2PPS
	case MSG_EVT_NOA_DISABLE:
	case MSG_EVT_NOA_UP:
#endif /* CONFIG_PHL_P2PPS */
#ifdef RTW_WKARD_LINUX_CMD_WKARD
	case MSG_EVT_LINUX_CMD_WRK_TRI_PS:
#endif /* RTW_WKARD_LINUX_CMD_WKARD */
	case MSG_EVT_DBG_RX_DUMP:
	case MSG_EVT_UPDT_EXT_TXPWR_LMT:
	case MSG_EVT_SET_MACID_PAUSE:
	case MSG_EVT_SET_MACID_PKT_DROP:
	case MSG_EVT_SET_UL_FIXINFO:
	case MSG_EVT_SET_STA_SEC_IV:
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): MDL_ID(%d)-EVT_ID(%d) in %s phase.\n", __func__,
			MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id),
			(IS_MSG_IN_PRE_PHASE(msg->msg_id) ? "pre-protocol" : "post-protocol"));
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id))
			ret = _ext_msg_pre_hdlr(ps, MSG_EVT_ID_FIELD(msg->msg_id));
		else
			ret = _ext_msg_post_hdlr(ps, MSG_EVT_ID_FIELD(msg->msg_id));
		break;
#ifdef RTW_WKARD_LINUX_CMD_WKARD
	case MSG_EVT_LINUX_CMD_WRK:
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id))
			ret = _linux_cmd_wkard_hdlr(ps);
		else
			ret = MDL_RET_SUCCESS;
		break;
#endif /* RTW_WKARD_LINUX_CMD_WKARD */
	case MSG_EVT_HW_WATCHDOG:
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			ret = _ps_watchdog_pre_hdlr(ps);
			if (ret == MDL_RET_SUCCESS) {
				ps->rssi_bcn_min = phl_get_min_rssi_bcn(ps->phl_info);
				PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): update rssi_bcn_min to %d\n", __func__, ps->rssi_bcn_min);
			}
		} else {
			ret = _ps_watchdog_post_hdlr(ps);
		}
		break;
	case MSG_EVT_RF_ON:
		if (!IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], MSG_EVT_RF_ON\n");
			ps->rej_pwr_req = false;
		}
		ret = MDL_RET_SUCCESS;
		break;
	case MSG_EVT_RF_OFF:
		if (!IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], MSG_EVT_RF_OFF\n");
			if (_leave_ps(ps, true, "msg rf off") == RTW_PHL_STATUS_SUCCESS) {
				_leave_success_hdlr(ps);
			} else {
				ret = _leave_fail_hdlr(ps);
				break;
			}
			ps->rej_pwr_req = true;
			_enter_ps(ps, PS_MODE_IPS, PS_MACID_NONE, false, "msg rf off");
		}
		ret = MDL_RET_SUCCESS;
		break;
	case MSG_EVT_PHY_ON:
		ret = _phy_on_msg_hdlr(ps, msg);
		break;
	case MSG_EVT_PHY_IDLE:
		ret = _phy_idle_msg_hdlr(ps, msg);
		break;
	case MSG_EVT_PKT_EVT_NTFY:
		ret = _ps_pkt_evt_hdlr(ps, msg);
		break;
	default:
		if (ps->cur_pwr_lvl != PS_PWR_LVL_PWRON) {
			PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): MDL_ID(%d)-EVT_ID(%d) get cannot I/O!\n",
			         __func__,
			         MSG_MDL_ID_FIELD(msg->msg_id),
			         MSG_EVT_ID_FIELD(msg->msg_id));
			ret = MDL_RET_CANNOT_IO;
		} else {
			ret = MDL_RET_SUCCESS;
		}
		break;
	}

	return ret;
}

static bool _is_ignored_datapath_evt(u16 evt_id)
{
	return false;
}

static enum phl_mdl_ret_code
_datapath_mdl_msg_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;

	if (_is_ignored_datapath_evt(MSG_EVT_ID_FIELD(msg->msg_id)))
		return MDL_RET_SUCCESS;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_TRX_PWR_REQ:
		ret = _tx_pwr_req_msg_hdlr(ps, msg);
		break;
	default:
		if (ps->cur_pwr_lvl != PS_PWR_LVL_PWRON) {
			PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): MDL_ID(%d)-EVT_ID(%d) get cannot I/O!\n",
			         __func__,
			         MSG_MDL_ID_FIELD(msg->msg_id),
			         MSG_EVT_ID_FIELD(msg->msg_id));
			ret = MDL_RET_CANNOT_IO;
		} else {
			ret = MDL_RET_SUCCESS;
		}
		break;
	}

	return ret;
}

/**
 * bypass msg of specific module
 * @msg: see phl_msg
 */
static bool _is_ignored_mdl(struct phl_msg *msg)
{
	if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_BTC)
		return true;
	if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_LED)
		return true;

	return false;
}

static enum phl_mdl_ret_code
_ps_mdl_hdl_external_evt(void *dispr, struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;

	if (_is_ignored_mdl(msg)) {
		/* PHL_INFO("[PS_CMD], %s(): ignore MDL_ID(%d)-EVT_ID(%d).\n", __func__,
		         MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id)); */
		return MDL_RET_SUCCESS;
	}

	switch (MSG_MDL_ID_FIELD(msg->msg_id)) {
	case PHL_MDL_GENERAL:
		ret = _general_mdl_msg_hdlr(ps, msg);
		break;
	case PHL_MDL_MRC:
		ret = _mrc_mdl_msg_hdlr(ps, msg);
		break;
	case PHL_MDL_TX:
	case PHL_MDL_RX:
		ret = _datapath_mdl_msg_hdlr(ps, msg);
		break;
	case PHL_MDL_SER:
		ret = _ser_mdl_msg_hdlr(ps, msg);
		break;
	/* handle ohters mdl here */
	default:
		if (ps->cur_pwr_lvl != PS_PWR_LVL_PWRON) {
			PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): MDL_ID(%d)-EVT_ID(%d) get cannot I/O!\n", __func__,
		         MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));
			ret = MDL_RET_CANNOT_IO;
		} else {
			ret = MDL_RET_SUCCESS;
		}
		break;
	}

	return ret;
}

static enum phl_mdl_ret_code _ps_cap_chg_msg_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_ps_rt_rson  rt_rson = PS_RT_RSON_NONE;
	bool ps_allow = false;
	struct rt_ps *rt_ps_info= NULL;
	u16 macid = PS_MACID_NONE;

	if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		rt_ps_info = (struct rt_ps *)msg->inbuf;
		ps_allow = rt_ps_info->ps_allow;
		rt_rson = rt_ps_info->rt_rson;
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): update -> ps_allow(%d), reason(%d).\n", __func__, ps_allow, rt_rson);

		if (ps_allow) {
			if (TEST_STATUS_FLAG(ps->rt_stop_rson, rt_rson) == true)
				CLEAR_STATUS_FLAG(ps->rt_stop_rson, rt_rson);
			else
				return MDL_RET_SUCCESS;
		} else {
			if (TEST_STATUS_FLAG(ps->rt_stop_rson, rt_rson) == false)
				SET_STATUS_FLAG(ps->rt_stop_rson, rt_rson);
			else
				return MDL_RET_SUCCESS;
		}

		if (ps->rej_pwr_req == true) {
			PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): reject pwr req.\n", __func__);
			return MDL_RET_CANNOT_IO;
		}

		if (ps->rt_stop_rson != PS_RT_RSON_NONE) {
			if (_leave_ps(ps, true, "cap chg") == RTW_PHL_STATUS_SUCCESS) {
				_leave_success_hdlr(ps);
				return MDL_RET_SUCCESS;
			} else {
				return _leave_fail_hdlr(ps);
			}
		}
	} else {
		if (ps->rt_stop_rson == PS_RT_RSON_NONE) {
			PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): try enter ips.\n", __func__);
			if (_chk_ips_enter(ps, &macid)) {
				_enter_ps(ps, PS_MODE_IPS, macid, false, "cap chg");
			}
		}
	}

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code _tx_pkt_ntfy_msg_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	if (!IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		PHL_WARN("[PS_CMD], %s(): rpwm with tx req.\n", __func__);
		/* rpwm with tx req */
		rtw_hal_ps_notify_wake(ps->phl_info->hal);
	}

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code _ps_period_chk_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	u16 macid = PS_MACID_NONE;

	if (IS_MSG_IN_PRE_PHASE(msg->msg_id))
		return MDL_RET_SUCCESS;

	ps->wdg_leave_ps = false;

	if (ps->ps_state != PS_STATE_ENTERED) {

		PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): \n", __func__);
		if (_chk_lps_enter(ps, &macid))
			_enter_ps(ps, PS_MODE_LPS, macid, false, "period chk");
		/* else if (_chk_ips_enter(ps, &macid))
			_enter_ps(ps, PS_MODE_IPS, macid, false, "period chk"); */
	}

	_os_set_timer(phl_to_drvpriv(ps->phl_info), &ps->ps_timer, CMD_PS_TIMER_PERIOD);

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code _ps_dbg_cmd_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(ps->phl_info);
	struct rtw_ps_cap_t *ps_sw_cap = _get_ps_sw_cap(ps->phl_info);
	struct ps_mdl_dbg_info *dbg_info = NULL;
	u16 macid = PS_MACID_NONE;
	u8 op = 0;

	dbg_info = (struct ps_mdl_dbg_info *)msg->inbuf;
	op = dbg_info->op;

	switch (op) {
	case FORCE_LPS_ENTER:
		if (!IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			if (!_chk_wrole_with_ps_mode(ps, PS_MODE_LPS, &macid))
				return MDL_RET_SUCCESS;
			_enter_ps(ps, PS_MODE_LPS, macid, false, "dbg lps enter");
			ps->rej_pwr_req = true;
		}
		break;
	case FORCE_LPS_LEAVE:
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			_leave_ps(ps, true, "dbg lps leave");
			ps->rej_pwr_req = false;
		}
		break;
	case FORCE_IPS_ENTER:
		if (!IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			if (!_chk_wrole_with_ps_mode(ps, PS_MODE_IPS, &macid))
				return MDL_RET_SUCCESS;
			_enter_ps(ps, PS_MODE_IPS, macid, false, "dbg ips enter");
			ps->rej_pwr_req = true;
		}
		break;
	case FORCE_IPS_LEAVE:
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			_leave_ps(ps, true, "dbg ips leave");
			ps->rej_pwr_req = false;
		}
		break;
	case SET_LPS_MODE:
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			if (ps_cap->lps_en != dbg_info->val) {
				ps_cap->lps_en = ps_sw_cap->lps_en = dbg_info->val;
				if (ps->ps_mode == PS_MODE_LPS) {
					if (_leave_ps(ps, true, "dbg lps mode") == RTW_PHL_STATUS_SUCCESS) {
						_leave_success_hdlr(ps);
						return MDL_RET_SUCCESS;
					} else {
						return _leave_fail_hdlr(ps);
					}
				}
			}
		}
		break;
	case SET_IPS_MODE:
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			if (ps_cap->ips_en != dbg_info->val) {
				ps_cap->ips_en = ps_sw_cap->ips_en = dbg_info->val;
				if (ps->ps_mode == PS_MODE_IPS)
					_leave_ps(ps, true, "dbg ips mode");

				if (ps_cap->ips_en != PS_OP_MODE_DISABLED) {
					if (_chk_ips_enter(ps, &macid))
						_enter_ps(ps, PS_MODE_IPS, macid, false, "dbg ips enter");
				}
			}
		}
		break;
	case SET_LPS_CAP:
		if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
			u8 lps_cap = dbg_info->val;
			if (ps_cap->lps_cap != lps_cap) {
				ps_cap->lps_cap = ps_sw_cap->lps_cap = lps_cap;
				if (ps->ps_mode == PS_MODE_LPS) {
					if (_leave_ps(ps, true, "dbg lps cap") == RTW_PHL_STATUS_SUCCESS) {
						_leave_success_hdlr(ps);
						return MDL_RET_SUCCESS;
					} else {
						return _leave_fail_hdlr(ps);
					}
				}
			}
		}
		break;
	default:
		break;
	}

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_ps_mdl_hdl_internal_evt(void *dispr, struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_CANNOT_IO;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_PHY_ON:
		ret = _phy_on_msg_hdlr(ps, msg);
		break;
	case MSG_EVT_PHY_IDLE:
		ret = _phy_idle_msg_hdlr(ps, msg);
		break;
	case MSG_EVT_PS_CAP_CHG:
		ret = _ps_cap_chg_msg_hdlr(ps, msg);
		break;
	case MSG_EVT_PS_PERIOD_CHK:
		ret = _ps_period_chk_hdlr(ps, msg);
		break;
	case MSG_EVT_PS_DBG_CMD:
		ret = _ps_dbg_cmd_hdlr(ps, msg);
		break;
	case MSG_EVT_TX_PKT_NTFY:
		ret = _tx_pkt_ntfy_msg_hdlr(ps, msg);
		break;
	default:
		ret = MDL_RET_CANNOT_IO;
		break;
	}

	return ret;
}

static enum phl_mdl_ret_code
_ps_recy_hdlr(struct cmd_ps *ps, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	struct _ps_recovery_info *recy_info = &ps->recy_info;

	if (recy_info->in_recovery) {
		/* return MDL_RET_PENDING to all event except from SER module during recovery */
		if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_SER) {
			ret = MDL_RET_PENDING;
		} else {
			PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): get MDL_ID(%d)-EVT_ID(%d) from SER.\n", __func__,
		         	  MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));
			if (MSG_EVT_ID_FIELD(msg->msg_id) == MSG_EVT_SER_M5_READY &&
				IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
				PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): get SER M5, stop pending.\n", __func__);
						  recy_info->in_recovery = false;
			}
		}
	}

	return ret;
}

static enum phl_mdl_ret_code
_ps_mdl_msg_hdlr(void *dispr, void *priv, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	struct cmd_ps *ps = (struct cmd_ps *)priv;
	struct _ps_recovery_info *recy_info = &ps->recy_info;

	ret = _ps_recy_hdlr(ps, msg);
	if (ret == MDL_RET_PENDING)
		goto end;

	if (IS_MSG_FAIL(msg->msg_id)) {
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): MDL_ID(%d)-EVT_ID(%d) fail.\n", __func__,
		          MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));
		return MDL_RET_SUCCESS;
	}

	switch (MSG_MDL_ID_FIELD(msg->msg_id)) {
	case PHL_MDL_POWER_MGNT:
		ret = _ps_mdl_hdl_internal_evt(dispr, ps, msg);
		break;
	default:
		ret = _ps_mdl_hdl_external_evt(dispr, ps, msg);
		break;
	}

end:
	if (ret == MDL_RET_PENDING) {
		if (MSG_EVT_ID_FIELD(msg->msg_id) == MSG_EVT_HW_WATCHDOG)
			recy_info->wdg_leave = true;
		PHL_TRACE(COMP_PHL_PS, _PHL_WARNING_, "[PS_CMD], %s(): MDL_ID(%d)-EVT_ID(%d) is pended.\n", __func__,
		          MSG_MDL_ID_FIELD(msg->msg_id), MSG_EVT_ID_FIELD(msg->msg_id));
	}

	return ret;
}

static enum phl_mdl_ret_code
_ps_mdl_set_info(void *dispr, void *priv, struct phl_module_op_info *info)
{
	struct cmd_ps *ps = (struct cmd_ps *)priv;

	switch (info->op_code) {
	case PS_MDL_OP_CANCEL_PWR_REQ:
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): cancel pwr req, evt_id %d\n", __func__, *(u16 *)info->inbuf);
		if (ps->rej_pwr_req == false)
			_cancel_pwr_req(ps, *(u16 *)info->inbuf);
		break;
	case PS_MDL_OP_BTC_PWR_REQ:
		PHL_TRACE(COMP_PHL_PS, _PHL_INFO_, "[PS_CMD], %s(): btc req pwr %d\n", __func__, *(bool *)info->inbuf);
		ps->btc_req_pwr = *(bool *)info->inbuf;
		break;
	}

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_ps_mdl_query_info(void *dispr, void *priv, struct phl_module_op_info *info)
{
	struct cmd_ps *ps = (struct cmd_ps *)priv;
	u8 pwr_lvl = PS_PWR_LVL_PWROFF;
	struct phl_cmd_ps_basic_info *basic_info = NULL;

	/* PHL_INFO("[PS_CMD], %s(): opcode %d.\n", __func__, info->op_code); */

	switch (info->op_code) {
	case PS_MDL_OP_CUR_PWR_LVL:
		pwr_lvl = ps->cur_pwr_lvl;
		_os_mem_cpy(phl_to_drvpriv(ps->phl_info), (void *)info->inbuf,
					&pwr_lvl, sizeof(pwr_lvl));
		break;
	case PS_MDL_OP_BASIC_INFO:
		basic_info = (struct phl_cmd_ps_basic_info *)info->inbuf;
		basic_info->ps_mode = ps->ps_mode;
		basic_info->cur_pwr_lvl = ps->cur_pwr_lvl;
		basic_info->rej_pwr_req = ps->rej_pwr_req;
		basic_info->btc_req_pwr = ps->btc_req_pwr;
		basic_info->rt_stop_rson = ps->rt_stop_rson;
		basic_info->ap_active = ps->mr_info.ap_active;
		basic_info->gc_active = ps->mr_info.gc_active;
		basic_info->sta = ps->sta;
		basic_info->last_tx_ping_time = ps->time_info.last_tx_ping_time;
		basic_info->last_tx_dhcp_time = ps->time_info.last_tx_dhcp_time;
		basic_info->recy_cnt = ps->recy_info.recy_cnt;
		_os_mem_cpy(phl_to_drvpriv(ps->phl_info),
			    &basic_info->bcn_tracking_i,
			    &ps->bcn_tracking_i,
			    sizeof(struct bcn_tracking_info));
		/* enter/leave ps reason */
		_os_mem_set(phl_to_drvpriv(ps->phl_info), basic_info->enter_rson, 0, MAX_CMD_PS_RSON_LENGTH);
		_os_mem_cpy(phl_to_drvpriv(ps->phl_info), basic_info->enter_rson, ps->enter_rson, MAX_CMD_PS_RSON_LENGTH);
		_os_mem_set(phl_to_drvpriv(ps->phl_info), basic_info->leave_rson, 0, MAX_CMD_PS_RSON_LENGTH);
		_os_mem_cpy(phl_to_drvpriv(ps->phl_info), basic_info->leave_rson, ps->leave_rson, MAX_CMD_PS_RSON_LENGTH);
		break;
	}

	return MDL_RET_SUCCESS;
}

enum rtw_phl_status phl_register_ps_module(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_cmd_dispatch_engine *disp_eng = &(phl_info->disp_eng);
	struct phl_bk_module_ops bk_ops = {0};
	u8 i = 0;

	PHL_INFO("[PS_CMD], %s(): \n", __func__);

	bk_ops.init = _ps_mdl_init;
	bk_ops.deinit = _ps_mdl_deinit;
	bk_ops.start = _ps_mdl_start;
	bk_ops.stop = _ps_mdl_stop;
	bk_ops.msg_hdlr = _ps_mdl_msg_hdlr;
	bk_ops.set_info = _ps_mdl_set_info;
	bk_ops.query_info = _ps_mdl_query_info;

	for (i = 0; i < disp_eng->phy_num; i++) {
		phl_status = phl_disp_eng_register_module(phl_info, i,
						 PHL_MDL_POWER_MGNT, &bk_ops);
		if (phl_status != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("register cmd PS module of phy%d failed.\n", i + 1);
			break;
		}
	}

	return phl_status;
}

u8 phl_ps_get_cur_pwr_lvl(struct phl_info_t *phl_info)
{
	struct phl_module_op_info op_info = {0};
	u8 pwr_lvl = PS_PWR_LVL_MAX;

	op_info.op_code = PS_MDL_OP_CUR_PWR_LVL;
	op_info.inbuf = (u8 *)&pwr_lvl;
	op_info.inlen = sizeof(pwr_lvl);

	phl_disp_eng_query_bk_module_info(phl_info, HW_BAND_0,
			PHL_MDL_POWER_MGNT, &op_info);

	PHL_TRACE(COMP_PHL_PS, _PHL_DEBUG_, "[PS_CMD], %s(): pwr lvl(%s)\n", __func__, phl_ps_pwr_lvl_to_str(pwr_lvl));

	return pwr_lvl;
}

bool phl_ps_is_datapath_allowed(struct phl_info_t *phl_info)
{
	struct phl_module_op_info op_info = {0};
	bool io_allowed = false;
	u8 pwr_lvl = PS_PWR_LVL_MAX;
	struct rtw_ps_cap_t *ps_cap = _get_ps_cap(phl_info);

	if (!ps_cap->lps_pause_tx)
		return true;

	op_info.op_code = PS_MDL_OP_CUR_PWR_LVL;
	op_info.inbuf = (u8 *)&pwr_lvl;
	op_info.inlen = sizeof(pwr_lvl);

	phl_disp_eng_query_bk_module_info(phl_info, HW_BAND_0,
			PHL_MDL_POWER_MGNT, &op_info);

	if (pwr_lvl == PS_PWR_LVL_PWRON)
		io_allowed = true;

	return io_allowed;
}

static void _ps_tx_pkt_ntfy_done(void *priv, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;

	PHL_DBG("[PS_CMD], %s(): reset ntfy\n", __func__);

	_os_atomic_set(phl_to_drvpriv(phl_info),
				   &phl_info->ps_info.tx_ntfy,
				   0);
}

void phl_ps_tx_pkt_ntfy(struct phl_info_t *phl_info)
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	if (phl_ps_get_cur_pwr_lvl(phl_info) == PS_PWR_LVL_PWRON)
		return;

	if (_os_atomic_read(phl_to_drvpriv(phl_info), &phl_info->ps_info.tx_ntfy)) {
		PHL_DBG("[PS_CMD], %s(): already ntfy\n", __func__);
		return;
	}

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_TX_PKT_NTFY);
	msg.band_idx = HW_BAND_0;
	attr.completion.completion = _ps_tx_pkt_ntfy_done;
	attr.completion.priv = phl_info;

	_os_atomic_set(phl_to_drvpriv(phl_info),
				   &phl_info->ps_info.tx_ntfy,
				   1);

	if (phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL) !=
				RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to send tx pkt notify.\n", __func__);
		_os_atomic_set(phl_to_drvpriv(phl_info),
				   	   &phl_info->ps_info.tx_ntfy,
				   	   0);
	}

	return;
}


static void _ps_set_rt_cap_done(void *priv, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;

	if (msg->inbuf && msg->inlen) {
		_os_mem_free(phl_to_drvpriv(phl_info),
			msg->inbuf, msg->inlen);
	}

}

void rtw_phl_ps_set_rt_cap(void *phl, u8 band_idx, bool ps_allow, enum phl_ps_rt_rson rt_rson)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	struct rt_ps *ps_rt_info = NULL;

	ps_rt_info = (struct rt_ps *)_os_mem_alloc(phl_to_drvpriv(phl_info), sizeof(ps_rt_info));
	if (ps_rt_info == NULL) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to alloc ps_rt_info memory.\n", __func__);
		return;
	}

	ps_rt_info->rt_rson = rt_rson;
	ps_rt_info->ps_allow = ps_allow;

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_PS_CAP_CHG);
	msg.band_idx = band_idx;
	msg.inbuf = (u8*)ps_rt_info;
	msg.inlen = sizeof(*ps_rt_info);
	attr.completion.completion = _ps_set_rt_cap_done;
	attr.completion.priv = phl_info;

	if (phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL) !=
				RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_PS, _PHL_ERR_, "[PS_CMD], %s(): fail to notify batter change.\n", __func__);
		goto cmd_fail;
	}

	return;

cmd_fail:
	_os_mem_free(phl_to_drvpriv(phl_info), ps_rt_info, sizeof(ps_rt_info));
}

enum rtw_phl_status
rtw_phl_ps_set_rf_state(void *phl, u8 band_idx, enum rtw_rf_state rf_state)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum phl_msg_evt_id evt_id = (rf_state == RTW_RF_ON) ? MSG_EVT_RF_ON : MSG_EVT_RF_OFF;

	phl_cmd_enqueue(phl_info, band_idx, evt_id, NULL, 0, NULL, PHL_CMD_WAIT, 0);

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_ps_hal_pwr_req(struct rtw_phl_com_t *phl_com, u8 src, bool pwr_req)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct phl_module_op_info op_info = {0};

	if (src != HAL_BTC_PWR_REQ)
		return RTW_PHL_STATUS_FAILURE;

	op_info.op_code = PS_MDL_OP_BTC_PWR_REQ;
	op_info.inbuf = (u8 *)&pwr_req;
	status = phl_disp_eng_set_bk_module_info(phl_info, HW_BAND_0,
				PHL_MDL_POWER_MGNT, &op_info);

	return status;
}
#endif
