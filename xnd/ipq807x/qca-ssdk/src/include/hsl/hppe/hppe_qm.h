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
#ifndef _HPPE_QM_H_
#define _HPPE_QM_H_

#define MCAST_PRIORITY_MAP0_MAX_ENTRY	16
#define MCAST_PRIORITY_MAP1_MAX_ENTRY	16
#define MCAST_PRIORITY_MAP2_MAX_ENTRY	16
#define MCAST_PRIORITY_MAP3_MAX_ENTRY	16
#define MCAST_PRIORITY_MAP4_MAX_ENTRY	16
#define MCAST_PRIORITY_MAP5_MAX_ENTRY	16
#define MCAST_PRIORITY_MAP6_MAX_ENTRY	16
#define MCAST_PRIORITY_MAP7_MAX_ENTRY	16
#define UQ_AGG_PROFILE_CFG_MAX_ENTRY	8
#define MQ_AGG_PROFILE_CFG_MAX_ENTRY	8
#define GRP_AGG_PROFILE_CFG_MAX_ENTRY	4
#define UQ_AGG_IN_PROFILE_CNT_MAX_ENTRY	8
#define UQ_AGG_OUT_PROFILE_CNT_MAX_ENTRY	8
#define MQ_AGG_IN_PROFILE_CNT_MAX_ENTRY	8
#define MQ_AGG_OUT_PROFILE_CNT_MAX_ENTRY	8
#define GRP_AGG_IN_PROFILE_CNT_MAX_ENTRY	4
#define GRP_AGG_OUT_PROFILE_CNT_MAX_ENTRY	4
#define UCAST_QUEUE_MAP_TBL_MAX_ENTRY	3072
#define UCAST_HASH_MAP_TBL_MAX_ENTRY	4096
#define UCAST_PRIORITY_MAP_TBL_MAX_ENTRY	256
#define MCAST_QUEUE_MAP_TBL_MAX_ENTRY	256
#define AC_MSEQ_TBL_MAX_ENTRY	256
#define AC_UNI_QUEUE_CFG_TBL_MAX_ENTRY	256
#define AC_MUL_QUEUE_CFG_TBL_MAX_ENTRY	44
#define AC_GRP_CFG_TBL_MAX_ENTRY	4
#define AC_UNI_QUEUE_CNT_TBL_MAX_ENTRY	256
#define AC_MUL_QUEUE_CNT_TBL_MAX_ENTRY	44
#define AC_GRP_CNT_TBL_MAX_ENTRY	4
#define AC_UNI_QUEUE_DROP_STATE_TBL_MAX_ENTRY	256
#define AC_MUL_QUEUE_DROP_STATE_TBL_MAX_ENTRY	44
#define AC_GRP_DROP_STATE_TBL_MAX_ENTRY	4
#define OQ_ENQ_OPR_TBL_MAX_ENTRY	300
#define OQ_DEQ_OPR_TBL_MAX_ENTRY	300
#define OQ_HEAD_UNI_TBL_MAX_ENTRY	256
#define OQ_HEAD_MUL_TBL_MAX_ENTRY	44
#define OQ_LL_UNI_TBL_MAX_ENTRY	2048
#define OQ_LL_MUL_P0_TBL_MAX_ENTRY	2048
#define OQ_LL_MUL_P1_TBL_MAX_ENTRY	2048
#define OQ_LL_MUL_P2_TBL_MAX_ENTRY	2048
#define OQ_LL_MUL_P3_TBL_MAX_ENTRY	2048
#define OQ_LL_MUL_P4_TBL_MAX_ENTRY	2048
#define OQ_LL_MUL_P5_TBL_MAX_ENTRY	2048
#define OQ_LL_MUL_P6_TBL_MAX_ENTRY	2048
#define OQ_LL_MUL_P7_TBL_MAX_ENTRY	2048
#define PKT_DESP_TBL_MAX_ENTRY	2048
#define UNI_DROP_CNT_TBL_MAX_ENTRY	1536
#define MUL_P0_DROP_CNT_TBL_MAX_ENTRY	48
#define MUL_P1_DROP_CNT_TBL_MAX_ENTRY	12
#define MUL_P2_DROP_CNT_TBL_MAX_ENTRY	12
#define MUL_P3_DROP_CNT_TBL_MAX_ENTRY	12
#define MUL_P4_DROP_CNT_TBL_MAX_ENTRY	12
#define MUL_P5_DROP_CNT_TBL_MAX_ENTRY	12
#define MUL_P6_DROP_CNT_TBL_MAX_ENTRY	12
#define MUL_P7_DROP_CNT_TBL_MAX_ENTRY	12
#define UQ_AGG_PROFILE_MAP_MAX_ENTRY	256
#define QUEUE_TX_COUNTER_TBL_MAX_ENTRY	300

sw_error_t
hppe_queue_tx_counter_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union queue_tx_counter_tbl_u *value);

sw_error_t
hppe_queue_tx_counter_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union queue_tx_counter_tbl_u *value);

sw_error_t
hppe_flush_cfg_get(
		a_uint32_t dev_id,
		union flush_cfg_u *value);

sw_error_t
hppe_flush_cfg_set(
		a_uint32_t dev_id,
		union flush_cfg_u *value);

sw_error_t
hppe_in_mirror_priority_ctrl_get(
		a_uint32_t dev_id,
		union in_mirror_priority_ctrl_u *value);

sw_error_t
hppe_in_mirror_priority_ctrl_set(
		a_uint32_t dev_id,
		union in_mirror_priority_ctrl_u *value);

sw_error_t
hppe_eg_mirror_priority_ctrl_get(
		a_uint32_t dev_id,
		union eg_mirror_priority_ctrl_u *value);

sw_error_t
hppe_eg_mirror_priority_ctrl_set(
		a_uint32_t dev_id,
		union eg_mirror_priority_ctrl_u *value);

sw_error_t
hppe_ucast_default_hash_get(
		a_uint32_t dev_id,
		union ucast_default_hash_u *value);

sw_error_t
hppe_ucast_default_hash_set(
		a_uint32_t dev_id,
		union ucast_default_hash_u *value);

sw_error_t
hppe_spare_reg0_get(
		a_uint32_t dev_id,
		union spare_reg0_u *value);

sw_error_t
hppe_spare_reg0_set(
		a_uint32_t dev_id,
		union spare_reg0_u *value);

sw_error_t
hppe_spare_reg1_get(
		a_uint32_t dev_id,
		union spare_reg1_u *value);

sw_error_t
hppe_spare_reg1_set(
		a_uint32_t dev_id,
		union spare_reg1_u *value);

sw_error_t
hppe_qm_dbg_addr_get(
		a_uint32_t dev_id,
		union qm_dbg_addr_u *value);

sw_error_t
hppe_qm_dbg_addr_set(
		a_uint32_t dev_id,
		union qm_dbg_addr_u *value);

sw_error_t
hppe_qm_dbg_data_get(
		a_uint32_t dev_id,
		union qm_dbg_data_u *value);

sw_error_t
hppe_qm_dbg_data_set(
		a_uint32_t dev_id,
		union qm_dbg_data_u *value);

sw_error_t
hppe_mcast_priority_map0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map0_u *value);

sw_error_t
hppe_mcast_priority_map0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map0_u *value);

sw_error_t
hppe_mcast_priority_map1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map1_u *value);

sw_error_t
hppe_mcast_priority_map1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map1_u *value);

sw_error_t
hppe_mcast_priority_map2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map2_u *value);

sw_error_t
hppe_mcast_priority_map2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map2_u *value);

sw_error_t
hppe_mcast_priority_map3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map3_u *value);

sw_error_t
hppe_mcast_priority_map3_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map3_u *value);

sw_error_t
hppe_mcast_priority_map4_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map4_u *value);

sw_error_t
hppe_mcast_priority_map4_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map4_u *value);

sw_error_t
hppe_mcast_priority_map5_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map5_u *value);

sw_error_t
hppe_mcast_priority_map5_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map5_u *value);

sw_error_t
hppe_mcast_priority_map6_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map6_u *value);

sw_error_t
hppe_mcast_priority_map6_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map6_u *value);

sw_error_t
hppe_mcast_priority_map7_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map7_u *value);

sw_error_t
hppe_mcast_priority_map7_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_priority_map7_u *value);

sw_error_t
hppe_agg_profile_cnt_en_get(
		a_uint32_t dev_id,
		union agg_profile_cnt_en_u *value);

sw_error_t
hppe_agg_profile_cnt_en_set(
		a_uint32_t dev_id,
		union agg_profile_cnt_en_u *value);

sw_error_t
hppe_uq_agg_profile_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_profile_cfg_u *value);

sw_error_t
hppe_uq_agg_profile_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_profile_cfg_u *value);

sw_error_t
hppe_mq_agg_profile_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_profile_cfg_u *value);

sw_error_t
hppe_mq_agg_profile_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_profile_cfg_u *value);

sw_error_t
hppe_grp_agg_profile_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_profile_cfg_u *value);

sw_error_t
hppe_grp_agg_profile_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_profile_cfg_u *value);

sw_error_t
hppe_uq_agg_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_in_profile_cnt_u *value);

sw_error_t
hppe_uq_agg_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_in_profile_cnt_u *value);

sw_error_t
hppe_uq_agg_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_out_profile_cnt_u *value);

sw_error_t
hppe_uq_agg_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_out_profile_cnt_u *value);

sw_error_t
hppe_mq_agg_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_in_profile_cnt_u *value);

sw_error_t
hppe_mq_agg_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_in_profile_cnt_u *value);

sw_error_t
hppe_mq_agg_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_out_profile_cnt_u *value);

sw_error_t
hppe_mq_agg_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mq_agg_out_profile_cnt_u *value);

sw_error_t
hppe_grp_agg_in_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_in_profile_cnt_u *value);

sw_error_t
hppe_grp_agg_in_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_in_profile_cnt_u *value);

sw_error_t
hppe_grp_agg_out_profile_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_out_profile_cnt_u *value);

sw_error_t
hppe_grp_agg_out_profile_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union grp_agg_out_profile_cnt_u *value);

sw_error_t
hppe_ucast_queue_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_queue_map_tbl_u *value);

sw_error_t
hppe_ucast_queue_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_queue_map_tbl_u *value);

sw_error_t
hppe_ucast_hash_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_hash_map_tbl_u *value);

sw_error_t
hppe_ucast_hash_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_hash_map_tbl_u *value);

sw_error_t
hppe_ucast_priority_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_priority_map_tbl_u *value);

sw_error_t
hppe_ucast_priority_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ucast_priority_map_tbl_u *value);

sw_error_t
hppe_mcast_queue_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_queue_map_tbl_u *value);

sw_error_t
hppe_mcast_queue_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mcast_queue_map_tbl_u *value);

sw_error_t
hppe_ac_mseq_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mseq_tbl_u *value);

sw_error_t
hppe_ac_mseq_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mseq_tbl_u *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_cfg_tbl_u *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_cfg_tbl_u *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_cfg_tbl_u *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_cfg_tbl_u *value);

sw_error_t
hppe_ac_grp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_cfg_tbl_u *value);

sw_error_t
hppe_ac_grp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_cfg_tbl_u *value);

sw_error_t
hppe_ac_uni_queue_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_cnt_tbl_u *value);

sw_error_t
hppe_ac_uni_queue_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_cnt_tbl_u *value);

sw_error_t
hppe_ac_mul_queue_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_cnt_tbl_u *value);

sw_error_t
hppe_ac_mul_queue_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_cnt_tbl_u *value);

sw_error_t
hppe_ac_grp_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_cnt_tbl_u *value);

sw_error_t
hppe_ac_grp_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_cnt_tbl_u *value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_drop_state_tbl_u *value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_uni_queue_drop_state_tbl_u *value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_drop_state_tbl_u *value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_mul_queue_drop_state_tbl_u *value);

sw_error_t
hppe_ac_grp_drop_state_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_drop_state_tbl_u *value);

sw_error_t
hppe_ac_grp_drop_state_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ac_grp_drop_state_tbl_u *value);

sw_error_t
hppe_oq_enq_opr_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_enq_opr_tbl_u *value);

sw_error_t
hppe_oq_enq_opr_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_enq_opr_tbl_u *value);

sw_error_t
hppe_oq_deq_opr_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_deq_opr_tbl_u *value);

sw_error_t
hppe_oq_deq_opr_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_deq_opr_tbl_u *value);

sw_error_t
hppe_oq_head_uni_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_head_uni_tbl_u *value);

sw_error_t
hppe_oq_head_uni_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_head_uni_tbl_u *value);

sw_error_t
hppe_oq_head_mul_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_head_mul_tbl_u *value);

sw_error_t
hppe_oq_head_mul_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_head_mul_tbl_u *value);

sw_error_t
hppe_oq_ll_uni_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_uni_tbl_u *value);

sw_error_t
hppe_oq_ll_uni_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_uni_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p0_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p0_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p1_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p1_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p2_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p2_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p3_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p3_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p4_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p4_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p5_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p5_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p6_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p6_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p7_tbl_u *value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union oq_ll_mul_p7_tbl_u *value);

sw_error_t
hppe_pkt_desp_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pkt_desp_tbl_u *value);

sw_error_t
hppe_pkt_desp_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pkt_desp_tbl_u *value);

sw_error_t
hppe_uni_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uni_drop_cnt_tbl_u *value);

sw_error_t
hppe_uni_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uni_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p0_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p0_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p0_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p0_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p1_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p1_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p1_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p1_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p2_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p2_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p2_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p2_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p3_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p3_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p3_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p3_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p4_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p4_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p4_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p4_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p5_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p5_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p5_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p5_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p6_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p6_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p6_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p6_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p7_drop_cnt_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p7_drop_cnt_tbl_u *value);

sw_error_t
hppe_mul_p7_drop_cnt_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mul_p7_drop_cnt_tbl_u *value);

sw_error_t
hppe_uq_agg_profile_map_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_profile_map_u *value);

sw_error_t
hppe_uq_agg_profile_map_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union uq_agg_profile_map_u *value);

sw_error_t
hppe_flush_cfg_flush_busy_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flush_cfg_flush_busy_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flush_cfg_flush_qid_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flush_cfg_flush_qid_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flush_cfg_flush_dst_port_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flush_cfg_flush_dst_port_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flush_cfg_flush_all_queues_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flush_cfg_flush_all_queues_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flush_cfg_flush_wt_time_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flush_cfg_flush_wt_time_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_flush_cfg_flush_status_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_flush_cfg_flush_status_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_in_mirror_priority_ctrl_priority_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_in_mirror_priority_ctrl_priority_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_eg_mirror_priority_ctrl_priority_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_eg_mirror_priority_ctrl_priority_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_ucast_default_hash_hash_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_ucast_default_hash_hash_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_spare_reg0_spare_reg0_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_spare_reg0_spare_reg0_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_spare_reg1_spare_reg1_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_spare_reg1_spare_reg1_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_qm_dbg_addr_dbg_addr_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_qm_dbg_addr_dbg_addr_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_qm_dbg_data_dbg_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_qm_dbg_data_dbg_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_mcast_priority_map0_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mcast_priority_map0_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mcast_priority_map1_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mcast_priority_map1_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mcast_priority_map2_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mcast_priority_map2_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mcast_priority_map3_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mcast_priority_map3_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mcast_priority_map4_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mcast_priority_map4_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mcast_priority_map5_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mcast_priority_map5_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mcast_priority_map6_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mcast_priority_map6_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mcast_priority_map7_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mcast_priority_map7_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p2_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p2_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_1_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_1_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p0_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p0_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_grp_1_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_grp_1_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_grp_0_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_grp_0_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p6_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p6_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_3_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_3_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p4_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p4_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_2_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_2_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_5_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_5_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_6_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_6_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_grp_3_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_grp_3_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_grp_2_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_grp_2_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_4_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_4_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p7_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p7_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_7_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_7_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_global_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_global_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p5_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p5_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p1_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p1_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_0_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_uq_en_0_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p3_en_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_agg_profile_cnt_en_mq_p3_en_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_uq_agg_profile_cfg_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_uq_agg_profile_cfg_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mq_agg_profile_cfg_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mq_agg_profile_cfg_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_grp_agg_profile_cfg_th_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_grp_agg_profile_cfg_th_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_uq_agg_in_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_uq_agg_in_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_uq_agg_out_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_uq_agg_out_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mq_agg_in_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mq_agg_in_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mq_agg_out_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mq_agg_out_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_grp_agg_in_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_grp_agg_in_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_grp_agg_out_profile_cnt_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_grp_agg_out_profile_cnt_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ucast_queue_map_tbl_profile_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ucast_queue_map_tbl_profile_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ucast_queue_map_tbl_queue_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ucast_queue_map_tbl_queue_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ucast_hash_map_tbl_hash_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ucast_hash_map_tbl_hash_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ucast_priority_map_tbl_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ucast_priority_map_tbl_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mcast_queue_map_tbl_class_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mcast_queue_map_tbl_class_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mseq_tbl_ac_mseq_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mseq_tbl_ac_mseq_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_yel_max_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_yel_max_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_wred_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_wred_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_red_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_red_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_grp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_grp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_color_aware_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_color_aware_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_yel_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_yel_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_yel_min_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_yel_min_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_weight_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_weight_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_dynamic_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_dynamic_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_red_max_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_red_max_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_pre_alloc_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_pre_alloc_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_force_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_force_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_red_min_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_red_min_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_grn_min_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_gap_grn_grn_min_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_ceiling_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_shared_ceiling_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_grn_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cfg_tbl_ac_cfg_grn_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_red_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_red_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_grp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_grp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_color_aware_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_color_aware_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_yel_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_yel_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_pre_alloc_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_pre_alloc_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_force_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_force_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_shared_ceiling_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_shared_ceiling_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_grn_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_grn_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_gap_grn_yel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_gap_grn_yel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_gap_grn_red_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cfg_tbl_ac_cfg_gap_grn_red_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_grn_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_grn_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_dp_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_dp_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_palloc_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_palloc_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_color_aware_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_color_aware_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_red_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_red_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_gap_grn_yel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_gap_grn_yel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_force_ac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_cfg_force_ac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_yel_resume_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_yel_resume_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_gap_grn_red_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_gap_grn_red_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_limit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cfg_tbl_ac_grp_limit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_cnt_tbl_ac_uni_queue_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_cnt_tbl_ac_uni_queue_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_cnt_tbl_ac_mul_queue_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_cnt_tbl_ac_mul_queue_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cnt_tbl_ac_grp_alloc_used_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cnt_tbl_ac_grp_alloc_used_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_cnt_tbl_ac_grp_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_cnt_tbl_ac_grp_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_red_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_red_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_red_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_red_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_yel_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_yel_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_grn_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_grn_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_yel_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_yel_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_grn_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_uni_queue_drop_state_tbl_grn_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_red_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_red_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_red_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_red_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_yel_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_yel_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_grn_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_grn_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_yel_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_yel_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_grn_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_mul_queue_drop_state_tbl_grn_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_drop_state_tbl_red_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_drop_state_tbl_red_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_drop_state_tbl_red_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_drop_state_tbl_red_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_drop_state_tbl_yel_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_drop_state_tbl_yel_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_drop_state_tbl_grn_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_drop_state_tbl_grn_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_drop_state_tbl_yel_drop_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_drop_state_tbl_yel_drop_state_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ac_grp_drop_state_tbl_grn_resume_thrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_ac_grp_drop_state_tbl_grn_resume_thrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_enq_opr_tbl_enq_disable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_enq_opr_tbl_enq_disable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_deq_opr_tbl_deq_drop_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_deq_opr_tbl_deq_drop_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_head_uni_tbl_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_head_uni_tbl_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_head_uni_tbl_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_head_uni_tbl_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_head_uni_tbl_empty_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_head_uni_tbl_empty_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_head_mul_tbl_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_head_mul_tbl_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_head_mul_tbl_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_head_mul_tbl_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_head_mul_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_head_mul_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_head_mul_tbl_empty_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_head_mul_tbl_empty_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_head_mul_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_head_mul_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_head_mul_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_head_mul_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_uni_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_uni_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p0_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p1_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p2_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p3_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p4_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p5_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p6_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_next_pointer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_next_pointer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_ingress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_ingress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_egress_mirr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_egress_mirr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_normal_fwd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_oq_ll_mul_p7_tbl_normal_fwd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_route_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_route_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_cpcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_cpcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_pkt_l3_edit_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_pkt_l3_edit_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_ctag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_ctag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_fake_mac_header_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_fake_mac_header_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_acl_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_acl_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_l4_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_l4_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_svid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_svid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_sdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_sdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_fc_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_fc_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_packet_length_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_packet_length_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_rx_ts_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_pkt_desp_tbl_rx_ts_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_pkt_desp_tbl_ts_dir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_ts_dir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_chg_port_vp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_chg_port_vp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_one_enq_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_one_enq_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_fc_grp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_fc_grp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_fake_l2_prot_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_fake_l2_prot_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_org_src_port_vp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_org_src_port_vp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_hash_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_hash_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_stag_fmt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_stag_fmt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_service_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_service_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_rx_ptp_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_rx_ptp_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_mac_da_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_pkt_desp_tbl_mac_da_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_pkt_desp_tbl_cpu_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_cpu_code_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_eg_vlan_tag_fmt_bypass_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_eg_vlan_tag_fmt_bypass_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_cvid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_cvid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_eg_vlan_xlt_bypass_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_eg_vlan_xlt_bypass_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_hash_value_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_hash_value_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_stag_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_stag_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_dst_l3_if_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_dst_l3_if_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_cdei_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_cdei_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_edma_vp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_edma_vp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_ac_group_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_ac_group_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_vp_tx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_vp_tx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_src_port_vp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_src_port_vp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_nat_action_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_nat_action_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_dscp_update_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_dscp_update_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_pppoe_strip_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_pppoe_strip_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_snap_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_snap_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_vsi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_vsi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_pkt_l2_edit_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_pkt_l2_edit_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_tx_ptp_tag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_tx_ptp_tag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_dp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_dp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_src_pn_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_src_pn_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_tx_ts_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_tx_ts_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_l4_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_l4_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_ttl_update_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_ttl_update_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_napt_port_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_napt_port_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_napt_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_napt_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_copy_cpu_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_copy_cpu_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_ttl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_ttl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_l3_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_l3_offset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_rsv0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_rsv0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_next_header_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_next_header_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_acl_index_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_acl_index_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_rx_ts_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_rx_ts_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_dscp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_dscp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_acl_index_toggle_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_acl_index_toggle_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_ctag_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_ctag_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_toggle_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_ip_addr_index_toggle_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_tx_os_correction_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_tx_os_correction_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_int_spcp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_int_spcp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_pppoe_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_pppoe_flag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_l3_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_l3_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pkt_desp_tbl_vsi_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pkt_desp_tbl_vsi_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_uni_drop_cnt_tbl_uni_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_uni_drop_cnt_tbl_uni_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_uni_drop_cnt_tbl_uni_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_uni_drop_cnt_tbl_uni_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mul_p0_drop_cnt_tbl_mul_p0_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mul_p0_drop_cnt_tbl_mul_p0_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mul_p0_drop_cnt_tbl_mul_p0_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_mul_p0_drop_cnt_tbl_mul_p0_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_mul_p1_drop_cnt_tbl_mul_p1_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_mul_p1_drop_cnt_tbl_mul_p1_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_mul_p1_drop_cnt_tbl_mul_p1_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mul_p1_drop_cnt_tbl_mul_p1_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mul_p2_drop_cnt_tbl_mul_p2_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mul_p2_drop_cnt_tbl_mul_p2_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mul_p2_drop_cnt_tbl_mul_p2_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_mul_p2_drop_cnt_tbl_mul_p2_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_mul_p3_drop_cnt_tbl_mul_p3_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mul_p3_drop_cnt_tbl_mul_p3_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mul_p3_drop_cnt_tbl_mul_p3_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_mul_p3_drop_cnt_tbl_mul_p3_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_mul_p4_drop_cnt_tbl_mul_p4_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mul_p4_drop_cnt_tbl_mul_p4_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mul_p4_drop_cnt_tbl_mul_p4_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_mul_p4_drop_cnt_tbl_mul_p4_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_mul_p5_drop_cnt_tbl_mul_p5_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_mul_p5_drop_cnt_tbl_mul_p5_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_mul_p5_drop_cnt_tbl_mul_p5_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mul_p5_drop_cnt_tbl_mul_p5_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mul_p6_drop_cnt_tbl_mul_p6_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_mul_p6_drop_cnt_tbl_mul_p6_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_mul_p6_drop_cnt_tbl_mul_p6_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mul_p6_drop_cnt_tbl_mul_p6_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mul_p7_drop_cnt_tbl_mul_p7_drop_pkt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_mul_p7_drop_cnt_tbl_mul_p7_drop_pkt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_mul_p7_drop_cnt_tbl_mul_p7_drop_byte_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_mul_p7_drop_cnt_tbl_mul_p7_drop_byte_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_uq_agg_profile_map_qid_2_agg_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_uq_agg_profile_map_qid_2_agg_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_uq_agg_profile_map_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_uq_agg_profile_map_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_queue_tx_counter_tbl_tx_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_queue_tx_counter_tbl_tx_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_queue_tx_counter_tbl_tx_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_queue_tx_counter_tbl_tx_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

#endif

