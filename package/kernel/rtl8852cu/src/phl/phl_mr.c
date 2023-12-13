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
#define _PHL_MR_C_
#include "phl_headers.h"

#ifdef DBG_PHL_MR
void phl_mr_dump_role_info(const char *caller, const int line, bool show_caller,
	struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole)
{
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (show_caller)
		PHL_INFO("###### FUN - %s LINE - %d #######\n", caller, line);

	PHL_INFO("\t[WROLE] RIDX:%d - MAC-Addr:%02x-%02x-%02x-%02x-%02x-%02x RTYPE:%d MSTS:%d\n",
		wrole->id, wrole->mac_addr[0], wrole->mac_addr[1], wrole->mac_addr[2],
		wrole->mac_addr[3], wrole->mac_addr[4], wrole->mac_addr[5],
		wrole->type, wrole->mstate);

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		PHL_INFO("\t[WROLE LINK] HW Band_idx:%d, Port_idx:%d, WMM_idx:%d\n", rlink->hw_band, rlink->hw_port, rlink->hw_wmm);

		if (rtw_phl_role_is_ap_category(wrole) || wrole->type == PHL_RTYPE_MESH) {
			#ifdef RTW_PHL_BCN
			PHL_INFO("\t[WROLE AP] BSSID:%02x-%02x-%02x-%02x-%02x-%02x\n",
				rlink->bcn_cmn.bssid[0], rlink->bcn_cmn.bssid[1], rlink->bcn_cmn.bssid[2],
				rlink->bcn_cmn.bssid[3], rlink->bcn_cmn.bssid[4], rlink->bcn_cmn.bssid[5]);
			PHL_INFO("\t[WROLE AP] BCN id:%d, interval:%d, rate:0x%04x, DTIM:%d\n",
				rlink->bcn_cmn.bcn_id, rlink->bcn_cmn.bcn_interval,
				rlink->bcn_cmn.bcn_rate, rlink->bcn_cmn.bcn_dtim);
			PHL_INFO("\t[WROLE AP] HW MBSSID idx:%d, MBID NUM:%d\n",
				rlink->hw_mbssid, rlink->mbid_num);
			#endif
		}
	}

	PHL_INFO("\n");
	if (show_caller)
		PHL_INFO("#################################\n");
}

void phl_mr_dump_rlink_info(const char *caller, const int line, bool show_caller,
	struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole,
	struct rtw_wifi_role_link_t *rlink)
{
	if (show_caller)
		PHL_INFO("###### FUN - %s LINE - %d #######\n", caller, line);

	PHL_INFO("\t[RLINK] LIDX:%d - MAC-Addr:%02x-%02x-%02x-%02x-%02x-%02x RTYPE:%d MSTS:%d\n",
		rlink->id,
		rlink->mac_addr[0], rlink->mac_addr[1], rlink->mac_addr[2],
		rlink->mac_addr[3], rlink->mac_addr[4], rlink->mac_addr[5],
		wrole->type, rlink->mstate);

	PHL_INFO("\t[RLINK] HW Band_idx:%d, Port_idx:%d, WMM_idx:%d\n",
		rlink->hw_band, rlink->hw_port, rlink->hw_wmm);

	if (rtw_phl_role_is_ap_category(wrole) || wrole->type == PHL_RTYPE_MESH) {
		#ifdef RTW_PHL_BCN
		PHL_INFO("\t[RLINK AP] BSSID:%02x-%02x-%02x-%02x-%02x-%02x\n",
			rlink->bcn_cmn.bssid[0], rlink->bcn_cmn.bssid[1],
			rlink->bcn_cmn.bssid[2], rlink->bcn_cmn.bssid[3],
			rlink->bcn_cmn.bssid[4], rlink->bcn_cmn.bssid[5]);

		PHL_INFO("\t[RLINK AP] BCN id:%d, interval:%d, rate:0x%04x, DTIM:%d\n",
			rlink->bcn_cmn.bcn_id, rlink->bcn_cmn.bcn_interval,
			rlink->bcn_cmn.bcn_rate, rlink->bcn_cmn.bcn_dtim);

		PHL_INFO("\t[RLINK AP] HW MBSSID idx:%d, MBID NUM:%d\n",
			rlink->hw_mbssid, rlink->mbid_num);
		#endif
	}
	PHL_INFO("\n");
	if (show_caller)
		PHL_INFO("#################################\n");
}

void phl_mr_dump_chctx_info(const char *caller, const int line, bool show_caller,
	struct phl_info_t *phl_info, struct phl_queue *chan_ctx_queue, struct rtw_chan_ctx *chanctx)
{
	u8 role_num = 0;
	role_num = phl_chanctx_get_rnum(phl_info, chan_ctx_queue, chanctx);

	if (show_caller)
		PHL_INFO("###### FUN - %s LINE - %d #######\n", caller, line);

	PHL_INFO("\t[CH-CTX] role num:%d map:0x%02x, DFS enable:%s\n",
		role_num, chanctx->role_map,
		(chanctx->dfs_enabled) ? "Y" : "N");
	PHL_DUMP_CHAN_DEF(&chanctx->chan_def);

	if (show_caller)
		PHL_INFO("#################################\n");
}

const char *const _opmod_str[] = {
	"MR_OP_NONE",
	"MR_OP_SWR",
	"MR_OP_SCC",
	"MR_OP_MCC",
	"MR_OP_DBCC",
	"MR_OP_NON"
};
#define _get_opmod_str(opmod) (((opmod) >= MR_OP_MAX) ? _opmod_str[MR_OP_MAX] : _opmod_str[(opmod)])


void phl_dump_cc_band_map(struct phl_info_t *phl_info, u16 cc_band_map)
{
	PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, "\t chctx_band -");
	if (cc_band_map == 0)
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_NON");
	if (cc_band_map & BIT(CC_BAND_24G))
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_24G");
	if (cc_band_map & BIT(CC_BAND_5G_B1))
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_5G_B1");
	if (cc_band_map & BIT(CC_BAND_5G_B2))
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_5G_B2");
	if (cc_band_map & BIT(CC_BAND_5G_B3))
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_5G_B3");
	if (cc_band_map & BIT(CC_BAND_5G_B4))
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_5G_B4");
	if (cc_band_map & BIT(CC_BAND_6G_U5))
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_6G_U5");
	if (cc_band_map & BIT(CC_BAND_6G_U6))
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_6G_U6");
	if (cc_band_map & BIT(CC_BAND_6G_U7))
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_6G_U7");
	if (cc_band_map & BIT(CC_BAND_6G_U8))
		PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, " %s ", "B_6G_U8");

	PHL_DATA(COMP_PHL_DBG, _PHL_INFO_, "\n");
}

void phl_mr_dump_band_info(const char *caller, const int line, bool show_caller,
	struct phl_info_t *phl_info, struct hw_band_ctl_t *band_ctrl)
{
	u8 role_num = 0;
	int chanctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);

	role_num = phl_mr_get_role_num(phl_info, band_ctrl);

	if (show_caller)
		PHL_INFO("###### FUN - %s LINE - %d #######\n", caller, line);

	PHL_INFO("\t[BAND-%d] op_mode:%s port map:0x%02x, role num:%d map:0x%02x\n",
		band_ctrl->id, _get_opmod_str(band_ctrl->op_mode),
		band_ctrl->port_map, role_num, band_ctrl->role_map);

	/*dump mr_info*/
	PHL_INFO("\t[BAND-%d] sta_num:%d, ld_sta_num:%d, lg_sta_num:%d\n",
		band_ctrl->id, band_ctrl->cur_info.sta_num,
		band_ctrl->cur_info.ld_sta_num, band_ctrl->cur_info.lg_sta_num);
	PHL_INFO("\t[BAND-%d] ap_num:%d, ld_ap_num:%d\n",
		band_ctrl->id, band_ctrl->cur_info.ap_num, band_ctrl->cur_info.ld_ap_num);

	PHL_INFO("\t[BAND-%d] chan_ctx num:%d, cc_band_map:0x%04x\n",
		band_ctrl->id, chanctx_num, band_ctrl->chctx_band_map);
	phl_dump_cc_band_map(phl_info, band_ctrl->chctx_band_map);

	if (chanctx_num) {
		struct rtw_chan_ctx *chanctx = NULL;
		_os_list *chctx_list = &band_ctrl->chan_ctx_queue.queue;
		void *drv = phl_to_drvpriv(phl_info);

		_os_spinlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
		phl_list_for_loop(chanctx, struct rtw_chan_ctx, chctx_list, list) {
			PHL_DUMP_CHAN_CTX(phl_info, &band_ctrl->chan_ctx_queue, chanctx);
		}
		_os_spinunlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
	}
	PHL_INFO("\n");
	if (show_caller)
		PHL_INFO("#################################\n");
}

void phl_mr_dump_info(const char *caller, const int line, bool show_caller,
	struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct rtw_wifi_role_t *wrole;
	u8 i;
	u8 role_num = 0;

	if (show_caller)
		PHL_INFO("###### FUN - %s LINE - %d #######\n", caller, line);

	for (i = 0; i < MAX_WIFI_ROLE_NUMBER; i++) {
		if (mr_ctl->role_map & BIT(i))
			role_num++;
	}

	PHL_INFO("[MR] MAX wrole num:%d, created num:%d map:0x%02x\n",
			MAX_WIFI_ROLE_NUMBER, role_num, mr_ctl->role_map);
	PHL_INFO("[MR] is_db:%s, cck_phyidx:%d\n",
				(mr_ctl->is_db) ? "Y" : "N",
				mr_ctl->cck_phyidx);

	for (i = 0; i < MAX_WIFI_ROLE_NUMBER; i++) {
		if (mr_ctl->role_map & BIT(i)) {
			wrole = phl_get_wrole_by_ridx(phl_info, i);
			PHL_DUMP_ROLE(phl_info, wrole);
		}
	}

	for (i = 0; i < MAX_BAND_NUM; i++)
		PHL_DUMP_BAND_CTL(phl_info, &mr_ctl->band_ctrl[i]);

	if (show_caller)
		PHL_INFO("#################################\n");
}

void phl_mr_dump_cur_chandef(const char *caller, const int line, bool show_caller,
		struct phl_info_t *phl_info, struct rtw_wifi_role_t *wifi_role)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct rtw_chan_def *chan_def = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (show_caller)
		PHL_INFO("###### FUN - %s LINE - %d #######\n", caller, line);

	for (idx = 0; idx < wifi_role->rlink_num; idx++) {
		rlink = get_rlink(wifi_role, idx);

		if (rlink->chanctx) {
			chan_def = &rlink->chanctx->chan_def;
			PHL_INFO("==== MR Chan-def ===\n");
			PHL_DUMP_CHAN_DEF(chan_def);
		}

		chan_def = &rlink->chandef;
		PHL_INFO("==== WR-%d RLINK-%d Chan-def ===\n", wifi_role->id, rlink->id);
		PHL_DUMP_CHAN_DEF(chan_def);

		chan_def = &mr_ctl->hal_com->band[rlink->hw_band].cur_chandef;
		PHL_INFO("==== HAL Band-%d Chan-def ===\n", rlink->hw_band);
		PHL_DUMP_CHAN_DEF(chan_def);
	}

	if (show_caller)
		PHL_INFO("#################################\n");
}

#ifdef	PHL_MR_PROC_CMD
void rtw_phl_mr_dump_info(void *phl, bool show_caller)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	if (show_caller)
		PHL_DUMP_MR(phl_info);
	else
		PHL_DUMP_MR_EX(phl_info);
}
void rtw_phl_mr_dump_band_ctl(void *phl, bool show_caller)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	int i;

	for (i = 0; i < MAX_BAND_NUM; i++) {
		if (show_caller)
			PHL_DUMP_BAND_CTL(phl_info, &mr_ctl->band_ctrl[i]);
		else
			PHL_DUMP_BAND_CTL_EX(phl_info, &mr_ctl->band_ctrl[i]);

	}
}
#endif /*PHL_MR_PROC_CMD*/
#endif /*DBG_PHL_MR*/

static struct rtw_wifi_role_t *_search_ld_sta_wrole(struct rtw_wifi_role_t *wrole,
                                                    struct rtw_wifi_role_link_t *rlink,
                                                    u8 exclude_self)
{
	u8 ridx = 0;
	struct rtw_phl_com_t *phl_com = wrole->phl_com;
	struct rtw_chan_ctx *chanctx = rlink->chanctx;
	struct rtw_wifi_role_t *wr = NULL;

	if (chanctx == NULL) {
		PHL_ERR("%s wifi role(%d) chan ctx is null\n", __func__, wrole->id);
		goto exit;
	}

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (chanctx->role_map & BIT(ridx)) {
			wr = &phl_com->wifi_roles[ridx];
			if (wr) {
				if ((exclude_self) && (wr == wrole))
					continue;
				if (rtw_phl_role_is_client_category(wr))
					break;
			}
		}
	}

	if (wr)
		PHL_INFO("search Linked STA wifi role (%d)\n", wr->id);
exit:
	return wr;
}

void rtw_phl_mr_dump_cur_chandef(void *phl, struct rtw_wifi_role_t *wrole)
{
	#ifdef	PHL_MR_PROC_CMD
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	PHL_DUMP_CUR_CHANDEF(phl_info, wrole);
	#endif
}


static enum rtw_phl_status
_phl_band_ctrl_init(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	struct hw_band_ctl_t *band_ctrl;
	u8 band_idx = 0;

	for (band_idx = 0; band_idx < MAX_BAND_NUM; band_idx++) {
		band_ctrl = &(mr_ctl->band_ctrl[band_idx]);
		band_ctrl->id = band_idx;
		_os_spinlock_init(drv, &(band_ctrl->lock));
		pq_init(drv, &band_ctrl->chan_ctx_queue);
		band_ctrl->op_mode = MR_OP_NONE;
		band_ctrl->tsf_sync_port = HW_PORT_MAX;
		band_ctrl->port_map = 0;
		band_ctrl->role_map = 0;
		band_ctrl->chctx_band_map = 0;
		#ifdef CONFIG_RTW_SUPPORT_MBSSID_VAP
		band_ctrl->mbssid_cnt = 0;
		band_ctrl->mbssid_map = 0;
		#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */
	}

	return RTW_PHL_STATUS_SUCCESS;
}

#ifdef RTW_WKARD_ISSUE_NULL_SLEEP_PROTECTION
#define ISSUE_NULL_TIME 50
#endif

struct mr_scan_chctx {
	struct rtw_chan_def *chandef;
	u8 role_map_ps;/*STA, MESH*/
	u8 role_map_ap;
};

static inline enum rtw_phl_status
_phl_wr_offch_bcn_hdl(struct phl_info_t *phl_info,
		 u8 hw_band,
		 struct rtw_wifi_role_t *wr,
		 bool off_ch
)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_SUCCESS;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	for (idx = 0; idx < wr->rlink_num; idx++) {
		rlink = get_rlink(wr, idx);

		if (rlink->hw_band != hw_band)
			continue;

		if (((TEST_STATUS_FLAG(rlink->status, RLINK_STATUS_BCN_STOP)) && off_ch) ||
		   ((!TEST_STATUS_FLAG(rlink->status, RLINK_STATUS_BCN_STOP)) && !off_ch))
			continue;

		hsts = rtw_hal_beacon_stop(phl_info->hal, rlink, off_ch);
		if (hsts == RTW_HAL_STATUS_SUCCESS) {
			if (off_ch)
				SET_STATUS_FLAG(rlink->status, RLINK_STATUS_BCN_STOP);
			else
				CLEAR_STATUS_FLAG(rlink->status, RLINK_STATUS_BCN_STOP);
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				"%s: wr(%d) rlink(%d)- hw_band(%d) off_ch(%d)\n",
				__func__, wr->id, rlink->id, hw_band, off_ch);
		}
		else {
			PHL_ERR("%s %s beacon failed\n", __func__,
				(off_ch) ? "stop" : "resume");
			psts = RTW_PHL_STATUS_FAILURE;
			break;
		}
	}
	return psts;
}

static inline enum rtw_phl_status
_phl_wr_offch_ps_hdl(struct phl_info_t *phl_info,
		 u8 hw_band,
		 struct rtw_wifi_role_t *wr,
		 bool off_ch,
		 void *obj_priv,
		 bool (*issue_null_data)(void *priv, u8 ridx, u8 lidx, bool ps)
)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	for (idx = 0; idx < wr->rlink_num; idx++) {
		rlink = get_rlink(wr, idx);

		if (rlink->hw_band != hw_band)
			continue;

		if (((TEST_STATUS_FLAG(rlink->status, RLINK_STATUS_PS_ANN)) && off_ch) ||
		    ((!TEST_STATUS_FLAG(rlink->status, RLINK_STATUS_PS_ANN)) && !off_ch))
			continue;

		if (issue_null_data(obj_priv, wr->id, idx, off_ch) == _SUCCESS) {
			if (off_ch) {
				SET_STATUS_FLAG(rlink->status, RLINK_STATUS_PS_ANN);
				#ifdef RTW_WKARD_ISSUE_NULL_SLEEP_PROTECTION
				_os_sleep_ms(phl_to_drvpriv(phl_info), ISSUE_NULL_TIME);
				#endif
			} else {
				CLEAR_STATUS_FLAG(rlink->status, RLINK_STATUS_PS_ANN);
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				"%s: wr(%d) rlink(%d)- hw_band(%d) off_ch(%d)\n",
				__func__, wr->id, rlink->id, hw_band, off_ch);

		} else {
			PHL_ERR("WR-ID:%d, RLINK-ID: %d issue null_data failed\n", wr->id, idx);
			_os_warn_on(1);
			psts = RTW_PHL_STATUS_FAILURE;
			break;
		}
	}

	return psts;
}

#ifdef CONFIG_MR_SUPPORT
static enum rtw_phl_status
_phl_mr_offch_hdl(struct phl_info_t *phl_info,
		 u8 hw_band,
		 bool off_ch,
		 void *obj_priv,
		 bool (*issue_null_data)(void *priv, u8 ridx, u8 lidx, bool ps)
)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[hw_band]);
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_chan_ctx *chanctx = NULL;
	struct rtw_wifi_role_t *wr = NULL;
	struct mr_scan_chctx mctx = {0};
	int ctx_num = 0;
	u8 ridx = 0;
	u8 cur_ch = rtw_hal_get_cur_ch(phl_info->hal, hw_band);
	bool found = false;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */

	if (issue_null_data == NULL) {
		PHL_ERR("%s issue null_data function not found\n", __func__);
		_os_warn_on(1);
		goto _exit;
	}

	ctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);

	if (ctx_num == 0) {
		PHL_DBG("ctx_num == 0!\n");
		goto _exit;
	}

	_os_spinlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
	phl_list_for_loop(chanctx, struct rtw_chan_ctx, &band_ctrl->chan_ctx_queue.queue, list) {
		/* Find the chanctx same as the current channel */
		if(chanctx->chan_def.chan != cur_ch)
			continue;

		PHL_DBG("%s current chanctx found!\n", __FUNCTION__);
		mctx.chandef = &chanctx->chan_def;
		found = true;

		for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
			if (chanctx->role_map & BIT(ridx)) {
				wr = &phl_com->wifi_roles[ridx];
				if(wr->active != true || wr->mstate != MLME_LINKED)
					continue;
				if (rtw_phl_role_is_client_category(wr)) {
					mctx.role_map_ps |= BIT(ridx);
				} else if (rtw_phl_role_is_ap_category(wr) ||
					wr->type == PHL_RTYPE_MESH) {
					mctx.role_map_ap |= BIT(ridx);
				}
			}
		}
	}
	_os_spinunlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);

	if (!found) {
		PHL_DBG("No chanctx is the same as current channel!\n");
		goto _exit;
	}

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if ((mctx.role_map_ap) && (mctx.role_map_ap & BIT(ridx))) {
			wr = &phl_com->wifi_roles[ridx];
			psts = _phl_wr_offch_bcn_hdl(phl_info, hw_band, wr, off_ch);
			if (psts == RTW_PHL_STATUS_FAILURE) {
				PHL_ERR("%s ridx:%d _phl_wr_offch_bcn_hdl failed\n",
					__func__, ridx);
				break;
			}
		}

		/* issue null-data on current channel */
		if ((mctx.role_map_ps) && (mctx.role_map_ps & BIT(ridx))) {
			wr = &phl_com->wifi_roles[ridx];
			psts = _phl_wr_offch_ps_hdl(phl_info, hw_band, wr, off_ch,
						      obj_priv, issue_null_data);
			if (psts == RTW_PHL_STATUS_FAILURE) {
				PHL_ERR("%s ridx:%d _phl_wr_offch_ps_hdl failed\n",
					__func__, ridx);
				break;
			}
		}
	}

_exit:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */

	return psts;
}

#else /*!CONFIG_MR_SUPPORT*/
static enum rtw_phl_status
_phl_wr_offch_hdl(struct phl_info_t *phl_info,
			   u8 hw_band,
			   struct rtw_wifi_role_t *wrole,
			   bool off_ch,
			   void *obj_priv,
			   bool (*issue_null_data)(void *priv, u8 ridx, u8 lidx, bool ps))
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[hw_band]);
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_chan_ctx *chanctx = NULL;
	u8 cur_ch = rtw_hal_get_cur_ch(phl_info->hal, hw_band);
	bool found = false;
	int ctx_num = 0;
#ifdef DBG_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_MONITOR_TIME */

	ctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);

	if (ctx_num == 0){
		PHL_DBG("ctx_num == 0!\n");
		goto _exit;
	}

	_os_spinlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
	phl_list_for_loop(chanctx, struct rtw_chan_ctx, &band_ctrl->chan_ctx_queue.queue, list) {
		/* Find the chanctx same as the current channel */
		if(chanctx->chan_def.chan != cur_ch)
			continue;

		PHL_DBG("%s current chanctx found!\n", __FUNCTION__);
		found = true;
	}
	_os_spinunlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);

	if (!found) {
		PHL_DBG("No chanctx is the same as current channel!\n");
		goto _exit;
	}

	if (rtw_phl_role_is_client_category(wrole)
					&& wrole->mstate == MLME_LINKED) {
		if(issue_null_data == NULL){
			PHL_ERR("%s issue null_data function not found\n", __func__);
			_os_warn_on(1);
			goto _exit;
		}
		psts = _phl_wr_offch_ps_hdl(phl_info,
					      hw_band,
					      wrole,
					      off_ch,
					      obj_priv,
					      issue_null_data);

	} else if (rtw_phl_role_is_ap_category(wrole)) {
		psts = _phl_wr_offch_bcn_hdl(phl_info,
					       hw_band,
					       wrole,
					       off_ch);
	}

_exit:
#ifdef DBG_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_MONITOR_TIME */

	return psts;
}
#endif

enum rtw_phl_status
phl_mr_offch_hdl(struct phl_info_t *phl_info,
                 struct rtw_wifi_role_t *wrole,
                 struct rtw_wifi_role_link_t *rlink,
                 bool off_ch,
                 void *obj_priv,
                 bool (*issue_null_data)(void *priv, u8 ridx, u8 lidx, bool ps)
)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

#ifdef CONFIG_MR_SUPPORT
	psts = _phl_mr_offch_hdl(phl_info, rlink->hw_band, off_ch, obj_priv,
				  issue_null_data);
#else /*!CONFIG_MR_SUPPORT*/
	psts = _phl_wr_offch_hdl(phl_info, rlink->hw_band, wrole,
				  off_ch, obj_priv,
				  issue_null_data);
#endif
	return psts;
}

#ifdef CONFIG_DBCC_SUPPORT
bool rtw_phl_mr_is_db(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);

	return (mr_ctl->is_db == true) ? true : false;
}

bool phl_mr_is_cckphy(struct phl_info_t *phl_info, u8 band_idx)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);

	return (mr_ctl->cck_phyidx == band_idx) ? true : false;
}

void rtw_phl_mr_cancel_dbcc_action(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);

#ifdef CONFIG_DBCC_FORCE
	if (is_frc_dbcc_mode(phl_info->phl_com)) {
		PHL_ERR("%s FORCE DBCC .....\n", __func__);
		return;
	}
#endif
	_os_spinlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
	mr_ctl->trigger_dbcc_cfg = false;
	_os_spinunlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
}

bool phl_mr_is_trigger_dbcc(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);

	return mr_ctl->trigger_dbcc_cfg;
}

enum rtw_phl_status
phl_mr_trig_dbcc_enable(struct phl_info_t *phl_info, bool trig)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);

	/*hal_com->dbcc_en*/
	if (trig == true) {
		_os_spinlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
		if (mr_ctl->is_db == false) /*single band*/
			mr_ctl->trigger_dbcc_cfg = true;
		_os_spinunlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
	} else {
		_os_spinlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
		mr_ctl->trigger_dbcc_cfg = false;
		mr_ctl->is_db = true;
		_os_spinunlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
	}
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
rtw_phl_mr_trig_dbcc_enable(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	#ifdef CONFIG_DBCC_FORCE
	if (is_frc_dbcc_mode(phl_info->phl_com)) {
		PHL_INFO("%s FORCE DBCC .....\n", __func__);
		return RTW_PHL_STATUS_SUCCESS;
	}
	#endif
	return phl_mr_trig_dbcc_enable(phl_info, true);
}

static inline enum rtw_phl_status
phl_mr_dbcc_proto_hdl(struct phl_info_t *phl_info,
				enum phl_band_idx band_idx,
				struct rtw_wifi_role_t *wrole,
				bool dbcc_en)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct dbcc_proto_msg dbcc_proto = {0};
	u8 ret = _FAIL;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	dbcc_proto.dbcc_en = dbcc_en;
	dbcc_proto.wr = wrole;
	ret = mr_ctl->mr_ops.dbcc_protocol_hdl(phl_com->drv_priv, band_idx, &dbcc_proto);
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return (ret == _SUCCESS) ? RTW_PHL_STATUS_SUCCESS
				 : RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
rtw_phl_mr_dbcc_enable(void *phl,
				  enum phl_band_idx band_idx,
				  struct rtw_wifi_role_t *wrole)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status psts;

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_MRC);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_DBCC_ENABLE);
	msg.band_idx = band_idx;
	msg.inbuf = (u8 *)wrole;

	psts = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);

	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: Dispr send msg fail!\n", __func__);
	}

	return psts;
}

static void _phl_mr_pause_trx(struct phl_info_t *phl_info, enum phl_band_idx band_idx)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;

#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	ctl.id = PHL_MDL_MRC;

	ctl.cmd = PHL_DATA_CTL_SW_TX_PAUSE;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_ERR("%s(): pause sw tx failure\n", __func__);
		goto err;
	}

	/* Stop DMA + Disable all of txch dma (Reset B1 TXBD idx to 0) */
	rtw_hal_dbcc_hci_ctrl(phl_info->hal, band_idx, true);
	/* - Redirect packets from old_band(B1) to new_band(B0)
	 *   [PCIE] Memory copy band1 TXBD to band0 DMA channel, update WD (DMA_CH/QSEL/...)
	 * - Or drop packets
	*/
	if (band_idx == HW_BAND_1)
		hci_trx_ops->tx_reset_hwband(phl_info, band_idx);

err:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return;
}
static void _phl_mr_resume_trx(struct phl_info_t *phl_info, enum phl_band_idx band_idx)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_data_ctl_t ctl = {0};
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */

	/* Enable all of txch dma */
	rtw_hal_dbcc_hci_ctrl(phl_info->hal, band_idx, false);

	ctl.id = PHL_MDL_MRC;

	ctl.cmd = PHL_DATA_CTL_SW_TX_RESUME;
	sts = phl_data_ctrler(phl_info, &ctl, NULL);
	if (RTW_PHL_STATUS_SUCCESS != sts) {
		PHL_ERR("%s(): resume sw tx failure\n", __func__);
		goto err;
	}

err:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return;
}

enum phl_mdl_ret_code
_phl_mrc_module_dbcc_enable(void *dispr,
                            void *priv,
                            struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 module_id = MSG_MDL_ID_FIELD(msg->msg_id);
	bool (*core_issue_null_data)(void *, u8, u8, bool) = NULL;
	struct rtw_phl_evt_ops *ops = &phl_com->evt_ops;
	struct rtw_chan_def op_chdef = {0};
	u8 hal_ch = 0;
	int chctx_num;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	if (module_id != PHL_MDL_MRC) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
			"%s: not mrc\n", __FUNCTION__);
		ret = MDL_RET_IGNORE;
		goto _exit;
	}

	role = (struct rtw_wifi_role_t *)msg->inbuf;
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s: Rid(%d)\n",
		__FUNCTION__, role->id);
	rlink = &role->rlink[RTW_RLINK_PRIMARY];
	if (rlink->hw_band != HW_BAND_0) {
		PHL_ERR("wrole(%d) not in band0\n", role->id);
		_os_warn_on(1);
		ret = MDL_RET_FAIL;
		goto _exit;
	}

	hal_ch = rtw_hal_get_cur_ch(phl_info->hal, rlink->hw_band);

	/* 1.issue null(1) - pause Rx*/
	/*switch ch to op-chan*/
	chctx_num = phl_mr_get_chandef_by_band(phl_info, rlink->hw_band, &op_chdef);
	PHL_DUMP_CHAN_DEF_EX(&op_chdef);
	hal_ch = rtw_hal_get_cur_ch(phl_info->hal, rlink->hw_band);
	if (chctx_num == 1) {
		if (hal_ch != op_chdef.chan) {
			PHL_ERR("hw chan(%d) not in op-chan(%d)\n", hal_ch, op_chdef.chan);
			phl_set_ch_bw(phl_info, rlink->hw_band, &op_chdef, false);
		}
	}

	if (ops->issue_null_data)
		core_issue_null_data = ops->issue_null_data;

	_phl_mr_offch_hdl(phl_info,
			  HW_BAND_0,
			  true,
			  phl_com->drv_priv,
			  core_issue_null_data);

	/* 2.pause Tx- SW,HW*/
	_phl_mr_pause_trx(phl_info, HW_BAND_0);
	rtw_hal_dbcc_trx_ctrl(phl_info->hal, phl_com, HW_BAND_0, true);

	/* 3.dbcc hw cfg (en)*/
	rtw_hal_dbcc_pre_cfg(phl_info->hal, phl_com, true);
	rtw_hal_dbcc_cfg(phl_info->hal, phl_com, true);

	/* 4.reallocate hw resouce - role*/
	phl_wifi_role_realloc_band(phl_info, role, rlink, HW_BAND_1);

	rtw_hal_dbcc_reset_hal(phl_info->hal);

	rtw_hal_dbcc_trx_ctrl(phl_info->hal, phl_com, HW_BAND_0, false);
	rtw_hal_dbcc_trx_ctrl(phl_info->hal, phl_com, HW_BAND_1, false);
	/* 5.unpause Tx- SW,HW*/
	_phl_mr_resume_trx(phl_info, HW_BAND_0);

	/* 6.issue null(0) - unpause Rx*/
	_phl_mr_offch_hdl(phl_info,
			  HW_BAND_0,
			  false,
			  phl_com->drv_priv,
			  core_issue_null_data);

	phl_mr_trig_dbcc_enable(phl_info, false);
	ret = MDL_RET_SUCCESS;
	PHL_INFO("%s wr(%d) success....\n", __func__, role->id);
_exit:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return ret;
}

enum rtw_phl_status
phl_mr_trig_dbcc_disable(struct phl_info_t *phl_info, bool trig)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);

	/*hal_com->dbcc_en*/
	if (trig == true) {
		_os_spinlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
		if (mr_ctl->is_db == true) /*dual band*/
			mr_ctl->trigger_dbcc_cfg = true;
		_os_spinunlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
	} else {
		_os_spinlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
		mr_ctl->trigger_dbcc_cfg = false;
		mr_ctl->is_db = false;
		_os_spinunlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
	}
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
rtw_phl_mr_trig_dbcc_disable(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	#ifdef CONFIG_DBCC_FORCE
	if (is_frc_dbcc_mode(phl_info->phl_com)) {
		PHL_INFO("%s FORCE DBCC .....\n", __func__);
		return RTW_PHL_STATUS_SUCCESS;
	}
	#endif
	return phl_mr_trig_dbcc_disable(phl_info, true);
}

enum rtw_phl_status
rtw_phl_mr_dbcc_disable(void *phl,
				    enum phl_band_idx band_idx,
				    struct rtw_wifi_role_t *wrole)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	enum rtw_phl_status psts;

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_MRC);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_DBCC_DISABLE);
	msg.band_idx = band_idx;
	msg.inbuf = (u8 *)wrole;

	psts = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);

	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: Dispr send msg fail!\n", __func__);
	}
	return psts;
}

static inline void
_phl_mr_band_info_cleanup(struct rtw_phl_com_t *phl_com, struct hw_band_ctl_t *band_ctrl)
{
	_os_mem_set(phl_com->drv_priv, &band_ctrl->cur_info, 0, sizeof(struct mr_info));
	band_ctrl->port_map = 0;
	band_ctrl->role_map = 0;
	band_ctrl->wmm_map = 0;
	band_ctrl->op_mode = MR_OP_NONE;
	band_ctrl->op_type = MR_OP_TYPE_NONE;
	band_ctrl->chctx_band_map = 0;

	if (band_ctrl->chan_ctx_queue.cnt) {
		PHL_ERR("%s Band_%d chan_ctx_queue.cnt(%d) not clearn up\n",
			__func__, band_ctrl->id, band_ctrl->chan_ctx_queue.cnt);
		_os_warn_on(1);
	}
}

static enum rtw_phl_status
_phl_dbcc_sync_rxfilter(struct phl_info_t *phl_info, enum phl_band_idx band_idx)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);
	struct rtw_wifi_role_t *role = NULL, *wr = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 ridx = 0;
	enum rtw_phl_status ret = RTW_PHL_STATUS_SUCCESS;

	PHL_INFO("%s: band=%d\n", __func__, band_idx);
	wr = phl_mr_get_role_by_bandidx(phl_info, band_idx);
	if (wr == NULL) {
		PHL_INFO("%s: can't find any role for band(%d)\n", __func__, band_idx);
		_os_warn_on(1);
		ret = RTW_PHL_STATUS_FAILURE;
		goto exit;
	}

	rlink = &wr->rlink[0];

	ret = phl_mr_info_upt(phl_info, rlink);
	if (ret !=  RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: phl_mr_info_upt fail(%d)\n", __func__, ret);
		goto exit;
	}

	ret = rtw_phl_mr_rx_filter_opt(phl_info, rlink);
	if (ret !=  RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: rtw_phl_mr_rx_filter_opt fail(%d)\n", __func__, ret);
		goto exit;
	}

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (band_ctrl->role_map & BIT(ridx)) {
			role = &(phl_com->wifi_roles[ridx]);
			rlink = &role->rlink[0];
			if (role->mstate == MLME_LINKED && rtw_phl_role_is_client_category(role)) {
				ret = rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, rlink, RX_FLTR_TYPE_MODE_STA_CONN);
				if (ret !=  RTW_PHL_STATUS_SUCCESS) {
					PHL_ERR("%s: rxfltr_type_by_mode fail(%d), role id=%d, rlink id=%d, mode=%d\n",
						__func__, ret, role->id, rlink->id, RX_FLTR_TYPE_MODE_STA_CONN);
					goto exit;
				}
			}
			if (rtw_phl_role_is_ap_category(role)) {
				ret = rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, rlink, RX_FLTR_TYPE_MODE_AP_CONN);
				if (ret !=  RTW_PHL_STATUS_SUCCESS) {
					PHL_ERR("%s: rxfltr_type_by_mode fail(%d), id=%d, rlink id=%d, mode=%d\n",
						__func__, ret, role->id, rlink->id, RX_FLTR_TYPE_MODE_AP_CONN);
					goto exit;
				}
			}
		}
	}
exit:
	return ret;
}

enum phl_mdl_ret_code
_phl_mrc_module_dbcc_disable(void *dispr,
                             void *priv,
                             struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band0_ctrl = &(mr_ctl->band_ctrl[HW_BAND_0]);
	struct hw_band_ctl_t *band1_ctrl = &(mr_ctl->band_ctrl[HW_BAND_1]);
	u8 role_num = 0, ridx;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_t *wr = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 module_id = MSG_MDL_ID_FIELD(msg->msg_id);
	bool (*core_issue_null_data)(void *priv, u8 ridx, u8 lidx, bool ps) = NULL;
	struct rtw_phl_evt_ops *ops = &phl_com->evt_ops;
	struct rtw_chan_def op_chdef = {0};
	int chctx_num = 0;
	enum phl_band_idx band_idx = HW_BAND_0;
	u8 hal_ch = 0;
	int b0_chctx_num = 0;
	int b1_chctx_num = 0;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	if ((module_id != PHL_MDL_MRC) &&
	    (module_id != PHL_FG_MDL_DISCONNECT) &&
	    (module_id != PHL_FG_MDL_CONNECT) &&
	    (module_id != PHL_FG_MDL_AP_STOP) &&
	    (module_id != PHL_FG_MDL_AP_START)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
			"%s: msg not from DISC-CONT/AP-STOP/START FG or MRC\n", __FUNCTION__);
		ret = MDL_RET_IGNORE;
		goto _exit;
	}

	if (ops->issue_null_data)
		core_issue_null_data = ops->issue_null_data;

	role = (struct rtw_wifi_role_t *)msg->inbuf;
	rlink = &role->rlink[RTW_RLINK_PRIMARY];

	if ((MSG_EVT_ID_FIELD(msg->msg_id) == MSG_EVT_DISCONNECT_END) ||
	    (MSG_EVT_ID_FIELD(msg->msg_id) == MSG_EVT_AP_STOP_END)) {
		bool rd_enabled = false;

		if (rlink->hw_band == HW_BAND_0)
			band_idx = HW_BAND_1;

		b0_chctx_num = phl_mr_get_chanctx_num(phl_info, band0_ctrl);
		PHL_INFO("%s Band_0 has chctx_num(%d)\n", __func__, b0_chctx_num);
		b1_chctx_num = phl_mr_get_chanctx_num(phl_info, band1_ctrl);
		PHL_INFO("%s Band_1 has chctx_num(%d)\n", __func__, b1_chctx_num);

		chctx_num = phl_mr_get_chandef_by_band(phl_info, band_idx, &op_chdef);
		PHL_INFO("Band_%d - op ch(%d), band(%d), offset(%d)\n",
				band_idx, op_chdef.chan, op_chdef.band, op_chdef.offset);

		if (chctx_num >= 1) {
			/*TODO - chctx_num == 2, get first chan_ctx*/
			hal_ch = rtw_hal_get_cur_ch(phl_info->hal, band_idx);
			if (hal_ch != op_chdef.chan) {
				PHL_ERR("hw chan(%d) not in op-chan(%d)\n", hal_ch, op_chdef.chan);
				phl_set_ch_bw(phl_info, band_idx, &op_chdef, false);
			}
		}

		/* 1.issue null(1) - pause Rx,stop beacon*/
		_phl_mr_offch_hdl(phl_info,
				  band_idx,
				  true,
				  phl_com->drv_priv,
				  core_issue_null_data);

		/* 2.pause Tx- SW,HW*/
		_phl_mr_pause_trx(phl_info, HW_BAND_1);
		rtw_hal_dbcc_trx_ctrl(phl_info->hal, phl_com, HW_BAND_0, true);
		rtw_hal_dbcc_trx_ctrl(phl_info->hal, phl_com, HW_BAND_1, true);

		/* 3.reallocate hw resouce all of role in band 1,before disable cmac1*/
		role_num = phl_mr_get_role_num(phl_info, band1_ctrl);
		if (role_num == 0) {
			PHL_ERR("BAND_1 role num == 0\n");
			_os_warn_on(1);
		}
		PHL_INFO("%s band-1 - role_num:%d\n", __func__, role_num);
		for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
			if (band1_ctrl->role_map & BIT(ridx)) {
				wr = &(phl_com->wifi_roles[ridx]);
				psts = phl_wifi_role_realloc_band(phl_info, wr, &wr->rlink[RTW_RLINK_PRIMARY], HW_BAND_0);
				if (psts == RTW_PHL_STATUS_FAILURE) {
					PHL_ERR("%s phl_wifi_role_realloc_band failed\n",
						__func__);
					break;
				}
			}
		}

		/* 4.move bnad1's chan_ctx to band0*/
		if (b1_chctx_num) {
			if (b0_chctx_num != 0) {
				PHL_ERR("%s Band_0 has chctx_num(%d)\n", __func__, b0_chctx_num);
				_os_warn_on(1);
			}
			phl_chanctx_switch(phl_info, band0_ctrl, band1_ctrl);
			rtw_hal_dbcc_band_switch_hdl(phl_info->hal, HW_BAND_1);
		}

		/* 5.dbcc hw cfg (en = false)*/
		rtw_hal_dbcc_pre_cfg(phl_info->hal, phl_com, false);
		rtw_hal_dbcc_cfg(phl_info->hal, phl_com, false);

		/* 6. clean up all of sw vaule of band1*/
		_phl_mr_band_info_cleanup(phl_com, band1_ctrl);

		/* 7. restore_phy0_ch*/
		#ifdef CONFIG_PHL_DFS
		rd_enabled = phl_is_radar_detect_enabled(phl_info, HW_BAND_0);
		#endif
		rtw_hal_set_ch_bw(phl_info->hal, HW_BAND_0, &op_chdef, true, rd_enabled, true);
		PHL_INFO("restore_phy0_ch - ch(%d), band(%d), offset(%d)\n",
				op_chdef.chan, op_chdef.band, op_chdef.offset);

		rtw_hal_dbcc_trx_ctrl(phl_info->hal, phl_com, HW_BAND_0, false);
		rtw_hal_dbcc_trx_ctrl(phl_info->hal, phl_com, HW_BAND_1, false);

		/* 8. unpause Tx- SW,HW*/
		_phl_mr_resume_trx(phl_info, HW_BAND_0);

		/* 9. check rx filter before unpause RX */
		_phl_dbcc_sync_rxfilter(phl_info, HW_BAND_0);

		/* 10. issue null(0) - unpause Rx*/
		_phl_mr_offch_hdl(phl_info,
				  HW_BAND_0,
				  false,
				  phl_com->drv_priv,
				  core_issue_null_data);
		PHL_INFO("%s wr(%d) phase-1 success....\n", __func__, role->id);
	}
	else if (MSG_EVT_ID_FIELD(msg->msg_id) == MSG_EVT_DBCC_DISABLE) {
		phl_mr_trig_dbcc_disable(phl_info, false);
		rtw_hal_notification(phl_info->hal,
				     MSG_EVT_DBCC_DISABLE,
				     HW_BAND_0);
		PHL_INFO("%s wr(%d) phase-2 success....\n", __func__, role->id);
	}
	ret = MDL_RET_SUCCESS;
_exit:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return ret;
}
#endif /*CONFIG_DBCC_SUPPORT*/

#ifdef CONFIG_CMD_DISP
static enum phl_mdl_ret_code
_phl_mrc_module_init(void *phl_info, void *dispr, void **priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	FUNCIN();
	*priv = phl_info;
	FUNCOUT();

	ret = MDL_RET_SUCCESS;
	return ret;
}

static void
_phl_mrc_module_deinit(void *dispr, void *priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	FUNCIN();
	FUNCOUT();

	ret = MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_phl_mrc_module_start(void *dispr, void *priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	FUNCIN();
#ifdef CONFIG_RTW_SUPPORT_MBSSID_VAP
	/* Ensure M-BSSID map cleaned up in case of VAP roles were not turned
	   off normally. */
	do {
		struct mr_ctl_t *mr_ctl = phl_to_mr_ctrl(priv);
		u8 band_idx = 0;

		for (band_idx = 0; band_idx < MAX_BAND_NUM; band_idx++) {
			struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);

			band_ctrl->mbssid_map = 0;
			band_ctrl->mbssid_cnt = 0;
		}
	} while (0);
#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */
	FUNCOUT();

	ret = MDL_RET_SUCCESS;
	return ret;
}

static enum phl_mdl_ret_code
_phl_mrc_module_stop(void *dispr, void *priv)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	FUNCIN();
	FUNCOUT();

	ret = MDL_RET_SUCCESS;
	return ret;
}

/* Same behaviour as rtw_phl_connect_prepare without cmd dispr */
enum rtw_phl_status
_phl_mrc_module_connect_start_hdlr(struct phl_info_t *phl_info,
                                   struct rtw_wifi_role_t *wrole,
                                   struct rtw_wifi_role_link_t *rlink)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: wrole->id(%d)\n",
		  __func__, wrole->id);
	if (rtw_phl_role_is_client_category(wrole)) {
		wrole->mstate = MLME_LINKING;
	}

	psts = phl_mr_info_upt(phl_info, rlink);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s mr info upt failed\n", __func__);
		goto _exit;
	}

	if (!rtw_phl_role_is_ap_category(wrole)) {
		psts = rtw_phl_mr_rx_filter_opt(phl_info, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
			goto _exit;
		}
	}
	PHL_DUMP_MR_EX(phl_info);
_exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: psts(%d)\n",
		  __func__, psts);
	return psts;
}

/* Same behaviour as rtw_phl_connected without cmd dispr */
enum rtw_phl_status
_phl_mrc_module_connect_end_hdlr(struct phl_info_t *phl_info,
				 struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	u8 idx = 0;
	struct rtw_wifi_role_link_t *rlink = NULL;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: wrole->id(%d)\n",
		  __func__, wrole->id);
	if (rtw_phl_role_is_client_category(wrole)) {
		psts = phl_role_notify(phl_info, wrole);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s sta role notify failed\n", __func__);
			goto _exit;
		}
	}

	if (!rtw_phl_role_is_ap_category(wrole)) {

		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rlink = get_rlink(wrole, idx);
			if (rlink->mstate != MLME_LINKED)
				continue;

			psts = phl_mr_info_upt(phl_info, rlink);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s mr info upt failed\n", __func__);
				goto _exit;
			}

			psts = rtw_phl_mr_rx_filter_opt(phl_info, rlink);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
				goto _exit;
			}

			psts = rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, rlink,
								  RX_FLTR_TYPE_MODE_STA_CONN);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s set mr_rx_filter_type failed\n", __func__);
				goto _exit;
			}

			psts = phl_mr_tsf_sync(phl_info, wrole, rlink, PHL_ROLE_MSTS_STA_CONN_END);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s set mr_tsf_sync failed\n", __func__);
				goto _exit;
			}
		}
	} else {
		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rlink = get_rlink(wrole, idx);

			psts = phl_mr_info_upt(phl_info, rlink);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s mr info upt failed\n", __func__);
				goto _exit;
			}

			psts = rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, rlink,
								  RX_FLTR_TYPE_MODE_AP_CONN);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s set mr_rx_filter_type failed\n", __func__);
				goto _exit;
			}
		}
	}

	PHL_DUMP_MR_EX(phl_info);
_exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: psts(%d)\n",
		  __func__, psts);
	return psts;
}

/* Same behaviour as rtw_phl_disconnect without cmd dispr */
enum rtw_phl_status
_phl_mrc_module_disconnect_hdlr(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	u8 idx = 0;
	struct rtw_wifi_role_link_t *rlink = NULL;
#ifdef CONFIG_PHL_TDLS
	enum role_type rtype = PHL_RTYPE_STATION;
#endif
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: wrole->id(%d)\n",
		  __func__, wrole->id);
#ifdef CONFIG_PHL_TDLS
	/* Disconnected with AP while there still exist linked TDLS peers */
	if (wrole->type == PHL_RTYPE_TDLS && wrole->mstate != MLME_LINKED) {
		psts = phl_wifi_role_change(phl_info, wrole, NULL, WR_CHG_TYPE, (u8*)&rtype);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			RTW_ERR("%s - change to phl role type = %d fail with error = %d\n", __func__, rtype, psts);
			goto _exit;
		}
	}
#endif
	if (rtw_phl_role_is_client_category(wrole)) {
		psts = phl_role_notify(phl_info, wrole);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s sta role notify failed\n", __func__);
			goto _exit;
		}
	}

	if (!rtw_phl_role_is_ap_category(wrole)) {

		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rlink = get_rlink(wrole, idx);

			psts = phl_mr_info_upt(phl_info, rlink);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s mr info upt failed\n", __func__);
				goto _exit;
			}

			psts = rtw_phl_mr_rx_filter_opt(phl_info, rlink);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
				goto _exit;
			}

			psts = rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, rlink,
								  RX_FLTR_TYPE_MODE_STA_DIS_CONN);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s set mr_rx_filter_type failed\n", __func__);
				goto _exit;
			}

			psts = phl_mr_tsf_sync(phl_info, wrole, rlink, PHL_ROLE_MSTS_STA_DIS_CONN);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s set mr_tsf_sync failed\n", __func__);
				goto _exit;
			}

			rtw_hal_disconnect_notify(phl_info->hal, &rlink->chandef);
		}
	} else {
		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rlink = get_rlink(wrole, idx);

			psts = phl_mr_info_upt(phl_info, rlink);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s mr info upt failed\n", __func__);
				goto _exit;
			}

			psts = rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, rlink,
								  RX_FLTR_TYPE_MODE_AP_DIS_CONN);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s set mr_rx_filter_type failed\n", __func__);
				goto _exit;
			}
		}
	}

	PHL_DUMP_MR_EX(phl_info);
_exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: psts(%d)\n",
		  __func__, psts);
	return psts;
}

/* Same behaviour as rtw_phl_ap_started without cmd dispr */
enum rtw_phl_status
_phl_mrc_module_ap_started_hdlr(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: wrole->id(%d)\n",
		  __func__, wrole->id);
	psts = phl_role_notify(phl_info, wrole);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s role notify failed\n", __func__);
		goto _exit;
	}

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		psts = phl_mr_info_upt(phl_info, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s mr info upt failed\n", __func__);
			goto _exit;
		}
		psts = rtw_phl_mr_rx_filter_opt(phl_info, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
			goto _exit;
		}

		psts = phl_mr_tsf_sync(phl_info, wrole, rlink, PHL_ROLE_MSTS_AP_START);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_tsf_sync failed\n", __func__);
			goto _exit;
		}
	}

	PHL_DUMP_MR_EX(phl_info);
_exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: psts(%d)\n",
		  __func__, psts);
	return psts;
}


/* Same behaviour as rtw_phl_ap_stop without cmd dispr */
enum rtw_phl_status
_phl_mrc_module_ap_stop_hdlr(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	u8 idx = 0;
	struct rtw_wifi_role_link_t *rlink = NULL;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: wrole->id(%d)\n",
		  __func__, wrole->id);
	wrole->mstate = MLME_NO_LINK;
	psts = phl_role_notify(phl_info, wrole);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s role notify failed\n", __func__);
		goto _exit;
	}

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);
		rtw_hal_disconnect_drop_all_pkt(phl_info->hal,
				rtw_phl_get_stainfo_self(phl_info, rlink));
		psts = phl_mr_info_upt(phl_info, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s mr info upt failed\n", __func__);
			goto _exit;
		}
		psts = rtw_phl_mr_rx_filter_opt(phl_info, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
			goto _exit;
		}

		psts = phl_mr_tsf_sync(phl_info, wrole, rlink, PHL_ROLE_MSTS_AP_STOP);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_tsf_sync failed\n", __func__);
			goto _exit;
		}

		rtw_hal_disconnect_notify(phl_info->hal, &rlink->chandef);
	}

	PHL_DUMP_MR_EX(phl_info);
_exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: psts(%d)\n",
		  __func__, psts);
	return psts;
}

enum phl_mdl_ret_code
_phl_mrc_module_swch_start_hdlr(void *dispr,
				void *priv,
				struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	struct phl_module_op_info op_info = {0};
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_chan_def chandef = {0};
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	u8 module_id = MSG_MDL_ID_FIELD(msg->msg_id);
	struct rtw_phl_evt_ops *ops = &phl_com->evt_ops;
	u8 idx = 0xff, hw_band = 0;
	struct rtw_wifi_role_link_t *rlink = NULL;
	bool (*core_issue_null_data)(void *, u8, u8, bool) = NULL;

	phl_dispr_get_idx(dispr, &idx);
	/*
	* Handle mr offchan before switching channel when
	* STA connect & AP start.
	*/
	if((module_id != PHL_FG_MDL_CONNECT) &&
	   (module_id != PHL_FG_MDL_AP_START) &&
	   (module_id != PHL_FG_MDL_SCAN)){
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		"%s: not connect/apstart/scan\n", __FUNCTION__);
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		"%s[%d]: MSG_EVT_SWCH_START\n", __FUNCTION__, msg->band_idx);

	op_info.op_code = FG_REQ_OP_GET_ROLE;
	if(phl_disp_eng_query_cur_cmd_info(phl_info, idx, &op_info)
		!= RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
				"Query wifi role fail!\n");
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)op_info.outbuf;
	if(role == NULL){
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
			"%s: role is NULL\n", __FUNCTION__);
		goto _exit;
	}

	op_info.op_code = FG_REQ_OP_GET_ROLE_LINK;
	if(phl_disp_eng_query_cur_cmd_info(phl_info, idx, &op_info)
		!= RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
				"Query wifi role link fail!\n");
		goto _exit;
	}
	rlink = (struct rtw_wifi_role_link_t *)op_info.outbuf;
	if(rlink == NULL){
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
			"%s: role link is NULL\n", __FUNCTION__);
		goto _exit;
	}

	hw_band = rlink->hw_band;

	/*
	* If we are already on STA/AP channel,
	* offch is unnecessary.
	*/
	if((module_id == PHL_FG_MDL_CONNECT) ||
	   (module_id == PHL_FG_MDL_AP_START)){
	   	#ifdef CONFIG_MR_SUPPORT
		chandef = mr_ctl->hal_com->band[hw_band].cur_chandef;

		if(rlink->chandef.chan == chandef.chan){
			ret = MDL_RET_SUCCESS;
			goto _exit;
		}
		#else
		ret = MDL_RET_SUCCESS;
		goto _exit;
		#endif
	}

	if (ops->issue_null_data)
		core_issue_null_data = ops->issue_null_data;

	phl_mr_offch_hdl(phl_info,
	                 role,
	                 rlink,
	                 true,
	                 phl_com->drv_priv,
	                 core_issue_null_data);

	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

enum phl_mdl_ret_code
_phl_mrc_module_swch_done_hdlr(void *dispr,
			       void *priv,
			       struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_wifi_role_t *role = NULL;
	u8 module_id = MSG_MDL_ID_FIELD(msg->msg_id);
	struct phl_scan_channel scan_ch = {0};
	struct rtw_phl_evt_ops *ops = &phl_info->phl_com->evt_ops;
	struct rtw_wifi_role_link_t *rlink = NULL;
	bool (*core_issue_null_data)(void *, u8, u8, bool) = NULL;

	/*
	* Handle mr offchan after switching channel to op channel
	*/
	if(module_id != PHL_FG_MDL_SCAN){
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
		"%s[%d]: MSG_EVT_SWCH_DONE\n", __FUNCTION__, msg->band_idx);
	scan_ch = *(struct phl_scan_channel *)(msg->inbuf);

	/* Issue null 0 and resume beacon when BACKOP during scanning */
	if(scan_ch.scan_mode != BACKOP_MODE){
		ret = MDL_RET_SUCCESS;
		goto _exit;
	}

	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[1].ptr;

	if (ops->issue_null_data)
		core_issue_null_data = ops->issue_null_data;

	phl_mr_offch_hdl(phl_info,
	                 role,
	                 rlink,
	                 false,
	                 phl_com->drv_priv,
	                 core_issue_null_data);

	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}


static enum rtw_phl_status
_mrc_module_chg_op_chdef_end_pre_hdlr(u8 *param)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct chg_opch_param *ch_param = (struct chg_opch_param *)param;
	struct rtw_wifi_role_t *wrole = ch_param->wrole;
	struct phl_info_t *phl = wrole->phl_com->phl_priv;
	struct rtw_phl_evt_ops *ops = ops = &phl->phl_com->evt_ops;
	bool (*core_issue_null)(void *, u8, u8, bool) = NULL;

	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: wrole->id(%d)\n",
		  __func__, wrole->id);
	/* Handle mr offchan after switching channel to new op channel */
	/* If the new pri-ch is as same as ori pri-ch, offch is unnecessary. */
	if(ch_param->new_chdef.chan == ch_param->ori_chdef.chan) {
		psts = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	}

	if (ops->issue_null_data) {
		core_issue_null = ops->issue_null_data;
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Ops issue_null_data is NULL\n",
			__func__);
	}

	if (RTW_PHL_STATUS_SUCCESS != phl_mr_offch_hdl(phl,
	                                               wrole,
	                                               ch_param->rlink,
	                                               false,
	                                               phl->phl_com->drv_priv,
	                                               core_issue_null)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "%s: Fail to offch\n",
			__func__);
		goto exit;
	}
	psts = RTW_PHL_STATUS_SUCCESS;
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: psts(%d)\n",
		  __func__, psts);
	return psts;
}

static enum phl_mdl_ret_code
_mrc_module_chg_op_chdef_start_hdlr(void *dispr, void *priv,
			 struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl = (struct phl_info_t *)priv;
	struct rtw_phl_evt_ops *ops = &phl->phl_com->evt_ops;
	struct chg_opch_param *ch_param = NULL;
	struct rtw_wifi_role_t *wrole = NULL;
	bool (*core_issue_null)(void *, u8, u8, bool) = NULL;
	u8 *cmd = NULL;
	u32 cmd_len;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_GENERAL) {
		ret = MDL_RET_IGNORE;
		goto exit;
	}
	if (phl_cmd_get_cur_cmdinfo(phl, msg->band_idx, msg, &cmd, &cmd_len)
						!= RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Fail to get cmd info \n",
			  __func__);
		goto exit;
	}
	ch_param = (struct chg_opch_param *)cmd;
	wrole = ch_param->wrole;
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s: wrole->id(%d)\n",
		  __func__, wrole->id);
	/* Handle mr offchan before switching channel */
	/* If the new pri-ch is as same as old pri-ch, offch is unnecessary. */
	if (ch_param->new_chdef.chan == ch_param->ori_chdef.chan) {
		ret = MDL_RET_SUCCESS;
		goto exit;
	}
	if (ops->issue_null_data) {
		core_issue_null = ops->issue_null_data;
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Ops issue_null_data is NULL\n",
			__func__);
	}

	if (RTW_PHL_STATUS_SUCCESS != phl_mr_offch_hdl(phl,
	                                               wrole,
	                                               ch_param->rlink,
	                                               true,
	                                               phl->phl_com->drv_priv,
	                                               core_issue_null)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "%s: Fail to offch\n",
			__func__);
		goto exit;
	}
	ret = MDL_RET_SUCCESS;
exit:
	return ret;
}

static enum phl_mdl_ret_code
_mrc_module_msg_pre_hdlr(void *dispr, void *priv, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl = (struct phl_info_t *)priv;
	u8 *cmd = NULL;
	u32 cmd_len;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_CHG_OP_CH_DEF_END:
		if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_GENERAL) {
			ret = MDL_RET_IGNORE;
			break;
		}
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s: MSG_EVT_CHG_OP_CH_DEF_END\n",
			__FUNCTION__);
		if (RTW_PHL_STATUS_SUCCESS != phl_cmd_get_cur_cmdinfo(phl,
					msg->band_idx, msg, &cmd, &cmd_len)) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Fail to get cmd info \n",
				  __func__);
			break;
		}
		if (RTW_PHL_STATUS_SUCCESS !=
			_mrc_module_chg_op_chdef_end_pre_hdlr(cmd)) {
			break;
		}
		ret = MDL_RET_SUCCESS;
		break;
	default:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s: MDL ID(%d), Event ID(%d), Not handle event in pre-phase\n",
			__FUNCTION__, MSG_MDL_ID_FIELD(msg->msg_id),
			MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_IGNORE;
		break;
	}
	if (ret == MDL_RET_FAIL) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			"%s: MDL(%d), EVT(%d), handle event failed\n",
			__FUNCTION__, MSG_MDL_ID_FIELD(msg->msg_id),
			MSG_EVT_ID_FIELD(msg->msg_id));
	}
	return ret;
}

static enum phl_mdl_ret_code
_mrc_module_msg_post_hdl(void *dispr,
			 void *priv,
			 struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_chan_def chandef = {0};
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
#if defined(CONFIG_STA_CMD_DISPR) || defined(CONFIG_AP_CMD_DISPR)
	struct phl_module_op_info op_info = {0};
#endif
	struct rtw_phl_evt_ops *ops = &phl_com->evt_ops;
	u8 idx = 0xff, hw_band = 0;
	struct rtw_phl_stainfo_t *sta = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	bool (*core_issue_null_data)(void *, u8, u8, bool) = NULL;
	u8 *cmd = NULL;
	u32 cmd_len;

	if (ops->issue_null_data)
		core_issue_null_data = ops->issue_null_data;

	phl_dispr_get_idx(dispr, &idx);
	switch(MSG_EVT_ID_FIELD(msg->msg_id)) {
		case MSG_EVT_CHG_OP_CH_DEF_START:
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s: MSG_EVT_CHG_OP_CH_DEF_START\n", __FUNCTION__);
			ret = _mrc_module_chg_op_chdef_start_hdlr(dispr, priv, msg);
		break;

		case MSG_EVT_SCAN_START:
			ret = MDL_RET_SUCCESS;
			break;
		case MSG_EVT_SCAN_END:
			if(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_SCAN){
				ret = MDL_RET_IGNORE;
				break;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_SCAN_END\n", __FUNCTION__, idx);

			role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			/* use primary link to serve scan request for legacy/MLD */
			rlink = &role->rlink[RTW_RLINK_PRIMARY];
			hw_band = rlink->hw_band;

			if (phl_mr_get_chandef(phl_info,
						role,
						rlink,
						false,
						&chandef) !=
					RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s phl_mr_get_chandef failed\n", __func__);
				break;
			}

			PHL_DUMP_CHAN_DEF_EX(&chandef);

			phl_set_ch_bw(phl_info, hw_band, &chandef, false);
			phl_mr_offch_hdl(phl_info,
					  role,
					  rlink,
					  false,
					  phl_com->drv_priv,
					  core_issue_null_data);
			ret = MDL_RET_SUCCESS;
			break;

		case MSG_EVT_SWCH_START:
			if (MSG_MDL_ID_FIELD(msg->msg_id) == PHL_MDL_GENERAL) {
				struct setch_param *ch_param = NULL;

				if (phl_cmd_get_cur_cmdinfo(phl_info, idx, msg, &cmd, &cmd_len) ==
					RTW_PHL_STATUS_SUCCESS) {
					ch_param = (struct setch_param *)cmd;
					PHL_INFO("[MRC] GEN_CMD_MDL Switch chan ......\n");
					PHL_INFO("[MRC] wrole idx:%d\n", ch_param->wrole->id);
					PHL_INFO("[MRC] ch:%d, bw:%d, offset:%d\n",
							ch_param->chdef.chan,
							ch_param->chdef.bw,
							ch_param->chdef.offset);
					PHL_INFO("[MRC] do_rfk:%d\n", ch_param->do_rfk);
					PHL_INFO("[MRC] GEN_CMD_MDL Switch chan ......\n");
				}
				ret = MDL_RET_SUCCESS;
			} else {
				ret = _phl_mrc_module_swch_start_hdlr(dispr,
								      priv,
								      msg);
			}
			break;
		case MSG_EVT_SWCH_DONE:
			ret = _phl_mrc_module_swch_done_hdlr(dispr,
							     priv,
							     msg);
			break;

		case MSG_EVT_TSF_SYNC_DONE:
			if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_GENERAL){
				ret = MDL_RET_IGNORE;
				break;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_TSF_SYNC_DONE\n", __FUNCTION__, idx);

			if (phl_cmd_get_cur_cmdinfo(phl_info, msg->band_idx,
						    msg, &cmd, &cmd_len)
						!= RTW_PHL_STATUS_SUCCESS) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Fail to get cmd info \n",
					__func__);
				break;
			}
			sta = (struct rtw_phl_stainfo_t *)cmd;
			if(sta == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: sta is NULL\n", __FUNCTION__);
				break;
			}
			SET_STATUS_FLAG(sta->rlink->status, RLINK_STATUS_TSF_SYNC);
			ret = MDL_RET_SUCCESS;
			break;
		case MSG_EVT_TDLS_SYNC:
			if(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_MRC){
				ret = MDL_RET_IGNORE;
				break;
			}

			/*
			 * MR decides to call mcc enable or not
			 */
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_TDLS_SYNC\n", __FUNCTION__, idx);

			role = (struct rtw_wifi_role_t *)msg->inbuf;
			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[0].ptr;
			if(rlink == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: rlink is NULL\n", __FUNCTION__);
				break;
			}

			if (phl_mr_info_upt(phl_info, rlink) != RTW_PHL_STATUS_SUCCESS) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "phl_mr_info_upt failed\n");
				break;
			}
			ret = MDL_RET_SUCCESS;
			break;
		case MSG_EVT_TX_RESUME:
			if(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_MRC){
				ret = MDL_RET_IGNORE;
				break;
			}

			/*
			 * MR resume the tx of the role in remain chanctx
			 */
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_TX_RESUME\n", __FUNCTION__, idx);
			role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[1].ptr;
			if(rlink == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role link is NULL\n", __FUNCTION__);
				break;
			}

			phl_mr_offch_hdl(phl_info,
			                 role,
			                 rlink,
			                 false,
			                 phl_com->drv_priv,
			                 core_issue_null_data);

			ret = MDL_RET_SUCCESS;
			break;

		case MSG_EVT_CONNECT_START:
#ifdef CONFIG_STA_CMD_DISPR
			if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT &&
			     MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_ADD_DEL_STA) {
				ret = MDL_RET_IGNORE;
				break;
			}

			role = (struct rtw_wifi_role_t *)msg->inbuf;
			if(role == NULL) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				"%s[%d]: MSG_EVT_CONNECT_START\n",
				__FUNCTION__, idx);
			rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[0].ptr;
			if(rlink == NULL) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role link is NULL\n", __FUNCTION__);
				break;
			}
			hw_band = rlink->hw_band;

			#ifdef CONFIG_DBCC_SUPPORT
			if (phl_mr_is_trigger_dbcc(phl_info)) {
				if (phl_mr_dbcc_proto_hdl(phl_info, msg->band_idx, role, true) !=
							   RTW_PHL_STATUS_SUCCESS) {
					PHL_ERR("%s mr dbcc send_msg failed\n", __func__);
					ret =  MDL_RET_FAIL;
				} else {
					ret =  MDL_RET_PENDING;
				}
			} else
			#endif /*CONFIG_DBCC_SUPPORT*/
			{
				if(_phl_mrc_module_connect_start_hdlr(phl_info, role, rlink) !=
					RTW_PHL_STATUS_SUCCESS) {
					break;
				}

				rtw_hal_notification(phl_info->hal, MSG_EVT_ID_FIELD(msg->msg_id),
						     hw_band);
				ret = MDL_RET_SUCCESS;
			}
#endif
			break;
#ifdef CONFIG_DBCC_SUPPORT
		case MSG_EVT_DBCC_ENABLE:
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				"%s: MSG_EVT_DBCC_ENABLE\n",
				__FUNCTION__);
			ret = _phl_mrc_module_dbcc_enable(dispr, priv, msg);
			if (ret != MDL_RET_SUCCESS) {
				PHL_ERR("MSG_EVT_DBCC_ENABLE failed\n");
				_os_warn_on(1);
			}
			phl_disp_eng_clr_pending_msg(phl_info, msg->band_idx);
			break;
#endif

		case MSG_EVT_CONNECT_END:
#ifdef CONFIG_STA_CMD_DISPR
			if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT &&
			    MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_ADD_DEL_STA) {
				ret = MDL_RET_IGNORE;
				break;
			}

			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_CONNECT_END\n", __FUNCTION__, idx);

			op_info.op_code = FG_REQ_OP_GET_ROLE;
			if(phl_disp_eng_query_cur_cmd_info(phl_info, idx, &op_info)
				!= RTW_PHL_STATUS_SUCCESS){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "Query wifi role fail!\n");
				break;
			}
			role = (struct rtw_wifi_role_t *)op_info.outbuf;
			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			op_info.op_code = FG_REQ_OP_GET_ROLE_LINK;
			if(phl_disp_eng_query_cur_cmd_info(phl_info, idx, &op_info)
				!= RTW_PHL_STATUS_SUCCESS){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
						"Query wifi role link fail!\n");
				break;
			}
			rlink = (struct rtw_wifi_role_link_t *)op_info.outbuf;
			if(rlink == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					"%s: role link is NULL\n", __FUNCTION__);
				break;
			}

			hw_band = rlink->hw_band;

			if(_phl_mrc_module_connect_end_hdlr(phl_info, role) !=
				RTW_PHL_STATUS_SUCCESS) {
				break;
			}

			rtw_hal_notification(phl_info->hal, MSG_EVT_ID_FIELD(msg->msg_id),
					     hw_band);
#endif
			ret = MDL_RET_SUCCESS;
			break;
		case MSG_EVT_DISCONNECT_PREPARE:
#ifdef CONFIG_STA_CMD_DISPR
			if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT &&
			    MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT
			) {
				ret = MDL_RET_IGNORE;
				break;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_DISCONNECT_PREPARE\n", __FUNCTION__, idx);
			role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;

			if (role == NULL) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}
#ifdef CONFIG_TWT
			rtw_phl_twt_disable_all_twt_by_role(phl_info,
							    role);
#endif
#endif
			ret = MDL_RET_SUCCESS;
			break;
		case MSG_EVT_DISCONNECT:
#ifdef CONFIG_STA_CMD_DISPR
			if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT &&
			    MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT &&
			    MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_ADD_DEL_STA) {
				ret = MDL_RET_IGNORE;
				break;
			}

			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_DISCONNECT\n", __FUNCTION__, idx);

			role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;

			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			if(_phl_mrc_module_disconnect_hdlr(phl_info, role) !=
				RTW_PHL_STATUS_SUCCESS) {
				break;
			}
#endif
			ret = MDL_RET_SUCCESS;
			break;

		case MSG_EVT_DISCONNECT_END:
		case MSG_EVT_AP_STOP_END:
#if defined(CONFIG_STA_CMD_DISPR) || defined(CONFIG_AP_CMD_DISPR)
			if ((MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT) &&
			     (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT) &&
			     (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) &&
			     (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START)) {
				ret = MDL_RET_IGNORE;
				break;
			}

			if (MSG_EVT_ID_FIELD(msg->msg_id) == MSG_EVT_DISCONNECT_END)
				PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_DISCONNECT_END\n", __FUNCTION__, idx);
			else
				PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_AP_STOP_END\n", __FUNCTION__, idx);

			#ifdef CONFIG_DBCC_SUPPORT
			if (phl_mr_is_trigger_dbcc(phl_info)) {
				ret = _phl_mrc_module_dbcc_disable(dispr, priv, msg);

				if (ret != MDL_RET_SUCCESS)
					break;

				/* core trx-nss protocol handshake*/
				role = (struct rtw_wifi_role_t *)msg->inbuf;
				if(role == NULL){
					PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
						  "%s: role is NULL\n", __FUNCTION__);
					break;
				}

				if (phl_mr_dbcc_proto_hdl(phl_info, msg->band_idx, role, false) !=
					RTW_PHL_STATUS_SUCCESS) {
					PHL_ERR("%s mr dbcc send_msg failed\n", __func__);
					ret = MDL_RET_FAIL;
					break;
				}
				ret = MDL_RET_PENDING;
			}else
			#endif /*CONFIG_DBCC_SUPPORT*/
			{
				ret = MDL_RET_SUCCESS;
			}
#endif
			break;

#ifdef CONFIG_DBCC_SUPPORT
		case MSG_EVT_DBCC_DISABLE:
			ret = _phl_mrc_module_dbcc_disable(dispr, priv, msg);
			if (ret != MDL_RET_SUCCESS) {
				PHL_ERR("MSG_EVT_DBCC_DISABLE failed\n");
				_os_warn_on(1);
			}
			phl_disp_eng_clr_pending_msg(phl_info, msg->band_idx);
			break;
#endif

		case MSG_EVT_AP_START_PREPARE:
#ifdef CONFIG_AP_CMD_DISPR
			if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START) {
				ret = MDL_RET_IGNORE;
				break;
			}

			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_AP_START_PREPARE\n", __FUNCTION__, idx);

			op_info.op_code = FG_REQ_OP_GET_ROLE;

			if (phl_disp_eng_query_cur_cmd_info(phl_info, idx, &op_info)
				!= RTW_PHL_STATUS_SUCCESS) {
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "Query wifi role fail!\n");
				break;
			}
			role = (struct rtw_wifi_role_t *)op_info.outbuf;
			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			op_info.op_code = FG_REQ_OP_GET_ROLE_LINK;
			if(phl_disp_eng_query_cur_cmd_info(phl_info, idx, &op_info)
				!= RTW_PHL_STATUS_SUCCESS){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
						"Query wifi role link fail!\n");
				break;
			}

			rlink = (struct rtw_wifi_role_link_t *)op_info.outbuf;
			if(rlink == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					"%s: role link is NULL\n", __FUNCTION__);
				break;
			}

			#ifdef CONFIG_DBCC_SUPPORT
			if (phl_mr_is_trigger_dbcc(phl_info)) {
				if (phl_mr_dbcc_proto_hdl(phl_info, msg->band_idx, role, true) !=
							  RTW_PHL_STATUS_SUCCESS) {
					PHL_ERR("%s mr dbcc send_msg failed\n", __func__);
					ret = MDL_RET_FAIL;
				} else {
					ret = MDL_RET_PENDING;
				}
			} else
			#endif /*CONFIG_DBCC_SUPPORT*/
			{
				ret = MDL_RET_SUCCESS;
			}
#endif
			break;
		case MSG_EVT_AP_START:
#ifdef CONFIG_AP_CMD_DISPR
			if(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START){
				ret = MDL_RET_IGNORE;
				break;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_AP_START\n", __FUNCTION__, idx);

			op_info.op_code = FG_REQ_OP_GET_ROLE;
			if(phl_disp_eng_query_cur_cmd_info(phl_info, idx, &op_info)
				!= RTW_PHL_STATUS_SUCCESS){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "Query wifi role fail!\n");
				break;
			}

			role = (struct rtw_wifi_role_t *)op_info.outbuf;
			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			if(_phl_mrc_module_ap_started_hdlr(phl_info, role) !=
				RTW_PHL_STATUS_SUCCESS) {
				break;
			}
#endif
			ret = MDL_RET_SUCCESS;
			break;
		case MSG_EVT_AP_START_END:
#ifdef CONFIG_AP_CMD_DISPR
			if(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START){
				ret = MDL_RET_IGNORE;
				break;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_AP_START_END\n", __FUNCTION__, idx);

			role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			if(msg->inbuf == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s:AP start status info not found!\n",
					  __FUNCTION__);
				ret = MDL_RET_FAIL;
				break;
			}
			if(*(msg->inbuf) != RTW_PHL_STATUS_SUCCESS){
				if(_phl_mrc_module_ap_stop_hdlr(phl_info, role)
				   != RTW_PHL_STATUS_SUCCESS) {
					break;
				}
			}
#endif
			ret = MDL_RET_SUCCESS;
			break;
		case MSG_EVT_AP_STOP:
#ifdef CONFIG_AP_CMD_DISPR
			if(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP){
				ret = MDL_RET_IGNORE;
				break;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_AP_STOP\n", __FUNCTION__, idx);

			role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;

			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			if(_phl_mrc_module_ap_stop_hdlr(phl_info, role) !=
				RTW_PHL_STATUS_SUCCESS) {
				break;
			}
#endif
			ret = MDL_RET_SUCCESS;
			break;
		case MSG_EVT_ECSA_SWITCH_DONE:
#ifdef CONFIG_PHL_ECSA
			if(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_ECSA){
				ret = MDL_RET_IGNORE;
				break;
			}
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_,
				  "%s[%d]: MSG_EVT_ECSA_SWITCH_DONE\n", __FUNCTION__, idx);
			role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;

			if(role == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role is NULL\n", __FUNCTION__);
				break;
			}

			rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[1].ptr;
			if(rlink == NULL){
				PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
					  "%s: role link is NULL\n", __FUNCTION__);
				break;
			}
			hw_band = rlink->hw_band;
			phl_mr_stop_all_beacon(phl_info, role, hw_band, false);
#endif /* CONFIG_PHL_ECSA */
			ret = MDL_RET_SUCCESS;
			break;
		case MSG_EVT_SER_M5_READY:
			if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_SER) {
				ret = MDL_RET_IGNORE;
				break;
			}

			PHL_INFO("%s: MSG_EVT_SER_M5_READY\n", __func__);
			phl_mr_err_recovery(phl_info, MSG_EVT_ID_FIELD(msg->msg_id));
			ret = MDL_RET_SUCCESS;
			break;
		default:
			ret = MDL_RET_SUCCESS;
			break;
	}
	if (ret == MDL_RET_FAIL) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			"%s: MDL(%d), EVT(%d), handle event failed\n",
			__FUNCTION__, MSG_MDL_ID_FIELD(msg->msg_id),
			MSG_EVT_ID_FIELD(msg->msg_id));
	}

	return ret;
}

static enum phl_mdl_ret_code
_phl_mrc_module_msg_hdlr(void *dispr, void *priv, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	FUNCIN();
	if (IS_MSG_FAIL(msg->msg_id)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
			  "%s: cmd dispatcher notify cmd failure: 0x%x.\n",
			   __FUNCTION__, msg->msg_id);
		FUNCOUT();
		return MDL_RET_FAIL;
	}
	if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		ret = _mrc_module_msg_pre_hdlr(dispr, priv, msg);
	} else {
		ret = _mrc_module_msg_post_hdl(dispr, priv, msg);
	}
	FUNCOUT();
	return ret;
}

static enum phl_mdl_ret_code
_phl_mrc_module_set_info(void *dispr,
			 void *priv,
			 struct phl_module_op_info *info)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	FUNCIN();
	FUNCOUT();

	ret = MDL_RET_SUCCESS;
	return ret;
}

static enum phl_mdl_ret_code
_phl_mrc_module_query_info(void *dispr,
			   void *priv,
			   struct phl_module_op_info *info)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	FUNCIN();
	FUNCOUT();

	ret = MDL_RET_SUCCESS;
	return ret;
}

static enum rtw_phl_status
_phl_role_bk_module_init(struct phl_info_t *phl_info)
{
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	struct phl_bk_module_ops *bk_ops = &mr_ctl->bk_ops;

	bk_ops->init = _phl_mrc_module_init;
	bk_ops->deinit = _phl_mrc_module_deinit;
	bk_ops->start = _phl_mrc_module_start;
	bk_ops->stop = _phl_mrc_module_stop;
	bk_ops->msg_hdlr = _phl_mrc_module_msg_hdlr;
	bk_ops->set_info = _phl_mrc_module_set_info;
	bk_ops->query_info = _phl_mrc_module_query_info;

	return RTW_PHL_STATUS_SUCCESS;
}
#endif /*CONFIG_CMD_DISP*/
/*
 * init wifi_role control components
 * init band_ctrl
 * init bk module
 * init wifi_role[]
 */
enum rtw_phl_status
phl_mr_ctrl_init(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	void *drv = phl_to_drvpriv(phl_info);
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	u8 ridx = MAX_WIFI_ROLE_NUMBER;
	struct rtw_wifi_role_link_t *rlink;
	u8 lidx;
	struct rtw_wifi_role_t *role = NULL;
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	mr_ctl->hal_com = rtw_hal_get_halcom(phl_info->hal);
	if (mr_ctl->hal_com == NULL) {
		PHL_ERR("%s mr_ctl->hal_com is NULL\n", __func__);
		_os_warn_on(1);
		return RTW_PHL_STATUS_FAILURE;
	}
	_os_spinlock_init(drv, &(mr_ctl->lock));
	mr_ctl->is_db = false;
	_phl_band_ctrl_init(phl_info);
	#ifdef CONFIG_CMD_DISP
	_phl_role_bk_module_init(phl_info);
	#endif
	_os_mem_set(phl_to_drvpriv(phl_info), phl_com->wifi_roles,
		0, sizeof(*phl_com->wifi_roles));

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		role = &(phl_com->wifi_roles[ridx]);
		pq_init(drv, &role->assoc_mld_queue);
		role->phl_com = phl_com;
		role->id = ridx;
		role->active = false;

		for (lidx = 0; lidx < RTW_RLINK_MAX; lidx++) {
			rlink = get_rlink(role, lidx);
			pq_init(drv, &rlink->assoc_sta_queue);
			rlink->wrole = role;
			rlink->id = lidx;
			rlink->chanctx = NULL;
		}
	}
	if (RTW_PHL_STATUS_SUCCESS != (status = rtw_phl_mcc_init(phl_info))) {
		PHL_ERR("%s mcc init fail\n", __func__);
		/* todo, need to discuss with Georgia*/
	}
	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_band_ctrl_deinit(struct phl_info_t *phl_info)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	struct hw_band_ctl_t *band_ctrl;
	u8 band_idx = 0;

	rtw_phl_mcc_deinit(phl_info);
	for (band_idx = 0; band_idx < MAX_BAND_NUM; band_idx++) {
		band_ctrl = &(mr_ctl->band_ctrl[band_idx]);
		phl_chanctx_free(phl_info, band_ctrl);
		_os_spinlock_free(drv, &(band_ctrl->lock));
		pq_deinit(drv , &band_ctrl->chan_ctx_queue);
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_mr_ctrl_deinit(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	u8 ridx = MAX_WIFI_ROLE_NUMBER;
	struct rtw_wifi_role_t *role;
	void *drv = phl_to_drvpriv(phl_info);
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct rtw_wifi_role_link_t *rlink;
	u8 lidx;

	_os_spinlock_free(drv, &(mr_ctl->lock));
	_phl_band_ctrl_deinit(phl_info);

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		role = &(phl_com->wifi_roles[ridx]);

		pq_deinit(drv, &role->assoc_mld_queue);
		for (lidx = 0; lidx < RTW_RLINK_MAX; lidx++) {
			rlink = get_rlink(role, lidx);
			pq_deinit(drv, &rlink->assoc_sta_queue);
		}
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_mr_chandef_sync(struct phl_info_t *phl_info, struct hw_band_ctl_t *band_ctrl,
			struct rtw_chan_ctx *chanctx, struct rtw_chan_def *chandef)
{
	void *drv = phl_to_drvpriv(phl_info);
	u8 ridx;
	u8 role_num = 0;
	enum band_type band_ret = BAND_MAX;
	u8 ch_ret = 0;
	enum channel_width bw_ret = CHANNEL_WIDTH_20;
	enum chan_offset offset_ret = CHAN_OFFSET_NO_EXT;
	struct rtw_wifi_role_t *wrole;
	enum rtw_phl_status phl_sts = RTW_PHL_STATUS_FAILURE;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (!chanctx) {
		PHL_ERR("%s failed, chanctx == NULL\n", __func__);
		goto _exit;
	}
	if (!chandef) {
		PHL_ERR("%s failed, chandef == NULL\n", __func__);
		goto _exit;
	}

	_os_spinlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (chanctx->role_map & BIT(ridx)) {
			wrole = phl_get_wrole_by_ridx(phl_info, ridx);
			if (wrole == NULL) {
				PHL_ERR("ridx :%d wrole == NULL\n", ridx);
				_os_warn_on(1);
				continue;
			}

			for (idx = 0; idx < wrole->rlink_num; idx++) {
				rlink = get_rlink(wrole, idx);
				if (rlink->hw_band == band_ctrl->id)
					break;
			}
			if (role_num == 0) {
				band_ret = rlink->chandef.band;
				ch_ret = rlink->chandef.chan;
				bw_ret = rlink->chandef.bw;
				offset_ret = rlink->chandef.offset;
				role_num++;
				continue;
			}
			if (band_ret != rlink->chandef.band) {
				PHL_ERR("band_ret(%d) !=  ridx(%d)-band_ret(%d)\n",
					band_ret, ridx, rlink->chandef.band);
				_os_warn_on(1);
				role_num = 0;
				break;
			}
			if (ch_ret !=  rlink->chandef.chan) {
				PHL_ERR("ch_ret(%d) !=  ridx(%d)-chan(%d)\n",
					ch_ret, ridx, rlink->chandef.chan);
				_os_warn_on(1);
				role_num = 0;
				break;
			}

			if (bw_ret < rlink->chandef.bw) {
				bw_ret = rlink->chandef.bw;
				offset_ret = rlink->chandef.offset;
			} else if (bw_ret == rlink->chandef.bw && offset_ret != rlink->chandef.offset) {
				role_num = 0;
				break;
			}

			role_num++;
		}
	}
	_os_spinunlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);

	if (role_num == 0) {
		PHL_ERR("%s role_num=0\n", __func__);
		_os_warn_on(!role_num);
		goto _exit;
	}

	PHL_INFO("%s org_chctx - band:%d, chan:%d, bw:%d, offset:%d\n",
		__func__, chandef->band, chandef->chan, chandef->bw, chandef->offset);
	PHL_INFO("%s mi_upt - band:%d, chan:%d, bw:%d, offset:%d\n",
		__func__, band_ret, ch_ret, bw_ret, offset_ret);

	chandef->band = band_ret;
	chandef->chan = ch_ret;
	chandef->bw = bw_ret;
	chandef->offset = offset_ret;
	phl_sts = RTW_PHL_STATUS_SUCCESS;

_exit:
	return phl_sts;
}

/*
 * MR change chctx from wrole->chdef to new chdef
 * @wrole: specific role, and we can get original chdef.
 * @new_chan: new chdef
 * @chctx_result: The final ch ctx after change new chdef to MR.
 * ex: In the scc case, it will be the group chdef.
 */
enum rtw_phl_status
phl_mr_chandef_chg(struct phl_info_t *phl,
                   struct rtw_wifi_role_t *wrole,
                   struct rtw_wifi_role_link_t *rlink,
                   struct rtw_chan_def *new_chan,
                   struct rtw_chan_def *chctx_result)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct rtw_chan_def chan_def = {0};
	struct rtw_mr_chctx_info mr_cc_info = {0};
	void *drv = phl_to_drvpriv(phl);
	u8 chanctx_num = 0;

	chanctx_num = (u8)rtw_phl_chanctx_del(phl, wrole, rlink, &chan_def);

	_os_mem_cpy(drv, &chan_def, new_chan, sizeof(struct rtw_chan_def));

	if (rtw_phl_chanctx_add((void *)phl, wrole, rlink, &chan_def, &mr_cc_info)) {
		_os_mem_cpy(drv, chctx_result, &chan_def,
				sizeof(struct rtw_chan_def));
		psts = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	}
	PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Add new chandef fail, something wrong!\n",
		__FUNCTION__);
	/* Error handle: Recover the chctx */
	_os_mem_cpy(drv, &chan_def, &rlink->chandef,
			sizeof(struct rtw_chan_def));

	if (!rtw_phl_chanctx_add((void *)phl, wrole, rlink, &chan_def, &mr_cc_info)) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Error handle failed for recovery!\n",
			__FUNCTION__);
		goto exit;
	}

exit:
	return psts;
}

enum rtw_phl_status
phl_mr_chandef_upt(struct phl_info_t *phl_info,
		struct hw_band_ctl_t *band_ctrl, struct rtw_chan_ctx *chanctx)
{
	enum rtw_phl_status phl_sts = RTW_PHL_STATUS_FAILURE;

	if (!chanctx) {
		PHL_ERR("%s chanctx == NULL\n", __func__);
		goto _exit;
	}

	phl_sts = phl_mr_chandef_sync(phl_info, band_ctrl, chanctx, &chanctx->chan_def);
	if (phl_sts != RTW_PHL_STATUS_SUCCESS)
		PHL_ERR("%s phl_mr_sync_chandef failed\n", __func__);
_exit:
	return phl_sts;
}

enum rtw_phl_status
rtw_phl_mr_upt_chandef(void *phl,
                       struct rtw_wifi_role_link_t *rlink)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[rlink->hw_band]);
	enum rtw_phl_status phl_sts = RTW_PHL_STATUS_FAILURE;

	if (!rlink->chanctx) {
		PHL_ERR("%s failed - rlink->chanctx == NULL\n", __func__);
		goto _exit;
	}
	phl_sts = phl_mr_chandef_upt(phl_info, band_ctrl, rlink->chanctx);
	if (phl_sts != RTW_PHL_STATUS_SUCCESS)
		PHL_ERR("%s phl_mr_chandef_upt failed\n", __func__);

_exit:
	return phl_sts;
}

int phl_mr_get_chandef_by_band(struct phl_info_t *phl_info,
					enum phl_band_idx band_idx,
					struct rtw_chan_def *chandef)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);
	struct phl_queue *chan_ctx_queue = &band_ctrl->chan_ctx_queue;
	struct rtw_chan_ctx *chanctx = NULL;
	int chctx_num = 0;
	u8 chctx_role_num = 0;

	if (!chandef) {
		PHL_ERR("%s failed - chandef == NULL\n", __func__);
		return chctx_num;
	}

	/*init chandef*/
	chandef->chan = 0;
	chctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);

	if (chctx_num == 0) {
		PHL_DBG("%s band:%d chctx_num(%d)\n", __func__, band_idx, chctx_num);
	}
	else if (chctx_num == 1) {/*SWR or SCC*/
		_os_spinlock(drv, &chan_ctx_queue->lock, _bh, NULL);
		if (list_empty(&chan_ctx_queue->queue)) {
			PHL_ERR("%s chan_ctx_queue->queue is empty\n", __func__);
			_os_warn_on(1);
		}

		chanctx = list_first_entry(&chan_ctx_queue->queue,
						struct rtw_chan_ctx, list);
		chctx_role_num = phl_chanctx_get_rnum(phl_info, chan_ctx_queue, chanctx);
		if (chctx_role_num == 0) {
			PHL_ERR("%s-%d chctx_role_num == 0\n", __FUNCTION__, __LINE__);
			chctx_num = 0;
			_os_warn_on(1);
		}
		/*chctx_role_num == 1*/
		_os_mem_cpy(drv, chandef, &chanctx->chan_def,
					sizeof(struct rtw_chan_def));
		_os_spinunlock(drv, &chan_ctx_queue->lock, _bh, NULL);
	}
	else if (chctx_num == 2) {/*MCC*/
		PHL_ERR("%s band:%d chctx_num(%d) is not support yet\n",
			__func__, band_idx, chctx_num);
	}
	else {
		PHL_ERR("%s band:%d chctx_num(%d) is invalid\n",
			__func__, band_idx, chctx_num);
		_os_warn_on(1);
	}
	return chctx_num;
}

int rtw_phl_mr_get_chandef_by_hwband(void *phl,
					enum phl_band_idx band_idx,
					struct rtw_chan_def *chandef)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return phl_mr_get_chandef_by_band(phl_info, band_idx, chandef);
}

enum rtw_phl_status
phl_mr_get_chandef(struct phl_info_t *phl_info,
                   struct rtw_wifi_role_t *wifi_role,
                   struct rtw_wifi_role_link_t *rlink,
                   bool sync,
                   struct rtw_chan_def *chandef)
{
	enum rtw_phl_status phl_sts = RTW_PHL_STATUS_SUCCESS;
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	u8 hw_band = rlink->hw_band;
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[hw_band]);
	struct phl_queue *chan_ctx_queue = &band_ctrl->chan_ctx_queue;
	struct rtw_chan_ctx *chanctx = NULL;
	int chctx_num = 0;
	u8 chctx_role_num = 0;

	if (!chandef) {
		PHL_ERR("%s failed - chandef == NULL\n", __func__);
		phl_sts = RTW_PHL_STATUS_FAILURE;
		goto _exit;
	}

	/*init chandef*/
	chandef->chan = 0;

	if (rlink->chanctx) {
		chctx_role_num = phl_chanctx_get_rnum_with_lock(phl_info, chan_ctx_queue, rlink->chanctx);

		if (chctx_role_num == 0) {
			PHL_ERR("%s-%d chctx_role_num == 0\n", __FUNCTION__, __LINE__);
			_os_warn_on(1);
		}
		if (sync && chctx_role_num >= 2) {
			phl_sts = phl_mr_chandef_sync(phl_info, band_ctrl,
			                              rlink->chanctx, chandef);
			if (phl_sts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s phl_mr_chandef_sync failed\n", __func__);
				_os_warn_on(1);
			}
		} else { /*chctx_role_num == 1*/
			_os_mem_cpy(drv, chandef, &rlink->chanctx->chan_def,
			            sizeof(struct rtw_chan_def));
		}
	} else {
		chctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);
		if (chctx_num == 0) {
			_os_mem_cpy(drv, chandef, &rlink->chandef,
			            sizeof(struct rtw_chan_def));
		} else if (chctx_num == 1) {
			_os_spinlock(drv, &chan_ctx_queue->lock, _bh, NULL);
			if (list_empty(&chan_ctx_queue->queue)) {
				PHL_ERR("%s chan_ctx_queue->queue is empty\n", __func__);
				_os_warn_on(1);
			}

			chanctx = list_first_entry(&chan_ctx_queue->queue,
						struct rtw_chan_ctx, list);
			chctx_role_num = phl_chanctx_get_rnum(phl_info, chan_ctx_queue, chanctx);
			if (chctx_role_num == 0) {
				PHL_ERR("%s-%d chctx_role_num == 0\n", __FUNCTION__, __LINE__);
				_os_warn_on(1);
			}
			if (sync && chctx_role_num >= 2) {
				phl_sts = phl_mr_chandef_sync(phl_info, band_ctrl,
								chanctx, chandef);
				if (phl_sts != RTW_PHL_STATUS_SUCCESS)
					PHL_ERR("%s phl_mr_chandef_sync failed\n", __func__);
			} else { /*chctx_role_num == 1*/
				_os_mem_cpy(drv, chandef, &chanctx->chan_def,
						sizeof(struct rtw_chan_def));
			}
			_os_spinunlock(drv, &chan_ctx_queue->lock, _bh, NULL);

		} else if (chctx_num == 2) { /*MCC*/

		} else {
			PHL_ERR("%s chctx_num(%d) is invalid\n", __func__, chctx_num);
			_os_warn_on(1);
			goto _exit;
		}
	}

_exit:
	return phl_sts;
}

struct rtw_wifi_role_t *
phl_mr_get_role_by_bandidx(struct phl_info_t *phl_info, u8 band_idx)
{
	struct hw_band_ctl_t *band_ctrl = get_band_ctrl(phl_info, band_idx);
	struct rtw_wifi_role_t *wrole = NULL;
	u8 i;
	u8 rnum = 0;

	rnum = phl_mr_get_role_num(phl_info, band_ctrl);
	if (rnum == 0) {
		PHL_ERR("%s B(%d) role number == 0\n", __func__, band_ctrl->id);
		goto _exit;
	}

	for (i = 0; i < MAX_WIFI_ROLE_NUMBER; i++) {
		if (band_ctrl->role_map & BIT(i)) {
			wrole = phl_get_wrole_by_ridx(phl_info, i);
			if (wrole->active == true)
				break;
			else
				wrole = NULL;
		}
	}

_exit :
	if (wrole == NULL) {
		PHL_ERR("%s B(%d) cannot get wrole\n", __func__, band_ctrl->id);
		/*_os_warn_on(1);*/
	}
	return wrole;
}

enum rtw_phl_status
rtw_phl_mr_get_chandef(void *phl,
                       struct rtw_wifi_role_t *wifi_role,
                       struct rtw_wifi_role_link_t *rlink,
                       struct rtw_chan_def *chandef)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return phl_mr_get_chandef(phl_info, wifi_role, rlink, false, chandef);
}
int rtw_phl_mr_get_chanctx_num(void *phl,
                               struct rtw_wifi_role_t *wifi_role,
                               struct rtw_wifi_role_link_t *rlink)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	u8 band_idx = rlink->hw_band;
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);

	return phl_mr_get_chanctx_num(phl_info, band_ctrl);
}

enum rtw_phl_status
rtw_phl_mr_rx_filter_opt(void *phl, struct rtw_wifi_role_link_t *rlink)
{
	/* Not switch RX filter according to role for AP applications. */
#if !defined(PHL_FEATURE_AP)
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_rx_fltr_opt_mode mode = RX_FLTR_OPT_MODE_STA_NORMAL;
#ifdef CONFIG_MR_SUPPORT
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[rlink->hw_band]);

	if (band_ctrl->cur_info.lg_sta_num >= 1)
		mode = RX_FLTR_OPT_MODE_STA_LINKING;
	else if (band_ctrl->cur_info.ap_num >= 1)
		mode = RX_FLTR_OPT_MODE_AP_NORMAL;
	else if (band_ctrl->cur_info.ld_sta_num >= 1)
		mode = RX_FLTR_OPT_MODE_STA_NORMAL;	/* Accpet BSSID matched frames */
	else
		mode = RX_FLTR_OPT_MODE_STA_NORMAL; /* For STA no link case */

	rtw_hal_set_rxfltr_opt_by_mode(phl_info->hal, rlink->hw_band, mode);
#else
	struct rtw_wifi_role_t *wrole = rlink->wrole;
	if (rtw_phl_role_is_client_category(wrole) && wrole->mstate == MLME_LINKED)
		mode = RX_FLTR_OPT_MODE_STA_NORMAL;
	else if (rtw_phl_role_is_client_category(wrole) && wrole->mstate == MLME_LINKING)
		mode = RX_FLTR_OPT_MODE_STA_LINKING;
	else if (rtw_phl_role_is_ap_category(wrole))
		mode = RX_FLTR_OPT_MODE_AP_NORMAL;
	else
		mode = RX_FLTR_OPT_MODE_STA_NORMAL;/* For STA no link case */

#endif /*CONFIG_MR_SUPPORT*/

	rtw_hal_set_rxfltr_opt_by_mode(phl_info->hal, rlink->hw_band, mode);
#endif /* !PHL_AP_FEATURE */
	return RTW_PHL_STATUS_SUCCESS;
}

/**
 * The decision rule of updating *trgt_cur by comparing with trgt_chk:
 * (chk = trgt_chk, cur = *trgt_cur, fnl = final decision of *trgt_cur)
 * (D = DROP, H = TO_HOST, W = TO_WLCPU)
 *
 *
 *	    |		cur
 *    fnl   |---------------------------
 *	    |	D	H	W
 * -------------------------------------
 *	| D |	D	H	W
 *	|   |
 *  chk	| H |	H	H	W
 *	|   |
 *	| W |	W	W	W
 */
static void _phl_mr_rxfltr_target_dcsn(enum rtw_rxfltr_target trgt_chk,
				       enum rtw_rxfltr_target *trgt_cur)
{
	switch (trgt_chk) {
	case RXFLTR_TARGET_DROP:
		break;
	case RXFLTR_TARGET_TO_HOST:
		if (*trgt_cur == RXFLTR_TARGET_DROP)
			*trgt_cur = RXFLTR_TARGET_TO_HOST;

		break;
	case RXFLTR_TARGET_TO_WLCPU:
		*trgt_cur = RXFLTR_TARGET_TO_WLCPU;
		break;
	case RXFLTR_TARGET_MAX:
	default:
		break;
	}
}

static void _phl_mr_rxfltr_target_dcsn_ctrl(struct rxfltr_cap_ctrl *cap_chk,
					    struct rxfltr_cap_to_set_ctrl *cap_cur)
{
	u8 i = 0;

	for (i = 0; i < RXFLTR_STYPE_CTRL_MAX; i++) {
		if (cap_cur->stype[i].set == false)
			continue;

		_phl_mr_rxfltr_target_dcsn(cap_chk->stype[i].target,
					   &(cap_cur->stype[i].target));
	}
}

static void _phl_mr_rxfltr_target_dcsn_mgnt(struct rxfltr_cap_mgnt *cap_chk,
					    struct rxfltr_cap_to_set_mgnt *cap_cur)
{
	u8 i = 0;

	for (i = 0; i < RXFLTR_STYPE_MGNT_MAX; i++) {
		if (cap_cur->stype[i].set == false)
			continue;

		_phl_mr_rxfltr_target_dcsn(cap_chk->stype[i].target,
					   &(cap_cur->stype[i].target));
	}
}

static void _phl_mr_rxfltr_target_dcsn_data(struct rxfltr_cap_data *cap_chk,
					    struct rxfltr_cap_to_set_data *cap_cur)
{
	u8 i = 0;

	for (i = 0; i < RXFLTR_STYPE_DATA_MAX; i++) {
		if (cap_cur->stype[i].set == false)
			continue;

		_phl_mr_rxfltr_target_dcsn(cap_chk->stype[i].target,
					   &(cap_cur->stype[i].target));
	}
}

static void _phl_mr_rx_filter_type(struct phl_info_t *phl_info,
				   struct rtw_wifi_role_link_t *rlink,
				   struct rxfltr_type_cap_to_set *cap_s)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[rlink->hw_band]);
	u8 role_map = band_ctrl->role_map;
	struct rtw_wifi_role_t *wr = NULL;
	struct rxfltr_type_cap *cap_rlink = NULL;
	u8 ridx = 0, lidx = 0;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		/* only consider allocated role on the same band */
		if (!(role_map & BIT(ridx)))
			continue;

		wr = &(phl_com->wifi_roles[ridx]);

		/* exclude inactive role */
		if (wr->active == false)
			continue;

		for (lidx = 0; lidx < wr->rlink_num; lidx++) {
			/* only consider rlink on the same hw_band */
			if (wr->rlink[lidx].hw_band != rlink->hw_band)
				continue;

			/* exclude self rlink */
			if ((wr->id == rlink->wrole->id) && (wr->rlink[lidx].id == rlink->id))
				continue;

			cap_rlink = &(wr->rlink[lidx].rxfltr_type_cap);

			_phl_mr_rxfltr_target_dcsn_ctrl(&(cap_rlink->rxfltr_ctrl),
							&(cap_s->rxfltr_ctrl));
			_phl_mr_rxfltr_target_dcsn_mgnt(&(cap_rlink->rxfltr_mgnt),
							&(cap_s->rxfltr_mgnt));
			_phl_mr_rxfltr_target_dcsn_data(&(cap_rlink->rxfltr_data),
							&(cap_s->rxfltr_data));
		}
	}
}

#define PHL_MR_SET_RXFLTR_STYPE_CTRL(s, t, cr, cs)	\
	do { \
		(cr)->rxfltr_ctrl.stype[s].target = t; \
		(cs)->rxfltr_ctrl.stype[s].set = true; \
		(cs)->rxfltr_ctrl.stype[s].target = t; \
	} while (0)
#define PHL_MR_SET_RXFLTR_STYPE_MGNT(s, t, cr, cs)	\
	do { \
		(cr)->rxfltr_mgnt.stype[s].target = t; \
		(cs)->rxfltr_mgnt.stype[s].set = true; \
		(cs)->rxfltr_mgnt.stype[s].target = t; \
	} while (0)
#define PHL_MR_SET_RXFLTR_STYPE_DATA(s, t, cr, cs)	\
	do { \
		(cr)->rxfltr_data.stype[s].target = t; \
		(cs)->rxfltr_data.stype[s].set = true; \
		(cs)->rxfltr_data.stype[s].target = t; \
	} while (0)

static void _phl_mr_set_rxfltr_cap_by_type(u8 type, u8 target, struct rxfltr_type_cap *cap_rxfltr,
					   struct rxfltr_type_cap_to_set *cap_rxfltr_s)
{
	u8 i = 0;

	switch (type) {
	case RTW_PHL_PKT_TYPE_CTRL:
		for (i = 0; i < RXFLTR_STYPE_CTRL_MAX; i++)
			PHL_MR_SET_RXFLTR_STYPE_CTRL(i, target, cap_rxfltr, cap_rxfltr_s);

		break;
	case RTW_PHL_PKT_TYPE_MGNT:
		for (i = 0; i < RXFLTR_STYPE_MGNT_MAX; i++)
			PHL_MR_SET_RXFLTR_STYPE_MGNT(i, target, cap_rxfltr, cap_rxfltr_s);

		break;
	case RTW_PHL_PKT_TYPE_DATA:
		for (i = 0; i < RXFLTR_STYPE_DATA_MAX; i++)
			PHL_MR_SET_RXFLTR_STYPE_DATA(i, target, cap_rxfltr, cap_rxfltr_s);

		break;
	}
}

#define PHL_MR_SET_RXFLTR_TYPE_CTRL(t, cr, cs)	\
	_phl_mr_set_rxfltr_cap_by_type(RTW_PHL_PKT_TYPE_CTRL, t, cr, cs)
#define PHL_MR_SET_RXFLTR_TYPE_MGNT(t, cr ,cs)	\
	_phl_mr_set_rxfltr_cap_by_type(RTW_PHL_PKT_TYPE_MGNT, t, cr, cs)
#define PHL_MR_SET_RXFLTR_TYPE_DATA(t, cr, cs)	\
	_phl_mr_set_rxfltr_cap_by_type(RTW_PHL_PKT_TYPE_DATA, t, cr, cs)

enum rtw_phl_status rtw_phl_mr_set_rxfltr_type_by_mode(void *phl, struct rtw_wifi_role_link_t *rlink,
						       enum rtw_rxfltr_type_mode mode)
{
	struct phl_info_t *phl_info = (struct phl_info_t*)phl;
	struct rxfltr_type_cap *cap_rlink = NULL;
	struct rxfltr_type_cap_to_set cap_rlink_set = {0};
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;

	cap_rlink = &(rlink->rxfltr_type_cap);

	/* set rxfltr type cap of this role link by mode */
	switch (mode) {
	case RX_FLTR_TYPE_MODE_ROLE_INIT:
		PHL_MR_SET_RXFLTR_TYPE_CTRL(RXFLTR_TARGET_DROP, cap_rlink, &cap_rlink_set);
		PHL_MR_SET_RXFLTR_TYPE_MGNT(RXFLTR_TARGET_TO_HOST, cap_rlink, &cap_rlink_set);
		PHL_MR_SET_RXFLTR_TYPE_DATA(RXFLTR_TARGET_TO_HOST, cap_rlink, &cap_rlink_set);
		break;
	case RX_FLTR_TYPE_MODE_STA_CONN:
		PHL_MR_SET_RXFLTR_STYPE_CTRL(RXFLTR_STYPE_VHT_HE_NDPA, RXFLTR_TARGET_TO_HOST,
					     cap_rlink, &cap_rlink_set);
		PHL_MR_SET_RXFLTR_STYPE_CTRL(RXFLTR_STYPE_BAR, RXFLTR_TARGET_TO_HOST,
					     cap_rlink, &cap_rlink_set);
#ifdef RTW_WKARD_RX_FLTR_HE_TF
		if (rlink->cap.wmode & WLAN_MD_11AX) {
			PHL_MR_SET_RXFLTR_STYPE_CTRL(RXFLTR_STYPE_TRIGGER, RXFLTR_TARGET_TO_HOST,
						     cap_rlink, &cap_rlink_set);
		}
#endif
		break;
	case RX_FLTR_TYPE_MODE_STA_DIS_CONN:
		PHL_MR_SET_RXFLTR_TYPE_CTRL(RXFLTR_TARGET_DROP, cap_rlink, &cap_rlink_set);
		break;
	case RX_FLTR_TYPE_MODE_AP_CONN:
		PHL_MR_SET_RXFLTR_STYPE_CTRL(RXFLTR_STYPE_VHT_HE_NDPA, RXFLTR_TARGET_TO_HOST,
					     cap_rlink, &cap_rlink_set);
		PHL_MR_SET_RXFLTR_STYPE_CTRL(RXFLTR_STYPE_BAR, RXFLTR_TARGET_TO_HOST,
					     cap_rlink, &cap_rlink_set);
		PHL_MR_SET_RXFLTR_STYPE_CTRL(RXFLTR_STYPE_PS_POLL, RXFLTR_TARGET_TO_HOST,
					     cap_rlink, &cap_rlink_set);
		break;
	case RX_FLTR_TYPE_MODE_AP_DIS_CONN:
		if (rlink->assoc_sta_queue.cnt < 2)
			PHL_MR_SET_RXFLTR_TYPE_CTRL(RXFLTR_TARGET_DROP, cap_rlink, &cap_rlink_set);
		break;

#ifdef CONFIG_PHL_CHANNEL_INFO
	case RX_FLTR_TYPE_MODE_CHAN_INFO_EN:
		PHL_MR_SET_RXFLTR_STYPE_CTRL(RXFLTR_STYPE_ACK, RXFLTR_TARGET_TO_HOST,
					     cap_rlink, &cap_rlink_set);
	break;
	case RX_FLTR_TYPE_MODE_CHAN_INFO_DIS:
		PHL_MR_SET_RXFLTR_TYPE_CTRL(RXFLTR_TARGET_DROP, cap_rlink, &cap_rlink_set);
	break;
#endif
	default:
		PHL_TRACE(COMP_PHL_RECV, _PHL_ERR_, "%s: does not handle rxfltr mode(%u)\n", __func__, mode);
		return RTW_PHL_STATUS_FAILURE;
	}

	/* jointly decide rxfltr type cap among all active rlinks */
	_phl_mr_rx_filter_type(phl_info, rlink, &cap_rlink_set);

	/* call HAL API to set rxfltr type cap */
	hsts = rtw_hal_set_rxfltr_by_stype_all(phl_info->hal, rlink->hw_band, &cap_rlink_set);

	return (hsts == RTW_HAL_STATUS_SUCCESS) ? RTW_PHL_STATUS_SUCCESS : RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status phl_mr_tsf_sync(void *phl,
                                    struct rtw_wifi_role_t *wrole,
                                    struct rtw_wifi_role_link_t *rlink,
                                    enum role_state state)
{
	enum rtw_phl_status ret = RTW_PHL_STATUS_SUCCESS;
#ifdef CONFIG_MR_SUPPORT
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	enum phl_band_idx band = rlink->hw_band;
	struct hw_band_ctl_t *band_ctl = &mr_ctl->band_ctrl[band];
	struct rtw_chan_ctx *chanctx = rlink->chanctx;
	struct rtw_phl_com_t *phl_com = wrole->phl_com;
	struct rtw_wifi_role_t *check_wr = NULL;
	struct rtw_wifi_role_t *ld_sta_wr = NULL;
	struct rtw_wifi_role_link_t *wrl_sync_from = NULL;
	struct rtw_wifi_role_link_t *wrl_sync_to = NULL;
	s8 tsf_sync_offset_tu = PHL_MR_TSF_SYNC_BASE_OFFSET; /* unit is TU(1024us) */
	u8 tsf_offset_range_rand = 0;
	u8 ridx = 0, lidx = 0;
	u8 ap_num = band_ctl->cur_info.ap_num;
	u8 ld_sta_num = band_ctl->cur_info.ld_sta_num;
	int chanctx_num = 0;

	tsf_offset_range_rand = _os_random32(drv) % (2 * PHL_MR_TSF_SYNC_OFFSET_RANGE + 1);
	if (tsf_offset_range_rand != 0) {
		if (tsf_offset_range_rand > PHL_MR_TSF_SYNC_OFFSET_RANGE)
			tsf_sync_offset_tu -= (tsf_offset_range_rand - PHL_MR_TSF_SYNC_OFFSET_RANGE);
		else
			tsf_sync_offset_tu += tsf_offset_range_rand;
	}

	chanctx_num = phl_mr_get_chanctx_num(phl_info, band_ctl);
	PHL_INFO("%s:state(%d), ap_num=%d, ld_sta_num=%d, tsf_sync_port=%d,chanctx_num=%d\n",
			__func__, state, ap_num, ld_sta_num, band_ctl->tsf_sync_port, chanctx_num);

	#ifdef CONFIG_MCC_SUPPORT
	if (chanctx_num > 1) {
		if (phl_com->dev_cap.mcc_sup == true) {
			PHL_INFO("%s: will process MCC, skip tsf sync\n", __func__);
			ret = RTW_PHL_STATUS_SUCCESS;
			goto exit;
		} else {
			PHL_ERR("%s: chanctx_num(%d) > 1, check chanctx\n", __func__, chanctx_num);
			ret = RTW_PHL_STATUS_FAILURE;
			goto exit;
		}
	}
	#endif

	switch (state) {
	case PHL_ROLE_MSTS_STA_CONN_END:
		if (chanctx == NULL) {
			PHL_WARN("%s: state%d wifi role (id=%d) role link (id=%d) chanctx=NULL\n", __func__, state, wrole->id, rlink->id);
			ret = RTW_PHL_STATUS_FAILURE;
			goto exit;
		}
		/* SoftAP already started, and no tsf sync before */
		if (ap_num >= 1 && band_ctl->tsf_sync_port == HW_PORT_MAX) {
			/* tsf sync for all softap */
			wrl_sync_from = rlink;
			for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
				if (chanctx->role_map & BIT(ridx)) {
					check_wr = &phl_com->wifi_roles[ridx];
					/* Get role link from another role within the same chanctx */
					for (lidx = 0; lidx < check_wr->rlink_num; lidx++) {
						wrl_sync_to = get_rlink(check_wr, lidx);
						if (wrl_sync_to->hw_band == band)
							break;
					}
					if (lidx == check_wr->rlink_num) {
						PHL_ERR("%s Can not get target link in the same hw band in another role!\n", __func__);
						ret = RTW_PHL_STATUS_FAILURE;
						goto exit;
					}

					if (wrole != check_wr) {
						switch (check_wr->type) {
						case PHL_RTYPE_AP:
						case PHL_RTYPE_VAP:
						case PHL_RTYPE_P2P_GO:
						case PHL_RTYPE_MESH:
							if (rtw_hal_tsf_sync(phl_info->hal, wrl_sync_from->hw_port,
								wrl_sync_to->hw_port, band, tsf_sync_offset_tu, HAL_TSF_EN_SYNC_AUTO) == RTW_HAL_STATUS_SUCCESS) {
								ret = RTW_PHL_STATUS_SUCCESS;
								PHL_INFO("%s, enable wrole-rlink:%d-%d(port:%d) sync from wrole-rlink:%d-%d(port:%d) success\n",
									__func__, check_wr->id, wrl_sync_to->id, wrl_sync_to->hw_port,
									wrole->id, wrl_sync_from->id, wrl_sync_from->hw_port);
							} else {
								ret = RTW_PHL_STATUS_FAILURE;
								PHL_ERR("%s, enable wrole-rlink:%d-%d(port:%d) sync from wrole-rlink:%d-%d(port:%d) fail\n",
									__func__, check_wr->id, wrl_sync_to->id, wrl_sync_to->hw_port,
									wrole->id, wrl_sync_from->id, wrl_sync_from->hw_port);
								break;
							}
							break;
						default :
							break;
						}
					}
				}
			}

			if (ret == RTW_PHL_STATUS_SUCCESS)
				band_ctl->tsf_sync_port = wrl_sync_from->hw_port;
			else
				band_ctl->tsf_sync_port = HW_PORT_MAX;
		}
		break;
	case PHL_ROLE_MSTS_STA_DIS_CONN:
		/* if TSF sync do not enable, skip disable flow */
		if (band_ctl->tsf_sync_port == HW_PORT_MAX) {
			ret = RTW_PHL_STATUS_SUCCESS;
			goto exit;
		}

		if (chanctx == NULL) {
			PHL_WARN("%s: state(%d) wifi role (id=%d) role link (id=%d) chanctx=NULL\n",
				__func__, state, wrole->id, rlink->id);
			ret = RTW_PHL_STATUS_FAILURE;
			goto exit;
		}

		/* if disconnected STA is sync port and SoftAP exists,
		find new sync port */
		if (rlink->hw_port == band_ctl->tsf_sync_port &&
			ap_num >= 1 && ld_sta_num > 0) {
			/* find linked sta */
			ld_sta_wr = _search_ld_sta_wrole(wrole, rlink, true);

			if (ld_sta_wr) {
				/* Get role link from ld_sta_wr within the same hw band */
				for (lidx = 0; lidx < ld_sta_wr->rlink_num; lidx++) {
					wrl_sync_from = get_rlink(ld_sta_wr, lidx);
					if (wrl_sync_from->hw_band == band)
						break;
				}
				if (lidx == ld_sta_wr->rlink_num) {
					PHL_ERR("%s Can not get target link in the same hw band in another role!\n", __func__);
					ret = RTW_PHL_STATUS_FAILURE;
					goto exit;
				}

				/* re-sync tsf for all softap */
				for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
					if (chanctx->role_map & BIT(ridx)) {
						check_wr = &phl_com->wifi_roles[ridx];

						check_wr = &phl_com->wifi_roles[ridx];
						/* Get role link from another role within the same chanctx */
						for (lidx = 0; lidx < check_wr->rlink_num; lidx++) {
							wrl_sync_to = get_rlink(check_wr, lidx);
							if (wrl_sync_to->hw_band == band)
								break;
						}
						if (lidx == check_wr->rlink_num) {
							PHL_ERR("%s Can not get target link in the same hw band in another role!\n", __func__);
							ret = RTW_PHL_STATUS_FAILURE;
							goto exit;
						}
						if (ld_sta_wr != check_wr) {
							switch (check_wr->type) {
							case PHL_RTYPE_AP:
							case PHL_RTYPE_VAP:
							case PHL_RTYPE_P2P_GO:
							case PHL_RTYPE_MESH:
								if (rtw_hal_tsf_sync(phl_info->hal, wrl_sync_from->hw_port,
									wrl_sync_to->hw_port, band, tsf_sync_offset_tu, HAL_TSF_EN_SYNC_AUTO) == RTW_HAL_STATUS_SUCCESS) {
									ret = RTW_PHL_STATUS_SUCCESS;
									PHL_INFO("%s, enable wrole-rlink:%d-%d(port:%d) sync from wrole-rlink:%d-%d(port:%d) success\n",
										 __func__, check_wr->id, wrl_sync_to->id, wrl_sync_to->hw_port,
										 ld_sta_wr->id, wrl_sync_from->id, wrl_sync_from->hw_port);
								} else {
									ret = RTW_PHL_STATUS_FAILURE;
									PHL_ERR("%s, enable wrole-rlink:%d-%d(port:%d) sync from wrole-rlink:%d-%d(port:%d) fail\n",
										 __func__, check_wr->id, wrl_sync_to->id, wrl_sync_to->hw_port,
										 ld_sta_wr->id, wrl_sync_from->id, wrl_sync_from->hw_port);
									break;
								}
								break;
							default :
								break;
							}
						}
					}
				}

				if (ret == RTW_PHL_STATUS_SUCCESS)
					band_ctl->tsf_sync_port = wrl_sync_from->hw_port;
				else
					band_ctl->tsf_sync_port = HW_PORT_MAX;
			}
		}

		/* if disconnected STA is sync port and no other linked sta exist,
		disable sofap tsf sync */
		if (rlink->hw_port == band_ctl->tsf_sync_port && ld_sta_num == 0) {
			for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
				if (chanctx->role_map & BIT(ridx)){
					check_wr = &phl_com->wifi_roles[ridx];

					/* Get role link from another role within the same chanctx */
					for (lidx = 0; lidx < check_wr->rlink_num; lidx++) {
						wrl_sync_to = get_rlink(check_wr, lidx);
						if (wrl_sync_to->hw_band == band)
							break;
					}
					if (lidx == check_wr->rlink_num) {
						PHL_ERR("%s Can not get target link in the same hw band in another role!\n", __func__);
						ret = RTW_PHL_STATUS_FAILURE;
						goto exit;
					}
					if (wrole != check_wr) {
						switch (check_wr->type) {
						case PHL_RTYPE_AP:
						case PHL_RTYPE_VAP:
						case PHL_RTYPE_P2P_GO:
						case PHL_RTYPE_MESH:
							if (check_wr->mstate == MLME_LINKED) {
								if (rtw_hal_tsf_sync(phl_info->hal, band_ctl->tsf_sync_port,
									wrl_sync_to->hw_port, band, tsf_sync_offset_tu, HAL_TSF_DIS_SYNC_AUTO) == RTW_HAL_STATUS_SUCCESS) {
									ret = RTW_PHL_STATUS_SUCCESS;
									PHL_INFO("%s, disable wrole-rlink:%d-%d(port:%d) sync from wrole (port:%d) success\n",
										__func__, check_wr->id, wrl_sync_to->id, wrl_sync_to->hw_port,
										band_ctl->tsf_sync_port);
								} else {
									ret = RTW_PHL_STATUS_FAILURE;
									PHL_ERR("%s, disable wrole-rlink:%d-%d(port:%d) sync from wrole:(port:%d) fail\n",
										__func__, check_wr->id, wrl_sync_to->id, wrl_sync_to->hw_port,
										band_ctl->tsf_sync_port);
									break;
								}
							}
							break;
						default :
							break;
						}
					}
				}
			}

			if (ret == RTW_PHL_STATUS_SUCCESS)
				band_ctl->tsf_sync_port = HW_PORT_MAX;
		}
		break;
	case PHL_ROLE_MSTS_AP_START:
		if (chanctx == NULL) {
			PHL_WARN("%s: state%d wifi role (id=%d) rlink (id=%d)chanctx=NULL\n", __func__, state, wrole->id, rlink->id);
			ret = RTW_PHL_STATUS_FAILURE;
			goto exit;
		}
		/* no linked sta, don't enable tsf sync */
		if (ld_sta_num == 0) {
			ret = RTW_PHL_STATUS_SUCCESS;
			goto exit;
		}
		/* New softAP start, and no tsf sync before, find sync port  */
		if (band_ctl->tsf_sync_port == HW_PORT_MAX) {
			wrl_sync_to = rlink;
			ld_sta_wr = _search_ld_sta_wrole(wrole, rlink, true);
			if (ld_sta_wr) {
				/* Get role link from ld_sta_wr within the same hw band */
				for (lidx = 0; lidx < ld_sta_wr->rlink_num; lidx++) {
					wrl_sync_from = get_rlink(ld_sta_wr, lidx);
					if (wrl_sync_from->hw_band == band)
						break;
				}
				if (lidx == ld_sta_wr->rlink_num) {
					PHL_ERR("%s Can not get target link in the same hw band in another role!\n", __func__);
					ret = RTW_PHL_STATUS_FAILURE;
					goto exit;
				}
				if (rtw_hal_tsf_sync(phl_info->hal, wrl_sync_from->hw_port,
					wrl_sync_to->hw_port, band, tsf_sync_offset_tu, HAL_TSF_EN_SYNC_AUTO) == RTW_HAL_STATUS_SUCCESS) {
					band_ctl->tsf_sync_port = wrl_sync_from->hw_port;
					ret = RTW_PHL_STATUS_SUCCESS;
					PHL_INFO("%s, enable wrole-rlink:%d-%d(port:%d) sync from wrole-rlink:%d-%d(port:%d) success\n",
						__func__, wrole->id, wrl_sync_to->id, wrl_sync_to->hw_port,
						ld_sta_wr->id, wrl_sync_from->id, wrl_sync_from->hw_port);
				} else {
					ret = RTW_PHL_STATUS_FAILURE;
					PHL_ERR("%s, enable wrole-rlink:%d-%d(port:%d) sync from wrole-rlink:%d-%d(port:%d) fail\n",
						__func__, wrole->id, wrl_sync_to->id, wrl_sync_to->hw_port,
						ld_sta_wr->id, wrl_sync_from->id, wrl_sync_from->hw_port);
				}
			}
		} else if (band_ctl->tsf_sync_port != HW_PORT_MAX) {
			/* New softAP start, enable tsf sync before, follow original sync port  */
			wrl_sync_to = rlink;
			if (rtw_hal_tsf_sync(phl_info->hal, band_ctl->tsf_sync_port,
				wrl_sync_to->hw_port, band, tsf_sync_offset_tu, HAL_TSF_EN_SYNC_AUTO) == RTW_HAL_STATUS_SUCCESS) {
				ret = RTW_PHL_STATUS_SUCCESS;
				PHL_INFO("%s, enable wrole-rlink:%d-%d(port:%d) sync from wrole(port:%d) success\n",
					__func__, wrole->id, wrl_sync_to->id, wrl_sync_to->hw_port,
					band_ctl->tsf_sync_port);
			} else {
				ret = RTW_PHL_STATUS_FAILURE;
				PHL_ERR("%s, enable wrole-rlink:%d-%d(port:%d) sync from wrole(port:%d) fail\n",
					__func__, wrole->id, wrl_sync_to->id, wrl_sync_to->hw_port,
					band_ctl->tsf_sync_port);
			}
		}
		break;
	case PHL_ROLE_MSTS_AP_STOP:
		/* if TSF sync do not enable, skip disable flow */
		if (band_ctl->tsf_sync_port == HW_PORT_MAX) {
			ret = RTW_PHL_STATUS_SUCCESS;
			goto exit;
		}
		wrl_sync_to = rlink;
		if (rtw_hal_tsf_sync(phl_info->hal, band_ctl->tsf_sync_port,
					wrl_sync_to->hw_port, band, tsf_sync_offset_tu, HAL_TSF_DIS_SYNC_AUTO) == RTW_HAL_STATUS_SUCCESS) {
			ret = RTW_PHL_STATUS_SUCCESS;
			PHL_INFO("%s, disable wrole-rlink:%d-%d(port:%d) sync from wrole(port:%d) success\n",
				__func__, wrole->id, wrl_sync_to->id, wrl_sync_to->hw_port,
				band_ctl->tsf_sync_port);
		} else {
			ret = RTW_PHL_STATUS_FAILURE;
			PHL_ERR("%s, disable wrole-rlink:%d-%d(port:%d) sync from wrole(port:%d) fail\n",
				__func__, wrole->id, wrl_sync_to->id, wrl_sync_to->hw_port,
				band_ctl->tsf_sync_port);
		}

		if (ap_num == 0)
			band_ctl->tsf_sync_port = HW_PORT_MAX;
		break;
	default:
		PHL_ERR("%s unsupport state(%d)\n", __func__, state);
		ret = RTW_PHL_STATUS_FAILURE;
		break;
	}

exit:
#endif
	return ret;
}

#ifdef CONFIG_FSM
enum rtw_phl_status
rtw_phl_mr_offch_hdl(void *phl,
		     struct rtw_wifi_role_t *wrole,
		     bool off_ch,
		     void *obj_priv,
		     bool (*issue_null_data)(void *priv, u8 ridx, bool ps),
		     struct rtw_chan_def *chandef)
{
	return RTW_PHL_STATUS_SUCCESS;
}
#endif

void phl_mr_stop_all_beacon(struct phl_info_t *phl_info,
                            struct rtw_wifi_role_t *wrole,
                            u8 band_idx,
                            bool stop)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);
	u8 role_map = band_ctrl->role_map;
	struct rtw_wifi_role_t *wr = NULL;
	u8 ridx;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (role_map & BIT(ridx)) {
			wr = &(phl_com->wifi_roles[ridx]);


			if (rtw_phl_role_is_ap_category(wr) ||
			   (wr->type == PHL_RTYPE_MESH)){
				for (idx = 0; idx < wr->rlink_num; idx++) {
					rlink = get_rlink(wr, idx);
					if (rlink->hw_band == band_idx) {
						if(((TEST_STATUS_FLAG(rlink->status, RLINK_STATUS_BCN_STOP)) && stop) ||
				                   ((!TEST_STATUS_FLAG(rlink->status, RLINK_STATUS_BCN_STOP)) && !stop))
							continue;

						rtw_hal_beacon_stop(phl_info->hal, rlink, stop);

						if(stop)
							SET_STATUS_FLAG(rlink->status, RLINK_STATUS_BCN_STOP);
						else
							CLEAR_STATUS_FLAG(rlink->status, RLINK_STATUS_BCN_STOP);
					}
				}
			}
		}
	}
}

#ifdef DBG_PHL_MR
enum rtw_phl_status
phl_mr_info_dbg(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	u8 ridx = MAX_WIFI_ROLE_NUMBER;
	u8 bidx = 0;
	int chanctx_num = 0;
	struct rtw_wifi_role_t *role = NULL;
	struct hw_band_ctl_t *band_ctrl = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		role = &(phl_com->wifi_roles[ridx]);

		for (idx = 0; idx < role->rlink_num; idx++) {
			rlink = get_rlink(role, idx);

			if (rlink->assoc_sta_queue.cnt) {
				PHL_DUMP_STACTRL_EX(phl_info);
				PHL_ERR("role_idx:%d link_idx: %d assoc_sta_queue(%d) not empty!\n",
					ridx, idx, rlink->assoc_sta_queue.cnt);
				_os_warn_on(1);
			}
		}
	}
	for (bidx = 0; bidx < MAX_BAND_NUM; bidx++) {
		band_ctrl = &(mr_ctl->band_ctrl[bidx]);
		chanctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);
		if (chanctx_num)
			PHL_ERR("band_idx:%d chanctx_num(%d) not empty!\n", bidx, chanctx_num);
	}

	return RTW_PHL_STATUS_SUCCESS;
}
#endif

enum rtw_phl_status
phl_mr_info_upt(struct phl_info_t *phl_info, struct rtw_wifi_role_link_t *rlink)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[rlink->hw_band]);
	u8 role_map = band_ctrl->role_map;
	struct rtw_wifi_role_t *wr = NULL;
	u8 ridx;

	_os_mem_set(drv, &band_ctrl->cur_info, 0, sizeof(struct mr_info));

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (role_map & BIT(ridx)) {
			wr = &(phl_com->wifi_roles[ridx]);

			switch (wr->type) {
			case PHL_RTYPE_STATION:
			case PHL_RTYPE_P2P_GC:
			case PHL_RTYPE_P2P_DEVICE:
			case PHL_RTYPE_TDLS:
				band_ctrl->cur_info.sta_num++;
				if (wr->mstate == MLME_LINKED)
					band_ctrl->cur_info.ld_sta_num++;
				if (wr->mstate == MLME_LINKING)
					band_ctrl->cur_info.lg_sta_num++;
				if (wr->type == PHL_RTYPE_P2P_GC)
					band_ctrl->cur_info.p2p_gc_num++;
				if (wr->type == PHL_RTYPE_P2P_DEVICE)
					band_ctrl->cur_info.p2p_device_num++;
			#ifdef CONFIG_PHL_TDLS
				if (wr->type == PHL_RTYPE_TDLS)
					band_ctrl->cur_info.ld_tdls_num++;
			#endif
				break;

			case PHL_RTYPE_AP:
			case PHL_RTYPE_VAP:
			case PHL_RTYPE_P2P_GO:
			case PHL_RTYPE_MESH:
				if (wr->mstate == MLME_LINKED)
					band_ctrl->cur_info.ap_num++;
				if (wr->assoc_mld_queue.cnt >= 2)
					band_ctrl->cur_info.ld_ap_num++;
				if (wr->type == PHL_RTYPE_P2P_GO)
					band_ctrl->cur_info.p2p_go_num++;
				break;

			case PHL_RTYPE_MONITOR:
				band_ctrl->cur_info.monitor_num++;
				break;

			case PHL_RTYPE_ADHOC:
			case PHL_RTYPE_ADHOC_MASTER:
			case PHL_RTYPE_NAN:
			default :
				break;
			}
		}
	}
	if(band_ctrl->op_mode == MR_OP_SCC ||
	   band_ctrl->op_mode == MR_OP_MCC){
		if(band_ctrl->cur_info.ld_sta_num && band_ctrl->cur_info.ap_num)
			band_ctrl->op_type = MR_OP_TYPE_STATION_AP;
		else if(band_ctrl->cur_info.ld_sta_num)
			band_ctrl->op_type = MR_OP_TYPE_STATION_ONLY;
		else
			band_ctrl->op_type = MR_OP_TYPE_STATION_AP;
	}
	else{
		band_ctrl->op_type = MR_OP_TYPE_NONE;
	}
	/*dump mr_info*/
	PHL_INFO("%s sta_num:%d, ld_sta_num:%d, lg_sta_num:%d\n",
		__func__, band_ctrl->cur_info.sta_num,
		band_ctrl->cur_info.ld_sta_num, band_ctrl->cur_info.lg_sta_num);
#ifdef CONFIG_PHL_TDLS
	PHL_INFO("%s ld_tdls_num:%d\n", __func__, band_ctrl->cur_info.ld_tdls_num);
#endif
	PHL_INFO("%s ap_num:%d, ld_ap_num:%d\n",
		__func__, band_ctrl->cur_info.ap_num, band_ctrl->cur_info.ld_ap_num);
	PHL_INFO("%s op mode:%d op type:%d\n",
		__func__, band_ctrl->op_mode, band_ctrl->op_type);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_mr_watchdog(struct phl_info_t *phl_info)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
#if defined(CONFIG_MR_SUPPORT) &&  defined(CONFIG_MCC_SUPPORT)
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	u8 band_idx = 0;
	int chanctx_num = 0;
	struct hw_band_ctl_t *band_ctrl = NULL;
	#if 0
	struct rtw_chan_ctx *chanctx = NULL;
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_wifi_role_t *wr = NULL;
	#endif

	for (band_idx = 0; band_idx < MAX_BAND_NUM; band_idx++) {
		band_ctrl = &(mr_ctl->band_ctrl[band_idx]);
		chanctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);
		if (chanctx_num == 0)
			continue;

		#if 0
		_os_spinlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
		phl_list_for_loop(chanctx, struct rtw_chan_ctx, &band_ctrl->chan_ctx_queue.queue, list) {
			for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
				if (chanctx->role_map & BIT(ridx)) {
					wr = &phl_com->wifi_roles[ridx];
					if(wr->type == PHL_RTYPE_STATION) {
						/*Sounding check*/
						/*phl_snd_watchdog(phl_info, wr);*/
					}
				}
			}
		}
		_os_spinunlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
		#endif

		if (chanctx_num == 2)
			rtw_phl_mcc_watchdog(phl_info, band_idx);
	}
#endif /*CONFIG_MCC_SUPPORT*/
	return psts;
}

static inline u8
__phl_mr_process(struct rtw_wifi_role_t *wrole,
		u8 role_map, bool exclude_self, void *data,
		u8(*ops_func)(struct rtw_wifi_role_t *wrole, void *data))
{
	u8 ridx = 0;
	u8 ret = 0;
	struct rtw_phl_com_t *phl_com = wrole->phl_com;
	struct rtw_wifi_role_t *wr = NULL;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (role_map & BIT(ridx)) {
			wr = &phl_com->wifi_roles[ridx];
			_os_warn_on(!wr->active);
			if ((exclude_self) && (wr == wrole))
				continue;
			if (ops_func)
				if (true == ops_func(wr, data))
					ret++;
		}
	}
	return ret;
}

static u8 _phl_mr_process_by_mrc(struct phl_info_t *phl_info,
		struct rtw_wifi_role_t *wrole, bool exclude_self, void *data,
		u8(*ops_func)(struct rtw_wifi_role_t *wrole, void *data))
{
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(wrole->phl_com);

	return __phl_mr_process(wrole, mr_ctl->role_map, exclude_self, data, ops_func);
}

static u8 _phl_mr_process_by_band(struct phl_info_t *phl_info,
                                  struct rtw_wifi_role_t *wrole,
                                  enum phl_band_idx band_idx,
                                  bool exclude_self,
                                  void *data,
                                  u8(*ops_func)(struct rtw_wifi_role_t *wrole,
                                                void *data))
{
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);

	return __phl_mr_process(wrole, band_ctrl->role_map, exclude_self, data, ops_func);
}

static u8 _phl_mr_process_by_chctx(struct phl_info_t *phl_info,
                                   struct rtw_wifi_role_t *wrole,
                                   struct rtw_wifi_role_link_t *rlink,
                                   bool exclude_self,
                                   void *data,
                                   u8(*ops_func)(struct rtw_wifi_role_t *wrole, void *data))
{
	struct rtw_chan_ctx *chanctx = rlink->chanctx;

	if (!chanctx)
		return 0;
	return __phl_mr_process(wrole, chanctx->role_map, exclude_self, data, ops_func);
}

static u8 _phl_mr_dump_mac_addr(struct rtw_wifi_role_t *wrole, void *data)
{
	PHL_INFO("RIDX:%d - MAC-Addr:%02x-%02x-%02x-%02x-%02x-%02x\n",
			wrole->id, wrole->mac_addr[0], wrole->mac_addr[1], wrole->mac_addr[2],
			wrole->mac_addr[3], wrole->mac_addr[4], wrole->mac_addr[5]);
	return true;
}
u8 rtw_phl_mr_dump_mac_addr(void *phl,
					struct rtw_wifi_role_t *wifi_role)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return _phl_mr_process_by_mrc(phl_info, wifi_role, false, NULL, _phl_mr_dump_mac_addr);
}
u8 rtw_phl_mr_buddy_dump_mac_addr(void *phl,
					struct rtw_wifi_role_t *wifi_role)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return _phl_mr_process_by_mrc(phl_info, wifi_role, true, NULL, _phl_mr_dump_mac_addr);
}

static inline u8
__phl_mr_process_ex(struct rtw_phl_com_t *phl_com,
		u8 role_map, void *data,
		u8(*ops_func)(void *drv_priv, u8 wr_idx, void *data))
{
	u8 ridx = 0;
	u8 ret = 0;
	struct rtw_wifi_role_t *wr = NULL;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (role_map & BIT(ridx)) {
			wr = &phl_com->wifi_roles[ridx];
			_os_warn_on(!wr->active);

			if (ops_func)
				if (true == ops_func(phl_com->drv_priv, wr->id, data))
					ret++;
		}
	}
	return ret;
}

u8 rtw_phl_mr_process_by_band(void *phl,
		enum phl_band_idx band_idx, void *data,
		u8(*ops_func)(void *drv_priv, u8 wr_idx, void *data))
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);

	return __phl_mr_process_ex(phl_info->phl_com, band_ctrl->role_map, data, ops_func);
}

bool rtw_phl_mr_query_info(void *phl,
                           struct rtw_wifi_role_t *wrole,
                           struct rtw_wifi_role_link_t *rlink,
                           struct mr_query_info *info)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[rlink->hw_band]);

	info->op_mode = band_ctrl->op_mode;
	info->op_type = band_ctrl->op_type;
	_os_mem_cpy(phl_to_drvpriv(phl_info), &info->cur_info,
			&band_ctrl->cur_info, sizeof(struct mr_info));
	return true;
}

void rtw_phl_mr_ops_init(void *phl, struct rtw_phl_mr_ops *mr_ops)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);

	mr_ctl->mr_ops.priv = mr_ops->priv;
#ifdef CONFIG_PHL_P2PPS
	mr_ctl->mr_ops.phl_mr_update_noa = mr_ops->phl_mr_update_noa;
#endif /* CONFIG_PHL_P2PPS */
#ifdef CONFIG_MCC_SUPPORT
	if (mr_ops->mcc_ops != NULL)
		rtw_phl_mcc_init_ops(phl_info, mr_ops->mcc_ops);
#endif
#ifdef CONFIG_DBCC_SUPPORT
	mr_ctl->mr_ops.dbcc_protocol_hdl = mr_ops->dbcc_protocol_hdl;
#endif

}

u8 rtw_phl_mr_get_opch_list(void *phl, u8 hw_band,
                            struct rtw_chan_def *chdef_list, u8 list_size,
                            struct rtw_wifi_role_link_t *rlink[], u8 rlink_num)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[hw_band]);
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_chan_ctx *chanctx = NULL;
	int ctx_num = 0;
	u8 total_op_num = 0, i = 0;

	if((chdef_list == NULL) || (list_size == 0)){
		PHL_ERR("Parameter Invalid!\n");
		goto _exit;
	}

	ctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);
	if (ctx_num == 0){
		PHL_DBG("ctx_num == 0!\n");
		goto _exit;
	}

	_os_spinlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
	phl_list_for_loop(chanctx, struct rtw_chan_ctx, &band_ctrl->chan_ctx_queue.queue, list) {

		if(total_op_num >= list_size)
			break;

		if (rlink_num != 0 && rlink != NULL) {
			for (i = 0; i < rlink_num; i++) {
				if (rlink[i]->hw_band == hw_band) {
					if (rlink[i]->chandef.chan == chanctx->chan_def.chan) {
						*(chdef_list + total_op_num) = chanctx->chan_def;
						total_op_num++;
						break;
					}
				}
			}
		} else {
			*(chdef_list + total_op_num) = chanctx->chan_def;
			total_op_num++;
		}
	}
	_os_spinunlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
_exit:
	return total_op_num;
}

enum mr_op_mode
rtw_phl_mr_get_opmode(void *phl,
                      struct rtw_wifi_role_t *wrole,
                      struct rtw_wifi_role_link_t *rlink)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[rlink->hw_band]);

	return band_ctrl->op_mode;
}

void
phl_mr_check_ecsa(struct phl_info_t *phl_info,
                  enum phl_band_idx hw_band
)
{
#ifdef CONFIG_PHL_ECSA
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[hw_band]);
	u8 role_map = band_ctrl->role_map;
	struct rtw_wifi_role_t *wr = NULL, *ap_wr = NULL, *sta_wr = NULL;
	u8 ridx;
	enum band_type ap_band_type = BAND_ON_24G, sta_band_type = BAND_ON_24G;
	enum phl_ecsa_start_reason reason = ECSA_START_UNKNOWN;
	u32 delay_start_ms = 0;
#ifdef CONFIG_PHL_ECSA_EXTEND_OPTION
	u32 extend_option = 0;
#endif
	bool ecsa_allow = false;
	struct rtw_phl_ecsa_param param = {0};
	void *d = phlcom_to_drvpriv(phl_com);
	struct rtw_wifi_role_link_t *rlink = NULL, *sta_rlink = NULL, *ap_rlink = NULL;

	if(band_ctrl->op_mode != MR_OP_MCC)
		return;
	if(band_ctrl->op_type != MR_OP_TYPE_STATION_AP)
		return;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (role_map & BIT(ridx)) {
			wr = &(phl_com->wifi_roles[ridx]);

			if (wr->rlink_num > 1)
				continue;

			rlink = phl_get_rlink_by_hw_band(wr, hw_band);

			switch (wr->type) {
			case PHL_RTYPE_STATION:
			case PHL_RTYPE_P2P_GC:
			case PHL_RTYPE_TDLS:
				if (rlink->mstate == MLME_LINKED){
					sta_band_type = rlink->chandef.band;
					sta_wr = wr;
					sta_rlink = rlink;
				}
				break;
			case PHL_RTYPE_AP:
			case PHL_RTYPE_VAP:
			case PHL_RTYPE_P2P_GO:
			case PHL_RTYPE_MESH:
				if (rlink->mstate == MLME_LINKED){
					ap_band_type = rlink->chandef.band;
					ap_wr = wr;
					ap_rlink = rlink;
				}
				break;
			case PHL_RTYPE_MONITOR:
			case PHL_RTYPE_P2P_DEVICE:
			default :
				break;
			}
		}
	}
	if(sta_band_type == BAND_ON_24G){
		if(ap_band_type == BAND_ON_24G)
			reason = ECSA_START_MCC_24G_TO_24G;
		else if(ap_band_type == BAND_ON_5G)
			reason = ECSA_START_MCC_5G_TO_24G;
		else
			reason = ECSA_START_UNKNOWN;
	}
	else if(sta_band_type == BAND_ON_5G){
		if(ap_band_type == BAND_ON_24G)
			reason = ECSA_START_MCC_24G_TO_5G;
		else if(ap_band_type == BAND_ON_5G)
			reason = ECSA_START_MCC_5G_TO_5G;
		else
			reason = ECSA_START_UNKNOWN;
	}
	else{
		reason = ECSA_START_UNKNOWN;
	}

	if(reason != ECSA_START_UNKNOWN){
		ecsa_allow = rtw_phl_ecsa_check_allow(phl_info,
		                                      ap_wr,
		                                      sta_rlink->chandef,
		                                      reason,
#ifdef CONFIG_PHL_ECSA_EXTEND_OPTION
		                                      &extend_option,
#endif
		                                      &delay_start_ms);
	}

	if(ecsa_allow){
		param.ecsa_type = ECSA_TYPE_AP;
		param.count = ECSA_DEFAULT_CHANNEL_SWITCH_COUNT;
		param.delay_start_ms = delay_start_ms;
		param.flag = 0;
		param.mode = CHANNEL_SWITCH_MODE_NORMAL;
		param.ch = sta_rlink->chandef.chan;
		param.op_class = rtw_phl_get_operating_class(sta_rlink->chandef);
		_os_mem_cpy(d, &(param.new_chan_def), &(sta_rlink->chandef),
			    sizeof(struct rtw_chan_def));

#ifdef CONFIG_PHL_ECSA_EXTEND_OPTION
		rtw_phl_ecsa_extend_option_hdlr(extend_option,
						ap_rlink,
						&param);
#endif

		pstatus = rtw_phl_ecsa_start(phl_info,
		                             ap_wr,
		                             ap_rlink,
		                             &param);
		if(pstatus == RTW_PHL_STATUS_SUCCESS)
			PHL_INFO("%s: ECSA start OK!\n", __FUNCTION__);
		else
			PHL_INFO("%s: ECSA start fail!\n", __FUNCTION__);
	}
#endif /* CONFIG_PHL_ECSA */
}

void
phl_mr_check_ecsa_cancel(struct phl_info_t *phl_info,
                         struct rtw_wifi_role_t *wrole,
                         struct rtw_wifi_role_link_t *rlink)
{
#ifdef CONFIG_PHL_ECSA
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[rlink->hw_band]);

	if(band_ctrl->op_mode != MR_OP_MCC)
		return;
	if(band_ctrl->op_type != MR_OP_TYPE_STATION_AP)
		return;

	pstatus = rtw_phl_ecsa_cancel(phl_info, wrole);
	if(pstatus == RTW_PHL_STATUS_SUCCESS)
		PHL_INFO("%s: ECSA cancel OK!\n", __FUNCTION__);
	else
		PHL_INFO("%s: ECSA cancel fail!\n", __FUNCTION__);
#endif /* CONFIG_PHL_ECSA */
}

enum rtw_phl_status
phl_mr_err_recovery(struct phl_info_t *phl, enum phl_msg_evt_id eid)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;

	if (eid == MSG_EVT_SER_M5_READY) {
		/* SER L1 DONE event */
		rtw_phl_tdmra_recovery(phl, HW_BAND_0);
	}

	return status;
}

struct rtw_wifi_role_link_t *
phl_mr_get_first_rlink_by_band(struct phl_info_t *phl,
				enum phl_band_idx band)
{
	struct hw_band_ctl_t *band_ctrl = get_band_ctrl(phl, band);
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 role_map = band_ctrl->role_map;
	u8 ridx = 0, idx = 0;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (!(role_map & BIT(ridx)))
			continue;
		wrole = &phl->phl_com->wifi_roles[ridx];

		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rlink = get_rlink(wrole, idx);
			if (rlink->chanctx == NULL)
				continue;
			if (rlink->hw_band == band) {
				PHL_INFO("%s: ridx(%d), rlink[%d]\n", __FUNCTION__,
					ridx, idx);
				return rlink;
			}
		}
	}
	return NULL;
}

struct rtw_wifi_role_link_t *
phl_mr_get_rlink_stay_in_cur_chdef(struct phl_info_t *phl,
				enum phl_band_idx band)
{
	struct hw_band_ctl_t *band_ctrl = get_band_ctrl(phl, band);
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_chan_def cur_chdef = {0};
	u8 role_map = band_ctrl->role_map;
	u8 ridx = 0, idx = 0;

	rtw_hal_get_cur_chdef(phl->hal, band, &cur_chdef);
	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (!(role_map & BIT(ridx)))
			continue;
		wrole = &phl->phl_com->wifi_roles[ridx];
		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rlink = get_rlink(wrole, idx);
			if (rlink->chanctx == NULL)
				continue;
			if (rlink->hw_band != band)
				continue;
			if (rlink->chanctx->chan_def.chan == cur_chdef.chan) {
				PHL_INFO("%s: ridx(%d), rlink[%d]\n", __FUNCTION__,
					ridx, idx);
				return rlink;
			}
		}
	}
	return NULL;
}


/* MR coex related code */

