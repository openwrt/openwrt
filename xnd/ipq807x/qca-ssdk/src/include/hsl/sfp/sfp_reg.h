/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
#ifndef _SFP_REG_H_
#define _SFP_REG_H_

/*[register] SFP_DEV_TYPE*/
#define SFP_DEV_TYPE
#define SFP_DEV_TYPE_ADDRESS 0x0
#define SFP_DEV_TYPE_NUM     1
#define SFP_DEV_TYPE_INC     0x1
#define SFP_DEV_TYPE_TYPE    REG_TYPE_RO
#define SFP_DEV_TYPE_DEFAULT 0x0
	/*[field] ID*/
	#define SFP_DEV_TYPE_ID
	#define SFP_DEV_TYPE_ID_OFFSET  0
	#define SFP_DEV_TYPE_ID_LEN     8
	#define SFP_DEV_TYPE_ID_DEFAULT 0x0

struct sfp_dev_type {
	a_uint8_t  id:8;
};

union sfp_dev_type_u {
	a_uint8_t val;
	struct sfp_dev_type bf;
};

/*[register] SFP_DEV_TYPE_EXT*/
#define SFP_DEV_TYPE_EXT
#define SFP_DEV_TYPE_EXT_ADDRESS 0x1
#define SFP_DEV_TYPE_EXT_NUM     1
#define SFP_DEV_TYPE_EXT_INC     0x1
#define SFP_DEV_TYPE_EXT_TYPE    REG_TYPE_RO
#define SFP_DEV_TYPE_EXT_DEFAULT 0x0
	/*[field] ID*/
	#define SFP_DEV_TYPE_EXT_ID
	#define SFP_DEV_TYPE_EXT_ID_OFFSET  0
	#define SFP_DEV_TYPE_EXT_ID_LEN     8
	#define SFP_DEV_TYPE_EXT_ID_DEFAULT 0x0

struct sfp_dev_type_ext {
	a_uint8_t  id:8;
};

union sfp_dev_type_ext_u {
	a_uint8_t val;
	struct sfp_dev_type_ext bf;
};

/*[register] SFP_DEV_CONNECTOR_TYPE*/
#define SFP_DEV_CONNECTOR_TYPE
#define SFP_DEV_CONNECTOR_TYPE_ADDRESS 0x2
#define SFP_DEV_CONNECTOR_TYPE_NUM     1
#define SFP_DEV_CONNECTOR_TYPE_INC     0x1
#define SFP_DEV_CONNECTOR_TYPE_TYPE    REG_TYPE_RO
#define SFP_DEV_CONNECTOR_TYPE_DEFAULT 0x0
	/*[field] CODE*/
	#define SFP_DEV_CONNECTOR_TYPE_CODE
	#define SFP_DEV_CONNECTOR_TYPE_CODE_OFFSET  0
	#define SFP_DEV_CONNECTOR_TYPE_CODE_LEN     8
	#define SFP_DEV_CONNECTOR_TYPE_CODE_DEFAULT 0x0

struct sfp_dev_connector_type {
	a_uint8_t  code:8;
};

union sfp_dev_connector_type_u {
	a_uint8_t val;
	struct sfp_dev_connector_type bf;
};

/*[register] SFP_TRANSC*/
#define SFP_TRANSC
#define SFP_TRANSC_ADDRESS 0x3
#define SFP_TRANSC_NUM     1
#define SFP_TRANSC_INC     0x8
#define SFP_TRANSC_TYPE    REG_TYPE_RO
#define SFP_TRANSC_DEFAULT 0x0
	/*[field] INFINIBAND_CCODE*/
	#define SFP_TRANSC_INFINIBAND_CCODE
	#define SFP_TRANSC_INFINIBAND_CCODE_OFFSET  0
	#define SFP_TRANSC_INFINIBAND_CCODE_LEN     4
	#define SFP_TRANSC_INFINIBAND_CCODE_DEFAULT 0x0
	/*[field] ETH_10G_CCODE*/
	#define SFP_TRANSC_ETH_10G_CCODE
	#define SFP_TRANSC_ETH_10G_CCODE_OFFSET  4
	#define SFP_TRANSC_ETH_10G_CCODE_LEN     4
	#define SFP_TRANSC_ETH_10G_CCODE_DEFAULT 0x0
	/*[field] SONET_CCODE_1*/
	#define SFP_TRANSC_SONET_CCODE_1
	#define SFP_TRANSC_SONET_CCODE_1_OFFSET  8
	#define SFP_TRANSC_SONET_CCODE_1_LEN     6
	#define SFP_TRANSC_SONET_CCODE_1_DEFAULT 0x0
	/*[field] ESCON_CCODE*/
	#define SFP_TRANSC_ESCON_CCODE
	#define SFP_TRANSC_ESCON_CCODE_OFFSET  14
	#define SFP_TRANSC_ESCON_CCODE_LEN     2
	#define SFP_TRANSC_ESCON_CCODE_DEFAULT 0x0
	/*[field] SONET_CCODE_2*/
	#define SFP_TRANSC_SONET_CCODE_2
	#define SFP_TRANSC_SONET_CCODE_2_OFFSET  16
	#define SFP_TRANSC_SONET_CCODE_2_LEN     8
	#define SFP_TRANSC_SONET_CCODE_2_DEFAULT 0x0
	/*[field] ETH_CCODE*/
	#define SFP_TRANSC_ETH_CCODE
	#define SFP_TRANSC_ETH_CCODE_OFFSET  24
	#define SFP_TRANSC_ETH_CCODE_LEN     8
	#define SFP_TRANSC_ETH_CCODE_DEFAULT 0x0
	/*[field] FIBER_CH_TECH_1*/
	#define SFP_TRANSC_FIBER_CH_TECH_1
	#define SFP_TRANSC_FIBER_CH_TECH_1_OFFSET  32
	#define SFP_TRANSC_FIBER_CH_TECH_1_LEN     3
	#define SFP_TRANSC_FIBER_CH_TECH_1_DEFAULT 0x0
	/*[field] FIBER_CH_LINK_LEN*/
	#define SFP_TRANSC_FIBER_CH_LINK_LEN
	#define SFP_TRANSC_FIBER_CH_LINK_LEN_OFFSET  35
	#define SFP_TRANSC_FIBER_CH_LINK_LEN_LEN     5
	#define SFP_TRANSC_FIBER_CH_LINK_LEN_DEFAULT 0x0
	/*[field] UNALLOCATED*/
	#define SFP_TRANSC_UNALLOCATED
	#define SFP_TRANSC_UNALLOCATED_OFFSET  40
	#define SFP_TRANSC_UNALLOCATED_LEN     2
	#define SFP_TRANSC_UNALLOCATED_DEFAULT 0x0
	/*[field] CABLE_TECH*/
	#define SFP_TRANSC_CABLE_TECH
	#define SFP_TRANSC_CABLE_TECH_OFFSET  42
	#define SFP_TRANSC_CABLE_TECH_LEN     2
	#define SFP_TRANSC_CABLE_TECH_DEFAULT 0x0
	/*[field] FIBER_CH_TECH_2*/
	#define SFP_TRANSC_FIBER_CH_TECH_2
	#define SFP_TRANSC_FIBER_CH_TECH_2_OFFSET  44
	#define SFP_TRANSC_FIBER_CH_TECH_2_LEN     4
	#define SFP_TRANSC_FIBER_CH_TECH_2_DEFAULT 0x0
	/*[field] FIBER_CHAN_TM_MEDIA*/
	#define SFP_TRANSC_FIBER_CHAN_TM_MEDIA
	#define SFP_TRANSC_FIBER_CHAN_TM_MEDIA_OFFSET  48
	#define SFP_TRANSC_FIBER_CHAN_TM_MEDIA_LEN     8
	#define SFP_TRANSC_FIBER_CHAN_TM_MEDIA_DEFAULT 0x0
	/*[field] FIBER_CH_SPEED*/
	#define SFP_TRANSC_FIBER_CH_SPEED
	#define SFP_TRANSC_FIBER_CH_SPEED_OFFSET  56
	#define SFP_TRANSC_FIBER_CH_SPEED_LEN     8
	#define SFP_TRANSC_FIBER_CH_SPEED_DEFAULT 0x0

struct sfp_transc {
	a_uint8_t  infiniband_ccode:4;
	a_uint8_t  eth_10g_ccode:4;
	a_uint8_t  sonet_ccode_1:6;
	a_uint8_t  escon_ccode:2;
	a_uint8_t  sonet_ccode_2:8;
	a_uint8_t  eth_ccode:8;
	a_uint8_t  fiber_ch_tech_1:3;
	a_uint8_t  fiber_ch_link_len:5;
	a_uint8_t  unallocated:2;
	a_uint8_t  cable_tech:2;
	a_uint8_t  fiber_ch_tech_2:4;
	a_uint8_t  fiber_chan_tm_media:8;
	a_uint8_t  fiber_ch_speed:8;
};

union sfp_transc_u {
	a_uint8_t val[8];
	struct sfp_transc bf;
};

/*[register] SFP_ENCODING*/
#define SFP_ENCODING
#define SFP_ENCODING_ADDRESS 0xb
#define SFP_ENCODING_NUM     1
#define SFP_ENCODING_INC     0x1
#define SFP_ENCODING_TYPE    REG_TYPE_RO
#define SFP_ENCODING_DEFAULT 0x0
	/*[field] CODE*/
	#define SFP_ENCODING_CODE
	#define SFP_ENCODING_CODE_OFFSET  0
	#define SFP_ENCODING_CODE_LEN     8
	#define SFP_ENCODING_CODE_DEFAULT 0x0

struct sfp_encoding {
	a_uint8_t  code:8;
};

union sfp_encoding_u {
	a_uint8_t val;
	struct sfp_encoding bf;
};

/*[register] SFP_BR*/
#define SFP_BR
#define SFP_BR_ADDRESS 0xc
#define SFP_BR_NUM     1
#define SFP_BR_INC     0x1
#define SFP_BR_TYPE    REG_TYPE_RO
#define SFP_BR_DEFAULT 0x0
	/*[field] BIT*/
	#define SFP_BR_BIT
	#define SFP_BR_BIT_OFFSET  0
	#define SFP_BR_BIT_LEN     8
	#define SFP_BR_BIT_DEFAULT 0x0

struct sfp_br {
	a_uint8_t  bit:8;
};

union sfp_br_u {
	a_uint8_t val;
	struct sfp_br bf;
};

/*[register] SFP_RATE*/
#define SFP_RATE
#define SFP_RATE_ADDRESS 0xd
#define SFP_RATE_NUM     1
#define SFP_RATE_INC     0x1
#define SFP_RATE_TYPE    REG_TYPE_RO
#define SFP_RATE_DEFAULT 0x0
	/*[field] ID*/
	#define SFP_RATE_ID
	#define SFP_RATE_ID_OFFSET  0
	#define SFP_RATE_ID_LEN     8
	#define SFP_RATE_ID_DEFAULT 0x0

struct sfp_rate {
	a_uint8_t  id:8;
};

union sfp_rate_u {
	a_uint8_t val;
	struct sfp_rate bf;
};

/*[register] SFP_LINK_LEN*/
#define SFP_LINK_LEN
#define SFP_LINK_LEN_ADDRESS 0xe
#define SFP_LINK_LEN_NUM     1
#define SFP_LINK_LEN_INC     0x6
#define SFP_LINK_LEN_TYPE    REG_TYPE_RO
#define SFP_LINK_LEN_DEFAULT 0x0
	/*[field] SINGLE_MODE_KM*/
	#define SFP_LINK_LEN_SINGLE_MODE_KM
	#define SFP_LINK_LEN_SINGLE_MODE_KM_OFFSET  0
	#define SFP_LINK_LEN_SINGLE_MODE_KM_LEN     8
	#define SFP_LINK_LEN_SINGLE_MODE_KM_DEFAULT 0x0
	/*[field] SINGLE_MODE_100M*/
	#define SFP_LINK_LEN_SINGLE_MODE_100M
	#define SFP_LINK_LEN_SINGLE_MODE_100M_OFFSET  8
	#define SFP_LINK_LEN_SINGLE_MODE_100M_LEN     8
	#define SFP_LINK_LEN_SINGLE_MODE_100M_DEFAULT 0x0
	/*[field] OM2_MODE_10M*/
	#define SFP_LINK_LEN_OM2_MODE_10M
	#define SFP_LINK_LEN_OM2_MODE_10M_OFFSET  16
	#define SFP_LINK_LEN_OM2_MODE_10M_LEN     8
	#define SFP_LINK_LEN_OM2_MODE_10M_DEFAULT 0x0
	/*[field] OM1_MODE_10M*/
	#define SFP_LINK_LEN_OM1_MODE_10M
	#define SFP_LINK_LEN_OM1_MODE_10M_OFFSET  24
	#define SFP_LINK_LEN_OM1_MODE_10M_LEN     8
	#define SFP_LINK_LEN_OM1_MODE_10M_DEFAULT 0x0
	/*[field] COPPER_MODE_1M*/
	#define SFP_LINK_LEN_COPPER_MODE_1M
	#define SFP_LINK_LEN_COPPER_MODE_1M_OFFSET  32
	#define SFP_LINK_LEN_COPPER_MODE_1M_LEN     8
	#define SFP_LINK_LEN_COPPER_MODE_1M_DEFAULT 0x0
	/*[field] OM3_MODE_1M*/
	#define SFP_LINK_LEN_OM3_MODE_1M
	#define SFP_LINK_LEN_OM3_MODE_1M_OFFSET  40
	#define SFP_LINK_LEN_OM3_MODE_1M_LEN     8
	#define SFP_LINK_LEN_OM3_MODE_1M_DEFAULT 0x0

struct sfp_link_len {
	a_uint8_t  single_mode_km:8;
	a_uint8_t  single_mode_100m:8;
	a_uint8_t  om2_mode_10m:8;
	a_uint8_t  om1_mode_10m:8;
	a_uint8_t  copper_mode_1m:8;
	a_uint8_t  om3_mode_1m:8;
};

union sfp_link_len_u {
	a_uint8_t val[6];
	struct sfp_link_len bf;
};

/*[register] SFP_VENDOR*/
#define SFP_VENDOR
#define SFP_VENDOR_ADDRESS 0x14
#define SFP_VENDOR_NUM     1
#define SFP_VENDOR_INC     0x28
#define SFP_VENDOR_TYPE    REG_TYPE_RO
#define SFP_VENDOR_DEFAULT 0x0
	/*[field] NAME*/
	#define SFP_VENDOR_NAME
	#define SFP_VENDOR_NAME_OFFSET  0
	#define SFP_VENDOR_NAME_LEN     128
	#define SFP_VENDOR_NAME_DEFAULT 0x0
	/*[field] TRANSCODE*/
	#define SFP_VENDOR_TRANSCODE
	#define SFP_VENDOR_TRANSCODE_OFFSET  128
	#define SFP_VENDOR_TRANSCODE_LEN     8
	#define SFP_VENDOR_TRANSCODE_DEFAULT 0x0
	/*[field] OUI*/
	#define SFP_VENDOR_OUI
	#define SFP_VENDOR_OUI_OFFSET  136
	#define SFP_VENDOR_OUI_LEN     24
	#define SFP_VENDOR_OUI_DEFAULT 0x0
	/*[field] PN*/
	#define SFP_VENDOR_PN
	#define SFP_VENDOR_PN_OFFSET  160
	#define SFP_VENDOR_PN_LEN     128
	#define SFP_VENDOR_PN_DEFAULT 0x0
	/*[field] REV*/
	#define SFP_VENDOR_REV
	#define SFP_VENDOR_REV_OFFSET  288
	#define SFP_VENDOR_REV_LEN     32
	#define SFP_VENDOR_REV_DEFAULT 0x0

struct sfp_vendor {
	a_uint8_t  name_0:8;
	a_uint8_t  name_1:8;
	a_uint8_t  name_2:8;
	a_uint8_t  name_3:8;
	a_uint8_t  name_4:8;
	a_uint8_t  name_5:8;
	a_uint8_t  name_6:8;
	a_uint8_t  name_7:8;
	a_uint8_t  name_8:8;
	a_uint8_t  name_9:8;
	a_uint8_t  name_10:8;
	a_uint8_t  name_11:8;
	a_uint8_t  name_12:8;
	a_uint8_t  name_13:8;
	a_uint8_t  name_14:8;
	a_uint8_t  name_15:8;
	a_uint8_t  transcode:8;
	a_uint8_t  oui_0:8;
	a_uint8_t  oui_1:8;
	a_uint8_t  oui_2:8;
	a_uint8_t  pn_0:8;
	a_uint8_t  pn_1:8;
	a_uint8_t  pn_2:8;
	a_uint8_t  pn_3:8;
	a_uint8_t  pn_4:8;
	a_uint8_t  pn_5:8;
	a_uint8_t  pn_6:8;
	a_uint8_t  pn_7:8;
	a_uint8_t  pn_8:8;
	a_uint8_t  pn_9:8;
	a_uint8_t  pn_10:8;
	a_uint8_t  pn_11:8;
	a_uint8_t  pn_12:8;
	a_uint8_t  pn_13:8;
	a_uint8_t  pn_14:8;
	a_uint8_t  pn_15:8;
	a_uint8_t  rev_0:8;
	a_uint8_t  rev_1:8;
	a_uint8_t  rev_2:8;
	a_uint8_t  rev_3:8;
};

union sfp_vendor_u {
	a_uint8_t val[40];
	struct sfp_vendor bf;
};

/*[register] SFP_LASER*/
#define SFP_LASER
#define SFP_LASER_ADDRESS 0x3c
#define SFP_LASER_NUM     1
#define SFP_LASER_INC     0x2
#define SFP_LASER_TYPE    REG_TYPE_RO
#define SFP_LASER_DEFAULT 0x0
	/*[field] WAVELENGTH*/
	#define SFP_LASER_WAVELENGTH
	#define SFP_LASER_WAVELENGTH_OFFSET  0
	#define SFP_LASER_WAVELENGTH_LEN     16
	#define SFP_LASER_WAVELENGTH_DEFAULT 0x0

struct sfp_laser {
	a_uint8_t  wavelength_0:8;
	a_uint8_t  wavelength_1:8;
};

union sfp_laser_u {
	a_uint8_t val[2];
	struct sfp_laser bf;
};

/*[register] SFP_BASE*/
#define SFP_BASE
#define SFP_BASE_ADDRESS 0x3f
#define SFP_BASE_NUM     1
#define SFP_BASE_INC     0x1
#define SFP_BASE_TYPE    REG_TYPE_RO
#define SFP_BASE_DEFAULT 0x0
	/*[field] CHECK_CODE*/
	#define SFP_BASE_CHECK_CODE
	#define SFP_BASE_CHECK_CODE_OFFSET  0
	#define SFP_BASE_CHECK_CODE_LEN     8
	#define SFP_BASE_CHECK_CODE_DEFAULT 0x0

struct sfp_base {
	a_uint8_t  check_code:8;
};

union sfp_base_u {
	a_uint8_t val;
	struct sfp_base bf;
};

/*[register] SFP_OPTION*/
#define SFP_OPTION
#define SFP_OPTION_ADDRESS 0x40
#define SFP_OPTION_NUM     1
#define SFP_OPTION_INC     0x2
#define SFP_OPTION_TYPE    REG_TYPE_RO
#define SFP_OPTION_DEFAULT 0x0
	/*[field] LINEAR_RECV_OUTPUT*/
	#define SFP_OPTION_LINEAR_RECV_OUTPUT
	#define SFP_OPTION_LINEAR_RECV_OUTPUT_OFFSET  0
	#define SFP_OPTION_LINEAR_RECV_OUTPUT_LEN     1
	#define SFP_OPTION_LINEAR_RECV_OUTPUT_DEFAULT 0x0
	/*[field] PWR_LEVEL_DECLAR*/
	#define SFP_OPTION_PWR_LEVEL_DECLAR
	#define SFP_OPTION_PWR_LEVEL_DECLAR_OFFSET  1
	#define SFP_OPTION_PWR_LEVEL_DECLAR_LEN     1
	#define SFP_OPTION_PWR_LEVEL_DECLAR_DEFAULT 0x0
	/*[field] COOL_TRANSC_DECLAR*/
	#define SFP_OPTION_COOL_TRANSC_DECLAR
	#define SFP_OPTION_COOL_TRANSC_DECLAR_OFFSET  2
	#define SFP_OPTION_COOL_TRANSC_DECLAR_LEN     1
	#define SFP_OPTION_COOL_TRANSC_DECLAR_DEFAULT 0x0
	/*[field] UNALLOCATED_1*/
	#define SFP_OPTION_UNALLOCATED_1
	#define SFP_OPTION_UNALLOCATED_1_OFFSET  3
	#define SFP_OPTION_UNALLOCATED_1_LEN     5
	#define SFP_OPTION_UNALLOCATED_1_DEFAULT 0x0
	/*[field] UNALLOCATED_2*/
	#define SFP_OPTION_UNALLOCATED_2
	#define SFP_OPTION_UNALLOCATED_2_OFFSET  8
	#define SFP_OPTION_UNALLOCATED_2_LEN     1
	#define SFP_OPTION_UNALLOCATED_2_DEFAULT 0x0
	/*[field] LOSS_SIGNAL*/
	#define SFP_OPTION_LOSS_SIGNAL
	#define SFP_OPTION_LOSS_SIGNAL_OFFSET  9
	#define SFP_OPTION_LOSS_SIGNAL_LEN     1
	#define SFP_OPTION_LOSS_SIGNAL_DEFAULT 0x0
	/*[field] LOSS_INVERT_SIGNAL*/
	#define SFP_OPTION_LOSS_INVERT_SIGNAL
	#define SFP_OPTION_LOSS_INVERT_SIGNAL_OFFSET  10
	#define SFP_OPTION_LOSS_INVERT_SIGNAL_LEN     1
	#define SFP_OPTION_LOSS_INVERT_SIGNAL_DEFAULT 0x0
	/*[field] TX_FAULT_SIGNAL*/
	#define SFP_OPTION_TX_FAULT_SIGNAL
	#define SFP_OPTION_TX_FAULT_SIGNAL_OFFSET  11
	#define SFP_OPTION_TX_FAULT_SIGNAL_LEN     1
	#define SFP_OPTION_TX_FAULT_SIGNAL_DEFAULT 0x0
	/*[field] TX_DISABLE*/
	#define SFP_OPTION_TX_DISABLE
	#define SFP_OPTION_TX_DISABLE_OFFSET  12
	#define SFP_OPTION_TX_DISABLE_LEN     1
	#define SFP_OPTION_TX_DISABLE_DEFAULT 0x0
	/*[field] RATE_SEL*/
	#define SFP_OPTION_RATE_SEL
	#define SFP_OPTION_RATE_SEL_OFFSET  13
	#define SFP_OPTION_RATE_SEL_LEN     1
	#define SFP_OPTION_RATE_SEL_DEFAULT 0x0
	/*[field] UNALLOCATED_3*/
	#define SFP_OPTION_UNALLOCATED_3
	#define SFP_OPTION_UNALLOCATED_3_OFFSET  14
	#define SFP_OPTION_UNALLOCATED_3_LEN     2
	#define SFP_OPTION_UNALLOCATED_3_DEFAULT 0x0

struct sfp_option {
	a_uint8_t  linear_recv_output:1;
	a_uint8_t  pwr_level_declar:1;
	a_uint8_t  cool_transc_declar:1;
	a_uint8_t  unallocated_1:5;
	a_uint8_t  unallocated_2:1;
	a_uint8_t  loss_signal:1;
	a_uint8_t  loss_invert_signal:1;
	a_uint8_t  tx_fault_signal:1;
	a_uint8_t  tx_disable:1;
	a_uint8_t  rate_sel:1;
	a_uint8_t  unallocated_3:2;
};

union sfp_option_u {
	a_uint8_t val[2];
	struct sfp_option bf;
};

/*[register] SFP_RATE_CTRL*/
#define SFP_RATE_CTRL
#define SFP_RATE_CTRL_ADDRESS 0x42
#define SFP_RATE_CTRL_NUM     1
#define SFP_RATE_CTRL_INC     0x2
#define SFP_RATE_CTRL_TYPE    REG_TYPE_RO
#define SFP_RATE_CTRL_DEFAULT 0x0
	/*[field] UPPER*/
	#define SFP_RATE_CTRL_UPPER
	#define SFP_RATE_CTRL_UPPER_OFFSET  0
	#define SFP_RATE_CTRL_UPPER_LEN     8
	#define SFP_RATE_CTRL_UPPER_DEFAULT 0x0
	/*[field] LOWER*/
	#define SFP_RATE_CTRL_LOWER
	#define SFP_RATE_CTRL_LOWER_OFFSET  8
	#define SFP_RATE_CTRL_LOWER_LEN     8
	#define SFP_RATE_CTRL_LOWER_DEFAULT 0x0

struct sfp_rate_ctrl {
	a_uint8_t  upper:8;
	a_uint8_t  lower:8;
};

union sfp_rate_ctrl_u {
	a_uint8_t val[2];
	struct sfp_rate_ctrl bf;
};

/*[register] SFP_VENDOR_EXT*/
#define SFP_VENDOR_EXT
#define SFP_VENDOR_EXT_ADDRESS 0x44
#define SFP_VENDOR_EXT_NUM     1
#define SFP_VENDOR_EXT_INC     0x18
#define SFP_VENDOR_EXT_TYPE    REG_TYPE_RO
#define SFP_VENDOR_EXT_DEFAULT 0x0
	/*[field] SN*/
	#define SFP_VENDOR_EXT_SN
	#define SFP_VENDOR_EXT_SN_OFFSET  0
	#define SFP_VENDOR_EXT_SN_LEN     128
	#define SFP_VENDOR_EXT_SN_DEFAULT 0x0
	/*[field] DATE_CODE*/
	#define SFP_VENDOR_EXT_DATE_CODE
	#define SFP_VENDOR_EXT_DATE_CODE_OFFSET  128
	#define SFP_VENDOR_EXT_DATE_CODE_LEN     64
	#define SFP_VENDOR_EXT_DATE_CODE_DEFAULT 0x0

struct sfp_vendor_ext {
	a_uint8_t  sn_0:8;
	a_uint8_t  sn_1:8;
	a_uint8_t  sn_2:8;
	a_uint8_t  sn_3:8;
	a_uint8_t  sn_4:8;
	a_uint8_t  sn_5:8;
	a_uint8_t  sn_6:8;
	a_uint8_t  sn_7:8;
	a_uint8_t  sn_8:8;
	a_uint8_t  sn_9:8;
	a_uint8_t  sn_10:8;
	a_uint8_t  sn_11:8;
	a_uint8_t  sn_12:8;
	a_uint8_t  sn_13:8;
	a_uint8_t  sn_14:8;
	a_uint8_t  sn_15:8;
	a_uint8_t  date_code_0:8;
	a_uint8_t  date_code_1:8;
	a_uint8_t  date_code_2:8;
	a_uint8_t  date_code_3:8;
	a_uint8_t  date_code_4:8;
	a_uint8_t  date_code_5:8;
	a_uint8_t  date_code_6:8;
	a_uint8_t  date_code_7:8;
};

union sfp_vendor_ext_u {
	a_uint8_t val[24];
	struct sfp_vendor_ext bf;
};

/*[register] SFP_ENHANCED*/
#define SFP_ENHANCED
#define SFP_ENHANCED_ADDRESS 0x5c
#define SFP_ENHANCED_NUM     1
#define SFP_ENHANCED_INC     0x3
#define SFP_ENHANCED_TYPE    REG_TYPE_RO
#define SFP_ENHANCED_DEFAULT 0x0
	/*[field] UNALLOCATED_TYPE*/
	#define SFP_ENHANCED_UNALLOCATED_TYPE
	#define SFP_ENHANCED_UNALLOCATED_TYPE_OFFSET  0
	#define SFP_ENHANCED_UNALLOCATED_TYPE_LEN     2
	#define SFP_ENHANCED_UNALLOCATED_TYPE_DEFAULT 0x0
	/*[field] ADDR_MODE*/
	#define SFP_ENHANCED_ADDR_MODE
	#define SFP_ENHANCED_ADDR_MODE_OFFSET  2
	#define SFP_ENHANCED_ADDR_MODE_LEN     1
	#define SFP_ENHANCED_ADDR_MODE_DEFAULT 0x0
	/*[field] RE_PWR_TYPE*/
	#define SFP_ENHANCED_RE_PWR_TYPE
	#define SFP_ENHANCED_RE_PWR_TYPE_OFFSET  3
	#define SFP_ENHANCED_RE_PWR_TYPE_LEN     1
	#define SFP_ENHANCED_RE_PWR_TYPE_DEFAULT 0x0
	/*[field] EXTERNAL_CAL*/
	#define SFP_ENHANCED_EXTERNAL_CAL
	#define SFP_ENHANCED_EXTERNAL_CAL_OFFSET  4
	#define SFP_ENHANCED_EXTERNAL_CAL_LEN     1
	#define SFP_ENHANCED_EXTERNAL_CAL_DEFAULT 0x0
	/*[field] INTERNAL_CAL*/
	#define SFP_ENHANCED_INTERNAL_CAL
	#define SFP_ENHANCED_INTERNAL_CAL_OFFSET  5
	#define SFP_ENHANCED_INTERNAL_CAL_LEN     1
	#define SFP_ENHANCED_INTERNAL_CAL_DEFAULT 0x0
	/*[field] DIAG_MON_FLAG*/
	#define SFP_ENHANCED_DIAG_MON_FLAG
	#define SFP_ENHANCED_DIAG_MON_FLAG_OFFSET  6
	#define SFP_ENHANCED_DIAG_MON_FLAG_LEN     1
	#define SFP_ENHANCED_DIAG_MON_FLAG_DEFAULT 0x0
	/*[field] LEGACY_TYPE*/
	#define SFP_ENHANCED_LEGACY_TYPE
	#define SFP_ENHANCED_LEGACY_TYPE_OFFSET  7
	#define SFP_ENHANCED_LEGACY_TYPE_LEN     1
	#define SFP_ENHANCED_LEGACY_TYPE_DEFAULT 0x0
	/*[field] UNALLOCATED_OP*/
	#define SFP_ENHANCED_UNALLOCATED_OP
	#define SFP_ENHANCED_UNALLOCATED_OP_OFFSET  8
	#define SFP_ENHANCED_UNALLOCATED_OP_LEN     1
	#define SFP_ENHANCED_UNALLOCATED_OP_DEFAULT 0x0
	/*[field] SOFT_RATE_SEL_OP*/
	#define SFP_ENHANCED_SOFT_RATE_SEL_OP
	#define SFP_ENHANCED_SOFT_RATE_SEL_OP_OFFSET  9
	#define SFP_ENHANCED_SOFT_RATE_SEL_OP_LEN     1
	#define SFP_ENHANCED_SOFT_RATE_SEL_OP_DEFAULT 0x0
	/*[field] APP_SEL_OP*/
	#define SFP_ENHANCED_APP_SEL_OP
	#define SFP_ENHANCED_APP_SEL_OP_OFFSET  10
	#define SFP_ENHANCED_APP_SEL_OP_LEN     1
	#define SFP_ENHANCED_APP_SEL_OP_DEFAULT 0x0
	/*[field] SOFT_RATE_CTRL_OP*/
	#define SFP_ENHANCED_SOFT_RATE_CTRL_OP
	#define SFP_ENHANCED_SOFT_RATE_CTRL_OP_OFFSET  11
	#define SFP_ENHANCED_SOFT_RATE_CTRL_OP_LEN     1
	#define SFP_ENHANCED_SOFT_RATE_CTRL_OP_DEFAULT 0x0
	/*[field] RX_LOS_OP*/
	#define SFP_ENHANCED_RX_LOS_OP
	#define SFP_ENHANCED_RX_LOS_OP_OFFSET  12
	#define SFP_ENHANCED_RX_LOS_OP_LEN     1
	#define SFP_ENHANCED_RX_LOS_OP_DEFAULT 0x0
	/*[field] TX_FAULT_OP*/
	#define SFP_ENHANCED_TX_FAULT_OP
	#define SFP_ENHANCED_TX_FAULT_OP_OFFSET  13
	#define SFP_ENHANCED_TX_FAULT_OP_LEN     1
	#define SFP_ENHANCED_TX_FAULT_OP_DEFAULT 0x0
	/*[field] TX_DISABLE_CTRL_OP*/
	#define SFP_ENHANCED_TX_DISABLE_CTRL_OP
	#define SFP_ENHANCED_TX_DISABLE_CTRL_OP_OFFSET  14
	#define SFP_ENHANCED_TX_DISABLE_CTRL_OP_LEN     1
	#define SFP_ENHANCED_TX_DISABLE_CTRL_OP_DEFAULT 0x0
	/*[field] ALARM_WARNING_FLAG_OP*/
	#define SFP_ENHANCED_ALARM_WARNING_FLAG_OP
	#define SFP_ENHANCED_ALARM_WARNING_FLAG_OP_OFFSET  15
	#define SFP_ENHANCED_ALARM_WARNING_FLAG_OP_LEN     1
	#define SFP_ENHANCED_ALARM_WARNING_FLAG_OP_DEFAULT 0x0
	/*[field] CMPL_FEATURE*/
	#define SFP_ENHANCED_CMPL_FEATURE
	#define SFP_ENHANCED_CMPL_FEATURE_OFFSET  16
	#define SFP_ENHANCED_CMPL_FEATURE_LEN     8
	#define SFP_ENHANCED_CMPL_FEATURE_DEFAULT 0x0

struct sfp_enhanced {
	a_uint8_t  unallocated_type:2;
	a_uint8_t  addr_mode:1;
	a_uint8_t  re_pwr_type:1;
	a_uint8_t  external_cal:1;
	a_uint8_t  internal_cal:1;
	a_uint8_t  diag_mon_flag:1;
	a_uint8_t  legacy_type:1;
	a_uint8_t  unallocated_op:1;
	a_uint8_t  soft_rate_sel_op:1;
	a_uint8_t  app_sel_op:1;
	a_uint8_t  soft_rate_ctrl_op:1;
	a_uint8_t  rx_los_op:1;
	a_uint8_t  tx_fault_op:1;
	a_uint8_t  tx_disable_ctrl_op:1;
	a_uint8_t  alarm_warning_flag_op:1;
	a_uint8_t  cmpl_feature:8;
};

union sfp_enhanced_u {
	a_uint8_t val[3];
	struct sfp_enhanced bf;
};

/*[register] SFP_EXT*/
#define SFP_EXT
#define SFP_EXT_ADDRESS 0x5f
#define SFP_EXT_NUM     1
#define SFP_EXT_INC     0x1
#define SFP_EXT_TYPE    REG_TYPE_RO
#define SFP_EXT_DEFAULT 0x0
	/*[field] CHECK_CODE*/
	#define SFP_EXT_CHECK_CODE
	#define SFP_EXT_CHECK_CODE_OFFSET  0
	#define SFP_EXT_CHECK_CODE_LEN     8
	#define SFP_EXT_CHECK_CODE_DEFAULT 0x0

struct sfp_ext {
	a_uint8_t  check_code:8;
};

union sfp_ext_u {
	a_uint8_t val;
	struct sfp_ext bf;
};

/*[register] SFP_DIAG_THRESHOLD*/
#define SFP_DIAG_THRESHOLD
#define SFP_DIAG_THRESHOLD_ADDRESS 0x0
#define SFP_DIAG_THRESHOLD_NUM     1
#define SFP_DIAG_THRESHOLD_INC     0x28
#define SFP_DIAG_THRESHOLD_TYPE    REG_TYPE_RO
#define SFP_DIAG_THRESHOLD_DEFAULT 0x0
	/*[field] TEMP_HIGH_ALARM*/
	#define SFP_DIAG_THRESHOLD_TEMP_HIGH_ALARM
	#define SFP_DIAG_THRESHOLD_TEMP_HIGH_ALARM_OFFSET  0
	#define SFP_DIAG_THRESHOLD_TEMP_HIGH_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_TEMP_HIGH_ALARM_DEFAULT 0x0
	/*[field] TEMP_LOW_ALARM*/
	#define SFP_DIAG_THRESHOLD_TEMP_LOW_ALARM
	#define SFP_DIAG_THRESHOLD_TEMP_LOW_ALARM_OFFSET  16
	#define SFP_DIAG_THRESHOLD_TEMP_LOW_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_TEMP_LOW_ALARM_DEFAULT 0x0
	/*[field] TEMP_HIGH_WARNING*/
	#define SFP_DIAG_THRESHOLD_TEMP_HIGH_WARNING
	#define SFP_DIAG_THRESHOLD_TEMP_HIGH_WARNING_OFFSET  32
	#define SFP_DIAG_THRESHOLD_TEMP_HIGH_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_TEMP_HIGH_WARNING_DEFAULT 0x0
	/*[field] TEMP_LOW_WARNING*/
	#define SFP_DIAG_THRESHOLD_TEMP_LOW_WARNING
	#define SFP_DIAG_THRESHOLD_TEMP_LOW_WARNING_OFFSET  48
	#define SFP_DIAG_THRESHOLD_TEMP_LOW_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_TEMP_LOW_WARNING_DEFAULT 0x0
	/*[field] VOL_HIGH_ALARM*/
	#define SFP_DIAG_THRESHOLD_VOL_HIGH_ALARM
	#define SFP_DIAG_THRESHOLD_VOL_HIGH_ALARM_OFFSET  64
	#define SFP_DIAG_THRESHOLD_VOL_HIGH_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_VOL_HIGH_ALARM_DEFAULT 0x0
	/*[field] VOL_LOW_ALARM*/
	#define SFP_DIAG_THRESHOLD_VOL_LOW_ALARM
	#define SFP_DIAG_THRESHOLD_VOL_LOW_ALARM_OFFSET  80
	#define SFP_DIAG_THRESHOLD_VOL_LOW_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_VOL_LOW_ALARM_DEFAULT 0x0
	/*[field] VOL_HIGH_WARNING*/
	#define SFP_DIAG_THRESHOLD_VOL_HIGH_WARNING
	#define SFP_DIAG_THRESHOLD_VOL_HIGH_WARNING_OFFSET  96
	#define SFP_DIAG_THRESHOLD_VOL_HIGH_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_VOL_HIGH_WARNING_DEFAULT 0x0
	/*[field] VOL_LOW_WARNING*/
	#define SFP_DIAG_THRESHOLD_VOL_LOW_WARNING
	#define SFP_DIAG_THRESHOLD_VOL_LOW_WARNING_OFFSET  112
	#define SFP_DIAG_THRESHOLD_VOL_LOW_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_VOL_LOW_WARNING_DEFAULT 0x0
	/*[field] BIAS_HIGH_ALARM*/
	#define SFP_DIAG_THRESHOLD_BIAS_HIGH_ALARM
	#define SFP_DIAG_THRESHOLD_BIAS_HIGH_ALARM_OFFSET  128
	#define SFP_DIAG_THRESHOLD_BIAS_HIGH_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_BIAS_HIGH_ALARM_DEFAULT 0x0
	/*[field] BIAS_LOW_ALARM*/
	#define SFP_DIAG_THRESHOLD_BIAS_LOW_ALARM
	#define SFP_DIAG_THRESHOLD_BIAS_LOW_ALARM_OFFSET  144
	#define SFP_DIAG_THRESHOLD_BIAS_LOW_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_BIAS_LOW_ALARM_DEFAULT 0x0
	/*[field] BIAS_HIGH_WARNING*/
	#define SFP_DIAG_THRESHOLD_BIAS_HIGH_WARNING
	#define SFP_DIAG_THRESHOLD_BIAS_HIGH_WARNING_OFFSET  160
	#define SFP_DIAG_THRESHOLD_BIAS_HIGH_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_BIAS_HIGH_WARNING_DEFAULT 0x0
	/*[field] BIAS_LOW_WARNING*/
	#define SFP_DIAG_THRESHOLD_BIAS_LOW_WARNING
	#define SFP_DIAG_THRESHOLD_BIAS_LOW_WARNING_OFFSET  176
	#define SFP_DIAG_THRESHOLD_BIAS_LOW_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_BIAS_LOW_WARNING_DEFAULT 0x0
	/*[field] TX_PWR_HIGH_ALARM*/
	#define SFP_DIAG_THRESHOLD_TX_PWR_HIGH_ALARM
	#define SFP_DIAG_THRESHOLD_TX_PWR_HIGH_ALARM_OFFSET  192
	#define SFP_DIAG_THRESHOLD_TX_PWR_HIGH_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_TX_PWR_HIGH_ALARM_DEFAULT 0x0
	/*[field] TX_PWR_LOW_ALARM*/
	#define SFP_DIAG_THRESHOLD_TX_PWR_LOW_ALARM
	#define SFP_DIAG_THRESHOLD_TX_PWR_LOW_ALARM_OFFSET  208
	#define SFP_DIAG_THRESHOLD_TX_PWR_LOW_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_TX_PWR_LOW_ALARM_DEFAULT 0x0
	/*[field] TX_PWR_HIGH_WARNING*/
	#define SFP_DIAG_THRESHOLD_TX_PWR_HIGH_WARNING
	#define SFP_DIAG_THRESHOLD_TX_PWR_HIGH_WARNING_OFFSET  224
	#define SFP_DIAG_THRESHOLD_TX_PWR_HIGH_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_TX_PWR_HIGH_WARNING_DEFAULT 0x0
	/*[field] TX_PWR_LOW_WARNING*/
	#define SFP_DIAG_THRESHOLD_TX_PWR_LOW_WARNING
	#define SFP_DIAG_THRESHOLD_TX_PWR_LOW_WARNING_OFFSET  240
	#define SFP_DIAG_THRESHOLD_TX_PWR_LOW_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_TX_PWR_LOW_WARNING_DEFAULT 0x0
	/*[field] RX_PWR_HIGH_ALARM*/
	#define SFP_DIAG_THRESHOLD_RX_PWR_HIGH_ALARM
	#define SFP_DIAG_THRESHOLD_RX_PWR_HIGH_ALARM_OFFSET  256
	#define SFP_DIAG_THRESHOLD_RX_PWR_HIGH_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_RX_PWR_HIGH_ALARM_DEFAULT 0x0
	/*[field] RX_PWR_LOW_ALARM*/
	#define SFP_DIAG_THRESHOLD_RX_PWR_LOW_ALARM
	#define SFP_DIAG_THRESHOLD_RX_PWR_LOW_ALARM_OFFSET  272
	#define SFP_DIAG_THRESHOLD_RX_PWR_LOW_ALARM_LEN     16
	#define SFP_DIAG_THRESHOLD_RX_PWR_LOW_ALARM_DEFAULT 0x0
	/*[field] RX_PWR_HIGH_WARNING*/
	#define SFP_DIAG_THRESHOLD_RX_PWR_HIGH_WARNING
	#define SFP_DIAG_THRESHOLD_RX_PWR_HIGH_WARNING_OFFSET  288
	#define SFP_DIAG_THRESHOLD_RX_PWR_HIGH_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_RX_PWR_HIGH_WARNING_DEFAULT 0x0
	/*[field] RX_PWR_LOW_WARNING*/
	#define SFP_DIAG_THRESHOLD_RX_PWR_LOW_WARNING
	#define SFP_DIAG_THRESHOLD_RX_PWR_LOW_WARNING_OFFSET  304
	#define SFP_DIAG_THRESHOLD_RX_PWR_LOW_WARNING_LEN     16
	#define SFP_DIAG_THRESHOLD_RX_PWR_LOW_WARNING_DEFAULT 0x0

struct sfp_diag_threshold {
	a_uint8_t  temp_high_alarm_0:8;
	a_uint8_t  temp_high_alarm_1:8;
	a_uint8_t  temp_low_alarm_0:8;
	a_uint8_t  temp_low_alarm_1:8;
	a_uint8_t  temp_high_warning_0:8;
	a_uint8_t  temp_high_warning_1:8;
	a_uint8_t  temp_low_warning_0:8;
	a_uint8_t  temp_low_warning_1:8;
	a_uint8_t  vol_high_alarm_0:8;
	a_uint8_t  vol_high_alarm_1:8;
	a_uint8_t  vol_low_alarm_0:8;
	a_uint8_t  vol_low_alarm_1:8;
	a_uint8_t  vol_high_warning_0:8;
	a_uint8_t  vol_high_warning_1:8;
	a_uint8_t  vol_low_warning_0:8;
	a_uint8_t  vol_low_warning_1:8;
	a_uint8_t  bias_high_alarm_0:8;
	a_uint8_t  bias_high_alarm_1:8;
	a_uint8_t  bias_low_alarm_0:8;
	a_uint8_t  bias_low_alarm_1:8;
	a_uint8_t  bias_high_warning_0:8;
	a_uint8_t  bias_high_warning_1:8;
	a_uint8_t  bias_low_warning_0:8;
	a_uint8_t  bias_low_warning_1:8;
	a_uint8_t  tx_pwr_high_alarm_0:8;
	a_uint8_t  tx_pwr_high_alarm_1:8;
	a_uint8_t  tx_pwr_low_alarm_0:8;
	a_uint8_t  tx_pwr_low_alarm_1:8;
	a_uint8_t  tx_pwr_high_warning_0:8;
	a_uint8_t  tx_pwr_high_warning_1:8;
	a_uint8_t  tx_pwr_low_warning_0:8;
	a_uint8_t  tx_pwr_low_warning_1:8;
	a_uint8_t  rx_pwr_high_alarm_0:8;
	a_uint8_t  rx_pwr_high_alarm_1:8;
	a_uint8_t  rx_pwr_low_alarm_0:8;
	a_uint8_t  rx_pwr_low_alarm_1:8;
	a_uint8_t  rx_pwr_high_warning_0:8;
	a_uint8_t  rx_pwr_high_warning_1:8;
	a_uint8_t  rx_pwr_low_warning_0:8;
	a_uint8_t  rx_pwr_low_warning_1:8;
};

union sfp_diag_threshold_u {
	a_uint8_t val[40];
	struct sfp_diag_threshold bf;
};

/*[register] SFP_DIAG_CAL_CONST*/
#define SFP_DIAG_CAL_CONST
#define SFP_DIAG_CAL_CONST_ADDRESS 0x38
#define SFP_DIAG_CAL_CONST_NUM     1
#define SFP_DIAG_CAL_CONST_INC     0x24
#define SFP_DIAG_CAL_CONST_TYPE    REG_TYPE_RO
#define SFP_DIAG_CAL_CONST_DEFAULT 0x0
	/*[field] RX_PWR_4*/
	#define SFP_DIAG_CAL_CONST_RX_PWR_4
	#define SFP_DIAG_CAL_CONST_RX_PWR_4_OFFSET  0
	#define SFP_DIAG_CAL_CONST_RX_PWR_4_LEN     32
	#define SFP_DIAG_CAL_CONST_RX_PWR_4_DEFAULT 0x0
	/*[field] RX_PWR_3*/
	#define SFP_DIAG_CAL_CONST_RX_PWR_3
	#define SFP_DIAG_CAL_CONST_RX_PWR_3_OFFSET  32
	#define SFP_DIAG_CAL_CONST_RX_PWR_3_LEN     32
	#define SFP_DIAG_CAL_CONST_RX_PWR_3_DEFAULT 0x0
	/*[field] RX_PWR_2*/
	#define SFP_DIAG_CAL_CONST_RX_PWR_2
	#define SFP_DIAG_CAL_CONST_RX_PWR_2_OFFSET  64
	#define SFP_DIAG_CAL_CONST_RX_PWR_2_LEN     32
	#define SFP_DIAG_CAL_CONST_RX_PWR_2_DEFAULT 0x0
	/*[field] RX_PWR_1*/
	#define SFP_DIAG_CAL_CONST_RX_PWR_1
	#define SFP_DIAG_CAL_CONST_RX_PWR_1_OFFSET  96
	#define SFP_DIAG_CAL_CONST_RX_PWR_1_LEN     32
	#define SFP_DIAG_CAL_CONST_RX_PWR_1_DEFAULT 0x0
	/*[field] RX_PWR_0*/
	#define SFP_DIAG_CAL_CONST_RX_PWR_0
	#define SFP_DIAG_CAL_CONST_RX_PWR_0_OFFSET  128
	#define SFP_DIAG_CAL_CONST_RX_PWR_0_LEN     32
	#define SFP_DIAG_CAL_CONST_RX_PWR_0_DEFAULT 0x0
	/*[field] TX_I_SLOPE*/
	#define SFP_DIAG_CAL_CONST_TX_I_SLOPE
	#define SFP_DIAG_CAL_CONST_TX_I_SLOPE_OFFSET  160
	#define SFP_DIAG_CAL_CONST_TX_I_SLOPE_LEN     16
	#define SFP_DIAG_CAL_CONST_TX_I_SLOPE_DEFAULT 0x0
	/*[field] TX_I_OFFSET*/
	#define SFP_DIAG_CAL_CONST_TX_I_OFFSET
	#define SFP_DIAG_CAL_CONST_TX_I_OFFSET_OFFSET  176
	#define SFP_DIAG_CAL_CONST_TX_I_OFFSET_LEN     16
	#define SFP_DIAG_CAL_CONST_TX_I_OFFSET_DEFAULT 0x0
	/*[field] TX_PWR_SLOPE*/
	#define SFP_DIAG_CAL_CONST_TX_PWR_SLOPE
	#define SFP_DIAG_CAL_CONST_TX_PWR_SLOPE_OFFSET  192
	#define SFP_DIAG_CAL_CONST_TX_PWR_SLOPE_LEN     16
	#define SFP_DIAG_CAL_CONST_TX_PWR_SLOPE_DEFAULT 0x0
	/*[field] TX_PWR_OFFSET*/
	#define SFP_DIAG_CAL_CONST_TX_PWR_OFFSET
	#define SFP_DIAG_CAL_CONST_TX_PWR_OFFSET_OFFSET  208
	#define SFP_DIAG_CAL_CONST_TX_PWR_OFFSET_LEN     16
	#define SFP_DIAG_CAL_CONST_TX_PWR_OFFSET_DEFAULT 0x0
	/*[field] T_SLOPE*/
	#define SFP_DIAG_CAL_CONST_T_SLOPE
	#define SFP_DIAG_CAL_CONST_T_SLOPE_OFFSET  224
	#define SFP_DIAG_CAL_CONST_T_SLOPE_LEN     16
	#define SFP_DIAG_CAL_CONST_T_SLOPE_DEFAULT 0x0
	/*[field] T_OFFSET*/
	#define SFP_DIAG_CAL_CONST_T_OFFSET
	#define SFP_DIAG_CAL_CONST_T_OFFSET_OFFSET  240
	#define SFP_DIAG_CAL_CONST_T_OFFSET_LEN     16
	#define SFP_DIAG_CAL_CONST_T_OFFSET_DEFAULT 0x0
	/*[field] V_SLOPE*/
	#define SFP_DIAG_CAL_CONST_V_SLOPE
	#define SFP_DIAG_CAL_CONST_V_SLOPE_OFFSET  256
	#define SFP_DIAG_CAL_CONST_V_SLOPE_LEN     16
	#define SFP_DIAG_CAL_CONST_V_SLOPE_DEFAULT 0x0
	/*[field] V_OFFSET*/
	#define SFP_DIAG_CAL_CONST_V_OFFSET
	#define SFP_DIAG_CAL_CONST_V_OFFSET_OFFSET  272
	#define SFP_DIAG_CAL_CONST_V_OFFSET_LEN     16
	#define SFP_DIAG_CAL_CONST_V_OFFSET_DEFAULT 0x0

struct sfp_diag_cal_const {
	a_uint8_t  rx_pwr_4_0:8;
	a_uint8_t  rx_pwr_4_1:8;
	a_uint8_t  rx_pwr_4_2:8;
	a_uint8_t  rx_pwr_4_3:8;
	a_uint8_t  rx_pwr_3_0:8;
	a_uint8_t  rx_pwr_3_1:8;
	a_uint8_t  rx_pwr_3_2:8;
	a_uint8_t  rx_pwr_3_3:8;
	a_uint8_t  rx_pwr_2_0:8;
	a_uint8_t  rx_pwr_2_1:8;
	a_uint8_t  rx_pwr_2_2:8;
	a_uint8_t  rx_pwr_2_3:8;
	a_uint8_t  rx_pwr_1_0:8;
	a_uint8_t  rx_pwr_1_1:8;
	a_uint8_t  rx_pwr_1_2:8;
	a_uint8_t  rx_pwr_1_3:8;
	a_uint8_t  rx_pwr_0_0:8;
	a_uint8_t  rx_pwr_0_1:8;
	a_uint8_t  rx_pwr_0_2:8;
	a_uint8_t  rx_pwr_0_3:8;
	a_uint8_t  tx_i_slope_0:8;
	a_uint8_t  tx_i_slope_1:8;
	a_uint8_t  tx_i_offset_0:8;
	a_uint8_t  tx_i_offset_1:8;
	a_uint8_t  tx_pwr_slope_0:8;
	a_uint8_t  tx_pwr_slope_1:8;
	a_uint8_t  tx_pwr_offset_0:8;
	a_uint8_t  tx_pwr_offset_1:8;
	a_uint8_t  t_slope_0:8;
	a_uint8_t  t_slope_1:8;
	a_uint8_t  t_offset_0:8;
	a_uint8_t  t_offset_1:8;
	a_uint8_t  v_slope_0:8;
	a_uint8_t  v_slope_1:8;
	a_uint8_t  v_offset_0:8;
	a_uint8_t  v_offset_1:8;
};

union sfp_diag_cal_const_u {
	a_uint8_t val[36];
	struct sfp_diag_cal_const bf;
};

/*[register] SFP_DIAG_DMI*/
#define SFP_DIAG_DMI
#define SFP_DIAG_DMI_ADDRESS 0x5f
#define SFP_DIAG_DMI_NUM     1
#define SFP_DIAG_DMI_INC     0x1
#define SFP_DIAG_DMI_TYPE    REG_TYPE_RO
#define SFP_DIAG_DMI_DEFAULT 0x0
	/*[field] CHECK_CODE*/
	#define SFP_DIAG_DMI_CHECK_CODE
	#define SFP_DIAG_DMI_CHECK_CODE_OFFSET  0
	#define SFP_DIAG_DMI_CHECK_CODE_LEN     8
	#define SFP_DIAG_DMI_CHECK_CODE_DEFAULT 0x0

struct sfp_diag_dmi {
	a_uint8_t  check_code:8;
};

union sfp_diag_dmi_u {
	a_uint8_t val;
	struct sfp_diag_dmi bf;
};

/*[register] SFP_DIAG_REALTIME*/
#define SFP_DIAG_REALTIME
#define SFP_DIAG_REALTIME_ADDRESS 0x60
#define SFP_DIAG_REALTIME_NUM     1
#define SFP_DIAG_REALTIME_INC     0xa
#define SFP_DIAG_REALTIME_TYPE    REG_TYPE_RO
#define SFP_DIAG_REALTIME_DEFAULT 0x0
	/*[field] TMP*/
	#define SFP_DIAG_REALTIME_TMP
	#define SFP_DIAG_REALTIME_TMP_OFFSET  0
	#define SFP_DIAG_REALTIME_TMP_LEN     16
	#define SFP_DIAG_REALTIME_TMP_DEFAULT 0x0
	/*[field] VCC*/
	#define SFP_DIAG_REALTIME_VCC
	#define SFP_DIAG_REALTIME_VCC_OFFSET  16
	#define SFP_DIAG_REALTIME_VCC_LEN     16
	#define SFP_DIAG_REALTIME_VCC_DEFAULT 0x0
	/*[field] TX_BIAS*/
	#define SFP_DIAG_REALTIME_TX_BIAS
	#define SFP_DIAG_REALTIME_TX_BIAS_OFFSET  32
	#define SFP_DIAG_REALTIME_TX_BIAS_LEN     16
	#define SFP_DIAG_REALTIME_TX_BIAS_DEFAULT 0x0
	/*[field] TX_PWR*/
	#define SFP_DIAG_REALTIME_TX_PWR
	#define SFP_DIAG_REALTIME_TX_PWR_OFFSET  48
	#define SFP_DIAG_REALTIME_TX_PWR_LEN     16
	#define SFP_DIAG_REALTIME_TX_PWR_DEFAULT 0x0
	/*[field] RX_PWR*/
	#define SFP_DIAG_REALTIME_RX_PWR
	#define SFP_DIAG_REALTIME_RX_PWR_OFFSET  64
	#define SFP_DIAG_REALTIME_RX_PWR_LEN     16
	#define SFP_DIAG_REALTIME_RX_PWR_DEFAULT 0x0

struct sfp_diag_realtime {
	a_uint8_t  tmp_0:8;
	a_uint8_t  tmp_1:8;
	a_uint8_t  vcc_0:8;
	a_uint8_t  vcc_1:8;
	a_uint8_t  tx_bias_0:8;
	a_uint8_t  tx_bias_1:8;
	a_uint8_t  tx_pwr_0:8;
	a_uint8_t  tx_pwr_1:8;
	a_uint8_t  rx_pwr_0:8;
	a_uint8_t  rx_pwr_1:8;
};

union sfp_diag_realtime_u {
	a_uint8_t val[10];
	struct sfp_diag_realtime bf;
};

/*[register] SFP_DIAG_OPTIONAL_CTRL_STATUS*/
#define SFP_DIAG_OPTIONAL_CTRL_STATUS
#define SFP_DIAG_OPTIONAL_CTRL_STATUS_ADDRESS 0x6e
#define SFP_DIAG_OPTIONAL_CTRL_STATUS_NUM     1
#define SFP_DIAG_OPTIONAL_CTRL_STATUS_INC     0x2
#define SFP_DIAG_OPTIONAL_CTRL_STATUS_TYPE    REG_TYPE_RO
#define SFP_DIAG_OPTIONAL_CTRL_STATUS_DEFAULT 0x0
	/*[field] DATA_READY*/
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_DATA_READY
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_DATA_READY_OFFSET  0
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_DATA_READY_LEN     1
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_DATA_READY_DEFAULT 0x0
	/*[field] RX_LOS*/
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RX_LOS
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RX_LOS_OFFSET  1
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RX_LOS_LEN     1
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RX_LOS_DEFAULT 0x0
	/*[field] TX_FAULT*/
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_TX_FAULT
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_TX_FAULT_OFFSET  2
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_TX_FAULT_LEN     1
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_TX_FAULT_DEFAULT 0x0
	/*[field] SOFT_RATE_SEL*/
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_SOFT_RATE_SEL
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_SOFT_RATE_SEL_OFFSET  3
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_SOFT_RATE_SEL_LEN     1
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_SOFT_RATE_SEL_DEFAULT 0x0
	/*[field] RATE_SEL*/
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RATE_SEL
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RATE_SEL_OFFSET  4
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RATE_SEL_LEN     1
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RATE_SEL_DEFAULT 0x0
	/*[field] RS*/
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RS
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RS_OFFSET  5
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RS_LEN     1
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_RS_DEFAULT 0x0
	/*[field] SOFT_TX_DISABLE_SEL*/
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_SOFT_TX_DISABLE_SEL
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_SOFT_TX_DISABLE_SEL_OFFSET  6
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_SOFT_TX_DISABLE_SEL_LEN     1
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_SOFT_TX_DISABLE_SEL_DEFAULT 0x0
	/*[field] TX_DISABLE*/
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_TX_DISABLE
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_TX_DISABLE_OFFSET  7
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_TX_DISABLE_LEN     1
	#define SFP_DIAG_OPTIONAL_CTRL_STATUS_TX_DISABLE_DEFAULT 0x0

struct sfp_diag_optional_ctrl_status {
	a_uint8_t  data_ready:1;
	a_uint8_t  rx_los:1;
	a_uint8_t  tx_fault:1;
	a_uint8_t  soft_rate_sel:1;
	a_uint8_t  rate_sel:1;
	a_uint8_t  rs:1;
	a_uint8_t  soft_tx_disable_sel:1;
	a_uint8_t  tx_disable:1;
	a_uint8_t  _reserved0:8;
};

union sfp_diag_optional_ctrl_status_u {
	a_uint8_t val[2];
	struct sfp_diag_optional_ctrl_status bf;
};

/*[register] SFP_DIAG_FLAG*/
#define SFP_DIAG_FLAG
#define SFP_DIAG_FLAG_ADDRESS 0x70
#define SFP_DIAG_FLAG_NUM     1
#define SFP_DIAG_FLAG_INC     0x6
#define SFP_DIAG_FLAG_TYPE    REG_TYPE_RO
#define SFP_DIAG_FLAG_DEFAULT 0x0
	/*[field] TX_PWR_LOW_ALARM*/
	#define SFP_DIAG_FLAG_TX_PWR_LOW_ALARM
	#define SFP_DIAG_FLAG_TX_PWR_LOW_ALARM_OFFSET  0
	#define SFP_DIAG_FLAG_TX_PWR_LOW_ALARM_LEN     1
	#define SFP_DIAG_FLAG_TX_PWR_LOW_ALARM_DEFAULT 0x0
	/*[field] TX_PWR_HIGH_ALARM*/
	#define SFP_DIAG_FLAG_TX_PWR_HIGH_ALARM
	#define SFP_DIAG_FLAG_TX_PWR_HIGH_ALARM_OFFSET  1
	#define SFP_DIAG_FLAG_TX_PWR_HIGH_ALARM_LEN     1
	#define SFP_DIAG_FLAG_TX_PWR_HIGH_ALARM_DEFAULT 0x0
	/*[field] TX_BIAS_LOW_ALARM*/
	#define SFP_DIAG_FLAG_TX_BIAS_LOW_ALARM
	#define SFP_DIAG_FLAG_TX_BIAS_LOW_ALARM_OFFSET  2
	#define SFP_DIAG_FLAG_TX_BIAS_LOW_ALARM_LEN     1
	#define SFP_DIAG_FLAG_TX_BIAS_LOW_ALARM_DEFAULT 0x0
	/*[field] TX_BIAS_HIGH_ALARM*/
	#define SFP_DIAG_FLAG_TX_BIAS_HIGH_ALARM
	#define SFP_DIAG_FLAG_TX_BIAS_HIGH_ALARM_OFFSET  3
	#define SFP_DIAG_FLAG_TX_BIAS_HIGH_ALARM_LEN     1
	#define SFP_DIAG_FLAG_TX_BIAS_HIGH_ALARM_DEFAULT 0x0
	/*[field] VCC_LOW_ALARM*/
	#define SFP_DIAG_FLAG_VCC_LOW_ALARM
	#define SFP_DIAG_FLAG_VCC_LOW_ALARM_OFFSET  4
	#define SFP_DIAG_FLAG_VCC_LOW_ALARM_LEN     1
	#define SFP_DIAG_FLAG_VCC_LOW_ALARM_DEFAULT 0x0
	/*[field] VCC_HIGH_ALARM*/
	#define SFP_DIAG_FLAG_VCC_HIGH_ALARM
	#define SFP_DIAG_FLAG_VCC_HIGH_ALARM_OFFSET  5
	#define SFP_DIAG_FLAG_VCC_HIGH_ALARM_LEN     1
	#define SFP_DIAG_FLAG_VCC_HIGH_ALARM_DEFAULT 0x0
	/*[field] TMP_LOW_ALARM*/
	#define SFP_DIAG_FLAG_TMP_LOW_ALARM
	#define SFP_DIAG_FLAG_TMP_LOW_ALARM_OFFSET  6
	#define SFP_DIAG_FLAG_TMP_LOW_ALARM_LEN     1
	#define SFP_DIAG_FLAG_TMP_LOW_ALARM_DEFAULT 0x0
	/*[field] TMP_HIGH_ALARM*/
	#define SFP_DIAG_FLAG_TMP_HIGH_ALARM
	#define SFP_DIAG_FLAG_TMP_HIGH_ALARM_OFFSET  7
	#define SFP_DIAG_FLAG_TMP_HIGH_ALARM_LEN     1
	#define SFP_DIAG_FLAG_TMP_HIGH_ALARM_DEFAULT 0x0
	/*[field] RX_PWR_LOW_ALARM*/
	#define SFP_DIAG_FLAG_RX_PWR_LOW_ALARM
	#define SFP_DIAG_FLAG_RX_PWR_LOW_ALARM_OFFSET  14
	#define SFP_DIAG_FLAG_RX_PWR_LOW_ALARM_LEN     1
	#define SFP_DIAG_FLAG_RX_PWR_LOW_ALARM_DEFAULT 0x0
	/*[field] RX_PWR_HIGH_ALARM*/
	#define SFP_DIAG_FLAG_RX_PWR_HIGH_ALARM
	#define SFP_DIAG_FLAG_RX_PWR_HIGH_ALARM_OFFSET  15
	#define SFP_DIAG_FLAG_RX_PWR_HIGH_ALARM_LEN     1
	#define SFP_DIAG_FLAG_RX_PWR_HIGH_ALARM_DEFAULT 0x0
	/*[field] UNALLOCATED*/
	#define SFP_DIAG_FLAG_UNALLOCATED
	#define SFP_DIAG_FLAG_UNALLOCATED_OFFSET  16
	#define SFP_DIAG_FLAG_UNALLOCATED_LEN     16
	#define SFP_DIAG_FLAG_UNALLOCATED_DEFAULT 0x0
	/*[field] TX_PWR_LOW_WARNING*/
	#define SFP_DIAG_FLAG_TX_PWR_LOW_WARNING
	#define SFP_DIAG_FLAG_TX_PWR_LOW_WARNING_OFFSET  32
	#define SFP_DIAG_FLAG_TX_PWR_LOW_WARNING_LEN     1
	#define SFP_DIAG_FLAG_TX_PWR_LOW_WARNING_DEFAULT 0x0
	/*[field] TX_PWR_HIGH_WARNING*/
	#define SFP_DIAG_FLAG_TX_PWR_HIGH_WARNING
	#define SFP_DIAG_FLAG_TX_PWR_HIGH_WARNING_OFFSET  33
	#define SFP_DIAG_FLAG_TX_PWR_HIGH_WARNING_LEN     1
	#define SFP_DIAG_FLAG_TX_PWR_HIGH_WARNING_DEFAULT 0x0
	/*[field] TX_BIAS_LOW_WARNING*/
	#define SFP_DIAG_FLAG_TX_BIAS_LOW_WARNING
	#define SFP_DIAG_FLAG_TX_BIAS_LOW_WARNING_OFFSET  34
	#define SFP_DIAG_FLAG_TX_BIAS_LOW_WARNING_LEN     1
	#define SFP_DIAG_FLAG_TX_BIAS_LOW_WARNING_DEFAULT 0x0
	/*[field] TX_BIAS_HIGH_WARNING*/
	#define SFP_DIAG_FLAG_TX_BIAS_HIGH_WARNING
	#define SFP_DIAG_FLAG_TX_BIAS_HIGH_WARNING_OFFSET  35
	#define SFP_DIAG_FLAG_TX_BIAS_HIGH_WARNING_LEN     1
	#define SFP_DIAG_FLAG_TX_BIAS_HIGH_WARNING_DEFAULT 0x0
	/*[field] VCC_LOW_WARNING*/
	#define SFP_DIAG_FLAG_VCC_LOW_WARNING
	#define SFP_DIAG_FLAG_VCC_LOW_WARNING_OFFSET  36
	#define SFP_DIAG_FLAG_VCC_LOW_WARNING_LEN     1
	#define SFP_DIAG_FLAG_VCC_LOW_WARNING_DEFAULT 0x0
	/*[field] VCC_HIGH_WARNING*/
	#define SFP_DIAG_FLAG_VCC_HIGH_WARNING
	#define SFP_DIAG_FLAG_VCC_HIGH_WARNING_OFFSET  37
	#define SFP_DIAG_FLAG_VCC_HIGH_WARNING_LEN     1
	#define SFP_DIAG_FLAG_VCC_HIGH_WARNING_DEFAULT 0x0
	/*[field] TMP_LOW_WARNING*/
	#define SFP_DIAG_FLAG_TMP_LOW_WARNING
	#define SFP_DIAG_FLAG_TMP_LOW_WARNING_OFFSET  38
	#define SFP_DIAG_FLAG_TMP_LOW_WARNING_LEN     1
	#define SFP_DIAG_FLAG_TMP_LOW_WARNING_DEFAULT 0x0
	/*[field] TMP_HIGH_WARNING*/
	#define SFP_DIAG_FLAG_TMP_HIGH_WARNING
	#define SFP_DIAG_FLAG_TMP_HIGH_WARNING_OFFSET  39
	#define SFP_DIAG_FLAG_TMP_HIGH_WARNING_LEN     1
	#define SFP_DIAG_FLAG_TMP_HIGH_WARNING_DEFAULT 0x0
	/*[field] RX_PWR_LOW_WARNING*/
	#define SFP_DIAG_FLAG_RX_PWR_LOW_WARNING
	#define SFP_DIAG_FLAG_RX_PWR_LOW_WARNING_OFFSET  46
	#define SFP_DIAG_FLAG_RX_PWR_LOW_WARNING_LEN     1
	#define SFP_DIAG_FLAG_RX_PWR_LOW_WARNING_DEFAULT 0x0
	/*[field] RX_PWR_HIGH_WARNING*/
	#define SFP_DIAG_FLAG_RX_PWR_HIGH_WARNING
	#define SFP_DIAG_FLAG_RX_PWR_HIGH_WARNING_OFFSET  47
	#define SFP_DIAG_FLAG_RX_PWR_HIGH_WARNING_LEN     1
	#define SFP_DIAG_FLAG_RX_PWR_HIGH_WARNING_DEFAULT 0x0

struct sfp_diag_flag {
	a_uint8_t  tx_pwr_low_alarm:1;
	a_uint8_t  tx_pwr_high_alarm:1;
	a_uint8_t  tx_bias_low_alarm:1;
	a_uint8_t  tx_bias_high_alarm:1;
	a_uint8_t  vcc_low_alarm:1;
	a_uint8_t  vcc_high_alarm:1;
	a_uint8_t  tmp_low_alarm:1;
	a_uint8_t  tmp_high_alarm:1;
	a_uint8_t  _reserved0:6;
	a_uint8_t  rx_pwr_low_alarm:1;
	a_uint8_t  rx_pwr_high_alarm:1;
	a_uint8_t  unallocated_0:8;
	a_uint8_t  unallocated_1:8;
	a_uint8_t  tx_pwr_low_warning:1;
	a_uint8_t  tx_pwr_high_warning:1;
	a_uint8_t  tx_bias_low_warning:1;
	a_uint8_t  tx_bias_high_warning:1;
	a_uint8_t  vcc_low_warning:1;
	a_uint8_t  vcc_high_warning:1;
	a_uint8_t  tmp_low_warning:1;
	a_uint8_t  tmp_high_warning:1;
	a_uint8_t  _reserved1:6;
	a_uint8_t  rx_pwr_low_warning:1;
	a_uint8_t  rx_pwr_high_warning:1;
};

union sfp_diag_flag_u {
	a_uint8_t val[6];
	struct sfp_diag_flag bf;
};

/*[register] SFP_DIAG_EXTENDED_CTRL_STATUS*/
#define SFP_DIAG_EXTENDED_CTRL_STATUS
#define SFP_DIAG_EXTENDED_CTRL_STATUS_ADDRESS 0x76
#define SFP_DIAG_EXTENDED_CTRL_STATUS_NUM     1
#define SFP_DIAG_EXTENDED_CTRL_STATUS_INC     0x2
#define SFP_DIAG_EXTENDED_CTRL_STATUS_TYPE    REG_TYPE_RO
#define SFP_DIAG_EXTENDED_CTRL_STATUS_DEFAULT 0x0
	/*[field] PWR_LEVEL_SEL*/
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_PWR_LEVEL_SEL
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_PWR_LEVEL_SEL_OFFSET  0
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_PWR_LEVEL_SEL_LEN     1
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_PWR_LEVEL_SEL_DEFAULT 0x0
	/*[field] PWR_LEVEL_OP_STATE*/
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_PWR_LEVEL_OP_STATE
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_PWR_LEVEL_OP_STATE_OFFSET  1
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_PWR_LEVEL_OP_STATE_LEN     1
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_PWR_LEVEL_OP_STATE_DEFAULT 0x0
	/*[field] SOFT_RS_SEL*/
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_SOFT_RS_SEL
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_SOFT_RS_SEL_OFFSET  3
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_SOFT_RS_SEL_LEN     1
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_SOFT_RS_SEL_DEFAULT 0x0
	/*[field] UNALLOCATED*/
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_UNALLOCATED
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_UNALLOCATED_OFFSET  4
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_UNALLOCATED_LEN     12
	#define SFP_DIAG_EXTENDED_CTRL_STATUS_UNALLOCATED_DEFAULT 0x0

struct sfp_diag_extended_ctrl_status {
	a_uint8_t  pwr_level_sel:1;
	a_uint8_t  pwr_level_op_state:1;
	a_uint8_t  _reserved0:1;
	a_uint8_t  soft_rs_sel:1;
	a_uint8_t  unallocated_0:4;
	a_uint8_t  unallocated_1:8;
};

union sfp_diag_extended_ctrl_status_u {
	a_uint8_t val[2];
	struct sfp_diag_extended_ctrl_status bf;
};

#endif
