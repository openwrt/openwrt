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
#ifndef _RTL8852CU_HAL_H_
#define _RTL8852CU_HAL_H_

/* rtl8852cu_halinit.c */
#ifdef CONFIG_WOWLAN
enum rtw_hal_status hal_wow_init_8852cu(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal, struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status hal_wow_deinit_8852cu(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal, struct rtw_phl_stainfo_t *sta);
#endif /* CONFIG_WOWLAN */

enum rtw_hal_status hal_mp_init_8852cu(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal);
enum rtw_hal_status hal_mp_deinit_8852cu(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal);
bool hal_mp_path_chk_8852cu(struct rtw_phl_com_t *phl_com, u8 ant_tx, u8 cur_phy);

enum rtw_hal_status hal_get_efuse_8852cu(struct rtw_phl_com_t *phl_com,
					 struct hal_info_t *hal);

enum rtw_hal_status hal_init_8852cu(struct rtw_phl_com_t *phl_com,
				    struct hal_info_t *hal);
void hal_deinit_8852cu(struct rtw_phl_com_t *phl_com,
		       struct hal_info_t *hal);

enum rtw_hal_status hal_start_8852cu(struct rtw_phl_com_t *phl_com,
				     struct hal_info_t *hal);
enum rtw_hal_status hal_stop_8852cu(struct rtw_phl_com_t *phl_com,
				    struct hal_info_t *hal);

u32 hal_hci_cfg_8852cu(struct rtw_phl_com_t *phl_com,
		struct hal_info_t *hal, struct rtw_ic_info *ic_info);

void init_hal_spec_8852cu(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal);

bool hal_recognize_int_8852cu(struct hal_info_t *hal);
u32 hal_int_hdler_8852cu(struct hal_info_t *hal);

#endif /* _RTL8852CU_HAL_H_ */
