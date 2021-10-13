/*
 * Copyright (c) 2016-2017, 2019, The Linux Foundation. All rights reserved.
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
#ifndef _HPPE_IP_REG_H_
#define _HPPE_IP_REG_H_

/*[table] MY_MAC_TBL*/
#define MY_MAC_TBL
#define MY_MAC_TBL_ADDRESS 0x0
#define MY_MAC_TBL_NUM     8
#define MY_MAC_TBL_INC     0x8
#define MY_MAC_TBL_TYPE    REG_TYPE_RW
#define MY_MAC_TBL_DEFAULT 0x0
	/*[field] MAC_DA*/
	#define MY_MAC_TBL_MAC_DA
	#define MY_MAC_TBL_MAC_DA_OFFSET  0
	#define MY_MAC_TBL_MAC_DA_LEN     48
	#define MY_MAC_TBL_MAC_DA_DEFAULT 0x0
	/*[field] VALID*/
	#define MY_MAC_TBL_VALID
	#define MY_MAC_TBL_VALID_OFFSET  48
	#define MY_MAC_TBL_VALID_LEN     1
	#define MY_MAC_TBL_VALID_DEFAULT 0x0

struct my_mac_tbl {
	a_uint32_t  mac_da_0:32;
	a_uint32_t  mac_da_1:16;
	a_uint32_t  valid:1;
	a_uint32_t  _reserved0:15;
};

union my_mac_tbl_u {
	a_uint32_t val[2];
	struct my_mac_tbl bf;
};

/*[register] L3_VSI*/
#define L3_VSI
#define L3_VSI_ADDRESS 0x40
#define L3_VSI_NUM     32
#define L3_VSI_INC     0x4
#define L3_VSI_TYPE    REG_TYPE_RW
#define L3_VSI_DEFAULT 0x0
	/*[field] L3_IF_VALID*/
	#define L3_VSI_L3_IF_VALID
	#define L3_VSI_L3_IF_VALID_OFFSET  0
	#define L3_VSI_L3_IF_VALID_LEN     1
	#define L3_VSI_L3_IF_VALID_DEFAULT 0x0
	/*[field] L3_IF_INDEX*/
	#define L3_VSI_L3_IF_INDEX
	#define L3_VSI_L3_IF_INDEX_OFFSET  1
	#define L3_VSI_L3_IF_INDEX_LEN     8
	#define L3_VSI_L3_IF_INDEX_DEFAULT 0x0
	/*[field] L2_IPV4_MC_EN*/
	#define L3_VSI_L2_IPV4_MC_EN
	#define L3_VSI_L2_IPV4_MC_EN_OFFSET  9
	#define L3_VSI_L2_IPV4_MC_EN_LEN     1
	#define L3_VSI_L2_IPV4_MC_EN_DEFAULT 0x0
	/*[field] L2_IPV4_MC_MODE*/
	#define L3_VSI_L2_IPV4_MC_MODE
	#define L3_VSI_L2_IPV4_MC_MODE_OFFSET  10
	#define L3_VSI_L2_IPV4_MC_MODE_LEN     1
	#define L3_VSI_L2_IPV4_MC_MODE_DEFAULT 0x0
	/*[field] L2_IPV6_MC_EN*/
	#define L3_VSI_L2_IPV6_MC_EN
	#define L3_VSI_L2_IPV6_MC_EN_OFFSET  11
	#define L3_VSI_L2_IPV6_MC_EN_LEN     1
	#define L3_VSI_L2_IPV6_MC_EN_DEFAULT 0x0
	/*[field] L2_IPV6_MC_MODE*/
	#define L3_VSI_L2_IPV6_MC_MODE
	#define L3_VSI_L2_IPV6_MC_MODE_OFFSET  12
	#define L3_VSI_L2_IPV6_MC_MODE_LEN     1
	#define L3_VSI_L2_IPV6_MC_MODE_DEFAULT 0x0

struct l3_vsi {
	a_uint32_t  l3_if_valid:1;
	a_uint32_t  l3_if_index:8;
	a_uint32_t  l2_ipv4_mc_en:1;
	a_uint32_t  l2_ipv4_mc_mode:1;
	a_uint32_t  l2_ipv6_mc_en:1;
	a_uint32_t  l2_ipv6_mc_mode:1;
	a_uint32_t  _reserved0:19;
};

union l3_vsi_u {
	a_uint32_t val;
	struct l3_vsi bf;
};

/*[register] L3_VSI_EXT*/
#define L3_VSI_EXT
#define L3_VSI_EXT_ADDRESS 0xc0
#define L3_VSI_EXT_NUM     32
#define L3_VSI_EXT_INC     0x4
#define L3_VSI_EXT_TYPE    REG_TYPE_RW
#define L3_VSI_EXT_DEFAULT 0x0
	/*[field] IPV4_SG_EN*/
	#define L3_VSI_EXT_IPV4_SG_EN
	#define L3_VSI_EXT_IPV4_SG_EN_OFFSET  0
	#define L3_VSI_EXT_IPV4_SG_EN_LEN     1
	#define L3_VSI_EXT_IPV4_SG_EN_DEFAULT 0x0
	/*[field] IPV4_SG_VIO_CMD*/
	#define L3_VSI_EXT_IPV4_SG_VIO_CMD
	#define L3_VSI_EXT_IPV4_SG_VIO_CMD_OFFSET  1
	#define L3_VSI_EXT_IPV4_SG_VIO_CMD_LEN     2
	#define L3_VSI_EXT_IPV4_SG_VIO_CMD_DEFAULT 0x0
	/*[field] IPV4_SG_PORT_EN*/
	#define L3_VSI_EXT_IPV4_SG_PORT_EN
	#define L3_VSI_EXT_IPV4_SG_PORT_EN_OFFSET  3
	#define L3_VSI_EXT_IPV4_SG_PORT_EN_LEN     1
	#define L3_VSI_EXT_IPV4_SG_PORT_EN_DEFAULT 0x0
	/*[field] IPV4_SG_SVLAN_EN*/
	#define L3_VSI_EXT_IPV4_SG_SVLAN_EN
	#define L3_VSI_EXT_IPV4_SG_SVLAN_EN_OFFSET  4
	#define L3_VSI_EXT_IPV4_SG_SVLAN_EN_LEN     1
	#define L3_VSI_EXT_IPV4_SG_SVLAN_EN_DEFAULT 0x0
	/*[field] IPV4_SG_CVLAN_EN*/
	#define L3_VSI_EXT_IPV4_SG_CVLAN_EN
	#define L3_VSI_EXT_IPV4_SG_CVLAN_EN_OFFSET  5
	#define L3_VSI_EXT_IPV4_SG_CVLAN_EN_LEN     1
	#define L3_VSI_EXT_IPV4_SG_CVLAN_EN_DEFAULT 0x0
	/*[field] IPV4_SRC_UNK_CMD*/
	#define L3_VSI_EXT_IPV4_SRC_UNK_CMD
	#define L3_VSI_EXT_IPV4_SRC_UNK_CMD_OFFSET  6
	#define L3_VSI_EXT_IPV4_SRC_UNK_CMD_LEN     2
	#define L3_VSI_EXT_IPV4_SRC_UNK_CMD_DEFAULT 0x0
	/*[field] IPV6_SG_EN*/
	#define L3_VSI_EXT_IPV6_SG_EN
	#define L3_VSI_EXT_IPV6_SG_EN_OFFSET  8
	#define L3_VSI_EXT_IPV6_SG_EN_LEN     1
	#define L3_VSI_EXT_IPV6_SG_EN_DEFAULT 0x0
	/*[field] IPV6_SG_VIO_CMD*/
	#define L3_VSI_EXT_IPV6_SG_VIO_CMD
	#define L3_VSI_EXT_IPV6_SG_VIO_CMD_OFFSET  9
	#define L3_VSI_EXT_IPV6_SG_VIO_CMD_LEN     2
	#define L3_VSI_EXT_IPV6_SG_VIO_CMD_DEFAULT 0x0
	/*[field] IPV6_SG_PORT_EN*/
	#define L3_VSI_EXT_IPV6_SG_PORT_EN
	#define L3_VSI_EXT_IPV6_SG_PORT_EN_OFFSET  11
	#define L3_VSI_EXT_IPV6_SG_PORT_EN_LEN     1
	#define L3_VSI_EXT_IPV6_SG_PORT_EN_DEFAULT 0x0
	/*[field] IPV6_SG_SVLAN_EN*/
	#define L3_VSI_EXT_IPV6_SG_SVLAN_EN
	#define L3_VSI_EXT_IPV6_SG_SVLAN_EN_OFFSET  12
	#define L3_VSI_EXT_IPV6_SG_SVLAN_EN_LEN     1
	#define L3_VSI_EXT_IPV6_SG_SVLAN_EN_DEFAULT 0x0
	/*[field] IPV6_SG_CVLAN_EN*/
	#define L3_VSI_EXT_IPV6_SG_CVLAN_EN
	#define L3_VSI_EXT_IPV6_SG_CVLAN_EN_OFFSET  13
	#define L3_VSI_EXT_IPV6_SG_CVLAN_EN_LEN     1
	#define L3_VSI_EXT_IPV6_SG_CVLAN_EN_DEFAULT 0x0
	/*[field] IPV6_SRC_UNK_CMD*/
	#define L3_VSI_EXT_IPV6_SRC_UNK_CMD
	#define L3_VSI_EXT_IPV6_SRC_UNK_CMD_OFFSET  14
	#define L3_VSI_EXT_IPV6_SRC_UNK_CMD_LEN     2
	#define L3_VSI_EXT_IPV6_SRC_UNK_CMD_DEFAULT 0x0
	/*[field] IP_ARP_SG_EN*/
	#define L3_VSI_EXT_IP_ARP_SG_EN
	#define L3_VSI_EXT_IP_ARP_SG_EN_OFFSET  16
	#define L3_VSI_EXT_IP_ARP_SG_EN_LEN     1
	#define L3_VSI_EXT_IP_ARP_SG_EN_DEFAULT 0x0
	/*[field] IP_ARP_SG_VIO_CMD*/
	#define L3_VSI_EXT_IP_ARP_SG_VIO_CMD
	#define L3_VSI_EXT_IP_ARP_SG_VIO_CMD_OFFSET  17
	#define L3_VSI_EXT_IP_ARP_SG_VIO_CMD_LEN     2
	#define L3_VSI_EXT_IP_ARP_SG_VIO_CMD_DEFAULT 0x0
	/*[field] IP_ARP_SG_PORT_EN*/
	#define L3_VSI_EXT_IP_ARP_SG_PORT_EN
	#define L3_VSI_EXT_IP_ARP_SG_PORT_EN_OFFSET  19
	#define L3_VSI_EXT_IP_ARP_SG_PORT_EN_LEN     1
	#define L3_VSI_EXT_IP_ARP_SG_PORT_EN_DEFAULT 0x0
	/*[field] IP_ARP_SG_SVLAN_EN*/
	#define L3_VSI_EXT_IP_ARP_SG_SVLAN_EN
	#define L3_VSI_EXT_IP_ARP_SG_SVLAN_EN_OFFSET  20
	#define L3_VSI_EXT_IP_ARP_SG_SVLAN_EN_LEN     1
	#define L3_VSI_EXT_IP_ARP_SG_SVLAN_EN_DEFAULT 0x0
	/*[field] IP_ARP_SG_CVLAN_EN*/
	#define L3_VSI_EXT_IP_ARP_SG_CVLAN_EN
	#define L3_VSI_EXT_IP_ARP_SG_CVLAN_EN_OFFSET  21
	#define L3_VSI_EXT_IP_ARP_SG_CVLAN_EN_LEN     1
	#define L3_VSI_EXT_IP_ARP_SG_CVLAN_EN_DEFAULT 0x0
	/*[field] IP_ARP_SRC_UNK_CMD*/
	#define L3_VSI_EXT_IP_ARP_SRC_UNK_CMD
	#define L3_VSI_EXT_IP_ARP_SRC_UNK_CMD_OFFSET  22
	#define L3_VSI_EXT_IP_ARP_SRC_UNK_CMD_LEN     2
	#define L3_VSI_EXT_IP_ARP_SRC_UNK_CMD_DEFAULT 0x0
	/*[field] IP_ND_SG_EN*/
	#define L3_VSI_EXT_IP_ND_SG_EN
	#define L3_VSI_EXT_IP_ND_SG_EN_OFFSET  24
	#define L3_VSI_EXT_IP_ND_SG_EN_LEN     1
	#define L3_VSI_EXT_IP_ND_SG_EN_DEFAULT 0x0
	/*[field] IP_ND_SG_VIO_CMD*/
	#define L3_VSI_EXT_IP_ND_SG_VIO_CMD
	#define L3_VSI_EXT_IP_ND_SG_VIO_CMD_OFFSET  25
	#define L3_VSI_EXT_IP_ND_SG_VIO_CMD_LEN     2
	#define L3_VSI_EXT_IP_ND_SG_VIO_CMD_DEFAULT 0x0
	/*[field] IP_ND_SG_PORT_EN*/
	#define L3_VSI_EXT_IP_ND_SG_PORT_EN
	#define L3_VSI_EXT_IP_ND_SG_PORT_EN_OFFSET  27
	#define L3_VSI_EXT_IP_ND_SG_PORT_EN_LEN     1
	#define L3_VSI_EXT_IP_ND_SG_PORT_EN_DEFAULT 0x0
	/*[field] IP_ND_SG_SVLAN_EN*/
	#define L3_VSI_EXT_IP_ND_SG_SVLAN_EN
	#define L3_VSI_EXT_IP_ND_SG_SVLAN_EN_OFFSET  28
	#define L3_VSI_EXT_IP_ND_SG_SVLAN_EN_LEN     1
	#define L3_VSI_EXT_IP_ND_SG_SVLAN_EN_DEFAULT 0x0
	/*[field] IP_ND_SG_CVLAN_EN*/
	#define L3_VSI_EXT_IP_ND_SG_CVLAN_EN
	#define L3_VSI_EXT_IP_ND_SG_CVLAN_EN_OFFSET  29
	#define L3_VSI_EXT_IP_ND_SG_CVLAN_EN_LEN     1
	#define L3_VSI_EXT_IP_ND_SG_CVLAN_EN_DEFAULT 0x0
	/*[field] IP_ND_SRC_UNK_CMD*/
	#define L3_VSI_EXT_IP_ND_SRC_UNK_CMD
	#define L3_VSI_EXT_IP_ND_SRC_UNK_CMD_OFFSET  30
	#define L3_VSI_EXT_IP_ND_SRC_UNK_CMD_LEN     2
	#define L3_VSI_EXT_IP_ND_SRC_UNK_CMD_DEFAULT 0x0

struct l3_vsi_ext {
	a_uint32_t  ipv4_sg_en:1;
	a_uint32_t  ipv4_sg_vio_cmd:2;
	a_uint32_t  ipv4_sg_port_en:1;
	a_uint32_t  ipv4_sg_svlan_en:1;
	a_uint32_t  ipv4_sg_cvlan_en:1;
	a_uint32_t  ipv4_src_unk_cmd:2;
	a_uint32_t  ipv6_sg_en:1;
	a_uint32_t  ipv6_sg_vio_cmd:2;
	a_uint32_t  ipv6_sg_port_en:1;
	a_uint32_t  ipv6_sg_svlan_en:1;
	a_uint32_t  ipv6_sg_cvlan_en:1;
	a_uint32_t  ipv6_src_unk_cmd:2;
	a_uint32_t  ip_arp_sg_en:1;
	a_uint32_t  ip_arp_sg_vio_cmd:2;
	a_uint32_t  ip_arp_sg_port_en:1;
	a_uint32_t  ip_arp_sg_svlan_en:1;
	a_uint32_t  ip_arp_sg_cvlan_en:1;
	a_uint32_t  ip_arp_src_unk_cmd:2;
	a_uint32_t  ip_nd_sg_en:1;
	a_uint32_t  ip_nd_sg_vio_cmd:2;
	a_uint32_t  ip_nd_sg_port_en:1;
	a_uint32_t  ip_nd_sg_svlan_en:1;
	a_uint32_t  ip_nd_sg_cvlan_en:1;
	a_uint32_t  ip_nd_src_unk_cmd:2;
};

union l3_vsi_ext_u {
	a_uint32_t val;
	struct l3_vsi_ext bf;
};

/*[register] NETWORK_ROUTE_IP*/
#define NETWORK_ROUTE_IP
#define NETWORK_ROUTE_IP_ADDRESS 0x140
#define NETWORK_ROUTE_IP_NUM     32
#define NETWORK_ROUTE_IP_INC     0x8
#define NETWORK_ROUTE_IP_TYPE    REG_TYPE_RW
#define NETWORK_ROUTE_IP_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define NETWORK_ROUTE_IP_IP_ADDR
	#define NETWORK_ROUTE_IP_IP_ADDR_OFFSET  0
	#define NETWORK_ROUTE_IP_IP_ADDR_LEN     32
	#define NETWORK_ROUTE_IP_IP_ADDR_DEFAULT 0x0
	/*[field] IP_ADDR_MASK*/
	#define NETWORK_ROUTE_IP_IP_ADDR_MASK
	#define NETWORK_ROUTE_IP_IP_ADDR_MASK_OFFSET  32
	#define NETWORK_ROUTE_IP_IP_ADDR_MASK_LEN     32
	#define NETWORK_ROUTE_IP_IP_ADDR_MASK_DEFAULT 0x0

struct network_route_ip {
	a_uint32_t  ip_addr:32;
	a_uint32_t  ip_addr_mask:32;
};

union network_route_ip_u {
	a_uint32_t val[2];
	struct network_route_ip bf;
};

/*[register] NETWORK_ROUTE_IP_EXT*/
#define NETWORK_ROUTE_IP_EXT
#define NETWORK_ROUTE_IP_EXT_ADDRESS 0x240
#define NETWORK_ROUTE_IP_EXT_NUM     32
#define NETWORK_ROUTE_IP_EXT_INC     0x4
#define NETWORK_ROUTE_IP_EXT_TYPE    REG_TYPE_RW
#define NETWORK_ROUTE_IP_EXT_DEFAULT 0x0
	/*[field] VALID*/
	#define NETWORK_ROUTE_IP_EXT_VALID
	#define NETWORK_ROUTE_IP_EXT_VALID_OFFSET  0
	#define NETWORK_ROUTE_IP_EXT_VALID_LEN     1
	#define NETWORK_ROUTE_IP_EXT_VALID_DEFAULT 0x0
	/*[field] ENTRY_TYPE*/
	#define NETWORK_ROUTE_IP_EXT_ENTRY_TYPE
	#define NETWORK_ROUTE_IP_EXT_ENTRY_TYPE_OFFSET  1
	#define NETWORK_ROUTE_IP_EXT_ENTRY_TYPE_LEN     1
	#define NETWORK_ROUTE_IP_EXT_ENTRY_TYPE_DEFAULT 0x0

struct network_route_ip_ext {
	a_uint32_t  valid:1;
	a_uint32_t  entry_type:1;
	a_uint32_t  _reserved0:30;
};

union network_route_ip_ext_u {
	a_uint32_t val;
	struct network_route_ip_ext bf;
};

/*[register] NETWORK_ROUTE_ACTION*/
#define NETWORK_ROUTE_ACTION
#define NETWORK_ROUTE_ACTION_ADDRESS 0x2c0
#define NETWORK_ROUTE_ACTION_NUM     32
#define NETWORK_ROUTE_ACTION_INC     0x4
#define NETWORK_ROUTE_ACTION_TYPE    REG_TYPE_RW
#define NETWORK_ROUTE_ACTION_DEFAULT 0x0
	/*[field] FWD_CMD*/
	#define NETWORK_ROUTE_ACTION_FWD_CMD
	#define NETWORK_ROUTE_ACTION_FWD_CMD_OFFSET  0
	#define NETWORK_ROUTE_ACTION_FWD_CMD_LEN     2
	#define NETWORK_ROUTE_ACTION_FWD_CMD_DEFAULT 0x0
	/*[field] DST_INFO*/
	#define NETWORK_ROUTE_ACTION_DST_INFO
	#define NETWORK_ROUTE_ACTION_DST_INFO_OFFSET  2
	#define NETWORK_ROUTE_ACTION_DST_INFO_LEN     14
	#define NETWORK_ROUTE_ACTION_DST_INFO_DEFAULT 0x0
	/*[field] LAN_WAN*/
	#define NETWORK_ROUTE_ACTION_LAN_WAN
	#define NETWORK_ROUTE_ACTION_LAN_WAN_OFFSET  16
	#define NETWORK_ROUTE_ACTION_LAN_WAN_LEN     1
	#define NETWORK_ROUTE_ACTION_LAN_WAN_DEFAULT 0x0

struct network_route_action {
	a_uint32_t  fwd_cmd:2;
	a_uint32_t  dst_info:14;
	a_uint32_t  lan_wan:1;
	a_uint32_t  _reserved0:15;
};

union network_route_action_u {
	a_uint32_t val;
	struct network_route_action bf;
};

/*[register] L3_ROUTE_CTRL*/
#define L3_ROUTE_CTRL
#define L3_ROUTE_CTRL_ADDRESS 0x340
#define L3_ROUTE_CTRL_NUM     1
#define L3_ROUTE_CTRL_INC     0x4
#define L3_ROUTE_CTRL_TYPE    REG_TYPE_RW
#define L3_ROUTE_CTRL_DEFAULT 0xdbc36db
	/*[field] IP_MRU_CHECK_FAIL*/
	#define L3_ROUTE_CTRL_IP_MRU_CHECK_FAIL
	#define L3_ROUTE_CTRL_IP_MRU_CHECK_FAIL_OFFSET  0
	#define L3_ROUTE_CTRL_IP_MRU_CHECK_FAIL_LEN     2
	#define L3_ROUTE_CTRL_IP_MRU_CHECK_FAIL_DEFAULT 0x3
	/*[field] IP_MRU_CHECK_FAIL_DE_ACCE*/
	#define L3_ROUTE_CTRL_IP_MRU_CHECK_FAIL_DE_ACCE
	#define L3_ROUTE_CTRL_IP_MRU_CHECK_FAIL_DE_ACCE_OFFSET  2
	#define L3_ROUTE_CTRL_IP_MRU_CHECK_FAIL_DE_ACCE_LEN     1
	#define L3_ROUTE_CTRL_IP_MRU_CHECK_FAIL_DE_ACCE_DEFAULT 0x0
	/*[field] IP_MTU_FAIL*/
	#define L3_ROUTE_CTRL_IP_MTU_FAIL
	#define L3_ROUTE_CTRL_IP_MTU_FAIL_OFFSET  3
	#define L3_ROUTE_CTRL_IP_MTU_FAIL_LEN     2
	#define L3_ROUTE_CTRL_IP_MTU_FAIL_DEFAULT 0x3
	/*[field] IP_MTU_FAIL_DE_ACCE*/
	#define L3_ROUTE_CTRL_IP_MTU_FAIL_DE_ACCE
	#define L3_ROUTE_CTRL_IP_MTU_FAIL_DE_ACCE_OFFSET  5
	#define L3_ROUTE_CTRL_IP_MTU_FAIL_DE_ACCE_LEN     1
	#define L3_ROUTE_CTRL_IP_MTU_FAIL_DE_ACCE_DEFAULT 0x0
	/*[field] IP_MTU_DF_FAIL*/
	#define L3_ROUTE_CTRL_IP_MTU_DF_FAIL
	#define L3_ROUTE_CTRL_IP_MTU_DF_FAIL_OFFSET  6
	#define L3_ROUTE_CTRL_IP_MTU_DF_FAIL_LEN     2
	#define L3_ROUTE_CTRL_IP_MTU_DF_FAIL_DEFAULT 0x3
	/*[field] IP_MTU_DF_FAIL_DE_ACCE*/
	#define L3_ROUTE_CTRL_IP_MTU_DF_FAIL_DE_ACCE
	#define L3_ROUTE_CTRL_IP_MTU_DF_FAIL_DE_ACCE_OFFSET  8
	#define L3_ROUTE_CTRL_IP_MTU_DF_FAIL_DE_ACCE_LEN     1
	#define L3_ROUTE_CTRL_IP_MTU_DF_FAIL_DE_ACCE_DEFAULT 0x0
	/*[field] IP_PREFIX_BC_CMD*/
	#define L3_ROUTE_CTRL_IP_PREFIX_BC_CMD
	#define L3_ROUTE_CTRL_IP_PREFIX_BC_CMD_OFFSET  9
	#define L3_ROUTE_CTRL_IP_PREFIX_BC_CMD_LEN     2
	#define L3_ROUTE_CTRL_IP_PREFIX_BC_CMD_DEFAULT 0x3
	/*[field] IP_PREFIX_BC_DE_ACCE*/
	#define L3_ROUTE_CTRL_IP_PREFIX_BC_DE_ACCE
	#define L3_ROUTE_CTRL_IP_PREFIX_BC_DE_ACCE_OFFSET  11
	#define L3_ROUTE_CTRL_IP_PREFIX_BC_DE_ACCE_LEN     1
	#define L3_ROUTE_CTRL_IP_PREFIX_BC_DE_ACCE_DEFAULT 0x0
	/*[field] FLOW_SRC_IF_CHECK_CMD*/
	#define L3_ROUTE_CTRL_FLOW_SRC_IF_CHECK_CMD
	#define L3_ROUTE_CTRL_FLOW_SRC_IF_CHECK_CMD_OFFSET  12
	#define L3_ROUTE_CTRL_FLOW_SRC_IF_CHECK_CMD_LEN     2
	#define L3_ROUTE_CTRL_FLOW_SRC_IF_CHECK_CMD_DEFAULT 0x3
	/*[field] FLOW_SRC_IF_CHECK_DE_ACCE*/
	#define L3_ROUTE_CTRL_FLOW_SRC_IF_CHECK_DE_ACCE
	#define L3_ROUTE_CTRL_FLOW_SRC_IF_CHECK_DE_ACCE_OFFSET  14
	#define L3_ROUTE_CTRL_FLOW_SRC_IF_CHECK_DE_ACCE_LEN     1
	#define L3_ROUTE_CTRL_FLOW_SRC_IF_CHECK_DE_ACCE_DEFAULT 0x0
	/*[field] FLOW_SERVICE_CODE_LOOP*/
	#define L3_ROUTE_CTRL_FLOW_SERVICE_CODE_LOOP
	#define L3_ROUTE_CTRL_FLOW_SERVICE_CODE_LOOP_OFFSET  15
	#define L3_ROUTE_CTRL_FLOW_SERVICE_CODE_LOOP_LEN     2
	#define L3_ROUTE_CTRL_FLOW_SERVICE_CODE_LOOP_DEFAULT 0x0
	/*[field] FLOW_SERVICE_CODE_LOOP_DE_ACCE*/
	#define L3_ROUTE_CTRL_FLOW_SERVICE_CODE_LOOP_DE_ACCE
	#define L3_ROUTE_CTRL_FLOW_SERVICE_CODE_LOOP_DE_ACCE_OFFSET  17
	#define L3_ROUTE_CTRL_FLOW_SERVICE_CODE_LOOP_DE_ACCE_LEN     1
	#define L3_ROUTE_CTRL_FLOW_SERVICE_CODE_LOOP_DE_ACCE_DEFAULT 0x0
	/*[field] FLOW_DE_ACCE_CMD*/
	#define L3_ROUTE_CTRL_FLOW_DE_ACCE_CMD
	#define L3_ROUTE_CTRL_FLOW_DE_ACCE_CMD_OFFSET  18
	#define L3_ROUTE_CTRL_FLOW_DE_ACCE_CMD_LEN     2
	#define L3_ROUTE_CTRL_FLOW_DE_ACCE_CMD_DEFAULT 0x3
	/*[field] FLOW_SYNC_MISMATCH_CMD*/
	#define L3_ROUTE_CTRL_FLOW_SYNC_MISMATCH_CMD
	#define L3_ROUTE_CTRL_FLOW_SYNC_MISMATCH_CMD_OFFSET  20
	#define L3_ROUTE_CTRL_FLOW_SYNC_MISMATCH_CMD_LEN     2
	#define L3_ROUTE_CTRL_FLOW_SYNC_MISMATCH_CMD_DEFAULT 0x3
	/*[field] FLOW_SYNC_MISMATCH_DE_ACCE*/
	#define L3_ROUTE_CTRL_FLOW_SYNC_MISMATCH_DE_ACCE
	#define L3_ROUTE_CTRL_FLOW_SYNC_MISMATCH_DE_ACCE_OFFSET  22
	#define L3_ROUTE_CTRL_FLOW_SYNC_MISMATCH_DE_ACCE_LEN     1
	#define L3_ROUTE_CTRL_FLOW_SYNC_MISMATCH_DE_ACCE_DEFAULT 0x0
	/*[field] ICMP_RDT_CMD*/
	#define L3_ROUTE_CTRL_ICMP_RDT_CMD
	#define L3_ROUTE_CTRL_ICMP_RDT_CMD_OFFSET  23
	#define L3_ROUTE_CTRL_ICMP_RDT_CMD_LEN     2
	#define L3_ROUTE_CTRL_ICMP_RDT_CMD_DEFAULT 0x3
	/*[field] ICMP_RDT_DE_ACCE*/
	#define L3_ROUTE_CTRL_ICMP_RDT_DE_ACCE
	#define L3_ROUTE_CTRL_ICMP_RDT_DE_ACCE_OFFSET  25
	#define L3_ROUTE_CTRL_ICMP_RDT_DE_ACCE_LEN     1
	#define L3_ROUTE_CTRL_ICMP_RDT_DE_ACCE_DEFAULT 0x0
	/*[field] PPPOE_MULTICAST_CMD*/
	#define L3_ROUTE_CTRL_PPPOE_MULTICAST_CMD
	#define L3_ROUTE_CTRL_PPPOE_MULTICAST_CMD_OFFSET  26
	#define L3_ROUTE_CTRL_PPPOE_MULTICAST_CMD_LEN     2
	#define L3_ROUTE_CTRL_PPPOE_MULTICAST_CMD_DEFAULT 0x3
	/*[field] PPPOE_MULTICAST_DE_ACCE*/
	#define L3_ROUTE_CTRL_PPPOE_MULTICAST_DE_ACCE
	#define L3_ROUTE_CTRL_PPPOE_MULTICAST_DE_ACCE_OFFSET  28
	#define L3_ROUTE_CTRL_PPPOE_MULTICAST_DE_ACCE_LEN     1
	#define L3_ROUTE_CTRL_PPPOE_MULTICAST_DE_ACCE_DEFAULT 0x0

struct l3_route_ctrl {
	a_uint32_t  ip_mru_check_fail:2;
	a_uint32_t  ip_mru_check_fail_de_acce:1;
	a_uint32_t  ip_mtu_fail:2;
	a_uint32_t  ip_mtu_fail_de_acce:1;
	a_uint32_t  ip_mtu_df_fail:2;
	a_uint32_t  ip_mtu_df_fail_de_acce:1;
	a_uint32_t  ip_prefix_bc_cmd:2;
	a_uint32_t  ip_prefix_bc_de_acce:1;
	a_uint32_t  flow_src_if_check_cmd:2;
	a_uint32_t  flow_src_if_check_de_acce:1;
	a_uint32_t  flow_service_code_loop:2;
	a_uint32_t  flow_service_code_loop_de_acce:1;
	a_uint32_t  flow_de_acce_cmd:2;
	a_uint32_t  flow_sync_mismatch_cmd:2;
	a_uint32_t  flow_sync_mismatch_de_acce:1;
	a_uint32_t  icmp_rdt_cmd:2;
	a_uint32_t  icmp_rdt_de_acce:1;
	a_uint32_t  pppoe_multicast_cmd:2;
	a_uint32_t  pppoe_multicast_de_acce:1;
	a_uint32_t  _reserved0:3;
};

union l3_route_ctrl_u {
	a_uint32_t val;
	struct l3_route_ctrl bf;
};

/*[register] L3_ROUTE_CTRL_EXT*/
#define L3_ROUTE_CTRL_EXT
#define L3_ROUTE_CTRL_EXT_ADDRESS 0x344
#define L3_ROUTE_CTRL_EXT_NUM     1
#define L3_ROUTE_CTRL_EXT_INC     0x4
#define L3_ROUTE_CTRL_EXT_TYPE    REG_TYPE_RW
#define L3_ROUTE_CTRL_EXT_DEFAULT 0x23
	/*[field] IP_ROUTE_MISMATCH*/
	#define L3_ROUTE_CTRL_EXT_IP_ROUTE_MISMATCH
	#define L3_ROUTE_CTRL_EXT_IP_ROUTE_MISMATCH_OFFSET  0
	#define L3_ROUTE_CTRL_EXT_IP_ROUTE_MISMATCH_LEN     2
	#define L3_ROUTE_CTRL_EXT_IP_ROUTE_MISMATCH_DEFAULT 0x3
	/*[field] FLOW_SERVICE_CODE_LOOP_EN*/
	#define L3_ROUTE_CTRL_EXT_FLOW_SERVICE_CODE_LOOP_EN
	#define L3_ROUTE_CTRL_EXT_FLOW_SERVICE_CODE_LOOP_EN_OFFSET  2
	#define L3_ROUTE_CTRL_EXT_FLOW_SERVICE_CODE_LOOP_EN_LEN     1
	#define L3_ROUTE_CTRL_EXT_FLOW_SERVICE_CODE_LOOP_EN_DEFAULT 0x0
	/*[field] HOST_HASH_MODE_0*/
	#define L3_ROUTE_CTRL_EXT_HOST_HASH_MODE_0
	#define L3_ROUTE_CTRL_EXT_HOST_HASH_MODE_0_OFFSET  3
	#define L3_ROUTE_CTRL_EXT_HOST_HASH_MODE_0_LEN     2
	#define L3_ROUTE_CTRL_EXT_HOST_HASH_MODE_0_DEFAULT 0x0
	/*[field] HOST_HASH_MODE_1*/
	#define L3_ROUTE_CTRL_EXT_HOST_HASH_MODE_1
	#define L3_ROUTE_CTRL_EXT_HOST_HASH_MODE_1_OFFSET  5
	#define L3_ROUTE_CTRL_EXT_HOST_HASH_MODE_1_LEN     2
	#define L3_ROUTE_CTRL_EXT_HOST_HASH_MODE_1_DEFAULT 0x1

struct l3_route_ctrl_ext {
	a_uint32_t  ip_route_mismatch:2;
	a_uint32_t  flow_service_code_loop_en:1;
	a_uint32_t  host_hash_mode_0:2;
	a_uint32_t  host_hash_mode_1:2;
	a_uint32_t  l3_flow_copy_escape:1;
	a_uint32_t  _reserved0:24;
};

union l3_route_ctrl_ext_u {
	a_uint32_t val;
	struct l3_route_ctrl_ext bf;
};

/*[register] HOST_TBL_OP*/
#define HOST_TBL_OP
#define HOST_TBL_OP_ADDRESS 0x4bc
#define HOST_TBL_OP_NUM     1
#define HOST_TBL_OP_INC     0x4
#define HOST_TBL_OP_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DEFAULT 0x0
	/*[field] CMD_ID*/
	#define HOST_TBL_OP_CMD_ID
	#define HOST_TBL_OP_CMD_ID_OFFSET  0
	#define HOST_TBL_OP_CMD_ID_LEN     4
	#define HOST_TBL_OP_CMD_ID_DEFAULT 0x0
	/*[field] BYP_RSLT_EN*/
	#define HOST_TBL_OP_BYP_RSLT_EN
	#define HOST_TBL_OP_BYP_RSLT_EN_OFFSET  4
	#define HOST_TBL_OP_BYP_RSLT_EN_LEN     1
	#define HOST_TBL_OP_BYP_RSLT_EN_DEFAULT 0x0
	/*[field] OP_TYPE*/
	#define HOST_TBL_OP_OP_TYPE
	#define HOST_TBL_OP_OP_TYPE_OFFSET  5
	#define HOST_TBL_OP_OP_TYPE_LEN     3
	#define HOST_TBL_OP_OP_TYPE_DEFAULT 0x0
	/*[field] HASH_BLOCK_BITMAP*/
	#define HOST_TBL_OP_HASH_BLOCK_BITMAP
	#define HOST_TBL_OP_HASH_BLOCK_BITMAP_OFFSET  8
	#define HOST_TBL_OP_HASH_BLOCK_BITMAP_LEN     2
	#define HOST_TBL_OP_HASH_BLOCK_BITMAP_DEFAULT 0x0
	/*[field] OP_MODE*/
	#define HOST_TBL_OP_OP_MODE
	#define HOST_TBL_OP_OP_MODE_OFFSET  10
	#define HOST_TBL_OP_OP_MODE_LEN     1
	#define HOST_TBL_OP_OP_MODE_DEFAULT 0x0
	/*[field] ENTRY_INDEX*/
	#define HOST_TBL_OP_ENTRY_INDEX
	#define HOST_TBL_OP_ENTRY_INDEX_OFFSET  11
	#define HOST_TBL_OP_ENTRY_INDEX_LEN     13
	#define HOST_TBL_OP_ENTRY_INDEX_DEFAULT 0x0
	/*[field] OP_RESULT*/
	#define HOST_TBL_OP_OP_RESULT
	#define HOST_TBL_OP_OP_RESULT_OFFSET  24
	#define HOST_TBL_OP_OP_RESULT_LEN     1
	#define HOST_TBL_OP_OP_RESULT_DEFAULT 0x0
	/*[field] BUSY*/
	#define HOST_TBL_OP_BUSY
	#define HOST_TBL_OP_BUSY_OFFSET  25
	#define HOST_TBL_OP_BUSY_LEN     1
	#define HOST_TBL_OP_BUSY_DEFAULT 0x0

struct host_tbl_op {
	a_uint32_t  cmd_id:4;
	a_uint32_t  byp_rslt_en:1;
	a_uint32_t  op_type:3;
	a_uint32_t  hash_block_bitmap:2;
	a_uint32_t  op_mode:1;
	a_uint32_t  entry_index:13;
	a_uint32_t  op_result:1;
	a_uint32_t  busy:1;
	a_uint32_t  _reserved0:6;
};

union host_tbl_op_u {
	a_uint32_t val;
	struct host_tbl_op bf;
};

/*[register] HOST_TBL_OP_DATA0*/
#define HOST_TBL_OP_DATA0
#define HOST_TBL_OP_DATA0_ADDRESS 0x4c0
#define HOST_TBL_OP_DATA0_NUM     1
#define HOST_TBL_OP_DATA0_INC     0x4
#define HOST_TBL_OP_DATA0_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA0_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA0_DATA
	#define HOST_TBL_OP_DATA0_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA0_DATA_LEN     32
	#define HOST_TBL_OP_DATA0_DATA_DEFAULT 0x0

struct host_tbl_op_data0 {
	a_uint32_t  data:32;
};

union host_tbl_op_data0_u {
	a_uint32_t val;
	struct host_tbl_op_data0 bf;
};

/*[register] HOST_TBL_OP_DATA1*/
#define HOST_TBL_OP_DATA1
#define HOST_TBL_OP_DATA1_ADDRESS 0x4c4
#define HOST_TBL_OP_DATA1_NUM     1
#define HOST_TBL_OP_DATA1_INC     0x4
#define HOST_TBL_OP_DATA1_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA1_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA1_DATA
	#define HOST_TBL_OP_DATA1_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA1_DATA_LEN     32
	#define HOST_TBL_OP_DATA1_DATA_DEFAULT 0x0

struct host_tbl_op_data1 {
	a_uint32_t  data:32;
};

union host_tbl_op_data1_u {
	a_uint32_t val;
	struct host_tbl_op_data1 bf;
};

/*[register] HOST_TBL_OP_DATA2*/
#define HOST_TBL_OP_DATA2
#define HOST_TBL_OP_DATA2_ADDRESS 0x4c8
#define HOST_TBL_OP_DATA2_NUM     1
#define HOST_TBL_OP_DATA2_INC     0x4
#define HOST_TBL_OP_DATA2_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA2_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA2_DATA
	#define HOST_TBL_OP_DATA2_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA2_DATA_LEN     32
	#define HOST_TBL_OP_DATA2_DATA_DEFAULT 0x0

struct host_tbl_op_data2 {
	a_uint32_t  data:32;
};

union host_tbl_op_data2_u {
	a_uint32_t val;
	struct host_tbl_op_data2 bf;
};

/*[register] HOST_TBL_OP_DATA3*/
#define HOST_TBL_OP_DATA3
#define HOST_TBL_OP_DATA3_ADDRESS 0x4cc
#define HOST_TBL_OP_DATA3_NUM     1
#define HOST_TBL_OP_DATA3_INC     0x4
#define HOST_TBL_OP_DATA3_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA3_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA3_DATA
	#define HOST_TBL_OP_DATA3_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA3_DATA_LEN     32
	#define HOST_TBL_OP_DATA3_DATA_DEFAULT 0x0

struct host_tbl_op_data3 {
	a_uint32_t  data:32;
};

union host_tbl_op_data3_u {
	a_uint32_t val;
	struct host_tbl_op_data3 bf;
};

/*[register] HOST_TBL_OP_DATA4*/
#define HOST_TBL_OP_DATA4
#define HOST_TBL_OP_DATA4_ADDRESS 0x4d0
#define HOST_TBL_OP_DATA4_NUM     1
#define HOST_TBL_OP_DATA4_INC     0x4
#define HOST_TBL_OP_DATA4_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA4_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA4_DATA
	#define HOST_TBL_OP_DATA4_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA4_DATA_LEN     32
	#define HOST_TBL_OP_DATA4_DATA_DEFAULT 0x0

struct host_tbl_op_data4 {
	a_uint32_t  data:32;
};

union host_tbl_op_data4_u {
	a_uint32_t val;
	struct host_tbl_op_data4 bf;
};

/*[register] HOST_TBL_OP_DATA5*/
#define HOST_TBL_OP_DATA5
#define HOST_TBL_OP_DATA5_ADDRESS 0x4d4
#define HOST_TBL_OP_DATA5_NUM     1
#define HOST_TBL_OP_DATA5_INC     0x4
#define HOST_TBL_OP_DATA5_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA5_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA5_DATA
	#define HOST_TBL_OP_DATA5_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA5_DATA_LEN     32
	#define HOST_TBL_OP_DATA5_DATA_DEFAULT 0x0

struct host_tbl_op_data5 {
	a_uint32_t  data:32;
};

union host_tbl_op_data5_u {
	a_uint32_t val;
	struct host_tbl_op_data5 bf;
};

/*[register] HOST_TBL_OP_DATA6*/
#define HOST_TBL_OP_DATA6
#define HOST_TBL_OP_DATA6_ADDRESS 0x4d8
#define HOST_TBL_OP_DATA6_NUM     1
#define HOST_TBL_OP_DATA6_INC     0x4
#define HOST_TBL_OP_DATA6_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA6_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA6_DATA
	#define HOST_TBL_OP_DATA6_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA6_DATA_LEN     32
	#define HOST_TBL_OP_DATA6_DATA_DEFAULT 0x0

struct host_tbl_op_data6 {
	a_uint32_t  data:32;
};

union host_tbl_op_data6_u {
	a_uint32_t val;
	struct host_tbl_op_data6 bf;
};

/*[register] HOST_TBL_OP_DATA7*/
#define HOST_TBL_OP_DATA7
#define HOST_TBL_OP_DATA7_ADDRESS 0x4dc
#define HOST_TBL_OP_DATA7_NUM     1
#define HOST_TBL_OP_DATA7_INC     0x4
#define HOST_TBL_OP_DATA7_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA7_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA7_DATA
	#define HOST_TBL_OP_DATA7_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA7_DATA_LEN     32
	#define HOST_TBL_OP_DATA7_DATA_DEFAULT 0x0

struct host_tbl_op_data7 {
	a_uint32_t  data:32;
};

union host_tbl_op_data7_u {
	a_uint32_t val;
	struct host_tbl_op_data7 bf;
};

/*[register] HOST_TBL_OP_DATA8*/
#define HOST_TBL_OP_DATA8
#define HOST_TBL_OP_DATA8_ADDRESS 0x4e0
#define HOST_TBL_OP_DATA8_NUM     1
#define HOST_TBL_OP_DATA8_INC     0x4
#define HOST_TBL_OP_DATA8_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA8_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA8_DATA
	#define HOST_TBL_OP_DATA8_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA8_DATA_LEN     32
	#define HOST_TBL_OP_DATA8_DATA_DEFAULT 0x0

struct host_tbl_op_data8 {
	a_uint32_t  data:32;
};

union host_tbl_op_data8_u {
	a_uint32_t val;
	struct host_tbl_op_data8 bf;
};

/*[register] HOST_TBL_OP_DATA9*/
#define HOST_TBL_OP_DATA9
#define HOST_TBL_OP_DATA9_ADDRESS 0x4e4
#define HOST_TBL_OP_DATA9_NUM     1
#define HOST_TBL_OP_DATA9_INC     0x4
#define HOST_TBL_OP_DATA9_TYPE    REG_TYPE_RW
#define HOST_TBL_OP_DATA9_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_OP_DATA9_DATA
	#define HOST_TBL_OP_DATA9_DATA_OFFSET  0
	#define HOST_TBL_OP_DATA9_DATA_LEN     32
	#define HOST_TBL_OP_DATA9_DATA_DEFAULT 0x0

struct host_tbl_op_data9 {
	a_uint32_t  data:32;
};

union host_tbl_op_data9_u {
	a_uint32_t val;
	struct host_tbl_op_data9 bf;
};

/*[register] HOST_TBL_OP_RSLT*/
#define HOST_TBL_OP_RSLT
#define HOST_TBL_OP_RSLT_ADDRESS 0x4e8
#define HOST_TBL_OP_RSLT_NUM     1
#define HOST_TBL_OP_RSLT_INC     0x4
#define HOST_TBL_OP_RSLT_TYPE    REG_TYPE_RO
#define HOST_TBL_OP_RSLT_DEFAULT 0x0
	/*[field] CMD_ID*/
	#define HOST_TBL_OP_RSLT_CMD_ID
	#define HOST_TBL_OP_RSLT_CMD_ID_OFFSET  0
	#define HOST_TBL_OP_RSLT_CMD_ID_LEN     4
	#define HOST_TBL_OP_RSLT_CMD_ID_DEFAULT 0x0
	/*[field] OP_RSLT*/
	#define HOST_TBL_OP_RSLT_OP_RSLT
	#define HOST_TBL_OP_RSLT_OP_RSLT_OFFSET  4
	#define HOST_TBL_OP_RSLT_OP_RSLT_LEN     1
	#define HOST_TBL_OP_RSLT_OP_RSLT_DEFAULT 0x0
	/*[field] ENTRY_INDEX*/
	#define HOST_TBL_OP_RSLT_ENTRY_INDEX
	#define HOST_TBL_OP_RSLT_ENTRY_INDEX_OFFSET  5
	#define HOST_TBL_OP_RSLT_ENTRY_INDEX_LEN     13
	#define HOST_TBL_OP_RSLT_ENTRY_INDEX_DEFAULT 0x0
	/*[field] VALID_CNT*/
	#define HOST_TBL_OP_RSLT_VALID_CNT
	#define HOST_TBL_OP_RSLT_VALID_CNT_OFFSET  18
	#define HOST_TBL_OP_RSLT_VALID_CNT_LEN     4
	#define HOST_TBL_OP_RSLT_VALID_CNT_DEFAULT 0x0

struct host_tbl_op_rslt {
	a_uint32_t  cmd_id:4;
	a_uint32_t  op_rslt:1;
	a_uint32_t  entry_index:13;
	a_uint32_t  valid_cnt:4;
	a_uint32_t  _reserved0:10;
};

union host_tbl_op_rslt_u {
	a_uint32_t val;
	struct host_tbl_op_rslt bf;
};

/*[register] HOST_TBL_RD_OP*/
#define HOST_TBL_RD_OP
#define HOST_TBL_RD_OP_ADDRESS 0x4ec
#define HOST_TBL_RD_OP_NUM     1
#define HOST_TBL_RD_OP_INC     0x4
#define HOST_TBL_RD_OP_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DEFAULT 0x0
	/*[field] CMD_ID*/
	#define HOST_TBL_RD_OP_CMD_ID
	#define HOST_TBL_RD_OP_CMD_ID_OFFSET  0
	#define HOST_TBL_RD_OP_CMD_ID_LEN     4
	#define HOST_TBL_RD_OP_CMD_ID_DEFAULT 0x0
	/*[field] BYP_RSLT_EN*/
	#define HOST_TBL_RD_OP_BYP_RSLT_EN
	#define HOST_TBL_RD_OP_BYP_RSLT_EN_OFFSET  4
	#define HOST_TBL_RD_OP_BYP_RSLT_EN_LEN     1
	#define HOST_TBL_RD_OP_BYP_RSLT_EN_DEFAULT 0x0
	/*[field] OP_TYPE*/
	#define HOST_TBL_RD_OP_OP_TYPE
	#define HOST_TBL_RD_OP_OP_TYPE_OFFSET  5
	#define HOST_TBL_RD_OP_OP_TYPE_LEN     3
	#define HOST_TBL_RD_OP_OP_TYPE_DEFAULT 0x0
	/*[field] HASH_BLOCK_BITMAP*/
	#define HOST_TBL_RD_OP_HASH_BLOCK_BITMAP
	#define HOST_TBL_RD_OP_HASH_BLOCK_BITMAP_OFFSET  8
	#define HOST_TBL_RD_OP_HASH_BLOCK_BITMAP_LEN     2
	#define HOST_TBL_RD_OP_HASH_BLOCK_BITMAP_DEFAULT 0x0
	/*[field] OP_MODE*/
	#define HOST_TBL_RD_OP_OP_MODE
	#define HOST_TBL_RD_OP_OP_MODE_OFFSET  10
	#define HOST_TBL_RD_OP_OP_MODE_LEN     1
	#define HOST_TBL_RD_OP_OP_MODE_DEFAULT 0x0
	/*[field] ENTRY_INDEX*/
	#define HOST_TBL_RD_OP_ENTRY_INDEX
	#define HOST_TBL_RD_OP_ENTRY_INDEX_OFFSET  11
	#define HOST_TBL_RD_OP_ENTRY_INDEX_LEN     13
	#define HOST_TBL_RD_OP_ENTRY_INDEX_DEFAULT 0x0
	/*[field] OP_RESULT*/
	#define HOST_TBL_RD_OP_OP_RESULT
	#define HOST_TBL_RD_OP_OP_RESULT_OFFSET  24
	#define HOST_TBL_RD_OP_OP_RESULT_LEN     1
	#define HOST_TBL_RD_OP_OP_RESULT_DEFAULT 0x0
	/*[field] BUSY*/
	#define HOST_TBL_RD_OP_BUSY
	#define HOST_TBL_RD_OP_BUSY_OFFSET  25
	#define HOST_TBL_RD_OP_BUSY_LEN     1
	#define HOST_TBL_RD_OP_BUSY_DEFAULT 0x0

struct host_tbl_rd_op {
	a_uint32_t  cmd_id:4;
	a_uint32_t  byp_rslt_en:1;
	a_uint32_t  op_type:3;
	a_uint32_t  hash_block_bitmap:2;
	a_uint32_t  op_mode:1;
	a_uint32_t  entry_index:13;
	a_uint32_t  op_result:1;
	a_uint32_t  busy:1;
	a_uint32_t  _reserved0:6;
};

union host_tbl_rd_op_u {
	a_uint32_t val;
	struct host_tbl_rd_op bf;
};

/*[register] HOST_TBL_RD_OP_DATA0*/
#define HOST_TBL_RD_OP_DATA0
#define HOST_TBL_RD_OP_DATA0_ADDRESS 0x4f0
#define HOST_TBL_RD_OP_DATA0_NUM     1
#define HOST_TBL_RD_OP_DATA0_INC     0x4
#define HOST_TBL_RD_OP_DATA0_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA0_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA0_DATA
	#define HOST_TBL_RD_OP_DATA0_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA0_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA0_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data0 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data0_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data0 bf;
};

/*[register] HOST_TBL_RD_OP_DATA1*/
#define HOST_TBL_RD_OP_DATA1
#define HOST_TBL_RD_OP_DATA1_ADDRESS 0x4f4
#define HOST_TBL_RD_OP_DATA1_NUM     1
#define HOST_TBL_RD_OP_DATA1_INC     0x4
#define HOST_TBL_RD_OP_DATA1_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA1_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA1_DATA
	#define HOST_TBL_RD_OP_DATA1_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA1_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA1_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data1 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data1_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data1 bf;
};

/*[register] HOST_TBL_RD_OP_DATA2*/
#define HOST_TBL_RD_OP_DATA2
#define HOST_TBL_RD_OP_DATA2_ADDRESS 0x4f8
#define HOST_TBL_RD_OP_DATA2_NUM     1
#define HOST_TBL_RD_OP_DATA2_INC     0x4
#define HOST_TBL_RD_OP_DATA2_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA2_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA2_DATA
	#define HOST_TBL_RD_OP_DATA2_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA2_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA2_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data2 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data2_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data2 bf;
};

/*[register] HOST_TBL_RD_OP_DATA3*/
#define HOST_TBL_RD_OP_DATA3
#define HOST_TBL_RD_OP_DATA3_ADDRESS 0x4fc
#define HOST_TBL_RD_OP_DATA3_NUM     1
#define HOST_TBL_RD_OP_DATA3_INC     0x4
#define HOST_TBL_RD_OP_DATA3_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA3_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA3_DATA
	#define HOST_TBL_RD_OP_DATA3_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA3_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA3_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data3 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data3_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data3 bf;
};

/*[register] HOST_TBL_RD_OP_DATA4*/
#define HOST_TBL_RD_OP_DATA4
#define HOST_TBL_RD_OP_DATA4_ADDRESS 0x500
#define HOST_TBL_RD_OP_DATA4_NUM     1
#define HOST_TBL_RD_OP_DATA4_INC     0x4
#define HOST_TBL_RD_OP_DATA4_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA4_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA4_DATA
	#define HOST_TBL_RD_OP_DATA4_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA4_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA4_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data4 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data4_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data4 bf;
};

/*[register] HOST_TBL_RD_OP_DATA5*/
#define HOST_TBL_RD_OP_DATA5
#define HOST_TBL_RD_OP_DATA5_ADDRESS 0x504
#define HOST_TBL_RD_OP_DATA5_NUM     1
#define HOST_TBL_RD_OP_DATA5_INC     0x4
#define HOST_TBL_RD_OP_DATA5_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA5_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA5_DATA
	#define HOST_TBL_RD_OP_DATA5_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA5_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA5_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data5 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data5_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data5 bf;
};

/*[register] HOST_TBL_RD_OP_DATA6*/
#define HOST_TBL_RD_OP_DATA6
#define HOST_TBL_RD_OP_DATA6_ADDRESS 0x508
#define HOST_TBL_RD_OP_DATA6_NUM     1
#define HOST_TBL_RD_OP_DATA6_INC     0x4
#define HOST_TBL_RD_OP_DATA6_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA6_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA6_DATA
	#define HOST_TBL_RD_OP_DATA6_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA6_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA6_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data6 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data6_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data6 bf;
};

/*[register] HOST_TBL_RD_OP_DATA7*/
#define HOST_TBL_RD_OP_DATA7
#define HOST_TBL_RD_OP_DATA7_ADDRESS 0x50c
#define HOST_TBL_RD_OP_DATA7_NUM     1
#define HOST_TBL_RD_OP_DATA7_INC     0x4
#define HOST_TBL_RD_OP_DATA7_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA7_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA7_DATA
	#define HOST_TBL_RD_OP_DATA7_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA7_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA7_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data7 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data7_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data7 bf;
};

/*[register] HOST_TBL_RD_OP_DATA8*/
#define HOST_TBL_RD_OP_DATA8
#define HOST_TBL_RD_OP_DATA8_ADDRESS 0x510
#define HOST_TBL_RD_OP_DATA8_NUM     1
#define HOST_TBL_RD_OP_DATA8_INC     0x4
#define HOST_TBL_RD_OP_DATA8_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA8_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA8_DATA
	#define HOST_TBL_RD_OP_DATA8_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA8_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA8_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data8 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data8_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data8 bf;
};

/*[register] HOST_TBL_RD_OP_DATA9*/
#define HOST_TBL_RD_OP_DATA9
#define HOST_TBL_RD_OP_DATA9_ADDRESS 0x514
#define HOST_TBL_RD_OP_DATA9_NUM     1
#define HOST_TBL_RD_OP_DATA9_INC     0x4
#define HOST_TBL_RD_OP_DATA9_TYPE    REG_TYPE_RW
#define HOST_TBL_RD_OP_DATA9_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_OP_DATA9_DATA
	#define HOST_TBL_RD_OP_DATA9_DATA_OFFSET  0
	#define HOST_TBL_RD_OP_DATA9_DATA_LEN     32
	#define HOST_TBL_RD_OP_DATA9_DATA_DEFAULT 0x0

struct host_tbl_rd_op_data9 {
	a_uint32_t  data:32;
};

union host_tbl_rd_op_data9_u {
	a_uint32_t val;
	struct host_tbl_rd_op_data9 bf;
};

/*[register] HOST_TBL_RD_OP_RSLT*/
#define HOST_TBL_RD_OP_RSLT
#define HOST_TBL_RD_OP_RSLT_ADDRESS 0x518
#define HOST_TBL_RD_OP_RSLT_NUM     1
#define HOST_TBL_RD_OP_RSLT_INC     0x4
#define HOST_TBL_RD_OP_RSLT_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_OP_RSLT_DEFAULT 0x0
	/*[field] CMD_ID*/
	#define HOST_TBL_RD_OP_RSLT_CMD_ID
	#define HOST_TBL_RD_OP_RSLT_CMD_ID_OFFSET  0
	#define HOST_TBL_RD_OP_RSLT_CMD_ID_LEN     4
	#define HOST_TBL_RD_OP_RSLT_CMD_ID_DEFAULT 0x0
	/*[field] OP_RSLT*/
	#define HOST_TBL_RD_OP_RSLT_OP_RSLT
	#define HOST_TBL_RD_OP_RSLT_OP_RSLT_OFFSET  4
	#define HOST_TBL_RD_OP_RSLT_OP_RSLT_LEN     1
	#define HOST_TBL_RD_OP_RSLT_OP_RSLT_DEFAULT 0x0
	/*[field] ENTRY_INDEX*/
	#define HOST_TBL_RD_OP_RSLT_ENTRY_INDEX
	#define HOST_TBL_RD_OP_RSLT_ENTRY_INDEX_OFFSET  5
	#define HOST_TBL_RD_OP_RSLT_ENTRY_INDEX_LEN     13
	#define HOST_TBL_RD_OP_RSLT_ENTRY_INDEX_DEFAULT 0x0
	/*[field] VALID_CNT*/
	#define HOST_TBL_RD_OP_RSLT_VALID_CNT
	#define HOST_TBL_RD_OP_RSLT_VALID_CNT_OFFSET  18
	#define HOST_TBL_RD_OP_RSLT_VALID_CNT_LEN     4
	#define HOST_TBL_RD_OP_RSLT_VALID_CNT_DEFAULT 0x0

struct host_tbl_rd_op_rslt {
	a_uint32_t  cmd_id:4;
	a_uint32_t  op_rslt:1;
	a_uint32_t  entry_index:13;
	a_uint32_t  valid_cnt:4;
	a_uint32_t  _reserved0:10;
};

union host_tbl_rd_op_rslt_u {
	a_uint32_t val;
	struct host_tbl_rd_op_rslt bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA0*/
#define HOST_TBL_RD_RSLT_DATA0
#define HOST_TBL_RD_RSLT_DATA0_ADDRESS 0x51c
#define HOST_TBL_RD_RSLT_DATA0_NUM     1
#define HOST_TBL_RD_RSLT_DATA0_INC     0x4
#define HOST_TBL_RD_RSLT_DATA0_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA0_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA0_DATA
	#define HOST_TBL_RD_RSLT_DATA0_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA0_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA0_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data0 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data0_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data0 bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA1*/
#define HOST_TBL_RD_RSLT_DATA1
#define HOST_TBL_RD_RSLT_DATA1_ADDRESS 0x520
#define HOST_TBL_RD_RSLT_DATA1_NUM     1
#define HOST_TBL_RD_RSLT_DATA1_INC     0x4
#define HOST_TBL_RD_RSLT_DATA1_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA1_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA1_DATA
	#define HOST_TBL_RD_RSLT_DATA1_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA1_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA1_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data1 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data1_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data1 bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA2*/
#define HOST_TBL_RD_RSLT_DATA2
#define HOST_TBL_RD_RSLT_DATA2_ADDRESS 0x524
#define HOST_TBL_RD_RSLT_DATA2_NUM     1
#define HOST_TBL_RD_RSLT_DATA2_INC     0x4
#define HOST_TBL_RD_RSLT_DATA2_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA2_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA2_DATA
	#define HOST_TBL_RD_RSLT_DATA2_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA2_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA2_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data2 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data2_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data2 bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA3*/
#define HOST_TBL_RD_RSLT_DATA3
#define HOST_TBL_RD_RSLT_DATA3_ADDRESS 0x528
#define HOST_TBL_RD_RSLT_DATA3_NUM     1
#define HOST_TBL_RD_RSLT_DATA3_INC     0x4
#define HOST_TBL_RD_RSLT_DATA3_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA3_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA3_DATA
	#define HOST_TBL_RD_RSLT_DATA3_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA3_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA3_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data3 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data3_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data3 bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA4*/
#define HOST_TBL_RD_RSLT_DATA4
#define HOST_TBL_RD_RSLT_DATA4_ADDRESS 0x52c
#define HOST_TBL_RD_RSLT_DATA4_NUM     1
#define HOST_TBL_RD_RSLT_DATA4_INC     0x4
#define HOST_TBL_RD_RSLT_DATA4_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA4_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA4_DATA
	#define HOST_TBL_RD_RSLT_DATA4_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA4_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA4_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data4 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data4_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data4 bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA5*/
#define HOST_TBL_RD_RSLT_DATA5
#define HOST_TBL_RD_RSLT_DATA5_ADDRESS 0x530
#define HOST_TBL_RD_RSLT_DATA5_NUM     1
#define HOST_TBL_RD_RSLT_DATA5_INC     0x4
#define HOST_TBL_RD_RSLT_DATA5_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA5_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA5_DATA
	#define HOST_TBL_RD_RSLT_DATA5_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA5_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA5_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data5 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data5_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data5 bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA6*/
#define HOST_TBL_RD_RSLT_DATA6
#define HOST_TBL_RD_RSLT_DATA6_ADDRESS 0x534
#define HOST_TBL_RD_RSLT_DATA6_NUM     1
#define HOST_TBL_RD_RSLT_DATA6_INC     0x4
#define HOST_TBL_RD_RSLT_DATA6_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA6_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA6_DATA
	#define HOST_TBL_RD_RSLT_DATA6_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA6_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA6_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data6 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data6_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data6 bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA7*/
#define HOST_TBL_RD_RSLT_DATA7
#define HOST_TBL_RD_RSLT_DATA7_ADDRESS 0x538
#define HOST_TBL_RD_RSLT_DATA7_NUM     1
#define HOST_TBL_RD_RSLT_DATA7_INC     0x4
#define HOST_TBL_RD_RSLT_DATA7_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA7_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA7_DATA
	#define HOST_TBL_RD_RSLT_DATA7_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA7_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA7_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data7 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data7_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data7 bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA8*/
#define HOST_TBL_RD_RSLT_DATA8
#define HOST_TBL_RD_RSLT_DATA8_ADDRESS 0x53c
#define HOST_TBL_RD_RSLT_DATA8_NUM     1
#define HOST_TBL_RD_RSLT_DATA8_INC     0x4
#define HOST_TBL_RD_RSLT_DATA8_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA8_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA8_DATA
	#define HOST_TBL_RD_RSLT_DATA8_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA8_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA8_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data8 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data8_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data8 bf;
};

/*[register] HOST_TBL_RD_RSLT_DATA9*/
#define HOST_TBL_RD_RSLT_DATA9
#define HOST_TBL_RD_RSLT_DATA9_ADDRESS 0x540
#define HOST_TBL_RD_RSLT_DATA9_NUM     1
#define HOST_TBL_RD_RSLT_DATA9_INC     0x4
#define HOST_TBL_RD_RSLT_DATA9_TYPE    REG_TYPE_RO
#define HOST_TBL_RD_RSLT_DATA9_DEFAULT 0x0
	/*[field] DATA*/
	#define HOST_TBL_RD_RSLT_DATA9_DATA
	#define HOST_TBL_RD_RSLT_DATA9_DATA_OFFSET  0
	#define HOST_TBL_RD_RSLT_DATA9_DATA_LEN     32
	#define HOST_TBL_RD_RSLT_DATA9_DATA_DEFAULT 0x0

struct host_tbl_rd_rslt_data9 {
	a_uint32_t  data:32;
};

union host_tbl_rd_rslt_data9_u {
	a_uint32_t val;
	struct host_tbl_rd_rslt_data9 bf;
};

/*[register] L3_DBG_CMD*/
#define L3_DBG_CMD
#define L3_DBG_CMD_ADDRESS 0xc04
#define L3_DBG_CMD_NUM     1
#define L3_DBG_CMD_INC     0x4
#define L3_DBG_CMD_TYPE    REG_TYPE_RW
#define L3_DBG_CMD_DEFAULT 0x0
	/*[field] ADDR*/
	#define L3_DBG_CMD_ADDR
	#define L3_DBG_CMD_ADDR_OFFSET  0
	#define L3_DBG_CMD_ADDR_LEN     8
	#define L3_DBG_CMD_ADDR_DEFAULT 0x0
	/*[field] TYPE*/
	#define L3_DBG_CMD_TYPE_F
	#define L3_DBG_CMD_TYPE_F_OFFSET  8
	#define L3_DBG_CMD_TYPE_F_LEN     2
	#define L3_DBG_CMD_TYPE_F_DEFAULT 0x0

struct l3_dbg_cmd {
	a_uint32_t  addr:8;
	a_uint32_t  type:2;
	a_uint32_t  _reserved0:22;
};

union l3_dbg_cmd_u {
	a_uint32_t val;
	struct l3_dbg_cmd bf;
};

/*[register] L3_DBG_WR_DATA*/
#define L3_DBG_WR_DATA
#define L3_DBG_WR_DATA_ADDRESS 0xc08
#define L3_DBG_WR_DATA_NUM     1
#define L3_DBG_WR_DATA_INC     0x4
#define L3_DBG_WR_DATA_TYPE    REG_TYPE_RW
#define L3_DBG_WR_DATA_DEFAULT 0x0
	/*[field] DATA*/
	#define L3_DBG_WR_DATA_DATA
	#define L3_DBG_WR_DATA_DATA_OFFSET  0
	#define L3_DBG_WR_DATA_DATA_LEN     32
	#define L3_DBG_WR_DATA_DATA_DEFAULT 0x0

struct l3_dbg_wr_data {
	a_uint32_t  data:32;
};

union l3_dbg_wr_data_u {
	a_uint32_t val;
	struct l3_dbg_wr_data bf;
};

/*[register] L3_DBG_RD_DATA*/
#define L3_DBG_RD_DATA
#define L3_DBG_RD_DATA_ADDRESS 0xc0c
#define L3_DBG_RD_DATA_NUM     1
#define L3_DBG_RD_DATA_INC     0x4
#define L3_DBG_RD_DATA_TYPE    REG_TYPE_RO
#define L3_DBG_RD_DATA_DEFAULT 0x0
	/*[field] DATA*/
	#define L3_DBG_RD_DATA_DATA
	#define L3_DBG_RD_DATA_DATA_OFFSET  0
	#define L3_DBG_RD_DATA_DATA_LEN     32
	#define L3_DBG_RD_DATA_DATA_DEFAULT 0x0

struct l3_dbg_rd_data {
	a_uint32_t  data:32;
};

union l3_dbg_rd_data_u {
	a_uint32_t val;
	struct l3_dbg_rd_data bf;
};

/*[register] IN_PUB_IP_ADDR_TBL*/
#define IN_PUB_IP_ADDR_TBL
#define IN_PUB_IP_ADDR_TBL_ADDRESS 0x378
#define IN_PUB_IP_ADDR_TBL_NUM     16
#define IN_PUB_IP_ADDR_TBL_INC     0x4
#define IN_PUB_IP_ADDR_TBL_TYPE    REG_TYPE_RW
#define IN_PUB_IP_ADDR_TBL_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define IN_PUB_IP_ADDR_TBL_IP_ADDR
	#define IN_PUB_IP_ADDR_TBL_IP_ADDR_OFFSET  0
	#define IN_PUB_IP_ADDR_TBL_IP_ADDR_LEN     32
	#define IN_PUB_IP_ADDR_TBL_IP_ADDR_DEFAULT 0x0

struct in_pub_ip_addr_tbl {
	a_uint32_t  ip_addr:32;
};

union in_pub_ip_addr_tbl_u {
	a_uint32_t val;
	struct in_pub_ip_addr_tbl bf;
};

/*[table] L3_VP_PORT_TBL*/
#define L3_VP_PORT_TBL
#define L3_VP_PORT_TBL_ADDRESS 0x1000
#define L3_VP_PORT_TBL_NUM     256
#define L3_VP_PORT_TBL_INC     0x10
#define L3_VP_PORT_TBL_TYPE    REG_TYPE_RW
#define L3_VP_PORT_TBL_DEFAULT 0x0
	/*[field] L3_IF_VALID*/
	#define L3_VP_PORT_TBL_L3_IF_VALID
	#define L3_VP_PORT_TBL_L3_IF_VALID_OFFSET  0
	#define L3_VP_PORT_TBL_L3_IF_VALID_LEN     1
	#define L3_VP_PORT_TBL_L3_IF_VALID_DEFAULT 0x0
	/*[field] L3_IF_INDEX*/
	#define L3_VP_PORT_TBL_L3_IF_INDEX
	#define L3_VP_PORT_TBL_L3_IF_INDEX_OFFSET  1
	#define L3_VP_PORT_TBL_L3_IF_INDEX_LEN     8
	#define L3_VP_PORT_TBL_L3_IF_INDEX_DEFAULT 0x0
	/*[field] IPV4_SG_EN*/
	#define L3_VP_PORT_TBL_IPV4_SG_EN
	#define L3_VP_PORT_TBL_IPV4_SG_EN_OFFSET  9
	#define L3_VP_PORT_TBL_IPV4_SG_EN_LEN     1
	#define L3_VP_PORT_TBL_IPV4_SG_EN_DEFAULT 0x0
	/*[field] IPV4_SG_VIO_CMD*/
	#define L3_VP_PORT_TBL_IPV4_SG_VIO_CMD
	#define L3_VP_PORT_TBL_IPV4_SG_VIO_CMD_OFFSET  10
	#define L3_VP_PORT_TBL_IPV4_SG_VIO_CMD_LEN     2
	#define L3_VP_PORT_TBL_IPV4_SG_VIO_CMD_DEFAULT 0x0
	/*[field] IPV4_SG_PORT_EN*/
	#define L3_VP_PORT_TBL_IPV4_SG_PORT_EN
	#define L3_VP_PORT_TBL_IPV4_SG_PORT_EN_OFFSET  12
	#define L3_VP_PORT_TBL_IPV4_SG_PORT_EN_LEN     1
	#define L3_VP_PORT_TBL_IPV4_SG_PORT_EN_DEFAULT 0x0
	/*[field] IPV4_SG_SVLAN_EN*/
	#define L3_VP_PORT_TBL_IPV4_SG_SVLAN_EN
	#define L3_VP_PORT_TBL_IPV4_SG_SVLAN_EN_OFFSET  13
	#define L3_VP_PORT_TBL_IPV4_SG_SVLAN_EN_LEN     1
	#define L3_VP_PORT_TBL_IPV4_SG_SVLAN_EN_DEFAULT 0x0
	/*[field] IPV4_SG_CVLAN_EN*/
	#define L3_VP_PORT_TBL_IPV4_SG_CVLAN_EN
	#define L3_VP_PORT_TBL_IPV4_SG_CVLAN_EN_OFFSET  14
	#define L3_VP_PORT_TBL_IPV4_SG_CVLAN_EN_LEN     1
	#define L3_VP_PORT_TBL_IPV4_SG_CVLAN_EN_DEFAULT 0x0
	/*[field] IPV4_SRC_UNK_CMD*/
	#define L3_VP_PORT_TBL_IPV4_SRC_UNK_CMD
	#define L3_VP_PORT_TBL_IPV4_SRC_UNK_CMD_OFFSET  15
	#define L3_VP_PORT_TBL_IPV4_SRC_UNK_CMD_LEN     2
	#define L3_VP_PORT_TBL_IPV4_SRC_UNK_CMD_DEFAULT 0x0
	/*[field] IPV6_SG_EN*/
	#define L3_VP_PORT_TBL_IPV6_SG_EN
	#define L3_VP_PORT_TBL_IPV6_SG_EN_OFFSET  17
	#define L3_VP_PORT_TBL_IPV6_SG_EN_LEN     1
	#define L3_VP_PORT_TBL_IPV6_SG_EN_DEFAULT 0x0
	/*[field] IPV6_SG_VIO_CMD*/
	#define L3_VP_PORT_TBL_IPV6_SG_VIO_CMD
	#define L3_VP_PORT_TBL_IPV6_SG_VIO_CMD_OFFSET  18
	#define L3_VP_PORT_TBL_IPV6_SG_VIO_CMD_LEN     2
	#define L3_VP_PORT_TBL_IPV6_SG_VIO_CMD_DEFAULT 0x0
	/*[field] IPV6_SG_PORT_EN*/
	#define L3_VP_PORT_TBL_IPV6_SG_PORT_EN
	#define L3_VP_PORT_TBL_IPV6_SG_PORT_EN_OFFSET  20
	#define L3_VP_PORT_TBL_IPV6_SG_PORT_EN_LEN     1
	#define L3_VP_PORT_TBL_IPV6_SG_PORT_EN_DEFAULT 0x0
	/*[field] IPV6_SG_SVLAN_EN*/
	#define L3_VP_PORT_TBL_IPV6_SG_SVLAN_EN
	#define L3_VP_PORT_TBL_IPV6_SG_SVLAN_EN_OFFSET  21
	#define L3_VP_PORT_TBL_IPV6_SG_SVLAN_EN_LEN     1
	#define L3_VP_PORT_TBL_IPV6_SG_SVLAN_EN_DEFAULT 0x0
	/*[field] IPV6_SG_CVLAN_EN*/
	#define L3_VP_PORT_TBL_IPV6_SG_CVLAN_EN
	#define L3_VP_PORT_TBL_IPV6_SG_CVLAN_EN_OFFSET  22
	#define L3_VP_PORT_TBL_IPV6_SG_CVLAN_EN_LEN     1
	#define L3_VP_PORT_TBL_IPV6_SG_CVLAN_EN_DEFAULT 0x0
	/*[field] IPV6_SRC_UNK_CMD*/
	#define L3_VP_PORT_TBL_IPV6_SRC_UNK_CMD
	#define L3_VP_PORT_TBL_IPV6_SRC_UNK_CMD_OFFSET  23
	#define L3_VP_PORT_TBL_IPV6_SRC_UNK_CMD_LEN     2
	#define L3_VP_PORT_TBL_IPV6_SRC_UNK_CMD_DEFAULT 0x0
	/*[field] IP_ARP_SG_EN*/
	#define L3_VP_PORT_TBL_IP_ARP_SG_EN
	#define L3_VP_PORT_TBL_IP_ARP_SG_EN_OFFSET  25
	#define L3_VP_PORT_TBL_IP_ARP_SG_EN_LEN     1
	#define L3_VP_PORT_TBL_IP_ARP_SG_EN_DEFAULT 0x0
	/*[field] IP_ARP_SG_VIO_CMD*/
	#define L3_VP_PORT_TBL_IP_ARP_SG_VIO_CMD
	#define L3_VP_PORT_TBL_IP_ARP_SG_VIO_CMD_OFFSET  26
	#define L3_VP_PORT_TBL_IP_ARP_SG_VIO_CMD_LEN     2
	#define L3_VP_PORT_TBL_IP_ARP_SG_VIO_CMD_DEFAULT 0x0
	/*[field] IP_ARP_SG_PORT_EN*/
	#define L3_VP_PORT_TBL_IP_ARP_SG_PORT_EN
	#define L3_VP_PORT_TBL_IP_ARP_SG_PORT_EN_OFFSET  28
	#define L3_VP_PORT_TBL_IP_ARP_SG_PORT_EN_LEN     1
	#define L3_VP_PORT_TBL_IP_ARP_SG_PORT_EN_DEFAULT 0x0
	/*[field] IP_ARP_SG_SVLAN_EN*/
	#define L3_VP_PORT_TBL_IP_ARP_SG_SVLAN_EN
	#define L3_VP_PORT_TBL_IP_ARP_SG_SVLAN_EN_OFFSET  29
	#define L3_VP_PORT_TBL_IP_ARP_SG_SVLAN_EN_LEN     1
	#define L3_VP_PORT_TBL_IP_ARP_SG_SVLAN_EN_DEFAULT 0x0
	/*[field] IP_ARP_SRC_UNK_CMD*/
	#define L3_VP_PORT_TBL_IP_ARP_SRC_UNK_CMD
	#define L3_VP_PORT_TBL_IP_ARP_SRC_UNK_CMD_OFFSET  30
	#define L3_VP_PORT_TBL_IP_ARP_SRC_UNK_CMD_LEN     2
	#define L3_VP_PORT_TBL_IP_ARP_SRC_UNK_CMD_DEFAULT 0x0
	/*[field] IP_ARP_SG_CVLAN_EN*/
	#define L3_VP_PORT_TBL_IP_ARP_SG_CVLAN_EN
	#define L3_VP_PORT_TBL_IP_ARP_SG_CVLAN_EN_OFFSET  32
	#define L3_VP_PORT_TBL_IP_ARP_SG_CVLAN_EN_LEN     1
	#define L3_VP_PORT_TBL_IP_ARP_SG_CVLAN_EN_DEFAULT 0x0
	/*[field] IP_ND_SG_EN*/
	#define L3_VP_PORT_TBL_IP_ND_SG_EN
	#define L3_VP_PORT_TBL_IP_ND_SG_EN_OFFSET  33
	#define L3_VP_PORT_TBL_IP_ND_SG_EN_LEN     1
	#define L3_VP_PORT_TBL_IP_ND_SG_EN_DEFAULT 0x0
	/*[field] IP_ND_SG_VIO_CMD*/
	#define L3_VP_PORT_TBL_IP_ND_SG_VIO_CMD
	#define L3_VP_PORT_TBL_IP_ND_SG_VIO_CMD_OFFSET  34
	#define L3_VP_PORT_TBL_IP_ND_SG_VIO_CMD_LEN     2
	#define L3_VP_PORT_TBL_IP_ND_SG_VIO_CMD_DEFAULT 0x0
	/*[field] IP_ND_SG_PORT_EN*/
	#define L3_VP_PORT_TBL_IP_ND_SG_PORT_EN
	#define L3_VP_PORT_TBL_IP_ND_SG_PORT_EN_OFFSET  36
	#define L3_VP_PORT_TBL_IP_ND_SG_PORT_EN_LEN     1
	#define L3_VP_PORT_TBL_IP_ND_SG_PORT_EN_DEFAULT 0x0
	/*[field] IP_ND_SG_SVLAN_EN*/
	#define L3_VP_PORT_TBL_IP_ND_SG_SVLAN_EN
	#define L3_VP_PORT_TBL_IP_ND_SG_SVLAN_EN_OFFSET  37
	#define L3_VP_PORT_TBL_IP_ND_SG_SVLAN_EN_LEN     1
	#define L3_VP_PORT_TBL_IP_ND_SG_SVLAN_EN_DEFAULT 0x0
	/*[field] IP_ND_SG_CVLAN_EN*/
	#define L3_VP_PORT_TBL_IP_ND_SG_CVLAN_EN
	#define L3_VP_PORT_TBL_IP_ND_SG_CVLAN_EN_OFFSET  38
	#define L3_VP_PORT_TBL_IP_ND_SG_CVLAN_EN_LEN     1
	#define L3_VP_PORT_TBL_IP_ND_SG_CVLAN_EN_DEFAULT 0x0
	/*[field] IP_ND_SRC_UNK_CMD*/
	#define L3_VP_PORT_TBL_IP_ND_SRC_UNK_CMD
	#define L3_VP_PORT_TBL_IP_ND_SRC_UNK_CMD_OFFSET  39
	#define L3_VP_PORT_TBL_IP_ND_SRC_UNK_CMD_LEN     2
	#define L3_VP_PORT_TBL_IP_ND_SRC_UNK_CMD_DEFAULT 0x0
	/*[field] VSI_VALID*/
	#define L3_VP_PORT_TBL_VSI_VALID
	#define L3_VP_PORT_TBL_VSI_VALID_OFFSET  41
	#define L3_VP_PORT_TBL_VSI_VALID_LEN     1
	#define L3_VP_PORT_TBL_VSI_VALID_DEFAULT 0x0
	/*[field] VSI*/
	#define L3_VP_PORT_TBL_VSI
	#define L3_VP_PORT_TBL_VSI_OFFSET  42
	#define L3_VP_PORT_TBL_VSI_LEN     5
	#define L3_VP_PORT_TBL_VSI_DEFAULT 0x0
	/*[field] MAC_VALID*/
	#define L3_VP_PORT_TBL_MAC_VALID
	#define L3_VP_PORT_TBL_MAC_VALID_OFFSET  47
	#define L3_VP_PORT_TBL_MAC_VALID_LEN     1
	#define L3_VP_PORT_TBL_MAC_VALID_DEFAULT 0x0
	/*[field] MAC_DA*/
	#define L3_VP_PORT_TBL_MAC_DA
	#define L3_VP_PORT_TBL_MAC_DA_OFFSET  48
	#define L3_VP_PORT_TBL_MAC_DA_LEN     48
	#define L3_VP_PORT_TBL_MAC_DA_DEFAULT 0x0

struct l3_vp_port_tbl {
	a_uint32_t  l3_if_valid:1;
	a_uint32_t  l3_if_index:8;
	a_uint32_t  ipv4_sg_en:1;
	a_uint32_t  ipv4_sg_vio_cmd:2;
	a_uint32_t  ipv4_sg_port_en:1;
	a_uint32_t  ipv4_sg_svlan_en:1;
	a_uint32_t  ipv4_sg_cvlan_en:1;
	a_uint32_t  ipv4_src_unk_cmd:2;
	a_uint32_t  ipv6_sg_en:1;
	a_uint32_t  ipv6_sg_vio_cmd:2;
	a_uint32_t  ipv6_sg_port_en:1;
	a_uint32_t  ipv6_sg_svlan_en:1;
	a_uint32_t  ipv6_sg_cvlan_en:1;
	a_uint32_t  ipv6_src_unk_cmd:2;
	a_uint32_t  ip_arp_sg_en:1;
	a_uint32_t  ip_arp_sg_vio_cmd:2;
	a_uint32_t  ip_arp_sg_port_en:1;
	a_uint32_t  ip_arp_sg_svlan_en:1;
	a_uint32_t  ip_arp_src_unk_cmd:2;
	a_uint32_t  ip_arp_sg_cvlan_en:1;
	a_uint32_t  ip_nd_sg_en:1;
	a_uint32_t  ip_nd_sg_vio_cmd:2;
	a_uint32_t  ip_nd_sg_port_en:1;
	a_uint32_t  ip_nd_sg_svlan_en:1;
	a_uint32_t  ip_nd_sg_cvlan_en:1;
	a_uint32_t  ip_nd_src_unk_cmd:2;
	a_uint32_t  vsi_valid:1;
	a_uint32_t  vsi:5;
	a_uint32_t  mac_valid:1;
	a_uint32_t  mac_da_0:16;
	a_uint32_t  mac_da_1:32;
};

union l3_vp_port_tbl_u {
	a_uint32_t val[3];
	struct l3_vp_port_tbl bf;
};

/*[table] IN_L3_IF_TBL*/
#define IN_L3_IF_TBL
#define IN_L3_IF_TBL_ADDRESS 0x2000
#define IN_L3_IF_TBL_NUM     256
#define IN_L3_IF_TBL_INC     0x8
#define IN_L3_IF_TBL_TYPE    REG_TYPE_RW
#define IN_L3_IF_TBL_DEFAULT 0x0
	/*[field] MRU*/
	#define IN_L3_IF_TBL_MRU
	#define IN_L3_IF_TBL_MRU_OFFSET  0
	#define IN_L3_IF_TBL_MRU_LEN     14
	#define IN_L3_IF_TBL_MRU_DEFAULT 0x0
	/*[field] MTU*/
	#define IN_L3_IF_TBL_MTU
	#define IN_L3_IF_TBL_MTU_OFFSET  14
	#define IN_L3_IF_TBL_MTU_LEN     14
	#define IN_L3_IF_TBL_MTU_DEFAULT 0x0
	/*[field] TTL_DEC_BYPASS*/
	#define IN_L3_IF_TBL_TTL_DEC_BYPASS
	#define IN_L3_IF_TBL_TTL_DEC_BYPASS_OFFSET  28
	#define IN_L3_IF_TBL_TTL_DEC_BYPASS_LEN     1
	#define IN_L3_IF_TBL_TTL_DEC_BYPASS_DEFAULT 0x0
	/*[field] IPV4_UC_ROUTE_EN*/
	#define IN_L3_IF_TBL_IPV4_UC_ROUTE_EN
	#define IN_L3_IF_TBL_IPV4_UC_ROUTE_EN_OFFSET  29
	#define IN_L3_IF_TBL_IPV4_UC_ROUTE_EN_LEN     1
	#define IN_L3_IF_TBL_IPV4_UC_ROUTE_EN_DEFAULT 0x0
	/*[field] IPV6_UC_ROUTE_EN*/
	#define IN_L3_IF_TBL_IPV6_UC_ROUTE_EN
	#define IN_L3_IF_TBL_IPV6_UC_ROUTE_EN_OFFSET  30
	#define IN_L3_IF_TBL_IPV6_UC_ROUTE_EN_LEN     1
	#define IN_L3_IF_TBL_IPV6_UC_ROUTE_EN_DEFAULT 0x0
	/*[field] ICMP_TRIGGER_EN*/
	#define IN_L3_IF_TBL_ICMP_TRIGGER_EN
	#define IN_L3_IF_TBL_ICMP_TRIGGER_EN_OFFSET  31
	#define IN_L3_IF_TBL_ICMP_TRIGGER_EN_LEN     1
	#define IN_L3_IF_TBL_ICMP_TRIGGER_EN_DEFAULT 0x0
	/*[field] TTL_EXCEED_CMD*/
	#define IN_L3_IF_TBL_TTL_EXCEED_CMD
	#define IN_L3_IF_TBL_TTL_EXCEED_CMD_OFFSET  32
	#define IN_L3_IF_TBL_TTL_EXCEED_CMD_LEN     2
	#define IN_L3_IF_TBL_TTL_EXCEED_CMD_DEFAULT 0x0
	/*[field] TTL_EXCEED_DE_ACCE*/
	#define IN_L3_IF_TBL_TTL_EXCEED_DE_ACCE
	#define IN_L3_IF_TBL_TTL_EXCEED_DE_ACCE_OFFSET  34
	#define IN_L3_IF_TBL_TTL_EXCEED_DE_ACCE_LEN     1
	#define IN_L3_IF_TBL_TTL_EXCEED_DE_ACCE_DEFAULT 0x0
	/*[field] MAC_BITMAP*/
	#define IN_L3_IF_TBL_MAC_BITMAP
	#define IN_L3_IF_TBL_MAC_BITMAP_OFFSET  35
	#define IN_L3_IF_TBL_MAC_BITMAP_LEN     8
	#define IN_L3_IF_TBL_MAC_BITMAP_DEFAULT 0x0
	/*[field] PPPOE_EN*/
	#define IN_L3_IF_TBL_PPPOE_EN
	#define IN_L3_IF_TBL_PPPOE_EN_OFFSET  43
	#define IN_L3_IF_TBL_PPPOE_EN_LEN     1
	#define IN_L3_IF_TBL_PPPOE_EN_DEFAULT 0x0

struct in_l3_if_tbl {
	a_uint32_t  mru:14;
	a_uint32_t  mtu:14;
	a_uint32_t  ttl_dec_bypass:1;
	a_uint32_t  ipv4_uc_route_en:1;
	a_uint32_t  ipv6_uc_route_en:1;
	a_uint32_t  icmp_trigger_en:1;
	a_uint32_t  ttl_exceed_cmd:2;
	a_uint32_t  ttl_exceed_de_acce:1;
	a_uint32_t  mac_bitmap:8;
	a_uint32_t  pppoe_en:1;
	a_uint32_t  _reserved0:20;
};

union in_l3_if_tbl_u {
	a_uint32_t val[2];
	struct in_l3_if_tbl bf;
};

/*[table] HOST_IPV6_MCAST_TBL*/
#define HOST_IPV6_MCAST_TBL
#define HOST_IPV6_MCAST_TBL_ADDRESS 0x20000
#define HOST_IPV6_MCAST_TBL_NUM     1536
#define HOST_IPV6_MCAST_TBL_INC     0x40
#define HOST_IPV6_MCAST_TBL_TYPE    REG_TYPE_RW
#define HOST_IPV6_MCAST_TBL_DEFAULT 0x0
	/*[field] VALID*/
	#define HOST_IPV6_MCAST_TBL_VALID
	#define HOST_IPV6_MCAST_TBL_VALID_OFFSET  0
	#define HOST_IPV6_MCAST_TBL_VALID_LEN     1
	#define HOST_IPV6_MCAST_TBL_VALID_DEFAULT 0x0
	/*[field] KEY_TYPE*/
	#define HOST_IPV6_MCAST_TBL_KEY_TYPE
	#define HOST_IPV6_MCAST_TBL_KEY_TYPE_OFFSET  1
	#define HOST_IPV6_MCAST_TBL_KEY_TYPE_LEN     2
	#define HOST_IPV6_MCAST_TBL_KEY_TYPE_DEFAULT 0x0
	/*[field] FWD_CMD*/
	#define HOST_IPV6_MCAST_TBL_FWD_CMD
	#define HOST_IPV6_MCAST_TBL_FWD_CMD_OFFSET  3
	#define HOST_IPV6_MCAST_TBL_FWD_CMD_LEN     2
	#define HOST_IPV6_MCAST_TBL_FWD_CMD_DEFAULT 0x0
	/*[field] SYN_TOGGLE*/
	#define HOST_IPV6_MCAST_TBL_SYN_TOGGLE
	#define HOST_IPV6_MCAST_TBL_SYN_TOGGLE_OFFSET  5
	#define HOST_IPV6_MCAST_TBL_SYN_TOGGLE_LEN     1
	#define HOST_IPV6_MCAST_TBL_SYN_TOGGLE_DEFAULT 0x0
	/*[field] DST_INFO*/
	#define HOST_IPV6_MCAST_TBL_DST_INFO
	#define HOST_IPV6_MCAST_TBL_DST_INFO_OFFSET  6
	#define HOST_IPV6_MCAST_TBL_DST_INFO_LEN     14
	#define HOST_IPV6_MCAST_TBL_DST_INFO_DEFAULT 0x0
	/*[field] LAN_WAN*/
	#define HOST_IPV6_MCAST_TBL_LAN_WAN
	#define HOST_IPV6_MCAST_TBL_LAN_WAN_OFFSET  20
	#define HOST_IPV6_MCAST_TBL_LAN_WAN_LEN     1
	#define HOST_IPV6_MCAST_TBL_LAN_WAN_DEFAULT 0x0
	/*[field] VSI*/
	#define HOST_IPV6_MCAST_TBL_VSI
	#define HOST_IPV6_MCAST_TBL_VSI_OFFSET  21
	#define HOST_IPV6_MCAST_TBL_VSI_LEN     5
	#define HOST_IPV6_MCAST_TBL_VSI_DEFAULT 0x0
	/*[field] SIPV6_ADDR*/
	#define HOST_IPV6_MCAST_TBL_SIPV6_ADDR
	#define HOST_IPV6_MCAST_TBL_SIPV6_ADDR_OFFSET  44
	#define HOST_IPV6_MCAST_TBL_SIPV6_ADDR_LEN     128
	#define HOST_IPV6_MCAST_TBL_SIPV6_ADDR_DEFAULT 0x0
	/*[field] GIPV6_ADDR*/
	#define HOST_IPV6_MCAST_TBL_GIPV6_ADDR
	#define HOST_IPV6_MCAST_TBL_GIPV6_ADDR_OFFSET  172
	#define HOST_IPV6_MCAST_TBL_GIPV6_ADDR_LEN     128
	#define HOST_IPV6_MCAST_TBL_GIPV6_ADDR_DEFAULT 0x0

struct host_tbl {
	a_uint32_t  valid:1;
	a_uint32_t  key_type:2;
	a_uint32_t  fwd_cmd:2;
	a_uint32_t  syn_toggle:1;
	a_uint32_t  dst_info:14;
	a_uint32_t  lan_wan:1;
	a_uint32_t  _reserved0:11;
	a_uint32_t  ip_addr:32;
	a_uint32_t  _reserved1:32;
};

union host_tbl_u {
	a_uint32_t val[3];
	struct host_tbl bf;
};

/*[table] HOST_IPV4_MCAST_TBL*/
#define HOST_IPV4_MCAST_TBL
#define HOST_IPV4_MCAST_TBL_ADDRESS 0x20000
#define HOST_IPV4_MCAST_TBL_NUM     3072
#define HOST_IPV4_MCAST_TBL_INC     0x20
#define HOST_IPV4_MCAST_TBL_TYPE    REG_TYPE_RW
#define HOST_IPV4_MCAST_TBL_DEFAULT 0x0
	/*[field] VALID*/
	#define HOST_IPV4_MCAST_TBL_VALID
	#define HOST_IPV4_MCAST_TBL_VALID_OFFSET  0
	#define HOST_IPV4_MCAST_TBL_VALID_LEN     1
	#define HOST_IPV4_MCAST_TBL_VALID_DEFAULT 0x0
	/*[field] KEY_TYPE*/
	#define HOST_IPV4_MCAST_TBL_KEY_TYPE
	#define HOST_IPV4_MCAST_TBL_KEY_TYPE_OFFSET  1
	#define HOST_IPV4_MCAST_TBL_KEY_TYPE_LEN     2
	#define HOST_IPV4_MCAST_TBL_KEY_TYPE_DEFAULT 0x0
	/*[field] FWD_CMD*/
	#define HOST_IPV4_MCAST_TBL_FWD_CMD
	#define HOST_IPV4_MCAST_TBL_FWD_CMD_OFFSET  3
	#define HOST_IPV4_MCAST_TBL_FWD_CMD_LEN     2
	#define HOST_IPV4_MCAST_TBL_FWD_CMD_DEFAULT 0x0
	/*[field] SYN_TOGGLE*/
	#define HOST_IPV4_MCAST_TBL_SYN_TOGGLE
	#define HOST_IPV4_MCAST_TBL_SYN_TOGGLE_OFFSET  5
	#define HOST_IPV4_MCAST_TBL_SYN_TOGGLE_LEN     1
	#define HOST_IPV4_MCAST_TBL_SYN_TOGGLE_DEFAULT 0x0
	/*[field] DST_INFO*/
	#define HOST_IPV4_MCAST_TBL_DST_INFO
	#define HOST_IPV4_MCAST_TBL_DST_INFO_OFFSET  6
	#define HOST_IPV4_MCAST_TBL_DST_INFO_LEN     14
	#define HOST_IPV4_MCAST_TBL_DST_INFO_DEFAULT 0x0
	/*[field] LAN_WAN*/
	#define HOST_IPV4_MCAST_TBL_LAN_WAN
	#define HOST_IPV4_MCAST_TBL_LAN_WAN_OFFSET  20
	#define HOST_IPV4_MCAST_TBL_LAN_WAN_LEN     1
	#define HOST_IPV4_MCAST_TBL_LAN_WAN_DEFAULT 0x0
	/*[field] VSI*/
	#define HOST_IPV4_MCAST_TBL_VSI
	#define HOST_IPV4_MCAST_TBL_VSI_OFFSET  21
	#define HOST_IPV4_MCAST_TBL_VSI_LEN     5
	#define HOST_IPV4_MCAST_TBL_VSI_DEFAULT 0x0
	/*[field] SIP_ADDR*/
	#define HOST_IPV4_MCAST_TBL_SIP_ADDR
	#define HOST_IPV4_MCAST_TBL_SIP_ADDR_OFFSET  85
	#define HOST_IPV4_MCAST_TBL_SIP_ADDR_LEN     32
	#define HOST_IPV4_MCAST_TBL_SIP_ADDR_DEFAULT 0x0
	/*[field] GIP_ADDR*/
	#define HOST_IPV4_MCAST_TBL_GIP_ADDR
	#define HOST_IPV4_MCAST_TBL_GIP_ADDR_OFFSET  117
	#define HOST_IPV4_MCAST_TBL_GIP_ADDR_LEN     32
	#define HOST_IPV4_MCAST_TBL_GIP_ADDR_DEFAULT 0x0

struct host_ipv6_tbl {
	a_uint32_t  valid:1;
	a_uint32_t  key_type:2;
	a_uint32_t  fwd_cmd:2;
	a_uint32_t  syn_toggle:1;
	a_uint32_t  dst_info:14;
	a_uint32_t  lan_wan:1;
	a_uint32_t  _reserved0:1;
	a_uint32_t  ipv6_addr_0:10;
	a_uint32_t  ipv6_addr_1:32;
	a_uint32_t  ipv6_addr_2:32;
	a_uint32_t  ipv6_addr_3:32;
	a_uint32_t  ipv6_addr_4:22;
	a_uint32_t  _reserved1:10;
};

union host_ipv6_tbl_u {
	a_uint32_t val[5];
	struct host_ipv6_tbl bf;
};

/*[table] HOST_TBL*/
#define HOST_TBL
#define HOST_TBL_ADDRESS 0x20000
#define HOST_TBL_NUM     6144
#define HOST_TBL_INC     0x10
#define HOST_TBL_TYPE    REG_TYPE_RW
#define HOST_TBL_DEFAULT 0x0
	/*[field] VALID*/
	#define HOST_TBL_VALID
	#define HOST_TBL_VALID_OFFSET  0
	#define HOST_TBL_VALID_LEN     1
	#define HOST_TBL_VALID_DEFAULT 0x0
	/*[field] KEY_TYPE*/
	#define HOST_TBL_KEY_TYPE
	#define HOST_TBL_KEY_TYPE_OFFSET  1
	#define HOST_TBL_KEY_TYPE_LEN     2
	#define HOST_TBL_KEY_TYPE_DEFAULT 0x0
	/*[field] FWD_CMD*/
	#define HOST_TBL_FWD_CMD
	#define HOST_TBL_FWD_CMD_OFFSET  3
	#define HOST_TBL_FWD_CMD_LEN     2
	#define HOST_TBL_FWD_CMD_DEFAULT 0x0
	/*[field] SYN_TOGGLE*/
	#define HOST_TBL_SYN_TOGGLE
	#define HOST_TBL_SYN_TOGGLE_OFFSET  5
	#define HOST_TBL_SYN_TOGGLE_LEN     1
	#define HOST_TBL_SYN_TOGGLE_DEFAULT 0x0
	/*[field] DST_INFO*/
	#define HOST_TBL_DST_INFO
	#define HOST_TBL_DST_INFO_OFFSET  6
	#define HOST_TBL_DST_INFO_LEN     14
	#define HOST_TBL_DST_INFO_DEFAULT 0x0
	/*[field] LAN_WAN*/
	#define HOST_TBL_LAN_WAN
	#define HOST_TBL_LAN_WAN_OFFSET  20
	#define HOST_TBL_LAN_WAN_LEN     1
	#define HOST_TBL_LAN_WAN_DEFAULT 0x0
	/*[field] IP_ADDR*/
	#define HOST_TBL_IP_ADDR
	#define HOST_TBL_IP_ADDR_OFFSET  32
	#define HOST_TBL_IP_ADDR_LEN     32
	#define HOST_TBL_IP_ADDR_DEFAULT 0x0

struct host_ipv4_mcast_tbl {
	a_uint32_t  valid:1;
	a_uint32_t  key_type:2;
	a_uint32_t  fwd_cmd:2;
	a_uint32_t  syn_toggle:1;
	a_uint32_t  dst_info:14;
	a_uint32_t  lan_wan:1;
	a_uint32_t  vsi:5;
	a_uint32_t  _reserved0_0:6;
	a_uint32_t  _reserved0_1:32;
	a_uint32_t  _reserved0_2:21;
	a_uint32_t  sip_addr_0:11;
	a_uint32_t  sip_addr_1:21;
	a_uint32_t  gip_addr_0:11;
	a_uint32_t  gip_addr_1:21;
	a_uint32_t  _reserved1:11;
};

union host_ipv4_mcast_tbl_u {
	a_uint32_t val[5];
	struct host_ipv4_mcast_tbl bf;
};

/*[table] HOST_IPV6_TBL*/
#define HOST_IPV6_TBL
#define HOST_IPV6_TBL_ADDRESS 0x20000
#define HOST_IPV6_TBL_NUM     3072
#define HOST_IPV6_TBL_INC     0x20
#define HOST_IPV6_TBL_TYPE    REG_TYPE_RW
#define HOST_IPV6_TBL_DEFAULT 0x0
	/*[field] VALID*/
	#define HOST_IPV6_TBL_VALID
	#define HOST_IPV6_TBL_VALID_OFFSET  0
	#define HOST_IPV6_TBL_VALID_LEN     1
	#define HOST_IPV6_TBL_VALID_DEFAULT 0x0
	/*[field] KEY_TYPE*/
	#define HOST_IPV6_TBL_KEY_TYPE
	#define HOST_IPV6_TBL_KEY_TYPE_OFFSET  1
	#define HOST_IPV6_TBL_KEY_TYPE_LEN     2
	#define HOST_IPV6_TBL_KEY_TYPE_DEFAULT 0x0
	/*[field] FWD_CMD*/
	#define HOST_IPV6_TBL_FWD_CMD
	#define HOST_IPV6_TBL_FWD_CMD_OFFSET  3
	#define HOST_IPV6_TBL_FWD_CMD_LEN     2
	#define HOST_IPV6_TBL_FWD_CMD_DEFAULT 0x0
	/*[field] SYN_TOGGLE*/
	#define HOST_IPV6_TBL_SYN_TOGGLE
	#define HOST_IPV6_TBL_SYN_TOGGLE_OFFSET  5
	#define HOST_IPV6_TBL_SYN_TOGGLE_LEN     1
	#define HOST_IPV6_TBL_SYN_TOGGLE_DEFAULT 0x0
	/*[field] DST_INFO*/
	#define HOST_IPV6_TBL_DST_INFO
	#define HOST_IPV6_TBL_DST_INFO_OFFSET  6
	#define HOST_IPV6_TBL_DST_INFO_LEN     14
	#define HOST_IPV6_TBL_DST_INFO_DEFAULT 0x0
	/*[field] LAN_WAN*/
	#define HOST_IPV6_TBL_LAN_WAN
	#define HOST_IPV6_TBL_LAN_WAN_OFFSET  20
	#define HOST_IPV6_TBL_LAN_WAN_LEN     1
	#define HOST_IPV6_TBL_LAN_WAN_DEFAULT 0x0
	/*[field] IPV6_ADDR*/
	#define HOST_IPV6_TBL_IPV6_ADDR
	#define HOST_IPV6_TBL_IPV6_ADDR_OFFSET  22
	#define HOST_IPV6_TBL_IPV6_ADDR_LEN     128
	#define HOST_IPV6_TBL_IPV6_ADDR_DEFAULT 0x0

struct host_ipv6_mcast_tbl {
	a_uint32_t  valid:1;
	a_uint32_t  key_type:2;
	a_uint32_t  fwd_cmd:2;
	a_uint32_t  syn_toggle:1;
	a_uint32_t  dst_info:14;
	a_uint32_t  lan_wan:1;
	a_uint32_t  vsi:5;
	a_uint32_t  _reserved0_0:6;
	a_uint32_t  _reserved0_1:12;
	a_uint32_t  sipv6_addr_0:20;
	a_uint32_t  sipv6_addr_1:32;
	a_uint32_t  sipv6_addr_2:32;
	a_uint32_t  sipv6_addr_3:32;
	a_uint32_t  sipv6_addr_4:12;
	a_uint32_t  gipv6_addr_0:20;
	a_uint32_t  gipv6_addr_1:32;
	a_uint32_t  gipv6_addr_2:32;
	a_uint32_t  gipv6_addr_3:32;
	a_uint32_t  gipv6_addr_4:12;
	a_uint32_t  _reserved1:20;
};

union host_ipv6_mcast_tbl_u {
	a_uint32_t val[10];
	struct host_ipv6_mcast_tbl bf;
};

/*[table] IN_NEXTHOP_TBL*/
#define IN_NEXTHOP_TBL
#define IN_NEXTHOP_TBL_ADDRESS 0x60000
#define IN_NEXTHOP_TBL_NUM     2560
#define IN_NEXTHOP_TBL_INC     0x10
#define IN_NEXTHOP_TBL_TYPE    REG_TYPE_RW
#define IN_NEXTHOP_TBL_DEFAULT 0x0
	/*[field] TYPE*/
	#define IN_NEXTHOP_TBL_TYPE_F
	#define IN_NEXTHOP_TBL_TYPE_F_OFFSET  0
	#define IN_NEXTHOP_TBL_TYPE_F_LEN     1
	#define IN_NEXTHOP_TBL_TYPE_F_DEFAULT 0x0
	/*[field] PORT reuse TYPE[0]*/
	#define IN_NEXTHOP_TBL_PORT
	#define IN_NEXTHOP_TBL_PORT_OFFSET  1
	#define IN_NEXTHOP_TBL_PORT_LEN     8
	#define IN_NEXTHOP_TBL_PORT_DEFAULT 0x0
	/*[field] VSI reuse TYPE[1]*/
	#define IN_NEXTHOP_TBL_VSI
	#define IN_NEXTHOP_TBL_VSI_OFFSET  1
	#define IN_NEXTHOP_TBL_VSI_LEN     5
	#define IN_NEXTHOP_TBL_VSI_DEFAULT 0x0
	/*[field] POST_L3_IF*/
	#define IN_NEXTHOP_TBL_POST_L3_IF
	#define IN_NEXTHOP_TBL_POST_L3_IF_OFFSET  9
	#define IN_NEXTHOP_TBL_POST_L3_IF_LEN     8
	#define IN_NEXTHOP_TBL_POST_L3_IF_DEFAULT 0x0
	/*[field] IP_TO_ME*/
	#define IN_NEXTHOP_TBL_IP_TO_ME
	#define IN_NEXTHOP_TBL_IP_TO_ME_OFFSET  17
	#define IN_NEXTHOP_TBL_IP_TO_ME_LEN     1
	#define IN_NEXTHOP_TBL_IP_TO_ME_DEFAULT 0x0
	/*[field] STAG_FMT*/
	#define IN_NEXTHOP_TBL_STAG_FMT
	#define IN_NEXTHOP_TBL_STAG_FMT_OFFSET  18
	#define IN_NEXTHOP_TBL_STAG_FMT_LEN     1
	#define IN_NEXTHOP_TBL_STAG_FMT_DEFAULT 0x0
	/*[field] SVID*/
	#define IN_NEXTHOP_TBL_SVID
	#define IN_NEXTHOP_TBL_SVID_OFFSET  19
	#define IN_NEXTHOP_TBL_SVID_LEN     12
	#define IN_NEXTHOP_TBL_SVID_DEFAULT 0x0
	/*[field] CTAG_FMT*/
	#define IN_NEXTHOP_TBL_CTAG_FMT
	#define IN_NEXTHOP_TBL_CTAG_FMT_OFFSET  31
	#define IN_NEXTHOP_TBL_CTAG_FMT_LEN     1
	#define IN_NEXTHOP_TBL_CTAG_FMT_DEFAULT 0x0
	/*[field] CVID*/
	#define IN_NEXTHOP_TBL_CVID
	#define IN_NEXTHOP_TBL_CVID_OFFSET  32
	#define IN_NEXTHOP_TBL_CVID_LEN     12
	#define IN_NEXTHOP_TBL_CVID_DEFAULT 0x0
	/*[field] IP_PUB_ADDR_INDEX*/
	#define IN_NEXTHOP_TBL_IP_PUB_ADDR_INDEX
	#define IN_NEXTHOP_TBL_IP_PUB_ADDR_INDEX_OFFSET  44
	#define IN_NEXTHOP_TBL_IP_PUB_ADDR_INDEX_LEN     4
	#define IN_NEXTHOP_TBL_IP_PUB_ADDR_INDEX_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define IN_NEXTHOP_TBL_MAC_ADDR
	#define IN_NEXTHOP_TBL_MAC_ADDR_OFFSET  48
	#define IN_NEXTHOP_TBL_MAC_ADDR_LEN     48
	#define IN_NEXTHOP_TBL_MAC_ADDR_DEFAULT 0x0
	/*[field] IP_ADDR_DNAT*/
	#define IN_NEXTHOP_TBL_IP_ADDR_DNAT
	#define IN_NEXTHOP_TBL_IP_ADDR_DNAT_OFFSET  96
	#define IN_NEXTHOP_TBL_IP_ADDR_DNAT_LEN     32
	#define IN_NEXTHOP_TBL_IP_ADDR_DNAT_DEFAULT 0x0

struct in_nexthop_tbl_1 {
	a_uint32_t  type:1;
	a_uint32_t  vsi:5;
	a_uint32_t  _reserved0:3;
	a_uint32_t  post_l3_if:8;
	a_uint32_t  ip_to_me:1;
	a_uint32_t  stag_fmt:1;
	a_uint32_t  svid:12;
	a_uint32_t  ctag_fmt:1;
	a_uint32_t  cvid:12;
	a_uint32_t  ip_pub_addr_index:4;
	a_uint32_t  mac_addr_0:16;
	a_uint32_t  mac_addr_1:32;
	a_uint32_t  ip_addr_dnat:32;
};

struct in_nexthop_tbl_0 {
	a_uint32_t  type:1;
	a_uint32_t  port:8;
	a_uint32_t  post_l3_if:8;
	a_uint32_t  ip_to_me:1;
	a_uint32_t  stag_fmt:1;
	a_uint32_t  svid:12;
	a_uint32_t  ctag_fmt:1;
	a_uint32_t  cvid:12;
	a_uint32_t  ip_pub_addr_index:4;
	a_uint32_t  mac_addr_0:16;
	a_uint32_t  mac_addr_1:32;
	a_uint32_t  ip_addr_dnat:32;
};

union in_nexthop_tbl_u {
	a_uint32_t val[4];
	struct in_nexthop_tbl_0 bf0;
	struct in_nexthop_tbl_1 bf1;
};

/*[table] EG_L3_IF_TBL*/
#define EG_L3_IF_TBL
#define EG_L3_IF_TBL_ADDRESS 0xe000
#define EG_L3_IF_TBL_NUM     256
#define EG_L3_IF_TBL_INC     0x10
#define EG_L3_IF_TBL_TYPE    REG_TYPE_RW
#define EG_L3_IF_TBL_DEFAULT 0x0
	/*[field] MAC_ADDR*/
	#define EG_L3_IF_TBL_MAC_ADDR
	#define EG_L3_IF_TBL_MAC_ADDR_OFFSET  0
	#define EG_L3_IF_TBL_MAC_ADDR_LEN     48
	#define EG_L3_IF_TBL_MAC_ADDR_DEFAULT 0x0
	/*[field] SESSION_ID*/
	#define EG_L3_IF_TBL_SESSION_ID
	#define EG_L3_IF_TBL_SESSION_ID_OFFSET  48
	#define EG_L3_IF_TBL_SESSION_ID_LEN     16
	#define EG_L3_IF_TBL_SESSION_ID_DEFAULT 0x0
	/*[field] PPPOE_EN*/
	#define EG_L3_IF_TBL_PPPOE_EN
	#define EG_L3_IF_TBL_PPPOE_EN_OFFSET  64
	#define EG_L3_IF_TBL_PPPOE_EN_LEN     1
	#define EG_L3_IF_TBL_PPPOE_EN_DEFAULT 0x0

struct eg_l3_if_tbl {
	a_uint32_t  mac_addr_0:32;
	a_uint32_t  mac_addr_1:16;
	a_uint32_t  session_id:16;
	a_uint32_t  pppoe_en:1;
	a_uint32_t  _reserved0:31;
};

union eg_l3_if_tbl_u {
	a_uint32_t val[3];
	struct eg_l3_if_tbl bf;
};

/*[table] RT_INTERFACE_CNT_TBL*/
#define RT_INTERFACE_CNT_TBL
#define RT_INTERFACE_CNT_TBL_ADDRESS 0x40000
#define RT_INTERFACE_CNT_TBL_NUM     512
#define RT_INTERFACE_CNT_TBL_INC     0x20
#define RT_INTERFACE_CNT_TBL_TYPE    REG_TYPE_RW
#define RT_INTERFACE_CNT_TBL_DEFAULT 0x0
	/*[field] PKT_CNT*/
	#define RT_INTERFACE_CNT_TBL_PKT_CNT
	#define RT_INTERFACE_CNT_TBL_PKT_CNT_OFFSET  0
	#define RT_INTERFACE_CNT_TBL_PKT_CNT_LEN     32
	#define RT_INTERFACE_CNT_TBL_PKT_CNT_DEFAULT 0x0
	/*[field] BYTE_CNT*/
	#define RT_INTERFACE_CNT_TBL_BYTE_CNT
	#define RT_INTERFACE_CNT_TBL_BYTE_CNT_OFFSET  32
	#define RT_INTERFACE_CNT_TBL_BYTE_CNT_LEN     40
	#define RT_INTERFACE_CNT_TBL_BYTE_CNT_DEFAULT 0x0
	/*[field] DROP_PKT_CNT*/
	#define RT_INTERFACE_CNT_TBL_DROP_PKT_CNT
	#define RT_INTERFACE_CNT_TBL_DROP_PKT_CNT_OFFSET  72
	#define RT_INTERFACE_CNT_TBL_DROP_PKT_CNT_LEN     32
	#define RT_INTERFACE_CNT_TBL_DROP_PKT_CNT_DEFAULT 0x0
	/*[field] DROP_BYTE_CNT*/
	#define RT_INTERFACE_CNT_TBL_DROP_BYTE_CNT
	#define RT_INTERFACE_CNT_TBL_DROP_BYTE_CNT_OFFSET  104
	#define RT_INTERFACE_CNT_TBL_DROP_BYTE_CNT_LEN     40
	#define RT_INTERFACE_CNT_TBL_DROP_BYTE_CNT_DEFAULT 0x0

struct rt_interface_cnt_tbl {
	a_uint32_t  pkt_cnt:32;
	a_uint32_t  byte_cnt_0:32;
	a_uint32_t  byte_cnt_1:8;
	a_uint32_t  drop_pkt_cnt_0:24;
	a_uint32_t  drop_pkt_cnt_1:8;
	a_uint32_t  drop_byte_cnt_0:24;
	a_uint32_t  drop_byte_cnt_1:16;
	a_uint32_t  _reserved0:16;
};

union rt_interface_cnt_tbl_u {
	a_uint32_t val[5];
	struct rt_interface_cnt_tbl bf;
};



#endif
