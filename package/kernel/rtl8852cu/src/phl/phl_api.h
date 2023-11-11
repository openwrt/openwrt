/******************************************************************************
 *
 * Copyright(c) 2019 - 2022 Realtek Corporation.
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
#ifndef _PHL_API_H_
#define _PHL_API_H_

u8 rtw_phl_read8(void *phl, u32 addr);
u16 rtw_phl_read16(void *phl, u32 addr);
u32 rtw_phl_read32(void *phl, u32 addr);
void rtw_phl_write8(void *phl, u32 addr, u8 val);
void rtw_phl_write16(void *phl, u32 addr, u16 val);
void rtw_phl_write32(void *phl, u32 addr, u32 val);

u32 rtw_phl_read_macreg(void *phl, u32 offset, u32 bit_mask);
void rtw_phl_write_macreg(void *phl,
			u32 offset, u32 bit_mask, u32 data);
u32 rtw_phl_read_bbreg(void *phl, u32 offset, u32 bit_mask);
void rtw_phl_write_bbreg(void *phl,
			u32 offset, u32 bit_mask, u32 data);
u32 rtw_phl_read_rfreg(void *phl,
			enum rf_path path, u32 offset, u32 bit_mask);
void rtw_phl_write_rfreg(void *phl,
			enum rf_path path, u32 offset, u32 bit_mask, u32 data);
enum rtw_phl_status rtw_phl_interrupt_handler(void *phl);
void rtw_phl_enable_interrupt(void *phl);
void rtw_phl_disable_interrupt_isr(void *phl);
void rtw_phl_disable_interrupt(void *phl);
bool rtw_phl_recognize_interrupt(void *phl);
void rtw_phl_clear_interrupt(void *phl);
void rtw_phl_restore_interrupt(void *phl);

enum rtw_phl_status
rtw_phl_cmd_cfg_hw_seq(void *phl,
                       struct rtw_wifi_role_link_t *rlink,
                       struct rtw_phl_stainfo_t *sta,
                       u32 seq,
                       u16 rts_rate,
                       enum phl_cmd_type cmd_type,
                       u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cfg_gtimer_register(void *phl,
                            struct rtw_wifi_role_link_t *rlink,
                            u8 type,
                            u8 enable,
                            u32 timeout,
                            enum phl_cmd_type cmd_type,
                            u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_cfg_lifetime(void *phl,
                         struct rtw_wifi_role_link_t *rlink,
                         u8 enable,
                         u16 acq_val,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_cfg_power_offset(void *phl,
                             struct rtw_wifi_role_link_t *rlink,
                             s8 ofst_mode,
                             s8 ofst_bw,
                             enum phl_cmd_type cmd_type,
                             u32 cmd_timeout);

#ifdef PHL_PLATFORM_LINUX
void rtw_phl_mac_reg_dump(void *sel, void *phl);
void rtw_phl_bb_reg_dump(void *sel, void *phl);
void rtw_phl_bb_reg_dump_ex(void *sel, void *phl);
void rtw_phl_rf_reg_dump(void *sel, void *phl);
#endif
bool rtw_phl_get_sec_cam(void *phl, u16 num, u8 *buf, u16 size);
bool rtw_phl_get_addr_cam(void *phl, u16 num, u8 *buf, u16 size);

struct rtw_phl_com_t *rtw_phl_get_com(void *phl);
enum rtw_phl_status rtw_phl_init(void *drv_priv, void **phl,
					struct rtw_ic_info *ic_info);
void rtw_phl_deinit(void *phl);
#ifdef PHL_WATCHDOG_REFINE
void rtw_phl_watchdog_init(void *phl,
                           u16 period,
                           void (*core_sw_wdog)(void *drv_priv),
                           void (*core_hw_wdog)(void *drv_priv),
                           void (*core_sw_post_wdog)(void *drv_priv));
#else
void rtw_phl_watchdog_init(void *phl,
                           u16 period,
                           void (*core_sw_wdog)(void *drv_priv),
                           void (*core_hw_wdog)(void *drv_priv));
#endif
void rtw_phl_watchdog_deinit(void *phl);
void rtw_phl_watchdog_start(void *phl);
void rtw_phl_watchdog_stop(void *phl);

enum rtw_phl_status rtw_phl_trx_alloc(void *phl);
void rtw_phl_trx_free(void *phl);
void rtw_phl_trx_free_handler(void *phl);
void rtw_phl_trx_free_sw_rsc(void *phl);
void rtw_phl_cap_pre_config(void *phl);
enum rtw_phl_status rtw_phl_preload(void *phl);
enum rtw_phl_status rtw_phl_start(void *phl);
void rtw_phl_stop(void *phl);
enum rtw_phl_status rtw_phl_pnp_stop(void *phl);
bool rtw_phl_is_init_completed(void *phl);

void rtw_phl_ps_set_rt_cap(void *phl, u8 band_idx, bool ps_allow, enum phl_ps_rt_rson rt_rson);
void rtw_phl_dbg_ps_op_mode(void *phl, u8 band_idx, u8 ps_mode, u8 ps_op_mode);
void rtw_phl_dbg_ps_cap(void *phl, u8 band_idx, u8 ps_mode, u8 ps_cap);
enum rtw_phl_status rtw_phl_ps_set_rf_state(void *phl, u8 band_idx, enum rtw_rf_state rf_state);

enum rtw_phl_status rtw_phl_suspend(void *phl, struct rtw_phl_stainfo_t *sta, u8 wow_en);
enum rtw_phl_status rtw_phl_resume(void *phl, struct rtw_phl_stainfo_t *sta, u8 *hw_reinit);

#ifdef CONFIG_PHL_HANDLE_SER_L2
enum rtw_phl_status rtw_phl_suspend_all_sta(void *phl, enum phl_cmd_type cmd_type);
enum rtw_phl_status rtw_phl_resume_all_sta(void *phl, enum phl_cmd_type cmd_type);
#endif
enum rtw_phl_status rtw_phl_ser_l2_done_notify(struct rtw_phl_com_t *phl_com);

enum rtw_phl_status rtw_phl_tx_req_notify(void *phl);
enum rtw_phl_status rtw_phl_add_tx_req(void *phl, struct rtw_xmit_req *tx_req);
void rtw_phl_tx_stop(void *phl);
void rtw_phl_tx_resume(void *phl);
u16 rtw_phl_tring_rsc(void *phl, u16 macid, u8 tid);
#ifdef CONFIG_PHL_PCI_TRX_RES_DBG
void rtw_phl_get_txbd(void *phl, u8 ch, u16 *host_idx, u16 *hw_idx, u16 *hw_res);
u8 rtw_phl_get_txch_num(void *phl);
void rtw_phl_get_rxbd(void *phl, u8 ch, u16 *host_idx, u16 *hw_idx, u16 *hw_res);
u8 rtw_phl_get_rxch_num(void *phl);
#endif /*  CONFIG_PHL_PCI_TRX_RES_DBG */
u8 rtw_phl_cvt_cat_to_tid(enum rtw_phl_ring_cat cat);
enum rtw_phl_ring_cat rtw_phl_cvt_tid_to_cat(u8 tid);

u16 rtw_phl_query_new_rx_num(void *phl);
struct rtw_recv_pkt *rtw_phl_query_rx_pkt(void *phl);

void rtw_phl_rx_deferred_In_token(void *phl);
void rtw_phl_post_in_complete(void *phl, void *rxobj, u32 inbuf_len, u8 status_code);
enum rtw_phl_status rtw_phl_return_rxbuf(void *phl, u8* rxpkt);

#ifdef CONFIG_PHL_USB_RX_AGGREGATION
enum rtw_phl_status
rtw_phl_cmd_usb_rx_agg_cfg(void *phl,
			   enum phl_usb_rx_agg_mode mode,
			   u8 agg_mode,
			   u8 drv_define,
			   u8 timeout,
			   u8 size,
			   u8 pkt_num,
			   enum phl_cmd_type cmd_type,
			   u32 cmd_timeout);
#endif

enum rtw_phl_status  rtw_phl_recycle_tx_buf(void *phl, u8 *tx_buf_ptr);

enum rtw_phl_status
rtw_phl_cmd_cfg_ampdu(void *phl,
			struct rtw_wifi_role_t *wrole,
			struct rtw_phl_stainfo_t *sta,
			enum phl_cmd_type cmd_type,
			u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_cfg_amsdu_tx(struct rtw_wifi_role_t *wrole,
	struct rtw_phl_stainfo_t *sta,
	struct rtw_amsdu_tx_param *input_param,
	bool cmd_wait,
	u32 cmd_timeout);

void rtw_phl_cfg_hwamsdu_init(void *phl,
	u8 hwamsdu_en,
	u8 pkt_num,
	u8 single_amsdu_en,
	u8 last_padding_en);

enum rtw_phl_status
rtw_phl_get_hw_sequence(void *phl,
			u16 *cur_hw_seq,
			u16 macid,
			u8 hw_ssn_mode,
			u8 hw_ssn_sel);

void rtw_phl_proc_cmd(void *phl, char proc_cmd,
	struct rtw_proc_cmd *incmd, char *output, u32 out_len);

void rtw_phl_get_halmac_ver(char *buf, u16 buf_len);
void rtw_phl_get_fw_ver(void *phl, char *ver_str, u16 len);

enum rtw_fw_status rtw_phl_get_fw_status(void *phl);

/* command thread jobs */
enum rtw_phl_status rtw_phl_job_run_func(void *phl,
	void *func, void *priv, void *parm, char *name);

/*WIFI Role management section*/
u8 rtw_phl_wifi_role_alloc(void *phl,
                           u8 *mac_addr,
                           u8 **rlink_mac_addr,
                           enum role_type rtype,
                           u8 ridx,
                           struct rtw_wifi_role_t **wifi_role,
                           enum rtw_device_type dtype,
                           bool ignore_hw_fail);

enum rtw_phl_status
rtw_phl_cmd_wrole_change(void *phl,
                         struct rtw_wifi_role_t *wrole,
                         struct rtw_wifi_role_link_t *rlink,
                         enum wr_chg_id chg_id,
                         u8 *chg_info,
                         u8 chg_info_len,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout
);

void rtw_phl_wifi_role_free(void *phl, u8 role_idx);

/*WIFI sta_info management section*/
struct rtw_phl_stainfo_t *
rtw_phl_alloc_stainfo_sw(void *phl,
                         u8 *sta_addr,
                         struct rtw_wifi_role_t *wrole,
                         enum rtw_device_type dtype,
                         u16 main_id,
                         struct rtw_wifi_role_link_t *rlink);

enum rtw_phl_status
rtw_phl_free_stainfo_sw(void *phl, struct rtw_phl_stainfo_t *sta);

enum rtw_phl_status
rtw_phl_cmd_alloc_stainfo(void *phl,
                          struct rtw_phl_stainfo_t **sta,
                          u8 *sta_addr,
                          struct rtw_wifi_role_t *wrole,
                          enum rtw_device_type dtype,
                          u16 main_id,
                          struct rtw_wifi_role_link_t *rlink,
                          bool alloc,
                          bool only_hw,
                          enum phl_cmd_type cmd_type,
                          u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_change_stainfo(void *phl,
	struct rtw_phl_stainfo_t *sta, enum sta_chg_id chg_id,
	u8 *chg_info, u8 chg_info_len,
	enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_update_media_status(void *phl,
                                struct rtw_phl_stainfo_t *sta,
                                u8 *sta_addr,
                                bool is_connect,
                                enum phl_cmd_type cmd_type,
                                u32 cmd_timeout);

struct rtw_phl_stainfo_t *
rtw_phl_get_stainfo_self(void *phl, struct rtw_wifi_role_link_t *rlink);

struct rtw_phl_stainfo_t *
rtw_phl_get_stainfo_by_addr(void *phl,
                            struct rtw_wifi_role_t *wrole,
                            struct rtw_wifi_role_link_t *rlink,
                            u8 *addr);

struct rtw_phl_stainfo_t *
rtw_phl_get_stainfo_by_macid(void *phl, u16 macid);

u8
rtw_phl_get_sta_rssi(struct rtw_phl_stainfo_t *sta);

enum rtw_phl_status
rtw_phl_query_rainfo(void *phl, struct rtw_phl_stainfo_t *phl_sta,
		     struct rtw_phl_rainfo *ra_info);
enum rtw_phl_status
rtw_phl_get_rx_stat(void *phl, struct rtw_phl_stainfo_t *phl_sta,
		     u16 *rx_rate, u8 *bw, u8 *gi_ltf);

void rtw_phl_sta_up_rx_bcn(void *phl, struct rtw_bcn_pkt_info *info);

/*macid management section, temporary for debuge*/
u16
rtw_phl_get_macid_max_num(void *phl);

u16
rtw_phl_wrole_bcmc_id_get(void *phl,
                          struct rtw_wifi_role_t *wrole,
                          struct rtw_wifi_role_link_t *rlink);

u8
rtw_phl_macid_is_bmc(void *phl, u16 macid);

u8
rtw_phl_macid_is_used(void *phl, u16 macid);

enum rtw_phl_status
rtw_phl_cmd_add_key(void *phl,
                    struct rtw_phl_stainfo_t *sta,
                    struct phl_sec_param_h *crypt,
                    u8 *keybuf,
                    enum phl_cmd_type cmd_type,
                    u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_del_key(void *phl,
                    struct rtw_phl_stainfo_t *sta,
                    struct phl_sec_param_h *crypt,
                    enum phl_cmd_type cmd_type,
                    u32 cmd_timeout);
/* phy msg forwarder functions*/
enum rtw_phl_status rtw_phl_msg_hub_register_recver(void* phl,
		struct phl_msg_receiver* ctx, enum phl_msg_recver_layer layer);
enum rtw_phl_status rtw_phl_msg_hub_update_recver_mask(void* phl,
		enum phl_msg_recver_layer layer, u8* mdl_id, u8 len, u8 clr);
enum rtw_phl_status rtw_phl_msg_hub_deregister_recver(void* phl,
					enum phl_msg_recver_layer layer);
enum rtw_phl_status rtw_phl_msg_hub_send(void* phl,
		struct phl_msg_attribute* attr, struct phl_msg* msg);


u8 rtw_phl_trans_sec_mode(u8 unicast, u8 multicast);

u8 rtw_phl_get_sec_cam_idx(void *phl, struct rtw_phl_stainfo_t *sta,
			u8 keyid, u8 key_type);

void rtw_phl_test_txtb_cfg(struct rtw_phl_com_t* phl_com, void *buf,
	u32 buf_len, u8 *cfg_bssid, u16 cfg_aid, u8 cfg_bsscolor);
/* command dispatcher module section*/
enum rtw_phl_status rtw_phl_register_module(void *phl, u8 band_idx,
					enum phl_module_id id,
					struct phl_bk_module_ops* ops);
enum rtw_phl_status rtw_phl_deregister_module(void *phl,u8 band_idx,
					enum phl_module_id id);
u8 rtw_phl_is_fg_empty(void *phl, u8 band_idx);
/* opt: refer to enum phl_msg_opt */
enum rtw_phl_status rtw_phl_send_msg_to_dispr(void *phl, struct phl_msg* msg,
					      struct phl_msg_attribute* attr, u32* msg_hdl);
enum rtw_phl_status rtw_phl_cancel_dispr_msg(void *phl, u8 band_idx, u32* msg_hdl);

enum rtw_phl_status rtw_phl_add_cmd_token_req(void *phl, u8 band_idx,
				struct phl_cmd_token_req* req, u32* req_hdl);
enum rtw_phl_status rtw_phl_cancel_cmd_token(void *phl, u8 band_idx, u32* req_hdl);
enum rtw_phl_status rtw_phl_set_cur_cmd_info(void *phl, u8 band_idx,
					       struct phl_module_op_info* op_info);
enum rtw_phl_status rtw_phl_query_cur_cmd_info(void *phl, u8 band_idx,
					       struct phl_module_op_info* op_info);

enum rtw_phl_status rtw_phl_free_cmd_token(void *phl, u8 band_idx, u32* req_hdl);
enum rtw_phl_status rtw_phl_set_bk_module_info(void *phl, u8 band_idx,
		enum phl_module_id id,	struct phl_module_op_info* op_info);
enum rtw_phl_status rtw_phl_query_bk_module_info(void *phl, u8 band_idx,
		enum phl_module_id id,	struct phl_module_op_info* op_info);
enum rtw_phl_status rtw_phl_set_msg_disp_seq(void *phl,
						struct phl_msg_attribute *attr,
						struct msg_self_def_seq* seq);

void rtw_phl_dump_mdl(struct phl_msg *msg, const char *caller);

/* BA session management */
void rtw_phl_stop_rx_ba_session(void *phl, struct rtw_phl_stainfo_t *sta,
				u16 tid);
enum rtw_phl_status
rtw_phl_start_rx_ba_session(void *phl, struct rtw_phl_stainfo_t *sta,
			    u8 dialog_token, u16 timeout, u16 start_seq_num,
			    u16 ba_policy, u16 tid, u16 buf_size);
void rtw_phl_rx_bar(void *phl, struct rtw_phl_stainfo_t *sta, u8 tid, u16 seq);
void rtw_phl_flush_reorder_buf(void *phl, struct rtw_phl_stainfo_t *sta);
enum rtw_phl_status
rtw_phl_enter_mon_mode(void *phl, struct rtw_wifi_role_t *wrole);
enum rtw_phl_status
rtw_phl_leave_mon_mode(void *phl, struct rtw_wifi_role_t *wrole);
#ifdef RTW_PHL_BCN
enum rtw_phl_status
rtw_phl_free_bcn_entry(void *phl,
                       struct rtw_wifi_role_link_t *rlink);

enum rtw_phl_status
rtw_phl_cmd_issue_beacon(void *phl,
                         struct rtw_wifi_role_link_t *rlink,
                         struct rtw_bcn_info_cmn *bcn_cmn,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_stop_beacon(void *phl,
                        struct rtw_wifi_role_link_t *rlink,
                        u8 stop,
                        enum phl_cmd_type cmd_type,
                        u32 cmd_timeout);
#ifdef CONFIG_RTW_DEBUG_BCN_TX
enum rtw_phl_status rtw_phl_get_beacon_cnt(void *phl,
				u8 bcn_id, struct rtw_bcn_stats **bcn_stats);
#endif
#endif

#ifdef CONFIG_CMD_DISP

enum rtw_phl_status
rtw_phl_cmd_chg_op_chdef(struct rtw_wifi_role_t *wrole,
                         struct rtw_wifi_role_link_t *rlink,
                         struct rtw_chan_def *new_chdef,
                         bool cmd_wait,
                         u32 cmd_timeout,
                         void (*chg_opch_done)(void *priv,
                                               u8 ridx,
                                               struct rtw_wifi_role_link_t *rlink,
                                               enum rtw_phl_status status)
);

enum rtw_phl_status
rtw_phl_cmd_set_ch_bw(struct rtw_wifi_role_t *wifi_role,
                      struct rtw_wifi_role_link_t *rlink,
                      struct rtw_chan_def *chdef,
                      bool do_rfk,
                      enum phl_cmd_type cmd_type,
                      u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_set_macid_pause(struct rtw_wifi_role_t *wifi_role,
                      struct rtw_phl_stainfo_t *phl_sta, bool pause,
                      enum phl_cmd_type cmd_type,
                      u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_set_macid_pkt_drop(struct rtw_wifi_role_t *wifi_role,
                      struct rtw_phl_stainfo_t *phl_sta, u8 sel,
                      enum phl_cmd_type cmd_type,
                      u32 cmd_timeout);

#ifdef CONFIG_PHL_DFS
enum rtw_phl_status
rtw_phl_cmd_dfs_change_domain(void *phl_info, enum phl_band_idx hw_band
	, enum dfs_regd_t domain, enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_enable_all_range(void *phl_info, enum phl_band_idx hw_band
	, enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_enable_with_sp_chbw(void *phl_info, enum phl_band_idx hw_band
	, bool cac, u8 sp_ch, enum channel_width sp_bw, enum chan_offset sp_offset
	, enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_enable_with_sp_freq_range(void *phl_info, enum phl_band_idx hw_band
	, bool cac, u32 sp_freq_hi, u32 sp_freq_lo
	, enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_set_cac_status(void *phl_info, enum phl_band_idx hw_band
	, bool cac, enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_dfs_rd_disable(void *phl_info, enum phl_band_idx hw_band
	, enum phl_cmd_type cmd_type, u32 cmd_timeout);
#endif /* CONFIG_PHL_DFS */

enum rtw_phl_status
rtw_phl_cmd_enqueue(void *phl,
                    enum phl_band_idx band_idx,
                    enum phl_msg_evt_id evt_id,
                    u8 *cmd_buf,
                    u32 cmd_len,
                    void (*core_cmd_complete)(void *priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status),
                    enum phl_cmd_type cmd_type,
                    u32 cmd_timeout);

#endif /*CONFIG_CMD_DISP*/

u8 rtw_phl_get_cur_ch(struct rtw_wifi_role_t *wifi_role,
                      struct rtw_wifi_role_link_t *rlink);

enum rtw_phl_status
rtw_phl_get_cur_hal_chdef(struct rtw_wifi_role_t *wifi_role,
                          struct rtw_wifi_role_link_t *rlink,
                          struct rtw_chan_def *cur_chandef);

enum rtw_phl_status
rtw_phl_get_cur_hal_chdef_by_hwband(void *phl_info,
                          enum phl_band_idx band_idx,
                          struct rtw_chan_def *cur_chandef);


u8 rtw_phl_get_center_ch(struct rtw_chan_def *chan_def);

enum rtw_phl_status
rtw_phl_cmd_dfs_csa_tx_pause(void *phl_info,
                         enum phl_band_idx hw_band,
                         bool pause,
                         bool csa,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_dfs_csa_mg_tx_pause(void *phl_info,
                         enum phl_band_idx hw_band,
                         bool pause,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout);

u8
rtw_phl_get_operating_class(
	struct rtw_chan_def chan_def
);

bool
rtw_phl_get_chandef_from_operating_class(
	u8 channel,
	u8 operating_class,
	struct rtw_chan_def *chan_def
);
/*
 * export API from sw cap module
 */
void rtw_phl_final_cap_decision(void *phl);


enum rtw_phl_status
rtw_phl_get_dft_proto_cap(void *phl,
                          u8 hw_band,
                          enum role_type rtype,
                          struct protocol_cap_t *protocol_cap);

enum rtw_phl_status
rtw_phl_get_dft_cap(void *phl,
                    u8 hw_band,
                    struct role_link_cap_t *cap);

void rtw_phl_mac_dbg_status_dump(void *phl, u32 *val, u8 *en);

#ifdef CONFIG_DBCC_SUPPORT

void
rtw_phl_mr_cancel_dbcc_action(void *phl);

enum rtw_phl_status
rtw_phl_mr_trig_dbcc_enable(void *phl);
enum rtw_phl_status
rtw_phl_mr_trig_dbcc_disable(void *phl);

enum rtw_phl_status
rtw_phl_mr_dbcc_enable(void *phl, enum phl_band_idx band_idx,
				  struct rtw_wifi_role_t *wrole);
enum rtw_phl_status
rtw_phl_mr_dbcc_disable(void *phl, enum phl_band_idx band_idx,
				  struct rtw_wifi_role_t *wrole);
bool rtw_phl_mr_is_db(void *phl);
#endif

/*
 * API for config channel info CR
 */
#ifdef CONFIG_PHL_CHANNEL_INFO
enum rtw_phl_status rtw_phl_cmd_cfg_chinfo(void *phl,
	struct rtw_chinfo_action_parm *act_parm, enum phl_cmd_type cmd_type, u32 cmd_timeout);
enum rtw_phl_status rtw_phl_query_chan_info(void *phl, u32 buf_len,
	u8* chan_info_buffer, u32 *length, struct csi_header_t *csi_header);
#endif /* CONFIG_PHL_CHANNEL_INFO */

void rtw_phl_set_edcca_mode(void *phl, enum rtw_edcca_mode mode);
enum rtw_edcca_mode rtw_phl_get_edcca_mode(void *phl);

bool rtw_phl_set_user_def_chplan(void *phl, struct rtw_user_def_chplan *udef);
bool rtw_phl_valid_regulation_domain(u8 domain);
bool rtw_phl_valid_regulation_domain_6ghz(u8 domain);
bool rtw_phl_regulation_set_domain(void *phl, u8 domain,
				       	enum regulation_rsn reason);
bool rtw_phl_regulation_set_domain_ex(void *phl,
					struct rtw_regulatory_domain *domain,
				       	enum regulation_rsn reason);
bool rtw_phl_regulation_set_country(void *phl, char *country,
					enum regulation_rsn reason);
bool rtw_phl_regulation_set_capability(void *phl,
		enum rtw_regulation_capability capability);
bool rtw_phl_regulation_query_chplan(
			void *phl, enum rtw_regulation_query type,
			struct rtw_chlist *filter,
			struct rtw_regulation_chplan *plan);
bool rtw_phl_query_specific_chplan(void *phl, u8 domain,
			struct rtw_regulation_chplan *plan);
bool rtw_phl_query_domain_channel(u8 domain, enum band_type band, u8 ch,
			enum ch_property *prop);
bool rtw_phl_query_domain_6g_channel(u8 domain, enum band_type band, u8 ch,
			enum ch_property *prop);
bool rtw_phl_query_country_chplan(char *country,
			struct rtw_regulation_country_chplan *country_chplan);

bool rtw_phl_query_cntry_exist(char *cntry);
bool rtw_phl_query_group_list(void *phl, char *query_list, u32 max_item_num, u8 group_id);
u32 rtw_phl_query_group_length(void *phl, u8 group_id);
bool rtw_phl_regu_policy_set_6g_bp(void *phl,
			u8 mode, u8 *cntry_list, u32 len);
bool rtw_phl_regu_policy_query_info(void *phl,
			struct rtw_regu_policy_info *info, char *country);

bool rtw_phl_generate_scan_instance(struct instance_strategy *strategy,
				struct rtw_regulation_chplan *chplan,
				struct instance *inst);
bool rtw_phl_scan_instance_insert_ch(void *phl, struct instance *inst,
					enum band_type band, u8 channel,
						u8 strategy_period);
bool rtw_phl_regulation_valid_channel(void *phl, enum band_type band,
					u16 channel, u8 reject);
bool rtw_phl_regulation_dfs_channel(void *phl, enum band_type band,
					u16 channel, bool *dfs);
bool rtw_phl_query_regulation_info(void *phl, struct rtw_regulation_info *info);
bool rtw_phl_regulation_query_ch(void *phl, enum band_type band, u8 channel,
					struct rtw_regulation_channel *ch);
u8 rtw_phl_get_domain_regulation_2g(u8 domain);
u8 rtw_phl_get_domain_regulation_5g(u8 domain);
u8 rtw_phl_get_domain_regulation_6g(u8 domain);
u16 rtw_phl_get_regu_chplan_ver(void);
u16 rtw_phl_get_regu_country_ver(void);

enum rtw_phl_status rtw_phl_get_mac_addr_efuse(void* phl, u8 *addr);

#ifdef CONFIG_PHL_DRV_HAS_NVM
/* For external NVM system to create efuse map */
enum rtw_phl_status
rtw_phl_extract_efuse_info(void *phl, u8 *efuse_map,
                           u32 info_type,
                           void *value,
                           u8 size, u8 map_valid);

enum rtw_phl_status
rtw_phl_get_efuse_size(void *phl, u32 *log_efuse_size,
                       u32 *limit_efuse_size, u32 *mask_size,
                       u32 *limit_mask_size);

/* Core ask to apply dev_sw_cap set from NVM to dev's HW cap */
enum rtw_phl_status
rtw_phl_nvm_apply_dev_cap(void *phl);

#endif /* CONFIG_PHL_DRV_HAS_NVM */

/**
 * rtw_phl_usb_tx_ep_id - query  USB tx end point index
 * identified by macid, tid and band
 * @macid: input target macid is 0 ~ 127
* @tid: input target tid, range is 0 ~ 7
 * @band: input target band, 0 for band 0 / 1 for band 1
 *
 * returns corresponding end point idx of a specific tid
 */
u8 rtw_phl_usb_tx_ep_id(void *phl, u16 macid, u8 tid, u8 band);

enum rtw_phl_status
rtw_phl_cfg_trx_path(void* phl, enum rf_path tx, u8 tx_nss,
		     enum rf_path rx, u8 rx_nss);

void rtw_phl_reset_stat_ma_rssi(struct rtw_phl_com_t *phl_com);

u8
rtw_phl_get_ma_rssi(struct rtw_phl_com_t *phl_com,
		    enum rtw_rssi_type rssi_type);

bool rtw_phl_adjust_chandef(void *phl,
			      struct rtw_wifi_role_link_t *rlink,
			      struct rtw_chan_def *new_chdef);

bool rtw_phl_chanctx_chk(void *phl,
                         struct rtw_wifi_role_t *wifi_role,
                         struct rtw_wifi_role_link_t *rlink,
                         struct rtw_chan_def *new_chdef,
                         struct rtw_mr_chctx_info *mr_cc_info);

bool rtw_phl_chanctx_add(void *phl,
                         struct rtw_wifi_role_t *wifi_role,
                         struct rtw_wifi_role_link_t *rlink,
                         struct rtw_chan_def *new_chdef,
                         struct rtw_mr_chctx_info *mr_cc_info);

int rtw_phl_chanctx_del(void *phl,
                        struct rtw_wifi_role_t *wifi_role,
                        struct rtw_wifi_role_link_t *rlink,
                        struct rtw_chan_def *chan_def);

enum rtw_phl_status rtw_phl_chanctx_del_no_self(void *phl,
                            struct rtw_wifi_role_t *wifi_role,
                            struct rtw_wifi_role_link_t *rlink);

int rtw_phl_mr_get_chanctx_num(void *phl,
                               struct rtw_wifi_role_t *wifi_role,
                               struct rtw_wifi_role_link_t *rlink);

enum rtw_phl_status
rtw_phl_mr_get_chandef(void *phl,
                       struct rtw_wifi_role_t *wifi_role,
                       struct rtw_wifi_role_link_t *rlink,
                       struct rtw_chan_def *chandef);

int rtw_phl_mr_get_chandef_by_hwband(void *phl,
					enum phl_band_idx band_idx,
					struct rtw_chan_def *chandef);

enum rtw_phl_status rtw_phl_mr_upt_chandef(void *phl, struct rtw_wifi_role_link_t *rlink);

#ifdef CONFIG_MR_COEX_SUPPORT
u8 rtw_phl_mr_coex_query_inprogress(void *phl,
                                    u8 hw_band,
                                    enum rtw_phl_mr_coex_chk_inprocess_type check_type);
#endif /* CONFIG_MR_COEX_SUPPORT */

u8 rtw_phl_mr_dump_mac_addr(void *phl,
					struct rtw_wifi_role_t *wifi_role);
u8 rtw_phl_mr_buddy_dump_mac_addr(void *phl,
					struct rtw_wifi_role_t *wifi_role);

u8 rtw_phl_mr_process_by_band(void *phl,
		enum phl_band_idx band_idx, void *data,
		u8(*ops_func)(void *drv_priv, u8 wr_idx, void *data));

enum rtw_phl_status
rtw_phl_mr_offch_hdl(void *phl,
                     struct rtw_wifi_role_t *wrole,
                     struct rtw_wifi_role_link_t *rlink,
                     bool off_ch,
                     void *obj_priv,
                     bool (*issue_null_data)(void *priv, u8 ridx, bool ps),
                     struct rtw_chan_def *chandef);

void rtw_phl_mr_ops_init (void *phl, struct rtw_phl_mr_ops *mr_ops);

#ifdef	PHL_MR_PROC_CMD
void rtw_phl_mr_dump_info(void *phl, bool show_caller);
void rtw_phl_mr_dump_band_ctl(void *phl, bool show_caller);
bool rtw_phl_chanctx_test(void *phl,
                          struct rtw_wifi_role_t *wifi_role,
                          struct rtw_wifi_role_link_t *rlink,
                          u8 test_mode,
                          struct rtw_chan_def *new_chdef,
                          struct rtw_mr_chctx_info *mr_cc_info);

enum rtw_phl_status
rtw_phl_wifi_role_realloc_port(void *phl,
                               struct rtw_wifi_role_t *wrole,
                               struct rtw_wifi_role_link_t *rlink,
                               u8 new_port);
#endif
void rtw_phl_sta_dump_info(void *phl, bool show_caller, struct rtw_wifi_role_t *wr, u8 mode);

enum rtw_phl_status
rtw_phl_sta_assoc_cap_process(struct rtw_phl_stainfo_t *sta,
					      bool backup);

bool rtw_phl_mr_query_info(void *phl,
                           struct rtw_wifi_role_t *wrole,
                           struct rtw_wifi_role_link_t *rlink,
                           struct mr_query_info *info);

u8 rtw_phl_mr_get_opch_list(void *phl, u8 hw_band,
                            struct rtw_chan_def *chdef_list, u8 list_size,
                            struct rtw_wifi_role_link_t *rlink[], u8 rlink_num);

void rtw_phl_mr_dump_cur_chandef(void *phl, struct rtw_wifi_role_t *wifi_role);

enum mr_op_mode
rtw_phl_mr_get_opmode(void *phl,
                      struct rtw_wifi_role_t *wrole,
                      struct rtw_wifi_role_link_t *rlink);

void rtw_phl_led_set_ctrl_mode(void *phl, enum rtw_led_id led_id,
			       enum rtw_led_ctrl_mode ctrl_mode);
void rtw_phl_led_set_toggle_intervals(void *phl, u8 intervals_idx,
				      u32 *intervals, u8 intervals_len);
void rtw_phl_led_set_action(void *phl, enum rtw_led_event event,
			    enum rtw_led_state state_condition,
			    struct rtw_led_action_args_t *action_args_arr,
			    u8 action_args_arr_len, u32 toggle_delay_unit);
void rtw_phl_led_control(void *phl, enum rtw_led_event led_event);
void rtw_phl_led_manual_mode_switch(void *phl, enum rtw_led_id led_id,
				    bool manual_mode_on);
void rtw_phl_led_manual_control(void *phl, enum rtw_led_id led_id,
				enum rtw_led_opt opt);

/* This config function should only be called before general module is started */
void rtw_phl_pcie_trx_mit_cfg(void *phl,
			      struct rtw_pcie_trx_mit_info_t *mit_info);

#ifdef CONFIG_RTW_ACS
enum rtw_phl_status rtw_phl_get_acs_info(void *phl, struct rtw_acs_info_parm *parm);
u8 rtw_phl_get_acs_chnl_tbl_idx(void *phl, enum band_type band, u8 channel);
#endif /* CONFIG_RTW_ACS */

void rtw_phl_get_env_rpt(void *phl,
                         struct rtw_env_report *env_rpt,
                         u8 hw_band);

#ifdef RTW_WKARD_DYNAMIC_BFEE_CAP
enum rtw_phl_status
rtw_phl_bfee_ctrl(void *phl, struct rtw_wifi_role_link_t *rlink, bool ctrl);
#endif

enum rtw_phl_status
rtw_phl_snd_init_ops_send_ndpa(void *phl,
                               enum rtw_phl_status (*snd_send_ndpa)(void *,
                                                                    struct rtw_wifi_role_link_t *,
                                                                    u8 *,
                                                                    u32 *,
                                                                    enum channel_width));

u8 rtw_phl_snd_chk_in_progress(void *phl);

enum rtw_phl_status
rtw_phl_sound_start(void *phl, u8 wrole_idx, u8 st_dlg_tkn, u8 period, u8 test_flag);

enum rtw_phl_status
rtw_phl_sound_abort(void *phl);

enum rtw_phl_status
rtw_phl_snd_add_grp(void *phl,
                    struct rtw_wifi_role_link_t *rlink,
                    u8 gidx,
                    u16 *macid,
                    u8 num_sta,
                    bool he,
                    bool mu);

enum rtw_phl_status
rtw_phl_snd_cmd_set_vht_gid(void *phl,
			struct rtw_wifi_role_t *wrole,
			struct rtw_phl_gid_pos_tbl *tbl);

enum rtw_phl_status
rtw_phl_snd_cmd_set_aid(void *phl,
			struct rtw_wifi_role_t *wrole,
			struct rtw_phl_stainfo_t *sta,
			u16 aid);

enum rtw_phl_status
rtw_phl_sound_start_ex(void *phl, u8 wrole_idx, u8 st_dlg_tkn, u8 period, u8 test_flag);

enum rtw_phl_status
rtw_phl_set_power_limit(void *phl);

s8 rtw_phl_get_power_limit(void *phl, u8 hw_band,
	u16 rate, u8 bandwidth, u8 beamforming, u8 tx_num, u8 channel);

void
rtw_phl_enable_ext_pwr_lmt(void *phl, u8 hw_band,
	struct rtw_phl_ext_pwr_lmt_info *ext_pwr_lmt_info);
void
rtw_phl_set_ext_pwr_lmt_en(void *phl, bool enable);
enum rtw_phl_status
rtw_phl_cmd_updt_ext_txpwr_lmt(void *phl,
	struct rtw_phl_cmd_epl_para *param,
	enum phl_cmd_type cmd_type,
	u32 cmd_timeout);

void rtw_phl_init_ppdu_sts_para(struct rtw_phl_com_t *phl_com,
				bool en_psts_per_pkt, bool psts_ampdu,
				u8 rx_fltr);

enum rtw_phl_status rtw_phl_rf_on(void *phl);
enum rtw_phl_status rtw_phl_rf_off(void *phl);


#ifdef CONFIG_PHL_TWT
enum rtw_phl_status
rtw_phl_twt_alloc_twt_config(void *phl,
                             struct rtw_wifi_role_link_t *rlink,
                             struct rtw_phl_twt_setup_info setup_info,
                             u8 benable,
                             u8 *id);

enum rtw_phl_status
rtw_phl_twt_free_twt_config(void *phl, u8 id);

enum rtw_phl_status
rtw_phl_twt_enable_twt_config(void *phl, u8 id);

enum rtw_phl_status
rtw_phl_twt_free_all_twt_by_role(void *phl, struct rtw_wifi_role_t *role);

enum rtw_phl_status
rtw_phl_twt_disable_all_twt_by_role(void *phl, struct rtw_wifi_role_t *role);

enum rtw_phl_status
rtw_phl_twt_enable_all_twt_by_role(void *phl, struct rtw_wifi_role_t *role);

enum rtw_phl_status
rtw_phl_twt_add_sta_info(void *phl, struct rtw_phl_stainfo_t *phl_sta,
			u8 config_id, u8 id);

enum rtw_phl_status
rtw_phl_twt_teardown_sta(void *phl, struct rtw_phl_stainfo_t *phl_sta,
			struct rtw_phl_twt_flow_field *twt_flow, u8 *bitmap);

enum rtw_phl_status
rtw_phl_twt_get_new_flow_id(void *phl, struct rtw_phl_stainfo_t *phl_sta, u8 *id);

enum rtw_phl_status
rtw_phl_twt_accept_for_sta_mode(void *phl,
			struct rtw_phl_twt_sta_accept_i *accept_i);

enum rtw_phl_status
rtw_phl_twt_teardown_for_sta_mode(void *phl,
			struct rtw_phl_twt_sta_teardown_i *teardown_i);

enum rtw_phl_status
rtw_phl_twt_delete_all_sta_info(void *phl, struct rtw_phl_stainfo_t *phl_sta,
				u8 *bitmap);

enum rtw_phl_status
rtw_phl_twt_get_target_wake_time(void *phl,
			struct rtw_phl_twt_get_twt_i *get_twt_i,
			enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_twt_sta_announce_to_fw(void *phl, u16 macid);

enum rtw_phl_status
rtw_phl_twt_handle_c2h(void *phl_com, void *c);

enum rtw_phl_status
rtw_phl_twt_fill_twt_element(struct rtw_phl_twt_element *twt_ele, u8 *buf,
								u8 *length);

enum rtw_phl_status
rtw_phl_twt_fill_flow_field(struct rtw_phl_twt_flow_field *twt_flow, u8 *buf,
								u16 *length);
enum rtw_phl_status
rtw_phl_twt_parse_element(u8 *twt_ele, u16 length,
				struct rtw_phl_twt_element *twt_element);

enum rtw_phl_status
rtw_phl_twt_parse_setup_info(u8 *pkt, u16 length,
				struct rtw_phl_twt_setup_info *setup_info);

enum rtw_phl_status
rtw_phl_twt_parse_flow_field(u8 *ie_twt_flow, u16 length,
				struct rtw_phl_twt_flow_field *twt_flow);
#endif /* CONFIG_PHL_TWT */


#ifdef RTW_WKARD_P2PPS_REFINE
#ifdef CONFIG_PHL_P2PPS
enum rtw_phl_status
rtw_phl_p2pps_noa_update(void *phl, struct rtw_phl_noa_desc *in_desc);

void
rtw_phl_p2pps_init_ops(void *phl, struct rtw_phl_p2pps_ops *ops);

void rtw_phl_p2pps_noa_disable_all(void *phl,
                                   struct rtw_wifi_role_t *w_role);
#endif
#endif

#ifdef CONFIG_PHL_P2PPS
enum rtw_phl_status
rtw_phl_noa_update(void *phl, struct rtw_phl_noa_desc *in_desc,
		enum phl_cmd_type cmd_type, u8 cmd_timeout);

enum rtw_phl_status
rtw_phl_noa_disable_all(void *phl, struct rtw_wifi_role_t *w_role,
		enum phl_cmd_type cmd_type, u8 cmd_timeout);
#endif /* CONFIG_PHL_P2PPS */

void rtw_phl_event_notify(void *phl, enum phl_msg_evt_id event,
			struct rtw_wifi_role_t *wrole);
void rtw_phl_notification(void *phl,
                          enum phl_msg_evt_id event,
                          struct rtw_wifi_role_t *wrole,
                          bool direct);
void rtw_phl_dev_terminate_ntf(void *phl);

enum rtw_phl_status
rtw_phl_cmd_force_usb_switch(void *phl, u32 speed,
				enum phl_band_idx band_idx,
				enum phl_cmd_type cmd_type, u32 cmd_timeout);
enum rtw_phl_status
rtw_phl_cmd_get_usb_speed(void *phl, u32* speed,
				enum phl_band_idx band_idx,
				enum phl_cmd_type cmd_type, u32 cmd_timeout);
enum rtw_phl_status
rtw_phl_cmd_get_usb_support_ability(void *phl, u32* ability,
				enum phl_band_idx band_idx,
				enum phl_cmd_type cmd_type, u32 cmd_timeout);
u8 rtw_phl_get_sta_mgnt_rssi(struct rtw_phl_stainfo_t *psta);

void rtw_phl_init_chdef(struct rtw_phl_com_t *phl_com, struct rtw_chan_def *chdef);

enum rtw_phl_status
rtw_phl_txsts_rpt_config(void *phl, struct rtw_phl_stainfo_t *phl_sta);

#if defined(CONFIG_USB_HCI) || defined(CONFIG_PCI_HCI)
/* tx_ok/tx_fail are from release report*/
enum rtw_phl_status
rtw_phl_get_tx_ok_rpt(void *phl, struct rtw_phl_stainfo_t *phl_sta, u32 *tx_ok_cnt,
 enum phl_ac_queue qsel);

enum rtw_phl_status
rtw_phl_get_tx_fail_rpt(void *phl, struct rtw_phl_stainfo_t *phl_sta, u32 *tx_fail_cnt,
 enum phl_ac_queue qsel);

/* tx retry is from ra sts report.*/
enum rtw_phl_status
rtw_phl_get_tx_ra_retry_rpt(void *phl, struct rtw_phl_stainfo_t *phl_sta,
			    u32 *tx_retry_cnt, enum phl_ac_queue qsel,
			    u8 reset);
/* tx ok is from ra sts report.*/
enum rtw_phl_status
rtw_phl_get_tx_ra_ok_rpt(void *phl, struct rtw_phl_stainfo_t *phl_sta,
			 u32 *tx_ok_cnt, enum phl_ac_queue qsel,
			 u8 reset);

#endif /* defined(CONFIG_USB_HCI) || defined(CONFIG_PCI_HCI) */

void rtw_phl_dbg_dump_rx(void *phl, struct rtw_wifi_role_t *wrole);
#ifdef CONFIG_PCI_HCI
u32 rtw_phl_get_hw_cnt_rdu(void *phl);
#endif

#ifdef CONFIG_RTW_MIRROR_DUMP
void rtw_phl_mirror_dump_buf(void *phl, u8 cat, u8 *buf, u32 sz);
void rtw_phl_set_mirror_dump_config(void *phl, u32 config);
void rtw_phl_set_mirror_dump_txch(void *phl, u8 txch);
#endif

#ifdef CONFIG_PCI_HCI
void rtw_phl_get_hw_cnt_tx_fail(void *phl, u32 *tx_fail, u32 *tx_fail_mgmt);
void rtw_phl_get_hw_cnt_tx_ok(void *phl, u32 *tx_ok, u32 *tx_ok_mgmt);
#endif

enum rtw_phl_status
rtl_phl_cmd_get_cur_tsf(void *phl,
                        struct rtw_phl_port_tsf *tsf,
                        struct rtw_wifi_role_link_t *rlink,
                        enum phl_cmd_type cmd_type,
                        u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_set_fw_ul_fixinfo(void *phl,
				struct rtw_wifi_role_t *wifi_role,
				struct rtw_phl_ax_ul_fixinfo *ul_fixinfo,
				enum phl_cmd_type cmd_type,
				u32 cmd_timeout);

void rtw_phl_set_one_txring_mode(void *phl, u8 value);
u8 rtw_phl_get_one_txring_mode(void *phl);

void rtw_phl_tx_packet_event_notify(void *phl,
	struct rtw_wifi_role_link_t *rlink,
	enum phl_pkt_evt_type pkt_type);
void rtw_phl_packet_event_notify(void *phl,
	struct rtw_wifi_role_link_t *rlink,
        enum phl_pkt_evt_type pkt_evt);


/******************************************************************************
 *
 * TX power APIs
 *
 *****************************************************************************/
int rtw_phl_get_pw_lmt_regu_type_from_str(void *phl, const char *str);
const char *rtw_phl_get_pw_lmt_regu_str_from_type(void *phl, u8 regu);

const char *rtw_phl_get_pw_lmt_regu_type_str(void *phl, enum band_type band);

bool rtw_phl_pw_lmt_regu_tbl_exist(void *phl, enum band_type band, u8 regu);
u8 rtw_phl_ext_reg_codemap_search(void *phl, u16 domain_code, const char *country, const char **reg_name);

bool rtw_phl_get_pwr_lmt_en(void *phl, u8 band_idx);

struct txpwr_regu_info_t *rtw_phl_get_pw_lmt_regu_info(void *phl);
void rtw_phl_free_pw_lmt_regu_info(void *phl, struct txpwr_regu_info_t *info);

enum rtw_phl_status
rtw_phl_cmd_txpwr_ctl(void *phl, struct txpwr_ctl_param *args
	, enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_set_pw_lmt_regu(void *phl, struct txpwr_regu_info_t *conf, bool sw_conf_only
	, enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_set_tx_power_constraint(void *phl, enum phl_band_idx band_idx, u16 mb
	, enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_set_tx_power(void *phl, enum phl_band_idx band_idx
	, enum phl_cmd_type cmd_type, u32 cmd_timeout);

enum rtw_phl_status
rtw_phl_cmd_get_txinfo_pwr(void *phl, s16 *pwr_dbm,
				enum phl_band_idx band_idx,
				bool direct);
/*****************************************************************************/

u32 rtw_phl_get_phy_stat_info(void *phl, enum phl_band_idx hw_band,
			      enum phl_stat_info_query phy_stat);

enum rtw_phl_status
rtw_phl_cmd_get_rx_cnt_by_idx(void *phl,
			      enum phl_band_idx hw_band,
			      enum phl_rxcnt_idx idx,
			      u16 *rx_cnt,
			      enum phl_cmd_type cmd_type,
			      u32 cmd_timeout);
enum rtw_phl_status
rtw_phl_cmd_set_reset_rx_cnt(void *phl,
			     enum phl_band_idx hw_band,
			     enum phl_cmd_type cmd_type,
			     u32 cmd_timeout);

void rtw_phl_dump_tpu(void *phl);

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
rtw_phl_cmd_tpe_update(struct rtw_wifi_role_link_t *rlink,
                       struct rtw_tpe_info_t *tpe_info,
                       bool cmd_wait,
                       u32 cmd_timeout);
#endif

enum rtw_phl_status
rtw_phl_free_mld(void *phl, struct rtw_phl_mld_t *mld);

struct rtw_phl_mld_t *
rtw_phl_alloc_mld(void *phl,
                  struct rtw_wifi_role_t *wrole,
                  u8 *mac_addr,
                  enum rtw_device_type type);

enum rtw_phl_status
rtw_phl_link_mld_stainfo(struct rtw_phl_mld_t *mld,
                         struct rtw_phl_stainfo_t *phl_sta);

enum rtw_phl_status
rtw_phl_unlink_mld_stainfo(struct rtw_phl_mld_t *mld,
                           struct rtw_phl_stainfo_t *phl_sta);

struct rtw_phl_mld_t *
rtw_phl_get_mld_by_addr(void *phl,
                        struct rtw_wifi_role_t *wrole,
                        u8 *addr);

struct rtw_phl_mld_t *
rtw_phl_get_mld_self(void *phl, struct rtw_wifi_role_t *wrole);

struct rtw_phl_stainfo_t *
rtw_phl_get_stainfo_by_mld(struct rtw_phl_mld_t *mld, u8 lidx);

void
rtw_phl_mld_apply_links(struct rtw_phl_stainfo_t *sta);

enum rtl_ic_id rtw_phl_get_ic_id(void *phl);

enum rtw_phl_status
rtw_phl_sta_tsf_sync_done(void *phl, struct rtw_phl_stainfo_t *sta);


const char *rtw_phl_get_lstate_str(enum link_state lstate);
enum rtw_phl_status rtw_phl_pwr_switch_mac(void *phl, bool on);
enum rtw_phl_status
rtw_phl_hw_cts2self_cfg(void *phl, u8 enable,
			u8 band_sel, u8 non_sec_thr, u8 sec_thr);
bool
rtw_phl_check_sta_has_busy_wp(struct rtw_phl_stainfo_t *sta);

void rtw_phl_set_tx_pwr_comp(void *phl, u8 regu, s8 ag_comp_2g,
			     s8 ag_comp_5g, s8 ag_comp_6g);
#endif /*_PHL_API_H_*/

