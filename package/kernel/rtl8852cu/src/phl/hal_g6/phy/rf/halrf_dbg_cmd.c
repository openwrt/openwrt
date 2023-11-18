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
#include "halrf_precomp.h"

struct halrf_cmd_info {
	char name[16];
	u8 id;
};

enum HALRF_CMD_ID {
	HALRF_HELP,
	HALRF_SUPPORTABILITY,
	HALRF_DBG_COMP,
	HALRF_PROFILE,
	HALRF_IQK,
	HALRF_IQK_DEBUG,
	HALRF_DPK,
	HALRF_DPK_TRACK,
	HALRF_DACK,
	HALRF_DACK_DEBUG,
	HALRF_RX_DCK,
	HALRF_DUMP_RFK_REG,
	HALRF_LO_TEST,
#ifdef CONFIG_2G_BAND_SHIFT
	HAL_BAND_SHIFT,
#endif
	HALRF_RCK,
	HALRF_WATCHDOG,
	HALRF_SINGLETONE_EN,
	HALRF_IQK_BYPASS,
	HALRF_IQK_KLOG,
	HALRF_TSSI,
	HALRF_PWR_TABLE,
	HALRF_IQK_DBCC,
	HALRF_RFK_CHECK,
	HALRF_RFK_TEST,
	HALRF_TXGAPK,
	HALRF_FW_IQK,
	HALRF_DUMP,
	HALRF_IQK_RXIMR,
	HALRF_THER,
	HALRF_XTAL_TRK,
	HALRF_HWTX,
	HALRF_KFREE,
	HALRF_CHL_RFK,
	HALRF_OP5K,
	HALRF_RFK
};

struct halrf_cmd_info halrf_cmd_i[] = {
	{"-h", HALRF_HELP},
	{"ability", HALRF_SUPPORTABILITY},
	{"dbg", HALRF_DBG_COMP},
	{"profile", HALRF_PROFILE},
	{"iqk", HALRF_IQK},
	{"dpk", HALRF_DPK},
	{"dpk_trk", HALRF_DPK_TRACK},
	{"dack", HALRF_DACK},
	{"dack_dbg", HALRF_DACK_DEBUG},
	{"rx_dck", HALRF_RX_DCK},
	{"dump_rfk_reg", HALRF_DUMP_RFK_REG},
	{"lo_test", HALRF_LO_TEST},	
	{"rck", HALRF_RCK},
	{"rfk_check", HALRF_RFK_CHECK},
#ifdef CONFIG_2G_BAND_SHIFT
	{"band_shift", HAL_BAND_SHIFT},
#endif
	{"watchdog_stop", HALRF_WATCHDOG},
	{"iqk_tone", HALRF_SINGLETONE_EN},
	{"iqk_bypass", HALRF_IQK_BYPASS},
	{"iqk_klog", HALRF_IQK_KLOG},
	{"iqk_dbcc", HALRF_IQK_DBCC},
	{"iqk_rximr", HALRF_IQK_RXIMR},
	{"tssi", HALRF_TSSI},
	{"pwr_table", HALRF_PWR_TABLE},
	{"test", HALRF_RFK_TEST},
	{"txgapk", HALRF_TXGAPK},
	{"iqk_fw", HALRF_FW_IQK},
	{"dump", HALRF_DUMP},
	{"ther", HALRF_THER},
	{"xtal_trk", HALRF_XTAL_TRK},
	{"hwtx", HALRF_HWTX},
	{"kfree", HALRF_KFREE},
	{"chl_rfk", HALRF_CHL_RFK},
	{"op5k", HALRF_OP5K},
	{"rfk", HALRF_RFK},
};

void halrf_cmd_parser(struct rf_info *rf, char input[][RF_MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{

	struct rf_dbg_cmd_info	*rf_dbg_cmd = &rf->rf_dbg_cmd_i;

	u32 used = 0;
	u8 id = 0;
	u32 i, val_1 = 0, val_2 = 0;
	u32 halrf_ary_size = rf_dbg_cmd->cmd_size;

	RF_DBG_CNSL(out_len, used, output + used, out_len - used, "\n");

	/* Parsing Cmd ID */
	if (input_num) {
		for (i = 0; i < halrf_ary_size; i++) {
			if (_os_strcmp(halrf_cmd_i[i].name, input[0]) == 0) {
				id = halrf_cmd_i[i].id;
				break;
			}
		}
		if (i == halrf_ary_size) {
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "HALRF command not found!\n");
			return;
		}
	}

	switch (id) {
	case HALRF_HELP: {
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "RF cmd ==>\n");

		for (i = 0; i < halrf_ary_size - 1; i++)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "  %-5d: %s\n", i, halrf_cmd_i[i + 1].name);
		}
		break;
	case HALRF_SUPPORTABILITY:
		halrf_support_ability(rf, input, &used, output, &out_len);
		break;		
	case HALRF_DBG_COMP:
		halrf_dbg_trace(rf, input, &used, output, &out_len);
		break;
	case HALRF_DUMP_RFK_REG:
		halrf_dump_rfk_reg(rf, input, &used, output, &out_len);
		break;
	case HALRF_IQK:
		halrf_iqk_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_DPK:
		halrf_dpk_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_DPK_TRACK:
		halrf_dpk_track_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_DACK:		
		halrf_dack_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_RX_DCK:		
		halrf_rx_dck_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_LO_TEST:
		_os_sscanf(input[1], "%d", &val_1);
		_os_sscanf(input[2], "%d", &val_2);
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
				 "S%d LO test %s!!\n", val_2, val_1 ? "on" : "off");
		halrf_lo_test(rf, (bool)val_1, val_2);
		break;
	case HALRF_RCK:
		halrf_rck_trigger(rf, HW_PHY_0);
		break;
	case HALRF_WATCHDOG:		
		_os_sscanf(input[1], "%d", &val_1);
		rf->is_watchdog_stop = (bool)val_1;
		if (val_1 == true)
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			"Halrf watchdog STOP!!\n");
		else
			RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			"Halrf watchdog GO!!\n");
		break;
	case HALRF_SINGLETONE_EN:		
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[IQK]single tone enable => 0: path 0; 1: path 1\n"); 	
		_os_sscanf(input[1], "%d", &val_1);
		halrf_iqk_toneleakage(rf, val_1 & 0x1);
		break;
	case HALRF_IQK_BYPASS:
		halrf_iqk_bypass_cmd(rf, input, &used, output, &out_len);
		break;	
	case HALRF_IQK_KLOG:
		halrf_iqk_klog_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_TSSI:
		halrf_tssi_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_PWR_TABLE:
		halrf_pwr_table_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_IQK_DBCC:		
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[IQK]dbcc enable => 0: PHY0; 1: PHY1\n"); 	
		_os_sscanf(input[1], "%d", &val_1);
		halrf_iqk_dbcc(rf, val_1 & 0x1);
		break;
	case HALRF_RFK_CHECK:
		halrf_rfk_check_reg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_RFK_TEST:
		halrf_test_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_TXGAPK:
		halrf_txgapk_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_FW_IQK:
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[IQK] IQK TYPE => 0: DRV,  1: FW\n"); 	
		_os_sscanf(input[1], "%d", &val_1);
		halrf_enable_fw_iqk(rf, val_1 & 0x1);
		break;
	case HALRF_DUMP:
		halrf_dump_rf_reg_cmd(rf, input, &used, output, &out_len);
		break;	
	case HALRF_IQK_RXIMR:		
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "[IQK] -path  -idx \n"); 	
		_os_sscanf(input[1], "%d", &val_1);		
		_os_sscanf(input[2], "%d", &val_2);
		i= halrf_iqk_get_rximr(rf, (u8)val_1, val_2);		
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			" dec, rximr = %d !!\n", i);
		break;
	case HALRF_THER:
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Thermal A: %d\n", halrf_get_thermal(rf, RF_PATH_A));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Thermal B: %d\n", halrf_get_thermal(rf, RF_PATH_B)); 
		break;
	case HALRF_XTAL_TRK:
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Xtal Tracking offset PHY0 : %d\n",
			 halrf_xtal_tracking_offset(rf, 0));
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Xtal Tracking offset PHY1 : %d\n",
			 halrf_xtal_tracking_offset(rf, 1)); 
		break;
	case HALRF_HWTX:
		halrf_hwtx_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_KFREE:
		halrf_kfree_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_CHL_RFK:
		halrf_chl_rfk_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_OP5K:
		halrf_op5k_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	case HALRF_RFK:
		halrf_rfk_dbg_cmd(rf, input, &used, output, &out_len);
		break;
	default:
		RF_DBG_CNSL(out_len, used, output + used, out_len - used,
			 "Do not support this command\n");
		break;
	}

}

void halrf_cmd_parser_init(struct rf_info *rf)
{
	struct rf_dbg_cmd_info	*rf_dbg_cmd = &rf->rf_dbg_cmd_i;

	rf_dbg_cmd->cmd_size = sizeof(halrf_cmd_i) / sizeof(struct halrf_cmd_info);

}
#if 0
#ifndef strsep
char *strsep(char **s, const char *ct)
{
	char *sbegin = *s;
	char tmp = 0;
	char *end = &tmp;

	if (!sbegin)
		return NULL;

	//end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return sbegin;
}
#endif
#endif

s32 halrf_cmd(struct rf_info *rf, char *input, char *output, u32 out_len)
{
	char *token;
	u32 argc = 0;
	char argv[RF_MAX_ARGC][RF_MAX_ARGV];

	do {
		token = _os_strsep(&input, ", "); //smae name in bb
		if (token) {
			if (_os_strlen(token) <= RF_MAX_ARGV)
				_os_strcpy(argv[argc], token);

			argc++;
		} else {
			break;
		}
	} while (argc < RF_MAX_ARGC);

	if (argc == 1)
		argv[0][_os_strlen(argv[0]) - 1] = '\0';

	halrf_cmd_parser(rf, argv, argc, output, out_len);
	return 0;
}

u32 halrf_get_multiple(u8 pow, u8 base)
{
	u8 i;
	u32 return_value = 1;

	for (i = 0; i < pow; i++)
		return_value *= base; /*base ^ pow*/

	return	return_value;
}

u32 halrf_str_2_dec(u8 val)
{
	if (val >= 0x30 && val <= 0x39) /*0~9*/
		return	(val - 0x30);
	else if (val >= 0x41 && val <= 0x46) /*A~F*/
		return	(val - 0x41 + 10);
	else if (val >= 0x61 && val <= 0x66) /*a~f*/
		return	(val - 0x61 + 10);
	else
		return	1;
}

void halrf_scanf(char *in, enum rf_scanf_type type, u32 *out)
{
	char buff[HALRF_DCMD_SCAN_LIMIT];
	u32 multiple = 1;
	u8 text_num = 0;
	u8 base = 10;
	u8 i = 0, j = 0;

	*out = 0;

	for (i = 0; i < HALRF_DCMD_SCAN_LIMIT; i++) {
		if (in[i] != 0x0) {	/* 0x0 = NULL. */
			buff[i] = in[i];
			continue;
		}

		if (type == DCMD_CHAR) {
			*out = *in;
			break;
		}

		base =  (type == DCMD_DECIMAL) ? 10 : 16;
		text_num = i;

		for (j = 0; j < text_num; j++) {
			multiple = halrf_get_multiple(text_num - 1 - j, base);
			*out += halrf_str_2_dec(buff[j]) * multiple;
		}
		break;
	}
}

