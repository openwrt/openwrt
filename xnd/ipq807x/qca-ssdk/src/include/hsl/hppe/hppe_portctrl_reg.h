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
#ifndef HPPE_PORTCTRL_REG_H
#define HPPE_PORTCTRL_REG_H

/*[register] MAC_ENABLE*/
#define MAC_ENABLE
#define MAC_ENABLE_ADDRESS 0x0
#define MAC_ENABLE_NUM     6
#define MAC_ENABLE_INC     0x200
#define MAC_ENABLE_TYPE    REG_TYPE_RW
#define MAC_ENABLE_DEFAULT 0x10
	/*[field] RXMAC_EN*/
	#define MAC_ENABLE_RXMAC_EN
	#define MAC_ENABLE_RXMAC_EN_OFFSET  0
	#define MAC_ENABLE_RXMAC_EN_LEN     1
	#define MAC_ENABLE_RXMAC_EN_DEFAULT 0x0
	/*[field] TXMAC_EN*/
	#define MAC_ENABLE_TXMAC_EN
	#define MAC_ENABLE_TXMAC_EN_OFFSET  1
	#define MAC_ENABLE_TXMAC_EN_LEN     1
	#define MAC_ENABLE_TXMAC_EN_DEFAULT 0x0
	/*[field] DUPLEX*/
	#define MAC_ENABLE_DUPLEX
	#define MAC_ENABLE_DUPLEX_OFFSET  4
	#define MAC_ENABLE_DUPLEX_LEN     1
	#define MAC_ENABLE_DUPLEX_DEFAULT 0x1
	/*[field] RX_FLOW_EN*/
	#define MAC_ENABLE_RX_FLOW_EN
	#define MAC_ENABLE_RX_FLOW_EN_OFFSET  5
	#define MAC_ENABLE_RX_FLOW_EN_LEN     1
	#define MAC_ENABLE_RX_FLOW_EN_DEFAULT 0x0
	/*[field] TX_FLOW_EN*/
	#define MAC_ENABLE_TX_FLOW_EN
	#define MAC_ENABLE_TX_FLOW_EN_OFFSET  6
	#define MAC_ENABLE_TX_FLOW_EN_LEN     1
	#define MAC_ENABLE_TX_FLOW_EN_DEFAULT 0x0

struct mac_enable {
	a_uint32_t  rxmac_en:1;
	a_uint32_t  txmac_en:1;
	a_uint32_t  _reserved0:2;
	a_uint32_t  duplex:1;
	a_uint32_t  rx_flow_en:1;
	a_uint32_t  tx_flow_en:1;
	a_uint32_t  _reserved1:25;
};

union mac_enable_u {
	a_uint32_t val;
	struct mac_enable bf;
};

/*[register] MAC_SPEED*/
#define MAC_SPEED
#define MAC_SPEED_ADDRESS 0x4
#define MAC_SPEED_NUM     6
#define MAC_SPEED_INC     0x200
#define MAC_SPEED_TYPE    REG_TYPE_RW
#define MAC_SPEED_DEFAULT 0x0
	/*[field] MAC_SPEED*/
	#define MAC_SPEED_MAC_SPEED
	#define MAC_SPEED_MAC_SPEED_OFFSET  0
	#define MAC_SPEED_MAC_SPEED_LEN     2
	#define MAC_SPEED_MAC_SPEED_DEFAULT 0x0

struct mac_speed {
	a_uint32_t  mac_speed:2;
	a_uint32_t  _reserved0:30;
};

union mac_speed_u {
	a_uint32_t val;
	struct mac_speed bf;
};

/*[register] GOL_MAC_ADDR0*/
#define GOL_MAC_ADDR0
#define GOL_MAC_ADDR0_ADDRESS 0x8
#define GOL_MAC_ADDR0_NUM     6
#define GOL_MAC_ADDR0_INC     0x200
#define GOL_MAC_ADDR0_TYPE    REG_TYPE_RW
#define GOL_MAC_ADDR0_DEFAULT 0x1
	/*[field] MAC_ADDR_BYTE4*/
	#define GOL_MAC_ADDR0_MAC_ADDR_BYTE4
	#define GOL_MAC_ADDR0_MAC_ADDR_BYTE4_OFFSET  0
	#define GOL_MAC_ADDR0_MAC_ADDR_BYTE4_LEN     8
	#define GOL_MAC_ADDR0_MAC_ADDR_BYTE4_DEFAULT 0x1
	/*[field] MAC_ADDR_BYTE5*/
	#define GOL_MAC_ADDR0_MAC_ADDR_BYTE5
	#define GOL_MAC_ADDR0_MAC_ADDR_BYTE5_OFFSET  8
	#define GOL_MAC_ADDR0_MAC_ADDR_BYTE5_LEN     8
	#define GOL_MAC_ADDR0_MAC_ADDR_BYTE5_DEFAULT 0x0

struct gol_mac_addr0 {
	a_uint32_t  mac_addr_byte4:8;
	a_uint32_t  mac_addr_byte5:8;
	a_uint32_t  _reserved0:16;
};

union gol_mac_addr0_u {
	a_uint32_t val;
	struct gol_mac_addr0 bf;
};

/*[register] GOL_MAC_ADDR1*/
#define GOL_MAC_ADDR1
#define GOL_MAC_ADDR1_ADDRESS 0xc
#define GOL_MAC_ADDR1_NUM     6
#define GOL_MAC_ADDR1_INC     0x200
#define GOL_MAC_ADDR1_TYPE    REG_TYPE_RW
#define GOL_MAC_ADDR1_DEFAULT 0x0
	/*[field] MAC_ADDR_BYTE3*/
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE3
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE3_OFFSET  0
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE3_LEN     8
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE3_DEFAULT 0x0
	/*[field] MAC_ADDR_BYTE2*/
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE2
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE2_OFFSET  8
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE2_LEN     8
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE2_DEFAULT 0x0
	/*[field] MAC_ADDR_BYTE1*/
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE1
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE1_OFFSET  16
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE1_LEN     8
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE1_DEFAULT 0x0
	/*[field] MAC_ADDR_BYTE0*/
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE0
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE0_OFFSET  24
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE0_LEN     8
	#define GOL_MAC_ADDR1_MAC_ADDR_BYTE0_DEFAULT 0x0

struct gol_mac_addr1 {
	a_uint32_t  mac_addr_byte3:8;
	a_uint32_t  mac_addr_byte2:8;
	a_uint32_t  mac_addr_byte1:8;
	a_uint32_t  mac_addr_byte0:8;
};

union gol_mac_addr1_u {
	a_uint32_t val;
	struct gol_mac_addr1 bf;
};

/*[register] MAC_CTRL0*/
#define MAC_CTRL0
#define MAC_CTRL0_ADDRESS 0x10
#define MAC_CTRL0_NUM     6
#define MAC_CTRL0_INC     0x200
#define MAC_CTRL0_TYPE    REG_TYPE_RW
#define MAC_CTRL0_DEFAULT 0xb00e6060
	/*[field] IPGT*/
	#define MAC_CTRL0_IPGT
	#define MAC_CTRL0_IPGT_OFFSET  0
	#define MAC_CTRL0_IPGT_LEN     7
	#define MAC_CTRL0_IPGT_DEFAULT 0x60
	/*[field] IPGR2*/
	#define MAC_CTRL0_IPGR2
	#define MAC_CTRL0_IPGR2_OFFSET  8
	#define MAC_CTRL0_IPGR2_LEN     7
	#define MAC_CTRL0_IPGR2_DEFAULT 0x60
	/*[field] HALF_THDF_CTRL*/
	#define MAC_CTRL0_HALF_THDF_CTRL
	#define MAC_CTRL0_HALF_THDF_CTRL_OFFSET  15
	#define MAC_CTRL0_HALF_THDF_CTRL_LEN     1
	#define MAC_CTRL0_HALF_THDF_CTRL_DEFAULT 0x0
	/*[field] HUGEN*/
	#define MAC_CTRL0_HUGEN
	#define MAC_CTRL0_HUGEN_OFFSET  16
	#define MAC_CTRL0_HUGEN_LEN     1
	#define MAC_CTRL0_HUGEN_DEFAULT 0x0
	/*[field] HUGE*/
	#define MAC_CTRL0_HUGE
	#define MAC_CTRL0_HUGE_OFFSET  17
	#define MAC_CTRL0_HUGE_LEN     1
	#define MAC_CTRL0_HUGE_DEFAULT 0x1
	/*[field] FLCHK*/
	#define MAC_CTRL0_FLCHK
	#define MAC_CTRL0_FLCHK_OFFSET  18
	#define MAC_CTRL0_FLCHK_LEN     1
	#define MAC_CTRL0_FLCHK_DEFAULT 0x1
	/*[field] ABEBE*/
	#define MAC_CTRL0_ABEBE
	#define MAC_CTRL0_ABEBE_OFFSET  19
	#define MAC_CTRL0_ABEBE_LEN     1
	#define MAC_CTRL0_ABEBE_DEFAULT 0x1
	/*[field] AMAXC_EN*/
	#define MAC_CTRL0_AMAXC_EN
	#define MAC_CTRL0_AMAXC_EN_OFFSET  28
	#define MAC_CTRL0_AMAXC_EN_LEN     1
	#define MAC_CTRL0_AMAXC_EN_DEFAULT 0x1
	/*[field] BPNB*/
	#define MAC_CTRL0_BPNB
	#define MAC_CTRL0_BPNB_OFFSET  29
	#define MAC_CTRL0_BPNB_LEN     1
	#define MAC_CTRL0_BPNB_DEFAULT 0x1
	/*[field] NOBO*/
	#define MAC_CTRL0_NOBO
	#define MAC_CTRL0_NOBO_OFFSET  30
	#define MAC_CTRL0_NOBO_LEN     1
	#define MAC_CTRL0_NOBO_DEFAULT 0x0
	/*[field] DRBNIB_RXOK_EN*/
	#define MAC_CTRL0_DRBNIB_RXOK_EN
	#define MAC_CTRL0_DRBNIB_RXOK_EN_OFFSET  31
	#define MAC_CTRL0_DRBNIB_RXOK_EN_LEN     1
	#define MAC_CTRL0_DRBNIB_RXOK_EN_DEFAULT 0x1

struct mac_ctrl0 {
	a_uint32_t  ipgt:7;
	a_uint32_t  _reserved0:1;
	a_uint32_t  ipgr2:7;
	a_uint32_t  half_thdf_ctrl:1;
	a_uint32_t  hugen:1;
	a_uint32_t  huge:1;
	a_uint32_t  flchk:1;
	a_uint32_t  abebe:1;
	a_uint32_t  _reserved1:8;
	a_uint32_t  amaxc_en:1;
	a_uint32_t  bpnb:1;
	a_uint32_t  nobo:1;
	a_uint32_t  drbnib_rxok_en:1;
};

union mac_ctrl0_u {
	a_uint32_t val;
	struct mac_ctrl0 bf;
};

/*[register] MAC_CTRL1*/
#define MAC_CTRL1
#define MAC_CTRL1_ADDRESS 0x14
#define MAC_CTRL1_NUM     6
#define MAC_CTRL1_INC     0x200
#define MAC_CTRL1_TYPE    REG_TYPE_RW
#define MAC_CTRL1_DEFAULT 0x3707f07
	/*[field] JAM_IPG*/
	#define MAC_CTRL1_JAM_IPG
	#define MAC_CTRL1_JAM_IPG_OFFSET  0
	#define MAC_CTRL1_JAM_IPG_LEN     4
	#define MAC_CTRL1_JAM_IPG_DEFAULT 0x7
	/*[field] TPAUSE*/
	#define MAC_CTRL1_TPAUSE
	#define MAC_CTRL1_TPAUSE_OFFSET  4
	#define MAC_CTRL1_TPAUSE_LEN     1
	#define MAC_CTRL1_TPAUSE_DEFAULT 0x0
	/*[field] TCTL*/
	#define MAC_CTRL1_TCTL
	#define MAC_CTRL1_TCTL_OFFSET  5
	#define MAC_CTRL1_TCTL_LEN     1
	#define MAC_CTRL1_TCTL_DEFAULT 0x0
	/*[field] SSTCT*/
	#define MAC_CTRL1_SSTCT
	#define MAC_CTRL1_SSTCT_OFFSET  6
	#define MAC_CTRL1_SSTCT_LEN     1
	#define MAC_CTRL1_SSTCT_DEFAULT 0x0
	/*[field] SIMR*/
	#define MAC_CTRL1_SIMR
	#define MAC_CTRL1_SIMR_OFFSET  7
	#define MAC_CTRL1_SIMR_LEN     1
	#define MAC_CTRL1_SIMR_DEFAULT 0x0
	/*[field] RETRY*/
	#define MAC_CTRL1_RETRY
	#define MAC_CTRL1_RETRY_OFFSET  8
	#define MAC_CTRL1_RETRY_LEN     4
	#define MAC_CTRL1_RETRY_DEFAULT 0xf
	/*[field] PRLEN*/
	#define MAC_CTRL1_PRLEN
	#define MAC_CTRL1_PRLEN_OFFSET  12
	#define MAC_CTRL1_PRLEN_LEN     4
	#define MAC_CTRL1_PRLEN_DEFAULT 0x7
	/*[field] PPAD*/
	#define MAC_CTRL1_PPAD
	#define MAC_CTRL1_PPAD_OFFSET  16
	#define MAC_CTRL1_PPAD_LEN     1
	#define MAC_CTRL1_PPAD_DEFAULT 0x0
	/*[field] POVR*/
	#define MAC_CTRL1_POVR
	#define MAC_CTRL1_POVR_OFFSET  17
	#define MAC_CTRL1_POVR_LEN     1
	#define MAC_CTRL1_POVR_DEFAULT 0x0
	/*[field] PHUG*/
	#define MAC_CTRL1_PHUG
	#define MAC_CTRL1_PHUG_OFFSET  18
	#define MAC_CTRL1_PHUG_LEN     1
	#define MAC_CTRL1_PHUG_DEFAULT 0x0
	/*[field] MBOF*/
	#define MAC_CTRL1_MBOF
	#define MAC_CTRL1_MBOF_OFFSET  19
	#define MAC_CTRL1_MBOF_LEN     1
	#define MAC_CTRL1_MBOF_DEFAULT 0x0
	/*[field] LCOL*/
	#define MAC_CTRL1_LCOL
	#define MAC_CTRL1_LCOL_OFFSET  20
	#define MAC_CTRL1_LCOL_LEN     8
	#define MAC_CTRL1_LCOL_DEFAULT 0x37
	/*[field] LONG_JAM_EN*/
	#define MAC_CTRL1_LONG_JAM_EN
	#define MAC_CTRL1_LONG_JAM_EN_OFFSET  28
	#define MAC_CTRL1_LONG_JAM_EN_LEN     1
	#define MAC_CTRL1_LONG_JAM_EN_DEFAULT 0x0

struct mac_ctrl1 {
	a_uint32_t  jam_ipg:4;
	a_uint32_t  tpause:1;
	a_uint32_t  tctl:1;
	a_uint32_t  sstct:1;
	a_uint32_t  simr:1;
	a_uint32_t  retry:4;
	a_uint32_t  prlen:4;
	a_uint32_t  ppad:1;
	a_uint32_t  povr:1;
	a_uint32_t  phug:1;
	a_uint32_t  mbof:1;
	a_uint32_t  lcol:8;
	a_uint32_t  long_jam_en:1;
	a_uint32_t  _reserved0:3;
};

union mac_ctrl1_u {
	a_uint32_t val;
	struct mac_ctrl1 bf;
};

/*[register] MAC_CTRL2*/
#define MAC_CTRL2
#define MAC_CTRL2_ADDRESS 0x18
#define MAC_CTRL2_NUM     6
#define MAC_CTRL2_INC     0x200
#define MAC_CTRL2_TYPE    REG_TYPE_RW
#define MAC_CTRL2_DEFAULT 0xc271c40
	/*[field] IPG_DEC_LEN*/
	#define MAC_CTRL2_IPG_DEC_LEN
	#define MAC_CTRL2_IPG_DEC_LEN_OFFSET  1
	#define MAC_CTRL2_IPG_DEC_LEN_LEN     1
	#define MAC_CTRL2_IPG_DEC_LEN_DEFAULT 0x0
	/*[field] TEST_PAUSE*/
	#define MAC_CTRL2_TEST_PAUSE
	#define MAC_CTRL2_TEST_PAUSE_OFFSET  2
	#define MAC_CTRL2_TEST_PAUSE_LEN     1
	#define MAC_CTRL2_TEST_PAUSE_DEFAULT 0x0
	/*[field] MAC_LPI_TX_IDLE*/
	#define MAC_CTRL2_MAC_LPI_TX_IDLE
	#define MAC_CTRL2_MAC_LPI_TX_IDLE_OFFSET  3
	#define MAC_CTRL2_MAC_LPI_TX_IDLE_LEN     1
	#define MAC_CTRL2_MAC_LPI_TX_IDLE_DEFAULT 0x0
	/*[field] MAC_LOOP_BACK*/
	#define MAC_CTRL2_MAC_LOOP_BACK
	#define MAC_CTRL2_MAC_LOOP_BACK_OFFSET  4
	#define MAC_CTRL2_MAC_LOOP_BACK_LEN     1
	#define MAC_CTRL2_MAC_LOOP_BACK_DEFAULT 0x0
	/*[field] IPG_DEC_EN*/
	#define MAC_CTRL2_IPG_DEC_EN
	#define MAC_CTRL2_IPG_DEC_EN_OFFSET  5
	#define MAC_CTRL2_IPG_DEC_EN_LEN     1
	#define MAC_CTRL2_IPG_DEC_EN_DEFAULT 0x0
	/*[field] CRS_SEL*/
	#define MAC_CTRL2_CRS_SEL
	#define MAC_CTRL2_CRS_SEL_OFFSET  6
	#define MAC_CTRL2_CRS_SEL_LEN     1
	#define MAC_CTRL2_CRS_SEL_DEFAULT 0x1
	/*[field] CRC_RSV_EN*/
	#define MAC_CTRL2_CRC_RSV_EN
	#define MAC_CTRL2_CRC_RSV_EN_OFFSET  7
	#define MAC_CTRL2_CRC_RSV_EN_LEN     1
	#define MAC_CTRL2_CRC_RSV_EN_DEFAULT 0x0
	/*[field] MAXFR*/
	#define MAC_CTRL2_MAXFR
	#define MAC_CTRL2_MAXFR_OFFSET  8
	#define MAC_CTRL2_MAXFR_LEN     14
	#define MAC_CTRL2_MAXFR_DEFAULT 0x271c
	/*[field] MAC_TX_THD*/
	#define MAC_CTRL2_MAC_TX_THD
	#define MAC_CTRL2_MAC_TX_THD_OFFSET  24
	#define MAC_CTRL2_MAC_TX_THD_LEN     4
	#define MAC_CTRL2_MAC_TX_THD_DEFAULT 0xc
	/*[field] MAC_RSV*/
	#define MAC_CTRL2_MAC_RSV
	#define MAC_CTRL2_MAC_RSV_OFFSET  28
	#define MAC_CTRL2_MAC_RSV_LEN     4
	#define MAC_CTRL2_MAC_RSV_DEFAULT 0x0

struct mac_ctrl2 {
	a_uint32_t  _reserved0:1;
	a_uint32_t  ipg_dec_len:1;
	a_uint32_t  test_pause:1;
	a_uint32_t  mac_lpi_tx_idle:1;
	a_uint32_t  mac_loop_back:1;
	a_uint32_t  ipg_dec_en:1;
	a_uint32_t  crs_sel:1;
	a_uint32_t  crc_rsv_en:1;
	a_uint32_t  maxfr:14;
	a_uint32_t  _reserved1:2;
	a_uint32_t  mac_tx_thd:4;
	a_uint32_t  mac_rsv:4;
};

union mac_ctrl2_u {
	a_uint32_t val;
	struct mac_ctrl2 bf;
};

/*[register] MAC_DBG_CTRL*/
#define MAC_DBG_CTRL
#define MAC_DBG_CTRL_ADDRESS 0x1c
#define MAC_DBG_CTRL_NUM     6
#define MAC_DBG_CTRL_INC     0x200
#define MAC_DBG_CTRL_TYPE    REG_TYPE_RW
#define MAC_DBG_CTRL_DEFAULT 0x80701040
	/*[field] IPGR1*/
	#define MAC_DBG_CTRL_IPGR1
	#define MAC_DBG_CTRL_IPGR1_OFFSET  0
	#define MAC_DBG_CTRL_IPGR1_LEN     7
	#define MAC_DBG_CTRL_IPGR1_DEFAULT 0x40
	/*[field] HIHG_IPG*/
	#define MAC_DBG_CTRL_HIHG_IPG
	#define MAC_DBG_CTRL_HIHG_IPG_OFFSET  8
	#define MAC_DBG_CTRL_HIHG_IPG_LEN     8
	#define MAC_DBG_CTRL_HIHG_IPG_DEFAULT 0x10
	/*[field] MAC_IPG_CTRL*/
	#define MAC_DBG_CTRL_MAC_IPG_CTRL
	#define MAC_DBG_CTRL_MAC_IPG_CTRL_OFFSET  20
	#define MAC_DBG_CTRL_MAC_IPG_CTRL_LEN     4
	#define MAC_DBG_CTRL_MAC_IPG_CTRL_DEFAULT 0x7
	/*[field] MAC_LEN_CTRL*/
	#define MAC_DBG_CTRL_MAC_LEN_CTRL
	#define MAC_DBG_CTRL_MAC_LEN_CTRL_OFFSET  30
	#define MAC_DBG_CTRL_MAC_LEN_CTRL_LEN     1
	#define MAC_DBG_CTRL_MAC_LEN_CTRL_DEFAULT 0x0
	/*[field] EDXSDFR_TRANSMIT_EN*/
	#define MAC_DBG_CTRL_EDXSDFR_TRANSMIT_EN
	#define MAC_DBG_CTRL_EDXSDFR_TRANSMIT_EN_OFFSET  31
	#define MAC_DBG_CTRL_EDXSDFR_TRANSMIT_EN_LEN     1
	#define MAC_DBG_CTRL_EDXSDFR_TRANSMIT_EN_DEFAULT 0x1

struct mac_dbg_ctrl {
	a_uint32_t  ipgr1:7;
	a_uint32_t  _reserved0:1;
	a_uint32_t  hihg_ipg:8;
	a_uint32_t  _reserved1:4;
	a_uint32_t  mac_ipg_ctrl:4;
	a_uint32_t  _reserved2:6;
	a_uint32_t  mac_len_ctrl:1;
	a_uint32_t  edxsdfr_transmit_en:1;
};

union mac_dbg_ctrl_u {
	a_uint32_t val;
	struct mac_dbg_ctrl bf;
};

/*[register] MAC_DBG_ADDR*/
#define MAC_DBG_ADDR
#define MAC_DBG_ADDR_ADDRESS 0x20
#define MAC_DBG_ADDR_NUM     6
#define MAC_DBG_ADDR_INC     0x200
#define MAC_DBG_ADDR_TYPE    REG_TYPE_RW
#define MAC_DBG_ADDR_DEFAULT 0x0
	/*[field] MAC_DEBUG_ADDR*/
	#define MAC_DBG_ADDR_MAC_DEBUG_ADDR
	#define MAC_DBG_ADDR_MAC_DEBUG_ADDR_OFFSET  0
	#define MAC_DBG_ADDR_MAC_DEBUG_ADDR_LEN     8
	#define MAC_DBG_ADDR_MAC_DEBUG_ADDR_DEFAULT 0x0

struct mac_dbg_addr {
	a_uint32_t  mac_debug_addr:8;
	a_uint32_t  _reserved0:24;
};

union mac_dbg_addr_u {
	a_uint32_t val;
	struct mac_dbg_addr bf;
};

/*[register] MAC_DBG_DATA*/
#define MAC_DBG_DATA
#define MAC_DBG_DATA_ADDRESS 0x24
#define MAC_DBG_DATA_NUM     6
#define MAC_DBG_DATA_INC     0x200
#define MAC_DBG_DATA_TYPE    REG_TYPE_RO
#define MAC_DBG_DATA_DEFAULT 0x0
	/*[field] MAC_DEBUG_DATA*/
	#define MAC_DBG_DATA_MAC_DEBUG_DATA
	#define MAC_DBG_DATA_MAC_DEBUG_DATA_OFFSET  0
	#define MAC_DBG_DATA_MAC_DEBUG_DATA_LEN     32
	#define MAC_DBG_DATA_MAC_DEBUG_DATA_DEFAULT 0x0

struct mac_dbg_data {
	a_uint32_t  mac_debug_data:32;
};

union mac_dbg_data_u {
	a_uint32_t val;
	struct mac_dbg_data bf;
};

/*[register] MAC_JUMBO_SIZE*/
#define MAC_JUMBO_SIZE
#define MAC_JUMBO_SIZE_ADDRESS 0x30
#define MAC_JUMBO_SIZE_NUM     6
#define MAC_JUMBO_SIZE_INC     0x200
#define MAC_JUMBO_SIZE_TYPE    REG_TYPE_RW
#define MAC_JUMBO_SIZE_DEFAULT 0x271c
	/*[field] MAC_JUMBO_SIZE*/
	#define MAC_JUMBO_SIZE_MAC_JUMBO_SIZE
	#define MAC_JUMBO_SIZE_MAC_JUMBO_SIZE_OFFSET  0
	#define MAC_JUMBO_SIZE_MAC_JUMBO_SIZE_LEN     14
	#define MAC_JUMBO_SIZE_MAC_JUMBO_SIZE_DEFAULT 0x271c

struct mac_jumbo_size {
	a_uint32_t  mac_jumbo_size:14;
	a_uint32_t  _reserved0:18;
};

union mac_jumbo_size_u {
	a_uint32_t val;
	struct mac_jumbo_size bf;
};

/*[register] PORT_IN_FORWARD*/
#define PORT_IN_FORWARD
#define PORT_IN_FORWARD_ADDRESS 0x700
#define PORT_IN_FORWARD_NUM     8
#define PORT_IN_FORWARD_INC     0x4
#define PORT_IN_FORWARD_TYPE    REG_TYPE_RW
#define PORT_IN_FORWARD_DEFAULT 0x0
	/*[field] SOURCE_FILTERING_BYPASS*/
	#define PORT_IN_FORWARD_SOURCE_FILTERING_BYPASS
	#define PORT_IN_FORWARD_SOURCE_FILTERING_BYPASS_OFFSET  0
	#define PORT_IN_FORWARD_SOURCE_FILTERING_BYPASS_LEN     1
	#define PORT_IN_FORWARD_SOURCE_FILTERING_BYPASS_DEFAULT 0x0

struct port_in_forward {
	a_uint32_t  source_filtering_bypass:1;
	a_uint32_t  _reserved0:31;
};

union port_in_forward_u {
	a_uint32_t val;
	struct port_in_forward bf;
};

/*[table] MRU_MTU_CTRL_TBL*/
#define MRU_MTU_CTRL_TBL
#define MRU_MTU_CTRL_TBL_ADDRESS 0x3000
#define MRU_MTU_CTRL_TBL_NUM     256
#define MRU_MTU_CTRL_TBL_INC     0x8
#define MRU_MTU_CTRL_TBL_TYPE    REG_TYPE_RW
#define MRU_MTU_CTRL_TBL_DEFAULT 0x0
	/*[field] MRU*/
	#define MRU_MTU_CTRL_TBL_MRU
	#define MRU_MTU_CTRL_TBL_MRU_OFFSET  0
	#define MRU_MTU_CTRL_TBL_MRU_LEN     14
	#define MRU_MTU_CTRL_TBL_MRU_DEFAULT 0x0
	/*[field] MRU_CMD*/
	#define MRU_MTU_CTRL_TBL_MRU_CMD
	#define MRU_MTU_CTRL_TBL_MRU_CMD_OFFSET  14
	#define MRU_MTU_CTRL_TBL_MRU_CMD_LEN     2
	#define MRU_MTU_CTRL_TBL_MRU_CMD_DEFAULT 0x0
	/*[field] MTU*/
	#define MRU_MTU_CTRL_TBL_MTU
	#define MRU_MTU_CTRL_TBL_MTU_OFFSET  16
	#define MRU_MTU_CTRL_TBL_MTU_LEN     14
	#define MRU_MTU_CTRL_TBL_MTU_DEFAULT 0x0
	/*[field] MTU_CMD*/
	#define MRU_MTU_CTRL_TBL_MTU_CMD
	#define MRU_MTU_CTRL_TBL_MTU_CMD_OFFSET  30
	#define MRU_MTU_CTRL_TBL_MTU_CMD_LEN     2
	#define MRU_MTU_CTRL_TBL_MTU_CMD_DEFAULT 0x0
	/*[field] RX_CNT_EN*/
	#define MRU_MTU_CTRL_TBL_RX_CNT_EN
	#define MRU_MTU_CTRL_TBL_RX_CNT_EN_OFFSET  32
	#define MRU_MTU_CTRL_TBL_RX_CNT_EN_LEN     1
	#define MRU_MTU_CTRL_TBL_RX_CNT_EN_DEFAULT 0x0
	/*[field] TX_CNT_EN*/
	#define MRU_MTU_CTRL_TBL_TX_CNT_EN
	#define MRU_MTU_CTRL_TBL_TX_CNT_EN_OFFSET  33
	#define MRU_MTU_CTRL_TBL_TX_CNT_EN_LEN     1
	#define MRU_MTU_CTRL_TBL_TX_CNT_EN_DEFAULT 0x0
	/*[field] SRC_PROFILE*/
	#define MRU_MTU_CTRL_TBL_SRC_PROFILE
	#define MRU_MTU_CTRL_TBL_SRC_PROFILE_OFFSET  34
	#define MRU_MTU_CTRL_TBL_SRC_PROFILE_LEN     2
	#define MRU_MTU_CTRL_TBL_SRC_PROFILE_DEFAULT 0x0

struct mru_mtu_ctrl_tbl {
	a_uint32_t  mru:14;
	a_uint32_t  mru_cmd:2;
	a_uint32_t  mtu:14;
	a_uint32_t  mtu_cmd:2;
	a_uint32_t  rx_cnt_en:1;
	a_uint32_t  tx_cnt_en:1;
	a_uint32_t  src_profile:2;
	a_uint32_t  _reserved0:28;
};

union mru_mtu_ctrl_tbl_u {
	a_uint32_t val[2];
	struct mru_mtu_ctrl_tbl bf;
};

/*[register] MC_MTU_CTRL_TBL*/
#define MC_MTU_CTRL_TBL
#define MC_MTU_CTRL_TBL_ADDRESS 0xa00
#define MC_MTU_CTRL_TBL_NUM     8
#define MC_MTU_CTRL_TBL_INC     0x4
#define MC_MTU_CTRL_TBL_TYPE    REG_TYPE_RW
#define MC_MTU_CTRL_TBL_DEFAULT 0x5ea
	/*[field] MTU*/
	#define MC_MTU_CTRL_TBL_MTU
	#define MC_MTU_CTRL_TBL_MTU_OFFSET  0
	#define MC_MTU_CTRL_TBL_MTU_LEN     14
	#define MC_MTU_CTRL_TBL_MTU_DEFAULT 0x5ea
	/*[field] MTU_CMD*/
	#define MC_MTU_CTRL_TBL_MTU_CMD
	#define MC_MTU_CTRL_TBL_MTU_CMD_OFFSET  14
	#define MC_MTU_CTRL_TBL_MTU_CMD_LEN     2
	#define MC_MTU_CTRL_TBL_MTU_CMD_DEFAULT 0x0
	/*[field] TX_CNT_EN*/
	#define MC_MTU_CTRL_TBL_TX_CNT_EN
	#define MC_MTU_CTRL_TBL_TX_CNT_EN_OFFSET  16
	#define MC_MTU_CTRL_TBL_TX_CNT_EN_LEN     1
	#define MC_MTU_CTRL_TBL_TX_CNT_EN_DEFAULT 0x0

struct mc_mtu_ctrl_tbl {
	a_uint32_t  mtu:14;
	a_uint32_t  mtu_cmd:2;
	a_uint32_t  tx_cnt_en:1;
	a_uint32_t  _reserved0:15;
};

union mc_mtu_ctrl_tbl_u {
	a_uint32_t val;
	struct mc_mtu_ctrl_tbl bf;
};

/*[register] TDM_CTRL*/
#define TDM_CTRL
#define TDM_CTRL_ADDRESS 0x0
#define TDM_CTRL_NUM     1
#define TDM_CTRL_INC     0x4
#define TDM_CTRL_TYPE    REG_TYPE_RW
#define TDM_CTRL_DEFAULT 0x80000050
	/*[field] TDM_DEPTH*/
	#define TDM_CTRL_TDM_DEPTH
	#define TDM_CTRL_TDM_DEPTH_OFFSET  0
	#define TDM_CTRL_TDM_DEPTH_LEN     8
	#define TDM_CTRL_TDM_DEPTH_DEFAULT 0x50
	/*[field] TDM_OFFSET*/
	#define TDM_CTRL_TDM_OFFSET
	#define TDM_CTRL_TDM_OFFSET_OFFSET  8
	#define TDM_CTRL_TDM_OFFSET_LEN     7
	#define TDM_CTRL_TDM_OFFSET_DEFAULT 0x0
	/*[field] TDM_EN*/
	#define TDM_CTRL_TDM_EN
	#define TDM_CTRL_TDM_EN_OFFSET  31
	#define TDM_CTRL_TDM_EN_LEN     1
	#define TDM_CTRL_TDM_EN_DEFAULT 0x1

struct tdm_ctrl {
	a_uint32_t  tdm_depth:8;
	a_uint32_t  tdm_offset:7;
	a_uint32_t  _reserved0:16;
	a_uint32_t  tdm_en:1;
};

union tdm_ctrl_u {
	a_uint32_t val;
	struct tdm_ctrl bf;
};

/*[register] RX_FIFO_CFG*/
#define RX_FIFO_CFG
#define RX_FIFO_CFG_ADDRESS 0x4
#define RX_FIFO_CFG_NUM     8
#define RX_FIFO_CFG_INC     0x4
#define RX_FIFO_CFG_TYPE    REG_TYPE_RW
#define RX_FIFO_CFG_DEFAULT 0x4
	/*[field] RX_FIFO_THRES*/
	#define RX_FIFO_CFG_RX_FIFO_THRES
	#define RX_FIFO_CFG_RX_FIFO_THRES_OFFSET  0
	#define RX_FIFO_CFG_RX_FIFO_THRES_LEN     3
	#define RX_FIFO_CFG_RX_FIFO_THRES_DEFAULT 0x4

struct rx_fifo_cfg {
	a_uint32_t  rx_fifo_thres:3;
	a_uint32_t  _reserved0:29;
};

union rx_fifo_cfg_u {
	a_uint32_t val;
	struct rx_fifo_cfg bf;
};

/*[table] TDM_CFG*/
#define TDM_CFG
#define TDM_CFG_ADDRESS 0x1000
#define TDM_CFG_NUM     128
#define TDM_CFG_INC     0x10
#define TDM_CFG_TYPE    REG_TYPE_RW
#define TDM_CFG_DEFAULT 0x0
	/*[field] PORT_NUM*/
	#define TDM_CFG_PORT_NUM
	#define TDM_CFG_PORT_NUM_OFFSET  0
	#define TDM_CFG_PORT_NUM_LEN     4
	#define TDM_CFG_PORT_NUM_DEFAULT 0x0
	/*[field] DIR*/
	#define TDM_CFG_DIR
	#define TDM_CFG_DIR_OFFSET  4
	#define TDM_CFG_DIR_LEN     1
	#define TDM_CFG_DIR_DEFAULT 0x0
	/*[field] VALID*/
	#define TDM_CFG_VALID
	#define TDM_CFG_VALID_OFFSET  5
	#define TDM_CFG_VALID_LEN     1
	#define TDM_CFG_VALID_DEFAULT 0x0

struct tdm_cfg {
	a_uint32_t  port_num:4;
	a_uint32_t  dir:1;
	a_uint32_t  valid:1;
	a_uint32_t  _reserved0:26;
};

union tdm_cfg_u {
	a_uint32_t val;
	struct tdm_cfg bf;
};

/*[table] DROP_STAT*/
#define DROP_STAT
#define DROP_STAT_ADDRESS 0x3000
#define DROP_STAT_NUM     30
#define DROP_STAT_INC     0x10
#define DROP_STAT_TYPE    REG_TYPE_RW
#define DROP_STAT_DEFAULT 0x0
	/*[field] PKTS*/
	#define DROP_STAT_PKTS
	#define DROP_STAT_PKTS_OFFSET  0
	#define DROP_STAT_PKTS_LEN     32
	#define DROP_STAT_PKTS_DEFAULT 0x0
	/*[field] BYTES*/
	#define DROP_STAT_BYTES
	#define DROP_STAT_BYTES_OFFSET  32
	#define DROP_STAT_BYTES_LEN     40
	#define DROP_STAT_BYTES_DEFAULT 0x0

struct drop_stat {
	a_uint32_t  pkts:32;
	a_uint32_t  bytes_0:32;
	a_uint32_t  bytes_1:8;
	a_uint32_t  _reserved0:24;
};

union drop_stat_u {
	a_uint32_t val[3];
	struct drop_stat bf;
};

/*[register] PORT_TX_COUNTER_TBL_REG*/
#define PORT_TX_COUNTER_TBL_REG
#define PORT_TX_COUNTER_TBL_REG_ADDRESS 0x900
#define PORT_TX_COUNTER_TBL_REG_NUM     8
#define PORT_TX_COUNTER_TBL_REG_INC     0x10
#define PORT_TX_COUNTER_TBL_REG_TYPE    REG_TYPE_RW
#define PORT_TX_COUNTER_TBL_REG_DEFAULT 0x0
	/*[field] TX_PACKETS*/
	#define PORT_TX_COUNTER_TBL_REG_TX_PACKETS
	#define PORT_TX_COUNTER_TBL_REG_TX_PACKETS_OFFSET  0
	#define PORT_TX_COUNTER_TBL_REG_TX_PACKETS_LEN     32
	#define PORT_TX_COUNTER_TBL_REG_TX_PACKETS_DEFAULT 0x0
	/*[field] TX_BYTES*/
	#define PORT_TX_COUNTER_TBL_REG_TX_BYTES
	#define PORT_TX_COUNTER_TBL_REG_TX_BYTES_OFFSET  32
	#define PORT_TX_COUNTER_TBL_REG_TX_BYTES_LEN     40
	#define PORT_TX_COUNTER_TBL_REG_TX_BYTES_DEFAULT 0x0

struct port_tx_counter_tbl_reg {
	a_uint32_t  tx_packets:32;
	a_uint32_t  tx_bytes_0:32;
	a_uint32_t  tx_bytes_1:8;
	a_uint32_t  _reserved0:24;
};

union port_tx_counter_tbl_reg_u {
	a_uint32_t val[3];
	struct port_tx_counter_tbl_reg bf;
};

/*[register] VP_TX_COUNTER_TBL_REG*/
#define VP_TX_COUNTER_TBL_REG
#define VP_TX_COUNTER_TBL_REG_ADDRESS 0x1000
#define VP_TX_COUNTER_TBL_REG_NUM     256
#define VP_TX_COUNTER_TBL_REG_INC     0x10
#define VP_TX_COUNTER_TBL_REG_TYPE    REG_TYPE_RW
#define VP_TX_COUNTER_TBL_REG_DEFAULT 0x0
	/*[field] TX_PACKETS*/
	#define VP_TX_COUNTER_TBL_REG_TX_PACKETS
	#define VP_TX_COUNTER_TBL_REG_TX_PACKETS_OFFSET  0
	#define VP_TX_COUNTER_TBL_REG_TX_PACKETS_LEN     32
	#define VP_TX_COUNTER_TBL_REG_TX_PACKETS_DEFAULT 0x0
	/*[field] TX_BYTES*/
	#define VP_TX_COUNTER_TBL_REG_TX_BYTES
	#define VP_TX_COUNTER_TBL_REG_TX_BYTES_OFFSET  32
	#define VP_TX_COUNTER_TBL_REG_TX_BYTES_LEN     40
	#define VP_TX_COUNTER_TBL_REG_TX_BYTES_DEFAULT 0x0

struct vp_tx_counter_tbl_reg {
	a_uint32_t  tx_packets:32;
	a_uint32_t  tx_bytes_0:32;
	a_uint32_t  tx_bytes_1:8;
	a_uint32_t  _reserved0:24;
};

union vp_tx_counter_tbl_reg_u {
	a_uint32_t val[3];
	struct vp_tx_counter_tbl_reg bf;
};

/*[register] EPE_DBG_IN_CNT_REG*/
#define EPE_DBG_IN_CNT_REG
#define EPE_DBG_IN_CNT_REG_ADDRESS 0x6054
#define EPE_DBG_IN_CNT_REG_NUM     1
#define EPE_DBG_IN_CNT_REG_INC     0x4
#define EPE_DBG_IN_CNT_REG_TYPE    REG_TYPE_RW
#define EPE_DBG_IN_CNT_REG_DEFAULT 0x0
	/*[field] COUNTER*/
	#define EPE_DBG_IN_CNT_REG_COUNTER
	#define EPE_DBG_IN_CNT_REG_COUNTER_OFFSET  0
	#define EPE_DBG_IN_CNT_REG_COUNTER_LEN     32
	#define EPE_DBG_IN_CNT_REG_COUNTER_DEFAULT 0x0

struct epe_dbg_in_cnt_reg {
	a_uint32_t  counter:32;
};

union epe_dbg_in_cnt_reg_u {
	a_uint32_t val;
	struct epe_dbg_in_cnt_reg bf;
};

/*[register] EPE_DBG_OUT_CNT_REG*/
#define EPE_DBG_OUT_CNT_REG
#define EPE_DBG_OUT_CNT_REG_ADDRESS 0x6070
#define EPE_DBG_OUT_CNT_REG_NUM     1
#define EPE_DBG_OUT_CNT_REG_INC     0x4
#define EPE_DBG_OUT_CNT_REG_TYPE    REG_TYPE_RW
#define EPE_DBG_OUT_CNT_REG_DEFAULT 0x0
	/*[field] COUNTER*/
	#define EPE_DBG_OUT_CNT_REG_COUNTER
	#define EPE_DBG_OUT_CNT_REG_COUNTER_OFFSET  0
	#define EPE_DBG_OUT_CNT_REG_COUNTER_LEN     32
	#define EPE_DBG_OUT_CNT_REG_COUNTER_DEFAULT 0x0

struct epe_dbg_out_cnt_reg {
	a_uint32_t  counter:32;
};

union epe_dbg_out_cnt_reg_u {
	a_uint32_t val;
	struct epe_dbg_out_cnt_reg bf;
};

/*[register] LPI_ENABLE*/
#define LPI_ENABLE
#define LPI_ENABLE_ADDRESS 0x0
#define LPI_ENABLE_NUM     1
#define LPI_ENABLE_INC     0x0
#define LPI_ENABLE_TYPE    REG_TYPE_RW
#define LPI_ENABLE_DEFAULT 0x0
	/*[field] LPI_PORT1_EN*/
	#define LPI_PORT1_EN
	#define LPI_PORT1_EN_OFFSET  0
	#define LPI_PORT1_EN_LEN     1
	#define LPI_PORT1_EN_DEFAULT 0x0
	/*[field] LPI_PORT2_EN*/
	#define LPI_PORT2_EN
	#define LPI_PORT2_EN_OFFSET  1
	#define LPI_PORT2_EN_LEN     1
	#define LPI_PORT2_EN_DEFAULT 0x0
	/*[field] LPI_PORT3_EN*/
	#define LPI_PORT3_EN
	#define LPI_PORT3_EN_OFFSET  2
	#define LPI_PORT3_EN_LEN     1
	#define LPI_PORT3_EN_DEFAULT 0x0
	/*[field] LPI_PORT4_EN*/
	#define LPI_PORT4_EN
	#define LPI_PORT4_EN_OFFSET  3
	#define LPI_PORT4_EN_LEN     1
	#define LPI_PORT4_EN_DEFAULT 0x0
	/*[field] LPI_PORT5_EN*/
	#define LPI_PORT5_EN
	#define LPI_PORT5_EN_OFFSET  4
	#define LPI_PORT5_EN_LEN     1
	#define LPI_PORT5_EN_DEFAULT 0x0
	/*[field] LPI_LPI_PORT6_EN*/
	#define LPI_PORT6_EN
	#define LPI_PORT6_EN_OFFSET  5
	#define LPI_PORT6_EN_LEN     1
	#define LPI_LPI_PORT6_EN_DEFAULT 0x0
struct lpi_enable {
	a_uint32_t  lpi_port1_en:1;
	a_uint32_t  lpi_port2_en:1;
	a_uint32_t  lpi_port3_en:1;
	a_uint32_t  lpi_port4_en:1;
	a_uint32_t  lpi_port5_en:1;
	a_uint32_t  lpi_port6_en:1;
	a_uint32_t  _reserved0:26;
};

union lpi_enable_u {
	a_uint32_t val;
	struct lpi_enable bf;
};

/*[register] LPI_PORT_TIMER*/
#define LPI_PORT_TIMER_ENABLE
#define LPI_PORT_TIMER_ADDRESS 0x0
#define LPI_PORT_TIMER_NUM     6
#define LPI_PORT_TIMER_INC     0x4
#define LPI_PORT_TIMER_TYPE    REG_TYPE_RW
#define LPI_PORT_TIMER_DEFAULT 0x0
	/*[field] LPI_PORT_WAKEUP_TIMER*/
	#define LPI_PORT_WAKEUP_TIMER
	#define LPI_PORT_WAKEUP_TIMER_OFFSET  0
	#define LPI_PORT_WAKEUP_TIMER_LEN     16
	#define LPI_PORT_WAKEUP_TIMER_DEFAULT 0x0
	/*[field] LPI_PORT_SLEEP_TIMER*/
	#define LPI_PORT_SLEEP_TIMER
	#define LPI_PORT_SLEEP_TIMER_OFFSET  16
	#define LPI_PORT_SLEEP_TIMER_LEN     16
	#define LPI_PORT_SLEEP_TIMER_DEFAULT 0x0

struct lpi_port_timer {
	a_uint32_t  lpi_port_wakeup_timer:16;
	a_uint32_t  lpi_port_sleep_timer:16;
};

union lpi_port_timer_u {
	a_uint32_t val;
	struct lpi_port_timer bf;
};

/*[register] LPI_DBG_ADDR*/
#define LPI_DBG_ADDR
#define LPI_DBG_ADDR_ADDRESS 0x1C
#define LPI_DBG_ADDR_NUM     1
#define LPI_DBG_ADDR_INC     0x1
#define LPI_DBG_ADDR_TYPE    REG_TYPE_RW
#define LPI_DBG_ADDR_DEFAULT 0x0
	/*[field] LPI_DBG_ADDR*/
	#define LPI_DBG_ADDR
	#define IIP_DBG_ADDR_OFFSET  0
	#define LPI_DBG_ADDR_LEN     8
	#define LPI_DBG_ADDR_DEFAULT 0x0

struct lpi_dbg_addr {
	a_uint32_t  lpi_debug_addr:8;
	a_uint32_t  _reserved0:24;
};

union lpi_dbg_addr_u {
	a_uint32_t val;
	struct lpi_dbg_addr bf;
};

/*[register] LPI_DBG_DATA*/
#define LPI_DBG_DATA
#define LPI_DBG_DATA_ADDRESS 0x20
#define LPI_DBG_DATA_NUM     1
#define LPI_DBG_DATA_INC     0x1
#define LPI_DBG_DATA_TYPE    REG_TYPE_RW
#define LPI_DBG_DATA_DEFAULT 0x0
	/*[field] LPI_DBG_DATA*/
	#define LPI_DBG_DATA
	#define LPI_DBG_DATA_OFFSET  0
	#define LPI_DBG_DATA_LEN     32
	#define LPI_DBG_DATA_DEFAULT 0x0

struct lpi_dbg_data {
	a_uint32_t  lpi_debug_data:8;
	a_uint32_t  _reserved0:24;
};

union lpi_dbg_data_u {
	a_uint32_t val;
	struct lpi_dbg_data bf;
};

/*[register] LPI_CNT*/
#define LPI_CNT
#define LPI_CNT_ADDRESS 0x30
#define LPI_CNT_NUM     1
#define LPI_CNT_INC     0x1
#define LPI_CNT_TYPE    REG_TYPE_RW
#define LPI_CNT_DEFAULT 0x0
	/*[field] LPI_CNT*/
	#define LPI_CNT
	#define LPI_CNT_OFFSET  0
	#define LPI_CNT_LEN     9
	#define LPI_CNT_DEFAULT 0x0

struct lpi_cnt {
	a_uint32_t  lpi_cnt_val:9;
	a_uint32_t  _reserved0:23;
};

union lpi_cnt_u {
	a_uint32_t val;
	struct lpi_cnt bf;
};

/*[register] DROP_CNT*/
#define DROP_CNT
#define DROP_CNT_ADDRESS 0x24
#define DROP_CNT_NUM     8
#define DROP_CNT_INC     0x4
#define DROP_CNT_TYPE    REG_TYPE_RW
#define DROP_CNT_DEFAULT 0x0
	/*[field] DROP_CNT*/
	#define DROP_CNT_DROP_CNT
	#define DROP_CNT_DROP_CNT_OFFSET  0
	#define DROP_CNT_DROP_CNT_LEN     32
	#define DROP_CNT_DROP_CNT_DEFAULT 0x0

struct drop_cnt {
	a_uint32_t  drop_cnt:32;
};

union drop_cnt_u {
	a_uint32_t val;
	struct drop_cnt bf;
};

/*[register] IPR_PKT_NUM_TBL_REG*/
#define IPR_PKT_NUM_TBL_REG
#define IPR_PKT_NUM_TBL_REG_ADDRESS 0x80
#define IPR_PKT_NUM_TBL_REG_NUM     8
#define IPR_PKT_NUM_TBL_REG_INC     0x4
#define IPR_PKT_NUM_TBL_REG_TYPE    REG_TYPE_RW
#define IPR_PKT_NUM_TBL_REG_DEFAULT 0x0
	/*[field] PACKETS*/
	#define IPR_PKT_NUM_TBL_REG_PACKETS
	#define IPR_PKT_NUM_TBL_REG_PACKETS_OFFSET  0
	#define IPR_PKT_NUM_TBL_REG_PACKETS_LEN     32
	#define IPR_PKT_NUM_TBL_REG_PACKETS_DEFAULT 0x0

struct ipr_pkt_num_tbl_reg {
	a_uint32_t  packets:32;
};

union ipr_pkt_num_tbl_reg_u {
	a_uint32_t val;
	struct ipr_pkt_num_tbl_reg bf;
};

/*[register] IPR_BYTE_LOW_REG_REG*/
#define IPR_BYTE_LOW_REG_REG
#define IPR_BYTE_LOW_REG_REG_ADDRESS 0xa0
#define IPR_BYTE_LOW_REG_REG_NUM     8
#define IPR_BYTE_LOW_REG_REG_INC     0x4
#define IPR_BYTE_LOW_REG_REG_TYPE    REG_TYPE_RW
#define IPR_BYTE_LOW_REG_REG_DEFAULT 0x0
	/*[field] BYTES*/
	#define IPR_BYTE_LOW_REG_REG_BYTES
	#define IPR_BYTE_LOW_REG_REG_BYTES_OFFSET  0
	#define IPR_BYTE_LOW_REG_REG_BYTES_LEN     32
	#define IPR_BYTE_LOW_REG_REG_BYTES_DEFAULT 0x0

struct ipr_byte_low_reg_reg {
	a_uint32_t  bytes:32;
};

union ipr_byte_low_reg_reg_u {
	a_uint32_t val;
	struct ipr_byte_low_reg_reg bf;
};

/*[register] IPR_BYTE_HIGH_REG*/
#define IPR_BYTE_HIGH_REG
#define IPR_BYTE_HIGH_REG_ADDRESS 0xc0
#define IPR_BYTE_HIGH_REG_NUM     8
#define IPR_BYTE_HIGH_REG_INC     0x4
#define IPR_BYTE_HIGH_REG_TYPE    REG_TYPE_RW
#define IPR_BYTE_HIGH_REG_DEFAULT 0x0
	/*[field] BYTES*/
	#define IPR_BYTE_HIGH_REG_BYTES
	#define IPR_BYTE_HIGH_REG_BYTES_OFFSET  0
	#define IPR_BYTE_HIGH_REG_BYTES_LEN     32
	#define IPR_BYTE_HIGH_REG_BYTES_DEFAULT 0x0

struct ipr_byte_high_reg {
	a_uint32_t  bytes:32;
};

union ipr_byte_high_reg_u {
	a_uint32_t val;
	struct ipr_byte_high_reg bf;
};


#endif
