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
#define _PHL_DFS_C_
#include "phl_headers.h"

#ifdef CONFIG_PHL_DFS

struct dfs_rd_ctl_param {
	/*
	* DFS region domain set from core
	* >=DFS_REGD_NUM => not set (keep original)
	* < DFS_REGD_NUM  => change domain (if needed) and ignore other parameters
	*/
	enum dfs_regd_t domain;

	/*
	* enable=true,  cac=1     => enable radar detect and is under CAC
	* enable=true,  cac=0     => enalbe radar detect, in-service monitoring
	* enable=true,  cac<0     => enable radar detect w/o changing CAC status
	* enable=false, cac=any => disable radar detect
	*/
	bool enable;

	/*
	* CAC status set from core
	* < 0: not set (keep original)
	*    0: not under CAC
	*    1: under CAC
	*/
	s8 cac;

	/*
	* configuration for specific radar detect range (5G band is implicit), the value of sp_ch:
	* < 0: not set (keep original)
	*    0: all detectable range
	* > 0: specific detect range (by ch, bw, offset)
	*/
	s16 sp_ch;
	enum channel_width sp_bw;
	enum chan_offset sp_offset;

	/*
	* configuration for specific radar detect range in freqency, valid when sp_ch < 0
	* 0: not set (keep original)
	*/
	u32 sp_freq_hi;
	u32 sp_freq_lo;
};

static bool phl_radar_detect_range_specified(struct rtw_dfs_t *dfs_info)
{
	return dfs_info->sp_detect_range_hi != 0;
}

static bool phl_overlap_radar_detect_range(struct rtw_dfs_t *dfs_info
	, enum band_type band, u8 ch, enum channel_width bw, enum chan_offset offset)
{
	bool ret = false;
	u32 hi = 0, lo = 0;
	int i;

	if (!rtw_phl_bchbw_to_freq_range(band, ch, bw, offset, &hi, &lo)) {
		_os_warn_on(1);
		goto exit;
	}

	if (RANGE_OVERLAP(hi, lo, dfs_info->sp_detect_range_hi, dfs_info->sp_detect_range_lo))
		ret = true;

exit:
	return ret;
}

static bool phl_should_radar_detect_enable_by_ch(struct phl_info_t *phl_info, u8 band_idx,
	enum band_type band, u8 channel, enum channel_width bwmode, enum chan_offset offset)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_dfs_t *dfs_info = &phl_com->dfs_info;

	if (dfs_info->enable && !PHL_DFS_REGD_IS_UNKNOWN(dfs_info->region_domain)
		&& rtw_hal_in_radar_domain(phl_info->hal, band, channel, bwmode, offset)
	) {
		if (!phl_radar_detect_range_specified(dfs_info)
			|| phl_overlap_radar_detect_range(dfs_info, band, channel, bwmode, offset))
			return true;
	}

	return false;
}

static void phl_dfs_handle_pending_domain_change(struct phl_info_t *phl_info, u8 band_idx
	, enum band_type band, u8 ch, enum channel_width bw, enum chan_offset offset)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_dfs_t *dfs_info = &phl_com->dfs_info;

	if (dfs_info->pending_domain_change) {
		rtw_hal_bb_dfs_change_domain(phl_info->hal, band, ch, bw, offset);
		dfs_info->pending_domain_change = false;
	}
}

static enum rtw_phl_status
phl_radar_detect_switch(struct phl_info_t *phl_info, u8 band_idx, bool enable)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_dfs_t *dfs_info = &phl_com->dfs_info;

	if (rtw_hal_radar_detect_cfg(phl_info->hal, enable) == RTW_HAL_STATUS_SUCCESS) {
		dfs_info->radar_detect_enabled = enable;
		return RTW_PHL_STATUS_SUCCESS;
	}
	return RTW_PHL_STATUS_FAILURE;
}

bool phl_is_radar_detect_enabled(struct phl_info_t *phl_info, u8 band_idx)
{
	return phl_info->phl_com->dfs_info.radar_detect_enabled;
}

static void phl_set_under_cac(struct phl_info_t *phl_info, u8 band_idx, bool under)
{
	if (under)
		SET_STATUS_FLAG(phl_info->phl_com->dev_state, RTW_DEV_IN_DFS_CAC_PERIOD);
	else
		CLEAR_STATUS_FLAG(phl_info->phl_com->dev_state, RTW_DEV_IN_DFS_CAC_PERIOD);
}

bool phl_is_under_cac(struct phl_info_t *phl_info, u8 band_idx)
{
	return TEST_STATUS_FLAG(phl_info->phl_com->dev_state, RTW_DEV_IN_DFS_CAC_PERIOD);
}

static enum rtw_phl_status
phl_cac_tx_pause_switch(struct phl_info_t *phl_info, u8 band_idx, bool enable)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_dfs_t *dfs_info = &phl_com->dfs_info;

	if (rtw_hal_dfs_pause_tx(phl_info->hal, band_idx, enable, PAUSE_RSON_DFS_CAC) == RTW_HAL_STATUS_SUCCESS) {
		dfs_info->cac_tx_paused = enable;
		return RTW_PHL_STATUS_SUCCESS;
	}
	return RTW_PHL_STATUS_FAILURE;
}

bool phl_is_cac_tx_paused(struct phl_info_t *phl_info, u8 band_idx)
{
	return phl_info->phl_com->dfs_info.cac_tx_paused;
}

void phl_dfs_rd_setting_before_ch_switch(struct phl_info_t *phl_info, u8 band_idx
	, enum band_type band, u8 ch, enum channel_width bw, enum chan_offset offset, struct dfs_rd_ch_switch_ctx *ctx)
{
	ctx->should_rd_en_on_new_ch = phl_should_radar_detect_enable_by_ch(phl_info
		, band_idx, band, ch, bw, offset);
	ctx->under_cac = phl_is_under_cac(phl_info, band_idx);
	ctx->cac_tx_paused = phl_is_cac_tx_paused(phl_info, band_idx);

	ctx->rd_enabled = phl_is_radar_detect_enabled(phl_info, band_idx);

	if (!ctx->should_rd_en_on_new_ch && ctx->rd_enabled) {
		/* turn off radar detect before channel setting (ex: leaving detection range) */
		enum rtw_phl_status rst = phl_radar_detect_switch(phl_info, band_idx, false);

		PHL_TRACE(COMP_PHL_DBG, (rst == RTW_PHL_STATUS_SUCCESS ? _PHL_INFO_ : _PHL_ERR_)
			, "[DFS] new ch=%d,%u,%d,%d disable radar detect%s\n"
			, band, ch, bw, offset, rst != RTW_PHL_STATUS_SUCCESS ? " failed" : "");

		ctx->rd_enabled = phl_is_radar_detect_enabled(phl_info, band_idx);
	}

	if (ctx->should_rd_en_on_new_ch && ctx->under_cac && !ctx->cac_tx_paused) {
		/* turn on CAC tx pause before channel setting (ex: entering detection range) */
		enum rtw_phl_status rst = phl_cac_tx_pause_switch(phl_info, band_idx, true);

		PHL_TRACE(COMP_PHL_DBG, (rst == RTW_PHL_STATUS_SUCCESS ? _PHL_INFO_ : _PHL_ERR_)
			, "[DFS] new ch=%d,%u,%d,%d enable CAC tx pause%s\n"
			, band, ch, bw, offset, rst != RTW_PHL_STATUS_SUCCESS ? " failed" : "");
	}
}

void phl_dfs_rd_setting_after_ch_switch(struct phl_info_t *phl_info, u8 band_idx
	, enum band_type band, u8 ch, enum channel_width bw, enum chan_offset offset, struct dfs_rd_ch_switch_ctx *ctx)
{
	if (ctx->should_rd_en_on_new_ch && !ctx->rd_enabled) {
		/* turn on radar detect after channel setting (ex: entered radar detect range) */
		enum rtw_phl_status rst = phl_radar_detect_switch(phl_info, band_idx, true);

		PHL_TRACE(COMP_PHL_DBG, (rst == RTW_PHL_STATUS_SUCCESS ? _PHL_INFO_ : _PHL_ERR_)
			, "[DFS] new ch=%d,%u,%d,%d enable radar detect%s\n"
			, band, ch, bw, offset, rst != RTW_PHL_STATUS_SUCCESS ? " failed" : "");
	}

	if ((!ctx->should_rd_en_on_new_ch || !ctx->under_cac) && ctx->cac_tx_paused) {
		/* turn off CAC tx pause after channel setting (ex: leaved detection range) */
		enum rtw_phl_status rst = phl_cac_tx_pause_switch(phl_info, band_idx, false);

		PHL_TRACE(COMP_PHL_DBG, (rst == RTW_PHL_STATUS_SUCCESS ? _PHL_INFO_ : _PHL_ERR_)
			, "[DFS] new ch=%d,%u,%d,%d disable CAC tx pause%s\n"
			, band, ch, bw, offset, rst != RTW_PHL_STATUS_SUCCESS ? " failed" : "");
	}
}

static enum rtw_phl_status
phl_radar_detect_confs_apply(struct phl_info_t *phl_info, u8 band_idx)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_dfs_t *dfs_info = &phl_com->dfs_info;
	enum rtw_phl_status rd_rst = RTW_PHL_STATUS_SUCCESS;
	enum rtw_phl_status cac_rst = RTW_PHL_STATUS_SUCCESS;
	struct rtw_chan_def chdef;
	bool should_rd_enable;
	bool under_cac;
	bool cac_tx_paused;

	rtw_hal_get_cur_chdef(phl_info->hal, band_idx, &chdef);

	phl_dfs_handle_pending_domain_change(phl_info, band_idx
		, chdef.band, chdef.chan, chdef.bw, chdef.offset);

	should_rd_enable = phl_should_radar_detect_enable_by_ch(phl_info, band_idx
		, chdef.band, chdef.chan, chdef.bw, chdef.offset);
	under_cac = phl_is_under_cac(phl_info, band_idx);
	cac_tx_paused = phl_is_cac_tx_paused(phl_info, band_idx);

	if (!dfs_info->radar_detect_enabled) {
		if (should_rd_enable) {
			rd_rst = phl_radar_detect_switch(phl_info, band_idx, true);
			PHL_TRACE(COMP_PHL_DBG, (rd_rst == RTW_PHL_STATUS_SUCCESS ? _PHL_INFO_ : _PHL_ERR_)
				, "[DFS] ch=%d,%u,%d,%d enable radar detect%s\n"
				, chdef.band, chdef.chan, chdef.bw, chdef.offset
				, rd_rst != RTW_PHL_STATUS_SUCCESS ? " failed" : "");
		}

	} else if (dfs_info->radar_detect_enabled) {
		if (!should_rd_enable) {
			rd_rst = phl_radar_detect_switch(phl_info, band_idx, false);
			PHL_TRACE(COMP_PHL_DBG, (rd_rst == RTW_PHL_STATUS_SUCCESS ? _PHL_INFO_ : _PHL_ERR_)
				, "[DFS] ch=%d,%u,%d,%d disable radar detect%s\n"
				, chdef.band, chdef.chan, chdef.bw, chdef.offset
				, rd_rst != RTW_PHL_STATUS_SUCCESS ? " failed" : "");
		}
	}

	if (!cac_tx_paused) {
		if (under_cac && dfs_info->radar_detect_enabled) {
			cac_rst = phl_cac_tx_pause_switch(phl_info, band_idx, true);
			PHL_TRACE(COMP_PHL_DBG, (cac_rst == RTW_PHL_STATUS_SUCCESS ? _PHL_INFO_ : _PHL_ERR_)
				, "[DFS] ch=%d,%u,%d,%d enable CAC tx pause%s\n"
				, chdef.band, chdef.chan, chdef.bw, chdef.offset
				, cac_rst != RTW_PHL_STATUS_SUCCESS ? " failed" : "");
		}

	} else 	if (cac_tx_paused) {
		if (!under_cac) {
			/*
			* Release CAC tx pause only when not under CAC
			* Keep CAC tx pause when under CAC and radar detect is turned off by
			* specifying new detect range which doesn't overlap current channel setting
			* (ex: operating channel switching to new DFS channel)
			*/
			cac_rst = phl_cac_tx_pause_switch(phl_info, band_idx, false);
			PHL_TRACE(COMP_PHL_DBG, (cac_rst == RTW_PHL_STATUS_SUCCESS ? _PHL_INFO_ : _PHL_ERR_)
				, "[DFS] ch=%d,%u,%d,%d disable CAC tx pause%s\n"
				, chdef.band, chdef.chan, chdef.bw, chdef.offset
				, cac_rst != RTW_PHL_STATUS_SUCCESS ? " failed" : "");
		}
	}

	if (rd_rst == RTW_PHL_STATUS_SUCCESS && cac_rst == RTW_PHL_STATUS_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	return RTW_PHL_STATUS_FAILURE;
}

static enum rtw_phl_status
_phl_dfs_rd_ctl_hdl(void *phl, u8 *param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_dfs_t *dfs_info = &phl_com->dfs_info;
	struct dfs_rd_ctl_param *rd_ctl_param = (struct dfs_rd_ctl_param *)param;
	u8 band_idx = HW_BAND_0; /* multi band/phy capable? */

	if (rd_ctl_param->domain < DFS_REGD_NUM) {
		if (rd_ctl_param->domain != dfs_info->region_domain) {
			PHL_INFO("%s set domain to %d\n", __func__, rd_ctl_param->domain);
			dfs_info->region_domain = rd_ctl_param->domain;
			dfs_info->pending_domain_change = true;
		}
		goto apply;
	}

	if (rd_ctl_param->enable) {
		if (!dfs_info->enable) {
			PHL_INFO("%s enable\n", __func__);
			dfs_info->enable = true;
		}
		if (rd_ctl_param->cac == 1) {
			if (!phl_is_under_cac(phl_info, band_idx)) {
				PHL_INFO("%s under CAC\n", __func__);
				phl_set_under_cac(phl_info, band_idx, true);
			}
		} else if (rd_ctl_param->cac == 0) {
			if (phl_is_under_cac(phl_info, band_idx)) {
				PHL_INFO("%s CAC done\n", __func__);
				phl_set_under_cac(phl_info, band_idx, false);
			}
		}

	} else {
		if (dfs_info->enable) {
			PHL_INFO("%s disable\n", __func__);
			dfs_info->enable = false;
		}
		phl_set_under_cac(phl_info, band_idx, false);
	}

	if (rd_ctl_param->sp_ch > 0) {
		u32 hi, lo;

		if (rtw_phl_bchbw_to_freq_range(BAND_ON_5G
				, rd_ctl_param->sp_ch, rd_ctl_param->sp_bw, rd_ctl_param->sp_offset
				, &hi, &lo)
		) {
			if (dfs_info->sp_detect_range_hi != hi || dfs_info->sp_detect_range_lo != lo) {
				PHL_INFO("%s sp_ch:%u,%d,%d is set\n", __func__
					, rd_ctl_param->sp_ch, rd_ctl_param->sp_bw, rd_ctl_param->sp_offset);
				dfs_info->sp_detect_range_hi = hi;
				dfs_info->sp_detect_range_lo = lo;
			}
		} else {
			PHL_WARN("%s sp_ch:%u,%d,%d to freq range fail, all range applied\n", __func__
				, rd_ctl_param->sp_ch, rd_ctl_param->sp_bw, rd_ctl_param->sp_offset);
			dfs_info->sp_detect_range_hi = 0;
		}
	} else if (rd_ctl_param->sp_ch == 0) {
		if (dfs_info->sp_detect_range_hi != 0) {
			PHL_INFO("%s all range applied\n", __func__);
			dfs_info->sp_detect_range_hi = 0;
		}
	} else if (rd_ctl_param->sp_freq_hi) {
		if (rd_ctl_param->sp_freq_hi <= rd_ctl_param->sp_freq_lo) {
			PHL_WARN("%s sp_freq_hi:%u <= sp_freq_lo:%u, all range applied\n", __func__
				, rd_ctl_param->sp_freq_hi, rd_ctl_param->sp_freq_lo);
		} else {
			if (dfs_info->sp_detect_range_hi != rd_ctl_param->sp_freq_hi
				|| dfs_info->sp_detect_range_lo != rd_ctl_param->sp_freq_lo
			) {
				PHL_INFO("%s sp_freq %u to %u is set\n", __func__
					, rd_ctl_param->sp_freq_lo, rd_ctl_param->sp_freq_hi);
				dfs_info->sp_detect_range_hi = rd_ctl_param->sp_freq_hi;
				dfs_info->sp_detect_range_lo = rd_ctl_param->sp_freq_lo;
			}
		}
	}

apply:
	/* apply new configs on cur channel */
	return phl_radar_detect_confs_apply(phl_info, band_idx);
}

enum rtw_phl_status
phl_cmd_dfs_rd_ctl_hdl(struct phl_info_t *phl_info, u8 *param)
{
	return _phl_dfs_rd_ctl_hdl(phl_info, param);
}

static void _phl_dfs_rd_ctl_done(void *drv_priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status)
{
	if (cmd) {
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}

static enum rtw_phl_status
_rtw_phl_cmd_dfs_rd_ctl(struct phl_info_t *phl_info, enum phl_band_idx hw_band
	, enum dfs_regd_t domain, bool enable, s8 cac, s16 sp_ch, enum channel_width sp_bw, enum chan_offset sp_offset
	, u32 sp_freq_hi, u32 sp_freq_lo
	, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
#ifdef CONFIG_CMD_DISP
	void *drv = phl_info->phl_com->drv_priv;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct dfs_rd_ctl_param *param = NULL;
	u32 param_len;

	param_len = sizeof(struct dfs_rd_ctl_param);
	param = _os_kmem_alloc(drv, param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		goto _exit;
	}

	param->domain = domain;
	param->enable = enable;
	param->cac = cac;
	param->sp_ch = sp_ch;
	param->sp_bw = sp_bw;
	param->sp_offset = sp_offset;
	param->sp_freq_hi = sp_freq_hi;
	param->sp_freq_lo = sp_freq_lo;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = phl_cmd_dfs_rd_ctl_hdl(phl_info, (u8 *)param);
		_phl_dfs_rd_ctl_done(drv, (u8 *)param, param_len, psts);
		goto _exit;
	}

	psts = phl_cmd_enqueue(phl_info,
			hw_band,
			MSG_EVT_DFS_RD_SETUP,
			(u8 *)param, param_len,
			_phl_dfs_rd_ctl_done,
			cmd_type, cmd_timeout);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(drv, param, param_len);
		psts = RTW_PHL_STATUS_FAILURE;
	}

_exit:
	return psts;
#else
	PHL_ERR("%s(), CONFIG_CMD_DISP need to be enabled for MSG_EVT_DFS_RD_SETUP \n",__func__);

	return RTW_PHL_STATUS_FAILURE;
#endif
}

enum rtw_phl_status
rtw_phl_cmd_dfs_change_domain(void *phl_info, enum phl_band_idx hw_band
	, enum dfs_regd_t domain, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	if (domain >= DFS_REGD_NUM) {
		PHL_WARN("%s(), invalid domain:%d\n", __func__, domain);
		return RTW_PHL_STATUS_INVALID_PARAM;
	}

	return _rtw_phl_cmd_dfs_rd_ctl(phl_info, hw_band
		, domain /* change domain, other parameters will be ignored */
		, false, 0, -1, 0, 0, 0, 0
		, cmd_type, cmd_timeout);
}

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_enable_all_range(void *phl_info, enum phl_band_idx hw_band
	, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	return _rtw_phl_cmd_dfs_rd_ctl(phl_info, hw_band
		, DFS_REGD_NUM
		, true, -1 /* enable radar detect w/o changing CAC status */
		, 0, 0, 0, 0, 0
		, cmd_type, cmd_timeout);
}

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_enable_with_sp_chbw(void *phl_info, enum phl_band_idx hw_band
	, bool cac, u8 sp_ch, enum channel_width sp_bw, enum chan_offset sp_offset
	, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	return _rtw_phl_cmd_dfs_rd_ctl(phl_info, hw_band
		, DFS_REGD_NUM
		, true, cac ? 1 : 0, sp_ch, sp_bw, sp_offset, 0, 0
		, cmd_type, cmd_timeout);
}

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_enable_with_sp_freq_range(void *phl_info, enum phl_band_idx hw_band
	, bool cac, u32 sp_freq_hi, u32 sp_freq_lo
	, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	return _rtw_phl_cmd_dfs_rd_ctl(phl_info, hw_band
		, DFS_REGD_NUM
		, true, cac ? 1 : 0, -1, 0, 0, sp_freq_hi, sp_freq_lo
		, cmd_type, cmd_timeout);
}

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_set_cac_status(void *phl_info, enum phl_band_idx hw_band
	, bool cac, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	return _rtw_phl_cmd_dfs_rd_ctl(phl_info, hw_band
		, DFS_REGD_NUM
		, true /* CAC status only valid when radar detect enable */
		, cac ? 1 : 0, -1, 0, 0, 0, 0
		, cmd_type, cmd_timeout);
}

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_disable(void *phl_info, enum phl_band_idx hw_band
	, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	return _rtw_phl_cmd_dfs_rd_ctl(phl_info, hw_band
		, DFS_REGD_NUM
		, false, 0, -1, 0, 0, 0, 0
		, cmd_type, cmd_timeout);
}
#endif
