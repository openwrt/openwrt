/******************************************************************************
 *
 * Copyright(c)2019 Realtek Corporation.
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
#ifndef _PHL_P2PPS_DEF_H_
#define _PHL_P2PPS_DEF_H_
#ifdef RTW_WKARD_P2PPS_REFINE
#ifdef CONFIG_PHL_P2PPS

struct rtw_phl_noa_info {
	u8 en_desc_num;
	u8 paused;
	struct rtw_phl_noa_desc noa_desc[MAX_NOA_DESC];
	struct rtw_phl_opps_desc opps_desc;
};

struct rtw_phl_p2pps_info {
	struct phl_info_t *phl_info;
	struct rtw_phl_p2pps_ops ops;
	_os_lock p2pps_lock;
	struct rtw_phl_noa_info noa_info[MAX_WIFI_ROLE_NUMBER];
};
#endif
#endif
#endif
