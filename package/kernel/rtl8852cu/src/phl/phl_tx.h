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
#ifndef __PHL_TX_H_
#define __PHL_TX_H_

void phl_datapath_deinit(struct phl_info_t *phl_info);
enum rtw_phl_status phl_datapath_init(struct phl_info_t *phl_info);
enum rtw_phl_status phl_datapath_start_hw(struct phl_info_t *phl_info);
void phl_datapath_stop_hw(struct phl_info_t *phl_info);
void phl_datapath_reset(struct phl_info_t *phl_info, u8 type);
enum rtw_phl_status phl_datapath_start_sw(struct phl_info_t *phl_info, enum phl_module_id id);
void phl_datapath_stop_sw(struct phl_info_t *phl_info, enum phl_module_id id);

void phl_trx_free_handler(void *phl);
void phl_trx_free_sw_rsc(void *phl);

bool phl_datapath_chk_trx_pause(struct phl_info_t *phl_info, u8 type);

struct phl_ring_status *phl_alloc_ring_sts(struct phl_info_t *phl_info);
void phl_release_ring_sts(struct phl_info_t *phl_info,
				struct phl_ring_status *ring_sts);

u8 phl_check_xmit_ring_resource(struct phl_info_t *phl_info,
				_os_list *sta_list);
void phl_tx_flow_ctrl(struct phl_info_t *phl_info, _os_list *sta_list);
enum rtw_phl_status rtw_phl_tx_req_notify(void *phl);
enum rtw_phl_status phl_register_handler(struct rtw_phl_com_t *phl_com,
				struct rtw_phl_handler *handler);
enum rtw_phl_status phl_deregister_handler(struct rtw_phl_com_t *phl_com,
				struct rtw_phl_handler *handler);
enum rtw_phl_status phl_schedule_handler(struct rtw_phl_com_t *phl_com,
				struct rtw_phl_handler *handler);
enum rtw_phl_status phl_indic_pkt_complete(void *phl);

enum rtw_phl_status phl_register_tx_ring(void *phl, u16 macid, u8 hw_band, u8 hw_wmm, u8 hw_port);
enum rtw_phl_status phl_deregister_tx_ring(void *phl, u16 macid);
enum rtw_phl_status phl_re_register_tx_ring(void *phl, u16 macid, u8 hw_band, u8 hw_wmm, u8 hw_port);

void phl_free_deferred_tx_ring(struct phl_info_t *phl_info);

enum rtw_phl_status phl_enqueue_busy_h2c_pkt(struct phl_info_t *phl_info,
				struct rtw_h2c_pkt *h2c_pkt, u8 pos);

enum rtw_phl_status phl_enqueue_idle_h2c_pkt(struct phl_info_t *phl_info,
				struct rtw_h2c_pkt *h2c_pkt);

struct rtw_h2c_pkt *phl_query_busy_h2c_pkt(struct phl_info_t *phl_info);
struct rtw_h2c_pkt *phl_query_idle_h2c_pkt(struct phl_info_t *phl_info, u8 type);

/**
 * this function will be used in read / write pointer mechanism and
 * return the number of available read pointer
 * @rptr: input, the read pointer
 * @wptr: input, the write pointer
 * @bndy: input, the boundary of read / write pointer mechanism
 */
u16 phl_calc_avail_rptr(u16 rptr, u16 wptr, u16 bndy);

/**
 * this function will be used in read / write pointer mechanism and
 * return the number of available write pointer
 * @rptr: input, the read pointer
 * @wptr: input, the write pointer
 * @bndy: input, the boundary of read / write pointer mechanism
 */
u16 phl_calc_avail_wptr(u16 rptr, u16 wptr, u16 bndy);

void phl_dump_sorted_ring(_os_list *sorted_ring);
void phl_dump_tx_plan(_os_list *sta_list);
void phl_dump_t_fctrl_result(_os_list *t_fctrl_result);
const char *phl_tfc_lvl_to_str(u8 lvl);
void phl_tx_traffic_upd(struct rtw_stats *sts);
void phl_tx_watchdog(struct phl_info_t *phl_info);
void phl_reset_tx_stats(struct rtw_stats *stats);
void phl_dump_h2c_pool_stats(struct phl_h2c_pkt_pool *h2c_pkt_pool);

enum rtw_phl_status
phl_cmd_cfg_ampdu_hdl(struct phl_info_t *phl_info, u8 *param);

enum rtw_phl_status
phl_cmd_cfg_amsdu_tx_hdl(struct phl_info_t *phl_info, u8 *param);

enum rtw_phl_status
phl_cmd_get_hwseq_hdl(struct phl_info_t *phl_info, u8 *param);

enum rtw_phl_status
phl_data_ctrler(struct phl_info_t *phl_info, struct phl_data_ctl_t *ctl,
		struct phl_msg *msg);

void rtw_phl_tx_stop(void *phl);
void rtw_phl_tx_resume(void *phl);
u8 rtw_phl_cvt_cat_to_tid(enum rtw_phl_ring_cat cat);

enum rtw_phl_status
phl_cmd_cfg_hw_seq_hdl(struct phl_info_t *phl_info, u8 *param);

enum rtw_phl_status
phl_cmd_cfg_gt3_hdl(struct phl_info_t *phl_info, u8 *param);

enum rtw_phl_status
phl_cmd_cfg_lifetime_hdl(struct phl_info_t *phl_info, u8 *param);

enum rtw_phl_status
phl_cmd_cfg_power_offset_hdl(struct phl_info_t *phl_info, u8 *param);

void rtw_phl_tx_packet_event_notify(void *phl,
	struct rtw_wifi_role_link_t *rlink,
	enum phl_pkt_evt_type pkt_type);
void rtw_phl_packet_event_notify(void *phl,
	struct rtw_wifi_role_link_t *rlink,
        enum phl_pkt_evt_type pkt_evt);
enum rtw_phl_status
phl_cmd_cfg_hw_cts2self_hdl(struct phl_info_t *phl_info, u8 *param);
void phl_tx_dbg_status_dump(struct phl_info_t *phl_info, enum phl_band_idx hwband);
#endif	/* __PHL_TX_H_ */
