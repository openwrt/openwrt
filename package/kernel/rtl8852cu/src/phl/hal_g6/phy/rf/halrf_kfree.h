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
#ifndef _HALRF_KFREE_H_
#define _HALRF_KFREE_H_

#if 0
#define RX_GAIN_K_PATH_MAX 2
#define RX_GAIN_K_OFFSET_MAX 5
#define RX_GAIN_K_HIDE_OFFSET_MAX 4
#endif

enum BITMASK {
	LOW_MASK = 0,
	HIGH_MASK = 1
};

struct halrf_kfree_info{
	u8 efuse_content[100];

#if 0
	bool efuse_chenk;
	bool hide_efuse_chenk;
	s8 rx_gain_offset[RX_GAIN_K_PATH_MAX][RX_GAIN_K_OFFSET_MAX];
	s8 rx_gain_cs[RX_GAIN_K_PATH_MAX][RX_GAIN_K_HIDE_OFFSET_MAX];
	s8 rx_gain_cg[RX_GAIN_K_PATH_MAX][RX_GAIN_K_HIDE_OFFSET_MAX];

	s8 rx_lna_err_2g[RX_GAIN_K_PATH_MAX][7];
	s8 rx_lna_err_5g[RX_GAIN_K_PATH_MAX][7];

	s8 rx_frontend_loss[RX_GAIN_K_PATH_MAX];
	s8 rx_rpl_bias_comp[RX_GAIN_K_PATH_MAX];
	s8 rx_rssi_bias_comp[RX_GAIN_K_PATH_MAX];
#endif
};

#endif	/*_HALRF_SET_PWR_TABLE_H_*/
