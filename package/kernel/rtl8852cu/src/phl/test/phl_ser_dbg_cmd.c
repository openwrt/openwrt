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
#define _PHL_SER_DBG_CMD_C_
#include "../phl_headers.h"

struct phl_ser_cmd_info {
	char name[16];
	u8 id;
};

enum PHL_SER_CMD_ID {
	PHL_SER_STATE,
	PHL_SER_CMAC,
	PHL_SER_DMAC,
};

struct phl_ser_cmd_info phl_ser_cmd_i[] = {
	{"state", PHL_SER_STATE},
	{"cmac", PHL_SER_CMAC},
	{"dmac", PHL_SER_DMAC}
};

/* echo phl ser state */
void _phl_ser_cmd_state(struct phl_info_t *phl_info, u32 *used, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len)
{
	struct rtw_stats *ser_stat = &phl_info->phl_com->phl_stats;

	PHL_DBG_MON_INFO(out_len, *used, output + *used,
					 out_len - *used, "\n[SER statistic]\
					 \nRTW_PHL_SER_L0_RESET = %d\
					 \nRTW_PHL_SER_PAUSE_TRX (M1) = %d\
					 \nRTW_PHL_SER_DO_RECOVERY (M3) = %d\
					 \nRTW_PHL_SER_READY (M5) = %d\
					 \nRTW_PHL_SER_L2_RESET = %d\
					 \nRTW_PHL_SER_EVENT_CHK = %d\
					 \nRTW_PHL_SER_DUMP_FW_LOG = %d\
					 \nRTW_PHL_SER_LOG_ONLY = %d\n",
	(int)ser_stat->ser_event[RTW_PHL_SER_L0_RESET],
	(int)ser_stat->ser_event[RTW_PHL_SER_PAUSE_TRX],
	(int)ser_stat->ser_event[RTW_PHL_SER_DO_RECOVERY],
	(int)ser_stat->ser_event[RTW_PHL_SER_READY],
	(int)ser_stat->ser_event[RTW_PHL_SER_L2_RESET],
	(int)ser_stat->ser_event[RTW_PHL_SER_EVENT_CHK],
	(int)ser_stat->ser_event[RTW_PHL_SER_DUMP_FW_LOG],
	(int)ser_stat->ser_event[RTW_PHL_SER_LOG_ONLY]);

}

/* echo phl ser cmac */
void _phl_ser_cmd_cmac(struct phl_info_t *phl_info, u32 *used, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len )
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)(phl_info->hal);

	status = rtw_hal_trigger_cmac_err(hal_info);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
			"\nSER CMAC Reset (status = 0x%x) \n", (int)status);
}

/* echo phl ser dmac */
void _phl_ser_cmd_dmac(struct phl_info_t *phl_info, u32 *used, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len )
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)(phl_info->hal);

	status = rtw_hal_trigger_dmac_err(hal_info);
	PHL_DBG_MON_INFO(out_len, *used, output + *used, out_len - *used,
			"\nSER DMAC Reset (status = 0x%x) \n", (int)status);
}

void phl_ser_cmd_parser(struct phl_info_t *phl_info, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len)
{
	u32 used = 0;
	u8 id = 0;
	u32 i;
	u32 array_size = sizeof(phl_ser_cmd_i) / sizeof(struct phl_ser_cmd_info);

	PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used, "\n");

	/* Parsing cmd ID */
	if (input_num) {
		for (i = 0; i < array_size; i++) {
			if (!_os_strcmp(phl_ser_cmd_i[i].name, input[1])) {
				id = phl_ser_cmd_i[i].id;
				break;
			}
		}
	}

	switch (id) {
	case PHL_SER_STATE:
		_phl_ser_cmd_state(phl_info, &used, input, input_num,
					output, out_len);
		break;
	case PHL_SER_CMAC:
		_phl_ser_cmd_cmac(phl_info, &used, input, input_num,
					output, out_len);
		break;
	case PHL_SER_DMAC:
		_phl_ser_cmd_dmac(phl_info, &used, input, input_num,
					output, out_len);
		break;
	default:
		PHL_DBG_MON_INFO(out_len, used, output + used, out_len - used,
			 "command not supported !!\n");
		break;
	}
}
