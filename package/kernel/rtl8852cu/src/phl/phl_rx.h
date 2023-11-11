/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#ifndef _PHL_RX_H_
#define _PHL_RX_H_

struct phl_rx_pkt_pool {
	struct rtw_phl_rx_pkt phl_rx[MAX_PHL_RING_RX_PKT_NUM];
	_os_list idle;
	_os_list busy;
	_os_lock idle_lock;	/* spinlock */
	_os_lock busy_lock;	/* spinlock */
	u32 idle_cnt;
};

enum rtw_phl_status phl_rx_init(struct phl_info_t *phl_info);
void phl_rx_deinit(struct phl_info_t *phl_info);

u8 phl_check_recv_ring_resource(struct phl_info_t *phl_info);
struct rtw_phl_rx_pkt *rtw_phl_query_phl_rx(void *phl);
u8 rtw_phl_is_phl_rx_idle(struct phl_info_t *phl_info);
void phl_release_phl_rx(struct phl_info_t *phl_info,
			struct rtw_phl_rx_pkt *phl_rx);
enum rtw_phl_status phl_rx_reorder(struct phl_info_t *phl_info,
                                   struct rtw_phl_rx_pkt *phl_rx,
                                   _os_list *frames);
void phl_rx_deinit(struct phl_info_t *phl_info);
void phl_recycle_rx_buf(struct phl_info_t *phl_info,
				struct rtw_phl_rx_pkt *phl_rx);
void phl_event_indicator(void *context);

enum rtw_phl_status rtw_phl_start_rx_process(void *phl);


void _phl_indic_new_rxpkt(struct phl_info_t *phl_info);

void
phl_handle_rx_frame_list(struct phl_info_t *phl_info,
                         _os_list *frames);
void phl_sta_rx_reorder_timer_expired(void *t);
void phl_rx_traffic_upd(struct rtw_stats *sts);
void phl_rx_watchdog(struct phl_info_t *phl_info);

#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
enum rtw_phl_status
phl_rx_proc_phy_sts(struct phl_info_t *phl_info, struct rtw_phl_rx_pkt *ppdu_sts);
bool
phl_rx_proc_wait_phy_sts(struct phl_info_t *phl_info,
			 struct rtw_phl_rx_pkt *phl_rx);
#endif

void phl_rx_proc_ppdu_sts(struct phl_info_t *phl_info,
			  struct rtw_phl_rx_pkt *phl_rx);
void phl_rx_wp_report_record_sts(struct phl_info_t *phl_info,
				 u8 macid, u16 ac_queue, u8 txsts);
void phl_handle_queued_rx(struct phl_info_t *phl_info);
void phl_reset_rx_stats(struct rtw_stats *stats);
void phl_dump_all_sta_rx_info(struct phl_info_t *phl_info);
u16 rtw_phl_query_new_rx_num(void *phl);
void phl_rx_dbg_dump(struct phl_info_t *phl_info, u8 band_idx);

enum rtw_phl_status
phl_cmd_get_rx_cnt_by_idx_hdl(struct phl_info_t *phl_info, u8 *cmd);
enum rtw_phl_status
phl_cmd_set_reset_rx_cnt_hdl(struct phl_info_t *phl_info, u8 *cmd);

#ifdef CONFIG_PCI_HCI
u32 rtw_phl_get_hw_cnt_rdu(void *phl);
#endif

#endif /*_PHL_RX_H_*/
