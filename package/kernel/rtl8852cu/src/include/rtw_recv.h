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
#ifndef _RTW_RECV_H_
#define _RTW_RECV_H_

#define RTW_RX_MSDU_ACT_NONE		0
#define RTW_RX_MSDU_ACT_INDICATE	BIT0
#define RTW_RX_MSDU_ACT_FORWARD		BIT1

#ifdef CONFIG_RTW_NAPI
	#define RTL_NAPI_WEIGHT (32)
#endif


#define NR_RECVFRAME 256

#define RXFRAME_ALIGN	8
#define RXFRAME_ALIGN_SZ	(1<<RXFRAME_ALIGN)

#define DRVINFO_SZ	4 /* unit is 8bytes */

#define MAX_RXFRAME_CNT	512
#define MAX_RX_NUMBLKS		(32)
#define RECVFRAME_HDR_ALIGN 128
#define MAX_CONTINUAL_NORXPACKET_COUNT 4    /*  In MAX_CONTINUAL_NORXPACKET_COUNT*2 sec  , no rx traffict would issue DELBA*/

#define PHY_RSSI_SLID_WIN_MAX				100
#define PHY_LINKQUALITY_SLID_WIN_MAX		20


#define SNAP_SIZE sizeof(struct ieee80211_snap_hdr)

#define MAX_SUBFRAME_COUNT	64
/* Bridge-Tunnel header (for EtherTypes ETH_P_AARP and ETH_P_IPX) */
extern u8 rtw_bridge_tunnel_header[];
extern u8 rtw_rfc1042_header[];

enum addba_rsp_ack_state {
	RTW_RECV_ACK_OR_TIMEOUT,
};

#ifdef RTW_PHL_RX
enum rtw_core_rx_state {
	CORE_RX_CONTINUE = _SUCCESS,
	CORE_RX_DONE,
	CORE_RX_DROP,
	CORE_RX_FAIL,
#ifdef CONFIG_RTW_CORE_RXSC
	CORE_RX_GO_SHORTCUT,
#endif
	CORE_RX_DEFRAG,
};
#endif

/* for Rx reordering buffer control */
struct recv_reorder_ctrl {
	_adapter	*padapter;
	u8 tid;
	u8 enable;
	u16 indicate_seq;/* =wstart_b, init_value=0xffff */
	u8 ampdu_size;
	unsigned long rec_abba_rsp_ack;

	#ifdef CONFIG_RECV_REORDERING_CTRL
	_queue pending_recvframe_queue;
	u8 wsize_b;
	_timer reordering_ctrl_timer;
	u8 bReorderWaiting;
	#endif
};

struct	stainfo_rxcache	{
	u16	tid_rxseq[16];
	u8 iv[16][8];
	u8 last_tid;
};


struct smooth_rssi_data {
	u32	elements[100];	/* array to store values */
	u32	index;			/* index to current array to store */
	u32	total_num;		/* num of valid elements */
	u32	total_val;		/* sum of valid elements */
};

struct signal_stat {
	u8	update_req;		/* used to indicate */
	u8	avg_val;		/* avg of valid elements */
	u32	total_num;		/* num of valid elements */
	u32	total_val;		/* sum of valid elements	 */
};

/*TODO get phyinfo from PHL PPDU status - RTW_WKARD_CORE_RSSI_V1*/
struct phydm_phyinfo_struct {
	bool is_valid;
	u8 rx_pwdb_all;
	u8 signal_quality;	/* OFDM: signal_quality=rx_mimo_signal_quality[0], CCK: signal qualityin 0-100 index. */

	s8 rx_power;		/* in dBm Translate from PWdB */
	s8 recv_signal_power;	/* Real power in dBm for this packet, no beautification and aggregation. Keep this raw info to be used for the other procedures. */
	u8 signal_strength;	/* in 0-100 index. */
	s8 rx_pwr[4];		/* per-path's pwdb */
	s8 rx_snr[4];		/* per-path's SNR	*/
	u8 rx_count:2;		/* RX path counter---*/
	u8 snr_fd_avg;
	u8 snr_fd[4];
	u8 snr_td_avg;
	u8 snr_td[4];
};


struct rx_pkt_attrib {
	u16	pkt_len;
	u8	physt;
	u8	drvinfo_sz;
	u8	shift_sz;
	u8	hdrlen; /* the WLAN Header Len */
	u8	to_fr_ds;
	u8	amsdu;
	u8	qos;
	u8	priority;
	u8	pw_save;
	u8	mdata;
	u16	seq_num;
	u8	frag_num;
	u8	mfrag;
	u8	order;
	u8	privacy; /* in frame_ctrl field */
	u8	bdecrypted;
	u8	encrypt; /* when 0 indicate no encrypt. when non-zero, indicate the encrypt algorith */
	u8	iv_len;
	u8	icv_len;
	u8	crc_err;
	u8	icv_err;

#ifdef CONFIG_RTW_CORE_RXSC
	u16	eth_type;
#endif

	u8	dst[ETH_ALEN];
	u8	src[ETH_ALEN];
	u8	ta[ETH_ALEN];
	u8	ra[ETH_ALEN];
	u8	bssid[ETH_ALEN];
#ifdef CONFIG_RTW_MESH
	u8	msa[ETH_ALEN]; /* mesh sa */
	u8	mda[ETH_ALEN]; /* mesh da */
	u8 mesh_ctrl_present;
	u8	mesh_ctrl_len; /* length of mesh control field */
#endif

	u8	ack_policy;

	u8	key_index;
	u16	data_rate; /* enum rtw_data_rate */
	u8	gi_ltf;
	u8 ch; /* RX channel */
	u8	bw;
	u8	stbc;
	u8	ldpc;
	u8	sgi;
	u8	pkt_rpt_type;
	u8	ampdu;
	u8	ppdu_cnt;
	u8	ampdu_eof;
	u32 	free_cnt;		/* free run counter */
	struct phydm_phyinfo_struct phy_info;
#ifdef CONFIG_WIFI_MONITOR
	u8 moif[16];
#endif

#ifdef CONFIG_TCP_CSUM_OFFLOAD_RX
	/* checksum offload realted varaiables */
	u8 csum_valid;		/* Checksum valid, 0: not check, 1: checked */
	u8 csum_err;		/* Checksum Error occurs */
#endif /* CONFIG_TCP_CSUM_OFFLOAD_RX */

#ifdef RTW_PHL_DBG_CMD
	u8	wl_type;
	u8	wl_subtype;
#endif
#ifdef CONFIG_RTW_CORE_RXSC
	u8	bsnaphdr;
#endif
	u8	addr_cam_vld;
	u16	macid;
};

#ifdef CONFIG_RTW_MESH
#define RATTRIB_GET_MCTRL_LEN(rattrib) ((rattrib)->mesh_ctrl_len)
#else
#define RATTRIB_GET_MCTRL_LEN(rattrib) 0
#endif

/* These definition is used for Rx packet reordering. */
#define SN_LESS(a, b)		(((a-b) & 0x800) != 0)
#define SN_EQUAL(a, b)	(a == b)
/* #define REORDER_WIN_SIZE	128 */
/* #define REORDER_ENTRY_NUM	128 */
#define REORDER_WAIT_TIME	(50) /* (ms) */

#if defined(CONFIG_PLATFORM_RTK390X) && defined(CONFIG_USB_HCI)
	#define RECVBUFF_ALIGN_SZ 32
#else
	#define RECVBUFF_ALIGN_SZ 8
#endif

/*GEORGIA_TODO_FIXIT_IC_DEPENDENCE*/
#define RXDESC_SIZE	24
#define RXDESC_OFFSET RXDESC_SIZE

#ifdef CONFIG_TRX_BD_ARCH
struct rx_buf_desc {
	/* RX has exactly one segment */
#ifdef CONFIG_64BIT_DMA
	unsigned int dword[4];
#else
	unsigned int dword[2];
#endif
};

struct recv_stat {
	unsigned int rxdw[8];
};
#else
struct recv_stat {
	unsigned int rxdw0;

	unsigned int rxdw1;

#if !(defined(CONFIG_RTL8822B) || defined(CONFIG_RTL8821C) || defined(CONFIG_RTL8822C)) && defined(CONFIG_PCI_HCI)  /* exclude 8822be, 8821ce ,8822ce*/
	unsigned int rxdw2;

	unsigned int rxdw3;
#endif

#ifndef BUF_DESC_ARCH
	unsigned int rxdw4;

	unsigned int rxdw5;

#ifdef CONFIG_PCI_HCI
	unsigned int rxdw6;

	unsigned int rxdw7;
#endif
#endif /* if BUF_DESC_ARCH is defined, rx_buf_desc occupy 4 double words */
};
#endif

#define EOR BIT(30)


/*
accesser of recv_priv: rtw_recv_entry(dispatch / passive level); recv_thread(passive) ; returnpkt(dispatch)
; halt(passive) ;

using enter_critical section to protect
*/

struct recv_info {
	u64 rx_bytes;
	u64 rx_pkts;
	u64 rx_drop;

#ifdef PRIVATE_R
	// total data rate index = 84 (refer to DESC_RATEVHTSS4MCS9 = 0x53)
	u64 rx_vo_pkt_count_per_data_rate[84];
	u64 rx_vo_pkt_retry_count;
#endif
	u64 dbg_rx_drop_count;
	u64 dbg_rx_ampdu_drop_count;
	u64 dbg_rx_ampdu_forced_indicate_count;
	u64 dbg_rx_ampdu_loss_count;
	u64 dbg_rx_dup_mgt_frame_drop_count;
	u64 dbg_rx_ampdu_window_shift_cnt;
	u64 dbg_rx_conflic_mac_addr_cnt;

	/* For display the phy informatiom */
	u8 is_signal_dbg;	/* for debug */
	u8 signal_strength_dbg;	/* for debug */

	/*RTW_WKARD_CORE_RSSI_V1 - GEORGIA MUST REFINE*/
	u8 signal_strength;
	u8 signal_qual;
	s8 rssi;	/* rtw_phl_rssi_to_dbm(ptarget_wlan->network.PhyInfo.SignalStrength); */


	#ifdef CONFIG_SIGNAL_STAT_PROCESS
	_timer signal_stat_timer;
	u32 signal_stat_sampling_interval;
	#endif

	/* u32 signal_stat_converging_constant; */
	struct signal_stat signal_qual_data;
	struct signal_stat signal_strength_data;


	u16 sink_udpport, pre_rtp_rxseq, cur_rtp_rxseq;

	boolean store_law_data_flag;
};
#ifdef CONFIG_SIGNAL_STAT_PROCESS
#define rtw_set_signal_stat_timer(recvinfo) _set_timer(&(recvinfo)->signal_stat_timer, (recvinfo)->signal_stat_sampling_interval)
#endif

struct sta_recv_priv {

	_lock	lock;
	sint	option;

	/* _queue	blk_strms[MAX_RX_NUMBLKS]; */
	_queue defrag_q;	 /* keeping the fragment frame until defrag */

	struct	stainfo_rxcache rxcache;
	u16	bmc_tid_rxseq[16];
	u16	nonqos_rxseq;
	u16	nonqos_bmc_rxseq;

	/* uint	sta_rx_bytes; */
	/* uint	sta_rx_pkts; */
	/* uint	sta_rx_fail; */

};

#if 0
struct recv_buf {
	_list list;

	_lock recvbuf_lock;

	u32	ref_cnt;

	_adapter *adapter;

	u8	*pbuf;
	u8	*pallocated_buf;

	u32	len;
	u8	*phead;
	u8	*pdata;
	u8	*ptail;
	u8	*pend;

#ifdef CONFIG_USB_HCI
	PURB	purb;
	dma_addr_t dma_transfer_addr;	/* (in) dma addr for transfer_buffer */
	u32 alloc_sz;

	u8  irp_pending;
	int  transfer_len;
#endif
	struct sk_buff *pskb;
};
#endif

struct recv_frame_hdr {
	_list	list;
	struct sk_buff *pkt;

	_adapter  *adapter;
	struct _ADAPTER_LINK *adapter_link;
	struct dvobj_priv *dvobj;

	u8 fragcnt;

	int frame_tag;
	int keytrack;
	struct rx_pkt_attrib attrib;

	uint  len;
	u8 *rx_head;
	u8 *rx_data;
	u8 *rx_tail;
	u8 *rx_end;

	void *precvbuf;


	/*  */
	struct sta_info *psta;
#ifdef CONFIG_RECV_REORDERING_CTRL
	/* for A-MPDU Rx reordering buffer control */
	struct recv_reorder_ctrl *preorder_ctrl;
#endif
#ifdef RTW_PHL_RX
	void *rx_req;
#endif
#ifdef CONFIG_RTW_CORE_RXSC
	struct core_rxsc_entry *rxsc_entry;
#endif

#ifdef CONFIG_WAPI_SUPPORT
	u8 UserPriority;
	u8 WapiTempPN[16];
	u8 WapiSrcAddr[6];
	u8 bWapiCheckPNInDecrypt;
	u8 bIsWaiPacket;
#endif

};


union recv_frame {
	union {
		_list list;
		struct recv_frame_hdr hdr;
		uint mem[RECVFRAME_HDR_ALIGN >> 2];
	} u;
	/* uint mem[MAX_RXSZ>>2]; */
};

enum rtw_rx_llc_hdl {
	RTW_RX_LLC_KEEP		= 0,
	RTW_RX_LLC_REMOVE	= 1,
	RTW_RX_LLC_VLAN		= 2,
};

struct recv_priv {
	struct dvobj_priv *dvobj;

	#ifdef CONFIG_RECV_THREAD_MODE
	_sema recv_sema;
	#endif
	_queue free_recv_queue; /*recv_frame*/
	#if 0
	_queue uc_swdec_pending_queue;
	#endif

	u8 *pallocated_frame_buf;
	u8 *precv_frame_buf;

	uint free_recvframe_cnt;

#if defined(PLATFORM_LINUX) || defined(PLATFORM_FREEBSD)
#ifdef CONFIG_RTW_NAPI
	struct sk_buff_head rx_napi_skb_queue;
#endif
#endif /* defined(PLATFORM_LINUX) || defined(PLATFORM_FREEBSD) */

};

bool rtw_rframe_del_wfd_ie(union recv_frame *rframe, u8 ies_offset);
#ifdef RTW_PHL_RX
extern void dump_recv_frame(_adapter *adapter, union recv_frame *prframe);
extern sint validate_recv_frame(_adapter *adapter, union recv_frame *precv_frame);
extern s32 rtw_core_rx_data_pre_process(_adapter *adapter, union recv_frame **prframe);
extern s32 rtw_core_rx_data_post_process(_adapter *adapter, union recv_frame *prframe);

enum rtw_phl_status rtw_core_rx_process(void *drv_priv);
void process_pwrbit_data(_adapter *padapter, union recv_frame *precv_frame, struct sta_info *psta);
void process_wmmps_data(_adapter *padapter, union recv_frame *precv_frame, struct sta_info *psta);
#ifdef CONFIG_RTW_CORE_RXSC
sint recv_ucast_pn_decache(union recv_frame *precv_frame);
sint recv_bcast_pn_decache(union recv_frame *precv_frame);
#endif /* CONFIG_RTW_CORE_RXSC */
#endif

extern void rtw_init_recvframe(union recv_frame *precvframe);
extern int rtw_free_recvframe(union recv_frame *precvframe);
union recv_frame *rtw_alloc_recvframe(_queue *pfree_recv_queue);

#if 0
u32 rtw_free_uc_swdec_pending_queue(struct dvobj_priv *dvobj);
#endif

#if defined(CONFIG_80211N_HT) && defined(CONFIG_RECV_REORDERING_CTRL)
void rtw_reordering_ctrl_timeout_handler(void *pcontext);
#endif

#if 0
void rx_query_phy_status(union recv_frame *rframe, u8 *phy_stat);
#endif

int rtw_inc_and_chk_continual_no_rx_packet(struct sta_info *sta, int tid_index);
void rtw_reset_continual_no_rx_packet(struct sta_info *sta, int tid_index);

#ifdef CONFIG_RECV_THREAD_MODE
thread_return rtw_recv_thread(thread_context context);
#endif
#ifdef RTW_WKARD_CORE_RSSI_V1
void rx_process_phy_info(union recv_frame *precvframe);
#endif
__inline static u8 *get_rxmem(union recv_frame *precvframe)
{
	/* always return rx_head... */
	if (precvframe == NULL)
		return NULL;

	return precvframe->u.hdr.rx_head;
}

__inline static u8 *get_rx_status(union recv_frame *precvframe)
{

	return get_rxmem(precvframe);

}

__inline static u8 *get_recvframe_data(union recv_frame *precvframe)
{

	/* alwasy return rx_data */
	if (precvframe == NULL)
		return NULL;

	return precvframe->u.hdr.rx_data;

}

__inline static u8 *recvframe_push(union recv_frame *precvframe, sint sz)
{
	/* append data before rx_data */

	/* add data to the start of recv_frame
	*
	*      This function extends the used data area of the recv_frame at the buffer
	*      start. rx_data must be still larger than rx_head, after pushing.
	*/

	if (precvframe == NULL)
		return NULL;


	precvframe->u.hdr.rx_data -= sz ;
	if (precvframe->u.hdr.rx_data < precvframe->u.hdr.rx_head) {
		precvframe->u.hdr.rx_data += sz ;
		return NULL;
	}

	precvframe->u.hdr.len += sz;

	return precvframe->u.hdr.rx_data;

}


__inline static u8 *recvframe_pull(union recv_frame *precvframe, sint sz)
{
	/* rx_data += sz; move rx_data sz bytes  hereafter */

	/* used for extract sz bytes from rx_data, update rx_data and return the updated rx_data to the caller */


	if (precvframe == NULL)
		return NULL;


	precvframe->u.hdr.rx_data += sz;

	if (precvframe->u.hdr.rx_data > precvframe->u.hdr.rx_tail) {
		precvframe->u.hdr.rx_data -= sz;
		return NULL;
	}

	precvframe->u.hdr.len -= sz;

	return precvframe->u.hdr.rx_data;

}

__inline static u8 *recvframe_put(union recv_frame *precvframe, sint sz)
{
	/* rx_tai += sz; move rx_tail sz bytes  hereafter */

	/* used for append sz bytes from ptr to rx_tail, update rx_tail and return the updated rx_tail to the caller */
	/* after putting, rx_tail must be still larger than rx_end. */
	unsigned char *prev_rx_tail;

	/* RTW_INFO("recvframe_put: len=%d\n", sz); */

	if (precvframe == NULL)
		return NULL;

	prev_rx_tail = precvframe->u.hdr.rx_tail;

	precvframe->u.hdr.rx_tail += sz;

	if (precvframe->u.hdr.rx_tail > precvframe->u.hdr.rx_end) {
		precvframe->u.hdr.rx_tail -= sz;
		return NULL;
	}

	precvframe->u.hdr.len += sz;

	return precvframe->u.hdr.rx_tail;

}



__inline static u8 *recvframe_pull_tail(union recv_frame *precvframe, sint sz)
{
	/* rmv data from rx_tail (by yitsen) */

	/* used for extract sz bytes from rx_end, update rx_end and return the updated rx_end to the caller */
	/* after pulling, rx_end must be still larger than rx_data. */

	if (precvframe == NULL)
		return NULL;

	precvframe->u.hdr.rx_tail -= sz;

	if (precvframe->u.hdr.rx_tail < precvframe->u.hdr.rx_data) {
		precvframe->u.hdr.rx_tail += sz;
		return NULL;
	}

	precvframe->u.hdr.len -= sz;

	return precvframe->u.hdr.rx_tail;

}

__inline static sint get_recvframe_len(union recv_frame *precvframe)
{
	return precvframe->u.hdr.len;
}


struct sta_info;
extern void _rtw_init_sta_recv_priv(struct sta_recv_priv *psta_recvpriv);

extern void  mgt_dispatcher(_adapter *padapter, union recv_frame *precv_frame);

u8 adapter_allow_bmc_data_rx(_adapter *adapter);
#if 0
s32 pre_recv_entry(union recv_frame *precvframe, u8 *pphy_status);
#endif
void count_rx_stats(_adapter *padapter, union recv_frame *prframe, struct sta_info *sta);
u8 rtw_init_lite_recv_resource(struct dvobj_priv *dvobj);
void rtw_free_lite_recv_resource(struct dvobj_priv *dvobj);
#endif
