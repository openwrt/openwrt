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
#ifndef _HAL_SCANOFLD_H_
#define _HAL_SCANOFLD_H_

#ifdef CONFIG_PHL_SCANOFLD
void rtw_hal_scan_ofld_leave_cur_chnl(void *hal, u8 hw_band, u8 ch_band, u8 ch);
void rtw_hal_scan_ofld_cfg_en(void *hal, u8 en);
enum rtw_hal_status
rtw_hal_scan_ofld_add_ch(void *hal, u8 hw_band, struct scan_ofld_ch_info *cfg,
			 bool ofld);
enum rtw_hal_status
rtw_hal_scan_ofld(void *hal, u16 mac_id, u8 hw_band, u8 hw_port,
		  struct scan_ofld_info *cfg);
enum rtw_hal_status
rtw_hal_scan_ofld_fw_busy(void *hal, u8 hw_band);
#endif /* CONFIG_PHL_SCANOFLD */

#endif /* _HAL_SCANOFLD_H_ */

