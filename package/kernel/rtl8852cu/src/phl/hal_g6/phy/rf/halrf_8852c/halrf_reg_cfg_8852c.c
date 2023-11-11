/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

#include "../halrf_precomp.h"
#ifdef RF_8852C_SUPPORT

void halrf_cfg_rf_radio_a_8852c(struct rf_info *rf, u32 addr, u32 data)
{
	/*laod radio a table*/
	halrf_wrf(rf, RF_PATH_A, addr, MASKRF, data);
	RF_DBG(rf, DBG_RF_INIT, "[RF_a] %08X %08X\n", addr, data);
}

void halrf_cfg_rf_radio_b_8852c(struct rf_info *rf, u32 addr, u32 data)
{
	/*laod radio b table*/
	halrf_wrf(rf, RF_PATH_B, addr, MASKRF, data);
	RF_DBG(rf, DBG_RF_INIT, "[RF_b] %08X %08X\n", addr, data);
}

void halrf_cfg_rf_nctl_8852c(struct rf_info *rf, u32 addr, u32 mask, u32 data)
{
/*laod NCTL table*/
	if (addr == 0xfe)
		halrf_delay_ms(rf, 50);
	else if (addr == 0xfd)
		halrf_delay_ms(rf, 5);
	else if (addr == 0xfc)
		halrf_delay_ms(rf, 1);
	else if (addr == 0xfb)
		halrf_delay_us(rf, 50);
	else if (addr == 0xfa)
		halrf_delay_us(rf, 5);
	else if (addr == 0xf9)
		halrf_delay_us(rf, 1);
	else
		halrf_wreg(rf, addr, mask, data);
	
	RF_DBG(rf, DBG_RF_INIT, "[RFK] %08X %08X\n", addr, data);
}

#endif
