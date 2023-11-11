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
#ifndef _HALRF_INIT_EX_H_
#define _HALRF_INIT_EX_H_

 /*@--------------------------[Define] ---------------------------------------*/

 /*@--------------------------[Enum]------------------------------------------*/
 
 /*@--------------------------[Structure]-------------------------------------*/
 
 /*@--------------------------[Prptotype]-------------------------------------*/
#if 0
struct rf_info;
void halrf_cmn_info_self_init(struct rf_info *rf);
void halrf_rfability_init_mp(struct rf_info *rf);
void halrf_rfability_init(struct rf_info *rf);
void halrf_set_rfability(struct rf_info *rf);

enum rtw_hal_status  halrf_init(struct rtw_phl_com_t *phl_com,
			 struct rtw_hal_com_t *hal_com, void **rf_out);
void halrf_deinit(struct rtw_phl_com_t *phl_com,
			 struct rtw_hal_com_t *hal_com, void *rf);
#endif

#endif
