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
#define _PHL_TEST_MP_TX_C_
#include "../../phl_headers.h"
#include "phl_test_mp_def.h"
#include "../../hal_g6/test/mp/hal_test_mp_api.h"
#include "../../phl_api.h"

#ifdef CONFIG_PHL_TEST_MP
static enum rtw_phl_status
phl_mp_get_plcp_usr_info(struct mp_context *mp, struct mp_tx_arg *arg)
{
	struct rtw_phl_com_t *phl_com = mp->phl_com;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	int plcp_usr_idx = arg->plcp_usr_idx;

	if(arg->nss > phl_com->phy_cap[mp->cur_phy].txss){
		hal_status = RTW_HAL_STATUS_FAILURE;
		PHL_INFO("%s Invalid NSS %d\n",__FUNCTION__, arg->nss);
	}
	else{
		PHL_INFO("%s arg->plcp_usr_idx = %d\n",__FUNCTION__, arg->plcp_usr_idx);
		PHL_INFO("%s arg->mcs = %d\n",__FUNCTION__, arg->mcs);
		PHL_INFO("%s arg->mpdu_len = %d\n",__FUNCTION__, arg->mpdu_len);
		PHL_INFO("%s arg->n_mpdu = %d\n",__FUNCTION__, arg->n_mpdu);
		PHL_INFO("%s arg->fec = %d\n",__FUNCTION__, arg->fec);
		PHL_INFO("%s arg->dcm = %d\n",__FUNCTION__, arg->dcm);
		PHL_INFO("%s arg->aid = %d\n",__FUNCTION__, arg->aid);
		PHL_INFO("%s arg->scrambler_seed = %d\n",__FUNCTION__, arg->scrambler_seed);
		PHL_INFO("%s arg->random_init_seed = %d\n",__FUNCTION__, arg->random_init_seed);
		PHL_INFO("%s arg->apep = %d\n",__FUNCTION__, arg->apep);
		PHL_INFO("%s arg->ru_alloc = %d\n",__FUNCTION__, arg->ru_alloc);
		PHL_INFO("%s arg->nss = %d\n",__FUNCTION__, arg->nss);
		PHL_INFO("%s arg->txbf = %d\n",__FUNCTION__, arg->txbf);
		PHL_INFO("%s arg->pwr_boost_db = %d\n",__FUNCTION__, arg->pwr_boost_db);

		/* _os_mem_cpy(mp->phl_com->drv_priv,(void*)(&(mp->usr[plcp_usr_idx])),(void*)((unsigned long)arg+offset1),(offset2-offset1)); */
		mp->usr[plcp_usr_idx].mcs = arg->mcs;
		mp->usr[plcp_usr_idx].mpdu_len = arg->mpdu_len;
		mp->usr[plcp_usr_idx].n_mpdu = arg->n_mpdu;
		mp->usr[plcp_usr_idx].fec = arg->fec;
		mp->usr[plcp_usr_idx].dcm = arg->dcm;
		mp->usr[plcp_usr_idx].aid = arg->aid;
#ifdef RTW_WKARD_AP_MP
		/* in AP mode, Random Seed is not good for MP, so this is a workaround */
		mp->usr[plcp_usr_idx].scrambler_seed = 0x4b;
		mp->usr[plcp_usr_idx].random_init_seed = 0x81;
#else
		mp->usr[plcp_usr_idx].scrambler_seed = arg->scrambler_seed;
		mp->usr[plcp_usr_idx].random_init_seed = arg->random_init_seed;
#endif
		mp->usr[plcp_usr_idx].apep = arg->apep;
		mp->usr[plcp_usr_idx].ru_alloc = arg->ru_alloc;
		mp->usr[plcp_usr_idx].nss = arg->nss;
		mp->usr[plcp_usr_idx].txbf = arg->txbf;
		mp->usr[plcp_usr_idx].pwr_boost_db = arg->pwr_boost_db;

		PHL_INFO("%s Copy to user\n",__FUNCTION__);
		PHL_INFO("%s plcp_usr_idx = %d\n",__FUNCTION__, plcp_usr_idx);
		PHL_INFO("%s mcs = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].mcs);
		PHL_INFO("%s mpdu_len = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].mpdu_len);
		PHL_INFO("%s n_mpdu = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].n_mpdu);
		PHL_INFO("%s fec = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].fec);
		PHL_INFO("%s dcm = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].dcm);
		PHL_INFO("%s aid = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].aid);
		PHL_INFO("%s scrambler_seed = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].scrambler_seed);
		PHL_INFO("%s random_init_seed = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].random_init_seed);
		PHL_INFO("%s apep = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].apep);
		PHL_INFO("%s ru_alloc = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].ru_alloc);
		PHL_INFO("%s nss = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].nss);
		PHL_INFO("%s txbf = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].txbf);
		PHL_INFO("%s pwr_boost_db = %d\n",__FUNCTION__, mp->usr[plcp_usr_idx].pwr_boost_db);
		hal_status = RTW_HAL_STATUS_SUCCESS;
	}

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;
	return RTW_PHL_STATUS_SUCCESS;
}

static void
phl_mp_get_plcp_common_info(struct mp_context *mp,
                            struct mp_tx_arg *arg,
                            struct mp_plcp_param_t *plcp_tx_struct)
{
	/*unsigned long offset1 = (unsigned long)(&(((struct mp_tx_arg *)0)->dbw));
	unsigned long offset2 = (unsigned long)(&(((struct mp_tx_arg *)0)->tb_rsvd));*/
	u8 i = 0;

	PHL_INFO("%s=============================\n",__FUNCTION__);
	PHL_INFO("%s arg->dbw = %d\n",__FUNCTION__, arg->dbw);
	PHL_INFO("%s arg->source_gen_mode = %d\n",__FUNCTION__, arg->source_gen_mode);
	PHL_INFO("%s arg->locked_clk = %d\n",__FUNCTION__, arg->locked_clk);
	PHL_INFO("%s arg->dyn_bw = %d\n",__FUNCTION__, arg->dyn_bw);
	PHL_INFO("%s arg->ndp_en = %d\n",__FUNCTION__, arg->ndp_en);
	PHL_INFO("%s arg->long_preamble_en = %d\n",__FUNCTION__, arg->long_preamble_en);
	PHL_INFO("%s arg->stbc = %d\n",__FUNCTION__, arg->stbc);
	PHL_INFO("%s arg->gi = %d\n",__FUNCTION__, arg->gi);
	PHL_INFO("%s arg->tb_l_len = %d\n",__FUNCTION__, arg->tb_l_len);
	PHL_INFO("%s arg->tb_ru_tot_sts_max = %d\n",__FUNCTION__, arg->tb_ru_tot_sts_max);
	PHL_INFO("%s arg->vht_txop_not_allowed = %d\n",__FUNCTION__, arg->vht_txop_not_allowed);
	PHL_INFO("%s arg->tb_disam = %d\n",__FUNCTION__, arg->tb_disam);
	PHL_INFO("%s arg->doppler = %d\n",__FUNCTION__, arg->doppler);
	PHL_INFO("%s arg->he_ltf_type = %d\n",__FUNCTION__, arg->he_ltf_type);
	PHL_INFO("%s arg->ht_l_len = %d\n",__FUNCTION__, arg->ht_l_len);
	PHL_INFO("%s arg->preamble_puncture = %d\n",__FUNCTION__, arg->preamble_puncture);
	PHL_INFO("%s arg->he_mcs_sigb = %d\n",__FUNCTION__, arg->he_mcs_sigb);
	PHL_INFO("%s arg->he_dcm_sigb = %d\n",__FUNCTION__, arg->he_dcm_sigb);
	PHL_INFO("%s arg->he_sigb_compress_en = %d\n",__FUNCTION__, arg->he_sigb_compress_en);
	PHL_INFO("%s arg->max_tx_time_0p4us = %d\n",__FUNCTION__, arg->max_tx_time_0p4us);
	PHL_INFO("%s arg->ul_flag = %d\n",__FUNCTION__, arg->ul_flag);
	PHL_INFO("%s arg->tb_ldpc_extra = %d\n",__FUNCTION__, arg->tb_ldpc_extra);
	PHL_INFO("%s arg->bss_color = %d\n",__FUNCTION__, arg->bss_color);
	PHL_INFO("%s arg->sr = %d\n",__FUNCTION__, arg->sr);
	PHL_INFO("%s arg->beamchange_en = %d\n",__FUNCTION__, arg->beamchange_en);
	PHL_INFO("%s arg->he_er_u106ru_en = %d\n",__FUNCTION__, arg->he_er_u106ru_en);
	PHL_INFO("%s arg->ul_srp1 = %d\n",__FUNCTION__, arg->ul_srp1);
	PHL_INFO("%s arg->ul_srp2 = %d\n",__FUNCTION__, arg->ul_srp2);
	PHL_INFO("%s arg->ul_srp3 = %d\n",__FUNCTION__, arg->ul_srp3);
	PHL_INFO("%s arg->ul_srp4 = %d\n",__FUNCTION__, arg->ul_srp4);
	PHL_INFO("%s arg->mode = %d\n",__FUNCTION__, arg->mode);
	PHL_INFO("%s arg->group_id = %d\n",__FUNCTION__, arg->group_id);
	PHL_INFO("%s arg->ppdu_type = %d\n",__FUNCTION__, arg->ppdu_type);
	PHL_INFO("%s arg->txop = %d\n",__FUNCTION__, arg->txop);
	PHL_INFO("%s arg->tb_strt_sts = %d\n",__FUNCTION__, arg->tb_strt_sts);
	PHL_INFO("%s arg->tb_pre_fec_padding_factor = %d\n",__FUNCTION__, arg->tb_pre_fec_padding_factor);
	PHL_INFO("%s arg->cbw = %d\n",__FUNCTION__, arg->cbw);
	PHL_INFO("%s arg->txsc = %d\n",__FUNCTION__, arg->txsc);
	PHL_INFO("%s arg->tb_mumimo_mode_en = %d\n",__FUNCTION__, arg->tb_mumimo_mode_en);
	PHL_INFO("%s arg->nominal_t_pe = %d\n",__FUNCTION__, arg->nominal_t_pe);
	PHL_INFO("%s arg->ness = %d\n",__FUNCTION__, arg->ness);
	PHL_INFO("%s arg->n_user = %d\n",__FUNCTION__, arg->n_user);
	PHL_INFO("%s arg->tb_rsvd = %d\n",__FUNCTION__, arg->tb_rsvd);
	PHL_INFO("%s=============================\n",__FUNCTION__);
	plcp_tx_struct->dbw = arg->dbw;
	plcp_tx_struct->source_gen_mode = arg->source_gen_mode;
	plcp_tx_struct->locked_clk = arg->locked_clk;
	plcp_tx_struct->dyn_bw = arg->dyn_bw;
	plcp_tx_struct->ndp_en = arg->ndp_en;
	plcp_tx_struct->long_preamble_en = arg->long_preamble_en;
	plcp_tx_struct->stbc = arg->stbc;
	plcp_tx_struct->gi = arg->gi;
	plcp_tx_struct->tb_l_len = arg->tb_l_len;
	plcp_tx_struct->tb_ru_tot_sts_max = arg->tb_ru_tot_sts_max;
	plcp_tx_struct->vht_txop_not_allowed = arg->vht_txop_not_allowed;
	plcp_tx_struct->tb_disam = arg->tb_disam;
	plcp_tx_struct->doppler = arg->doppler;
	plcp_tx_struct->he_ltf_type = arg->he_ltf_type;
	plcp_tx_struct->ht_l_len = arg->ht_l_len;
	plcp_tx_struct->preamble_puncture = arg->preamble_puncture;
	plcp_tx_struct->he_mcs_sigb = arg->he_mcs_sigb;
	plcp_tx_struct->he_dcm_sigb = arg->he_dcm_sigb;
	plcp_tx_struct->he_sigb_compress_en = arg->he_sigb_compress_en;
	plcp_tx_struct->max_tx_time_0p4us = arg->max_tx_time_0p4us;
	plcp_tx_struct->ul_flag = arg->ul_flag;
	plcp_tx_struct->tb_ldpc_extra = arg->tb_ldpc_extra;
	plcp_tx_struct->bss_color = arg->bss_color;
	plcp_tx_struct->sr = arg->sr;
	plcp_tx_struct->beamchange_en = arg->beamchange_en;
	plcp_tx_struct->he_er_u106ru_en = arg->he_er_u106ru_en;
	plcp_tx_struct->ul_srp1 = arg->ul_srp1;
	plcp_tx_struct->ul_srp2 = arg->ul_srp2;
	plcp_tx_struct->ul_srp3 = arg->ul_srp3;
	plcp_tx_struct->ul_srp4 = arg->ul_srp4;
	plcp_tx_struct->mode = arg->mode;
	plcp_tx_struct->group_id = arg->group_id;
	plcp_tx_struct->ppdu_type = arg->ppdu_type;
	plcp_tx_struct->txop = arg->txop;
	plcp_tx_struct->tb_strt_sts = arg->tb_strt_sts;
	plcp_tx_struct->tb_pre_fec_padding_factor = arg->tb_pre_fec_padding_factor;
	plcp_tx_struct->cbw = arg->cbw;
	plcp_tx_struct->txsc = arg->txsc;
	plcp_tx_struct->tb_mumimo_mode_en = arg->tb_mumimo_mode_en;
	plcp_tx_struct->nominal_t_pe = (u8)arg->nominal_t_pe;
	plcp_tx_struct->ness = (u8)arg->ness;
	plcp_tx_struct->n_user = (u8)arg->n_user;
	plcp_tx_struct->tb_rsvd = (u16)arg->tb_rsvd;

#if 0
	//copy common info
	_os_mem_cpy(mp->phl_com->drv_priv,(void*)(plcp_tx_struct),(void*)((unsigned long)arg+offset1),(offset2-offset1));
#endif

	//copy user info
	_os_mem_cpy(mp->phl_com->drv_priv, (void *)(&(plcp_tx_struct->usr[0])),(void*)(&(mp->usr[0])),sizeof(struct mp_usr_plcp_gen_in)*4);

	for(i = 0; i < 4; i++) {
		PHL_INFO("%s=============================\n",__FUNCTION__);
		PHL_INFO("%s plcp_usr_idx = %d\n",__FUNCTION__, i);
		PHL_INFO("%s mcs = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].mcs);
		PHL_INFO("%s mpdu_len = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].mpdu_len);
		PHL_INFO("%s n_mpdu = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].n_mpdu);
		PHL_INFO("%s fec = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].fec);
		PHL_INFO("%s dcm = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].dcm);
		PHL_INFO("%s aid = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].aid);
		PHL_INFO("%s scrambler_seed = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].scrambler_seed);
		PHL_INFO("%s random_init_seed = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].random_init_seed);
		PHL_INFO("%s apep = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].apep);
		PHL_INFO("%s ru_alloc = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].ru_alloc);
		PHL_INFO("%s nss = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].nss);
		PHL_INFO("%s txbf = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].txbf);
		PHL_INFO("%s pwr_boost_db = %d\n",__FUNCTION__, plcp_tx_struct->usr[i].pwr_boost_db);
		PHL_INFO("%s=============================\n",__FUNCTION__);
	}
}

static enum rtw_phl_status phl_mp_tx_tmac(struct mp_context *mp,
							 struct mp_tx_arg *sw_tx_param)
{
	u32 stbc = sw_tx_param->stbc;
	u32 gi = sw_tx_param->gi;
	u32 data_rate = sw_tx_param->data_rate;
	u32 is_ldpc = sw_tx_param->fec;
	u32 dbw = sw_tx_param ->dbw;
	u32 dcm = sw_tx_param ->dcm;
	struct rtw_trx_test_param test_param = {0};

	/*u32 stbc = sw_tx_param->stbc;
	u32 gi = sw_tx_param->gi;
	enum rtw_data_rate data_rate = sw_tx_param->usr[sw_tx_gen_usr_idx].data_rate;;
	u32 is_ldpc = sw_tx_param->usr[sw_tx_gen_usr_idx].fec;
	u32 mpdu_len = sw_tx_param->usr[sw_tx_gen_usr_idx].mpdu_len;*/
	// PHL_INFO("%s: stbc = %d, gi = %d, data rate = %X, coding = %d, mpdu_len = %d\n", __func__, stbc, gi, data_rate, is_ldpc, mpdu_len);

	PHL_INFO("%s: stbc = %d, gi = %d, data rate = %X, coding = %d\n", __func__, stbc, gi, data_rate, is_ldpc);
	rtw_phl_trx_default_param(mp->phl, &test_param);
	test_param.tx_cap.f_rate = (u16)data_rate;
	test_param.tx_cap.f_gi_ltf = (u8)gi;
	test_param.tx_cap.f_stbc = (u8)stbc;
	test_param.tx_cap.f_ldpc = (u8)is_ldpc;
	test_param.tx_cap.f_bw = (u8)dbw;
	test_param.tx_cap.f_dcm = (u8)dcm;
	rtw_phl_trx_testsuite(mp->phl, &test_param);
	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_tx_plcp_gen(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct mp_plcp_param_t plcp_tx_struct = {0};

	phl_mp_get_plcp_common_info(mp,arg, &plcp_tx_struct);

	hal_status = rtw_hal_mp_tx_plcp_gen(mp, arg, &plcp_tx_struct);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_tx_packet(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	if (arg->tx_method == MP_PMACT_TX) {
		PHL_INFO("%s: CMD = MP_PMACT_TX\n", __func__);
		hal_status = rtw_hal_mp_tx_pmac_packet(mp, arg);
		phl_status = RTW_PHL_STATUS_SUCCESS;
	} else if (arg->tx_method == MP_TMACT_TX) {
		PHL_INFO("%s: CMD = MP_TMACT_TX\n", __func__);
		phl_status = phl_mp_tx_tmac(mp, arg);
		hal_status = RTW_HAL_STATUS_SUCCESS;
	}
	else if(arg->tx_method == MP_SW_TX) {
		/* Remove this part after revise the dll command */
		PHL_INFO("%s: CMD = MP_SW_TX\n", __func__);
		phl_status = phl_mp_tx_tmac(mp, arg);
		hal_status = RTW_HAL_STATUS_SUCCESS;
	}
	else if(arg->tx_method == MP_FW_PMAC_TX){
		PHL_INFO("%s: CMD = MP_FW_PMAC_TX\n", __func__);
		hal_status = rtw_hal_mp_tx_pmac_fw_trigger(mp, arg);
		phl_status = RTW_PHL_STATUS_SUCCESS;
	}

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return phl_status;
}

static enum rtw_phl_status phl_mp_tx_continuous_packet(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	if (arg->tx_method == MP_PMACT_TX) {
		PHL_INFO("%s: CMD = MP_PMACT_TX\n", __func__);
		hal_status = rtw_hal_mp_tx_pmac_continuous(mp, arg);
	} else if (arg->tx_method == MP_TMACT_TX) {
		PHL_INFO("%s: CMD = MP_TMACT_TX\n", __func__);
		/* Call hal API */
	}

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_tx_single_tone(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_tx_single_tone(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_tx_carrier_suppression(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_tx_carrier_suppression(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_tx_phy_ok(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_tx_phy_ok_cnt(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_tx_tb_test(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	u8 ui_bssid[6];

	ui_bssid[0] = arg->bSS_id_addr0;
	ui_bssid[1] = arg->bSS_id_addr1;
	ui_bssid[2] = arg->bSS_id_addr2;
	ui_bssid[3] = arg->bSS_id_addr3;
	ui_bssid[4] = arg->bSS_id_addr4;
	ui_bssid[5] = arg->bSS_id_addr5;

	PHL_INFO("Bssid = %x - %x - %x - %x - %x - %x \n",ui_bssid[0],ui_bssid[1],ui_bssid[2],ui_bssid[3],ui_bssid[4],ui_bssid[5]);
	PHL_INFO("Bss aid = %x \n",arg->aid);
	PHL_INFO("Bss color = %x \n",arg->bss_color);

	rtw_phl_test_txtb_cfg(mp->phl_com, &arg->is_link_mode, sizeof(arg->is_link_mode), ui_bssid, (u8)arg->aid, (u8)arg->bss_color);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_tx_dpd_bypass(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_set_dpd_bypass(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_tx_check_tx_idle(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;

	rtw_hal_mp_check_tx_idle(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = hal_status;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status phl_mp_tx_bb_loop_bck(
	struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mp_bb_loop_bck(mp, arg);

	/* Record the result */
	arg->cmd_ok = true;
	arg->status = RTW_HAL_STATUS_SUCCESS;

	/* Transfer to report */
	mp->rpt = arg;
	mp->rpt_len = sizeof(struct mp_tx_arg);
	mp->buf = NULL;
	mp->buf_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}


enum rtw_phl_status mp_tx(struct mp_context *mp, struct mp_tx_arg *arg)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	switch(arg->cmd) {
	case MP_TX_CONFIG_PLCP_PATTERN:
		PHL_INFO("%s: CMD = MP_TX_SET_PLCP_PATTERN\n", __func__);
		break;
	case MP_TX_CONFIG_PLCP_USER_INFO:
		PHL_INFO("%s: CMD = MP_TX_CONFIG_PLCP_USER_INFO\n", __func__);
		phl_status = phl_mp_get_plcp_usr_info(mp,arg);
		break;
	case MP_TX_CONFIG_PLCP_COMMON_INFO:
		PHL_INFO("%s: CMD = MP_TX_CONFIG_PLCP_COMMON_INFO\n", __func__);
		phl_status = phl_mp_tx_plcp_gen(mp, arg);
		break;
	case MP_TX_PACKETS:
		PHL_INFO("%s: CMD = MP_PACKETS_TX\n", __func__);
		phl_status = phl_mp_tx_packet(mp, arg);
		break;
	case MP_TX_CONTINUOUS:
		PHL_INFO("%s: CMD = MP_TX_CONTINUOUS\n", __func__);
		phl_status = phl_mp_tx_continuous_packet(mp, arg);
		break;
	case MP_TX_SINGLE_TONE:
		PHL_INFO("%s: CMD = MP_TX_SINGLE_TONE\n", __func__);
		phl_status = phl_mp_tx_single_tone(mp, arg);
		break;
	case MP_TX_CCK_Carrier_Suppression:
		PHL_INFO("%s: CMD = MP_TX_CCK_Carrier_Suppression\n", __func__);
		phl_status = phl_mp_tx_carrier_suppression(mp, arg);
		break;
	case MP_TX_CMD_PHY_OK:
		PHL_INFO("%s: CMD = MP_TX_CMD_PHY_OK\n", __FUNCTION__);
		phl_status = phl_mp_tx_phy_ok(mp, arg);
		break;
	case MP_TX_NONE:
		PHL_INFO("%s: CMD = MP_TX_NONE\n", __func__);
		break;
	case MP_TX_TB_TEST:
		PHL_INFO("%s: CMD = MP_TX_TB_TEST\n", __FUNCTION__);
		phl_status = phl_mp_tx_tb_test(mp, arg);
		break;
	case MP_TX_DPD_BYPASS:
		PHL_INFO("%s: CMD = MP_TX_DPD_BYPASS\n", __FUNCTION__);
		phl_status = phl_mp_tx_dpd_bypass(mp, arg);
		break;
	case MP_TX_CHECK_TX_IDLE:
		PHL_INFO("%s: CMD = MP_TX_CHECK_TX_IDLE\n", __FUNCTION__);
		phl_status = phl_mp_tx_check_tx_idle(mp, arg);
		break;
	case MP_TX_CMD_BB_LOOPBCK:
		PHL_INFO("%s: CMD = MP_CONFIG_CMD_BB_LOOPBCK\n", __FUNCTION__);
		phl_status = phl_mp_tx_bb_loop_bck(mp, arg);
		break;
	default:
		PHL_INFO("%s: CMD = Unknown\n", __func__);
		break;
	}

	return phl_status;
}
#endif /* CONFIG_PHL_TEST_MP */
