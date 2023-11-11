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
#ifndef _HALRF_TSSI_8852C_H_
#define _HALRF_TSSI_8852C_H_
#ifdef RF_8852C_SUPPORT

#define TSSI_VER_8852C 0x1d

#define TSSI_PATH_MAX_8852C 2

/*@--------------------------Define Parameters-------------------------------*/


/*@-----------------------End Define Parameters-----------------------*/
void halrf_tssi_get_efuse_8852c(struct rf_info *rf,
						enum phl_phy_idx phy);
bool halrf_tssi_check_efuse_data_8852c(struct rf_info *rf,
						enum phl_phy_idx phy);
void halrf_set_tssi_de_for_tx_verify_8852c(struct rf_info *rf,
				enum phl_phy_idx phy, u32 tssi_de, u8 path);
void halrf_set_tssi_de_offset_8852c(struct rf_info *rf,
				enum phl_phy_idx phy, u32 tssi_de_offset, u8 path);
void halrf_set_tssi_de_offset_zero_8852c(struct rf_info *rf,
							enum phl_phy_idx phy);
void halrf_do_tssi_8852c(struct rf_info *rf, enum phl_phy_idx phy, bool hwtx_en);
void halrf_do_tssi_scan_8852c(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_tssi_enable_8852c(struct rf_info *rf, enum phl_phy_idx phy);
void halrf_tssi_disable_8852c(struct rf_info *rf, enum phl_phy_idx phy);
s32 halrf_get_online_tssi_de_8852c(struct rf_info *rf, enum phl_phy_idx phy_idx,
				u8 path, s32 dbm, s32 puot);
void halrf_tssi_cck_8852c(struct rf_info *rf, enum phl_phy_idx phy,
				bool is_cck);
void halrf_set_tssi_avg_mp_8852c(struct rf_info *rf,
					enum phl_phy_idx phy_idx, s32 xdbm);
void halrf_tssi_set_efuse_to_de_8852c(struct rf_info *rf,
						enum phl_phy_idx phy);
void halrf_tssi_set_efuse_to_slope_8852c(struct rf_info *rf,
						enum phl_phy_idx phy);
void halrf_tssi_default_txagc_8852c(struct rf_info *rf,
					enum phl_phy_idx phy, bool enable);

void halrf_tssi_scan_ch_setting_8852c(struct rf_info *rf,
	enum phl_phy_idx phy, enum rf_path path);

void halrf_tssi_scan_ch_8852c(struct rf_info *rf, enum rf_path path);

void halrf_tssi_backup_txagc_8852c(struct rf_info *rf, enum phl_phy_idx phy, bool enable);

u32 halrf_tssi_get_final_8852c(struct rf_info *rf, enum rf_path path);

void halrf_tssi_hw_tx_8852c(struct rf_info *rf,
			enum phl_phy_idx phy, u8 path, u16 cnt, s16 dbm, u32 rate, u8 bw,
			bool enable);

void halrf_tssi_ant_open_8852c(struct rf_info *rf);

void halrf_tssi_slope_onoff_8852c(struct rf_info *rf, u8 tssi_slope_type);

void halrf_get_tssi_info_8852c(struct rf_info *rf,
		char input[][16], u32 *_used, char *output, u32 *_out_len);

void halrf_tssi_do_slope_8852c(struct rf_info *rf);

u32 halrf_tssi_get_cw_8852c(struct rf_info *rf, enum rf_path path);

void halrf_tssi_slope_apply_8852c(struct rf_info *rf,
	u32 gain_first, u32 gain_second,
	u32 cw_first, u32 cw_second, enum rf_path path,
	u32* gain_diff, u32* cw_diff);

void halrf_tssi_current_slope_apply_8852c(struct rf_info *rf,
	u32 gain_diff, u32 cw_diff, enum rf_path path);

void halrf_tssi_finish_slope_8852c(struct rf_info *rf);

#endif
#endif	/*_HALRF_SET_PWR_TABLE_8852C_H_*/