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
#ifndef HPPE_POLICER_REG_H
#define HPPE_POLICER_REG_H

/*[register] METER_CMPST_LENGTH_REG*/
#define METER_CMPST_LENGTH_REG
#define METER_CMPST_LENGTH_REG_ADDRESS 0x0
#define METER_CMPST_LENGTH_REG_NUM     8
#define METER_CMPST_LENGTH_REG_INC     0x4
#define METER_CMPST_LENGTH_REG_TYPE    REG_TYPE_RW
#define METER_CMPST_LENGTH_REG_DEFAULT 0x0
	/*[field] CMPST_LENGTH*/
	#define METER_CMPST_LENGTH_REG_CMPST_LENGTH
	#define METER_CMPST_LENGTH_REG_CMPST_LENGTH_OFFSET  0
	#define METER_CMPST_LENGTH_REG_CMPST_LENGTH_LEN     5
	#define METER_CMPST_LENGTH_REG_CMPST_LENGTH_DEFAULT 0x0

struct meter_cmpst_length_reg {
	a_uint32_t  cmpst_length:5;
	a_uint32_t  _reserved0:27;
};

union meter_cmpst_length_reg_u {
	a_uint32_t val;
	struct meter_cmpst_length_reg bf;
};

/*[register] PC_DROP_BYPASS_REG*/
#define PC_DROP_BYPASS_REG
#define PC_DROP_BYPASS_REG_ADDRESS 0x20
#define PC_DROP_BYPASS_REG_NUM     1
#define PC_DROP_BYPASS_REG_INC     0x4
#define PC_DROP_BYPASS_REG_TYPE    REG_TYPE_RW
#define PC_DROP_BYPASS_REG_DEFAULT 0x0
	/*[field] DROP_BYPASS_EN*/
	#define PC_DROP_BYPASS_REG_DROP_BYPASS_EN
	#define PC_DROP_BYPASS_REG_DROP_BYPASS_EN_OFFSET  0
	#define PC_DROP_BYPASS_REG_DROP_BYPASS_EN_LEN     1
	#define PC_DROP_BYPASS_REG_DROP_BYPASS_EN_DEFAULT 0x0

struct pc_drop_bypass_reg {
	a_uint32_t  drop_bypass_en:1;
	a_uint32_t  _reserved0:31;
};

union pc_drop_bypass_reg_u {
	a_uint32_t val;
	struct pc_drop_bypass_reg bf;
};

/*[register] PC_SPARE_REG*/
#define PC_SPARE_REG
#define PC_SPARE_REG_ADDRESS 0x30
#define PC_SPARE_REG_NUM     1
#define PC_SPARE_REG_INC     0x4
#define PC_SPARE_REG_TYPE    REG_TYPE_RW
#define PC_SPARE_REG_DEFAULT 0x0
	/*[field] SPARE_REG*/
	#define PC_SPARE_REG_SPARE_REG
	#define PC_SPARE_REG_SPARE_REG_OFFSET  0
	#define PC_SPARE_REG_SPARE_REG_LEN     32
	#define PC_SPARE_REG_SPARE_REG_DEFAULT 0x0

struct pc_spare_reg {
	a_uint32_t  spare_reg:32;
};

union pc_spare_reg_u {
	a_uint32_t val;
	struct pc_spare_reg bf;
};

/*[register] TIME_SLOT_REG*/
#define TIME_SLOT_REG
#define TIME_SLOT_REG_ADDRESS 0x40
#define TIME_SLOT_REG_NUM     1
#define TIME_SLOT_REG_INC     0x4
#define TIME_SLOT_REG_TYPE    REG_TYPE_RW
#define TIME_SLOT_REG_DEFAULT 0x0
	/*[field] TIME_SLOT*/
	#define TIME_SLOT_REG_TIME_SLOT
	#define TIME_SLOT_REG_TIME_SLOT_OFFSET  0
	#define TIME_SLOT_REG_TIME_SLOT_LEN     10
	#define TIME_SLOT_REG_TIME_SLOT_DEFAULT 0x0

struct time_slot_reg {
	a_uint32_t  time_slot:10;
	a_uint32_t  _reserved0:22;
};

union time_slot_reg_u {
	a_uint32_t val;
	struct time_slot_reg bf;
};

/*[register] PC_DBG_ADDR_REG*/
#define PC_DBG_ADDR_REG
#define PC_DBG_ADDR_REG_ADDRESS 0x80
#define PC_DBG_ADDR_REG_NUM     1
#define PC_DBG_ADDR_REG_INC     0x4
#define PC_DBG_ADDR_REG_TYPE    REG_TYPE_RW
#define PC_DBG_ADDR_REG_DEFAULT 0x0
	/*[field] DBG_ADDR*/
	#define PC_DBG_ADDR_REG_DBG_ADDR
	#define PC_DBG_ADDR_REG_DBG_ADDR_OFFSET  24
	#define PC_DBG_ADDR_REG_DBG_ADDR_LEN     8
	#define PC_DBG_ADDR_REG_DBG_ADDR_DEFAULT 0x0

struct pc_dbg_addr_reg {
	a_uint32_t  dbg_addr:8;
};

union pc_dbg_addr_reg_u {
	a_uint32_t val;
	struct pc_dbg_addr_reg bf;
};

/*[register] PC_DBG_DATA_REG*/
#define PC_DBG_DATA_REG
#define PC_DBG_DATA_REG_ADDRESS 0x84
#define PC_DBG_DATA_REG_NUM     1
#define PC_DBG_DATA_REG_INC     0x4
#define PC_DBG_DATA_REG_TYPE    REG_TYPE_RO
#define PC_DBG_DATA_REG_DEFAULT 0x0
	/*[field] DBG_DATA*/
	#define PC_DBG_DATA_REG_DBG_DATA
	#define PC_DBG_DATA_REG_DBG_DATA_OFFSET  0
	#define PC_DBG_DATA_REG_DBG_DATA_LEN     32
	#define PC_DBG_DATA_REG_DBG_DATA_DEFAULT 0x0

struct pc_dbg_data_reg {
	a_uint32_t  dbg_data:32;
};

union pc_dbg_data_reg_u {
	a_uint32_t val;
	struct pc_dbg_data_reg bf;
};

/*[table] IN_ACL_METER_CFG_TBL*/
#define IN_ACL_METER_CFG_TBL
#define IN_ACL_METER_CFG_TBL_ADDRESS 0x4000
#define IN_ACL_METER_CFG_TBL_NUM     512
#define IN_ACL_METER_CFG_TBL_INC     0x10
#define IN_ACL_METER_CFG_TBL_TYPE    REG_TYPE_RW
#define IN_ACL_METER_CFG_TBL_DEFAULT 0x0
	/*[field] METER_EN*/
	#define IN_ACL_METER_CFG_TBL_METER_EN
	#define IN_ACL_METER_CFG_TBL_METER_EN_OFFSET  0
	#define IN_ACL_METER_CFG_TBL_METER_EN_LEN     1
	#define IN_ACL_METER_CFG_TBL_METER_EN_DEFAULT 0x0
	/*[field] COLOR_MODE*/
	#define IN_ACL_METER_CFG_TBL_COLOR_MODE
	#define IN_ACL_METER_CFG_TBL_COLOR_MODE_OFFSET  1
	#define IN_ACL_METER_CFG_TBL_COLOR_MODE_LEN     1
	#define IN_ACL_METER_CFG_TBL_COLOR_MODE_DEFAULT 0x0
	/*[field] COUPLING_FLAG*/
	#define IN_ACL_METER_CFG_TBL_COUPLING_FLAG
	#define IN_ACL_METER_CFG_TBL_COUPLING_FLAG_OFFSET  2
	#define IN_ACL_METER_CFG_TBL_COUPLING_FLAG_LEN     1
	#define IN_ACL_METER_CFG_TBL_COUPLING_FLAG_DEFAULT 0x0
	/*[field] METER_MODE*/
	#define IN_ACL_METER_CFG_TBL_METER_MODE
	#define IN_ACL_METER_CFG_TBL_METER_MODE_OFFSET  3
	#define IN_ACL_METER_CFG_TBL_METER_MODE_LEN     1
	#define IN_ACL_METER_CFG_TBL_METER_MODE_DEFAULT 0x0
	/*[field] TOKEN_UNIT*/
	#define IN_ACL_METER_CFG_TBL_TOKEN_UNIT
	#define IN_ACL_METER_CFG_TBL_TOKEN_UNIT_OFFSET  4
	#define IN_ACL_METER_CFG_TBL_TOKEN_UNIT_LEN     3
	#define IN_ACL_METER_CFG_TBL_TOKEN_UNIT_DEFAULT 0x0
	/*[field] METER_UNIT*/
	#define IN_ACL_METER_CFG_TBL_METER_UNIT
	#define IN_ACL_METER_CFG_TBL_METER_UNIT_OFFSET  7
	#define IN_ACL_METER_CFG_TBL_METER_UNIT_LEN     1
	#define IN_ACL_METER_CFG_TBL_METER_UNIT_DEFAULT 0x0
	/*[field] CBS*/
	#define IN_ACL_METER_CFG_TBL_CBS
	#define IN_ACL_METER_CFG_TBL_CBS_OFFSET  8
	#define IN_ACL_METER_CFG_TBL_CBS_LEN     16
	#define IN_ACL_METER_CFG_TBL_CBS_DEFAULT 0x0
	/*[field] CIR*/
	#define IN_ACL_METER_CFG_TBL_CIR
	#define IN_ACL_METER_CFG_TBL_CIR_OFFSET  24
	#define IN_ACL_METER_CFG_TBL_CIR_LEN     18
	#define IN_ACL_METER_CFG_TBL_CIR_DEFAULT 0x0
	/*[field] EBS*/
	#define IN_ACL_METER_CFG_TBL_EBS
	#define IN_ACL_METER_CFG_TBL_EBS_OFFSET  42
	#define IN_ACL_METER_CFG_TBL_EBS_LEN     16
	#define IN_ACL_METER_CFG_TBL_EBS_DEFAULT 0x0
	/*[field] EIR*/
	#define IN_ACL_METER_CFG_TBL_EIR
	#define IN_ACL_METER_CFG_TBL_EIR_OFFSET  58
	#define IN_ACL_METER_CFG_TBL_EIR_LEN     18
	#define IN_ACL_METER_CFG_TBL_EIR_DEFAULT 0x0
	/*[field] EXCEED_CHG_PRI_CMD*/
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_PRI_CMD
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_PRI_CMD_OFFSET  76
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_PRI_CMD_LEN     1
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_PRI_CMD_DEFAULT 0x0
	/*[field] EXCEED_CHG_DP_CMD*/
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_DP_CMD
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_DP_CMD_OFFSET  77
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_DP_CMD_LEN     1
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_DP_CMD_DEFAULT 0x0
	/*[field] EXCEED_CHG_PCP_CMD*/
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_PCP_CMD
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_PCP_CMD_OFFSET  78
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_PCP_CMD_LEN     1
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_PCP_CMD_DEFAULT 0x0
	/*[field] EXCEED_CHG_DEI_CMD*/
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_DEI_CMD
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_DEI_CMD_OFFSET  79
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_DEI_CMD_LEN     1
	#define IN_ACL_METER_CFG_TBL_EXCEED_CHG_DEI_CMD_DEFAULT 0x0
	/*[field] EXCEED_PRI*/
	#define IN_ACL_METER_CFG_TBL_EXCEED_PRI
	#define IN_ACL_METER_CFG_TBL_EXCEED_PRI_OFFSET  80
	#define IN_ACL_METER_CFG_TBL_EXCEED_PRI_LEN     4
	#define IN_ACL_METER_CFG_TBL_EXCEED_PRI_DEFAULT 0x0
	/*[field] EXCEED_DP*/
	#define IN_ACL_METER_CFG_TBL_EXCEED_DP
	#define IN_ACL_METER_CFG_TBL_EXCEED_DP_OFFSET  84
	#define IN_ACL_METER_CFG_TBL_EXCEED_DP_LEN     2
	#define IN_ACL_METER_CFG_TBL_EXCEED_DP_DEFAULT 0x0
	/*[field] EXCEED_PCP*/
	#define IN_ACL_METER_CFG_TBL_EXCEED_PCP
	#define IN_ACL_METER_CFG_TBL_EXCEED_PCP_OFFSET  86
	#define IN_ACL_METER_CFG_TBL_EXCEED_PCP_LEN     3
	#define IN_ACL_METER_CFG_TBL_EXCEED_PCP_DEFAULT 0x0
	/*[field] EXCEED_DEI*/
	#define IN_ACL_METER_CFG_TBL_EXCEED_DEI
	#define IN_ACL_METER_CFG_TBL_EXCEED_DEI_OFFSET  89
	#define IN_ACL_METER_CFG_TBL_EXCEED_DEI_LEN     1
	#define IN_ACL_METER_CFG_TBL_EXCEED_DEI_DEFAULT 0x0
	/*[field] VIOLATE_CMD*/
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CMD
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CMD_OFFSET  90
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CMD_LEN     1
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CMD_DEFAULT 0x0
	/*[field] VIOLATE_CHG_PRI_CMD*/
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_PRI_CMD
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_PRI_CMD_OFFSET  91
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_PRI_CMD_LEN     1
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_PRI_CMD_DEFAULT 0x0
	/*[field] VIOLATE_CHG_DP_CMD*/
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_DP_CMD
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_DP_CMD_OFFSET  92
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_DP_CMD_LEN     1
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_DP_CMD_DEFAULT 0x0
	/*[field] VIOLATE_CHG_PCP_CMD*/
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_PCP_CMD
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_PCP_CMD_OFFSET  93
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_PCP_CMD_LEN     1
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_PCP_CMD_DEFAULT 0x0
	/*[field] VIOLATE_CHG_DEI_CMD*/
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_DEI_CMD
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_DEI_CMD_OFFSET  94
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_DEI_CMD_LEN     1
	#define IN_ACL_METER_CFG_TBL_VIOLATE_CHG_DEI_CMD_DEFAULT 0x0
	/*[field] VIOLATE_PRI*/
	#define IN_ACL_METER_CFG_TBL_VIOLATE_PRI
	#define IN_ACL_METER_CFG_TBL_VIOLATE_PRI_OFFSET  95
	#define IN_ACL_METER_CFG_TBL_VIOLATE_PRI_LEN     4
	#define IN_ACL_METER_CFG_TBL_VIOLATE_PRI_DEFAULT 0x0
	/*[field] VIOLATE_DP*/
	#define IN_ACL_METER_CFG_TBL_VIOLATE_DP
	#define IN_ACL_METER_CFG_TBL_VIOLATE_DP_OFFSET  99
	#define IN_ACL_METER_CFG_TBL_VIOLATE_DP_LEN     2
	#define IN_ACL_METER_CFG_TBL_VIOLATE_DP_DEFAULT 0x0
	/*[field] VIOLATE_PCP*/
	#define IN_ACL_METER_CFG_TBL_VIOLATE_PCP
	#define IN_ACL_METER_CFG_TBL_VIOLATE_PCP_OFFSET  101
	#define IN_ACL_METER_CFG_TBL_VIOLATE_PCP_LEN     3
	#define IN_ACL_METER_CFG_TBL_VIOLATE_PCP_DEFAULT 0x0
	/*[field] VIOLATE_DEI*/
	#define IN_ACL_METER_CFG_TBL_VIOLATE_DEI
	#define IN_ACL_METER_CFG_TBL_VIOLATE_DEI_OFFSET  104
	#define IN_ACL_METER_CFG_TBL_VIOLATE_DEI_LEN     1
	#define IN_ACL_METER_CFG_TBL_VIOLATE_DEI_DEFAULT 0x0

struct in_acl_meter_cfg_tbl {
	a_uint32_t  meter_en:1;
	a_uint32_t  color_mode:1;
	a_uint32_t  coupling_flag:1;
	a_uint32_t  meter_mode:1;
	a_uint32_t  token_unit:3;
	a_uint32_t  meter_unit:1;
	a_uint32_t  cbs:16;
	a_uint32_t  cir_0:8;
	a_uint32_t  cir_1:10;
	a_uint32_t  ebs:16;
	a_uint32_t  eir_0:6;
	a_uint32_t  eir_1:12;
	a_uint32_t  exceed_chg_pri_cmd:1;
	a_uint32_t  exceed_chg_dp_cmd:1;
	a_uint32_t  exceed_chg_pcp_cmd:1;
	a_uint32_t  exceed_chg_dei_cmd:1;
	a_uint32_t  exceed_pri:4;
	a_uint32_t  exceed_dp:2;
	a_uint32_t  exceed_pcp:3;
	a_uint32_t  exceed_dei:1;
	a_uint32_t  violate_cmd:1;
	a_uint32_t  violate_chg_pri_cmd:1;
	a_uint32_t  violate_chg_dp_cmd:1;
	a_uint32_t  violate_chg_pcp_cmd:1;
	a_uint32_t  violate_chg_dei_cmd:1;
	a_uint32_t  violate_pri_0:1;
	a_uint32_t  violate_pri_1:3;
	a_uint32_t  violate_dp:2;
	a_uint32_t  violate_pcp:3;
	a_uint32_t  violate_dei:1;
	a_uint32_t  _reserved0:23;
};

union in_acl_meter_cfg_tbl_u {
	a_uint32_t val[4];
	struct in_acl_meter_cfg_tbl bf;
};

/*[table] IN_ACL_METER_CRDT_TBL*/
#define IN_ACL_METER_CRDT_TBL
#define IN_ACL_METER_CRDT_TBL_ADDRESS 0x8000
#define IN_ACL_METER_CRDT_TBL_NUM     512
#define IN_ACL_METER_CRDT_TBL_INC     0x10
#define IN_ACL_METER_CRDT_TBL_TYPE    REG_TYPE_RW
#define IN_ACL_METER_CRDT_TBL_DEFAULT 0x0
	/*[field] C_CRDT*/
	#define IN_ACL_METER_CRDT_TBL_C_CRDT
	#define IN_ACL_METER_CRDT_TBL_C_CRDT_OFFSET  0
	#define IN_ACL_METER_CRDT_TBL_C_CRDT_LEN     32
	#define IN_ACL_METER_CRDT_TBL_C_CRDT_DEFAULT 0x0
	/*[field] E_CRDT*/
	#define IN_ACL_METER_CRDT_TBL_E_CRDT
	#define IN_ACL_METER_CRDT_TBL_E_CRDT_OFFSET  32
	#define IN_ACL_METER_CRDT_TBL_E_CRDT_LEN     32
	#define IN_ACL_METER_CRDT_TBL_E_CRDT_DEFAULT 0x0

struct in_acl_meter_crdt_tbl {
	a_uint32_t  c_crdt:32;
	a_uint32_t  e_crdt:32;
};

union in_acl_meter_crdt_tbl_u {
	a_uint32_t val[2];
	struct in_acl_meter_crdt_tbl bf;
};

/*[table] IN_PORT_METER_CFG_TBL*/
#define IN_PORT_METER_CFG_TBL
#define IN_PORT_METER_CFG_TBL_ADDRESS 0xc000
#define IN_PORT_METER_CFG_TBL_NUM     8
#define IN_PORT_METER_CFG_TBL_INC     0x10
#define IN_PORT_METER_CFG_TBL_TYPE    REG_TYPE_RW
#define IN_PORT_METER_CFG_TBL_DEFAULT 0x0
	/*[field] METER_EN*/
	#define IN_PORT_METER_CFG_TBL_METER_EN
	#define IN_PORT_METER_CFG_TBL_METER_EN_OFFSET  0
	#define IN_PORT_METER_CFG_TBL_METER_EN_LEN     1
	#define IN_PORT_METER_CFG_TBL_METER_EN_DEFAULT 0x0
	/*[field] COLOR_MODE*/
	#define IN_PORT_METER_CFG_TBL_COLOR_MODE
	#define IN_PORT_METER_CFG_TBL_COLOR_MODE_OFFSET  1
	#define IN_PORT_METER_CFG_TBL_COLOR_MODE_LEN     1
	#define IN_PORT_METER_CFG_TBL_COLOR_MODE_DEFAULT 0x0
	/*[field] METER_FLAG*/
	#define IN_PORT_METER_CFG_TBL_METER_FLAG
	#define IN_PORT_METER_CFG_TBL_METER_FLAG_OFFSET  2
	#define IN_PORT_METER_CFG_TBL_METER_FLAG_LEN     5
	#define IN_PORT_METER_CFG_TBL_METER_FLAG_DEFAULT 0x0
	/*[field] COUPLING_FLAG*/
	#define IN_PORT_METER_CFG_TBL_COUPLING_FLAG
	#define IN_PORT_METER_CFG_TBL_COUPLING_FLAG_OFFSET  7
	#define IN_PORT_METER_CFG_TBL_COUPLING_FLAG_LEN     1
	#define IN_PORT_METER_CFG_TBL_COUPLING_FLAG_DEFAULT 0x0
	/*[field] METER_MODE*/
	#define IN_PORT_METER_CFG_TBL_METER_MODE
	#define IN_PORT_METER_CFG_TBL_METER_MODE_OFFSET  8
	#define IN_PORT_METER_CFG_TBL_METER_MODE_LEN     1
	#define IN_PORT_METER_CFG_TBL_METER_MODE_DEFAULT 0x0
	/*[field] TOKEN_UNIT*/
	#define IN_PORT_METER_CFG_TBL_TOKEN_UNIT
	#define IN_PORT_METER_CFG_TBL_TOKEN_UNIT_OFFSET  9
	#define IN_PORT_METER_CFG_TBL_TOKEN_UNIT_LEN     3
	#define IN_PORT_METER_CFG_TBL_TOKEN_UNIT_DEFAULT 0x0
	/*[field] METER_UNIT*/
	#define IN_PORT_METER_CFG_TBL_METER_UNIT
	#define IN_PORT_METER_CFG_TBL_METER_UNIT_OFFSET  12
	#define IN_PORT_METER_CFG_TBL_METER_UNIT_LEN     1
	#define IN_PORT_METER_CFG_TBL_METER_UNIT_DEFAULT 0x0
	/*[field] CBS*/
	#define IN_PORT_METER_CFG_TBL_CBS
	#define IN_PORT_METER_CFG_TBL_CBS_OFFSET  13
	#define IN_PORT_METER_CFG_TBL_CBS_LEN     16
	#define IN_PORT_METER_CFG_TBL_CBS_DEFAULT 0x0
	/*[field] CIR*/
	#define IN_PORT_METER_CFG_TBL_CIR
	#define IN_PORT_METER_CFG_TBL_CIR_OFFSET  29
	#define IN_PORT_METER_CFG_TBL_CIR_LEN     18
	#define IN_PORT_METER_CFG_TBL_CIR_DEFAULT 0x0
	/*[field] EBS*/
	#define IN_PORT_METER_CFG_TBL_EBS
	#define IN_PORT_METER_CFG_TBL_EBS_OFFSET  47
	#define IN_PORT_METER_CFG_TBL_EBS_LEN     16
	#define IN_PORT_METER_CFG_TBL_EBS_DEFAULT 0x0
	/*[field] EIR*/
	#define IN_PORT_METER_CFG_TBL_EIR
	#define IN_PORT_METER_CFG_TBL_EIR_OFFSET  63
	#define IN_PORT_METER_CFG_TBL_EIR_LEN     18
	#define IN_PORT_METER_CFG_TBL_EIR_DEFAULT 0x0
	/*[field] EXCEED_CHG_PRI_CMD*/
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_PRI_CMD
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_PRI_CMD_OFFSET  81
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_PRI_CMD_LEN     1
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_PRI_CMD_DEFAULT 0x0
	/*[field] EXCEED_CHG_DP_CMD*/
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_DP_CMD
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_DP_CMD_OFFSET  82
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_DP_CMD_LEN     1
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_DP_CMD_DEFAULT 0x0
	/*[field] EXCEED_CHG_PCP_CMD*/
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_PCP_CMD
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_PCP_CMD_OFFSET  83
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_PCP_CMD_LEN     1
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_PCP_CMD_DEFAULT 0x0
	/*[field] EXCEED_CHG_DEI_CMD*/
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_DEI_CMD
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_DEI_CMD_OFFSET  84
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_DEI_CMD_LEN     1
	#define IN_PORT_METER_CFG_TBL_EXCEED_CHG_DEI_CMD_DEFAULT 0x0
	/*[field] EXCEED_PRI*/
	#define IN_PORT_METER_CFG_TBL_EXCEED_PRI
	#define IN_PORT_METER_CFG_TBL_EXCEED_PRI_OFFSET  85
	#define IN_PORT_METER_CFG_TBL_EXCEED_PRI_LEN     4
	#define IN_PORT_METER_CFG_TBL_EXCEED_PRI_DEFAULT 0x0
	/*[field] EXCEED_DP*/
	#define IN_PORT_METER_CFG_TBL_EXCEED_DP
	#define IN_PORT_METER_CFG_TBL_EXCEED_DP_OFFSET  89
	#define IN_PORT_METER_CFG_TBL_EXCEED_DP_LEN     2
	#define IN_PORT_METER_CFG_TBL_EXCEED_DP_DEFAULT 0x0
	/*[field] EXCEED_PCP*/
	#define IN_PORT_METER_CFG_TBL_EXCEED_PCP
	#define IN_PORT_METER_CFG_TBL_EXCEED_PCP_OFFSET  91
	#define IN_PORT_METER_CFG_TBL_EXCEED_PCP_LEN     3
	#define IN_PORT_METER_CFG_TBL_EXCEED_PCP_DEFAULT 0x0
	/*[field] EXCEED_DEI*/
	#define IN_PORT_METER_CFG_TBL_EXCEED_DEI
	#define IN_PORT_METER_CFG_TBL_EXCEED_DEI_OFFSET  94
	#define IN_PORT_METER_CFG_TBL_EXCEED_DEI_LEN     1
	#define IN_PORT_METER_CFG_TBL_EXCEED_DEI_DEFAULT 0x0
	/*[field] VIOLATE_CMD*/
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CMD
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CMD_OFFSET  95
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CMD_LEN     1
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CMD_DEFAULT 0x0
	/*[field] VIOLATE_CHG_PRI_CMD*/
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_PRI_CMD
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_PRI_CMD_OFFSET  96
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_PRI_CMD_LEN     1
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_PRI_CMD_DEFAULT 0x0
	/*[field] VIOLATE_CHG_DP_CMD*/
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_DP_CMD
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_DP_CMD_OFFSET  97
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_DP_CMD_LEN     1
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_DP_CMD_DEFAULT 0x0
	/*[field] VIOLATE_CHG_PCP_CMD*/
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_PCP_CMD
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_PCP_CMD_OFFSET  98
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_PCP_CMD_LEN     1
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_PCP_CMD_DEFAULT 0x0
	/*[field] VIOLATE_CHG_DEI_CMD*/
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_DEI_CMD
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_DEI_CMD_OFFSET  99
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_DEI_CMD_LEN     1
	#define IN_PORT_METER_CFG_TBL_VIOLATE_CHG_DEI_CMD_DEFAULT 0x0
	/*[field] VIOLATE_PRI*/
	#define IN_PORT_METER_CFG_TBL_VIOLATE_PRI
	#define IN_PORT_METER_CFG_TBL_VIOLATE_PRI_OFFSET  100
	#define IN_PORT_METER_CFG_TBL_VIOLATE_PRI_LEN     4
	#define IN_PORT_METER_CFG_TBL_VIOLATE_PRI_DEFAULT 0x0
	/*[field] VIOLATE_DP*/
	#define IN_PORT_METER_CFG_TBL_VIOLATE_DP
	#define IN_PORT_METER_CFG_TBL_VIOLATE_DP_OFFSET  104
	#define IN_PORT_METER_CFG_TBL_VIOLATE_DP_LEN     2
	#define IN_PORT_METER_CFG_TBL_VIOLATE_DP_DEFAULT 0x0
	/*[field] VIOLATE_PCP*/
	#define IN_PORT_METER_CFG_TBL_VIOLATE_PCP
	#define IN_PORT_METER_CFG_TBL_VIOLATE_PCP_OFFSET  106
	#define IN_PORT_METER_CFG_TBL_VIOLATE_PCP_LEN     3
	#define IN_PORT_METER_CFG_TBL_VIOLATE_PCP_DEFAULT 0x0
	/*[field] VIOLATE_DEI*/
	#define IN_PORT_METER_CFG_TBL_VIOLATE_DEI
	#define IN_PORT_METER_CFG_TBL_VIOLATE_DEI_OFFSET  109
	#define IN_PORT_METER_CFG_TBL_VIOLATE_DEI_LEN     1
	#define IN_PORT_METER_CFG_TBL_VIOLATE_DEI_DEFAULT 0x0

struct in_port_meter_cfg_tbl {
	a_uint32_t  meter_en:1;
	a_uint32_t  color_mode:1;
	a_uint32_t  meter_flag:5;
	a_uint32_t  coupling_flag:1;
	a_uint32_t  meter_mode:1;
	a_uint32_t  token_unit:3;
	a_uint32_t  meter_unit:1;
	a_uint32_t  cbs:16;
	a_uint32_t  cir_0:3;
	a_uint32_t  cir_1:15;
	a_uint32_t  ebs:16;
	a_uint32_t  eir_0:1;
	a_uint32_t  eir_1:17;
	a_uint32_t  exceed_chg_pri_cmd:1;
	a_uint32_t  exceed_chg_dp_cmd:1;
	a_uint32_t  exceed_chg_pcp_cmd:1;
	a_uint32_t  exceed_chg_dei_cmd:1;
	a_uint32_t  exceed_pri:4;
	a_uint32_t  exceed_dp:2;
	a_uint32_t  exceed_pcp:3;
	a_uint32_t  exceed_dei:1;
	a_uint32_t  violate_cmd:1;
	a_uint32_t  violate_chg_pri_cmd:1;
	a_uint32_t  violate_chg_dp_cmd:1;
	a_uint32_t  violate_chg_pcp_cmd:1;
	a_uint32_t  violate_chg_dei_cmd:1;
	a_uint32_t  violate_pri:4;
	a_uint32_t  violate_dp:2;
	a_uint32_t  violate_pcp:3;
	a_uint32_t  violate_dei:1;
	a_uint32_t  _reserved0:18;
};

union in_port_meter_cfg_tbl_u {
	a_uint32_t val[4];
	struct in_port_meter_cfg_tbl bf;
};

/*[table] IN_PORT_METER_CRDT_TBL*/
#define IN_PORT_METER_CRDT_TBL
#define IN_PORT_METER_CRDT_TBL_ADDRESS 0xd000
#define IN_PORT_METER_CRDT_TBL_NUM     8
#define IN_PORT_METER_CRDT_TBL_INC     0x10
#define IN_PORT_METER_CRDT_TBL_TYPE    REG_TYPE_RW
#define IN_PORT_METER_CRDT_TBL_DEFAULT 0x0
	/*[field] C_CRDT*/
	#define IN_PORT_METER_CRDT_TBL_C_CRDT
	#define IN_PORT_METER_CRDT_TBL_C_CRDT_OFFSET  0
	#define IN_PORT_METER_CRDT_TBL_C_CRDT_LEN     32
	#define IN_PORT_METER_CRDT_TBL_C_CRDT_DEFAULT 0x0
	/*[field] E_CRDT*/
	#define IN_PORT_METER_CRDT_TBL_E_CRDT
	#define IN_PORT_METER_CRDT_TBL_E_CRDT_OFFSET  32
	#define IN_PORT_METER_CRDT_TBL_E_CRDT_LEN     32
	#define IN_PORT_METER_CRDT_TBL_E_CRDT_DEFAULT 0x0

struct in_port_meter_crdt_tbl {
	a_uint32_t  c_crdt:32;
	a_uint32_t  e_crdt:32;
};

union in_port_meter_crdt_tbl_u {
	a_uint32_t val[2];
	struct in_port_meter_crdt_tbl bf;
};

/*[table] IN_PORT_METER_CNT_TBL*/
#define IN_PORT_METER_CNT_TBL
#define IN_PORT_METER_CNT_TBL_ADDRESS 0xe000
#define IN_PORT_METER_CNT_TBL_NUM     24
#define IN_PORT_METER_CNT_TBL_INC     0x10
#define IN_PORT_METER_CNT_TBL_TYPE    REG_TYPE_RW
#define IN_PORT_METER_CNT_TBL_DEFAULT 0x0
	/*[field] PKT_CNT*/
	#define IN_PORT_METER_CNT_TBL_PKT_CNT
	#define IN_PORT_METER_CNT_TBL_PKT_CNT_OFFSET  0
	#define IN_PORT_METER_CNT_TBL_PKT_CNT_LEN     32
	#define IN_PORT_METER_CNT_TBL_PKT_CNT_DEFAULT 0x0
	/*[field] BYTE_CNT*/
	#define IN_PORT_METER_CNT_TBL_BYTE_CNT
	#define IN_PORT_METER_CNT_TBL_BYTE_CNT_OFFSET  32
	#define IN_PORT_METER_CNT_TBL_BYTE_CNT_LEN     40
	#define IN_PORT_METER_CNT_TBL_BYTE_CNT_DEFAULT 0x0

struct in_port_meter_cnt_tbl {
	a_uint32_t  pkt_cnt:32;
	a_uint32_t  byte_cnt_0:32;
	a_uint32_t  byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union in_port_meter_cnt_tbl_u {
	a_uint32_t val[3];
	struct in_port_meter_cnt_tbl bf;
};

/*[table] IN_ACL_METER_CNT_TBL*/
#define IN_ACL_METER_CNT_TBL
#define IN_ACL_METER_CNT_TBL_ADDRESS 0x10000
#define IN_ACL_METER_CNT_TBL_NUM     1536
#define IN_ACL_METER_CNT_TBL_INC     0x10
#define IN_ACL_METER_CNT_TBL_TYPE    REG_TYPE_RW
#define IN_ACL_METER_CNT_TBL_DEFAULT 0x0
	/*[field] PKT_CNT*/
	#define IN_ACL_METER_CNT_TBL_PKT_CNT
	#define IN_ACL_METER_CNT_TBL_PKT_CNT_OFFSET  0
	#define IN_ACL_METER_CNT_TBL_PKT_CNT_LEN     32
	#define IN_ACL_METER_CNT_TBL_PKT_CNT_DEFAULT 0x0
	/*[field] BYTE_CNT*/
	#define IN_ACL_METER_CNT_TBL_BYTE_CNT
	#define IN_ACL_METER_CNT_TBL_BYTE_CNT_OFFSET  32
	#define IN_ACL_METER_CNT_TBL_BYTE_CNT_LEN     40
	#define IN_ACL_METER_CNT_TBL_BYTE_CNT_DEFAULT 0x0

struct in_acl_meter_cnt_tbl {
	a_uint32_t  pkt_cnt:32;
	a_uint32_t  byte_cnt_0:32;
	a_uint32_t  byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union in_acl_meter_cnt_tbl_u {
	a_uint32_t val[3];
	struct in_acl_meter_cnt_tbl bf;
};

/*[table] PC_GLOBAL_CNT_TBL*/
#define PC_GLOBAL_CNT_TBL
#define PC_GLOBAL_CNT_TBL_ADDRESS 0x58000
#define PC_GLOBAL_CNT_TBL_NUM     3
#define PC_GLOBAL_CNT_TBL_INC     0x10
#define PC_GLOBAL_CNT_TBL_TYPE    REG_TYPE_RW
#define PC_GLOBAL_CNT_TBL_DEFAULT 0x0
	/*[field] PKT_CNT*/
	#define PC_GLOBAL_CNT_TBL_PKT_CNT
	#define PC_GLOBAL_CNT_TBL_PKT_CNT_OFFSET  0
	#define PC_GLOBAL_CNT_TBL_PKT_CNT_LEN     32
	#define PC_GLOBAL_CNT_TBL_PKT_CNT_DEFAULT 0x0
	/*[field] BYTE_CNT*/
	#define PC_GLOBAL_CNT_TBL_BYTE_CNT
	#define PC_GLOBAL_CNT_TBL_BYTE_CNT_OFFSET  32
	#define PC_GLOBAL_CNT_TBL_BYTE_CNT_LEN     40
	#define PC_GLOBAL_CNT_TBL_BYTE_CNT_DEFAULT 0x0

struct pc_global_cnt_tbl {
	a_uint32_t  pkt_cnt:32;
	a_uint32_t  byte_cnt_0:32;
	a_uint32_t  byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union pc_global_cnt_tbl_u {
	a_uint32_t val[3];
	struct pc_global_cnt_tbl bf;
};

/*[table] DROP_CPU_CNT_TBL*/
#define DROP_CPU_CNT_TBL
#define DROP_CPU_CNT_TBL_ADDRESS 0x60000
#define DROP_CPU_CNT_TBL_NUM     1280
#define DROP_CPU_CNT_TBL_INC     0x10
#define DROP_CPU_CNT_TBL_TYPE    REG_TYPE_RW
#define DROP_CPU_CNT_TBL_DEFAULT 0x0
	/*[field] PKT_CNT*/
	#define DROP_CPU_CNT_TBL_PKT_CNT
	#define DROP_CPU_CNT_TBL_PKT_CNT_OFFSET  0
	#define DROP_CPU_CNT_TBL_PKT_CNT_LEN     32
	#define DROP_CPU_CNT_TBL_PKT_CNT_DEFAULT 0x0
	/*[field] BYTE_CNT*/
	#define DROP_CPU_CNT_TBL_BYTE_CNT
	#define DROP_CPU_CNT_TBL_BYTE_CNT_OFFSET  32
	#define DROP_CPU_CNT_TBL_BYTE_CNT_LEN     40
	#define DROP_CPU_CNT_TBL_BYTE_CNT_DEFAULT 0x0

struct drop_cpu_cnt_tbl {
	a_uint32_t  pkt_cnt:32;
	a_uint32_t  byte_cnt_0:32;
	a_uint32_t  byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union drop_cpu_cnt_tbl_u {
	a_uint32_t val[3];
	struct drop_cpu_cnt_tbl bf;
};

/*[table] PORT_TX_DROP_CNT_TBL*/
#define PORT_TX_DROP_CNT_TBL
#define PORT_TX_DROP_CNT_TBL_ADDRESS 0x7d000
#define PORT_TX_DROP_CNT_TBL_NUM     8
#define PORT_TX_DROP_CNT_TBL_INC     0x10
#define PORT_TX_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define PORT_TX_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] TX_DROP_PKT_CNT*/
	#define PORT_TX_DROP_CNT_TBL_TX_DROP_PKT_CNT
	#define PORT_TX_DROP_CNT_TBL_TX_DROP_PKT_CNT_OFFSET  0
	#define PORT_TX_DROP_CNT_TBL_TX_DROP_PKT_CNT_LEN     32
	#define PORT_TX_DROP_CNT_TBL_TX_DROP_PKT_CNT_DEFAULT 0x0
	/*[field] TX_DROP_BYTE_CNT*/
	#define PORT_TX_DROP_CNT_TBL_TX_DROP_BYTE_CNT
	#define PORT_TX_DROP_CNT_TBL_TX_DROP_BYTE_CNT_OFFSET  32
	#define PORT_TX_DROP_CNT_TBL_TX_DROP_BYTE_CNT_LEN     40
	#define PORT_TX_DROP_CNT_TBL_TX_DROP_BYTE_CNT_DEFAULT 0x0

struct port_tx_drop_cnt_tbl {
	a_uint32_t  tx_drop_pkt_cnt:32;
	a_uint32_t  tx_drop_byte_cnt_0:32;
	a_uint32_t  tx_drop_byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union port_tx_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct port_tx_drop_cnt_tbl bf;
};

/*[table] VP_TX_DROP_CNT_TBL*/
#define VP_TX_DROP_CNT_TBL
#define VP_TX_DROP_CNT_TBL_ADDRESS 0x7e000
#define VP_TX_DROP_CNT_TBL_NUM     256
#define VP_TX_DROP_CNT_TBL_INC     0x10
#define VP_TX_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define VP_TX_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] TX_DROP_PKT_CNT*/
	#define VP_TX_DROP_CNT_TBL_TX_DROP_PKT_CNT
	#define VP_TX_DROP_CNT_TBL_TX_DROP_PKT_CNT_OFFSET  0
	#define VP_TX_DROP_CNT_TBL_TX_DROP_PKT_CNT_LEN     32
	#define VP_TX_DROP_CNT_TBL_TX_DROP_PKT_CNT_DEFAULT 0x0
	/*[field] TX_DROP_BYTE_CNT*/
	#define VP_TX_DROP_CNT_TBL_TX_DROP_BYTE_CNT
	#define VP_TX_DROP_CNT_TBL_TX_DROP_BYTE_CNT_OFFSET  32
	#define VP_TX_DROP_CNT_TBL_TX_DROP_BYTE_CNT_LEN     40
	#define VP_TX_DROP_CNT_TBL_TX_DROP_BYTE_CNT_DEFAULT 0x0

struct vp_tx_drop_cnt_tbl {
	a_uint32_t  tx_drop_pkt_cnt:32;
	a_uint32_t  tx_drop_byte_cnt_0:32;
	a_uint32_t  tx_drop_byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union vp_tx_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct vp_tx_drop_cnt_tbl bf;
};

/*[table] VLAN_DEV_CNT_TBL*/
#define VLAN_DEV_CNT_TBL
#define VLAN_DEV_CNT_TBL_ADDRESS 0x7f000
#define VLAN_DEV_CNT_TBL_NUM     64
#define VLAN_DEV_CNT_TBL_INC     0x10
#define VLAN_DEV_CNT_TBL_TYPE    REG_TYPE_RW
#define VLAN_DEV_CNT_TBL_DEFAULT 0x0
	/*[field] RX_PKT_CNT*/
	#define VLAN_DEV_CNT_TBL_RX_PKT_CNT
	#define VLAN_DEV_CNT_TBL_RX_PKT_CNT_OFFSET  0
	#define VLAN_DEV_CNT_TBL_RX_PKT_CNT_LEN     32
	#define VLAN_DEV_CNT_TBL_RX_PKT_CNT_DEFAULT 0x0
	/*[field] RX_BYTE_CNT*/
	#define VLAN_DEV_CNT_TBL_RX_BYTE_CNT
	#define VLAN_DEV_CNT_TBL_RX_BYTE_CNT_OFFSET  32
	#define VLAN_DEV_CNT_TBL_RX_BYTE_CNT_LEN     40
	#define VLAN_DEV_CNT_TBL_RX_BYTE_CNT_DEFAULT 0x0

struct vlan_dev_cnt_tbl {
	a_uint32_t  rx_pkt_cnt:32;
	a_uint32_t  rx_byte_cnt_0:32;
	a_uint32_t  rx_byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union vlan_dev_cnt_tbl_u {
	a_uint32_t val[3];
	struct vlan_dev_cnt_tbl bf;
};



#endif
