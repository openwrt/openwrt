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
#ifndef _HALBB_RA_EX_H_
#define _HALBB_RA_EX_H_
/*@--------------------------[Define] ---------------------------------------*/



/*@--------------------------[Enum]------------------------------------------*/



/*@--------------------------[Structure]-------------------------------------*/


/*@--------------------------[Prptotype]-------------------------------------*/


extern bool rtw_halbb_raregistered(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
extern bool rtw_halbb_raupdate(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
extern bool rtw_halbb_dft_mask(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
extern bool rtw_halbb_ra_deregistered(struct bb_info *bb, struct rtw_phl_stainfo_t *phl_sta_i);
extern bool rtw_halbb_query_txsts(struct bb_info *bb, u16 macid0, u16 macid1);
void halbb_drv_cmac_rpt_parsing(struct bb_info *bb, u8 *rpt);
void halbb_ra_shift_darf_tc(struct bb_info *bb, bool enable, u8 *init_fb_cnt);
#endif
