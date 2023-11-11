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
#define _PHL_CUSTOMIZE_FEATURE_C_
#include "../phl_headers.h"

#ifdef CONFIG_PHL_CUSTOM_FEATURE_VR
#include "phl_custom_vr.h"

#define LLC_HDR_LENGTH                  6
#define SNAP_HDR_LENGTH                 2

enum phl_mdl_ret_code
_is_vr_mode_valid(void* custom_ctx,
                  struct _custom_vr_ctx* vr_ctx,
                  struct phl_msg* msg, u32 size)
{
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);
	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;
	if (!vr_ctx->init.enable || cmd->len < size || vr_ctx->init.wifi_role == NULL) {
		PHL_INFO(" %s, evt_id(%d) not accepted\n",
		         __FUNCTION__,
		         MSG_EVT_ID_FIELD(msg->msg_id));
		ret = MDL_RET_FAIL;
		return ret;
	}

	return ret;
}

bool
_feature_vr_enable_init_setting(void* custom_ctx,
                             struct _custom_vr_ctx* vr_ctx)
{
	struct phl_info_t *phl = phl_custom_get_phl_info(custom_ctx);

	vr_ctx->init.phl = phl;

	return true;
}

void
_feature_vr_enable_deinit_setting(void* custom_ctx,
                               struct _custom_vr_ctx* vr_ctx)
{
	/* TBD: merge codes from custom branch for the deinit setting*/

	return;
}

enum phl_mdl_ret_code
_phl_custom_vr_feature_enable(void* custom_ctx,
                          struct _custom_vr_ctx* vr_ctx,
                          struct phl_msg* msg)
{
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);

	if (cmd->len < sizeof(u32))
		return MDL_RET_FAIL;

	vr_ctx->init.enable = *(u32*)(cmd->data);
	vr_ctx->init.test_mode = 0;
	if (vr_ctx->init.enable){
		_feature_vr_enable_init_setting(custom_ctx, vr_ctx);
	} else {
		_feature_vr_enable_deinit_setting(custom_ctx, vr_ctx);
	}

	PHL_INFO(" %s, vr feature enable(%d)\n", __FUNCTION__, vr_ctx->init.enable);
	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&(vr_ctx->init.enable),
	                           sizeof(u32));

	return MDL_RET_SUCCESS;
}

enum phl_mdl_ret_code
_phl_custom_vr_feature_query(void* custom_ctx,
                             struct _custom_vr_ctx* vr_ctx,
                             struct phl_msg* msg)
{
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	if (cmd->len < sizeof(u32))
		return ret;
	PHL_INFO("%s, vr query feature enable(%d)\n",
	         __FUNCTION__,
	         vr_ctx->init.enable);
	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&(vr_ctx->init.enable),
	                           sizeof(u32));
	ret = MDL_RET_SUCCESS;
	return ret;
}

enum phl_mdl_ret_code
_phl_custom_vr_testmode_param(void* custom_ctx,
                              struct _custom_vr_ctx* vr_ctx,
                              struct phl_msg* msg)
{
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	if (cmd->len < sizeof(u32))
		return ret;
	vr_ctx->init.test_mode = *(u32*)(cmd->data);
	PHL_INFO("%s, test mode(0x%x)\n", __FUNCTION__,
	         vr_ctx->init.test_mode);
	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&ret,
	                           sizeof(u8));

	ret = MDL_RET_SUCCESS;
	return ret;
}

enum phl_mdl_ret_code
_phl_custom_vr_ampdu_cfg(void* custom_ctx,
                         struct _custom_vr_ctx* vr_ctx,
                         struct phl_msg* msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct phl_info_t *phl = phl_custom_get_phl_info(custom_ctx);
	void *d = phl_to_drvpriv(phl);
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);
	struct rtw_phl_custom_ampdu_cfg custom_vr_ampdu_cfg = {0};
	u32 size = sizeof(struct rtw_phl_custom_ampdu_cfg);
	u8* val = cmd->data;
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	ret = _is_vr_mode_valid(custom_ctx, vr_ctx, msg, size);
	if (ret != MDL_RET_SUCCESS) {
		goto exit;
	}

	wrole = vr_ctx->init.wifi_role;
	_os_mem_cpy(d, &custom_vr_ampdu_cfg, val, size);

	PHL_INFO("%s, halsta(%d) ampdu dur(%d) num(%d)\n",
	         __FUNCTION__,
	         hal_status,
	         custom_vr_ampdu_cfg.max_agg_time_32us,
	         custom_vr_ampdu_cfg.max_agg_num);

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		hal_status = rtw_hal_custom_cfg_tx_ampdu(phl->hal,
		                                         rlink,
		                                         &custom_vr_ampdu_cfg);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			ret = MDL_RET_FAIL;
	}

exit:
	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&ret,
	                           sizeof(u8));
	return ret;
}

enum phl_mdl_ret_code
_phl_custom_vr_ampdu_query(void* custom_ctx,
                           struct _custom_vr_ctx* vr_ctx,
                           struct phl_msg* msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct phl_info_t *phl = phl_custom_get_phl_info(custom_ctx);
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);
	struct rtw_phl_custom_ampdu_cfg custom_vr_ampdu_cfg = {0};
	u32 size = sizeof(struct rtw_phl_custom_ampdu_cfg);
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	ret = _is_vr_mode_valid(custom_ctx, vr_ctx, msg, size);
	if (ret != MDL_RET_SUCCESS) {
		goto exit;
	}

	wrole = vr_ctx->init.wifi_role;

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		hal_status = rtw_hal_get_ampdu_cfg(phl->hal,
		                                   rlink,
		                                   &custom_vr_ampdu_cfg);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			ret = MDL_RET_FAIL;
	}

	PHL_INFO(" %s, ampdu dur(%d) time(%d)\n",
	         __FUNCTION__,
	         custom_vr_ampdu_cfg.max_agg_time_32us,
	         custom_vr_ampdu_cfg.max_agg_num);
exit:
	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&custom_vr_ampdu_cfg,
	                           sizeof(struct rtw_phl_custom_ampdu_cfg));
	return ret;
}

enum phl_mdl_ret_code
_phl_custom_vr_pdthr_cfg(void* custom_ctx,
                         struct _custom_vr_ctx* vr_ctx,
                         struct phl_msg* msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct phl_info_t *phl = phl_custom_get_phl_info(custom_ctx);
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	int pd_thr = 0xff;
	u32 size = sizeof(int);

	ret = _is_vr_mode_valid(custom_ctx, vr_ctx, msg, size);
	if (ret != MDL_RET_SUCCESS) {
		goto exit;
	}

	wrole = vr_ctx->init.wifi_role;
	rlink = get_rlink(wrole, RTW_RLINK_PRIMARY);

	pd_thr = *(int*)(cmd->data);
	PHL_INFO("%s, pd_thr(%d)\n", __FUNCTION__, pd_thr);

	hal_status = rtw_hal_set_pkt_detect_thold(phl->hal,
						rlink->hw_band,
						(u32)pd_thr);
	PHL_INFO("%s, hal_status(%d)\n", __FUNCTION__, hal_status);
	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		ret = MDL_RET_FAIL;
exit:
	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&ret,
	                           sizeof(u8));
	return ret;
}

enum phl_mdl_ret_code
_phl_custom_vr_pdthr_query(void* custom_ctx,
                           struct _custom_vr_ctx* vr_ctx,
                           struct phl_msg* msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl = phl_custom_get_phl_info(custom_ctx);
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	int pd_thr = 0xff;
	u32 size = sizeof(int);

	ret = _is_vr_mode_valid(custom_ctx, vr_ctx, msg, size);
	if (ret != MDL_RET_SUCCESS) {
		goto exit;
	}

	wrole = vr_ctx->init.wifi_role;
	rlink = get_rlink(wrole, RTW_RLINK_PRIMARY);

	/*confirm whether pd thr is enabling or not*/
	pd_thr = rtw_hal_query_pkt_detect_thold(phl->hal,
	                                        true,
	                                        rlink->hw_band);
	if (pd_thr == 0) {
		PHL_INFO("%s, disable! pd_thr(%d)\n", __FUNCTION__, pd_thr);
	} else {
		pd_thr = rtw_hal_query_pkt_detect_thold(phl->hal,
		                                        false,
		                                        rlink->hw_band);
		PHL_INFO("%s, pd_thr(%d)\n", __FUNCTION__, pd_thr);
	}
exit:
	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&pd_thr,
	                           sizeof(int));

	return ret;
}

enum phl_mdl_ret_code
_phl_custom_vr_pop_cfg(void* custom_ctx,
                       struct _custom_vr_ctx* vr_ctx,
                       struct phl_msg* msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct phl_info_t *phl = phl_custom_get_phl_info(custom_ctx);
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);
	u32 size = sizeof(u32);
	u32 pop_enable = 0xff;

	ret = _is_vr_mode_valid(custom_ctx, vr_ctx, msg, size);
	if (ret != MDL_RET_SUCCESS) {
		goto exit;
	}

	wrole = vr_ctx->init.wifi_role;
	rlink = get_rlink(wrole, RTW_RLINK_PRIMARY);

	pop_enable = *(u32*)(cmd->data);
	PHL_INFO("%s, pop_enable(%d)\n", __FUNCTION__, pop_enable);
	if (pop_enable != 0xff) {
		hal_status = rtw_hal_set_pop_en(phl->hal,
		                                (bool)pop_enable,
		                                rlink->hw_band);
		if (hal_status != RTW_HAL_STATUS_SUCCESS)
			ret = MDL_RET_FAIL;
	}
exit:
	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&ret,
	                           sizeof(u8));

	return ret;
}

enum phl_mdl_ret_code
_phl_custom_vr_pop_query(void* custom_ctx,
                         struct _custom_vr_ctx* vr_ctx,
                         struct phl_msg* msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_info_t *phl = phl_custom_get_phl_info(custom_ctx);
	struct rtw_wifi_role_t *wrole = NULL;
	struct rtw_wifi_role_link_t *rlink = NULL;
	struct rtw_custom_decrpt *cmd = (struct rtw_custom_decrpt *)(msg->inbuf);
	u32 size = sizeof(u32);
	u32 pop_enable = 0xff;

	ret = _is_vr_mode_valid(custom_ctx, vr_ctx, msg, size);
	if (ret != MDL_RET_SUCCESS){
		goto exit;
	}

	wrole = vr_ctx->init.wifi_role;
	rlink = get_rlink(wrole, RTW_RLINK_PRIMARY);

	pop_enable = rtw_hal_query_pop_en(phl->hal, rlink->hw_band);
	PHL_INFO("%s, pop_en(%d)\n", __FUNCTION__, pop_enable);
exit:
	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&pop_enable,
	                            sizeof(u32));

	return ret;
}

enum phl_mdl_ret_code
phl_custom_hdl_vr_evt(void* dispr,
                      void* custom_ctx,
                      struct _custom_vr_ctx* vr_ctx,
                      struct phl_msg* msg)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	u8 prephase = (IS_MSG_IN_PRE_PHASE(msg->msg_id)) ? (true) : (false);

	if (prephase == true)
		return MDL_RET_SUCCESS;

	switch (MSG_EVT_ID_FIELD(msg->msg_id)) {
		case MSG_EVT_CUSTOME_FEATURE_ENABLE:
			ret = _phl_custom_vr_feature_enable(custom_ctx, vr_ctx, msg);
			break;
		case MSG_EVT_CUSTOME_FEATURE_QUERY:
			ret = _phl_custom_vr_feature_query(custom_ctx, vr_ctx, msg);
			break;
		case MSG_EVT_CUSTOME_TESTMODE_PARAM:
			ret = _phl_custom_vr_testmode_param(custom_ctx, vr_ctx, msg);
			break;
		case MSG_EVT_AMPDU_CFG:
			ret = _phl_custom_vr_ampdu_cfg(custom_ctx, vr_ctx, msg);
			break;
		case MSG_EVT_AMPDU_QUERY:
			ret = _phl_custom_vr_ampdu_query(custom_ctx, vr_ctx, msg);
			break;
		case MSG_EVT_PDTHR_CFG:
			ret = _phl_custom_vr_pdthr_cfg(custom_ctx, vr_ctx, msg);
			break;
		case MSG_EVT_PDTHR_QUERY:
			ret = _phl_custom_vr_pdthr_query(custom_ctx, vr_ctx, msg);
			break;
		case MSG_EVT_POP_CFG:
			ret = _phl_custom_vr_pop_cfg(custom_ctx, vr_ctx, msg);
			break;
		case MSG_EVT_POP_QUERY:
			ret = _phl_custom_vr_pop_query(custom_ctx, vr_ctx, msg);
			break;
		default:
			ret = MDL_RET_SUCCESS;
			break;
	}
	PHL_INFO("%s, evt(%d), ret(%d)\n", __FUNCTION__,
	         MSG_EVT_ID_FIELD(msg->msg_id),
	         ret);
	return ret;
}
enum phl_mdl_ret_code
phl_custom_hdl_vr_fail_evt(void* dispr,
                           void* custom_ctx,
                           struct _custom_vr_ctx* vr_ctx,
                           struct phl_msg* msg)
{
	return MDL_RET_IGNORE;
}

enum phl_mdl_ret_code
_phl_custom_vr_set_wifi_role(void* dispr,
                             void* custom_ctx,
                             struct _custom_vr_ctx* vr_ctx,
                             struct rtw_custom_decrpt *cmd)
{
	enum phl_mdl_ret_code status = MDL_RET_SUCCESS;
	u32 size = sizeof(struct rtw_wifi_role_t *);

	PHL_INFO("%s, start\n", __FUNCTION__);

	if (cmd->len < size) {
		PHL_INFO("%s: illegal info len\n", __FUNCTION__);
		status = MDL_RET_FAIL;
		return status;
	}

	vr_ctx->init.wifi_role = *(struct rtw_wifi_role_t **)(cmd->data);

	phl_custom_prepare_evt_rpt(custom_ctx,
	                           cmd->evt_id,
	                           cmd->customer_id,
	                           (u8*)&status,
	                           sizeof(u8));

	PHL_INFO("%s, status(%d).\n", __FUNCTION__, status);

	return status;
}

enum phl_mdl_ret_code
phl_custom_vr_feature_set_hdlr(void* dispr,
                               void* custom_ctx,
                               struct _custom_vr_ctx* vr_ctx,
                               struct rtw_custom_decrpt *cmd)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	switch (cmd->evt_id) {
		case MSG_EVT_CUSTOME_SET_WIFI_ROLE:
			ret = _phl_custom_vr_set_wifi_role(dispr, custom_ctx, vr_ctx, cmd);
			break;
		default:
			ret = MDL_RET_SUCCESS;
			break;
	}
	PHL_INFO("%s, evt(%d), ret(%d)\n", __FUNCTION__, cmd->evt_id, ret);
	return ret;
}

enum phl_mdl_ret_code
phl_custom_vr_feature_query_hdlr(void* dispr,
                                 void* custom_ctx,
                                 struct _custom_vr_ctx* vr_ctx,
                                 struct rtw_custom_decrpt *cmd)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;

	PHL_INFO("%s, evt(%d), ret(%d)\n", __FUNCTION__, cmd->evt_id, ret);
	return ret;
}
#endif
