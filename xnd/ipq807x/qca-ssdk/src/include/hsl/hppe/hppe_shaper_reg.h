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
#ifndef HPPE_SHAPER_REG_H
#define HPPE_SHAPER_REG_H

/*[register] SHP_SLOT_CFG_L0*/
#define SHP_SLOT_CFG_L0
#define SHP_SLOT_CFG_L0_ADDRESS 0x10
#define SHP_SLOT_CFG_L0_NUM     1
#define SHP_SLOT_CFG_L0_INC     0x4
#define SHP_SLOT_CFG_L0_TYPE    REG_TYPE_RW
#define SHP_SLOT_CFG_L0_DEFAULT 0x12c
	/*[field] L0_SHP_SLOT_TIME*/
	#define SHP_SLOT_CFG_L0_L0_SHP_SLOT_TIME
	#define SHP_SLOT_CFG_L0_L0_SHP_SLOT_TIME_OFFSET  0
	#define SHP_SLOT_CFG_L0_L0_SHP_SLOT_TIME_LEN     12
	#define SHP_SLOT_CFG_L0_L0_SHP_SLOT_TIME_DEFAULT 0x12c

struct shp_slot_cfg_l0 {
	a_uint32_t  l0_shp_slot_time:12;
	a_uint32_t  _reserved0:20;
};

union shp_slot_cfg_l0_u {
	a_uint32_t val;
	struct shp_slot_cfg_l0 bf;
};

/*[register] SHP_SLOT_CFG_L1*/
#define SHP_SLOT_CFG_L1
#define SHP_SLOT_CFG_L1_ADDRESS 0x14
#define SHP_SLOT_CFG_L1_NUM     1
#define SHP_SLOT_CFG_L1_INC     0x4
#define SHP_SLOT_CFG_L1_TYPE    REG_TYPE_RW
#define SHP_SLOT_CFG_L1_DEFAULT 0x40
	/*[field] L1_SHP_SLOT_TIME*/
	#define SHP_SLOT_CFG_L1_L1_SHP_SLOT_TIME
	#define SHP_SLOT_CFG_L1_L1_SHP_SLOT_TIME_OFFSET  0
	#define SHP_SLOT_CFG_L1_L1_SHP_SLOT_TIME_LEN     12
	#define SHP_SLOT_CFG_L1_L1_SHP_SLOT_TIME_DEFAULT 0x40

struct shp_slot_cfg_l1 {
	a_uint32_t  l1_shp_slot_time:12;
	a_uint32_t  _reserved0:20;
};

union shp_slot_cfg_l1_u {
	a_uint32_t val;
	struct shp_slot_cfg_l1 bf;
};

/*[register] SHP_SLOT_CFG_PORT*/
#define SHP_SLOT_CFG_PORT
#define SHP_SLOT_CFG_PORT_ADDRESS 0x18
#define SHP_SLOT_CFG_PORT_NUM     1
#define SHP_SLOT_CFG_PORT_INC     0x4
#define SHP_SLOT_CFG_PORT_TYPE    REG_TYPE_RW
#define SHP_SLOT_CFG_PORT_DEFAULT 0x8
	/*[field] PORT_SHP_SLOT_TIME*/
	#define SHP_SLOT_CFG_PORT_PORT_SHP_SLOT_TIME
	#define SHP_SLOT_CFG_PORT_PORT_SHP_SLOT_TIME_OFFSET  0
	#define SHP_SLOT_CFG_PORT_PORT_SHP_SLOT_TIME_LEN     12
	#define SHP_SLOT_CFG_PORT_PORT_SHP_SLOT_TIME_DEFAULT 0x8

struct shp_slot_cfg_port {
	a_uint32_t  port_shp_slot_time:12;
	a_uint32_t  _reserved0:20;
};

union shp_slot_cfg_port_u {
	a_uint32_t val;
	struct shp_slot_cfg_port bf;
};

/*[table] L0_SHP_CREDIT_TBL*/
#define L0_SHP_CREDIT_TBL
#define L0_SHP_CREDIT_TBL_ADDRESS 0x1a000
#define L0_SHP_CREDIT_TBL_NUM     300
#define L0_SHP_CREDIT_TBL_INC     0x10
#define L0_SHP_CREDIT_TBL_TYPE    REG_TYPE_RW
#define L0_SHP_CREDIT_TBL_DEFAULT 0x0
	/*[field] C_SHAPER_CREDIT*/
	#define L0_SHP_CREDIT_TBL_C_SHAPER_CREDIT
	#define L0_SHP_CREDIT_TBL_C_SHAPER_CREDIT_OFFSET  0
	#define L0_SHP_CREDIT_TBL_C_SHAPER_CREDIT_LEN     30
	#define L0_SHP_CREDIT_TBL_C_SHAPER_CREDIT_DEFAULT 0x0
	/*[field] C_SHAPER_CREDIT_NEG*/
	#define L0_SHP_CREDIT_TBL_C_SHAPER_CREDIT_NEG
	#define L0_SHP_CREDIT_TBL_C_SHAPER_CREDIT_NEG_OFFSET  30
	#define L0_SHP_CREDIT_TBL_C_SHAPER_CREDIT_NEG_LEN     1
	#define L0_SHP_CREDIT_TBL_C_SHAPER_CREDIT_NEG_DEFAULT 0x0
	/*[field] E_SHAPER_CREDIT*/
	#define L0_SHP_CREDIT_TBL_E_SHAPER_CREDIT
	#define L0_SHP_CREDIT_TBL_E_SHAPER_CREDIT_OFFSET  31
	#define L0_SHP_CREDIT_TBL_E_SHAPER_CREDIT_LEN     30
	#define L0_SHP_CREDIT_TBL_E_SHAPER_CREDIT_DEFAULT 0x0
	/*[field] E_SHAPER_CREDIT_NEG*/
	#define L0_SHP_CREDIT_TBL_E_SHAPER_CREDIT_NEG
	#define L0_SHP_CREDIT_TBL_E_SHAPER_CREDIT_NEG_OFFSET  61
	#define L0_SHP_CREDIT_TBL_E_SHAPER_CREDIT_NEG_LEN     1
	#define L0_SHP_CREDIT_TBL_E_SHAPER_CREDIT_NEG_DEFAULT 0x0

struct l0_shp_credit_tbl {
	a_uint32_t  c_shaper_credit:30;
	a_uint32_t  c_shaper_credit_neg:1;
	a_uint32_t  e_shaper_credit_0:1;
	a_uint32_t  e_shaper_credit_1:29;
	a_uint32_t  e_shaper_credit_neg:1;
	a_uint32_t  _reserved0:2;
};

union l0_shp_credit_tbl_u {
	a_uint32_t val[2];
	struct l0_shp_credit_tbl bf;
};

/*[table] L0_SHP_CFG_TBL*/
#define L0_SHP_CFG_TBL
#define L0_SHP_CFG_TBL_ADDRESS 0x1c000
#define L0_SHP_CFG_TBL_NUM     300
#define L0_SHP_CFG_TBL_INC     0x10
#define L0_SHP_CFG_TBL_TYPE    REG_TYPE_RW
#define L0_SHP_CFG_TBL_DEFAULT 0x0
	/*[field] CIR*/
	#define L0_SHP_CFG_TBL_CIR
	#define L0_SHP_CFG_TBL_CIR_OFFSET  0
	#define L0_SHP_CFG_TBL_CIR_LEN     18
	#define L0_SHP_CFG_TBL_CIR_DEFAULT 0x0
	/*[field] CBS*/
	#define L0_SHP_CFG_TBL_CBS
	#define L0_SHP_CFG_TBL_CBS_OFFSET  18
	#define L0_SHP_CFG_TBL_CBS_LEN     14
	#define L0_SHP_CFG_TBL_CBS_DEFAULT 0x0
	/*[field] EIR*/
	#define L0_SHP_CFG_TBL_EIR
	#define L0_SHP_CFG_TBL_EIR_OFFSET  32
	#define L0_SHP_CFG_TBL_EIR_LEN     18
	#define L0_SHP_CFG_TBL_EIR_DEFAULT 0x0
	/*[field] EBS*/
	#define L0_SHP_CFG_TBL_EBS
	#define L0_SHP_CFG_TBL_EBS_OFFSET  50
	#define L0_SHP_CFG_TBL_EBS_LEN     14
	#define L0_SHP_CFG_TBL_EBS_DEFAULT 0x0
	/*[field] TOKEN_UNIT*/
	#define L0_SHP_CFG_TBL_TOKEN_UNIT
	#define L0_SHP_CFG_TBL_TOKEN_UNIT_OFFSET  64
	#define L0_SHP_CFG_TBL_TOKEN_UNIT_LEN     3
	#define L0_SHP_CFG_TBL_TOKEN_UNIT_DEFAULT 0x0
	/*[field] METER_UNIT*/
	#define L0_SHP_CFG_TBL_METER_UNIT
	#define L0_SHP_CFG_TBL_METER_UNIT_OFFSET  67
	#define L0_SHP_CFG_TBL_METER_UNIT_LEN     1
	#define L0_SHP_CFG_TBL_METER_UNIT_DEFAULT 0x0
	/*[field] C_SHAPER_ENABLE*/
	#define L0_SHP_CFG_TBL_C_SHAPER_ENABLE
	#define L0_SHP_CFG_TBL_C_SHAPER_ENABLE_OFFSET  68
	#define L0_SHP_CFG_TBL_C_SHAPER_ENABLE_LEN     1
	#define L0_SHP_CFG_TBL_C_SHAPER_ENABLE_DEFAULT 0x0
	/*[field] E_SHAPER_ENABLE*/
	#define L0_SHP_CFG_TBL_E_SHAPER_ENABLE
	#define L0_SHP_CFG_TBL_E_SHAPER_ENABLE_OFFSET  69
	#define L0_SHP_CFG_TBL_E_SHAPER_ENABLE_LEN     1
	#define L0_SHP_CFG_TBL_E_SHAPER_ENABLE_DEFAULT 0x0
	/*[field] CF*/
	#define L0_SHP_CFG_TBL_CF
	#define L0_SHP_CFG_TBL_CF_OFFSET  70
	#define L0_SHP_CFG_TBL_CF_LEN     1
	#define L0_SHP_CFG_TBL_CF_DEFAULT 0x0

struct l0_shp_cfg_tbl {
	a_uint32_t  cir:18;
	a_uint32_t  cbs:14;
	a_uint32_t  eir:18;
	a_uint32_t  ebs:14;
	a_uint32_t  token_unit:3;
	a_uint32_t  meter_unit:1;
	a_uint32_t  c_shaper_enable:1;
	a_uint32_t  e_shaper_enable:1;
	a_uint32_t  cf:1;
	a_uint32_t  _reserved0:25;
};

union l0_shp_cfg_tbl_u {
	a_uint32_t val[3];
	struct l0_shp_cfg_tbl bf;
};

/*[table] L0_COMP_TBL*/
#define L0_COMP_TBL
#define L0_COMP_TBL_ADDRESS 0x26000
#define L0_COMP_TBL_NUM     300
#define L0_COMP_TBL_INC     0x10
#define L0_COMP_TBL_TYPE    REG_TYPE_RO
#define L0_COMP_TBL_DEFAULT 0x0
	/*[field] C_SHAPER_COMPENSATE_BYTE_CNT*/
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_CNT
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_CNT_OFFSET  0
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_CNT_LEN     18
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_CNT_DEFAULT 0x0
	/*[field] C_SHAPER_COMPENSATE_BYTE_NEG*/
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_NEG
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_NEG_OFFSET  18
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_NEG_LEN     1
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_NEG_DEFAULT 0x0
	/*[field] C_SHAPER_COMPENSATE_PKT_CNT*/
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_PKT_CNT
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_PKT_CNT_OFFSET  19
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_PKT_CNT_LEN     4
	#define L0_COMP_TBL_C_SHAPER_COMPENSATE_PKT_CNT_DEFAULT 0x0
	/*[field] C_DRR_COMPENSATE_BYTE_CNT*/
	#define L0_COMP_TBL_C_DRR_COMPENSATE_BYTE_CNT
	#define L0_COMP_TBL_C_DRR_COMPENSATE_BYTE_CNT_OFFSET  23
	#define L0_COMP_TBL_C_DRR_COMPENSATE_BYTE_CNT_LEN     18
	#define L0_COMP_TBL_C_DRR_COMPENSATE_BYTE_CNT_DEFAULT 0x0
	/*[field] C_DRR_COMPENSATE_BYTE_NEG*/
	#define L0_COMP_TBL_C_DRR_COMPENSATE_BYTE_NEG
	#define L0_COMP_TBL_C_DRR_COMPENSATE_BYTE_NEG_OFFSET  41
	#define L0_COMP_TBL_C_DRR_COMPENSATE_BYTE_NEG_LEN     1
	#define L0_COMP_TBL_C_DRR_COMPENSATE_BYTE_NEG_DEFAULT 0x0
	/*[field] C_DRR_COMPENSATE_PKT_CNT*/
	#define L0_COMP_TBL_C_DRR_COMPENSATE_PKT_CNT
	#define L0_COMP_TBL_C_DRR_COMPENSATE_PKT_CNT_OFFSET  42
	#define L0_COMP_TBL_C_DRR_COMPENSATE_PKT_CNT_LEN     4
	#define L0_COMP_TBL_C_DRR_COMPENSATE_PKT_CNT_DEFAULT 0x0
	/*[field] E_SHAPER_COMPENSATE_BYTE_CNT*/
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_CNT
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_CNT_OFFSET  46
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_CNT_LEN     18
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_CNT_DEFAULT 0x0
	/*[field] E_SHAPER_COMPENSATE_BYTE_NEG*/
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_NEG
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_NEG_OFFSET  64
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_NEG_LEN     1
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_NEG_DEFAULT 0x0
	/*[field] E_SHAPER_COMPENSATE_PKT_CNT*/
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_PKT_CNT
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_PKT_CNT_OFFSET  65
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_PKT_CNT_LEN     4
	#define L0_COMP_TBL_E_SHAPER_COMPENSATE_PKT_CNT_DEFAULT 0x0
	/*[field] E_DRR_COMPENSATE_BYTE_CNT*/
	#define L0_COMP_TBL_E_DRR_COMPENSATE_BYTE_CNT
	#define L0_COMP_TBL_E_DRR_COMPENSATE_BYTE_CNT_OFFSET  69
	#define L0_COMP_TBL_E_DRR_COMPENSATE_BYTE_CNT_LEN     18
	#define L0_COMP_TBL_E_DRR_COMPENSATE_BYTE_CNT_DEFAULT 0x0
	/*[field] E_DRR_COMPENSATE_BYTE_NEG*/
	#define L0_COMP_TBL_E_DRR_COMPENSATE_BYTE_NEG
	#define L0_COMP_TBL_E_DRR_COMPENSATE_BYTE_NEG_OFFSET  87
	#define L0_COMP_TBL_E_DRR_COMPENSATE_BYTE_NEG_LEN     1
	#define L0_COMP_TBL_E_DRR_COMPENSATE_BYTE_NEG_DEFAULT 0x0
	/*[field] E_DRR_COMPENSATE_PKT_CNT*/
	#define L0_COMP_TBL_E_DRR_COMPENSATE_PKT_CNT
	#define L0_COMP_TBL_E_DRR_COMPENSATE_PKT_CNT_OFFSET  88
	#define L0_COMP_TBL_E_DRR_COMPENSATE_PKT_CNT_LEN     4
	#define L0_COMP_TBL_E_DRR_COMPENSATE_PKT_CNT_DEFAULT 0x0

struct l0_comp_tbl {
	a_uint32_t  c_shaper_compensate_byte_cnt:18;
	a_uint32_t  c_shaper_compensate_byte_neg:1;
	a_uint32_t  c_shaper_compensate_pkt_cnt:4;
	a_uint32_t  c_drr_compensate_byte_cnt_0:9;
	a_uint32_t  c_drr_compensate_byte_cnt_1:9;
	a_uint32_t  c_drr_compensate_byte_neg:1;
	a_uint32_t  c_drr_compensate_pkt_cnt:4;
	a_uint32_t  e_shaper_compensate_byte_cnt:18;
	a_uint32_t  e_shaper_compensate_byte_neg:1;
	a_uint32_t  e_shaper_compensate_pkt_cnt:4;
	a_uint32_t  e_drr_compensate_byte_cnt:18;
	a_uint32_t  e_drr_compensate_byte_neg:1;
	a_uint32_t  e_drr_compensate_pkt_cnt:4;
	a_uint32_t  _reserved0:4;
};

union l0_comp_tbl_u {
	a_uint32_t val[3];
	struct l0_comp_tbl bf;
};

/*[table] L0_COMP_CFG_TBL*/
#define L0_COMP_CFG_TBL
#define L0_COMP_CFG_TBL_ADDRESS 0x28000
#define L0_COMP_CFG_TBL_NUM     300
#define L0_COMP_CFG_TBL_INC     0x10
#define L0_COMP_CFG_TBL_TYPE    REG_TYPE_RW
#define L0_COMP_CFG_TBL_DEFAULT 0x0
	/*[field] SHAPER_METER_LEN*/
	#define L0_COMP_CFG_TBL_SHAPER_METER_LEN
	#define L0_COMP_CFG_TBL_SHAPER_METER_LEN_OFFSET  0
	#define L0_COMP_CFG_TBL_SHAPER_METER_LEN_LEN     2
	#define L0_COMP_CFG_TBL_SHAPER_METER_LEN_DEFAULT 0x0
	/*[field] DRR_METER_LEN*/
	#define L0_COMP_CFG_TBL_DRR_METER_LEN
	#define L0_COMP_CFG_TBL_DRR_METER_LEN_OFFSET  2
	#define L0_COMP_CFG_TBL_DRR_METER_LEN_LEN     2
	#define L0_COMP_CFG_TBL_DRR_METER_LEN_DEFAULT 0x0

struct l0_comp_cfg_tbl {
	a_uint32_t  shaper_meter_len:2;
	a_uint32_t  drr_meter_len:2;
	a_uint32_t  _reserved0:28;
};

union l0_comp_cfg_tbl_u {
	a_uint32_t val;
	struct l0_comp_cfg_tbl bf;
};

/*[table] L1_SHP_CREDIT_TBL*/
#define L1_SHP_CREDIT_TBL
#define L1_SHP_CREDIT_TBL_ADDRESS 0x5c000
#define L1_SHP_CREDIT_TBL_NUM     64
#define L1_SHP_CREDIT_TBL_INC     0x10
#define L1_SHP_CREDIT_TBL_TYPE    REG_TYPE_RW
#define L1_SHP_CREDIT_TBL_DEFAULT 0x0
	/*[field] C_SHAPER_CREDIT*/
	#define L1_SHP_CREDIT_TBL_C_SHAPER_CREDIT
	#define L1_SHP_CREDIT_TBL_C_SHAPER_CREDIT_OFFSET  0
	#define L1_SHP_CREDIT_TBL_C_SHAPER_CREDIT_LEN     30
	#define L1_SHP_CREDIT_TBL_C_SHAPER_CREDIT_DEFAULT 0x0
	/*[field] C_SHAPER_CREDIT_NEG*/
	#define L1_SHP_CREDIT_TBL_C_SHAPER_CREDIT_NEG
	#define L1_SHP_CREDIT_TBL_C_SHAPER_CREDIT_NEG_OFFSET  30
	#define L1_SHP_CREDIT_TBL_C_SHAPER_CREDIT_NEG_LEN     1
	#define L1_SHP_CREDIT_TBL_C_SHAPER_CREDIT_NEG_DEFAULT 0x0
	/*[field] E_SHAPER_CREDIT*/
	#define L1_SHP_CREDIT_TBL_E_SHAPER_CREDIT
	#define L1_SHP_CREDIT_TBL_E_SHAPER_CREDIT_OFFSET  31
	#define L1_SHP_CREDIT_TBL_E_SHAPER_CREDIT_LEN     30
	#define L1_SHP_CREDIT_TBL_E_SHAPER_CREDIT_DEFAULT 0x0
	/*[field] E_SHAPER_CREDIT_NEG*/
	#define L1_SHP_CREDIT_TBL_E_SHAPER_CREDIT_NEG
	#define L1_SHP_CREDIT_TBL_E_SHAPER_CREDIT_NEG_OFFSET  61
	#define L1_SHP_CREDIT_TBL_E_SHAPER_CREDIT_NEG_LEN     1
	#define L1_SHP_CREDIT_TBL_E_SHAPER_CREDIT_NEG_DEFAULT 0x0

struct l1_shp_credit_tbl {
	a_uint32_t  c_shaper_credit:30;
	a_uint32_t  c_shaper_credit_neg:1;
	a_uint32_t  e_shaper_credit_0:1;
	a_uint32_t  e_shaper_credit_1:29;
	a_uint32_t  e_shaper_credit_neg:1;
	a_uint32_t  _reserved0:2;
};

union l1_shp_credit_tbl_u {
	a_uint32_t val[2];
	struct l1_shp_credit_tbl bf;
};

/*[table] L1_SHP_CFG_TBL*/
#define L1_SHP_CFG_TBL
#define L1_SHP_CFG_TBL_ADDRESS 0x5e000
#define L1_SHP_CFG_TBL_NUM     64
#define L1_SHP_CFG_TBL_INC     0x10
#define L1_SHP_CFG_TBL_TYPE    REG_TYPE_RW
#define L1_SHP_CFG_TBL_DEFAULT 0x0
	/*[field] CIR*/
	#define L1_SHP_CFG_TBL_CIR
	#define L1_SHP_CFG_TBL_CIR_OFFSET  0
	#define L1_SHP_CFG_TBL_CIR_LEN     18
	#define L1_SHP_CFG_TBL_CIR_DEFAULT 0x0
	/*[field] CBS*/
	#define L1_SHP_CFG_TBL_CBS
	#define L1_SHP_CFG_TBL_CBS_OFFSET  18
	#define L1_SHP_CFG_TBL_CBS_LEN     14
	#define L1_SHP_CFG_TBL_CBS_DEFAULT 0x0
	/*[field] EIR*/
	#define L1_SHP_CFG_TBL_EIR
	#define L1_SHP_CFG_TBL_EIR_OFFSET  32
	#define L1_SHP_CFG_TBL_EIR_LEN     18
	#define L1_SHP_CFG_TBL_EIR_DEFAULT 0x0
	/*[field] EBS*/
	#define L1_SHP_CFG_TBL_EBS
	#define L1_SHP_CFG_TBL_EBS_OFFSET  50
	#define L1_SHP_CFG_TBL_EBS_LEN     14
	#define L1_SHP_CFG_TBL_EBS_DEFAULT 0x0
	/*[field] TOKEN_UNIT*/
	#define L1_SHP_CFG_TBL_TOKEN_UNIT
	#define L1_SHP_CFG_TBL_TOKEN_UNIT_OFFSET  64
	#define L1_SHP_CFG_TBL_TOKEN_UNIT_LEN     3
	#define L1_SHP_CFG_TBL_TOKEN_UNIT_DEFAULT 0x0
	/*[field] METER_UNIT*/
	#define L1_SHP_CFG_TBL_METER_UNIT
	#define L1_SHP_CFG_TBL_METER_UNIT_OFFSET  67
	#define L1_SHP_CFG_TBL_METER_UNIT_LEN     1
	#define L1_SHP_CFG_TBL_METER_UNIT_DEFAULT 0x0
	/*[field] C_SHAPER_ENABLE*/
	#define L1_SHP_CFG_TBL_C_SHAPER_ENABLE
	#define L1_SHP_CFG_TBL_C_SHAPER_ENABLE_OFFSET  68
	#define L1_SHP_CFG_TBL_C_SHAPER_ENABLE_LEN     1
	#define L1_SHP_CFG_TBL_C_SHAPER_ENABLE_DEFAULT 0x0
	/*[field] E_SHAPER_ENABLE*/
	#define L1_SHP_CFG_TBL_E_SHAPER_ENABLE
	#define L1_SHP_CFG_TBL_E_SHAPER_ENABLE_OFFSET  69
	#define L1_SHP_CFG_TBL_E_SHAPER_ENABLE_LEN     1
	#define L1_SHP_CFG_TBL_E_SHAPER_ENABLE_DEFAULT 0x0
	/*[field] CF*/
	#define L1_SHP_CFG_TBL_CF
	#define L1_SHP_CFG_TBL_CF_OFFSET  70
	#define L1_SHP_CFG_TBL_CF_LEN     1
	#define L1_SHP_CFG_TBL_CF_DEFAULT 0x0

struct l1_shp_cfg_tbl {
	a_uint32_t  cir:18;
	a_uint32_t  cbs:14;
	a_uint32_t  eir:18;
	a_uint32_t  ebs:14;
	a_uint32_t  token_unit:3;
	a_uint32_t  meter_unit:1;
	a_uint32_t  c_shaper_enable:1;
	a_uint32_t  e_shaper_enable:1;
	a_uint32_t  cf:1;
	a_uint32_t  _reserved0:25;
};

union l1_shp_cfg_tbl_u {
	a_uint32_t val[3];
	struct l1_shp_cfg_tbl bf;
};

/*[table] L1_COMP_TBL*/
#define L1_COMP_TBL
#define L1_COMP_TBL_ADDRESS 0x68000
#define L1_COMP_TBL_NUM     64
#define L1_COMP_TBL_INC     0x10
#define L1_COMP_TBL_TYPE    REG_TYPE_RO
#define L1_COMP_TBL_DEFAULT 0x0
	/*[field] C_SHAPER_COMPENSATE_BYTE_CNT*/
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_CNT
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_CNT_OFFSET  0
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_CNT_LEN     18
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_CNT_DEFAULT 0x0
	/*[field] C_SHAPER_COMPENSATE_BYTE_NEG*/
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_NEG
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_NEG_OFFSET  18
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_NEG_LEN     1
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_BYTE_NEG_DEFAULT 0x0
	/*[field] C_SHAPER_COMPENSATE_PKT_CNT*/
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_PKT_CNT
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_PKT_CNT_OFFSET  19
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_PKT_CNT_LEN     4
	#define L1_COMP_TBL_C_SHAPER_COMPENSATE_PKT_CNT_DEFAULT 0x0
	/*[field] C_DRR_COMPENSATE_BYTE_CNT*/
	#define L1_COMP_TBL_C_DRR_COMPENSATE_BYTE_CNT
	#define L1_COMP_TBL_C_DRR_COMPENSATE_BYTE_CNT_OFFSET  23
	#define L1_COMP_TBL_C_DRR_COMPENSATE_BYTE_CNT_LEN     18
	#define L1_COMP_TBL_C_DRR_COMPENSATE_BYTE_CNT_DEFAULT 0x0
	/*[field] C_DRR_COMPENSATE_BYTE_NEG*/
	#define L1_COMP_TBL_C_DRR_COMPENSATE_BYTE_NEG
	#define L1_COMP_TBL_C_DRR_COMPENSATE_BYTE_NEG_OFFSET  41
	#define L1_COMP_TBL_C_DRR_COMPENSATE_BYTE_NEG_LEN     1
	#define L1_COMP_TBL_C_DRR_COMPENSATE_BYTE_NEG_DEFAULT 0x0
	/*[field] C_DRR_COMPENSATE_PKT_CNT*/
	#define L1_COMP_TBL_C_DRR_COMPENSATE_PKT_CNT
	#define L1_COMP_TBL_C_DRR_COMPENSATE_PKT_CNT_OFFSET  42
	#define L1_COMP_TBL_C_DRR_COMPENSATE_PKT_CNT_LEN     4
	#define L1_COMP_TBL_C_DRR_COMPENSATE_PKT_CNT_DEFAULT 0x0
	/*[field] E_SHAPER_COMPENSATE_BYTE_CNT*/
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_CNT
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_CNT_OFFSET  46
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_CNT_LEN     18
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_CNT_DEFAULT 0x0
	/*[field] E_SHAPER_COMPENSATE_BYTE_NEG*/
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_NEG
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_NEG_OFFSET  64
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_NEG_LEN     1
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_BYTE_NEG_DEFAULT 0x0
	/*[field] E_SHAPER_COMPENSATE_PKT_CNT*/
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_PKT_CNT
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_PKT_CNT_OFFSET  65
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_PKT_CNT_LEN     4
	#define L1_COMP_TBL_E_SHAPER_COMPENSATE_PKT_CNT_DEFAULT 0x0
	/*[field] E_DRR_COMPENSATE_BYTE_CNT*/
	#define L1_COMP_TBL_E_DRR_COMPENSATE_BYTE_CNT
	#define L1_COMP_TBL_E_DRR_COMPENSATE_BYTE_CNT_OFFSET  69
	#define L1_COMP_TBL_E_DRR_COMPENSATE_BYTE_CNT_LEN     18
	#define L1_COMP_TBL_E_DRR_COMPENSATE_BYTE_CNT_DEFAULT 0x0
	/*[field] E_DRR_COMPENSATE_BYTE_NEG*/
	#define L1_COMP_TBL_E_DRR_COMPENSATE_BYTE_NEG
	#define L1_COMP_TBL_E_DRR_COMPENSATE_BYTE_NEG_OFFSET  87
	#define L1_COMP_TBL_E_DRR_COMPENSATE_BYTE_NEG_LEN     1
	#define L1_COMP_TBL_E_DRR_COMPENSATE_BYTE_NEG_DEFAULT 0x0
	/*[field] E_DRR_COMPENSATE_PKT_CNT*/
	#define L1_COMP_TBL_E_DRR_COMPENSATE_PKT_CNT
	#define L1_COMP_TBL_E_DRR_COMPENSATE_PKT_CNT_OFFSET  88
	#define L1_COMP_TBL_E_DRR_COMPENSATE_PKT_CNT_LEN     4
	#define L1_COMP_TBL_E_DRR_COMPENSATE_PKT_CNT_DEFAULT 0x0

struct l1_comp_tbl {
	a_uint32_t  c_shaper_compensate_byte_cnt:18;
	a_uint32_t  c_shaper_compensate_byte_neg:1;
	a_uint32_t  c_shaper_compensate_pkt_cnt:4;
	a_uint32_t  c_drr_compensate_byte_cnt_0:9;
	a_uint32_t  c_drr_compensate_byte_cnt_1:9;
	a_uint32_t  c_drr_compensate_byte_neg:1;
	a_uint32_t  c_drr_compensate_pkt_cnt:4;
	a_uint32_t  e_shaper_compensate_byte_cnt:18;
	a_uint32_t  e_shaper_compensate_byte_neg:1;
	a_uint32_t  e_shaper_compensate_pkt_cnt:4;
	a_uint32_t  e_drr_compensate_byte_cnt:18;
	a_uint32_t  e_drr_compensate_byte_neg:1;
	a_uint32_t  e_drr_compensate_pkt_cnt:4;
	a_uint32_t  _reserved0:4;
};

union l1_comp_tbl_u {
	a_uint32_t val[3];
	struct l1_comp_tbl bf;
};

/*[table] L1_COMP_CFG_TBL*/
#define L1_COMP_CFG_TBL
#define L1_COMP_CFG_TBL_ADDRESS 0x6a000
#define L1_COMP_CFG_TBL_NUM     64
#define L1_COMP_CFG_TBL_INC     0x10
#define L1_COMP_CFG_TBL_TYPE    REG_TYPE_RW
#define L1_COMP_CFG_TBL_DEFAULT 0x0
	/*[field] SHAPER_METER_LEN*/
	#define L1_COMP_CFG_TBL_SHAPER_METER_LEN
	#define L1_COMP_CFG_TBL_SHAPER_METER_LEN_OFFSET  0
	#define L1_COMP_CFG_TBL_SHAPER_METER_LEN_LEN     2
	#define L1_COMP_CFG_TBL_SHAPER_METER_LEN_DEFAULT 0x0
	/*[field] DRR_METER_LEN*/
	#define L1_COMP_CFG_TBL_DRR_METER_LEN
	#define L1_COMP_CFG_TBL_DRR_METER_LEN_OFFSET  2
	#define L1_COMP_CFG_TBL_DRR_METER_LEN_LEN     2
	#define L1_COMP_CFG_TBL_DRR_METER_LEN_DEFAULT 0x0

struct l1_comp_cfg_tbl {
	a_uint32_t  shaper_meter_len:2;
	a_uint32_t  drr_meter_len:2;
	a_uint32_t  _reserved0:28;
};

union l1_comp_cfg_tbl_u {
	a_uint32_t val;
	struct l1_comp_cfg_tbl bf;
};

/*[table] PSCH_SHP_SIGN_TBL*/
#define PSCH_SHP_SIGN_TBL
#define PSCH_SHP_SIGN_TBL_ADDRESS 0x70000
#define PSCH_SHP_SIGN_TBL_NUM     8
#define PSCH_SHP_SIGN_TBL_INC     0x10
#define PSCH_SHP_SIGN_TBL_TYPE    REG_TYPE_RW
#define PSCH_SHP_SIGN_TBL_DEFAULT 0x0
	/*[field] SHAPER_CREDIT_NEG*/
	#define PSCH_SHP_SIGN_TBL_SHAPER_CREDIT_NEG
	#define PSCH_SHP_SIGN_TBL_SHAPER_CREDIT_NEG_OFFSET  0
	#define PSCH_SHP_SIGN_TBL_SHAPER_CREDIT_NEG_LEN     1
	#define PSCH_SHP_SIGN_TBL_SHAPER_CREDIT_NEG_DEFAULT 0x0

struct psch_shp_sign_tbl {
	a_uint32_t  shaper_credit_neg:1;
	a_uint32_t  _reserved0:31;
};

union psch_shp_sign_tbl_u {
	a_uint32_t val;
	struct psch_shp_sign_tbl bf;
};

/*[table] PSCH_SHP_CREDIT_TBL*/
#define PSCH_SHP_CREDIT_TBL
#define PSCH_SHP_CREDIT_TBL_ADDRESS 0x72000
#define PSCH_SHP_CREDIT_TBL_NUM     8
#define PSCH_SHP_CREDIT_TBL_INC     0x10
#define PSCH_SHP_CREDIT_TBL_TYPE    REG_TYPE_RW
#define PSCH_SHP_CREDIT_TBL_DEFAULT 0x0
	/*[field] SHAPER_CREDIT*/
	#define PSCH_SHP_CREDIT_TBL_SHAPER_CREDIT
	#define PSCH_SHP_CREDIT_TBL_SHAPER_CREDIT_OFFSET  0
	#define PSCH_SHP_CREDIT_TBL_SHAPER_CREDIT_LEN     30
	#define PSCH_SHP_CREDIT_TBL_SHAPER_CREDIT_DEFAULT 0x0

struct psch_shp_credit_tbl {
	a_uint32_t  shaper_credit:30;
	a_uint32_t  _reserved0:2;
};

union psch_shp_credit_tbl_u {
	a_uint32_t val;
	struct psch_shp_credit_tbl bf;
};

/*[table] PSCH_SHP_CFG_TBL*/
#define PSCH_SHP_CFG_TBL
#define PSCH_SHP_CFG_TBL_ADDRESS 0x74000
#define PSCH_SHP_CFG_TBL_NUM     8
#define PSCH_SHP_CFG_TBL_INC     0x10
#define PSCH_SHP_CFG_TBL_TYPE    REG_TYPE_RW
#define PSCH_SHP_CFG_TBL_DEFAULT 0x0
	/*[field] CIR*/
	#define PSCH_SHP_CFG_TBL_CIR
	#define PSCH_SHP_CFG_TBL_CIR_OFFSET  0
	#define PSCH_SHP_CFG_TBL_CIR_LEN     18
	#define PSCH_SHP_CFG_TBL_CIR_DEFAULT 0x0
	/*[field] CBS*/
	#define PSCH_SHP_CFG_TBL_CBS
	#define PSCH_SHP_CFG_TBL_CBS_OFFSET  18
	#define PSCH_SHP_CFG_TBL_CBS_LEN     14
	#define PSCH_SHP_CFG_TBL_CBS_DEFAULT 0x0
	/*[field] TOKEN_UNIT*/
	#define PSCH_SHP_CFG_TBL_TOKEN_UNIT
	#define PSCH_SHP_CFG_TBL_TOKEN_UNIT_OFFSET  32
	#define PSCH_SHP_CFG_TBL_TOKEN_UNIT_LEN     3
	#define PSCH_SHP_CFG_TBL_TOKEN_UNIT_DEFAULT 0x0
	/*[field] METER_UNIT*/
	#define PSCH_SHP_CFG_TBL_METER_UNIT
	#define PSCH_SHP_CFG_TBL_METER_UNIT_OFFSET  35
	#define PSCH_SHP_CFG_TBL_METER_UNIT_LEN     1
	#define PSCH_SHP_CFG_TBL_METER_UNIT_DEFAULT 0x0
	/*[field] SHAPER_ENABLE*/
	#define PSCH_SHP_CFG_TBL_SHAPER_ENABLE
	#define PSCH_SHP_CFG_TBL_SHAPER_ENABLE_OFFSET  36
	#define PSCH_SHP_CFG_TBL_SHAPER_ENABLE_LEN     1
	#define PSCH_SHP_CFG_TBL_SHAPER_ENABLE_DEFAULT 0x0

struct psch_shp_cfg_tbl {
	a_uint32_t  cir:18;
	a_uint32_t  cbs:14;
	a_uint32_t  token_unit:3;
	a_uint32_t  meter_unit:1;
	a_uint32_t  shaper_enable:1;
	a_uint32_t  _reserved0:27;
};

union psch_shp_cfg_tbl_u {
	a_uint32_t val[2];
	struct psch_shp_cfg_tbl bf;
};

/*[table] PSCH_COMP_TBL*/
#define PSCH_COMP_TBL
#define PSCH_COMP_TBL_ADDRESS 0x76000
#define PSCH_COMP_TBL_NUM     8
#define PSCH_COMP_TBL_INC     0x10
#define PSCH_COMP_TBL_TYPE    REG_TYPE_RO
#define PSCH_COMP_TBL_DEFAULT 0x0
	/*[field] SHAPER_COMPENSATE_BYTE_CNT*/
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_BYTE_CNT
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_BYTE_CNT_OFFSET  0
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_BYTE_CNT_LEN     18
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_BYTE_CNT_DEFAULT 0x0
	/*[field] SHAPER_COMPENSATE_BYTE_NEG*/
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_BYTE_NEG
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_BYTE_NEG_OFFSET  18
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_BYTE_NEG_LEN     1
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_BYTE_NEG_DEFAULT 0x0
	/*[field] SHAPER_COMPENSATE_PKT_CNT*/
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_PKT_CNT
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_PKT_CNT_OFFSET  19
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_PKT_CNT_LEN     4
	#define PSCH_COMP_TBL_SHAPER_COMPENSATE_PKT_CNT_DEFAULT 0x0

struct psch_comp_tbl {
	a_uint32_t  shaper_compensate_byte_cnt:18;
	a_uint32_t  shaper_compensate_byte_neg:1;
	a_uint32_t  shaper_compensate_pkt_cnt:4;
	a_uint32_t  _reserved0:9;
};

union psch_comp_tbl_u {
	a_uint32_t val;
	struct psch_comp_tbl bf;
};

/*[table] PSCH_COMP_CFG_TBL*/
#define PSCH_COMP_CFG_TBL
#define PSCH_COMP_CFG_TBL_ADDRESS 0x78000
#define PSCH_COMP_CFG_TBL_NUM     8
#define PSCH_COMP_CFG_TBL_INC     0x10
#define PSCH_COMP_CFG_TBL_TYPE    REG_TYPE_RW
#define PSCH_COMP_CFG_TBL_DEFAULT 0x0
	/*[field] SHAPER_METER_LEN*/
	#define PSCH_COMP_CFG_TBL_SHAPER_METER_LEN
	#define PSCH_COMP_CFG_TBL_SHAPER_METER_LEN_OFFSET  0
	#define PSCH_COMP_CFG_TBL_SHAPER_METER_LEN_LEN     2
	#define PSCH_COMP_CFG_TBL_SHAPER_METER_LEN_DEFAULT 0x0

struct psch_comp_cfg_tbl {
	a_uint32_t  shaper_meter_len:2;
	a_uint32_t  _reserved0:30;
};

union psch_comp_cfg_tbl_u {
	a_uint32_t val;
	struct psch_comp_cfg_tbl bf;
};

/*[register] IPG_PRE_LEN_CFG*/
#define IPG_PRE_LEN_CFG
#define IPG_PRE_LEN_CFG_ADDRESS 0x8
#define IPG_PRE_LEN_CFG_NUM     1
#define IPG_PRE_LEN_CFG_INC     0x4
#define IPG_PRE_LEN_CFG_TYPE    REG_TYPE_RW
#define IPG_PRE_LEN_CFG_DEFAULT 0x0
	/*[field] IPG_PRE_LEN*/
	#define IPG_PRE_LEN_CFG_IPG_PRE_LEN
	#define IPG_PRE_LEN_CFG_IPG_PRE_LEN_OFFSET  0
	#define IPG_PRE_LEN_CFG_IPG_PRE_LEN_LEN     5
	#define IPG_PRE_LEN_CFG_IPG_PRE_LEN_DEFAULT 0x0

struct ipg_pre_len_cfg {
	a_uint32_t  ipg_pre_len:5;
	a_uint32_t  _reserved0:27;
};

union ipg_pre_len_cfg_u {
	a_uint32_t val;
	struct ipg_pre_len_cfg bf;
};

#endif
