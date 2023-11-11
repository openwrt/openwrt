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
#ifndef _HALBTC_DBG_CMD_H_
#define _HALBTC_DBG_CMD_H_

void halbtc_cmd_parser(struct btc_t *btc, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len);
s32 halbtc_cmd(struct btc_t *btc, char *input, char *output, u32 out_len);
void _get_wl_nhm_dbm(struct btc_t *btc);
void _get_wl_cn_report(struct btc_t *btc);
void _get_wl_evm_report(struct btc_t *btc);
void _bt_psd_setup(struct btc_t *btc, u8 start_idx, u8 auto_rpt_type);
void _bt_psd_update(struct btc_t *btc, u8 *buf, u32 len);





#endif
