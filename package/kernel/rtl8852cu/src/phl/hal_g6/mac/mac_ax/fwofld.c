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

#include "fwdl.h"
#include "fwofld.h"

static u32 get_io_ofld_cap(struct mac_ax_adapter *adapter, u32 *val)
{
	*val |= FW_CAP_IO_OFLD;

	return MACSUCCESS;
}

u32 mac_get_fw_cap(struct mac_ax_adapter *adapter, u32 *val)
{
	*val = 0;
	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACFWNONRDY;

	get_io_ofld_cap(adapter, val);

	return MACSUCCESS;
}

u32 mac_get_wlanfw_cap(struct mac_ax_adapter *adapter, struct rtw_wcpu_cap_t *wcpu_cap)
{
	u32 offset = 0;
	u32 cap_len;
	u32 defined_cap_len;
	u32 len_to_cp;
	u32 sizeof_cur_blk;
	struct mac_wlanfw_cap_hdr *cap_hdr;
	u32 size = adapter->fw_info.cap_size;
	u8 *content = adapter->fw_info.cap_buff;
	u8 mod_id;
	u8 mod_idx;
	u8 *target_addr;

	PLTFM_MEMSET(wcpu_cap, 0, sizeof(struct rtw_wcpu_cap_t));
	if (size > MAC_WLANFW_CAP_MAX_SIZE) {
		PLTFM_MSG_ERR("[FwCap] Size (%d) exceeds def (%d)", size, MAC_WLANFW_CAP_MAX_SIZE);
		return MACBUFSZ;
	}
	while (offset < size) {
		sizeof_cur_blk = *content;
		content++;
		offset++;
		if (sizeof_cur_blk < sizeof(struct mac_wlanfw_cap_hdr)) {
			content += sizeof_cur_blk;
			offset += sizeof_cur_blk;
			continue;
		}
		cap_hdr = (struct mac_wlanfw_cap_hdr *)content;
		PLTFM_MSG_TRACE("[FwCap] magic_code (%x), num_mods (%d)\n",
				cap_hdr->magic_code, cap_hdr->num_mods);
		if (cap_hdr->magic_code != MAC_WLANFW_CAP_MAGIC_CODE) {
			content += sizeof_cur_blk;
			offset += sizeof_cur_blk;
			continue;
		}
		content += sizeof(struct mac_wlanfw_cap_hdr);
		sizeof_cur_blk -= sizeof(struct mac_wlanfw_cap_hdr);

		for (mod_idx = 0; mod_idx < cap_hdr->num_mods; mod_idx++) {
			if (sizeof_cur_blk < 2) {
				PLTFM_MSG_ERR("[FwCap] No enough space for modId and capLen\n");
				PLTFM_MEMSET(wcpu_cap, 0, sizeof(struct rtw_wcpu_cap_t));
				return MACNOITEM;
			}
			mod_id = *content++;
			cap_len = ((u32)*content++) * 4;
			sizeof_cur_blk -= 2;
			PLTFM_MSG_TRACE("[FwCap] mod (%x), capLen (%d) byte\n", mod_id, cap_len);
			if (cap_len > sizeof_cur_blk) {
				PLTFM_MSG_ERR("[FwCap] No enough space for mod (%x) * %d Byte\n",
					      mod_id, cap_len);
				PLTFM_MEMSET(wcpu_cap, 0, sizeof(struct rtw_wcpu_cap_t));
				return MACNOITEM;
			}

			switch (mod_id) {
			case MAC_WLANFW_MAC_CAP_SUBID:
				defined_cap_len = sizeof(wcpu_cap->mac_ofld_cap);
				target_addr = (u8 *)&wcpu_cap->mac_ofld_cap;
				break;
			case MAC_WLANFW_BB_CAP_SUBID:
				defined_cap_len = sizeof(wcpu_cap->bb_ofld_cap);
				target_addr = (u8 *)&wcpu_cap->bb_ofld_cap;
				break;
			case MAC_WLANFW_RF_CAP_SUBID:
				defined_cap_len = sizeof(wcpu_cap->rf_ofld_cap);
				target_addr = (u8 *)&wcpu_cap->rf_ofld_cap;
				break;
			case MAC_WLANFW_BTC_CAP_SUBID:
				defined_cap_len = sizeof(wcpu_cap->btc_ofld_cap);
				target_addr = (u8 *)&wcpu_cap->btc_ofld_cap;
				break;
			default:
				PLTFM_MSG_ERR("[FwCap] Unknown modId (%x), abort\n", mod_id);
				PLTFM_MEMSET(wcpu_cap, 0, sizeof(struct rtw_wcpu_cap_t));
				return MACNOITEM;
			}
			len_to_cp = (cap_len < defined_cap_len) ? cap_len : defined_cap_len;
			PLTFM_MSG_TRACE("[FwCap] cp %d bytes\n", len_to_cp);
			PLTFM_MEMCPY(target_addr, content, len_to_cp);
			content += cap_len;
			sizeof_cur_blk -= cap_len;
		}
		return MACSUCCESS;
	}
	PLTFM_MSG_ERR("[FwCap] wcpu cap not found.\n");
	return MACNOITEM;
}

static inline void mac_pkt_ofld_set_bitmap(u8 *bitmap, u16 index)
{
	bitmap[index >> 3] |= (1 << (index & 7));
}

static inline void mac_pkt_ofld_unset_bitmap(struct mac_ax_adapter *adapter, u8 *bitmap, u16 index)
{
	if (index == PKT_OFLD_NOT_EXISTS_ID) {
		PLTFM_MSG_ERR("pktofld id %d is for NOT_EXISTS and shouldn't be unset\n",
			      PKT_OFLD_NOT_EXISTS_ID);
		return;
	}
	bitmap[index >> 3] &= ~(1 << (index & 7));
}

static inline u8 mac_pkt_ofld_get_bitmap(u8 *bitmap, u16 index)
{
	if (index == PKT_OFLD_NOT_EXISTS_ID)
		return 1;
	return bitmap[index / 8] & (1 << (index & 7)) ? 1 : 0;
}

u32 mac_reset_fwofld_state(struct mac_ax_adapter *adapter, u8 op)
{
	switch (op) {
	case FW_OFLD_OP_DUMP_EFUSE:
		adapter->sm.efuse_ofld = MAC_AX_OFLD_H2C_IDLE;
		break;

	case FW_OFLD_OP_PACKET_OFLD:
		PLTFM_MSG_TRACE("%s: set pktofld st to idle and clear occupied ids\n", __func__);
		adapter->sm.pkt_ofld = MAC_AX_OFLD_H2C_IDLE;
		PLTFM_MEMSET(adapter->pkt_ofld_info.id_bitmap, 0, (PKT_OFLD_MAX_COUNT >> 3));
		mac_pkt_ofld_set_bitmap(adapter->pkt_ofld_info.id_bitmap, PKT_OFLD_NOT_EXISTS_ID);
		adapter->pkt_ofld_info.last_op = PKT_OFLD_OP_MAX;
		adapter->pkt_ofld_info.free_id_count = PKT_OFLD_MAX_VALID_ID_NUM;
		adapter->pkt_ofld_info.used_id_count = 1;
		break;

	case FW_OFLD_OP_READ_OFLD:
		adapter->sm.read_request = MAC_AX_OFLD_REQ_IDLE;
		adapter->sm.read_h2c = MAC_AX_OFLD_H2C_IDLE;
		break;

	case FW_OFLD_OP_WRITE_OFLD:
		adapter->sm.write_request = MAC_AX_OFLD_REQ_IDLE;
		adapter->sm.write_h2c = MAC_AX_OFLD_H2C_IDLE;
		break;

	case FW_OFLD_OP_CONF_OFLD:
		adapter->sm.conf_request = MAC_AX_OFLD_REQ_IDLE;
		adapter->sm.conf_h2c = MAC_AX_OFLD_H2C_IDLE;
		break;
	case FW_OFLD_OP_CH_SWITCH:
		adapter->sm.ch_switch = MAC_AX_OFLD_H2C_IDLE;
		break;

	default:
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 mac_check_fwofld_done(struct mac_ax_adapter *adapter, u8 op)
{
	struct mac_ax_pkt_ofld_info *ofld_info = &adapter->pkt_ofld_info;

	switch (op) {
	case FW_OFLD_OP_DUMP_EFUSE:
		if (adapter->sm.efuse_ofld == MAC_AX_OFLD_H2C_IDLE)
			return MACSUCCESS;
		break;

	case FW_OFLD_OP_PACKET_OFLD:
		if (ofld_info->last_op == PKT_OFLD_OP_READ) {
			if (adapter->sm.pkt_ofld == MAC_AX_OFLD_H2C_DONE)
				return MACSUCCESS;
		} else {
			if (adapter->sm.pkt_ofld == MAC_AX_OFLD_H2C_IDLE)
				return MACSUCCESS;
		}
		break;
	case FW_OFLD_OP_READ_OFLD:
		if (adapter->sm.read_h2c == MAC_AX_OFLD_H2C_DONE)
			return MACSUCCESS;
		break;
	case FW_OFLD_OP_WRITE_OFLD:
		if (adapter->sm.write_h2c == MAC_AX_OFLD_H2C_IDLE)
			return MACSUCCESS;
		break;
	case FW_OFLD_OP_CONF_OFLD:
		if (adapter->sm.conf_h2c == MAC_AX_OFLD_H2C_IDLE)
			return MACSUCCESS;
		break;
	case FW_OFLD_OP_CH_SWITCH:
		if (adapter->sm.ch_switch == MAC_AX_OFLD_H2C_IDLE ||
		    adapter->sm.ch_switch == MAC_AX_CH_SWITCH_GET_RPT)
			return MACSUCCESS;
		break;
	default:
		return MACNOITEM;
	}

	return MACPROCBUSY;
}

static u32 cnv_write_ofld_state(struct mac_ax_adapter *adapter, u8 dest)
{
	u8 state;

	state = adapter->sm.write_request;

	if (state > MAC_AX_OFLD_REQ_CLEANED)
		return MACPROCERR;

	if (dest == MAC_AX_OFLD_REQ_IDLE) {
		if (state != MAC_AX_OFLD_REQ_H2C_SENT)
			return MACPROCERR;
	} else if (dest == MAC_AX_OFLD_REQ_CLEANED) {
		if (state == MAC_AX_OFLD_REQ_H2C_SENT)
			return MACPROCERR;
	} else if (dest == MAC_AX_OFLD_REQ_CREATED) {
		if (state == MAC_AX_OFLD_REQ_IDLE ||
		    state == MAC_AX_OFLD_REQ_H2C_SENT)
			return MACPROCERR;
	} else if (dest == MAC_AX_OFLD_REQ_H2C_SENT) {
		if (state != MAC_AX_OFLD_REQ_CREATED)
			return MACPROCERR;
	}

	adapter->sm.write_request = dest;

	return MACSUCCESS;
}

u32 mac_clear_write_request(struct mac_ax_adapter *adapter)
{
	if (adapter->sm.write_request == MAC_AX_OFLD_REQ_H2C_SENT)
		return MACPROCERR;

	if (cnv_write_ofld_state(adapter, MAC_AX_OFLD_REQ_CLEANED)
	    != MACSUCCESS)
		return MACPROCERR;

	PLTFM_FREE(adapter->write_ofld_info.buf,
		   adapter->write_ofld_info.buf_size);
	adapter->write_ofld_info.buf = NULL;
	adapter->write_ofld_info.buf_wptr = NULL;
	adapter->write_ofld_info.last_req = NULL;
	adapter->write_ofld_info.buf_size = 0;
	adapter->write_ofld_info.avl_buf_size = 0;
	adapter->write_ofld_info.used_size = 0;
	adapter->write_ofld_info.req_num = 0;

	return MACSUCCESS;
}

u32 mac_add_write_request(struct mac_ax_adapter *adapter,
			  struct mac_ax_write_req *req, u8 *value, u8 *mask)
{
	struct mac_ax_write_ofld_info *ofld_info = &adapter->write_ofld_info;
	struct fwcmd_write_ofld_req *write_ptr;
	u32 data_len = 0;
	u8 state;

	state = adapter->sm.write_request;

	if (!(state == MAC_AX_OFLD_REQ_CREATED ||
	      state == MAC_AX_OFLD_REQ_CLEANED)) {
		return MACPROCERR;
	}

	if (!ofld_info->buf) {
		ofld_info->buf = (u8 *)PLTFM_MALLOC(WRITE_OFLD_MAX_LEN);
		if (!ofld_info->buf)
			return MACNPTR;
		ofld_info->buf_wptr = ofld_info->buf;
		ofld_info->buf_size = WRITE_OFLD_MAX_LEN;
		ofld_info->avl_buf_size = WRITE_OFLD_MAX_LEN;
		ofld_info->used_size = 0;
		ofld_info->req_num = 0;
	}

	data_len = sizeof(struct mac_ax_write_req);
	data_len += req->value_len;
	if (req->mask_en == 1)
		data_len += req->value_len;

	if (ofld_info->avl_buf_size < data_len)
		return MACNOBUF;

	if (!value)
		return MACNPTR;

	if (req->mask_en == 1 && !mask)
		return MACNPTR;

	if (cnv_write_ofld_state(adapter,
				 MAC_AX_OFLD_REQ_CREATED) != MACSUCCESS)
		return MACPROCERR;

	if (ofld_info->req_num != 0)
		ofld_info->last_req->ls = 0;

	ofld_info->last_req = (struct mac_ax_write_req *)ofld_info->buf_wptr;

	req->ls = 1;

	write_ptr = (struct fwcmd_write_ofld_req *)ofld_info->buf_wptr;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(req->value_len,
			     FWCMD_H2C_WRITE_OFLD_REQ_VALUE_LEN) |
		    SET_WORD(req->ofld_id,
			     FWCMD_H2C_WRITE_OFLD_REQ_OFLD_ID) |
		    SET_WORD(req->entry_num,
			     FWCMD_H2C_WRITE_OFLD_REQ_ENTRY_NUM) |
		    req->polling | req->mask_en | req->ls
	);

	write_ptr->dword1 =
	cpu_to_le32(SET_WORD(req->offset,
			     FWCMD_H2C_WRITE_OFLD_REQ_OFFSET)
	);

	ofld_info->buf_wptr += sizeof(struct mac_ax_write_req);
	ofld_info->avl_buf_size -= sizeof(struct mac_ax_write_req);
	ofld_info->used_size += sizeof(struct mac_ax_write_req);

	PLTFM_MEMCPY(ofld_info->buf_wptr, value, req->value_len);

	ofld_info->buf_wptr += req->value_len;
	ofld_info->avl_buf_size -= req->value_len;
	ofld_info->used_size += req->value_len;

	if (req->mask_en == 1) {
		PLTFM_MEMCPY(ofld_info->buf_wptr, mask, req->value_len);
		ofld_info->buf_wptr += req->value_len;
		ofld_info->avl_buf_size -= req->value_len;
		ofld_info->used_size += req->value_len;
	}

	ofld_info->req_num++;

	return MACSUCCESS;
}

u32 mac_write_ofld(struct mac_ax_adapter *adapter)
{
	u8 *buf;
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct mac_ax_write_ofld_info *ofld_info = &adapter->write_ofld_info;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	if (ofld_info->used_size + FWCMD_HDR_LEN > READ_OFLD_MAX_LEN)
		return MACBUFSZ;

	if (adapter->sm.write_h2c != MAC_AX_OFLD_H2C_IDLE)
		return MACPROCERR;

	if (adapter->sm.write_request != MAC_AX_OFLD_REQ_CREATED)
		return MACPROCERR;

	if (cnv_write_ofld_state(adapter,
				 MAC_AX_OFLD_REQ_H2C_SENT) != MACSUCCESS)
		return MACPROCERR;

	adapter->sm.write_h2c = MAC_AX_OFLD_H2C_SENDING;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, ofld_info->used_size);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMCPY(buf, ofld_info->buf, ofld_info->used_size);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_WRITE_OFLD,
			      1, 1);

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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		adapter->sm.write_request = MAC_AX_OFLD_REQ_IDLE;
		adapter->sm.write_h2c = MAC_AX_OFLD_H2C_IDLE;
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	if (cnv_write_ofld_state(adapter, MAC_AX_OFLD_REQ_IDLE) != MACSUCCESS)
		return MACPROCERR;

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

static u32 cnv_conf_ofld_state(struct mac_ax_adapter *adapter, u8 dest)
{
	u8 state;

	state = adapter->sm.conf_request;

	if (state > MAC_AX_OFLD_REQ_CLEANED)
		return MACPROCERR;

	if (dest == MAC_AX_OFLD_REQ_IDLE) {
		if (state != MAC_AX_OFLD_REQ_H2C_SENT)
			return MACPROCERR;
	} else if (dest == MAC_AX_OFLD_REQ_CLEANED) {
		if (state == MAC_AX_OFLD_REQ_H2C_SENT)
			return MACPROCERR;
	} else if (dest == MAC_AX_OFLD_REQ_CREATED) {
		if (state == MAC_AX_OFLD_REQ_IDLE ||
		    state == MAC_AX_OFLD_REQ_H2C_SENT)
			return MACPROCERR;
	} else if (dest == MAC_AX_OFLD_REQ_H2C_SENT) {
		if (state != MAC_AX_OFLD_REQ_CREATED)
			return MACPROCERR;
	}

	adapter->sm.conf_request = dest;

	return MACSUCCESS;
}

u32 mac_clear_conf_request(struct mac_ax_adapter *adapter)
{
	if (adapter->sm.conf_request == MAC_AX_OFLD_REQ_H2C_SENT)
		return MACPROCERR;

	if (cnv_conf_ofld_state(adapter, MAC_AX_OFLD_REQ_CLEANED) !=
	    MACSUCCESS)
		return MACPROCERR;

	PLTFM_FREE(adapter->conf_ofld_info.buf,
		   adapter->conf_ofld_info.buf_size);
	adapter->conf_ofld_info.buf = NULL;
	adapter->conf_ofld_info.buf_wptr = NULL;
	adapter->conf_ofld_info.buf_size = 0;
	adapter->conf_ofld_info.avl_buf_size = 0;
	adapter->conf_ofld_info.used_size = 0;
	adapter->conf_ofld_info.req_num = 0;

	return MACSUCCESS;
}

u32 mac_add_conf_request(struct mac_ax_adapter *adapter,
			 struct mac_ax_conf_ofld_req *req)
{
	struct mac_ax_conf_ofld_info *ofld_info = &adapter->conf_ofld_info;
	struct fwcmd_conf_ofld_req_cmd *write_ptr;
	u8 state;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	state = adapter->sm.conf_request;

	if (!(state == MAC_AX_OFLD_REQ_CREATED ||
	      state == MAC_AX_OFLD_REQ_CLEANED)) {
		return MACPROCERR;
	}

	if (!ofld_info->buf) {
		ofld_info->buf = (u8 *)PLTFM_MALLOC(CONF_OFLD_MAX_LEN);
		if (!ofld_info->buf)
			return MACNPTR;
		ofld_info->buf_wptr = ofld_info->buf;
		ofld_info->buf_size = CONF_OFLD_MAX_LEN;
		ofld_info->avl_buf_size = CONF_OFLD_MAX_LEN;
		ofld_info->used_size = 0;
		ofld_info->req_num = 0;
	}

	if (ofld_info->avl_buf_size < sizeof(struct mac_ax_conf_ofld_req))
		return MACNOBUF;

	if (cnv_conf_ofld_state(adapter, MAC_AX_OFLD_REQ_CREATED) != MACSUCCESS)
		return MACPROCERR;

	write_ptr = (struct fwcmd_conf_ofld_req_cmd *)ofld_info->buf_wptr;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(req->device,
			     FWCMD_H2C_CONF_OFLD_REQ_CMD_DEVICE)
	);

	write_ptr->dword1 =
	cpu_to_le32(SET_WORD(req->req.hioe.hioe_op,
			     FWCMD_H2C_CONF_OFLD_REQ_CMD_HIOE_OP) |
		    SET_WORD(req->req.hioe.inst_type,
			     FWCMD_H2C_CONF_OFLD_REQ_CMD_INST_TYPE) |
		    SET_WORD(req->req.hioe.data_mode,
			     FWCMD_H2C_CONF_OFLD_REQ_CMD_DATA_MODE)
	);

	write_ptr->dword2 = cpu_to_le32(req->req.hioe.param0.register_addr);

	write_ptr->dword3 =
	cpu_to_le32(SET_WORD(req->req.hioe.param1.byte_data_h,
			     FWCMD_H2C_CONF_OFLD_REQ_CMD_BYTE_DATA_H) |
		    SET_WORD(req->req.hioe.param2.byte_data_l,
			     FWCMD_H2C_CONF_OFLD_REQ_CMD_BYTE_DATA_L)
	);

	ofld_info->buf_wptr += sizeof(struct mac_ax_conf_ofld_req);
	ofld_info->avl_buf_size -= sizeof(struct mac_ax_conf_ofld_req);
	ofld_info->used_size += sizeof(struct mac_ax_conf_ofld_req);

	ofld_info->req_num++;

	return MACSUCCESS;
}

u32 mac_conf_ofld(struct mac_ax_adapter *adapter)
{
	u8 *buf;
	u32 ret;
	struct fwcmd_conf_ofld *write_ptr;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct mac_ax_conf_ofld_info *ofld_info = &adapter->conf_ofld_info;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	if (ofld_info->used_size + FWCMD_HDR_LEN > CONF_OFLD_MAX_LEN)
		return MACBUFSZ;

	if (adapter->sm.conf_h2c != MAC_AX_OFLD_H2C_IDLE)
		return MACPROCERR;

	if (adapter->sm.conf_request != MAC_AX_OFLD_REQ_CREATED)
		return MACPROCERR;

	if (cnv_conf_ofld_state(adapter,
				MAC_AX_OFLD_REQ_H2C_SENT) != MACSUCCESS)
		return MACPROCERR;

	adapter->sm.conf_h2c = MAC_AX_OFLD_H2C_SENDING;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_conf_ofld_hdr));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	write_ptr = (struct fwcmd_conf_ofld *)buf;

	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(ofld_info->req_num,
			     FWCMD_H2C_CONF_OFLD_PATTERN_COUNT));

	buf = h2cb_put(h2cb, ofld_info->used_size);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMCPY(buf, ofld_info->buf, ofld_info->used_size);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_CONF_OFLD,
			      1, 1);
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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		adapter->sm.conf_request = MAC_AX_OFLD_REQ_IDLE;
		adapter->sm.conf_h2c = MAC_AX_OFLD_H2C_IDLE;
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	if (cnv_conf_ofld_state(adapter, MAC_AX_OFLD_REQ_IDLE) != MACSUCCESS)
		return MACPROCERR;

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_read_pkt_ofld(struct mac_ax_adapter *adapter, u8 id)
{
	u8 *buf;
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_packet_ofld *write_ptr;
	struct mac_ax_pkt_ofld_info *ofld_info = &adapter->pkt_ofld_info;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	if (id == PKT_OFLD_NOT_EXISTS_ID)
		return MACNOITEM;

	if (mac_pkt_ofld_get_bitmap(ofld_info->id_bitmap, id) == 0)
		return MACNOITEM;

	if (adapter->sm.pkt_ofld != MAC_AX_OFLD_H2C_IDLE)
		return MACPROCERR;

	adapter->sm.pkt_ofld = MAC_AX_OFLD_H2C_SENDING;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_pkt_ofld_hdr));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	write_ptr = (struct fwcmd_packet_ofld *)buf;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(id, FWCMD_H2C_PACKET_OFLD_PKT_IDX) |
		    SET_WORD(PKT_OFLD_OP_READ, FWCMD_H2C_PACKET_OFLD_PKT_OP)
	);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_PACKET_OFLD,
			      1, 1);
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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		adapter->sm.pkt_ofld = MAC_AX_OFLD_H2C_IDLE;
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	ofld_info->last_op = PKT_OFLD_OP_READ;

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_del_pkt_ofld(struct mac_ax_adapter *adapter, u8 id)
{
	u8 *buf;
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_packet_ofld *write_ptr;
	struct mac_ax_pkt_ofld_info *ofld_info = &adapter->pkt_ofld_info;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	if (id == PKT_OFLD_NOT_EXISTS_ID)
		return MACNOITEM;

	if (mac_pkt_ofld_get_bitmap(ofld_info->id_bitmap, id) == 0)
		return MACNOITEM;

	if (ofld_info->used_id_count == 0)
		return MACNOITEM;

	if (adapter->sm.pkt_ofld != MAC_AX_OFLD_H2C_IDLE)
		return MACPROCERR;

	adapter->sm.pkt_ofld = MAC_AX_OFLD_H2C_SENDING;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_pkt_ofld_hdr));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	write_ptr = (struct fwcmd_packet_ofld *)buf;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(id, FWCMD_H2C_PACKET_OFLD_PKT_IDX) |
		    SET_WORD(PKT_OFLD_OP_DEL, FWCMD_H2C_PACKET_OFLD_PKT_OP)
	);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_PACKET_OFLD,
			      1, 1);
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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		adapter->sm.pkt_ofld = MAC_AX_OFLD_H2C_IDLE;
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	ofld_info->last_op = PKT_OFLD_OP_DEL;

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_add_pkt_ofld(struct mac_ax_adapter *adapter, u8 *pkt, u16 len, u8 *id)
{
	u8 *buf;
	u16 alloc_id;
	u32 ret;
	u32 sizeof_pkt_ofld_hdr = sizeof(struct mac_ax_pkt_ofld_hdr);
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_packet_ofld *write_ptr;
	struct mac_ax_pkt_ofld_info *ofld_info = &adapter->pkt_ofld_info;
	enum h2c_buf_class h2cb_type;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	if (ofld_info->free_id_count == 0)
		return MACNOBUF;

	if (adapter->sm.pkt_ofld != MAC_AX_OFLD_H2C_IDLE)
		return MACPROCERR;

	adapter->sm.pkt_ofld = MAC_AX_OFLD_H2C_SENDING;

	for (alloc_id = 0; alloc_id < PKT_OFLD_MAX_COUNT - 1; alloc_id++) {
		if (mac_pkt_ofld_get_bitmap(ofld_info->id_bitmap,
					    alloc_id) == 0)
			break;
	}
	if (alloc_id == PKT_OFLD_NOT_EXISTS_ID)
		return MACNOBUF;

	PLTFM_MSG_TRACE("pkt ofld add. alloc_id: %d, free cnt: %d, use cnt: %d\n",
			alloc_id, ofld_info->free_id_count,
			ofld_info->used_id_count);

	if (len + sizeof_pkt_ofld_hdr <= (H2C_CMD_LEN - FWCMD_HDR_LEN)) {
		h2cb_type = H2CB_CLASS_CMD;
		PLTFM_MSG_TRACE("pkt ofld size %d, using CMD Q\n", len);
	} else if (len + sizeof_pkt_ofld_hdr <= (H2C_DATA_LEN - FWCMD_HDR_LEN)) {
		h2cb_type = H2CB_CLASS_DATA;
		PLTFM_MSG_TRACE("pkt ofld size %d, using DATA Q\n", len);
	} else if (len + sizeof_pkt_ofld_hdr <= (H2C_LONG_DATA_LEN - FWCMD_HDR_LEN)) {
		h2cb_type = H2CB_CLASS_LONG_DATA;
		PLTFM_MSG_TRACE("pkt ofld size %d, using LDATA Q\n", len);
	} else {
		PLTFM_MSG_ERR("pkt ofld size %d, exceed LDATA Q size, abort\n", len);
		return MACBUFSZ;
	}
	h2cb = h2cb_alloc(adapter, h2cb_type);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_pkt_ofld_hdr));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	write_ptr = (struct fwcmd_packet_ofld *)buf;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD((u8)alloc_id, FWCMD_H2C_PACKET_OFLD_PKT_IDX) |
		    SET_WORD(PKT_OFLD_OP_ADD, FWCMD_H2C_PACKET_OFLD_PKT_OP) |
		    SET_WORD(len, FWCMD_H2C_PACKET_OFLD_PKT_LENGTH)
	);

	*id = (u8)alloc_id;

	buf = h2cb_put(h2cb, len);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMCPY(buf, pkt, len);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_PACKET_OFLD,
			      1, 1);
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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		adapter->sm.pkt_ofld = MAC_AX_OFLD_H2C_IDLE;
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	ofld_info->last_op = PKT_OFLD_OP_ADD;

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_pkt_ofld_packet(struct mac_ax_adapter *adapter,
			u8 **pkt_buf, u16 *pkt_len, u8 *pkt_id)
{
	struct mac_ax_pkt_ofld_pkt *pkt_info = &adapter->pkt_ofld_pkt;
	*pkt_buf = NULL;

	if (adapter->sm.pkt_ofld != MAC_AX_OFLD_H2C_DONE)
		return MACPROCERR;

	*pkt_buf = (u8 *)PLTFM_MALLOC(pkt_info->pkt_len);
	if (!*pkt_buf)
		return MACBUFALLOC;

	PLTFM_MEMCPY(*pkt_buf, pkt_info->pkt, pkt_info->pkt_len);

	*pkt_len = pkt_info->pkt_len;
	*pkt_id = pkt_info->pkt_id;

	adapter->sm.pkt_ofld = MAC_AX_OFLD_H2C_IDLE;

	return MACSUCCESS;
}

u32 mac_dump_efuse_ofld(struct mac_ax_adapter *adapter, u32 efuse_size,
			u8 type)
{
	u32 ret, size;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct mac_ax_efuse_ofld_info *ofld_info = &adapter->efuse_ofld_info;
	u8 *buf;
	struct fwcmd_dump_efuse *write_ptr;
	u8 is_hidden = 0, is_dav = 0;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	if (adapter->sm.efuse_ofld != MAC_AX_OFLD_H2C_IDLE)
		return MACPROCERR;

	adapter->sm.efuse_ofld = MAC_AX_OFLD_H2C_SENDING;

	size = efuse_size;
	if (!ofld_info->buf) {
		ofld_info->buf = (u8 *)PLTFM_MALLOC(size);
		if (!ofld_info->buf)
			return MACBUFALLOC;
	}

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_pkt_ofld_hdr));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	if (type == DUMP_OFLD_TYPE_HIDDEN)
		is_hidden = 1;
	if (type == DUMP_OFLD_TYPE_DAV)
		is_dav = 1;

	write_ptr = (struct fwcmd_dump_efuse *)buf;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(efuse_size, FWCMD_H2C_DUMP_EFUSE_DUMP_SIZE) |
		    (is_hidden ? FWCMD_H2C_DUMP_EFUSE_IS_HIDDEN : 0) |
		    (is_dav ? FWCMD_H2C_DUMP_EFUSE_IS_DAV : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_DUMP_EFUSE,
			      1, 0);
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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx\n");
		goto fail;
	}

	h2cb_free(adapter, h2cb);
	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);
	return ret;
}

u32 mac_efuse_ofld_map(struct mac_ax_adapter *adapter, u8 *efuse_map,
		       u32 efuse_size)
{
	u32 size = efuse_size;
	struct mac_ax_efuse_ofld_info *ofld_info = &adapter->efuse_ofld_info;

	if (adapter->sm.efuse_ofld != MAC_AX_OFLD_H2C_DONE)
		return MACPROCERR;

	PLTFM_MEMCPY(efuse_map, ofld_info->buf, size);

	adapter->sm.efuse_ofld = MAC_AX_OFLD_H2C_IDLE;

	return MACSUCCESS;
}

static u32 cnv_read_ofld_state(struct mac_ax_adapter *adapter, u8 dest)
{
	u8 state;

	state = adapter->sm.read_request;

	if (state > MAC_AX_OFLD_REQ_CLEANED)
		return MACPROCERR;

	if (dest == MAC_AX_OFLD_REQ_IDLE) {
		if (state != MAC_AX_OFLD_REQ_H2C_SENT)
			return MACPROCERR;
	} else if (dest == MAC_AX_OFLD_REQ_CLEANED) {
		if (state == MAC_AX_OFLD_REQ_H2C_SENT)
			return MACPROCERR;
	} else if (dest == MAC_AX_OFLD_REQ_CREATED) {
		if (state == MAC_AX_OFLD_REQ_IDLE ||
		    state == MAC_AX_OFLD_REQ_H2C_SENT)
			return MACPROCERR;
	} else if (dest == MAC_AX_OFLD_REQ_H2C_SENT) {
		if (state != MAC_AX_OFLD_REQ_CREATED)
			return MACPROCERR;
	}

	adapter->sm.read_request = dest;

	return MACSUCCESS;
}

u32 mac_clear_read_request(struct mac_ax_adapter *adapter)
{
	if (adapter->sm.read_request == MAC_AX_OFLD_REQ_H2C_SENT)
		return MACPROCERR;

	if (cnv_read_ofld_state(adapter, MAC_AX_OFLD_REQ_CLEANED)
	    != MACSUCCESS)
		return MACPROCERR;

	PLTFM_FREE(adapter->read_ofld_info.buf,
		   adapter->read_ofld_info.buf_size);
	adapter->read_ofld_info.buf = NULL;
	adapter->read_ofld_info.buf_wptr = NULL;
	adapter->read_ofld_info.last_req = NULL;
	adapter->read_ofld_info.buf_size = 0;
	adapter->read_ofld_info.avl_buf_size = 0;
	adapter->read_ofld_info.used_size = 0;
	adapter->read_ofld_info.req_num = 0;

	return MACSUCCESS;
}

u32 mac_add_read_request(struct mac_ax_adapter *adapter,
			 struct mac_ax_read_req *req)
{
	struct mac_ax_read_ofld_info *ofld_info = &adapter->read_ofld_info;
	struct fwcmd_read_ofld_req *write_ptr;
	u8 state;

	state = adapter->sm.read_request;

	if (!(state == MAC_AX_OFLD_REQ_CREATED ||
	      state == MAC_AX_OFLD_REQ_CLEANED)) {
		return MACPROCERR;
	}

	if (!ofld_info->buf) {
		ofld_info->buf = (u8 *)PLTFM_MALLOC(READ_OFLD_MAX_LEN);
		if (!ofld_info->buf)
			return MACNPTR;
		ofld_info->buf_wptr = ofld_info->buf;
		ofld_info->buf_size = READ_OFLD_MAX_LEN;
		ofld_info->avl_buf_size = READ_OFLD_MAX_LEN;
		ofld_info->used_size = 0;
		ofld_info->req_num = 0;
	}

	if (ofld_info->avl_buf_size < sizeof(struct mac_ax_read_req))
		return MACNOBUF;

	if (cnv_read_ofld_state(adapter, MAC_AX_OFLD_REQ_CREATED) != MACSUCCESS)
		return MACPROCERR;

	if (ofld_info->req_num != 0)
		ofld_info->last_req->ls = 0;

	ofld_info->last_req = (struct mac_ax_read_req *)ofld_info->buf_wptr;

	req->ls = 1;

	write_ptr = (struct fwcmd_read_ofld_req *)ofld_info->buf_wptr;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(req->value_len,
			     FWCMD_H2C_READ_OFLD_REQ_VALUE_LEN) |
		    SET_WORD(req->ofld_id,
			     FWCMD_H2C_READ_OFLD_REQ_OFLD_ID) |
		    SET_WORD(req->entry_num,
			     FWCMD_H2C_READ_OFLD_REQ_ENTRY_NUM) | req->ls
	);

	write_ptr->dword1 =
	cpu_to_le32(SET_WORD(req->offset,
			     FWCMD_H2C_READ_OFLD_REQ_OFFSET)
	);

	ofld_info->buf_wptr += sizeof(struct mac_ax_read_req);
	ofld_info->avl_buf_size -= sizeof(struct mac_ax_read_req);
	ofld_info->used_size += sizeof(struct mac_ax_read_req);
	ofld_info->req_num++;

	return MACSUCCESS;
}

u32 mac_read_ofld(struct mac_ax_adapter *adapter)
{
	u8 *buf;
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct mac_ax_read_ofld_info *ofld_info = &adapter->read_ofld_info;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	if (ofld_info->used_size + FWCMD_HDR_LEN > READ_OFLD_MAX_LEN)
		return MACBUFSZ;

	if (adapter->sm.read_h2c != MAC_AX_OFLD_H2C_IDLE)
		return MACPROCERR;

	if (adapter->sm.read_request != MAC_AX_OFLD_REQ_CREATED)
		return MACPROCERR;

	if (cnv_read_ofld_state(adapter,
				MAC_AX_OFLD_REQ_H2C_SENT) != MACSUCCESS)
		return MACPROCERR;

	adapter->sm.read_h2c = MAC_AX_OFLD_H2C_SENDING;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, ofld_info->used_size);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMCPY(buf, ofld_info->buf, ofld_info->used_size);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_READ_OFLD,
			      1, 1);

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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		adapter->sm.read_request = MAC_AX_OFLD_REQ_IDLE;
		adapter->sm.read_h2c = MAC_AX_OFLD_H2C_IDLE;
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	if (cnv_read_ofld_state(adapter, MAC_AX_OFLD_REQ_IDLE) != MACSUCCESS)
		return MACPROCERR;

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_read_ofld_value(struct mac_ax_adapter *adapter,
			u8 **val_buf, u16 *val_len)
{
	struct mac_ax_read_ofld_value *value_info = &adapter->read_ofld_value;
	*val_buf = NULL;

	if (adapter->sm.read_h2c != MAC_AX_OFLD_H2C_DONE)
		return MACPROCERR;

	*val_buf = (u8 *)PLTFM_MALLOC(value_info->len);
	if (!*val_buf)
		return MACBUFALLOC;

	PLTFM_MEMCPY(*val_buf, value_info->buf, value_info->len);

	*val_len = value_info->len;

	adapter->sm.read_h2c = MAC_AX_OFLD_H2C_IDLE;

	return MACSUCCESS;
}

u32 mac_general_pkt_ids(struct mac_ax_adapter *adapter,
			struct mac_ax_general_pkt_ids *ids)
{
	u8 *buf;
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_general_pkt *write_ptr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_general_pkt_ids));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	write_ptr = (struct fwcmd_general_pkt *)buf;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(ids->macid, FWCMD_H2C_GENERAL_PKT_MACID) |
		    SET_WORD(ids->probersp, FWCMD_H2C_GENERAL_PKT_PROBRSP_ID) |
		    SET_WORD(ids->pspoll, FWCMD_H2C_GENERAL_PKT_PSPOLL_ID) |
		    SET_WORD(ids->nulldata, FWCMD_H2C_GENERAL_PKT_NULL_ID)
	);

	write_ptr->dword1 =
	cpu_to_le32(SET_WORD(ids->qosnull, FWCMD_H2C_GENERAL_PKT_QOS_NULL_ID) |
		    SET_WORD(ids->cts2self, FWCMD_H2C_GENERAL_PKT_CTS2SELF_ID) |
		    SET_WORD(ids->probereq, FWCMD_H2C_GENERAL_PKT_PROBREQ_ID) |
		    SET_WORD(ids->apcsa, FWCMD_H2C_GENERAL_PKT_APCSA_ID)
	);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_INFO,
			      FWCMD_H2C_FUNC_GENERAL_PKT,
			      1, 1);
	if (ret)
		goto fail;

	// Return MACSUCCESS if h2c aggregation is enabled and enqueued successfully.
	// The H2C shall be sent by mac_h2c_agg_tx.
	ret = h2c_agg_enqueue(adapter, h2cb);
	if (ret == MACSUCCESS)
		return MACSUCCESS;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

static u32 base_offset_to_h2c(struct mac_ax_adapter *adapter, u32 *base_offset,
			      enum rtw_mac_src_cmd_ofld *src, enum rtw_mac_rf_path *rf_path)
{
#define BASE_BITS 0x00FF0000
#define MAC_BASE_OFFSET (0x18600000 & BASE_BITS)
#define BB_BASE_OFFSET (0x18610000 & BASE_BITS)
#define RF_ADIE_BASE_OFFSET (BB_BASE_OFFSET | BIT(23))
#define RF_PATH_B_BASE_OFFSET 0x1000
	u32 ret = MACSUCCESS;

	switch (*base_offset & BASE_BITS) {
	case MAC_BASE_OFFSET:
		*src = RTW_MAC_MAC_CMD_OFLD;
		*rf_path = RTW_MAC_RF_PATH_A;
		break;
	case BB_BASE_OFFSET:
		*src = RTW_MAC_BB_CMD_OFLD;
		*rf_path = RTW_MAC_RF_PATH_A;
		break;
	case (RF_ADIE_BASE_OFFSET | RF_PATH_B_BASE_OFFSET):
		*src = RTW_MAC_RF_CMD_OFLD;
		*rf_path = RTW_MAC_RF_PATH_A;
		break;
	case (RF_ADIE_BASE_OFFSET & ~RF_PATH_B_BASE_OFFSET):
		*src = RTW_MAC_RF_CMD_OFLD;
		*rf_path = RTW_MAC_RF_PATH_B;
		break;
	default:
		PLTFM_MSG_ERR("[ERR] wrong base_offset: %x\n", *base_offset);
		ret = MACHWNOSUP;
		break;
	}
	*base_offset &= ~BASE_BITS;
	return ret;
#undef BASE_BITS
#undef MAC_BASE_OFFSET
#undef BB_BASE_OFFSET
#undef RF_DDIE_BASE_OFFSET
#undef RF_PATH_B_BASE_OFFSET
}

static u32 gen_base_offset(struct mac_ax_adapter *adapter, enum rtw_mac_src_cmd_ofld src,
			   enum rtw_mac_rf_path rf_path, u32 *base_offset)
{
#define BASE_BITS 0x00FF0000
#define MAC_BASE_OFFSET (0x18600000 & BASE_BITS)
#define BB_BASE_OFFSET (0x18610000 & BASE_BITS)
#define RF_ADIE_BASE_OFFSET (BB_BASE_OFFSET | BIT(23))
#define RF_PATH_B_BASE_OFFSET 0x1000
	u32 ret = MACSUCCESS;

	switch (src) {
	case RTW_MAC_MAC_CMD_OFLD:
		*base_offset = MAC_BASE_OFFSET;
		break;
	case RTW_MAC_BB_CMD_OFLD:
		*base_offset = BB_BASE_OFFSET;
		break;
	case RTW_MAC_RF_CMD_OFLD:
		*base_offset = RF_ADIE_BASE_OFFSET;
		switch (rf_path) {
		case RTW_MAC_RF_PATH_A:
			*base_offset &= ~RF_PATH_B_BASE_OFFSET;
			break;
		case RTW_MAC_RF_PATH_B:
			*base_offset |= RF_PATH_B_BASE_OFFSET;
			break;
		default:
			PLTFM_MSG_ERR("[ERR] wrong path: %d\n", rf_path);
			ret = MACHWNOSUP;
			break;
		}
		break;
	default:
		PLTFM_MSG_ERR("[ERR] wrong src: %d\n", src);
		ret = MACHWNOSUP;
		break;
	}
	return ret;
#undef BASE_BITS
#undef MAC_BASE_OFFSET
#undef BB_BASE_OFFSET
#undef RF_DDIE_BASE_OFFSET
#undef RF_PATH_B_BASE_OFFSET
}

static u32 add_cmd_v1(struct mac_ax_adapter *adapter, struct rtw_mac_cmd_v1 *cmd)
{
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	struct fwcmd_cmd_ofld_v1 *write_ptr;
	u32 base_offset;
	u32 ret;
	u16 total_len = CMD_OFLD_V1_SIZE;

	/* initialize */
	if (!ofld_info->buf) {
		ofld_info->buf = (u8 *)PLTFM_MALLOC(CMD_OFLD_MAX_LEN);
		if (!ofld_info->buf)
			return MACBUFALLOC;
		ofld_info->buf_wptr = ofld_info->buf;
		ofld_info->last_wptr = NULL;
		ofld_info->buf_size = CMD_OFLD_MAX_LEN;
		ofld_info->avl_buf_size = CMD_OFLD_MAX_LEN;
		ofld_info->used_size = 0;
		ofld_info->cmd_num = 0;
		ofld_info->accu_delay = 0;
	}

	write_ptr = (struct fwcmd_cmd_ofld_v1 *)ofld_info->buf_wptr;

	ret = gen_base_offset(adapter, cmd->src0, cmd->rf_path0, &base_offset);
	if (ret != MACSUCCESS)
		return ret;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(cmd->type, FWCMD_H2C_CMD_OFLD_V1_TYPE) |
		    (cmd->lc ? FWCMD_H2C_CMD_OFLD_V1_LC : 0) |
		    SET_WORD(((u32)cmd->offset0) + base_offset, FWCMD_H2C_CMD_OFLD_V1_OFFSET0)
	);

	ret = gen_base_offset(adapter, cmd->src1, cmd->rf_path1, &base_offset);
	if (ret != MACSUCCESS)
		return ret;
	write_ptr->dword1 =
	cpu_to_le32(SET_WORD(((u32)cmd->offset1) + base_offset, FWCMD_H2C_CMD_OFLD_V1_OFFSET1) |
		    SET_WORD(ofld_info->cmd_num, FWCMD_H2C_CMD_OFLD_V1_CMD_NUM));
	write_ptr->dword2 =
	cpu_to_le32(SET_WORD(cmd->mask0, FWCMD_H2C_CMD_OFLD_V1_MASK0));
	write_ptr->dword3 =
	cpu_to_le32(SET_WORD(cmd->value, FWCMD_H2C_CMD_OFLD_V1_VALUE));

	ofld_info->last_ver = MAC_AX_CMD_OFLD_V1;
	ofld_info->last_wptr = ofld_info->buf_wptr;
	ofld_info->buf_wptr += total_len;
	ofld_info->avl_buf_size -= total_len;
	ofld_info->used_size += total_len;
	ofld_info->cmd_num++;
	if (cmd->type == RTW_MAC_DELAY_OFLD)
		ofld_info->accu_delay += cmd->value;

	return MACSUCCESS;
}

static u32 add_cmd(struct mac_ax_adapter *adapter, struct rtw_mac_cmd *cmd)
{
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	u16 total_len = CMD_OFLD_SIZE;
	struct fwcmd_cmd_ofld *write_ptr;
	enum rtw_mac_src_cmd_ofld src = cmd->src;

	if (!ofld_info->buf) {
		ofld_info->buf = (u8 *)PLTFM_MALLOC(CMD_OFLD_MAX_LEN);
		if (!ofld_info->buf)
			return MACBUFALLOC;
		ofld_info->buf_wptr = ofld_info->buf;
		ofld_info->last_wptr = NULL;
		ofld_info->buf_size = CMD_OFLD_MAX_LEN;
		ofld_info->avl_buf_size = CMD_OFLD_MAX_LEN;
		ofld_info->used_size = 0;
		ofld_info->cmd_num = 0;
		ofld_info->accu_delay = 0;
	}

	write_ptr = (struct fwcmd_cmd_ofld *)ofld_info->buf_wptr;

	src = cmd->src == RTW_MAC_RF_DDIE_CMD_OFLD ? RTW_MAC_RF_CMD_OFLD : cmd->src;

	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(src, FWCMD_H2C_CMD_OFLD_SRC) |
		    SET_WORD(cmd->type, FWCMD_H2C_CMD_OFLD_TYPE) |
		    (cmd->lc ? FWCMD_H2C_CMD_OFLD_LC : 0) |
		    SET_WORD(cmd->rf_path, FWCMD_H2C_CMD_OFLD_PATH) |
		    SET_WORD(cmd->offset, FWCMD_H2C_CMD_OFLD_OFFSET) |
		    SET_WORD(ofld_info->cmd_num, FWCMD_H2C_CMD_OFLD_CMD_NUM)
	);
	/* TODO: change FWCMD_H2C_CMD_OFLD_OFFSET to FWCMD_H2C_CMD_OFLD_BASE_OFFSET */
	write_ptr->dword1 =
	cpu_to_le32(SET_WORD(cmd->id, FWCMD_H2C_CMD_OFLD_ID) |
		    SET_WORD(cmd->src == RTW_MAC_RF_DDIE_CMD_OFLD ? 1 : 0,
			     FWCMD_H2C_CMD_OFLD_OFFSET)
	);

	write_ptr->dword2 =
	cpu_to_le32(SET_WORD(cmd->value, FWCMD_H2C_CMD_OFLD_VALUE));
	write_ptr->dword3 =
	cpu_to_le32(SET_WORD(cmd->mask, FWCMD_H2C_CMD_OFLD_MASK));

	ofld_info->last_ver = MAC_AX_CMD_OFLD;
	ofld_info->last_wptr = ofld_info->buf_wptr;
	ofld_info->buf_wptr += total_len;
	ofld_info->avl_buf_size -= total_len;
	ofld_info->used_size += total_len;
	ofld_info->cmd_num++;
	if (cmd->type == RTW_MAC_DELAY_OFLD)
		ofld_info->accu_delay += cmd->value;

	return MACSUCCESS;
}

static u32 chk_cmd_ofld_reg(struct mac_ax_adapter *adapter)
{
#define MAC_AX_CMD_OFLD_POLL_CNT 1000
#define MAC_AX_CMD_OFLD_POLL_US 50
	struct mac_ax_c2hreg_poll c2h;
	struct fwcmd_c2hreg *c2h_content;
	u32 ret, result, i, cmd_num;
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	u8 *cmd;

	c2h.polling_id = FWCMD_C2HREG_FUNC_IO_OFLD_RESULT;
	c2h.retry_cnt = MAC_AX_CMD_OFLD_POLL_CNT;
	c2h.retry_wait_us = MAC_AX_CMD_OFLD_POLL_US;
	ret = proc_msg_reg(adapter, NULL, &c2h);
	if (ret) {
		PLTFM_MSG_ERR("%s: fail to wait FW done(%d)\n", __func__, ret);
		return ret;
	}

	c2h_content = &c2h.c2hreg_cont.c2h_content;
	result = GET_FIELD(c2h_content->dword0,
			   FWCMD_C2HREG_IO_OFLD_RESULT_RET);
	if (result) {
		cmd_num = GET_FIELD(c2h_content->dword0,
				    FWCMD_C2HREG_IO_OFLD_RESULT_CMD_NUM);
		cmd = ofld_info->buf + cmd_num * CMD_OFLD_SIZE;
		PLTFM_MSG_ERR("%s: fail to finish IO offload\n", __func__);
		PLTFM_MSG_ERR("fail offset = %x\n", c2h_content->dword1);
		PLTFM_MSG_ERR("exp val = %x\n", c2h_content->dword2);
		PLTFM_MSG_ERR("read val = %x\n", c2h_content->dword3);
		PLTFM_MSG_ERR("fail cmd num = %d\n", cmd_num);
		for (i = 0; i < CMD_OFLD_SIZE; i += 4)
			PLTFM_MSG_ERR("%x\n", *((u32 *)(cmd + i)));

		return MACFIOOFLD;
	}

	return MACSUCCESS;
}

static u32 chk_cmd_ofld_pkt(struct mac_ax_adapter *adapter)
{
	u32 cnt = MAC_AX_CMD_OFLD_POLL_CNT;
	struct mac_ax_state_mach *sm = &adapter->sm;
	struct mac_ax_drv_stats *drv_stats = &adapter->drv_stats;
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;

	while (--cnt) {
		if (sm->cmd_state == MAC_AX_CMD_OFLD_RCVD)
			break;
		if (drv_stats->drv_rm)
			return MACDRVRM;
		PLTFM_DELAY_US(MAC_AX_CMD_OFLD_POLL_US);
	}

	PLTFM_MSG_TRACE("%s: cnt = %d, us = %d\n",
			__func__, cnt, MAC_AX_CMD_OFLD_POLL_US);

	if (!cnt) {
		PLTFM_MSG_ERR("%s: polling timeout\n", __func__);
		return MACPOLLTO;
	}

	if (ofld_info->result) {
		PLTFM_MSG_ERR("%s: ofld FAIL!!!\n", __func__);
		return MACFIOOFLD;
	}

	return MACSUCCESS;
}

static u32 chk_cmd_ofld(struct mac_ax_adapter *adapter, u8 rx_ok)
{
	u32 ret;

	if (rx_ok)
		ret = chk_cmd_ofld_pkt(adapter);
	else
		ret = chk_cmd_ofld_reg(adapter);

	return ret;
}

static u32 h2c_buf_to_cmd_ofld(struct mac_ax_adapter *adapter, struct fwcmd_cmd_ofld *h2c_content,
			       struct rtw_mac_cmd *cmd, u8 *cmd_num)
{
	u16 base_offset;

	if (!cmd || !cmd_num) {
		PLTFM_MSG_ERR("[ERR][CMD_OFLD] cmd = NULL or cmd_num = NULL");
		return MACNPTR;
	}

	cmd->src = (enum rtw_mac_src_cmd_ofld)GET_FIELD(h2c_content->dword0,
							FWCMD_H2C_CMD_OFLD_SRC);
	cmd->type = (enum rtw_mac_cmd_type_ofld)GET_FIELD(h2c_content->dword0,
							  FWCMD_H2C_CMD_OFLD_TYPE);
	cmd->lc = (h2c_content->dword0 & FWCMD_H2C_CMD_OFLD_LC) ? 1 : 0;
	cmd->rf_path = (enum rtw_mac_rf_path)GET_FIELD(h2c_content->dword0,
						       FWCMD_H2C_CMD_OFLD_PATH);
	cmd->offset = GET_FIELD(h2c_content->dword0, FWCMD_H2C_CMD_OFLD_OFFSET);
	*cmd_num = GET_FIELD(h2c_content->dword0, FWCMD_H2C_CMD_OFLD_CMD_NUM);

	cmd->id = GET_FIELD(h2c_content->dword1, FWCMD_H2C_CMD_OFLD_ID);
	base_offset = GET_FIELD(h2c_content->dword1, FWCMD_H2C_CMD_OFLD_BASE_OFFSET);
	if (base_offset) {
		if (cmd->src != RTW_MAC_RF_CMD_OFLD) {
			PLTFM_MSG_ERR("[ERR][CMD_OFLD]BASE_OFFSET = %d, while src = %d",
				      base_offset, cmd->src);
		} else {
			cmd->src = RTW_MAC_RF_DDIE_CMD_OFLD;
		}
	}

	cmd->value = GET_FIELD(h2c_content->dword2, FWCMD_H2C_CMD_OFLD_VALUE);
	cmd->mask = GET_FIELD(h2c_content->dword3, FWCMD_H2C_CMD_OFLD_MASK);

	return MACSUCCESS;
}

static u32 h2c_buf_to_cmd_ofld_v1(struct mac_ax_adapter *adapter,
				  struct fwcmd_cmd_ofld_v1 *h2c_content,
				  struct rtw_mac_cmd_v1 *cmd, u8 *cmd_num)
{
	u32 offset;

	if (!cmd || !cmd_num) {
		PLTFM_MSG_ERR("[ERR][CMD_OFLD_V1] cmd = NULL or cmd_num = NULL");
		return MACNPTR;
	}

	cmd->type = (enum rtw_mac_cmd_type_ofld)GET_FIELD(h2c_content->dword0,
							  FWCMD_H2C_CMD_OFLD_V1_TYPE);
	cmd->lc = (h2c_content->dword0 & FWCMD_H2C_CMD_OFLD_V1_LC) ? 1 : 0;
	offset = GET_FIELD(h2c_content->dword0, FWCMD_H2C_CMD_OFLD_V1_OFFSET0);
	base_offset_to_h2c(adapter, &offset, &cmd->src0, &cmd->rf_path0);
	cmd->offset0 = (u16)offset;
	offset = GET_FIELD(h2c_content->dword1, FWCMD_H2C_CMD_OFLD_V1_OFFSET1);
	base_offset_to_h2c(adapter, &offset, &cmd->src1, &cmd->rf_path1);
	cmd->offset1 = (u16)offset;

	*cmd_num = GET_FIELD(h2c_content->dword1, FWCMD_H2C_CMD_OFLD_V1_CMD_NUM);
	cmd->mask0 = GET_FIELD(h2c_content->dword2, FWCMD_H2C_CMD_OFLD_V1_MASK0);
	cmd->value = GET_FIELD(h2c_content->dword3, FWCMD_H2C_CMD_OFLD_V1_VALUE);

	return MACSUCCESS;
}

static u32 dump_cmd_ofld(struct mac_ax_adapter *adapter)
{
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	struct fwcmd_cmd_ofld *content = (struct fwcmd_cmd_ofld *)ofld_info->buf;
	struct rtw_mac_cmd cmd;
	u8 cmd_num;
	u32 ret;

	while (content <= (struct fwcmd_cmd_ofld *)ofld_info->last_wptr) {
		ret = h2c_buf_to_cmd_ofld(adapter, content, &cmd, &cmd_num);
		if (ret != MACSUCCESS)
			return ret;
		if (content > (struct fwcmd_cmd_ofld *)ofld_info->buf)
			PLTFM_MSG_ERR("[ERR][CMD_OFLD] ========\n");

		PLTFM_MSG_ERR("[ERR][CMD_OFLD] src = %d, type = %d, lc = %hu\n",
			      cmd.src, cmd.type, cmd.lc);
		PLTFM_MSG_ERR("[ERR][CMD_OFLD] rf_path = %d, cmd_num = %hu\n",
			      cmd.rf_path, cmd_num);
		PLTFM_MSG_ERR("[ERR][CMD_OFLD] offset = 0x%hx, id = 0x%hx\n",
			      cmd.offset, cmd.id);
		PLTFM_MSG_ERR("[ERR][CMD_OFLD] value = 0x%x, mask = 0x%x\n",
			      cmd.value, cmd.mask);
		content++;
	}

	return MACSUCCESS;
}

static u32 dump_cmd_ofld_v1(struct mac_ax_adapter *adapter)
{
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	struct fwcmd_cmd_ofld_v1 *content = (struct fwcmd_cmd_ofld_v1 *)ofld_info->buf;
	struct rtw_mac_cmd_v1 cmd;
	u8 cmd_num;
	u32 ret;

	while (content <= (struct fwcmd_cmd_ofld_v1 *)ofld_info->last_wptr) {
		ret = h2c_buf_to_cmd_ofld_v1(adapter, content, &cmd, &cmd_num);
		if (ret != MACSUCCESS)
			return ret;
		if (content > (struct fwcmd_cmd_ofld_v1 *)ofld_info->buf)
			PLTFM_MSG_ERR("[ERR][CMD_OFLD_V1] ========\n");

		PLTFM_MSG_ERR("[ERR][CMD_OFLD_V1] src0=%d, rf_path0=%d, src1=%d, rf_path1=%d\n",
			      cmd.src0, cmd.rf_path0, cmd.src1, cmd.rf_path1);
		PLTFM_MSG_ERR("[ERR][CMD_OFLD_V1] type = %d, lc = %hu, cmd_num = %hu\n",
			      cmd.type, cmd.lc, cmd_num);
		PLTFM_MSG_ERR("[ERR][CMD_OFLD_V1] offset0 = 0x%hx, offset1 = 0x%hx\n",
			      cmd.offset0, cmd.offset1);
		PLTFM_MSG_ERR("[ERR][CMD_OFLD_V1] mask0 = 0x%x, value = 0x%x\n",
			      cmd.mask0, cmd.value);
		content++;
	}

	return MACSUCCESS;
}

static u32 dump_cmd_ofld_h2c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	u32 ret = MACNOTSUP;

	if (ofld_info->last_ver == MAC_AX_CMD_OFLD)
		ret = dump_cmd_ofld(adapter);
	else if (ofld_info->last_ver == MAC_AX_CMD_OFLD_V1)
		ret = dump_cmd_ofld_v1(adapter);

	return ret;
}

static u32 proc_cmd_ofld(struct mac_ax_adapter *adapter, u8 func)
{
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	u8 *buffer;
	struct mac_ax_state_mach *sm = &adapter->sm;

	PLTFM_MSG_TRACE("%s===>\n", __func__);

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buffer = h2cb_put(h2cb, ofld_info->used_size);
	if (!buffer) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMCPY(buffer, ofld_info->buf, ofld_info->used_size);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, func,
			      0, 0);
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
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx\n");
		goto fail;
	}

	if (ofld_info->accu_delay)
		PLTFM_DELAY_US(ofld_info->accu_delay);

	sm->cmd_state = MAC_AX_CMD_OFLD_SENDING;

	ret = chk_cmd_ofld(adapter, adapter->drv_stats.rx_ok);
	if (ret) {
		PLTFM_MSG_ERR("%s: check IO offload fail\n", __func__);
		dump_cmd_ofld_h2c(adapter);
		goto fail;
	}

	h2cb_free(adapter, h2cb);
	PLTFM_FREE(ofld_info->buf, CMD_OFLD_MAX_LEN);
	ofld_info->buf = NULL;

	PLTFM_MSG_TRACE("%s<===\n", __func__);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);
	PLTFM_FREE(ofld_info->buf, CMD_OFLD_MAX_LEN);
	ofld_info->buf = NULL;

	return ret;
}

static u32 cmd_ofld(struct mac_ax_adapter *adapter)
{
	u8 func;
	u8 rx_ok = adapter->drv_stats.rx_ok;

	func = rx_ok ? FWCMD_H2C_FUNC_CMD_OFLD_PKT :
		FWCMD_H2C_FUNC_CMD_OFLD_REG;
	return proc_cmd_ofld(adapter, func);
}

static u32 cmd_ofld_v1(struct mac_ax_adapter *adapter)
{
	u8 func;
	u8 rx_ok = adapter->drv_stats.rx_ok;

	func = rx_ok ? FWCMD_H2C_FUNC_CMD_OFLD_V1_PKT :
		FWCMD_H2C_FUNC_CMD_OFLD_V1_REG;
	return proc_cmd_ofld(adapter, func);
}

u32 mac_cmd_ofld(struct mac_ax_adapter *adapter)
{
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	struct mac_ax_state_mach *sm = &adapter->sm;
	u32 ret = MACSUCCESS;

	if (!ofld_info->buf) {
		PLTFM_MSG_ERR("%s: ofld_info->buf is null\n", __func__);
		return MACNPTR;
	}

	PLTFM_MUTEX_LOCK(&ofld_info->cmd_ofld_lock);
	if (sm->cmd_state != MAC_AX_CMD_OFLD_IDLE) {
		PLTFM_MSG_ERR("%s: IO offload is busy\n", __func__);
		PLTFM_MUTEX_UNLOCK(&ofld_info->cmd_ofld_lock);
		return MACPROCERR;
	}
	sm->cmd_state = MAC_AX_CMD_OFLD_PROC;
	PLTFM_MUTEX_UNLOCK(&ofld_info->cmd_ofld_lock);

	if (ofld_info->last_ver == MAC_AX_CMD_OFLD)
		ret = cmd_ofld(adapter);
	else if (ofld_info->last_ver == MAC_AX_CMD_OFLD_V1)
		ret = cmd_ofld_v1(adapter);

	PLTFM_MUTEX_LOCK(&ofld_info->cmd_ofld_lock);
	sm->cmd_state = MAC_AX_CMD_OFLD_IDLE;
	PLTFM_MUTEX_UNLOCK(&ofld_info->cmd_ofld_lock);

	return ret;
}

static u32 ofld_incompatible_full_cmd(struct mac_ax_adapter *adapter, enum cmd_ofld_ver crnt_ver)
{
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	u32 ret = MACSUCCESS;

	if (!ofld_info->buf)
		return ret;
	/* full or incompatible */
	if (ofld_info->avl_buf_size < CMD_OFLD_V1_SIZE ||
	    (ofld_info->last_ver != crnt_ver && ofld_info->avl_buf_size < WRITE_OFLD_MAX_LEN)) {
		if (!ofld_info->last_wptr) {
			PLTFM_MSG_ERR("%s: wrong pointer\n", __func__);
			return MACNPTR;
		}
		if (ofld_info->last_ver == MAC_AX_CMD_OFLD) {
			*ofld_info->last_wptr = *ofld_info->last_wptr |
				FWCMD_H2C_CMD_OFLD_LC;
			ret = cmd_ofld(adapter);
		} else if (ofld_info->last_ver == MAC_AX_CMD_OFLD_V1) {
			*ofld_info->last_wptr = *ofld_info->last_wptr |
				FWCMD_H2C_CMD_OFLD_V1_LC;
			ret = cmd_ofld_v1(adapter);
		} else {
			PLTFM_MSG_ERR("%s: unrecognized cmd ver\n", __func__);
			return MACNOTSUP;
		}
		if (ret)
			PLTFM_MSG_ERR("%s: send IO offload fail\n", __func__);
	}
	return ret;
}

u32 mac_add_cmd_ofld(struct mac_ax_adapter *adapter, struct rtw_mac_cmd *cmd)
{
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	struct mac_ax_state_mach *sm = &adapter->sm;
	u32 ret = MACSUCCESS;

	if (cmd->type !=  RTW_MAC_DELAY_OFLD &&
	    (cmd->src != RTW_MAC_RF_CMD_OFLD && cmd->src != RTW_MAC_RF_DDIE_CMD_OFLD) &&
	    cmd->offset & (4 - 1))
		return MACBADDR;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MUTEX_LOCK(&ofld_info->cmd_ofld_lock);
	if (sm->cmd_state != MAC_AX_CMD_OFLD_IDLE) {
		PLTFM_MSG_ERR("%s: IO offload is busy\n", __func__);
		PLTFM_MUTEX_UNLOCK(&ofld_info->cmd_ofld_lock);
		return MACPROCERR;
	}
	sm->cmd_state = MAC_AX_CMD_OFLD_PROC;
	PLTFM_MUTEX_UNLOCK(&ofld_info->cmd_ofld_lock);

	ret = ofld_incompatible_full_cmd(adapter, MAC_AX_CMD_OFLD);
	if (ret != MACSUCCESS)
		goto END;

	ret = add_cmd(adapter, cmd);
	if (ret)
		goto END;

	if (!cmd->lc)
		goto END;

	ret = cmd_ofld(adapter);

END:
	PLTFM_MUTEX_LOCK(&ofld_info->cmd_ofld_lock);
	sm->cmd_state = MAC_AX_CMD_OFLD_IDLE;
	PLTFM_MUTEX_UNLOCK(&ofld_info->cmd_ofld_lock);

	return ret;
}

u32 mac_add_cmd_ofld_v1(struct mac_ax_adapter *adapter, struct rtw_mac_cmd_v1 *cmd)
{
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	struct mac_ax_state_mach *sm = &adapter->sm;
	u32 ret = MACSUCCESS;

	if (cmd->type != RTW_MAC_DELAY_OFLD &&
	    ((cmd->src0 != RTW_MAC_RF_CMD_OFLD && cmd->src0 != RTW_MAC_RF_DDIE_CMD_OFLD &&
	     cmd->offset0 & (4 - 1)) ||
	     (cmd->src1 != RTW_MAC_RF_CMD_OFLD && cmd->src1 != RTW_MAC_RF_DDIE_CMD_OFLD &&
	     cmd->offset1 & (4 - 1))))
		return MACBADDR;
	else if (cmd->type == RTW_MAC_MOVE_OFLD &&
		 (cmd->mask0 >> shift_mask(cmd->mask0)) != (cmd->value >> shift_mask(cmd->value))) {
		PLTFM_MSG_ERR("cmd->mask0: %x, cmd->value: %x\n", cmd->mask0, cmd->value);
		return MACBADDR;
	}

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	PLTFM_MUTEX_LOCK(&ofld_info->cmd_ofld_lock);
	if (sm->cmd_state != MAC_AX_CMD_OFLD_IDLE) {
		PLTFM_MSG_ERR("%s: IO offload is busy\n", __func__);
		PLTFM_MUTEX_UNLOCK(&ofld_info->cmd_ofld_lock);
		return MACPROCERR;
	}
	sm->cmd_state = MAC_AX_CMD_OFLD_PROC;
	PLTFM_MUTEX_UNLOCK(&ofld_info->cmd_ofld_lock);

	ret = ofld_incompatible_full_cmd(adapter, MAC_AX_CMD_OFLD_V1);
	if (ret != MACSUCCESS)
		goto END;

	ret = add_cmd_v1(adapter, cmd);
	if (ret)
		goto END;

	if (!cmd->lc)
		goto END;

	ret = cmd_ofld_v1(adapter);

END:
	PLTFM_MUTEX_LOCK(&ofld_info->cmd_ofld_lock);
	sm->cmd_state = MAC_AX_CMD_OFLD_IDLE;
	PLTFM_MUTEX_UNLOCK(&ofld_info->cmd_ofld_lock);

	return ret;
}

u32 write_mac_reg_ofld(struct mac_ax_adapter *adapter,
		       u16 offset, u32 mask, u32 val, u8 lc)
{
	struct rtw_mac_cmd cmd = {RTW_MAC_MAC_CMD_OFLD, RTW_MAC_WRITE_OFLD,
		0, RTW_MAC_RF_PATH_A, 0, 0, 0, 0};

	cmd.offset = offset;
	cmd.mask = mask;
	cmd.value = val;
	cmd.lc = lc;

	return mac_add_cmd_ofld(adapter, &cmd);
}

u32 poll_mac_reg_ofld(struct mac_ax_adapter *adapter,
		      u16 offset, u32 mask, u32 val, u8 lc)
{
	struct rtw_mac_cmd cmd = {RTW_MAC_MAC_CMD_OFLD, RTW_MAC_COMPARE_OFLD,
		0, RTW_MAC_RF_PATH_A, 0, 0, 0, 0};

	cmd.offset = offset;
	cmd.mask = mask;
	cmd.value = val;
	cmd.lc = lc;

	return mac_add_cmd_ofld(adapter, &cmd);
}

u32 delay_ofld(struct mac_ax_adapter *adapter,
	       u32 val, u8 lc)
{
	struct rtw_mac_cmd cmd = {RTW_MAC_MAC_CMD_OFLD, RTW_MAC_DELAY_OFLD,
		0, RTW_MAC_RF_PATH_A, 0, 0, 0, 0};

	cmd.value = val;
	cmd.lc = lc;

	return mac_add_cmd_ofld(adapter, &cmd);
}

u32 write_mac_reg_ofld_v1(struct mac_ax_adapter *adapter,
			  u16 offset, u32 mask, u32 val, u8 lc)
{
	struct rtw_mac_cmd_v1 cmd = {
		RTW_MAC_MAC_CMD_OFLD, /* src0 */
		RTW_MAC_RF_PATH_A, /* rf_path0 */
		RTW_MAC_MAC_CMD_OFLD, /* src1 */
		RTW_MAC_RF_PATH_A, /* rf_path1 */
		RTW_MAC_WRITE_OFLD, /* type */
		0,
		0, 0, 0, 0};

	cmd.lc = lc;
	cmd.offset0 = offset;
	cmd.mask0 = mask;
	cmd.value = val;

	return mac_add_cmd_ofld_v1(adapter, &cmd);
}

u32 poll_mac_reg_ofld_v1(struct mac_ax_adapter *adapter,
			 u16 offset, u32 mask, u32 val, u8 lc)
{
	struct rtw_mac_cmd_v1 cmd = {
		RTW_MAC_MAC_CMD_OFLD, /* src0 */
		RTW_MAC_RF_PATH_A, /* rf_path0 */
		RTW_MAC_MAC_CMD_OFLD, /* src1 */
		RTW_MAC_RF_PATH_A, /* rf_path1 */
		RTW_MAC_COMPARE_OFLD, /* type */
		0,
		0, 0, 0, 0};

	cmd.lc = lc;
	cmd.offset0 = offset;
	cmd.mask0 = mask;
	cmd.value = val;

	return mac_add_cmd_ofld_v1(adapter, &cmd);
}

u32 delay_ofld_v1(struct mac_ax_adapter *adapter,
		  u32 val)
{
	struct rtw_mac_cmd_v1 cmd = {
		RTW_MAC_MAC_CMD_OFLD, /* src0 */
		RTW_MAC_RF_PATH_A, /* rf_path0 */
		RTW_MAC_MAC_CMD_OFLD, /* src1 */
		RTW_MAC_RF_PATH_A, /* rf_path1 */
		RTW_MAC_DELAY_OFLD, /* type */
		0, /*lc*/
		0, 0, 0, 0};

	cmd.value = val;

	return mac_add_cmd_ofld_v1(adapter, &cmd);
}

u32 move_mac_reg_ofld(struct mac_ax_adapter *adapter,
		      u16 offset0, u16 offset1, u32 mask0, u32 mask1, u8 lc)
{
	struct rtw_mac_cmd_v1 cmd = {
		RTW_MAC_MAC_CMD_OFLD, /* src0 */
		RTW_MAC_RF_PATH_A, /* rf_path0 */
		RTW_MAC_MAC_CMD_OFLD, /* src1 */
		RTW_MAC_RF_PATH_A, /* rf_path1 */
		RTW_MAC_MOVE_OFLD, /* type */
		0, /*lc*/
		0, 0, 0, 0};

	cmd.lc = lc;
	cmd.offset0 = offset0;
	cmd.offset1 = offset1;
	cmd.mask0 = mask0;
	cmd.value = mask1;

	return mac_add_cmd_ofld_v1(adapter, &cmd);
}

u32 mac_ccxrpt_parsing(struct mac_ax_adapter *adapter, u8 *buf, struct mac_ax_ccxrpt *info)
{
	u32 val_d0;
	u32 val_d3;
	u32 dword0 = *((u32 *)buf);
	u32 dword3 = *((u32 *)(buf + 12));

	val_d0 = le32_to_cpu(dword0);
	val_d3 = le32_to_cpu(dword3);
	info->tx_state = GET_FIELD(val_d0, TXCCXRPT_TX_STATE);
	info->sw_define = GET_FIELD(val_d0, TXCCXRPT_SW_DEFINE);
	info->macid = GET_FIELD(val_d0, TXCCXRPT_MACID);
	info->pkt_ok_num = GET_FIELD(val_d3, TXCCXRPT_PKT_OK_NUM);
	info->data_txcnt = GET_FIELD(val_d3, TXCCXRPT_DATA_TX_CNT);

	return MACSUCCESS;
}

static inline u8 scanofld_ch_list_len(struct scan_chinfo_list *list)
{
	return list->size;
}

static inline void scanofld_ch_list_init(struct scan_chinfo_list *list)
{
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

static inline u32 scanofld_ch_list_insert_head(struct mac_ax_adapter *adapter,
					       struct scan_chinfo_list *list,
					       struct mac_ax_scanofld_chinfo *chinfo)
{
	struct scanofld_chinfo_node *node;

	node = (struct scanofld_chinfo_node *)PLTFM_MALLOC(sizeof(struct scanofld_chinfo_node));
	if (!node)
		return MACNOBUF;
	node->next = list->head;
	if (list->size == 0)
		list->tail = node;
	list->size++;
	list->head = node;
	node->chinfo = chinfo;
	return MACSUCCESS;
}

static inline u32 scanofld_ch_list_insert_tail(struct mac_ax_adapter *adapter,
					       struct scan_chinfo_list *list,
					       struct mac_ax_scanofld_chinfo *chinfo)
{
	struct scanofld_chinfo_node *node;

	node = (struct scanofld_chinfo_node *)PLTFM_MALLOC(sizeof(struct scanofld_chinfo_node));

	if (!node)
		return MACNOBUF;
	if (list->size == 0)
		list->head = node;
	else
		list->tail->next = node;

	list->tail = node;
	node->chinfo = chinfo;
	node->next = NULL;
	list->size++;
	return MACSUCCESS;
}

static inline void scanofld_ch_node_print(struct mac_ax_adapter *adapter,
					  struct scanofld_chinfo_node *curr_node, u8 i)
{
	struct mac_ax_scanofld_chinfo *chinfo = curr_node->chinfo;

	PLTFM_MSG_TRACE("[scnofld][CH %d] - DWORD 0:%x\n", i, *((u32 *)(chinfo)));
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- period (%d) dwell (%d)\n", i,
			chinfo->period, chinfo->dwell_time);
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- cent (%d) pri (%d)\n", i,
			chinfo->central_ch, chinfo->pri_ch);
	PLTFM_MSG_TRACE("[scnofld][CH %d] - DWORD 1:%x\n", i, *((u32 *)(chinfo) + 1));
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- bw (%d) notiDwell (%d)\n", i,
			chinfo->bw, chinfo->c2h_notify_dwell);
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- noti preT (%d) postT (%d) enter (%d) leave (%d)\n",
			i, chinfo->c2h_notify_preTX, chinfo->c2h_notify_postTX,
			chinfo->c2h_notify_enterCH, chinfo->c2h_notify_leaveCH);
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- numAddtionPkt = %d\n", i, chinfo->num_addition_pkt);
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- tx (%d) pause (%d) band (%d) probeID (%d) dfs (%d)\n",
			i, chinfo->tx_pkt, chinfo->pause_tx_data,
			chinfo->ch_band, chinfo->probe_req_pkt_id, chinfo->dfs_ch);
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- null (%d) randSeq (%d) cfgPwr (%d) txProbe (%d)\n", i,
			chinfo->tx_null, chinfo->rand_seq_num, chinfo->cfg_tx_pwr,
			chinfo->tx_probe_req);
	PLTFM_MSG_TRACE("[scnofld][CH %d] - DWORD 2:%x\n", i, *((u32 *)(chinfo) + 2));
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- (%d)(%d)(%d)(%d)\n", i,
			chinfo->additional_pkt_id[0], chinfo->additional_pkt_id[1],
			chinfo->additional_pkt_id[2], chinfo->additional_pkt_id[3]);
	PLTFM_MSG_TRACE("[scnofld][CH %d] - DWORD 3:%x\n", i, *((u32 *)(chinfo) + 3));
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- (%d)(%d)(%d)(%d)\n", i,
			chinfo->additional_pkt_id[4], chinfo->additional_pkt_id[5],
			chinfo->additional_pkt_id[6], chinfo->additional_pkt_id[7]);
	PLTFM_MSG_TRACE("[scnofld][CH %d] - DWORD 4:%x\n", i, *((u32 *)(chinfo) + 4));
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- pwrIdx (%d), chkpt (%d), additional_acking (%x)\n", i,
			chinfo->tx_pwr_idx, chinfo->chkpt_timer, chinfo->additional_pkt_acking);
	PLTFM_MSG_TRACE("[scnofld][CH %d] - DWORD 5:%x\n", i, *((u32 *)(chinfo) + 5));
	PLTFM_MSG_TRACE("[scnofld][CH %d] -- prob template (%d) ssid(%x)\n", i,
			chinfo->fw_probe0_tplt_id, chinfo->fw_probe0_ssids);
}

static inline void scanofld_ch_list_print(struct mac_ax_adapter *adapter,
					  struct scan_chinfo_list *list, u8 verbose)
{
	struct scanofld_chinfo_node *curr_node = list->head;
	u8 i = 0;

	PLTFM_MSG_TRACE("[scnofld][CH List] len = %d\n", list->size);

	if (!verbose)
		return;

	PLTFM_MSG_TRACE("[scnofld]------------------------------------------\n");
	while (curr_node) {
		scanofld_ch_node_print(adapter, curr_node, i);
		PLTFM_MSG_TRACE("\n");
		curr_node = curr_node->next;
		i++;
	}
	PLTFM_MSG_TRACE("[scnofld]------------------------------------------\n");
}

void mac_scanofld_ch_list_clear(struct mac_ax_adapter *adapter,
				struct scan_chinfo_list *list)
{
	struct scanofld_chinfo_node *curr_node = list->head;
	struct scanofld_chinfo_node *tmp;

	while (curr_node) {
		tmp = curr_node;
		curr_node = curr_node->next;
		PLTFM_FREE(tmp->chinfo, sizeof(struct mac_ax_scanofld_chinfo));
		PLTFM_FREE(tmp, sizeof(struct scanofld_chinfo_node));
		list->size--;
	}
	list->head = NULL;
	list->tail = NULL;
	scanofld_ch_list_print(adapter, list, 0);
}

void mac_scanofld_reset_state(struct mac_ax_adapter *adapter)
{
	struct mac_ax_scanofld_info *scanofld_info;
	u8 band_idx;

	scanofld_info = &adapter->scanofld_info;

	PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
	for (band_idx = 0; band_idx < MAC_AX_BAND_NUM; band_idx++)
		scanofld_info->drv_chlist_busy[band_idx] = 0;
	PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);

	PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
	for (band_idx = 0; band_idx < MAC_AX_BAND_NUM; band_idx++)
		scanofld_info->fw_chlist_busy[band_idx] = 0;
	PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);

	for (band_idx = 0; band_idx < MAC_AX_BAND_NUM; band_idx++)
		scanofld_info->fw_scan_busy[band_idx] = 0;

	PLTFM_MSG_TRACE("[scnofld] Reset all state to 0\n");
}

u32 mac_add_scanofld_ch(struct mac_ax_adapter *adapter, struct mac_ax_scanofld_chinfo *chinfo,
			u8 send_h2c, u8 clear_after_send, u8 band)
{
	struct mac_ax_scanofld_info *scanofld_info;
	struct scan_chinfo_list *list;
	struct scanofld_chinfo_node *curr_node;
	struct mac_ax_scanofld_chinfo *tmp;
	u32 ret;
	u8 list_size;
	u8 *buf8;
	u32 *buf32;
	u32 *chinfo32;
	u8 chinfo_dword;
	struct fwcmd_add_scanofld_ch *pkt;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cbuf;
	#else
	struct h2c_buf *h2cbuf;
	#endif

	if (chinfo->period == 0 || chinfo->pri_ch == 0 || chinfo->central_ch == 0 ||
	    chinfo->ch_band >= CHSW_CHBAND_MAX) {
		PLTFM_MSG_ERR("[scnofld][add] period (%d) pri (%d) cent (%d) band (%d) bad val\n",
			      chinfo->period, chinfo->pri_ch,
			      chinfo->central_ch, chinfo->ch_band);
		return MACFUNCINPUT;
	}

	scanofld_info = &adapter->scanofld_info;
	PLTFM_MSG_TRACE("[scnofld][add][%d] ==> drv_chlist_busy=%d, fw_chlist_busy=%d\n", band,
			scanofld_info->drv_chlist_busy[band], scanofld_info->fw_chlist_busy[band]);
	PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
	if (scanofld_info->drv_chlist_busy[band]) {
		PLTFM_MSG_TRACE("[scnofld][add][%d] Halmac scan list busy, abort adding.\n", band);
		PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);
		return MACPROCBUSY;
	}
	scanofld_info->drv_chlist_busy[band] = 1;
	PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);

	ret = MACSUCCESS;
	if (chinfo->dfs_ch && chinfo->period < MAC_AX_BCN_INTERVAL_DEFAULT) {
		PLTFM_MSG_WARN("[scnofld][add][%d] Period (%d) might be too short for a DFS ch\n",
			       band, chinfo->period);
	}

	if (!scanofld_info->list[band]) {
		list = (struct scan_chinfo_list *)PLTFM_MALLOC(sizeof(struct scan_chinfo_list));
		scanofld_info->list[band] = list;
		scanofld_ch_list_init(adapter->scanofld_info.list[band]);
	}
	list = scanofld_info->list[band];

	tmp = (struct mac_ax_scanofld_chinfo *)PLTFM_MALLOC(sizeof(struct mac_ax_scanofld_chinfo));
	PLTFM_MEMCPY(tmp, chinfo, sizeof(struct mac_ax_scanofld_chinfo));
	ret = scanofld_ch_list_insert_tail(adapter, list, tmp);
	if (ret) {
		PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
		scanofld_info->drv_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);
		return ret;
	}
	scanofld_ch_list_print(adapter, list, send_h2c);

	if (!send_h2c) {
		PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
		scanofld_info->drv_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);
		return ret;
	}

	PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
	if (scanofld_info->fw_chlist_busy[band]) {
		PLTFM_MSG_TRACE("[scnofld][add][%d] FW scan list busy, abort sending.\n", band);
		PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
		scanofld_info->drv_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		if (clear_after_send) {
			PLTFM_MSG_TRACE("[scnofld][add] clear halmac ch list\n");
			mac_scanofld_ch_list_clear(adapter, list);
		}
		return MACPROCBUSY;
	}
	adapter->scanofld_info.fw_chlist_busy[band] = 1;
	PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);

	list_size = scanofld_ch_list_len(list);
	if (list_size == 0) {
		PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
		scanofld_info->drv_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		if (clear_after_send) {
			PLTFM_MSG_TRACE("[scnofld][add] clear halmac ch list\n");
			mac_scanofld_ch_list_clear(adapter, list);
		}
		return MACNOITEM;
	}

	h2cbuf = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cbuf) {
		PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
		scanofld_info->drv_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		if (clear_after_send) {
			PLTFM_MSG_TRACE("[scnofld][add] clear halmac ch list\n");
			mac_scanofld_ch_list_clear(adapter, list);
		}
		return MACNPTR;
	}

	buf8 = h2cb_put(h2cbuf,
			sizeof(struct fwcmd_add_scanofld_ch) +
			list_size * sizeof(struct mac_ax_scanofld_chinfo));
	if (!buf8) {
		PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
		scanofld_info->drv_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		if (clear_after_send) {
			PLTFM_MSG_TRACE("[scnofld][add] clear halmac ch list\n");
			mac_scanofld_ch_list_clear(adapter, list);
		}
		return MACNOBUF;
	}

	pkt = (struct fwcmd_add_scanofld_ch *)buf8;
	pkt->dword0 = cpu_to_le32(SET_WORD(list_size, FWCMD_H2C_ADD_SCANOFLD_CH_NUM_OF_CH) |
				  SET_WORD(sizeof(struct mac_ax_scanofld_chinfo) / 4,
					   FWCMD_H2C_ADD_SCANOFLD_CH_SIZE_OF_CHINFO) |
				  (band ? FWCMD_H2C_ADD_SCANOFLD_CH_BAND : 0));
	buf32 = (u32 *)(buf8 + sizeof(struct fwcmd_add_scanofld_ch));
	curr_node = list->head;
	while (curr_node) {
		chinfo32 = (u32 *)(curr_node->chinfo);
		for (chinfo_dword = 0;
		     chinfo_dword < (sizeof(struct mac_ax_scanofld_chinfo) / 4);
		     chinfo_dword++) {
			*buf32 = cpu_to_le32(*chinfo32);
			buf32++;
			chinfo32++;
		}
		curr_node = curr_node->next;
	}

	ret = h2c_pkt_set_hdr(adapter, h2cbuf, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_ADD_SCANOFLD_CH, 1, 1);
	if (ret) {
		PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
		scanofld_info->drv_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		if (clear_after_send) {
			PLTFM_MSG_TRACE("[scnofld][add] clear halmac ch list\n");
			mac_scanofld_ch_list_clear(adapter, list);
		}
		return ret;
	}
	ret = h2c_pkt_build_txd(adapter, h2cbuf);
	if (ret) {
		PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
		scanofld_info->drv_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_chlist_busy[band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		if (clear_after_send) {
			PLTFM_MSG_TRACE("[scnofld][add] clear halmac ch list\n");
			mac_scanofld_ch_list_clear(adapter, list);
		}
		return ret;
	}
	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cbuf);
	#else
	ret = PLTFM_TX(h2cbuf->data, h2cbuf->len);
	#endif

	if (clear_after_send) {
		PLTFM_MSG_TRACE("[scnofld][add][%d] clear halmac ch list\n", band);
		mac_scanofld_ch_list_clear(adapter, list);
	}

	PLTFM_MUTEX_LOCK(&scanofld_info->drv_chlist_state_lock);
	scanofld_info->drv_chlist_busy[band] = 0;
	PLTFM_MUTEX_UNLOCK(&scanofld_info->drv_chlist_state_lock);

	h2cb_free(adapter, h2cbuf);
	if (ret)
		return ret;
	h2c_end_flow(adapter);
	PLTFM_MSG_TRACE("[scnofld][add][%d] <== drv_chlist_busy=%d, fw_chlist_busy=%d\n", band,
			scanofld_info->drv_chlist_busy[band], scanofld_info->fw_chlist_busy[band]);
	return ret;
}

u32 mac_scanofld(struct mac_ax_adapter *adapter, struct mac_ax_scanofld_param *scanParam)
{
	u8 *buf;
	u8 *byte;
	u32 ret;
	u32 ssids_lv_size;
	u32 total_h2c_size;
	u8 i;
	struct mac_ax_scanofld_info *scanofld_info;
	struct fwcmd_scanofld *pkt;
	enum h2c_buf_class h2cb_type;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cbuf;
	#else
	struct h2c_buf *h2cbuf;
	#endif
	u8 band;

	band = (u8)scanParam->band;
	ssids_lv_size = 0;
	scanofld_info = &adapter->scanofld_info;
	ret = MACSUCCESS;

	if ((scanParam->target_ch_ch_band >= CHSW_CHBAND_MAX ||
	     scanParam->target_central_ch == 0 ||
	     scanParam->target_pri_ch == 0) && scanParam->target_ch_mode) {
		PLTFM_MSG_ERR("[scnofld][cfg][%d] TGT cent (%d) pri (%d) band (%d) bad val\n", band,
			      scanParam->target_central_ch, scanParam->target_pri_ch,
			      scanParam->target_ch_ch_band);
		return MACFUNCINPUT;
	}

	PLTFM_MSG_TRACE("[scnofld][cfg][%d] op=%d (%d), fw_scan_busy=%d, fw_chlist_busy=%d\n", band,
			scanParam->operation, !!(scanParam->operation),
			scanofld_info->fw_scan_busy[band], scanofld_info->fw_chlist_busy[band]);

	PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
	scanofld_info->last_fw_chlist_busy[band] = scanofld_info->fw_chlist_busy[band];
	PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
	scanofld_info->last_fw_scan_busy[band] = scanofld_info->fw_scan_busy[band];

	if (scanParam->operation != MAC_AX_SCAN_OP_GETRPT) {
		if (!!(scanParam->operation) && scanofld_info->fw_scan_busy[band]) {
			PLTFM_MSG_TRACE("[scnofld][cfg][%d] Cant start scanning while scanning\n",
					band);
			return MACPROCBUSY;
		}
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		if (!!(scanParam->operation) && scanofld_info->fw_chlist_busy[band]) {
			PLTFM_MSG_TRACE("[scnofld][cfg][%d] Cant start scan when fw chlist busy\n",
					band);
			PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
			return MACPROCBUSY;
		}

		scanofld_info->fw_chlist_busy[band] = (u8)!!(scanParam->operation);
		scanofld_info->fw_scan_busy[band] = (u8)!!(scanParam->operation);

		PLTFM_MSG_TRACE("[scnofld][cfg][%d] fw_chlist_busy = %d, fw_scan_busy=%d\n", band,
				scanofld_info->fw_chlist_busy[band],
				scanofld_info->fw_scan_busy[band]);
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
	} else if (!scanofld_info->fw_scan_busy[band]) {
		PLTFM_MSG_TRACE("[scnofld][cfg][%d] Cant get rpt when fw is not scanning\n", band);
		return MACPROCERR;
	}

	PLTFM_MSG_TRACE("[scnofld][cfg][%d] macid (%d) port (%d) phy_band (%d)\n", band,
			scanParam->macid, scanParam->port_id, scanParam->band);
	PLTFM_MSG_TRACE("[scnofld][cfg][%d] op (%d) tgt_mode (%d) start_mode (%d) scan_type (%d)\n",
			band, scanParam->operation, scanParam->target_ch_mode,
			scanParam->start_mode, scanParam->scan_type);
	if (scanParam->target_ch_mode) {
		PLTFM_MSG_TRACE("[scnofld][cfg][tgt][%d] bw (%d) pri (%d) cent (%d) band (%d)\n",
				band, scanParam->target_ch_bw, scanParam->target_pri_ch,
				scanParam->target_central_ch, scanParam->target_ch_ch_band);
		PLTFM_MSG_TRACE("[scnofld][cfg][tgt][%d] #pkts (%d) [%d, %d, %d, %d]\n", band,
				scanParam->target_num_pkts,
				scanParam->target_ch_pkt[0], scanParam->target_ch_pkt[1],
				scanParam->target_ch_pkt[2], scanParam->target_ch_pkt[3]);
	}
	PLTFM_MSG_TRACE("[scnofld][cfg][%d] norm_pd (%d) norm_cy (%d) slow_pd (%d)\n", band,
			scanParam->norm_pd, scanParam->norm_cy, scanParam->slow_pd);

	for (i = 0; i < scanParam->numssid; i++)
		ssids_lv_size += (1 + scanParam->ssid[i].len);

	total_h2c_size = sizeof(struct fwcmd_scanofld) + ssids_lv_size;
	if (total_h2c_size <= (H2C_CMD_LEN - FWCMD_HDR_LEN)) {
		h2cb_type = H2CB_CLASS_CMD;
		PLTFM_MSG_TRACE("[scnofld] size %d, using CMD Q\n", total_h2c_size);
	} else if (total_h2c_size <= (H2C_DATA_LEN - FWCMD_HDR_LEN)) {
		h2cb_type = H2CB_CLASS_DATA;
		PLTFM_MSG_TRACE("[scnofld] size %d, using DATA Q\n", total_h2c_size);
	} else if (total_h2c_size <= (H2C_LONG_DATA_LEN - FWCMD_HDR_LEN)) {
		h2cb_type = H2CB_CLASS_LONG_DATA;
		PLTFM_MSG_TRACE("[scnofld] size %d, using LDATA Q\n", total_h2c_size);
	} else {
		PLTFM_MSG_ERR("[scnofld] size %d, exceed LDATA Q size, abort\n", total_h2c_size);
		return MACBUFSZ;
	}

	h2cbuf = h2cb_alloc(adapter, h2cb_type);
	if (!h2cbuf) {
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_chlist_busy[band] = scanofld_info->last_fw_chlist_busy[band];
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_scan_busy[band] = scanofld_info->last_fw_scan_busy[band];

		return MACNPTR;
	}

	buf = h2cb_put(h2cbuf, total_h2c_size);
	if (!buf) {
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_chlist_busy[band] = scanofld_info->last_fw_chlist_busy[band];
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_scan_busy[band] = scanofld_info->last_fw_scan_busy[band];

		h2cb_free(adapter, h2cbuf);
		return MACNOBUF;
	}

	pkt = (struct fwcmd_scanofld *)buf;
	pkt->dword0 = cpu_to_le32(SET_WORD(scanParam->macid, FWCMD_H2C_SCANOFLD_MACID) |
				  SET_WORD(scanParam->norm_cy, FWCMD_H2C_SCANOFLD_NORM_CY) |
				  SET_WORD(scanParam->port_id, FWCMD_H2C_SCANOFLD_PORT_ID) |
				  (scanParam->band ? FWCMD_H2C_SCANOFLD_BAND : 0) |
				  SET_WORD(scanParam->operation, FWCMD_H2C_SCANOFLD_OPERATION) |
				  SET_WORD(scanParam->target_ch_ch_band,
					   FWCMD_H2C_SCANOFLD_TARGET_CH_CH_BAND));
	pkt->dword1 = cpu_to_le32((scanParam->c2h_end ? FWCMD_H2C_SCANOFLD_C2H_NOTIFY_END : 0) |
				  (scanParam->target_ch_mode ?
				   FWCMD_H2C_SCANOFLD_TARGET_CH_MODE : 0) |
				  (scanParam->start_mode ?
				   FWCMD_H2C_SCANOFLD_START_MODE : 0) |
				  SET_WORD(scanParam->scan_type, FWCMD_H2C_SCANOFLD_SCAN_TYPE) |
				  SET_WORD(scanParam->target_ch_bw,
					   FWCMD_H2C_SCANOFLD_TARGET_CH_BW) |
				  SET_WORD(scanParam->target_pri_ch,
					   FWCMD_H2C_SCANOFLD_TARGET_PRI_CH) |
				  SET_WORD(scanParam->target_central_ch,
					   FWCMD_H2C_SCANOFLD_TARGET_CENTRAL_CH) |
				  SET_WORD(scanParam->target_num_pkts,
					   FWCMD_H2C_SCANOFLD_TARGET_NUM_PKTS));
	pkt->dword2 = cpu_to_le32(SET_WORD(scanParam->norm_pd, FWCMD_H2C_SCANOFLD_NORM_PD) |
				  SET_WORD(scanParam->slow_pd, FWCMD_H2C_SCANOFLD_SLOW_PD));
	pkt->dword3 = cpu_to_le32(scanParam->tsf_high);
	pkt->dword4 = cpu_to_le32(scanParam->tsf_low);
	pkt->dword5 = cpu_to_le32(SET_WORD(scanParam->target_ch_pkt[0],
					   FWCMD_H2C_SCANOFLD_TARGET_CH_PKT0) |
				  SET_WORD(scanParam->target_ch_pkt[1],
					   FWCMD_H2C_SCANOFLD_TARGET_CH_PKT1) |
				  SET_WORD(scanParam->target_ch_pkt[2],
					   FWCMD_H2C_SCANOFLD_TARGET_CH_PKT2) |
				  SET_WORD(scanParam->target_ch_pkt[3],
					   FWCMD_H2C_SCANOFLD_TARGET_CH_PKT3));
	pkt->dword6 = cpu_to_le32(SET_WORD(scanParam->numssid, FWCMD_H2C_SCANOFLD_NUMSSID));

	byte = buf + sizeof(struct fwcmd_scanofld);
	for (i = 0; i < scanParam->numssid; i++) {
		*(byte++) = (u8)scanParam->ssid[i].len;
		PLTFM_MEMCPY(byte, scanParam->ssid[i].content, scanParam->ssid[i].len);
		byte += scanParam->ssid[i].len;
	}

	ret = h2c_pkt_set_hdr(adapter, h2cbuf, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_SCANOFLD, 1,
			      scanParam->operation != MAC_AX_SCAN_OP_GETRPT);
	if (ret) {
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_chlist_busy[band] = scanofld_info->last_fw_chlist_busy[band];
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_scan_busy[band] = scanofld_info->last_fw_scan_busy[band];

		h2cb_free(adapter, h2cbuf);
		return ret;
	}
	ret = h2c_pkt_build_txd(adapter, h2cbuf);
	if (ret) {
		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_chlist_busy[band] = scanofld_info->last_fw_chlist_busy[band];
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);
		scanofld_info->fw_scan_busy[band] = scanofld_info->last_fw_scan_busy[band];

		h2cb_free(adapter, h2cbuf);
		return ret;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cbuf);
	#else
	ret = PLTFM_TX(h2cbuf->data, h2cbuf->len);
	#endif
	h2cb_free(adapter, h2cbuf);
	if (ret)
		return ret;
	h2c_end_flow(adapter);
	return ret;
}

u32 mac_scanofld_fw_busy(struct mac_ax_adapter *adapter, u8 band)
{
	if (adapter->scanofld_info.fw_scan_busy[band])
		return MACPROCBUSY;
	else
		return MACSUCCESS;
}

u32 mac_scanofld_chlist_busy(struct mac_ax_adapter *adapter, u8 band)
{
	if (adapter->scanofld_info.drv_chlist_busy[band] ||
	    adapter->scanofld_info.fw_chlist_busy[band])
		return MACPROCBUSY;
	else
		return MACSUCCESS;
}

u32 mac_scanofld_hst_ctrl(struct mac_ax_adapter *adapter, u8 pri_ch, u8 ch_band,
			  enum mac_ax_scanofld_ctrl op, u8 band)
{
	u8 *buf;
	u32 ret;
	struct fwcmd_scanofld_drv_ctrl *pkt;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cbuf;
	#else
	struct h2c_buf *h2cbuf;
	#endif

	ret = MACSUCCESS;

	h2cbuf = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cbuf)
		return MACNPTR;

	buf = h2cb_put(h2cbuf, sizeof(struct fwcmd_scanofld_drv_ctrl));
	if (!buf) {
		h2cb_free(adapter, h2cbuf);
		return MACNOBUF;
	}

	pkt = (struct fwcmd_scanofld_drv_ctrl *)buf;
	pkt->dword0 = cpu_to_le32(SET_WORD(pri_ch, FWCMD_H2C_SCANOFLD_DRV_CTRL_PRI_CH) |
				  SET_WORD(ch_band, FWCMD_H2C_SCANOFLD_DRV_CTRL_CH_BAND) |
				  SET_WORD(op, FWCMD_H2C_SCANOFLD_DRV_CTRL_CMD) |
				  (band ? FWCMD_H2C_SCANOFLD_DRV_CTRL_BAND : 0));

	ret = h2c_pkt_set_hdr(adapter, h2cbuf, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_SCANOFLD_DRV_CTRL, 0, 0);
	if (ret) {
		h2cb_free(adapter, h2cbuf);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cbuf);
	if (ret) {
		h2cb_free(adapter, h2cbuf);
		return ret;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cbuf);
	#else
	ret = PLTFM_TX(h2cbuf->data, h2cbuf->len);
	#endif
	h2cb_free(adapter, h2cbuf);
	if (ret)
		return ret;
	h2c_end_flow(adapter);
	return ret;
}

u32 get_ccxrpt_event(struct mac_ax_adapter *adapter,
		     struct rtw_c2h_info *c2h,
		     enum phl_msg_evt_id *id, u8 *c2h_info)
{
	struct mac_ax_ccxrpt *info;
	u32 val_d0, val_d3;
	u32 dword0 = *((u32 *)c2h->content);
	u32 dword3 = *((u32 *)(c2h->content + 12));

	info = (struct mac_ax_ccxrpt *)c2h_info;
	val_d0 = le32_to_cpu(dword0);
	val_d3 = le32_to_cpu(dword3);
	info->tx_state = GET_FIELD(val_d0, TXCCXRPT_TX_STATE);
	info->sw_define = GET_FIELD(val_d0, TXCCXRPT_SW_DEFINE);
	info->macid = GET_FIELD(val_d0, TXCCXRPT_MACID);
	info->pkt_ok_num = GET_FIELD(val_d3, TXCCXRPT_PKT_OK_NUM);
	info->data_txcnt = GET_FIELD(val_d3, TXCCXRPT_DATA_TX_CNT);

	if (info->tx_state)
		*id = MSG_EVT_CCX_REPORT_TX_FAIL;
	else
		*id = MSG_EVT_CCX_REPORT_TX_OK;

	return MACSUCCESS;
}

u32 pktofld_self_test(struct mac_ax_adapter *adapter)
{
	u32 poll_cnt, ret;
	u16 pkt_len, i;
	u8 pkt_id, state;
	u8 *pkt;

	pkt_len = 16;
	pkt = (u8 *)PLTFM_MALLOC(pkt_len);
	for (i = 0; i < pkt_len; i++)
		pkt[i] = 15 - i;

	ret = mac_add_pkt_ofld(adapter, pkt, pkt_len, &pkt_id);
	if (ret != 0) {
		PLTFM_FREE(pkt, pkt_len);
		pkt = NULL;
		PLTFM_MSG_ERR("Packet ofld self test fail at ADD\n");
		return ret;
	}
	PLTFM_FREE(pkt, pkt_len);
	pkt = NULL;

	for (poll_cnt = 1000; poll_cnt > 0; poll_cnt--) {
		state = adapter->sm.pkt_ofld;
		if (state == MAC_AX_OFLD_H2C_IDLE)
			break;
		PLTFM_DELAY_MS(1);
	}
	if (poll_cnt == 0) {
		PLTFM_MSG_ERR("Packet ofld self test timeout at ADD\n");
		return MACPOLLTO;
	}

	ret = mac_read_pkt_ofld(adapter, pkt_id);
	if (ret != 0) {
		PLTFM_MSG_ERR("Packet ofld self test fail at READ\n");
		return ret;
	}

	for (poll_cnt = 1000; poll_cnt > 0; poll_cnt--) {
		state = adapter->sm.pkt_ofld;
		if (state == MAC_AX_OFLD_H2C_DONE)
			break;
		PLTFM_DELAY_MS(1);
	}
	if (poll_cnt == 0) {
		PLTFM_MSG_ERR("Packet ofld self test timout at READ\n");
		return MACPOLLTO;
	}

	ret = mac_pkt_ofld_packet(adapter, &pkt, &pkt_len, &pkt_id);
	if (ret != 0) {
		PLTFM_MSG_ERR("Packet ofld self test fail at READOUT\n");
		return ret;
	}
	for (i = 0; i < pkt_len; i++) {
		if (pkt[i] != (15 - i)) {
			PLTFM_FREE(pkt, pkt_len);
			pkt = NULL;
			PLTFM_MSG_ERR("Packet ofld self test fail at READOUT (Byte %d)\n", i);
			return MACCMP;
		}
	}
	PLTFM_FREE(pkt, pkt_len);
	pkt = NULL;

	ret = mac_del_pkt_ofld(adapter, pkt_id);
	if (ret != 0) {
		PLTFM_MSG_ERR("Packet ofld self test fail at DEL\n");
		return ret;
	}
	for (poll_cnt = 1000; poll_cnt > 0; poll_cnt--) {
		state = adapter->sm.pkt_ofld;
		if (state == MAC_AX_OFLD_H2C_IDLE)
			break;
		PLTFM_DELAY_MS(1);
	}
	if (poll_cnt == 0) {
		PLTFM_MSG_ERR("Packet ofld self test timout at DEL\n");
		return MACPOLLTO;
	}
	PLTFM_MSG_TRACE("Packet ofld self test pass\n");

	return MACSUCCESS;
}

u32 mac_ch_switch_ofld(struct mac_ax_adapter *adapter, struct mac_ax_ch_switch_parm parm)
{
	u32 ret;
	u8 *buf;
	struct fwcmd_ch_switch *pkt;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cbuf;
	#else
	struct h2c_buf *h2cbuf;
	#endif
	if (adapter->sm.ch_switch != MAC_AX_OFLD_H2C_IDLE &&
	    adapter->sm.ch_switch != MAC_AX_CH_SWITCH_GET_RPT)
		return MACPROCBUSY;
	adapter->sm.ch_switch = MAC_AX_OFLD_H2C_SENDING;
	h2cbuf = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cbuf) {
		adapter->sm.ch_switch = MAC_AX_OFLD_H2C_IDLE;
		return MACNOBUF;
	}
	buf = h2cb_put(h2cbuf, sizeof(struct fwcmd_ch_switch));
	if (!buf) {
		adapter->sm.ch_switch = MAC_AX_OFLD_H2C_IDLE;
		return MACNOBUF;
	}
	pkt = (struct fwcmd_ch_switch *)buf;
	pkt->dword0 = cpu_to_le32(SET_WORD(parm.pri_ch, FWCMD_H2C_CH_SWITCH_PRI_CH) |
				  SET_WORD(parm.central_ch, FWCMD_H2C_CH_SWITCH_CENTRAL_CH) |
				  SET_WORD(parm.bw, FWCMD_H2C_CH_SWITCH_BW) |
				  SET_WORD(parm.ch_band, FWCMD_H2C_CH_SWITCH_CH_BAND) |
				  (parm.band ? FWCMD_H2C_CH_SWITCH_BAND : 0) |
				  (parm.reload_rf ? FWCMD_H2C_CH_SWITCH_RELOAD_RF : 0));
	ret = h2c_pkt_set_hdr(adapter, h2cbuf, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_CH_SWITCH, 1, 0);
	if (ret) {
		adapter->sm.ch_switch = MAC_AX_OFLD_H2C_IDLE;
		return ret;
	}
	ret = h2c_pkt_build_txd(adapter, h2cbuf);
	if (ret) {
		adapter->sm.ch_switch = MAC_AX_OFLD_H2C_IDLE;
		return ret;
	}
	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cbuf);
	#else
	ret = PLTFM_TX(h2cbuf->data, h2cbuf->len);
	#endif
	h2cb_free(adapter, h2cbuf);
	if (ret) {
		adapter->sm.ch_switch = MAC_AX_OFLD_H2C_IDLE;
		return ret;
	}
	h2c_end_flow(adapter);
	return ret;
}

u32 mac_get_ch_switch_rpt(struct mac_ax_adapter *adapter, struct mac_ax_ch_switch_rpt *rpt)
{
	struct mac_ax_state_mach *sm = &adapter->sm;

	if (sm->ch_switch != MAC_AX_CH_SWITCH_GET_RPT)
		return MACPROCERR;
	PLTFM_MEMCPY(rpt, &adapter->ch_switch_rpt, sizeof(struct mac_ax_ch_switch_rpt));
	sm->ch_switch = MAC_AX_OFLD_H2C_IDLE;
	return MACSUCCESS;
}

u32 mac_cfg_bcn_filter(struct mac_ax_adapter *adapter, struct mac_ax_bcn_fltr cfg)
{
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buffer;
	struct fwcmd_cfg_bcnfltr *write_ptr;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buffer = h2cb_put(h2cb, sizeof(struct fwcmd_cfg_bcnfltr));
	if (!buffer) {
		h2cb_free(adapter, h2cb);
		return MACNOBUF;
	}

	write_ptr = (struct fwcmd_cfg_bcnfltr *)buffer;
	write_ptr->dword0 = cpu_to_le32((cfg.mon_rssi ? FWCMD_H2C_CFG_BCNFLTR_MON_RSSI : 0) |
					(cfg.mon_bcn ? FWCMD_H2C_CFG_BCNFLTR_MON_BCN : 0) |
					(cfg.mon_tp ? FWCMD_H2C_CFG_BCNFLTR_MON_TP : 0) |
					SET_WORD(cfg.tp_thld, FWCMD_H2C_CFG_BCNFLTR_TP_THLD) |
					SET_WORD(cfg.bcn_loss_cnt,
						 FWCMD_H2C_CFG_BCNFLTR_BCN_LOSS_CNT) |
					SET_WORD(cfg.rssi_hys, FWCMD_H2C_CFG_BCNFLTR_RSSI_HYS) |
					SET_WORD(cfg.rssi_thld, FWCMD_H2C_CFG_BCNFLTR_RSSI_THLD) |
					SET_WORD(cfg.macid, FWCMD_H2C_CFG_BCNFLTR_MACID));

	ret = h2c_pkt_set_hdr(adapter, h2cb, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_CFG_BCNFLTR,
			      0, 0);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	h2cb_free(adapter, h2cb);
	return MACSUCCESS;
}

u32 mac_bcn_filter_rssi(struct mac_ax_adapter *adapter, u8 macid, u8 size, u8 *rssi)
{
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buffer;
	u32 *buffer_32;
	u32 *rssi_32;
	struct fwcmd_ofld_rssi *write_ptr;
	u8 append_size;
	u8 sh;

	if (size == 0)
		return MACSETVALERR;

	append_size = (size + 3) & (~0x3);

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buffer = h2cb_put(h2cb, sizeof(struct fwcmd_ofld_rssi) + append_size);
	if (!buffer) {
		h2cb_free(adapter, h2cb);
		return MACNOBUF;
	}

	write_ptr = (struct fwcmd_ofld_rssi *)buffer;
	write_ptr->dword0 = cpu_to_le32(SET_WORD(macid, FWCMD_H2C_OFLD_RSSI_MACID) |
					SET_WORD(size, FWCMD_H2C_OFLD_RSSI_NUM_RSSI));

	rssi_32 = (u32 *)rssi;
	buffer_32 = ((u32 *)buffer) + 1;
	for (sh = 0; sh < (append_size >> 2) ; sh++)
		*(buffer_32 + sh) = cpu_to_le32(*(rssi_32 + sh));

	ret = h2c_pkt_set_hdr(adapter, h2cb, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_OFLD_RSSI,
			      0, 0);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	h2cb_free(adapter, h2cb);
	return MACSUCCESS;
}

u32 mac_bcn_filter_tp(struct mac_ax_adapter *adapter, u8 macid, u16 tx, u16 rx)
{
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buffer;
	struct fwcmd_ofld_tp *write_ptr;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	buffer = h2cb_put(h2cb, sizeof(struct fwcmd_ofld_tp));
	if (!buffer) {
		h2cb_free(adapter, h2cb);
		return MACNOBUF;
	}

	write_ptr = (struct fwcmd_ofld_tp *)buffer;
	write_ptr->dword0 = cpu_to_le32(SET_WORD(tx, FWCMD_H2C_OFLD_TP_TXTP) |
					SET_WORD(rx, FWCMD_H2C_OFLD_TP_RXTP) |
					SET_WORD(macid, FWCMD_H2C_OFLD_TP_MACID));

	ret = h2c_pkt_set_hdr(adapter, h2cb, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_OFLD_TP,
			      0, 0);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		h2cb_free(adapter, h2cb);
		return ret;
	}

	h2cb_free(adapter, h2cb);
	return MACSUCCESS;
}

u32 mac_host_efuse_rec(struct mac_ax_adapter *adapter, u32 host_id, u32 efuse_val)
{
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_host_efuse_rec *hdr;
	u32 ret = MACSUCCESS;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_host_efuse_rec *)h2cb_put(h2cb,
		sizeof(struct fwcmd_host_efuse_rec));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	hdr->dword0 =
		cpu_to_le32(SET_WORD(host_id, FWCMD_H2C_HOST_EFUSE_REC_HOST_ID));
	hdr->dword1 =
		cpu_to_le32(SET_WORD(efuse_val, FWCMD_H2C_HOST_EFUSE_REC_EFUSE_VAL));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_HOST_EFUSE_REC,
			      0,
			      0);
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

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_cfg_sensing_csi(struct mac_ax_adapter *adapter, struct rtw_hal_mac_sensing_csi_param *para)
{
	struct sensing_csi_info *csi_info = &adapter->csi_info;
	struct mac_ax_state_mach *sm = &adapter->sm;
	u8 *buf;
	u32 ret;
	u32 *pkt_id_dword;
	u8 id_size = 0;
	u8 *id_array = NULL;
	u32 i;
	struct fwcmd_wifi_sensing_csi *pkt;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cbuf;
#else
	struct h2c_buf *h2cbuf;
#endif

	PLTFM_MSG_TRACE("[csi] macid=%d, en=%d, period=%d\n",
			para->macid, para->en, para->period);
	PLTFM_MSG_TRACE("[csi] retry_cnt=%d, rate=%d, pkt_num=%d\n",
			para->retry_cnt, para->rate, para->pkt_num);
	for (i = 0; i < para->pkt_num; i++)
		PLTFM_MSG_TRACE("[csi] pkt_id[%d]=%d\n", i, para->pkt_id[i]);

	PLTFM_MUTEX_LOCK(&csi_info->state_lock);
	if (sm->sensing_csi_st != MAC_AX_SENSING_CSI_IDLE) {
		PLTFM_MSG_ERR("[WARN][csi][Cfg] H2C is sent before!\n");
		PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);
	}
	sm->sensing_csi_st = MAC_AX_SENSING_CSI_SENDING;
	PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);

	if (csi_info->start_cmd_send || csi_info->stop_cmd_send)
		PLTFM_MSG_ERR("[ERR][csi][Cfg] state machine error!\n");

	if (para->pkt_num) {
		id_size = (para->pkt_num & (~0x3)) + 4;

		id_array = (u8 *)PLTFM_MALLOC(id_size);
		if (!id_array) {
			PLTFM_MSG_ERR("[ERR][csi] id_array malloc fail!\n");
			PLTFM_MUTEX_LOCK(&csi_info->state_lock);
			sm->sensing_csi_st = MAC_AX_SENSING_CSI_IDLE;
			PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);
			return MACNPTR;
		}
		PLTFM_MEMSET(id_array, 0, id_size);
		PLTFM_MEMCPY(id_array, para->pkt_id, id_size);
	}

	h2cbuf = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cbuf) {
		PLTFM_MSG_ERR("[ERR][csi] h2cbuf malloc fail!\n");
		PLTFM_FREE(id_array, id_size);
		PLTFM_MUTEX_LOCK(&csi_info->state_lock);
		sm->sensing_csi_st = MAC_AX_SENSING_CSI_IDLE;
		PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);
		return MACNPTR;
	}

	buf = h2cb_put(h2cbuf, sizeof(struct fwcmd_wifi_sensing_csi) + id_size);
	if (!buf) {
		h2cb_free(adapter, h2cbuf);
		PLTFM_FREE(id_array, id_size);
		PLTFM_MUTEX_LOCK(&csi_info->state_lock);
		sm->sensing_csi_st = MAC_AX_SENSING_CSI_IDLE;
		PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);
		return MACNOBUF;
	}

	pkt = (struct fwcmd_wifi_sensing_csi *)buf;
	pkt->dword0 = cpu_to_le32(SET_WORD(para->macid, FWCMD_H2C_WIFI_SENSING_CSI_MACID) |
				  (para->en ? FWCMD_H2C_WIFI_SENSING_CSI_EN : 0) |
				  SET_WORD(para->period, FWCMD_H2C_WIFI_SENSING_CSI_PERIOD));
	pkt->dword1 = cpu_to_le32(SET_WORD(para->retry_cnt, FWCMD_H2C_WIFI_SENSING_CSI_RETRY_CNT) |
				  SET_WORD(para->rate, FWCMD_H2C_WIFI_SENSING_CSI_RATE) |
				  SET_WORD(para->pkt_num, FWCMD_H2C_WIFI_SENSING_CSI_PKT_NUM));

	pkt_id_dword = (u32 *)(buf + sizeof(struct fwcmd_wifi_sensing_csi));
	for (i = 0; i < id_size; i += 4) {
		*pkt_id_dword = cpu_to_le32(*(u32 *)(id_array + i));
		pkt_id_dword++;
	}

	ret = h2c_pkt_set_hdr(adapter, h2cbuf,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_WIFI_SENSING_CSI,
			      0,
			      1);
	if (ret) {
		h2cb_free(adapter, h2cbuf);
		PLTFM_FREE(id_array, id_size);
		PLTFM_MUTEX_LOCK(&csi_info->state_lock);
		sm->sensing_csi_st = MAC_AX_SENSING_CSI_IDLE;
		PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);
		return ret;
	}
	ret = h2c_pkt_build_txd(adapter, h2cbuf);
	if (ret) {
		h2cb_free(adapter, h2cbuf);
		PLTFM_FREE(id_array, id_size);
		PLTFM_MUTEX_LOCK(&csi_info->state_lock);
		sm->sensing_csi_st = MAC_AX_SENSING_CSI_IDLE;
		PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);
		return ret;
	}

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cbuf);
#else
	ret = PLTFM_TX(h2cbuf->data, h2cbuf->len);
#endif
	h2cb_free(adapter, h2cbuf);
	PLTFM_FREE(id_array, id_size);
	if (ret)
		return ret;
	h2c_end_flow(adapter);

	if (para->en)
		csi_info->start_cmd_send = 1;
	else
		csi_info->stop_cmd_send = 1;

	return ret;
}

u32 mac_chk_sensing_csi_done(struct mac_ax_adapter *adapter, u8 chk_state)
{
	struct sensing_csi_info *csi_info = &adapter->csi_info;
	struct mac_ax_state_mach *sm = &adapter->sm;

	PLTFM_MUTEX_LOCK(&csi_info->state_lock);
	if (sm->sensing_csi_st != MAC_AX_SENSING_CSI_IDLE) {
		PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);
		return MACPROCBUSY;
	}
	PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);

	if (chk_state != csi_info->func_en)
		return MACCMP;
	else
		return MACSUCCESS;
}

u32 get_sensing_csi_event(struct mac_ax_adapter *adapter,
			  struct rtw_c2h_info *c2h,
			  enum phl_msg_evt_id *id, u8 *c2h_info)
{
	struct rtw_hal_mac_sensing_csi_tx_result *info;
	u32 val_d0;
	u32 dword0 = *((u32 *)c2h->content);
	u32 val_tmp;
	u8 i;

	info = (struct rtw_hal_mac_sensing_csi_tx_result *)c2h_info;
	val_d0 = le32_to_cpu(dword0);
	info->macid = GET_FIELD(val_d0, FWCMD_C2H_WIFI_SENSING_CSI_TX_RESULT_MACID);
	info->num = GET_FIELD(val_d0, FWCMD_C2H_WIFI_SENSING_CSI_TX_RESULT_NUM);
	info->seq_num = GET_FIELD(val_d0, FWCMD_C2H_WIFI_SENSING_CSI_TX_RESULT_SEQ_NUM);

	if (info->num > RTW_PHL_SOFTAP_MAX_CLIENT_NUM)
		return MACBUFSZ;

	for (i = 0; i < info->num; i++) {
		val_tmp = le32_to_cpu(*((u32 *)(c2h->content + 4 + i)));
		PLTFM_MEMCPY(&info->tx_rpt[i], &val_tmp, 4);
	}

	*id = MSG_EVT_CSI_TX_RESULT;

	return MACSUCCESS;
}

u32 mac_fwcpumgenq_test(struct mac_ax_adapter *adapter, u8 macid, u16 len, u8 *pkt)
{
	u8 *buf;
	u32 ret;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_cpumgenq_test *write_ptr;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACNOFW;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_cpumgenq_test));
	if (!buf) {
		ret = MACNOBUF;
		goto end;
	}

	write_ptr = (struct fwcmd_cpumgenq_test *)buf;
	write_ptr->dword0 =
	cpu_to_le32(SET_WORD(len, FWCMD_H2C_CPUMGENQ_TEST_PKTLEN) |
		    SET_WORD(macid, FWCMD_H2C_CPUMGENQ_TEST_MACID));
	buf = h2cb_put(h2cb, len);
	if (!buf) {
		ret = MACNOBUF;
		goto end;
	}

	PLTFM_MEMCPY(buf, pkt, len);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C, FWCMD_H2C_CAT_TEST,
			      FWCMD_H2C_CL_FW_AUTO_TEST, FWCMD_H2C_FUNC_CPUMGENQ_TEST,
			      0, 0);
	if (ret)
		goto end;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto end;

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("[ERR]platform tx: %d\n", ret);
		goto end;
	}

	h2cb_free(adapter, h2cb);

	ret = MACSUCCESS;
end:
	h2cb_free(adapter, h2cb);
	return ret;
}

u32 mac_cfg_bcn_early_rpt(struct mac_ax_adapter *adapter, u8 band, u8 port, u8 en)
{
	u32 ret;
	u8 *buf;
	struct fwcmd_bcnerlyrpt *pkt;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cbuf;
	#else
	struct h2c_buf *h2cbuf;
	#endif
	h2cbuf = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cbuf)
		return MACNOBUF;
	buf = h2cb_put(h2cbuf, sizeof(struct fwcmd_bcnerlyrpt));
	if (!buf)
		return MACNOBUF;
	pkt = (struct fwcmd_bcnerlyrpt *)buf;
	pkt->dword0 = cpu_to_le32(SET_WORD(port, FWCMD_H2C_BCNERLYRPT_PORT) |
				  (band ? FWCMD_H2C_BCNERLYRPT_BAND : 0) |
				  (en ? FWCMD_H2C_BCNERLYRPT_CONTROL : 0));
	ret = h2c_pkt_set_hdr(adapter, h2cbuf, FWCMD_TYPE_H2C, FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD, FWCMD_H2C_FUNC_BCNERLYRPT, 0, 0);
	if (ret)
		return ret;
	ret = h2c_pkt_build_txd(adapter, h2cbuf);
	if (ret)
		return ret;
	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cbuf);
	#else
	ret = PLTFM_TX(h2cbuf->data, h2cbuf->len);
	#endif
	h2cb_free(adapter, h2cbuf);
	if (ret)
		return ret;
	h2c_end_flow(adapter);
	return ret;
}

u32 get_bcn_erly_event(struct mac_ax_adapter *adapter, struct rtw_c2h_info *c2h,
		       enum phl_msg_evt_id *id, u8 *c2h_info)
{
	struct mac_ax_bcn_erly_rpt *info;
	u32 val_d0;
	u32 dword0 = *((u32 *)c2h->content);

	info = (struct mac_ax_bcn_erly_rpt *)c2h_info;
	val_d0 = le32_to_cpu(dword0);
	info->band = (val_d0 & FWCMD_C2H_BCNERLYNTFY_BAND) ? 1 : 0;
	info->port = (u8)GET_FIELD(val_d0, FWCMD_C2H_BCNERLYNTFY_PORT);
	*id = MSG_EVT_BCN_EARLY_REPORT;
	PLTFM_MSG_TRACE("[BCNErlyRpt] band %d, port %d\n", info->band, info->port);

	return MACSUCCESS;
}
