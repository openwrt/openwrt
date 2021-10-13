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
#ifndef HPPE_ACL_REG_H
#define HPPE_ACL_REG_H

/*[register] NON_IP_UDF0_CTRL_REG*/
#define NON_IP_UDF0_CTRL_REG
#define NON_IP_UDF0_CTRL_REG_ADDRESS 0x38
#define NON_IP_UDF0_CTRL_REG_NUM     1
#define NON_IP_UDF0_CTRL_REG_INC     0x4
#define NON_IP_UDF0_CTRL_REG_TYPE    REG_TYPE_RW
#define NON_IP_UDF0_CTRL_REG_DEFAULT 0x0
	/*[field] UDF0_BASE*/
	#define NON_IP_UDF0_CTRL_REG_UDF0_BASE
	#define NON_IP_UDF0_CTRL_REG_UDF0_BASE_OFFSET  0
	#define NON_IP_UDF0_CTRL_REG_UDF0_BASE_LEN     2
	#define NON_IP_UDF0_CTRL_REG_UDF0_BASE_DEFAULT 0x0
	/*[field] UDF0_OFFSET*/
	#define NON_IP_UDF0_CTRL_REG_UDF0_OFFSET
	#define NON_IP_UDF0_CTRL_REG_UDF0_OFFSET_OFFSET  8
	#define NON_IP_UDF0_CTRL_REG_UDF0_OFFSET_LEN     6
	#define NON_IP_UDF0_CTRL_REG_UDF0_OFFSET_DEFAULT 0x0

struct udf_ctrl_reg {
	a_uint32_t  udf_base:2;
	a_uint32_t  _reserved0:6;
	a_uint32_t  udf_offset:6;
	a_uint32_t  _reserved1:18;
};

union udf_ctrl_reg_u {
	a_uint32_t val;
	struct udf_ctrl_reg bf;
};

/*[register] NON_IP_UDF1_CTRL_REG*/
#define NON_IP_UDF1_CTRL_REG
#define NON_IP_UDF1_CTRL_REG_ADDRESS 0x3c
#define NON_IP_UDF1_CTRL_REG_NUM     1
#define NON_IP_UDF1_CTRL_REG_INC     0x4
#define NON_IP_UDF1_CTRL_REG_TYPE    REG_TYPE_RW
#define NON_IP_UDF1_CTRL_REG_DEFAULT 0x0
	/*[field] UDF1_BASE*/
	#define NON_IP_UDF1_CTRL_REG_UDF1_BASE
	#define NON_IP_UDF1_CTRL_REG_UDF1_BASE_OFFSET  0
	#define NON_IP_UDF1_CTRL_REG_UDF1_BASE_LEN     2
	#define NON_IP_UDF1_CTRL_REG_UDF1_BASE_DEFAULT 0x0
	/*[field] UDF1_OFFSET*/
	#define NON_IP_UDF1_CTRL_REG_UDF1_OFFSET
	#define NON_IP_UDF1_CTRL_REG_UDF1_OFFSET_OFFSET  8
	#define NON_IP_UDF1_CTRL_REG_UDF1_OFFSET_LEN     6
	#define NON_IP_UDF1_CTRL_REG_UDF1_OFFSET_DEFAULT 0x0

/*[register] NON_IP_UDF2_CTRL_REG*/
#define NON_IP_UDF2_CTRL_REG
#define NON_IP_UDF2_CTRL_REG_ADDRESS 0x40
#define NON_IP_UDF2_CTRL_REG_NUM     1
#define NON_IP_UDF2_CTRL_REG_INC     0x4
#define NON_IP_UDF2_CTRL_REG_TYPE    REG_TYPE_RW
#define NON_IP_UDF2_CTRL_REG_DEFAULT 0x0
	/*[field] UDF2_BASE*/
	#define NON_IP_UDF2_CTRL_REG_UDF2_BASE
	#define NON_IP_UDF2_CTRL_REG_UDF2_BASE_OFFSET  0
	#define NON_IP_UDF2_CTRL_REG_UDF2_BASE_LEN     2
	#define NON_IP_UDF2_CTRL_REG_UDF2_BASE_DEFAULT 0x0
	/*[field] UDF2_OFFSET*/
	#define NON_IP_UDF2_CTRL_REG_UDF2_OFFSET
	#define NON_IP_UDF2_CTRL_REG_UDF2_OFFSET_OFFSET  8
	#define NON_IP_UDF2_CTRL_REG_UDF2_OFFSET_LEN     6
	#define NON_IP_UDF2_CTRL_REG_UDF2_OFFSET_DEFAULT 0x0

/*[register] NON_IP_UDF3_CTRL_REG*/
#define NON_IP_UDF3_CTRL_REG
#define NON_IP_UDF3_CTRL_REG_ADDRESS 0x44
#define NON_IP_UDF3_CTRL_REG_NUM     1
#define NON_IP_UDF3_CTRL_REG_INC     0x4
#define NON_IP_UDF3_CTRL_REG_TYPE    REG_TYPE_RW
#define NON_IP_UDF3_CTRL_REG_DEFAULT 0x0
	/*[field] UDF3_BASE*/
	#define NON_IP_UDF3_CTRL_REG_UDF3_BASE
	#define NON_IP_UDF3_CTRL_REG_UDF3_BASE_OFFSET  0
	#define NON_IP_UDF3_CTRL_REG_UDF3_BASE_LEN     2
	#define NON_IP_UDF3_CTRL_REG_UDF3_BASE_DEFAULT 0x0
	/*[field] UDF3_OFFSET*/
	#define NON_IP_UDF3_CTRL_REG_UDF3_OFFSET
	#define NON_IP_UDF3_CTRL_REG_UDF3_OFFSET_OFFSET  8
	#define NON_IP_UDF3_CTRL_REG_UDF3_OFFSET_LEN     6
	#define NON_IP_UDF3_CTRL_REG_UDF3_OFFSET_DEFAULT 0x0

/*[register] IPV4_UDF0_CTRL_REG*/
#define IPV4_UDF0_CTRL_REG
#define IPV4_UDF0_CTRL_REG_ADDRESS 0x48
#define IPV4_UDF0_CTRL_REG_NUM     1
#define IPV4_UDF0_CTRL_REG_INC     0x4
#define IPV4_UDF0_CTRL_REG_TYPE    REG_TYPE_RW
#define IPV4_UDF0_CTRL_REG_DEFAULT 0x0
	/*[field] UDF0_BASE*/
	#define IPV4_UDF0_CTRL_REG_UDF0_BASE
	#define IPV4_UDF0_CTRL_REG_UDF0_BASE_OFFSET  0
	#define IPV4_UDF0_CTRL_REG_UDF0_BASE_LEN     2
	#define IPV4_UDF0_CTRL_REG_UDF0_BASE_DEFAULT 0x0
	/*[field] UDF0_OFFSET*/
	#define IPV4_UDF0_CTRL_REG_UDF0_OFFSET
	#define IPV4_UDF0_CTRL_REG_UDF0_OFFSET_OFFSET  8
	#define IPV4_UDF0_CTRL_REG_UDF0_OFFSET_LEN     6
	#define IPV4_UDF0_CTRL_REG_UDF0_OFFSET_DEFAULT 0x0

/*[register] IPV4_UDF1_CTRL_REG*/
#define IPV4_UDF1_CTRL_REG
#define IPV4_UDF1_CTRL_REG_ADDRESS 0x4c
#define IPV4_UDF1_CTRL_REG_NUM     1
#define IPV4_UDF1_CTRL_REG_INC     0x4
#define IPV4_UDF1_CTRL_REG_TYPE    REG_TYPE_RW
#define IPV4_UDF1_CTRL_REG_DEFAULT 0x0
	/*[field] UDF1_BASE*/
	#define IPV4_UDF1_CTRL_REG_UDF1_BASE
	#define IPV4_UDF1_CTRL_REG_UDF1_BASE_OFFSET  0
	#define IPV4_UDF1_CTRL_REG_UDF1_BASE_LEN     2
	#define IPV4_UDF1_CTRL_REG_UDF1_BASE_DEFAULT 0x0
	/*[field] UDF1_OFFSET*/
	#define IPV4_UDF1_CTRL_REG_UDF1_OFFSET
	#define IPV4_UDF1_CTRL_REG_UDF1_OFFSET_OFFSET  8
	#define IPV4_UDF1_CTRL_REG_UDF1_OFFSET_LEN     6
	#define IPV4_UDF1_CTRL_REG_UDF1_OFFSET_DEFAULT 0x0

/*[register] IPV4_UDF2_CTRL_REG*/
#define IPV4_UDF2_CTRL_REG
#define IPV4_UDF2_CTRL_REG_ADDRESS 0x50
#define IPV4_UDF2_CTRL_REG_NUM     1
#define IPV4_UDF2_CTRL_REG_INC     0x4
#define IPV4_UDF2_CTRL_REG_TYPE    REG_TYPE_RW
#define IPV4_UDF2_CTRL_REG_DEFAULT 0x0
	/*[field] UDF2_BASE*/
	#define IPV4_UDF2_CTRL_REG_UDF2_BASE
	#define IPV4_UDF2_CTRL_REG_UDF2_BASE_OFFSET  0
	#define IPV4_UDF2_CTRL_REG_UDF2_BASE_LEN     2
	#define IPV4_UDF2_CTRL_REG_UDF2_BASE_DEFAULT 0x0
	/*[field] UDF2_OFFSET*/
	#define IPV4_UDF2_CTRL_REG_UDF2_OFFSET
	#define IPV4_UDF2_CTRL_REG_UDF2_OFFSET_OFFSET  8
	#define IPV4_UDF2_CTRL_REG_UDF2_OFFSET_LEN     6
	#define IPV4_UDF2_CTRL_REG_UDF2_OFFSET_DEFAULT 0x0

/*[register] IPV4_UDF3_CTRL_REG*/
#define IPV4_UDF3_CTRL_REG
#define IPV4_UDF3_CTRL_REG_ADDRESS 0x54
#define IPV4_UDF3_CTRL_REG_NUM     1
#define IPV4_UDF3_CTRL_REG_INC     0x4
#define IPV4_UDF3_CTRL_REG_TYPE    REG_TYPE_RW
#define IPV4_UDF3_CTRL_REG_DEFAULT 0x0
	/*[field] UDF3_BASE*/
	#define IPV4_UDF3_CTRL_REG_UDF3_BASE
	#define IPV4_UDF3_CTRL_REG_UDF3_BASE_OFFSET  0
	#define IPV4_UDF3_CTRL_REG_UDF3_BASE_LEN     2
	#define IPV4_UDF3_CTRL_REG_UDF3_BASE_DEFAULT 0x0
	/*[field] UDF3_OFFSET*/
	#define IPV4_UDF3_CTRL_REG_UDF3_OFFSET
	#define IPV4_UDF3_CTRL_REG_UDF3_OFFSET_OFFSET  8
	#define IPV4_UDF3_CTRL_REG_UDF3_OFFSET_LEN     6
	#define IPV4_UDF3_CTRL_REG_UDF3_OFFSET_DEFAULT 0x0

/*[register] IPV6_UDF0_CTRL_REG*/
#define IPV6_UDF0_CTRL_REG
#define IPV6_UDF0_CTRL_REG_ADDRESS 0x58
#define IPV6_UDF0_CTRL_REG_NUM     1
#define IPV6_UDF0_CTRL_REG_INC     0x4
#define IPV6_UDF0_CTRL_REG_TYPE    REG_TYPE_RW
#define IPV6_UDF0_CTRL_REG_DEFAULT 0x0
	/*[field] UDF0_BASE*/
	#define IPV6_UDF0_CTRL_REG_UDF0_BASE
	#define IPV6_UDF0_CTRL_REG_UDF0_BASE_OFFSET  0
	#define IPV6_UDF0_CTRL_REG_UDF0_BASE_LEN     2
	#define IPV6_UDF0_CTRL_REG_UDF0_BASE_DEFAULT 0x0
	/*[field] UDF0_OFFSET*/
	#define IPV6_UDF0_CTRL_REG_UDF0_OFFSET
	#define IPV6_UDF0_CTRL_REG_UDF0_OFFSET_OFFSET  8
	#define IPV6_UDF0_CTRL_REG_UDF0_OFFSET_LEN     6
	#define IPV6_UDF0_CTRL_REG_UDF0_OFFSET_DEFAULT 0x0

/*[register] IPV6_UDF1_CTRL_REG*/
#define IPV6_UDF1_CTRL_REG
#define IPV6_UDF1_CTRL_REG_ADDRESS 0x5c
#define IPV6_UDF1_CTRL_REG_NUM     1
#define IPV6_UDF1_CTRL_REG_INC     0x4
#define IPV6_UDF1_CTRL_REG_TYPE    REG_TYPE_RW
#define IPV6_UDF1_CTRL_REG_DEFAULT 0x0
	/*[field] UDF1_BASE*/
	#define IPV6_UDF1_CTRL_REG_UDF1_BASE
	#define IPV6_UDF1_CTRL_REG_UDF1_BASE_OFFSET  0
	#define IPV6_UDF1_CTRL_REG_UDF1_BASE_LEN     2
	#define IPV6_UDF1_CTRL_REG_UDF1_BASE_DEFAULT 0x0
	/*[field] UDF1_OFFSET*/
	#define IPV6_UDF1_CTRL_REG_UDF1_OFFSET
	#define IPV6_UDF1_CTRL_REG_UDF1_OFFSET_OFFSET  8
	#define IPV6_UDF1_CTRL_REG_UDF1_OFFSET_LEN     6
	#define IPV6_UDF1_CTRL_REG_UDF1_OFFSET_DEFAULT 0x0

/*[register] IPV6_UDF2_CTRL_REG*/
#define IPV6_UDF2_CTRL_REG
#define IPV6_UDF2_CTRL_REG_ADDRESS 0x60
#define IPV6_UDF2_CTRL_REG_NUM     1
#define IPV6_UDF2_CTRL_REG_INC     0x4
#define IPV6_UDF2_CTRL_REG_TYPE    REG_TYPE_RW
#define IPV6_UDF2_CTRL_REG_DEFAULT 0x0
	/*[field] UDF2_BASE*/
	#define IPV6_UDF2_CTRL_REG_UDF2_BASE
	#define IPV6_UDF2_CTRL_REG_UDF2_BASE_OFFSET  0
	#define IPV6_UDF2_CTRL_REG_UDF2_BASE_LEN     2
	#define IPV6_UDF2_CTRL_REG_UDF2_BASE_DEFAULT 0x0
	/*[field] UDF2_OFFSET*/
	#define IPV6_UDF2_CTRL_REG_UDF2_OFFSET
	#define IPV6_UDF2_CTRL_REG_UDF2_OFFSET_OFFSET  8
	#define IPV6_UDF2_CTRL_REG_UDF2_OFFSET_LEN     6
	#define IPV6_UDF2_CTRL_REG_UDF2_OFFSET_DEFAULT 0x0

/*[register] IPV6_UDF3_CTRL_REG*/
#define IPV6_UDF3_CTRL_REG
#define IPV6_UDF3_CTRL_REG_ADDRESS 0x64
#define IPV6_UDF3_CTRL_REG_NUM     1
#define IPV6_UDF3_CTRL_REG_INC     0x4
#define IPV6_UDF3_CTRL_REG_TYPE    REG_TYPE_RW
#define IPV6_UDF3_CTRL_REG_DEFAULT 0x0
	/*[field] UDF3_BASE*/
	#define IPV6_UDF3_CTRL_REG_UDF3_BASE
	#define IPV6_UDF3_CTRL_REG_UDF3_BASE_OFFSET  0
	#define IPV6_UDF3_CTRL_REG_UDF3_BASE_LEN     2
	#define IPV6_UDF3_CTRL_REG_UDF3_BASE_DEFAULT 0x0
	/*[field] UDF3_OFFSET*/
	#define IPV6_UDF3_CTRL_REG_UDF3_OFFSET
	#define IPV6_UDF3_CTRL_REG_UDF3_OFFSET_OFFSET  8
	#define IPV6_UDF3_CTRL_REG_UDF3_OFFSET_LEN     6
	#define IPV6_UDF3_CTRL_REG_UDF3_OFFSET_DEFAULT 0x0

/*[table] IPO_RULE_REG*/
#define IPO_RULE_REG
#define IPO_RULE_REG_ADDRESS 0x0
#define IPO_RULE_REG_NUM     512
#define IPO_RULE_REG_INC     0x10
#define IPO_RULE_REG_TYPE    REG_TYPE_RW
#define IPO_RULE_REG_DEFAULT 0x0
	/*[field] RULE_FIELD*/
	#define IPO_RULE_REG_RULE_FIELD
	#define IPO_RULE_REG_RULE_FIELD_OFFSET  0
	#define IPO_RULE_REG_RULE_FIELD_LEN     52
	#define IPO_RULE_REG_RULE_FIELD_DEFAULT 0x0
	/*[field] FAKE_MAC_HEADER*/
	#define IPO_RULE_REG_FAKE_MAC_HEADER
	#define IPO_RULE_REG_FAKE_MAC_HEADER_OFFSET  52
	#define IPO_RULE_REG_FAKE_MAC_HEADER_LEN     1
	#define IPO_RULE_REG_FAKE_MAC_HEADER_DEFAULT 0x0
	/*[field] RANGE_EN*/
	#define IPO_RULE_REG_RANGE_EN
	#define IPO_RULE_REG_RANGE_EN_OFFSET  53
	#define IPO_RULE_REG_RANGE_EN_LEN     1
	#define IPO_RULE_REG_RANGE_EN_DEFAULT 0x0
	/*[field] INVERSE_EN*/
	#define IPO_RULE_REG_INVERSE_EN
	#define IPO_RULE_REG_INVERSE_EN_OFFSET  54
	#define IPO_RULE_REG_INVERSE_EN_LEN     1
	#define IPO_RULE_REG_INVERSE_EN_DEFAULT 0x0
	/*[field] RULE_TYPE*/
	#define IPO_RULE_REG_RULE_TYPE
	#define IPO_RULE_REG_RULE_TYPE_OFFSET  55
	#define IPO_RULE_REG_RULE_TYPE_LEN     4
	#define IPO_RULE_REG_RULE_TYPE_DEFAULT 0x0
	/*[field] SRC_TYPE*/
	#define IPO_RULE_REG_SRC_TYPE
	#define IPO_RULE_REG_SRC_TYPE_OFFSET  59
	#define IPO_RULE_REG_SRC_TYPE_LEN     2
	#define IPO_RULE_REG_SRC_TYPE_DEFAULT 0x0
	/*[field] SRC*/
	#define IPO_RULE_REG_SRC
	#define IPO_RULE_REG_SRC_OFFSET  61
	#define IPO_RULE_REG_SRC_LEN     8
	#define IPO_RULE_REG_SRC_DEFAULT 0x0
	/*[field] PRI*/
	#define IPO_RULE_REG_PRI
	#define IPO_RULE_REG_PRI_OFFSET  69
	#define IPO_RULE_REG_PRI_LEN     9
	#define IPO_RULE_REG_PRI_DEFAULT 0x0
	/*[field] RES_CHAIN*/
	#define IPO_RULE_REG_RES_CHAIN
	#define IPO_RULE_REG_RES_CHAIN_OFFSET  78
	#define IPO_RULE_REG_RES_CHAIN_LEN     1
	#define IPO_RULE_REG_RES_CHAIN_DEFAULT 0x0
	/*[field] POST_ROUTING_EN*/
	#define IPO_RULE_REG_POST_ROUTING_EN
	#define IPO_RULE_REG_POST_ROUTING_EN_OFFSET  79
	#define IPO_RULE_REG_POST_ROUTING_EN_LEN     1
	#define IPO_RULE_REG_POST_ROUTING_EN_DEFAULT 0x0

struct ipo_rule_reg {
	a_uint32_t  rule_field_0:32;
	a_uint32_t  rule_field_1:20;
	a_uint32_t  fake_mac_header:1;
	a_uint32_t  range_en:1;
	a_uint32_t  inverse_en:1;
	a_uint32_t  rule_type:4;
	a_uint32_t  src_type:2;
	a_uint32_t  src_0:3;
	a_uint32_t  src_1:5;
	a_uint32_t  pri:9;
	a_uint32_t  res_chain:1;
	a_uint32_t  post_routing_en:1;
	a_uint32_t  _reserved0:16;
};

union ipo_rule_reg_u {
	a_uint32_t val[3];
	struct ipo_rule_reg bf;
};

/*[table] IPO_MASK_REG*/
#define IPO_MASK_REG
#define IPO_MASK_REG_ADDRESS 0x2000
#define IPO_MASK_REG_NUM     512
#define IPO_MASK_REG_INC     0x10
#define IPO_MASK_REG_TYPE    REG_TYPE_RW
#define IPO_MASK_REG_DEFAULT 0x0
	/*[field] MASKFIELD*/
	#define IPO_MASK_REG_MASKFIELD
	#define IPO_MASK_REG_MASKFIELD_OFFSET  0
	#define IPO_MASK_REG_MASKFIELD_LEN     53
	#define IPO_MASK_REG_MASKFIELD_DEFAULT 0x0

struct ipo_mask_reg {
	a_uint32_t  maskfield_0:32;
	a_uint32_t  maskfield_1:21;
	a_uint32_t  _reserved0:11;
};

union ipo_mask_reg_u {
	a_uint32_t val[2];
	struct ipo_mask_reg bf;
};

/*[register] RULE_EXT_1_REG*/
#define RULE_EXT_1_REG
#define RULE_EXT_1_REG_ADDRESS 0x4000
#define RULE_EXT_1_REG_NUM     64
#define RULE_EXT_1_REG_INC     0x4
#define RULE_EXT_1_REG_TYPE    REG_TYPE_RW
#define RULE_EXT_1_REG_DEFAULT 0x0
	/*[field] EXT2_0*/
	#define RULE_EXT_1_REG_EXT2_0
	#define RULE_EXT_1_REG_EXT2_0_OFFSET  0
	#define RULE_EXT_1_REG_EXT2_0_LEN     1
	#define RULE_EXT_1_REG_EXT2_0_DEFAULT 0x0
	/*[field] EXT2_1*/
	#define RULE_EXT_1_REG_EXT2_1
	#define RULE_EXT_1_REG_EXT2_1_OFFSET  1
	#define RULE_EXT_1_REG_EXT2_1_LEN     1
	#define RULE_EXT_1_REG_EXT2_1_DEFAULT 0x0
	/*[field] EXT2_2*/
	#define RULE_EXT_1_REG_EXT2_2
	#define RULE_EXT_1_REG_EXT2_2_OFFSET  2
	#define RULE_EXT_1_REG_EXT2_2_LEN     1
	#define RULE_EXT_1_REG_EXT2_2_DEFAULT 0x0
	/*[field] EXT2_3*/
	#define RULE_EXT_1_REG_EXT2_3
	#define RULE_EXT_1_REG_EXT2_3_OFFSET  3
	#define RULE_EXT_1_REG_EXT2_3_LEN     1
	#define RULE_EXT_1_REG_EXT2_3_DEFAULT 0x0

struct rule_ext_1_reg {
	a_uint32_t  ext2_0:1;
	a_uint32_t  ext2_1:1;
	a_uint32_t  ext2_2:1;
	a_uint32_t  ext2_3:1;
	a_uint32_t  _reserved0:28;
};

union rule_ext_1_reg_u {
	a_uint32_t val;
	struct rule_ext_1_reg bf;
};

/*[register] RULE_EXT_2_REG*/
#define RULE_EXT_2_REG
#define RULE_EXT_2_REG_ADDRESS 0x4100
#define RULE_EXT_2_REG_NUM     64
#define RULE_EXT_2_REG_INC     0x4
#define RULE_EXT_2_REG_TYPE    REG_TYPE_RW
#define RULE_EXT_2_REG_DEFAULT 0x0
	/*[field] EXT4_0*/
	#define RULE_EXT_2_REG_EXT4_0
	#define RULE_EXT_2_REG_EXT4_0_OFFSET  0
	#define RULE_EXT_2_REG_EXT4_0_LEN     1
	#define RULE_EXT_2_REG_EXT4_0_DEFAULT 0x0
	/*[field] EXT4_1*/
	#define RULE_EXT_2_REG_EXT4_1
	#define RULE_EXT_2_REG_EXT4_1_OFFSET  1
	#define RULE_EXT_2_REG_EXT4_1_LEN     1
	#define RULE_EXT_2_REG_EXT4_1_DEFAULT 0x0

struct rule_ext_2_reg {
	a_uint32_t  ext4_0:1;
	a_uint32_t  ext4_1:1;
	a_uint32_t  _reserved0:30;
};

union rule_ext_2_reg_u {
	a_uint32_t val;
	struct rule_ext_2_reg bf;
};

/*[register] RULE_EXT_4_REG*/
#define RULE_EXT_4_REG
#define RULE_EXT_4_REG_ADDRESS 0x4200
#define RULE_EXT_4_REG_NUM     64
#define RULE_EXT_4_REG_INC     0x4
#define RULE_EXT_4_REG_TYPE    REG_TYPE_RW
#define RULE_EXT_4_REG_DEFAULT 0x0
	/*[field] EXT8*/
	#define RULE_EXT_4_REG_EXT8
	#define RULE_EXT_4_REG_EXT8_OFFSET  0
	#define RULE_EXT_4_REG_EXT8_LEN     1
	#define RULE_EXT_4_REG_EXT8_DEFAULT 0x0

struct rule_ext_4_reg {
	a_uint32_t  ext8:1;
	a_uint32_t  _reserved0:31;
};

union rule_ext_4_reg_u {
	a_uint32_t val;
	struct rule_ext_4_reg bf;
};

/*[register] IPO_DBG_ADDR_REG*/
#define IPO_DBG_ADDR_REG
#define IPO_DBG_ADDR_REG_ADDRESS 0x4300
#define IPO_DBG_ADDR_REG_NUM     1
#define IPO_DBG_ADDR_REG_INC     0x4
#define IPO_DBG_ADDR_REG_TYPE    REG_TYPE_RW
#define IPO_DBG_ADDR_REG_DEFAULT 0x0
	/*[field] IPO_DBG_ADDR*/
	#define IPO_DBG_ADDR_REG_IPO_DBG_ADDR
	#define IPO_DBG_ADDR_REG_IPO_DBG_ADDR_OFFSET  0
	#define IPO_DBG_ADDR_REG_IPO_DBG_ADDR_LEN     32
	#define IPO_DBG_ADDR_REG_IPO_DBG_ADDR_DEFAULT 0x0

struct ipo_dbg_addr_reg {
	a_uint32_t  ipo_dbg_addr:32;
};

union ipo_dbg_addr_reg_u {
	a_uint32_t val;
	struct ipo_dbg_addr_reg bf;
};

/*[register] IPO_DBG_DATA_REG*/
#define IPO_DBG_DATA_REG
#define IPO_DBG_DATA_REG_ADDRESS 0x4304
#define IPO_DBG_DATA_REG_NUM     1
#define IPO_DBG_DATA_REG_INC     0x4
#define IPO_DBG_DATA_REG_TYPE    REG_TYPE_RO
#define IPO_DBG_DATA_REG_DEFAULT 0x0
	/*[field] IPO_DBG_DATA*/
	#define IPO_DBG_DATA_REG_IPO_DBG_DATA
	#define IPO_DBG_DATA_REG_IPO_DBG_DATA_OFFSET  0
	#define IPO_DBG_DATA_REG_IPO_DBG_DATA_LEN     32
	#define IPO_DBG_DATA_REG_IPO_DBG_DATA_DEFAULT 0x0

struct ipo_dbg_data_reg {
	a_uint32_t  ipo_dbg_data:32;
};

union ipo_dbg_data_reg_u {
	a_uint32_t val;
	struct ipo_dbg_data_reg bf;
};

/*[register] IPO_SPARE_REG_REG*/
#define IPO_SPARE_REG_REG
#define IPO_SPARE_REG_REG_ADDRESS 0x4308
#define IPO_SPARE_REG_REG_NUM     1
#define IPO_SPARE_REG_REG_INC     0x4
#define IPO_SPARE_REG_REG_TYPE    REG_TYPE_RW
#define IPO_SPARE_REG_REG_DEFAULT 0x0
	/*[field] SPARE_REG*/
	#define IPO_SPARE_REG_REG_SPARE_REG
	#define IPO_SPARE_REG_REG_SPARE_REG_OFFSET  0
	#define IPO_SPARE_REG_REG_SPARE_REG_LEN     32
	#define IPO_SPARE_REG_REG_SPARE_REG_DEFAULT 0x0

struct ipo_spare_reg_reg {
	a_uint32_t  spare_reg:32;
};

union ipo_spare_reg_reg_u {
	a_uint32_t val;
	struct ipo_spare_reg_reg bf;
};

/*[register] IPO_GLB_HIT_COUNTER_REG*/
#define IPO_GLB_HIT_COUNTER_REG
#define IPO_GLB_HIT_COUNTER_REG_ADDRESS 0x430c
#define IPO_GLB_HIT_COUNTER_REG_NUM     1
#define IPO_GLB_HIT_COUNTER_REG_INC     0x4
#define IPO_GLB_HIT_COUNTER_REG_TYPE    REG_TYPE_RO
#define IPO_GLB_HIT_COUNTER_REG_DEFAULT 0x0
	/*[field] HIT_COUNT*/
	#define IPO_GLB_HIT_COUNTER_REG_HIT_COUNT
	#define IPO_GLB_HIT_COUNTER_REG_HIT_COUNT_OFFSET  0
	#define IPO_GLB_HIT_COUNTER_REG_HIT_COUNT_LEN     32
	#define IPO_GLB_HIT_COUNTER_REG_HIT_COUNT_DEFAULT 0x0

struct ipo_glb_hit_counter_reg {
	a_uint32_t  hit_count:32;
};

union ipo_glb_hit_counter_reg_u {
	a_uint32_t val;
	struct ipo_glb_hit_counter_reg bf;
};

/*[register] IPO_GLB_MISS_COUNTER_REG*/
#define IPO_GLB_MISS_COUNTER_REG
#define IPO_GLB_MISS_COUNTER_REG_ADDRESS 0x4310
#define IPO_GLB_MISS_COUNTER_REG_NUM     1
#define IPO_GLB_MISS_COUNTER_REG_INC     0x4
#define IPO_GLB_MISS_COUNTER_REG_TYPE    REG_TYPE_RO
#define IPO_GLB_MISS_COUNTER_REG_DEFAULT 0x0
	/*[field] MISS_COUNT*/
	#define IPO_GLB_MISS_COUNTER_REG_MISS_COUNT
	#define IPO_GLB_MISS_COUNTER_REG_MISS_COUNT_OFFSET  0
	#define IPO_GLB_MISS_COUNTER_REG_MISS_COUNT_LEN     32
	#define IPO_GLB_MISS_COUNTER_REG_MISS_COUNT_DEFAULT 0x0

struct ipo_glb_miss_counter_reg {
	a_uint32_t  miss_count:32;
};

union ipo_glb_miss_counter_reg_u {
	a_uint32_t val;
	struct ipo_glb_miss_counter_reg bf;
};

/*[register] IPO_GLB_BYPASS_COUNTER_REG*/
#define IPO_GLB_BYPASS_COUNTER_REG
#define IPO_GLB_BYPASS_COUNTER_REG_ADDRESS 0x4314
#define IPO_GLB_BYPASS_COUNTER_REG_NUM     1
#define IPO_GLB_BYPASS_COUNTER_REG_INC     0x4
#define IPO_GLB_BYPASS_COUNTER_REG_TYPE    REG_TYPE_RO
#define IPO_GLB_BYPASS_COUNTER_REG_DEFAULT 0x0
	/*[field] BYPASS_COUNT*/
	#define IPO_GLB_BYPASS_COUNTER_REG_BYPASS_COUNT
	#define IPO_GLB_BYPASS_COUNTER_REG_BYPASS_COUNT_OFFSET  0
	#define IPO_GLB_BYPASS_COUNTER_REG_BYPASS_COUNT_LEN     32
	#define IPO_GLB_BYPASS_COUNTER_REG_BYPASS_COUNT_DEFAULT 0x0

struct ipo_glb_bypass_counter_reg {
	a_uint32_t  bypass_count:32;
};

union ipo_glb_bypass_counter_reg_u {
	a_uint32_t val;
	struct ipo_glb_bypass_counter_reg bf;
};

/*[table] IPO_CNT_TBL*/
#define IPO_CNT_TBL
#define IPO_CNT_TBL_ADDRESS 0x74000
#define IPO_CNT_TBL_NUM     512
#define IPO_CNT_TBL_INC     0x10
#define IPO_CNT_TBL_TYPE    REG_TYPE_RW
#define IPO_CNT_TBL_DEFAULT 0x0
	/*[field] HIT_PKT_CNT*/
	#define IPO_CNT_TBL_HIT_PKT_CNT
	#define IPO_CNT_TBL_HIT_PKT_CNT_OFFSET  0
	#define IPO_CNT_TBL_HIT_PKT_CNT_LEN     32
	#define IPO_CNT_TBL_HIT_PKT_CNT_DEFAULT 0x0
	/*[field] HIT_BYTE_CNT*/
	#define IPO_CNT_TBL_HIT_BYTE_CNT
	#define IPO_CNT_TBL_HIT_BYTE_CNT_OFFSET  32
	#define IPO_CNT_TBL_HIT_BYTE_CNT_LEN     40
	#define IPO_CNT_TBL_HIT_BYTE_CNT_DEFAULT 0x0

struct ipo_cnt_tbl {
	a_uint32_t  hit_pkt_cnt:32;
	a_uint32_t  hit_byte_cnt_0:32;
	a_uint32_t  hit_byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union ipo_cnt_tbl_u {
	a_uint32_t val[3];
	struct ipo_cnt_tbl bf;
};

/*[table] IPO_ACTION*/
#define IPO_ACTION
#define IPO_ACTION_ADDRESS 0x8000
#define IPO_ACTION_NUM     512
#define IPO_ACTION_INC     0x20
#define IPO_ACTION_TYPE    REG_TYPE_RW
#define IPO_ACTION_DEFAULT 0x0
	/*[field] DEST_INFO_CHANGE_EN*/
	#define IPO_ACTION_DEST_INFO_CHANGE_EN
	#define IPO_ACTION_DEST_INFO_CHANGE_EN_OFFSET  0
	#define IPO_ACTION_DEST_INFO_CHANGE_EN_LEN     1
	#define IPO_ACTION_DEST_INFO_CHANGE_EN_DEFAULT 0x0
	/*[field] FWD_CMD*/
	#define IPO_ACTION_FWD_CMD
	#define IPO_ACTION_FWD_CMD_OFFSET  1
	#define IPO_ACTION_FWD_CMD_LEN     2
	#define IPO_ACTION_FWD_CMD_DEFAULT 0x0
	/*[field] DEST_INFO*/
	#define IPO_ACTION_DEST_INFO
	#define IPO_ACTION_DEST_INFO_OFFSET  3
	#define IPO_ACTION_DEST_INFO_LEN     14
	#define IPO_ACTION_DEST_INFO_DEFAULT 0x0
	/*[field] MIRROR_EN*/
	#define IPO_ACTION_MIRROR_EN
	#define IPO_ACTION_MIRROR_EN_OFFSET  17
	#define IPO_ACTION_MIRROR_EN_LEN     1
	#define IPO_ACTION_MIRROR_EN_DEFAULT 0x0
	/*[field] BYPASS_BITMAP*/
	#define IPO_ACTION_BYPASS_BITMAP
	#define IPO_ACTION_BYPASS_BITMAP_OFFSET  18
	#define IPO_ACTION_BYPASS_BITMAP_LEN     32
	#define IPO_ACTION_BYPASS_BITMAP_DEFAULT 0x0
	/*[field] SVID_CHANGE_EN*/
	#define IPO_ACTION_SVID_CHANGE_EN
	#define IPO_ACTION_SVID_CHANGE_EN_OFFSET  50
	#define IPO_ACTION_SVID_CHANGE_EN_LEN     1
	#define IPO_ACTION_SVID_CHANGE_EN_DEFAULT 0x0
	/*[field] STAG_FMT*/
	#define IPO_ACTION_STAG_FMT
	#define IPO_ACTION_STAG_FMT_OFFSET  51
	#define IPO_ACTION_STAG_FMT_LEN     1
	#define IPO_ACTION_STAG_FMT_DEFAULT 0x0
	/*[field] SVID*/
	#define IPO_ACTION_SVID
	#define IPO_ACTION_SVID_OFFSET  52
	#define IPO_ACTION_SVID_LEN     12
	#define IPO_ACTION_SVID_DEFAULT 0x0
	/*[field] CVID_CHANGE_EN*/
	#define IPO_ACTION_CVID_CHANGE_EN
	#define IPO_ACTION_CVID_CHANGE_EN_OFFSET  64
	#define IPO_ACTION_CVID_CHANGE_EN_LEN     1
	#define IPO_ACTION_CVID_CHANGE_EN_DEFAULT 0x0
	/*[field] CTAG_FMT*/
	#define IPO_ACTION_CTAG_FMT
	#define IPO_ACTION_CTAG_FMT_OFFSET  65
	#define IPO_ACTION_CTAG_FMT_LEN     1
	#define IPO_ACTION_CTAG_FMT_DEFAULT 0x0
	/*[field] CVID*/
	#define IPO_ACTION_CVID
	#define IPO_ACTION_CVID_OFFSET  66
	#define IPO_ACTION_CVID_LEN     12
	#define IPO_ACTION_CVID_DEFAULT 0x0
	/*[field] DSCP_TC_CHANGE_EN*/
	#define IPO_ACTION_DSCP_TC_CHANGE_EN
	#define IPO_ACTION_DSCP_TC_CHANGE_EN_OFFSET  78
	#define IPO_ACTION_DSCP_TC_CHANGE_EN_LEN     1
	#define IPO_ACTION_DSCP_TC_CHANGE_EN_DEFAULT 0x0
	/*[field] DSCP_TC*/
	#define IPO_ACTION_DSCP_TC
	#define IPO_ACTION_DSCP_TC_OFFSET  79
	#define IPO_ACTION_DSCP_TC_LEN     8
	#define IPO_ACTION_DSCP_TC_DEFAULT 0x0
	/*[field] STAG_PCP_CHANGE_EN*/
	#define IPO_ACTION_STAG_PCP_CHANGE_EN
	#define IPO_ACTION_STAG_PCP_CHANGE_EN_OFFSET  87
	#define IPO_ACTION_STAG_PCP_CHANGE_EN_LEN     1
	#define IPO_ACTION_STAG_PCP_CHANGE_EN_DEFAULT 0x0
	/*[field] STAG_PCP*/
	#define IPO_ACTION_STAG_PCP
	#define IPO_ACTION_STAG_PCP_OFFSET  88
	#define IPO_ACTION_STAG_PCP_LEN     3
	#define IPO_ACTION_STAG_PCP_DEFAULT 0x0
	/*[field] STAG_DEI_CHANGE_EN*/
	#define IPO_ACTION_STAG_DEI_CHANGE_EN
	#define IPO_ACTION_STAG_DEI_CHANGE_EN_OFFSET  91
	#define IPO_ACTION_STAG_DEI_CHANGE_EN_LEN     1
	#define IPO_ACTION_STAG_DEI_CHANGE_EN_DEFAULT 0x0
	/*[field] STAG_DEI*/
	#define IPO_ACTION_STAG_DEI
	#define IPO_ACTION_STAG_DEI_OFFSET  92
	#define IPO_ACTION_STAG_DEI_LEN     1
	#define IPO_ACTION_STAG_DEI_DEFAULT 0x0
	/*[field] CTAG_PCP_CHANGE_EN*/
	#define IPO_ACTION_CTAG_PCP_CHANGE_EN
	#define IPO_ACTION_CTAG_PCP_CHANGE_EN_OFFSET  93
	#define IPO_ACTION_CTAG_PCP_CHANGE_EN_LEN     1
	#define IPO_ACTION_CTAG_PCP_CHANGE_EN_DEFAULT 0x0
	/*[field] CTAG_PCP*/
	#define IPO_ACTION_CTAG_PCP
	#define IPO_ACTION_CTAG_PCP_OFFSET  94
	#define IPO_ACTION_CTAG_PCP_LEN     3
	#define IPO_ACTION_CTAG_PCP_DEFAULT 0x0
	/*[field] CTAG_DEI_CHANGE_EN*/
	#define IPO_ACTION_CTAG_DEI_CHANGE_EN
	#define IPO_ACTION_CTAG_DEI_CHANGE_EN_OFFSET  97
	#define IPO_ACTION_CTAG_DEI_CHANGE_EN_LEN     1
	#define IPO_ACTION_CTAG_DEI_CHANGE_EN_DEFAULT 0x0
	/*[field] CTAG_DEI*/
	#define IPO_ACTION_CTAG_DEI
	#define IPO_ACTION_CTAG_DEI_OFFSET  98
	#define IPO_ACTION_CTAG_DEI_LEN     1
	#define IPO_ACTION_CTAG_DEI_DEFAULT 0x0
	/*[field] ENQUEUE_PRI_CHANGE_EN*/
	#define IPO_ACTION_ENQUEUE_PRI_CHANGE_EN
	#define IPO_ACTION_ENQUEUE_PRI_CHANGE_EN_OFFSET  99
	#define IPO_ACTION_ENQUEUE_PRI_CHANGE_EN_LEN     1
	#define IPO_ACTION_ENQUEUE_PRI_CHANGE_EN_DEFAULT 0x0
	/*[field] ENQUEUE_PRI*/
	#define IPO_ACTION_ENQUEUE_PRI
	#define IPO_ACTION_ENQUEUE_PRI_OFFSET  100
	#define IPO_ACTION_ENQUEUE_PRI_LEN     4
	#define IPO_ACTION_ENQUEUE_PRI_DEFAULT 0x0
	/*[field] INT_DP_CHANGE_EN*/
	#define IPO_ACTION_INT_DP_CHANGE_EN
	#define IPO_ACTION_INT_DP_CHANGE_EN_OFFSET  104
	#define IPO_ACTION_INT_DP_CHANGE_EN_LEN     1
	#define IPO_ACTION_INT_DP_CHANGE_EN_DEFAULT 0x0
	/*[field] INT_DP*/
	#define IPO_ACTION_INT_DP
	#define IPO_ACTION_INT_DP_OFFSET  105
	#define IPO_ACTION_INT_DP_LEN     2
	#define IPO_ACTION_INT_DP_DEFAULT 0x0
	/*[field] POLICER_EN*/
	#define IPO_ACTION_POLICER_EN
	#define IPO_ACTION_POLICER_EN_OFFSET  107
	#define IPO_ACTION_POLICER_EN_LEN     1
	#define IPO_ACTION_POLICER_EN_DEFAULT 0x0
	/*[field] POLICER_INDEX*/
	#define IPO_ACTION_POLICER_INDEX
	#define IPO_ACTION_POLICER_INDEX_OFFSET  108
	#define IPO_ACTION_POLICER_INDEX_LEN     9
	#define IPO_ACTION_POLICER_INDEX_DEFAULT 0x0
	/*[field] QID_EN*/
	#define IPO_ACTION_QID_EN
	#define IPO_ACTION_QID_EN_OFFSET  117
	#define IPO_ACTION_QID_EN_LEN     1
	#define IPO_ACTION_QID_EN_DEFAULT 0x0
	/*[field] QID*/
	#define IPO_ACTION_QID
	#define IPO_ACTION_QID_OFFSET  118
	#define IPO_ACTION_QID_LEN     8
	#define IPO_ACTION_QID_DEFAULT 0x0
	/*[field] SERVICE_CODE_EN*/
	#define IPO_ACTION_SERVICE_CODE_EN
	#define IPO_ACTION_SERVICE_CODE_EN_OFFSET  126
	#define IPO_ACTION_SERVICE_CODE_EN_LEN     1
	#define IPO_ACTION_SERVICE_CODE_EN_DEFAULT 0x0
	/*[field] SERVICE_CODE*/
	#define IPO_ACTION_SERVICE_CODE
	#define IPO_ACTION_SERVICE_CODE_OFFSET  127
	#define IPO_ACTION_SERVICE_CODE_LEN     8
	#define IPO_ACTION_SERVICE_CODE_DEFAULT 0x0
	/*[field] SYN_TOGGLE*/
	#define IPO_ACTION_SYN_TOGGLE
	#define IPO_ACTION_SYN_TOGGLE_OFFSET  135
	#define IPO_ACTION_SYN_TOGGLE_LEN     1
	#define IPO_ACTION_SYN_TOGGLE_DEFAULT 0x0
	/*[field] CPU_CODE_EN*/
	#define IPO_ACTION_CPU_CODE_EN
	#define IPO_ACTION_CPU_CODE_EN_OFFSET  136
	#define IPO_ACTION_CPU_CODE_EN_LEN     1
	#define IPO_ACTION_CPU_CODE_EN_DEFAULT 0x0
	/*[field] CPU_CODE*/
	#define IPO_ACTION_CPU_CODE
	#define IPO_ACTION_CPU_CODE_OFFSET  137
	#define IPO_ACTION_CPU_CODE_LEN     8
	#define IPO_ACTION_CPU_CODE_DEFAULT 0x0
	/*[field] METADATA_EN*/
	#define IPO_ACTION_METADATA_EN
	#define IPO_ACTION_METADATA_EN_OFFSET  145
	#define IPO_ACTION_METADATA_EN_LEN     1
	#define IPO_ACTION_METADATA_EN_DEFAULT 0x0

struct ipo_action {
	a_uint32_t  dest_info_change_en:1;
	a_uint32_t  fwd_cmd:2;
	a_uint32_t  dest_info:14;
	a_uint32_t  mirror_en:1;
	a_uint32_t  bypass_bitmap_0:14;
	a_uint32_t  bypass_bitmap_1:18;
	a_uint32_t  svid_change_en:1;
	a_uint32_t  stag_fmt:1;
	a_uint32_t  svid:12;
	a_uint32_t  cvid_change_en:1;
	a_uint32_t  ctag_fmt:1;
	a_uint32_t  cvid:12;
	a_uint32_t  dscp_tc_change_en:1;
	a_uint32_t  dscp_tc:8;
	a_uint32_t  stag_pcp_change_en:1;
	a_uint32_t  stag_pcp:3;
	a_uint32_t  stag_dei_change_en:1;
	a_uint32_t  stag_dei:1;
	a_uint32_t  ctag_pcp_change_en:1;
	a_uint32_t  ctag_pcp_0:2;
	a_uint32_t  ctag_pcp_1:1;
	a_uint32_t  ctag_dei_change_en:1;
	a_uint32_t  ctag_dei:1;
	a_uint32_t  enqueue_pri_change_en:1;
	a_uint32_t  enqueue_pri:4;
	a_uint32_t  int_dp_change_en:1;
	a_uint32_t  int_dp:2;
	a_uint32_t  policer_en:1;
	a_uint32_t  policer_index:9;
	a_uint32_t  qid_en:1;
	a_uint32_t  qid:8;
	a_uint32_t  service_code_en:1;
	a_uint32_t  service_code_0:1;
	a_uint32_t  service_code_1:7;
	a_uint32_t  syn_toggle:1;
	a_uint32_t  cpu_code_en:1;
	a_uint32_t  cpu_code:8;
	a_uint32_t  metadata_en:1;
	a_uint32_t  dscp_tc_mask:8;
	a_uint32_t  qos_res_prec:3;
	a_uint32_t  _reserved0:3;
};

union ipo_action_u {
	a_uint32_t val[5];
	struct ipo_action bf;
};


#endif
