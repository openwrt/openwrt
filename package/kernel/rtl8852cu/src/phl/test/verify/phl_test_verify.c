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
#define _PHL_TEST_VERIFY_C_
#include "../../phl_headers.h"
#include "phl_test_verify_def.h"
#include "phl_test_verify_api.h"
#include "./dbcc/phl_test_dbcc_api.h"

#ifdef CONFIG_PHL_TEST_VERIFY

static struct s_handler test_handlers[VERIFY_FEATURES_MAX] = {
	{ func_null }, /* VERIFY_FEATURES_NULL = 0 */
	{ rtw_test_dbcc_cmd_process }, /* VERIFY_FEATURES_DBCC */
};

void verify_cmd_done_notification_complete(void *ctx, struct phl_msg *msg)
{
	struct verify_context *ver_ctx = (struct verify_context *)ctx;

	if (msg->inbuf) {
		PHL_INFO("%s: Free info buf\n", __FUNCTION__);
		_os_mem_free(ver_ctx->phl_com->drv_priv, msg->inbuf, msg->inlen);
	}
}

void verify_cmd_done_notification(struct verify_context *ver_ctx, u8 feature,
	u8 cmd_id)
{
	struct phl_msg msg = { 0 };
	struct phl_msg_attribute attr = { 0 };
	u8 *info = NULL;

	info = _os_mem_alloc(ver_ctx->phl_com->drv_priv, 2);
	if (info == NULL) {
		PHL_ERR("%s: Allocate msg hub buffer fail!\n", __FUNCTION__);
		return;
	}

	info[0] = feature;
	info[1] = cmd_id;

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FUNC_MDL_TEST_MODULE);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_MP_CMD_DONE);

	attr.completion.completion = verify_cmd_done_notification_complete;
	attr.completion.priv = ver_ctx;

	msg.inbuf = info;
	msg.inlen = 2;

	if (phl_msg_hub_send(ver_ctx->phl, &attr, &msg) != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: send msg_hub failed\n", __func__);
		_os_mem_free(ver_ctx->phl_com->drv_priv, info, 2);
	}
}

bool verify_get_rpt_check(struct verify_context *ver_ctx, void *rpt_buf)
{
	bool ret = true;
	struct verify_cmd_hdr *rpt_hdr = (struct verify_cmd_hdr *)ver_ctx->rpt;
	struct verify_cmd_hdr *rpt_buf_hdr = (struct verify_cmd_hdr *)rpt_buf;

	if ((rpt_hdr->feature != rpt_buf_hdr->feature) ||
		(rpt_hdr->cmd != rpt_buf_hdr->cmd)) {
		PHL_WARN("%s: Report buffer not match!\n", __FUNCTION__);
		rpt_buf_hdr->cmd_ok = true;
		rpt_buf_hdr->status = RTW_PHL_STATUS_FAILURE;
		ret = false;
	}

	return ret;
}

u8 verify_get_feature_from_buf(struct verify_context *ver_ctx)
{
	u8 *buf_tmp = NULL;
	u8 feature = VERIFY_FEATURES_MAX;

	if (ver_ctx && ver_ctx->buf) {
		buf_tmp	= (u8 *)ver_ctx->buf;
		feature = buf_tmp[0];
	}
	return feature;
}

u8 verify_bp_handler(void *ctx, struct test_bp_info *bp_info)
{
	struct verify_context *ver_ctx = (struct verify_context *)ctx;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	PHL_INFO("%s: bp_info->type = %x\n", __FUNCTION__, bp_info->type);

	switch (bp_info->type) {
	case BP_INFO_TYPE_MP_CMD_EVENT:
		if (ver_ctx->status == VERIFY_STATUS_WAIT_CMD) {
			ver_ctx->status = VERIFY_STATUS_CMD_EVENT;
			_os_sema_up(ver_ctx->phl_com->drv_priv, &(ver_ctx->cmd_sema));
			phl_status = RTW_PHL_STATUS_SUCCESS;
		}
	default:
		break;
	}
	return phl_status;
}

u8 verify_get_fail_rsn(void *ctx, char *rsn, u32 max_len)
{
	//struct verify_context *ver_ctx = (struct verify_context *)ctx;
	return true;
}

u8 verify_is_test_end(void *ctx)
{
	struct verify_context *ver_ctx = (struct verify_context *)ctx;

	return ver_ctx->is_test_end;
}

u8 verifyg_is_test_pass(void *ctx)
{
	//struct verify_context *ver_ctx = (struct verify_context *)ctx;
	return true;
}

u8 verify_start(void *ctx)
{
	struct verify_context *ver_ctx = (struct verify_context *)ctx;
	struct rtw_phl_com_t *phl_com = ver_ctx->phl_com;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	u8 feature = VERIFY_FEATURES_MAX;

	FUNCIN();

	/* drv_mode maybe control by MP */
	//phl_com->drv_mode = RTW_DRV_MODE_MP;
	while (!verify_is_test_end(ver_ctx)) {
		_os_sema_down(phl_com->drv_priv, &(ver_ctx->cmd_sema));
		if (ver_ctx->status == VERIFY_STATUS_CMD_EVENT) {
			ver_ctx->status = VERIFY_STATUS_RUN_CMD;
			feature = verify_get_feature_from_buf(ver_ctx);

			/* Clear report buffer before executing next command */
			if (ver_ctx->rpt != NULL) {
				PHL_INFO("%s: Report not empty, cleanup!\n", __FUNCTION__);
				_os_mem_free(phl_com->drv_priv, ver_ctx->rpt, ver_ctx->rpt_len);
				ver_ctx->rpt = NULL;
				ver_ctx->rpt_len = 0;
			}

			if (feature < VERIFY_FEATURES_MAX) {
				PHL_INFO("%s: feature id = %u\n", __FUNCTION__, feature);
				phl_status = ver_ctx->handler[feature].callback(ver_ctx);
			} else {
				PHL_WARN("%s: Unknown VERIFY_FEATURE! (%d)\n", __FUNCTION__, feature);
			}

			if (ver_ctx->rpt != NULL) {
				struct verify_cmd_hdr *hdr = (struct verify_cmd_hdr *)ver_ctx->rpt;
				verify_cmd_done_notification(ver_ctx, hdr->feature, hdr->cmd);
				PHL_INFO("%s: Indication class(%d) cmd(%d)\n",
					__FUNCTION__, hdr->feature, hdr->cmd);
			}

			/* Clear command buffer after executing the command */
			if (ver_ctx->buf != NULL) {
				PHL_INFO("%s: Command buf not empty, cleanup!\n", __FUNCTION__);
				_os_mem_free(phl_com->drv_priv, ver_ctx->buf, ver_ctx->buf_len);
				ver_ctx->buf = NULL;
				ver_ctx->buf_len = 0;
			}
			ver_ctx->status = VERIFY_STATUS_WAIT_CMD;
		}
	}

	FUNCOUT();
	return (u8)phl_status;
}

void verify_change_mode(struct verify_context *ver_ctx, enum rtw_drv_mode driver_mode)
{
	struct phl_info_t *phl_info = ver_ctx->phl;

	PHL_INFO("%s Change to %x\n", __FUNCTION__, driver_mode);

	/* Need PHL stop function later */

	phl_info->phl_com->drv_mode = driver_mode;

#ifdef RTW_WKARD_MP_MODE_CHANGE
#else
	/* Ouden : does need reset ? it already do in the mp start */
	//rtw_phl_reset(phl_info);
#endif

}

enum rtw_phl_status phl_test_verify_alloc(struct phl_info_t *phl_info, void *hal, void **ctx)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct verify_context *ver_ctx = NULL;

	ver_ctx = _os_mem_alloc(phl_com->drv_priv, sizeof(struct verify_context));

	if (ver_ctx == NULL) {
		PHL_ERR("alloc verify context failed\n");
		phl_status = RTW_PHL_STATUS_RESOURCE;
		goto exit;
	}
	_os_sema_init(phl_com->drv_priv, &(ver_ctx->cmd_sema), 0);
	ver_ctx->cur_phy = HW_PHY_0;
	ver_ctx->phl = phl_info;
	ver_ctx->phl_com = phl_com;
	ver_ctx->hal = hal;
	ver_ctx->status = VERIFY_STATUS_INIT;
	ver_ctx->handler = test_handlers;
	*ctx = ver_ctx;
	phl_status = RTW_PHL_STATUS_SUCCESS;

exit:
	return phl_status;
}

void phl_test_verify_free(void **ctx)
{
	struct verify_context *ver_ctx = NULL;

	if (*ctx == NULL)
		return;

	ver_ctx = (struct verify_context *)(*ctx);
	_os_sema_free(ver_ctx->phl_com->drv_priv, &(ver_ctx->cmd_sema));
	_os_mem_free(ver_ctx->phl_com->drv_priv, ver_ctx, sizeof(struct verify_context));
	ver_ctx = NULL;
	*ctx = NULL;
}

void phl_test_verify_init(void *ctx)
{
	struct verify_context *ver_ctx = NULL;
	struct test_obj_ctrl_interface *pCtrl = NULL;

	if (ctx == NULL)
		return;

	ver_ctx = (struct verify_context *)ctx;
	pCtrl = &(ver_ctx->test_ctrl);

	ver_ctx->max_para = 30;
	ver_ctx->status = VERIFY_STATUS_WAIT_CMD;
	ver_ctx->is_test_end = false;
	pCtrl->bp_handler = verify_bp_handler;
	pCtrl->get_fail_rsn = verify_get_fail_rsn;
	pCtrl->is_test_end = verify_is_test_end;
	pCtrl->is_test_pass = verifyg_is_test_pass;
	pCtrl->start_test = verify_start;
	rtw_phl_test_add_new_test_obj(ver_ctx->phl_com,
	                              "verify_test",
	                              ver_ctx,
	                              TEST_LVL_LOW,
	                              pCtrl,
	                              -1,
	                              TEST_SUB_MODULE_VERIFY,
	                              INTGR_TEST_MODE);
}

void phl_test_verify_deinit(void *ctx)
{
	struct verify_context *ver_ctx = NULL;
	if (ctx == NULL)
		return;

	ver_ctx = (struct verify_context *)ctx;

	if (ver_ctx->status < VERIFY_STATUS_WAIT_CMD)
		return;

	ver_ctx->is_test_end = true;
	_os_sema_up(ver_ctx->phl_com->drv_priv, &(ver_ctx->cmd_sema));
	ver_ctx->status = VERIFY_STATUS_INIT;
}

void phl_test_verify_start(void *ctx)
{
	struct verify_context *ver_ctx = NULL;

	if (ctx == NULL)
		return;

	ver_ctx = (struct verify_context *)ctx;

#if 0	/* verify keep in current mode */
	verify_change_mode(ver_ctx, RTW_DRV_MODE_MP);
#endif
}

void phl_test_verify_stop(void *ctx)
{
	struct verify_context *ver_ctx = NULL;

	if (ctx == NULL)
		return;

	ver_ctx = (struct verify_context *)ctx;

	if (ver_ctx->status < VERIFY_STATUS_WAIT_CMD)
		return;
#if 0	/* verify keep in current mode */
	verify_change_mode(ver_ctx, RTW_DRV_MODE_NORMAL);
#endif
}

void phl_test_verify_cmd_process(void *ctx, void *buf, u32 buf_len, u8 submdid)
{
	struct verify_context *ver_ctx = NULL;
	struct rtw_phl_com_t *phl_com = NULL;
	struct test_bp_info bp_info;

	FUNCIN();

	if (ctx == NULL)
		return;

	ver_ctx = (struct verify_context *)ctx;
	phl_com = ver_ctx->phl_com;

	if ((buf == NULL) || (buf_len > ver_ctx->max_para)) {
		PHL_ERR("%s: Invalid buffer content!\n", __func__);
		return;
	}

	if (ver_ctx->status == VERIFY_STATUS_WAIT_CMD) {
		ver_ctx->buf_len = buf_len;
		ver_ctx->buf = _os_mem_alloc(phl_com->drv_priv, buf_len);
		_os_mem_cpy(phl_com->drv_priv, ver_ctx->buf, buf, buf_len);
		_os_mem_set(phl_com->drv_priv, &bp_info, 0, sizeof(struct test_bp_info));
		bp_info.type = BP_INFO_TYPE_MP_CMD_EVENT;
		rtw_phl_test_setup_bp(phl_com, &bp_info, submdid);
	} else {
		PHL_WARN("%s: Previous command is still running!\n", __FUNCTION__);
	}

	FUNCOUT();
}

void phl_test_verify_get_rpt(void *ctx, void *buf, u32 buf_len)
{
	struct verify_context *ver_ctx = NULL;
	FUNCIN();

	if (ctx == NULL) {
		PHL_WARN("%s: mp is NULL!\n", __FUNCTION__);
		goto exit;
	}

	ver_ctx = (struct verify_context *)ctx;

	if (ver_ctx->status != VERIFY_STATUS_WAIT_CMD) {
		PHL_WARN("%s: command is running!\n", __FUNCTION__);
		goto exit;
	}

	if (ver_ctx->rpt == NULL) {
		PHL_DBG("%s: ver_ctx->rpt  is NULL!\n", __FUNCTION__);
		goto exit;
	}

	if (buf_len < ver_ctx->rpt_len) {
		PHL_WARN("%s: buffer not enough!\n", __FUNCTION__);
		goto exit;
	}

	if (verify_get_rpt_check(ver_ctx, buf) == true) {
		_os_mem_cpy(ver_ctx->phl_com->drv_priv, buf, ver_ctx->rpt, ver_ctx->rpt_len);
		_os_mem_free(ver_ctx->phl_com->drv_priv, ver_ctx->rpt, ver_ctx->rpt_len);
		ver_ctx->rpt = NULL;
		ver_ctx->rpt_len = 0;
	}

exit:
	FUNCOUT();
}
#endif /* CONFIG_PHL_TEST_MP */
