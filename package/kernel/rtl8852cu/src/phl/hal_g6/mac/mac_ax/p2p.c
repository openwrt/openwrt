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

#include "p2p.h"

static u32 p2p_info_size = sizeof(struct mac_ax_p2p_info) * P2P_MAX_NUM;

static u32 _get_valid_p2pid(struct mac_ax_adapter *adapter, u8 macid, u8 *p2pid)
{
	struct mac_ax_p2p_info *info = adapter->p2p_info;
	u8 idx;

	for (idx = 0; idx < P2P_MAX_NUM; idx++) {
		if (info[idx].run && info[idx].macid == macid) {
			PLTFM_MSG_ERR("[ERR]macid %d has running p2pid %d\n",
				      macid, idx);
			*p2pid = P2PID_INVALID;
			return MACSUCCESS;
		}
	}

	for (idx = 0; idx < P2P_MAX_NUM; idx++) {
		if (!info[idx].run && !info[idx].wait_dack) {
			*p2pid = idx;
			return MACSUCCESS;
		}
	}

	PLTFM_MSG_ERR("[ERR]no valid p2p\n");
	*p2pid = P2PID_INVALID;
	return MACHWNOSUP;
}

static u32 _get_macid_p2pid(struct mac_ax_adapter *adapter, u8 macid, u8 *p2pid)
{
	struct mac_ax_p2p_info *info = adapter->p2p_info;
	u8 idx;

	for (idx = 0; idx < P2P_MAX_NUM; idx++) {
		if (info[idx].run && info[idx].macid == macid) {
			*p2pid = idx;
			return MACSUCCESS;
		}
	}

	PLTFM_MSG_ERR("[ERR]no valid p2pid for macid %d\n", macid);
	*p2pid = P2PID_INVALID;
	return MACFUNCINPUT;
}

u32 get_wait_dack_p2pid(struct mac_ax_adapter *adapter, u8 *p2pid)
{
	struct mac_ax_p2p_info *info = adapter->p2p_info;
	u8 idx;
	u8 hit = 0;

	for (idx = 0; idx < P2P_MAX_NUM; idx++) {
		if (info[idx].wait_dack && !hit) {
			*p2pid = idx;
			hit = 1;
		} else if (info[idx].wait_dack) {
			PLTFM_MSG_ERR("[ERR]multiple wait dack p2p\n");
			return MACPROCERR;
		}
	}

	if (!hit)
		*p2pid = P2PID_INVALID;

	return MACSUCCESS;
}

u32 p2p_info_init(struct mac_ax_adapter *adapter)
{
	u32 i;

	adapter->p2p_info =
		(struct mac_ax_p2p_info *)PLTFM_MALLOC(p2p_info_size);
	for (i = 0; i < P2P_MAX_NUM; i++)
		PLTFM_MEMSET(&adapter->p2p_info[i], 0,
			     sizeof(struct mac_ax_p2p_info));

	return MACSUCCESS;
}

u32 p2p_info_exit(struct mac_ax_adapter *adapter)
{
	PLTFM_FREE(adapter->p2p_info, p2p_info_size);

	return MACSUCCESS;
}

u32 rst_p2p_info(struct mac_ax_adapter *adapter)
{
	PLTFM_MEMSET(adapter->p2p_info, 0, p2p_info_size);

	return MACSUCCESS;
}

u32 mac_p2p_act_h2c(struct mac_ax_adapter *adapter,
		    struct mac_ax_p2p_act_info *info)
{
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_p2p_act *hdr;
	u32 ret = MACSUCCESS;
	u8 p2pid;

	if (info->noaid >= NOA_MAX_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid noaid %d\n", info->noaid);
		return MACFUNCINPUT;
	}

	if (info->act == P2P_ACT_INIT) {
		ret = _get_valid_p2pid(adapter, info->macid, &p2pid);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]get valid p2pid %d\n", ret);
			return MACHWNOSUP;
		}
		if (p2pid == P2PID_INVALID) {
			PLTFM_MSG_ERR("[ERR]get invalid p2pid\n");
			return MACNOITEM;
		}
	} else {
		ret = _get_macid_p2pid(adapter, info->macid, &p2pid);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]get macid %d p2pid %d\n",
				      info->macid, ret);
			return MACFUNCINPUT;
		}
		if (p2pid == P2PID_INVALID) {
			PLTFM_MSG_ERR("[ERR]get macid p2pid shall ret err\n");
			return MACCMP;
		}
	}

	if (adapter->sm.p2p_stat == MAC_AX_P2P_ACT_BUSY) {
		PLTFM_MSG_ERR("[ERR] p2p act h2c stat busy\n");
		return MACPROCERR;
	}
	adapter->sm.p2p_stat = MAC_AX_P2P_ACT_BUSY;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_p2p_act *)
	      h2cb_put(h2cb, sizeof(struct fwcmd_p2p_act));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(info->macid, FWCMD_H2C_P2P_ACT_MACID) |
			    SET_WORD(p2pid, FWCMD_H2C_P2P_ACT_P2PID) |
			    SET_WORD(info->noaid, FWCMD_H2C_P2P_ACT_NOAID) |
			    SET_WORD(info->act, FWCMD_H2C_P2P_ACT_ACT) |
			    (info->type ? FWCMD_H2C_P2P_ACT_TYPE : 0) |
			    (info->all_slep ? FWCMD_H2C_P2P_ACT_ALL_SLEP : 0));

	hdr->dword1 =
		cpu_to_le32(SET_WORD(info->srt, FWCMD_H2C_P2P_ACT_SRT));

	hdr->dword2 =
		cpu_to_le32(SET_WORD(info->itvl, FWCMD_H2C_P2P_ACT_ITVL));

	hdr->dword3 =
		cpu_to_le32(SET_WORD(info->dur, FWCMD_H2C_P2P_ACT_DUR));

	hdr->dword4 =
		cpu_to_le32(SET_WORD(info->cnt, FWCMD_H2C_P2P_ACT_CNT) |
			    SET_WORD(info->ctw, FWCMD_H2C_P2P_ACT_CTW));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PS,
			      FWCMD_H2C_FUNC_P2P_ACT,
			      0,
			      1);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	if (info->act == P2P_ACT_INIT) {
		adapter->p2p_info[p2pid].macid = info->macid;
		adapter->p2p_info[p2pid].wait_init = 1;
	}
	if (info->act == P2P_ACT_TERM)
		adapter->p2p_info[p2pid].wait_term = 1;

	adapter->p2p_info[p2pid].wait_dack = 1;

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_p2p_macid_ctrl_h2c(struct mac_ax_adapter *adapter,
			   struct mac_ax_p2p_macid_info *info)
{
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_p2p_macid_ctrl *hdr;
	u32 ret = MACSUCCESS;
	u32 idx, bmap_ext_size, pldsize, bmap_srt_sh, bmap_last;
	u32 bmap_idx_srt, bmap_idx_end, bmap_idx_diff;
	u8 *curr_buf;
	u8 p2pid;

	if (info->ctrl_type >= P2P_MACID_CTRL_MAX) {
		PLTFM_MSG_ERR("[ERR]p2p macid ctrl invalid ctrl type %d\n",
			      info->ctrl_type);
		return MACFUNCINPUT;
	}

	ret = _get_macid_p2pid(adapter, info->main_macid, &p2pid);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]get macid %d p2pid %d\n",
			      info->main_macid, ret);
		return MACFUNCINPUT;
	}
	if (p2pid == P2PID_INVALID) {
		PLTFM_MSG_ERR("[ERR]get macid p2pid shall ret err\n");
		return MACNOITEM;
	}

	bmap_idx_srt = 0;
	for (idx = 0; idx < info->bmap_len; idx++) {
		if (*(info->bitmap + idx))
			break;
		if (idx && !(idx & MACID_BMAP_BYTE_MSK))
			bmap_idx_srt++;
	}

	bmap_idx_end = info->bmap_len >> MACID_BMAP_BYTE_SH;
	bmap_last = info->bmap_len ? (info->bmap_len - 1) : 0;
	for (idx = bmap_last; idx > 0; idx--) {
		if (*(info->bitmap + idx))
			break;
		if (!(idx & MACID_BMAP_BYTE_MSK))
			bmap_idx_end--;
	}

	if (bmap_idx_srt > bmap_idx_end) {
		PLTFM_MSG_ERR("[ERR] p2p macid ctrl bitmap comp err\n");
		return MACCMP;
	}

	bmap_idx_diff = bmap_idx_end - bmap_idx_srt;
	bmap_ext_size = bmap_idx_diff << MACID_BMAP_BYTE_SH;

	if (adapter->sm.p2p_stat == MAC_AX_P2P_ACT_BUSY) {
		PLTFM_MSG_ERR("[ERR] p2p macid ctrl h2c stat busy\n");
		return MACPROCERR;
	}
	adapter->sm.p2p_stat = MAC_AX_P2P_ACT_BUSY;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	pldsize = sizeof(struct fwcmd_p2p_macid_ctrl) + bmap_ext_size;
	hdr = (struct fwcmd_p2p_macid_ctrl *)h2cb_put(h2cb, pldsize);
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(p2pid, FWCMD_H2C_P2P_MACID_CTRL_P2PID) |
			    SET_WORD(info->ctrl_type,
				     FWCMD_H2C_P2P_MACID_CTRL_CTRL_TYPE) |
			    SET_WORD((bmap_idx_srt << MACID_BMAP_BIT_SH),
				     FWCMD_H2C_P2P_MACID_CTRL_MACID_SRT) |
			    SET_WORD((bmap_idx_diff + 1),
				     FWCMD_H2C_P2P_MACID_CTRL_BMAP_LEN));

	for (idx = 0; idx <= ((bmap_idx_diff + 1) << MACID_BMAP_BYTE_SH); idx++) {
		bmap_srt_sh = (bmap_idx_srt << MACID_BMAP_BYTE_SH) + idx;
		curr_buf = (u8 *)(&hdr->dword1) + idx;
		if (bmap_srt_sh < info->bmap_len)
			*curr_buf = *(info->bitmap + bmap_srt_sh);
		else
			*curr_buf = 0;
	}

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_PS,
			      FWCMD_H2C_FUNC_P2P_MACID_CTRL,
			      0,
			      1);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	adapter->p2p_info[p2pid].wait_dack = 1;

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_get_p2p_stat(struct mac_ax_adapter *adapter)
{
	switch (adapter->sm.p2p_stat) {
	case MAC_AX_P2P_ACT_IDLE:
		return MACSUCCESS;
	case MAC_AX_P2P_ACT_BUSY:
		return MACPROCBUSY;
	case MAC_AX_P2P_ACT_FAIL:
	default:
		return MACP2PSTFAIL;
	}
}

