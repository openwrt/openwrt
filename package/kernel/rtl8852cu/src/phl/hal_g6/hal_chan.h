/******************************************************************************
 *
 * Copyright(c)2019 Realtek Corporation.
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
#ifndef _HAL_CHAN_H_
#define _HAL_CHAN_H_

enum rtw_hal_status rtw_hal_set_ch_bw(void *hal, u8 band_idx,
		struct rtw_chan_def *chdef, bool do_rfk, bool rd_enabled, bool frc_switch);

void rtw_hal_get_cur_chdef(void *hal, u8 band_idx,
				struct rtw_chan_def *cur_chandef);

#ifdef DBG_PHL_CHAN
void phl_chan_dump_chandef(const char *caller, const int line, bool show_caller,
						struct rtw_chan_def *chandef);
#define PHL_DUMP_CHAN_DEF(_chandef) phl_chan_dump_chandef(__FUNCTION__, __LINE__, false, _chandef);
#define PHL_DUMP_CHAN_DEF_EX(_chandef) phl_chan_dump_chandef(__FUNCTION__, __LINE__, true, _chandef);
#else
#define PHL_DUMP_CHAN_DEF(_chandef)
#define PHL_DUMP_CHAN_DEF_EX(_chandef)
#endif

#endif /* _HAL_CHAN_H_ */

