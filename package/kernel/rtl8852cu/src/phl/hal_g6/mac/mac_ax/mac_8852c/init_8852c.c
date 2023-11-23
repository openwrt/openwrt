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

#include "init_8852c.h"
#include "../pwr.h"
#include "../efuse.h"
#include "../init.h"
#include "../trxcfg.h"
#include "pwr_seq_8852c.h"
#include "../hw.h"
#include "security_cam_8852c.h"
#include "../trx_desc.h"
#include "../../feature_cfg.h"
#include "../fwcmd.h"
#include "../fwdl.h"
#include "../fwofld.h"
#include "../role.h"
#include "../tblupd.h"
#include "tblupd_8852c.h"
#include "../rx_forwarding.h"
#include "../rx_filter.h"
#include "../phy_rpt.h"
#include "../hwamsdu.h"
#include "../status.h"
#include "../hdr_conv.h"
#include "../hw_seq.h"
#include "gpio_8852c.h"
#include "../gpio.h"
#include "../cpuio.h"
#include "../sounding.h"
#include "../power_saving.h"
#include "../wowlan.h"
#include "../tcpip_checksum_offload.h"
#include "../la_mode.h"
#include "../dle.h"
#include "../coex.h"
#include "../mcc.h"
#include "../twt.h"
#include "../mport.h"
#include "../p2p.h"
#include "../flash.h"
#include "../dbg_cmd.h"
#include "../phy_misc.h"
#include "pwr_seq_func_8852c.h"
#include "../h2c_agg.h"
#include "mac_priv_8852c.h"
#include "trx_desc_8852c.h"
#include "../dbcc.h"
#include "../beacon.h"
#include "coex_8852c.h"
#include "phy_rpt_8852c.h"
#include "hwamsdu_8852c.h"
#include "hdr_conv_rx_8852c.h"
#include "dle_8852c.h"
#include "../secure_boot.h"
#include "../nan.h"

#if MAC_AX_SDIO_SUPPORT
#include "../_sdio.h"
#include "_sdio_8852c.h"
#endif
#if MAC_AX_USB_SUPPORT
#include "_usb_8852c.h"
#endif
#if MAC_AX_PCIE_SUPPORT
#include "../_pcie.h"
#include "_pcie_8852c.h"
#endif
#if MAC_AX_FEATURE_DBGPKG
#include "../dbgpkg.h"
#include "../dbgport_hw.h"
#endif
#if MAC_AX_8852C_SUPPORT

#if MAC_AX_SDIO_SUPPORT
static struct mac_ax_intf_ops mac8852c_sdio_ops = {
	reg_read8_sdio, /* reg_read8 */
	reg_write8_sdio, /* reg_write8 */
	reg_read16_sdio, /* reg_read16 */
	reg_write16_sdio, /* reg_write16 */
	reg_read32_sdio, /* reg_read32 */
	reg_write32_sdio, /* reg_write32 */
	tx_allow_sdio, /* tx_allow_sdio */
	tx_cmd_addr_sdio, /* tx_cmd_addr_sdio */
	sdio_pre_init_8852c, /* intf_pre_init */
	sdio_init, /* intf_init */
	sdio_deinit, /* intf_deinit */
	reg_read_n_sdio, /* reg_read_n_sdio */
	NULL, /*get_bulkout_id*/
	ltr_set_sdio, /* ltr_set_pcie */
	NULL, /*u2u3_switch*/
	NULL, /*get_usb_mode*/
	NULL, /*get_usb_support_ability*/
	NULL, /*usb_tx_agg_cfg*/
	NULL, /*usb_rx_agg_cfg*/
	set_sdio_wowlan, /*set_wowlan*/
	ctrl_txdma_ch_sdio, /*ctrl_txdma_ch*/
	clr_idx_all_sdio, /*clr_idx_all*/
	poll_txdma_ch_idle_sdio, /*poll_txdma_ch_idle*/
	poll_rxdma_ch_idle_sdio, /*poll_rxdma_ch_idle*/
	set_pcie_speed_sdio, /*set_pcie_speed*/
	get_pcie_speed_sdio, /*get_pcie_speed*/
	ctrl_txhci_sdio, /*ctrl_txhci*/
	ctrl_rxhci_sdio, /*ctrl_rxhci*/
	ctrl_dma_io_sdio, /*ctrl_dma_io*/
	get_io_stat_sdio, /* get_io_stat */
	sdio_get_txagg_num, /*get_txagg_num*/
	get_avail_txbd_sdio, /*get_avail_txbd*/
	get_avail_rxbd_sdio, /*get_avail_rxbd*/
	trigger_txdma_sdio, /*trigger_txdma*/
	notify_rxdone_sdio, /*notify_rxdone*/
	sdio_get_rx_state, /*get_usb_rx_state*/
	dbcc_hci_ctrl_sdio, /* dbcc_hci_ctrl */
	sdio_autok_counter_avg, /* pcie_autok_counter_avg */
	sdio_tp_adjust, /* tp_adjust */
};
#endif

#if MAC_AX_USB_SUPPORT
static struct mac_ax_intf_ops mac8852c_usb_ops = {
	reg_read8_usb_8852c, /* reg_read8 */
	reg_write8_usb_8852c, /* reg_write8 */
	reg_read16_usb_8852c, /* reg_read16 */
	reg_write16_usb_8852c, /* reg_write16 */
	reg_read32_usb_8852c, /* reg_read32 */
	reg_write32_usb_8852c, /* reg_write32 */
	NULL, /* tx_allow_sdio */
	NULL, /* tx_cmd_addr_sdio */
	usb_pre_init_8852c, /* intf_pre_init */
	usb_init_8852c, /* intf_init */
	usb_deinit_8852c, /* intf_deinit */
	NULL, /* reg_read_n_sdio */
	get_bulkout_id_8852c, /*get_bulkout_id*/
	ltr_set_usb, /* ltr_set_pcie */
	u2u3_switch_8852c, /*u2u3_switch*/
	get_usb_mode, /*get_usb_mode*/
	get_usb_support_ability_8852c,/*get_usb_support_ability*/
	usb_tx_agg_cfg_8852c, /*usb_tx_agg_cfg*/
	usb_rx_agg_cfg_8852c, /*usb_rx_agg_cfg*/
	set_usb_wowlan_8852c, /*set_wowlan*/
	ctrl_txdma_ch_usb, /*ctrl_txdma_ch*/
	clr_idx_all_usb, /*clr_idx_all*/
	poll_txdma_ch_idle_usb, /*poll_txdma_ch_idle*/
	poll_rxdma_ch_idle_usb, /*poll_rxdma_ch_idle*/
	set_pcie_speed_usb, /*set_pcie_speed*/
	get_pcie_speed_usb, /*get_pcie_speed*/
	ctrl_txhci_usb, /*ctrl_txhci*/
	ctrl_rxhci_usb, /*ctrl_rxhci*/
	ctrl_dma_io_usb, /*ctrl_dma_io*/
	get_io_stat_usb, /* get_io_stat */
	usb_get_txagg_num_8852c, /*get_txagg_num*/
	get_avail_txbd_usb, /*get_avail_txbd*/
	get_avail_rxbd_usb, /*get_avail_rxbd*/
	trigger_txdma_usb, /*trigger_txdma*/
	notify_rxdone_usb, /*notify_rxdone*/
	usb_get_rx_state_8852c, /*get_usb_rx_state*/
	dbcc_hci_ctrl_usb, /* dbcc_hci_ctrl */
	usb_autok_counter_avg, /* pcie_autok_counter_avg */
	usb_tp_adjust, /* tp_adjust */
};
#endif

#if MAC_AX_PCIE_SUPPORT
static struct mac_ax_intf_ops mac8852c_pcie_ops = {
	reg_read8_pcie, /* reg_read8 */
	reg_write8_pcie, /* reg_write8 */
	reg_read16_pcie, /* reg_read16 */
	reg_write16_pcie, /* reg_write16 */
	reg_read32_pcie, /* reg_read32 */
	reg_write32_pcie, /* reg_write32 */
	NULL, /* tx_allow_sdio */
	NULL, /* tx_cmd_addr_sdio */
	pcie_pre_init, /* intf_pre_init */
	pcie_init, /* intf_init */
	pcie_deinit, /* intf_deinit */
	NULL, /* reg_read_n_sdio */
	NULL, /*get_bulkout_id*/
	ltr_set_pcie, /* ltr_set_pcie */
	NULL, /*u2u3_switch*/
	NULL, /*get_usb_mode*/
	NULL,/*get_usb_support_ability*/
	NULL, /*usb_tx_agg_cfg*/
	NULL, /*usb_rx_agg_cfg*/
	set_pcie_wowlan, /*set_wowlan*/
	ctrl_txdma_ch_pcie_8852c, /*ctrl_txdma_ch*/
	clr_idx_all_pcie, /*clr_idx_all*/
	poll_txdma_ch_idle_pcie_8852c, /*poll_txdma_ch_idle*/
	poll_rxdma_ch_idle_pcie_8852c, /*poll_rxdma_ch_idle*/
	set_pcie_speed_8852c, /*set_pcie_speed*/
	get_pcie_speed_8852c, /*get_pcie_speed*/
	ctrl_txhci_pcie, /*ctrl_txhci*/
	ctrl_rxhci_pcie, /*ctrl_rxhci*/
	ctrl_dma_io_pcie, /*ctrl_dma_io*/
	get_io_stat_pcie_8852c, /* get_io_stat */
	pcie_get_txagg_num, /*get_txagg_num*/
	get_avail_txbd_8852c, /*get_avail_txbd*/
	get_avail_rxbd_8852c, /*get_avail_rxbd*/
	trigger_txdma_pcie, /*trigger_txdma*/
	notify_rxdone_pcie, /*notify_rxdone*/
	NULL, /*get_usb_rx_state*/
	dbcc_hci_ctrl_pcie, /* dbcc_hci_ctrl */
	pcie_autok_counter_avg, /* pcie_autok_counter_avg */
	pcie_tp_adjust, /* tp_adjust */
};
#endif

static struct mac_ax_ops mac8852c_ops = {
	NULL, /* intf_ops */
	/*System level*/
	mac_hal_init, /* hal_init */
	mac_hal_fast_init, /* hal_fast_init */
	mac_hal_deinit, /* hal_deinit */
	mac_hal_fast_deinit, /*hal_fast_deinit*/
	mac_add_role, /* add_role */
	mac_remove_role, /* remove_role */
	mac_change_role, /* change_role */
	mac_pwr_switch, /* pwr_switch */
	mac_sys_init, /* sys_init */
	mac_trx_init, /* init */
	mac_feat_init, /*feature init */
	mac_romdl, /* romdl */
	mac_enable_cpu, /* enable_cpu */
	mac_disable_cpu, /* disable_cpu */
	mac_fwredl, /* fwredl */
	mac_fwdl, /* fwdl */
	mac_query_fw_buff, /* query_fw_buff */
	mac_enable_fw, /* enable_fw */
	mac_get_dynamic_hdr_ax, /* get_dynamic_hdr */
	mac_lv1_rcvy, /* lv1_rcvy */
	mac_get_macaddr,
	mac_build_txdesc_8852c, /* build_txdesc */
	mac_refill_txdesc_8852c, /* refill_txdesc */
	mac_parse_rxdesc_8852c, /* parse_rxdesc */
	mac_watchdog, /* watchdog */
	/*FW offload related*/
	mac_reset_fwofld_state,
	mac_check_fwofld_done,
	mac_read_pkt_ofld,
	mac_del_pkt_ofld,
	mac_add_pkt_ofld,
	mac_pkt_ofld_packet,
	mac_dump_efuse_ofld,
	mac_efuse_ofld_map,
	mac_upd_dctl_info_8852c, /* update dmac ctrl info */
	mac_upd_cctl_info_8852c, /* update cmac ctrl info */
	mac_ie_cam_upd, /* ie_cam_upd */
	mac_twt_info_upd_h2c, /* twt info update h2c */
	mac_twt_act_h2c, /* twt act h2c */
	mac_twt_staanno_h2c, /* twt anno h2c */
	mac_twt_wait_anno,
	mac_host_getpkt_h2c,
	mac_p2p_act_h2c, /* p2p_act_h2c */
	mac_p2p_macid_ctrl_h2c, /* p2p_macid_ctrl_h2c */
	mac_get_p2p_stat, /* get_p2p_stat */
	mac_tsf32_togl_h2c, /* tsf32_togl_h2c */
	mac_get_t32_togl_rpt, /* get_t32_togl_rpt */
	mac_ccxrpt_parsing,
	mac_host_efuse_rec,
	mac_cfg_sensing_csi,
	mac_chk_sensing_csi_done,
	mac_calc_crc, /* calc_crc */
	mac_bcn_ofld_ctrl, /* bcn_ofld_ctrl */
	/*Association, de-association related*/
	mac_sta_add_key_8852c, /* add station key */
	mac_sta_del_key_8852c, /* del station key */
	mac_sta_search_key_idx, /* search station key index */
	mac_sta_hw_security_support, /* control hw security support */
	mac_set_mu_table, /*set mu score table*/
	mac_ss_dl_grp_upd_8852c, /* update SS dl group info*/
	mac_ss_ul_grp_upd, /* update SS ul group info*/
	mac_ss_ul_sta_upd, /* add sta into SS ul link*/
	mac_bacam_avl_std_entry_idx, /*search available std entry idx in BA CAM*/
	mac_bacam_info, /*update BA CAM info*/
	/*TRX related*/
	mac_txdesc_len_8852c, /* txdesc_len */
	mac_upd_shcut_mhdr,/*update short cut mac header*/
	mac_enable_hwamsdu, /* enable_hwmasdu */
	mac_hwamsdu_fwd_search_en,
	mac_hwamsdu_macid_en,
	mac_hwamsdu_get_macid_en,
	mac_hwamsdu_max_len,
	mac_hwamsdu_get_max_len,
	mac_enable_cut_hwamsdu_8852c, /* enable_cut_hwamsdu */
	mac_cut_hwamsdu_chk_mpdu_len_en_8852c, /* enable cut-amsdu chk mpdu size*/
	mac_hdr_conv_en, /* enable mac hdr conv */
	mac_hdr_conv_tx_set_eth_type, /* set eth type */
	mac_hdr_conv_tx_get_eth_type, /* get eth type */
	mac_hdr_conv_tx_set_oui, /* get oui */
	mac_hdr_conv_tx_get_oui, /* get oui */
	mac_hdr_conv_tx_macid_en, /* enable mac hdr conv for specifical macid */
	mac_hdr_conv_tx_vlan_tag_valid_en, /* enable vlantag valid */
	mac_hdr_conv_tx_get_vlan_tag_valid, /* get vlantag valid*/
	mac_hdr_conv_tx_qos_field_en, /* enable qos control field translation */
	mac_hdr_conv_tx_get_qos_field_en, /* get qos control field translation */
	mac_hdr_conv_tx_get_qos_field_h, /* setup qos control field bit 8-15*/
	mac_hdr_conv_tx_target_wlan_hdr_len, /* setup target header length */
	mac_hdr_conv_tx_get_target_wlan_hdr_len, /* get target header length */
	mac_hdr_conv_rx_en_8852c, /* enable rx mac hdr conversion*/
	mac_hdr_conv_rx_en_driv_info_hdr_8852c, /*en rx hdr conv driver info*/
	mac_set_hwseq_reg, /* set hw seq by reg */
	mac_set_hwseq_dctrl, /*for set hw seq content*/
	mac_get_hwseq_cfg, /*for get hw seq content*/
	mac_process_c2h, /* process_c2h */
	mac_parse_dfs, /* parse_dfs */
	mac_parse_ppdu, /* parse_ppdu */
	mac_cfg_phy_rpt, /* cfg_phy_rpt */
	mac_set_rx_forwarding, /* rx_forwarding */
	mac_get_rx_fltr_opt, /* set rx fltr mac, pclp header opt */
	mac_set_rx_fltr_opt, /* get rx fltr mac, pclp header opt */
	mac_set_typ_fltr_opt, /* set machdr type fltr opt */
	mac_set_typsbtyp_fltr_opt, /* set machdr typ subtyp fltr opt */
	mac_set_typsbtyp_fltr_detail, /* set detail type subtype filter config*/
	mac_get_cfg_addr_cam, /* get addrcam setting */
	mac_get_cfg_addr_cam_dis, /* get addrcam disable default setting */
	mac_cfg_addr_cam, /* config addrcam setting */
	mac_cfg_addr_cam_dis, /* config addrcam disable default setting */
	mac_sr_update, /* set sr parameter */
	mac_two_nav_cfg,  /* config 2NAV hw setting */
	mac_wde_pkt_drop, /* pkt_drop */
	mac_send_bcn_h2c, /* send beacon h2c */
	mac_tx_mode_sel, /*tx mode sel*/
	mac_tcpip_chksum_ofd, /* tcpip_chksum_ofd */
	mac_chk_rx_tcpip_chksum_ofd, /* chk_rx_tcpip_chksum_ofd */
	mac_chk_allq_empty, /*chk_allq_empty*/
	mac_is_txq_empty_8852c, /*is_txq_empty*/
	mac_is_rxq_empty_8852c, /*is_rxq_empty*/
	mac_parse_bcn_stats_c2h, /*parse tx bcn statistics*/
	mac_tx_idle_poll, /*tx_idle_poll*/
	mac_sifs_chk_cca_en, /* check cca in sifs enable/disable */
	mac_patch_rx_rate_8852c, /*for patch rx rate*/
	mac_get_wp_offset_8852c, /* wd offload for wp_offset, called while forming metadata */
	/*frame exchange related*/
	mac_upd_mudecision_para, /* upd_ba_infotbl */
	mac_mu_sta_upd, /* upd_mu_sta */
	mac_upd_ul_fixinfo, /* upd_ul_fixinfo */
	NULL,
	mac_f2p_test_cmd_8852c, /*f2p test cmd para*/
	mac_snd_test_cmd, /* f2p test cmd para */
	mac_set_fixmode_mib, /* set_fw_testmode */
	mac_dumpwlanc,
	mac_dumpwlans,
	mac_dumpwland,
	mac_ss_dl_rpt_cfg,
	/*outsrcing related */
	mac_outsrc_h2c_common, /* outsrc common h2c */
	mac_read_pwr_reg, /* for read tx power reg*/
	mac_write_pwr_reg, /* for write tx power reg*/
	mac_write_msk_pwr_reg, /* for write tx power reg*/
	mac_write_pwr_ofst_mode, /* for write tx power mode offset reg*/
	mac_write_pwr_ofst_bw, /* for write tx power BW offset reg*/
	mac_write_pwr_ref_reg, /* for write tx power ref reg*/
	mac_write_pwr_limit_en, /* for write tx power limit enable reg*/
	mac_write_pwr_limit_rua_reg, /* for write tx power limit rua reg*/
	mac_write_pwr_limit_reg, /* for write tx power limit reg*/
	mac_write_pwr_by_rate_reg, /* for write tx power by rate reg*/
	mac_lamode_cfg, /*cfg la mode para*/
	mac_lamode_trigger, /*trigger la mode start*/
	mac_lamode_buf_cfg, /*la mode buf size cfg */
	mac_get_lamode_st, /*get la mode status*/
	mac_read_xcap_reg_dav, /*read xcap xo/xi reg*/
	mac_write_xcap_reg_dav, /*write xcap xo/xi reg*/
	mac_write_bbrst_reg, /*write bb rst reg*/
	mac_tx_path_map_cfg, /*for BB control TX PATH*/
	/*sounding related*/
	mac_get_csi_buffer_index, /* get CSI buffer index */
	mac_set_csi_buffer_index, /* set CSI buffer index */
	mac_get_snd_sts_index, /* get MACID SND status */
	mac_set_snd_sts_index, /* set SND status MACID */
	mac_init_snd_mer,/* init SND MER */
	mac_init_snd_mee,/* init SND MEE */
	mac_csi_force_rate, /*CSI fix rate reg*/
	mac_csi_rrsc, /*CSI RRSC*/
	mac_set_snd_para, /*set sound parameter*/
	mac_set_csi_para_reg, /*set reg csi para*/
	mac_set_csi_para_cctl, /*set csi para in cmac ctrl info*/
	mac_hw_snd_pause_release, /*HW SND pause release*/
	mac_bypass_snd_sts, /*bypass SND status*/
	mac_deinit_mee, /*deinit mee*/
	mac_snd_sup, /*bf entry num and SU MU buffer num*/
	mac_gidpos, /*VHT MU GID position setting*/
	/*ps related*/
	mac_cfg_lps, /*config LPS*/
	mac_ps_pwr_state, /*set or check lps power state*/
	mac_chk_leave_lps, /*check already leave protocol ps*/
	mac_cfg_ips, /*config IPS*/
	mac_chk_leave_ips, /*check already leave IPS protocol*/
	mac_ps_notify_wake, /*send RPWM to wake up HW/FW*/
	mac_cfg_ps_advance_parm, /*config advance parameter for power saving*/
	mac_periodic_wake_cfg, /*config ips periodic wake*/
	mac_req_pwr_lvl_cfg, /*config request power level*/
	mac_lps_option_cfg, /*config request lps option*/
	/* Wowlan related*/
	mac_cfg_wow_wake, /*config wowlan wake*/
	mac_cfg_disconnect_det, /*config disconnect det*/
	mac_cfg_keep_alive, /*config keep alive*/
	mac_cfg_gtk_ofld, /*config gtk ofld*/
	mac_cfg_arp_ofld, /*config arp ofld*/
	mac_cfg_ndp_ofld, /*config ndp ofld*/
	mac_cfg_realwow, /*config realwow*/
	mac_cfg_nlo, /*config nlo*/
	mac_cfg_dev2hst_gpio, /*config dev2hst gpio*/
	mac_cfg_uphy_ctrl, /*config uphy ctrl*/
	mac_cfg_wowcam_upd, /*config wowcam update*/
	mac_get_wow_wake_rsn, /* Get wowlan wakeup reason with reset option */
	mac_cfg_wow_sleep, /*config wowlan before sleep/after wake*/
	mac_get_wow_fw_status, /*get wowlan fw status*/
	mac_request_aoac_report, /* request_aoac_report */
	mac_read_aoac_report, /* read_aoac_report */
	mac_check_aoac_report_done, /* check_aoac_report_done */
	mac_wow_stop_trx, /* wow_stop_trx */
	mac_cfg_wow_auto_test, /* cfg_wow_auto_test */
	/*system related*/
	mac_dbcc_enable, /*enable / disable dbcc */
	mac_dbcc_pre_cfg, /* dbcc_pre_cfg */
	mac_dbcc_cfg, /* dbcc_cfg */
	mac_dbcc_trx_ctrl, /* dbcc_trx_ctrl */
	mac_port_cfg, /* cofig port para */
	mac_port_init, /* init port para */
	mac_enable_imr, /* enable CMAC/DMAC IMR */
	mac_dump_efuse_map_wl_plus, /* dump_wl_efuse*/
	mac_dump_efuse_map_bt, /* dump_bt_efuse */
	mac_write_efuse_plus, /* write_wl_bt_efuse */
	mac_read_efuse_plus, /* read_wl_bt_efuse */
	mac_read_hidden_efuse, /* read_hidden_efuse */
	mac_get_efuse_avl_size, /* get_available_efuse_size */
	mac_get_efuse_avl_size_bt, /* get_available_efuse_size_bt */
	mac_dump_log_efuse_plus, /* dump_logical_efuse */
	mac_read_log_efuse_plus, /* read_logical_efuse */
	mac_write_log_efuse_plus, /* write_logical_efuse */
	mac_dump_log_efuse_bt, /* dump_logical_efuse_bt */
	mac_read_log_efuse_bt, /* read_logical_efuse_bt */
	mac_write_log_efuse_bt, /* write_logical_efuse_bt */
	mac_pg_efuse_by_map_plus, /* program_efuse_map */
	mac_pg_efuse_by_map_bt, /* program_efuse_map_bt */
	mac_mask_log_efuse, /* mask_logical_efuse_map */
	mac_pg_sec_data_by_map, /* program_secure_data_map */
	mac_cmp_sec_data_by_map, /* compare_secure_data_map */
	mac_get_efuse_info, /* get_efuse_info */
	mac_set_efuse_info, /* set_efuse_info */
	mac_read_hidden_rpt, /* read_efuse_hidden_report */
	mac_check_efuse_autoload, /* check_efuse_autoload */
	mac_pg_simulator_plus, /* efuse pg simulator */
	mac_checksum_update, /* checksum update */
	mac_checksum_rpt, /*report checksum comparison result*/
	mac_disable_rf_ofld_by_info, /* Disable RF Offload */
	mac_set_efuse_ctrl, /*set efuse ctrl 0x30 or 0xC30*/
	mac_otp_test, /*efuse OTP test R/W to 0x7ff*/
	mac_get_ft_status, /* get_mac_ft_status */
	mac_fw_log_cfg, /* fw_log_cfg */
	mac_pinmux_set_func_8852c, /* pinmux_set_func */
	mac_pinmux_free_func, /* pinmux_free_func */
	mac_sel_uart_tx_pin, /* sel_uart_tx_pin */
	mac_sel_uart_rx_pin, /* sel_uart_rx_pin */
	mac_gpio_init_8852c, /* init_gpio */
	mac_set_gpio_func_8852c, /* set_gpio_func */
	mac_get_gpio_val, /* get_gpio_val */
	mac_get_uart_fw_dbg_gpio, /* get_uart_fw_dbg_gpio */
	mac_get_hw_info, /* get_hw_info */
	mac_set_hw_value, /* set_hw_value */
	mac_get_hw_value, /* get_hw_value */
	mac_get_err_status, /* get_err_status */
	mac_set_err_status, /* set_err_status */
	mac_general_pkt_ids, /*general_pkt_ids */
	mac_coex_init_8852c, /* coex_init */
	mac_read_coex_reg_8852c, /* coex_read */
	mac_write_coex_reg_8852c, /* coex_write */
	mac_trigger_cmac_err, /*trigger_cmac_err*/
	mac_trigger_cmac1_err, /*trigger_cmac1_err*/
	mac_trigger_dmac_err, /*trigger_dmac_err*/
	mac_tsf_sync, /*tsf_sync*/
	mac_read_xtal_si, /*read_xtal_si*/
	mac_write_xtal_si, /*write_xtal_si*/
	mac_io_chk_access, /* io_chk_access */
	mac_ser_ctrl, /* ser_ctrl */
	mac_chk_err_status, /* chk_ser_status */
	mac_get_freerun, /* mac_get_freerun */
	/* mcc */
	mac_reset_mcc_group,
	mac_reset_mcc_request,
	mac_add_mcc, /* add_mcc */
	mac_start_mcc, /* start_mcc */
	mac_stop_mcc, /* stop_mcc */
	mac_del_mcc_group, /* del_mcc_group */
	mac_mcc_request_tsf, /* mcc_request_tsf */
	mac_mcc_macid_bitmap, /* mcc_macid_bitmap */
	mac_mcc_sync_enable, /* mcc_sync_enable */
	mac_mcc_set_duration, /* mcc_set_duration */
	mac_get_mcc_tsf_rpt,
	mac_get_mcc_status_rpt,
	mac_get_mcc_group,
	mac_check_add_mcc_done,
	mac_check_start_mcc_done,
	mac_check_stop_mcc_done,
	mac_check_del_mcc_group_done,
	mac_check_mcc_request_tsf_done,
	mac_check_mcc_macid_bitmap_done,
	mac_check_mcc_sync_enable_done,
	mac_check_mcc_set_duration_done,
	/* not mcc */
	mac_check_access,
	mac_set_led_mode, /* set_led_mode */
	mac_led_ctrl, /* led_ctrl */
	mac_set_sw_gpio_mode, /* set_sw_gpio_mode */
	mac_sw_gpio_ctrl, /* sw_gpio_ctrl */
	mac_get_c2h_event, /* get_c2h_event */
	mac_cfg_wps, /* cfg_wps */
	mac_get_wl_dis_val, /* get_wl_dis_val */
	mac_cfg_per_pkt_phy_rpt_8852c, /* cfg_per_pkt_phy_rpt */
#if MAC_AX_FEATURE_DBGPKG
	mac_fwcmd_lb, /* fwcmd_lb */
	mac_mem_dump, /* sram mem dump */
	mac_get_mem_size, /* get mem size */
	mac_dbg_status_dump, /* mac dbg status dump */
	mac_reg_dump, /* debug reg dump for MAC/BB/RF*/
	mac_rx_cnt,
	mac_dump_fw_rsvd_ple,
	mac_fw_dbg_dump,
	mac_event_notify,
	mac_dbgport_hw_set, /* Set debug port for LA */
#endif
#if MAC_AX_FEATURE_HV
	mac_ram_boot, /* ram_boot */
	/*fw offload related*/
	mac_clear_write_request, /* clear_write_request */
	mac_add_write_request, /* add_write_request */
	mac_write_ofld, /* write_ofld */
	mac_clear_conf_request, /* clear_conf_request */
	mac_add_conf_request, /* add_conf_request */
	mac_conf_ofld, /* conf_ofld */
	mac_clear_read_request, /* clear_read_request */
	mac_add_read_request, /* add_read_request */
	mac_read_ofld, /* read_ofld */
	mac_read_ofld_value, /* read_ofld_value */
#endif
	mac_add_cmd_ofld, /* add_cmd_ofld */
	mac_add_cmd_ofld_v1, /* add_cmd_ofld_v1 */
	mac_cmd_ofld, /* cmd_ofld */
	mac_flash_erase,
	mac_flash_read,
	mac_flash_write,
	mac_fw_status_cmd, /* fw_status_cmd */
	mac_tx_duty, /* tx_duty */
	mac_tx_duty_stop, /* tx_duty _stop */
	mac_fwc2h_ofdma_sts_parse, /* parse c2h fw sts */
	mac_fw_ofdma_sts_en, /* send fw sts en to fw */
	mac_get_phy_rpt_cfg, /* get_phy_rpt_cfg */
#if MAC_AX_FEATURE_DBGCMD
	mac_halmac_cmd, /* halmac_cmd */
	mac_halmac_cmd_parser, /* halmac_cmd_parser */
#endif
	mac_fast_ch_sw,
	mac_fast_ch_sw_done,
	mac_get_fast_ch_sw_rpt,
	mac_h2c_agg_enable,
	mac_h2c_agg_flush,
	mac_h2c_agg_tx,
	mac_fw_dbg_dle_cfg,
	mac_add_scanofld_ch,
	mac_scanofld,
	mac_scanofld_fw_busy,
	mac_scanofld_chlist_busy,
	mac_scanofld_hst_ctrl,
#if MAC_AX_FEATURE_DBGDEC
	mac_fw_log_set_array,
	mac_fw_log_unset_array,
#endif
	mac_get_fw_status,
	mac_role_sync,
	mac_ch_switch_ofld,
	mac_get_ch_switch_rpt,
	mac_cfg_bcn_filter,
	mac_bcn_filter_rssi,
	mac_bcn_filter_tp,
	mac_cfg_bcn_early_rpt,
	/*Proxy related*/
	mac_proxyofld,
	mac_proxy_mdns_serv_pktofld,
	mac_proxy_mdns_txt_pktofld,
	mac_proxy_mdns,
	mac_proxy_ptcl_pattern,
	mac_check_proxy_done,
	/*fw cap related*/
	mac_get_wlanfw_cap,
	/* NAN related */
	mac_nan_act_schedule_req,
	mac_nan_bcn_req,
	mac_nan_func_ctrl,
	mac_nan_pause_faw_tx,
	mac_nan_de_info,
	mac_nan_join_cluster,
	mac_get_act_schedule_id,
	mac_nan_get_cluster_info,
	mac_check_cluster_info,
	/* MP security related */
	mac_chk_sec_rec, /* mp_chk_sec_rec*/
	mac_pg_sec_phy_wifi, /* mp_pg_sec_phy_wifi */
	mac_cmp_sec_phy_wifi, /* mp_cmp_sec_phy_wifi */
	mac_pg_sec_hid_wifi, /* mp_pg_sec_hid_wifi */
	mac_cmp_sec_hid_wifi, /* mp_cmp_sec_hid_wifi */
	mac_pg_sec_dis, /* mp_pg_sec_dis */
	mac_cmp_sec_dis, /* mp_cmp_sec_dis */
	mac_sic_dis, /* mp_sic_dis */
	mac_chk_sic_dis, /* mp_chk_sic_dis */
	mac_jtag_dis, /* mp_jtag_dis */
	mac_chk_jtag_dis, /* mp_chk_jtag_dis */
	mac_uart_tx_dis, /* mp_uart_tx_dis */
	mac_chk_uart_tx_dis, /* mp_chk_uart_tx_dis */
	mac_uart_rx_dis, /* mp_chk_uart_rx_dis */
	mac_chk_uart_rx_dis, /* mp_chk_uart_rx_dis */
};

static struct mac_ax_hw_info mac8852c_hw_info = {
	0, /* done */
	MAC_AX_CHIP_ID_8852C, /* chip_id */
	0xFF, /* cv */
	0, /* acv */
	MAC_AX_INTF_INVALID, /* intf */
	19, /* tx_ch_num */
	10, /* tx_data_ch_num */
	WD_BODY_LEN_V1, /* wd_body_len */
	WD_INFO_LEN, /* wd_info_len */
	pwr_on_seq_8852c, /* pwr_on_seq */
	pwr_off_seq_8852c, /* pwr_off_seq */
	PWR_SEQ_VER_8852C, /* pwr_seq_ver */
	458752, /* fifo_size */
	MAC_STA_NUM, /* macid_num */
	5, /* port_num */
	16, /* mbssid_num */
	20, /* bssid_num */
	1536, /* wl_efuse_size */
	1216, /* wl_zone2_efuse_size */
	2048, /* log_efuse_size */
	1280, /* limit_efuse_size_PCIE */
	1280, /* limit_efuse_size_USB */
	1280, /* limit_efuse_size_SDIO */
	512, /* bt_efuse_size */
	1024, /* bt_log_efuse_size */
	96, /* hidden_efuse_rf_size */
	32, /* hidden_efuse_mac_size */
	4, /* sec_ctrl_efuse_size */
	192, /* sec_data_efuse_size */
	NULL, /* sec_cam_table_t pointer */
	NULL, /* dctl_sec_info_t pointer */
	32, /* ple_rsvd_space */
	24, /* payload_desc_size */
	8, /* efuse_version_size */
	128, /* wl_efuse_size_DAV  */
	96, /* wl_zone2_efuse_size_DAV  */
	32, /* hidden_efuse_size_DAV  */
	16, /* log_efuse_size_DAV  */
	0, /* wl_efuse_start_addr */
	0, /* wl_efuse_start_addr_DAV  */
	0x600, /* bt_efuse_start_addr */
	0, /* wd_checksum_en */
	0, /* sw_amsdu_max_size */
	0, /* ind_aces_cnt */
	0, /* dbg_port_cnt */
	0, /* core_swr_volt */
	0, /* is_sec_ic */
	0xFF, /* sta_empty_flg */
	{{{0}, {0}, 0}}, /* cust_proc_id */
	MAC_AX_SWR_NORM, /* core_swr_volt_sel */
	MAC_AX_DRV_INFO_NONE, /* cmac0_drv_info */
	MAC_AX_DRV_INFO_NONE, /* cmac1_drv_info */
};

struct mac_ax_ft_status mac_8852c_ft_status[] = {
	{MAC_AX_FT_DUMP_EFUSE, MAC_AX_STATUS_IDLE, NULL, 0},
	{MAC_AX_FT_MAX, MAC_AX_STATUS_ERR, NULL, 0},
};

static struct mac_ax_adapter mac_8852c_adapter = {
	&mac8852c_ops, /* ops */
	NULL, /* drv_adapter */
	NULL, /* phl_adapter */
	NULL, /* pltfm_cb */
	MAC_AX_DFLT_SM, /* sm */
	NULL, /* hw_info */
	{0}, /* fw_info */
	{{0}}, /* ser_info */
	{0}, /* efuse_param */
	{0}, /* mac_pwr_info */
	mac_8852c_ft_status, /* ft_stat */
	NULL, /* hfc_param */
	{MAC_AX_QTA_SCC, 64, 128, 0, 0, 0, 0, 0,
	 0, 0, 0, 0}, /* dle_info */
	{0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0,
	 0xFF, 0xFF, DFLT_GPIO_STATE, DFLT_SW_IO_MODE}, /* gpio_info */
	NULL, /* role table */
	{NULL, NULL, NULL, 0, 0, 0, 0}, /* read_ofld_info */
	{0, 0, NULL}, /* read_ofld_value */
	{NULL, NULL, NULL, 0, 0, 0, 0}, /* write_ofld_info */
	{NULL}, /* efuse_ofld_info */
	{NULL, NULL, 0, 0, 0, 0}, /* conf_ofld_info */
	{PKT_OFLD_OP_MAX, PKT_OFLD_MAX_COUNT - 1, 0, {0}}, /* pkt_ofld_info */
	{0, 0, 0, NULL}, /* pkt_ofld_pkt */
	{NULL, NULL, NULL, 0, 0, 0, 0, 0, 0}, /* cmd_ofld_info */
	{{{0}, {0}, {0}, {0}}}, /* mcc_group_info */
	{NULL}, /* wowlan_info */
	NULL, /* p2p_info */
	NULL, /* t32_togl_rpt */
	NULL, /* port_info */
	{0}, /* struct mac_ax_int_stats stats */
	{0}, /*h2c_agg_info*/
	{0, 0}, /* struct mac_ax_drv_stats drv_stats */
	{0}, /* csi_info */
	{{0}, 0, 0, 0, 0, 0, 0, 0, {0}}, /* nan_info */
#if MAC_AX_SDIO_SUPPORT
	{MAC_AX_SDIO_4BYTE_MODE_DISABLE, MAC_AX_SDIO_TX_MODE_AGG,
	MAC_AX_SDIO_SPEC_VER_2_00, MAC_AX_SDIO_OPN_MODE_BLOCK,
	512, 1, 8, 0}, /* sdio_info */
#endif
#if MAC_AX_USB_SUPPORT
	{0}, /* usb_info */
#endif
#if MAC_AX_PCIE_SUPPORT
	{0}, /* pcie_info */
#endif
	{0, 0, 0, 0, 0, 0, 0, 0, 0, NULL}, /*flash_info */
	{0, 0}, /* fast_ch_sw_info */
#if MAC_AX_FEATURE_HV
	NULL, /*hv_ax_ops*/
	HV_AX_ASIC, /* env */
#endif
#if MAC_AX_FEATURE_DBGCMD
	{NULL}, /*fw_dbgcmd*/
#endif
#if MAC_AX_FEATURE_DBGDEC
	NULL, /*fw_log_array*/
	NULL,
	0,
#endif
	{{NULL, NULL}}, /*scan ofld info*/
	{0}, /*log_cfg*/
	NULL, /* twt_info */
	{0}, /*ch_switch_rpt*/
	NULL, /* dbcc_info */
	{0}, /* bn_fltr_rpt */
};

#ifdef CONFIG_NEW_HALMAC_INTERFACE
struct mac_ax_adapter *get_mac_8852c_adapter(enum mac_ax_intf intf,
					     u8 chip_cut, void *phl_adapter,
					     void *drv_adapter,
					     struct mac_ax_pltfm_cb *pltfm_cb)
{
	struct mac_ax_adapter *adapter = NULL;
	struct mac_ax_mac_pwr_info *pwr_info;

	adapter =
	(struct mac_ax_adapter *)hal_mem_alloc(drv_adapter,
		sizeof(struct mac_ax_adapter));
	if (!adapter)
		return NULL;

	hal_mem_cpy(drv_adapter, adapter, &mac_8852c_adapter,
		    sizeof(struct mac_ax_adapter));
	pwr_info = &adapter->mac_pwr_info;

	adapter->phl_adapter = phl_adapter;
	adapter->drv_adapter = drv_adapter;
	adapter->pltfm_cb = pltfm_cb;
	adapter->hw_info->chip_cut = chip_cut;
	adapter->hw_info->intf = intf;
	adapter->hw_info->done = 1;

	switch (intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		adapter->ops->intf_ops = &mac8852c_sdio_ops;
		pwr_info->intf_pwr_switch = sdio_pwr_switch;
		break;
#endif
#if MAC_AX_USB_SUPPORT
	case MAC_AX_INTF_USB:
		adapter->ops->intf_ops = &mac8852c_usb_ops;
		pwr_info->intf_pwr_switch = usb_pwr_switch;
		break;
#endif
#if MAC_AX_PCIE_SUPPORT
	case MAC_AX_INTF_PCIE:
		adapter->ops->intf_ops = &mac8852c_pcie_ops;
		pwr_info->intf_pwr_switch = pcie_pwr_switch;
		break;
#endif
	default:
		return NULL;
	}

	return adapter;
}
#else
struct mac_ax_adapter *get_mac_8852c_adapter(enum mac_ax_intf intf,
					     u8 cv, void *drv_adapter,
					     struct mac_ax_pltfm_cb *pltfm_cb)
{
	struct mac_ax_adapter *adapter = NULL;
	struct mac_ax_hw_info *hw_info = NULL;
	struct mac_ax_priv_ops **p;
	struct mac_ax_mac_pwr_info *pwr_info;
	u32 priv_size;

	if (!pltfm_cb)
		return NULL;

	priv_size = get_mac_ax_priv_size();
	adapter = (struct mac_ax_adapter *)pltfm_cb->rtl_malloc(drv_adapter,
		sizeof(struct mac_ax_adapter) + priv_size);
	if (!adapter) {
		pltfm_cb->msg_print(drv_adapter, _PHL_ERR_, "Malloc adapter fail\n");
		return NULL;
	}

	pltfm_cb->rtl_memcpy(drv_adapter, adapter, &mac_8852c_adapter,
		     sizeof(struct mac_ax_adapter));

	/*Alloc HW INFO */
	hw_info = (struct mac_ax_hw_info *)pltfm_cb->rtl_malloc(drv_adapter,
		sizeof(struct mac_ax_hw_info));

	if (!hw_info) {
		pltfm_cb->msg_print(drv_adapter, _PHL_ERR_, "Malloc hw info fail\n");
		return NULL;
	}

	pltfm_cb->rtl_memcpy(drv_adapter, hw_info, &mac8852c_hw_info,
		sizeof(struct mac_ax_hw_info));

	pwr_info = &adapter->mac_pwr_info;

	adapter->drv_adapter = drv_adapter;
	adapter->pltfm_cb = pltfm_cb;
	adapter->hw_info = hw_info;
	adapter->hw_info->cv = cv;
	adapter->hw_info->intf = intf;
	adapter->hw_info->done = 1;

	p = get_priv(adapter);
	*p = get_mac_8852c_priv_ops(intf);

	switch (intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		adapter->ops->intf_ops = &mac8852c_sdio_ops;
		break;
#endif
#if MAC_AX_USB_SUPPORT
	case MAC_AX_INTF_USB:
		adapter->ops->intf_ops = &mac8852c_usb_ops;
		break;
#endif
#if MAC_AX_PCIE_SUPPORT
	case MAC_AX_INTF_PCIE:
		adapter->ops->intf_ops = &mac8852c_pcie_ops;
		break;
#endif
	default:
		return NULL;
	}

	return adapter;
}
#endif

u32 dmac_func_en_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;
	u32 ret = 0;

	val32 = (B_AX_MAC_FUNC_EN | B_AX_DMAC_FUNC_EN | B_AX_MAC_SEC_EN |
		 B_AX_DISPATCHER_EN | B_AX_DLE_CPUIO_EN | B_AX_PKT_IN_EN |
		 B_AX_DMAC_TBL_EN | B_AX_PKT_BUF_EN | B_AX_STA_SCH_EN |
		 B_AX_TXPKT_CTRL_EN | B_AX_WD_RLS_EN | B_AX_MPDU_PROC_EN |
		 B_AX_DMAC_CRPRT | B_AX_H_AXIDMA_EN);
	MAC_REG_W32(R_AX_DMAC_FUNC_EN, val32);

	val32 = (B_AX_MAC_SEC_CLK_EN | B_AX_DISPATCHER_CLK_EN |
		 B_AX_DLE_CPUIO_CLK_EN | B_AX_PKT_IN_CLK_EN |
		 B_AX_STA_SCH_CLK_EN | B_AX_TXPKT_CTRL_CLK_EN |
		 B_AX_WD_RLS_CLK_EN | B_AX_BBRPT_CLK_EN);
	MAC_REG_W32(R_AX_DMAC_CLK_EN, val32);

	adapter->sm.dmac_func = MAC_AX_FUNC_ON;

	return ret;
}

u32 dmac_func_pre_en_8852c(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = (B_AX_MAC_FUNC_EN | B_AX_DMAC_FUNC_EN |
		 B_AX_DISPATCHER_EN | B_AX_PKT_BUF_EN | B_AX_H_AXIDMA_EN);
	MAC_REG_W32(R_AX_DMAC_FUNC_EN, val32);

	val32 = (B_AX_DISPATCHER_CLK_EN);
	MAC_REG_W32(R_AX_DMAC_CLK_EN, val32);

	adapter->sm.dmac_func = MAC_AX_FUNC_ON;

	val32 = MAC_REG_R32(R_AX_HAXI_INIT_CFG1);
	switch (adapter->hw_info->intf) {
	case MAC_AX_INTF_USB:
		val32 = SET_CLR_WORD(val32, DMA_MOD_USB,
				     B_AX_DMA_MODE);
		break;
	case MAC_AX_INTF_PCIE:
		val32 = SET_CLR_WORD(val32, DMA_MOD_PCIE_1B,
				     B_AX_DMA_MODE);
		break;
	case MAC_AX_INTF_SDIO:
		val32 = SET_CLR_WORD(val32, DMA_MOD_SDIO,
				     B_AX_DMA_MODE);
		break;
	default:
		PLTFM_MSG_ERR("[ERR]DMAC init with not support intf: %X\n",
			      adapter->hw_info->intf);
		return MACINTF;
	}
	val32 = (val32 & ~B_AX_STOP_AXI_MST) | B_AX_TXHCI_EN_V1 |
		 B_AX_RXHCI_EN_V1;
	MAC_REG_W32(R_AX_HAXI_INIT_CFG1, val32);

	val32 = MAC_REG_R32(R_AX_HAXI_DMA_STOP1) &
		~(B_AX_STOP_ACH0 | B_AX_STOP_ACH1 | B_AX_STOP_ACH2 |
		  B_AX_STOP_ACH3 | B_AX_STOP_ACH4 | B_AX_STOP_ACH5 |
		  B_AX_STOP_ACH6 | B_AX_STOP_ACH7 | B_AX_STOP_CH8  |
		  B_AX_STOP_CH9 | B_AX_STOP_CH12);
	MAC_REG_W32(R_AX_HAXI_DMA_STOP1, val32);

	val32 = MAC_REG_R32(R_AX_HAXI_DMA_STOP2) &
		~(B_AX_STOP_CH10 | B_AX_STOP_CH11);
	MAC_REG_W32(R_AX_HAXI_DMA_STOP2, val32);

	val32 = MAC_REG_R32(R_AX_PLATFORM_ENABLE) | B_AX_AXIDMA_EN;
	MAC_REG_W32(R_AX_PLATFORM_ENABLE, val32);

	return MACSUCCESS;
}
#endif /* #if MAC_AX_8852C_SUPPORT */
