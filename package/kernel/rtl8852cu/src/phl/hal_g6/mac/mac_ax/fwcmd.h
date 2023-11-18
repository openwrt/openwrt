/** @file */
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

#ifndef _MAC_AX_FW_CMD_H_
#define _MAC_AX_FW_CMD_H_

#include "../type.h"
#include "../fw_ax/inc_hdr/fwcmd_intf.h"
#include "fwcmd_intf_f2p.h"
#include "fwcmd_intf_f2p_v1.h"
#include "trx_desc.h"
#include "fwofld.h"
#include "p2p.h"
#include "flash.h"
#include "dbg_cmd.h"

#define FWCMD_HDR_LEN		8
#define C2HREG_HDR_LEN 2
#define H2CREG_HDR_LEN 2
#define C2HREG_CONTENT_LEN 14
#define H2CREG_CONTENT_LEN 14
#define C2HREG_LEN (C2HREG_HDR_LEN + C2HREG_CONTENT_LEN)
#define H2CREG_LEN (H2CREG_HDR_LEN + H2CREG_CONTENT_LEN)

#define H2C_CMD_LEN		64
#define H2C_DATA_LEN		256
#define H2C_LONG_DATA_LEN	2048

#define SET_FWCMD_ID(_t, _ca, _cl, _f)                                         \
		(SET_WORD(_t, H2C_HDR_DEL_TYPE) | SET_WORD(_ca, H2C_HDR_CAT) | \
		 SET_WORD(_cl, H2C_HDR_CLASS) | SET_WORD(_f, H2C_HDR_FUNC))
#define GET_FWCMD_TYPE(id)	(GET_FIELD(id, C2H_HDR_DEL_TYPE))
#define GET_FWCMD_CAT(id)	(GET_FIELD(id, C2H_HDR_CAT))
#define GET_FWCMD_CLASS(id)	(GET_FIELD(id, C2H_HDR_CLASS))
#define GET_FWCMD_FUNC(id)	(GET_FIELD(id, C2H_HDR_FUNC))

#define FWCMD_TYPE_H2C	0
#define FWCMD_TYPE_C2H	1

#define FWCMD_C2H_CL_NULL		0xFF
#define FWCMD_C2H_FUNC_NULL		0xFF
#define FWCMD_C2H_CAT_NULL		0xFF

/**
 * @struct h2c_buf_head
 * @brief h2c_buf_head
 *
 * @var h2c_buf_head::next
 * Please Place Description here.
 * @var h2c_buf_head::prev
 * Please Place Description here.
 * @var h2c_buf_head::pool
 * Please Place Description here.
 * @var h2c_buf_head::size
 * Please Place Description here.
 * @var h2c_buf_head::qlen
 * Please Place Description here.
 * @var h2c_buf_head::suspend
 * Please Place Description here.
 * @var h2c_buf_head::lock
 * Please Place Description here.
 */
struct h2c_buf_head {
	/* keep first */
	struct h2c_buf *next;
	struct h2c_buf *prev;
	u8 *pool;
	u32 size;
	u32 qlen;
	u8 suspend;
	mac_ax_mutex lock;
};

/**
 * @struct fwcmd_wkb_head
 * @brief fwcmd_wkb_head
 *
 * @var fwcmd_wkb_head::next
 * Please Place Description here.
 * @var fwcmd_wkb_head::prev
 * Please Place Description here.
 * @var fwcmd_wkb_head::qlen
 * Please Place Description here.
 * @var fwcmd_wkb_head::lock
 * Please Place Description here.
 */
struct fwcmd_wkb_head {
	/* keep first */
	struct h2c_buf *next;
	struct h2c_buf *prev;
	u32 qlen;
	mac_ax_mutex lock;
};

/**
 * @struct h2c_buf
 * @brief h2c_buf
 *
 * @var h2c_buf::next
 * Please Place Description here.
 * @var h2c_buf::prev
 * Please Place Description here.
 * @var h2c_buf::_class_
 * Please Place Description here.
 * @var h2c_buf::id
 * Please Place Description here.
 * @var h2c_buf::master
 * Please Place Description here.
 * @var h2c_buf::len
 * Please Place Description here.
 * @var h2c_buf::head
 * Please Place Description here.
 * @var h2c_buf::end
 * Please Place Description here.
 * @var h2c_buf::data
 * Please Place Description here.
 * @var h2c_buf::tail
 * Please Place Description here.
 * @var h2c_buf::hdr_len
 * Please Place Description here.
 * @var h2c_buf::flags
 * Please Place Description here.
 * @var h2c_buf::h2c_seq
 * Please Place Description here.
 */
struct h2c_buf {
	/* keep first */
	struct h2c_buf *next;
	struct h2c_buf *prev;
	enum h2c_buf_class _class_;
	u32 id;
	u8 master;
	u32 len;
	u8 *head;
	u8 *end;
	u8 *data;
	u8 *tail;
	u32 hdr_len;
#define H2CB_FLAGS_FREED	BIT(0)
	u32 flags;
	u8 h2c_seq;
};

/**
 * @struct c2h_proc_class
 * @brief c2h_proc_class
 *
 * @var c2h_proc_class::id
 * Please Place Description here.
 * @var c2h_proc_class::handler
 * Please Place Description here.
 */
struct c2h_proc_class {
	u16 id;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info);
};

struct fw_status_proc_class {
	u16 id;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len);
};

/**
 * @struct c2h_proc_func
 * @brief c2h_proc_func
 *
 * @var c2h_proc_func::id
 * Please Place Description here.
 * @var c2h_proc_func::handler
 * Please Place Description here.
 */
struct c2h_proc_func {
	u16 id;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info);
};

/**
 * @struct mac_ax_c2hreg_info
 * @brief mac_ax_c2hreg_info
 *
 * @var mac_ax_c2hreg_info::id
 * Please Place Description here.
 * @var mac_ax_c2hreg_info::total_len
 * Please Place Description here.
 * @var mac_ax_c2hreg_info::content
 * Please Place Description here.
 * @var mac_ax_c2hreg_info::c2hreg
 * Please Place Description here.
 */

struct mac_ax_c2hreg_info {
	u8 id;
	u8 content_len;
	u8 *content;
	u8 c2hreg[C2HREG_LEN];
};

struct mac_ax_c2hreg_cont {
	u8 id;
	u8 content_len;
	struct fwcmd_c2hreg c2h_content;
};

struct mac_ax_c2hreg_poll {
	u8 polling_id;
	u32 retry_cnt;
	u32 retry_wait_us;
	struct mac_ax_c2hreg_cont c2hreg_cont;
};

struct mac_ax_h2creg_info {
	u8 id;
	u8 content_len;
	struct fwcmd_h2creg h2c_content;
};

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */
/**
 * @brief h2cb_init
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 h2cb_init(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2cb_exit
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 h2cb_exit(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2cb_alloc
 *
 * @param *adapter
 * @param buf_class
 * @return Please Place Description here.
 * @retval  rtw_h2c_pkt
 */
#if MAC_AX_PHL_H2C
struct rtw_h2c_pkt *h2cb_alloc(struct mac_ax_adapter *adapter,
			       enum rtw_h2c_pkt_type buf_class);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2cb_free
 *
 * @param *adapter
 * @param *h2cb
 * @return Please Place Description here.
 * @retval void
 */
void h2cb_free(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2cb_push
 *
 * @param *h2cb
 * @param len
 * @return Please Place Description here.
 * @retval u8
 */
u8 *h2cb_push(struct rtw_h2c_pkt *h2cb, u32 len);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2cb_pull
 *
 * @param *h2cb
 * @param len
 * @return Please Place Description here.
 * @retval u8
 */
u8 *h2cb_pull(struct rtw_h2c_pkt *h2cb, u32 len);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2cb_put
 *
 * @param *h2cb
 * @param len
 * @return Please Place Description here.
 * @retval u8
 */
u8 *h2cb_put(struct rtw_h2c_pkt *h2cb, u32 len);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2c_pkt_set_hdr
 *
 * @param *adapter
 * @param *h2cb
 * @param type
 * @param cat
 * @param _class_
 * @param func
 * @param rack
 * @param dack
 * @return Please Place Description here.
 * @retval u32
 */
u32 h2c_pkt_set_hdr(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		    u8 type, u8 cat, u8 _class_, u8 func, u16 rack, u16 dack);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2c_pkt_set_hdr_fwdl
 *
 * @param *adapter
 * @param *h2cb
 * @param type
 * @param cat
 * @param _class_
 * @param func
 * @param rack
 * @param dack
 * @return Please Place Description here.
 * @retval u32
 */
u32 h2c_pkt_set_hdr_fwdl(struct mac_ax_adapter *adapter,
			 struct rtw_h2c_pkt *h2cb, u8 type, u8 cat, u8 _class_,
			 u8 func, u16 rack, u16 dack);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2c_pkt_set_cmd
 *
 * @param *adapter
 * @param *h2cb
 * @param *cmd
 * @param len
 * @return Please Place Description here.
 * @retval u32
 */
u32 h2c_pkt_set_cmd(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		    u8 *cmd, u32 len);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2c_pkt_build_txd
 *
 * @param *adapter
 * @param *h2cb
 * @return Please Place Description here.
 * @retval u32
 */
u32 h2c_pkt_build_txd(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb);
/**
 * @}
 * @}
 */

/**
 * @brief h2c_agg_enqueue
 *
 * @param *adapter
 * @param *h2cb
 * @return Please Place Description here.
 * @retval u32
 */
u32 h2c_agg_enqueue(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2cb_alloc
 *
 * @param *adapter
 * @param buf_class
 * @return Please Place Description here.
 * @retval  h2c_buf
 */
#else
struct h2c_buf *h2cb_alloc(struct mac_ax_adapter *adapter,
			   enum h2c_buf_class buf_class);
/**
 * @}
 * @}
 */
void h2cb_free(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb);
u8 *h2cb_push(struct h2c_buf *h2cb, u32 len);
u8 *h2cb_pull(struct h2c_buf *h2cb, u32 len);
u8 *h2cb_put(struct h2c_buf *h2cb, u32 len);
u32 h2c_pkt_set_hdr(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb,
		    u8 type, u8 cat, u8 _class_, u8 func, u16 rack, u16 dack);
u32 h2c_pkt_set_hdr_fwdl(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb,
			 u8 type, u8 cat, u8 _class_, u8 func, u16 rack,
			 u16 dack);
u32 h2c_pkt_set_cmd(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb,
		    u8 *cmd, u32 len);
u32 h2c_pkt_build_txd(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb);
u32 h2c_agg_enqueue(struct mac_ax_adapter *adapter, h2c_buf *h2cb);

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief fwcmd_wq_enqueue
 *
 * @param *adapter
 * @param *h2cb
 * @return Please Place Description here.
 * @retval u32
 */
#endif
u32 fwcmd_wq_enqueue(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief fwcmd_wq_dequeue
 *
 * @param *adapter
 * @param id
 * @return Please Place Description here.
 * @retval  h2c_buf
 */
struct h2c_buf *fwcmd_wq_dequeue(struct mac_ax_adapter *adapter, u32 id);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief fwcmd_wq_idle
 *
 * @param *adapter
 * @param id
 * @return Please Place Description here.
 * @retval u32
 */
u32 fwcmd_wq_idle(struct mac_ax_adapter *adapter, u32 id);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup C2H
 * @{
 */

/**
 * @brief mac_process_c2h
 *
 * @param *adapter
 * @param *buf
 * @param len
 * @param *ret
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_process_c2h(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		    u8 *ret);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup C2H
 * @{
 */

/**
 * @brief c2h_field_parsing
 *
 * @param *hdr
 * @param *info
 * @return Please Place Description here.
 * @retval u8
 */
u8 c2h_field_parsing(struct fwcmd_hdr *hdr, struct rtw_c2h_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup C2H
 * @{
 */

/**
 * @brief mac_fw_log_cfg
 *
 * @param *adapter
 * @param *log_cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_fw_log_cfg(struct mac_ax_adapter *adapter,
		   struct mac_ax_fw_log *log_cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup Beacon
 * @{
 */

/**
 * @brief mac_send_bcn_h2c
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_send_bcn_h2c(struct mac_ax_adapter *adapter,
		     struct mac_ax_bcn_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief mac_host_getpkt_h2c
 *
 * @param *adapter
 * @param macid
 * @param pkttype
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_host_getpkt_h2c(struct mac_ax_adapter *adapter, u8 macid, u8 pkttype);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief mac_outsrc_h2c_common
 *
 * @param *adapter
 * @param *hdr
 * @param *pvalue
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_outsrc_h2c_common(struct mac_ax_adapter *adapter,
			  struct rtw_g6_h2c_hdr *hdr, u32 *pvalue);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup Beacon
 * @{
 */

/**
 * @brief mac_ie_cam_upd
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_ie_cam_upd(struct mac_ax_adapter *adapter,
		   struct mac_ax_ie_cam_cmd_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief h2c_end_flow
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 h2c_end_flow(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup H2C
 * @{
 */

/**
 * @brief mac_send_h2creg
 *
 * @param *adapter
 * @param *content
 * @param len
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_send_h2creg(struct mac_ax_adapter *adapter, u32 *content, u8 len);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup C2H
 * @{
 */

/**
 * @brief mac_process_c2hreg
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_process_c2hreg(struct mac_ax_adapter *adapter,
		       struct mac_ax_c2hreg_info *info);
/**
 * @}
 * @}
 */

u32 proc_msg_reg(struct mac_ax_adapter *adapter,
		 struct mac_ax_h2creg_info *h2c,
		 struct mac_ax_c2hreg_poll *c2h);

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup C2H
 * @{
 */

/**
 * @brief mac_get_c2h_event
 *
 * Get the phl_msg_evt_id from C2H packet
 *
 * @param *adapter
 * @param *c2h
 * @param *id
 * @return Return 0 when getting event ID successfully.
 * @retval u32
 */

u32 mac_get_c2h_event(struct mac_ax_adapter *adapter,
		      struct rtw_c2h_info *c2h,
		      enum phl_msg_evt_id *id,
		      u8 *c2h_info);

/**
 * @}
 * @}
 */

u32 mac_notify_fw_dbcc(struct mac_ax_adapter *adapter, u8 en);

#endif

