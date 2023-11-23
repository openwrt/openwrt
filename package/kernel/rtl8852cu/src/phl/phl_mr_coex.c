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
#define _PHL_MR_COEX_C_
#include "phl_headers.h"

#ifdef CONFIG_MR_COEX_SUPPORT
static bool _tdmra_need(struct phl_info_t *phl_info,
				enum phl_band_idx band_idx, u8 *ap_role_idx)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);
	struct mr_info *cur_info = &band_ctrl->cur_info;
	u8 role_map = band_ctrl->role_map;
	struct rtw_wifi_role_t *wr = NULL, *ap_wr = NULL;
	struct rtw_chan_def *chandef = NULL;
	int ctx_num = 0;
	u8 ridx;
	u8 tdmra_need = false;
	struct rtw_wifi_role_link_t *rlink = NULL;

	ctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);
	if (ctx_num == 0)
		goto exit;

	PHL_INFO("[MR]%s: band_idx=%d,ctx_num=%d,ap_num=%d,op_mode=%d\n",
		__func__, band_idx, ctx_num, cur_info->ap_num, band_ctrl->op_mode);
	if (ctx_num == 1) {
		if (cur_info->ap_num == 1) {
			/* only for sole AP, check op mode */
			if (band_ctrl->op_mode == MR_OP_SWR) {
				/* find sole AP */
				for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
					if (role_map & BIT(ridx)) {
						wr = &(phl_com->wifi_roles[ridx]);
						if (rtw_phl_role_is_ap_category(wr)
							&& wr->mstate == MLME_LINKED) {
							ap_wr = wr;
							*ap_role_idx = ap_wr->id;
							break;
						}
					}
				}

				rlink = phl_get_rlink_by_hw_band(wr,
				                                 band_idx);
				chandef = &rlink->chandef;

				/* enable tdmra for 2.4G sole AP */
				if (rtw_hal_get_btc_req_slot(phl_info->hal, band_idx) > 0
					&& chandef->band == BAND_ON_24G) {
					tdmra_need = true;
				} else {
					tdmra_need = false;
				}
			} else {
				/* SCC case */
				tdmra_need = false;
			}
		} else {
			if (rtw_hal_get_btc_req_slot(phl_info->hal, band_idx) > 0)
				PHL_INFO("[MR]%s: Do not support for nonAP + BT in one ch ctx\n", __func__);

			tdmra_need = false;
		}
	} else if (ctx_num == 2) {
		if (phl_com->dev_cap.mcc_sup == false) {
			PHL_INFO("%s(): don't support MCC\n", __func__);
			tdmra_need = false;
		} else {
			tdmra_need = true;
		}
	} else {
		PHL_INFO("[MR]%s: Do not support ctx_num(%d)\n",
			__func__, ctx_num);
	}
exit:
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "<<< %s: tdmra_need(%d)\n",
		__func__, tdmra_need);
	return tdmra_need;
}

/*
 * check all role state
 * Output: True: can enabe TDMRA, False: can't enable TDMRA
 */
static bool _role_state_check(struct phl_info_t *phl,
				enum phl_band_idx band_idx)
{
	struct rtw_wifi_role_t *wr = NULL;
	u8 ridx = INVALID_WIFI_ROLE_IDX, role_map = 0;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 lidx = 0;

	role_map = phl_get_chanctx_rolemap(phl, band_idx);
	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (!(role_map & BIT(ridx)))
			continue;
		wr = &(phl->phl_com->wifi_roles[ridx]);
		if (MLME_LINKED != wr->mstate) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MLME_LINKED != wr->mstate, we can't enable tdmra now, ridx(%d), type(%d), mstate(%d)\n",
				__FUNCTION__, ridx, wr->type, wr->mstate);
			return false;
		}
		if (rtw_phl_role_is_client_category(wr)) {
			for (lidx = 0; lidx < wr->rlink_num; lidx++) {
				rlink = get_rlink(wr, lidx);
				if (!TEST_STATUS_FLAG(rlink->status, RLINK_STATUS_TSF_SYNC)) {
					PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: Test RLINK_STATUS_TSF_SYNC fail, we can't enable tdmra now, ridx(%d), lidx(%d), type(%d), status(%d)\n",
						__FUNCTION__, ridx, lidx, wr->type,
						rlink->status);
					return false;
				}
			}
		}
	}
	return true;
}

static bool _role_is_in_tdmra_chctx_q(struct phl_info_t *phl_info,
                                         struct rtw_wifi_role_t *wr,
                                         struct rtw_wifi_role_link_t *rlink)
{
	bool ret = false;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	void *drv = phl_to_drvpriv(phl_info);
	struct hw_band_ctl_t *band_ctrl = NULL;
	struct rtw_chan_ctx *chanctx = NULL;
	_os_list *chan_ctx_list = NULL;
	u8 role_map = 0;

	if (wr == NULL)
		goto exit;

	band_ctrl = &(mr_ctl->band_ctrl[rlink->hw_band]);
	chan_ctx_list = &band_ctrl->chan_ctx_queue.queue;

	/* find wr is under existed chanctx durin TDMRA */
	_os_spinlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
	phl_list_for_loop(chanctx, struct rtw_chan_ctx, chan_ctx_list, list) {
		role_map = chanctx->role_map;
		if (role_map & BIT(wr->id)) {
			ret = true;
			break;
		}
	}
	_os_spinunlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
exit:
	return ret;
}



/* find any existed role */
struct rtw_wifi_role_t *_find_existed_role(struct phl_info_t *phl_info,
							enum phl_band_idx band_idx)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_wifi_role_t *wr = NULL;
	u8 role_map;
	u8 ridx = 0;

	role_map = phl_get_chanctx_rolemap(phl_info, band_idx);
	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (role_map & BIT(ridx)) {
			wr = &phl_com->wifi_roles[ridx];
			break;
		}
	}

	return wr;
}

enum rtw_phl_status _tdmra_disable(struct phl_info_t *phl_info,
			struct rtw_wifi_role_t *cur_wrole, enum phl_band_idx band_idx)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	struct rtw_wifi_role_t *spec_role = NULL;
	struct rtw_wifi_role_link_t *cur_rlink = NULL;
	struct rtw_wifi_role_link_t *spec_rlink = NULL;

	if (!rtw_phl_mcc_inprogress(phl_info, band_idx)) {
		psts = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	} else {
		/* find existed tdmra role */
		spec_role = _find_existed_role(phl_info, band_idx);
		if (spec_role == NULL) {
			PHL_ERR("%s: find no tdmra role for tdmra disable\n", __func__);
			psts = RTW_PHL_STATUS_FAILURE;
			goto exit;
		}

		spec_rlink = phl_get_rlink_by_hw_band(spec_role,
		                                      band_idx);
	}

	if (cur_wrole == NULL) {
		/* check wrole is null or not */
		psts = rtw_phl_tdmra_disable(phl_info, spec_role, spec_rlink);
	} else {
		cur_rlink = phl_get_rlink_by_hw_band(cur_wrole, band_idx);
		/* check wrole is in chan ctx queue */
		if (_role_is_in_tdmra_chctx_q(phl_info, cur_wrole, cur_rlink))
			psts = rtw_phl_tdmra_disable(phl_info, cur_wrole, cur_rlink);
		else
			psts = rtw_phl_tdmra_disable(phl_info, spec_role, spec_rlink);
	}

exit:
	return psts;
}

/*
 * Handle MR coex mechanism for 2g_1ap_btc, mcc, mcc_btc
 * Specific concurrent mode : 2g ap category x1 + BTC, MCC, MCC + BTC
 * @handle: True: handle specific concurrent mode for all interfaces; False: Not handleand maybe handle it by other coex mechanism.
 */
enum rtw_phl_status
_tdmra_handle(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *cur_wrole, u16 slot,
				enum phl_band_idx band_idx,
				enum mr_coex_trigger trgger,
				enum mr_coex_mode *coex_mode)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct phl_tdmra_dur_change_info info = {0};
	bool tdmra_inprogress = false, tdmra_need = false;
	u8 ap_role_id = INVALID_WIFI_ROLE_IDX;
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	struct rtw_wifi_role_t * existed_role = NULL;
	struct rtw_wifi_role_link_t *cur_rlink = NULL;
	struct rtw_wifi_role_link_t *ap_rlink = NULL;
	struct rtw_wifi_role_link_t *existed_rlink = NULL;

	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, ">>> %s: slot(%d), band_idx(%d), trgger(%d)\n",
		__func__, slot, band_idx, trgger);
	if (!_role_state_check(phl_info, band_idx)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: Fail to check role state\n",
			__func__);
		goto exit;
	}
	tdmra_inprogress =  rtw_phl_mcc_inprogress(phl_info, band_idx);
	tdmra_need = _tdmra_need(phl_info, band_idx, &ap_role_id);
	if (tdmra_need) {
		switch (trgger){
		case MR_COEX_TRIG_BY_BT:
			if (tdmra_inprogress) {
				/* update TDMRA (if TDMRAing) */
				info.bt_role = true;
				info.hw_band = band_idx;
				/* find existed tdmra role */
				info.role = _find_existed_role(phl_info, band_idx);
				info.dur = slot;
				if (info.role != NULL)
					psts = rtw_phl_tdmra_duration_change(phl_info, &info);
				else
					PHL_ERR("%s: find no tdmra role for tdmra duration change\n", __func__);
			} else {
				/* enable TDMRA under solo AP (no TDMRA) */
				if (ap_role_id < INVALID_WIFI_ROLE_IDX) {
					*coex_mode = MR_COEX_MODE_TDMRA;

					ap_rlink = phl_get_rlink_by_hw_band(&phl_com->wifi_roles[ap_role_id],
					                                    band_idx);
					psts = rtw_phl_tdmra_enable(phl_info,
					                            &phl_com->wifi_roles[ap_role_id],
					                            ap_rlink);
				} else {
					PHL_ERR("%s: Do not find solo AP\n", __func__);
				}
			}
			break;
		case MR_COEX_TRIG_BY_LINKING:
			/* enable TDMRA (for link flow) */
			if (cur_wrole != NULL) {
				*coex_mode = MR_COEX_MODE_TDMRA;
				phl_mr_stop_all_beacon(phl_info, cur_wrole, band_idx, false);

				cur_rlink = phl_get_rlink_by_hw_band(cur_wrole, band_idx);
				psts = rtw_phl_tdmra_enable(phl_info,
				                            cur_wrole,
				                            cur_rlink);
			} else {
				PHL_ERR("%s: cur_wrole = NULL, check code flow\n", __func__);
			}
			break;
		case MR_COEX_TRIG_BY_DIS_LINKING:
			*coex_mode = MR_COEX_MODE_TDMRA;
			/* find any existed role to trigger TDMRA */
			existed_role = _find_existed_role(phl_info, band_idx);

			existed_rlink = phl_get_rlink_by_hw_band(existed_role,
			                                         band_idx);
			psts = rtw_phl_tdmra_enable(phl_info,
			                            existed_role,
			                            existed_rlink);
			break;
		case MR_COEX_TRIG_BY_CHG_SLOT:
			if (tdmra_inprogress) {
				/* update TDMRA (if TDMRAing) */
				info.bt_role = false;
				info.hw_band = band_idx;
				/* find existed tdmra role */
				info.role = _find_existed_role(phl_info, band_idx);
				info.dur = slot;
				if (info.role != NULL)
					psts = rtw_phl_tdmra_duration_change(phl_info, &info);
				else
					PHL_ERR("%s: find no tdmra role\n", __func__);
			}
			break;
		case MR_COEX_TRIG_BY_SCAN:
			*coex_mode = MR_COEX_MODE_TDMRA;

			cur_rlink = phl_get_rlink_by_hw_band(cur_wrole, band_idx);
			psts = rtw_phl_tdmra_enable(phl_info, cur_wrole, cur_rlink);
			break;
		case MR_COEX_TRIG_BY_ECSA:
		case MR_COEX_TRIG_BY_ASSOC_TSF_EFFOR:
		case MR_COEX_TRIG_BY_EN_DBCC:
		case MR_COEX_TRIG_BY_CHG_OP_CHDEF:
			*coex_mode = MR_COEX_MODE_TDMRA;

			cur_rlink = phl_get_rlink_by_hw_band(cur_wrole, band_idx);
			psts = rtw_phl_tdmra_enable(phl_info, cur_wrole, cur_rlink);
			break;
		default:
			break;
		}
	} else {
			/* disable TDMRA (if TDMRAing) */
		if (*coex_mode == MR_COEX_MODE_TDMRA) {
			psts = _tdmra_disable(phl_info, cur_wrole, band_idx);
			if (psts != RTW_PHL_STATUS_SUCCESS)
				PHL_ERR("%s: MR TDMRA disable fail\n", __func__);
			else
				*coex_mode = MR_COEX_MODE_NONE;
		}
	}
exit:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s: coex_mode(%d), psts(%d)\n",
		__func__, *coex_mode, psts);
	return psts;
}


#ifdef CONFIG_PHL_P2PPS
static bool
_up_noa_for_bt_req(struct phl_info_t *phl,
                           u16 bt_slot,
                           struct rtw_wifi_role_t *wrole,
                           struct rtw_wifi_role_link_t *rlink,
                           enum p2pps_trig_tag tag)
{
	bool ret = false;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl->phl_com);
	struct rtw_phl_noa param = {0};
	u32 tsf_h = 0, tsf_l = 0;
	u64 tsf = 0, start_tsf = 0;
	u16 offset = 0;

	if (!mr_ctl->mr_ops.phl_mr_update_noa) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "%s(): ops.phl_mr_update_noa == NULL\n",
			__func__);
		goto exit;
	}
	param.wrole = wrole;
	param.rlink = rlink;
	param.tag = tag;
	param.dur = bt_slot;
	if (param.dur == 0)
		goto _ops;
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_get_tsf(phl->hal,
	                                              rlink->hw_band,
	                                              rlink->hw_port,
	                                              &tsf_h,
	                                              &tsf_l)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "%s(): Get tsf fail, hw port(%d)\n",
			__func__, rlink->hw_port);
		goto exit;
	}
	tsf = tsf_h;
	tsf = tsf << 32;
	tsf |= tsf_l;

	if (!phl_calc_offset_from_tbtt(phl, wrole, rlink, tsf, &offset)) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_WARNING_, "%s(): Get offset fail\n",
			__func__);
		goto exit;
	}
	param.cnt = 255;
	param.interval = phl_role_get_bcn_intvl(phl, wrole, rlink);

	start_tsf = tsf - offset * TU + ((param.interval - param.dur) / 2) * TU;
	start_tsf = start_tsf + param.interval;/*Next beacon start Noa*/
	param.start_t_h = (u32)(start_tsf >> 32);
	param.start_t_l = (u32)start_tsf;
_ops:
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "%s(): NOA_start(0x%08x %08x), dur(%d), cnt(%d), interval(%d)\n",
		__func__, param.start_t_h, param.start_t_l, param.dur,
		param.cnt, param.interval);
	mr_ctl->mr_ops.phl_mr_update_noa(mr_ctl->mr_ops.priv, &param);
	ret = true;
exit:
	return ret;
}

/*
 * Return True: If concurrent mode is ap category x1 and client category x1
 */
static bool
_is_2g_scc_1ap_1sta(struct phl_info_t *phl,
			enum phl_band_idx band_idx, struct rtw_wifi_role_t **ap_wr)
{
	bool need = false;
	struct hw_band_ctl_t *band_ctrl = get_band_ctrl(phl, band_idx);
	struct mr_info *cur_info = &band_ctrl->cur_info;
	u8 ridx = 0, role_map = band_ctrl->role_map;
	struct rtw_wifi_role_t *wr = NULL;
	struct rtw_wifi_role_link_t *ap_rlink = NULL;
	u8 idx = 0;

	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: band_idx(%d), op_mode(%d), ap_num(%d), p2p_go_num(%d), ld_sta_num(%d)\n",
		__FUNCTION__, band_idx, band_ctrl->op_mode,
		cur_info->ap_num, cur_info->p2p_go_num,
		cur_info->ld_sta_num);
	if (band_ctrl->op_mode != MR_OP_SCC)
		goto exit;
	if (!(1 == cur_info->ap_num || 1 == cur_info->p2p_go_num))
		goto exit;
	if (cur_info->ld_sta_num == 0)
		goto exit;
	/*get ap role*/
	*ap_wr = NULL;
	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (!(role_map & BIT(ridx)))
			continue;
		wr = &(phl->phl_com->wifi_roles[ridx]);
		if (rtw_phl_role_is_ap_category(wr) &&
			wr->mstate == MLME_LINKED) {
			*ap_wr = wr;
			break;
		}
	}

	if (*ap_wr == NULL)
		goto exit;

	for (idx = 0; idx < (*ap_wr)->rlink_num; idx++) {
		ap_rlink = &((*ap_wr)->rlink[idx]);
		if (ap_rlink->chandef.band != BAND_ON_24G)
			goto exit;
	}
	need = true;
exit:
	return need;
}

/*
 * Handle MR coex mechanism for 2g_scc_1ap_1sta_btc
 * Specific concurrent mode : ap category x1, client category x1 + BTC
 * @handle: True: handle specific concurrent mode for all interfaces; False: Not handleand maybe handle it by other coex mechanism.
 */
static enum rtw_phl_status
_mr_coex_2g_scc_1ap_1sta_btc_handle(struct phl_info_t *phl,
		enum phl_band_idx band_idx, enum mr_coex_trigger trgger,
		enum mr_coex_mode *coex_mode)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct rtw_wifi_role_t *ap_wr = NULL;
	u16 bt_slot = 0;
	struct rtw_wifi_role_link_t *ap_rlink = NULL;

	if (!_is_2g_scc_1ap_1sta(phl, band_idx, &ap_wr)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: It's not 2g_scc_1ap_1sta\n",
			__FUNCTION__);
		goto exit;
	}
	bt_slot = (u16)rtw_hal_get_btc_req_slot(phl->hal, band_idx);
	if (trgger != MR_COEX_TRIG_BY_BT && bt_slot == 0)
		goto exit;
	ap_rlink = phl_get_rlink_by_hw_band(ap_wr, band_idx);
	if (_up_noa_for_bt_req(phl,
	                               bt_slot,
	                               ap_wr,
	                               ap_rlink,
	                               P2PPS_TRIG_2G_SCC_1AP_1STA_BT)) {
		if (bt_slot > 0)
			*coex_mode = MR_COEX_MODE_2GSCC_1AP_1STA_BTC;
		else
			*coex_mode = MR_COEX_MODE_NONE;
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: Up Noa ok\n",
			__FUNCTION__);
	} else {
		status = RTW_PHL_STATUS_FAILURE;
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: Up Noa fail\n",
			__FUNCTION__);
	}
exit:
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: coex_mode(%d), status(%d), trgger(%d), bt_slot(%d)\n",
		__FUNCTION__, *coex_mode, status, trgger, bt_slot);
	return status;
}

/*
 * Disable MR coex mechanism of 2g_scc_1ap_1sta_btc
 */
enum rtw_phl_status
_mr_coex_2g_scc_1ap_1sta_btc_disable(struct phl_info_t *phl,
			enum phl_band_idx band_idx)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct rtw_wifi_role_t *ap_wr = NULL;
	struct rtw_wifi_role_link_t *ap_rlink = NULL;

	if (!_is_2g_scc_1ap_1sta(phl, band_idx, &ap_wr)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: It is not 2g_scc_1ap_1sta\n",
			__FUNCTION__);
		goto exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: band_idx(%d)\n",
		__FUNCTION__, band_idx);

	ap_rlink = phl_get_rlink_by_hw_band(ap_wr, band_idx);
	if (_up_noa_for_bt_req(phl,
	                               0,
	                               ap_wr,
	                               ap_rlink,
	                               P2PPS_TRIG_2G_SCC_1AP_1STA_BT)) {
		status = RTW_PHL_STATUS_SUCCESS;
	} else {
		status = RTW_PHL_STATUS_FAILURE;
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: Up Noa fail\n",
			__FUNCTION__);
	}
exit:
	return status;
}

static void _noa_desc_to_mcc_limit_req_info(struct rtw_phl_noa_desc *noa_desc,
	struct phl_mcc_dur_lim_req_info *limit_req_info)
{
	if (noa_desc->enable && (noa_desc->tag != P2PPS_TRIG_MCC)) {
		/* limited by NoA */
		limit_req_info->tag = RTW_MCC_DUR_LIM_NOA;
		limit_req_info->enable = true;
		limit_req_info->start_t_h = noa_desc->start_t_h;
		limit_req_info->start_t_l = noa_desc->start_t_l;
		limit_req_info->dur = noa_desc->duration;
		limit_req_info->intvl = noa_desc->interval;
	} else {
		/* No limit for NoA disable */
		limit_req_info->tag = RTW_MCC_DUR_LIM_NONE;
		limit_req_info->enable = false;
	}
}

/* call by noa module for noa enable/disable */
bool phl_mr_coex_noa_dur_lim_change(struct phl_info_t *phl_info,
                               struct rtw_wifi_role_t *wrole,
                               struct rtw_wifi_role_link_t *rlink,
                               struct rtw_phl_noa_desc *noa_desc)
{
	u8 tdmra_inprogress = false, ctrl_by_tdmra = false, need_tdmra = false;
	struct phl_mcc_dur_lim_req_info lim_req = {0};
	u8 ap_role_idx;

	tdmra_inprogress = rtw_phl_mcc_inprogress(phl_info, rlink->hw_band);

	/* tdmra inprogress */
	if (tdmra_inprogress) {
		ctrl_by_tdmra = true;
		/* under MCC, control by MCC module */
		_noa_desc_to_mcc_limit_req_info(noa_desc, &lim_req);
		rtw_phl_mcc_dur_lim_change(phl_info, wrole, &lim_req);
	} else {
		/*
			tdmra not inprogress,
			but will process tdmra if need_tdmra = true
			then, tdmra will qurey NoA parameter
		*/
		need_tdmra = _tdmra_need(phl_info, rlink->hw_band, &ap_role_idx);
		if (need_tdmra)
			ctrl_by_tdmra = true;
		else
			ctrl_by_tdmra = false;
	}

	return ctrl_by_tdmra;
}
#endif /* CONFIG_PHL_P2PPS */


enum rtw_phl_status
phl_mr_coex_query_role_time_slot_lim(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole,
				struct rtw_wifi_role_link_t *rlink,
				struct phl_mcc_dur_lim_req_info *limit_req_info)
{
#ifdef CONFIG_PHL_P2PPS
	struct rtw_phl_noa_desc noa_desc = {0};

	/* limited by NoA */
	phl_p2pps_query_noa_with_cnt255(phl_info, wrole, rlink, &noa_desc);
	_noa_desc_to_mcc_limit_req_info(&noa_desc, limit_req_info);
#else
	/* No limit for NoA disable */
	limit_req_info->tag = RTW_MCC_DUR_LIM_NONE;
	limit_req_info->enable = false;
#endif
	return RTW_PHL_STATUS_SUCCESS;
}

u8 rtw_phl_mr_coex_query_inprogress(void *phl,
			u8 hw_band,
			enum rtw_phl_mr_coex_chk_inprocess_type check_type)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct mr_coex_info *mrcx_i = get_mr_coex_i(phl, hw_band);
	struct hw_band_ctl_t *band_ctrl = get_band_ctrl(phl, hw_band);
	int chanctx_num = 0;
	u8 ret = false;

	chanctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);

	switch (check_type) {
	case RTW_MR_COEX_CHK_INPROGRESS:
		if (mrcx_i->coex_mode != MR_COEX_MODE_NONE)
			ret = true;
		break;
	case RTW_MR_COEX_CHK_INPROGRESS_TDMRA:
		if (mrcx_i->coex_mode == MR_COEX_MODE_TDMRA)
			ret = true;
		break;
	case RTW_MR_COEX_CHK_INPROGRESS_TDMRA_SINGLE_CH:
		if (chanctx_num == 1 &&
			mrcx_i->coex_mode == MR_COEX_MODE_TDMRA)
			ret = true;
		break;
	case RTW_MR_COEX_CHK_INPROGRESS_TDMRA_MULTI_CH:
		if (chanctx_num > 1 &&
			mrcx_i->coex_mode == MR_COEX_MODE_TDMRA)
			ret = true;
		break;
	case RTW_MR_COEX_CHK_MAX:
		break;
	}
	return ret;
}

/*
 * Disable MR coex mechanism which is TDMRA or 2g_scc_1ap_1sta_btc mechanism
 */
static enum rtw_phl_status
_mr_coex_disable(struct phl_info_t *phl,
		struct rtw_wifi_role_t *cur_wrole, enum phl_band_idx band_idx,
		enum mr_coex_trigger trgger)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct mr_coex_info *mrcx_i = get_mr_coex_i(phl, band_idx);

	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: band_idx(%d), trgger(%d)\n",
		__FUNCTION__, band_idx, trgger);
	if (trgger == MR_COEX_TRIG_BY_LINKING ||
	    trgger == MR_COEX_TRIG_BY_DIS_LINKING ||
	    trgger == MR_COEX_TRIG_BY_SCAN ||
	    trgger == MR_COEX_TRIG_BY_ECSA) {
		set_prohibit(phl, band_idx, true, trgger);
	}
#ifdef CONFIG_PHL_P2PPS
	if (mrcx_i->coex_mode == MR_COEX_MODE_2GSCC_1AP_1STA_BTC) {
		status = _mr_coex_2g_scc_1ap_1sta_btc_disable(phl, band_idx);
		if (status == RTW_PHL_STATUS_SUCCESS)
			mrcx_i->coex_mode = MR_COEX_MODE_NONE;
	} else
#endif /* CONFIG_PHL_P2PPS */
#ifdef CONFIG_MCC_SUPPORT
	if (mrcx_i->coex_mode == MR_COEX_MODE_TDMRA) {
		status = _tdmra_disable(phl, cur_wrole, band_idx);
		if (status == RTW_PHL_STATUS_SUCCESS)
			mrcx_i->coex_mode = MR_COEX_MODE_NONE;
	} else
#endif /* CONFIG_MCC_SUPPORT */
	{
		status = RTW_PHL_STATUS_SUCCESS;
	}
	if (trgger == MR_COEX_TRIG_BY_SCAN) {
		phl_mr_stop_all_beacon(phl, cur_wrole, band_idx, true);
	}
	if (status != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_ERR_, "%s: Handle by %d fail\n",
			__FUNCTION__, mrcx_i->coex_mode);
	}
	return status;
}



/*
 * MR will excute suitable coex mechanism for Multi-interface
 * @cur_wrole: current role
 * @slot: time slot, Interpretation by trgger event.
 * Ignore it, if trgger event is MR_COEX_TRIG_BY_LINKING/MR_COEX_TRIG_BY_DIS_LINKING/
 * MR_COEX_TRIG_BY_SCAN/MR_COEX_TRIG_BY_ECSA.
 * @band_idx: hw band
 * @trgger: Trigger event
 */
static enum rtw_phl_status
_mr_coex_handle(struct phl_info_t *phl, struct rtw_wifi_role_t *cur_wrole,
	u16 slot, enum phl_band_idx band_idx, enum mr_coex_trigger trgger)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct hw_band_ctl_t *band_ctrl = get_band_ctrl(phl, band_idx);
	struct mr_coex_info *mrcx_i = get_mr_coex_i(phl, band_idx);
	enum mr_coex_mode pre_mode = mrcx_i->coex_mode;
	int ctx_num = phl_mr_get_chanctx_num(phl, band_ctrl);

	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: band_idx(%d), trgger(%d), slot(%d)\n",
		__FUNCTION__, band_idx, trgger, slot);

	mrcx_i->coex_trig = trgger;
	if (trgger == MR_COEX_TRIG_BY_LINKING ||
	    trgger == MR_COEX_TRIG_BY_DIS_LINKING ||
	    trgger == MR_COEX_TRIG_BY_SCAN ||
	    trgger == MR_COEX_TRIG_BY_ECSA) {
		set_prohibit(phl, band_idx, false, trgger);
	}
#ifdef CONFIG_PHL_P2PPS
	status = _mr_coex_2g_scc_1ap_1sta_btc_handle(phl, band_idx, trgger,
							&mrcx_i->coex_mode);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_ERR_, "%s: Handle 2g_scc_1ap_1sta_btc fail\n",
			__FUNCTION__);
		goto exit;
	}
#endif /* CONFIG_PHL_P2PPS */
	if (mrcx_i->coex_mode != MR_COEX_MODE_2GSCC_1AP_1STA_BTC) {
		status = _tdmra_handle(phl, cur_wrole, slot, band_idx,
						trgger, &mrcx_i->coex_mode);
		if (status != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_ERR_, "%s: Handle TDMRA fail\n",
			__FUNCTION__);
			goto exit;
		}
	}
	/* we shll check whether that we need to enable ECSA
	     After enable TDMRA for 2 chctx concurrent mode */
	if ((ctx_num == 2) && (pre_mode != MR_COEX_MODE_TDMRA)
		&& (mrcx_i->coex_mode == MR_COEX_MODE_TDMRA))
		phl_mr_check_ecsa(phl, band_idx);
	if (trgger == MR_COEX_TRIG_BY_SCAN) {
		phl_mr_stop_all_beacon(phl, cur_wrole, band_idx, false);
	}
exit:
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: status(%d), coex_mode %d\n",
			__FUNCTION__, status, mrcx_i->coex_mode);
	return status;
}



/* msg handle start */
static enum phl_mdl_ret_code
_mr_coex_pre_handle_connect_start(struct phl_info_t *phl, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->inbuf;
	rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl, role,
				rlink->hw_band, MR_COEX_TRIG_BY_LINKING)) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
				__FUNCTION__);
			goto _exit;
		}
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

/*msg handle end */
static enum phl_mdl_ret_code
_mr_coex_pre_handle_disconnect_prepare(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT &&
		MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl, role,
				rlink->hw_band, MR_COEX_TRIG_BY_DIS_LINKING)) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
				__FUNCTION__);
			goto _exit;
		}
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_pre_handle_disconnect_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if ((MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT) &&
		(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT)) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: BandIdx(%d)\n",
		__FUNCTION__, msg->band_idx);
#ifdef CONFIG_DBCC_SUPPORT
	/* Be going to disable DBCC */
	if (phl_mr_is_trigger_dbcc(phl)) {
		enum phl_band_idx oth_band = (msg->band_idx == HW_BAND_0)
						? HW_BAND_1 : HW_BAND_0;
		struct rtw_wifi_role_link_t *rlink = NULL;

		/* Disable MCC of the other band */
		rlink = phl_mr_get_first_rlink_by_band(phl, oth_band);
		if (rlink != NULL) {
			if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl,
						rlink->wrole, oth_band,
						MR_COEX_TRIG_BY_DIS_DBCC)) {
				PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_,
					"%s: MR coex disable fail\n",
					__FUNCTION__);
				goto _exit;
			}
		}
	}
#endif /* CONFIG_DBCC_SUPPORT */
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_pre_handle_ap_start_prepare(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct phl_module_op_info op_info = {0};
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	op_info.op_code = FG_REQ_OP_GET_ROLE;
	if (phl_disp_eng_query_cur_cmd_info(phl, msg->band_idx, &op_info)
						!= RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "Query wifi role fail!\n");
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)op_info.outbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl, role,
				rlink->hw_band, MR_COEX_TRIG_BY_LINKING)) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
				__FUNCTION__);
			goto _exit;
		}
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_pre_handle_ap_stop_prepare(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl, role,
				rlink->hw_band, MR_COEX_TRIG_BY_DIS_LINKING)) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
				__FUNCTION__);
			goto _exit;
		}
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_pre_handle_ap_stop_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: BandIdx(%d)\n",
		__FUNCTION__, msg->band_idx);
#ifdef CONFIG_DBCC_SUPPORT
	/* Be going to disable DBCC */
	if (phl_mr_is_trigger_dbcc(phl)) {
		enum phl_band_idx oth_band = (msg->band_idx == HW_BAND_0)
						? HW_BAND_1 : HW_BAND_0;
		struct rtw_wifi_role_link_t *rlink = NULL;

		/* Disable MCC of the other band */
		rlink = phl_mr_get_first_rlink_by_band(phl, oth_band);
		if (rlink != NULL) {
			if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl,
						rlink->wrole, oth_band,
						MR_COEX_TRIG_BY_DIS_DBCC)) {
				PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_,
					"%s: MR coex disable fail\n",
					__FUNCTION__);
				goto _exit;
			}
		}
	}
#endif /* CONFIG_DBCC_SUPPORT */
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}


static enum phl_mdl_ret_code
_mr_coex_pre_handle_chg_op_ch_def_start(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	u8 *cmd = NULL;
	u32 cmd_len;
	struct chg_opch_param *ch_param = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_GENERAL) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	if (RTW_PHL_STATUS_SUCCESS != phl_cmd_get_cur_cmdinfo(phl,
				msg->band_idx, msg, &cmd, &cmd_len)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_ERR_, "%s: Fail to get cmd info \n",
			__FUNCTION__);
		goto _exit;
	}
	ch_param = (struct chg_opch_param *)cmd;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, ch_param->wrole->id);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl, ch_param->wrole,
						ch_param->rlink->hw_band,
						MR_COEX_TRIG_BY_CHG_OP_CHDEF)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
			__FUNCTION__);
		goto _exit;
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_pre_handle_scan_start(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_SCAN) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	/* use primary link to serve scan request for legacy/MLD */
	rlink = get_rlink(role, RTW_RLINK_PRIMARY);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl, role,
					rlink->hw_band, MR_COEX_TRIG_BY_SCAN)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
			__FUNCTION__);
		goto _exit;
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_pre_handle_ecsa_switch_start(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_ECSA) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[1].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__,  msg->band_idx, role->id);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl, role,
					rlink->hw_band, MR_COEX_TRIG_BY_ECSA)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
			__FUNCTION__);
		goto _exit;
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_pre_handle_associated_tsf_error(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 *cmd = NULL;
	u32 cmd_len = 0;

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
	sta = (struct rtw_phl_stainfo_t *)cmd;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d) RlinkId(%d)\n",
		__FUNCTION__,  msg->band_idx, sta->wrole->id, sta->rlink->id);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_disable(phl, sta->wrole,
					sta->rlink->hw_band, MR_COEX_TRIG_BY_ASSOC_TSF_EFFOR)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
			__FUNCTION__);
		goto exit;
	}
	ret = MDL_RET_SUCCESS;
exit:
	return ret;
}


static enum phl_mdl_ret_code
_mr_coex_post_handle_tsf_sync_done(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 *cmd = NULL;
	u32 cmd_len;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_GENERAL) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	if (phl_cmd_get_cur_cmdinfo(phl, msg->band_idx, msg, &cmd, &cmd_len)
						!= RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Fail to get cmd info \n",
			__func__);
		goto _exit;
	}
	sta = (struct rtw_phl_stainfo_t *)cmd;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, sta->wrole->id);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, sta->wrole, 0,
			sta->rlink->hw_band, MR_COEX_TRIG_BY_LINKING)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex handle fail\n",
			__FUNCTION__);
		goto _exit;
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;

}

static enum phl_mdl_ret_code
_mr_coex_post_handle_disconnect_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if ((MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT) &&
		(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT)) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->inbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__,  msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, role, 0,
				rlink->hw_band, MR_COEX_TRIG_BY_DIS_LINKING)) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_,
				"%s: MR coex disable fail\n",
				__FUNCTION__);
			goto _exit;
		}
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}


static enum phl_mdl_ret_code
_mr_coex_post_handle_ap_start_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, role, 0,
				rlink->hw_band, MR_COEX_TRIG_BY_LINKING)) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex handle fail\n",
				__FUNCTION__);
			goto _exit;
		}
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_post_handle_ap_stop_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if ((MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) &&
		(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START)) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->inbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, role, 0,
				rlink->hw_band, MR_COEX_TRIG_BY_DIS_LINKING)) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
				__FUNCTION__);
			goto _exit;
		}

	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_post_handle_chg_op_ch_def_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct chg_opch_param *ch_param = NULL;
	u8 *cmd = NULL;
	u32 cmd_len;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_GENERAL) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	if (phl_cmd_get_cur_cmdinfo(phl, msg->band_idx, msg, &cmd, &cmd_len)
						!= RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_ERR_, "%s: Fail to get cmd info \n",
			__FUNCTION__);
		goto _exit;
	}
	ch_param = (struct chg_opch_param *)cmd;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__,  msg->band_idx, ch_param->wrole->id);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, ch_param->wrole,
						0, ch_param->rlink->hw_band,
						MR_COEX_TRIG_BY_CHG_OP_CHDEF)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex handle fail\n\n",
			__FUNCTION__);
		goto _exit;
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_post_handle_scan_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_SCAN) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	/* use primary link to serve scan request for legacy/MLD */
	rlink = get_rlink(role, RTW_RLINK_PRIMARY);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, role,
						0, rlink->hw_band,
						MR_COEX_TRIG_BY_SCAN)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex handle fail\n\n",
			__FUNCTION__);
		goto _exit;
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_post_handle_ecsa_switch_done(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_ECSA) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	rlink = (struct rtw_wifi_role_link_t *)msg->rsvd[1].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, role,
						0, rlink->hw_band,
						MR_COEX_TRIG_BY_ECSA)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex handle fail\n\n",
			__FUNCTION__);
		goto _exit;
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_mr_coex_post_handle_btc_req_slot(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	u16 slot = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_BTC) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	slot = (u16)(*(u32 *)msg->inbuf);
	if (is_mrcx_prohibit(phl, msg->band_idx)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: BandIdx(%d), Slot(%d), skip it by proh_rsn(%d), FG is running, we will handle slot req after FG complete\n",
			__FUNCTION__, msg->band_idx, slot,
			get_proh_rsn(phl, msg->band_idx));
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: BandIdx(%d), Slot(%d)\n",
		__FUNCTION__, msg->band_idx, slot);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, NULL, slot,
					msg->band_idx, MR_COEX_TRIG_BY_BT)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex handle fail\n\n",
			__FUNCTION__);
		goto _exit;
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

#ifdef CONFIG_DBCC_SUPPORT
static enum phl_mdl_ret_code
_mr_coex_post_handle_dbcc_enable(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_link_t *rlink = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_MRC) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d)\n",
		__FUNCTION__, msg->band_idx);
	/* resume mechanism for band0 */
	rlink = phl_mr_get_rlink_stay_in_cur_chdef(phl, HW_BAND_0);
	if (rlink != NULL) {
		if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, rlink->wrole,
				0, HW_BAND_0, MR_COEX_TRIG_BY_EN_DBCC)) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_,
				"%s: MR coex disable fail\n",
				__FUNCTION__);
			goto _exit;
		}
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}
#endif /* CONFIG_DBCC_SUPPORT */

static enum phl_mdl_ret_code
_mr_coex_post_handle_associated_tsf_error(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 *cmd = NULL;
	u32 cmd_len = 0;

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
	sta = (struct rtw_phl_stainfo_t *)cmd;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d) RlinkId(%d)\n",
		__FUNCTION__,  msg->band_idx, sta->wrole->id, sta->rlink->id);
	if (RTW_PHL_STATUS_SUCCESS != _mr_coex_handle(phl, sta->wrole, 0,
		sta->rlink->hw_band, MR_COEX_TRIG_BY_ASSOC_TSF_EFFOR)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: MR coex disable fail\n",
			__FUNCTION__);
		goto exit;
	}
	ret = MDL_RET_SUCCESS;
exit:
	return ret;
}


static enum phl_mdl_ret_code
_mr_coex_msg_pre_hdlr(void *dispr, void *priv, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl = (struct phl_info_t *)priv;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_CONNECT_START:
		ret = _mr_coex_pre_handle_connect_start(phl, msg);
		break;
	case MSG_EVT_DISCONNECT_PREPARE:
		ret = _mr_coex_pre_handle_disconnect_prepare(phl, msg);
		break;
	case MSG_EVT_DISCONNECT_END:
		ret = _mr_coex_pre_handle_disconnect_end(phl, msg);
		break;
	case MSG_EVT_AP_START_PREPARE:
		ret = _mr_coex_pre_handle_ap_start_prepare(phl, msg);
		break;
	case MSG_EVT_AP_STOP_PREPARE:
		ret = _mr_coex_pre_handle_ap_stop_prepare(phl, msg);
		break;
	case MSG_EVT_AP_STOP_END:
		ret = _mr_coex_pre_handle_ap_stop_end(phl, msg);
		break;
	case MSG_EVT_CHG_OP_CH_DEF_START:
		ret = _mr_coex_pre_handle_chg_op_ch_def_start(phl, msg);
		break;
	case MSG_EVT_SCAN_START:
		ret = _mr_coex_pre_handle_scan_start(phl, msg);
		break;
	case MSG_EVT_ECSA_SWITCH_START:
		ret = _mr_coex_pre_handle_ecsa_switch_start(phl, msg);
		break;
	case MSG_EVT_ASSOCIATED_TSF_ERROR:
		ret = _mr_coex_pre_handle_associated_tsf_error(phl, msg);
		break;
	default:
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_,
			"%s: MDL(%d), EVT(%d), Not handle event in pre-phase\n",
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
_mr_coex_msg_post_hdl(void *dispr, void *priv, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl = (struct phl_info_t *)priv;

	switch(MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_TSF_SYNC_DONE: /* Sta shall wait tsf sync */
		ret = _mr_coex_post_handle_tsf_sync_done(phl, msg);
		break;
	case MSG_EVT_DISCONNECT_END:
		ret = _mr_coex_post_handle_disconnect_end(phl, msg);
		break;
	case MSG_EVT_AP_START_END:
		ret = _mr_coex_post_handle_ap_start_end(phl, msg);
		break;
	case MSG_EVT_AP_STOP_END:
		ret = _mr_coex_post_handle_ap_stop_end(phl, msg);
		break;
	case MSG_EVT_CHG_OP_CH_DEF_END:
		ret = _mr_coex_post_handle_chg_op_ch_def_end(phl, msg);
		break;
	case MSG_EVT_SCAN_END:
		ret = _mr_coex_post_handle_scan_end(phl, msg);
		break;
	case MSG_EVT_ECSA_SWITCH_DONE:
		ret = _mr_coex_post_handle_ecsa_switch_done(phl, msg);
		break;
	case MSG_EVT_BTC_REQ_BT_SLOT:
		ret = _mr_coex_post_handle_btc_req_slot(phl, msg);
		break;
#ifdef CONFIG_DBCC_SUPPORT
	case MSG_EVT_DBCC_ENABLE:
		ret = _mr_coex_post_handle_dbcc_enable(phl, msg);
		break;
#endif /* CONFIG_DBCC_SUPPORT */
	case MSG_EVT_ASSOCIATED_TSF_ERROR:
		ret = _mr_coex_post_handle_associated_tsf_error(phl, msg);
		break;
	default:
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: MDL(%d), EVT(%d), Not handle event in post-phase\n",
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


/* parasitic feature start */

/* move from _phl_mrc_module_connect_start_hdlr */
static enum phl_mdl_ret_code
_parasitic_pre_handle_connect_start(struct phl_info_t *phl, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->inbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
#ifdef CONFIG_PHL_P2PPS
		phl_p2pps_noa_all_role_pause(phl, rlink->hw_band);
#endif
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

/* move from _phl_mrc_module_disconnect_pre_hdlr*/
static enum phl_mdl_ret_code
_parasitic_pre_handle_disconnect_prepare(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT &&
		MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		phl_mr_check_ecsa_cancel(phl, role, rlink);
#ifdef CONFIG_PHL_P2PPS
		/* disable NoA for this role */
		phl_p2pps_noa_disable_all(phl, role, rlink);
		/* pasue buddy NoA */
		phl_p2pps_noa_all_role_pause(phl, rlink->hw_band);
#endif
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_parasitic_pre_handle_disconnect_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if ((MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT) &&
		(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT)) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d)\n",
		__FUNCTION__, msg->band_idx);
#ifdef CONFIG_DBCC_SUPPORT
	/* Be going to disable DBCC */
	if (phl_mr_is_trigger_dbcc(phl)) {
		enum phl_band_idx oth_band = (msg->band_idx == HW_BAND_0)
						? HW_BAND_1 : HW_BAND_0;

		/* Disable all mechanism of the other band */
		phl_p2pps_noa_all_role_pause(phl, oth_band);
	}
#endif /* CONFIG_DBCC_SUPPORT */
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

/* move from _phl_mrc_module_ap_start_pre_hdlr */
static enum phl_mdl_ret_code
_parasitic_pre_handle_ap_start_prepare(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct phl_module_op_info op_info = {0};
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	op_info.op_code = FG_REQ_OP_GET_ROLE;
	if (phl_disp_eng_query_cur_cmd_info(phl, msg->band_idx, &op_info)
						!= RTW_PHL_STATUS_SUCCESS) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "Query wifi role fail!\n");
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)op_info.outbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		#ifdef CONFIG_PHL_P2PPS
		/* pasue all NoA */
		phl_p2pps_noa_all_role_pause(phl, rlink->hw_band);
		#endif
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

/* move from _phl_mrc_module_ap_stop_pre_hdlr */
static enum phl_mdl_ret_code
_parasitic_pre_handle_ap_stop_prepare(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		phl_mr_check_ecsa_cancel(phl, role, rlink);
		#ifdef CONFIG_PHL_P2PPS
		/* disable NoA for this role */
		/* shall refine for MLD */
		phl_p2pps_noa_disable_all(phl, role, rlink);
		/* pasue buddy NoA */
		phl_p2pps_noa_all_role_pause(phl, rlink->hw_band);
		#endif
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_parasitic_pre_handle_ap_stop_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d)\n",
		__FUNCTION__, msg->band_idx);
#ifdef CONFIG_DBCC_SUPPORT
	/* Be going to disable DBCC */
	if (phl_mr_is_trigger_dbcc(phl)) {
		enum phl_band_idx oth_band = (msg->band_idx == HW_BAND_0)
						? HW_BAND_1 : HW_BAND_0;

		/* Disable all mechanism of the other band */
		phl_p2pps_noa_all_role_pause(phl, oth_band);
	}
#endif /* CONFIG_DBCC_SUPPORT */
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}


static enum phl_mdl_ret_code
_parasitic_pre_handle_scan_start(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_SCAN) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	/* use primary link to serve scan request for legacy/MLD */
	rlink = get_rlink(role, RTW_RLINK_PRIMARY);
	phl_p2pps_noa_pause_all(phl, role);
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

static enum phl_mdl_ret_code
_parasitic_pre_handle_associated_tsf_error(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 *cmd = NULL;
	u32 cmd_len = 0;

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
	sta = (struct rtw_phl_stainfo_t *)cmd;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d) RlinkId(%d)\n",
		__FUNCTION__,  msg->band_idx, sta->wrole->id, sta->rlink->id);
#ifdef CONFIG_PHL_P2PPS
	phl_p2pps_noa_pause_all(phl, sta->wrole);
#endif /* CONFIG_PHL_P2PPS */
	ret = MDL_RET_SUCCESS;
exit:
	return ret;
}


static enum phl_mdl_ret_code
_parasitic_post_handle_connect_done(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;
	struct phl_module_op_info op_info = {0};

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	op_info.op_code = FG_REQ_OP_GET_ROLE;
	if(phl_disp_eng_query_cur_cmd_info(phl, msg->band_idx, &op_info)
		!= RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "Query wifi role fail!\n");
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)op_info.outbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	rlink = (struct rtw_wifi_role_link_t *)op_info.outbuf;
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		#ifdef CONFIG_PHL_P2PPS
		/* Process NOA */
		phl_p2pps_noa_all_role_resume(phl, rlink->hw_band);
		#endif
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;

}

/* move from rtw_phl_disconnected_resume_hdlr*/
static enum phl_mdl_ret_code
_parasitic_post_handle_disconnect_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if ((MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_DISCONNECT) &&
		(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_CONNECT)) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->inbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		#ifdef CONFIG_PHL_P2PPS
		/* resume buddy NoA */
		phl_p2pps_noa_all_role_resume(phl, rlink->hw_band);
		#endif
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

/* move from _phl_mrc_module_ap_started_hdlr -> phl_mr_state_upt*/
static enum phl_mdl_ret_code
_parasitic_post_handle_ap_start_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		#ifdef CONFIG_PHL_P2PPS
		/* Process NOA */
		phl_p2pps_noa_all_role_resume(phl, rlink->hw_band);
		#endif
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

/* move from rtw_phl_ap_stop_resume_hdlr */
static enum phl_mdl_ret_code
_parasitic_post_handle_ap_stop_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	if ((MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_STOP) &&
		(MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_AP_START)) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->inbuf;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	for (idx = 0; idx < role->rlink_num; idx++) {
		rlink = get_rlink(role, idx);
		#ifdef CONFIG_PHL_P2PPS
		/* resume buddy NoA */
		phl_p2pps_noa_all_role_resume(phl, rlink->hw_band);
		#endif
	}
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

/* mover from _mrc_module_msg_post_hdl of MSG_EVT_SCAN_END*/
static enum phl_mdl_ret_code
_parasitic_post_handle_scan_end(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_wifi_role_t *role = NULL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_FG_MDL_SCAN) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	role = (struct rtw_wifi_role_t *)msg->rsvd[0].ptr;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d)\n",
		__FUNCTION__, msg->band_idx, role->id);
	if (role == NULL) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_, "%s: role is NULL\n",
			__FUNCTION__);
		goto _exit;
	}
	phl_p2pps_noa_resume_all(phl, role);
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}

#ifdef CONFIG_DBCC_SUPPORT
static enum phl_mdl_ret_code
_parasitic_post_handle_dbcc_enable(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if (MSG_MDL_ID_FIELD(msg->msg_id) != PHL_MDL_MRC) {
		ret = MDL_RET_IGNORE;
		goto _exit;
	}
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d)\n",
		__FUNCTION__, msg->band_idx);
	/* resume mechanism for band0 */
	phl_p2pps_noa_all_role_resume(phl, HW_BAND_0);
	ret = MDL_RET_SUCCESS;
_exit:
	return ret;
}
#endif /* CONFIG_DBCC_SUPPORT */

static enum phl_mdl_ret_code
_parasitic_post_handle_associated_tsf_error(struct phl_info_t *phl,
					struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 *cmd = NULL;
	u32 cmd_len = 0;

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
	sta = (struct rtw_phl_stainfo_t *)cmd;
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: Msg_band(%d), Rid(%d) RlinkId(%d)\n",
		__FUNCTION__,  msg->band_idx, sta->wrole->id, sta->rlink->id);
#ifdef CONFIG_PHL_P2PPS
	phl_p2pps_noa_resume_all(phl, sta->wrole);
#endif /* CONFIG_PHL_P2PPS */
	ret = MDL_RET_SUCCESS;
exit:
	return ret;
}


static enum phl_mdl_ret_code
_parasitic_msg_pre_hdlr(void *dispr, void *priv, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl = (struct phl_info_t *)priv;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_CONNECT_START:
		ret = _parasitic_pre_handle_connect_start(phl, msg);
		break;
	case MSG_EVT_DISCONNECT_PREPARE:
		ret = _parasitic_pre_handle_disconnect_prepare(phl, msg);
		break;
	case MSG_EVT_DISCONNECT_END:
		ret = _parasitic_pre_handle_disconnect_end(phl, msg);
		break;
	case MSG_EVT_AP_START_PREPARE:
		ret = _parasitic_pre_handle_ap_start_prepare(phl, msg);
		break;
	case MSG_EVT_AP_STOP_PREPARE:
		ret = _parasitic_pre_handle_ap_stop_prepare(phl, msg);
		break;
	case MSG_EVT_AP_STOP_END:
		ret = _parasitic_pre_handle_ap_stop_end(phl, msg);
		break;
	case MSG_EVT_SCAN_START:
		ret = _parasitic_pre_handle_scan_start(phl, msg);
		break;
	case MSG_EVT_ASSOCIATED_TSF_ERROR:
		ret = _parasitic_pre_handle_associated_tsf_error(phl, msg);
		break;
	default:
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: MDL(%d), EVT(%d), Not handle event in pre-phase\n",
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
_parasitic_msg_post_hdl(void *dispr, void *priv, struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl= (struct phl_info_t *)priv;

	switch(MSG_EVT_ID_FIELD(msg->msg_id)) {
	case MSG_EVT_CONNECT_END:
		ret = _parasitic_post_handle_connect_done(phl, msg);
		break;
	case MSG_EVT_DISCONNECT_END:
		ret = _parasitic_post_handle_disconnect_end(phl, msg);
		break;
	case MSG_EVT_AP_START_END:
		ret = _parasitic_post_handle_ap_start_end(phl, msg);
		break;
	case MSG_EVT_AP_STOP_END:
		ret = _parasitic_post_handle_ap_stop_end(phl, msg);
		break;
	case MSG_EVT_SCAN_END:
		ret = _parasitic_post_handle_scan_end(phl, msg);
		break;
#ifdef CONFIG_DBCC_SUPPORT
	case MSG_EVT_DBCC_ENABLE:
		ret = _parasitic_post_handle_dbcc_enable(phl, msg);
		break;
#endif /* CONFIG_DBCC_SUPPORT */
	case MSG_EVT_ASSOCIATED_TSF_ERROR:
		ret = _parasitic_post_handle_associated_tsf_error(phl, msg);
		break;
	default:
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_INFO_, "%s: MDL(%d), EVT(%d), Not handle event in post-phase\n",
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




/* parasitic feature end */



static enum phl_mdl_ret_code
_phl_mr_coex_module_init(void *phl, void *dispr, void **priv)
{
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_DEBUG_,"%s \n", __FUNCTION__);
	*priv = phl;
	return MDL_RET_SUCCESS;
}

static void
_phl_mr_coex_module_deinit(void *dispr, void *priv)
{
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_DEBUG_,"%s \n", __FUNCTION__);
}

static enum phl_mdl_ret_code
_phl_mr_coex_module_start(void *dispr, void *priv)
{
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_DEBUG_,"%s \n", __FUNCTION__);
	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_phl_mr_coex_module_stop(void *dispr, void *priv)
{
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_DEBUG_,"%s \n", __FUNCTION__);;
	return MDL_RET_SUCCESS;
}


static enum phl_mdl_ret_code
_phl_mr_coex_module_set_info(void *dispr, void *priv,
				struct phl_module_op_info *info)
{
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_DEBUG_,"%s \n", __FUNCTION__);;

	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_phl_mr_coex_module_query_info(void *dispr, void *priv,
				struct phl_module_op_info *info)
{
	PHL_TRACE(COMP_PHL_MR_COEX, _PHL_DEBUG_,"%s \n", __FUNCTION__);;
	return MDL_RET_SUCCESS;
}

static enum phl_mdl_ret_code
_phl_mr_coex_module_msg_hdlr(void *dispr, void *priv,
				struct phl_msg *msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if (IS_MSG_FAIL(msg->msg_id)) {
		PHL_TRACE(COMP_PHL_MR_COEX, _PHL_WARNING_,
			  "%s: cmd dispatcher notify cmd failure: 0x%x.\n",
			   __FUNCTION__, msg->msg_id);
		return MDL_RET_FAIL;
	}
	if (IS_MSG_IN_PRE_PHASE(msg->msg_id)) {
		ret = _mr_coex_msg_pre_hdlr(dispr, priv, msg);
		if (ret != MDL_RET_FAIL)
			ret = _parasitic_msg_pre_hdlr(dispr, priv, msg);
	} else {
		ret = _parasitic_msg_post_hdl(dispr, priv, msg);
		if (ret != MDL_RET_FAIL)
			ret = _mr_coex_msg_post_hdl(dispr, priv, msg);
	}
	return ret;
}

enum rtw_phl_status
phl_register_mr_coex_module(struct phl_info_t *phl)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct phl_cmd_dispatch_engine *disp_eng = &(phl->disp_eng);
	struct phl_bk_module_ops bk_ops;
	u8 i = 0;

	bk_ops.init = _phl_mr_coex_module_init;
	bk_ops.deinit = _phl_mr_coex_module_deinit;
	bk_ops.start = _phl_mr_coex_module_start;
	bk_ops.stop = _phl_mr_coex_module_stop;
	bk_ops.msg_hdlr = _phl_mr_coex_module_msg_hdlr;
	bk_ops.set_info = _phl_mr_coex_module_set_info;
	bk_ops.query_info = _phl_mr_coex_module_query_info;

	for (i = 0; i < disp_eng->phy_num; i++) {
		sts = phl_disp_eng_register_module(phl, i, PHL_MDL_MR_COEX,
						&bk_ops);
		if (RTW_PHL_STATUS_SUCCESS != sts) {
			PHL_TRACE(COMP_PHL_MR_COEX, _PHL_ERR_,
				"%s register PHL_MDL_MR_COEX in cmd disp failed :%d\n",
				__FUNCTION__, i);
			break;
		}
	}
	return sts;
}

#endif /* CONFIG_MR_COEX_SUPPORT  */
