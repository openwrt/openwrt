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
#ifndef _HAL_API_RF_H_
#define _HAL_API_RF_H_

enum halrf_thermal_status{
	HALRF_THERMAL_STATUS_BELOW_THRESHOLD,
	HALRF_THERMAL_STATUS_STAY_THRESHOLD,
	HALRF_THERMAL_STATUS_ABOVE_THRESHOLD,
	HALRF_THERMAL_STATUS_UNKNOWN
};

enum rtw_hal_status
rtw_hal_rf_init(struct rtw_phl_com_t *phl_com,
			struct hal_info_t *hal_info);

void rtw_hal_rf_deinit(struct rtw_phl_com_t *phl_com,
			struct hal_info_t *hal_info);

void rtw_hal_init_rf_reg(struct rtw_phl_com_t *phl_com, void *hal);

enum rtw_hal_status rtw_hal_rf_get_pwrtrack(struct hal_info_t *hal_info, u8 *txpwr_track_status, u8 phy_idx);

void rtw_hal_rf_dm_init(struct hal_info_t *hal_info);

enum rtw_hal_status rtw_hal_rf_set_pwrtrack(struct hal_info_t *hal_info, u8 phy_idx, u8 txpwr_track_status);

void rtw_hal_rf_dm_init(struct hal_info_t *hal_info);

enum rtw_hal_status rtw_hal_rf_get_thermal(struct hal_info_t *hal_info, u8 rf_path, u8 *thermal);

enum rtw_hal_status rtw_hal_rf_set_tssi(struct hal_info_t *hal_info, u8 phy_idx, u8 rf_path, u32 tssi_de);

enum rtw_hal_status rtw_hal_rf_get_tssi(struct hal_info_t *hal_info, u8 phy_idx, u8 rf_path, u32 *tssi);

enum rtw_hal_status rtw_hal_rf_get_online_tssi_de(struct hal_info_t *hal_info, u8 phy_idx, u8 rf_path,s32 dbm, s32 pout, s32 *tssi_de);

enum rtw_hal_status rtw_hal_rf_set_tssi_offset(struct hal_info_t *hal_info, u8 phy_idx, u32 tssi_de_offset, u8 rf_path);

enum rtw_hal_status rtw_hal_rf_set_continuous_tx(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_rf_set_singletone_tx(struct hal_info_t *hal_info,
							 u8 is_on,enum rf_path path);

enum rtw_hal_status rtw_hal_rf_set_suppression_tx(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_rf_chl_rfk_trigger(struct rtw_hal_com_t *hal_com,
                           u8 phy_idx,
                           u8 force);

enum rtw_hal_status rtw_hal_rf_dack_trigger(struct hal_info_t *hal_info,
			   u8 force);


enum rtw_hal_status rtw_hal_rf_iqk_trigger(struct hal_info_t *hal_info,
			   u8 phy_idx, u8 force);


enum rtw_hal_status rtw_hal_rf_lck_trigger(struct hal_info_t *hal_info);


enum rtw_hal_status rtw_hal_rf_dpk_trigger(struct hal_info_t *hal_info,
			   u8 phy_idx, u8 force);


enum rtw_hal_status rtw_hal_rf_tssi_trigger(struct hal_info_t *hal_info,
				u8 phy_idx);


enum rtw_hal_status rtw_hal_rf_gapk_trigger(struct hal_info_t *hal_info,
			   u8 phy_idx, u8 force);


enum rtw_hal_status rtw_hal_rf_set_capability_dack(struct hal_info_t *hal_info,
				u8 enable);


enum rtw_hal_status rtw_hal_rf_set_capability_iqk(struct hal_info_t *hal_info,
				u8 enable);


enum rtw_hal_status rtw_hal_rf_set_capability_dpk(struct hal_info_t *hal_info,
			   u8 enable);


enum rtw_hal_status rtw_hal_rf_set_capability_dpk_track(struct hal_info_t *hal_info,
			   u8 enable);



enum rtw_hal_status rtw_hal_rf_set_capability_tssi(struct hal_info_t *hal_info,
				u8 enable);


enum rtw_hal_status rtw_hal_rf_set_capability_gapk(struct hal_info_t *hal_info,
			   u8 enable);


enum rtw_hal_status rtw_hal_rf_get_capability_dack(struct hal_info_t *hal_info,
				u8 *enable);


enum rtw_hal_status rtw_hal_rf_get_capability_iqk(struct hal_info_t *hal_info,
				u8 *enable);


enum rtw_hal_status rtw_hal_rf_get_capability_dpk(struct hal_info_t *hal_info,
			   u8 *enable);


enum rtw_hal_status rtw_hal_rf_get_capability_dpk_track(struct hal_info_t *hal_info,
				u8 *enable);


enum rtw_hal_status rtw_hal_rf_get_capability_tssi(struct hal_info_t *hal_info,
				u8 *enable);


enum rtw_hal_status rtw_hal_rf_get_capability_gapk(struct hal_info_t *hal_info,
			   u8 *enable);

enum rtw_hal_status rtw_hal_rf_get_tssi_de_value(struct hal_info_t *hal_info);

enum rtw_hal_status rtw_hal_rf_set_tssi_de_tx_verify(struct hal_info_t *hal_info);

enum rtw_hal_status rtw_hal_rf_get_txpwr_final_abs(struct hal_info_t *hal_info);

int rtw_hal_rf_get_predefined_pw_lmt_regu_type_from_str(const char *str);
const char * const *rtw_hal_rf_get_predefined_pw_lmt_regu_type_str_array(u8 *num);

u8 rtw_hal_rf_get_pw_lmt_regu_type(struct hal_info_t *hal_info, enum band_type band);
const char *rtw_hal_rf_get_pw_lmt_regu_type_str(struct hal_info_t *hal_info, enum band_type band);

bool rtw_hal_rf_pw_lmt_regu_tbl_exist(struct hal_info_t *hal_info, enum band_type band, u8 regu);
int rtw_hal_rf_file_regd_ext_search(struct hal_info_t *hal_info, u16 domain_code, const char *country);

void rtw_hal_rf_auto_pw_lmt_regu(struct hal_info_t *hal_info);
void rtw_hal_rf_force_pw_lmt_regu(struct hal_info_t *hal_info,
	u8 regu_2g[], u8 regu_2g_len, u8 regu_5g[], u8 regu_5g_len, u8 regu_6g[], u8 regu_6g_len);

bool
rtw_hal_rf_proc_cmd(struct hal_info_t *hal_info,
					struct rtw_proc_cmd *incmd,
					char *output, u32 out_len);

enum rtw_hal_status
rtw_hal_rf_recover(struct hal_info_t *hal_info, u32 offset, u32 val, enum rf_path path);

enum rtw_hal_status rtw_hal_rf_watchdog(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_rf_set_power(struct hal_info_t *hal_info, enum phl_phy_idx phy,
				enum phl_pwr_table pwr_table);

enum rtw_hal_status
rtw_hal_rf_set_power_constraint(struct hal_info_t *hal_info, enum phl_phy_idx phy,
					u16 mb);

enum rtw_hal_status rtw_hal_rf_set_gain_offset(struct hal_info_t *hal_info, u8 cur_phy_idx,
						s8 offset, u8 rf_path);

enum rtw_hal_status rtw_hal_rf_trigger_dpk_tracking(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_rf_get_default_rfe_type(struct rtw_hal_com_t *hal_com);

enum rtw_hal_status
rtw_hal_rf_get_default_xtal(struct rtw_hal_com_t *hal_com);


enum rtw_hal_status rtw_hal_rf_set_tssi_avg(struct hal_info_t *hal_info, u8 cur_phy_idx,
						s32 xdbm);
void
rtw_hal_rf_do_tssi_scan(struct hal_info_t *hal_ionfo, u8 cur_phy_idx);

void
rtw_hal_rf_update_ext_pwr_lmt_table(struct hal_info_t *hal_info,
					      enum phl_phy_idx phy);

enum rtw_hal_status
rtw_hal_rf_config_radio_to_fw(struct hal_info_t *hal_info);

bool
rtw_hal_rf_check_efuse_data(struct rtw_hal_com_t *hal_com, enum phl_phy_idx phy_idx);

void
rtw_hal_rf_disconnect_notify(void *hal, struct rtw_chan_def *chandef);

bool
rtw_hal_rf_check_mcc_ch(void *hal, struct rtw_chan_def *chandef);

void
rtw_hal_rf_dpk_switch(void *hal, bool enable);

void
rtw_hal_rf_tssi_config(void *hal, enum phl_phy_idx phy_idx, bool enable);


enum rtw_hal_status
rtw_hal_rf_set_ch_bw(struct rtw_hal_com_t *hal_com,
                     enum phl_phy_idx phy,
                     u8 center_ch,
                     enum band_type band,
                     enum channel_width bw);

void
rtw_hal_rf_get_efuse_ex(struct rtw_hal_com_t *hal_com, enum phl_phy_idx phy_idx);


/* PSD */
enum rtw_hal_status rtw_hal_rf_psd_init(struct hal_info_t *hal_info, u8 cur_phy_idx,
					u8 path, u8 iq_path, u32 avg, u32 fft);

enum rtw_hal_status rtw_hal_rf_psd_restore(struct hal_info_t *hal_info, u8 cur_phy_idx);

enum rtw_hal_status rtw_hal_rf_psd_get_point_data(struct hal_info_t *hal_info, u8 cur_phy_idx,
					s32 point, u32 *value);

enum rtw_hal_status rtw_hal_rf_psd_query(struct hal_info_t *hal_info, u8 cur_phy_idx,
					u32 point, u32 start_point, u32 stop_point, u32 *outbuf);

enum rtw_hal_status rtw_hal_rf_bfer_cfg(struct hal_info_t *hal_info);

void rtw_hal_rf_rx_ant(struct hal_info_t *hal_info, /*enum halrf_ant*/ u8 ant);

enum halrf_thermal_status
rtw_hal_rf_get_ther_protected_threshold(
	struct hal_info_t *hal_info
);

void rtw_hal_rf_notification(struct hal_info_t *hal_info,
			     enum phl_msg_evt_id event,
			     enum phl_phy_idx phy_idx);

void rtw_hal_rf_cmd_notification(struct hal_info_t *hal_info,
                             void *hal_cmd,
                             enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_rf_syn_config(struct rtw_hal_com_t *hal_com,
                      u8 syn_id,
                      enum phl_phy_idx phy_idx,
                      u8 path,
                      bool turn_on);
void
rtw_hal_rf_set_mp_regulation(struct hal_info_t *hal_info,
		enum phl_phy_idx phy_idx, u8 regulation);

void rtw_hal_rf_test_event_trigger(struct hal_info_t *hal_info,
				enum phl_phy_idx phy_idx,
				u8 event,
				u8 func,
				u32 *buf);

void rtw_hal_rf_test_event_trigger(struct hal_info_t *hal_info,
				enum phl_phy_idx phy_idx,
				u8 event,
				u8 func,
				u32 *buf);

s8 rtw_hal_rf_get_power_limit(struct hal_info_t *hal_info,
	enum phl_phy_idx phy, u16 rate, u8 bandwidth,
	u8 beamforming, u8 tx_num, u8 channel);

void rtw_hal_rf_rfe_ant_num_chk(struct rtw_hal_com_t *hal_com);

u8 rtw_hal_rf_get_tx_tbl_to_pwr_times(struct hal_info_t *hal_info);
void rtw_hal_rf_set_tx_pwr_comp(struct hal_info_t *hal_info,
			enum phl_phy_idx phy,
			struct rtw_phl_regu_dyn_ant_gain *dyn_ag);

#endif /*_HAL_API_RF_H_*/
