/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#ifndef _RTW_XMIT_SHORTCUT_H_
#define _RTW_XMIT_SHORTCUT_H_

#ifdef CONFIG_CORE_TXSC

#ifndef USE_ONE_WLHDR
#define USE_PREV_WLHDR_BUF
#endif

#define CORE_TXSC_ENTRY_NUM 8
#define CORE_TXSC_WLHDR_SIZE (WLHDR_SIZE + SNAP_SIZE + 2 + _AES_IV_LEN_)
#define CORE_TXSC_DEBUG_BUF_SIZE (sizeof(struct rtw_xmit_req) + sizeof(struct rtw_pkt_buf_list)*2)

enum txsc_action_type {
	TXSC_NONE,
	TXSC_SKIP,
	TXSC_ADD,
	TXSC_APPLY,
	TXSC_AMSDU_APPLY,
	TXSC_DEBUG,
};

enum full_cnt_type {
	PHL_WD_EMPTY,
	PHL_BD_FULL,
	PHL_WD_RECYCLE_NOTHING,
	PHL_WD_RECYCLE_OK,
};

struct txsc_pkt_entry {
	enum txsc_action_type step;
	struct sta_info *psta;
	struct rtw_xmit_req *ptxreq;

	u8 txsc_id;
	u8 priority;

	struct sk_buff *xmit_skb[MAX_TXSC_SKB_NUM];
	u8 skb_cnt;
};

struct txsc_entry {
	u8	txsc_is_used;
	u8	txsc_ethdr[ETH_HLEN];

	/* wlhdr --- */
	#ifdef USE_ONE_WLHDR
	u8	*txsc_wlhdr;
	#else
	u8	txsc_wlhdr[CORE_TXSC_WLHDR_SIZE];
	#endif
	u8	txsc_wlhdr_len;
	struct rtw_pkt_buf_list	txsc_pkt_list0;
	/* wlhdr --- */

	struct rtw_t_meta_data	txsc_mdata;
	u32	txsc_frag_len;/* for pkt frag check */

	u8	txsc_phl_id; /* CONFIG_PHL_TXSC */
	u32	txsc_cache_hit;
};

void _print_txreq_mdata(struct rtw_t_meta_data *mdata, const char *func);
void _print_txreq_pklist(struct xmit_frame *pxframe, struct rtw_xmit_req *ptxsc_txreq, struct sk_buff *pskb, const char *func);
void txsc_init(_adapter *padapter);
void txsc_clear(_adapter *padapter);
void txsc_dump(_adapter *padapter);
void txsc_dump_data(u8 *buf, u16 buf_len, const char *prefix);
u8 txsc_get_sc_cached_entry(_adapter *padapter, struct sk_buff *pskb, struct txsc_pkt_entry *txsc_pkt);
void txsc_add_sc_cache_entry(_adapter *padapter, struct xmit_frame *pxframe, struct txsc_pkt_entry *txsc_pkt);
u8 txsc_apply_sc_cached_entry(_adapter *padapter, struct txsc_pkt_entry *txsc_pkt);
#ifdef CONFIG_PCI_HCI
void txsc_fill_txreq_phyaddr(_adapter *padapter, struct rtw_pkt_buf_list *pkt_list);
void txsc_recycle_txreq_phyaddr(_adapter *padapter, struct rtw_xmit_req *txreq);
#endif
void txsc_free_txreq(_adapter *padapter, struct rtw_xmit_req *txreq);
void txsc_debug_sc_entry(_adapter *padapter, struct xmit_frame *pxframe, struct txsc_pkt_entry *txsc_pkt);
void txsc_issue_addbareq_cmd(_adapter *padapter, u8 priority, struct sta_info *psta, u8 issue_when_busy);
#endif /* CONFIG_CORE_TXSC */
#endif /* _RTW_XMIT_SHORTCUT_H_ */

