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
#ifndef HPPE_TRUNK_REG_H
#define HPPE_TRUNK_REG_H

/*[register] TRUNK_HASH_FIELD_REG*/
#define TRUNK_HASH_FIELD_REG
#define TRUNK_HASH_FIELD_REG_ADDRESS 0x68
#define TRUNK_HASH_FIELD_REG_NUM     1
#define TRUNK_HASH_FIELD_REG_INC     0x4
#define TRUNK_HASH_FIELD_REG_TYPE    REG_TYPE_RW
#define TRUNK_HASH_FIELD_REG_DEFAULT 0x0
	/*[field] SRC_PORT_INCL*/
	#define TRUNK_HASH_FIELD_REG_SRC_PORT_INCL
	#define TRUNK_HASH_FIELD_REG_SRC_PORT_INCL_OFFSET  0
	#define TRUNK_HASH_FIELD_REG_SRC_PORT_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_SRC_PORT_INCL_DEFAULT 0x0
	/*[field] MAC_DA_INCL*/
	#define TRUNK_HASH_FIELD_REG_MAC_DA_INCL
	#define TRUNK_HASH_FIELD_REG_MAC_DA_INCL_OFFSET  1
	#define TRUNK_HASH_FIELD_REG_MAC_DA_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_MAC_DA_INCL_DEFAULT 0x0
	/*[field] MAC_SA_INCL*/
	#define TRUNK_HASH_FIELD_REG_MAC_SA_INCL
	#define TRUNK_HASH_FIELD_REG_MAC_SA_INCL_OFFSET  2
	#define TRUNK_HASH_FIELD_REG_MAC_SA_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_MAC_SA_INCL_DEFAULT 0x0
	/*[field] SRC_IP_INCL*/
	#define TRUNK_HASH_FIELD_REG_SRC_IP_INCL
	#define TRUNK_HASH_FIELD_REG_SRC_IP_INCL_OFFSET  3
	#define TRUNK_HASH_FIELD_REG_SRC_IP_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_SRC_IP_INCL_DEFAULT 0x0
	/*[field] DST_IP_INCL*/
	#define TRUNK_HASH_FIELD_REG_DST_IP_INCL
	#define TRUNK_HASH_FIELD_REG_DST_IP_INCL_OFFSET  4
	#define TRUNK_HASH_FIELD_REG_DST_IP_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_DST_IP_INCL_DEFAULT 0x0
	/*[field] L4_SRC_PORT_INCL*/
	#define TRUNK_HASH_FIELD_REG_L4_SRC_PORT_INCL
	#define TRUNK_HASH_FIELD_REG_L4_SRC_PORT_INCL_OFFSET  5
	#define TRUNK_HASH_FIELD_REG_L4_SRC_PORT_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_L4_SRC_PORT_INCL_DEFAULT 0x0
	/*[field] L4_DST_PORT_INCL*/
	#define TRUNK_HASH_FIELD_REG_L4_DST_PORT_INCL
	#define TRUNK_HASH_FIELD_REG_L4_DST_PORT_INCL_OFFSET  6
	#define TRUNK_HASH_FIELD_REG_L4_DST_PORT_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_L4_DST_PORT_INCL_DEFAULT 0x0
	/*[field] UDF0_INCL*/
	#define TRUNK_HASH_FIELD_REG_UDF0_INCL
	#define TRUNK_HASH_FIELD_REG_UDF0_INCL_OFFSET  7
	#define TRUNK_HASH_FIELD_REG_UDF0_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_UDF0_INCL_DEFAULT 0x0
	/*[field] UDF1_INCL*/
	#define TRUNK_HASH_FIELD_REG_UDF1_INCL
	#define TRUNK_HASH_FIELD_REG_UDF1_INCL_OFFSET  8
	#define TRUNK_HASH_FIELD_REG_UDF1_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_UDF1_INCL_DEFAULT 0x0
	/*[field] UDF2_INCL*/
	#define TRUNK_HASH_FIELD_REG_UDF2_INCL
	#define TRUNK_HASH_FIELD_REG_UDF2_INCL_OFFSET  9
	#define TRUNK_HASH_FIELD_REG_UDF2_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_UDF2_INCL_DEFAULT 0x0
	/*[field] UDF3_INCL*/
	#define TRUNK_HASH_FIELD_REG_UDF3_INCL
	#define TRUNK_HASH_FIELD_REG_UDF3_INCL_OFFSET  10
	#define TRUNK_HASH_FIELD_REG_UDF3_INCL_LEN     1
	#define TRUNK_HASH_FIELD_REG_UDF3_INCL_DEFAULT 0x0

struct trunk_hash_field_reg {
	a_uint32_t  src_port_incl:1;
	a_uint32_t  mac_da_incl:1;
	a_uint32_t  mac_sa_incl:1;
	a_uint32_t  src_ip_incl:1;
	a_uint32_t  dst_ip_incl:1;
	a_uint32_t  l4_src_port_incl:1;
	a_uint32_t  l4_dst_port_incl:1;
	a_uint32_t  udf0_incl:1;
	a_uint32_t  udf1_incl:1;
	a_uint32_t  udf2_incl:1;
	a_uint32_t  udf3_incl:1;
	a_uint32_t  _reserved0:21;
};

union trunk_hash_field_reg_u {
	a_uint32_t val;
	struct trunk_hash_field_reg bf;
};

/*[register] TRUNK_FILTER*/
#define TRUNK_FILTER
#define TRUNK_FILTER_ADDRESS 0x50
#define TRUNK_FILTER_NUM     2
#define TRUNK_FILTER_INC     0x4
#define TRUNK_FILTER_TYPE    REG_TYPE_RW
#define TRUNK_FILTER_DEFAULT 0x0
	/*[field] MEM_BITMAP*/
	#define TRUNK_FILTER_MEM_BITMAP
	#define TRUNK_FILTER_MEM_BITMAP_OFFSET  0
	#define TRUNK_FILTER_MEM_BITMAP_LEN     8
	#define TRUNK_FILTER_MEM_BITMAP_DEFAULT 0x0

struct trunk_filter {
	a_uint32_t  mem_bitmap:8;
	a_uint32_t  _reserved0:24;
};

union trunk_filter_u {
	a_uint32_t val;
	struct trunk_filter bf;
};

/*[register] TRUNK_MEMBER*/
#define TRUNK_MEMBER
#define TRUNK_MEMBER_ADDRESS 0x60
#define TRUNK_MEMBER_NUM     2
#define TRUNK_MEMBER_INC     0x4
#define TRUNK_MEMBER_TYPE    REG_TYPE_RW
#define TRUNK_MEMBER_DEFAULT 0x0
	/*[field] MEMBER_0_PORT_ID*/
	#define TRUNK_MEMBER_MEMBER_0_PORT_ID
	#define TRUNK_MEMBER_MEMBER_0_PORT_ID_OFFSET  0
	#define TRUNK_MEMBER_MEMBER_0_PORT_ID_LEN     3
	#define TRUNK_MEMBER_MEMBER_0_PORT_ID_DEFAULT 0x0
	/*[field] MEMBER_1_PORT_ID*/
	#define TRUNK_MEMBER_MEMBER_1_PORT_ID
	#define TRUNK_MEMBER_MEMBER_1_PORT_ID_OFFSET  4
	#define TRUNK_MEMBER_MEMBER_1_PORT_ID_LEN     3
	#define TRUNK_MEMBER_MEMBER_1_PORT_ID_DEFAULT 0x0
	/*[field] MEMBER_2_PORT_ID*/
	#define TRUNK_MEMBER_MEMBER_2_PORT_ID
	#define TRUNK_MEMBER_MEMBER_2_PORT_ID_OFFSET  8
	#define TRUNK_MEMBER_MEMBER_2_PORT_ID_LEN     3
	#define TRUNK_MEMBER_MEMBER_2_PORT_ID_DEFAULT 0x0
	/*[field] MEMBER_3_PORT_ID*/
	#define TRUNK_MEMBER_MEMBER_3_PORT_ID
	#define TRUNK_MEMBER_MEMBER_3_PORT_ID_OFFSET  12
	#define TRUNK_MEMBER_MEMBER_3_PORT_ID_LEN     3
	#define TRUNK_MEMBER_MEMBER_3_PORT_ID_DEFAULT 0x0
	/*[field] MEMBER_4_PORT_ID*/
	#define TRUNK_MEMBER_MEMBER_4_PORT_ID
	#define TRUNK_MEMBER_MEMBER_4_PORT_ID_OFFSET  16
	#define TRUNK_MEMBER_MEMBER_4_PORT_ID_LEN     3
	#define TRUNK_MEMBER_MEMBER_4_PORT_ID_DEFAULT 0x0
	/*[field] MEMBER_5_PORT_ID*/
	#define TRUNK_MEMBER_MEMBER_5_PORT_ID
	#define TRUNK_MEMBER_MEMBER_5_PORT_ID_OFFSET  20
	#define TRUNK_MEMBER_MEMBER_5_PORT_ID_LEN     3
	#define TRUNK_MEMBER_MEMBER_5_PORT_ID_DEFAULT 0x0
	/*[field] MEMBER_6_PORT_ID*/
	#define TRUNK_MEMBER_MEMBER_6_PORT_ID
	#define TRUNK_MEMBER_MEMBER_6_PORT_ID_OFFSET  24
	#define TRUNK_MEMBER_MEMBER_6_PORT_ID_LEN     3
	#define TRUNK_MEMBER_MEMBER_6_PORT_ID_DEFAULT 0x0
	/*[field] MEMBER_7_PORT_ID*/
	#define TRUNK_MEMBER_MEMBER_7_PORT_ID
	#define TRUNK_MEMBER_MEMBER_7_PORT_ID_OFFSET  28
	#define TRUNK_MEMBER_MEMBER_7_PORT_ID_LEN     3
	#define TRUNK_MEMBER_MEMBER_7_PORT_ID_DEFAULT 0x0

struct trunk_member {
	a_uint32_t  member_0_port_id:3;
	a_uint32_t  _reserved0:1;
	a_uint32_t  member_1_port_id:3;
	a_uint32_t  _reserved1:1;
	a_uint32_t  member_2_port_id:3;
	a_uint32_t  _reserved2:1;
	a_uint32_t  member_3_port_id:3;
	a_uint32_t  _reserved3:1;
	a_uint32_t  member_4_port_id:3;
	a_uint32_t  _reserved4:1;
	a_uint32_t  member_5_port_id:3;
	a_uint32_t  _reserved5:1;
	a_uint32_t  member_6_port_id:3;
	a_uint32_t  _reserved6:1;
	a_uint32_t  member_7_port_id:3;
	a_uint32_t  _reserved7:1;
};

union trunk_member_u {
	a_uint32_t val;
	struct trunk_member bf;
};

/*[register] PORT_TRUNK_ID*/
#define PORT_TRUNK_ID
#define PORT_TRUNK_ID_ADDRESS 0x600
#define PORT_TRUNK_ID_NUM     8
#define PORT_TRUNK_ID_INC     0x4
#define PORT_TRUNK_ID_TYPE    REG_TYPE_RW
#define PORT_TRUNK_ID_DEFAULT 0x0
	/*[field] TRUNK_EN*/
	#define PORT_TRUNK_ID_TRUNK_EN
	#define PORT_TRUNK_ID_TRUNK_EN_OFFSET  0
	#define PORT_TRUNK_ID_TRUNK_EN_LEN     1
	#define PORT_TRUNK_ID_TRUNK_EN_DEFAULT 0x0
	/*[field] TRUNK_ID*/
	#define PORT_TRUNK_ID_TRUNK_ID
	#define PORT_TRUNK_ID_TRUNK_ID_OFFSET  1
	#define PORT_TRUNK_ID_TRUNK_ID_LEN     1
	#define PORT_TRUNK_ID_TRUNK_ID_DEFAULT 0x0

struct port_trunk_id {
	a_uint32_t  trunk_en:1;
	a_uint32_t  trunk_id:1;
	a_uint32_t  _reserved0:30;
};

union port_trunk_id_u {
	a_uint32_t val;
	struct port_trunk_id bf;
};

#endif
