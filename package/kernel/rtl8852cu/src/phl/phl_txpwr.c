/******************************************************************************
 *
 * Copyright(c) 2021 Realtek Corporation.
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
#define _PHL_TXPWR_C_
#include "phl_headers.h"

int rtw_phl_get_pw_lmt_regu_type_from_str(void *phl, const char *str)
{
	struct phl_info_t *phl_info = phl;

	return rtw_hal_get_pw_lmt_regu_type_from_str(phl_info->hal, str);
}

const char *rtw_phl_get_pw_lmt_regu_str_from_type(void *phl, u8 regu)
{
	struct phl_info_t *phl_info = phl;

	return rtw_hal_get_pw_lmt_regu_str_from_type(phl_info->hal, regu);
}

const char *rtw_phl_get_pw_lmt_regu_type_str(void *phl, enum band_type band)
{
	struct phl_info_t *phl_info = phl;

	return rtw_hal_get_pw_lmt_regu_type_str(phl_info->hal, band);
}

bool rtw_phl_pw_lmt_regu_tbl_exist(void *phl, enum band_type band, u8 regu)
{
	struct phl_info_t *phl_info = phl;

	return rtw_hal_pw_lmt_regu_tbl_exist(phl_info->hal, band, regu);
}

u8 rtw_phl_ext_reg_codemap_search(void *phl, u16 domain_code, const char *country, const char **reg_name)
{
	struct phl_info_t *phl_info = phl;

	return rtw_hal_ext_reg_codemap_search(phl_info->hal, domain_code, country, reg_name);
}

bool rtw_phl_get_pwr_lmt_en(void *phl, u8 band_idx)
{
	struct phl_info_t *phl_info = phl;

	return rtw_hal_get_pwr_lmt_en(phl_info->hal, band_idx);
}

static void phl_set_embed_pwr_lmt_regu_info(void *drv
	, struct txpwr_regu_info_t *dst, u8 *embed_start
	, bool force, u8 *regu_2g, u8 regu_2g_len
	, u8 *regu_5g, u8 regu_5g_len, u8 *regu_6g, u8 regu_6g_len)
{
	u8 *pos = embed_start;

	dst->force = force;

	if (regu_2g && regu_2g_len) {
		dst->regu_2g = pos;
		_os_mem_cpy(drv, pos, regu_2g, regu_2g_len);
		dst->regu_2g_len = regu_2g_len;
		pos += regu_2g_len;
	} else {
		dst->regu_2g = NULL;
		dst->regu_2g_len = 0;
	}
	if (regu_5g && regu_5g_len) {
		dst->regu_5g = pos;
		_os_mem_cpy(drv, pos, regu_5g, regu_5g_len);
		dst->regu_5g_len = regu_5g_len;
		pos += regu_5g_len;
	} else {
		dst->regu_5g = NULL;
		dst->regu_5g_len = 0;
	}
	if (regu_6g && regu_6g_len) {
		dst->regu_6g = pos;
		_os_mem_cpy(drv, pos, regu_6g, regu_6g_len);
		dst->regu_6g_len = regu_6g_len;
		pos += regu_6g_len;
	} else {
		dst->regu_6g = NULL;
		dst->regu_6g_len = 0;
	}
}

static void phl_set_embed_force_pwr_lmt_regu_info(void *drv
	, struct txpwr_regu_info_t *dst, u8 *embed_start
	, const struct txpwr_regu_info_t *src)
{
	phl_set_embed_pwr_lmt_regu_info(drv, dst, embed_start
		, src->force
		, src->regu_2g, src->regu_2g_len
		, src->regu_5g, src->regu_5g_len
		, src->regu_6g, src->regu_6g_len);
}

static struct txpwr_regu_info_t *phl_get_embed_auto_pwr_lmt_regu_info(void *drv
	, struct phl_info_t *phl_info)
{
	struct txpwr_regu_info_t *ret;
	size_t ret_len;
	u8 regu_2g;
	u8 regu_5g;
	u8 regu_6g;

	ret_len = sizeof(*ret) + 1 + 1 + 1;
	ret = _os_kmem_alloc(drv, (u32)ret_len);
	if (ret == NULL) {
		PHL_ERR("%s: alloc ret failed!\n", __func__);
		goto exit;
	}

	regu_2g = rtw_hal_get_pw_lmt_regu_type(phl_info->hal, BAND_ON_24G);
	regu_5g = rtw_hal_get_pw_lmt_regu_type(phl_info->hal, BAND_ON_5G);
	regu_6g = rtw_hal_get_pw_lmt_regu_type(phl_info->hal, BAND_ON_6G);

	phl_set_embed_pwr_lmt_regu_info(drv, ret, ((u8 *)ret) + sizeof(*ret)
		, false, &regu_2g, 1, &regu_5g, 1, &regu_6g, 1);

exit:
	return ret;
}

static struct txpwr_regu_info_t *phl_get_embed_force_pwr_lmt_regu_info(void *drv
	, const struct txpwr_regu_info_t *src)
{
	struct txpwr_regu_info_t *ret;
	size_t ret_len;

	ret_len = sizeof(*ret)
		+ src->regu_2g_len
		+ src->regu_5g_len
		+ src->regu_6g_len;

	ret = _os_kmem_alloc(drv, (u32)ret_len);
	if (ret == NULL) {
		PHL_ERR("%s: alloc ret failed!\n", __func__);
		goto exit;
	}

	phl_set_embed_pwr_lmt_regu_info(drv, ret, ((u8 *)ret) + sizeof(*ret)
		, src->force
		, src->regu_2g, src->regu_2g_len
		, src->regu_5g, src->regu_5g_len
		, src->regu_6g, src->regu_6g_len);

exit:
	return ret;
}

struct txpwr_regu_info_t *rtw_phl_get_pw_lmt_regu_info(void *phl)
{
	struct phl_info_t *phl_info = phl;
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct txpwr_regu_info_t *pr_info = &phl_com->txpwr_regu_info;
	struct txpwr_regu_info_t *ret;

	if (pr_info->force)
		ret = phl_get_embed_force_pwr_lmt_regu_info(drv, pr_info);
	else
		ret = phl_get_embed_auto_pwr_lmt_regu_info(drv, phl_info);

	return ret;
}

void rtw_phl_free_pw_lmt_regu_info(void *phl, struct txpwr_regu_info_t *info)
{
	struct phl_info_t *phl_info = phl;
	void *drv = phl_to_drvpriv(phl_info);
	size_t len;

	len = sizeof(*info);
	len += info->regu_2g_len
		+ info->regu_5g_len
		+ info->regu_6g_len;

	_os_kmem_free(drv, info, (u32)len);
}

static bool phl_set_pw_lmt_regu(struct phl_info_t *phl_info, struct txpwr_regu_info_t *conf, const char *caller)
{
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct txpwr_regu_info_t *pr_info = &phl_com->txpwr_regu_info;
	u8 *tmp_regu_2g = NULL, *tmp_regu_5g = NULL, *tmp_regu_6g = NULL;
	bool config_change = false;

	if (conf->force) {
		if (conf->regu_2g_len && conf->regu_2g_len != pr_info->regu_2g_len) {
			tmp_regu_2g = _os_kmem_alloc(drv, conf->regu_2g_len);
			if (!tmp_regu_2g) {
				PHL_ERR("%s: alloc tmp_regu_2g failed!\n", __func__);
				goto exit;
			}
		}
		if (conf->regu_5g_len && conf->regu_5g_len != pr_info->regu_5g_len) {
			tmp_regu_5g = _os_kmem_alloc(drv, conf->regu_5g_len);
			if (!tmp_regu_5g) {
				PHL_ERR("%s: alloc tmp_regu_5g failed!\n", __func__);
				goto exit;
			}
		}
		if (conf->regu_6g_len && conf->regu_6g_len != pr_info->regu_6g_len) {
			tmp_regu_6g = _os_kmem_alloc(drv, conf->regu_6g_len);
			if (!tmp_regu_6g) {
				PHL_ERR("%s: alloc tmp_regu_6g failed!\n", __func__);
				goto exit;
			}
		}

		if (conf->regu_2g_len != pr_info->regu_2g_len
			|| _os_mem_cmp(drv, conf->regu_2g, pr_info->regu_2g, conf->regu_2g_len)
		) {
			config_change = true;
			if (pr_info->regu_2g && conf->regu_2g_len != pr_info->regu_2g_len)
				_os_kmem_free(drv, pr_info->regu_2g, pr_info->regu_2g_len);
			if (tmp_regu_2g) {
				pr_info->regu_2g = tmp_regu_2g;
				pr_info->regu_2g_len = conf->regu_2g_len;
			} else if (!conf->regu_2g_len) {
				pr_info->regu_2g = NULL;
				pr_info->regu_2g_len = 0;
			}
			if (conf->regu_2g_len)
				_os_mem_cpy(drv, pr_info->regu_2g, conf->regu_2g, conf->regu_2g_len);
		}
		if (conf->regu_5g_len != pr_info->regu_5g_len
			|| _os_mem_cmp(drv, conf->regu_5g, pr_info->regu_5g, conf->regu_5g_len)
		) {
			config_change = true;
			if (pr_info->regu_5g && conf->regu_5g_len != pr_info->regu_5g_len)
				_os_kmem_free(drv, pr_info->regu_5g, pr_info->regu_5g_len);
			if (tmp_regu_5g) {
				pr_info->regu_5g = tmp_regu_5g;
				pr_info->regu_5g_len = conf->regu_5g_len;
			} else if (!conf->regu_5g_len) {
				pr_info->regu_5g = NULL;
				pr_info->regu_5g_len = 0;
			}
			if (conf->regu_5g_len)
				_os_mem_cpy(drv, pr_info->regu_5g, conf->regu_5g, conf->regu_5g_len);
		}
		if (conf->regu_6g_len != pr_info->regu_6g_len
			|| _os_mem_cmp(drv, conf->regu_6g, pr_info->regu_6g, conf->regu_6g_len)
		) {
			config_change = true;
			if (pr_info->regu_6g && conf->regu_6g_len != pr_info->regu_6g_len)
				_os_kmem_free(drv, pr_info->regu_6g, pr_info->regu_6g_len);
			if (tmp_regu_6g) {
				pr_info->regu_6g = tmp_regu_6g;
				pr_info->regu_6g_len = conf->regu_6g_len;
			} else if (!conf->regu_6g_len) {
				pr_info->regu_6g = NULL;
				pr_info->regu_6g_len = 0;
			}
			if (conf->regu_6g_len)
				_os_mem_cpy(drv, pr_info->regu_6g, conf->regu_6g, conf->regu_6g_len);
		}

	} else {
		if (pr_info->regu_2g) {
			_os_kmem_free(drv, pr_info->regu_2g, pr_info->regu_2g_len);
			pr_info->regu_2g = NULL;
			pr_info->regu_2g_len = 0;
		}
		if (pr_info->regu_5g) {
			_os_kmem_free(drv, pr_info->regu_5g, pr_info->regu_5g_len);
			pr_info->regu_5g = NULL;
			pr_info->regu_5g_len = 0;
		}
		if (pr_info->regu_6g) {
			_os_kmem_free(drv, pr_info->regu_6g, pr_info->regu_6g_len);
			pr_info->regu_6g = NULL;
			pr_info->regu_6g_len = 0;
		}
	}

	if (conf->force != pr_info->force) {
		config_change = true;
		pr_info->force = conf->force;
		PHL_INFO("%s txpwr_regu_info force:%u is set\n", caller, conf->force);
	}

	if (!config_change)
		goto exit;

	if (!pr_info->force)
		rtw_hal_auto_pw_lmt_regu(phl_info->hal);
	else {
		PHL_INFO("%s txpwr_regu_info regu_2g_len:%u is set\n", caller, conf->regu_2g_len);
		PHL_INFO("%s txpwr_regu_info regu_5g_len:%u is set\n", caller, conf->regu_5g_len);
		PHL_INFO("%s txpwr_regu_info regu_6g_len:%u is set\n", caller, conf->regu_6g_len);
		rtw_hal_force_pw_lmt_regu(phl_info->hal, pr_info->regu_2g, pr_info->regu_2g_len
			, pr_info->regu_5g, pr_info->regu_5g_len, pr_info->regu_6g, pr_info->regu_6g_len);
	}

exit:
	if (!config_change) {
		if (tmp_regu_2g)
			_os_kmem_free(drv, tmp_regu_2g, conf->regu_2g_len);
		if (tmp_regu_5g)
			_os_kmem_free(drv, tmp_regu_5g, conf->regu_5g_len);
		if (tmp_regu_6g)
			_os_kmem_free(drv, tmp_regu_6g, conf->regu_6g_len);
	}

	return config_change;
}

static u16 phl_get_pwr_constraint(struct phl_info_t *phl_info, u8 band_idx)
{
	return rtw_hal_get_pwr_constraint(phl_info->hal, band_idx);
}

static enum rtw_phl_status phl_set_pwr_constraint(struct phl_info_t *phl_info, u8 band_idx, u16 mb)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	hstatus = rtw_hal_set_pwr_constraint(phl_info->hal, band_idx, mb);
	if (hstatus != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("%s rtw_hal_set_pwr_constraint: statuts = %u\n", __func__, hstatus);

	return hstatus == RTW_HAL_STATUS_SUCCESS ? RTW_PHL_STATUS_SUCCESS : RTW_PHL_STATUS_FAILURE;
}

static enum rtw_phl_status phl_set_tx_power(void *phl, u8 band_idx)
{
	struct phl_info_t *phl_info = phl;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	hstatus = rtw_hal_set_tx_power(phl_info->hal, band_idx, PWR_BY_RATE | PWR_LIMIT | PWR_LIMIT_RU);
	if (hstatus != RTW_HAL_STATUS_SUCCESS)
		PHL_ERR("%s rtw_hal_set_tx_power: statuts = %u\n", __func__, hstatus);

	return hstatus == RTW_HAL_STATUS_SUCCESS ? RTW_PHL_STATUS_SUCCESS : RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
phl_cmd_txpwr_ctl_hdl(void *phl, u8 *param_buf)
{
	struct phl_info_t *phl_info = phl;
	struct txpwr_ctl_param *param = (struct txpwr_ctl_param *)param_buf;
	u8 band_idx = param->band_idx;
	bool config_change = false;
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;

	if (param->pr_info_config)
		config_change = phl_set_pw_lmt_regu(phl_info, &param->pr_info, __func__);

	if (param->constraint_mb >= 0) {
		u16 constraint_mb = (u16)param->constraint_mb;

		if (constraint_mb != phl_get_pwr_constraint(phl_info, band_idx)) {
			psts = phl_set_pwr_constraint(phl_info, band_idx, constraint_mb);
			if (psts != RTW_PHL_STATUS_SUCCESS)
				goto exit;
			PHL_INFO("%s constraint_mb:%u is set\n", __func__, constraint_mb);
			config_change = true;
		}
	}

	if (param->force_write_txpwr && param->force_sw_config) {
		_os_warn_on(1);
		param->force_sw_config = false;
	}

	if (param->force_sw_config)
		goto exit;

	if (param->force_write_txpwr || config_change)
		psts = phl_set_tx_power(phl_info, band_idx);

exit:
	return psts;
}

static void phl_txpwr_ctl_done(void *drv_priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status)
{
	if (cmd) {
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
	}
}

enum rtw_phl_status
rtw_phl_cmd_txpwr_ctl(void *phl, struct txpwr_ctl_param *args
	, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
#ifdef CONFIG_CMD_DISP
	struct phl_info_t *phl_info = phl;
	void *drv = phl_to_drvpriv(phl_info);
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct txpwr_ctl_param *param = NULL;
	u32 param_len;

	param_len = sizeof(*param);
	if (args->pr_info_config) {
		param_len += args->pr_info.regu_2g_len
			+ args->pr_info.regu_5g_len
			+ args->pr_info.regu_6g_len;
	}
	param = _os_kmem_alloc(drv, param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		goto _exit;
	}

	_os_mem_cpy(drv, param, args, sizeof(*param));
	if (args->pr_info_config) {
		phl_set_embed_force_pwr_lmt_regu_info(drv
			, &param->pr_info, ((u8 *)param) + sizeof(*param)
			, &args->pr_info);
	}

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = phl_cmd_txpwr_ctl_hdl(phl_info, (u8 *)param);
		phl_txpwr_ctl_done(drv, (u8 *)param, param_len, psts);
		goto _exit;
	}

	psts = phl_cmd_enqueue(phl_info,
			args->band_idx,
			MSG_EVT_TXPWR_SETUP,
			(u8 *)param, param_len,
			phl_txpwr_ctl_done,
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
	PHL_ERR("%s(), CONFIG_CMD_DISP need to be enabled for MSG_EVT_TXPWR_SETUP \n",__func__);
	return RTW_PHL_STATUS_FAILURE;
#endif
}

enum rtw_phl_status
rtw_phl_cmd_set_pw_lmt_regu(void *phl, struct txpwr_regu_info_t *conf, bool sw_conf_only
	, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	struct phl_info_t *phl_info = phl;
	void *drv = phl_to_drvpriv(phl_info);
	struct txpwr_ctl_param args;

	txpwr_ctl_param_init(&args);
	args.band_idx = HW_BAND_0;
	args.force_sw_config = sw_conf_only;
	args.pr_info_config = true;
	_os_mem_cpy(drv, &args.pr_info, conf, sizeof(args.pr_info));

	return rtw_phl_cmd_txpwr_ctl(phl, &args
		, cmd_type, cmd_timeout);
}

enum rtw_phl_status
rtw_phl_cmd_set_tx_power_constraint(void *phl, enum phl_band_idx band_idx, u16 mb
	, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	struct txpwr_ctl_param args;

	txpwr_ctl_param_init(&args);
	args.band_idx = band_idx;
	args.constraint_mb = mb;

	return rtw_phl_cmd_txpwr_ctl(phl, &args
		, cmd_type, cmd_timeout);
}

enum rtw_phl_status
rtw_phl_cmd_set_tx_power(void *phl, enum phl_band_idx band_idx
	, enum phl_cmd_type cmd_type, u32 cmd_timeout)
{
	struct txpwr_ctl_param args;

	txpwr_ctl_param_init(&args);
	args.band_idx = band_idx;
	args.force_write_txpwr = true;

	return rtw_phl_cmd_txpwr_ctl(phl, &args
		, cmd_type, cmd_timeout);
}

enum rtw_phl_status rtw_phl_get_txinfo_pwr(void *phl, s16 *pwr_dbm)
{
	struct phl_info_t *phl_info = phl;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	s16 power_dbm = 0;

	hstatus = rtw_hal_get_txinfo_power(phl_info->hal, &power_dbm);
	*pwr_dbm = power_dbm;

	return hstatus == RTW_HAL_STATUS_SUCCESS ? RTW_PHL_STATUS_SUCCESS : RTW_PHL_STATUS_FAILURE;
}

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
rtw_phl_cmd_get_txinfo_pwr(void *phl, s16 *pwr_dbm,
				enum phl_band_idx band_idx,
				bool direct) /* if caller already in cmd/msg, use direct = true */
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

	if (direct) {
		psts = rtw_phl_get_txinfo_pwr(phl, pwr_dbm);
		goto exit;
	}

	psts = phl_cmd_enqueue(phl_info,
				band_idx,
				MSG_EVT_GET_TX_PWR_DBM,
				(u8*)pwr_dbm,
				sizeof(s16),
				NULL,
				PHL_CMD_WAIT,
				0);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
	}

exit:
	return psts;
}
#else
enum rtw_phl_status
rtw_phl_cmd_get_txinfo_pwr(void *phl, s16 *pwr_dbm,
				enum phl_band_idx band_idx,
				bool direct)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_phl_get_txinfo_pwr(phl, pwr_dbm);
}
#endif

static enum rtw_phl_status
_update_tpe_info(void *phl,
	struct rtw_tpe_info_t *tpe_info)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	if (_os_mem_cmp(phl_to_drvpriv(phl_info),
			&phl_info->phl_com->tpe_info,
			tpe_info,
			sizeof(struct rtw_tpe_info_t)) != 0) {
		/* Update tpe info if contents are not the same */
		_os_mem_cpy(phl_to_drvpriv(phl_info),
			&phl_info->phl_com->tpe_info,
			tpe_info,
			sizeof(struct rtw_tpe_info_t));
	}
	return RTW_PHL_STATUS_SUCCESS;
}

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_cmd_tpe_update_hdl(void *phl, u8 *param)
{
	struct rtw_tpe_info_t *tpe_info = (struct rtw_tpe_info_t *)param;
	return _update_tpe_info(phl, tpe_info);
}

static void
_phl_tpe_update_done(void *drv_priv, u8 *cmd, u32 cmd_len,
			enum rtw_phl_status status)
{
	if (cmd) {
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}

enum rtw_phl_status
rtw_phl_cmd_tpe_update(struct rtw_wifi_role_link_t *rlink,
                       struct rtw_tpe_info_t *tpe_info,
                       bool cmd_wait,
                       u32 cmd_timeout)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl = rlink->wrole->phl_com->phl_priv;
	void *drv = phl_to_drvpriv(phl);
	u32 param_len = sizeof(struct rtw_tpe_info_t);
	struct rtw_tpe_info_t *param = _os_kmem_alloc(drv, param_len);

	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		goto _exit;
	}
	_os_mem_cpy(drv, param, tpe_info,
			sizeof(struct rtw_tpe_info_t));
	psts = phl_cmd_enqueue(phl,
	                       rlink->hw_band,
	                       MSG_EVT_TPE_INFO_UPDATE,
	                       (u8 *)param,
	                       param_len,
	                       _phl_tpe_update_done,
	                       cmd_wait ? PHL_CMD_WAIT : PHL_CMD_NO_WAIT,
	                       cmd_timeout);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_INFO("%s: Fail to issue tpe info update!!\n",
			__func__);
		if (!is_cmd_failure(psts)) {
			/* Send cmd fail */
			_os_kmem_free(drv, param, param_len);
			psts = RTW_PHL_STATUS_FAILURE;
		}
		goto _exit;
	}
_exit:
	PHL_INFO("%s: Issue cmd, status(%d)\n", __func__, psts);
	return psts;
}

void
rtw_phl_set_tx_pwr_comp(void *phl, u8 regu, s8 ag_comp_2g,
	s8 ag_comp_5g, s8 ag_comp_6g)
{
	struct	phl_info_t *phl_info = (struct phl_info_t *)phl;
	u8 tx_tbl_to_pwr_times = rtw_hal_get_tx_tbl_to_pwr_times(phl_info->hal);
	enum phl_phy_idx phy_idx = HW_PHY_0;
	struct rtw_phl_regu_dyn_ant_gain dyn_ag;


	PHL_INFO("%s...\n", __func__);

	dyn_ag.regu = regu;
	dyn_ag.ag_2g_comp = ag_comp_2g * tx_tbl_to_pwr_times;
	dyn_ag.ag_5g_comp = ag_comp_5g * tx_tbl_to_pwr_times;
	dyn_ag.ag_6g_comp = ag_comp_6g * tx_tbl_to_pwr_times;

	rtw_hal_set_tx_pwr_comp(phl_info->hal, phy_idx, &dyn_ag);

}

#endif
