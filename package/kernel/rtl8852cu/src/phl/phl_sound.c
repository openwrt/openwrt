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

#ifdef CONFIG_PHL_CMD_BF

void __reset_snd_grp(struct phl_snd_grp *grp)
{
	u8 i = 0;

	grp->snd_type = PHL_SND_TYPE_INVALID;
	grp->band = 0;
	grp->num_sta = 0;
	grp->wrole_idx = 0;
	grp->grp_tier = PHL_SND_GRP_TIER_1;
	grp->snd_sts = PHL_SND_STS_PENDING;
	for (i = 0; i < MAX_NUM_STA_SND_GRP; i++) {
		grp->sta[i].valid = false;
		grp->sta[i].macid = 0;
		grp->sta[i].bw = CHANNEL_WIDTH_20;
		grp->sta[i].snd_fb_t = PHL_SND_FB_TYPE_SU;
		grp->sta[i].npda_sta_info = 0;
		grp->sta[i].bf_entry = NULL;
		grp->sta[i].snd_sts = PHL_SND_STS_PENDING;
	}
}

enum rtw_phl_status phl_snd_init_snd_grp(struct phl_info_t *phl_info)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *param = &snd->snd_param;
	u8 i = 0;
	do {
		if (param->snd_grp == NULL) {
			status = RTW_PHL_STATUS_FAILURE;
			break;
		}
		for (i = 0; i < MAX_SND_GRP_NUM; i++) {
			__reset_snd_grp(&param->snd_grp[i]);
			param->snd_grp[i].gidx = i;
		}
	} while (0);

	return status;
}

/* PHL SOUND EXTERNAL APIs */
/* get sounding in progress */
u8 rtw_phl_snd_chk_in_progress(void *phl)
{
	u8 ret = 0;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	void *d = phl_to_drvpriv(phl_info);

	_os_spinlock(d, &snd->snd_lock, _bh, NULL);
	ret = snd->snd_in_progress;
	_os_spinunlock(d, &snd->snd_lock, _bh, NULL);

	return ret;
}

enum rtw_phl_status
rtw_phl_snd_add_grp(void *phl,
                    struct rtw_wifi_role_link_t *rlink,
                    u8 gidx,
                    u16 *macid,
                    u8 num_sta,
                    bool he,
                    bool mu)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	struct phl_snd_grp *snd_grp = NULL;
	struct rtw_phl_stainfo_t *sta_info = NULL;
	u8 i = 0;
	u8 cnt = 0;

	do {
		if (gidx >= MAX_SND_GRP_NUM)
			break;
		if ((num_sta > MAX_NUM_STA_SND_GRP) || (0 == num_sta))
			break;
		if ((NULL == rlink) || (NULL == macid))
			break;


		snd_grp = &snd_param->snd_grp[gidx];

		__reset_snd_grp(snd_grp);
		snd_grp->wrole_idx = rlink->wrole->id;
		snd_grp->band = rlink->hw_band;

		if (RTW_FW_AP == phl_info->phl_com->fw_info.fw_type) {
			snd_grp->snd_type = (he ? PHL_SND_TYPE_HE_SW : PHL_SND_TYPE_VHT_SW);
		} else if ((RTW_FW_NIC == phl_info->phl_com->fw_info.fw_type) ||
		           (RTW_FW_WOWLAN == phl_info->phl_com->fw_info.fw_type)) {
			if (num_sta > 1)
				break;
			snd_grp->snd_type = (he ? PHL_SND_TYPE_HE_HW : PHL_SND_TYPE_VHT_HW);
		} else {
			PHL_INFO("%s :Unknown FW type!!!\n", __func__);
			break;
		}

		for (i = 0; i < num_sta; i++) {
			sta_info = rtw_phl_get_stainfo_by_macid(phl_info, macid[i]);

			if (sta_info == NULL)
				continue;

			snd_grp->sta[cnt].macid = macid[i];
			snd_grp->sta[cnt].valid = true;
			snd_grp->sta[cnt].bw = sta_info->chandef.bw;
			snd_grp->sta[cnt].snd_fb_t = (mu ? PHL_SND_FB_TYPE_MU : PHL_SND_FB_TYPE_SU);
			snd_grp->sta[cnt].snd_sts = PHL_SND_STS_PENDING;
			cnt++;
		}

		snd_grp->num_sta = cnt;

		if (cnt > 0) {
			snd_param->grp_used_map |= BIT(gidx);
			status = RTW_PHL_STATUS_SUCCESS;
		}
	} while(0);

	return status;
}

/**
 * rtw_phl_sound_start
 * @phl:(struct phl_info_t *)
 * @st_dlg_tkn: start dialog token value, if 0, it will use previous sounding dialog token;
 * @period: sounding process period (group--> next group)
 * @test_flag: test mode flags
 **/
enum rtw_phl_status
rtw_phl_sound_start_ex(void *phl, u8 wrole_idx, u8 st_dlg_tkn, u8 period, u8 test_flag)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	struct rtw_wifi_role_t *wrole = NULL;
	u8 i = 0;

	wrole = phl_get_wrole_by_ridx(phl_info, wrole_idx);
	do {
		snd_param->m_wrole = (void *)wrole;
		snd_param->snd_proc_timeout_ms = 10;/* ms */
		snd_param->snd_dialog_token = st_dlg_tkn;
		snd_param->snd_proc_period = period;
		snd_param->test_flag = test_flag;
		snd_param->bypass_snd_sts_chk = true;/* temp by pass */
		snd_param->snd_fail_counter = 0;

		/* check grp status */
		for (i = 0; i < MAX_SND_GRP_NUM;i++) {
			if (0 != (snd_param->grp_used_map&BIT(i)))
				snd_param->snd_func_grp_num++;
		}

		/* start with grouping */
		status = phl_snd_cmd_sound_evt(phl, wrole, &snd_param->snd_grp[0], SND_CMD_BFER_PRECFG);

	} while (0);

	return status;

}

/**
 * rtw_phl_sound_start
 * @phl:(struct phl_info_t *)
 * @st_dlg_tkn: start dialog token value, if 0, it will use previous sounding dialog token;
 * @period: sounding process period (group--> next group)
 * @test_flag: test mode flags
 **/
enum rtw_phl_status
rtw_phl_sound_start(void *phl, u8 wrole_idx, u8 st_dlg_tkn, u8 period, u8 test_flag)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	struct rtw_wifi_role_t *wrole = NULL;

	wrole = phl_get_wrole_by_ridx(phl_info, wrole_idx);
	do {
		snd_param->m_wrole = (void *)wrole;
		snd_param->snd_proc_timeout_ms = 10;/* ms */
		snd_param->snd_dialog_token = st_dlg_tkn;
		snd_param->snd_proc_period = period;
		snd_param->test_flag = test_flag;
		snd_param->bypass_snd_sts_chk = true;/* temp by pass */
		snd_param->snd_fail_counter = 0;

		/* start with grouping */
		status = phl_snd_cmd_sound_evt(phl, wrole, &snd_param->snd_grp[0], SND_CMD_BFER_GROUPING);

	} while (0);

	return status;

}

enum rtw_phl_status
rtw_phl_sound_down_ev(void *phl)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	u8 cur_idx = 0;
	struct rtw_wifi_role_t *wrole = NULL;
	void *d = phl_to_drvpriv(phl_info);

	do {
		if (snd == NULL)
			break;

		if (snd->is_terminated)
			break;

		if (0 == snd->snd_in_progress)
			break;

		cur_idx = snd_param->cur_proc_grp_idx;

		if (SND_CMD_BFER_SOUND !=
			snd_param->snd_grp[cur_idx].snd_cmd.event)
				break;

		wrole = phl_get_wrole_by_ridx(phl_info, snd_param->snd_grp[cur_idx].wrole_idx);

		if (NULL == wrole)
			break;

		phl_snd_cmd_sound_cancel_msg(phl_info);
		_os_cancel_timer(d, &(snd->snd_timer));

		status = phl_snd_cmd_sound_evt(phl_info,
					       wrole,
					       &(snd_param->snd_grp[cur_idx]),
					       SND_CMD_BFER_POSTCFG);

	} while(0);

	return status;
}


enum rtw_phl_status
rtw_phl_sound_abort(void *phl)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	struct rtw_wifi_role_t *wrole = (struct rtw_wifi_role_t *)snd_param->m_wrole;
	void *d = phl_to_drvpriv(phl_info);

	phl_snd_cmd_sound_cancel_msg(phl_info);
	_os_cancel_timer(d, &(snd->snd_timer));

	status = phl_snd_cmd_sound_evt(phl_info,
				       wrole,
				       &(snd_param->snd_grp[0]),
				       SND_CMD_BFER_TERMINATE);

	return status;
}

/* set fixed mode parameters APIs*/
void rtw_phl_snd_dump_fix_para(struct phl_info_t *phl_info)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_snd_fix_param *para = NULL;
	u8 i = 0;

	para = &snd->snd_param.fix_param;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "===> rtw_phl_snd_fix_dump_para \n");

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "test_flag = 0x%x \n", snd->snd_param.test_flag);

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "en_fix_gidx = %d \n", para->en_fix_gidx ? 1 : 0);
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "en_fix_fb_type = %d \n", para->en_fix_fb_type ? 1 : 0);
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "en_fix_sta = %d \n", para->en_fix_sta ? 1 : 0);
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "en_fix_snd_bw = %d \n", para->en_fix_snd_bw ? 1 : 0);

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "grp_idx = %d \n", para->grp_idx);
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "snd_fb_type = %d \n", para->snd_fb_type);

	for (i = 0; i < MAX_NUM_STA_SND_GRP; i++) {
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "sta_macid[i] = 0x%x \n", para->sta_macid[i]);
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "bw[i] = %d \n",para->bw[i]);
	}

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "<=== rtw_phl_snd_fix_dump_para \n");
}
/* fixed group idx */
void rtw_phl_snd_fix_gidx(struct phl_info_t *phl_info, bool en, u8 gidx)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "rtw_phl_snd_fix_gidx() set sounding gidx = 0x%x\n", gidx);
	if (en) {
		snd->snd_param.fix_param.en_fix_gidx = 1;
		snd->snd_param.fix_param.grp_idx = gidx;
	} else {
		snd->snd_param.fix_param.en_fix_gidx = 0;
	}
}
/* fixed snd feedback type */
void rtw_phl_snd_fix_snd_fb_type(struct phl_info_t *phl_info,
				 bool en, enum snd_fb_type fb_type)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "rtw_phl_snd_fix_gidx() set sounding fb_type = 0x%x\n",
		 fb_type);
	if (en) {
		snd->snd_param.fix_param.en_fix_fb_type = 1;
		snd->snd_param.fix_param.snd_fb_type = fb_type;
	} else {
		snd->snd_param.fix_param.en_fix_fb_type = 0;
	}
}

/* fixed sounding sta macids */
void rtw_phl_snd_fix_set_sta(struct phl_info_t *phl_info,
					bool en, u8 sidx, u16 macid)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "rtw_phl_snd_fix_set_sta() set sta[%d] macid = 0x%x\n",
		 sidx, macid);
	if (en) {
		snd->snd_param.fix_param.en_fix_sta = 1;
		if (sidx < MAX_NUM_STA_SND_GRP)
			snd->snd_param.fix_param.sta_macid[sidx] = macid;
		else
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "ERROR, sidx >= 4\n");
	} else {
		snd->snd_param.fix_param.en_fix_sta = 0;
	}
}

/* fixed sounding sta bw */
void rtw_phl_snd_fix_set_bw(struct phl_info_t *phl_info,
					bool en, u8 sidx, enum channel_width bw)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "rtw_phl_snd_fix_set_bw() set sta[%d] bw = 0x%x\n", sidx, bw);
	if (en) {
		snd->snd_param.fix_param.en_fix_snd_bw = 1;
		if (sidx < MAX_NUM_STA_SND_GRP)
			snd->snd_param.fix_param.bw[sidx] = bw;
		else
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "ERROR, sidx >= 4\n");
	} else {
		snd->snd_param.fix_param.en_fix_snd_bw = 0;
	}
}

/* set forced fw tx mu-mimo (forced fw tx decision) */
void rtw_phl_snd_fix_tx_he_mu(struct phl_info_t *phl_info, u8 gid, bool en)
{
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "rtw_phl_snd_fix_tx_mu_para()\n");

	rtw_hal_bf_set_txmu_para(phl_info->hal, gid, en,
				 HAL_PROT_NO_PROETCT, HAL_ACK_N_USER_BA);

	rtw_hal_bf_set_fix_mode(phl_info->hal, gid, en);
}


/* PHL SOUND INTERNAL APIs */
/* SND FUNC */
enum rtw_phl_status
phl_snd_func_snd_init(struct phl_info_t *phl_info)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	void *d = phl_to_drvpriv(phl_info);
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	u8 f_ru_tbl_80m[MAX_SND_HE_BFRP_USER_NUM][MAX_SND_HE_BFRP_USER_NUM] = {
		{RTW_HE_RU996_1, RTW_HE_RU996_1, RTW_HE_RU996_1 ,RTW_HE_RU996_1},
		{RTW_HE_RU484_1, RTW_HE_RU484_2, RTW_HE_RU996_1 ,RTW_HE_RU996_1},
		{RTW_HE_RU484_1, RTW_HE_RU242_3, RTW_HE_RU242_4 ,RTW_HE_RU996_1},
		{RTW_HE_RU242_1, RTW_HE_RU242_2, RTW_HE_RU242_3 ,RTW_HE_RU242_4}
	};

	u8 f_ru_tbl_20m[MAX_SND_HE_BFRP_USER_NUM][MAX_SND_HE_BFRP_USER_NUM] = {
		{RTW_HE_RU242_1, RTW_HE_RU242_1, RTW_HE_RU242_1, RTW_HE_RU242_1},
		{RTW_HE_RU106_1, RTW_HE_RU106_1, RTW_HE_RU242_1, RTW_HE_RU242_1},
		{RTW_HE_RU106_1, RTW_HE_RU52_3, RTW_HE_RU52_4, RTW_HE_RU242_1},
		{RTW_HE_RU52_1, RTW_HE_RU52_2, RTW_HE_RU52_3, RTW_HE_RU52_4}
	};

	/* Add Other Sounding FUNC/PRCO Initialization Here */
	snd->snd_param.snd_proc_timeout_ms = SND_PROC_DEFAULT_TIMEOUT;/* ms */
	snd->snd_param.cur_proc_grp_idx = 0;
	snd->snd_param.pre_proc_grp_idx = 0;
	snd->snd_param.snd_dialog_token = 1;
	snd->snd_param.snd_func_grp_num = 0;
	snd->snd_param.grp_used_map = 0;
	snd->snd_param.snd_proc_period = SND_PROC_DEFAULT_PERIOD;
	snd->snd_param.snd_fail_counter = 0;
	snd->snd_in_progress = 0;

	/*fixed_ru_tbl*/
	_os_mem_cpy(d, snd->snd_param.fix_param.f_ru_tbl_20, f_ru_tbl_20m,
		    MAX_SND_HE_BFRP_USER_NUM * MAX_SND_HE_BFRP_USER_NUM);
	_os_mem_cpy(d, snd->snd_param.fix_param.f_ru_tbl_80, f_ru_tbl_80m,
		    MAX_SND_HE_BFRP_USER_NUM * MAX_SND_HE_BFRP_USER_NUM);

	return pstatus;
}

enum rtw_phl_status
phl_snd_func_pre_config(struct phl_info_t *phl_info)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	void *d = phl_to_drvpriv(phl_info);

	snd_param->proc_start_time = _os_get_cur_time_ms();
	snd_param->cur_proc_grp_idx = 0; /* default start from group idx 0 */
	snd_param->pre_proc_grp_idx = 0;
	_os_spinlock(d, &snd->snd_lock, _bh, NULL);
	snd->is_terminated = 0;
	snd->snd_in_progress = 1;
	_os_spinunlock(d, &snd->snd_lock, _bh, NULL);

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "PHL SND FUNC Start with SND Dialog Token = 0x%x\n",
		 snd_param->snd_dialog_token);


	return pstatus;
}


/* SND_FUNC : GROUP related */
/**
 * phl_snd_proc_get_grp()
 * 	get the grp(struct phl_sound_grp *) with group index.
 * input:
 * @gidx: group idx.
 * return:
 * @grp: (struct phl_snd_grp *grp), NULL = FAIL;
 */
struct phl_snd_grp *
phl_snd_get_grp_byidx(struct phl_info_t *phl_info, u8 gidx)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	struct phl_snd_grp *grp = NULL;

	do {
		if (gidx >= MAX_SND_GRP_NUM)
			break;

		if (!(snd_param->grp_used_map & BIT(gidx)))
			break;

		if (0 == snd_param->snd_grp[gidx].num_sta)
			break;

		if (PHL_SND_TYPE_INVALID == snd_param->snd_grp[gidx].snd_type)
			break;

		grp = &snd_param->snd_grp[gidx];

	} while (0);

	return grp;
}

/**
 * phl_snd_func_remove_grp()
 * 	remove the target sounding grp from sound process;
 * input:
 * @grp: (struct phl_snd_grp *) target sounding grp,
 */
enum rtw_phl_status
phl_snd_func_remove_grp(struct phl_info_t *phl_info, struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;

	if (grp == NULL) {
		return pstatus;
	}
	if (snd_param->grp_used_map & BIT(grp->gidx)) {

		/* Check and Release all the BF resource */
		pstatus = phl_snd_proc_release_res(phl_info, grp);
		if (pstatus != RTW_PHL_STATUS_SUCCESS) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "PHL SND Remove Grp : release BF resouce fail\n");
		}

		/* Reset group content to default value */
		__reset_snd_grp(grp);

		/* Clear Group BIT */
		snd_param->grp_used_map &= ~BIT(grp->gidx);
		snd_param->snd_func_grp_num--;

	} else {
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "PHL SND Remove Grp : Group is not in used!!!\n");
	}

	return pstatus;
}

/**
 * phl_snd_func_remove_grp_all()
 * 	remove the all of the sounding grp from sound process;
 */
void
phl_snd_func_remove_grp_all(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_snd_grp *grp = NULL;
	u8 idx = 0;

	for(idx = 0; idx < MAX_SND_GRP_NUM; idx++) {
		grp = phl_snd_get_grp_byidx(phl_info, idx);
		if (grp != NULL) {
			pstatus = phl_snd_func_remove_grp(phl_info, grp);
			if (pstatus != RTW_PHL_STATUS_SUCCESS) {
				PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "Remove SND GRP[%d] Fail\n", idx);
			}
		}
	}
}

/**
 * _phl_snd_get_available_gidx()
 *	Get available group resource.
 * return:
 * @gidx: u8, the group idx in snd_param->grp[n]
 */
u8 _phl_snd_get_available_gidx(struct phl_sound_obj *snd)
{
	struct phl_sound_param *param = &snd->snd_param;
	u8 gidx = MAX_SND_GRP_NUM;

	for (gidx = 0; gidx < MAX_SND_GRP_NUM; gidx++) {
		if (!(param->grp_used_map & BIT(gidx))) {
			param->grp_used_map |= BIT(gidx);
			break;
		}
	}

	return gidx;
}

/**
 * _phl_snd_func_grp_add_sta()
 *	Add the STA into sounding group.
 * input:
 * @sta: (struct rtw_phl_stainfo_t *) the target sta to be added.
 * 	 the function will use  the macid / bw information in sta_info;
 * @gidx: the group idx to add
 */
enum rtw_phl_status
_phl_snd_func_grp_add_sta(
	struct phl_info_t *phl_info, struct rtw_phl_stainfo_t *sta, u8 gidx)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	struct phl_snd_grp *grp = NULL;
	u8 i = 0;
	bool chk = false;

	do {

		if (NULL == sta) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "STA == NULL !!!!\n");
			break;
		}

		if (gidx >= MAX_SND_GRP_NUM) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "Get SND Grp Resource Fail : gidx >= MAX_SND_GRP_NUM\n");
			break;
		}

		grp = &snd_param->snd_grp[gidx];

		/* check grp->sta[i].macid with sta->macid, skip it if same.*/
		for (i = 0; i < grp->num_sta; i++) {
			if(grp->sta[i].macid == sta->macid) {
				chk = true;
				break;
			}
		}
		if (true == chk)
			break;

		if (grp->num_sta >= MAX_NUM_STA_SND_GRP) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "The SND Grp is already has 4 STAs\n");
			break;
		}

		grp->sta[grp->num_sta].macid = sta->macid;
		grp->sta[grp->num_sta].snd_sts = PHL_SND_STS_PENDING;
		grp->sta[grp->num_sta].bw = sta->chandef.bw;
		grp->sta[grp->num_sta].valid = true;
		grp->num_sta++;
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "sta bw = %d\n", sta->chandef.bw);
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "grp->num_sta = %d\n", grp->num_sta);

		pstatus = RTW_PHL_STATUS_SUCCESS;
	} while (0);

	return pstatus;
}

/**
 * phl_snd_func_add_snd_grp :
 * 	Add a Sounding Group with Primary STA for FW Sounding
 * @phl_info: struct phl_info_t *
 * @he_snd: 1 = HE , 0 =VHT
 * @gidx: return value, snd group idxx in group list
 * @psta: (struct rtw_phl_stainfo_t *)Primary Sounding STA,
 *         if pSTA is unavailable , SND PROC for this group will be terminated.
 **/
enum rtw_phl_status
phl_snd_func_add_snd_grp(
	struct phl_info_t *phl_info, bool he_snd,
	u8 wrole_idx, struct rtw_phl_stainfo_t *psta, u8 *gidx)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	struct phl_snd_grp *grp = NULL;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

	do {
		/* Check Primary STA Available*/
		if (psta == NULL) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "psta == NULL\n");
			break;
		}

		/* Get available sounding group resource */
		*gidx = _phl_snd_get_available_gidx(snd);
		if (*gidx >= MAX_SND_GRP_NUM) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "Get SND Grp Resource Fail : gidx >= MAX_SND_GRP_NUM\n");
			break;
		}

		grp = &(snd_param->snd_grp[*gidx]);
		grp->band = psta->rlink->hw_band;
		grp->snd_type = he_snd ? PHL_SND_TYPE_HE_SW :
					 PHL_SND_TYPE_VHT_SW;
		grp->wrole_idx = wrole_idx;
		grp->snd_sts = PHL_SND_STS_PENDING;
		grp->num_sta = 0;

		/* Primary STA use idx-0 */
		_phl_snd_func_grp_add_sta(phl_info, psta, *gidx);

		snd_param->snd_func_grp_num++;

		pstatus = RTW_PHL_STATUS_SUCCESS;

		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "phl_snd_func_add_snd_grp : Add group[%d] Success\n",
			  *gidx);
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "phl_snd_func_add_snd_grp : grp->snd_type 0x%x\n",
			  grp->snd_type);

	} while (0);

	return pstatus;
}

/**
 * _phl_snd_func_set_grp_fb_mu()
 * 	Set the whole sounding grp's feedback type = MU
 * input:
 * @grp: (struct phl_snd_grp *) the target group.
 */
void _phl_snd_func_set_grp_fb_mu(struct phl_snd_grp *grp)
{
	u8 i = 0;
	if (grp == NULL)
		return;
	for (i = 0; i < grp->num_sta; i++) {
		grp->sta[i].snd_fb_t = PHL_SND_FB_TYPE_MU;
	}
}

/**
 * phl_snd_func_grouping()
 * 	function for soundind fsm state : SND_FUNC_READY
 * input:
 * @wroleidx: the index of wrole which the sounding proc run under with.
 */
enum rtw_phl_status
phl_snd_func_grouping(struct phl_info_t *phl_info, u8 wroleidx)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	struct phl_snd_fix_param *fix_para = &snd->snd_param.fix_param;
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_phl_mld_t *mld_self = NULL;
	struct rtw_phl_stainfo_t *sta_self = NULL, *sta;
	struct phl_snd_grp *grp = NULL;
	void *drv = phl_to_drvpriv(phl_info);
	struct phl_queue *sta_queue;
	u8 gidx = 0;
	u8 cnt = 0, ridx = 0, midx = 0;

	wrole = phl_get_wrole_by_ridx(phl_info, wroleidx);

	/* if wrole(STA) is linked, seft = AP */
	/* if wrole is AP, self = ???? */
	mld_self = rtw_phl_get_mld_self(phl_info, wrole);
	if (mld_self == NULL) {
		PHL_ERR("Cannot get self's phl_sta\n");
		return pstatus;
	}

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, " PHL_RTYPE_STATION == wrole->type \n");

	if (rtw_phl_role_is_client_category(wrole)) {
		/* STA Mode : Only SU TxBF with AP */
		for (midx = 0; midx < mld_self->sta_num; midx++) {
			sta_self = mld_self->phl_sta[midx];
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "sta_self->macid = 0x%x \n", sta_self->macid);
			debug_dump_mac_address(sta_self->mac_addr);

			pstatus = phl_snd_func_add_snd_grp(
					phl_info,
					(sta_self->wmode & WLAN_MD_11AX) ? true :
								       false,
					wrole->id, sta_self, &gidx);
			grp = &snd_param->snd_grp[gidx];
			grp->grp_tier = PHL_SND_GRP_TIER_0;
			grp->sta[0].snd_fb_t = PHL_SND_FB_TYPE_SU;
			grp->snd_type = (sta_self->wmode & WLAN_MD_11AX) ?
					PHL_SND_TYPE_HE_HW : PHL_SND_TYPE_VHT_HW;
		}
	} else {
		/* Test Code: Group-1 :Forced MU Sounding with first 1~4 STAs */
		/* the mu sounding list shall get from mu grouping module */
		for (ridx = 0; ridx < wrole->rlink_num; ridx++) {
			rlink = get_rlink(wrole, ridx);
			sta_queue = &rlink->assoc_sta_queue;
			cnt = 0;

			_os_spinlock(drv, &sta_queue->lock, _bh, NULL);
			phl_list_for_loop(sta, struct rtw_phl_stainfo_t,
					  &sta_queue->queue, list) {
				if (is_broadcast_mac_addr(sta->mac_addr))
					continue;

				for (midx = 0; midx < mld_self->sta_num; midx++) {
					if (sta == mld_self->phl_sta[midx])
						continue;
				}
				/* First STA */
				if (cnt == 0) {
					pstatus = phl_snd_func_add_snd_grp(
							phl_info,
							(sta->wmode & WLAN_MD_11AX) ?
								 true : false,
							wrole->id, sta, &gidx);
					if (pstatus != RTW_PHL_STATUS_SUCCESS)
						break;
				} else {
					/* get next associated sta and add to group */
					_phl_snd_func_grp_add_sta(phl_info, sta, gidx);
				}
				cnt++;
				if (cnt >= 4)
					break;
			}
			_os_spinunlock(drv, &sta_queue->lock, _bh, NULL);

			if(pstatus != RTW_PHL_STATUS_SUCCESS)
				return RTW_PHL_STATUS_FAILURE;

			grp = &snd_param->snd_grp[gidx];
			grp->grp_tier = PHL_SND_GRP_TIER_0;
			/* Test : forced MU */
			_phl_snd_func_set_grp_fb_mu(&snd_param->snd_grp[gidx]);
		}
	}

	/*TODO: fixed paramters gidx when multi-group */
	if (snd_param->test_flag&PHL_SND_TEST_F_GRP_SND_PARA) {
		/*Test Mode force set the group fb type = MU */
		if (fix_para->en_fix_fb_type) {
			if (PHL_SND_FB_TYPE_MU == fix_para->snd_fb_type) {
				_phl_snd_func_set_grp_fb_mu(
						&snd_param->snd_grp[gidx]);
			}
			/**
			 * Note : 8852A only support two CSI Buffer for SU,
			 * take care that num of STA in SU sounding of a group shall < 2.
			 **/
		}

		if(fix_para->en_fix_snd_bw) {
			grp = &snd_param->snd_grp[gidx];
			for (cnt = 0; cnt < MAX_NUM_STA_SND_GRP; cnt++) {
				if (grp->sta[cnt].valid)
					grp->sta[cnt].bw = fix_para->bw[cnt];
			}
		}
	} else {
		grp = &snd_param->snd_grp[gidx];
		if (grp->num_sta > 2) {
			/* forced using MU feedback because of SU CSI buffer number */
			_phl_snd_func_set_grp_fb_mu(&snd_param->snd_grp[gidx]);
		}
	}

	if (snd_param->test_flag & PHL_SND_TEST_F_GRP_EN_BF_FIX) {
		snd_param->snd_grp[gidx].en_fix_mode = 1; /* post confg forced mode setting */
	}
	return pstatus;
}

/* SND PROC */

/* Free BF/CQI resource */
enum rtw_phl_status
_phl_snd_proc_release_res_cqi(
	struct phl_info_t *phl_info, struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;

	/*CQI Fb doesn't query any resource*/

	return pstatus;
}

enum rtw_phl_status
_phl_snd_proc_release_res_bf(
	struct phl_info_t *phl_info, struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct phl_snd_sta *snd_sta;
	struct rtw_phl_stainfo_t *sta = NULL;
	u8 idx = 0;

	for (idx = 0; idx < grp->num_sta; idx++) {
		snd_sta = &grp->sta[idx];
		if(0 == snd_sta->valid)
			continue;

		sta = rtw_phl_get_stainfo_by_macid(
				phl_info, snd_sta->macid);
		if (NULL == sta) {
			PHL_ERR("_phl_snd_proc_release_res_bf: Cannot find STA macid 0x%x in PHL STA Info List \n",
				 snd_sta->macid);
			continue;
		}
		if (NULL == sta->hal_sta->bf_entry)
			continue;

		hstatus = rtw_hal_snd_release_proc_sta_res(phl_info->hal, sta);
		if(hstatus != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("_phl_snd_proc_release_res_bf: macid 0x%x Free Sounding Resource FAIL \n",
				 snd_sta->macid);
			continue;
		}
		/* un link the bf entry to STA info */
		sta->hal_sta->bf_entry = NULL;
	}
	return pstatus;
}
/**
 * phl_snd_proc_release_res:
 * 	Release the sounding resource for the group
 * @phl_info: phl_info_t
 * @grp: (struct phl_snd_grp *) sounding gorup for release resource
 **/
enum rtw_phl_status
phl_snd_proc_release_res(struct phl_info_t *phl_info, struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_snd_sta *snd_sta;
	snd_sta = &grp->sta[0];
	if (snd_sta->snd_fb_t == PHL_SND_FB_TYPE_CQI)
		pstatus = _phl_snd_proc_release_res_cqi(phl_info, grp);
	else
		pstatus = _phl_snd_proc_release_res_bf(phl_info, grp);

	return pstatus;
}

/**
 * _phl_snd_proc_get_bf_res_cqi_fb:
 * 	CQI Sounding doesn't need BF Reresource
 * @phl_info: phl_info_t
 * @grp: (struct phl_sound_grp *) sounding gorup
 * @nsta: return value : how many sta query resource success
 **/
enum rtw_phl_status
_phl_snd_proc_get_res_cqi_fb(
	struct phl_info_t *phl_info, struct phl_snd_grp *grp, u8 *nsta)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_snd_sta *snd_sta;
	u8 idx = 0;
	struct rtw_phl_stainfo_t *sta = NULL;

	*nsta = 0;

	for (idx = 0; idx < grp->num_sta; idx++) {
		snd_sta = &grp->sta[idx];
		sta = rtw_phl_get_stainfo_by_macid(phl_info, snd_sta->macid);
		if (NULL == sta) {
			PHL_ERR("phl_snd_proc_get_bf_res: Cannot find STA macid 0x%x in PHL STA Info List \n",
				 snd_sta->macid);
			continue;
		}

		rtw_hal_snd_ndpa_sta_info_he(
			sta,
			&snd_sta->npda_sta_info,
			snd_sta->bw,
			PHL_SND_FB_TYPE_CQI);

		(*nsta)++;
	}
	if (*nsta == 0) {
		grp->snd_sts = PHL_SND_STS_FAILURE;
		pstatus = RTW_PHL_STATUS_FAILURE;
	}
	if (*nsta != grp->num_sta) {
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, " Sounding STAs is fewer than group sta because of resource!");
	}

	return pstatus;
}

/**
 * _phl_snd_proc_get_res_bf:
 * 	Get BF Resource for SU/MU Sounding
 * @phl_info: phl_info_t
 * @grp: (struct phl_sound_grp *) sounding gorup
 * @nsta: return value : how many sta query bf resource success
 **/
enum rtw_phl_status
_phl_snd_proc_get_res_bf(
	struct phl_info_t *phl_info, struct phl_snd_grp *grp, u8 *nsta)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct phl_snd_sta *snd_sta;
	u8 idx = 0;
	struct rtw_phl_stainfo_t *sta = NULL;
	bool mu, qry_bf;

	*nsta = 0;

	for (idx = 0; idx < grp->num_sta; idx++) {
		snd_sta = &grp->sta[idx];
		sta = rtw_phl_get_stainfo_by_macid(phl_info, snd_sta->macid);
		if (NULL == sta) {
			PHL_ERR("phl_snd_proc_get_bf_res: Cannot find STA macid 0x%x in PHL STA Info List \n",
				 snd_sta->macid);
			continue;
		}

		mu = (snd_sta->snd_fb_t == PHL_SND_FB_TYPE_MU) ? true:false;
		qry_bf = false;
		if (sta->hal_sta->bf_entry != NULL) {
			/* The sta already have BF reource */
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, " sta->bf_entry != NULL\n");
			/* Check the BF resource */
			hstatus = rtw_hal_snd_chk_bf_res(phl_info->hal,
							 sta, mu, sta->chandef.bw);
			if (RTW_HAL_STATUS_FAILURE == hstatus) {
				rtw_hal_snd_release_proc_sta_res(phl_info->hal,
								 sta);
				qry_bf = true;

			} else {
				PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "Use Original BF Resource \n");
				qry_bf = false;
			}
		} else {
			qry_bf = true;
		}

		if (true == qry_bf) {
			hstatus = rtw_hal_snd_query_proc_sta_res(
					phl_info->hal, sta, mu, sta->chandef.bw,
					grp->en_swap_mode);
			if (hstatus != RTW_HAL_STATUS_SUCCESS) {
				PHL_ERR("phl_snd_proc_get_bf_res: macid 0x%x query sounding resource FAIL \n",
				snd_sta->macid);
				if (grp->en_swap_mode) {
					break;/* break in swap mode if one of sta query bf res fail */
				}
				continue;
			}
		}
		if (grp->snd_type >= PHL_SND_TYPE_HE_HW) {
			rtw_hal_snd_ndpa_sta_info_he(
				sta,
				&snd_sta->npda_sta_info,
				snd_sta->bw,
				snd_sta->snd_fb_t);
		} else {
			rtw_hal_snd_ndpa_sta_info_vht(sta,
				&snd_sta->npda_sta_info, mu);
		}

		/* Link STA information to Group Information */
		snd_sta->bf_entry = sta->hal_sta->bf_entry;

		(*nsta)++;
	}

	if (*nsta == 0) {
		grp->snd_sts = PHL_SND_STS_FAILURE;
		pstatus = RTW_PHL_STATUS_FAILURE;
	}
	if (*nsta != grp->num_sta) {
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "FAIL : Sounding STAs is fewer than group sta because of resource!\n");
		pstatus = RTW_PHL_STATUS_FAILURE;
	}

	return pstatus;
}
/* snd proc get BF/CSI resource */
enum rtw_phl_status
phl_snd_proc_get_res(
	struct phl_info_t *phl_info, struct phl_snd_grp *grp, u8 *nsta)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct phl_snd_sta *snd_sta;
	FUNCIN_WSTS(pstatus);

	snd_sta = &grp->sta[0];
	/* CQI Fb cannot mixed with SU/MU feedback type*/
	if(snd_sta->snd_fb_t == PHL_SND_FB_TYPE_CQI)
		pstatus = _phl_snd_proc_get_res_cqi_fb(phl_info, grp, nsta);
	else
		pstatus = _phl_snd_proc_get_res_bf(phl_info, grp, nsta);

	if(pstatus != RTW_PHL_STATUS_SUCCESS)
		grp->snd_sts = PHL_SND_STS_FAILURE;

	FUNCOUT_WSTS(pstatus);
	return pstatus;
}

/* 2. SND Preconfiguration */

/**
 * _get_mu_mimo_gid_2sta()
 * 	hard code for 8852A, gid relattion-ship
 **/
static u8 _get_mu_mimo_gid_2sta(u8 primary, u8 secondary)
{
	u8 gid_tbl[6][6] = { {0xFF, 1, 2, 3, 4, 5},
					{16, 0xFF, 6, 7, 8, 9},
					{17, 21, 0xFF, 10, 11, 12},
					{18, 22, 25, 0xFF, 13, 14},
					{19, 23, 26, 28, 0xFF, 15},
					{20, 24, 27, 29, 30, 0xFF} };
	u8 ret = 0xFF;

	if ((primary < 6) && (secondary < 6))
		ret = gid_tbl[primary][secondary];

	return ret;
}
/* pre calculate mu-gid */
enum rtw_phl_status
phl_snd_cal_mu_grp_bitmap(struct phl_info_t *phl_info, struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_stainfo_t *psta_info = NULL;
	struct rtw_phl_stainfo_t *tmp_psta_info = NULL;
	struct phl_snd_sta *sta = NULL;
	struct phl_snd_sta *tmp_sta = NULL;
	u8 bfmu_idx , bfmu_idx_tmp;
	u8 i = 0, j = 0;

	for (i = 0; i < MAX_NUM_STA_SND_GRP; i++) {
		sta = &grp->sta[i];
		if (false == sta->valid)
			continue;
		/* primary STA */
		psta_info = rtw_phl_get_stainfo_by_macid(
				phl_info, sta->macid);

		if (NULL == sta->bf_entry)
			continue;
		if (false == rtw_hal_bf_chk_bf_type(phl_info->hal,
				psta_info, true)) {
			continue; /*BF SU Entry*/
		}
		/* primary STA MU Entry Idx */
		bfmu_idx = rtw_hal_bf_get_sumu_idx(phl_info->hal,
							sta->bf_entry);

		psta_info->hal_sta->mugrp_bmp = 0; /* clear first */

		for (j = 0; j < MAX_NUM_STA_SND_GRP; j++) {
			if (j == i) /* self */
				continue;
			/* secondary sta */
			tmp_sta = &grp->sta[j];

			if (NULL == tmp_sta->bf_entry)
				continue;

			tmp_psta_info = rtw_phl_get_stainfo_by_macid(
					phl_info, tmp_sta->macid);

			if (false == rtw_hal_bf_chk_bf_type(phl_info->hal,
				tmp_psta_info, true)) {
				continue; /* BF SU Entry */
			}

			/* secondary sta MU Entry Idx */
			bfmu_idx_tmp = rtw_hal_bf_get_sumu_idx(phl_info->hal,
						tmp_sta->bf_entry);

			/* Default set group bit enable = 1 */
			/* grp bitmap doesn't include self */
			/**   BIT    0     1     2     3     4
			 *  MU_0 : MU_1  MU_2  MU_3  MU_4  MU_5
			 *  MU_1 : MU_0  MU_2  MU_3  MU_4  MU_5
			 *  MU_2 : MU_0  MU_1  MU_3  MU_4  MU_5
			 *  ...
			 *  MU_5 : MU_0  MU_1  MU_2  MU_3  MU_4
			 **/

			if (bfmu_idx_tmp > bfmu_idx) {
				psta_info->hal_sta->mugrp_bmp |=
							BIT(bfmu_idx_tmp - 1);
			} else {
				psta_info->hal_sta->mugrp_bmp |=
							BIT(bfmu_idx_tmp);
			}
		}
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "sta(macid = 0x%x)  mugrp_bmp = 0x%x \n",
			 psta_info->macid, psta_info->hal_sta->mugrp_bmp);
	}

	return pstatus;
}

/* Preconfiguration before souding */
enum rtw_phl_status
phl_snd_proc_precfg(struct phl_info_t *phl_info, struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct phl_snd_sta *sta = NULL;
	u8 idx = 0;
	struct rtw_phl_stainfo_t *psta_info = NULL;

	FUNCIN_WSTS(pstatus);
	do {
		if (grp == NULL) {
			pstatus = RTW_PHL_STATUS_FAILURE;
			break;
		}
		if (PHL_SND_TYPE_INVALID == grp->snd_type) {
			/* both SW/HW mode need to set call halmac api to set bf entry */
			break;
		}
		for (idx = 0; idx < MAX_NUM_STA_SND_GRP; idx++) {
			sta = &grp->sta[idx];
			if (false == sta->valid)
				continue;
			psta_info = rtw_phl_get_stainfo_by_macid(
						phl_info, sta->macid);

			/*check bf entry available and snd_fb_type = SU/MU */
			if ((NULL != psta_info->hal_sta->bf_entry) &&
				(PHL_SND_FB_TYPE_CQI != sta->snd_fb_t)) {

				hstatus = rtw_hal_snd_proc_pre_cfg_sta(
						phl_info->hal, psta_info);

				if (hstatus != RTW_HAL_STATUS_SUCCESS) {
					pstatus = RTW_PHL_STATUS_FAILURE;
				}
			}
		}

		/* config for each sounding sequence */
		if (RTW_HAL_STATUS_SUCCESS != rtw_hal_snd_proc_pre_cfg(phl_info->hal)) {
				pstatus = RTW_PHL_STATUS_FAILURE;
		}

		/* Prepare Group bitmap for Tx MU-MIMO */
		if (PHL_SND_FB_TYPE_MU == grp->sta[0].snd_fb_t)
			pstatus = phl_snd_cal_mu_grp_bitmap(phl_info, grp);

	} while (0);


	if(pstatus != RTW_PHL_STATUS_SUCCESS)
		grp->snd_sts = PHL_SND_STS_FAILURE;

	FUNCOUT_WSTS(pstatus);
	return pstatus;
}
/* 3. Send Sounding Command to HAL/FW */
/*TODO: RU Allocation is now hard code value */
/* HE TB Sounding : 2 sta in a grp */
void
_phl_snd_proc_fw_cmd_he_tb_2sta(struct phl_info_t *phl_info,
				struct phl_snd_grp *grp,
				u8 *cmd, u8 bfrp_num)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct rtw_phl_stainfo_t *sta_info = NULL;
	u8 *f_ru_tbl = NULL;

	if (grp->num_sta != 2)
		return;
	/* get first sta */
	sta_info = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[0].macid);

	if (bfrp_num == 1) {
		if (CHANNEL_WIDTH_20 == grp->sta[0].bw)
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_20[1][0];/* Fixed 20MHz RU Table of 2 STA */
		else
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_80[1][0];/* Fixed 80MHz RU Table of 2 STA */
	} else {
		if (CHANNEL_WIDTH_20 == grp->sta[0].bw)
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_20[0][0];/* Fixed 20MHz RU Table of 1 STA */
		else
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_80[0][0];/* Fixed 80MHz RU Table of 1 STA */
	}

	/* fill commmand */
	rtw_hal_snd_ax_fwcmd_tb_pri(phl_info->hal, cmd, grp->sta[0].bw,
		sta_info, grp->num_sta, 0);
	/* Always use BFRP#0 for primary user */
	rtw_hal_snd_ax_fwcmd_tb_add_sta(
			phl_info->hal, cmd,
			&grp->sta[0].npda_sta_info,
			sta_info,
			f_ru_tbl[0],
			0,
			0,
			0);

	/*get second sta*/
	sta_info = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[1].macid);

	rtw_hal_snd_ax_fwcmd_tb_add_sta(
			phl_info->hal, cmd,
			&grp->sta[1].npda_sta_info,
			sta_info,
			f_ru_tbl[1],
			1,
			(bfrp_num == 1) ? 0 : 1,
			(bfrp_num == 1) ? 1 : 0);

}

/* HE TB Sounding : 3 sta in a grp */
void
_phl_snd_proc_fw_cmd_he_tb_3sta(struct phl_info_t *phl_info,
				struct phl_snd_grp *grp,
				u8 *cmd, u8 bfrp_num)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct rtw_phl_stainfo_t *sta_info = NULL;
	u8 *f_ru_tbl = NULL;

	if(grp->num_sta != 3)
		return;
	/* get first sta */
	sta_info = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[0].macid);

	if (bfrp_num == 1) {
		if (CHANNEL_WIDTH_20 == grp->sta[0].bw)
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_20[2][0];/* Fixed 20MHz RU Table of 3 STA */
		else
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_80[2][0];/* Fixed 80MHz RU Table of 3 STA */
	} else {
		if (CHANNEL_WIDTH_20 == grp->sta[0].bw)
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_20[1][0];/* Fixed 20MHz RU Table of 2 STA */
		else
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_80[1][0];/* Fixed 80MHz RU Table of 2 STA */
	}

	/* fill commmand */
	rtw_hal_snd_ax_fwcmd_tb_pri(phl_info->hal, cmd, grp->sta[0].bw,
		sta_info, grp->num_sta, 0);
	/* Always use BFRP#0 for primary user */
	rtw_hal_snd_ax_fwcmd_tb_add_sta(
			phl_info->hal, cmd,
			&grp->sta[0].npda_sta_info,
			sta_info,
			f_ru_tbl[0],
			0,
			0,
			0);

	/*get second sta*/
	sta_info = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[1].macid);

	rtw_hal_snd_ax_fwcmd_tb_add_sta(
			phl_info->hal, cmd,
			&grp->sta[1].npda_sta_info,
			sta_info,
			f_ru_tbl[1],
			1,
			0,
			1);
	/*get third sta*/
	sta_info = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[2].macid);

	rtw_hal_snd_ax_fwcmd_tb_add_sta(
			phl_info->hal, cmd,
			&grp->sta[2].npda_sta_info,
			sta_info,
			f_ru_tbl[2],
			2,
			(bfrp_num == 1) ? 0 : 1,
			(bfrp_num == 1) ? 2 : 0);

}

/* HE TB Sounding : 4 sta in a grp */
void
_phl_snd_proc_fw_cmd_he_tb_4sta(struct phl_info_t *phl_info,
				struct phl_snd_grp *grp,
				u8 *cmd, u8 bfrp_num)
{
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct rtw_phl_stainfo_t *sta_info = NULL;
	u8 *f_ru_tbl = NULL;

	if(grp->num_sta != 4)
		return;
	/* get first sta */
	sta_info = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[0].macid);

	if (bfrp_num == 1) {
		if (CHANNEL_WIDTH_20 == grp->sta[0].bw)
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_20[3][0];/* Fixed 20MHz RU Table of 4 STA */
		else
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_80[3][0];/* Fixed 80MHz RU Table of 4 STA */
	} else {
		if (CHANNEL_WIDTH_20 == grp->sta[0].bw)
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_20[1][0];/* Fixed 20MHz RU Table of 2 STA */
		else
			f_ru_tbl = &snd->snd_param.fix_param.f_ru_tbl_80[1][0];/* Fixed 80MHz RU Table of 2 STA */
	}

	/* fill commmand */
	rtw_hal_snd_ax_fwcmd_tb_pri(phl_info->hal, cmd, grp->sta[0].bw,
		sta_info, grp->num_sta, 0);
	/* Always use BFRP#0 for primary user */
	rtw_hal_snd_ax_fwcmd_tb_add_sta(
			phl_info->hal, cmd,
			&grp->sta[0].npda_sta_info,
			sta_info,
			f_ru_tbl[0],
			0,
			0,
			0);

	/*get second sta*/
	sta_info = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[1].macid);

	rtw_hal_snd_ax_fwcmd_tb_add_sta(
			phl_info->hal, cmd,
			&grp->sta[1].npda_sta_info,
			sta_info,
			f_ru_tbl[1],
			1,
			(bfrp_num == 1) ? 0 : 0,
			(bfrp_num == 1) ? 1 : 1);
	/*get third sta*/
	sta_info = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[2].macid);

	rtw_hal_snd_ax_fwcmd_tb_add_sta(
			phl_info->hal, cmd,
			&grp->sta[2].npda_sta_info,
			sta_info,
			f_ru_tbl[2],
			2,
			(bfrp_num == 1) ? 0 : 1,
			(bfrp_num == 1) ? 2 : 0);

	/*get 4th sta*/
	sta_info = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[3].macid);

	rtw_hal_snd_ax_fwcmd_tb_add_sta(
			phl_info->hal, cmd,
			&grp->sta[3].npda_sta_info,
			sta_info,
			f_ru_tbl[3],
			3,
			(bfrp_num == 1) ? 0 : 1,
			(bfrp_num == 1) ? 3 : 1);

}


enum rtw_phl_status
phl_snd_proc_start_sounding_fw(struct phl_info_t *phl_info,
			       struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct phl_sound_param *snd_param = &snd->snd_param;
	struct rtw_phl_stainfo_t *sta_info = NULL;
	u8 *cmd = NULL;
	u8 i = 0;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "==> phl_snd_proc_start_sounding_fw \n");
	do {
		if (NULL == grp)
			break;
		if(grp->sta[0].valid == 0)
			break;

		/*get first sta*/
		sta_info = rtw_phl_get_stainfo_by_macid(
					phl_info, grp->sta[0].macid);

		switch (grp->snd_type) {
		case PHL_SND_TYPE_VHT_SW:
		{
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "--> PHL_SND_TYPE_VHT_SW\n");
			cmd = rtw_hal_snd_prepare_snd_cmd(phl_info->hal);
			if (cmd == NULL)
				break;
			if (grp->num_sta == 1) {
				rtw_hal_snd_vht_fwcmd_su(
						phl_info->hal, cmd,
						grp->sta[0].bw,
						sta_info,
						&grp->sta[0].npda_sta_info);

			} else {
				rtw_hal_snd_vht_fwcmd_mu_pri(
						phl_info->hal, cmd,
						grp->sta[0].bw,
						sta_info,
						grp->num_sta,
						&grp->sta[0].npda_sta_info);

				for (i = 1; i < grp->num_sta; i++) {
					if(grp->sta[i].valid == 0)
						break;
					sta_info = rtw_phl_get_stainfo_by_macid(
						phl_info, grp->sta[i].macid);

					rtw_hal_snd_vht_fwcmd_mu_add_sta(
						phl_info->hal, cmd,
						&grp->sta[i].npda_sta_info,
						sta_info,
						i,
						(i==(grp->num_sta-1)) ? 1 : 0
						);
				}
			}
			rtw_hal_snd_set_fw_cmd_dialogtkn(
					phl_info->hal, cmd,
					0,
					snd_param->snd_dialog_token);
			hstatus = rtw_hal_snd_send_fw_cmd(phl_info->hal, cmd);
			if (hstatus != RTW_HAL_STATUS_SUCCESS) {
				PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
					  "ERROR: rtw_hal_snd_send_fw_cmd Fail!!!!\n");
			}
			/* free cmd buf at last !!! */
			hstatus = rtw_hal_snd_release_snd_cmd(phl_info->hal, cmd);
		}
		break;
		case PHL_SND_TYPE_HE_SW:
		{
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "--> PHL_SND_TYPE_HE_SW\n");
			cmd = rtw_hal_snd_prepare_snd_cmd(phl_info->hal);
			if (cmd == NULL)
				break;
			if (grp->num_sta == 1) {
				rtw_hal_snd_ax_fwcmd_nontb(
					phl_info->hal, cmd,
					grp->sta[0].bw,
					sta_info,
					&grp->sta[0].npda_sta_info);
			} else {
				/* Default use only 1 BFRP */
				/* TODO: Fixed mode or when to use 2 BFRP */
				if (grp->num_sta == 4)
					_phl_snd_proc_fw_cmd_he_tb_4sta(
							phl_info, grp, cmd, 1);
				else if (grp->num_sta == 3)
					_phl_snd_proc_fw_cmd_he_tb_3sta(
							phl_info, grp, cmd, 1);
				else if (grp->num_sta == 2)
					_phl_snd_proc_fw_cmd_he_tb_2sta(
							phl_info, grp, cmd, 1);
				else
					PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "phl sounding : 1 sta with HE-TB case is NOT Ready ; need add fake sta into NDPA\n");
			}
			rtw_hal_snd_set_fw_cmd_dialogtkn(
					phl_info->hal, cmd,
					1,
					snd_param->snd_dialog_token);

			hstatus = rtw_hal_snd_send_fw_cmd(phl_info->hal, cmd);
			if (hstatus != RTW_HAL_STATUS_SUCCESS) {
				PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
					  "ERROR: rtw_hal_snd_send_fw_cmd Fail!!!!\n");
			}
			/* free cmd buf at last !!! */
			hstatus = rtw_hal_snd_release_snd_cmd(phl_info->hal, cmd);
		}
		break;
		case PHL_SND_TYPE_VHT_HW:
		{
			u8 dialog_tkn = (snd->snd_param.snd_dialog_token << 2);
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
				  "PHL_SND_TYPE_VHT_HW:\n");
			if(NULL == snd->ops.snd_send_ndpa)
				break;

			rtw_hal_snd_mac_ctrl(phl_info->hal, sta_info->rlink->hw_band, 0);
			pstatus = snd->ops.snd_send_ndpa(
					phl_to_drvpriv(phl_info),
					sta_info->rlink,
					&dialog_tkn,
					&grp->sta[0].npda_sta_info,
					grp->sta[0].bw);
		}
		break;
		case PHL_SND_TYPE_HE_HW:
		{
			u8 dialog_tkn = (snd->snd_param.snd_dialog_token << 2) | BIT(1);
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
				  "PHL_SND_TYPE_HE_HW:\n");
			if(NULL == snd->ops.snd_send_ndpa)
				break;

			rtw_hal_snd_mac_ctrl(phl_info->hal, sta_info->rlink->hw_band, 0);
			pstatus = snd->ops.snd_send_ndpa(
					phl_to_drvpriv(phl_info),
					sta_info->rlink,
					&dialog_tkn,
					&grp->sta[0].npda_sta_info,
					grp->sta[0].bw);
		}
		break;
		case PHL_SND_TYPE_INVALID:
		default:
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "ERROR: grp->snd_type invalid\n");
			break;
		}
		snd->snd_param.snd_dialog_token++;
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} while (0);
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "<== phl_snd_proc_start_sounding_fw \n");
	return pstatus;
}

/* 4. Post Configruation */

/* BY MU_GID if MU Sounding */
enum rtw_phl_status
_phl_snd_proc_postcfg_mu_gid(struct phl_info_t *phl_info,
					struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct rtw_phl_stainfo_t *psta_info = NULL;
	struct phl_snd_sta *sta = NULL;
	struct rtw_hal_muba_info ba_info;
	u8 i = 0, j = 0;
	u8 bfmu_idx;
	u8 mugrp_bmp = 0;
	u8 gid = 0xFF;


	for (i = 0; i < MAX_NUM_STA_SND_GRP; i++) {
		sta = &grp->sta[i];
		if((false == sta->valid) || (NULL == sta->bf_entry))
			continue;

		bfmu_idx = rtw_hal_bf_get_sumu_idx(phl_info->hal,
					sta->bf_entry);

		psta_info = rtw_phl_get_stainfo_by_macid(
					phl_info, sta->macid);

		mugrp_bmp = psta_info->hal_sta->mugrp_bmp;

		/* GID(X + Y)'s setting is same as GID(Y + X)*/
		for (j = bfmu_idx; j < 5; j++) {
			if (mugrp_bmp & BIT(j)) {
				gid = _get_mu_mimo_gid_2sta(bfmu_idx, j + 1);
				/*Prepare MU BAR Info*/
				rtw_hal_bf_preset_mu_ba_info(phl_info->hal,
							psta_info, &ba_info);
				PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "snd_post_cfg : gid = 0x%x \n", gid);
				hstatus = rtw_hal_snd_proc_post_cfg_gid(
						phl_info->hal,
						gid,
						(void *)&ba_info);

				if (RTW_HAL_STATUS_SUCCESS != hstatus) {
					pstatus = RTW_PHL_STATUS_FAILURE;
				}
			}
		}
	}

	return pstatus;
}
/* Per STA setting */
enum rtw_phl_status
_phl_snd_proc_postcfg_sta(struct phl_info_t *phl_info,
				struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct rtw_phl_stainfo_t *psta_info = NULL;
	struct phl_snd_sta *sta = NULL;
	u8 idx = 0;
	bool mu = false;

	/*post config for a single sta*/
	for (idx = 0; idx < MAX_NUM_STA_SND_GRP; idx++) {
		sta = &grp->sta[idx];
		mu = (sta->snd_fb_t == PHL_SND_FB_TYPE_MU) ? true : false;
		if (false == sta->valid)
			continue;

		psta_info = rtw_phl_get_stainfo_by_macid(phl_info, sta->macid);
		if (NULL == psta_info)
			continue;

		rtw_hal_snd_polling_snd_sts(phl_info->hal, psta_info);
		if (RTW_HAL_STATUS_SUCCESS ==
			rtw_hal_bf_get_entry_snd_sts(psta_info->hal_sta->bf_entry)) {
			sta->snd_sts = PHL_SND_STS_SUCCESS;
		} else {
			sta->snd_sts = PHL_SND_STS_FAILURE;
		}

		if ((PHL_SND_STS_SUCCESS != sta->snd_sts) &&
			(false == snd->snd_param.bypass_snd_sts_chk)) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "SKIP STA Post Config because of sounding fail\n");
			continue; /*Sounding Fail, Next STA */
		}

		hstatus = rtw_hal_snd_proc_post_cfg_sta(phl_info->hal,
							psta_info, mu);

		if (hstatus != RTW_HAL_STATUS_SUCCESS) {
			pstatus = RTW_PHL_STATUS_FAILURE;
		}
	}

	return pstatus;
}
/* SND PROC Post Config API for FSM */
enum rtw_phl_status
phl_snd_proc_postcfg(struct phl_info_t *phl_info, struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	bool mu = false, he = true;

	FUNCIN();

	do {
		if (grp == NULL) {
			pstatus = RTW_PHL_STATUS_FAILURE;
			break;
		}
		he = (grp->snd_type >= PHL_SND_TYPE_HE_HW) ? true : false;
		mu = (grp->sta[0].snd_fb_t == PHL_SND_FB_TYPE_MU) ? true :
								    false;

		/* 1. post config for whole sounding group */
		if (grp->skip_post_cfg & BIT(1)) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "SKIP SND PROC POST CFG - Group \n");
		} else {
			hstatus = rtw_hal_snd_proc_post_cfg(
						phl_info->hal,
						he,
						mu,
						grp->en_fix_mode);
			if (hstatus != RTW_HAL_STATUS_SUCCESS) {
				pstatus = RTW_PHL_STATUS_FAILURE;
			}
		}

		/* 2. post config for gid (STA + STA) */
		if (grp->skip_post_cfg & BIT(2)) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "SKIP SND PROC POST CFG - GID \n");
		} else {
			if (true == mu) {
				/* only mu sounding has gid related config */
				_phl_snd_proc_postcfg_mu_gid(phl_info, grp);
			}
		}

		/* 3. (Shall always at last) post config for each STA in group */
		if (grp->skip_post_cfg & BIT(3)) {
			PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "SKIP SND PROC POST CFG - STA \n");
		} else {
			_phl_snd_proc_postcfg_sta(phl_info, grp);
		}

	} while (0);

	FUNCOUT();
	return pstatus;
}

/* SND_PROC_DOWN --> Next Sounding : Check sounding module status */
enum rtw_phl_status
phl_snd_proc_chk_condition(struct phl_info_t *phl_info, struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_sound_obj *snd = (struct phl_sound_obj *)phl_info->snd_obj;
	struct rtw_wifi_role_t *role = NULL;
	struct phl_snd_sta *sta = NULL;
	struct rtw_phl_mld_t *mld = NULL;
	struct rtw_phl_stainfo_t *psta = NULL;
	struct phl_sound_param *para = &snd->snd_param;
	u8 i = 0;
	u8 terminate = 0;
	u8 idx = 0;

	/* TODO: Add any conditions to stop the sounding fsm here */
	do {
		if (true == snd->is_terminated)
			break;

		/* get role from group */
		role = phl_get_wrole_by_ridx(phl_info, grp->wrole_idx);

		if (NULL != role) {
			if (rtw_phl_role_is_client_category(role)) {
				if (MLME_NO_LINK == role->mstate)
					goto exit;

				mld = rtw_phl_get_mld_self(phl_info, role);
				for (idx = 0; idx < role->rlink_num; idx++) {
					psta = mld->phl_sta[idx];
					if (rtw_hal_bf_get_entry_snd_sts(
							psta->hal_sta->bf_entry)) {
						para->snd_fail_counter++;
						if (para->snd_fail_counter > 10) {
							PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_ ,
								  "Sounding Fail Count > 10, break sounding !!!!\n");
							goto exit;
						}
					} else {
						para->snd_fail_counter = 0;
					}
				}
			} else if (rtw_phl_role_is_ap_category(role)) {
				if (false == role->active)
					goto exit;

				for (idx = 0; idx < role->rlink_num; idx++) {
					if (grp->sta[0].bw > role->rlink[idx].chandef.bw)
						goto exit;
				}

				if (0 == grp->num_sta)
					goto exit;

				for (i = 0; i < grp->num_sta; i++) {
					sta = &grp->sta[i];
					psta = rtw_phl_get_stainfo_by_macid(phl_info, sta->macid);
					if (NULL == psta) {
						terminate = 1;
						goto exit;
					}
					if (false == psta->active) {
						terminate = 1;
						goto exit;
					}
					if (sta->bw != psta->chandef.bw) {
						terminate = 1;
						goto exit;
					}
				}
				if(terminate)
					goto exit;
			}
		}

		pstatus = RTW_PHL_STATUS_SUCCESS;
	} while (0);

exit:
	return pstatus;
}



/**
 * Check the previous sounding group sounding status and free the resource.
 * if grp is TIER0 grp, skip release BF/CQI resource.
 **/
void
phl_snd_proc_chk_prev_grp(struct phl_info_t *phl_info,
			  struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	bool free_res = false;

	if (PHL_SND_STS_FAILURE == grp->snd_sts) {
		/* Sounding Fail */
		free_res = true;
	} else if ((PHL_SND_GRP_TIER_1 == grp->grp_tier) && (PHL_SND_STS_PENDING != grp->snd_sts)) {
		/* Sounding Success and Group is TIER_1 */
		free_res = true;
	}

	if (free_res) {
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "Free Previous SND Group's Resource\n");
		pstatus = phl_snd_proc_release_res(phl_info, grp);
	}

	return;
}

enum rtw_phl_status
phl_snd_polling_pri_sta_sts(struct phl_info_t *phl_info,
			    struct phl_snd_grp *grp)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rtw_phl_stainfo_t *sta = NULL;

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
		  "phl_snd_polling_stutus : polling primay sta sounding status\n");
	sta = rtw_phl_get_stainfo_by_macid(phl_info, grp->sta[0].macid);
	if (sta != NULL) {
		if (sta->active == true)
			rtw_hal_snd_polling_snd_sts(phl_info->hal, sta);
		else
			pstatus = RTW_PHL_STATUS_FAILURE;
	} else {
		pstatus = RTW_PHL_STATUS_FAILURE;
	}

	return pstatus;
}

void
phl_snd_mac_ctrl(struct phl_info_t *phl_info,
		 enum phl_band_idx band, u8 ctrl)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;

	hstatus = rtw_hal_snd_mac_ctrl(phl_info->hal, band, ctrl);
}

enum rtw_phl_status
rtw_phl_snd_init_ops_send_ndpa(void *phl,
                               enum rtw_phl_status (*snd_send_ndpa)(void *,
                                                                    struct rtw_wifi_role_link_t *,
                                                                    u8 *,
                                                                    u32 *,
                                                                    enum channel_width))
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_sound_obj *snd = NULL;
	if((phl_info != NULL) && (snd_send_ndpa != NULL)) {
		if (phl_info->snd_obj != NULL) {
			snd = (struct phl_sound_obj *)phl_info->snd_obj;
			snd->ops.snd_send_ndpa = snd_send_ndpa;
			pstatus = RTW_PHL_STATUS_SUCCESS;
		}
	}
	return pstatus;
}

#else

enum rtw_phl_status
rtw_phl_snd_init_ops_send_ndpa(void *phl, void *snd_send_ndpa)
{
	return RTW_PHL_STATUS_SUCCESS;
}

u8 rtw_phl_snd_chk_in_progress(void *phl)
{
	return 0;
}

enum rtw_phl_status
rtw_phl_snd_add_grp(void *phl,
                    struct rtw_wifi_role_link_t *rlink,
                    u8 gidx,
                    u16 *macid,
                    u8 num_sta,
                    bool he,
                    bool mu)
{
	return RTW_PHL_STATUS_SUCCESS;
}
enum rtw_phl_status
rtw_phl_sound_start_ex(void *phl, u8 wrole_idx, u8 st_dlg_tkn, u8 period, u8 test_flag)
{
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
rtw_phl_sound_start(void *phl, u8 wrole_idx, u8 st_dlg_tkn, u8 period, u8 test_flag)
{
	return RTW_PHL_STATUS_SUCCESS;
}


enum rtw_phl_status
rtw_phl_sound_abort(void *phl)
{
	return RTW_PHL_STATUS_SUCCESS;
}

#endif