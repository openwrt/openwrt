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
#ifndef _HAL_COM_I_C_
#define _HAL_COM_I_C_
#include "hal_headers.h"

#define HAL_MAC_TX_LIFETIME_UNIT_US_SHT 8 /* 256 us */

void
rtw_hal_com_scan_set_tx_lifetime(void *hal, u8 band)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	/* set mgnt queue pkt lifetime */
#define HAL_COM_MGQ_TX_LIFETIME_MS 19
	rtw_hal_mac_set_tx_lifetime(hal_info, band, PHL_LIFETIME_MGQ, true,
		((HAL_COM_MGQ_TX_LIFETIME_MS * 1000) >> HAL_MAC_TX_LIFETIME_UNIT_US_SHT));
}

void rtw_hal_com_scan_restore_tx_lifetime(void *hal, u8 band)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	/* reset data/mgnt queue pkt lifetime to 0 */
	rtw_hal_mac_set_tx_lifetime(hal_info, band, PHL_LIFETIME_MGQ, false, 0);
}

/* set ac queue pkt lifetime, val is in unit of ms */
enum rtw_hal_status
rtw_hal_com_set_tx_lifetime(void *hal, u8 band, u8 en, u16 val)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	if (en)
		return rtw_hal_mac_set_tx_lifetime(hal_info, band, PHL_LIFETIME_ACQ, true,
					((val * 1000) >> HAL_MAC_TX_LIFETIME_UNIT_US_SHT));
	else
		return rtw_hal_mac_set_tx_lifetime(hal_info, band, PHL_LIFETIME_ACQ, false,
					0);
}

/* set TPU ofst_mode & ofst_bw, step 0.5 dB */
enum rtw_hal_status
rtw_hal_com_set_power_offset(void *hal, u8 band, s8 ofst_mode, s8 ofst_bw)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hsts;

	hsts = rtw_hal_mac_set_pwr_ofst_mode(hal_info->hal_com, band, ofst_mode);
	if(hsts != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("%s set tpu pwr_ofst_mode - failed\n", __func__);
		return RTW_HAL_STATUS_FAILURE;
	}

	hsts = rtw_hal_mac_write_pwr_ofst_mode(hal_info->hal_com, band);
	if(hsts != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("%s write tpu pwr_ofst_mode - failed\n", __func__);
		return RTW_HAL_STATUS_FAILURE;
	}

	hsts = rtw_hal_mac_set_pwr_ofst_bw(hal_info->hal_com, band, ofst_bw);
	if(hsts != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("%s set tpu pwr_ofst_bw - failed\n", __func__);
		return RTW_HAL_STATUS_FAILURE;
	}

	hsts = rtw_hal_mac_write_pwr_ofst_bw(hal_info->hal_com, band);
	if(hsts != RTW_HAL_STATUS_SUCCESS){
		PHL_ERR("%s write tpu pwr_ofst_bw - failed\n", __func__);
		return RTW_HAL_STATUS_FAILURE;
	}

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_com_set_gt3(void *hal, u8 en, u8 timeout)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	return rtw_hal_mac_set_gt3(hal_info, en, timeout);
}

enum rtw_hal_status rtw_hal_scan_pause_tx_fifo(void *hinfo,
	u8 band_idx, bool off_ch)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hinfo;


	hal_status = rtw_hal_tx_pause(hal_info->hal_com, band_idx,
		off_ch, PAUSE_RSON_NOR_SCAN);

	return hal_status;
}

enum rtw_hal_status
rtw_hal_dfs_pause_tx(void *hinfo, u8 band_idx, bool off_ch, enum tx_pause_rson reason)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hinfo;

	hal_status = rtw_hal_tx_pause(hal_info->hal_com, band_idx,
				      off_ch, reason);

	return hal_status;
}

#ifdef CONFIG_FSM
enum rtw_hal_status rtw_hal_scan_flush_queue(void *hinfo,
	struct rtw_wifi_role_t *wrole)
{
	return RTW_HAL_STATUS_SUCCESS;
}
#endif

enum rtw_hal_status rtw_hal_notify_switch_band(void *hinfo,
			enum band_type band, enum phl_phy_idx phy_idx)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
	struct hal_info_t *hal_info = (struct hal_info_t *)hinfo;

	PHL_INFO("%s: band(%d), phy_idx(%d)\n", __FUNCTION__, band, phy_idx);
#ifdef CONFIG_BTCOEX
	rtw_hal_btc_switch_band_ntfy(hal_info, phy_idx, band);
#endif

	rtw_hal_bb_fw_edcca(hal_info);

	rtw_hal_rf_set_power(hal_info, phy_idx, PWR_BY_RATE);

	rtw_hal_rf_do_tssi_scan(hal_info, phy_idx);

	return hal_status;
}

enum rtw_hal_status
rtw_hal_proc_cmd(void *hal, char proc_cmd, struct rtw_proc_cmd *incmd,
							char *output, u32 out_len)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	if(proc_cmd == RTW_PROC_CMD_BB){
		if(rtw_hal_bb_proc_cmd(hal_info, incmd, output, out_len))
			hal_status = RTW_HAL_STATUS_SUCCESS;
	}
	if(proc_cmd == RTW_PROC_CMD_RF){
		if(rtw_hal_rf_proc_cmd(hal_info, incmd, output, out_len))
			hal_status = RTW_HAL_STATUS_SUCCESS;
	}
	if(proc_cmd == RTW_PROC_CMD_MAC){
		if(rtw_hal_mac_proc_cmd(hal_info, incmd, output, out_len))
			hal_status = RTW_HAL_STATUS_SUCCESS;
	}
#ifdef CONFIG_BTCOEX
	if(proc_cmd == RTW_PROC_CMD_BTC){
		if(rtw_hal_btc_proc_cmd(hal_info, incmd, output, out_len))
			hal_status = RTW_HAL_STATUS_SUCCESS;
	}
#endif
	if(proc_cmd == RTW_PROC_CMD_EFUSE){
		if(rtw_hal_efuse_proc_cmd(hal_info, incmd, output, out_len))
			hal_status = RTW_HAL_STATUS_SUCCESS;
	}
	return hal_status;
}

void rtw_hal_get_mac_version(char *ver_str, u16 len)
{
	rtw_hal_mac_get_version(ver_str, len);
}

void rtw_hal_get_fw_ver(void *hal, char *ver_str, u16 len)
{
	rtw_hal_mac_get_fw_ver((struct hal_info_t *)hal, ver_str, len);
}

enum rtw_hal_status
rtw_hal_set_mu_edca(void *hal, u8 band, u8 ac,
	u16 timer, u8 cw_min, u8 cw_max, u8 aifsn)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	u16 timer_32us;
	u8 aifs_us;
	u8 slot_time, sifs;

	/* TODO: Get aSlotTime and aSIFS according to current PHY */
	slot_time = 9;
	sifs = 16;
	timer_32us = (timer<<8);
	aifs_us = (aifsn == 0)?0:(aifsn*slot_time + sifs);
	hal_status = rtw_hal_mac_set_mu_edca(hal_info->hal_com,
					band, ac, timer_32us, cw_min, cw_max, aifs_us);
	return hal_status;
}

enum rtw_hal_status
rtw_hal_set_mu_edca_ctrl(void *hal, u8 band, u8 wmm, u8 set)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	hal_status = rtw_hal_mac_set_mu_edca_ctrl(hal_info->hal_com,
					band, wmm, set);
	return hal_status;
}

enum rtw_hal_status rtw_hal_ppdu_sts_init(void *hal, u8 band_idx,
			bool en, u8 appen_info, u8 filter)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	hal_status = rtw_hal_mac_ppdu_stat_cfg(
				hal_info, band_idx, en,
				appen_info,
				filter);

	hal_com->band[band_idx].ppdu_sts_appen_info = appen_info;
	hal_com->band[band_idx].ppdu_sts_filter = filter;

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("%s - failed\n", __func__);
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return hal_status;
}

enum rtw_hal_status rtw_hal_ppdu_sts_cfg(void *hal, u8 band_idx, bool en)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;

	if (en) {
		hal_status = rtw_hal_mac_ppdu_stat_cfg(
			hal_info, band_idx, true,
			hal_com->band[band_idx].ppdu_sts_appen_info,
			hal_com->band[band_idx].ppdu_sts_filter);
	} else {
		hal_status = rtw_hal_mac_ppdu_stat_cfg(hal_info, band_idx, false, 0, 0);
	}

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("%s (en %d) - failed\n", __func__, en);

	return hal_status;
}

enum rtw_hal_status
rtw_hal_reset(struct rtw_hal_com_t *hal_com, enum phl_phy_idx phy_idx, u8 band_idx, bool reset)
{
	struct hal_info_t *hal_info = hal_com->hal_priv;
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;

#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	PHL_INFO("%s: phy_idx(%d) band_idx(%d)\n", __FUNCTION__, phy_idx, band_idx);

	if (reset){
		/* disable contention */
		status = rtw_hal_tx_pause(hal_com, band_idx, true, PAUSE_RSON_RESET);
		if(status != RTW_HAL_STATUS_SUCCESS){
			PHL_ERR("%s rtw_hal_tx_pause - failed\n", __func__);
			return status;
		}

		/* disable ppdu_sts */
		status = rtw_hal_ppdu_sts_cfg(hal_info, band_idx, false);
		if(status != RTW_HAL_STATUS_SUCCESS){
			PHL_ERR("%s rtw_hal_ppdu_sts_cfg - failed\n", __func__);
			return status;
		}

		rtw_hal_bb_bb_reset_cmn(hal_info, true, phy_idx);
	}else{
		/*enable ppdu_sts*/
		status = rtw_hal_ppdu_sts_cfg(hal_info, band_idx, true);

		if(status != RTW_HAL_STATUS_SUCCESS){
			PHL_ERR("%s rtw_hal_ppdu_sts_cfg - failed\n", __func__);
			return status;
		}

		rtw_hal_bb_bb_reset_cmn(hal_info, false, phy_idx);

		status = rtw_hal_tx_pause(hal_com, band_idx, false, PAUSE_RSON_RESET);
		if(status != RTW_HAL_STATUS_SUCCESS){
			PHL_ERR("%s rtw_hal_tx_pause - failed\n", __func__);
			return status;
		}
	}
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return status;
}

void
rtw_hal_disconnect_notify(void *hal, struct rtw_chan_def *chandef)
{
	rtw_hal_rf_disconnect_notify(hal, chandef);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_hal_disconnect_notify(): chandef band(%d), chan(%d), bw(%d), offset(%d)\n",
		chandef->band, chandef->chan, chandef->bw, chandef->offset);
}

bool rtw_hal_check_ch_rfk(void *hal, struct rtw_chan_def *chandef)
{
	bool check = false;
	check = rtw_hal_rf_check_mcc_ch(hal, chandef);
	PHL_TRACE(COMP_PHL_MCC, _PHL_INFO_, "rtw_hal_check_ch_rfk(): check ok(%d), band(%d), chan(%d), bw(%d), offset(%d)\n",
		check, chandef->band, chandef->chan, chandef->bw, chandef->offset);
	return check;
}

void rtw_hal_env_rpt(struct rtw_hal_com_t *hal_com,
                     struct rtw_env_report *env_rpt,
                     u8 hw_band)
{
	enum phl_phy_idx p_idx = HW_PHY_0;

	p_idx = rtw_hal_hw_band_to_phy_idx(hw_band);
	rtw_hal_bb_env_rpt(hal_com, env_rpt, p_idx);
}

enum phl_band_idx rtw_hal_phy_idx_to_hw_band(enum phl_phy_idx p_idx)
{
	enum phl_band_idx band_idx = HW_BAND_MAX;

	if (p_idx == HW_PHY_0)
		band_idx = HW_BAND_0;
	else if (p_idx == HW_PHY_1)
		band_idx = HW_BAND_1;
	else
		PHL_ERR("%s: error phy_idx(%d)\n", __func__, p_idx);
	return band_idx;
}

enum phl_phy_idx rtw_hal_hw_band_to_phy_idx(enum phl_band_idx band_idx)
{
	enum phl_phy_idx p_idx = HW_PHY_MAX;

	if (band_idx == HW_BAND_0)
		p_idx = HW_PHY_0;
	else if (band_idx == HW_BAND_1)
		p_idx = HW_PHY_1;
	else
		PHL_ERR("%s: error band_idx(%d)\n", __func__, band_idx);
	return p_idx;
}

void
rtw_hal_init_hw_band_info(void *hal, enum phl_band_idx band_idx)
{
	struct hal_info_t *hal_i = (struct hal_info_t *)hal;
	struct rtw_hw_band *hw_band_i = &hal_i->hal_com->band[band_idx];

	PHL_INFO("%s: band_idx(%d)\n", __FUNCTION__, band_idx);
	_os_mem_set(hal_i->phl_com->drv_priv, hw_band_i,
			0, sizeof(struct rtw_hw_band));
	rtw_phl_init_chdef(hal_i->phl_com, &hw_band_i->cur_chandef);
}
#ifdef DBG_DUMP_TX_COUNTER
void rtw_hal_dump_tx_status(void *hal, enum phl_band_idx bidx)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;

	rtw_hal_mac_get_tx_cnt(hal_info, bidx, 0);
	rtw_hal_bb_dump_tx_sts(hal_info, true, rtw_hal_hw_band_to_phy_idx(bidx));

}
#endif
#endif /* _HAL_COM_I_C_ */
