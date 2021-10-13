/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
#ifndef _CPPE_QOS_REG_H_
#define _CPPE_QOS_REG_H_

/*[table] QOS_MAPPING_TBL*/
#define QOS_MAPPING_TBL
#define QOS_MAPPING_TBL_ADDRESS 0x20000
#define QOS_MAPPING_TBL_NUM     2592
#define QOS_MAPPING_TBL_INC     0x10
#define QOS_MAPPING_TBL_TYPE    REG_TYPE_RW
#define QOS_MAPPING_TBL_DEFAULT 0x0
	/*[field] INT_DSCP_TC*/
	#define QOS_MAPPING_TBL_INT_DSCP_TC
	#define QOS_MAPPING_TBL_INT_DSCP_TC_OFFSET  0
	#define QOS_MAPPING_TBL_INT_DSCP_TC_LEN     8
	#define QOS_MAPPING_TBL_INT_DSCP_TC_DEFAULT 0x0
	/*[field] DSCP_TC_MASK*/
	#define QOS_MAPPING_TBL_DSCP_TC_MASK
	#define QOS_MAPPING_TBL_DSCP_TC_MASK_OFFSET  8
	#define QOS_MAPPING_TBL_DSCP_TC_MASK_LEN     8
	#define QOS_MAPPING_TBL_DSCP_TC_MASK_DEFAULT 0x0
	/*[field] INT_DSCP_EN*/
	#define QOS_MAPPING_TBL_INT_DSCP_EN
	#define QOS_MAPPING_TBL_INT_DSCP_EN_OFFSET  16
	#define QOS_MAPPING_TBL_INT_DSCP_EN_LEN     1
	#define QOS_MAPPING_TBL_INT_DSCP_EN_DEFAULT 0x0
	/*[field] INT_PCP_EN*/
	#define QOS_MAPPING_TBL_INT_PCP_EN
	#define QOS_MAPPING_TBL_INT_PCP_EN_OFFSET  17
	#define QOS_MAPPING_TBL_INT_PCP_EN_LEN     1
	#define QOS_MAPPING_TBL_INT_PCP_EN_DEFAULT 0x0
	/*[field] INT_PCP*/
	#define QOS_MAPPING_TBL_INT_PCP
	#define QOS_MAPPING_TBL_INT_PCP_OFFSET  18
	#define QOS_MAPPING_TBL_INT_PCP_LEN     3
	#define QOS_MAPPING_TBL_INT_PCP_DEFAULT 0x0
	/*[field] INT_DEI_EN*/
	#define QOS_MAPPING_TBL_INT_DEI_EN
	#define QOS_MAPPING_TBL_INT_DEI_EN_OFFSET  21
	#define QOS_MAPPING_TBL_INT_DEI_EN_LEN     1
	#define QOS_MAPPING_TBL_INT_DEI_EN_DEFAULT 0x0
	/*[field] INT_DEI*/
	#define QOS_MAPPING_TBL_INT_DEI
	#define QOS_MAPPING_TBL_INT_DEI_OFFSET  22
	#define QOS_MAPPING_TBL_INT_DEI_LEN     1
	#define QOS_MAPPING_TBL_INT_DEI_DEFAULT 0x0
	/*[field] INT_PRI_EN*/
	#define QOS_MAPPING_TBL_INT_PRI_EN
	#define QOS_MAPPING_TBL_INT_PRI_EN_OFFSET  23
	#define QOS_MAPPING_TBL_INT_PRI_EN_LEN     1
	#define QOS_MAPPING_TBL_INT_PRI_EN_DEFAULT 0x0
	/*[field] INT_PRI*/
	#define QOS_MAPPING_TBL_INT_PRI
	#define QOS_MAPPING_TBL_INT_PRI_OFFSET  24
	#define QOS_MAPPING_TBL_INT_PRI_LEN     4
	#define QOS_MAPPING_TBL_INT_PRI_DEFAULT 0x0
	/*[field] INT_DP_EN*/
	#define QOS_MAPPING_TBL_INT_DP_EN
	#define QOS_MAPPING_TBL_INT_DP_EN_OFFSET  28
	#define QOS_MAPPING_TBL_INT_DP_EN_LEN     1
	#define QOS_MAPPING_TBL_INT_DP_EN_DEFAULT 0x0
	/*[field] INT_DP*/
	#define QOS_MAPPING_TBL_INT_DP
	#define QOS_MAPPING_TBL_INT_DP_OFFSET  29
	#define QOS_MAPPING_TBL_INT_DP_LEN     2
	#define QOS_MAPPING_TBL_INT_DP_DEFAULT 0x0
	/*[field] QOS_RES_PREC*/
	#define QOS_MAPPING_TBL_QOS_RES_PREC
	#define QOS_MAPPING_TBL_QOS_RES_PREC_OFFSET  31
	#define QOS_MAPPING_TBL_QOS_RES_PREC_LEN     3
	#define QOS_MAPPING_TBL_QOS_RES_PREC_DEFAULT 0x0

struct qos_mapping_tbl {
	a_uint32_t  int_dscp_tc:8;
	a_uint32_t  dscp_tc_mask:8;
	a_uint32_t  int_dscp_en:1;
	a_uint32_t  int_pcp_en:1;
	a_uint32_t  int_pcp:3;
	a_uint32_t  int_dei_en:1;
	a_uint32_t  int_dei:1;
	a_uint32_t  int_pri_en:1;
	a_uint32_t  int_pri:4;
	a_uint32_t  int_dp_en:1;
	a_uint32_t  int_dp:2;
	a_uint32_t  qos_res_prec_0:1;
	a_uint32_t  qos_res_prec_1:2;
	a_uint32_t  _reserved0:30;
};

union qos_mapping_tbl_u {
	a_uint32_t val[2];
	struct qos_mapping_tbl bf;
};

#endif