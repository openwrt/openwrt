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
#include "h2c_agg.h"

static u32 mac_h2c_agg_tx_single_normal_h2c(struct mac_ax_adapter *adapter, u8 *h2cb)
{
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *tx_h2cb = (struct rtw_h2c_pkt *)h2cb;
#else
	struct h2c_buf *tx_h2cb = (struct h2c_buf *)h2cb;
#endif
	u32 ret = MACSUCCESS;

	ret = h2c_pkt_build_txd(adapter, tx_h2cb);
	if (ret)
		goto fail;

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(tx_h2cb);
#else
	ret = PLTFM_TX(tx_h2cb->data, tx_h2cb->len);
	if (ret)
		goto fail;
	h2cb_free(adapter, tx_h2cb);
#endif

fail:
	return ret;
}

static u32 mac_h2c_agg_tx_single_agg_h2c(struct mac_ax_adapter *adapter, u8 *agg_h2cb)
{
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *tx_h2cb = (struct rtw_h2c_pkt *)agg_h2cb;
#else
	struct h2c_buf *tx_h2cb = (struct h2c_buf *)agg_h2cb;
#endif
	u32 ret = MACSUCCESS;

	ret = h2c_pkt_set_hdr(adapter,
			      tx_h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_H2C_AGG,
			      0,
			      0);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, tx_h2cb);
	if (ret)
		goto fail;

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(tx_h2cb);
#else
	ret = PLTFM_TX(tx_h2cb->data, tx_h2cb->len);
	if (ret)
		goto fail;
	h2cb_free(adapter, tx_h2cb);
#endif

fail:
	return ret;
}

void mac_h2c_agg_enable(struct mac_ax_adapter *adapter, u8 enable)
{
	PLTFM_MUTEX_LOCK(&adapter->h2c_agg_info.h2c_agg_lock);
	adapter->h2c_agg_info.h2c_agg_en = enable;
	PLTFM_MUTEX_UNLOCK(&adapter->h2c_agg_info.h2c_agg_lock);
}

u32 mac_h2c_agg_tx(struct mac_ax_adapter *adapter)
{
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *agg_h2cb = NULL;
	struct rtw_h2c_pkt *cur_h2cb = NULL;
#else
	struct h2c_buf *agg_h2cb = NULL;
	struct h2c_buf *cur_h2cb = NULL;
#endif
	struct mac_ax_h2c_agg_node *cur_agg_node = NULL;
	struct mac_ax_h2c_agg_node *tmp_agg_node = NULL;
	u32 agg_len = 0;
	u32 cur_h2cb_len = 0;
	u32 cur_sub_h2c_len = 0;
	u32 cur_sub_h2c_len_swap = 0;
	u32 next_h2cb_len = 0;
	u32 ret = MACSUCCESS;
	u8 *cur_h2cb_data = NULL;
	u8 *buf = NULL;

	PLTFM_MUTEX_LOCK(&adapter->h2c_agg_info.h2c_agg_lock);

	if (!adapter->h2c_agg_info.h2c_agg_queue_head)
		goto fail;

	cur_agg_node = adapter->h2c_agg_info.h2c_agg_queue_head;
	agg_h2cb = NULL;
	agg_len = 0;

	while (cur_agg_node) {
#if MAC_AX_PHL_H2C
		cur_h2cb = (struct rtw_h2c_pkt *)cur_agg_node->h2c_pkt;
		cur_h2cb_len = cur_h2cb->data_len;
		cur_h2cb_data = cur_h2cb->vir_data;
		next_h2cb_len = (!cur_agg_node->next ? 0 :
						((struct rtw_h2c_pkt *)
						(cur_agg_node->next->h2c_pkt))->data_len);
#else
		cur_h2cb = (struct h2c_buf *)cur_agg_node->h2c_pkt;
		cur_h2cb_len = cur_h2cb->len;
		cur_h2cb_data = cur_h2cb->data;
		next_h2cb_len = (!cur_agg_node->next ? 0 :
						((struct h2c_buf *)
						(cur_agg_node->next->h2c_pkt))->len);
#endif

		if (!agg_h2cb) {
			if (!H2C_PKT_AGGREGATABLE(cur_h2cb_len) ||
			    !cur_agg_node->next ||
			    (cur_agg_node->next &&
			    !H2C_PKT_AGGREGATABLE(next_h2cb_len))) {
				ret = mac_h2c_agg_tx_single_normal_h2c(adapter, (u8 *)cur_h2cb);
				if (ret) {
					PLTFM_MSG_ERR("tx normal h2c pkt fail\n");
					goto fail;
				}

				tmp_agg_node = cur_agg_node;
				cur_agg_node = cur_agg_node->next;
				adapter->h2c_agg_info.h2c_agg_queue_head = cur_agg_node;
				PLTFM_FREE(tmp_agg_node, sizeof(struct mac_ax_h2c_agg_node));
				continue;
			}

			agg_h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
			if (!agg_h2cb) {
				PLTFM_MSG_ERR("allocate agg_h2c fail\n");
				ret = MACNOBUF;
				goto fail;
			}
			agg_len = WD_BODY_LEN + FWCMD_HDR_LEN;
		}

		cur_sub_h2c_len = ALIGN_4_BYTE(cur_h2cb_len);

		buf = h2cb_put(agg_h2cb, cur_sub_h2c_len + H2C_AGG_SUB_HDR_LEN);
		if (!buf) {
			PLTFM_MSG_ERR("creat sub_h2c_buf in agg_h2cb get fail\n");
			ret = MACNOITEM;
			goto fail;
		}

		cur_sub_h2c_len_swap = cpu_to_le32(cur_sub_h2c_len);
		PLTFM_MEMCPY(buf, &cur_sub_h2c_len_swap, H2C_AGG_SUB_HDR_LEN);
		PLTFM_MEMCPY(buf + H2C_AGG_SUB_HDR_LEN, cur_h2cb_data, cur_h2cb_len);
		agg_len = agg_len + cur_sub_h2c_len + H2C_AGG_SUB_HDR_LEN;

		tmp_agg_node = cur_agg_node;
		cur_agg_node = cur_agg_node->next;
		adapter->h2c_agg_info.h2c_agg_queue_head = cur_agg_node;
		PLTFM_FREE(tmp_agg_node, sizeof(struct mac_ax_h2c_agg_node));

#if MAC_AX_PHL_H2C
		PLTFM_RECYCLE_H2C(cur_h2cb);
#else
		h2cb_free(adapter, cur_h2cb);
#endif

		if (!cur_agg_node ||
		    ((agg_len + ALIGN_4_BYTE(next_h2cb_len) +
		    H2C_AGG_SUB_HDR_LEN) >= H2C_LONG_DATA_LEN)) {
			ret = mac_h2c_agg_tx_single_agg_h2c(adapter, (u8 *)agg_h2cb);
			if (ret) {
				PLTFM_MSG_ERR("tx normal agg_h2c pkt fail\n");
				goto fail;
			}
			agg_h2cb = NULL;
			agg_len = 0;
		}
	}

fail:
	if (ret) {
		PLTFM_MSG_ERR("h2c agg error handle\n");

		mac_h2c_agg_flush(adapter);

		if (agg_h2cb) {
#if MAC_AX_PHL_H2C
			PLTFM_RECYCLE_H2C((struct rtw_h2c_pkt *)agg_h2cb);
#else
			h2cb_free(adapter, agg_h2cb);
#endif
		}
	} else {
		adapter->h2c_agg_info.h2c_agg_queue_head = NULL;
		adapter->h2c_agg_info.h2c_agg_queue_last = NULL;
		adapter->h2c_agg_info.h2c_agg_pkt_num = 0;
	}
	PLTFM_MUTEX_UNLOCK(&adapter->h2c_agg_info.h2c_agg_lock);
	return ret;
}

void mac_h2c_agg_flush(struct mac_ax_adapter *adapter)
{
	struct mac_ax_h2c_agg_node *cur_agg_node = NULL;
	struct mac_ax_h2c_agg_node *tmp_agg_node = NULL;

	PLTFM_MUTEX_LOCK(&adapter->h2c_agg_info.h2c_agg_lock);
	cur_agg_node = adapter->h2c_agg_info.h2c_agg_queue_head;

	while (cur_agg_node) {
#if MAC_AX_PHL_H2C
		PLTFM_RECYCLE_H2C((struct rtw_h2c_pkt *)cur_agg_node->h2c_pkt);
#else
		h2cb_free(adapter, (struct h2c_buf *)cur_agg_node->h2c_pkt);
#endif
		tmp_agg_node = cur_agg_node;
		cur_agg_node = cur_agg_node->next;
		PLTFM_FREE(tmp_agg_node, sizeof(struct mac_ax_h2c_agg_node));
	}

	adapter->h2c_agg_info.h2c_agg_queue_head = NULL;
	adapter->h2c_agg_info.h2c_agg_queue_last = NULL;
	adapter->h2c_agg_info.h2c_agg_pkt_num = 0;
	PLTFM_MUTEX_UNLOCK(&adapter->h2c_agg_info.h2c_agg_lock);
}
