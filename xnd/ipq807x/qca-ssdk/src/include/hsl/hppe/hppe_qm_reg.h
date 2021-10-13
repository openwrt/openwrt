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
#ifndef HPPE_QM_REG_H
#define HPPE_QM_REG_H

/*[register] FLUSH_CFG*/
#define FLUSH_CFG
#define FLUSH_CFG_ADDRESS 0x0
#define FLUSH_CFG_NUM     1
#define FLUSH_CFG_INC     0x4
#define FLUSH_CFG_TYPE    REG_TYPE_RW
#define FLUSH_CFG_DEFAULT 0x80000
	/*[field] FLUSH_QID*/
	#define FLUSH_CFG_FLUSH_QID
	#define FLUSH_CFG_FLUSH_QID_OFFSET  0
	#define FLUSH_CFG_FLUSH_QID_LEN     9
	#define FLUSH_CFG_FLUSH_QID_DEFAULT 0x0
	/*[field] FLUSH_STATUS*/
	#define FLUSH_CFG_FLUSH_STATUS
	#define FLUSH_CFG_FLUSH_STATUS_OFFSET  10
	#define FLUSH_CFG_FLUSH_STATUS_LEN     1
	#define FLUSH_CFG_FLUSH_STATUS_DEFAULT 0x0
	/*[field] FLUSH_WT_TIME*/
	#define FLUSH_CFG_FLUSH_WT_TIME
	#define FLUSH_CFG_FLUSH_WT_TIME_OFFSET  11
	#define FLUSH_CFG_FLUSH_WT_TIME_LEN     10
	#define FLUSH_CFG_FLUSH_WT_TIME_DEFAULT 0x100
	/*[field] FLUSH_DST_PORT*/
	#define FLUSH_CFG_FLUSH_DST_PORT
	#define FLUSH_CFG_FLUSH_DST_PORT_OFFSET  21
	#define FLUSH_CFG_FLUSH_DST_PORT_LEN     3
	#define FLUSH_CFG_FLUSH_DST_PORT_DEFAULT 0x0
	/*[field] FLUSH_ALL_QUEUES*/
	#define FLUSH_CFG_FLUSH_ALL_QUEUES
	#define FLUSH_CFG_FLUSH_ALL_QUEUES_OFFSET  24
	#define FLUSH_CFG_FLUSH_ALL_QUEUES_LEN     1
	#define FLUSH_CFG_FLUSH_ALL_QUEUES_DEFAULT 0x0
	/*[field] FLUSH_BUSY*/
	#define FLUSH_CFG_FLUSH_BUSY
	#define FLUSH_CFG_FLUSH_BUSY_OFFSET  31
	#define FLUSH_CFG_FLUSH_BUSY_LEN     1
	#define FLUSH_CFG_FLUSH_BUSY_DEFAULT 0x0

struct flush_cfg {
	a_uint32_t  flush_qid:9;
	a_uint32_t  _reserved0:1;
	a_uint32_t  flush_status:1;
	a_uint32_t  flush_wt_time:10;
	a_uint32_t  flush_dst_port:3;
	a_uint32_t  flush_all_queues:1;
	a_uint32_t  _reserved1:6;
	a_uint32_t  flush_busy:1;
};

union flush_cfg_u {
	a_uint32_t val;
	struct flush_cfg bf;
};

/*[register] IN_MIRROR_PRIORITY_CTRL*/
#define IN_MIRROR_PRIORITY_CTRL
#define IN_MIRROR_PRIORITY_CTRL_ADDRESS 0x4
#define IN_MIRROR_PRIORITY_CTRL_NUM     1
#define IN_MIRROR_PRIORITY_CTRL_INC     0x4
#define IN_MIRROR_PRIORITY_CTRL_TYPE    REG_TYPE_RW
#define IN_MIRROR_PRIORITY_CTRL_DEFAULT 0x0
	/*[field] PRIORITY*/
	#define IN_MIRROR_PRIORITY_CTRL_PRIORITY
	#define IN_MIRROR_PRIORITY_CTRL_PRIORITY_OFFSET  0
	#define IN_MIRROR_PRIORITY_CTRL_PRIORITY_LEN     4
	#define IN_MIRROR_PRIORITY_CTRL_PRIORITY_DEFAULT 0x0

struct in_mirror_priority_ctrl {
	a_uint32_t  priority:4;
	a_uint32_t  _reserved0:28;
};

union in_mirror_priority_ctrl_u {
	a_uint32_t val;
	struct in_mirror_priority_ctrl bf;
};

/*[register] EG_MIRROR_PRIORITY_CTRL*/
#define EG_MIRROR_PRIORITY_CTRL
#define EG_MIRROR_PRIORITY_CTRL_ADDRESS 0x8
#define EG_MIRROR_PRIORITY_CTRL_NUM     1
#define EG_MIRROR_PRIORITY_CTRL_INC     0x4
#define EG_MIRROR_PRIORITY_CTRL_TYPE    REG_TYPE_RW
#define EG_MIRROR_PRIORITY_CTRL_DEFAULT 0x0
	/*[field] PRIORITY*/
	#define EG_MIRROR_PRIORITY_CTRL_PRIORITY
	#define EG_MIRROR_PRIORITY_CTRL_PRIORITY_OFFSET  0
	#define EG_MIRROR_PRIORITY_CTRL_PRIORITY_LEN     4
	#define EG_MIRROR_PRIORITY_CTRL_PRIORITY_DEFAULT 0x0

struct eg_mirror_priority_ctrl {
	a_uint32_t  priority:4;
	a_uint32_t  _reserved0:28;
};

union eg_mirror_priority_ctrl_u {
	a_uint32_t val;
	struct eg_mirror_priority_ctrl bf;
};

/*[register] UCAST_DEFAULT_HASH*/
#define UCAST_DEFAULT_HASH
#define UCAST_DEFAULT_HASH_ADDRESS 0x60
#define UCAST_DEFAULT_HASH_NUM     1
#define UCAST_DEFAULT_HASH_INC     0x4
#define UCAST_DEFAULT_HASH_TYPE    REG_TYPE_RW
#define UCAST_DEFAULT_HASH_DEFAULT 0x0
	/*[field] HASH*/
	#define UCAST_DEFAULT_HASH_HASH
	#define UCAST_DEFAULT_HASH_HASH_OFFSET  0
	#define UCAST_DEFAULT_HASH_HASH_LEN     8
	#define UCAST_DEFAULT_HASH_HASH_DEFAULT 0x0

struct ucast_default_hash {
	a_uint32_t  hash:8;
	a_uint32_t  _reserved0:24;
};

union ucast_default_hash_u {
	a_uint32_t val;
	struct ucast_default_hash bf;
};

/*[register] SPARE_REG0*/
#define SPARE_REG0
#define SPARE_REG0_ADDRESS 0x70
#define SPARE_REG0_NUM     1
#define SPARE_REG0_INC     0x4
#define SPARE_REG0_TYPE    REG_TYPE_RW
#define SPARE_REG0_DEFAULT 0x0
	/*[field] SPARE_REG0*/
	#define SPARE_REG0_SPARE_REG0
	#define SPARE_REG0_SPARE_REG0_OFFSET  0
	#define SPARE_REG0_SPARE_REG0_LEN     32
	#define SPARE_REG0_SPARE_REG0_DEFAULT 0x0

struct spare_reg0 {
	a_uint32_t  spare_reg0:32;
};

union spare_reg0_u {
	a_uint32_t val;
	struct spare_reg0 bf;
};

/*[register] SPARE_REG1*/
#define SPARE_REG1
#define SPARE_REG1_ADDRESS 0x74
#define SPARE_REG1_NUM     1
#define SPARE_REG1_INC     0x4
#define SPARE_REG1_TYPE    REG_TYPE_RW
#define SPARE_REG1_DEFAULT 0x0
	/*[field] SPARE_REG1*/
	#define SPARE_REG1_SPARE_REG1
	#define SPARE_REG1_SPARE_REG1_OFFSET  0
	#define SPARE_REG1_SPARE_REG1_LEN     32
	#define SPARE_REG1_SPARE_REG1_DEFAULT 0x0

struct spare_reg1 {
	a_uint32_t  spare_reg1:32;
};

union spare_reg1_u {
	a_uint32_t val;
	struct spare_reg1 bf;
};

/*[register] QM_DBG_ADDR*/
#define QM_DBG_ADDR
#define QM_DBG_ADDR_ADDRESS 0x80
#define QM_DBG_ADDR_NUM     1
#define QM_DBG_ADDR_INC     0x4
#define QM_DBG_ADDR_TYPE    REG_TYPE_RW
#define QM_DBG_ADDR_DEFAULT 0x0
	/*[field] DBG_ADDR*/
	#define QM_DBG_ADDR_DBG_ADDR
	#define QM_DBG_ADDR_DBG_ADDR_OFFSET  0
	#define QM_DBG_ADDR_DBG_ADDR_LEN     8
	#define QM_DBG_ADDR_DBG_ADDR_DEFAULT 0x0

struct qm_dbg_addr {
	a_uint32_t  dbg_addr:8;
	a_uint32_t  _reserved0:24;
};

union qm_dbg_addr_u {
	a_uint32_t val;
	struct qm_dbg_addr bf;
};

/*[register] QM_DBG_DATA*/
#define QM_DBG_DATA
#define QM_DBG_DATA_ADDRESS 0x84
#define QM_DBG_DATA_NUM     1
#define QM_DBG_DATA_INC     0x4
#define QM_DBG_DATA_TYPE    REG_TYPE_RO
#define QM_DBG_DATA_DEFAULT 0x0
	/*[field] DBG_DATA*/
	#define QM_DBG_DATA_DBG_DATA
	#define QM_DBG_DATA_DBG_DATA_OFFSET  0
	#define QM_DBG_DATA_DBG_DATA_LEN     32
	#define QM_DBG_DATA_DBG_DATA_DEFAULT 0x0

struct qm_dbg_data {
	a_uint32_t  dbg_data:32;
};

union qm_dbg_data_u {
	a_uint32_t val;
	struct qm_dbg_data bf;
};

/*[register] MCAST_PRIORITY_MAP0*/
#define MCAST_PRIORITY_MAP0
#define MCAST_PRIORITY_MAP0_ADDRESS 0x100
#define MCAST_PRIORITY_MAP0_NUM     16
#define MCAST_PRIORITY_MAP0_INC     0x4
#define MCAST_PRIORITY_MAP0_TYPE    REG_TYPE_RW
#define MCAST_PRIORITY_MAP0_DEFAULT 0x0
	/*[field] CLASS*/
	#define MCAST_PRIORITY_MAP0_CLASS
	#define MCAST_PRIORITY_MAP0_CLASS_OFFSET  0
	#define MCAST_PRIORITY_MAP0_CLASS_LEN     4
	#define MCAST_PRIORITY_MAP0_CLASS_DEFAULT 0x0

struct mcast_priority_map0 {
	a_uint32_t  class:4;
	a_uint32_t  _reserved0:28;
};

union mcast_priority_map0_u {
	a_uint32_t val;
	struct mcast_priority_map0 bf;
};

/*[register] MCAST_PRIORITY_MAP1*/
#define MCAST_PRIORITY_MAP1
#define MCAST_PRIORITY_MAP1_ADDRESS 0x140
#define MCAST_PRIORITY_MAP1_NUM     16
#define MCAST_PRIORITY_MAP1_INC     0x4
#define MCAST_PRIORITY_MAP1_TYPE    REG_TYPE_RW
#define MCAST_PRIORITY_MAP1_DEFAULT 0x0
	/*[field] CLASS*/
	#define MCAST_PRIORITY_MAP1_CLASS
	#define MCAST_PRIORITY_MAP1_CLASS_OFFSET  0
	#define MCAST_PRIORITY_MAP1_CLASS_LEN     2
	#define MCAST_PRIORITY_MAP1_CLASS_DEFAULT 0x0

struct mcast_priority_map1 {
	a_uint32_t  class:2;
	a_uint32_t  _reserved0:30;
};

union mcast_priority_map1_u {
	a_uint32_t val;
	struct mcast_priority_map1 bf;
};

/*[register] MCAST_PRIORITY_MAP2*/
#define MCAST_PRIORITY_MAP2
#define MCAST_PRIORITY_MAP2_ADDRESS 0x180
#define MCAST_PRIORITY_MAP2_NUM     16
#define MCAST_PRIORITY_MAP2_INC     0x4
#define MCAST_PRIORITY_MAP2_TYPE    REG_TYPE_RW
#define MCAST_PRIORITY_MAP2_DEFAULT 0x0
	/*[field] CLASS*/
	#define MCAST_PRIORITY_MAP2_CLASS
	#define MCAST_PRIORITY_MAP2_CLASS_OFFSET  0
	#define MCAST_PRIORITY_MAP2_CLASS_LEN     2
	#define MCAST_PRIORITY_MAP2_CLASS_DEFAULT 0x0

struct mcast_priority_map2 {
	a_uint32_t  class:2;
	a_uint32_t  _reserved0:30;
};

union mcast_priority_map2_u {
	a_uint32_t val;
	struct mcast_priority_map2 bf;
};

/*[register] MCAST_PRIORITY_MAP3*/
#define MCAST_PRIORITY_MAP3
#define MCAST_PRIORITY_MAP3_ADDRESS 0x1c0
#define MCAST_PRIORITY_MAP3_NUM     16
#define MCAST_PRIORITY_MAP3_INC     0x4
#define MCAST_PRIORITY_MAP3_TYPE    REG_TYPE_RW
#define MCAST_PRIORITY_MAP3_DEFAULT 0x0
	/*[field] CLASS*/
	#define MCAST_PRIORITY_MAP3_CLASS
	#define MCAST_PRIORITY_MAP3_CLASS_OFFSET  0
	#define MCAST_PRIORITY_MAP3_CLASS_LEN     2
	#define MCAST_PRIORITY_MAP3_CLASS_DEFAULT 0x0

struct mcast_priority_map3 {
	a_uint32_t  class:2;
	a_uint32_t  _reserved0:30;
};

union mcast_priority_map3_u {
	a_uint32_t val;
	struct mcast_priority_map3 bf;
};

/*[register] MCAST_PRIORITY_MAP4*/
#define MCAST_PRIORITY_MAP4
#define MCAST_PRIORITY_MAP4_ADDRESS 0x200
#define MCAST_PRIORITY_MAP4_NUM     16
#define MCAST_PRIORITY_MAP4_INC     0x4
#define MCAST_PRIORITY_MAP4_TYPE    REG_TYPE_RW
#define MCAST_PRIORITY_MAP4_DEFAULT 0x0
	/*[field] CLASS*/
	#define MCAST_PRIORITY_MAP4_CLASS
	#define MCAST_PRIORITY_MAP4_CLASS_OFFSET  0
	#define MCAST_PRIORITY_MAP4_CLASS_LEN     2
	#define MCAST_PRIORITY_MAP4_CLASS_DEFAULT 0x0

struct mcast_priority_map4 {
	a_uint32_t  class:2;
	a_uint32_t  _reserved0:30;
};

union mcast_priority_map4_u {
	a_uint32_t val;
	struct mcast_priority_map4 bf;
};

/*[register] MCAST_PRIORITY_MAP5*/
#define MCAST_PRIORITY_MAP5
#define MCAST_PRIORITY_MAP5_ADDRESS 0x240
#define MCAST_PRIORITY_MAP5_NUM     16
#define MCAST_PRIORITY_MAP5_INC     0x4
#define MCAST_PRIORITY_MAP5_TYPE    REG_TYPE_RW
#define MCAST_PRIORITY_MAP5_DEFAULT 0x0
	/*[field] CLASS*/
	#define MCAST_PRIORITY_MAP5_CLASS
	#define MCAST_PRIORITY_MAP5_CLASS_OFFSET  0
	#define MCAST_PRIORITY_MAP5_CLASS_LEN     2
	#define MCAST_PRIORITY_MAP5_CLASS_DEFAULT 0x0

struct mcast_priority_map5 {
	a_uint32_t  class:2;
	a_uint32_t  _reserved0:30;
};

union mcast_priority_map5_u {
	a_uint32_t val;
	struct mcast_priority_map5 bf;
};

/*[register] MCAST_PRIORITY_MAP6*/
#define MCAST_PRIORITY_MAP6
#define MCAST_PRIORITY_MAP6_ADDRESS 0x280
#define MCAST_PRIORITY_MAP6_NUM     16
#define MCAST_PRIORITY_MAP6_INC     0x4
#define MCAST_PRIORITY_MAP6_TYPE    REG_TYPE_RW
#define MCAST_PRIORITY_MAP6_DEFAULT 0x0
	/*[field] CLASS*/
	#define MCAST_PRIORITY_MAP6_CLASS
	#define MCAST_PRIORITY_MAP6_CLASS_OFFSET  0
	#define MCAST_PRIORITY_MAP6_CLASS_LEN     2
	#define MCAST_PRIORITY_MAP6_CLASS_DEFAULT 0x0

struct mcast_priority_map6 {
	a_uint32_t  class:2;
	a_uint32_t  _reserved0:30;
};

union mcast_priority_map6_u {
	a_uint32_t val;
	struct mcast_priority_map6 bf;
};

/*[register] MCAST_PRIORITY_MAP7*/
#define MCAST_PRIORITY_MAP7
#define MCAST_PRIORITY_MAP7_ADDRESS 0x2c0
#define MCAST_PRIORITY_MAP7_NUM     16
#define MCAST_PRIORITY_MAP7_INC     0x4
#define MCAST_PRIORITY_MAP7_TYPE    REG_TYPE_RW
#define MCAST_PRIORITY_MAP7_DEFAULT 0x0
	/*[field] CLASS*/
	#define MCAST_PRIORITY_MAP7_CLASS
	#define MCAST_PRIORITY_MAP7_CLASS_OFFSET  0
	#define MCAST_PRIORITY_MAP7_CLASS_LEN     2
	#define MCAST_PRIORITY_MAP7_CLASS_DEFAULT 0x0

struct mcast_priority_map7 {
	a_uint32_t  class:2;
	a_uint32_t  _reserved0:30;
};

union mcast_priority_map7_u {
	a_uint32_t val;
	struct mcast_priority_map7 bf;
};

/*[register] AGG_PROFILE_CNT_EN*/
#define AGG_PROFILE_CNT_EN
#define AGG_PROFILE_CNT_EN_ADDRESS 0x300
#define AGG_PROFILE_CNT_EN_NUM     1
#define AGG_PROFILE_CNT_EN_INC     0x4
#define AGG_PROFILE_CNT_EN_TYPE    REG_TYPE_RW
#define AGG_PROFILE_CNT_EN_DEFAULT 0xfffff
	/*[field] UQ_EN_0*/
	#define AGG_PROFILE_CNT_EN_UQ_EN_0
	#define AGG_PROFILE_CNT_EN_UQ_EN_0_OFFSET  0
	#define AGG_PROFILE_CNT_EN_UQ_EN_0_LEN     1
	#define AGG_PROFILE_CNT_EN_UQ_EN_0_DEFAULT 0x1
	/*[field] UQ_EN_1*/
	#define AGG_PROFILE_CNT_EN_UQ_EN_1
	#define AGG_PROFILE_CNT_EN_UQ_EN_1_OFFSET  1
	#define AGG_PROFILE_CNT_EN_UQ_EN_1_LEN     1
	#define AGG_PROFILE_CNT_EN_UQ_EN_1_DEFAULT 0x1
	/*[field] UQ_EN_2*/
	#define AGG_PROFILE_CNT_EN_UQ_EN_2
	#define AGG_PROFILE_CNT_EN_UQ_EN_2_OFFSET  2
	#define AGG_PROFILE_CNT_EN_UQ_EN_2_LEN     1
	#define AGG_PROFILE_CNT_EN_UQ_EN_2_DEFAULT 0x1
	/*[field] UQ_EN_3*/
	#define AGG_PROFILE_CNT_EN_UQ_EN_3
	#define AGG_PROFILE_CNT_EN_UQ_EN_3_OFFSET  3
	#define AGG_PROFILE_CNT_EN_UQ_EN_3_LEN     1
	#define AGG_PROFILE_CNT_EN_UQ_EN_3_DEFAULT 0x1
	/*[field] UQ_EN_4*/
	#define AGG_PROFILE_CNT_EN_UQ_EN_4
	#define AGG_PROFILE_CNT_EN_UQ_EN_4_OFFSET  4
	#define AGG_PROFILE_CNT_EN_UQ_EN_4_LEN     1
	#define AGG_PROFILE_CNT_EN_UQ_EN_4_DEFAULT 0x1
	/*[field] UQ_EN_5*/
	#define AGG_PROFILE_CNT_EN_UQ_EN_5
	#define AGG_PROFILE_CNT_EN_UQ_EN_5_OFFSET  5
	#define AGG_PROFILE_CNT_EN_UQ_EN_5_LEN     1
	#define AGG_PROFILE_CNT_EN_UQ_EN_5_DEFAULT 0x1
	/*[field] UQ_EN_6*/
	#define AGG_PROFILE_CNT_EN_UQ_EN_6
	#define AGG_PROFILE_CNT_EN_UQ_EN_6_OFFSET  6
	#define AGG_PROFILE_CNT_EN_UQ_EN_6_LEN     1
	#define AGG_PROFILE_CNT_EN_UQ_EN_6_DEFAULT 0x1
	/*[field] UQ_EN_7*/
	#define AGG_PROFILE_CNT_EN_UQ_EN_7
	#define AGG_PROFILE_CNT_EN_UQ_EN_7_OFFSET  7
	#define AGG_PROFILE_CNT_EN_UQ_EN_7_LEN     1
	#define AGG_PROFILE_CNT_EN_UQ_EN_7_DEFAULT 0x1
	/*[field] MQ_P0_EN*/
	#define AGG_PROFILE_CNT_EN_MQ_P0_EN
	#define AGG_PROFILE_CNT_EN_MQ_P0_EN_OFFSET  8
	#define AGG_PROFILE_CNT_EN_MQ_P0_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_MQ_P0_EN_DEFAULT 0x1
	/*[field] MQ_P1_EN*/
	#define AGG_PROFILE_CNT_EN_MQ_P1_EN
	#define AGG_PROFILE_CNT_EN_MQ_P1_EN_OFFSET  9
	#define AGG_PROFILE_CNT_EN_MQ_P1_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_MQ_P1_EN_DEFAULT 0x1
	/*[field] MQ_P2_EN*/
	#define AGG_PROFILE_CNT_EN_MQ_P2_EN
	#define AGG_PROFILE_CNT_EN_MQ_P2_EN_OFFSET  10
	#define AGG_PROFILE_CNT_EN_MQ_P2_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_MQ_P2_EN_DEFAULT 0x1
	/*[field] MQ_P3_EN*/
	#define AGG_PROFILE_CNT_EN_MQ_P3_EN
	#define AGG_PROFILE_CNT_EN_MQ_P3_EN_OFFSET  11
	#define AGG_PROFILE_CNT_EN_MQ_P3_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_MQ_P3_EN_DEFAULT 0x1
	/*[field] MQ_P4_EN*/
	#define AGG_PROFILE_CNT_EN_MQ_P4_EN
	#define AGG_PROFILE_CNT_EN_MQ_P4_EN_OFFSET  12
	#define AGG_PROFILE_CNT_EN_MQ_P4_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_MQ_P4_EN_DEFAULT 0x1
	/*[field] MQ_P5_EN*/
	#define AGG_PROFILE_CNT_EN_MQ_P5_EN
	#define AGG_PROFILE_CNT_EN_MQ_P5_EN_OFFSET  13
	#define AGG_PROFILE_CNT_EN_MQ_P5_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_MQ_P5_EN_DEFAULT 0x1
	/*[field] MQ_P6_EN*/
	#define AGG_PROFILE_CNT_EN_MQ_P6_EN
	#define AGG_PROFILE_CNT_EN_MQ_P6_EN_OFFSET  14
	#define AGG_PROFILE_CNT_EN_MQ_P6_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_MQ_P6_EN_DEFAULT 0x1
	/*[field] MQ_P7_EN*/
	#define AGG_PROFILE_CNT_EN_MQ_P7_EN
	#define AGG_PROFILE_CNT_EN_MQ_P7_EN_OFFSET  15
	#define AGG_PROFILE_CNT_EN_MQ_P7_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_MQ_P7_EN_DEFAULT 0x1
	/*[field] GRP_0_EN*/
	#define AGG_PROFILE_CNT_EN_GRP_0_EN
	#define AGG_PROFILE_CNT_EN_GRP_0_EN_OFFSET  16
	#define AGG_PROFILE_CNT_EN_GRP_0_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_GRP_0_EN_DEFAULT 0x1
	/*[field] GRP_1_EN*/
	#define AGG_PROFILE_CNT_EN_GRP_1_EN
	#define AGG_PROFILE_CNT_EN_GRP_1_EN_OFFSET  17
	#define AGG_PROFILE_CNT_EN_GRP_1_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_GRP_1_EN_DEFAULT 0x1
	/*[field] GRP_2_EN*/
	#define AGG_PROFILE_CNT_EN_GRP_2_EN
	#define AGG_PROFILE_CNT_EN_GRP_2_EN_OFFSET  18
	#define AGG_PROFILE_CNT_EN_GRP_2_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_GRP_2_EN_DEFAULT 0x1
	/*[field] GRP_3_EN*/
	#define AGG_PROFILE_CNT_EN_GRP_3_EN
	#define AGG_PROFILE_CNT_EN_GRP_3_EN_OFFSET  19
	#define AGG_PROFILE_CNT_EN_GRP_3_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_GRP_3_EN_DEFAULT 0x1
	/*[field] GLOBAL_EN*/
	#define AGG_PROFILE_CNT_EN_GLOBAL_EN
	#define AGG_PROFILE_CNT_EN_GLOBAL_EN_OFFSET  31
	#define AGG_PROFILE_CNT_EN_GLOBAL_EN_LEN     1
	#define AGG_PROFILE_CNT_EN_GLOBAL_EN_DEFAULT 0x0

struct agg_profile_cnt_en {
	a_uint32_t  uq_en_0:1;
	a_uint32_t  uq_en_1:1;
	a_uint32_t  uq_en_2:1;
	a_uint32_t  uq_en_3:1;
	a_uint32_t  uq_en_4:1;
	a_uint32_t  uq_en_5:1;
	a_uint32_t  uq_en_6:1;
	a_uint32_t  uq_en_7:1;
	a_uint32_t  mq_p0_en:1;
	a_uint32_t  mq_p1_en:1;
	a_uint32_t  mq_p2_en:1;
	a_uint32_t  mq_p3_en:1;
	a_uint32_t  mq_p4_en:1;
	a_uint32_t  mq_p5_en:1;
	a_uint32_t  mq_p6_en:1;
	a_uint32_t  mq_p7_en:1;
	a_uint32_t  grp_0_en:1;
	a_uint32_t  grp_1_en:1;
	a_uint32_t  grp_2_en:1;
	a_uint32_t  grp_3_en:1;
	a_uint32_t  _reserved0:11;
	a_uint32_t  global_en:1;
};

union agg_profile_cnt_en_u {
	a_uint32_t val;
	struct agg_profile_cnt_en bf;
};

/*[register] UQ_AGG_PROFILE_CFG*/
#define UQ_AGG_PROFILE_CFG
#define UQ_AGG_PROFILE_CFG_ADDRESS 0x320
#define UQ_AGG_PROFILE_CFG_NUM     8
#define UQ_AGG_PROFILE_CFG_INC     0x4
#define UQ_AGG_PROFILE_CFG_TYPE    REG_TYPE_RW
#define UQ_AGG_PROFILE_CFG_DEFAULT 0x0
	/*[field] TH_CFG*/
	#define UQ_AGG_PROFILE_CFG_TH_CFG
	#define UQ_AGG_PROFILE_CFG_TH_CFG_OFFSET  0
	#define UQ_AGG_PROFILE_CFG_TH_CFG_LEN     11
	#define UQ_AGG_PROFILE_CFG_TH_CFG_DEFAULT 0x0

struct uq_agg_profile_cfg {
	a_uint32_t  th_cfg:11;
	a_uint32_t  _reserved0:21;
};

union uq_agg_profile_cfg_u {
	a_uint32_t val;
	struct uq_agg_profile_cfg bf;
};

/*[register] MQ_AGG_PROFILE_CFG*/
#define MQ_AGG_PROFILE_CFG
#define MQ_AGG_PROFILE_CFG_ADDRESS 0x340
#define MQ_AGG_PROFILE_CFG_NUM     8
#define MQ_AGG_PROFILE_CFG_INC     0x4
#define MQ_AGG_PROFILE_CFG_TYPE    REG_TYPE_RW
#define MQ_AGG_PROFILE_CFG_DEFAULT 0x0
	/*[field] TH_CFG*/
	#define MQ_AGG_PROFILE_CFG_TH_CFG
	#define MQ_AGG_PROFILE_CFG_TH_CFG_OFFSET  0
	#define MQ_AGG_PROFILE_CFG_TH_CFG_LEN     11
	#define MQ_AGG_PROFILE_CFG_TH_CFG_DEFAULT 0x0

struct mq_agg_profile_cfg {
	a_uint32_t  th_cfg:11;
	a_uint32_t  _reserved0:21;
};

union mq_agg_profile_cfg_u {
	a_uint32_t val;
	struct mq_agg_profile_cfg bf;
};

/*[register] GRP_AGG_PROFILE_CFG*/
#define GRP_AGG_PROFILE_CFG
#define GRP_AGG_PROFILE_CFG_ADDRESS 0x360
#define GRP_AGG_PROFILE_CFG_NUM     4
#define GRP_AGG_PROFILE_CFG_INC     0x4
#define GRP_AGG_PROFILE_CFG_TYPE    REG_TYPE_RW
#define GRP_AGG_PROFILE_CFG_DEFAULT 0x0
	/*[field] TH_CFG*/
	#define GRP_AGG_PROFILE_CFG_TH_CFG
	#define GRP_AGG_PROFILE_CFG_TH_CFG_OFFSET  0
	#define GRP_AGG_PROFILE_CFG_TH_CFG_LEN     11
	#define GRP_AGG_PROFILE_CFG_TH_CFG_DEFAULT 0x0

struct grp_agg_profile_cfg {
	a_uint32_t  th_cfg:11;
	a_uint32_t  _reserved0:21;
};

union grp_agg_profile_cfg_u {
	a_uint32_t val;
	struct grp_agg_profile_cfg bf;
};

/*[register] UQ_AGG_IN_PROFILE_CNT*/
#define UQ_AGG_IN_PROFILE_CNT
#define UQ_AGG_IN_PROFILE_CNT_ADDRESS 0x380
#define UQ_AGG_IN_PROFILE_CNT_NUM     8
#define UQ_AGG_IN_PROFILE_CNT_INC     0x4
#define UQ_AGG_IN_PROFILE_CNT_TYPE    REG_TYPE_RW
#define UQ_AGG_IN_PROFILE_CNT_DEFAULT 0x0
	/*[field] CNT*/
	#define UQ_AGG_IN_PROFILE_CNT_CNT
	#define UQ_AGG_IN_PROFILE_CNT_CNT_OFFSET  0
	#define UQ_AGG_IN_PROFILE_CNT_CNT_LEN     32
	#define UQ_AGG_IN_PROFILE_CNT_CNT_DEFAULT 0x0

struct uq_agg_in_profile_cnt {
	a_uint32_t  cnt:32;
};

union uq_agg_in_profile_cnt_u {
	a_uint32_t val;
	struct uq_agg_in_profile_cnt bf;
};

/*[register] UQ_AGG_OUT_PROFILE_CNT*/
#define UQ_AGG_OUT_PROFILE_CNT
#define UQ_AGG_OUT_PROFILE_CNT_ADDRESS 0x3a0
#define UQ_AGG_OUT_PROFILE_CNT_NUM     8
#define UQ_AGG_OUT_PROFILE_CNT_INC     0x4
#define UQ_AGG_OUT_PROFILE_CNT_TYPE    REG_TYPE_RW
#define UQ_AGG_OUT_PROFILE_CNT_DEFAULT 0x0
	/*[field] CNT*/
	#define UQ_AGG_OUT_PROFILE_CNT_CNT
	#define UQ_AGG_OUT_PROFILE_CNT_CNT_OFFSET  0
	#define UQ_AGG_OUT_PROFILE_CNT_CNT_LEN     32
	#define UQ_AGG_OUT_PROFILE_CNT_CNT_DEFAULT 0x0

struct uq_agg_out_profile_cnt {
	a_uint32_t  cnt:32;
};

union uq_agg_out_profile_cnt_u {
	a_uint32_t val;
	struct uq_agg_out_profile_cnt bf;
};

/*[register] MQ_AGG_IN_PROFILE_CNT*/
#define MQ_AGG_IN_PROFILE_CNT
#define MQ_AGG_IN_PROFILE_CNT_ADDRESS 0x3c0
#define MQ_AGG_IN_PROFILE_CNT_NUM     8
#define MQ_AGG_IN_PROFILE_CNT_INC     0x4
#define MQ_AGG_IN_PROFILE_CNT_TYPE    REG_TYPE_RW
#define MQ_AGG_IN_PROFILE_CNT_DEFAULT 0x0
	/*[field] CNT*/
	#define MQ_AGG_IN_PROFILE_CNT_CNT
	#define MQ_AGG_IN_PROFILE_CNT_CNT_OFFSET  0
	#define MQ_AGG_IN_PROFILE_CNT_CNT_LEN     32
	#define MQ_AGG_IN_PROFILE_CNT_CNT_DEFAULT 0x0

struct mq_agg_in_profile_cnt {
	a_uint32_t  cnt:32;
};

union mq_agg_in_profile_cnt_u {
	a_uint32_t val;
	struct mq_agg_in_profile_cnt bf;
};

/*[register] MQ_AGG_OUT_PROFILE_CNT*/
#define MQ_AGG_OUT_PROFILE_CNT
#define MQ_AGG_OUT_PROFILE_CNT_ADDRESS 0x3e0
#define MQ_AGG_OUT_PROFILE_CNT_NUM     8
#define MQ_AGG_OUT_PROFILE_CNT_INC     0x4
#define MQ_AGG_OUT_PROFILE_CNT_TYPE    REG_TYPE_RW
#define MQ_AGG_OUT_PROFILE_CNT_DEFAULT 0x0
	/*[field] CNT*/
	#define MQ_AGG_OUT_PROFILE_CNT_CNT
	#define MQ_AGG_OUT_PROFILE_CNT_CNT_OFFSET  0
	#define MQ_AGG_OUT_PROFILE_CNT_CNT_LEN     32
	#define MQ_AGG_OUT_PROFILE_CNT_CNT_DEFAULT 0x0

struct mq_agg_out_profile_cnt {
	a_uint32_t  cnt:32;
};

union mq_agg_out_profile_cnt_u {
	a_uint32_t val;
	struct mq_agg_out_profile_cnt bf;
};

/*[register] GRP_AGG_IN_PROFILE_CNT*/
#define GRP_AGG_IN_PROFILE_CNT
#define GRP_AGG_IN_PROFILE_CNT_ADDRESS 0x400
#define GRP_AGG_IN_PROFILE_CNT_NUM     4
#define GRP_AGG_IN_PROFILE_CNT_INC     0x4
#define GRP_AGG_IN_PROFILE_CNT_TYPE    REG_TYPE_RW
#define GRP_AGG_IN_PROFILE_CNT_DEFAULT 0x0
	/*[field] CNT*/
	#define GRP_AGG_IN_PROFILE_CNT_CNT
	#define GRP_AGG_IN_PROFILE_CNT_CNT_OFFSET  0
	#define GRP_AGG_IN_PROFILE_CNT_CNT_LEN     32
	#define GRP_AGG_IN_PROFILE_CNT_CNT_DEFAULT 0x0

struct grp_agg_in_profile_cnt {
	a_uint32_t  cnt:32;
};

union grp_agg_in_profile_cnt_u {
	a_uint32_t val;
	struct grp_agg_in_profile_cnt bf;
};

/*[register] GRP_AGG_OUT_PROFILE_CNT*/
#define GRP_AGG_OUT_PROFILE_CNT
#define GRP_AGG_OUT_PROFILE_CNT_ADDRESS 0x410
#define GRP_AGG_OUT_PROFILE_CNT_NUM     4
#define GRP_AGG_OUT_PROFILE_CNT_INC     0x4
#define GRP_AGG_OUT_PROFILE_CNT_TYPE    REG_TYPE_RW
#define GRP_AGG_OUT_PROFILE_CNT_DEFAULT 0x0
	/*[field] CNT*/
	#define GRP_AGG_OUT_PROFILE_CNT_CNT
	#define GRP_AGG_OUT_PROFILE_CNT_CNT_OFFSET  0
	#define GRP_AGG_OUT_PROFILE_CNT_CNT_LEN     32
	#define GRP_AGG_OUT_PROFILE_CNT_CNT_DEFAULT 0x0

struct grp_agg_out_profile_cnt {
	a_uint32_t  cnt:32;
};

union grp_agg_out_profile_cnt_u {
	a_uint32_t val;
	struct grp_agg_out_profile_cnt bf;
};

/*[table] UCAST_QUEUE_MAP_TBL*/
#define UCAST_QUEUE_MAP_TBL
#define UCAST_QUEUE_MAP_TBL_ADDRESS 0x10000
#define UCAST_QUEUE_MAP_TBL_NUM     3072
#define UCAST_QUEUE_MAP_TBL_INC     0x10
#define UCAST_QUEUE_MAP_TBL_TYPE    REG_TYPE_RW
#define UCAST_QUEUE_MAP_TBL_DEFAULT 0x0
	/*[field] PROFILE_ID*/
	#define UCAST_QUEUE_MAP_TBL_PROFILE_ID
	#define UCAST_QUEUE_MAP_TBL_PROFILE_ID_OFFSET  0
	#define UCAST_QUEUE_MAP_TBL_PROFILE_ID_LEN     4
	#define UCAST_QUEUE_MAP_TBL_PROFILE_ID_DEFAULT 0x0
	/*[field] QUEUE_ID*/
	#define UCAST_QUEUE_MAP_TBL_QUEUE_ID
	#define UCAST_QUEUE_MAP_TBL_QUEUE_ID_OFFSET  4
	#define UCAST_QUEUE_MAP_TBL_QUEUE_ID_LEN     8
	#define UCAST_QUEUE_MAP_TBL_QUEUE_ID_DEFAULT 0x0

struct ucast_queue_map_tbl {
	a_uint32_t  profile_id:4;
	a_uint32_t  queue_id:8;
	a_uint32_t  _reserved0:20;
};

union ucast_queue_map_tbl_u {
	a_uint32_t val;
	struct ucast_queue_map_tbl bf;
};

/*[table] UCAST_HASH_MAP_TBL*/
#define UCAST_HASH_MAP_TBL
#define UCAST_HASH_MAP_TBL_ADDRESS 0x30000
#define UCAST_HASH_MAP_TBL_NUM     4096
#define UCAST_HASH_MAP_TBL_INC     0x10
#define UCAST_HASH_MAP_TBL_TYPE    REG_TYPE_RW
#define UCAST_HASH_MAP_TBL_DEFAULT 0x0
	/*[field] HASH*/
	#define UCAST_HASH_MAP_TBL_HASH
	#define UCAST_HASH_MAP_TBL_HASH_OFFSET  0
	#define UCAST_HASH_MAP_TBL_HASH_LEN     8
	#define UCAST_HASH_MAP_TBL_HASH_DEFAULT 0x0

struct ucast_hash_map_tbl {
	a_uint32_t  hash:8;
	a_uint32_t  _reserved0:24;
};

union ucast_hash_map_tbl_u {
	a_uint32_t val;
	struct ucast_hash_map_tbl bf;
};

/*[table] UCAST_PRIORITY_MAP_TBL*/
#define UCAST_PRIORITY_MAP_TBL
#define UCAST_PRIORITY_MAP_TBL_ADDRESS 0x42000
#define UCAST_PRIORITY_MAP_TBL_NUM     256
#define UCAST_PRIORITY_MAP_TBL_INC     0x10
#define UCAST_PRIORITY_MAP_TBL_TYPE    REG_TYPE_RW
#define UCAST_PRIORITY_MAP_TBL_DEFAULT 0x0
	/*[field] CLASS*/
	#define UCAST_PRIORITY_MAP_TBL_CLASS
	#define UCAST_PRIORITY_MAP_TBL_CLASS_OFFSET  0
	#define UCAST_PRIORITY_MAP_TBL_CLASS_LEN     4
	#define UCAST_PRIORITY_MAP_TBL_CLASS_DEFAULT 0x0

struct ucast_priority_map_tbl {
	a_uint32_t  class:4;
	a_uint32_t  _reserved0:28;
};

union ucast_priority_map_tbl_u {
	a_uint32_t val;
	struct ucast_priority_map_tbl bf;
};

/*[table] MCAST_QUEUE_MAP_TBL*/
#define MCAST_QUEUE_MAP_TBL
#define MCAST_QUEUE_MAP_TBL_ADDRESS 0x44000
#define MCAST_QUEUE_MAP_TBL_NUM     256
#define MCAST_QUEUE_MAP_TBL_INC     0x10
#define MCAST_QUEUE_MAP_TBL_TYPE    REG_TYPE_RW
#define MCAST_QUEUE_MAP_TBL_DEFAULT 0x0
	/*[field] CLASS*/
	#define MCAST_QUEUE_MAP_TBL_CLASS
	#define MCAST_QUEUE_MAP_TBL_CLASS_OFFSET  0
	#define MCAST_QUEUE_MAP_TBL_CLASS_LEN     4
	#define MCAST_QUEUE_MAP_TBL_CLASS_DEFAULT 0x0

struct mcast_queue_map_tbl {
	a_uint32_t  class:4;
	a_uint32_t  _reserved0:28;
};

union mcast_queue_map_tbl_u {
	a_uint32_t val;
	struct mcast_queue_map_tbl bf;
};

/*[table] AC_MSEQ_TBL*/
#define AC_MSEQ_TBL
#define AC_MSEQ_TBL_ADDRESS 0x46000
#define AC_MSEQ_TBL_NUM     256
#define AC_MSEQ_TBL_INC     0x10
#define AC_MSEQ_TBL_TYPE    REG_TYPE_RW
#define AC_MSEQ_TBL_DEFAULT 0x0
	/*[field] AC_MSEQ*/
	#define AC_MSEQ_TBL_AC_MSEQ
	#define AC_MSEQ_TBL_AC_MSEQ_OFFSET  0
	#define AC_MSEQ_TBL_AC_MSEQ_LEN     15
	#define AC_MSEQ_TBL_AC_MSEQ_DEFAULT 0x0

struct ac_mseq_tbl {
	a_uint32_t  ac_mseq:15;
	a_uint32_t  _reserved0:17;
};

union ac_mseq_tbl_u {
	a_uint32_t val;
	struct ac_mseq_tbl bf;
};

/*[table] AC_UNI_QUEUE_CFG_TBL*/
#define AC_UNI_QUEUE_CFG_TBL
#define AC_UNI_QUEUE_CFG_TBL_ADDRESS 0x48000
#define AC_UNI_QUEUE_CFG_TBL_NUM     256
#define AC_UNI_QUEUE_CFG_TBL_INC     0x10
#define AC_UNI_QUEUE_CFG_TBL_TYPE    REG_TYPE_RW
#define AC_UNI_QUEUE_CFG_TBL_DEFAULT 0x0
	/*[field] AC_CFG_AC_EN*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_AC_EN
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_AC_EN_OFFSET  0
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_AC_EN_LEN     1
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_AC_EN_DEFAULT 0x0
	/*[field] AC_CFG_WRED_EN*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_WRED_EN
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_WRED_EN_OFFSET  1
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_WRED_EN_LEN     1
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_WRED_EN_DEFAULT 0x0
	/*[field] AC_CFG_FORCE_AC_EN*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_FORCE_AC_EN
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_FORCE_AC_EN_OFFSET  2
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_FORCE_AC_EN_LEN     1
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_FORCE_AC_EN_DEFAULT 0x0
	/*[field] AC_CFG_COLOR_AWARE*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_COLOR_AWARE
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_COLOR_AWARE_OFFSET  3
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_COLOR_AWARE_LEN     1
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_COLOR_AWARE_DEFAULT 0x0
	/*[field] AC_CFG_GRP_ID*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GRP_ID
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GRP_ID_OFFSET  4
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GRP_ID_LEN     2
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GRP_ID_DEFAULT 0x0
	/*[field] AC_CFG_PRE_ALLOC_LIMIT*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_PRE_ALLOC_LIMIT
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_PRE_ALLOC_LIMIT_OFFSET  6
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_PRE_ALLOC_LIMIT_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_PRE_ALLOC_LIMIT_DEFAULT 0x0
	/*[field] AC_CFG_SHARED_DYNAMIC*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_DYNAMIC
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_DYNAMIC_OFFSET  17
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_DYNAMIC_LEN     1
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_DYNAMIC_DEFAULT 0x0
	/*[field] AC_CFG_SHARED_WEIGHT*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_WEIGHT
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_WEIGHT_OFFSET  18
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_WEIGHT_LEN     3
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_WEIGHT_DEFAULT 0x0
	/*[field] AC_CFG_SHARED_CEILING*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_CEILING
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_CEILING_OFFSET  21
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_CEILING_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_SHARED_CEILING_DEFAULT 0x0
	/*[field] AC_CFG_GAP_GRN_GRN_MIN*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_GRN_MIN
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_GRN_MIN_OFFSET  32
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_GRN_MIN_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_GRN_MIN_DEFAULT 0x0
	/*[field] AC_CFG_GAP_GRN_YEL_MAX*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_MAX
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_MAX_OFFSET  43
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_MAX_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_MAX_DEFAULT 0x0
	/*[field] AC_CFG_GAP_GRN_YEL_MIN*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_MIN
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_MIN_OFFSET  54
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_MIN_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_MIN_DEFAULT 0x0
	/*[field] AC_CFG_GAP_GRN_RED_MAX*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_MAX
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_MAX_OFFSET  65
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_MAX_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_MAX_DEFAULT 0x0
	/*[field] AC_CFG_GAP_GRN_RED_MIN*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_MIN
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_MIN_OFFSET  76
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_MIN_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_MIN_DEFAULT 0x0
	/*[field] AC_CFG_RED_RESUME_OFFSET*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_RED_RESUME_OFFSET
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_RED_RESUME_OFFSET_OFFSET  87
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_RED_RESUME_OFFSET_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_RED_RESUME_OFFSET_DEFAULT 0x0
	/*[field] AC_CFG_YEL_RESUME_OFFSET*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_YEL_RESUME_OFFSET
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_YEL_RESUME_OFFSET_OFFSET  98
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_YEL_RESUME_OFFSET_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_YEL_RESUME_OFFSET_DEFAULT 0x0
	/*[field] AC_CFG_GRN_RESUME_OFFSET*/
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GRN_RESUME_OFFSET
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GRN_RESUME_OFFSET_OFFSET  109
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GRN_RESUME_OFFSET_LEN     11
	#define AC_UNI_QUEUE_CFG_TBL_AC_CFG_GRN_RESUME_OFFSET_DEFAULT 0x0

struct ac_uni_queue_cfg_tbl {
	a_uint32_t  ac_cfg_ac_en:1;
	a_uint32_t  ac_cfg_wred_en:1;
	a_uint32_t  ac_cfg_force_ac_en:1;
	a_uint32_t  ac_cfg_color_aware:1;
	a_uint32_t  ac_cfg_grp_id:2;
	a_uint32_t  ac_cfg_pre_alloc_limit:11;
	a_uint32_t  ac_cfg_shared_dynamic:1;
	a_uint32_t  ac_cfg_shared_weight:3;
	a_uint32_t  ac_cfg_shared_ceiling:11;
	a_uint32_t  ac_cfg_gap_grn_grn_min:11;
	a_uint32_t  ac_cfg_gap_grn_yel_max:11;
	a_uint32_t  ac_cfg_gap_grn_yel_min_0:10;
	a_uint32_t  ac_cfg_gap_grn_yel_min_1:1;
	a_uint32_t  ac_cfg_gap_grn_red_max:11;
	a_uint32_t  ac_cfg_gap_grn_red_min:11;
	a_uint32_t  ac_cfg_red_resume_offset_0:9;
	a_uint32_t  ac_cfg_red_resume_offset_1:2;
	a_uint32_t  ac_cfg_yel_resume_offset:11;
	a_uint32_t  ac_cfg_grn_resume_offset:11;
	a_uint32_t  _reserved0:8;
};

union ac_uni_queue_cfg_tbl_u {
	a_uint32_t val[4];
	struct ac_uni_queue_cfg_tbl bf;
};

/*[table] AC_MUL_QUEUE_CFG_TBL*/
#define AC_MUL_QUEUE_CFG_TBL
#define AC_MUL_QUEUE_CFG_TBL_ADDRESS 0x4a000
#define AC_MUL_QUEUE_CFG_TBL_NUM     44
#define AC_MUL_QUEUE_CFG_TBL_INC     0x10
#define AC_MUL_QUEUE_CFG_TBL_TYPE    REG_TYPE_RW
#define AC_MUL_QUEUE_CFG_TBL_DEFAULT 0x0
	/*[field] AC_CFG_AC_EN*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_AC_EN
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_AC_EN_OFFSET  0
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_AC_EN_LEN     1
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_AC_EN_DEFAULT 0x0
	/*[field] AC_CFG_FORCE_AC_EN*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_FORCE_AC_EN
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_FORCE_AC_EN_OFFSET  1
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_FORCE_AC_EN_LEN     1
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_FORCE_AC_EN_DEFAULT 0x0
	/*[field] AC_CFG_COLOR_AWARE*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_COLOR_AWARE
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_COLOR_AWARE_OFFSET  2
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_COLOR_AWARE_LEN     1
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_COLOR_AWARE_DEFAULT 0x0
	/*[field] AC_CFG_GRP_ID*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GRP_ID
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GRP_ID_OFFSET  3
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GRP_ID_LEN     2
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GRP_ID_DEFAULT 0x0
	/*[field] AC_CFG_PRE_ALLOC_LIMIT*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_PRE_ALLOC_LIMIT
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_PRE_ALLOC_LIMIT_OFFSET  5
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_PRE_ALLOC_LIMIT_LEN     11
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_PRE_ALLOC_LIMIT_DEFAULT 0x0
	/*[field] AC_CFG_SHARED_CEILING*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_SHARED_CEILING
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_SHARED_CEILING_OFFSET  16
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_SHARED_CEILING_LEN     11
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_SHARED_CEILING_DEFAULT 0x0
	/*[field] AC_CFG_GAP_GRN_YEL*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_OFFSET  27
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_LEN     11
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_YEL_DEFAULT 0x0
	/*[field] AC_CFG_GAP_GRN_RED*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_OFFSET  38
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_LEN     11
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GAP_GRN_RED_DEFAULT 0x0
	/*[field] AC_CFG_RED_RESUME_OFFSET*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_RED_RESUME_OFFSET
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_RED_RESUME_OFFSET_OFFSET  49
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_RED_RESUME_OFFSET_LEN     11
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_RED_RESUME_OFFSET_DEFAULT 0x0
	/*[field] AC_CFG_YEL_RESUME_OFFSET*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_YEL_RESUME_OFFSET
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_YEL_RESUME_OFFSET_OFFSET  60
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_YEL_RESUME_OFFSET_LEN     11
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_YEL_RESUME_OFFSET_DEFAULT 0x0
	/*[field] AC_CFG_GRN_RESUME_OFFSET*/
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GRN_RESUME_OFFSET
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GRN_RESUME_OFFSET_OFFSET  71
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GRN_RESUME_OFFSET_LEN     11
	#define AC_MUL_QUEUE_CFG_TBL_AC_CFG_GRN_RESUME_OFFSET_DEFAULT 0x0

struct ac_mul_queue_cfg_tbl {
	a_uint32_t  ac_cfg_ac_en:1;
	a_uint32_t  ac_cfg_force_ac_en:1;
	a_uint32_t  ac_cfg_color_aware:1;
	a_uint32_t  ac_cfg_grp_id:2;
	a_uint32_t  ac_cfg_pre_alloc_limit:11;
	a_uint32_t  ac_cfg_shared_ceiling:11;
	a_uint32_t  ac_cfg_gap_grn_yel_0:5;
	a_uint32_t  ac_cfg_gap_grn_yel_1:6;
	a_uint32_t  ac_cfg_gap_grn_red:11;
	a_uint32_t  ac_cfg_red_resume_offset:11;
	a_uint32_t  ac_cfg_yel_resume_offset_0:4;
	a_uint32_t  ac_cfg_yel_resume_offset_1:7;
	a_uint32_t  ac_cfg_grn_resume_offset:11;
	a_uint32_t  _reserved0:14;
};

union ac_mul_queue_cfg_tbl_u {
	a_uint32_t val[3];
	struct ac_mul_queue_cfg_tbl bf;
};

/*[table] AC_GRP_CFG_TBL*/
#define AC_GRP_CFG_TBL
#define AC_GRP_CFG_TBL_ADDRESS 0x4c000
#define AC_GRP_CFG_TBL_NUM     4
#define AC_GRP_CFG_TBL_INC     0x10
#define AC_GRP_CFG_TBL_TYPE    REG_TYPE_RW
#define AC_GRP_CFG_TBL_DEFAULT 0x0
	/*[field] AC_CFG_AC_EN*/
	#define AC_GRP_CFG_TBL_AC_CFG_AC_EN
	#define AC_GRP_CFG_TBL_AC_CFG_AC_EN_OFFSET  0
	#define AC_GRP_CFG_TBL_AC_CFG_AC_EN_LEN     1
	#define AC_GRP_CFG_TBL_AC_CFG_AC_EN_DEFAULT 0x0
	/*[field] AC_CFG_FORCE_AC_EN*/
	#define AC_GRP_CFG_TBL_AC_CFG_FORCE_AC_EN
	#define AC_GRP_CFG_TBL_AC_CFG_FORCE_AC_EN_OFFSET  1
	#define AC_GRP_CFG_TBL_AC_CFG_FORCE_AC_EN_LEN     1
	#define AC_GRP_CFG_TBL_AC_CFG_FORCE_AC_EN_DEFAULT 0x0
	/*[field] AC_CFG_COLOR_AWARE*/
	#define AC_GRP_CFG_TBL_AC_CFG_COLOR_AWARE
	#define AC_GRP_CFG_TBL_AC_CFG_COLOR_AWARE_OFFSET  2
	#define AC_GRP_CFG_TBL_AC_CFG_COLOR_AWARE_LEN     1
	#define AC_GRP_CFG_TBL_AC_CFG_COLOR_AWARE_DEFAULT 0x0
	/*[field] AC_GRP_GAP_GRN_RED*/
	#define AC_GRP_CFG_TBL_AC_GRP_GAP_GRN_RED
	#define AC_GRP_CFG_TBL_AC_GRP_GAP_GRN_RED_OFFSET  3
	#define AC_GRP_CFG_TBL_AC_GRP_GAP_GRN_RED_LEN     11
	#define AC_GRP_CFG_TBL_AC_GRP_GAP_GRN_RED_DEFAULT 0x0
	/*[field] AC_GRP_GAP_GRN_YEL*/
	#define AC_GRP_CFG_TBL_AC_GRP_GAP_GRN_YEL
	#define AC_GRP_CFG_TBL_AC_GRP_GAP_GRN_YEL_OFFSET  14
	#define AC_GRP_CFG_TBL_AC_GRP_GAP_GRN_YEL_LEN     11
	#define AC_GRP_CFG_TBL_AC_GRP_GAP_GRN_YEL_DEFAULT 0x0
	/*[field] AC_GRP_DP_THRD*/
	#define AC_GRP_CFG_TBL_AC_GRP_DP_THRD
	#define AC_GRP_CFG_TBL_AC_GRP_DP_THRD_OFFSET  25
	#define AC_GRP_CFG_TBL_AC_GRP_DP_THRD_LEN     11
	#define AC_GRP_CFG_TBL_AC_GRP_DP_THRD_DEFAULT 0x0
	/*[field] AC_GRP_LIMIT*/
	#define AC_GRP_CFG_TBL_AC_GRP_LIMIT
	#define AC_GRP_CFG_TBL_AC_GRP_LIMIT_OFFSET  36
	#define AC_GRP_CFG_TBL_AC_GRP_LIMIT_LEN     11
	#define AC_GRP_CFG_TBL_AC_GRP_LIMIT_DEFAULT 0x0
	/*[field] AC_GRP_RED_RESUME_OFFSET*/
	#define AC_GRP_CFG_TBL_AC_GRP_RED_RESUME_OFFSET
	#define AC_GRP_CFG_TBL_AC_GRP_RED_RESUME_OFFSET_OFFSET  47
	#define AC_GRP_CFG_TBL_AC_GRP_RED_RESUME_OFFSET_LEN     11
	#define AC_GRP_CFG_TBL_AC_GRP_RED_RESUME_OFFSET_DEFAULT 0x0
	/*[field] AC_GRP_YEL_RESUME_OFFSET*/
	#define AC_GRP_CFG_TBL_AC_GRP_YEL_RESUME_OFFSET
	#define AC_GRP_CFG_TBL_AC_GRP_YEL_RESUME_OFFSET_OFFSET  58
	#define AC_GRP_CFG_TBL_AC_GRP_YEL_RESUME_OFFSET_LEN     11
	#define AC_GRP_CFG_TBL_AC_GRP_YEL_RESUME_OFFSET_DEFAULT 0x0
	/*[field] AC_GRP_GRN_RESUME_OFFSET*/
	#define AC_GRP_CFG_TBL_AC_GRP_GRN_RESUME_OFFSET
	#define AC_GRP_CFG_TBL_AC_GRP_GRN_RESUME_OFFSET_OFFSET  69
	#define AC_GRP_CFG_TBL_AC_GRP_GRN_RESUME_OFFSET_LEN     11
	#define AC_GRP_CFG_TBL_AC_GRP_GRN_RESUME_OFFSET_DEFAULT 0x0
	/*[field] AC_GRP_PALLOC_LIMIT*/
	#define AC_GRP_CFG_TBL_AC_GRP_PALLOC_LIMIT
	#define AC_GRP_CFG_TBL_AC_GRP_PALLOC_LIMIT_OFFSET  80
	#define AC_GRP_CFG_TBL_AC_GRP_PALLOC_LIMIT_LEN     11
	#define AC_GRP_CFG_TBL_AC_GRP_PALLOC_LIMIT_DEFAULT 0x0

struct ac_grp_cfg_tbl {
	a_uint32_t  ac_cfg_ac_en:1;
	a_uint32_t  ac_cfg_force_ac_en:1;
	a_uint32_t  ac_cfg_color_aware:1;
	a_uint32_t  ac_grp_gap_grn_red:11;
	a_uint32_t  ac_grp_gap_grn_yel:11;
	a_uint32_t  ac_grp_dp_thrd_0:7;
	a_uint32_t  ac_grp_dp_thrd_1:4;
	a_uint32_t  ac_grp_limit:11;
	a_uint32_t  ac_grp_red_resume_offset:11;
	a_uint32_t  ac_grp_yel_resume_offset_0:6;
	a_uint32_t  ac_grp_yel_resume_offset_1:5;
	a_uint32_t  ac_grp_grn_resume_offset:11;
	a_uint32_t  ac_grp_palloc_limit:11;
	a_uint32_t  _reserved0:5;
};

union ac_grp_cfg_tbl_u {
	a_uint32_t val[3];
	struct ac_grp_cfg_tbl bf;
};

/*[table] AC_UNI_QUEUE_CNT_TBL*/
#define AC_UNI_QUEUE_CNT_TBL
#define AC_UNI_QUEUE_CNT_TBL_ADDRESS 0x4e000
#define AC_UNI_QUEUE_CNT_TBL_NUM     256
#define AC_UNI_QUEUE_CNT_TBL_INC     0x10
#define AC_UNI_QUEUE_CNT_TBL_TYPE    REG_TYPE_RW
#define AC_UNI_QUEUE_CNT_TBL_DEFAULT 0x0
	/*[field] AC_UNI_QUEUE_CNT*/
	#define AC_UNI_QUEUE_CNT_TBL_AC_UNI_QUEUE_CNT
	#define AC_UNI_QUEUE_CNT_TBL_AC_UNI_QUEUE_CNT_OFFSET  0
	#define AC_UNI_QUEUE_CNT_TBL_AC_UNI_QUEUE_CNT_LEN     12
	#define AC_UNI_QUEUE_CNT_TBL_AC_UNI_QUEUE_CNT_DEFAULT 0x0

struct ac_uni_queue_cnt_tbl {
	a_uint32_t  ac_uni_queue_cnt:12;
	a_uint32_t  _reserved0:20;
};

union ac_uni_queue_cnt_tbl_u {
	a_uint32_t val;
	struct ac_uni_queue_cnt_tbl bf;
};

/*[table] AC_MUL_QUEUE_CNT_TBL*/
#define AC_MUL_QUEUE_CNT_TBL
#define AC_MUL_QUEUE_CNT_TBL_ADDRESS 0x52000
#define AC_MUL_QUEUE_CNT_TBL_NUM     44
#define AC_MUL_QUEUE_CNT_TBL_INC     0x10
#define AC_MUL_QUEUE_CNT_TBL_TYPE    REG_TYPE_RW
#define AC_MUL_QUEUE_CNT_TBL_DEFAULT 0x0
	/*[field] AC_MUL_QUEUE_CNT*/
	#define AC_MUL_QUEUE_CNT_TBL_AC_MUL_QUEUE_CNT
	#define AC_MUL_QUEUE_CNT_TBL_AC_MUL_QUEUE_CNT_OFFSET  0
	#define AC_MUL_QUEUE_CNT_TBL_AC_MUL_QUEUE_CNT_LEN     13
	#define AC_MUL_QUEUE_CNT_TBL_AC_MUL_QUEUE_CNT_DEFAULT 0x0

struct ac_mul_queue_cnt_tbl {
	a_uint32_t  ac_mul_queue_cnt:13;
	a_uint32_t  _reserved0:19;
};

union ac_mul_queue_cnt_tbl_u {
	a_uint32_t val;
	struct ac_mul_queue_cnt_tbl bf;
};

/*[table] AC_GRP_CNT_TBL*/
#define AC_GRP_CNT_TBL
#define AC_GRP_CNT_TBL_ADDRESS 0x54000
#define AC_GRP_CNT_TBL_NUM     4
#define AC_GRP_CNT_TBL_INC     0x10
#define AC_GRP_CNT_TBL_TYPE    REG_TYPE_RW
#define AC_GRP_CNT_TBL_DEFAULT 0x0
	/*[field] AC_GRP_CNT*/
	#define AC_GRP_CNT_TBL_AC_GRP_CNT
	#define AC_GRP_CNT_TBL_AC_GRP_CNT_OFFSET  0
	#define AC_GRP_CNT_TBL_AC_GRP_CNT_LEN     16
	#define AC_GRP_CNT_TBL_AC_GRP_CNT_DEFAULT 0x0
	/*[field] AC_GRP_ALLOC_USED*/
	#define AC_GRP_CNT_TBL_AC_GRP_ALLOC_USED
	#define AC_GRP_CNT_TBL_AC_GRP_ALLOC_USED_OFFSET  16
	#define AC_GRP_CNT_TBL_AC_GRP_ALLOC_USED_LEN     16
	#define AC_GRP_CNT_TBL_AC_GRP_ALLOC_USED_DEFAULT 0x0

struct ac_grp_cnt_tbl {
	a_uint32_t  ac_grp_cnt:16;
	a_uint32_t  ac_grp_alloc_used:16;
};

union ac_grp_cnt_tbl_u {
	a_uint32_t val;
	struct ac_grp_cnt_tbl bf;
};

/*[table] AC_UNI_QUEUE_DROP_STATE_TBL*/
#define AC_UNI_QUEUE_DROP_STATE_TBL
#define AC_UNI_QUEUE_DROP_STATE_TBL_ADDRESS 0x56000
#define AC_UNI_QUEUE_DROP_STATE_TBL_NUM     256
#define AC_UNI_QUEUE_DROP_STATE_TBL_INC     0x10
#define AC_UNI_QUEUE_DROP_STATE_TBL_TYPE    REG_TYPE_RW
#define AC_UNI_QUEUE_DROP_STATE_TBL_DEFAULT 0x0
	/*[field] RED_RESUME_THRD*/
	#define AC_UNI_QUEUE_DROP_STATE_TBL_RED_RESUME_THRD
	#define AC_UNI_QUEUE_DROP_STATE_TBL_RED_RESUME_THRD_OFFSET  0
	#define AC_UNI_QUEUE_DROP_STATE_TBL_RED_RESUME_THRD_LEN     11
	#define AC_UNI_QUEUE_DROP_STATE_TBL_RED_RESUME_THRD_DEFAULT 0x0
	/*[field] YEL_RESUME_THRD*/
	#define AC_UNI_QUEUE_DROP_STATE_TBL_YEL_RESUME_THRD
	#define AC_UNI_QUEUE_DROP_STATE_TBL_YEL_RESUME_THRD_OFFSET  11
	#define AC_UNI_QUEUE_DROP_STATE_TBL_YEL_RESUME_THRD_LEN     11
	#define AC_UNI_QUEUE_DROP_STATE_TBL_YEL_RESUME_THRD_DEFAULT 0x0
	/*[field] GRN_RESUME_THRD*/
	#define AC_UNI_QUEUE_DROP_STATE_TBL_GRN_RESUME_THRD
	#define AC_UNI_QUEUE_DROP_STATE_TBL_GRN_RESUME_THRD_OFFSET  22
	#define AC_UNI_QUEUE_DROP_STATE_TBL_GRN_RESUME_THRD_LEN     11
	#define AC_UNI_QUEUE_DROP_STATE_TBL_GRN_RESUME_THRD_DEFAULT 0x0
	/*[field] RED_DROP_STATE*/
	#define AC_UNI_QUEUE_DROP_STATE_TBL_RED_DROP_STATE
	#define AC_UNI_QUEUE_DROP_STATE_TBL_RED_DROP_STATE_OFFSET  33
	#define AC_UNI_QUEUE_DROP_STATE_TBL_RED_DROP_STATE_LEN     4
	#define AC_UNI_QUEUE_DROP_STATE_TBL_RED_DROP_STATE_DEFAULT 0x0
	/*[field] YEL_DROP_STATE*/
	#define AC_UNI_QUEUE_DROP_STATE_TBL_YEL_DROP_STATE
	#define AC_UNI_QUEUE_DROP_STATE_TBL_YEL_DROP_STATE_OFFSET  37
	#define AC_UNI_QUEUE_DROP_STATE_TBL_YEL_DROP_STATE_LEN     4
	#define AC_UNI_QUEUE_DROP_STATE_TBL_YEL_DROP_STATE_DEFAULT 0x0
	/*[field] GRN_DROP_STATE*/
	#define AC_UNI_QUEUE_DROP_STATE_TBL_GRN_DROP_STATE
	#define AC_UNI_QUEUE_DROP_STATE_TBL_GRN_DROP_STATE_OFFSET  41
	#define AC_UNI_QUEUE_DROP_STATE_TBL_GRN_DROP_STATE_LEN     4
	#define AC_UNI_QUEUE_DROP_STATE_TBL_GRN_DROP_STATE_DEFAULT 0x0

struct ac_uni_queue_drop_state_tbl {
	a_uint32_t  red_resume_thrd:11;
	a_uint32_t  yel_resume_thrd:11;
	a_uint32_t  grn_resume_thrd_0:10;
	a_uint32_t  grn_resume_thrd_1:1;
	a_uint32_t  red_drop_state:4;
	a_uint32_t  yel_drop_state:4;
	a_uint32_t  grn_drop_state:4;
	a_uint32_t  _reserved0:19;
};

union ac_uni_queue_drop_state_tbl_u {
	a_uint32_t val[2];
	struct ac_uni_queue_drop_state_tbl bf;
};

/*[table] AC_MUL_QUEUE_DROP_STATE_TBL*/
#define AC_MUL_QUEUE_DROP_STATE_TBL
#define AC_MUL_QUEUE_DROP_STATE_TBL_ADDRESS 0x58000
#define AC_MUL_QUEUE_DROP_STATE_TBL_NUM     44
#define AC_MUL_QUEUE_DROP_STATE_TBL_INC     0x10
#define AC_MUL_QUEUE_DROP_STATE_TBL_TYPE    REG_TYPE_RW
#define AC_MUL_QUEUE_DROP_STATE_TBL_DEFAULT 0x0
	/*[field] RED_RESUME_THRD*/
	#define AC_MUL_QUEUE_DROP_STATE_TBL_RED_RESUME_THRD
	#define AC_MUL_QUEUE_DROP_STATE_TBL_RED_RESUME_THRD_OFFSET  0
	#define AC_MUL_QUEUE_DROP_STATE_TBL_RED_RESUME_THRD_LEN     11
	#define AC_MUL_QUEUE_DROP_STATE_TBL_RED_RESUME_THRD_DEFAULT 0x0
	/*[field] YEL_RESUME_THRD*/
	#define AC_MUL_QUEUE_DROP_STATE_TBL_YEL_RESUME_THRD
	#define AC_MUL_QUEUE_DROP_STATE_TBL_YEL_RESUME_THRD_OFFSET  11
	#define AC_MUL_QUEUE_DROP_STATE_TBL_YEL_RESUME_THRD_LEN     11
	#define AC_MUL_QUEUE_DROP_STATE_TBL_YEL_RESUME_THRD_DEFAULT 0x0
	/*[field] GRN_RESUME_THRD*/
	#define AC_MUL_QUEUE_DROP_STATE_TBL_GRN_RESUME_THRD
	#define AC_MUL_QUEUE_DROP_STATE_TBL_GRN_RESUME_THRD_OFFSET  22
	#define AC_MUL_QUEUE_DROP_STATE_TBL_GRN_RESUME_THRD_LEN     11
	#define AC_MUL_QUEUE_DROP_STATE_TBL_GRN_RESUME_THRD_DEFAULT 0x0
	/*[field] RED_DROP_STATE*/
	#define AC_MUL_QUEUE_DROP_STATE_TBL_RED_DROP_STATE
	#define AC_MUL_QUEUE_DROP_STATE_TBL_RED_DROP_STATE_OFFSET  33
	#define AC_MUL_QUEUE_DROP_STATE_TBL_RED_DROP_STATE_LEN     1
	#define AC_MUL_QUEUE_DROP_STATE_TBL_RED_DROP_STATE_DEFAULT 0x0
	/*[field] YEL_DROP_STATE*/
	#define AC_MUL_QUEUE_DROP_STATE_TBL_YEL_DROP_STATE
	#define AC_MUL_QUEUE_DROP_STATE_TBL_YEL_DROP_STATE_OFFSET  34
	#define AC_MUL_QUEUE_DROP_STATE_TBL_YEL_DROP_STATE_LEN     1
	#define AC_MUL_QUEUE_DROP_STATE_TBL_YEL_DROP_STATE_DEFAULT 0x0
	/*[field] GRN_DROP_STATE*/
	#define AC_MUL_QUEUE_DROP_STATE_TBL_GRN_DROP_STATE
	#define AC_MUL_QUEUE_DROP_STATE_TBL_GRN_DROP_STATE_OFFSET  35
	#define AC_MUL_QUEUE_DROP_STATE_TBL_GRN_DROP_STATE_LEN     1
	#define AC_MUL_QUEUE_DROP_STATE_TBL_GRN_DROP_STATE_DEFAULT 0x0

struct ac_mul_queue_drop_state_tbl {
	a_uint32_t  red_resume_thrd:11;
	a_uint32_t  yel_resume_thrd:11;
	a_uint32_t  grn_resume_thrd_0:10;
	a_uint32_t  grn_resume_thrd_1:1;
	a_uint32_t  red_drop_state:1;
	a_uint32_t  yel_drop_state:1;
	a_uint32_t  grn_drop_state:1;
	a_uint32_t  _reserved0:28;
};

union ac_mul_queue_drop_state_tbl_u {
	a_uint32_t val[2];
	struct ac_mul_queue_drop_state_tbl bf;
};

/*[table] AC_GRP_DROP_STATE_TBL*/
#define AC_GRP_DROP_STATE_TBL
#define AC_GRP_DROP_STATE_TBL_ADDRESS 0x5a000
#define AC_GRP_DROP_STATE_TBL_NUM     4
#define AC_GRP_DROP_STATE_TBL_INC     0x10
#define AC_GRP_DROP_STATE_TBL_TYPE    REG_TYPE_RW
#define AC_GRP_DROP_STATE_TBL_DEFAULT 0x0
	/*[field] RED_RESUME_THRD*/
	#define AC_GRP_DROP_STATE_TBL_RED_RESUME_THRD
	#define AC_GRP_DROP_STATE_TBL_RED_RESUME_THRD_OFFSET  0
	#define AC_GRP_DROP_STATE_TBL_RED_RESUME_THRD_LEN     11
	#define AC_GRP_DROP_STATE_TBL_RED_RESUME_THRD_DEFAULT 0x0
	/*[field] YEL_RESUME_THRD*/
	#define AC_GRP_DROP_STATE_TBL_YEL_RESUME_THRD
	#define AC_GRP_DROP_STATE_TBL_YEL_RESUME_THRD_OFFSET  11
	#define AC_GRP_DROP_STATE_TBL_YEL_RESUME_THRD_LEN     11
	#define AC_GRP_DROP_STATE_TBL_YEL_RESUME_THRD_DEFAULT 0x0
	/*[field] GRN_RESUME_THRD*/
	#define AC_GRP_DROP_STATE_TBL_GRN_RESUME_THRD
	#define AC_GRP_DROP_STATE_TBL_GRN_RESUME_THRD_OFFSET  22
	#define AC_GRP_DROP_STATE_TBL_GRN_RESUME_THRD_LEN     11
	#define AC_GRP_DROP_STATE_TBL_GRN_RESUME_THRD_DEFAULT 0x0
	/*[field] RED_DROP_STATE*/
	#define AC_GRP_DROP_STATE_TBL_RED_DROP_STATE
	#define AC_GRP_DROP_STATE_TBL_RED_DROP_STATE_OFFSET  33
	#define AC_GRP_DROP_STATE_TBL_RED_DROP_STATE_LEN     1
	#define AC_GRP_DROP_STATE_TBL_RED_DROP_STATE_DEFAULT 0x0
	/*[field] YEL_DROP_STATE*/
	#define AC_GRP_DROP_STATE_TBL_YEL_DROP_STATE
	#define AC_GRP_DROP_STATE_TBL_YEL_DROP_STATE_OFFSET  34
	#define AC_GRP_DROP_STATE_TBL_YEL_DROP_STATE_LEN     1
	#define AC_GRP_DROP_STATE_TBL_YEL_DROP_STATE_DEFAULT 0x0
	/*[field] GRN_DROP_STATE*/
	#define AC_GRP_DROP_STATE_TBL_GRN_DROP_STATE
	#define AC_GRP_DROP_STATE_TBL_GRN_DROP_STATE_OFFSET  35
	#define AC_GRP_DROP_STATE_TBL_GRN_DROP_STATE_LEN     1
	#define AC_GRP_DROP_STATE_TBL_GRN_DROP_STATE_DEFAULT 0x0

struct ac_grp_drop_state_tbl {
	a_uint32_t  red_resume_thrd:11;
	a_uint32_t  yel_resume_thrd:11;
	a_uint32_t  grn_resume_thrd_0:10;
	a_uint32_t  grn_resume_thrd_1:1;
	a_uint32_t  red_drop_state:1;
	a_uint32_t  yel_drop_state:1;
	a_uint32_t  grn_drop_state:1;
	a_uint32_t  _reserved0:28;
};

union ac_grp_drop_state_tbl_u {
	a_uint32_t val[2];
	struct ac_grp_drop_state_tbl bf;
};

/*[table] OQ_ENQ_OPR_TBL*/
#define OQ_ENQ_OPR_TBL
#define OQ_ENQ_OPR_TBL_ADDRESS 0x5c000
#define OQ_ENQ_OPR_TBL_NUM     300
#define OQ_ENQ_OPR_TBL_INC     0x10
#define OQ_ENQ_OPR_TBL_TYPE    REG_TYPE_RW
#define OQ_ENQ_OPR_TBL_DEFAULT 0x0
	/*[field] ENQ_DISABLE*/
	#define OQ_ENQ_OPR_TBL_ENQ_DISABLE
	#define OQ_ENQ_OPR_TBL_ENQ_DISABLE_OFFSET  0
	#define OQ_ENQ_OPR_TBL_ENQ_DISABLE_LEN     1
	#define OQ_ENQ_OPR_TBL_ENQ_DISABLE_DEFAULT 0x0

struct oq_enq_opr_tbl {
	a_uint32_t  enq_disable:1;
	a_uint32_t  _reserved0:31;
};

union oq_enq_opr_tbl_u {
	a_uint32_t val;
	struct oq_enq_opr_tbl bf;
};

/*[table] OQ_DEQ_OPR_TBL*/
#define OQ_DEQ_OPR_TBL
#define OQ_DEQ_OPR_TBL_ADDRESS 0x64000
#define OQ_DEQ_OPR_TBL_NUM     300
#define OQ_DEQ_OPR_TBL_INC     0x10
#define OQ_DEQ_OPR_TBL_TYPE    REG_TYPE_RW
#define OQ_DEQ_OPR_TBL_DEFAULT 0x0
	/*[field] DEQ_DROP*/
	#define OQ_DEQ_OPR_TBL_DEQ_DROP
	#define OQ_DEQ_OPR_TBL_DEQ_DROP_OFFSET  0
	#define OQ_DEQ_OPR_TBL_DEQ_DROP_LEN     1
	#define OQ_DEQ_OPR_TBL_DEQ_DROP_DEFAULT 0x0

struct oq_deq_opr_tbl {
	a_uint32_t  deq_drop:1;
	a_uint32_t  _reserved0:31;
};

union oq_deq_opr_tbl_u {
	a_uint32_t val;
	struct oq_deq_opr_tbl bf;
};

/*[table] OQ_HEAD_UNI_TBL*/
#define OQ_HEAD_UNI_TBL
#define OQ_HEAD_UNI_TBL_ADDRESS 0x6c000
#define OQ_HEAD_UNI_TBL_NUM     256
#define OQ_HEAD_UNI_TBL_INC     0x10
#define OQ_HEAD_UNI_TBL_TYPE    REG_TYPE_RW
#define OQ_HEAD_UNI_TBL_DEFAULT 0x0
	/*[field] EMPTY*/
	#define OQ_HEAD_UNI_TBL_EMPTY
	#define OQ_HEAD_UNI_TBL_EMPTY_OFFSET  0
	#define OQ_HEAD_UNI_TBL_EMPTY_LEN     1
	#define OQ_HEAD_UNI_TBL_EMPTY_DEFAULT 0x0
	/*[field] TAIL*/
	#define OQ_HEAD_UNI_TBL_TAIL
	#define OQ_HEAD_UNI_TBL_TAIL_OFFSET  1
	#define OQ_HEAD_UNI_TBL_TAIL_LEN     11
	#define OQ_HEAD_UNI_TBL_TAIL_DEFAULT 0x0
	/*[field] HEAD*/
	#define OQ_HEAD_UNI_TBL_HEAD
	#define OQ_HEAD_UNI_TBL_HEAD_OFFSET  12
	#define OQ_HEAD_UNI_TBL_HEAD_LEN     11
	#define OQ_HEAD_UNI_TBL_HEAD_DEFAULT 0x0

struct oq_head_uni_tbl {
	a_uint32_t  empty:1;
	a_uint32_t  tail:11;
	a_uint32_t  head:11;
	a_uint32_t  _reserved0:9;
};

union oq_head_uni_tbl_u {
	a_uint32_t val;
	struct oq_head_uni_tbl bf;
};

/*[table] OQ_HEAD_MUL_TBL*/
#define OQ_HEAD_MUL_TBL
#define OQ_HEAD_MUL_TBL_ADDRESS 0x74000
#define OQ_HEAD_MUL_TBL_NUM     44
#define OQ_HEAD_MUL_TBL_INC     0x10
#define OQ_HEAD_MUL_TBL_TYPE    REG_TYPE_RW
#define OQ_HEAD_MUL_TBL_DEFAULT 0x0
	/*[field] NORMAL_FWD*/
	#define OQ_HEAD_MUL_TBL_NORMAL_FWD
	#define OQ_HEAD_MUL_TBL_NORMAL_FWD_OFFSET  0
	#define OQ_HEAD_MUL_TBL_NORMAL_FWD_LEN     1
	#define OQ_HEAD_MUL_TBL_NORMAL_FWD_DEFAULT 0x0
	/*[field] EGRESS_MIRR*/
	#define OQ_HEAD_MUL_TBL_EGRESS_MIRR
	#define OQ_HEAD_MUL_TBL_EGRESS_MIRR_OFFSET  1
	#define OQ_HEAD_MUL_TBL_EGRESS_MIRR_LEN     1
	#define OQ_HEAD_MUL_TBL_EGRESS_MIRR_DEFAULT 0x0
	/*[field] INGRESS_MIRR*/
	#define OQ_HEAD_MUL_TBL_INGRESS_MIRR
	#define OQ_HEAD_MUL_TBL_INGRESS_MIRR_OFFSET  2
	#define OQ_HEAD_MUL_TBL_INGRESS_MIRR_LEN     1
	#define OQ_HEAD_MUL_TBL_INGRESS_MIRR_DEFAULT 0x0
	/*[field] EMPTY*/
	#define OQ_HEAD_MUL_TBL_EMPTY
	#define OQ_HEAD_MUL_TBL_EMPTY_OFFSET  3
	#define OQ_HEAD_MUL_TBL_EMPTY_LEN     1
	#define OQ_HEAD_MUL_TBL_EMPTY_DEFAULT 0x0
	/*[field] TAIL*/
	#define OQ_HEAD_MUL_TBL_TAIL
	#define OQ_HEAD_MUL_TBL_TAIL_OFFSET  4
	#define OQ_HEAD_MUL_TBL_TAIL_LEN     11
	#define OQ_HEAD_MUL_TBL_TAIL_DEFAULT 0x0
	/*[field] HEAD*/
	#define OQ_HEAD_MUL_TBL_HEAD
	#define OQ_HEAD_MUL_TBL_HEAD_OFFSET  15
	#define OQ_HEAD_MUL_TBL_HEAD_LEN     11
	#define OQ_HEAD_MUL_TBL_HEAD_DEFAULT 0x0

struct oq_head_mul_tbl {
	a_uint32_t  normal_fwd:1;
	a_uint32_t  egress_mirr:1;
	a_uint32_t  ingress_mirr:1;
	a_uint32_t  empty:1;
	a_uint32_t  tail:11;
	a_uint32_t  head:11;
	a_uint32_t  _reserved0:6;
};

union oq_head_mul_tbl_u {
	a_uint32_t val;
	struct oq_head_mul_tbl bf;
};

/*[table] OQ_LL_UNI_TBL*/
#define OQ_LL_UNI_TBL
#define OQ_LL_UNI_TBL_ADDRESS 0x90000
#define OQ_LL_UNI_TBL_NUM     2048
#define OQ_LL_UNI_TBL_INC     0x10
#define OQ_LL_UNI_TBL_TYPE    REG_TYPE_RW
#define OQ_LL_UNI_TBL_DEFAULT 0x0
	/*[field] NEXT_POINTER*/
	#define OQ_LL_UNI_TBL_NEXT_POINTER
	#define OQ_LL_UNI_TBL_NEXT_POINTER_OFFSET  0
	#define OQ_LL_UNI_TBL_NEXT_POINTER_LEN     11
	#define OQ_LL_UNI_TBL_NEXT_POINTER_DEFAULT 0x0

struct oq_ll_uni_tbl {
	a_uint32_t  next_pointer:11;
	a_uint32_t  _reserved0:21;
};

union oq_ll_uni_tbl_u {
	a_uint32_t val;
	struct oq_ll_uni_tbl bf;
};

/*[table] OQ_LL_MUL_P0_TBL*/
#define OQ_LL_MUL_P0_TBL
#define OQ_LL_MUL_P0_TBL_ADDRESS 0xb0000
#define OQ_LL_MUL_P0_TBL_NUM     2048
#define OQ_LL_MUL_P0_TBL_INC     0x10
#define OQ_LL_MUL_P0_TBL_TYPE    REG_TYPE_RW
#define OQ_LL_MUL_P0_TBL_DEFAULT 0x0
	/*[field] NORMAL_FWD*/
	#define OQ_LL_MUL_P0_TBL_NORMAL_FWD
	#define OQ_LL_MUL_P0_TBL_NORMAL_FWD_OFFSET  0
	#define OQ_LL_MUL_P0_TBL_NORMAL_FWD_LEN     1
	#define OQ_LL_MUL_P0_TBL_NORMAL_FWD_DEFAULT 0x0
	/*[field] EGRESS_MIRR*/
	#define OQ_LL_MUL_P0_TBL_EGRESS_MIRR
	#define OQ_LL_MUL_P0_TBL_EGRESS_MIRR_OFFSET  1
	#define OQ_LL_MUL_P0_TBL_EGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P0_TBL_EGRESS_MIRR_DEFAULT 0x0
	/*[field] INGRESS_MIRR*/
	#define OQ_LL_MUL_P0_TBL_INGRESS_MIRR
	#define OQ_LL_MUL_P0_TBL_INGRESS_MIRR_OFFSET  2
	#define OQ_LL_MUL_P0_TBL_INGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P0_TBL_INGRESS_MIRR_DEFAULT 0x0
	/*[field] NEXT_POINTER*/
	#define OQ_LL_MUL_P0_TBL_NEXT_POINTER
	#define OQ_LL_MUL_P0_TBL_NEXT_POINTER_OFFSET  3
	#define OQ_LL_MUL_P0_TBL_NEXT_POINTER_LEN     11
	#define OQ_LL_MUL_P0_TBL_NEXT_POINTER_DEFAULT 0x0

struct oq_ll_mul_p0_tbl {
	a_uint32_t  normal_fwd:1;
	a_uint32_t  egress_mirr:1;
	a_uint32_t  ingress_mirr:1;
	a_uint32_t  next_pointer:11;
	a_uint32_t  _reserved0:18;
};

union oq_ll_mul_p0_tbl_u {
	a_uint32_t val;
	struct oq_ll_mul_p0_tbl bf;
};

/*[table] OQ_LL_MUL_P1_TBL*/
#define OQ_LL_MUL_P1_TBL
#define OQ_LL_MUL_P1_TBL_ADDRESS 0xd0000
#define OQ_LL_MUL_P1_TBL_NUM     2048
#define OQ_LL_MUL_P1_TBL_INC     0x10
#define OQ_LL_MUL_P1_TBL_TYPE    REG_TYPE_RW
#define OQ_LL_MUL_P1_TBL_DEFAULT 0x0
	/*[field] NORMAL_FWD*/
	#define OQ_LL_MUL_P1_TBL_NORMAL_FWD
	#define OQ_LL_MUL_P1_TBL_NORMAL_FWD_OFFSET  0
	#define OQ_LL_MUL_P1_TBL_NORMAL_FWD_LEN     1
	#define OQ_LL_MUL_P1_TBL_NORMAL_FWD_DEFAULT 0x0
	/*[field] EGRESS_MIRR*/
	#define OQ_LL_MUL_P1_TBL_EGRESS_MIRR
	#define OQ_LL_MUL_P1_TBL_EGRESS_MIRR_OFFSET  1
	#define OQ_LL_MUL_P1_TBL_EGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P1_TBL_EGRESS_MIRR_DEFAULT 0x0
	/*[field] INGRESS_MIRR*/
	#define OQ_LL_MUL_P1_TBL_INGRESS_MIRR
	#define OQ_LL_MUL_P1_TBL_INGRESS_MIRR_OFFSET  2
	#define OQ_LL_MUL_P1_TBL_INGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P1_TBL_INGRESS_MIRR_DEFAULT 0x0
	/*[field] NEXT_POINTER*/
	#define OQ_LL_MUL_P1_TBL_NEXT_POINTER
	#define OQ_LL_MUL_P1_TBL_NEXT_POINTER_OFFSET  3
	#define OQ_LL_MUL_P1_TBL_NEXT_POINTER_LEN     11
	#define OQ_LL_MUL_P1_TBL_NEXT_POINTER_DEFAULT 0x0

struct oq_ll_mul_p1_tbl {
	a_uint32_t  normal_fwd:1;
	a_uint32_t  egress_mirr:1;
	a_uint32_t  ingress_mirr:1;
	a_uint32_t  next_pointer:11;
	a_uint32_t  _reserved0:18;
};

union oq_ll_mul_p1_tbl_u {
	a_uint32_t val;
	struct oq_ll_mul_p1_tbl bf;
};

/*[table] OQ_LL_MUL_P2_TBL*/
#define OQ_LL_MUL_P2_TBL
#define OQ_LL_MUL_P2_TBL_ADDRESS 0x110000
#define OQ_LL_MUL_P2_TBL_NUM     2048
#define OQ_LL_MUL_P2_TBL_INC     0x10
#define OQ_LL_MUL_P2_TBL_TYPE    REG_TYPE_RW
#define OQ_LL_MUL_P2_TBL_DEFAULT 0x0
	/*[field] NORMAL_FWD*/
	#define OQ_LL_MUL_P2_TBL_NORMAL_FWD
	#define OQ_LL_MUL_P2_TBL_NORMAL_FWD_OFFSET  0
	#define OQ_LL_MUL_P2_TBL_NORMAL_FWD_LEN     1
	#define OQ_LL_MUL_P2_TBL_NORMAL_FWD_DEFAULT 0x0
	/*[field] EGRESS_MIRR*/
	#define OQ_LL_MUL_P2_TBL_EGRESS_MIRR
	#define OQ_LL_MUL_P2_TBL_EGRESS_MIRR_OFFSET  1
	#define OQ_LL_MUL_P2_TBL_EGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P2_TBL_EGRESS_MIRR_DEFAULT 0x0
	/*[field] INGRESS_MIRR*/
	#define OQ_LL_MUL_P2_TBL_INGRESS_MIRR
	#define OQ_LL_MUL_P2_TBL_INGRESS_MIRR_OFFSET  2
	#define OQ_LL_MUL_P2_TBL_INGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P2_TBL_INGRESS_MIRR_DEFAULT 0x0
	/*[field] NEXT_POINTER*/
	#define OQ_LL_MUL_P2_TBL_NEXT_POINTER
	#define OQ_LL_MUL_P2_TBL_NEXT_POINTER_OFFSET  3
	#define OQ_LL_MUL_P2_TBL_NEXT_POINTER_LEN     11
	#define OQ_LL_MUL_P2_TBL_NEXT_POINTER_DEFAULT 0x0

struct oq_ll_mul_p2_tbl {
	a_uint32_t  normal_fwd:1;
	a_uint32_t  egress_mirr:1;
	a_uint32_t  ingress_mirr:1;
	a_uint32_t  next_pointer:11;
	a_uint32_t  _reserved0:18;
};

union oq_ll_mul_p2_tbl_u {
	a_uint32_t val;
	struct oq_ll_mul_p2_tbl bf;
};

/*[table] OQ_LL_MUL_P3_TBL*/
#define OQ_LL_MUL_P3_TBL
#define OQ_LL_MUL_P3_TBL_ADDRESS 0x130000
#define OQ_LL_MUL_P3_TBL_NUM     2048
#define OQ_LL_MUL_P3_TBL_INC     0x10
#define OQ_LL_MUL_P3_TBL_TYPE    REG_TYPE_RW
#define OQ_LL_MUL_P3_TBL_DEFAULT 0x0
	/*[field] NORMAL_FWD*/
	#define OQ_LL_MUL_P3_TBL_NORMAL_FWD
	#define OQ_LL_MUL_P3_TBL_NORMAL_FWD_OFFSET  0
	#define OQ_LL_MUL_P3_TBL_NORMAL_FWD_LEN     1
	#define OQ_LL_MUL_P3_TBL_NORMAL_FWD_DEFAULT 0x0
	/*[field] EGRESS_MIRR*/
	#define OQ_LL_MUL_P3_TBL_EGRESS_MIRR
	#define OQ_LL_MUL_P3_TBL_EGRESS_MIRR_OFFSET  1
	#define OQ_LL_MUL_P3_TBL_EGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P3_TBL_EGRESS_MIRR_DEFAULT 0x0
	/*[field] INGRESS_MIRR*/
	#define OQ_LL_MUL_P3_TBL_INGRESS_MIRR
	#define OQ_LL_MUL_P3_TBL_INGRESS_MIRR_OFFSET  2
	#define OQ_LL_MUL_P3_TBL_INGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P3_TBL_INGRESS_MIRR_DEFAULT 0x0
	/*[field] NEXT_POINTER*/
	#define OQ_LL_MUL_P3_TBL_NEXT_POINTER
	#define OQ_LL_MUL_P3_TBL_NEXT_POINTER_OFFSET  3
	#define OQ_LL_MUL_P3_TBL_NEXT_POINTER_LEN     11
	#define OQ_LL_MUL_P3_TBL_NEXT_POINTER_DEFAULT 0x0

struct oq_ll_mul_p3_tbl {
	a_uint32_t  normal_fwd:1;
	a_uint32_t  egress_mirr:1;
	a_uint32_t  ingress_mirr:1;
	a_uint32_t  next_pointer:11;
	a_uint32_t  _reserved0:18;
};

union oq_ll_mul_p3_tbl_u {
	a_uint32_t val;
	struct oq_ll_mul_p3_tbl bf;
};

/*[table] OQ_LL_MUL_P4_TBL*/
#define OQ_LL_MUL_P4_TBL
#define OQ_LL_MUL_P4_TBL_ADDRESS 0x150000
#define OQ_LL_MUL_P4_TBL_NUM     2048
#define OQ_LL_MUL_P4_TBL_INC     0x10
#define OQ_LL_MUL_P4_TBL_TYPE    REG_TYPE_RW
#define OQ_LL_MUL_P4_TBL_DEFAULT 0x0
	/*[field] NORMAL_FWD*/
	#define OQ_LL_MUL_P4_TBL_NORMAL_FWD
	#define OQ_LL_MUL_P4_TBL_NORMAL_FWD_OFFSET  0
	#define OQ_LL_MUL_P4_TBL_NORMAL_FWD_LEN     1
	#define OQ_LL_MUL_P4_TBL_NORMAL_FWD_DEFAULT 0x0
	/*[field] EGRESS_MIRR*/
	#define OQ_LL_MUL_P4_TBL_EGRESS_MIRR
	#define OQ_LL_MUL_P4_TBL_EGRESS_MIRR_OFFSET  1
	#define OQ_LL_MUL_P4_TBL_EGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P4_TBL_EGRESS_MIRR_DEFAULT 0x0
	/*[field] INGRESS_MIRR*/
	#define OQ_LL_MUL_P4_TBL_INGRESS_MIRR
	#define OQ_LL_MUL_P4_TBL_INGRESS_MIRR_OFFSET  2
	#define OQ_LL_MUL_P4_TBL_INGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P4_TBL_INGRESS_MIRR_DEFAULT 0x0
	/*[field] NEXT_POINTER*/
	#define OQ_LL_MUL_P4_TBL_NEXT_POINTER
	#define OQ_LL_MUL_P4_TBL_NEXT_POINTER_OFFSET  3
	#define OQ_LL_MUL_P4_TBL_NEXT_POINTER_LEN     11
	#define OQ_LL_MUL_P4_TBL_NEXT_POINTER_DEFAULT 0x0

struct oq_ll_mul_p4_tbl {
	a_uint32_t  normal_fwd:1;
	a_uint32_t  egress_mirr:1;
	a_uint32_t  ingress_mirr:1;
	a_uint32_t  next_pointer:11;
	a_uint32_t  _reserved0:18;
};

union oq_ll_mul_p4_tbl_u {
	a_uint32_t val;
	struct oq_ll_mul_p4_tbl bf;
};

/*[table] OQ_LL_MUL_P5_TBL*/
#define OQ_LL_MUL_P5_TBL
#define OQ_LL_MUL_P5_TBL_ADDRESS 0x170000
#define OQ_LL_MUL_P5_TBL_NUM     2048
#define OQ_LL_MUL_P5_TBL_INC     0x10
#define OQ_LL_MUL_P5_TBL_TYPE    REG_TYPE_RW
#define OQ_LL_MUL_P5_TBL_DEFAULT 0x0
	/*[field] NORMAL_FWD*/
	#define OQ_LL_MUL_P5_TBL_NORMAL_FWD
	#define OQ_LL_MUL_P5_TBL_NORMAL_FWD_OFFSET  0
	#define OQ_LL_MUL_P5_TBL_NORMAL_FWD_LEN     1
	#define OQ_LL_MUL_P5_TBL_NORMAL_FWD_DEFAULT 0x0
	/*[field] EGRESS_MIRR*/
	#define OQ_LL_MUL_P5_TBL_EGRESS_MIRR
	#define OQ_LL_MUL_P5_TBL_EGRESS_MIRR_OFFSET  1
	#define OQ_LL_MUL_P5_TBL_EGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P5_TBL_EGRESS_MIRR_DEFAULT 0x0
	/*[field] INGRESS_MIRR*/
	#define OQ_LL_MUL_P5_TBL_INGRESS_MIRR
	#define OQ_LL_MUL_P5_TBL_INGRESS_MIRR_OFFSET  2
	#define OQ_LL_MUL_P5_TBL_INGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P5_TBL_INGRESS_MIRR_DEFAULT 0x0
	/*[field] NEXT_POINTER*/
	#define OQ_LL_MUL_P5_TBL_NEXT_POINTER
	#define OQ_LL_MUL_P5_TBL_NEXT_POINTER_OFFSET  3
	#define OQ_LL_MUL_P5_TBL_NEXT_POINTER_LEN     11
	#define OQ_LL_MUL_P5_TBL_NEXT_POINTER_DEFAULT 0x0

struct oq_ll_mul_p5_tbl {
	a_uint32_t  normal_fwd:1;
	a_uint32_t  egress_mirr:1;
	a_uint32_t  ingress_mirr:1;
	a_uint32_t  next_pointer:11;
	a_uint32_t  _reserved0:18;
};

union oq_ll_mul_p5_tbl_u {
	a_uint32_t val;
	struct oq_ll_mul_p5_tbl bf;
};

/*[table] OQ_LL_MUL_P6_TBL*/
#define OQ_LL_MUL_P6_TBL
#define OQ_LL_MUL_P6_TBL_ADDRESS 0x190000
#define OQ_LL_MUL_P6_TBL_NUM     2048
#define OQ_LL_MUL_P6_TBL_INC     0x10
#define OQ_LL_MUL_P6_TBL_TYPE    REG_TYPE_RW
#define OQ_LL_MUL_P6_TBL_DEFAULT 0x0
	/*[field] NORMAL_FWD*/
	#define OQ_LL_MUL_P6_TBL_NORMAL_FWD
	#define OQ_LL_MUL_P6_TBL_NORMAL_FWD_OFFSET  0
	#define OQ_LL_MUL_P6_TBL_NORMAL_FWD_LEN     1
	#define OQ_LL_MUL_P6_TBL_NORMAL_FWD_DEFAULT 0x0
	/*[field] EGRESS_MIRR*/
	#define OQ_LL_MUL_P6_TBL_EGRESS_MIRR
	#define OQ_LL_MUL_P6_TBL_EGRESS_MIRR_OFFSET  1
	#define OQ_LL_MUL_P6_TBL_EGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P6_TBL_EGRESS_MIRR_DEFAULT 0x0
	/*[field] INGRESS_MIRR*/
	#define OQ_LL_MUL_P6_TBL_INGRESS_MIRR
	#define OQ_LL_MUL_P6_TBL_INGRESS_MIRR_OFFSET  2
	#define OQ_LL_MUL_P6_TBL_INGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P6_TBL_INGRESS_MIRR_DEFAULT 0x0
	/*[field] NEXT_POINTER*/
	#define OQ_LL_MUL_P6_TBL_NEXT_POINTER
	#define OQ_LL_MUL_P6_TBL_NEXT_POINTER_OFFSET  3
	#define OQ_LL_MUL_P6_TBL_NEXT_POINTER_LEN     11
	#define OQ_LL_MUL_P6_TBL_NEXT_POINTER_DEFAULT 0x0

struct oq_ll_mul_p6_tbl {
	a_uint32_t  normal_fwd:1;
	a_uint32_t  egress_mirr:1;
	a_uint32_t  ingress_mirr:1;
	a_uint32_t  next_pointer:11;
	a_uint32_t  _reserved0:18;
};

union oq_ll_mul_p6_tbl_u {
	a_uint32_t val;
	struct oq_ll_mul_p6_tbl bf;
};

/*[table] OQ_LL_MUL_P7_TBL*/
#define OQ_LL_MUL_P7_TBL
#define OQ_LL_MUL_P7_TBL_ADDRESS 0x1b0000
#define OQ_LL_MUL_P7_TBL_NUM     2048
#define OQ_LL_MUL_P7_TBL_INC     0x10
#define OQ_LL_MUL_P7_TBL_TYPE    REG_TYPE_RW
#define OQ_LL_MUL_P7_TBL_DEFAULT 0x0
	/*[field] NORMAL_FWD*/
	#define OQ_LL_MUL_P7_TBL_NORMAL_FWD
	#define OQ_LL_MUL_P7_TBL_NORMAL_FWD_OFFSET  0
	#define OQ_LL_MUL_P7_TBL_NORMAL_FWD_LEN     1
	#define OQ_LL_MUL_P7_TBL_NORMAL_FWD_DEFAULT 0x0
	/*[field] EGRESS_MIRR*/
	#define OQ_LL_MUL_P7_TBL_EGRESS_MIRR
	#define OQ_LL_MUL_P7_TBL_EGRESS_MIRR_OFFSET  1
	#define OQ_LL_MUL_P7_TBL_EGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P7_TBL_EGRESS_MIRR_DEFAULT 0x0
	/*[field] INGRESS_MIRR*/
	#define OQ_LL_MUL_P7_TBL_INGRESS_MIRR
	#define OQ_LL_MUL_P7_TBL_INGRESS_MIRR_OFFSET  2
	#define OQ_LL_MUL_P7_TBL_INGRESS_MIRR_LEN     1
	#define OQ_LL_MUL_P7_TBL_INGRESS_MIRR_DEFAULT 0x0
	/*[field] NEXT_POINTER*/
	#define OQ_LL_MUL_P7_TBL_NEXT_POINTER
	#define OQ_LL_MUL_P7_TBL_NEXT_POINTER_OFFSET  3
	#define OQ_LL_MUL_P7_TBL_NEXT_POINTER_LEN     11
	#define OQ_LL_MUL_P7_TBL_NEXT_POINTER_DEFAULT 0x0

struct oq_ll_mul_p7_tbl {
	a_uint32_t  normal_fwd:1;
	a_uint32_t  egress_mirr:1;
	a_uint32_t  ingress_mirr:1;
	a_uint32_t  next_pointer:11;
	a_uint32_t  _reserved0:18;
};

union oq_ll_mul_p7_tbl_u {
	a_uint32_t val;
	struct oq_ll_mul_p7_tbl bf;
};

/*[table] PKT_DESP_TBL*/
#define PKT_DESP_TBL
#define PKT_DESP_TBL_ADDRESS 0x1c0000
#define PKT_DESP_TBL_NUM     2048
#define PKT_DESP_TBL_INC     0x40
#define PKT_DESP_TBL_TYPE    REG_TYPE_RW
#define PKT_DESP_TBL_DEFAULT 0x0
	/*[field] ORG_SRC_PORT_VP*/
	#define PKT_DESP_TBL_ORG_SRC_PORT_VP
	#define PKT_DESP_TBL_ORG_SRC_PORT_VP_OFFSET  0
	#define PKT_DESP_TBL_ORG_SRC_PORT_VP_LEN     12
	#define PKT_DESP_TBL_ORG_SRC_PORT_VP_DEFAULT 0x0
	/*[field] SRC_PORT_VP*/
	#define PKT_DESP_TBL_SRC_PORT_VP
	#define PKT_DESP_TBL_SRC_PORT_VP_OFFSET  12
	#define PKT_DESP_TBL_SRC_PORT_VP_LEN     12
	#define PKT_DESP_TBL_SRC_PORT_VP_DEFAULT 0x0
	/*[field] PACKET_LENGTH*/
	#define PKT_DESP_TBL_PACKET_LENGTH
	#define PKT_DESP_TBL_PACKET_LENGTH_OFFSET  24
	#define PKT_DESP_TBL_PACKET_LENGTH_LEN     14
	#define PKT_DESP_TBL_PACKET_LENGTH_DEFAULT 0x0
	/*[field] TS_DIR*/
	#define PKT_DESP_TBL_TS_DIR
	#define PKT_DESP_TBL_TS_DIR_OFFSET  38
	#define PKT_DESP_TBL_TS_DIR_LEN     1
	#define PKT_DESP_TBL_TS_DIR_DEFAULT 0x0
	/*[field] TX_TS_EN reuse TS_DIR[0]*/
	#define PKT_DESP_TBL_TX_TS_EN
	#define PKT_DESP_TBL_TX_TS_EN_OFFSET  39
	#define PKT_DESP_TBL_TX_TS_EN_LEN     1
	#define PKT_DESP_TBL_TX_TS_EN_DEFAULT 0x0
	/*[field] RX_TS_VALID reuse TS_DIR[1]*/
	#define PKT_DESP_TBL_RX_TS_VALID
	#define PKT_DESP_TBL_RX_TS_VALID_OFFSET  39
	#define PKT_DESP_TBL_RX_TS_VALID_LEN     1
	#define PKT_DESP_TBL_RX_TS_VALID_DEFAULT 0x0
	/*[field] RX_TS reuse TS_DIR[1]*/
	#define PKT_DESP_TBL_RX_TS
	#define PKT_DESP_TBL_RX_TS_OFFSET  40
	#define PKT_DESP_TBL_RX_TS_LEN     40
	#define PKT_DESP_TBL_RX_TS_DEFAULT 0x0
	/*[field] TX_OS_CORRECTION_EN reuse TS_DIR[0]*/
	#define PKT_DESP_TBL_TX_OS_CORRECTION_EN
	#define PKT_DESP_TBL_TX_OS_CORRECTION_EN_OFFSET  40
	#define PKT_DESP_TBL_TX_OS_CORRECTION_EN_LEN     1
	#define PKT_DESP_TBL_TX_OS_CORRECTION_EN_DEFAULT 0x0
	/*[field] TX_PTP_TAG reuse TS_DIR[0]*/
	#define PKT_DESP_TBL_TX_PTP_TAG
	#define PKT_DESP_TBL_TX_PTP_TAG_OFFSET  41
	#define PKT_DESP_TBL_TX_PTP_TAG_LEN     10
	#define PKT_DESP_TBL_TX_PTP_TAG_DEFAULT 0x0
	/*[field] INT_PRI*/
	#define PKT_DESP_TBL_INT_PRI
	#define PKT_DESP_TBL_INT_PRI_OFFSET  80
	#define PKT_DESP_TBL_INT_PRI_LEN     4
	#define PKT_DESP_TBL_INT_PRI_DEFAULT 0x0
	/*[field] INT_DP*/
	#define PKT_DESP_TBL_INT_DP
	#define PKT_DESP_TBL_INT_DP_OFFSET  84
	#define PKT_DESP_TBL_INT_DP_LEN     2
	#define PKT_DESP_TBL_INT_DP_DEFAULT 0x0
	/*[field] CPU_CODE*/
	#define PKT_DESP_TBL_CPU_CODE
	#define PKT_DESP_TBL_CPU_CODE_OFFSET  86
	#define PKT_DESP_TBL_CPU_CODE_LEN     8
	#define PKT_DESP_TBL_CPU_CODE_DEFAULT 0x0
	/*[field] SERVICE_CODE*/
	#define PKT_DESP_TBL_SERVICE_CODE
	#define PKT_DESP_TBL_SERVICE_CODE_OFFSET  94
	#define PKT_DESP_TBL_SERVICE_CODE_LEN     8
	#define PKT_DESP_TBL_SERVICE_CODE_DEFAULT 0x0
	/*[field] DST_L3_IF*/
	#define PKT_DESP_TBL_DST_L3_IF
	#define PKT_DESP_TBL_DST_L3_IF_OFFSET  102
	#define PKT_DESP_TBL_DST_L3_IF_LEN     8
	#define PKT_DESP_TBL_DST_L3_IF_DEFAULT 0x0
	/*[field] MAC_DA*/
	#define PKT_DESP_TBL_MAC_DA
	#define PKT_DESP_TBL_MAC_DA_OFFSET  110
	#define PKT_DESP_TBL_MAC_DA_LEN     48
	#define PKT_DESP_TBL_MAC_DA_DEFAULT 0x0
	/*[field] ROUTE_FLAG*/
	#define PKT_DESP_TBL_ROUTE_FLAG
	#define PKT_DESP_TBL_ROUTE_FLAG_OFFSET  158
	#define PKT_DESP_TBL_ROUTE_FLAG_LEN     1
	#define PKT_DESP_TBL_ROUTE_FLAG_DEFAULT 0x0
	/*[field] PPPOE_STRIP_FLAG*/
	#define PKT_DESP_TBL_PPPOE_STRIP_FLAG
	#define PKT_DESP_TBL_PPPOE_STRIP_FLAG_OFFSET  159
	#define PKT_DESP_TBL_PPPOE_STRIP_FLAG_LEN     1
	#define PKT_DESP_TBL_PPPOE_STRIP_FLAG_DEFAULT 0x0
	/*[field] NAT_ACTION*/
	#define PKT_DESP_TBL_NAT_ACTION
	#define PKT_DESP_TBL_NAT_ACTION_OFFSET  160
	#define PKT_DESP_TBL_NAT_ACTION_LEN     3
	#define PKT_DESP_TBL_NAT_ACTION_DEFAULT 0x0
	/*[field] NAPT_PORT*/
	#define PKT_DESP_TBL_NAPT_PORT
	#define PKT_DESP_TBL_NAPT_PORT_OFFSET  163
	#define PKT_DESP_TBL_NAPT_PORT_LEN     16
	#define PKT_DESP_TBL_NAPT_PORT_DEFAULT 0x0
	/*[field] NAPT_ADDR*/
	#define PKT_DESP_TBL_NAPT_ADDR
	#define PKT_DESP_TBL_NAPT_ADDR_OFFSET  179
	#define PKT_DESP_TBL_NAPT_ADDR_LEN     32
	#define PKT_DESP_TBL_NAPT_ADDR_DEFAULT 0x0
	/*[field] DSCP_UPDATE*/
	#define PKT_DESP_TBL_DSCP_UPDATE
	#define PKT_DESP_TBL_DSCP_UPDATE_OFFSET  211
	#define PKT_DESP_TBL_DSCP_UPDATE_LEN     1
	#define PKT_DESP_TBL_DSCP_UPDATE_DEFAULT 0x0
	/*[field] DSCP*/
	#define PKT_DESP_TBL_DSCP
	#define PKT_DESP_TBL_DSCP_OFFSET  212
	#define PKT_DESP_TBL_DSCP_LEN     8
	#define PKT_DESP_TBL_DSCP_DEFAULT 0x0
	/*[field] TTL_UPDATE*/
	#define PKT_DESP_TBL_TTL_UPDATE
	#define PKT_DESP_TBL_TTL_UPDATE_OFFSET  220
	#define PKT_DESP_TBL_TTL_UPDATE_LEN     1
	#define PKT_DESP_TBL_TTL_UPDATE_DEFAULT 0x0
	/*[field] TTL*/
	#define PKT_DESP_TBL_TTL
	#define PKT_DESP_TBL_TTL_OFFSET  221
	#define PKT_DESP_TBL_TTL_LEN     8
	#define PKT_DESP_TBL_TTL_DEFAULT 0x0
	/*[field] STAG_FLAG*/
	#define PKT_DESP_TBL_STAG_FLAG
	#define PKT_DESP_TBL_STAG_FLAG_OFFSET  229
	#define PKT_DESP_TBL_STAG_FLAG_LEN     1
	#define PKT_DESP_TBL_STAG_FLAG_DEFAULT 0x0
	/*[field] CTAG_FLAG*/
	#define PKT_DESP_TBL_CTAG_FLAG
	#define PKT_DESP_TBL_CTAG_FLAG_OFFSET  230
	#define PKT_DESP_TBL_CTAG_FLAG_LEN     1
	#define PKT_DESP_TBL_CTAG_FLAG_DEFAULT 0x0
	/*[field] SNAP_FLAG*/
	#define PKT_DESP_TBL_SNAP_FLAG
	#define PKT_DESP_TBL_SNAP_FLAG_OFFSET  231
	#define PKT_DESP_TBL_SNAP_FLAG_LEN     1
	#define PKT_DESP_TBL_SNAP_FLAG_DEFAULT 0x0
	/*[field] PPPOE_FLAG*/
	#define PKT_DESP_TBL_PPPOE_FLAG
	#define PKT_DESP_TBL_PPPOE_FLAG_OFFSET  232
	#define PKT_DESP_TBL_PPPOE_FLAG_LEN     1
	#define PKT_DESP_TBL_PPPOE_FLAG_DEFAULT 0x0
	/*[field] INT_STAG_FMT*/
	#define PKT_DESP_TBL_INT_STAG_FMT
	#define PKT_DESP_TBL_INT_STAG_FMT_OFFSET  233
	#define PKT_DESP_TBL_INT_STAG_FMT_LEN     1
	#define PKT_DESP_TBL_INT_STAG_FMT_DEFAULT 0x0
	/*[field] INT_CTAG_FMT*/
	#define PKT_DESP_TBL_INT_CTAG_FMT
	#define PKT_DESP_TBL_INT_CTAG_FMT_OFFSET  234
	#define PKT_DESP_TBL_INT_CTAG_FMT_LEN     1
	#define PKT_DESP_TBL_INT_CTAG_FMT_DEFAULT 0x0
	/*[field] INT_SVID*/
	#define PKT_DESP_TBL_INT_SVID
	#define PKT_DESP_TBL_INT_SVID_OFFSET  235
	#define PKT_DESP_TBL_INT_SVID_LEN     12
	#define PKT_DESP_TBL_INT_SVID_DEFAULT 0x0
	/*[field] INT_CVID*/
	#define PKT_DESP_TBL_INT_CVID
	#define PKT_DESP_TBL_INT_CVID_OFFSET  247
	#define PKT_DESP_TBL_INT_CVID_LEN     12
	#define PKT_DESP_TBL_INT_CVID_DEFAULT 0x0
	/*[field] INT_SPCP*/
	#define PKT_DESP_TBL_INT_SPCP
	#define PKT_DESP_TBL_INT_SPCP_OFFSET  259
	#define PKT_DESP_TBL_INT_SPCP_LEN     3
	#define PKT_DESP_TBL_INT_SPCP_DEFAULT 0x0
	/*[field] INT_CPCP*/
	#define PKT_DESP_TBL_INT_CPCP
	#define PKT_DESP_TBL_INT_CPCP_OFFSET  262
	#define PKT_DESP_TBL_INT_CPCP_LEN     3
	#define PKT_DESP_TBL_INT_CPCP_DEFAULT 0x0
	/*[field] INT_SDEI*/
	#define PKT_DESP_TBL_INT_SDEI
	#define PKT_DESP_TBL_INT_SDEI_OFFSET  265
	#define PKT_DESP_TBL_INT_SDEI_LEN     1
	#define PKT_DESP_TBL_INT_SDEI_DEFAULT 0x0
	/*[field] INT_CDEI*/
	#define PKT_DESP_TBL_INT_CDEI
	#define PKT_DESP_TBL_INT_CDEI_OFFSET  266
	#define PKT_DESP_TBL_INT_CDEI_LEN     1
	#define PKT_DESP_TBL_INT_CDEI_DEFAULT 0x0
	/*[field] VSI*/
	#define PKT_DESP_TBL_VSI
	#define PKT_DESP_TBL_VSI_OFFSET  267
	#define PKT_DESP_TBL_VSI_LEN     5
	#define PKT_DESP_TBL_VSI_DEFAULT 0x0
	/*[field] L3_OFFSET*/
	#define PKT_DESP_TBL_L3_OFFSET
	#define PKT_DESP_TBL_L3_OFFSET_OFFSET  272
	#define PKT_DESP_TBL_L3_OFFSET_LEN     8
	#define PKT_DESP_TBL_L3_OFFSET_DEFAULT 0x0
	/*[field] L3_TYPE*/
	#define PKT_DESP_TBL_L3_TYPE
	#define PKT_DESP_TBL_L3_TYPE_OFFSET  280
	#define PKT_DESP_TBL_L3_TYPE_LEN     2
	#define PKT_DESP_TBL_L3_TYPE_DEFAULT 0x0
	/*[field] L4_OFFSET*/
	#define PKT_DESP_TBL_L4_OFFSET
	#define PKT_DESP_TBL_L4_OFFSET_OFFSET  282
	#define PKT_DESP_TBL_L4_OFFSET_LEN     8
	#define PKT_DESP_TBL_L4_OFFSET_DEFAULT 0x0
	/*[field] L4_TYPE*/
	#define PKT_DESP_TBL_L4_TYPE
	#define PKT_DESP_TBL_L4_TYPE_OFFSET  290
	#define PKT_DESP_TBL_L4_TYPE_LEN     3
	#define PKT_DESP_TBL_L4_TYPE_DEFAULT 0x0
	/*[field] NEXT_HEADER*/
	#define PKT_DESP_TBL_NEXT_HEADER
	#define PKT_DESP_TBL_NEXT_HEADER_OFFSET  293
	#define PKT_DESP_TBL_NEXT_HEADER_LEN     8
	#define PKT_DESP_TBL_NEXT_HEADER_DEFAULT 0x0
	/*[field] EG_VLAN_TAG_FMT_BYPASS_EN*/
	#define PKT_DESP_TBL_EG_VLAN_TAG_FMT_BYPASS_EN
	#define PKT_DESP_TBL_EG_VLAN_TAG_FMT_BYPASS_EN_OFFSET  301
	#define PKT_DESP_TBL_EG_VLAN_TAG_FMT_BYPASS_EN_LEN     1
	#define PKT_DESP_TBL_EG_VLAN_TAG_FMT_BYPASS_EN_DEFAULT 0x0
	/*[field] EG_VLAN_XLT_BYPASS_EN*/
	#define PKT_DESP_TBL_EG_VLAN_XLT_BYPASS_EN
	#define PKT_DESP_TBL_EG_VLAN_XLT_BYPASS_EN_OFFSET  302
	#define PKT_DESP_TBL_EG_VLAN_XLT_BYPASS_EN_LEN     1
	#define PKT_DESP_TBL_EG_VLAN_XLT_BYPASS_EN_DEFAULT 0x0
	/*[field] PKT_L2_EDIT_BYPASS*/
	#define PKT_DESP_TBL_PKT_L2_EDIT_BYPASS
	#define PKT_DESP_TBL_PKT_L2_EDIT_BYPASS_OFFSET  303
	#define PKT_DESP_TBL_PKT_L2_EDIT_BYPASS_LEN     1
	#define PKT_DESP_TBL_PKT_L2_EDIT_BYPASS_DEFAULT 0x0
	/*[field] PKT_L3_EDIT_BYPASS*/
	#define PKT_DESP_TBL_PKT_L3_EDIT_BYPASS
	#define PKT_DESP_TBL_PKT_L3_EDIT_BYPASS_OFFSET  304
	#define PKT_DESP_TBL_PKT_L3_EDIT_BYPASS_LEN     1
	#define PKT_DESP_TBL_PKT_L3_EDIT_BYPASS_DEFAULT 0x0
	/*[field] ACL_INDEX_TOGGLE*/
	#define PKT_DESP_TBL_ACL_INDEX_TOGGLE
	#define PKT_DESP_TBL_ACL_INDEX_TOGGLE_OFFSET  305
	#define PKT_DESP_TBL_ACL_INDEX_TOGGLE_LEN     1
	#define PKT_DESP_TBL_ACL_INDEX_TOGGLE_DEFAULT 0x0
	/*[field] ACL_INDEX_VALID*/
	#define PKT_DESP_TBL_ACL_INDEX_VALID
	#define PKT_DESP_TBL_ACL_INDEX_VALID_OFFSET  306
	#define PKT_DESP_TBL_ACL_INDEX_VALID_LEN     1
	#define PKT_DESP_TBL_ACL_INDEX_VALID_DEFAULT 0x0
	/*[field] ACL_INDEX*/
	#define PKT_DESP_TBL_ACL_INDEX
	#define PKT_DESP_TBL_ACL_INDEX_OFFSET  307
	#define PKT_DESP_TBL_ACL_INDEX_LEN     9
	#define PKT_DESP_TBL_ACL_INDEX_DEFAULT 0x0
	/*[field] IP_ADDR_INDEX_TYPE*/
	#define PKT_DESP_TBL_IP_ADDR_INDEX_TYPE
	#define PKT_DESP_TBL_IP_ADDR_INDEX_TYPE_OFFSET  316
	#define PKT_DESP_TBL_IP_ADDR_INDEX_TYPE_LEN     1
	#define PKT_DESP_TBL_IP_ADDR_INDEX_TYPE_DEFAULT 0x0
	/*[field] IP_ADDR_INDEX_TOGGLE*/
	#define PKT_DESP_TBL_IP_ADDR_INDEX_TOGGLE
	#define PKT_DESP_TBL_IP_ADDR_INDEX_TOGGLE_OFFSET  317
	#define PKT_DESP_TBL_IP_ADDR_INDEX_TOGGLE_LEN     1
	#define PKT_DESP_TBL_IP_ADDR_INDEX_TOGGLE_DEFAULT 0x0
	/*[field] IP_ADDR_INDEX_VALID*/
	#define PKT_DESP_TBL_IP_ADDR_INDEX_VALID
	#define PKT_DESP_TBL_IP_ADDR_INDEX_VALID_OFFSET  318
	#define PKT_DESP_TBL_IP_ADDR_INDEX_VALID_LEN     1
	#define PKT_DESP_TBL_IP_ADDR_INDEX_VALID_DEFAULT 0x0
	/*[field] IP_ADDR_INDEX*/
	#define PKT_DESP_TBL_IP_ADDR_INDEX
	#define PKT_DESP_TBL_IP_ADDR_INDEX_OFFSET  319
	#define PKT_DESP_TBL_IP_ADDR_INDEX_LEN     13
	#define PKT_DESP_TBL_IP_ADDR_INDEX_DEFAULT 0x0
	/*[field] CHG_PORT_VP*/
	#define PKT_DESP_TBL_CHG_PORT_VP
	#define PKT_DESP_TBL_CHG_PORT_VP_OFFSET  332
	#define PKT_DESP_TBL_CHG_PORT_VP_LEN     12
	#define PKT_DESP_TBL_CHG_PORT_VP_DEFAULT 0x0
	/*[field] HASH_FLAG*/
	#define PKT_DESP_TBL_HASH_FLAG
	#define PKT_DESP_TBL_HASH_FLAG_OFFSET  344
	#define PKT_DESP_TBL_HASH_FLAG_LEN     3
	#define PKT_DESP_TBL_HASH_FLAG_DEFAULT 0x0
	/*[field] HASH_VALUE*/
	#define PKT_DESP_TBL_HASH_VALUE
	#define PKT_DESP_TBL_HASH_VALUE_OFFSET  347
	#define PKT_DESP_TBL_HASH_VALUE_LEN     21
	#define PKT_DESP_TBL_HASH_VALUE_DEFAULT 0x0
	/*[field] COPY_CPU_FLAG*/
	#define PKT_DESP_TBL_COPY_CPU_FLAG
	#define PKT_DESP_TBL_COPY_CPU_FLAG_OFFSET  368
	#define PKT_DESP_TBL_COPY_CPU_FLAG_LEN     1
	#define PKT_DESP_TBL_COPY_CPU_FLAG_DEFAULT 0x0
	/*[field] SRC_PN*/
	#define PKT_DESP_TBL_SRC_PN
	#define PKT_DESP_TBL_SRC_PN_OFFSET  369
	#define PKT_DESP_TBL_SRC_PN_LEN     4
	#define PKT_DESP_TBL_SRC_PN_DEFAULT 0x0
	/*[field] RX_PTP_TYPE*/
	#define PKT_DESP_TBL_RX_PTP_TYPE
	#define PKT_DESP_TBL_RX_PTP_TYPE_OFFSET  373
	#define PKT_DESP_TBL_RX_PTP_TYPE_LEN     4
	#define PKT_DESP_TBL_RX_PTP_TYPE_DEFAULT 0x0
	/*[field] FAKE_L2_PROT*/
	#define PKT_DESP_TBL_FAKE_L2_PROT
	#define PKT_DESP_TBL_FAKE_L2_PROT_OFFSET  377
	#define PKT_DESP_TBL_FAKE_L2_PROT_LEN     1
	#define PKT_DESP_TBL_FAKE_L2_PROT_DEFAULT 0x0
	/*[field] FAKE_MAC_HEADER*/
	#define PKT_DESP_TBL_FAKE_MAC_HEADER
	#define PKT_DESP_TBL_FAKE_MAC_HEADER_OFFSET  378
	#define PKT_DESP_TBL_FAKE_MAC_HEADER_LEN     1
	#define PKT_DESP_TBL_FAKE_MAC_HEADER_DEFAULT 0x0
	/*[field] VSI_VALID*/
	#define PKT_DESP_TBL_VSI_VALID
	#define PKT_DESP_TBL_VSI_VALID_OFFSET  379
	#define PKT_DESP_TBL_VSI_VALID_LEN     1
	#define PKT_DESP_TBL_VSI_VALID_DEFAULT 0x0
	/*[field] VP_TX_CNT_EN*/
	#define PKT_DESP_TBL_VP_TX_CNT_EN
	#define PKT_DESP_TBL_VP_TX_CNT_EN_OFFSET  380
	#define PKT_DESP_TBL_VP_TX_CNT_EN_LEN     1
	#define PKT_DESP_TBL_VP_TX_CNT_EN_DEFAULT 0x0
	/*[field] RSV0*/
	#define PKT_DESP_TBL_RSV0
	#define PKT_DESP_TBL_RSV0_OFFSET  381
	#define PKT_DESP_TBL_RSV0_LEN     7
	#define PKT_DESP_TBL_RSV0_DEFAULT 0x0
	/*[field] AC_GROUP_BITMAP*/
	#define PKT_DESP_TBL_AC_GROUP_BITMAP
	#define PKT_DESP_TBL_AC_GROUP_BITMAP_OFFSET  388
	#define PKT_DESP_TBL_AC_GROUP_BITMAP_LEN     4
	#define PKT_DESP_TBL_AC_GROUP_BITMAP_DEFAULT 0x0
	/*[field] ONE_ENQ_FLAG*/
	#define PKT_DESP_TBL_ONE_ENQ_FLAG
	#define PKT_DESP_TBL_ONE_ENQ_FLAG_OFFSET  392
	#define PKT_DESP_TBL_ONE_ENQ_FLAG_LEN     1
	#define PKT_DESP_TBL_ONE_ENQ_FLAG_DEFAULT 0x0
	/*[field] EDMA_VP*/
	#define PKT_DESP_TBL_EDMA_VP
	#define PKT_DESP_TBL_EDMA_VP_OFFSET  393
	#define PKT_DESP_TBL_EDMA_VP_LEN     3
	#define PKT_DESP_TBL_EDMA_VP_DEFAULT 0x0
	/*[field] FC_GRP_ID*/
	#define PKT_DESP_TBL_FC_GRP_ID
	#define PKT_DESP_TBL_FC_GRP_ID_OFFSET  396
	#define PKT_DESP_TBL_FC_GRP_ID_LEN     3
	#define PKT_DESP_TBL_FC_GRP_ID_DEFAULT 0x0
	/*[field] FC_EN*/
	#define PKT_DESP_TBL_FC_EN
	#define PKT_DESP_TBL_FC_EN_OFFSET  399
	#define PKT_DESP_TBL_FC_EN_LEN     1
	#define PKT_DESP_TBL_FC_EN_DEFAULT 0x0

struct pkt_desp_tbl_0 {
	a_uint32_t  org_src_port_vp:12;
	a_uint32_t  src_port_vp:12;
	a_uint32_t  packet_length_0:8;
	a_uint32_t  packet_length_1:6;
	a_uint32_t  ts_dir:1;
	a_uint32_t  tx_ts_en:1;
	a_uint32_t  tx_os_correction_en:1;
	a_uint32_t  tx_ptp_tag:10;
	a_uint32_t  _reserved0_0:13;
	a_uint32_t  _reserved0_1:16;
	a_uint32_t  int_pri:4;
	a_uint32_t  int_dp:2;
	a_uint32_t  cpu_code:8;
	a_uint32_t  service_code_0:2;
	a_uint32_t  service_code_1:6;
	a_uint32_t  dst_l3_if:8;
	a_uint32_t  mac_da_0:18;
	a_uint32_t  mac_da_1:30;
	a_uint32_t  route_flag:1;
	a_uint32_t  pppoe_strip_flag:1;
	a_uint32_t  nat_action:3;
	a_uint32_t  napt_port:16;
	a_uint32_t  napt_addr_0:13;
	a_uint32_t  napt_addr_1:19;
	a_uint32_t  dscp_update:1;
	a_uint32_t  dscp:8;
	a_uint32_t  ttl_update:1;
	a_uint32_t  ttl_0:3;
	a_uint32_t  ttl_1:5;
	a_uint32_t  stag_flag:1;
	a_uint32_t  ctag_flag:1;
	a_uint32_t  snap_flag:1;
	a_uint32_t  pppoe_flag:1;
	a_uint32_t  int_stag_fmt:1;
	a_uint32_t  int_ctag_fmt:1;
	a_uint32_t  int_svid:12;
	a_uint32_t  int_cvid_0:9;
	a_uint32_t  int_cvid_1:3;
	a_uint32_t  int_spcp:3;
	a_uint32_t  int_cpcp:3;
	a_uint32_t  int_sdei:1;
	a_uint32_t  int_cdei:1;
	a_uint32_t  vsi:5;
	a_uint32_t  l3_offset:8;
	a_uint32_t  l3_type:2;
	a_uint32_t  l4_offset_0:6;
	a_uint32_t  l4_offset_1:2;
	a_uint32_t  l4_type:3;
	a_uint32_t  next_header:8;
	a_uint32_t  eg_vlan_tag_fmt_bypass_en:1;
	a_uint32_t  eg_vlan_xlt_bypass_en:1;
	a_uint32_t  pkt_l2_edit_bypass:1;
	a_uint32_t  pkt_l3_edit_bypass:1;
	a_uint32_t  acl_index_toggle:1;
	a_uint32_t  acl_index_valid:1;
	a_uint32_t  acl_index:9;
	a_uint32_t  ip_addr_index_type:1;
	a_uint32_t  ip_addr_index_toggle:1;
	a_uint32_t  ip_addr_index_valid:1;
	a_uint32_t  ip_addr_index_0:1;
	a_uint32_t  ip_addr_index_1:12;
	a_uint32_t  chg_port_vp:12;
	a_uint32_t  hash_flag:3;
	a_uint32_t  hash_value_0:5;
	a_uint32_t  hash_value_1:16;
	a_uint32_t  copy_cpu_flag:1;
	a_uint32_t  src_pn:4;
	a_uint32_t  rx_ptp_type:4;
	a_uint32_t  fake_l2_prot:1;
	a_uint32_t  fake_mac_header:1;
	a_uint32_t  vsi_valid:1;
	a_uint32_t  vp_tx_cnt_en:1;
	a_uint32_t  rsv0_0:3;
	a_uint32_t  rsv0_1:4;
	a_uint32_t  ac_group_bitmap:4;
	a_uint32_t  one_enq_flag:1;
	a_uint32_t  edma_vp:3;
	a_uint32_t  fc_grp_id:3;
	a_uint32_t  fc_en:1;
	a_uint32_t  _reserved1:16;
};

struct pkt_desp_tbl_1 {
	a_uint32_t  org_src_port_vp:12;
	a_uint32_t  src_port_vp:12;
	a_uint32_t  packet_length_0:8;
	a_uint32_t  packet_length_1:6;
	a_uint32_t  ts_dir:1;
	a_uint32_t  rx_ts_valid:1;
	a_uint32_t  rx_ts_0:24;
	a_uint32_t  rx_ts_1:16;
	a_uint32_t  int_pri:4;
	a_uint32_t  int_dp:2;
	a_uint32_t  cpu_code:8;
	a_uint32_t  service_code_0:2;
	a_uint32_t  service_code_1:6;
	a_uint32_t  dst_l3_if:8;
	a_uint32_t  mac_da_0:18;
	a_uint32_t  mac_da_1:30;
	a_uint32_t  route_flag:1;
	a_uint32_t  pppoe_strip_flag:1;
	a_uint32_t  nat_action:3;
	a_uint32_t  napt_port:16;
	a_uint32_t  napt_addr_0:13;
	a_uint32_t  napt_addr_1:19;
	a_uint32_t  dscp_update:1;
	a_uint32_t  dscp:8;
	a_uint32_t  ttl_update:1;
	a_uint32_t  ttl_0:3;
	a_uint32_t  ttl_1:5;
	a_uint32_t  stag_flag:1;
	a_uint32_t  ctag_flag:1;
	a_uint32_t  snap_flag:1;
	a_uint32_t  pppoe_flag:1;
	a_uint32_t  int_stag_fmt:1;
	a_uint32_t  int_ctag_fmt:1;
	a_uint32_t  int_svid:12;
	a_uint32_t  int_cvid_0:9;
	a_uint32_t  int_cvid_1:3;
	a_uint32_t  int_spcp:3;
	a_uint32_t  int_cpcp:3;
	a_uint32_t  int_sdei:1;
	a_uint32_t  int_cdei:1;
	a_uint32_t  vsi:5;
	a_uint32_t  l3_offset:8;
	a_uint32_t  l3_type:2;
	a_uint32_t  l4_offset_0:6;
	a_uint32_t  l4_offset_1:2;
	a_uint32_t  l4_type:3;
	a_uint32_t  next_header:8;
	a_uint32_t  eg_vlan_tag_fmt_bypass_en:1;
	a_uint32_t  eg_vlan_xlt_bypass_en:1;
	a_uint32_t  pkt_l2_edit_bypass:1;
	a_uint32_t  pkt_l3_edit_bypass:1;
	a_uint32_t  acl_index_toggle:1;
	a_uint32_t  acl_index_valid:1;
	a_uint32_t  acl_index:9;
	a_uint32_t  ip_addr_index_type:1;
	a_uint32_t  ip_addr_index_toggle:1;
	a_uint32_t  ip_addr_index_valid:1;
	a_uint32_t  ip_addr_index_0:1;
	a_uint32_t  ip_addr_index_1:12;
	a_uint32_t  chg_port_vp:12;
	a_uint32_t  hash_flag:3;
	a_uint32_t  hash_value_0:5;
	a_uint32_t  hash_value_1:16;
	a_uint32_t  copy_cpu_flag:1;
	a_uint32_t  src_pn:4;
	a_uint32_t  rx_ptp_type:4;
	a_uint32_t  fake_l2_prot:1;
	a_uint32_t  fake_mac_header:1;
	a_uint32_t  vsi_valid:1;
	a_uint32_t  vp_tx_cnt_en:1;
	a_uint32_t  rsv0_0:3;
	a_uint32_t  rsv0_1:4;
	a_uint32_t  ac_group_bitmap:4;
	a_uint32_t  one_enq_flag:1;
	a_uint32_t  edma_vp:3;
	a_uint32_t  fc_grp_id:3;
	a_uint32_t  fc_en:1;
	a_uint32_t  _reserved0:16;
};

union pkt_desp_tbl_u {
	a_uint32_t val[13];
	struct pkt_desp_tbl_0 bf0;
	struct pkt_desp_tbl_1 bf1;
};

/*[table] UNI_DROP_CNT_TBL*/
#define UNI_DROP_CNT_TBL
#define UNI_DROP_CNT_TBL_ADDRESS 0x1e0000
#define UNI_DROP_CNT_TBL_NUM     1536
#define UNI_DROP_CNT_TBL_INC     0x10
#define UNI_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define UNI_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] UNI_DROP_PKT*/
	#define UNI_DROP_CNT_TBL_UNI_DROP_PKT
	#define UNI_DROP_CNT_TBL_UNI_DROP_PKT_OFFSET  0
	#define UNI_DROP_CNT_TBL_UNI_DROP_PKT_LEN     32
	#define UNI_DROP_CNT_TBL_UNI_DROP_PKT_DEFAULT 0x0
	/*[field] UNI_DROP_BYTE*/
	#define UNI_DROP_CNT_TBL_UNI_DROP_BYTE
	#define UNI_DROP_CNT_TBL_UNI_DROP_BYTE_OFFSET  32
	#define UNI_DROP_CNT_TBL_UNI_DROP_BYTE_LEN     40
	#define UNI_DROP_CNT_TBL_UNI_DROP_BYTE_DEFAULT 0x0

struct uni_drop_cnt_tbl {
	a_uint32_t  uni_drop_pkt:32;
	a_uint32_t  uni_drop_byte_0:32;
	a_uint32_t  uni_drop_byte_1:8;
	a_uint32_t  _reserved0:24;
};

union uni_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct uni_drop_cnt_tbl bf;
};

/*[table] MUL_P0_DROP_CNT_TBL*/
#define MUL_P0_DROP_CNT_TBL
#define MUL_P0_DROP_CNT_TBL_ADDRESS 0x1f0000
#define MUL_P0_DROP_CNT_TBL_NUM     48
#define MUL_P0_DROP_CNT_TBL_INC     0x10
#define MUL_P0_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define MUL_P0_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] MUL_P0_DROP_PKT*/
	#define MUL_P0_DROP_CNT_TBL_MUL_P0_DROP_PKT
	#define MUL_P0_DROP_CNT_TBL_MUL_P0_DROP_PKT_OFFSET  0
	#define MUL_P0_DROP_CNT_TBL_MUL_P0_DROP_PKT_LEN     32
	#define MUL_P0_DROP_CNT_TBL_MUL_P0_DROP_PKT_DEFAULT 0x0
	/*[field] MUL_P0_DROP_BYTE*/
	#define MUL_P0_DROP_CNT_TBL_MUL_P0_DROP_BYTE
	#define MUL_P0_DROP_CNT_TBL_MUL_P0_DROP_BYTE_OFFSET  32
	#define MUL_P0_DROP_CNT_TBL_MUL_P0_DROP_BYTE_LEN     40
	#define MUL_P0_DROP_CNT_TBL_MUL_P0_DROP_BYTE_DEFAULT 0x0

struct mul_p0_drop_cnt_tbl {
	a_uint32_t  mul_p0_drop_pkt:32;
	a_uint32_t  mul_p0_drop_byte_0:32;
	a_uint32_t  mul_p0_drop_byte_1:8;
	a_uint32_t  _reserved0:24;
};

union mul_p0_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct mul_p0_drop_cnt_tbl bf;
};

/*[table] MUL_P1_DROP_CNT_TBL*/
#define MUL_P1_DROP_CNT_TBL
#define MUL_P1_DROP_CNT_TBL_ADDRESS 0x1f1000
#define MUL_P1_DROP_CNT_TBL_NUM     12
#define MUL_P1_DROP_CNT_TBL_INC     0x10
#define MUL_P1_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define MUL_P1_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] MUL_P1_DROP_PKT*/
	#define MUL_P1_DROP_CNT_TBL_MUL_P1_DROP_PKT
	#define MUL_P1_DROP_CNT_TBL_MUL_P1_DROP_PKT_OFFSET  0
	#define MUL_P1_DROP_CNT_TBL_MUL_P1_DROP_PKT_LEN     32
	#define MUL_P1_DROP_CNT_TBL_MUL_P1_DROP_PKT_DEFAULT 0x0
	/*[field] MUL_P1_DROP_BYTE*/
	#define MUL_P1_DROP_CNT_TBL_MUL_P1_DROP_BYTE
	#define MUL_P1_DROP_CNT_TBL_MUL_P1_DROP_BYTE_OFFSET  32
	#define MUL_P1_DROP_CNT_TBL_MUL_P1_DROP_BYTE_LEN     40
	#define MUL_P1_DROP_CNT_TBL_MUL_P1_DROP_BYTE_DEFAULT 0x0

struct mul_p1_drop_cnt_tbl {
	a_uint32_t  mul_p1_drop_pkt:32;
	a_uint32_t  mul_p1_drop_byte_0:32;
	a_uint32_t  mul_p1_drop_byte_1:8;
	a_uint32_t  _reserved0:24;
};

union mul_p1_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct mul_p1_drop_cnt_tbl bf;
};

/*[table] MUL_P2_DROP_CNT_TBL*/
#define MUL_P2_DROP_CNT_TBL
#define MUL_P2_DROP_CNT_TBL_ADDRESS 0x1f2000
#define MUL_P2_DROP_CNT_TBL_NUM     12
#define MUL_P2_DROP_CNT_TBL_INC     0x10
#define MUL_P2_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define MUL_P2_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] MUL_P2_DROP_PKT*/
	#define MUL_P2_DROP_CNT_TBL_MUL_P2_DROP_PKT
	#define MUL_P2_DROP_CNT_TBL_MUL_P2_DROP_PKT_OFFSET  0
	#define MUL_P2_DROP_CNT_TBL_MUL_P2_DROP_PKT_LEN     32
	#define MUL_P2_DROP_CNT_TBL_MUL_P2_DROP_PKT_DEFAULT 0x0
	/*[field] MUL_P2_DROP_BYTE*/
	#define MUL_P2_DROP_CNT_TBL_MUL_P2_DROP_BYTE
	#define MUL_P2_DROP_CNT_TBL_MUL_P2_DROP_BYTE_OFFSET  32
	#define MUL_P2_DROP_CNT_TBL_MUL_P2_DROP_BYTE_LEN     40
	#define MUL_P2_DROP_CNT_TBL_MUL_P2_DROP_BYTE_DEFAULT 0x0

struct mul_p2_drop_cnt_tbl {
	a_uint32_t  mul_p2_drop_pkt:32;
	a_uint32_t  mul_p2_drop_byte_0:32;
	a_uint32_t  mul_p2_drop_byte_1:8;
	a_uint32_t  _reserved0:24;
};

union mul_p2_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct mul_p2_drop_cnt_tbl bf;
};

/*[table] MUL_P3_DROP_CNT_TBL*/
#define MUL_P3_DROP_CNT_TBL
#define MUL_P3_DROP_CNT_TBL_ADDRESS 0x1f3000
#define MUL_P3_DROP_CNT_TBL_NUM     12
#define MUL_P3_DROP_CNT_TBL_INC     0x10
#define MUL_P3_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define MUL_P3_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] MUL_P3_DROP_PKT*/
	#define MUL_P3_DROP_CNT_TBL_MUL_P3_DROP_PKT
	#define MUL_P3_DROP_CNT_TBL_MUL_P3_DROP_PKT_OFFSET  0
	#define MUL_P3_DROP_CNT_TBL_MUL_P3_DROP_PKT_LEN     32
	#define MUL_P3_DROP_CNT_TBL_MUL_P3_DROP_PKT_DEFAULT 0x0
	/*[field] MUL_P3_DROP_BYTE*/
	#define MUL_P3_DROP_CNT_TBL_MUL_P3_DROP_BYTE
	#define MUL_P3_DROP_CNT_TBL_MUL_P3_DROP_BYTE_OFFSET  32
	#define MUL_P3_DROP_CNT_TBL_MUL_P3_DROP_BYTE_LEN     40
	#define MUL_P3_DROP_CNT_TBL_MUL_P3_DROP_BYTE_DEFAULT 0x0

struct mul_p3_drop_cnt_tbl {
	a_uint32_t  mul_p3_drop_pkt:32;
	a_uint32_t  mul_p3_drop_byte_0:32;
	a_uint32_t  mul_p3_drop_byte_1:8;
	a_uint32_t  _reserved0:24;
};

union mul_p3_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct mul_p3_drop_cnt_tbl bf;
};

/*[table] MUL_P4_DROP_CNT_TBL*/
#define MUL_P4_DROP_CNT_TBL
#define MUL_P4_DROP_CNT_TBL_ADDRESS 0x1f4000
#define MUL_P4_DROP_CNT_TBL_NUM     12
#define MUL_P4_DROP_CNT_TBL_INC     0x10
#define MUL_P4_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define MUL_P4_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] MUL_P4_DROP_PKT*/
	#define MUL_P4_DROP_CNT_TBL_MUL_P4_DROP_PKT
	#define MUL_P4_DROP_CNT_TBL_MUL_P4_DROP_PKT_OFFSET  0
	#define MUL_P4_DROP_CNT_TBL_MUL_P4_DROP_PKT_LEN     32
	#define MUL_P4_DROP_CNT_TBL_MUL_P4_DROP_PKT_DEFAULT 0x0
	/*[field] MUL_P4_DROP_BYTE*/
	#define MUL_P4_DROP_CNT_TBL_MUL_P4_DROP_BYTE
	#define MUL_P4_DROP_CNT_TBL_MUL_P4_DROP_BYTE_OFFSET  32
	#define MUL_P4_DROP_CNT_TBL_MUL_P4_DROP_BYTE_LEN     40
	#define MUL_P4_DROP_CNT_TBL_MUL_P4_DROP_BYTE_DEFAULT 0x0

struct mul_p4_drop_cnt_tbl {
	a_uint32_t  mul_p4_drop_pkt:32;
	a_uint32_t  mul_p4_drop_byte_0:32;
	a_uint32_t  mul_p4_drop_byte_1:8;
	a_uint32_t  _reserved0:24;
};

union mul_p4_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct mul_p4_drop_cnt_tbl bf;
};

/*[table] MUL_P5_DROP_CNT_TBL*/
#define MUL_P5_DROP_CNT_TBL
#define MUL_P5_DROP_CNT_TBL_ADDRESS 0x1f5000
#define MUL_P5_DROP_CNT_TBL_NUM     12
#define MUL_P5_DROP_CNT_TBL_INC     0x10
#define MUL_P5_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define MUL_P5_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] MUL_P5_DROP_PKT*/
	#define MUL_P5_DROP_CNT_TBL_MUL_P5_DROP_PKT
	#define MUL_P5_DROP_CNT_TBL_MUL_P5_DROP_PKT_OFFSET  0
	#define MUL_P5_DROP_CNT_TBL_MUL_P5_DROP_PKT_LEN     32
	#define MUL_P5_DROP_CNT_TBL_MUL_P5_DROP_PKT_DEFAULT 0x0
	/*[field] MUL_P5_DROP_BYTE*/
	#define MUL_P5_DROP_CNT_TBL_MUL_P5_DROP_BYTE
	#define MUL_P5_DROP_CNT_TBL_MUL_P5_DROP_BYTE_OFFSET  32
	#define MUL_P5_DROP_CNT_TBL_MUL_P5_DROP_BYTE_LEN     40
	#define MUL_P5_DROP_CNT_TBL_MUL_P5_DROP_BYTE_DEFAULT 0x0

struct mul_p5_drop_cnt_tbl {
	a_uint32_t  mul_p5_drop_pkt:32;
	a_uint32_t  mul_p5_drop_byte_0:32;
	a_uint32_t  mul_p5_drop_byte_1:8;
	a_uint32_t  _reserved0:24;
};

union mul_p5_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct mul_p5_drop_cnt_tbl bf;
};

/*[table] MUL_P6_DROP_CNT_TBL*/
#define MUL_P6_DROP_CNT_TBL
#define MUL_P6_DROP_CNT_TBL_ADDRESS 0x1f6000
#define MUL_P6_DROP_CNT_TBL_NUM     12
#define MUL_P6_DROP_CNT_TBL_INC     0x10
#define MUL_P6_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define MUL_P6_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] MUL_P6_DROP_PKT*/
	#define MUL_P6_DROP_CNT_TBL_MUL_P6_DROP_PKT
	#define MUL_P6_DROP_CNT_TBL_MUL_P6_DROP_PKT_OFFSET  0
	#define MUL_P6_DROP_CNT_TBL_MUL_P6_DROP_PKT_LEN     32
	#define MUL_P6_DROP_CNT_TBL_MUL_P6_DROP_PKT_DEFAULT 0x0
	/*[field] MUL_P6_DROP_BYTE*/
	#define MUL_P6_DROP_CNT_TBL_MUL_P6_DROP_BYTE
	#define MUL_P6_DROP_CNT_TBL_MUL_P6_DROP_BYTE_OFFSET  32
	#define MUL_P6_DROP_CNT_TBL_MUL_P6_DROP_BYTE_LEN     40
	#define MUL_P6_DROP_CNT_TBL_MUL_P6_DROP_BYTE_DEFAULT 0x0

struct mul_p6_drop_cnt_tbl {
	a_uint32_t  mul_p6_drop_pkt:32;
	a_uint32_t  mul_p6_drop_byte_0:32;
	a_uint32_t  mul_p6_drop_byte_1:8;
	a_uint32_t  _reserved0:24;
};

union mul_p6_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct mul_p6_drop_cnt_tbl bf;
};

/*[table] MUL_P7_DROP_CNT_TBL*/
#define MUL_P7_DROP_CNT_TBL
#define MUL_P7_DROP_CNT_TBL_ADDRESS 0x1f7000
#define MUL_P7_DROP_CNT_TBL_NUM     12
#define MUL_P7_DROP_CNT_TBL_INC     0x10
#define MUL_P7_DROP_CNT_TBL_TYPE    REG_TYPE_RW
#define MUL_P7_DROP_CNT_TBL_DEFAULT 0x0
	/*[field] MUL_P7_DROP_PKT*/
	#define MUL_P7_DROP_CNT_TBL_MUL_P7_DROP_PKT
	#define MUL_P7_DROP_CNT_TBL_MUL_P7_DROP_PKT_OFFSET  0
	#define MUL_P7_DROP_CNT_TBL_MUL_P7_DROP_PKT_LEN     32
	#define MUL_P7_DROP_CNT_TBL_MUL_P7_DROP_PKT_DEFAULT 0x0
	/*[field] MUL_P7_DROP_BYTE*/
	#define MUL_P7_DROP_CNT_TBL_MUL_P7_DROP_BYTE
	#define MUL_P7_DROP_CNT_TBL_MUL_P7_DROP_BYTE_OFFSET  32
	#define MUL_P7_DROP_CNT_TBL_MUL_P7_DROP_BYTE_LEN     40
	#define MUL_P7_DROP_CNT_TBL_MUL_P7_DROP_BYTE_DEFAULT 0x0

struct mul_p7_drop_cnt_tbl {
	a_uint32_t  mul_p7_drop_pkt:32;
	a_uint32_t  mul_p7_drop_byte_0:32;
	a_uint32_t  mul_p7_drop_byte_1:8;
	a_uint32_t  _reserved0:24;
};

union mul_p7_drop_cnt_tbl_u {
	a_uint32_t val[3];
	struct mul_p7_drop_cnt_tbl bf;
};

/*[table] UQ_AGG_PROFILE_MAP*/
#define UQ_AGG_PROFILE_MAP
#define UQ_AGG_PROFILE_MAP_ADDRESS 0x1f8000
#define UQ_AGG_PROFILE_MAP_NUM     256
#define UQ_AGG_PROFILE_MAP_INC     0x10
#define UQ_AGG_PROFILE_MAP_TYPE    REG_TYPE_RW
#define UQ_AGG_PROFILE_MAP_DEFAULT 0x0
	/*[field] QID_2_AGG_ID*/
	#define UQ_AGG_PROFILE_MAP_QID_2_AGG_ID
	#define UQ_AGG_PROFILE_MAP_QID_2_AGG_ID_OFFSET  0
	#define UQ_AGG_PROFILE_MAP_QID_2_AGG_ID_LEN     3
	#define UQ_AGG_PROFILE_MAP_QID_2_AGG_ID_DEFAULT 0x0
	/*[field] ENABLE*/
	#define UQ_AGG_PROFILE_MAP_ENABLE
	#define UQ_AGG_PROFILE_MAP_ENABLE_OFFSET  3
	#define UQ_AGG_PROFILE_MAP_ENABLE_LEN     1
	#define UQ_AGG_PROFILE_MAP_ENABLE_DEFAULT 0x0

struct uq_agg_profile_map {
	a_uint32_t  qid_2_agg_id:3;
	a_uint32_t  enable:1;
	a_uint32_t  _reserved0:28;
};

union uq_agg_profile_map_u {
	a_uint32_t val;
	struct uq_agg_profile_map bf;
};

/*[table] QUEUE_TX_COUNTER_TBL*/
#define QUEUE_TX_COUNTER_TBL
#define QUEUE_TX_COUNTER_TBL_ADDRESS 0x4000
#define QUEUE_TX_COUNTER_TBL_NUM     300
#define QUEUE_TX_COUNTER_TBL_INC     0x10
#define QUEUE_TX_COUNTER_TBL_TYPE    REG_TYPE_RW
#define QUEUE_TX_COUNTER_TBL_DEFAULT 0x0
	/*[field] TX_PACKETS*/
	#define QUEUE_TX_COUNTER_TBL_TX_PACKETS
	#define QUEUE_TX_COUNTER_TBL_TX_PACKETS_OFFSET  0
	#define QUEUE_TX_COUNTER_TBL_TX_PACKETS_LEN     32
	#define QUEUE_TX_COUNTER_TBL_TX_PACKETS_DEFAULT 0x0
	/*[field] TX_BYTES*/
	#define QUEUE_TX_COUNTER_TBL_TX_BYTES
	#define QUEUE_TX_COUNTER_TBL_TX_BYTES_OFFSET  32
	#define QUEUE_TX_COUNTER_TBL_TX_BYTES_LEN     40
	#define QUEUE_TX_COUNTER_TBL_TX_BYTES_DEFAULT 0x0

struct queue_tx_counter_tbl {
	a_uint32_t  tx_packets:32;
	a_uint32_t  tx_bytes_0:32;
	a_uint32_t  tx_bytes_1:8;
	a_uint32_t  _reserved0:24;
};

union queue_tx_counter_tbl_u {
	a_uint32_t val[3];
	struct queue_tx_counter_tbl bf;
};

#endif
