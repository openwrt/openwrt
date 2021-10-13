/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup
 * @{
 */
#ifndef _HPPE_XGPORTCTRL_H_
#define _HPPE_XGPORTCTRL_H_

#define MAC_TX_CONFIGURATION_MAX_ENTRY	2
#define MAC_RX_CONFIGURATION_MAX_ENTRY	2
#define MAC_PACKET_FILTER_MAX_ENTRY	2
#define MAC_WATCHDOG_TIMEOUT_MAX_ENTRY	2
#define MAC_VLAN_TAG_MAX_ENTRY	2
#define MAC_RX_ETH_TYPE_MATCH_MAX_ENTRY	2
#define MAC_Q0_TX_FLOW_CTRL_MAX_ENTRY	2
#define MAC_RX_FLOW_CTRL_MAX_ENTRY	2
#define MAC_INTERRUPT_STATUS_MAX_ENTRY	2
#define MAC_INTERRUPT_ENABLE_MAX_ENTRY	2
#define MAC_RX_TX_STATUS_MAX_ENTRY	2
#define MAC_LPI_CONTROL_STATUS_MAX_ENTRY	2
#define MAC_LPI_TIMERS_CONTROL_MAX_ENTRY	2
#define MAC_LPI_AUTO_ENTRY_TIMER_MAX_ENTRY	2
#define MAC_1US_TIC_COUNTER_MAX_ENTRY	2
#define MAC_ADDRESS0_HIGH_MAX_ENTRY	2
#define MAC_ADDRESS0_LOW_MAX_ENTRY	2
#define MMC_RECEIVE_INTERRUPT_MAX_ENTRY	2
#define MMC_TRANSMIT_INTERRUPT_MAX_ENTRY	2
#define MMC_RECEIVE_INTERRUPT_ENABLE_MAX_ENTRY	2
#define MMC_TRANSMIT_INTERRUPT_ENABLE_MAX_ENTRY	2

sw_error_t
hppe_mac_tx_configuration_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_tx_configuration_u *value);

sw_error_t
hppe_mac_tx_configuration_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_tx_configuration_u *value);

sw_error_t
hppe_mac_rx_configuration_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_configuration_u *value);

sw_error_t
hppe_mac_rx_configuration_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_configuration_u *value);

sw_error_t
hppe_mac_packet_filter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_packet_filter_u *value);

sw_error_t
hppe_mac_packet_filter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_packet_filter_u *value);

sw_error_t
hppe_mac_watchdog_timeout_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_watchdog_timeout_u *value);

sw_error_t
hppe_mac_watchdog_timeout_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_watchdog_timeout_u *value);

sw_error_t
hppe_mac_vlan_tag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_vlan_tag_u *value);

sw_error_t
hppe_mac_vlan_tag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_vlan_tag_u *value);

sw_error_t
hppe_mac_rx_eth_type_match_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_eth_type_match_u *value);

sw_error_t
hppe_mac_rx_eth_type_match_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_eth_type_match_u *value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_q0_tx_flow_ctrl_u *value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_q0_tx_flow_ctrl_u *value);

sw_error_t
hppe_mac_rx_flow_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_flow_ctrl_u *value);

sw_error_t
hppe_mac_rx_flow_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_flow_ctrl_u *value);

sw_error_t
hppe_mac_interrupt_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_interrupt_status_u *value);

sw_error_t
hppe_mac_interrupt_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_interrupt_status_u *value);

sw_error_t
hppe_mac_interrupt_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_interrupt_enable_u *value);

sw_error_t
hppe_mac_interrupt_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_interrupt_enable_u *value);

sw_error_t
hppe_mac_rx_tx_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_tx_status_u *value);

sw_error_t
hppe_mac_rx_tx_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_tx_status_u *value);

sw_error_t
hppe_mac_lpi_control_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_control_status_u *value);

sw_error_t
hppe_mac_lpi_control_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_control_status_u *value);

sw_error_t
hppe_mac_lpi_timers_control_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_timers_control_u *value);

sw_error_t
hppe_mac_lpi_timers_control_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_timers_control_u *value);

sw_error_t
hppe_mac_lpi_auto_entry_timer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_auto_entry_timer_u *value);

sw_error_t
hppe_mac_lpi_auto_entry_timer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_auto_entry_timer_u *value);

sw_error_t
hppe_mac_1us_tic_counter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_1us_tic_counter_u *value);

sw_error_t
hppe_mac_1us_tic_counter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_1us_tic_counter_u *value);

sw_error_t
hppe_mac_address0_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_address0_high_u *value);

sw_error_t
hppe_mac_address0_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_address0_high_u *value);

sw_error_t
hppe_mac_address0_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_address0_low_u *value);

sw_error_t
hppe_mac_address0_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_address0_low_u *value);

sw_error_t
hppe_mmc_receive_interrupt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_receive_interrupt_u *value);

sw_error_t
hppe_mmc_receive_interrupt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_receive_interrupt_u *value);

sw_error_t
hppe_mmc_transmit_interrupt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_transmit_interrupt_u *value);

sw_error_t
hppe_mmc_transmit_interrupt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_transmit_interrupt_u *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_receive_interrupt_enable_u *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_receive_interrupt_enable_u *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_transmit_interrupt_enable_u *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_transmit_interrupt_enable_u *value);

sw_error_t
hppe_mac_tx_configuration_vne_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_vne_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_ddic_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_ddic_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_te_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_te_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_ipg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_ipg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_ism_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_ism_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_ifp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_ifp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_sarc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_sarc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_isr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_isr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_ss_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_ss_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_g9991en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_g9991en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_uss_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_uss_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_vnm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_vnm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_tx_configuration_jd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_tx_configuration_jd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_lm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_lm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_je_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_je_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_arpen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_arpen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_elen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_elen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_gmpslce_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_gmpslce_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_hdsms_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_hdsms_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_spen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_spen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_usp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_usp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_ipc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_ipc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_gpsl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_gpsl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_re_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_re_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_cst_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_cst_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_dcrcc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_dcrcc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_wd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_wd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_acs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_acs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_configuration_s2kp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_configuration_s2kp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_pcf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_pcf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_hmc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_hmc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_dntu_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_dntu_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_saf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_saf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_dbf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_dbf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_huc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_huc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_vtfe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_vtfe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_daif_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_daif_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_ra_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_ra_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_hpf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_hpf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_pm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_pm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_vucc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_vucc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_pr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_pr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_ipfe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_ipfe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_packet_filter_saif_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_packet_filter_saif_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_watchdog_timeout_pwe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_watchdog_timeout_pwe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_watchdog_timeout_wto_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_watchdog_timeout_wto_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_eivls_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_eivls_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_vthm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_vthm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_vl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_vl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_dovltc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_dovltc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_etv_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_etv_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_erivlt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_erivlt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_eivlrxs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_eivlrxs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_vtim_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_vtim_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_edvlp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_edvlp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_evlrxs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_evlrxs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_evls_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_evls_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_esvl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_esvl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_vlan_tag_ersvlm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_vlan_tag_ersvlm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_eth_type_match_et_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_eth_type_match_et_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_pt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_pt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_plt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_plt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_tfe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_tfe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_fcb_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_fcb_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_dapq_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_q0_tx_flow_ctrl_dapq_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_flow_ctrl_pfce_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_flow_ctrl_pfce_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_flow_ctrl_up_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_flow_ctrl_up_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_flow_ctrl_rfe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_flow_ctrl_rfe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_txesis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_txesis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_gpiis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_gpiis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_tsis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_tsis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_mmctxis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_mmctxis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_ls_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_ls_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_mmcrxis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_mmcrxis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_smi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_smi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_pmtis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_pmtis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_rxesis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_rxesis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_status_lpiis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_status_lpiis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_enable_tsie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_enable_tsie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_enable_lpiie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_enable_lpiie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_enable_txesie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_enable_txesie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_enable_pmtie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_enable_pmtie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_interrupt_enable_rxesie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_interrupt_enable_rxesie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_tx_status_tjt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_tx_status_tjt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_rx_tx_status_rwt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_rx_tx_status_rwt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_tlpien_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_tlpien_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_lpitcse_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_lpitcse_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_rxrstp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_rxrstp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_lpite_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_lpite_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_pls_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_pls_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_rlpiex_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_rlpiex_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_rlpien_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_rlpien_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_rlpist_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_rlpist_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_tlpist_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_tlpist_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_txrstp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_txrstp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_plsdis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_plsdis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_lpitxa_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_lpitxa_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_tlpiex_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_tlpiex_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_control_status_lpitxen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_control_status_lpitxen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_timers_control_lst_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_timers_control_lst_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_timers_control_twt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_timers_control_twt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_lpi_auto_entry_timer_lpiet_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_lpi_auto_entry_timer_lpiet_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_1us_tic_counter_tic_1us_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_1us_tic_counter_tic_1us_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_address0_high_addrhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_address0_high_addrhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_address0_high_ae_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_address0_high_ae_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_address0_high_dcs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_address0_high_dcs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_address0_low_addrlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_address0_low_addrlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxorangefis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxorangefis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxlenerfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxlenerfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rx65t127octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rx65t127octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxprmmcis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxprmmcis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rx512t1023octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rx512t1023octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxgboctis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxgboctis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxlpiuscis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxlpiuscis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxjaberfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxjaberfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxvlangbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxvlangbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxpausfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxpausfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxcrcerfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxcrcerfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxdisocgbis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxdisocgbis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxwdogfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxwdogfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rx128t255octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rx128t255octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxdisfcgbis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxdisfcgbis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxosizegfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxosizegfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rx1024tmaxoctgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rx1024tmaxoctgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxruntfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxruntfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxmcgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxmcgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rx256t511octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rx256t511octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rx64octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rx64octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxfovfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxfovfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxgoctis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxgoctis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxgbfrmis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxgbfrmis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxlpitrcis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxlpitrcis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxbcgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxbcgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxusizegfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxusizegfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_rxucgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_rxucgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txgbfrmis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txgbfrmis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txprmmcis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txprmmcis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_tx1024tmaxoctgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_tx1024tmaxoctgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_tx256t511octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_tx256t511octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txlpitrcis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txlpitrcis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txbcgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txbcgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_tx64octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_tx64octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txlpiuscis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txlpiuscis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txuflowerfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txuflowerfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txbcgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txbcgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txpausfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txpausfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txvlangfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txvlangfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txgboctis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txgboctis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txgfrmis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txgfrmis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_tx512t1023octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_tx512t1023octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txmcgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txmcgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txucgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txucgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_tx65t127octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_tx65t127octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txmcgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txmcgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_tx128t255octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_tx128t255octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_txgoctis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_txgoctis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxprmmcise_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxprmmcise_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx65t127octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx65t127octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxruntfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxruntfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxcrcerfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxcrcerfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx256t511octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx256t511octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlenerfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlenerfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxusizegfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxusizegfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxosizegfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxosizegfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxfovfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxfovfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxmcgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxmcgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxvlangbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxvlangbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxwdogfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxwdogfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxdisocie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxdisocie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgbfrmie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgbfrmie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxjaberfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxjaberfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlpiuscie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlpiuscie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxucgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxucgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx1024tmaxoctgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx1024tmaxoctgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxpausfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxpausfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxdisfcie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxdisfcie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgoctie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgoctie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgboctie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgboctie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx128t255octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx128t255octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlpitrcie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlpitrcie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxbcgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxbcgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx64octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx64octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxorangefie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rxorangefie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx512t1023octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_receive_interrupt_enable_rx512t1023octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txucgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txucgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx64octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx64octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txbcgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txbcgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgbfrmie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgbfrmie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgfrmie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgfrmie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgoctie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgoctie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txbcgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txbcgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txlpitrcie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txlpitrcie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txvlangfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txvlangfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txpausfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txpausfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgboctie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgboctie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txmcgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txmcgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txuflowerfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txuflowerfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txlpiuscie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txlpiuscie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx256t511octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx256t511octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx65t127octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx65t127octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx128t255octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx128t255octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx512t1023octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx512t1023octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx1024tmaxoctgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx1024tmaxoctgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txprmmcise_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txprmmcise_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txmcgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mmc_transmit_interrupt_enable_txmcgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

