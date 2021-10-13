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
#include "sw.h"
#include "hsl.h"
#include "hppe_reg_access.h"
#include "hppe_qos_reg.h"
#include "hppe_qos.h"

sw_error_t
hppe_dscp_qos_group_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dscp_qos_group_0_u *value)
{
	if (index >= DSCP_QOS_GROUP_0_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + DSCP_QOS_GROUP_0_ADDRESS + \
				index * DSCP_QOS_GROUP_0_INC,
				&value->val);
}

sw_error_t
hppe_dscp_qos_group_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dscp_qos_group_0_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + DSCP_QOS_GROUP_0_ADDRESS + \
				index * DSCP_QOS_GROUP_0_INC,
				value->val);
}

sw_error_t
hppe_dscp_qos_group_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dscp_qos_group_1_u *value)
{
	if (index >= DSCP_QOS_GROUP_1_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + DSCP_QOS_GROUP_1_ADDRESS + \
				index * DSCP_QOS_GROUP_1_INC,
				&value->val);
}

sw_error_t
hppe_dscp_qos_group_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union dscp_qos_group_1_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + DSCP_QOS_GROUP_1_ADDRESS + \
				index * DSCP_QOS_GROUP_1_INC,
				value->val);
}

sw_error_t
hppe_pcp_qos_group_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pcp_qos_group_0_u *value)
{
	if (index >= PCP_QOS_GROUP_0_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + PCP_QOS_GROUP_0_ADDRESS + \
				index * PCP_QOS_GROUP_0_INC,
				&value->val);
}

sw_error_t
hppe_pcp_qos_group_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pcp_qos_group_0_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + PCP_QOS_GROUP_0_ADDRESS + \
				index * PCP_QOS_GROUP_0_INC,
				value->val);
}

sw_error_t
hppe_pcp_qos_group_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pcp_qos_group_1_u *value)
{
	if (index >= PCP_QOS_GROUP_1_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + PCP_QOS_GROUP_1_ADDRESS + \
				index * PCP_QOS_GROUP_1_INC,
				&value->val);
}

sw_error_t
hppe_pcp_qos_group_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union pcp_qos_group_1_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + PCP_QOS_GROUP_1_ADDRESS + \
				index * PCP_QOS_GROUP_1_INC,
				value->val);
}

sw_error_t
hppe_flow_qos_group_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_qos_group_0_u *value)
{
	if (index >= FLOW_QOS_GROUP_0_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FLOW_QOS_GROUP_0_ADDRESS + \
				index * FLOW_QOS_GROUP_0_INC,
				&value->val);
}

sw_error_t
hppe_flow_qos_group_0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_qos_group_0_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FLOW_QOS_GROUP_0_ADDRESS + \
				index * FLOW_QOS_GROUP_0_INC,
				value->val);
}

sw_error_t
hppe_flow_qos_group_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_qos_group_1_u *value)
{
	if (index >= FLOW_QOS_GROUP_1_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + FLOW_QOS_GROUP_1_ADDRESS + \
				index * FLOW_QOS_GROUP_1_INC,
				&value->val);
}

sw_error_t
hppe_flow_qos_group_1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union flow_qos_group_1_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + FLOW_QOS_GROUP_1_ADDRESS + \
				index * FLOW_QOS_GROUP_1_INC,
				value->val);
}

sw_error_t
hppe_port_qos_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_qos_ctrl_u *value)
{
	if (index >= PORT_QOS_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_QOS_CTRL_ADDRESS + \
				index * PORT_QOS_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_port_qos_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_qos_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_QOS_CTRL_ADDRESS + \
				index * PORT_QOS_CTRL_INC,
				value->val);
}
#ifndef IN_QOS_MINI
sw_error_t
hppe_tdm_depth_cfg_get(
		a_uint32_t dev_id,
		union tdm_depth_cfg_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + TDM_DEPTH_CFG_ADDRESS,
				&value->val);
}
#endif
sw_error_t
hppe_tdm_depth_cfg_set(
		a_uint32_t dev_id,
		union tdm_depth_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + TDM_DEPTH_CFG_ADDRESS,
				value->val);
}
#ifndef IN_QOS_MINI
sw_error_t
hppe_min_max_mode_cfg_get(
		a_uint32_t dev_id,
		union min_max_mode_cfg_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + MIN_MAX_MODE_CFG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_min_max_mode_cfg_set(
		a_uint32_t dev_id,
		union min_max_mode_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + MIN_MAX_MODE_CFG_ADDRESS,
				value->val);
}

sw_error_t
hppe_tm_dbg_addr_get(
		a_uint32_t dev_id,
		union tm_dbg_addr_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + TM_DBG_ADDR_ADDRESS,
				&value->val);
}

sw_error_t
hppe_tm_dbg_addr_set(
		a_uint32_t dev_id,
		union tm_dbg_addr_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + TM_DBG_ADDR_ADDRESS,
				value->val);
}

sw_error_t
hppe_tm_dbg_data_get(
		a_uint32_t dev_id,
		union tm_dbg_data_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + TM_DBG_DATA_ADDRESS,
				&value->val);
}

sw_error_t
hppe_tm_dbg_data_set(
		a_uint32_t dev_id,
		union tm_dbg_data_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_eco_reserve_0_get(
		a_uint32_t dev_id,
		union eco_reserve_0_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + ECO_RESERVE_0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_eco_reserve_0_set(
		a_uint32_t dev_id,
		union eco_reserve_0_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + ECO_RESERVE_0_ADDRESS,
				value->val);
}

sw_error_t
hppe_eco_reserve_1_get(
		a_uint32_t dev_id,
		union eco_reserve_1_u *value)
{
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + ECO_RESERVE_1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_eco_reserve_1_set(
		a_uint32_t dev_id,
		union eco_reserve_1_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + ECO_RESERVE_1_ADDRESS,
				value->val);
}
#endif
sw_error_t
hppe_l0_flow_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_map_tbl_u *value)
{
	if (index >= L0_FLOW_MAP_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_FLOW_MAP_TBL_ADDRESS + \
				index * L0_FLOW_MAP_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_flow_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_map_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_FLOW_MAP_TBL_ADDRESS + \
				index * L0_FLOW_MAP_TBL_INC,
				value->val);
}

sw_error_t
hppe_l0_c_sp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_sp_cfg_tbl_u *value)
{
	if (index >= L0_C_SP_CFG_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_C_SP_CFG_TBL_ADDRESS + \
				index * L0_C_SP_CFG_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_c_sp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_sp_cfg_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_C_SP_CFG_TBL_ADDRESS + \
				index * L0_C_SP_CFG_TBL_INC,
				value->val);
}

sw_error_t
hppe_l0_e_sp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_sp_cfg_tbl_u *value)
{
	if (index >= L0_E_SP_CFG_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_E_SP_CFG_TBL_ADDRESS + \
				index * L0_E_SP_CFG_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_e_sp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_sp_cfg_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_E_SP_CFG_TBL_ADDRESS + \
				index * L0_E_SP_CFG_TBL_INC,
				value->val);
}

sw_error_t
hppe_l0_flow_port_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_port_map_tbl_u *value)
{
	if (index >= L0_FLOW_PORT_MAP_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_FLOW_PORT_MAP_TBL_ADDRESS + \
				index * L0_FLOW_PORT_MAP_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_flow_port_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_port_map_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_FLOW_PORT_MAP_TBL_ADDRESS + \
				index * L0_FLOW_PORT_MAP_TBL_INC,
				value->val);
}
#ifndef IN_QOS_MINI
sw_error_t
hppe_l0_c_drr_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_head_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_C_DRR_HEAD_TBL_ADDRESS + \
				index * L0_C_DRR_HEAD_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_l0_c_drr_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_head_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_head_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_E_DRR_HEAD_TBL_ADDRESS + \
				index * L0_E_DRR_HEAD_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_l0_e_drr_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_head_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_drr_credit_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_drr_credit_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_DRR_CREDIT_TBL_ADDRESS + \
				index * L0_DRR_CREDIT_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_l0_drr_credit_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_drr_credit_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_ll_tbl_u *value)
{
	if (index >= L0_C_DRR_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_C_DRR_LL_TBL_ADDRESS + \
				index * L0_C_DRR_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_c_drr_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_reverse_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_reverse_ll_tbl_u *value)
{
	if (index >= L0_C_DRR_REVERSE_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_C_DRR_REVERSE_LL_TBL_ADDRESS + \
				index * L0_C_DRR_REVERSE_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_c_drr_reverse_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_c_drr_reverse_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_ll_tbl_u *value)
{
	if (index >= L0_E_DRR_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_E_DRR_LL_TBL_ADDRESS + \
				index * L0_E_DRR_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_e_drr_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_reverse_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_reverse_ll_tbl_u *value)
{
	if (index >= L0_E_DRR_REVERSE_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_E_DRR_REVERSE_LL_TBL_ADDRESS + \
				index * L0_E_DRR_REVERSE_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_e_drr_reverse_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_e_drr_reverse_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_sp_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_sp_entry_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_SP_ENTRY_TBL_ADDRESS + \
				index * L0_SP_ENTRY_TBL_INC,
				value->val,
				5);
}

sw_error_t
hppe_l0_sp_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_sp_entry_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_ll_tbl_u *value)
{
	if (index >= L0_ENS_Q_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_ENS_Q_LL_TBL_ADDRESS + \
				index * L0_ENS_Q_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_ens_q_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_head_tbl_u *value)
{
	if (index >= L0_ENS_Q_HEAD_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_ENS_Q_HEAD_TBL_ADDRESS + \
				index * L0_ENS_Q_HEAD_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_ens_q_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_head_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_entry_tbl_u *value)
{
	if (index >= L0_ENS_Q_ENTRY_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_ENS_Q_ENTRY_TBL_ADDRESS + \
				index * L0_ENS_Q_ENTRY_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_ens_q_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_ens_q_entry_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_flow_status_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_status_tbl_u *value)
{
	if (index >= L0_FLOW_STATUS_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L0_FLOW_STATUS_TBL_ADDRESS + \
				index * L0_FLOW_STATUS_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l0_flow_status_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l0_flow_status_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_ring_q_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ring_q_map_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + RING_Q_MAP_TBL_ADDRESS + \
				index * RING_Q_MAP_TBL_INC,
				value->val,
				10);
}

sw_error_t
hppe_ring_q_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ring_q_map_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + RING_Q_MAP_TBL_ADDRESS + \
				index * RING_Q_MAP_TBL_INC,
				value->val,
				10);
}
#ifndef IN_QOS_MINI
sw_error_t
hppe_rfc_block_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfc_block_tbl_u *value)
{
	if (index >= RFC_BLOCK_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + RFC_BLOCK_TBL_ADDRESS + \
				index * RFC_BLOCK_TBL_INC,
				&value->val);
}

sw_error_t
hppe_rfc_block_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfc_block_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rfc_status_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfc_status_tbl_u *value)
{
	if (index >= RFC_STATUS_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + RFC_STATUS_TBL_ADDRESS + \
				index * RFC_STATUS_TBL_INC,
				&value->val);
}

sw_error_t
hppe_rfc_status_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rfc_status_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_deq_dis_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union deq_dis_tbl_u *value)
{
	if (index >= DEQ_DIS_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + DEQ_DIS_TBL_ADDRESS + \
				index * DEQ_DIS_TBL_INC,
				&value->val);
}

sw_error_t
hppe_deq_dis_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union deq_dis_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + DEQ_DIS_TBL_ADDRESS + \
				index * DEQ_DIS_TBL_INC,
				value->val);
}

sw_error_t
hppe_l1_flow_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_map_tbl_u *value)
{
	if (index >= L1_FLOW_MAP_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_FLOW_MAP_TBL_ADDRESS + \
				index * L1_FLOW_MAP_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_flow_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_map_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_FLOW_MAP_TBL_ADDRESS + \
				index * L1_FLOW_MAP_TBL_INC,
				value->val);
}

sw_error_t
hppe_l1_c_sp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_sp_cfg_tbl_u *value)
{
	if (index >= L1_C_SP_CFG_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_C_SP_CFG_TBL_ADDRESS + \
				index * L1_C_SP_CFG_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_c_sp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_sp_cfg_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_C_SP_CFG_TBL_ADDRESS + \
				index * L1_C_SP_CFG_TBL_INC,
				value->val);
}

sw_error_t
hppe_l1_e_sp_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_sp_cfg_tbl_u *value)
{
	if (index >= L1_E_SP_CFG_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_E_SP_CFG_TBL_ADDRESS + \
				index * L1_E_SP_CFG_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_e_sp_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_sp_cfg_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_E_SP_CFG_TBL_ADDRESS + \
				index * L1_E_SP_CFG_TBL_INC,
				value->val);
}

sw_error_t
hppe_l1_flow_port_map_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_port_map_tbl_u *value)
{
	if (index >= L1_FLOW_PORT_MAP_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_FLOW_PORT_MAP_TBL_ADDRESS + \
				index * L1_FLOW_PORT_MAP_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_flow_port_map_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_port_map_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_FLOW_PORT_MAP_TBL_ADDRESS + \
				index * L1_FLOW_PORT_MAP_TBL_INC,
				value->val);
}

sw_error_t
hppe_l1_c_drr_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_head_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_C_DRR_HEAD_TBL_ADDRESS + \
				index * L1_C_DRR_HEAD_TBL_INC,
				value->val,
				2);
}
#ifndef IN_QOS_MINI
sw_error_t
hppe_l1_c_drr_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_head_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_head_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_E_DRR_HEAD_TBL_ADDRESS + \
				index * L1_E_DRR_HEAD_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_l1_e_drr_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_head_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_drr_credit_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_drr_credit_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_DRR_CREDIT_TBL_ADDRESS + \
				index * L1_DRR_CREDIT_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_l1_drr_credit_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_drr_credit_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_ll_tbl_u *value)
{
	if (index >= L1_C_DRR_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_C_DRR_LL_TBL_ADDRESS + \
				index * L1_C_DRR_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_c_drr_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_reverse_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_reverse_ll_tbl_u *value)
{
	if (index >= L1_C_DRR_REVERSE_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_C_DRR_REVERSE_LL_TBL_ADDRESS + \
				index * L1_C_DRR_REVERSE_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_c_drr_reverse_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_c_drr_reverse_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_ll_tbl_u *value)
{
	if (index >= L1_E_DRR_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_E_DRR_LL_TBL_ADDRESS + \
				index * L1_E_DRR_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_e_drr_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_reverse_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_reverse_ll_tbl_u *value)
{
	if (index >= L1_E_DRR_REVERSE_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_E_DRR_REVERSE_LL_TBL_ADDRESS + \
				index * L1_E_DRR_REVERSE_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_e_drr_reverse_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_e_drr_reverse_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_a_flow_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_a_flow_entry_tbl_u *value)
{
	if (index >= L1_A_FLOW_ENTRY_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_A_FLOW_ENTRY_TBL_ADDRESS + \
				index * L1_A_FLOW_ENTRY_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_a_flow_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_a_flow_entry_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_b_flow_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_b_flow_entry_tbl_u *value)
{
	if (index >= L1_B_FLOW_ENTRY_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_B_FLOW_ENTRY_TBL_ADDRESS + \
				index * L1_B_FLOW_ENTRY_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_b_flow_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_b_flow_entry_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_sp_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_sp_entry_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_SP_ENTRY_TBL_ADDRESS + \
				index * L1_SP_ENTRY_TBL_INC,
				value->val,
				9);
}

sw_error_t
hppe_l1_sp_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_sp_entry_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_ll_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_ll_tbl_u *value)
{
	if (index >= L1_ENS_Q_LL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_ENS_Q_LL_TBL_ADDRESS + \
				index * L1_ENS_Q_LL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_ens_q_ll_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_ll_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_head_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_head_tbl_u *value)
{
	if (index >= L1_ENS_Q_HEAD_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_ENS_Q_HEAD_TBL_ADDRESS + \
				index * L1_ENS_Q_HEAD_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_ens_q_head_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_head_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_entry_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_entry_tbl_u *value)
{
	if (index >= L1_ENS_Q_ENTRY_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_ENS_Q_ENTRY_TBL_ADDRESS + \
				index * L1_ENS_Q_ENTRY_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_ens_q_entry_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_ens_q_entry_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_flow_status_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_status_tbl_u *value)
{
	if (index >= L1_FLOW_STATUS_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + L1_FLOW_STATUS_TBL_ADDRESS + \
				index * L1_FLOW_STATUS_TBL_INC,
				&value->val);
}

sw_error_t
hppe_l1_flow_status_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union l1_flow_status_tbl_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_psch_tdm_cfg_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_tdm_cfg_tbl_u *value)
{
	if (index >= PSCH_TDM_CFG_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_TDM_CFG_TBL_ADDRESS + \
				index * PSCH_TDM_CFG_TBL_INC,
				&value->val);
}
#endif
sw_error_t
hppe_psch_tdm_cfg_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union psch_tdm_cfg_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				TRAFFIC_MANAGER_BASE_ADDR + PSCH_TDM_CFG_TBL_ADDRESS + \
				index * PSCH_TDM_CFG_TBL_INC,
				value->val);
}
#ifndef IN_QOS_MINI
sw_error_t
hppe_tdm_depth_cfg_tdm_depth_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tdm_depth_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_depth_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.tdm_depth;
	return ret;
}

sw_error_t
hppe_tdm_depth_cfg_tdm_depth_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union tdm_depth_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_depth_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tdm_depth = value;
	ret = hppe_tdm_depth_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_min_max_mode_cfg_min_max_mode_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union min_max_mode_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_min_max_mode_cfg_get(dev_id, &reg_val);
	*value = reg_val.bf.min_max_mode;
	return ret;
}

sw_error_t
hppe_min_max_mode_cfg_min_max_mode_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union min_max_mode_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_min_max_mode_cfg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.min_max_mode = value;
	ret = hppe_min_max_mode_cfg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_tm_dbg_addr_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tm_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tm_dbg_addr_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_addr;
	return ret;
}

sw_error_t
hppe_tm_dbg_addr_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union tm_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tm_dbg_addr_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dbg_addr = value;
	ret = hppe_tm_dbg_addr_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_tm_dbg_data_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tm_dbg_data_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tm_dbg_data_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_data;
	return ret;
}

sw_error_t
hppe_tm_dbg_data_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_eco_reserve_0_eco_res_0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union eco_reserve_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eco_reserve_0_get(dev_id, &reg_val);
	*value = reg_val.bf.eco_res_0;
	return ret;
}

sw_error_t
hppe_eco_reserve_0_eco_res_0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union eco_reserve_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eco_reserve_0_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eco_res_0 = value;
	ret = hppe_eco_reserve_0_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_eco_reserve_1_eco_res_1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union eco_reserve_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eco_reserve_1_get(dev_id, &reg_val);
	*value = reg_val.bf.eco_res_1;
	return ret;
}

sw_error_t
hppe_eco_reserve_1_eco_res_1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union eco_reserve_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_eco_reserve_1_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eco_res_1 = value;
	ret = hppe_eco_reserve_1_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_e_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_pri;
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_e_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_pri = value;
	ret = hppe_l0_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_c_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_pri;
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_c_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_pri = value;
	ret = hppe_l0_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_e_drr_wt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_wt;
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_e_drr_wt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_drr_wt = value;
	ret = hppe_l0_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_c_drr_wt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_wt;
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_c_drr_wt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_drr_wt = value;
	ret = hppe_l0_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_sp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.sp_id;
	return ret;
}

sw_error_t
hppe_l0_flow_map_tbl_sp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.sp_id = value;
	ret = hppe_l0_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_c_sp_cfg_tbl_drr_credit_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_sp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_credit_unit;
	return ret;
}

sw_error_t
hppe_l0_c_sp_cfg_tbl_drr_credit_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_c_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_sp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_credit_unit = value;
	ret = hppe_l0_c_sp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_c_sp_cfg_tbl_drr_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_sp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_id;
	return ret;
}

sw_error_t
hppe_l0_c_sp_cfg_tbl_drr_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_c_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_sp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_id = value;
	ret = hppe_l0_c_sp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_e_sp_cfg_tbl_drr_credit_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_sp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_credit_unit;
	return ret;
}

sw_error_t
hppe_l0_e_sp_cfg_tbl_drr_credit_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_e_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_sp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_credit_unit = value;
	ret = hppe_l0_e_sp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_e_sp_cfg_tbl_drr_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_sp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_id;
	return ret;
}

sw_error_t
hppe_l0_e_sp_cfg_tbl_drr_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_e_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_sp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_id = value;
	ret = hppe_l0_e_sp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_flow_port_map_tbl_port_num_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_flow_port_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_port_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_num;
	return ret;
}

sw_error_t
hppe_l0_flow_port_map_tbl_port_num_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l0_flow_port_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_port_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_num = value;
	ret = hppe_l0_flow_port_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l0_c_drr_head_tbl_backup_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_head;
	return ret;
}

sw_error_t
hppe_l0_c_drr_head_tbl_backup_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_head_tbl_active_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_vld;
	return ret;
}

sw_error_t
hppe_l0_c_drr_head_tbl_active_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_head_tbl_backup_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_max_n;
	return ret;
}

sw_error_t
hppe_l0_c_drr_head_tbl_backup_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_head_tbl_active_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_tail_1 << 8 | \
		reg_val.bf.active_tail_0;
	return ret;
}

sw_error_t
hppe_l0_c_drr_head_tbl_active_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_head_tbl_active_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_head;
	return ret;
}

sw_error_t
hppe_l0_c_drr_head_tbl_active_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_head_tbl_backup_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_vld;
	return ret;
}

sw_error_t
hppe_l0_c_drr_head_tbl_backup_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_head_tbl_active_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_max_n;
	return ret;
}

sw_error_t
hppe_l0_c_drr_head_tbl_active_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_head_tbl_backup_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_tail;
	return ret;
}

sw_error_t
hppe_l0_c_drr_head_tbl_backup_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_head_tbl_backup_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_head;
	return ret;
}

sw_error_t
hppe_l0_e_drr_head_tbl_backup_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_head_tbl_active_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_vld;
	return ret;
}

sw_error_t
hppe_l0_e_drr_head_tbl_active_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_head_tbl_backup_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_max_n;
	return ret;
}

sw_error_t
hppe_l0_e_drr_head_tbl_backup_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_head_tbl_active_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_tail_1 << 8 | \
		reg_val.bf.active_tail_0;
	return ret;
}

sw_error_t
hppe_l0_e_drr_head_tbl_active_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_head_tbl_active_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_head;
	return ret;
}

sw_error_t
hppe_l0_e_drr_head_tbl_active_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_head_tbl_backup_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_vld;
	return ret;
}

sw_error_t
hppe_l0_e_drr_head_tbl_backup_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_head_tbl_active_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_max_n;
	return ret;
}

sw_error_t
hppe_l0_e_drr_head_tbl_active_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_head_tbl_backup_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_tail;
	return ret;
}

sw_error_t
hppe_l0_e_drr_head_tbl_backup_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_drr_credit_tbl_e_drr_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_drr_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_drr_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_credit_neg;
	return ret;
}

sw_error_t
hppe_l0_drr_credit_tbl_e_drr_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_drr_credit_tbl_c_drr_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_drr_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_drr_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_credit_neg;
	return ret;
}

sw_error_t
hppe_l0_drr_credit_tbl_c_drr_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_drr_credit_tbl_c_drr_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_drr_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_drr_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_credit;
	return ret;
}

sw_error_t
hppe_l0_drr_credit_tbl_c_drr_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_drr_credit_tbl_e_drr_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_drr_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_drr_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_credit_1 << 7 | \
		reg_val.bf.e_drr_credit_0;
	return ret;
}

sw_error_t
hppe_l0_drr_credit_tbl_e_drr_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_ptr;
	return ret;
}

sw_error_t
hppe_l0_c_drr_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_c_drr_reverse_ll_tbl_pre_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_c_drr_reverse_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_c_drr_reverse_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pre_ptr;
	return ret;
}

sw_error_t
hppe_l0_c_drr_reverse_ll_tbl_pre_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_ptr;
	return ret;
}

sw_error_t
hppe_l0_e_drr_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_e_drr_reverse_ll_tbl_pre_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_e_drr_reverse_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_e_drr_reverse_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pre_ptr;
	return ret;
}

sw_error_t
hppe_l0_e_drr_reverse_ll_tbl_pre_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_sp_entry_tbl_entry_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_sp_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_sp_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_vld;
	return ret;
}

sw_error_t
hppe_l0_sp_entry_tbl_entry_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_sp_entry_tbl_entry_path_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union l0_sp_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_sp_entry_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.entry_path_id_1 << 32 | \
		reg_val.bf.entry_path_id_0;
	return ret;
}

sw_error_t
hppe_l0_sp_entry_tbl_entry_path_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_ens_q_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_ens_q_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_ptr;
	return ret;
}

sw_error_t
hppe_l0_ens_q_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_head_tbl_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_ens_q_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_ens_q_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vld;
	return ret;
}

sw_error_t
hppe_l0_ens_q_head_tbl_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_head_tbl_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_ens_q_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_ens_q_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.head;
	return ret;
}

sw_error_t
hppe_l0_ens_q_head_tbl_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_head_tbl_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_ens_q_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_ens_q_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tail;
	return ret;
}

sw_error_t
hppe_l0_ens_q_head_tbl_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_in_q_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_ens_q_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_ens_q_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_ens_in_q;
	return ret;
}

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_in_q_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_ens_q_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_ens_q_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_ens_vld;
	return ret;
}

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_ens_q_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_ens_q_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_ens_type;
	return ret;
}

sw_error_t
hppe_l0_ens_q_entry_tbl_entry_ens_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_flow_status_tbl_en_cdrr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_flow_status_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_status_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.en_cdrr;
	return ret;
}

sw_error_t
hppe_l0_flow_status_tbl_en_cdrr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_flow_status_tbl_en_edrr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_flow_status_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_status_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.en_edrr;
	return ret;
}

sw_error_t
hppe_l0_flow_status_tbl_en_edrr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l0_flow_status_tbl_en_level_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l0_flow_status_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l0_flow_status_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.en_level;
	return ret;
}

sw_error_t
hppe_l0_flow_status_tbl_en_level_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_ring_q_map_tbl_queue_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union ring_q_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ring_q_map_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.queue_bitmap_1 << 32 | \
		reg_val.bf.queue_bitmap_0;
	return ret;
}

sw_error_t
hppe_ring_q_map_tbl_queue_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union ring_q_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ring_q_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.queue_bitmap_1 = value >> 32;
	reg_val.bf.queue_bitmap_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_ring_q_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rfc_block_tbl_rfc_block_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rfc_block_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rfc_block_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rfc_block;
	return ret;
}

sw_error_t
hppe_rfc_block_tbl_rfc_block_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rfc_status_tbl_rfc_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rfc_status_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rfc_status_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rfc_status;
	return ret;
}

sw_error_t
hppe_rfc_status_tbl_rfc_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_deq_dis_tbl_deq_dis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union deq_dis_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_deq_dis_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.deq_dis;
	return ret;
}

sw_error_t
hppe_deq_dis_tbl_deq_dis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union deq_dis_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_deq_dis_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.deq_dis = value;
	ret = hppe_deq_dis_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_e_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_pri;
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_e_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_pri = value;
	ret = hppe_l1_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_c_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_pri;
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_c_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_pri = value;
	ret = hppe_l1_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_e_drr_wt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_wt;
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_e_drr_wt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.e_drr_wt = value;
	ret = hppe_l1_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_c_drr_wt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_wt;
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_c_drr_wt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.c_drr_wt = value;
	ret = hppe_l1_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_sp_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.sp_id;
	return ret;
}

sw_error_t
hppe_l1_flow_map_tbl_sp_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_flow_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.sp_id = value;
	ret = hppe_l1_flow_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_c_sp_cfg_tbl_drr_credit_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_sp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_credit_unit;
	return ret;
}

sw_error_t
hppe_l1_c_sp_cfg_tbl_drr_credit_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_c_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_sp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_credit_unit = value;
	ret = hppe_l1_c_sp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_c_sp_cfg_tbl_drr_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_sp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_id;
	return ret;
}

sw_error_t
hppe_l1_c_sp_cfg_tbl_drr_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_c_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_sp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_id = value;
	ret = hppe_l1_c_sp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_e_sp_cfg_tbl_drr_credit_unit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_sp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_credit_unit;
	return ret;
}

sw_error_t
hppe_l1_e_sp_cfg_tbl_drr_credit_unit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_e_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_sp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_credit_unit = value;
	ret = hppe_l1_e_sp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_e_sp_cfg_tbl_drr_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_sp_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drr_id;
	return ret;
}

sw_error_t
hppe_l1_e_sp_cfg_tbl_drr_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_e_sp_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_sp_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drr_id = value;
	ret = hppe_l1_e_sp_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_flow_port_map_tbl_port_num_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_flow_port_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_port_map_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_num;
	return ret;
}

sw_error_t
hppe_l1_flow_port_map_tbl_port_num_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union l1_flow_port_map_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_port_map_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_num = value;
	ret = hppe_l1_flow_port_map_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_l1_c_drr_head_tbl_backup_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_head;
	return ret;
}

sw_error_t
hppe_l1_c_drr_head_tbl_backup_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_head_tbl_active_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_vld;
	return ret;
}

sw_error_t
hppe_l1_c_drr_head_tbl_active_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_head_tbl_backup_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_max_n;
	return ret;
}

sw_error_t
hppe_l1_c_drr_head_tbl_backup_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_head_tbl_active_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_tail;
	return ret;
}

sw_error_t
hppe_l1_c_drr_head_tbl_active_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_head_tbl_active_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_head;
	return ret;
}

sw_error_t
hppe_l1_c_drr_head_tbl_active_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_head_tbl_backup_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_vld;
	return ret;
}

sw_error_t
hppe_l1_c_drr_head_tbl_backup_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_head_tbl_active_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_max_n_1 << 1 | \
		reg_val.bf.active_max_n_0;
	return ret;
}

sw_error_t
hppe_l1_c_drr_head_tbl_active_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_head_tbl_backup_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_tail;
	return ret;
}

sw_error_t
hppe_l1_c_drr_head_tbl_backup_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_head_tbl_backup_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_head;
	return ret;
}

sw_error_t
hppe_l1_e_drr_head_tbl_backup_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_head_tbl_active_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_vld;
	return ret;
}

sw_error_t
hppe_l1_e_drr_head_tbl_active_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_head_tbl_backup_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_max_n;
	return ret;
}

sw_error_t
hppe_l1_e_drr_head_tbl_backup_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_head_tbl_active_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_tail;
	return ret;
}

sw_error_t
hppe_l1_e_drr_head_tbl_active_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_head_tbl_active_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_head;
	return ret;
}

sw_error_t
hppe_l1_e_drr_head_tbl_active_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_head_tbl_backup_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_vld;
	return ret;
}

sw_error_t
hppe_l1_e_drr_head_tbl_backup_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_head_tbl_active_max_n_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.active_max_n_1 << 1 | \
		reg_val.bf.active_max_n_0;
	return ret;
}

sw_error_t
hppe_l1_e_drr_head_tbl_active_max_n_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_head_tbl_backup_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.backup_tail;
	return ret;
}

sw_error_t
hppe_l1_e_drr_head_tbl_backup_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_drr_credit_tbl_e_drr_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_drr_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_drr_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_credit_neg;
	return ret;
}

sw_error_t
hppe_l1_drr_credit_tbl_e_drr_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_drr_credit_tbl_c_drr_credit_neg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_drr_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_drr_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_credit_neg;
	return ret;
}

sw_error_t
hppe_l1_drr_credit_tbl_c_drr_credit_neg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_drr_credit_tbl_c_drr_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_drr_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_drr_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.c_drr_credit;
	return ret;
}

sw_error_t
hppe_l1_drr_credit_tbl_c_drr_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_drr_credit_tbl_e_drr_credit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_drr_credit_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_drr_credit_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.e_drr_credit_1 << 7 | \
		reg_val.bf.e_drr_credit_0;
	return ret;
}

sw_error_t
hppe_l1_drr_credit_tbl_e_drr_credit_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_ptr;
	return ret;
}

sw_error_t
hppe_l1_c_drr_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_c_drr_reverse_ll_tbl_pre_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_c_drr_reverse_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_c_drr_reverse_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pre_ptr;
	return ret;
}

sw_error_t
hppe_l1_c_drr_reverse_ll_tbl_pre_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_ptr;
	return ret;
}

sw_error_t
hppe_l1_e_drr_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_e_drr_reverse_ll_tbl_pre_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_e_drr_reverse_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_e_drr_reverse_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pre_ptr;
	return ret;
}

sw_error_t
hppe_l1_e_drr_reverse_ll_tbl_pre_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_a_flow_entry_tbl_entry_path_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_a_flow_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_a_flow_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_path_id;
	return ret;
}

sw_error_t
hppe_l1_a_flow_entry_tbl_entry_path_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_b_flow_entry_tbl_entry_path_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_b_flow_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_b_flow_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_path_id;
	return ret;
}

sw_error_t
hppe_l1_b_flow_entry_tbl_entry_path_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_sp_entry_tbl_entry_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_sp_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_sp_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_vld;
	return ret;
}

sw_error_t
hppe_l1_sp_entry_tbl_entry_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_sp_entry_tbl_entry_path_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union l1_sp_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_sp_entry_tbl_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.entry_path_id_1 << 32 | \
		reg_val.bf.entry_path_id_0;
	return ret;
}

sw_error_t
hppe_l1_sp_entry_tbl_entry_path_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_ll_tbl_next_ptr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_ens_q_ll_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_ens_q_ll_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.next_ptr;
	return ret;
}

sw_error_t
hppe_l1_ens_q_ll_tbl_next_ptr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_head_tbl_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_ens_q_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_ens_q_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vld;
	return ret;
}

sw_error_t
hppe_l1_ens_q_head_tbl_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_head_tbl_head_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_ens_q_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_ens_q_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.head;
	return ret;
}

sw_error_t
hppe_l1_ens_q_head_tbl_head_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_head_tbl_tail_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_ens_q_head_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_ens_q_head_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tail;
	return ret;
}

sw_error_t
hppe_l1_ens_q_head_tbl_tail_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_in_q_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_ens_q_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_ens_q_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_ens_in_q;
	return ret;
}

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_in_q_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_vld_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_ens_q_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_ens_q_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_ens_vld;
	return ret;
}

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_vld_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_ens_q_entry_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_ens_q_entry_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.entry_ens_type;
	return ret;
}

sw_error_t
hppe_l1_ens_q_entry_tbl_entry_ens_type_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_flow_status_tbl_en_cdrr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_flow_status_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_status_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.en_cdrr;
	return ret;
}

sw_error_t
hppe_l1_flow_status_tbl_en_cdrr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_flow_status_tbl_en_edrr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_flow_status_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_status_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.en_edrr;
	return ret;
}

sw_error_t
hppe_l1_flow_status_tbl_en_edrr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_l1_flow_status_tbl_en_level_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union l1_flow_status_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_l1_flow_status_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.en_level;
	return ret;
}

sw_error_t
hppe_l1_flow_status_tbl_en_level_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_psch_tdm_cfg_tbl_ens_port_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_tdm_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_tdm_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ens_port;
	return ret;
}

sw_error_t
hppe_psch_tdm_cfg_tbl_ens_port_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_tdm_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_tdm_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ens_port = value;
	ret = hppe_psch_tdm_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_tdm_cfg_tbl_des_port_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_tdm_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_tdm_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.des_port;
	return ret;
}

sw_error_t
hppe_psch_tdm_cfg_tbl_des_port_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_tdm_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_tdm_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.des_port = value;
	ret = hppe_psch_tdm_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_psch_tdm_cfg_tbl_ens_port_bitmap_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union psch_tdm_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_tdm_cfg_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ens_port_bitmap;
	return ret;
}

sw_error_t
hppe_psch_tdm_cfg_tbl_ens_port_bitmap_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union psch_tdm_cfg_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_psch_tdm_cfg_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ens_port_bitmap = value;
	ret = hppe_psch_tdm_cfg_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_flow_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_flow_qos_pri;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_flow_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_flow_qos_pri = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_dscp_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_dscp_qos_pri;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_dscp_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_dscp_qos_pri = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_dscp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_dscp_change_en;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_dscp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_dscp_change_en = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_pcp_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_pcp_change_en;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_pcp_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_pcp_change_en = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_acl_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_acl_qos_pri;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_acl_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_acl_qos_pri = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_flow_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.flow_qos_group_id;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_flow_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flow_qos_group_id = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_preheader_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_preheader_qos_pri;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_preheader_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_preheader_qos_pri = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_pcp_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pcp_qos_group_id;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_pcp_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pcp_qos_group_id = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_dscp_qos_group_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dscp_qos_group_id;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_dscp_qos_group_id_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dscp_qos_group_id = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_pcp_qos_pri_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_pcp_qos_pri;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_pcp_qos_pri_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_pcp_qos_pri = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_dei_change_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_dei_change_en;
	return ret;
}

sw_error_t
hppe_port_qos_ctrl_port_dei_change_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_qos_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_qos_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_dei_change_en = value;
	ret = hppe_port_qos_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pcp_qos_group_0_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pcp_qos_group_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pcp_qos_group_0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qos_info;
	return ret;
}

sw_error_t
hppe_pcp_qos_group_0_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pcp_qos_group_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pcp_qos_group_0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qos_info = value;
	ret = hppe_pcp_qos_group_0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_pcp_qos_group_1_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union pcp_qos_group_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pcp_qos_group_1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qos_info;
	return ret;
}

sw_error_t
hppe_pcp_qos_group_1_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union pcp_qos_group_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_pcp_qos_group_1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qos_info = value;
	ret = hppe_pcp_qos_group_1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_flow_qos_group_0_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union flow_qos_group_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flow_qos_group_0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qos_info;
	return ret;
}

sw_error_t
hppe_flow_qos_group_0_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union flow_qos_group_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flow_qos_group_0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qos_info = value;
	ret = hppe_flow_qos_group_0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_flow_qos_group_1_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union flow_qos_group_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flow_qos_group_1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qos_info;
	return ret;
}

sw_error_t
hppe_flow_qos_group_1_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union flow_qos_group_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_flow_qos_group_1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qos_info = value;
	ret = hppe_flow_qos_group_1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_dscp_qos_group_0_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union dscp_qos_group_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dscp_qos_group_0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qos_info;
	return ret;
}

sw_error_t
hppe_dscp_qos_group_0_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union dscp_qos_group_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dscp_qos_group_0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qos_info = value;
	ret = hppe_dscp_qos_group_0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_dscp_qos_group_1_qos_info_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union dscp_qos_group_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dscp_qos_group_1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.qos_info;
	return ret;
}

sw_error_t
hppe_dscp_qos_group_1_qos_info_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union dscp_qos_group_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dscp_qos_group_1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qos_info = value;
	ret = hppe_dscp_qos_group_1_set(dev_id, index, &reg_val);
	return ret;
}
#endif