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
#define _PHL_EXT_TX_PWR_LMT_C_
#include "phl_headers.h"

/**
 * The function to update current TX power limit value to HW register
 * @phl: see struct phl_info_t
 *
 */
enum rtw_phl_status
rtw_phl_set_power_limit(void *phl)
{
	struct	phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	u8 hw_band = 0;

	for (hw_band = 0; hw_band < HW_BAND_MAX; hw_band++) {
		if (rtw_hal_set_power_limit(phl_info->hal, hw_band) == RTW_HAL_STATUS_SUCCESS)
			phl_status = RTW_PHL_STATUS_SUCCESS;
	}
	return phl_status;
}

/**
 * The function to get TX power limit value with specific parameters
 * @phl: see struct phl_info_t
 * @hw_band: 0x0: band0, 0x1: band1
 * @rate: data rate
 * @bandwidth: banddwidth
 * @beamforming: 0: TX w/o BF, 1: TX w/ BF
 * @tx_num: tx number, 0: 1TX, 1: 2TX
 * @channel: center channel
 *
 */
s8 rtw_phl_get_power_limit(void *phl, u8 hw_band,
	u16 rate, u8 bandwidth, u8 beamforming, u8 tx_num, u8 channel)
{
	struct	phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_get_power_limit(phl_info->hal, hw_band, rate,
		bandwidth, beamforming, tx_num, channel);
}

/**
 * The function to update user defined extended tx power limit to halrf
 * @phl: see struct phl_info_t
 * @hw_band: 0x0: band0, 0x1: band1
 * @ext_pwr_lmt_info: table of extended tx power limit value
 *
 * Note,
 * This function will enable extended tx power limit mechanism.
 * After enabled this mechanism, halrf will use
 * min(original tx power limit, extended tx power limit) to be
 * final tx power limit value.
 *
 */
void
rtw_phl_enable_ext_pwr_lmt(void *phl, u8 hw_band,
		struct rtw_phl_ext_pwr_lmt_info *ext_pwr_lmt_info)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_enable_ext_pwr_lmt(phl_info->hal, hw_band, ext_pwr_lmt_info);
}

void
rtw_phl_set_ext_pwr_lmt_en(void *phl, bool enable)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	u8 hw_band = 0;

	for (hw_band = 0; hw_band < HW_BAND_MAX; hw_band++) {
		rtw_hal_set_ext_pwr_lmt_en(phl_info->hal, hw_band, enable);
	}
}

static void
_phl_cmd_updt_epl_done(
	void *drv_priv, u8 *cmd, u32 cmd_len,
	enum rtw_phl_status status)
{
	if (cmd) {
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s...\n", __func__);
	}
}


static enum rtw_phl_status
_phl_updt_ext_txpwr_lmt(
	void *phl, struct rtw_phl_cmd_epl_para *param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	u8 hw_band = 0;
	bool cur_state = rtw_hal_get_ext_pwr_lmt_en(phl_info->hal, hw_band);
	bool tgt_state = (param->en ? true : false);

	if (param->upt_info) {
		for (hw_band = 0; hw_band < HW_BAND_MAX; hw_band++) {
			rtw_phl_enable_ext_pwr_lmt(phl, hw_band, &param->info);
		}
	}
	/* Only update state and reload when state changed OR table updated */
	if (tgt_state != cur_state || param->upt_info) {
		rtw_phl_set_ext_pwr_lmt_en(phl, tgt_state);
		if (param->pwrlmt_rld)
			rtw_phl_set_power_limit(phl);
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_cmd_updt_ext_txpwr_lmt(
	void *phl, u8 *param)
{
	struct rtw_phl_cmd_epl_para *cmd_para = (struct rtw_phl_cmd_epl_para *)param;

	return _phl_updt_ext_txpwr_lmt(phl, cmd_para);
}

enum rtw_phl_status
rtw_phl_cmd_updt_ext_txpwr_lmt(void *phl,
	struct rtw_phl_cmd_epl_para *param,
	enum phl_cmd_type cmd_type,
	u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_cmd_epl_para *cmd_param = NULL;
	u32 cmd_param_len = 0;

#ifdef CONFIG_CMD_DISP
	if (cmd_type == PHL_CMD_DIRECTLY)
		return _phl_updt_ext_txpwr_lmt(phl, param);

	cmd_param_len = sizeof(struct rtw_phl_cmd_epl_para);
	cmd_param = _os_kmem_alloc(phl_to_drvpriv(phl_info), cmd_param_len);
	if (cmd_param == NULL) {
		PHL_ERR("%s: alloc cmd_param failed!\n", __func__);
		goto end;
	} else {
		_os_mem_cpy(phl_to_drvpriv(phl_info), cmd_param,
			param, cmd_param_len);
	}

	sts = phl_cmd_enqueue(phl,
			HW_BAND_0,
			MSG_EVT_UPDT_EXT_TXPWR_LMT,
			(u8 *)cmd_param, 0,
			_phl_cmd_updt_epl_done,
			cmd_type, cmd_timeout);

	if (is_cmd_failure(sts)) {
		/* Send cmd success, but wait cmd fail*/
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(phl_to_drvpriv(phl_info), cmd_param, cmd_param_len);
		sts = RTW_PHL_STATUS_FAILURE;
	}
end:
	return sts;
#else
	PHL_ERR("%s : CONFIG_CMD_DISP need to be enabled for MSG_EVT_UPDT_EXT_TXPWR_LMT !! \n", __func__);

	return sts;
#endif
}

