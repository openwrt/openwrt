/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
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
#ifndef _PHL_STRUCT_H_
#define _PHL_STRUCT_H_

#define PHL_STA_TID_NUM (16)    /* TODO: */

struct hci_info_t {
	/* enum rtw_hci_type hci_type; */

#if defined(CONFIG_PCI_HCI)

	u8 total_txch_num;
	u8 total_rxch_num;
	u8 *txbd_buf;
	u8 *rxbd_buf;
#if defined(PCIE_TRX_MIT_EN)
	u8 fixed_mitigation; /*no watchdog dynamic setting*/
	u8 rx_mit_counter_high;
	u32 rx_mit_timer_high;
#endif
#elif defined(CONFIG_USB_HCI)
	u16 usb_bulkout_size;
#elif defined(CONFIG_SDIO_HCI)
	u32 tx_drop_cnt;	/* bit31 means overflow or not */
#ifdef SDIO_TX_THREAD
	_os_sema tx_thrd_sema;
	_os_thread tx_thrd;
#ifdef CONFIG_PHL_SDIO_TX_CB_THREAD
#ifndef RTW_WKARD_SDIO_TX_USE_YIELD
	_os_lock tx_buf_lock;
	_os_event *tx_buf_event;
#endif /* !RTW_WKARD_SDIO_TX_USE_YIELD */
#endif /* CONFIG_PHL_SDIO_TX_CB_THREAD */
#endif /* SDIO_TX_THREAD */
#endif

	u8 *wd_ring;
	u8 *txbuf_pool;
	u8 *rxbuf_pool;
	u8 *wp_tag;
	u16 wp_seq[PHL_MACID_MAX_NUM]; 	/* maximum macid number */

};


#define MAX_PHL_RING_STATUS_NUMBER PHL_MACID_MAX_NUM
#define RX_REORDER_RING_NUMBER PHL_MACID_MAX_NUM
#define PCIE_BUS_EFFICIENCY 4
#define ETH_ALEN 6

struct phl_ring_status {
	_os_list list;
	u8 band;/*0 or 1*/
	u8 wmm;/*0 or 1*/
	u8 port;
	/*u8 mbssid*/
	u16 req_busy;
	struct rtw_phl_tx_ring *ring_ptr;
};

struct phl_ring_sts_pool {
	struct phl_ring_status ring_sts[MAX_PHL_RING_STATUS_NUMBER];
	_os_list idle;
	_os_list busy;
	_os_lock idle_lock;
	_os_lock busy_lock;
};

/**
 * struct phl_hci_trx_ops - interface specific operations
 *
 * @hci_trx_init: the function for HCI trx init
 * @hci_trx_deinit: the function for HCI trx deinit
 * @prepare_tx: prepare packets for hal transmission
 * @recycle_rx_buf: recycle rx buffer
 * @tx: tx packet to hw
 * @rx: rx packet to sw
 */
struct phl_info_t;
struct phl_hci_trx_ops {
	enum rtw_phl_status (*hci_trx_init)(struct phl_info_t *phl);
	void (*hci_trx_deinit)(struct phl_info_t *phl);
	enum rtw_phl_status (*prepare_tx)(struct phl_info_t *phl,
					struct rtw_xmit_req *tx_req);
	enum rtw_phl_status (*recycle_rx_buf)(struct phl_info_t *phl,
					void *r, u8 ch, enum rtw_rx_type type);
	enum rtw_phl_status (*tx)(struct phl_info_t *phl);
	enum rtw_phl_status (*rx)(struct phl_info_t *phl);
	enum rtw_phl_status (*trx_cfg)(struct phl_info_t *phl);
	void (*trx_stop)(struct phl_info_t *phl);
	enum rtw_phl_status (*pltfm_tx)(struct phl_info_t *phl, void *pkt);
	void (*free_h2c_pkt_buf)(struct phl_info_t *phl_info,
				   struct rtw_h2c_pkt *_h2c_pkt);
	enum rtw_phl_status (*alloc_h2c_pkt_buf)(struct phl_info_t *phl_info,
		struct rtw_h2c_pkt *_h2c_pkt, u32 buf_len);
	void (*trx_reset)(struct phl_info_t *phl, u8 type);
	void (*trx_resume)(struct phl_info_t *phl, u8 type);
	void (*tx_reset_hwband)(struct phl_info_t *phl_info, enum phl_band_idx band_idx);
	void (*req_tx_stop)(struct phl_info_t *phl);
	void (*req_rx_stop)(struct phl_info_t *phl);
	bool (*is_tx_pause)(struct phl_info_t *phl);
	bool (*is_rx_pause)(struct phl_info_t *phl);
	void *(*get_txbd_buf)(struct phl_info_t *phl);
	void *(*get_rxbd_buf)(struct phl_info_t *phl);
	void (*recycle_rx_pkt)(struct phl_info_t *phl,
			       struct rtw_phl_rx_pkt *phl_rx);
	void (*rx_handle_normal)(struct phl_info_t *phl_info,
						struct rtw_phl_rx_pkt *phl_rx);
	void (*tx_watchdog)(struct phl_info_t *phl_info);

#ifdef CONFIG_PCI_HCI
	enum rtw_phl_status (*recycle_busy_wd)(struct phl_info_t *phl);
	enum rtw_phl_status (*recycle_busy_h2c)(struct phl_info_t *phl);
	void (*return_tx_wps)(struct phl_info_t *phl);
#endif

#ifdef CONFIG_USB_HCI
	enum rtw_phl_status (*pend_rxbuf)(struct phl_info_t *phl, void *rxobj,
						u32 inbuf_len, u8 status_code);
	enum rtw_phl_status (*recycle_tx_buf)(void *phl, u8 *tx_buf_ptr);
#endif

#if defined(CONFIG_SDIO_HCI) && defined(CONFIG_PHL_SDIO_READ_RXFF_IN_INT)
	enum rtw_phl_status (*recv_rxfifo)(struct phl_info_t *phl);
#endif

#ifdef CONFIG_PCI_HCI
	void (*dump_wd_info)(struct phl_info_t *phl, u32 val);
#endif

};

/**
 * struct phl_tid_ampdu_rx - TID aggregation information (Rx).
 *
 * @reorder_buf: buffer to reorder incoming aggregated MPDUs.
 * @reorder_time: time when frame was added
 * @sta: station we are attached to
 * @head_seq_num: head sequence number in reordering buffer.
 * @stored_mpdu_num: number of MPDUs in reordering buffer
 * @ssn: Starting Sequence Number expected to be aggregated.
 * @buf_size: buffer size for incoming A-MPDUs
 * @timeout: reset timer value (in TUs).
 * @tid: TID number
 * @started: this session has started (head ssn or higher was received)
 */
struct phl_tid_ampdu_rx {
	struct rtw_phl_rx_pkt **reorder_buf;
	u32 *reorder_time;
	struct rtw_phl_stainfo_t *sta;
	u16 head_seq_num;
	u16 stored_mpdu_num;
	u16 ssn;
	u16 buf_size;
	u16 tid;
	u8 started:1,
 	   removed:1,
	   sleep:1;

	void *drv_priv;
	struct phl_info_t *phl_info;
};

#ifdef DEBUG_PHL_RX
struct phl_rx_stats {
	u32 rx_isr;
	u32 phl_rx;
	u32 rx_type_all;
	u32 rx_type_wifi;
	u32 rx_type_ppdu;
	u32 rx_type_wp;
	u32 rx_type_c2h;
	u32 rx_amsdu;

	u32 rx_dont_reorder;
	u32 rx_put_reorder;

	u32 rx_drop_get;
	u32 rx_rdy_fail;
	u32 rxbd_fail;
	u32 rx_drop_reorder;
	u32 reorder_seq_less;
	u32 reorder_dup;
#ifdef PHL_RXSC_AMPDU
	u32 rxsc_ampdu[3];
#endif

	u32 rx_pkt_core;
	u32 rx_pktsz_phl;
	u32 rx_pktsz_core;
#ifdef CONFIG_DYNAMIC_RX_BUF
	u32 rxbuf_empty;
#endif
};
#endif /* DEBUG_PHL_RX */

struct macid_ctl_t {
	_os_lock lock;
	/*  used macid bitmap share for all wifi role */
	u32 used_map[PHL_MACID_MAX_ARRAY_NUM];
	u32 used_mld_map[PHL_MACID_MAX_ARRAY_NUM];
	u32 used_legacy_map[PHL_MACID_MAX_ARRAY_NUM];
	/* record bmc macid bitmap for all wifi role */
	u32 bmc_map[PHL_MACID_MAX_ARRAY_NUM];
	/* record used macid bitmap for each wifi role */
	u32 wifi_role_usedmap[MAX_WIFI_ROLE_NUMBER][PHL_MACID_MAX_ARRAY_NUM];
	/* record bmc TX macid for wifi role */
	u16 wrole_bmc[MAX_WIFI_ROLE_NUMBER][RTW_RLINK_MAX];
	/* record total stainfo by macid */
	struct rtw_phl_stainfo_t *sta[PHL_MACID_MAX_NUM];
	u16 max_num;
};

struct stainfo_ctl_t {
	struct phl_info_t *phl_info;
	u8 *allocated_stainfo_buf;
	int allocated_stainfo_sz;
	u8 *stainfo_buf;
	struct phl_queue free_sta_queue;
};

struct mld_ctl_t {
	struct phl_info_t *phl_info;
	u8 *allocated_mld_buf;
	int allocated_mld_sz;
	u8 *mld_buf;
	struct phl_queue free_mld_queue;
};

struct phl_h2c_pkt_pool {
	struct rtw_h2c_pkt *h2c_pkt_buf;
	struct phl_queue idle_h2c_pkt_cmd_list;
	struct phl_queue idle_h2c_pkt_data_list;
	struct phl_queue idle_h2c_pkt_ldata_list;
	struct phl_queue busy_h2c_pkt_list;
	_os_lock recycle_lock;
};

struct gtimer_ctx {
	u8 en;
	u8 timer_type;
	u32 duration;
};

#define _GT3_ENABLE 1
#define _GT3_DISABLE 0

#define _GT3_TYPE_SH_TASK 1

struct lifetime_ctx {
	u8 hw_band;
	u8 en;
	u16 val;
};

#define _LIFETIME_ENABLE 1
#define _LIFETIME_DISABLE 0

struct power_offset_ctx {
	u8 hw_band;
	s8 ofst_mode;
	s8 ofst_bw;
};

enum phl_tx_status {
	PHL_TX_STATUS_IDLE = 0,
	PHL_TX_STATUS_RUNNING = 1,
	PHL_TX_STATUS_STOP_INPROGRESS = 2,
	PHL_TX_STATUS_SW_PAUSE = 3,
	PHL_TX_STATUS_MAX = 0xFF
};

enum phl_rx_status {
	PHL_RX_STATUS_IDLE = 0,
	PHL_RX_STATUS_RUNNING = 1,
	PHL_RX_STATUS_STOP_INPROGRESS = 2,
	PHL_RX_STATUS_SW_PAUSE = 3,
	PHL_RX_STATUS_MAX = 0xFF
};

enum data_ctrl_mdl {
	DATA_CTRL_MDL_NONE = 0,
	DATA_CTRL_MDL_CMD_CTRLER = BIT0,
	DATA_CTRL_MDL_SER = BIT1,
	DATA_CTRL_MDL_PS = BIT2,
	DATA_CTRL_MDL_MRC = BIT3,
	DATA_CTRL_MDL_MAX = BIT7
};

enum data_ctrl_err_code {
	CTRL_ERR_SW_TX_PAUSE_POLLTO = 1,
	CTRL_ERR_SW_TX_PAUSE_FAIL = 2,
	CTRL_ERR_SW_TX_RESUME_FAIL = 3,
	CTRL_ERR_SW_RX_PAUSE_POLLTO = 4,
	CTRL_ERR_SW_RX_PAUSE_FAIL = 5,
	CTRL_ERR_SW_RX_RESUME_FAIL = 6,
	CTRL_ERR_HW_TRX_PAUSE_FAIL = 7,
	CTRL_ERR_HW_TRX_RESUME_FAIL = 8,
	CTRL_ERR_MAX = 0xFF
};

#ifdef CONFIG_POWER_SAVE
struct phl_ps_info {
	bool init;
	_os_atomic tx_ntfy;
};
#endif

#define PHL_CTRL_TX BIT0
#define PHL_CTRL_RX BIT1
#define PHL_CTRL_IN_PIPE BIT2
#define PHL_CTRL_OUT_PIPE BIT3
#define POLL_SW_TX_PAUSE_MAX_MS 500
#define POLL_SW_RX_PAUSE_MAX_MS 500

#if defined(CONFIG_VW_REFINE) || defined(CONFIG_ONE_TXQ)
#define WP_MAX_CNT 4096
#define WP_USED_SEQ 0xFFFF
#endif

struct phl_info_t {
	struct macid_ctl_t macid_ctrl;
	struct stainfo_ctl_t sta_ctrl;
	struct mld_ctl_t mld_ctrl;

	struct rtw_regulation regulation;

	struct rtw_phl_com_t *phl_com;
	struct rtw_phl_handler phl_tx_handler;
	struct rtw_phl_handler phl_rx_handler;
	struct rtw_phl_handler phl_event_handler;
	struct rtw_phl_rx_ring phl_rx_ring;
	_os_atomic phl_sw_tx_sts;
	_os_atomic phl_sw_tx_more;
	_os_atomic phl_sw_tx_req_pwr;
	_os_atomic phl_sw_rx_sts;
	_os_atomic phl_sw_rx_more;
	_os_atomic phl_sw_rx_req_pwr;
	_os_atomic is_hw_trx_pause;
	enum data_ctrl_mdl pause_tx_id;
	enum data_ctrl_mdl pause_rx_id;
	_os_lock t_ring_list_lock;
	_os_lock rx_ring_lock;
	_os_lock t_fctrl_result_lock;
	_os_lock t_ring_free_list_lock;
	_os_list t_ring_list;
	_os_list t_fctrl_result;
	_os_list t_ring_free_list;
	void *ring_sts_pool;
	void *rx_pkt_pool;
	struct phl_h2c_pkt_pool *h2c_pool;

	struct hci_info_t *hci;
	struct phl_hci_trx_ops *hci_trx_ops;

	struct pkt_ofld_obj *pkt_ofld;

	struct phl_cmd_dispatch_engine disp_eng;
	struct phl_watchdog wdog;
	void *msg_hub;
	void *cmd_que;
	void *hal;

#ifdef CONFIG_FSM
	void *fsm_root;
	void *cmd_fsm;
	void *cmd_obj;

	void *ser_fsm;
	void *ser_obj;

	void *snd_fsm;
#endif /*CONFIG_FSM*/
	void *snd_obj;

	void *ps_obj;

	void *led_ctrl;

	void *ecsa_ctrl;
	void *phl_twt_info; /* struct phl_twt_info */
#ifdef PHL_RX_BATCH_IND
	u8 rx_new_pending;
#endif
#ifdef DEBUG_PHL_RX
	struct phl_rx_stats rx_stats;
	u32 cnt_rx_pktsz;
#endif

	struct phl_wow_info wow_info;

#ifdef CONFIG_POWER_SAVE
	struct phl_ps_info ps_info;
#endif

#ifdef CONFIG_RTW_ACS
	void *acs_info;
#endif

#ifdef CONFIG_PHL_TEST_SUITE
	void *trx_test;
#endif

	struct gtimer_ctx gt3_ctx;
	struct lifetime_ctx lt_ctx;
	struct power_offset_ctx pwr_ofst_ctx;

#if defined(CONFIG_VW_REFINE) || defined(CONFIG_ONE_TXQ)
	u16 free_wp[WP_MAX_CNT];
	u16 fw_ptr, fr_ptr;  /* wp management r/w ptr */
#endif
	u8 use_onetxring;

};

#define phl_to_drvpriv(_phl)		(_phl->phl_com->drv_priv)

#define phlcom_to_test_mgnt(_phl_com)	((_phl_com)->test_mgnt)
#define phlcom_to_mr_ctrl(_phl_com)	(&(_phl_com->mr_ctrl))

#define phl_to_mr_ctrl(_phl)	(&(((struct phl_info_t *)_phl)->phl_com->mr_ctrl))
#define phl_to_mac_ctrl(_phlinfo)	(&(_phlinfo->macid_ctrl))
#define phl_to_sta_ctrl(_phlinfo)	(&(_phlinfo->sta_ctrl))

#define get_band_ctrl(_phl, _band)	(&(phl_to_mr_ctrl(_phl)->band_ctrl[_band]))

#define phl_to_p2pps_info(_phl)	(((_phl)->phl_com->p2pps_info))
#define get_role_idx(_wrole) (_wrole->id)

#define phl_to_mld_ctrl(_phlinfo) (&(_phlinfo->mld_ctrl))
#define get_rlink(_wrole, _idx) (&(_wrole->rlink[_idx]))

#endif /*_PHL_STRUCT_H_*/
