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
#define _PHL_TEST_DBCC_C_
#include "../../../phl_headers.h"
#include "../phl_test_verify_def.h"
#include "../phl_test_verify_api.h"
#include "phl_test_dbcc_def.h"

#ifdef CONFIG_PHL_TEST_VERIFY
static u8 dbcc_get_class_from_buf(struct verify_context *ctx)
{
	u8 *buf_tmp = NULL;
	u8 dbcc_class = DBCC_CLASS_MAX;

	if (ctx && ctx->buf &&
		(ctx->buf_len > VERIFY_CMD_HDR_SIZE)) {
		buf_tmp	= (u8 *)VERIFY_GET_SUBUF((u8 *)ctx->buf);
		dbcc_class = buf_tmp[0];
	}
	return dbcc_class;
}

#ifdef CONFIG_DBCC_SUPPORT
static enum rtw_phl_status
phl_dbcc_test(void *phl, void *param)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
	bool dbcc_en = *(bool *)param;

	PHL_INFO("[DBCC] %s: %s\n", __func__, (dbcc_en) ? "EN" : "DIS");


	hsts = rtw_hal_dbcc_pre_cfg(phl_info->hal, phl_info->phl_com, dbcc_en);
	if (hsts != RTW_HAL_STATUS_SUCCESS) {
		PHL_INFO("[DBCC] %s: pre_cfg fail\n", __func__ );
		psts = RTW_PHL_STATUS_FAILURE;
		goto exit;
	}

	hsts = rtw_hal_dbcc_cfg(phl_info->hal, phl_info->phl_com, dbcc_en);
	if (hsts != RTW_HAL_STATUS_SUCCESS) {
		PHL_INFO("[DBCC] %s: cfg fail\n", __func__ );
		psts = RTW_PHL_STATUS_FAILURE;
		goto exit;
	}

	if (dbcc_en == true) {
		hsts = rtw_hal_dbcc_reset_hal(phl_info->hal);\
		if (hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_INFO("[DBCC] %s: reset fail\n", __func__ );
			psts = RTW_PHL_STATUS_FAILURE;
			goto exit;
		}
	}

exit:
	return psts;
}
#endif /* CONFIG_DBCC_SUPPORT */

static enum rtw_phl_status
dbcc_config(struct verify_context *ctx, struct dbcc_config_arg *arg)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = ctx->phl;

	if (arg == NULL) {
		return phl_status;
	}

	PHL_INFO("%s: en %u id %u\n", __FUNCTION__, arg->dbcc_en, arg->macid);
#ifdef CONFIG_DBCC_SUPPORT
	//if (phl_info->phl_com->dev_cap.dbcc_sup)
	phl_status = phl_dbcc_test(phl_info, &arg->dbcc_en);
#else
	PHL_ERR("%s: %s No Support DBCC\n", __FUNCTION__,
			phl_info->phl_com->hal_spec.ic_name);
	phl_status = RTW_PHL_STATUS_SUCCESS;
#endif
	return phl_status;
}

enum rtw_phl_status rtw_test_dbcc_cmd_process(void *priv)
{
	struct verify_context *ctx = NULL;
	struct rtw_phl_com_t *phl_com = NULL;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	u8 dbcc_class = DBCC_CLASS_MAX;

	FUNCIN();

	if (priv == NULL)
		return phl_status;

	ctx = (struct verify_context *)priv;
	phl_com = ctx->phl_com;

	if ((ctx->buf_len < VERIFY_CMD_HDR_SIZE)) {
		PHL_ERR("%s: Invalid buffer content!\n", __FUNCTION__);
		return phl_status;
	}

	dbcc_class = dbcc_get_class_from_buf(ctx);
	switch (dbcc_class) {
	case DBCC_CLASS_CONFIG:
		PHL_INFO("%s: class = DBCC_CLASS_CONFIG\n", __FUNCTION__);
		phl_status = dbcc_config(ctx, (struct dbcc_config_arg *)VERIFY_GET_SUBUF((u8 *)ctx->buf));
		break;
	default:
		PHL_WARN("%s: Unknown DBCC_CLASS! (%d)\n", __FUNCTION__, dbcc_class);
		break;
	}

	FUNCOUT();

	return phl_status;
}
#else
enum rtw_phl_status rtw_test_dbcc_cmd_process(void *priv)
{
	return RTW_PHL_STATUS_SUCCESS;
}
#endif /* CONFIG_PHL_TEST_VERIFY */
