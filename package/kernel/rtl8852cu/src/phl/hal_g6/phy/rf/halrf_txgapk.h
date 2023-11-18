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
#ifndef _HALRF_TXGAPK_H_
#define _HALRF_TXGAPK_H_
#include "halrf_headers.h"

#define NUM		4

enum txgapk_id {
	TXGAPK_TRACK	= 0x00,
	TXGAPK_PWR		= 0x01,
	TXGAPK_IQKBK    = 0x02
};


struct halrf_gapk_info {

	bool is_gapk_init;
	u32 gapk_rf3f_bp[5][12][NUM]; /* band(2Gcck/2GOFDM/5GL/5GM/5GH)/idx/path */
	bool gapk_bp_done;
	s8 offset[12][NUM];
	s8 fianl_offset[12][NUM];
	u8 read_txgain;
	bool is_gapk_enable;
	u8 band[NUM];
	u8 ch[NUM];
	u8 bw[NUM];	
	s32 track_d[2][17];
	s32 track_ta[2][17];
	s32 power_d[2][17];
	s32 power_ta[2][17];
	u32 txgapk_time;

	bool	is_txgapk_ok;
	u32		failcode;
	u32     r0x8010[2]; /* before and after txgapk */
	u32     txgapk_chk_cnt[2][3][2]; /* path */ /* track pwr iqkbk*/ /* 0xbff8 0x80fc*/

	/* u8 txgapk_mcc_ch[2][NUM]; */
	/* u8 txgapk_table_idx[NUM]; */
	u8 txgapk_mcc_ch[2]; /* channel */
	u8 txgapk_table_idx;
};

#endif
