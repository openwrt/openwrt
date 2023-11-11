/******************************************************************************
 *
 * Copyright(c) 2021 Realtek Corporation. All rights reserved.
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
#include "nan.h"

u32 mac_get_act_schedule_id(struct mac_ax_adapter *adapter,
			    struct mac_ax_act_ack_info *act_ack_info)
{
	struct mac_ax_act_ack_info *ack_info = &adapter->nan_info.nan_act_ack_info;

	PLTFM_MEMCPY(act_ack_info, &adapter->nan_info.nan_act_ack_info,
		     sizeof(struct mac_ax_act_ack_info));
	PLTFM_MSG_TRACE("act ack id =  %d\n", ack_info->schedule_id);
	return MACSUCCESS;
}

u32 mac_check_cluster_info(struct mac_ax_adapter *adapter, struct mac_ax_nan_info *cluster_info)
{
	struct mac_ax_nan_info *ack_cluster_info = &adapter->nan_info;

	PLTFM_MEMCPY(cluster_info, &adapter->nan_info, sizeof(struct mac_ax_nan_info));
	PLTFM_MSG_TRACE("info ambtt =  %d\n", ack_cluster_info->rpt_ambtt);
	return MACSUCCESS;
}

u32 mac_nan_act_schedule_req(struct mac_ax_adapter *adapter, struct mac_ax_nan_sched_info *info)
{
	u32 ret = MACSUCCESS;

    #if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif

	struct fwcmd_act_schedule_req *hdr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

    /*h2c access*/
	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_act_schedule_req *)
				h2cb_put(h2cb, sizeof(struct fwcmd_act_schedule_req));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(info->module_id, FWCMD_H2C_ACT_SCHEDULE_REQ_MODULE_ID) |
			    SET_WORD(info->priority, FWCMD_H2C_ACT_SCHEDULE_REQ_PRIORITY) |
			    SET_WORD(info->options, FWCMD_H2C_ACT_SCHEDULE_REQ_OPTIONS) |
			    (info->faw_en ? FWCMD_H2C_ACT_SCHEDULE_REQ_FAW_EN : 0));

	hdr->dword1 =
		cpu_to_le32(SET_WORD(info->start_time, FWCMD_H2C_ACT_SCHEDULE_REQ_START_TIME));

	hdr->dword2 =
		cpu_to_le32(SET_WORD(info->duration, FWCMD_H2C_ACT_SCHEDULE_REQ_DURATION));

	hdr->dword3 =
		cpu_to_le32(SET_WORD(info->period, FWCMD_H2C_ACT_SCHEDULE_REQ_PERIOD));

	hdr->dword4 =
		cpu_to_le32(SET_WORD(info->tsf_idx, FWCMD_H2C_ACT_SCHEDULE_REQ_TSF_IDX) |
			SET_WORD(info->channel, FWCMD_H2C_ACT_SCHEDULE_REQ_CHANNEL) |
			SET_WORD(info->bw, FWCMD_H2C_ACT_SCHEDULE_REQ_BW) |
			SET_WORD(info->primary_ch_idx, FWCMD_H2C_ACT_SCHEDULE_REQ_PRIMARY_CH_IDX));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_NAN,
			      FWCMD_H2C_FUNC_ACT_SCHEDULE_REQ, 0, 0);

	if (ret != MACSUCCESS)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret != MACSUCCESS)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;

fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_nan_bcn_req(struct mac_ax_adapter *adapter, struct mac_ax_nan_bcn *info)
{
	u32 ret = MACSUCCESS;

    #if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif

	struct fwcmd_bcn_req *hdr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

    /*h2c access*/
	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_bcn_req *)h2cb_put(h2cb, sizeof(struct fwcmd_bcn_req));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(info->module_id, FWCMD_H2C_BCN_REQ_MODULE_ID) |
			    SET_WORD(info->bcn_intvl_ms, FWCMD_H2C_BCN_REQ_BCN_INTVL_MS) |
			    SET_WORD(info->priority, FWCMD_H2C_BCN_REQ_PRIORITY));

	hdr->dword1 =
		cpu_to_le32(SET_WORD(info->bcn_offset_us, FWCMD_H2C_BCN_REQ_BCN_OFFSET_US));

	hdr->dword2 =
		cpu_to_le32(SET_WORD(info->cur_tbtt, FWCMD_H2C_BCN_REQ_CUR_TBTT));

	hdr->dword3 =
		cpu_to_le32(SET_WORD(info->cur_tbtt_fr, FWCMD_H2C_BCN_REQ_CUR_TBTT_FR));

	hdr->dword4 =
		cpu_to_le32(SET_WORD(info->prohibit_before_ms,
				     FWCMD_H2C_BCN_REQ_PROHIBIT_BEFORE_MS) |
			    SET_WORD(info->prohibit_after_ms, FWCMD_H2C_BCN_REQ_PROHIBIT_AFTER_MS) |
			    SET_WORD(info->port_idx, FWCMD_H2C_BCN_REQ_PORT_IDX) |
			    SET_WORD(info->options, FWCMD_H2C_BCN_REQ_OPTIONS));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_NAN,
			      FWCMD_H2C_FUNC_BCN_REQ, 0, 1);

		if (ret != MACSUCCESS)
			goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret != MACSUCCESS)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;

fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_nan_func_ctrl(struct mac_ax_adapter *adapter, struct mac_ax_nan_func_info *info)
{
	u32 ret = MACSUCCESS;

	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif

	struct fwcmd_nan_func_ctrl *hdr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

    /*h2c access*/
	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_nan_func_ctrl *)h2cb_put(h2cb, sizeof(struct fwcmd_nan_func_ctrl));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(info->port_idx, FWCMD_H2C_NAN_FUNC_CTRL_PORT_IDX) |
			SET_WORD(info->mac_id, FWCMD_H2C_NAN_FUNC_CTRL_MAC_ID) |
			SET_WORD(info->master_pref, FWCMD_H2C_NAN_FUNC_CTRL_MASTER_PREF) |
			SET_WORD(info->random_factor, FWCMD_H2C_NAN_FUNC_CTRL_RANDOM_FACTOR));

	hdr->dword1 =
		cpu_to_le32(SET_WORD(info->op_ch_24g, FWCMD_H2C_NAN_FUNC_CTRL_OP_CH_24G) |
			SET_WORD(info->op_ch_5g, FWCMD_H2C_NAN_FUNC_CTRL_OP_CH_5G) |
			SET_WORD(info->options, FWCMD_H2C_NAN_FUNC_CTRL_OPTIONS));

	hdr->dword2 =
		cpu_to_le32(SET_WORD(info->time_indicate_period,
				     FWCMD_H2C_NAN_FUNC_CTRL_TIME_INDICATE_PERIOD) |
			SET_WORD(info->cluster_id[0], FWCMD_H2C_NAN_FUNC_CTRL_NAN_CLUSTER_ID0) |
			SET_WORD(info->cluster_id[1], FWCMD_H2C_NAN_FUNC_CTRL_NAN_CLUSTER_ID1) |
			SET_WORD(info->cluster_id[2], FWCMD_H2C_NAN_FUNC_CTRL_NAN_CLUSTER_ID2));

	hdr->dword3 =
		cpu_to_le32(SET_WORD(info->cluster_id[3], FWCMD_H2C_NAN_FUNC_CTRL_NAN_CLUSTER_ID3) |
			SET_WORD(info->cluster_id[4], FWCMD_H2C_NAN_FUNC_CTRL_NAN_CLUSTER_ID4) |
			SET_WORD(info->cluster_id[5], FWCMD_H2C_NAN_FUNC_CTRL_NAN_CLUSTER_ID5));

	hdr->dword4 =
		cpu_to_le32(SET_WORD(info->para_options, FWCMD_H2C_NAN_FUNC_CTRL_PARA_OPTIONS));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_NAN,
			      FWCMD_H2C_FUNC_NAN_FUNC_CTRL, 0, 1);

	if (ret != MACSUCCESS)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret != MACSUCCESS)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;

fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_nan_de_info(struct mac_ax_adapter *adapter, u8 status, u8 loc_bcast_sdf)
{
	u32 ret = MACSUCCESS;

    #if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif

	struct fwcmd_nan_de_info *hdr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

    /*h2c access*/
	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_nan_de_info *)h2cb_put(h2cb, sizeof(struct fwcmd_nan_de_info));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(status, FWCMD_H2C_NAN_DE_INFO_STATUS) |
			    SET_WORD(loc_bcast_sdf, FWCMD_H2C_NAN_DE_INFO_LOC_BCAST_SDF));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,	FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_NAN,
			      FWCMD_H2C_FUNC_NAN_FUNC_CTRL, 0, 1);

	if (ret != MACSUCCESS)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret != MACSUCCESS)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;

fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_nan_join_cluster(struct mac_ax_adapter *adapter, u8 is_allow)
{
	u32 ret = MACSUCCESS;

    #if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif

	struct fwcmd_nan_join_cluster *hdr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

    /*h2c access*/
	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_nan_join_cluster *)
			  h2cb_put(h2cb, sizeof(struct fwcmd_nan_join_cluster));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(is_allow, FWCMD_H2C_NAN_JOIN_CLUSTER_IS_ALLOW));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,	FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_NAN,
			      FWCMD_H2C_FUNC_NAN_FUNC_CTRL, 0, 1);

	if (ret != MACSUCCESS)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret != MACSUCCESS)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;

fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_nan_pause_faw_tx(struct mac_ax_adapter *adapter, u32 id_map)
{
	u32 ret = MACSUCCESS;

    #if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif

	struct fwcmd_pause_faw_tx *hdr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

    /*h2c access*/
	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_pause_faw_tx *)h2cb_put(h2cb, sizeof(struct fwcmd_pause_faw_tx));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(id_map, FWCMD_H2C_PAUSE_FAW_TX_ID_MAP));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_NAN,
			      FWCMD_H2C_FUNC_NAN_FUNC_CTRL, 0, 1);

	if (ret != MACSUCCESS)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret != MACSUCCESS)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;

fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_nan_get_cluster_info(struct mac_ax_adapter *adapter,
			     struct mac_ax_nan_info *cluster_info)
{
	u32 ret = MACSUCCESS;

#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif

	struct fwcmd_nan_get_cluster_info *hdr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		return MACFWNONRDY;
	}

	/*h2c access*/
	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_nan_get_cluster_info *)
		  h2cb_put(h2cb, sizeof(struct fwcmd_nan_get_cluster_info));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_NAN,
			      FWCMD_H2C_FUNC_NAN_GET_CLUSTER_INFO, 0, 1);

	if (ret != MACSUCCESS)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto fail;

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret != MACSUCCESS)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

fail:
	h2cb_free(adapter, h2cb);

	return ret;
}
