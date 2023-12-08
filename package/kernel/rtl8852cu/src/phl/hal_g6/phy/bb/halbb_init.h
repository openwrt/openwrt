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
#ifndef _HALBB_INIT_H_
#define _HALBB_INIT_H_
#include "../../hal_headers_le.h"


 /*@--------------------------[Define] ---------------------------------------*/

 /*@--------------------------[Enum]------------------------------------------*/
 
 /*@--------------------------[Structure]-------------------------------------*/
 
 /*@--------------------------[Prptotype]-------------------------------------*/
void halbb_cmn_info_self_init_per_phy(struct bb_info *bb);
void halbb_timer_ctrl(struct bb_info *bb, enum bb_timer_cfg_t timer_state);
void halbb_timer_init(struct bb_info *bb);
void halbb_cr_cfg_init(struct bb_info *bb);
enum rtw_hal_status halbb_dm_init_per_phy(struct bb_info *bb_0, enum phl_phy_idx phy_idx);
void halbb_hw_init(struct bb_info *bb);
#endif