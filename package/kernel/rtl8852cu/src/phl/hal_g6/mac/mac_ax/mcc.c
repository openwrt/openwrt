/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#include "mcc.h"

u32 mac_reset_mcc_group(struct mac_ax_adapter *adapter, u8 group)
{
	struct fwcmd_reset_mcc_group *ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	u32 ret = 0;

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, group);
		return MACNOITEM;
	}

	adapter->sm.mcc_group[group] = MAC_AX_MCC_EMPTY;
	adapter->sm.mcc_request[group] = MAC_AX_MCC_REQ_IDLE;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: H2C alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_reset_mcc_group));
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: H2C put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	ptr = (struct fwcmd_reset_mcc_group *)buf;
	ptr->dword0 =
	cpu_to_le32(SET_WORD(group, FWCMD_H2C_RESET_MCC_GROUP_GROUP));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MCC,
			      FWCMD_H2C_FUNC_RESET_MCC_GROUP, 0, 0);

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Set H2C HDR fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Build H2C TXD fail\n", __func__);
		goto fail;
	}
	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: platform tx err: %d\n", __func__, ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_reset_mcc_request(struct mac_ax_adapter *adapter, u8 group)
{
	if (adapter->sm.mcc_request[group] != MAC_AX_MCC_REQ_FAIL) {
		PLTFM_MSG_ERR("[ERR]%s: state != req fail\n", __func__);
		return MACPROCERR;
	}

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, group);
		return MACNOITEM;
	}

	adapter->sm.mcc_request[group] = MAC_AX_MCC_REQ_IDLE;

	return MACSUCCESS;
}

u32 mac_add_mcc(struct mac_ax_adapter *adapter, struct mac_ax_mcc_role *info)
{
	struct fwcmd_add_mcc *ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	u32 ret = 0;

	if (!(adapter->sm.mcc_group[info->group] == MAC_AX_MCC_EMPTY ||
	      adapter->sm.mcc_group[info->group] == MAC_AX_MCC_ADD_DONE)) {
		PLTFM_MSG_ERR("[ERR]%s: state != empty or add done\n",
			      __func__);
		return MACPROCERR;
	}

	adapter->sm.mcc_group[info->group] = MAC_AX_MCC_STATE_H2C_SENT;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: H2C alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_add_mcc));
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: H2C put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	ptr = (struct fwcmd_add_mcc *)buf;
	ptr->dword0 =
	cpu_to_le32(SET_WORD(info->macid, FWCMD_H2C_ADD_MCC_MACID) |
		    SET_WORD(info->central_ch_seg0,
			     FWCMD_H2C_ADD_MCC_CENTRAL_CH_SEG0) |
		    SET_WORD(info->central_ch_seg1,
			     FWCMD_H2C_ADD_MCC_CENTRAL_CH_SEG1) |
		    SET_WORD(info->primary_ch, FWCMD_H2C_ADD_MCC_PRIMARY_CH));

	ptr->dword1 =
	cpu_to_le32((info->dis_tx_null ? FWCMD_H2C_ADD_MCC_DIS_TX_NULL : 0) |
		    (info->dis_sw_retry ? FWCMD_H2C_ADD_MCC_DIS_SW_RETRY : 0) |
		    (info->in_curr_ch ? FWCMD_H2C_ADD_MCC_IN_CURR_CH : 0) |
		    SET_WORD(info->bandwidth, FWCMD_H2C_ADD_MCC_BANDWIDTH) |
		    SET_WORD(info->group, FWCMD_H2C_ADD_MCC_GROUP) |
		    SET_WORD(info->c2h_rpt, FWCMD_H2C_ADD_MCC_C2H_RPT) |
		    SET_WORD(info->sw_retry_count,
			     FWCMD_H2C_ADD_MCC_SW_RETRY_COUNT) |
		    SET_WORD(info->tx_null_early,
			     FWCMD_H2C_ADD_MCC_TX_NULL_EARLY) |
		    (info->btc_in_2g ? FWCMD_H2C_ADD_MCC_BTC_IN_2G : 0) |
		    (info->pta_en ? FWCMD_H2C_ADD_MCC_PTA_EN : 0) |
		    (info->rfk_by_pass ? FWCMD_H2C_ADD_MCC_RFK_BY_PASS : 0) |
		    SET_WORD(info->ch_band_type,
			     FWCMD_H2C_ADD_MCC_CH_BAND_TYPE));

	ptr->dword2 =
	cpu_to_le32(SET_WORD(info->duration, FWCMD_H2C_ADD_MCC_DURATION));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MCC,
			      FWCMD_H2C_FUNC_ADD_MCC, 0, 0);

	ptr->dword3 =
	cpu_to_le32((info->courtesy_en ? FWCMD_H2C_ADD_MCC_COURTESY_EN : 0) |
		    SET_WORD(info->courtesy_num,
			     FWCMD_H2C_ADD_MCC_COURTESY_NUM) |
		    SET_WORD(info->courtesy_target,
			     FWCMD_H2C_ADD_MCC_COURTESY_TARGET));

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Set H2C HDR fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Build H2C TXD fail\n", __func__);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: platform tx err: %d\n", __func__, ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_start_mcc(struct mac_ax_adapter *adapter,
		  struct mac_ax_mcc_start *info)
{
	struct fwcmd_start_mcc *ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	u32 ret = 0;
	u32 group = info->group;

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, info->group);
		return MACNOITEM;
	}

	if (adapter->sm.mcc_group[info->group] != MAC_AX_MCC_ADD_DONE) {
		PLTFM_MSG_ERR("[ERR]%s: state != add done\n", __func__);
		return MACPROCERR;
	}

	adapter->sm.mcc_group[info->group] = MAC_AX_MCC_STATE_H2C_SENT;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: H2C alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_start_mcc));
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: H2C put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	ptr = (struct fwcmd_start_mcc *)buf;
	ptr->dword0 =
	cpu_to_le32(SET_WORD(info->group, FWCMD_H2C_START_MCC_GROUP) |
		    (info->btc_in_group ? FWCMD_H2C_START_MCC_BTC_IN_GROUP : 0) |
		    SET_WORD(info->old_group_action,
			     FWCMD_H2C_START_MCC_OLD_GROUP_ACTION) |
		    SET_WORD(info->old_group, FWCMD_H2C_START_MCC_OLD_GROUP) |
		    SET_WORD(info->notify_cnt, FWCMD_H2C_START_MCC_NOTIFY_CNT) |
		    (info->notify_rxdbg_en ? FWCMD_H2C_START_MCC_NOTIFY_RXDBG_EN : 0) |
		    SET_WORD(info->macid, FWCMD_H2C_START_MCC_MACID));

	ptr->dword1 =
	cpu_to_le32(SET_WORD(info->tsf_low, FWCMD_H2C_START_MCC_TSF_LOW));

	ptr->dword2 =
	cpu_to_le32(SET_WORD(info->tsf_high, FWCMD_H2C_START_MCC_TSF_HIGH));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MCC,
			      FWCMD_H2C_FUNC_START_MCC, 0, 0);

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Set H2C HDR fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Build H2C TXD fail\n", __func__);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: platform tx err: %d\n", __func__, ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_stop_mcc(struct mac_ax_adapter *adapter, u8 group, u8 macid,
		 u8 prev_groups)
{
	struct fwcmd_stop_mcc *ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	u32 ret = 0;

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, group);
		return MACNOITEM;
	}

	if (adapter->sm.mcc_group[group] != MAC_AX_MCC_START_DONE) {
		PLTFM_MSG_ERR("[ERR]%s: state != start done\n", __func__);
		return MACPROCERR;
	}

	adapter->sm.mcc_group[group] = MAC_AX_MCC_STATE_H2C_SENT;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: H2C alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_stop_mcc));
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: H2C put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	ptr = (struct fwcmd_stop_mcc *)buf;
	ptr->dword0 =
	cpu_to_le32(SET_WORD(group, FWCMD_H2C_STOP_MCC_GROUP) |
		    SET_WORD(macid, FWCMD_H2C_STOP_MCC_MACID) |
		    (prev_groups ? FWCMD_H2C_STOP_MCC_PREV_GROUPS : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MCC,
			      FWCMD_H2C_FUNC_STOP_MCC, 0, 0);

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Set H2C HDR fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Build H2C TXD fail\n", __func__);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: platform tx err: %d\n", __func__, ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_del_mcc_group(struct mac_ax_adapter *adapter, u8 group,
		      u8 prev_groups)
{
	struct fwcmd_del_mcc_group *ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	u32 ret = 0;

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, group);
		return MACNOITEM;
	}

	if (!(adapter->sm.mcc_group[group] == MAC_AX_MCC_ADD_DONE ||
	      adapter->sm.mcc_group[group] == MAC_AX_MCC_STOP_DONE)) {
		PLTFM_MSG_ERR("[ERR]%s: state != add or stop done\n",
			      __func__);
		return MACPROCERR;
	}

	adapter->sm.mcc_group[group] = MAC_AX_MCC_STATE_H2C_SENT;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: H2C alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_del_mcc_group));
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: H2C put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	ptr = (struct fwcmd_del_mcc_group *)buf;
	ptr->dword0 =
	cpu_to_le32(SET_WORD(group, FWCMD_H2C_DEL_MCC_GROUP_GROUP) |
		    (prev_groups ? FWCMD_H2C_DEL_MCC_GROUP_PREV_GROUPS : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MCC,
			      FWCMD_H2C_FUNC_DEL_MCC_GROUP, 0, 0);

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Set H2C HDR fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Build H2C TXD fail\n", __func__);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: platform tx err: %d\n", __func__, ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_mcc_request_tsf(struct mac_ax_adapter *adapter, u8 group,
			u8 macid_x, u8 macid_y)
{
	struct fwcmd_mcc_req_tsf *ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	u32 ret = 0;

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, group);
		return MACNOITEM;
	}

	adapter->sm.mcc_request[group] = MAC_AX_MCC_REQ_H2C_SENT;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: H2C alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_mcc_req_tsf));
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: H2C put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	ptr = (struct fwcmd_mcc_req_tsf *)buf;
	ptr->dword0 =
	cpu_to_le32(SET_WORD(group, FWCMD_H2C_MCC_REQ_TSF_GROUP) |
		    SET_WORD(macid_x, FWCMD_H2C_MCC_REQ_TSF_MACID_X) |
		    SET_WORD(macid_y, FWCMD_H2C_MCC_REQ_TSF_MACID_Y));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MCC,
			      FWCMD_H2C_FUNC_MCC_REQ_TSF, 0, 0);

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Set H2C HDR fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Build H2C TXD fail\n", __func__);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: platform tx err: %d\n", __func__, ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_mcc_macid_bitmap(struct mac_ax_adapter *adapter, u8 group,
			 u8 macid, u8 *bitmap, u8 len)
{
	struct fwcmd_mcc_macid_bitmap *ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	u32 ret = 0;

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, group);
		return MACNOITEM;
	}

	adapter->sm.mcc_request[group] = MAC_AX_MCC_REQ_H2C_SENT;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: H2C alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, len + 4);
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: H2C put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	ptr = (struct fwcmd_mcc_macid_bitmap *)buf;
	ptr->dword0 =
	cpu_to_le32(SET_WORD(group, FWCMD_H2C_MCC_MACID_BITMAP_GROUP) |
		    SET_WORD(macid, FWCMD_H2C_MCC_MACID_BITMAP_MACID) |
		    SET_WORD(len, FWCMD_H2C_MCC_MACID_BITMAP_BITMAP_LENGTH));

	PLTFM_MEMCPY((u8 *)ptr + sizeof(u32), bitmap, len);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MCC,
			      FWCMD_H2C_FUNC_MCC_MACID_BITMAP, 0, 0);

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Set H2C HDR fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Build H2C TXD fail\n", __func__);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: platform tx error %d\n", __func__, ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_mcc_sync_enable(struct mac_ax_adapter *adapter, u8 group,
			u8 source, u8 target, u8 offset)
{
	struct fwcmd_mcc_sync *ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	u32 ret = 0;

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, group);
		return MACNOITEM;
	}

	adapter->sm.mcc_request[group] = MAC_AX_MCC_REQ_H2C_SENT;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: H2C alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_mcc_sync));
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: H2C put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	ptr = (struct fwcmd_mcc_sync *)buf;
	ptr->dword0 =
	cpu_to_le32(SET_WORD(group, FWCMD_H2C_MCC_SYNC_GROUP) |
		    SET_WORD(source, FWCMD_H2C_MCC_SYNC_MACID_SOURCE) |
		    SET_WORD(target, FWCMD_H2C_MCC_SYNC_MACID_TARGET) |
		    SET_WORD(offset, FWCMD_H2C_MCC_SYNC_SYNC_OFFSET));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MCC,
			      FWCMD_H2C_FUNC_MCC_SYNC, 0, 0);

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Set H2C HDR fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Build H2C TXD fail\n", __func__);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: platform tx error %d\n", __func__, ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_mcc_set_duration(struct mac_ax_adapter *adapter,
			 struct mac_ax_mcc_duration_info *info)
{
	struct fwcmd_mcc_set_duration *ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	u32 ret = 0;

	adapter->sm.mcc_request[info->group] = MAC_AX_MCC_REQ_H2C_SENT;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: H2C alloc fail\n", __func__);
		return MACNPTR;
	}

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_mcc_set_duration));
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: H2C put fail\n", __func__);
		ret = MACNOBUF;
		goto fail;
	}

	ptr = (struct fwcmd_mcc_set_duration *)buf;
	ptr->dword0 =
	cpu_to_le32(SET_WORD(info->group, FWCMD_H2C_MCC_SET_DURATION_GROUP) |
		    (info->btc_in_group ?
		     FWCMD_H2C_MCC_SET_DURATION_BTC_IN_GROUP : 0) |
		    SET_WORD(info->start_macid,
			     FWCMD_H2C_MCC_SET_DURATION_START_MACID) |
		    SET_WORD(info->macid_x,
			     FWCMD_H2C_MCC_SET_DURATION_MACID_X) |
		    SET_WORD(info->macid_y,
			     FWCMD_H2C_MCC_SET_DURATION_MACID_Y));
	ptr->dword1 =
	cpu_to_le32(SET_WORD(info->start_tsf_low,
			     FWCMD_H2C_MCC_SET_DURATION_START_TSF_LOW));

	ptr->dword2 =
	cpu_to_le32(SET_WORD(info->start_tsf_high,
			     FWCMD_H2C_MCC_SET_DURATION_START_TSF_HIGH));

	ptr->dword3 =
	cpu_to_le32(SET_WORD(info->duration_x,
			     FWCMD_H2C_MCC_SET_DURATION_DURATION_X));

	ptr->dword4 =
	cpu_to_le32(SET_WORD(info->duration_y,
			     FWCMD_H2C_MCC_SET_DURATION_DURATION_Y));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MCC,
			      FWCMD_H2C_FUNC_MCC_SET_DURATION, 0, 0);

	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Set H2C HDR fail\n", __func__);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: Build H2C TXD fail\n", __func__);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: platform tx err %d\n", __func__, ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_get_mcc_status_rpt(struct mac_ax_adapter *adapter,
			   u8 group, u8 *status, u32 *tsf_high, u32 *tsf_low)
{
	struct mac_ax_mcc_group_info *mcc_info = &adapter->mcc_group_info;

	if (adapter->sm.mcc_group[group] == MAC_AX_MCC_EMPTY) {
		PLTFM_MSG_ERR("[ERR]%s: state = empty\n", __func__);
		return MACPROCERR;
	}

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, group);
		return MACNOITEM;
	}

	*status = mcc_info->groups[group].rpt_status;
	PLTFM_MSG_TRACE("[TRACE]%s: mcc status: %d\n", __func__, *status);

	*tsf_high = mcc_info->groups[group].rpt_tsf_high;
	*tsf_low = mcc_info->groups[group].rpt_tsf_low;

	PLTFM_MSG_TRACE("[TRACE]%s: report tsf_high: 0x%x\n",
			__func__, *tsf_high);
	PLTFM_MSG_TRACE("[TRACE]%s: report tsf_low: 0x%x\n",
			__func__, *tsf_low);

	return MACSUCCESS;
}

u32 mac_get_mcc_tsf_rpt(struct mac_ax_adapter *adapter, u8 group,
			u32 *tsf_x_high, u32 *tsf_x_low,
			u32 *tsf_y_high, u32 *tsf_y_low)
{
	struct mac_ax_mcc_group_info *mcc_info = &adapter->mcc_group_info;

	if (group > MCC_GROUP_ID_MAX) {
		PLTFM_MSG_ERR("[ERR]%s: invalid group: %d\n", __func__, group);
		return MACNOITEM;
	}

	*tsf_x_high = mcc_info->groups[group].tsf_x_high;
	*tsf_x_low = mcc_info->groups[group].tsf_x_low;
	PLTFM_MSG_TRACE("[TRACE]%s: tsf_x_high: 0x%x\n", __func__, *tsf_x_high);
	PLTFM_MSG_TRACE("[TRACE]%s: tsf_x_low: 0x%x\n", __func__, *tsf_x_low);

	*tsf_y_high = mcc_info->groups[group].tsf_y_high;
	*tsf_y_low += mcc_info->groups[group].tsf_y_low;
	PLTFM_MSG_TRACE("[TRACE]%s: tsf_y_high: 0x%x\n", __func__, *tsf_y_high);
	PLTFM_MSG_TRACE("[TRACE]%s: tsf_y_low: 0x%x\n", __func__, *tsf_y_low);

	adapter->sm.mcc_request[group] = MAC_AX_MCC_REQ_IDLE;

	return MACSUCCESS;
}

u32 mac_get_mcc_group(struct mac_ax_adapter *adapter, u8 *pget_group)
{
	struct mac_ax_state_mach *sm = &adapter->sm;
	u8 group_idx;

	for (group_idx = 0; group_idx <= MCC_GROUP_ID_MAX; group_idx++) {
		if (sm->mcc_group[group_idx] == MAC_AX_MCC_EMPTY) {
			*pget_group = group_idx;
			PLTFM_MSG_TRACE("[TRACE]%s: get mcc empty group %u\n",
					__func__, *pget_group);
			adapter->sm.mcc_group[group_idx] = MAC_AX_MCC_EMPTY;
			adapter->sm.mcc_request[group_idx] = MAC_AX_MCC_REQ_IDLE;
			return MACSUCCESS;
		}
	}
	return MACMCCGPFL;
}

u32 mac_check_add_mcc_done(struct mac_ax_adapter *adapter, u8 group)
{
	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr state: %d (%d)\n", __func__,
			group, adapter->sm.mcc_group[group],
			adapter->sm.mcc_group_state[group]);

	if (adapter->sm.mcc_group[group] == MAC_AX_MCC_ADD_DONE)
		return MACSUCCESS;
	else
		return MACPROCBUSY;
}

u32 mac_check_start_mcc_done(struct mac_ax_adapter *adapter, u8 group)
{
	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr state: %d (%d)\n", __func__,
			group, adapter->sm.mcc_group[group],
			adapter->sm.mcc_group_state[group]);

	if (adapter->sm.mcc_group[group] == MAC_AX_MCC_START_DONE)
		return MACSUCCESS;
	else
		return MACPROCBUSY;
}

u32 mac_check_stop_mcc_done(struct mac_ax_adapter *adapter, u8 group)
{
	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr state: %d (%d)\n", __func__,
			group, adapter->sm.mcc_group[group],
			adapter->sm.mcc_group_state[group]);

	if (adapter->sm.mcc_group[group] == MAC_AX_MCC_STOP_DONE)
		return MACSUCCESS;
	else
		return MACPROCBUSY;
}

u32 mac_check_del_mcc_group_done(struct mac_ax_adapter *adapter, u8 group)
{
	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr state: %d (%d)\n", __func__,
			group, adapter->sm.mcc_group[group],
			adapter->sm.mcc_group_state[group]);

	if (adapter->sm.mcc_group[group] == MAC_AX_MCC_EMPTY)
		return MACSUCCESS;
	else
		return MACPROCBUSY;
}

u32 mac_check_mcc_request_tsf_done(struct mac_ax_adapter *adapter, u8 group)
{
	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr req state: %d (%d)\n", __func__,
			group, adapter->sm.mcc_request[group],
			adapter->sm.mcc_request_state[group]);

	if (adapter->sm.mcc_request[group] == MAC_AX_MCC_REQ_DONE)
		return MACSUCCESS;
	else
		return MACPROCBUSY;
}

u32 mac_check_mcc_macid_bitmap_done(struct mac_ax_adapter *adapter, u8 group)
{
	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr req state: %d (%d)\n", __func__,
			group, adapter->sm.mcc_request[group],
			adapter->sm.mcc_request_state[group]);

	if (adapter->sm.mcc_request[group] == MAC_AX_MCC_REQ_IDLE)
		return MACSUCCESS;
	else
		return MACPROCBUSY;
}

u32 mac_check_mcc_sync_enable_done(struct mac_ax_adapter *adapter, u8 group)
{
	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr req state: %d (%d)\n", __func__,
			group, adapter->sm.mcc_request[group],
			adapter->sm.mcc_request_state[group]);

	if (adapter->sm.mcc_request[group] == MAC_AX_MCC_REQ_IDLE)
		return MACSUCCESS;
	else
		return MACPROCBUSY;
}

u32 mac_check_mcc_set_duration_done(struct mac_ax_adapter *adapter, u8 group)
{
	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr req state: %d (%d)\n", __func__,
			group, adapter->sm.mcc_request[group],
			adapter->sm.mcc_request_state[group]);

	if (adapter->sm.mcc_request[group] == MAC_AX_MCC_REQ_IDLE)
		return MACSUCCESS;
	else
		return MACPROCBUSY;
}

