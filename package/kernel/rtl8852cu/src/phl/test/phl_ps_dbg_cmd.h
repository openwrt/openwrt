/******************************************************************************
 *
 * Copyright(c) 2019 - 2022 Realtek Corporation.
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
#ifndef _PHL_PS_DBG_CMD_H_
#define _PHL_PS_DBG_CMD_H_
#ifdef CONFIG_POWER_SAVE
#define PS_CNSL(max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
	do {								\
		u32 *used_len_tmp = &(used_len);			\
		if (*used_len_tmp < max_buff_len)			\
			*used_len_tmp += _os_snprintf(buff_addr, remain_len, fmt, ##__VA_ARGS__);\
	} while (0)

void phl_ps_cmd_parser(struct phl_info_t *phl_info, char input[][MAX_ARGV],
			u32 input_num, char *output, u32 out_len);
void phl_ps_dbg_dump(struct phl_info_t *phl_info, u32 *used,
		char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len);

void phl_ps_dbg_stop_ps(struct phl_info_t *phl_info, u32 *used,
		char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len);
void phl_ps_dbg_force_ps(struct phl_info_t *phl_info, u32 *used,
		char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len);
void rtw_phl_dbg_ps_op_mode(void *phl, u8 band_idx, u8 ps_mode, u8 ps_op_mode);
void phl_ps_dbg_ps_op_mode(struct phl_info_t *phl_info, u32 *used,
		char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len);
void rtw_phl_dbg_ps_cap(void *phl, u8 band_idx, u8 ps_mode, u8 ps_cap);
void phl_ps_dbg_ps_cap(struct phl_info_t *phl_info, u32 *used,
		char input[][MAX_ARGV], u32 input_num, char *output, u32 out_len);
#endif
#endif /* _PHL_PS_DBG_CMD_H_ */

