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
#define _PHL_TEST_MP_C_
#include "../../phl_headers.h"
#include "phl_test_mp_def.h"
#include "phl_test_mp_api.h"
#include "../../hal_g6/test/mp/hal_test_mp_api.h"

#ifdef CONFIG_PHL_TEST_MP
void mp_notification_complete(void* priv, struct phl_msg* msg)
{
	struct mp_context *mp_ctx = (struct mp_context *)priv;

	if(msg->inbuf){
		PHL_INFO("%s: Free info buf\n", __FUNCTION__);
		_os_kmem_free(mp_ctx->phl_com->drv_priv, msg->inbuf, msg->inlen);
	}
}

void mp_cmd_done_notification(struct mp_context *mp_ctx, enum mp_class mp_class,
							u8 mp_cmd_id)
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr;
	u8 *info = NULL;

	info = _os_kmem_alloc(mp_ctx->phl_com->drv_priv, 2);

	if(info == NULL){
		PHL_ERR("%s: Allocate msg hub buffer fail!\n", __FUNCTION__);
		return;
	}

	info[0] = mp_class;
	info[1] = mp_cmd_id;

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FUNC_MDL_TEST_MODULE);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_MP_CMD_DONE);

	attr.completion.completion = mp_notification_complete;
	attr.completion.priv = mp_ctx;

	msg.inbuf = info;
	msg.inlen = 2;

	if (phl_msg_hub_send(mp_ctx->phl,
			&attr, &msg) != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: send msg_hub failed\n", __func__);
		_os_kmem_free(mp_ctx->phl_com->drv_priv, info, 2);
	}
}

/*
 * @enum phl_msg_evt_id id: Assign different types of MP related msg event
 *	to pass buffer to another layer for further process
 */
void mp_buf_notification(struct mp_context *mp_ctx, void *buf, u32 buf_len,
			 enum phl_msg_evt_id id)
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr;
	u8 *info = NULL;

	info = _os_kmem_alloc(mp_ctx->phl_com->drv_priv, buf_len);

	if(info == NULL){
		PHL_ERR("%s: Allocate msg hub buffer fail!\n", __FUNCTION__);
		return;
	}

	_os_mem_cpy(mp_ctx->phl_com->drv_priv, info, buf, buf_len);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FUNC_MDL_TEST_MODULE);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, id);

	attr.completion.completion = mp_notification_complete;
	attr.completion.priv = mp_ctx;

	msg.inbuf = info;
	msg.inlen = buf_len;

	if (phl_msg_hub_send(mp_ctx->phl, &attr, &msg) != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: send msg_hub failed\n", __func__);
		_os_kmem_free(mp_ctx->phl_com->drv_priv, info, buf_len);
	}
}


bool mp_get_rpt_check(struct mp_context *mp_ctx, void *rpt_buf)
{
	bool ret = true;
	struct mp_arg_hdr *rpt_hdr = (struct mp_arg_hdr *)mp_ctx->rpt;
	struct mp_arg_hdr *rpt_buf_hdr = (struct mp_arg_hdr *)rpt_buf;

	if((rpt_hdr->mp_class != rpt_buf_hdr->mp_class) ||
		(rpt_hdr->cmd != rpt_buf_hdr->cmd)) {
		PHL_WARN("%s: Report buffer not match!\n", __FUNCTION__);
		rpt_buf_hdr->cmd_ok = true;
		rpt_buf_hdr->status = RTW_PHL_STATUS_FAILURE;
		ret = false;
	}

	return ret;
}

u8 mp_get_class_from_buf(struct mp_context *mp_ctx)
{
	u8 *buf_tmp = NULL;
	u8 mp_class = MP_CLASS_MAX;
	if(mp_ctx && mp_ctx->buf) {
		buf_tmp	= (u8 *)mp_ctx->buf;
		mp_class = buf_tmp[0];
	}
	return mp_class;
}

u8 mp_bp_handler(void *priv, struct test_bp_info* bp_info)
{
	struct mp_context *mp_ctx = (struct mp_context *)priv;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	PHL_DBG("%s: bp_info->type = %x\n", __FUNCTION__, bp_info->type);

	switch(bp_info->type){
		case BP_INFO_TYPE_MP_CMD_EVENT:
			if(mp_ctx->status == MP_STATUS_WAIT_CMD) {
				mp_ctx->status = MP_STATUS_CMD_EVENT;
				_os_sema_up(mp_ctx->phl_com->drv_priv,&(mp_ctx->mp_cmd_sema));
				phl_status = RTW_PHL_STATUS_SUCCESS;
			}
			break;
		case BP_INFO_TYPE_MP_RX_PHYSTS:
			if(mp_ctx->rx_physts == true) {
				u32 i = 0;
				for(i = 0; i < (bp_info->len/4); i ++)
					PHL_DBG("0x%08X\n",
						*((u32 *)(bp_info->ptr)+i));

				mp_buf_notification(mp_ctx,
						    bp_info->ptr,
						    bp_info->len,
						    MSG_EVT_MP_RX_PHYSTS);
			}
			break;
		case BP_INFO_TYPE_NONE:
		case BP_INFO_TYPE_WAIT_BEACON_JOIN:
		case BP_INFO_TYPE_SEND_AUTH_ODD:
		case BP_INFO_TYPE_SEND_ASOC_REQ:
		case BP_INFO_TYPE_SEND_DISASSOC:
		case BP_INFO_TYPE_FILL_DISASSOC_RSN:
		case BP_INFO_TYPE_SEND_PROBE_REQ:
		case BP_INFO_TYPE_RX_TEST_WPRPT:
		case BP_INFO_TYPE_RX_TEST_PATTERN:
		case BP_INFO_TYPE_MAX:
			PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "mp_bp_handler(): Unsupported case:%d, please check it\n",
					bp_info->type);
			break;
		default:
			PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "mp_bp_handler(): Unrecognize case:%d, please check it\n",
					bp_info->type);
			break;
	}
	return phl_status;
}

u8 mp_get_fail_rsn(void *priv,char* rsn, u32 max_len)
{
	//struct mp_context *mp_ctx = (struct mp_context *)priv;
	return true;
}

u8 mp_is_test_end(void *priv)
{
	struct mp_context *mp_ctx = (struct mp_context *)priv;

	return mp_ctx->is_mp_test_end;
}

u8 mp_is_test_pass(void *priv)
{
	//struct mp_context *mp_ctx = (struct mp_context *)priv;
	return true;
}

u8 mp_start(void *priv)
{
	struct mp_context *mp_ctx = (struct mp_context *)priv;
	struct rtw_phl_com_t* phl_com = mp_ctx->phl_com;

	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	u8 mp_class = MP_CLASS_MAX;
	FUNCIN();
	while(!mp_is_test_end(mp_ctx)){
		_os_sema_down(phl_com->drv_priv,&(mp_ctx->mp_cmd_sema));
		if(mp_ctx->status == MP_STATUS_CMD_EVENT){
			mp_ctx->status = MP_STATUS_RUN_CMD;
			mp_class = mp_get_class_from_buf(mp_ctx);

			/* Clear report buffer before executing next command */
			if(mp_ctx->rpt != NULL) {
				PHL_INFO("%s: Report not empty, cleanup!\n", __FUNCTION__);
				_os_mem_free(phl_com->drv_priv, mp_ctx->rpt, mp_ctx->rpt_len);
				mp_ctx->rpt = NULL;
				mp_ctx->rpt_len = 0;
			}

			switch(mp_class){
				case MP_CLASS_CONFIG:
					PHL_INFO("%s: class = MP_CLASS_CONFIG\n", __FUNCTION__);
					phl_status = mp_config(mp_ctx, (struct mp_config_arg *)mp_ctx->buf);
					break;
				case MP_CLASS_TX:
					PHL_INFO("%s: class = MP_CLASS_TX\n", __FUNCTION__);
					phl_status = mp_tx(mp_ctx, (struct mp_tx_arg *)mp_ctx->buf);
					break;
				case MP_CLASS_RX:
					PHL_INFO("%s: class = MP_CLASS_RX\n", __FUNCTION__);
					phl_status = mp_rx(mp_ctx, (struct mp_rx_arg *)mp_ctx->buf);
					break;
				case MP_CLASS_EFUSE:
					PHL_INFO("%s: class = MP_CLASS_EFUSE\n", __FUNCTION__);
					phl_status = mp_efuse(mp_ctx, (struct mp_efuse_arg *)mp_ctx->buf);
					break;
				case MP_CLASS_REG:
					PHL_INFO("%s: class = MP_CLASS_REG\n", __FUNCTION__);
					phl_status = mp_reg(mp_ctx, (struct mp_reg_arg *)mp_ctx->buf);
					break;
				case MP_CLASS_TXPWR:
					PHL_INFO("%s: class = MP_CLASS_TXPWR\n", __FUNCTION__);
					phl_status = mp_txpwr(mp_ctx, (struct mp_txpwr_arg *)mp_ctx->buf);
					break;
				case MP_CLASS_CAL:
					PHL_INFO("%s: class = MP_CLASS_CAL\n", __FUNCTION__);
					phl_status = mp_cal(mp_ctx, (struct mp_cal_arg *)mp_ctx->buf);
					break;
				default:
					PHL_WARN("%s: Unknown mp class! (%d)\n", __FUNCTION__, mp_class);
					break;

			}

			if(mp_ctx->rpt != NULL) {
				struct mp_arg_hdr *hdr = (struct mp_arg_hdr *)mp_ctx->rpt;
				mp_cmd_done_notification(mp_ctx, hdr->mp_class, hdr->cmd);
				PHL_INFO("%s: Indication class(%d) cmd(%d)\n",
						 __FUNCTION__, hdr->mp_class, hdr->cmd);
			}

			/* Clear command buffer after executing the command */
			if(mp_ctx->buf != NULL) {
				PHL_INFO("%s: Command buf not empty, cleanup!\n", __FUNCTION__);
				_os_mem_free(phl_com->drv_priv, mp_ctx->buf, mp_ctx->buf_len);
				mp_ctx->buf = NULL;
				mp_ctx->buf_len = 0;
			}
			mp_ctx->status = MP_STATUS_WAIT_CMD;
		}
	}

	FUNCOUT();
	return (u8)phl_status;
}

void mp_change_mode(struct mp_context *mp_ctx, enum rtw_drv_mode driver_mode)
{
	struct phl_info_t *phl_info = mp_ctx->phl;
	#ifdef RTW_WKARD_AP_MP
	struct hal_info_t *hal_info = mp_ctx->hal;
	#endif
	PHL_INFO("%s Change to %x\n", __FUNCTION__, driver_mode);

	/* Need PHL stop function later */

	phl_info->phl_com->drv_mode = driver_mode;

#ifdef RTW_WKARD_MP_MODE_CHANGE
#else
	rtw_phl_reset(phl_info);
#endif

	if(true == phl_is_mp_mode(phl_info->phl_com)) {

		/* Load bt map to shadow map */
		rtw_hal_mp_efuse_bt_shadow_reload(mp_ctx);

		rtw_hal_acpt_crc_err_pkt(mp_ctx->hal,mp_ctx->cur_phy,true);
		#ifdef RTW_WKARD_AP_MP
		rtw_hal_bb_rx_ndp_mp(mp_ctx->hal);
		/* RSSI flow process under MP mode */
		rtw_hal_bb_dm_init_mp(mp_ctx->hal);
		rtw_hal_rf_dm_init_mp(mp_ctx->hal);
		rtw_hal_mac_set_rxfltr_mp_mode(hal_info, 0, 0x1c00);
		#endif
	}
	else {
		rtw_hal_acpt_crc_err_pkt(mp_ctx->hal,mp_ctx->cur_phy,false);
	}

	rtw_hal_mp_ic_hw_setting_init(mp_ctx);
	rtw_hal_mp_cfg(phl_info->phl_com ,mp_ctx->hal);
}

enum rtw_phl_status phl_test_mp_alloc(struct phl_info_t *phl_info, void *hal, void **mp)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mp_context *mp_ctx = NULL;

	mp_ctx = _os_mem_alloc(phl_com->drv_priv, sizeof(struct mp_context));

	if (mp_ctx == NULL) {
		PHL_ERR("alloc mp_context failed\n");
		phl_status = RTW_PHL_STATUS_RESOURCE;
		goto exit;
	}
	_os_sema_init(phl_com->drv_priv,&(mp_ctx->mp_cmd_sema), 0);
	mp_ctx->cur_phy = HW_PHY_0;
	mp_ctx->rx_physts = false;
	mp_ctx->phl = phl_info;
	mp_ctx->phl_com = phl_com;
	mp_ctx->hal = hal;
	mp_ctx->status = MP_STATUS_INIT;
	*mp = mp_ctx;
	phl_status = RTW_PHL_STATUS_SUCCESS;

exit:
	return phl_status;
}

void phl_test_mp_free(void **mp)
{
	struct mp_context *mp_ctx = NULL;

	if(*mp == NULL)
		return;

	mp_ctx = (struct mp_context *)(*mp);
	_os_sema_free(mp_ctx->phl_com->drv_priv, &(mp_ctx->mp_cmd_sema));
	_os_mem_free(mp_ctx->phl_com->drv_priv, mp_ctx, sizeof(struct mp_context));
	mp_ctx = NULL;
	*mp = NULL;
}


void phl_test_mp_init(void *mp)
{
	struct mp_context *mp_ctx = NULL;
	struct test_obj_ctrl_interface *pctrl = NULL;
	u8 status = false;

	if(mp == NULL)
		return;

	mp_ctx = (struct mp_context *)mp;
	pctrl = &(mp_ctx->mp_test_ctrl);

	mp_ctx->max_para = 2000;
	mp_ctx->status = MP_STATUS_WAIT_CMD;
	mp_ctx->is_mp_test_end = false;
	pctrl->bp_handler = mp_bp_handler;
	pctrl->get_fail_rsn = mp_get_fail_rsn;
	pctrl->is_test_end = mp_is_test_end;
	pctrl->is_test_pass = mp_is_test_pass;
	pctrl->start_test = mp_start;
	status = rtw_phl_test_add_new_test_obj(mp_ctx->phl_com,
	                              "mp_test",
	                              mp_ctx,
	                              TEST_LVL_LOW,
	                              pctrl,
	                              -1,
	                              TEST_SUB_MODULE_MP,
	                              INTGR_TEST_MODE);
}

void phl_test_mp_deinit(void *mp)
{
	struct mp_context *mp_ctx = NULL;
	if(mp == NULL)
		return;

	mp_ctx = (struct mp_context *)mp;

	if(mp_ctx->status < MP_STATUS_WAIT_CMD)
		return;

	mp_ctx->is_mp_test_end = true;
	_os_sema_up(mp_ctx->phl_com->drv_priv,&(mp_ctx->mp_cmd_sema));
	mp_ctx->status = MP_STATUS_INIT;
}

void phl_test_mp_start(void *mp, u8 tm_mode)
{
	struct mp_context *mp_ctx = NULL;

	if(mp == NULL)
		return;

	mp_ctx = (struct mp_context *)mp;

	mp_change_mode(mp_ctx, tm_mode);

	/* stop phl watchdog */
	rtw_phl_watchdog_stop(mp_ctx->phl);
}

void phl_test_mp_stop(void *mp, u8 tm_mode)
{
	struct mp_context *mp_ctx = NULL;
	if(mp == NULL)
		return;

	mp_ctx = (struct mp_context *)mp;

	if(mp_ctx->status < MP_STATUS_WAIT_CMD)
		return;

	mp_change_mode(mp_ctx, tm_mode);
	/* start phl watchdog */
	rtw_phl_watchdog_start(mp_ctx->phl);
}


void phl_test_mp_cmd_process(void *mp, void *buf, u32 buf_len, u8 submdid)
{
	struct mp_context *mp_ctx = NULL;
	struct rtw_phl_com_t *phl_com = NULL;
	struct test_bp_info bp_info;
	FUNCIN();

	if(mp == NULL)
		return;

	mp_ctx = (struct mp_context *)mp;
	phl_com = mp_ctx->phl_com;

	if((buf == NULL) || (buf_len > mp_ctx->max_para)) {
		PHL_ERR("%s: Invalid buffer content!\n", __func__);
		return;
	}


	if(mp_ctx->status == MP_STATUS_WAIT_CMD) {
		mp_ctx->buf_len = buf_len;
		mp_ctx->buf = _os_mem_alloc(phl_com->drv_priv, buf_len);
		_os_mem_cpy(phl_com->drv_priv, mp_ctx->buf, buf, buf_len);
		_os_mem_set(phl_com->drv_priv, &bp_info, 0, sizeof(struct test_bp_info));
		bp_info.type = BP_INFO_TYPE_MP_CMD_EVENT;
		rtw_phl_test_setup_bp(phl_com, &bp_info, submdid);
	}
	else {
		PHL_WARN("%s: Previous command is still running!\n", __FUNCTION__);
	}

	FUNCOUT();
}

void phl_test_mp_get_rpt(void *mp, void *buf, u32 buf_len)
{
	struct mp_context *mp_ctx = NULL;
	FUNCIN();

	if(mp == NULL) {
		PHL_WARN("%s: mp is NULL!\n", __FUNCTION__);
		goto exit;
	}

	mp_ctx = (struct mp_context *)mp;

	if(mp_ctx->status != MP_STATUS_WAIT_CMD) {
		PHL_WARN("%s: command is running!\n", __FUNCTION__);
		goto exit;
	}

	if(mp_ctx->rpt == NULL) {
		PHL_DBG("%s: mp_ctx->rpt  is NULL!\n", __FUNCTION__);
		goto exit;
	}

	if(buf_len < mp_ctx->rpt_len) {
		PHL_WARN("%s: buffer not enough!\n", __FUNCTION__);
		goto exit;
	}

	if(mp_get_rpt_check(mp_ctx, buf) == true) {
		_os_mem_cpy(mp_ctx->phl_com->drv_priv, buf, mp_ctx->rpt, mp_ctx->rpt_len);
		_os_mem_free(mp_ctx->phl_com->drv_priv, mp_ctx->rpt, mp_ctx->rpt_len);
		mp_ctx->rpt = NULL;
		mp_ctx->rpt_len = 0;
	}

exit:
	FUNCOUT();
}
#endif /* CONFIG_PHL_TEST_MP */
