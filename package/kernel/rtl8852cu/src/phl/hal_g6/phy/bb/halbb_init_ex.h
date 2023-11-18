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
#ifndef _HALBB_INIT_EX_H_
#define _HALBB_INIT_EX_H_

 /*@--------------------------[Define] ---------------------------------------*/

 /*@--------------------------[Enum]------------------------------------------*/
 
 /*@--------------------------[Structure]-------------------------------------*/
 
 /*@--------------------------[Prptotype]-------------------------------------*/
void halbb_bb_pre_init(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_bb_post_init(struct bb_info *bb, enum phl_phy_idx phy_idx);
void halbb_dm_deinit(struct rtw_phl_com_t *phl_com, void *bb_phy_0);
void halbb_dm_init_phy1(struct bb_info *bb_0);
enum rtw_hal_status halbb_dm_init(struct bb_info *bb, enum phl_phy_idx phy_idx);

void halbb_buffer_deinit(struct rtw_phl_com_t *phl_com,
			 struct rtw_hal_com_t *hal_com, void *bb_phy_0);
u32 halbb_buffer_init(struct rtw_phl_com_t *phl_com,
		      struct rtw_hal_com_t *hal_com, void **bb_out_addr);

void halbb_get_efuse_init(struct bb_info *bb);

void halbb_ic_hw_setting_init(struct bb_info *bb);

#endif
