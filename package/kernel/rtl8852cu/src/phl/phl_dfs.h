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
#ifndef _PHL_DFS_H_
#define _PHL_DFS_H_

#ifdef CONFIG_PHL_DFS
bool phl_is_radar_detect_enabled(struct phl_info_t *phl_info, u8 band_idx);

bool phl_is_under_cac(struct phl_info_t *phl_info, u8 band_idx);

bool phl_is_cac_tx_paused(struct phl_info_t *phl_info, u8 band_idx);

struct dfs_rd_ch_switch_ctx {
	bool rd_enabled;
	bool should_rd_en_on_new_ch;
	bool under_cac;
	bool cac_tx_paused;
};

void phl_dfs_rd_setting_before_ch_switch(struct phl_info_t *phl_info, u8 band_idx
	, enum band_type band, u8 ch, enum channel_width bw, enum chan_offset offset, struct dfs_rd_ch_switch_ctx *ctx);

void phl_dfs_rd_setting_after_ch_switch(struct phl_info_t *phl_info, u8 band_idx
	, enum band_type band, u8 ch, enum channel_width bw, enum chan_offset offset, struct dfs_rd_ch_switch_ctx *ctx);

enum rtw_phl_status
phl_cmd_dfs_rd_ctl_hdl(struct phl_info_t *phl_info, u8 *param);
#endif

#endif /* _PHL_DFS_H_ */
