/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
#ifndef _HAL_API_H_
#define _HAL_API_H_

/******************* IO  APIs *******************/
u8 rtw_hal_read8(void *h, u32 addr);
u16 rtw_hal_read16(void *h, u32 addr);
u32 rtw_hal_read32(void *h, u32 addr);
void rtw_hal_write8(void *h, u32 addr, u8 val);
void rtw_hal_write16(void *h, u32 addr, u16 val);
void rtw_hal_write32(void *h, u32 addr, u32 val);

u32 rtw_hal_read_macreg(void *h, u32 offset, u32 bit_mask);
void rtw_hal_write_macreg(void *h,
			u32 offset, u32 bit_mask, u32 data);
u32 rtw_hal_read_bbreg(void *h, u32 offset, u32 bit_mask);
void rtw_hal_write_bbreg(void *h,
			u32 offset, u32 bit_mask, u32 data);
u32 rtw_hal_read_rfreg(void *h,
			enum rf_path path, u32 offset, u32 bit_mask);
void rtw_hal_write_rfreg(void *h,
			enum rf_path path, u32 offset, u32 bit_mask, u32 data);

#ifdef PHL_PLATFORM_LINUX
void rtw_hal_mac_reg_dump(void *sel, void *h);
void rtw_hal_bb_reg_dump(void *sel, void *h);
void rtw_hal_bb_reg_dump_ex(void *sel, void *h);
void rtw_hal_rf_reg_dump(void *sel, void *h);
#endif

enum rtw_hal_status
rtw_hal_get_addr_cam(void *h, u16 num, u8 *buf, u16 size);
enum rtw_hal_status
rtw_hal_get_sec_cam(void *h, u16 num, u8 *buf, u16 size);

void rtw_hal_init_int_default_value(struct rtw_phl_com_t *phl_com, void *h, enum rtw_hal_int_set_opt opt);
void rtw_hal_enable_interrupt(struct rtw_phl_com_t *phl_com, void *h);
void rtw_hal_disable_interrupt_isr(struct rtw_phl_com_t *phl_com, void *h);
void rtw_hal_disable_interrupt(struct rtw_phl_com_t *phl_com, void *h);
void rtw_hal_config_interrupt(void *h, enum rtw_phl_config_int int_mode);
bool rtw_hal_recognize_interrupt(void *h);
bool rtw_hal_recognize_halt_c2h_interrupt(void *h);
void rtw_hal_clear_interrupt(void *h);
void rtw_hal_restore_interrupt(struct rtw_phl_com_t *phl_com, void *h);

u32 rtw_hal_interrupt_handler(void *h);
void rtw_hal_restore_rx_interrupt(void *h);
#ifdef PHL_RXSC_ISR
u16 rtw_hal_rpq_isr_check(void *hal, u8 dma_ch);
#endif

enum rtw_hal_status rtw_hal_get_pwr_state(void *h, enum rtw_mac_pwr_st *pwr_state);

enum rtw_hal_status rtw_hal_init(void *drv_priv,
	struct rtw_phl_com_t *phl_com, void **hal, enum rtl_ic_id chip_id);
struct rtw_hal_com_t *rtw_hal_get_halcom(void *hal);
void rtw_hal_deinit(struct rtw_phl_com_t *phl_com, void *hal);

bool rtw_hal_is_inited(struct rtw_phl_com_t *phl_com, void *hal);

u32 rtw_hal_hci_cfg(struct rtw_phl_com_t *phl_com, void *hal,
					struct rtw_ic_info *ic_info);
u32 rtw_hal_read_chip_info(struct rtw_phl_com_t *phl_com, void *hal);

void rtw_hal_set_default_var(void *hal);

u32 rtw_hal_var_init(struct rtw_phl_com_t *phl_com, void *hal);

enum rtw_hal_status rtw_hal_ser_ctrl(void *hal, enum rtw_hal_ser_rsn rsn, bool en);

u32
rtw_hal_ser_get_error_status(void *hal, u32 *err);

enum rtw_hal_status rtw_hal_ser_set_error_status(void *hal, u32 err);

bool rtw_hal_ser_chk_ser_l1(void *hal);

enum rtw_hal_status rtw_hal_trigger_cmac_err(void *hal);

enum rtw_hal_status rtw_hal_trigger_dmac_err(void *hal);

enum rtw_hal_status rtw_hal_lv1_rcvy(void *hal, u32 step);

void
rtw_hal_dump_fw_rsvd_ple(void *phl);
void
rtw_hal_ser_reset_wdt_intr(void *hal);

void rtw_hal_ser_int_cfg(void *hal, struct rtw_phl_com_t *phl_com, enum RTW_PHL_SER_CFG_STEP step);


enum rtw_hal_status rtw_hal_en_fw_log(void *hal, u32 comp, bool en);

enum rtw_hal_status
rtw_hal_download_fw(struct rtw_phl_com_t *phl_com, void *hal);

enum rtw_hal_status
rtw_hal_redownload_fw(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_fw_dbg_dump(void *hal);

enum rtw_fw_status rtw_hal_get_fw_status(void *h);

enum rtw_hal_status rtw_hal_preload(struct rtw_phl_com_t *phl_com, void *hal);
enum rtw_hal_status rtw_hal_start(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_stop(struct rtw_phl_com_t *phl_com, void *hal);
enum rtw_hal_status rtw_hal_restart(struct rtw_phl_com_t *phl_com, void *hal);
enum rtw_hal_status rtw_hal_hal_deinit(struct rtw_phl_com_t *phl_com, void *hal);

enum rtw_hal_status rtw_hal_role_deinit(void *hal, struct rtw_wifi_role_link_t *rlink);

#ifdef CONFIG_WOWLAN
enum rtw_hal_status rtw_hal_get_wake_rsn(void *hal, enum rtw_mac_wow_wake_reason *wake_rsn, u8 *reset);
enum rtw_hal_status rtw_hal_cfg_wow_sleep(void *hal, u8 sleep);
enum rtw_hal_status rtw_hal_get_wow_aoac_rpt(void *hal, struct rtw_aoac_report *aoac_info, u8 rx_ready);
enum rtw_hal_status rtw_hal_get_wow_fw_status(void *hal, u8 *status);
enum rtw_hal_status rtw_hal_wow_cfg_txdma(void *hal, u8 en);

enum rtw_hal_status rtw_hal_wow_init(struct rtw_phl_com_t *phl_com, void *hal, struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status rtw_hal_wow_deinit(struct rtw_phl_com_t *phl_com, void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_wow_func_en(struct rtw_phl_com_t *phl_com, void *hal, u16 macid,
                    struct rtw_hal_wow_cfg *cfg);
enum rtw_hal_status
rtw_hal_wow_func_dis(struct rtw_phl_com_t *phl_com, void *hal, u16 macid,
                     struct rtw_hal_wow_cfg *cfg);

enum rtw_hal_status rtw_hal_wow_func_start(struct rtw_phl_com_t *phl_com, void *hal, u16 macid, struct rtw_hal_wow_cfg *cfg);
enum rtw_hal_status rtw_hal_wow_func_stop(struct rtw_phl_com_t *phl_com, void *hal, u16 macid);

enum rtw_hal_status rtw_hal_set_wowlan(struct rtw_phl_com_t *phl_com, void *hal, u8 enter);
enum rtw_hal_status rtw_hal_sw_gpio_ctrl(struct rtw_phl_com_t *phl_com, void *hal, u8 high, u8 gpio);
enum rtw_hal_status rtw_hal_set_sw_gpio_mode(struct rtw_phl_com_t *phl_com, void *hal, enum rtw_gpio_mode mode, u8 gpio);
enum rtw_hal_status rtw_hal_wow_drop_tx(void *hal, u8 band);

enum rtw_hal_status
rtw_hal_wow_cfg_nlo_chnl_list(void *hal, struct rtw_nlo_info *cfg);

enum rtw_hal_status
rtw_hal_wow_cfg_nlo(void *hal, enum SCAN_OFLD_OP op, u16 mac_id,
                    u8 hw_band, u8 hw_port, struct rtw_nlo_info *cfg);

#endif /* CONFIG_WOWLAN */

int rtw_hal_find_ext_regd_num(struct rtw_para_pwrlmt_info_t *para_info, const char *regd_name);
void
 rtw_hal_dl_all_para_file(struct rtw_phl_com_t *phl_com, char *ic_name, void *hal);

enum rtw_hal_status rtw_hal_trx_init(void *hal, u8 *txbd_buf, u8 *rxbd_buf);
void rtw_hal_trx_deinit(void *hal);

enum rtw_hal_status
rtw_hal_role_cfg(void *hal,
                 struct rtw_wifi_role_t *wrole,
                 struct rtw_wifi_role_link_t *rlink);

enum rtw_hal_status
rtw_hal_role_cfg_ex(void *hal,
                    struct rtw_wifi_role_link_t *rlink,
                    enum pcfg_type type,
                    void *param);

enum rtw_hal_status
rtw_hal_beacon_stop(void *hal,
                    struct rtw_wifi_role_link_t *rlink,
                    bool stop);

enum rtw_hal_status
rtw_hal_stainfo_init(void *hal, struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_stainfo_deinit(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_set_dctrl_tbl_seciv(void *hal,
                 struct rtw_phl_stainfo_t *sta, u64 iv);

enum rtw_hal_status
rtw_hal_add_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_del_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_restore_sta_entry(struct rtw_phl_com_t* phl_com, void *hal, struct rtw_phl_stainfo_t *sta,
							bool is_connect);
enum rtw_hal_status
rtw_hal_update_sta_entry(struct rtw_phl_com_t* phl_com, void *hal, struct rtw_phl_stainfo_t *sta,
							bool is_connect);
enum rtw_hal_status
rtw_hal_change_sta_entry(void *hal, struct rtw_phl_stainfo_t *sta,
							enum phl_upd_mode mode);
void
rtw_hal_disconnect_drop_all_pkt(void *hal, struct rtw_phl_stainfo_t *sta);
u8 rtw_hal_get_sta_rssi(struct rtw_phl_stainfo_t *sta);
u8 rtw_hal_get_sta_rssi_bcn(struct rtw_phl_stainfo_t *sta);

bool rtw_hal_is_sta_linked(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_set_sta_rx_sts(struct rtw_phl_stainfo_t *sta, u8 rst,
		       struct rtw_r_meta_data *meta);

enum rtw_hal_status
rtw_hal_query_rainfo(void *hal, struct rtw_hal_stainfo_t *hal_sta,
		     struct rtw_phl_rainfo *phl_rainfo);

enum rtw_hal_status
rtw_hal_start_ba_session(void *hal, struct rtw_phl_stainfo_t *sta,
                         u8 dialog_token, u16 timeout,
                         u16 start_seq_num, u16 ba_policy,
                         u16 tid, u16 buf_size);
enum rtw_hal_status
rtw_hal_stop_ba_session(void *hal, struct rtw_phl_stainfo_t *sta, u16 tid);

/**
 * rtw_hal_set_edca() - setup WMM EDCA parameter
 * @hal:	struct hal_info_t *
 * @wrole:	struct rtw_wifi_role_t *
 * @ac:		Access Category, 0:BE, 1:BK, 2:VI, 3:VO
 * @param:	AIFS:BIT[7:0], CWMIN:BIT[11:8], CWMAX:BIT[15:12],
 *		TXOP:BIT[31:16]
 *
 * Setup WMM EDCA parameter set for sending packets.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status
rtw_hal_set_edca(void *hal,
                 struct rtw_wifi_role_link_t *rlink,
                 u8 ac,
                 u32 edca);

enum rtw_hal_status
rtw_hal_upd_ampdu_cctrl_info(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_cfg_tx_ampdu(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_cfg_tx_amsdu(void *hal,
		     u8 macid,
		     u8 enable,
		     u8 amsdu_max_len,
		     u8 qos_field_h,
		     u8 qos_field_h_en,
		     u8 mhdr_len,
		     u8 vlan_tag_valid);

/**
 * rtw_hal_cfg_rsc() - set rsc config
 * @hal:	struct hal_info_t *
 * @wrole:	struct rtw_wifi_role_t *
 * @rsc_cfg:    00: Primary 20Mhz BW(0x483[3:0] )
 *                      01: Follow Rx packet BW mode
 *                      10: Duplicate mode. (ofdm rate only, others bw20
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status
rtw_hal_cfg_rsc(void *hal, struct rtw_phl_stainfo_t *sta, u8 rsc_cfg);

enum rtw_hal_status
rtw_hal_cfg_rrsr_ref_rate_sel(void *hal, struct rtw_phl_stainfo_t *sta, bool ref_rate_sel);

#ifdef CONFIG_PHL_CUSTOM_FEATURE
enum rtw_hal_status
rtw_hal_custom_cfg_tx_ampdu(void *hal,
                            struct rtw_wifi_role_link_t *rlink,
                            struct rtw_phl_custom_ampdu_cfg *ampdu);

enum rtw_hal_status
rtw_hal_get_ampdu_cfg(void *hal,
                      struct rtw_wifi_role_link_t *rlink,
                      struct rtw_phl_custom_ampdu_cfg *cfg);

enum rtw_hal_status
rtw_hal_get_edca(void *hal,
                 struct rtw_wifi_role_link_t *rlink,
                 struct rtw_edca_param *edca_param);

enum rtw_hal_status
rtw_hal_set_usr_edca(void *hal,
                     u32 idx,
                     u32 enable,
                     u32 band,
                     u32 wmm,
                     struct rtw_edca_param *aggr,
                     struct rtw_edca_param *mod);

enum rtw_hal_status
rtw_hal_set_pop_en(void *hal, bool en, enum phl_phy_idx phy_idx);

bool
rtw_hal_query_pop_en(void *hal, enum phl_phy_idx phy_idx);

enum rtw_hal_status
rtw_hal_set_pkt_detect_thold(void *hal, enum phl_band_idx band_idx, u32 bound);

u8
rtw_hal_query_pkt_detect_thold(void *hal,
                               bool get_en_info,
                               enum phl_phy_idx phy_idx);
#endif
enum rtw_hal_status
rtw_hal_thermal_protect_cfg_tx_duty(
	void *hal,
	u16 tx_duty_interval,
	u8 ratio);

enum rtw_hal_status
rtw_hal_thermal_protect_stop_tx_duty(
	void *hal);

enum rtw_hal_status
rtw_hal_set_key(void *hal, struct rtw_phl_stainfo_t *sta, u8 type, u8 ext_key, u8 spp,
				u8 keyid, u8 keytype, u8 *keybuf);

u32
rtw_hal_search_key_idx(void *hal, struct rtw_phl_stainfo_t *sta,
						u8 keyid, u8 keytype);
/**
 * rtw_hal_tx_chnl_mapping - query hw tx dma channel mapping to the sw xmit ring
 * identified by macid, tid and band
 * @hal: see struct hal_info_t
 * @macid: input target macid is 0 ~ 127
 * @cat: input target packet category, see enum rtw_phl_ring_cat
 * @band: input target band, 0 for band 0 / 1 for band 1
 *
 * returns the mapping hw tx dma channel
 */
u8 rtw_hal_tx_chnl_mapping(void *hal, u16 macid,
			   enum rtw_phl_ring_cat cat, u8 band);

enum rtw_hal_status
rtw_hal_enable_hwamsdu(void *hal,
			u8 enable,
			u8 max_num,
			u8 en_single_amsdu,
			u8 en_last_amsdu_padding);

/**
 * rtw_hal_convert_qsel_to_tid - convert qsel to tid value
 * @hal: see struct hal_info_t
 * @qsel: HW queue selection
 * @tid_indic: tid indicator
 *
 * returns enum RTW_HAL_STATUS
 */
u8 rtw_hal_convert_qsel_to_tid(void *hal, u8 qsel_id, u8 tid_indic);


/**
 * rtw_hal_tx_res_query - query current HW tx resource with specifc dma channel
 * @hal: see struct hal_info_t
 * @dma_ch: the target dma channel
 * @host_idx: current host index of this channel
 * @hw_idx: current hw index of this channel
 *
 * this function returns the number of available tx resource
 * NOTE, input host_idx and hw_idx ptr shall NOT be NULL
 */
u16 rtw_hal_tx_res_query(void *hal, u8 dma_ch, u16 *host_idx, u16 *hw_idx);

/**
 * rtw_hal_rx_res_query - query current HW rx resource with specifc dma channel
 * @hal: see struct hal_info_t
 * @dma_ch: the target dma channel
 * @host_idx: current host index of this channel
 * @hw_idx: current hw index of this channel
 *
 * this function returns the number of available tx resource
 * NOTE, input host_idx and hw_idx ptr shall NOT be NULL
 */
u16 rtw_hal_rx_res_query(void *hal, u8 dma_ch, u16 *host_idx, u16 *hw_idx);
u16 rtw_hal_get_rxbd_num(void *hal, u8 dma_ch);
u16 rtw_hal_get_rxbuf_num(void *hal, u8 dma_ch);
u16 rtw_hal_get_rxbuf_size(void *hal, u8 dma_ch);

enum phl_band_idx rtw_hal_query_txch_hwband(void *hal, u8 dma_ch);

/**
 * rtw_hal_query_txch_num - query total hw tx dma channels number
 *
 * returns the number of  hw tx dma channel
 */
u8 rtw_hal_query_txch_num(void *hal);

/**
 * rtw_hal_query_rxch_num - query total hw rx dma channels number
 *
 * returns the number of  hw rx dma channel
 */
u8 rtw_hal_query_rxch_num(void *hal);


/**
 * rtw_hal_update_wd_page - update wd page for xmit packet
 * @hal: see struct hal_info_t
 * @phl_pkt_req: packet xmit request from phl, see struct rtw_phl_pkt_req
 *
 * returns enum RTW_HAL_STATUS
 */
enum rtw_hal_status rtw_hal_update_wd_page(void *hal, void *phl_pkt_req);

enum rtw_hal_status
rtw_hal_handle_rx_buffer(struct rtw_phl_com_t *phl_com, void* hal,
				u8 *buf, u32 buf_size,
				struct rtw_phl_rx_pkt *rxpkt);

enum rtw_hal_status
rtw_hal_query_info(void* hal, u8 info_id, void *value);

#ifdef CONFIG_PHL_DFS
bool rtw_hal_in_radar_domain(void *hal, enum band_type band, u8 ch
	, enum channel_width bw, enum chan_offset offset);
void rtw_hal_bb_dfs_change_domain(void *hal, enum band_type band, u8 ch
	, enum channel_width bw, enum chan_offset offset);
enum rtw_hal_status rtw_hal_radar_detect_cfg(void *hal, bool dfs_enable);
#endif /*CONFIG_PHL_DFS*/

enum rtw_hal_status
rtw_hal_set_ch_bw(void *hal, u8 band_idx,
		struct rtw_chan_def *chdef, bool do_rfk, bool rd_enabled, bool frc_switch);
u8 rtw_hal_get_cur_ch(void *hal, u8 band_idx);
void rtw_hal_get_cur_chdef(void *hal, u8 band_idx,
				struct rtw_chan_def *cur_chandef);

void rtw_hal_sync_cur_ch(void *hal, u8 band_idx, struct rtw_chan_def chandef);

u8 rtw_hal_get_fwcmd_queue_idx(void* hal);
void rtw_hal_cfg_txhci(void *hal, u8 en);
void rtw_hal_cfg_rxhci(void *hal, u8 en);
enum rtw_hal_status rtw_hal_chk_allq_empty(void *hal, u8 *empty);
enum rtw_hal_status rtw_hal_set_power_limit(void *hal, u8 band_idx);
s8 rtw_hal_get_power_limit(void *hal, enum phl_phy_idx phy,
	u16 rate, u8 bandwidth, u8 beamforming, u8 tx_num, u8 channel);
bool rtw_hal_get_ext_pwr_lmt_en(void *hal, u8 hw_band);
void rtw_hal_set_ext_pwr_lmt_en(void *hal, u8 hw_band, bool enable);
void rtw_hal_enable_ext_pwr_lmt(void *hal, u8 hw_band,
		struct rtw_phl_ext_pwr_lmt_info *ext_pwr_lmt_info);

u8 rtw_hal_get_tx_tbl_to_pwr_times(void *hal);
void rtw_hal_set_tx_pwr_comp(void *hal, enum phl_phy_idx phy,
		struct rtw_phl_regu_dyn_ant_gain *dyn_ag);

#ifdef CONFIG_RTW_ACS
void rtw_hal_acs_mntr_trigger(void *hal, enum phl_band_idx band_idx, struct acs_mntr_parm *parm);
enum rtw_hal_status rtw_hal_acs_mntr_result(void *hal,
					enum phl_band_idx band_idx, struct acs_mntr_rpt *rpt);
#endif /* CONFIG_RTW_ACS */

/*watchdog update env result*/
void rtw_hal_env_rpt(struct rtw_hal_com_t *hal_com,
                     struct rtw_env_report *env_rpt,
                     u8 hw_band);

#ifdef CONFIG_PCI_HCI
/**
 * rtw_hal_update_txbd - update tx bd for xmit packet
 * @hal: see struct hal_info_t
 * @txbd: the target txbd to update
 * @wd: buffer pointer of wd page to fill in txbd
 *
 * returns enum RTW_HAL_STATUS
 * NOTE, this function is PCIe specific function
 */
enum rtw_hal_status
rtw_hal_update_txbd(void *hal, void *txbd, void *wd, u8 dma_ch, u16 wd_num);

/**
 * rtw_hal_update_rxbd - update rx bd for recv packet
 * @hal: see struct hal_info_t
 * @rxbd: the target rxbd to update
 * @wd: buffer pointer of wd page to fill in rxbd
 *
 * returns enum RTW_HAL_STATUS
 * NOTE, this function is PCIe specific function
 */
enum rtw_hal_status
rtw_hal_update_rxbd(void *hal, struct rx_base_desc *rxbd,
					struct rtw_rx_buf *rxbuf, u8 ch_idx);

/**
 * rtw_hal_update_trigger_txstart - trigger hw to start tx
 * @hal: see struct hal_info_t
 * @txbd: the target txbd to update
 * @dma_ch: the dma channel index of this txbd_ring
 *
 * returns enum RTW_HAL_STATUS
 */
enum rtw_hal_status
rtw_hal_trigger_txstart(void *hal, struct tx_base_desc *txbd, u8 dma_ch);

/**
 * rtw_hal_notify_rxdone - notify hw rx done
 * @hal: see struct hal_info_t
 * @rxbd: the target rxbd to update
 * @dma_ch: the target rx channel
 *
 * returns enum RTW_HAL_STATUS
 */
enum rtw_hal_status
rtw_hal_notify_rxdone(void* hal, struct rx_base_desc *rxbd, u8 dma_ch);

/**
 * rtw_hal_handle_wp_rpt -parsing the wp recycle report packet
 * @hal: see struct hal_info_t
 * @rp: input, the pointer of target recycle report buffer to parse
 * @len: input, the length of this report buffer
 * @sw_retry: output, the packet mentioned in this report needs sw retry if sw_retry == 1
 * @dma_ch: output, the tx dma channel of this packet mentioned in this report
 * @wp_seq: output, the wp_seq of this packet mentioned in this report
 * @macid: output, the mac_id of this packet mentioned in this report
 * @ac_queue: output, the tid of this packet mentioned in this report
 * @txsts: output, the tx status of this packet mentioned in this report
 *
 * returns the length of report buffer which has been parsed in this function
 */
u16 rtw_hal_handle_wp_rpt(void *hal, u8 *rp, u16 len, u8 *sw_retry, u8 *dma_ch,
			  u16 *wp_seq, u8 *macid, u8 *ac_queue, u8 *txsts);

u8 rtw_hal_check_rxrdy(struct rtw_phl_com_t *phl_com, void* hal,
		       struct rtw_rx_buf *rxbuf, u8 dma_ch);
u8 rtw_hal_handle_rxbd_info(void* hal, u8 *rxbuf, u16 *buf_size);
enum rtw_hal_status rtw_hal_set_l2_leave(void *hal);
void rtw_hal_clear_bdidx(void *hal);
void rtw_hal_rst_bdram(void *hal);
bool rtw_hal_poll_txdma_idle(void *hal);
void rtw_hal_cfg_dma_io(void *hal, u8 en);


enum rtw_hal_status rtw_hal_ltr_sw_trigger(void *hal,
	enum rtw_pcie_ltr_state state);
enum rtw_hal_status rtw_hal_ltr_en_hw_mode(void *hal, bool hw_mode);
#ifdef RTW_WKARD_DYNAMIC_LTR
void rtw_hal_ltr_update_stats(void *hal, bool clear);
bool rtw_hal_ltr_is_sw_ctrl(struct rtw_phl_com_t *phl_com, void *hal);
bool rtw_hal_ltr_is_hw_ctrl(struct rtw_phl_com_t *phl_com, void *hal);
#endif

#endif /*CONFIG_PCI_HCI*/

#ifdef CONFIG_USB_HCI

u8 rtw_hal_get_bulkout_id(void *hal, u8 dma_ch, u8 mode);

u32 rtw_hal_get_wd_len(void *hal,
				struct rtw_xmit_req *tx_req);

enum rtw_hal_status rtw_hal_fill_wd(void *hal,
				struct rtw_xmit_req *tx_req,
				u8 *wd_buf, u32 *wd_len);

enum rtw_hal_status
	rtw_hal_usb_tx_agg_cfg(void *hal, u8* wd_buf, u8 agg_num);

/**
 * rtw_hal_usb_rx_agg_cfg - configure USB Rx aggregation setting
 * @hal: see struct hal_info_t
 * @mode: Parameter settings for Rx aggregation. see enum phl_usb_rx_agg_mode.
 * @agg_mode: Mode of USB data transfer. see enum mac_ax_rx_agg_mode.
 * @timeout: Maximum Rx aggregation time. In 32 ms.
 * @size: Maximum Rx aggregation size. In 4096 bytes.
 * @pkt_num: Maximum Rx aggregation packet number.
 */
enum rtw_hal_status
	rtw_hal_usb_rx_agg_cfg(void *hal, u8 mode, u8 agg_mode,
	u8 drv_define, u8 timeout, u8 size, u8 pkt_num);

enum rtw_hal_status
rtw_hal_usb_rx_agg_init(struct rtw_phl_com_t *phl_com, void *hal);

u8 rtw_hal_get_max_bulkout_wd_num(void *hal);
u16 rtw_hal_get_max_dma_txagg_msk(void *hal);
u32 rtwl_hal_get_cur_usb_mode(void *h);
u32 rtwl_hal_get_usb_support_ability(void *h);
enum rtw_hal_status rtw_hal_force_usb_switch(void *h, enum usb_type type);
/**
 * rtw_hal_handle_wp_rpt_usb -parsing the wp recycle report packet
 * @hal: see struct hal_info_t
 * @rp: input, the pointer of target recycle report buffer to parse
 * @len: input, the length of this report buffer
 * @macid: output, the mac_id of this packet mentioned in this report
 * @ac_queue: output, the tid of this packet mentioned in this report
 * @txsts: output, the tx status of this packet mentioned in this report
 *
 * returns the length of report buffer which has been parsed in this function
 */
u16 rtw_hal_handle_wp_rpt_usb(void *hal, u8 *rp, u16 len, u8 *macid, u8 *ac_queue,
			u8 *txsts);

#endif

#ifdef CONFIG_SDIO_HCI
/**
 * rtw_hal_sdio_tx_cfg - SDIO TX related setting
 * @hal:	pointer of struct hal_info_t
 *
 * Configure setting for SDIO TX.
 *
 * No return value for this function.
 */
void rtw_hal_sdio_tx_cfg(void *hal);

enum rtw_hal_status rtw_hal_sdio_tx(void *hal, u8 dma_ch, u8 *buf, u32 buf_len,
				    u8 agg_count, u16 *pkt_len, u8 *wp_offset);
void rtw_hal_sdio_rx_agg_cfg(void *hal, bool enable, u8 drv_define,
			     u8 timeout, u8 size, u8 pkt_num);
int rtw_hal_sdio_rx(void *hal, struct rtw_rx_buf *rxbuf);
int rtw_hal_sdio_parse_rx(void *hal, struct rtw_rx_buf *rxbuf);
/**
 * rtw_hal_sdio_lps_flg - get lps status
 * @hal:	pointer of struct hal_info_t
 *
 * Get driver lps status
 *
 * return lps flag state defined in enum rtw_hal_lps_flg_state
 * RTW_HAL_LPS_FLG_STATE_LPS when driver set LPS-CG/LPS-PG enable;
 * RTW_HAL_LPS_FLG_STATE_ACTIVE when driver is not in LPS-CG/LPS-PG
 */
enum rtw_hal_lps_flg_state rtw_hal_sdio_lps_flg(void *hal);
#endif /* CONFIG_SDIO_HCI */

/* HAL SOUND API */
enum rtw_hal_status rtw_hal_snd_query_proc_sta_res(
	void *hal, struct rtw_phl_stainfo_t *sta,
	bool mu, enum channel_width bw, bool en_swap);

enum rtw_hal_status
rtw_hal_snd_release_proc_sta_res(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_snd_proc_pre_cfg_sta(void *hal, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_snd_proc_pre_cfg(void *hal);

void
rtw_hal_snd_ndpa_sta_info_vht(struct rtw_phl_stainfo_t *psta_info,
			      u32 *ndpa, u8 mu);

void
rtw_hal_snd_ndpa_sta_info_he(struct rtw_phl_stainfo_t *psta_info,
			     u32 *ndpa, enum channel_width bw, u8 fb_type);

enum rtw_hal_status
rtw_hal_snd_proc_post_cfg_sta(void *hal,
				struct rtw_phl_stainfo_t *sta, bool mu);

enum rtw_hal_status
rtw_hal_snd_proc_post_cfg_gid(void *hal, u8 gid, void *ba_info);

enum rtw_hal_status
rtw_hal_snd_proc_post_cfg(void *hal, bool he, bool mu, bool en_fixed_mode);

enum rtw_hal_status
rtw_hal_snd_mac_ctrl(void *hal, u8 band, u8 ctrl);

enum rtw_hal_status
rtw_hal_snd_chk_bf_res(void *hal, struct rtw_phl_stainfo_t *sta,
		       bool mu, enum channel_width bw);

void
rtw_hal_snd_polling_snd_sts(void *hal, struct rtw_phl_stainfo_t *sta);

/* fw sounding commmand ralated */
void rtw_hal_snd_set_fw_cmd_dialogtkn(void *hal, u8 *buf, u8 he, u8 token);

void rtw_hal_snd_vht_fwcmd_su(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u32 *npda_sta);

void rtw_hal_snd_vht_fwcmd_mu_pri(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u8 sta_nr, u32 *ndpa_sta);

void rtw_hal_snd_vht_fwcmd_mu_add_sta(void *hal, u8 *buf, u32 *ndpa_sta,
				      struct rtw_phl_stainfo_t *sta,
				      u8 ndpa_idx, u8 last);

void rtw_hal_snd_ax_fwcmd_nontb(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u32 *npda_sta);

void rtw_hal_snd_ax_fwcmd_tb_pri(void *hal, u8 *buf, enum channel_width bw,
		    struct rtw_phl_stainfo_t *psta, u8 sta_nr1, u8 sta_nr2);

void rtw_hal_snd_ax_fwcmd_tb_add_sta(void *hal, u8 *buf, u32 *ndpa_sta,
				     struct rtw_phl_stainfo_t *sta, u8 ru_idx,
				     u8 ndpa_idx, u8 bfrp_idx, u8 bfrp_u_idx);

u8 *rtw_hal_snd_prepare_snd_cmd(void *hal);
enum rtw_hal_status rtw_hal_snd_release_snd_cmd(void *hal, u8 *buf);
enum rtw_hal_status rtw_hal_snd_send_fw_cmd(void *hal, u8 *cmd);

/*HAL CSI Buffer Mgnt APIs*/
enum channel_width
rtw_hal_get_csi_buf_bw(void *buf);

bool
rtw_hal_get_csi_buf_type(void *buf);

/*HAL Beamform Mgnt APIs*/
void rtw_hal_bf_dbg_dump_entry(void *entry);
void rtw_hal_bf_dbg_dump_entry_all(void *hal);
u8 rtw_hal_bf_get_sumu_idx(void *hal, void *entry);
bool rtw_hal_bf_chk_bf_type(void *hal_info,
			struct rtw_phl_stainfo_t *sta, bool mu);
void rtw_hal_bf_preset_mu_ba_info(void *hal,
			struct rtw_phl_stainfo_t *psta, void *hal_ba_info);
void rtw_hal_bf_set_txmu_para(void *hal, u8 gid , u8 en,
			      enum rtw_hal_protection_type rts_type,
			      enum rtw_hal_ack_resp_type ack_type);
enum rtw_hal_status
rtw_hal_bf_set_fix_mode(void *hal, bool mu, bool he);
enum rtw_hal_status
rtw_hal_bf_get_entry_snd_sts(void *entry);

void rtw_hal_beamform_set_vht_gid(void *hal, u8 band,
				  struct rtw_phl_gid_pos_tbl *tbl);

/******************************************************************************
 *
 * BTC APIs
 *
 *****************************************************************************/
void rtw_hal_btc_scan_start_ntfy(void *hinfo, enum phl_phy_idx phy_idx,
				  enum band_type band);
void rtw_hal_btc_scan_finish_ntfy(void *hinfo, enum phl_phy_idx phy_idx);

void rtw_hal_btc_update_role_info_ntfy(void *hinfo, u8 role_id,
				       struct rtw_wifi_role_t *wrole,
				       struct rtw_wifi_role_link_t *rlink,
				       struct rtw_phl_stainfo_t *sta,
				       enum link_state lstate);

void
rtw_hal_btc_ap_client_notify(void *hinfo,
		struct rtw_wifi_role_link_t *rlink, enum link_state lstate);

void rtw_hal_btc_packet_event_ntfy(void *hinfo, u8 pkt_evt_type);
void rtw_hal_btc_radio_state_ntfy(void *hinfo, u8 rf_state);
void rtw_hal_btc_customerize_ntfy(void *hinfo, u8 type, u16 len, u8 *buf);
void rtw_hal_btc_wl_status_ntfy(void *hinfo, struct rtw_phl_com_t *phl_com, u8 ntfy_num,
					struct rtw_phl_stainfo_t *sta[],
									u8 reason);
void rtw_hal_btc_timer(void *hinfo, void *timer);
void rtw_hal_btc_fwinfo_ntfy(void *hinfo);

enum rtw_hal_status rtw_hal_scan_set_rxfltr_by_mode(void *hinfo,
	enum phl_phy_idx phy_idx, bool off_channel, u8 *mode);
enum rtw_hal_status
rtw_hal_enter_mon_mode(void *hinfo, enum phl_phy_idx phy_idx);
enum rtw_hal_status
rtw_hal_leave_mon_mode(void *hinfo, enum phl_phy_idx phy_idx);
#ifdef CONFIG_FSM
enum rtw_hal_status rtw_hal_scan_flush_queue(void *hinfo,
	struct rtw_wifi_role_t *wrole);
#endif
enum rtw_hal_status rtw_hal_scan_pause_tx_fifo(void *hinfo,
	u8 band_idx, bool off_ch);

enum rtw_hal_status
rtw_hal_dfs_pause_tx(void *hinfo, u8 band_idx, bool off_ch, enum tx_pause_rson rson);

/*****************************************************************************/

void rtw_hal_com_scan_set_tx_lifetime(void *hal, u8 band);
void rtw_hal_com_scan_restore_tx_lifetime(void *hal, u8 band);
enum rtw_hal_status
rtw_hal_com_set_tx_lifetime(void *hal, u8 band, u8 en, u16 val);

enum rtw_hal_status
rtw_hal_com_set_power_offset(void *hal, u8 band, s8 ofst_mode, s8 ofst_bw);

#ifdef RTW_PHL_BCN
enum rtw_hal_status
rtw_hal_add_beacon(struct rtw_phl_com_t *phl_com, void *hal, void *bcn_cmn);

enum rtw_hal_status
rtw_hal_update_beacon(struct rtw_phl_com_t *phl_com, void *hal, u8 bcn_id);

enum rtw_hal_status
rtw_hal_free_beacon(struct rtw_phl_com_t *phl_com, void *hal, u8 bcn_id);
#ifdef CONFIG_RTW_DEBUG_BCN_TX
enum rtw_hal_status
rtw_hal_get_beacon_cnt(struct rtw_phl_com_t *phl_com, void *hal,
		u8 bcn_id, struct rtw_bcn_stats **bcn_stats);
#endif
#endif

enum rtw_hal_status
rtw_hal_com_set_gt3(void *hal, u8 enable, u32 timeout);

enum rtw_hal_status
rtw_hal_set_dctrl_tbl_seq(void *hal,
	struct rtw_phl_stainfo_t *sta, u32 seq);

enum rtw_hal_status
rtw_hal_proc_cmd(void *hal, char proc_cmd, struct rtw_proc_cmd *incmd,
							char *output, u32 out_len);

void rtw_hal_get_mac_version(char *ver_str, u16 len);
void rtw_hal_get_fw_ver(void *hal, char *ver_str, u16 len);

enum rtw_hal_status
rtw_hal_tx_pause(struct rtw_hal_com_t *hal_com,
			u8 band_idx, bool tx_pause, enum tx_pause_rson rson);

enum rtw_hal_status rtw_hal_set_macid_pause(void *hinfo,
                                            u16 macid,
                                            bool pause);

/**
 * rtw_hal_set_rxfltr_opt_by_mode - Set rx filter option by scenario
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @mode:	scenario mode
 *
 * Set RX filter option setting by scenario.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_opt_by_mode(void *hal, u8 band,
					       enum rtw_rx_fltr_opt_mode mode);

/**
 * rtw_hal_get_rxfltr_opt_mode - Get rx filter option
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 *
 * Get RX filter mode
 *
 * Return rtw_rx_fltr_opt_mode
 */
enum rtw_rx_fltr_opt_mode rtw_hal_get_rxfltr_opt_mode(void *hal, u8 band);

/**
 * rtw_hal_set_rxfltr_type_by_mode - Set target for rx filter type by scenario
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @mode:	scenario mode
 *
 * Set target of RX filter type by scenario.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_type_by_mode(void *hal, u8 band,
						    enum rtw_rxfltr_type_mode mode);

/**
 * rtw_hal_set_rxfltr_by_stype_ctrl - Set target for rx CTRL filter sub-type
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @cap_ctrl:	data structure to set target
 *
 * Set target for rx CTRL filter sub-type.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_by_stype_ctrl(void *hal, u8 band,
						     struct rxfltr_cap_to_set_ctrl *cap_ctrl);

/**
 * rtw_hal_set_rxfltr_by_stype_mgnt - Set target for rx MGNT filter sub-type
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @cap_mgnt:	data structure to set target
 *
 * Set target for rx MGNT filter sub-type.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_by_stype_mgnt(void *hal, u8 band,
						     struct rxfltr_cap_to_set_mgnt *cap_mgnt);

/**
 * rtw_hal_set_rxfltr_by_stype_data - Set target for rx DATA filter sub-type
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @cap_data:	data structure to set target
 *
 * Set target for rx DATA filter sub-type.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_by_stype_data(void *hal, u8 band,
						     struct rxfltr_cap_to_set_data *cap_data);

/**
 * rtw_hal_set_rxfltr_by_stype_all - Set target for rx CTRL/MGNT/DATA filter sub-type
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @cap_all:	data structure to set target
 *
 * Set target for rx CTRL/MGNT/DATA filter filter sub-type.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_by_stype_all(void *hal, u8 band,
						    struct rxfltr_type_cap_to_set *cap_all);
/**
 * rtw_hal_mac_acpt_crc_err_pkt - Accept CRC error packets or not
 * @hal:	pointer of struct rtw_hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @enable:	0: deny, 1: accept
 *
 * Control accepting CRC error packets or not.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_acpt_crc_err_pkt(void *hal, u8 band, u8 enable);

/**
 * rtw_hal_set_rxfltr_mpdu_size - Set max MPDU size
 * @hal:	pointer of struct hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @size:	MPDU max size, unit: byte. 0 for no limit.
 *
 * MPDU size exceed max size would be dropped.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_mpdu_size(void *hal, u8 band, u16 size);

/**
 * rtw_hal_set_rxfltr_by_type - Set target for rx CTRL/MGNT/DATA filter type
 * @hal:	pointer of struct rtw_hal_info_t
 * @band:	0x0: band0, 0x1: band1
 * @type:	enum rtw_packet_type
 * @target:	enum rtw_rxfltr_target
 *
 * Set target for rx CTRL/MGNT/DATA filter type.
 *
 * Return RTW_HAL_STATUS_SUCCESS when setting is ok.
 */
enum rtw_hal_status rtw_hal_set_rxfltr_by_type(void *hal, u8 band, enum rtw_packet_type type,
					       enum rtw_rxfltr_target target);

void rtw_hal_dbg_status_dump(void *hal, struct hal_mac_dbg_dump_cfg *cfg);
/******************************************************************************
 *
 * Packet Offload APIs
 *
 *****************************************************************************/
enum rtw_hal_status rtw_hal_reset_pkt_ofld_state(void *hal);

enum rtw_hal_status rtw_hal_pkt_ofld(void *hal, u8 *id, u8 op,
					u8 *pkt, u16 *len);
enum rtw_hal_status rtw_hal_pkt_update_ids(void *hal,
					struct pkt_ofld_entry *entry);
void rtw_hal_fw_cap_pre_config(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_bus_cap_pre_config(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_fw_final_cap_config(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_final_cap_decision(struct rtw_phl_com_t *phl_com, void *hal);

/******************************************************************************
 *
 * Power Save APIs
 *
 *****************************************************************************/

enum rtw_hal_status
rtw_hal_ps_pwr_lvl_cfg(struct rtw_phl_com_t *phl_com, void *hal,
			u32 req_pwr_lvl);
enum rtw_hal_status
rtw_hal_ps_lps_cfg(void *hal, struct rtw_hal_lps_info *lps_info);
enum rtw_hal_status
rtw_hal_ps_ips_cfg(void *hal, struct rtw_hal_ips_info *ips_info);
enum rtw_hal_status
rtw_hal_get_bf_proto_cap(struct rtw_phl_com_t *phl_com, void *hal,
			 u8 band, struct protocol_cap_t *proto_cap);

enum rtw_hal_status
rtw_hal_get_stbc_proto_cap(struct rtw_phl_com_t *phl_com, void *hal,
			 u8 band, struct protocol_cap_t *proto_cap);

enum rtw_hal_status rtw_hal_watchdog(void *hal, struct rtw_phl_com_t *phl_com);
enum rtw_hal_status rtw_hal_simple_watchdog(void *hal, u8 io_en);
void rtw_hal_ps_chk_hw_rf_state(struct rtw_phl_com_t *phl_com, void *hal);
void rtw_hal_ps_notify_wake(void *hal);
void rtw_hal_lps_pvb_wait_rx(void *hal, u16 macid, bool pvb_wait_rx);

enum rtw_hal_status
rtw_hal_lps_bcn_tracking_cfg(void *hal,
	struct rtw_bcn_tracking_cfg *cfg, struct rtw_bcn_tracking_cfg *cfg_cur);

/******************************************************************************
 *
 * TWT APIs
 *
 *****************************************************************************/
#ifdef CONFIG_PHL_TWT
enum rtw_hal_status
rtw_hal_twt_info_update(void *hal,
                        struct rtw_phl_twt_info twt_info,
                        struct rtw_wifi_role_link_t *rlink,
                        u8 action);

enum rtw_hal_status
rtw_hal_twt_sta_update(void *hal, u8 macid, u8 twt_id, u8 action);

enum rtw_hal_status
rtw_hal_twt_sta_announce(void *hal, u8 macid);
#endif /* CONFIG_PHL_TWT */

enum rtw_hal_status
rtw_hal_set_fw_ul_fixinfo(void *hal,
			  struct  rtw_phl_ax_ul_fixinfo *tbl);
/******************************************************************************
 *
 * BB APIs
 *
 *****************************************************************************/
/*@--------------------------[Prptotype]-------------------------------------*/
/**
 * rtw_hal_bb_get_txsc input arguments:
 * @hal_com: hal com info
 * @pri_ch: Spec-defined primary channel index
 * @central_ch: Spec-defined central channel index
 * @cbw: Channel BW
 * @dbw: Data BW
 */
u8 rtw_hal_bb_get_txsc(struct rtw_hal_com_t *hal_com, u8 pri_ch,
		u8 central_ch, enum channel_width cbw, enum channel_width dbw);

enum rtw_hal_status
rtw_hal_bb_lps_info_update(void *hal, u16 macid);

#ifdef CONFIG_DBCC_SUPPORT
void rtw_hal_dbcc_hci_ctrl(void *hal, enum phl_band_idx band_idx, u8 pause);
enum rtw_hal_status
rtw_hal_dbcc_trx_ctrl(void *hal, struct rtw_phl_com_t *phl_com,
				  enum phl_band_idx band_idx, bool pause);

enum rtw_hal_status
rtw_hal_dbcc_pre_cfg(void *hal, struct rtw_phl_com_t *phl_com, bool dbcc_en);

enum rtw_hal_status
rtw_hal_dbcc_cfg(void *hal, struct rtw_phl_com_t *phl_com, bool dbcc_en);

enum rtw_hal_status
rtw_hal_dbcc_reset_hal(void *hal);

enum rtw_hal_status
rtw_hal_dbcc_band_switch_hdl(void *hal, enum phl_band_idx band_idx);
#endif

/*
 * API for config channel info CR
 */
#ifdef CONFIG_PHL_CHANNEL_INFO
enum rtw_hal_status
rtw_hal_cfg_chinfo(void *hal, struct rtw_chinfo_action_parm *act_param);

enum rtw_hal_status
rtw_hal_ch_info_en(void *hal, struct rtw_chinfo_action_parm *act_param,
						u8 pkt_id);
#endif /* CONFIG_PHL_CHANNEL_INFO */

enum rtw_hal_status
rtw_hal_get_efuse_info(void *hal, enum rtw_efuse_info info_type,
		       void *value, u8 size);

enum rtw_hal_status
rtw_hal_cfg_trx_path(void *hal, enum rf_path tx, u8 tx_nss,
		     enum rf_path rx, u8 rx_nss);

enum rtw_hal_status
rtw_hal_tsf_sync(void *hal, u8 wrole_sync_from, u8 wrole_sync_to,
			enum phl_band_idx band, s32 sync_offset_tu,
			enum hal_tsf_sync_act act);

/**
 * rtw_hal_fill_txdesc() - Fill hardware tx header
 * @hal:	pointer of struct hal_info_t
 * @treq:	the xmit request for this tx descriptor
 * @wd_buf:	the wd buffer to fill
 * @wd_len:	output, return the total length of filled wd
 *
 * Fill hardware tx header/tx descriptor/wifi descriptor
 *
 * Return RTW_HAL_STATUS_SUCCESS when everything is ok.
 */
enum rtw_hal_status
rtw_hal_fill_txdesc(void *hal,
                    struct rtw_xmit_req *treq,
                    u8 *wd_buf,
                    u32 *wd_len);

enum rtw_hal_status rtw_hal_poll_hw_tx_done(void *hal);
enum rtw_hal_status rtw_hal_hw_tx_resume(void *hal);
enum rtw_hal_status rtw_hal_poll_hw_rx_done(void *hal);
enum rtw_hal_status rtw_hal_hw_rx_resume(void *hal);

enum rtw_hal_status
rtw_hal_get_hwseq(void *hal, u16 macid, u8 ref_sel, u8 ssn_sel, u16 *hw_seq);


#ifdef RTW_WKARD_DYNAMIC_BFEE_CAP
enum rtw_hal_status rtw_hal_bf_bfee_ctrl(void *hal, u8 band, bool ctrl);
#endif


enum rtw_hal_status
rtw_hal_set_mu_edca(void *hal, u8 band, u8 ac,
	u16 timer, u8 cw_min, u8 cw_max, u8 aifsn);
enum rtw_hal_status
rtw_hal_set_mu_edca_ctrl(void *hal, u8 band, u8 wmm, u8 set);

enum rtw_hal_status rtw_hal_led_set_ctrl_mode(void *hal, enum rtw_led_id led_id,
					      enum rtw_led_ctrl_mode ctrl_mode);
enum rtw_hal_status rtw_hal_led_control(void *hal, enum rtw_led_id led_id,
					u8 high);

enum rtw_hal_status rtw_hal_pcie_trx_mit(void *hal, u32 tx_timer, u8 tx_counter,
					 u32 rx_timer, u8 rx_counter);

enum rtw_hal_status rtw_hal_get_tsf(void *hal, enum phl_band_idx band,
				u8 port, u32 *tsf_h, u32 *tsf_l);

u32 rtw_hal_get_btc_req_slot(void *hal, enum phl_band_idx hw_band);

enum rtw_hal_status
rtw_hal_set_macid_pause(void *hal, u16 macid, bool pause);

enum rtw_hal_status
rtw_hal_set_macid_grp_pause(void *hal, u32 *macid_arr, u8 arr_size, bool pause);

enum rtw_hal_status
rtw_hal_set_macid_pkt_drop(void *hal, u16 macid, u8 sel, u8 band, u8 port,
                           u8 mbssid);

#ifdef CONFIG_MCC_SUPPORT
enum rtw_hal_status rtw_hal_mcc_get_2ports_tsf(void *hal, u8 group,
			u16 macid_x, u16 macid_y, u32 *tsf_x_h, u32 *tsf_x_l,
			u32 *tsf_y_h, u32 *tsf_y_l);

enum rtw_hal_status rtw_hal_notify_mcc_macid(void *hal,
                                             struct rtw_phl_mcc_role *mrole,
                                             enum rtw_phl_tdmra_wmode wmode);

enum rtw_hal_status rtw_hal_mcc_update_macid_bitmap(void *hal, u8 group,
			u16 macid, struct rtw_phl_mcc_macid_bitmap *info);

enum rtw_hal_status rtw_hal_mcc_sync_enable(void *hal,
					struct rtw_phl_mcc_en_info *info);

enum rtw_hal_status rtw_hal_mcc_change_pattern(void *hal,
				struct rtw_phl_mcc_en_info *ori_info,
				struct rtw_phl_mcc_en_info *new_info,
				struct rtw_phl_mcc_bt_info *new_bt_info);

enum rtw_hal_status rtw_hal_mcc_reset(void *hal, u8 group,
					enum rtw_phl_tdmra_wmode wmode);

enum rtw_hal_status rtw_hal_mcc_disable(void *hal, u8 group, u16 macid,
					enum rtw_phl_tdmra_wmode wmode);

enum rtw_hal_status rtw_hal_mcc_enable(void *hal, struct rtw_phl_mcc_en_info *info,
					struct rtw_phl_mcc_bt_info *bt_info,
					enum rtw_phl_tdmra_wmode wmode);
#endif /* CONFIG_MCC_SUPPORT */

#ifdef CONFIG_PHL_P2PPS
enum rtw_hal_status rtw_hal_noa_enable(void *hal,
	struct rtw_phl_noa_info *noa_info,
	struct rtw_phl_noa_desc *in_desc,
	u16 macid);

enum rtw_hal_status rtw_hal_noa_disable(void *hal,
	struct rtw_phl_noa_info *noa_info,
	struct rtw_phl_noa_desc *in_desc,
	u16 macid);

enum rtw_hal_status
rtw_hal_tsf32_tog_enable(void *hal,
                         struct rtw_wifi_role_link_t *rlink);

enum rtw_hal_status
rtw_hal_tsf32_tog_disable(void *hal,
                         struct rtw_wifi_role_link_t *rlink);

enum rtw_hal_status rtw_hal_get_tsf32_tog_rpt(void *hal,
	struct rtw_phl_tsf32_tog_rpt *rpt);

enum rtw_hal_status rtw_hal_noa_sta_macid_up(void *hal,
			u16 wrole_macid, bool join, u8 *bitmap, u32 bitmap_len);

#endif
void rtw_hal_disconnect_notify(void *hal, struct rtw_chan_def *chandef);

bool rtw_hal_check_ch_rfk(void *hal, struct rtw_chan_def *chandef);
enum rtw_hal_status rtw_hal_ppdu_sts_cfg(void *hal, u8 band_idx, bool en);
void rtw_hal_notification(void *hal, enum phl_msg_evt_id event, u8 hw_idx);
void rtw_hal_notification_ex(void *hal, enum phl_msg_evt_id event,
			     bool to_bb, bool to_mac, bool to_rf, u8 hw_idx);

void rtw_hal_cmd_notification(void *hal,
                   enum phl_msg_evt_id event,
                   void *hal_cmd,
                   u8 hw_idx);

enum rtw_hal_status
rtw_hal_config_rts_th(void *hal, u8 band_idx, u16 rts_time_th, u16 rts_len_th);

enum rtw_hal_status
rtw_hal_query_txsts_rpt(void *hal, u16 macid);

enum rtw_hal_status rtw_hal_set_dfs_tb_ctrl(void *hal, u8 set);
enum rtw_hal_status
rtw_hal_thermal_protect_cfg_tx_ampdu(
	void *hal,
	struct rtw_phl_stainfo_t *sta,
	u8 ratio);

bool rtw_hal_check_thermal_protect(
	struct rtw_phl_com_t *phl_com,
	void *hal);

enum rtw_hal_status
rtw_hal_beamform_set_aid(void *hal, struct rtw_phl_stainfo_t *sta, u16 aid);

#ifdef CONFIG_PHL_DRV_HAS_NVM
enum rtw_hal_status
rtw_hal_extract_efuse_info(void *hal, u8 *efuse_map,
                           enum rtw_efuse_info info_type,
                           void *value,
                           u8 size, u8 map_valid);

enum rtw_hal_status
rtw_hal_get_efuse_size(void *phl, u32 *log_efuse_size, u32 *limit_efuse_size,
                       u32 *mask_size, u32 *limit_mask_size);

enum rtw_hal_status
rtw_hal_nvm_apply_dev_cap(void *hal, struct rtw_phl_com_t *phl_com);

#endif /* CONFIG_RTW_DRV_HAS_NVM */

/******************************************************************************
 *
 * TX power APIs
 *
 *****************************************************************************/
int rtw_hal_get_pw_lmt_regu_type_from_str(void *hal, const char *str);
const char *rtw_hal_get_pw_lmt_regu_str_from_type(void *hal, u8 regu);

u8 rtw_hal_get_pw_lmt_regu_type(void *hal, enum band_type band);
const char *rtw_hal_get_pw_lmt_regu_type_str(void *hal, enum band_type band);

bool rtw_hal_pw_lmt_regu_tbl_exist(void *hal, enum band_type band, u8 regu);
u8 rtw_hal_ext_reg_codemap_search(void *hal, u16 domain_code, const char *country, const char **reg_name);

bool rtw_hal_get_pwr_lmt_en(void *hal, u8 band_idx);

void rtw_hal_auto_pw_lmt_regu(void *hal);
void rtw_hal_force_pw_lmt_regu(void *hal,
	u8 regu_2g[], u8 regu_2g_len, u8 regu_5g[], u8 regu_5g_len, u8 regu_6g[], u8 regu_6g_len);

u16 rtw_hal_get_pwr_constraint(void *hal, u8 band_idx);
enum rtw_hal_status rtw_hal_set_pwr_constraint(void *hal, u8 band_idx, u16 mb);

enum rtw_hal_status rtw_hal_set_tx_power(void *hal, u8 band_idx,
					enum phl_pwr_table pwr_table);

enum rtw_hal_status rtw_hal_get_txinfo_power(
	void *hal, s16 *txinfo_power_dbm);
/*****************************************************************************/

u32 rtw_hal_get_phy_stat_info(void *hal, enum phl_band_idx hw_band,
			      enum phl_stat_info_query phy_stat);

enum rtw_hal_status rtw_hal_get_rx_cnt_by_idx(void *hal,
					      enum phl_band_idx hw_band,
					      enum phl_rxcnt_idx idx,
					      u16 *rx_cnt);
enum rtw_hal_status rtw_hal_set_reset_rx_cnt(void *hal,
					     enum phl_band_idx hw_band);
#ifdef RTW_WKARD_AP_MP
void rtw_hal_bb_rx_ndp_mp(void *hal);
void rtw_hal_bb_dm_init_mp(void *hal);
void rtw_hal_rf_dm_init_mp(void *hal);
enum rtw_hal_status
rtw_hal_mac_set_rxfltr_mp_mode(void *hal, u8 band, u16 size);
#endif /*RTW_WKARD_AP_MP */

enum rtw_hal_status rtw_hal_bb_dump_tpu_info (void *hal);

u16 rtw_hal_get_ampdu_num(void *hal, u8 band);

enum rtw_chip_id rtw_hal_get_chip_id(void *hal);
#ifdef DBG_DUMP_TX_COUNTER
void rtw_hal_dump_tx_status(void *hal, enum phl_band_idx bidx);
#endif

enum rtw_hal_status rtw_hal_pwr_switch_mac(void *hal, bool on);

enum rtw_hal_status rtw_hal_cfg_hw_cts2self(void *hal, u8 band_sel, u8 enable,
					    u8 non_sec_thr, u8 sec_thr);
bool
rtw_hal_check_sta_has_busy_wp(struct rtw_phl_stainfo_t *sta);
#endif /*_HAL_API_H_*/
