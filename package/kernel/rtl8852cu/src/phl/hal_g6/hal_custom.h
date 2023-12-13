
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
#ifndef _HAL_CUSTOM_H_
#define _HAL_CUSTOM_H_

#ifdef CONFIG_PHL_CUSTOM_FEATURE
enum rtw_hal_status
rtw_hal_custom_cfg_tx_ampdu(void *hal,
                            struct rtw_wifi_role_link_t *rlink,
                            struct rtw_phl_custom_ampdu_cfg *ampdu);

enum rtw_hal_status
rtw_hal_get_ampdu_cfg(void *hal,
                      struct rtw_wifi_role_link_t *rlink,
                      struct rtw_phl_custom_ampdu_cfg *cfg);

enum rtw_hal_status
rtw_hal_get_edca(void *hal,
                 struct rtw_wifi_role_link_t *rlink,
                 struct rtw_edca_param *edca_param);

enum rtw_hal_status
rtw_hal_set_usr_edca(void *hal,
                     u32 idx,
                     u32 enable,
                     u32 band,
                     u32 wmm,
                     struct rtw_edca_param *aggr,
                     struct rtw_edca_param *mod);

enum rtw_hal_status
rtw_hal_set_pop_en(void *hal, bool en, enum phl_phy_idx phy_idx);

bool
rtw_hal_query_pop_en(void *hal, enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_set_pkt_detect_thold(void *hal, enum phl_band_idx band_idx, u32 bound);

u8
rtw_hal_query_pkt_detect_thold(void *hal,
                               bool get_en_info,
                               enum phl_phy_idx phy_idx);
#endif

#endif /*_HAL_CUSTOM_H_*/
