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
#define _HAL_SOUND_C_

#include "hal_headers.h"

#ifdef CONFIG_PHL_CMD_BF
struct csi_rpt_na {
	u8 nr;
	u8 nc;
	u8 na;
};

#define CSI_NA_MATRIX_SIZE 35
static const struct csi_rpt_na csi_na[CSI_NA_MATRIX_SIZE] =
{
	{2, 1, 2}, {2, 2, 2},
	{3, 1, 4}, {3, 2, 6}, {3, 3, 6},
	{4, 1, 6}, {4, 2, 10}, {4, 3, 12}, {4, 4, 12},
	{5, 1, 8}, {5, 2, 14}, {5, 3, 18}, {5, 4, 20}, {5, 5, 20},
	{6, 1, 10}, {6, 2, 18}, {6, 3, 24}, {6, 4, 28}, {6, 5, 30},
	{6, 6, 30},
	{7, 1, 12}, {7, 2, 22}, {7, 3, 30}, {7, 4, 36},
	{7, 5, 40}, {7, 6, 42}, {7, 7, 42},
	{8, 1, 14}, {8, 2, 26}, {8, 3, 36}, {8, 4, 55},
	{8, 5, 50}, {8, 6, 54}, {8, 7, 56}, {8, 8, 56}
};


u32 _cal_he_csi_size(u8 mu, enum channel_width bw, u8 nr, u8 nc, u8 ng, u8 cb)
{
	u8 na = 0;
	u8 ns = 0;
	u8 i = 0;
	u32 csi_size = 0;
	u8 cb_s = 0;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
		  "%s : mu(%d) ; bw(%d) ; nr(%d) ; nc(%d), ng(%d), cb(%d)\n",
		  __func__, mu, bw, nr, nc, ng ,cb);
	do {
		if (CHANNEL_WIDTH_80 == bw)
			ns = (ng == 4) ? 250 : 64;
		else if(CHANNEL_WIDTH_40 == bw)
			ns = (ng == 4) ? 122 : 32;
		else if(CHANNEL_WIDTH_20 == bw)
			ns = (ng == 4) ? 64 : 20;
		else
			break;

		for (i = 0; i < CSI_NA_MATRIX_SIZE; i++) {
			if ((nr == csi_na[i].nr) && (nc ==csi_na[i].nc)) {
				na = csi_na[i].na;
				break;
			}
		}
		if (na == 0)
			break;

		if (cb) {
			if(mu)
				cb_s = 8;
			else
				cb_s = 5;
		} else {
			if(mu)
				cb_s = 6;
			else
				cb_s = 3;
		}
	} while(0);

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
		  "%s : na %d ; ns %d ; cb_s %d",__func__, na, ns, cb_s);

	if ((0 != na) && (0 != ns) && (0 != cb_s)) {
		csi_size = ((8 * (u32)nc) +
			    ((u32)na * (u32)cb_s * (u32)ns)) / 8;
		if(mu)
			csi_size += (4 * (u32)nc * (u32)ns) / 8;
	}

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_,
		  "%s : expected he csi report size = %d byte",
		  __func__, csi_size);
	return csi_size;
}

u32 _cal_he_cqi_only_rpt_size(enum channel_width bw, u8 nc)
{
	u32 ret = 0;
	if (CHANNEL_WIDTH_80 == bw)
		ret = (u32)nc * 37;
	else if(CHANNEL_WIDTH_40 == bw)
		ret = (u32)nc * 18;
	else if(CHANNEL_WIDTH_20 == bw)
		ret = (u32)nc * 9;

	return ret;
}

/*1. BF Resource Related*/
u8 _get_bw_ru_end_idx(enum channel_width bw) {
	u8 ru_end_idx = 0;

	switch (bw) {
	case CHANNEL_WIDTH_20:
		ru_end_idx = HAL_NPDA_RU_IDX_END_20MHZ;
	break;
	case CHANNEL_WIDTH_40:
		ru_end_idx = HAL_NPDA_RU_IDX_END_40MHZ;
	break;
	case CHANNEL_WIDTH_80:
		ru_end_idx = HAL_NPDA_RU_IDX_END_80MHZ;
	break;
	case CHANNEL_WIDTH_160:
	case CHANNEL_WIDTH_80_80:
		ru_end_idx = HAL_NPDA_RU_IDX_END_160MHZ;
	default:
	break;
	}
	return ru_end_idx;
}

void _hal_snd_set_default_var(struct hal_snd_obj *snd_obj)
{
	snd_obj->ndpa_xpara.bw = CHANNEL_WIDTH_20;
	snd_obj->ndpa_xpara.rate = RTW_DATA_RATE_OFDM6;
	snd_obj->ndpa_xpara.gi_ltf = RTW_GILTF_LGI_4XHE32;
	snd_obj->ndpa_xpara.stbc = 0;
	snd_obj->ndpa_xpara.ldpc = 0;

	snd_obj->bfrp_xpara.bw = CHANNEL_WIDTH_20;
	snd_obj->bfrp_xpara.rate = RTW_DATA_RATE_OFDM6;
	snd_obj->bfrp_xpara.gi_ltf = RTW_GILTF_LGI_4XHE32;
	snd_obj->bfrp_xpara.stbc = 0;
	snd_obj->bfrp_xpara.ldpc = 0;
}

enum rtw_hal_status
hal_snd_obj_init(void *hal)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_snd_obj *snd_obj = NULL;

	do {
		hal_com->snd_obj = _os_mem_alloc(hal_to_drvpriv(hal_info),
				    sizeof(struct hal_snd_obj));
		if (NULL == hal_com->snd_obj) {
			break;
		}
		snd_obj = hal_com->snd_obj;
		/* preset hal sounding default values */
 		_hal_snd_set_default_var(snd_obj);

		hstatus = RTW_HAL_STATUS_SUCCESS;
	} while (0);
	return hstatus;
}


enum rtw_hal_status
hal_snd_obj_deinit(void *hal)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;

	do {
		if (hal_com->snd_obj == NULL)
			break;
		_os_mem_free(hal_to_drvpriv(hal_info), hal_com->snd_obj,
			     sizeof(struct hal_snd_obj));
		hal_com->snd_obj = NULL;
	} while (0);

	return hstatus;
}

/**
 * rtw_hal_snd_release_proc_sta_res
 * 	free the resource for a STA used in sounding process
 * input:
 * @hal: (struct hal_info_t *)
 * @sta: (struct rtw_phl_stainfo_t *)
 **/
enum rtw_hal_status
rtw_hal_snd_release_proc_sta_res(void *hal, struct rtw_phl_stainfo_t *sta)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_stainfo_t *hal_sta = sta->hal_sta;
	do {
		if (hal_sta->bf_entry != NULL) {
			hal_status = hal_bf_release_target_bf_entry(
					hal_info, hal_sta->bf_entry);
		}

		if (hal_is_csi_buf_valid(hal_info, &hal_sta->bf_csi_buf)) {
			hal_status = hal_csi_release_csi_buf(
					hal_info, &hal_sta->bf_csi_buf);
		}
		if (hal_is_csi_buf_valid(hal_info, &hal_sta->bf_csi_buf_swap)) {
			hal_status = hal_csi_release_csi_buf(
					hal_info, &hal_sta->bf_csi_buf_swap);
		}
	} while(0);

	return hal_status;
}


/**
 * rtw_hal_snd_query_proc_sta_res
 * input:
 * @hal: hal_info
 * @sta: rtw_phl_stainfo_t
 * @mu: is this MU-MIMO STA resource request
 * @bw: enum channel_width sounding bandwidth
 **/
enum rtw_hal_status
rtw_hal_snd_query_proc_sta_res(
	void *hal,
	struct rtw_phl_stainfo_t *sta,
	bool mu,
	enum channel_width bw,
	bool en_swap)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_stainfo_t *hal_sta = sta->hal_sta;

	do {
		if (hal_sta->bf_entry != NULL) {
			PHL_TRACE(COMP_PHL_SOUND,  _PHL_INFO_, "[WARNING] sta->bf_entry != NULL\n");
			/* TODO:Shall Release First ?*/
			/* rtw_hal_snd_release_proc_sta_res(hal, sta); */
		}

		hal_sta->bf_entry =
			(void *)hal_bf_query_idle_bf_entry(hal_info, mu);

		if (hal_sta->bf_entry == NULL)
			break;

		hal_status = hal_csi_query_idle_csi_buf(
				hal_info, mu, bw, &hal_sta->bf_csi_buf);

		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			break;

		if (mu && en_swap) {
			hal_status = hal_csi_query_idle_csi_buf(
				hal_info, mu, bw, &hal_sta->bf_csi_buf_swap);
		}
		if (hal_status == RTW_HAL_STATUS_FAILURE) {
			PHL_INFO("Cannot Enable Swap Mode! Because of the CSI resource is not enougth.\n");
			break;
		}

		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "sta->bf_csi_buf 0x%x \n",
				hal_sta->bf_csi_buf);
		PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "sta->bf_csi_buf_swap 0x%x \n",
				hal_sta->bf_csi_buf_swap);

		/* Set STA-INFO info to BF Entry */
		hal_status = hal_bf_cfg_swbf_entry(sta, en_swap);
	} while (0);

	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		PHL_INFO("rtw_hal_snd_query_proc_sta_res FAIL \n");
		rtw_hal_snd_release_proc_sta_res(hal, sta);
	}

	return hal_status;
}

/**
 * rtw_hal_snd_proc_pre_cfg_sta
 * 	hw preconfiguration for a sounding sta
 * input:
 * @hal: hal_info
 * @sta: (struct rtw_phl_stainfo_t *)
 **/
enum rtw_hal_status
rtw_hal_snd_proc_pre_cfg_sta(
	void *hal,
	struct rtw_phl_stainfo_t *sta)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_stainfo_t *hal_sta = sta->hal_sta;

	/* 1. MAC HW BF Entry Settings */
	hal_status = hal_bf_set_entry_hwcfg(
				hal_info, hal_sta->bf_entry);

	/* 2. Add other HAL setting here */
	/*TODO:*/

	return hal_status;
}

/**
 * rtw_hal_snd_proc_pre_cfg
 * 	hw preconfiguration for a sounding sequence
 * input:
 * @hal: hal_info
 **/
enum rtw_hal_status
rtw_hal_snd_proc_pre_cfg(void *hal)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	/* 1. RF BFer Settings */
	hal_status = rtw_hal_rf_bfer_cfg(hal_info);

	return hal_status;
}

/**
 * rtw_hal_snd_proc_post_cfg
 * 	hw/fw post configuration for a sounding event
 * input:
 * @hal: hal_info
 **/
enum rtw_hal_status
rtw_hal_snd_proc_post_cfg(void *hal, bool he, bool mu, bool en_fixed_mode)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_mu_score_tbl *hal_score_table = &hal_info->hal_com->bb_mu_score_tbl;
	if (mu) {
		/*1. MU Score Board */
		hal_status = rtw_hal_mac_ax_set_mu_table_whole(
				hal_info->mac, hal_score_table);

		/*2. (optional) MU Fixed Mode */
		if (en_fixed_mode)
			hal_status = rtw_hal_bf_set_fix_mode(hal, mu, he);
	}

	return hal_status;
}


/**
 * rtw_hal_snd_proc_post_cfg_gid
 * 	hw/fw post configuration for a vht/he gid
 * input:
 * @hal: hal_info
 * @gid: wifi protolcol gid (PLCP) for configuration
 * @ba_info: pointer of struct rtw_hal_muba_info
 **/
enum rtw_hal_status
rtw_hal_snd_proc_post_cfg_gid(void *hal, u8 gid, void *ba_info)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	/*
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_muba_info *hal_ba_info = (struct rtw_hal_muba_info *)ba_info;
	*/

	/*TODO:*/
	/*1. MU BAR Table : Table ID = GID, GID = STA-x + sSTA-y*/
	return hal_status;
}

/**
 * rtw_hal_snd_proc_post_cfg_sta
 * 	hw/fw post configuration for a single sounding sta
 * input:
 * @hal: hal_info
 * @sta: (struct rtw_phl_stainfo_t *)
 **/
enum rtw_hal_status
rtw_hal_snd_proc_post_cfg_sta(
	void *hal,
	struct rtw_phl_stainfo_t *sta,
	bool mu)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	/* MAC/FW Settings */
	if (mu) {
		/*1. FW MU STA Update */
		hal_status = hal_bf_set_mu_sta_fw(hal, sta);
		/*2. MU Score Board */
		/* mac->mac_set_mu_table_single_sta */
	}

	/* BB/FW Settings */
	/*TODO:*/

	/* 2. Add other setting here */
	/*TODO:*/

	return hal_status;
}

/**
 * rtw_hal_snd_mac_ctrl
 * 	control sounding process : pause or start.
 * @hal: hal_info
 * @band: band0 / band1
 * @ctrl: 0 = pause souning / 1 = start sounding
 **/
enum rtw_hal_status
rtw_hal_snd_mac_ctrl(void *hal, u8 band, u8 ctrl)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	hstatus = rtw_hal_mac_ax_hw_snd_control(hal_info->mac, band, ctrl);

	return hstatus;
}

/**
 * rtw_hal_snd_chk_bf_res
 * 	check the sta's sounding resource is enough for sounding
 * input :
 * @hal: hal_info
 * @sta: (struct rtw_hal_stainfo_t *)
 * @mu: true = mu / false = su
 * @bw: enum channel_width
 * return :
 * @hstatus: RTW_HAL_STATUS_FAILURE = need release and query bf entry and CSI buffer
 *  	     RTW_HAL_STATUS_SUCCESS = STA's BF Entry and CSI Buffer is same to condition.
 **/
enum rtw_hal_status
rtw_hal_snd_chk_bf_res(void *hal, struct rtw_phl_stainfo_t *sta,
	bool mu, enum channel_width bw)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_stainfo_t *hal_sta = sta->hal_sta;
	do {
		if (sta == NULL)
			break;

		if (false == rtw_hal_bf_chk_bf_type(hal, sta, mu))
			break;
		if (mu != rtw_hal_get_csi_buf_type(&hal_sta->bf_csi_buf))
			break;
		if (bw != rtw_hal_get_csi_buf_bw(&hal_sta->bf_csi_buf))
			break;

		hstatus = RTW_HAL_STATUS_SUCCESS;
	} while (0);

	return hstatus;
}

/**
 * rtw_hal_snd_polling_snd_sts
 * 	update the sta's sounding status into sta->hal_sta->bf_entry->bfee
 * input :
 * @hal: hal_info
 * @sta: (struct rtw_hal_stainfo_t *)
 **/
void
rtw_hal_snd_polling_snd_sts(void *hal, struct rtw_phl_stainfo_t *sta)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	do {
		if (sta == NULL)
			break;

		hal_bf_update_entry_snd_sts(hal_info, sta->hal_sta->bf_entry);

	} while (0);
}

/* SND H2C CMD related functions */
void
rtw_hal_snd_ndpa_sta_info_vht(struct rtw_phl_stainfo_t *psta_info,
			      u32 *ndpa, u8 mu)
{
	struct hal_vht_ndpa_sta_info *ndpa_sta =
		(struct hal_vht_ndpa_sta_info *)ndpa;
	u8 num_snd_dim = psta_info->rlink->protocol_cap.num_snd_dim;

	if (rtw_phl_role_is_client_category(psta_info->wrole))
		ndpa_sta->aid12 = 0; /* Target is an AP, AID = 0 */
	else
		ndpa_sta->aid12 = psta_info->aid;
	ndpa_sta->feedback_type	= (mu == 0) ? HAL_NPDA_AC_SU : HAL_NPDA_AC_MU;
	/* Nc shall alwary <= Nr */
	if (psta_info->asoc_cap.max_nc > num_snd_dim)
		ndpa_sta->nc = num_snd_dim;
	else
		ndpa_sta->nc = psta_info->asoc_cap.max_nc;

	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "vht ndpa_sta aid12 0x%x ; fb 0x%x ; nc 0x%x\n",
		  ndpa_sta->aid12, ndpa_sta->feedback_type, ndpa_sta->nc);
}

void
rtw_hal_snd_ndpa_sta_info_he(struct rtw_phl_stainfo_t *psta_info,
			     u32 *ndpa, enum channel_width bw, u8 fb_type)
{

	struct hal_he_ndpa_sta_info *ndpa_sta =
		(struct hal_he_ndpa_sta_info *)ndpa;
	u16 ru_start = HAL_NPDA_RU_IDX_START;
	u16 ru_end = _get_bw_ru_end_idx(bw);
	u8 num_snd_dim_greater_80mhz =
		psta_info->rlink->protocol_cap.num_snd_dim_greater_80mhz;
	u8 num_snd_dim = psta_info->rlink->protocol_cap.num_snd_dim;

	if (rtw_phl_role_is_client_category(psta_info->wrole))
		ndpa_sta->aid = 0; /* Target is and AP, AID = 0 */
	else
		ndpa_sta->aid = (psta_info->aid&0x7FF);
	ndpa_sta->bw = ((ru_start&0x7F) << 0) | ((ru_end&0x7F) << 7);
	if (0 == fb_type) {
		ndpa_sta->fb_ng = psta_info->asoc_cap.ng_16_su_fb ?
			 HAL_NDPA_AX_FB_SU_NG_16 : HAL_NDPA_AX_FB_SU_NG_4;
		ndpa_sta->cb = psta_info->asoc_cap.cb_sz_su_fb ?
			HAL_NPDA_AX_CB_SU42_MU75 : HAL_NPDA_AX_CB_SU64_MU97;
	} else if (1 == fb_type) {
		ndpa_sta->fb_ng = psta_info->asoc_cap.ng_16_mu_fb ?
			HAL_NDPA_AX_FB_MU_NG_16 : HAL_NDPA_AX_FB_MU_NG_4;
		ndpa_sta->cb = psta_info->asoc_cap.cb_sz_mu_fb ?
			HAL_NPDA_AX_CB_SU42_MU75 : HAL_NPDA_AX_CB_SU64_MU97;
	} else {
		ndpa_sta->fb_ng = HAL_NDPA_AX_FB_CQI;
		ndpa_sta->cb = 1;
	}
	ndpa_sta->disambiguation = 1;
	/* Nc shall alwary <= Nr */
	if ((CHANNEL_WIDTH_160 == bw) || (CHANNEL_WIDTH_80_80 == bw)) {
		if (psta_info->asoc_cap.max_nc > num_snd_dim_greater_80mhz)
			ndpa_sta->nc = num_snd_dim_greater_80mhz;
		else
			ndpa_sta->nc = psta_info->asoc_cap.max_nc;
	} else {
		if (psta_info->asoc_cap.max_nc > num_snd_dim)
			ndpa_sta->nc = num_snd_dim;
		else
			ndpa_sta->nc = psta_info->asoc_cap.max_nc;
	}
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "HE NDPA : aid 0x%x fb_ng 0x%x cb 0x%x nc 0x%x \n",
		  ndpa_sta->aid, ndpa_sta->fb_ng, ndpa_sta->cb, ndpa_sta->nc);
}


/**
 * rtw_hal_snd_set_fw_cmd_dialogtkn()
 * 	Set cmd dialog token value in NDPA
 * input
 * @he: is NDPA HE or not(VHT).
 * @token: dialog token value.
 **/
void rtw_hal_snd_set_fw_cmd_dialogtkn(void *hal, u8 *buf, u8 he, u8 token)
{
	struct hal_ax_fwcmd_snd *cmd = (struct hal_ax_fwcmd_snd *)buf;

	cmd->ndpa.snd_dialog.he = he;
	cmd->ndpa.snd_dialog.token = token;
}

/**
 * rtw_hal_snd_vht_fwcmd_su()
 * 	Prepared VHT SU Sounding Fw Cmd.
 * @hal:
 * @buf: (struct hal_ax_fwcmd_snd *) cmd buffer pointer.
 * @psta:  (struct rtw_phl_stainfo_t *) STA to be sounding.
 * @npda_sta: NPDA sta_info value
 **/
void rtw_hal_snd_vht_fwcmd_su(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u32 *npda_sta)
{
	struct hal_ax_fwcmd_snd *cmd = (struct hal_ax_fwcmd_snd *)buf;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_snd_obj *snd_obj =
		(struct hal_snd_obj *)hal_info->hal_com->snd_obj;
	u8 i = 0;

	cmd->frame_ex_type = HAL_FEXG_TYPE_AC_SU;
	cmd->macid[0] = psta->macid;
	cmd->ndpa.common.frame_ctl = HAL_SND_VHT_NDPA_FRM_CTRL; /*TODO*/
	for (i = 0; i < 6;i++) {
		cmd->ndpa.common.addr1[i] = psta->mac_addr[i]; /* NDPA-RA*/
		cmd->ndpa.common.addr2[i] = psta->wrole->mac_addr[i]; /* NDPA-TA*/
	}
	cmd->ndpa.common.duration = 100;
	cmd->ndpa.ndpa_sta_info[0] = *npda_sta;
	/* NDPA WD */
	cmd->wd[0].txpktsize = 21;/* 2 + 2 + 6 + 6 + 1 + 4, NO FCS */
	cmd->wd[0].ndpa_duration = 100;
	cmd->wd[0].disdatafb = 1;
	cmd->wd[0].datarate = snd_obj->ndpa_xpara.rate;
	cmd->wd[0].data_bw = snd_obj->ndpa_xpara.bw;
	cmd->wd[0].macid = psta->macid;
	cmd->wd[0].gi_ltf = snd_obj->ndpa_xpara.gi_ltf;
	cmd->wd[0].data_stbc = snd_obj->ndpa_xpara.stbc;
	cmd->wd[0].data_ldpc = snd_obj->ndpa_xpara.ldpc;
	cmd->wd[0].sifs_tx = 1;
	cmd->wd[0].snd_pkt_sel = HAL_SND_PKT_SEL_UST_NDPA;/* unicast ndpa */
	cmd->wd[0].ndpa = HAL_SND_PKT_NDPA_VHT;
	/* NDP WD */
	cmd->wd[1].disdatafb = 1;

	if (1 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_VHT_NSS2_MCS0;
	else if (2 ==psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_VHT_NSS3_MCS0;
	else if (3 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_VHT_NSS4_MCS0;
	else
		cmd->wd[1].datarate = RTW_DATA_RATE_VHT_NSS2_MCS0;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "psta->asoc_cap.nss_rx = 0x%x\n", psta->asoc_cap.nss_rx);
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "cmd->wd[1].datarate = 0x%x\n", cmd->wd[1].datarate);
	cmd->wd[1].data_bw = bw;
	cmd->wd[1].macid = psta->macid;
	cmd->wd[1].gi_ltf = RTW_GILTF_LGI_4XHE32;
	cmd->wd[1].sifs_tx = 0;
	cmd->wd[1].snd_pkt_sel = HAL_SND_PKT_SEL_LAST_NDP;
	cmd->wd[1].ndpa = HAL_SND_PKT_NDPA_VHT;
}

void rtw_hal_snd_vht_fwcmd_mu_add_sta(void *hal, u8 *buf, u32 *ndpa_sta,
				      struct rtw_phl_stainfo_t *sta,
		  		      u8 ndpa_idx, u8 last)
{
	struct hal_ax_fwcmd_snd *cmd = (struct hal_ax_fwcmd_snd *)buf;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_snd_obj *snd_obj =
		(struct hal_snd_obj *)hal_info->hal_com->snd_obj;
	u8 i = 0;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "==> rtw_hal_snd_vht_fwcmd_mu_add_sta\n");

	if (ndpa_idx >= HAL_MAX_VHT_SND_STA_NUM)
		return;

	cmd->macid[ndpa_idx] = sta->macid;
	/* NDPA sta_info*/
	cmd->ndpa.ndpa_sta_info[ndpa_idx] = *ndpa_sta;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "mac id 0x%x ; ndpa sta_info 0x%x\n",
		  cmd->macid[ndpa_idx], cmd->ndpa.ndpa_sta_info[ndpa_idx]);

	/* VHT BFRP */
	cmd->bfrp.hdr[ndpa_idx - 1].frame_ctl = HAL_SND_VHT_BFRP_FRM_CTRL;
	cmd->bfrp.hdr[ndpa_idx - 1].duration = 100;
	for (i = 0; i < 6; i++) {
		cmd->bfrp.hdr[ndpa_idx - 1].addr1[i] = sta->mac_addr[i]; /* NDPA-RA = Broadcast*/
		cmd->bfrp.hdr[ndpa_idx - 1].addr2[i] = sta->wrole->mac_addr[i]; /* NDPA-TA*/
	}
	cmd->bfrp.vht_para[ndpa_idx - 1].rexmit_bmp = 0;
	/*BFRP WD*/
	cmd->wd[1 + ndpa_idx].txpktsize = 17;  /* 2 + 2 + 6 + 6 + 1 */
	cmd->wd[1 + ndpa_idx].ndpa_duration = 100;
	cmd->wd[1 + ndpa_idx].datarate = snd_obj->bfrp_xpara.rate;
	cmd->wd[1 + ndpa_idx].data_ldpc = snd_obj->bfrp_xpara.ldpc;
	cmd->wd[1 + ndpa_idx].data_stbc = snd_obj->bfrp_xpara.stbc;
	cmd->wd[1 + ndpa_idx].macid = sta->macid;
	cmd->wd[1 + ndpa_idx].data_bw = snd_obj->bfrp_xpara.bw;
	cmd->wd[1 + ndpa_idx].gi_ltf = snd_obj->bfrp_xpara.gi_ltf;
	cmd->wd[1 + ndpa_idx].disdatafb = 1;
	if(last) {
		cmd->wd[1 + ndpa_idx].sifs_tx = 0;
		cmd->wd[1 + ndpa_idx].snd_pkt_sel = HAL_SND_PKT_SEL_LAST_BFRP;/* Final BFRP */
	} else {
		cmd->wd[1 + ndpa_idx].sifs_tx = 1;
		cmd->wd[1 + ndpa_idx].snd_pkt_sel = HAL_SND_PKT_SEL_MID_BFRP;
	}
	cmd->wd[1 + ndpa_idx].ndpa = HAL_SND_PKT_NDPA_VHT;

}

void rtw_hal_snd_vht_fwcmd_mu_pri(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u8 sta_nr, u32 *ndpa_sta)
{
	struct hal_ax_fwcmd_snd *cmd = (struct hal_ax_fwcmd_snd *)buf;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_snd_obj *snd_obj =
		(struct hal_snd_obj *)hal_info->hal_com->snd_obj;
	u8 i = 0;

	if (sta_nr == 4)
		cmd->frame_ex_type = HAL_FEXG_TYPE_AC_MU_3;
	else if(sta_nr == 3)
		cmd->frame_ex_type = HAL_FEXG_TYPE_AC_MU_2;
	else if(sta_nr == 2)
		cmd->frame_ex_type = HAL_FEXG_TYPE_AC_MU_1;
	else
		PHL_INFO("rtw_hal_snd_vht_fwcmd_mu_pri : ERROR!!!!!!!!!!!!!!!");
	cmd->macid[0] = psta->macid;

	/* NDPA */
	cmd->ndpa.common.frame_ctl = HAL_SND_VHT_NDPA_FRM_CTRL;
	for (i = 0; i < 6;i++) {
		/**
		 * Addr1: In VHT Case, Fill User_0's MAC Address to match BF Entry,
		 * HW will auto re-fill to Broadcast Address
		 **/
		cmd->ndpa.common.addr1[i] = psta->mac_addr[i];
		cmd->ndpa.common.addr2[i] = psta->wrole->mac_addr[i]; /* NDPA-TA*/
	}
	cmd->ndpa.common.duration = 150;/*TODO:*/
	/* primary sta ndpa sta_info*/
	cmd->ndpa.ndpa_sta_info[0] = *ndpa_sta;

	/* VHT BFRP ==> by STA */

	/* NDPA WD */
	cmd->wd[0].txpktsize = 17 + HAL_SND_VHT_NDPA_STA_SZ * sta_nr;/* 2 + 2 + 6 + 6 + 1 + 2, NO FCS */
	cmd->wd[0].ndpa_duration = 150;
	cmd->wd[0].disdatafb = 1;
	cmd->wd[0].datarate = snd_obj->ndpa_xpara.rate;
	cmd->wd[0].data_bw = snd_obj->ndpa_xpara.bw;
	cmd->wd[0].data_ldpc = snd_obj->ndpa_xpara.ldpc;
	cmd->wd[0].data_stbc = snd_obj->ndpa_xpara.stbc;
	cmd->wd[0].macid = psta->macid;
	cmd->wd[0].gi_ltf = snd_obj->ndpa_xpara.gi_ltf;
	cmd->wd[0].sifs_tx = 1;
	cmd->wd[0].snd_pkt_sel = HAL_SND_PKT_SEL_BST_NDPA;
	cmd->wd[0].ndpa = HAL_SND_PKT_NDPA_VHT;
	/* NDP WD */
	cmd->wd[1].disdatafb = 1;

	if (1 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_VHT_NSS2_MCS0;
	else if (2 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_VHT_NSS3_MCS0;
	else if (3 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_VHT_NSS4_MCS0;
	else
		cmd->wd[1].datarate = RTW_DATA_RATE_VHT_NSS2_MCS0;
	cmd->wd[1].data_bw = bw;
	cmd->wd[1].macid = psta->macid;
	cmd->wd[1].gi_ltf = RTW_GILTF_LGI_4XHE32;
	cmd->wd[1].sifs_tx = 1;
	cmd->wd[1].snd_pkt_sel = HAL_SND_PKT_SEL_MID_NDP;/*Last NDP*/
	cmd->wd[1].ndpa = HAL_SND_PKT_NDPA_VHT;
}

/* HE FW Command */
/**
 * rtw_hal_snd_ax_fwcmd_su()
 * 	fill the fw cmd for HE sounding type HE Non-TB : NDPA - NDP - CSI Report
 *
 **/
void rtw_hal_snd_ax_fwcmd_nontb(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u32 *npda_sta)
{
	struct hal_ax_fwcmd_snd *cmd = (struct hal_ax_fwcmd_snd *)buf;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_snd_obj *snd_obj =
		(struct hal_snd_obj *)hal_info->hal_com->snd_obj;
	u8 i = 0;

	cmd->frame_ex_type = HAL_FEXG_TYPE_AX_SU;
	cmd->macid[0] = psta->macid;
	cmd->ndpa.common.frame_ctl = HAL_SND_HE_NDPA_FRM_CTRL;
	for (i = 0; i < 6;i++) {
		cmd->ndpa.common.addr1[i] = psta->mac_addr[i]; /* NDPA-RA*/
		cmd->ndpa.common.addr2[i] = psta->wrole->mac_addr[i]; /* NDPA-TA*/
	}
	cmd->ndpa.common.duration = 100;
	cmd->ndpa.ndpa_sta_info[0] = *npda_sta;
	/* NDPA WD */
	cmd->wd[0].txpktsize = 21;/* 2 + 2 + 6 + 6 + 1 + 4, NO FCS */
	cmd->wd[0].ndpa_duration = 100;
	cmd->wd[0].disdatafb = 1;
	cmd->wd[0].datarate = snd_obj->ndpa_xpara.rate;
	cmd->wd[0].data_bw = snd_obj->ndpa_xpara.bw;
	cmd->wd[0].data_stbc = snd_obj->ndpa_xpara.stbc;
	cmd->wd[0].data_ldpc = snd_obj->ndpa_xpara.ldpc;
	cmd->wd[0].macid = psta->macid;
	cmd->wd[0].gi_ltf = snd_obj->ndpa_xpara.gi_ltf;
	cmd->wd[0].sifs_tx = 1;
	cmd->wd[0].snd_pkt_sel = HAL_SND_PKT_SEL_UST_NDPA;/*unicast ndpa*/
	cmd->wd[0].ndpa = HAL_SND_PKT_NDPA_HE;
	/* NDP WD */
	cmd->wd[1].disdatafb = 1;

	if (1 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_HE_NSS2_MCS0;
	else if (2 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_HE_NSS3_MCS0;
	else if (3 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_HE_NSS4_MCS0;
	else
		cmd->wd[1].datarate = RTW_DATA_RATE_HE_NSS2_MCS0;
	cmd->wd[1].data_bw = bw;
	cmd->wd[1].macid = psta->macid;
	cmd->wd[1].gi_ltf = RTW_GILTF_2XHE16; /* TODO: if psta->asoc_cap.ltf_gi support NDP 4XHE32;*/
	cmd->wd[1].sifs_tx = 0;
	cmd->wd[1].snd_pkt_sel = HAL_SND_PKT_SEL_LAST_NDP;
	cmd->wd[1].ndpa = HAL_SND_PKT_NDPA_HE;
}

/**
 * rtw_hal_snd_ax_fwcmd_tb_add_sta()
 * 	fill fw cmd for HE sounding with TB case.
 * 	shall call rtw_hal_snd_ax_fwcmd_tb_pri() first, and add STA by this api.
 * 	Note that RU-Allocation shall dicide from phl caller,
 * 	and shall avoid same with others
 * function input:
 * @hal:
 * @buf: (struct hal_ax_fwcmd_snd *) command buf pointer.
 * @ndpa_sta:
 * @sta: (struct rtw_phl_stainfo_t *)
 * @ru_idx: RU Allocation index;
 * @ndpa_idx: 0-7 for 8852a
 * @bfrp_idx: 0-1 for 8852a
 * @bfrp_u_idx: 0-3 for 8852a
 **/
void rtw_hal_snd_ax_fwcmd_tb_add_sta(void *hal, u8 *buf, u32 *ndpa_sta,
				     struct rtw_phl_stainfo_t *sta, u8 ru_idx,
		  		     u8 ndpa_idx, u8 bfrp_idx, u8 bfrp_u_idx)
{
	struct hal_ax_fwcmd_snd *cmd = (struct hal_ax_fwcmd_snd *)buf;
	struct hal_he_ndpa_sta_info *ndpa =
		(struct hal_he_ndpa_sta_info *)ndpa_sta;
	u8 mu = 0, ng = 4;
	u32 rpt_size = 0;

	if (ndpa_idx >= HAL_MAX_HE_SND_STA_NUM)
		return;
	if (bfrp_idx >= HAL_MAX_HE_BFRP_NUM)
		return;
	cmd->macid[ndpa_idx] = sta->macid;
	/* NDPA sta_info*/
	cmd->ndpa.ndpa_sta_info[ndpa_idx] = *ndpa_sta;
	/* BFRP user_info */
	cmd->bfrp.he_para[bfrp_idx].fbseg_rexmit_bmp[bfrp_u_idx] = 0;
	cmd->bfrp.he_para[bfrp_idx].user[bfrp_u_idx].aid12 = sta->aid;
	if (cmd->bfrp.he_para[bfrp_idx].common.ul_bw > CHANNEL_WIDTH_80) {
		ru_idx = (ru_idx << 1) | BIT(0);
	} else {
		ru_idx = (ru_idx << 1) &(~BIT(0));
	}
	cmd->bfrp.he_para[bfrp_idx].user[bfrp_u_idx].ru_pos = ru_idx;

	cmd->bfrp.he_para[bfrp_idx].user[bfrp_u_idx].ul_fec_code = 0;
	cmd->bfrp.he_para[bfrp_idx].user[bfrp_u_idx].ul_mcs = 3;/* HE-MCS3, TODO: Default value of golden */
	cmd->bfrp.he_para[bfrp_idx].user[bfrp_u_idx].ul_dcm = 0 ;
	cmd->bfrp.he_para[bfrp_idx].user[bfrp_u_idx].ss_alloc = 0;/* 1SS, TODO: Default value of golden */
	cmd->bfrp.he_para[bfrp_idx].user[bfrp_u_idx].ul_tgt_rssi =
			(u8)(sta->hal_sta->rssi_stat.rssi >> 1);/*TODO: From Rx ST SU RSSI */
	if (ndpa->fb_ng == HAL_NDPA_AX_FB_MU_NG_4) {
		mu = 1;
		ng = 4;
	} else if (ndpa->fb_ng == HAL_NDPA_AX_FB_MU_NG_16) {
		mu = 1;
		ng = 16;
	} else if (ndpa->fb_ng == HAL_NDPA_AX_FB_SU_NG_4) {
		mu = 0;
		ng = 4;
	} else if (ndpa->fb_ng == HAL_NDPA_AX_FB_SU_NG_16) {
		mu = 0;
		ng = 4;
	}
	rpt_size = _cal_he_csi_size(mu, sta->chandef.bw,
				    (sta->rlink->protocol_cap.num_snd_dim + 1),
				    ((u8)ndpa->nc + 1), ng, (u8)ndpa->cb);
	if (cmd->bfrp.he_para[bfrp_idx].f2p_info.csi_len_bfrp < (rpt_size/64))
		cmd->bfrp.he_para[bfrp_idx].f2p_info.csi_len_bfrp =
			(u16)(rpt_size / 64); /*unit 64 byte*/
}

void rtw_hal_snd_ax_fwcmd_tb_pri(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u8 sta_nr1, u8 sta_nr2)
{
	struct hal_ax_fwcmd_snd *cmd = (struct hal_ax_fwcmd_snd *)buf;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct hal_snd_obj *snd_obj =
		(struct hal_snd_obj *)hal_info->hal_com->snd_obj;
	u8 i = 0;
	u8 sta_nr = sta_nr1 + sta_nr2;

	if (sta_nr2 != 0)
		cmd->frame_ex_type = HAL_FEXG_TYPE_AX_MU_2;
	else
		cmd->frame_ex_type = HAL_FEXG_TYPE_AX_MU_1;
	cmd->bfrp0_sta_nr = sta_nr1;
	cmd->bfrp1_sta_nr = sta_nr2;
	cmd->macid[0] = psta->macid;

	/* NDPA */
	cmd->ndpa.common.frame_ctl = HAL_SND_HE_NDPA_FRM_CTRL; /*TODO:*/
	for (i = 0; i < 6;i++) {
		cmd->ndpa.common.addr1[i] = 0xFF; /* NDPA-RA = Broadcast*/
		cmd->ndpa.common.addr2[i] = psta->wrole->mac_addr[i]; /* NDPA-TA*/
		/* BFRP - 1*/
		cmd->bfrp.hdr[0].addr1[i] = 0xFF;
		cmd->bfrp.hdr[0].addr2[i] = psta->wrole->mac_addr[i];
		if (sta_nr2 != 0) {
			/* BFRP - 2*/
			cmd->bfrp.hdr[1].addr1[i] = 0xFF;
			cmd->bfrp.hdr[1].addr2[i] =  psta->wrole->mac_addr[i];
		}
	}
	cmd->ndpa.common.duration = 150;/*TODO:*/
	/*TODO: ndpa user info in other api */

	/* BFRP #1 */
	cmd->bfrp.hdr[0].frame_ctl = HAL_SND_HE_BFRP_FRM_CTRL; /* Trigger Frame */
	cmd->bfrp.hdr[0].duration = 100;
	cmd->bfrp.he_para[0].common.tgr_info = HAL_SND_TRIG_INFO_BFRP;/*BFRP*/
	cmd->bfrp.he_para[0].common.ul_len = 0xFFF;/*TODO: LSIG Length : sw provide or fw calculate */
	cmd->bfrp.he_para[0].common.more_tf = 0;
	cmd->bfrp.he_para[0].common.cs_rqd = 1;
	cmd->bfrp.he_para[0].common.ul_bw = bw;
	cmd->bfrp.he_para[0].common.gi_ltf = RTW_TB_GILTF_4XHE32;/* 8852A Limitation of UL OFDMA */
	cmd->bfrp.he_para[0].common.num_heltf = 0;/* TODO: Default value of golden */
	cmd->bfrp.he_para[0].common.ul_pktext = 0;/* TODO: fw? hw? */
	cmd->bfrp.he_para[0].common.ap_tx_pwr = 0x3C;/* TODO: Default value of golden */

	/* F2P cmd parameters */
	cmd->bfrp.he_para[0].f2p_info.tb_t_pe_bfrp = 2;
	cmd->bfrp.he_para[0].f2p_info.tri_pad_bfrp = 2;
	cmd->bfrp.he_para[0].f2p_info.ul_cqi_rpt_tri_bfrp = 0;
	cmd->bfrp.he_para[0].f2p_info.rf_gain_idx_bfrp = 0;/* ?? */
	cmd->bfrp.he_para[0].f2p_info.fix_gain_en_bfrp = 0;/* ?? */

	if (sta_nr2) {
		cmd->bfrp.hdr[1].frame_ctl = HAL_SND_HE_BFRP_FRM_CTRL; /* Trigger Frame */
		cmd->bfrp.hdr[1].duration = 100;
		cmd->bfrp.he_para[1].common.tgr_info = HAL_SND_TRIG_INFO_BFRP;/*BFRP*/
		cmd->bfrp.he_para[1].common.ul_len = 0xFFF;/*TODO: LSIG Length : sw provide or fw calculate */
		cmd->bfrp.he_para[1].common.more_tf = 0;
		cmd->bfrp.he_para[1].common.cs_rqd = 1;
		cmd->bfrp.he_para[1].common.ul_bw = bw;
		cmd->bfrp.he_para[1].common.gi_ltf = RTW_TB_GILTF_4XHE32;/* 8852A Limitation of UL OFDMA */
		cmd->bfrp.he_para[1].common.num_heltf = 0;/* TODO: Default value of golden */
		cmd->bfrp.he_para[1].common.ul_pktext = 0;/* TODO: fw? hw? */
		cmd->bfrp.he_para[1].common.ap_tx_pwr = 0x32;/* TODO: Default value of golden */

		cmd->bfrp.he_para[1].f2p_info.tb_t_pe_bfrp = 2;
		cmd->bfrp.he_para[1].f2p_info.tri_pad_bfrp = 2;
		cmd->bfrp.he_para[1].f2p_info.ul_cqi_rpt_tri_bfrp = 0;
		cmd->bfrp.he_para[1].f2p_info.rf_gain_idx_bfrp = 0;
		cmd->bfrp.he_para[1].f2p_info.fix_gain_en_bfrp = 0;
	}
	/* NDPA WD */
	cmd->wd[0].txpktsize = 17 + HAL_SND_HE_NDPA_STA_SZ * sta_nr;/* 2 + 2 + 6 + 6 + 1 + 4, NO FCS */
	cmd->wd[0].ndpa_duration = 100;
	cmd->wd[0].disdatafb = 1;
	cmd->wd[0].datarate = snd_obj->ndpa_xpara.rate;
	cmd->wd[0].data_bw = snd_obj->ndpa_xpara.bw;
	cmd->wd[0].data_stbc = snd_obj->ndpa_xpara.stbc;
	cmd->wd[0].data_ldpc = snd_obj->ndpa_xpara.ldpc;
	cmd->wd[0].macid = psta->macid;
	cmd->wd[0].gi_ltf = snd_obj->ndpa_xpara.gi_ltf;
	cmd->wd[0].sifs_tx = 1;
	cmd->wd[0].snd_pkt_sel = HAL_SND_PKT_SEL_BST_NDPA;
	cmd->wd[0].ndpa = HAL_SND_PKT_NDPA_HE;
	/* NDP WD */
	cmd->wd[1].disdatafb = 1;

	if (1 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_HE_NSS2_MCS0;
	else if (2 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_HE_NSS3_MCS0;
	else if(3 == psta->rlink->protocol_cap.num_snd_dim)
		cmd->wd[1].datarate = RTW_DATA_RATE_HE_NSS4_MCS0;
	else
		cmd->wd[1].datarate = RTW_DATA_RATE_HE_NSS2_MCS0;
	cmd->wd[1].data_bw = bw;
	cmd->wd[1].macid = psta->macid;
	cmd->wd[1].gi_ltf = RTW_GILTF_2XHE16; /* TODO: if support 4x32 NDP */
	cmd->wd[1].sifs_tx = 0;
	cmd->wd[1].snd_pkt_sel = HAL_SND_PKT_SEL_MID_NDP;
	cmd->wd[1].ndpa = HAL_SND_PKT_NDPA_HE;
	/* BFRP #1 WD */
	cmd->wd[2].txpktsize = 24 + HAL_SND_HE_BFRP_STA_SZ * sta_nr1;  /* 2 + 2 + 6 + 6 + 8 + 5*N */
	cmd->wd[2].ndpa_duration = 100;
	cmd->wd[2].datarate = snd_obj->bfrp_xpara.rate;
	//cmd->wd[2].macid
	cmd->wd[2].data_bw = snd_obj->bfrp_xpara.bw;
	cmd->wd[2].data_stbc = snd_obj->bfrp_xpara.stbc;
	cmd->wd[2].data_ldpc = snd_obj->bfrp_xpara.ldpc;
	cmd->wd[2].gi_ltf = snd_obj->bfrp_xpara.gi_ltf;
	cmd->wd[2].disdatafb = 1;
	if(sta_nr2) {
		cmd->wd[2].sifs_tx = 1;
		cmd->wd[2].snd_pkt_sel = HAL_SND_PKT_SEL_MID_BFRP;
	} else {
		cmd->wd[2].sifs_tx = 0;/* Final BFRP */
		cmd->wd[2].snd_pkt_sel = HAL_SND_PKT_SEL_LAST_BFRP;
	}
	cmd->wd[2].ndpa = HAL_SND_PKT_NDPA_HE;
	if (sta_nr2) {
		cmd->wd[3].txpktsize = 24 + HAL_SND_HE_BFRP_STA_SZ * sta_nr2;  /* 2 + 2 + 6 + 6 + 8 + 5*N */
		cmd->wd[3].ndpa_duration = 100;
		cmd->wd[3].datarate = snd_obj->bfrp_xpara.rate;
		cmd->wd[3].data_bw = snd_obj->bfrp_xpara.bw;
		cmd->wd[3].data_stbc = snd_obj->bfrp_xpara.stbc;
		cmd->wd[3].data_ldpc = snd_obj->bfrp_xpara.ldpc;
		cmd->wd[3].gi_ltf = snd_obj->bfrp_xpara.gi_ltf;
		cmd->wd[3].disdatafb = 1;
		cmd->wd[3].sifs_tx = 0;/* Final BFRP */
		cmd->wd[3].snd_pkt_sel = HAL_SND_PKT_SEL_LAST_BFRP;
		cmd->wd[3].ndpa = HAL_SND_PKT_NDPA_HE;
	}
}

u8 *
rtw_hal_snd_prepare_snd_cmd(void *hal)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u8 *buf = NULL;

	buf = _os_mem_alloc(hal_to_drvpriv(hal_info),
			    sizeof(struct hal_ax_fwcmd_snd));

	return buf;
}


enum rtw_hal_status
rtw_hal_snd_release_snd_cmd(void *hal, u8 *buf)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	do {
		if (buf == NULL)
			break;
		_os_mem_free(hal_to_drvpriv(hal_info), buf,
			     sizeof(struct hal_ax_fwcmd_snd));
	} while (0);

	return hstatus;
}


enum rtw_hal_status
rtw_hal_snd_send_fw_cmd(void *hal, u8 *cmd)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "==> rtw_hal_snd_send_fw_cmd \n");
	hstatus = hal_mac_ax_send_fw_snd(hal_info,
			(struct hal_ax_fwcmd_snd *)cmd);
	/*TODO: Dump CMD content */
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "<== rtw_hal_snd_send_fw_cmd \n");
	return hstatus;
}

#endif
