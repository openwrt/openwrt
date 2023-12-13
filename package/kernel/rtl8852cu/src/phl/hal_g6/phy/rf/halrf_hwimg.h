/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
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
#ifndef _HALRF_HWIMG_H_
#define _HALRF_HWIMG_H_

#define RADIO_TO_FW_PAGE_SIZE 6
#define RADIO_TO_FW_DATA_SIZE 500

struct halrf_radio_info {
	u32 write_times_a;
	u32 write_times_b;
	u32 radio_a_parameter[RADIO_TO_FW_PAGE_SIZE][RADIO_TO_FW_DATA_SIZE];
	u32 radio_b_parameter[RADIO_TO_FW_PAGE_SIZE][RADIO_TO_FW_DATA_SIZE];
};

#endif /*  _HALRF_HW_IMG_8852A_H_ */
