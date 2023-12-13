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
#ifndef _HALBB_XBOX_RA_EX_H_
#define _HALBB_XBOX_RA_EX_H_
#ifdef BB_8852C_SUPPORT
/*@--------------------------[Define] ---------------------------------------*/



/*@--------------------------[Enum]------------------------------------------*/



/*@--------------------------[Structure]-------------------------------------*/
struct rtw_xpltfm_mcs_table;

/*@--------------------------[Prptotype]-------------------------------------*/

void halbb_upt_mcs_table(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i, struct rtw_xpltfm_mcs_table *mcs_table);
u8 halbb_arfr_table(struct bb_info *bb, u16 init_rate, u8 switch_table);
#endif
#endif
