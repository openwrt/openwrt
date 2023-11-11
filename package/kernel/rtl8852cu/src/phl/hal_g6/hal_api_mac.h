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
#ifndef _HAL_API_MAC_H_
#define _HAL_API_MAC_H_
#include "mac/mac_ax.h"

#define POLLING_HALMAC_TIME 5
#define POLLING_HALMAC_CNT 100

#define hal_to_mac(_halinfo)	((struct mac_ax_adapter *)((_halinfo)->mac))

#ifdef CONFIG_SDIO_HCI
u8 hal_mac_sdio_read8(struct rtw_hal_com_t *hal, u32 addr);
u16 hal_mac_sdio_read16(struct rtw_hal_com_t *hal, u32 addr);
u32 hal_mac_sdio_read32(struct rtw_hal_com_t *hal, u32 addr);
int hal_mac_sdio_write8(struct rtw_hal_com_t *hal, u32 addr, u8 value);
int hal_mac_sdio_write16(struct rtw_hal_com_t *hal, u32 addr, u16 value);
int hal_mac_sdio_write32(struct rtw_hal_com_t *hal, u32 addr, u32 value);
void hal_mac_sdio_read_mem(struct rtw_hal_com_t *hal,
					u32 addr, u32 cnt, u8 *pmem);

#ifdef CONFIG_SDIO_INDIRECT_ACCESS
u8 hal_mac_sdio_iread8(struct rtw_hal_com_t *hal, u32 addr);
u16 hal_mac_sdio_iread16(struct rtw_hal_com_t *hal, u32 addr);
u32 hal_mac_sdio_iread32(struct rtw_hal_com_t *hal, u32 addr);
#endif /* CONFIG_SDIO_INDIRECT_ACCESS */

#endif /* CONFIG_SDIO_HCI */

void rtw_hal_mac_get_version(char *ver_str, u16 len);
void rtw_hal_mac_get_fw_ver(struct hal_info_t *hal_info, char *ver_str, u16 len);

u16 hal_mac_get_macid_num(struct hal_info_t *hal);
void hal_mac_get_hwinfo(struct hal_info_t *hal, struct hal_spec_t *hal_spec);

#ifdef DBG_PHL_MAC_REG_RW
bool rtw_hal_mac_reg_chk(struct rtw_hal_com_t *hal_com, u32 addr);
#endif

u32 rtw_hal_mac_init(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal_info);
u32 rtw_hal_mac_deinit(struct rtw_phl_com_t *phl_com,
					struct hal_info_t *hal_info);
bool rtw_hal_mac_proc_cmd(struct hal_info_t *hal_info, struct rtw_proc_cmd *incmd,
						char *output, u32 out_len);

enum rtw_hal_status rtw_hal_mac_get_hwseq(void *hal, u16 macid, u8 ref_sel, u8 ssn_sel,
						u16 *hw_seq);

enum rtw_hal_status rtw_hal_mac_get_pwr_state(struct hal_info_t *hal_info,
						enum rtw_mac_pwr_st *pwr_state);

enum rtw_hal_status rtw_hal_mac_power_switch(struct rtw_phl_com_t *phl_com,
					     struct hal_info_t *hal_info,
					     u8 on_off);

enum rtw_hal_status rtw_hal_mac_dbcc_pre_cfg(struct rtw_phl_com_t *phl_com,
					     struct hal_info_t *hal_info,
					     u8 dbcc_en);
enum rtw_hal_status rtw_hal_mac_dbcc_cfg(struct rtw_phl_com_t *phl_com,
					 struct hal_info_t *hal_info,
					 u8 dbcc_en);
enum rtw_hal_status rtw_hal_mac_dbcc_trx_ctrl(struct hal_info_t *hal_info,
                                              enum phl_band_idx band_idx,
                                              bool pause);
enum rtw_hal_status rtw_hal_mac_dbcc_hci_ctrl(struct hal_info_t *hal_info,
                                              enum phl_band_idx band_idx, u8 pause);

enum rtw_hal_status
rtw_hal_mac_poll_hw_tx_done(struct hal_info_t *hal_info);
enum rtw_hal_status
rtw_hal_mac_hw_tx_resume(struct hal_info_t *hal_info);
enum rtw_hal_status
rtw_hal_mac_poll_hw_rx_done(struct hal_info_t *hal_info);
enum rtw_hal_status
rtw_hal_mac_hw_rx_resume(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_mac_watchdog(struct hal_info_t *hal_info, struct rtw_phl_com_t *phl_com);
#ifdef CONFIG_PCI_HCI
enum rtw_hal_status rtw_hal_mac_set_pcicfg(struct hal_info_t *hal_info,
					struct mac_ax_pcie_cfgspc_param *pci_cfgspc);
enum rtw_hal_status rtw_hal_mac_ltr_set_pcie(struct hal_info_t *hal_info,
					enum rtw_pcie_bus_func_cap_t hw_ctrl,
					u8 idle_ctrl, u32 idle_val, u8 act_ctrl, u32 act_val);

enum rtw_hal_status rtw_hal_mac_ltr_sw_trigger(struct hal_info_t *hal_info, enum rtw_pcie_ltr_state state);
enum rtw_hal_status hal_mac_set_l2_leave(struct hal_info_t *hal_info);

enum rtw_hal_status rtw_hal_mac_poll_txdma_idle(struct hal_info_t *hal,
					struct mac_ax_txdma_ch_map *ch_map);

enum rtw_hal_status rtw_hal_mac_poll_rxdma_idle(struct hal_info_t *hal,
					struct mac_ax_rxdma_ch_map *ch_map);

enum rtw_hal_status rtw_hal_mac_clr_bdidx(struct hal_info_t *hal);

enum rtw_hal_status rtw_hal_mac_rst_bdram(struct hal_info_t *hal);

enum rtw_hal_status rtw_hal_mac_cfg_txdma(struct hal_info_t *hal,
					struct mac_ax_txdma_ch_map *ch_map);

enum rtw_hal_status rtw_hal_mac_cfg_dma_io(struct hal_info_t *hal, u8 en);

enum rtw_hal_status
rtw_hal_mac_tx_res_query(struct hal_info_t *hal, u8 ch_idx, u16 *host_idx,
                         u16 *hw_idx, u16 *avail_txbd);
enum rtw_hal_status
rtw_hal_mac_rx_res_query(struct hal_info_t *hal, u8 ch_idx, u16 *host_idx,
                         u16 *hw_idx, u16 *avail_rxbd);
enum rtw_hal_status
rtw_hal_mac_trigger_txstart(struct hal_info_t *hal,
                            struct tx_base_desc *txbd_ring, u8 ch_idx);
enum rtw_hal_status
rtw_hal_mac_notify_rxdone(struct hal_info_t *hal, struct rx_base_desc *rxbd,
                          u8 ch_idx);
#endif


#ifdef CONFIG_USB_HCI
u8 hal_mac_get_bulkout_id(struct hal_info_t *hal, u8 ch_dma, u8 mode);
u32 hal_mac_usb_tx_agg_cfg(struct hal_info_t *hal, u8* wd_buf, u8 agg_num);
u32 hal_mac_usb_rx_agg_cfg(struct hal_info_t *hal, u8 agg_mode,
	u8 drv_define, u8 timeout, u8 size, u8 pkt_num);
u8 hal_mac_usb_get_max_bulkout_wd_num(struct hal_info_t *hal);
u16 hal_mac_usb_get_max_dma_txagg_msk(struct hal_info_t *hal);
enum rtw_hal_status hal_mac_force_usb_switch(struct hal_info_t *hal);
u32 hal_mac_get_cur_usb_mode(struct hal_info_t *hal);
u32 hal_mac_get_usb_support_ability(struct hal_info_t *hal);
#endif

#ifdef CONFIG_SDIO_HCI
void rtw_hal_mac_sdio_cfg(struct rtw_phl_com_t *phl_com,
	struct hal_info_t *hal_info, struct rtw_ic_info *ic_info);
void rtw_hal_mac_sdio_tx_cfg(struct rtw_hal_com_t *hal);
void rtw_hal_mac_sdio_rx_agg_cfg(struct rtw_hal_com_t *hal, bool enable,
				 u8 drv_define, u8 timeout, u8 size, u8 pkt_num);
bool rtw_hal_mac_sdio_check_tx_allow(struct rtw_hal_com_t *hal, u8 dma_ch,
				     u8 *buf, u32 len, u8 agg_count,
				     u16 *pkt_len, u8 *wp_offset, u32 *txaddr,
				     u32 *txlen);
int rtw_hal_mac_sdio_parse_rx(struct rtw_hal_com_t *hal,
			      struct rtw_rx_buf *rxbuf);
int rtw_hal_mac_sdio_rx(struct rtw_hal_com_t *hal,
			struct rtw_rx_buf *rxbuf);
void rtw_hal_mac_sdio_lps_flg(struct rtw_hal_com_t *hal, u8 *flg);
#endif

struct hal_init_info_t {
	struct mac_ax_trx_info trx_info;
	struct mac_ax_intf_info intf_info;
	char *ic_name;
};
enum rtw_hal_status
rtw_hal_mac_init_mac(void *mac, struct hal_init_info_t *init_info);

enum rtw_hal_status
rtw_hal_mac_trx_init(void *mac, struct hal_init_info_t *init_info);

enum rtw_hal_status
rtw_hal_mac_hal_init(struct rtw_phl_com_t *phl_com,
		     struct hal_info_t *hal_info,
		     struct hal_init_info_t *init_info);

enum rtw_hal_status
rtw_hal_mac_hal_fast_init(struct rtw_phl_com_t *phl_com,
			  struct hal_info_t *hal_info,
			  struct hal_init_info_t *init_info);

enum rtw_hal_status
rtw_hal_mac_hal_deinit(struct rtw_phl_com_t *phl_com, struct hal_info_t *hal_info);

enum rtw_hal_status rtw_hal_mac_chk_allq_empty(struct hal_info_t *hal_info, u8 *empty);

#ifdef CONFIG_WOWLAN
enum rtw_hal_status
rtw_hal_mac_cfg_wow_sleep(struct hal_info_t *hal_info, u8 sleep);
enum rtw_hal_status
rtw_hal_mac_get_wow_fw_status(struct hal_info_t *hal_info, u8 *status, u8 func_en);

enum rtw_hal_status
rtw_hal_mac_cfg_wow_wake(struct hal_info_t *hal_info, u16 macid, u8 en, struct rtw_wow_wake_info *info);

enum rtw_hal_status
rtw_hal_mac_cfg_disc_dec(struct hal_info_t *hal_info, u16 macid, u8 en, struct rtw_disc_det_info *info);

enum rtw_hal_status
rtw_hal_mac_cfg_dev2hst_gpio(struct hal_info_t *hal_info, u8 en, struct rtw_wow_gpio_info *cfg);

enum rtw_hal_status
rtw_hal_mac_cfg_keep_alive(struct hal_info_t *hal_info, u16 macid, u8 en, struct rtw_keep_alive_info *info);

enum rtw_hal_status
rtw_hal_mac_cfg_ndp_ofld(struct hal_info_t *hal_info, u16 macid, u8 en, struct rtw_ndp_ofld_info *cfg);

enum rtw_hal_status
rtw_hal_mac_cfg_arp_ofld(struct hal_info_t *hal_info, u16 macid, u8 en, struct rtw_arp_ofld_info *cfg);

enum rtw_hal_status
rtw_hal_mac_cfg_gtk_ofld(struct hal_info_t *hal_info, u16 macid, u8 en, struct rtw_gtk_ofld_info *cfg);

enum rtw_hal_status
rtw_hal_mac_cfg_realwow(struct hal_info_t *hal_info, u16 macid, u8 en, struct rtw_realwow_info *cfg);

enum rtw_hal_status rtw_hal_mac_get_wake_rsn(struct hal_info_t *hal_info, u8 *wake_rsn, u8 *reset);

enum rtw_hal_status
rtw_hal_mac_cfg_wow_cam(struct hal_info_t *hal_info, u16 macid, u8 en, struct rtw_pattern_match_info *cfg);

enum rtw_hal_status
rtw_hal_mac_get_aoac_rpt(struct hal_info_t *hal_info, struct rtw_aoac_report *aoac_info, u8 rx_ready);

enum rtw_hal_status rtw_hal_mac_set_wowlan(struct hal_info_t *hal, u8 enter);
enum rtw_hal_status rtw_hal_mac_wow_chk_txq_empty(struct hal_info_t *hal, u8 *empty);
enum rtw_hal_status rtw_hal_mac_wow_wde_drop(struct hal_info_t *hal, u8 band);
enum rtw_hal_status
rtw_hal_mac_cfg_nlo(struct hal_info_t *hal, u16 macid, u8 en,
		    struct rtw_nlo_info *cfg);
enum rtw_hal_status
rtw_hal_mac_cfg_periodic_wake(struct hal_info_t *hal, u16 macid, u8 en,
			      struct rtw_periodic_wake_info *cfg);
#endif /* CONFIG_WOWLAN */

#ifdef CONFIG_PHL_SCANOFLD
enum rtw_hal_status
rtw_hal_mac_scan_ofld_add_ch(struct hal_info_t *hal, u8 hw_band,
                             struct scan_ofld_ch_info *cfg, bool ofld);

enum rtw_hal_status
rtw_hal_mac_scan_ofld_fw_busy(struct hal_info_t *hal, u8 hw_band);

enum rtw_hal_status
rtw_hal_mac_scan_ofld_chlist_busy(struct hal_info_t *hal, u8 hw_band);

enum rtw_hal_status
rtw_hal_mac_scan_ofld(struct hal_info_t *hal, u16 mac_id, u8 hw_band, u8 hw_port,
                      struct scan_ofld_info *cfg);

u8 rtw_hal_mac_scan_ofld_trans_notify_rsn(u8 mac_rsn);

void rtw_hal_mac_scan_ofld_leave_cur_chnl(struct hal_info_t *hal, u8 hw_band,
					  u8 ch_band, u8 ch);
#endif /* CONFIG_PHL_SCANOFLD */

u32
rtw_hal_mac_get_txdesc_len(void *mac, struct rtw_xmit_req *treq);

enum rtw_hal_status
rtw_hal_mac_fill_txdesc(void *mac,
                        struct rtw_xmit_req *treq,
                        u8 *wd_buf,
                        u32 *wd_len);

enum rtw_hal_status
rtw_hal_mac_enable_cpu(struct hal_info_t *hal_info, u8 reason, u8 dlfw);

enum rtw_hal_status
rtw_hal_mac_disable_cpu(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_mac_romdl(struct hal_info_t *hal_info, u8 *rom_buf, u32 rom_size);

enum rtw_hal_status
rtw_hal_mac_fwdl(struct hal_info_t *hal_info, u8 *fw_buf, u32 fw_size);

enum rtw_hal_status
rtw_hal_mac_enable_fw(struct hal_info_t *hal_info, u8 fw_type);

enum rtw_hal_status
rtw_hal_mac_set_hw_ampdu_cfg(struct hal_info_t *hal_info, u8 band,
								  u16 max_agg_num, u8 max_agg_time);

enum rtw_hal_status
rtw_hal_mac_hwamsdu_macid_en(struct hal_info_t *hal_info,
			   u8 macid,
			   u8 enable);

enum rtw_hal_status
rtw_hal_mac_enable_hwamsdu(struct hal_info_t *hal_info,
			   u8 enable,
			   u8 max_num,
			   u8 en_single_amsdu,
			   u8 en_last_amsdu_padding);

enum rtw_hal_status
rtw_hal_mac_hwamsdu_max_len(struct hal_info_t *hal_info,
			   u8 macid,
			   u8 amsdu_max_len);

enum rtw_hal_status
rtw_hal_mac_hdr_conv_tx_macid_en(struct hal_info_t *hal_info,
				 u8 macid,
				 u8 qos_field_h,
				 u8 qos_field_h_en,
				 u8 mhdr_len,
				 u8 vlan_tag_valid);

enum rtw_hal_status rtw_hal_dmc_tbl_cfg(struct hal_info_t *hal_info,
					struct mac_ax_dctl_info *dctl_info,
					struct mac_ax_dctl_info *dctl_info_mask,
					u16 macid);

enum rtw_hal_status rtw_hal_cmc_tbl_cfg(struct hal_info_t *hal_info,
				struct rtw_hal_mac_ax_cctl_info *cctl_info,
				struct rtw_hal_mac_ax_cctl_info *cctl_info_mask,
				u16 macid);

enum rtw_hal_status rtw_hal_bacam_cfg(struct hal_info_t *hal_info,
				      struct mac_ax_bacam_info *ba_cam);

enum rtw_hal_status
rtw_hal_mac_port_init(struct hal_info_t *hal_info,
                      struct rtw_wifi_role_t *wifi_role,
                      struct rtw_wifi_role_link_t *rlink);

enum rtw_hal_status
rtw_hal_mac_port_deinit(struct hal_info_t *hal_i,
				struct rtw_wifi_role_link_t *rlink);

enum rtw_hal_status
rtw_hal_mac_port_cfg(struct hal_info_t *hal_info,
                     struct rtw_wifi_role_link_t *rlink,
                     enum pcfg_type type,
                     void *param);

enum rtw_hal_status
rtw_hal_mac_addr_cam_add_entry(struct hal_info_t *hal_info,
					struct rtw_phl_stainfo_t *sta);
enum rtw_hal_status
rtw_hal_mac_addr_cam_change_entry(struct hal_info_t *hal_info,
					struct rtw_phl_stainfo_t *sta,
					enum phl_upd_mode mode,
					bool is_connect);
enum rtw_hal_status
rtw_hal_mac_addr_cam_del_entry(struct hal_info_t *hal_info,
					struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_mac_add_key(struct hal_info_t *hal_info, u8 macid, u8 type, u8 ext_key,
					u8 spp,	u8 keyid, u8 keytype, u8 *keybuf);
enum rtw_hal_status
rtw_hal_mac_delete_key(struct hal_info_t *hal_info, u8 macid, u8 type,
						u8 ext_key, u8 spp, u8 keyid, u8 keytype);

u32
rtw_hal_mac_search_key_idx(struct hal_info_t *hal_info, u8 macid, u8 keyid, u8 keytype);

enum rtw_hal_status
rtw_hal_mac_set_fw_ul_fixinfo(void *hal,
			struct rtw_phl_ax_ul_fixinfo *tbl);

u32
rtw_hal_mac_ser_reset_wdt_intr(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_mac_ser_get_error_status(struct hal_info_t *hal_info, u32 *err);

bool rtw_hal_mac_ser_chk_ser_l1(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_mac_ser_set_error_status(struct hal_info_t *hal_info, enum RTW_PHL_SER_RCVY_STEP err);

enum rtw_hal_status
rtw_hal_mac_trigger_cmac_err(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_mac_trigger_dmac_err(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_mac_lv1_rcvy(struct hal_info_t *hal_info, enum rtw_phl_ser_lv1_recv_step step);

enum rtw_hal_status rtw_hal_mac_ser_ctrl(struct hal_info_t *hal_info,
			enum rtw_hal_ser_rsn rsn, bool en);

enum rtw_hal_status
rtw_hal_mac_dump_fw_rsvd_ple(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_mac_set_bw(struct hal_info_t *hal_info,	u8 band_idx, u8 pri_ch,
		u8 central_ch_seg0, u8 central_ch_seg1,	enum band_type band, enum channel_width bw);

enum rtw_hal_status
rtw_hal_mac_ax_set_bf_entry(void *mac, u8 band, u8 macid,
			    u8 bfee_idx, u8 txbf_idx, u16 buffer_idx);

enum rtw_hal_status
rtw_hal_mac_ax_get_snd_sts(void *mac, u8 band, u8 bfee_idx);

enum rtw_hal_status
rtw_hal_mac_ax_bfee_para_reg(void *mac, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_mac_ax_bfee_para_cctl(void *mac, struct rtw_phl_stainfo_t *sta);

enum rtw_hal_status
rtw_hal_mac_bfee_set_vht_gid(struct hal_info_t *hal,
			     u8 band, struct rtw_phl_gid_pos_tbl *tbl);

enum rtw_hal_status
rtw_hal_mac_ax_hw_snd_control(void *mac, u8 band, u8 hw_snd_ctrl);

enum rtw_hal_status
rtw_hal_mac_ax_mu_sta_upd(void *mac, u8 macid, u8 bfmu_idx,
			enum rtw_hal_protection_type prot_type,
			enum rtw_hal_ack_resp_type resp_type, u8 mugrp_bm);

enum rtw_hal_status
rtw_hal_mac_ax_mu_decision_para(void *mac, u32 mu_thold,
	bool bypass_thold, bool bypass_tp);

enum rtw_hal_status
rtw_hal_mac_ax_set_mu_fix_mode(
	void *mac, u8 gid, enum rtw_hal_protection_type prot_type,
	enum rtw_hal_ack_resp_type resp_type,
	bool fix_mu, bool he, bool fix_resp, bool fix_prot);

enum rtw_hal_status
rtw_hal_mac_ax_set_mu_table_whole(void *mac, void *hal_score_tbl);

enum rtw_hal_status
rtw_hal_mac_ax_snd_fx_cmd(void *mac, u8 *cmd_buf);

enum rtw_hal_status
rtw_hal_mac_parse_c2h(void *hal, u8 *buf, u32 buf_len, void *c2h);

enum rtw_hal_status
rtw_hal_mac_ax_parse_ppdu_sts(void *hal, u8 mac_valid, u8 *buf, u16 buf_l,
							  void *ppdu_sts, void *rx_mdata);
enum rtw_hal_status
rtw_hal_hdr_conv_cfg(struct hal_info_t *hal_info, u8 en_hdr_conv);
enum rtw_hal_status
rtw_hal_mac_enable_bb_rf(struct hal_info_t *hal_info, u8 enable);

#ifdef RTW_PHL_BCN
enum rtw_hal_status
hal_mac_ax_config_beacon(struct hal_info_t *hal, struct rtw_bcn_entry *bcn_entry);

enum rtw_hal_status
hal_mac_ax_send_beacon(struct hal_info_t *hal, struct rtw_bcn_entry *bcn_entry);
#endif

enum rtw_hal_status
rtw_hal_mac_ppdu_stat_cfg(struct hal_info_t *hal_info,
				u8 band_idx, bool ppdu_stat_en,
				u8 appen_info, u8 filter);
enum rtw_hal_status rtw_hal_mac_config_hw_mgnt_sec( struct hal_info_t *hal_info, u8 en);
enum rtw_hal_status rtw_hal_mac_get_append_fcs(struct hal_info_t *hal_info, u8 *val);
enum rtw_hal_status rtw_hal_mac_get_acpt_icv_err(struct hal_info_t *hal_info, u8 *val);


#ifdef CONFIG_PHL_CHANNEL_INFO
enum rtw_hal_status
rtw_hal_mac_chan_info_cfg(struct hal_info_t *hal_info,
				bool chinfo_en, u8 macid,
				enum phl_chinfo_mode mode, u8 filter, u8 sg_size);

enum rtw_hal_status
rtw_hal_mac_cfg_sensing_csi(struct hal_info_t *hal_info,
							u8 macid, u8 en, u16 period, u8 retry_cnt,
							u16 rate, u8 pkt_num, u8 *pkt_id);
#endif

void rtw_hal_mac_dbg_status_dump(struct hal_info_t *hal, struct hal_mac_dbg_dump_cfg *cfg);

#ifdef CONFIG_PHL_DFS
enum rtw_hal_status
rtw_hal_mac_dfs_rpt_cfg(struct hal_info_t *hal_info, struct hal_mac_dfs_rpt_cfg *conf);
enum rtw_hal_status
rtw_hal_mac_parse_dfs(struct hal_info_t *hal_info,
			u8 *buf, u32 buf_len, struct mac_ax_dfs_rpt *dfs_rpt);
#endif /*CONFIG_PHL_DFS*/
u32
rtw_hal_mac_lamode_trig(struct rtw_hal_com_t *hal_com, u8 trig);

enum rtw_hal_status
rtw_hal_mac_lamode_cfg_buf(struct rtw_hal_com_t *hal_com, u8 buf_sel,
			   u32 *addr_start, u32 *addr_end);

enum rtw_hal_status
rtw_hal_mac_lamode_cfg(struct rtw_hal_com_t *hal_com, u8 func_en,
		       u8 restart_en, u8 timeout_en, u8 timeout_val,
		       u8 data_loss_imr, u8 la_tgr_tu_sel, u8 tgr_time_val);
enum rtw_hal_status
rtw_hal_mac_get_lamode_st(struct rtw_hal_com_t *hal_com, u8 *la_state,
			  u16 *la_finish_addr, bool *la_round_up,
			  bool *la_loss_data);

void
rtl_hal_dump_sec_cam_tbl(struct rtw_hal_com_t *hal_com);

enum rtw_hal_status
rtw_hal_mac_set_rxfltr_opt_by_mode(struct rtw_hal_com_t *hal_com, u8 band,
				   enum rtw_rx_fltr_opt_mode mode);
enum rtw_hal_status
rtw_hal_mac_set_rxfltr_acpt_crc_err(struct rtw_hal_com_t *hal_com,
				    u8 band, u8 enable);
enum rtw_hal_status
rtw_hal_mac_set_rxfltr_mpdu_size(struct rtw_hal_com_t *hal_com,
				 u8 band, u16 size);
enum rtw_hal_status
rtw_hal_mac_set_rxfltr_by_type(struct rtw_hal_com_t *hal_com, u8 band,
			       enum rtw_packet_type type, enum rtw_rxfltr_target target);
enum rtw_hal_status
rtw_hal_mac_set_rxfltr_by_subtype(struct rtw_hal_com_t *hal_com, u8 band,
				  enum rtw_packet_type type, u8 subtype, enum rtw_rxfltr_target target);

enum rtw_hal_status
rtw_hal_mac_fw_log_cfg(struct rtw_hal_com_t *hal_com,
			struct rtw_hal_fw_log_cfg *fl_cfg);

enum rtw_hal_status rtw_hal_set_macid_pause(void *hinfo,
                                            u16 macid,
                                            bool pause);

void
rtw_hal_mac_get_buffer_data(struct rtw_hal_com_t *hal_com, u32 strt_addr,
			    u8 *buf, u32 len, u32 dbg_path);

enum rtw_hal_status rtw_hal_mac_pkt_ofld(struct hal_info_t *hal, u8 *id, u8 op,
						u8 *pkt, u16 *len);
enum rtw_hal_status rtw_hal_mac_pkt_update_ids(struct hal_info_t *hal,
						struct pkt_ofld_entry *entry);

enum rtw_hal_status rtw_hal_mac_reset_pkt_ofld_state(struct hal_info_t *hal_info);


enum rtw_hal_status
rtw_hal_mac_set_edca(struct rtw_hal_com_t *hal_com,
                     u8 band,
                     u8 wmm,
                     u8 ac,
                     u32 param);
enum rtw_hal_status
rtw_hal_mac_get_ampdu_cfg(struct rtw_hal_com_t *hal_com,
                          u8 band,
                          struct mac_ax_ampdu_cfg *cfg);
enum rtw_hal_status
rtw_hal_mac_set_rty_lmt(struct rtw_hal_com_t *hal_com, u8 macid,
	u8 rts_lmt_sel, u8 rts_lmt_val, u8 data_lmt_sel, u8 data_lmt_val);
enum rtw_hal_status
rtw_hal_mac_is_tx_mgnt_empty(struct hal_info_t *hal_info, u8 band, u8 *st);

enum rtw_hal_status
rtw_hal_mac_fw_dbg_dump(struct hal_info_t *hal_info);
enum rtw_fw_status
rtw_hal_mac_get_fw_status(struct hal_info_t *hal_info);
enum rtw_hal_status
rtw_hal_mac_ps_notify_wake(struct hal_info_t *hal_info);
enum rtw_hal_status
rtw_hal_mac_lps_pvb_wait_rx(struct hal_info_t *hal_info,
                            u16 macid, bool pvb_wait_rx);
enum rtw_hal_status
rtw_hal_mac_req_pwr_state(struct hal_info_t *hal_info, u8 pwr_state);
enum rtw_hal_status
rtw_hal_mac_chk_pwr_state(struct hal_info_t *hal_info, u8 pwr_state, u32 *mac_sts);
enum rtw_hal_status
rtw_hal_mac_lps_cfg(struct hal_info_t *hal_info,
			struct rtw_hal_lps_info *lps_info);
enum rtw_hal_status
rtw_hal_mac_lps_chk_leave(struct hal_info_t *hal_info, u16 macid, u32 *mac_sts);
enum rtw_hal_status
rtw_hal_mac_ips_cfg(struct hal_info_t *hal_info, u16 macid, bool enable);
enum rtw_hal_status
rtw_hal_mac_ips_chk_leave(struct hal_info_t *hal_info, u16 macid);

enum rtw_hal_status
rtw_hal_mac_lps_chk_access(struct hal_info_t *hal_info, u32 offset);

enum rtw_hal_status
rtw_hal_mac_lps_bcn_tracking_cfg(struct hal_info_t *hal,
	struct rtw_bcn_tracking_cfg *cfg, struct rtw_bcn_tracking_cfg *cfg_cur);

enum rtw_hal_status
hal_mac_ax_send_fw_snd(struct hal_info_t *hal_info,
		       struct hal_ax_fwcmd_snd *hal_cmd);

enum rtw_hal_status
rtw_hal_mac_tx_mode_sel(struct hal_info_t *hal_info, u8 fw_tx, u8 txop_wmm_en_bm);
enum rtw_hal_status
rtw_hal_mac_get_tx_cnt(struct hal_info_t *hal, enum phl_band_idx bidx, u8 sel);

enum rtw_hal_status
rtw_hal_mac_get_rx_cnt(struct hal_info_t *hal_info, u8 cur_phy_idx, u8 type_idx, u32 *ret_value);
enum rtw_hal_status
rtw_hal_mac_get_rx_cnt_by_idx(struct hal_info_t *hal_info, u8 cur_phy_idx,
			      u8 cnt_idx, u16 *ret_value);
enum rtw_hal_status
rtw_hal_mac_set_reset_rx_cnt(struct hal_info_t *hal_info, u8 cur_phy_idx);

#ifdef CONFIG_PHL_TWT
enum rtw_hal_status
rtw_hal_mac_twt_info_update(void *hal,
                            struct rtw_phl_twt_info twt_info,
                            struct rtw_wifi_role_link_t *rlink,
                            u8 action);

enum rtw_hal_status
rtw_hal_mac_twt_sta_update(void *hal, u8 macid, u8 twt_id, u8 action);

enum rtw_hal_status
rtw_hal_mac_twt_sta_announce(void *hal, u8 macid);
#endif /* CONFIG_PHL_TWT */

enum rtw_hal_status
rtw_hal_mac_ax_bfee_set_csi_rrsc(void *mac, u8 band, u32 rrsc);

u32 rtw_hal_mac_process_c2h(void *hal, struct rtw_c2h_info *c2h, struct c2h_evt_msg *c2h_msg);
void rtw_hal_mac_upd_c2h(void *hal, struct rtw_c2h_info *c2h, u32 evt_id, struct c2h_evt_msg *c2h_msg);

enum rtw_hal_status
rtw_hal_mac_set_mu_edca(struct rtw_hal_com_t *hal_com, u8 band, u8 ac,
	u16 timer, u8 cw_min, u8 cw_max, u8 aifs);
enum rtw_hal_status
rtw_hal_mac_set_mu_edca_ctrl(struct rtw_hal_com_t *hal_com,
	u8 band, u8 wmm, u8 set);

enum rtw_hal_status rtw_hal_mac_led_set_ctrl_mode(struct hal_info_t *hal_info,
						  enum mac_ax_led_mode mode,
						  u8 led_id);
enum rtw_hal_status rtw_hal_mac_led_ctrl(struct hal_info_t *hal_info, u8 high,
					 u8 led_id);

enum rtw_hal_status rtw_hal_mac_sw_gpio_ctrl(struct hal_info_t *hal_info, u8 high,
					 u8 gpio);
enum rtw_hal_status rtw_hal_mac_set_sw_gpio_mode(struct hal_info_t *hal_info, enum rtw_gpio_mode mode,
					 u8 gpio);

enum rtw_hal_status rtw_hal_mac_get_wl_dis_val(struct hal_info_t *hal_info, u8 *val);

enum rtw_hal_status
rtw_hal_mac_pcie_trx_mit(struct hal_info_t *hal_info,
			 struct mac_ax_pcie_trx_mitigation *mit_info);

enum rtw_hal_status
rtw_hal_mac_tsf_sync(struct hal_info_t *hal_info,
						u8 from_port, u8 to_port, enum phl_band_idx band,
						s32 sync_offset_tu, enum hal_tsf_sync_act action);

enum rtw_hal_status
rtw_hal_mac_get_sec_cam(struct hal_info_t *hal_info, u16 num, u8 *buf, u16 size);
enum rtw_hal_status
rtw_hal_mac_get_addr_cam(struct hal_info_t *hal_info, u16 num, u8 *buf, u16 size);

enum rtw_hal_status rtw_hal_mac_get_tsf(struct hal_info_t *hal,
		enum phl_band_idx band, u8 port, u32 *tsf_h, u32 *tsf_l);

enum rtw_hal_status rtw_hal_mac_cfg_txhci(struct hal_info_t *hal,u8 en);

enum rtw_hal_status rtw_hal_mac_cfg_rxhci(struct hal_info_t *hal,u8 en);

#ifdef CONFIG_MCC_SUPPORT
enum rtw_hal_status rtw_hal_mac_add_mcc(struct hal_info_t *hal,
					struct rtw_phl_mcc_role *mcc_role);

enum rtw_hal_status rtw_hal_mac_start_mcc(struct hal_info_t *hal,
		u8 group, u8 macid, u32 tsf_high, u32 tsf_low, u8 btc_in_group,
		u8 old_group_action, u8 old_group);

enum rtw_hal_status rtw_hal_mac_stop_mcc(struct hal_info_t *hal, u8 group,
					u8 macid);

enum rtw_hal_status rtw_hal_mac_reset_mcc_group(struct hal_info_t *hal, u8 group);

enum rtw_hal_status rtw_hal_mac_del_mcc_group(struct hal_info_t *hal, u8 group);

enum rtw_hal_status rtw_hal_mac_mcc_request_tsf(struct hal_info_t *hal,
					u8 group, u8 macid_x, u8 macid_y);

enum rtw_hal_status rtw_hal_mac_mcc_macid_bitmap(struct hal_info_t *hal,
					u8 group, u8 macid, u8 *bitmap, u8 len);

enum rtw_hal_status rtw_hal_mac_mcc_sync_enable(struct hal_info_t *hal,
				u8 group, u8 source, u8 target, u8 offset);

enum rtw_hal_status rtw_hal_mac_set_duration(struct hal_info_t *hal,
					struct rtw_phl_mcc_en_info *en_info,
					struct rtw_phl_mcc_bt_info *bt_info);

enum rtw_hal_status rtw_hal_mac_get_mcc_tsf_rpt(struct hal_info_t *hal,
					u8 group, u32 *tsf_x_h, u32 *tsf_x_l,
					u32 *tsf_y_h, u32 *tsf_y_l);

enum rtw_hal_status rtw_hal_mac_get_mcc_status_rpt(struct hal_info_t *hal,
				u8 group, u8 *status, u32 *tsf_h, u32 *tsf_l);

enum rtw_hal_status rtw_hal_mac_get_mcc_group(struct hal_info_t *hal, u8 *group);
#endif /* CONFIG_MCC_SUPPORT */
void rtw_hal_mac_notification(struct hal_info_t *hal_info,
                              enum phl_msg_evt_id event,
                              u8 band);

void rtw_hal_mac_cmd_notification(struct hal_info_t *hal_info,
                              void *hal_cmd,
                              u8 band);

enum rtw_hal_status rtw_hal_mac_set_hw_rts_th(struct hal_info_t *hal, u8 band,
					      u16 time_th, u16 len_th);

enum rtw_hal_status rtw_hal_mac_set_dfs_tb_ctrl(struct hal_info_t *hal, u8 set);

enum rtw_hal_status
rtw_hal_mac_trigger_fw_conflict(struct hal_info_t *hal_com, u32 addr, u8 vol);

u8 rtw_hal_mac_get_efuse_ver_len(struct rtw_hal_com_t *hal_com);

#ifdef CONFIG_PHL_P2PPS

enum rtw_hal_status rtw_hal_mac_p2p_macid_ctrl(struct hal_info_t *hal,
	bool join,
	u16 main_macid,
	u8 *bitmap,
	u32 bitmap_len
	);

enum rtw_hal_status rtw_hal_mac_noa_init(struct hal_info_t *hal,
	struct rtw_phl_noa_info *noa_info,
	struct rtw_phl_noa_desc *in_desc,
	u16 macid);

enum rtw_hal_status rtw_hal_mac_noa_update(struct hal_info_t *hal,
	struct rtw_phl_noa_info *noa_info,
	struct rtw_phl_noa_desc *in_desc,
	u16 macid);

enum rtw_hal_status rtw_hal_mac_noa_remove(struct hal_info_t *hal,
	struct rtw_phl_noa_info *noa_info,
	struct rtw_phl_noa_desc *in_desc,
	u16 macid);

enum rtw_hal_status rtw_hal_mac_noa_terminate(struct hal_info_t *hal,
	struct rtw_phl_noa_info *noa_info,
	struct rtw_phl_noa_desc *in_desc,
	u16 macid);

enum rtw_hal_status rtw_hal_mac_tsf32_tog_enable(struct hal_info_t *hal,
	u8 hw_band,
	u8 port,
	u16 early);

enum rtw_hal_status rtw_hal_mac_tsf32_tog_disable(struct hal_info_t *hal,
	u8 hw_band,
	u8 port);

enum rtw_hal_status rtw_hal_mac_get_tsf32_tog_rpt(struct hal_info_t *hal,
	struct rtw_phl_tsf32_tog_rpt *rpt);

#endif
enum rtw_hal_status
rtw_hal_mac_addr_cam_set_aid(struct hal_info_t *hal_info,
			     struct rtw_phl_stainfo_t *sta,
			     u16 aid);

enum rtw_hal_status
rtw_hal_mac_set_tx_lifetime(struct hal_info_t *hal, enum phl_band_idx band,
			    enum phl_lifetime_queue que, bool en, u16 val);

enum rtw_hal_status
rtw_hal_mac_set_gt3(struct hal_info_t *hal, u8 en, u32 timeout);

#ifdef CONFIG_RTW_MULTI_DEV_MULTI_BAND
enum rtw_hal_status
rtw_hal_mac_set_xtal_aac(struct rtw_hal_com_t *hal_com, u8 value);
#endif /* CONFIG_RTW_MULTI_DEV_MULTI_BAND */

enum rtw_hal_status
rtw_hal_mac_patch_rx_rate(struct hal_info_t *hal_info, struct rtw_r_meta_data *mdata);

enum rtw_hal_status
rtw_hal_mac_drvinfo_cfg(struct hal_info_t *hal_info,
			bool enable, enum phl_band_idx hw_band);

enum rtw_hal_status
rtw_hal_mac_set_tx_duty(struct hal_info_t *hal_info,
			u16 pause_interval,
			u16 tx_interval);

enum rtw_hal_status
rtw_hal_mac_stop_tx_duty(struct hal_info_t *hal_info);
u16 rtw_hal_mac_query_ampdu_num(struct hal_info_t *hal, u8 band);

void rtw_hal_mac_init_txagg_num(struct hal_info_t *hal_info);

enum rtw_hal_status
rtw_hal_mac_pwr_switch(struct hal_info_t *hal_info, bool on);

enum rtw_hal_status
rtw_hal_mac_cfg_hw_cts2slef(struct hal_info_t *hal_info,
			 struct mac_ax_cts2self_cfg *cts_cfg);

enum rtw_hal_status
rtw_hal_mac_set_usr_edca(struct hal_info_t *hal,
                         u32 idx,
                         u32 enable,
                         u32 band,
                         u32 wmm,
                         struct rtw_edca_param *aggr,
                         struct rtw_edca_param *mod);

enum rtw_hal_status
rtw_hal_mac_get_edca(struct rtw_hal_com_t *hal_com,
                     u8 band,
                     u8 wmm,
                     struct rtw_edca_param *edca_param);
#endif /*_HAL_API_MAC_H_*/
