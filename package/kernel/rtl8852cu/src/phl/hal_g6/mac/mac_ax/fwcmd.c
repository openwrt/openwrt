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

#include "fwcmd.h"
#include "mcc.h"
#include "mac_priv.h"
#include "twt.h"

#if MAC_AX_FEATURE_HV
#include "../hv_ax/dbgpkg_hv.h"
#endif

/* 8852A/8852B: the format of H2C/DLFW descriptor: WD Body
 * 8852C: the format of H2C/DLFW descriptor: RX Descriptor
 * WD body max len: 24 bytes
 * RX descriptor max len: 32 bytes
 * We use the max RX descriptor size as the header size
 * WD_BODY_LEN_V1 = RX descriptor max len = 32 bytes
 */

#define H2CB_CMD_HDR_SIZE	(FWCMD_HDR_LEN + WD_BODY_LEN_V1)
#define H2CB_CMD_SIZE		(H2C_CMD_LEN - FWCMD_HDR_LEN)
#define H2CB_CMD_QLEN		8

#define H2CB_DATA_HDR_SIZE	(FWCMD_HDR_LEN + WD_BODY_LEN_V1)
#define H2CB_DATA_SIZE		(H2C_DATA_LEN - FWCMD_HDR_LEN)
#define H2CB_DATA_QLEN		4

#define H2CB_LONG_DATA_HDR_SIZE	(FWCMD_HDR_LEN + WD_BODY_LEN)
#define H2CB_LONG_DATA_SIZE	(H2C_LONG_DATA_LEN - FWCMD_HDR_LEN)
#define H2CB_LONG_DATA_QLEN	1

#define FWCMD_WQ_MAX_JOB_NUM	5

#define FWCMD_LMT		12

#define MAC_AX_H2C_LMT_EN	0

#define FWCMD_H2CREG_BYTE0_SH 0
#define FWCMD_H2CREG_BYTE0_MSK 0xFF
#define FWCMD_H2CREG_BYTE1_SH 8
#define FWCMD_H2CREG_BYTE1_MSK 0xFF
#define FWCMD_H2CREG_BYTE2_SH 16
#define FWCMD_H2CREG_BYTE2_MSK 0xFF
#define FWCMD_H2CREG_BYTE3_SH 24
#define FWCMD_H2CREG_BYTE3_MSK 0xFF

#define BCN_GRPIE_OFST_EN BIT(7)

#define SCANOFLD_RSP_EVT_ID 1
#define SCANOFLD_RSP_EVT_PARSE 1
#define SCANOFLD_ACK_BAND_SHIFT 6
#define SCANOFLD_ACK_RETURN_MASK 0x3F

static struct h2c_buf_head h2cb_head[H2CB_CLASS_MAX];
static struct fwcmd_wkb_head fwcmd_wq_head;

struct fwcmd_outsrc_info {
#define MAX_OUTSRC_LEN 60 //need to extend if needed
	u32 dword0[MAX_OUTSRC_LEN];
};

struct c2h_event_id_proc {
	u8 cat;
	u8 cls;
	u8 func;
	u32 (*hdl)(struct mac_ax_adapter *adapter, struct rtw_c2h_info *c2h,
		   enum phl_msg_evt_id *id, u8 *c2h_info);
};

static inline u32 h2cb_queue_len(struct h2c_buf_head *list)
{
	return list->qlen;
}

static inline void __h2cb_queue_head_init(struct h2c_buf_head *list)
{
	list->prev = (struct h2c_buf *)list;
	list->next = (struct h2c_buf *)list;
	list->qlen = 0;
	list->suspend = 0;
}

static inline void h2cb_queue_head_init(struct mac_ax_adapter *adapter,
					struct h2c_buf_head *list)
{
	PLTFM_MUTEX_INIT(&list->lock);
	__h2cb_queue_head_init(list);
}

static inline void __h2cb_insert(struct h2c_buf *new_h2cb, struct h2c_buf *prev,
				 struct h2c_buf *next,
				 struct h2c_buf_head *list)
{
	new_h2cb->next = next;
	new_h2cb->prev = prev;
	next->prev  = new_h2cb;
	prev->next = new_h2cb;
	list->qlen++;
}

static inline void __h2cb_queue_before(struct h2c_buf_head *list,
				       struct h2c_buf *next,
				       struct h2c_buf *new_h2cb)
{
	__h2cb_insert(new_h2cb, next->prev, next, list);
}

static inline void __h2cb_queue_tail(struct h2c_buf_head *list,
				     struct h2c_buf *new_h2cb)
{
	__h2cb_queue_before(list, (struct h2c_buf *)list, new_h2cb);
}

static inline void __h2cb_unlink(struct h2c_buf *h2cb,
				 struct h2c_buf_head *list)
{
	struct h2c_buf *next, *prev;

	list->qlen--;
	next = h2cb->next;
	prev = h2cb->prev;
	h2cb->prev = NULL;
	h2cb->next = NULL;
	next->prev = prev;
	prev->next = next;
}

static inline struct h2c_buf *h2cb_peek(struct h2c_buf_head *list)
{
	struct h2c_buf *h2cb = list->next;

	if (h2cb == (struct h2c_buf *)list)
		h2cb = NULL;
	return h2cb;
}

#if MAC_AX_PHL_H2C
static inline u8 *h2cb_tail_pointer(const struct rtw_h2c_pkt *h2cb)
{
	return h2cb->vir_tail;
}
#else
static inline u8 *h2cb_tail_pointer(const struct h2c_buf *h2cb)
{
	return h2cb->tail;
}
#endif

static inline struct h2c_buf *h2cb_dequeue(struct h2c_buf_head *list)
{
	struct h2c_buf *h2cb = h2cb_peek(list);

	if (h2cb)
		__h2cb_unlink(h2cb, list);
	return h2cb;
}

static u8 *__h2cb_alloc_buf_pool(struct mac_ax_adapter *adapter,
				 struct h2c_buf_head *list, u32 size, int num)
{
	u32 block_size = (size * num);
	u8 *ptr;

	ptr = (u8 *)PLTFM_MALLOC(block_size);
	list->pool = ptr;
	list->size = block_size;

	return ptr;
}

static struct h2c_buf *__h2cb_alloc(struct mac_ax_adapter *adapter,
				    enum h2c_buf_class buf_class,
				    u32 hdr_size, u8 *buf_ptr, int buf_size)
{
	struct h2c_buf *h2cb;

	//_ASSERT_(!buf_ptr);

	h2cb = (struct h2c_buf *)PLTFM_MALLOC(sizeof(struct h2c_buf));
	if (!h2cb)
		return NULL;
	PLTFM_MEMSET(h2cb, 0, sizeof(struct h2c_buf));

	h2cb->_class_ = buf_class;
	h2cb->id = 0;
	h2cb->master = 0;
	h2cb->len = 0;
	h2cb->head = buf_ptr;
	h2cb->end = h2cb->head + buf_size;
	h2cb->data = h2cb->head + hdr_size;
	h2cb->tail = h2cb->data;
	h2cb->hdr_len = hdr_size;
	h2cb->flags |= H2CB_FLAGS_FREED;

	return h2cb;
}

static u32 __h2cb_free(struct mac_ax_adapter *adapter,
		       enum h2c_buf_class buf_class)
{
	struct h2c_buf_head *list_head = &h2cb_head[buf_class];
	struct h2c_buf *h2cb;

	if (buf_class >= H2CB_CLASS_LAST)
		return MACNOITEM;

	if (!list_head->pool)
		return MACNPTR;

	if (!h2cb_queue_len(list_head))
		return MACSUCCESS;

	while ((h2cb = h2cb_dequeue(list_head)))
		PLTFM_FREE(h2cb, sizeof(struct h2c_buf));

	PLTFM_FREE(list_head->pool, list_head->size);
	list_head->pool = NULL;
	list_head->size = 0;
	PLTFM_MUTEX_DEINIT(&list_head->lock);

	return MACSUCCESS;
}

static u32 __h2cb_init(struct mac_ax_adapter *adapter,
		       enum h2c_buf_class buf_class, u32 num, u32 buf_size,
		       u32 hdr_size, u32 tailer_size)
{
	u32 i;
	u8 *ptr;
	struct h2c_buf_head *list_head = &h2cb_head[buf_class];
	u32 real_size = buf_size + hdr_size + tailer_size;
	struct h2c_buf *h2cb;

	if (buf_class >= H2CB_CLASS_LAST)
		return MACNOITEM;

	if (h2cb_queue_len(list_head))
		return MACBUFSZ;

	h2cb_queue_head_init(adapter, list_head);

	ptr = __h2cb_alloc_buf_pool(adapter, list_head, real_size, num);
	if (!ptr)
		return MACNPTR;

	for (i = 0; i < num; i++) {
		h2cb = __h2cb_alloc(adapter,
				    buf_class, hdr_size, ptr, real_size);
		if (!h2cb)
			goto h2cb_fail;
		__h2cb_queue_tail(list_head, h2cb);
		ptr += real_size;
	}

	return MACSUCCESS;
h2cb_fail:
	__h2cb_free(adapter, buf_class);

	return MACBUFALLOC;
}

static inline u32 fwcmd_wkb_queue_len(struct fwcmd_wkb_head *list)
{
	return list->qlen;
};

static inline void __fwcmd_wkb_queue_head_init(struct fwcmd_wkb_head *list)
{
	list->prev = (struct h2c_buf *)list;
	list->next = (struct h2c_buf *)list;
	list->qlen = 0;
};

static inline void fwcmd_wkb_queue_head_init(struct mac_ax_adapter *adapter,
					     struct fwcmd_wkb_head *list)
{
	PLTFM_MUTEX_INIT(&list->lock);
	__fwcmd_wkb_queue_head_init(list);
}

static u32 __fwcmd_wkb_init(struct mac_ax_adapter *adapter)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;

	if (fwcmd_wkb_queue_len(list_head))
		return MACBUFSZ;

	fwcmd_wkb_queue_head_init(adapter, list_head);

	return MACSUCCESS;
}

u32 h2cb_init(struct mac_ax_adapter *adapter)
{
	u32 ret;

	ret = __h2cb_init(adapter, H2CB_CLASS_CMD, H2CB_CMD_QLEN,
			  H2CB_CMD_SIZE, H2CB_CMD_HDR_SIZE, 0);
	if (ret)
		return ret;

	ret = __h2cb_init(adapter, H2CB_CLASS_DATA, H2CB_DATA_QLEN,
			  H2CB_DATA_SIZE, H2CB_DATA_HDR_SIZE, 0);
	if (ret)
		return ret;

	ret = __h2cb_init(adapter, H2CB_CLASS_LONG_DATA, H2CB_LONG_DATA_QLEN,
			  H2CB_LONG_DATA_SIZE, H2CB_LONG_DATA_HDR_SIZE, 0);
	if (ret)
		return ret;

	ret = __fwcmd_wkb_init(adapter);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 h2cb_exit(struct mac_ax_adapter *adapter)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;

	if (fwcmd_wkb_queue_len(list_head))
		return MACBUFSZ;

	__h2cb_free(adapter, H2CB_CLASS_CMD);
	__h2cb_free(adapter, H2CB_CLASS_DATA);
	__h2cb_free(adapter, H2CB_CLASS_LONG_DATA);

	return MACSUCCESS;
}

u32 h2c_end_flow(struct mac_ax_adapter *adapter)
{
	struct mac_ax_fw_info *fwinfo = &adapter->fw_info;

	PLTFM_MUTEX_LOCK(&fwinfo->seq_lock);
	fwinfo->h2c_seq++;
	PLTFM_MUTEX_UNLOCK(&fwinfo->seq_lock);

	return MACSUCCESS;
}

#if MAC_AX_PHL_H2C
u32 h2c_agg_enqueue(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb)
#else
u32 h2c_agg_enqueue(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb)
#endif
{
	struct mac_ax_h2c_agg_node *h2c_agg_node = NULL;
	u32 ret = MACSUCCESS;

	PLTFM_MUTEX_LOCK(&adapter->h2c_agg_info.h2c_agg_lock);

	if (!adapter->h2c_agg_info.h2c_agg_en) {
		/* return fall and send the H2C pkt by the orginal function*/
		ret = MACNOITEM;
		goto fail;
	}

	h2c_agg_node = (struct mac_ax_h2c_agg_node *)
				   PLTFM_MALLOC(sizeof(struct mac_ax_h2c_agg_node));
	if (!h2c_agg_node) {
		/* return fall and send the H2C pkt by the orginal function*/
		ret = MACBUFALLOC;
		goto fail;
	}

	PLTFM_MEMSET(h2c_agg_node, 0, sizeof(struct mac_ax_h2c_agg_node));
	h2c_agg_node->h2c_pkt = (u8 *)h2cb;
	h2c_agg_node->next = NULL;
	adapter->h2c_agg_info.h2c_agg_pkt_num++;

	if (!adapter->h2c_agg_info.h2c_agg_queue_head) {
		adapter->h2c_agg_info.h2c_agg_queue_head = h2c_agg_node;
		adapter->h2c_agg_info.h2c_agg_queue_last = h2c_agg_node;
	} else {
		adapter->h2c_agg_info.h2c_agg_queue_last->next = h2c_agg_node;
		adapter->h2c_agg_info.h2c_agg_queue_last = h2c_agg_node;
	}

fail:
	PLTFM_MUTEX_UNLOCK(&adapter->h2c_agg_info.h2c_agg_lock);
	return ret;
}

#if MAC_AX_PHL_H2C
struct rtw_h2c_pkt *h2cb_alloc(struct mac_ax_adapter *adapter,
			       enum rtw_h2c_pkt_type buf_class)
{
	struct rtw_h2c_pkt *h2cb;
#if MAC_AX_H2C_LMT_EN
	struct mac_ax_fw_info *fwinfo = &adapter->fw_info;
	u8 diff;
	u8 cnt = 100;
#endif

	if (buf_class >= H2CB_TYPE_MAX) {
		PLTFM_MSG_ERR("[ERR]unknown class\n");
		return NULL;
	}

#if MAC_AX_H2C_LMT_EN
	while (--cnt) {
		if (fwinfo->h2c_seq >= fwinfo->rec_seq)
			diff = fwinfo->h2c_seq - fwinfo->rec_seq;
		else
			diff = (255 - fwinfo->rec_seq) + fwinfo->h2c_seq;

		if (diff < FWCMD_LMT)
			break;
		PLTFM_DELAY_US(100);
	}

	if (diff >= FWCMD_LMT) {
		PLTFM_MSG_ERR("The number of H2C has reached the limitation\n");
		PLTFM_MSG_ERR("curr: %d, rec: %d\n",
			      fwinfo->h2c_seq, fwinfo->rec_seq);
		return NULL;
	}
#endif

	h2cb = PLTFM_QUERY_H2C(buf_class);

	return h2cb;
}

void h2cb_free(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb)
{
}

u8 *h2cb_push(struct rtw_h2c_pkt *h2cb, u32 len)
{
	h2cb->vir_data -= len;
	h2cb->data_len  += len;

	if (h2cb->vir_data < h2cb->vir_head)
		return NULL;

	return h2cb->vir_data;
}

u8 *h2cb_pull(struct rtw_h2c_pkt *h2cb, u32 len)
{
	h2cb->vir_data += len;

	if (h2cb->vir_data > h2cb->vir_end)
		return NULL;

	if (h2cb->data_len < len)
		return NULL;

	h2cb->data_len -= len;

	return h2cb->vir_data;
}

u8 *h2cb_put(struct rtw_h2c_pkt *h2cb, u32 len)
{
	u8 *tmp = h2cb_tail_pointer(h2cb);

	h2cb->vir_tail += len;
	h2cb->data_len += len;

	if (h2cb->vir_tail > h2cb->vir_end)
		return NULL;

	return tmp;
}

u32 h2c_pkt_set_hdr(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		    u8 type, u8 cat, u8 _class_, u8 func, u16 rack, u16 dack)
{
	struct fwcmd_hdr *hdr;
	struct mac_ax_fw_info *fwinfo = &adapter->fw_info;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACFWNONRDY;

	if (adapter->sm.mac_rdy != MAC_AX_MAC_RDY) {
		PLTFM_MSG_TRACE("%s: MAC is not ready\n", __func__);
		adapter->stats.h2c_pkt_uninit++;
	}

	hdr = (struct fwcmd_hdr *)h2cb_push(h2cb, FWCMD_HDR_LEN);
	if (!hdr)
		return MACNPTR;

	hdr->hdr0 = cpu_to_le32(SET_WORD(type, H2C_HDR_DEL_TYPE) |
				SET_WORD(cat, H2C_HDR_CAT) |
				SET_WORD(_class_, H2C_HDR_CLASS) |
				SET_WORD(func, H2C_HDR_FUNC) |
				SET_WORD(fwinfo->h2c_seq, H2C_HDR_H2C_SEQ));

	hdr->hdr1 = cpu_to_le32(SET_WORD(h2cb->data_len, H2C_HDR_TOTAL_LEN) |
				(rack ?	H2C_HDR_REC_ACK : 0) |
				(dack ? H2C_HDR_DONE_ACK : 0));

	h2cb->id = SET_FWCMD_ID(type, cat, _class_, func);
	h2cb->h2c_seq = fwinfo->h2c_seq;

	return MACSUCCESS;
}

u32 h2c_pkt_set_hdr_fwdl(struct mac_ax_adapter *adapter,
			 struct rtw_h2c_pkt *h2cb, u8 type, u8 cat,
			 u8 _class_, u8 func, u16 rack, u16 dack)
{
	struct fwcmd_hdr *hdr;
	struct mac_ax_fw_info *fwinfo = &adapter->fw_info;

	hdr = (struct fwcmd_hdr *)h2cb_push(h2cb, FWCMD_HDR_LEN);
	if (!hdr)
		return MACNPTR;

	hdr->hdr0 = cpu_to_le32(SET_WORD(type, H2C_HDR_DEL_TYPE) |
				SET_WORD(cat, H2C_HDR_CAT) |
				SET_WORD(_class_, H2C_HDR_CLASS) |
				SET_WORD(func, H2C_HDR_FUNC) |
				SET_WORD(fwinfo->h2c_seq, H2C_HDR_H2C_SEQ));

	hdr->hdr1 = cpu_to_le32(SET_WORD(h2cb->data_len, H2C_HDR_TOTAL_LEN) |
				(rack ? H2C_HDR_REC_ACK : 0) |
				(dack ? H2C_HDR_DONE_ACK : 0));

	h2cb->id = SET_FWCMD_ID(type, cat, _class_, func);
	h2cb->h2c_seq = fwinfo->h2c_seq;

	return MACSUCCESS;
}

u32 h2c_pkt_set_cmd(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		    u8 *cmd, u32 len)
{
	u8 *buf;

	buf = h2cb_put(h2cb, len);
	if (!buf)
		return MACNPTR;
	PLTFM_MEMCPY(buf, cmd, len);
	return MACSUCCESS;
}

u32 h2c_pkt_build_txd(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb)
{
	u8 *buf;
	u32 ret;
	u32 txd_len;
	struct rtw_t_meta_data info = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);

	info.type = RTW_PHL_PKT_TYPE_H2C;
	info.pktlen = (u16)h2cb->data_len;
	txd_len = ops->txdesc_len(adapter, &info);
	if (adapter->hw_info->intf == MAC_AX_INTF_USB) {
		if (((info.pktlen + txd_len) & (512 - 1)) == 0) {
			buf = h2cb_put(h2cb, 4);
			if (!buf) {
				PLTFM_MSG_ERR("Avoid USB 512-byte FAIL\n");
				return MACNPTR;
			}
			info.pktlen = (u16)h2cb->data_len;
			txd_len = ops->txdesc_len(adapter, &info);
		}
	}

	buf = h2cb_push(h2cb, txd_len);
	if (!buf)
		return MACNPTR;

	ret = ops->build_txdesc(adapter, &info, buf, txd_len);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 fwcmd_wq_idle(struct mac_ax_adapter *adapter, u32 id)
{
	return MACSUCCESS;
}

#else
struct h2c_buf *h2cb_alloc(struct mac_ax_adapter *adapter,
			   enum h2c_buf_class buf_class)
{
	struct h2c_buf_head *list_head = &h2cb_head[buf_class];
	struct h2c_buf *h2cb;

	if (buf_class >= H2CB_CLASS_LAST) {
		PLTFM_MSG_ERR("[ERR]unknown class\n");
		return NULL;
	}

	PLTFM_MUTEX_LOCK(&list_head->lock);

	h2cb = h2cb_dequeue(list_head);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]allocate h2cb, class : %d\n", buf_class);
		goto h2cb_fail;
	}

	if (!(h2cb->flags & H2CB_FLAGS_FREED)) {
		PLTFM_MSG_ERR("[ERR]not freed flag\n");
		PLTFM_FREE(h2cb, sizeof(struct h2c_buf));
		goto h2cb_fail;
	}

	h2cb->flags &= ~H2CB_FLAGS_FREED;
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return h2cb;
h2cb_fail:
	PLTFM_MUTEX_UNLOCK(&list_head->lock);
	return NULL;
}

void h2cb_free(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb)
{
	struct h2c_buf_head *list_head;

	if (h2cb->flags & H2CB_FLAGS_FREED) {
		PLTFM_MSG_ERR("[ERR]freed flag\n");
		return;
	}

	if (h2cb->_class_ >= H2CB_CLASS_LAST) {
		PLTFM_MSG_ERR("[ERR]unknown class\n");
		return;
	}

	list_head = &h2cb_head[h2cb->_class_];

	h2cb->len = 0;
	h2cb->data = h2cb->head + h2cb->hdr_len;
	h2cb->tail = h2cb->data;
	h2cb->flags |= H2CB_FLAGS_FREED;

	PLTFM_MUTEX_LOCK(&list_head->lock);
	__h2cb_queue_tail(list_head, h2cb);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);
}

u8 *h2cb_push(struct h2c_buf *h2cb, u32 len)
{
	h2cb->data -= len;
	h2cb->len  += len;

	if (h2cb->data < h2cb->head)
		return NULL;

	return h2cb->data;
}

u8 *h2cb_pull(struct h2c_buf *h2cb, u32 len)
{
	h2cb->data += len;

	if (h2cb->data > h2cb->end)
		return NULL;

	if (h2cb->len < len)
		return NULL;

	h2cb->len -= len;

	return h2cb->data;
}

u8 *h2cb_put(struct h2c_buf *h2cb, u32 len)
{
	u8 *tmp = h2cb_tail_pointer(h2cb);

	h2cb->tail += len;
	h2cb->len += len;

	if (h2cb->tail > h2cb->end)
		return NULL;

	return tmp;
}

u32 h2c_pkt_set_hdr(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb,
		    u8 type, u8 cat, u8 _class_, u8 func, u16 rack, u16 dack)
{
	struct fwcmd_hdr *hdr;
	struct mac_ax_fw_info *fwinfo = &adapter->fw_info;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACFWNONRDY;

	if (adapter->sm.mac_rdy != MAC_AX_MAC_RDY) {
		PLTFM_MSG_TRACE("MAC is not ready\n");
		adapter->stats.h2c_pkt_uninit++;
	}

	hdr = (struct fwcmd_hdr *)h2cb_push(h2cb, FWCMD_HDR_LEN);
	if (!hdr)
		return MACNPTR;

	hdr->hdr0 = cpu_to_le32(SET_WORD(type, H2C_HDR_DEL_TYPE) |
				SET_WORD(cat, H2C_HDR_CAT) |
				SET_WORD(_class_, H2C_HDR_CLASS) |
				SET_WORD(func, H2C_HDR_FUNC) |
				SET_WORD(fwinfo->h2c_seq, H2C_HDR_H2C_SEQ));

	hdr->hdr1 = cpu_to_le32(SET_WORD(h2cb->len, H2C_HDR_TOTAL_LEN) |
				(rack ? H2C_HDR_REC_ACK : 0) |
				(dack ? H2C_HDR_DONE_ACK : 0));

	h2cb->id = SET_FWCMD_ID(type, cat, _class_, func);
	h2cb->h2c_seq = fwinfo->h2c_seq;

	return 0;
}

u32 h2c_pkt_set_hdr_fwdl(struct mac_ax_adapter *adapter,
			 struct h2c_buf *h2cb, u8 type, u8 cat,
			 u8 _class_, u8 func, u16 rack, u16 dack)
{
	struct fwcmd_hdr *hdr;
	struct mac_ax_fw_info *fwinfo = &adapter->fw_info;

	hdr = (struct fwcmd_hdr *)h2cb_push(h2cb, FWCMD_HDR_LEN);
	if (!hdr)
		return MACNPTR;

	hdr->hdr0 = cpu_to_le32(SET_WORD(type, H2C_HDR_DEL_TYPE) |
				SET_WORD(cat, H2C_HDR_CAT) |
				SET_WORD(_class_, H2C_HDR_CLASS) |
				SET_WORD(func, H2C_HDR_FUNC) |
				SET_WORD(fwinfo->h2c_seq, H2C_HDR_H2C_SEQ));

	hdr->hdr1 = cpu_to_le32(SET_WORD(h2cb->len, H2C_HDR_TOTAL_LEN) |
				(rack ? H2C_HDR_REC_ACK : 0) |
				(dack ? H2C_HDR_DONE_ACK : 0));

	h2cb->id = SET_FWCMD_ID(type, cat, _class_, func);
	h2cb->h2c_seq = fwinfo->h2c_seq;

	return 0;
}

u32 h2c_pkt_set_cmd(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb,
		    u8 *cmd, u32 len)
{
	u8 *buf;

	buf = h2cb_put(h2cb, len);
	if (!buf)
		return MACNPTR;
	PLTFM_MEMCPY(buf, cmd, len);
	return MACSUCCESS;
}

u32 h2c_pkt_build_txd(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb)
{
	u8 *buf;
	u32 ret;
	u32 txd_len;
	struct rtw_t_meta_data info = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);

	info.type = RTW_PHL_PKT_TYPE_H2C;
	info.pktlen = (u16)h2cb->len;
	txd_len = ops->txdesc_len(adapter, &info);
	if (adapter->hw_info->intf == MAC_AX_INTF_USB) {
		if (((info.pktlen + txd_len) & (512 - 1)) == 0) {
			buf = h2cb_put(h2cb, 4);
			if (!buf) {
				PLTFM_MSG_ERR("Avoid USB 512-byte FAIL\n");
				return MACNPTR;
			}
			info.pktlen = (u16)h2cb->len;
			txd_len = ops->txdesc_len(adapter, &info);
		}
	}

	buf = h2cb_push(h2cb, txd_len);
	if (!buf)
		return MACNPTR;

	ret = ops->build_txdesc(adapter, &info, buf, txd_len);
	if (ret)
		return ret;

	return MACSUCCESS;
}

static inline void __fwcmd_wq_insert(struct h2c_buf *new_h2cb,
				     struct h2c_buf *prev, struct h2c_buf *next,
				     struct fwcmd_wkb_head *list)
{
	new_h2cb->next = next;
	new_h2cb->prev = prev;
	next->prev  = new_h2cb;
	prev->next = new_h2cb;
	list->qlen++;
}

static inline void __fwcmd_wq_before(struct fwcmd_wkb_head *list,
				     struct h2c_buf *next,
				     struct h2c_buf *new_h2cb)
{
	__fwcmd_wq_insert(new_h2cb, next->prev, next, list);
}

static inline void __fwcmd_wq_tail(struct fwcmd_wkb_head *list,
				   struct h2c_buf *new_h2cb)
{
	__fwcmd_wq_before(list, (struct h2c_buf *)list, new_h2cb);
}

u32 fwcmd_wq_enqueue(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;

	if (list_head->qlen > FWCMD_WQ_MAX_JOB_NUM) {
		PLTFM_MSG_WARN("[WARN]fwcmd work queue full\n");
		return MACBUFALLOC;
	}

	/* worq queue doesn't need wd body */
	h2cb_pull(h2cb, WD_BODY_LEN);
	PLTFM_MUTEX_LOCK(&list_head->lock);
	__fwcmd_wq_tail(list_head, h2cb);
	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return MACSUCCESS;
}

static inline void __fwcmd_wq_unlink(struct h2c_buf *h2cb,
				     struct fwcmd_wkb_head *list)
{
	struct h2c_buf *next, *prev;

	list->qlen--;
	next = h2cb->next;
	prev = h2cb->prev;
	h2cb->prev = NULL;
	h2cb->next = NULL;
	next->prev = prev;
	prev->next = next;
}

struct h2c_buf *fwcmd_wq_dequeue(struct mac_ax_adapter *adapter, u32 id)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;
	struct h2c_buf *h2cb;
	u32 hdr0;
	u16 type = GET_FWCMD_TYPE(id);
	u16 cat = GET_FWCMD_CAT(id);
	u16 _class_ = GET_FWCMD_CLASS(id);
	u16 func = GET_FWCMD_FUNC(id);

	PLTFM_MUTEX_LOCK(&list_head->lock);

	for (h2cb = list_head->next; h2cb->next != list_head->next;
	     h2cb = h2cb->next) {
		hdr0 = ((struct fwcmd_hdr *)h2cb->data)->hdr0;
		hdr0 = le32_to_cpu(hdr0);
		if (type == GET_FIELD(hdr0, H2C_HDR_DEL_TYPE) &&
		    cat == GET_FIELD(hdr0, H2C_HDR_CAT) &&
		    _class_ == GET_FIELD(hdr0, H2C_HDR_CLASS) &&
		    func == GET_FIELD(hdr0, H2C_HDR_FUNC)) {
			__fwcmd_wq_unlink(h2cb, list_head);
			PLTFM_MUTEX_UNLOCK(&list_head->lock);
			return h2cb;
		}
	}

	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	PLTFM_MSG_ERR("[ERR]cannot find wq item: %X\n", id);

	return NULL;
}

u32 fwcmd_wq_idle(struct mac_ax_adapter *adapter, u32 id)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;
	struct h2c_buf *h2cb;

	PLTFM_MUTEX_LOCK(&list_head->lock);

	for (h2cb = list_head->next; h2cb->next != list_head->next;
	     h2cb = h2cb->next) {
		if (h2cb->id == id) {
			PLTFM_MUTEX_UNLOCK(&list_head->lock);
			return MACWQBUSY;
		}
	}

	PLTFM_MUTEX_UNLOCK(&list_head->lock);

	return MACSUCCESS;
}
#endif

static u32 c2h_fwi_cmd_log(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			   struct rtw_c2h_info *info)
{
#if MAC_AX_FEATURE_DBGDEC
	u8 syntax_1 = 0, syntax_2 = 0;

	if ((len - FWCMD_HDR_LEN) >= 11) {
		syntax_1 = *(buf + FWCMD_HDR_LEN);
		syntax_2 = *(buf + FWCMD_HDR_LEN + 1);
	}
	//PLTFM_MSG_WARN("C2H encoded log syntax_1 %x, syntax_2 %x", syntax_1, syntax_2);
	if (syntax_1 == 0xa5 && syntax_2 == 0xa5) {
		//PLTFM_MSG_WARN("C2H encoded log");
		fw_log_dump(adapter, buf, len, info);
	} else {
		if (buf[len - 1] != '\0')
			buf[len - 1] = '\0';
		PLTFM_MSG_WARN("C2H log: %s", (char *)(buf + FWCMD_HDR_LEN));
	}
#else
	if (buf[len - 1] != '\0')
		buf[len - 1] = '\0';
	PLTFM_MSG_WARN("C2H log: %s", (char *)(buf + FWCMD_HDR_LEN));
#endif
	return MACSUCCESS;
}

static u32 c2h_wow_rcv_ack_hdl(struct mac_ax_adapter *adapter,
			       struct rtw_c2h_info *info)
{
	u8 *state;

	switch (info->c2h_func) {
	case FWCMD_H2C_FUNC_AOAC_REPORT_REQ:
		state = &adapter->sm.aoac_rpt;
		PLTFM_MSG_ERR("aoac_rpt rcv ack success\n");
		break;

	default:
		return MACSUCCESS;
	}

	if (*state == MAC_AX_AOAC_RPT_H2C_SENDING)
		*state = MAC_AX_AOAC_RPT_H2C_RCVD;

	return MACSUCCESS;
}

static u32 c2h_fwofld_rcv_ack_hdl(struct mac_ax_adapter *adapter,
				  struct rtw_c2h_info *info)
{
	u8 *state;

	switch (info->c2h_func) {
	case FWCMD_H2C_FUNC_WRITE_OFLD:
		state = &adapter->sm.write_h2c;
		break;

	case FWCMD_H2C_FUNC_CONF_OFLD:
		state = &adapter->sm.conf_h2c;
		break;

	case FWCMD_H2C_FUNC_PACKET_OFLD:
		state = &adapter->sm.pkt_ofld;
		break;

	case FWCMD_H2C_FUNC_READ_OFLD:
		state = &adapter->sm.read_h2c;
		break;

	case FWCMD_H2C_FUNC_DUMP_EFUSE:
		state = &adapter->sm.efuse_ofld;
		break;
	case FWCMD_H2C_FUNC_CH_SWITCH:
		state = &adapter->sm.ch_switch;
		break;

	default:
		return MACSUCCESS;
	}

	if (*state == MAC_AX_OFLD_H2C_SENDING)
		*state = MAC_AX_OFLD_H2C_RCVD;

	return MACSUCCESS;
}

static u32 c2h_proxy_ack(struct mac_ax_adapter *adapter, struct rtw_c2h_info *info, u8 is_rcv)
{
	struct mac_ax_state_mach *sm = &adapter->sm;

	switch (is_rcv) {
	case 0:
		if (sm->proxy_st != MAC_AX_PROXY_BUSY) {
			PLTFM_MSG_ERR("[Proxy][DoneAck] is_rcv (%d) doesn't match sm (%d)\n",
				      is_rcv, sm->proxy_st);
			return MACPROCERR;
		}
		sm->proxy_st = MAC_AX_PROXY_IDLE;
		sm->proxy_ret = info->h2c_return;
		if (info->h2c_return != MACSUCCESS)
			PLTFM_MSG_ERR("[Proxy][DoneAck] h2c return not success (%d)\n",
				      info->h2c_return);
		else
			PLTFM_MSG_TRACE("[Proxy][DoneAck]\n");
		break;
	case 1:
		if (sm->proxy_st != MAC_AX_PROXY_SENDING) {
			PLTFM_MSG_ERR("[Proxy][RecvAck] is_rcv (%d) doesn't match sm (%d)\n",
				      is_rcv, sm->proxy_st);
			return MACPROCERR;
		}
		PLTFM_MSG_TRACE("[Proxy][RecvAck]\n");
		sm->proxy_st = MAC_AX_PROXY_BUSY;
		break;
	default:
		PLTFM_MSG_ERR("[Proxy][Ack] is_rcv bad value (%d)\n", is_rcv);
	}
	return MACSUCCESS;
}

static u32 c2h_fwi_rev_ack(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			   struct rtw_c2h_info *info)
{
	u32 data = *(u32 *)(buf + FWCMD_HDR_LEN);
	u32 ret;
	u32 cat;

	data = le32_to_cpu(data);

	cat = GET_FIELD(data, FWCMD_C2H_REC_ACK_CAT);
	if (cat == FWCMD_H2C_CAT_OUTSRC || cat == FWCMD_H2C_CAT_TEST)
		return MACSUCCESS;

	info->c2h_cat = GET_FIELD(data, FWCMD_C2H_REC_ACK_CAT);
	info->c2h_class = GET_FIELD(data, FWCMD_C2H_REC_ACK_CLASS);
	info->c2h_func = GET_FIELD(data, FWCMD_C2H_REC_ACK_FUNC);
	info->h2c_seq = GET_FIELD(data, FWCMD_C2H_REC_ACK_H2C_SEQ);
	adapter->fw_info.rec_seq = info->h2c_seq;
	info->type_rec_ack = 1;

	if (info->c2h_cat == FWCMD_H2C_CAT_MAC) {
		switch (info->c2h_class) {
		case FWCMD_H2C_CL_WOW:
			ret = c2h_wow_rcv_ack_hdl(adapter, info);
			if (ret)
				return ret;
			break;

		case FWCMD_H2C_CL_FW_OFLD:
			ret = c2h_fwofld_rcv_ack_hdl(adapter, info);
			if (ret)
				return ret;
			break;
		case FWCMD_H2C_CL_PROXY:
			ret = c2h_proxy_ack(adapter, info, 1);
			if (ret)
				return ret;
			break;

		default:
			return MACSUCCESS;
		}
	}

	return MACSUCCESS;
}

static u32 c2h_fwofld_done_ack_hdl(struct mac_ax_adapter *adapter,
				   struct rtw_c2h_info *info)
{
	struct mac_ax_state_mach *sm = &adapter->sm;
	struct mac_ax_pkt_ofld_info *ofld_info = &adapter->pkt_ofld_info;
	struct mac_ax_scanofld_info *scanofld_info = &adapter->scanofld_info;
	struct sensing_csi_info *csi_info = &adapter->csi_info;
	u8 scanofld_band;
	u8 scanofld_return;
	u8 *pfw_scan_busy;
	u8 *pfw_chlist_busy;

	switch (info->c2h_func) {
	case FWCMD_H2C_FUNC_WRITE_OFLD:
		if (sm->write_h2c == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS)
				sm->write_h2c = MAC_AX_OFLD_H2C_IDLE;
			else
				sm->write_h2c = MAC_AX_OFLD_H2C_ERROR;
		}
		break;

	case FWCMD_H2C_FUNC_CONF_OFLD:
		if (sm->conf_h2c == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS)
				sm->conf_h2c = MAC_AX_OFLD_H2C_IDLE;
			else
				sm->conf_h2c = MAC_AX_OFLD_H2C_ERROR;
		}
		break;

	case FWCMD_H2C_FUNC_PACKET_OFLD:
		if (sm->pkt_ofld == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS) {
				if (ofld_info->last_op == PKT_OFLD_OP_READ)
					sm->pkt_ofld = MAC_AX_OFLD_H2C_DONE;
				else
					sm->pkt_ofld = MAC_AX_OFLD_H2C_IDLE;
			} else {
				sm->pkt_ofld = MAC_AX_OFLD_H2C_ERROR;
			}
		} else {
			PLTFM_MSG_ERR("cant set pkt ofld state since no recv ack is received.");
		}
		break;

	case FWCMD_H2C_FUNC_READ_OFLD:
		if (sm->read_h2c == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS)
				sm->read_h2c = MAC_AX_OFLD_H2C_DONE;
			else
				sm->read_h2c = MAC_AX_OFLD_H2C_ERROR;
		}
		break;

	case FWCMD_H2C_FUNC_DUMP_EFUSE:
		if (sm->efuse_ofld == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS)
				sm->efuse_ofld = MAC_AX_OFLD_H2C_DONE;
			else
				sm->efuse_ofld = MAC_AX_OFLD_H2C_ERROR;
		}
		break;
	case FWCMD_H2C_FUNC_ADD_SCANOFLD_CH:
		scanofld_band = info->h2c_return >> SCANOFLD_ACK_BAND_SHIFT;
		scanofld_return = info->h2c_return & SCANOFLD_ACK_RETURN_MASK;
		PLTFM_MSG_TRACE("[scnofld](%d) got add scanofld done ack. clear fw chlist busy\n",
				scanofld_band);

		PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
		adapter->scanofld_info.fw_chlist_busy[scanofld_band] = 0;
		PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);

		PLTFM_MSG_TRACE("[scnofld][%d] drv_chlist_state = %d, fw_chlist_state = %d\n",
				scanofld_band,
				adapter->scanofld_info.drv_chlist_busy[scanofld_band],
				adapter->scanofld_info.fw_chlist_busy[scanofld_band]);
		break;
	case FWCMD_H2C_FUNC_SCANOFLD:
		scanofld_band = info->h2c_return >> SCANOFLD_ACK_BAND_SHIFT;
		scanofld_return = info->h2c_return & SCANOFLD_ACK_RETURN_MASK;
		if (scanofld_return != MACSUCCESS) {
			PLTFM_MSG_TRACE("[scnofld][%d] scan func fail,revert fw related status\n",
					scanofld_band);

			PLTFM_MUTEX_LOCK(&scanofld_info->fw_chlist_state_lock);
			pfw_chlist_busy = &scanofld_info->fw_chlist_busy[scanofld_band];
			*pfw_chlist_busy = scanofld_info->last_fw_chlist_busy[scanofld_band];
			PLTFM_MUTEX_UNLOCK(&scanofld_info->fw_chlist_state_lock);

			pfw_scan_busy = &scanofld_info->fw_scan_busy[scanofld_band];
			*pfw_scan_busy = scanofld_info->last_fw_scan_busy[scanofld_band];
		}
		PLTFM_MSG_TRACE("[scnofld][%d] fw_scan_busy = %d, fw_chlist_state = %d\n",
				scanofld_band,
				scanofld_info->fw_scan_busy[scanofld_band],
				scanofld_info->fw_chlist_busy[scanofld_band]);
		break;

	case FWCMD_H2C_FUNC_WIFI_SENSING_CSI:
		if (csi_info->start_cmd_send && csi_info->stop_cmd_send)
			PLTFM_MSG_ERR("[ERR][csi][DoneAck] state machine error!\n");

		if (csi_info->start_cmd_send) {
			if (info->h2c_return == MACSUCCESS)
				csi_info->func_en = 1;

			csi_info->start_cmd_send = 0;
		}

		if (csi_info->stop_cmd_send) {
			if (info->h2c_return == MACSUCCESS)
				csi_info->func_en = 0;

			csi_info->stop_cmd_send = 0;
		}

		PLTFM_MUTEX_LOCK(&csi_info->state_lock);
		sm->sensing_csi_st = MAC_AX_SENSING_CSI_IDLE;
		PLTFM_MUTEX_UNLOCK(&csi_info->state_lock);
		break;

	case FWCMD_H2C_FUNC_SCH_TX_EN_PKT:
		if (sm->sch_tx_en_ofld == MAC_AX_OFLD_H2C_SENDING) {
			if (info->h2c_return == MACSUCCESS)
				sm->sch_tx_en_ofld = MAC_AX_OFLD_H2C_DONE;
			else
				sm->sch_tx_en_ofld = MAC_AX_OFLD_H2C_ERROR;
		}
		break;

	case FWCMD_H2C_FUNC_MACID_PAUSE_SLEEP:
		if (sm->macid_pause_sleep == MAC_AX_OFLD_H2C_SENDING) {
			if (info->h2c_return == MACSUCCESS)
				sm->macid_pause_sleep = MAC_AX_OFLD_H2C_DONE;
			else
				sm->macid_pause_sleep = MAC_AX_OFLD_H2C_ERROR;
		}
		break;

	default:
		break;
	}
	return MACSUCCESS;
}

static u32 c2h_role_done_ack_hdl(struct mac_ax_adapter *adapter,
				 struct rtw_c2h_info *info)
{
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	struct mac_ax_state_mach *sm = &adapter->sm;

	if (info->c2h_class == FWCMD_H2C_CL_MEDIA_RPT &&
	    info->c2h_func == FWCMD_H2C_FUNC_FWROLE_MAINTAIN) {
		if (info->h2c_return == MACSUCCESS) {
			sm->role_stat = MAC_AX_ROLE_ALOC_SUCC;
		} else {
			PLTFM_MSG_ERR("[ERR]role_maintain: alloc failed\n");
			sm->role_stat = MAC_AX_ROLE_ALOC_FAIL;
			return MACROLEALOCFL;
		}
	} else if (info->c2h_class == FWCMD_H2C_CL_MEDIA_RPT &&
		   info->c2h_func == FWCMD_H2C_FUNC_JOININFO) {
		if (info->h2c_return == MACSUCCESS) {
			sm->role_stat = MAC_AX_ROLE_INIT_SUCC;
		} else {
			PLTFM_MSG_ERR("[ERR]role_join: init failed\n");
			sm->role_stat = MAC_AX_ROLE_INIT_FAIL;
			return MACROLEINITFL;
		}
	} else if (info->c2h_class == FWCMD_H2C_CL_MEDIA_RPT &&
		   info->c2h_func == FWCMD_H2C_FUNC_NOTIFY_DBCC) {
		if (info->h2c_return == MACSUCCESS) {
			dbcc_info->notify_fw_flag = 0;
		} else {
			PLTFM_MSG_ERR("[ERR]dbcc notify ack fail %d\n",
				      info->h2c_return);
			return MACSUCCESS;
		}
	} else if (info->c2h_class == FWCMD_H2C_CL_ADDR_CAM_UPDATE &&
		   info->c2h_func == FWCMD_H2C_FUNC_ADDRCAM_INFO) {
		if (info->h2c_return == MACSUCCESS) {
			sm->role_stat = MAC_AX_ROLE_HW_UPD_SUCC;
		} else {
			PLTFM_MSG_ERR("[ERR]ADDR_CAM: upd failed\n");
			sm->role_stat = MAC_AX_ROLE_HW_UPD_FAIL;
			return MACROLEHWUPDFL;
		}
	}
	return MACSUCCESS;
}

static u32 c2h_ps_done_ack_hdl(struct mac_ax_adapter *adapter,
			       struct rtw_c2h_info *info)
{
	struct mac_ax_state_mach *sm = &adapter->sm;
	u8 p2pid;
	u32 ret;

	switch (info->c2h_func) {
	case FWCMD_H2C_FUNC_P2P_ACT:
		if (sm->p2p_stat != MAC_AX_P2P_ACT_BUSY) {
			PLTFM_MSG_ERR("[ERR]p2p act dack stat err %d\n",
				      sm->p2p_stat);
			return MACPROCERR;
		}

		if (info->h2c_return != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]p2p act dack ret %d\n",
				      info->h2c_return);
			sm->p2p_stat = MAC_AX_P2P_ACT_FAIL;
			break;
		}

		p2pid = P2PID_INVALID;
		ret = get_wait_dack_p2pid(adapter, &p2pid);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]p2p act dack get wait id %d\n", ret);
			return ret;
		}
		if (p2pid == P2PID_INVALID) {
			PLTFM_MSG_ERR("[ERR]p2p act dack no wait id\n");
			sm->p2p_stat = MAC_AX_P2P_ACT_IDLE;
			return MACPROCERR;
		}

		if (adapter->p2p_info[p2pid].wait_term &&
		    adapter->p2p_info[p2pid].wait_init) {
			PLTFM_MSG_ERR("[ERR]p2p act dack wait term & init\n");
		} else if (adapter->p2p_info[p2pid].wait_term) {
			PLTFM_MEMSET(&adapter->p2p_info[p2pid], 0,
				     sizeof(struct mac_ax_p2p_info));
		} else if (adapter->p2p_info[p2pid].wait_init) {
			adapter->p2p_info[p2pid].run = 1;
			adapter->p2p_info[p2pid].wait_init = 0;
			adapter->p2p_info[p2pid].wait_dack = 0;
		} else {
			adapter->p2p_info[p2pid].wait_dack = 0;
		}
		sm->p2p_stat = MAC_AX_P2P_ACT_IDLE;
		break;
	case FWCMD_H2C_FUNC_P2P_MACID_CTRL:
		if (sm->p2p_stat != MAC_AX_P2P_ACT_BUSY) {
			PLTFM_MSG_ERR("[ERR]p2p macid ctrl dack stat err %d\n",
				      sm->p2p_stat);
			return MACPROCERR;
		}

		if (info->h2c_return != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]p2p macid ctrl dack ret %d\n",
				      info->h2c_return);
			sm->p2p_stat = MAC_AX_P2P_ACT_FAIL;
			break;
		}

		p2pid = P2PID_INVALID;
		ret = get_wait_dack_p2pid(adapter, &p2pid);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]p2p macid ctrl dack get wait id %d\n",
				      ret);
			return ret;
		}
		if (p2pid != P2PID_INVALID) {
			if (adapter->p2p_info[p2pid].run) {
				adapter->p2p_info[p2pid].wait_dack = 0;
			} else {
				PLTFM_MSG_ERR("[ERR]p2p%d macid ctrl dack not run\n",
					      p2pid);
			}
		} else {
			PLTFM_MSG_ERR("[ERR]p2p macid ctrl dack no wait id\n");
		}
		sm->p2p_stat = MAC_AX_P2P_ACT_IDLE;
		break;
	case FWCMD_H2C_FUNC_IPS_CFG:
		if (info->h2c_return != MACSUCCESS)
			PLTFM_MSG_ERR("[ERR]fwips dack ret %d\n", info->h2c_return);
		break;
	case FWCMD_H2C_FUNC_PS_ADVANCE_PARM:
		if (info->h2c_return != MACSUCCESS)
			PLTFM_MSG_ERR("[ERR]ps advanced parm dack ret %d\n", info->h2c_return);
		break;
	case FWCMD_H2C_FUNC_PERIODIC_WAKE:
		if (info->h2c_return != MACSUCCESS)
			PLTFM_MSG_ERR("[ERR] periodic wake dack ret %d\n", info->h2c_return);
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

static u32 c2h_nan_done_ack_hdl(struct mac_ax_adapter *adapter, struct rtw_c2h_info *info)
{
	struct mac_ax_state_mach *sm = &adapter->sm;

	switch (info->c2h_func) {
	case FWCMD_H2C_FUNC_ACT_SCHEDULE_REQ:

		PLTFM_MSG_ERR("[ERR] nan current state %d\n", sm->nan_stat);
		if (info->h2c_return == MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]nan act dack ret %d\n", info->h2c_return);
			sm->nan_stat = MAX_AX_NAN_ACT_H2C_FAIL;
			break;
		}
	}
	return MACSUCCESS;
}

static u32 c2h_fwi_done_ack(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			    struct rtw_c2h_info *info)
{
	u32 data = *(u32 *)(buf + FWCMD_HDR_LEN);
	u32 ret;
	u32 cat;

	data = le32_to_cpu(data);

	cat = GET_FIELD(data, FWCMD_C2H_REC_ACK_CAT);
	if (cat == FWCMD_H2C_CAT_OUTSRC || cat == FWCMD_H2C_CAT_TEST)
		return MACSUCCESS;

	info->c2h_cat = GET_FIELD(data, FWCMD_C2H_DONE_ACK_CAT);
	info->c2h_class = GET_FIELD(data, FWCMD_C2H_DONE_ACK_CLASS);
	info->c2h_func = GET_FIELD(data, FWCMD_C2H_DONE_ACK_FUNC);
	info->h2c_return = GET_FIELD(data, FWCMD_C2H_DONE_ACK_H2C_RETURN);
	info->h2c_seq = GET_FIELD(data, FWCMD_C2H_DONE_ACK_H2C_SEQ);
	info->type_done_ack = 1;

	if (info->c2h_cat == FWCMD_H2C_CAT_MAC) {
		if (info->c2h_class == FWCMD_H2C_CL_FW_OFLD) {
			ret = c2h_fwofld_done_ack_hdl(adapter, info);
			if (ret != MACSUCCESS)
				return ret;
		} else if (info->c2h_class == FWCMD_H2C_CL_PS) {
			ret = c2h_ps_done_ack_hdl(adapter, info);
			if (ret != MACSUCCESS)
				return ret;
		} else if (info->c2h_class == FWCMD_H2C_CL_MEDIA_RPT ||
			   info->c2h_class == FWCMD_H2C_CL_ADDR_CAM_UPDATE) {
			ret = c2h_role_done_ack_hdl(adapter, info);
			if (ret != MACSUCCESS)
				return ret;
		} else if (info->c2h_class == FWCMD_H2C_CL_PROXY) {
			ret = c2h_proxy_ack(adapter, info, 0);
			if (ret != MACSUCCESS)
				return ret;
		} else if (info->c2h_class == FWCMD_H2C_CL_NAN) {
			ret = c2h_nan_done_ack_hdl(adapter, info);
			if (ret != MACSUCCESS)
				return ret;
		}
	}

	return MACSUCCESS;
}

static u32 c2h_fwi_bcn_stats(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			     struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static u32 c2h_fwi_bcn_csazero(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			       struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_fw_info_cmd[] = {
	{FWCMD_C2H_FUNC_REC_ACK, c2h_fwi_rev_ack},
	{FWCMD_C2H_FUNC_DONE_ACK, c2h_fwi_done_ack},
	{FWCMD_C2H_FUNC_C2H_LOG, c2h_fwi_cmd_log},
	{FWCMD_C2H_FUNC_BCN_CNT, c2h_fwi_bcn_stats},
	{FWCMD_C2H_FUNC_BCN_CSAZERO, c2h_fwi_bcn_csazero},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_fw_info(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_fw_info_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

static u32 c2h_dump_efuse_hdl(struct mac_ax_adapter *adapter, u8 *buf,
			      u32 len, struct rtw_c2h_info *info)
{
	struct mac_ax_efuse_ofld_info *ofld_info = &adapter->efuse_ofld_info;
	u32 size;

	if (adapter->sm.efuse_ofld != MAC_AX_OFLD_H2C_RCVD) {
		PLTFM_MSG_ERR("[ERR]not cmd sending\n");
		return MACPROCERR;
	}

	size = adapter->hw_info->efuse_size;

	if (!ofld_info->buf) {
		ofld_info->buf = (u8 *)PLTFM_MALLOC(size);
		if (!ofld_info->buf) {
			adapter->sm.efuse = MAC_AX_EFUSE_IDLE;
			return MACBUFALLOC;
		}
	}

	PLTFM_MEMCPY(ofld_info->buf, buf + FWCMD_HDR_LEN, size);

	adapter->sm.efuse_ofld = MAC_AX_OFLD_H2C_DONE;

	return MACSUCCESS;
}

static u32 c2h_read_rsp_hdl(struct mac_ax_adapter *adapter, u8 *buf,
			    u32 len, struct rtw_c2h_info *info)
{
	struct mac_ax_read_ofld_value *value_info = &adapter->read_ofld_value;
	u32 hdr1;
	u16 read_len;
	u8 *read_buff;

	if (value_info->buf)
		PLTFM_FREE(value_info->buf, value_info->len);

	hdr1 = ((struct fwcmd_hdr *)buf)->hdr1;
	hdr1 = le32_to_cpu(hdr1);

	read_len = GET_FIELD(hdr1, C2H_HDR_TOTAL_LEN) - FWCMD_HDR_LEN;

	read_buff = (u8 *)PLTFM_MALLOC(read_len);
	if (!read_buff)
		return MACBUFALLOC;

	PLTFM_MEMCPY(read_buff, buf + FWCMD_HDR_LEN, read_len);

	value_info->len = read_len;
	value_info->buf = read_buff;

	return MACSUCCESS;
}

static u32 c2h_pkt_ofld_rsp_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				u32 len, struct rtw_c2h_info *info)
{
	struct mac_ax_pkt_ofld_info *ofld_info = &adapter->pkt_ofld_info;
	struct mac_ax_pkt_ofld_pkt *ofld_pkt = &adapter->pkt_ofld_pkt;
	struct mac_ax_state_mach *sm = &adapter->sm;
	u32 c2h_content = *(u32 *)(buf + FWCMD_HDR_LEN);
	u16 pkt_len;
	u8 id, pkt_op;
	u8 *pkt_buff;
	u8 *pkt_content;

	c2h_content = le32_to_cpu(c2h_content);

	pkt_op = GET_FIELD(c2h_content, FWCMD_C2H_PKT_OFLD_RSP_PKT_OP);
	pkt_len = GET_FIELD(c2h_content, FWCMD_C2H_PKT_OFLD_RSP_PKT_LENGTH);
	id = GET_FIELD(c2h_content, FWCMD_C2H_PKT_OFLD_RSP_PKT_ID);

	PLTFM_MSG_TRACE("get pktofld rsp. pkt_op: %d, pkt_len: %d, id: %d\n", pkt_op, pkt_len, id);
	if (id == PKT_OFLD_NOT_EXISTS_ID) {
		PLTFM_MSG_ERR("pktofld id %d is for NOT_EXISTS and shouldn't be rpt from fw\n",
			      PKT_OFLD_NOT_EXISTS_ID);
		return MACNOITEM;
	}

	switch (pkt_op) {
	case PKT_OFLD_OP_ADD:
		if (pkt_len != 0) {
			if (!(ofld_info->id_bitmap[id >> 3] & (1 << (id & 7)))) {
				ofld_info->free_id_count--;
				ofld_info->used_id_count++;
			}
			ofld_info->id_bitmap[id >> 3] |= (1 << (id & 7));
		}

		break;

	case PKT_OFLD_OP_DEL:
		if (pkt_len != 0) {
			if (ofld_info->id_bitmap[id >> 3] & (1 << (id & 7))) {
				ofld_info->free_id_count++;
				ofld_info->used_id_count--;
			}
			ofld_info->id_bitmap[id >> 3] &= ~(1 << (id & 7));
		}

		break;

	case PKT_OFLD_OP_READ:
		if (pkt_len != 0) {
			if (ofld_pkt->pkt)
				PLTFM_FREE(ofld_pkt->pkt, ofld_pkt->pkt_len);

			pkt_buff = (u8 *)PLTFM_MALLOC(pkt_len);
			if (!pkt_buff)
				return MACBUFALLOC;

			pkt_content = buf + FWCMD_HDR_LEN;
			pkt_content += sizeof(struct mac_ax_pkt_ofld_hdr);
			PLTFM_MEMCPY(pkt_buff, pkt_content, pkt_len);
			ofld_pkt->pkt_id = id;
			ofld_pkt->pkt_len = pkt_len;
			ofld_pkt->pkt = pkt_buff;
		}
		break;

	default:
		PLTFM_MSG_ERR("[ERR]invalid packet offload op: %d", pkt_op);
		break;
	}
	if (sm->pkt_ofld == MAC_AX_OFLD_H2C_ERROR) {
		if (ofld_info->last_op == PKT_OFLD_OP_READ)
			sm->pkt_ofld = MAC_AX_OFLD_H2C_DONE;
		else
			sm->pkt_ofld = MAC_AX_OFLD_H2C_IDLE;
	}

	return MACSUCCESS;
}

static u32 c2h_beacon_resend_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				 u32 len, struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static u32 c2h_macid_pause_hdl(struct mac_ax_adapter *adapter, u8 *buf,
			       u32 len, struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static u32 c2h_tx_duty_hdl(struct mac_ax_adapter *adapter, u8 *buf,
			   u32 len, struct rtw_c2h_info *info)
{
	u32 content = *(u32 *)(buf + FWCMD_HDR_LEN);
	struct mac_ax_tx_duty_ofld_info ofld_info;

	content = le32_to_cpu(content);

	ofld_info.timer_err =
		GET_FIELD(content, FWCMD_C2H_TX_DUTY_RPT_TIMER_ERR);

	if (ofld_info.timer_err)
		PLTFM_MSG_ERR("[ERR]Tx duty FW timer error\n");

	return MACSUCCESS;
}

static u32 c2h_tsf32_togl_rpt_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				  u32 len, struct rtw_c2h_info *info)
{
	struct fwcmd_tsf32_togl_rpt rpt;
	struct mac_ax_t32_togl_rpt *out_rpt;
	u8 band, port;

	if (!buf) {
		PLTFM_MSG_ERR("[ERR]tsf32 togl rpt no buf\n");
		return MACNPTR;
	}

	rpt.dword0 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN));
	rpt.dword1 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 4));
	rpt.dword2 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 8));

	band = rpt.dword0 & FWCMD_C2H_TSF32_TOGL_RPT_BAND;
	if (band >= MAC_AX_BAND_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid band %d in tsf32 togl rpt\n", band);
		return MACNOITEM;
	}

	port = GET_FIELD(rpt.dword0, FWCMD_C2H_TSF32_TOGL_RPT_PORT);
	if (port >= MAC_AX_PORT_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid port %d in tsf32 togl rpt\n", port);
		return MACNOITEM;
	}

	out_rpt = &adapter->t32_togl_rpt[get_bp_idx(band, port)];
	out_rpt->band = band;
	out_rpt->port = port;
	out_rpt->status = GET_FIELD(rpt.dword0, FWCMD_C2H_TSF32_TOGL_RPT_STATUS);
	out_rpt->early = GET_FIELD(rpt.dword0, FWCMD_C2H_TSF32_TOGL_RPT_EARLY);
	out_rpt->tsf_l = GET_FIELD(rpt.dword1, FWCMD_C2H_TSF32_TOGL_RPT_TSF_L);
	out_rpt->tsf_h = GET_FIELD(rpt.dword2, FWCMD_C2H_TSF32_TOGL_RPT_TSF_H);
	out_rpt->valid = 1;

	return MACSUCCESS;
}

static u32 c2h_cmd_ofld_rsp_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				u32 len, struct rtw_c2h_info *info)
{
	struct fwcmd_cmd_ofld_rsp rsp;
	struct mac_ax_cmd_ofld_info *ofld_info = &adapter->cmd_ofld_info;
	struct mac_ax_state_mach *sm = &adapter->sm;

	if (!buf) {
		PLTFM_MSG_ERR("[ERR]tsf32 togl rpt no buf\n");
		return MACNPTR;
	}

	rsp.dword0 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN));

	ofld_info->result = rsp.dword0 & FWCMD_C2H_CMD_OFLD_RSP_RESULT;
	if (ofld_info->result) {
		PLTFM_MSG_ERR("%s: IO offload fail!!!\n", __func__);
		rsp.dword1 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 4));
		rsp.dword2 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 8));
		rsp.dword3 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 12));
		PLTFM_MSG_ERR("offset = %x\n", rsp.dword1);
		PLTFM_MSG_ERR("expected val = %x\n", rsp.dword2);
		PLTFM_MSG_ERR("read val = %x\n", rsp.dword3);
	}

	if (sm->cmd_state != MAC_AX_CMD_OFLD_SENDING)
		PLTFM_MSG_ERR("%s: IO offload stat err\n", __func__);
	else
		sm->cmd_state = MAC_AX_CMD_OFLD_RCVD;

	return MACSUCCESS;
}

static u32 c2h_scanofld_rsp_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				u32 len, struct rtw_c2h_info *info)
{
#if SCANOFLD_RSP_EVT_PARSE
	return 0;
#else
	struct fwcmd_scanofld_rsp *pkg;
	struct mac_ax_scanofld_rsp rsp;
	struct mac_ax_scanofld_chrpt chrpt_struct;
	u32 chrpt_size_h_dw;
	u32 *chrpt;
	u32 *chrpt_in;
	u32 sh;
	u32 chidx;

	pkg = (struct fwcmd_scanofld_rsp *)(buf + FWCMD_HDR_LEN);
	chrpt_in = (u32 *)(buf + FWCMD_HDR_LEN + sizeof(struct fwcmd_scanofld_rsp));

	chrpt_size_h_dw = sizeof(struct mac_ax_scanofld_chrpt) / sizeof(u32);

	PLTFM_MEMSET(&rsp, 0, sizeof(struct mac_ax_scanofld_rsp));

	pkg->dword0 = le32_to_cpu(pkg->dword0);
	pkg->dword1 = le32_to_cpu(pkg->dword1);
	pkg->dword2 = le32_to_cpu(pkg->dword2);
	pkg->dword3 = le32_to_cpu(pkg->dword3);

	rsp.pri_ch = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_PRI_CH);
	rsp.notify_reason = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_NOTIFY_REASON);
	rsp.status = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_STATUS);
	rsp.ch_band = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_CH_BAND);
	rsp.band = pkg->dword3 & FWCMD_C2H_SCANOFLD_RSP_BAND;
	PLTFM_MSG_TRACE("[scnofld][rsp][%d]: Reason %d, ch %d (band %d), status %d\n",
			rsp.band, rsp.notify_reason, rsp.pri_ch, rsp.ch_band, rsp.status);

	switch (rsp.notify_reason) {
	case MAC_AX_SCAN_END_SCAN_NOTIFY:
		PLTFM_MUTEX_LOCK(&adapter->scanofld_info.fw_chlist_state_lock);
		adapter->scanofld_info.fw_chlist_busy[rsp.band] = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->scanofld_info.fw_chlist_state_lock);
		adapter->scanofld_info.fw_scan_busy[rsp.band] = 0;
		/* fall through */

	case MAC_AX_SCAN_GET_RPT_NOTIFY:
		rsp.scanned_round = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_SCANNED_ROUND);
		rsp.spent_low = pkg->dword1;
		rsp.spent_high = pkg->dword2;
		rsp.air_density = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_AIR_DENSITY);
		rsp.actual_period = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_ACTUAL_PERIOD);
		rsp.tx_fail_cnt = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_TX_FAIL_CNT);
		rsp.num_ch_rpt = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_NUM_CH_RPT);
		rsp.ch_rpt_size = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_CH_RPT_SIZE);

		PLTFM_MSG_TRACE("[scnofld][rsp][%d][end] scan %d rnd in %llu us, last slot %d us\n",
				rsp.band, rsp.scanned_round,
				((u64)rsp.spent_high << 32) + rsp.spent_low, rsp.actual_period);
		PLTFM_MSG_TRACE("[scnofld][rsp][%d][end] airDense %d, txFail %d\n",
				rsp.band, rsp.air_density, rsp.tx_fail_cnt);
		PLTFM_MSG_TRACE("[scnofld][rsp][%d][end] %d ch rpt (size %d)\n",
				rsp.band, rsp.num_ch_rpt, rsp.ch_rpt_size);

		if (!rsp.num_ch_rpt || !rsp.ch_rpt_size)
			break;

		for (chidx = 0; chidx < rsp.num_ch_rpt; chidx++) {
			chrpt = (u32 *)&chrpt_struct;
			for (sh = 0; sh < chrpt_size_h_dw; sh++) {
				*chrpt_in = le32_to_cpu(*chrpt_in);
				PLTFM_MEMCPY(chrpt++, chrpt_in++, sizeof(u32));
			}
			chrpt_in += (rsp.ch_rpt_size - chrpt_size_h_dw);
			PLTFM_MSG_TRACE("[scnofld][rsp][%d][end] ch %d, rx %d, txfail %x, hit %d\n",
					rsp.band, chrpt_struct.pri_ch, chrpt_struct.rx_cnt,
					chrpt_struct.tx_fail, chrpt_struct.parsed);
		}
		break;

	case MAC_AX_SCAN_LEAVE_CH_NOTIFY:
		rsp.actual_period = GET_FIELD(pkg->dword0,
					      FWCMD_C2H_SCANOFLD_RSP_ACTUAL_PERIOD);
		rsp.tx_fail_cnt = GET_FIELD(pkg->dword3,
					    FWCMD_C2H_SCANOFLD_RSP_TX_FAIL_CNT);
		PLTFM_MSG_TRACE("[scnofld][rsp][%d][leave] pd %d, txfail %d\n",
				rsp.band, rsp.actual_period, rsp.tx_fail_cnt);
		break;

	default:
		break;
	}
	return 0;
#endif
}

static u32 c2h_ch_switch_rpt_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				 u32 len, struct rtw_c2h_info *info)
{
	u32 *content = (u32 *)(buf + FWCMD_HDR_LEN);
	u8 *state;
	struct mac_ax_ch_switch_rpt *rpt;

	state = &adapter->sm.ch_switch;
	if (*state != MAC_AX_OFLD_H2C_SENDING && *state != MAC_AX_OFLD_H2C_RCVD)
		return MACPROCERR;

	rpt = &adapter->ch_switch_rpt;
	rpt->result = (u8)GET_FIELD(le32_to_cpu(*content), FWCMD_C2H_CH_SWITCH_RPT_RESULT);
	*state = MAC_AX_CH_SWITCH_GET_RPT;
	return MACSUCCESS;
}

static u32 c2h_bcn_filter_rpt_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				  u32 len, struct rtw_c2h_info *info)
{
	struct fwcmd_bcnfltr_rpt *rpt = (struct fwcmd_bcnfltr_rpt *)(buf + FWCMD_HDR_LEN);
	u32 dword;
	u8 macid, type, rssi_evt, rssi_ma;
	struct mac_ax_bcn_fltr_rpt *adp_rpt;

	dword = le32_to_cpu(rpt->dword0);
	macid = GET_FIELD(dword, FWCMD_C2H_BCNFLTR_RPT_MACID);
	type = GET_FIELD(dword, FWCMD_C2H_BCNFLTR_RPT_TYPE);
	rssi_evt = GET_FIELD(dword, FWCMD_C2H_BCNFLTR_RPT_RSSI_EVT);
	rssi_ma = GET_FIELD(dword, FWCMD_C2H_BCNFLTR_RPT_RSSI_MA);

	adp_rpt = &adapter->bcn_fltr_rpt;
	adp_rpt->macid = macid;
	adp_rpt->type = type;
	switch (type) {
	case BCNFLTR_NOTI_BCN_LOSS:
		PLTFM_MSG_TRACE("[BCNFLTR] bcn loss\n");
		break;
	case BCNFLTR_NOTI_DENY_SCAN:
		PLTFM_MSG_TRACE("[BCNFLTR] deny scan\n");
		break;
	case BCNFLTR_NOTI_RSSI:
		adp_rpt->rssi_evt = rssi_evt;
		adp_rpt->rssi_ma = rssi_ma;
		PLTFM_MSG_TRACE("[BCNFLTR] rssi: ma=%d, evt=%d\n", rssi_ma, rssi_evt);
		break;
	}
	adp_rpt->notified = 1;
	return MACSUCCESS;
}

static u32 c2h_csi_tx_result_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				 u32 len, struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static u32 c2h_bcn_erly_notify(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			       struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_fw_ofld_cmd[] = {
	{FWCMD_C2H_FUNC_EFUSE_DUMP, c2h_dump_efuse_hdl},
	{FWCMD_C2H_FUNC_READ_RSP, c2h_read_rsp_hdl},
	{FWCMD_C2H_FUNC_PKT_OFLD_RSP, c2h_pkt_ofld_rsp_hdl},
	{FWCMD_C2H_FUNC_BEACON_RESEND, c2h_beacon_resend_hdl},
	{FWCMD_C2H_FUNC_MACID_PAUSE, c2h_macid_pause_hdl},
	{FWCMD_C2H_FUNC_TSF32_TOGL_RPT, c2h_tsf32_togl_rpt_hdl},
	{FWCMD_C2H_FUNC_CMD_OFLD_RSP, c2h_cmd_ofld_rsp_hdl},
	{FWCMD_C2H_FUNC_SCANOFLD_RSP, c2h_scanofld_rsp_hdl},
	{FWCMD_C2H_FUNC_TX_DUTY_RPT, c2h_tx_duty_hdl},
	{FWCMD_C2H_FUNC_CH_SWITCH_RPT, c2h_ch_switch_rpt_hdl},
	{FWCMD_C2H_FUNC_BCNFLTR_RPT, c2h_bcn_filter_rpt_hdl},
	{FWCMD_C2H_FUNC_WIFI_SENSING_CSI_TX_RESULT, c2h_csi_tx_result_hdl},
	{FWCMD_C2H_FUNC_BCNERLYNTFY, c2h_bcn_erly_notify},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_fw_ofld(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_fw_ofld_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

static u32 c2h_wait_announ_hdl(struct mac_ax_adapter *adapter, u8 *buf,
			       u32 len, struct rtw_c2h_info *info)
{
	struct fwcmd_wait_announce rpt;

	if (!buf) {
		PLTFM_MSG_ERR("[ERR]wait announce no buf\n");
		return MACNPTR;
	}

	rpt.dword0 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN));

	return MACSUCCESS;
}

static u32 c2h_stat_rpt_hdl(struct mac_ax_adapter *adapter, u8 *buf,
			    u32 len, struct rtw_c2h_info *info)
{
	struct fwcmd_stat_rpt rpt;
	struct mac_ax_twt_info *twt_info = adapter->twt_info;
	u32 i, buff_sh;

	if (!buf || !info->content) {
		PLTFM_MSG_ERR("[ERR]stat rpt no buf\n");
		return MACNPTR;
	}

	if (!twt_info) {
		PLTFM_MSG_ERR("[ERR]no twt info\n");
		return MACNPTR;
	}

	rpt.dword0 = le32_to_cpu(*(u32 *)info->content);

	twt_info->err_rec = GET_FIELD(rpt.dword0, FWCMD_C2H_STAT_RPT_TWT_ERR_REC);
	for (i = 0; i < TWT_DBG_INFO_SIZE; i += 4) {
		buff_sh = 4 + i;
		if (buff_sh >= info->content_len) {
			*(u32 *)(twt_info->pdbg_info + i) = MAC_AX_R32_FF;
		} else {
			*(u32 *)(twt_info->pdbg_info + i) =
				le32_to_cpu(*(u32 *)(info->content + buff_sh));
		}
	}

	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_twt_cmd[] = {
	{FWCMD_C2H_FUNC_WAIT_ANNOUNCE, c2h_wait_announ_hdl},
	{FWCMD_C2H_FUNC_STAT_RPT, c2h_stat_rpt_hdl},
	{FWCMD_C2H_FUNC_NULL, NULL}
};

u32 c2h_twt(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
	    struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_twt_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 c2h_wow_aoac_report_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			    struct rtw_c2h_info *info)
{
	struct mac_ax_wowlan_info *wowlan_info = &adapter->wowlan_info;
	u8 *c2h_content = buf + FWCMD_HDR_LEN;

	if (adapter->sm.aoac_rpt != MAC_AX_AOAC_RPT_H2C_RCVD)
		return MACPROCERR;

	if (!wowlan_info->aoac_report)
		return MACBUFALLOC;

	PLTFM_MEMCPY(wowlan_info->aoac_report, c2h_content, sizeof(struct mac_ax_aoac_report));

	adapter->sm.aoac_rpt = MAC_AX_AOAC_RPT_H2C_DONE;

	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_wow_cmd[] = {
	{FWCMD_C2H_FUNC_AOAC_REPORT, c2h_wow_aoac_report_hdl},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_wow(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
	    struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_wow_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 c2h_mcc_rcv_ack_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			struct rtw_c2h_info *info)
{
	struct mac_ax_state_mach *sm = &adapter->sm;
	u32 c2h_content = *(u32 *)(buf + FWCMD_HDR_LEN);
	u8 group, h2c_func;

	c2h_content = le32_to_cpu(c2h_content);
	group = GET_FIELD(c2h_content, FWCMD_C2H_MCC_RCV_ACK_GROUP);
	h2c_func = GET_FIELD(c2h_content, FWCMD_C2H_MCC_RCV_ACK_H2C_FUNC);

	if (h2c_func <= FWCMD_H2C_FUNC_RESET_MCC_GROUP) {
		PLTFM_MSG_TRACE("[TRACE]%s: MCC group H2C rcv ack\n",
				__func__);

		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_SENT) {
			sm->mcc_group[group] = MAC_AX_MCC_STATE_H2C_RCVD;

			PLTFM_MSG_TRACE("[TRACE]%s: MCC group %d state: %d\n",
					__func__, group,
					MAC_AX_MCC_STATE_H2C_RCVD);
		}
	} else if (h2c_func <= FWCMD_H2C_FUNC_MCC_SET_DURATION) {
		PLTFM_MSG_TRACE("[TRACE]%s: MCC request H2C rcv ack\n",
				__func__);

		if (sm->mcc_request[group] == MAC_AX_MCC_REQ_H2C_SENT) {
			sm->mcc_request[group] = MAC_AX_MCC_REQ_H2C_RCVD;

			PLTFM_MSG_TRACE("[TRACE]%s: MCC group %d state: %d\n",
					__func__, group,
					MAC_AX_MCC_REQ_H2C_RCVD);
		}
	} else {
		PLTFM_MSG_ERR("[ERR]%s: invalid MCC H2C func %d\n",
			      __func__, h2c_func);
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 c2h_mcc_req_ack_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			struct rtw_c2h_info *info)
{
	struct mac_ax_state_mach *sm = &adapter->sm;
	u32 c2h_content = *(u32 *)(buf + FWCMD_HDR_LEN);
	u8 group, h2c_func, h2c_return;

	c2h_content = le32_to_cpu(c2h_content);
	group = GET_FIELD(c2h_content, FWCMD_C2H_MCC_REQ_ACK_GROUP);
	h2c_func = GET_FIELD(c2h_content, FWCMD_C2H_MCC_REQ_ACK_H2C_FUNC);
	h2c_return = GET_FIELD(c2h_content, FWCMD_C2H_MCC_REQ_ACK_H2C_RETURN);

	PLTFM_MSG_TRACE("[TRACE]%s: group: %d, h2c_func: %d, h2c_return: %d\n",
			__func__, group, h2c_func, h2c_return);

	if (h2c_func < FWCMD_H2C_FUNC_MCC_REQ_TSF) {
		PLTFM_MSG_ERR("[ERR]%s: invalid MCC H2C func: %d\n",
			      __func__, h2c_func);
		return MACNOITEM;
	}

	sm->mcc_request_state[group] = h2c_return;

	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr req state: %d\n",
			__func__, group, sm->mcc_request[group]);

	if (sm->mcc_request[group] == MAC_AX_MCC_REQ_H2C_RCVD) {
		if (h2c_return == 0) {
			if (h2c_func == FWCMD_H2C_FUNC_MCC_REQ_TSF)
				sm->mcc_request[group] = MAC_AX_MCC_REQ_DONE;
			else
				sm->mcc_request[group] = MAC_AX_MCC_REQ_IDLE;
		} else {
			sm->mcc_request[group] = MAC_AX_MCC_REQ_FAIL;
			PLTFM_MSG_ERR("[ERR]%s: MCC H2C func %d fail: %d\n",
				      __func__, h2c_func, h2c_return);
		}
	}

	return MACSUCCESS;
}

u32 c2h_mcc_tsf_rpt_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			struct rtw_c2h_info *info)
{
	struct mac_ax_mcc_group_info *mcc_info = &adapter->mcc_group_info;
	struct fwcmd_mcc_tsf_rpt *tsf_rpt;
	u32 c2h_content;
	u32 tsf;
	u8 macid_x, macid_y, group;

	PLTFM_MSG_TRACE("[TRACE]%s: mcc tsf report received\n", __func__);

	tsf_rpt = (struct fwcmd_mcc_tsf_rpt *)(buf + FWCMD_HDR_LEN);

	c2h_content = tsf_rpt->dword0;
	c2h_content = le32_to_cpu(c2h_content);
	group = GET_FIELD(c2h_content, FWCMD_C2H_MCC_TSF_RPT_GROUP);
	macid_x = GET_FIELD(c2h_content, FWCMD_C2H_MCC_TSF_RPT_MACID_X);
	macid_y = GET_FIELD(c2h_content, FWCMD_C2H_MCC_TSF_RPT_MACID_Y);

	PLTFM_MSG_TRACE("[TRACE]%s: group: %d, macid_x: %d, macid_y: %d\n",
			__func__, group, macid_x, macid_y);

	mcc_info->groups[group].macid_x = macid_x;
	mcc_info->groups[group].macid_y = macid_y;

	tsf = tsf_rpt->dword1;
	tsf = le32_to_cpu(tsf);
	mcc_info->groups[group].tsf_x_low = tsf;

	tsf = tsf_rpt->dword2;
	tsf = le32_to_cpu(tsf);
	mcc_info->groups[group].tsf_x_high = tsf;

	tsf = tsf_rpt->dword3;
	tsf = le32_to_cpu(tsf);
	mcc_info->groups[group].tsf_y_low = tsf;

	tsf = tsf_rpt->dword4;
	tsf = le32_to_cpu(tsf);
	mcc_info->groups[group].tsf_y_high = tsf;

	PLTFM_MSG_TRACE("[TRACE]%s: tsf_x_high: 0x%x, tsf_x_low: 0x%x\n",
			__func__, mcc_info->groups[group].tsf_x_high,
			mcc_info->groups[group].tsf_x_low);

	PLTFM_MSG_TRACE("[TRACE]%s: tsf_y_high: 0x%x, tsf_y_low: 0x%x\n",
			__func__, mcc_info->groups[group].tsf_y_high,
			mcc_info->groups[group].tsf_y_low);

	return MACSUCCESS;
}

u32 c2h_mcc_status_rpt_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			   struct rtw_c2h_info *info)
{
	struct mac_ax_mcc_group_info *mcc_info = &adapter->mcc_group_info;
	struct mac_ax_state_mach *sm = &adapter->sm;
	struct fwcmd_mcc_status_rpt *mcc_rpt;
	u32 c2h_content;
	u32 tsf_low;
	u32 tsf_high;
	u8 group, status, macid;

	PLTFM_MSG_TRACE("[TRACE]%s: mcc status report received\n", __func__);

	mcc_rpt = (struct fwcmd_mcc_status_rpt *)(buf + FWCMD_HDR_LEN);

	c2h_content = mcc_rpt->dword0;
	tsf_low = mcc_rpt->dword1;
	tsf_high = mcc_rpt->dword2;

	c2h_content = le32_to_cpu(c2h_content);
	group = GET_FIELD(c2h_content, FWCMD_C2H_MCC_STATUS_RPT_GROUP);
	macid = GET_FIELD(c2h_content, FWCMD_C2H_MCC_STATUS_RPT_MACID);
	status = GET_FIELD(c2h_content, FWCMD_C2H_MCC_STATUS_RPT_STATUS);

	PLTFM_MSG_TRACE("[TRACE]%s: mcc group: %d, macid: %d, status: %d\n",
			__func__, group, macid, status);

	sm->mcc_group_state[group] = status;

	switch (status) {
	case MAC_AX_MCC_ADD_ROLE_OK:
		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_RCVD) {
			sm->mcc_group[group] = MAC_AX_MCC_ADD_DONE;
			PLTFM_MSG_TRACE("[TRACE]%s: mcc group %d add done\n",
					__func__, group);
		}
		break;

	case MAC_AX_MCC_START_GROUP_OK:
		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_RCVD) {
			sm->mcc_group[group] = MAC_AX_MCC_START_DONE;
			PLTFM_MSG_TRACE("[TRACE]%s: mcc group %d start done\n",
					__func__, group);
		}
		break;

	case MAC_AX_MCC_STOP_GROUP_OK:
		sm->mcc_group[group] = MAC_AX_MCC_STOP_DONE;
		PLTFM_MSG_TRACE("[TRACE]%s: mcc group %d stop done\n",
				__func__, group);
		break;

	case MAC_AX_MCC_DEL_GROUP_OK:
		sm->mcc_group[group] = MAC_AX_MCC_EMPTY;
		PLTFM_MSG_TRACE("[TRACE]%s: mcc group %d empty\n",
				__func__, group);
		break;

	case MAC_AX_MCC_RESET_GROUP_OK:
		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_RCVD) {
			sm->mcc_group[group] = MAC_AX_MCC_EMPTY;
			PLTFM_MSG_TRACE("[TRACE]%s: mcc group %d empty\n",
					__func__, group);
		}
		break;

	case MAC_AX_MCC_EMPTY_GRP_FAIL:
	case MAC_AX_MCC_ROLE_NOT_EXIST_FAIL:
	case MAC_AX_MCC_DATA_NOT_FOUND_FAIL:
	case MAC_AX_MCC_ACT_INVALID_FAIL:
	case MAC_AX_MCC_BANDTYPE_INVALID_FAIL:
	case MAC_AX_MCC_ADD_PSTIMER_FAIL:
	case MAC_AX_MCC_MALLOC_FAIL:
	case MAC_AX_MCC_SWITCH_CH_FAIL:
	case MAC_AX_MCC_TXNULL0_FAIL:
		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_RCVD) {
			PLTFM_MSG_ERR("[ERR]%s: mcc group %d fail status: %d\n",
				      __func__, group, status);
			sm->mcc_group[group] = MAC_AX_MCC_STATE_ERROR;
		}
		break;

	default:
		break;
	}

	tsf_low = le32_to_cpu(tsf_low);
	tsf_high = le32_to_cpu(tsf_high);

	mcc_info->groups[group].rpt_status = status;
	mcc_info->groups[group].rpt_macid = macid;
	mcc_info->groups[group].rpt_tsf_low = tsf_low;
	mcc_info->groups[group].rpt_tsf_high = tsf_high;

	PLTFM_MSG_TRACE("[TRACE]%s: tsf_high: 0x%x, tsf_low: 0x%x\n",
			__func__, tsf_high, tsf_low);

	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_mcc_cmd[] = {
	{FWCMD_C2H_FUNC_MCC_RCV_ACK, c2h_mcc_rcv_ack_hdl},
	{FWCMD_C2H_FUNC_MCC_REQ_ACK, c2h_mcc_req_ack_hdl},
	{FWCMD_C2H_FUNC_MCC_TSF_RPT, c2h_mcc_tsf_rpt_hdl},
	{FWCMD_C2H_FUNC_MCC_STATUS_RPT, c2h_mcc_status_rpt_hdl},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_mcc(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
	    struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_mcc_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	PLTFM_MSG_TRACE("[TRACE]%s: func: %d\n", __func__, func);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]%s: null func handler id: %X",
			      __func__, func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 c2h_rx_dbg_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		   struct rtw_c2h_info *info)
{
	PLTFM_MSG_ERR("[ERR]%s: FW encounter Rx problem!\n", __func__);

	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_fw_dbg_cmd[] = {
	{FWCMD_C2H_FUNC_RX_DBG, c2h_rx_dbg_hdl},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_fw_dbg(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
	       struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_fw_dbg_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	PLTFM_MSG_TRACE("[TRACE]%s: func: %d\n", __func__, func);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]%s: null func handler id: %X",
			      __func__, func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 c2h_wps_rpt(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		struct rtw_c2h_info *info)
{
	PLTFM_MSG_TRACE("recevied wps report\n");
	return MACSUCCESS;
}

static u32 c2h_misc_ccxrpt(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			   struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_misc[] = {
	{FWCMD_C2H_FUNC_WPS_RPT, c2h_wps_rpt},
	{FWCMD_C2H_FUNC_CCXRPT, c2h_misc_ccxrpt},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

static u32 c2h_cl_misc(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_misc;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 c2h_fast_ch_sw_rpt_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			   struct rtw_c2h_info *info)
{
	u32 *c2h_content;
	u32 *rpt_status;

	PLTFM_MSG_TRACE("[HM][C2H][FCS] get rpt func\n");
	adapter->fast_ch_sw_info.busy = 0;
	c2h_content = (u32 *)(buf + FWCMD_HDR_LEN);
	rpt_status = &adapter->fast_ch_sw_info.status;
	PLTFM_MEMCPY(rpt_status, c2h_content, sizeof(u32));
	PLTFM_MSG_TRACE("[HM][C2H][FCS] Report Status: 0x%x\n",  adapter->fast_ch_sw_info.status);

	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_fast_ch_sw_cmd[] = {
	{FWCMD_C2H_FUNC_FCS_RPT, c2h_fast_ch_sw_rpt_hdl},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_fast_ch_sw(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		   struct rtw_c2h_info *info)
{
	u32 hdr0;
	u32 func;
	u32 (*handler)(struct mac_ax_adapter *adpater, u8 *buf,
		       u32 len, struct rtw_c2h_info *info);
	struct c2h_proc_func *proc;

	proc = c2h_proc_fast_ch_sw_cmd;
	handler = NULL;
	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR][%s]: sent id = %x", __func__, func);
		return MACNOITEM;
	}

	return  handler(adapter, buf, len, info);
}

u32 c2h_port_init_stat(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info)
{
	struct fwcmd_port_init_stat stat;
	struct mac_ax_port_info *pinfo;
	u8 band, port;
	u32 ret, tmp32;

	stat.dword0 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN));
	stat.dword1 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 4));
	stat.dword2 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 8));

	band = stat.dword0 & FWCMD_C2H_PORT_INIT_STAT_BAND;
	if (band >= MAC_AX_BAND_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid band %d in port init stat\n", band);
		return MACNOITEM;
	}

	port = GET_FIELD(stat.dword0, FWCMD_C2H_PORT_INIT_STAT_PORT);
	if (port >= MAC_AX_PORT_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid port %d in port init stat\n", port);
		return MACNOITEM;
	}

	pinfo = &adapter->port_info[get_bp_idx(band, port)];

	ret = GET_FIELD(stat.dword2, FWCMD_C2H_PORT_INIT_STAT_RET);
	if (ret != C2H_MPORT_RET_SUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%d init fail: ret %d\n",
			      band, port, ret);
		tmp32 = GET_FIELD(stat.dword0, FWCMD_C2H_PORT_INIT_STAT_STEP);
		PLTFM_MSG_ERR("[ERR]B%dP%d init fail: step %d\n",
			      band, port, tmp32);
		tmp32 = GET_FIELD(stat.dword0, FWCMD_C2H_PORT_INIT_STAT_CFG_MBID_IDX);
		PLTFM_MSG_ERR("[ERR]B%dP%d init fail: cfg mbid %d\n",
			      band, port, tmp32);
		tmp32 = GET_FIELD(stat.dword0, FWCMD_C2H_PORT_INIT_STAT_CFG_TYPE);
		PLTFM_MSG_ERR("[ERR]B%dP%d init fail: cfg type %d\n",
			      band, port, tmp32);
		tmp32 = GET_FIELD(stat.dword1, FWCMD_C2H_PORT_INIT_STAT_CFG_VAL);
		PLTFM_MSG_ERR("[ERR]B%dP%d init fail: cfg val %d\n",
			      band, port, tmp32);
		pinfo->h2c_sm = MAC_AX_PORT_H2C_FAIL;
	} else {
		pinfo->h2c_sm = MAC_AX_PORT_H2C_IDLE;
	}

	return MACSUCCESS;
}

u32 c2h_port_cfg_stat(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		      struct rtw_c2h_info *info)
{
	struct fwcmd_port_cfg_stat stat;
	struct mac_ax_port_info *pinfo;
	u8 band, port, mbssid;
	u32 ret, tmp32;

	stat.dword0 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN));
	stat.dword1 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 4));
	stat.dword2 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 8));

	band = stat.dword0 & FWCMD_C2H_PORT_CFG_STAT_BAND;
	if (band >= MAC_AX_BAND_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid band %d in port cfg stat\n", band);
		return MACNOITEM;
	}

	port = GET_FIELD(stat.dword0, FWCMD_C2H_PORT_CFG_STAT_PORT);
	if (port >= MAC_AX_PORT_NUM) {
		PLTFM_MSG_ERR("[ERR]invalid port %d in port cfg stat\n", port);
		return MACNOITEM;
	}

	pinfo = &adapter->port_info[get_bp_idx(band, port)];
	mbssid = GET_FIELD(stat.dword0, FWCMD_C2H_PORT_CFG_STAT_MBSSID_IDX);

	ret = GET_FIELD(stat.dword2, FWCMD_C2H_PORT_CFG_STAT_RET);
	if (ret != C2H_MPORT_RET_SUCCESS) {
		PLTFM_MSG_ERR("[ERR]B%dP%dMB%d cfg fail: ret %d\n",
			      band, port, mbssid, ret);
		tmp32 = GET_FIELD(stat.dword0, FWCMD_C2H_PORT_CFG_STAT_TYPE);
		PLTFM_MSG_ERR("[ERR]B%dP%dMB%d cfg fail: type %d\n",
			      band, port, mbssid, tmp32);
		tmp32 = GET_FIELD(stat.dword1, FWCMD_C2H_PORT_CFG_STAT_VAL);
		PLTFM_MSG_ERR("[ERR]B%dP%dMB%d cfg fail: val %d\n",
			      band, port, mbssid, tmp32);
		pinfo->h2c_sm = MAC_AX_PORT_H2C_FAIL;
	} else {
		pinfo->h2c_sm = MAC_AX_PORT_H2C_IDLE;
	}

	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_mport[] = {
	{FWCMD_C2H_FUNC_PORT_INIT_STAT, c2h_port_init_stat},
	{FWCMD_C2H_FUNC_PORT_CFG_STAT, c2h_port_cfg_stat},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

static u32 c2h_cl_mport(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_mport;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 c2h_nan_act_req_ack_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			    struct rtw_c2h_info *info)
{
	struct fwcmd_act_schedule_req_ack act_schedule;
	struct mac_ax_act_ack_info *ack_info;

	if (!buf) {
		PLTFM_MSG_ERR("[ERR]nan act schedule req ack no buf\n");
		return MACNPTR;
	}

	act_schedule.dword0 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN));
	act_schedule.dword1 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 4));
	ack_info = &adapter->nan_info.nan_act_ack_info;
	ack_info->schedule_id = GET_FIELD(act_schedule.dword1,
					  FWCMD_C2H_ACT_SCHEDULE_REQ_ACK_SCHEDULE_ID);
	PLTFM_MSG_TRACE("[test]nan act schedule req ack moduleid %d\n", ack_info->schedule_id);
	return MACSUCCESS;
}

u32 c2h_nan_cluster_info_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			     struct rtw_c2h_info *info)
{
	struct fwcmd_nan_info_notify_cluster_info cluster_info;
	struct mac_ax_nan_info *nan_info;

	if (!buf) {
		PLTFM_MSG_ERR("[ERR]nan cluster info ack no buf\n");
		return MACNPTR;
	}

	cluster_info.dword0 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN));
	cluster_info.dword1 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 4));
	cluster_info.dword2 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 8));
	cluster_info.dword3 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 12));
	cluster_info.dword4 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 16));
	cluster_info.dword5 = le32_to_cpu(*(u32 *)(buf + FWCMD_HDR_LEN + 20));

	nan_info = &adapter->nan_info;

	nan_info->rpt_cluster_id[0] = GET_FIELD(cluster_info.dword1,
						FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_CLUSTER_ID0);
	nan_info->rpt_cluster_id[1] = GET_FIELD(cluster_info.dword1,
						FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_CLUSTER_ID1);
	nan_info->rpt_cluster_id[2] = GET_FIELD(cluster_info.dword1,
						FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_CLUSTER_ID2);
	nan_info->rpt_cluster_id[3] = GET_FIELD(cluster_info.dword1,
						FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_CLUSTER_ID3);
	nan_info->rpt_cluster_id[4] = GET_FIELD(cluster_info.dword2,
						FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_CLUSTER_ID4);
	nan_info->rpt_cluster_id[5] = GET_FIELD(cluster_info.dword2,
						FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_CLUSTER_ID5);
	nan_info->rpt_master_pref = GET_FIELD(cluster_info.dword2,
					      FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_MASTERPREF);
	nan_info->rpt_random_factor =
		GET_FIELD(cluster_info.dword2, FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_RANDOMFACTOR);
	nan_info->rpt_amr = GET_FIELD(cluster_info.dword3,
				      FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_AMR);
	nan_info->rpt_ambtt = GET_FIELD(cluster_info.dword4,
					FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_AMBTT);
	nan_info->rpt_hop_count = GET_FIELD(cluster_info.dword5,
					    FWCMD_C2H_NAN_INFO_NOTIFY_CLUSTER_INFO_HOPCOUNT);

	PLTFM_MSG_TRACE("[test]nan rpt_ambtt %d\n", nan_info->rpt_ambtt);
	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_nan_cmd[] = {
	{FWCMD_C2H_FUNC_ACT_SCHEDULE_REQ_ACK, c2h_nan_act_req_ack_hdl},
	{FWCMD_C2H_FUNC_BCN_REQ_ACK, c2h_nan_act_req_ack_hdl},
	{FWCMD_C2H_FUNC_NAN_FUNC_CTRL_ACK, c2h_nan_act_req_ack_hdl},
	{FWCMD_C2H_FUNC_NAN_DE_INFO_ACK, c2h_nan_act_req_ack_hdl},
	{FWCMD_C2H_FUNC_NAN_JOIN_CLUSTER_ACK, c2h_nan_act_req_ack_hdl},
	{FWCMD_C2H_FUNC_NAN_PAUSE_FAW_TX_ACK, c2h_nan_act_req_ack_hdl},
	{FWCMD_C2H_FUNC_NAN_INFO_NOTIFY_CLUSTER_INFO, c2h_nan_cluster_info_hdl},
	//{FWCMD_C2H_FUNC_NAN_INFO_NOTIFY_CLUSTER_JOIN, c2h_nan_join_info_hdl},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_nan(struct mac_ax_adapter *adapter, u8 *buf, u32 len, struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_nan_cmd;
	u32(*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		      struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	PLTFM_MSG_TRACE("[TRACE]%s: func: %d\n", __func__, func);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]%s: null func handler id: %X", __func__, func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

static struct c2h_proc_class c2h_proc_sys[] = {
#if MAC_AX_FEATURE_DBGPKG
	{FWCMD_C2H_CL_CMD_PATH, c2h_sys_cmd_path},
	{FWCMD_H2C_CL_PLAT_AUTO_TEST, c2h_sys_plat_autotest},
#if MAC_AX_FEATURE_HV
	{FWCMD_C2H_CL_FW_AUTO, c2h_sys_fw_autotest},
#endif
#endif
	{FWCMD_C2H_CL_FW_STATUS, c2h_fw_status},
	{FWCMD_C2H_CL_NULL, NULL},
};

static struct c2h_proc_class c2h_proc_mac[] = {
	{FWCMD_C2H_CL_FW_INFO, c2h_fw_info},
	{FWCMD_C2H_CL_FW_OFLD, c2h_fw_ofld},
	{FWCMD_C2H_CL_TWT, c2h_twt},
	{FWCMD_C2H_CL_WOW, c2h_wow},
	{FWCMD_C2H_CL_MCC, c2h_mcc},
	{FWCMD_C2H_CL_FW_DBG, c2h_fw_dbg},
	{FWCMD_C2H_CL_FLASH, c2h_sys_flash_pkt},
	{FWCMD_C2H_CL_MISC, c2h_cl_misc},
	{FWCMD_C2H_CL_FCS, c2h_fast_ch_sw},
	{FWCMD_C2H_CL_MPORT, c2h_cl_mport},
	{FWCMD_C2H_CL_NAN, c2h_nan},
	{FWCMD_C2H_CL_NULL, NULL},
};

static inline struct c2h_proc_class *c2h_proc_sel(u8 cat)
{
	struct c2h_proc_class *proc;

	switch (cat) {
	case FWCMD_C2H_CAT_TEST:
		proc = c2h_proc_sys;
		break;
	case FWCMD_C2H_CAT_MAC:
		proc = c2h_proc_mac;
		break;
	default:
		proc = NULL;
		break;
	}

	return proc;
}

u8 c2h_field_parsing(struct fwcmd_hdr *hdr, struct rtw_c2h_info *info)
{
	u32 val;

	val = le32_to_cpu(hdr->hdr0);
	info->c2h_cat = GET_FIELD(val, C2H_HDR_CAT);
	info->c2h_class = GET_FIELD(val, C2H_HDR_CLASS);
	info->c2h_func = GET_FIELD(val, C2H_HDR_FUNC);

	val = le32_to_cpu(hdr->hdr1);
	info->content_len = GET_FIELD(val, C2H_HDR_TOTAL_LEN) -
				FWCMD_HDR_LEN;
	info->content = (u8 *)(hdr + 1);

	return MACSUCCESS;
}

u32 mac_process_c2h(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		    u8 *ret)
{
	u8 _class_, result;
	struct c2h_proc_class *proc;
	struct fwcmd_hdr *hdr;
	struct rtw_c2h_info *info;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u8 cat;
	u32 val;

	hdr = (struct fwcmd_hdr *)buf;

	info = (struct rtw_c2h_info *)ret;
	val = le32_to_cpu(hdr->hdr0);

	result = c2h_field_parsing(hdr, info);
	if (result) {
		PLTFM_MSG_ERR("[ERR]parsing c2h hdr error: %X\n", val);
		return MACNOITEM;
	}

	if (GET_FIELD(val, C2H_HDR_DEL_TYPE) != FWCMD_TYPE_C2H) {
		PLTFM_MSG_ERR("[ERR]wrong fwcmd type: %X\n", val);
		return MACNOITEM;
	}

	cat = (u8)GET_FIELD(val, C2H_HDR_CAT);

	if (cat == FWCMD_C2H_CAT_OUTSRC)
		return MACSUCCESS;

	proc = c2h_proc_sel(cat);
	if (!proc) {
		PLTFM_MSG_ERR("[ERR]wrong fwcmd cat: %X\n", val);
		return MACNOITEM;
	}

	_class_ = GET_FIELD(val, C2H_HDR_CLASS);

	for (; proc->id != FWCMD_C2H_CL_NULL; proc++) {
		if (_class_ == proc->id) {
			handler = proc->handler;
			break;
		}
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null class handler id: %X", proc->id);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 mac_outsrc_h2c_common(struct mac_ax_adapter *adapter,
			  struct rtw_g6_h2c_hdr *hdr, u32 *pvalue)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_outsrc_info *info;

	/*temp workaround for h2cb no functionality and outsrc has its timer*/
	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_ERR("FW is not ready\n");
		return MACFWNONRDY;
	}

	h2cb = h2cb_alloc(adapter, (enum h2c_buf_class)hdr->type);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, hdr->content_len);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}
	info = (struct fwcmd_outsrc_info *)buf;
	PLTFM_MEMCPY(info->dword0, pvalue, hdr->content_len);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_OUTSRC,
			      hdr->h2c_class,
			      hdr->h2c_func,
			      hdr->rec_ack,
			      hdr->done_ack);
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
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_fw_log_cfg(struct mac_ax_adapter *adapter,
		   struct mac_ax_fw_log *log_cfg)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_log_cfg *log;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_log_cfg));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}
	PLTFM_MEMSET(buf, 0, sizeof(struct fwcmd_log_cfg));

	log = (struct fwcmd_log_cfg *)buf;

	log->dword0 = cpu_to_le32(SET_WORD(log_cfg->level,
					   FWCMD_H2C_LOG_CFG_DBG_LV) |
				  SET_WORD(log_cfg->output,
					   FWCMD_H2C_LOG_CFG_PATH));
	log->dword1 = cpu_to_le32(log_cfg->comp);

	log->dword2 = cpu_to_le32(log_cfg->comp_ext);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_INFO,
			      FWCMD_H2C_FUNC_LOG_CFG,
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

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_send_bcn_h2c(struct mac_ax_adapter *adapter,
		     struct mac_ax_bcn_info *info)
{
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	struct fwcmd_bcn_upd *hdr;
	u32 ret = MACSUCCESS;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_bcn_upd *)
		h2cb_put(h2cb, sizeof(struct fwcmd_bcn_upd));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	info->grp_ie_ofst |= info->grp_ie_ofst ? BCN_GRPIE_OFST_EN : 0;

	hdr->dword0 =
		cpu_to_le32(SET_WORD(info->port,
				     FWCMD_H2C_BCN_UPD_PORT) |
			    SET_WORD(info->mbssid,
				     FWCMD_H2C_BCN_UPD_MBSSID) |
			    SET_WORD(info->band,
				     FWCMD_H2C_BCN_UPD_BAND) |
			    SET_WORD(info->grp_ie_ofst,
				     FWCMD_H2C_BCN_UPD_GRP_IE_OFST));

	hdr->dword1 =
		cpu_to_le32(SET_WORD(info->macid,
				     FWCMD_H2C_BCN_UPD_MACID) |
			    SET_WORD(info->ssn_sel,
				     FWCMD_H2C_BCN_UPD_SSN_SEL) |
			    SET_WORD(info->ssn_mode,
				     FWCMD_H2C_BCN_UPD_SSN_MODE) |
			    SET_WORD(info->rate_sel,
				     FWCMD_H2C_BCN_UPD_RATE) |
			    SET_WORD(info->txpwr,
				     FWCMD_H2C_BCN_UPD_TXPWR));

	hdr->dword2 =
		cpu_to_le32((info->txinfo_ctrl_en ?
			     FWCMD_H2C_BCN_UPD_TXINFO_CTRL_EN : 0) |
			    SET_WORD(info->ntx_path_en,
				     FWCMD_H2C_BCN_UPD_NTX_PATH_EN) |
			    SET_WORD(info->path_map_a,
				     FWCMD_H2C_BCN_UPD_PATH_MAP_A) |
			    SET_WORD(info->path_map_b,
				     FWCMD_H2C_BCN_UPD_PATH_MAP_B) |
			    SET_WORD(info->path_map_c,
				     FWCMD_H2C_BCN_UPD_PATH_MAP_C) |
			    SET_WORD(info->path_map_d,
				     FWCMD_H2C_BCN_UPD_PATH_MAP_D) |
			    (info->antsel_a ?
			     FWCMD_H2C_BCN_UPD_ANTSEL_A : 0) |
			    (info->antsel_b ?
			     FWCMD_H2C_BCN_UPD_ANTSEL_B : 0) |
			    (info->antsel_c ?
			     FWCMD_H2C_BCN_UPD_ANTSEL_C : 0) |
			    (info->antsel_d ?
			     FWCMD_H2C_BCN_UPD_ANTSEL_D : 0) |
			     SET_WORD(info->csa_ofst,
				      FWCMD_H2C_BCN_UPD_CSA_OFST));

	buf = h2cb_put(h2cb, info->pld_len);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMCPY(buf, info->pld_buf, info->pld_len);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_BCN_UPD,
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

u32 mac_host_getpkt_h2c(struct mac_ax_adapter *adapter, u8 macid, u8 pkttype)
{
	struct mac_ax_h2creg_info content = {0};
	u32 ret;

	content.id = FWCMD_H2CREG_FUNC_GETPKT_INFORM;
	content.content_len = 4;
	content.h2c_content.dword0 =
				SET_WORD(macid,
					 FWCMD_H2CREG_GETPKT_INFORM_MACID) |
				SET_WORD(pkttype,
					 FWCMD_H2CREG_GETPKT_INFORM_PKTTYPE);

	ret = proc_msg_reg(adapter, &content, NULL);

	return ret;
}

#if MAC_AX_PHL_H2C
u32 __ie_cam_set_cmd(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		     struct mac_ax_ie_cam_cmd_info *info)
{
	struct fwcmd_ie_cam *cmd;
	u8 *buf;
	u32 ret = MACSUCCESS;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ie_cam));
	if (!buf) {
		ret = MACNOBUF;
		return ret;
	}

	cmd = (struct fwcmd_ie_cam *)buf;
	cmd->dword0 =
		cpu_to_le32((info->en ? FWCMD_H2C_IE_CAM_CAM_EN : 0) |
			    (info->band ? FWCMD_H2C_IE_CAM_BAND : 0) |
			    (info->hit_en ? FWCMD_H2C_IE_CAM_HIT_FRWD_EN : 0) |
			    (info->miss_en ?
			     FWCMD_H2C_IE_CAM_MISS_FRWD_EN : 0) |
			    (info->rst ? FWCMD_H2C_IE_CAM_RST : 0) |
			    SET_WORD(info->port, FWCMD_H2C_IE_CAM_PORT) |
			    SET_WORD(info->hit_sel, FWCMD_H2C_IE_CAM_HIT_FRWD) |
			    SET_WORD(info->miss_sel,
				     FWCMD_H2C_IE_CAM_MISS_FRWD) |
			    SET_WORD(info->num, FWCMD_H2C_IE_CAM_UPD_NUM));

	buf = h2cb_put(h2cb, info->buf_len);
	if (!buf) {
		ret = MACNOBUF;
		return ret;
	}

	PLTFM_MEMCPY(buf, info->buf, info->buf_len);

	return ret;
}

#else
u32 __ie_cam_set_cmd(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb,
		     struct mac_ax_ie_cam_cmd_info *info)
{
	struct fwcmd_ie_cam *cmd;
	u8 *buf;
	u32 ret = MACSUCCESS;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ie_cam));
	if (!buf) {
		ret = MACNOBUF;
		return ret;
	}

	cmd = (struct fwcmd_ie_cam *)buf;
	cmd->dword0 =
		cpu_to_le32((info->en ? FWCMD_H2C_IE_CAM_CAM_EN : 0) |
			    (info->band ? FWCMD_H2C_IE_CAM_BAND : 0) |
			    (info->hit_en ? FWCMD_H2C_IE_CAM_HIT_FRWD_EN : 0) |
			    (info->miss_en ?
			     FWCMD_H2C_IE_CAM_MISS_FRWD_EN : 0) |
			    (info->rst ? FWCMD_H2C_IE_CAM_RST : 0) |
			    SET_WORD(info->port, FWCMD_H2C_IE_CAM_PORT) |
			    SET_WORD(info->hit_sel, FWCMD_H2C_IE_CAM_HIT_FRWD) |
			    SET_WORD(info->miss_sel,
				     FWCMD_H2C_IE_CAM_MISS_FRWD) |
			    SET_WORD(info->num, FWCMD_H2C_IE_CAM_UPD_NUM));

	buf = h2cb_put(h2cb, info->buf_len);
	if (!buf) {
		ret = MACNOBUF;
		return ret;
	}

	PLTFM_MEMCPY(buf, info->buf, info->buf_len);

	return ret;
}
#endif
u32 mac_ie_cam_upd(struct mac_ax_adapter *adapter,
		   struct mac_ax_ie_cam_cmd_info *info)
{
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u32 ret;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	ret = __ie_cam_set_cmd(adapter, h2cb, info);
	if (ret) {
		PLTFM_MSG_ERR("H2C IE CAM set cmd fail %d\n", ret);
		goto fail;
	}

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_IE_CAM,
			      FWCMD_H2C_FUNC_IE_CAM, 0, 1);
	if (ret) {
		PLTFM_MSG_ERR("H2C IE CAM set hdr fail %d\n", ret);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("H2C IE CAM build txd fail %d\n", ret);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("H2C IE CAM tx fail %d\n", ret);
		goto fail;
	}

fail:
	h2cb_free(adapter, h2cb);

	if (!ret)
		h2c_end_flow(adapter);

	return ret;
}

u32 _mac_send_h2creg(struct mac_ax_adapter *adapter,
		     struct mac_ax_h2creg_info *h2c)
{
#define MAC_AX_H2CREG_CNT 100
#define MAC_AX_H2CREG_US 200
	u32 cnt = MAC_AX_H2CREG_CNT;
	u8 len, byte0, byte1, val;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct fwcmd_h2creg h2creg;
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_h2creg_offset *h2creg_offset;

	if (!h2c)
		return MACSUCCESS;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_ERR("FW is not ready\n");
		return MACFWNONRDY;
	}

	if (adapter->sm.mac_rdy != MAC_AX_MAC_RDY) {
		PLTFM_MSG_TRACE("MAC is not ready\n");
		adapter->stats.h2c_reg_uninit++;
	}

	h2creg_offset = p_ops->get_h2creg_offset(adapter);
	if (!h2creg_offset) {
		PLTFM_MSG_ERR("Get H2CREG offset FAIL\n");
		return MACNPTR;
	}

	do {
		if (!(MAC_REG_R8(h2creg_offset->ctrl) & B_AX_H2CREG_TRIGGER))
			break;
		PLTFM_DELAY_US(MAC_AX_H2CREG_US);
		cnt--;
	} while (cnt);

	if (!cnt) {
		PLTFM_MSG_ERR("FW does not process H2CREG\n");
		return MACPOLLTO;
	}

	if (h2c->content_len > H2CREG_CONTENT_LEN) {
		PLTFM_MSG_ERR("%s: h2creg len is TOO large\n", __func__);
		return MACFUNCINPUT;
	}

	len = h2c->content_len + H2CREG_HDR_LEN;
	if ((h2c->content_len + H2CREG_HDR_LEN) & 3)
		len = ((h2c->content_len + H2CREG_HDR_LEN) >> 2) + 1;
	else
		len = (h2c->content_len + H2CREG_HDR_LEN) >> 2;

	byte0 = (u8)GET_FIELD(h2c->h2c_content.dword0, FWCMD_H2CREG_BYTE2);
	byte1 = (u8)GET_FIELD(h2c->h2c_content.dword0, FWCMD_H2CREG_BYTE3);

	h2creg.dword0 = SET_WORD(h2c->id, FWCMD_H2CREG_H2CREG_HDR_FUNC) |
			SET_WORD(len, FWCMD_H2CREG_H2CREG_HDR_TOTAL_LEN) |
			SET_WORD(byte0, FWCMD_H2CREG_BYTE2) |
			SET_WORD(byte1, FWCMD_H2CREG_BYTE3);

	h2creg.dword1 = h2c->h2c_content.dword1;
	h2creg.dword2 = h2c->h2c_content.dword2;
	h2creg.dword3 = h2c->h2c_content.dword3;

	MAC_REG_W32(h2creg_offset->data0, h2creg.dword0);
	MAC_REG_W32(h2creg_offset->data1, h2creg.dword1);
	MAC_REG_W32(h2creg_offset->data2, h2creg.dword2);
	MAC_REG_W32(h2creg_offset->data3, h2creg.dword3);

	val = MAC_REG_R8(h2creg_offset->ctrl);
	MAC_REG_W8(h2creg_offset->ctrl, val | B_AX_H2CREG_TRIGGER);

	return MACSUCCESS;
}

u32 __recv_c2hreg(struct mac_ax_adapter *adapter, struct fwcmd_c2hreg *c2h)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_c2hreg_offset *c2hreg;
	u8 val;

	c2hreg = p_ops->get_c2hreg_offset(adapter);
	if (!c2hreg) {
		PLTFM_MSG_ERR("Get C2HREG offset FAIL\n");
		return MACNPTR;
	}

	if (!(MAC_REG_R8(c2hreg->ctrl) & B_AX_C2HREG_TRIGGER))
		return MACC2HREGEMP;

	c2h->dword0 = MAC_REG_R32(c2hreg->data0);
	c2h->dword1 = MAC_REG_R32(c2hreg->data1);
	c2h->dword2 = MAC_REG_R32(c2hreg->data2);
	c2h->dword3 = MAC_REG_R32(c2hreg->data3);
	val = MAC_REG_R8(c2hreg->ctrl);
	MAC_REG_W8(c2hreg->ctrl, val & ~B_AX_C2HREG_TRIGGER);

	return MACSUCCESS;
}

u32 mac_recv_c2hreg(struct mac_ax_adapter *adapter,
		    struct mac_ax_c2hreg_cont *cont)
{
	u32 ret;

	cont->id = FWCMD_C2H_FUNC_NULL;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_ERR("FW is not ready\n");
		return MACFWNONRDY;
	}

	if (adapter->sm.mac_rdy != MAC_AX_MAC_RDY) {
		PLTFM_MSG_TRACE("MAC is not ready\n");
		adapter->stats.c2h_reg_uninit++;
	}

	ret = __recv_c2hreg(adapter, &cont->c2h_content);
	if (ret == MACC2HREGEMP) {
		return ret;
	} else if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]Get C2H REG fail %d\n", ret);
		return ret;
	}

	cont->id = GET_FIELD(cont->c2h_content.dword0,
			     FWCMD_C2HREG_C2HREG_HDR_FUNC);
	cont->content_len = GET_FIELD(cont->c2h_content.dword0,
				      FWCMD_C2HREG_C2HREG_HDR_TOTAL_LEN);
	cont->content_len = (cont->content_len << 2) - C2HREG_HDR_LEN;

	return MACSUCCESS;
}

u32 mac_notify_fw_dbcc(struct mac_ax_adapter *adapter, u8 en)
{
	#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u32 ret;
	struct fwcmd_notify_dbcc *dbcc;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	dbcc = (struct fwcmd_notify_dbcc *)
		h2cb_put(h2cb, sizeof(struct fwcmd_notify_dbcc));
	if (!dbcc) {
		ret = MACNPTR;
		PLTFM_MSG_ERR("%s: h2c put fail\n", __func__);
		goto fail;
	}

	dbcc->dword0 = en ? FWCMD_H2C_NOTIFY_DBCC_EN : 0;

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MEDIA_RPT,
			      FWCMD_H2C_FUNC_NOTIFY_DBCC, 0, 1);
	if (ret) {
		PLTFM_MSG_ERR("%s: set hdr fail %d\n", __func__, ret);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("%s: build txd fail %d\n", __func__, ret);
		goto fail;
	}

	#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("%s: tx fail %d\n", __func__, ret);
		goto fail;
	}

fail:
	h2cb_free(adapter, h2cb);

	if (!ret)
		h2c_end_flow(adapter);

	return ret;
}

u32 poll_c2hreg(struct mac_ax_adapter *adapter,
		struct mac_ax_c2hreg_poll *c2h)
{
	u32 cnt, poll_us, ret;
	struct mac_ax_c2hreg_cont *c2hreg_cont;
	struct mac_ax_drv_stats *drv_stats = &adapter->drv_stats;

	if (!c2h)
		return MACSUCCESS;

	cnt = c2h->retry_cnt;
	poll_us = c2h->retry_wait_us;
	c2hreg_cont = &c2h->c2hreg_cont;
	do {
		ret = mac_recv_c2hreg(adapter, c2hreg_cont);
		if (cnt == 0 || ret == MACSUCCESS)
			break;

		if (drv_stats->drv_rm) {
			PLTFM_MSG_ERR("%s: driver removed\n", __func__);
			return MACDRVRM;
		}

		if (ret != MACSUCCESS) {
			if (ret == MACC2HREGEMP) {
				PLTFM_DELAY_US(poll_us);
				cnt--;
			} else {
				PLTFM_MSG_ERR("%s: c2hreg fail\n", __func__);
				return ret;
			}
		}
	} while (cnt);

	PLTFM_MSG_TRACE("%s: cnt = %d, us = %d\n",
			__func__, cnt, poll_us);

	if (ret == MACSUCCESS) {
		if (c2h->polling_id != FWCMD_C2H_FUNC_NULL &&
		    c2h->polling_id != c2hreg_cont->id) {
			PLTFM_MSG_ERR("%s: surprised c2h\n", __func__);
			PLTFM_MSG_ERR("rev: %x\n", c2h->polling_id);
			PLTFM_MSG_ERR("exp: %x\n", c2hreg_cont->id);
			ret = MACBADC2HREG;
		}
	} else {
		PLTFM_MSG_ERR("%s: polling c2hreg timeout\n", __func__);
	}

	return ret;
}

u32 proc_msg_reg(struct mac_ax_adapter *adapter,
		 struct mac_ax_h2creg_info *h2c,
		 struct mac_ax_c2hreg_poll *c2h)
{
	struct mac_ax_dbgpkg_en en = {0};
	u32 ret = MACSUCCESS;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY) {
		PLTFM_MSG_ERR("FW is not ready\n");
		return MACFWNONRDY;
	}

	if (adapter->sm.fw_rst != MAC_AX_FW_RESET_IDLE) {
		PLTFM_MSG_ERR("FW is not ready\n");
		return MACIOERRSERL1;
	}

	PLTFM_MUTEX_LOCK(&adapter->fw_info.msg_reg);

	ret = _mac_send_h2creg(adapter, h2c);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: send h2c reg fail: %d\n", __func__, ret);
		goto END;
	}

	ret = poll_c2hreg(adapter, c2h);
	if (ret != MACSUCCESS)
		PLTFM_MSG_ERR("%s: poll c2h reg fail: %d\n", __func__, ret);

END:
	PLTFM_MUTEX_UNLOCK(&adapter->fw_info.msg_reg);
	if (ret != MACSUCCESS) {
		en.plersvd_dbg = 1;
		mac_dbg_status_dump(adapter, NULL, &en);
	}
	return ret;
}

static u32 get_wps_rpt_event_id(struct mac_ax_adapter *adapter,
				struct rtw_c2h_info *c2h,
				enum phl_msg_evt_id *id,
				u8 *c2h_info)
{
	struct fwcmd_wps_rpt *rpt = (struct fwcmd_wps_rpt *)c2h->content;
	u32 state, val;

	val = le32_to_cpu(rpt->dword0);
	state = GET_FIELD(val, FWCMD_C2H_WPS_RPT_STATE);

	if (!state)
		*id = MSG_EVT_WPS_RELEASED;
	else
		*id = MSG_EVT_WPS_PRESSED;

	return MACSUCCESS;
}

static u32 get_bcn_resend_event(struct mac_ax_adapter *adapter,
				struct rtw_c2h_info *c2h,
				enum phl_msg_evt_id *id,
				u8 *c2h_info)
{
	*id = MSG_EVT_BCN_RESEND;

	return MACSUCCESS;
}

static u32 get_tsf32_togl_rpt_event(struct mac_ax_adapter *adapter,
				    struct rtw_c2h_info *c2h,
				    enum phl_msg_evt_id *id,
				    u8 *c2h_info)
{
	*id = MSG_EVT_TSF32_TOG;

	return MACSUCCESS;
}

static u32 get_fw_rx_dbg_event(struct mac_ax_adapter *adapter,
			       struct rtw_c2h_info *c2h,
			       enum phl_msg_evt_id *id,
			       u8 *c2h_info)
{
	*id = MSG_EVT_DBG_RX_DUMP;

	return MACSUCCESS;
}

static u32 get_bcn_csa_event(struct mac_ax_adapter *adapter,
			     struct rtw_c2h_info *c2h,
			     enum phl_msg_evt_id *id,
			     u8 *c2h_info)
{
	*id = MSG_EVT_CSA_COUNTDOWN_ZERO;

	return MACSUCCESS;
}

static u32 get_scanofld_event(struct mac_ax_adapter *adapter, struct rtw_c2h_info *c2h,
			      enum phl_msg_evt_id *id, u8 *c2h_info)
{
#if SCANOFLD_RSP_EVT_PARSE
	struct fwcmd_scanofld_rsp *pkg;
	struct mac_ax_scanofld_rsp *rsp;
	struct mac_ax_scanofld_chrpt chrpt_struct;
	u32 chrpt_size_h_dw;
	u32 *chrpt;
	u32 *chrpt_in;
	u32 sh;
	u32 chidx;

	pkg = (struct fwcmd_scanofld_rsp *)c2h->content;
	rsp = (struct mac_ax_scanofld_rsp *)c2h_info;
	chrpt_in = (u32 *)(c2h->content + sizeof(struct fwcmd_scanofld_rsp));

	chrpt_size_h_dw = sizeof(struct mac_ax_scanofld_chrpt) / sizeof(u32);

	PLTFM_MEMSET(rsp, 0, sizeof(struct mac_ax_scanofld_rsp));

	pkg->dword0 = le32_to_cpu(pkg->dword0);
	pkg->dword1 = le32_to_cpu(pkg->dword1);
	pkg->dword2 = le32_to_cpu(pkg->dword2);
	pkg->dword3 = le32_to_cpu(pkg->dword3);

	rsp->pri_ch = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_PRI_CH);
	rsp->notify_reason = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_NOTIFY_REASON);
	rsp->status = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_STATUS);
	rsp->ch_band = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_CH_BAND);
	rsp->band = pkg->dword3 & FWCMD_C2H_SCANOFLD_RSP_BAND;
	PLTFM_MSG_TRACE("[scnofld][rsp][%d]: Reason %d, ch %d (band %d), status %d\n",
			rsp->band, rsp->notify_reason, rsp->pri_ch, rsp->ch_band, rsp->status);
	switch (rsp->notify_reason) {
	case MAC_AX_SCAN_END_SCAN_NOTIFY:
		PLTFM_MUTEX_LOCK(&adapter->scanofld_info.fw_chlist_state_lock);
		adapter->scanofld_info.fw_chlist_busy[rsp->band] = 0;
		PLTFM_MUTEX_UNLOCK(&adapter->scanofld_info.fw_chlist_state_lock);
		adapter->scanofld_info.fw_scan_busy[rsp->band] = 0;
		/* fall through */

	case MAC_AX_SCAN_GET_RPT_NOTIFY:
		rsp->scanned_round = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_SCANNED_ROUND);
		rsp->spent_low = pkg->dword1;
		rsp->spent_high = pkg->dword2;
		rsp->air_density = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_AIR_DENSITY);
		rsp->actual_period = GET_FIELD(pkg->dword0, FWCMD_C2H_SCANOFLD_RSP_ACTUAL_PERIOD);
		rsp->tx_fail_cnt = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_TX_FAIL_CNT);
		rsp->num_ch_rpt = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_NUM_CH_RPT);
		rsp->ch_rpt_size = GET_FIELD(pkg->dword3, FWCMD_C2H_SCANOFLD_RSP_CH_RPT_SIZE);

		PLTFM_MSG_TRACE("[scnofld][rsp][%d][end] scan %d rnd in %llu us, last slt %d us\n",
				rsp->band, rsp->scanned_round,
				((u64)rsp->spent_high << 32) + rsp->spent_low, rsp->actual_period);
		PLTFM_MSG_TRACE("[scnofld][rsp][%d][end] airDense %d, txFail %d\n",
				rsp->band, rsp->air_density, rsp->tx_fail_cnt);
		PLTFM_MSG_TRACE("[scnofld][rsp][%d][end] %d ch rpt (size %d)\n",
				rsp->band, rsp->num_ch_rpt, rsp->ch_rpt_size);

		if (!rsp->num_ch_rpt || !rsp->ch_rpt_size)
			break;

		for (chidx = 0; chidx < rsp->num_ch_rpt; chidx++) {
			chrpt = (u32 *)&chrpt_struct;
			for (sh = 0; sh < chrpt_size_h_dw; sh++) {
				*chrpt_in = le32_to_cpu(*chrpt_in);
				PLTFM_MEMCPY(chrpt++, chrpt_in++, sizeof(u32));
			}
			chrpt_in += (rsp->ch_rpt_size - chrpt_size_h_dw);
			PLTFM_MSG_TRACE("[scnofld][rsp][%d][end] ch %d, rx %d, txfail %x, hit %d\n",
					rsp->band, chrpt_struct.pri_ch, chrpt_struct.rx_cnt,
					chrpt_struct.tx_fail, chrpt_struct.parsed);
		}
		break;

	case MAC_AX_SCAN_LEAVE_CH_NOTIFY:
		rsp->actual_period = GET_FIELD(pkg->dword0,
					       FWCMD_C2H_SCANOFLD_RSP_ACTUAL_PERIOD);
		rsp->tx_fail_cnt = GET_FIELD(pkg->dword3,
					     FWCMD_C2H_SCANOFLD_RSP_TX_FAIL_CNT);
		PLTFM_MSG_TRACE("[scnofld][rsp][%d][leave] pd %d, txfail %d\n",
				rsp->band, rsp->actual_period, rsp->tx_fail_cnt);
		break;

	default:
		break;
	}
#endif //SCANOFLD_RSP_EVT_PARSE
#if SCANOFLD_RSP_EVT_ID
	*id = MSG_EVT_SCANOFLD;
#endif //SCANOFLD_RSP_EVT_ID
	return MACSUCCESS;
}

static struct c2h_event_id_proc event_proc[] = {
	/* cat, class, func, hdl */
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_MISC,
	 FWCMD_C2H_FUNC_WPS_RPT, get_wps_rpt_event_id},
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_FW_OFLD,
	 FWCMD_C2H_FUNC_BEACON_RESEND, get_bcn_resend_event},
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_FW_OFLD,
	 FWCMD_C2H_FUNC_TSF32_TOGL_RPT, get_tsf32_togl_rpt_event},
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_MISC,
	 FWCMD_C2H_FUNC_CCXRPT, get_ccxrpt_event},
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_FW_DBG,
	 FWCMD_C2H_FUNC_RX_DBG, get_fw_rx_dbg_event},
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_FW_INFO,
	 FWCMD_C2H_FUNC_BCN_CNT, get_bcn_stats_event},
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_FW_INFO,
	 FWCMD_C2H_FUNC_BCN_CSAZERO, get_bcn_csa_event},
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_FW_OFLD,
	 FWCMD_C2H_FUNC_SCANOFLD_RSP, get_scanofld_event},
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_FW_OFLD,
	 FWCMD_C2H_FUNC_WIFI_SENSING_CSI_TX_RESULT, get_sensing_csi_event},
	{FWCMD_C2H_CAT_MAC, FWCMD_C2H_CL_FW_OFLD,
	 FWCMD_C2H_FUNC_BCNERLYNTFY, get_bcn_erly_event},
	{FWCMD_C2H_CAT_NULL, FWCMD_C2H_CL_NULL,
	 FWCMD_C2H_FUNC_NULL, NULL},
};

u32 mac_get_c2h_event(struct mac_ax_adapter *adapter,
		      struct rtw_c2h_info *c2h,
		      enum phl_msg_evt_id *id,
		      u8 *c2h_info)
{
	struct c2h_event_id_proc *proc;
	u32 (*hdl)(struct mac_ax_adapter *adapter, struct rtw_c2h_info *c2h,
		   enum phl_msg_evt_id *id, u8 *c2h_info) = NULL;

	/*for C2H ack bit no need to process*/
	if (c2h->type_done_ack == 1 || c2h->type_rec_ack == 1)
		return MACSUCCESS;

	proc = event_proc;
	while (proc->cat != FWCMD_C2H_CAT_NULL) {
		if (proc->cat == c2h->c2h_cat &&
		    proc->cls == c2h->c2h_class &&
		    proc->func == c2h->c2h_func) {
			hdl = proc->hdl;
			return hdl(adapter, c2h, id, c2h_info);
		}
		proc++;
	}

	return MACSUCCESS;
}
