/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#define _PHL_P2PPS_C_
#include "phl_headers.h"
#ifdef RTW_WKARD_P2PPS_REFINE
#ifdef CONFIG_PHL_P2PPS
enum rtw_phl_status phl_p2pps_init(struct phl_info_t *phl)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_com_t *phl_com = phl->phl_com;
	struct rtw_phl_p2pps_info *info;

	info = (struct rtw_phl_p2pps_info *)_os_mem_alloc(phl_to_drvpriv(phl),
		sizeof(*info));
	if (info == NULL)
		return RTW_PHL_STATUS_RESOURCE;
	_os_mem_set(phl_to_drvpriv(phl),
			info, 0, sizeof(*info));
	phl_com->p2pps_info = (void*)info;
	info->phl_info = phl;
	_os_spinlock_init(phl_to_drvpriv(phl), &info->p2pps_lock);
	return status;
}

void phl_p2pps_deinit(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_phl_p2pps_info *info ;
	info = (struct rtw_phl_p2pps_info *)phl_com->p2pps_info;
	if (info) {
		_os_spinlock_free(phl_to_drvpriv(phl_info), &info->p2pps_lock);
		_os_mem_free(phl_to_drvpriv(phl_info), info, sizeof(*info));
	}
	phl_com->p2pps_info = NULL;
}

void
_phl_p2pps_dump_single_noa_desc(struct rtw_phl_noa_desc *desc)
{
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():enable = %d\n",
		desc->enable);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():start_t_h = 0x%x\n",
		desc->start_t_h);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():start_t_l = 0x%x\n",
		desc->start_t_l);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():interval = %d\n",
		desc->interval);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():duration = %d\n",
		desc->duration);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():count = %d\n",
		desc->count);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():noa_id = %d\n",
		desc->noa_id);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():tag = %d\n",
		desc->tag);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():w_role = 0x%p\n",
		desc->w_role);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_single_noa_desc():rlink = 0x%p\n",
		desc->rlink);
}

void
_phl_p2pps_dump_noa_table(struct rtw_phl_p2pps_info *psinfo,
	struct rtw_phl_noa_info *info)
{
	void *drvpriv = phlcom_to_drvpriv(psinfo->phl_info->phl_com);
	struct rtw_phl_noa_desc *desc = NULL;
	u8 i = 0;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():====>\n");
	_os_spinlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA] info.en_desc_num = %d, pause = %d\n",
		info->en_desc_num, info->paused);
	for (i = 0; i < MAX_NOA_DESC; i++) {
		desc = &info->noa_desc[i];
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]================DESC[%d]==================\n",
			i);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():enable = %d\n",
			desc->enable);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():start_t_h = 0x%x\n",
			desc->start_t_h);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():start_t_l = 0x%x\n",
			desc->start_t_l);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():interval = %d\n",
			desc->interval);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():duration = %d\n",
			desc->duration);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():count = %d\n",
			desc->count);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():noa_id = %d\n",
			desc->noa_id);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():tag = %d\n",
			desc->tag);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():w_role = 0x%p\n",
			desc->w_role);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_dump_noa_table():rlink = 0x%p\n",
			desc->rlink);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]================DESC[%d]==================\n",
			i);
	}
	_os_spinunlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
}

struct rtw_phl_noa_info *
_phl_p2pps_get_noa_info_by_role(struct rtw_phl_p2pps_info *psinfo,
	struct rtw_wifi_role_t *wrole)
{
	u8 idx = get_role_idx(wrole);
	return &psinfo->noa_info[idx];
}

struct rtw_phl_noa_desc *
_phl_p2pps_get_first_noa_desc_with_cnt255(struct phl_info_t *phl,
	struct rtw_phl_noa_info *info)
{
	u8 i = 0;

	struct rtw_phl_noa_desc *tmp_desc;
	for (i = 0; i < MAX_NOA_DESC; i++) {
		tmp_desc = &info->noa_desc[i];
		if(tmp_desc->count == 255 && tmp_desc->enable) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_get_first_noa_desc_with_cnt255():get desc, tag = %d!!\n",
				tmp_desc->tag);
			return tmp_desc;
		}
	}
	return NULL;
}

#ifdef RTW_WKARD_P2PPS_SINGLE_NOA

u8
_phl_p2pps_query_mcc_inprog_wkard(struct phl_info_t *phl_info,
	struct rtw_wifi_role_t *w_role)
{
	u8 ret = false;
#ifdef CONFIG_MR_COEX_SUPPORT
	ret = rtw_phl_mr_coex_query_inprogress(phl_info,
					get_rlink(w_role, RTW_RLINK_PRIMARY)->hw_band,
					RTW_MR_COEX_CHK_INPROGRESS_TDMRA);
#endif /* CONFIG_MR_COEX_SUPPORT */
	return ret;
}

struct rtw_wifi_role_t *
_phl_get_role_by_band_port(struct phl_info_t* phl_info,
                           u8 hw_band,
                           u8 hw_port)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[hw_band]);
	struct rtw_wifi_role_t *wrole = NULL;
	u8 ridx = 0;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (!(band_ctrl->role_map & BIT(ridx)))
			continue;
		wrole = phl_get_wrole_by_ridx(phl_info, ridx);
		if (wrole == NULL)
			continue;

		if (wrole->rlink_num == 1) {
			if (wrole->rlink[wrole->rlink_num-1].hw_band == hw_band &&
			    wrole->rlink[wrole->rlink_num-1].hw_port == hw_port) {
				PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_get_role_by_band_port():role_id(%d) hw_band = %d, hw_port = %d\n",
				                                     ridx,
				                                     wrole->rlink[wrole->rlink_num-1].hw_band,
				                                     wrole->rlink[wrole->rlink_num-1].hw_port);

				return wrole;
			}
		}
	}
	return NULL;
}

void
_phl_p2pps_calc_next_noa_s_time(struct phl_info_t *phl_info,
	struct rtw_wifi_role_t *w_role,
	struct rtw_phl_tsf32_tog_rpt *rpt,
	struct rtw_phl_noa_desc *orig_desc,
	struct rtw_phl_noa_desc *new_desc)
{
	void *d = phl_to_drvpriv(phl_info);
	u64 new_st = 0, old_st = 0;
	u64 tog_t = 0, delta_t = 0, intv_cnt = 0;

	_os_mem_cpy(d, new_desc, orig_desc, sizeof(*orig_desc));
	old_st = (((u64)orig_desc->start_t_h << 32) | orig_desc->start_t_l);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_calc_next_noa_s_time():old_st: 0x%08x %08x\n",
		(u32)(old_st >> 32), (u32)old_st);
	tog_t = (((u64)rpt->tsf_h << 32) | rpt->tsf_l);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_calc_next_noa_s_time():tog_t = 0x%08x %08x\n",
		(u32)(tog_t >> 32), (u32)tog_t);
	delta_t = tog_t - old_st;
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_calc_next_noa_s_time():delta_t = 0x%08x %08x\n",
		(u32)(delta_t >> 32), (u32)delta_t);
	intv_cnt = _os_division64(delta_t,  new_desc->interval) + 1;
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_calc_next_noa_s_time():intv_cnt = 0x%08x %08x\n",
		(u32)(intv_cnt >> 32), (u32)intv_cnt);
	new_st = old_st + (intv_cnt * new_desc->interval);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_calc_next_noa_s_time():new_st = 0x%08x %08x\n",
		(u32)(new_st >> 32), (u32)new_st);
	new_desc->start_t_h = new_st >> 32;
	new_desc->start_t_l = new_st & 0xFFFFFFFF;
}

void _phl_p2pps_ap_on_tsf32_tog(struct phl_info_t* phl_info,
	struct rtw_wifi_role_t *wrole,
	struct rtw_phl_tsf32_tog_rpt *rpt)
{
	struct rtw_phl_p2pps_info *psinfo = phl_to_p2pps_info(phl_info);
	struct rtw_phl_noa_info *info = NULL;
	struct rtw_phl_noa_desc *orig_desc = NULL;
	struct rtw_phl_noa_desc new_desc = {0};
	void *d = phl_to_drvpriv(phl_info);

	info = _phl_p2pps_get_noa_info_by_role(psinfo, wrole);
	orig_desc = _phl_p2pps_get_first_noa_desc_with_cnt255(phl_info, info);
	if (orig_desc) {
		_phl_p2pps_calc_next_noa_s_time(phl_info, wrole, rpt,
						orig_desc, &new_desc);
		_os_mem_cpy(d, orig_desc, &new_desc, sizeof(new_desc));
		_phl_p2pps_dump_single_noa_desc(&new_desc);
		if(psinfo->ops.tsf32_tog_update_single_noa)
			psinfo->ops.tsf32_tog_update_single_noa(d, wrole, &new_desc);
	} else {
		return;
	}
}
#endif

void phl_p2pps_tsf32_tog_handler(struct phl_info_t* phl_info)
{
	void *hal = phl_info->hal;
	struct rtw_phl_tsf32_tog_rpt rpt = {0};
	struct rtw_wifi_role_t *wrole = NULL;
	enum rtw_hal_status h_stat;

	h_stat = rtw_hal_get_tsf32_tog_rpt(hal, &rpt);
	if (h_stat != RTW_HAL_STATUS_SUCCESS)
		return;
	if (!rpt.valid) {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_WARNING_, "[NOA]phl_p2pps_tsf32_tog_handler():report not valid!!\n");
		return;
	}
	wrole = _phl_get_role_by_band_port(phl_info, rpt.band, rpt.port);
	if (wrole) {
		if (wrole->type == PHL_RTYPE_AP ||
			wrole->type == PHL_RTYPE_P2P_GO) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_tsf32_tog_handler():role(%d) is AP/GO mode, handle noa update\n",
				wrole->id);
#ifdef RTW_WKARD_P2PPS_SINGLE_NOA
			_phl_p2pps_ap_on_tsf32_tog(phl_info, wrole, &rpt);
#endif
		} else if (wrole->type == PHL_RTYPE_STATION ||
				wrole->type == PHL_RTYPE_P2P_GC) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_tsf32_tog_handler():role(%d) is STA/GO mode, currently do nothing\n",
				wrole->id);
			/*Call NoA disable all?*/
		}
	} else {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_WARNING_, "[NOA]phl_p2pps_tsf32_tog_handler():NULL ROLE!!, hwband = %d, hwport = %d\n",
			rpt.band, rpt.port);
	}
}

void
_phl_p2pps_copy_noa_desc(struct rtw_phl_p2pps_info *psinfo,
	struct rtw_phl_noa_desc *dest,
	struct rtw_phl_noa_desc *src)
{
	void *drvpriv = phlcom_to_drvpriv(psinfo->phl_info->phl_com);

	_os_spinlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	_os_mem_cpy(drvpriv, dest, src, sizeof(struct rtw_phl_noa_desc));
	_os_spinunlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
}

void
_phl_p2pps_clear_noa_desc(struct rtw_phl_p2pps_info *psinfo,
	struct rtw_phl_noa_desc *desc)
{
	void *drvpriv = phlcom_to_drvpriv(psinfo->phl_info->phl_com);

	_os_spinlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	_os_mem_set(drvpriv, desc, 0, sizeof(struct rtw_phl_noa_desc));
	_os_spinunlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
}

void
_phl_p2pps_noa_increase_desc(struct rtw_phl_p2pps_info *psinfo,
	struct rtw_phl_noa_info *info)
{
	void *drvpriv = phlcom_to_drvpriv(psinfo->phl_info->phl_com);

	_os_spinlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	info->en_desc_num++;
	_os_spinunlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
}

void
_phl_p2pps_noa_decrease_desc(struct rtw_phl_p2pps_info *psinfo,
	struct rtw_phl_noa_info *info)
{
	void *drvpriv = phlcom_to_drvpriv(psinfo->phl_info->phl_com);

	_os_spinlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	if (info->en_desc_num > 0)
		info->en_desc_num--;
	else
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_WARNING_, "[NOA]_phl_p2pps_noa_decrease_desc():info->en_desc_num == 0! Flow error\n");
	_os_spinunlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
}

u8
_phl_p2pps_noa_should_activate(struct rtw_phl_p2pps_info *psinfo,
	struct rtw_phl_noa_desc *in_desc)
{
	u8 ret = true;
	if (in_desc->tag == P2PPS_TRIG_GO) {
		ret = true;
	} else if (in_desc->tag == P2PPS_TRIG_GC) {
		ret = true;
	} else if (in_desc->tag == P2PPS_TRIG_GC_255) {
		ret = true;
	} else if (in_desc->tag == P2PPS_TRIG_2G_SCC_1AP_1STA_BT) {
		ret = true;
	} else if (in_desc->tag == P2PPS_TRIG_MCC) {
		ret = false;
#ifdef RTW_WKARD_P2PPS_NOA_MCC
		goto exit;
#endif
	}
#ifdef RTW_WKARD_P2PPS_SINGLE_NOA
#ifdef CONFIG_MR_COEX_SUPPORT
	/*Currently should only notify MRC for limit request*/
	/*Under count == 255 case */
	if (in_desc->count != 255) {
		if (_phl_p2pps_query_mcc_inprog_wkard(psinfo->phl_info,
							in_desc->w_role)) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_WARNING_, "[NOA]_phl_p2pps_noa_should_activate():mcc in progress and noa requset != 255, currently not handling!\n");
			ret = false;
		}
	} else {
		if (phl_mr_coex_noa_dur_lim_change(psinfo->phl_info,
				in_desc->w_role,
				get_rlink(in_desc->w_role, RTW_RLINK_PRIMARY),
				in_desc)) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_noa_should_activate():mrc take over this req!\n");
			ret = false;
		}
	}
#endif /* CONFIG_MR_COEX_SUPPORT */
#endif
exit:
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_noa_should_activate():tag = %d, return = %d\n",
		in_desc->tag, ret);
	return ret;
}

u8
_phl_p2pps_noa_is_all_disable(struct rtw_phl_p2pps_info *psinfo,
	struct rtw_phl_noa_info *info)
{
	u8 i = 0;
	void *drvpriv = phlcom_to_drvpriv(psinfo->phl_info->phl_com);
	_os_spinlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	for (i = 0; i < MAX_NOA_DESC; i++) {
		struct rtw_phl_noa_desc *desc = &info->noa_desc[i];
		if(desc->enable) {
			_os_spinunlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
			return false;
		}
	}
	_os_spinunlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	return true;
}

u8
_phl_p2pps_noa_assign_noaid(struct rtw_phl_p2pps_info *psinfo,
	struct rtw_phl_noa_info *info,
	struct rtw_phl_noa_desc *desc)
{
	u8 max = 0, id = NOAID_NONE, i = 0;
	void *drvpriv = phlcom_to_drvpriv(psinfo->phl_info->phl_com);

	_os_spinlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	if (info->en_desc_num == 0) {
		id = 0;/*not inited flow*/
	} else {

		for (i = 0; i < MAX_NOA_DESC; i++) {

			if (info->noa_desc[i].noa_id == NOAID_NONE)
				continue;
			if (info->noa_desc[i].noa_id > max)
				max = info->noa_desc[i].noa_id;
		}
		if(max != 0)
			id = max + 1;
		else id = 0;
	}
	_os_spinunlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_noa_assign_noaid(): Final ID = %d.\n",
		id);
	return id;
}

enum rtw_phl_status
_phl_p2pps_noa_disable(struct rtw_phl_p2pps_info *psinfo,
                       struct rtw_phl_noa_info *noa_info,
                       struct rtw_phl_noa_desc *noa_desc,
                       u8 clear_desc)
{
	enum rtw_phl_status ret = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hal_ret = RTW_HAL_STATUS_FAILURE;
	void *drvpriv = phlcom_to_drvpriv(psinfo->phl_info->phl_com);
	void *hal = psinfo->phl_info->hal;
	struct rtw_phl_stainfo_t *sta_info = NULL;
	struct rtw_wifi_role_t *w_role = NULL;
	struct phl_info_t *phl_info = psinfo->phl_info;
	u8 en_to_fw = 0;
	u8 idx = 0;

	if (noa_info->paused && clear_desc) {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_noa_disable():NoA info is in puase state, clear desc only!\n");
		_phl_p2pps_clear_noa_desc(psinfo,noa_desc);
		return RTW_PHL_STATUS_SUCCESS;
	}

	w_role = noa_desc->w_role;

	_os_spinlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	en_to_fw = (noa_desc->noa_id != NOAID_NONE && noa_desc->enable);
	_os_spinunlock(drvpriv, &psinfo->p2pps_lock, _bh, NULL);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NoA]%s(): en_to_fw(%d), clear_desc(%d)\n",
		__func__, en_to_fw, clear_desc);
	if (en_to_fw) {
		sta_info = rtw_phl_get_stainfo_self(psinfo->phl_info,
		                                    noa_desc->rlink);
		hal_ret = rtw_hal_noa_disable(hal, noa_info, noa_desc,
							sta_info->macid);
		if (hal_ret!= RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_ERR_, "[NOA]_phl_p2pps_noa_disable():NoA Disable fail! tag = %d, ID = %d, HAL return = %d\n",
				noa_desc->tag, noa_desc->noa_id, hal_ret);
			ret = RTW_PHL_STATUS_FAILURE;
		} else {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_noa_disable():NoA disable SUCCESS! tag = %d, ID = %d\n",
				noa_desc->tag, noa_desc->noa_id);
			_phl_p2pps_noa_decrease_desc(psinfo,noa_info);
			ret = RTW_PHL_STATUS_SUCCESS;
			if (clear_desc)
				_phl_p2pps_clear_noa_desc(psinfo,noa_desc);
		}
	} else {
		/*not enabled to fw case*/
		ret = RTW_PHL_STATUS_SUCCESS;
		if (clear_desc)
			_phl_p2pps_clear_noa_desc(psinfo,noa_desc);
	}

	if(RTW_PHL_STATUS_SUCCESS == ret) {
		if(NULL != w_role) {
			/* notify BTC */
			/* copy noa_desc array to w_role*/
			for (idx = 0; idx < MAX_NOA_DESC; idx ++) {
				_phl_p2pps_copy_noa_desc(psinfo,
					w_role->noa_desc + idx,
					noa_info->noa_desc + idx);
			}
			phl_role_noa_notify(phl_info, w_role);
		} else {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_WARNING_, "[NOA]_phl_p2pps_noa_disable():w_role in noa_desc is NULL, not to notify to BTC\n");
		}
	}

	return ret;
}

void _phl_p2pps_noa_disable_all(struct phl_info_t *phl,
	struct rtw_wifi_role_t *w_role)
{
	struct rtw_phl_p2pps_info *psinfo = phl_to_p2pps_info(phl);
	u8 role_id = get_role_idx(w_role);
	struct rtw_phl_noa_info *noa_info = &psinfo->noa_info[role_id];
	u8 i = 0;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:====>\n", __func__);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s: Disable all NoA for wrole(%d)!\n",
	                                      __func__, role_id);
	_phl_p2pps_dump_noa_table(phl_to_p2pps_info(phl),noa_info);
	for (i = 0; i < MAX_NOA_DESC; i++) {
		struct rtw_phl_noa_desc *desc = &noa_info->noa_desc[i];
		if (desc->enable) {
			_phl_p2pps_noa_disable(psinfo, noa_info, desc, true);
		}
	}
	noa_info->paused = false;
	_phl_p2pps_dump_noa_table(phl_to_p2pps_info(phl),noa_info);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:<====\n", __func__);
}

enum rtw_phl_status
_phl_p2p_ps_up_clients_macid_for_ap_role(struct phl_info_t *phl,
		struct rtw_phl_p2pps_info *psinfo, struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct macid_ctl_t *mc = phl_to_mac_ctrl(phl);
	struct rtw_phl_stainfo_t *self_sta = rtw_phl_get_stainfo_self(phl,
					&wrole->rlink[RTW_RLINK_PRIMARY]);
	u32 clients_usedmap[PHL_MACID_MAX_ARRAY_NUM] = {0};

	if (wrole->rlink[RTW_RLINK_PRIMARY].assoc_sta_queue.cnt == 1) {
		psts = RTW_PHL_STATUS_SUCCESS;
		goto exit;
	}
	_os_mem_cpy(phlcom_to_drvpriv(phl->phl_com), clients_usedmap,
			&mc->wifi_role_usedmap[wrole->id][0],
			PHL_MACID_MAX_ARRAY_NUM * sizeof(u32));
	/*clean self macid*/
	phl_macid_map_clr(clients_usedmap, self_sta->macid);
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_noa_sta_macid_up(phl->hal,
					self_sta->macid, true,
					(u8 *)clients_usedmap,
					PHL_MACID_MAX_ARRAY_NUM * sizeof(u32))) {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_ERR_, "[NoA]%s(): Up clients_usedmap fail\n",
			__func__);
	} else {
		psts = RTW_PHL_STATUS_SUCCESS;
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_ERR_, "[NoA]%s(): Up clients_usedmap ok\n",
			__func__);
	}
exit:
	return psts;
}

enum rtw_phl_status
_phl_p2pps_noa_enable(struct rtw_phl_p2pps_info *psinfo,
                      struct rtw_phl_noa_info *noa_info,
                      struct rtw_phl_noa_desc *noa_desc,
                      struct rtw_phl_noa_desc *in_desc)
{
	enum rtw_phl_status ret = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hal_ret = RTW_HAL_STATUS_FAILURE;
	void *hal = psinfo->phl_info->hal;
	struct rtw_phl_stainfo_t *sta_info = NULL;
	struct rtw_wifi_role_t *w_role = NULL;
	struct phl_info_t *phl_info = psinfo->phl_info;
	u8 idx = 0;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NoA]%s()\n",
		__func__);
	_phl_p2pps_copy_noa_desc(psinfo, noa_desc, in_desc);

	/* get w_role */
	w_role = noa_desc->w_role;

	if(NULL != w_role) {
		/* notify BTC */
		/* copy noa_desc array to w_role */
		for (idx = 0; idx < MAX_NOA_DESC; idx ++) {
			_phl_p2pps_copy_noa_desc(psinfo,
				w_role->noa_desc+idx,
				noa_info->noa_desc+idx);
		}
		phl_role_noa_notify(phl_info, w_role);
	} else {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_WARNING_, "[NOA]_phl_p2pps_noa_enable():w_role in noa_desc is NULL, not to notify to BTC\n");
	}

	if (noa_info->paused) {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_noa_enable():NoA is in pause state, record request and leave\n");
		return RTW_PHL_STATUS_SUCCESS;
	}
	if (_phl_p2pps_noa_should_activate(psinfo, noa_desc)) {
		noa_desc->noa_id = _phl_p2pps_noa_assign_noaid(psinfo, noa_info,
					noa_desc);
		sta_info = rtw_phl_get_stainfo_self(psinfo->phl_info,
		                                    noa_desc->rlink);
		hal_ret = rtw_hal_noa_enable(hal, noa_info, noa_desc,
							sta_info->macid);
		if (hal_ret != RTW_HAL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_ERR_, "[NOA]_phl_p2pps_noa_enable():NoA enable fail! tag = %d, ID = %d, HAL return = %d\n",
				noa_desc->tag, noa_desc->noa_id, hal_ret);
			noa_desc->noa_id = NOAID_NONE;
			if (hal_ret == RTW_HAL_STATUS_RESOURCE)
				ret = RTW_PHL_STATUS_RESOURCE;
			else
				ret = RTW_PHL_STATUS_FAILURE;
		} else {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]_phl_p2pps_noa_enable():NoA enable SUCCESS! tag = %d, ID = %d, HAL return = %d\n",
				noa_desc->tag, noa_desc->noa_id, hal_ret);
			_phl_p2pps_noa_increase_desc(psinfo,noa_info);
			/* Update macid exclude self macid */
			if (rtw_phl_role_is_ap_category(w_role)) {
				ret = _phl_p2p_ps_up_clients_macid_for_ap_role(
								phl_info, psinfo,
								w_role);
			} else {
				ret = RTW_PHL_STATUS_SUCCESS;
			}
		}
	} else {
		noa_desc->noa_id = NOAID_NONE; /*not activate*/
		ret = RTW_PHL_STATUS_SUCCESS;
	}

	return ret;
}

bool
_phl_p2p_noa_runing(struct phl_info_t *phl,
			struct rtw_wifi_role_t *w_role)
{
	struct rtw_phl_p2pps_info *psinfo = phl_to_p2pps_info(phl);
	u8 rid = get_role_idx(w_role);
	struct rtw_phl_noa_info *noa_info = &psinfo->noa_info[rid];
	u8 i = 0;
	bool ret = false;

	for (i = 0; i < MAX_NOA_DESC; i++) {
		struct rtw_phl_noa_desc *desc = &noa_info->noa_desc[i];

		if(desc->enable) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s: rid(%d), tag(%d) is runing\n",
				__FUNCTION__, rid, desc->tag);
			ret = true;
			break;
		}
	}
	return ret;
}

static void _phl_noa_update_done(void *priv, u8 *param,
				u32 param_len, enum rtw_phl_status sts)
{
	if (param) {
		_os_kmem_free(priv, param, param_len);
		param = NULL;
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "%s\n", __func__);
	}
}

void
phl_p2pps_noa_resume_all(struct phl_info_t *phl,
	struct rtw_wifi_role_t *w_role)
{
	struct rtw_phl_p2pps_info *psinfo = phl_to_p2pps_info(phl);
	u8 role_idx = get_role_idx(w_role);
	struct rtw_phl_noa_info *noa_info = &psinfo->noa_info[role_idx];
	u8 i = 0;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_noa_resume_all():====>\n");
	if (!noa_info->paused) {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_noa_resume_all():NoA not paused on role:%d\n",
			w_role->id);
		goto exit;
	}
	// _phl_p2pps_dump_noa_table(phl_to_p2pps_info(phl),noa_info);
	noa_info->paused = false;
	for (i = 0; i < MAX_NOA_DESC; i++) {
		struct rtw_phl_noa_desc *desc = &noa_info->noa_desc[i];
		if(desc->enable)
			_phl_p2pps_noa_enable(psinfo, noa_info, desc, desc);
	}
	// _phl_p2pps_dump_noa_table(phl_to_p2pps_info(phl),noa_info);
exit:
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_noa_resume_all():<====\n");
}

void
phl_p2pps_noa_all_role_resume(struct phl_info_t *phl_info, u8 band_idx)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);
	struct rtw_wifi_role_t *wrole = NULL;
	u8 ridx = 0;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (!(band_ctrl->role_map & BIT(ridx)))
			continue;
		wrole = phl_get_wrole_by_ridx(phl_info, ridx);
		if (wrole == NULL)
			continue;
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]p2pps_noa_all_role_resume():role_id(%d)\n",
			ridx);
		phl_p2pps_noa_resume_all(phl_info, wrole);
	}
}

void
phl_p2pps_noa_pause_all(struct phl_info_t *phl,
	struct rtw_wifi_role_t *w_role)
{
	struct rtw_phl_p2pps_info *psinfo = phl_to_p2pps_info(phl);
	u8 role_idx = get_role_idx(w_role);
	struct rtw_phl_noa_info *noa_info = &psinfo->noa_info[role_idx];
	u8 i = 0;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_noa_pause_all():====>\n");
	//_phl_p2pps_dump_noa_table(phl_to_p2pps_info(phl),noa_info);
	if (noa_info->paused) {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_noa_pause_all():This role(%d) NoA is in pause state\n",
			role_idx);
		goto exit;
	}
	noa_info->paused = true;
	for (i = 0; i < MAX_NOA_DESC; i++) {
		struct rtw_phl_noa_desc *desc = &noa_info->noa_desc[i];
		_phl_p2pps_noa_disable(psinfo, noa_info, desc, false);
	}
	//_phl_p2pps_dump_noa_table(phl_to_p2pps_info(phl),noa_info);
exit:
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_noa_pause_all():<====\n");
}

void phl_p2pps_noa_all_role_pause(struct phl_info_t *phl_info, u8 band_idx)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl = &(mr_ctl->band_ctrl[band_idx]);
	struct rtw_wifi_role_t *wrole = NULL;
	u8 ridx = 0;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_noa_all_role_pause(): band_idx(%d)\n",
		band_idx);
	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (!(band_ctrl->role_map & BIT(ridx)))
			continue;
		wrole = phl_get_wrole_by_ridx(phl_info, ridx);
		if (wrole == NULL)
			continue;
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_p2pps_noa_all_role_pause():role_id(%d)\n",
			ridx);
		phl_p2pps_noa_pause_all(phl_info, wrole);
	}
}

void phl_p2pps_noa_disable_all(struct phl_info_t *phl_info,
                               struct rtw_wifi_role_t *w_role,
                               struct rtw_wifi_role_link_t *rlink)
{
#ifdef RTW_WKARD_P2PPS_SINGLE_NOA
#ifdef CONFIG_MR_COEX_SUPPORT

	struct rtw_phl_noa_desc dis_desc = {0};
	/*for notify MR for limitation disabled*/
	dis_desc.enable = false;
	dis_desc.w_role = w_role;
	/*open when mr ready*/
	phl_mr_coex_noa_dur_lim_change(phl_info,
					w_role,
					get_rlink(w_role, RTW_RLINK_PRIMARY),
					&dis_desc);
#endif /* CONFIG_MR_COEX_SUPPORT */
#endif
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:====>\n", __func__);
	_phl_p2pps_noa_disable_all(phl_info, w_role);
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:Disable TSF 32 TOG for role %d\n",
	                                      __func__, w_role->id);

	rtw_hal_tsf32_tog_disable(phl_info->hal, rlink);

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:<====\n", __func__);
}

void phl_p2pps_query_noa_with_cnt255(struct phl_info_t* phl_info,
                                     struct rtw_wifi_role_t *w_role,
                                     struct rtw_wifi_role_link_t *rlink,
                                     struct rtw_phl_noa_desc *desc)
{
	struct rtw_phl_p2pps_info *psinfo = phl_to_p2pps_info(phl_info);
	u8 role_idx = get_role_idx(w_role);
	struct rtw_phl_noa_info *info = &psinfo->noa_info[role_idx];
	struct rtw_phl_noa_desc *tmp_desc = NULL;

	tmp_desc = _phl_p2pps_get_first_noa_desc_with_cnt255(phl_info, info);
	if (tmp_desc) {
		_phl_p2pps_copy_noa_desc(psinfo, desc, tmp_desc);
	} else {
		desc->enable = false;
		desc->w_role = w_role;
		desc->rlink = rlink;
	}
}

enum rtw_phl_status
phl_noa_update(struct phl_info_t *phl_i,
	struct rtw_phl_noa_desc *in_desc)
{
	enum rtw_phl_status ret= RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_p2pps_info *psinfo = phl_to_p2pps_info(phl_i);
	struct rtw_wifi_role_t *w_role = in_desc->w_role;
	u8 role_id = get_role_idx(w_role);
	struct rtw_phl_noa_info *noa_info = &psinfo->noa_info[role_id];
	u8 desc_idx = in_desc->tag;
	struct rtw_phl_noa_desc *noa_desc = &noa_info->noa_desc[desc_idx];

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:DUMP BEFORE!\n", __func__);
	_phl_p2pps_dump_noa_table(psinfo, noa_info);

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:cur FW en desc num = %d\n",
		__func__, noa_info->en_desc_num);
	if (in_desc->enable) {
		if (_phl_p2pps_noa_is_all_disable(psinfo, noa_info)) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:roleid(%d) Enable TSF 32 Toggle!\n",
				__func__, role_id);

			rtw_hal_tsf32_tog_enable(phl_i->hal, in_desc->rlink);
			/*todo set TSF_ BIT TOG H2C ON*/
		}
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:Tag = %d, NoA enable request!\n",
			__func__, in_desc->tag);
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:Tag = %d, NoA disable origninl req first!\n",
			__func__, in_desc->tag);
		_phl_p2pps_noa_disable(psinfo, noa_info, noa_desc, true);
		ret = _phl_p2pps_noa_enable(psinfo, noa_info, noa_desc,
						in_desc);
	} else {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:Tag = %d, NoA disable request!\n",
			__func__, in_desc->tag);
		ret = _phl_p2pps_noa_disable(psinfo, noa_info, noa_desc, true);
		if (_phl_p2pps_noa_is_all_disable(psinfo, noa_info)) {
			PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:roleid(%d) Disable TSF 32 Toggle!\n",
				__func__, role_id);

			rtw_hal_tsf32_tog_disable(phl_i->hal, in_desc->rlink);
			/*todo set TSF_ BIT TOG H2C OFF*/
		}
	}
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s:DUMP AFTER!\n", __func__);
	_phl_p2pps_dump_noa_table(psinfo, noa_info);
	return ret;
}

enum rtw_phl_status
rtw_phl_p2pps_noa_update(void *phl,
	struct rtw_phl_noa_desc *in_desc)
{
	return phl_noa_update((struct phl_info_t *)phl, in_desc);
}

void
phl_p2pps_ap_client_notify(struct phl_info_t *phl,
		struct rtw_wifi_role_t *wrole, enum link_state lstate, u16 client_macid)
{
	struct rtw_phl_stainfo_t *self_sta = NULL;
	bool join = false;
	u32 macidmap[PHL_MACID_MAX_ARRAY_NUM] = {0};

	if (lstate != PHL_ClIENT_JOINING &&
		lstate != PHL_ClIENT_LEFT)
		return;
	if (false == _phl_p2p_noa_runing(phl, wrole))
		return;
	self_sta = rtw_phl_get_stainfo_self(phl, &wrole->rlink[RTW_RLINK_PRIMARY]);
	if (lstate == PHL_ClIENT_JOINING)
		join = true;
	phl_macid_map_set(macidmap, client_macid);
	if (RTW_HAL_STATUS_SUCCESS != rtw_hal_noa_sta_macid_up(phl->hal,
					self_sta->macid, join, (u8 *)macidmap,
					PHL_MACID_MAX_ARRAY_NUM * sizeof(u32))) {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_ERR_, "[NoA]%s(): link_sts(%d), Up client macid(%d) fail\n",
			__func__, lstate, client_macid);
	} else {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_ERR_, "[NoA]%s(): link_sts(%d), Up client macid(%d) ok\n",
			__func__, lstate, client_macid);
	}
}

void
rtw_phl_p2pps_noa_disable_all(void *phl, struct rtw_wifi_role_t *w_role)
{
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s()!\n", __func__);

	for (idx = 0; idx < w_role->rlink_num; idx++) {
		rlink = get_rlink(w_role, idx);

		phl_p2pps_noa_disable_all((struct phl_info_t *)phl, w_role, rlink);
	}
}

enum rtw_phl_status
phl_cmd_noa_disable_hdl(struct phl_info_t *phl, u8 *param)
{
	enum rtw_phl_status ret = RTW_PHL_STATUS_FAILURE;
	struct rtw_wifi_role_t *w_role = (struct rtw_wifi_role_t *)param;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s\n", __func__);

	for (idx = 0; idx < w_role->rlink_num; idx++) {
		rlink = get_rlink(w_role, idx);
		phl_p2pps_noa_disable_all((struct phl_info_t *)phl, w_role, rlink);
	}
	ret = RTW_PHL_STATUS_SUCCESS;
	return ret;
}

enum rtw_phl_status
rtw_phl_noa_update(void *phl, struct rtw_phl_noa_desc *in_desc,
		enum phl_cmd_type cmd_type, u8 cmd_timeout)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_i = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_i);
	struct rtw_phl_noa_desc *para = NULL;
	u32 para_len = 0;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "%s: wr id(%d), cmd_type(%d), cmd_timeout(%d)\n",
		__func__, in_desc->w_role->id,cmd_type, cmd_timeout);
	_phl_p2pps_dump_single_noa_desc(in_desc);
	if (PHL_CMD_DIRECTLY == cmd_type) {
		psts = phl_noa_update(phl_i, in_desc);
		goto _exit;
	}
	para_len = sizeof(struct rtw_phl_noa_desc);
	para = _os_kmem_alloc(drv_priv, para_len);
	if (para == NULL) {
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_ERR_, "%s - alloc param failed!\n", __func__);
		goto _exit;
	}
	_os_mem_cpy(drv_priv, para, in_desc, sizeof(struct rtw_phl_noa_desc));
	psts = phl_cmd_enqueue(phl_i,
				para->rlink->hw_band,
				MSG_EVT_NOA_UP,
				(u8 *)para,
				para_len,
				_phl_noa_update_done,
				cmd_type,
				cmd_timeout);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(drv_priv, para, para_len);
		psts = RTW_PHL_STATUS_FAILURE;
	}
_exit:
	return psts;
}

enum rtw_phl_status
rtw_phl_noa_disable_all(void *phl, struct rtw_wifi_role_t *wrole,
		enum phl_cmd_type cmd_type, u8 cmd_timeout)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_i = (struct phl_info_t *)phl;

	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]%s: wr id(%d), cmd_type(%d), cmd_timeout(%d)\n",
		__func__, wrole->id, cmd_type, cmd_timeout);
	if (PHL_CMD_DIRECTLY == cmd_type) {
		psts = phl_cmd_noa_disable_hdl(phl_i, (u8 *)wrole);
		goto _exit;
	}
	psts = phl_cmd_enqueue(phl_i,
			get_rlink(wrole, RTW_RLINK_PRIMARY)->hw_band,
			MSG_EVT_NOA_DISABLE,
			(u8 *)wrole, 0,
			NULL,
			cmd_type,
			cmd_timeout);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
	}
_exit:
	return psts;
}

void rtw_phl_p2pps_init_ops(void *phl,
	struct rtw_phl_p2pps_ops *ops)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_p2pps_info *psinfo = NULL;

	psinfo = phl_to_p2pps_info(phl_info);
	psinfo->ops.priv = ops->priv;
	psinfo->ops.tsf32_tog_update_noa = ops->tsf32_tog_update_noa;
	psinfo->ops.tsf32_tog_update_single_noa = ops->tsf32_tog_update_single_noa;
	PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "rtw_phl_p2pps_init_ops(): init ok\n");
}
#endif
#endif
