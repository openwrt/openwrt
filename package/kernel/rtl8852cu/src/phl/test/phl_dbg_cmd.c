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
 * Author: vincent_fann@realtek.com
 *
 *****************************************************************************/
#define _PHL_DBG_CMD_C_
#include "phl_dbg_cmd.h"
#include "phl_ps_dbg_cmd.h"

/*
 * proc debug command of core
 */
enum PHL_DBG__CORE_CMD_ID {
	PHL_DBG_CORE_HELP,
	PHL_DBG_CORE_GIT_INFO
};

static const struct phl_dbg_cmd_info phl_dbg_core_cmd_i[] = {
	{"git_info", PHL_DBG_CORE_GIT_INFO}
};

void phl_dbg_git_info(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
/* #REMOVE BEGIN */
#if CONFIG_GEN_GIT_INFO
#include "../phl_git_info.h"

	u32 used = 0;

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"\ncore_ver   : %s\n", RTK_CORE_TAGINFO);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"phl_ver    : %s\n", RTK_PHL_TAGINFO);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"halmac_ver : %s\n", RTK_HALMAC_TAGINFO);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"halbb_ver  : %s\n", RTK_HALBB_TAGINFO);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"halrf_ver  : %s\n", RTK_HALRF_TAGINFO);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"btc_ver    : %s\n", RTK_BTC_TAGINFO);

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"\ncore_sha1  : %s\n", RTK_CORE_SHA1);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"phl_sha1   : %s\n", RTK_PHL_SHA1);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"halmac_sha1: %s\n", RTK_HALMAC_SHA1);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"halbb_sha1 : %s\n", RTK_HALBB_SHA1);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"halrf_sha1 : %s\n", RTK_HALRF_SHA1);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"btc_sha1   : %s\n", RTK_BTC_SHA1);
#endif /* CONFIG_GEN_GIT_INFO */
/* #REMOVE END */
}

void phl_dbg_core_cmd_parser(void *phl, char input[][MAX_ARGV],
		        u32 input_num, char *output, u32 out_len)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	u8 id = 0;
	u32 i;
	u32 used = 0;
	u32 phl_ary_size = sizeof(phl_dbg_core_cmd_i) /
			   sizeof(struct phl_dbg_cmd_info);

	if (phl_ary_size == 0)
		return;

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used, "\n");
	/* Parsing Cmd ID */
	if (input_num) {
		for (i = 0; i < phl_ary_size; i++) {
			if (_os_strcmp(phl_dbg_core_cmd_i[i].name, input[0]) == 0) {
				id = phl_dbg_core_cmd_i[i].id;
				PHL_INFO("[%s]===>\n", phl_dbg_core_cmd_i[i].name);
				break;
			}
		}
		if (i == phl_ary_size) {
			PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "PHL CMD not found!\n");
			return;
		}
	}

	switch (id) {
	case PHL_DBG_CORE_HELP:
	{
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				 "phl_dbg_core_cmd_parser : PHL_DBG_CORE_HELP \n");
		for (i = 0; i < phl_ary_size; i++)
			PHL_DBG_MON_INFO(out_len, used, output + used,
					 out_len - used, "%-5d: %s\n",
			          (int)i, phl_dbg_core_cmd_i[i].name);

	}
	break;
	case PHL_DBG_CORE_GIT_INFO:
	{
		phl_dbg_git_info(phl_info, input, input_num, output, out_len);
	}
	break;
	default:
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "[DBG] Do not support this command\n");
		break;
	}
}

s32
phl_dbg_core_proc_cmd(struct phl_info_t *phl_info,
		 char *input, char *output, u32 out_len)
{
	char *token;
	u32 argc = 0;
	char argv[MAX_ARGC][MAX_ARGV];

	do {
		token = _os_strsep(&input, ", ");
		if (token) {
			if (_os_strlen((u8 *)token) <= MAX_ARGV)
				_os_strcpy(argv[argc], token);

			argc++;
		} else {
			break;
		}
	} while (argc < MAX_ARGC);

	phl_dbg_core_cmd_parser(phl_info, argv, argc, output, out_len);

	return 0;
}

enum rtw_phl_status
rtw_phl_dbg_core_cmd(struct phl_info_t *phl_info,
		     struct rtw_proc_cmd *incmd,
		     char *output,
		     u32 out_len)
{
	if (incmd->in_type == RTW_ARG_TYPE_BUF) {
		phl_dbg_core_proc_cmd(phl_info, incmd->in.buf, output, out_len);
	} else if(incmd->in_type == RTW_ARG_TYPE_ARRAY){
		phl_dbg_core_cmd_parser(phl_info, incmd->in.vector,
				   incmd->in_cnt_len, output, out_len);
	}
	return RTW_PHL_STATUS_SUCCESS;
}

#ifdef CONFIG_PHL_TEST_SUITE
bool
_is_hex_digit(char ch_tmp)
{
	if( (ch_tmp >= '0' && ch_tmp <= '9') ||
		(ch_tmp >= 'a' && ch_tmp <= 'f') ||
		(ch_tmp >= 'A' && ch_tmp <= 'F') ) {
		return true;
	} else {
		return false;
	}
}


u32
_map_char_to_hex_digit(char ch_tmp)
{
	if(ch_tmp >= '0' && ch_tmp <= '9')
		return (ch_tmp - '0');
	else if(ch_tmp >= 'a' && ch_tmp <= 'f')
		return (10 + (ch_tmp - 'a'));
	else if(ch_tmp >= 'A' && ch_tmp <= 'F')
		return (10 + (ch_tmp - 'A'));
	else
		return 0;
}


bool
_get_hex_from_string(char *szstr, u32 *val)
{
	char *sz_scan = szstr;

	/* Check input parameter.*/
	if (szstr == NULL || val == NULL) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_,
			"_get_hex_from_string(): Invalid inpur argumetns! szStr: %p, pu4bVal: %p \n",
			 szstr, val);
		return false;
	}

	/* Initialize output. */
	*val = 0;

	/* Skip leading space. */
	while(*sz_scan != '\0' && (*sz_scan == ' ' || *sz_scan == '\t')) {
		sz_scan++;
	}

	/* Skip leading '0x' or '0X'. */
	if (*sz_scan == '0' && (*(sz_scan+1) == 'x' || *(sz_scan+1) == 'X')) {
		sz_scan += 2;
	}

	if (!_is_hex_digit(*sz_scan)) {
		return false;
	}

	do {
		(*val) <<= 4;
		*val += _map_char_to_hex_digit(*sz_scan);

		sz_scan++;
	} while (_is_hex_digit(*sz_scan));

	return true;
}

void
_phl_dbg_cmd_switch_chbw(struct phl_info_t *phl_info, char input[][MAX_ARGV],
			 u32 input_num, char *output, u32 out_len)
{
	u32 band_idx = 0;
	u32 bw = 0;
	u32 offset = 0;
	u32 ch = 36;
	u32 ch_band = (u32)BAND_MAX;
	u32 used = 0;
	struct rtw_chan_def chdef = {0};

	do {
		if (input_num < 6){
			PHL_DBG_MON_INFO(out_len, used, output + used,
					 out_len - used,
					 "\n[DBG] echo phl set_ch [band_idx(0,1)] [ch_band(0,1,2)] [ch(hex)] [bw(0,1,2,3)] [offset(0,1,2,3)]\n");
			break;
		}

		if (!_get_hex_from_string(input[1], &band_idx))
			break;

		if (band_idx > 1)
			break;

		if (!_get_hex_from_string(input[2], &ch_band))
			break;

		if (!_get_hex_from_string(input[3], &ch))
			break;


		if (!_get_hex_from_string(input[4], &bw))
			break;


		if (!_get_hex_from_string(input[5], &offset))
			break;

		PHL_DBG_MON_INFO(out_len, used, output + used,
				 out_len - used,
				 "\n[DBG] PHL_DBG_SET_CH_BW ==> band_idx = %d\n",
				 (int)band_idx);
		PHL_DBG_MON_INFO(out_len, used, output + used,
				 out_len - used,
				 "\n[DBG] PHL_DBG_SET_CH_BW ==> ch_band = %d\n",
				 (int)ch_band);
		PHL_DBG_MON_INFO(out_len, used, output + used,
				 out_len - used,
				 "[DBG] PHL_DBG_SET_CH_BW ==> ch = %d\n",
				 (int)ch);
		PHL_DBG_MON_INFO(out_len, used, output + used,
				 out_len - used,
				 "[DBG] PHL_DBG_SET_CH_BW ==> bw = %d\n",
				 (int)bw);
		PHL_DBG_MON_INFO(out_len, used, output + used,
				 out_len - used,
				 "[DBG] PHL_DBG_SET_CH_BW ==> offset = %d\n",
				 (int)offset);

		chdef.chan = (u8)ch;
		chdef.band = (enum band_type)ch_band;
		chdef.bw = (enum channel_width)bw;
		chdef.offset = (enum chan_offset)offset;

		rtw_hal_set_ch_bw(phl_info->hal, (u8)band_idx, &chdef, false, false, false);

	} while (0);
}

void _dump_wifi_role(struct phl_info_t *phl_info,
		char input[][MAX_ARGV], u32 input_num,
		char *output, u32 out_len)
{
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_phl_stainfo_t *sta_info = NULL;
#if defined(CONFIG_PHL_RELEASE_RPT_ENABLE) || defined(CONFIG_PCI_HCI)
	struct rtw_wp_rpt_stats *rpt_stats = NULL;
#endif
	u32 used = 0;
	u8 j = 0;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 lidx;
	u32 show_cap = 0, role_idx = 0xff, rlink_idx = 0xff;

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				"==> PHL_DBG_DUMP_WROLE CH/BW information\n");

	if (input_num > 1) {
		if (!_os_strcmp(input[1], "-h")) {
			PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "phl role [option] [wrole idx] [rlink idx]\n"
						"option:\n"
						"-h\tshow usage\n"
						"-c\tshow capability\n");
			return;
		}

		if (!_os_strcmp(input[1], "-c"))
			show_cap = 1;
	}

	if (input_num > 2)
		_get_hex_from_string(input[2], &role_idx);

	if (input_num > 3)
		_get_hex_from_string(input[3], &rlink_idx);

	for( j = 0; j < MAX_WIFI_ROLE_NUMBER; j++) {
		if (role_idx != 0xff && role_idx != j)
			continue;

		wrole = phl_get_wrole_by_ridx(phl_info, j);
		if (NULL == wrole)
			continue;

		PHL_DBG_MON_INFO(out_len, used, output + used,
				 out_len - used, "wrole idx = %d \n", j);
		PHL_DBG_MON_INFO(out_len, used, output + used,
				 out_len - used, "wrole->type = %u \n",
				 wrole->type);
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used,
				"wrole->mac_addr : %02x-%02x-%02x-%02x-%02x-%02x- \n",
				wrole->mac_addr[0], wrole->mac_addr[1],
				wrole->mac_addr[2], wrole->mac_addr[3],
				wrole->mac_addr[4], wrole->mac_addr[5]);

		for (lidx = 0; lidx < RTW_RLINK_MAX; lidx++) {
			if (rlink_idx != 0xff && rlink_idx != lidx)
				continue;

			rlink = get_rlink(wrole, lidx);

			PHL_DBG_MON_INFO(out_len, used, output + used,
				 out_len - used, "rlink->lidx = %d \n", lidx);
			PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "rlink->chandef.bw = %u \n",
					rlink->chandef.bw);
			PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "rlink->chandef.band = %u \n",
					rlink->chandef.band);
			PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "rlink->chandef.center_ch = %u \n",
					rlink->chandef.center_ch);
			PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "rlink->chandef.chan = %u \n",
					rlink->chandef.chan);
			PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "rlink->chandef.center_freq1 = %u \n",
					(int)rlink->chandef.center_freq1);
			PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "rlink->chandef.center_freq2 = %u \n",
					(int)rlink->chandef.center_freq2);
			PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "rlink->chandef.offset = %u \n",
					rlink->chandef.offset);

			if (show_cap)  {
				PHL_DBG_MON_DUMP(out_len, used, output + used, out_len - used,
						"rlink->cap", &rlink->cap, sizeof(rlink->cap));
				PHL_DBG_MON_DUMP(out_len, used, output + used, out_len - used,
						"rlink->protocol_cap", &rlink->protocol_cap,
						sizeof(rlink->protocol_cap));
			}

			sta_info = rtw_phl_get_stainfo_self(phl_info, rlink);
			if (NULL == sta_info) {
				PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "-----------------------------\n");
				continue;
			}
			PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "sta_info->macid = %u \n",
				sta_info->macid);
			PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "sta_info->aid = %u \n",
				sta_info->aid);
			PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used,
				"sta_info->mac_addr : %02x-%02x-%02x-%02x-%02x-%02x- \n",
				sta_info->mac_addr[0], sta_info->mac_addr[1],
				sta_info->mac_addr[2], sta_info->mac_addr[3],
				sta_info->mac_addr[4], sta_info->mac_addr[5]);
			PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "sta_info->wmode = 0x%x \n",
				sta_info->wmode);
#if defined(CONFIG_PHL_RELEASE_RPT_ENABLE) || defined(CONFIG_PCI_HCI)
			rpt_stats =
				(struct rtw_wp_rpt_stats *)sta_info->hal_sta->trx_stat.wp_rpt_stats;
			PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "sta_info->busy_cnt = %u \n",
				(unsigned int)rpt_stats->busy_cnt);
#endif
			PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "-----------------------------\n");
		}
	}
}

void _dump_mr_info(struct phl_info_t *phl_info, char *output, u32 out_len)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	u8 role_num = 0, i;
	u32 used = 0;
	int chanctx_num = 0;
	struct hw_band_ctl_t *band_ctrl = NULL;

	for (i = 0; i < MAX_BAND_NUM; i++) {
		band_ctrl = &mr_ctl->band_ctrl[i];
		chanctx_num = phl_mr_get_chanctx_num(phl_info, band_ctrl);
		role_num = phl_mr_get_role_num(phl_info, band_ctrl);

		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				 "[BAND-%d] port map:0x%02x, role num:%d map:0x%02x\n",
				band_ctrl->id, band_ctrl->port_map, role_num, band_ctrl->role_map);

		/*dump mr_info*/
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				"[BAND-%d] sta_num:%d, ld_sta_num:%d, lg_sta_num:%d\n",
				band_ctrl->id, band_ctrl->cur_info.sta_num,
				band_ctrl->cur_info.ld_sta_num, band_ctrl->cur_info.lg_sta_num);
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				"[BAND-%d] ap_num:%d, ld_ap_num:%d\n",
				band_ctrl->id, band_ctrl->cur_info.ap_num, band_ctrl->cur_info.ld_ap_num);

		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				"[BAND-%d] chan_ctx num:%d, cc_band_map:0x%04x\n",
				band_ctrl->id, chanctx_num, band_ctrl->chctx_band_map);

		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				"\t chctx_band -");
		if (band_ctrl->chctx_band_map == 0)
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_NON\n");
		if (band_ctrl->chctx_band_map & BIT(CC_BAND_24G))
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_24G\n");
		if (band_ctrl->chctx_band_map & BIT(CC_BAND_5G_B1))
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_5G_B1\n");
		if (band_ctrl->chctx_band_map & BIT(CC_BAND_5G_B2))
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_5G_B2\n");
		if (band_ctrl->chctx_band_map & BIT(CC_BAND_5G_B3))
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_5G_B3\n");
		if (band_ctrl->chctx_band_map & BIT(CC_BAND_5G_B4))
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_5G_B4\n");
		if (band_ctrl->chctx_band_map & BIT(CC_BAND_6G_U5))
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_6G_U5\n");
		if (band_ctrl->chctx_band_map & BIT(CC_BAND_6G_U6))
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_6G_U6\n");
		if (band_ctrl->chctx_band_map & BIT(CC_BAND_6G_U7))
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_6G_U7\n");
		if (band_ctrl->chctx_band_map & BIT(CC_BAND_6G_U8))
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				" %s ", "B_6G_U8\n");

		if (chanctx_num) {
			struct rtw_chan_ctx *chanctx = NULL;
			_os_list *chctx_list = &band_ctrl->chan_ctx_queue.queue;
			void *drv = phl_to_drvpriv(phl_info);

			_os_spinlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
			phl_list_for_loop(chanctx, struct rtw_chan_ctx, chctx_list, list) {

				role_num = phl_chanctx_get_rnum(phl_info, &band_ctrl->chan_ctx_queue, chanctx);

				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
					"\t[CH-CTX] role num:%d map:0x%02x, DFS enable:%s\n",
					role_num, chanctx->role_map,
					(chanctx->dfs_enabled) ? "Y" : "N");

				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
						"\t[CH] band:%u\n", chanctx->chan_def.band);
				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
						"\t[CH] chan:%u\n", chanctx->chan_def.chan);
				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
						"\t[CH] center_ch:%u\n", chanctx->chan_def.center_ch);
				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
						"\t[CH] bw:%u\n", chanctx->chan_def.bw);
				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
						"\t[CH] offset:%u\n", chanctx->chan_def.offset);

				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
						"\t[CH] center_freq1:%u\n", (int)chanctx->chan_def.center_freq1);
				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
						"\t[CH] center_freq2:%u\n", (int)chanctx->chan_def.center_freq2);
				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
						"\t[CH] hw_value:%u\n",chanctx ->chan_def.hw_value);
			}
			_os_spinunlock(drv, &band_ctrl->chan_ctx_queue.lock, _bh, NULL);
		}
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
						"-----------------------------\n");
	}
}

void _dump_rx_rate(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 ctrl = 0;
	u32 used = 0;
	struct rtw_stats *rx_stat = &phl_info->phl_com->phl_stats;
	if(input_num < 2)
		return;
	_get_hex_from_string(input[1], &ctrl);

	if (ctrl == 2) {
		/*TODO: Clear Counter*/
		return;
	}

	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "\nOFDM 6M = %d ; OFDM 24M = %d ; OFDM 54M = %d\n",
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_OFDM6],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_OFDM24],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_OFDM54]);

	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "VHT 1SS\
			\nMCS0 = %d ; MCS1 = %d ; MCS2 = %d ;\
			\nMCS3 = %d ; MCS4 = %d ; MCS5 = %d ;\
			\nMCS6 = %d ; MCS7 = %d ; MCS8 = %d ;\
			\nMCS9 = %d ;\n",
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS0],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS1],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS2],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS3],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS4],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS5],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS6],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS7],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS8],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS1_MCS9]
			);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "VHT 2SS\
			\nMCS0 = %d ; MCS1 = %d ; MCS2 = %d ;\
			\nMCS3 = %d ; MCS4 = %d ; MCS5 = %d ;\
			\nMCS6 = %d ; MCS7 = %d ; MCS8 = %d ;\
			\nMCS9 = %d ;\n",
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS0],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS1],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS2],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS3],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS4],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS5],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS6],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS7],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS8],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_VHT_NSS2_MCS9]
			);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "HE 1SS\
			\nMCS0 = %d ; MCS1 = %d ; MCS2 = %d ;\
			\nMCS3 = %d ; MCS4 = %d ; MCS5 = %d ;\
			\nMCS6 = %d ; MCS7 = %d ; MCS8 = %d ;\
			\nMCS9 = %d ; MCS10 = %d ; MCS11 = %d;\n",
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS0],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS1],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS2],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS3],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS4],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS5],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS6],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS7],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS8],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS9],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS10],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS1_MCS11]);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "HE 2SS\
			\nMCS0 = %d ; MCS1 = %d ; MCS2 = %d ;\
			\nMCS3 = %d ; MCS4 = %d ; MCS5 = %d ;\
			\nMCS6 = %d ; MCS7 = %d ; MCS8 = %d ;\
			\nMCS9 = %d ; MCS10 = %d ; MCS11 = %d ;\n",
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS0],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS1],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS2],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS3],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS4],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS5],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS6],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS7],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS8],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS9],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS10],
			(int)rx_stat->rx_rate_nmr[RTW_DATA_RATE_HE_NSS2_MCS11]
			);

}

#ifdef DEBUG_PHL_RX
void _dump_phl_rx(struct phl_info_t *phl_info, char *output, u32 out_len)
{
	u32 used = 0;
	struct phl_rx_stats *rx_stats = &phl_info->rx_stats;

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"rx_isr=%u\n", rx_stats->rx_isr);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"phl_rx=%u\n", rx_stats->phl_rx);

#ifdef CONFIG_PCI_HCI
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"rdu=%u\n", rtw_phl_get_hw_cnt_rdu(phl_info));

	do {
		struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
		struct rtw_rx_buf_ring *rx_buf_ring;
		u8 ch;

		for (ch = 0; ch < hci_info->total_rxch_num; ch++) {
			u16 rxcnt, host_idx, hw_idx;
			rxcnt = rtw_hal_rx_res_query(phl_info->hal, ch, &host_idx, &hw_idx);
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				"rxbd[%d]: host_idx=%3u,hw_idx=%3u,num=%3u,avail=%u\n",
				ch, host_idx, hw_idx, rtw_hal_get_rxbd_num(phl_info->hal, ch), rxcnt);
		}

		rx_buf_ring = (struct rtw_rx_buf_ring *)phl_info->hci->rxbuf_pool;

		for (ch = 0; ch < hci_info->total_rxch_num; ch++) {
#ifdef CONFIG_DYNAMIC_RX_BUF
			if (ch == 0)
				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
					"rxbuf_pool[%d]: idle_rxbuf_cnt=%3u,busy_rxbuf_cnt=%3u,empty_rxbuf_cnt=%u\n",
					ch, rx_buf_ring[ch].idle_rxbuf_cnt, rx_buf_ring[ch].busy_rxbuf_cnt,
					rx_buf_ring[ch].empty_rxbuf_cnt);
			else
#endif
				PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
					"rxbuf_pool[%d]: idle_rxbuf_cnt=%3u,busy_rxbuf_cnt=%3u\n",
					ch, rx_buf_ring[ch].idle_rxbuf_cnt, rx_buf_ring[ch].busy_rxbuf_cnt);
		}
	} while (0);
#endif /* CONFIG_PCI_HCI */

	do {
		struct rtw_phl_rx_ring *ring = &phl_info->phl_rx_ring;
		u16 wptr, rptr, ring_res;

		wptr = (u16)_os_atomic_read(phl_to_drvpriv(phl_info), &ring->phl_idx);
		rptr = (u16)_os_atomic_read(phl_to_drvpriv(phl_info), &ring->core_idx);
		ring_res = phl_calc_avail_rptr(rptr, wptr, MAX_PHL_RX_RING_ENTRY_NUM);

		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"phl_rx_ring: phl_idx=%d,core_idx=%d,num=%d,rx_pkt_num=%d\n",
			wptr, rptr, MAX_PHL_RX_RING_ENTRY_NUM, ring_res);
	} while (0);

	do {
		struct phl_rx_pkt_pool *rx_pkt_pool;

		rx_pkt_pool = (struct phl_rx_pkt_pool *)phl_info->rx_pkt_pool;
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"phl_rx_pkt_pool: total=%d,idle_cnt=%d\n",
			MAX_PHL_RING_RX_PKT_NUM, rx_pkt_pool->idle_cnt);
	} while (0);

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"rx_type: all=%u,wifi=%u,wp=%u,ppdu=%u,c2h=%u\n",
		rx_stats->rx_type_all,
		rx_stats->rx_type_wifi, rx_stats->rx_type_wp,
		rx_stats->rx_type_ppdu, rx_stats->rx_type_c2h);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"amsdu=%u\n", rx_stats->rx_amsdu);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"drop@get_single_rx=%u (rx_rdy=%u,rxbd=%u)\n",
		rx_stats->rx_drop_get, rx_stats->rx_rdy_fail, rx_stats->rxbd_fail);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"drop@reorder=%u (seq_less=%u,dup=%u)\n",
		rx_stats->rx_drop_reorder,
		rx_stats->reorder_seq_less, rx_stats->reorder_dup);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"reorder: dont=%u,put=%u (total=%u)\n",
		rx_stats->rx_dont_reorder, rx_stats->rx_put_reorder,
		rx_stats->rx_dont_reorder + rx_stats->rx_put_reorder);
#ifdef PHL_RXSC_AMPDU
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"ampdu: orig=%u,rxsc=%u (add=%u)\n",
		rx_stats->rxsc_ampdu[0], rx_stats->rxsc_ampdu[1],
		rx_stats->rxsc_ampdu[2]);
#endif
#ifdef CONFIG_DYNAMIC_RX_BUF
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"rxbuf_empty=%u\n", rx_stats->rxbuf_empty);
#endif

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"cnt_rx_pktsz =%u\n", phl_info->cnt_rx_pktsz);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"rx_pktsz     =%u\n", rx_stats->rx_pktsz_phl);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"rx_pktsz_core=%u\n", rx_stats->rx_pktsz_core);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"rx_pkt       =%u\n", rx_stats->rx_type_wifi);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"rx_pkt_core  =%u\n", rx_stats->rx_pkt_core);

}

void phl_dbg_cmd_phl_rx(struct phl_info_t *phl_info, char input[][MAX_ARGV],
				u32 input_num, char *output, u32 out_len)
{
	u32 used = 0;

	if (1 == input_num) {
		_dump_phl_rx(phl_info, output, out_len);
	} else if (!_os_strcmp(input[1], "clear")) {
		_os_mem_set(phl_to_drvpriv(phl_info), &phl_info->rx_stats,
			0, sizeof(phl_info->rx_stats));
	} else if (!_os_strcmp(input[1], "cnt_rx_pktsz")) {
		int value;
		if (input_num > 2 && _os_sscanf(input[2], "%d", &value) == 1)
			phl_info->cnt_rx_pktsz = value;
	} else if (!_os_strcmp(input[1], "sched_phl_rx")) {
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"Schedule PHL RX!\n");
		rtw_phl_start_rx_process(phl_info);
	} else if (!_os_strcmp(input[1], "sched_core_rx")) {
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"Schedule Core RX!\n");
		_phl_indic_new_rxpkt(phl_info);
	}
}
#endif /* DEBUG_PHL_RX */

static const char *_get_mac_pwr_st_str(enum rtw_mac_pwr_st st)
{
	switch (st) {
	case RTW_MAC_PWR_OFF:
		return "off";
	case RTW_MAC_PWR_ON:
		return "on";
	case RTW_MAC_PWR_LPS:
		return "lps";
	default:
		return "n/a";
	}
}

static const char *_get_wow_opmode_str(enum rtw_wow_op_mode mode)
{
	switch (mode) {
	case RTW_WOW_OP_PWR_DOWN:
		return "power down";
	case RTW_WOW_OP_DISCONNECT_STBY:
		return "disconnected standby";
	case RTW_WOW_OP_CONNECT_STBY:
		return "connected standby";
	default:
		return "n/a";
	}
}

void _dump_wow_stats(struct phl_info_t *phl_info, char input[][MAX_ARGV],
	u32 input_num, char *output, u32 out_len)
{
#ifdef CONFIG_WOWLAN
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	struct phl_wow_stat *wow_stat = &wow_info->wow_stat;
	u32 used = 0;

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
					 "\n%-20s : op_mode = %s, func_en = %s, mac pwr = %s, wake_rsn = %s\
					 \n%-20s : keep_alive_en = %d, disc_det_en = %d, arp_en = %d, ndp_en = %d, gtk_en = %d, dot11w_en = %d\
					 \n%-20s : init err = 0x%x, deinit err = 0x%x\
					 \n%-20s : aoac_rpt_fail_cnt = 0x%x\n",
	"[wow information]",
	_get_wow_opmode_str(wow_stat->op_mode), (wow_stat->func_en == 1 ? "yes" : "no"),
	_get_mac_pwr_st_str(wow_stat->mac_pwr), rtw_phl_get_wow_rsn_str(phl_info, wow_stat->wake_rsn),
	"[wow function]",
	wow_stat->keep_alive_en, wow_stat->disc_det_en, wow_stat->arp_en, wow_stat->ndp_en, wow_stat->gtk_en, wow_stat->dot11w_en,
	"[wow error]",
	wow_stat->err.init, wow_stat->err.deinit,
	"[wow aoac]",
	wow_stat->aoac_rpt_fail_cnt);
#endif /* CONFIG_WOWLAN */
}

#ifdef CONFIG_MCC_SUPPORT
void _dump_mcc_info(struct phl_info_t *phl_info, char input[][MAX_ARGV],
	u32 input_num, char *output, u32 out_len)
{
	bool get_ok = false;
	struct rtw_phl_mcc_en_info en_info = {0};
	enum rtw_phl_tdmra_wmode mcc_mode;
	enum rtw_phl_mcc_state state;
	enum rtw_phl_mcc_coex_mode coex_mode;
	struct rtw_phl_mcc_bt_info bt_info = {0};
	struct rtw_phl_mcc_role *mrole = NULL;
	struct rtw_chan_def *chdef = NULL;
	struct rtw_phl_mcc_dur_info *dur_i = NULL;
	struct rtw_phl_mcc_pattern *m_pattern = NULL;
	struct rtw_phl_mcc_dbg_slot_info *dbg_slot_i = NULL;
	u32 used = 0;
	u8 i = 0;

	get_ok = rtw_phl_mcc_get_dbg_info(phl_info, 0, MCC_DBG_STATE, &state);
	if (false == get_ok) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_dump_mcc_info(): get MCC_DBG_STATE fail\n");
		goto exit;
	}
	get_ok = rtw_phl_mcc_get_dbg_info(phl_info, 0, MCC_DBG_OP_MODE, &mcc_mode);
	if (false == get_ok) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_dump_mcc_info(): get MCC_DBG_OP_MODE fail\n");
		goto exit;
	}
	get_ok = rtw_phl_mcc_get_dbg_info(phl_info, 0, MCC_DBG_COEX_MODE, &coex_mode);
	if (false == get_ok) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_dump_mcc_info(): get MCC_DBG_COEX_MODE fail\n");
		goto exit;
	}
	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "\n======== MCC Info ========\n");
	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "MCC Info: State(%s, %d), Mode(%s, %d), Coex(%s, %d)\n",
			((state == MCC_NONE) ? "None" :
			((state == MCC_TRIGGER_FW_EN) ? "Trig_Fw_En" :
			((state == MCC_FW_EN_FAIL) ? "Fw_En_Fail" :
			((state == MCC_RUNING) ? "Runing" :
			((state == MCC_TRIGGER_FW_DIS) ? "Trig_Fw_Dis" :
			((state == MCC_FW_DIS_FAIL) ? "Fw_Dis_Fail" :
			((state == MCC_STOP) ? "Fw_Stop" : "Unknown"))))))),
			state,
			((mcc_mode == RTW_PHL_TDMRA_WMODE_NONE) ? "None" :
			((mcc_mode == RTW_PHL_TDMRA_AP_CLIENT_WMODE) ? "AP_Client" :
			((mcc_mode == RTW_PHL_TDMRA_2CLIENTS_WMODE) ? "2Clients" :
			((mcc_mode == RTW_PHL_TDMRA_AP_WMODE) ? "AP" : "Unknown")))),
			mcc_mode,
			((coex_mode == RTW_PHL_MCC_COEX_MODE_NONE) ? "None" :
			((coex_mode == RTW_PHL_MCC_COEX_MODE_BT_MASTER) ? "BT_M" :
			((coex_mode == RTW_PHL_MCC_COEX_MODE_WIFI_MASTER) ? "Wifi_M" :
			((coex_mode == RTW_PHL_MCC_COEX_MODE_BT_WIFI_BALANCE) ? "Balance" : "Unknown")))),
			coex_mode);
	get_ok = rtw_phl_mcc_get_dbg_info(phl_info, 0, MCC_DBG_BT_INFO, &bt_info);
	if (false == get_ok) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_dump_mcc_info(): get MCC_DBG_BT_INFO fail\n");
		goto exit;
	}
	if (bt_info.bt_seg_num <= BT_SEG_NUM) {
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "MCC Info: Bt_dur(%d), Add_Bt_Role(%d), Seg_Num(%d), bt_seg[0](%d), bt_seg[1](%d)\n",
				bt_info.bt_dur, bt_info.add_bt_role,
				bt_info.bt_seg_num, bt_info.bt_seg[0],
				bt_info.bt_seg[1]);
	}
	get_ok = rtw_phl_mcc_get_dbg_info(phl_info, 0, MCC_DBG_EN_INFO, &en_info);
	if (false == get_ok) {
		PHL_TRACE(COMP_PHL_MCC, _PHL_ERR_, "_dump_mcc_info(): get MCC_DBG_EN_INFO fail\n");
		goto exit;
	}
	m_pattern = &en_info.m_pattern;
	PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "MCC Info: mcc_intvl(%d), Start_tsf(0x%04x%04x %04x%04x), bcns_offset(%d), tob_r(%d), toa_r(%d), tob_a(%d), toa_a(%d)\n",
				en_info.mcc_intvl, (u16)(en_info.tsf_high >> 16),
				(u16)(en_info.tsf_high),
				(u16)(en_info.tsf_low >> 16),
				(u16)(en_info.tsf_low), m_pattern->bcns_offset,
				m_pattern->tob_r, m_pattern->toa_r,
				m_pattern->tob_a, m_pattern->toa_a);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "\n======== MCC Role Info ========\n");
	for (i = 0; i < en_info.mrole_num; i++) {
		mrole = &en_info.mcc_role[i];
		chdef = mrole->chandef;
		dur_i = &mrole->policy.dur_info;
		if (NULL == chdef)
			break;
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "MCC Info: Role(%d): macid(0x%x), Bcn_intvl(%d), band(%d), chan(%d), center_ch(%d), bw(%d), offset(%d)\n",
				i, mrole->macid, mrole->bcn_intvl, chdef->band,
				chdef->chan, chdef->center_ch, chdef->bw,
				chdef->offset);
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "MCC Info: Role(%d): dur(0x%x), lim_en(%d), tag(%d), max_dur(%d), max_toa(%d), max_tob(%d)\n\n",
				i, dur_i->dur, dur_i->dur_limit.enable,
				dur_i->dur_limit.tag, dur_i->dur_limit.max_dur,
				dur_i->dur_limit.max_toa,
				dur_i->dur_limit.max_tob);
	}
	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "======== Hal MCC Info ========\n");
	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "MCC Info: btc_in_group(%d)\n",
			en_info.dbg_hal_i.btc_in_group);
	for (i = 0; i < en_info.dbg_hal_i.slot_num; i++) {
		dbg_slot_i = &en_info.dbg_hal_i.dbg_slot_i[i];
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "MCC Info: FW Slot(%d), BT_Role(%d), Dur(%d), Ch(%d), Macid(0x%x)\n",
				i, dbg_slot_i->bt_role, dbg_slot_i->dur,
				dbg_slot_i->ch, dbg_slot_i->macid);
	}
exit:
	return;
}
#endif /*CONFIG_MCC_SUPPORT*/


void _rxbcn_dbg_info(struct phl_info_t *phl_info, char input[][MAX_ARGV],
	u32 input_num, char *output, u32 out_len)
{
	u32 used = 0;
	u8 idx = 0;
	u16 lower_bound = 0;

	if (!input_num)
		goto _exit;
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used, "\n");
	if (!_os_strcmp("-h", input[2])) {
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "_rxbcn_dbg_info: Para1 string list: show\n");
		goto _exit;
	} else if (!_os_strcmp("show", input[2])) {
		struct rtw_wifi_role_link_t *rlink = NULL;
		struct rtw_phl_stainfo_t *sta = NULL;
		struct rtw_rx_bcn_info bcn_i = {0};
		struct rtw_bcn_histogram *hist_i = NULL;
		struct rtw_wifi_role_t *wrole = NULL;
		u8 ridx = 0, lidx = 0;

		for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
			wrole = phl_get_wrole_by_ridx(phl_info, ridx);
			if ((wrole == NULL) || (wrole->active == false) ||
			    (wrole->mstate != MLME_LINKED))
				continue;
			if (!rtw_phl_role_is_client_category(wrole))
				continue;
			for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
				rlink = get_rlink(wrole, lidx);
				sta = rtw_phl_get_stainfo_self(phl_info, rlink);
				phl_get_sta_bcn_info(phl_info, sta, &bcn_i);
				PHL_DBG_MON_INFO(out_len, used, output + used,
						out_len - used,
						"Pkt len(%d), rate(%d), num_wdg(%d), TBTT_Ofst(%d), TBTT_conf_lvl(%d)\n",
						bcn_i.pkt_len, bcn_i.rate,
						bcn_i.last_num_per_watchdog,
						bcn_i.offset_i.offset,
						bcn_i.offset_i.conf_lvl);
				PHL_DBG_MON_INFO(out_len, used, output + used,
						out_len - used,
						"bcn_dist_i: num(%d), min(%d), max(%d), outlier_num(%d), outlier_l(%d), outlier_h(%d)\n",
						bcn_i.num,
						bcn_i.bcn_dist_i.min,
						bcn_i.bcn_dist_i.max,
						bcn_i.bcn_dist_i.outlier_num,
						bcn_i.bcn_dist_i.outlier_l,
						bcn_i.bcn_dist_i.outlier_h);
				hist_i = &bcn_i.bcn_dist_i.hist_i;
				PHL_DBG_MON_INFO(out_len, used, output + used,
						out_len - used,
						"Mod Histogram, Num(%d)\n",
						hist_i->num);
				for (idx = 0; idx < MAX_NUM_BIN; idx++) {
					PHL_DBG_MON_INFO(out_len, used, output + used,
							out_len - used,
							"Histogram: %2d ~ %02d, num: %d\n",
							lower_bound,
							hist_i->h_array[H_UPPER][idx],
							hist_i->h_array[H_CNT][idx]);
					lower_bound = hist_i->h_array[H_UPPER][idx];
				}
				PHL_DBG_MON_INFO(out_len, used, output + used,
							out_len - used,
							"Suggest bcn_timeout(%d), macid(%d)\n",
							bcn_i.cfg.bcn_timeout,
							bcn_i.cfg.macid);
			}
		}
	} else {
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "_rxbcn_dbg_info: error Para1, try enter -h\n");
		goto _exit;
	}
_exit:
	return;
}


void _bcn_cmd_parser(struct phl_info_t *phl_info, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len)
{
	u32 used = 0;

	if (!input_num)
		return;
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used, "\n");
	if (!_os_strcmp("-h", input[1])) {
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "_bcn_cmd_parser: Para string list: rxbcn\n");
	} else if (!_os_strcmp("rxbcn", input[1])) {
		_rxbcn_dbg_info(phl_info, input, input_num, output, out_len);
	} else {
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "_bcn_cmd_parser: error Para, try enter -h\n");
	}
}

void phl_dbg_cmd_snd(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
#ifdef CONFIG_PHL_CMD_BF
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	u32 role_idx;
	u32 ctrl;
	u32 used = 0;

	if (input_num < 2)
		return;

	_get_hex_from_string(input[1], &ctrl);
	_get_hex_from_string(input[2], &role_idx);

	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used, "ctrl 0x%x with role_idx 0x%x!!\n",
			 (int)ctrl, (int)role_idx);

	if (1 == ctrl) {
		psts = rtw_phl_sound_start(phl_info, (u8)role_idx, 0, 200,
					   PHL_SND_TEST_F_ONE_TIME |
					   PHL_SND_TEST_F_PASS_STS_CHK);
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "SOUND START(once) : wrole %d !!\n", (int)role_idx);
	} else if (2 == ctrl) {
		psts = rtw_phl_sound_abort(phl_info);
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "SOUND ABORT!!\n");
	} else if (3 == ctrl) {
		psts = rtw_phl_sound_start(phl_info, (u8)role_idx, 0, 200,
					   PHL_SND_TEST_F_PASS_STS_CHK);
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "SOUND START(loop) : wrole %d !!\n", (int)role_idx);
	} else if (4 == ctrl) {
		u16 macid[4] = { 0, 0, 0, 0};
		struct rtw_wifi_role_t *role = NULL;
		struct rtw_wifi_role_link_t *rlink = NULL;
		u8 idx = 0;

		role = phl_get_wrole_by_ridx(phl_info, (u8)role_idx);
		if (NULL != role) {
			for (idx = 0; idx < role->rlink_num; idx++) {
				rlink = get_rlink(role, idx);

				psts = rtw_phl_snd_add_grp(phl_info, rlink, 0, (u16 *)&macid, 1, true, false);
				psts = rtw_phl_snd_add_grp(phl_info, rlink, 1, (u16 *)&macid, 1, true, false);

				psts = rtw_phl_sound_start_ex(phl_info, (u8)role_idx, 0, 100,
							PHL_SND_TEST_F_PASS_STS_CHK);

				PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "SOUND START(loop), skip group : wrole %d !!\n", (int)role_idx);
			}
		}
	} else if (9 == ctrl) {
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "DUMP BF Entry in debugview\n");
			rtw_hal_bf_dbg_dump_entry_all(phl_info->hal);
	} else {
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "SOUND TEST CMD not found!\n");
	}
#endif
}

void _convert_tx_rate(enum hal_rate_mode mode, u8 mcs_ss_idx, char *str, u32 str_len)
{
	switch (mode) {
	case HAL_LEGACY_MODE:
		switch (mcs_ss_idx) {
		case RTW_DATA_RATE_CCK1:	_os_snprintf(str, str_len,"CCK 1"); break;
		case RTW_DATA_RATE_CCK2:	_os_snprintf(str, str_len,"CCK 2"); break;
		case RTW_DATA_RATE_CCK5_5:	_os_snprintf(str, str_len,"CCK 5_5"); break;
		case RTW_DATA_RATE_CCK11:	_os_snprintf(str, str_len,"CCK 11"); break;
		case RTW_DATA_RATE_OFDM6:	_os_snprintf(str, str_len,"OFDM 6"); break;
		case RTW_DATA_RATE_OFDM9:	_os_snprintf(str, str_len,"OFDM 9"); break;
		case RTW_DATA_RATE_OFDM12:	_os_snprintf(str, str_len,"OFDM 12"); break;
		case RTW_DATA_RATE_OFDM18:	_os_snprintf(str, str_len,"OFDM 18"); break;
		case RTW_DATA_RATE_OFDM24:	_os_snprintf(str, str_len,"OFDM 24"); break;
		case RTW_DATA_RATE_OFDM36:	_os_snprintf(str, str_len,"OFDM 36"); break;
		case RTW_DATA_RATE_OFDM48:	_os_snprintf(str, str_len,"OFDM 48"); break;
		case RTW_DATA_RATE_OFDM54:	_os_snprintf(str, str_len,"OFDM 54"); break;
		default:
			_os_snprintf(str, str_len,"Unkown rate(0x%01x)", mcs_ss_idx);
			break;
		}
		break;
	case HAL_HT_MODE:
		_os_snprintf(str, str_len,"MCS%d", mcs_ss_idx);
		break;
	case HAL_VHT_MODE:
	case HAL_HE_MODE:
		_os_snprintf(str, str_len,"%dSS MCS%d",
		             ((mcs_ss_idx & 0x70)>> 4) + 1,
		             (mcs_ss_idx & 0x0F));
		break;
	default:
		_os_snprintf(str, str_len,"Unknown mode(0x%01x)", mode);
		break;
	}
}

void _convert_rx_rate(u32 rx_rate, char *str, u32 str_len)
{
	switch(rx_rate) {
	case RTW_DATA_RATE_CCK1:	_os_snprintf(str, str_len,"CCK 1"); break;
	case RTW_DATA_RATE_CCK2:	_os_snprintf(str, str_len,"CCK 2"); break;
	case RTW_DATA_RATE_CCK5_5:	_os_snprintf(str, str_len,"CCK 5_5"); break;
	case RTW_DATA_RATE_CCK11:	_os_snprintf(str, str_len,"CCK 11"); break;
	case RTW_DATA_RATE_OFDM6:	_os_snprintf(str, str_len,"OFDM 6"); break;
	case RTW_DATA_RATE_OFDM9:	_os_snprintf(str, str_len,"OFDM 9"); break;
	case RTW_DATA_RATE_OFDM12:	_os_snprintf(str, str_len,"OFDM 12"); break;
	case RTW_DATA_RATE_OFDM18:	_os_snprintf(str, str_len,"OFDM 18"); break;
	case RTW_DATA_RATE_OFDM24:	_os_snprintf(str, str_len,"OFDM 24"); break;
	case RTW_DATA_RATE_OFDM36:	_os_snprintf(str, str_len,"OFDM 36"); break;
	case RTW_DATA_RATE_OFDM48:	_os_snprintf(str, str_len,"OFDM 48"); break;
	case RTW_DATA_RATE_OFDM54:	_os_snprintf(str, str_len,"OFDM 54"); break;

	case RTW_DATA_RATE_MCS0:	_os_snprintf(str, str_len,"MCS 0"); break;
	case RTW_DATA_RATE_MCS1:	_os_snprintf(str, str_len,"MCS 1"); break;
	case RTW_DATA_RATE_MCS2:	_os_snprintf(str, str_len,"MCS 2"); break;
	case RTW_DATA_RATE_MCS3:	_os_snprintf(str, str_len,"MCS 3"); break;
	case RTW_DATA_RATE_MCS4:	_os_snprintf(str, str_len,"MCS 4"); break;
	case RTW_DATA_RATE_MCS5:	_os_snprintf(str, str_len,"MCS 5"); break;
	case RTW_DATA_RATE_MCS6:	_os_snprintf(str, str_len,"MCS 6"); break;
	case RTW_DATA_RATE_MCS7:	_os_snprintf(str, str_len,"MCS 7"); break;
	case RTW_DATA_RATE_MCS8:	_os_snprintf(str, str_len,"MCS 8"); break;
	case RTW_DATA_RATE_MCS9:	_os_snprintf(str, str_len,"MCS 9"); break;
	case RTW_DATA_RATE_MCS10:	_os_snprintf(str, str_len,"MCS 10"); break;
	case RTW_DATA_RATE_MCS11:	_os_snprintf(str, str_len,"MCS 11"); break;
	case RTW_DATA_RATE_MCS12:	_os_snprintf(str, str_len,"MCS 12"); break;
	case RTW_DATA_RATE_MCS13:	_os_snprintf(str, str_len,"MCS 13"); break;
	case RTW_DATA_RATE_MCS14:	_os_snprintf(str, str_len,"MCS 14"); break;
	case RTW_DATA_RATE_MCS15:	_os_snprintf(str, str_len,"MCS 15"); break;
	case RTW_DATA_RATE_MCS16:	_os_snprintf(str, str_len,"MCS 16"); break;
	case RTW_DATA_RATE_MCS17:	_os_snprintf(str, str_len,"MCS 17"); break;
	case RTW_DATA_RATE_MCS18:	_os_snprintf(str, str_len,"MCS 18"); break;
	case RTW_DATA_RATE_MCS19:	_os_snprintf(str, str_len,"MCS 19"); break;
	case RTW_DATA_RATE_MCS20:	_os_snprintf(str, str_len,"MCS 20"); break;
	case RTW_DATA_RATE_MCS21:	_os_snprintf(str, str_len,"MCS 21"); break;
	case RTW_DATA_RATE_MCS22:	_os_snprintf(str, str_len,"MCS 22"); break;
	case RTW_DATA_RATE_MCS23:	_os_snprintf(str, str_len,"MCS 23"); break;
	case RTW_DATA_RATE_MCS24:	_os_snprintf(str, str_len,"MCS 24"); break;
	case RTW_DATA_RATE_MCS25:	_os_snprintf(str, str_len,"MCS 25"); break;
	case RTW_DATA_RATE_MCS26:	_os_snprintf(str, str_len,"MCS 26"); break;
	case RTW_DATA_RATE_MCS27:	_os_snprintf(str, str_len,"MCS 27"); break;
	case RTW_DATA_RATE_MCS28:	_os_snprintf(str, str_len,"MCS 28"); break;
	case RTW_DATA_RATE_MCS29:	_os_snprintf(str, str_len,"MCS 29"); break;
	case RTW_DATA_RATE_MCS30:	_os_snprintf(str, str_len,"MCS 30"); break;
	case RTW_DATA_RATE_MCS31:	_os_snprintf(str, str_len,"MCS 31"); break;

	case RTW_DATA_RATE_VHT_NSS1_MCS0:	_os_snprintf(str, str_len,"NSS1_MCS0"); break;
	case RTW_DATA_RATE_VHT_NSS1_MCS1:	_os_snprintf(str, str_len,"NSS1_MCS1"); break;
	case RTW_DATA_RATE_VHT_NSS1_MCS2:	_os_snprintf(str, str_len,"NSS1_MCS2"); break;
	case RTW_DATA_RATE_VHT_NSS1_MCS3:	_os_snprintf(str, str_len,"NSS1_MCS3"); break;
	case RTW_DATA_RATE_VHT_NSS1_MCS4:	_os_snprintf(str, str_len,"NSS1_MCS4"); break;
	case RTW_DATA_RATE_VHT_NSS1_MCS5:	_os_snprintf(str, str_len,"NSS1_MCS5"); break;
	case RTW_DATA_RATE_VHT_NSS1_MCS6:	_os_snprintf(str, str_len,"NSS1_MCS6"); break;
	case RTW_DATA_RATE_VHT_NSS1_MCS7:	_os_snprintf(str, str_len,"NSS1_MCS7"); break;
	case RTW_DATA_RATE_VHT_NSS1_MCS8:	_os_snprintf(str, str_len,"NSS1_MCS8"); break;
	case RTW_DATA_RATE_VHT_NSS1_MCS9:	_os_snprintf(str, str_len,"NSS1_MCS9"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS0:	_os_snprintf(str, str_len,"NSS2_MCS0"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS1:	_os_snprintf(str, str_len,"NSS2_MCS1"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS2:	_os_snprintf(str, str_len,"NSS2_MCS2"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS3:	_os_snprintf(str, str_len,"NSS2_MCS3"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS4:	_os_snprintf(str, str_len,"NSS2_MCS4"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS5:	_os_snprintf(str, str_len,"NSS2_MCS5"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS6:	_os_snprintf(str, str_len,"NSS2_MCS6"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS7:	_os_snprintf(str, str_len,"NSS2_MCS7"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS8:	_os_snprintf(str, str_len,"NSS2_MCS8"); break;
	case RTW_DATA_RATE_VHT_NSS2_MCS9:	_os_snprintf(str, str_len,"NSS2_MCS9"); break;

	case RTW_DATA_RATE_VHT_NSS3_MCS0:	_os_snprintf(str, str_len,"NSS3_MCS0"); break;
	case RTW_DATA_RATE_VHT_NSS3_MCS1:	_os_snprintf(str, str_len,"NSS3_MCS1"); break;
	case RTW_DATA_RATE_VHT_NSS3_MCS2:	_os_snprintf(str, str_len,"NSS3_MCS2"); break;
	case RTW_DATA_RATE_VHT_NSS3_MCS3:	_os_snprintf(str, str_len,"NSS3_MCS3"); break;
	case RTW_DATA_RATE_VHT_NSS3_MCS4:	_os_snprintf(str, str_len,"NSS3_MCS4"); break;
	case RTW_DATA_RATE_VHT_NSS3_MCS5:	_os_snprintf(str, str_len,"NSS3_MCS5"); break;
	case RTW_DATA_RATE_VHT_NSS3_MCS6:	_os_snprintf(str, str_len,"NSS3_MCS6"); break;
	case RTW_DATA_RATE_VHT_NSS3_MCS7:	_os_snprintf(str, str_len,"NSS3_MCS7"); break;
	case RTW_DATA_RATE_VHT_NSS3_MCS8:	_os_snprintf(str, str_len,"NSS3_MCS8"); break;
	case RTW_DATA_RATE_VHT_NSS3_MCS9:	_os_snprintf(str, str_len,"NSS3_MCS9"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS0:	_os_snprintf(str, str_len,"NSS4_MCS0"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS1:	_os_snprintf(str, str_len,"NSS4_MCS1"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS2:	_os_snprintf(str, str_len,"NSS4_MCS2"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS3:	_os_snprintf(str, str_len,"NSS4_MCS3"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS4:	_os_snprintf(str, str_len,"NSS4_MCS4"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS5:	_os_snprintf(str, str_len,"NSS4_MCS5"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS6:	_os_snprintf(str, str_len,"NSS4_MCS6"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS7:	_os_snprintf(str, str_len,"NSS4_MCS7"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS8:	_os_snprintf(str, str_len,"NSS4_MCS8"); break;
	case RTW_DATA_RATE_VHT_NSS4_MCS9:	_os_snprintf(str, str_len,"NSS4_MCS9"); break;

	case RTW_DATA_RATE_HE_NSS1_MCS0:	_os_snprintf(str, str_len,"HE_NSS1_MCS0"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS1:	_os_snprintf(str, str_len,"HE_NSS1_MCS1"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS2:	_os_snprintf(str, str_len,"HE_NSS1_MCS2"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS3:	_os_snprintf(str, str_len,"HE_NSS1_MCS3"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS4:	_os_snprintf(str, str_len,"HE_NSS1_MCS4"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS5:	_os_snprintf(str, str_len,"HE_NSS1_MCS5"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS6:	_os_snprintf(str, str_len,"HE_NSS1_MCS6"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS7:	_os_snprintf(str, str_len,"HE_NSS1_MCS7"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS8:	_os_snprintf(str, str_len,"HE_NSS1_MCS8"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS9:	_os_snprintf(str, str_len,"HE_NSS1_MCS9"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS10:	_os_snprintf(str, str_len,"HE_NSS1_MCS10"); break;
	case RTW_DATA_RATE_HE_NSS1_MCS11:	_os_snprintf(str, str_len,"HE_NSS1_MCS11"); break;

	case RTW_DATA_RATE_HE_NSS2_MCS0:	_os_snprintf(str, str_len,"HE_NSS2_MCS0"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS1:	_os_snprintf(str, str_len,"HE_NSS2_MCS1"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS2:	_os_snprintf(str, str_len,"HE_NSS2_MCS2"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS3:	_os_snprintf(str, str_len,"HE_NSS2_MCS3"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS4:	_os_snprintf(str, str_len,"HE_NSS2_MCS4"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS5:	_os_snprintf(str, str_len,"HE_NSS2_MCS5"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS6:	_os_snprintf(str, str_len,"HE_NSS2_MCS6"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS7:	_os_snprintf(str, str_len,"HE_NSS2_MCS7"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS8:	_os_snprintf(str, str_len,"HE_NSS2_MCS8"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS9:	_os_snprintf(str, str_len,"HE_NSS2_MCS9"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS10:	_os_snprintf(str, str_len,"HE_NSS2_MCS10"); break;
	case RTW_DATA_RATE_HE_NSS2_MCS11:	_os_snprintf(str, str_len,"HE_NSS2_MCS11"); break;

	case RTW_DATA_RATE_HE_NSS3_MCS0:	_os_snprintf(str, str_len,"HE_NSS3_MCS0"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS1:	_os_snprintf(str, str_len,"HE_NSS3_MCS1"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS2:	_os_snprintf(str, str_len,"HE_NSS3_MCS2"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS3:	_os_snprintf(str, str_len,"HE_NSS3_MCS3"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS4:	_os_snprintf(str, str_len,"HE_NSS3_MCS4"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS5:	_os_snprintf(str, str_len,"HE_NSS3_MCS5"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS6:	_os_snprintf(str, str_len,"HE_NSS3_MCS6"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS7:	_os_snprintf(str, str_len,"HE_NSS3_MCS7"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS8:	_os_snprintf(str, str_len,"HE_NSS3_MCS8"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS9:	_os_snprintf(str, str_len,"HE_NSS3_MCS9"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS10:	_os_snprintf(str, str_len,"HE_NSS3_MCS10"); break;
	case RTW_DATA_RATE_HE_NSS3_MCS11:	_os_snprintf(str, str_len,"HE_NSS3_MCS11"); break;

	case RTW_DATA_RATE_HE_NSS4_MCS0:	_os_snprintf(str, str_len,"HE_NSS4_MCS0"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS1:	_os_snprintf(str, str_len,"HE_NSS4_MCS1"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS2:	_os_snprintf(str, str_len,"HE_NSS4_MCS2"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS3:	_os_snprintf(str, str_len,"HE_NSS4_MCS3"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS4:	_os_snprintf(str, str_len,"HE_NSS4_MCS4"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS5:	_os_snprintf(str, str_len,"HE_NSS4_MCS5"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS6:	_os_snprintf(str, str_len,"HE_NSS4_MCS6"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS7:	_os_snprintf(str, str_len,"HE_NSS4_MCS7"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS8:	_os_snprintf(str, str_len,"HE_NSS4_MCS8"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS9:	_os_snprintf(str, str_len,"HE_NSS4_MCS9"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS10:	_os_snprintf(str, str_len,"HE_NSS4_MCS10"); break;
	case RTW_DATA_RATE_HE_NSS4_MCS11:	_os_snprintf(str, str_len,"HE_NSS4_MCS11"); break;

	case RTW_DATA_RATE_MAX:			_os_snprintf(str, str_len,"RATE_MAX"); break;
	default:
		_os_snprintf(str, str_len,"Unknown rate(0x%01x)", (unsigned int)rx_rate);
		break;
	}
}

void phl_dbg_cmd_asoc_sta(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 ctrl = 0, wrole_idx = 0;
	u32 used = 0;
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_phl_stainfo_t *n, *psta;
	void *drv = phl_to_drvpriv(phl_info);
	char tx_rate_str[32] = {0};
	char rx_rate_str[32] = {0};
	u8 idx = 0;
	u8 path_idx = 0;
	u16 rssi = 0;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_wifi_role_link_t *rlink = NULL;

	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "\nDUMP Associate STA infomations\n");
	if(input_num < 2) {
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "phl asoc_sta [option] [wrole index]\n"
					"option:\n"
					"1\tdump list basic information\n"
					"2\tdump list badic & cap information\n");
		return;
	}

	if (input_num > 1)
		_get_hex_from_string(input[1], &ctrl);

	if (input_num > 2)
		_get_hex_from_string(input[2], &wrole_idx);

	if (wrole_idx >= MAX_WIFI_ROLE_NUMBER) {
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "Wrole Index shall < %d\n",
				MAX_WIFI_ROLE_NUMBER);
		return;
	}
	wrole = phl_get_wrole_by_ridx(phl_info, (u8)wrole_idx);
	if (1 == ctrl || 2 == ctrl) {
		for (idx = 0; idx < wrole->rlink_num; idx++) {
			rlink = get_rlink(wrole, idx);
			_os_spinlock(drv, &rlink->assoc_sta_queue.lock, _bh, NULL);
			phl_list_for_loop_safe(psta, n, struct rtw_phl_stainfo_t,
			       &rlink->assoc_sta_queue.queue, list) {
				PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "\nmac_id 0x%x ; aid 0x%x ; mac addr %02x-%02x-%02x-%02x-%02x-%02x ; AX_Support %d\n",
				(int)psta->macid, (int)psta->aid,
				psta->mac_addr[0], psta->mac_addr[1],
				psta->mac_addr[2], psta->mac_addr[3],
				psta->mac_addr[4], psta->mac_addr[5],
				(psta->wmode&WLAN_MD_11AX) ? 1 : 0);

				PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "WROLE-IDX:%d RLINK-IDX:%d wlan_mode:0x%02x, chan:%d, bw:%d, rlink_state:%s\n",
				psta->wrole->id,
				psta->rlink->id,
				psta->wmode,
				psta->chandef.chan,
				psta->chandef.bw,
				rlink->mstate?((rlink->mstate & MLME_LINKING)?"Linking":"Linked Up"):"No Link");

				PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "[Stats] Tput - [Tx:%d KBits Rx:%d KBits]\n",
				(int)psta->stats.tx_tp_kbits, (int)psta->stats.rx_tp_kbits);

				PHL_DBG_MON_INFO(out_len, used, output + used,
						 out_len - used, "[Stats] MA SNR:%d\n",
						 psta->hal_sta->rssi_stat.snr_ma >> PHL_RSSI_MA_H );

				PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "[Stats] MA RSSI:%d(dBm)\n",
				psta->hal_sta->rssi_stat.ma_rssi - PHL_MAX_RSSI);

				for (path_idx = 0;path_idx < phl_com->phy_cap[0].rx_path_num; path_idx++) {
					rssi = (psta->hal_sta->rssi_stat.rssi_ma_path[path_idx] >> 5) & 0x7f;
					PHL_DBG_MON_INFO(out_len, used, output + used,
							 out_len - used, "[Stats] Path[%d] MA RSSI:%d(dBm)\n",
		  					 path_idx, rssi - PHL_MAX_RSSI);

					rssi = (psta->hal_sta->rssi_stat.rssi_bcn_ma_path[path_idx] >> 5) & 0x7f;
					PHL_DBG_MON_INFO(out_len, used, output + used,
							 out_len - used, "[Stats] Path[%d] BCN MA RSSI:%d(dBm)\n",
							 path_idx, rssi - PHL_MAX_RSSI);
				}

				if((HAL_HT_MODE == psta->hal_sta->ra_info.rpt_rt_i.mode)
					|| (HAL_VHT_MODE == psta->hal_sta->ra_info.rpt_rt_i.mode)) {
					PHL_DBG_MON_INFO(out_len, used, output + used,
						out_len - used, "[Stats] is_support_sgi:%s\n",
						(RTW_GILTF_SGI_4XHE08 == psta->hal_sta->ra_info.rpt_rt_i.gi_ltf)?"yes":"no");
				} else {
					PHL_DBG_MON_INFO(out_len, used, output + used,
						out_len - used, "[Stats] is_support_sgi:no\n");
				}

				_convert_tx_rate( psta->hal_sta->ra_info.rpt_rt_i.mode,
						psta->hal_sta->ra_info.rpt_rt_i.mcs_ss_idx,
						tx_rate_str, 32);
				PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "[Stats] Tx Rate:%s\n",
					tx_rate_str);

				_convert_rx_rate(psta->stats.rx_rate, rx_rate_str, 32);
				PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "[Stats] Rx Rate:%s\n",
					rx_rate_str);

				PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "[Stats] rx_ok_cnt:%d rx_err_cnt:%d\n",
					(int)psta->hal_sta->trx_stat.rx_ok_cnt,
					(int)psta->hal_sta->trx_stat.rx_err_cnt);

				PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "[Stats] curr_retry_ratio:%d\n",
					psta->hal_sta->ra_info.curr_retry_ratio);

				PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "[Stats] dis_ra:%s\n",
					psta->hal_sta->ra_info.dis_ra?"yes":"no");

				PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "[Stats] ra_mask:0x%016llx\n",
					psta->hal_sta->ra_info.ra_mask);

				PHL_DBG_MON_INFO(out_len, used, output + used,
					out_len - used, "[Stats] cur_ra_mask:0x%016llx\n",
					psta->hal_sta->ra_info.cur_ra_mask);

				PHL_DBG_MON_INFO(out_len, used, output + used,
						 out_len - used, "[Stats] tx retry ratio:%d %%\n",
						 (u8)psta->hal_sta->ra_info.curr_retry_ratio);

				if (ctrl == 2)
					PHL_DBG_MON_DUMP(out_len, used, output + used, out_len - used,
						"[Cap] asoc_cap", &psta->asoc_cap, sizeof(psta->asoc_cap));
			}
			_os_spinunlock(drv, &rlink->assoc_sta_queue.lock, _bh, NULL);
		}
	}
}

void phl_dbg_cmd_phy_stats(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	u32 used = 0;
	int i;

	PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "\nDUMP PHY stats\n");
	for(i=0; i<HW_BAND_MAX; i++) {
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "\n[PHY%d]\tcurrent_igi = 0x%x, fa_cnt_cck = %d, fa_cnt_ofdm = %d, Total False Alarm = %d\n",
			i,
			(unsigned int)hal_com->band[i].stat_info.igi_fa_rssi,
			(int)hal_com->band[i].stat_info.cnt_cck_fail,
			(int)hal_com->band[i].stat_info.cnt_ofdm_fail,
			(int)hal_com->band[i].stat_info.cnt_fail_all);

		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "\tcnt_cca_all = %d, cnt_crc32_error_all = %d, cnt_crc32_ok_all = %d\n",
			(int)hal_com->band[i].stat_info.cnt_cca_all,
			(int)hal_com->band[i].stat_info.cnt_crc32_error_all,
			(int)hal_com->band[i].stat_info.cnt_crc32_ok_all);
	}
}

#ifdef CONFIG_PCI_HCI
void _dbg_tx_stats_pcie(struct phl_info_t *phl_info, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len, u32 *used,
			u32 ctrl, u32 ch)

{
	struct hci_info_t *hci_info = (struct hci_info_t *)phl_info->hci;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);
	struct rtw_wp_rpt_stats *rpt_stats = NULL;

	rpt_stats = (struct rtw_wp_rpt_stats *)hal_com->trx_stat.wp_rpt_stats;

	if (ch >= hci_info->total_txch_num) {
		PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
				 "\n Invalid channel number\n");
		return;
	}
	if (0xFF == ctrl) {
		rpt_stats[ch].tx_ok_cnt = 0;
		rpt_stats[ch].rty_fail_cnt = 0;
		rpt_stats[ch].lifetime_drop_cnt = 0;
		rpt_stats[ch].macid_drop_cnt = 0;
		rpt_stats[ch].sw_drop_cnt = 0;
		rpt_stats[ch].recycle_fail_cnt = 0;
		rpt_stats[ch].delay_tx_ok_cnt = 0;
		rpt_stats[ch].delay_rty_fail_cnt = 0;
		rpt_stats[ch].delay_lifetime_drop_cnt = 0;
		rpt_stats[ch].delay_macid_drop_cnt = 0;
		return;
	}

	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"\n== wp report statistics == \n");
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"ch			: %u\n", (int)ch);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"busy count		: %u\n", (int)rpt_stats[ch].busy_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"ok count		: %u\n", (int)rpt_stats[ch].tx_ok_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"retry fail count	: %u\n",
			 (int)rpt_stats[ch].rty_fail_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"lifetime drop count	: %u\n",
			 (int)rpt_stats[ch].lifetime_drop_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"macid drop count	: %u\n",
			 (int)rpt_stats[ch].macid_drop_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"sw drop count		: %u\n",
			 (int)rpt_stats[ch].sw_drop_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"recycle fail count	: %u\n",
			 (int)rpt_stats[ch].recycle_fail_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"delay ok count			: %u\n",
			 (int)rpt_stats[ch].delay_tx_ok_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"delay retry fail count		: %u\n",
			 (int)rpt_stats[ch].delay_rty_fail_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"delay lifetime drop count	: %u\n",
			 (int)rpt_stats[ch].delay_lifetime_drop_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"delay macid drop count		: %u\n",
			 (int)rpt_stats[ch].delay_macid_drop_cnt);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"ltr tx delay count		: %u\n",
			 (int)hal_com->trx_stat.ltr_tx_dly_count);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
		"ltr last tx delay time	: %u\n",
			 (int)hal_com->trx_stat.ltr_last_tx_dly_time);
}
#endif

void phl_dbg_ltr_stats(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		       u32 input_num, char *output, u32 out_len)
{
	#ifdef CONFIG_PCI_HCI
	u32 used = 0;
	#ifdef RTW_WKARD_DYNAMIC_LTR
		if (input_num == 3) {
			if (0 == _os_strcmp(input[1], "set")) {
					if (0 == _os_strcmp(input[2], "disable")) {
						phl_ltr_hw_ctrl(phl_info->phl_com, false);
						phl_ltr_sw_ctrl(phl_info->phl_com, phl_info->hal, false);
					} else if (0 == _os_strcmp(input[2], "enable")) {
						phl_ltr_hw_ctrl(phl_info->phl_com, true);
						phl_ltr_sw_ctrl(phl_info->phl_com, phl_info->hal, true);
					} else {
						return;
					}
			}
			return;
		}

		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"\nltr sw ctrl   	   : %u\n",
				rtw_hal_ltr_is_sw_ctrl(phl_info->phl_com, phl_info->hal) ? 1 : 0);
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"ltr hw ctrl   	   : %u\n",
				rtw_hal_ltr_is_hw_ctrl(phl_info->phl_com, phl_info->hal) ? 1 : 0);
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"ltr current state   : %u\n",
				phl_ltr_get_cur_state(phl_info->phl_com));
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"ltr last trigger time : %lu\n",
				phl_ltr_get_last_trigger_time(phl_info->phl_com));
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"ltr active trigger cnt: %lu\n",
				phl_ltr_get_tri_cnt(phl_info->phl_com, RTW_PCIE_LTR_SW_ACT));
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"ltr idle trigger cnt: %lu\n",
				phl_ltr_get_tri_cnt(phl_info->phl_com, RTW_PCIE_LTR_SW_IDLE));
	#else
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"Not Support Dynamical LTR\n");
	#endif
	#endif
}

void phl_dbg_trx_stats(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		       u32 input_num, char *output, u32 out_len)
{
 	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_stats *phl_stats = &phl_com->phl_stats;
	u32 used = 0;
	u32 ctrl = 0;
	u32 ch = 0;

	if (input_num < 2) {
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"Invalid Input\n");
		return;
	}
	_get_hex_from_string(input[1], &ch);

	if (input_num == 3) {
		_get_hex_from_string(input[2], &ctrl);
	}

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"\nunicast tx bytes	: %llu\n", phl_stats->tx_byte_uni);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"total tx bytes		: %llu\n", phl_stats->tx_byte_total);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		 "tx throughput		: %d(kbps)\n",
			 (int)phl_stats->tx_tp_kbits);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"last tx time		: %d(ms)\n",
			 (int)phl_stats->last_tx_time_ms);

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"tx request num to phl	: %d\n",
			 (int)phl_stats->txreq_num);

#ifdef CONFIG_PCI_HCI
	_dbg_tx_stats_pcie(phl_info, input, input_num, output, out_len, &used,
			   ctrl, ch);
#endif

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"\nunicast rx bytes	: %llu\n", phl_stats->rx_byte_uni);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"total rx bytes		: %llu\n", phl_stats->rx_byte_total);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		 "rx throughput		: %d(kbps)\n",
			 (int)phl_stats->rx_tp_kbits);
	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
		"last rx time		: %d(ms)\n",
			 (int)phl_stats->last_rx_time_ms);
}

void phl_dbg_cmd_show_rssi(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		 	   u32 input_num, char *output, u32 out_len)
{
	u32 used = 0;
	struct rtw_phl_rssi_stat *rssi_stat = NULL;
	struct rtw_phl_stainfo_t *psta = NULL;
	struct rtw_phl_com_t *phl_com = NULL;
	u32 macid = 0;

	if (NULL == phl_info)
		return;

	rssi_stat = &phl_info->phl_com->rssi_stat;
	phl_com = phl_info->phl_com;

	_os_spinlock(phl_com->drv_priv,
		     &(phl_com->rssi_stat.lock), _bh, NULL);

	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used, "\nShow Moving Average RSSI Statistics Informations\n");

	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "DATA (A1 Match, AddrCam Valid) : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_DATA_ACAM_A1M]);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "DATA (A1 Not Match, AddrCam Valid) : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_DATA_ACAM]);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "DATA (other) : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_DATA_OTHER]);

	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "MGNT (A1 Match, AddrCam Valid) : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_MGNT_ACAM_A1M]);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "MGNT (A1 Not Match, AddrCam Valid) : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_MGNT_ACAM]);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "MGNT (other) : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_MGNT_OTHER]);

	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "CTRL (A1 Match, AddrCam Valid) : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_CTRL_ACAM_A1M]);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "CTRL (A1 Not Match, AddrCam Valid) : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_CTRL_ACAM]);
	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "CTRL (other) : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_CTRL_OTHER]);

	PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "Unknown : %d\n",
			 rssi_stat->ma_rssi[RTW_RSSI_UNKNOWN]);
	_os_spinunlock(phl_com->drv_priv,
		     &(phl_com->rssi_stat.lock), _bh, NULL);

	if(input_num > 1) {
		_get_hex_from_string(input[1], &macid);
		psta = rtw_phl_get_stainfo_by_macid(phl_info, (u16)macid);
		if (psta != NULL) {
			PHL_DBG_MON_INFO(out_len, used, output + used,
			 out_len - used,
			 "STA macid : %d ; mv_avg_rssi = %d ; asoc_rssi = %d (bb : %d, %d)\n",
			 (int)macid, (int)psta->hal_sta->rssi_stat.ma_rssi,
			 (int)psta->hal_sta->rssi_stat.assoc_rssi,
			 (int)(psta->hal_sta->rssi_stat.rssi >> 1),
			 (int)(psta->hal_sta->rssi_stat.rssi_ma >> 5));
		}
	}

}

static void _phl_switch_lamode(struct phl_info_t *phl_info, char input[][MAX_ARGV],
			 u32 input_num, char *output, u32 out_len)
{
	u32 used = 0;
	u32 mode = 0;
	struct rtw_phl_com_t *phl_com = (struct rtw_phl_com_t *) phl_info->phl_com;

	_get_hex_from_string(input[1], &mode);
	phl_com->dev_cap.la_mode = mode > 0 ? true:false;

	if (phl_com->dev_cap.la_mode){
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used, "enable lamode\n");
	} else {
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used, "disable lamode\n");
	}
}

#ifdef CONFIG_PHL_CHANNEL_INFO_DBG
void _phl_dbg_cmd_chan_info(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
	u32 en;
	u32 data_sz = 0;
	u32 mimo_sz = 0;
	u32 group_num = 0;
	u32 used = 0;
	u32 mode = 0;
	struct rtw_chinfo_action_parm act_parm;
	struct rtw_phl_stainfo_t *phl_sta;
	struct rtw_wifi_role_link_t *rlink;
	void *d = phl_to_drvpriv(phl_info);

	if (input_num != 5 && input_num != 2 && input_num != 6){
		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used,
				 "\n[DBG] echo phl set_chan_info [en(0,1)] [mode(0,1,2,3)]");

		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used,
				 "[data_sz(0,1)] [mimo_sz(hex)] [group_num(0,1,2,3)]\n");

		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used,
				 "\n[DBG] echo phl set_chan_info [en(0,1)] [data_sz(0,1)] [mimo_sz(hex)] [group_num(0,1,2,3)]\n");

		PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used,
				 "\n[DBG] echo phl set_chan_info 0 to disable\n");
		return;
	}

	rlink = &(phl_info->phl_com->wifi_roles[0].rlink[0]);
	if (rlink->mstate != MLME_LINKED) {
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used,
			"[DBG] please connect to AP firset\n");
		return;
	}

	phl_sta = rtw_phl_get_stainfo_self(phl_info, rlink);
	if (phl_sta == NULL || phl_sta->active != true) {
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used,
			"[DBG] please check interface state first\n");
		return;
	}

	if (!_get_hex_from_string(input[1], &en))
		return;

	PHL_DBG_MON_INFO(out_len, used, output + used,
					 out_len - used,
					 "\n[DBG] PHL_DBG_CHAN_INFO ==> en = %d\n",
					 (int)en);

	_os_mem_set(d, &act_parm, 0, sizeof(struct rtw_chinfo_action_parm));

	if (input_num == 2) {
		act_parm.act = CHINFO_ACT_EN;
		act_parm.sta = phl_sta;
		act_parm.mode = CHINFO_MODE_MACID;
		act_parm.enable = (u8)en;
		rtw_phl_cmd_cfg_chinfo(phl_info, &act_parm, PHL_CMD_WAIT, 500);
	} else {
		if (input_num == 5) {
			/* default for macid hit */
			act_parm.mode = CHINFO_MODE_MACID;
			if (!_get_hex_from_string(input[2], &data_sz))
				return;

			if (!_get_hex_from_string(input[3], &mimo_sz))
				return;

			if (!_get_hex_from_string(input[4], &group_num))
				return;
		} else if (input_num == 6) {
			/* choose mode*/
			if (!_get_hex_from_string(input[2], &mode))
				return;
			act_parm.mode = mode;
			if (!_get_hex_from_string(input[3], &data_sz))
				return;

			if (!_get_hex_from_string(input[4], &mimo_sz))
				return;

			if (!_get_hex_from_string(input[5], &group_num))
				return;
		}

		PHL_DBG_MON_INFO(out_len, used, output + used,
					 out_len - used,
					 "[DBG] PHL_DBG_CHAN_INFO ==> mode = %d\n",
					 (int)act_parm.mode);

		if(data_sz == 0) {
			act_parm.accuracy = CHINFO_ACCU_1BYTE;
			PHL_DBG_MON_INFO(out_len, used, output + used,
					 out_len - used,
					 "[DBG] PHL_DBG_CHAN_INFO ==> data_sz = 1BYTE(%d)\n",
					 (int)data_sz);
		} else if (data_sz == 1) {
			act_parm.accuracy = CHINFO_ACCU_2BYTES;
			PHL_DBG_MON_INFO(out_len, used, output + used,
					 out_len - used,
					 "[DBG] PHL_DBG_CHAN_INFO ==> data_sz = 2BYTES(%d)\n",
					 (int)data_sz);
		} else {
			PHL_DBG_MON_INFO(out_len, used, output + used,
					 out_len - used,
					 "[DBG] PHL_DBG_CHAN_INFO ==> unknown data_sz = %d\n",
					 (int)data_sz);
			return;
		}

		PHL_DBG_MON_INFO(out_len, used, output + used,
					 out_len - used,
					 "[DBG] PHL_DBG_CHAN_INFO ==> mimo_sz = 0x%02x\n",
					 (int)mimo_sz);

		if(group_num == 0) {
			act_parm.group_num = CHINFO_GROUP_NUM_1;
			PHL_DBG_MON_INFO(out_len, used, output + used,
						 out_len - used,
						 "[DBG] PHL_DBG_CHAN_INFO ==> group_num = 1/1(%d)\n",
						 (int)group_num);
		} else if (group_num == 1) {
			act_parm.group_num = CHINFO_GROUP_NUM_2;
			PHL_DBG_MON_INFO(out_len, used, output + used,
						 out_len - used,
						 "[DBG] PHL_DBG_CHAN_INFO ==> group_num = 1/2(%d)\n",
						 (int)group_num);
		} else if (group_num == 2) {
			act_parm.group_num = CHINFO_GROUP_NUM_4;
			PHL_DBG_MON_INFO(out_len, used, output + used,
						 out_len - used,
						 "[DBG] PHL_DBG_CHAN_INFO ==> group_num = 1/4(%d)\n",
						 (int)group_num);
		} else if (group_num == 3) {
			act_parm.group_num = CHINFO_GROUP_NUM_16;
			PHL_DBG_MON_INFO(out_len, used, output + used,
						 out_len - used,
						 "[DBG] PHL_DBG_CHAN_INFO ==> group_num = 1/16(%d)\n",
						 (int)group_num);
		} else {
			PHL_DBG_MON_INFO(out_len, used, output + used,
						 out_len - used,
						 "[DBG] PHL_DBG_CHAN_INFO ==> unknown group_num = %d\n",
						 (int)group_num);
			return;
		}
		act_parm.act = CHINFO_ACT_CFG;
		act_parm.sta = phl_sta;
		act_parm.enable = (u8)en;
		act_parm.trig_period = 30;
		act_parm.ele_bitmap = mimo_sz;
		rtw_phl_cmd_cfg_chinfo(phl_info, &act_parm, PHL_CMD_WAIT, 500);
		act_parm.act = CHINFO_ACT_EN;
		rtw_phl_cmd_cfg_chinfo(phl_info, &act_parm, PHL_CMD_WAIT, 500);
	}
}
#endif

void phl_dbg_cmd_parser(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		        u32 input_num, char *output, u32 out_len)
{
	u8 id = 0;
	u32 i;
	u32 used = 0;
	u32 phl_ary_size = sizeof(phl_dbg_cmd_i) /
			   sizeof(struct phl_dbg_cmd_info);

	if (phl_ary_size == 0)
		return;

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used, "\n");
	/* Parsing Cmd ID */
	if (input_num) {
		for (i = 0; i < phl_ary_size; i++) {
			if (_os_strcmp(phl_dbg_cmd_i[i].name, input[0]) == 0) {
				id = phl_dbg_cmd_i[i].id;
				PHL_DBG("[%s]===>\n", phl_dbg_cmd_i[i].name);
				break;
			}
		}
		if (i == phl_ary_size) {
			PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "PHL CMD not found!\n");
			return;
		}
	}

	switch (id) {
	case PHL_DBG_MON_HELP:
	{
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				 "phl_dbg_cmd_parser : PHL_DBG_MON_HELP \n");
		for (i = 0; i < phl_ary_size - 2; i++)
			PHL_DBG_MON_INFO(out_len, used, output + used,
					 out_len - used, "%-5d: %s\n",
			          (int)i, phl_dbg_cmd_i[i + 2].name);

	}
	break;
	case PHL_DBG_MON_TEST:
	{
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				 "[DBG] PHL_DBG_MON_TEST (SAMPLE) \n");
	}
	break;
	case PHL_DBG_COMP:
	{
		u32 ctrl = 0, comp = 0;
		u32 ret = 0;

		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"[DBG] PHL_DBG_COMP [1=set comp_bit, 2=clear comp_bit, 3=show, 4=set value] \n");
		if (input_num <= 1)
			break;
		_get_hex_from_string(input[1], &ctrl);
		if (ctrl != 3 && input_num <= 2)
			break;
		_get_hex_from_string(input[2], &comp);

		ret = rtw_phl_dbg_ctrl_comp((u8)ctrl, comp);
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"[DBG] PHL_DBG_COMP (COMP = 0x%x) \n", (int)ret);
	}
	break;
	case PHL_DBG_DUMP_WROLE:
	{
		_dump_wifi_role(phl_info, input, input_num, output, out_len);
	}
	break;
	case PHL_DBG_SET_CH_BW:
	{
		_phl_dbg_cmd_switch_chbw(phl_info, input, input_num,
					 output, out_len);
	}
	break;
	case PHL_DBG_SHOW_RX_RATE:
	{
		_dump_rx_rate(phl_info, input, input_num, output, out_len);
	}
	break;
#ifdef DEBUG_PHL_RX
	case PHL_DBG_PHL_RX:
	{
		phl_dbg_cmd_phl_rx(phl_info, input, input_num, output, out_len);
	}
	break;
#endif
	case PHL_DBG_SOUND :
	{
		phl_dbg_cmd_snd(phl_info, input, input_num, output, out_len);
	}
	break;
	case PHL_DBG_ASOC_STA:
	{
		phl_dbg_cmd_asoc_sta(phl_info, input, input_num, output, out_len);
	}
	break;
	case PHL_DBG_PHY_STATS:
	{
		phl_dbg_cmd_phy_stats(phl_info, input, input_num, output, out_len);
	}
	break;
	#ifdef CONFIG_FSM
	case PHL_DBG_FSM:
	{
		phl_fsm_dbg(phl_info, input, input_num, output, out_len);
	}
	break;
	#endif
	case PHL_DBG_TRX_STATS:
	{
		phl_dbg_trx_stats(phl_info, input, input_num, output, out_len);
	}
	break;
	case PHL_DBG_LTR:
	{
		phl_dbg_ltr_stats(phl_info, input, input_num, output, out_len);
	}
	break;
	case PHL_SHOW_RSSI_STAT :
	{
		phl_dbg_cmd_show_rssi(phl_info, input, input_num, output, out_len);
	}
	break;
	case PHL_DBG_SER:
	{
		phl_ser_cmd_parser(phl_info, input, input_num, output, out_len);
	}
	break;
	case PHL_DBG_WOW:
	{
		_dump_wow_stats(phl_info, input, input_num, output, out_len);
	}
	break;
#ifdef CONFIG_POWER_SAVE
	case PHL_DBG_PS:
	{
		phl_ps_cmd_parser(phl_info, input, input_num, output, out_len);
	}
	break;
#endif
#ifdef CONFIG_MCC_SUPPORT
	case PHL_DBG_MCC:
	{
		_dump_mcc_info(phl_info, input, input_num, output, out_len);
	}
	break;
#endif /* CONFIG_MCC_SUPPORT */
	case PHL_DBG_BCN:
	{
		_bcn_cmd_parser(phl_info, input, input_num, output, out_len);
	}
	break;
	case PHL_DBG_MR:
	{
		_dump_mr_info(phl_info, output, out_len);
	}
	break;

	case PHL_DBG_ECSA:
	{
#ifdef CONFIG_PHL_ECSA
		struct rtw_wifi_role_t *role = NULL;
		struct rtw_phl_ecsa_param param = {0};
		int chan = 0;
		int bw = 0;
		int offset = 0;
		int count = 0;
		int op_class = 0;
		int mode = 0;
		int delay_start_ms = 0;
		if (input_num <= 7)
			break;
		_os_sscanf(input[1], "%d", &chan);
		_os_sscanf(input[2], "%d", &bw);
		_os_sscanf(input[3], "%d", &offset);
		_os_sscanf(input[4], "%d", &count);
		_os_sscanf(input[5], "%d", &op_class);
		_os_sscanf(input[6], "%d", &mode);
		_os_sscanf(input[7], "%d", &delay_start_ms);
		role = phl_get_wrole_by_ridx(phl_info, 2);
		if(role){
			param.ecsa_type = ECSA_TYPE_AP;
			param.ch = (u8)chan;
			param.count = (u8)count;
			param.flag = 0;
			param.mode = (u8)mode;
			param.op_class = (u8)op_class;
			param.delay_start_ms = delay_start_ms;
			param.new_chan_def.chan = (u8)chan;
			param.new_chan_def.bw = (u8)bw;
			param.new_chan_def.offset = (u8)offset;

			if (role->rlink_num == 1) {
				rtw_phl_ecsa_start(phl_info,
				                   role,
				                   &role->rlink[role->rlink_num-1],
				                   &param);
			} else {
				PHL_DBG_MON_INFO(out_len, used, output + used,
				out_len - used, "[DBG] MLD not support ECSA\n");
			}
		}
		else
			PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "[DBG] Role 2 is NULL\n");
#else
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "[DBG] ECSA not support!\n");
#endif /* CONFIG_PHL_ECSA */
	}
	break;

	case PHL_DBG_LA_ENABLE:
		_phl_switch_lamode(phl_info, input, input_num, output, out_len);
	break;

	case PHL_DBG_CFG_TX_DUTY:
	{
		int tx_duty = 100;
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"[DBG] PHL_DBG_CFG_TX_DUTY [100-30] \n");
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"[DBG] TX duty interval = 100ms \n");
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"[DBG] No TX duty control: 100 \n");
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"[DBG] Minimum TX duty control: 30 (TX 30ms pause TX 70ms)\n");
		if (input_num <= 1)
			break;
		_os_sscanf(input[1], "%d", &tx_duty);
		if (tx_duty < 30 || tx_duty > 100) {
			PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
				"[DBG] TX duty should be 30~100 \n");
			break;
		}
		if (tx_duty == 100)
			phl_thermal_protect_stop_tx_duty(phl_info);
		else
			phl_thermal_protect_cfg_tx_duty(phl_info, 100, (u8)tx_duty);

		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			"[DBG] Current TX duty control: %d \n", tx_duty);
	}
	break;
#ifdef CONFIG_PHL_CHANNEL_INFO_DBG
	case PHL_DBG_CHAN_INFO:
		_phl_dbg_cmd_chan_info(phl_info, input, input_num, output, out_len);
		break;
#endif
	default:
		PHL_DBG_MON_INFO(out_len, used, output + used,
			out_len - used, "[DBG] Do not support this command\n");
		break;
	}
}

s32
phl_dbg_proc_cmd(struct phl_info_t *phl_info,
		 char *input, char *output, u32 out_len)
{
	char *token;
	u32 argc = 0;
	char argv[MAX_ARGC][MAX_ARGV];

	do {
		token = _os_strsep(&input, ", ");
		if (token) {
			if (_os_strlen((u8 *)token) <= MAX_ARGV)
				_os_strcpy(argv[argc], token);

			argc++;
		} else {
			break;
		}
	} while (argc < MAX_ARGC);

	phl_dbg_cmd_parser(phl_info, argv, argc, output, out_len);

	return 0;
}

enum rtw_hal_status
rtw_phl_dbg_proc_cmd(struct phl_info_t *phl_info,
		     struct rtw_proc_cmd *incmd,
		     char *output,
		     u32 out_len)
{
	if (incmd->in_type == RTW_ARG_TYPE_BUF) {
		phl_dbg_proc_cmd(phl_info, incmd->in.buf, output, out_len);
	} else if(incmd->in_type == RTW_ARG_TYPE_ARRAY){
		phl_dbg_cmd_parser(phl_info, incmd->in.vector,
				   incmd->in_cnt_len, output, out_len);
	}
	return RTW_HAL_STATUS_SUCCESS;
}

#endif
