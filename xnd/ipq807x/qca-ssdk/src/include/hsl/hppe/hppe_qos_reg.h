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
#ifndef HPPE_QOS_REG_H
#define HPPE_QOS_REG_H

/*[register] TDM_DEPTH_CFG*/
#define TDM_DEPTH_CFG
#define TDM_DEPTH_CFG_ADDRESS 0x0
#define TDM_DEPTH_CFG_NUM     1
#define TDM_DEPTH_CFG_INC     0x4
#define TDM_DEPTH_CFG_TYPE    REG_TYPE_RW
#define TDM_DEPTH_CFG_DEFAULT 0x28
	/*[field] TDM_DEPTH*/
	#define TDM_DEPTH_CFG_TDM_DEPTH
	#define TDM_DEPTH_CFG_TDM_DEPTH_OFFSET  0
	#define TDM_DEPTH_CFG_TDM_DEPTH_LEN     8
	#define TDM_DEPTH_CFG_TDM_DEPTH_DEFAULT 0x28

struct tdm_depth_cfg {
	a_uint32_t  tdm_depth:8;
	a_uint32_t  _reserved0:24;
};

union tdm_depth_cfg_u {
	a_uint32_t val;
	struct tdm_depth_cfg bf;
};

/*[register] MIN_MAX_MODE_CFG*/
#define MIN_MAX_MODE_CFG
#define MIN_MAX_MODE_CFG_ADDRESS 0x4
#define MIN_MAX_MODE_CFG_NUM     1
#define MIN_MAX_MODE_CFG_INC     0x4
#define MIN_MAX_MODE_CFG_TYPE    REG_TYPE_RW
#define MIN_MAX_MODE_CFG_DEFAULT 0x0
	/*[field] MIN_MAX_MODE*/
	#define MIN_MAX_MODE_CFG_MIN_MAX_MODE
	#define MIN_MAX_MODE_CFG_MIN_MAX_MODE_OFFSET  0
	#define MIN_MAX_MODE_CFG_MIN_MAX_MODE_LEN     1
	#define MIN_MAX_MODE_CFG_MIN_MAX_MODE_DEFAULT 0x0

struct min_max_mode_cfg {
	a_uint32_t  min_max_mode:1;
	a_uint32_t  _reserved0:31;
};

union min_max_mode_cfg_u {
	a_uint32_t val;
	struct min_max_mode_cfg bf;
};


/*[register] TM_DBG_ADDR*/
#define TM_DBG_ADDR
#define TM_DBG_ADDR_ADDRESS 0x20
#define TM_DBG_ADDR_NUM     1
#define TM_DBG_ADDR_INC     0x4
#define TM_DBG_ADDR_TYPE    REG_TYPE_RW
#define TM_DBG_ADDR_DEFAULT 0x0
	/*[field] DBG_ADDR*/
	#define TM_DBG_ADDR_DBG_ADDR
	#define TM_DBG_ADDR_DBG_ADDR_OFFSET  0
	#define TM_DBG_ADDR_DBG_ADDR_LEN     8
	#define TM_DBG_ADDR_DBG_ADDR_DEFAULT 0x0

struct tm_dbg_addr {
	a_uint32_t  dbg_addr:8;
	a_uint32_t  _reserved0:24;
};

union tm_dbg_addr_u {
	a_uint32_t val;
	struct tm_dbg_addr bf;
};

/*[register] TM_DBG_DATA*/
#define TM_DBG_DATA
#define TM_DBG_DATA_ADDRESS 0x24
#define TM_DBG_DATA_NUM     1
#define TM_DBG_DATA_INC     0x4
#define TM_DBG_DATA_TYPE    REG_TYPE_RO
#define TM_DBG_DATA_DEFAULT 0x0
	/*[field] DBG_DATA*/
	#define TM_DBG_DATA_DBG_DATA
	#define TM_DBG_DATA_DBG_DATA_OFFSET  0
	#define TM_DBG_DATA_DBG_DATA_LEN     32
	#define TM_DBG_DATA_DBG_DATA_DEFAULT 0x0

struct tm_dbg_data {
	a_uint32_t  dbg_data:32;
};

union tm_dbg_data_u {
	a_uint32_t val;
	struct tm_dbg_data bf;
};

/*[register] ECO_RESERVE_0*/
#define ECO_RESERVE_0
#define ECO_RESERVE_0_ADDRESS 0x28
#define ECO_RESERVE_0_NUM     1
#define ECO_RESERVE_0_INC     0x4
#define ECO_RESERVE_0_TYPE    REG_TYPE_RW
#define ECO_RESERVE_0_DEFAULT 0x0
	/*[field] ECO_RES_0*/
	#define ECO_RESERVE_0_ECO_RES_0
	#define ECO_RESERVE_0_ECO_RES_0_OFFSET  0
	#define ECO_RESERVE_0_ECO_RES_0_LEN     32
	#define ECO_RESERVE_0_ECO_RES_0_DEFAULT 0x0

struct eco_reserve_0 {
	a_uint32_t  eco_res_0:32;
};

union eco_reserve_0_u {
	a_uint32_t val;
	struct eco_reserve_0 bf;
};

/*[register] ECO_RESERVE_1*/
#define ECO_RESERVE_1
#define ECO_RESERVE_1_ADDRESS 0x2c
#define ECO_RESERVE_1_NUM     1
#define ECO_RESERVE_1_INC     0x4
#define ECO_RESERVE_1_TYPE    REG_TYPE_RW
#define ECO_RESERVE_1_DEFAULT 0x0
	/*[field] ECO_RES_1*/
	#define ECO_RESERVE_1_ECO_RES_1
	#define ECO_RESERVE_1_ECO_RES_1_OFFSET  0
	#define ECO_RESERVE_1_ECO_RES_1_LEN     32
	#define ECO_RESERVE_1_ECO_RES_1_DEFAULT 0x0

struct eco_reserve_1 {
	a_uint32_t  eco_res_1:32;
};

union eco_reserve_1_u {
	a_uint32_t val;
	struct eco_reserve_1 bf;
};

/*[table] L0_FLOW_MAP_TBL*/
#define L0_FLOW_MAP_TBL
#define L0_FLOW_MAP_TBL_ADDRESS 0x2000
#define L0_FLOW_MAP_TBL_NUM     300
#define L0_FLOW_MAP_TBL_INC     0x10
#define L0_FLOW_MAP_TBL_TYPE    REG_TYPE_RW
#define L0_FLOW_MAP_TBL_DEFAULT 0x0
	/*[field] SP_ID*/
	#define L0_FLOW_MAP_TBL_SP_ID
	#define L0_FLOW_MAP_TBL_SP_ID_OFFSET  0
	#define L0_FLOW_MAP_TBL_SP_ID_LEN     6
	#define L0_FLOW_MAP_TBL_SP_ID_DEFAULT 0x0
	/*[field] C_PRI*/
	#define L0_FLOW_MAP_TBL_C_PRI
	#define L0_FLOW_MAP_TBL_C_PRI_OFFSET  6
	#define L0_FLOW_MAP_TBL_C_PRI_LEN     3
	#define L0_FLOW_MAP_TBL_C_PRI_DEFAULT 0x0
	/*[field] E_PRI*/
	#define L0_FLOW_MAP_TBL_E_PRI
	#define L0_FLOW_MAP_TBL_E_PRI_OFFSET  9
	#define L0_FLOW_MAP_TBL_E_PRI_LEN     3
	#define L0_FLOW_MAP_TBL_E_PRI_DEFAULT 0x0
	/*[field] C_DRR_WT*/
	#define L0_FLOW_MAP_TBL_C_DRR_WT
	#define L0_FLOW_MAP_TBL_C_DRR_WT_OFFSET  12
	#define L0_FLOW_MAP_TBL_C_DRR_WT_LEN     10
	#define L0_FLOW_MAP_TBL_C_DRR_WT_DEFAULT 0x0
	/*[field] E_DRR_WT*/
	#define L0_FLOW_MAP_TBL_E_DRR_WT
	#define L0_FLOW_MAP_TBL_E_DRR_WT_OFFSET  22
	#define L0_FLOW_MAP_TBL_E_DRR_WT_LEN     10
	#define L0_FLOW_MAP_TBL_E_DRR_WT_DEFAULT 0x0

struct l0_flow_map_tbl {
	a_uint32_t  sp_id:6;
	a_uint32_t  c_pri:3;
	a_uint32_t  e_pri:3;
	a_uint32_t  c_drr_wt:10;
	a_uint32_t  e_drr_wt:10;
};

union l0_flow_map_tbl_u {
	a_uint32_t val;
	struct l0_flow_map_tbl bf;
};

/*[table] L0_C_SP_CFG_TBL*/
#define L0_C_SP_CFG_TBL
#define L0_C_SP_CFG_TBL_ADDRESS 0x4000
#define L0_C_SP_CFG_TBL_NUM     512
#define L0_C_SP_CFG_TBL_INC     0x10
#define L0_C_SP_CFG_TBL_TYPE    REG_TYPE_RW
#define L0_C_SP_CFG_TBL_DEFAULT 0x0
	/*[field] DRR_ID*/
	#define L0_C_SP_CFG_TBL_DRR_ID
	#define L0_C_SP_CFG_TBL_DRR_ID_OFFSET  0
	#define L0_C_SP_CFG_TBL_DRR_ID_LEN     8
	#define L0_C_SP_CFG_TBL_DRR_ID_DEFAULT 0x0
	/*[field] DRR_CREDIT_UNIT*/
	#define L0_C_SP_CFG_TBL_DRR_CREDIT_UNIT
	#define L0_C_SP_CFG_TBL_DRR_CREDIT_UNIT_OFFSET  8
	#define L0_C_SP_CFG_TBL_DRR_CREDIT_UNIT_LEN     1
	#define L0_C_SP_CFG_TBL_DRR_CREDIT_UNIT_DEFAULT 0x0

struct l0_c_sp_cfg_tbl {
	a_uint32_t  drr_id:8;
	a_uint32_t  drr_credit_unit:1;
	a_uint32_t  _reserved0:23;
};

union l0_c_sp_cfg_tbl_u {
	a_uint32_t val;
	struct l0_c_sp_cfg_tbl bf;
};

/*[table] L0_E_SP_CFG_TBL*/
#define L0_E_SP_CFG_TBL
#define L0_E_SP_CFG_TBL_ADDRESS 0x6000
#define L0_E_SP_CFG_TBL_NUM     512
#define L0_E_SP_CFG_TBL_INC     0x10
#define L0_E_SP_CFG_TBL_TYPE    REG_TYPE_RW
#define L0_E_SP_CFG_TBL_DEFAULT 0x0
	/*[field] DRR_ID*/
	#define L0_E_SP_CFG_TBL_DRR_ID
	#define L0_E_SP_CFG_TBL_DRR_ID_OFFSET  0
	#define L0_E_SP_CFG_TBL_DRR_ID_LEN     8
	#define L0_E_SP_CFG_TBL_DRR_ID_DEFAULT 0x0
	/*[field] DRR_CREDIT_UNIT*/
	#define L0_E_SP_CFG_TBL_DRR_CREDIT_UNIT
	#define L0_E_SP_CFG_TBL_DRR_CREDIT_UNIT_OFFSET  8
	#define L0_E_SP_CFG_TBL_DRR_CREDIT_UNIT_LEN     1
	#define L0_E_SP_CFG_TBL_DRR_CREDIT_UNIT_DEFAULT 0x0

struct l0_e_sp_cfg_tbl {
	a_uint32_t  drr_id:8;
	a_uint32_t  drr_credit_unit:1;
	a_uint32_t  _reserved0:23;
};

union l0_e_sp_cfg_tbl_u {
	a_uint32_t val;
	struct l0_e_sp_cfg_tbl bf;
};

/*[table] L0_FLOW_PORT_MAP_TBL*/
#define L0_FLOW_PORT_MAP_TBL
#define L0_FLOW_PORT_MAP_TBL_ADDRESS 0x8000
#define L0_FLOW_PORT_MAP_TBL_NUM     300
#define L0_FLOW_PORT_MAP_TBL_INC     0x10
#define L0_FLOW_PORT_MAP_TBL_TYPE    REG_TYPE_RW
#define L0_FLOW_PORT_MAP_TBL_DEFAULT 0x0
	/*[field] PORT_NUM*/
	#define L0_FLOW_PORT_MAP_TBL_PORT_NUM
	#define L0_FLOW_PORT_MAP_TBL_PORT_NUM_OFFSET  0
	#define L0_FLOW_PORT_MAP_TBL_PORT_NUM_LEN     4
	#define L0_FLOW_PORT_MAP_TBL_PORT_NUM_DEFAULT 0x0

struct l0_flow_port_map_tbl {
	a_uint32_t  port_num:4;
	a_uint32_t  _reserved0:28;
};

union l0_flow_port_map_tbl_u {
	a_uint32_t val;
	struct l0_flow_port_map_tbl bf;
};

/*[table] L0_C_DRR_HEAD_TBL*/
#define L0_C_DRR_HEAD_TBL
#define L0_C_DRR_HEAD_TBL_ADDRESS 0xa000
#define L0_C_DRR_HEAD_TBL_NUM     160
#define L0_C_DRR_HEAD_TBL_INC     0x10
#define L0_C_DRR_HEAD_TBL_TYPE    REG_TYPE_RO
#define L0_C_DRR_HEAD_TBL_DEFAULT 0x0
	/*[field] BACKUP_TAIL*/
	#define L0_C_DRR_HEAD_TBL_BACKUP_TAIL
	#define L0_C_DRR_HEAD_TBL_BACKUP_TAIL_OFFSET  0
	#define L0_C_DRR_HEAD_TBL_BACKUP_TAIL_LEN     9
	#define L0_C_DRR_HEAD_TBL_BACKUP_TAIL_DEFAULT 0x0
	/*[field] BACKUP_HEAD*/
	#define L0_C_DRR_HEAD_TBL_BACKUP_HEAD
	#define L0_C_DRR_HEAD_TBL_BACKUP_HEAD_OFFSET  9
	#define L0_C_DRR_HEAD_TBL_BACKUP_HEAD_LEN     9
	#define L0_C_DRR_HEAD_TBL_BACKUP_HEAD_DEFAULT 0x0
	/*[field] BACKUP_VLD*/
	#define L0_C_DRR_HEAD_TBL_BACKUP_VLD
	#define L0_C_DRR_HEAD_TBL_BACKUP_VLD_OFFSET  18
	#define L0_C_DRR_HEAD_TBL_BACKUP_VLD_LEN     1
	#define L0_C_DRR_HEAD_TBL_BACKUP_VLD_DEFAULT 0x0
	/*[field] BACKUP_MAX_N*/
	#define L0_C_DRR_HEAD_TBL_BACKUP_MAX_N
	#define L0_C_DRR_HEAD_TBL_BACKUP_MAX_N_OFFSET  19
	#define L0_C_DRR_HEAD_TBL_BACKUP_MAX_N_LEN     5
	#define L0_C_DRR_HEAD_TBL_BACKUP_MAX_N_DEFAULT 0x0
	/*[field] ACTIVE_TAIL*/
	#define L0_C_DRR_HEAD_TBL_ACTIVE_TAIL
	#define L0_C_DRR_HEAD_TBL_ACTIVE_TAIL_OFFSET  24
	#define L0_C_DRR_HEAD_TBL_ACTIVE_TAIL_LEN     9
	#define L0_C_DRR_HEAD_TBL_ACTIVE_TAIL_DEFAULT 0x0
	/*[field] ACTIVE_HEAD*/
	#define L0_C_DRR_HEAD_TBL_ACTIVE_HEAD
	#define L0_C_DRR_HEAD_TBL_ACTIVE_HEAD_OFFSET  33
	#define L0_C_DRR_HEAD_TBL_ACTIVE_HEAD_LEN     9
	#define L0_C_DRR_HEAD_TBL_ACTIVE_HEAD_DEFAULT 0x0
	/*[field] ACTIVE_VLD*/
	#define L0_C_DRR_HEAD_TBL_ACTIVE_VLD
	#define L0_C_DRR_HEAD_TBL_ACTIVE_VLD_OFFSET  42
	#define L0_C_DRR_HEAD_TBL_ACTIVE_VLD_LEN     1
	#define L0_C_DRR_HEAD_TBL_ACTIVE_VLD_DEFAULT 0x0
	/*[field] ACTIVE_MAX_N*/
	#define L0_C_DRR_HEAD_TBL_ACTIVE_MAX_N
	#define L0_C_DRR_HEAD_TBL_ACTIVE_MAX_N_OFFSET  43
	#define L0_C_DRR_HEAD_TBL_ACTIVE_MAX_N_LEN     5
	#define L0_C_DRR_HEAD_TBL_ACTIVE_MAX_N_DEFAULT 0x0

struct l0_c_drr_head_tbl {
	a_uint32_t  backup_tail:9;
	a_uint32_t  backup_head:9;
	a_uint32_t  backup_vld:1;
	a_uint32_t  backup_max_n:5;
	a_uint32_t  active_tail_0:8;
	a_uint32_t  active_tail_1:1;
	a_uint32_t  active_head:9;
	a_uint32_t  active_vld:1;
	a_uint32_t  active_max_n:5;
	a_uint32_t  _reserved0:16;
};

union l0_c_drr_head_tbl_u {
	a_uint32_t val[2];
	struct l0_c_drr_head_tbl bf;
};

/*[table] L0_E_DRR_HEAD_TBL*/
#define L0_E_DRR_HEAD_TBL
#define L0_E_DRR_HEAD_TBL_ADDRESS 0xc000
#define L0_E_DRR_HEAD_TBL_NUM     160
#define L0_E_DRR_HEAD_TBL_INC     0x10
#define L0_E_DRR_HEAD_TBL_TYPE    REG_TYPE_RO
#define L0_E_DRR_HEAD_TBL_DEFAULT 0x0
	/*[field] BACKUP_TAIL*/
	#define L0_E_DRR_HEAD_TBL_BACKUP_TAIL
	#define L0_E_DRR_HEAD_TBL_BACKUP_TAIL_OFFSET  0
	#define L0_E_DRR_HEAD_TBL_BACKUP_TAIL_LEN     9
	#define L0_E_DRR_HEAD_TBL_BACKUP_TAIL_DEFAULT 0x0
	/*[field] BACKUP_HEAD*/
	#define L0_E_DRR_HEAD_TBL_BACKUP_HEAD
	#define L0_E_DRR_HEAD_TBL_BACKUP_HEAD_OFFSET  9
	#define L0_E_DRR_HEAD_TBL_BACKUP_HEAD_LEN     9
	#define L0_E_DRR_HEAD_TBL_BACKUP_HEAD_DEFAULT 0x0
	/*[field] BACKUP_VLD*/
	#define L0_E_DRR_HEAD_TBL_BACKUP_VLD
	#define L0_E_DRR_HEAD_TBL_BACKUP_VLD_OFFSET  18
	#define L0_E_DRR_HEAD_TBL_BACKUP_VLD_LEN     1
	#define L0_E_DRR_HEAD_TBL_BACKUP_VLD_DEFAULT 0x0
	/*[field] BACKUP_MAX_N*/
	#define L0_E_DRR_HEAD_TBL_BACKUP_MAX_N
	#define L0_E_DRR_HEAD_TBL_BACKUP_MAX_N_OFFSET  19
	#define L0_E_DRR_HEAD_TBL_BACKUP_MAX_N_LEN     5
	#define L0_E_DRR_HEAD_TBL_BACKUP_MAX_N_DEFAULT 0x0
	/*[field] ACTIVE_TAIL*/
	#define L0_E_DRR_HEAD_TBL_ACTIVE_TAIL
	#define L0_E_DRR_HEAD_TBL_ACTIVE_TAIL_OFFSET  24
	#define L0_E_DRR_HEAD_TBL_ACTIVE_TAIL_LEN     9
	#define L0_E_DRR_HEAD_TBL_ACTIVE_TAIL_DEFAULT 0x0
	/*[field] ACTIVE_HEAD*/
	#define L0_E_DRR_HEAD_TBL_ACTIVE_HEAD
	#define L0_E_DRR_HEAD_TBL_ACTIVE_HEAD_OFFSET  33
	#define L0_E_DRR_HEAD_TBL_ACTIVE_HEAD_LEN     9
	#define L0_E_DRR_HEAD_TBL_ACTIVE_HEAD_DEFAULT 0x0
	/*[field] ACTIVE_VLD*/
	#define L0_E_DRR_HEAD_TBL_ACTIVE_VLD
	#define L0_E_DRR_HEAD_TBL_ACTIVE_VLD_OFFSET  42
	#define L0_E_DRR_HEAD_TBL_ACTIVE_VLD_LEN     1
	#define L0_E_DRR_HEAD_TBL_ACTIVE_VLD_DEFAULT 0x0
	/*[field] ACTIVE_MAX_N*/
	#define L0_E_DRR_HEAD_TBL_ACTIVE_MAX_N
	#define L0_E_DRR_HEAD_TBL_ACTIVE_MAX_N_OFFSET  43
	#define L0_E_DRR_HEAD_TBL_ACTIVE_MAX_N_LEN     5
	#define L0_E_DRR_HEAD_TBL_ACTIVE_MAX_N_DEFAULT 0x0

struct l0_e_drr_head_tbl {
	a_uint32_t  backup_tail:9;
	a_uint32_t  backup_head:9;
	a_uint32_t  backup_vld:1;
	a_uint32_t  backup_max_n:5;
	a_uint32_t  active_tail_0:8;
	a_uint32_t  active_tail_1:1;
	a_uint32_t  active_head:9;
	a_uint32_t  active_vld:1;
	a_uint32_t  active_max_n:5;
	a_uint32_t  _reserved0:16;
};

union l0_e_drr_head_tbl_u {
	a_uint32_t val[2];
	struct l0_e_drr_head_tbl bf;
};

/*[table] L0_DRR_CREDIT_TBL*/
#define L0_DRR_CREDIT_TBL
#define L0_DRR_CREDIT_TBL_ADDRESS 0xe000
#define L0_DRR_CREDIT_TBL_NUM     300
#define L0_DRR_CREDIT_TBL_INC     0x10
#define L0_DRR_CREDIT_TBL_TYPE    REG_TYPE_RO
#define L0_DRR_CREDIT_TBL_DEFAULT 0x0
	/*[field] C_DRR_CREDIT*/
	#define L0_DRR_CREDIT_TBL_C_DRR_CREDIT
	#define L0_DRR_CREDIT_TBL_C_DRR_CREDIT_OFFSET  0
	#define L0_DRR_CREDIT_TBL_C_DRR_CREDIT_LEN     24
	#define L0_DRR_CREDIT_TBL_C_DRR_CREDIT_DEFAULT 0x0
	/*[field] C_DRR_CREDIT_NEG*/
	#define L0_DRR_CREDIT_TBL_C_DRR_CREDIT_NEG
	#define L0_DRR_CREDIT_TBL_C_DRR_CREDIT_NEG_OFFSET  24
	#define L0_DRR_CREDIT_TBL_C_DRR_CREDIT_NEG_LEN     1
	#define L0_DRR_CREDIT_TBL_C_DRR_CREDIT_NEG_DEFAULT 0x0
	/*[field] E_DRR_CREDIT*/
	#define L0_DRR_CREDIT_TBL_E_DRR_CREDIT
	#define L0_DRR_CREDIT_TBL_E_DRR_CREDIT_OFFSET  25
	#define L0_DRR_CREDIT_TBL_E_DRR_CREDIT_LEN     24
	#define L0_DRR_CREDIT_TBL_E_DRR_CREDIT_DEFAULT 0x0
	/*[field] E_DRR_CREDIT_NEG*/
	#define L0_DRR_CREDIT_TBL_E_DRR_CREDIT_NEG
	#define L0_DRR_CREDIT_TBL_E_DRR_CREDIT_NEG_OFFSET  49
	#define L0_DRR_CREDIT_TBL_E_DRR_CREDIT_NEG_LEN     1
	#define L0_DRR_CREDIT_TBL_E_DRR_CREDIT_NEG_DEFAULT 0x0

struct l0_drr_credit_tbl {
	a_uint32_t  c_drr_credit:24;
	a_uint32_t  c_drr_credit_neg:1;
	a_uint32_t  e_drr_credit_0:7;
	a_uint32_t  e_drr_credit_1:17;
	a_uint32_t  e_drr_credit_neg:1;
	a_uint32_t  _reserved0:14;
};

union l0_drr_credit_tbl_u {
	a_uint32_t val[2];
	struct l0_drr_credit_tbl bf;
};

/*[table] L0_C_DRR_LL_TBL*/
#define L0_C_DRR_LL_TBL
#define L0_C_DRR_LL_TBL_ADDRESS 0x10000
#define L0_C_DRR_LL_TBL_NUM     300
#define L0_C_DRR_LL_TBL_INC     0x10
#define L0_C_DRR_LL_TBL_TYPE    REG_TYPE_RO
#define L0_C_DRR_LL_TBL_DEFAULT 0x0
	/*[field] NEXT_PTR*/
	#define L0_C_DRR_LL_TBL_NEXT_PTR
	#define L0_C_DRR_LL_TBL_NEXT_PTR_OFFSET  0
	#define L0_C_DRR_LL_TBL_NEXT_PTR_LEN     9
	#define L0_C_DRR_LL_TBL_NEXT_PTR_DEFAULT 0x0

struct l0_c_drr_ll_tbl {
	a_uint32_t  next_ptr:9;
	a_uint32_t  _reserved0:23;
};

union l0_c_drr_ll_tbl_u {
	a_uint32_t val;
	struct l0_c_drr_ll_tbl bf;
};

/*[table] L0_C_DRR_REVERSE_LL_TBL*/
#define L0_C_DRR_REVERSE_LL_TBL
#define L0_C_DRR_REVERSE_LL_TBL_ADDRESS 0x12000
#define L0_C_DRR_REVERSE_LL_TBL_NUM     300
#define L0_C_DRR_REVERSE_LL_TBL_INC     0x10
#define L0_C_DRR_REVERSE_LL_TBL_TYPE    REG_TYPE_RO
#define L0_C_DRR_REVERSE_LL_TBL_DEFAULT 0x0
	/*[field] PRE_PTR*/
	#define L0_C_DRR_REVERSE_LL_TBL_PRE_PTR
	#define L0_C_DRR_REVERSE_LL_TBL_PRE_PTR_OFFSET  0
	#define L0_C_DRR_REVERSE_LL_TBL_PRE_PTR_LEN     9
	#define L0_C_DRR_REVERSE_LL_TBL_PRE_PTR_DEFAULT 0x0

struct l0_c_drr_reverse_ll_tbl {
	a_uint32_t  pre_ptr:9;
	a_uint32_t  _reserved0:23;
};

union l0_c_drr_reverse_ll_tbl_u {
	a_uint32_t val;
	struct l0_c_drr_reverse_ll_tbl bf;
};

/*[table] L0_E_DRR_LL_TBL*/
#define L0_E_DRR_LL_TBL
#define L0_E_DRR_LL_TBL_ADDRESS 0x14000
#define L0_E_DRR_LL_TBL_NUM     300
#define L0_E_DRR_LL_TBL_INC     0x10
#define L0_E_DRR_LL_TBL_TYPE    REG_TYPE_RO
#define L0_E_DRR_LL_TBL_DEFAULT 0x0
	/*[field] NEXT_PTR*/
	#define L0_E_DRR_LL_TBL_NEXT_PTR
	#define L0_E_DRR_LL_TBL_NEXT_PTR_OFFSET  0
	#define L0_E_DRR_LL_TBL_NEXT_PTR_LEN     9
	#define L0_E_DRR_LL_TBL_NEXT_PTR_DEFAULT 0x0

struct l0_e_drr_ll_tbl {
	a_uint32_t  next_ptr:9;
	a_uint32_t  _reserved0:23;
};

union l0_e_drr_ll_tbl_u {
	a_uint32_t val;
	struct l0_e_drr_ll_tbl bf;
};

/*[table] L0_E_DRR_REVERSE_LL_TBL*/
#define L0_E_DRR_REVERSE_LL_TBL
#define L0_E_DRR_REVERSE_LL_TBL_ADDRESS 0x16000
#define L0_E_DRR_REVERSE_LL_TBL_NUM     300
#define L0_E_DRR_REVERSE_LL_TBL_INC     0x10
#define L0_E_DRR_REVERSE_LL_TBL_TYPE    REG_TYPE_RO
#define L0_E_DRR_REVERSE_LL_TBL_DEFAULT 0x0
	/*[field] PRE_PTR*/
	#define L0_E_DRR_REVERSE_LL_TBL_PRE_PTR
	#define L0_E_DRR_REVERSE_LL_TBL_PRE_PTR_OFFSET  0
	#define L0_E_DRR_REVERSE_LL_TBL_PRE_PTR_LEN     9
	#define L0_E_DRR_REVERSE_LL_TBL_PRE_PTR_DEFAULT 0x0

struct l0_e_drr_reverse_ll_tbl {
	a_uint32_t  pre_ptr:9;
	a_uint32_t  _reserved0:23;
};

union l0_e_drr_reverse_ll_tbl_u {
	a_uint32_t val;
	struct l0_e_drr_reverse_ll_tbl bf;
};

/*[table] L0_SP_ENTRY_TBL*/
#define L0_SP_ENTRY_TBL
#define L0_SP_ENTRY_TBL_ADDRESS 0x18000
#define L0_SP_ENTRY_TBL_NUM     64
#define L0_SP_ENTRY_TBL_INC     0x20
#define L0_SP_ENTRY_TBL_TYPE    REG_TYPE_RO
#define L0_SP_ENTRY_TBL_DEFAULT 0x0
	/*[field] ENTRY_PATH_ID*/
	#define L0_SP_ENTRY_TBL_ENTRY_PATH_ID
	#define L0_SP_ENTRY_TBL_ENTRY_PATH_ID_OFFSET  0
	#define L0_SP_ENTRY_TBL_ENTRY_PATH_ID_LEN     144
	#define L0_SP_ENTRY_TBL_ENTRY_PATH_ID_DEFAULT 0x0
	/*[field] ENTRY_VLD*/
	#define L0_SP_ENTRY_TBL_ENTRY_VLD
	#define L0_SP_ENTRY_TBL_ENTRY_VLD_OFFSET  144
	#define L0_SP_ENTRY_TBL_ENTRY_VLD_LEN     16
	#define L0_SP_ENTRY_TBL_ENTRY_VLD_DEFAULT 0x0

struct l0_sp_entry_tbl {
	a_uint32_t  entry_path_id_0:32;
	a_uint32_t  entry_path_id_1:32;
	a_uint32_t  entry_path_id_2:32;
	a_uint32_t  entry_path_id_3:32;
	a_uint32_t  entry_path_id_4:16;
	a_uint32_t  entry_vld:16;
};

union l0_sp_entry_tbl_u {
	a_uint32_t val[5];
	struct l0_sp_entry_tbl bf;
};


/*[table] L0_ENS_Q_LL_TBL*/
#define L0_ENS_Q_LL_TBL
#define L0_ENS_Q_LL_TBL_ADDRESS 0x1e000
#define L0_ENS_Q_LL_TBL_NUM     300
#define L0_ENS_Q_LL_TBL_INC     0x10
#define L0_ENS_Q_LL_TBL_TYPE    REG_TYPE_RO
#define L0_ENS_Q_LL_TBL_DEFAULT 0x0
	/*[field] NEXT_PTR*/
	#define L0_ENS_Q_LL_TBL_NEXT_PTR
	#define L0_ENS_Q_LL_TBL_NEXT_PTR_OFFSET  0
	#define L0_ENS_Q_LL_TBL_NEXT_PTR_LEN     9
	#define L0_ENS_Q_LL_TBL_NEXT_PTR_DEFAULT 0x0

struct l0_ens_q_ll_tbl {
	a_uint32_t  next_ptr:9;
	a_uint32_t  _reserved0:23;
};

union l0_ens_q_ll_tbl_u {
	a_uint32_t val;
	struct l0_ens_q_ll_tbl bf;
};

/*[table] L0_ENS_Q_HEAD_TBL*/
#define L0_ENS_Q_HEAD_TBL
#define L0_ENS_Q_HEAD_TBL_ADDRESS 0x20000
#define L0_ENS_Q_HEAD_TBL_NUM     8
#define L0_ENS_Q_HEAD_TBL_INC     0x10
#define L0_ENS_Q_HEAD_TBL_TYPE    REG_TYPE_RO
#define L0_ENS_Q_HEAD_TBL_DEFAULT 0x0
	/*[field] TAIL*/
	#define L0_ENS_Q_HEAD_TBL_TAIL
	#define L0_ENS_Q_HEAD_TBL_TAIL_OFFSET  0
	#define L0_ENS_Q_HEAD_TBL_TAIL_LEN     9
	#define L0_ENS_Q_HEAD_TBL_TAIL_DEFAULT 0x0
	/*[field] HEAD*/
	#define L0_ENS_Q_HEAD_TBL_HEAD
	#define L0_ENS_Q_HEAD_TBL_HEAD_OFFSET  9
	#define L0_ENS_Q_HEAD_TBL_HEAD_LEN     9
	#define L0_ENS_Q_HEAD_TBL_HEAD_DEFAULT 0x0
	/*[field] VLD*/
	#define L0_ENS_Q_HEAD_TBL_VLD
	#define L0_ENS_Q_HEAD_TBL_VLD_OFFSET  18
	#define L0_ENS_Q_HEAD_TBL_VLD_LEN     1
	#define L0_ENS_Q_HEAD_TBL_VLD_DEFAULT 0x0

struct l0_ens_q_head_tbl {
	a_uint32_t  tail:9;
	a_uint32_t  head:9;
	a_uint32_t  vld:1;
	a_uint32_t  _reserved0:13;
};

union l0_ens_q_head_tbl_u {
	a_uint32_t val;
	struct l0_ens_q_head_tbl bf;
};

/*[table] L0_ENS_Q_ENTRY_TBL*/
#define L0_ENS_Q_ENTRY_TBL
#define L0_ENS_Q_ENTRY_TBL_ADDRESS 0x22000
#define L0_ENS_Q_ENTRY_TBL_NUM     300
#define L0_ENS_Q_ENTRY_TBL_INC     0x10
#define L0_ENS_Q_ENTRY_TBL_TYPE    REG_TYPE_RO
#define L0_ENS_Q_ENTRY_TBL_DEFAULT 0x0
	/*[field] ENTRY_ENS_TYPE*/
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_TYPE
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_TYPE_OFFSET  0
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_TYPE_LEN     2
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_TYPE_DEFAULT 0x0
	/*[field] ENTRY_ENS_VLD*/
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_VLD
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_VLD_OFFSET  2
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_VLD_LEN     1
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_VLD_DEFAULT 0x0
	/*[field] ENTRY_ENS_IN_Q*/
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_IN_Q
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_IN_Q_OFFSET  3
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_IN_Q_LEN     1
	#define L0_ENS_Q_ENTRY_TBL_ENTRY_ENS_IN_Q_DEFAULT 0x0

struct l0_ens_q_entry_tbl {
	a_uint32_t  entry_ens_type:2;
	a_uint32_t  entry_ens_vld:1;
	a_uint32_t  entry_ens_in_q:1;
	a_uint32_t  _reserved0:28;
};

union l0_ens_q_entry_tbl_u {
	a_uint32_t val;
	struct l0_ens_q_entry_tbl bf;
};

/*[table] L0_FLOW_STATUS_TBL*/
#define L0_FLOW_STATUS_TBL
#define L0_FLOW_STATUS_TBL_ADDRESS 0x24000
#define L0_FLOW_STATUS_TBL_NUM     300
#define L0_FLOW_STATUS_TBL_INC     0x10
#define L0_FLOW_STATUS_TBL_TYPE    REG_TYPE_RO
#define L0_FLOW_STATUS_TBL_DEFAULT 0x0
	/*[field] EN_LEVEL*/
	#define L0_FLOW_STATUS_TBL_EN_LEVEL
	#define L0_FLOW_STATUS_TBL_EN_LEVEL_OFFSET  0
	#define L0_FLOW_STATUS_TBL_EN_LEVEL_LEN     1
	#define L0_FLOW_STATUS_TBL_EN_LEVEL_DEFAULT 0x0
	/*[field] EN_CDRR*/
	#define L0_FLOW_STATUS_TBL_EN_CDRR
	#define L0_FLOW_STATUS_TBL_EN_CDRR_OFFSET  1
	#define L0_FLOW_STATUS_TBL_EN_CDRR_LEN     1
	#define L0_FLOW_STATUS_TBL_EN_CDRR_DEFAULT 0x0
	/*[field] EN_EDRR*/
	#define L0_FLOW_STATUS_TBL_EN_EDRR
	#define L0_FLOW_STATUS_TBL_EN_EDRR_OFFSET  2
	#define L0_FLOW_STATUS_TBL_EN_EDRR_LEN     1
	#define L0_FLOW_STATUS_TBL_EN_EDRR_DEFAULT 0x0

struct l0_flow_status_tbl {
	a_uint32_t  en_level:1;
	a_uint32_t  en_cdrr:1;
	a_uint32_t  en_edrr:1;
	a_uint32_t  _reserved0:29;
};

union l0_flow_status_tbl_u {
	a_uint32_t val;
	struct l0_flow_status_tbl bf;
};


/*[table] RING_Q_MAP_TBL*/
#define RING_Q_MAP_TBL
#define RING_Q_MAP_TBL_ADDRESS 0x2a000
#define RING_Q_MAP_TBL_NUM     16
#define RING_Q_MAP_TBL_INC     0x40
#define RING_Q_MAP_TBL_TYPE    REG_TYPE_RW
#define RING_Q_MAP_TBL_DEFAULT 0x0
	/*[field] QUEUE_BITMAP*/
	#define RING_Q_MAP_TBL_QUEUE_BITMAP
	#define RING_Q_MAP_TBL_QUEUE_BITMAP_OFFSET  0
	#define RING_Q_MAP_TBL_QUEUE_BITMAP_LEN     300
	#define RING_Q_MAP_TBL_QUEUE_BITMAP_DEFAULT 0x0

struct ring_q_map_tbl {
	a_uint32_t  queue_bitmap_0:32;
	a_uint32_t  queue_bitmap_1:32;
	a_uint32_t  queue_bitmap_2:32;
	a_uint32_t  queue_bitmap_3:32;
	a_uint32_t  queue_bitmap_4:32;
	a_uint32_t  queue_bitmap_5:32;
	a_uint32_t  queue_bitmap_6:32;
	a_uint32_t  queue_bitmap_7:32;
	a_uint32_t  queue_bitmap_8:32;
	a_uint32_t  queue_bitmap_9:12;
	a_uint32_t  _reserved0:20;
};

union ring_q_map_tbl_u {
	a_uint32_t val[10];
	struct ring_q_map_tbl bf;
};

/*[table] RFC_BLOCK_TBL*/
#define RFC_BLOCK_TBL
#define RFC_BLOCK_TBL_ADDRESS 0x2c000
#define RFC_BLOCK_TBL_NUM     300
#define RFC_BLOCK_TBL_INC     0x10
#define RFC_BLOCK_TBL_TYPE    REG_TYPE_RO
#define RFC_BLOCK_TBL_DEFAULT 0x0
	/*[field] RFC_BLOCK*/
	#define RFC_BLOCK_TBL_RFC_BLOCK
	#define RFC_BLOCK_TBL_RFC_BLOCK_OFFSET  0
	#define RFC_BLOCK_TBL_RFC_BLOCK_LEN     1
	#define RFC_BLOCK_TBL_RFC_BLOCK_DEFAULT 0x0

struct rfc_block_tbl {
	a_uint32_t  rfc_block:1;
	a_uint32_t  _reserved0:31;
};

union rfc_block_tbl_u {
	a_uint32_t val;
	struct rfc_block_tbl bf;
};

/*[table] RFC_STATUS_TBL*/
#define RFC_STATUS_TBL
#define RFC_STATUS_TBL_ADDRESS 0x2e000
#define RFC_STATUS_TBL_NUM     300
#define RFC_STATUS_TBL_INC     0x10
#define RFC_STATUS_TBL_TYPE    REG_TYPE_RO
#define RFC_STATUS_TBL_DEFAULT 0x0
	/*[field] RFC_STATUS*/
	#define RFC_STATUS_TBL_RFC_STATUS
	#define RFC_STATUS_TBL_RFC_STATUS_OFFSET  0
	#define RFC_STATUS_TBL_RFC_STATUS_LEN     1
	#define RFC_STATUS_TBL_RFC_STATUS_DEFAULT 0x0

struct rfc_status_tbl {
	a_uint32_t  rfc_status:1;
	a_uint32_t  _reserved0:31;
};

union rfc_status_tbl_u {
	a_uint32_t val;
	struct rfc_status_tbl bf;
};

/*[table] DEQ_DIS_TBL*/
#define DEQ_DIS_TBL
#define DEQ_DIS_TBL_ADDRESS 0x30000
#define DEQ_DIS_TBL_NUM     300
#define DEQ_DIS_TBL_INC     0x10
#define DEQ_DIS_TBL_TYPE    REG_TYPE_RW
#define DEQ_DIS_TBL_DEFAULT 0x0
	/*[field] DEQ_DIS*/
	#define DEQ_DIS_TBL_DEQ_DIS
	#define DEQ_DIS_TBL_DEQ_DIS_OFFSET  0
	#define DEQ_DIS_TBL_DEQ_DIS_LEN     1
	#define DEQ_DIS_TBL_DEQ_DIS_DEFAULT 0x0

struct deq_dis_tbl {
	a_uint32_t  deq_dis:1;
	a_uint32_t  _reserved0:31;
};

union deq_dis_tbl_u {
	a_uint32_t val;
	struct deq_dis_tbl bf;
};

/*[table] L1_FLOW_MAP_TBL*/
#define L1_FLOW_MAP_TBL
#define L1_FLOW_MAP_TBL_ADDRESS 0x40000
#define L1_FLOW_MAP_TBL_NUM     64
#define L1_FLOW_MAP_TBL_INC     0x10
#define L1_FLOW_MAP_TBL_TYPE    REG_TYPE_RW
#define L1_FLOW_MAP_TBL_DEFAULT 0x0
	/*[field] SP_ID*/
	#define L1_FLOW_MAP_TBL_SP_ID
	#define L1_FLOW_MAP_TBL_SP_ID_OFFSET  0
	#define L1_FLOW_MAP_TBL_SP_ID_LEN     4
	#define L1_FLOW_MAP_TBL_SP_ID_DEFAULT 0x0
	/*[field] C_PRI*/
	#define L1_FLOW_MAP_TBL_C_PRI
	#define L1_FLOW_MAP_TBL_C_PRI_OFFSET  4
	#define L1_FLOW_MAP_TBL_C_PRI_LEN     3
	#define L1_FLOW_MAP_TBL_C_PRI_DEFAULT 0x0
	/*[field] E_PRI*/
	#define L1_FLOW_MAP_TBL_E_PRI
	#define L1_FLOW_MAP_TBL_E_PRI_OFFSET  7
	#define L1_FLOW_MAP_TBL_E_PRI_LEN     3
	#define L1_FLOW_MAP_TBL_E_PRI_DEFAULT 0x0
	/*[field] C_DRR_WT*/
	#define L1_FLOW_MAP_TBL_C_DRR_WT
	#define L1_FLOW_MAP_TBL_C_DRR_WT_OFFSET  10
	#define L1_FLOW_MAP_TBL_C_DRR_WT_LEN     10
	#define L1_FLOW_MAP_TBL_C_DRR_WT_DEFAULT 0x0
	/*[field] E_DRR_WT*/
	#define L1_FLOW_MAP_TBL_E_DRR_WT
	#define L1_FLOW_MAP_TBL_E_DRR_WT_OFFSET  20
	#define L1_FLOW_MAP_TBL_E_DRR_WT_LEN     10
	#define L1_FLOW_MAP_TBL_E_DRR_WT_DEFAULT 0x0

struct l1_flow_map_tbl {
	a_uint32_t  sp_id:4;
	a_uint32_t  c_pri:3;
	a_uint32_t  e_pri:3;
	a_uint32_t  c_drr_wt:10;
	a_uint32_t  e_drr_wt:10;
	a_uint32_t  _reserved0:2;
};

union l1_flow_map_tbl_u {
	a_uint32_t val;
	struct l1_flow_map_tbl bf;
};

/*[table] L1_C_SP_CFG_TBL*/
#define L1_C_SP_CFG_TBL
#define L1_C_SP_CFG_TBL_ADDRESS 0x42000
#define L1_C_SP_CFG_TBL_NUM     64
#define L1_C_SP_CFG_TBL_INC     0x10
#define L1_C_SP_CFG_TBL_TYPE    REG_TYPE_RW
#define L1_C_SP_CFG_TBL_DEFAULT 0x0
	/*[field] DRR_ID*/
	#define L1_C_SP_CFG_TBL_DRR_ID
	#define L1_C_SP_CFG_TBL_DRR_ID_OFFSET  0
	#define L1_C_SP_CFG_TBL_DRR_ID_LEN     6
	#define L1_C_SP_CFG_TBL_DRR_ID_DEFAULT 0x0
	/*[field] DRR_CREDIT_UNIT*/
	#define L1_C_SP_CFG_TBL_DRR_CREDIT_UNIT
	#define L1_C_SP_CFG_TBL_DRR_CREDIT_UNIT_OFFSET  6
	#define L1_C_SP_CFG_TBL_DRR_CREDIT_UNIT_LEN     1
	#define L1_C_SP_CFG_TBL_DRR_CREDIT_UNIT_DEFAULT 0x0

struct l1_c_sp_cfg_tbl {
	a_uint32_t  drr_id:6;
	a_uint32_t  drr_credit_unit:1;
	a_uint32_t  _reserved0:25;
};

union l1_c_sp_cfg_tbl_u {
	a_uint32_t val;
	struct l1_c_sp_cfg_tbl bf;
};

/*[table] L1_E_SP_CFG_TBL*/
#define L1_E_SP_CFG_TBL
#define L1_E_SP_CFG_TBL_ADDRESS 0x44000
#define L1_E_SP_CFG_TBL_NUM     64
#define L1_E_SP_CFG_TBL_INC     0x10
#define L1_E_SP_CFG_TBL_TYPE    REG_TYPE_RW
#define L1_E_SP_CFG_TBL_DEFAULT 0x0
	/*[field] DRR_ID*/
	#define L1_E_SP_CFG_TBL_DRR_ID
	#define L1_E_SP_CFG_TBL_DRR_ID_OFFSET  0
	#define L1_E_SP_CFG_TBL_DRR_ID_LEN     6
	#define L1_E_SP_CFG_TBL_DRR_ID_DEFAULT 0x0
	/*[field] DRR_CREDIT_UNIT*/
	#define L1_E_SP_CFG_TBL_DRR_CREDIT_UNIT
	#define L1_E_SP_CFG_TBL_DRR_CREDIT_UNIT_OFFSET  6
	#define L1_E_SP_CFG_TBL_DRR_CREDIT_UNIT_LEN     1
	#define L1_E_SP_CFG_TBL_DRR_CREDIT_UNIT_DEFAULT 0x0

struct l1_e_sp_cfg_tbl {
	a_uint32_t  drr_id:6;
	a_uint32_t  drr_credit_unit:1;
	a_uint32_t  _reserved0:25;
};

union l1_e_sp_cfg_tbl_u {
	a_uint32_t val;
	struct l1_e_sp_cfg_tbl bf;
};

/*[table] L1_FLOW_PORT_MAP_TBL*/
#define L1_FLOW_PORT_MAP_TBL
#define L1_FLOW_PORT_MAP_TBL_ADDRESS 0x46000
#define L1_FLOW_PORT_MAP_TBL_NUM     64
#define L1_FLOW_PORT_MAP_TBL_INC     0x10
#define L1_FLOW_PORT_MAP_TBL_TYPE    REG_TYPE_RW
#define L1_FLOW_PORT_MAP_TBL_DEFAULT 0x0
	/*[field] PORT_NUM*/
	#define L1_FLOW_PORT_MAP_TBL_PORT_NUM
	#define L1_FLOW_PORT_MAP_TBL_PORT_NUM_OFFSET  0
	#define L1_FLOW_PORT_MAP_TBL_PORT_NUM_LEN     4
	#define L1_FLOW_PORT_MAP_TBL_PORT_NUM_DEFAULT 0x0

struct l1_flow_port_map_tbl {
	a_uint32_t  port_num:4;
	a_uint32_t  _reserved0:28;
};

union l1_flow_port_map_tbl_u {
	a_uint32_t val;
	struct l1_flow_port_map_tbl bf;
};

/*[table] L1_C_DRR_HEAD_TBL*/
#define L1_C_DRR_HEAD_TBL
#define L1_C_DRR_HEAD_TBL_ADDRESS 0x48000
#define L1_C_DRR_HEAD_TBL_NUM     36
#define L1_C_DRR_HEAD_TBL_INC     0x10
#define L1_C_DRR_HEAD_TBL_TYPE    REG_TYPE_RO
#define L1_C_DRR_HEAD_TBL_DEFAULT 0x0
	/*[field] BACKUP_TAIL*/
	#define L1_C_DRR_HEAD_TBL_BACKUP_TAIL
	#define L1_C_DRR_HEAD_TBL_BACKUP_TAIL_OFFSET  0
	#define L1_C_DRR_HEAD_TBL_BACKUP_TAIL_LEN     6
	#define L1_C_DRR_HEAD_TBL_BACKUP_TAIL_DEFAULT 0x0
	/*[field] BACKUP_HEAD*/
	#define L1_C_DRR_HEAD_TBL_BACKUP_HEAD
	#define L1_C_DRR_HEAD_TBL_BACKUP_HEAD_OFFSET  6
	#define L1_C_DRR_HEAD_TBL_BACKUP_HEAD_LEN     6
	#define L1_C_DRR_HEAD_TBL_BACKUP_HEAD_DEFAULT 0x0
	/*[field] BACKUP_VLD*/
	#define L1_C_DRR_HEAD_TBL_BACKUP_VLD
	#define L1_C_DRR_HEAD_TBL_BACKUP_VLD_OFFSET  12
	#define L1_C_DRR_HEAD_TBL_BACKUP_VLD_LEN     1
	#define L1_C_DRR_HEAD_TBL_BACKUP_VLD_DEFAULT 0x0
	/*[field] BACKUP_MAX_N*/
	#define L1_C_DRR_HEAD_TBL_BACKUP_MAX_N
	#define L1_C_DRR_HEAD_TBL_BACKUP_MAX_N_OFFSET  13
	#define L1_C_DRR_HEAD_TBL_BACKUP_MAX_N_LEN     5
	#define L1_C_DRR_HEAD_TBL_BACKUP_MAX_N_DEFAULT 0x0
	/*[field] ACTIVE_TAIL*/
	#define L1_C_DRR_HEAD_TBL_ACTIVE_TAIL
	#define L1_C_DRR_HEAD_TBL_ACTIVE_TAIL_OFFSET  18
	#define L1_C_DRR_HEAD_TBL_ACTIVE_TAIL_LEN     6
	#define L1_C_DRR_HEAD_TBL_ACTIVE_TAIL_DEFAULT 0x0
	/*[field] ACTIVE_HEAD*/
	#define L1_C_DRR_HEAD_TBL_ACTIVE_HEAD
	#define L1_C_DRR_HEAD_TBL_ACTIVE_HEAD_OFFSET  24
	#define L1_C_DRR_HEAD_TBL_ACTIVE_HEAD_LEN     6
	#define L1_C_DRR_HEAD_TBL_ACTIVE_HEAD_DEFAULT 0x0
	/*[field] ACTIVE_VLD*/
	#define L1_C_DRR_HEAD_TBL_ACTIVE_VLD
	#define L1_C_DRR_HEAD_TBL_ACTIVE_VLD_OFFSET  30
	#define L1_C_DRR_HEAD_TBL_ACTIVE_VLD_LEN     1
	#define L1_C_DRR_HEAD_TBL_ACTIVE_VLD_DEFAULT 0x0
	/*[field] ACTIVE_MAX_N*/
	#define L1_C_DRR_HEAD_TBL_ACTIVE_MAX_N
	#define L1_C_DRR_HEAD_TBL_ACTIVE_MAX_N_OFFSET  31
	#define L1_C_DRR_HEAD_TBL_ACTIVE_MAX_N_LEN     5
	#define L1_C_DRR_HEAD_TBL_ACTIVE_MAX_N_DEFAULT 0x0

struct l1_c_drr_head_tbl {
	a_uint32_t  backup_tail:6;
	a_uint32_t  backup_head:6;
	a_uint32_t  backup_vld:1;
	a_uint32_t  backup_max_n:5;
	a_uint32_t  active_tail:6;
	a_uint32_t  active_head:6;
	a_uint32_t  active_vld:1;
	a_uint32_t  active_max_n_0:1;
	a_uint32_t  active_max_n_1:4;
	a_uint32_t  _reserved0:28;
};

union l1_c_drr_head_tbl_u {
	a_uint32_t val[2];
	struct l1_c_drr_head_tbl bf;
};

/*[table] L1_E_DRR_HEAD_TBL*/
#define L1_E_DRR_HEAD_TBL
#define L1_E_DRR_HEAD_TBL_ADDRESS 0x4a000
#define L1_E_DRR_HEAD_TBL_NUM     36
#define L1_E_DRR_HEAD_TBL_INC     0x10
#define L1_E_DRR_HEAD_TBL_TYPE    REG_TYPE_RO
#define L1_E_DRR_HEAD_TBL_DEFAULT 0x0
	/*[field] BACKUP_TAIL*/
	#define L1_E_DRR_HEAD_TBL_BACKUP_TAIL
	#define L1_E_DRR_HEAD_TBL_BACKUP_TAIL_OFFSET  0
	#define L1_E_DRR_HEAD_TBL_BACKUP_TAIL_LEN     6
	#define L1_E_DRR_HEAD_TBL_BACKUP_TAIL_DEFAULT 0x0
	/*[field] BACKUP_HEAD*/
	#define L1_E_DRR_HEAD_TBL_BACKUP_HEAD
	#define L1_E_DRR_HEAD_TBL_BACKUP_HEAD_OFFSET  6
	#define L1_E_DRR_HEAD_TBL_BACKUP_HEAD_LEN     6
	#define L1_E_DRR_HEAD_TBL_BACKUP_HEAD_DEFAULT 0x0
	/*[field] BACKUP_VLD*/
	#define L1_E_DRR_HEAD_TBL_BACKUP_VLD
	#define L1_E_DRR_HEAD_TBL_BACKUP_VLD_OFFSET  12
	#define L1_E_DRR_HEAD_TBL_BACKUP_VLD_LEN     1
	#define L1_E_DRR_HEAD_TBL_BACKUP_VLD_DEFAULT 0x0
	/*[field] BACKUP_MAX_N*/
	#define L1_E_DRR_HEAD_TBL_BACKUP_MAX_N
	#define L1_E_DRR_HEAD_TBL_BACKUP_MAX_N_OFFSET  13
	#define L1_E_DRR_HEAD_TBL_BACKUP_MAX_N_LEN     5
	#define L1_E_DRR_HEAD_TBL_BACKUP_MAX_N_DEFAULT 0x0
	/*[field] ACTIVE_TAIL*/
	#define L1_E_DRR_HEAD_TBL_ACTIVE_TAIL
	#define L1_E_DRR_HEAD_TBL_ACTIVE_TAIL_OFFSET  18
	#define L1_E_DRR_HEAD_TBL_ACTIVE_TAIL_LEN     6
	#define L1_E_DRR_HEAD_TBL_ACTIVE_TAIL_DEFAULT 0x0
	/*[field] ACTIVE_HEAD*/
	#define L1_E_DRR_HEAD_TBL_ACTIVE_HEAD
	#define L1_E_DRR_HEAD_TBL_ACTIVE_HEAD_OFFSET  24
	#define L1_E_DRR_HEAD_TBL_ACTIVE_HEAD_LEN     6
	#define L1_E_DRR_HEAD_TBL_ACTIVE_HEAD_DEFAULT 0x0
	/*[field] ACTIVE_VLD*/
	#define L1_E_DRR_HEAD_TBL_ACTIVE_VLD
	#define L1_E_DRR_HEAD_TBL_ACTIVE_VLD_OFFSET  30
	#define L1_E_DRR_HEAD_TBL_ACTIVE_VLD_LEN     1
	#define L1_E_DRR_HEAD_TBL_ACTIVE_VLD_DEFAULT 0x0
	/*[field] ACTIVE_MAX_N*/
	#define L1_E_DRR_HEAD_TBL_ACTIVE_MAX_N
	#define L1_E_DRR_HEAD_TBL_ACTIVE_MAX_N_OFFSET  31
	#define L1_E_DRR_HEAD_TBL_ACTIVE_MAX_N_LEN     5
	#define L1_E_DRR_HEAD_TBL_ACTIVE_MAX_N_DEFAULT 0x0

struct l1_e_drr_head_tbl {
	a_uint32_t  backup_tail:6;
	a_uint32_t  backup_head:6;
	a_uint32_t  backup_vld:1;
	a_uint32_t  backup_max_n:5;
	a_uint32_t  active_tail:6;
	a_uint32_t  active_head:6;
	a_uint32_t  active_vld:1;
	a_uint32_t  active_max_n_0:1;
	a_uint32_t  active_max_n_1:4;
	a_uint32_t  _reserved0:28;
};

union l1_e_drr_head_tbl_u {
	a_uint32_t val[2];
	struct l1_e_drr_head_tbl bf;
};

/*[table] L1_DRR_CREDIT_TBL*/
#define L1_DRR_CREDIT_TBL
#define L1_DRR_CREDIT_TBL_ADDRESS 0x4c000
#define L1_DRR_CREDIT_TBL_NUM     64
#define L1_DRR_CREDIT_TBL_INC     0x10
#define L1_DRR_CREDIT_TBL_TYPE    REG_TYPE_RO
#define L1_DRR_CREDIT_TBL_DEFAULT 0x0
	/*[field] C_DRR_CREDIT*/
	#define L1_DRR_CREDIT_TBL_C_DRR_CREDIT
	#define L1_DRR_CREDIT_TBL_C_DRR_CREDIT_OFFSET  0
	#define L1_DRR_CREDIT_TBL_C_DRR_CREDIT_LEN     24
	#define L1_DRR_CREDIT_TBL_C_DRR_CREDIT_DEFAULT 0x0
	/*[field] C_DRR_CREDIT_NEG*/
	#define L1_DRR_CREDIT_TBL_C_DRR_CREDIT_NEG
	#define L1_DRR_CREDIT_TBL_C_DRR_CREDIT_NEG_OFFSET  24
	#define L1_DRR_CREDIT_TBL_C_DRR_CREDIT_NEG_LEN     1
	#define L1_DRR_CREDIT_TBL_C_DRR_CREDIT_NEG_DEFAULT 0x0
	/*[field] E_DRR_CREDIT*/
	#define L1_DRR_CREDIT_TBL_E_DRR_CREDIT
	#define L1_DRR_CREDIT_TBL_E_DRR_CREDIT_OFFSET  25
	#define L1_DRR_CREDIT_TBL_E_DRR_CREDIT_LEN     24
	#define L1_DRR_CREDIT_TBL_E_DRR_CREDIT_DEFAULT 0x0
	/*[field] E_DRR_CREDIT_NEG*/
	#define L1_DRR_CREDIT_TBL_E_DRR_CREDIT_NEG
	#define L1_DRR_CREDIT_TBL_E_DRR_CREDIT_NEG_OFFSET  49
	#define L1_DRR_CREDIT_TBL_E_DRR_CREDIT_NEG_LEN     1
	#define L1_DRR_CREDIT_TBL_E_DRR_CREDIT_NEG_DEFAULT 0x0

struct l1_drr_credit_tbl {
	a_uint32_t  c_drr_credit:24;
	a_uint32_t  c_drr_credit_neg:1;
	a_uint32_t  e_drr_credit_0:7;
	a_uint32_t  e_drr_credit_1:17;
	a_uint32_t  e_drr_credit_neg:1;
	a_uint32_t  _reserved0:14;
};

union l1_drr_credit_tbl_u {
	a_uint32_t val[2];
	struct l1_drr_credit_tbl bf;
};

/*[table] L1_C_DRR_LL_TBL*/
#define L1_C_DRR_LL_TBL
#define L1_C_DRR_LL_TBL_ADDRESS 0x4e000
#define L1_C_DRR_LL_TBL_NUM     64
#define L1_C_DRR_LL_TBL_INC     0x10
#define L1_C_DRR_LL_TBL_TYPE    REG_TYPE_RO
#define L1_C_DRR_LL_TBL_DEFAULT 0x0
	/*[field] NEXT_PTR*/
	#define L1_C_DRR_LL_TBL_NEXT_PTR
	#define L1_C_DRR_LL_TBL_NEXT_PTR_OFFSET  0
	#define L1_C_DRR_LL_TBL_NEXT_PTR_LEN     6
	#define L1_C_DRR_LL_TBL_NEXT_PTR_DEFAULT 0x0

struct l1_c_drr_ll_tbl {
	a_uint32_t  next_ptr:6;
	a_uint32_t  _reserved0:26;
};

union l1_c_drr_ll_tbl_u {
	a_uint32_t val;
	struct l1_c_drr_ll_tbl bf;
};

/*[table] L1_C_DRR_REVERSE_LL_TBL*/
#define L1_C_DRR_REVERSE_LL_TBL
#define L1_C_DRR_REVERSE_LL_TBL_ADDRESS 0x50000
#define L1_C_DRR_REVERSE_LL_TBL_NUM     64
#define L1_C_DRR_REVERSE_LL_TBL_INC     0x10
#define L1_C_DRR_REVERSE_LL_TBL_TYPE    REG_TYPE_RO
#define L1_C_DRR_REVERSE_LL_TBL_DEFAULT 0x0
	/*[field] PRE_PTR*/
	#define L1_C_DRR_REVERSE_LL_TBL_PRE_PTR
	#define L1_C_DRR_REVERSE_LL_TBL_PRE_PTR_OFFSET  0
	#define L1_C_DRR_REVERSE_LL_TBL_PRE_PTR_LEN     6
	#define L1_C_DRR_REVERSE_LL_TBL_PRE_PTR_DEFAULT 0x0

struct l1_c_drr_reverse_ll_tbl {
	a_uint32_t  pre_ptr:6;
	a_uint32_t  _reserved0:26;
};

union l1_c_drr_reverse_ll_tbl_u {
	a_uint32_t val;
	struct l1_c_drr_reverse_ll_tbl bf;
};

/*[table] L1_E_DRR_LL_TBL*/
#define L1_E_DRR_LL_TBL
#define L1_E_DRR_LL_TBL_ADDRESS 0x52000
#define L1_E_DRR_LL_TBL_NUM     64
#define L1_E_DRR_LL_TBL_INC     0x10
#define L1_E_DRR_LL_TBL_TYPE    REG_TYPE_RO
#define L1_E_DRR_LL_TBL_DEFAULT 0x0
	/*[field] NEXT_PTR*/
	#define L1_E_DRR_LL_TBL_NEXT_PTR
	#define L1_E_DRR_LL_TBL_NEXT_PTR_OFFSET  0
	#define L1_E_DRR_LL_TBL_NEXT_PTR_LEN     6
	#define L1_E_DRR_LL_TBL_NEXT_PTR_DEFAULT 0x0

struct l1_e_drr_ll_tbl {
	a_uint32_t  next_ptr:6;
	a_uint32_t  _reserved0:26;
};

union l1_e_drr_ll_tbl_u {
	a_uint32_t val;
	struct l1_e_drr_ll_tbl bf;
};

/*[table] L1_E_DRR_REVERSE_LL_TBL*/
#define L1_E_DRR_REVERSE_LL_TBL
#define L1_E_DRR_REVERSE_LL_TBL_ADDRESS 0x54000
#define L1_E_DRR_REVERSE_LL_TBL_NUM     64
#define L1_E_DRR_REVERSE_LL_TBL_INC     0x10
#define L1_E_DRR_REVERSE_LL_TBL_TYPE    REG_TYPE_RO
#define L1_E_DRR_REVERSE_LL_TBL_DEFAULT 0x0
	/*[field] PRE_PTR*/
	#define L1_E_DRR_REVERSE_LL_TBL_PRE_PTR
	#define L1_E_DRR_REVERSE_LL_TBL_PRE_PTR_OFFSET  0
	#define L1_E_DRR_REVERSE_LL_TBL_PRE_PTR_LEN     6
	#define L1_E_DRR_REVERSE_LL_TBL_PRE_PTR_DEFAULT 0x0

struct l1_e_drr_reverse_ll_tbl {
	a_uint32_t  pre_ptr:6;
	a_uint32_t  _reserved0:26;
};

union l1_e_drr_reverse_ll_tbl_u {
	a_uint32_t val;
	struct l1_e_drr_reverse_ll_tbl bf;
};

/*[table] L1_A_FLOW_ENTRY_TBL*/
#define L1_A_FLOW_ENTRY_TBL
#define L1_A_FLOW_ENTRY_TBL_ADDRESS 0x56000
#define L1_A_FLOW_ENTRY_TBL_NUM     64
#define L1_A_FLOW_ENTRY_TBL_INC     0x10
#define L1_A_FLOW_ENTRY_TBL_TYPE    REG_TYPE_RO
#define L1_A_FLOW_ENTRY_TBL_DEFAULT 0x0
	/*[field] ENTRY_PATH_ID*/
	#define L1_A_FLOW_ENTRY_TBL_ENTRY_PATH_ID
	#define L1_A_FLOW_ENTRY_TBL_ENTRY_PATH_ID_OFFSET  0
	#define L1_A_FLOW_ENTRY_TBL_ENTRY_PATH_ID_LEN     10
	#define L1_A_FLOW_ENTRY_TBL_ENTRY_PATH_ID_DEFAULT 0x0

struct l1_a_flow_entry_tbl {
	a_uint32_t  entry_path_id:10;
	a_uint32_t  _reserved0:22;
};

union l1_a_flow_entry_tbl_u {
	a_uint32_t val;
	struct l1_a_flow_entry_tbl bf;
};

/*[table] L1_B_FLOW_ENTRY_TBL*/
#define L1_B_FLOW_ENTRY_TBL
#define L1_B_FLOW_ENTRY_TBL_ADDRESS 0x58000
#define L1_B_FLOW_ENTRY_TBL_NUM     64
#define L1_B_FLOW_ENTRY_TBL_INC     0x10
#define L1_B_FLOW_ENTRY_TBL_TYPE    REG_TYPE_RO
#define L1_B_FLOW_ENTRY_TBL_DEFAULT 0x0
	/*[field] ENTRY_PATH_ID*/
	#define L1_B_FLOW_ENTRY_TBL_ENTRY_PATH_ID
	#define L1_B_FLOW_ENTRY_TBL_ENTRY_PATH_ID_OFFSET  0
	#define L1_B_FLOW_ENTRY_TBL_ENTRY_PATH_ID_LEN     10
	#define L1_B_FLOW_ENTRY_TBL_ENTRY_PATH_ID_DEFAULT 0x0

struct l1_b_flow_entry_tbl {
	a_uint32_t  entry_path_id:10;
	a_uint32_t  _reserved0:22;
};

union l1_b_flow_entry_tbl_u {
	a_uint32_t val;
	struct l1_b_flow_entry_tbl bf;
};

/*[table] L1_SP_ENTRY_TBL*/
#define L1_SP_ENTRY_TBL
#define L1_SP_ENTRY_TBL_ADDRESS 0x5a000
#define L1_SP_ENTRY_TBL_NUM     8
#define L1_SP_ENTRY_TBL_INC     0x40
#define L1_SP_ENTRY_TBL_TYPE    REG_TYPE_RO
#define L1_SP_ENTRY_TBL_DEFAULT 0x0
	/*[field] ENTRY_PATH_ID*/
	#define L1_SP_ENTRY_TBL_ENTRY_PATH_ID
	#define L1_SP_ENTRY_TBL_ENTRY_PATH_ID_OFFSET  0
	#define L1_SP_ENTRY_TBL_ENTRY_PATH_ID_LEN     256
	#define L1_SP_ENTRY_TBL_ENTRY_PATH_ID_DEFAULT 0x0
	/*[field] ENTRY_VLD*/
	#define L1_SP_ENTRY_TBL_ENTRY_VLD
	#define L1_SP_ENTRY_TBL_ENTRY_VLD_OFFSET  256
	#define L1_SP_ENTRY_TBL_ENTRY_VLD_LEN     16
	#define L1_SP_ENTRY_TBL_ENTRY_VLD_DEFAULT 0x0

struct l1_sp_entry_tbl {
	a_uint32_t  entry_path_id_0:32;
	a_uint32_t  entry_path_id_1:32;
	a_uint32_t  entry_path_id_2:32;
	a_uint32_t  entry_path_id_3:32;
	a_uint32_t  entry_path_id_4:32;
	a_uint32_t  entry_path_id_5:32;
	a_uint32_t  entry_path_id_6:32;
	a_uint32_t  entry_path_id_7:32;
	a_uint32_t  entry_vld:16;
	a_uint32_t  _reserved0:16;
};

union l1_sp_entry_tbl_u {
	a_uint32_t val[9];
	struct l1_sp_entry_tbl bf;
};


/*[table] L1_ENS_Q_LL_TBL*/
#define L1_ENS_Q_LL_TBL
#define L1_ENS_Q_LL_TBL_ADDRESS 0x60000
#define L1_ENS_Q_LL_TBL_NUM     64
#define L1_ENS_Q_LL_TBL_INC     0x10
#define L1_ENS_Q_LL_TBL_TYPE    REG_TYPE_RO
#define L1_ENS_Q_LL_TBL_DEFAULT 0x0
	/*[field] NEXT_PTR*/
	#define L1_ENS_Q_LL_TBL_NEXT_PTR
	#define L1_ENS_Q_LL_TBL_NEXT_PTR_OFFSET  0
	#define L1_ENS_Q_LL_TBL_NEXT_PTR_LEN     6
	#define L1_ENS_Q_LL_TBL_NEXT_PTR_DEFAULT 0x0

struct l1_ens_q_ll_tbl {
	a_uint32_t  next_ptr:6;
	a_uint32_t  _reserved0:26;
};

union l1_ens_q_ll_tbl_u {
	a_uint32_t val;
	struct l1_ens_q_ll_tbl bf;
};

/*[table] L1_ENS_Q_HEAD_TBL*/
#define L1_ENS_Q_HEAD_TBL
#define L1_ENS_Q_HEAD_TBL_ADDRESS 0x62000
#define L1_ENS_Q_HEAD_TBL_NUM     8
#define L1_ENS_Q_HEAD_TBL_INC     0x10
#define L1_ENS_Q_HEAD_TBL_TYPE    REG_TYPE_RO
#define L1_ENS_Q_HEAD_TBL_DEFAULT 0x0
	/*[field] TAIL*/
	#define L1_ENS_Q_HEAD_TBL_TAIL
	#define L1_ENS_Q_HEAD_TBL_TAIL_OFFSET  0
	#define L1_ENS_Q_HEAD_TBL_TAIL_LEN     6
	#define L1_ENS_Q_HEAD_TBL_TAIL_DEFAULT 0x0
	/*[field] HEAD*/
	#define L1_ENS_Q_HEAD_TBL_HEAD
	#define L1_ENS_Q_HEAD_TBL_HEAD_OFFSET  6
	#define L1_ENS_Q_HEAD_TBL_HEAD_LEN     6
	#define L1_ENS_Q_HEAD_TBL_HEAD_DEFAULT 0x0
	/*[field] VLD*/
	#define L1_ENS_Q_HEAD_TBL_VLD
	#define L1_ENS_Q_HEAD_TBL_VLD_OFFSET  12
	#define L1_ENS_Q_HEAD_TBL_VLD_LEN     1
	#define L1_ENS_Q_HEAD_TBL_VLD_DEFAULT 0x0

struct l1_ens_q_head_tbl {
	a_uint32_t  tail:6;
	a_uint32_t  head:6;
	a_uint32_t  vld:1;
	a_uint32_t  _reserved0:19;
};

union l1_ens_q_head_tbl_u {
	a_uint32_t val;
	struct l1_ens_q_head_tbl bf;
};

/*[table] L1_ENS_Q_ENTRY_TBL*/
#define L1_ENS_Q_ENTRY_TBL
#define L1_ENS_Q_ENTRY_TBL_ADDRESS 0x64000
#define L1_ENS_Q_ENTRY_TBL_NUM     64
#define L1_ENS_Q_ENTRY_TBL_INC     0x10
#define L1_ENS_Q_ENTRY_TBL_TYPE    REG_TYPE_RO
#define L1_ENS_Q_ENTRY_TBL_DEFAULT 0x0
	/*[field] ENTRY_ENS_TYPE*/
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_TYPE
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_TYPE_OFFSET  0
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_TYPE_LEN     2
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_TYPE_DEFAULT 0x0
	/*[field] ENTRY_ENS_VLD*/
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_VLD
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_VLD_OFFSET  2
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_VLD_LEN     1
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_VLD_DEFAULT 0x0
	/*[field] ENTRY_ENS_IN_Q*/
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_IN_Q
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_IN_Q_OFFSET  3
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_IN_Q_LEN     1
	#define L1_ENS_Q_ENTRY_TBL_ENTRY_ENS_IN_Q_DEFAULT 0x0

struct l1_ens_q_entry_tbl {
	a_uint32_t  entry_ens_type:2;
	a_uint32_t  entry_ens_vld:1;
	a_uint32_t  entry_ens_in_q:1;
	a_uint32_t  _reserved0:28;
};

union l1_ens_q_entry_tbl_u {
	a_uint32_t val;
	struct l1_ens_q_entry_tbl bf;
};

/*[table] L1_FLOW_STATUS_TBL*/
#define L1_FLOW_STATUS_TBL
#define L1_FLOW_STATUS_TBL_ADDRESS 0x66000
#define L1_FLOW_STATUS_TBL_NUM     64
#define L1_FLOW_STATUS_TBL_INC     0x10
#define L1_FLOW_STATUS_TBL_TYPE    REG_TYPE_RO
#define L1_FLOW_STATUS_TBL_DEFAULT 0x0
	/*[field] EN_LEVEL*/
	#define L1_FLOW_STATUS_TBL_EN_LEVEL
	#define L1_FLOW_STATUS_TBL_EN_LEVEL_OFFSET  0
	#define L1_FLOW_STATUS_TBL_EN_LEVEL_LEN     1
	#define L1_FLOW_STATUS_TBL_EN_LEVEL_DEFAULT 0x0
	/*[field] EN_CDRR*/
	#define L1_FLOW_STATUS_TBL_EN_CDRR
	#define L1_FLOW_STATUS_TBL_EN_CDRR_OFFSET  1
	#define L1_FLOW_STATUS_TBL_EN_CDRR_LEN     1
	#define L1_FLOW_STATUS_TBL_EN_CDRR_DEFAULT 0x0
	/*[field] EN_EDRR*/
	#define L1_FLOW_STATUS_TBL_EN_EDRR
	#define L1_FLOW_STATUS_TBL_EN_EDRR_OFFSET  2
	#define L1_FLOW_STATUS_TBL_EN_EDRR_LEN     1
	#define L1_FLOW_STATUS_TBL_EN_EDRR_DEFAULT 0x0

struct l1_flow_status_tbl {
	a_uint32_t  en_level:1;
	a_uint32_t  en_cdrr:1;
	a_uint32_t  en_edrr:1;
	a_uint32_t  _reserved0:29;
};

union l1_flow_status_tbl_u {
	a_uint32_t val;
	struct l1_flow_status_tbl bf;
};

/*[table] PSCH_TDM_CFG_TBL*/
#define PSCH_TDM_CFG_TBL
#define PSCH_TDM_CFG_TBL_ADDRESS 0x7a000
#define PSCH_TDM_CFG_TBL_NUM     128
#define PSCH_TDM_CFG_TBL_INC     0x10
#define PSCH_TDM_CFG_TBL_TYPE    REG_TYPE_RW
#define PSCH_TDM_CFG_TBL_DEFAULT 0x0
	/*[field] DES_PORT*/
	#define PSCH_TDM_CFG_TBL_DES_PORT
	#define PSCH_TDM_CFG_TBL_DES_PORT_OFFSET  0
	#define PSCH_TDM_CFG_TBL_DES_PORT_LEN     4
	#define PSCH_TDM_CFG_TBL_DES_PORT_DEFAULT 0x0
	/*[field] ENS_PORT*/
	#define PSCH_TDM_CFG_TBL_ENS_PORT
	#define PSCH_TDM_CFG_TBL_ENS_PORT_OFFSET  4
	#define PSCH_TDM_CFG_TBL_ENS_PORT_LEN     4
	#define PSCH_TDM_CFG_TBL_ENS_PORT_DEFAULT 0x0
	/*[field] ENS_PORT_BITMAP*/
	#define PSCH_TDM_CFG_TBL_ENS_PORT_BITMAP
	#define PSCH_TDM_CFG_TBL_ENS_PORT_BITMAP_OFFSET  8
	#define PSCH_TDM_CFG_TBL_ENS_PORT_BITMAP_LEN     8
	#define PSCH_TDM_CFG_TBL_ENS_PORT_BITMAP_DEFAULT 0x0

struct psch_tdm_cfg_tbl {
	a_uint32_t  des_port:4;
	a_uint32_t  ens_port:4;
	a_uint32_t  ens_port_bitmap:8;
	a_uint32_t  _reserved0:16;
};

union psch_tdm_cfg_tbl_u {
	a_uint32_t val;
	struct psch_tdm_cfg_tbl bf;
};

/*[register] PORT_QOS_CTRL*/
#define PORT_QOS_CTRL
#define PORT_QOS_CTRL_ADDRESS 0x900
#define PORT_QOS_CTRL_NUM     8
#define PORT_QOS_CTRL_INC     0x10
#define PORT_QOS_CTRL_TYPE    REG_TYPE_RW
#define PORT_QOS_CTRL_DEFAULT 0x23440
	/*[field] PCP_QOS_GROUP_ID*/
	#define PORT_QOS_CTRL_PCP_QOS_GROUP_ID
	#define PORT_QOS_CTRL_PCP_QOS_GROUP_ID_OFFSET  0
	#define PORT_QOS_CTRL_PCP_QOS_GROUP_ID_LEN     1
	#define PORT_QOS_CTRL_PCP_QOS_GROUP_ID_DEFAULT 0x0
	/*[field] DSCP_QOS_GROUP_ID*/
	#define PORT_QOS_CTRL_DSCP_QOS_GROUP_ID
	#define PORT_QOS_CTRL_DSCP_QOS_GROUP_ID_OFFSET  1
	#define PORT_QOS_CTRL_DSCP_QOS_GROUP_ID_LEN     1
	#define PORT_QOS_CTRL_DSCP_QOS_GROUP_ID_DEFAULT 0x0
	/*[field] FLOW_QOS_GROUP_ID*/
	#define PORT_QOS_CTRL_FLOW_QOS_GROUP_ID
	#define PORT_QOS_CTRL_FLOW_QOS_GROUP_ID_OFFSET  2
	#define PORT_QOS_CTRL_FLOW_QOS_GROUP_ID_LEN     1
	#define PORT_QOS_CTRL_FLOW_QOS_GROUP_ID_DEFAULT 0x0
	/*[field] PORT_DSCP_QOS_PRI*/
	#define PORT_QOS_CTRL_PORT_DSCP_QOS_PRI
	#define PORT_QOS_CTRL_PORT_DSCP_QOS_PRI_OFFSET  3
	#define PORT_QOS_CTRL_PORT_DSCP_QOS_PRI_LEN     3
	#define PORT_QOS_CTRL_PORT_DSCP_QOS_PRI_DEFAULT 0x0
	/*[field] PORT_PCP_QOS_PRI*/
	#define PORT_QOS_CTRL_PORT_PCP_QOS_PRI
	#define PORT_QOS_CTRL_PORT_PCP_QOS_PRI_OFFSET  6
	#define PORT_QOS_CTRL_PORT_PCP_QOS_PRI_LEN     3
	#define PORT_QOS_CTRL_PORT_PCP_QOS_PRI_DEFAULT 0x1
	/*[field] PORT_PREHEADER_QOS_PRI*/
	#define PORT_QOS_CTRL_PORT_PREHEADER_QOS_PRI
	#define PORT_QOS_CTRL_PORT_PREHEADER_QOS_PRI_OFFSET  9
	#define PORT_QOS_CTRL_PORT_PREHEADER_QOS_PRI_LEN     3
	#define PORT_QOS_CTRL_PORT_PREHEADER_QOS_PRI_DEFAULT 0x2
	/*[field] PORT_FLOW_QOS_PRI*/
	#define PORT_QOS_CTRL_PORT_FLOW_QOS_PRI
	#define PORT_QOS_CTRL_PORT_FLOW_QOS_PRI_OFFSET  12
	#define PORT_QOS_CTRL_PORT_FLOW_QOS_PRI_LEN     3
	#define PORT_QOS_CTRL_PORT_FLOW_QOS_PRI_DEFAULT 0x3
	/*[field] PORT_ACL_QOS_PRI*/
	#define PORT_QOS_CTRL_PORT_ACL_QOS_PRI
	#define PORT_QOS_CTRL_PORT_ACL_QOS_PRI_OFFSET  15
	#define PORT_QOS_CTRL_PORT_ACL_QOS_PRI_LEN     3
	#define PORT_QOS_CTRL_PORT_ACL_QOS_PRI_DEFAULT 0x4
	/*[field] PORT_PCP_CHANGE_EN*/
	#define PORT_QOS_CTRL_PORT_PCP_CHANGE_EN
	#define PORT_QOS_CTRL_PORT_PCP_CHANGE_EN_OFFSET  18
	#define PORT_QOS_CTRL_PORT_PCP_CHANGE_EN_LEN     1
	#define PORT_QOS_CTRL_PORT_PCP_CHANGE_EN_DEFAULT 0x0
	/*[field] PORT_DEI_CHANGE_EN*/
	#define PORT_QOS_CTRL_PORT_DEI_CHANGE_EN
	#define PORT_QOS_CTRL_PORT_DEI_CHANGE_EN_OFFSET  19
	#define PORT_QOS_CTRL_PORT_DEI_CHANGE_EN_LEN     1
	#define PORT_QOS_CTRL_PORT_DEI_CHANGE_EN_DEFAULT 0x0
	/*[field] PORT_DSCP_CHANGE_EN*/
	#define PORT_QOS_CTRL_PORT_DSCP_CHANGE_EN
	#define PORT_QOS_CTRL_PORT_DSCP_CHANGE_EN_OFFSET  20
	#define PORT_QOS_CTRL_PORT_DSCP_CHANGE_EN_LEN     1
	#define PORT_QOS_CTRL_PORT_DSCP_CHANGE_EN_DEFAULT 0x0

struct port_qos_ctrl {
	a_uint32_t  pcp_qos_group_id:1;
	a_uint32_t  dscp_qos_group_id:1;
	a_uint32_t  flow_qos_group_id:1;
	a_uint32_t  port_dscp_qos_pri:3;
	a_uint32_t  port_pcp_qos_pri:3;
	a_uint32_t  port_preheader_qos_pri:3;
	a_uint32_t  port_flow_qos_pri:3;
	a_uint32_t  port_acl_qos_pri:3;
	a_uint32_t  port_pcp_change_en:1;
	a_uint32_t  port_dei_change_en:1;
	a_uint32_t  port_dscp_change_en:1;
	a_uint32_t  _reserved0:11;
};

union port_qos_ctrl_u {
	a_uint32_t val;
	struct port_qos_ctrl bf;
};

/*[register] PCP_QOS_GROUP_0*/
#define PCP_QOS_GROUP_0
#define PCP_QOS_GROUP_0_ADDRESS 0xb00
#define PCP_QOS_GROUP_0_NUM     16
#define PCP_QOS_GROUP_0_INC     0x4
#define PCP_QOS_GROUP_0_TYPE    REG_TYPE_RW
#define PCP_QOS_GROUP_0_DEFAULT 0x0
	/*[field] QOS_INFO*/
	#define PCP_QOS_GROUP_0_QOS_INFO
	#define PCP_QOS_GROUP_0_QOS_INFO_OFFSET  0
	#define PCP_QOS_GROUP_0_QOS_INFO_LEN     16
	#define PCP_QOS_GROUP_0_QOS_INFO_DEFAULT 0x0

struct pcp_qos_group_0 {
	a_uint32_t  qos_info:16;
	a_uint32_t  _reserved0:16;
};

union pcp_qos_group_0_u {
	a_uint32_t val;
	struct pcp_qos_group_0 bf;
};

/*[register] PCP_QOS_GROUP_1*/
#define PCP_QOS_GROUP_1
#define PCP_QOS_GROUP_1_ADDRESS 0xc00
#define PCP_QOS_GROUP_1_NUM     16
#define PCP_QOS_GROUP_1_INC     0x4
#define PCP_QOS_GROUP_1_TYPE    REG_TYPE_RW
#define PCP_QOS_GROUP_1_DEFAULT 0x0
	/*[field] QOS_INFO*/
	#define PCP_QOS_GROUP_1_QOS_INFO
	#define PCP_QOS_GROUP_1_QOS_INFO_OFFSET  0
	#define PCP_QOS_GROUP_1_QOS_INFO_LEN     16
	#define PCP_QOS_GROUP_1_QOS_INFO_DEFAULT 0x0

struct pcp_qos_group_1 {
	a_uint32_t  qos_info:16;
	a_uint32_t  _reserved0:16;
};

union pcp_qos_group_1_u {
	a_uint32_t val;
	struct pcp_qos_group_1 bf;
};

/*[register] FLOW_QOS_GROUP_0*/
#define FLOW_QOS_GROUP_0
#define FLOW_QOS_GROUP_0_ADDRESS 0xd00
#define FLOW_QOS_GROUP_0_NUM     32
#define FLOW_QOS_GROUP_0_INC     0x4
#define FLOW_QOS_GROUP_0_TYPE    REG_TYPE_RW
#define FLOW_QOS_GROUP_0_DEFAULT 0x0
	/*[field] QOS_INFO*/
	#define FLOW_QOS_GROUP_0_QOS_INFO
	#define FLOW_QOS_GROUP_0_QOS_INFO_OFFSET  0
	#define FLOW_QOS_GROUP_0_QOS_INFO_LEN     16
	#define FLOW_QOS_GROUP_0_QOS_INFO_DEFAULT 0x0

struct flow_qos_group_0 {
	a_uint32_t  qos_info:16;
	a_uint32_t  _reserved0:16;
};

union flow_qos_group_0_u {
	a_uint32_t val;
	struct flow_qos_group_0 bf;
};

/*[register] FLOW_QOS_GROUP_1*/
#define FLOW_QOS_GROUP_1
#define FLOW_QOS_GROUP_1_ADDRESS 0xe00
#define FLOW_QOS_GROUP_1_NUM     32
#define FLOW_QOS_GROUP_1_INC     0x4
#define FLOW_QOS_GROUP_1_TYPE    REG_TYPE_RW
#define FLOW_QOS_GROUP_1_DEFAULT 0x0
	/*[field] QOS_INFO*/
	#define FLOW_QOS_GROUP_1_QOS_INFO
	#define FLOW_QOS_GROUP_1_QOS_INFO_OFFSET  0
	#define FLOW_QOS_GROUP_1_QOS_INFO_LEN     16
	#define FLOW_QOS_GROUP_1_QOS_INFO_DEFAULT 0x0

struct flow_qos_group_1 {
	a_uint32_t  qos_info:16;
	a_uint32_t  _reserved0:16;
};

union flow_qos_group_1_u {
	a_uint32_t val;
	struct flow_qos_group_1 bf;
};

/*[register] DSCP_QOS_GROUP_0*/
#define DSCP_QOS_GROUP_0
#define DSCP_QOS_GROUP_0_ADDRESS 0x2000
#define DSCP_QOS_GROUP_0_NUM     64
#define DSCP_QOS_GROUP_0_INC     0x10
#define DSCP_QOS_GROUP_0_TYPE    REG_TYPE_RW
#define DSCP_QOS_GROUP_0_DEFAULT 0x0
	/*[field] QOS_INFO*/
	#define DSCP_QOS_GROUP_0_QOS_INFO
	#define DSCP_QOS_GROUP_0_QOS_INFO_OFFSET  0
	#define DSCP_QOS_GROUP_0_QOS_INFO_LEN     16
	#define DSCP_QOS_GROUP_0_QOS_INFO_DEFAULT 0x0

struct dscp_qos_group_0 {
	a_uint32_t  qos_info:16;
	a_uint32_t  _reserved0:16;
};

union dscp_qos_group_0_u {
	a_uint32_t val;
	struct dscp_qos_group_0 bf;
};

/*[register] DSCP_QOS_GROUP_1*/
#define DSCP_QOS_GROUP_1
#define DSCP_QOS_GROUP_1_ADDRESS 0x2800
#define DSCP_QOS_GROUP_1_NUM     64
#define DSCP_QOS_GROUP_1_INC     0x10
#define DSCP_QOS_GROUP_1_TYPE    REG_TYPE_RW
#define DSCP_QOS_GROUP_1_DEFAULT 0x0
	/*[field] QOS_INFO*/
	#define DSCP_QOS_GROUP_1_QOS_INFO
	#define DSCP_QOS_GROUP_1_QOS_INFO_OFFSET  0
	#define DSCP_QOS_GROUP_1_QOS_INFO_LEN     16
	#define DSCP_QOS_GROUP_1_QOS_INFO_DEFAULT 0x0

struct dscp_qos_group_1 {
	a_uint32_t  qos_info:16;
	a_uint32_t  _reserved0:16;
};

union dscp_qos_group_1_u {
	a_uint32_t val;
	struct dscp_qos_group_1 bf;
};

#endif
