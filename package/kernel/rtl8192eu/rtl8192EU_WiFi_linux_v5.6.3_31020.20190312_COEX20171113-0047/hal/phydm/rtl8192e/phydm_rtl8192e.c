/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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

/* ************************************************************
 * include files
 * ************************************************************ */

#include "mp_precomp.h"
#include "../phydm_precomp.h"
#if (RTL8192E_SUPPORT == 1)
s8 phydm_cck_rssi_8192e(struct dm_struct *dm, u16 lna_idx, u8 vga_idx)
{
	s8 rx_pwr_all = 0;
	s8 lna_gain = 0;
	s8 lna_gain_table_0[8] = {15, 9, -10, -21, -23, -27, -43, -44};
	s8 lna_gain_table_1[8] = {24, 18, 13, -4, -11, -18, -31, -36};

	if (dm->cck_agc_report_type == 0)
		lna_gain = lna_gain_table_0[lna_idx];
	else
		lna_gain = lna_gain_table_1[lna_idx];

	rx_pwr_all = lna_gain - (2 * vga_idx);

	return rx_pwr_all;
}
#endif

