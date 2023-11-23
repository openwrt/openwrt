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
#ifndef _HAL_API_BTC_H_
#define _HAL_API_BTC_H_

#ifdef CONFIG_BTCOEX
enum rtw_hal_status
rtw_hal_btc_init(struct rtw_phl_com_t *phl_com,
			struct hal_info_t *hal_info);

void rtw_hal_btc_deinit(struct rtw_phl_com_t *phl_com,
				struct hal_info_t *hal_info);
#else
#define rtw_hal_btc_init(_phl_com_, _hal_info_) RTW_HAL_STATUS_SUCCESS
#define rtw_hal_btc_deinit(_phl_com_, _hal_info_)
#endif

void rtw_hal_btc_switch_band_ntfy(void *hinfo, enum phl_phy_idx phy_idx,
				  enum band_type band);
void rtw_hal_btc_radio_state_ntfy(void *hinfo, u8 rf_state);

u32 rtw_hal_btc_process_c2h(void *hal, struct rtw_c2h_info *c2h, struct c2h_evt_msg *c2h_msg);
void rtw_hal_btc_init_coex_cfg_ntfy(void *hinfo);
bool rtw_hal_btc_proc_cmd(struct hal_info_t *hal_info, struct rtw_proc_cmd *incmd,
						char *output, u32 out_len);
u32 rtw_hal_btc_req_bt_slot_t(void *hinfo, enum phl_band_idx hw_band);

#endif /*_HAL_API_BTC_H_*/
