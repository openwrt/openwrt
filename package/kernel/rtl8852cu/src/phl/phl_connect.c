/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#include "phl_headers.h"

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
rtw_phl_connect_prepare(void *phl,
                        enum phl_band_idx band_idx,
                        struct rtw_wifi_role_t *wrole,
                        struct rtw_wifi_role_link_t *rlink,
                        u8 *addr)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	if (rtw_phl_role_is_client_category(wrole))
		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_CONNECT);
	else if (rtw_phl_role_is_ap_category(wrole))
		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_ADD_DEL_STA);

	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_CONNECT_START);
	msg.band_idx = band_idx;
	msg.inbuf = (u8 *)wrole;
	msg.rsvd[0].ptr = (u8 *)rlink;

	phl_status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if(phl_status != RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Dispr send msg fail!\n",
			  __FUNCTION__);
		goto exit;
	}

exit:
	return phl_status;
}

enum rtw_phl_status
rtw_phl_connect_linked(void *phl,
                       enum phl_band_idx band_idx,
                       struct rtw_wifi_role_t *wrole,
                       struct rtw_phl_stainfo_t *sta,
                       u8 *sta_addr)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	if (rtw_phl_role_is_client_category(wrole))
		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_CONNECT);
	else if (rtw_phl_role_is_ap_category(wrole))
		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_ADD_DEL_STA);

	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_CONNECT_LINKED);
	msg.band_idx = band_idx;
	msg.rsvd[0].ptr = sta;
	msg.rsvd[1].ptr = sta_addr;

	phl_status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if(phl_status != RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Dispr send msg fail!\n",
			  __FUNCTION__);
		goto exit;
	}

exit:
	return phl_status;
}

enum rtw_phl_status rtw_phl_connected(void *phl,
				      enum phl_band_idx band_idx,
				      struct rtw_wifi_role_t *wrole,
				      struct rtw_phl_stainfo_t *sta)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	if (rtw_phl_role_is_client_category(wrole))
		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_CONNECT);
	else if (rtw_phl_role_is_ap_category(wrole))
		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_ADD_DEL_STA);

	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_CONNECT_END);
	msg.band_idx = band_idx;
	msg.rsvd[0].ptr = sta;

	phl_status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if(phl_status != RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Dispr send msg fail!\n",
			  __FUNCTION__);
		goto exit;
	}

exit:
	return phl_status;
}

enum rtw_phl_status rtw_phl_disconnect(void *phl,
				       enum phl_band_idx band_idx,
				       struct rtw_wifi_role_t *wrole,
				       bool is_disconnect)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};


	if (rtw_phl_role_is_client_category(wrole)) {
		if (is_disconnect)
			SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_DISCONNECT);
		else
			SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_CONNECT);
	} else if (rtw_phl_role_is_ap_category(wrole))
		SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_ADD_DEL_STA);

	if (is_disconnect)
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_DISCONNECT_PREPARE);
	else
		SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_DISCONNECT);

	msg.band_idx = band_idx;
	msg.rsvd[0].ptr = (u8*)wrole;

	phl_status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if(phl_status != RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Dispr send msg fail!\n",
			  __FUNCTION__);
		goto exit;
	}

exit:
	return phl_status;
}
#else /* CONFIG_CMD_DISP */
enum rtw_phl_status
rtw_phl_connect_prepare(void *phl,
                        struct rtw_wifi_role_t *wrole,
                        struct rtw_wifi_role_link_t *rlink,
                        u8 *addr)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, ">>> rtw_phl_connect_prepare()\n");
	wrole->mstate = MLME_LINKING;
	psts = phl_mr_info_upt(phl_info, rlink);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s mr info upt failed\n", __func__);
		goto _exit;
	}

	psts = rtw_phl_mr_rx_filter_opt(phl, rlink);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
		goto _exit;
	}

	PHL_DUMP_MR_EX(phl_info);
_exit:
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<<< rtw_phl_connect_prepare()\n");
	return psts;
}

enum rtw_phl_status rtw_phl_connected(void *phl,
				      struct rtw_wifi_role_t *wrole,
				      struct rtw_phl_stainfo_t *sta)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, ">>> rtw_phl_connected()\n");
	psts = phl_mr_info_upt(phl_info, sta->rlink);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s mr info upt failed\n", __func__);
		goto _exit;
	}

	psts = rtw_phl_mr_rx_filter_opt(phl, sta->rlink);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
		goto _exit;
	}
	psts = phl_mr_tsf_sync(phl, wrole, sta->rlink, PHL_ROLE_MSTS_STA_CONN_END);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s set mr_tsf_sync failed\n", __func__);
		goto _exit;
	}
	PHL_DUMP_MR_EX(phl_info);
_exit:
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<<< rtw_phl_connected()\n");
	return psts;
}
enum rtw_phl_status rtw_phl_disconnect_prepare(void *phl,
				struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, ">>> rtw_phl_disconnect_prepare()\n");

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);
	}

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<<< rtw_phl_disconnect_prepare()\n");
	return psts;
}

enum rtw_phl_status
rtw_phl_disconnect(void *phl,
                   struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, ">>> rtw_phl_disconnect()\n");
	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		psts = phl_mr_info_upt(phl_info, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s mr info upt failed\n", __func__);
			goto _exit;
		}

		psts = rtw_phl_mr_rx_filter_opt(phl, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
			goto _exit;
		}

		psts = phl_mr_tsf_sync(phl, wrole, rlink, PHL_ROLE_MSTS_STA_DIS_CONN);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_tsf_sync failed\n", __func__);
			goto _exit;
		}
		rtw_hal_disconnect_notify(phl_info->hal, &rlink->chandef);
	}

	PHL_DUMP_MR_EX(phl_info);
_exit:
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<<< rtw_phl_disconnect()\n");
	return psts;
}
#endif

#ifdef CONFIG_AP_CMD_DISPR
enum rtw_phl_status rtw_phl_ap_started(void *phl,
			enum phl_band_idx band_idx,
			struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_START);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_AP_START);
	msg.band_idx = band_idx;

	phl_status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if(phl_status != RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Dispr send msg fail!\n",
			  __FUNCTION__);
		goto exit;
	}

exit:
	return phl_status;
}

enum rtw_phl_status rtw_phl_ap_stop(void *phl,
			enum phl_band_idx band_idx,
			struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_FG_MDL_AP_STOP);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_AP_STOP_PREPARE);
	msg.band_idx = band_idx;
	msg.rsvd[0].ptr = (u8*)wrole;

	phl_status = phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL);
	if(phl_status != RTW_PHL_STATUS_SUCCESS){
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "%s: Dispr send msg fail!\n",
			  __FUNCTION__);
		goto exit;
	}

exit:
	return phl_status;
}
#else  /* CONFIG_AP_CMD_DISPR */

enum rtw_phl_status rtw_phl_ap_started(void *phl, struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, ">>> rtw_phl_ap_started()\n");
	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		psts = phl_mr_info_upt(phl_info, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s mr info upt failed\n", __func__);
			goto _exit;
		}
		psts = rtw_phl_mr_rx_filter_opt(phl, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
			goto _exit;
		}

		psts = phl_mr_tsf_sync(phl, wrole, rlink, PHL_ROLE_MSTS_AP_START);

		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_tsf_sync failed\n", __func__);
			goto _exit;
		}
	}

	PHL_DUMP_MR_EX(phl_info);

_exit:
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<<< rtw_phl_ap_started()\n");
	return psts;
}

enum rtw_phl_status rtw_phl_ap_stop(void *phl, struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, ">>> rtw_phl_ap_stop()\n");
	wrole->mstate = MLME_NO_LINK;
	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		psts = phl_mr_info_upt(phl_info, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s mr info upt failed\n", __func__);
			goto _exit;
		}
		psts = rtw_phl_mr_rx_filter_opt(phl, rlink);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_rx_filter_opt failed\n", __func__);
			goto _exit;
		}

		psts = phl_mr_tsf_sync(phl, wrole, rlink, PHL_ROLE_MSTS_AP_STOP);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s set mr_tsf_sync failed\n", __func__);
			goto _exit;
		}
		#ifdef RTW_PHL_BCN
			psts = rtw_phl_free_bcn_entry(phl_info, rlink);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s phl_free_bcn_entry failed\n", __func__);
				goto _exit;
			}
		#endif

		rtw_hal_disconnect_notify(phl_info->hal, &rlink->chandef);
	}

	PHL_DUMP_MR_EX(phl_info);
_exit:
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<<< rtw_phl_ap_stop()\n");
	return psts;
}
#endif

#ifdef CONFIG_IBSS_CMD_DISPR
enum rtw_phl_status rtw_phl_ibss_started(void *phl, struct rtw_wifi_role_t *wrole)
{
	PHL_ERR("%s failed, cmd dispatcher not support\n", __func__);
	return RTW_PHL_STATUS_FAILURE;
}
#else  /* CONFIG_IBSS_CMD_DISPR */
enum rtw_phl_status rtw_phl_ibss_started(void *phl, struct rtw_wifi_role_t *wrole)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_wifi_role_link_t *rlink = NULL;

	if (wrole->rlink_num == 1) {
		rlink = &wrole->rlink[wrole->rlink_num-1];

		#ifdef RTW_WKARD_IBSS_SNIFFER_MODE
			rtw_hal_set_rxfltr_opt_by_mode(phl_info->hal,
			                               rlink->hw_band,
			                               RX_FLTR_OPT_MODE_SNIFFER);
		#endif
	}

	return RTW_PHL_STATUS_SUCCESS;
}
#endif

#ifdef RTW_WKARD_P2P_LISTEN
enum rtw_phl_status rtw_phl_p2p_listen_start(void *phl, struct rtw_wifi_role_t *wrole)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		rtw_hal_set_rxfltr_opt_by_mode(phl_info->hal, rlink->hw_band,
				RX_FLTR_OPT_MODE_SNIFFER);
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status rtw_phl_p2p_listen_end(void *phl, struct rtw_wifi_role_t *wrole)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		/* restore rx filter mode */
		rtw_phl_mr_rx_filter_opt(phl_info, rlink);
	}

	return RTW_PHL_STATUS_SUCCESS;
}
#endif /* RTW_WKARD_P2P_LISTEN */

