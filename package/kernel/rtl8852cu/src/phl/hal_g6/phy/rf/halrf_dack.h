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
#ifndef _HALRF_DACK_H_
#define _HALRF_DACK_H_

/*@--------------------------Define Parameters-------------------------------*/


/*@-----------------------End Define Parameters-----------------------*/

struct halrf_dack_info {
	bool dack_done;
	u8 msbk_d[2][2][16];
	u8 dadck_d[2][2];		/*path/IQ*/
	u16 addck_d[2][2];	/*path/IQ*/
	u16 biask_d[2][2];		/*path/IQ*/
	u32 dack_cnt;
	u32 dack_time;
	bool addck_timeout[2];
	bool dadck_timeout[2];
	bool msbk_timeout[2];
	bool dack_fail;
};


#endif