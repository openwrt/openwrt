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
#ifndef _HPPE_QOS_H_
#define _HPPE_QOS_H_

#define L0_FLOW_MAP_TBL_MAX_ENTRY	300
#define L0_C_SP_CFG_TBL_MAX_ENTRY	512
#define L0_E_SP_CFG_TBL_MAX_ENTRY	512
#define L0_FLOW_PORT_MAP_TBL_MAX_ENTRY	300
#define L0_C_DRR_HEAD_TBL_MAX_ENTRY	160
#define L0_E_DRR_HEAD_TBL_MAX_ENTRY	160
#define L0_DRR_CREDIT_TBL_MAX_ENTRY	300
#define L0_C_DRR_LL_TBL_MAX_ENTRY	300
#define L0_C_DRR_REVERSE_LL_TBL_MAX_ENTRY	300
#define L0_E_DRR_LL_TBL_MAX_ENTRY	300
#define L0_E_DRR_REVERSE_LL_TBL_MAX_ENTRY	300
#define L0_SP_ENTRY_TBL_MAX_ENTRY	64
#define L0_ENS_Q_LL_TBL_MAX_ENTRY	300
#define L0_ENS_Q_HEAD_TBL_MAX_ENTRY	8
#define L0_ENS_Q_ENTRY_TBL_MAX_ENTRY	300
#define L0_FLOW_STATUS_TBL_MAX_ENTRY	300
#define RING_Q_MAP_TBL_MAX_ENTRY	16
#define RFC_BLOCK_TBL_MAX_ENTRY	300
#define RFC_STATUS_TBL_MAX_ENTRY	300
#define DEQ_DIS_TBL_MAX_ENTRY	300
#define L1_FLOW_MAP_TBL_MAX_ENTRY	64
#define L1_C_SP_CFG_TBL_MAX_ENTRY	64
#define L1_E_SP_CFG_TBL_MAX_ENTRY	64
#define L1_FLOW_PORT_MAP_TBL_MAX_ENTRY	64
#define L1_C_DRR_HEAD_TBL_MAX_ENTRY	36
#define L1_E_DRR_HEAD_TBL_MAX_ENTRY	36
#define L1_DRR_CREDIT_TBL_MAX_ENTRY	64
#define L1_C_DRR_LL_TBL_MAX_ENTRY	64
#define L1_C_DRR_REVERSE_LL_TBL_MAX_ENTRY	64
#define L1_E_DRR_LL_TBL_MAX_ENTRY	64
#define L1_E_DRR_REVERSE_LL_TBL_MAX_ENTRY	64
#define L1_A_FLOW_ENTRY_TBL_MAX_ENTRY	64
#define L1_B_FLOW_ENTRY_TBL_MAX_ENTRY	64
#define L1_SP_ENTRY_TBL_MAX_ENTRY	8

#define L1_ENS_Q_LL_TBL_MAX_ENTRY	64
#define L1_ENS_Q_HEAD_TBL_MAX_ENTRY	8
#define L1_ENS_Q_ENTRY_TBL_MAX_ENTRY	64
#define L1_FLOW_STATUS_TBL_MAX_ENTRY	64
#define PSCH_TDM_CFG_TBL_MAX_ENTRY	128

#define PORT_QOS_CTRL_MAX_ENTRY	8
#define PCP_QOS_GROUP_0_MAX_ENTRY	16
#define PCP_QOS_GROUP_1_MAX_ENTRY	16
#define FLOW_QOS_GROUP_0_MAX_ENTRY	32
#define FLOW_QOS_GROUP_1_MAX_ENTRY	32
#define DSCP_QOS_GROUP_0_MAX_ENTRY	64
#define DSCP_QOS_GROUP_1_MAX_ENTRY	64

sw_error_t
hppe_dscp_qos_group_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dscp_qos_group_0_u *value);

sw_error_t
hppe_dscp_qos_group_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dscp_qos_group_0_u *value);

sw_error_t
hppe_dscp_qos_group_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dscp_qos_group_1_u *value);

sw_error_t
hppe_dscp_qos_group_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dscp_qos_group_1_u *value);


sw_error_t
hppe_pcp_qos_group_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pcp_qos_group_0_u *value);

sw_error_t
hppe_pcp_qos_group_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pcp_qos_group_0_u *value);

sw_error_t
hppe_pcp_qos_group_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pcp_qos_group_1_u *value);

sw_error_t
hppe_pcp_qos_group_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pcp_qos_group_1_u *value);

sw_error_t
hppe_flow_qos_group_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_qos_group_0_u *value);

sw_error_t
hppe_flow_qos_group_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_qos_group_0_u *value);

sw_error_t
hppe_flow_qos_group_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_qos_group_1_u *value);

sw_error_t
hppe_flow_qos_group_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_qos_group_1_u *value);

sw_error_t
hppe_port_qos_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_qos_ctrl_u *value);

sw_error_t
hppe_port_qos_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_qos_ctrl_u *value);

sw_error_t
hppe_tdm_depth_cfg_get(
		a_uint32_t dev_id,
		union tdm_depth_cfg_u *value);

sw_error_t
hppe_tdm_depth_cfg_set(
		a_uint32_t dev_id,
		union tdm_depth_cfg_u *value);

sw_error_t
hppe_min_max_mode_cfg_get(
		a_uint32_t dev_id,
		union min_max_mode_cfg_u *value);

sw_error_t
hppe_min_max_mode_cfg_set(
		a_uint32_t dev_id,
		union min_max_mode_cfg_u *value);

sw_error_t
hppe_tm_dbg_addr_get(
		a_uint32_t dev_id,
		union tm_dbg_addr_u *value);

sw_error_t
hppe_tm_dbg_addr_set(
		a_uint32_t dev_id,
		union tm_dbg_addr_u *value);

sw_error_t
hppe_tm_dbg_data_get(
		a_uint32_t dev_id,
		union tm_dbg_data_u *value);

sw_error_t
hppe_tm_dbg_data_set(
		a_uint32_t dev_id,
		union tm_dbg_data_u *value);

sw_error_t
hppe_eco_reserve_0_get(
		a_uint32_t dev_id,
		union eco_reserve_0_u *value);

sw_error_t
hppe_eco_reserve_0_set(
		a_uint32_t dev_id,
		union eco_reserve_0_u *value);

sw_error_t
hppe_eco_reserve_1_get(
		a_uint32_t dev_id,
		union eco_reserve_1_u *value);

sw_error_t
hppe_eco_reserve_1_set(
		a_uint32_t dev_id,
		union eco_reserve_1_u *value);

sw_error_t
hppe_l0_flow_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_map_tbl_u *value);

sw_error_t
hppe_l0_flow_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_map_tbl_u *value);

sw_error_t
hppe_l0_c_sp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_sp_cfg_tbl_u *value);

sw_error_t
hppe_l0_c_sp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_sp_cfg_tbl_u *value);

sw_error_t
hppe_l0_e_sp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_sp_cfg_tbl_u *value);

sw_error_t
hppe_l0_e_sp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_sp_cfg_tbl_u *value);

sw_error_t
hppe_l0_flow_port_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_port_map_tbl_u *value);

sw_error_t
hppe_l0_flow_port_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_port_map_tbl_u *value);

sw_error_t
hppe_l0_c_drr_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_head_tbl_u *value);

sw_error_t
hppe_l0_c_drr_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_head_tbl_u *value);

sw_error_t
hppe_l0_e_drr_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_head_tbl_u *value);

sw_error_t
hppe_l0_e_drr_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_head_tbl_u *value);

sw_error_t
hppe_l0_drr_credit_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_drr_credit_tbl_u *value);

sw_error_t
hppe_l0_drr_credit_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_drr_credit_tbl_u *value);

sw_error_t
hppe_l0_c_drr_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_ll_tbl_u *value);

sw_error_t
hppe_l0_c_drr_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_ll_tbl_u *value);

sw_error_t
hppe_l0_c_drr_reverse_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_reverse_ll_tbl_u *value);

sw_error_t
hppe_l0_c_drr_reverse_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_reverse_ll_tbl_u *value);

sw_error_t
hppe_l0_e_drr_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_ll_tbl_u *value);

sw_error_t
hppe_l0_e_drr_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_ll_tbl_u *value);

sw_error_t
hppe_l0_e_drr_reverse_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_reverse_ll_tbl_u *value);

sw_error_t
hppe_l0_e_drr_reverse_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_reverse_ll_tbl_u *value);

sw_error_t
hppe_l0_sp_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_sp_entry_tbl_u *value);

sw_error_t
hppe_l0_sp_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_sp_entry_tbl_u *value);

sw_error_t
hppe_l0_ens_q_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_ll_tbl_u *value);

sw_error_t
hppe_l0_ens_q_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_ll_tbl_u *value);

sw_error_t
hppe_l0_ens_q_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_head_tbl_u *value);

sw_error_t
hppe_l0_ens_q_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_head_tbl_u *value);

sw_error_t
hppe_l0_ens_q_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_entry_tbl_u *value);

sw_error_t
hppe_l0_ens_q_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_entry_tbl_u *value);

sw_error_t
hppe_l0_flow_status_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_status_tbl_u *value);

sw_error_t
hppe_l0_flow_status_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_status_tbl_u *value);

sw_error_t
hppe_ring_q_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ring_q_map_tbl_u *value);

sw_error_t
hppe_ring_q_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ring_q_map_tbl_u *value);

sw_error_t
hppe_rfc_block_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfc_block_tbl_u *value);

sw_error_t
hppe_rfc_block_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfc_block_tbl_u *value);

sw_error_t
hppe_rfc_status_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfc_status_tbl_u *value);

sw_error_t
hppe_rfc_status_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfc_status_tbl_u *value);

sw_error_t
hppe_deq_dis_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union deq_dis_tbl_u *value);

sw_error_t
hppe_deq_dis_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union deq_dis_tbl_u *value);

sw_error_t
hppe_l1_flow_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_map_tbl_u *value);

sw_error_t
hppe_l1_flow_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_map_tbl_u *value);

sw_error_t
hppe_l1_c_sp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_sp_cfg_tbl_u *value);

sw_error_t
hppe_l1_c_sp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_sp_cfg_tbl_u *value);

sw_error_t
hppe_l1_e_sp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_sp_cfg_tbl_u *value);

sw_error_t
hppe_l1_e_sp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_sp_cfg_tbl_u *value);

sw_error_t
hppe_l1_flow_port_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_port_map_tbl_u *value);

sw_error_t
hppe_l1_flow_port_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_port_map_tbl_u *value);

sw_error_t
hppe_l1_c_drr_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_head_tbl_u *value);

sw_error_t
hppe_l1_c_drr_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_head_tbl_u *value);

sw_error_t
hppe_l1_e_drr_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_head_tbl_u *value);

sw_error_t
hppe_l1_e_drr_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_head_tbl_u *value);

sw_error_t
hppe_l1_drr_credit_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_drr_credit_tbl_u *value);

sw_error_t
hppe_l1_drr_credit_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_drr_credit_tbl_u *value);

sw_error_t
hppe_l1_c_drr_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_ll_tbl_u *value);

sw_error_t
hppe_l1_c_drr_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_ll_tbl_u *value);

sw_error_t
hppe_l1_c_drr_reverse_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_reverse_ll_tbl_u *value);

sw_error_t
hppe_l1_c_drr_reverse_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_reverse_ll_tbl_u *value);

sw_error_t
hppe_l1_e_drr_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_ll_tbl_u *value);

sw_error_t
hppe_l1_e_drr_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_ll_tbl_u *value);

sw_error_t
hppe_l1_e_drr_reverse_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_reverse_ll_tbl_u *value);

sw_error_t
hppe_l1_e_drr_reverse_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_reverse_ll_tbl_u *value);

sw_error_t
hppe_l1_a_flow_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_a_flow_entry_tbl_u *value);

sw_error_t
hppe_l1_a_flow_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_a_flow_entry_tbl_u *value);

sw_error_t
hppe_l1_b_flow_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_b_flow_entry_tbl_u *value);

sw_error_t
hppe_l1_b_flow_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_b_flow_entry_tbl_u *value);

sw_error_t
hppe_l1_sp_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_sp_entry_tbl_u *value);

sw_error_t
hppe_l1_sp_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_sp_entry_tbl_u *value);

sw_error_t
hppe_l1_ens_q_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_ll_tbl_u *value);

sw_error_t
hppe_l1_ens_q_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_ll_tbl_u *value);

sw_error_t
hppe_l1_ens_q_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_head_tbl_u *value);

sw_error_t
hppe_l1_ens_q_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_head_tbl_u *value);

sw_error_t
hppe_l1_ens_q_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_entry_tbl_u *value);

sw_error_t
hppe_l1_ens_q_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_entry_tbl_u *value);

sw_error_t
hppe_l1_flow_status_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_status_tbl_u *value);

sw_error_t
hppe_l1_flow_status_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_status_tbl_u *value);

sw_error_t
hppe_psch_tdm_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_tdm_cfg_tbl_u *value);

sw_error_t
hppe_psch_tdm_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_tdm_cfg_tbl_u *value);

sw_error_t
hppe_tdm_depth_cfg_tdm_depth_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_tdm_depth_cfg_tdm_depth_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_min_max_mode_cfg_min_max_mode_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_min_max_mode_cfg_min_max_mode_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_tm_dbg_addr_dbg_addr_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_tm_dbg_addr_dbg_addr_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_tm_dbg_data_dbg_data_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_tm_dbg_data_dbg_data_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_eco_reserve_0_eco_res_0_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_eco_reserve_0_eco_res_0_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_eco_reserve_1_eco_res_1_get(
		a_uint32_t dev_id,
		unsigned int *value);

sw_error_t
hppe_eco_reserve_1_eco_res_1_set(
		a_uint32_t dev_id,
		unsigned int value);

sw_error_t
hppe_l0_flow_map_tbl_e_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_flow_map_tbl_e_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_flow_map_tbl_c_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_flow_map_tbl_c_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_flow_map_tbl_e_drr_wt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_flow_map_tbl_e_drr_wt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_flow_map_tbl_c_drr_wt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_flow_map_tbl_c_drr_wt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_flow_map_tbl_sp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_flow_map_tbl_sp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_sp_cfg_tbl_drr_credit_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_sp_cfg_tbl_drr_credit_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_sp_cfg_tbl_drr_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_sp_cfg_tbl_drr_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_sp_cfg_tbl_drr_credit_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_sp_cfg_tbl_drr_credit_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_sp_cfg_tbl_drr_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_sp_cfg_tbl_drr_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_flow_port_map_tbl_port_num_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_flow_port_map_tbl_port_num_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_head_tbl_backup_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_head_tbl_backup_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_head_tbl_active_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_head_tbl_active_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_head_tbl_backup_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_head_tbl_backup_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_head_tbl_active_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_head_tbl_active_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_head_tbl_active_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_head_tbl_active_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_head_tbl_backup_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_head_tbl_backup_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_head_tbl_active_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_head_tbl_active_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_head_tbl_backup_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_head_tbl_backup_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_head_tbl_backup_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_head_tbl_backup_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_head_tbl_active_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_head_tbl_active_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_head_tbl_backup_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_head_tbl_backup_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_head_tbl_active_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_head_tbl_active_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_head_tbl_active_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_head_tbl_active_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_head_tbl_backup_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_head_tbl_backup_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_head_tbl_active_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_head_tbl_active_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_head_tbl_backup_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_head_tbl_backup_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_drr_credit_tbl_e_drr_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_drr_credit_tbl_e_drr_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_drr_credit_tbl_c_drr_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_drr_credit_tbl_c_drr_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_drr_credit_tbl_c_drr_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_drr_credit_tbl_c_drr_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_drr_credit_tbl_e_drr_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_drr_credit_tbl_e_drr_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_c_drr_reverse_ll_tbl_pre_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_c_drr_reverse_ll_tbl_pre_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_e_drr_reverse_ll_tbl_pre_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_e_drr_reverse_ll_tbl_pre_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_sp_entry_tbl_entry_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_sp_entry_tbl_entry_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_sp_entry_tbl_entry_path_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_l0_sp_entry_tbl_entry_path_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_l0_ens_q_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_ens_q_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_ens_q_head_tbl_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_ens_q_head_tbl_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_ens_q_head_tbl_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_ens_q_head_tbl_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_ens_q_head_tbl_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_ens_q_head_tbl_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_in_q_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_in_q_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_flow_status_tbl_en_cdrr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_flow_status_tbl_en_cdrr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_flow_status_tbl_en_edrr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_flow_status_tbl_en_edrr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l0_flow_status_tbl_en_level_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l0_flow_status_tbl_en_level_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_ring_q_map_tbl_queue_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_ring_q_map_tbl_queue_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_rfc_block_tbl_rfc_block_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rfc_block_tbl_rfc_block_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_rfc_status_tbl_rfc_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_rfc_status_tbl_rfc_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_deq_dis_tbl_deq_dis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_deq_dis_tbl_deq_dis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_flow_map_tbl_e_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_flow_map_tbl_e_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_flow_map_tbl_c_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_flow_map_tbl_c_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_flow_map_tbl_e_drr_wt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_flow_map_tbl_e_drr_wt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_flow_map_tbl_c_drr_wt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_flow_map_tbl_c_drr_wt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_flow_map_tbl_sp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_flow_map_tbl_sp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_sp_cfg_tbl_drr_credit_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_sp_cfg_tbl_drr_credit_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_sp_cfg_tbl_drr_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_sp_cfg_tbl_drr_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_sp_cfg_tbl_drr_credit_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_sp_cfg_tbl_drr_credit_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_sp_cfg_tbl_drr_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_sp_cfg_tbl_drr_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_flow_port_map_tbl_port_num_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_flow_port_map_tbl_port_num_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_head_tbl_backup_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_head_tbl_backup_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_head_tbl_active_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_head_tbl_active_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_head_tbl_backup_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_head_tbl_backup_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_head_tbl_active_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_head_tbl_active_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_head_tbl_active_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_head_tbl_active_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_head_tbl_backup_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_head_tbl_backup_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_head_tbl_active_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_head_tbl_active_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_head_tbl_backup_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_head_tbl_backup_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_head_tbl_backup_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_head_tbl_backup_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_head_tbl_active_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_head_tbl_active_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_head_tbl_backup_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_head_tbl_backup_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_head_tbl_active_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_head_tbl_active_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_head_tbl_active_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_head_tbl_active_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_head_tbl_backup_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_head_tbl_backup_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_head_tbl_active_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_head_tbl_active_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_head_tbl_backup_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_head_tbl_backup_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_drr_credit_tbl_e_drr_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_drr_credit_tbl_e_drr_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_drr_credit_tbl_c_drr_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_drr_credit_tbl_c_drr_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_drr_credit_tbl_c_drr_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_drr_credit_tbl_c_drr_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_drr_credit_tbl_e_drr_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_drr_credit_tbl_e_drr_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_c_drr_reverse_ll_tbl_pre_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_c_drr_reverse_ll_tbl_pre_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_e_drr_reverse_ll_tbl_pre_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_e_drr_reverse_ll_tbl_pre_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_a_flow_entry_tbl_entry_path_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_a_flow_entry_tbl_entry_path_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_b_flow_entry_tbl_entry_path_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_b_flow_entry_tbl_entry_path_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_sp_entry_tbl_entry_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_sp_entry_tbl_entry_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_sp_entry_tbl_entry_path_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value);

sw_error_t
hppe_l1_sp_entry_tbl_entry_path_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value);

sw_error_t
hppe_l1_ens_q_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_ens_q_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_ens_q_head_tbl_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_ens_q_head_tbl_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_ens_q_head_tbl_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_ens_q_head_tbl_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_ens_q_head_tbl_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_ens_q_head_tbl_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_in_q_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_in_q_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_flow_status_tbl_en_cdrr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_flow_status_tbl_en_cdrr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_flow_status_tbl_en_edrr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_flow_status_tbl_en_edrr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_l1_flow_status_tbl_en_level_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_l1_flow_status_tbl_en_level_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_psch_tdm_cfg_tbl_ens_port_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_psch_tdm_cfg_tbl_ens_port_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_psch_tdm_cfg_tbl_des_port_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_psch_tdm_cfg_tbl_des_port_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_psch_tdm_cfg_tbl_ens_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_psch_tdm_cfg_tbl_ens_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_port_flow_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_port_flow_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_port_dscp_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_port_dscp_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_port_dscp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_port_dscp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_port_pcp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_port_pcp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_port_acl_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_port_acl_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_flow_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_flow_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_port_preheader_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_port_preheader_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_pcp_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_pcp_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_dscp_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_dscp_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_port_pcp_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_port_pcp_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_port_qos_ctrl_port_dei_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_port_qos_ctrl_port_dei_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pcp_qos_group_0_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pcp_qos_group_0_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_pcp_qos_group_1_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_pcp_qos_group_1_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_qos_group_0_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_qos_group_0_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_flow_qos_group_1_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_flow_qos_group_1_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_dscp_qos_group_0_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_dscp_qos_group_0_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);

sw_error_t
hppe_dscp_qos_group_1_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value);

sw_error_t
hppe_dscp_qos_group_1_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value);


#endif

