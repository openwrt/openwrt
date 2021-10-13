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
#ifndef _HPPE_PORTCTRL_H_
#define _HPPE_PORTCTRL_H_


#define MAC_ENABLE_MAX_ENTRY	6
#define MAC_SPEED_MAX_ENTRY	6
#define GOL_MAC_ADDR0_MAX_ENTRY	6
#define GOL_MAC_ADDR1_MAX_ENTRY	6
#define MAC_CTRL0_MAX_ENTRY	6
#define MAC_CTRL1_MAX_ENTRY	6
#define MAC_CTRL2_MAX_ENTRY	6
#define MAC_DBG_CTRL_MAX_ENTRY	6
#define MAC_DBG_ADDR_MAX_ENTRY	6
#define MAC_DBG_DATA_MAX_ENTRY	6
#define MAC_JUMBO_SIZE_MAX_ENTRY	6
#define MC_MTU_CTRL_TBL_MAX_ENTRY	8
#define MRU_MTU_CTRL_TBL_MAX_ENTRY	256
#define RX_FIFO_CFG_MAX_ENTRY	8
#define TDM_CFG_MAX_ENTRY	128
#define PORT_IN_FORWARD_MAX_ENTRY	8
#define PORT_TX_COUNTER_TBL_REG_MAX_ENTRY	8
#define VP_TX_COUNTER_TBL_REG_MAX_ENTRY	256
#define IPR_PKT_NUM_TBL_REG_MAX_ENTRY	8
#define IPR_BYTE_LOW_REG_REG_MAX_ENTRY	8
#define IPR_BYTE_HIGH_REG_MAX_ENTRY	8
#define DROP_CNT_MAX_ENTRY	8
#define DROP_PKT_STAT_MAX_ENTRY	30


sw_error_t
hppe_mac_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_enable_u *value);

sw_error_t
hppe_mac_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_enable_u *value);

sw_error_t
hppe_mac_speed_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_speed_u *value);

sw_error_t
hppe_mac_speed_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_speed_u *value);

sw_error_t
hppe_gol_mac_addr0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union gol_mac_addr0_u *value);

sw_error_t
hppe_gol_mac_addr0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union gol_mac_addr0_u *value);

sw_error_t
hppe_gol_mac_addr1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union gol_mac_addr1_u *value);

sw_error_t
hppe_gol_mac_addr1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union gol_mac_addr1_u *value);

sw_error_t
hppe_mac_ctrl0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl0_u *value);

sw_error_t
hppe_mac_ctrl0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl0_u *value);

sw_error_t
hppe_mac_ctrl1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl1_u *value);

sw_error_t
hppe_mac_ctrl1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl1_u *value);

sw_error_t
hppe_mac_ctrl2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl2_u *value);

sw_error_t
hppe_mac_ctrl2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl2_u *value);

sw_error_t
hppe_mac_dbg_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_ctrl_u *value);

sw_error_t
hppe_mac_dbg_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_ctrl_u *value);

sw_error_t
hppe_mac_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_addr_u *value);

sw_error_t
hppe_mac_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_addr_u *value);

sw_error_t
hppe_mac_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_data_u *value);

sw_error_t
hppe_mac_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_data_u *value);

sw_error_t
hppe_mac_jumbo_size_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_jumbo_size_u *value);

sw_error_t
hppe_mac_jumbo_size_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_jumbo_size_u *value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mru_mtu_ctrl_tbl_u *value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mru_mtu_ctrl_tbl_u *value);

sw_error_t
hppe_mc_mtu_ctrl_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mc_mtu_ctrl_tbl_u *value);

sw_error_t
hppe_mc_mtu_ctrl_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mc_mtu_ctrl_tbl_u *value);

sw_error_t
hppe_tdm_ctrl_get(
		a_uint32_t dev_id,
		union tdm_ctrl_u *value);

sw_error_t
hppe_tdm_ctrl_set(
		a_uint32_t dev_id,
		union tdm_ctrl_u *value);

sw_error_t
hppe_rx_fifo_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifo_cfg_u *value);

sw_error_t
hppe_rx_fifo_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifo_cfg_u *value);

sw_error_t
hppe_tdm_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tdm_cfg_u *value);

sw_error_t
hppe_tdm_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tdm_cfg_u *value);

sw_error_t
hppe_drop_stat_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_stat_u *value);

sw_error_t
hppe_drop_stat_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_stat_u *value);

sw_error_t
hppe_mac_enable_txmac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_enable_txmac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_enable_rxmac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_enable_rxmac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_enable_tx_flow_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_enable_tx_flow_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_enable_rx_flow_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_enable_rx_flow_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_enable_duplex_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_enable_duplex_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_speed_mac_speed_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_speed_mac_speed_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_gol_mac_addr0_mac_addr_byte4_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_gol_mac_addr0_mac_addr_byte4_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_gol_mac_addr0_mac_addr_byte5_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_gol_mac_addr0_mac_addr_byte5_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte3_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_amaxc_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_amaxc_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_ipgt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_ipgt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_nobo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_nobo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_half_thdf_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_half_thdf_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_hugen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_hugen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_bpnb_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_bpnb_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_flchk_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_flchk_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_ipgr2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_ipgr2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_drbnib_rxok_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_drbnib_rxok_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_huge_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_huge_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl0_abebe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl0_abebe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_povr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_povr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_simr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_simr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_jam_ipg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_jam_ipg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_lcol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_lcol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_tctl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_tctl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_retry_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_retry_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_prlen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_prlen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_ppad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_ppad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_long_jam_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_long_jam_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_phug_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_phug_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_sstct_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_sstct_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_mbof_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_mbof_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl1_tpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl1_tpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_ipg_dec_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_ipg_dec_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_mac_rsv_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_mac_rsv_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_mac_tx_thd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_mac_tx_thd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_crc_rsv_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_crc_rsv_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_crs_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_crs_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_ipg_dec_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_ipg_dec_len_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_maxfr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_maxfr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_mac_lpi_tx_idle_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_mac_lpi_tx_idle_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_mac_loop_back_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_mac_loop_back_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_ctrl2_test_pause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_ctrl2_test_pause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_dbg_ctrl_edxsdfr_transmit_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_dbg_ctrl_edxsdfr_transmit_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_dbg_ctrl_hihg_ipg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_dbg_ctrl_hihg_ipg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_dbg_ctrl_mac_ipg_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_dbg_ctrl_mac_ipg_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_dbg_ctrl_mac_len_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_dbg_ctrl_mac_len_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_dbg_ctrl_ipgr1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_dbg_ctrl_ipgr1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_dbg_addr_mac_debug_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_dbg_addr_mac_debug_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_dbg_data_mac_debug_data_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_dbg_data_mac_debug_data_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mac_jumbo_size_mac_jumbo_size_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mac_jumbo_size_mac_jumbo_size_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_mtu_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_mtu_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_rx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_rx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_tx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_tx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_mru_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_mru_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_mru_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_mru_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_src_profile_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_src_profile_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_mtu_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mru_mtu_ctrl_tbl_mtu_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mc_mtu_ctrl_tbl_mtu_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mc_mtu_ctrl_tbl_mtu_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mc_mtu_ctrl_tbl_tx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mc_mtu_ctrl_tbl_tx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mc_mtu_ctrl_tbl_mtu_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mc_mtu_ctrl_tbl_mtu_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);
sw_error_t
hppe_tdm_ctrl_tdm_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_tdm_ctrl_tdm_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_tdm_ctrl_tdm_offset_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_tdm_ctrl_tdm_offset_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_tdm_ctrl_tdm_depth_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_tdm_ctrl_tdm_depth_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_rx_fifo_cfg_rx_fifo_thres_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rx_fifo_cfg_rx_fifo_thres_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tdm_cfg_port_num_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tdm_cfg_port_num_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tdm_cfg_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tdm_cfg_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_tdm_cfg_dir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_tdm_cfg_dir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_in_forward_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_in_forward_u *value);

sw_error_t
hppe_port_in_forward_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_in_forward_u *value);

sw_error_t
hppe_port_in_forward_source_filtering_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_in_forward_source_filtering_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);
sw_error_t
hppe_port_tx_counter_tbl_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_tx_counter_tbl_reg_u *value);

sw_error_t
hppe_port_tx_counter_tbl_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_tx_counter_tbl_reg_u *value);

sw_error_t
hppe_vp_tx_counter_tbl_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vp_tx_counter_tbl_reg_u *value);

sw_error_t
hppe_vp_tx_counter_tbl_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vp_tx_counter_tbl_reg_u *value);

sw_error_t
hppe_epe_dbg_in_cnt_reg_get(
		a_uint32_t dev_id,
		union epe_dbg_in_cnt_reg_u *value);

sw_error_t
hppe_epe_dbg_in_cnt_reg_set(
		a_uint32_t dev_id,
		union epe_dbg_in_cnt_reg_u *value);

sw_error_t
hppe_epe_dbg_out_cnt_reg_get(
		a_uint32_t dev_id,
		union epe_dbg_out_cnt_reg_u *value);

sw_error_t
hppe_epe_dbg_out_cnt_reg_set(
		a_uint32_t dev_id,
		union epe_dbg_out_cnt_reg_u *value);

sw_error_t
hppe_port_tx_counter_tbl_reg_tx_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_port_tx_counter_tbl_reg_tx_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_port_tx_counter_tbl_reg_tx_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_tx_counter_tbl_reg_tx_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_vp_tx_counter_tbl_reg_tx_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_vp_tx_counter_tbl_reg_tx_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_vp_tx_counter_tbl_reg_tx_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_vp_tx_counter_tbl_reg_tx_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_epe_dbg_in_cnt_reg_counter_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_epe_dbg_in_cnt_reg_counter_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_epe_dbg_out_cnt_reg_counter_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_epe_dbg_out_cnt_reg_counter_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_drop_stat_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_drop_stat_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_drop_stat_pkts_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_drop_stat_pkts_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_lpi_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_enable_u *value);

sw_error_t
hppe_lpi_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_enable_u *value);

sw_error_t
hppe_lpi_timer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_port_timer_u *value);

sw_error_t
hppe_lpi_timer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_port_timer_u *value);

sw_error_t
hppe_lpi_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_dbg_addr_u *value);

sw_error_t
hppe_lpi_adb_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_dbg_addr_u *value);

sw_error_t
hppe_lpi_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_dbg_data_u *value);

sw_error_t
hppe_lpi_adb_data_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_dbg_data_u *value);

sw_error_t
hppe_lpi_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_cnt_u *value);

sw_error_t
hppe_lpi_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_cnt_u *value);

sw_error_t
hppe_drop_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_cnt_u *value);

sw_error_t
hppe_drop_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_cnt_u *value);

sw_error_t
hppe_drop_cnt_drop_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_drop_cnt_drop_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipr_pkt_num_tbl_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_pkt_num_tbl_reg_u *value);

sw_error_t
hppe_ipr_pkt_num_tbl_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_pkt_num_tbl_reg_u *value);

sw_error_t
hppe_ipr_byte_low_reg_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_byte_low_reg_reg_u *value);

sw_error_t
hppe_ipr_byte_low_reg_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_byte_low_reg_reg_u *value);

sw_error_t
hppe_ipr_byte_high_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_byte_high_reg_u *value);

sw_error_t
hppe_ipr_byte_high_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_byte_high_reg_u *value);

sw_error_t
hppe_ipr_pkt_num_tbl_reg_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipr_pkt_num_tbl_reg_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipr_byte_low_reg_reg_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipr_byte_low_reg_reg_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ipr_byte_high_reg_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ipr_byte_high_reg_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);


#endif

