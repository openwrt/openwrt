/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

#include "halbb_precomp.h"

#ifdef HALBB_CNSL_CMN_INFO_SUPPORT

void halbb_env_mntr_log_cnsl(struct bb_info *bb, u32 *_used,
			     char *output, u32 *_out_len)
{
	struct bb_env_mntr_info *env = &bb->bb_env_mntr_i;
	u8 i = 0;

	if (bb->bb_watchdog_mode != BB_WATCHDOG_NORMAL)
		return;

	if ((env->ccx_watchdog_result & CCX_SEL_EXCLU_NHM_FAHM) == CCX_FAIL) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "[CLM/IFS_CLM/EDCCA_CLM]Env_mntr get CCX result failed!\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "APP:{CLM, IFS_CLM, EDCCA} = {%d, %d, %d}\n",
			    env->clm_app, env->ifs_clm_app, env->edcca_clm_app);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "[CLM/IFS_CLM/EDCCA_CLM]{Tx, CCA_sec, EDCCA_p20, EDCCA_exclu_CCA} = {%d, %d, %d, %d} %%\n",
			    env->ifs_clm_tx_ratio, env->clm_ratio,
			    env->edcca_clm_ratio,
			    env->ifs_clm_edcca_excl_cca_ratio);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used, "FA{CCK, OFDM}={%d, %d} %%, CCA_exclu_FA{CCK, OFDM}={%d, %d} %%\n",
			    env->ifs_clm_cck_fa_ratio,
			    env->ifs_clm_ofdm_fa_ratio,
			    env->ifs_clm_cck_cca_excl_fa_ratio,
			    env->ifs_clm_ofdm_cca_excl_fa_ratio);
	}

	if (((env->ccx_watchdog_result & NHM_SEL) == CCX_FAIL) &&
	    (env->nhm_app != NHM_FW_CTRL)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "[NHM]Env_mntr get NHM result failed! NHM APP=%d\n",
			    env->nhm_app);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "[NHM]{Tx, Idle, CCA_p20} = {%d, %d, %d} %%, nhm_ratio = %d %%\n",
			    env->nhm_tx_ratio, env->nhm_idle_ratio,
			    env->nhm_cca_ratio, env->nhm_ratio);
		if ((bb->ic_type == BB_RTL8852A) ||
		    (bb->ic_type == BB_RTL8852B) ||
		    (bb->ic_type == BB_RTL8852C)) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    "  Th", NHM_TH_2_RSSI(env->nhm_th[9]),
				    NHM_TH_2_RSSI(env->nhm_th[8]),
				    NHM_TH_2_RSSI(env->nhm_th[7]),
				    NHM_TH_2_RSSI(env->nhm_th[6]),
				    NHM_TH_2_RSSI(env->nhm_th[5]),
				    NHM_TH_2_RSSI(env->nhm_th[4]),
				    NHM_TH_2_RSSI(env->nhm_th[3]),
				    NHM_TH_2_RSSI(env->nhm_th[2]),
				    NHM_TH_2_RSSI(env->nhm_th[1]),
				    NHM_TH_2_RSSI(env->nhm_th[0]));
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[NHM]  (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    env->nhm_pwr, 5 * (env->nhm_pwr_0p5 & 0x1),
				    env->nhm_rpt[10], env->nhm_rpt[9],
				    env->nhm_rpt[8], env->nhm_rpt[7],
				    env->nhm_rpt[6], env->nhm_rpt[5],
				    env->nhm_rpt[4], env->nhm_rpt[3],
				    env->nhm_rpt[2], env->nhm_rpt[1],
				    env->nhm_rpt[0]);
		} else {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    "  Th", NHM_TH_2_RSSI(env->nhm_th[10]),
				    NHM_TH_2_RSSI(env->nhm_th[9]),
				    NHM_TH_2_RSSI(env->nhm_th[8]),
				    NHM_TH_2_RSSI(env->nhm_th[7]),
				    NHM_TH_2_RSSI(env->nhm_th[6]),
				    NHM_TH_2_RSSI(env->nhm_th[5]),
				    NHM_TH_2_RSSI(env->nhm_th[4]),
				    NHM_TH_2_RSSI(env->nhm_th[3]),
				    NHM_TH_2_RSSI(env->nhm_th[2]),
				    NHM_TH_2_RSSI(env->nhm_th[1]),
				    NHM_TH_2_RSSI(env->nhm_th[0]));
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "[NHM]  (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
				    env->nhm_pwr, 5 * (env->nhm_pwr_0p5 & 0x1),
				    env->nhm_rpt[11], env->nhm_rpt[10],
				    env->nhm_rpt[9], env->nhm_rpt[8],
				    env->nhm_rpt[7], env->nhm_rpt[6],
				    env->nhm_rpt[5], env->nhm_rpt[4],
				    env->nhm_rpt[3], env->nhm_rpt[2],
				    env->nhm_rpt[1], env->nhm_rpt[0]);
		}
	}

	if ((env->fahm_watchdog_result == false) && (env->fahm_app != FAHM_DIG)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "[FAHM]Env_mntr get FAHM result failed! FAHM APP=%d\n",
			    env->fahm_app);
		return;
	}
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[FAHM]{FA, CRC_err} = {%d, %d} %%\n",
	       env->fahm_ratio, env->fahm_denom_ratio);
	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
	    (bb->ic_type == BB_RTL8852C)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			    "  Th", FAHM_TH_2_RSSI(env->fahm_th[9]),
			    FAHM_TH_2_RSSI(env->fahm_th[8]),
			    FAHM_TH_2_RSSI(env->fahm_th[7]),
			    FAHM_TH_2_RSSI(env->fahm_th[6]),
			    FAHM_TH_2_RSSI(env->fahm_th[5]),
			    FAHM_TH_2_RSSI(env->fahm_th[4]),
			    FAHM_TH_2_RSSI(env->fahm_th[3]),
			    FAHM_TH_2_RSSI(env->fahm_th[2]),
			    FAHM_TH_2_RSSI(env->fahm_th[1]),
			    FAHM_TH_2_RSSI(env->fahm_th[0]));
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[FAHM] (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			    env->fahm_pwr, 5 * (env->fahm_pwr_0p5 & 0x1),
			    env->fahm_rpt[10], env->fahm_rpt[9], env->fahm_rpt[8],
			    env->fahm_rpt[7], env->fahm_rpt[6], env->fahm_rpt[5],
			    env->fahm_rpt[4], env->fahm_rpt[3], env->fahm_rpt[2],
			    env->fahm_rpt[1], env->fahm_rpt[0]);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "%-18s[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			    "  Th", FAHM_TH_2_RSSI(env->fahm_th[10]),
			    FAHM_TH_2_RSSI(env->fahm_th[9]),
			    FAHM_TH_2_RSSI(env->fahm_th[8]),
			    FAHM_TH_2_RSSI(env->fahm_th[7]),
			    FAHM_TH_2_RSSI(env->fahm_th[6]),
			    FAHM_TH_2_RSSI(env->fahm_th[5]),
			    FAHM_TH_2_RSSI(env->fahm_th[4]),
			    FAHM_TH_2_RSSI(env->fahm_th[3]),
			    FAHM_TH_2_RSSI(env->fahm_th[2]),
			    FAHM_TH_2_RSSI(env->fahm_th[1]),
			    FAHM_TH_2_RSSI(env->fahm_th[0]));
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[FAHM] (pwr:%02d.%d)[%.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d  %.2d]\n",
			    env->fahm_pwr, 5 * (env->fahm_pwr_0p5 & 0x1),
			    env->fahm_rpt[11], env->fahm_rpt[10], env->fahm_rpt[9],
			    env->fahm_rpt[8], env->fahm_rpt[7], env->fahm_rpt[6],
			    env->fahm_rpt[5], env->fahm_rpt[4], env->fahm_rpt[3],
			    env->fahm_rpt[2], env->fahm_rpt[1], env->fahm_rpt[0]);
	}
}

void halbb_basic_dbg_msg_pmac_cnsl(struct bb_info *bb, u32 *_used,
				   char *output, u32 *_out_len)
{
#ifdef HALBB_STATISTICS_SUPPORT
	struct bb_stat_info *stat = &bb->bb_stat_i;
	struct bb_fa_info *fa = &stat->bb_fa_i;
	struct bb_cck_fa_info *cck_fa = &fa->bb_cck_fa_i;
	struct bb_legacy_fa_info *legacy_fa = &fa->bb_legacy_fa_i;
	struct bb_ht_fa_info *ht_fa = &fa->bb_ht_fa_i;
	struct bb_vht_fa_info *vht_fa = &fa->bb_vht_fa_i;
	struct bb_he_fa_info *he_fa = &fa->bb_he_fa_i;
	struct bb_cca_info *cca = &stat->bb_cca_i;
	struct bb_crc_info *crc = &stat->bb_crc_i;
	//struct bb_crc2_info *crc2 = &stat_t->bb_crc2_i;

	if (bb->bb_watchdog_mode != BB_WATCHDOG_NORMAL)
		return;

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[Tx]{CCK_TxEN, CCK_TxON, OFDM_TxEN, OFDM_TxON}: {%d, %d, %d, %d}\n",
		    stat->bb_tx_cnt_i.cck_mac_txen, stat->bb_tx_cnt_i.cck_phy_txon,
		    stat->bb_tx_cnt_i.ofdm_mac_txen,
		    stat->bb_tx_cnt_i.ofdm_phy_txon);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[CRC]{B/G/N/AC/AX/All/MPDU} OK:{%d, %d, %d, %d, %d, %d, %d} Err:{%d, %d, %d, %d, %d, %d, %d}\n",
		    crc->cnt_cck_crc32_ok, crc->cnt_ofdm_crc32_ok,
		    crc->cnt_ht_crc32_ok, crc->cnt_vht_crc32_ok,
		    crc->cnt_he_crc32_ok, crc->cnt_crc32_ok_all,
		    crc->cnt_ampdu_crc_ok, crc->cnt_cck_crc32_error,
		    crc->cnt_ofdm_crc32_error, crc->cnt_ht_crc32_error,
		    crc->cnt_vht_crc32_error, crc->cnt_he_crc32_error,
		    crc->cnt_crc32_error_all, crc->cnt_ampdu_crc_error);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[CCA]{CCK, OFDM, All}: %d, %d, %d\n",
		    cca->cnt_cck_cca, cca->cnt_ofdm_cca, cca->cnt_cca_all);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[FA]{CCK, OFDM, All}: %d, %d, %d\n",
		    fa->cnt_cck_fail, fa->cnt_ofdm_fail, fa->cnt_fail_all);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    " *[CCK]sfd/sig_GG=%d/%d, *[OFDM]Prty=%d, Rate=%d, LSIG_brk_s/l=%d/%d, SBD=%d\n",
		    cck_fa->sfd_gg_cnt, cck_fa->sig_gg_cnt,
		    legacy_fa->cnt_parity_fail, legacy_fa->cnt_rate_illegal,
		    legacy_fa->cnt_lsig_brk_s_th, legacy_fa->cnt_lsig_brk_l_th,
		    legacy_fa->cnt_sb_search_fail);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    " *[HT]CRC8=%d, MCS=%d, *[VHT]SIGA_CRC8=%d, MCS=%d\n",
		    ht_fa->cnt_crc8_fail, ht_fa->cnt_mcs_fail,
		    vht_fa->cnt_crc8_fail_vhta, vht_fa->cnt_mcs_fail_vht);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    " *[HE]SIGA_CRC4{SU/ERSU/MU}=%d/%d/%d, SIGB_CRC4{ch1/ch2}=%d/%d, MCS{nrml/bcc/dcm}=%d/%d/%d\n",
		    he_fa->cnt_crc4_fail_hea_su, he_fa->cnt_crc4_fail_hea_ersu,
		    he_fa->cnt_crc4_fail_hea_mu, he_fa->cnt_crc4_fail_heb_ch1_mu,
		    he_fa->cnt_crc4_fail_heb_ch2_mu, he_fa->cnt_mcs_fail_he,
		    he_fa->cnt_mcs_fail_he_bcc, he_fa->cnt_mcs_fail_he_dcm);
#endif
}

void halbb_crc32_cnt2_cmn_log_cnsl(struct bb_info *bb, u32 *_used,
				   char *output, u32 *_out_len)
{
	struct bb_stat_info *stat_t = &bb->bb_stat_i;
	struct bb_crc2_info *crc2 = &stat_t->bb_crc2_i;
	struct bb_usr_set_info *usr_set = &stat_t->bb_usr_set_i;
	char dbg_buf[4][HALBB_SNPRINT_SIZE];

	halbb_mem_set(bb, dbg_buf, 0, sizeof(dbg_buf[0][0]) * 4 * HALBB_SNPRINT_SIZE);

	halbb_print_rate_2_buff(bb, usr_set->ofdm2_rate_idx,
				RTW_GILTF_LGI_4XHE32, dbg_buf[0], HALBB_SNPRINT_SIZE);
	halbb_print_rate_2_buff(bb, usr_set->ht2_rate_idx,
				RTW_GILTF_LGI_4XHE32, dbg_buf[1], HALBB_SNPRINT_SIZE);
	halbb_print_rate_2_buff(bb, usr_set->vht2_rate_idx,
				RTW_GILTF_LGI_4XHE32, dbg_buf[2], HALBB_SNPRINT_SIZE);
	halbb_print_rate_2_buff(bb, usr_set->he2_rate_idx,
				RTW_GILTF_LGI_4XHE32, dbg_buf[3], HALBB_SNPRINT_SIZE);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[CRC32 OK Cnt] {%s, %s, %s, %s}= {%d, %d, %d, %d}\n",
		    dbg_buf[0], dbg_buf[1], dbg_buf[2], dbg_buf[3],
		    crc2->cnt_ofdm2_crc32_ok, crc2->cnt_ht2_crc32_ok,
		    crc2->cnt_vht2_crc32_ok, crc2->cnt_he2_crc32_ok);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[CRC32 Err Cnt] {%s, %s, %s , %s}= {%d, %d, %d, %d}\n",
		    dbg_buf[0], dbg_buf[1], dbg_buf[2], dbg_buf[3],
		    crc2->cnt_ofdm2_crc32_error, crc2->cnt_ht2_crc32_error,
		    crc2->cnt_vht2_crc32_error, crc2->cnt_he2_crc32_error);
}

void halbb_crc32_cnt3_cmn_log_cnsl(struct bb_info *bb, u32 *_used,
				   char *output, u32 *_out_len)
{
	struct bb_stat_info *stat_t = &bb->bb_stat_i;
	struct bb_usr_set_info *usr_set = &stat_t->bb_usr_set_i;
	struct bb_crc2_info *crc2 = &stat_t->bb_crc2_i;

	u32 total_cnt = 0;
	u8 pcr = 0;
	total_cnt = crc2->cnt_ofdm3_crc32_ok + crc2->cnt_ofdm3_crc32_error;
	pcr = (u8)HALBB_DIV(crc2->cnt_ofdm3_crc32_ok * 100, total_cnt);

	switch(usr_set->stat_type_sel_i) {
	case STATE_PROBE_RESP:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Probe Response Data CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_BEACON:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Beacon CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_ACTION:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Action CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_BFRP:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[BFRP CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_NDPA:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[NDPA CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_BA:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[BA CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_RTS:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[RTS CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_CTS:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[CTS CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_ACK:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[ACK CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_DATA:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[DATA CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_NULL:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[Null CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	case STATE_QOS:
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[QoS CRC32 Cnt(OFDM only)] {error, ok}= {%d, %d} (PCR=%d percent)\n",
			    crc2->cnt_ofdm3_crc32_error,
			    crc2->cnt_ofdm3_crc32_ok, pcr);
		break;
	default:
		break;
	}
}

void halbb_ra_dbgreg_cnsl(struct bb_info *bb, u32 *_used, char *output,
			  u32 *_out_len)
{
	struct rtw_phl_com_t *phl = bb->phl_com;
	struct dev_cap_t *dev = &phl->dev_cap;
	struct bb_dbg_info *dbg = &bb->bb_dbg_i;
	struct bb_ra_dbgreg *dbgreg = &dbg->ra_dbgreg_i;

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[RA dbgreg]CMAC_tbl DWORD0{macid0,macid1}={0x%x,0x%x}\n",
		    dbgreg->cmac_tbl_id0, dbgreg->cmac_tbl_id1);
	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B) ||
	    (bb->ic_type == BB_RTL8851B)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "{PER,RDR,R4}={%d,%d,%d}, try:{PER,RDR,R4}={%d,%d,%d}\n",
			    dbgreg->per, dbgreg->rdr, dbgreg->r4,
			    dbgreg->try_per, dbgreg->try_rdr, dbgreg->try_r4);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "{PER,RDR,R4,cls}={%d,%d,%d,%d}, try:{PER,RDR,R4}={%d,%d,%d}\n",
			    dbgreg->per, dbgreg->rdr, dbgreg->r4, dbgreg->cls,
			    dbgreg->try_per, dbgreg->try_rdr, dbgreg->try_r4);
	}
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "{up_lmt_cnt,PER_ma,VAR}={%d,%d,%d},d_o:{n,p}={%d,%d},TH:{RD,RU}={%d,%d}\n",
		    dbgreg->rate_up_lmt_cnt, dbgreg->per_ma, dbgreg->var,
		    dbgreg->d_o_n, dbgreg->d_o_p, dbgreg->rd_th, dbgreg->ru_th);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "TxRPT.tot=%d,RAtimer=%d, RAreturn:{tot=0/disra/trying, R4}={%d,%d}\n",
		    dbgreg->txrpt_tot, dbgreg->ra_timer,
		    dbgreg->tot_disra_trying_return, dbgreg->r4_return);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "RA mask[H->L]={0x%x,0x%x}, highest_rate=0x%x, lowest_rate=0x%x\n",
		    dbgreg->ra_mask_h, dbgreg->ra_mask_l,
		    dbgreg->highest_rate, dbgreg->lowest_rate);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "RA registered, H2C DWORD[0->3]={0x%x,0x%x,0x%x,0x%x}\n",
		    dbgreg->upd_all_h2c_0, dbgreg->upd_all_h2c_1,
		    dbgreg->upd_all_h2c_2, dbgreg->upd_all_h2c_3);
	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "dyn_stbc:{en,ok,banned,fail_cnt}={%d,%d,%d,%d}\n",
			    (dbgreg->dyn_stbc >> 7) & 0x1,
			    (dbgreg->dyn_stbc >> 6) & 0x1,
			    (dbgreg->dyn_stbc >> 5) & 0x1,
			    dbgreg->dyn_stbc & 0x1f);
	}
	/*MU MIMO RA*/
	if (dev->rfe_type >= 50) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "==== MU MIMO RA ====\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "sta_1 (su macid 1) : mu_mcs=%d, su_mcs=%d, mu_id=%d, lowest_rate=0x%x\n",
			    dbgreg->mu_mcs & MASKBYTE0,
			    (dbgreg->mu_mcs & MASKBYTE1) >> 8,
			    dbgreg->mu_id_lowest_rate & MASKBYTE0,
			    (dbgreg->mu_id_lowest_rate & MASKBYTE1) >> 8);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "mu_rd_th=%d, mu_ru_th=%d, mu_per=%d, mu_try_per=%d\n",
			    (dbgreg->mu_rd_ru_th & MASKBYTE1) >> 8,
			    dbgreg->mu_rd_ru_th & MASKBYTE0,
			    dbgreg->mu_per & MASKBYTE0,
			    (dbgreg->mu_per & MASKBYTE1) >> 8);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "sta_2 (non su macid 1) : mu_mcs=%d, su_mcs=%d, mu_id=%d, lowest_rate=0x%x\n",
			    (dbgreg->mu_mcs & MASKBYTE2) >> 16,
			    (dbgreg->mu_mcs & MASKBYTE3) >> 24,
			    (dbgreg->mu_id_lowest_rate & MASKBYTE2) >> 16,
			    (dbgreg->mu_id_lowest_rate & MASKBYTE3) >> 24);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "mu_rd_th=%d, mu_ru_th=%d, mu_per=%d, mu_try_per=%d\n",
			    (dbgreg->mu_rd_ru_th & MASKBYTE3) >> 24,
			    (dbgreg->mu_rd_ru_th & MASKBYTE2) >> 16,
			    (dbgreg->mu_per & MASKBYTE2) >> 16,
			    (dbgreg->mu_per & MASKBYTE3) >> 24);
	}
}

void halbb_basic_dbg_msg_tx_dbg_reg_cnsl(struct bb_info *bb, u32 *_used,
					 char *output, u32 *_out_len)
{
	struct bb_dbg_info *dbg = &bb->bb_dbg_i;
	struct bb_tx_info *txdbg = &dbg->tx_info_i;
	s32 pw = 0;
	u8 i = 0;
	char ppdu[][10] = {{"L-CCK"}, {"S-CCK"}, {"Legacy"}, {"HT"},
			   {"HT GF"}, {"VHT SU"}, {"VHT MU"}, {"HE SU"},
			   {"HE ER SU"}, {"HE MU"}, {"HE TB"}, {"RSVD-11"},
			   {"RSVD-12"}, {"RSVD-13"}, {"RSVD-14"}, {"RSVD-15"}};
	char gi_type[][4] = {{"0.4"}, {"0.8"}, {"1.6"}, {"3.2"}};
	char fec_type[][5] = {{"BCC"}, {"LDPC"}};
	char precoding_type[][8] = {{"normal"}, {"TxBF"}, {"MU-MIMO"}};
	char *txcmd = NULL;
	char tx_pw0[HALBB_SNPRINT_SIZE];
	char tx_pw1[HALBB_SNPRINT_SIZE];

	if (bb->bb_watchdog_mode != BB_WATCHDOG_NORMAL)
		return;

	/*Store the whole tx_dbg register to avoid timing issue*/
	halbb_get_tx_dbg_reg(bb);

	pw = halbb_cnvrt_2_sign((u32)txdbg->txpw_path0, 9);
	halbb_print_sign_frac_digit(bb, (u32)pw, 9, 2, tx_pw0,
				    HALBB_SNPRINT_SIZE);
	if (bb->num_rf_path >= 2) {
		pw = halbb_cnvrt_2_sign((u32)txdbg->txpw_path1, 9);
		halbb_print_sign_frac_digit(bb, (u32)pw, 9, 2, tx_pw1,
					    HALBB_SNPRINT_SIZE);
	}

	if (bb->ic_type == BB_RTL8852C) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used, "txpw_bb={%s,%s}dBm\n",
			    tx_pw0, tx_pw1);
		return;
	}

	if ((txdbg->tx_path_en == 0) && (txdbg->tx_pw == 0) &&
	    (txdbg->l_sig == 0)) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "Txinfo is empty!BB reset has been probably toggled.\n");
		return;
	}

	pw = halbb_cnvrt_2_sign((u32)txdbg->tx_pw, 9);
	halbb_print_sign_frac_digit(bb, (u32)pw, 9, 2, bb->dbg_buf, HALBB_SNPRINT_SIZE);

	/*Move txcmd to array declaration would cause warning due to larger frame size*/
	switch (txdbg->txcmd_num) {
	case 0:
		txcmd = "data";
		break;
	case 1:
		txcmd = "beacon";
		break;
	case 2:
		txcmd = "HT-NDPA";
		break;
	case 3:
		txcmd = "VHT-NDPA";
		break;
	case 4:
		txcmd = "HE-NDPA";
		break;
	case 8:
		txcmd = "RTS";
		break;
	case 9:
		txcmd = "CTS2self";
		break;
	case 10:
		txcmd = "CF_end";
		break;
	case 11:
		txcmd = "compressed-BAR";
		break;
	case 12:
		txcmd = "BFRP";
		break;
	case 13:
		txcmd = "NDP";
		break;
	case 14:
		txcmd = "QoS_Null";
		break;
	case 16:
		txcmd = "ACK";
		break;
	case 17:
		txcmd = "CTS";
		break;
	case 18:
		txcmd = "compressed-BA";
		break;
	case 19:
		txcmd = "Multi-STA-BA";
		break;
	case 20:
		txcmd = "HT-CSI";
		break;
	case 21:
		txcmd = "VHT-CSI";
		break;
	case 22:
		txcmd = "HE-CSI";
		break;
	case 31:
		txcmd = "TB_PPDU";
		break;
	case 32:
		txcmd = "TRIG-BASIC";
		break;
	case 33:
		txcmd = "TRIG-BFRP";
		break;
	case 34:
		txcmd = "TRIG-MUBAR";
		break;
	case 35:
		txcmd = "TRIG-MU-RTS";
		break;
	case 36:
		txcmd = "TRIG-BSRP";
		break;
	case 37:
		txcmd = "TRIG-BQRP";
		break;
	case 38:
		txcmd = "TRIG-NFRP";
		break;
	case 48:
		txcmd = "TRIG-BASIC-DATA";
		break;
	default:
		txcmd = "RSVD";
		break;
	}
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[%s][%s-%d] BW=%dM, TxSC=%d, TxPathEn=%d, PathMap=0x%x\n",
		    ppdu[txdbg->type], txcmd, txdbg->txcmd_num, 20 << txdbg->bw,
		    txdbg->txsc, txdbg->tx_path_en, txdbg->path_map);
	if ((bb->ic_type == BB_RTL8852A) || (bb->ic_type == BB_RTL8852B))
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "User_num=%d, TxPw:tmac=%sdBm/bb=(%s,%s)dBm\n",
			    txdbg->n_usr, bb->dbg_buf, tx_pw0, tx_pw1);
	else if (bb->ic_type == BB_RTL8851B)
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "User_num=%d, TxPw:tmac=%sdBm/bb=%sdBm\n",
			    txdbg->n_usr, bb->dbg_buf, tx_pw0);
	else
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "User_num=%d, TxPw:tmac=%sdBm/bb=(%s,%s)dBm, max_mcs=%d\n",
			    txdbg->n_usr, bb->dbg_buf, tx_pw0, tx_pw1,
			    txdbg->max_mcs);

	if (txdbg->type > 6) { // === HE === //
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used, "STBC=%d, GILTF=%dx%s\n",
			    txdbg->stbc, 1 << txdbg->ltf, gi_type[txdbg->gi]);
		for (i = 0; i < txdbg->n_usr; i++) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "U_id=%d, FEC=%s, N_sts=%d, MCS=%d, precoding=%s, DCM=%d\n",
				    txdbg->u_id[i], fec_type[txdbg->fec[i]],
				    txdbg->n_sts[i], txdbg->mcs[i],
				    precoding_type[txdbg->precoding[i]],
				    txdbg->dcm[i]);
		}
		if (txdbg->type == 7) /*SU only temporarily*/
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "n_sym=%d, t_data=%d us, PSDU_length=%d Bytes, pre_fec=%d, pkt_ext=%d us\n",
				    txdbg->n_sym, txdbg->t_data,
				    txdbg->psdu_length, txdbg->pre_fec,
				    txdbg->pkt_ext << 2);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "L-SIG/HE-SIG-A1/HE-SIG-A2={0x%08x, 0x%08x, 0x%08x}\n",
			    txdbg->l_sig, txdbg->sig_a1, txdbg->sig_a2);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "TxInfo={0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
			    txdbg->txinfo[0], txdbg->txinfo[1],
			    txdbg->txinfo[2], txdbg->txinfo[3]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "TxComCt={0x%08x, 0x%08x}, TxTimCt=0x%08x\n",
			    txdbg->txcomct[0], txdbg->txcomct[1],
			    txdbg->txtimct);
		for (i = 0; i < txdbg->n_usr; i++) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "U_id=%d, TxUsrCt={0x%08x, 0x%08x}\n",
				    txdbg->u_id[i], txdbg->txusrct[i][0],
				    txdbg->txusrct[i][1]);
		}
	} else if (txdbg->type > 4) { // === VHT === //
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used, "STBC=%d, GI=%s\n",
			    txdbg->stbc, gi_type[txdbg->gi]);
		for (i = 0; i < txdbg->n_usr; i++) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "U_id=%d, FEC=%s, N_sts=%d, MCS=%d, precoding=%s\n",
				    txdbg->u_id[i], fec_type[txdbg->fec[i]],
				    txdbg->n_sts[i], txdbg->mcs[i],
				    precoding_type[txdbg->precoding[i]]);
		}
		if (txdbg->type == 5) /*SU only temporarily*/
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "n_sym=%d, t_data=%d us, VHT_length=%d Bytes\n",
				    txdbg->n_sym, txdbg->t_data,
				    txdbg->psdu_length);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "L-SIG/VHT-SIG-A1/VHT-SIG-A2/VHT-SIG-B={0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
			    txdbg->l_sig, txdbg->sig_a1, txdbg->sig_a2,
			    txdbg->sig_b);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "TxInfo={0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
			    txdbg->txinfo[0], txdbg->txinfo[1],
			    txdbg->txinfo[2], txdbg->txinfo[3]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "TxComCt={0x%08x, 0x%08x}, TxTimCt=0x%08x\n",
			    txdbg->txcomct[0], txdbg->txcomct[1],
			    txdbg->txtimct);
		for (i = 0; i < txdbg->n_usr; i++) {
			BB_DBG_CNSL(*_out_len, *_used, output + *_used,
				    *_out_len - *_used,
				    "U_id=%d, TxUsrCt={0x%08x, 0x%08x}\n",
				    txdbg->u_id[i], txdbg->txusrct[i][0],
				    txdbg->txusrct[i][1]);
		}
	} else if (txdbg->type > 2) { // === HT === //
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "STBC=%d, FEC=%s, GI=%s, N_sts=%d, MCS=%d\n",
			    txdbg->stbc, fec_type[txdbg->fec[0]],
			    gi_type[txdbg->gi], txdbg->n_sts[0], txdbg->mcs[0]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "n_sym=%d, t_data=%d us, HT_length=%d Bytes\n",
			    txdbg->n_sym, txdbg->t_data, txdbg->psdu_length);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "L-SIG/HT-SIG1/HT-SIG2={0x%08x, 0x%08x, 0x%08x}\n",
			    txdbg->l_sig, txdbg->sig_a1, txdbg->sig_a2);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "TxInfo={0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
			    txdbg->txinfo[0], txdbg->txinfo[1],
			    txdbg->txinfo[2], txdbg->txinfo[3]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "TxComCt={0x%08x, 0x%08x}, TxTimCt=0x%08x\n",
			    txdbg->txcomct[0], txdbg->txcomct[1],
			    txdbg->txtimct);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "U_id=%d, TxUsrCt={0x%08x, 0x%08x}\n",
			    txdbg->u_id[0], txdbg->txusrct[0][0],
			    txdbg->txusrct[0][1]);
	} else if (txdbg->type > 1) { // === OFDM === //
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used, "rate=%dM\n",
			    bb_phy_rate_table[4 + txdbg->mcs[0]]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "n_sym=%d, t_data=%d us, L_length=%d Bytes\n",
			    txdbg->n_sym, txdbg->t_data, txdbg->psdu_length);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "L-SIG={0x%08x}\n", txdbg->l_sig);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "TxInfo={0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
			    txdbg->txinfo[0], txdbg->txinfo[1],
			    txdbg->txinfo[2], txdbg->txinfo[3]);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "TxComCt={0x%08x, 0x%08x}, TxTimCt=0x%08x\n",
			    txdbg->txcomct[0], txdbg->txcomct[1],
			    txdbg->txtimct);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "U_id=%d, TxUsrCt={0x%08x, 0x%08x}\n",
			    txdbg->u_id[0], txdbg->txusrct[0][0],
			    txdbg->txusrct[0][1]);
	} else { // === CCK === //
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "TxInfo={0x%08x, 0x%08x, 0x%08x, 0x%08x}\n",
			    txdbg->txinfo[0], txdbg->txinfo[1],
			    txdbg->txinfo[2], txdbg->txinfo[3]);
	}
}

void halbb_basic_dbg_msg_tx_info_cnsl(struct bb_info *bb, u32 *_used,
				      char *output, u32 *_out_len)
{
	struct bb_ch_info *ch = &bb->bb_ch_i;
	struct rtw_phl_stainfo_t *sta;
	struct rtw_ra_sta_info	*ra;
	u16 sta_cnt = 0;
	u8 i = 0;
	u8 tmp = 0;
	u16 curr_tx_rt = 0;
	enum rtw_gi_ltf curr_gi_ltf = RTW_GILTF_LGI_4XHE32;
	enum hal_rate_bw curr_bw = HAL_RATE_BW_20;

	for (i = 0; i < PHL_MAX_STA_NUM; i++) {
		if (!bb->sta_exist[i])
			continue;
		sta = bb->phl_sta_info[i];
		if (!is_sta_active(sta))
			continue;

		ra = &sta->hal_sta->ra_info;
		if (bb->ic_type & BB_IC_AX_SERIES)
			curr_tx_rt = (u16)(ra->rpt_rt_i.mcs_ss_idx & 0x7f) | ((u16)(ra->rpt_rt_i.mode & 0x3) << 7);
		else
			curr_tx_rt = (u16)(ra->rpt_rt_i.mcs_ss_idx & 0xff) | ((u16)(ra->rpt_rt_i.mode & 0xf) << 8);

		curr_gi_ltf = ra->rpt_rt_i.gi_ltf;
		curr_bw = ra->rpt_rt_i.bw;

		halbb_print_rate_2_buff(bb, curr_tx_rt, curr_gi_ltf, bb->dbg_buf, HALBB_SNPRINT_SIZE);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "TxRate[%d]=%s (0x%x-%d), PER=(%d), TXBW=(%d)\n",
			    i, bb->dbg_buf, curr_tx_rt, curr_gi_ltf,
			    ra->curr_retry_ratio, (20<<curr_bw));
		sta_cnt++;
		if (sta_cnt >= bb->hal_com->assoc_sta_cnt)
			break;
	}
	halbb_get_ra_dbgreg(bb);
	halbb_ra_dbgreg_cnsl(bb, _used, output, _out_len);
}

void halbb_basic_dbg_msg_rx_info_cnsl(struct bb_info *bb, u32 *_used,
				      char *output, u32 *_out_len)
{
	struct bb_ch_info *ch = &bb->bb_ch_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_cap_info *pkt_cnt_cap = &cmn_rpt->bb_pkt_cnt_all_i;
	struct bb_physts_pop_info *pop_info = &cmn_rpt->bb_physts_pop_i;
	struct bb_dbg_cr_info *cr = &bb->bb_dbg_i.bb_dbg_cr_i;
	u8 tmp = 0;
	u32 bb_monitor1 = 0;

	if (bb->bb_watchdog_mode != BB_WATCHDOG_NORMAL)
		return;	

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "rxsc_idx {Lgcy, 20, 40, 80} = {%d, %d, %d, %d}\n",
		    ch->rxsc_l, ch->rxsc_20, ch->rxsc_40, ch->rxsc_80);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		"RX Pkt Cnt: LDPC=(%d), BCC=(%d), STBC=(%d), SU_BF=(%d), MU_BF=(%d), \n",
		    pkt_cnt_cap->pkt_cnt_ldpc, pkt_cnt_cap->pkt_cnt_bcc,
		    pkt_cnt_cap->pkt_cnt_stbc, pkt_cnt_cap->pkt_cnt_subf,
		    pkt_cnt_cap->pkt_cnt_mubf);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		"Dly_sprd=(%d)\n", tmp);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[POP] cnt=%d, hist_cck/ofdm[0:3]={%d | %d, %d, %d}/{%d | %d, %d, %d}\n",
		    bb->bb_stat_i.bb_cca_i.pop_cnt,
		    pop_info->pop_hist_cck[0], pop_info->pop_hist_cck[1],
		    pop_info->pop_hist_cck[2], pop_info->pop_hist_cck[3],
		    pop_info->pop_hist_ofdm[0], pop_info->pop_hist_ofdm[1],
		    pop_info->pop_hist_ofdm[2], pop_info->pop_hist_ofdm[3]);

	halbb_set_reg(bb, cr->bb_monitor_sel1, cr->bb_monitor_sel1_m, 1);
	bb_monitor1 = halbb_get_reg(bb, cr->bb_monitor1, cr->bb_monitor1_m);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "BB monitor1 = (0x%x)\n", bb_monitor1);
}


void halbb_basic_dbg_msg_physts_su_cnsl(struct bb_info *bb, u32 *_used,
					char *output, u32 *_out_len)
{
	struct bb_ch_info *ch = &bb->bb_ch_i;
	struct bb_link_info *link = &bb->bb_link_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bb_rssi_su_acc_info *acc = &cmn_rpt->bb_rssi_su_acc_i;
	struct bb_rssi_su_avg_info *avg = &cmn_rpt->bb_rssi_su_avg_i;
	u8 rssi_avg_tmp = 0;
	u8 rssi_tmp[HALBB_MAX_PATH];
	u16 pkt_cnt_ss = 0;
	u8 i = 0, j =0;
	u8 rate_num = bb->num_rf_path, ss_ofst = 0;
	char dbg_buf2[32] = {0};
	u16 avg_phy_rate = 0, utility = 0;

	/*RX Rate*/
	halbb_print_rate_2_buff(bb, link->rx_rate_plurality,
				RTW_GILTF_LGI_4XHE32, dbg_buf2, 32);

	halbb_print_rate_2_buff(bb, cmn_rpt->bb_pkt_cnt_bcn_i.beacon_phy_rate,
				RTW_GILTF_LGI_4XHE32, bb->dbg_buf, HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "Plurality_RxRate:%s (0x%x), Bcn_Rate=%s (0x%x), Bcn_cnt=%d\n",
		    dbg_buf2, link->rx_rate_plurality,
		    bb->dbg_buf ,cmn_rpt->bb_pkt_cnt_bcn_i.beacon_phy_rate,
		    cmn_rpt->bb_pkt_cnt_bcn_i.pkt_cnt_beacon);

	/*RX Rate Distribution & RSSI*/
#if 1

	if (bb->bb_link_i.wlan_mode_bitmap & WLAN_MD_11AX) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[GI_LTF] {4x32/4x08/2x16/2x08/1x16/1x08}={%d, %d, %d, %d, %d, %d}\n",
			    pkt_cnt->gi_ltf_cnt[0], pkt_cnt->gi_ltf_cnt[1],
			    pkt_cnt->gi_ltf_cnt[2], pkt_cnt->gi_ltf_cnt[3],
			    pkt_cnt->gi_ltf_cnt[4], pkt_cnt->gi_ltf_cnt[5]);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[GI] {LGI:%d, SGI:%d}\n",
			    pkt_cnt->gi_ltf_cnt[0], pkt_cnt->gi_ltf_cnt[1]);
	}

	avg->rssi_cck_avg = (u8)HALBB_DIV(acc->rssi_cck_avg_acc, pkt_cnt->pkt_cnt_cck);
	avg->rssi_ofdm_avg = (u8)HALBB_DIV(acc->rssi_ofdm_avg_acc, pkt_cnt->pkt_cnt_ofdm);
	avg->rssi_t_avg = (u8)HALBB_DIV(acc->rssi_t_avg_acc, pkt_cnt->pkt_cnt_t);
		
	for (i = 0; i < HALBB_MAX_PATH; i++) {
		if (i >= bb->num_rf_path)
			break;

		avg->rssi_cck[i] = (u8)HALBB_DIV(acc->rssi_cck_acc[i], pkt_cnt->pkt_cnt_cck);
		avg->rssi_ofdm[i] = (u8)HALBB_DIV(acc->rssi_ofdm_acc[i], pkt_cnt->pkt_cnt_ofdm);
		avg->rssi_t[i] = (u8)HALBB_DIV(acc->rssi_t_acc[i], pkt_cnt->pkt_cnt_t);
		//BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used, "*rssi_ofdm_avg %02d =  rssi_ofdm_acc %02d / pkt_cnt_ofdm%02d}\n",
		//	avg->rssi_ofdm_avg, avg->rssi_ofdm[i], acc->rssi_ofdm_acc[i], pkt_cnt->pkt_cnt_ofdm);
	}

	/*@======[Lgcy-non-data]=============================================*/
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[Lgcy-non-data] {%d, %d, %d, %d | %d, %d, %d, %d, %d, %d, %d, %d} {%d}\n",
		    pkt_cnt->pkt_cnt_legacy_non_data[0], pkt_cnt->pkt_cnt_legacy_non_data[1],
		    pkt_cnt->pkt_cnt_legacy_non_data[2], pkt_cnt->pkt_cnt_legacy_non_data[3],
		    pkt_cnt->pkt_cnt_legacy_non_data[4], pkt_cnt->pkt_cnt_legacy_non_data[5],
		    pkt_cnt->pkt_cnt_legacy_non_data[6], pkt_cnt->pkt_cnt_legacy_non_data[7],
		    pkt_cnt->pkt_cnt_legacy_non_data[8], pkt_cnt->pkt_cnt_legacy_non_data[9],
		    pkt_cnt->pkt_cnt_legacy_non_data[10], pkt_cnt->pkt_cnt_legacy_non_data[11],
		    pkt_cnt->pkt_cnt_else_non_data);

	/*@======CCK=========================================================*/
	if (bb->num_rf_path >= 2)
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "*CCK     RSSI:{%02d| %02d,%02d} cnt:{%03d| %d, %d, %d, %d}\n",
			    avg->rssi_cck_avg >> 1,
			    avg->rssi_cck[0] >> 1, avg->rssi_cck[1] >> 1,
			    pkt_cnt->pkt_cnt_cck,
			    pkt_cnt->pkt_cnt_legacy[0],
			    pkt_cnt->pkt_cnt_legacy[1],
			    pkt_cnt->pkt_cnt_legacy[2],
			    pkt_cnt->pkt_cnt_legacy[3]);
	else
		BB_DBG_CNSL(*_out_len, *_used, output +
			    *_used, *_out_len - *_used,
			    "*CCK     RSSI:{%02d| %02d} cnt:{%03d| %d, %d, %d, %d}\n",
			    avg->rssi_cck_avg >> 1,
			    avg->rssi_cck[0] >> 1,
			    pkt_cnt->pkt_cnt_cck,
			    pkt_cnt->pkt_cnt_legacy[0],
			    pkt_cnt->pkt_cnt_legacy[1],
			    pkt_cnt->pkt_cnt_legacy[2],
			    pkt_cnt->pkt_cnt_legacy[3]);
	/*@======OFDM========================================================*/
	if (bb->num_rf_path >= 2)
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "*OFDM    RSSI:{%02d| %02d,%02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d}\n",
			    avg->rssi_ofdm_avg >> 1,
			    avg->rssi_ofdm[0] >> 1, avg->rssi_ofdm[1] >> 1,
			    pkt_cnt->pkt_cnt_ofdm,
			    pkt_cnt->pkt_cnt_legacy[4],
			    pkt_cnt->pkt_cnt_legacy[5],
			    pkt_cnt->pkt_cnt_legacy[6],
			    pkt_cnt->pkt_cnt_legacy[7],
			    pkt_cnt->pkt_cnt_legacy[8],
			    pkt_cnt->pkt_cnt_legacy[9],
			    pkt_cnt->pkt_cnt_legacy[10],
			    pkt_cnt->pkt_cnt_legacy[11]);
	else
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "*OFDM    RSSI:{%02d| %02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d}\n",
			    avg->rssi_ofdm_avg >> 1,
			    avg->rssi_ofdm[0] >> 1,
			    pkt_cnt->pkt_cnt_ofdm,
			    pkt_cnt->pkt_cnt_legacy[4],
			    pkt_cnt->pkt_cnt_legacy[5],
			    pkt_cnt->pkt_cnt_legacy[6],
			    pkt_cnt->pkt_cnt_legacy[7],
			    pkt_cnt->pkt_cnt_legacy[8],
			    pkt_cnt->pkt_cnt_legacy[9],
			    pkt_cnt->pkt_cnt_legacy[10],
			    pkt_cnt->pkt_cnt_legacy[11]);
	/*@======HT==========================================================*/

	if (pkt_cnt->ht_pkt_not_zero) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = (i << 3);
			for (j = 0; j < HT_NUM_MCS ; j++) {
					pkt_cnt_ss += pkt_cnt->pkt_cnt_ht[ss_ofst + j];
			}

			if (pkt_cnt_ss == 0) {
				rssi_avg_tmp = 0;
				rssi_tmp[0] = 0;
				rssi_tmp[1] = 0;
			} else {
				rssi_avg_tmp = avg->rssi_t_avg >> 1;
				rssi_tmp[0] = avg->rssi_t[0] >> 1;
				rssi_tmp[1] = avg->rssi_t[1] >> 1;
			}
			if (bb->num_rf_path >= 2)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*HT%02d:%02d RSSI:{%02d| %02d,%02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (ss_ofst), (ss_ofst + 7),
					    rssi_avg_tmp, rssi_tmp[0],
					    rssi_tmp[1],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 7]);

			else
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*HT%02d:%02d RSSI:{%02d| %02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (ss_ofst), (ss_ofst + 7),
					    rssi_avg_tmp, rssi_tmp[0],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_ht[ss_ofst + 7]);

			pkt_cnt_ss = 0;
		}
	}

	/*@======VHT==========================================================*/
	if (pkt_cnt->vht_pkt_not_zero) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = HE_VHT_NUM_MCS * i;

			for (j = 0; j < HE_VHT_NUM_MCS ; j++) {
				pkt_cnt_ss += pkt_cnt->pkt_cnt_vht[ss_ofst + j];
			}

			if (pkt_cnt_ss == 0) {
				rssi_avg_tmp = 0;
				rssi_tmp[0] = 0;
				rssi_tmp[1] = 0;
			} else {
				rssi_avg_tmp = avg->rssi_t_avg >> 1;
				rssi_tmp[0] = avg->rssi_t[0] >> 1;
				rssi_tmp[1] = avg->rssi_t[1] >> 1;
			}
			if (bb->num_rf_path >= 2)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*VHT %d-S RSSI:{%02d| %02d,%02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (i + 1),
					    rssi_avg_tmp, rssi_tmp[0],
					    rssi_tmp[1],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 7],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 8],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 9],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 10],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 11]);
			else
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*VHT %d-S RSSI:{%02d| %02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (i + 1),
					    rssi_avg_tmp, rssi_tmp[0],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 7],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 8],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 9],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 10],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 11]);

			pkt_cnt_ss = 0;
		}

	}

	/*@======HE==========================================================*/
	if (pkt_cnt->he_pkt_not_zero) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = HE_VHT_NUM_MCS * i;

			for (j = 0; j < HE_VHT_NUM_MCS ; j++) {
				pkt_cnt_ss += pkt_cnt->pkt_cnt_he[ss_ofst + j];
			}

			if (pkt_cnt_ss == 0) {
				rssi_avg_tmp = 0;
				rssi_tmp[0] = 0;
				rssi_tmp[1] = 0;
			} else {
				rssi_avg_tmp = avg->rssi_t_avg >> 1;
				rssi_tmp[0] = avg->rssi_t[0] >> 1;
				rssi_tmp[1] = avg->rssi_t[1] >> 1;
			}
			if (bb->num_rf_path >= 2)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*HE %d-SS RSSI:{%02d| %02d,%02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (i + 1),
					    rssi_avg_tmp, rssi_tmp[0],
					    rssi_tmp[1],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_he[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 7],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 8],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 9],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 10],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 11]);
			else
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*HE %d-SS RSSI:{%02d| %02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (i + 1),
					    rssi_avg_tmp, rssi_tmp[0],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_he[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 7],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 8],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 9],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 10],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 11]);

			pkt_cnt_ss = 0;
		}

	}

	/*@======SC_BW========================================================*/

	if (pkt_cnt->sc20_occur) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = 12 * i;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "*[Low BW 20M] %d-ss MCS[0:11] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				    (i + 1),
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 0],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 1],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 2],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 3],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 4],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 5],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 6],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 7],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 8],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 9],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 10],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 11]);
		}
	}

	if (pkt_cnt->sc40_occur) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = 12 * i;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "*[Low BW 40M] %d-ss MCS[0:11] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				    (i + 1),
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 0],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 1],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 2],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 3],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 4],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 5],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 6],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 7],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 8],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 9],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 10],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 11]);
		}
	}

	if (pkt_cnt->sc80_occur) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = 12 * i;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "*[Low BW 80M] %d-ss MCS[0:11] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				    (i + 1),
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 0],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 1],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 2],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 3],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 4],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 5],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 6],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 7],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 8],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 9],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 10],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 11]);
		}
	}
#endif

	/*RX Utility*/
	avg_phy_rate = halbb_rx_avg_phy_rate(bb);
	utility = halbb_rx_utility(bb, avg_phy_rate, bb->num_rf_path, bb->hal_com->band[0].cur_chandef.bw);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "Avg_rx_rate = %d, rx_utility=( %d / 1000 )\n",
		    avg_phy_rate, utility);
}


void halbb_show_phy_hitogram_su_cnsl(struct bb_info *bb, u32 *_used,
						 char *output, u32 *_out_len)
{
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bb_physts_acc_info *acc = &cmn_rpt->bb_physts_acc_i;
	struct bb_physts_avg_info *avg = &cmn_rpt->bb_physts_avg_i;
	struct bb_physts_hist_info *hist = &cmn_rpt->bb_physts_hist_i;
	struct bb_physts_hist_th_info *hist_th = &cmn_rpt->bb_physts_hist_th_i;
	char buf[HALBB_SNPRINT_SIZE] = {0};
	u16 valid_cnt = pkt_cnt->pkt_cnt_t + pkt_cnt->pkt_cnt_ofdm;

	/*=== [EVM, SNR] =====================================================*/

	halbb_print_hist_2_buf_u8(bb, hist_th->evm_hist_th, BB_HIST_TH_SIZE, bb->dbg_buf,
			       HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "  %-8s %-9s  %s\n", "[TH]", "(Avg)", bb->dbg_buf);
	/*val*/
	avg->evm_1ss = (u8)HALBB_DIV(acc->evm_1ss, (pkt_cnt->pkt_cnt_1ss + pkt_cnt->pkt_cnt_ofdm));
	halbb_print_hist_2_buf(bb, hist->evm_1ss, BB_HIST_SIZE, bb->dbg_buf,
			       HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "%-9s (%02d.%03d)  %s\n", "[EVM_1ss]",
		    (avg->evm_1ss >> 2),
	       halbb_show_fraction_num(avg->evm_1ss & 0x3, 2), bb->dbg_buf);

	avg->evm_max = (u8)HALBB_DIV(acc->evm_max_acc, pkt_cnt->pkt_cnt_2ss);
	halbb_print_hist_2_buf(bb, hist->evm_max_hist, BB_HIST_SIZE, bb->dbg_buf,
			       HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "%-9s (%02d.%03d)  %s\n", "[EVM_max]",
		    (avg->evm_max >> 2),
		    halbb_show_fraction_num(avg->evm_max & 0x3, 2), bb->dbg_buf);
	
	avg->evm_min = (u8)HALBB_DIV(acc->evm_min_acc, pkt_cnt->pkt_cnt_2ss);
	halbb_print_hist_2_buf(bb, hist->evm_min_hist, BB_HIST_SIZE, bb->dbg_buf,
			       HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "%-9s (%02d.%03d)  %s\n", "[EVM_min]",
		    (avg->evm_min >> 2),
		    halbb_show_fraction_num(avg->evm_min & 0x3, 2), bb->dbg_buf);
	

	avg->snr_avg = (u8)HALBB_DIV(acc->snr_avg_acc, valid_cnt);
	halbb_print_hist_2_buf(bb, hist->snr_avg_hist, BB_HIST_SIZE, bb->dbg_buf,
			       HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "%-9s (%02d.000)  %s\n", "[SNR_avg]",
		    avg->snr_avg, bb->dbg_buf);

	/*=== [CN] ===========================================================*/
	/*Threshold*/
	halbb_print_hist_2_buf_u8(bb, hist_th->cn_hist_th, BB_HIST_TH_SIZE, bb->dbg_buf,
			       HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "  %-8s %-9s  %s\n", "[TH]", "(Avg)", bb->dbg_buf);
	/*val*/
	avg->cn_avg = (u8)HALBB_DIV(acc->cn_avg_acc, pkt_cnt->pkt_cnt_2ss);
	halbb_print_hist_2_buf(bb, hist->cn_avg_hist, BB_HIST_SIZE, bb->dbg_buf,
			       HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "%-9s (%02d.%03d)  %s\n", "[CN_avg]",
		    (avg->cn_avg >> 1),
		    halbb_show_fraction_num(avg->cn_avg & 0x1, 1), bb->dbg_buf);

	/*=== [CFO] ==========================================================*/
	/*Threshold*/
	halbb_print_hist_2_buf_u8(bb, hist_th->cfo_hist_th, BB_HIST_TH_SIZE, bb->dbg_buf,
			       HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "  %-8s %-9s  %s\n", "[TH]", "(Avg)", bb->dbg_buf);
	/*val*/
	avg->cfo_avg = (s16)HALBB_DIV(acc->cfo_avg_acc, valid_cnt);

	halbb_print_sign_frac_digit(bb, avg->cfo_avg, 16, 2, buf, HALBB_SNPRINT_SIZE);
	halbb_print_hist_2_buf(bb, hist->cfo_avg_hist, BB_HIST_SIZE, bb->dbg_buf,
			       HALBB_SNPRINT_SIZE);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "%-9s (%s K) %s\n", "[CFO_avg]",
		    buf, bb->dbg_buf);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "CFO_src: %s\n",
		    (bb->bb_cfo_trk_i.cfo_src == CFO_SRC_FD) ? "FD" : "Preamble");
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "valid_cnt = %d\n", valid_cnt);
}

void halbb_basic_dbg_msg_physts_mu_cnsl(struct bb_info *bb, u32 *_used,
					char *output, u32 *_out_len)
{
	struct bb_ch_info *ch = &bb->bb_ch_i;
	struct bb_link_info *link = &bb->bb_link_i;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_mu_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_mu_i;
	struct bb_rssi_mu_acc_info *acc = &cmn_rpt->bb_rssi_mu_acc_i;
	struct bb_rssi_mu_avg_info *avg = &cmn_rpt->bb_rssi_mu_avg_i;
	u8 rssi_avg_tmp = 0;
	u8 rssi_tmp[HALBB_MAX_PATH];
	u16 pkt_cnt_ss = 0;
	u8 i = 0, j =0;
	u8 rate_num = bb->num_rf_path, ss_ofst = 0;

	if (bb->bb_cmn_rpt_i.bb_pkt_cnt_mu_i.pkt_cnt_all == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "NO MU pkt\n");
		return;
	}

	/*RX Rate*/
	halbb_print_rate_2_buff(bb, link->rx_rate_plurality_mu,
				RTW_GILTF_LGI_4XHE32, bb->dbg_buf, 32);

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "Plurality_RxRate:%s (0x%x)\n",
		    bb->dbg_buf, link->rx_rate_plurality);

	/*RX Rate Distribution & RSSI*/

	if (bb->bb_link_i.wlan_mode_bitmap & WLAN_MD_11AX) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[GI_LTF] {4x32/4x08/2x16/2x08/1x16/1x08}={%d, %d, %d, %d, %d, %d}\n",
			    pkt_cnt->gi_ltf_cnt[0], pkt_cnt->gi_ltf_cnt[1],
			    pkt_cnt->gi_ltf_cnt[2], pkt_cnt->gi_ltf_cnt[3],
			    pkt_cnt->gi_ltf_cnt[4], pkt_cnt->gi_ltf_cnt[5]);
	} else {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[GI] {LGI:%d, SGI:%d}\n",
			    pkt_cnt->gi_ltf_cnt[0], pkt_cnt->gi_ltf_cnt[1]);
	}

	avg->rssi_t_avg = (u8)HALBB_DIV(acc->rssi_t_avg_acc, pkt_cnt->pkt_cnt_all);
		
	for (i = 0; i < HALBB_MAX_PATH; i++) {
		if (i >= bb->num_rf_path)
			break;

		avg->rssi_t[i] = (u8)HALBB_DIV(acc->rssi_t_acc[i], pkt_cnt->pkt_cnt_all);
	}

	/*@======VHT==========================================================*/
	if (pkt_cnt->vht_pkt_not_zero) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = HE_VHT_NUM_MCS * i;

			for (j = 0; j < HE_VHT_NUM_MCS ; j++) {
				pkt_cnt_ss += pkt_cnt->pkt_cnt_vht[ss_ofst + j];
			}

			if (pkt_cnt_ss == 0) {
				rssi_avg_tmp = 0;
				rssi_tmp[0] = 0;
				rssi_tmp[1] = 0;
			} else {
				rssi_avg_tmp = avg->rssi_t_avg >> 1;
				rssi_tmp[0] = avg->rssi_t[0] >> 1;
				rssi_tmp[1] = avg->rssi_t[1] >> 1;
			}
			if (bb->num_rf_path >= 2)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*[MU] VHT %d-S RSSI:{%02d| %02d,%02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (i + 1),
					    rssi_avg_tmp, rssi_tmp[0], rssi_tmp[1],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 7],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 8],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 9],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 10],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 11]);
			else
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*[MU] VHT %d-S RSSI:{%02d| %02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (i + 1),
					    rssi_avg_tmp, rssi_tmp[0],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 7],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 8],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 9],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 10],
					    pkt_cnt->pkt_cnt_vht[ss_ofst + 11]);

			pkt_cnt_ss = 0;
		}

	}

	/*@======HE==========================================================*/
	if (pkt_cnt->he_pkt_not_zero) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = HE_VHT_NUM_MCS * i;

			for (j = 0; j < HE_VHT_NUM_MCS ; j++) {
				pkt_cnt_ss += pkt_cnt->pkt_cnt_he[ss_ofst + j];
			}

			if (pkt_cnt_ss == 0) {
				rssi_avg_tmp = 0;
				rssi_tmp[0] = 0;
				rssi_tmp[1] = 0;
			} else {
				rssi_avg_tmp = avg->rssi_t_avg >> 1;
				rssi_tmp[0] = avg->rssi_t[0] >> 1;
				rssi_tmp[1] = avg->rssi_t[1] >> 1;
			}
			if (bb->num_rf_path >= 2)
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*[MU] HE %d-SS RSSI:{%02d| %02d,%02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (i + 1),
					    rssi_avg_tmp, rssi_tmp[0],
					    rssi_tmp[1],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_he[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 7],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 8],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 9],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 10],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 11]);
			else
				BB_DBG_CNSL(*_out_len, *_used, output + *_used,
					    *_out_len - *_used,
					    "*[MU] HE %d-SS RSSI:{%02d| %02d} cnt:{%03d| %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					    (i + 1),
					    rssi_avg_tmp, rssi_tmp[0],
					    pkt_cnt_ss,
					    pkt_cnt->pkt_cnt_he[ss_ofst + 0],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 1],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 2],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 3],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 4],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 5],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 6],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 7],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 8],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 9],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 10],
					    pkt_cnt->pkt_cnt_he[ss_ofst + 11]);

			pkt_cnt_ss = 0;
		}

	}

	/*@======SC_BW========================================================*/

	if (pkt_cnt->sc20_occur) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = 12 * i;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "*[MU][Low BW 20M] %d-ss MCS[0:11] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				    (i + 1),
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 0],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 1],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 2],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 3],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 4],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 5],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 6],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 7],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 8],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 9],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 10],
				    pkt_cnt->pkt_cnt_sc20[ss_ofst + 11]);
		}
	}

	if (pkt_cnt->sc40_occur) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = 12 * i;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "*[MU][Low BW 40M] %d-ss MCS[0:11] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				    (i + 1),
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 0],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 1],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 2],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 3],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 4],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 5],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 6],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 7],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 8],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 9],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 10],
				    pkt_cnt->pkt_cnt_sc40[ss_ofst + 11]);
		}
	}

	if (pkt_cnt->sc80_occur) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = 12 * i;

			BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
				    "*[MU][Low BW 80M] %d-ss MCS[0:11] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				    (i + 1),
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 0],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 1],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 2],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 3],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 4],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 5],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 6],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 7],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 8],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 9],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 10],
				    pkt_cnt->pkt_cnt_sc80[ss_ofst + 11]);
		}
	}
}

void halbb_dm_summary_cnsl(struct bb_info *bb, u32 *_used,
			   char *output, u32 *_out_len)
{
#ifdef HALBB_CFO_TRK_SUPPORT
		struct bb_cfo_trk_info *cfo_trk = &bb->bb_cfo_trk_i;
#endif
	struct bb_dig_cr_info *cr = &bb->bb_dig_i.bb_dig_cr_i;
	u8 i = 0;
	u8 lna = 0, tia = 0, rxbb = 0;
	u8 ofdm_pd_th = 0, ofdm_pd_th_en = 0, cck_pd_th_en = 0;
	u8 rx_num_path = bb->phl_com->phy_cap[bb->bb_phy_idx].rx_path_num;
	s8 cck_pd_th = 0;

	for (i = 0; i < rx_num_path; i++) {
		lna = halbb_get_lna_idx(bb, i);
		tia = halbb_get_tia_idx(bb, i);
		rxbb = halbb_get_rxb_idx(bb, i);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[DIG][Path-%d] Get(lna,tia,rxb)=(%d,%d,%d)\n",
			    i, lna, tia, rxbb);
	}

	ofdm_pd_th = (u8)halbb_get_reg_cmn(bb, cr->seg0r_pd_lower_bound_a,
					   cr->seg0r_pd_lower_bound_a_m,
					   bb->bb_phy_idx);
	ofdm_pd_th_en = (u8)halbb_get_reg_cmn(bb, cr->seg0r_pd_spatial_reuse_en_a,
					      cr->seg0r_pd_spatial_reuse_en_a_m,
					      bb->bb_phy_idx);
	cck_pd_th = (s8)halbb_get_reg(bb, cr->rssi_nocca_low_th_a,
				      cr->rssi_nocca_low_th_a_m);
	cck_pd_th_en = (u8)halbb_get_reg(bb, cr->cca_rssi_lmt_en_a,
					 cr->cca_rssi_lmt_en_a_m);

	if ((bb->ic_type == BB_RTL8852A && bb->hal_com->cv < CCV) ||
	    (bb->ic_type == BB_RTL8852B && bb->hal_com->cv < CBV))
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "PD_low_bd_en(ofdm) : (%d), PD_low_bd(ofdm) = (-%d) dBm\n",
			    ofdm_pd_th_en, 102 - (ofdm_pd_th << 1));
	else
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "PD_low_bd_en(ofdm, cck) : (%d, %d), PD_low_bd(ofdm, cck) = (-%d, %d) dBm\n",
			    ofdm_pd_th_en, cck_pd_th_en, 102 - (ofdm_pd_th << 1),
			    cck_pd_th);

	/*[CFO TRK]*/
#ifdef HALBB_CFO_TRK_SUPPORT
		halbb_print_sign_frac_digit(bb, cfo_trk->cfo_avg_pre, 16, 2, bb->dbg_buf, HALBB_SNPRINT_SIZE);
	
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			"CFO[T-1]=(%s kHz), cryst_cap=(%s%d), cfo_ofst=%d\n",
			  bb->dbg_buf,
			  ((cfo_trk->crystal_cap > cfo_trk->def_x_cap) ? "+" : "-"),
			  DIFF_2(cfo_trk->crystal_cap, cfo_trk->def_x_cap),
			  cfo_trk->x_cap_ofst);
#endif
}

void halbb_reset_cnsl(struct bb_info *bb)
{
	if (bb->bb_cmn_hooker->bb_cmn_dbg_i.cmn_log_2_cnsl_en ||
	    bb->bb_cmn_hooker->bb_cmn_dbg_i.cmn_log_2_drv_statistic_en) {

	    BB_DBG(bb, DBG_CMN, "[%s] log_2_cnsl=%d, log_2_drv_statistic=%d",
		  __func__, bb->bb_cmn_hooker->bb_cmn_dbg_i.cmn_log_2_cnsl_en,
		  bb->bb_cmn_hooker->bb_cmn_dbg_i.cmn_log_2_drv_statistic_en);
	} else {
		return;
	}

	halbb_store_data(bb);
	#ifdef HALBB_STATISTICS_SUPPORT
	halbb_statistics_reset(bb);
	#endif
	halbb_cmn_info_rpt_reset(bb);
}

void halbb_statistic_exp(struct bb_info *bb_0,
			 struct halbb_statistic_exp_t *exp,
			 enum phl_phy_idx phy_idx)
{
	struct bb_info *bb = bb_0;
	struct rtw_phl_stainfo_t *sta;
	struct rtw_ra_sta_info	*ra;
	struct bb_cmn_rpt_info	*cmn_rpt = &bb->bb_cmn_rpt_i;
	struct bb_pkt_cnt_su_info *pkt_cnt = &cmn_rpt->bb_pkt_cnt_su_i;
	struct bb_rssi_su_acc_info *acc_rssi = &cmn_rpt->bb_rssi_su_acc_i;
	struct bb_rssi_su_avg_info *avg_rssi = &cmn_rpt->bb_rssi_su_avg_i;
	struct bb_physts_acc_info *acc = &cmn_rpt->bb_physts_acc_i;
	//struct bb_physts_avg_info *avg = &cmn_rpt->bb_physts_avg_i;
	u16 valid_cnt = pkt_cnt->pkt_cnt_t + pkt_cnt->pkt_cnt_ofdm;
	u8 cali_val = 3;
	struct bb_edcca_info *bb_edcca = &bb->bb_edcca_i;
	struct edcca_hw_rpt *rpt = &bb_edcca->edcca_rpt;
	u8 i = 0;

#ifdef HALBB_DBCC_SUPPORT
	HALBB_GET_PHY_PTR(bb_0, bb, phy_idx);
#endif

	BB_DBG(bb, DBG_CMN, "[%s] phy_idx=%d", __func__, phy_idx);

	if (!bb->bb_cmn_hooker->bb_cmn_dbg_i.cmn_log_2_drv_statistic_en)
		return;

	if (bb->sta_exist[0]) {
		sta = bb->phl_sta_info[0];
		ra = &sta->hal_sta->ra_info;
		exp->tx_rate = (u16)(ra->rpt_rt_i.mcs_ss_idx & 0x7f) | ((u16)(ra->rpt_rt_i.mode & 0x3) << 7);
		exp->tx_per = ra->curr_retry_ratio;
	}

	avg_rssi->rssi_cck_avg = (u8)HALBB_DIV(acc_rssi->rssi_cck_avg_acc, pkt_cnt->pkt_cnt_cck);
	avg_rssi->rssi_ofdm_avg = (u8)HALBB_DIV(acc_rssi->rssi_ofdm_avg_acc, pkt_cnt->pkt_cnt_ofdm);
	avg_rssi->rssi_t_avg = (u8)HALBB_DIV(acc_rssi->rssi_t_avg_acc, pkt_cnt->pkt_cnt_t);

	for (i = 0; i < HALBB_MAX_PATH; i++) {
		if (i >= bb->num_rf_path)
			break;

		avg_rssi->rssi_cck[i] = (u8)HALBB_DIV(acc_rssi->rssi_cck_acc[i], pkt_cnt->pkt_cnt_cck);
		avg_rssi->rssi_ofdm[i] = (u8)HALBB_DIV(acc_rssi->rssi_ofdm_acc[i], pkt_cnt->pkt_cnt_ofdm);
		avg_rssi->rssi_t[i] = (u8)HALBB_DIV(acc_rssi->rssi_t_acc[i], pkt_cnt->pkt_cnt_t);
	}

	exp->evm_1ss = (u8)(HALBB_DIV(acc->evm_1ss, (pkt_cnt->pkt_cnt_1ss + pkt_cnt->pkt_cnt_ofdm)) >> 2) + cali_val;
	exp->evm_max = (u8)(HALBB_DIV(acc->evm_max_acc, pkt_cnt->pkt_cnt_2ss) >> 2) + cali_val;
	exp->evm_min = (u8)(HALBB_DIV(acc->evm_min_acc, pkt_cnt->pkt_cnt_2ss) >> 2) + cali_val;
	exp->snr_avg = (u8)HALBB_DIV(acc->snr_avg_acc, valid_cnt);

	//halbb_mem_cpy(bb, &exp->bb_rssi_su_acc_exp, acc, sizeof(struct bb_pkt_cnt_su_info));
	halbb_mem_cpy(bb, &exp->bb_pkt_cnt_exp, pkt_cnt, sizeof(struct bb_pkt_cnt_su_info));
	halbb_mem_cpy(bb, &exp->bb_rssi_su_avg_exp, avg_rssi, sizeof(struct bb_rssi_su_avg_info));
	//halbb_mem_cpy(bb, &exp->avg_info, avg, sizeof(struct bb_physts_avg_info));

	/*Reste Counter*/
	//halbb_reset_cnsl(bb);

	halbb_edcca_get_result(bb);
	exp->edcca_fb_pwdb = rpt->pwdb_fb;
}

void halbb_statistic_reset(struct bb_info *bb)
{
	BB_DBG(bb, DBG_CMN, "[%s]", __func__);
	halbb_reset_cnsl(bb);
}

void halbb_statistic_exp_en(struct bb_info *bb, bool en)
{
	bb->bb_cmn_hooker->bb_cmn_dbg_i.cmn_log_2_drv_statistic_en = en;

	BB_DBG(bb, DBG_CMN, "[%s] en=%d", __func__, en);
}

void halbb_basic_dbg_message_cnsl_dbg(struct bb_info *bb, char input[][16], u32 *_used,
				      char *output, u32 *_out_len)
{
	struct bb_link_info	*link = &bb->bb_link_i;
	struct bb_ch_info	*ch = &bb->bb_ch_i;
	struct bb_dbg_info	*dbg = &bb->bb_dbg_i;
	struct bb_physts_info	*physts = &bb->bb_physts_i;
	struct bb_cmn_dbg_info *cmn_dbg = &bb->bb_cmn_hooker->bb_cmn_dbg_i;
	enum channel_width bw = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.bw;
	struct halbb_statistic_exp_t statistic_exp;
	u32 var[10] = {0};
	bool dbg_en;
	u8 fc = bb->hal_com->band[bb->bb_phy_idx].cur_chandef.center_ch;
	u8 sta_cnt = 0;
	u8 i;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "{0:to log, 1:to consol}\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "exp1 {en}\n");
		return;
	} else if (_os_strcmp(input[1], "exp1") == 0){
		HALBB_SCAN(input[2], DCMD_DECIMAL, &var[0]);
		dbg_en = (bool)var[0];
		halbb_statistic_exp_en(bb, dbg_en);

		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "dbg_en=%d\n", dbg_en);

		if (!dbg_en)
			return;

		halbb_statistic_exp(bb, &statistic_exp, bb->bb_phy_idx);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "tx_rate=0x%x, tx_PER=%d\n", statistic_exp.tx_rate, statistic_exp.tx_per);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "pkt_cnt_ofdm=%d\n", statistic_exp.bb_pkt_cnt_exp.pkt_cnt_ofdm);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "rssi_ofdm_avg=%d\n", statistic_exp.bb_rssi_su_avg_exp.rssi_ofdm_avg);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "evm_1ss=%d, evm_2ss={%d, %d}, SNR = %d\n", statistic_exp.evm_1ss,
			    statistic_exp.evm_max, statistic_exp.evm_min, statistic_exp.snr_avg);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used,
			    *_out_len - *_used,
			    "edcca_fb_pwdb=%d\n", statistic_exp.edcca_fb_pwdb);
		return;
	}

	HALBB_SCAN(input[1], DCMD_DECIMAL, &var[0]);
	cmn_dbg->cmn_log_2_cnsl_en = (bool)var[0];

	/*base on 2021.12.28 halbb master (028C)*/
	if (!cmn_dbg->cmn_log_2_cnsl_en)
		return;

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		"====[1. System] (%08d sec) (Ability=0x%08llx)\n",
	        bb->bb_sys_up_time, bb->support_ability);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		"[%s mode], TP{T,R,ALL}={%d, %d, %d}, BW:%d, CH_fc:%d\n",
	       ((bb->bb_watchdog_mode == BB_WATCHDOG_NORMAL) ? "Normal" :
	       ((bb->bb_watchdog_mode == BB_WATCHDOG_LOW_IO) ? "LowIO" : "NonIO")),
	       link->tx_tp, link->rx_tp, link->total_tp, 20 << bw, fc);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
	       "Phy:%d, linked: %d, Num_sta: %d, rssi_max/min= {%02d.%d, %02d.%d}, Noisy:%d\n",
	       bb->bb_phy_idx,
	       link->is_linked, bb->hal_com->assoc_sta_cnt,
	       ch->rssi_max >> 1, (ch->rssi_max & 1) * 5,
	       ch->rssi_min >> 1, (ch->rssi_min & 1) * 5,
	       ch->is_noisy);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
	       "Mode=%s%s%s%s%s\n",
	       (link->wlan_mode_bitmap & WLAN_MD_11B) ? "B" : " ",
	       (link->wlan_mode_bitmap & (WLAN_MD_11G | WLAN_MD_11A)) ? "G" : " ",
	       (link->wlan_mode_bitmap & WLAN_MD_11N) ? "N" : " ",
	       (link->wlan_mode_bitmap & WLAN_MD_11AC) ? "AC" : " ",
	       (link->wlan_mode_bitmap & WLAN_MD_11AX) ? "AX" : " ");

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "physts_cnt{all, 2_self, err_len, ok_ie, err_ie}={%d,%d,%d,%d,%d}, invalid_he=%d\n",
	       physts->bb_physts_cnt_i.all_cnt, physts->bb_physts_cnt_i.is_2_self_cnt,
	       physts->bb_physts_cnt_i.ok_ie_cnt, physts->bb_physts_cnt_i.err_ie_cnt,
	       physts->bb_physts_cnt_i.err_len_cnt,
	       physts->bb_physts_cnt_i.invalid_he_cnt);

	for (i = 0; i< PHL_MAX_STA_NUM; i++) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "[%d] Linked macid=%d\n", i, bb->sta_exist[i]);
		sta_cnt++;
		if (sta_cnt >= bb->hal_com->assoc_sta_cnt)
			break;
	}
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used, "\n");
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used, "====[2. ENV Mntr]\n");
	halbb_env_mntr_log_cnsl(bb, _used, output, _out_len);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used, "\n");
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "====[3. PMAC]\n");
	halbb_basic_dbg_msg_pmac_cnsl(bb, _used, output, _out_len);
	halbb_crc32_cnt2_cmn_log_cnsl(bb, _used, output, _out_len);
	halbb_crc32_cnt3_cmn_log_cnsl(bb, _used, output, _out_len);
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "\n");
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "====[4. TX General]\n");
	halbb_basic_dbg_msg_tx_dbg_reg_cnsl(bb, _used, output, _out_len);

	if (bb->bb_link_i.is_linked) {
		halbb_basic_dbg_msg_tx_info_cnsl(bb, _used, output, _out_len);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "====[5. RX General]\n");
		halbb_basic_dbg_msg_rx_info_cnsl(bb, _used, output, _out_len);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "====[6. AVG RSSI/RxRate]\n");
		halbb_basic_dbg_msg_physts_su_cnsl(bb, _used, output, _out_len);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "====[7. BB Hist]\n");
		halbb_show_phy_hitogram_su_cnsl(bb, _used, output, _out_len);
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "\n");
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "====[8. [MU] AVG RSSI/RxRate]\n");
		halbb_basic_dbg_msg_physts_mu_cnsl(bb, _used, output, _out_len);
	}
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "\n");
	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "====[9. DM Summary]\n");
	halbb_dm_summary_cnsl(bb, _used, output, _out_len);

	/*Reste Counter*/
	halbb_reset_cnsl(bb);
}

void halbb_show_rx_rate(struct bb_info *bb, char input[][16], u32 *_used,
			      char *output, u32 *_out_len)
{
	struct bb_cmn_dbg_info *cmn_dbg = &bb->bb_cmn_hooker->bb_cmn_dbg_i;
	u32 val[10] = {0};
	bool en = false;

	if (_os_strcmp(input[1], "-h") == 0) {
		BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
			    "{en}\n");
		return;
	}

	HALBB_SCAN(input[1], DCMD_DECIMAL, &val[0]);
	cmn_dbg->cmn_log_2_cnsl_en = (bool)val[0];

	if (!cmn_dbg->cmn_log_2_cnsl_en)
		return;

	BB_DBG_CNSL(*_out_len, *_used, output + *_used, *_out_len - *_used,
		    "[SU RX Rate]\n");
	halbb_basic_dbg_msg_physts_su_cnsl(bb, _used, output, _out_len);


	/*Reste Counter*/
	halbb_reset_cnsl(bb);
}

#endif

