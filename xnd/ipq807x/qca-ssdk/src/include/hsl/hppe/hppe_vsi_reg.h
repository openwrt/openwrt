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
#ifndef HPPE_VSI_REG_H
#define HPPE_VSI_REG_H

/*[table] VSI_TBL*/
#define VSI_TBL
#define VSI_TBL_ADDRESS 0x1800
#define VSI_TBL_NUM     32
#define VSI_TBL_INC     0x10
#define VSI_TBL_TYPE    REG_TYPE_RW
#define VSI_TBL_DEFAULT 0x0
	/*[field] MEMBER_PORT_BITMAP*/
	#define VSI_TBL_MEMBER_PORT_BITMAP
	#define VSI_TBL_MEMBER_PORT_BITMAP_OFFSET  0
	#define VSI_TBL_MEMBER_PORT_BITMAP_LEN     8
	#define VSI_TBL_MEMBER_PORT_BITMAP_DEFAULT 0x0
	/*[field] UUC_BITMAP*/
	#define VSI_TBL_UUC_BITMAP
	#define VSI_TBL_UUC_BITMAP_OFFSET  8
	#define VSI_TBL_UUC_BITMAP_LEN     8
	#define VSI_TBL_UUC_BITMAP_DEFAULT 0x0
	/*[field] UMC_BITMAP*/
	#define VSI_TBL_UMC_BITMAP
	#define VSI_TBL_UMC_BITMAP_OFFSET  16
	#define VSI_TBL_UMC_BITMAP_LEN     8
	#define VSI_TBL_UMC_BITMAP_DEFAULT 0x0
	/*[field] BC_BITMAP*/
	#define VSI_TBL_BC_BITMAP
	#define VSI_TBL_BC_BITMAP_OFFSET  24
	#define VSI_TBL_BC_BITMAP_LEN     8
	#define VSI_TBL_BC_BITMAP_DEFAULT 0x0
	/*[field] NEW_ADDR_LRN_EN*/
	#define VSI_TBL_NEW_ADDR_LRN_EN
	#define VSI_TBL_NEW_ADDR_LRN_EN_OFFSET  32
	#define VSI_TBL_NEW_ADDR_LRN_EN_LEN     1
	#define VSI_TBL_NEW_ADDR_LRN_EN_DEFAULT 0x0
	/*[field] NEW_ADDR_FWD_CMD*/
	#define VSI_TBL_NEW_ADDR_FWD_CMD
	#define VSI_TBL_NEW_ADDR_FWD_CMD_OFFSET  33
	#define VSI_TBL_NEW_ADDR_FWD_CMD_LEN     2
	#define VSI_TBL_NEW_ADDR_FWD_CMD_DEFAULT 0x0
	/*[field] STATION_MOVE_LRN_EN*/
	#define VSI_TBL_STATION_MOVE_LRN_EN
	#define VSI_TBL_STATION_MOVE_LRN_EN_OFFSET  35
	#define VSI_TBL_STATION_MOVE_LRN_EN_LEN     1
	#define VSI_TBL_STATION_MOVE_LRN_EN_DEFAULT 0x0
	/*[field] STATION_MOVE_FWD_CMD*/
	#define VSI_TBL_STATION_MOVE_FWD_CMD
	#define VSI_TBL_STATION_MOVE_FWD_CMD_OFFSET  36
	#define VSI_TBL_STATION_MOVE_FWD_CMD_LEN     2
	#define VSI_TBL_STATION_MOVE_FWD_CMD_DEFAULT 0x0

struct vsi_tbl {
	a_uint32_t  member_port_bitmap:8;
	a_uint32_t  uuc_bitmap:8;
	a_uint32_t  umc_bitmap:8;
	a_uint32_t  bc_bitmap:8;
	a_uint32_t  new_addr_lrn_en:1;
	a_uint32_t  new_addr_fwd_cmd:2;
	a_uint32_t  station_move_lrn_en:1;
	a_uint32_t  station_move_fwd_cmd:2;
	a_uint32_t  _reserved0:26;
};

union vsi_tbl_u {
	a_uint32_t val[2];
	struct vsi_tbl bf;
};

/*[table] VLAN_CNT_TBL*/
#define VLAN_CNT_TBL
#define VLAN_CNT_TBL_ADDRESS 0x78000
#define VLAN_CNT_TBL_NUM     32
#define VLAN_CNT_TBL_INC     0x10
#define VLAN_CNT_TBL_TYPE    REG_TYPE_RW
#define VLAN_CNT_TBL_DEFAULT 0x0
	/*[field] RX_PKT_CNT*/
	#define VLAN_CNT_TBL_RX_PKT_CNT
	#define VLAN_CNT_TBL_RX_PKT_CNT_OFFSET  0
	#define VLAN_CNT_TBL_RX_PKT_CNT_LEN     32
	#define VLAN_CNT_TBL_RX_PKT_CNT_DEFAULT 0x0
	/*[field] RX_BYTE_CNT*/
	#define VLAN_CNT_TBL_RX_BYTE_CNT
	#define VLAN_CNT_TBL_RX_BYTE_CNT_OFFSET  32
	#define VLAN_CNT_TBL_RX_BYTE_CNT_LEN     40
	#define VLAN_CNT_TBL_RX_BYTE_CNT_DEFAULT 0x0

struct vlan_cnt_tbl {
	a_uint32_t  rx_pkt_cnt:32;
	a_uint32_t  rx_byte_cnt_0:32;
	a_uint32_t  rx_byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union vlan_cnt_tbl_u {
	a_uint32_t val[3];
	struct vlan_cnt_tbl bf;
};

/*[table] EG_VSI_COUNTER_TBL*/
#define EG_VSI_COUNTER_TBL
#define EG_VSI_COUNTER_TBL_ADDRESS 0x600
#define EG_VSI_COUNTER_TBL_NUM     32
#define EG_VSI_COUNTER_TBL_INC     0x10
#define EG_VSI_COUNTER_TBL_TYPE    REG_TYPE_RW
#define EG_VSI_COUNTER_TBL_DEFAULT 0x0
	/*[field] TX_PACKETS*/
	#define EG_VSI_COUNTER_TBL_TX_PACKETS
	#define EG_VSI_COUNTER_TBL_TX_PACKETS_OFFSET  0
	#define EG_VSI_COUNTER_TBL_TX_PACKETS_LEN     32
	#define EG_VSI_COUNTER_TBL_TX_PACKETS_DEFAULT 0x0
	/*[field] TX_BYTES*/
	#define EG_VSI_COUNTER_TBL_TX_BYTES
	#define EG_VSI_COUNTER_TBL_TX_BYTES_OFFSET  32
	#define EG_VSI_COUNTER_TBL_TX_BYTES_LEN     40
	#define EG_VSI_COUNTER_TBL_TX_BYTES_DEFAULT 0x0

struct eg_vsi_counter_tbl {
	a_uint32_t  tx_packets:32;
	a_uint32_t  tx_bytes_0:32;
	a_uint32_t  tx_bytes_1:8;
	a_uint32_t  _reserved0:24;
};

union eg_vsi_counter_tbl_u {
	a_uint32_t val[3];
	struct eg_vsi_counter_tbl bf;
};

/*[table] PRE_L2_CNT_TBL*/
#define PRE_L2_CNT_TBL
#define PRE_L2_CNT_TBL_ADDRESS 0x7c000
#define PRE_L2_CNT_TBL_NUM     32
#define PRE_L2_CNT_TBL_INC     0x20
#define PRE_L2_CNT_TBL_TYPE    REG_TYPE_RW
#define PRE_L2_CNT_TBL_DEFAULT 0x0
	/*[field] RX_PKT_CNT*/
	#define PRE_L2_CNT_TBL_RX_PKT_CNT
	#define PRE_L2_CNT_TBL_RX_PKT_CNT_OFFSET  0
	#define PRE_L2_CNT_TBL_RX_PKT_CNT_LEN     32
	#define PRE_L2_CNT_TBL_RX_PKT_CNT_DEFAULT 0x0
	/*[field] RX_BYTE_CNT*/
	#define PRE_L2_CNT_TBL_RX_BYTE_CNT
	#define PRE_L2_CNT_TBL_RX_BYTE_CNT_OFFSET  32
	#define PRE_L2_CNT_TBL_RX_BYTE_CNT_LEN     40
	#define PRE_L2_CNT_TBL_RX_BYTE_CNT_DEFAULT 0x0
	/*[field] RX_DROP_PKT_CNT*/
	#define PRE_L2_CNT_TBL_RX_DROP_PKT_CNT
	#define PRE_L2_CNT_TBL_RX_DROP_PKT_CNT_OFFSET  72
	#define PRE_L2_CNT_TBL_RX_DROP_PKT_CNT_LEN     32
	#define PRE_L2_CNT_TBL_RX_DROP_PKT_CNT_DEFAULT 0x0
	/*[field] RX_DROP_BYTE_CNT*/
	#define PRE_L2_CNT_TBL_RX_DROP_BYTE_CNT
	#define PRE_L2_CNT_TBL_RX_DROP_BYTE_CNT_OFFSET  104
	#define PRE_L2_CNT_TBL_RX_DROP_BYTE_CNT_LEN     40
	#define PRE_L2_CNT_TBL_RX_DROP_BYTE_CNT_DEFAULT 0x0

struct pre_l2_cnt_tbl {
	a_uint32_t  rx_pkt_cnt:32;
	a_uint32_t  rx_byte_cnt_0:32;
	a_uint32_t  rx_byte_cnt_1:8;
	a_uint32_t  rx_drop_pkt_cnt_0:24;
	a_uint32_t  rx_drop_pkt_cnt_1:8;
	a_uint32_t  rx_drop_byte_cnt_0:24;
	a_uint32_t  rx_drop_byte_cnt_1:16;
	a_uint32_t  _reserved0:16;
};

union pre_l2_cnt_tbl_u {
	a_uint32_t val[5];
	struct pre_l2_cnt_tbl bf;
};

#endif
