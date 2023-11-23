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
#ifndef _HALRF_INIT_H_
#define _HALRF_INIT_H_
#include "../../hal_headers_le.h"


 /*@--------------------------[Define] ---------------------------------------*/

 /*@--------------------------[Enum]------------------------------------------*/
 
 /*@--------------------------[Structure]-------------------------------------*/
 
 /*@--------------------------[Prptotype]-------------------------------------*/
struct rf_info;
void halrf_cmn_info_self_init(struct rf_info *rf);
void halrf_rfability_init_mp(struct rf_info *rf);
void halrf_rfability_init(struct rf_info *rf);
void halrf_set_rfability(struct rf_info *rf);
#endif
